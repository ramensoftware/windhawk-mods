// ==WindhawkMod==
// @id              taskbar-quick-pin
// @name            Left Taskbar Quick Pin Dock
// @description     A persistent icon dock anchored left of the Start button. Drag any app to pin it. Left-click to launch or focus. Double-right-click to unpin. Drag within the dock to reorder.
// @version         1.0.0
// @author          Ashix
// @github          https://github.com/k-ashix/
// @twitter         https://x.com/k_ashix
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lpsapi -lshell32 -lole32 -loleaut32 -luuid -lshlwapi -lgdi32 -lmsimg32 -luiautomationcore
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Left Taskbar Quick Pin Dock

A lightweight, always-visible icon dock anchored just left of the Start button
on the Windows 11 taskbar. Pin any app by dragging it onto the dock; click to
launch or focus; double-right-click to unpin.

## Usage

| Action | Result |
|---|---|
| Drag any running app window onto the dock | Pin it |
| Drag a pinned icon left or right within the dock | Reorder it |
| Left-click a pinned icon | Launch or bring to front |
| Double-right-click a pinned icon | Unpin it |
| Drag a pinned icon off the dock | Unpin it |
| 3 rapid left-clicks on any icon or nearby dock area | Unpin all |
| Configurable hotkey (default Ctrl+Alt+P) | Pin or unpin the focused app |

## How it works

The dock is a layered, click-through overlay window positioned immediately left
of the Start button. It tracks the Start button position through Windows
accessibility events so it stays anchored correctly across taskbar moves,
resolution changes, and DPI switches.

### App identity

When you press the mouse button to begin a drag, the mod runs a three-layer
resolver to identify the app under the cursor:

1. **UI Automation hit-test**  --  asks Windows which taskbar button is at the
   cursor. This handles UWP apps, grouped windows, and most modern apps.
2. **Process path extraction**  --  falls back to the foreground window's process
   executable path when the taskbar hit-test returns nothing.
3. **Running-window scan**  --  last resort; searches all visible windows for a
   match.

To prevent mis-pins, app identity is pre-sampled every 80 ms while the cursor
hovers near the taskbar, so the first click is always resolved from a stable,
confirmed sample rather than cold at press time.

### Drag safety

- System processes (Explorer, Search, shell hosts, credential dialog) are
  blocked from pinning automatically.
- Apps without extractable icons cannot be pinned  --  no placeholder icons appear.
- The active window is **never** used as a drag source. Identity is always
  cursor-bound.

### Persistence

Pins are saved to the registry under
`HKCU\Software\WindhawkMods\taskbar-quick-pin`. Registry writes happen off the
animation lock so pin and unpin operations never stall the dock animation.

## Settings

All settings are accessible in the Windhawk settings panel. Each one has a
description shown alongside the control  --  refer to the settings panel for
details.

**Hotkey tip:** The modifier and key dropdowns work together. Select modifiers
first (e.g. Ctrl + Alt), then select the key (e.g. P). To disable the hotkey,
set modifiers to **Disabled**.

## Known Limitations

- UWP apps hosted by ApplicationFrameHost may resolve to the host path on rare
  occasions when the inner app window is not yet visible.
- Double-right-click on the overlay passes through to the taskbar when the
  cursor is not directly over a pinned icon.
- Multi-monitor dock mirrors are read-only  --  pinning and unpinning is only
  supported on the primary monitor's dock.
- A small number of apps (typically custom-icon-handler games or launchers)
  may still show a generic shell icon despite the 5-attempt icon resolver.
  This is a shell limitation; no Win32 API reliably resolves every icon.

## Tip

The dock takes a moment on first load while it detects the taskbar geometry. If
it doesn't appear immediately, try increasing the **Startup delay** setting.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxPinnedApps: 5
  $name: Max pinned apps
  $description: >-
    Maximum number of apps that can be pinned (1 - 10).
    The dock width is always sized for this many slots  --  it does not shrink
    when slots are empty.

- iconSize: 28
  $name: Icon size (px at 96 DPI)
  $description: >-
    Base icon size in pixels before DPI scaling (16 - 48).
    At 150 % display scaling a value of 28 renders at roughly 42 px.

- iconSpacing: 9
  $name: Icon spacing (px at 96 DPI)
  $description: >-
    Gap between icons in pixels before DPI scaling (2 - 24).

- separatorOpacity: 100
  $name: Separator opacity
  $description: >-
    Visibility of the right-edge separator line that divides the dock from the
    rest of the taskbar (0 = hidden, 100 = fully visible).

- enableGlassOverlay: true
  $name: Glass overlay
  $description: >-
    Draws a subtle gradient tint behind the dock for a frosted-glass look.
    Also upgrades pin/unpin feedback from a flat separator flash to a
    full-dock colour shift.

- enableReorder: true
  $name: Drag to reorder
  $description: >-
    Drag a pinned icon left or right within the dock to rearrange it.
    When disabled, dragging a dock icon off the dock edge unpins it instead.

- multiMonitorDock: false
  $name: Multi-monitor dock
  $description: >-
    Show a mirrored, read-only dock on each secondary monitor's taskbar.
    Only the primary dock supports pinning and unpinning.
    Requires a mod reload to take effect after toggling.

- startupDelay: 0
  $name: Startup delay (ms)
  $description: >-
    Extra delay in milliseconds before the dock initialises (0 - 3000).
    Increase this on machines where the taskbar takes several seconds to
    fully render after login and the dock appears in the wrong position.

- autoHideSync: false
  $name: Sync with taskbar auto-hide
  $description: >-
    When enabled, the dock hides alongside the taskbar whenever the taskbar's
    auto-hide animation slides it off screen.
    Disabled by default  --  the dock stays visible at all times.

- hotkeyModifiers: 3
  $name: Pin hotkey  --  modifiers
  $description: >-
    Modifier keys to hold when pressing the hotkey key to pin or unpin the
    focused application. Set to "Disabled" to turn the hotkey off entirely.
  $options:
  - 0: Disabled (no hotkey)
  - 1: Alt
  - 2: Ctrl
  - 3: Ctrl + Alt
  - 4: Shift
  - 5: Ctrl + Shift
  - 6: Alt + Shift
  - 8: Win
  - 9: Win + Alt
  - 10: Win + Ctrl

- hotkeyKey: 80
  $name: Pin hotkey  --  key
  $description: >-
    The key to press together with the modifiers above to pin or unpin the
    focused app. Has no effect if modifiers is set to "Disabled".
  $options:
  - 0: (none  --  disabled)
  - 65: A
  - 66: B
  - 67: C
  - 68: D
  - 69: E
  - 70: F
  - 71: G
  - 72: H
  - 73: I
  - 74: J
  - 75: K
  - 76: L
  - 77: M
  - 78: N
  - 79: O
  - 80: P
  - 81: Q
  - 82: R
  - 83: S
  - 84: T
  - 85: U
  - 86: V
  - 87: W
  - 88: X
  - 89: Y
  - 90: Z
  - 112: F1
  - 113: F2
  - 114: F3
  - 115: F4
  - 116: F5
  - 117: F6
  - 118: F7
  - 119: F8
  - 120: F9
  - 121: F10
  - 122: F11
  - 123: F12
*/
// ==/WindhawkModSettings==

// ============================================================
//  INCLUDES
// ============================================================
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <initguid.h>
#include <commoncontrols.h>
#include <uiautomation.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

// ============================================================
//  CONFIGURATION  --  base values at 96 DPI, scaled at runtime
// ============================================================
static const int   DRAG_THRESHOLD_PX      = 6;     // Pixels before drag is confirmed
static const int   BASE_DOCK_PAD_LEFT     = 6;     // Left internal padding
static const int   BASE_DOCK_PAD_RIGHT    = 8;     // Right internal padding
static const int   BASE_MAGNETIC_RANGE    = 60;    // Ghost visibility radius beyond dock
static const int   BASE_GHOST_SIZE        = 40;    // Drag ghost size in pixels
static const int   CLICK_MAX_MS           = 300;   // Max duration for a click (not a drag)
static const int   CLICK_MAX_MOVE_PX      = 5;     // Max cursor movement for a click
static const int   MIN_VALID_DOCK_WIDTH   = 80;    // Minimum acceptable dock pixel width
static const int   BOOT_PHASE_MS          = 600;   // Boot stabilisation timeout (dynamic exit preferred)
static const int   DOCK_SAFE_GAP          = 6;     // Gap between dock right edge and Start left edge
static const int   RAPID_CLICK_THRESHOLD  = 3;     // Clicks to trigger unpin-all (reduced from 5 for usability)
static const int   RAPID_CLICK_WINDOW_MS  = 1000;  // Window for rapid-click detection (ms)
static const int   RIGHT_DOUBLE_CLICK_MS  = 350;   // Right double-click unpin threshold
static const int   MAGNETIC_ZONE_EXPAND_PX = 8;    // Expand magnetic zone up/down
static const int   POS_LOCK_THRESHOLD     = 20;    // Ignore dock position changes < this px
static const int   POS_RESET_THRESHOLD    = 100;   // Full lock reset if change > this px
static const int   RUNNING_STATE_CHECK_MS = 500;   // Period between running-app window scans
static bool        ENABLE_AUTOHIDE_SYNC   = false; // Sync dock visibility with taskbar auto-hide
static const int   HOVER_SAMPLE_MS        = 80;    // UIA call rate limit during idle hover tracking
static const int   STABILITY_CONFIRM_MS   = 150;   // Min ms a candidate must be stable before use
static const int   SHAKE_DURATION_MS      = 400;   // Duration of limit-hit shake animation
static const float SHAKE_AMPLITUDE_PX     = 3.f;   // Peak pixel displacement during shake

static const float ANIM_LERP_FACTOR       = 0.24f; // Ease-out lerp  --  tuned for Windows-native feel
static const float ANIM_MOMENTUM_DECAY    = 0.68f; // Strong damping  --  crisp, no overshoot
static const int   ANIM_POS_THRESHOLD     = 2;     // px: ignore dock position micro-changes
static const float ANIM_SNAP_THRESHOLD    = 0.6f;  // px: snap sooner for responsiveness
static const float HOVER_SCALE_FACTOR     = 1.10f; // Icon scale on hover (10%  --  matches Win11)
static const float HOVER_SCALE_IN_SPEED   = 0.35f; // Scale-up speed (fast  --  instant feel)
static const float HOVER_SCALE_OUT_SPEED  = 0.20f; // Scale-down speed (slightly slower  --  graceful)
static const float PIN_FADE_SPEED         = 0.11f; // ~90 ms fade-in  --  fast, native-feeling
static const float PIN_SLIDE_OFFSET       = 10.f;  // Entrance slide in pixels (more visible)
static const float ICON_ANIM_SPEED        = 0.26f; // Icon X position lerp speed
static const float FRAME_DELTA_SNAP_MS    = 50.f;  // Snap animations on frames slower than this

// User-configurable (clamped in Wh_ModInit)
static int  MAX_PINNED_APPS      = 5;
static int  BASE_ICON_SIZE       = 28;
static int  BASE_ICON_SPACING    = 9;
static int  SEPARATOR_OPACITY    = 100;
static bool ENABLE_GLASS_OVERLAY = true;   // Premium glass gradient behind dock
static bool ENABLE_REORDER       = true;   // Drag-to-reorder within dock
static bool MULTI_MONITOR_DOCK   = false;  // Secondary-monitor mirror docks
static int  STARTUP_DELAY_MS     = 0;      // Extra init delay (slow machines)

// DPI-scaled values  --  set by RefreshDpiScale(), never set manually
static int ICON_SIZE        = 28;
static int ICON_SPACING     = 9;
static int DOCK_PAD_LEFT    = 6;
static int DOCK_PAD_RIGHT   = 8;
static int MAGNETIC_RANGE_PX = 60;
static int GHOST_SIZE       = 40;

// ============================================================
//  REGISTRY / WINDOW CLASS NAMES
// ============================================================
static const wchar_t* REG_KEY       = L"Software\\WindhawkMods\\TaskbarQuickPin";
static const wchar_t* REG_VALUE     = L"PinnedApps";
static const wchar_t* OVERLAY_CLASS = L"QPDockOverlay";
static const wchar_t* GHOST_CLASS   = L"QPDockGhost";

// ============================================================
//  DRAG STATE MACHINE
//  Transitions: IDLE -> PRESS -> DRAGGING -> DROPPED -> IDLE
//                                        -> CANCELLED -> IDLE
// ============================================================
enum DragState {
    DRAG_IDLE,      // No interaction in progress
    DRAG_PRESS,     // Mouse down, waiting for move threshold
    DRAG_REORDER,   // Dragging a pinned icon within the dock to reorder
    DRAG_DRAGGING,  // Threshold crossed, ghost visible (unpin / external drop)
    DRAG_DROPPED,   // Drop has been processed
    DRAG_CANCELLED  // Drag aborted (no valid source, Escape, etc.)
};
static DragState g_dragState = DRAG_IDLE;

// ============================================================
//  BOOT STATE
// ============================================================
enum SystemState { STATE_BOOT, STATE_STABILIZING, STATE_STABLE };
static SystemState g_systemState = STATE_BOOT;

// ============================================================
//  EXCLUDED APPS  --  system processes that must never be pinned
// ============================================================
static const wchar_t* g_excludeList[] = {
    L"explorer.exe",
    L"SearchHost.exe",
    L"StartMenuExperienceHost.exe",
    L"ShellExperienceHost.exe",
    L"ApplicationFrameHost.exe",
    L"LockApp.exe",
    L"LogonUI.exe",
    L"Taskmgr.exe",
    L"TextInputHost.exe",
    L"SystemSettings.exe",
    NULL
};

// Case-insensitive substring check against the exclusion list.
// Empty path is always excluded.
static bool IsExcludedApp(const std::wstring& path) {
    if (path.empty()) return true;
    for (int i = 0; g_excludeList[i]; ++i)
        if (StrStrIW(path.c_str(), g_excludeList[i]))
            return true;
    return false;
}

// ============================================================
//  DATA STRUCTURES
// ============================================================
struct PinnedApp {
    std::wstring exePath;
    HICON        icon        = NULL;
    float        currentX    = 0.f;  // Animated X position (overlay-local)
    float        targetX     = 0.f;  // Destination X position
    float        velocityX   = 0.f;  // Momentum for position animation
    float        opacity     = 0.f;  // 0.0 - 1.0 for fade-in animation
    bool         isNew       = true; // Triggers entrance animation
    float        hoverScale  = 1.0f; // Animated scale: 1.0 = normal, HOVER_SCALE_FACTOR = hovered
    bool         running     = false; // True when >=1 visible window of this app exists
};

// ============================================================
//  GLOBALS  --  window handles
// ============================================================
static HWND    g_overlayWnd   = NULL;
static HWND    g_ghostWnd     = NULL;
static HBITMAP g_ghostDIB     = NULL;
static BYTE*   g_ghostBits    = NULL;

// ============================================================
//  GLOBALS  --  taskbar / dock geometry (screen-space)
// ============================================================
static HWND    g_cachedTaskbar       = NULL;
static RECT    g_cachedTBRect        = {};
static RECT    g_cachedDockRect      = {};  // Authoritative drop-zone rect
static int     g_dockLocalW          = 0;   // Overlay client width
static int     g_dockLocalH          = 0;   // Overlay client height
static bool    g_dockWidthLocked     = false;
static bool    g_positionInitialized = false;
static int     g_stableGeometryCount = 0;
static int     g_lastStableWidth     = 0;
static DWORD   g_bootStartTime       = 0;

// Fixed dock width cache  --  computed once per DPI level, reset on DPI change
static int     g_fixedDockWidth      = 0;
static int     g_lastDpiForWidth     = 0;  // DPI at which g_fixedDockWidth was computed

// Position stabilization
static int     g_stabilizedDockLeft  = 0;
static bool    g_dockPositionLocked  = false;

// Geometry change-detection state
static RECT    g_lastTBRect    = {};
static LONG    g_lastStartLeft = 0;
static LONG    g_lastScreenW   = 0;
static LONG    g_lastScreenH   = 0;

// ============================================================
//  GLOBALS  --  smooth dock position animation
// ============================================================
static float   g_dockCurrentX     = 0.f;
static float   g_dockCurrentY     = 0.f;
static float   g_dockVelocityX    = 0.f;
static float   g_dockVelocityY    = 0.f;
static float   g_dockTargetX      = 0.f;
static float   g_dockTargetY      = 0.f;
static bool    g_dockPosAnimActive = false;

// ============================================================
//  GLOBALS  --  per-frame animation tracking
// ============================================================
static DWORD   g_lastFrameTime      = 0;
static float   g_frameDeltaMs       = 16.f;
static bool    g_anyAnimationActive = false;
static int     g_idleFrames         = 0;

// ============================================================
//  GLOBALS  --  interaction state
// ============================================================
static int     g_hoverIndex      = -1;
static POINT   g_dragStartPt     = {};
static DWORD   g_mouseDownTime   = 0;
static std::wstring g_draggedAppPath;  // Path locked at PRESS
static std::wstring g_lockedDragPath;  // Second lock confirmed at DRAGGING threshold
static HICON   g_dragGhostIcon   = NULL;
static bool    g_dragFromDock    = false;
static int     g_dragFromDockIdx = -1;
static bool    g_dropZoneActive  = false;

// Rapid-click state
static int     g_rapidClickCount = 0;
static DWORD   g_rapidClickStart = 0;
static int     g_rapidClickIndex = -1;

// Visual feedback
static bool    g_limitFlashActive = false;
static DWORD   g_limitFlashStart  = 0;
static bool    g_shakeActive      = false;  // Dock shake on pin-limit hit
static DWORD   g_shakeStart       = 0;

// Taskbar auto-hide integration
static bool    g_taskbarAutoHide  = false;  // True when taskbar has ABS_AUTOHIDE set

// Pin/unpin hotkey
static const UINT HOTKEY_PIN_ID        = 1777;        // WM_HOTKEY wParam identifier
// WM_APP message posted by the worker thread to trigger secondary dock
// rebuild on the main thread (DestroyWindow must only be called by the
// thread that created the window -- i.e. the main thread).
static const UINT WM_QPD_REBUILD_SECONDARY = WM_APP + 1;
static UINT    g_hotkeyMods       = MOD_CONTROL | MOD_ALT;
static UINT    g_hotkeyKey        = 'P';               // Default: Ctrl+Alt+P

// Hover candidate  --  pre-sampled resolver result for anti-flicker stability
static std::wstring g_hoverCandidate;
static DWORD        g_hoverCandidateTime = 0;  // Tick when current candidate was first seen
static DWORD        g_hoverLastSample    = 0;  // Tick of last resolver call during hover

// Launch rate limiting
static DWORD   g_lastLaunchTime = 0;
static int     g_launchCount   = 0;
static int     g_launchIndex   = -1;

// Last-active app (informational only, not used for drag decisions)
// g_lastActiveAppPath removed  --  was updated by UpdateLastActiveWindow() every idle
// frame but never read by any decision path. Removed to eliminate dead CPU work.

// ============================================================
//  GLOBALS  --  drag-reorder state
// ============================================================
static int g_reorderSrcIdx    = -1;  // Index of icon being reordered
static int g_reorderTargetIdx = -1;  // Current target slot

// ============================================================
//  GLOBALS  --  multi-monitor secondary docks
// ============================================================
struct SecondaryDock {
    HWND overlay  = NULL;
    RECT tbRect   = {};
    RECT dockRect = {};
    int  localW   = 0;
    int  localH   = 0;
};
static std::vector<SecondaryDock> g_secondaryDocks;

// ============================================================
// PER-MONITOR DOCK STUB  (disabled  --  remove #if 0 to activate)
// Each display gets an independent dock position and optional
// per-monitor pinned-app list keyed by monitor index.
// Registry layout: QPDock\Monitor_<index>\Pinned
// ============================================================
#if 0
struct MonitorDockState {
    HMONITOR              hMon       = NULL;
    RECT                  monRect    = {};
    RECT                  tbRect     = {};
    RECT                  dockRect   = {};
    int                   localW     = 0;
    int                   localH     = 0;
    HWND                  overlay    = NULL;
    std::vector<PinnedApp> pinnedApps; // Independent per-monitor pin list
};
static std::vector<MonitorDockState> g_monitorDocks;

static void LoadMonitorPins(MonitorDockState& dock, int monIdx) {
    wchar_t key[64];
    swprintf_s(key, L"monitor%d_pinned", monIdx);
    // TODO: read from registry sub-key using monIdx
    (void)dock; (void)key;
}
static void SaveMonitorPins(const MonitorDockState& dock, int monIdx) {
    wchar_t key[64];
    swprintf_s(key, L"monitor%d_pinned", monIdx);
    (void)dock; (void)key;
}
static BOOL CALLBACK MonitorEnumProcPerDock(HMONITOR hMon, HDC, LPRECT, LPARAM) {
    MonitorDockState s;
    s.hMon = hMon;
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);
    s.monRect = mi.rcWork;
    g_monitorDocks.push_back(s);
    return TRUE;
}
static void InitPerMonitorDocks() {
    g_monitorDocks.clear();
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProcPerDock, 0);
    for (int i = 0; i < (int)g_monitorDocks.size(); ++i)
        LoadMonitorPins(g_monitorDocks[i], i);
}
static void DestroyPerMonitorDocks() {
    for (auto& d : g_monitorDocks)
        if (d.overlay && IsWindow(d.overlay)) DestroyWindow(d.overlay);
    g_monitorDocks.clear();
}
#endif // PER_MONITOR_DOCK_STUB

// ============================================================
//  GLOBALS  --  pinned apps list
// ============================================================
static std::vector<PinnedApp> g_pinnedApps;

// ============================================================
//  GLOBALS  --  cached GDI objects (created once, freed in Wh_ModUninit)
// ============================================================
static HBRUSH  g_blackBrush    = NULL;
static HPEN    g_linePenNormal = NULL;
static HPEN    g_linePenFlash  = NULL;
static HPEN    g_linePenDrop   = NULL;
static HBRUSH  g_runDotBrush   = NULL;  // Cached brush for running-state indicator dots

// Alpha-blend off-screen buffer (reused every paint to avoid per-frame allocations)
static HBITMAP g_alphaBlendBmp  = NULL;
static HDC     g_alphaBlendDC   = NULL;
static BYTE*   g_alphaBlendBits = NULL;
static int     g_alphaBlendSize = 0;  // Edge length of the current square buffer

// Separator alpha-blend DIB  --  cached across frames, rebuilt only when height changes
static HDC     g_sepDC      = NULL;
static HBITMAP g_sepDIB     = NULL;
static BYTE*   g_sepBits    = NULL;
static int     g_sepCachedH = 0;  // lineH for which the cached DIB was built

// ============================================================
//  GLOBALS  --  threading
// ============================================================
static CRITICAL_SECTION g_cs;
static bool    g_csInitialized  = false;
static HANDLE  g_exitEvent      = NULL;
static HANDLE  g_workerThread   = NULL;
// g_animationActive removed  --  was written but never read. g_anyAnimationActive is the live flag.

// Dedicated lock for g_secondaryDocks  --  worker thread iterates it (RepaintSecondaryDocks)
// while the main thread may destroy/recreate it (WinEventProc, Wh_ModUninit).
static CRITICAL_SECTION g_secondaryDocksCS;
static bool    g_secondaryDocksCSInit = false;

// ============================================================
//  GLOBALS  --  WinEvent hook
// ============================================================
static HWINEVENTHOOK g_winEventHook = NULL;

// ============================================================
//  DEBUG LOGGING
// ============================================================
#define DEBUG_LOG(fmt, ...) Wh_Log(L"[QPDock] " fmt, ##__VA_ARGS__)

// DragTraceLog  --  logs every state transition immediately (no rate limit).
// Only called on genuine state changes, never per-frame, so verbosity is controlled.
static void DragTraceLog(const wchar_t* event, const wchar_t* detail = L"") {
    if (detail && detail[0])
        Wh_Log(L"[DRAG] %s | %s", event, detail);
    else
        Wh_Log(L"[DRAG] %s", event);
}

// ============================================================
//  FORWARD DECLARATIONS
// ============================================================
void  SavePinnedApps();
void  LoadPinnedApps();
void  RepositionOverlay();
RECT  GetIconRectLocal(int index, int totalCount);
int   MaxIconsFit();
bool  IsPinned(const std::wstring& path);
void  PinApp(const std::wstring& path);
void  UnpinAppByIndex(int i);
void  UnpinAllApps();

static void     GhostCleanup();
static void     GhostDragReset();
static bool     IsInDockZone(POINT screenPt, int tolerancePx = 10);
static void     UpdateAutoHideState();
static bool     IsNearDockZone(POINT screenPt);
static bool     IsCursorInDockOrTaskbarRegion(POINT screenPt);
static int      HitTestIcon(POINT screenPt);
static void     UpdateDockTargetPosition(float newTargetX, float newTargetY);
static bool     AnimateDockPositionStep();
static HICON    LoadAppIconStrict(const std::wstring& path);
static void     SmartLaunch(int idx);
static void     LaunchApp(int idx);
static void     UpdateGhostWindow(POINT cursorPt);
static bool     HasTaskbarGeometryChanged();
static void     RefreshTaskbarCache();
static void     RefreshDpiScale();
static void     TriggerLimitFlash();
static void     ValidateAndCleanPinnedList();
static bool     IsSystemWindow(HWND hwnd);
static bool     IsTrueSystemWindow(HWND hwnd);
static int      CalculateReorderSlot(POINT screenPt, int n);
static void     UpdateReorderPositions();
static void     CommitReorder();
static void     UpdateRunningState();
static void     InitSecondaryDocks();
static void     DestroySecondaryDocks();
static void     RepaintSecondaryDocks();
LRESULT CALLBACK SecondaryOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool     IsCursorOverTaskbar(POINT pt);
static HWND     GetRealWindowFromPoint(POINT pt);
static LONG     GetStartButtonLeftEdge(HWND taskbar, const RECT& tbRect);

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD   WINAPI   WorkerThread(LPVOID);

// Zero-rejection resolver
static std::wstring ResolveDragSourceZeroRejection(POINT pt);
static std::wstring Resolver_Layer1_UIHit(POINT pt);
static std::wstring Resolver_Layer2_TaskbarIntelligence(POINT pt);
static std::wstring Resolver_Layer3_ProcessFallback(POINT pt);

// ============================================================
//  DPI SCALING
// ============================================================
static void RefreshDpiScale() {
    HWND tb = g_cachedTaskbar ? g_cachedTaskbar : FindWindowW(L"Shell_TrayWnd", NULL);
    UINT dpi = tb ? GetDpiForWindow(tb) : 96u;
    if (dpi == 0) dpi = 96;
    float s = dpi / 96.0f;

    ICON_SIZE         = (int)(BASE_ICON_SIZE      * s);
    ICON_SPACING      = (int)(BASE_ICON_SPACING   * s);
    DOCK_PAD_LEFT     = (int)(BASE_DOCK_PAD_LEFT  * s);
    DOCK_PAD_RIGHT    = (int)(BASE_DOCK_PAD_RIGHT * s);
    MAGNETIC_RANGE_PX = (int)(BASE_MAGNETIC_RANGE * s);
    GHOST_SIZE        = (int)(BASE_GHOST_SIZE     * s);

    // Invalidate the fixed width cache whenever DPI changes
    if ((int)dpi != g_lastDpiForWidth) {
        g_fixedDockWidth  = 0;
        g_lastDpiForWidth = (int)dpi;
    }
}

// ============================================================
//  WIN11 START BUTTON DETECTION
// ============================================================
struct StartFindCtx {
    HWND taskbar;
    RECT taskbarRect;
    LONG bestLeft;
    bool found;
};

static BOOL CALLBACK FindStartCallback(HWND hwnd, LPARAM lParam) {
    StartFindCtx* ctx = (StartFindCtx*)lParam;
    wchar_t cls[256] = {};
    GetClassNameW(hwnd, cls, 256);
    bool isStart = (wcsstr(cls, L"Start") != NULL) || (wcsstr(cls, L"InputSite") != NULL);
    if (!isStart) return TRUE;

    RECT r = {};
    GetWindowRect(hwnd, &r);
    LONG tbMid = ctx->taskbarRect.left + (ctx->taskbarRect.right - ctx->taskbarRect.left) / 2;
    if (r.left > tbMid) return TRUE;  // Ignore Start buttons on the right half (centered taskbar)

    if (!ctx->found || r.left < ctx->bestLeft) {
        ctx->bestLeft = r.left;
        ctx->found    = true;
    }
    return TRUE;
}

static LONG GetStartButtonLeftEdge(HWND taskbar, const RECT& tbRect) {
    // Fast path: direct child lookup
    HWND startDirect = FindWindowExW(taskbar, NULL, L"Start", NULL);
    if (startDirect) {
        RECT sr = {};
        GetWindowRect(startDirect, &sr);
        if (sr.left > tbRect.left && sr.left < tbRect.right)
            return sr.left;
    }

    // Enumerate all taskbar children to find Start
    StartFindCtx ctx = {};
    ctx.taskbar      = taskbar;
    ctx.taskbarRect  = tbRect;
    ctx.bestLeft     = tbRect.left + (tbRect.right - tbRect.left) / 5;  // Sensible fallback
    ctx.found        = false;
    EnumChildWindows(taskbar, FindStartCallback, (LPARAM)&ctx);

    return ctx.found ? ctx.bestLeft : (tbRect.left + (tbRect.right - tbRect.left) / 5);
}

// ============================================================
//  TASKBAR GEOMETRY CHANGE DETECTION
// ============================================================
static bool HasTaskbarGeometryChanged() {
    HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!tb) return false;

    RECT tbr = {};
    GetWindowRect(tb, &tbr);
    if (tbr.left   != g_lastTBRect.left   || tbr.top    != g_lastTBRect.top  ||
        tbr.right  != g_lastTBRect.right  || tbr.bottom != g_lastTBRect.bottom)
        return true;

    LONG startLeft = GetStartButtonLeftEdge(tb, tbr);
    if (startLeft != g_lastStartLeft) return true;

    LONG sw = GetSystemMetrics(SM_CXSCREEN);
    LONG sh = GetSystemMetrics(SM_CYSCREEN);
    return (sw != g_lastScreenW || sh != g_lastScreenH);
}

// ============================================================
//  DOCK TARGET POSITION  --  stabilized, jitter-filtered
// ============================================================
static void UpdateDockTargetPosition(float newTargetX, float newTargetY) {
    if (fabsf(newTargetX - g_dockTargetX) > (float)ANIM_POS_THRESHOLD ||
        fabsf(newTargetY - g_dockTargetY) > (float)ANIM_POS_THRESHOLD) {
        g_dockTargetX      = newTargetX;
        g_dockTargetY      = newTargetY;
        g_dockPosAnimActive = true;
    }
}

// ============================================================
//  TASKBAR CACHE REFRESH  --  called on change events and at boot
// ============================================================
static void RefreshTaskbarCache() {
    HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!tb) return;

    RECT tbr = {};
    GetWindowRect(tb, &tbr);
    g_cachedTaskbar = tb;
    g_cachedTBRect  = tbr;
    g_lastTBRect    = tbr;

    LONG startLeft  = GetStartButtonLeftEdge(tb, tbr);
    g_lastStartLeft = startLeft;
    g_lastScreenW   = GetSystemMetrics(SM_CXSCREEN);
    g_lastScreenH   = GetSystemMetrics(SM_CYSCREEN);

    // DPI must be refreshed before any geometry calculations
    RefreshDpiScale();

    // Fixed dock width  --  constant for a given DPI level so the dock
    // never resizes when icons are added or removed.
    if (g_fixedDockWidth == 0) {
        g_fixedDockWidth = DOCK_PAD_LEFT
                         + (MAX_PINNED_APPS * ICON_SIZE)
                         + ((MAX_PINNED_APPS - 1) * ICON_SPACING)
                         + DOCK_PAD_RIGHT;
        g_fixedDockWidth = std::max(g_fixedDockWidth, 160);
        g_fixedDockWidth = std::min(g_fixedDockWidth, 400);
    }

    // Anchor dock right edge just left of the Start button
    int dockRight  = (int)(startLeft - DOCK_SAFE_GAP);
    int dockLeft   = dockRight - g_fixedDockWidth;
    int dockTop    = tbr.top;
    int dockBottom = tbr.bottom;

    // Edge guard: never let dock go off-screen left
    if (dockLeft < 0) {
        dockLeft  = 0;
        dockRight = g_fixedDockWidth;
    }

    // Position stabilization: lock X after first valid read, only update on
    // significant changes to prevent micro-jitter from Start-button detection noise.
    if (!g_dockPositionLocked) {
        g_stabilizedDockLeft = dockLeft;
        g_dockPositionLocked = true;
    } else {
        int delta = abs(dockLeft - g_stabilizedDockLeft);
        if (delta > POS_RESET_THRESHOLD) {
            // Large change (resolution switch, taskbar moved)  --  full re-lock
            g_stabilizedDockLeft = dockLeft;
        } else if (delta > POS_LOCK_THRESHOLD) {
            // Moderate drift  --  update the locked position
            g_stabilizedDockLeft = dockLeft;
        }
        dockLeft  = g_stabilizedDockLeft;
        dockRight = dockLeft + g_fixedDockWidth;
    }

    g_cachedDockRect = { dockLeft, dockTop, dockRight, dockBottom };

    // Feed the smooth animation target (2-px jitter filter inside)
    UpdateDockTargetPosition((float)dockLeft, (float)dockTop);

    // Snap current position on first valid geometry to prevent fly-in from (0,0)
    if (!g_positionInitialized && dockLeft > 0) {
        g_dockCurrentX        = (float)dockLeft;
        g_dockCurrentY        = (float)dockTop;
        g_dockTargetX         = g_dockCurrentX;
        g_dockTargetY         = g_dockCurrentY;
        g_positionInitialized = true;
    }

    int newW = dockRight - dockLeft;
    int newH = dockBottom - dockTop;

    // Rate-limited geometry log
    static DWORD s_lastGeomLog = 0;
    DWORD now = GetTickCount();
    if (now - s_lastGeomLog > 1000) {
        DEBUG_LOG(L"GEOMETRY: w=%d dockLeft=%d startLeft=%d", newW, dockLeft, startLeft);
        s_lastGeomLog = now;
    }

    if (newW <= 0 || startLeft <= tbr.left + 50) return;

    // Helper lambda: re-seat all pinned-app icon positions from the now-valid
    // g_dockLocalW.  LoadPinnedApps runs before geometry is ready, so all icons
    // start at x=0 (GetIconRectLocal returns {} when dockLocalW==0).  We fix that
    // here at every geometry-lock boundary so icons never pile up at the left edge.
    // Called while g_dockLocalW has already been written; acquires CS internally.
    auto ReseatIconPositions = [&]() {
        if (!g_csInitialized) return;
        EnterCriticalSection(&g_cs);
        int total = (int)g_pinnedApps.size();
        for (int i = 0; i < total; ++i) {
            RECT r = GetIconRectLocal(i, total);
            g_pinnedApps[i].targetX  = (float)r.left;
            g_pinnedApps[i].currentX = (float)r.left;
            g_pinnedApps[i].velocityX = 0.f;
        }
        LeaveCriticalSection(&g_cs);
    };

    // Boot: accept first valid width immediately, start stabilisation.
    // Count this measurement as the FIRST stable reading (count=1, lastWidth=newW)
    // so the very next call in STATE_STABILIZING that agrees within 10 px immediately
    // reaches count=2 -> STATE_STABLE.  Previously lastWidth was left at 0 here,
    // requiring two extra STABILIZING calls instead of one.
    if (g_systemState == STATE_BOOT) {
        if (newW > MIN_VALID_DOCK_WIDTH) {
            g_dockLocalW          = newW;
            g_dockLocalH          = newH;
            g_dockCurrentX        = (float)dockLeft;
            g_dockCurrentY        = (float)dockTop;
            g_systemState         = STATE_STABILIZING;
            g_stableGeometryCount = 1;      // boot reading counts as first stable sample
            g_lastStableWidth     = newW;   // seed so next agreeing call hits count>=2
            ReseatIconPositions();   // positions were 0 from LoadPinnedApps
            DEBUG_LOG(L"BOOT: first geometry accepted w=%d", newW);
            RepositionOverlay();
            if (g_overlayWnd && IsWindow(g_overlayWnd))
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }
        return;
    }

    // Stabilising: lock after 2 consecutive reads within +/-10 px
    if (g_systemState == STATE_STABILIZING) {
        if (newW > MIN_VALID_DOCK_WIDTH) {
            if (g_lastStableWidth > 0 && abs(newW - g_lastStableWidth) < 10) {
                g_stableGeometryCount++;
            } else {
                g_stableGeometryCount = 1;
                g_lastStableWidth     = newW;
            }
            if (g_stableGeometryCount >= 2) {
                g_dockLocalW      = newW;
                g_dockLocalH      = newH;
                g_dockWidthLocked = true;
                g_systemState     = STATE_STABLE;
                ReseatIconPositions();  // final geometry lock  --  settle all icons
                DEBUG_LOG(L"GEOMETRY: STABLE w=%d h=%d", newW, newH);
                RepositionOverlay();
                if (g_overlayWnd && IsWindow(g_overlayWnd))
                    InvalidateRect(g_overlayWnd, NULL, FALSE);
            }
        }
        // Boot-phase timeout: lock whatever width we have after BOOT_PHASE_MS
        if (g_lastStableWidth > 0 &&
            (int)(GetTickCount() - g_bootStartTime) > BOOT_PHASE_MS) {
            g_dockLocalW      = g_lastStableWidth;
            g_dockLocalH      = newH;
            g_dockWidthLocked = true;
            g_systemState     = STATE_STABLE;
            ReseatIconPositions();  // timeout path also needs position correction
            DEBUG_LOG(L"GEOMETRY: STABLE (boot timeout) w=%d", g_dockLocalW);
        }
        return;
    }

    // Stable: only update on significant change (>50 px = resolution/taskbar change)
    if (abs(newW - g_dockLocalW) > 50 && newW > MIN_VALID_DOCK_WIDTH) {
        g_dockLocalW = newW;
        DEBUG_LOG(L"GEOMETRY: major update w=%d", newW);
    }
    g_dockLocalH = newH;
}

// ============================================================
//  ICON GEOMETRY  --  overlay-LOCAL coords (origin = dock top-left)
//  Icons anchor to the RIGHT edge and grow leftward.
// ============================================================
RECT GetIconRectLocal(int index, int totalCount) {
    if (totalCount <= 0 || g_dockLocalH <= 0 || g_dockLocalW <= 0)
        return {};

    int rightEdge    = g_dockLocalW - DOCK_PAD_RIGHT;
    int reverseIndex = (totalCount - 1) - index;
    int x = rightEdge - ICON_SIZE - (reverseIndex * (ICON_SIZE + ICON_SPACING));
    int y = (g_dockLocalH - ICON_SIZE) / 2;
    if (y < 2) y = 2;
    return { x, y, x + ICON_SIZE, y + ICON_SIZE };
}

// Maximum icons that can be physically laid out in the current dock width.
int MaxIconsFit() {
    int w = g_dockLocalW - DOCK_PAD_LEFT - DOCK_PAD_RIGHT;
    if (w <= 0) return 0;
    return (w + ICON_SPACING) / (ICON_SIZE + ICON_SPACING);
}

// ============================================================
//  ZONE HELPERS  --  all screen-space coordinates
// ============================================================

// Strict drop zone: dock rect + optional pixel tolerance.
static bool IsInDockZone(POINT screenPt, int tolerancePx) {
    // Always use the stable authoritative cached rect  --  never the animated position.
    // Mid-lerp animated positions (g_dockCurrentX/Y) cause false "outside zone"
    // detections during fast drags where the dock hasn't finished interpolating.
    RECT z = (g_cachedDockRect.right > g_cachedDockRect.left)
             ? g_cachedDockRect
             : RECT{ (LONG)g_dockCurrentX, (LONG)g_dockCurrentY,
                     (LONG)g_dockCurrentX + g_dockLocalW,
                     (LONG)g_dockCurrentY + g_dockLocalH };
    if (tolerancePx > 0) {
        z.left   -= tolerancePx;
        z.right  += tolerancePx;
        z.top    -= tolerancePx;
        z.bottom += tolerancePx;
    }
    return PtInRect(&z, screenPt) != 0;
}

// Magnetic zone: larger area used only for ghost visibility and hover effects.
static bool IsNearDockZone(POINT screenPt) {
    RECT z;
    if (g_dockCurrentX > 0.f || g_dockCurrentY > 0.f) {
        z.left   = (int)g_dockCurrentX;
        z.top    = (int)g_dockCurrentY;
        z.right  = z.left + g_dockLocalW;
        z.bottom = z.top  + g_dockLocalH;
    } else {
        z = g_cachedDockRect;
    }
    z.right  += MAGNETIC_RANGE_PX;
    z.top    -= MAGNETIC_ZONE_EXPAND_PX;
    z.bottom += MAGNETIC_ZONE_EXPAND_PX;
    return PtInRect(&z, screenPt) != 0;
}

// Combined dock + full taskbar region for ghost visibility scoping.
static bool IsCursorInDockOrTaskbarRegion(POINT screenPt) {
    if (PtInRect(&g_cachedDockRect, screenPt)) return true;
    RECT tbr = g_cachedTBRect;
    tbr.left   -= MAGNETIC_RANGE_PX;
    tbr.right  += MAGNETIC_RANGE_PX;
    tbr.top    -= MAGNETIC_ZONE_EXPAND_PX;
    tbr.bottom += MAGNETIC_ZONE_EXPAND_PX;
    return PtInRect(&tbr, screenPt) != 0;
}

// ============================================================
//  HIT TEST  --  screen coordinates -> pinned-app slot index
//  Returns -1 when no icon is under the given point.
//  Acquires g_cs to protect g_pinnedApps  --  called from both the main thread
//  (WM_NCHITTEST) and the worker thread (drag state machine / click handler).
// ============================================================
static int HitTestIcon(POINT screenPt) {
    if (g_dockLocalW <= 0 || g_dockLocalH <= 0) return -1;
    if (!g_csInitialized) return -1;

    // Use animated position for accurate hit testing during dock glide
    int dockLeft = (int)g_dockCurrentX;
    int dockTop  = (int)g_dockCurrentY;
    if (dockLeft == 0 && g_cachedDockRect.left > 0) {
        dockLeft = g_cachedDockRect.left;
        dockTop  = g_cachedDockRect.top;
    }

    POINT local = { screenPt.x - dockLeft, screenPt.y - dockTop };

    // CS held for the minimum time  --  pure geometry arithmetic, sub-microsecond.
    EnterCriticalSection(&g_cs);
    int n   = (int)g_pinnedApps.size();
    int hit = -1;
    for (int i = 0; i < n && hit < 0; ++i) {
        if (!g_pinnedApps[i].icon) continue;
        RECT r = GetIconRectLocal(i, n);
        InflateRect(&r, 2, 2);
        if (PtInRect(&r, local)) hit = i;
    }
    LeaveCriticalSection(&g_cs);
    return hit;
}

// ============================================================
//  OVERLAY REPOSITION
// ============================================================
void RepositionOverlay() {
    if (!g_overlayWnd || !IsWindow(g_overlayWnd)) return;

    // Emergency fallback: if dock size is still zero, provide a safe default
    if (g_dockLocalW <= 0) {
        g_dockLocalW = 200;
        g_dockLocalH = 48;
        HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
        if (tb) {
            RECT tbr = {};
            GetWindowRect(tb, &tbr);
            LONG sl = GetStartButtonLeftEdge(tb, tbr);
            g_dockCurrentX = (float)(sl - DOCK_SAFE_GAP - g_dockLocalW);
            g_dockCurrentY = (float)tbr.top;
            g_dockTargetX  = g_dockCurrentX;
            g_dockTargetY  = g_dockCurrentY;
        }
        DEBUG_LOG(L"FALLBACK: forced dock geometry w=%d h=%d", g_dockLocalW, g_dockLocalH);
    }

    int w = g_dockLocalW;
    int h = g_dockLocalH;
    int x = (int)g_dockCurrentX;
    int y = (int)g_dockCurrentY;

    static DWORD s_lastReposLog = 0;
    static int   s_lastReposX   = -9999;
    DWORD now = GetTickCount();
    if (abs(x - s_lastReposX) > 50 || now - s_lastReposLog > 2000) {
        DEBUG_LOG(L"REPOSITION: x=%d y=%d w=%d h=%d", x, y, w, h);
        s_lastReposLog = now;
        s_lastReposX   = x;
    }

    // Auto-hide integration: when the taskbar is slid out of view (< 8px tall/wide),
    // hide the dock overlay too so it doesn't float in empty space.
    if (g_taskbarAutoHide) {
        bool tbOnScreen = (g_cachedTBRect.bottom - g_cachedTBRect.top) > 8 &&
                          (g_cachedTBRect.right  - g_cachedTBRect.left) > 8;
        ShowWindow(g_overlayWnd, tbOnScreen ? SW_SHOWNOACTIVATE : SW_HIDE);
        SetWindowPos(g_overlayWnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
    } else {
        SetWindowPos(g_overlayWnd, HWND_TOPMOST, x, y, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    // Rounded corners: apply a rounded-rectangle clip region so the dock edges
    // blend softly into the taskbar instead of sharp right angles.
    // Corner radius = 5px (at 96 DPI) -- subtle, matches Win11 element radii.
    // SetWindowRgn transfers ownership; no need to DeleteObject on success.
    {
        static int s_lastRgnW = 0, s_lastRgnH = 0;
        if (w != s_lastRgnW || h != s_lastRgnH) {
            int r = 5;
            HRGN hRgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, r * 2, r * 2);
            if (hRgn) SetWindowRgn(g_overlayWnd, hRgn, FALSE);
            s_lastRgnW = w;
            s_lastRgnH = h;
        }
    }
}

// ============================================================
//  POSITION ANIMATION  --  time-based lerp + momentum decay
// ============================================================
static bool AnimateDockPositionStep() {
    if (!g_dockPosAnimActive) return false;

    float dx = g_dockTargetX - g_dockCurrentX;
    float dy = g_dockTargetY - g_dockCurrentY;

    if (fabsf(dx) <= ANIM_SNAP_THRESHOLD && fabsf(dy) <= ANIM_SNAP_THRESHOLD) {
        g_dockCurrentX      = g_dockTargetX;
        g_dockCurrentY      = g_dockTargetY;
        g_dockVelocityX     = 0.f;
        g_dockVelocityY     = 0.f;
        g_dockPosAnimActive = false;
        return true;
    }

    float timeFactor = (g_frameDeltaMs / 16.0f) * ANIM_LERP_FACTOR;
    if (timeFactor > 0.5f) timeFactor = 0.5f;

    g_dockVelocityX = (g_dockVelocityX + dx * timeFactor) * ANIM_MOMENTUM_DECAY;
    g_dockVelocityY = (g_dockVelocityY + dy * timeFactor) * ANIM_MOMENTUM_DECAY;
    g_dockCurrentX += g_dockVelocityX;
    g_dockCurrentY += g_dockVelocityY;
    return true;
}

// ============================================================
//  ICON LOADING  --  strict; returns NULL if icon cannot be obtained.
//  Always duplicates the icon handle so we own our copy outright.
// ============================================================
static HICON LoadAppIconStrict(const std::wstring& path) {
    if (path.empty()) return NULL;

    HICON hSource = NULL;

    // Attempt 1: SHGetImageList JUMBO (256px) -- covers modern apps with high-DPI icons.
    // This is the highest-fidelity source and resolves icons for apps that embed
    // only large icon resources (UWP, Electron, etc.).
    if (!hSource) {
        IImageList* pImgList = NULL;
        if (SUCCEEDED(SHGetImageList(SHIL_JUMBO, IID_IImageList, (void**)&pImgList)) && pImgList) {
            SHFILEINFOW sfi = {};
            DWORD_PTR hr = SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                                           SHGFI_SYSICONINDEX);
            if (hr && sfi.iIcon >= 0) {
                HICON hTmp = NULL;
                if (SUCCEEDED(pImgList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hTmp)) && hTmp)
                    hSource = hTmp;
            }
            pImgList->Release();
        }
    }

    // Attempt 2: SHGetImageList EXTRALARGE (48px) -- standard large icon.
    if (!hSource) {
        IImageList* pImgList = NULL;
        if (SUCCEEDED(SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&pImgList)) && pImgList) {
            SHFILEINFOW sfi = {};
            DWORD_PTR hr = SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                                           SHGFI_SYSICONINDEX);
            if (hr && sfi.iIcon >= 0) {
                HICON hTmp = NULL;
                if (SUCCEEDED(pImgList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hTmp)) && hTmp)
                    hSource = hTmp;
            }
            pImgList->Release();
        }
    }

    // Attempt 3: SHDefExtractIconW -- shell default extractor (handles icon overlays,
    // shell icon handlers, and apps with custom icon extractors like Steam games).
    if (!hSource) {
        HICON hLg = NULL, hSm = NULL;
        if (SUCCEEDED(SHDefExtractIconW(path.c_str(), 0, 0, &hLg, &hSm, 48)) && hLg) {
            hSource = hLg;
            if (hSm) DestroyIcon(hSm);
        }
    }

    // Attempt 4: ExtractIconExW -- direct PE resource extraction.
    // Fastest for traditional Win32 apps; works even when shell handlers fail.
    if (!hSource) {
        HICON hLarge = NULL;
        if (ExtractIconExW(path.c_str(), 0, &hLarge, NULL, 1) > 0 && hLarge)
            hSource = hLarge;
    }

    // Attempt 5: SHGetFileInfoW LARGEICON -- broadest compatibility fallback.
    // Handles UWP tile icons, file-association icons, and system shortcuts.
    if (!hSource) {
        SHFILEINFOW sfi = {};
        DWORD_PTR ok = SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                                      SHGFI_ICON | SHGFI_LARGEICON);
        if (ok && sfi.hIcon)
            hSource = sfi.hIcon;
    }

    if (!hSource) return NULL;

    // Validate the icon handle before duplicating it
    ICONINFO ii = {};
    if (!GetIconInfo(hSource, &ii)) {
        DestroyIcon(hSource);
        return NULL;
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);

    // Always own a private copy  --  never share the shell-managed handle
    HICON hCopy = CopyIcon(hSource);
    DestroyIcon(hSource);
    return hCopy;
}

// ============================================================
//  PROCESS / WINDOW HELPERS
// ============================================================
static std::wstring GetProcessPath(HWND hwnd) {
    if (!hwnd) return L"";
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || !pid) return L"";
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return L"";
    wchar_t buf[MAX_PATH] = {};
    DWORD len = MAX_PATH;
    QueryFullProcessImageNameW(hProc, 0, buf, &len);
    CloseHandle(hProc);
    return std::wstring(buf);
}

static bool IsSystemWindow(HWND hwnd) {
    if (!hwnd) return true;
    wchar_t cls[256] = {};
    GetClassNameW(hwnd, cls, 256);

    // Taskbar windows are user-interaction layers, not system UI
    if (wcsstr(cls, L"Shell_TrayWnd")    != NULL) return false;
    if (wcsstr(cls, L"Taskbar")          != NULL) return false;
    if (wcsstr(cls, L"MSTaskListWClass") != NULL) return false;
    if (wcsstr(cls, L"ToolbarWindow32")  != NULL) return false;

    // True system surfaces that should never be drag sources
    if (wcsstr(cls, L"StartMenuExperienceHost") != NULL) return true;
    if (wcsstr(cls, L"SearchHost")              != NULL) return true;
    if (wcsstr(cls, L"ShellExperienceHost")     != NULL) return true;
    if (wcsstr(cls, L"Progman")                 != NULL) return true;
    if (wcsstr(cls, L"WorkerW")                 != NULL) return true;
    if (wcsstr(cls, L"#32769")                  != NULL) return true;  // Desktop

    // Other Shell_ windows (excluding Shell_TrayWnd which is allowed above)
    if (wcsstr(cls, L"Shell_") != NULL && wcsstr(cls, L"Shell_TrayWnd") == NULL)
        return true;

    return false;
}

// Identical classification used in the resolver pipeline
static bool IsTrueSystemWindow(HWND hwnd) {
    if (!hwnd) return true;
    wchar_t cls[256] = {};
    GetClassNameW(hwnd, cls, 256);

    bool isTaskbar = wcsstr(cls, L"Shell_TrayWnd")    != NULL ||
                     wcsstr(cls, L"Taskbar")           != NULL ||
                     wcsstr(cls, L"MSTaskListWClass")  != NULL ||
                     wcsstr(cls, L"ToolbarWindow32")   != NULL ||
                     wcsstr(cls, L"ReBarWindow32")     != NULL;
    if (isTaskbar) return false;

    if (wcsstr(cls, L"Progman") != NULL || wcsstr(cls, L"WorkerW") != NULL ||
        wcsstr(cls, L"#32769")  != NULL)
        return true;

    if (wcsstr(cls, L"Shell_") != NULL && wcsstr(cls, L"Shell_TrayWnd") == NULL)
        return true;

    if (wcsstr(cls, L"StartMenuExperienceHost") != NULL) return true;
    if (wcsstr(cls, L"SearchHost")              != NULL) return true;
    if (wcsstr(cls, L"ShellExperienceHost")     != NULL) return true;

    return false;
}

static bool IsCursorOverTaskbar(POINT pt) {
    HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!tb) return false;
    RECT tbr = {};
    GetWindowRect(tb, &tbr);
    InflateRect(&tbr, 20, 10);
    return PtInRect(&tbr, pt) != 0;
}

// Return the real window under a screen point, bypassing our transparent overlay.
static HWND GetRealWindowFromPoint(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) return NULL;

    // If WindowFromPoint returns our overlay it means the cursor is over an icon
    // slot (HTCLIENT area).  Return NULL so the caller falls through to the next
    // resolver layer rather than doing a cross-thread hide/restore of the overlay
    // window, which is unsafe and causes a one-frame flicker.
    // In practice, Phase-0 of the resolver already handles clicks on dock icons
    // before this function is ever reached, so this branch is a pure safety guard.
    if (hwnd == g_overlayWnd) return NULL;

    return hwnd;
}

// ============================================================
//  ZERO-REJECTION DRAG RESOLVER  --  3-layer resolution pipeline
//  Resolves the app path under the cursor at mouse-down time.
//  Returns empty string only when all three layers fail.
// ============================================================

// Helper context for UWP child enumeration
struct UwpCtx { HWND found; DWORD hostPid; };
static BOOL CALLBACK UwpEnumProc(HWND child, LPARAM lp) {
    UwpCtx* c = (UwpCtx*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(child, &pid);
    if (pid && pid != c->hostPid) { c->found = child; return FALSE; }
    return TRUE;
}

// Layer 1: Direct window-from-point lookup with UWP unwrapping
static std::wstring Resolver_Layer1_UIHit(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) return L"";

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) root = hwnd;

    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    if (!pid) return L"";

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return L"";

    wchar_t path[MAX_PATH] = {};
    DWORD len = MAX_PATH;
    QueryFullProcessImageNameW(hProc, 0, path, &len);
    CloseHandle(hProc);

    if (len == 0) return L"";
    std::wstring result(path);

    // Guard: if we resolved explorer.exe and the cursor is in the taskbar region,
    // reject here  --  Layer 2 (UIAutomation) is authoritative for the taskbar surface.
    if (StrStrIW(result.c_str(), L"explorer.exe")) {
        HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
        RECT tbr = {};
        if (tb && GetWindowRect(tb, &tbr)) {
            InflateRect(&tbr, 0, 6);
            if (PtInRect(&tbr, pt))
                return L"";  // Defer to Layer 2
        }
    }

    // Unwrap UWP: ApplicationFrameHost is a host  --  find the real child process
    if (StrStrIW(result.c_str(), L"ApplicationFrameHost.exe")) {
        UwpCtx ctx = { NULL, pid };
        EnumChildWindows(root, UwpEnumProc, (LPARAM)&ctx);
        if (ctx.found) {
            DWORD childPid = 0;
            GetWindowThreadProcessId(ctx.found, &childPid);
            if (childPid && childPid != pid) {
                HANDLE hReal = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                           FALSE, childPid);
                if (hReal) {
                    wchar_t realPath[MAX_PATH] = {};
                    DWORD rlen = MAX_PATH;
                    if (QueryFullProcessImageNameW(hReal, 0, realPath, &rlen) && rlen > 0)
                        if (!StrStrIW(realPath, L"ApplicationFrameHost.exe"))
                            result = realPath;
                    CloseHandle(hReal);
                }
            }
        }
    }

    return result;
}

// ============================================================
//  TASKBAR TITLE MATCHER  --  used by Layer 2 when the UIA element
//  belongs to explorer.exe (the taskbar host process).
//  Enumerates all top-level windows and finds the best title match.
// ============================================================
struct TitleMatchCtx {
    const wchar_t* needle;   // Name string from the UIA element
    HWND           bestHwnd;
    int            bestScore;
};

static BOOL CALLBACK TitleMatchEnumProc(HWND hwnd, LPARAM lp) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    TitleMatchCtx* c = (TitleMatchCtx*)lp;

    // Skip pure system surfaces
    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, 64);
    if (wcsstr(cls, L"Shell_TrayWnd")        != NULL) return TRUE;
    if (wcsstr(cls, L"Progman")              != NULL) return TRUE;
    if (wcsstr(cls, L"WorkerW")              != NULL) return TRUE;
    if (wcsstr(cls, L"#32769")               != NULL) return TRUE;

    wchar_t title[256] = {};
    GetWindowTextW(hwnd, title, 256);
    if (!title[0]) return TRUE;

    // Score the match quality
    int score = 0;
    if (_wcsicmp(title, c->needle) == 0)        score = 100;  // Exact
    else if (StrStrIW(title, c->needle) != NULL) score = 60;  // Title contains needle
    else if (StrStrIW(c->needle, title) != NULL) score = 40;  // Needle contains title
    else return TRUE;

    // Prefer proper app windows
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_APPWINDOW)  score += 20;
    if (ex & WS_EX_TOOLWINDOW) score -= 30;

    if (score > c->bestScore) {
        c->bestScore = score;
        c->bestHwnd  = hwnd;
    }
    return TRUE;
}

// Resolve a process executable path by finding a running window whose title
// matches the given display name fragment (as reported by a UIA element Name).
static std::wstring FindProcessPathByWindowTitle(const wchar_t* name) {
    if (!name || !name[0]) return L"";
    TitleMatchCtx ctx = { name, NULL, -1 };
    EnumWindows(TitleMatchEnumProc, (LPARAM)&ctx);
    if (!ctx.bestHwnd) return L"";

    DWORD pid = 0;
    GetWindowThreadProcessId(ctx.bestHwnd, &pid);
    if (!pid) return L"";

    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return L"";
    wchar_t path[MAX_PATH] = {};
    DWORD len = MAX_PATH;
    QueryFullProcessImageNameW(h, 0, path, &len);
    CloseHandle(h);
    return std::wstring(path);
}

// File-scope constant for the UIA AUMID property ID.
// Declared here (not as a lambda-local static) for compatibility with all
// Windhawk/Clang build configurations -- some versions reject static locals
// inside lambdas.
static const PROPERTYID kAumidPropId = 30113;

// Layer 2: UIAutomation  --  authoritative resolver for the Windows 11 taskbar surface.
//
// Resolution pipeline:
//   1. AUMID -> filesystem path  (works for UWP and many Store apps)
//   2. Element process ID  --  if NOT explorer.exe, use it directly
//   3. Element Name (display label on taskbar button) -> title-match running window
//      This is the key fix: Windows 11 taskbar buttons live inside explorer.exe
//      but their element Name is the real app's window title.
//   4. Walk up the UIA tree one level and retry steps 2 - 3
static std::wstring Resolver_Layer2_TaskbarIntelligence(POINT pt) {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!taskbar) return L"";

    RECT tbRect = {};
    GetWindowRect(taskbar, &tbRect);
    // Expand slightly  --  auto-hide taskbar may clip the rect momentarily
    InflateRect(&tbRect, 0, 6);
    if (!PtInRect(&tbRect, pt)) return L"";

    // COM must be initialised on the calling thread.
    // The worker thread has no Win32 message pump, so COINIT_APARTMENTTHREADED
    // (STA) would block waiting for messages that never arrive.
    // COINIT_MULTITHREADED (MTA) works without a message pump.
    // RPC_E_CHANGED_MODE means COM was already initialised as MTA (fine).
    HRESULT hrInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) return L"";

    IUIAutomation* pAuto = NULL;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IUIAutomation, (void**)&pAuto);
    if (FAILED(hr) || !pAuto) {
        if (SUCCEEDED(hrInit)) CoUninitialize();
        return L"";
    }

    // Helper lambda (C++11): resolve a single UIA element to a path.
    // Returns empty string if the element belongs to explorer / is excluded.
    auto ResolveElement = [&](IUIAutomationElement* el) -> std::wstring {
        if (!el) return L"";
        std::wstring res;

        // Pass 1: AUMID -> filesystem path
        VARIANT varAumid;
        VariantInit(&varAumid);
        // kAumidPropId is now at file scope (above Resolver_Layer2_TaskbarIntelligence)
        // to avoid static-local-in-lambda issues with some Windhawk Clang builds.
        if (SUCCEEDED(el->GetCurrentPropertyValue(kAumidPropId, &varAumid)) &&
            varAumid.vt == VT_BSTR && varAumid.bstrVal && varAumid.bstrVal[0]) {
            IShellItem2* pItem = NULL;
            if (SUCCEEDED(SHCreateItemFromParsingName(varAumid.bstrVal, NULL,
                                                       IID_PPV_ARGS(&pItem))) && pItem) {
                PWSTR fsPath = NULL;
                if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &fsPath)) && fsPath) {
                    res = fsPath;
                    CoTaskMemFree(fsPath);
                }
                pItem->Release();
            }
        }
        VariantClear(&varAumid);
        if (!res.empty() && !IsExcludedApp(res)) return res;
        res.clear();

        // Pass 2: element process ID  --  use directly if it is NOT explorer.exe
        int processId = 0;
        el->get_CurrentProcessId(&processId);
        bool ownerIsExplorer = true;
        if (processId > 0) {
            HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                       FALSE, (DWORD)processId);
            if (hProc) {
                wchar_t procPath[MAX_PATH] = {};
                DWORD plen = MAX_PATH;
                if (QueryFullProcessImageNameW(hProc, 0, procPath, &plen) && plen > 0) {
                    ownerIsExplorer = (StrStrIW(procPath, L"explorer.exe") != NULL);
                    if (!ownerIsExplorer && !IsExcludedApp(procPath))
                        res = procPath;
                }
                CloseHandle(hProc);
            }
        }
        if (!res.empty()) return res;

        // Pass 3: element Name -> title-match running window -> process path.
        // This is how we crack Windows 11 taskbar buttons: the button Name
        // IS the window title of the app it represents, even though the element
        // process is explorer.exe.
        BSTR elemName = NULL;
        if (SUCCEEDED(el->get_CurrentName(&elemName)) && elemName && elemName[0]) {
            res = FindProcessPathByWindowTitle(elemName);
            SysFreeString(elemName);
            if (!res.empty() && !IsExcludedApp(res)) return res;
            res.clear();
        }

        return L"";
    };

    std::wstring result;
    IUIAutomationElement* pElement = NULL;
    hr = pAuto->ElementFromPoint(pt, &pElement);

    if (SUCCEEDED(hr) && pElement) {
        result = ResolveElement(pElement);

        // If still empty, walk one level up the UIA tree and retry.
        // Taskbar buttons in Windows 11 are sometimes nested inside group containers.
        if (result.empty()) {
            IUIAutomationTreeWalker* pWalker = NULL;
            if (SUCCEEDED(pAuto->get_RawViewWalker(&pWalker)) && pWalker) {
                IUIAutomationElement* pParent = NULL;
                if (SUCCEEDED(pWalker->GetParentElement(pElement, &pParent)) && pParent) {
                    result = ResolveElement(pParent);
                    pParent->Release();
                }
                pWalker->Release();
            }
        }

        pElement->Release();
    }

    pAuto->Release();
    if (SUCCEEDED(hrInit)) CoUninitialize();
    return result;
}

// Layer 3: Process enumeration fallback  --  finds the closest visible window to the cursor
struct Layer3EnumCtx {
    POINT        pt;
    std::wstring bestMatch;
    int          bestDistance;
};

static BOOL CALLBACK Layer3EnumProc(HWND hwnd, LPARAM lp) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    Layer3EnumCtx* c = (Layer3EnumCtx*)lp;

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) return TRUE;

    bool inside = PtInRect(&rc, c->pt) != 0;
    int  dist   = 1000;
    if (!inside) {
        int dx = 0, dy = 0;
        if (c->pt.x < rc.left)   dx = rc.left   - c->pt.x;
        else if (c->pt.x > rc.right)  dx = c->pt.x - rc.right;
        if (c->pt.y < rc.top)    dy = rc.top    - c->pt.y;
        else if (c->pt.y > rc.bottom) dy = c->pt.y - rc.bottom;
        dist = dx + dy;
    } else {
        dist = 0;
    }
    if (dist > 50) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return TRUE;

    wchar_t path[MAX_PATH] = {};
    DWORD len = MAX_PATH;
    QueryFullProcessImageNameW(hProc, 0, path, &len);
    CloseHandle(hProc);
    if (len == 0) return TRUE;

    std::wstring procPath(path);
    // Skip known system processes that are never valid pin targets
    if (StrStrIW(procPath.c_str(), L"explorer.exe") ||
        StrStrIW(procPath.c_str(), L"dwm.exe")      ||
        StrStrIW(procPath.c_str(), L"csrss.exe")    ||
        StrStrIW(procPath.c_str(), L"services.exe") ||
        StrStrIW(procPath.c_str(), L"lsass.exe")    ||
        StrStrIW(procPath.c_str(), L"svchost.exe"))
        return TRUE;

    if (dist < c->bestDistance) {
        c->bestMatch    = procPath;
        c->bestDistance = dist;
    }
    return TRUE;
}

static std::wstring Resolver_Layer3_ProcessFallback(POINT pt) {
    Layer3EnumCtx ctx;
    ctx.pt           = pt;
    ctx.bestDistance = 1000;
    EnumWindows(Layer3EnumProc, (LPARAM)&ctx);
    return ctx.bestMatch;
}

// Main resolver entry point.
static std::wstring ResolveDragSourceZeroRejection(POINT pt) {
    // Phase 0: dock icons are always the primary source when clicked
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        if (g_dockLocalW > 0 && g_dockLocalH > 0) {
            int dockLeft = (int)g_dockCurrentX;
            int dockTop  = (int)g_dockCurrentY;
            if (dockLeft == 0 && g_cachedDockRect.left > 0) {
                dockLeft = g_cachedDockRect.left;
                dockTop  = g_cachedDockRect.top;
            }
            POINT local = { pt.x - dockLeft, pt.y - dockTop };
            int n = (int)g_pinnedApps.size();
            for (int i = 0; i < n; ++i) {
                if (!g_pinnedApps[i].icon) continue;
                RECT r = GetIconRectLocal(i, n);
                InflateRect(&r, 2, 2);
                if (PtInRect(&r, local)) {
                    std::wstring p = g_pinnedApps[i].exePath;
                    LeaveCriticalSection(&g_cs);
                    DragTraceLog(L"RESOLVER: dock icon", p.c_str());
                    return p;
                }
            }
        }
        LeaveCriticalSection(&g_cs);
    }

    std::wstring result;

    // -- Taskbar-authority model ----------------------------------------------
    //  When the cursor is over the taskbar, only UIAutomation (L2) is used.
    //  Active window, foreground window, and process enumeration are ALL
    //  excluded  --  they contaminate the resolved identity with the foreground
    //  app rather than the clicked taskbar button.
    //  If L2 returns empty, the resolver returns empty immediately; there is
    //  no fallback when over the taskbar.  This is immutable.
    //
    //  When the cursor is NOT over the taskbar, L2 is skipped entirely.
    //  L1 (UI hit) runs first; L3 (process enum) runs only if L1 yields empty.
    // ------------------------------------------------------------------------
    bool overTaskbar = IsCursorOverTaskbar(pt);
    if (overTaskbar) {
        // Taskbar-exclusive pipeline: UIAutomation only
        result = Resolver_Layer2_TaskbarIntelligence(pt);
        if (!result.empty())
            DragTraceLog(L"RESOLVER: L2 taskbar", result.c_str());
        // Deliberate: no L1, no L3, no active-window fallback on taskbar path
    } else {
        // Non-taskbar pipeline: UI hit -> process enum
        result = Resolver_Layer1_UIHit(pt);
        if (!result.empty()) {
            HWND testHwnd = GetRealWindowFromPoint(pt);
            if (testHwnd) {
                HWND root = GetAncestor(testHwnd, GA_ROOT);
                if (!root) root = testHwnd;
                if (IsTrueSystemWindow(root)) {
                    result.clear();  // System window  --  discard
                } else {
                    DragTraceLog(L"RESOLVER: L1 hit", result.c_str());
                }
            }
        }
        // L3 only runs off-taskbar and only as a true last resort
        if (result.empty()) {
            result = Resolver_Layer3_ProcessFallback(pt);
            if (!result.empty())
                DragTraceLog(L"RESOLVER: L3 process enum", result.c_str());
        }
    }

    if (result.empty()) {
        static DWORD s_lastMissLog = 0;
        if (GetTickCount() - s_lastMissLog > 1000) {
            Wh_Log(L"[RESOLVER MISS] all layers failed at (%d,%d)", pt.x, pt.y);
            s_lastMissLog = GetTickCount();
        }
        return L"";
    }

    // Post-resolution validation
    if (IsExcludedApp(result)) {
        DragTraceLog(L"RESOLVER: excluded", result.c_str());
        return L"";
    }
    if (StrStrIW(result.c_str(), L"ApplicationFrameHost.exe")) {
        DragTraceLog(L"RESOLVER: unresolved UWP host rejected");
        return L"";
    }

    // Require an extractable icon  --  no ghost pins
    HICON testIcon = LoadAppIconStrict(result);
    if (!testIcon) {
        DragTraceLog(L"RESOLVER: no icon  --  rejected", result.c_str());
        return L"";
    }
    DestroyIcon(testIcon);

    DragTraceLog(L"RESOLVER: OK", result.c_str());
    return result;
}

// Alias kept for call-site readability in the worker thread
static inline std::wstring ResolveDragSourceAtPoint(POINT pt) {
    return ResolveDragSourceZeroRejection(pt);
}

// ============================================================
//  VISUAL FEEDBACK
// ============================================================
static void TriggerLimitFlash() {
    g_limitFlashActive = true;
    g_limitFlashStart  = GetTickCount();
    // Shake the whole dock left-right  --  explicit, non-silent feedback
    g_shakeActive = true;
    g_shakeStart  = g_limitFlashStart;
}

// ============================================================
//  PIN
// ============================================================
// FIX-2: IsPinned MUST be called while g_cs is held by the caller.
// Calling it outside the lock while another thread erases elements is UB.
bool IsPinned(const std::wstring& p) {
    for (const auto& app : g_pinnedApps)
        if (_wcsicmp(app.exePath.c_str(), p.c_str()) == 0) return true;
    return false;
}

void PinApp(const std::wstring& path) {
    if (path.empty())        { DragTraceLog(L"PIN REJECT: empty path"); return; }
    if (IsExcludedApp(path)) { DragTraceLog(L"PIN REJECT: excluded", path.c_str()); return; }
    // FIX-2: IsPinned check moved INSIDE the CS section below (see TOCTOU comment there).
    // Do NOT call IsPinned here  --  g_pinnedApps must not be read without the lock.

    // Belt-and-suspenders: block specific system executables by name
    const wchar_t* exe = wcsrchr(path.c_str(), L'\\');
    if (exe) {
        exe++;
        if (_wcsicmp(exe, L"explorer.exe")              == 0 ||
            _wcsicmp(exe, L"ApplicationFrameHost.exe")   == 0 ||
            _wcsicmp(exe, L"SearchHost.exe")             == 0 ||
            _wcsicmp(exe, L"ShellExperienceHost.exe")    == 0 ||
            _wcsicmp(exe, L"StartMenuExperienceHost.exe") == 0) {
            DragTraceLog(L"PIN REJECT: system exe", exe);
            return;
        }
    }

    // Load a fresh icon from disk  --  never reuse ghost or cached icons
    HICON icon = LoadAppIconStrict(path);
    if (!icon) { DragTraceLog(L"PIN REJECT: no icon", path.c_str()); return; }

    // Validate the icon handle is functional
    ICONINFO ii = {};
    if (!GetIconInfo(icon, &ii)) {
        DestroyIcon(icon);
        DragTraceLog(L"PIN REJECT: invalid icon", path.c_str());
        return;
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);

    EnterCriticalSection(&g_cs);

    // FIX-2: Re-check for duplicates inside the CS (TOCTOU guard).
    // Two concurrent PinApp calls  --  e.g. worker-thread DROP and main-thread hotkey  -- 
    // could both pass the pre-lock check and push the same app twice.
    // The re-check here, under the lock, guarantees exactly-once insertion.
    if (IsPinned(path)) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        DragTraceLog(L"PIN REJECT (duplicate, inside CS): already pinned", path.c_str());
        return;
    }

    int cap = std::min(MAX_PINNED_APPS, std::max(MaxIconsFit(), MAX_PINNED_APPS));
    if ((int)g_pinnedApps.size() >= cap) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        TriggerLimitFlash();
        DEBUG_LOG(L"PIN BLOCKED limit=%d: %s", cap, path.c_str());
        return;
    }

    PinnedApp app;
    app.exePath  = path;
    app.icon     = icon;
    app.opacity  = 0.f;
    app.isNew    = true;
    app.velocityX = 0.f;

    int n       = (int)g_pinnedApps.size();
    RECT r      = GetIconRectLocal(n, n + 1);
    app.targetX  = (float)r.left;
    app.currentX = app.targetX + PIN_SLIDE_OFFSET;  // Slide in from right

    g_pinnedApps.push_back(app);

    // Recalculate all targets after insertion
    int total = (int)g_pinnedApps.size();
    for (int i = 0; i < total; ++i) {
        RECT ri = GetIconRectLocal(i, total);
        g_pinnedApps[i].targetX = (float)ri.left;
    }

    LeaveCriticalSection(&g_cs);
    SavePinnedApps();  // Outside CS  --  registry I/O must not block the input loop

    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);

    DEBUG_LOG(L"PIN OK: %s (total=%d)", path.c_str(), (int)g_pinnedApps.size());
}

// ============================================================
//  GHOST DRAG STATE CLEANUP
//  Two levels:
//    GhostDragReset()  --  resets ONLY drag-ghost fields (no drag-state change)
//    GhostCleanup()    --  full drag reset including state -> IDLE
// ============================================================
static void GhostDragReset() {
    if (g_dragGhostIcon) {
        DestroyIcon(g_dragGhostIcon);
        g_dragGhostIcon = NULL;
    }
    g_draggedAppPath.clear();
    g_lockedDragPath.clear();
    g_dragFromDock    = false;
    g_dragFromDockIdx = -1;
    g_dropZoneActive  = false;
}

static void GhostCleanup() {
    GhostDragReset();
    g_dragState = DRAG_IDLE;
}

// ============================================================
//  HARD STATE RESET  --  called after unpin to guarantee clean UI state
// ============================================================
static void HardStateReset() {
    g_hoverIndex      = -1;
    g_dragFromDockIdx = -1;
    g_dragState       = DRAG_IDLE;
    g_dragFromDock    = false;
    g_dropZoneActive  = false;
    g_rapidClickCount = 0;
    g_rapidClickIndex = -1;
    g_rapidClickStart = 0;
    g_launchCount     = 0;
    g_launchIndex     = -1;
    g_lastLaunchTime  = 0;

    // Destroy the ghost icon directly without calling GhostCleanup() to
    // avoid the g_dragState write conflicting with an in-progress drop.
    if (g_dragGhostIcon) {
        DestroyIcon(g_dragGhostIcon);
        g_dragGhostIcon = NULL;
    }
    g_draggedAppPath.clear();
    g_lockedDragPath.clear();
    g_dropZoneActive = false;

    // Snap all icon positions to their targets and reset per-icon state
    g_reorderSrcIdx    = -1;
    g_reorderTargetIdx = -1;
    for (auto& a : g_pinnedApps) {
        a.currentX   = a.targetX;
        a.hoverScale = 1.0f;
    }
}

// ============================================================
//  UNPIN
// ============================================================
void UnpinAppByIndex(int i) {
    if (i < 0) return;
    // Unpin is blocked during any active drag to prevent mid-drag data destruction
    if (g_dragState == DRAG_DRAGGING || g_dragState == DRAG_PRESS ||
        g_dragState == DRAG_REORDER) return;

    EnterCriticalSection(&g_cs);
    if (i >= (int)g_pinnedApps.size()) {
        LeaveCriticalSection(&g_cs);
        return;
    }

    DEBUG_LOG(L"UNPIN index=%d path=%s", i, g_pinnedApps[i].exePath.c_str());

    if (g_pinnedApps[i].icon) {
        DestroyIcon(g_pinnedApps[i].icon);
        g_pinnedApps[i].icon = NULL;
    }
    g_pinnedApps.erase(g_pinnedApps.begin() + i);

    // Snap remaining icons to their new target positions (no animation after unpin)
    int afterSize = (int)g_pinnedApps.size();
    for (int j = 0; j < afterSize; ++j) {
        RECT r = GetIconRectLocal(j, afterSize);
        g_pinnedApps[j].targetX  = (float)r.left;
        g_pinnedApps[j].currentX = (float)r.left;
    }

    HardStateReset();
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();  // Outside CS  --  registry I/O must not block the input loop

    // Dock width is fixed by MAX_PINNED_APPS  --  do NOT modify g_dockLocalW here

    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);
}

void UnpinAllApps() {
    if (g_dragState == DRAG_DRAGGING || g_dragState == DRAG_PRESS ||
        g_dragState == DRAG_REORDER) return;

    EnterCriticalSection(&g_cs);
    for (auto& app : g_pinnedApps)
        if (app.icon) { DestroyIcon(app.icon); app.icon = NULL; }
    g_pinnedApps.clear();
    HardStateReset();
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();  // Outside CS  --  registry I/O must not block the input loop

    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);

    DragTraceLog(L"UNPIN ALL");
}

// ============================================================
//  LAUNCH HELPERS
// ============================================================
static void LaunchApp(int idx) {
    // Snapshot path under CS  --  the vector can be modified by an unpin on the main
    // thread (hotkey) while we are in the ShellExecuteExW call below.
    std::wstring path;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        if (idx >= 0 && idx < (int)g_pinnedApps.size())
            path = g_pinnedApps[idx].exePath;
        LeaveCriticalSection(&g_cs);
    }
    if (path.empty()) return;

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpFile = path.c_str();
    sei.nShow  = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}

struct RunningCtx { const wchar_t* path; HWND found; };
static BOOL CALLBACK FindRunningProc(HWND hwnd, LPARAM lp) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    RunningCtx* c = (RunningCtx*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return TRUE;
    wchar_t buf[MAX_PATH] = {};
    DWORD blen = MAX_PATH;
    QueryFullProcessImageNameW(h, 0, buf, &blen);
    CloseHandle(h);
    if (blen > 0 && _wcsicmp(buf, c->path) == 0) { c->found = hwnd; return FALSE; }
    return TRUE;
}

static HWND FindRunningAppWindow(const std::wstring& path) {
    RunningCtx ctx = { path.c_str(), NULL };
    EnumWindows(FindRunningProc, (LPARAM)&ctx);
    return ctx.found;
}

// Smart launch: focus if running, otherwise launch (rate-limited).
static void SmartLaunch(int idx) {
    // Snapshot the path under CS before any long-running operation.
    // A hotkey unpin on the main thread can call UnpinAppByIndex (erase) while
    // we are inside FindRunningAppWindow (EnumWindows)  --  that would reallocate
    // the vector and leave a dangling reference if we held a const& instead.
    std::wstring path;
    if (!g_csInitialized) return;
    EnterCriticalSection(&g_cs);
    if (idx < 0 || idx >= (int)g_pinnedApps.size()) {
        LeaveCriticalSection(&g_cs);
        return;
    }
    path = g_pinnedApps[idx].exePath;   // value copy  --  safe after CS release
    LeaveCriticalSection(&g_cs);

    if (path.empty()) return;

    // Check running state before any rate limiting
    HWND running = FindRunningAppWindow(path);
    if (running) {
        if (IsIconic(running)) ShowWindow(running, SW_RESTORE);
        SetWindowPos(running, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        SetForegroundWindow(running);
        SetActiveWindow(running);
        DragTraceLog(L"FOCUS: brought to front", path.c_str());
        return;
    }

    // Per-icon launch rate limiting (one launch per 800 ms)
    if (idx != g_launchIndex) {
        g_launchIndex = idx;
        g_launchCount = 0;
    }
    DWORD now = GetTickCount();
    if (now - g_lastLaunchTime > 800) g_launchCount = 0;
    if (g_launchCount >= 1) {
        DragTraceLog(L"LAUNCH BLOCKED: rate limit", path.c_str());
        return;
    }
    g_launchCount++;
    g_lastLaunchTime = now;

    DragTraceLog(L"LAUNCH: new instance", path.c_str());
    LaunchApp(idx);
}

// ============================================================
//  PERSISTENCE
// ============================================================
// SavePinnedApps  --  MUST be called outside any Critical Section.
// It acquires the CS briefly to snapshot the path list, then releases before
// doing registry I/O. This prevents blocking the animation/input loop during
// potentially slow registry writes.
void SavePinnedApps() {
    // Snapshot under CS  --  fast: just string copies
    std::wstring multiSz;
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    for (const auto& app : g_pinnedApps) { multiSz += app.exePath; multiSz += L'\0'; }
    if (g_csInitialized) LeaveCriticalSection(&g_cs);
    multiSz += L'\0';

    // Registry write  --  outside CS  --  slow I/O won't block the UI/input loop
    HKEY hKey = NULL;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, NULL,
                        REG_OPTION_NON_VOLATILE, KEY_SET_VALUE,
                        NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, REG_VALUE, 0, REG_MULTI_SZ,
                       (const BYTE*)multiSz.c_str(),
                       (DWORD)(multiSz.size() * sizeof(wchar_t)));
        RegCloseKey(hKey);
    }
}

// Remove invalid entries from the list.
// Must be called WITHOUT the critical section held.
static void ValidateAndCleanPinnedList() {
    EnterCriticalSection(&g_cs);
    for (int i = (int)g_pinnedApps.size() - 1; i >= 0; --i) {
        bool invalid = !g_pinnedApps[i].icon
                    || g_pinnedApps[i].exePath.empty()
                    || IsExcludedApp(g_pinnedApps[i].exePath);
        if (invalid) {
            if (g_pinnedApps[i].icon) DestroyIcon(g_pinnedApps[i].icon);
            g_pinnedApps.erase(g_pinnedApps.begin() + i);
            DEBUG_LOG(L"VALIDATE: removed invalid entry at index %d", i);
        }
    }
    LeaveCriticalSection(&g_cs);
}

void LoadPinnedApps() {
    g_pinnedApps.clear();

    HKEY hKey = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return;

    DWORD dataSize = 0;
    if (RegQueryValueExW(hKey, REG_VALUE, NULL, NULL, NULL, &dataSize) == ERROR_SUCCESS
        && dataSize > sizeof(wchar_t)) {

        std::vector<BYTE> buf(dataSize);
        if (RegQueryValueExW(hKey, REG_VALUE, NULL, NULL,
                             buf.data(), &dataSize) == ERROR_SUCCESS) {
            const wchar_t* p = (const wchar_t*)buf.data();

            // At load time, dock geometry may not be finalised yet.
            // Fall back to MAX_PINNED_APPS as the cap when MaxIconsFit() returns 0.
            int cap = MAX_PINNED_APPS;
            if (g_dockLocalW > 0) {
                int fit = MaxIconsFit();
                if (fit > 0) cap = std::min(MAX_PINNED_APPS, fit);
            }

            while (*p && (int)g_pinnedApps.size() < cap) {
                std::wstring exePath = p;
                p += wcslen(p) + 1;

                if (IsExcludedApp(exePath)) continue;

                HICON icon = LoadAppIconStrict(exePath);
                if (!icon) {
                    DEBUG_LOG(L"LOAD SKIP (no icon): %s", exePath.c_str());
                    continue;
                }

                PinnedApp app;
                app.exePath   = exePath;
                app.icon      = icon;
                app.opacity   = 1.0f;   // Pre-existing pins start fully visible
                app.isNew     = false;  // No entrance animation
                app.velocityX = 0.f;
                g_pinnedApps.push_back(app);
                DEBUG_LOG(L"LOAD OK: %s", exePath.c_str());
            }
        }
    }
    RegCloseKey(hKey);

    // ValidateAndCleanPinnedList acquires the CS internally  --  must be called outside CS
    ValidateAndCleanPinnedList();

    int total = (int)g_pinnedApps.size();
    for (int i = 0; i < total; ++i) {
        RECT r = GetIconRectLocal(i, total);
        g_pinnedApps[i].currentX = g_pinnedApps[i].targetX = (float)r.left;
    }
    DEBUG_LOG(L"LOAD DONE: %d apps", total);
}

// ============================================================
//  GHOST DRAG WINDOW  --  pre-multiplied alpha DIB for UpdateLayeredWindow
// ============================================================
static void InitGhostDIB() {
    if (g_ghostDIB) { DeleteObject(g_ghostDIB); g_ghostDIB = NULL; }
    g_ghostBits = NULL;

    BITMAPINFO bi             = {};
    bi.bmiHeader.biSize       = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth      = GHOST_SIZE;
    bi.bmiHeader.biHeight     = -GHOST_SIZE;  // Top-down
    bi.bmiHeader.biPlanes     = 1;
    bi.bmiHeader.biBitCount   = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(NULL);
    if (hdc) {
        g_ghostDIB = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS,
                                      (void**)&g_ghostBits, NULL, 0);
        ReleaseDC(NULL, hdc);
    }
    if (g_ghostBits)
        memset(g_ghostBits, 0, (size_t)(GHOST_SIZE * GHOST_SIZE * 4));
}

// Pre-multiply the RGB channels of a 32-bpp BGRA buffer by the alpha channel.
// UpdateLayeredWindow with ULW_ALPHA requires pre-multiplied alpha.
static void PremultiplyAlpha(BYTE* bits, int pixelCount) {
    for (int i = 0; i < pixelCount; ++i) {
        BYTE* px = bits + i * 4;
        BYTE  a  = px[3];
        px[0] = (BYTE)(px[0] * a / 255);
        px[1] = (BYTE)(px[1] * a / 255);
        px[2] = (BYTE)(px[2] * a / 255);
    }
}

static void UpdateGhostWindow(POINT cursorPt) {
    if (!g_ghostWnd || g_dragState != DRAG_DRAGGING || !g_ghostBits || !g_ghostDIB) {
        if (g_ghostWnd) ShowWindow(g_ghostWnd, SW_HIDE);
        return;
    }
    if (IsExcludedApp(g_draggedAppPath) || !g_dragGhostIcon) {
        ShowWindow(g_ghostWnd, SW_HIDE);
        return;
    }
    // Only show ghost when cursor is near the dock / taskbar region
    if (!IsCursorInDockOrTaskbarRegion(cursorPt)) {
        ShowWindow(g_ghostWnd, SW_HIDE);
        return;
    }

    // Validate icon handle  --  only real icons reach the ghost window.
    // GetIconInfo is a lightweight OS call that confirms the handle refers to a
    // valid icon; a stale or garbage handle returns FALSE without crashing.
    {
        ICONINFO ii = {};
        if (!GetIconInfo(g_dragGhostIcon, &ii)) {
            ShowWindow(g_ghostWnd, SW_HIDE);
            return;
        }
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask)  DeleteObject(ii.hbmMask);
    }

    // Draw icon into the DIB buffer
    memset(g_ghostBits, 0, (size_t)(GHOST_SIZE * GHOST_SIZE * 4));
    {
        HDC mdc = CreateCompatibleDC(NULL);
        if (mdc) {
            HBITMAP old = (HBITMAP)SelectObject(mdc, g_ghostDIB);
            DrawIconEx(mdc, 0, 0, g_dragGhostIcon, GHOST_SIZE, GHOST_SIZE, 0, NULL, DI_NORMAL);
            SelectObject(mdc, old);
            DeleteDC(mdc);
        }
    }

    // Pre-multiply alpha so UpdateLayeredWindow composites correctly
    PremultiplyAlpha(g_ghostBits, GHOST_SIZE * GHOST_SIZE);

    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    HDC mdc2 = CreateCompatibleDC(sdc);
    if (!mdc2) { ReleaseDC(NULL, sdc); return; }

    HBITMAP old2 = (HBITMAP)SelectObject(mdc2, g_ghostDIB);
    POINT dest   = { cursorPt.x - GHOST_SIZE / 2, cursorPt.y - GHOST_SIZE / 2 - 6 };
    SIZE  sz     = { GHOST_SIZE, GHOST_SIZE };
    POINT src    = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 190, AC_SRC_ALPHA };

    SetWindowPos(g_ghostWnd, HWND_TOPMOST,
                 dest.x, dest.y, GHOST_SIZE, GHOST_SIZE,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateLayeredWindow(g_ghostWnd, sdc, &dest, &sz, mdc2, &src, 0, &bf, ULW_ALPHA);

    SelectObject(mdc2, old2);
    DeleteDC(mdc2);
    ReleaseDC(NULL, sdc);
}

// ============================================================
//  OVERLAY WNDPROC
// ============================================================
LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    // Hit-test: return HTCLIENT over icons so we receive mouse events there,
    // HTTRANSPARENT everywhere else so the taskbar gets input.
    case WM_NCHITTEST: {
        POINT sp = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        return HitTestIcon(sp) >= 0 ? HTCLIENT : HTTRANSPARENT;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (!hdc) return 0;

        if (!g_csInitialized) { EndPaint(hwnd, &ps); return 0; }

        EnterCriticalSection(&g_cs);

        // Fill background with the colour-key RGB(1,0,1) so it becomes transparent.
        // RGB(0,0,0) was previously the key but caused the black-rectangle bug:
        // near-black glass gradient pixels were incorrectly keyed out.
        RECT cr = {};
        GetClientRect(hwnd, &cr);
        if (!g_blackBrush) g_blackBrush = CreateSolidBrush(RGB(1, 0, 1));
        FillRect(hdc, &cr, g_blackBrush);

        if (g_dockLocalW <= 0) {
            // Dock geometry not ready yet  --  paint nothing
            static DWORD s_lastPaintErr = 0;
            DWORD now = GetTickCount();
            if (now - s_lastPaintErr > 5000) {
                DEBUG_LOG(L"PAINT: early exit  --  dock width <= 0");
                s_lastPaintErr = now;
            }
            LeaveCriticalSection(&g_cs);
            EndPaint(hwnd, &ps);
            return 0;
        }

        int n = (int)g_pinnedApps.size();
        if (n == 0) {
            // Empty dock: draw subtle glass background + separator so the dock has
            // a visible presence even before any apps are pinned.
            // Without this the entire client area is black (= transparent colour-key)
            // and the dock is completely invisible, leaving users unable to tell
            // whether the mod loaded at all.
            if (ENABLE_GLASS_OVERLAY) {
                TRIVERTEX tv[2] = {};
                tv[0].x = cr.left;  tv[0].y = cr.top;
                tv[1].x = cr.right; tv[1].y = cr.bottom;
                // Brighter blue-gray: clearly visible as a frosted glass strip,
                // not near-black. Chosen so it's never close to RGB(1,0,1) key.
                tv[0].Red = 30<<8; tv[0].Green = 32<<8; tv[0].Blue = 48<<8;
                tv[1].Red = 18<<8; tv[1].Green = 20<<8; tv[1].Blue = 34<<8;
                GRADIENT_RECT gr = { 0, 1 };
                GradientFill(hdc, tv, 2, &gr, 1, GRADIENT_FILL_RECT_H);
            }
            if (SEPARATOR_OPACITY > 0) {
                if (!g_linePenNormal) g_linePenNormal = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
                HPEN oldP = (HPEN)SelectObject(hdc, g_linePenNormal);
                MoveToEx(hdc, cr.right - 1, cr.top    + 3, NULL);
                LineTo  (hdc, cr.right - 1, cr.bottom - 3);
                SelectObject(hdc, oldP);
            }
            g_hoverIndex       = -1;
            g_dragFromDockIdx  = -1;
            LeaveCriticalSection(&g_cs);
            EndPaint(hwnd, &ps);
            return 0;
        }

        // ---- Premium glass gradient tint ----
        // Pure GDI GradientFill  --  single scanline op, near-zero CPU overhead.
        // RGB(1,0,1) is the LWA_COLORKEY transparent colour; these values are
        // deliberately non-zero so the gradient is visible as a subtle dock tint.
        if (ENABLE_GLASS_OVERLAY) {
            TRIVERTEX tv[2] = {};
            tv[0].x = cr.left;  tv[0].y = cr.top;
            tv[1].x = cr.right; tv[1].y = cr.bottom;
            if (g_limitFlashActive) {
                // Red wash -- dock-full feedback (boosted to be clearly visible)
                tv[0].Red = 90<<8; tv[0].Green = 20<<8; tv[0].Blue = 20<<8;
                tv[1].Red = 55<<8; tv[1].Green = 10<<8; tv[1].Blue = 10<<8;
            } else if (g_dropZoneActive && g_dragState == DRAG_DRAGGING) {
                // Green wash -- valid drop target (boosted from near-black)
                tv[0].Red = 20<<8; tv[0].Green = 65<<8; tv[0].Blue = 25<<8;
                tv[1].Red = 12<<8; tv[1].Green = 40<<8; tv[1].Blue = 15<<8;
            } else if (g_dragState == DRAG_REORDER) {
                // Amber wash -- reorder in progress (boosted from near-black)
                tv[0].Red = 60<<8; tv[0].Green = 45<<8; tv[0].Blue = 10<<8;
                tv[1].Red = 35<<8; tv[1].Green = 28<<8; tv[1].Blue =  6<<8;
            } else {
                // Normal: blue-gray frosted-glass tint, clearly visible on taskbar.
                // Values well above RGB(1,0,1) key so they are never keyed out.
                tv[0].Red = 30<<8; tv[0].Green = 32<<8; tv[0].Blue = 48<<8;
                tv[1].Red = 18<<8; tv[1].Green = 20<<8; tv[1].Blue = 34<<8;
            }
            GRADIENT_RECT gr = { 0, 1 };
            GradientFill(hdc, tv, 2, &gr, 1, GRADIENT_FILL_RECT_H);
        }

        // Cursor position  --  used by icon loop hover scale, reorder visual, and separator
        POINT cursorPt;
        GetCursorPos(&cursorPt);

        // Shake offset  --  sinusoidal left-right displacement, decays to zero over SHAKE_DURATION_MS.
        // Computed once per paint; added to every icon's baseX so the whole dock shakes together.
        int shakeOffset = 0;
        if (g_shakeActive) {
            int elapsed = (int)(GetTickCount() - g_shakeStart);
            if (elapsed < SHAKE_DURATION_MS) {
                float t = (float)elapsed / (float)SHAKE_DURATION_MS;
                shakeOffset = (int)(SHAKE_AMPLITUDE_PX * sinf(t * 3.14159f * 5.f) * (1.f - t));
            } else {
                g_shakeActive = false;
            }
            InvalidateRect(hwnd, NULL, FALSE);  // keep animating until done
        }

        // ---- Render each icon ----
        // Icons fading in are drawn at ICON_SIZE with alpha-blend (buffer-size stable).
        // Fully opaque icons are drawn at ICON_SIZE * app.hoverScale, centred on their slot,
        // giving a smooth spring-based hover effect with no separate redraw pass.
        for (int i = 0; i < n; ++i) {
            const auto& app = g_pinnedApps[i];
            if (!app.icon) continue;

            // Visual detachment: hide the icon being dragged from the dock
            if (g_dragFromDock && i == g_dragFromDockIdx && g_dragState == DRAG_DRAGGING)
                continue;

            if (app.opacity <= 0.01f) continue;

            RECT r     = GetIconRectLocal(i, n);
            int  baseX = (int)(app.currentX != 0.f ? app.currentX : r.left) + shakeOffset;
            if (baseX < DOCK_PAD_LEFT) baseX = r.left + shakeOffset;
            if (baseX < 0) continue;

            if (app.opacity < 1.0f) {
                // ---- Fade-in: draw at base ICON_SIZE using cached alpha-blend buffer ----
                int sz = ICON_SIZE;
                if (!g_alphaBlendDC || g_alphaBlendSize != sz) {
                    if (g_alphaBlendDC)  DeleteDC(g_alphaBlendDC);
                    if (g_alphaBlendBmp) DeleteObject(g_alphaBlendBmp);
                    g_alphaBlendBits = NULL;
                    g_alphaBlendSize = sz;

                    BITMAPINFO bi         = {};
                    bi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
                    bi.bmiHeader.biWidth  = sz;
                    bi.bmiHeader.biHeight = -sz;
                    bi.bmiHeader.biPlanes = 1;
                    bi.bmiHeader.biBitCount    = 32;
                    bi.bmiHeader.biCompression = BI_RGB;

                    HDC screenDC = GetDC(NULL);
                    if (screenDC) {
                        g_alphaBlendBmp = CreateDIBSection(
                            screenDC, &bi, DIB_RGB_COLORS,
                            (void**)&g_alphaBlendBits, NULL, 0);
                        g_alphaBlendDC = CreateCompatibleDC(screenDC);
                        if (g_alphaBlendDC && g_alphaBlendBmp)
                            SelectObject(g_alphaBlendDC, g_alphaBlendBmp);
                        ReleaseDC(NULL, screenDC);
                    }
                }
                if (g_alphaBlendDC && g_alphaBlendBmp && g_alphaBlendBits) {
                    memset(g_alphaBlendBits, 0, (size_t)(sz * sz * 4));
                    DrawIconEx(g_alphaBlendDC, 0, 0, app.icon, sz, sz, 0, NULL, DI_NORMAL);
                    // AC_SRC_ALPHA requires pre-multiplied alpha in the source DIB.
                    // DrawIconEx writes straight alpha -- premultiply before blending.
                    PremultiplyAlpha(g_alphaBlendBits, sz * sz);
                    BLENDFUNCTION bf = {
                        AC_SRC_OVER, 0,
                        (BYTE)(int)(255 * app.opacity),
                        AC_SRC_ALPHA
                    };
                    AlphaBlend(hdc, baseX, r.top, sz, sz,
                               g_alphaBlendDC, 0, 0, sz, sz, bf);
                }
            } else {
                // ---- Full opacity: draw at animated hover scale, centred on slot ----
                float scale  = (app.hoverScale > 1.0f) ? app.hoverScale : 1.0f;
                int   sz     = (int)(ICON_SIZE * scale);
                int   offset = (sz - ICON_SIZE) / 2;
                int   x      = std::max(0, baseX - offset);
                int   y      = r.top - offset;
                DrawIconEx(hdc, x, y, app.icon, sz, sz, 0, NULL, DI_NORMAL);
            }
        }

        // ---- Running-state indicator dots ----
        // A small dot is drawn below each icon of a currently running app.
        // Style mirrors Windows 11: centred below icon, white-blue tint, ~3px diameter.
        // g_runDotBrush is cached after first paint; freed in Wh_ModUninit.
        if (!g_runDotBrush) g_runDotBrush = CreateSolidBrush(RGB(200, 210, 235));
        if (g_runDotBrush) {
            HGDIOBJ oldBr  = SelectObject(hdc, g_runDotBrush);
            HGDIOBJ oldPen = SelectObject(hdc, GetStockObject(NULL_PEN));
            for (int i = 0; i < n; ++i) {
                const auto& app = g_pinnedApps[i];
                if (!app.running || !app.icon) continue;
                if (app.opacity < 0.5f) continue;
                if (g_dragFromDock && i == g_dragFromDockIdx &&
                    g_dragState == DRAG_DRAGGING) continue;
                RECT r    = GetIconRectLocal(i, n);
                int baseX = (int)(app.currentX > 0.f ? app.currentX : r.left);
                int cx    = baseX + ICON_SIZE / 2;
                int dotSz = 3;
                int dotY  = r.bottom + 2;
                if (dotY + dotSz <= g_dockLocalH)
                    Ellipse(hdc, cx - 1, dotY, cx + 2, dotY + dotSz);
            }
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBr);
        }

        // ---- Reorder drag: draw dragged icon floating at cursor (semi-transparent) ----
        if (g_dragState == DRAG_REORDER &&
            g_reorderSrcIdx >= 0 && g_reorderSrcIdx < n) {
            const auto& src = g_pinnedApps[g_reorderSrcIdx];
            if (src.icon) {
                int sz = ICON_SIZE;
                // Ensure alpha-blend buffer exists (all-opaque icon path doesn't create it)
                if (!g_alphaBlendDC || g_alphaBlendSize != sz) {
                    if (g_alphaBlendDC)  { DeleteDC(g_alphaBlendDC);     g_alphaBlendDC  = NULL; }
                    if (g_alphaBlendBmp) { DeleteObject(g_alphaBlendBmp); g_alphaBlendBmp = NULL; }
                    g_alphaBlendBits = NULL;
                    g_alphaBlendSize = sz;
                    BITMAPINFO bi = {};
                    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
                    bi.bmiHeader.biWidth       = sz;
                    bi.bmiHeader.biHeight      = -sz;
                    bi.bmiHeader.biPlanes      = 1;
                    bi.bmiHeader.biBitCount    = 32;
                    bi.bmiHeader.biCompression = BI_RGB;
                    HDC sdc = GetDC(NULL);
                    if (sdc) {
                        g_alphaBlendBmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS,
                                                           (void**)&g_alphaBlendBits, NULL, 0);
                        if (g_alphaBlendBmp) {
                            g_alphaBlendDC = CreateCompatibleDC(sdc);
                            if (g_alphaBlendDC) SelectObject(g_alphaBlendDC, g_alphaBlendBmp);
                            else { DeleteObject(g_alphaBlendBmp); g_alphaBlendBmp = NULL; }
                        }
                        ReleaseDC(NULL, sdc);
                    }
                }
                if (g_alphaBlendDC && g_alphaBlendBits) {
                    memset(g_alphaBlendBits, 0, (size_t)(sz * sz * 4));
                    DrawIconEx(g_alphaBlendDC, 0, 0, src.icon, sz, sz, 0, NULL, DI_NORMAL);
                    // Centre icon on cursor, clamped to dock client area
                    int localX = cursorPt.x - (int)g_dockCurrentX - sz / 2;
                    int localY = (g_dockLocalH - sz) / 2;
                    localX = std::max((int)cr.left, std::min(localX, (int)cr.right - sz));
                    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 195, AC_SRC_ALPHA };
                    AlphaBlend(hdc, localX, localY, sz, sz,
                               g_alphaBlendDC, 0, 0, sz, sz, bf);
                }
            }
        }

        // ---- Interactive visuals (only when cursor is in dock/taskbar area) ----
        bool cursorInRegion = IsCursorInDockOrTaskbarRegion(cursorPt);

        if (cursorInRegion && n > 0) {
            // Separator / drop-zone indicator line on the right edge
            if (SEPARATOR_OPACITY > 0) {
                GetClientRect(hwnd, &cr);
                bool inDrop = IsInDockZone(cursorPt, 8);

                // When glass overlay is active, the dock itself tints red for limit
                // flash  --  the separator reverts to neutral so we don't double-signal.
                COLORREF lineColor = (g_limitFlashActive && !ENABLE_GLASS_OVERLAY)
                                                               ? RGB(210,  30,  30)
                                   : (inDrop && g_dragState == DRAG_DRAGGING)
                                                               ? RGB( 30, 180,  80)
                                                               : RGB( 80,  80,  80);

                // Ensure cached pens exist for each colour
                if (!g_linePenNormal) g_linePenNormal = CreatePen(PS_SOLID, 1, RGB( 80,  80,  80));
                if (!g_linePenFlash)  g_linePenFlash  = CreatePen(PS_SOLID, 1, RGB(210,  30,  30));
                if (!g_linePenDrop)   g_linePenDrop   = CreatePen(PS_SOLID, 1, RGB( 30, 180,  80));

                HPEN penToUse = (lineColor == RGB(210, 30, 30)) ? g_linePenFlash
                              : (lineColor == RGB( 30,180, 80)) ? g_linePenDrop
                              : g_linePenNormal;

                int alpha = (255 * SEPARATOR_OPACITY) / 100;
                if (alpha >= 255) {
                    // Full opacity  --  direct draw
                    HPEN old = (HPEN)SelectObject(hdc, penToUse);
                    MoveToEx(hdc, cr.right - 1, cr.top    + 3, NULL);
                    LineTo  (hdc, cr.right - 1, cr.bottom - 3);
                    SelectObject(hdc, old);
                } else if (alpha > 0) {
                    // Partial opacity  --  alpha blend a 1-px wide DIB column.
                    // The DIB is CACHED across frames (g_sepDC/g_sepDIB/g_sepBits)
                    // and rebuilt only when the separator height changes (DPI / taskbar resize).
                    // This eliminates per-frame CreateDIBSection/DeleteObject overhead.
                    int lineH = cr.bottom - cr.top - 6;
                    if (lineH > 0) {
                        // Rebuild cache only when height changes
                        if (lineH != g_sepCachedH || !g_sepDC || !g_sepDIB) {
                            if (g_sepDIB) { SelectObject(g_sepDC, (HBITMAP)NULL); DeleteObject(g_sepDIB); g_sepDIB = NULL; }
                            if (g_sepDC)  { DeleteDC(g_sepDC); g_sepDC = NULL; }
                            g_sepBits    = NULL;
                            g_sepCachedH = 0;

                            BITMAPINFO bi         = {};
                            bi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
                            bi.bmiHeader.biWidth  = 1;
                            bi.bmiHeader.biHeight = lineH;
                            bi.bmiHeader.biPlanes = 1;
                            bi.bmiHeader.biBitCount    = 32;
                            bi.bmiHeader.biCompression = BI_RGB;

                            HDC screenDC = GetDC(NULL);
                            if (screenDC) {
                                g_sepDC  = CreateCompatibleDC(screenDC);
                                g_sepDIB = CreateDIBSection(screenDC, &bi, DIB_RGB_COLORS,
                                                            (void**)&g_sepBits, NULL, 0);
                                ReleaseDC(NULL, screenDC);
                            }
                            if (g_sepDC && g_sepDIB) {
                                SelectObject(g_sepDC, g_sepDIB);
                                g_sepCachedH = lineH;
                            }
                        }

                        if (g_sepDC && g_sepDIB && g_sepBits && g_sepCachedH == lineH) {
                            // Repaint pixel data (colour may change per frame)
                            memset(g_sepBits, 0, (size_t)(lineH * 4));
                            HPEN oldPen = (HPEN)SelectObject(g_sepDC, penToUse);
                            MoveToEx(g_sepDC, 0, 0,      NULL);
                            LineTo  (g_sepDC, 0, lineH - 1);
                            SelectObject(g_sepDC, oldPen);
                            // FIX-4: AlphaBlend with AC_SRC_ALPHA requires pre-multiplied
                            // pixel data.  GDI draws the pen colour but leaves alpha=0;
                            // we must write alpha AND multiply the RGB channels by it.
                            // At alpha=255 (default, full opacity) the multiply is a no-op.
                            // Without this, partial-opacity separators appeared too bright.
                            BYTE a = (BYTE)alpha;
                            for (int j = 0; j < lineH; ++j) {
                                BYTE* px = g_sepBits + j * 4;
                                // px[0]=B, px[1]=G, px[2]=R written by GDI; px[3]=A (0)
                                px[0] = (BYTE)((px[0] * a) / 255);
                                px[1] = (BYTE)((px[1] * a) / 255);
                                px[2] = (BYTE)((px[2] * a) / 255);
                                px[3] = a;
                            }

                            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                            AlphaBlend(hdc, cr.right - 1, cr.top + 3,
                                       1, lineH, g_sepDC, 0, 0, 1, lineH, bf);
                        }
                    }
                }
            }
        }

        LeaveCriticalSection(&g_cs);
        // Keep secondary monitor docks in sync (cheap InvalidateRect calls only)
        RepaintSecondaryDocks();
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_MOUSEMOVE: {
        POINT sp;
        GetCursorPos(&sp);
        int idx = HitTestIcon(sp);
        if (idx != g_hoverIndex) {
            g_hoverIndex = idx;
            SetCursor(LoadCursor(NULL, idx >= 0 ? IDC_HAND : IDC_ARROW));
            InvalidateRect(hwnd, NULL, FALSE);
        }
        TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
        TrackMouseEvent(&tme);
        return 0;
    }

    case WM_MOUSELEAVE:
        g_hoverIndex = -1;
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_LBUTTONUP:
        // Click logic is handled entirely in the worker-thread state machine.
        // Processing it here would cause double-launch races.
        return 0;

    case WM_LBUTTONDBLCLK:
        // Double-click also routes through the worker thread (it sees the
        // second click as a PRESS->IDLE transition and calls SmartLaunch).
        // Handling it here would cause a second launch on the UI thread.
        return 0;

    case WM_RBUTTONUP: {
        // Right-click behaviour:
        //   Single right-click -> nothing (prevents accidental unpin)
        //   Double right-click within RIGHT_DOUBLE_CLICK_MS -> unpin
        static DWORD s_lastRClickTime = 0;
        static int   s_lastRClickIdx  = -1;
        DWORD nowR = GetTickCount();
        POINT sp;
        GetCursorPos(&sp);
        int idx = HitTestIcon(sp);

        if (idx >= 0) {
            bool isDouble = (nowR - s_lastRClickTime < (DWORD)RIGHT_DOUBLE_CLICK_MS)
                         && (idx == s_lastRClickIdx);
            if (isDouble) {
                UnpinAppByIndex(idx);
                s_lastRClickTime = 0;
                s_lastRClickIdx  = -1;
            } else {
                s_lastRClickTime = nowR;
                s_lastRClickIdx  = idx;
            }
        } else {
            s_lastRClickTime = 0;
            s_lastRClickIdx  = -1;
        }
        return 0;
    }

    // Worker thread posts this when secondary docks need rebuilding.
    // DestroyWindow is only safe on the thread that created the window
    // (the main thread), so it cannot be called from WorkerThread directly.
    case WM_QPD_REBUILD_SECONDARY:
        if (MULTI_MONITOR_DOCK) {
            DestroySecondaryDocks();
            InitSecondaryDocks();
        }
        return 0;

    case WM_HOTKEY:
        if ((UINT)wParam == HOTKEY_PIN_ID) {
            // Hotkey handler: pin the currently focused app, or unpin it if already pinned.
            // Uses the same validation pipeline as drag  --  not a raw active-window shortcut.
            HWND fg = GetForegroundWindow();
            if (fg && !IsSystemWindow(fg)) {
                std::wstring path = GetProcessPath(fg);
                if (!path.empty() && !IsExcludedApp(path)) {
                    EnterCriticalSection(&g_cs);
                    bool pinned = IsPinned(path);
                    int  idx    = -1;
                    if (pinned)
                        for (int i = 0; i < (int)g_pinnedApps.size(); ++i)
                            if (_wcsicmp(g_pinnedApps[i].exePath.c_str(), path.c_str()) == 0)
                                { idx = i; break; }
                    LeaveCriticalSection(&g_cs);
                    if (idx >= 0) {
                        UnpinAppByIndex(idx);
                        DragTraceLog(L"HOTKEY: unpin", path.c_str());
                    } else {
                        PinApp(path);
                        DragTraceLog(L"HOTKEY: pin", path.c_str());
                    }
                }
            }
        }
        return 0;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

// ============================================================
//  WINDOW CREATION
// ============================================================
static bool CreateOverlayWindow() {
    WNDCLASSEXW wc   = { sizeof(wc) };
    wc.lpfnWndProc   = OverlayProc;
    wc.hInstance     = GetModuleHandleW(NULL);
    wc.lpszClassName = OVERLAY_CLASS;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.style         = CS_DBLCLKS;
    RegisterClassExW(&wc);

    int initialW = g_dockLocalW > 0 ? g_dockLocalW : 200;
    int initialH = g_dockLocalH > 0 ? g_dockLocalH : 48;

    // WS_EX_TRANSPARENT: overlay never captures mouse outside of icon hit-test areas
    g_overlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
        OVERLAY_CLASS, L"",
        WS_POPUP,
        0, 0, initialW, initialH,
        NULL, NULL, wc.hInstance, NULL);

    if (!g_overlayWnd) {
        DEBUG_LOG(L"OVERLAY: CreateWindow failed err=%lu", GetLastError());
        return false;
    }

    // Colour-key for transparency: RGB(1,0,1) magenta.
    // Previously RGB(0,0,0) black was used, but the glass gradient paints
    // near-black values that got incorrectly keyed out -- making the dock
    // appear as a solid black rectangle instead of transparent.
    // RGB(1,0,1) is never produced by the glass gradient or icon rendering,
    // so only the true background pixels become transparent.
    SetLayeredWindowAttributes(g_overlayWnd, RGB(1, 0, 1), 0, LWA_COLORKEY);

    // Register the global pin/unpin hotkey (Ctrl+Alt+P by default).
    // hotkeyKey == 0 or hotkeyMods == 0 means disabled by user.
    // Guard against mods==0: RegisterHotKey with no modifiers intercepts the bare
    // key globally in every application, which is almost never intentional.
    if (g_hotkeyKey != 0 && g_hotkeyMods != 0)
        RegisterHotKey(g_overlayWnd, HOTKEY_PIN_ID, g_hotkeyMods, g_hotkeyKey);

    DEBUG_LOG(L"OVERLAY: created hwnd=%p hotkey=mods:0x%X key:0x%X",
              g_overlayWnd, g_hotkeyMods, g_hotkeyKey);
    return true;
}

static bool CreateGhostWindow() {
    WNDCLASSEXW gw   = { sizeof(gw) };
    gw.lpfnWndProc   = DefWindowProcW;
    gw.hInstance     = GetModuleHandleW(NULL);
    gw.lpszClassName = GHOST_CLASS;
    RegisterClassExW(&gw);

    g_ghostWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE |
        WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        GHOST_CLASS, L"",
        WS_POPUP,
        0, 0, GHOST_SIZE, GHOST_SIZE,
        NULL, NULL, gw.hInstance, NULL);

    if (!g_ghostWnd) {
        DEBUG_LOG(L"GHOST: CreateWindow failed err=%lu", GetLastError());
        return false;
    }
    ShowWindow(g_ghostWnd, SW_HIDE);
    return true;
}

// ============================================================
//  WINEVENT HOOK  --  event-driven taskbar geometry refresh
// ============================================================
static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                   LONG idObject, LONG, DWORD, DWORD) {
    // Only care about window location and foreground changes
    if (idObject != OBJID_WINDOW && idObject != OBJID_CLIENT &&
        idObject != OBJID_CURSOR)
        return;
    if (!hwnd) return;

    // Filter quickly to taskbar-related windows only
    if (hwnd != g_cachedTaskbar) {
        wchar_t cls[32] = {};
        GetClassNameW(hwnd, cls, 32);
        if (wcsstr(cls, L"Shell_TrayWnd") == NULL &&
            wcsstr(cls, L"Start")         == NULL &&
            wcsstr(cls, L"Tray")          == NULL)
            return;
        // Only enforce the root-ancestor check when our cached taskbar HWND is
        // known.  If g_cachedTaskbar is NULL (explorer restarted, geometry not yet
        // measured) allow the event through so RefreshTaskbarCache can re-acquire it.
        if (g_cachedTaskbar != NULL && GetAncestor(hwnd, GA_ROOT) != g_cachedTaskbar)
            return;
    }

    if (event == EVENT_OBJECT_LOCATIONCHANGE || event == EVENT_SYSTEM_FOREGROUND) {
        if (HasTaskbarGeometryChanged()) {
            RefreshTaskbarCache();
            RepositionOverlay();
            // FIX-6: Rate-limit secondary dock rebuild to at most once every 2 s.
            // EVENT_OBJECT_LOCATIONCHANGE fires 30-60x/s during any drag; without a
            // guard each event tears down and recreates secondary dock HWNDs, causing
            // massive UI churn on multi-monitor systems.
            if (MULTI_MONITOR_DOCK) {
                static DWORD s_lastSecondaryRebuild = 0;
                DWORD nowRebuild = GetTickCount();
                if (nowRebuild - s_lastSecondaryRebuild >= 2000) {
                    s_lastSecondaryRebuild = nowRebuild;
                    // FIX: DestroyWindow must be called on the main thread (the
                    // thread that created the windows).  Post a message to the
                    // overlay wndproc instead of calling Destroy/Init directly.
                    if (g_overlayWnd && IsWindow(g_overlayWnd))
                        PostMessageW(g_overlayWnd, WM_QPD_REBUILD_SECONDARY, 0, 0);
                }
            }
        }
    }
}

// ============================================================
//  WORKER THREAD  --  input polling and animation loop
// ============================================================
DWORD WINAPI WorkerThread(LPVOID) {
    bool  lastLDown         = false;
    DWORD lastGeometryCheck = GetTickCount();
    DWORD bootWatchdogStart = GetTickCount();

    while (WaitForSingleObject(g_exitEvent, 0) != WAIT_OBJECT_0) {
        if (!g_csInitialized) { Sleep(16); continue; }

        POINT cursor = {};
        GetCursorPos(&cursor);
        DWORD now = GetTickCount();

        // Safety geometry poll + running-state + auto-hide check.
        // Poll every 100 ms while still in boot/stabilizing so the dock becomes
        // visible within one poll cycle after Wh_ModInit.  Once geometry is locked
        // (STATE_STABLE), drop to a relaxed 500 ms cadence to save CPU.
        DWORD pollIntervalMs = (g_systemState != STATE_STABLE) ? 100u : (DWORD)RUNNING_STATE_CHECK_MS;
        if (now - lastGeometryCheck > pollIntervalMs) {
            bool needRefresh = (g_systemState != STATE_STABLE) || HasTaskbarGeometryChanged();
            if (needRefresh) {
                RefreshTaskbarCache();
                RepositionOverlay();
                if (MULTI_MONITOR_DOCK) {
                    // FIX: secondary dock windows were created on the main thread;
                    // DestroyWindow from the worker thread is undefined behaviour.
                    // Post to the overlay wndproc to rebuild on the correct thread.
                    if (g_overlayWnd && IsWindow(g_overlayWnd))
                        PostMessageW(g_overlayWnd, WM_QPD_REBUILD_SECONDARY, 0, 0);
                }
            }
            if (ENABLE_AUTOHIDE_SYNC) UpdateAutoHideState();
            UpdateRunningState();
            if (g_overlayWnd) InvalidateRect(g_overlayWnd, NULL, FALSE);
            lastGeometryCheck = now;
        }

        // -- Hover stability tracking ------------------------------------------
        // Pre-samples the resolver at HOVER_SAMPLE_MS intervals while the cursor
        // is near the drag zone so PRESS uses a temporally stable identity
        // rather than resolving cold at click time.
        if (g_dragState == DRAG_IDLE &&
            now - g_hoverLastSample >= (DWORD)HOVER_SAMPLE_MS &&
            (IsInDockZone(cursor, 30) || IsCursorOverTaskbar(cursor))) {
            std::wstring fresh = ResolveDragSourceAtPoint(cursor);
            if (_wcsicmp(fresh.c_str(), g_hoverCandidate.c_str()) != 0) {
                g_hoverCandidate     = fresh;
                g_hoverCandidateTime = now;
            }
            g_hoverLastSample = now;
        }

        // Boot watchdog: force geometry recovery if dock width is still 0 after 100 ms
        if (g_dockLocalW == 0 && (int)(now - bootWatchdogStart) > 100) {
            DEBUG_LOG(L"WATCHDOG: forcing geometry refresh");
            RefreshTaskbarCache();
            if (g_dockLocalW > 0) {
                RepositionOverlay();
                if (g_overlayWnd && IsWindow(g_overlayWnd))
                    InvalidateRect(g_overlayWnd, NULL, FALSE);
            }
            bootWatchdogStart = now;
        }

        if (!g_overlayWnd || !IsWindow(g_overlayWnd)) { Sleep(16); continue; }

        // Re-assert HWND_TOPMOST every 3 s  --  avoids hammering DWM every frame.
        // Per-frame SetWindowPos was the single largest source of input jitter.
        {
            static DWORD s_lastTopMostMs = 0;
            if (now - s_lastTopMostMs > 3000) {
                SetWindowPos(g_overlayWnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                s_lastTopMostMs = now;
            }
        }

        bool lDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        // ================================================================
        //  DRAG STATE MACHINE
        // ================================================================

        // IDLE / CANCELLED -> PRESS (mouse down)
        if ((g_dragState == DRAG_IDLE || g_dragState == DRAG_CANCELLED) &&
            lDown && !lastLDown) {

            g_mouseDownTime   = now;
            g_dragStartPt     = cursor;
            g_dropZoneActive  = false;
            g_dragFromDock    = false;
            g_dragFromDockIdx = -1;
            g_draggedAppPath.clear();
            if (g_dragGhostIcon) { DestroyIcon(g_dragGhostIcon); g_dragGhostIcon = NULL; }

            int dockIdx = HitTestIcon(cursor);
            if (dockIdx >= 0) {
                // FIX-1: HitTestIcon acquires then releases g_cs.  The window
                // between its return and here is a dangling-index window: the
                // main thread (hotkey / right-click) may call UnpinAppByIndex
                // which erases the element and reallocates the vector  --  making
                // g_pinnedApps[dockIdx] point at freed or wrong memory.
                // Re-acquire g_cs, bounds-check, and snapshot all needed fields
                // atomically before touching the vector again.
                std::wstring snapPath;
                HICON        snapIconCopy = NULL;
                bool         validSnap    = false;

                EnterCriticalSection(&g_cs);
                if (dockIdx < (int)g_pinnedApps.size()) {
                    snapPath = g_pinnedApps[dockIdx].exePath;
                    if (g_pinnedApps[dockIdx].icon)
                        snapIconCopy = CopyIcon(g_pinnedApps[dockIdx].icon);
                    validSnap = true;
                }
                LeaveCriticalSection(&g_cs);

                if (validSnap) {
                    // Source: a pinned icon
                    g_dragFromDock    = true;
                    g_dragFromDockIdx = dockIdx;
                    g_draggedAppPath  = snapPath;
                    g_dragGhostIcon   = snapIconCopy;  // may be NULL  --  that is fine
                    g_dragState = DRAG_PRESS;
                    DragTraceLog(L"PRESS: dock icon", g_draggedAppPath.c_str());
                } else {
                    // Index was unpinned between HitTestIcon and our snapshot
                    if (snapIconCopy) DestroyIcon(snapIconCopy);
                    DragTraceLog(L"PRESS: dock index gone before snapshot  --  abort");
                }
            } else {
                // Source: taskbar button or application window.
                // Prefer the pre-confirmed hover candidate (stable across STABILITY_CONFIRM_MS)
                // over a cold resolve at press time  --  eliminates first-frame identity flicker.
                std::wstring path;
                DWORD candidateAge = now - g_hoverCandidateTime;
                if (!g_hoverCandidate.empty() &&
                    candidateAge >= (DWORD)STABILITY_CONFIRM_MS &&
                    candidateAge <= (DWORD)(HOVER_SAMPLE_MS * 6)) {
                    path = g_hoverCandidate;
                    DragTraceLog(L"PRESS: stable candidate", path.c_str());
                } else {
                    path = ResolveDragSourceAtPoint(cursor);
                    DragTraceLog(L"PRESS: fresh resolve", path.c_str());
                }
                if (path.empty()) {
                    g_dragState = DRAG_CANCELLED;
                    DragTraceLog(L"REJECT: no valid source");
                } else {
                    g_draggedAppPath = path;
                    HICON tmp = LoadAppIconStrict(path);
                    if (tmp) {
                        g_dragGhostIcon = CopyIcon(tmp);
                        DestroyIcon(tmp);
                    }
                    g_dragState = DRAG_PRESS;
                }
            }
        }

        // PRESS -> DRAG_REORDER or DRAGGING (threshold crossed)
        if (g_dragState == DRAG_PRESS && lDown) {
            int dx = abs(cursor.x - g_dragStartPt.x);
            int dy = abs(cursor.y - g_dragStartPt.y);
            if (dx > DRAG_THRESHOLD_PX || dy > DRAG_THRESHOLD_PX) {
                if (ENABLE_REORDER && g_dragFromDock && IsInDockZone(cursor)) {
                    // Cursor stayed inside dock -> enter reorder mode
                    g_reorderSrcIdx    = g_dragFromDockIdx;
                    g_reorderTargetIdx = g_dragFromDockIdx;
                    g_dragState        = DRAG_REORDER;
                    if (g_ghostWnd) ShowWindow(g_ghostWnd, SW_HIDE);
                    DragTraceLog(L"REORDER: started", g_draggedAppPath.c_str());
                } else if (!g_draggedAppPath.empty()) {
                    g_lockedDragPath = g_draggedAppPath;  // Hard lock  --  never re-resolved
                    g_dragState      = DRAG_DRAGGING;
                    DragTraceLog(L"DRAG CONFIRMED", g_draggedAppPath.c_str());
                } else {
                    g_dragState = DRAG_CANCELLED;
                    DragTraceLog(L"CANCEL: no path at threshold");
                }
            }
        }

        // DRAG_REORDER  --  update target slot and animate icons while button held
        if (g_dragState == DRAG_REORDER && lDown) {
            int n = (int)g_pinnedApps.size();
            if (n > 1 && g_reorderSrcIdx >= 0 && g_reorderSrcIdx < n) {
                int newTarget = CalculateReorderSlot(cursor, n);
                newTarget = std::max(0, std::min(newTarget, n - 1));
                if (newTarget != g_reorderTargetIdx) {
                    g_reorderTargetIdx = newTarget;
                    UpdateReorderPositions();
                }
            }
            if (g_overlayWnd) InvalidateRect(g_overlayWnd, NULL, FALSE);

            // If cursor leaves dock zone -> abort reorder, fall through to unpin drag
            if (!IsInDockZone(cursor, MAGNETIC_RANGE_PX * 3)) {
                g_dragState      = DRAG_DRAGGING;
                g_lockedDragPath = g_draggedAppPath;
                // Restore canonical positions before switching to unpin-drag
                EnterCriticalSection(&g_cs);
                int total = (int)g_pinnedApps.size();
                for (int i = 0; i < total; ++i) {
                    RECT r = GetIconRectLocal(i, total);
                    g_pinnedApps[i].targetX = (float)r.left;
                }
                g_reorderSrcIdx    = -1;
                g_reorderTargetIdx = -1;
                LeaveCriticalSection(&g_cs);
                DragTraceLog(L"REORDER->DRAG: cursor left dock zone");
            }
        }

        // PRESS -> IDLE (mouse up before threshold = click)
        if (g_dragState == DRAG_PRESS && !lDown && lastLDown) {
            DWORD elapsed = now - g_mouseDownTime;
            int   dx      = abs(cursor.x - g_dragStartPt.x);
            int   dy      = abs(cursor.y - g_dragStartPt.y);
            bool  wasClick = (elapsed < (DWORD)CLICK_MAX_MS &&
                              dx <= CLICK_MAX_MOVE_PX &&
                              dy <= CLICK_MAX_MOVE_PX);

            g_dragState = DRAG_IDLE;
            if (wasClick) {
                int clickIdx = HitTestIcon(cursor);
                if (clickIdx >= 0) SmartLaunch(clickIdx);
            }
            GhostCleanup();
        }

        // DRAG_CANCELLED auto-reset when mouse is released
        if (g_dragState == DRAG_CANCELLED && !lDown) {
            g_dragState = DRAG_IDLE;
            g_draggedAppPath.clear();
            GhostCleanup();
        }

        // DRAGGING  --  update ghost, nothing else may interrupt this state
        if (g_dragState == DRAG_DRAGGING) {
            g_dropZoneActive = IsNearDockZone(cursor);
            UpdateGhostWindow(cursor);
            if (g_dragFromDock && g_overlayWnd)
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }

        // DRAGGING -> DROPPED / CANCELLED (mouse up)
        if ((g_dragState == DRAG_DRAGGING || g_dragState == DRAG_CANCELLED) &&
            !lDown && lastLDown) {

            if (g_dragState == DRAG_DRAGGING) {
                // Evaluate drop zone using authoritative static rect (not animated position)
                RECT dropZone = g_cachedDockRect;
                InflateRect(&dropZone, 8, 8);
                bool dropped = PtInRect(&dropZone, cursor) != 0;

                if (g_dragFromDock) {
                    int lockedIdx = g_dragFromDockIdx;
                    g_dragState   = DRAG_DROPPED;
                    if (!dropped) {
                        DragTraceLog(L"DROP: off dock -> UNPIN");
                        UnpinAppByIndex(lockedIdx);
                    } else {
                        DragTraceLog(L"DROP: back on dock -> no change");
                    }
                } else {
                    std::wstring dropPath = !g_lockedDragPath.empty()
                                         ? g_lockedDragPath : g_draggedAppPath;
                    g_dragState = DRAG_DROPPED;
                    if (dropped && !dropPath.empty()) {
                        DragTraceLog(L"DROP: in zone -> PIN", dropPath.c_str());
                        PinApp(dropPath);
                    } else {
                        DragTraceLog(L"DROP: outside zone -> cancel");
                    }
                }
            }

            ShowWindow(g_ghostWnd, SW_HIDE);
            g_lockedDragPath.clear();
            GhostCleanup();

            if (g_overlayWnd)
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }

        // DRAG_REORDER -> committed (mouse released inside dock)
        if (g_dragState == DRAG_REORDER && !lDown && lastLDown) {
            CommitReorder();
            ShowWindow(g_ghostWnd, SW_HIDE);
            GhostCleanup();
            g_dragState = DRAG_IDLE;
            if (g_overlayWnd) InvalidateRect(g_overlayWnd, NULL, FALSE);
            RepaintSecondaryDocks();
        }

        // ---- RAPID-CLICK RESET (3+ clicks in RAPID_CLICK_WINDOW_MS) ----
        // Normal single-click launches are handled in PRESS->IDLE above.
        // This block only tracks rapid-click sequences for the unpin-all gesture.
        // Detection zone: dock area + a 40px leftward extension so clicks slightly
        // outside the dock (on the taskbar to the left of icons) are also counted.
        if (g_dragState == DRAG_IDLE && !lDown && lastLDown) {
            DWORD elapsed = now - g_mouseDownTime;
            int   dx      = abs(cursor.x - g_dragStartPt.x);
            int   dy      = abs(cursor.y - g_dragStartPt.y);
            bool  wasClick = (elapsed < (DWORD)CLICK_MAX_MS &&
                              dx <= CLICK_MAX_MOVE_PX &&
                              dy <= CLICK_MAX_MOVE_PX);
            if (wasClick) {
                // Use a widened hit zone for rapid-click counting: dock area plus
                // 40px extended to the left (where no icon slot is, but finger/click
                // still intends the dock).  The icon index is snapped to nearest edge.
                int clickIdx = HitTestIcon(cursor);
                if (clickIdx < 0) {
                    // Check if cursor is in the expanded dock zone (left extension)
                    bool inExpandedZone = IsInDockZone(cursor, 40);
                    if (inExpandedZone && g_csInitialized) {
                        EnterCriticalSection(&g_cs);
                        int nn = (int)g_pinnedApps.size();
                        // Snap to the leftmost icon slot for counting purposes
                        if (nn > 0) clickIdx = 0;
                        LeaveCriticalSection(&g_cs);
                    }
                }
                if (clickIdx >= 0) {
                    if (clickIdx != g_rapidClickIndex) {
                        g_rapidClickIndex = clickIdx;
                        g_rapidClickCount = 1;
                        g_rapidClickStart = now;
                    } else if (now - g_rapidClickStart > (DWORD)RAPID_CLICK_WINDOW_MS) {
                        g_rapidClickStart = now;
                        g_rapidClickCount = 1;
                    } else {
                        g_rapidClickCount++;
                    }
                    if (g_rapidClickCount >= RAPID_CLICK_THRESHOLD) {
                        UnpinAllApps();
                        g_rapidClickCount = 0;
                        g_rapidClickStart = 0;
                        g_rapidClickIndex = -1;
                    }
                }
            }
        }

        // Hide ghost when drag is not active
        if (g_dragState != DRAG_DRAGGING && g_ghostWnd && IsWindowVisible(g_ghostWnd))
            ShowWindow(g_ghostWnd, SW_HIDE);

        // ================================================================
        //  TIME-BASED FRAME DELTA
        // ================================================================
        DWORD frameNow = GetTickCount();
        if (g_lastFrameTime == 0) g_lastFrameTime = frameNow;
        g_frameDeltaMs = (float)(frameNow - g_lastFrameTime);
        if (g_frameDeltaMs < 1.f) g_frameDeltaMs = 1.f;

        bool snapAnimations = (g_frameDeltaMs > FRAME_DELTA_SNAP_MS);
        if (g_frameDeltaMs > 33.f) g_frameDeltaMs = 33.f;  // Cap for normal 30+ fps
        g_lastFrameTime = frameNow;

        // ================================================================
        //  PER-ICON ANIMATION (opacity fade + momentum position)
        // ================================================================
        bool animActive  = false;
        HWND localOverlay = NULL;

        if (g_csInitialized && !g_pinnedApps.empty()) {
            EnterCriticalSection(&g_cs);
            localOverlay = g_overlayWnd;
            int n = (int)g_pinnedApps.size();

            for (int i = 0; i < n; ++i) {
                auto& app = g_pinnedApps[i];

                // Opacity fade-in for newly pinned icons
                if (app.opacity < 1.0f) {
                    if (snapAnimations) {
                        app.opacity = 1.0f;
                    } else {
                        app.opacity += PIN_FADE_SPEED * (g_frameDeltaMs / 16.f);
                        if (app.opacity > 1.0f) app.opacity = 1.0f;
                    }
                    animActive = true;
                }

                // Momentum-based X position animation.
                // PIN_SLIDE_OFFSET is already encoded in the initial currentX set
                // at PinApp time (currentX = targetX + PIN_SLIDE_OFFSET).  Adding it
                // again here would cancel the driving force (diff -> 0) and freeze
                // the slide-in entirely, leaving the icon 10 px to the right of its
                // slot until the isNew flag is cleared by the opacity threshold.
                float diff = app.targetX - app.currentX;

                if (fabsf(diff) <= 1.0f && !app.isNew) {
                    app.currentX  = app.targetX;
                    app.velocityX = 0.f;
                } else if (snapAnimations) {
                    app.currentX  = app.targetX;
                    app.velocityX = 0.f;
                    app.isNew     = false;
                } else {
                    float tf = std::min((g_frameDeltaMs / 16.0f) * ICON_ANIM_SPEED, 0.5f);
                    app.velocityX = (app.velocityX + diff * tf) * ANIM_MOMENTUM_DECAY;
                    app.currentX += app.velocityX;
                    animActive = true;
                }

                // Clear entrance flag once settled
                if (app.isNew && fabsf(diff) < 2.0f && app.opacity >= 0.9f)
                    app.isNew = false;

                // Hover scale spring animation
                // Scale-up is fast (instant feel), scale-down is slightly slower (graceful).
                bool isBeingDragged = g_dragFromDock && (i == g_dragFromDockIdx)
                                   && (g_dragState == DRAG_DRAGGING);
                float targetScale = (i == g_hoverIndex && !isBeingDragged && app.opacity >= 0.99f)
                                  ? HOVER_SCALE_FACTOR : 1.0f;
                float scaleDiff   = targetScale - app.hoverScale;
                if (fabsf(scaleDiff) > 0.002f) {
                    float spd = (scaleDiff > 0.f) ? HOVER_SCALE_IN_SPEED : HOVER_SCALE_OUT_SPEED;
                    float tf  = std::min((g_frameDeltaMs / 16.0f) * spd, 0.8f);
                    app.hoverScale += scaleDiff * tf;
                    animActive = true;
                } else {
                    app.hoverScale = targetScale;
                }
            }

            LeaveCriticalSection(&g_cs);
        }

        g_anyAnimationActive = animActive;
        if (animActive && localOverlay && IsWindow(localOverlay))
            InvalidateRect(localOverlay, NULL, FALSE);

        // Limit-flash expiry (2 s)
        if (g_limitFlashActive && now - g_limitFlashStart > 2000) {
            g_limitFlashActive = false;
            if (g_overlayWnd) InvalidateRect(g_overlayWnd, NULL, FALSE);
        }

        // Dock position smooth glide
        if (AnimateDockPositionStep()) {
            RepositionOverlay();
            g_anyAnimationActive = true;
        }

        lastLDown = lDown;

        // ================================================================
        //  ADAPTIVE SLEEP
        //  8 ms   --  dragging or any animation active
        //  16 ms  --  brief post-active transition
        //  50 ms  --  fully idle (saves CPU)
        // ================================================================
        if (g_dragState == DRAG_DRAGGING || g_dragState == DRAG_REORDER ||
            g_anyAnimationActive || g_dockPosAnimActive) {
            g_idleFrames = 0;
            Sleep(8);
        } else if (g_idleFrames < 10) {
            g_idleFrames++;
            Sleep(16);
        } else {
            Sleep(50);
        }
    }

    return 0;
}

// ============================================================
//  RUNNING-STATE DETECTION
// ============================================================
// Enumerates all visible top-level windows every RUNNING_STATE_CHECK_MS and
// marks each pinned app as running/not-running.  Uses PROCESS_QUERY_LIMITED_
// INFORMATION (lowest-privilege handle) for safety.
struct RunStateCtx {
    wchar_t**  paths;    // Flat buffer of resolved paths
    int        count;
    int        capacity;
};

static BOOL CALLBACK RunStateEnumProc(HWND hwnd, LPARAM lp) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    // Skip tool windows  --  they are not user-facing app windows
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    // Skip windows with no title (background/service windows)
    int titleLen = GetWindowTextLengthW(hwnd);
    if (titleLen == 0) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return TRUE;

    RunStateCtx* c = (RunStateCtx*)lp;
    if (c->count < c->capacity) {
        DWORD len = MAX_PATH;
        QueryFullProcessImageNameW(hProc, 0, c->paths[c->count], &len);
        if (len > 0) c->count++;
    }
    CloseHandle(hProc);
    return TRUE;
}

// ============================================================
//  AUTO-HIDE DETECTION
// ============================================================
// Queries the shell for the current taskbar auto-hide state.
// Must be called from a context where COM is not required (it is not a COM call).
static void UpdateAutoHideState() {
    APPBARDATA abd = {};
    abd.cbSize = sizeof(APPBARDATA);
    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    bool nowHide = (state & ABS_AUTOHIDE) != 0;
    if (nowHide != g_taskbarAutoHide) {
        g_taskbarAutoHide = nowHide;
        DEBUG_LOG(L"TASKBAR: auto-hide %s", nowHide ? L"ON" : L"OFF");
        RepositionOverlay();  // immediately apply show/hide decision
    }
}

static void UpdateRunningState() {
    // Stack-allocate path buffers  --  no heap allocation on the hot path
    // FIX-5: cap raised from 64 to 128  --  64 was silently truncating on busy systems
    // (enterprise machines with many browser/document windows), making those apps
    // always appear "not running". 128 * MAX_PATH * 2 = ~66 KB of static data  --  fine.
    static wchar_t pathBuf[128][MAX_PATH];
    static wchar_t* pathPtrs[128];
    static bool     s_ptrInit = false;
    if (!s_ptrInit) {
        for (int i = 0; i < 128; ++i) pathPtrs[i] = pathBuf[i];
        s_ptrInit = true;
    }

    RunStateCtx ctx;
    ctx.paths    = pathPtrs;
    ctx.count    = 0;
    ctx.capacity = 128;
    EnumWindows(RunStateEnumProc, (LPARAM)&ctx);

    if (!g_csInitialized) return;
    EnterCriticalSection(&g_cs);
    for (auto& app : g_pinnedApps) {
        app.running = false;
        for (int i = 0; i < ctx.count; ++i) {
            if (_wcsicmp(app.exePath.c_str(), ctx.paths[i]) == 0) {
                app.running = true;
                break;
            }
        }
    }
    LeaveCriticalSection(&g_cs);
}

// ============================================================
//  REORDER HELPERS
// ============================================================
static int CalculateReorderSlot(POINT screenPt, int n) {
    // Map screen X to overlay-local X and find the closest slot midpoint.
    // Safe to call outside CS  --  only reads g_dockCurrentX (float, atomic read).
    if (n <= 1) return 0;
    int localX = screenPt.x - (int)g_dockCurrentX;
    for (int i = 0; i < n; ++i) {
        RECT r   = GetIconRectLocal(i, n);
        int  mid = (r.left + r.right) / 2;
        if (localX < mid) return i;
    }
    return n - 1;
}

static void UpdateReorderPositions() {
    // Assigns targetX to every icon as if g_reorderSrcIdx is placed at
    // g_reorderTargetIdx and all others shift around it.
    if (g_reorderSrcIdx < 0 || g_reorderTargetIdx < 0) return;
    EnterCriticalSection(&g_cs);
    int n   = (int)g_pinnedApps.size();
    int src = g_reorderSrcIdx;
    int dst = g_reorderTargetIdx;
    if (src >= 0 && src < n && dst >= 0 && dst < n) {
        int nonSrc = 0;
        for (int slot = 0; slot < n; ++slot) {
            int iconIdx;
            if (slot == dst) {
                iconIdx = src;
            } else {
                // Guard: nonSrc must stay within [0, n) and must skip src exactly once
                while (nonSrc < n && nonSrc == src) ++nonSrc;
                if (nonSrc >= n) break;  // Exhausted non-src icons  --  layout is complete
                iconIdx = nonSrc++;
            }
            if (iconIdx >= 0 && iconIdx < n) {
                RECT r = GetIconRectLocal(slot, n);
                g_pinnedApps[iconIdx].targetX = (float)r.left;
            }
        }
    }
    LeaveCriticalSection(&g_cs);
}

static void CommitReorder() {
    // Moves the reorder-source icon to the target slot, saves, and snaps positions.
    // Production-safe: all state cleared even on early exit.
    int src = g_reorderSrcIdx;
    int dst = g_reorderTargetIdx;
    g_reorderSrcIdx    = -1;
    g_reorderTargetIdx = -1;
    if (src < 0 || dst < 0 || src == dst || !g_csInitialized) return;
    EnterCriticalSection(&g_cs);
    int n = (int)g_pinnedApps.size();
    if (src < n && dst < n) {
        PinnedApp item = g_pinnedApps[src];
        g_pinnedApps.erase(g_pinnedApps.begin() + src);
        // After erasing src, insert at dst directly.
        // When src < dst the erase shifts all subsequent elements left by one,
        // but std::vector::insert semantics mean insert(dst, x) still places x
        // at display slot dst  --  verified by tracing UpdateReorderPositions.
        // The old "(src<dst)?dst-1:dst" formula was wrong and produced off-by-one
        // placement whenever the dragged icon moved rightward.
        int insertAt = std::max(0, std::min(dst, (int)g_pinnedApps.size()));
        g_pinnedApps.insert(g_pinnedApps.begin() + insertAt, item);
        // Snap all positions to canonical layout (no re-animation after commit)
        int total = (int)g_pinnedApps.size();
        for (int i = 0; i < total; ++i) {
            RECT r = GetIconRectLocal(i, total);
            g_pinnedApps[i].targetX  = (float)r.left;
            g_pinnedApps[i].currentX = (float)r.left;
        }
    }
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();  // Called outside CS  --  safe
}

// ============================================================
//  SECONDARY DOCK (MULTI-MONITOR) HELPERS
// ============================================================

LRESULT CALLBACK SecondaryOverlayProc(HWND hwnd, UINT msg,
                                       WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (!hdc) return 0;

        RECT cr = {};
        GetClientRect(hwnd, &cr);
        // Use RGB(1,0,1) matching the colour-key set in SetLayeredWindowAttributes
        if (!g_blackBrush) g_blackBrush = CreateSolidBrush(RGB(1, 0, 1));
        FillRect(hdc, &cr, g_blackBrush);

        if (!g_csInitialized) { EndPaint(hwnd, &ps); return 0; }
        EnterCriticalSection(&g_cs);
        int n = (int)g_pinnedApps.size();

        // Glass tint  --  same normal palette as primary
        if (ENABLE_GLASS_OVERLAY && cr.right > 0) {
            TRIVERTEX tv[2] = {};
            tv[0].x = cr.left;  tv[0].y = cr.top;
            tv[1].x = cr.right; tv[1].y = cr.bottom;
            tv[0].Red = 30<<8; tv[0].Green = 32<<8; tv[0].Blue = 48<<8;
            tv[1].Red = 18<<8; tv[1].Green = 20<<8; tv[1].Blue = 34<<8;
            GRADIENT_RECT gr = { 0, 1 };
            GradientFill(hdc, tv, 2, &gr, 1, GRADIENT_FILL_RECT_H);
        }

        // Draw icons at canonical positions  --  no animation state on secondary docks
        for (int i = 0; i < n; ++i) {
            const auto& app = g_pinnedApps[i];
            if (!app.icon || app.opacity < 0.05f) continue;
            RECT r = GetIconRectLocal(i, n);
            DrawIconEx(hdc, r.left, r.top, app.icon,
                       ICON_SIZE, ICON_SIZE, 0, NULL, DI_NORMAL);
        }

        // Separator
        if (SEPARATOR_OPACITY > 0 && n > 0) {
            if (!g_linePenNormal) g_linePenNormal = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
            HPEN old = (HPEN)SelectObject(hdc, g_linePenNormal);
            MoveToEx(hdc, cr.right - 1, cr.top + 3, NULL);
            LineTo  (hdc, cr.right - 1, cr.bottom - 3);
            SelectObject(hdc, old);
        }

        LeaveCriticalSection(&g_cs);
        EndPaint(hwnd, &ps);
        return 0;
    }
    // Secondary docks are visual-only  --  pass all hit-testing to the desktop below
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void InitSecondaryDocks() {
    if (!MULTI_MONITOR_DOCK || g_fixedDockWidth <= 0) return;

    // Register the secondary window class once
    static bool s_classRegistered = false;
    if (!s_classRegistered) {
        WNDCLASSEXW wc   = { sizeof(wc) };
        wc.lpfnWndProc   = SecondaryOverlayProc;
        wc.hInstance     = GetModuleHandleW(NULL);
        wc.lpszClassName = L"WH_QPDockSecondary";
        wc.style         = CS_HREDRAW | CS_VREDRAW;
        RegisterClassExW(&wc);
        s_classRegistered = true;
    }

    // Enumerate Shell_SecondaryTrayWnd windows  --  one per secondary monitor
    // Build the new list locally; only acquire the lock to commit it.
    std::vector<SecondaryDock> newDocks;
    HWND sec = NULL;
    while ((sec = FindWindowExW(NULL, sec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        RECT tbr = {};
        if (!GetWindowRect(sec, &tbr)) continue;
        int tbW = tbr.right - tbr.left;
        int tbH = tbr.bottom - tbr.top;
        if (tbW < 100 || tbH < 20) continue;

        // Position dock at the left edge of the secondary taskbar
        int dockLeft  = tbr.left;
        int dockRight = dockLeft + g_fixedDockWidth;
        if (dockRight > tbr.right) dockRight = tbr.right;

        HWND hwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT,
            L"WH_QPDockSecondary", L"", WS_POPUP,
            dockLeft, tbr.top, dockRight - dockLeft, tbH,
            NULL, NULL, GetModuleHandleW(NULL), NULL);
        if (!hwnd) continue;

        SetLayeredWindowAttributes(hwnd, RGB(1, 0, 1), 0, LWA_COLORKEY);
        SetWindowPos(hwnd, HWND_TOPMOST, dockLeft, tbr.top,
                     dockRight - dockLeft, tbH,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);

        SecondaryDock sd;
        sd.tbRect   = tbr;
        sd.dockRect = { dockLeft, tbr.top, dockRight, tbr.bottom };
        sd.localW   = dockRight - dockLeft;
        sd.localH   = tbH;
        sd.overlay  = hwnd;
        newDocks.push_back(sd);
    }

    // FIX-3: commit under g_secondaryDocksCS so RepaintSecondaryDocks on the
    // worker thread never races the push_back.
    if (g_secondaryDocksCSInit) EnterCriticalSection(&g_secondaryDocksCS);
    for (auto& nd : newDocks) g_secondaryDocks.push_back(nd);
    if (g_secondaryDocksCSInit) LeaveCriticalSection(&g_secondaryDocksCS);
}

static void DestroySecondaryDocks() {
    // FIX-3: take the lock before clearing the vector so RepaintSecondaryDocks
    // on the worker thread cannot iterate a partially-cleared container.
    if (g_secondaryDocksCSInit) EnterCriticalSection(&g_secondaryDocksCS);
    std::vector<SecondaryDock> toDestroy;
    toDestroy.swap(g_secondaryDocks);  // move out atomically under the lock
    if (g_secondaryDocksCSInit) LeaveCriticalSection(&g_secondaryDocksCS);

    // Destroy the HWNDs outside the lock  --  DestroyWindow can block briefly
    for (auto& sd : toDestroy)
        if (sd.overlay && IsWindow(sd.overlay))
            DestroyWindow(sd.overlay);
}

static void RepaintSecondaryDocks() {
    // FIX-3: guard the iteration so we cannot read a vector that is being
    // cleared by DestroySecondaryDocks on another thread.
    // Cheap: only posts WM_PAINT  --  no rendering on the calling thread.
    if (g_secondaryDocksCSInit) EnterCriticalSection(&g_secondaryDocksCS);
    for (auto& sd : g_secondaryDocks)
        if (sd.overlay && IsWindow(sd.overlay))
            InvalidateRect(sd.overlay, NULL, FALSE);
    if (g_secondaryDocksCSInit) LeaveCriticalSection(&g_secondaryDocksCS);
}

// ============================================================
//  MODULE ENTRY POINTS
// ============================================================
// Forward declaration  --  Wh_ModInit calls this on any failure path to ensure
// every partially-initialised resource (HWND, HICON, HANDLE, CS) is released
// even if Windhawk does not guarantee calling Wh_ModUninit after a FALSE return.
void Wh_ModUninit();

BOOL Wh_ModInit() {
    // Read and clamp user settings
    MAX_PINNED_APPS      = Wh_GetIntSetting(L"maxPinnedApps",     5);
    BASE_ICON_SIZE       = Wh_GetIntSetting(L"iconSize",          28);
    BASE_ICON_SPACING    = Wh_GetIntSetting(L"iconSpacing",        9);
    SEPARATOR_OPACITY    = Wh_GetIntSetting(L"separatorOpacity", 100);
    ENABLE_GLASS_OVERLAY = Wh_GetIntSetting(L"enableGlassOverlay", 1) != 0;
    ENABLE_REORDER       = Wh_GetIntSetting(L"enableReorder",       1) != 0;
    MULTI_MONITOR_DOCK   = Wh_GetIntSetting(L"multiMonitorDock",    0) != 0;
    STARTUP_DELAY_MS     = Wh_GetIntSetting(L"startupDelay",        0);
    // Hotkey: 0 modifiers or 0 key = disabled. Clamp modifiers to valid MOD_* flags.
    g_hotkeyMods = (UINT)Wh_GetIntSetting(L"hotkeyModifiers", (int)(MOD_CONTROL | MOD_ALT));
    g_hotkeyKey  = (UINT)Wh_GetIntSetting(L"hotkeyKey",       (int)'P');
    g_hotkeyMods &= (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);
    // Auto-hide sync: default OFF  --  dock stays visible even if taskbar auto-hides
    ENABLE_AUTOHIDE_SYNC = Wh_GetIntSetting(L"autoHideSync", 0) != 0;

    MAX_PINNED_APPS   = std::max(1,  std::min(10,   MAX_PINNED_APPS));
    BASE_ICON_SIZE    = std::max(16, std::min(48,   BASE_ICON_SIZE));
    BASE_ICON_SPACING = std::max(2,  std::min(24,   BASE_ICON_SPACING));
    SEPARATOR_OPACITY = std::max(0,  std::min(100,  SEPARATOR_OPACITY));
    STARTUP_DELAY_MS  = std::max(0,  std::min(3000, STARTUP_DELAY_MS));

    // Optional startup delay for slow machines  --  applied before any GDI/window work
    if (STARTUP_DELAY_MS > 0)
        Sleep((DWORD)STARTUP_DELAY_MS);

    InitializeCriticalSection(&g_cs);
    g_csInitialized = true;

    // FIX-3: secondary-docks CS must be live before any possible call to the
    // Init/Destroy/Repaint helpers (including the secondary dock path in the
    // worker thread geometry poll).
    InitializeCriticalSection(&g_secondaryDocksCS);
    g_secondaryDocksCSInit = true;

    g_exitEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_exitEvent) { Wh_ModUninit(); return FALSE; }

    g_bootStartTime = GetTickCount();
    g_systemState   = STATE_BOOT;

    RefreshTaskbarCache();
    LoadPinnedApps();

    if (!CreateOverlayWindow()) { Wh_ModUninit(); return FALSE; }
    InitGhostDIB();
    if (!CreateGhostWindow())   { Wh_ModUninit(); return FALSE; }

    // Snap position immediately so there is no fly-in from (0,0) at startup
    if (!g_positionInitialized && g_cachedDockRect.left > 0) {
        g_dockCurrentX        = (float)g_cachedDockRect.left;
        g_dockCurrentY        = (float)g_cachedDockRect.top;
        g_dockTargetX         = g_dockCurrentX;
        g_dockTargetY         = g_dockCurrentY;
        g_positionInitialized = true;
    }

    // Force the overlay visible and in position before the worker thread starts
    if (g_overlayWnd && IsWindow(g_overlayWnd)) {
        ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);
        SetWindowPos(g_overlayWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        InvalidateRect(g_overlayWnd, NULL, FALSE);
    }
    RepositionOverlay();

    // Subscribe to taskbar location/foreground events for geometry-driven refresh.
    // WINEVENT_INCONTEXT: callback runs on the main thread (the thread calling
    // SetWinEventHook) which owns the overlay window and has a message pump.
    // OUTOFCONTEXT fires on a random thread-pool thread with no pump, causing
    // RefreshTaskbarCache / RepositionOverlay to race with the overlay's wndproc.
    // NULL hmodWinEventProc is required for INCONTEXT -- the DLL handle is resolved
    // automatically by the hook system since we are in-process.
    g_winEventHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_SYSTEM_FOREGROUND,
        GetModuleHandleW(NULL), WinEventProc, 0, GetCurrentThreadId(),
        WINEVENT_INCONTEXT | WINEVENT_SKIPOWNPROCESS);

    // Multi-monitor: create secondary dock overlays after geometry is ready
    InitSecondaryDocks();

    g_workerThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
    if (!g_workerThread) { Wh_ModUninit(); return FALSE; }

    if (ENABLE_AUTOHIDE_SYNC) UpdateAutoHideState();  // only when user enables sync

    DEBUG_LOG(L"INIT: v31.0.0 OK. state=%d pinned=%d glass=%d reorder=%d multimon=%d delay=%d hotkey=0x%X+0x%X autohide=%d",
              g_systemState, (int)g_pinnedApps.size(),
              (int)ENABLE_GLASS_OVERLAY, (int)ENABLE_REORDER,
              (int)MULTI_MONITOR_DOCK, STARTUP_DELAY_MS,
              g_hotkeyMods, g_hotkeyKey, (int)ENABLE_AUTOHIDE_SYNC);
    return TRUE;
}

void Wh_ModUninit() {
    // Stop event hook first to prevent callbacks after teardown begins
    if (g_winEventHook) { UnhookWinEvent(g_winEventHook); g_winEventHook = NULL; }
    // Unregister hotkey before the overlay window is destroyed
    if (g_overlayWnd && IsWindow(g_overlayWnd) && g_hotkeyKey != 0 && g_hotkeyMods != 0)
        UnregisterHotKey(g_overlayWnd, HOTKEY_PIN_ID);

    // Signal the worker thread and wait up to 2 seconds for clean exit
    if (g_exitEvent) {
        SetEvent(g_exitEvent);
        if (g_workerThread) {
            WaitForSingleObject(g_workerThread, 2000);
            CloseHandle(g_workerThread);
            g_workerThread = NULL;
        }
        CloseHandle(g_exitEvent);
        g_exitEvent = NULL;
    }

    // Destroy secondary monitor docks first (they repaint from the same icon handles)
    DestroySecondaryDocks();
    // FIX-3: secondary-docks CS  --  delete after DestroySecondaryDocks so no further
    // callers can attempt to acquire it (all callers check g_secondaryDocksCSInit first).
    if (g_secondaryDocksCSInit) {
        g_secondaryDocksCSInit = false;
        DeleteCriticalSection(&g_secondaryDocksCS);
    }

    // Destroy GDI resources in reverse creation order
    if (g_ghostDIB)  { DeleteObject(g_ghostDIB);  g_ghostDIB  = NULL; }
    if (g_ghostWnd)  { DestroyWindow(g_ghostWnd);  g_ghostWnd  = NULL; }
    if (g_overlayWnd){ DestroyWindow(g_overlayWnd);g_overlayWnd= NULL; }

    if (g_blackBrush)    { DeleteObject(g_blackBrush);    g_blackBrush    = NULL; }
    if (g_linePenNormal) { DeleteObject(g_linePenNormal); g_linePenNormal = NULL; }
    if (g_linePenFlash)  { DeleteObject(g_linePenFlash);  g_linePenFlash  = NULL; }
    if (g_linePenDrop)   { DeleteObject(g_linePenDrop);   g_linePenDrop   = NULL; }
    if (g_runDotBrush)   { DeleteObject(g_runDotBrush);   g_runDotBrush   = NULL; }

    // Alpha-blend off-screen buffer (bmp is already selected out of DC in paint loop)
    if (g_alphaBlendDC)  { DeleteDC(g_alphaBlendDC);       g_alphaBlendDC   = NULL; }
    if (g_alphaBlendBmp) { DeleteObject(g_alphaBlendBmp);  g_alphaBlendBmp  = NULL; }
    g_alphaBlendBits = NULL;
    g_alphaBlendSize = 0;

    // Separator alpha-blend DIB cache
    if (g_sepDIB && g_sepDC) SelectObject(g_sepDC, (HBITMAP)NULL);
    if (g_sepDIB) { DeleteObject(g_sepDIB); g_sepDIB = NULL; }
    if (g_sepDC)  { DeleteDC(g_sepDC);      g_sepDC  = NULL; }
    g_sepBits    = NULL;
    g_sepCachedH = 0;

    // Destroy all pinned-app icon handles and free memory
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        for (auto& a : g_pinnedApps)
            if (a.icon) { DestroyIcon(a.icon); a.icon = NULL; }
        g_pinnedApps.clear();
        LeaveCriticalSection(&g_cs);
        DeleteCriticalSection(&g_cs);
        g_csInitialized = false;
    }
}

// Called by Windhawk whenever the user changes mod settings in the UI.
// Re-reads all settings, invalidates the dock-width cache, and refreshes
// the overlay so changes take effect immediately without a mod reload.
void Wh_ModSettingsChanged() {
    MAX_PINNED_APPS      = Wh_GetIntSetting(L"maxPinnedApps",      5);
    BASE_ICON_SIZE       = Wh_GetIntSetting(L"iconSize",           28);
    BASE_ICON_SPACING    = Wh_GetIntSetting(L"iconSpacing",         9);
    SEPARATOR_OPACITY    = Wh_GetIntSetting(L"separatorOpacity",  100);
    ENABLE_GLASS_OVERLAY = Wh_GetIntSetting(L"enableGlassOverlay",  1) != 0;
    ENABLE_REORDER       = Wh_GetIntSetting(L"enableReorder",       1) != 0;
    MULTI_MONITOR_DOCK   = Wh_GetIntSetting(L"multiMonitorDock",    0) != 0;
    STARTUP_DELAY_MS     = Wh_GetIntSetting(L"startupDelay",        0);
    ENABLE_AUTOHIDE_SYNC = Wh_GetIntSetting(L"autoHideSync",        0) != 0;
    g_hotkeyMods = (UINT)Wh_GetIntSetting(L"hotkeyModifiers", (int)(MOD_CONTROL | MOD_ALT));
    g_hotkeyKey  = (UINT)Wh_GetIntSetting(L"hotkeyKey",       (int)'P');
    g_hotkeyMods &= (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);

    MAX_PINNED_APPS   = std::max(1,  std::min(10,   MAX_PINNED_APPS));
    BASE_ICON_SIZE    = std::max(16, std::min(48,   BASE_ICON_SIZE));
    BASE_ICON_SPACING = std::max(2,  std::min(24,   BASE_ICON_SPACING));
    SEPARATOR_OPACITY = std::max(0,  std::min(100,  SEPARATOR_OPACITY));
    STARTUP_DELAY_MS  = std::max(0,  std::min(3000, STARTUP_DELAY_MS));

    // Invalidate cached dock-width so it is recalculated with new sizes/DPI.
    g_fixedDockWidth = 0;
    g_lastDpiForWidth = 0;

    // Refresh geometry and force an immediate repaint.
    RefreshTaskbarCache();
    RepositionOverlay();
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);

    DEBUG_LOG(L"SETTINGS CHANGED: maxPins=%d iconSz=%d spacing=%d glass=%d reorder=%d",
              MAX_PINNED_APPS, BASE_ICON_SIZE, BASE_ICON_SPACING,
              (int)ENABLE_GLASS_OVERLAY, (int)ENABLE_REORDER);
}

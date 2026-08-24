// ==WindhawkMod==
// @id              taskbar-quick-pin
// @name            Left Taskbar Quick Pin Dock
// @description     A persistent icon dock anchored left of the Start button. Drag any app to pin it. Left-click to launch or focus. Double-right-click to unpin. Drag within the dock to reorder.
// @version         2.0.0
// @author          Ashix
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lpsapi -lshell32 -lole32 -loleaut32 -luuid -lshlwapi -lgdi32 -lmsimg32 -luiautomationcore -ldwmapi -lwinmm
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Left Taskbar Quick Pin Dock

A lightweight icon dock that sits just left of the Start button on the
Windows 11 taskbar. Drag any app onto it to pin, click to launch or focus, and
drag it off (or double-right-click) to unpin. It's your own list  --  separate
from the taskbar's own pinned apps  --  and it survives Explorer restarts and
reboots.

No extra DLLs, no service, no installer  --  just a single-file mod injected
into `explorer.exe`.

## Quick usage

| Action | Result |
|---|---|
| Drag an app window onto the dock | Pin it |
| Left-click a pinned icon | Launch, or focus if it's already running |
| Drag a pinned icon left / right | Reorder it |
| Drag a pinned icon off the dock | Unpin it |
| Double-right-click an icon | Unpin it (off by default) |
| Hotkey (default Ctrl + Alt + P) | Pin or unpin the focused app |
| Triple-tap **L** | Lock / unlock the dock |

There are a few more gestures (rapid-click unpin-all, tap **P** & **U** three
times) -- see the full guide on GitHub for the complete list and every setting.

**Lock the dock:** triple-tap the **L** key to lock it, triple-tap **L** again
to unlock. While locked, the quick *gesture* unpins are refused (triple-tap
**U**, the rapid-click unpin-all, double-right-click, and the hotkey's unpin) so
a stray gesture can't drop a pin. A deliberate drag-off that breaks the rope
still unpins, and pinning / reordering still work.

## First run  --  please be patient

On the very first launch the dock needs a moment to detect the taskbar and
settle, so it may not appear instantly. That's normal  --  it's working in the
background, so please wait rather than reinstalling. If it still hasn't shown up
after a few minutes, restarting Explorer (or, most reliably, your PC) sorts it
out. You can also raise the **Startup delay** setting.

## Known limitations

- Secondary-monitor docks are mirrored and **read-only**  --  Beta
- A few apps with custom icon handlers may show a generic Windows icon. This is
  a shell limitation  --  no Win32 API resolves every icon reliably.
- Some UWP apps may occasionally resolve to their host process if their window
  isn't ready yet.

## Full documentation

This is just a quick overview. For the complete guide  --  every gesture, the
full settings reference, and the developer internals  --  see the full README
on GitHub:

https://github.com/k-ashix/taskbar-quick-pin

## Credits

Created with ❤️ by Ashix. Thanks to the **Taskbar Dock Animation**
(`taskbar-dock-animation`) and **Taskbar Dock Animation Plus**
(`taskbar-dock-animation-plus`) mods for animation refinements.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- maxPinnedApps: 5
  $name: Max pinned apps
  $description: >-
    Maximum number of apps that can be pinned (1 - 20).
    The dock now sizes itself to fit what is actually pinned: it shrinks when
    you unpin (never below a 5-slot minimum) and grows as you pin, but stops
    growing at 10 visible slots. Pin more than 10 and the extra icons are
    reached with scroll-wheel navigation instead of the dock getting wider.

- iconSize: 33
  $name: Icon size (px at 96 DPI)
  $description: >-
    Base icon size in pixels before DPI scaling (16 - 48).
    At 150 % display scaling a value of 33 renders at roughly 50 px.

- dockGapFromStart: 6
  $name: Dock gap from Start button
  $description: >-
    Distance in pixels between the dock's right edge and the Start button
    (0 - 40). This is independent of the fixed per-icon icon spacing  --  it
    only controls how far the whole dock sits from Start. Applies live.

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

- enableDoubleRightClickUnpin: false
  $name: Double-right-click to unpin
  $description: >-
    Unpin an icon by double-right-clicking it (it disintegrates with a "dust"
    effect). When off, double-right-click is ignored and the normal right-click
    menu shows instead  --  you can still unpin by dragging an icon off the dock
    or with the hotkey. Applies live.

- enableScrollNav: true
  $name: Scroll-wheel navigation
  $description: >-
    Hover the dock and use the mouse scroll wheel to move the highlight
    across your pinned icons (wheel down = next, wheel up = previous).
    The highlighted icon magnifies like the macOS dock.

- enableDragTether: true
  $name: Drag tether (balloon thread)
  $description: >-
    Shows a thin thread while you drag a pinned icon off the dock to unpin it.
    It links the icon's dock slot to your cursor, stretches as you pull, and
    snaps once you pull far enough. It's purely visual and stays visible for the
    whole drag. Turn it off to drag icons off the dock with no thread.

- dragTetherThickness: 2
  $name: Drag tether  --  thickness
  $description: >-
    Thickness of the drag rope's thread from 1 (ultra-thin hair strand) to 10
    (bold cord). Each step increases evenly. Applies live.

- dragRopeBreakLength: 450
  $name: Drag tether  --  break length (px)
  $description: >-
    How far (in pixels) you can pull an icon off the dock before the rope tears
    and unpins the app (150 - 650). Lower = snaps sooner, higher = lets you drag
    further before it breaks. Applies live.

- unpinTrigger: 0
  $name: Unpin trigger (drag-off)
  $description: >-
    Decides WHEN dragging a pinned icon off the dock actually unpins it.
    "Only when the rope breaks" means the icon stays pinned unless you pull far
    enough to snap the rope  --  if you let go before it breaks, the rope
    recoils back to the dock and the icon stays put. "Rope breaks OR released
    outside the dock" also unpins when you simply drop the icon anywhere off the
    dock. Default: only when the rope breaks. Applies live.
  $options:
  - 0: Only when the rope BREAKS (pull past the break length)
  - 1: Rope breaks OR icon released outside the dock

- cornerRoundness: 100
  $name: Corner roundness
  $description: >-
    Dock corner shape (0 - 100). 0 = square/rectangular corners,
    1 - 40 = small rounded corners, 41 - 100 = fully rounded corners.
    All levels are drawn by the compositor so they stay smooth (never
    pixelated). Changing this applies live -- no need to restart Explorer.

- enableExplorerWorkspacePins: false
  $name: Explorer workspace pins
  $description: >-
    Allow File Explorer to be dragged into the dock as an Explorer workspace
    pin. When disabled, explorer.exe remains excluded so the dock only accepts
    normal application pins.

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

- logLevel: 1
  $name: Logging level
  $description: >-
    Production logging filter. 0 = none, 1 = errors, 2 = important production
    events, 3 = debug diagnostics, 4 = trace-level sampling. Trace is noisy and
    should only be enabled while diagnosing a specific issue.
  $options:
  - 0: None
  - 1: Errors
  - 2: Important
  - 3: Debug
  - 4: Trace

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
#include <dwmapi.h>
#include <mmsystem.h>
#include <exdisp.h>
#include <initguid.h>
#include <commoncontrols.h>
#include <uiautomation.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <cstdarg>

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMSBT_NONE
#define DWMSBT_NONE 1
#define DWMSBT_MAINWINDOW 2
#define DWMSBT_TRANSIENTWINDOW 3
#define DWMSBT_TABBEDWINDOW 4
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_DEFAULT
#define DWMWCP_DEFAULT 0
#define DWMWCP_DONOTROUND 1
#define DWMWCP_ROUND 2
#define DWMWCP_ROUNDSMALL 3
#endif

// ============================================================
//  CONFIGURATION  --  base values at 96 DPI, scaled at runtime
// ============================================================
static const int   DRAG_THRESHOLD_PX      = 6;     // Pixels before drag is confirmed
static const int   BASE_DOCK_PAD_LEFT     = 6;     // Left internal padding
static const int   BASE_DOCK_PAD_RIGHT    = 8;     // Right internal padding
static const int   BASE_SECTION_GAP       = 20;    // Gap between workspace and app regions
static const int   BASE_MAGNETIC_RANGE    = 60;    // Ghost visibility radius beyond dock
static const int   BASE_GHOST_SIZE        = 40;    // Drag ghost size in pixels
static const int   CLICK_MAX_MS           = 300;   // Max duration for a click (not a drag)
static const int   CLICK_MAX_MOVE_PX      = 5;     // Max cursor movement for a click
static const int   MIN_VALID_DOCK_WIDTH   = 80;    // Minimum acceptable dock pixel width
static const int   BOOT_PHASE_MS          = 300;   // Boot stabilisation timeout (dynamic exit preferred; halved from 600 to reach a stable dock sooner on boot)
[[maybe_unused]] static const int DOCK_SAFE_GAP = 6; // Legacy fallback; live gap is DOCK_GAP_FROM_START (user setting)
static const int   RAPID_CLICK_THRESHOLD  = 3;     // Clicks to trigger unpin-all (reduced from 5 for usability)
static const int   RAPID_CLICK_WINDOW_MS  = 1000;  // Window for rapid-click detection (ms)
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
static const float HOVER_SCALE_FACTOR     = 1.15f; // Icon scale on hover (15%  --  macOS-style pop)
static const float HOVER_SCALE_IN_SPEED   = 0.45f; // Scale-up speed (snappy  --  Apple-like)
static const float HOVER_SCALE_OUT_SPEED  = 0.24f; // Scale-down speed (graceful ease-out)
static const float PIN_FADE_SPEED         = 0.11f; // ~90 ms fade-in  --  fast, native-feeling
static const float ICON_HIDE_FADE_SPEED   = 0.12f; // Fade-OUT rate for overflow icons sliding off the edge (>10 pins). Tuned to finish ~as the one-slot slide completes, so hide reads as a soft dissolve, not a pop.
static const float PIN_SLIDE_OFFSET       = 10.f;  // Entrance slide in pixels (more visible)
static const float ICON_ANIM_SPEED        = 0.32f; // Icon X position lerp speed (smoother reflow)
static const float FRAME_DELTA_SNAP_MS    = 50.f;  // Snap animations on frames slower than this
// ---- macOS-dock continuous magnification (cursor-pixel-X driven) ----
static const float MAG_RADIUS_PX          = 90.f;  // Cursor-to-icon-center pixel distance where magnification falls to 0 (cosine bell). Tuned ~2.4 icon slots at 33px+12px spacing.
static const float MAG_SPREAD_FACTOR      = 0.55f; // Neighbour spread: fraction of each icon's size-gain that pushes neighbours outward (mirrors reference spacingFactor)
static const float HOVER_SHIFT_SPEED      = 0.40f; // Ease speed for the horizontal spread offset (between scale-in/out for a settled feel)

// ---- Balloon-thread ("cotton tether") tuning (decorative, one thread/icon) ----
static bool        ENABLE_ICON_THREADS    = true;  // Master switch (user setting "enableDragTether"); loaded in Wh_ModInit
static const float THREAD_SPRING          = 0.42f; // TUNED 0.30->0.42: snappier tip-follow so the rope tracks the cursor with a lively Blender-noodle whip (still lags slightly). Per 16ms; frame-rate scaled below.
// FIX (tether often not rendering): the single 46px threshold made the thread
// read taut AND begin fading/breaking almost immediately -- any real off-dock
// unpin drag travels far more than 46px, so it faded out within ~200ms and was
// effectively never visible. Split into two distances: TAUT (where the bow
// straightens) and a much larger BREAK (where it actually snaps + fades). The
// thread now stays visible for the whole realistic pull-off gesture.
static const float THREAD_TAUT_PX         = 90.f;  // TUNED 60->90: bow straightens over a longer, more natural pull so the resting sag reads as a real hanging noodle before it goes taut.
static float       THREAD_MAX_STRETCH_PX  = 450.f; // "dragRopeBreakLength" (user setting, 150..650 px): the rope's MAX length. Once the pull passes this the rope TEARS in the middle and unpins. Loaded in Wh_ModInit / Wh_ModSettingsChanged.
static int         UNPIN_TRIGGER          = 0;     // "unpinTrigger": 0 = unpin ONLY when the rope breaks (release-without-break recoils + stays pinned); 1 = rope breaks OR icon released outside the dock. Loaded in Wh_ModInit / Wh_ModSettingsChanged.
static const int   THREAD_BREAK_MS        = 340;   // Snap/recoil duration (raised 200->340 so the torn halves + fray + flash read as a real break, not a quick cut).

// Drag-rope appearance (user settings; loaded in Wh_ModInit / Wh_ModSettingsChanged).
static int  THREAD_THICKNESS  = 2;    // "dragTetherThickness" 1..6 (core stroke width)
static int  THREAD_COLOR_MODE = 1;    // "dragTetherColorMode": 0 = dynamic rainbow, 1 = fixed (DEFAULT: single earthy colour, not neon)
static int  THREAD_HUE        = 30;   // "dragTetherHue" 0..359; default 30 = warm tan/brown (earthy thread)

// Interaction settings (user settings; loaded in Wh_ModInit / Wh_ModSettingsChanged).
static bool ENABLE_DOUBLE_RCLICK_UNPIN = false;  // "enableDoubleRightClickUnpin": double-right-click a pinned icon to unpin it (DEFAULT OFF)

// User-configurable (clamped in Wh_ModInit)
static int  MAX_PINNED_APPS      = 5;
static int  BASE_ICON_SIZE       = 33;
static int  BASE_ICON_SPACING    = 12;
static int  DOCK_GAP_FROM_START  = 6;      // Gap between dock right edge and Start (user-configurable, clamp 0..40)
static int  SEPARATOR_OPACITY    = 100;
static bool ENABLE_GLASS_OVERLAY = true;   // Premium glass gradient behind dock
static bool ENABLE_REORDER       = true;   // Drag-to-reorder within dock
static bool ENABLE_SCROLL_NAV    = true;   // Scroll-wheel navigation across pins
static int  CORNER_ROUNDNESS     = 100;    // 0 = square dock, 100 = fully rounded (DWM)
static bool ENABLE_EXPLORER_WORKSPACE_PINS = false; // User-controlled explorer.exe exclusion
static bool MULTI_MONITOR_DOCK   = false;  // Secondary-monitor mirror docks
static int  STARTUP_DELAY_MS     = 0;      // Extra init delay (slow machines)

// DPI-scaled values  --  set by RefreshDpiScale(), never set manually
static int ICON_SIZE        = 33;
static int ICON_SPACING     = 12;
static int DOCK_GAP_PX      = 6;   // DPI-scaled dock-to-Start gap (from DOCK_GAP_FROM_START)
static int DOCK_PAD_LEFT    = 6;
static int DOCK_PAD_RIGHT   = 8;
static int SECTION_GAP      = 20;
static int MAGNETIC_RANGE_PX = 60;
static int GHOST_SIZE       = 40;

static const int MAX_WORKSPACE_PINS = 2;
static int MAX_APP_PINS       = 5;   // FIX: mutable; tracks maxPinnedApps setting

// Dynamic dock-width sizing (app region).
//   * MIN_APP_SLOTS       : the dock never shrinks below this many app slots,
//                           so it keeps a stable, predictable minimum footprint.
//   * MAX_VISIBLE_APP_SLOTS: the dock never GROWS past this many app slots.
//                           When more apps than this are pinned, the extra ones
//                           are reached with scroll-wheel navigation (a viewport
//                           slides over the app region) instead of widening the
//                           dock indefinitely.
static const int MIN_APP_SLOTS        = 5;
static const int MAX_VISIBLE_APP_SLOTS = 10;

// ============================================================
//  REGISTRY / WINDOW CLASS NAMES
// ============================================================
static const wchar_t* REG_KEY       = L"Software\\WindhawkMods\\TaskbarQuickPin";
static const wchar_t* REG_VALUE     = L"PinnedApps";
static const wchar_t* OVERLAY_CLASS = L"QPDockOverlay";
static const wchar_t* INPUT_CLASS   = L"QPDockInputOwner";
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
static bool IsExplorerExePath(const std::wstring& path) {
    return StrStrIW(path.c_str(), L"explorer.exe") != NULL;
}

static bool IsExcludedApp(const std::wstring& path) {
    if (path.empty()) return true;
    for (int i = 0; g_excludeList[i]; ++i) {
        if (ENABLE_EXPLORER_WORKSPACE_PINS &&
            _wcsicmp(g_excludeList[i], L"explorer.exe") == 0)
            continue;
        if (StrStrIW(path.c_str(), g_excludeList[i]))
            return true;
    }
    return false;
}

// ============================================================
//  DATA STRUCTURES
// ============================================================
enum PinType {
    PIN_APP,
    PIN_WORKSPACE
};

struct WorkspaceSnapshot {
    std::wstring id;
    std::wstring displayName;
    std::vector<std::wstring> folderPaths;
    struct WindowGroup {
        HWND hwnd = NULL;
        int activeTab = 0;
        std::vector<std::wstring> tabPaths;
    };
    std::vector<WindowGroup> windows;
};

struct PinnedApp {
    PinType      type        = PIN_APP;
    std::wstring exePath;
    std::wstring workspaceId;
    std::wstring displayName;
    HICON        icon        = NULL;
    float        currentX    = 0.f;  // Animated X position (overlay-local)
    float        targetX     = 0.f;  // Destination X position
    float        velocityX   = 0.f;  // Momentum for position animation
    float        opacity     = 0.f;  // 0.0 - 1.0 for fade-in animation
    bool         isNew       = true; // Triggers entrance animation
    float        hoverScale  = 1.0f; // Animated scale: 1.0 = normal, HOVER_SCALE_FACTOR = hovered
    float        hoverShiftX = 0.f;  // Animated horizontal spread: magnified icons push neighbours outward (macOS-dock feel)
    bool         running     = false; // True when >=1 visible window of this app exists
};

// ============================================================
//  GLOBALS  --  window handles
// ============================================================
static HWND    g_overlayWnd   = NULL;
static HWND    g_inputWnd     = NULL;
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
static volatile bool g_dockWidthDirty = false; // Set on pin/unpin: forces the dock width to be recomputed so it shrinks/grows to fit the real content

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

// High-resolution frame timing.  GetTickCount() only has ~15.6 ms resolution,
// which quantises g_frameDeltaMs to 0/15/16/31 ms and makes every lerp stutter.
// QueryPerformanceCounter gives sub-microsecond deltas for smooth interpolation.
static LARGE_INTEGER g_perfFreq      = {};    // ticks/sec, filled once in Wh_ModInit
static LONGLONG      g_lastFrameQpc  = 0;      // QPC value at previous frame
static bool          g_timerPeriodActive = false; // true while timeBeginPeriod(1) is held

// Returns elapsed milliseconds since the previous call (worker-thread only).
// First call primes the baseline and returns a nominal 16 ms.
static float QpcFrameDeltaMs() {
    if (g_perfFreq.QuadPart == 0) return 16.f;   // QPC unavailable -> nominal frame
    LARGE_INTEGER nowQpc;
    QueryPerformanceCounter(&nowQpc);
    if (g_lastFrameQpc == 0) { g_lastFrameQpc = nowQpc.QuadPart; return 16.f; }
    double ms = (double)(nowQpc.QuadPart - g_lastFrameQpc) * 1000.0 / (double)g_perfFreq.QuadPart;
    g_lastFrameQpc = nowQpc.QuadPart;
    return (float)ms;
}

// Raise the system timer resolution to 1 ms so Sleep(8)/Sleep(16) are honoured
// instead of rounding up to the ~15.6 ms default tick.  Held only while animating
// or dragging so we don't impose a permanent system-wide power penalty when idle.
static void SetHighResTimer(bool enable) {
    if (enable == g_timerPeriodActive) return;
    if (enable) { if (timeBeginPeriod(1) == TIMERR_NOERROR) g_timerPeriodActive = true; }
    else        { timeEndPeriod(1); g_timerPeriodActive = false; }
}

// ============================================================
//  GLOBALS  --  interaction state
// ============================================================
static int     g_hoverIndex      = -1;
static POINT   g_dragStartPt     = {};
static DWORD   g_mouseDownTime   = 0;
static std::wstring g_draggedAppPath;  // Path locked at PRESS
static std::wstring g_lockedDragPath;  // Second lock confirmed at DRAGGING threshold
static PinType      g_draggedPinType = PIN_APP;
static PinType      g_lockedDragPinType = PIN_APP;
static HWND         g_draggedExplorerHwnd = NULL;
static HWND         g_lockedExplorerHwnd  = NULL;
static HICON   g_dragGhostIcon   = NULL;
static bool    g_dragFromDock    = false;
static int     g_dragFromDockIdx = -1;
static bool    g_dropZoneActive  = false;

// Rapid-click state
static int     g_rapidClickCount = 0;
static DWORD   g_rapidClickStart = 0;
static int     g_rapidClickIndex = -1;

// ---- Keyboard triple-tap gesture (worker-thread polled) ----
// Tap the P key 3x quickly -> PIN the focused app; tap U key 3x quickly ->
// UNPIN it. Rising-edge detected so a held key counts as one tap. Reset if the
// taps are spread out beyond KEY_TAP_WINDOW_MS.
static const int KEY_TAP_THRESHOLD  = 3;    // taps to fire
static const int KEY_TAP_WINDOW_MS  = 600;  // all 3 taps must land within this window
static int     g_tapCountP   = 0;   static DWORD g_tapStartP = 0;   static bool g_pWasDown = false;
static int     g_tapCountU   = 0;   static DWORD g_tapStartU = 0;   static bool g_uWasDown = false;
static int     g_tapCountL   = 0;   static DWORD g_tapStartL = 0;   static bool g_lWasDown = false;

// ---- Icon lock (triple-tap L) ----
// Triple-tap the L key to LOCK the dock, tap it 3x again to UNLOCK. While
// locked, all *gesture* unpins are refused -- triple-tap U, the rapid-click
// unpin-all, double-right-click unpin, and the hotkey's unpin action -- so an
// accidental gesture can never drop a pin. A deliberate drag-off that BREAKS
// the rope still unpins (that is an explicit, hard-to-trigger-by-accident act),
// as does pinning new apps and reordering. Single-writer (worker thread) /
// multi-reader bool, safe lock-free on x86-64 like the other per-frame flags.
static bool    g_iconsLocked = false;

// INTERACTIVE lock glow. Rather than glowing the whole time the dock is locked
// (which made a locked dock look permanently "highlighted"), the gold edge only
// FLASHES when the user does something lock-related -- attempts a blocked
// gesture unpin, or toggles the lock -- then fades out, so a locked dock looks
// completely natural at rest. g_lockGlowStart = tick the current flash began
// (0 = no flash / idle); the flash runs for LOCK_GLOW_MS.
static DWORD   g_lockGlowStart = 0;
static const int LOCK_GLOW_MS  = 750;

// Visual feedback
static bool    g_limitFlashActive = false;
static DWORD   g_limitFlashStart  = 0;
static bool    g_shakeActive      = false;  // Dock shake on pin-limit hit
static DWORD   g_shakeStart       = 0;

// Taskbar auto-hide integration
static bool    g_taskbarAutoHide  = false;  // True when taskbar has ABS_AUTOHIDE set
static int     g_autoHideMiss     = 0;      // Hysteresis counter for auto-hide slide (anti-flicker)
static int     g_appScrollStart   = 0;      // First VISIBLE app-pin ordinal. When more apps are pinned than MAX_VISIBLE_APP_SLOTS, the app region becomes a scrollable viewport; this is the left edge of that window (wheel nav slides it).
static DWORD   g_scrollNavUntil   = 0;      // Scroll-lock deadline (GetTickCount) protecting the scrolled highlight
static POINT   g_scrollNavPt      = {};     // Cursor pos when the scroll-lock was armed; real movement past it releases the lock
static const DWORD SCROLL_NAV_LOCK_MS = 700; // How long a wheel-selected highlight is protected from stray WM_MOUSEMOVE

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

// Click debounce for SmartLaunch  --  a single physical click can re-enter the
// click handler across a few worker-loop iterations (8-50 ms each). The app
// path is already protected by the g_launchCount limiter, but the workspace
// (and focus) paths were not, so a single click could restore a workspace 2-3
// times. This idempotency guard collapses any duplicate invocations for the
// same icon within a short window into one, regardless of pin type.
static const DWORD SMARTLAUNCH_DEBOUNCE_MS = 500;
static DWORD   g_lastSmartLaunchTime = 0;
static int     g_lastSmartLaunchIdx  = -1;

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
static HBITMAP g_alphaBlendOldBmp = NULL;
static BYTE*   g_alphaBlendBits = NULL;
static int     g_alphaBlendSize = 0;  // Edge length of the current square buffer

// Dedicated NATIVE-size icon-blit buffer (used by BlitIconAlpha). Kept separate
// from g_alphaBlend* so its size tracks the icon's native pixels, not ICON_SIZE.
static HBITMAP g_iconBlitBmp    = NULL;
static HDC     g_iconBlitDC     = NULL;
static HBITMAP g_iconBlitOldBmp = NULL;
static BYTE*   g_iconBlitBits   = NULL;
static int     g_iconBlitSize   = 0;  // Edge length of the current native-size buffer

// Separator alpha-blend DIB  --  cached across frames, rebuilt only when height changes
static HDC     g_sepDC      = NULL;
static HBITMAP g_sepDIB     = NULL;
static HBITMAP g_sepOldBmp  = NULL;
static BYTE*   g_sepBits    = NULL;
static int     g_sepCachedH = 0;  // lineH for which the cached DIB was built

// Paint back buffer  --  cached across frames to present each paint atomically.
static HDC     g_paintDC       = NULL;
static HBITMAP g_paintDIB      = NULL;
static HBITMAP g_paintOldBmp   = NULL;
static int     g_paintCachedW  = 0;
static int     g_paintCachedH  = 0;
static bool    g_mouseTracking = false;

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
enum LogLevel {
    LOG_NONE = 0,
    LOG_ERROR,
    LOG_IMPORTANT,
    LOG_DEBUG,
    LOG_TRACE
};

static LogLevel g_logLevel = LOG_ERROR;   // Default logging level = 1 (errors only)

struct RateLogBucket {
    const wchar_t* key;
    DWORD lastTick;
    unsigned suppressed;
};

static RateLogBucket g_rateLogs[16] = {};

static bool ShouldLog(LogLevel level) {
    return level != LOG_NONE && level <= g_logLevel;
}

static LogLevel ClampLogLevel(int level) {
    if (level < (int)LOG_NONE) return LOG_NONE;
    if (level > (int)LOG_TRACE) return LOG_TRACE;
    return (LogLevel)level;
}

static void QPLog(LogLevel level, const wchar_t* prefix, const wchar_t* fmt, ...) {
    if (!ShouldLog(level)) return;

    wchar_t body[1024] = {};
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(body, ARRAYSIZE(body), _TRUNCATE, fmt, args);
    va_end(args);

    Wh_Log(L"%s %s", prefix, body);
}

static void QPLogRateLimited(LogLevel level,
                             const wchar_t* key,
                             DWORD cooldownMs,
                             const wchar_t* prefix,
                             const wchar_t* fmt, ...) {
    if (!ShouldLog(level)) return;

    DWORD now = GetTickCount();
    RateLogBucket* bucket = NULL;
    RateLogBucket* empty = NULL;
    for (auto& b : g_rateLogs) {
        if (b.key == key || (b.key && key && wcscmp(b.key, key) == 0)) {
            bucket = &b;
            break;
        }
        if (!b.key && !empty) empty = &b;
    }
    if (!bucket) {
        bucket = empty ? empty : &g_rateLogs[0];
        bucket->key = key;
        bucket->lastTick = 0;
        bucket->suppressed = 0;
    }

    if (bucket->lastTick && now - bucket->lastTick < cooldownMs) {
        bucket->suppressed++;
        return;
    }

    wchar_t body[1024] = {};
    va_list args;
    va_start(args, fmt);
    _vsnwprintf_s(body, ARRAYSIZE(body), _TRUNCATE, fmt, args);
    va_end(args);

    if (bucket->suppressed) {
        Wh_Log(L"%s %s (suppressed %u repeats)", prefix, body, bucket->suppressed);
        bucket->suppressed = 0;
    } else {
        Wh_Log(L"%s %s", prefix, body);
    }
    bucket->lastTick = now;
}

#define LOG_ERROR(fmt, ...)     QPLog(LOG_ERROR,     L"[QPDock]", fmt, ##__VA_ARGS__)
#define LOG_IMPORTANT(fmt, ...) QPLog(LOG_IMPORTANT, L"[QPDock]", fmt, ##__VA_ARGS__)
#define DEBUG_LOG(fmt, ...)     QPLog(LOG_DEBUG,     L"[QPDock]", fmt, ##__VA_ARGS__)
#define TRACE_LOG(fmt, ...)     QPLog(LOG_TRACE,     L"[QPDock]", fmt, ##__VA_ARGS__)
#define LOG_RATE(level, key, cooldown, fmt, ...) \
    QPLogRateLimited(level, key, cooldown, L"[QPDock]", fmt, ##__VA_ARGS__)

// DragTraceLog  --  logs every state transition immediately (no rate limit).
// Only called on genuine state changes, never per-frame, so verbosity is controlled.
static void DragTraceLog(const wchar_t* event, const wchar_t* detail = L"") {
    if (detail && detail[0])
        QPLog(LOG_IMPORTANT, L"[DRAG]", L"%s | %s", event, detail);
    else
        QPLog(LOG_IMPORTANT, L"[DRAG]", L"%s", event);
}

static void DragDebugLog(const wchar_t* event, const wchar_t* detail = L"") {
    if (detail && detail[0])
        QPLog(LOG_DEBUG, L"[DRAG]", L"%s | %s", event, detail);
    else
        QPLog(LOG_DEBUG, L"[DRAG]", L"%s", event);
}

static void DragTraceVerboseLog(const wchar_t* event, const wchar_t* detail = L"") {
    if (detail && detail[0])
        QPLog(LOG_TRACE, L"[DRAG]", L"%s | %s", event, detail);
    else
        QPLog(LOG_TRACE, L"[DRAG]", L"%s", event);
}

// ============================================================
//  FORWARD DECLARATIONS
// ============================================================
void  SavePinnedApps();
void  LoadPinnedApps();
void  RepositionOverlay();
RECT  GetIconRectLocal(int index, int totalCount);
int   MaxIconsFit();
static int CountPinsByType(PinType type);   // fwd decl: used by RefreshTaskbarCache (defined later)
bool  IsPinned(const std::wstring& path);
void  PinApp(const std::wstring& path);
void  PinWorkspace(HWND explorerHwnd);
void  UnpinAppByIndex(int i);
void  UnpinAllApps();

static void     GhostCleanup();
static void     GhostDragReset();
static void     HideDragTether();   // fwd decl: hides the drag-tether overlay (defined with tether globals)
static void     TriggerIconVanish(HICON hIcon, int screenX, int screenY, int S); // fwd decl: "Thanos" vanish (defined with vanish globals); used by the finite-rope mid-drag break
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
static void     LaunchWorkspace(const std::wstring& workspaceId);
static void     RestoreExplorerWindowGroup(const WorkspaceSnapshot::WindowGroup& group);
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
static void     ShowPinContextMenu(HWND hwnd, int idx, POINT screenPt);
static void     UpdateWorkspaceSnapshotByIndex(int idx);
static void     RenameWorkspaceByIndex(int idx);
static void     InitSecondaryDocks();
static void     DestroySecondaryDocks();
static void     RepaintSecondaryDocks();
LRESULT CALLBACK SecondaryOverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void     ApplyDockRegion(HWND hwnd);
static bool     IsCursorOverTaskbar(POINT pt);
static HWND     GetRealWindowFromPoint(POINT pt);
static LONG     GetStartButtonLeftEdge(HWND taskbar, const RECT& tbRect);

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK InputOwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD   WINAPI   WorkerThread(LPVOID);

// Zero-rejection resolver
static std::wstring ResolveDragSourceZeroRejection(POINT pt);
static std::wstring Resolver_Layer1_UIHit(POINT pt);
static std::wstring Resolver_Layer2_TaskbarIntelligence(POINT pt);
static std::wstring Resolver_Layer3_ProcessFallback(POINT pt);
static bool     IsExplorerWorkspaceDragSource(POINT pt, HWND* outExplorerHwnd = NULL);
static bool     CaptureWorkspaceSnapshot(WorkspaceSnapshot& snapshot, HWND ownerHwnd = NULL);
static bool     SaveWorkspaceSnapshot(const WorkspaceSnapshot& snapshot);

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
    SECTION_GAP       = (int)(BASE_SECTION_GAP    * s);
    MAGNETIC_RANGE_PX = (int)(BASE_MAGNETIC_RANGE * s);
    GHOST_SIZE        = (int)(BASE_GHOST_SIZE     * s);
    DOCK_GAP_PX       = (int)(DOCK_GAP_FROM_START * s);

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

    // Dock width  --  content-fitted, cached until it needs to change.
    // A pin/unpin marks the width dirty so the dock re-fits its real content
    // (shrink on unpin, grow on pin).  Clearing the cache forces a recompute
    // below.  Also reset the app-region scroll viewport so it never points
    // past the (possibly smaller) new pin count.
    if (g_dockWidthDirty) {
        g_fixedDockWidth = 0;
        g_appScrollStart = 0;
        g_dockWidthDirty = false;
    }

    if (g_fixedDockWidth == 0) {
        // Content-fitted dock width.  The dock SHRINKS to what is actually
        // pinned and GROWS as pins are added, within two bounds:
        //   * App region  : sized for the number of apps actually pinned, but
        //                    never fewer than MIN_APP_SLOTS (a stable minimum
        //                    footprint) and never more than MAX_VISIBLE_APP_SLOTS
        //                    (past that the app region becomes a scroll
        //                    viewport -- see GetIconRectLocal -- so the dock
        //                    stops growing and scroll-wheel nav pages the rest).
        //   * Workspace region : reserves slots ONLY for workspace pins that
        //                    actually exist (0..MAX_WORKSPACE_PINS) -- no
        //                    invisible reserved space when there are none.
        //   * Section gap  : applied ONLY when BOTH regions are populated, so an
        //                    empty workspace region leaves no phantom gap.
        int appPinCount = 0;
        int wsPinCount  = 0;
        if (g_csInitialized) {
            EnterCriticalSection(&g_cs);
            appPinCount = CountPinsByType(PIN_APP);
            wsPinCount  = CountPinsByType(PIN_WORKSPACE);
            LeaveCriticalSection(&g_cs);
        }

        int appSlots = appPinCount;
        if (appSlots < MIN_APP_SLOTS)          appSlots = MIN_APP_SLOTS;
        if (appSlots > MAX_VISIBLE_APP_SLOTS)  appSlots = MAX_VISIBLE_APP_SLOTS;   // cap growth
        int wsSlots  = wsPinCount;
        if (wsSlots < 0)                   wsSlots = 0;
        if (wsSlots > MAX_WORKSPACE_PINS)  wsSlots = MAX_WORKSPACE_PINS;

        int wsRegionW  = (wsSlots > 0)
                         ? (wsSlots * ICON_SIZE) + ((wsSlots - 1) * ICON_SPACING)
                         : 0;
        int appRegionW = (appSlots * ICON_SIZE) + ((appSlots - 1) * ICON_SPACING);
        int sectionGap = (wsSlots > 0 && appPinCount > 0) ? SECTION_GAP : 0;

        g_fixedDockWidth = DOCK_PAD_LEFT
                         + wsRegionW
                         + sectionGap
                         + appRegionW
                         + DOCK_PAD_RIGHT;
        g_fixedDockWidth = std::max(g_fixedDockWidth, MIN_VALID_DOCK_WIDTH);
        g_fixedDockWidth = std::min(g_fixedDockWidth, 900);
    }

    // Anchor dock right edge just left of the Start button
    int dockRight  = (int)(startLeft - DOCK_GAP_PX);
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
            g_pinnedApps[i].hoverScale  = 1.0f;
            g_pinnedApps[i].hoverShiftX = 0.f;
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

    // Stable: apply width changes.  newW is deterministic (it equals
    // g_fixedDockWidth, which is only recomputed on a DPI change or when a
    // pin/unpin marks the width dirty), so ANY difference here is a real
    // content/DPI resize -- never measurement jitter.  Follow it exactly and
    // re-seat every icon so the shrink/grow reflows the slots cleanly.
    if (newW != g_dockLocalW && newW > MIN_VALID_DOCK_WIDTH) {
        g_dockLocalW = newW;
        ReseatIconPositions();
        DEBUG_LOG(L"GEOMETRY: width update w=%d", newW);
        RepositionOverlay();
        if (g_overlayWnd && IsWindow(g_overlayWnd))
            InvalidateRect(g_overlayWnd, NULL, FALSE);
    }
    g_dockLocalH = newH;
}

// ============================================================
//  ICON GEOMETRY  --  overlay-LOCAL coords (origin = dock top-left)
//  Icons anchor to the RIGHT edge and grow leftward.
// ============================================================
static int CountPinsByType(PinType type) {
    int count = 0;
    for (const auto& app : g_pinnedApps)
        if (app.type == type) ++count;
    return count;
}

static int PinOrdinalWithinType(int index, PinType type) {
    int ordinal = 0;
    for (int i = 0; i < index && i < (int)g_pinnedApps.size(); ++i)
        if (g_pinnedApps[i].type == type) ++ordinal;
    return ordinal;
}

RECT GetIconRectLocal(int index, int totalCount) {
    (void)totalCount;
    if (index < 0 || g_dockLocalH <= 0 || g_dockLocalW <= 0)
        return {};

    PinType type = PIN_APP;
    int ordinal = index;
    if (index < (int)g_pinnedApps.size()) {
        type = g_pinnedApps[index].type;
        ordinal = PinOrdinalWithinType(index, type);
    }

    int y = (g_dockLocalH - ICON_SIZE) / 2;
    if (y < 2) y = 2;

    if (type == PIN_APP) {
        // App pins anchor to the RIGHT edge and grow leftward.  When more apps
        // are pinned than MAX_VISIBLE_APP_SLOTS, the dock does NOT keep growing
        // (see the width computation) -- instead the app region becomes a
        // fixed-size VIEWPORT of `vis` slots, and g_appScrollStart selects which
        // contiguous window of app ordinals is currently shown.  Scroll-wheel
        // navigation slides that window.  Ordinals outside the window are parked
        // far off the left edge so they are clipped by the dock's window region
        // and never hit-tested.
        int appCount = CountPinsByType(PIN_APP);
        int vis = appCount;
        if (vis > MAX_VISIBLE_APP_SLOTS) vis = MAX_VISIBLE_APP_SLOTS;
        if (vis < 1) vis = 1;

        int start = g_appScrollStart;
        int maxStart = appCount - vis;
        if (maxStart < 0) maxStart = 0;
        if (start < 0) start = 0;
        if (start > maxStart) start = maxStart;

        int rightEdge = g_dockLocalW - DOCK_PAD_RIGHT;
        if (ordinal < start || ordinal >= start + vis) {
            // SYMMETRICAL OVERFLOW PARKING (premium >10-pin hide/reveal):
            // Icons scrolled PAST the window wait just off the LEFT edge; icons
            // NOT YET reached wait just off the RIGHT edge. Previously every
            // overflow icon parked far off the left (-ICON_SIZE*4), so scrolling
            // forward made the incoming icon teleport across the whole dock --
            // which the retarget code had to mask with a hard snap (the abrupt,
            // "junky" pop). Parking on the correct side keeps every icon's travel
            // to a single slot, so it glides (and fades, see the anim loop) on
            // and off like a balanced conveyor instead of snapping. Both parks
            // sit fully outside [0, dockLocalW) so the window region clips them
            // and they are never hit-tested.
            int hiddenX = (ordinal < start)
                        ? -(ICON_SIZE + ICON_SPACING)   // just off the LEFT edge
                        : g_dockLocalW + ICON_SPACING;  // just off the RIGHT edge
            return { hiddenX, y, hiddenX + ICON_SIZE, y + ICON_SIZE };
        }

        // 0 == rightmost visible slot; larger == further left within the window.
        int windowReverse = (start + vis - 1) - ordinal;
        if (windowReverse < 0) windowReverse = 0;
        int x = rightEdge - ICON_SIZE - (windowReverse * (ICON_SIZE + ICON_SPACING));
        return { x, y, x + ICON_SIZE, y + ICON_SIZE };
    }

    // Workspace pins: left-anchored, always visible (max MAX_WORKSPACE_PINS).
    int x = DOCK_PAD_LEFT + ordinal * (ICON_SIZE + ICON_SPACING);
    return { x, y, x + ICON_SIZE, y + ICON_SIZE };
}

// Maximum icons that can be physically laid out in the current dock width.
int MaxIconsFit() {
    return MAX_WORKSPACE_PINS + MAX_APP_PINS;
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
            g_dockCurrentX = (float)(sl - DOCK_GAP_PX - g_dockLocalW);
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
    if (g_inputWnd && IsWindow(g_inputWnd))
        SetWindowPos(g_inputWnd, HWND_TOPMOST, x, y, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);

    if (g_taskbarAutoHide) {
        bool tbOnScreen = (g_cachedTBRect.bottom - g_cachedTBRect.top) > 8 &&
                          (g_cachedTBRect.right  - g_cachedTBRect.left) > 8;
        // Anti-flicker hysteresis: the taskbar's slide animation briefly reports
        // in-between sizes as it crosses the 8px threshold. Show immediately when
        // the bar is on screen, but require two consecutive off-screen samples
        // before hiding so a single mid-slide frame can't flash the dock.
        if (tbOnScreen) g_autoHideMiss = 0;
        else            ++g_autoHideMiss;
        bool showDock = tbOnScreen || g_autoHideMiss < 2;
        ShowWindow(g_overlayWnd, showDock ? SW_SHOWNOACTIVATE : SW_HIDE);
        if (g_inputWnd && IsWindow(g_inputWnd))
            ShowWindow(g_inputWnd, showDock ? SW_SHOWNOACTIVATE : SW_HIDE);
        SetWindowPos(g_overlayWnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE);
    } else {
        SetWindowPos(g_overlayWnd, HWND_TOPMOST, x, y, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    // Rounded corners: the overlay is a colour-key layered WS_POPUP, so DWM's
    // DWMWA_WINDOW_CORNER_PREFERENCE does NOT visibly round the painted content
    // (it only shapes a real non-client frame, which this window does not have).
    // The ONLY reliable way to round the *visible* pixels of a colour-key
    // layered window is a GDI window region. ApplyDockRegion() computes the
    // radius from CORNER_ROUNDNESS (0 = square .. >=41 = full pill) and clips
    // the window to it. Because SetWindowPos above resized the window, the
    // region must be recomputed for the new size here -- otherwise a stale
    // region would clip to the old bounds. This is what makes the corner-
    // roundness setting actually take visible effect and update live.
    (void)w; (void)h;
    ApplyDockRegion(g_overlayWnd);
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

    g_dockVelocityX = (g_dockVelocityX + dx * timeFactor) * powf(ANIM_MOMENTUM_DECAY, g_frameDeltaMs / 16.0f);
    g_dockVelocityY = (g_dockVelocityY + dy * timeFactor) * powf(ANIM_MOMENTUM_DECAY, g_frameDeltaMs / 16.0f);
    g_dockCurrentX += g_dockVelocityX;
    g_dockCurrentY += g_dockVelocityY;
    return true;
}

// ============================================================
//  ICON LOADING  --  strict; returns NULL if icon cannot be obtained.
//  Always duplicates the icon handle so we own our copy outright.
// ============================================================
static HICON LoadShellImageListIcon(const std::wstring& path, int imageListSize) {
    IImageList* pImgList = NULL;
    if (FAILED(SHGetImageList(imageListSize, IID_IImageList, (void**)&pImgList)) || !pImgList)
        return NULL;

    HICON hIcon = NULL;
    SHFILEINFOW sfi = {};
    DWORD_PTR hr = SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
    if (hr && sfi.iIcon >= 0)
        pImgList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
    pImgList->Release();
    return hIcon;
}

// ============================================================
//  ICON QUALITY GUARD  --  reject blank/hidden and generic placeholder icons
//  so only real, meaningful app icons are ever pinned or shown.
// ============================================================
static bool RasterizeIconRGBA(HICON hIcon, int S, std::vector<BYTE>& out) {
    if (!hIcon || S <= 0) return false;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = S;
    bi.bmiHeader.biHeight      = -S;   // top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC sdc = GetDC(NULL);
    if (!sdc) return false;
    void* bits = NULL;
    HBITMAP dib = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    HDC mdc = CreateCompatibleDC(sdc);
    bool ok = false;
    if (dib && mdc && bits) {
        HGDIOBJ old = SelectObject(mdc, dib);
        memset(bits, 0, (size_t)S * S * 4);
        SetStretchBltMode(mdc, HALFTONE);
        SetBrushOrgEx(mdc, 0, 0, NULL);
        if (DrawIconEx(mdc, 0, 0, hIcon, S, S, 0, NULL, DI_NORMAL)) {
            BYTE* p = (BYTE*)bits;
            out.assign(p, p + (size_t)S * S * 4);
            ok = true;
        }
        SelectObject(mdc, old);
    }
    if (mdc) DeleteDC(mdc);
    if (dib) DeleteObject(dib);
    ReleaseDC(NULL, sdc);
    return ok;
}

static float IconAlphaCoverage(const std::vector<BYTE>& px) {
    if (px.empty()) return 0.f;
    int n = (int)px.size() / 4, c = 0;
    for (int i = 0; i < n; ++i) if (px[(size_t)i * 4 + 3] > 24) ++c;
    return (float)c / (float)n;
}

// Cached alpha silhouette (16x16) of the shell's generic placeholder icon.
static bool g_genericSigReady = false;
static bool g_genericSigValid = false;
static BYTE g_genericSig[256] = {};

static void BuildGenericSig() {
    g_genericSigReady = true;
    SHSTOCKICONINFO sii = {};
    sii.cbSize = sizeof(sii);
    HICON hGen = NULL;
    if (SUCCEEDED(SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON | SHGSI_LARGEICON, &sii)) && sii.hIcon)
        hGen = sii.hIcon;
    if (!hGen) return;
    std::vector<BYTE> px;
    if (RasterizeIconRGBA(hGen, 16, px) && (int)px.size() >= 256 * 4) {
        for (int i = 0; i < 256; ++i) g_genericSig[i] = px[(size_t)i * 4 + 3];
        g_genericSigValid = true;
    }
    DestroyIcon(hGen);
}

static bool IconMatchesGeneric(const std::vector<BYTE>& px16) {
    if (!g_genericSigReady) BuildGenericSig();
    if (!g_genericSigValid || (int)px16.size() < 256 * 4) return false;
    long sad = 0;
    for (int i = 0; i < 256; ++i) {
        int d = (int)px16[(size_t)i * 4 + 3] - (int)g_genericSig[i];
        sad += (d < 0) ? -d : d;
    }
    return sad < 256 * 10;   // near-identical silhouette => generic placeholder
}

// Returns false for blank/hidden icons (almost no opaque pixels) and, when
// checkGeneric is set, for the shell generic placeholder icon.
static bool IconIsUsable(HICON hIcon, bool checkGeneric) {
    if (!hIcon) return false;
    std::vector<BYTE> px32;
    if (!RasterizeIconRGBA(hIcon, 32, px32)) return false;
    if (IconAlphaCoverage(px32) < 0.03f) return false;   // blank / hidden / transparent
    if (checkGeneric) {
        std::vector<BYTE> px16;
        if (RasterizeIconRGBA(hIcon, 16, px16) && IconMatchesGeneric(px16)) return false;
    }
    return true;
}

static HICON LoadAppIconStrict(const std::wstring& path) {
    if (path.empty()) return NULL;

    HICON hSource = NULL;
    bool  prone   = false;   // true if icon came from a generic-substituting fallback

    // Prefer the shell image list closest to the actual drawn size. Starting
    // with 256px jumbo icons and letting GDI downscale to taskbar size causes
    // soft edges; native taskbar rendering picks a size-appropriate source.
    if (ICON_SIZE <= 32) hSource = LoadShellImageListIcon(path, SHIL_LARGE);
    if (!hSource && ICON_SIZE <= 48) hSource = LoadShellImageListIcon(path, SHIL_EXTRALARGE);
    if (!hSource) hSource = LoadShellImageListIcon(path, SHIL_JUMBO);
    if (!hSource) hSource = LoadShellImageListIcon(path, SHIL_EXTRALARGE);
    if (!hSource) hSource = LoadShellImageListIcon(path, SHIL_LARGE);

    // Attempt 3: SHDefExtractIconW -- shell default extractor (handles icon overlays,
    // shell icon handlers, and apps with custom icon extractors like Steam games).
    if (!hSource) {
        HICON hLg = NULL, hSm = NULL;
        if (SUCCEEDED(SHDefExtractIconW(path.c_str(), 0, 0, &hLg, &hSm, 48)) && hLg) {
            hSource = hLg;
            prone   = true;
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
        if (ok && sfi.hIcon) {
            hSource = sfi.hIcon;
            prone   = true;
        }
    }

    if (!hSource) return NULL;

    // FIX (reliable icons): reject blank/hidden icons always, and the shell
    // generic placeholder when the icon came from a fallback that substitutes
    // one. This guarantees only real, meaningful app icons ever get pinned.
    if (!IconIsUsable(hSource, prone)) {
        DestroyIcon(hSource);
        return NULL;
    }

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
            if (StrStrIW(res.c_str(), L"explorer.exe")) res.clear();
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
    if (IsExplorerExePath(procPath) ||
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
                    DragTraceVerboseLog(L"RESOLVER: dock icon", p.c_str());
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
            DragTraceVerboseLog(L"RESOLVER: L2 taskbar", result.c_str());
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
                    DragTraceVerboseLog(L"RESOLVER: L1 hit", result.c_str());
                }
            }
        }
        // L3 only runs off-taskbar and only as a true last resort
        if (result.empty()) {
            result = Resolver_Layer3_ProcessFallback(pt);
            if (!result.empty())
                DragTraceVerboseLog(L"RESOLVER: L3 process enum", result.c_str());
        }
    }

    if (result.empty()) {
        LOG_RATE(LOG_TRACE, L"resolver-miss", 5000,
                 L"RESOLVER MISS: all layers failed near (%d,%d)", pt.x, pt.y);
        return L"";
    }

    // Post-resolution validation
    if (IsExplorerExePath(result))
        return L"";
    if (IsExcludedApp(result)) {
        DragTraceVerboseLog(L"RESOLVER: excluded", result.c_str());
        return L"";
    }
    if (StrStrIW(result.c_str(), L"ApplicationFrameHost.exe")) {
        DragTraceVerboseLog(L"RESOLVER: unresolved UWP host rejected");
        return L"";
    }

    // Require an extractable icon  --  no ghost pins
    HICON testIcon = LoadAppIconStrict(result);
    if (!testIcon) {
        DragDebugLog(L"RESOLVER: no icon  --  rejected", result.c_str());
        return L"";
    }
    DestroyIcon(testIcon);

    DragTraceVerboseLog(L"RESOLVER: OK", result.c_str());
    return result;
}

// Alias kept for call-site readability in the worker thread
static inline std::wstring ResolveDragSourceAtPoint(POINT pt) {
    return ResolveDragSourceZeroRejection(pt);
}

// ============================================================
//  EXPLORER WORKSPACE RESOLVER / STORAGE
// ============================================================
static std::wstring JsonEscape(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size() + 8);
    for (wchar_t ch : s) {
        if (ch == L'\\' || ch == L'"') out += L'\\';
        out += ch;
    }
    return out;
}

static std::wstring JsonUnescape(const std::wstring& s) {
    std::wstring out;
    out.reserve(s.size());
    bool esc = false;
    for (wchar_t ch : s) {
        if (esc) { out += ch; esc = false; }
        else if (ch == L'\\') esc = true;
        else out += ch;
    }
    return out;
}

static std::wstring NormalizeWorkspaceFolderPath(const std::wstring& path) {
    if (path.size() > 2 &&
        ((path[0] >= L'A' && path[0] <= L'Z') || (path[0] >= L'a' && path[0] <= L'z')) &&
        path[1] == L':' && path[2] != L'\\' && path[2] != L'/') {
        return path.substr(0, 2) + L"\\" + path.substr(2);
    }
    return path;
}

static bool EnsureDirectory(const std::wstring& path) {
    if (path.empty()) return false;
    if (CreateDirectoryW(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
        return true;
    return false;
}

static std::wstring WorkspaceRootDir() {
    wchar_t appData[MAX_PATH] = {};
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL,
                                SHGFP_TYPE_CURRENT, appData)))
        return L"";
    std::wstring root = std::wstring(appData) + L"\\WindhawkMods";
    EnsureDirectory(root);
    root += L"\\TaskbarQuickPin";
    EnsureDirectory(root);
    root += L"\\workspaces";
    EnsureDirectory(root);
    return root;
}

static std::wstring WorkspaceFilePath(const std::wstring& workspaceId) {
    std::wstring root = WorkspaceRootDir();
    if (root.empty() || workspaceId.empty()) return L"";
    return root + L"\\" + workspaceId + L".json";
}

static std::wstring MakeWorkspaceId() {
    wchar_t buf[64] = {};
    swprintf_s(buf, L"workspace_%08X_%08X", (unsigned)GetTickCount(), (unsigned)time(NULL));
    return buf;
}

static std::wstring BaseNameFromPath(const std::wstring& path) {
    if (path.empty()) return L"Workspace";
    wchar_t tmp[MAX_PATH] = {};
    wcsncpy_s(tmp, path.c_str(), _TRUNCATE);
    PathRemoveBackslashW(tmp);
    const wchar_t* name = PathFindFileNameW(tmp);
    if (name && name[0]) return name;
    return path;
}

static HICON LoadFolderIcon(const std::wstring& folderPath) {
    SHFILEINFOW idxInfo = {};
    DWORD_PTR idxOk = SHGetFileInfoW(folderPath.c_str(), FILE_ATTRIBUTE_DIRECTORY,
                                     &idxInfo, sizeof(idxInfo),
                                     SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
    if (idxOk) {
        IImageList* imageList = NULL;
        int imageListKind = ICON_SIZE >= 48 ? SHIL_JUMBO : SHIL_EXTRALARGE;
        if (SUCCEEDED(SHGetImageList(imageListKind, IID_IImageList, (void**)&imageList)) &&
            imageList) {
            HICON hi = NULL;
            HRESULT hr = imageList->GetIcon(idxInfo.iIcon, ILD_TRANSPARENT, &hi);
            imageList->Release();
            if (SUCCEEDED(hr) && hi) {
                HICON copy = CopyIcon(hi);
                DestroyIcon(hi);
                if (copy) return copy;
            }
        }
    }

    SHFILEINFOW sfi = {};
    DWORD_PTR ok = SHGetFileInfoW(folderPath.c_str(), FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi),
                                  SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES);
    if (!ok || !sfi.hIcon) return NULL;
    HICON copy = CopyIcon(sfi.hIcon);
    DestroyIcon(sfi.hIcon);
    return copy;
}

static bool SaveWorkspaceSnapshot(const WorkspaceSnapshot& snapshot) {
    std::wstring file = WorkspaceFilePath(snapshot.id);
    if (file.empty()) return false;

    std::wstring body;
    body += L"{\r\n";
    body += L"  \"id\": \"" + JsonEscape(snapshot.id) + L"\",\r\n";
    body += L"  \"displayName\": \"" + JsonEscape(snapshot.displayName) + L"\",\r\n";
    body += L"  \"folders\": [\r\n";
    for (size_t i = 0; i < snapshot.folderPaths.size(); ++i) {
        body += L"    { \"path\": \"" + JsonEscape(snapshot.folderPaths[i]) + L"\" }";
        if (i + 1 < snapshot.folderPaths.size()) body += L",";
        body += L"\r\n";
    }
    body += L"  ],\r\n";
    body += L"  \"windows\": [\r\n";
    for (size_t w = 0; w < snapshot.windows.size(); ++w) {
        const auto& group = snapshot.windows[w];
        wchar_t activeBuf[32] = {};
        swprintf_s(activeBuf, L"%d", group.activeTab);
        body += L"    { \"window\": " + std::to_wstring((unsigned long long)(ULONG_PTR)group.hwnd)
              + L", \"activeTab\": " + activeBuf + L", \"tabs\": [\r\n";
        for (size_t t = 0; t < group.tabPaths.size(); ++t) {
            body += L"      { \"path\": \"" + JsonEscape(group.tabPaths[t]) + L"\" }";
            if (t + 1 < group.tabPaths.size()) body += L",";
            body += L"\r\n";
        }
        body += L"    ] }";
        if (w + 1 < snapshot.windows.size()) body += L",";
        body += L"\r\n";
    }
    body += L"  ]\r\n}\r\n";

    HANDLE h = CreateFileW(file.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return false;
    const wchar_t bom = 0xFEFF;
    DWORD written = 0;
    bool ok = WriteFile(h, &bom, sizeof(bom), &written, NULL) != 0;
    if (ok) {
        DWORD bytes = (DWORD)(body.size() * sizeof(wchar_t));
        ok = WriteFile(h, body.data(), bytes, &written, NULL) != 0 && written == bytes;
    }
    CloseHandle(h);
    return ok;
}

static bool ExtractJsonStringValue(const std::wstring& line,
                                   const wchar_t* key,
                                   std::wstring& value) {
    const wchar_t* k = StrStrIW(line.c_str(), key);
    if (!k) return false;
    const wchar_t* colon = wcschr(k, L':');
    if (!colon) return false;
    const wchar_t* start = wcschr(colon, L'"');
    if (!start) return false;
    ++start;
    std::wstring raw;
    bool esc = false;
    for (const wchar_t* p = start; *p; ++p) {
        if (esc) { raw += L'\\'; raw += *p; esc = false; continue; }
        if (*p == L'\\') { esc = true; continue; }
        if (*p == L'"') { value = raw; return true; }
        raw += *p;
    }
    return false;
}

static bool ExtractJsonIntValue(const std::wstring& line,
                                const wchar_t* key,
                                int& value) {
    const wchar_t* k = StrStrIW(line.c_str(), key);
    if (!k) return false;
    const wchar_t* colon = wcschr(k, L':');
    if (!colon) return false;
    value = _wtoi(colon + 1);
    return true;
}

static bool LoadWorkspaceSnapshot(const std::wstring& workspaceId, WorkspaceSnapshot& snapshot) {
    std::wstring file = WorkspaceFilePath(workspaceId);
    if (file.empty()) return false;

    HANDLE h = CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD size = GetFileSize(h, NULL);
    if (size == INVALID_FILE_SIZE || size < sizeof(wchar_t)) {
        CloseHandle(h);
        return false;
    }
    std::vector<wchar_t> buf(size / sizeof(wchar_t) + 1, 0);
    DWORD read = 0;
    bool ok = ReadFile(h, buf.data(), size, &read, NULL) != 0;
    CloseHandle(h);
    if (!ok || read < sizeof(wchar_t)) return false;

    snapshot = WorkspaceSnapshot();
    snapshot.id = workspaceId;
    wchar_t* text = buf.data();
    if (text[0] == 0xFEFF) ++text;

    std::wstringstream ss(text);
    std::wstring line;
    WorkspaceSnapshot::WindowGroup* currentGroup = NULL;
    while (std::getline(ss, line)) {
        std::wstring v;
        if (snapshot.displayName.empty() &&
            ExtractJsonStringValue(line, L"\"displayName\"", v))
            snapshot.displayName = JsonUnescape(v);
        if (StrStrIW(line.c_str(), L"\"window\"") != NULL) {
            snapshot.windows.push_back(WorkspaceSnapshot::WindowGroup());
            currentGroup = &snapshot.windows.back();
            int active = 0;
            if (ExtractJsonIntValue(line, L"\"activeTab\"", active))
                currentGroup->activeTab = active;
        }
        if (ExtractJsonStringValue(line, L"\"path\"", v)) {
            std::wstring path = NormalizeWorkspaceFolderPath(JsonUnescape(v));
            if (std::find_if(snapshot.folderPaths.begin(), snapshot.folderPaths.end(),
                             [&](const std::wstring& p) { return _wcsicmp(p.c_str(), path.c_str()) == 0; })
                == snapshot.folderPaths.end())
                snapshot.folderPaths.push_back(path);
            if (currentGroup) currentGroup->tabPaths.push_back(path);
        }
    }
    if (snapshot.windows.empty() && !snapshot.folderPaths.empty()) {
        WorkspaceSnapshot::WindowGroup group;
        group.activeTab = 0;
        group.tabPaths = snapshot.folderPaths;
        snapshot.windows.push_back(group);
    }
    return !snapshot.folderPaths.empty();
}

static std::wstring FolderPathFromExplorer(IWebBrowserApp* app) {
    if (!app) return L"";
    BSTR url = NULL;
    if (FAILED(app->get_LocationURL(&url)) || !url || !url[0]) {
        if (url) SysFreeString(url);
        return L"";
    }

    wchar_t path[MAX_PATH] = {};
    DWORD len = ARRAYSIZE(path);
    HRESULT hr = PathCreateFromUrlW(url, path, &len, 0);
    SysFreeString(url);
    if (FAILED(hr) || !path[0]) return L"";

    DWORD attrs = GetFileAttributesW(path);
    if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY))
        return L"";
    return path;
}

static int ActiveExplorerTabIndexFromUIA(HWND hwnd,
                                         const std::vector<std::wstring>& tabPaths) {
    if (!hwnd || !IsWindow(hwnd) || tabPaths.empty()) return 0;

    int active = 0;
    IUIAutomation* automation = NULL;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IUIAutomation, (void**)&automation);
    if (FAILED(hr) || !automation) return active;

    IUIAutomationElement* root = NULL;
    if (SUCCEEDED(automation->ElementFromHandle(hwnd, &root)) && root) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = UIA_TabItemControlTypeId;
        IUIAutomationCondition* cond = NULL;
        if (SUCCEEDED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond)) && cond) {
            IUIAutomationElementArray* items = NULL;
            if (SUCCEEDED(root->FindAll(TreeScope_Subtree, cond, &items)) && items) {
                int length = 0;
                items->get_Length(&length);
                for (int i = 0; i < length; ++i) {
                    IUIAutomationElement* item = NULL;
                    if (FAILED(items->GetElement(i, &item)) || !item) continue;

                    BOOL selected = FALSE;
                    IUIAutomationSelectionItemPattern* sel = NULL;
                    hr = item->GetCurrentPatternAs(UIA_SelectionItemPatternId,
                                                   IID_IUIAutomationSelectionItemPattern,
                                                   (void**)&sel);
                    if (SUCCEEDED(hr) && sel) {
                        sel->get_CurrentIsSelected(&selected);
                        sel->Release();
                    }
                    if (selected) {
                        BSTR name = NULL;
                        if (SUCCEEDED(item->get_CurrentName(&name)) && name && name[0]) {
                            for (int p = 0; p < (int)tabPaths.size(); ++p) {
                                std::wstring base = BaseNameFromPath(tabPaths[p]);
                                if (!base.empty() && StrStrIW(name, base.c_str())) {
                                    active = p;
                                    break;
                                }
                            }
                        }
                        if (name) SysFreeString(name);
                        item->Release();
                        break;
                    }
                    item->Release();
                }
                items->Release();
            }
            cond->Release();
        }
        root->Release();
    }
    automation->Release();
    return active;
}

static bool CaptureWorkspaceSnapshot(WorkspaceSnapshot& snapshot, HWND ownerHwnd) {
    HRESULT hrInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) {
        LOG_ERROR(L"WORKSPACE CAPTURE: CoInitializeEx failed hr=0x%08X", (unsigned)hrInit);
        return false;
    }

    IShellWindows* shellWindows = NULL;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
                                  IID_IShellWindows, (void**)&shellWindows);
    if (FAILED(hr) || !shellWindows) {
        LOG_ERROR(L"WORKSPACE CAPTURE: ShellWindows unavailable hr=0x%08X", (unsigned)hr);
        if (SUCCEEDED(hrInit)) CoUninitialize();
        return false;
    }

    long count = 0;
    shellWindows->get_Count(&count);
    HWND ownerRoot = ownerHwnd ? GetAncestor(ownerHwnd, GA_ROOT) : NULL;
    if (!ownerRoot) ownerRoot = ownerHwnd;

    std::vector<std::wstring> folders;
    std::vector<WorkspaceSnapshot::WindowGroup> groups;
    for (long i = 0; i < count; ++i) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;
        IDispatch* disp = NULL;
        if (SUCCEEDED(shellWindows->Item(v, &disp)) && disp) {
            IWebBrowserApp* app = NULL;
            if (SUCCEEDED(disp->QueryInterface(IID_IWebBrowserApp, (void**)&app)) && app) {
                std::wstring folder = FolderPathFromExplorer(app);
                if (!folder.empty()) {
                    SHANDLE_PTR hwndShell = 0;
                    app->get_HWND(&hwndShell);
                    HWND hwndExplorer = (HWND)hwndShell;
                    HWND hwndRoot = hwndExplorer ? GetAncestor(hwndExplorer, GA_ROOT) : NULL;
                    if (!hwndRoot) hwndRoot = hwndExplorer;
                    if (!ownerRoot || hwndRoot == ownerRoot) {
                        auto groupIt = std::find_if(groups.begin(), groups.end(),
                            [&](const WorkspaceSnapshot::WindowGroup& g) { return g.hwnd == hwndRoot; });
                        if (groupIt == groups.end()) {
                            WorkspaceSnapshot::WindowGroup group;
                            group.hwnd = hwndRoot;
                            groups.push_back(group);
                            groupIt = groups.end() - 1;
                        }
                        groupIt->tabPaths.push_back(folder);

                        if (std::find_if(folders.begin(), folders.end(),
                                         [&](const std::wstring& p) { return _wcsicmp(p.c_str(), folder.c_str()) == 0; })
                            == folders.end())
                            folders.push_back(folder);
                    }
                }
                app->Release();
            }
            disp->Release();
        }
        VariantClear(&v);
    }
    shellWindows->Release();

    if (folders.empty()) {
        if (SUCCEEDED(hrInit)) CoUninitialize();
        LOG_IMPORTANT(L"WORKSPACE CAPTURE: no Explorer folder tabs found");
        return false;
    }

    HWND activeRoot = ownerRoot ? ownerRoot : GetForegroundWindow();
    if (activeRoot) {
        HWND root = GetAncestor(activeRoot, GA_ROOT);
        if (root) activeRoot = root;
    }
    wchar_t activeTitle[512] = {};
    if (activeRoot) GetWindowTextW(activeRoot, activeTitle, ARRAYSIZE(activeTitle));
    for (auto& group : groups) {
        group.activeTab = ActiveExplorerTabIndexFromUIA(group.hwnd, group.tabPaths);
        if (group.hwnd != activeRoot || !activeTitle[0]) continue;
        for (int i = 0; i < (int)group.tabPaths.size(); ++i) {
            std::wstring base = BaseNameFromPath(group.tabPaths[i]);
            if (!base.empty() && StrStrIW(activeTitle, base.c_str())) {
                group.activeTab = i;
                break;
            }
        }
    }

    snapshot.id = MakeWorkspaceId();
    snapshot.folderPaths = folders;
    snapshot.windows = groups;
    snapshot.displayName = BaseNameFromPath(folders[0]);
    if (folders.size() > 1) {
        wchar_t suffix[32] = {};
        swprintf_s(suffix, L" +%u", (unsigned)(folders.size() - 1));
        snapshot.displayName += suffix;
    }
    if (SUCCEEDED(hrInit)) CoUninitialize();
    return true;
}

static bool IsExplorerWorkspaceDragSource(POINT pt, HWND* outExplorerHwnd) {
    if (outExplorerHwnd) *outExplorerHwnd = NULL;
    HWND hit = WindowFromPoint(pt);
    HWND root = hit ? GetAncestor(hit, GA_ROOT) : NULL;
    if (!root) root = hit;
    if (!root) return false;

    HRESULT hrInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) return false;

    bool match = false;
    std::wstring taskbarElementName;
    if (IsCursorOverTaskbar(pt)) {
        IUIAutomation* pAuto = NULL;
        if (SUCCEEDED(CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                       IID_IUIAutomation, (void**)&pAuto)) && pAuto) {
            IUIAutomationElement* el = NULL;
            if (SUCCEEDED(pAuto->ElementFromPoint(pt, &el)) && el) {
                BSTR name = NULL;
                if (SUCCEEDED(el->get_CurrentName(&name)) && name && name[0])
                    taskbarElementName = name;
                if (name) SysFreeString(name);
                el->Release();
            }
            pAuto->Release();
        }
    }

    IShellWindows* shellWindows = NULL;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
                                  IID_IShellWindows, (void**)&shellWindows);
    if (SUCCEEDED(hr) && shellWindows) {
        long count = 0;
        shellWindows->get_Count(&count);
        for (long i = 0; i < count && !match; ++i) {
            VARIANT v;
            VariantInit(&v);
            v.vt = VT_I4;
            v.lVal = i;
            IDispatch* disp = NULL;
            if (SUCCEEDED(shellWindows->Item(v, &disp)) && disp) {
                IWebBrowserApp* app = NULL;
                if (SUCCEEDED(disp->QueryInterface(IID_IWebBrowserApp, (void**)&app)) && app) {
                    SHANDLE_PTR hwndShell = 0;
                    if (SUCCEEDED(app->get_HWND(&hwndShell))) {
                        HWND explorerHwnd = (HWND)hwndShell;
                        HWND explorerRoot = explorerHwnd ? GetAncestor(explorerHwnd, GA_ROOT) : NULL;
                        if (!explorerRoot) explorerRoot = explorerHwnd;
                        if (explorerRoot == root || explorerHwnd == root || IsChild(explorerHwnd, hit)) {
                            match = true;
                            if (outExplorerHwnd) *outExplorerHwnd = explorerRoot;
                        }
                        if (!match && !taskbarElementName.empty()) {
                            wchar_t title[512] = {};
                            GetWindowTextW(explorerRoot, title, ARRAYSIZE(title));
                            bool fileExplorerButton =
                                StrStrIW(taskbarElementName.c_str(), L"File Explorer") != NULL;
                            bool titleMatch = title[0] &&
                                (_wcsicmp(title, taskbarElementName.c_str()) == 0 ||
                                 StrStrIW(title, taskbarElementName.c_str()) != NULL ||
                                 StrStrIW(taskbarElementName.c_str(), title) != NULL);
                            if (fileExplorerButton || titleMatch) {
                                match = true;
                                if (outExplorerHwnd) *outExplorerHwnd = explorerRoot;
                            }
                        }
                    }
                    app->Release();
                }
                disp->Release();
            }
            VariantClear(&v);
        }
        shellWindows->Release();
    }
    if (SUCCEEDED(hrInit)) CoUninitialize();
    return match;
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
        if (app.type == PIN_APP && _wcsicmp(app.exePath.c_str(), p.c_str()) == 0) return true;
    return false;
}

static bool IsWorkspacePinned(const std::wstring& workspaceId) {
    for (const auto& app : g_pinnedApps)
        if (app.type == PIN_WORKSPACE && _wcsicmp(app.workspaceId.c_str(), workspaceId.c_str()) == 0)
            return true;
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

    int cap = std::min(MAX_APP_PINS, std::max(1, MAX_PINNED_APPS));
    if (CountPinsByType(PIN_APP) >= cap) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        TriggerLimitFlash();
        LOG_IMPORTANT(L"PIN BLOCKED limit=%d: %s", cap, path.c_str());
        return;
    }

    PinnedApp app;
    app.type     = PIN_APP;
    app.exePath  = path;
    app.icon     = icon;
    app.opacity  = 0.f;
    app.isNew    = true;
    app.velocityX = 0.f;
    app.hoverScale  = 1.0f;
    app.hoverShiftX = 0.f;

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
    g_dockWidthDirty = true;   // pin count changed -> re-fit dock width (grow)

    LeaveCriticalSection(&g_cs);
    SavePinnedApps();  // Outside CS  --  registry I/O must not block the input loop

    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);

    LOG_IMPORTANT(L"PIN OK: %s (total=%d)", path.c_str(), (int)g_pinnedApps.size());
}

void PinWorkspace(HWND explorerHwnd) {
    if (!ENABLE_EXPLORER_WORKSPACE_PINS) {
        DragTraceLog(L"WORKSPACE PIN REJECT: setting disabled");
        return;
    }

    WorkspaceSnapshot snapshot;
    if (!CaptureWorkspaceSnapshot(snapshot, explorerHwnd)) {
        DragTraceLog(L"WORKSPACE PIN REJECT: no Explorer folders");
        return;
    }
    LOG_IMPORTANT(L"WORKSPACE CAPTURE OK: owner=%p windows=%d folders=%d active=%d",
                  explorerHwnd,
                  (int)snapshot.windows.size(), (int)snapshot.folderPaths.size(),
                  snapshot.windows.empty() ? 0 : snapshot.windows[0].activeTab);
    if (!SaveWorkspaceSnapshot(snapshot)) {
        DragTraceLog(L"WORKSPACE PIN REJECT: save failed", snapshot.id.c_str());
        return;
    }

    HICON icon = LoadFolderIcon(snapshot.folderPaths[0]);
    if (!icon) {
        DragTraceLog(L"WORKSPACE PIN REJECT: no folder icon", snapshot.folderPaths[0].c_str());
        return;
    }

    EnterCriticalSection(&g_cs);
    if (IsWorkspacePinned(snapshot.id)) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        return;
    }

    int cap = MAX_WORKSPACE_PINS;
    if (CountPinsByType(PIN_WORKSPACE) >= cap) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        TriggerLimitFlash();
        LOG_IMPORTANT(L"WORKSPACE PIN BLOCKED limit=%d: %s", cap, snapshot.id.c_str());
        return;
    }

    PinnedApp app;
    app.type        = PIN_WORKSPACE;
    app.workspaceId = snapshot.id;
    app.displayName = snapshot.displayName;
    app.icon        = icon;
    app.opacity     = 0.f;
    app.isNew       = true;
    app.velocityX   = 0.f;
    app.hoverScale  = 1.0f;
    app.hoverShiftX = 0.f;

    int n = (int)g_pinnedApps.size();
    RECT r = GetIconRectLocal(n, n + 1);
    app.targetX = (float)r.left;
    app.currentX = app.targetX + PIN_SLIDE_OFFSET;

    g_pinnedApps.push_back(app);

    int total = (int)g_pinnedApps.size();
    for (int i = 0; i < total; ++i) {
        RECT ri = GetIconRectLocal(i, total);
        g_pinnedApps[i].targetX = (float)ri.left;
    }
    g_dockWidthDirty = true;   // workspace pin count changed -> re-fit dock width

    LeaveCriticalSection(&g_cs);
    SavePinnedApps();

    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);

    LOG_IMPORTANT(L"WORKSPACE PIN OK: %s folders=%d", snapshot.id.c_str(), (int)snapshot.folderPaths.size());
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
    g_draggedPinType = PIN_APP;
    g_lockedDragPinType = PIN_APP;
    g_draggedExplorerHwnd = NULL;
    g_lockedExplorerHwnd = NULL;
    g_dragFromDock    = false;
    g_dragFromDockIdx = -1;
    g_dropZoneActive  = false;
    // Kill any live drag-tether frame so a finished/cancelled drag never leaves
    // a lingering thread on screen (root cause of the "stale thread" artifact).
    HideDragTether();
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
    /* THREAD-SAFETY: g_draggedAppPath / g_lockedDragPath are std::wstrings owned
       by the worker thread's drag state machine.  HardStateReset() runs on the
       MAIN thread (unpin via hotkey / right-click / unpin-all), so clearing them
       here raced with the worker reading/assigning them (non-atomic realloc -
       potential crash).  They are intentionally NOT cleared from this thread; the
       worker re-clears g_draggedAppPath at the next IDLE- transition and the
       stale value is never read while g_dragState == DRAG_IDLE.  Only scalar
       fields (atomic on x86-64) are reset here. */
    g_draggedPinType = PIN_APP;
    g_lockedDragPinType = PIN_APP;
    g_draggedExplorerHwnd = NULL;
    g_lockedExplorerHwnd = NULL;
    g_dropZoneActive = false;
    
    // Snap all icon positions to their targets and reset per-icon state
    g_reorderSrcIdx    = -1;
    g_reorderTargetIdx = -1;
    // FIX (smooth unpin): do NOT snap currentX to targetX here -- let the
    // animation loop ease icons to their targets so reflow stays smooth.
    for (auto& a : g_pinnedApps) {
        a.hoverScale  = 1.0f;
        a.hoverShiftX = 0.f;
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

    LOG_IMPORTANT(L"UNPIN index=%d id=%s",
              i,
              g_pinnedApps[i].type == PIN_WORKSPACE
                  ? g_pinnedApps[i].workspaceId.c_str()
                  : g_pinnedApps[i].exePath.c_str());

    if (g_pinnedApps[i].icon) {
        DestroyIcon(g_pinnedApps[i].icon);
        g_pinnedApps[i].icon = NULL;
    }
    g_pinnedApps.erase(g_pinnedApps.begin() + i);

    // Snap remaining icons to their new target positions (no animation after unpin)
    // FIX (smooth unpin): set only the new targetX and let the per-icon
    // momentum animation slide the remaining icons into place -- previously
    // currentX was snapped too, producing a harsh instant jump.
    int afterSize = (int)g_pinnedApps.size();
    for (int j = 0; j < afterSize; ++j) {
        RECT r = GetIconRectLocal(j, afterSize);
        g_pinnedApps[j].targetX  = (float)r.left;
    }
    g_dockWidthDirty = true;   // pin count changed -> re-fit dock width (shrink)

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
    g_dockWidthDirty = true;   // all pins removed -> re-fit dock width to minimum
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
        if (idx >= 0 && idx < (int)g_pinnedApps.size() && g_pinnedApps[idx].type == PIN_APP)
            path = g_pinnedApps[idx].exePath;
        LeaveCriticalSection(&g_cs);
    }
    if (path.empty()) return;

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpFile = path.c_str();
    sei.nShow  = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}

static void LaunchWorkspace(const std::wstring& workspaceId) {
    WorkspaceSnapshot snapshot;
    if (!LoadWorkspaceSnapshot(workspaceId, snapshot)) {
        DragTraceLog(L"WORKSPACE LAUNCH REJECT: load failed", workspaceId.c_str());
        return;
    }

    int launched = 0;
    for (const auto& group : snapshot.windows) {
        WorkspaceSnapshot::WindowGroup validGroup;
        validGroup.activeTab = group.activeTab;
        for (const auto& folder : group.tabPaths) {
            DWORD attrs = GetFileAttributesW(folder.c_str());
            if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                LOG_IMPORTANT(L"WORKSPACE SKIP missing folder: %s", folder.c_str());
                continue;
            }
            validGroup.tabPaths.push_back(folder);
        }
        if (!validGroup.tabPaths.empty()) {
            RestoreExplorerWindowGroup(validGroup);
            launched++;
        }
    }
    LOG_IMPORTANT(L"WORKSPACE LAUNCH: %s restored=%d", workspaceId.c_str(), launched);
}

static DWORD WINAPI LaunchWorkspaceThread(LPVOID param) {
    std::wstring* workspaceId = (std::wstring*)param;
    if (workspaceId) {
        LaunchWorkspace(*workspaceId);
        delete workspaceId;
    }
    return 0;
}

static void LaunchWorkspaceAsync(const std::wstring& workspaceId) {
    if (workspaceId.empty()) return;
    std::wstring* ownedId = new std::wstring(workspaceId);
    HANDLE h = CreateThread(NULL, 0, LaunchWorkspaceThread, ownedId, 0, NULL);
    if (h) CloseHandle(h);
    else {
        delete ownedId;
        LaunchWorkspace(workspaceId);
    }
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

struct ExplorerTab {
    IWebBrowserApp* app = NULL;
    HWND hwnd = NULL;
    std::wstring folder;
};

static void ReleaseExplorerTabs(std::vector<ExplorerTab>& tabs) {
    for (auto& tab : tabs) {
        if (tab.app) {
            tab.app->Release();
            tab.app = NULL;
        }
    }
    tabs.clear();
}

static std::vector<ExplorerTab> EnumerateExplorerTabs(HWND hwndFilter = NULL) {
    std::vector<ExplorerTab> tabs;

    IShellWindows* shellWindows = NULL;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
                                  IID_IShellWindows, (void**)&shellWindows);
    if (SUCCEEDED(hr) && shellWindows) {
        long count = 0;
        shellWindows->get_Count(&count);
        for (long i = 0; i < count; ++i) {
            VARIANT v;
            VariantInit(&v);
            v.vt = VT_I4;
            v.lVal = i;
            IDispatch* disp = NULL;
            if (SUCCEEDED(shellWindows->Item(v, &disp)) && disp) {
                IWebBrowserApp* app = NULL;
                if (SUCCEEDED(disp->QueryInterface(IID_IWebBrowserApp, (void**)&app)) && app) {
                    std::wstring path = FolderPathFromExplorer(app);
                    SHANDLE_PTR hwndShell = 0;
                    if (!path.empty() && SUCCEEDED(app->get_HWND(&hwndShell))) {
                        HWND hwnd = (HWND)hwndShell;
                        if (!hwndFilter || hwnd == hwndFilter) {
                            ExplorerTab tab;
                            tab.app = app;
                            tab.hwnd = hwnd;
                            tab.folder = path;
                            tabs.push_back(tab);
                            app = NULL; // ownership moved to tabs
                        }
                    }
                    if (app) app->Release();
                }
                disp->Release();
            }
            VariantClear(&v);
        }
        shellWindows->Release();
    } else {
        LOG_ERROR(L"EXPLORER COM: ShellWindows unavailable hr=0x%08X", (unsigned)hr);
    }
    return tabs;
}

static IWebBrowserApp* FindExplorerAppForFolder(const std::wstring& folder, HWND* outHwnd = NULL) {
    IWebBrowserApp* found = NULL;
    std::vector<ExplorerTab> tabs = EnumerateExplorerTabs();
    for (auto& tab : tabs) {
        if (_wcsicmp(tab.folder.c_str(), folder.c_str()) == 0) {
            found = tab.app;
            tab.app = NULL;
            if (outHwnd) *outHwnd = tab.hwnd;
            break;
        }
    }
    ReleaseExplorerTabs(tabs);
    return found;
}

static IWebBrowserApp* WaitForNewExplorerAppForFolder(
    const std::wstring& folder,
    const std::vector<HWND>& existingWindows,
    DWORD timeoutMs,
    HWND* outHwnd = NULL) {
    DWORD start = GetTickCount();
    while (GetTickCount() - start < timeoutMs) {
        std::vector<ExplorerTab> tabs = EnumerateExplorerTabs();
        for (auto& tab : tabs) {
            bool knownWindow = false;
            for (HWND known : existingWindows) {
                if (tab.hwnd == known) {
                    knownWindow = true;
                    break;
                }
            }
            if (!knownWindow && _wcsicmp(tab.folder.c_str(), folder.c_str()) == 0) {
                IWebBrowserApp* app = tab.app;
                tab.app = NULL;
                if (outHwnd) *outHwnd = tab.hwnd;
                ReleaseExplorerTabs(tabs);
                return app;
            }
        }
        ReleaseExplorerTabs(tabs);
        Sleep(80);
    }
    return FindExplorerAppForFolder(folder, outHwnd);
}

static bool NavigateExplorerTab(IWebBrowserApp* app, const std::wstring& folder) {
    if (!app || folder.empty()) return false;
    BSTR target = SysAllocString(folder.c_str());
    if (!target) return false;
    VARIANT empty;
    VariantInit(&empty);
    HRESULT hr = app->Navigate(target, &empty, &empty, &empty, &empty);
    SysFreeString(target);
    if (FAILED(hr))
        LOG_ERROR(L"EXPLORER RESTORE: Navigate failed hr=0x%08X folder=%s", (unsigned)hr, folder.c_str());
    return SUCCEEDED(hr);
}

static IUIAutomationElement* FindNamedDescendant(IUIAutomation* automation,
                                                 IUIAutomationElement* root,
                                                 const wchar_t* name) {
    if (!automation || !root || !name) return NULL;
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(name);
    if (!v.bstrVal) return NULL;

    IUIAutomationCondition* cond = NULL;
    IUIAutomationElement* found = NULL;
    HRESULT hr = automation->CreatePropertyCondition(UIA_NamePropertyId, v, &cond);
    if (SUCCEEDED(hr) && cond) {
        root->FindFirst(TreeScope_Subtree, cond, &found);
        cond->Release();
    }
    VariantClear(&v);
    return found;
}

static bool InvokeExplorerNewTab(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;

    bool ok = false;
    IUIAutomation* automation = NULL;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IUIAutomation, (void**)&automation);
    if (SUCCEEDED(hr) && automation) {
        IUIAutomationElement* root = NULL;
        hr = automation->ElementFromHandle(hwnd, &root);
        if (SUCCEEDED(hr) && root) {
            IUIAutomationElement* newTab = FindNamedDescendant(automation, root, L"New tab");
            if (!newTab)
                newTab = FindNamedDescendant(automation, root, L"Add new tab");
            if (newTab) {
                IUIAutomationInvokePattern* invoke = NULL;
                hr = newTab->GetCurrentPatternAs(UIA_InvokePatternId,
                                                 IID_IUIAutomationInvokePattern,
                                                 (void**)&invoke);
                if (SUCCEEDED(hr) && invoke) {
                    hr = invoke->Invoke();
                    ok = SUCCEEDED(hr);
                    if (!ok)
                        LOG_ERROR(L"EXPLORER TABS: Invoke New tab failed hr=0x%08X", (unsigned)hr);
                    invoke->Release();
                } else {
                    LOG_ERROR(L"EXPLORER TABS: New tab button has no InvokePattern hr=0x%08X", (unsigned)hr);
                }
                newTab->Release();
            } else {
                LOG_ERROR(L"EXPLORER TABS: New tab control not found hwnd=%p", hwnd);
            }
            root->Release();
        } else {
            LOG_ERROR(L"EXPLORER TABS: ElementFromHandle failed hr=0x%08X hwnd=%p", (unsigned)hr, hwnd);
        }
        automation->Release();
    } else {
        LOG_ERROR(L"EXPLORER TABS: CUIAutomation unavailable hr=0x%08X", (unsigned)hr);
    }
    return ok;
}

static IWebBrowserApp* WaitForNewExplorerTab(HWND hwnd,
                                             const std::vector<std::wstring>& knownFolders,
                                             DWORD timeoutMs) {
    DWORD start = GetTickCount();
    while (GetTickCount() - start < timeoutMs) {
        std::vector<ExplorerTab> tabs = EnumerateExplorerTabs(hwnd);
        for (auto& tab : tabs) {
            bool known = false;
            for (const auto& folder : knownFolders) {
                if (_wcsicmp(tab.folder.c_str(), folder.c_str()) == 0) {
                    known = true;
                    break;
                }
            }
            if (!known) {
                IWebBrowserApp* app = tab.app;
                tab.app = NULL;
                ReleaseExplorerTabs(tabs);
                return app;
            }
        }
        ReleaseExplorerTabs(tabs);
        Sleep(80);
    }
    return NULL;
}

static bool FocusExplorerTabByFolder(HWND hwnd, const std::wstring& folder) {
    bool selected = false;
    if (hwnd && IsWindow(hwnd)) {
        IUIAutomation* automation = NULL;
        HRESULT hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                      IID_IUIAutomation, (void**)&automation);
        if (SUCCEEDED(hr) && automation) {
            IUIAutomationElement* root = NULL;
            if (SUCCEEDED(automation->ElementFromHandle(hwnd, &root)) && root) {
                VARIANT v;
                VariantInit(&v);
                v.vt = VT_I4;
                v.lVal = UIA_TabItemControlTypeId;
                IUIAutomationCondition* cond = NULL;
                if (SUCCEEDED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond)) && cond) {
                    IUIAutomationElementArray* items = NULL;
                    if (SUCCEEDED(root->FindAll(TreeScope_Subtree, cond, &items)) && items) {
                        int length = 0;
                        items->get_Length(&length);
                        std::wstring needle = BaseNameFromPath(folder);
                        for (int i = 0; i < length && !selected; ++i) {
                            IUIAutomationElement* item = NULL;
                            if (SUCCEEDED(items->GetElement(i, &item)) && item) {
                                BSTR name = NULL;
                                if (SUCCEEDED(item->get_CurrentName(&name)) && name && name[0] &&
                                    StrStrIW(name, needle.c_str())) {
                                    IUIAutomationSelectionItemPattern* select = NULL;
                                    hr = item->GetCurrentPatternAs(UIA_SelectionItemPatternId,
                                                                   IID_IUIAutomationSelectionItemPattern,
                                                                   (void**)&select);
                                    if (SUCCEEDED(hr) && select) {
                                        selected = SUCCEEDED(select->Select());
                                        select->Release();
                                    }
                                }
                                if (name) SysFreeString(name);
                                item->Release();
                            }
                        }
                        items->Release();
                    }
                    cond->Release();
                }
                root->Release();
            }
            automation->Release();
        }
    }

    if (hwnd && IsWindow(hwnd)) {
        SetForegroundWindow(hwnd);
        return true;
    }
    return selected;
}

static void RestoreExplorerWindowGroup(const WorkspaceSnapshot::WindowGroup& group) {
    if (group.tabPaths.empty()) return;

    HRESULT hrInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) {
        LOG_ERROR(L"EXPLORER RESTORE: CoInitializeEx failed hr=0x%08X", (unsigned)hrInit);
        return;
    }

    LOG_IMPORTANT(L"EXPLORER RESTORE: begin tabs=%d active=%d",
                  (int)group.tabPaths.size(), group.activeTab);

    std::vector<HWND> existingExplorerWindows;
    {
        std::vector<ExplorerTab> existingTabs = EnumerateExplorerTabs();
        for (const auto& tab : existingTabs) {
            if (std::find(existingExplorerWindows.begin(), existingExplorerWindows.end(), tab.hwnd)
                == existingExplorerWindows.end())
                existingExplorerWindows.push_back(tab.hwnd);
        }
        ReleaseExplorerTabs(existingTabs);
    }

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpFile = L"explorer.exe";
    std::wstring explorerArgs = L"/n,\"" + group.tabPaths[0] + L"\"";
    sei.lpParameters = explorerArgs.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) {
        LOG_ERROR(L"EXPLORER RESTORE: ShellExecuteEx failed err=%lu folder=%s",
                  GetLastError(), group.tabPaths[0].c_str());
        if (SUCCEEDED(hrInit)) CoUninitialize();
        return;
    }

    HWND hwnd = NULL;
    IWebBrowserApp* first = WaitForNewExplorerAppForFolder(group.tabPaths[0],
                                                           existingExplorerWindows,
                                                           4500, &hwnd);
    if (!first || !hwnd) {
        if (first) first->Release();
        LOG_ERROR(L"EXPLORER RESTORE: first window attach timeout folder=%s", group.tabPaths[0].c_str());
        if (SUCCEEDED(hrInit)) CoUninitialize();
        return;
    }

    std::vector<std::wstring> knownFolders;
    knownFolders.push_back(group.tabPaths[0]);
    NavigateExplorerTab(first, group.tabPaths[0]);
    first->Release();

    int restored = 1;
    for (size_t i = 1; i < group.tabPaths.size(); ++i) {
        if (!InvokeExplorerNewTab(hwnd)) {
            LOG_ERROR(L"EXPLORER RESTORE: native tab creation failed index=%d folder=%s",
                      (int)i, group.tabPaths[i].c_str());
            continue;
        }

        IWebBrowserApp* tab = WaitForNewExplorerTab(hwnd, knownFolders, 2500);
        if (!tab) {
            LOG_ERROR(L"EXPLORER RESTORE: new tab COM attach timeout index=%d folder=%s",
                      (int)i, group.tabPaths[i].c_str());
            continue;
        }

        if (NavigateExplorerTab(tab, group.tabPaths[i])) {
            restored++;
            knownFolders.push_back(group.tabPaths[i]);
        }
        tab->Release();
    }

    int active = std::max(0, std::min(group.activeTab, (int)group.tabPaths.size() - 1));
    if (active < (int)group.tabPaths.size())
        FocusExplorerTabByFolder(hwnd, group.tabPaths[active]);
    else if (hwnd && IsWindow(hwnd))
        SetForegroundWindow(hwnd);

    LOG_IMPORTANT(L"EXPLORER RESTORE: complete restored=%d requested=%d hwnd=%p",
                  restored, (int)group.tabPaths.size(), hwnd);
    if (SUCCEEDED(hrInit)) CoUninitialize();
}

static void UpdateWorkspaceSnapshotByIndex(int idx) {
    std::wstring workspaceId;
    if (!g_csInitialized) return;
    EnterCriticalSection(&g_cs);
    if (idx >= 0 && idx < (int)g_pinnedApps.size() &&
        g_pinnedApps[idx].type == PIN_WORKSPACE)
        workspaceId = g_pinnedApps[idx].workspaceId;
    LeaveCriticalSection(&g_cs);
    if (workspaceId.empty()) return;

    WorkspaceSnapshot snapshot;
    if (!CaptureWorkspaceSnapshot(snapshot)) {
        DragTraceLog(L"WORKSPACE UPDATE REJECT: no Explorer folders", workspaceId.c_str());
        return;
    }
    LOG_IMPORTANT(L"WORKSPACE UPDATE CAPTURE OK: %s windows=%d folders=%d",
                  workspaceId.c_str(), (int)snapshot.windows.size(),
                  (int)snapshot.folderPaths.size());
    snapshot.id = workspaceId;
    if (!SaveWorkspaceSnapshot(snapshot)) {
        DragTraceLog(L"WORKSPACE UPDATE REJECT: save failed", workspaceId.c_str());
        return;
    }

    HICON icon = LoadFolderIcon(snapshot.folderPaths[0]);
    EnterCriticalSection(&g_cs);
    if (idx >= 0 && idx < (int)g_pinnedApps.size() &&
        g_pinnedApps[idx].type == PIN_WORKSPACE &&
        _wcsicmp(g_pinnedApps[idx].workspaceId.c_str(), workspaceId.c_str()) == 0) {
        g_pinnedApps[idx].displayName = snapshot.displayName;
        if (icon) {
            if (g_pinnedApps[idx].icon) DestroyIcon(g_pinnedApps[idx].icon);
            g_pinnedApps[idx].icon = icon;
            icon = NULL;
        }
    }
    LeaveCriticalSection(&g_cs);
    if (icon) DestroyIcon(icon);
    SavePinnedApps();
    if (g_overlayWnd && IsWindow(g_overlayWnd)) InvalidateRect(g_overlayWnd, NULL, FALSE);
}

struct RenameDialogState {
    HWND edit;
    std::wstring value;
    bool accepted;
};

static LRESULT CALLBACK RenameDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    RenameDialogState* state = (RenameDialogState*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    switch (msg) {
    case WM_CREATE: {
        CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
        state = (RenameDialogState*)cs->lpCreateParams;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)state);
        CreateWindowExW(0, L"STATIC", L"Workspace name", WS_CHILD | WS_VISIBLE,
                        14, 12, 240, 20, hwnd, NULL, GetModuleHandleW(NULL), NULL);
        state->edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", state->value.c_str(),
                                      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                      14, 38, 260, 24, hwnd, (HMENU)1001,
                                      GetModuleHandleW(NULL), NULL);
        CreateWindowExW(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                        118, 76, 74, 26, hwnd, (HMENU)IDOK, GetModuleHandleW(NULL), NULL);
        CreateWindowExW(0, L"BUTTON", L"Cancel", WS_CHILD | WS_VISIBLE,
                        200, 76, 74, 26, hwnd, (HMENU)IDCANCEL, GetModuleHandleW(NULL), NULL);
        SendMessageW(state->edit, EM_SETSEL, 0, -1);
        SetFocus(state->edit);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK && state) {
            wchar_t buf[128] = {};
            GetWindowTextW(state->edit, buf, ARRAYSIZE(buf));
            state->value = buf;
            state->accepted = true;
            DestroyWindow(hwnd);
            return 0;
        }
        if (LOWORD(wParam) == IDCANCEL) {
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool PromptWorkspaceName(HWND owner, const std::wstring& currentName, std::wstring& outName) {
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = RenameDialogProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"QPDockRenameDialog";
    wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
    RegisterClassExW(&wc);

    RenameDialogState state = {};
    state.value = currentName;

    RECT ownerRect = {};
    if (owner) GetWindowRect(owner, &ownerRect);
    int x = ownerRect.left + 20;
    int y = ownerRect.top - 124;
    if (y < 20) y = ownerRect.bottom + 8;

    HWND dlg = CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                               L"QPDockRenameDialog", L"Rename Workspace",
                               WS_POPUP | WS_CAPTION | WS_SYSMENU,
                               x, y, 300, 140, owner, NULL, GetModuleHandleW(NULL), &state);
    if (!dlg) return false;
    EnableWindow(owner, FALSE);
    ShowWindow(dlg, SW_SHOWNORMAL);

    MSG msg;
    while (IsWindow(dlg) && GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (!IsDialogMessageW(dlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    EnableWindow(owner, TRUE);
    SetForegroundWindow(owner);
    if (!state.accepted || state.value.empty()) return false;
    outName = state.value;
    return true;
}

static void RenameWorkspaceByIndex(int idx) {
    std::wstring workspaceId;
    std::wstring currentName;
    EnterCriticalSection(&g_cs);
    if (idx >= 0 && idx < (int)g_pinnedApps.size() &&
        g_pinnedApps[idx].type == PIN_WORKSPACE) {
        workspaceId = g_pinnedApps[idx].workspaceId;
        currentName = g_pinnedApps[idx].displayName;
    }
    LeaveCriticalSection(&g_cs);
    if (workspaceId.empty()) return;

    std::wstring newName;
    if (!PromptWorkspaceName(g_overlayWnd, currentName, newName)) return;

    WorkspaceSnapshot snapshot;
    if (LoadWorkspaceSnapshot(workspaceId, snapshot)) {
        snapshot.displayName = newName;
        SaveWorkspaceSnapshot(snapshot);
    }

    EnterCriticalSection(&g_cs);
    if (idx >= 0 && idx < (int)g_pinnedApps.size() &&
        g_pinnedApps[idx].type == PIN_WORKSPACE &&
        _wcsicmp(g_pinnedApps[idx].workspaceId.c_str(), workspaceId.c_str()) == 0)
        g_pinnedApps[idx].displayName = newName;
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();
}

static void ShowPinContextMenu(HWND hwnd, int idx, POINT screenPt) {
    if (idx < 0 || !g_csInitialized) return;

    PinType type = PIN_APP;
    bool valid = false;
    EnterCriticalSection(&g_cs);
    if (idx >= 0 && idx < (int)g_pinnedApps.size()) {
        type = g_pinnedApps[idx].type;
        valid = true;
    }
    LeaveCriticalSection(&g_cs);
    if (!valid) return;

    enum {
        IDM_OPEN = 41001,
        IDM_UNPIN,
        IDM_UNPIN_ALL,
        IDM_UPDATE_WORKSPACE,
        IDM_RENAME_WORKSPACE,
        IDM_REVEAL_WORKSPACE
    };

    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    if (type == PIN_WORKSPACE) {
        AppendMenuW(menu, MF_STRING, IDM_OPEN, L"Restore Workspace");
        AppendMenuW(menu, MF_STRING, IDM_UPDATE_WORKSPACE, L"Update Snapshot");
        AppendMenuW(menu, MF_STRING, IDM_RENAME_WORKSPACE, L"Rename Workspace");
        AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(menu, MF_STRING, IDM_UNPIN, L"Unpin Workspace");
        AppendMenuW(menu, MF_STRING, IDM_UNPIN_ALL, L"Unpin All");
    } else {
        AppendMenuW(menu, MF_STRING, IDM_OPEN, L"Open");
        AppendMenuW(menu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(menu, MF_STRING, IDM_UNPIN, L"Unpin");
        AppendMenuW(menu, MF_STRING, IDM_UNPIN_ALL, L"Unpin All");
    }

    SetForegroundWindow(hwnd);
    UINT cmd = TrackPopupMenu(menu,
                              TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NOANIMATION,
                              screenPt.x, screenPt.y, 0, hwnd, NULL);
    DestroyMenu(menu);

    switch (cmd) {
    case IDM_OPEN:
        SmartLaunch(idx);
        break;
    case IDM_UPDATE_WORKSPACE:
        UpdateWorkspaceSnapshotByIndex(idx);
        break;
    case IDM_RENAME_WORKSPACE:
        RenameWorkspaceByIndex(idx);
        break;
    case IDM_UNPIN:
        UnpinAppByIndex(idx);
        break;
    case IDM_UNPIN_ALL:
        UnpinAllApps();
        break;
    default:
        break;
    }
}

// Smart launch: focus if running, otherwise launch (rate-limited).
static void SmartLaunch(int idx) {
    // Snapshot the path under CS before any long-running operation.
    // A hotkey unpin on the main thread can call UnpinAppByIndex (erase) while
    // we are inside FindRunningAppWindow (EnumWindows)  --  that would reallocate
    // the vector and leave a dangling reference if we held a const& instead.
    std::wstring path;
    std::wstring workspaceId;
    PinType type = PIN_APP;
    if (!g_csInitialized) return;
    EnterCriticalSection(&g_cs);
    if (idx < 0 || idx >= (int)g_pinnedApps.size()) {
        LeaveCriticalSection(&g_cs);
        return;
    }
    type = g_pinnedApps[idx].type;
    path = g_pinnedApps[idx].exePath;             // value copy  --  safe after CS release
    workspaceId = g_pinnedApps[idx].workspaceId;  // value copy  --  safe after CS release
    LeaveCriticalSection(&g_cs);

    if (type == PIN_WORKSPACE) {
        // Idempotency guard: a single physical click can re-enter this handler
        // across a few worker-loop iterations (8-50 ms each). Unlike the app
        // "new instance" path below, the workspace restore had no limiter, so a
        // single click could restore a workspace 2-3 times. Collapse duplicate
        // invocations for the same icon within SMARTLAUNCH_DEBOUNCE_MS into one.
        DWORD wsNow = GetTickCount();
        if (idx == g_lastSmartLaunchIdx &&
            (wsNow - g_lastSmartLaunchTime) < SMARTLAUNCH_DEBOUNCE_MS) {
            DragTraceLog(L"SMARTLAUNCH: duplicate workspace click debounced");
            return;
        }
        g_lastSmartLaunchIdx  = idx;
        g_lastSmartLaunchTime = wsNow;
        if (!workspaceId.empty()) LaunchWorkspaceAsync(workspaceId);
        return;
    }

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
    for (const auto& app : g_pinnedApps) {
        if (app.type == PIN_WORKSPACE) {
            multiSz += L"WORKSPACE|";
            multiSz += app.workspaceId;
            multiSz += L"|";
            multiSz += app.displayName;
        } else {
            multiSz += L"APP|";
            multiSz += app.exePath;
        }
        multiSz += L'\0';
    }
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
        bool invalid = !g_pinnedApps[i].icon;
        if (!invalid && g_pinnedApps[i].type == PIN_WORKSPACE) {
            WorkspaceSnapshot snapshot;
            invalid = g_pinnedApps[i].workspaceId.empty()
                   || !LoadWorkspaceSnapshot(g_pinnedApps[i].workspaceId, snapshot);
        } else if (!invalid) {
            invalid = g_pinnedApps[i].exePath.empty()
                   || IsExplorerExePath(g_pinnedApps[i].exePath)
                   || IsExcludedApp(g_pinnedApps[i].exePath);
        }
        if (invalid) {
            if (g_pinnedApps[i].icon) DestroyIcon(g_pinnedApps[i].icon);
            g_pinnedApps.erase(g_pinnedApps.begin() + i);
            LOG_IMPORTANT(L"VALIDATE: removed invalid entry at index %d", i);
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

            int appCap = std::min(MAX_APP_PINS, std::max(1, MAX_PINNED_APPS));
            int workspaceCap = MAX_WORKSPACE_PINS;

            while (*p) {
                std::wstring record = p;
                p += wcslen(p) + 1;

                PinType type = PIN_APP;
                std::wstring exePath;
                std::wstring workspaceId;
                std::wstring displayName;

                if (record.rfind(L"APP|", 0) == 0) {
                    exePath = record.substr(4);
                } else if (record.rfind(L"WORKSPACE|", 0) == 0) {
                    type = PIN_WORKSPACE;
                    size_t idStart = 10;
                    size_t sep = record.find(L'|', idStart);
                    if (sep == std::wstring::npos) {
                        workspaceId = record.substr(idStart);
                    } else {
                        workspaceId = record.substr(idStart, sep - idStart);
                        displayName = record.substr(sep + 1);
                    }
                } else {
                    // Backward compatibility: pre-workspace versions stored raw paths.
                    exePath = record;
                }

                if (type == PIN_WORKSPACE) {
                    if (CountPinsByType(PIN_WORKSPACE) >= workspaceCap) continue;
                    WorkspaceSnapshot snapshot;
                    if (!LoadWorkspaceSnapshot(workspaceId, snapshot)) {
                        LOG_IMPORTANT(L"LOAD SKIP (workspace missing): %s", workspaceId.c_str());
                        continue;
                    }
                    HICON icon = LoadFolderIcon(snapshot.folderPaths[0]);
                    if (!icon) {
                        LOG_IMPORTANT(L"LOAD SKIP (workspace no icon): %s", workspaceId.c_str());
                        continue;
                    }
                    PinnedApp app;
                    app.type        = PIN_WORKSPACE;
                    app.workspaceId = workspaceId;
                    app.displayName = displayName.empty() ? snapshot.displayName : displayName;
                    app.icon        = icon;
                    app.opacity     = 1.0f;
                    app.isNew       = false;
                    app.velocityX   = 0.f;
                    g_pinnedApps.push_back(app);
                    DEBUG_LOG(L"LOAD WORKSPACE OK: %s", workspaceId.c_str());
                    continue;
                }

                if (CountPinsByType(PIN_APP) >= appCap) continue;
                if (IsExplorerExePath(exePath) || IsExcludedApp(exePath)) continue;

                HICON icon = LoadAppIconStrict(exePath);
                if (!icon) {
                    DEBUG_LOG(L"LOAD SKIP (no icon): %s", exePath.c_str());
                    continue;
                }

                PinnedApp app;
                app.type      = PIN_APP;
                app.exePath   = exePath;
                app.icon      = icon;
                app.opacity   = 1.0f;   // Pre-existing pins start fully visible
                app.isNew     = false;  // No entrance animation
                app.velocityX = 0.f;
                app.hoverScale  = 1.0f;
                app.hoverShiftX = 0.f;
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
        g_pinnedApps[i].hoverScale  = 1.0f;
        g_pinnedApps[i].hoverShiftX = 0.f;
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
    if ((g_draggedPinType == PIN_APP && IsExcludedApp(g_draggedAppPath)) || !g_dragGhostIcon) {
        ShowWindow(g_ghostWnd, SW_HIDE);
        return;
    }
    // Ghost-icon visibility scope (user spec):
    //   * UNPIN drag (dragging a pinned dock icon OFF -- g_dragFromDock == true):
    //     show the ghost across the ENTIRE screen, so the user sees the icon
    //     they are tearing off wherever the cursor travels.
    //   * PIN drag (dragging an app IN to pin it -- g_dragFromDock == false):
    //     keep the ghost restricted to the dock / taskbar region so NO icon
    //     trails the cursor out over app windows (deliberate -- avoids the ugly
    //     software-icon-follows-cursor feel while pinning).
    if (!g_dragFromDock && !IsCursorInDockOrTaskbarRegion(cursorPt)) {
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

static void DrawWorkspaceGlyph(HDC hdc, const RECT& iconRect, BYTE alpha = 255) {
    if (!hdc) return;

    int w = iconRect.right - iconRect.left;
    int h = iconRect.bottom - iconRect.top;
    if (w <= 0 || h <= 0) return;

    int bw = std::max(10, w / 3);
    int bh = std::max(8, h / 4);
    int bx = iconRect.right - bw - 1;
    int by = iconRect.bottom - bh - 1;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = bw;
    bi.bmiHeader.biHeight = -bh;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    BYTE* bits = NULL;
    HDC screen = GetDC(NULL);
    if (!screen) return;
    HBITMAP dib = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, (void**)&bits, NULL, 0);
    HDC mem = dib ? CreateCompatibleDC(screen) : NULL;
    ReleaseDC(NULL, screen);
    if (!dib || !mem || !bits) {
        if (mem) DeleteDC(mem);
        if (dib) DeleteObject(dib);
        return;
    }

    memset(bits, 0, (size_t)bw * bh * 4);
    int radius = std::max(3, bh / 2);
    for (int y = 0; y < bh; ++y) {
        for (int x = 0; x < bw; ++x) {
            int cx = x < radius ? radius : (x >= bw - radius ? bw - radius - 1 : x);
            int cy = y < radius ? radius : (y >= bh - radius ? bh - radius - 1 : y);
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= radius * radius) {
                BYTE a = (BYTE)((170 * alpha) / 255);
                BYTE* px = bits + ((size_t)y * bw + x) * 4;
                px[0] = (BYTE)((28 * a) / 255);
                px[1] = (BYTE)((34 * a) / 255);
                px[2] = (BYTE)((46 * a) / 255);
                px[3] = a;
            }
        }
    }
    for (int y = bh / 3; y <= (bh / 3) + 1 && y < bh - 1; ++y) {
        for (int x = 3; x < bw - 3; ++x) {
            BYTE a = (BYTE)((220 * alpha) / 255);
            BYTE* px = bits + ((size_t)y * bw + x) * 4;
            px[0] = (BYTE)((235 * a) / 255);
            px[1] = (BYTE)((240 * a) / 255);
            px[2] = (BYTE)((248 * a) / 255);
            px[3] = a;
        }
    }
    for (int y = (bh * 2) / 3; y <= ((bh * 2) / 3) + 1 && y < bh - 1; ++y) {
        for (int x = 3; x < bw - 4; ++x) {
            BYTE a = (BYTE)((190 * alpha) / 255);
            BYTE* px = bits + ((size_t)y * bw + x) * 4;
            px[0] = (BYTE)((220 * a) / 255);
            px[1] = (BYTE)((228 * a) / 255);
            px[2] = (BYTE)((238 * a) / 255);
            px[3] = a;
        }
    }

    HBITMAP old = (HBITMAP)SelectObject(mem, dib);
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(hdc, bx, by, bw, bh, mem, 0, 0, bw, bh, bf);
    SelectObject(mem, old);
    DeleteDC(mem);
    DeleteObject(dib);
}

// Reads the current Windows app theme (Personalize\AppsUseLightTheme).
static bool IsSystemLightTheme() {
    DWORD val = 0, sz = sizeof(val);
    HKEY k = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &k) == ERROR_SUCCESS) {
        RegQueryValueExW(k, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&val, &sz);
        RegCloseKey(k);
    }
    return val != 0;
}

// Clips the overlay to a rounded "pill" using a GDI window region. A window
// region clips BOTH the DWM acrylic backdrop AND the drop shadow to the pill,
// so neither the frosted material nor the shadow can leak outside the dock.
// Radius is continuous, driven by CORNER_ROUNDNESS (0 = square .. >=41 = full
// half-height radius), so corner roundness is smooth rather than 3-step.
static void ApplyDockRegion(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;
    int radius;
    // Continuous radius across the whole 0..100 range so every slider step
    // visibly changes the corner (previously 41..100 all collapsed to the same
    // full pill, so the top ~60% of the slider appeared to do nothing).
    // 0 = square, 100 = full pill (half the dock height).
    if (CORNER_ROUNDNESS <= 0) radius = 0;
    else                       radius = (int)lroundf((h * 0.5f) * (CORNER_ROUNDNESS / 100.0f));
    HRGN rgn = (radius <= 0) ? CreateRectRgn(0, 0, w, h)
                             : CreateRoundRectRgn(0, 0, w + 1, h + 1, radius * 2, radius * 2);
    if (rgn) SetWindowRgn(hwnd, rgn, TRUE);  // window takes ownership of the region
}

// ---- Win11 acrylic blur-behind (undocumented accent API) for tinted frosted glass ----
enum QP_ACCENT_STATE { QP_ACCENT_DISABLED = 0, QP_ACCENT_ENABLE_ACRYLICBLURBEHIND = 4 };
struct QP_ACCENT_POLICY { int AccentState; int AccentFlags; unsigned int GradientColor; int AnimationId; };
struct QP_WINCOMPATTRDATA { int Attrib; PVOID pvData; SIZE_T cbData; };
typedef BOOL (WINAPI *QP_pSetWindowCompositionAttribute)(HWND, QP_WINCOMPATTRDATA*);
// ABGR packing for GradientColor: 0xAABBGGRR
// [[maybe_unused]]: retained as documentation of the accent GradientColor byte
// order; its only former caller (SetDockAcrylic) was removed, so silence
// -Wunused-function without deleting the reference helper.
[[maybe_unused]] static inline unsigned int QP_ABGR(int a, int r, int g, int b) {
    return ((unsigned)(a) << 24) | ((unsigned)(b) << 16) | ((unsigned)(g) << 8) | (unsigned)(r);
}
static QP_pSetWindowCompositionAttribute QP_GetSetWCA() {
    static QP_pSetWindowCompositionAttribute fn = (QP_pSetWindowCompositionAttribute)-1;
    if (fn == (QP_pSetWindowCompositionAttribute)-1) {
        HMODULE u = GetModuleHandleW(L"user32.dll");
        fn = u ? (QP_pSetWindowCompositionAttribute)GetProcAddress(u, "SetWindowCompositionAttribute") : NULL;
    }
    return fn;
}
// NOTE: SetDockAcrylic() (which enabled ACCENT_ENABLE_ACRYLICBLURBEHIND) was
// removed. On real Windows 11 hardware that accent painted an opaque BLACK slab
// behind the colour-keyed layered dock (see ApplyDockGlassTint below). Only the
// DISABLE path is kept.
// FIX (glass-off = black): fully DISABLE the acrylic accent. Previously the
// glass-off path called SetDockAcrylic(hwnd, QP_ABGR(0,0,0,0)), but that still
// sets AccentState = QP_ACCENT_ENABLE_ACRYLICBLURBEHIND, so DWM drew a black
// (zero-gradient) acrylic slab. Setting AccentState = QP_ACCENT_DISABLED removes
// the material entirely, letting the colour-key layered dock render normally.
static void DisableDockAcrylic(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    QP_pSetWindowCompositionAttribute fn = QP_GetSetWCA();
    if (!fn) return;
    QP_ACCENT_POLICY policy = {};
    policy.AccentState = QP_ACCENT_DISABLED;
    policy.AccentFlags = 0;
    policy.GradientColor = 0;
    QP_WINCOMPATTRDATA data = { 19 /*WCA_ACCENT_POLICY*/, &policy, sizeof(policy) };
    fn(hwnd, &data);
}
// Pick a state-based tint and apply it to the given dock window. Alpha stays low
// so red/green/yellow read as blurry coloured glass, not solid colour.
static void ApplyDockGlassTint(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    // FIX (black background slab on real GPUs -- clean in VM, ugly on hardware):
    // The frosted look + coloured drag feedback used to be produced by the
    // undocumented ACCENT_ENABLE_ACRYLICBLURBEHIND accent (SetDockAcrylic). On a
    // VM there is no hardware compositor, so DWM silently ignores that accent and
    // the dock fell back to pure LWA_COLORKEY transparency -- which is exactly why
    // it looked clean there. On a real Windows 11 machine the accent DOES take
    // effect, but ACCENT_ENABLE_ACRYLICBLURBEHIND no longer resolves to
    // translucent blur behind a colour-keyed layered window: it composites against
    // an opaque backing and paints a solid BLACK slab under the icons and the drag
    // "rope". A LWA_COLORKEY window can only do binary (all-or-nothing)
    // transparency and cannot carry a translucent backdrop, so mixing the two is
    // unsupported. We therefore DISABLE the acrylic accent unconditionally and let
    // the background stay purely colour-key transparent -- identical, clean output
    // on both the VM and real hardware. The subtle glass rim is still drawn by
    // DrawGlassEdge in WM_PAINT.
    //
    // This is intentionally a no-op now: the acrylic accent is never enabled
    // anywhere (SetDockAcrylic was removed), and it is defensively disabled ONCE
    // at window creation in ApplyNativeBackdrop(). Keeping this a no-op avoids a
    // per-frame SetWindowCompositionAttribute() call from WM_PAINT, which could
    // trigger needless recomposition and hurt the smooth feel during animations.
    (void)hwnd;
}

static void ApplyNativeBackdrop(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    // FIX (transparent dock): The DWM system backdrop (DWMSBT_TRANSIENTWINDOW +
    // DwmExtendFrameIntoClientArea) painted an opaque acrylic material behind the
    // whole overlay -- an ugly grey (~#545454) in dark mode and a milky wash in
    // light mode. The dock is a per-pixel colour-keyed layered window, so it must
    // have NO material behind it: fully transparent in BOTH light and dark themes.
    // The subtle frosted-glass tint is still drawn manually in WM_PAINT during
    // interactions (drop / reorder / limit flash), so nothing is lost visually.
    //
    // We also stop force-setting DWMWA_USE_IMMERSIVE_DARK_MODE = TRUE: the overlay
    // is WS_POPUP (no non-client area), so it only served to darken the backdrop
    // we are now disabling, and forcing it hurt the light-theme appearance.
    // FIX (macOS-style glass): use a REAL DWM acrylic backdrop for genuine
    // frosted-blur translucency, matched to the SYSTEM theme -- NOT forced dark
    // (which previously produced the flat, ugly grey ~#545454). Light theme =>
    // a light frosted panel; dark theme => a dark frosted-glass panel. Together
    // with the pill window region and the GDI edge sheen (DrawGlassEdge) this
    // yields an Apple-like glass dock that looks right in both themes.
    BOOL dark = IsSystemLightTheme() ? FALSE : TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    // FIX (smooth corners): let DWM round the window corners with proper
    // anti-aliasing instead of a hard 1-bit GDI region (which looked jagged).
    // Corner shape is user-configurable (cornerRoundness 0..100): 0 = square,
    // low = small radius, high = full radius. Every level is DWM-composited so
    // it stays anti-aliased (no jagged 1-bit region). Re-applied live from the
    // settings-changed handler, so no Explorer restart is required.
    int corner = (CORNER_ROUNDNESS <= 0)  ? DWMWCP_DONOTROUND
               : (CORNER_ROUNDNESS <= 40) ? DWMWCP_ROUNDSMALL
                                          : DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    // FIX (leaking blur/shadow outside the dock): DWMSBT_TRANSIENTWINDOW draws
    // its acrylic material behind the ENTIRE window bounds, and this overlay is a
    // WS_EX_LAYERED colour-key window whose background pixels are punched fully
    // transparent. DWM therefore showed the frosted material THROUGH the keyed
    // pixels and cast a drop shadow AROUND the whole window rectangle -- the
    // grey halo / shadow leaking beyond the visible icons. A system backdrop is
    // semantically mismatched with a colour-key layered window, so we disable it
    // and render the frosted tint ourselves in WM_PAINT (confined to the client
    // rect, so nothing can leak outside the painted dock).
    // FIX (dark 'lock' panel under the dock): a WS_EX_LAYERED colour-key window
    // must NOT carry a DWM system backdrop. DWMSBT_TRANSIENTWINDOW draws its
    // acrylic material behind the ENTIRE window rect and shows through the keyed
    // (transparent) pixels as a dark rounded panel + shadow halo -- the "lock"
    // background the user reported. The frosted look is already produced by the
    // ACRYLICBLURBEHIND accent applied in ApplyDockGlassTint(), which is confined
    // to the pill region, so the system backdrop is redundant AND harmful here.
    // Force it off in both glass states.
    int backdrop = DWMSBT_NONE;
    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
    // No sheet-of-glass frame extension: MARGINS{-1,...} gave the window a
    // full-surface glass frame AND a drop shadow that leaked around the dock as
    // a dark halo. Keep margins zero so DWM adds neither a backdrop nor a shadow.
    MARGINS margins = { 0, 0, 0, 0 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    // Clip the (now shadow-only) DWM effects to the rounded pill so nothing leaks out.
    ApplyDockRegion(hwnd);
    // Defensively clear the acrylic blur-behind accent ONCE at setup. On real
    // Windows 11 hardware that accent rendered a solid BLACK slab behind the
    // colour-keyed layered dock (fine in a VM, where the compositor ignores it),
    // so we never enable it and make sure any stale accent is removed. The dock's
    // background therefore stays purely LWA_COLORKEY transparent everywhere.
    DisableDockAcrylic(hwnd);
}

static void DestroyPaintBuffer() {
    if (g_paintDC && g_paintOldBmp) {
        SelectObject(g_paintDC, g_paintOldBmp);
        g_paintOldBmp = NULL;
    }
    if (g_paintDIB) {
        DeleteObject(g_paintDIB);
        g_paintDIB = NULL;
    }
    if (g_paintDC) {
        DeleteDC(g_paintDC);
        g_paintDC = NULL;
    }
    g_paintCachedW = 0;
    g_paintCachedH = 0;
}

static bool EnsurePaintBuffer(HDC refDC, int w, int h) {
    if (!refDC || w <= 0 || h <= 0) return false;
    if (g_paintDC && g_paintDIB && g_paintCachedW == w && g_paintCachedH == h)
        return true;

    DestroyPaintBuffer();

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = NULL;
    g_paintDIB = CreateDIBSection(refDC, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!g_paintDIB) return false;

    g_paintDC = CreateCompatibleDC(refDC);
    if (!g_paintDC) {
        DeleteObject(g_paintDIB);
        g_paintDIB = NULL;
        return false;
    }

    g_paintOldBmp = (HBITMAP)SelectObject(g_paintDC, g_paintDIB);
    g_paintCachedW = w;
    g_paintCachedH = h;
    return true;
}

static void PresentPaintBuffer(HDC targetDC, int w, int h) {
    if (targetDC && g_paintDC && w > 0 && h > 0)
        BitBlt(targetDC, 0, 0, w, h, g_paintDC, 0, 0, SRCCOPY);
}

// macOS-style glass edge: a hairline rounded border plus a soft top sheen
// (the classic light-catch), drawn on top of the DWM acrylic panel. Theme-
// aware so it reads cleanly on both light and dark taskbars.
static void DrawGlassEdge(HDC hdc, const RECT& cr) {
    if (!ENABLE_GLASS_OVERLAY) return;
    // REMOVED (on request): the thin "top sheen" hairline that used to be
    // stroked across the dock's upper edge (cr.top + 2). It read as an
    // unnecessary thin line -- easily mistaken for a slider/control -- and the
    // glass look is already fully provided by the DWM acrylic panel + rounded
    // corner region, so no GDI edge line is drawn any more.
    (void)hdc; (void)cr;
}

// ============================================================
//  LOCK INDICATOR  --  breathing gold edge glow
//  Drawn only while the dock is locked (g_iconsLocked, toggled by triple-tap
//  L). It traces the dock's rounded outline in gold and gently pulses its
//  brightness so the locked state reads at a glance without a settings toggle.
//  Works regardless of the glass setting. The overlay is a colour-key layered
//  window (opaque for any non-key pixel), so the glow is a solid stroke whose
//  *brightness* breathes rather than a true alpha bloom -- we fake depth with a
//  thicker, dimmer outer band under a bright inner line. The outline radius is
//  computed exactly like ApplyDockRegion so it hugs the rounded corners.
// ============================================================
static void DrawLockGlow(HDC hdc, const RECT& cr) {
    // Interactive flash only: draw nothing unless a flash is currently in
    // flight. A locked-but-idle dock therefore looks completely natural.
    if (g_lockGlowStart == 0) return;
    DWORD elapsed = GetTickCount() - g_lockGlowStart;
    if (elapsed >= (DWORD)LOCK_GLOW_MS) { g_lockGlowStart = 0; return; }

    int w = cr.right - cr.left, h = cr.bottom - cr.top;
    if (w <= 4 || h <= 4) return;

    int radius = (CORNER_ROUNDNESS <= 0)
                 ? 0
                 : (int)lroundf((h * 0.5f) * (CORNER_ROUNDNESS / 100.0f));

    // One smooth bump over the flash: fast rise, gentle fall (0 -> 1 -> 0).
    float t     = (float)elapsed / (float)LOCK_GLOW_MS;   // 0..1
    float pulse = sinf(3.14159265f * t);                 // 0 -> 1 -> 0
    if (pulse <= 0.02f) return;

    // Gold ramp: deep amber (#B8860B) at trough -> bright gold (#FFD24A) at peak.
    int gr = (int)(184 + (255 - 184) * pulse);
    int gg = (int)(134 + (210 - 134) * pulse);
    int gb = (int)(11  + (74  - 11 ) * pulse);
    COLORREF goldBright = RGB(gr, gg, gb);

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    // SINGLE, UNIFIED edge glow -- it IS the dock's own edge lighting up, not a
    // separate ring. This is painted straight onto the dock engine's own
    // WM_PAINT (this overlay); there is NO separate glow window/layer to manage.
    //
    // The dock's visible edge is the outline the window region clips to
    // (ApplyDockRegion: RoundRect(0,0,w+1,h+1, radius*2, radius*2)). Previously
    // the glow was drawn as TWO concentric strokes inset well inside that edge,
    // so it read as a SECOND edge offset from the real dock edge (the reported
    // "two edges" artifact). We now draw ONE stroke and align its OUTER side to
    // the real dock boundary: a GDI pen is centred on its path, so we inset by
    // half the pen width and shrink the corner radius by the same amount so the
    // stroke stays concentric with -- and sits exactly on -- the dock's edge.
    const int GLOW_PEN = 3;
    const int inset    = 0;              // trace the EXACT region boundary -- the pen's outer half is
                                         // clipped by the window region, leaving the stroke sitting
                                         // precisely ON the dock's real edge (never an inner ring)
    int rEdge = radius;                  // same radius as ApplyDockRegion so the curve matches exactly

    HPEN pen    = CreatePen(PS_SOLID, GLOW_PEN, goldBright);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    // Match ApplyDockRegion's geometry EXACTLY so the glow's corner arc lands on
    // the region's corner: CreateRoundRectRgn(0,0, w+1, h+1, radius*2, radius*2).
    // The +1 matters -- without it the glow's rounded corners sit 1px inside the
    // dock's real corners and look misaligned.
    if (rEdge > 0) RoundRect(hdc, cr.left + inset, cr.top + inset, cr.right + 1 - inset, cr.bottom + 1 - inset, rEdge * 2, rEdge * 2);
    else           Rectangle(hdc, cr.left + inset, cr.top + inset, cr.right - inset, cr.bottom - inset);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SelectObject(hdc, oldBrush);
}

// Begins (or restarts) an interactive lock-glow flash. Called from the blocked
// gesture sites and from the lock toggle. Safe from either thread: it only
// writes one DWORD and posts a cross-thread-safe InvalidateRect.
static void TriggerLockGlow() {
    g_lockGlowStart = GetTickCount();
    if (g_lockGlowStart == 0) g_lockGlowStart = 1;   // 0 is the "idle" sentinel
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);
}

// ============================================================
//  DRAG TETHER ("balloon tug-of-war thread")
//  A single thin thread shown ONLY while a pinned dock icon is being
//  dragged OFF the dock to unpin it. Think of a tug-of-war for a balloon
//  on a string: the DOCK (person A) held the icon with no thread shown;
//  when the USER (person B) snatches it off the dock, a thread connects
//  the icon's ORIGINAL dock-edge slot (fixed end) to the dragged ghost /
//  cursor (moving end). Pull far enough and it snaps; let go early or
//  drag it back and it just disappears and the icon stays. This is PURELY
//  visual -- it never gates or delays the real pin/unpin logic.
//
//  It is drawn on its OWN click-through layered window (like the ghost),
//  because the dock overlay is region-clipped to the dock and physically
//  cannot paint in the empty space outside the dock where the thread
//  lives. The window is created lazily on first drag and torn down in
//  Wh_ModUninit alongside the ghost.
// ============================================================
static HWND    g_tetherWnd  = NULL;
static HBITMAP g_tetherDIB  = NULL;
static BYTE*   g_tetherBits = NULL;
static int     g_tetherW    = 0;
static int     g_tetherH    = 0;
static float   g_tetherBreakFade = 1.f;   // 1 = solid, fades to 0 once taut past break
static float   g_tetherTipX = 0.f, g_tetherTipY = 0.f;  // smoothed moving end (whip lag)
static bool    g_tetherTipInit = false;
static bool    g_tetherWasVisible = false;   // a rope frame was presented this excursion
static bool    g_tetherRetracting = false;   // springing back to the dock anchor (dragged back without breaking)
static const int THREAD_RETRACT_MS = 300;    // TUNED 150->300: the release spring-back read as an instant "neutral" blink at 150ms; a longer window lets the eased spring-home actually be seen as a smooth recoil before it fades.

// Mid-rope snap state. When the pull passes the break distance the rope parts
// in the MIDDLE and the two halves recoil back to their anchored ends. We
// freeze the endpoints at the instant of the snap so the break is a clean,
// self-contained recoil (not a line that keeps sliding with the cursor).
static bool    g_tetherBreaking  = false;
static float   g_tetherBreakSrcX = 0.f, g_tetherBreakSrcY = 0.f;  // frozen dock anchor
static float   g_tetherBreakTipX = 0.f, g_tetherBreakTipY = 0.f;  // frozen cursor grip
// Last anchor (src) actually rendered this frame. Snapshotted every frame in
// UpdateTetherWindow so the drop handler can trigger a mid-rope SNAP from the
// correct frozen anchor AFTER the icon has been unpinned (its dock slot is gone
// by then, so recomputing the anchor would be wrong). This is the key to the
// rope surviving a fast drag+release: on release we freeze these + the last tip
// and let the self-contained snap animation play to completion.
static float   g_tetherLastSrcX  = 0.f, g_tetherLastSrcY  = 0.f;

// ============================================================
//  ICON DISINTEGRATION ("Thanos" vanish)
//  When a pinned icon is LET GO off the dock (an unpin), the icon it was
//  carrying does not just blink out -- it crumbles into drifting dust, like
//  the Avengers: Endgame disintegration. This is a self-contained, purely
//  decorative overlay (its OWN click-through layered window, exactly like the
//  drag ghost/tether). Each visible pixel of the rasterised icon becomes a
//  particle that, after a position-based delay (a sweep across the icon),
//  drifts up + outward, shrinks and fades. It never touches the real
//  pin/unpin data logic; it is triggered once on the off-dock release and
//  pumped to completion by the worker loop.
// ============================================================
static HWND    g_vanishWnd  = NULL;
static HBITMAP g_vanishDIB  = NULL;
static BYTE*   g_vanishBits = NULL;
static int     g_vanishW    = 0;
static int     g_vanishH    = 0;
static bool    g_vanishActive   = false;
static float   g_vanishProgress = 0.f;   // 0..1 over VANISH_MS
static int     g_vanishOriginX  = 0;     // SCREEN top-left of the overlay window (min of all motes)
static int     g_vanishOriginY  = 0;
static int     g_vanishBoxW     = 0;     // overlay window size covering every mote + its drift
static int     g_vanishBoxH     = 0;
static int     g_vanishAccentR  = 220;   // accent tint = the icon's own natural colour (ember flash)
static int     g_vanishAccentG  = 210;
static int     g_vanishAccentB  = 190;
static const int VANISH_MS      = 900;   // total disintegration duration (dramatic, lingering dust)

// Accumulators used while building a vanish (bbox of all motes + accent average).
static float   g_vanBBminX = 0.f, g_vanBBminY = 0.f, g_vanBBmaxX = 0.f, g_vanBBmaxY = 0.f;
static double  g_vanAccSumR = 0, g_vanAccSumG = 0, g_vanAccSumB = 0;
static long    g_vanAccCount = 0;

// Cross-thread vanish request: the double-right-click unpin runs on the MAIN
// (overlay wndproc) thread, but all vanish GDI happens on the worker thread.
// The main thread hands over a CopyIcon + screen rect via these fields (pointer
// published last, with a release/acquire fence) and the worker loop consumes it.
static HICON volatile g_vanishReqIcon = NULL;
static LONG    g_vanishReqX = 0, g_vanishReqY = 0, g_vanishReqS = 0;

// One dust mote per opaque icon pixel. Positions are ABSOLUTE screen px so a
// single vanish can span many icons at once (e.g. unpin-all). Drift velocities
// are the total displacement applied across the mote's dissolve window.
struct VanishParticle {
    float x, y;          // base position (absolute screen px)
    float vx, vy;        // total drift (px) applied as the mote dissolves
    float delay;         // 0..~0.45: when this mote starts dissolving (sweep)
    BYTE  r, g, b, a;    // source colour + straight alpha
};
static std::vector<VanishParticle> g_vanishParticles;

// HSV -> RGB (h in degrees 0..360, s/v in 0..1). Used to paint the drag rope as
// a vivid, flowing multi-hue gradient instead of a single flat tone.
static void HsvToRgb(float h, float s, float v, int* r, int* g, int* b) {
    h = fmodf(h, 360.f); if (h < 0.f) h += 360.f;
    float c = v * s;
    float x = c * (1.f - fabsf(fmodf(h / 60.f, 2.f) - 1.f));
    float m = v - c;
    float rf = 0.f, gf = 0.f, bf = 0.f;
    if      (h <  60.f) { rf = c; gf = x; }
    else if (h < 120.f) { rf = x; gf = c; }
    else if (h < 180.f) { gf = c; bf = x; }
    else if (h < 240.f) { gf = x; bf = c; }
    else if (h < 300.f) { rf = x; bf = c; }
    else                { rf = c; bf = x; }
    *r = std::min(255, std::max(0, (int)((rf + m) * 255.f)));
    *g = std::min(255, std::max(0, (int)((gf + m) * 255.f)));
    *b = std::min(255, std::max(0, (int)((bf + m) * 255.f)));
}

// Immediately hide the drag-tether overlay and reset its per-drag state.
// Called from the drag reset paths (GhostDragReset / GhostCleanup) so a
// finished or cancelled drag can never leave a stale thread frame on screen.
static void HideDragTether() {
    if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE);
    g_tetherTipInit    = false;
    g_tetherBreakFade  = 1.f;
    g_tetherWasVisible = false;
    g_tetherRetracting = false;
    g_tetherBreaking   = false;
}

// (Re)create the tether DIB/window at EXACTLY w x h.
//
// The DIB is sized to exactly the pixels we present -- stride == width ==
// window width -- mirroring the ghost window's proven model. Earlier this used
// a grow-only DIB and presented a w x h sub-region of a larger buffer, relying
// on UpdateLayeredWindow reading a sub-rect via the DIB's own stride. That
// behaviour is only *inferred* from the docs (psize < bitmap size is not a
// documented, guaranteed path) and any skew there paints garbage / a black
// slab. An exact-size DIB removes that inference entirely: what we allocate is
// exactly what we blit. We recreate the DIB whenever the required size changes
// (a drag is a rare, short-lived event, so per-resize reallocation is cheap).
static bool EnsureTetherSurface() {
    // FIXED-SIZE surface + window: sized ONCE to cover the largest possible rope
    // (break length + generous margins for sag), created lazily, and NEVER
    // resized per frame.
    //
    // ROOT-CAUSE FIX (black bg under the rope + dragged icon on real Windows 11,
    // clean in a VM): the rope's content bounding box changes every frame, and
    // the old code recreated the DIB and let UpdateLayeredWindow RESIZE the
    // layered window to match. Resizing a layered (ULW) window exposes a fresh,
    // still-uninitialised backing surface that a real hardware DWM compositor
    // presents as an OPAQUE BLACK slab for one frame before the content lands
    // (a VM's basic software compositor coalesces it, so it looks clean there).
    // The dragged ghost icon never showed this because it is a FIXED-size window
    // that only ever MOVES -- moving a layered window is flicker-free. We now
    // mirror that: the tether window is a fixed square that we MOVE each frame
    // (via UpdateLayeredWindow's pptDst) but never resize, drawing the rope into
    // the top-left region and leaving the surplus fully transparent.
    int dim = (int)ceilf(THREAD_MAX_STRETCH_PX) + 160;   // max rope span + margins/sag headroom
    if (dim < 320)  dim = 320;
    if (dim > 2048) dim = 2048;

    // Window class + window created once, lazily.
    if (!g_tetherWnd) {
        static bool s_cls = false;
        if (!s_cls) {
            WNDCLASSEXW wc = { sizeof(wc) };
            wc.lpfnWndProc   = DefWindowProcW;
            wc.hInstance     = GetModuleHandleW(NULL);
            wc.lpszClassName = L"QPDockTether";
            RegisterClassExW(&wc);
            s_cls = true;
        }
        g_tetherWnd = CreateWindowExW(
            // WS_EX_TOPMOST: keep the rope in the top-most Z band from creation
            // (the ghost icon already has this). Without it the rope could slip
            // UNDER other top-most windows before the first SetWindowPos.
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
            L"QPDockTether", L"", WS_POPUP,
            0, 0, dim, dim, NULL, NULL, GetModuleHandleW(NULL), NULL);
        if (!g_tetherWnd) return false;
    }

    // Reuse the DIB unless the fixed dimension changed (e.g. break-length setting).
    if (g_tetherDIB && g_tetherBits && g_tetherW == dim && g_tetherH == dim)
        return true;

    if (g_tetherDIB) {
        DeleteObject(g_tetherDIB);
        g_tetherDIB = NULL; g_tetherBits = NULL;
    }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = dim;
    bi.bmiHeader.biHeight      = -dim;   // top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC sdc = GetDC(NULL);
    if (sdc) {
        g_tetherDIB = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS,
                                       (void**)&g_tetherBits, NULL, 0);
        ReleaseDC(NULL, sdc);
    }
    if (!g_tetherDIB) { g_tetherBits = NULL; g_tetherW = g_tetherH = 0; return false; }
    g_tetherW = dim; g_tetherH = dim;
    return true;
}

// Plot a soft round dot into the top-down 32-bit ARGB tether buffer.
// (Manual AA: PolyBezier into a raw DIB would be aliased and would not carry
// an alpha channel for UpdateLayeredWindow, so we stamp the curve ourselves.)
static inline void TetherPlot(int bufW, int bufH, float fx, float fy,
                              int r8, int g8, int b8, float coverage, float radius) {
    if (coverage <= 0.f || radius <= 0.f) return;
    // SUB-PIXEL, DISTANCE-FIELD stamp. The old version snapped the dot centre to
    // an integer pixel (lroundf) and measured distance from that snapped centre
    // -- which quantises the curve to whole-pixel jumps and is the root cause of
    // the "pixelated / stair-stepped" diagonal. We now keep the TRUE fractional
    // centre and measure distance from each pixel's CENTRE (x+0.5, y+0.5), then
    // convert distance -> coverage with a smoothstep, exactly like a signed
    // distance-field capsule stroke. Dense sub-pixel stamps composited with MAX
    // coverage (below) approximate a perfectly smooth capsule at any angle.
    int x0 = (int)floorf(fx - radius - 1.5f), x1 = (int)ceilf(fx + radius + 1.5f);
    int y0 = (int)floorf(fy - radius - 1.5f), y1 = (int)ceilf(fy + radius + 1.5f);
    for (int y = y0; y <= y1; ++y) {
        if (y < 0 || y >= bufH) continue;
        float ddy = (float)y + 0.5f - fy;
        for (int x = x0; x <= x1; ++x) {
            if (x < 0 || x >= bufW) continue;
            float ddx = (float)x + 0.5f - fx;
            float d = sqrtf(ddx * ddx + ddy * ddy);
            // 1 - smoothstep over a ~1.2px edge centred on `radius`.
            float e0 = radius - 0.6f, e1 = radius + 0.6f;
            float t = (e1 > e0) ? (d - e0) / (e1 - e0) : (d >= e1 ? 1.f : 0.f);
            if (t < 0.f) t = 0.f; else if (t > 1.f) t = 1.f;
            float cov = coverage * (1.f - t * t * (3.f - 2.f * t));
            if (cov <= 0.003f) continue;
            BYTE* px = g_tetherBits + ((size_t)y * bufW + x) * 4;   // straight BGRA
            // MAX-coverage compositing: the strand is one solid colour, so keep
            // the strongest coverage seen at each pixel. This gives a clean AA
            // union with NO edge over-accumulation from the dense overlapping
            // stamps (additive alpha used to thicken/alias the edge). Colour is
            // stored straight; premultiplied once per frame before present.
            float nv = cov * 255.f;
            if (nv > (float)px[3]) {
                px[0] = (BYTE)b8; px[1] = (BYTE)g8; px[2] = (BYTE)r8;
                px[3] = (BYTE)std::min(255.f, nv);
            }
        }
    }
}

// Plot an anti-aliased CAPSULE (round-capped line segment) into the top-down
// 32-bit ARGB tether buffer using a per-pixel DISTANCE FIELD. This is the
// smooth-rope primitive: coverage comes from the TRUE distance of each pixel
// centre to the segment A-B (clamped projection), not from stamping a chain of
// discrete round dots -- so a diagonal strand has no stair-step at ANY angle.
// Composited with MAX coverage so a run of overlapping capsules unions into one
// clean tube with no edge over-accumulation. (Recommended CPU technique for a
// smooth vector stroke: flatten the curve to short segments + capsule SDF.)
static inline void TetherCapsule(int bufW, int bufH,
                                 float ax, float ay, float bx, float by,
                                 int r8, int g8, int b8, float coverage, float radius) {
    if (coverage <= 0.f || radius <= 0.f) return;
    float ex = bx - ax, ey = by - ay;
    float elen2 = ex * ex + ey * ey;
    int x0 = (int)floorf(std::min(ax, bx) - radius - 1.5f);
    int x1 = (int)ceilf (std::max(ax, bx) + radius + 1.5f);
    int y0 = (int)floorf(std::min(ay, by) - radius - 1.5f);
    int y1 = (int)ceilf (std::max(ay, by) + radius + 1.5f);
    for (int y = y0; y <= y1; ++y) {
        if (y < 0 || y >= bufH) continue;
        float py = (float)y + 0.5f;
        for (int x = x0; x <= x1; ++x) {
            if (x < 0 || x >= bufW) continue;
            float px = (float)x + 0.5f;
            // Nearest point on the segment (clamped projection), then distance.
            float t = (elen2 > 1e-6f) ? ((px - ax) * ex + (py - ay) * ey) / elen2 : 0.f;
            if (t < 0.f) t = 0.f; else if (t > 1.f) t = 1.f;
            float dx = px - (ax + ex * t), dy = py - (ay + ey * t);
            float d  = sqrtf(dx * dx + dy * dy);
            // 1 - smoothstep over a ~1.4px edge centred on `radius`.
            float e0 = radius - 0.7f, e1 = radius + 0.7f;
            float tt = (e1 > e0) ? (d - e0) / (e1 - e0) : (d >= e1 ? 1.f : 0.f);
            if (tt < 0.f) tt = 0.f; else if (tt > 1.f) tt = 1.f;
            float cov = coverage * (1.f - tt * tt * (3.f - 2.f * tt));
            if (cov <= 0.003f) continue;
            BYTE* pp = g_tetherBits + ((size_t)y * bufW + x) * 4;   // straight BGRA
            float nv = cov * 255.f;
            if (nv > (float)pp[3]) {
                pp[0] = (BYTE)b8; pp[1] = (BYTE)g8; pp[2] = (BYTE)r8;
                pp[3] = (BYTE)std::min(255.f, nv);
            }
        }
    }
}

// ============================================================
//  VERLET ROPE  --  realistic interactive thin-thread simulation
// ============================================================
// The intact drag thread is no longer a hand-drawn quadratic bow. It is a chain
// of point masses advanced with VERLET INTEGRATION and satisfied by iterative
// distance (spring) constraints -- the standard, stable rope/cloth technique.
// One end is pinned to the dock anchor, the other to the cursor grip. The final
// shape EMERGES from the sim: it bends and forms organic arcs when the grip
// moves fast (motion lag), stretches taut when pulled, sags under a light
// gravity when slack, and settles smoothly via velocity damping when motion
// stops (no endless oscillation). It is rendered as the SAME thin distance-field
// capsule strand used before (rounded caps, no debug elements) so it still reads
// as a fine thread, not a heavy rope.
static const int   ROPE_MAX_PTS = 26;      // chain resolution (points); segments = PTS-1. Smooth curve, cheap enough for 120fps.
static int         g_ropeN      = 0;       // active point count (0 => uninitialised)
static float       g_ropeX[ROPE_MAX_PTS],  g_ropeY[ROPE_MAX_PTS];    // current positions (screen space)
static float       g_ropePX[ROPE_MAX_PTS], g_ropePY[ROPE_MAX_PTS];   // previous positions (Verlet velocity = cur - prev)
static const float ROPE_DAMPING = 0.90f;   // velocity retention per step (<1 => damping; settles instead of oscillating forever)
static const float ROPE_GRAVITY = 0.75f;   // downward accel (px/frame^2 at ~60fps); small => a LIGHT thread's gentle hang, not a heavy rope
static const int   ROPE_ITERS   = 16;      // constraint relaxation passes/frame; higher => stiffer, less rubbery stretch

// Reset the chain to a straight line anchor(ax,ay) -> tip(bx,by) with zero velocity.
static void RopeReset(float ax, float ay, float bx, float by) {
    g_ropeN = ROPE_MAX_PTS;
    for (int i = 0; i < g_ropeN; ++i) {
        float t = (float)i / (float)(g_ropeN - 1);
        float x = ax + (bx - ax) * t, y = ay + (by - ay) * t;
        g_ropeX[i] = g_ropePX[i] = x;
        g_ropeY[i] = g_ropePY[i] = y;
    }
}

// Advance the chain one frame. Endpoints are pinned (p0 = anchor, pN-1 = grip).
// restTotal = the rope's rest length: ~= straight distance when taut, a little
// longer when slack so the surplus sags into an organic curve under gravity.
static void RopeStep(float ax, float ay, float bx, float by, float restTotal, float dtMs) {
    if (g_ropeN < 2) { RopeReset(ax, ay, bx, by); return; }
    // Frame-rate-normalised step, clamped so a slow frame can't explode the sim.
    float f = dtMs / 16.6667f;
    if (f > 2.5f) f = 2.5f; else if (f < 0.20f) f = 0.20f;
    // 1) Integrate interior points (Verlet: x += (x - xprev)*damping + accel).
    float grav = ROPE_GRAVITY * f * f;
    for (int i = 1; i < g_ropeN - 1; ++i) {
        float vx = (g_ropeX[i] - g_ropePX[i]) * ROPE_DAMPING;
        float vy = (g_ropeY[i] - g_ropePY[i]) * ROPE_DAMPING;
        g_ropePX[i] = g_ropeX[i];
        g_ropePY[i] = g_ropeY[i];
        g_ropeX[i] += vx;
        g_ropeY[i] += vy + grav;
    }
    // 2) Pin endpoints to the dock anchor + the cursor grip.
    g_ropeX[0] = ax; g_ropeY[0] = ay;
    g_ropeX[g_ropeN - 1] = bx; g_ropeY[g_ropeN - 1] = by;
    // 3) Satisfy segment-length constraints (Jakobsen relaxation): pull the two
    //    ends of each over/under-stretched segment half-way to the rest length,
    //    then re-pin the anchored ends. Several passes => a stiff-but-supple rope.
    float rest = restTotal / (float)(g_ropeN - 1);
    for (int it = 0; it < ROPE_ITERS; ++it) {
        for (int i = 0; i < g_ropeN - 1; ++i) {
            float dx = g_ropeX[i + 1] - g_ropeX[i];
            float dy = g_ropeY[i + 1] - g_ropeY[i];
            float d  = sqrtf(dx * dx + dy * dy);
            if (d < 1e-4f) d = 1e-4f;
            float corr = (d - rest) / d * 0.5f;
            float ox = dx * corr, oy = dy * corr;
            g_ropeX[i]     += ox; g_ropeY[i]     += oy;
            g_ropeX[i + 1] -= ox; g_ropeY[i + 1] -= oy;
        }
        g_ropeX[0] = ax; g_ropeY[0] = ay;
        g_ropeX[g_ropeN - 1] = bx; g_ropeY[g_ropeN - 1] = by;
    }
}

// Render + present the CURRENT Verlet chain as one thin thread. `alpha` is the
// global fade (g_tetherBreakFade drives the retract dissolve). Bounds come from
// the ACTUAL point positions so a deep sag is never clipped. Uses the same
// exact-size DIB + premultiplied ULW present as DrawTetherRope (incl. the
// present-then-assert-Z-order ordering that fixes the black-slab artifact).
static void PresentPhysicsRope(float alpha, float stretch01 = 0.f) {
    if (g_ropeN < 2) { if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE); return; }
    const int MARGIN = 24;
    float minX = g_ropeX[0], maxX = g_ropeX[0], minY = g_ropeY[0], maxY = g_ropeY[0];
    for (int i = 1; i < g_ropeN; ++i) {
        if (g_ropeX[i] < minX) minX = g_ropeX[i];
        if (g_ropeX[i] > maxX) maxX = g_ropeX[i];
        if (g_ropeY[i] < minY) minY = g_ropeY[i];
        if (g_ropeY[i] > maxY) maxY = g_ropeY[i];
    }
    int left = (int)floorf(minX) - MARGIN, top = (int)floorf(minY) - MARGIN;
    int cw = (int)ceilf(maxX) + MARGIN - left, ch = (int)ceilf(maxY) + MARGIN - top;   // content extent (tiny-guard only)
    if (cw < 2 || ch < 2) { if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE); return; }
    if (!EnsureTetherSurface()) return;
    // FIXED-size buffer/window: draw the rope into the top-left region (offset by
    // left/top) and MOVE the window there via ULW's pptDst; the window is NEVER
    // resized (that is what caused the black-slab flash on real GPUs). The
    // surplus buffer area beyond the content stays fully transparent.
    int w = g_tetherW, h = g_tetherH;
    memset(g_tetherBits, 0, (size_t)w * h * 4);
    g_tetherWasVisible = true;

    // Earthy single-colour thin thread (same palette + thickness as DrawTetherRope).
    int r8, g8, b8; HsvToRgb((float)THREAD_HUE, 0.20f, 0.74f, &r8, &g8, &b8);
    // Neck the strand thinner as it nears the break (stretch01: 0 = relaxed,
    // 1 = at break length) so a taut thread visibly narrows before it snaps.
    if (stretch01 < 0.f) stretch01 = 0.f; else if (stretch01 > 1.f) stretch01 = 1.f;
    float thin  = 1.f - 0.45f * stretch01;
    float coreR = (0.15f + (float)THREAD_THICKNESS * 0.40f) * thin;   // ~0.55 .. 4.15 px, necked near break
    float haloR = coreR + 0.6f;

    // Two passes: a soft dim halo, then the solid core, both as MAX-coverage
    // distance-field capsules between consecutive chain points => one smooth
    // tube at any angle with rounded joints (no stair-step, no dotting).
    for (int pass = 0; pass < 2; ++pass) {
        float pr = (pass == 0) ? haloR : std::max(coreR, 0.9f);
        float pa = (pass == 0) ? alpha * 0.12f : alpha;
        for (int i = 0; i < g_ropeN - 1; ++i) {
            TetherCapsule(w, h,
                          g_ropeX[i]     - left, g_ropeY[i]     - top,
                          g_ropeX[i + 1] - left, g_ropeY[i + 1] - top,
                          r8, g8, b8, pa, pr);
        }
    }
    // Rounded endpoints (anchor + grip).
    TetherPlot(w, h, g_ropeX[0]           - left, g_ropeY[0]           - top, r8, g8, b8, alpha, coreR + 0.4f);
    TetherPlot(w, h, g_ropeX[g_ropeN - 1] - left, g_ropeY[g_ropeN - 1] - top, r8, g8, b8, alpha, coreR + 0.4f);

    PremultiplyAlpha(g_tetherBits, w * h);

    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    HDC mdc = CreateCompatibleDC(sdc);
    if (!mdc) { ReleaseDC(NULL, sdc); return; }
    HBITMAP oldb = (HBITMAP)SelectObject(mdc, g_tetherDIB);
    POINT dest  = { left, top };
    SIZE  sz    = { w, h };
    POINT srcpt = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    // Present with the SAME proven recipe as the drag ghost (which never shows
    // the black slab or flash on real hardware): MOVE the fixed-size window to
    // dest and SHOW it via SetWindowPos FIRST, THEN hand the pixels to ULW.
    // Because w/h are the FIXED surface dims, SetWindowPos only ever MOVES the
    // window (never resizes) -- exactly like the ghost. Letting ULW move a large
    // layered window via its own pptDst was the real-GPU black + flash trigger.
    SetWindowPos(g_tetherWnd, HWND_TOPMOST, dest.x, dest.y, w, h,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateLayeredWindow(g_tetherWnd, sdc, &dest, &sz, mdc, &srcpt, 0, &bf, ULW_ALPHA);
    SelectObject(mdc, oldb);
    DeleteDC(mdc);
    ReleaseDC(NULL, sdc);
}

// Render + present ONE rope frame between two SCREEN-space endpoints.
//   taut     : 0 (slack, bowed) .. 1 (straight) -- only used when !breaking.
//   breaking : draw as two halves parting in the MIDDLE, recoiling to their
//              anchored ends (breakP 0 -> 1) with a light gravity droop.
// The thread is a thin, crisp, animated multi-hue gradient. Global alpha comes
// from g_tetherBreakFade so the caller drives every fade.
static void DrawTetherRope(float srcX, float srcY, float tipX, float tipY,
                           float taut, bool breaking, float breakP) {
    // Bounding box around both endpoints (+ margin for bow/droop + stroke), so
    // the soft edges are never clipped at the layered-window border.
    const int MARGIN = 24;
    int left   = (int)floorf(std::min(srcX, tipX)) - MARGIN;
    int top    = (int)floorf(std::min(srcY, tipY)) - MARGIN;
    int right  = (int)ceilf (std::max(srcX, tipX)) + MARGIN;
    int bottom = (int)ceilf (std::max(srcY, tipY)) + MARGIN;
    int cw = right - left, ch = bottom - top;   // content extent (tiny-guard only)
    if (cw < 2 || ch < 2) { if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE); return; }
    if (!EnsureTetherSurface()) {
        LOG_IMPORTANT(L"TETHER-DBG EnsureTetherSurface FAILED cw=%d ch=%d wnd=%p", cw, ch, (void*)g_tetherWnd);
        return;
    }

    // FIXED-size buffer/window (never resized -> no black-slab flash on real
    // GPUs; the window only MOVES, like the ghost). Draw the tear into the
    // top-left region (offset by left/top); the surplus stays transparent.
    int w = g_tetherW, h = g_tetherH;
    // One contiguous clear to fully-transparent, then rebuild straight-alpha
    // from scratch this frame (no persistent state to corrupt).
    memset(g_tetherBits, 0, (size_t)w * h * 4);
    g_tetherWasVisible = true;

    float ax = srcX - left, ay = srcY - top;   // dock anchor (buffer space)
    float bx = tipX - left, by = tipY - top;   // cursor grip (buffer space)
    float alpha = g_tetherBreakFade;

    // Colour (user setting). Rainbow mode: hue sweeps over time AND along the
    // length -> lively, multi-colour. Fixed mode: a chosen hue with a small
    // along-length shimmer so it still reads as a gradient, not a flat line.
    // SINGLE COLOUR ONLY. The dynamic multi-hue "rainbow" mode is intentionally
    // disabled -- the thread must always be one calm, earthy strand (never RGB).
    // THREAD_COLOR_MODE is ignored for rendering; kept only for settings compat.
    bool  rainbow = false;
    (void)THREAD_COLOR_MODE;
    float baseHue = rainbow ? ((float)(GetTickCount() % 4000) / 4000.f * 360.f)
                            : (float)THREAD_HUE;
    // Along-length colour variation: wide sweep for rainbow. The fixed thread
    // is now a TRUE single colour (no along-length hue shift) per the request
    // for "only single color -- no dynamic color".
    float hueSpan = rainbow ? 160.f : 0.f;

    // EARTHY THREAD look (fixed mode = default): further muted so it stops
    // reading as a neon/glowing wire and instead looks like a real cotton/jute
    // strand -- a soft warm tan/greige (~RGB 189,170,151). Saturation and
    // brightness are both pulled down: low sat kills the neon cast, a mid
    // value keeps ONE flat tone that stays legible on BOTH light and dark
    // taskbars (neither pure white nor pure dark). Rainbow keeps its vivid mix.
    float sat = rainbow ? 0.85f : 0.20f;   // lower sat -> earthy, no neon cast
    float val = rainbow ? 1.00f : 0.74f;   // mid value -> calm thread, not glow

    // Thickness (user setting 1..6): crisp solid core + a hair of halo.
    // REFINED to read as a fine THREAD rather than a heavy rope -- the core is
    // thinner and the halo tighter, so even the bold end of the range stays
    // delicate. Range now ~0.80 .. 2.30 px across the 1..6 setting.
    // Thickness setting 1..10 maps ultra-thin -> bold. At 1 the core is a
    // hair-fine ~0.55px strand; at 10 it is a bold ~4.15px cord. Linear so each
    // step is an even, predictable increase.
    float coreR = 0.15f + (float)THREAD_THICKNESS * 0.40f;   // ~0.55 .. 4.15 px across 1..10
    float haloR = coreR + 0.6f;

    // Stable per-break pseudo-random (seeded from the frozen endpoints) so the
    // torn look is uneven but does NOT flicker frame-to-frame.
    unsigned seed = (unsigned)(srcX * 7.13f + srcY * 3.71f + tipX * 5.17f + tipY * 9.29f);
    auto rnd = [&](int k) {
        unsigned x = (seed + (unsigned)k * 2654435761u) * 1103515245u + 12345u;
        return (float)((x >> 8) & 0xFFFF) / 65535.f;   // 0..1
    };

    // Stroke a quadratic A->ctrl->B as a CONTINUOUS anti-aliased CAPSULE CHAIN.
    // The old path stamped discrete round dots along the curve (~3/px) and, at
    // sub-pixel core widths, that read as a stair-stepped / pixelated diagonal.
    // Now we FLATTEN the quadratic into short line segments and rasterise each
    // as a distance-field capsule; overlapping capsules composited by MAX
    // coverage form one perfectly smooth tube at ANY angle -- a real rope, not a
    // dotted rod. `gt0/gt1` map the segment onto the rope's 0..1 hue parameter so
    // the gradient stays continuous across segments.
    auto strokeSeg = [&](float sax, float say, float scx, float scy,
                         float sbx, float sby, float gt0, float gt1, float aMul) {
        float l1 = sqrtf((scx - sax) * (scx - sax) + (scy - say) * (scy - say));
        float l2 = sqrtf((sbx - scx) * (sbx - scx) + (sby - scy) * (sby - scy));
        int segN = (int)((l1 + l2) * 0.5f);   // ~one flat segment per 2px of arc -> smooth & cheap
        if (segN < 8)   segN = 8;
        if (segN > 512) segN = 512;
        for (int pass = 0; pass < 2; ++pass) {
            // Keep the visible core at least ~0.9px so even the thinnest thread
            // renders as a smooth, gap-free strand rather than a dotted hairline.
            float pr = (pass == 0) ? haloR : std::max(coreR, 0.9f);
            float pa = ((pass == 0) ? alpha * (rainbow ? 0.22f : 0.12f) : alpha) * aMul;   // dimmer halo for the earthy thread (less glow); subtle halo for rainbow, solid core
            float px0 = sax, py0 = say;
            for (int s = 1; s <= segN; ++s) {
                float t = (float)s / segN, omt = 1.f - t;
                float px1 = omt * omt * sax + 2 * omt * t * scx + t * t * sbx;
                float py1 = omt * omt * say + 2 * omt * t * scy + t * t * sby;
                int r8, g8, b8;
                float gt = gt0 + (gt1 - gt0) * (t - 0.5f / segN);   // segment-midpoint hue
                HsvToRgb(baseHue + gt * hueSpan, sat, val, &r8, &g8, &b8);
                TetherCapsule(w, h, px0, py0, px1, py1, r8, g8, b8, pa, pr);
                px0 = px1; py0 = py1;
            }
        }
    };

    // Draw a few short, uneven FRIZZ strands off a broken end so it reads as
    // TORN (frayed) rather than cleanly cut. (dirX,dirY) points back along the
    // rope -- the direction the loose fibres trail.
    auto frayEnd = [&](float ex, float ey, float dirX, float dirY, int base, float gt) {
        float dl = sqrtf(dirX * dirX + dirY * dirY); if (dl < 0.001f) return;
        dirX /= dl; dirY /= dl;
        float nx = -dirY, ny = dirX;   // perpendicular
        int r8, g8, b8; HsvToRgb(baseHue + gt * hueSpan, sat, val, &r8, &g8, &b8);
        for (int i = 0; i < 5; ++i) {   // 5 loose fibres per torn end (was 3) -> more clearly frayed
            float len  = 3.f + rnd(base + i) * 6.f;             // 3..9 px, uneven
            float off  = (rnd(base + 10 + i) - 0.5f) * 5.f;     // lateral splay
            float curl = (rnd(base + 20 + i) - 0.5f) * 4.f;     // slight curl
            float fx = ex + dirX * len + nx * off;
            float fy = ey + dirY * len + ny * off;
            float mxs = (ex + fx) * 0.5f + nx * curl;
            float mys = (ey + fy) * 0.5f + ny * curl;
            for (int s = 0; s <= 8; ++s) {
                float t = (float)s / 8.f, omt = 1.f - t;
                float qx = omt * omt * ex + 2 * omt * t * mxs + t * t * fx;
                float qy = omt * omt * ey + 2 * omt * t * mys + t * t * fy;
                TetherPlot(w, h, qx, qy, r8, g8, b8, alpha * (1.f - t) * 0.9f, coreR * 0.8f);
            }
        }
    };

    int cra, cga, cba, crb, cgb, cbb;
    HsvToRgb(baseHue,           sat, val, &cra, &cga, &cba);   // colour at the dock end
    HsvToRgb(baseHue + hueSpan, sat, val, &crb, &cgb, &cbb);   // colour at the cursor end

    if (!breaking) {
        // Intact rope: one gently-bowed thread; bow sags when slack, straightens taut.
        // A light thread never hangs perfectly straight: keep a small constant
        // sag even when taut, growing as it goes slack, so it reads as a supple
        // strand with real hang rather than a stiff stick.
        float bow = (1.f - taut) * 14.f + 3.f;
        strokeSeg(ax, ay, (ax + bx) * 0.5f, (ay + by) * 0.5f + bow, bx, by, 0.f, 1.f, 1.f);
        TetherPlot(w, h, ax, ay, cra, cga, cba, alpha, coreR + 0.4f);
        TetherPlot(w, h, bx, by, crb, cgb, cbb, alpha, coreR + 0.4f);
    } else {
        // Snapped: the rope TEARS. The break point is deliberately OFF exact
        // centre (uneven halves) so it reads as "broken", not "cut". Each half's
        // broken inner end recoils toward its anchored end (ease-out) with a
        // gravity droop and a slight jagged kink, and its tip frays into strands.
        float splitT = 0.40f + rnd(1) * 0.20f;                       // 0.40..0.60 (uneven)
        float mpx = ax + (bx - ax) * splitT, mpy = ay + (by - ay) * splitT;
        // Stronger recoil (0.62 -> 0.94): the two torn halves snap almost all
        // the way back to their anchors, opening a clear GAP in the middle so it
        // unmistakably reads as SNAPPED-APART rather than a clean cut in place.
        // Smoothstep recoil (S-curve): the halves pull back with a gentle
        // start, a quick middle and a soft settle -- reads as an elastic snap
        // that relaxes naturally rather than a hard jerk. Still travels 0.94 of
        // the way to the anchors so the middle gap is clearly open.
        float sp = breakP * breakP * (3.f - 2.f * breakP);
        float recoil = sp * 0.94f;
        float ia_x = mpx + (ax - mpx) * recoil, ia_y = mpy + (ay - mpy) * recoil;
        float ib_x = mpx + (bx - mpx) * recoil, ib_y = mpy + (by - mpy) * recoil;
        float droop = breakP * breakP * 22.f;  // quadratic gravity: the free ends accelerate downward like a real cut cord
        // Perpendicular unit vector for the jagged kink near each broken end.
        float pnx = -(by - ay), pny = (bx - ax);
        float pl = sqrtf(pnx * pnx + pny * pny); if (pl > 0.001f) { pnx /= pl; pny /= pl; }
        float ja = (rnd(2) - 0.5f) * 6.f, jb = (rnd(3) - 0.5f) * 6.f;
        strokeSeg(ax, ay, (ax + ia_x) * 0.5f + pnx * ja, (ay + ia_y) * 0.5f + pny * ja + droop, ia_x, ia_y, 0.f,     splitT, 1.f);
        strokeSeg(bx, by, (bx + ib_x) * 0.5f + pnx * jb, (by + ib_y) * 0.5f + pny * jb + droop, ib_x, ib_y, 1.f,     splitT, 1.f);
        // Frayed broken tips (fibres trail back toward the middle they tore from).
        frayEnd(ia_x, ia_y, mpx - ia_x, mpy - ia_y, 100, splitT);
        frayEnd(ib_x, ib_y, mpx - ib_x, mpy - ib_y, 200, splitT);
        // SNAP FLASH: a bright burst at the tear point, strongest at the instant
        // of the break and gone within the first third of the recoil.
        float flash = std::max(0.f, 1.f - breakP * 3.f);
        if (flash > 0.f) {
            int fr, fg, fb; HsvToRgb(baseHue + splitT * hueSpan, 0.55f, 1.f, &fr, &fg, &fb);
            TetherPlot(w, h, mpx, mpy, 255, 255, 255, flash * alpha,        coreR + 2.0f);
            TetherPlot(w, h, mpx, mpy, fr, fg, fb,    flash * alpha * 0.8f, coreR + 4.5f * flash + 2.0f);
            for (int i = 0; i < 6; ++i) {   // radial spark spokes
                float aang = (float)i / 6.f * 6.2832f + rnd(30 + i);
                float sl = (4.f + rnd(40 + i) * 6.f) * flash;
                TetherPlot(w, h, mpx + cosf(aang) * sl, mpy + sinf(aang) * sl,
                           fr, fg, fb, flash * alpha * 0.7f, coreR * 0.9f);
            }
        }
        TetherPlot(w, h, ax, ay, cra, cga, cba, alpha, coreR + 0.4f);
        TetherPlot(w, h, bx, by, crb, cgb, cbb, alpha, coreR + 0.4f);
    }

    // ULW_ALPHA needs premultiplied RGB; runs exactly once per frame on the
    // freshly-rebuilt straight-alpha buffer.
    PremultiplyAlpha(g_tetherBits, w * h);

    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    HDC mdc = CreateCompatibleDC(sdc);
    if (!mdc) { ReleaseDC(NULL, sdc); return; }
    HBITMAP oldb = (HBITMAP)SelectObject(mdc, g_tetherDIB);
    POINT dest  = { left, top };
    SIZE  sz    = { w, h };
    POINT srcpt = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    // FIX (BLACK SLAB trailing the rope on real GPUs -- clean in a VM):
    // this used to SetWindowPos(..., w, h) to move+RESIZE the layered window
    // every frame and THEN call UpdateLayeredWindow. The tether's bounding box
    // changes on every frame as the cursor moves, so that pre-resize grew the
    // window onto a brand-new, still-uninitialised backing surface which a
    // hardware DWM compositor presented as OPAQUE BLACK for one frame before
    // ULW repainted it -- exactly the "black background under the rope" the
    // user sees on their main PC but not in the VM (whose software compositor
    // coalesces the resize+update, so the black frame never shows).
    // UpdateLayeredWindow ALREADY moves (pptDst = &dest) AND resizes
    // (psize = &sz) the window atomically, so the separate pre-resize is both
    // redundant and the sole source of the artifact. Let ULW own all geometry,
    // then re-assert Z-order WITHOUT touching position/size (SWP_NOMOVE |
    // SWP_NOSIZE) so nothing is ever exposed black.
    // Present with the SAME proven recipe as the drag ghost (fixed-size window,
    // SetWindowPos move+show FIRST, then ULW). See PresentPhysicsRope for the
    // full rationale -- this kills the black slab + corner flash on real GPUs.
    SetWindowPos(g_tetherWnd, HWND_TOPMOST, dest.x, dest.y, w, h,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateLayeredWindow(g_tetherWnd, sdc, &dest, &sz, mdc, &srcpt, 0, &bf, ULW_ALPHA);
    SelectObject(mdc, oldb);
    DeleteDC(mdc);
    ReleaseDC(NULL, sdc);
}

// Update / show / hide the drag tether. Called every worker frame beside
// UpdateGhostWindow. cursorPt is screen-space (the moving end).
static void UpdateTetherWindow(POINT cursorPt) {
    // Tug-of-war rope: shown ONLY once a DOCK icon has actually been pulled
    // OUTSIDE the dock zone (unpin drag). While the icon is still over/near the
    // dock the dock is "holding" it (g_dropZoneActive) -- nothing to fight over,
    // so NO thread is drawn. The thread appears the moment the icon leaves the
    // zone and hides again if it is dragged back in (thread relaxes, no break),
    // matching the spec: "while the dock holds them -> no thread is shown."
    // This also guarantees a single, unambiguous thread (never one trailing an
    // in-dock icon plus one for the off-dock ghost).
    // A dock icon is mid-drag (regardless of whether it is currently over the dock).
    bool inDrag = ENABLE_ICON_THREADS && g_dragFromDock &&
                  g_dragState == DRAG_DRAGGING && g_dragFromDockIdx >= 0;

    // TEMP DIAGNOSTIC: edge-triggered log each time the tether "active" gate
    // flips. Shows WHY the rope may not run (threads off? never DRAGGING?).
    // Remove once the rope is confirmed working.
    static bool s_prevInDrag = false;
    if (inDrag != s_prevInDrag) {
        LOG_IMPORTANT(L"TETHER-DBG inDrag=%d state=%d fromDock=%d idx=%d threadsOn=%d dropZone=%d",
                      (int)inDrag, (int)g_dragState, (int)g_dragFromDock,
                      g_dragFromDockIdx, (int)ENABLE_ICON_THREADS, (int)g_dropZoneActive);
        s_prevInDrag = inDrag;
    }

    // A mid-rope SNAP, once begun, plays to completion on its own -- even after
    // the drag/drop has ended and the icon is already unpinned. This is what
    // makes the break in the MIDDLE reliably visible: previously the drop reset
    // the overlay before the ~160ms snap could play, so it looked like it never
    // broke. While snapping we ignore the drag entirely and render the two
    // recoiling halves from the FROZEN endpoints captured at the instant of the
    // snap, then hard-reset once fully faded.
    if (g_tetherBreaking) {
        g_tetherBreakFade = std::max(0.f, g_tetherBreakFade - (g_frameDeltaMs / (float)THREAD_BREAK_MS));
        if (g_tetherBreakFade <= 0.02f) {
            if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE);
            g_tetherTipInit    = false;
            g_tetherBreakFade  = 1.f;
            g_tetherWasVisible = false;
            g_tetherRetracting = false;
            g_tetherBreaking   = false;
            return;
        }
        float breakP = std::min(1.f, 1.f - g_tetherBreakFade);
        DrawTetherRope(g_tetherBreakSrcX, g_tetherBreakSrcY,
                       g_tetherBreakTipX, g_tetherBreakTipY, 1.f, true, breakP);
        return;
    }

    // A RETRACT (spring-home + fade) that was begun at release must also play
    // to completion even though the drag has ended (inDrag == false). Without
    // this, the `if (!inDrag)` hard-reset below would swallow the retract and
    // the rope would blink out on an on-dock release. Uses the frozen last
    // anchor and eases the smoothed tip home, then cleans up exactly once.
    if (g_tetherRetracting && !inDrag) {
        g_tetherBreakFade = std::max(0.f, g_tetherBreakFade - (g_frameDeltaMs / (float)THREAD_RETRACT_MS));
        // TUNED 2.4->1.35: at *2.4 the tip snapped home within ~2 frames and the
        // rest of the window was just an empty fade -- reading as a flat, "neutral"
        // blink. A gentler factor spreads the spring-home across the full
        // THREAD_RETRACT_MS so the recoil is actually seen as a smooth ease-out
        // glide back to the dock before it dissolves.
        float tf = std::min(1.f, (g_frameDeltaMs / (float)THREAD_RETRACT_MS) * 1.35f);
        g_tetherTipX += (g_tetherLastSrcX - g_tetherTipX) * tf;
        g_tetherTipY += (g_tetherLastSrcY - g_tetherTipY) * tf;
        float rdx = g_tetherTipX - g_tetherLastSrcX, rdy = g_tetherTipY - g_tetherLastSrcY;
        if (g_tetherBreakFade <= 0.02f || (rdx * rdx + rdy * rdy) < 9.f) {
            if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE);
            g_tetherTipInit    = false;
            g_tetherBreakFade  = 1.f;
            g_tetherWasVisible = false;
            g_tetherRetracting = false;
            return;
        }
        float rdist = sqrtf(rdx * rdx + rdy * rdy);
        float rtaut = std::min(1.f, rdist / THREAD_TAUT_PX);
        // Keep the physics running as the grip springs home: the chain recoils
        // and settles under damping for a natural retract (not a stiff lerp),
        // then dissolves as g_tetherBreakFade fades.
        float rslack = (1.f - rtaut) * 42.f + 6.f;
        RopeStep(g_tetherLastSrcX, g_tetherLastSrcY, g_tetherTipX, g_tetherTipY, rdist + rslack, g_frameDeltaMs);
        PresentPhysicsRope(g_tetherBreakFade);
        return;
    }

    if (!inDrag) {
        // Drag fully ended -- hard reset (the drop path also calls HideDragTether).
        if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE);
        g_tetherTipInit    = false;
        g_tetherBreakFade  = 1.f;
        g_tetherWasVisible = false;
        g_tetherRetracting = false;
        return;
    }

    // ------------------------------------------------------------------
    // USER SPEC (refined): the rope must stay visible for the ENTIRE drag --
    // over the dock / taskbar AND over app windows -- so it is NO LONGER
    // retracted or hidden just because the cursor re-enters the dock
    // drop-zone (g_dropZoneActive). The rope's lifetime is now tied SOLELY to
    // the active drag (inDrag) -- exactly the gate that governs the ghost icon
    // -- so the two are coupled: "no dragged icon => no rope". The earlier
    // drop-zone-only suppression (which blanked the rope over the taskbar and
    // made it feel like an independent state) is removed. The mid-rope SNAP on
    // an off-dock release and the release RETRACT are still handled by the
    // dedicated blocks at the top of this function.
    // ------------------------------------------------------------------
    g_tetherRetracting = false;      // always follow the cursor while dragging

    // Fixed end = the dragged icon's original slot, on the dock edge nearest
    // the cursor, in SCREEN coordinates. Snapshot index/count under g_cs.
    int idx = g_dragFromDockIdx, n = 0;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        n = (int)g_pinnedApps.size();
        LeaveCriticalSection(&g_cs);
    }
    if (n <= 0 || idx < 0 || idx >= n) { if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE); return; }
    if (g_cachedDockRect.right <= g_cachedDockRect.left) { if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE); return; }

    // GLIDING ANCHOR -- a ring sliding on a rail around the WHOLE dock outline.
    // The fixed end is the closest point on the dock rectangle's perimeter to
    // the cursor, so as the icon is pulled off in ANY direction the attach point
    // slides continuously along whichever of the four edges (and around the
    // corners) the cursor is nearest -- not just the bottom edge as before,
    // where the other three edges never glided. The live anchor is then EASED
    // toward that target every frame (frame-rate scaled) so it glides smoothly
    // instead of snapping between edges, which removes the uneven top/bottom
    // sync the old discrete top-or-bottom pick produced.
    float dockL = (float)g_cachedDockRect.left,  dockR = (float)g_cachedDockRect.right;
    float dockT = (float)g_cachedDockRect.top,   dockB = (float)g_cachedDockRect.bottom;
    const float EDGE_INSET = 3.f;                 // keep the ring a hair inside the outline
    float inL = dockL + EDGE_INSET, inR = dockR - EDGE_INSET;
    float inT = dockT + EDGE_INSET, inB = dockB - EDGE_INSET;
    if (inR < inL) inL = inR = (dockL + dockR) * 0.5f;   // degenerate-rect guards
    if (inB < inT) inT = inB = (dockT + dockB) * 0.5f;
    float curX = (float)cursorPt.x, curY = (float)cursorPt.y;
    // Closest point on the inset rectangle to the cursor (the ring's target).
    float glideTargetX = std::min(std::max(curX, inL), inR);
    float glideTargetY = std::min(std::max(curY, inT), inB);
    // If the cursor is INSIDE the dock, project the anchor out to the nearest
    // edge so the ring always rides the outline (a rail), never floats inside.
    if (curX > inL && curX < inR && curY > inT && curY < inB) {
        float dL = curX - inL, dR = inR - curX, dT = curY - inT, dB = inB - curY;
        float m = std::min(std::min(dL, dR), std::min(dT, dB));
        if      (m == dL) glideTargetX = inL;
        else if (m == dR) glideTargetX = inR;
        else if (m == dT) glideTargetY = inT;
        else              glideTargetY = inB;
    }
    // Ease the live anchor toward the target (smooth glide). Snap straight to
    // the target on the first frame of a fresh drag (tip not yet initialised)
    // so the ring starts already on the outline instead of sliding in from a
    // stale position.
    float anchorTf = std::min(1.f, (g_frameDeltaMs / 16.0f) * 0.28f);   // gentle ease => smooth glide, no corner wobble
    float srcX, srcY;
    if (!g_tetherTipInit) { srcX = glideTargetX; srcY = glideTargetY; }
    else {
        srcX = g_tetherLastSrcX + (glideTargetX - g_tetherLastSrcX) * anchorTf;
        srcY = g_tetherLastSrcY + (glideTargetY - g_tetherLastSrcY) * anchorTf;
    }

    // Freeze the live anchor every frame so a release-triggered SNAP or RETRACT
    // (which runs after the dock slot is gone) tears/springs from the right spot,
    // and so the next frame's ease continues smoothly from here.
    g_tetherLastSrcX = srcX; g_tetherLastSrcY = srcY;

    // Moving end: smooth toward the cursor for a slight whip/lag. Frame-rate
    // independent so the lag feels the same regardless of the worker cadence.
    if (!g_tetherTipInit) { g_tetherTipX = (float)cursorPt.x; g_tetherTipY = (float)cursorPt.y; g_tetherTipInit = true; g_tetherBreaking = false; RopeReset(srcX, srcY, g_tetherTipX, g_tetherTipY); }
    // Normally the tip eases toward the CURSOR with a slight whip lag. During a
    // retract it instead springs back toward the dock anchor (srcX/srcY) while
    // the whole rope fades over ~THREAD_RETRACT_MS. Both are frame-rate scaled.
    float targetX  = g_tetherRetracting ? srcX : (float)cursorPt.x;
    float targetY  = g_tetherRetracting ? srcY : (float)cursorPt.y;
    float followTf = g_tetherRetracting
                     ? std::min(1.f, (g_frameDeltaMs / (float)THREAD_RETRACT_MS) * 2.4f)
                     : std::min(1.f, (g_frameDeltaMs / 16.0f) * THREAD_SPRING);
    g_tetherTipX += (targetX - g_tetherTipX) * followTf;
    g_tetherTipY += (targetY - g_tetherTipY) * followTf;
    float tipX = g_tetherTipX, tipY = g_tetherTipY;

    if (g_tetherRetracting) {
        g_tetherBreakFade = std::max(0.f, g_tetherBreakFade - (g_frameDeltaMs / (float)THREAD_RETRACT_MS));
        float rdx = tipX - srcX, rdy = tipY - srcY;
        // Finished once faded out or the tip has sprung home to the anchor.
        if (g_tetherBreakFade <= 0.02f || (rdx * rdx + rdy * rdy) < 9.f) {
            if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE);
            g_tetherTipInit    = false;
            g_tetherBreakFade  = 1.f;
            g_tetherWasVisible = false;
            g_tetherRetracting = false;
            return;
        }
    }

    // Stretch reads taut once past THREAD_TAUT_PX (bow straightens). The thread
    // only begins to SNAP + fade once pulled past the much larger break
    // distance -- so it stays fully visible for the whole realistic pull-off
    // gesture instead of fading almost immediately. The fade step is scaled by
    // the real frame delta so the snap always lasts ~THREAD_BREAK_MS.
    float dx = tipX - srcX, dy = tipY - srcY;
    float dist = sqrtf(dx * dx + dy * dy);
    float taut = std::min(1.f, dist / THREAD_TAUT_PX);   // 0 slack .. 1 taut (bow removed at 1)

    // ------------------------------------------------------------------
    // FINITE ROPE + MID-DRAG BREAK (user spec): the rope is NOT infinite. Once
    // the pull reaches THREAD_MAX_STRETCH_PX the rope has hit its defined
    // length and TEARS in the middle right there -- the user does NOT have to
    // release to unpin. This gives a clear, self-defining break point (the
    // rope's own max length) and fires the SAME unpin + "Thanos" vanish
    // feedback as the release path. Guards:
    //   * !g_tetherBreaking  -> fire exactly once per pull-off.
    //   * !g_dropZoneActive  -> only when actually pulled OFF the dock, so
    //     sliding along the taskbar near the dock never auto-unpins.
    if (!g_tetherBreaking && !g_dropZoneActive && dist >= THREAD_MAX_STRETCH_PX) {
        int lockedIdx = g_dragFromDockIdx;

        // Snapshot the pinned icon for the disintegration BEFORE unpinning
        // (mirrors the release handler -- the dock icon is always valid here).
        HICON vanishIcon = NULL;
        if (g_csInitialized) {
            EnterCriticalSection(&g_cs);
            if (lockedIdx >= 0 && lockedIdx < (int)g_pinnedApps.size() &&
                g_pinnedApps[lockedIdx].icon)
                vanishIcon = CopyIcon(g_pinnedApps[lockedIdx].icon);
            LeaveCriticalSection(&g_cs);
        }

        // Freeze the endpoints so the tear parts exactly where the rope reached
        // its limit; DrawTetherRope(..., broken=true) splits it near the
        // midpoint -> the "break in the middle" feedback the user asked for.
        g_tetherBreakSrcX = srcX; g_tetherBreakSrcY = srcY;
        g_tetherBreakTipX = tipX; g_tetherBreakTipY = tipY;
        g_tetherBreakFade  = 1.f;
        g_tetherWasVisible = true;
        g_tetherRetracting = false;
        g_tetherBreaking   = true;

        // CRITICAL ORDER: flip the drag state OFF DRAG_DRAGGING *before*
        // unpinning. UnpinAppByIndex() is a deliberate no-op while
        // g_dragState == DRAG_DRAGGING (a guard against mid-drag vector edits),
        // so unpinning first would silently fail and the icon would stay pinned
        // even though the rope "broke" (the reported bug). DRAG_CANCELLED also
        // lets the button-up path auto-reset to IDLE + GhostCleanup while the
        // worker loop's `else if (g_tetherBreaking...)` pump plays the tear to
        // completion. Hide the ghost so nothing lingers at the cursor.
        if (g_ghostWnd) ShowWindow(g_ghostWnd, SW_HIDE);
        g_dragState = DRAG_CANCELLED;

        // Real unpin now (state is no longer DRAGGING, so it actually removes
        // the icon); the vanish crumbles the icon at the grip (tip) point.
        UnpinAppByIndex(lockedIdx);
        HICON srcIcon = vanishIcon ? vanishIcon : g_dragGhostIcon;
        if (srcIcon)
            TriggerIconVanish(srcIcon, (int)tipX - GHOST_SIZE / 2,
                              (int)tipY - GHOST_SIZE / 2 - 6, GHOST_SIZE);
        if (vanishIcon) DestroyIcon(vanishIcon);

        // Present the first frame of the tear on this very frame.
        DrawTetherRope(g_tetherBreakSrcX, g_tetherBreakSrcY,
                       g_tetherBreakTipX, g_tetherBreakTipY, 1.f, true, 0.f);
        return;
    }
    if (g_tetherBreakFade <= 0.02f) { if (g_tetherWnd) ShowWindow(g_tetherWnd, SW_HIDE); return; }

    // Intact (or retracting) rope: one thin, flowing-gradient thread from the
    // dock anchor to the cursor. The shared renderer handles surface, colour,
    // stroke and present; alpha comes from g_tetherBreakFade (so retract fades).
    // TEMP DIAGNOSTIC (throttled ~4/sec): confirms the live-rope draw is reached
    // and shows the endpoints. Remove once the rope is confirmed working.
    { static DWORD s_t = 0; DWORD nowt = GetTickCount();
      if (nowt - s_t > 250) { s_t = nowt;
        LOG_IMPORTANT(L"TETHER-DBG live-draw src=(%.0f,%.0f) tip=(%.0f,%.0f) dist=%.0f fade=%.2f",
                      srcX, srcY, tipX, tipY, dist, g_tetherBreakFade); } }
    // Advance + present the VERLET rope. Rest length = straight distance plus a
    // little slack (grows as the rope goes slack, ~none when taut) so the
    // surplus length sags into an organic arc under gravity while a hard pull
    // straightens it out. The bend / lag / settle all emerge from the sim.
    //
    // STRETCH-NEAR-BREAK: as the pull approaches THREAD_MAX_STRETCH_PX the rope
    // is drawn progressively TIGHTER than the straight distance (rest < dist) so
    // the springs are visibly over-tensioned, the sag slack vanishes, and the
    // strand necks thinner (stretch01 passed to the renderer). This makes it
    // read as a thread straining right before it tears, instead of snapping
    // with no build-up.
    float nearBreak = (THREAD_MAX_STRETCH_PX > 1.f)
                          ? std::min(1.f, dist / THREAD_MAX_STRETCH_PX) : 0.f;
    // Concentrate the "straining" feel into the last part of the pull with an
    // ease-in (cubic) curve: the rope reads calm for most of the drag, then
    // visibly over-tensions and necks HARD in the final approach to the break.
    float strain    = nearBreak * nearBreak * nearBreak;                 // ease-in 0..1
    float slack     = ((1.f - taut) * 42.f + 6.f) * (1.f - nearBreak);   // slack -> 0 near break
    // Draw the rope progressively SHORTER than the straight span as it nears the
    // limit (rest < dist => springs over-tensioned, strand pulls dead-straight
    // and taut) -- up to ~20% tighter right before it tears, so it clearly reads
    // as a thread straining to its limit instead of snapping with no build-up.
    float restTotal = dist * (1.f - 0.20f * strain) + slack;
    RopeStep(srcX, srcY, tipX, tipY, restTotal, g_frameDeltaMs);
    // Pass the eased strain (not the raw ratio) so the visible necking also
    // ramps up mostly in the final approach, matching the tension build-up.
    PresentPhysicsRope(g_tetherBreakFade, strain);
}

// ---- Icon disintegration: soft round dot into the vanish DIB (BGRA top-down).
// Mirrors TetherPlot but targets g_vanishBits so the two overlays never share
// a buffer.
static inline void VanishPlot(int bufW, int bufH, float fx, float fy,
                              int r8, int g8, int b8, float coverage, float radius) {
    if (coverage <= 0.f || radius <= 0.f) return;
    // Sub-pixel dust mote: distance from each pixel CENTRE to the true float
    // position (no integer snapping) with a smoothstep edge -> soft round grain
    // that doesn't shimmer. Additive blend so overlapping motes read as a denser
    // cloud (unlike the rope, dust SHOULD accumulate).
    int x0 = (int)floorf(fx - radius - 1.5f), x1 = (int)ceilf(fx + radius + 1.5f);
    int y0 = (int)floorf(fy - radius - 1.5f), y1 = (int)ceilf(fy + radius + 1.5f);
    for (int y = y0; y <= y1; ++y) {
        if (y < 0 || y >= bufH) continue;
        float ddy = (float)y + 0.5f - fy;
        for (int x = x0; x <= x1; ++x) {
            if (x < 0 || x >= bufW) continue;
            float ddx = (float)x + 0.5f - fx;
            float d = sqrtf(ddx * ddx + ddy * ddy);
            float e0 = radius - 0.6f, e1 = radius + 0.6f;
            float t = (e1 > e0) ? (d - e0) / (e1 - e0) : (d >= e1 ? 1.f : 0.f);
            if (t < 0.f) t = 0.f; else if (t > 1.f) t = 1.f;
            float a = coverage * (1.f - t * t * (3.f - 2.f * t));
            if (a <= 0.003f) continue;
            BYTE* px = g_vanishBits + ((size_t)y * bufW + x) * 4;   // BGRA
            px[0] = (BYTE)std::min(255.f, px[0] * (1 - a) + b8 * a);
            px[1] = (BYTE)std::min(255.f, px[1] * (1 - a) + g8 * a);
            px[2] = (BYTE)std::min(255.f, px[2] * (1 - a) + r8 * a);
            px[3] = (BYTE)std::min(255.f, px[3] + 255.f * a);
        }
    }
}

// FIXED-SIZE vanish DIB/window (mirrors EnsureTetherSurface).
//
// ROOT-CAUSE FIX (black slab behind the disintegrating icon + rope on real
// Windows 11 hardware, clean in a VM): the old code sized the vanish window to
// each effect's content bounding box and let UpdateLayeredWindow RESIZE the
// layered window between effects (a small single-icon unpin vs. a wide
// unpin-all differ in size, and the box also differs per icon size/position).
// Resizing a layered (ULW) window exposes a fresh, still-uninitialised backing
// surface that a hardware DWM compositor presents as OPAQUE BLACK for one frame
// before the content lands (a VM's software compositor coalesces it, so it
// looks clean there). This is the exact artifact the user sees ONLY on their
// main PC when they pull an icon off the dock to unpin it.
//
// We now allocate the surface ONCE at a fixed size big enough to cover the
// widest plausible disintegration (a full-width unpin-all across the dock plus
// the motes' upward/sideways drift). The window then only ever MOVES (via
// UpdateLayeredWindow's pptDst) and is never resized, so no black frame is ever
// exposed. Content is drawn into the top-left region at a local offset and the
// surplus stays fully transparent. The DIB is only re-created if the desktop
// resolution actually changes (never per effect).
static bool EnsureVanishSurface() {
    // Fixed dimensions from the virtual desktop, clamped so the DIB stays a sane
    // size. Width covers a full-width unpin-all; height covers an icon plus its
    // full vertical dust drift (motes lift ~2-3x the icon size upward).
    int fw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    if (fw < 640)  fw = 640;
    if (fw > 1600) fw = 1600;   // bound the layered window: a small/moderate surface avoids the
                                // real-GPU flashing that a screen-wide layered window can cause
                                // (covers a single-icon unpin and a typical full-dock unpin-all)
    const int fh = 512;   // icon + full vertical dust drift never needs the whole screen height
    int w = fw, h = fh;
    if (!g_vanishWnd) {
        static bool s_cls = false;
        if (!s_cls) {
            WNDCLASSEXW wc = { sizeof(wc) };
            wc.lpfnWndProc   = DefWindowProcW;
            wc.hInstance     = GetModuleHandleW(NULL);
            wc.lpszClassName = L"QPDockVanish";
            RegisterClassExW(&wc);
            s_cls = true;
        }
        g_vanishWnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
            L"QPDockVanish", L"", WS_POPUP,
            0, 0, w, h, NULL, NULL, GetModuleHandleW(NULL), NULL);
        if (!g_vanishWnd) return false;
    }
    if (g_vanishDIB && g_vanishBits && w == g_vanishW && h == g_vanishH)
        return true;
    if (g_vanishDIB) {
        DeleteObject(g_vanishDIB);
        g_vanishDIB = NULL; g_vanishBits = NULL;
    }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;   // top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC sdc = GetDC(NULL);
    if (sdc) {
        g_vanishDIB = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS,
                                       (void**)&g_vanishBits, NULL, 0);
        ReleaseDC(NULL, sdc);
    }
    if (!g_vanishDIB) { g_vanishBits = NULL; g_vanishW = g_vanishH = 0; return false; }
    g_vanishW = w; g_vanishH = h;
    return true;
}

// Start building a (possibly multi-icon) disintegration. Clears any previous
// mote set and resets the bounding-box + accent accumulators.
static void BeginVanish() {
    g_vanishParticles.clear();
    g_vanBBminX = g_vanBBminY = 1e9f;
    g_vanBBmaxX = g_vanBBmaxY = -1e9f;
    g_vanAccSumR = g_vanAccSumG = g_vanAccSumB = 0;
    g_vanAccCount = 0;
}

// Rasterise ONE icon and append its motes at absolute SCREEN top-left
// (screenX, screenY). Safe to call several times between Begin/Commit to
// dissolve many icons at once (e.g. unpin-all). Accumulates the screen bbox
// (base position + full drift extent) and the icon's average colour (accent).
static void AddIconVanish(HICON hIcon, int screenX, int screenY, int S) {
    if (!hIcon || S <= 0) return;
    std::vector<BYTE> px;
    if (!RasterizeIconRGBA(hIcon, S, px)) return;
    if ((int)px.size() < S * S * 4) return;

    unsigned seed = (unsigned)screenX * 73856093u ^ (unsigned)screenY * 19349663u ^ GetTickCount();
    auto rnd = [&]() {
        seed = seed * 1103515245u + 12345u;
        return (float)((seed >> 8) & 0xFFFF) / 65535.f;   // 0..1
    };

    const float denom = (S > 1) ? (float)(2 * (S - 1)) : 1.f;
    for (int y = 0; y < S; ++y) {
        for (int x = 0; x < S; ++x) {
            const BYTE* p = &px[((size_t)y * S + x) * 4];   // BGRA, top-down
            BYTE a8 = p[3];
            if (a8 < 16) continue;                          // skip transparent pixels
            VanishParticle vp;
            vp.x = (float)(screenX + x); vp.y = (float)(screenY + y);   // ABSOLUTE screen px
            vp.b = p[0]; vp.g = p[1]; vp.r = p[2]; vp.a = a8;
            // Dissolve sweep: the top-right corner turns to dust first, then it
            // washes down/left across the icon (Thanos-style progressive fade).
            float sweep = ((float)(S - 1 - y) + (float)x) / denom;   // 0..1
            vp.delay = sweep * 0.42f + rnd() * 0.10f;
            if (vp.delay < 0.f) vp.delay = 0.f;
            // Dust is BLOWN APART -- strong upward lift + wide sideways scatter.
            vp.vx = (rnd() - 0.5f) * S * 1.8f + 0.30f * S;
            vp.vy = -((0.7f + rnd() * 1.5f) * S);            // negative = upward
            g_vanishParticles.push_back(vp);

            // Bounding box over base pos + the mote's full drift travel (+2px pad).
            float exLo = vp.x + (vp.vx < 0 ? vp.vx : 0.f) - 2.f;
            float exHi = vp.x + (vp.vx > 0 ? vp.vx : 0.f) + 2.f;
            float eyLo = vp.y + (vp.vy < 0 ? vp.vy : 0.f) - 2.f;
            float eyHi = vp.y + (vp.vy > 0 ? vp.vy : 0.f) + 2.f;
            if (exLo < g_vanBBminX) g_vanBBminX = exLo;
            if (exHi > g_vanBBmaxX) g_vanBBmaxX = exHi;
            if (eyLo < g_vanBBminY) g_vanBBminY = eyLo;
            if (eyHi > g_vanBBmaxY) g_vanBBmaxY = eyHi;

            g_vanAccSumR += p[2]; g_vanAccSumG += p[1]; g_vanAccSumB += p[0];
            ++g_vanAccCount;
        }
    }
}

// Finish a vanish: derive the accent (average icon colour, lifted toward white
// so it reads as an ember), set the overlay window box, and arm the animation.
static void CommitVanish() {
    if (g_vanishParticles.empty() || g_vanAccCount <= 0) { g_vanishParticles.clear(); return; }
    int ar = (int)(g_vanAccSumR / g_vanAccCount);
    int ag = (int)(g_vanAccSumG / g_vanAccCount);
    int ab = (int)(g_vanAccSumB / g_vanAccCount);
    // Lift toward white so the flash reads as a glowing ember of the icon's hue.
    g_vanishAccentR = std::min(255, (int)(ar + (255 - ar) * 0.55f));
    g_vanishAccentG = std::min(255, (int)(ag + (255 - ag) * 0.55f));
    g_vanishAccentB = std::min(255, (int)(ab + (255 - ab) * 0.55f));

    g_vanishOriginX = (int)floorf(g_vanBBminX);
    g_vanishOriginY = (int)floorf(g_vanBBminY);
    g_vanishBoxW    = (int)ceilf(g_vanBBmaxX) - g_vanishOriginX;
    g_vanishBoxH    = (int)ceilf(g_vanBBmaxY) - g_vanishOriginY;
    if (g_vanishBoxW < 2 || g_vanishBoxH < 2) { g_vanishParticles.clear(); return; }
    // Safety cap so a pathological box can't request a huge DIB.
    if (g_vanishBoxW > 4096) g_vanishBoxW = 4096;
    if (g_vanishBoxH > 4096) g_vanishBoxH = 4096;

    g_vanishProgress = 0.f;
    g_vanishActive   = true;
}

// Convenience: dissolve a single icon at (screenX, screenY). The HICON is
// rasterised immediately, so the caller may free it right after.
static void TriggerIconVanish(HICON hIcon, int screenX, int screenY, int S) {
    BeginVanish();
    AddIconVanish(hIcon, screenX, screenY, S);
    CommitVanish();
}

// Advance + present ONE frame of the icon disintegration. Self-contained: it
// steps its own progress by the real frame delta, cleans up exactly once when
// finished, and is safe to call every worker frame (no-op while inactive).
static void UpdateVanishWindow() {
    if (!g_vanishActive) return;

    g_vanishProgress += g_frameDeltaMs / (float)VANISH_MS;
    if (g_vanishProgress >= 1.f || g_vanishParticles.empty() ||
        g_vanishBoxW < 2 || g_vanishBoxH < 2) {
        g_vanishActive   = false;
        g_vanishProgress = 0.f;
        g_vanishParticles.clear();
        if (g_vanishWnd) ShowWindow(g_vanishWnd, SW_HIDE);
        return;
    }

    if (!EnsureVanishSurface()) { g_vanishActive = false; return; }
    // FIXED-size buffer/window (never resized -> no black-slab flash on real
    // GPUs; the window only MOVES, like the ghost + tether). The dust is drawn
    // into the buffer at a local offset (mote - origin); the surplus around it
    // stays fully transparent. w/h below are the FIXED surface dims, so the
    // memset, VanishPlot stride, PremultiplyAlpha span and the ULW size that
    // follow all operate on the whole fixed surface.
    int w = g_vanishW, h = g_vanishH;
    memset(g_vanishBits, 0, (size_t)w * h * 4);

    // Accent = the icon's own natural colour (lifted toward white), so the
    // ignition flash adapts per icon instead of a fixed warm white.
    float accR = (float)g_vanishAccentR, accG = (float)g_vanishAccentG, accB = (float)g_vanishAccentB;

    const float FADE_WIN = 0.60f;   // each mote fades over this fraction of progress after its delay
    float p = g_vanishProgress;
    for (const auto& pt : g_vanishParticles) {
        float lp = (p - pt.delay) / FADE_WIN;
        if (lp < 0.f) lp = 0.f;      // not yet dissolving -> still a solid icon pixel
        if (lp > 1.f) continue;      // fully gone
        float drift = lp * lp;       // ease-in: the mote hangs, then whisks away
        float fx = pt.x - (float)g_vanishOriginX + pt.vx * drift;   // absolute -> window-local
        float fy = pt.y - (float)g_vanishOriginY + pt.vy * drift;
        // Lingering dust tail: fade slower than linear so the cloud hangs in the
        // air a beat longer before vanishing (more dramatic, less abrupt).
        float a  = (pt.a / 255.f) * powf(1.f - lp, 0.7f);
        // Ember flash toward the ICON'S OWN accent colour the instant a mote
        // lets go, settling back to its true colour as it drifts away.
        float glow = (lp < 0.35f) ? (0.35f - lp) / 0.35f : 0.f;   // 1 at ignition -> 0
        int r8 = (int)std::min(255.f, pt.r + (accR - pt.r) * glow);
        int g8 = (int)std::min(255.f, pt.g + (accG - pt.g) * glow);
        int b8 = (int)std::min(255.f, pt.b + (accB - pt.b) * glow);
        float rad = 1.25f - 0.7f * lp;   // start a touch bigger, shrink to a fine speck
        VanishPlot(w, h, fx, fy, r8, g8, b8, a, rad);
    }

    PremultiplyAlpha(g_vanishBits, w * h);

    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    HDC mdc = CreateCompatibleDC(sdc);
    if (!mdc) { ReleaseDC(NULL, sdc); return; }
    HBITMAP oldb = (HBITMAP)SelectObject(mdc, g_vanishDIB);
    POINT dest  = { g_vanishOriginX, g_vanishOriginY };
    SIZE  sz    = { w, h };
    POINT srcpt = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    // Present with the SAME proven recipe as the drag ghost (fixed-size window,
    // SetWindowPos move+show FIRST, then ULW). w/h are the FIXED surface dims so
    // SetWindowPos only MOVES the window (never resizes) -- moving a layered
    // window this way is flicker-free, whereas letting ULW move a large layered
    // window via pptDst flashed an opaque BLACK slab behind the dust on real
    // hardware (clean in a VM). Mirrors DrawTetherRope / PresentPhysicsRope.
    SetWindowPos(g_vanishWnd, HWND_TOPMOST, dest.x, dest.y, w, h,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateLayeredWindow(g_vanishWnd, sdc, &dest, &sz, mdc, &srcpt, 0, &bf, ULW_ALPHA);
    SelectObject(mdc, oldb);
    DeleteDC(mdc);
    ReleaseDC(NULL, sdc);
}

// Present ONE fully-transparent frame on a freshly-created layered window so its
// backing surface is registered with DWM and initialised transparent up front.
static void PrewarmLayered(HWND wnd, HBITMAP dib, int w, int h) {
    if (!wnd || !dib || w < 1 || h < 1) return;
    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    HDC mdc = CreateCompatibleDC(sdc);
    if (mdc) {
        HBITMAP oldb = (HBITMAP)SelectObject(mdc, dib);
        POINT dest = { 0, 0 };
        SIZE  sz   = { w, h };
        POINT src  = { 0, 0 };
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        // The DIB is zero-initialised by CreateDIBSection => fully transparent.
        UpdateLayeredWindow(wnd, sdc, &dest, &sz, mdc, &src, 0, &bf, ULW_ALPHA);
        SelectObject(mdc, oldb);
        DeleteDC(mdc);
    }
    ReleaseDC(NULL, sdc);
    ShowWindow(wnd, SW_HIDE);
}

// Create + WARM the drag-effect layered windows (rope + vanish) at mod init,
// exactly like the ghost window.
//
// ROOT-CAUSE FIX (black rope + black icon on unpin, real hardware only, clean in
// a VM): the ghost never flashed black because it is CREATED ONCE at startup, so
// by the time the user drags, DWM has long since registered its layered surface.
// The tether and vanish windows were instead created LAZILY on the worker thread
// during the drag/unpin gesture -- and a freshly-created layered window's very
// FIRST UpdateLayeredWindow present can flash an opaque BLACK slab for one frame
// on a hardware DWM compositor (a VM's software compositor coalesces it away).
// Creating + presenting one transparent frame here at init gives the rope/vanish
// the same head start as the ghost, so their first real present during a gesture
// is an ordinary move, never a cold first-show. It also moves ownership of these
// windows to the main (UI) thread -- again matching the ghost, whose SetWindowPos
// + ULW are driven cross-thread from the worker with no black frame.
static void PrewarmDragEffectWindows() {
    if (EnsureTetherSurface())
        PrewarmLayered(g_tetherWnd, g_tetherDIB, g_tetherW, g_tetherH);
    if (EnsureVanishSurface())
        PrewarmLayered(g_vanishWnd, g_vanishDIB, g_vanishW, g_vanishH);
}

// ============================================================
//  CLEAN ICON BLIT  --  premultiplied-alpha, no dark edge fringe
//  Renders a 32-bit icon into the shared premultiplied-alpha DIB at ICON_SIZE
//  and AlphaBlend-stretches it to destSz. This is the SAME proven path the
//  fade-in branch uses. The resting/hover branch previously called DrawIconEx
//  STRAIGHT onto the colour-key back buffer, which does NOT alpha-composite an
//  icon's anti-aliased edge pixels -- so every icon picked up a dark (black)
//  fringe and looked low quality. Routing the opaque draw through here removes
//  that fringe and keeps edges smooth at any hover scale.
//  Main-thread paint only (owns the g_alphaBlend* cache). alpha = overall
//  constant opacity (255 = fully opaque).
// ============================================================
// ------------------------------------------------------------
//  CLEAN ICON RASTER  --  the fix for the dark "halo"/"stroke" fringe.
//  Reads an icon's TRUE 32-bit pixels via GetDIBits (STRAIGHT alpha, with NO
//  compositing over any background) and premultiplies them once, in place.
//  Why this matters: DrawIconEx onto a zeroed (black) DIB alpha-composites the
//  icon's anti-aliased edge pixels *over black*, so those edge pixels are
//  already darkened; premultiplying afterwards darkens them a SECOND time,
//  which is exactly the black fringe seen around every icon. Pulling the raw
//  colour-bitmap bits skips all compositing, so edges keep their real colour
//  and stay crisp. Legacy icons that carry no alpha channel fall back to the
//  AND mask. `dst` must point at size*size*4 bytes (top-down 32bpp BGRA).
// ------------------------------------------------------------
static bool FillIconPremulBits(HICON icon, int size, BYTE* dst) {
    if (!icon || size <= 0 || !dst) return false;
    ICONINFO ii = {};
    if (!GetIconInfo(icon, &ii)) return false;

    bool ok = false;
    HDC sdc = GetDC(NULL);
    if (sdc && ii.hbmColor) {
        BITMAPINFO bi         = {};
        bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth       = size;
        bi.bmiHeader.biHeight      = -size;   // top-down
        bi.bmiHeader.biPlanes      = 1;
        bi.bmiHeader.biBitCount    = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        if (GetDIBits(sdc, ii.hbmColor, 0, size, dst, &bi, DIB_RGB_COLORS)) {
            const int px = size * size;
            bool hasAlpha = false;
            for (int i = 0; i < px; ++i)
                if (dst[i * 4 + 3] != 0) { hasAlpha = true; break; }
            if (!hasAlpha) {
                // No alpha channel (legacy icon): derive it from the AND mask.
                // In the AND mask a 0 bit == opaque, a 1 bit == transparent.
                std::vector<BYTE> mask((size_t)px * 4, 0);
                if (ii.hbmMask &&
                    GetDIBits(sdc, ii.hbmMask, 0, size, mask.data(), &bi, DIB_RGB_COLORS)) {
                    for (int i = 0; i < px; ++i)
                        dst[i * 4 + 3] = (mask[(size_t)i * 4] == 0) ? 255 : 0;
                } else {
                    for (int i = 0; i < px; ++i) dst[i * 4 + 3] = 255;
                }
            }
            PremultiplyAlpha(dst, px);   // straight -> premultiplied (once)
            ok = true;
        }
    }
    if (sdc) ReleaseDC(NULL, sdc);
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);
    return ok;
}

static void BlitIconAlpha(HDC hdc, HICON icon, int destX, int destY, int destSz, BYTE alpha) {
    if (!icon || destSz <= 0) return;

    // Rasterise at the icon's NATIVE pixel size so DrawIconEx performs NO
    // scaling. Interpolating straight-alpha icon data (a scaled DrawIconEx /
    // StretchBlt) bleeds the icon's transparent pixels -- which are usually pure
    // BLACK -- into the anti-aliased edge, which is the dark "stroke" fringe plus
    // the blurry, low-quality look. Instead: draw 1:1 (no bleed) -> premultiply
    // -> let AlphaBlend do the resize. AlphaBlend scales PREMULTIPLIED data
    // correctly, so nothing black bleeds in and the downscale stays crisp.
    int srcSz = ICON_SIZE;
    ICONINFO ii = {};
    if (GetIconInfo(icon, &ii)) {
        BITMAP bm = {};
        if (ii.hbmColor && GetObjectW(ii.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0)
            srcSz = bm.bmWidth;
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask)  DeleteObject(ii.hbmMask);
    }
    if (srcSz < 1)   srcSz = ICON_SIZE;
    if (srcSz > 256) srcSz = 256;   // JUMBO cap -- bounds the cached buffer

    if (!g_iconBlitDC || g_iconBlitSize != srcSz) {
        if (g_iconBlitDC && g_iconBlitOldBmp) {
            SelectObject(g_iconBlitDC, g_iconBlitOldBmp);
            g_iconBlitOldBmp = NULL;
        }
        if (g_iconBlitDC)  { DeleteDC(g_iconBlitDC);  g_iconBlitDC  = NULL; }
        if (g_iconBlitBmp) { DeleteObject(g_iconBlitBmp); g_iconBlitBmp = NULL; }
        g_iconBlitBits = NULL;
        g_iconBlitSize = srcSz;

        BITMAPINFO bi         = {};
        bi.bmiHeader.biSize   = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth  = srcSz;
        bi.bmiHeader.biHeight = -srcSz;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount    = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        HDC screenDC = GetDC(NULL);
        if (screenDC) {
            g_iconBlitBmp = CreateDIBSection(
                screenDC, &bi, DIB_RGB_COLORS,
                (void**)&g_iconBlitBits, NULL, 0);
            g_iconBlitDC = CreateCompatibleDC(screenDC);
            if (g_iconBlitDC && g_iconBlitBmp)
                g_iconBlitOldBmp = (HBITMAP)SelectObject(g_iconBlitDC, g_iconBlitBmp);
            ReleaseDC(NULL, screenDC);
        }
    }
    if (g_iconBlitDC && g_iconBlitBmp && g_iconBlitBits) {
        memset(g_iconBlitBits, 0, (size_t)(srcSz * srcSz * 4));
        // Fill from the icon's TRUE bits (GetDIBits, NO compositing over black)
        // and premultiply once -- this removes the dark edge fringe. Rasterised
        // 1:1 at native size so AlphaBlend does the only (HALFTONE) resize.
        if (!FillIconPremulBits(icon, srcSz, g_iconBlitBits)) {
            // Fallback: legacy path (still better than nothing if GetDIBits fails).
            DrawIconEx(g_iconBlitDC, 0, 0, icon, srcSz, srcSz, 0, NULL, DI_NORMAL);
            PremultiplyAlpha(g_iconBlitBits, srcSz * srcSz);
        }
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
        SetStretchBltMode(hdc, HALFTONE);   // smooth resize of PREMULTIPLIED data -> no fringe
        AlphaBlend(hdc, destX, destY, destSz, destSz,
                   g_iconBlitDC, 0, 0, srcSz, srcSz, bf);
    }
}

// ============================================================
//  OVERLAY WNDPROC
// ============================================================
LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    // Hit-test: the dock owns its full window rectangle, including transparent
    // pixels and icon gaps. This prevents right-clicks from leaking into the
    // underlying taskbar jump-list/menu surface.
    case WM_NCHITTEST: {
        return HTCLIENT;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC paintDC = BeginPaint(hwnd, &ps);
        if (!paintDC) return 0;

        if (!g_csInitialized) { EndPaint(hwnd, &ps); return 0; }

        EnterCriticalSection(&g_cs);

        // Fill background with the colour-key RGB(1,0,1) so it becomes transparent.
        // RGB(0,0,0) was previously the key but caused the black-rectangle bug:
        // near-black glass gradient pixels were incorrectly keyed out.
        RECT cr = {};
        GetClientRect(hwnd, &cr);
        int paintW = cr.right - cr.left;
        int paintH = cr.bottom - cr.top;
        bool usingBackBuffer = EnsurePaintBuffer(paintDC, paintW, paintH);
        HDC hdc = usingBackBuffer ? g_paintDC : paintDC;
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
            if (usingBackBuffer) PresentPaintBuffer(paintDC, paintW, paintH);
            EndPaint(hwnd, &ps);
            return 0;
        }

        // macOS-style glass: hairline border + top sheen around the pill.
        DrawGlassEdge(hdc, cr);
        // Locked-state indicator: gold breathing edge glow (no-op when unlocked).
        DrawLockGlow(hdc, cr);
        int n = (int)g_pinnedApps.size();
        if (n == 0) {
            // Empty dock: draw subtle glass background + separator so the dock has
            // a visible presence even before any apps are pinned.
            // Without this the entire client area is black (= transparent colour-key)
            // and the dock is completely invisible, leaving users unable to tell
            // whether the mod loaded at all.
            if (SEPARATOR_OPACITY > 0) {
                if (!g_linePenNormal) g_linePenNormal = CreatePen(PS_SOLID, 1, RGB(80, 80, 80));
                HPEN oldP = (HPEN)SelectObject(hdc, g_linePenNormal);
                MoveToEx(hdc, cr.right - 1, cr.top    + 3, NULL);
                LineTo  (hdc, cr.right - 1, cr.bottom - 3);
                SelectObject(hdc, oldP);
            }
            g_hoverIndex       = -1;
            g_dragFromDockIdx  = -1;
            // FIX (dock stays red after unpin): this empty-dock branch previously
            // returned WITHOUT refreshing the acrylic tint, so when the LAST icon
            // was unpinned the window kept whatever accent the drag left behind --
            // the vivid red "unpin" feedback -- and it never reset to neutral.
            // Recompute the tint here (the drag state is already reset, so this
            // resolves to the neutral frosted glass) before presenting.
            ApplyDockGlassTint(hwnd);
            LeaveCriticalSection(&g_cs);
            if (usingBackBuffer) PresentPaintBuffer(paintDC, paintW, paintH);
            EndPaint(hwnd, &ps);
            return 0;
        }

        // Tinted frosted-glass state feedback is now provided by the acrylic
        // blur (ApplyDockGlassTint) instead of an opaque GradientFill, so the
        // dock body stays translucent and the wallpaper blurs through.
        ApplyDockGlassTint(hwnd);

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

        // Section divider between workspace/folder pins and app pins. This is a
        // quiet structural cue, distinct from the right-edge dock separator.
        // Rendered as a short vertical "pill" (rounded-end capsule) in the
        // Explorer-accent gold #FFD766 so it reads as a deliberate group divider
        // rather than a thin hairline. Only this section divider is styled --
        // the right-edge dock separator is intentionally left untouched.
        int sepWsSlots  = std::min(CountPinsByType(PIN_WORKSPACE), MAX_WORKSPACE_PINS);
        int sepAppCount = CountPinsByType(PIN_APP);
        // Only draw the workspace/app divider when BOTH groups are populated --
        // this matches the dock-width section gap (an empty workspace region has
        // no gap, so there is nothing to divide).  Position it from the ACTUAL
        // workspace slot count so it sits in the centre of the real gap.
        if (SEPARATOR_OPACITY > 0 && sepWsSlots > 0 && sepAppCount > 0) {
            int sepWsRegionW = (sepWsSlots * ICON_SIZE) + ((sepWsSlots - 1) * ICON_SPACING);
            int sepX = DOCK_PAD_LEFT + sepWsRegionW + SECTION_GAP / 2;

            // Pill geometry: a little thicker than a hairline, with rounded
            // ends. Width scales gently with DPI (via ICON_SIZE) but is clamped
            // so it never looks like a fat bar.
            int pillW = std::max(3, std::min(5, ICON_SIZE / 9));   // ~3-5 px thick
            int inset = 6;                                          // vertical padding from top/bottom
            int pillTop    = cr.top + inset;
            int pillBottom = cr.bottom - inset;
            if (pillBottom > pillTop) {
                int pillLeft  = sepX - pillW / 2;
                int pillRight = pillLeft + pillW;
                int radius    = pillW;   // fully rounded capsule ends

                // #FFC832 -- richer, more saturated Explorer-style gold accent.
                HBRUSH pillBrush = CreateSolidBrush(RGB(255, 200, 50));
                HPEN   pillPen   = CreatePen(PS_SOLID, 1, RGB(255, 200, 50));
                if (pillBrush && pillPen) {
                    HBRUSH oldB = (HBRUSH)SelectObject(hdc, pillBrush);
                    HPEN   oldP = (HPEN)SelectObject(hdc, pillPen);
                    RoundRect(hdc, pillLeft, pillTop, pillRight, pillBottom,
                              radius, radius);
                    SelectObject(hdc, oldB);
                    SelectObject(hdc, oldP);
                }
                if (pillBrush) DeleteObject(pillBrush);
                if (pillPen)   DeleteObject(pillPen);
            }
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
            // Neighbour SPREAD (macOS-dock feel): shift magnified icons outward
            // from the focus so they don't overlap. hoverShiftX is eased in the
            // worker loop and is 0 for icons that aren't magnifying, so this is a
            // no-op at rest. Rounded so the icon centre stays crisp.
            baseX += (int)lroundf(app.hoverShiftX);
            if (baseX < 0) continue;
            // FIX (phantom "second dock" cluster on the primary taskbar):
            // overflow icons (>MAX_VISIBLE_APP_SLOTS pins) are parked just off
            // the RIGHT edge at x = g_dockLocalW + ICON_SPACING. The left
            // overflow was skipped above, but the right side had NO guard and
            // relied entirely on the window region / back-buffer clipping those
            // pixels. When a pin/unpin sets g_dockWidthDirty and the width is
            // recomputed (e.g. the "13 apps -> remove while locked" case), the
            // width recompute can briefly lag the actual window resize, so the
            // right-parked overflow icons render into the stale extra width as a
            // duplicate cluster beside the real dock. Skipping any icon whose
            // LEFT edge has reached/passed the dock's right border makes right
            // overflow symmetric with left overflow: fully-off icons never draw,
            // while partially-visible slide-in/out icons still animate.
            if (baseX >= g_dockLocalW) continue;

            if (app.opacity < 1.0f) {
                // ---- Fade-in: draw at base ICON_SIZE using cached alpha-blend buffer ----
                int sz = ICON_SIZE;
                if (!g_alphaBlendDC || g_alphaBlendSize != sz) {
                    if (g_alphaBlendDC && g_alphaBlendOldBmp) {
                        SelectObject(g_alphaBlendDC, g_alphaBlendOldBmp);
                        g_alphaBlendOldBmp = NULL;
                    }
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
                            g_alphaBlendOldBmp = (HBITMAP)SelectObject(g_alphaBlendDC, g_alphaBlendBmp);
                        ReleaseDC(NULL, screenDC);
                    }
                }
                {
                    // Clean premultiplied blit (GetDIBits raster, no black edge
                    // fringe, crisp HALFTONE downscale) -- same path as the
                    // resting/hover branch, just carrying the fade-in opacity.
                    BlitIconAlpha(hdc, app.icon, baseX, r.top, sz, (BYTE)(int)(255 * app.opacity));
                    if (app.type == PIN_WORKSPACE) {
                        // baseX already includes hoverShiftX (defined above).
                        RECT badgeRect = { baseX, r.top, baseX + sz, r.top + sz };
                        DrawWorkspaceGlyph(hdc, badgeRect, (BYTE)(int)(255 * app.opacity));
                    }
                }
            } else {
                // ---- Full opacity: draw at animated hover scale, centred on slot ----
                float scale  = (app.hoverScale > 1.0f) ? app.hoverScale : 1.0f;
                int   sz     = (int)lroundf(ICON_SIZE * scale);
                // FIX (hover wobble): the icon size grows in integer GDI steps.
                // The old code offset by (sz-ICON_SIZE)/2 with integer truncation
                // and anchored Y at r.top, so every 1px step nudged the icon
                // asymmetrically left/right and bobbed it up/down -- the wobble.
                // Pin the icon CENTRE to the slot centre and round symmetrically
                // so scaling reads as a smooth zoom about the centre.
                // baseX already includes the neighbour-spread offset (hoverShiftX),
                // so the magnified icon centre follows the spread.
                float cxF    = (float)baseX + ICON_SIZE * 0.5f;
                float cyF    = (float)r.top + ICON_SIZE * 0.5f;
                int   x      = std::max(0, (int)lroundf(cxF - sz * 0.5f));
                int   y      = (int)lroundf(cyF - sz * 0.5f);
                // FIX (black edge fringe + low-quality icons): the resting/hover
                // icon was drawn with DrawIconEx STRAIGHT onto the colour-key back
                // buffer, which does NOT alpha-composite the icon's anti-aliased
                // edge pixels -- so every icon carried a dark (black) halo and the
                // edges looked blocky. Draw through the SAME premultiplied-alpha
                // path the fade-in branch uses: BlitIconAlpha rasterises at
                // ICON_SIZE then AlphaBlend-stretches to the hover size, so edges
                // stay clean and smooth with no fringe.
                BlitIconAlpha(hdc, app.icon, x, y, sz, 255);
                if (app.type == PIN_WORKSPACE) {
                    RECT badgeRect = { x, y, x + sz, y + sz };
                    DrawWorkspaceGlyph(hdc, badgeRect);
                }
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
                // Follow the icon's spread offset so the running-dot stays under
                // its icon during magnification (mirrors the draw loop's shift).
                baseX += (int)lroundf(app.hoverShiftX);
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
                    if (g_alphaBlendDC && g_alphaBlendOldBmp) {
                        SelectObject(g_alphaBlendDC, g_alphaBlendOldBmp);
                        g_alphaBlendOldBmp = NULL;
                    }
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
                            if (g_alphaBlendDC) g_alphaBlendOldBmp = (HBITMAP)SelectObject(g_alphaBlendDC, g_alphaBlendBmp);
                            else { DeleteObject(g_alphaBlendBmp); g_alphaBlendBmp = NULL; }
                        }
                        ReleaseDC(NULL, sdc);
                    }
                }
                {
                    // Centre icon on cursor, clamped to dock client area
                    int localX = cursorPt.x - (int)g_dockCurrentX - sz / 2;
                    int localY = (g_dockLocalH - sz) / 2;
                    localX = std::max((int)cr.left, std::min(localX, (int)cr.right - sz));
                    // Clean premultiplied blit (no black edge fringe); ~76% alpha.
                    BlitIconAlpha(hdc, src.icon, localX, localY, sz, 195);
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

                // Feedback colours -- intensified for a clearer, more vivid signal.
                //   FLASH (red)  = limit hit / unpin.   DROP (green) = valid drop zone.
                // Pure, high-saturation hues read much better against the glass tint
                // than the older muted tones. Defined once so the pen cache and the
                // selection test below always compare against the SAME value (the
                // previous code compared against different literals, so the flash/drop
                // pens were never actually selected -- a real colour bug).
                const COLORREF FEEDBACK_RED   = RGB(255,  45,  45);   // vivid red
                const COLORREF FEEDBACK_GREEN = RGB(  0, 220,  90);   // vivid green
                const COLORREF FEEDBACK_NEUTRAL = RGB( 80,  80,  80);

                // When glass overlay is active, the dock itself tints red for limit
                // flash  --  the separator reverts to neutral so we don't double-signal.
                COLORREF lineColor = (g_limitFlashActive && !ENABLE_GLASS_OVERLAY)
                                                               ? FEEDBACK_RED
                                   : (inDrop && g_dragState == DRAG_DRAGGING)
                                                               ? FEEDBACK_GREEN
                                                               : FEEDBACK_NEUTRAL;

                // Ensure cached pens exist for each colour
                if (!g_linePenNormal) g_linePenNormal = CreatePen(PS_SOLID, 1, FEEDBACK_NEUTRAL);
                if (!g_linePenFlash)  g_linePenFlash  = CreatePen(PS_SOLID, 1, FEEDBACK_RED);
                if (!g_linePenDrop)   g_linePenDrop   = CreatePen(PS_SOLID, 1, FEEDBACK_GREEN);

                HPEN penToUse = (lineColor == FEEDBACK_RED)   ? g_linePenFlash
                              : (lineColor == FEEDBACK_GREEN) ? g_linePenDrop
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
                            if (g_sepDC && g_sepOldBmp) {
                                SelectObject(g_sepDC, g_sepOldBmp);
                                g_sepOldBmp = NULL;
                            }
                            if (g_sepDIB) { DeleteObject(g_sepDIB); g_sepDIB = NULL; }
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
                                g_sepOldBmp = (HBITMAP)SelectObject(g_sepDC, g_sepDIB);
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
        if (usingBackBuffer) PresentPaintBuffer(paintDC, paintW, paintH);
        // Keep secondary monitor docks in sync (cheap InvalidateRect calls only)
        RepaintSecondaryDocks();
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_MOUSEWHEEL: {
        // Scroll-wheel navigation: move the hover highlight across pinned
        // icons. Delivered to this window by Win10+ 'scroll inactive windows
        // on hover'. Wheel down => next (right), wheel up => previous (left).
        if (!ENABLE_SCROLL_NAV || !g_csInitialized) return 0;
        short wheel = GET_WHEEL_DELTA_WPARAM(wParam);

        EnterCriticalSection(&g_cs);
        int wn = (int)g_pinnedApps.size();
        if (wn <= 0) { LeaveCriticalSection(&g_cs); return 0; }

        int cur = (g_hoverIndex >= 0 && g_hoverIndex < wn) ? g_hoverIndex
                                                           : (wheel < 0 ? -1 : wn);
        int nextIdx = cur + (wheel < 0 ? 1 : -1);
        if (nextIdx < 0) nextIdx = 0;
        if (nextIdx > wn - 1) nextIdx = wn - 1;
        bool changed = (nextIdx != g_hoverIndex);
        g_hoverIndex = nextIdx;

        // ---- Viewport paging: keep the highlighted app inside the visible -----
        // window.  When more apps are pinned than MAX_VISIBLE_APP_SLOTS the dock
        // does not grow; instead this slides g_appScrollStart so the wheel walks
        // through every pin.  For <= MAX_VISIBLE_APP_SLOTS apps this is a no-op
        // (all visible) and behaviour is exactly as before.
        int appCount = CountPinsByType(PIN_APP);
        int vis = appCount;
        if (vis > MAX_VISIBLE_APP_SLOTS) vis = MAX_VISIBLE_APP_SLOTS;
        if (vis < 1) vis = 1;
        int maxStart = appCount - vis;
        if (maxStart < 0) maxStart = 0;

        if (g_pinnedApps[nextIdx].type == PIN_APP) {
            int ord = PinOrdinalWithinType(nextIdx, PIN_APP);
            if (ord < g_appScrollStart)                 g_appScrollStart = ord;
            else if (ord > g_appScrollStart + vis - 1)  g_appScrollStart = ord - (vis - 1);
        }
        if (g_appScrollStart > maxStart) g_appScrollStart = maxStart;
        if (g_appScrollStart < 0)        g_appScrollStart = 0;

        // Retarget every icon to its (possibly newly scrolled) slot.  Icons that
        // jump between hidden and visible are snapped so they don't fly across
        // the whole dock; icons shifting within the window animate one slot.
        float snapDist = (float)(2 * (ICON_SIZE + ICON_SPACING));
        for (int i = 0; i < wn; ++i) {
            RECT r = GetIconRectLocal(i, wn);
            float t = (float)r.left;
            if (fabsf(t - g_pinnedApps[i].currentX) > snapDist)
                g_pinnedApps[i].currentX = t;
            g_pinnedApps[i].targetX = t;
        }
        LeaveCriticalSection(&g_cs);

        if (changed) {
            // Arm the scroll-lock so the stray WM_MOUSEMOVE that Windows emits
            // right after a wheel event (cursor is stationary) cannot snap the
            // highlight back to whatever icon is physically under the cursor.
            g_scrollNavUntil = GetTickCount() + SCROLL_NAV_LOCK_MS;
            GetCursorPos(&g_scrollNavPt);
            SetCursor(LoadCursor(NULL, IDC_HAND));
        }
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_MOUSEMOVE: {
        POINT sp;
        GetCursorPos(&sp);
        // While the scroll-lock is active, ignore hover updates until the user
        // physically moves the cursor a real distance (releases the lock).
        if (g_scrollNavUntil && GetTickCount() < g_scrollNavUntil) {
            int dxl = sp.x - g_scrollNavPt.x, dyl = sp.y - g_scrollNavPt.y;
            if (dxl * dxl + dyl * dyl < DRAG_THRESHOLD_PX * DRAG_THRESHOLD_PX) {
                if (!g_mouseTracking) {
                    TRACKMOUSEEVENT tmel = { sizeof(tmel), TME_LEAVE, hwnd, 0 };
                    g_mouseTracking = TrackMouseEvent(&tmel) != FALSE;
                }
                return 0;
            }
        }
        g_scrollNavUntil = 0;
        int idx = HitTestIcon(sp);
        if (idx != g_hoverIndex) {
            g_hoverIndex = idx;
            SetCursor(LoadCursor(NULL, idx >= 0 ? IDC_HAND : IDC_ARROW));
            InvalidateRect(hwnd, NULL, FALSE);
        }
        if (!g_mouseTracking) {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            g_mouseTracking = TrackMouseEvent(&tme) != FALSE;
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        g_mouseTracking = false;
        if (g_hoverIndex != -1) {
            g_hoverIndex = -1;
            InvalidateRect(hwnd, NULL, FALSE);
        }
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

    case WM_RBUTTONDOWN:
        return 0;

    case WM_RBUTTONDBLCLK: {
        // User setting: when double-right-click unpin is turned OFF, ignore the
        // double-click for unpinning and let the normal context menu (shown on
        // WM_RBUTTONUP) be the only right-click behaviour.
        if (!ENABLE_DOUBLE_RCLICK_UNPIN)
            return 0;
        // Locked: refuse the double-right-click unpin gesture (only a deliberate
        // drag-off rope-break can unpin while the dock is locked). Flash the glow.
        if (g_iconsLocked) {
            TriggerLockGlow();
            return 0;
        }
        // Double-right-click a pinned icon = unpin it, with the Thanos icon
        // disintegration. All vanish GDI must run on the worker thread, so we
        // only snapshot the icon + its SCREEN rect here (on the main/UI thread)
        // and hand the request off through g_vanishReq*; the worker loop fires
        // TriggerIconVanish when it picks the request up. Coordinates are
        // published BEFORE the icon handle so the worker never reads a stale
        // rect for a fresh icon (it tests the icon first, then the coords).
        POINT sp;
        GetCursorPos(&sp);
        int idx = HitTestIcon(sp);
        if (idx >= 0) {
            HICON icoCopy = NULL;
            int sx = 0, sy = 0, s = 0;
            if (g_csInitialized) {
                EnterCriticalSection(&g_cs);
                int n = (int)g_pinnedApps.size();
                if (idx < n) {
                    RECT lr = GetIconRectLocal(idx, n);
                    sx = g_cachedDockRect.left + lr.left;
                    sy = g_cachedDockRect.top  + lr.top;
                    s  = lr.right - lr.left;
                    if (g_pinnedApps[idx].icon)
                        icoCopy = CopyIcon(g_pinnedApps[idx].icon);
                }
                LeaveCriticalSection(&g_cs);
            }
            if (icoCopy && s > 0) {
                InterlockedExchange(&g_vanishReqX, sx);
                InterlockedExchange(&g_vanishReqY, sy);
                InterlockedExchange(&g_vanishReqS, s);
                HICON prev = (HICON)InterlockedExchangePointer(
                    (PVOID volatile*)&g_vanishReqIcon, icoCopy);
                if (prev) DestroyIcon(prev);   // drop an unconsumed earlier request
            } else if (icoCopy) {
                DestroyIcon(icoCopy);
            }
            UnpinAppByIndex(idx);
        }
        return 0;
    }

    case WM_RBUTTONUP: {
        POINT sp;
        GetCursorPos(&sp);
        int idx = HitTestIcon(sp);
        if (idx >= 0) ShowPinContextMenu(hwnd, idx, sp);
        else {
            HMENU menu = CreatePopupMenu();
            if (menu) {
                AppendMenuW(menu, MF_STRING | MF_DISABLED, 1, L"Quick Pin Dock");
                TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_NOANIMATION,
                               sp.x, sp.y, 0, hwnd, NULL);
                DestroyMenu(menu);
            }
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
                        // Locked: refuse the hotkey's unpin action (pinning is
                        // still allowed). Only a drag-off rope-break can unpin
                        // while the dock is locked.
                        if (!g_iconsLocked) {
                            UnpinAppByIndex(idx);
                            DragTraceLog(L"HOTKEY: unpin", path.c_str());
                        } else {
                            TriggerLockGlow();   // locked: flash instead of unpinning
                        }
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
LRESULT CALLBACK InputOwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_NCHITTEST:
        return HTCLIENT;

    // The input-owner window is the only non-click-through window over the
    // dock, so it is the window that actually receives wheel/move messages.
    // (The painted overlay is WS_EX_TRANSPARENT and never gets them.)  Scroll
    // navigation and hover magnification are therefore driven from here and
    // repaint the overlay via g_overlayWnd.
    case WM_MOUSEWHEEL: {
        // Scroll-wheel navigation: move the hover highlight across pinned
        // icons. Wheel down => next (right), wheel up => previous (left).
        if (!ENABLE_SCROLL_NAV || !g_csInitialized) return 0;
        short wheel = GET_WHEEL_DELTA_WPARAM(wParam);

        EnterCriticalSection(&g_cs);
        int wn = (int)g_pinnedApps.size();
        if (wn <= 0) { LeaveCriticalSection(&g_cs); return 0; }

        int cur = (g_hoverIndex >= 0 && g_hoverIndex < wn) ? g_hoverIndex
                                                           : (wheel < 0 ? -1 : wn);
        int nextIdx = cur + (wheel < 0 ? 1 : -1);
        if (nextIdx < 0) nextIdx = 0;
        if (nextIdx > wn - 1) nextIdx = wn - 1;
        bool changed = (nextIdx != g_hoverIndex);
        g_hoverIndex = nextIdx;

        // Viewport paging: keep the highlighted app inside the visible window
        // (same logic as the overlay proc; no-op for <= MAX_VISIBLE_APP_SLOTS).
        int appCount = CountPinsByType(PIN_APP);
        int vis = appCount;
        if (vis > MAX_VISIBLE_APP_SLOTS) vis = MAX_VISIBLE_APP_SLOTS;
        if (vis < 1) vis = 1;
        int maxStart = appCount - vis;
        if (maxStart < 0) maxStart = 0;

        if (g_pinnedApps[nextIdx].type == PIN_APP) {
            int ord = PinOrdinalWithinType(nextIdx, PIN_APP);
            if (ord < g_appScrollStart)                 g_appScrollStart = ord;
            else if (ord > g_appScrollStart + vis - 1)  g_appScrollStart = ord - (vis - 1);
        }
        if (g_appScrollStart > maxStart) g_appScrollStart = maxStart;
        if (g_appScrollStart < 0)        g_appScrollStart = 0;

        float snapDist = (float)(2 * (ICON_SIZE + ICON_SPACING));
        for (int i = 0; i < wn; ++i) {
            RECT r = GetIconRectLocal(i, wn);
            float t = (float)r.left;
            if (fabsf(t - g_pinnedApps[i].currentX) > snapDist)
                g_pinnedApps[i].currentX = t;
            g_pinnedApps[i].targetX = t;
        }
        LeaveCriticalSection(&g_cs);

        if (changed) {
            // Arm the scroll-lock (see the primary proc for the rationale).
            g_scrollNavUntil = GetTickCount() + SCROLL_NAV_LOCK_MS;
            GetCursorPos(&g_scrollNavPt);
            SetCursor(LoadCursor(NULL, IDC_HAND));
        }
        if (g_overlayWnd && IsWindow(g_overlayWnd))
            InvalidateRect(g_overlayWnd, NULL, FALSE);
        return 0;
    }

    case WM_MOUSEMOVE: {
        POINT sp;
        GetCursorPos(&sp);
        // Honour the scroll-lock: ignore hover updates until the cursor
        // physically moves a real distance from where the wheel was used.
        if (g_scrollNavUntil && GetTickCount() < g_scrollNavUntil) {
            int dxl = sp.x - g_scrollNavPt.x, dyl = sp.y - g_scrollNavPt.y;
            if (dxl * dxl + dyl * dyl < DRAG_THRESHOLD_PX * DRAG_THRESHOLD_PX) {
                if (!g_mouseTracking) {
                    TRACKMOUSEEVENT tmel = { sizeof(tmel), TME_LEAVE, hwnd, 0 };
                    g_mouseTracking = TrackMouseEvent(&tmel) != FALSE;
                }
                return 0;
            }
        }
        g_scrollNavUntil = 0;
        int idx = HitTestIcon(sp);
        if (idx != g_hoverIndex) {
            g_hoverIndex = idx;
            SetCursor(LoadCursor(NULL, idx >= 0 ? IDC_HAND : IDC_ARROW));
            if (g_overlayWnd && IsWindow(g_overlayWnd))
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }
        if (!g_mouseTracking) {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            g_mouseTracking = TrackMouseEvent(&tme) != FALSE;
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        g_mouseTracking = false;
        if (g_hoverIndex != -1) {
            g_hoverIndex = -1;
            if (g_overlayWnd && IsWindow(g_overlayWnd))
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }
        return 0;

    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        return 0;
    case WM_RBUTTONUP: {
        POINT sp;
        GetCursorPos(&sp);
        int idx = HitTestIcon(sp);
        if (idx >= 0) ShowPinContextMenu(hwnd, idx, sp);
        else {
            HMENU menu = CreatePopupMenu();
            if (menu) {
                AppendMenuW(menu, MF_STRING | MF_DISABLED, 1, L"Quick Pin Dock");
                TrackPopupMenu(menu, TPM_RIGHTBUTTON | TPM_NOANIMATION,
                               sp.x, sp.y, 0, hwnd, NULL);
                DestroyMenu(menu);
            }
        }
        return 0;
    }
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

static bool CreateInputOwnerWindow() {
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = InputOwnerProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = INPUT_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);

    int initialW = g_dockLocalW > 0 ? g_dockLocalW : 200;
    int initialH = g_dockLocalH > 0 ? g_dockLocalH : 48;
    g_inputWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        INPUT_CLASS, L"",
        WS_POPUP,
        0, 0, initialW, initialH,
        NULL, NULL, wc.hInstance, NULL);

    if (!g_inputWnd) {
        LOG_ERROR(L"INPUT: CreateWindow failed err=%lu", GetLastError());
        return false;
    }
    SetLayeredWindowAttributes(g_inputWnd, 0, 1, LWA_ALPHA);
    return true;
}

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

    g_overlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        OVERLAY_CLASS, L"",
        WS_POPUP,
        0, 0, initialW, initialH,
        NULL, NULL, wc.hInstance, NULL);

    if (!g_overlayWnd) {
        LOG_ERROR(L"OVERLAY: CreateWindow failed err=%lu", GetLastError());
        return false;
    }

    // Colour-key for transparency: RGB(1,0,1) magenta.
    // Previously RGB(0,0,0) black was used, but the glass gradient paints
    // near-black values that got incorrectly keyed out -- making the dock
    // appear as a solid black rectangle instead of transparent.
    // RGB(1,0,1) is never produced by the glass gradient or icon rendering,
    // so only the true background pixels become transparent.
    SetLayeredWindowAttributes(g_overlayWnd, RGB(1, 0, 1), 0, LWA_COLORKEY);
    ApplyNativeBackdrop(g_overlayWnd);

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
        LOG_ERROR(L"GHOST: CreateWindow failed err=%lu", GetLastError());
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
// ------------------------------------------------------------
//  KEYBOARD-GESTURE HELPERS  --  explicit pin / unpin of the focused app.
//  Shared by the triple-tap P (pin) and triple-tap U (unpin) gestures polled
//  in the worker loop. They reuse the SAME validation pipeline as the drag and
//  the Ctrl+Alt+P hotkey (never a raw active-window action): system windows and
//  excluded apps are rejected. Safe on the worker thread -- PinApp /
//  UnpinAppByIndex are already invoked from here on drop.
static void PinForegroundApp() {
    HWND fg = GetForegroundWindow();
    if (!fg || IsSystemWindow(fg)) return;
    std::wstring path = GetProcessPath(fg);
    if (path.empty() || IsExcludedApp(path)) return;
    bool pinned = false;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        pinned = IsPinned(path);
        LeaveCriticalSection(&g_cs);
    }
    if (!pinned) { PinApp(path); DragTraceLog(L"KEY-TAP: pin", path.c_str()); }
}

static void UnpinForegroundApp() {
    HWND fg = GetForegroundWindow();
    if (!fg || IsSystemWindow(fg)) return;
    std::wstring path = GetProcessPath(fg);
    if (path.empty()) return;
    int idx = -1;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        for (int i = 0; i < (int)g_pinnedApps.size(); ++i)
            if (_wcsicmp(g_pinnedApps[i].exePath.c_str(), path.c_str()) == 0) { idx = i; break; }
        LeaveCriticalSection(&g_cs);
    }
    if (idx >= 0) { UnpinAppByIndex(idx); DragTraceLog(L"KEY-TAP: unpin", path.c_str()); }
}

DWORD WINAPI WorkerThread(LPVOID) {
    bool  lastLDown         = false;
    DWORD lastGeometryCheck = GetTickCount();
    DWORD bootWatchdogStart = GetTickCount();

    while (WaitForSingleObject(g_exitEvent, 0) != WAIT_OBJECT_0) {
        if (!g_csInitialized) { WaitForSingleObject(g_exitEvent, 16); continue; }

        POINT cursor = {};
        GetCursorPos(&cursor);
        DWORD now = GetTickCount();

        // Safety geometry poll + running-state + auto-hide check.
        // Poll every 100 ms while still in boot/stabilizing so the dock becomes
        // visible within one poll cycle after Wh_ModInit.  Once geometry is locked
        // (STATE_STABLE), drop to a relaxed 500 ms cadence to save CPU.
        DWORD pollIntervalMs = (g_systemState != STATE_STABLE) ? 100u : (DWORD)RUNNING_STATE_CHECK_MS;
        if (now - lastGeometryCheck > pollIntervalMs || g_dockWidthDirty) {
            bool needRefresh = (g_systemState != STATE_STABLE) || HasTaskbarGeometryChanged() || g_dockWidthDirty;
            if (needRefresh) {
                RefreshTaskbarCache();
                RepositionOverlay();
                if (MULTI_MONITOR_DOCK) {
                    // FIX: secondary dock windows were created on the main thread;
                    // DestroyWindow from the worker thread is undefined behaviour.
                    // Post to the overlay wndproc to rebuild on the correct thread.
                    // Rate-limit to once every 2 s (matching the WinEventProc path):
                    // during boot/stabilization this poll fires every 100 ms and,
                    // without a guard, tore down and recreated every secondary-monitor
                    // dock on each cycle -- visible flicker + churn on multi-mon setups.
                    static DWORD s_lastSecondaryRebuildWk = 0;
                    if (now - s_lastSecondaryRebuildWk >= 2000) {
                        s_lastSecondaryRebuildWk = now;
                        if (g_overlayWnd && IsWindow(g_overlayWnd))
                            PostMessageW(g_overlayWnd, WM_QPD_REBUILD_SECONDARY, 0, 0);
                    }
                }
            }
            if (ENABLE_AUTOHIDE_SYNC) UpdateAutoHideState();
            UpdateRunningState();
            if (g_overlayWnd) InvalidateRect(g_overlayWnd, NULL, FALSE);
            lastGeometryCheck = now;
        }

        // -- Hover stability tracking (adaptive) -------------------------------
        // Pre-samples the resolver so PRESS uses a temporally stable identity
        // rather than resolving cold at click time.  ResolveDragSourceAtPoint
        // performs a cross-process UI Automation hit-test (several ms and can
        // briefly block the input queue), so we only pay for it when it can
        // actually change the result:
        //   * the cursor has physically moved since the last sample, OR
        //   * we do not yet have a candidate.
        // A stationary cursor over the same target no longer spams UIA every
        // HOVER_SAMPLE_MS, removing the biggest source of idle input latency.
        static POINT s_lastSamplePt = { -100000, -100000 };
        if (g_dragState == DRAG_IDLE &&
            now - g_hoverLastSample >= (DWORD)HOVER_SAMPLE_MS &&
            (IsInDockZone(cursor, 30) || IsCursorOverTaskbar(cursor))) {
            int moved = abs(cursor.x - s_lastSamplePt.x) + abs(cursor.y - s_lastSamplePt.y);
            if (moved >= 3 || g_hoverCandidate.empty()) {
                std::wstring fresh = ResolveDragSourceAtPoint(cursor);
                if (_wcsicmp(fresh.c_str(), g_hoverCandidate.c_str()) != 0) {
                    g_hoverCandidate     = fresh;
                    g_hoverCandidateTime = now;
                }
                s_lastSamplePt = cursor;
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

        if (!g_overlayWnd || !IsWindow(g_overlayWnd)) { WaitForSingleObject(g_exitEvent, 16); continue; }

        // Re-assert HWND_TOPMOST every 3 s  --  avoids hammering DWM every frame.
        // Per-frame SetWindowPos was the single largest source of input jitter.
        {
            static DWORD s_lastTopMostMs = 0;
            if (now - s_lastTopMostMs > 3000) {
                if (g_inputWnd && IsWindow(g_inputWnd))
                    SetWindowPos(g_inputWnd, HWND_TOPMOST, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                SetWindowPos(g_overlayWnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                s_lastTopMostMs = now;
            }
        }

        bool lDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        // ================================================================
        //  KEYBOARD TRIPLE-TAP GESTURE  --  tap P x3 = pin, tap U x3 = unpin
        // ================================================================
        // Rising-edge count of the bare P / U keys. Three taps inside
        // KEY_TAP_WINDOW_MS pin / unpin the focused app through the same
        // validated pipeline as the drag and the Ctrl+Alt+P hotkey. A held key
        // counts as one tap (edge-detected), and the counter resets when taps
        // are too slow so it does not fire during ordinary typing.
        {
            bool pDown    = (GetAsyncKeyState('P') & 0x8000) != 0;
            bool uDown    = (GetAsyncKeyState('U') & 0x8000) != 0;
            bool lKeyDown = (GetAsyncKeyState('L') & 0x8000) != 0;  // triple-tap = lock toggle
            if (pDown && !g_pWasDown) {
                if (g_tapCountP == 0 || (now - g_tapStartP) > (DWORD)KEY_TAP_WINDOW_MS) {
                    g_tapCountP = 1; g_tapStartP = now;
                } else {
                    g_tapCountP++;
                }
                if (g_tapCountP >= KEY_TAP_THRESHOLD) {
                    PinForegroundApp();
                    g_tapCountP = 0; g_tapStartP = 0;
                }
            }
            if (uDown && !g_uWasDown) {
                if (g_tapCountU == 0 || (now - g_tapStartU) > (DWORD)KEY_TAP_WINDOW_MS) {
                    g_tapCountU = 1; g_tapStartU = now;
                } else {
                    g_tapCountU++;
                }
                if (g_tapCountU >= KEY_TAP_THRESHOLD) {
                    // Locked: refuse the triple-tap-U gesture unpin (the whole
                    // point of the lock) and flash the gold lock glow instead.
                    // Still consume the count so it can't fire right after unlock.
                    if (!g_iconsLocked) UnpinForegroundApp();
                    else                TriggerLockGlow();
                    g_tapCountU = 0; g_tapStartU = 0;
                }
            }
            // Triple-tap L -> toggle the icon lock (see g_iconsLocked). Same
            // rising-edge / KEY_TAP_WINDOW_MS logic as P and U.
            if (lKeyDown && !g_lWasDown) {
                if (g_tapCountL == 0 || (now - g_tapStartL) > (DWORD)KEY_TAP_WINDOW_MS) {
                    g_tapCountL = 1; g_tapStartL = now;
                } else {
                    g_tapCountL++;
                }
                if (g_tapCountL >= KEY_TAP_THRESHOLD) {
                    g_iconsLocked = !g_iconsLocked;
                    LOG_IMPORTANT(L"ICON LOCK: %s", g_iconsLocked ? L"LOCKED" : L"UNLOCKED");
                    TriggerLockGlow();   // brief gold confirmation flash on lock & unlock
                    g_tapCountL = 0; g_tapStartL = 0;
                }
            }
            g_pWasDown = pDown;
            g_uWasDown = uDown;
            g_lWasDown = lKeyDown;
        }

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
            g_draggedPinType = PIN_APP;
            g_lockedDragPinType = PIN_APP;
            g_draggedExplorerHwnd = NULL;
            g_lockedExplorerHwnd = NULL;
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
                PinType      snapType     = PIN_APP;
                HICON        snapIconCopy = NULL;
                bool         validSnap    = false;

                EnterCriticalSection(&g_cs);
                if (dockIdx < (int)g_pinnedApps.size()) {
                    snapType = g_pinnedApps[dockIdx].type;
                    snapPath = (snapType == PIN_WORKSPACE)
                             ? g_pinnedApps[dockIdx].workspaceId
                             : g_pinnedApps[dockIdx].exePath;
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
                    g_draggedPinType  = snapType;
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
                HWND workspaceHwnd = NULL;
                bool workspaceSource = ENABLE_EXPLORER_WORKSPACE_PINS &&
                                       IsExplorerWorkspaceDragSource(cursor, &workspaceHwnd);
                std::wstring path;
                if (workspaceSource) {
                    path = L"Explorer workspace";
                    g_draggedPinType = PIN_WORKSPACE;
                    g_draggedExplorerHwnd = workspaceHwnd;
                    DragTraceLog(L"PRESS: Explorer workspace");
                } else {
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
                    g_draggedPinType = PIN_APP;
                }
                if (path.empty()) {
                    g_dragState = DRAG_CANCELLED;
                    DragTraceLog(L"REJECT: no valid source");
                } else {
                    g_draggedAppPath = path;
                    HICON tmp = workspaceSource ? LoadFolderIcon(L"C:\\") : LoadAppIconStrict(path);
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
                    g_lockedDragPinType = g_draggedPinType;
                    g_lockedExplorerHwnd = g_draggedExplorerHwnd;
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

            // If cursor leaves dock zone -> abort reorder, fall through to unpin drag.
            // FIX (rope never showed): the old threshold was MAGNETIC_RANGE_PX * 3
            // (~180px), so a normal pull-off stayed in DRAG_REORDER the whole time
            // and the tug-of-war rope (only pumped in DRAG_DRAGGING) never rendered
            // -- you had to fling the icon ~180px away to see it. Convert to an
            // unpin drag as soon as the icon is pulled clearly off the dock (a small
            // hysteresis past the dock edge). Horizontal reorder keeps the cursor
            // inside the dock rect so this doesn't disturb reordering, while a
            // deliberate pull-away now unpins AND shows the rope immediately.
            // Use the SAME predicate the rope uses for its visibility
            // (IsNearDockZone / g_dropZoneActive): the instant the icon leaves
            // the magnetic zone the state flips to DRAG_DRAGGING and the
            // tug-of-war rope is pumped on that very frame -- so pulling an icon
            // off the dock always shows the rope, with no dead-zone gap.
            if (!IsNearDockZone(cursor)) {
                g_dragState      = DRAG_DRAGGING;
                g_lockedDragPath = g_draggedAppPath;
                g_lockedDragPinType = g_draggedPinType;
                g_lockedExplorerHwnd = g_draggedExplorerHwnd;
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
            // Present the rope FIRST, then the ghost icon. Both re-assert
            // HWND_TOPMOST every present, so the one presented LAST ends up on
            // top of the top-most band. Drawing the icon last keeps it cleanly
            // above the rope tip (the rope appears to attach behind the icon)
            // while both remain above every other window.
            UpdateTetherWindow(cursor);   // rope: visible for the WHOLE drag (taskbar + windows)
            UpdateGhostWindow(cursor);    // ghost icon: taskbar region only (by design)
            if (g_dragFromDock && g_overlayWnd)
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        } else if (g_tetherBreaking || g_tetherRetracting) {
            // The drag/drop has already ended but a rope animation (mid-rope
            // SNAP or spring-home RETRACT) is still in flight. Keep pumping it
            // to completion so a fast drag+release can never abort it midway
            // (root cause of "the rope can't survive a fast release"). Both
            // animations are self-contained inside UpdateTetherWindow and clean
            // themselves up exactly once when finished.
            UpdateTetherWindow(cursor);
            if (g_overlayWnd)
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }

        // Consume a cross-thread vanish request (posted by the main thread's
        // double-right-click unpin handler). All vanish GDI must live on this
        // worker thread, so the main thread only hands off an icon + screen
        // rect via the g_vanishReq* fields and we fire the effect here.
        {
            HICON reqIcon = (HICON)InterlockedExchangePointer(
                (PVOID volatile*)&g_vanishReqIcon, NULL);
            if (reqIcon) {
                int rqx = (int)InterlockedExchange(&g_vanishReqX, 0);
                int rqy = (int)InterlockedExchange(&g_vanishReqY, 0);
                int rqs = (int)InterlockedExchange(&g_vanishReqS, 0);
                if (rqs > 0) TriggerIconVanish(reqIcon, rqx, rqy, rqs);
                DestroyIcon(reqIcon);   // we own the caller's CopyIcon handle
            }
        }

        // Pump the icon "Thanos" disintegration to completion, independent of
        // drag state -- it is triggered on the off-dock unpin release handled
        // below and must keep animating after the drag has fully ended.
        if (g_vanishActive) {
            UpdateVanishWindow();
            if (g_overlayWnd)
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }

        // DRAGGING -> DROPPED / CANCELLED (mouse up)
        if ((g_dragState == DRAG_DRAGGING || g_dragState == DRAG_CANCELLED) &&
            !lDown && lastLDown) {

            // Did the user let go of a dock icon OUTSIDE the dock (an unpin)?
            // Drives the tug-of-war outcome below: off-dock release -> the rope
            // TEARS in the middle; on-dock release -> it relaxes / retracts.
            bool  releasedOffDockUnpin = false;
            HICON vanishIcon = NULL;   // icon snapshot for the disintegration (freed after triggering)

            if (g_dragState == DRAG_DRAGGING) {
                // Evaluate drop zone using authoritative static rect (not animated position)
                RECT dropZone = g_cachedDockRect;
                InflateRect(&dropZone, 8, 8);
                bool dropped = PtInRect(&dropZone, cursor) != 0;

                if (g_dragFromDock) {
                    int lockedIdx = g_dragFromDockIdx;
                    g_dragState   = DRAG_DROPPED;
                    // UNPIN_TRIGGER == 0 (default): releasing off the dock does NOT
                    // unpin -- the rope recoils home and the icon stays pinned; only
                    // an actual rope BREAK (handled mid-drag in UpdateTetherWindow)
                    // removes it. UNPIN_TRIGGER == 1: releasing off the dock unpins too.
                    if (!dropped && UNPIN_TRIGGER == 1) {
                        DragTraceLog(L"DROP: off dock (mode 1) -> UNPIN");
                        releasedOffDockUnpin = true;   // rope must TEAR in the middle
                        // Snapshot the icon for the disintegration BEFORE unpinning
                        // (the drag-ghost icon can be NULL for some dock icons, which
                        // is why the vanish sometimes never appeared). The pinned
                        // app's own icon is always valid here.
                        if (g_csInitialized) {
                            EnterCriticalSection(&g_cs);
                            if (lockedIdx >= 0 && lockedIdx < (int)g_pinnedApps.size() &&
                                g_pinnedApps[lockedIdx].icon)
                                vanishIcon = CopyIcon(g_pinnedApps[lockedIdx].icon);
                            LeaveCriticalSection(&g_cs);
                        }
                        UnpinAppByIndex(lockedIdx);
                    } else {
                        DragTraceLog(L"DROP: back on dock -> no change");
                    }
                } else {
                    std::wstring dropPath = !g_lockedDragPath.empty()
                                         ? g_lockedDragPath : g_draggedAppPath;
                    PinType dropType = !g_lockedDragPath.empty()
                                     ? g_lockedDragPinType : g_draggedPinType;
                    HWND explorerHwnd = g_lockedExplorerHwnd ? g_lockedExplorerHwnd : g_draggedExplorerHwnd;
                    g_dragState = DRAG_DROPPED;
                    if (dropped && dropType == PIN_WORKSPACE) {
                        DragTraceLog(L"DROP: in zone -> PIN WORKSPACE");
                        PinWorkspace(explorerHwnd);
                    } else if (dropped && !dropPath.empty()) {
                        DragTraceLog(L"DROP: in zone -> PIN", dropPath.c_str());
                        PinApp(dropPath);
                    } else {
                        DragTraceLog(L"DROP: outside zone -> cancel");
                    }
                }
            }

            ShowWindow(g_ghostWnd, SW_HIDE);
            // FIX (rope must SURVIVE a fast drag+release): previously this hard-
            // called HideDragTether() the instant the button came up, so on a
            // quick yank the rope simply blinked out -- the mid-rope snap never
            // played and it "felt under development". Now the release DECIDES
            // the tug-of-war outcome and hands the rope to a self-contained
            // animation that the worker loop pumps to completion (see the
            // `else if (g_tetherBreaking || g_tetherRetracting)` pump below):
            //   * let go OFF the dock (an unpin)  -> TEAR in the middle
            //   * let go back ON the dock          -> relax / retract home
            // Only when there is genuinely nothing on screen do we hard-hide.
            if (releasedOffDockUnpin) {
                // ALWAYS TEAR on an off-dock unpin -- even on a FAST yank where
                // the rope never got to render a single frame (g_tetherWasVisible
                // false). In that case the frozen last-anchor/tip are stale, so
                // synthesise them from the dock rect + the release point. This is
                // the root-cause fix for "rope breaking is not happening" on a
                // quick drag-and-release. The snap plays to completion via the
                // g_tetherBreaking block, pumped by the worker loop below.
                float bsx, bsy, btx, bty;
                if (g_tetherWasVisible && g_tetherBreakFade > 0.02f) {
                    bsx = g_tetherLastSrcX; bsy = g_tetherLastSrcY;
                    btx = g_tetherTipX;     bty = g_tetherTipY;
                } else {
                    bsx = ((float)g_cachedDockRect.left + (float)g_cachedDockRect.right) * 0.5f;
                    bsy = ((float)cursor.y >= (float)g_cachedDockRect.bottom)
                              ? (float)g_cachedDockRect.bottom
                              : (float)g_cachedDockRect.top;
                    btx = (float)cursor.x;  bty = (float)cursor.y;
                }
                g_tetherBreakSrcX = bsx; g_tetherBreakSrcY = bsy;
                g_tetherBreakTipX = btx; g_tetherBreakTipY = bty;
                g_tetherBreakFade  = 1.f;
                g_tetherWasVisible = true;
                g_tetherRetracting = false;
                g_tetherBreaking   = true;
            } else if (g_tetherWasVisible && !g_tetherBreaking && g_tetherBreakFade > 0.02f) {
                // Settled back onto the dock: spring the tip home and fade.
                g_tetherRetracting = true;
            } else if (!g_tetherBreaking && !g_tetherRetracting) {
                HideDragTether();
            }

            // ICON "THANOS" VANISH: the icon the user just yanked off the dock
            // crumbles into drifting dust at the point it was let go. Rasterise
            // it NOW, before GhostCleanup() destroys the drag-ghost icon. Purely
            // decorative -- the real unpin already happened above.
            if (releasedOffDockUnpin) {
                HICON srcIcon = vanishIcon ? vanishIcon : g_dragGhostIcon;
                if (srcIcon) {
                    TriggerIconVanish(srcIcon,
                                      cursor.x - GHOST_SIZE / 2,
                                      cursor.y - GHOST_SIZE / 2 - 6,
                                      GHOST_SIZE);
                }
            }
            if (vanishIcon) { DestroyIcon(vanishIcon); vanishIcon = NULL; }

            g_lockedDragPath.clear();
            g_lockedExplorerHwnd = NULL;
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
                    if (g_rapidClickCount >= RAPID_CLICK_THRESHOLD && g_iconsLocked) {
                        // Locked: refuse the rapid-click unpin-all gesture, flash
                        // the lock glow, and clear the counter so it can't fire
                        // right after unlock.
                        TriggerLockGlow();
                        g_rapidClickCount = 0;
                        g_rapidClickStart = 0;
                        g_rapidClickIndex = -1;
                    }
                    else if (g_rapidClickCount >= RAPID_CLICK_THRESHOLD) {
                        // Disintegrate EVERY pinned icon at once before they are
                        // removed. Snapshot each icon + its screen rect under the
                        // lock, then rasterise/append them all into one vanish so
                        // the whole dock turns to dust in a single sweep. We run on
                        // the worker thread here, so it is safe to touch the vanish
                        // GDI directly (no cross-thread request needed).
                        struct VShot { HICON ic; int x, y, s; };
                        std::vector<VShot> shots;
                        if (g_csInitialized) {
                            EnterCriticalSection(&g_cs);
                            int n = (int)g_pinnedApps.size();
                            for (int i = 0; i < n; ++i) {
                                if (!g_pinnedApps[i].icon) continue;
                                RECT lr = GetIconRectLocal(i, n);
                                int s = lr.right - lr.left;
                                if (s <= 0) continue;
                                HICON ic = CopyIcon(g_pinnedApps[i].icon);
                                if (!ic) continue;
                                shots.push_back({ ic,
                                                  g_cachedDockRect.left + lr.left,
                                                  g_cachedDockRect.top  + lr.top,
                                                  s });
                            }
                            LeaveCriticalSection(&g_cs);
                        }
                        if (!shots.empty()) {
                            BeginVanish();
                            for (auto& sh : shots) {
                                AddIconVanish(sh.ic, sh.x, sh.y, sh.s);
                                DestroyIcon(sh.ic);
                            }
                            CommitVanish();
                        }

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
        g_frameDeltaMs = QpcFrameDeltaMs();  // sub-ms delta (was GetTickCount ~15ms quantised)
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

                // Opacity fade  --  bidirectional.
                // Visible icons fade IN (this also drives the new-pin slide+fade
                // entrance, unchanged). Overflow icons parked off either edge by
                // GetIconRectLocal (targetX fully outside [0, dockLocalW)) fade
                // OUT as they glide away, so the >10-pin scroll hide/reveal reads
                // as a soft symmetrical dissolve instead of a hard pop. Detecting
                // "hidden" from the parked targetX keeps this in lock-step with
                // the geometry without recomputing the scroll window here.
                bool  iconHidden    = (app.targetX + (float)ICON_SIZE <= 0.f) ||
                                      (app.targetX >= (float)g_dockLocalW);
                float targetOpacity = iconHidden ? 0.f : 1.f;
                if (snapAnimations) {
                    if (app.opacity != targetOpacity) animActive = true;
                    app.opacity = targetOpacity;
                } else if (app.opacity < targetOpacity) {
                    app.opacity += PIN_FADE_SPEED * (g_frameDeltaMs / 16.f);
                    if (app.opacity > targetOpacity) app.opacity = targetOpacity;
                    animActive = true;
                } else if (app.opacity > targetOpacity) {
                    app.opacity -= ICON_HIDE_FADE_SPEED * (g_frameDeltaMs / 16.f);
                    if (app.opacity < targetOpacity) app.opacity = targetOpacity;
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
                    app.velocityX = (app.velocityX + diff * tf) * powf(ANIM_MOMENTUM_DECAY, g_frameDeltaMs / 16.0f);
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
                // macOS-dock magnification wave: the hovered icon scales up
                // the most and its neighbours scale progressively less, for
                // an Apple-like interactive feel. Because scroll-wheel nav
                // moves g_hoverIndex, this doubles as the scroll highlight.
                //
                // FEEL UPGRADE (borrowed from the reference XAML mod, math only):
                // magnification is now driven by the CONTINUOUS cursor pixel-X
                // distance to each icon's centre (a smooth cosine bell over
                // MAG_RADIUS_PX), instead of the old integer index distance.
                // This gives the authentic "glide between icons" the macOS dock
                // has -- the peak of the wave slides fluidly with the cursor
                // rather than snapping icon-to-icon. When the highlight is being
                // driven by the scroll wheel (no real hover), we centre the
                // falloff on the scroll-selected icon's centre so scroll nav
                // still magnifies exactly as before.
                float targetScale = 1.0f;
                if (g_hoverIndex >= 0 && !isBeingDragged && app.opacity >= 0.99f) {
                    // Each icon's centre in overlay-local space. currentX is the
                    // animated overlay-local left edge, so centre = currentX + half.
                    float iconCenterX = app.currentX + ICON_SIZE * 0.5f;

                    // Focus X in overlay-local coordinates. Prefer the REAL cursor
                    // position for the authentic glide; the cursor screen pos is
                    // mapped to overlay-local exactly like HitTestIcon/CalculateReorderSlot
                    // do (subtract the animated dock left edge g_dockCurrentX).
                    // If the highlight is currently held by the scroll wheel (the
                    // scroll-lock is armed), the cursor is stationary and does not
                    // reflect the selected icon -- in that case we centre the
                    // falloff on the scroll-selected icon's own centre instead.
                    float focusX;
                    bool  scrollDriven = (g_scrollNavUntil != 0 &&
                                          GetTickCount() < g_scrollNavUntil);
                    if (scrollDriven &&
                        g_hoverIndex >= 0 && g_hoverIndex < n) {
                        focusX = g_pinnedApps[g_hoverIndex].currentX + ICON_SIZE * 0.5f;
                    } else {
                        POINT cp; GetCursorPos(&cp);
                        focusX = (float)cp.x - g_dockCurrentX;
                    }

                    float dist = fabsf(focusX - iconCenterX);
                    if (dist <= MAG_RADIUS_PX) {
                        // Raised-cosine bell: 1.0 at the focus -> 0.0 at the
                        // radius, matching the reference CalculateScale() cosine.
                        float t = dist / MAG_RADIUS_PX;             // 0..1
                        float w = 0.5f * (1.0f + cosf(3.14159265f * t));
                        targetScale = 1.0f + (HOVER_SCALE_FACTOR - 1.0f) * w;
                    }
                }
                float scaleDiff   = targetScale - app.hoverScale;
                if (fabsf(scaleDiff) > 0.002f) {
                    float spd = (scaleDiff > 0.f) ? HOVER_SCALE_IN_SPEED : HOVER_SCALE_OUT_SPEED;
                    float tf  = std::min((g_frameDeltaMs / 16.0f) * spd, 0.8f);
                    app.hoverScale += scaleDiff * tf;
                    animActive = true;
                } else {
                    app.hoverScale = targetScale;
                }

                // ---- Neighbour SPREAD (macOS-dock feel, adapted to GDI) ----
                // Reference math: each icon shifts by (cumulativeShift + selfShift
                //   - centerOffset), so magnified icons push their neighbours
                // outward while the whole row stays visually centred. We adapt
                // this to a per-icon horizontal offset: an icon's size-gain
                // (hoverScale-1)*ICON_SIZE spreads half to each side, so icons on
                // the left of the focus slide left and icons on the right slide
                // right, keeping magnified icons from overlapping. hoverShiftX is
                // eased frame-rate-independently just like hoverScale.
                float targetShiftX = 0.f;
                if (g_hoverIndex >= 0 && !isBeingDragged && app.opacity >= 0.99f) {
                    // Focus centre (same convention as the scale block above).
                    float focusX;
                    bool  scrollDriven = (g_scrollNavUntil != 0 &&
                                          GetTickCount() < g_scrollNavUntil);
                    if (scrollDriven && g_hoverIndex >= 0 && g_hoverIndex < n) {
                        focusX = g_pinnedApps[g_hoverIndex].currentX + ICON_SIZE * 0.5f;
                    } else {
                        POINT cp; GetCursorPos(&cp);
                        focusX = (float)cp.x - g_dockCurrentX;
                    }
                    float iconCenterX = app.currentX + ICON_SIZE * 0.5f;
                    // Magnitude of this icon's widening (px), spread outward.
                    float gain = (app.hoverScale - 1.0f) * (float)ICON_SIZE * MAG_SPREAD_FACTOR;
                    // Push away from the focus: sign by side of the focus.
                    if (iconCenterX < focusX)      targetShiftX = -gain;
                    else if (iconCenterX > focusX) targetShiftX =  gain;
                    else                           targetShiftX =  0.f; // focused icon itself: no shift
                }
                float shiftDiff = targetShiftX - app.hoverShiftX;
                if (fabsf(shiftDiff) > 0.05f) {
                    float tf = std::min((g_frameDeltaMs / 16.0f) * HOVER_SHIFT_SPEED, 0.8f);
                    app.hoverShiftX += shiftDiff * tf;
                    animActive = true;
                } else {
                    app.hoverShiftX = targetShiftX;
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

        // Lock glow: repaint ONLY while an interactive flash is in flight so the
        // gold edge animates its bump then stops. Nothing runs when the dock is
        // simply locked-and-idle, so a locked dock costs zero extra CPU at rest.
        if (g_lockGlowStart != 0 && g_overlayWnd && IsWindow(g_overlayWnd)) {
            if (now - g_lockGlowStart >= (DWORD)LOCK_GLOW_MS) {
                g_lockGlowStart = 0;
                InvalidateRect(g_overlayWnd, NULL, FALSE);   // one final clear
            } else {
                static DWORD s_lastLockPulse = 0;
                if (now - s_lastLockPulse >= 33) {           // ~30 fps
                    s_lastLockPulse = now;
                    InvalidateRect(g_overlayWnd, NULL, FALSE);
                }
            }
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
        // NOTE: sleep on g_exitEvent instead of Sleep() so teardown is instant.
        // A plain Sleep(50) here delayed Wh_ModUninit's worker-join by up to a
        // full frame (plus any in-flight scan), which blocked Windhawk during a
        // recompile/reload and made the first "Compile" click(s) appear to do
        // nothing. WaitForSingleObject returns immediately once g_exitEvent is
        // signalled, so the thread exits within ~1 ms of teardown starting.
        //
        // BOOT: never fall to the slow 50 ms idle cadence until geometry is
        // locked (STATE_STABLE). Otherwise the worker "goes to sleep" mid-init
        // -- g_idleFrames climbs past 10 while still in STATE_BOOT/STABILIZING,
        // the loop drops to 50 ms polls, and the dock stalls before it has
        // reached a stable width and painted its first correct frame. Staying at
        // the fast cadence during boot lets initialisation run straight through
        // to completion (boot is only a few hundred ms, so no steady-state cost).
        bool notStableYet = (g_systemState != STATE_STABLE);
        if (g_dragState == DRAG_DRAGGING || g_dragState == DRAG_REORDER ||
            g_anyAnimationActive || g_dockPosAnimActive) {
            g_idleFrames = 0;
            SetHighResTimer(true);
            WaitForSingleObject(g_exitEvent, 8);
        } else if (g_idleFrames < 10 || notStableYet) {
            if (!notStableYet) g_idleFrames++;   // don't accrue idle frames during boot
            WaitForSingleObject(g_exitEvent, 16);
        } else {
            SetHighResTimer(false);
            WaitForSingleObject(g_exitEvent, 50);
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
// PID -> image-path cache.  A process's image path is fixed for its lifetime,
// so caching lets repeated running-state scans skip OpenProcess/QueryImageName
// for PIDs already resolved.  The whole cache is flushed periodically so a
// recycled PID cannot keep a stale path indefinitely.
struct PidPathCacheEntry { DWORD pid; DWORD lastSeenGen; std::wstring path; };
static std::vector<PidPathCacheEntry> g_pidPathCache;
static DWORD g_pidCacheGen = 0;

// Resolve a PID to its executable path, using (and populating) the cache.
// Returns an empty string if the process cannot be opened/queried.
static std::wstring ResolvePidPath(DWORD pid) {
    for (auto& e : g_pidPathCache) {
        if (e.pid == pid) { e.lastSeenGen = g_pidCacheGen; return e.path; }
    }
    std::wstring path;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProc) {
        wchar_t buf[MAX_PATH];
        DWORD len = MAX_PATH;
        // FIX: the previous code ignored the return value and pre-set len to
        // MAX_PATH, so a FAILED query still left len != 0 and counted a stale
        // buffer as a valid path.  Check the BOOL result explicitly.
        if (QueryFullProcessImageNameW(hProc, 0, buf, &len) && len > 0)
            path.assign(buf, len);
        CloseHandle(hProc);
    }
    PidPathCacheEntry e; e.pid = pid; e.lastSeenGen = g_pidCacheGen; e.path = path;
    g_pidPathCache.push_back(e);
    return path;
}

// Collects the unique PIDs of candidate top-level windows.  De-duplicating here
// means many windows of the same process no longer exhaust the scan nor trigger
// repeated OpenProcess calls (the old flat-path buffer capped at 128 entries and
// silently truncated on busy systems, making apps wrongly show as "not running").
struct RunStateCtx {
    std::vector<DWORD>* pids;
};

static BOOL CALLBACK RunStateEnumProc(HWND hwnd, LPARAM lp) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    // Skip tool windows  --  they are not user-facing app windows
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
    // Skip windows with no title (background/service windows)
    if (GetWindowTextLengthW(hwnd) == 0) return TRUE;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return TRUE;

    std::vector<DWORD>* pids = ((RunStateCtx*)lp)->pids;
    for (DWORD existing : *pids)
        if (existing == pid) return TRUE;   // already recorded this process
    pids->push_back(pid);
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
    // 1) Enumerate candidate windows -> unique PIDs (no per-window OpenProcess).
    std::vector<DWORD> pids;
    pids.reserve(64);
    RunStateCtx ctx;
    ctx.pids = &pids;
    EnumWindows(RunStateEnumProc, (LPARAM)&ctx);

    // 2) New scan generation.  Flush the whole cache every 60 scans so a recycled
    //    PID cannot hold a stale path forever (bounded staleness).
    g_pidCacheGen++;
    if ((g_pidCacheGen % 60) == 0) g_pidPathCache.clear();

    // 3) Resolve each unique PID to a path (cached).  Copy into a local list so
    //    later cache growth cannot invalidate the strings we match against.
    std::vector<std::wstring> runningPaths;
    runningPaths.reserve(pids.size());
    for (DWORD pid : pids) {
        std::wstring p = ResolvePidPath(pid);
        if (!p.empty()) runningPaths.push_back(std::move(p));
    }

    // 4) Match pinned apps against the resolved running paths.
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        for (auto& app : g_pinnedApps) {
            app.running = false;
            if (app.type != PIN_APP) continue;
            for (const std::wstring& rp : runningPaths) {
                if (_wcsicmp(app.exePath.c_str(), rp.c_str()) == 0) {
                    app.running = true;
                    break;
                }
            }
        }
        LeaveCriticalSection(&g_cs);
    }

    // 5) Evict cache entries not seen for several scans (process has exited).
    for (size_t i = 0; i < g_pidPathCache.size(); ) {
        if ((int)(g_pidCacheGen - g_pidPathCache[i].lastSeenGen) > 4)
            g_pidPathCache.erase(g_pidPathCache.begin() + (long)i);
        else ++i;
    }
}

// ============================================================
//  REORDER HELPERS
// ============================================================
static int CalculateReorderSlot(POINT screenPt, int n) {
    // Map screen X to overlay-local X and find the closest slot midpoint.
    // Safe to call outside CS  --  only reads g_dockCurrentX (float, atomic read).
    if (n <= 1) return 0;
    int localX = screenPt.x - (int)g_dockCurrentX;
    PinType reorderType = g_draggedPinType;
    int fallback = g_reorderSrcIdx;
    for (int i = 0; i < n; ++i) {
        if (i >= (int)g_pinnedApps.size() || g_pinnedApps[i].type != reorderType) continue;
        RECT r   = GetIconRectLocal(i, n);
        int  mid = (r.left + r.right) / 2;
        if (localX < mid) return i;
        fallback = i;
    }
    return fallback;
}

static void UpdateReorderPositions() {
    // Assigns targetX to every icon as if g_reorderSrcIdx is placed at
    // g_reorderTargetIdx and all others shift around it.
    if (g_reorderSrcIdx < 0 || g_reorderTargetIdx < 0) return;
    EnterCriticalSection(&g_cs);
    int n   = (int)g_pinnedApps.size();
    int src = g_reorderSrcIdx;
    int dst = g_reorderTargetIdx;
    if (src >= 0 && src < n && dst >= 0 && dst < n &&
        g_pinnedApps[src].type == g_pinnedApps[dst].type) {
        PinType type = g_pinnedApps[src].type;
        std::vector<int> section;
        for (int i = 0; i < n; ++i)
            if (g_pinnedApps[i].type == type) section.push_back(i);

        auto srcIt = std::find(section.begin(), section.end(), src);
        auto dstIt = std::find(section.begin(), section.end(), dst);
        if (srcIt != section.end() && dstIt != section.end()) {
            int moving = *srcIt;
            section.erase(srcIt);
            dstIt = std::find(section.begin(), section.end(), dst);
            if (dstIt == section.end()) section.push_back(moving);
            else section.insert(dstIt, moving);

            for (int iconIdx : section) {
                RECT r = GetIconRectLocal(iconIdx, n);
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
    if (src < n && dst < n && g_pinnedApps[src].type == g_pinnedApps[dst].type) {
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
            // FIX (reorder animation): clear residual momentum so the committed
            // layout is stable and never overshoots/jitters after the drop.
            g_pinnedApps[i].velocityX = 0.f;
            // Clear hover magnification/spread so the committed layout is clean.
            g_pinnedApps[i].hoverScale  = 1.0f;
            g_pinnedApps[i].hoverShiftX = 0.f;
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

        // Frosted blur (same as primary). ApplyNativeBackdrop already applied the
        // neutral acrylic tint to secondary docks; no opaque fill so the blur shows.
        if (ENABLE_GLASS_OVERLAY && cr.right > 0) ApplyDockGlassTint(hwnd);

        // Draw icons at canonical positions  --  no animation state on secondary docks
        for (int i = 0; i < n; ++i) {
            const auto& app = g_pinnedApps[i];
            if (!app.icon || app.opacity < 0.05f) continue;
            // FIX (dock/mirror desync): while an icon is being dragged OFF the
            // real dock to unpin, the primary overlay hides it (see the
            // `continue` in OverlayWndProc's icon loop). The mirror must hide it
            // too, otherwise the same icon appears detached on the real dock but
            // still present on the mirror -- reading as a duplicate/ghost icon
            // (the "two icons/threads for one icon" artifact across dock+mirror).
            if (g_dragFromDock && i == g_dragFromDockIdx &&
                g_dragState == DRAG_DRAGGING) continue;
            RECT r = GetIconRectLocal(i, n);
            // Clean premultiplied blit so mirror icons match the primary dock
            // (no dark edge fringe; crisp downscale).
            BlitIconAlpha(hdc, app.icon, r.left, r.top, ICON_SIZE, (BYTE)(int)(255 * app.opacity));
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

    // IDEMPOTENCY GUARD (never stack a second mirror-dock layer): every caller
    // is expected to DestroySecondaryDocks() first, but the rebuild is posted
    // cross-thread (WM_QPD_REBUILD_SECONDARY) and rate-limited on two paths
    // (WinEventProc + WorkerThread), so a race could reach Init while a live set
    // still exists -- appending to it would leave two overlapping mirror docks
    // (the reported duplicate/"second dock" layer). Tear down any live set here
    // before building the fresh one so we always end with exactly one layer.
    if (!g_secondaryDocks.empty()) DestroySecondaryDocks();

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
    BASE_ICON_SIZE       = Wh_GetIntSetting(L"iconSize",          33);
    BASE_ICON_SPACING    = 12;   // fixed per-icon gap (iconSpacing setting removed from the Windhawk UI)
    SEPARATOR_OPACITY    = Wh_GetIntSetting(L"separatorOpacity", 100);
    ENABLE_GLASS_OVERLAY = Wh_GetIntSetting(L"enableGlassOverlay", 1) != 0;
    ENABLE_REORDER       = Wh_GetIntSetting(L"enableReorder",       1) != 0;
    ENABLE_SCROLL_NAV    = Wh_GetIntSetting(L"enableScrollNav",     1) != 0;
    ENABLE_ICON_THREADS  = Wh_GetIntSetting(L"enableDragTether",    1) != 0;
    THREAD_THICKNESS     = std::max(1, std::min(10, Wh_GetIntSetting(L"dragTetherThickness", 2)));
    THREAD_MAX_STRETCH_PX = (float)std::max(150, std::min(650, Wh_GetIntSetting(L"dragRopeBreakLength", 450)));
    UNPIN_TRIGGER        = std::max(0, std::min(1, Wh_GetIntSetting(L"unpinTrigger", 0)));
    THREAD_COLOR_MODE    = Wh_GetIntSetting(L"dragTetherColorMode", 1) != 0 ? 1 : 0;
    THREAD_HUE           = ((Wh_GetIntSetting(L"dragTetherHue", 30) % 360) + 360) % 360;
    ENABLE_DOUBLE_RCLICK_UNPIN = Wh_GetIntSetting(L"enableDoubleRightClickUnpin", 0) != 0;
    CORNER_ROUNDNESS     = Wh_GetIntSetting(L"cornerRoundness",     100);
    ENABLE_EXPLORER_WORKSPACE_PINS = Wh_GetIntSetting(L"enableExplorerWorkspacePins", 0) != 0;
    MULTI_MONITOR_DOCK   = Wh_GetIntSetting(L"multiMonitorDock",    0) != 0;
    STARTUP_DELAY_MS     = Wh_GetIntSetting(L"startupDelay",        0);
    DOCK_GAP_FROM_START  = Wh_GetIntSetting(L"dockGapFromStart",    6);
    g_logLevel           = ClampLogLevel(Wh_GetIntSetting(L"logLevel", (int)LOG_ERROR));
    // Hotkey: 0 modifiers or 0 key = disabled. Clamp modifiers to valid MOD_* flags.
    g_hotkeyMods = (UINT)Wh_GetIntSetting(L"hotkeyModifiers", (int)(MOD_CONTROL | MOD_ALT));
    g_hotkeyKey  = (UINT)Wh_GetIntSetting(L"hotkeyKey",       (int)'P');
    g_hotkeyMods &= (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);
    // Auto-hide sync: default OFF  --  dock stays visible even if taskbar auto-hides
    ENABLE_AUTOHIDE_SYNC = Wh_GetIntSetting(L"autoHideSync", 0) != 0;

    MAX_PINNED_APPS   = std::max(1,  std::min(20,   MAX_PINNED_APPS));
    // FIX (decouple): the per-type app cap and the fixed dock width both
    // derive from MAX_APP_PINS. Previously it was hard-coded to 5, silently
    // capping the maxPinnedApps setting. Track the user's value instead.
    MAX_APP_PINS = MAX_PINNED_APPS;
    BASE_ICON_SIZE    = std::max(16, std::min(48,   BASE_ICON_SIZE));
    // BASE_ICON_SPACING is fixed at 12 (no user setting) -- no clamp needed.
    SEPARATOR_OPACITY = std::max(0,  std::min(100,  SEPARATOR_OPACITY));
    CORNER_ROUNDNESS  = std::max(0,  std::min(100,  CORNER_ROUNDNESS));
    STARTUP_DELAY_MS  = std::max(0,  std::min(3000, STARTUP_DELAY_MS));
    DOCK_GAP_FROM_START = std::max(0, std::min(40,  DOCK_GAP_FROM_START));

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

    QueryPerformanceFrequency(&g_perfFreq);  // enable sub-ms animation timing
    g_bootStartTime = GetTickCount();
    g_systemState   = STATE_BOOT;

    // Bounded geometry retry: the taskbar / DWM composition may not be ready at
    // injection time, so a single RefreshTaskbarCache can leave g_dockLocalW == 0
    // and the dock invisible until the first worker poll (or flying in from 0,0).
    // Retry briefly so the FIRST paint uses correct geometry -> faster,
    // flicker-free dock appearance at startup.
    //
    // PERF: this loop runs on Windhawk's calling thread, so every ms here is a ms
    // that a Compile/reload blocks the Windhawk UI. On a normal reload the taskbar
    // is already up, so the FIRST RefreshTaskbarCache succeeds and we break with
    // ZERO sleep. The retries only matter on a cold boot when DWM isn't ready yet,
    // so the per-attempt wait was halved (20 -> 10 ms, ~60 ms worst case instead
    // of ~120 ms). If geometry still isn't ready, the worker's 100 ms boot poll
    // + snap-position logic corrects it within one cycle anyway.
    for (int attempt = 0; attempt < 6; ++attempt) {
        RefreshTaskbarCache();
        if (g_dockLocalW > 0) break;
        Sleep(10);
    }
    LoadPinnedApps();

    if (!CreateInputOwnerWindow()) { Wh_ModUninit(); return FALSE; }
    if (!CreateOverlayWindow()) { Wh_ModUninit(); return FALSE; }
    InitGhostDIB();
    if (!CreateGhostWindow())   { Wh_ModUninit(); return FALSE; }

    // Pre-warm the drag-effect layered windows (rope + vanish) NOW, at startup,
    // exactly like the ghost above -- create them + present one transparent frame
    // so DWM registers their layered surfaces up front. This is the real fix for
    // the black rope/icon that appeared only on real hardware during unpin:
    // creating them lazily mid-gesture let their first present flash black for a
    // frame on a hardware compositor. Non-fatal if it fails (they self-create on
    // first use as before).
    PrewarmDragEffectWindows();

    // Snap position immediately so there is no fly-in from (0,0) at startup
    if (!g_positionInitialized && g_cachedDockRect.left > 0) {
        g_dockCurrentX        = (float)g_cachedDockRect.left;
        g_dockCurrentY        = (float)g_cachedDockRect.top;
        g_dockTargetX         = g_dockCurrentX;
        g_dockTargetY         = g_dockCurrentY;
        g_positionInitialized = true;
    }

    // Force the overlay visible and in position before the worker thread starts
    if (g_inputWnd && IsWindow(g_inputWnd)) {
        ShowWindow(g_inputWnd, SW_SHOWNOACTIVATE);
        SetWindowPos(g_inputWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
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

    LOG_IMPORTANT(L"INIT: v31.0.0 OK. state=%d pinned=%d glass=%d reorder=%d explorerWorkspaces=%d multimon=%d delay=%d hotkey=0x%X+0x%X autohide=%d logLevel=%d",
              g_systemState, (int)g_pinnedApps.size(),
              (int)ENABLE_GLASS_OVERLAY, (int)ENABLE_REORDER,
              (int)ENABLE_EXPLORER_WORKSPACE_PINS,
              (int)MULTI_MONITOR_DOCK, STARTUP_DELAY_MS,
              g_hotkeyMods, g_hotkeyKey, (int)ENABLE_AUTOHIDE_SYNC, (int)g_logLevel);
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

    SetHighResTimer(false);  // release 1ms timer period if still held

    // Destroy secondary monitor docks first (they repaint from the same icon handles)
    DestroySecondaryDocks();
    // FIX-3: secondary-docks CS  --  delete after DestroySecondaryDocks so no further
    // callers can attempt to acquire it (all callers check g_secondaryDocksCSInit first).
    if (g_secondaryDocksCSInit) {
        g_secondaryDocksCSInit = false;
        DeleteCriticalSection(&g_secondaryDocksCS);
    }

    // Destroy GDI resources in reverse creation order
    if (g_tetherDIB) { DeleteObject(g_tetherDIB); g_tetherDIB = NULL; g_tetherBits = NULL; }
    if (g_tetherWnd) { DestroyWindow(g_tetherWnd); g_tetherWnd = NULL; }
    g_tetherW = g_tetherH = 0;
    if (g_vanishDIB) { DeleteObject(g_vanishDIB); g_vanishDIB = NULL; g_vanishBits = NULL; }
    if (g_vanishWnd) { DestroyWindow(g_vanishWnd); g_vanishWnd = NULL; }
    g_vanishW = g_vanishH = 0;
    g_vanishActive = false;
    g_vanishParticles.clear();
    // Release any cross-thread vanish request that was posted (double-right-click)
    // but never consumed by the worker loop before shutdown.
    {
        HICON pend = (HICON)InterlockedExchangePointer(
            (PVOID volatile*)&g_vanishReqIcon, NULL);
        if (pend) DestroyIcon(pend);
    }
    if (g_ghostDIB)  { DeleteObject(g_ghostDIB);  g_ghostDIB  = NULL; }
    if (g_ghostWnd)  { DestroyWindow(g_ghostWnd);  g_ghostWnd  = NULL; }
    if (g_overlayWnd){ DestroyWindow(g_overlayWnd);g_overlayWnd= NULL; }
    if (g_inputWnd)  { DestroyWindow(g_inputWnd);  g_inputWnd  = NULL; }

    if (g_blackBrush)    { DeleteObject(g_blackBrush);    g_blackBrush    = NULL; }
    if (g_linePenNormal) { DeleteObject(g_linePenNormal); g_linePenNormal = NULL; }
    if (g_linePenFlash)  { DeleteObject(g_linePenFlash);  g_linePenFlash  = NULL; }
    if (g_linePenDrop)   { DeleteObject(g_linePenDrop);   g_linePenDrop   = NULL; }
    if (g_runDotBrush)   { DeleteObject(g_runDotBrush);   g_runDotBrush   = NULL; }

    // Alpha-blend off-screen buffer
    if (g_alphaBlendDC && g_alphaBlendOldBmp) {
        SelectObject(g_alphaBlendDC, g_alphaBlendOldBmp);
        g_alphaBlendOldBmp = NULL;
    }
    if (g_alphaBlendDC)  { DeleteDC(g_alphaBlendDC);       g_alphaBlendDC   = NULL; }
    if (g_alphaBlendBmp) { DeleteObject(g_alphaBlendBmp);  g_alphaBlendBmp  = NULL; }
    g_alphaBlendBits = NULL;
    g_alphaBlendSize = 0;

    // Native-size icon-blit buffer (BlitIconAlpha)
    if (g_iconBlitDC && g_iconBlitOldBmp) {
        SelectObject(g_iconBlitDC, g_iconBlitOldBmp);
        g_iconBlitOldBmp = NULL;
    }
    if (g_iconBlitDC)  { DeleteDC(g_iconBlitDC);       g_iconBlitDC   = NULL; }
    if (g_iconBlitBmp) { DeleteObject(g_iconBlitBmp);  g_iconBlitBmp  = NULL; }
    g_iconBlitBits = NULL;
    g_iconBlitSize = 0;

    // Separator alpha-blend DIB cache
    if (g_sepDC && g_sepOldBmp) {
        SelectObject(g_sepDC, g_sepOldBmp);
        g_sepOldBmp = NULL;
    }
    if (g_sepDIB) { DeleteObject(g_sepDIB); g_sepDIB = NULL; }
    if (g_sepDC)  { DeleteDC(g_sepDC);      g_sepDC  = NULL; }
    g_sepBits    = NULL;
    g_sepCachedH = 0;

    DestroyPaintBuffer();

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
    BASE_ICON_SIZE       = Wh_GetIntSetting(L"iconSize",           33);
    BASE_ICON_SPACING    = 12;   // fixed per-icon gap (iconSpacing setting removed from the Windhawk UI)
    SEPARATOR_OPACITY    = Wh_GetIntSetting(L"separatorOpacity",  100);
    ENABLE_GLASS_OVERLAY = Wh_GetIntSetting(L"enableGlassOverlay",  1) != 0;
    ENABLE_REORDER       = Wh_GetIntSetting(L"enableReorder",       1) != 0;
    ENABLE_SCROLL_NAV    = Wh_GetIntSetting(L"enableScrollNav",     1) != 0;
    ENABLE_ICON_THREADS  = Wh_GetIntSetting(L"enableDragTether",    1) != 0;
    THREAD_THICKNESS     = std::max(1, std::min(10, Wh_GetIntSetting(L"dragTetherThickness", 2)));
    THREAD_MAX_STRETCH_PX = (float)std::max(150, std::min(650, Wh_GetIntSetting(L"dragRopeBreakLength", 450)));
    UNPIN_TRIGGER        = std::max(0, std::min(1, Wh_GetIntSetting(L"unpinTrigger", 0)));
    THREAD_COLOR_MODE    = Wh_GetIntSetting(L"dragTetherColorMode", 1) != 0 ? 1 : 0;
    THREAD_HUE           = ((Wh_GetIntSetting(L"dragTetherHue", 30) % 360) + 360) % 360;
    ENABLE_DOUBLE_RCLICK_UNPIN = Wh_GetIntSetting(L"enableDoubleRightClickUnpin", 0) != 0;
    CORNER_ROUNDNESS     = Wh_GetIntSetting(L"cornerRoundness",     100);
    ENABLE_EXPLORER_WORKSPACE_PINS = Wh_GetIntSetting(L"enableExplorerWorkspacePins", 0) != 0;
    MULTI_MONITOR_DOCK   = Wh_GetIntSetting(L"multiMonitorDock",    0) != 0;
    STARTUP_DELAY_MS     = Wh_GetIntSetting(L"startupDelay",        0);
    DOCK_GAP_FROM_START  = Wh_GetIntSetting(L"dockGapFromStart",    6);
    ENABLE_AUTOHIDE_SYNC = Wh_GetIntSetting(L"autoHideSync",        0) != 0;
    g_logLevel           = ClampLogLevel(Wh_GetIntSetting(L"logLevel", (int)LOG_ERROR));
    g_hotkeyMods = (UINT)Wh_GetIntSetting(L"hotkeyModifiers", (int)(MOD_CONTROL | MOD_ALT));
    g_hotkeyKey  = (UINT)Wh_GetIntSetting(L"hotkeyKey",       (int)'P');
    g_hotkeyMods &= (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);

    MAX_PINNED_APPS   = std::max(1,  std::min(20,   MAX_PINNED_APPS));
    // FIX (decouple): the per-type app cap and the fixed dock width both
    // derive from MAX_APP_PINS. Previously it was hard-coded to 5, silently
    // capping the maxPinnedApps setting. Track the user's value instead.
    MAX_APP_PINS = MAX_PINNED_APPS;
    BASE_ICON_SIZE    = std::max(16, std::min(48,   BASE_ICON_SIZE));
    // BASE_ICON_SPACING is fixed at 12 (no user setting) -- no clamp needed.
    SEPARATOR_OPACITY = std::max(0,  std::min(100,  SEPARATOR_OPACITY));
    CORNER_ROUNDNESS  = std::max(0,  std::min(100,  CORNER_ROUNDNESS));
    STARTUP_DELAY_MS  = std::max(0,  std::min(3000, STARTUP_DELAY_MS));
    DOCK_GAP_FROM_START = std::max(0, std::min(40,  DOCK_GAP_FROM_START));

    // Invalidate cached dock-width so it is recalculated with new sizes/DPI.
    g_fixedDockWidth = 0;
    g_lastDpiForWidth = 0;

    // LIVE DOCK-GAP FIX: the dock's X is protected by a jitter lock
    // (g_stabilizedDockLeft / g_dockPositionLocked) that ignores horizontal
    // moves smaller than POS_LOCK_THRESHOLD (20 px) so Start-button detection
    // noise can't wobble the dock. But that same filter also swallowed a
    // deliberate "Dock gap from Start" change (range 0..40 px -> often a
    // sub-20 px shift), so the setting appeared to do nothing until a large
    // jump. A settings change is an explicit user action, not jitter, so drop
    // the lock here: the next RefreshTaskbarCache re-seats g_stabilizedDockLeft
    // to the exact new dockLeft and UpdateDockTargetPosition glides the dock
    // there smoothly. This makes DOCK_GAP_FROM_START apply live and precisely.
    g_dockPositionLocked = false;

    // Refresh geometry and force an immediate repaint.
    RefreshTaskbarCache();
    RepositionOverlay();
    if (g_overlayWnd && IsWindow(g_overlayWnd)) {
        ApplyNativeBackdrop(g_overlayWnd);
        InvalidateRect(g_overlayWnd, NULL, FALSE);
    }

    LOG_IMPORTANT(L"SETTINGS CHANGED: maxPins=%d iconSz=%d spacing=%d glass=%d reorder=%d explorerWorkspaces=%d logLevel=%d",
              MAX_PINNED_APPS, BASE_ICON_SIZE, BASE_ICON_SPACING,
              (int)ENABLE_GLASS_OVERLAY, (int)ENABLE_REORDER,
              (int)ENABLE_EXPLORER_WORKSPACE_PINS, (int)g_logLevel);
}

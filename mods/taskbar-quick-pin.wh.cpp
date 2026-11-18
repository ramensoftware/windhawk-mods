// ==WindhawkMod==
// @id              taskbar-quick-pin
// @name            Left Taskbar Quick Pin Dock
// @description     A persistent icon dock anchored left of the Start button. Drag any app or File Explorer window to pin, left-click to launch or focus, drag within the dock to reorder, or drag off to unpin.
// @author          Ashix
// @version         3.2026.1811
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @donateUrl       https://ko-fi.com/kashix
// @include         windhawk-mod.exe
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -lole32 -loleaut32 -luuid -lshlwapi -lgdi32 -lgdiplus -lmsimg32 -ldwmapi -lwinmm
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Left Taskbar Quick Pin Dock

> **Windows 11 (centered taskbar) only.** On left-aligned or unsupported layouts, the dock stays hidden and idle.

![Left Taskbar Quick Pin Dock](https://raw.githubusercontent.com/k-ashix/ashix-windhawk-mods/main/Taskbar_Quick_Pin/Taskbar-quick-pin.png)

![Edge Pill Workspace Dock](https://raw.githubusercontent.com/k-ashix/ashix-windhawk-mods/main/Taskbar_Quick_Pin/Edge-Pill.png)

A lightweight, DPI-aware icon dock anchored left of the Start button. Pin apps or File Explorer workspaces, launch or focus with one click, and drag to reorder or unpin.

## Quick Usage

| Icon | Trigger | Action |
|---|---|---|
| 📌 | Drag an app or Explorer window onto dock | Pin app, or pin **only the active tab** of that Explorer window |
| 🗂️ | `Ctrl` + Drag an Explorer window onto dock | Group **all tabs inside that one Explorer window** (never merges other windows) |
| ➕ | Drag an Explorer window onto an existing workspace icon | Append that window's active tab (or all its tabs with `Ctrl`) |
| 🚀 | Left-click icon | Launch app/workspace, or focus its already-open window/tab |
| ↔️ | Drag icon left / right | Reorder icons within their section (workspaces or apps) |
| 🗑️ | Double-click, right-click, or drag icon off dock | Unpin / delete item from the dock |
| 👁️ | Hover an Edge Pill Arc Card (or app icon) | Show the **Read-Only Thumbnail Preview** of captured apps and split ratios |
| 🛠️ | Right-click (single or double) an Arc Card | Open & lock **Customize Mode** in the Thumbnail Preview to add (`+` / drag-drop) or delete (`×` / double-click) apps |
| ❌ | Click top-right `×` on an Arc Card | Delete that saved workspace state from the Edge Pill Arc |
| ⌨️ | `Ctrl + Alt + P` | Pin or unpin the focused app |
| 🔒 | Triple-tap `P` / `U` / `L` (opt-in) | Pin / Unpin / Lock dock (resets on typing, modifiers, or clicks) |

## How Explorer Grouping Works

- **Separate Windows Never Merge:** Normal drag pins only the **active tab** (1 yellow folder). **`Ctrl` + Drag** on a 5-tab Explorer window groups only those 5 tabs; **`Ctrl` + Drag** on a separate 10-tab window groups only those 10 tabs—they never merge.
- **Color Coding:** Single-folder workspaces always stay **native Windows yellow**. Grouped (2+) workspaces automatically get a distinct muted color from the 30-color palette (yellow/black/white excluded) with a smooth transition.
- **Manage a Group:** Hover a grouped workspace icon to open its frosted drop-up folder list, click inside to hold it open, drag rows to reorder, or click `×` to remove a folder (removing down to 1 reverts to native yellow).

## Key Settings

| Setting | What It Controls | Default |
|---|---|---|
| **Limit visible pins before scrolling** | OFF = grow leftward up to taskbar's safe edge (Widgets-aware); ON = cap visible app/workspace slots and scroll | `OFF` |
| **Dock background style** | `Frosted glass pill` (translucent frost) or `Transparent` (icons only) | `Transparent` |
| **Max workspace pins** | Maximum workspace icons allowed on the dock (`1 - 30`) | `10` |
| **Max visible paths in drop-up** | Visible folder rows in a workspace drop-up before wheel scrolling (`3 - 20`, no scrollbar) | `8` |
| **Grouped folder color mode** | `Random (unused colors first)` or `Fixed order from palette` | `Random` |
| **Grouped folder color palette** | Single-line pipe-separated `#RRGGBB` list (yellow/extremes auto-skipped) | 30 muted colors |
| **Open grouped paths in separate windows** | OFF = restore as tabs in one Explorer window; ON = separate windows | `OFF` |
| **Triple-tap `P` / `U` / `L` gestures** | Opt-in bare-key triple-tap to pin, unpin, or lock the dock | `OFF` |

## Known Limitations

- Apps with custom shell icon handlers or unready UWP hosts may skip pinning if no valid icon resolves.
- English UI strings only.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dock:
  - maxPinnedApps: 5
    $name: Max pinned apps
    $description: Maximum number of apps that can be pinned (1 - 50).
  - limitVisiblePins: false
    $name: Limit visible pins before scrolling
    $description: >-
      OFF (default) = dock grows leftward up to the taskbar's safe edge and only
      scrolls if pins exceed your monitor width. ON = cap visible slots and scroll.
  - maxVisibleApps: 10
    $name: Max visible app pins
    $description: Visible app slots before scrolling when Limit visible pins is ON (1 - 30).
    $showIf: {limitVisiblePins: true}
  - maxVisibleWorkspaces: 10
    $name: Max visible workspaces
    $description: Visible workspace slots before scrolling when Limit visible pins is ON (1 - 30).
    $showIf: {limitVisiblePins: true}
  - iconSize: 33
    $name: Icon size (px at 96 DPI)
    $description: Base icon size in pixels before DPI scaling (16 - 48).
  - dockGapFromStart: 6
    $name: Gap from Start button (px)
    $description: Distance between the dock right edge and the Start button (0 - 40).
  - dockStyle: transparent
    $name: Dock background style
    $description: Choose between icons only, frosted glass, or frosted glass with an outline.
    $options:
    - transparent: Transparent (icons only)
    - glass: Frosted glass pill
    - glassBorder: Frosted glass pill (with outline)
  - cornerRoundness: 40
    $name: Corner roundness (0 - 100)
    $description: 0 = square, 1 - 40 = rounded, 41 - 100 = full pill.
  - showTooltips: true
    $name: Show hover tooltips
    $description: Show the app or workspace name pill when hovering an icon.
  $name: Dock Layout & Appearance

- workspaces:
  - enableExplorerWorkspacePins: true
    $name: Enable Explorer workspace pins
    $description: Allow File Explorer windows and tabs to be pinned as a workspace.
  - maxWorkspacePins: 10
    $name: Max workspace pins
    $description: Maximum Explorer workspace pins allowed on the dock (1 - 30).
  - maxPathsPerWorkspace: 15
    $name: Max paths per workspace group
    $description: Maximum folders/tabs grouped in one workspace pin (1 - 30).
  - popupVisiblePaths: 8
    $name: Max visible paths in drop-up
    $description: >-
      Maximum folder rows shown at once in the workspace drop-up before wheel
      scrolling (3 - 20). Scroll with the mouse wheel (no scrollbar).
  - folderColorMode: random
    $name: Grouped folder color mode
    $description: >-
      Single-path workspaces always stay native Windows yellow. Grouped (2+)
      workspaces pick either an unused random color or fixed palette order.
    $options:
    - random: Random (unused colors first)
    - ordered: Fixed order from palette
  - folderColorPalette: "#4F7CAC|#3D5A80|#5B8FB9|#2E6F95|#6B9AC4|#5E60CE|#6D597A|#7B6D8D|#8E7DBE|#52489C|#B56576|#C06C84|#A2495E|#9D5C63|#C97B84|#C86D51|#B85D43|#CC7A5B|#A45A49|#B07156|#3D9970|#4A8B71|#2F7A6D|#3B8EA5|#529985|#6B8E6E|#4F775A|#768976|#5A7D7C|#687980"
    $name: Grouped folder color palette (30 hex colors, pipe-separated)
    $description: >-
      Pipe-separated #RRGGBB colors for grouped workspaces. Yellow/amber/gold
      and near-black/white tones are automatically skipped even if pasted.
  - openInSeparateWindows: false
    $name: Open grouped paths in separate windows
    $description: OFF = restore as tabs in one Explorer window; ON = separate windows.
  - reopenClosedPathsOnFocus: false
    $name: Reopen closed paths when focusing workspace
    $description: >-
      OFF (default) = focus any already-open folder without reopening closed ones.
      ON = reopen any closed folders in the group.
  - showWorkspaceDivider: true
    $name: Show workspace divider
    $description: Show the gold divider between pinned workspaces and app pins.
  $name: Explorer Workspaces

- interactions:
  - enableReorder: true
    $name: Drag to reorder
    $description: Drag pinned icons left or right within their section to rearrange them.
  - enableScrollNav: true
    $name: Scroll-wheel navigation
    $description: Hover the dock and scroll the wheel to step across pinned icons.
  - enableRapidUnpinAll: false
    $name: Rapid triple-click unpin-all (opt-in)
    $description: Clicking three times inside the dock zone within 1 second unpins every item.
  $name: Mouse & Drag Interactions

- shortcuts:
  - hotkeyModifiers: ctrlAlt
    $name: Pin/unpin hotkey modifiers
    $description: Modifier keys for the pin/unpin hotkey (or Disabled).
    $options:
    - none: Disabled (no hotkey)
    - alt: Alt
    - ctrl: Ctrl
    - ctrlAlt: Ctrl + Alt
    - shift: Shift
    - ctrlShift: Ctrl + Shift
    - altShift: Alt + Shift
    - win: Win
    - winAlt: Win + Alt
    - winCtrl: Win + Ctrl
  - hotkeyKey: P
    $name: Pin/unpin hotkey key
    $description: Key letter (A - Z), function key (F1 - F12), or "none" to disable.
  - enableKeyGestures: false
    $name: Triple-tap P / U / L gestures (opt-in)
    $description: >-
      Tap P / U / L three times quickly to pin, unpin, or lock the dock.
      Guarded against typing: resets on other keys, Space, Enter, Backspace, Tab, modifiers, or clicks.
  - tripleTapWindowMs: 300
    $name: Triple-tap max gap (ms)
    $description: Maximum milliseconds allowed between consecutive taps (150 - 350).
    $showIf: {enableKeyGestures: true}
  - enableLockAnimation: true
    $name: Lock/unlock sweep animation
    $description: OFF = subtle edge highlight; ON = left-to-right sweep across the dock.
  $name: Hotkeys & Key Gestures

- advanced:
  - fullscreenBehavior: hide
    $name: Fullscreen behavior
    $description: How the dock behaves when a fullscreen app or game is active.
    $options:
    - hide: Hide dock (disabled while fullscreen)
    - allow: Keep visible (active)
  - captureBehavior: exclude
    $name: Screen capture behavior
    $description: >-
      Exclude hides the dock and popups from screenshots and recordings
      (WDA_EXCLUDEFROMCAPTURE); Ignore keeps them visible.
    $options:
    - exclude: Exclude from screenshots
    - ignore: Allow in screenshots (Ignore)
  - autoHideSync: false
    $name: Sync with taskbar auto-hide
    $description: Hide the dock when the Windows taskbar auto-hides off screen.
  - startupDelay: 0
    $name: Startup delay (ms)
    $description: Extra wait before initializing (0 - 3000 ms).
  $name: Visibility & System

- edgePin:
  - enableEdgePill: true
    $name: Show Edge Pill
    $description: Show the borderless screen-edge pill for quick workspace capture and switching.
  - defaultEdge: left
    $name: Default screen edge
    $description: Screen edge where the pill anchors initially (drag the pill to any of the 4 edges at any time).
    $options:
    - left: Left edge
    - right: Right edge
    - top: Top edge
    - bottom: Bottom edge
  - rememberWindowPosition: true
    $name: Remember dragged edge & position
    $description: Persist the last screen edge and offset where you dragged the pill.
  - windowSizePreset: extraLarge
    $name: Arc dock & schematic scale preset
    $description: Overall scale preset for the Arc Cards and schematic preview card (Extra Large by default).
    $options:
    - extraLarge: Extra Large (135% - Default)
    - large: Large (118%)
    - default: Standard (100%)
  - pillThickness: 8
    $name: Pill thickness (px at 96 DPI)
    $description: Thickness of the borderless capsule edge pill (4 - 24 px).
  - pillLength: 96
    $name: Pill length (px at 96 DPI)
    $description: Length of the borderless capsule edge pill (48 - 260 px).
  - pillToArcGap: 10
    $name: Gap from Pill to Arc Cards (px at 96 DPI)
    $description: Breathing space between the edge pill and the partial-circle arc (4 - 48 px).
  - arcCardSize: 48
    $name: Arc Card size (px at 96 DPI)
    $description: Width and height of each rounded-rectangle pinned card in the arc (32 - 84 px).
  - arcVisibleSlots: 4
    $name: Visible workspace cards around center '+' (2 - 6)
    $description: Number of visible workspace cards surrounding the center '+' card (scroll wheel rotates through all pinned workspaces).
  - hoverLingerMs: 280
    $name: Hover linger time (ms)
    $description: How long the coupled arc and schematic thumbnail stay open after the cursor leaves both (100 - 800 ms).
  - proximityRevealPx: 48
    $name: Drag-near-pill reveal distance (px)
    $description: Distance from the pill where dragging a window reveals the drop zone (16 - 160 px).
  $name: Edge Pill & Workspace Arc

- edgeWorkspaces:
  - toggleDockHotkey: "Alt+W"
    $name: Toggle Edge Pill Hover Dock hotkey
    $description: Shortcut (e.g. Alt+W, Ctrl+Alt+W, or none) to toggle the Edge Pill Hover Dock.
  - captureWorkspaceHotkey: "Alt+Shift+W"
    $name: Capture current monitor workspace hotkey
    $description: Shortcut (e.g. Alt+Shift+W or none) to capture the cursor's monitor into a new layout workspace.
  - captureWindowFilter: edgeTouching
    $name: Workspace window capture mode
    $description: >-
      Choose whether '+' captures only docked/snapped windows whose edges touch
      or sit near the screen edges (ignoring small floating popups), or all visible windows.
    $options:
    - edgeTouching: Edge-touching / snapped windows only (ignore floating windows)
    - allVisible: All visible non-occluded windows
  - allowDuplicateWorkspaces: false
    $name: Allow duplicate workspace shortcuts
    $description: >-
      OFF (default) = prevent clutter when pinning the same set of apps again
      (e.g. Edge + VS Code). ON = allow creating duplicate pins for the same apps.
  - duplicateWorkspaceAction: ignore
    $name: When pinning an already-pinned app group
    $description: >-
      Choose whether re-pinning the same apps keeps the existing workspace untouched
      (No update needed), updates its window split ratios, or creates a duplicate.
    $options:
    - ignore: Keep existing workspace (no duplicate, no update needed)
    - update: Update existing workspace layout & split ratios
    - allow: Create duplicate workspace pin anyway
  - maxAppsPerWorkspace: 8
    $name: Max captured apps per workspace (2 - 12)
    $description: Maximum visible windows captured into one multi-app workspace layout.
  - ignoreOccludedWindows: true
    $name: Ignore occluded background windows
    $description: Skip windows that are 86%+ covered by higher Z-order windows during capture.
  - captureBrowserActiveTab: true
    $name: Capture active browser URL
    $description: Record the active tab URL of Chromium/Firefox browsers when capturing a workspace.
  - reuseOpenWindows: true
    $name: Reuse already-open app windows on restore
    $description: Reposition running windows instead of spawning duplicate instances when restoring a workspace.
  - excludedProcesses: ""
    $name: Extra excluded processes (comma-separated)
    $description: Additional .exe names to ignore when capturing a workspace layout.
  $name: Workspace Capture & Hotkeys
*/
// ==/WindhawkModSettings==

// ============================================================
//  INCLUDES
// ============================================================
#include <initguid.h>   // MUST be first: defines DEFINE_GUID to emit storage
#include <windows.h>
#include <windowsx.h>
#include <winioctl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <shlguid.h>
#include <dwmapi.h>
#include <mmsystem.h>
#include <exdisp.h>
#include <commoncontrols.h>
#include <uiautomation.h>
#include <gdiplus.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <atomic>

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
// Dock border outline is controlled by DOCK_STYLE (glass vs glassBorder).

// ============================================================
//  CONFIGURATION  --  base values at 96 DPI, scaled at runtime
// ============================================================
static const int   DRAG_THRESHOLD_PX      = 6;     // Pixels before drag is confirmed
static const int   DOCK_TOP_OVERFLOW_PX   = 12;    // Extra transparent headroom for 1.15x hover scale + 50% corner badge
static const int   BASE_DOCK_PAD_LEFT     = 6;     // Left internal padding
static const int   BASE_DOCK_PAD_RIGHT    = 8;     // Right internal padding
static const int   BASE_SECTION_GAP       = 20;    // Gap between workspace and app regions
static const int   BASE_MAGNETIC_RANGE    = 60;    // Ghost visibility radius beyond dock
static const int   BASE_GHOST_SIZE        = 40;    // Drag ghost size in pixels
// CLICK_MAX_MS was removed: the press->release state machine is a polling loop,
// so a time cap silently ate real clicks (see the PRESS->IDLE comment). Click
// vs. drag is now decided on MOVEMENT ONLY (CLICK_MAX_MOVE_PX).
static const int   CLICK_MAX_MOVE_PX      = 5;     // Max cursor movement for a click
static const int   MIN_VALID_DOCK_WIDTH   = 80;    // Minimum acceptable dock pixel width
static const int   BOOT_PHASE_MS          = 300;   // Boot stabilisation timeout (dynamic exit preferred; halved from 600 to reach a stable dock sooner on boot)
static const int   RAPID_CLICK_THRESHOLD  = 3;     // Clicks to trigger unpin-all (reduced from 5 for usability)
static const int   RAPID_CLICK_WINDOW_MS  = 1000;  // Window for rapid-click detection (ms)
static const int   MAGNETIC_ZONE_EXPAND_PX = 8;    // Expand magnetic zone up/down
static const int   POS_LOCK_THRESHOLD     = 20;    // Ignore dock position changes < this px
static const int   POS_RESET_THRESHOLD    = 100;   // Full lock reset if change > this px
static const int   RUNNING_STATE_CHECK_MS = 500;   // Period between running-app window scans
static bool        ENABLE_AUTOHIDE_SYNC   = false; // Sync dock visibility with taskbar auto-hide
static const int   SHAKE_DURATION_MS      = 400;   // Duration of limit-hit shake animation
static const float SHAKE_AMPLITUDE_PX     = 3.f;   // Peak pixel displacement during shake

static const float ANIM_LERP_FACTOR       = 0.24f; // Ease-out lerp  --  tuned for Windows-native feel
static const float ANIM_MOMENTUM_DECAY    = 0.68f; // Strong damping  --  crisp, no overshoot
static const int   ANIM_POS_THRESHOLD     = 2;     // px: ignore dock position micro-changes
static const float ANIM_SNAP_THRESHOLD    = 0.6f;  // px: snap sooner for responsiveness

static const float PIN_FADE_SPEED         = 0.11f; // ~90 ms fade-in  --  fast, native-feeling
static const float ICON_HIDE_FADE_SPEED   = 0.12f; // Fade-OUT rate for overflow icons sliding off the edge (>10 pins). Tuned to finish ~as the one-slot slide completes, so hide reads as a soft dissolve, not a pop.
static const float PIN_SLIDE_OFFSET       = 10.f;  // Entrance slide in pixels (more visible)
static const float ICON_ANIM_SPEED        = 0.32f; // Icon X position lerp speed (smoother reflow)
static const float FRAME_DELTA_SNAP_MS    = 50.f;  // Snap animations on frames slower than this


// Lightweight off-dock unpin feedback (replaces legacy tether/particle layers)

// Interaction settings (user settings; loaded in WhTool_ModInit / WhTool_ModSettingsChanged).
static bool ENABLE_RAPID_UNPIN_ALL     = false;  // "enableRapidUnpinAll": rapid triple-click in the dock zone unpins ALL pins (DEFAULT OFF, opt-in). Fixes accidental wipe from impatiently clicking a slow app.
static bool ENABLE_KEY_GESTURES        = false;  // "enableKeyGestures": bare-key P/U/L triple-tap gestures (DEFAULT OFF, opt-in). The Ctrl+Alt+P hotkey covers pin/unpin without firing while typing.

// User-configurable (clamped in WhTool_ModInit)
static int  MAX_PINNED_APPS      = 5;
static int  BASE_ICON_SIZE       = 33;
static int  BASE_ICON_SPACING    = 12;
static int  DOCK_GAP_FROM_START  = 6;      // Gap between dock right edge and Start (user-configurable, clamp 0..40)
static std::atomic<bool> SHOW_WORKSPACE_DIVIDER{true};  // v2.5.4: boolean toggle (replaces the old int separatorOpacity). Worker LoadSettings writes, UI WM_PAINT reads.

// v2.5.4 (this-round Issue 3): the right-edge separator LINE was removed, so the
// old 0..100 "separatorOpacity" (whose values 1..100 all behaved identically) is
// now a plain on/off toggle for the gold workspace/app divider pill. Pure decision,
// mirrored 1:1 into tests/workspace_divider.h and unit-tested in workspace_divider_test.cpp.
static inline bool ShouldDrawWorkspaceDivider(bool showWorkspaceDivider,
                                              int workspaceSlots, int appCount) {
    return showWorkspaceDivider && workspaceSlots > 0 && appCount > 0;
}
// ENABLE_GLASS_OVERLAY removed: ApplyDockGlassTint / DrawGlassEdge were no-ops
// and the setting description ("whole-dock red tint") was never implemented.
// Shipping a setting that inverts what its label says is worse than no setting.
static bool ENABLE_REORDER       = true;   // Drag-to-reorder within dock
static std::atomic<bool> ENABLE_SCROLL_NAV{true};   // Scroll-wheel navigation across pins. FIX(Issue 1): atomic -- worker LoadSettings writes, UI WM_MOUSEWHEEL reads.
static std::atomic<int> CORNER_ROUNDNESS{40};     // default: small-rounded. 0 = square dock, 100 = fully rounded (DWM). FIX(Issue 1): atomic -- worker writes, UI WM_PAINT reads.
static bool HIDE_DOCK_BORDER     = false;  // "hideDockBorder": ON => colourless DWM window border (no grey outline). Default OFF.
static bool ENABLE_EXPLORER_WORKSPACE_PINS = true;  // User-controlled explorer.exe exclusion
static std::atomic<int>  DOCK_STYLE{1};                // 0 = frosted glass pill, 1 = transparent
static std::atomic<bool> SHOW_TOOLTIPS{true};          // Hover tooltip pill
static std::atomic<bool> g_renderQueued{false};        // Coalesce WM_QPD_RENDER_DOCK posts
static std::atomic<int>  TRIPLE_TAP_WINDOW_MS{300};    // Gesture Guard max gap (150..350 ms)
static std::atomic<int>  FULLSCREEN_BEHAVIOR{0};       // 0 = hide (disabled), 1 = allow (visible)
static std::atomic<int>  CAPTURE_BEHAVIOR{0};          // 0 = exclude (WDA_EXCLUDEFROMCAPTURE), 1 = ignore (WDA_NONE)
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

static std::atomic<int>  MAX_WORKSPACE_PINS{10};
static std::atomic<int>  MAX_PATHS_PER_WORKSPACE{15};
static std::atomic<int>  POPUP_VISIBLE_PATHS{8};
static std::atomic<bool> OPEN_IN_SEPARATE_WINDOWS{false};
static std::atomic<bool> REOPEN_CLOSED_PATHS_ON_FOCUS{false};

static std::atomic<bool> LIMIT_VISIBLE_PINS{false};
static std::atomic<int>  MAX_VISIBLE_APPS_SETTING{10};
static std::atomic<int>  MAX_VISIBLE_WORKSPACES_SETTING{10};
static std::atomic<bool> FOLDER_COLOR_MODE_ORDERED{false};
static std::vector<std::wstring> g_folderPalette; // Guarded by g_cs after init
static int g_safeLeftEdgePx   = 12;  // Screen-bounded left limit (after Widgets + margin)
static std::atomic<int> g_dynamicMaxAppSlots{10}; // Computed per monitor in RefreshTaskbarCache
static std::atomic<int> g_wsPopupScrollStart{0};  // First visible workspace ordinal when limitVisiblePins is ON

static const wchar_t* DEFAULT_FOLDER_PALETTE =
    L"#4F7CAC|#3D5A80|#5B8FB9|#2E6F95|#6B9AC4|#5E60CE|#6D597A|#7B6D8D|#8E7DBE|#52489C|"
    L"#B56576|#C06C84|#A2495E|#9D5C63|#C97B84|#C86D51|#B85D43|#CC7A5B|#A45A49|#B07156|"
    L"#3D9970|#4A8B71|#2F7A6D|#3B8EA5|#529985|#6B8E6E|#4F775A|#768976|#5A7D7C|#687980";

// Dynamic dock-width sizing (app region).
//   * MIN_APP_SLOTS : the dock never shrinks below this many app slots (visual hit area).
//   * MAX_VISIBLE_APP_SLOTS : fallback cap used when LIMIT_VISIBLE_PINS is ON.
static const int MIN_APP_SLOTS        = 5;

// ============================================================
//  REGISTRY / WINDOW CLASS NAMES
// ============================================================
// FIX-C3 (#7): REG_KEY removed -- pinned apps no longer live in HKCU. REG_VALUE
// is now the Windhawk mod-storage value name for the pinned-apps blob.
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
    DRAG_CANDIDATE, // Real drag confirmed (non-dock source), no dock intent yet --
                    // nothing expensive has run: no resolver, no UIA, no OpenProcess,
                    // no icon extraction. Re-checked every frame; only advances to
                    // DRAG_DRAGGING once the cursor actually shows dock intent.
    DRAG_REORDER,   // Dragging a pinned icon within the dock to reorder
    DRAG_DRAGGING,  // Threshold crossed, ghost visible (unpin / external drop)
    DRAG_DROPPED,   // Drop has been processed
    DRAG_CANCELLED  // Drag aborted (no valid source, Escape, etc.)
};
static std::atomic<DragState> g_dragState{DRAG_IDLE};  // FIX #8: written by worker, read by window procs -- atomic to avoid a torn cross-thread read

// ============================================================
//  BOOT STATE
// ============================================================
enum SystemState { STATE_BOOT, STATE_STABILIZING, STATE_STABLE };
static SystemState g_systemState = STATE_BOOT;
// FIX (Issue 2): set when the taskbar is left-aligned (Start hugs the left edge)
// so there is no room for the dock. The dock is hidden and the state machine is
// settled to STATE_STABLE so polling backs off -- instead of the old behaviour
// of returning early every call and spinning in STATE_BOOT at 100 ms forever.
static bool g_layoutUnsupported = false;


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

// Case-insensitive FILE-NAME check (not a whole-path substring match, which
// would wrongly exclude any path that merely contains "explorer.exe" as a
// substring). Empty path is always excluded.
static bool IsExplorerExePath(const std::wstring& path) {
    if (path.empty()) return true;
    return _wcsicmp(PathFindFileNameW(path.c_str()), L"explorer.exe") == 0;
}

static bool IsExcludedApp(const std::wstring& path) {
    if (path.empty()) return true;
    const wchar_t* fileName = PathFindFileNameW(path.c_str());
    for (int i = 0; g_excludeList[i]; ++i) {
        if (ENABLE_EXPLORER_WORKSPACE_PINS &&
            _wcsicmp(g_excludeList[i], L"explorer.exe") == 0)
            continue;
        // Exclusion list holds bare exe names, so compare the file name only.
        if (_wcsicmp(fileName, g_excludeList[i]) == 0)
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
    bool         customNamed = false;
    std::wstring colorHex;
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
    std::wstring workspaceSig;
    std::wstring colorHex;
    std::wstring renderedColorHex;
    std::vector<std::wstring> workspacePaths;
    std::wstring displayName;
    bool         customNamed = false;
    HICON        icon        = NULL;
    Gdiplus::Bitmap* cachedBmp = nullptr;     // Cached 32bpp PARGB icon for crisp Bicubic draw
    Gdiplus::Bitmap* prevCachedBmp = nullptr; // Previous icon bitmap during smooth color crossfade
    float        colorAnimProgress = 1.0f;    // 0..1 QPC-driven color transition progress
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
static ULONG_PTR g_gdiplusToken = 0;
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
// Issue 2 (settings race): WhTool_ModSettingsChanged runs on an arbitrary Windhawk
// thread. It must NOT read settings or write worker-owned geometry globals directly
// (that raced the worker's reads/writes). It only raises this flag; the worker (the
// sole owner of these globals) re-reads every setting via LoadSettings and re-applies
// geometry on its next poll. Atomic so the cross-thread handoff is well-defined.
static std::atomic<bool> g_settingsReloadPending{false};

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
static LARGE_INTEGER g_perfFreq      = {};    // ticks/sec, filled once in WhTool_ModInit
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
static std::atomic<int> g_hoverIndex{-1};  // FIX #8: worker writes, window procs read -- atomic
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
// Stage-1 lightweight source classification, recorded at mouse-down for a
// non-dock press (WindowFromPoint only -- see GetRealWindowFromPoint). NOT a
// resolved identity: no path, no process, no icon. Consumed once, at the
// Stage 3/4 dock-intent gate in DRAG_CANDIDATE, to decide whether the source
// is even worth the Stage 5 resolve.
static HWND    g_dragCandidateWindow = NULL;

// Rapid-click state
static int     g_rapidClickCount = 0;
static DWORD   g_rapidClickStart = 0;
static int     g_rapidClickIndex = -1;

// ---- Keyboard triple-tap gesture (worker-thread polled) ----
// Tap the P key 3x quickly -> PIN the focused app; tap U key 3x quickly ->
// UNPIN it. Rising-edge detected so a held key counts as one tap. Reset if the
// taps are spread out beyond KEY_TAP_WINDOW_MS.
static const int   KEY_TAP_THRESHOLD   = 3;
static HHOOK       g_kbHook            = NULL;
static std::atomic<wchar_t> g_pendingKeyGesture{0};
static std::atomic<bool> g_gestureResetRequested{false};
static DWORD       g_tapTargetVk       = 0;
static int         g_tapCount          = 0;
static ULONGLONG   g_tapLastMs         = 0;
static DWORD       g_tapDownVk         = 0;  // Suppress auto-repeat while held

static void ResetGestureGuard() {
    g_gestureResetRequested.store(true);
}

static LRESULT CALLBACK LowLevelKeyboardGestureProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (g_gestureResetRequested.exchange(false)) {
        g_tapTargetVk = 0;
        g_tapCount    = 0;
        g_tapLastMs   = 0;
    }
    if (nCode == HC_ACTION && ENABLE_KEY_GESTURES) {
        const KBDLLHOOKSTRUCT* kb = (const KBDLLHOOKSTRUCT*)lParam;
        if (kb && !(kb->flags & LLKHF_INJECTED)) {
            DWORD vk = kb->vkCode;
            if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                if (vk == g_tapDownVk) g_tapDownVk = 0;
            } else if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                // Ignore auto-repeat while key is held down
                if (vk == g_tapDownVk)
                    return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
                g_tapDownVk = vk;

                bool modHeld =
                    (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                    (GetAsyncKeyState(VK_MENU)    & 0x8000) ||
                    (GetAsyncKeyState(VK_SHIFT)   & 0x8000) ||
                    (GetAsyncKeyState(VK_LWIN)    & 0x8000) ||
                    (GetAsyncKeyState(VK_RWIN)    & 0x8000);

                if (modHeld || (vk != 'P' && vk != 'U' && vk != 'L')) {
                    // Immediate reset on any modifier, Space, Enter, Backspace, Tab, or other key
                    g_tapTargetVk = 0;
                    g_tapCount    = 0;
                    g_tapLastMs   = 0;
                } else {
                    ULONGLONG now64 = GetTickCount64();
                    if (g_tapTargetVk != vk || (now64 - g_tapLastMs) > (ULONGLONG)TRIPLE_TAP_WINDOW_MS) {
                        g_tapTargetVk = vk;
                        g_tapCount    = 1;
                    } else {
                        g_tapCount++;
                    }
                    g_tapLastMs = now64;
                    if (g_tapCount >= KEY_TAP_THRESHOLD) {
                        g_pendingKeyGesture.store((wchar_t)vk);
                        g_tapTargetVk = 0;
                        g_tapCount    = 0;
                        g_tapLastMs   = 0;
                    }
                }
            }
        }
    }
    return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
}

static HINSTANCE GetModHInstance();

static void SyncKeyboardGestureHookOnUiThread() {
    if (ENABLE_KEY_GESTURES && !g_kbHook) {
        g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardGestureProc, GetModHInstance(), 0);
    } else if (!ENABLE_KEY_GESTURES && g_kbHook) {
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = NULL;
        ResetGestureGuard();
        g_tapDownVk = 0;
    }
}

// ---- Icon lock (triple-tap L) ----
// Triple-tap the L key to LOCK the dock, tap it 3x again to UNLOCK. While
// locked, all *gesture* unpins are refused -- triple-tap U, the rapid-click
// unpin-all, double-right-click unpin, and the hotkey's unpin action -- so an
// accidental gesture can never drop a pin. A deliberate drag-off that BREAKS
// the rope still unpins (that is an explicit, hard-to-trigger-by-accident act),
// as does pinning new apps and reordering. Single-writer (worker thread) /
// multi-reader bool, safe lock-free on x86-64 like the other per-frame flags.
static std::atomic<bool> g_iconsLocked{false};  // FIX #8: toggled by worker (triple-tap L), read by window procs -- atomic

// INTERACTIVE lock glow. Rather than glowing the whole time the dock is locked
// (which made a locked dock look permanently "highlighted"), the gold edge only
// FLASHES when the user does something lock-related -- attempts a blocked
// gesture unpin, or toggles the lock -- then fades out, so a locked dock looks
// completely natural at rest. g_lockGlowStart = tick the current flash began
// (0 = no flash / idle); the flash runs for LOCK_GLOW_MS.
static std::atomic<DWORD>   g_lockGlowStart{0};
static const int LOCK_GLOW_MS  = 750;

// ---- Cinematic lock/unlock FLASH -- pure decision logic + layer state --------
// The flash is a brief, premium bloom shown on the lock TOGGLE (and on a refused
// gesture-unpin while locked). It renders on its OWN per-pixel-alpha layered
// window (see EnsureLockGlowSurface / RenderLockGlow) so it can paint a true
// soft glow that spills OUTSIDE the dock -- something the dock's colour-key
// overlay (opaque for any non-key pixel) can never do. Two visually distinct
// kinds, chosen from the lock state at trigger time:
//   LOCKGLOW_SEAL     warm GOLD bloom that CONVERGES onto the dock edge  (LOCK,
//                     and the "denied" pulse of a blocked gesture-unpin).
//   LOCKGLOW_RELEASE  cool GREEN ripple that EXPANDS outward from the edge (UNLOCK).
// The colour ramp, alpha envelope and ring geometry below are mirrored 1:1 in
// tests/lock_glow.h and unit-tested by tests/lock_glow_test.cpp.
enum LockGlowKind { LOCKGLOW_SEAL, LOCKGLOW_RELEASE, LOCKGLOW_LIMIT };

// TriggerLockGlow() runs AFTER the caller flips g_iconsLocked, so the NEW locked
// state picks the effect: locked => gold seal-in, unlocked => green release.
static inline LockGlowKind LockGlowKindFromLocked(bool locked) {
    return locked ? LOCKGLOW_SEAL : LOCKGLOW_RELEASE;
}
// Flash alpha envelope over t in [0,1]: fast smoothstep rise to an early peak
// (~22%), then a gentle ease-out to 0 (0 at both ends). Returns 0..1.
static inline float LockGlowAlpha01(float t) {
    if (t <= 0.0f || t >= 1.0f) return 0.0f;
    const float peak = 0.22f;
    if (t < peak) { float r = t / peak; return r * r * (3.0f - 2.0f * r); }
    float f = (t - peak) / (1.0f - peak); float e = 1.0f - f; return e * e;
}
struct LockGlowRGB { int r, g, b; };
// Colour ramp per kind, brightening toward the peak (p in [0,1]).
static inline LockGlowRGB LockGlowColor(LockGlowKind kind, float p) {
    if (p < 0.0f) p = 0.0f; else if (p > 1.0f) p = 1.0f;
    if (kind == LOCKGLOW_SEAL)   // amber -> bright gold
        return LockGlowRGB{ (int)(184 + (255 - 184) * p), (int)(134 + (205 - 134) * p), (int)(11 + (90 - 11) * p) };
    if (kind == LOCKGLOW_LIMIT)  // deep red -> bright red (dock full / pin limit)
        return LockGlowRGB{ (int)(120 + (255 - 120) * p), (int)(20 + (70 - 20) * p), (int)(20 + (70 - 20) * p) };
    // RELEASE: deep green -> bright green
    return LockGlowRGB{ (int)(20 + (80 - 20) * p), (int)(120 + (235 - 120) * p), (int)(70 + (140 - 70) * p) };
}
// --- Confinement + render mode (mirrored 1:1 in tests/lock_glow.h) ----------
// The glow is CONFINED to the dock's own rectangle: it never spills outside and
// never draws a fabricated outline. One user setting picks the look:
//   SWEEP (ON)  -- loading-style bar that fills the dock LEFT -> RIGHT.
//   EDGE  (OFF, default) -- brief highlight on the dock's OWN edge, then calm.
enum LockGlowMode { LOCKGLOW_MODE_EDGE, LOCKGLOW_MODE_SWEEP };
// enableLockAnimation is OFF by default -> edge-only; ON -> full sweep.
static inline LockGlowMode LockGlowModeFromSetting(bool animationEnabled) {
    return animationEnabled ? LOCKGLOW_MODE_SWEEP : LOCKGLOW_MODE_EDGE;
}
// Left->right sweep front in [0,1] across the dock width (smoothstep "loading"
// feel, monotonic non-decreasing).
static inline float LockGlowSweepX01(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    return t * t * (3.0f - 2.0f * t);
}
// --- Glow remover / lifecycle guard (mirrored 1:1 in tests/lock_glow.h) ------
// The glow rides the dock's REAL rectangle (g_cachedDockRect). If the dock
// stops showing mid-flash it must be torn down at once or it is left painting
// over empty space. DockHiddenForGlow mirrors RepositionOverlay's three hard
// hide branches (fullscreen/snip, unsupported layout, zero-width boot race).
static inline bool DockHiddenForGlow(bool fullscreenActive, bool layoutUnsupported,
                                     int dockLocalW) {
    return fullscreenActive || layoutUnsupported || dockLocalW <= 0;
}
// Remove the in-flight glow this frame? Nothing to remove when idle; otherwise
// remove NOW if the dock is hidden (the stuck-glow fix), or once the flash has
// run its full duration.
static inline bool LockGlowShouldTeardown(bool glowActive, bool dockHidden,
                                          unsigned elapsedMs, unsigned durationMs) {
    if (!glowActive) return false;
    if (dockHidden)  return true;
    return elapsedMs >= durationMs;
}

// --- Dock-inactive gate (v2.5.3, mirrored 1:1 in tests/dock_active_gate.h) ---
// Is the dock ACTIVE (on screen) and therefore allowed to run any drag / resolve
// / dock-zone / pin-unpin / lock-glow work this frame? The overlay is suppressed
// (kept hidden) for several reasons -- a fullscreen/exclusive app or secure snip
// overlay, an unsupported (left-aligned) layout, a missing taskbar while Explorer
// restarts, or geometry that has not resolved yet (zero width / empty cached
// rect). A hidden dock must be a NON-INTERACTIVE dock: if ANY suppressor is set
// the whole drag pipeline is skipped for the frame, so nothing resolves sources,
// hit-tests dock zones, or evaluates pin/unpin against stale cached geometry.
static inline bool DockInteractionAllowed(bool fullscreenActive,
                                          bool layoutUnsupported,
                                          bool haveCachedTaskbar,
                                          int  dockLocalW,
                                          bool cachedRectValid,
                                          bool autoHideHidden) {
    if (fullscreenActive)   return false;
    if (layoutUnsupported)  return false;
    if (!haveCachedTaskbar) return false;
    if (dockLocalW <= 0)    return false;
    if (!cachedRectValid)   return false;
    if (autoHideHidden)     return false;   // Issue 3: auto-hidden off screen -> dock not on screen
    return true;
}

// Live state + the per-pixel-alpha glow layer's handles (mirrors the tether/ghost).
// v2.5.3 thread-safety fix -- g_lockGlowKind / g_lockGlowMode / g_lockGlowStart
// are written both from the worker loop AND from the UI thread's OverlayProc
// (TriggerLockGlow on a keyboard/hotkey gesture) and read by the worker's
// RenderLockGlow. Plain scalar/enum globals shared across threads are a data
// RACE (undefined behaviour) no matter how "atomic" a single load/store happens
// to be on x86-64, so all three are now std::atomic. The intended handoff still
// holds -- a trigger publishes kind + mode BEFORE g_lockGlowStart (the "a flash
// is live" gate), and RenderLockGlow reads g_lockGlowStart FIRST -- and with
// seq-cst atomics that ordering is now guaranteed, not merely assumed. The layer
// WINDOW itself is never touched cross-thread: it is created and destroyed only
// on the worker (its sole owner).
static std::atomic<LockGlowKind> g_lockGlowKind{LOCKGLOW_SEAL};  // effect chosen for the current flash
static std::atomic<LockGlowMode> g_lockGlowMode{LOCKGLOW_MODE_EDGE};  // sweep (setting ON) vs edge-only (OFF, default)
static bool         g_dragGlowHold = false;  // drag-to-pin glow is a SUSTAINED hold (not a timed flash) while the icon is held in the drop zone
static bool    ENABLE_LOCK_ANIMATION = false;        // "enableLockAnimation" user setting (default OFF)
// Lock/drag glow now renders directly on the ULW_ALPHA dock surface (zero Z-order fighting).

// Visual feedback
static std::atomic<bool>  g_shakeActive{false};  // Dock shake on pin-limit hit. FIX(Issue 3): atomic -- TriggerLimitFlash writes, UI WM_PAINT reads/clears.
static std::atomic<DWORD> g_shakeStart{0};
// (g_limitFlash* removed: pin-limit feedback is now a RED lock-glow bloom, not the red right-edge line.)

// Taskbar auto-hide integration
static bool    g_taskbarAutoHide  = false;  // True when taskbar has ABS_AUTOHIDE set
static int     g_autoHideMiss     = 0;      // Hysteresis counter for auto-hide slide (anti-flicker)

// Fullscreen-app suppression: the dock is HWND_TOPMOST, so like the taskbar it
// must fully hide while a fullscreen app / exclusive presentation / secure snip
// overlay owns the screen (otherwise it floats on top of games, F11 video, and
// the Snipping Tool clip). Sampled each poll by UpdateFullscreenState(); the
// pure decision + hysteresis are mirrored in tests/fullscreen_suppress.h.
static bool    g_fullscreenActive  = false; // latched result the worker acts on
static int     g_fullscreenMiss    = 0;     // consecutive "clear" samples (anti-flicker restore)
static const int FULLSCREEN_RESTORE_FRAMES = 2;  // clear samples required before the dock reappears
// --- pure decision, mirrored 1:1 in tests/fullscreen_suppress.h ------------
static inline bool ShouldSuppressForFullscreen(bool foregroundCoversMonitor,
                                               bool foregroundIsShell,
                                               bool shellReportsFullscreen) {
    if (shellReportsFullscreen) return true;              // exclusive D3D / presentation
    return foregroundCoversMonitor && !foregroundIsShell; // borderless / F11 fullscreen app
}
// Hide at once on detection; only restore after N consecutive clear samples so
// an app's own mode-switch frame can't blink the dock in and back out. The
// restore delay only EXTENDS an existing hide (prevActive): when the dock was
// not already hidden and nothing is fullscreen, never manufacture a hide -- so
// a fresh boot (missCount=0) and opening Start/Search don't blink the dock.
static inline bool FullscreenHideDecision(bool suppressNow, bool prevActive,
                                          int missCount, int restoreDelayFrames) {
    if (suppressNow) return true;
    if (!prevActive) return false;
    return missCount < restoreDelayFrames;
}
// Edge classification of the latched g_fullscreenActive flag across two poll
// samples, so UpdateFullscreenState() logs EXACTLY ONE line when the dock
// starts hiding and one when it clears -- never per-frame. Mirrored 1:1 in
// tests/fullscreen_suppress.h.
enum FullscreenTransition { FS_NONE, FS_ENTERED, FS_CLEARED };
static inline FullscreenTransition
FullscreenTransitionEvent(bool prevActive, bool nowActive) {
    if (nowActive && !prevActive) return FS_ENTERED;   // just started hiding
    if (!nowActive && prevActive) return FS_CLEARED;    // just cleared -> restore
    return FS_NONE;                                     // steady state, no edge
}
static std::atomic<int> g_appScrollStart{0};      // FIX #8 atomic. First VISIBLE app-pin ordinal. When more apps are pinned than MAX_VISIBLE_APP_SLOTS, the app region becomes a scrollable viewport; this is the left edge of that window (wheel nav slides it).
static std::atomic<DWORD> g_scrollNavUntil{0};      // FIX #8 atomic. Scroll-lock deadline (GetTickCount) protecting the scrolled highlight
static POINT   g_scrollNavPt      = {};     // Cursor pos when the scroll-lock was armed; real movement past it releases the lock
static const DWORD SCROLL_NAV_LOCK_MS = 700; // How long a wheel-selected highlight is protected from stray WM_MOUSEMOVE

// Pin/unpin hotkey
static const UINT HOTKEY_PIN_ID        = 1777;        // WM_HOTKEY wParam identifier
// FIX (Issue 6): posted to the overlay window (from WhTool_ModSettingsChanged) so the
// global hotkey is re-registered on its OWNING thread when the user changes the
// modifier/key setting -- RegisterHotKey/UnregisterHotKey must run on the thread
// that created g_overlayWnd, so a settings-thread call would silently no-op.
static const UINT WM_QPD_REREGISTER_HOTKEY  = WM_APP + 7;
static const UINT WM_QPD_RENDER_DOCK        = WM_APP + 8;
static const UINT WM_QPD_WSPOPUP_OPEN       = WM_APP + 9;
static const UINT WM_QPD_WSPOPUP_CLOSE      = WM_APP + 10;
static std::atomic<UINT> g_hotkeyMods{MOD_CONTROL | MOD_ALT};  // FIX(Issue 1): atomic -- worker LoadSettings writes, UI hotkey re-register reads.
static std::atomic<UINT> g_hotkeyKey{'P'};               // Default: Ctrl+Alt+P. FIX(v10): atomic -- worker LoadSettings writes, UI hotkey re-register reads.

// Hover candidate  --  pre-sampled resolver result for anti-flicker stability

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
//  GLOBALS  --  pinned apps list
// ============================================================
static std::vector<PinnedApp> g_pinnedApps;

// ============================================================
//  GLOBALS  --  cached GDI objects (created once, freed in WhTool_ModUninit)
// ============================================================
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
// FIX-C1 (#1/#2/#8): dedicated UI thread that OWNS all top-level window/class/
// hotkey lifecycle and runs the message pump. Windows are created, pumped, and
// destroyed on this ONE thread (Windhawk's lifecycle callbacks run on an
// arbitrary thread with no message pump, which broke dispatch + teardown).
static HANDLE g_uiThread     = NULL;
static DWORD  g_uiThreadId   = 0;
static HANDLE g_uiReadyEvent = NULL;  // signaled by the UI thread once windows exist (or creation failed)
static bool   g_uiInitOk     = false; // set by the UI thread; read by WhTool_ModInit after the ready event
// FIX-B2 (#6) + review #4 (Bug A): LaunchWorkspaceAsync's threads are tracked so
// WhTool_ModUninit can join EVERY outstanding one before the mod image unloads. The
// old single-slot design closed+overwrote a still-running thread's handle when a
// second launch overlapped it, dropping the join point -- that thread then ran
// mod code after FreeLibrary (return address in the unmapped image) and crashed
// the host. We now keep a list and only reap handles that have already finished.
// g_launchWorkspaceStop lets the worker bail out early if the mod is being torn
// down mid-launch. g_launchCs guards the list (UI + worker threads both launch).
static std::vector<HANDLE> g_launchWorkspaceThreads;
static CRITICAL_SECTION    g_launchCs;
static bool                g_launchCsInit = false;
static CRITICAL_SECTION    g_restoreExecCs;
static bool                g_restoreExecCsInit = false;
static std::atomic<bool>   g_launchWorkspaceStop{false};
// FIX-B3 (#3): the mod's own module handle. Window classes must be registered
// with (and unregistered against) THIS handle, not GetModuleHandleW(NULL) which
// returns explorer.exe. Populated in WhTool_ModInit via GetModHInstance().
static HINSTANCE g_hModule = NULL;
// g_animationActive removed  --  was written but never read. g_anyAnimationActive is the live flag.

// FIX-B3 (#3): resolve the HINSTANCE of the mod DLL itself (the module that
// contains this code), regardless of which process/thread we run in. Cached in
// g_hModule. Used as the hInstance for every RegisterClassExW / CreateWindowExW /
// UnregisterClassW that belongs to this mod's own window classes.
static HINSTANCE GetModHInstance() {
    if (!g_hModule) {
        GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCWSTR)&GetModHInstance, &g_hModule);
    }
    return g_hModule;
}

// FIX-A7 (#4): g_winEventHook / WinEventProc removed. The hook could never fire
// and the worker poll loop already drives every geometry refresh; see the note
// in WhTool_ModInit.

// ============================================================
//  DEBUG LOGGING
//  All logging goes straight to Windhawk's own Wh_Log, which is auto-prefixed
//  with the mod name, formats printf-style, and is gated by the "Logging
//  enabled" toggle in the Windhawk UI (a cheap check when off). No in-mod
//  logging layer or verbosity gate is used.
// ============================================================

// ============================================================
//  STRING-SETTING MAPPERS
//  Windhawk only renders a $options dropdown for STRING-valued
//  settings, so these settings are stored as string tokens and
//  mapped back to the internal numeric representation here.
//  (Fix for review item: "$options on numeric settings won't
//   produce a dropdown".)
// ============================================================
// Note: Wh_GetStringSetting never returns NULL -- it returns L"" when the key
// is absent. The if (v) guards that were here were therefore dead. Use a local
// std::wstring to get automatic lifetime instead of manual Wh_FreeStringSetting.
static std::wstring ReadStringSettingWithFallback(PCWSTR nestedKey, PCWSTR legacyKey) {
    PCWSTR raw = Wh_GetStringSetting(nestedKey);
    std::wstring v(raw ? raw : L"");
    Wh_FreeStringSetting(raw);
    if (v.empty() && legacyKey) {
        PCWSTR leg = Wh_GetStringSetting(legacyKey);
        v = (leg ? leg : L"");
        Wh_FreeStringSetting(leg);
    }
    return v;
}

static int ReadIntSettingWithFallback(PCWSTR nestedKey, PCWSTR legacyKey, int defVal) {
    int val = Wh_GetIntSetting(nestedKey, INT_MIN);
    if (val != INT_MIN) return val;
    if (legacyKey) {
        int leg = Wh_GetIntSetting(legacyKey, INT_MIN);
        if (leg != INT_MIN) return leg;
    }
    return defVal;
}

static UINT LoadHotkeyModifiersSetting() {
    std::wstring v = ReadStringSettingWithFallback(L"shortcuts.hotkeyModifiers", L"hotkeyModifiers");
    if      (_wcsicmp(v.c_str(), L"none")      == 0) return 0;
    else if (_wcsicmp(v.c_str(), L"alt")       == 0) return MOD_ALT;
    else if (_wcsicmp(v.c_str(), L"ctrl")      == 0) return MOD_CONTROL;
    else if (_wcsicmp(v.c_str(), L"shift")     == 0) return MOD_SHIFT;
    else if (_wcsicmp(v.c_str(), L"ctrlShift") == 0) return MOD_CONTROL | MOD_SHIFT;
    else if (_wcsicmp(v.c_str(), L"altShift")  == 0) return MOD_ALT | MOD_SHIFT;
    else if (_wcsicmp(v.c_str(), L"win")       == 0) return MOD_WIN;
    else if (_wcsicmp(v.c_str(), L"winAlt")    == 0) return MOD_WIN | MOD_ALT;
    else if (_wcsicmp(v.c_str(), L"winCtrl")   == 0) return MOD_WIN | MOD_CONTROL;
    return MOD_CONTROL | MOD_ALT;
}

static UINT LoadHotkeyKeySetting() {
    std::wstring v = ReadStringSettingWithFallback(L"shortcuts.hotkeyKey", L"hotkeyKey");
    while (!v.empty() && iswspace(v.front())) v.erase(v.begin());
    while (!v.empty() && iswspace(v.back()))  v.pop_back();
    if (v.empty() || _wcsicmp(v.c_str(), L"none") == 0) return 0;
    if (v.size() == 1) {
        wchar_t ch = (wchar_t)towupper(v[0]);
        if (ch >= L'A' && ch <= L'Z') return (UINT)ch;
    }
    if ((v[0] == L'F' || v[0] == L'f') && v.size() >= 2) {
        int fn = 0;
        bool allDigits = true;
        for (size_t i = 1; i < v.size(); ++i) {
            if (v[i] < L'0' || v[i] > L'9') { allDigits = false; break; }
            fn = fn * 10 + (int)(v[i] - L'0');
        }
        if (allDigits && fn >= 1 && fn <= 12) return (UINT)(VK_F1 + (fn - 1));
    }
    return 'P';   // default
}

// Logging goes straight to Wh_Log (Windhawk's own "Logging enabled" toggle is the only gate).

// DragTraceLog  --  a genuine drag state transition (never per-frame).
static void DragTraceLog(const wchar_t* event, const wchar_t* detail = L"") {
    if (detail && detail[0]) Wh_Log(L"[DRAG] %s | %s", event, detail);
    else                     Wh_Log(L"[DRAG] %s", event);
}

// ============================================================
//  FORWARD DECLARATIONS
// ============================================================
enum WsPopupState {
    WS_POPUP_CLOSED = 0,
    WS_POPUP_OPEN_HOVER,
    WS_POPUP_OPEN_HOLD,
    WS_POPUP_CLOSING
};

static HWND              g_tooltipWnd     = NULL;
static HWND              g_wsPopupWnd     = NULL;
static std::atomic<int>  g_wsPopupState{WS_POPUP_CLOSED};
static std::atomic<int>  g_wsPopupPinIdx{-1};

struct ExplorerTab {
    IWebBrowserApp* app = NULL;
    HWND hwnd = NULL;
    std::wstring folder;
};

static std::wstring             NormalizeForCompare(std::wstring p);
static void                     ReleaseExplorerTabs(std::vector<ExplorerTab>& tabs);
static std::vector<ExplorerTab> EnumerateExplorerTabs(HWND hwndFilter = NULL, bool includeVirtual = false);
static bool                     FocusExplorerTabByFolder(HWND hwnd, const std::wstring& folder);
void  SavePinnedApps();
void  LoadPinnedApps();
void  RepositionOverlay();
RECT  GetIconRectLocal(int index, int totalCount);
static int CountPinsByType(PinType type);   // fwd decl: used by RefreshTaskbarCache (defined later)
bool  IsPinned(const std::wstring& path);
void  PinApp(const std::wstring& path);
void  PinWorkspace(HWND explorerHwnd);
void  UnpinAppByIndex(int i);
void  UnpinAllApps();

static void     GhostCleanup();
static void     GhostDragReset();
static void     TriggerLockGlowKind(LockGlowKind kind);  // fwd: explicit-colour glow (red LIMIT dock-full / green RELEASE drop-entry); defined with TriggerLockGlow
static bool     IsInDockZone(POINT screenPt, int tolerancePx = 10);
static void     UpdateAutoHideState();
static bool     IsNearDockZone(POINT screenPt);
static bool     IsCursorInDockOrTaskbarRegion(POINT screenPt);
static int      HitTestIcon(POINT screenPt);
static void     ForceForegroundWindow(HWND hwnd);  // fwd: defined near SmartLaunch, used earlier
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
static void     LoadSettings();   // fwd: worker calls this to apply a marshaled settings reload (Issue 2)
static void     RefreshDpiScale();
static void     TriggerLimitFlash();
static void     ValidateAndCleanPinnedList();
static bool     IsSystemWindow(HWND hwnd);
static bool     IsTrueSystemWindow(HWND hwnd);
static int      CalculateReorderSlot(POINT screenPt, int n);
static void     UpdateReorderPositions();
static void     CommitReorder();
static void     UpdateRunningState();
static bool     IsCursorOverTaskbar(POINT pt);
static HWND     GetRealWindowFromPoint(POINT pt);
static bool     GetStartButtonLeftEdge(HWND taskbar, const RECT& tbRect, LONG* outLeft);

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK InputOwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD   WINAPI   WorkerThread(LPVOID);

// Zero-rejection resolver
static std::wstring ResolveDragSourceZeroRejection(POINT pt);
static std::wstring Resolver_Layer1_UIHit(POINT pt);
static std::wstring Resolver_Layer2_TaskbarIntelligence(POINT pt);
static std::wstring Resolver_Layer3_ProcessFallback(POINT pt);
static bool     IsExplorerWorkspaceDragSource(POINT pt, HWND* outExplorerHwnd = NULL);
static bool     CaptureWorkspaceSnapshot(WorkspaceSnapshot& snapshot, HWND ownerHwnd = NULL, bool allTabsInWindow = false);
static bool     SaveWorkspaceSnapshot(const WorkspaceSnapshot& snapshot);

// ============================================================
//  SINGLE-INSTANCE
// ============================================================
// As a TOOL MOD this runs in a dedicated windhawk.exe launched with -tool-mod
// taskbar-quick-pin. The official tool launcher guarantees a single instance
// via its own mutex (windhawk-tool-mod_<id>), so the old Explorer ownership
// machinery (which explorer.exe owns Shell_TrayWnd) is gone. Dock visibility
// is gated purely on valid geometry (see RepositionOverlay).

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

static bool GetStartButtonLeftEdge(HWND taskbar, const RECT& tbRect, LONG* outLeft) {
    // Fast path: direct child lookup
    HWND startDirect = FindWindowExW(taskbar, NULL, L"Start", NULL);
    if (startDirect) {
        RECT sr = {};
        GetWindowRect(startDirect, &sr);
        if (sr.left > tbRect.left && sr.left < tbRect.right) {
            if (outLeft) *outLeft = sr.left;
            return true;
        }
    }

    // Enumerate all taskbar children to find Start
    StartFindCtx ctx = {};
    ctx.taskbar      = taskbar;
    ctx.taskbarRect  = tbRect;
    ctx.bestLeft     = 0;
    ctx.found        = false;
    EnumChildWindows(taskbar, FindStartCallback, (LPARAM)&ctx);

    // FIX (Issue 1B): report Start explicitly as not-found instead of
    // substituting a fabricated tbRect.left + width/5 estimate. Callers treat a
    // false result as QP_LAYOUT_PENDING (keep booting) rather than committing to
    // a layout for a Start position that was never actually resolved.
    // (Layout decision mirrored + unit-tested in tests/start_edge_probe.h.)
    if (ctx.found) {
        if (outLeft) *outLeft = ctx.bestLeft;
        return true;
    }
    return false;
}

// ============================================================
//  TASKBAR GEOMETRY CHANGE DETECTION
// ============================================================
static bool HasTaskbarGeometryChanged() {
    HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
    // TOOL-MODE lifecycle (spec 2): the tool process outlives Explorer. A gone
    // taskbar is a change while a stale dock still exists, so RefreshTaskbarCache
    // can tear it down; and a brand-new Shell_TrayWnd handle is a change even if
    // its rectangle matches the old one.
    // (Mirrors DecideTaskbarChanged in tests/taskbar_lifecycle.h.)
    if (!tb) return (g_cachedTaskbar != NULL) || (g_dockLocalW > 0);
    if (tb != g_cachedTaskbar) return true;

    RECT tbr = {};
    GetWindowRect(tb, &tbr);
    if (tbr.left   != g_lastTBRect.left   || tbr.top    != g_lastTBRect.top  ||
        tbr.right  != g_lastTBRect.right  || tbr.bottom != g_lastTBRect.bottom)
        return true;

    // v2.5.3: probe Start explicitly. A Start button that no longer resolves --
    // Shell_TrayWnd still present but Start temporarily gone during a taskbar
    // rebuild/startup -- is itself a geometry change WHEN a dock is currently
    // placed, so RefreshTaskbarCache runs and tears the now-stale dock down
    // instead of leaving it anchored to a Start edge that no longer exists. A
    // resolved Start that simply MOVED is the ordinary tracked-field change.
    // (Mirrors StartLossIsChange in tests/taskbar_lifecycle.h.)
    LONG startLeft = 0;
    bool startResolved = GetStartButtonLeftEdge(tb, tbr, &startLeft);
    if (!startResolved && g_dockLocalW > 0) return true;          // StartLossIsChange
    if (startResolved && startLeft != g_lastStartLeft) return true;

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
// ---- Taskbar layout decision (refinement Issue 2) --------------------------
// Left-aligned taskbars (Win11 "Align: Left", all Win10) put Start at ~tbr.left
// + 16, so the dock -- which anchors to the LEFT of Start -- has no room. The
// old code bailed with `if (startLeft <= tbr.left + 50) return;` BEFORE the
// state machine ran, so g_systemState never left STATE_BOOT: the dock was
// clamped over Start (eating clicks) and the worker polled at 100 ms forever.
//
// This helper turns that single early-return into three explicit outcomes so
// the caller can ALWAYS settle the state machine instead of spinning:
//   QP_LAYOUT_OK          -> room left of Start; place the dock normally.
//   QP_LAYOUT_UNSUPPORTED -> left-aligned / no room; hide dock, go idle.
//   QP_LAYOUT_PENDING     -> geometry not trustworthy yet; keep booting.
// (Mirrors DecideTaskbarLayout in tests/taskbar_layout.h, which unit-tests this
// exact decision.)
enum QpLayoutDecision { QP_LAYOUT_PENDING = 0, QP_LAYOUT_OK, QP_LAYOUT_UNSUPPORTED };
static inline QpLayoutDecision QpDecideTaskbarLayout(long tbrLeft, long tbrRight,
                                                     long startLeft,
                                                     int dockGapPx, int dockWidth,
                                                     int minRoom) {
    if (tbrRight <= tbrLeft) return QP_LAYOUT_PENDING;
    // startLeft strictly inside taskbar bounds required for a trustworthy probe:
    //   >= tbrRight: nonsensical read -- probe failed, keep retrying (PENDING).
    //   <= tbrLeft:  Start hugs the left edge -- this IS the left-aligned layout;
    //                it's a confirmed bad layout, not a missing probe (UNSUPPORTED).
    if (startLeft >= tbrRight) return QP_LAYOUT_PENDING;
    if (startLeft <= tbrLeft)  return QP_LAYOUT_UNSUPPORTED;
    if (dockGapPx < 0) dockGapPx = 0;
    long room = (startLeft - (long)dockGapPx) - tbrLeft;
    if (room < 0) room = 0;
    int need = dockWidth < minRoom ? dockWidth : minRoom;
    if (need < 1) need = 1;
    return (room >= need) ? QP_LAYOUT_OK : QP_LAYOUT_UNSUPPORTED;
}

// Detect the rightmost edge of any left-anchored taskbar widget (e.g. Win11 Widgets)
// so screen-bounded dock growth stops cleanly before overlapping it.
struct LeftWidgetCtx { LONG tbLeft; LONG tbMid; LONG maxRight; };
static BOOL CALLBACK LeftWidgetEnumProc(HWND child, LPARAM lp) {
    if (!IsWindowVisible(child)) return TRUE;
    LeftWidgetCtx* c = (LeftWidgetCtx*)lp;
    wchar_t cls[128] = {};
    GetClassNameW(child, cls, 128);
    if (wcsstr(cls, L"AugmentedEntryPoint") != NULL || wcsstr(cls, L"Widgets") != NULL) {
        RECT r = {};
        if (GetWindowRect(child, &r) && r.right > c->tbLeft && r.right < c->tbMid) {
            if (r.right > c->maxRight) c->maxRight = r.right;
        }
    }
    return TRUE;
}

static int GetTaskbarSafeLeftEdge(HWND taskbar, const RECT& tbRect) {
    UINT dpi = taskbar ? GetDpiForWindow(taskbar) : 96u;
    if (dpi == 0) dpi = 96;
    int safeMargin = (int)lroundf(12.0f * (dpi / 96.0f));
    int safeLeft   = (int)tbRect.left + safeMargin;
    if (!taskbar) return safeLeft;

    LONG tbMid = tbRect.left + (tbRect.right - tbRect.left) / 2;
    LeftWidgetCtx ctx = { tbRect.left, tbMid, safeLeft };
    EnumChildWindows(taskbar, LeftWidgetEnumProc, (LPARAM)&ctx);

    return (ctx.maxRight > safeLeft) ? (int)(ctx.maxRight + safeMargin) : safeLeft;
}

static inline int GetVisibleAppSlots(int appCount) {
    int cap = LIMIT_VISIBLE_PINS.load()
            ? std::max(1, MAX_VISIBLE_APPS_SETTING.load())
            : std::max(MIN_APP_SLOTS, g_dynamicMaxAppSlots.load());
    int vis = appCount;
    if (vis > cap) vis = cap;
    if (vis < 1) vis = 1;
    return vis;
}

static inline int GetVisibleWorkspaceSlots(int wsCount) {
    int cap = MAX_WORKSPACE_PINS.load();
    if (LIMIT_VISIBLE_PINS.load())
        cap = std::min(cap, std::max(1, MAX_VISIBLE_WORKSPACES_SETTING.load()));
    int vis = wsCount;
    if (vis > cap) vis = cap;
    if (vis < 0) vis = 0;
    return vis;
}

static void RefreshTaskbarCache() {
    HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
    // TOOL-MODE lifecycle (spec 2): Explorer can restart while this dedicated
    // tool process stays alive. When the taskbar is gone, hide the dock, drop the
    // cached taskbar + live geometry and reset to STATE_BOOT so a returning
    // Shell_TrayWnd is re-resolved from scratch (fresh Start detection -> centered
    // layout -> dock reappears). Without this the tool keeps stale dock geometry.
    if (!tb) {
        if (g_overlayWnd && IsWindow(g_overlayWnd)) ShowWindow(g_overlayWnd, SW_HIDE);
        if (g_inputWnd   && IsWindow(g_inputWnd))   ShowWindow(g_inputWnd,   SW_HIDE);
        g_cachedTaskbar       = NULL;
        g_cachedDockRect      = {};        // Issue 4: clear stale drop-zone rect (state consistency)
        g_dockWidthLocked     = false;
        g_positionInitialized = false;
        g_dockPositionLocked  = false;
        g_stableGeometryCount = 0;
        g_lastStableWidth     = 0;
        g_fixedDockWidth      = 0;
        g_layoutUnsupported   = false;
        g_systemState         = STATE_BOOT;
        // FIX(v10): reset live dock geometry under g_cs (same lock WM_PAINT holds); after STATE_BOOT so the PART 2i char budget still matches.
        if (g_csInitialized) EnterCriticalSection(&g_cs);
        g_dockLocalW          = 0;
        g_dockLocalH          = 0;
        if (g_csInitialized) LeaveCriticalSection(&g_cs);
        return;
    }

    RECT tbr = {};
    GetWindowRect(tb, &tbr);
    g_cachedTaskbar = tb;
    g_cachedTBRect  = tbr;
    g_lastTBRect    = tbr;

    LONG startLeft = 0;
    if (!GetStartButtonLeftEdge(tb, tbr, &startLeft)) {
        // FIX (v2.5.3): Shell_TrayWnd exists but the Start button could not be
        // resolved (taskbar mid-rebuild / Start temporarily gone). The old code
        // simply returned, leaving the previously cached dock geometry in place
        // -- so the dock kept running against stale state anchored to a Start
        // edge that no longer exists. Invalidate EVERYTHING and drop back to
        // STATE_BOOT, exactly like the taskbar-lost teardown above, so no stale
        // geometry can survive an unavailable Start. The dock re-resolves from
        // scratch on the poll where Start reappears. (HasTaskbarGeometryChanged
        // reports this via the StartLossIsChange mirror, so this refresh is
        // actually reached instead of being skipped as "no change".)
        if (g_overlayWnd && IsWindow(g_overlayWnd)) ShowWindow(g_overlayWnd, SW_HIDE);
        if (g_inputWnd   && IsWindow(g_inputWnd))   ShowWindow(g_inputWnd,   SW_HIDE);
        g_cachedTaskbar       = NULL;      // force a clean re-resolve next poll
        g_cachedDockRect      = {};        // clear cached dock rectangle
        g_dockWidthLocked     = false;     // reset width-lock state
        g_positionInitialized = false;
        g_dockPositionLocked  = false;
        g_stableGeometryCount = 0;         // reset geometry stabilization state
        g_lastStableWidth     = 0;
        g_fixedDockWidth      = 0;
        g_lastStartLeft       = 0;
        g_layoutUnsupported   = false;
        g_systemState         = STATE_BOOT;
        // FIX(v10): reset live dock geometry under g_cs (same lock WM_PAINT holds).
        if (g_csInitialized) EnterCriticalSection(&g_cs);
        g_dockLocalW          = 0;
        g_dockLocalH          = 0;
        if (g_csInitialized) LeaveCriticalSection(&g_cs);
        return;
    }
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
        g_fixedDockWidth     = 0;
        g_appScrollStart     = 0;
        g_wsPopupScrollStart = 0;
        g_dockWidthDirty     = false;
    }

    if (g_fixedDockWidth == 0) {
        int appPinCount = 0;
        int wsPinCount  = 0;
        if (g_csInitialized) {
            EnterCriticalSection(&g_cs);
            appPinCount = CountPinsByType(PIN_APP);
            wsPinCount  = CountPinsByType(PIN_WORKSPACE);
            LeaveCriticalSection(&g_cs);
        }

        g_safeLeftEdgePx = GetTaskbarSafeLeftEdge(tb, tbr);
        int maxDockBudgetPx = (int)(startLeft - DOCK_GAP_PX) - g_safeLeftEdgePx;
        if (maxDockBudgetPx < MIN_VALID_DOCK_WIDTH) maxDockBudgetPx = MIN_VALID_DOCK_WIDTH;

        // Workspaces stay anchored; if limitVisiblePins is ON, respect maxVisibleWorkspaces
        int wsSlots = GetVisibleWorkspaceSlots(wsPinCount);

        int wsRegionW  = (wsSlots > 0)
                         ? (wsSlots * ICON_SIZE) + ((wsSlots - 1) * ICON_SPACING)
                         : 0;
        int sectionGap = (wsSlots > 0 && appPinCount > 0) ? SECTION_GAP : 0;
        int fixedOverhead = DOCK_PAD_LEFT + wsRegionW + sectionGap + DOCK_PAD_RIGHT;

        int slotPitch = ICON_SIZE + ICON_SPACING;
        int maxFitApps = (maxDockBudgetPx - fixedOverhead + ICON_SPACING) / std::max(1, slotPitch);
        if (maxFitApps < 1) maxFitApps = 1;
        g_dynamicMaxAppSlots = maxFitApps;

        int appSlots = std::max(MIN_APP_SLOTS, appPinCount);
        int capApps  = LIMIT_VISIBLE_PINS.load()
                     ? std::min(maxFitApps, std::max(1, MAX_VISIBLE_APPS_SETTING.load()))
                     : maxFitApps;
        if (appSlots > capApps) appSlots = capApps;

        int appRegionW = (appSlots * ICON_SIZE) + ((appSlots - 1) * ICON_SPACING);
        g_fixedDockWidth = fixedOverhead + appRegionW;
        g_fixedDockWidth = std::max(g_fixedDockWidth, MIN_VALID_DOCK_WIDTH);
        g_fixedDockWidth = std::min(g_fixedDockWidth, maxDockBudgetPx);
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
        Wh_Log(L"GEOMETRY: w=%d dockLeft=%d startLeft=%d", newW, dockLeft, startLeft);
        s_lastGeomLog = now;
    }

    // FIX (Issue 2): classify the layout instead of the old blanket early-return
    // `if (newW <= 0 || startLeft <= tbr.left + 50) return;`, which fired on every
    // call for a LEFT-ALIGNED taskbar (Start at ~tbr.left+16) and left the state
    // machine stuck in STATE_BOOT forever (dock clamped over Start, 100 ms poll
    // that never backs off). Decide against the *intended* dock width
    // (g_fixedDockWidth), not the left-edge-clamped newW.
    QpLayoutDecision layout = QpDecideTaskbarLayout(
        tbr.left, tbr.right, startLeft, DOCK_GAP_PX,
        g_fixedDockWidth, MIN_VALID_DOCK_WIDTH);

    if (layout == QP_LAYOUT_PENDING) {
        // Geometry not trustworthy yet (cold boot / Start not detected). Keep
        // the boot poll running; do NOT commit to a layout or settle the state.
        return;
    }

    if (layout == QP_LAYOUT_UNSUPPORTED) {
        // Left-aligned taskbar: no room for the dock. Hide it and SETTLE the
        // state machine to STATE_STABLE so the worker's poll cadence relaxes
        // (STATE_STABLE -> 500 ms) instead of spinning at 100 ms in STATE_BOOT.
        if (!g_layoutUnsupported) {
            Wh_Log(L"GEOMETRY: left-aligned taskbar -- no room for dock, hiding (startLeft=%d tbLeft=%d)",
                      (int)startLeft, (int)tbr.left);
        }
        g_layoutUnsupported = true;
        g_systemState       = STATE_STABLE;   // terminal/idle: polling backs off
        // FIX (Issue 2): hide BOTH the overlay AND the input window. Hiding only
        // the overlay left the transparent g_inputWnd sitting at its last (now
        // wrong) position, still eating clicks over Start on a left-aligned
        // taskbar. Hide both so nothing is left at an old/fake position.
        if (g_overlayWnd && IsWindow(g_overlayWnd))
            ShowWindow(g_overlayWnd, SW_HIDE);
        if (g_inputWnd && IsWindow(g_inputWnd))
            ShowWindow(g_inputWnd, SW_HIDE);
        return;
    }

    // QP_LAYOUT_OK: there is room. If we were previously hidden as unsupported
    // (e.g. the user just switched Start back to centered), un-hide and let the
    // normal state-machine flow below re-establish geometry.
    if (g_layoutUnsupported) {
        g_layoutUnsupported = false;
        // FIX (Issue 2): restore BOTH windows we hid above (overlay + input),
        // mirroring the hide path, so the dock and its click surface both come
        // back. RepositionOverlay below re-seats them at the now-valid geometry
        // so neither is left at an old/fake position.
        if (g_overlayWnd && IsWindow(g_overlayWnd))
            ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);
        if (g_inputWnd && IsWindow(g_inputWnd))
            ShowWindow(g_inputWnd, SW_SHOWNOACTIVATE);
        Wh_Log(L"GEOMETRY: room reappeared left of Start -- dock re-enabled");
    }

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
            if (g_csInitialized) EnterCriticalSection(&g_cs);   // FIX(Issue 2): guard geometry writes with the lock WM_PAINT holds
            g_dockLocalW          = newW;
            g_dockLocalH          = newH;
            if (g_csInitialized) LeaveCriticalSection(&g_cs);
            g_dockCurrentX        = (float)dockLeft;
            g_dockCurrentY        = (float)dockTop;
            g_systemState         = STATE_STABILIZING;
            g_stableGeometryCount = 1;      // boot reading counts as first stable sample
            g_lastStableWidth     = newW;   // seed so next agreeing call hits count>=2
            ReseatIconPositions();   // positions were 0 from LoadPinnedApps
            Wh_Log(L"BOOT: first geometry accepted w=%d", newW);
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
                if (g_csInitialized) EnterCriticalSection(&g_cs);   // FIX(Issue 2): guard geometry writes with the lock WM_PAINT holds
                g_dockLocalW      = newW;
                g_dockLocalH      = newH;
                if (g_csInitialized) LeaveCriticalSection(&g_cs);
                g_dockWidthLocked = true;
                g_systemState     = STATE_STABLE;
                ReseatIconPositions();  // final geometry lock  --  settle all icons
                Wh_Log(L"GEOMETRY: STABLE w=%d h=%d", newW, newH);
                RepositionOverlay();
                if (g_overlayWnd && IsWindow(g_overlayWnd))
                    InvalidateRect(g_overlayWnd, NULL, FALSE);
            }
        }
        // Boot-phase timeout: lock whatever width we have after BOOT_PHASE_MS
        if (g_lastStableWidth > 0 &&
            (int)(GetTickCount() - g_bootStartTime) > BOOT_PHASE_MS) {
            if (g_csInitialized) EnterCriticalSection(&g_cs);   // FIX(Issue 2): guard geometry writes with the lock WM_PAINT holds
            g_dockLocalW      = g_lastStableWidth;
            g_dockLocalH      = newH;
            if (g_csInitialized) LeaveCriticalSection(&g_cs);
            g_dockWidthLocked = true;
            g_systemState     = STATE_STABLE;
            ReseatIconPositions();  // timeout path also needs position correction
            Wh_Log(L"GEOMETRY: STABLE (boot timeout) w=%d", g_dockLocalW);
        }
        return;
    }

    // Stable: apply width changes.  newW is deterministic (it equals
    // g_fixedDockWidth, which is only recomputed on a DPI change or when a
    // pin/unpin marks the width dirty), so ANY difference here is a real
    // content/DPI resize -- never measurement jitter.  Follow it exactly and
    // re-seat every icon so the shrink/grow reflows the slots cleanly.
    if (newW != g_dockLocalW && newW > MIN_VALID_DOCK_WIDTH) {
        if (g_csInitialized) EnterCriticalSection(&g_cs);   // FIX(Issue 2): guard geometry write with the lock WM_PAINT holds
        g_dockLocalW = newW;
        if (g_csInitialized) LeaveCriticalSection(&g_cs);
        ReseatIconPositions();
        Wh_Log(L"GEOMETRY: width update w=%d", newW);
        RepositionOverlay();
        if (g_overlayWnd && IsWindow(g_overlayWnd))
            InvalidateRect(g_overlayWnd, NULL, FALSE);
    }
    if (g_csInitialized) EnterCriticalSection(&g_cs);   // FIX(Issue 2): guard geometry write with the lock WM_PAINT holds
    g_dockLocalH = newH;
    if (g_csInitialized) LeaveCriticalSection(&g_cs);
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
        int vis = GetVisibleAppSlots(appCount);

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

    // Workspace pins: left-anchored; when limitVisiblePins is ON and workspaces
    // exceed maxVisibleWorkspaces, park overflow workspaces cleanly off-screen
    // so they never collide with the gold divider or app region.
    int wsCount = CountPinsByType(PIN_WORKSPACE);
    int wsVis   = GetVisibleWorkspaceSlots(wsCount);
    if (wsVis < 1) wsVis = 1;
    int wsStart = g_wsPopupScrollStart.load();
    int wsMaxStart = std::max(0, wsCount - wsVis);
    if (wsStart < 0) wsStart = 0;
    if (wsStart > wsMaxStart) wsStart = wsMaxStart;

    if (ordinal < wsStart || ordinal >= wsStart + wsVis) {
        int hiddenX = (ordinal < wsStart)
                    ? -(ICON_SIZE + ICON_SPACING)
                    : g_dockLocalW + ICON_SPACING;
        return { hiddenX, y, hiddenX + ICON_SIZE, y + ICON_SIZE };
    }
    int visOrdinal = ordinal - wsStart;
    int x = DOCK_PAD_LEFT + visOrdinal * (ICON_SIZE + ICON_SPACING);
    return { x, y, x + ICON_SIZE, y + ICON_SIZE };
}

// Maximum icons that can be physically laid out in the current dock width.
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
    int halfGap = std::max(1, ICON_SPACING / 2);
    EnterCriticalSection(&g_cs);
    int n   = (int)g_pinnedApps.size();
    int hit = -1;
    for (int i = 0; i < n && hit < 0; ++i) {
        if (!g_pinnedApps[i].icon) continue;
        RECT r = GetIconRectLocal(i, n);
        // Strictly partitioned horizontal bounds: midpoint between neighbors, never overlapping.
        RECT slot = { r.left - halfGap, 0, r.right + halfGap, g_dockLocalH };
        if (PtInRect(&slot, local)) hit = i;
    }
    LeaveCriticalSection(&g_cs);
    return hit;
}

static int HitTestWorkspaceAppendTarget(POINT screenPt) {
    if (g_dockLocalW <= 0 || g_dockLocalH <= 0 || !g_csInitialized) return -1;
    int dockLeft = (int)g_dockCurrentX;
    int dockTop  = (int)g_dockCurrentY;
    if (dockLeft == 0 && g_cachedDockRect.left > 0) {
        dockLeft = g_cachedDockRect.left;
        dockTop  = g_cachedDockRect.top;
    }
    POINT local = { screenPt.x - dockLeft, screenPt.y - dockTop };
    int halfGap = std::max(1, ICON_SPACING / 2);
    EnterCriticalSection(&g_cs);
    int n = (int)g_pinnedApps.size();
    int hit = -1;
    for (int i = 0; i < n && hit < 0; ++i) {
        if (!g_pinnedApps[i].icon || g_pinnedApps[i].type != PIN_WORKSPACE) continue;
        RECT r = GetIconRectLocal(i, n);
        // Expand upward only (+22px) during drag so neighbors never clash horizontally.
        RECT slot = { r.left - halfGap, -22, r.right + halfGap, g_dockLocalH };
        if (PtInRect(&slot, local)) hit = i;
    }
    LeaveCriticalSection(&g_cs);
    return hit;
}

// ============================================================
//  FULLSCREEN-APP SUPPRESSION
//  The dock is HWND_TOPMOST; while a fullscreen app owns the screen it must
//  hide exactly as the taskbar does. Sample the raw Win32 signals here, feed
//  the pure ShouldSuppressForFullscreen() decision, then apply the anti-flicker
//  hysteresis via FullscreenHideDecision(). Worker-thread only (owns g_*).
// ============================================================
static std::wstring GetProcessPath(HWND hwnd);

static void UpdateFullscreenState() {
    HWND fg = GetForegroundWindow();

    // No foreground / desktop / shell => never suppress (treat as "shell").
    bool isShell = false;
    if (!fg) {
        isShell = true;
    } else if (fg == g_overlayWnd || fg == g_inputWnd || fg == g_wsPopupWnd || fg == g_tooltipWnd) {
        isShell = true;                       // our own dock windows never count
    } else {
        HWND shell = GetShellWindow();
        HWND deskt = GetDesktopWindow();
        if (fg == shell || fg == deskt) isShell = true;
        else {
            wchar_t cls[64] = {0};
            GetClassNameW(fg, cls, 63);
            // WorkerW / Progman host the desktop wallpaper; Shell_TrayWnd is the
            // taskbar itself. XamlExplorerHostIslandWindow is the Windows 11
            // Alt+Tab / Task View switcher and MultitaskingViewFrame is the
            // Windows 10 Task View: these transiently own the whole monitor, so
            // the raw "covers monitor" test flags them as fullscreen and hides
            // the dock the moment you press Alt+Tab or Esc-cancel a switch. They
            // are shell UI, NOT a fullscreen app, so treat them as shell and
            // keep the dock put. None of these count as a fullscreen app.
            // StartMenuExperienceHost (Start menu) and SearchHost (Search) are
            // shell UI too: opening them must NEVER drive the fullscreen
            // suppress->restore path, which briefly hid then re-revealed the
            // dock when clicking Start or searching. Matched by class substring
            // exactly like the IsSystemWindow / IsTrueSystemWindow classifiers.
            if (!lstrcmpiW(cls, L"WorkerW")     || !lstrcmpiW(cls, L"Progman") ||
                !lstrcmpiW(cls, L"Shell_TrayWnd") ||
                !lstrcmpiW(cls, L"XamlExplorerHostIslandWindow") ||
                !lstrcmpiW(cls, L"MultitaskingViewFrame") ||
                wcsstr(cls, L"StartMenuExperienceHost") != NULL ||
                wcsstr(cls, L"SearchHost")              != NULL ||
                wcsstr(cls, L"ScreenClipping")          != NULL ||
                wcsstr(cls, L"SnippingTool")            != NULL ||
                wcsstr(cls, L"XboxGameBar")             != NULL) {
                isShell = true;
            } else {
                std::wstring fgExe = GetProcessPath(fg);
                const wchar_t* leaf = PathFindFileNameW(fgExe.c_str());
                if (leaf && (_wcsicmp(leaf, L"ScreenClippingHost.exe") == 0 ||
                             _wcsicmp(leaf, L"SnippingTool.exe") == 0 ||
                             _wcsicmp(leaf, L"ScreenSketch.exe") == 0 ||
                             _wcsicmp(leaf, L"GameBar.exe") == 0)) {
                    isShell = true;
                }
            }
        }
    }

    // Does the foreground window cover its entire monitor (borderless / F11
    // fullscreen)? Compare the window rect to that monitor's full bounds with a
    // small tolerance for off-by-one borders.
    bool coversMonitor = false;
    if (fg && !isShell) {
        RECT wr;
        if (GetWindowRect(fg, &wr)) {
            HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO mi = { sizeof(mi) };
            if (GetMonitorInfoW(mon, &mi)) {
                const LONG tol = 2;
                coversMonitor =
                    (wr.left   <= mi.rcMonitor.left   + tol) &&
                    (wr.top    <= mi.rcMonitor.top    + tol) &&
                    (wr.right  >= mi.rcMonitor.right  - tol) &&
                    (wr.bottom >= mi.rcMonitor.bottom - tol);
            }
        }
    }

    // Exclusive D3D fullscreen / presentation (games, slideshows): the shell
    // itself reports this, and the Snipping Tool secure clip surfaces here too.
    // When a shell or screenshot/snipping overlay is in the foreground (isShell == true),
    // do not let SHQueryUserNotificationState suppress the dock/pill; WDA_EXCLUDEFROMCAPTURE
    // or WDA_NONE handles screenshot visibility in hardware without any hide/reveal flicker.
    bool shellFullscreen = false;
    if (!isShell) {
        QUERY_USER_NOTIFICATION_STATE quns;
        if (SUCCEEDED(SHQueryUserNotificationState(&quns))) {
            shellFullscreen = (quns == QUNS_RUNNING_D3D_FULL_SCREEN) ||
                              (quns == QUNS_PRESENTATION_MODE);
        }
    }

    bool suppressNow = ShouldSuppressForFullscreen(coversMonitor, isShell, shellFullscreen);
    if (suppressNow) g_fullscreenMiss = 0;
    else             ++g_fullscreenMiss;
    bool prevFullscreenActive = g_fullscreenActive;
    g_fullscreenActive = FullscreenHideDecision(suppressNow, prevFullscreenActive,
                                                g_fullscreenMiss, FULLSCREEN_RESTORE_FRAMES);
    // Edge-triggered diagnostics on the SINGLE Wh_Log surface (never per-frame):
    // one line the moment the dock starts hiding for a fullscreen app / snip /
    // Alt+Tab, and one when it clears and is about to be restored. This is the
    // trail for diagnosing a "stuck hidden" dock without flooding the log.
    switch (FullscreenTransitionEvent(prevFullscreenActive, g_fullscreenActive)) {
        case FS_ENTERED:
            Wh_Log(L"FULLSCREEN: suppress dock (coversMonitor=%d isShell=%d shellFS=%d)",
                   (int)coversMonitor, (int)isShell, (int)shellFullscreen);
            break;
        case FS_CLEARED:
            Wh_Log(L"FULLSCREEN: clear -> restore dock (missCount=%d)", g_fullscreenMiss);
            break;
        default:
            break;
    }
}

// ============================================================
//  OVERLAY REPOSITION
// ============================================================
void RepositionOverlay() {
    if (!g_overlayWnd || !IsWindow(g_overlayWnd)) return;

    // Fullscreen app / exclusive presentation / secure snip overlay owns the
    // screen: hide the WHOLE dock (overlay + input) and cancel any live glow
    // flash, mirroring how Windows hides the taskbar. Restore is gated by the
    // hysteresis in UpdateFullscreenState() so reappearing feels seamless
    // rather than a hide-then-pop blink.
    if (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0) {
        if (g_inputWnd && IsWindow(g_inputWnd)) ShowWindow(g_inputWnd, SW_HIDE);
        ShowWindow(g_overlayWnd, SW_HIDE);
        if (g_lockGlowStart != 0) g_lockGlowStart = 0;
        return;
    }

    // FIX (Issue 2): on an unsupported (left-aligned) layout the dock is hidden
    // and has no valid geometry. Keep both windows hidden and return so the
    // reposition/show path can never un-hide the dock we deliberately hid,
    // parking it back on top of Start. RefreshTaskbarCache re-shows it if room
    // returns.
    if (g_layoutUnsupported) {
        if (g_inputWnd && IsWindow(g_inputWnd)) ShowWindow(g_inputWnd, SW_HIDE);
        ShowWindow(g_overlayWnd, SW_HIDE);
        return;
    }

    // FIX (Hotfix 2 -- premature fallback regression): if dock size is still
    // zero we do NOT yet have valid geometry. The synthetic 200x48 fallback was
    // removed entirely: it could un-hide the dock before RefreshTaskbarCache had
    // resolved a real geometry, violating the "stay hidden until valid ownership
    // + valid geometry" invariant. The worker's boot/stabilization path already
    // waits for valid geometry and calls RefreshTaskbarCache() -> RepositionOverlay()
    // once it is available, so no synthetic fallback is needed. Until then, keep
    // BOTH windows hidden (no top-left mini-dock flash, no premature show).
    if (g_dockLocalW <= 0) {
        if (g_inputWnd && IsWindow(g_inputWnd))
            ShowWindow(g_inputWnd, SW_HIDE);
        ShowWindow(g_overlayWnd, SW_HIDE);
        return;
    }

    int w = g_dockLocalW;
    int h = g_dockLocalH;
    int x = (int)g_dockCurrentX;
    int y = (int)g_dockCurrentY;

    static DWORD s_lastReposLog = 0;
    static int   s_lastReposX   = -9999;
    DWORD now = GetTickCount();
    if (abs(x - s_lastReposX) > 50 || now - s_lastReposLog > 2000) {
        Wh_Log(L"REPOSITION: x=%d y=%d w=%d h=%d", x, y, w, h);
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
        if (tbOnScreen) g_autoHideMiss = 0;
        else            ++g_autoHideMiss;
        bool showDock = tbOnScreen || g_autoHideMiss < 2;
        ShowWindow(g_overlayWnd, showDock ? SW_SHOWNOACTIVATE : SW_HIDE);
        if (g_inputWnd && IsWindow(g_inputWnd))
            ShowWindow(g_inputWnd, showDock ? SW_SHOWNOACTIVATE : SW_HIDE);
        if (showDock) PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
    } else {
        ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);
        PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
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
static void FreePinnedAppResources(PinnedApp& app) {
    if (app.prevCachedBmp) { delete app.prevCachedBmp; app.prevCachedBmp = nullptr; }
    if (app.cachedBmp)     { delete app.cachedBmp;     app.cachedBmp = nullptr; }
    if (app.icon)          { DestroyIcon(app.icon);    app.icon = NULL; }
}

// Convert an HBITMAP (32bpp ARGB from IShellItemImageFactory) into a standalone HICON.
static HICON HiconFromShellBitmap(HBITMAP hbm) {
    if (!hbm) return NULL;
    BITMAP bm = {};
    if (!GetObjectW(hbm, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight <= 0)
        return NULL;
    HBITMAP hMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    if (!hMask) return NULL;
    ICONINFO ii = {};
    ii.fIcon    = TRUE;
    ii.hbmMask  = hMask;
    ii.hbmColor = hbm;
    HICON hi = CreateIconIndirect(&ii);
    DeleteObject(hMask);
    return hi;
}

static void RgbToHsl(BYTE r8, BYTE g8, BYTE b8, float& outH, float& outS, float& outL);
static void HslToRgb(float h, float s, float l, BYTE& outR, BYTE& outG, BYTE& outB);
static bool ParseHexColor(const std::wstring& hex, BYTE& r, BYTE& g, BYTE& b);

// Build a cached 32bpp PARGB Gdiplus::Bitmap from an HICON once (off the paint hot path).
// When tintHex is non-empty, selectively recolors only the Windows folder yellow/amber
// hue band [25..65 deg] while preserving white paper, shadows, and 3D Fluent luminance.
static Gdiplus::Bitmap* BuildGdiPlusBitmapFromIcon(HICON icon, const std::wstring& tintHex = L"") {
    if (!icon) return nullptr;
    ICONINFO ii = {};
    if (!GetIconInfo(icon, &ii)) return nullptr;
    Gdiplus::Bitmap* result = nullptr;
    BITMAP bm = {};
    if (ii.hbmColor && GetObjectW(ii.hbmColor, sizeof(bm), &bm) && bm.bmWidth > 0 && bm.bmHeight > 0) {
        int w = bm.bmWidth, h = bm.bmHeight;
        std::vector<BYTE> px((size_t)w * h * 4, 0);
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth       = w;
        bi.bmiHeader.biHeight      = -h;
        bi.bmiHeader.biPlanes      = 1;
        bi.bmiHeader.biBitCount    = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        HDC sdc = GetDC(NULL);
        if (sdc && GetDIBits(sdc, ii.hbmColor, 0, h, px.data(), &bi, DIB_RGB_COLORS)) {
            bool hasAlpha = false;
            for (int i = 0; i < w * h; ++i) {
                if (px[(size_t)i * 4 + 3] != 0) { hasAlpha = true; break; }
            }
            if (!hasAlpha && ii.hbmMask) {
                std::vector<BYTE> mask((size_t)w * h * 4, 0);
                if (GetDIBits(sdc, ii.hbmMask, 0, h, mask.data(), &bi, DIB_RGB_COLORS)) {
                    for (int i = 0; i < w * h; ++i)
                        px[(size_t)i * 4 + 3] = (mask[(size_t)i * 4] == 0) ? 255 : 0;
                }
            }
            result = new Gdiplus::Bitmap(w, h, PixelFormat32bppPARGB);
            Gdiplus::BitmapData bd;
            Gdiplus::Rect lockRect(0, 0, w, h);
            BYTE tr = 0, tg = 0, tb = 0;
            bool doTint = !tintHex.empty() && ParseHexColor(tintHex, tr, tg, tb);
            float tH = 0.0f, tS = 0.0f, tL = 0.5f;
            if (doTint) RgbToHsl(tr, tg, tb, tH, tS, tL);

            if (result->LockBits(&lockRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB, &bd) == Gdiplus::Ok) {
                for (int y = 0; y < h; ++y) {
                    BYTE* dstRow = (BYTE*)bd.Scan0 + (size_t)y * bd.Stride;
                    const BYTE* srcRow = px.data() + (size_t)y * w * 4;
                    for (int x = 0; x < w; ++x) {
                        BYTE b8 = srcRow[x * 4 + 0];
                        BYTE g8 = srcRow[x * 4 + 1];
                        BYTE r8 = srcRow[x * 4 + 2];
                        BYTE a  = srcRow[x * 4 + 3];
                        if (doTint && a > 8) {
                            float ph = 0.0f, ps = 0.0f, pl = 0.0f;
                            RgbToHsl(r8, g8, b8, ph, ps, pl);
                            // Target only yellow/amber folder pixels; keep white paper & neutral shadows intact
                            if (ph >= 25.0f && ph <= 65.0f && ps > 0.20f) {
                                // Smooth weight at band edges [25..32] and [58..65] for anti-aliased seams
                                float wBand = 1.0f;
                                if (ph < 32.0f)      wBand = (ph - 25.0f) / 7.0f;
                                else if (ph > 58.0f) wBand = (65.0f - ph) / 7.0f;
                                float wSat = (ps < 0.30f) ? ((ps - 0.20f) / 0.10f) : 1.0f;
                                float blend = std::max(0.0f, std::min(1.0f, wBand * wSat));

                                // Preserve Fluent front/back luminance shading relative to native folder (~0.56 mid)
                                float newL = std::max(0.14f, std::min(0.88f, tL + (pl - 0.56f) * 0.85f));
                                float newS = std::max(0.15f, std::min(0.82f, tS * (0.75f + 0.35f * ps)));
                                BYTE nr = r8, ng = g8, nb = b8;
                                HslToRgb(tH, newS, newL, nr, ng, nb);
                                r8 = (BYTE)lroundf(r8 * (1.0f - blend) + nr * blend);
                                g8 = (BYTE)lroundf(g8 * (1.0f - blend) + ng * blend);
                                b8 = (BYTE)lroundf(b8 * (1.0f - blend) + nb * blend);
                            }
                        }
                        dstRow[x * 4 + 0] = (BYTE)(b8 * a / 255);
                        dstRow[x * 4 + 1] = (BYTE)(g8 * a / 255);
                        dstRow[x * 4 + 2] = (BYTE)(r8 * a / 255);
                        dstRow[x * 4 + 3] = a;
                    }
                }
                result->UnlockBits(&bd);
            } else {
                delete result;
                result = nullptr;
            }
        }
        if (sdc) ReleaseDC(NULL, sdc);
    }
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);
    return result;
}

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

    // Attempt 1: IShellItemImageFactory at >=2x DPI-scaled target size for crisp
    // Bicubic downsampling (avoids blurry 32px/48px shell list upscaling).
    {
        int wantSz = std::max(32, ICON_SIZE);
        HRESULT hrCom = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        IShellItemImageFactory* pFactory = NULL;
        if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), NULL, IID_PPV_ARGS(&pFactory))) && pFactory) {
            HBITMAP hbm = NULL;
            SIZE sz = { wantSz, wantSz };
            if (SUCCEEDED(pFactory->GetImage(sz, SIIGBF_BIGGERSIZEOK | SIIGBF_RESIZETOFIT | SIIGBF_ICONONLY, &hbm)) && hbm) {
                hSource = HiconFromShellBitmap(hbm);
                DeleteObject(hbm);
            }
            pFactory->Release();
        }
        if (SUCCEEDED(hrCom)) CoUninitialize();
    }

    // Attempt 2: High-res Shell image list (JUMBO 256px -> EXTRALARGE 48px -> LARGE 32px)
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

// INTENTIONALLY SEPARATE from IsTrueSystemWindow below -- do NOT merge them.
// They look near-identical but are two different classifiers used on two
// different code paths, and their class lists / ordering differ on purpose:
//   * IsSystemWindow  -- the DRAG-SOURCE / foreground gate. Used at the general
//     call sites (SmartLaunch foreground checks, drag-candidate gating). Its job
//     is "should we treat the window under the cursor as a real app?"
//   * IsTrueSystemWindow -- the RESOLVER-pipeline classifier. It additionally
//     rejects ReBarWindow32 and orders its checks for the resolver's needs.
// Their taskbar/system class sets are deliberately NOT the same set, so folding
// one into the other would silently change behavior on one path or the other.
// There is no runtime harness here to prove such a merge safe, so they stay two
// functions. If you touch one, decide consciously whether the other should
// change too.
static bool IsSystemWindow(HWND hwnd) {
    if (!hwnd) return true;
    wchar_t cls[256] = {};
    GetClassNameW(hwnd, cls, 256);

    // Taskbar windows are user-interaction layers, not system UI
    if (wcsstr(cls, L"Shell_TrayWnd")    != NULL) return false;
    if (wcsstr(cls, L"Taskbar")          != NULL) return false;
    if (wcsstr(cls, L"MSTaskListWClass") != NULL) return false;
    if (wcsstr(cls, L"ToolbarWindow32")  != NULL) return false;

    // True system surfaces and our own dock/popup windows that should never be drag sources
    if (wcsncmp(cls, L"QPDock", 6) == 0)                 return true;
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

// Resolver-pipeline classifier. SIMILAR to IsSystemWindow above but NOT
// identical and NOT interchangeable (see the long note there): this variant
// also rejects ReBarWindow32 and orders its checks for the resolver. Keep the
// two separate.
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

    if (wcsncmp(cls, L"QPDock", 6) == 0)                 return true;
    if (wcsstr(cls, L"StartMenuExperienceHost") != NULL) return true;
    if (wcsstr(cls, L"SearchHost")              != NULL) return true;
    if (wcsstr(cls, L"ShellExperienceHost")     != NULL) return true;

    return false;
}

// PERF NOTE (intentional, do not "optimize" blindly): this does a fresh
// FindWindowW + GetWindowRect on every call, and it runs on a hot path (every
// ~8 ms frame in DRAG_CANDIDATE). g_cachedTBRect already holds a maintained
// taskbar rect and could replace the live query -- BUT that swap changes the
// freshness semantics: g_cachedTBRect is only refreshed on the worker's cache
// tick, so during a taskbar move/auto-hide slide the cached rect can lag the
// real one, and this function is used for a hit test where a stale rect gives a
// wrong yes/no. The live query is always correct. Switching to the cache is a
// behavioral change that needs runtime verification on a real shell (cursor
// crossing the taskbar edge mid-slide), which cannot be done from the static
// test harness -- so it is deliberately left as a live query for now.
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
    extern HWND g_ewpPillWnd, g_ewpDockWnd, g_ewpPreviewWnd, g_ewpDropZoneWnd;
    if (hwnd == g_overlayWnd || hwnd == g_inputWnd ||
        hwnd == g_wsPopupWnd || hwnd == g_tooltipWnd || hwnd == g_ghostWnd ||
        hwnd == g_ewpPillWnd || hwnd == g_ewpDockWnd ||
        hwnd == g_ewpPreviewWnd || hwnd == g_ewpDropZoneWnd)
        return NULL;

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
    // Match by FILENAME (IsExplorerExePath -> PathFindFileNameW + _wcsicmp), not a
    // whole-path StrStrIW substring, which would false-match any path that merely
    // contains the text "explorer.exe".
    if (IsExplorerExePath(result)) {
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

    // Score the match quality using TIERS, not a flat scale.
    //
    // BUG FIX ("skips most actions when the real windhawk.exe is open and
    // foreground"): the old flat scale (exact=100, title-contains-needle=60,
    // needle-contains-title=40, +20 app / -30 tool) let a foreground Windhawk
    // window -- an app window whose title embeds the mod moniker / target exe
    // name -- out-score the button's EXACT-title target once style bonuses were
    // applied, so the resolver picked windhawk.exe (then discarded it) and the
    // taskbar path MISSed with no fallback.
    //
    // Fix: an exact match is its own tier that no substring match + style bonus
    // can ever beat. Style is only a small INTRA-tier tie-break. Mirrors
    // tests/title_match.h (Tm_ScoreTier / Tm_Score).
    //   tier 3 = exact, 2 = window-title-in-needle, 1 = needle-in-window-title
    int tier;
    if (_wcsicmp(title, c->needle) == 0)         tier = 3;  // Exact
    else if (StrStrIW(title, c->needle) != NULL) tier = 1;  // Title contains needle
    else if (StrStrIW(c->needle, title) != NULL) tier = 2;  // Needle contains title
    else return TRUE;

    int score = tier * 1000;

    // Prefer proper app windows -- but only as a small tie-break WITHIN a tier,
    // never large enough to promote a weaker tier above a stronger one.
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_APPWINDOW)  score += 2;
    if (ex & WS_EX_TOOLWINDOW) score -= 3;

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

    // Seam-recovery retry: an exact-pixel ElementFromPoint can land in the
    // ~1-3px gap BETWEEN two taskbar buttons (or a hair above/below a button's
    // clickable sub-rect) and return the explorer taskbar container, giving an
    // empty result and an immediate MISS -- with no fallback on the taskbar
    // path. Retry UIA at a small, bounded ring of nudged points snapped toward
    // the button-row center. This stays UIA-only (still taskbar-authoritative;
    // it never consults the foreground/active window or process enum), and the
    // horizontal nudge is clamped to < half a button pitch so a probe can never
    // resolve a NEIGHBORING button. Mirrors tests/taskbar_probe.h BuildTaskbarProbes.
    if (result.empty()) {
        // Button pitch/mid-Y derived from the taskbar rect. Win11 taskbar
        // buttons are ~square, so the taskbar row height is a good pitch proxy.
        int tbHeight  = tbRect.bottom - tbRect.top;
        int pitch     = (tbHeight > 12) ? tbHeight : 44;      // sane fallback
        int midY      = (tbRect.top + tbRect.bottom) / 2;

        int hStep = pitch / 3; if (hStep < 1) hStep = 1;
        int hMax  = (pitch / 2) - 1; if (hMax < 1) hMax = 1;

        // Ordered nearest-first: center-Y first, then symmetric horizontal nudges.
        std::vector<POINT> probes;
        if (midY != pt.y) probes.push_back(POINT{ pt.x, midY });
        for (int dx = hStep; dx <= hMax && result.empty(); dx += hStep) {
            probes.push_back(POINT{ pt.x - dx, midY });
            probes.push_back(POINT{ pt.x + dx, midY });
        }

        for (const POINT& pp : probes) {
            if (!result.empty()) break;
            IUIAutomationElement* pProbe = NULL;
            if (SUCCEEDED(pAuto->ElementFromPoint(pp, &pProbe)) && pProbe) {
                result = ResolveElement(pProbe);
                if (result.empty()) {
                    IUIAutomationTreeWalker* pW = NULL;
                    if (SUCCEEDED(pAuto->get_RawViewWalker(&pW)) && pW) {
                        IUIAutomationElement* pPar = NULL;
                        if (SUCCEEDED(pW->GetParentElement(pProbe, &pPar)) && pPar) {
                            result = ResolveElement(pPar);
                            pPar->Release();
                        }
                        pW->Release();
                    }
                }
                pProbe->Release();
            }
        }
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

    // Manhattan gap from the cursor to this window's rect: 0 inside, dx+dy
    // outside. (Mirrors Layer3PointRectDistance in tests/layer3_accept.h,
    // which unit-tests this exact geometry.)
    int dx = 0, dy = 0;
    if (c->pt.x < rc.left)        dx = rc.left - c->pt.x;
    else if (c->pt.x > rc.right)  dx = c->pt.x - rc.right;
    if (c->pt.y < rc.top)         dy = rc.top - c->pt.y;
    else if (c->pt.y > rc.bottom) dy = c->pt.y - rc.bottom;
    int dist = dx + dy;
    // Functionality-note #1: only accept a window the cursor is actually INSIDE
    // (dist == 0). The old "within 50 px" radius let a press on the desktop or
    // on a window's edge resolve to a neighbouring app and pin the wrong thing.
    // (Mirrors Layer3AcceptsDistance in tests/layer3_accept.h.)
    if (dist != 0) return TRUE;

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
                    // A dock-icon hit must NEVER resolve to empty. A WORKSPACE
                    // pin carries a workspaceId and has an empty exePath; return
                    // the workspace sentinel the press path uses instead of "".
                    // Returning "" here (the old behaviour) made the resolver
                    // fall through to L1/L2/L3, which all miss over the dock, so
                    // a workspace pin could be opened only once and never again.
                    // Mirrors tests/dock_icon_resolve.h ResolveDockIconSource.
                    std::wstring p;
                    if (g_pinnedApps[i].type == PIN_WORKSPACE)
                        p = g_pinnedApps[i].workspaceId.empty()
                                ? std::wstring()
                                : std::wstring(L"Explorer workspace");
                    else
                        p = g_pinnedApps[i].exePath;
                    LeaveCriticalSection(&g_cs);
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
                }
            }
        }
        // L3 only runs off-taskbar and only as a true last resort
        if (result.empty()) {
            result = Resolver_Layer3_ProcessFallback(pt);
        }
    }

    if (result.empty()) {
        Wh_Log(L"RESOLVER MISS: all layers failed near (%d,%d)", pt.x, pt.y);
        return L"";
    }

    // Post-resolution validation
    if (IsExplorerExePath(result))
        return L"";
    if (IsExcludedApp(result)) {
        return L"";
    }
    if (StrStrIW(result.c_str(), L"ApplicationFrameHost.exe")) {
        return L"";
    }

    // Require an extractable icon  --  no ghost pins
    HICON testIcon = LoadAppIconStrict(result);
    if (!testIcon) {
        return L"";
    }
    DestroyIcon(testIcon);

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

static bool IsUsbStorageBus(wchar_t driveLetter) {
    wchar_t volPath[16] = {};
    swprintf_s(volPath, L"\\\\.\\%c:", driveLetter);
    HANDLE hVol = CreateFileW(volPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, 0, NULL);
    if (hVol == INVALID_HANDLE_VALUE) return false;

    STORAGE_PROPERTY_QUERY query = {};
    query.PropertyId = StorageDeviceProperty;
    query.QueryType  = PropertyStandardQuery;
    BYTE buf[1024] = {};
    DWORD bytesRet = 0;
    bool isUsb = false;
    if (DeviceIoControl(hVol, IOCTL_STORAGE_QUERY_PROPERTY,
                        &query, sizeof(query),
                        buf, sizeof(buf), &bytesRet, NULL) &&
        bytesRet >= sizeof(STORAGE_DEVICE_DESCRIPTOR)) {
        const STORAGE_DEVICE_DESCRIPTOR* desc = (const STORAGE_DEVICE_DESCRIPTOR*)buf;
        if (desc->BusType == BusTypeUsb) isUsb = true;
    }
    CloseHandle(hVol);
    return isUsb;
}

static bool IsExternalOrRemovablePath(const std::wstring& path) {
    if (path.empty()) return true;
    // Reject shell virtual / MTP / WPD namespaces (e.g. mobile phones)
    if (path.rfind(L"::{", 0) == 0 || path.rfind(L"shell:", 0) == 0)
        return true;

    std::wstring clean = path;
    if (clean.rfind(L"\\\\?\\", 0) == 0 && clean.rfind(L"\\\\?\\UNC\\", 0) != 0)
        clean = clean.substr(4);

    if (clean.size() >= 2 &&
        ((clean[0] >= L'A' && clean[0] <= L'Z') || (clean[0] >= L'a' && clean[0] <= L'z')) &&
        clean[1] == L':') {
        wchar_t driveCh = (wchar_t)towupper(clean[0]);
        wchar_t root[4] = { driveCh, L':', L'\\', L'\0' };
        UINT dt = GetDriveTypeW(root);
        if (dt == DRIVE_REMOVABLE || dt == DRIVE_CDROM ||
            dt == DRIVE_UNKNOWN || dt == DRIVE_NO_ROOT_DIR || dt == DRIVE_RAMDISK)
            return true;
        if (dt == DRIVE_FIXED && IsUsbStorageBus(driveCh))
            return true;
        return false;
    }

    // Reject non-drive-letter / non-UNC paths
    if (clean.rfind(L"\\\\", 0) != 0)
        return true;
    return false;
}

static std::wstring NormalizeWorkspaceFolderPath(const std::wstring& path) {
    if (path.size() > 2 &&
        ((path[0] >= L'A' && path[0] <= L'Z') || (path[0] >= L'a' && path[0] <= L'z')) &&
        path[1] == L':' && path[2] != L'\\' && path[2] != L'/') {
        return path.substr(0, 2) + L"\\" + path.substr(2);
    }
    return path;
}

// FIX-C3 (#7): workspace snapshots are persisted in Windhawk's own per-mod
// storage instead of JSON files under %APPDATA%\WindhawkMods\..., so disabling
// or uninstalling the mod leaves nothing behind (Windhawk cleans its own values
// up). One string value per workspace id, keyed "ws:<id>", with the character
// count stored alongside as "wslen:<id>" -- Wh_GetStringValue can't report the
// size it needs and copies nothing into a too-small buffer, so we size the read
// buffer from that companion int. (The former EnsureDirectory / WorkspaceRootDir
// / WorkspaceFilePath helpers were removed with the file I/O they served.)
static std::wstring WorkspaceValueName(const std::wstring& workspaceId) {
    return L"ws:" + workspaceId;
}
static std::wstring WorkspaceLenName(const std::wstring& workspaceId) {
    return L"wslen:" + workspaceId;
}

static std::wstring MakeWorkspaceId() {
    // Functionality-note #10: GetTickCount() ^ time(NULL) could collide for two
    // workspaces pinned in the same second on the same tick. A GUID gives 128
    // bits of collision-free entropy. (Formatting mirrors FormatWorkspaceId in
    // tests/workspace_id.h, which unit-tests the exact layout.)
    GUID g = {};
    wchar_t buf[64] = {};
    if (SUCCEEDED(CoCreateGuid(&g))) {
        swprintf_s(buf, L"workspace_%08X_%08X_%08X_%08X",
                   (unsigned)g.Data1,
                   ((unsigned)g.Data2 << 16) | (unsigned)g.Data3,
                   ((unsigned)g.Data4[0] << 24) | ((unsigned)g.Data4[1] << 16) |
                   ((unsigned)g.Data4[2] << 8)  |  (unsigned)g.Data4[3],
                   ((unsigned)g.Data4[4] << 24) | ((unsigned)g.Data4[5] << 16) |
                   ((unsigned)g.Data4[6] << 8)  |  (unsigned)g.Data4[7]);
    } else {
        // CoCreateGuid effectively never fails, but keep a unique fallback.
        swprintf_s(buf, L"workspace_%08X_%08X_%08X_%08X",
                   (unsigned)GetTickCount(), (unsigned)time(NULL),
                   (unsigned)GetCurrentProcessId(), (unsigned)GetTickCount());
    }
    return buf;
}

static std::wstring BaseNameFromPath(const std::wstring& path) {
    if (path.empty()) return L"Workspace";
    std::wstring clean = path;
    while (clean.size() > 3 && (clean.back() == L'\\' || clean.back() == L'/'))
        clean.pop_back();
    size_t pos = clean.find_last_of(L"\\/");
    if (pos != std::wstring::npos && pos + 1 < clean.size())
        return clean.substr(pos + 1);
    return clean;
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

static void DeleteWorkspaceSnapshotStorage(const std::wstring& workspaceId) {
    if (workspaceId.empty()) return;
    Wh_SetStringValue(WorkspaceValueName(workspaceId).c_str(), L"");
    Wh_SetIntValue(WorkspaceLenName(workspaceId).c_str(), 0);
}

// ============================================================
//  HSL COLOR MATH & WORKSPACE PALETTE (YELLOW/EXTREME-EXCLUDED)
// ============================================================
static void RgbToHsl(BYTE r8, BYTE g8, BYTE b8, float& outH, float& outS, float& outL) {
    float r = r8 / 255.0f, g = g8 / 255.0f, b = b8 / 255.0f;
    float mx = std::max(r, std::max(g, b));
    float mn = std::min(r, std::min(g, b));
    outL = 0.5f * (mx + mn);
    float d = mx - mn;
    if (d < 1e-5f) {
        outH = 0.0f;
        outS = 0.0f;
        return;
    }
    outS = (outL > 0.5f) ? (d / (2.0f - mx - mn)) : (d / (mx + mn));
    if (mx == r)      outH = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (mx == g) outH = (b - r) / d + 2.0f;
    else              outH = (r - g) / d + 4.0f;
    outH *= 60.0f;
}

static void HslToRgb(float h, float s, float l, BYTE& outR, BYTE& outG, BYTE& outB) {
    auto clamp01 = [](float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); };
    s = clamp01(s);
    l = clamp01(l);
    h = fmodf(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
    if (s < 1e-5f) {
        BYTE v = (BYTE)lroundf(l * 255.0f);
        outR = outG = outB = v;
        return;
    }
    float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
    float p = 2.0f * l - q;
    auto hue2rgb = [&](float t) {
        if (t < 0.0f) t += 1.0f;
        if (t > 1.0f) t -= 1.0f;
        if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
        if (t < 1.0f / 2.0f) return q;
        if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
        return p;
    };
    float hn = h / 360.0f;
    outR = (BYTE)lroundf(clamp01(hue2rgb(hn + 1.0f / 3.0f)) * 255.0f);
    outG = (BYTE)lroundf(clamp01(hue2rgb(hn)) * 255.0f);
    outB = (BYTE)lroundf(clamp01(hue2rgb(hn - 1.0f / 3.0f)) * 255.0f);
}

static bool ParseHexColor(const std::wstring& hex, BYTE& r, BYTE& g, BYTE& b) {
    std::wstring s = hex;
    while (!s.empty() && iswspace(s.front())) s.erase(s.begin());
    while (!s.empty() && iswspace(s.back()))  s.pop_back();
    if (!s.empty() && s[0] == L'#') s.erase(s.begin());
    if (s.size() != 6) return false;
    unsigned int val = 0;
    for (wchar_t ch : s) {
        val <<= 4;
        if (ch >= L'0' && ch <= L'9')      val |= (unsigned)(ch - L'0');
        else if (ch >= L'a' && ch <= L'f') val |= (unsigned)(ch - L'a' + 10);
        else if (ch >= L'A' && ch <= L'F') val |= (unsigned)(ch - L'A' + 10);
        else return false;
    }
    r = (BYTE)((val >> 16) & 0xFF);
    g = (BYTE)((val >> 8)  & 0xFF);
    b = (BYTE)(val & 0xFF);
    return true;
}

static bool IsExcludedFolderColor(BYTE r, BYTE g, BYTE b) {
    float h = 0.0f, s = 0.0f, l = 0.0f;
    RgbToHsl(r, g, b, h, s, l);
    // Reject near-black and near-white extremes
    if (l < 0.18f || l > 0.85f) return true;
    // Reject yellow / amber / gold tones (native Windows single-folder reserve)
    if (s > 0.15f && h >= 25.0f && h <= 75.0f) return true;
    return false;
}

static std::vector<std::wstring> ParseFolderColorPalette(const std::wstring& raw) {
    std::vector<std::wstring> out;
    std::wstringstream ss(raw);
    std::wstring tok;
    while (std::getline(ss, tok, L'|')) {
        BYTE r = 0, g = 0, b = 0;
        if (!ParseHexColor(tok, r, g, b)) continue;
        if (IsExcludedFolderColor(r, g, b)) continue;
        wchar_t norm[16] = {};
        swprintf_s(norm, L"#%02X%02X%02X", r, g, b);
        if (std::find(out.begin(), out.end(), norm) == out.end())
            out.push_back(norm);
    }
    if (out.empty() && raw != DEFAULT_FOLDER_PALETTE)
        return ParseFolderColorPalette(DEFAULT_FOLDER_PALETTE);
    return out;
}

static std::wstring PickGroupedWorkspaceColor(int wsOrdinalHint = -1) {
    std::vector<std::wstring> localPal;
    std::vector<std::wstring> used;
    int ord = wsOrdinalHint;

    if (g_csInitialized) EnterCriticalSection(&g_cs);
    if (g_folderPalette.empty())
        g_folderPalette = ParseFolderColorPalette(DEFAULT_FOLDER_PALETTE);
    localPal = g_folderPalette;
    if (ord < 0) ord = CountPinsByType(PIN_WORKSPACE);
    for (const auto& a : g_pinnedApps) {
        if (a.type == PIN_WORKSPACE && !a.colorHex.empty())
            used.push_back(a.colorHex);
    }
    if (g_csInitialized) LeaveCriticalSection(&g_cs);

    if (localPal.empty()) return L"";
    if (FOLDER_COLOR_MODE_ORDERED.load()) {
        if (ord < 0) ord = 0;
        return localPal[(size_t)ord % localPal.size()];
    }

    // Random mode: strictly prefer unused colors first
    std::vector<std::wstring> unused;
    for (const auto& c : localPal) {
        bool taken = false;
        for (const auto& u : used) {
            if (_wcsicmp(c.c_str(), u.c_str()) == 0) { taken = true; break; }
        }
        if (!taken) unused.push_back(c);
    }
    const auto& pool = unused.empty() ? localPal : unused;
    static std::atomic<unsigned int> s_rngSeq{0};
    unsigned int seed = (unsigned int)(GetTickCount64() ^ (s_rngSeq.fetch_add(1) * 2654435761u));
    return pool[(size_t)(seed % pool.size())];
}

static bool SaveWorkspaceSnapshot(const WorkspaceSnapshot& snapshot) {
    if (snapshot.id.empty()) return false;   // FIX-C3 (#7)

    std::wstring body;
    // MEM: reserve up front so the ~dozen `+=` appends below don't trigger a
    // chain of geometric reallocations (each realloc copies the whole buffer).
    // Sized from the actual snapshot so it fits in one allocation in the common
    // case without over-committing.
    body.reserve(128 + snapshot.folderPaths.size() * 96 + snapshot.windows.size() * 160);
    body += L"{\r\n";
    body += L"  \"id\": \"" + JsonEscape(snapshot.id) + L"\",\r\n";
    body += L"  \"displayName\": \"" + JsonEscape(snapshot.displayName) + L"\",\r\n";
    if (snapshot.customNamed)
        body += L"  \"customName\": 1,\r\n";
    if (!snapshot.colorHex.empty())
        body += L"  \"color\": \"" + JsonEscape(snapshot.colorHex) + L"\",\r\n";
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

    // FIX-C3 (#7): persist to Windhawk mod storage instead of a %APPDATA% file.
    // No BOM needed (that was a file-encoding marker); the loader still tolerates
    // a leading BOM for safety. The companion length lets the loader size its buffer.
    if (!Wh_SetStringValue(WorkspaceValueName(snapshot.id).c_str(), body.c_str()))
        return false;
    Wh_SetIntValue(WorkspaceLenName(snapshot.id).c_str(), (int)body.size());
    return true;
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
    if (workspaceId.empty()) return false;

    // FIX-C3 (#7): read the snapshot from Windhawk mod storage. The companion
    // "wslen:<id>" int (0 when unset) gives the exact character count so we can
    // size the buffer -- Wh_GetStringValue copies nothing into a too-small buffer.
    int chars = Wh_GetIntValue(WorkspaceLenName(workspaceId).c_str(), 0);
    if (chars <= 0) return false;
    std::vector<wchar_t> buf((size_t)chars + 1, 0);
    if (Wh_GetStringValue(WorkspaceValueName(workspaceId).c_str(),
                          buf.data(), buf.size()) == 0)
        return false;

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
        int cnVal = 0;
        if (ExtractJsonIntValue(line, L"\"customName\"", cnVal) && cnVal != 0)
            snapshot.customNamed = true;
        if (snapshot.colorHex.empty() &&
            ExtractJsonStringValue(line, L"\"color\"", v))
            snapshot.colorHex = JsonUnescape(v);
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

static DWORD GetLongPathAttributesW(const std::wstring& path) {
    if (path.empty()) return INVALID_FILE_ATTRIBUTES;
    if (path.size() < MAX_PATH || path.rfind(L"\\\\?\\", 0) == 0)
        return GetFileAttributesW(path.c_str());
    std::wstring ext = (path.rfind(L"\\\\", 0) == 0)
        ? (L"\\\\?\\UNC\\" + path.substr(2))
        : (L"\\\\?\\" + path);
    return GetFileAttributesW(ext.c_str());
}

static std::wstring FolderPathFromExplorer(IWebBrowserApp* app) {
    if (!app) return L"";

    // Primary: query the ShellBrowser's active folder PIDL directly via
    // IShellBrowser -> QueryActiveShellView -> IFolderView -> IPersistFolder2::GetCurFolder
    // -> SHGetNameFromIDList(SIGDN_FILESYSPATH). Unlike get_LocationURL + PathCreateFromUrlW,
    // this works for paths > MAX_PATH, paths with '#' or '%', and updates immediately after BrowseObject.
    IServiceProvider* sp = NULL;
    if (SUCCEEDED(app->QueryInterface(IID_IServiceProvider, (void**)&sp)) && sp) {
        IShellBrowser* sb = NULL;
        if (SUCCEEDED(sp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&sb)) && sb) {
            IShellView* sv = NULL;
            if (SUCCEEDED(sb->QueryActiveShellView(&sv)) && sv) {
                IFolderView* fv = NULL;
                if (SUCCEEDED(sv->QueryInterface(IID_IFolderView, (void**)&fv)) && fv) {
                    IPersistFolder2* pf2 = NULL;
                    if (SUCCEEDED(fv->GetFolder(IID_IPersistFolder2, (void**)&pf2)) && pf2) {
                        LPITEMIDLIST pidl = NULL;
                        if (SUCCEEDED(pf2->GetCurFolder(&pidl)) && pidl) {
                            PWSTR pszPath = NULL;
                            std::wstring pidlPath;
                            if (SUCCEEDED(SHGetNameFromIDList(pidl, SIGDN_FILESYSPATH, &pszPath)) && pszPath) {
                                pidlPath = pszPath;
                                CoTaskMemFree(pszPath);
                            }
                            CoTaskMemFree(pidl);
                            if (!pidlPath.empty()) {
                                DWORD attrs = GetLongPathAttributesW(pidlPath);
                                if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                                    pf2->Release();
                                    fv->Release();
                                    sv->Release();
                                    sb->Release();
                                    sp->Release();
                                    return pidlPath;
                                }
                            }
                        }
                        pf2->Release();
                    }
                    fv->Release();
                }
                sv->Release();
            }
            sb->Release();
        }
        sp->Release();
    }

    // Fallback: get_LocationURL + PathCreateFromUrlW
    BSTR url = NULL;
    if (FAILED(app->get_LocationURL(&url)) || !url || !url[0]) {
        if (url) SysFreeString(url);
        return L"";
    }

    DWORD cap = std::max<DWORD>(4096, SysStringLen(url) + 256);
    std::vector<wchar_t> pathBuf(cap, 0);
    DWORD len = cap;
    HRESULT hr = PathCreateFromUrlW(url, pathBuf.data(), &len, 0);
    if ((hr == E_POINTER || hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) && cap < 32768) {
        cap = std::max<DWORD>(32768, len + 16);
        pathBuf.assign(cap, 0);
        len = cap;
        hr = PathCreateFromUrlW(url, pathBuf.data(), &len, 0);
    }
    SysFreeString(url);
    if (FAILED(hr) || !pathBuf[0]) return L"";

    std::wstring resolved(pathBuf.data());
    DWORD attrs = GetLongPathAttributesW(resolved);
    if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY))
        return L"";
    return resolved;
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

static bool CaptureWorkspaceSnapshot(WorkspaceSnapshot& snapshot, HWND ownerHwnd, bool allTabsInWindow) {
    HRESULT hrInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) {
        Wh_Log(L"WORKSPACE CAPTURE: CoInitializeEx failed hr=0x%08X", (unsigned)hrInit);
        return false;
    }

    IShellWindows* shellWindows = NULL;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
                                  IID_IShellWindows, (void**)&shellWindows);
    if (FAILED(hr) || !shellWindows) {
        Wh_Log(L"WORKSPACE CAPTURE: ShellWindows unavailable hr=0x%08X", (unsigned)hr);
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
                if (!folder.empty() && !IsExternalOrRemovablePath(folder)) {
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

                        if ((int)folders.size() < MAX_PATHS_PER_WORKSPACE &&
                            std::find_if(folders.begin(), folders.end(),
                                         [&](const std::wstring& p) { return _wcsicmp(NormalizeForCompare(p).c_str(), NormalizeForCompare(folder).c_str()) == 0; })
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
        Wh_Log(L"WORKSPACE CAPTURE: no Explorer folder tabs found");
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

    if (!allTabsInWindow && !groups.empty() && !groups[0].tabPaths.empty()) {
        int aIdx = std::max(0, std::min(groups[0].activeTab, (int)groups[0].tabPaths.size() - 1));
        std::wstring singlePath = groups[0].tabPaths[aIdx];
        groups[0].tabPaths.clear();
        groups[0].tabPaths.push_back(singlePath);
        groups[0].activeTab = 0;
        folders.clear();
        folders.push_back(singlePath);
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
                            HWND fg = GetForegroundWindow();
                            HWND fgRoot = fg ? GetAncestor(fg, GA_ROOT) : NULL;
                            if (!fgRoot) fgRoot = fg;
                            wchar_t title[512] = {};
                            GetWindowTextW(explorerRoot, title, ARRAYSIZE(title));
                            bool titleMatch = title[0] &&
                                (_wcsicmp(title, taskbarElementName.c_str()) == 0 ||
                                 StrStrIW(title, taskbarElementName.c_str()) != NULL ||
                                 StrStrIW(taskbarElementName.c_str(), title) != NULL);
                            bool fileExplorerButton =
                                StrStrIW(taskbarElementName.c_str(), L"File Explorer") != NULL;
                            if (explorerRoot == fgRoot || titleMatch || fileExplorerButton) {
                                // Prefer foreground/title match over generic fallback
                                if (!outExplorerHwnd || *outExplorerHwnd == NULL ||
                                    explorerRoot == fgRoot || titleMatch) {
                                    if (outExplorerHwnd) *outExplorerHwnd = explorerRoot;
                                }
                                if (explorerRoot == fgRoot || titleMatch) {
                                    match = true;
                                }
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
    if (!match && outExplorerHwnd && *outExplorerHwnd != NULL)
        match = true;
    if (SUCCEEDED(hrInit)) CoUninitialize();
    return match;
}

// ============================================================
//  VISUAL FEEDBACK
// ============================================================
static void TriggerLimitFlash() {
    // "Dock full" (pin-limit) feedback: a RED lock-glow bloom, replacing the old
    // red right-edge feedback line. TriggerLockGlowKind is forward-declared near
    // the top; it is defined with the other lock-glow helpers.
    TriggerLockGlowKind(LOCKGLOW_LIMIT);
    // Shake the whole dock left-right  --  explicit, non-silent feedback
    g_shakeActive = true;
    g_shakeStart  = GetTickCount();
}

// ============================================================
//  PIN
// ============================================================
// FIX-2: IsPinned MUST be called while g_cs is held by the caller.
// Calling it outside the lock while another thread erases elements is UB.
static bool ExePathsMatchOrSameApp(const std::wstring& a, const std::wstring& b) {
    if (a.empty() || b.empty()) return false;
    if (_wcsicmp(a.c_str(), b.c_str()) == 0) return true;
    const wchar_t* leafA = PathFindFileNameW(a.c_str());
    const wchar_t* leafB = PathFindFileNameW(b.c_str());
    return (leafA && leafA[0] && leafB && leafB[0] && _wcsicmp(leafA, leafB) == 0);
}

bool IsPinned(const std::wstring& p) {
    for (const auto& app : g_pinnedApps)
        if (app.type == PIN_APP && ExePathsMatchOrSameApp(app.exePath, p)) return true;
    return false;
}

static std::wstring MakeWorkspaceSignature(const std::vector<std::wstring>& folders) {
    std::vector<std::wstring> sorted;
    sorted.reserve(folders.size());
    for (const auto& f : folders) {
        std::wstring norm = NormalizeForCompare(f);
        for (wchar_t& ch : norm) ch = (wchar_t)towlower(ch);
        if (!norm.empty()) sorted.push_back(std::move(norm));
    }
    std::sort(sorted.begin(), sorted.end());
    std::wstring sig;
    for (size_t i = 0; i < sorted.size(); ++i) {
        if (i > 0) sig += L'|';
        sig += sorted[i];
    }
    return sig;
}

static bool IsWorkspacePinned(const std::wstring& idOrSig) {
    for (const auto& app : g_pinnedApps) {
        if (app.type != PIN_WORKSPACE) continue;
        if (_wcsicmp(app.workspaceId.c_str(), idOrSig.c_str()) == 0) return true;
        if (!app.workspaceSig.empty() && _wcsicmp(app.workspaceSig.c_str(), idOrSig.c_str()) == 0) return true;
    }
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

    int cap = std::max(1, MAX_PINNED_APPS);
    if (CountPinsByType(PIN_APP) >= cap) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        TriggerLimitFlash();
        Wh_Log(L"PIN BLOCKED limit=%d: %s", cap, path.c_str());
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

    Wh_Log(L"PIN OK: %s (total=%d)", path.c_str(), (int)g_pinnedApps.size());
}


static std::wstring FormatWorkspaceDisplayName(const std::vector<std::wstring>& folders) {
    if (folders.empty()) return L"Workspace";
    std::wstring name = BaseNameFromPath(folders[0]);
    if (folders.size() > 1) {
        wchar_t suffix[32] = {};
        swprintf_s(suffix, L" +%u", (unsigned)(folders.size() - 1));
        name += suffix;
    }
    return name;
}

static bool AppendFoldersToWorkspace(int pinIdx, const std::vector<std::wstring>& newFolders) {
    if (newFolders.empty() || !g_csInitialized) return false;
    std::wstring wsId;
    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE) {
        wsId = g_pinnedApps[pinIdx].workspaceId;
    }
    LeaveCriticalSection(&g_cs);
    if (wsId.empty()) return false;

    WorkspaceSnapshot snap;
    if (!LoadWorkspaceSnapshot(wsId, snap)) {
        snap.id = wsId;
    }

    int added = 0;
    for (const auto& rawPath : newFolders) {
        if ((int)snap.folderPaths.size() >= MAX_PATHS_PER_WORKSPACE) break;
        std::wstring norm = NormalizeWorkspaceFolderPath(rawPath);
        if (norm.empty() || IsExternalOrRemovablePath(norm)) continue;
        bool exists = false;
        for (const auto& existing : snap.folderPaths) {
            if (_wcsicmp(NormalizeForCompare(existing).c_str(),
                         NormalizeForCompare(norm).c_str()) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            snap.folderPaths.push_back(norm);
            added++;
        }
    }
    if (added == 0) return false;

    if (snap.folderPaths.size() > 1 && snap.colorHex.empty())
        snap.colorHex = PickGroupedWorkspaceColor();
    else if (snap.folderPaths.size() <= 1)
        snap.colorHex.clear();

    if (!snap.customNamed)
        snap.displayName = FormatWorkspaceDisplayName(snap.folderPaths);
    snap.windows.clear();
    WorkspaceSnapshot::WindowGroup grp;
    grp.activeTab = 0;
    grp.tabPaths  = snap.folderPaths;
    snap.windows.push_back(grp);
    if (!SaveWorkspaceSnapshot(snap)) return false;

    std::wstring newSig = MakeWorkspaceSignature(snap.folderPaths);
    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE &&
        _wcsicmp(g_pinnedApps[pinIdx].workspaceId.c_str(), wsId.c_str()) == 0) {
        g_pinnedApps[pinIdx].workspacePaths = snap.folderPaths;
        g_pinnedApps[pinIdx].workspaceSig   = newSig;
        g_pinnedApps[pinIdx].displayName    = snap.displayName;
        g_pinnedApps[pinIdx].customNamed    = snap.customNamed;
        g_pinnedApps[pinIdx].colorHex       = snap.colorHex;
    }
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
    return true;
}

static void RemoveWorkspacePath(int pinIdx, int pathIdx) {
    if (!g_csInitialized) return;
    std::wstring wsId;
    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE) {
        wsId = g_pinnedApps[pinIdx].workspaceId;
    }
    LeaveCriticalSection(&g_cs);
    if (wsId.empty()) return;

    WorkspaceSnapshot snap;
    if (!LoadWorkspaceSnapshot(wsId, snap)) return;
    if (pathIdx < 0 || pathIdx >= (int)snap.folderPaths.size()) return;

    snap.folderPaths.erase(snap.folderPaths.begin() + pathIdx);
    if (snap.folderPaths.empty()) {
        UnpinAppByIndex(pinIdx);
        return;
    }

    if (snap.folderPaths.size() <= 1)
        snap.colorHex.clear();

    if (!snap.customNamed)
        snap.displayName = FormatWorkspaceDisplayName(snap.folderPaths);
    snap.windows.clear();
    WorkspaceSnapshot::WindowGroup grp;
    grp.activeTab = 0;
    grp.tabPaths  = snap.folderPaths;
    snap.windows.push_back(grp);
    SaveWorkspaceSnapshot(snap);

    std::wstring newSig = MakeWorkspaceSignature(snap.folderPaths);
    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE &&
        _wcsicmp(g_pinnedApps[pinIdx].workspaceId.c_str(), wsId.c_str()) == 0) {
        g_pinnedApps[pinIdx].workspacePaths = snap.folderPaths;
        g_pinnedApps[pinIdx].workspaceSig   = newSig;
        g_pinnedApps[pinIdx].displayName    = snap.displayName;
        g_pinnedApps[pinIdx].customNamed    = snap.customNamed;
        g_pinnedApps[pinIdx].colorHex       = snap.colorHex;
    }
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
}

static void ReorderWorkspacePath(int pinIdx, int fromIdx, int toIdx) {
    if (!g_csInitialized || fromIdx == toIdx) return;
    std::wstring wsId;
    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE) {
        wsId = g_pinnedApps[pinIdx].workspaceId;
    }
    LeaveCriticalSection(&g_cs);
    if (wsId.empty()) return;

    WorkspaceSnapshot snap;
    if (!LoadWorkspaceSnapshot(wsId, snap)) return;
    int n = (int)snap.folderPaths.size();
    if (fromIdx < 0 || fromIdx >= n || toIdx < 0 || toIdx >= n) return;

    std::wstring item = snap.folderPaths[fromIdx];
    snap.folderPaths.erase(snap.folderPaths.begin() + fromIdx);
    snap.folderPaths.insert(snap.folderPaths.begin() + toIdx, item);

    if (!snap.customNamed)
        snap.displayName = FormatWorkspaceDisplayName(snap.folderPaths);
    snap.windows.clear();
    WorkspaceSnapshot::WindowGroup grp;
    grp.activeTab = 0;
    grp.tabPaths  = snap.folderPaths;
    snap.windows.push_back(grp);
    SaveWorkspaceSnapshot(snap);

    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE &&
        _wcsicmp(g_pinnedApps[pinIdx].workspaceId.c_str(), wsId.c_str()) == 0) {
        g_pinnedApps[pinIdx].workspacePaths = snap.folderPaths;
        g_pinnedApps[pinIdx].displayName    = snap.displayName;
        g_pinnedApps[pinIdx].customNamed    = snap.customNamed;
    }
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
}

static void RenameWorkspace(int pinIdx, const std::wstring& rawName) {
    if (!g_csInitialized) return;
    std::wstring wsId;
    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE) {
        wsId = g_pinnedApps[pinIdx].workspaceId;
    }
    LeaveCriticalSection(&g_cs);
    if (wsId.empty()) return;

    std::wstring trimmed = rawName;
    while (!trimmed.empty() && iswspace(trimmed.front())) trimmed.erase(trimmed.begin());
    while (!trimmed.empty() && iswspace(trimmed.back()))  trimmed.pop_back();
    if (trimmed.size() > 48) trimmed.resize(48);

    WorkspaceSnapshot snap;
    if (!LoadWorkspaceSnapshot(wsId, snap)) return;

    if (trimmed.empty()) {
        snap.customNamed = false;
        snap.displayName = FormatWorkspaceDisplayName(snap.folderPaths);
    } else {
        snap.customNamed = true;
        snap.displayName = trimmed;
    }
    if (!SaveWorkspaceSnapshot(snap)) return;

    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE &&
        _wcsicmp(g_pinnedApps[pinIdx].workspaceId.c_str(), wsId.c_str()) == 0) {
        g_pinnedApps[pinIdx].displayName = snap.displayName;
        g_pinnedApps[pinIdx].customNamed = snap.customNamed;
    }
    LeaveCriticalSection(&g_cs);
    SavePinnedApps();
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
}

void PinWorkspace(HWND explorerHwnd) {
    if (!ENABLE_EXPLORER_WORKSPACE_PINS) {
        DragTraceLog(L"WORKSPACE PIN REJECT: setting disabled");
        return;
    }

    WorkspaceSnapshot snapshot;
    bool allTabsInWin = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    if (!explorerHwnd || !CaptureWorkspaceSnapshot(snapshot, explorerHwnd, allTabsInWin)) {
        DragTraceLog(L"WORKSPACE PIN REJECT: no Explorer folders");
        return;
    }
    std::wstring wsSig = MakeWorkspaceSignature(snapshot.folderPaths);
    if (!wsSig.empty()) {
        bool dup = false;
        if (g_csInitialized) {
            EnterCriticalSection(&g_cs);
            dup = IsWorkspacePinned(wsSig);
            LeaveCriticalSection(&g_cs);
        }
        if (dup) {
            DragTraceLog(L"WORKSPACE PIN REJECT: duplicate workspace", wsSig.c_str());
            TriggerLockGlowKind(LOCKGLOW_SEAL);
            return;
        }
    }
    if (snapshot.folderPaths.size() > 1)
        snapshot.colorHex = PickGroupedWorkspaceColor();
    else
        snapshot.colorHex.clear();

    Wh_Log(L"WORKSPACE CAPTURE OK: owner=%p windows=%d folders=%d active=%d",
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
    if (IsWorkspacePinned(snapshot.id) || (!wsSig.empty() && IsWorkspacePinned(wsSig))) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        return;
    }

    int cap = MAX_WORKSPACE_PINS;
    if (CountPinsByType(PIN_WORKSPACE) >= cap) {
        LeaveCriticalSection(&g_cs);
        DestroyIcon(icon);
        TriggerLimitFlash();
        Wh_Log(L"WORKSPACE PIN BLOCKED limit=%d: %s", cap, snapshot.id.c_str());
        return;
    }

    PinnedApp app;
    app.type         = PIN_WORKSPACE;
    app.workspaceId  = snapshot.id;
    app.workspaceSig   = wsSig;
    app.colorHex       = snapshot.colorHex;
    app.workspacePaths = snapshot.folderPaths;
    app.displayName    = snapshot.displayName;
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

    Wh_Log(L"WORKSPACE PIN OK: %s folders=%d", snapshot.id.c_str(), (int)snapshot.folderPaths.size());
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

    Wh_Log(L"UNPIN index=%d id=%s",
              i,
              g_pinnedApps[i].type == PIN_WORKSPACE
                  ? g_pinnedApps[i].workspaceId.c_str()
                  : g_pinnedApps[i].exePath.c_str());

    std::wstring removedWsId = (g_pinnedApps[i].type == PIN_WORKSPACE)
                             ? g_pinnedApps[i].workspaceId : L"";
    FreePinnedAppResources(g_pinnedApps[i]);
    g_pinnedApps.erase(g_pinnedApps.begin() + i);
    int curPopIdx = g_wsPopupPinIdx.load();
    if (curPopIdx == i) {
        if (g_overlayWnd && IsWindow(g_overlayWnd))
            PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 1, 0);
    } else if (curPopIdx > i) {
        g_wsPopupPinIdx.store(curPopIdx - 1);
    }

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
    if (!removedWsId.empty()) DeleteWorkspaceSnapshotStorage(removedWsId);
    SavePinnedApps();  // Outside CS  --  registry I/O must not block the input loop

    // Dock width is fixed by MAX_PINNED_APPS  --  do NOT modify g_dockLocalW here

    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);
}

void UnpinAllApps() {
    if (g_dragState == DRAG_DRAGGING || g_dragState == DRAG_PRESS ||
        g_dragState == DRAG_REORDER) return;

    std::vector<std::wstring> removedWsIds;
    EnterCriticalSection(&g_cs);
    for (auto& app : g_pinnedApps) {
        if (app.type == PIN_WORKSPACE && !app.workspaceId.empty())
            removedWsIds.push_back(app.workspaceId);
        FreePinnedAppResources(app);
    }
    g_pinnedApps.clear();
    g_dockWidthDirty = true;   // all pins removed -> re-fit dock width to minimum
    HardStateReset();
    LeaveCriticalSection(&g_cs);
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 1, 0);
    for (const auto& id : removedWsIds) DeleteWorkspaceSnapshotStorage(id);
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

// ------------------------------------------------------------------
//  Workspace click: focus an already-open window instead of opening a
//  duplicate. Mirrors PickWorkspaceWindowToFocus in tests/workspace_focus.h
//  (unit-tested there with no Win32/COM deps). The app-pin path already
//  focuses a running app (FindRunningAppWindow -> SetForegroundWindow);
//  this gives workspace pins the same behaviour, so a second click doesn't
//  stack another Explorer window on the same folder.
// ------------------------------------------------------------------
struct OpenExplorerWindow {
    HWND         hwnd;
    std::wstring folder;
};

static std::wstring NormalizeForCompare(std::wstring p) {
    for (wchar_t& c : p)
        if (c == L'/') c = L'\\';
    if (p.size() >= 2 &&
        ((p[0] >= L'A' && p[0] <= L'Z') || (p[0] >= L'a' && p[0] <= L'z')) &&
        p[1] == L':') {
        if (p.size() == 2)      p += L"\\";                              // "C:"    -> "C:\"
        else if (p[2] != L'\\') p = p.substr(0, 2) + L"\\" + p.substr(2); // "C:x" -> "C:\x"
    }
    while (p.size() > 3 && p.back() == L'\\')
        p.pop_back();
    return p;
}

static bool WorkspaceFoldersMatch(const std::wstring& a, const std::wstring& b) {
    return _wcsicmp(NormalizeForCompare(a).c_str(),
                    NormalizeForCompare(b).c_str()) == 0;
}

// Returns the index into `open` of a window already showing `primaryFolder`
// (focus it), or -1 when none matches (open a new window).
static int PickWorkspaceWindowToFocus(const std::wstring& primaryFolder,
                                      const std::vector<OpenExplorerWindow>& open) {
    if (primaryFolder.empty())
        return -1;
    for (int i = 0; i < (int)open.size(); ++i) {
        if (open[i].hwnd && WorkspaceFoldersMatch(open[i].folder, primaryFolder))
            return i;
    }
    return -1;
}

// Try several candidate folders (a workspace group's tab folders, active tab
// first). Empty candidates are skipped -- a virtual Explorer location (This PC,
// Home, a library) resolves to "" at capture time, so keying the focus-if-open
// decision on ONLY the active tab meant such a workspace stacked a duplicate
// window on every click. Mirrors PickWorkspaceWindowToFocusAny in
// tests/workspace_focus.h.
static int PickWorkspaceWindowToFocusAny(const std::vector<std::wstring>& candidateFolders,
                                         const std::vector<OpenExplorerWindow>& open) {
    for (const std::wstring& folder : candidateFolders) {
        if (folder.empty())
            continue;
        int idx = PickWorkspaceWindowToFocus(folder, open);
        if (idx >= 0)
            return idx;
    }
    return -1;
}

static void PumpStaMessagesBrief() {
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

static bool WaitWithStaPump(DWORD timeoutMs) {
    DWORD start = GetTickCount();
    while (true) {
        if (g_launchWorkspaceStop.load()) return true;
        PumpStaMessagesBrief();
        DWORD elapsed = GetTickCount() - start;
        if (elapsed >= timeoutMs) return false;
        DWORD rem = timeoutMs - elapsed;
        if (g_exitEvent) {
            DWORD r = MsgWaitForMultipleObjects(1, &g_exitEvent, FALSE, rem, QS_ALLINPUT);
            if (r == WAIT_OBJECT_0) return true;
            if (r == WAIT_TIMEOUT)  return false;
        } else {
            Sleep(std::min<DWORD>(rem, 15));
        }
    }
}

static void LaunchWorkspace(const std::wstring& workspaceId) {
    if (g_restoreExecCsInit) EnterCriticalSection(&g_restoreExecCs);
    struct RestoreGuard {
        ~RestoreGuard() { if (g_restoreExecCsInit) LeaveCriticalSection(&g_restoreExecCs); }
    } restoreGuard;

    WorkspaceSnapshot snapshot;
    if (!LoadWorkspaceSnapshot(workspaceId, snapshot)) {
        DragTraceLog(L"WORKSPACE LAUNCH REJECT: load failed", workspaceId.c_str());
        return;
    }

    std::vector<std::wstring> validFolders;
    for (const auto& folder : snapshot.folderPaths) {
        DWORD attrs = GetLongPathAttributesW(folder);
        if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(L"WORKSPACE SKIP missing folder: %s", folder.c_str());
            continue;
        }
        validFolders.push_back(folder);
    }
    if (validFolders.empty()) return;

    if (OPEN_IN_SEPARATE_WINDOWS.load()) {
        HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        std::vector<OpenExplorerWindow> openWindows;
        {
            std::vector<ExplorerTab> existingTabs = EnumerateExplorerTabs();
            for (const auto& tab : existingTabs)
                openWindows.push_back({ tab.hwnd, tab.folder });
            ReleaseExplorerTabs(existingTabs);
        }
        bool requireAll = REOPEN_CLOSED_PATHS_ON_FOCUS.load();
        if (!requireAll) {
            int focusIdx = PickWorkspaceWindowToFocusAny(validFolders, openWindows);
            if (focusIdx >= 0) {
                HWND existing = openWindows[focusIdx].hwnd;
                if (existing && IsWindow(existing)) {
                    if (IsIconic(existing)) ShowWindow(existing, SW_RESTORE);
                    FocusExplorerTabByFolder(existing, openWindows[focusIdx].folder);
                }
                if (SUCCEEDED(hrInit)) CoUninitialize();
                return;
            }
        }
        for (const auto& folder : validFolders) {
            if (g_launchWorkspaceStop.load()) break;
            if (requireAll && PickWorkspaceWindowToFocus(folder, openWindows) >= 0)
                continue;
            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.lpFile = L"explorer.exe";
            std::wstring args = L"/separate,\"" + folder + L"\"";
            sei.lpParameters = args.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        }
        if (SUCCEEDED(hrInit)) CoUninitialize();
        Wh_Log(L"WORKSPACE LAUNCH (separate): %s count=%d", workspaceId.c_str(), (int)validFolders.size());
        return;
    }

    WorkspaceSnapshot::WindowGroup unifiedGroup;
    unifiedGroup.activeTab = 0;
    unifiedGroup.tabPaths  = std::move(validFolders);
    RestoreExplorerWindowGroup(unifiedGroup);
    Wh_Log(L"WORKSPACE LAUNCH (single window): %s tabs=%d", workspaceId.c_str(), (int)unifiedGroup.tabPaths.size());
}

static DWORD WINAPI LaunchWorkspaceThread(LPVOID param) {
    // v2.5.4 (this-round Issue 2): the workspace-restore path calls window-geometry
    // and SetForegroundWindow APIs, so set per-monitor-v2 DPI here too, matching the
    // UI and worker threads in the tool process.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    std::wstring* workspaceId = (std::wstring*)param;
    if (workspaceId) {
        // FIX-B2 (#6): bail out early if the mod is being torn down. Cheap guard
        // so a reload while this thread is queued doesn't kick off seconds of
        // COM/ShellExecuteExW work against a soon-to-be-unmapped image.
        if (!g_launchWorkspaceStop.load())
            LaunchWorkspace(*workspaceId);
        delete workspaceId;
    }
    return 0;
}

static void LaunchWorkspaceAsync(const std::wstring& workspaceId) {
    if (workspaceId.empty()) return;

    // Review #4 (Bug A): never drop a still-running launch thread. Track every
    // outstanding handle in g_launchWorkspaceThreads and only reap the ones that
    // have already finished; WhTool_ModUninit joins whatever is left. Also refuse to
    // start a new launch once teardown has begun, so we can't spawn a thread that
    // outlives WhTool_ModUninit's join (which is reachable from the UI thread's
    // "Restore Workspace" menu path).
    if (g_launchWorkspaceStop.load()) return;
    if (!g_launchCsInit) { LaunchWorkspace(workspaceId); return; }

    HANDLE h = NULL;
    std::wstring* ownedId = new std::wstring(workspaceId);

    EnterCriticalSection(&g_launchCs);
    // Reap any finished handles first (join point already satisfied).
    for (size_t i = 0; i < g_launchWorkspaceThreads.size();) {
        HANDLE t = g_launchWorkspaceThreads[i];
        if (WaitForSingleObject(t, 0) == WAIT_OBJECT_0) {
            CloseHandle(t);
            g_launchWorkspaceThreads.erase(g_launchWorkspaceThreads.begin() + i);
        } else {
            ++i;   // still running -- KEEP it so WhTool_ModUninit can join it
        }
    }
    h = CreateThread(NULL, 0, LaunchWorkspaceThread, ownedId, 0, NULL);
    if (h) g_launchWorkspaceThreads.push_back(h);
    LeaveCriticalSection(&g_launchCs);

    if (!h) {
        delete ownedId;
        LaunchWorkspace(workspaceId);   // fall back to synchronous launch
    }
}

static void OpenSingleFolderPath(const std::wstring& folderPath) {
    if (folderPath.empty()) return;
    if (g_restoreExecCsInit) EnterCriticalSection(&g_restoreExecCs);
    struct SingleGuard {
        ~SingleGuard() { if (g_restoreExecCsInit) LeaveCriticalSection(&g_restoreExecCs); }
    } singleGuard;
    HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    std::vector<OpenExplorerWindow> openWindows;
    std::vector<ExplorerTab> existingTabs = EnumerateExplorerTabs();
    for (const auto& tab : existingTabs)
        openWindows.push_back({ tab.hwnd, tab.folder });
    ReleaseExplorerTabs(existingTabs);

    int matchIdx = PickWorkspaceWindowToFocus(folderPath, openWindows);
    if (matchIdx >= 0) {
        HWND existing = openWindows[matchIdx].hwnd;
        if (existing && IsWindow(existing)) {
            if (IsIconic(existing)) ShowWindow(existing, SW_RESTORE);
            FocusExplorerTabByFolder(existing, openWindows[matchIdx].folder);
        }
        if (SUCCEEDED(hrInit)) CoUninitialize();
        return;
    }

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"open";
    sei.lpFile = folderPath.c_str();
    sei.nShow  = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
    if (SUCCEEDED(hrInit)) CoUninitialize();
}

static DWORD WINAPI OpenSingleFolderThread(LPVOID param) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    std::wstring* folderPath = (std::wstring*)param;
    if (folderPath) {
        if (!g_launchWorkspaceStop.load())
            OpenSingleFolderPath(*folderPath);
        delete folderPath;
    }
    return 0;
}

static void OpenSingleFolderAsync(const std::wstring& folderPath) {
    if (folderPath.empty() || g_launchWorkspaceStop.load()) return;
    if (!g_launchCsInit) { OpenSingleFolderPath(folderPath); return; }

    std::wstring* ownedPath = new std::wstring(folderPath);
    HANDLE h = NULL;
    EnterCriticalSection(&g_launchCs);
    for (size_t i = 0; i < g_launchWorkspaceThreads.size();) {
        HANDLE t = g_launchWorkspaceThreads[i];
        if (WaitForSingleObject(t, 0) == WAIT_OBJECT_0) {
            CloseHandle(t);
            g_launchWorkspaceThreads.erase(g_launchWorkspaceThreads.begin() + i);
        } else {
            ++i;
        }
    }
    h = CreateThread(NULL, 0, OpenSingleFolderThread, ownedPath, 0, NULL);
    if (h) g_launchWorkspaceThreads.push_back(h);
    LeaveCriticalSection(&g_launchCs);

    if (!h) {
        delete ownedPath;
        OpenSingleFolderPath(folderPath);
    }
}

struct RunningCtx { const wchar_t* path; HWND found; };
static BOOL CALLBACK FindRunningProc(HWND hwnd, LPARAM lp) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    // Functionality-note #3: the first visible top-level window of a process can
    // be a splash screen, a tooltip host or a hidden helper rather than the main
    // window. Skip tool windows and untitled windows so focus-if-running latches
    // onto a real app window. (Same idiom as RunStateEnumProc below.)
    if (GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindowTextLengthW(hwnd) == 0) return TRUE;
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

static void ReleaseExplorerTabs(std::vector<ExplorerTab>& tabs) {
    for (auto& tab : tabs) {
        if (tab.app) {
            tab.app->Release();
            tab.app = NULL;
        }
    }
    tabs.clear();
}

static HWND GetExplorerTabOrWindowHwnd(IWebBrowserApp* app, HWND* outRootHwnd = NULL) {
    if (outRootHwnd) *outRootHwnd = NULL;
    if (!app) return NULL;

    HWND tabHwnd = NULL;
    IServiceProvider* sp = NULL;
    if (SUCCEEDED(app->QueryInterface(IID_IServiceProvider, (void**)&sp)) && sp) {
        IShellBrowser* sb = NULL;
        if (SUCCEEDED(sp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&sb)) && sb) {
            sb->GetWindow(&tabHwnd);
            sb->Release();
        }
        sp->Release();
    }

    SHANDLE_PTR hwndShell = 0;
    HWND topHwnd = (SUCCEEDED(app->get_HWND(&hwndShell)) && hwndShell)
                 ? (HWND)hwndShell
                 : (tabHwnd ? GetAncestor(tabHwnd, GA_ROOT) : NULL);
    if (topHwnd) {
        HWND r = GetAncestor(topHwnd, GA_ROOT);
        if (r) topHwnd = r;
    }
    if (outRootHwnd) *outRootHwnd = topHwnd;
    return tabHwnd ? tabHwnd : topHwnd;
}

static std::vector<ExplorerTab> EnumerateExplorerTabs(HWND hwndFilter, bool includeVirtual) {
    std::vector<ExplorerTab> tabs;
    HWND filterRoot = hwndFilter ? GetAncestor(hwndFilter, GA_ROOT) : NULL;
    if (!filterRoot) filterRoot = hwndFilter;

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
                    HWND rootHwnd = NULL;
                    HWND tabHwnd  = GetExplorerTabOrWindowHwnd(app, &rootHwnd);
                    if ((includeVirtual || !path.empty()) && rootHwnd) {
                        if (!filterRoot || rootHwnd == filterRoot || tabHwnd == hwndFilter) {
                            ExplorerTab tab;
                            tab.app = app;
                            tab.hwnd = rootHwnd;
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
        Wh_Log(L"EXPLORER COM: ShellWindows unavailable hr=0x%08X", (unsigned)hr);
    }
    return tabs;
}

static IWebBrowserApp* FindExplorerAppForFolder(const std::wstring& folder, HWND* outHwnd = NULL) {
    IWebBrowserApp* found = NULL;
    std::vector<ExplorerTab> tabs = EnumerateExplorerTabs();
    for (auto& tab : tabs) {
        if (WorkspaceFoldersMatch(tab.folder, folder)) {
            found = tab.app;
            tab.app = NULL;
            if (outHwnd) *outHwnd = tab.hwnd;
            break;
        }
    }
    ReleaseExplorerTabs(tabs);
    return found;
}

static const DWORD kRestorePollIntervalMs = 20;

static IWebBrowserApp* WaitForNewExplorerAppForFolder(
    const std::wstring& folder,
    const std::vector<HWND>& existingWindows,
    DWORD timeoutMs,
    HWND* outHwnd = NULL) {
    DWORD start = GetTickCount();
    while (GetTickCount() - start < timeoutMs) {
        if (g_launchWorkspaceStop.load()) return NULL;
        std::vector<ExplorerTab> tabs = EnumerateExplorerTabs();
        for (auto& tab : tabs) {
            bool knownWindow = false;
            for (HWND known : existingWindows) {
                if (tab.hwnd == known) {
                    knownWindow = true;
                    break;
                }
            }
            if (!knownWindow && WorkspaceFoldersMatch(tab.folder, folder)) {
                IWebBrowserApp* app = tab.app;
                tab.app = NULL;
                if (outHwnd) *outHwnd = tab.hwnd;
                ReleaseExplorerTabs(tabs);
                return app;
            }
        }
        ReleaseExplorerTabs(tabs);
        if (WaitWithStaPump(kRestorePollIntervalMs))
            return NULL;
    }
    if (g_launchWorkspaceStop.load()) return NULL;
    return FindExplorerAppForFolder(folder, outHwnd);
}

static bool NavigateExplorerTabByPidlVariant(IWebBrowserApp* app, PIDLIST_ABSOLUTE pidl) {
    if (!app || !pidl) return false;
    UINT cb = ILGetSize(pidl);
    if (cb == 0) return false;
    SAFEARRAY* psa = SafeArrayCreateVector(VT_UI1, 0, cb);
    if (!psa) return false;
    void* pData = NULL;
    HRESULT hr = E_FAIL;
    if (SUCCEEDED(SafeArrayAccessData(psa, &pData)) && pData) {
        memcpy(pData, pidl, cb);
        SafeArrayUnaccessData(psa);
        VARIANT vPidl;
        VariantInit(&vPidl);
        vPidl.vt = VT_ARRAY | VT_UI1;
        vPidl.parray = psa;
        VARIANT vOpt;
        VariantInit(&vOpt);
        vOpt.vt = VT_ERROR;
        vOpt.scode = DISP_E_PARAMNOTFOUND;
        IWebBrowser2* wb2 = NULL;
        if (SUCCEEDED(app->QueryInterface(IID_IWebBrowser2, (void**)&wb2)) && wb2) {
            hr = wb2->Navigate2(&vPidl, &vOpt, &vOpt, &vOpt, &vOpt);
            wb2->Release();
        }
        VariantClear(&vPidl); // destroys psa
    } else {
        SafeArrayDestroy(psa);
    }
    return SUCCEEDED(hr);
}

static bool NavigateExplorerTab(IWebBrowserApp* app, const std::wstring& folder) {
    if (!app || folder.empty()) return false;

    PIDLIST_ABSOLUTE pidl = NULL;
    SFGAOF sfgao = 0;
    std::wstring parseTarget = folder;
    if (parseTarget.size() >= MAX_PATH && parseTarget.rfind(L"\\\\?\\", 0) != 0) {
        parseTarget = (parseTarget.rfind(L"\\\\", 0) == 0)
            ? (L"\\\\?\\UNC\\" + parseTarget.substr(2))
            : (L"\\\\?\\" + parseTarget);
    }
    if (FAILED(SHParseDisplayName(parseTarget.c_str(), NULL, &pidl, 0, &sfgao)) || !pidl)
        pidl = ILCreateFromPathW(folder.c_str());

    if (pidl) {
        // 1. Primary: IWebBrowser2::Navigate2 with PIDL SAFEARRAY (Simon Mourier / AHK v2 reference).
        // Updates both the shell view AND the tab title/address bar cleanly from Home / This PC.
        bool nav2Ok = NavigateExplorerTabByPidlVariant(app, pidl);

        // 2. Secondary: IShellBrowser::BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE).
        bool browseOk = false;
        if (!nav2Ok) {
            IServiceProvider* sp = NULL;
            if (SUCCEEDED(app->QueryInterface(IID_IServiceProvider, (void**)&sp)) && sp) {
                IShellBrowser* sb = NULL;
                if (SUCCEEDED(sp->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&sb)) && sb) {
                    browseOk = SUCCEEDED(sb->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE));
                    sb->Release();
                }
                sp->Release();
            }
        }
        CoTaskMemFree(pidl);
        if (nav2Ok || browseOk) return true;
    }

    // 3. Fallback: IWebBrowser2::Navigate2(BSTR) or IWebBrowserApp::Navigate(BSTR)
    BSTR target = SysAllocString(folder.c_str());
    if (!target) return false;
    VARIANT empty;
    VariantInit(&empty);
    HRESULT hr = app->Navigate(target, &empty, &empty, &empty, &empty);
    SysFreeString(target);
    return SUCCEEDED(hr);
}

static bool NavigateAndVerifyExplorerTab(IWebBrowserApp* tab, const std::wstring& folder) {
    if (!tab || folder.empty()) return false;

    // Wait briefly for the newly created '+' tab's initial Home/This PC load to settle.
    if (WaitWithStaPump(60))
        return false;
    DWORD settleStart = GetTickCount();
    while (GetTickCount() - settleStart < 450) {
        if (g_launchWorkspaceStop.load()) return false;
        VARIANT_BOOL busy = VARIANT_FALSE;
        tab->get_Busy(&busy);
        if (busy == VARIANT_FALSE)
            break;
        if (WaitWithStaPump(20))
            return false;
    }

    if (!NavigateExplorerTab(tab, folder))
        return false;

    // Verify the tab landed on the target folder (or re-issue once if Home/This PC
    // finished loading right after our first Navigate2 call).
    DWORD navStart = GetTickCount();
    bool retried = false;
    while (GetTickCount() - navStart < 950) {
        if (g_launchWorkspaceStop.load()) return true;
        std::wstring cur = FolderPathFromExplorer(tab);
        if (!cur.empty() && WorkspaceFoldersMatch(cur, folder))
            return true;
        VARIANT_BOOL busy = VARIANT_FALSE;
        tab->get_Busy(&busy);
        if (!retried && busy == VARIANT_FALSE && (GetTickCount() - navStart) >= 220) {
            retried = true;
            NavigateExplorerTab(tab, folder);
        }
        if (WaitWithStaPump(25))
            return true;
    }
    return true;
}

static IUIAutomationElement* FindNamedInScope(IUIAutomation* automation,
                                              IUIAutomationElement* root,
                                              const wchar_t* name,
                                              enum TreeScope scope) {
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
        root->FindFirst(scope, cond, &found);
        cond->Release();
    }
    VariantClear(&v);
    return found;
}

static IUIAutomationElement* FindNamedDescendant(IUIAutomation* automation,
                                                 IUIAutomationElement* root,
                                                 const wchar_t* name) {
    return FindNamedInScope(automation, root, name, TreeScope_Subtree);
}

static IUIAutomationElement* FindExplorerTabBar(IUIAutomation* automation,
                                                IUIAutomationElement* root) {
    if (!automation || !root) return NULL;
    VARIANT v;
    VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = UIA_TabControlTypeId;
    IUIAutomationCondition* cond = NULL;
    IUIAutomationElement* tabBar = NULL;
    if (SUCCEEDED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond)) && cond) {
        root->FindFirst(TreeScope_Subtree, cond, &tabBar);
        cond->Release();
    }
    VariantClear(&v);
    return tabBar;
}

static BOOL CALLBACK FindShellTabChildProc(HWND child, LPARAM lParam) {
    wchar_t cls[64] = {};
    if (GetClassNameW(child, cls, ARRAYSIZE(cls)) &&
        wcscmp(cls, L"ShellTabWindowClass") == 0) {
        *(HWND*)lParam = child;
        return FALSE;
    }
    return TRUE;
}

static BOOL CALLBACK CountShellTabChildrenProc(HWND child, LPARAM lParam) {
    wchar_t cls[64] = {};
    if (GetClassNameW(child, cls, ARRAYSIZE(cls)) &&
        wcscmp(cls, L"ShellTabWindowClass") == 0) {
        (*(int*)lParam)++;
    }
    return TRUE;
}

static int CountShellTabChildren(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return 0;
    int count = 0;
    EnumChildWindows(hwnd, CountShellTabChildrenProc, (LPARAM)&count);
    return count;
}

static bool InvokeExplorerNewTabUIA(IUIAutomation* automation, HWND hwnd) {
    if (!automation || !hwnd || !IsWindow(hwnd)) return false;
    bool ok = false;
    IUIAutomationElement* root = NULL;
    if (SUCCEEDED(automation->ElementFromHandle(hwnd, &root)) && root) {
        // 1. Try AutomationId == "AddButton" (official Windows 11 XAML TabView '+' button id,
        // language-neutral across all Windows locales -- Simon Mourier reference).
        VARIANT vId;
        VariantInit(&vId);
        vId.vt = VT_BSTR;
        vId.bstrVal = SysAllocString(L"AddButton");
        if (vId.bstrVal) {
            IUIAutomationCondition* idCond = NULL;
            if (SUCCEEDED(automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, vId, &idCond)) && idCond) {
                IUIAutomationElement* addBtn = NULL;
                root->FindFirst(TreeScope_Subtree, idCond, &addBtn);
                idCond->Release();
                if (addBtn) {
                    IUIAutomationInvokePattern* invoke = NULL;
                    if (SUCCEEDED(addBtn->GetCurrentPatternAs(UIA_InvokePatternId,
                                                              IID_IUIAutomationInvokePattern,
                                                              (void**)&invoke)) && invoke) {
                        ok = SUCCEEDED(invoke->Invoke());
                        invoke->Release();
                    }
                    addBtn->Release();
                }
            }
            VariantClear(&vId);
        }

        // 2. Fallback: localized Name "New tab" / "Add new tab"
        if (!ok) {
            IUIAutomationElement* tabBar = FindExplorerTabBar(automation, root);
            IUIAutomationElement* scope  = tabBar ? tabBar : root;
            IUIAutomationElement* newTab = FindNamedInScope(automation, scope, L"New tab", TreeScope_Descendants);
            if (!newTab)
                newTab = FindNamedInScope(automation, scope, L"Add new tab", TreeScope_Descendants);
            if (!newTab && tabBar) {
                newTab = FindNamedDescendant(automation, root, L"New tab");
                if (!newTab)
                    newTab = FindNamedDescendant(automation, root, L"Add new tab");
            }
            if (tabBar) tabBar->Release();
            if (newTab) {
                IUIAutomationInvokePattern* invoke = NULL;
                if (SUCCEEDED(newTab->GetCurrentPatternAs(UIA_InvokePatternId,
                                                          IID_IUIAutomationInvokePattern,
                                                          (void**)&invoke)) && invoke) {
                    ok = SUCCEEDED(invoke->Invoke());
                    invoke->Release();
                }
                newTab->Release();
            }
        }
        root->Release();
    }
    return ok;
}

static long GetShellWindowsTotalCount() {
    long count = 0;
    IShellWindows* sw = NULL;
    if (SUCCEEDED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
                                   IID_IShellWindows, (void**)&sw)) && sw) {
        sw->get_Count(&count);
        sw->Release();
    }
    return count;
}

static IWebBrowserApp* GetNewestShellWindowForRoot(HWND targetRoot,
                                                   const std::vector<HWND>& prevTabHwnds) {
    IWebBrowserApp* chosen = NULL;
    IShellWindows* sw = NULL;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
                                IID_IShellWindows, (void**)&sw)) || !sw)
        return NULL;

    long count = 0;
    sw->get_Count(&count);
    HWND wantRoot = targetRoot ? GetAncestor(targetRoot, GA_ROOT) : NULL;
    if (!wantRoot) wantRoot = targetRoot;

    // Walk backward from the most recently registered ShellWindow (count - 1 down to 0).
    for (long i = count - 1; i >= 0 && !chosen; --i) {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;
        IDispatch* disp = NULL;
        if (SUCCEEDED(sw->Item(v, &disp)) && disp) {
            IWebBrowserApp* app = NULL;
            if (SUCCEEDED(disp->QueryInterface(IID_IWebBrowserApp, (void**)&app)) && app) {
                HWND rootHwnd = NULL;
                HWND tabHwnd  = GetExplorerTabOrWindowHwnd(app, &rootHwnd);
                bool sameWin  = (!wantRoot || rootHwnd == wantRoot);
                bool wasKnown = (tabHwnd != NULL) &&
                    (std::find(prevTabHwnds.begin(), prevTabHwnds.end(), tabHwnd) != prevTabHwnds.end());
                if (sameWin && !wasKnown) {
                    chosen = app;
                    app = NULL;
                }
                if (app) app->Release();
            }
            disp->Release();
        }
        VariantClear(&v);
    }

    // If all tabs had matching HWNDs (rare), take the highest-index item belonging to wantRoot.
    if (!chosen && count > 0) {
        for (long i = count - 1; i >= 0 && !chosen; --i) {
            VARIANT v;
            VariantInit(&v);
            v.vt = VT_I4;
            v.lVal = i;
            IDispatch* disp = NULL;
            if (SUCCEEDED(sw->Item(v, &disp)) && disp) {
                IWebBrowserApp* app = NULL;
                if (SUCCEEDED(disp->QueryInterface(IID_IWebBrowserApp, (void**)&app)) && app) {
                    HWND rootHwnd = NULL;
                    GetExplorerTabOrWindowHwnd(app, &rootHwnd);
                    if (!wantRoot || rootHwnd == wantRoot) {
                        chosen = app;
                        app = NULL;
                    }
                    if (app) app->Release();
                }
                disp->Release();
            }
            VariantClear(&v);
        }
    }

    sw->Release();
    return chosen;
}

static IWebBrowserApp* CreateAndAttachNewExplorerTab(IUIAutomation* automation,
                                                     HWND hwnd,
                                                     DWORD timeoutMs) {
    if (!hwnd || !IsWindow(hwnd)) return NULL;

    // Wait until the Explorer window has at least 1 initialized ShellTabWindowClass child
    // (newly spawned explorer.exe /n windows create their XAML tab host asynchronously).
    DWORD readyWait = GetTickCount();
    while (GetTickCount() - readyWait < 2000) {
        if (g_launchWorkspaceStop.load()) return NULL;
        if (CountShellTabChildren(hwnd) >= 1) break;
        if (WaitWithStaPump(20))
            return NULL;
    }

    std::vector<HWND> prevTabHwnds;
    {
        std::vector<ExplorerTab> existing = EnumerateExplorerTabs(hwnd, true);
        for (const auto& t : existing) {
            HWND th = GetExplorerTabOrWindowHwnd(t.app, NULL);
            if (th) prevTabHwnds.push_back(th);
        }
        ReleaseExplorerTabs(existing);
    }
    int  beforeChildTabs = CountShellTabChildren(hwnd);
    long beforeSwCount   = GetShellWindowsTotalCount();

    // Step 1: SendMessageTimeoutW WM_COMMAND 0xA21B to ShellTabWindowClass (never hangs if Explorer UI is busy).
    HWND tabChild = NULL;
    EnumChildWindows(hwnd, FindShellTabChildProc, (LPARAM)&tabChild);
    if (tabChild && IsWindow(tabChild)) {
        DWORD_PTR cmdRes = 0;
        if (!SendMessageTimeoutW(tabChild, WM_COMMAND, 0xA21B, 0,
                                 SMTO_ABORTIFHUNG | SMTO_NORMAL, 600, &cmdRes)) {
            PostMessageW(tabChild, WM_COMMAND, 0xA21B, 0);
        }
    } else {
        InvokeExplorerNewTabUIA(automation, hwnd);
    }

    bool uiaRetried   = false;
    bool ctrlTRetried = false;
    DWORD start = GetTickCount();
    while (GetTickCount() - start < timeoutMs) {
        if (g_launchWorkspaceStop.load()) return NULL;

        long nowSwCount   = GetShellWindowsTotalCount();
        int  nowChildTabs = CountShellTabChildren(hwnd);

        if (nowSwCount > beforeSwCount && nowChildTabs > beforeChildTabs) {
            IWebBrowserApp* newTab = GetNewestShellWindowForRoot(hwnd, prevTabHwnds);
            if (newTab) return newTab;
        }

        DWORD elapsed = GetTickCount() - start;
        if (!uiaRetried && nowChildTabs <= beforeChildTabs && elapsed >= 350) {
            uiaRetried = true;
            InvokeExplorerNewTabUIA(automation, hwnd);
        }
        if (!ctrlTRetried && nowChildTabs <= beforeChildTabs && elapsed >= 900) {
            ctrlTRetried = true;
            ForceForegroundWindow(hwnd);
            INPUT inputs[4] = {};
            inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_CONTROL;
            inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = 'T';
            inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = 'T';        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
            inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_CONTROL; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(4, inputs, sizeof(INPUT));
        }

        if (WaitWithStaPump(kRestorePollIntervalMs))
            return NULL;
    }
    return (CountShellTabChildren(hwnd) > beforeChildTabs)
         ? GetNewestShellWindowForRoot(hwnd, prevTabHwnds)
         : NULL;
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
        ForceForegroundWindow(hwnd);
        return true;
    }
    return selected;
}

static void RestoreExplorerWindowGroup(const WorkspaceSnapshot::WindowGroup& group) {
    if (group.tabPaths.empty()) return;

    HRESULT hrInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hrInit) && hrInit != RPC_E_CHANGED_MODE) {
        Wh_Log(L"EXPLORER RESTORE: CoInitializeEx failed hr=0x%08X", (unsigned)hrInit);
        return;
    }

    Wh_Log(L"EXPLORER RESTORE: begin tabs=%d active=%d",
                  (int)group.tabPaths.size(), group.activeTab);

    std::vector<HWND> existingExplorerWindows;
    std::vector<OpenExplorerWindow> openWindows;
    {
        std::vector<ExplorerTab> existingTabs = EnumerateExplorerTabs();
        for (const auto& tab : existingTabs) {
            if (std::find(existingExplorerWindows.begin(), existingExplorerWindows.end(), tab.hwnd)
                == existingExplorerWindows.end())
                existingExplorerWindows.push_back(tab.hwnd);
            openWindows.push_back({ tab.hwnd, tab.folder });
        }
        ReleaseExplorerTabs(existingTabs);
    }

    {
        int activeIdx = std::max(0, std::min(group.activeTab,
                                             (int)group.tabPaths.size() - 1));
        std::vector<std::wstring> candidateFolders;
        candidateFolders.push_back(group.tabPaths[activeIdx]);
        for (int i = 0; i < (int)group.tabPaths.size(); ++i)
            if (i != activeIdx) candidateFolders.push_back(group.tabPaths[i]);

        bool requireAll = REOPEN_CLOSED_PATHS_ON_FOCUS.load();
        int anyMatchIdx = PickWorkspaceWindowToFocusAny(candidateFolders, openWindows);
        if (!requireAll && anyMatchIdx >= 0) {
            const std::wstring& primaryFolder = openWindows[anyMatchIdx].folder;
            HWND existing = openWindows[anyMatchIdx].hwnd;
            if (existing && IsWindow(existing)) {
                if (IsIconic(existing)) ShowWindow(existing, SW_RESTORE);
                FocusExplorerTabByFolder(existing, primaryFolder);
            }
            Wh_Log(L"EXPLORER RESTORE: focused already-open window (any-match) folder=%s",
                   primaryFolder.c_str());
            if (SUCCEEDED(hrInit)) CoUninitialize();
            return;
        }
        if (requireAll && anyMatchIdx >= 0) {
            HWND targetHwnd = openWindows[anyMatchIdx].hwnd;
            std::vector<std::wstring> missingFolders;
            for (const auto& f : group.tabPaths) {
                if (!f.empty() && PickWorkspaceWindowToFocus(f, openWindows) < 0)
                    missingFolders.push_back(f);
            }
            if (missingFolders.empty()) {
                if (targetHwnd && IsWindow(targetHwnd)) {
                    if (IsIconic(targetHwnd)) ShowWindow(targetHwnd, SW_RESTORE);
                    FocusExplorerTabByFolder(targetHwnd, openWindows[anyMatchIdx].folder);
                }
                if (SUCCEEDED(hrInit)) CoUninitialize();
                return;
            }
            if (targetHwnd && IsWindow(targetHwnd)) {
                if (IsIconic(targetHwnd)) ShowWindow(targetHwnd, SW_RESTORE);
                IUIAutomation* sharedAutomation = NULL;
                CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                 IID_IUIAutomation, (void**)&sharedAutomation);
                for (const auto& mf : missingFolders) {
                    if (g_launchWorkspaceStop.load()) break;
                    IWebBrowserApp* tab = CreateAndAttachNewExplorerTab(sharedAutomation, targetHwnd, 3500);
                    if (tab) {
                        NavigateAndVerifyExplorerTab(tab, mf);
                        tab->Release();
                    }
                }
                if (sharedAutomation) sharedAutomation->Release();
                FocusExplorerTabByFolder(targetHwnd, missingFolders.back());
                if (SUCCEEDED(hrInit)) CoUninitialize();
                return;
            }
        }
    }

    // Launch the first folder directly via Shell "open" verb (no "/n," flag) so Windows 11
    // File Explorer initializes its normal tabbed frame instead of a standalone "/n" instance.
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"open";
    sei.lpFile = group.tabPaths[0].c_str();
    sei.nShow  = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"EXPLORER RESTORE: ShellExecuteEx failed err=%lu folder=%s",
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
        Wh_Log(L"EXPLORER RESTORE: first window attach timeout folder=%s", group.tabPaths[0].c_str());
        if (SUCCEEDED(hrInit)) CoUninitialize();
        return;
    }

    // Wait for the first tab to finish its initial load; only call NavigateAndVerifyExplorerTab
    // if it did not already land on group.tabPaths[0] (avoids redundant double-navigation race).
    {
        std::wstring firstCur = FolderPathFromExplorer(first);
        if (firstCur.empty() || !WorkspaceFoldersMatch(firstCur, group.tabPaths[0])) {
            NavigateAndVerifyExplorerTab(first, group.tabPaths[0]);
        } else {
            WaitWithStaPump(80);
        }
    }
    first->Release();

    IUIAutomation* sharedAutomation = NULL;
    HRESULT hrUia = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                     IID_IUIAutomation, (void**)&sharedAutomation);
    if (FAILED(hrUia) || !sharedAutomation)
        Wh_Log(L"EXPLORER RESTORE: CUIAutomation unavailable hr=0x%08X", (unsigned)hrUia);

    int restored = 1;
    for (size_t i = 1; i < group.tabPaths.size(); ++i) {
        if (g_launchWorkspaceStop.load()) break;
        IWebBrowserApp* tab = CreateAndAttachNewExplorerTab(sharedAutomation, hwnd, 3500);
        if (tab) {
            if (NavigateAndVerifyExplorerTab(tab, group.tabPaths[i]))
                restored++;
            tab->Release();
        } else {
            Wh_Log(L"EXPLORER RESTORE: failed to create tab index=%d folder=%s",
                   (int)i, group.tabPaths[i].c_str());
        }
    }

    if (sharedAutomation) sharedAutomation->Release();

    int active = std::max(0, std::min(group.activeTab, (int)group.tabPaths.size() - 1));
    if (active < (int)group.tabPaths.size())
        FocusExplorerTabByFolder(hwnd, group.tabPaths[active]);
    else if (hwnd && IsWindow(hwnd))
        ForceForegroundWindow(hwnd);

    Wh_Log(L"EXPLORER RESTORE: complete restored=%d requested=%d hwnd=%p",
                  restored, (int)group.tabPaths.size(), hwnd);
    if (SUCCEEDED(hrInit)) CoUninitialize();
}

// Force a window to the foreground even when a DIFFERENT app currently holds
// the foreground. A bare SetForegroundWindow() from explorer's worker thread is
// REFUSED by Windows' foreground lock when another thread owns the foreground
// (SPI_SETFOREGROUNDLOCKTIMEOUT) -- the call silently fails and the taskbar
// button just flashes. This is why clicks were "skipped" while the real
// windhawk.exe (or any app) was foreground.
//
// Fix: attach our input queue to the current foreground thread's input queue so
// the OS treats the call as coming from the foreground thread, then set it.
// The attach/detach decision mirrors tests/foreground_focus.h PlanForegroundFocus.
static void ForceForegroundWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;

    if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);

    HWND  fg          = GetForegroundWindow();
    DWORD callerTid   = GetCurrentThreadId();
    DWORD targetTid   = GetWindowThreadProcessId(hwnd, NULL);
    DWORD foregroundTid = fg ? GetWindowThreadProcessId(fg, NULL) : 0;

    // Decide whether we must attach (see tests/foreground_focus.h).
    bool  needsAttach = (foregroundTid != 0 &&
                         foregroundTid != targetTid &&
                         foregroundTid != callerTid);

    bool attached = false;
    if (needsAttach && fg && IsWindow(fg) && !IsHungAppWindow(fg))
        attached = (AttachThreadInput(callerTid, foregroundTid, TRUE) != FALSE);

    // Nudge past the lock, then take foreground.
    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);
    if (targetTid == callerTid)
        SetActiveWindow(hwnd);

    if (attached) AttachThreadInput(callerTid, foregroundTid, FALSE);
}

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
        // Use the foreground-lock-aware path: a bare SetForegroundWindow is
        // refused when another app (e.g. windhawk.exe) currently holds the
        // foreground, so the click appeared to do nothing.
        ForceForegroundWindow(running);
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

    // FIX-C3 (#7): persist to Windhawk mod storage (reversible on uninstall)
    // instead of HKCU. The exact same double-null-terminated blob is written
    // verbatim via Wh_SetBinaryValue; its byte length is stored alongside so
    // LoadPinnedApps can size its read buffer (Wh_GetBinaryValue copies nothing
    // into a too-small buffer and gives no way to query the needed size). This is
    // still outside the CS, so the (now local, fast) storage write can't block
    // the animation/input loop.
    size_t blobBytes = multiSz.size() * sizeof(wchar_t);
    Wh_SetBinaryValue(REG_VALUE, multiSz.c_str(), blobBytes);
    Wh_SetIntValue(L"PinnedAppsBytes", (int)blobBytes);
}

// Remove invalid entries from the list.
// Must be called WITHOUT the critical section held.
static void ValidateAndCleanPinnedList() {
    // Review (Optional): do NOT hold g_cs across LoadWorkspaceSnapshot's storage
    // I/O (Wh_GetIntValue/Wh_GetStringValue). The old code held the lock for the
    // whole pass, stalling the worker (reorder/reposition) on disk latency for
    // every workspace pin. Split into three phases:
    //   1. Under lock: decide app-pin verdicts (in-memory only) and COPY the
    //      workspace ids that need a storage lookup. No I/O under the lock.
    //   2. Lock released: run LoadWorkspaceSnapshot per copied id -> collect the
    //      ids that are stale/missing.
    //   3. Re-take the lock and erase the entries now known invalid, matching by
    //      identity (type + id/path) so a concurrent PinApp/reorder between the
    //      phases can't make us erase the wrong row.
    std::vector<std::wstring> workspaceIdsToCheck;   // phase 1 -> phase 2
    std::vector<std::wstring> invalidExePaths;       // app pins invalid (phase 1)
    std::vector<std::wstring> invalidWorkspaceIds;   // phase 3 erase set

    // ---- Phase 1: snapshot under the lock, no storage I/O ----
    EnterCriticalSection(&g_cs);
    for (size_t i = 0; i < g_pinnedApps.size(); ++i) {
        const auto& e = g_pinnedApps[i];
        if (e.type == PIN_WORKSPACE) {
            if (!e.icon || e.workspaceId.empty())
                invalidWorkspaceIds.push_back(e.workspaceId);   // decided already
            else
                workspaceIdsToCheck.push_back(e.workspaceId);   // needs a lookup
        } else {
            if (!e.icon || e.exePath.empty()
                || IsExplorerExePath(e.exePath) || IsExcludedApp(e.exePath))
                invalidExePaths.push_back(e.exePath);
        }
    }
    LeaveCriticalSection(&g_cs);

    // ---- Phase 2: storage I/O with the lock RELEASED ----
    for (const auto& id : workspaceIdsToCheck) {
        WorkspaceSnapshot snapshot;
        if (!LoadWorkspaceSnapshot(id, snapshot))
            invalidWorkspaceIds.push_back(id);
    }

    if (invalidExePaths.empty() && invalidWorkspaceIds.empty())
        return;   // nothing to remove -- skip the re-lock entirely

    // ---- Phase 3: re-take the lock and erase, matching by identity ----
    EnterCriticalSection(&g_cs);
    for (int i = (int)g_pinnedApps.size() - 1; i >= 0; --i) {
        const auto& e = g_pinnedApps[i];
        bool invalid = false;
        if (e.type == PIN_WORKSPACE) {
            for (const auto& id : invalidWorkspaceIds)
                if (e.workspaceId == id) { invalid = true; break; }
        } else {
            for (const auto& path : invalidExePaths)
                if (e.exePath == path) { invalid = true; break; }
        }
        if (invalid) {
            FreePinnedAppResources(g_pinnedApps[i]);
            g_pinnedApps.erase(g_pinnedApps.begin() + i);
            Wh_Log(L"VALIDATE: removed invalid entry at index %d", i);
        }
    }
    LeaveCriticalSection(&g_cs);
}

void LoadPinnedApps() {
    g_pinnedApps.clear();

    // FIX-C3 (#7): read the pinned-apps blob from Windhawk mod storage instead of
    // HKCU. "PinnedAppsBytes" (0 when unset) gives the exact byte length so we can
    // size the read buffer -- Wh_GetBinaryValue copies nothing into a too-small one.
    int savedBytes = Wh_GetIntValue(L"PinnedAppsBytes", 0);
    if (savedBytes > (int)sizeof(wchar_t)) {

        std::vector<BYTE> buf((size_t)savedBytes);
        if (Wh_GetBinaryValue(REG_VALUE, buf.data(), buf.size()) == (size_t)savedBytes) {
            const wchar_t* p = (const wchar_t*)buf.data();

            int appCap = std::max(1, MAX_PINNED_APPS);
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
                        Wh_Log(L"LOAD SKIP (workspace missing): %s", workspaceId.c_str());
                        continue;
                    }
                    HICON icon = LoadFolderIcon(snapshot.folderPaths[0]);
                    if (!icon) {
                        Wh_Log(L"LOAD SKIP (workspace no icon): %s", workspaceId.c_str());
                        continue;
                    }
                    if (snapshot.folderPaths.size() <= 1) {
                        snapshot.colorHex.clear();
                    } else {
                        BYTE cr = 0, cg = 0, cb = 0;
                        if (snapshot.colorHex.empty() ||
                            !ParseHexColor(snapshot.colorHex, cr, cg, cb) ||
                            IsExcludedFolderColor(cr, cg, cb)) {
                            snapshot.colorHex = PickGroupedWorkspaceColor(CountPinsByType(PIN_WORKSPACE));
                            SaveWorkspaceSnapshot(snapshot);
                        }
                    }
                    PinnedApp app;
                    app.type         = PIN_WORKSPACE;
                    app.workspaceId  = workspaceId;
                    app.workspaceSig   = MakeWorkspaceSignature(snapshot.folderPaths);
                    app.colorHex       = snapshot.colorHex;
                    app.workspacePaths = snapshot.folderPaths;
                    app.customNamed    = snapshot.customNamed;
                    app.displayName    = (snapshot.customNamed && !snapshot.displayName.empty())
                                     ? snapshot.displayName
                                     : (displayName.empty() ? snapshot.displayName : displayName);
                    app.icon        = icon;
                    app.opacity     = 1.0f;
                    app.isNew       = false;
                    app.velocityX   = 0.f;
                    g_pinnedApps.push_back(app);
                    Wh_Log(L"LOAD WORKSPACE OK: %s", workspaceId.c_str());
                    continue;
                }

                if (CountPinsByType(PIN_APP) >= appCap) continue;
                if (IsExplorerExePath(exePath) || IsExcludedApp(exePath)) continue;

                HICON icon = LoadAppIconStrict(exePath);
                if (!icon) {
                    Wh_Log(L"LOAD SKIP (no icon): %s", exePath.c_str());
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
                Wh_Log(L"LOAD OK: %s", exePath.c_str());
            }
        }
    }

    // ValidateAndCleanPinnedList acquires the CS internally  --  must be called outside CS
    ValidateAndCleanPinnedList();

    int total = (int)g_pinnedApps.size();
    for (int i = 0; i < total; ++i) {
        RECT r = GetIconRectLocal(i, total);
        g_pinnedApps[i].currentX = g_pinnedApps[i].targetX = (float)r.left;
        g_pinnedApps[i].hoverScale  = 1.0f;
        g_pinnedApps[i].hoverShiftX = 0.f;
    }
    Wh_Log(L"LOAD DONE: %d apps", total);
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
    extern bool EwpPtInAnyWindow(POINT pt);
    if (!g_dragFromDock && !IsCursorInDockOrTaskbarRegion(cursorPt) && !EwpPtInAnyWindow(cursorPt)) {
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

// ============================================================
//  CINEMATIC LOCK / UNLOCK FLASH  --  per-pixel-alpha glow layer
//  A brief, premium bloom shown when the icon lock TOGGLES (or a locked gesture-
//  unpin is refused). Unlike the dock's own colour-key overlay (opaque for any
//  non-key pixel, so it could only ever fake depth with a flat stroke -- the old
//  "thin line"), this renders on its OWN click-through, per-pixel-alpha layered
//  window -- the same proven recipe as the drag tether/ghost (CreateDIBSection +
//  UpdateLayeredWindow/ULW_ALPHA) -- so it paints a TRUE soft bloom that spills
//  OUTSIDE the dock's rounded edge.
//    * LOCK    -> LOCKGLOW_SEAL:    warm GOLD ring that CONVERGES onto the edge.
//    * UNLOCK  -> LOCKGLOW_RELEASE: cool GREEN ripple that EXPANDS outward.
//  Lazily created on the first flash, moved + repainted each worker tick while
//  the flash is live, hidden the instant it ends (a locked-and-idle dock shows
//  nothing), and destroyed alongside the other layered windows in both uninit
//  paths. Purely visual: it never gates or delays the real lock/pin logic.
// ============================================================

// Pure, Win32-free decision for the drag-to-pin feedback glow (mirrored 1:1 from
// tests/edge_feedback_line.h). The old right-edge feedback LINE was removed; drag
// feedback now reuses the lock-glow bloom. It shows:
//   * only for a PIN drag (a NEW app dragged IN) -- never an unpin / reorder /
//     rope-break drag of an already-pinned icon,
//   * only while the cursor is INSIDE the dock drop zone (never eager),
//   * RED the moment it enters a FULL dock (pin limit) -- never green-then-red;
//     GREEN while there is still room.
// The worker samples this every drag frame, holds it while the icon stays, and
// tears it down the instant it returns DRAGGLOW_NONE (left zone / released / drop).
enum DragGlow { DRAGGLOW_NONE, DRAGGLOW_GREEN, DRAGGLOW_RED };
static inline DragGlow DragGlowFor(bool pinningDrag, bool inDropZone, bool dockFull) {
    if (!pinningDrag || !inDropZone) return DRAGGLOW_NONE;
    return dockFull ? DRAGGLOW_RED : DRAGGLOW_GREEN;
}

// Pure, Win32-free companion to DragGlowFor (mirrored 1:1 in
// tests/pin_limit_calm.h). Computes the `pinningDrag` input: a drag only earns
// new-pin feedback (GREEN "will add" / RED "dock full" + shake) when it
// genuinely adds a NEW pin -- an EXTERNAL drag (fromDock == false) of an app
// that is NOT already pinned. Dragging or deleting an already-pinned dock icon
// (reorder / pull-off unpin), or dropping an already-pinned app back on the dock
// (a no-op PinApp dedups), STAYS CALM in BOTH limit states (full or with room).
static inline bool IsNewPinFeedbackDrag(bool fromDock, bool alreadyPinned) {
    return !fromDock && !alreadyPinned;
}

// Begins (or restarts) an interactive lock-glow flash with an EXPLICIT colour
// kind. Shared core: the drag-feedback sites call it directly with the colour
// they want -- LOCKGLOW_LIMIT = red "dock full" (pin limit), LOCKGLOW_RELEASE =
// green "entered drop zone". Safe from either thread: it only writes one DWORD
// and posts a cross-thread-safe InvalidateRect.
static void TriggerLockGlowKind(LockGlowKind kind) {
    g_lockGlowKind  = kind;
    // Pick the confined render mode from the user toggle (default OFF = edge
    // highlight only; ON = full left->right sweep). Latched per flash so a
    // mid-flash settings change can't tear the animation.
    g_lockGlowMode  = LockGlowModeFromSetting(ENABLE_LOCK_ANIMATION);
    g_lockGlowStart = GetTickCount();
    if (g_lockGlowStart == 0) g_lockGlowStart = 1;   // 0 is the "idle" sentinel
    // The flash animates on its own per-pixel-alpha layer (RenderLockGlow),
    // pumped by the worker loop; nudge the overlay once so dependent chrome
    // refreshes cleanly.
    if (g_overlayWnd && IsWindow(g_overlayWnd))
        InvalidateRect(g_overlayWnd, NULL, FALSE);
}

// Lock/unlock gesture entry point (unchanged callers): picks the effect from the
// CURRENT (already-updated) lock state -- locked => gold seal-in, unlocked =>
// green release. A blocked (locked) gesture-unpin also lands here while locked
// and so shows gold -- the intended "it's locked" feedback.
static void TriggerLockGlow() {
    // Derive the effect from the CURRENT lock state, then fire it. The explicit
    // g_lockGlowKind assignment is kept here (not only inside the shared core) so
    // it stays obvious -- and the regression gate can assert -- that the lock/
    // unlock glow colour comes straight from g_iconsLocked at trigger time.
    g_lockGlowKind = LockGlowKindFromLocked(g_iconsLocked.load());
    TriggerLockGlowKind(g_lockGlowKind);
}

#ifndef WDA_NONE
#define WDA_NONE 0x00000000
#endif
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

static HBITMAP   g_tooltipDIB         = NULL;
static HDC       g_tooltipDC          = NULL;
static HBITMAP   g_tooltipOldBmp      = NULL;
static int       g_tooltipLastIdx     = -1;

static void ApplyCaptureAffinity(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    DWORD affinity = (CAPTURE_BEHAVIOR.load() == 0) ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE;
    SetWindowDisplayAffinity(hwnd, affinity);
}

static void ApplyCaptureBehaviorAll() {
    ApplyCaptureAffinity(g_inputWnd);
    ApplyCaptureAffinity(g_overlayWnd);
    ApplyCaptureAffinity(g_ghostWnd);
    ApplyCaptureAffinity(g_tooltipWnd);
    ApplyCaptureAffinity(g_wsPopupWnd);
}

static void AddRoundedRectPath(Gdiplus::GraphicsPath& path, Gdiplus::RectF r, float radius) {
    if (radius <= 0.5f) {
        path.AddRectangle(r);
        return;
    }
    float d = radius * 2.0f;
    if (d > r.Width)  d = r.Width;
    if (d > r.Height) d = r.Height;
    path.AddArc(r.X, r.Y, d, d, 180.0f, 90.0f);
    path.AddArc(r.X + r.Width - d, r.Y, d, d, 270.0f, 90.0f);
    path.AddArc(r.X + r.Width - d, r.Y + r.Height - d, d, d, 0.0f, 90.0f);
    path.AddArc(r.X, r.Y + r.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

static std::wstring FriendlyPinLabel(const PinnedApp& app) {
    if (app.type == PIN_WORKSPACE) {
        return app.displayName.empty() ? L"Workspace" : app.displayName;
    }
    std::wstring name = BaseNameFromPath(app.exePath);
    if (name.size() > 4 && _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0)
        name.resize(name.size() - 4);
    if (!name.empty())
        name[0] = (wchar_t)towupper(name[0]);
    return name;
}

static void HideTooltipWindow() {
    if (g_tooltipWnd && IsWindow(g_tooltipWnd) && IsWindowVisible(g_tooltipWnd))
        ShowWindow(g_tooltipWnd, SW_HIDE);
    g_tooltipLastIdx = -1;
}

static void DestroyTooltipResources() {
    if (g_tooltipDC && g_tooltipOldBmp) {
        SelectObject(g_tooltipDC, g_tooltipOldBmp);
        g_tooltipOldBmp = NULL;
    }
    if (g_tooltipDC)  { DeleteDC(g_tooltipDC);      g_tooltipDC  = NULL; }
    if (g_tooltipDIB) { DeleteObject(g_tooltipDIB); g_tooltipDIB = NULL; }
    g_tooltipLastIdx = -1;
}

// Single-shot fast 4x-downsampled 2-pass box blur of screen region behind a popup.
// Cached per open so per-frame animation cost is 0% and blur + text stay 100% synced in one ULW_ALPHA DIB.
static Gdiplus::Bitmap* CaptureFrostedBackdrop(HDC sdc, int sx, int sy, int w, int h) {
    if (!sdc || w <= 4 || h <= 4) return nullptr;
    int bw = std::max(8, w / 4);
    int bh = std::max(8, h / 4);

    HDC capDC = CreateCompatibleDC(sdc);
    if (!capDC) return nullptr;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = bw;
    bi.bmiHeader.biHeight      = -bh;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* rawBits = nullptr;
    HBITMAP capDib = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, &rawBits, NULL, 0);
    if (!capDib || !rawBits) {
        if (capDib) DeleteObject(capDib);
        DeleteDC(capDC);
        return nullptr;
    }
    HBITMAP oldBmp = (HBITMAP)SelectObject(capDC, capDib);
    SetStretchBltMode(capDC, HALFTONE);
    StretchBlt(capDC, 0, 0, bw, bh, sdc, sx, sy, w, h, SRCCOPY);
    GdiFlush();

    std::vector<BYTE> buf((size_t)bw * bh * 4);
    memcpy(buf.data(), rawBits, buf.size());
    std::vector<BYTE> tmp(buf.size());

    // 2-pass separable 5-tap box blur (radius 2 on 1/4-res == ~8px smooth frost)
    for (int pass = 0; pass < 2; ++pass) {
        for (int y = 0; y < bh; ++y) {
            for (int x = 0; x < bw; ++x) {
                int r = 0, g = 0, b = 0, cnt = 0;
                for (int dx = -2; dx <= 2; ++dx) {
                    int nx = std::max(0, std::min(bw - 1, x + dx));
                    const BYTE* p = buf.data() + ((size_t)y * bw + nx) * 4;
                    b += p[0]; g += p[1]; r += p[2]; cnt++;
                }
                BYTE* d = tmp.data() + ((size_t)y * bw + x) * 4;
                d[0] = (BYTE)((b + cnt / 2) / cnt); d[1] = (BYTE)((g + cnt / 2) / cnt); d[2] = (BYTE)((r + cnt / 2) / cnt); d[3] = 255;
            }
        }
        for (int y = 0; y < bh; ++y) {
            for (int x = 0; x < bw; ++x) {
                int r = 0, g = 0, b = 0, cnt = 0;
                for (int dy = -2; dy <= 2; ++dy) {
                    int ny = std::max(0, std::min(bh - 1, y + dy));
                    const BYTE* p = tmp.data() + ((size_t)ny * bw + x) * 4;
                    b += p[0]; g += p[1]; r += p[2]; cnt++;
                }
                BYTE* d = buf.data() + ((size_t)y * bw + x) * 4;
                d[0] = (BYTE)((b + cnt / 2) / cnt); d[1] = (BYTE)((g + cnt / 2) / cnt); d[2] = (BYTE)((r + cnt / 2) / cnt); d[3] = 255;
            }
        }
    }

    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bw, bh, PixelFormat32bppPARGB);
    Gdiplus::BitmapData bd;
    Gdiplus::Rect lr(0, 0, bw, bh);
    if (bmp->LockBits(&lr, Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB, &bd) == Gdiplus::Ok) {
        for (int y = 0; y < bh; ++y)
            memcpy((BYTE*)bd.Scan0 + (size_t)y * bd.Stride, buf.data() + (size_t)y * bw * 4, (size_t)bw * 4);
        bmp->UnlockBits(&bd);
    } else {
        delete bmp;
        bmp = nullptr;
    }

    SelectObject(capDC, oldBmp);
    DeleteObject(capDib);
    DeleteDC(capDC);
    return bmp;
}

static Gdiplus::Bitmap* g_wsPopupBlurBmp = nullptr;

static void UpdateTooltipWindow(int hoverIdx) {
    if (!SHOW_TOOLTIPS || hoverIdx < 0 || g_dragState != DRAG_IDLE ||
        g_wsPopupState.load() != WS_POPUP_CLOSED ||
        (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0) ||
        g_layoutUnsupported || !g_csInitialized || !g_overlayWnd || !IsWindowVisible(g_overlayWnd)) {
        HideTooltipWindow();
        return;
    }

    std::wstring label;
    RECT iconLocal = {};
    float shiftX = 0.f;
    float curX = 0.f;
    EnterCriticalSection(&g_cs);
    int n = (int)g_pinnedApps.size();
    if (hoverIdx >= 0 && hoverIdx < n && g_pinnedApps[hoverIdx].opacity > 0.5f) {
        label     = FriendlyPinLabel(g_pinnedApps[hoverIdx]);
        iconLocal = GetIconRectLocal(hoverIdx, n);
        shiftX    = g_pinnedApps[hoverIdx].hoverShiftX;
        curX      = g_pinnedApps[hoverIdx].currentX;
    }
    LeaveCriticalSection(&g_cs);

    if (label.empty() || iconLocal.right <= iconLocal.left) {
        HideTooltipWindow();
        return;
    }
    if (hoverIdx == g_tooltipLastIdx && g_tooltipWnd && IsWindowVisible(g_tooltipWnd))
        return;

    if (!g_tooltipWnd) {
        static bool s_ttCls = false;
        if (!s_ttCls) {
            WNDCLASSEXW wc = { sizeof(wc) };
            wc.lpfnWndProc   = DefWindowProcW;
            wc.hInstance     = GetModHInstance();
            wc.lpszClassName = L"QPDockTooltip";
            RegisterClassExW(&wc);
            s_ttCls = true;
        }
        g_tooltipWnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
            L"QPDockTooltip", L"", WS_POPUP,
            0, 0, 120, 32, NULL, NULL, GetModHInstance(), NULL);
        if (!g_tooltipWnd) return;
        ApplyCaptureAffinity(g_tooltipWnd);
    }

    UINT dpi = GetDpiForWindow(g_overlayWnd);
    if (dpi == 0) dpi = 96;
    float s = dpi / 96.0f;

    Gdiplus::FontFamily ff(L"Segoe UI");
    Gdiplus::Font font(&ff, 12.0f * s, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    Gdiplus::Graphics measureG(sdc);
    Gdiplus::RectF layout(0, 0, 320.0f * s, 40.0f * s);
    Gdiplus::RectF bounds;
    measureG.MeasureString(label.c_str(), -1, &font, layout, &fmt, &bounds);

    int padX = (int)lroundf(10.0f * s);
    int padY = (int)lroundf(5.0f * s);
    int w = std::max(44, std::min((int)(320.0f * s), (int)ceilf(bounds.Width) + padX * 2));
    int h = std::max(24, (int)ceilf(bounds.Height) + padY * 2);

    DestroyTooltipResources();
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g_tooltipDIB = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    g_tooltipDC  = g_tooltipDIB ? CreateCompatibleDC(sdc) : NULL;
    if (g_tooltipDC && g_tooltipDIB && bits) {
        g_tooltipOldBmp = (HBITMAP)SelectObject(g_tooltipDC, g_tooltipDIB);
        memset(bits, 0, (size_t)w * h * 4);

        int baseX = (int)(curX != 0.f ? curX : (float)iconLocal.left) + (int)lroundf(shiftX);
        int iconCenterX = (int)g_dockCurrentX + baseX + ICON_SIZE / 2;
        int tx = std::max(4, iconCenterX - w / 2);
        int ty = (int)g_dockCurrentY - h - (int)lroundf(6.0f * s);
        if (ty < 4) ty = (int)g_dockCurrentY + g_dockLocalH + (int)lroundf(6.0f * s);

        Gdiplus::Graphics g(g_tooltipDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        bool light = IsSystemLightTheme();
        Gdiplus::GraphicsPath path;
        AddRoundedRectPath(path, Gdiplus::RectF(1.0f, 1.0f, (float)w - 2.0f, (float)h - 2.0f), (float)(h - 2) * 0.45f);
        bool ttWasVis = (g_tooltipWnd && IsWindowVisible(g_tooltipWnd));
        if (ttWasVis && CAPTURE_BEHAVIOR.load() != 0) ShowWindow(g_tooltipWnd, SW_HIDE);
        Gdiplus::Bitmap* ttBlur = CaptureFrostedBackdrop(sdc, tx, ty, w, h);
        if (ttBlur) {
            g.SetClip(&path);
            g.DrawImage(ttBlur, Gdiplus::RectF(1.0f, 1.0f, (float)w - 2.0f, (float)h - 2.0f),
                        0.0f, 0.0f, (float)ttBlur->GetWidth(), (float)ttBlur->GetHeight(), Gdiplus::UnitPixel);
            g.ResetClip();
            delete ttBlur;
        }
        Gdiplus::SolidBrush bg(light ? Gdiplus::Color(175, 246, 248, 252) : Gdiplus::Color(185, 24, 28, 36));
        g.FillPath(&bg, &path);
        Gdiplus::Pen border(light ? Gdiplus::Color(65, 255, 255, 255) : Gdiplus::Color(68, 255, 255, 255), 1.0f);
        g.DrawPath(&border, &path);

        Gdiplus::SolidBrush textBr(light ? Gdiplus::Color(248, 18, 20, 24) : Gdiplus::Color(248, 244, 246, 250));
        Gdiplus::RectF textRc(0.0f, 0.0f, (float)w, (float)h);
        g.DrawString(label.c_str(), -1, &font, textRc, &fmt, &textBr);

        POINT dest = { tx, ty };
        SIZE  sz   = { w, h };
        POINT src  = { 0, 0 };
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        SetWindowPos(g_tooltipWnd, HWND_TOPMOST, dest.x, dest.y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
        UpdateLayeredWindow(g_tooltipWnd, sdc, &dest, &sz, g_tooltipDC, &src, 0, &bf, ULW_ALPHA);
        g_tooltipLastIdx = hoverIdx;
    }
    ReleaseDC(NULL, sdc);
}


// ============================================================
//  GROUPED WORKSPACE DROP-UP POPUP (QPDockWorkspacePopup)
//  - ULW_ALPHA layered window, zero-flash, zero ugly strokes/borders
//  - Opens upward from the taskbar above the workspace icon
//  - State machine: CLOSED -> OPEN_HOVER (200ms linger) -> OPEN_HOLD (on click)
//  - Outside click / Esc / Fullscreen dismisses
// ============================================================
static HBITMAP       g_wsPopupDIB         = NULL;
static HDC           g_wsPopupDC          = NULL;
static HBITMAP       g_wsPopupOldBmp      = NULL;
static int           g_wsPopupCachedW     = 0;
static int           g_wsPopupCachedH     = 0;
static int               g_wsPopupAllocW      = 0;
static int               g_wsPopupAllocH      = 0;
static int               g_wsPopupBlurOrigH   = 0;

static RECT              g_wsPopupRect    = {};
static std::atomic<LONGLONG> g_wsPopupRectPacked1{0}; // left (low 32) | top (high 32)
static std::atomic<LONGLONG> g_wsPopupRectPacked2{0}; // right (low 32) | bottom (high 32)

static void SetWsPopupRectSafe(int l, int t, int r, int b) {
    g_wsPopupRect = { l, t, r, b };
    LONGLONG p1 = ((LONGLONG)(LONG)t << 32) | (ULONG)(LONG)l;
    LONGLONG p2 = ((LONGLONG)(LONG)b << 32) | (ULONG)(LONG)r;
    g_wsPopupRectPacked1.store(p1);
    g_wsPopupRectPacked2.store(p2);
}

static bool PtInWsPopupRectSafe(POINT pt) {
    LONGLONG p1 = g_wsPopupRectPacked1.load();
    LONGLONG p2 = g_wsPopupRectPacked2.load();
    RECT rc = {
        (LONG)(ULONG)(p1 & 0xFFFFFFFFULL),
        (LONG)(p1 >> 32),
        (LONG)(ULONG)(p2 & 0xFFFFFFFFULL),
        (LONG)(p2 >> 32)
    };
    return rc.right > rc.left && rc.bottom > rc.top && PtInRect(&rc, pt) != 0;
}
static int               g_wsPopupHoverRow    = -1;
static int               g_wsPopupHoverDelRow = -1;
static int               g_wsPopupPressRow    = -1;
static POINT             g_wsPopupPressPt     = {};
static int               g_wsPopupDragRow     = -1;
static int               g_wsPopupDragDropRow = -1;
static bool              g_wsPopupDraggingRow = false;
static float             g_wsPopupDragAnchorY = 0.0f;
static float             g_wsPopupDragCurY    = 0.0f;
static std::vector<float> g_wsPopupRowAnimY;
static LARGE_INTEGER     g_wsPopupRowAnimLastQpc = {};
static bool              g_wsPopupMouseTrack  = false;
static int               g_wsPopupRowTopY     = 0;
static int               g_wsPopupRowH        = 32;
static int               g_wsPopupPadX        = 10;
static int               g_wsPopupRowCount    = 0;
static int               g_wsPopupScrollOffset = 0;

static LARGE_INTEGER     g_wsPopupAnimStart   = {};
static LARGE_INTEGER     g_wsPopupQpcFreq     = {};
static float             g_wsPopupAnimProg    = 0.0f;
static bool              g_wsPopupAnimating   = false;
static DWORD             g_wsPopupLingerUntil = 0;
static int               g_wsPopupHoverPin    = -1;
static DWORD             g_wsPopupHoverSince  = 0;

static const UINT_PTR    WS_POPUP_TIMER_ID    = 0x51D0;
static const UINT_PTR    WS_POPUP_CARET_TIMER_ID = 0x51D1;
static const float       WS_POPUP_OPEN_MS     = 200.0f;
static bool              g_wsPopupRenaming    = false;
static std::wstring      g_wsPopupRenameText;
static size_t            g_wsPopupRenameCaret = 0;
static bool              g_wsPopupRenameSelAll = false;
static bool              g_wsPopupCaretVisible = true;
static bool              g_wsPopupHeaderHover  = false;
static RECT              g_wsPopupHeaderRc     = {};
static const float       WS_POPUP_CLOSE_MS    = 140.0f;
static int               g_wsPopupBaseX       = 0;
static int               g_wsPopupTargetY     = 0;
static float             g_wsPopupScaleS      = 1.0f;
static bool              g_wsPopupFlipDown    = false;

static void DestroyWsPopupResources() {
    if (g_wsPopupDC && g_wsPopupOldBmp) {
        SelectObject(g_wsPopupDC, g_wsPopupOldBmp);
        g_wsPopupOldBmp = NULL;
    }
    if (g_wsPopupDC)  { DeleteDC(g_wsPopupDC);      g_wsPopupDC  = NULL; }
    if (g_wsPopupDIB) { DeleteObject(g_wsPopupDIB); g_wsPopupDIB = NULL; }
    if (g_wsPopupBlurBmp) { delete g_wsPopupBlurBmp; g_wsPopupBlurBmp = nullptr; }
    g_wsPopupCachedW   = 0;
    g_wsPopupCachedH   = 0;
    g_wsPopupAllocW    = 0;
    g_wsPopupAllocH    = 0;
    g_wsPopupBlurOrigH = 0;
}

static bool EnsureWsPopupDIB(HDC refDC, int w, int h, void** outBits) {
    if (!refDC || w <= 0 || h <= 0) return false;
    if (g_wsPopupDC && g_wsPopupDIB && g_wsPopupAllocW >= w && g_wsPopupAllocH >= h) {
        BITMAP bm = {};
        GetObjectW(g_wsPopupDIB, sizeof(bm), &bm);
        g_wsPopupCachedW = w;
        g_wsPopupCachedH = h;
        if (outBits) *outBits = bm.bmBits;
        return bm.bmBits != nullptr;
    }
    DestroyWsPopupResources();
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g_wsPopupDIB = CreateDIBSection(refDC, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    g_wsPopupDC  = g_wsPopupDIB ? CreateCompatibleDC(refDC) : NULL;
    if (!g_wsPopupDC || !g_wsPopupDIB || !bits) {
        DestroyWsPopupResources();
        return false;
    }
    g_wsPopupOldBmp  = (HBITMAP)SelectObject(g_wsPopupDC, g_wsPopupDIB);
    g_wsPopupCachedW = w;
    g_wsPopupCachedH = h;
    g_wsPopupAllocW  = w;
    g_wsPopupAllocH  = h;
    if (outBits) *outBits = bits;
    return true;
}

static void SplitPathForRow(const std::wstring& fullPath,
                            std::wstring& outDrive,
                            std::wstring& outParent,
                            std::wstring& outLeaf) {
    outDrive.clear();
    outParent.clear();
    outLeaf.clear();
    if (fullPath.size() >= 2 && fullPath[1] == L':') {
        outDrive = fullPath.substr(0, 2);
    } else if (fullPath.size() >= 2 && fullPath[0] == L'\\' && fullPath[1] == L'\\') {
        outDrive = L"NET";
    } else {
        outDrive = L"DIR";
    }

    std::wstring clean = fullPath;
    while (clean.size() > 3 && (clean.back() == L'\\' || clean.back() == L'/'))
        clean.pop_back();

    size_t pos = clean.find_last_of(L"\\/");
    if (pos == std::wstring::npos) {
        outLeaf = clean;
    } else {
        outLeaf = clean.substr(pos + 1);
        if (outLeaf.empty()) outLeaf = clean;
        if (pos > 2) {
            size_t start = (clean.size() >= 3 && clean[1] == L':') ? 3 : 0;
            if (pos > start) {
                std::wstring mid = clean.substr(start, pos - start);
                if (mid.size() > 18)
                    outParent = L"...\\" + mid.substr(mid.size() - 15) + L"\\";
                else
                    outParent = mid + L"\\";
            }
        }
    }
}

static void RenderWorkspacePopupULW() {
    int pinIdx = g_wsPopupPinIdx.load();
    int state  = g_wsPopupState.load();
    if (!g_wsPopupWnd || !IsWindow(g_wsPopupWnd) || state == WS_POPUP_CLOSED || pinIdx < 0)
        return;

    std::wstring title;
    std::vector<std::wstring> paths;
    RECT iconLocal = {};
    float shiftX = 0.f;
    float curX = 0.f;

    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        int n = (int)g_pinnedApps.size();
        if (pinIdx >= 0 && pinIdx < n && g_pinnedApps[pinIdx].type == PIN_WORKSPACE) {
            title     = g_pinnedApps[pinIdx].displayName.empty() ? L"Workspace" : g_pinnedApps[pinIdx].displayName;
            paths     = g_pinnedApps[pinIdx].workspacePaths;
            iconLocal = GetIconRectLocal(pinIdx, n);
            shiftX    = g_pinnedApps[pinIdx].hoverShiftX;
            curX      = g_pinnedApps[pinIdx].currentX;
        }
        LeaveCriticalSection(&g_cs);
    }

    if (paths.empty()) {
        g_wsPopupState = WS_POPUP_CLOSED;
        g_wsPopupPinIdx = -1;
        g_wsPopupAnimating = false;
        KillTimer(g_wsPopupWnd, WS_POPUP_TIMER_ID);
        ShowWindow(g_wsPopupWnd, SW_HIDE);
        SetWsPopupRectSafe(0, 0, 0, 0);
        return;
    }

    UINT dpi = g_overlayWnd ? GetDpiForWindow(g_overlayWnd) : 96;
    if (dpi == 0) dpi = 96;
    float s = dpi / 96.0f;

    int padX   = (int)lroundf(10.0f * s);
    int padY   = (int)lroundf(10.0f * s);
    int hdrH   = (int)lroundf(26.0f * s);
    int rowH   = (int)lroundf(32.0f * s);
    int w      = (int)lroundf(316.0f * s);
    int visCap = std::max(3, std::min(20, POPUP_VISIBLE_PATHS.load()));
    int maxVis = std::min((int)paths.size(), visCap);
    int h      = padY * 2 + hdrH + maxVis * rowH;

    g_wsPopupPadX     = padX;
    g_wsPopupRowTopY  = padY + hdrH;
    g_wsPopupRowH     = rowH;
    g_wsPopupRowCount = (int)paths.size();
    int maxScroll = std::max(0, g_wsPopupRowCount - maxVis);
    if (g_wsPopupScrollOffset > maxScroll) g_wsPopupScrollOffset = maxScroll;
    if (g_wsPopupScrollOffset < 0)         g_wsPopupScrollOffset = 0;

    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    void* bits = nullptr;
    if (!EnsureWsPopupDIB(sdc, w, h, &bits)) {
        ReleaseDC(NULL, sdc);
        return;
    }
    memset(bits, 0, (size_t)g_wsPopupAllocW * h * 4);

    float t = g_wsPopupAnimProg;
    float ease = t * t * (3.0f - 2.0f * t);
    BYTE masterAlpha = (BYTE)std::max(0, std::min(255, (int)lroundf(255.0f * ease)));
    int riseOffset = (int)lroundf((1.0f - ease) * 6.0f * s);

    int baseXPos = (int)(curX != 0.f ? curX : (float)iconLocal.left) + (int)lroundf(shiftX);
    int iconCenterXPos = (int)g_dockCurrentX + baseXPos + ICON_SIZE / 2;
    int px = iconCenterXPos - w / 2;
    int targetPy = (int)g_dockCurrentY - h - (int)lroundf(8.0f * s);
    bool flipDown = false;
    HMONITOR mon = MonitorFromWindow(g_overlayWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfoW(mon, &mi)) {
        px = std::max((int)mi.rcWork.left + 6, std::min(px, (int)mi.rcWork.right - w - 6));
        if (targetPy < (int)mi.rcWork.top + 6) {
            targetPy = (int)g_dockCurrentY + g_dockLocalH + (int)lroundf(8.0f * s);
            flipDown = true;
        }
    }
    int py = flipDown ? (targetPy - riseOffset) : (targetPy + riseOffset);
    g_wsPopupBaseX    = px;
    g_wsPopupTargetY  = targetPy;
    g_wsPopupScaleS   = s;
    g_wsPopupFlipDown = flipDown;
    if (!g_wsPopupBlurBmp) {
        bool popWasVis = (g_wsPopupWnd && IsWindowVisible(g_wsPopupWnd));
        if (popWasVis && CAPTURE_BEHAVIOR.load() != 0) ShowWindow(g_wsPopupWnd, SW_HIDE);
        int finalPy = (int)g_dockCurrentY - h - (int)lroundf(8.0f * s);
        if (GetMonitorInfoW(mon, &mi) && finalPy < (int)mi.rcWork.top + 6)
            finalPy = (int)g_dockCurrentY + g_dockLocalH + (int)lroundf(8.0f * s);
        g_wsPopupBlurBmp   = CaptureFrostedBackdrop(sdc, px, finalPy, w, h);
        g_wsPopupBlurOrigH = h;
    }

    Gdiplus::Graphics g(g_wsPopupDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

    bool light = IsSystemLightTheme();

    // True frosted glass surface (single-shot blurred backdrop + translucent frost + 1px specular rim)
    Gdiplus::GraphicsPath cardPath;
    AddRoundedRectPath(cardPath, Gdiplus::RectF(0.5f, 0.5f, (float)w - 1.0f, (float)h - 1.0f), 12.0f * s);
    if (g_wsPopupBlurBmp) {
        float bmpW = (float)g_wsPopupBlurBmp->GetWidth();
        float bmpH = (float)g_wsPopupBlurBmp->GetHeight();
        float srcY = 0.0f;
        float srcH = bmpH;
        if (g_wsPopupBlurOrigH > 0 && h < g_wsPopupBlurOrigH) {
            float ratio = (float)h / (float)g_wsPopupBlurOrigH;
            srcH = std::max(1.0f, bmpH * ratio);
            srcY = flipDown ? 0.0f : std::max(0.0f, bmpH - srcH);
        }
        g.SetClip(&cardPath);
        g.DrawImage(g_wsPopupBlurBmp, Gdiplus::RectF(0.0f, 0.0f, (float)w, (float)h),
                    0.0f, srcY, bmpW, srcH,
                    Gdiplus::UnitPixel);
        g.ResetClip();
    }
    // Uniform translucent frost fill (eliminates 8-bit vertical gradient banding in Light & Dark modes)
    Gdiplus::SolidBrush cardBg(light ? Gdiplus::Color(218, 247, 248, 252)
                                     : Gdiplus::Color(224, 22, 24, 30));
    g.FillPath(&cardBg, &cardPath);
    Gdiplus::Pen cardRim(light ? Gdiplus::Color(30, 0, 0, 0)
                               : Gdiplus::Color(34, 255, 255, 255), 1.0f);
    g.DrawPath(&cardRim, &cardPath);

    // Header: Workspace title + count pill
    Gdiplus::FontFamily ff(L"Segoe UI");
    Gdiplus::Font hdrFont(&ff, 11.5f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font subFont(&ff, 10.5f * s, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font rowFont(&ff, 11.5f * s, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font leafFont(&ff, 11.5f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font driveFont(&ff, 10.0f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

    Gdiplus::StringFormat fmtLeft;
    fmtLeft.SetAlignment(Gdiplus::StringAlignmentNear);
    fmtLeft.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    fmtLeft.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    fmtLeft.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

    Gdiplus::StringFormat fmtCenter;
    fmtCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmtCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    // Crisp theme-adaptive typography: dark ink (#14161A) in Light Mode, off-white (#F4F6FA) in Dark Mode
    Gdiplus::SolidBrush titleBr(light ? Gdiplus::Color(250, 20, 22, 26) : Gdiplus::Color(250, 244, 246, 250));
    Gdiplus::SolidBrush dimBr(light ? Gdiplus::Color(175, 58, 62, 70)   : Gdiplus::Color(180, 186, 194, 206));
    Gdiplus::SolidBrush leafBr(light ? Gdiplus::Color(252, 16, 18, 22)  : Gdiplus::Color(252, 246, 248, 252));

    Gdiplus::RectF hdrRc((float)padX, (float)padY + 2.0f * s, (float)(w - padX * 2 - 88 * s), (float)hdrH - 4.0f * s);
    g_wsPopupHeaderRc = {
        (LONG)lroundf(hdrRc.X), (LONG)lroundf(hdrRc.Y),
        (LONG)lroundf(hdrRc.GetRight()), (LONG)lroundf(hdrRc.GetBottom())
    };
    if (g_wsPopupRenaming) {
        Gdiplus::GraphicsPath editPath;
        AddRoundedRectPath(editPath, hdrRc, 6.0f * s);
        Gdiplus::SolidBrush editBg(light ? Gdiplus::Color(26, 0, 0, 0) : Gdiplus::Color(32, 255, 255, 255));
        g.FillPath(&editBg, &editPath);
        Gdiplus::Pen editRim(Gdiplus::Color(210, 64, 156, 255), 1.2f);
        g.DrawPath(&editRim, &editPath);

        Gdiplus::RectF textRc(hdrRc.X + 6.0f * s, hdrRc.Y, std::max(10.0f, hdrRc.Width - 12.0f * s), hdrRc.Height);
        Gdiplus::StringFormat fmtEdit(Gdiplus::StringFormat::GenericTypographic());
        fmtEdit.SetAlignment(Gdiplus::StringAlignmentNear);
        fmtEdit.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        fmtEdit.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap | Gdiplus::StringFormatFlagsMeasureTrailingSpaces);

        if (g_wsPopupRenameSelAll && !g_wsPopupRenameText.empty()) {
            Gdiplus::RectF selBounds;
            g.MeasureString(g_wsPopupRenameText.c_str(), -1, &hdrFont, textRc, &fmtEdit, &selBounds);
            float selW = std::min(textRc.Width, selBounds.Width + 2.0f * s);
            Gdiplus::SolidBrush selBr(Gdiplus::Color(95, 64, 156, 255));
            g.FillRectangle(&selBr, textRc.X, textRc.Y + 3.0f * s, selW, std::max(4.0f, textRc.Height - 6.0f * s));
        }

        if (g_wsPopupRenameText.empty()) {
            g.DrawString(L"Group name...", -1, &subFont, textRc, &fmtEdit, &dimBr);
        } else {
            g.SetClip(textRc);
            g.DrawString(g_wsPopupRenameText.c_str(), -1, &hdrFont, textRc, &fmtEdit, &titleBr);
            g.ResetClip();
        }

        if (g_wsPopupCaretVisible) {
            float caretOffsetX = 0.0f;
            if (g_wsPopupRenameCaret > 0 && !g_wsPopupRenameText.empty()) {
                size_t cLen = std::min(g_wsPopupRenameCaret, g_wsPopupRenameText.size());
                Gdiplus::RectF cb;
                g.MeasureString(g_wsPopupRenameText.c_str(), (INT)cLen, &hdrFont,
                                Gdiplus::RectF(0, 0, 2000.0f, textRc.Height), &fmtEdit, &cb);
                caretOffsetX = std::min(textRc.Width - 2.0f * s, cb.Width);
            }
            float cx = textRc.X + caretOffsetX;
            Gdiplus::Pen caretPen(light ? Gdiplus::Color(245, 20, 22, 26) : Gdiplus::Color(245, 244, 246, 250), 1.5f);
            g.DrawLine(&caretPen, cx, textRc.Y + 4.0f * s, cx, textRc.GetBottom() - 4.0f * s);
        }
    } else {
        if (g_wsPopupHeaderHover) {
            Gdiplus::GraphicsPath hp;
            AddRoundedRectPath(hp, hdrRc, 6.0f * s);
            Gdiplus::SolidBrush hBg(light ? Gdiplus::Color(16, 0, 0, 0) : Gdiplus::Color(20, 255, 255, 255));
            g.FillPath(&hBg, &hp);
        }
        Gdiplus::RectF titleRc(hdrRc.X + 4.0f * s, hdrRc.Y, std::max(10.0f, hdrRc.Width - 6.0f * s), hdrRc.Height);
        g.DrawString(title.c_str(), -1, &hdrFont, titleRc, &fmtLeft, &titleBr);
    }

    std::wstring countTxt = (g_wsPopupRowCount > maxVis)
        ? (std::to_wstring(g_wsPopupScrollOffset + 1) + L"–" +
           std::to_wstring(std::min(g_wsPopupRowCount, g_wsPopupScrollOffset + maxVis)) +
           L" of " + std::to_wstring(g_wsPopupRowCount))
        : (std::to_wstring(g_wsPopupRowCount) + (paths.size() == 1 ? L" folder" : L" folders"));
    Gdiplus::StringFormat fmtRight;
    fmtRight.SetAlignment(Gdiplus::StringAlignmentFar);
    fmtRight.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::RectF cntRc((float)(w - padX - 88 * s), (float)padY, 86.0f * s, (float)hdrH);
    g.DrawString(countTxt.c_str(), -1, &subFont, cntRc, &fmtRight, &dimBr);

    // Rows (scrolled slice, zero scrollbar)
    int drawCount = std::min((int)paths.size() - g_wsPopupScrollOffset, maxVis);
    if ((int)g_wsPopupRowAnimY.size() != drawCount) {
        g_wsPopupRowAnimY.resize(drawCount);
        for (int v = 0; v < drawCount; ++v)
            g_wsPopupRowAnimY[v] = (float)(padY + hdrH + v * rowH);
    }

    auto drawRowAt = [&](int i, float ry, bool isHeld) {
        float liftX = isHeld ? 2.0f * s : 0.0f;
        float liftY = isHeld ? 1.0f * s : 0.0f;
        Gdiplus::RectF rowRc((float)padX - liftX, ry + 1.0f * s - liftY,
                             (float)(w - padX * 2) + liftX * 2.0f,
                             (float)rowH - 2.0f * s + liftY * 2.0f);

        if (isHeld) {
            // Z-axis elevation: layered soft drop shadow + lifted frosted card + top specular highlight
            for (int sh = 3; sh >= 1; --sh) {
                float offY = (float)sh * 1.6f * s;
                float padS = (float)(4 - sh) * 0.5f * s;
                Gdiplus::RectF shRc(rowRc.X - padS, rowRc.Y + offY, rowRc.Width + padS * 2.0f, rowRc.Height);
                Gdiplus::GraphicsPath shPath;
                AddRoundedRectPath(shPath, shRc, 8.0f * s);
                BYTE shAlpha = (BYTE)(sh == 1 ? 46 : (sh == 2 ? 26 : 12));
                Gdiplus::SolidBrush shBr(Gdiplus::Color(shAlpha, 0, 0, 0));
                g.FillPath(&shBr, &shPath);
            }
            Gdiplus::GraphicsPath heldPath;
            AddRoundedRectPath(heldPath, rowRc, 8.0f * s);
            Gdiplus::SolidBrush heldFill(light
                ? Gdiplus::Color(246, 252, 253, 255)
                : Gdiplus::Color(242, 42, 47, 58));
            g.FillPath(&heldFill, &heldPath);
            Gdiplus::Pen heldRim(light
                ? Gdiplus::Color(155, 64, 156, 255)
                : Gdiplus::Color(175, 96, 178, 255), 1.2f);
            g.DrawPath(&heldRim, &heldPath);
            Gdiplus::Pen topHi(Gdiplus::Color(70, 255, 255, 255), 1.0f);
            g.DrawLine(&topHi, rowRc.X + 8.0f * s, rowRc.Y + 0.8f,
                       rowRc.GetRight() - 8.0f * s, rowRc.Y + 0.8f);
        } else if (i == g_wsPopupHoverRow && !g_wsPopupDraggingRow) {
            Gdiplus::GraphicsPath rPath;
            AddRoundedRectPath(rPath, rowRc, 7.0f * s);
            Gdiplus::SolidBrush rowFill(light ? Gdiplus::Color(24, 0, 0, 0) : Gdiplus::Color(26, 255, 255, 255));
            g.FillPath(&rowFill, &rPath);
        }

        std::wstring drive, parent, leaf;
        SplitPathForRow(paths[i], drive, parent, leaf);

        // Drive letter fill badge (C: highlighted in red to indicate system/critical drive)
        float dbW = 28.0f * s;
        float dbH = 18.0f * s;
        float dbX = rowRc.X + 6.0f * s;
        float dbY = rowRc.Y + (rowRc.Height - dbH) * 0.5f;
        bool isCDrive = (drive.size() >= 2 && towupper(drive[0]) == L'C' && drive[1] == L':');
        Gdiplus::GraphicsPath dPath;
        AddRoundedRectPath(dPath, Gdiplus::RectF(dbX, dbY, dbW, dbH), 5.0f * s);
        Gdiplus::SolidBrush dFill(isCDrive
            ? (light ? Gdiplus::Color(42, 214, 48, 48) : Gdiplus::Color(58, 224, 62, 62))
            : (light ? Gdiplus::Color(30, 0, 0, 0)    : Gdiplus::Color(36, 255, 255, 255)));
        g.FillPath(&dFill, &dPath);
        Gdiplus::SolidBrush dTxt(isCDrive
            ? (light ? Gdiplus::Color(248, 185, 24, 24) : Gdiplus::Color(250, 255, 115, 115))
            : (light ? Gdiplus::Color(230, 30, 35, 45)  : Gdiplus::Color(235, 210, 224, 255)));
        g.DrawString(drive.c_str(), -1, &driveFont, Gdiplus::RectF(dbX, dbY, dbW, dbH), &fmtCenter, &dTxt);

        // Delete 'x' circle on the right
        float delSz = 18.0f * s;
        float delX  = rowRc.GetRight() - delSz - 6.0f * s;
        float delY  = rowRc.Y + (rowRc.Height - delSz) * 0.5f;
        bool delHot = (!isHeld && i == g_wsPopupHoverDelRow);
        Gdiplus::SolidBrush delFill(delHot
            ? Gdiplus::Color(210, 220, 62, 62)
            : (light ? Gdiplus::Color(22, 0, 0, 0) : Gdiplus::Color(28, 255, 255, 255)));
        g.FillEllipse(&delFill, delX, delY, delSz, delSz);
        Gdiplus::SolidBrush xBr(delHot ? Gdiplus::Color(255, 255, 255, 255)
                                       : (light ? Gdiplus::Color(180, 60, 65, 75) : Gdiplus::Color(190, 200, 206, 218)));
        g.DrawString(L"\u00D7", -1, &rowFont, Gdiplus::RectF(delX, delY - 0.5f * s, delSz, delSz), &fmtCenter, &xBr);

        // Path text: dimmed parent + bold leaf
        float textX = dbX + dbW + 8.0f * s;
        float textMaxW = delX - textX - 6.0f * s;
        if (textMaxW > 16.0f) {
            float parentW = 0.0f;
            if (!parent.empty()) {
                Gdiplus::RectF pb;
                g.MeasureString(parent.c_str(), -1, &rowFont, Gdiplus::RectF(0, 0, textMaxW * 0.55f, rowRc.Height), &fmtLeft, &pb);
                parentW = std::min(textMaxW * 0.55f, pb.Width + 2.0f * s);
                Gdiplus::RectF pr(textX, rowRc.Y, parentW, rowRc.Height);
                g.DrawString(parent.c_str(), -1, &rowFont, pr, &fmtLeft, &dimBr);
            }
            Gdiplus::RectF lr(textX + parentW, rowRc.Y, std::max(10.0f, textMaxW - parentW), rowRc.Height);
            g.DrawString(leaf.c_str(), -1, &leafFont, lr, &fmtLeft, &leafBr);
        }
    };

    // Draw non-dragged rows first (with smooth QPC-animated slot reflow)
    int heldIdx = -1;
    for (int v = 0; v < drawCount; ++v) {
        int i = g_wsPopupScrollOffset + v;
        if (g_wsPopupDraggingRow && i == g_wsPopupDragRow) {
            heldIdx = i;
            continue;
        }
        int targetVis = v;
        if (g_wsPopupDraggingRow && g_wsPopupDragRow >= 0 && g_wsPopupDragDropRow >= 0) {
            if (g_wsPopupDragRow < g_wsPopupDragDropRow &&
                i > g_wsPopupDragRow && i <= g_wsPopupDragDropRow)
                targetVis = v - 1;
            else if (g_wsPopupDragRow > g_wsPopupDragDropRow &&
                     i >= g_wsPopupDragDropRow && i < g_wsPopupDragRow)
                targetVis = v + 1;
        }
        float targetY = (float)(padY + hdrH + targetVis * rowH);
        if (!g_wsPopupDraggingRow)
            g_wsPopupRowAnimY[v] = targetY;
        drawRowAt(i, g_wsPopupRowAnimY[v], false);
    }

    // Draw held row LAST on top of all other rows (Z-axis separation)
    if (heldIdx >= 0 && heldIdx < (int)paths.size()) {
        float minY = (float)(padY + hdrH);
        float maxY = (float)(padY + hdrH + std::max(0, drawCount - 1) * rowH);
        float drawY = std::max(minY, std::min(maxY, g_wsPopupDragCurY));
        drawRowAt(heldIdx, drawY, true);
    }

    SetWsPopupRectSafe(px, py, px + w, py + h);

    POINT dest = { px, py };
    SIZE  sz   = { w, h };
    POINT src  = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA };
    UpdateLayeredWindow(g_wsPopupWnd, sdc, &dest, &sz, g_wsPopupDC, &src, 0, &bf, ULW_ALPHA);
    if (!IsWindowVisible(g_wsPopupWnd))
        SetWindowPos(g_wsPopupWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    ReleaseDC(NULL, sdc);
}

static int WsPopupHitTestRow(POINT clientPt, bool* outOnDelete) {
    if (outOnDelete) *outOnDelete = false;
    if (g_wsPopupRowCount <= 0) return -1;
    int visCap = std::max(3, std::min(20, POPUP_VISIBLE_PATHS.load()));
    int maxVis = std::min(g_wsPopupRowCount, visCap);
    int relY = clientPt.y - g_wsPopupRowTopY;
    if (relY < 0) return -1;
    int visRow = relY / std::max(1, g_wsPopupRowH);
    if (visRow < 0 || visRow >= maxVis) return -1;
    int row = g_wsPopupScrollOffset + visRow;
    if (row < 0 || row >= g_wsPopupRowCount) return -1;
    if (outOnDelete) {
        int delRight = (g_wsPopupRect.right - g_wsPopupRect.left) - g_wsPopupPadX - 4;
        int delLeft  = delRight - 24;
        if (clientPt.x >= delLeft && clientPt.x <= delRight)
            *outOnDelete = true;
    }
    return row;
}

static void CloseWorkspacePopupOnUi(bool immediate);

static void BeginWsPopupRename(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !g_csInitialized) return;
    int pinIdx = g_wsPopupPinIdx.load();
    std::wstring curTitle;
    EnterCriticalSection(&g_cs);
    if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
        g_pinnedApps[pinIdx].type == PIN_WORKSPACE) {
        curTitle = g_pinnedApps[pinIdx].displayName;
    }
    LeaveCriticalSection(&g_cs);

    g_wsPopupState        = WS_POPUP_OPEN_HOLD;
    g_wsPopupRenaming     = true;
    g_wsPopupRenameText   = curTitle;
    g_wsPopupRenameCaret  = g_wsPopupRenameText.size();
    g_wsPopupRenameSelAll = true;
    g_wsPopupCaretVisible = true;
    g_wsPopupHeaderHover  = false;

    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_NOACTIVATE)
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_NOACTIVATE);
    ForceForegroundWindow(hwnd);
    SetFocus(hwnd);
    SetTimer(hwnd, WS_POPUP_CARET_TIMER_ID, 500, NULL);
    RenderWorkspacePopupULW();
}

static void CommitWsPopupRename(HWND hwnd, bool save) {
    if (!g_wsPopupRenaming) return;
    g_wsPopupRenaming     = false;
    g_wsPopupRenameSelAll = false;
    if (hwnd && IsWindow(hwnd)) {
        KillTimer(hwnd, WS_POPUP_CARET_TIMER_ID);
        LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (!(ex & WS_EX_NOACTIVATE))
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_NOACTIVATE);
    }
    if (save) {
        RenameWorkspace(g_wsPopupPinIdx.load(), g_wsPopupRenameText);
    }
    RenderWorkspacePopupULW();
}

static LRESULT CALLBACK WsPopupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_NCHITTEST:
        return HTCLIENT;

    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (wParam == WS_POPUP_TIMER_ID) {
            if (g_wsPopupQpcFreq.QuadPart == 0)
                QueryPerformanceFrequency(&g_wsPopupQpcFreq);
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            if (g_wsPopupDraggingRow && !g_wsPopupAnimating) {
                float dtMs = 16.0f;
                if (g_wsPopupRowAnimLastQpc.QuadPart > 0) {
                    dtMs = (float)((double)(now.QuadPart - g_wsPopupRowAnimLastQpc.QuadPart) * 1000.0 /
                                   (double)std::max<LONGLONG>(1, g_wsPopupQpcFreq.QuadPart));
                }
                g_wsPopupRowAnimLastQpc = now;
                dtMs = std::max(1.0f, std::min(33.0f, dtMs));
                float alpha = 1.0f - powf(0.0015f, dtMs / 160.0f);
                int visCap = std::max(3, std::min(20, POPUP_VISIBLE_PATHS.load()));
                int drawCount = std::min(std::max(0, g_wsPopupRowCount - g_wsPopupScrollOffset), visCap);
                bool anyMoved = false;
                for (int v = 0; v < drawCount && v < (int)g_wsPopupRowAnimY.size(); ++v) {
                    int i = g_wsPopupScrollOffset + v;
                    if (i == g_wsPopupDragRow) continue;
                    int targetVis = v;
                    if (g_wsPopupDragRow < g_wsPopupDragDropRow &&
                        i > g_wsPopupDragRow && i <= g_wsPopupDragDropRow)
                        targetVis = v - 1;
                    else if (g_wsPopupDragRow > g_wsPopupDragDropRow &&
                             i >= g_wsPopupDragDropRow && i < g_wsPopupDragRow)
                        targetVis = v + 1;
                    float targetY = (float)(g_wsPopupRowTopY + targetVis * g_wsPopupRowH);
                    float diff = targetY - g_wsPopupRowAnimY[v];
                    if (fabsf(diff) > 0.35f) {
                        g_wsPopupRowAnimY[v] += diff * alpha;
                        anyMoved = true;
                    } else if (g_wsPopupRowAnimY[v] != targetY) {
                        g_wsPopupRowAnimY[v] = targetY;
                        anyMoved = true;
                    }
                }
                if (anyMoved) RenderWorkspacePopupULW();
                return 0;
            }
            float elapsedMs = (float)((double)(now.QuadPart - g_wsPopupAnimStart.QuadPart) * 1000.0 /
                                      (double)std::max<LONGLONG>(1, g_wsPopupQpcFreq.QuadPart));
            int st = g_wsPopupState.load();
            if (st == WS_POPUP_CLOSING) {
                g_wsPopupAnimProg = std::max(0.0f, 1.0f - elapsedMs / WS_POPUP_CLOSE_MS);
                if (g_wsPopupAnimProg <= 0.001f) {
                    g_wsPopupAnimating = false;
                    g_wsPopupState = WS_POPUP_CLOSED;
                    g_wsPopupPinIdx = -1;
                    KillTimer(hwnd, WS_POPUP_TIMER_ID);
                    ShowWindow(hwnd, SW_HIDE);
                    SetWsPopupRectSafe(0, 0, 0, 0);
                    return 0;
                }
            } else {
                g_wsPopupAnimProg = std::min(1.0f, elapsedMs / WS_POPUP_OPEN_MS);
                if (g_wsPopupAnimProg >= 0.999f) {
                    g_wsPopupAnimProg = 1.0f;
                    g_wsPopupAnimating = false;
                    KillTimer(hwnd, WS_POPUP_TIMER_ID);
                }
            }
            if (g_wsPopupDC && g_wsPopupCachedW > 0 && g_wsPopupCachedH > 0) {
                float t = g_wsPopupAnimProg;
                float ease = t * t * (3.0f - 2.0f * t);
                BYTE masterAlpha = (BYTE)std::max(0, std::min(255, (int)lroundf(255.0f * ease)));
                int riseOffset = (int)lroundf((1.0f - ease) * 6.0f * g_wsPopupScaleS);
                int py = g_wsPopupFlipDown ? (g_wsPopupTargetY - riseOffset) : (g_wsPopupTargetY + riseOffset);
                SetWsPopupRectSafe(g_wsPopupBaseX, py, g_wsPopupBaseX + g_wsPopupCachedW, py + g_wsPopupCachedH);
                HDC sdc = GetDC(NULL);
                if (sdc) {
                    POINT dest = { g_wsPopupBaseX, py };
                    SIZE  sz   = { g_wsPopupCachedW, g_wsPopupCachedH };
                    POINT ptSrc = { 0, 0 };
                    BLENDFUNCTION bf = { AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA };
                    UpdateLayeredWindow(hwnd, sdc, &dest, &sz, g_wsPopupDC, &ptSrc, 0, &bf, ULW_ALPHA);
                    if (!IsWindowVisible(hwnd))
                        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
                    ReleaseDC(NULL, sdc);
                }
            }
            return 0;
        }
        if (wParam == WS_POPUP_CARET_TIMER_ID) {
            if (g_wsPopupRenaming) {
                g_wsPopupCaretVisible = !g_wsPopupCaretVisible;
                RenderWorkspacePopupULW();
            } else {
                KillTimer(hwnd, WS_POPUP_CARET_TIMER_ID);
            }
            return 0;
        }
        break;

    case WM_MOUSEMOVE: {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        if (!g_wsPopupMouseTrack) {
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            g_wsPopupMouseTrack = TrackMouseEvent(&tme) != FALSE;
        }
        bool onDel = false;
        int row = WsPopupHitTestRow(pt, &onDel);
        bool dirty = false;
        if (g_wsPopupPressRow >= 0 && !g_wsPopupDraggingRow) {
            if (abs(pt.x - g_wsPopupPressPt.x) > DRAG_THRESHOLD_PX ||
                abs(pt.y - g_wsPopupPressPt.y) > DRAG_THRESHOLD_PX) {
                int visCap = std::max(3, std::min(20, POPUP_VISIBLE_PATHS.load()));
                int drawCount = std::min(std::max(0, g_wsPopupRowCount - g_wsPopupScrollOffset), visCap);
                int dragVis = g_wsPopupPressRow - g_wsPopupScrollOffset;
                float startRowY = (float)(g_wsPopupRowTopY + dragVis * g_wsPopupRowH);
                g_wsPopupDragAnchorY = (float)g_wsPopupPressPt.y - startRowY;
                g_wsPopupDragCurY    = startRowY;
                g_wsPopupRowAnimY.resize(drawCount);
                for (int v = 0; v < drawCount; ++v)
                    g_wsPopupRowAnimY[v] = (float)(g_wsPopupRowTopY + v * g_wsPopupRowH);
                QueryPerformanceCounter(&g_wsPopupRowAnimLastQpc);
                g_wsPopupDraggingRow = true;
                g_wsPopupDragRow     = g_wsPopupPressRow;
                g_wsPopupDragDropRow = g_wsPopupPressRow;
                g_wsPopupHoverRow    = -1;
                g_wsPopupHoverDelRow = -1;
                SetTimer(hwnd, WS_POPUP_TIMER_ID, 8, NULL);
                dirty = true;
            }
        }
        if (!g_wsPopupDraggingRow && g_wsPopupPressRow < 0) {
            bool hdrHot = (!g_wsPopupRenaming && PtInRect(&g_wsPopupHeaderRc, pt) != 0);
            if (row != g_wsPopupHoverRow || (onDel ? row : -1) != g_wsPopupHoverDelRow || hdrHot != g_wsPopupHeaderHover)
                dirty = true;
            g_wsPopupHoverRow    = row;
            g_wsPopupHoverDelRow = onDel ? row : -1;
            g_wsPopupHeaderHover = hdrHot;
        }
        if (g_wsPopupDraggingRow) {
            int visCap = std::max(3, std::min(20, POPUP_VISIBLE_PATHS.load()));
            int maxVis = std::min(g_wsPopupRowCount - g_wsPopupScrollOffset, visCap);
            if (maxVis < 1) maxVis = 1;
            float minY = (float)g_wsPopupRowTopY;
            float maxY = (float)(g_wsPopupRowTopY + (maxVis - 1) * g_wsPopupRowH);
            float nextY = std::max(minY, std::min(maxY, (float)pt.y - g_wsPopupDragAnchorY));
            if (fabsf(nextY - g_wsPopupDragCurY) >= 0.25f) {
                g_wsPopupDragCurY = nextY;
                dirty = true;
            }
            float centerY = g_wsPopupDragCurY + (float)g_wsPopupRowH * 0.5f;
            int targetVis = (int)floorf((centerY - (float)g_wsPopupRowTopY) / (float)std::max(1, g_wsPopupRowH));
            targetVis = std::max(0, std::min(maxVis - 1, targetVis));
            int newDrop = std::max(0, std::min(g_wsPopupRowCount - 1, g_wsPopupScrollOffset + targetVis));
            if (newDrop != g_wsPopupDragDropRow) {
                g_wsPopupDragDropRow = newDrop;
                dirty = true;
            }
        }
        if (g_wsPopupState.load() == WS_POPUP_OPEN_HOVER)
            g_wsPopupLingerUntil = GetTickCount() + 220;
        if (dirty) RenderWorkspacePopupULW();
        return 0;
    }

    case WM_MOUSELEAVE:
        g_wsPopupMouseTrack  = false;
        if (g_wsPopupHoverRow != -1 || g_wsPopupHoverDelRow != -1 || g_wsPopupHeaderHover) {
            g_wsPopupHoverRow    = -1;
            g_wsPopupHoverDelRow = -1;
            g_wsPopupHeaderHover = false;
            RenderWorkspacePopupULW();
        }
        if (g_wsPopupState.load() == WS_POPUP_OPEN_HOVER)
            g_wsPopupLingerUntil = GetTickCount() + 200;
        return 0;

    case WM_LBUTTONDOWN: {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        if (g_wsPopupState.load() == WS_POPUP_OPEN_HOVER)
            g_wsPopupState = WS_POPUP_OPEN_HOLD;
        if (PtInRect(&g_wsPopupHeaderRc, pt)) {
            if (!g_wsPopupRenaming)
                BeginWsPopupRename(hwnd);
            return 0;
        }
        if (g_wsPopupRenaming)
            CommitWsPopupRename(hwnd, true);
        bool onDel = false;
        int row = WsPopupHitTestRow(pt, &onDel);
        if (row >= 0 && onDel) {
            g_wsPopupPressRow    = -1;
            g_wsPopupDraggingRow = false;
            int pinIdx = g_wsPopupPinIdx.load();
            RemoveWorkspacePath(pinIdx, row);
            g_wsPopupHoverDelRow = -1;
            if (g_wsPopupHoverRow >= g_wsPopupRowCount - 1)
                g_wsPopupHoverRow = -1;
            RenderWorkspacePopupULW();
            return 0;
        }
        if (row >= 0) {
            g_wsPopupPressRow    = row;
            g_wsPopupPressPt     = pt;
            g_wsPopupDragRow     = -1;
            g_wsPopupDragDropRow = -1;
            g_wsPopupDraggingRow = false;
            SetCapture(hwnd);
        }
        return 0;
    }

    case WM_LBUTTONDBLCLK: {
        POINT pt = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        if (PtInRect(&g_wsPopupHeaderRc, pt)) {
            if (!g_wsPopupRenaming)
                BeginWsPopupRename(hwnd);
            return 0;
        }
        if (GetCapture() == hwnd) ReleaseCapture();
        g_wsPopupPressRow    = -1;
        g_wsPopupDraggingRow = false;
        g_wsPopupDragRow     = -1;
        g_wsPopupDragDropRow = -1;
        bool onDel = false;
        int row = WsPopupHitTestRow(pt, &onDel);
        if (row >= 0 && !onDel && g_csInitialized) {
            int pinIdx = g_wsPopupPinIdx.load();
            std::wstring targetPath;
            EnterCriticalSection(&g_cs);
            if (pinIdx >= 0 && pinIdx < (int)g_pinnedApps.size() &&
                g_pinnedApps[pinIdx].type == PIN_WORKSPACE &&
                row < (int)g_pinnedApps[pinIdx].workspacePaths.size()) {
                targetPath = g_pinnedApps[pinIdx].workspacePaths[row];
            }
            LeaveCriticalSection(&g_cs);
            if (!targetPath.empty()) {
                CloseWorkspacePopupOnUi(true);
                OpenSingleFolderAsync(targetPath);
            }
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (GetCapture() == hwnd) ReleaseCapture();
        bool wasDragging = g_wsPopupDraggingRow;
        int pinIdx = g_wsPopupPinIdx.load();
        int fromR  = g_wsPopupDragRow;
        int toR    = g_wsPopupDragDropRow;
        g_wsPopupPressRow    = -1;
        g_wsPopupDraggingRow = false;
        g_wsPopupDragRow     = -1;
        g_wsPopupDragDropRow = -1;
        g_wsPopupRowAnimY.clear();
        if (!g_wsPopupAnimating) KillTimer(hwnd, WS_POPUP_TIMER_ID);
        if (wasDragging) {
            if (fromR >= 0 && toR >= 0 && fromR != toR)
                ReorderWorkspacePath(pinIdx, fromR, toR);
            RenderWorkspacePopupULW();
        }
        return 0;
    }

    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && g_wsPopupRenaming) {
            CommitWsPopupRename(hwnd, true);
        }
        return 0;

    case WM_CAPTURECHANGED:
        g_wsPopupPressRow = -1;
        if (g_wsPopupDraggingRow) {
            g_wsPopupDraggingRow = false;
            g_wsPopupDragRow     = -1;
            g_wsPopupDragDropRow = -1;
            RenderWorkspacePopupULW();
        }
        return 0;

    case WM_MOUSEWHEEL: {
        short wheel = GET_WHEEL_DELTA_WPARAM(wParam);
        int visCap = std::max(3, std::min(20, POPUP_VISIBLE_PATHS.load()));
        int maxVis = std::min(g_wsPopupRowCount, visCap);
        int maxScroll = std::max(0, g_wsPopupRowCount - maxVis);
        if (maxScroll > 0) {
            int nextOff = g_wsPopupScrollOffset + (wheel < 0 ? 1 : -1);
            nextOff = std::max(0, std::min(nextOff, maxScroll));
            if (nextOff != g_wsPopupScrollOffset) {
                g_wsPopupScrollOffset = nextOff;
                if (g_wsPopupState.load() == WS_POPUP_OPEN_HOVER)
                    g_wsPopupLingerUntil = GetTickCount() + 260;
                RenderWorkspacePopupULW();
            }
        }
        return 0;
    }

    case WM_CHAR:
        if (g_wsPopupRenaming) {
            wchar_t ch = (wchar_t)wParam;
            if (ch >= 0x20 && ch != 0x7F) {
                if (g_wsPopupRenameSelAll) {
                    g_wsPopupRenameText.clear();
                    g_wsPopupRenameCaret  = 0;
                    g_wsPopupRenameSelAll = false;
                }
                if (g_wsPopupRenameText.size() < 48) {
                    if (g_wsPopupRenameCaret > g_wsPopupRenameText.size())
                        g_wsPopupRenameCaret = g_wsPopupRenameText.size();
                    g_wsPopupRenameText.insert(g_wsPopupRenameCaret, 1, ch);
                    g_wsPopupRenameCaret++;
                }
                g_wsPopupCaretVisible = true;
                RenderWorkspacePopupULW();
            }
            return 0;
        }
        break;

    case WM_KEYDOWN:
        if (g_wsPopupRenaming) {
            bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            if (wParam == VK_RETURN) {
                CommitWsPopupRename(hwnd, true);
                return 0;
            }
            if (wParam == VK_ESCAPE) {
                CommitWsPopupRename(hwnd, false);
                return 0;
            }
            if (ctrl && (wParam == 'A' || wParam == 'a')) {
                g_wsPopupRenameSelAll = true;
                g_wsPopupRenameCaret  = g_wsPopupRenameText.size();
                RenderWorkspacePopupULW();
                return 0;
            }
            if (wParam == VK_BACK) {
                if (g_wsPopupRenameSelAll) {
                    g_wsPopupRenameText.clear();
                    g_wsPopupRenameCaret  = 0;
                    g_wsPopupRenameSelAll = false;
                } else if (g_wsPopupRenameCaret > 0 && g_wsPopupRenameCaret <= g_wsPopupRenameText.size()) {
                    g_wsPopupRenameText.erase(g_wsPopupRenameCaret - 1, 1);
                    g_wsPopupRenameCaret--;
                }
                g_wsPopupCaretVisible = true;
                RenderWorkspacePopupULW();
                return 0;
            }
            if (wParam == VK_DELETE) {
                if (g_wsPopupRenameSelAll) {
                    g_wsPopupRenameText.clear();
                    g_wsPopupRenameCaret  = 0;
                    g_wsPopupRenameSelAll = false;
                } else if (g_wsPopupRenameCaret < g_wsPopupRenameText.size()) {
                    g_wsPopupRenameText.erase(g_wsPopupRenameCaret, 1);
                }
                g_wsPopupCaretVisible = true;
                RenderWorkspacePopupULW();
                return 0;
            }
            if (wParam == VK_LEFT) {
                if (g_wsPopupRenameSelAll) {
                    g_wsPopupRenameCaret  = 0;
                    g_wsPopupRenameSelAll = false;
                } else if (g_wsPopupRenameCaret > 0) {
                    g_wsPopupRenameCaret--;
                }
                g_wsPopupCaretVisible = true;
                RenderWorkspacePopupULW();
                return 0;
            }
            if (wParam == VK_RIGHT) {
                if (g_wsPopupRenameSelAll) {
                    g_wsPopupRenameCaret  = g_wsPopupRenameText.size();
                    g_wsPopupRenameSelAll = false;
                } else if (g_wsPopupRenameCaret < g_wsPopupRenameText.size()) {
                    g_wsPopupRenameCaret++;
                }
                g_wsPopupCaretVisible = true;
                RenderWorkspacePopupULW();
                return 0;
            }
            if (wParam == VK_HOME) {
                g_wsPopupRenameCaret  = 0;
                g_wsPopupRenameSelAll = false;
                g_wsPopupCaretVisible = true;
                RenderWorkspacePopupULW();
                return 0;
            }
            if (wParam == VK_END) {
                g_wsPopupRenameCaret  = g_wsPopupRenameText.size();
                g_wsPopupRenameSelAll = false;
                g_wsPopupCaretVisible = true;
                RenderWorkspacePopupULW();
                return 0;
            }
            return 0;
        }
        if (wParam == VK_ESCAPE && g_wsPopupState.load() != WS_POPUP_CLOSED) {
            PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 0, 0);
            return 0;
        }
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void EnsureWsPopupWindow() {
    if (g_wsPopupWnd && IsWindow(g_wsPopupWnd)) return;
    static bool s_wsCls = false;
    if (!s_wsCls) {
        WNDCLASSEXW wc = { sizeof(wc) };
        wc.lpfnWndProc   = WsPopupProc;
        wc.hInstance     = GetModHInstance();
        wc.lpszClassName = L"QPDockWorkspacePopup";
        wc.style         = CS_DBLCLKS;
        RegisterClassExW(&wc);
        s_wsCls = true;
    }
    g_wsPopupWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        L"QPDockWorkspacePopup", L"", WS_POPUP,
        0, 0, 316, 80, NULL, NULL, GetModHInstance(), NULL);
    if (g_wsPopupWnd) ApplyCaptureAffinity(g_wsPopupWnd);
}

static void OpenWorkspacePopupOnUi(int pinIdx, bool hold) {
    if (pinIdx < 0) return;
    if (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0) return;
    EnsureWsPopupWindow();
    if (!g_wsPopupWnd) return;

    int curState = g_wsPopupState.load();
    int curPin   = g_wsPopupPinIdx.load();
    if (curPin == pinIdx && (curState == WS_POPUP_OPEN_HOVER || curState == WS_POPUP_OPEN_HOLD)) {
        if (hold && curState != WS_POPUP_OPEN_HOLD)
            g_wsPopupState = WS_POPUP_OPEN_HOLD;
        g_wsPopupLingerUntil = GetTickCount() + 220;
        return;
    }

    HideTooltipWindow();
    g_wsPopupPinIdx       = pinIdx;
    g_wsPopupState        = hold ? WS_POPUP_OPEN_HOLD : WS_POPUP_OPEN_HOVER;
    g_wsPopupHoverRow     = -1;
    g_wsPopupHoverDelRow  = -1;
    g_wsPopupDraggingRow  = false;
    g_wsPopupScrollOffset = 0;
    g_wsPopupLingerUntil = GetTickCount() + 220;

    if (g_wsPopupQpcFreq.QuadPart == 0)
        QueryPerformanceFrequency(&g_wsPopupQpcFreq);
    QueryPerformanceCounter(&g_wsPopupAnimStart);
    if (g_wsPopupBlurBmp) { delete g_wsPopupBlurBmp; g_wsPopupBlurBmp = nullptr; }
    g_wsPopupBlurOrigH = 0;
    g_wsPopupAnimProg  = 0.05f;
    g_wsPopupAnimating = true;
    SetTimer(g_wsPopupWnd, WS_POPUP_TIMER_ID, 10, NULL);
    RenderWorkspacePopupULW();
}

static void CloseWorkspacePopupOnUi(bool immediate) {
    if (g_wsPopupRenaming && g_wsPopupWnd)
        CommitWsPopupRename(g_wsPopupWnd, true);
    int st = g_wsPopupState.load();
    if (st == WS_POPUP_CLOSED) return;
    if (!g_wsPopupWnd || !IsWindow(g_wsPopupWnd)) {
        g_wsPopupState = WS_POPUP_CLOSED;
        g_wsPopupPinIdx = -1;
        return;
    }
    if (immediate) {
        g_wsPopupAnimating = false;
        g_wsPopupState = WS_POPUP_CLOSED;
        g_wsPopupPinIdx = -1;
        KillTimer(g_wsPopupWnd, WS_POPUP_TIMER_ID);
        ShowWindow(g_wsPopupWnd, SW_HIDE);
        SetWsPopupRectSafe(0, 0, 0, 0);
        return;
    }
    if (st != WS_POPUP_CLOSING) {
        g_wsPopupState = WS_POPUP_CLOSING;
        if (g_wsPopupQpcFreq.QuadPart == 0)
            QueryPerformanceFrequency(&g_wsPopupQpcFreq);
        QueryPerformanceCounter(&g_wsPopupAnimStart);
        g_wsPopupAnimating = true;
        SetTimer(g_wsPopupWnd, WS_POPUP_TIMER_ID, 10, NULL);
    }
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

static void RenderOverlayULW() {
    if (!g_overlayWnd || !IsWindow(g_overlayWnd) || !g_csInitialized) return;
    if (g_layoutUnsupported || g_dockLocalW <= 0 || g_dockLocalH <= 0) return;
    if (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0) return;

    int w = g_dockLocalW;
    int h = g_dockLocalH + DOCK_TOP_OVERFLOW_PX;
    int x = (int)g_dockCurrentX;
    int y = (int)g_dockCurrentY - DOCK_TOP_OVERFLOW_PX;

    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    if (!EnsurePaintBuffer(sdc, w, h)) {
        ReleaseDC(NULL, sdc);
        return;
    }

    BITMAP bm = {};
    if (GetObjectW(g_paintDIB, sizeof(bm), &bm) && bm.bmBits)
        memset(bm.bmBits, 0, (size_t)w * h * 4);

    EnterCriticalSection(&g_cs);
    int n = (int)g_pinnedApps.size();

    Gdiplus::Graphics g(g_paintDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeDefault);

    // 1. Dock pill surface (inside [0, DOCK_TOP_OVERFLOW_PX, w, g_dockLocalH])
    {
        float radius = (CORNER_ROUNDNESS <= 0)
            ? 0.0f
            : ((float)g_dockLocalH * 0.5f) * ((float)CORNER_ROUNDNESS / 100.0f);
        Gdiplus::RectF pillRc(0.5f, (float)DOCK_TOP_OVERFLOW_PX + 1.5f,
                              (float)w - 1.0f, (float)g_dockLocalH - 3.0f);
        Gdiplus::GraphicsPath pillPath;
        AddRoundedRectPath(pillPath, pillRc, radius);

        bool light = IsSystemLightTheme();
        if (DOCK_STYLE == 0) {
            // True frosted glass pill: low-opacity translucent frost letting taskbar Mica/Acrylic shine through
            Gdiplus::LinearGradientBrush glassGrad(
                Gdiplus::PointF(0.0f, pillRc.Y),
                Gdiplus::PointF(0.0f, pillRc.Y + pillRc.Height),
                light ? Gdiplus::Color(72, 252, 253, 255) : Gdiplus::Color(62, 54, 62, 76),
                light ? Gdiplus::Color(48, 232, 238, 248) : Gdiplus::Color(42, 28, 32, 42));
            g.FillPath(&glassGrad, &pillPath);
            Gdiplus::Pen topHi(light
                ? Gdiplus::Color(145, 255, 255, 255)
                : Gdiplus::Color(82, 255, 255, 255), 1.0f);
            g.DrawLine(&topHi, pillRc.X + radius, pillRc.Y + 0.5f,
                       pillRc.X + pillRc.Width - radius, pillRc.Y + 0.5f);
            if (!HIDE_DOCK_BORDER) {
                Gdiplus::Pen border(light
                    ? Gdiplus::Color(68, 255, 255, 255)
                    : Gdiplus::Color(48, 255, 255, 255), 1.0f);
                g.DrawPath(&border, &pillPath);
            }
        } else {
            // Transparent mode: 1/255 alpha keeps clicks inside the pill owned by the dock
            Gdiplus::SolidBrush hitFill(Gdiplus::Color(1, 0, 0, 0));
            g.FillPath(&hitFill, &pillPath);
        }

        // Integrated lock/drag glow inside the dock pill (replaces separate g_lockGlowWnd)
        if (g_lockGlowStart != 0) {
            DWORD elapsed = GetTickCount() - g_lockGlowStart;
            float t = g_dragGlowHold ? 1.0f : (float)elapsed / (float)LOCK_GLOW_MS;
            float a = g_dragGlowHold ? 1.0f : LockGlowAlpha01(t);
            if (a > 0.01f) {
                LockGlowRGB col = LockGlowColor(g_lockGlowKind, a);
                BYTE glowA = (BYTE)std::min(255, (int)(a * 115.0f));
                if (g_lockGlowMode == LOCKGLOW_MODE_SWEEP && !g_dragGlowHold) {
                    float sweepW = std::max(8.0f, pillRc.Width * LockGlowSweepX01(t));
                    g.SetClip(&pillPath);
                    Gdiplus::LinearGradientBrush sweepGrad(
                        Gdiplus::PointF(pillRc.X, 0.0f),
                        Gdiplus::PointF(pillRc.X + sweepW, 0.0f),
                        Gdiplus::Color((BYTE)(glowA * 0.45f), col.r, col.g, col.b),
                        Gdiplus::Color(0, col.r, col.g, col.b));
                    Gdiplus::Color colors[3] = {
                        Gdiplus::Color((BYTE)(glowA * 0.35f), col.r, col.g, col.b),
                        Gdiplus::Color(glowA, col.r, col.g, col.b),
                        Gdiplus::Color(0, col.r, col.g, col.b)
                    };
                    Gdiplus::REAL pos[3] = { 0.0f, 0.68f, 1.0f };
                    sweepGrad.SetInterpolationColors(colors, pos, 3);
                    g.FillRectangle(&sweepGrad, pillRc.X, pillRc.Y, sweepW, pillRc.Height);
                    g.ResetClip();
                } else {
                    Gdiplus::SolidBrush glowFill(Gdiplus::Color((BYTE)(glowA / 3), col.r, col.g, col.b));
                    g.FillPath(&glowFill, &pillPath);
                    Gdiplus::Pen glowPen(Gdiplus::Color(std::min(255, glowA + 60), col.r, col.g, col.b), 2.0f);
                    g.DrawPath(&glowPen, &pillPath);
                }
            }
        }
    }

    if (n == 0) {
        g_hoverIndex      = -1;
        g_dragFromDockIdx = -1;
        LeaveCriticalSection(&g_cs);
        POINT dest = { x, y };
        SIZE  sz   = { w, h };
        POINT src  = { 0, 0 };
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        UpdateLayeredWindow(g_overlayWnd, sdc, &dest, &sz, g_paintDC, &src, 0, &bf, ULW_ALPHA);
        ReleaseDC(NULL, sdc);
        return;
    }

    POINT cursorPt;
    GetCursorPos(&cursorPt);

    int shakeOffset = 0;
    if (g_shakeActive) {
        int elapsed = (int)(GetTickCount() - g_shakeStart);
        if (elapsed < SHAKE_DURATION_MS) {
            float t = (float)elapsed / (float)SHAKE_DURATION_MS;
            shakeOffset = (int)(SHAKE_AMPLITUDE_PX * sinf(t * 3.14159f * 5.f) * (1.f - t));
        } else {
            g_shakeActive = false;
        }
    }

    // 2. Gold workspace divider pill
    int sepWsSlots  = GetVisibleWorkspaceSlots(CountPinsByType(PIN_WORKSPACE));
    int sepAppCount = CountPinsByType(PIN_APP);
    if (ShouldDrawWorkspaceDivider(SHOW_WORKSPACE_DIVIDER, sepWsSlots, sepAppCount)) {
        int sepWsRegionW = (sepWsSlots * ICON_SIZE) + ((sepWsSlots - 1) * ICON_SPACING);
        int sepX = DOCK_PAD_LEFT + sepWsRegionW + SECTION_GAP / 2;
        float pillW = (float)std::max(3, std::min(5, ICON_SIZE / 9));
        float inset = 8.0f;
        float pillTop = (float)DOCK_TOP_OVERFLOW_PX + inset;
        float pillH   = (float)g_dockLocalH - inset * 2.0f;
        if (pillH > 4.0f) {
            Gdiplus::GraphicsPath divPath;
            AddRoundedRectPath(divPath, Gdiplus::RectF((float)sepX - pillW * 0.5f, pillTop, pillW, pillH), pillW * 0.5f);
            Gdiplus::SolidBrush divBr(Gdiplus::Color(240, 255, 200, 50));
            g.FillPath(&divBr, &divPath);
        }
    }

    auto drawAppIcon = [&](PinnedApp& app, float dx, float dy, float dsz, float opacity) {
        std::wstring wantHex = (app.type == PIN_WORKSPACE && app.workspacePaths.size() > 1)
                             ? app.colorHex : std::wstring();
        if (app.icon && (!app.cachedBmp || app.renderedColorHex != wantHex)) {
            Gdiplus::Bitmap* fresh = BuildGdiPlusBitmapFromIcon(app.icon, wantHex);
            if (fresh) {
                if (app.cachedBmp) {
                    if (app.prevCachedBmp) delete app.prevCachedBmp;
                    app.prevCachedBmp = app.cachedBmp;
                    app.colorAnimProgress = 0.0f;
                } else {
                    app.colorAnimProgress = 1.0f;
                }
                app.cachedBmp = fresh;
                app.renderedColorHex = wantHex;
            }
        }
        if (!app.cachedBmp) return;

        auto drawBmpAlpha = [&](Gdiplus::Bitmap* bmp, float a) {
            if (!bmp || a <= 0.005f) return;
            Gdiplus::RectF dstRc(dx, dy, dsz, dsz);
            if (a >= 0.995f) {
                g.DrawImage(bmp, dstRc, 0.0f, 0.0f, (float)bmp->GetWidth(), (float)bmp->GetHeight(), Gdiplus::UnitPixel);
            } else {
                Gdiplus::ColorMatrix cm = {{
                    {1, 0, 0, 0, 0},
                    {0, 1, 0, 0, 0},
                    {0, 0, 1, 0, 0},
                    {0, 0, 0, a, 0},
                    {0, 0, 0, 0, 1}
                }};
                Gdiplus::ImageAttributes ia;
                ia.SetColorMatrix(&cm, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
                g.DrawImage(bmp, dstRc, 0.0f, 0.0f, (float)bmp->GetWidth(), (float)bmp->GetHeight(), Gdiplus::UnitPixel, &ia);
            }
        };

        if (app.prevCachedBmp && app.colorAnimProgress < 0.999f) {
            float t = std::max(0.0f, std::min(1.0f, app.colorAnimProgress));
            float ease = t * t * (3.0f - 2.0f * t);
            drawBmpAlpha(app.prevCachedBmp, opacity * (1.0f - ease));
            drawBmpAlpha(app.cachedBmp, opacity * ease);
        } else {
            drawBmpAlpha(app.cachedBmp, opacity);
        }
    };

    // 3. Render icons with Bicubic GDI+ interpolation
    for (int i = 0; i < n; ++i) {
        auto& app = g_pinnedApps[i];
        if (!app.icon) continue;
        if (g_dragFromDock && i == g_dragFromDockIdx && g_dragState == DRAG_DRAGGING)
            continue;
        if (app.opacity <= 0.01f) continue;

        RECT r = GetIconRectLocal(i, n);
        float baseX = (app.currentX != 0.f ? app.currentX : (float)r.left) + (float)shakeOffset;
        if (baseX < DOCK_PAD_LEFT) baseX = (float)(r.left + shakeOffset);
        baseX += app.hoverShiftX;
        if (baseX < 0.0f || baseX >= (float)g_dockLocalW) continue;

        float scale = (app.opacity >= 0.99f && app.hoverScale > 1.0f) ? app.hoverScale : 1.0f;
        float sz    = (float)ICON_SIZE * scale;
        float cxF   = baseX + ICON_SIZE * 0.5f;
        float cyF   = (float)(DOCK_TOP_OVERFLOW_PX + r.top) + ICON_SIZE * 0.5f;
        float ix    = std::max(0.0f, cxF - sz * 0.5f);
        float iy    = cyF - sz * 0.5f;
        if (scale <= 1.001f) {
            ix = (float)lroundf(ix);
            iy = (float)lroundf(iy);
            sz = (float)ICON_SIZE;
        }
        float edgeFade = 1.0f;
        if (ix < (float)DOCK_PAD_LEFT)
            edgeFade = std::max(0.0f, 1.0f - ((float)DOCK_PAD_LEFT - ix) / (float)std::max(8, ICON_SIZE / 2));
        else if (ix + sz > (float)(w - DOCK_PAD_RIGHT))
            edgeFade = std::max(0.0f, 1.0f - (ix + sz - (float)(w - DOCK_PAD_RIGHT)) / (float)std::max(8, ICON_SIZE / 2));
        drawAppIcon(app, ix, iy, sz, app.opacity * edgeFade);
        if (app.type == PIN_WORKSPACE && app.workspacePaths.size() > 1) {
            int cnt = (int)app.workspacePaths.size();
            std::wstring cntStr = std::to_wstring(cnt);
            float bh = std::max(14.0f, sz * 0.42f);
            float bw = (cnt >= 10) ? (bh * 1.28f) : bh;
            float bx = std::min((float)w - bw - 0.5f, ix + sz - bw * 0.5f);
            float by = std::max(0.5f, iy - bh * 0.5f);
            Gdiplus::GraphicsPath bPath;
            AddRoundedRectPath(bPath, Gdiplus::RectF(bx, by, bw, bh), bh * 0.5f);
            BYTE ba = (BYTE)std::max(0, std::min(255, (int)lroundf(242.0f * app.opacity)));
            Gdiplus::SolidBrush bFill(IsSystemLightTheme()
                ? Gdiplus::Color(ba, 28, 30, 36)
                : Gdiplus::Color(ba, 42, 128, 255));
            g.FillPath(&bFill, &bPath);
            Gdiplus::FontFamily bff(L"Segoe UI");
            Gdiplus::Font bFont(&bff, bh * 0.64f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
            Gdiplus::StringFormat bFmt;
            bFmt.SetAlignment(Gdiplus::StringAlignmentCenter);
            bFmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            Gdiplus::SolidBrush bTxt(Gdiplus::Color(ba, 255, 255, 255));
            g.DrawString(cntStr.c_str(), -1, &bFont, Gdiplus::RectF(bx, by, bw, bh), &bFmt, &bTxt);
        }
    }

    // 4. Running-state indicator dots
    {
        Gdiplus::SolidBrush dotBr(Gdiplus::Color(235, 200, 215, 245));
        for (int i = 0; i < n; ++i) {
            const auto& app = g_pinnedApps[i];
            if (!app.running || !app.icon || app.opacity < 0.5f) continue;
            if (g_dragFromDock && i == g_dragFromDockIdx && g_dragState == DRAG_DRAGGING) continue;
            RECT r = GetIconRectLocal(i, n);
            float baseX = (app.currentX > 0.f ? app.currentX : (float)r.left) + app.hoverShiftX;
            if (baseX < 0.0f || baseX >= (float)g_dockLocalW) continue;
            float cx = baseX + ICON_SIZE * 0.5f;
            float dotY = (float)(DOCK_TOP_OVERFLOW_PX + r.bottom + 2);
            if (dotY + 3.5f <= (float)h)
                g.FillEllipse(&dotBr, cx - 1.8f, dotY, 3.6f, 3.6f);
        }
    }

    // 5. Reorder drag floating icon
    if (g_dragState == DRAG_REORDER && g_reorderSrcIdx >= 0 && g_reorderSrcIdx < n) {
        auto& srcApp = g_pinnedApps[g_reorderSrcIdx];
        if (srcApp.icon) {
            float sz = (float)ICON_SIZE;
            float localX = (float)(cursorPt.x - (int)g_dockCurrentX) - sz * 0.5f;
            localX = std::max(0.0f, std::min(localX, (float)w - sz));
            float localY = (float)DOCK_TOP_OVERFLOW_PX + ((float)g_dockLocalH - sz) * 0.5f;
            drawAppIcon(srcApp, localX, localY, sz, 0.76f);
        }
    }

    LeaveCriticalSection(&g_cs);

    POINT dest = { x, y };
    SIZE  sz   = { w, h };
    POINT src  = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(g_overlayWnd, sdc, &dest, &sz, g_paintDC, &src, 0, &bf, ULW_ALPHA);
    ReleaseDC(NULL, sdc);
}

// ============================================================
//  OVERLAY WNDPROC
// ============================================================
static bool IsDockCurrentlyActive() {
    bool cachedRectValid = (g_cachedDockRect.right  > g_cachedDockRect.left) &&
                           (g_cachedDockRect.bottom > g_cachedDockRect.top);
    bool autoHideHidden = g_taskbarAutoHide &&
        !((g_cachedTBRect.bottom - g_cachedTBRect.top) > 8 &&
          (g_cachedTBRect.right  - g_cachedTBRect.left) > 8);
    bool fsSuppress = g_fullscreenActive && (FULLSCREEN_BEHAVIOR == 0);
    return DockInteractionAllowed(
        fsSuppress, g_layoutUnsupported,
        g_cachedTaskbar != NULL, g_dockLocalW, cachedRectValid, autoHideHidden);
}

extern std::atomic<bool> g_ewpEnabled;
extern HWND g_ewpPillWnd;
extern HWND g_ewpDockWnd;
extern HWND g_ewpPreviewWnd;
extern HWND g_ewpDropZoneWnd;
extern int  g_ewpPrevActiveWsIdx;
static int  EwpHitTestArcDock(POINT pt, int* outWsIdx, int* outSubAppIdx, bool* outAdd, bool* outOnCardDel = nullptr);
static const UINT WM_EWP_DROP_APP_PATH_FWD = WM_APP + 0x34;
static void EwpRenderPill();
static void EwpRenderHoverDock();
static void EwpCreateWindowsOnUiThread();
static void EwpApplySettingsOnUiThread();

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

    case WM_QPD_RENDER_DOCK:
        g_renderQueued.store(false);
        RenderOverlayULW();
        UpdateTooltipWindow(g_hoverIndex.load());
        if (!g_ewpEnabled.load() || (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0)) {
            EwpRenderPill();
            EwpRenderHoverDock();
        } else if (g_ewpPillWnd && !IsWindowVisible(g_ewpPillWnd)) {
            EwpRenderPill();
        }
        return 0;

    case WM_QPD_WSPOPUP_OPEN:
        OpenWorkspacePopupOnUi((int)wParam, lParam != 0);
        return 0;

    case WM_QPD_WSPOPUP_CLOSE:
        CloseWorkspacePopupOnUi(wParam != 0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);
        EndPaint(hwnd, &ps);
        RenderOverlayULW();
        UpdateTooltipWindow(g_hoverIndex.load());
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

        int hi  = g_hoverIndex.load();
        int cur = (hi >= 0 && hi < wn) ? hi
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
        int vis = GetVisibleAppSlots(appCount);
        int maxStart = appCount - vis;
        if (maxStart < 0) maxStart = 0;

        if (g_pinnedApps[nextIdx].type == PIN_APP) {
            int ord = PinOrdinalWithinType(nextIdx, PIN_APP);
            if (ord < g_appScrollStart)                 g_appScrollStart = ord;
            else if (ord > g_appScrollStart + vis - 1)  g_appScrollStart = ord - (vis - 1);
        } else if (g_pinnedApps[nextIdx].type == PIN_WORKSPACE) {
            int wsCount = CountPinsByType(PIN_WORKSPACE);
            int wsVis   = std::max(1, GetVisibleWorkspaceSlots(wsCount));
            int wsMax   = std::max(0, wsCount - wsVis);
            int wsOrd   = PinOrdinalWithinType(nextIdx, PIN_WORKSPACE);
            int wsStart = g_wsPopupScrollStart.load();
            if (wsOrd < wsStart)                 wsStart = wsOrd;
            else if (wsOrd > wsStart + wsVis - 1) wsStart = wsOrd - (wsVis - 1);
            g_wsPopupScrollStart = std::max(0, std::min(wsStart, wsMax));
        }
        if (g_appScrollStart > maxStart) g_appScrollStart = maxStart;
        if (g_appScrollStart < 0)        g_appScrollStart = 0;

        // Smooth QPC-driven retargeting (no abrupt snap-jump).
        for (int i = 0; i < wn; ++i) {
            RECT r = GetIconRectLocal(i, wn);
            g_pinnedApps[i].targetX = (float)r.left;
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
        ResetGestureGuard();
        return 0;

    case WM_RBUTTONUP:
        return 0;

    case WM_QPD_REREGISTER_HOTKEY:
        ApplyCaptureBehaviorAll();
        SyncKeyboardGestureHookOnUiThread();
        EwpCreateWindowsOnUiThread();
        EwpApplySettingsOnUiThread();
        // FIX (Issue 6): re-register the global hotkey after a settings change.
        // Runs on the overlay window's OWNING thread (this proc), which is the
        // thread that originally called RegisterHotKey -- the only thread on
        // which Register/UnregisterHotKey are valid. Always unregister the old
        // binding first (unconditionally: the previous key may differ from the
        // current globals, so we must not gate the unregister on current values),
        // then register the new one unless the user disabled it (mods/key == 0).
        UnregisterHotKey(hwnd, HOTKEY_PIN_ID);
        if (g_hotkeyKey != 0 && g_hotkeyMods != 0)
            RegisterHotKey(hwnd, HOTKEY_PIN_ID, g_hotkeyMods, g_hotkeyKey);
        Wh_Log(L"HOTKEY: re-registered live mods:0x%X key:0x%X", (UINT)g_hotkeyMods, (UINT)g_hotkeyKey);
        return 0;

    case WM_HOTKEY:
        if ((UINT)wParam == HOTKEY_PIN_ID) {
            if (!IsDockCurrentlyActive()) return 0;
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
                            if (g_pinnedApps[i].type == PIN_APP && ExePathsMatchOrSameApp(g_pinnedApps[i].exePath, path))
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

        int hi  = g_hoverIndex.load();
        int cur = (hi >= 0 && hi < wn) ? hi
                                       : (wheel < 0 ? -1 : wn);
        int nextIdx = cur + (wheel < 0 ? 1 : -1);
        if (nextIdx < 0) nextIdx = 0;
        if (nextIdx > wn - 1) nextIdx = wn - 1;
        bool changed = (nextIdx != g_hoverIndex);
        g_hoverIndex = nextIdx;

        // Viewport paging: keep the highlighted app inside the visible window
        // (same logic as the overlay proc; no-op for <= MAX_VISIBLE_APP_SLOTS).
        int appCount = CountPinsByType(PIN_APP);
        int vis = GetVisibleAppSlots(appCount);
        int maxStart = appCount - vis;
        if (maxStart < 0) maxStart = 0;

        if (g_pinnedApps[nextIdx].type == PIN_APP) {
            int ord = PinOrdinalWithinType(nextIdx, PIN_APP);
            if (ord < g_appScrollStart)                 g_appScrollStart = ord;
            else if (ord > g_appScrollStart + vis - 1)  g_appScrollStart = ord - (vis - 1);
        } else if (g_pinnedApps[nextIdx].type == PIN_WORKSPACE) {
            int wsCount = CountPinsByType(PIN_WORKSPACE);
            int wsVis   = std::max(1, GetVisibleWorkspaceSlots(wsCount));
            int wsMax   = std::max(0, wsCount - wsVis);
            int wsOrd   = PinOrdinalWithinType(nextIdx, PIN_WORKSPACE);
            int wsStart = g_wsPopupScrollStart.load();
            if (wsOrd < wsStart)                 wsStart = wsOrd;
            else if (wsOrd > wsStart + wsVis - 1) wsStart = wsOrd - (wsVis - 1);
            g_wsPopupScrollStart = std::max(0, std::min(wsStart, wsMax));
        }
        if (g_appScrollStart > maxStart) g_appScrollStart = maxStart;
        if (g_appScrollStart < 0)        g_appScrollStart = 0;

        // Smooth QPC-driven retargeting (no abrupt snap-jump).
        for (int i = 0; i < wn; ++i) {
            RECT r = GetIconRectLocal(i, wn);
            g_pinnedApps[i].targetX = (float)r.left;
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
        ResetGestureGuard();
        return 0;

    case WM_RBUTTONUP:
        return 0;
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

static bool CreateInputOwnerWindow() {
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = InputOwnerProc;
    wc.hInstance = GetModHInstance();   // FIX-B3 (#3): mod module, not explorer.exe
    wc.lpszClassName = INPUT_CLASS;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS;
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
        Wh_Log(L"INPUT: CreateWindow failed err=%lu", GetLastError());
        return false;
    }
    SetLayeredWindowAttributes(g_inputWnd, 0, 1, LWA_ALPHA);
    ApplyCaptureAffinity(g_inputWnd);
    return true;
}

static bool CreateOverlayWindow() {
    WNDCLASSEXW wc   = { sizeof(wc) };
    wc.lpfnWndProc   = OverlayProc;
    wc.hInstance     = GetModHInstance();   // FIX-B3 (#3): mod module, not explorer.exe
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
        Wh_Log(L"OVERLAY: CreateWindow failed err=%lu", GetLastError());
        return false;
    }

    // Colour-key for transparency: RGB(1,0,1) magenta.
    // Previously RGB(0,0,0) black was used, but the glass gradient paints
    // near-black values that got incorrectly keyed out -- making the dock
    // appear as a solid black rectangle instead of transparent.
    // RGB(1,0,1) is never produced by the glass gradient or icon rendering,
    // so only the true background pixels become transparent.
    ApplyCaptureAffinity(g_overlayWnd);

    // Register the global pin/unpin hotkey (Ctrl+Alt+P by default).
    // hotkeyKey == 0 or hotkeyMods == 0 means disabled by user.
    // Guard against mods==0: RegisterHotKey with no modifiers intercepts the bare
    // key globally in every application, which is almost never intentional.
    if (g_hotkeyKey != 0 && g_hotkeyMods != 0)
        RegisterHotKey(g_overlayWnd, HOTKEY_PIN_ID, g_hotkeyMods, g_hotkeyKey);

    Wh_Log(L"OVERLAY: created hwnd=%p hotkey=mods:0x%X key:0x%X",
              g_overlayWnd, (UINT)g_hotkeyMods, (UINT)g_hotkeyKey);
    return true;
}

static bool CreateGhostWindow() {
    WNDCLASSEXW gw   = { sizeof(gw) };
    gw.lpfnWndProc   = DefWindowProcW;
    gw.hInstance     = GetModHInstance();   // FIX-B3 (#3): mod module, not explorer.exe
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
        Wh_Log(L"GHOST: CreateWindow failed err=%lu", GetLastError());
        return false;
    }
    ShowWindow(g_ghostWnd, SW_HIDE);
    ApplyCaptureAffinity(g_ghostWnd);
    return true;
}

// ============================================================
//  WINEVENT HOOK  --  event-driven taskbar geometry refresh
// ============================================================
// FIX-A7 (#4): WinEventProc removed together with its SetWinEventHook
// registration. The hook could never fire (inverted event range,
// WINEVENT_SKIPOWNPROCESS excluded explorer.exe, idThread pinned to our own
// thread) and every geometry refresh it would have performed is already driven
// by the worker poll loop below (see the
// HasTaskbarGeometryChanged -> RefreshTaskbarCache -> RepositionOverlay block).

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
            if (g_pinnedApps[i].type == PIN_APP && ExePathsMatchOrSameApp(g_pinnedApps[i].exePath, path)) { idx = i; break; }
        LeaveCriticalSection(&g_cs);
    }
    if (idx >= 0) { UnpinAppByIndex(idx); DragTraceLog(L"KEY-TAP: unpin", path.c_str()); }
}

DWORD WINAPI WorkerThread(LPVOID) {
    // v2.5.4 (this-round Issue 2): match the UI thread's per-monitor-v2 DPI context
    // so the worker's geometry polling (GetWindowRect on the taskbar/Start, dock
    // anchoring, GetDpiForWindow) reads true physical pixels in the tool process.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // FIX-A8 (#11): the optional startup delay now runs HERE, on the worker
    // thread, instead of blocking WhTool_ModInit (which runs on the process main
    // thread at load, or the Windhawk Engine thread on a live reload -- either
    // way a Sleep there stalls Explorer startup / the Windhawk UI). Wait on
    // g_exitEvent so a disable/reload during the delay tears down instantly
    // rather than blocking for up to 3 s.
    if (STARTUP_DELAY_MS > 0) {
        if (WaitForSingleObject(g_exitEvent, (DWORD)STARTUP_DELAY_MS) == WAIT_OBJECT_0)
            return 0;
    }

    bool  lastLDown         = false;
    bool  lastRDown         = false;
    bool  lastEscDown       = false;
    int   pendingDockClickIdx  = -1;
    DWORD pendingDockClickTick = 0;
    int   rPressDockIdx        = -1;
    POINT rPressDockPt         = {};
    DWORD lastGeometryCheck = GetTickCount();
    DWORD bootWatchdogStart = GetTickCount();

    while (WaitForSingleObject(g_exitEvent, 0) != WAIT_OBJECT_0) {
        if (!g_csInitialized) { WaitForSingleObject(g_exitEvent, 16); continue; }

        // ================================================================
        //  SETTINGS RELOAD (Issue 2)  --  marshaled onto the worker thread
        // ================================================================
        // WhTool_ModSettingsChanged runs on an arbitrary Windhawk thread and now
        // only raises g_settingsReloadPending. ALL setting reads and worker-owned
        // geometry writes happen HERE, on the worker -- their sole owner -- so a
        // settings change can never race the worker's reads/writes. Reload + clamp,
        // invalidate the width/position caches (under g_cs, matching how
        // RefreshTaskbarCache guards them), then let the normal geometry poll below
        // re-resolve and re-show the dock. Backdrop + hotkey depend on the freshly
        // read settings, so (re)apply them here too, after LoadSettings has run.
        if (g_settingsReloadPending.exchange(false)) {
            LoadSettings();
            EnterCriticalSection(&g_cs);
            g_dockPositionLocked = false;   // a settings change is explicit, not jitter: let DOCK_GAP_FROM_START apply live
            g_fixedDockWidth     = 0;       // force a width recompute at the new settings
            g_lastDpiForWidth    = 0;
            LeaveCriticalSection(&g_cs);
            g_dockWidthDirty     = true;    // next poll runs RefreshTaskbarCache + RepositionOverlay
            if (g_overlayWnd && IsWindow(g_overlayWnd)) {
                InvalidateRect(g_overlayWnd, NULL, FALSE);
                PostMessageW(g_overlayWnd, WM_QPD_REREGISTER_HOTKEY, 0, 0);
            }
            Wh_Log(L"SETTINGS APPLIED (worker): maxPins=%d iconSz=%d reorder=%d explorerWorkspaces=%d",
                   MAX_PINNED_APPS, BASE_ICON_SIZE, (int)ENABLE_REORDER,
                   (int)ENABLE_EXPLORER_WORKSPACE_PINS);
        }

        POINT cursor = {};
        GetCursorPos(&cursor);
        DWORD now = GetTickCount();

        // Safety geometry poll + running-state + auto-hide check.
        // Poll every 100 ms while still in boot/stabilizing so the dock becomes
        // visible within one poll cycle after WhTool_ModInit.  Once geometry is locked
        // (STATE_STABLE), drop to a relaxed 500 ms cadence to save CPU.
        DWORD pollIntervalMs = (g_systemState != STATE_STABLE) ? 100u : (DWORD)RUNNING_STATE_CHECK_MS;
        if (now - lastGeometryCheck > pollIntervalMs || g_dockWidthDirty) {
            // Fullscreen suppression is cheap and must be sampled every poll so
            // the dock hides/reappears in lock-step with fullscreen apps and the
            // snip overlay -- not only when taskbar geometry happens to change.
            static bool s_prevFullscreenActive = false;
            UpdateFullscreenState();
            // Falling-edge recovery: RepositionOverlay() is the ONLY code path
            // that re-SHOWs the overlay (the 3 s topmost re-assert deliberately
            // omits SWP_SHOWWINDOW, and nothing else calls SW_SHOWNOACTIVATE).
            // While fullscreen / snip / Alt+Tab suppression is active,
            // g_fullscreenActive keeps needRefresh true and the dock stays
            // hidden -- correct. But on the exact poll where suppression CLEARS,
            // g_fullscreenActive has already gone back to false, so without an
            // explicit edge term needRefresh can be false (geometry stable,
            // nothing dirty) and RepositionOverlay() never runs to un-hide the
            // dock. That is the "tested a snip / hit Esc / Alt+Tab and the dock
            // disappeared and never came back" bug. Force exactly one refresh on
            // the true->false transition so the dock reliably reappears.
            bool fullscreenJustCleared = (s_prevFullscreenActive && !g_fullscreenActive);
            s_prevFullscreenActive = g_fullscreenActive;
            bool needRefresh = (g_systemState != STATE_STABLE) || HasTaskbarGeometryChanged() || g_dockWidthDirty || g_fullscreenActive || fullscreenJustCleared;
            if (needRefresh) {
                RefreshTaskbarCache();
                RepositionOverlay();
            }
            if (ENABLE_AUTOHIDE_SYNC) UpdateAutoHideState();
            UpdateRunningState();
            if (g_ewpEnabled.load() && !(g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0) && g_overlayWnd) {
                if (!g_ewpPillWnd || !IsWindow(g_ewpPillWnd)) {
                    PostMessageW(g_overlayWnd, WM_QPD_REREGISTER_HOTKEY, 0, 0);
                } else if (!IsWindowVisible(g_ewpPillWnd)) {
                    needRefresh = true;
                }
            }
            if (needRefresh && g_overlayWnd) PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
            lastGeometryCheck = now;
        }

        // -- Hover pre-sampling: REMOVED --------------------------------------
        // This used to call ResolveDragSourceAtPoint() on a timer purely from
        // idle hovering near the dock/taskbar, with no mouse button down and no
        // drag underway at all. That is exactly the unnecessary work the new
        // drag-intent pipeline exists to eliminate: normal hovering (and, by
        // extension, normal clicks) must never reach the resolver, UI
        // Automation, OpenProcess, or icon extraction. Source identity is now
        // resolved at most once per genuine drag-to-pin, in the DRAG_CANDIDATE
        // block below (Stage 5), only after a real drag AND dock intent AND a
        // pinnable-candidate check have all passed.

        // Boot watchdog: force geometry recovery if dock width is still 0 after 100 ms.
        // FIX (Issue 2): skip while the layout is UNSUPPORTED. There, g_dockLocalW
        // stays 0 by design (RefreshTaskbarCache returns before locking a width),
        // so without this guard the watchdog would hammer RefreshTaskbarCache every
        // 100 ms forever on a left-aligned taskbar. RefreshTaskbarCache still runs
        // on the normal geometry poll above, which re-enables the dock if room
        // reappears -- the watchdog is only for the genuine zero-width boot race.
        if (!g_layoutUnsupported && g_dockLocalW == 0 && (int)(now - bootWatchdogStart) > 100) {
            Wh_Log(L"WATCHDOG: forcing geometry refresh");
            RefreshTaskbarCache();
            if (g_dockLocalW > 0) {
                RepositionOverlay();
                if (g_overlayWnd && IsWindow(g_overlayWnd))
                    InvalidateRect(g_overlayWnd, NULL, FALSE);
            }
            bootWatchdogStart = now;
        }

        // ================================================================
        //  KEYBOARD TRIPLE-TAP GESTURE  --  tap P x3 = pin, tap U x3 = unpin
        // ================================================================
        // Rising-edge count of the bare P / U / L keys. Three taps inside
        // KEY_TAP_WINDOW_MS pin / unpin the focused app (or toggle the lock)
        // through the same validated pipeline as the drag and the Ctrl+Alt+P
        // hotkey. A held key counts as one tap (edge-detected), and the counter
        // resets when taps are too slow so it does not fire during typing.
        //
        // IMPORTANT: this runs BEFORE the "no overlay window" continue-guard
        // below. These gestures act on GetForegroundWindow() and never touch the
        // dock's overlay window, so they must work whenever the feature is on --
        // even if the overlay hasn't come up yet (e.g. while the dock is still
        // stabilising its geometry). Gating them behind the overlay guard was the
        // "P/U/L do nothing even when enabled" bug. (Decision mirrored in
        // tests/key_gesture_gating.h: ShouldEvalKeyGesture takes no overlay arg.)
        {
            wchar_t gesture = g_pendingKeyGesture.exchange(0);
            if (gesture != 0 && IsDockCurrentlyActive()) {
                if (gesture == L'P') {
                    PinForegroundApp();
                } else if (gesture == L'U') {
                    if (!g_iconsLocked) UnpinForegroundApp();
                    else                TriggerLockGlow();
                } else if (gesture == L'L') {
                    g_iconsLocked = !g_iconsLocked;
                    Wh_Log(L"ICON LOCK: %s", g_iconsLocked ? L"LOCKED" : L"UNLOCKED");
                    TriggerLockGlow();
                }
            }
        }

        if (!g_overlayWnd || !IsWindow(g_overlayWnd)) { WaitForSingleObject(g_exitEvent, 16); continue; }

        // Re-assert HWND_TOPMOST every 3 s  --  avoids hammering DWM every frame.
        // Per-frame SetWindowPos was the single largest source of input jitter.
        {
            static DWORD s_lastTopMostMs = 0;
            if (now - s_lastTopMostMs > 3000) {
                // FIX (Issue 3): Z-order maintenance ONLY -- do NOT pass
                // SWP_SHOWWINDOW here. This periodic re-assert must never change
                // visibility: with SWP_SHOWWINDOW it would silently re-show a
                // dock that another code path intentionally hid (e.g. the
                // unsupported/left-aligned layout, or auto-hide). Visibility is
                // owned solely by the ShowWindow calls in those paths; this block
                // only keeps the windows on top.
                if (g_inputWnd && IsWindow(g_inputWnd))
                    SetWindowPos(g_inputWnd, HWND_TOPMOST, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                SetWindowPos(g_overlayWnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                if (g_wsPopupWnd && IsWindow(g_wsPopupWnd) && g_wsPopupState.load() != WS_POPUP_CLOSED)
                    SetWindowPos(g_wsPopupWnd, HWND_TOPMOST, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                if (g_tooltipWnd && IsWindow(g_tooltipWnd) && IsWindowVisible(g_tooltipWnd))
                    SetWindowPos(g_tooltipWnd, HWND_TOPMOST, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                if (g_ewpPillWnd && IsWindow(g_ewpPillWnd)) {
                    if (IsWindowVisible(g_ewpPillWnd)) {
                        SetWindowPos(g_ewpPillWnd, HWND_TOPMOST, 0, 0, 0, 0,
                                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    } else if (!(g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0)) {
                        PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
                    }
                }
                s_lastTopMostMs = now;
            }
        }

        bool lDown   = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        bool rDown   = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        bool escDown = (GetAsyncKeyState(VK_ESCAPE)  & 0x8000) != 0;
        if (escDown && !lastEscDown && g_wsPopupState.load() != WS_POPUP_CLOSED && g_overlayWnd) {
            PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 0, 0);
        }
        lastEscDown = escDown;

        // ================================================================
        //  DRAG STATE MACHINE
        // ================================================================
        // Do not process any drag logic when the layout is unsupported (dock
        // hidden): g_cachedDockRect describes a stale/invalid rect that would
        // accept drops onto a region with no dock visible.
        //
        // FIX (Issue 2 -- unsupported-layout busy loop): every OTHER early
        // `continue` in this loop first parks on g_exitEvent (the 8/16/50 ms
        // adaptive sleeps live BELOW this point), so a bare `continue` here
        // spins the worker at 100 % of a core (GetCursorPos / GetTickCount /
        // IsWindow / GetAsyncKeyState every iteration + EnumWindows every
        // 500 ms) on precisely the harmless layouts the README describes
        // (every Windows 10 machine, left-aligned Windows 11). It also left a
        // timeBeginPeriod(1) active from before the layout flipped. Release the
        // high-res timer and wait on g_exitEvent (so a disable/reload tears
        // down instantly) before looping.
        // ================================================================
        //  DOCK-INACTIVE GATE (v2.5.3) -- a hidden dock is non-interactive
        // ================================================================
        // The overlay is suppressed (kept hidden) whenever a fullscreen/exclusive
        // app or secure snip overlay owns the screen, the layout is unsupported
        // (left-aligned), the taskbar is gone (Explorer restarting), or geometry
        // has not resolved yet (zero width / empty cached rect). In every one of
        // those states the dock is NOT on screen, so NO drag / resolve / dock-zone
        // / pin-unpin / lock-glow work may run against the stale cached geometry.
        // Decide from one mirrored predicate (DockInteractionAllowed,
        // tests/dock_active_gate.h); when the dock is down, cancel any in-flight
        // drag, drop the lightweight candidate, hide the ghost + tether, stop the
        // lock-glow, and skip the whole drag state machine for this frame.
        bool cachedRectValid = (g_cachedDockRect.right  > g_cachedDockRect.left) &&
                               (g_cachedDockRect.bottom > g_cachedDockRect.top);
        // Issue 3: when "Sync with taskbar auto-hide" is ON and the taskbar has
        // slid off screen, RepositionOverlay hides the dock/input window while the
        // cached rect stays valid. Mirror that hide condition (same >8px taskbar
        // threshold) so a dock hidden by auto-hide is treated as non-interactive --
        // no drag / resolve / hit-test / pin-unpin against a dock nobody can see.
        bool autoHideHidden = g_taskbarAutoHide &&
            !((g_cachedTBRect.bottom - g_cachedTBRect.top) > 8 &&
              (g_cachedTBRect.right  - g_cachedTBRect.left) > 8);
        bool fsSuppress = g_fullscreenActive && (FULLSCREEN_BEHAVIOR == 0);
        bool dockInteractive = DockInteractionAllowed(
            fsSuppress, g_layoutUnsupported,
            g_cachedTaskbar != NULL, g_dockLocalW, cachedRectValid, autoHideHidden);
        if (!dockInteractive) {
            // Tear down anything a previous (interactive) frame left mid-flight so
            // the dock cannot resolve, glow, or drop in the background while it is
            // hidden. This runs on the worker thread -- the same thread that owns
            // the drag state machine AND the lock-glow window -- so every reset
            // here is thread-consistent.
            if (g_wsPopupState.load() != WS_POPUP_CLOSED && g_overlayWnd)
                PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 1, 0);
            HideTooltipWindow();
            if (g_dragState != DRAG_IDLE || g_dragCandidateWindow ||
                g_dragGlowHold || g_lockGlowStart != 0) {
                GhostCleanup();                 // drag fields reset, tether hidden, state -> IDLE
                if (g_ghostWnd) ShowWindow(g_ghostWnd, SW_HIDE);
                g_dragCandidateWindow = NULL;   // clear the lightweight drag candidate/intent
                g_dragGlowHold  = false;        // stop any held / animating lock-glow
                g_lockGlowStart = 0;
                if (g_overlayWnd) PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
                DragTraceLog(L"GATE: dock inactive -- drag/resolve/glow suppressed");
            }
            lastLDown = lDown;
            lastRDown = rDown;
            // Unsupported layout parks at the slow idle cadence and releases the
            // hi-res timer (Issue 2: never a 0 ms busy-loop). The other transient
            // inactive states (fullscreen / taskbar gone / booting) poll a little
            // faster so the dock re-arms the instant it returns.
            if (g_layoutUnsupported) { SetHighResTimer(false); WaitForSingleObject(g_exitEvent, 50); }
            else                     { WaitForSingleObject(g_exitEvent, 16); }
            continue;
        }

        // IDLE / CANCELLED -> PRESS (mouse down)
        if (lDown && !lastLDown) ResetGestureGuard();

        extern bool EwpPtInAnyWindow(POINT pt);
        bool clickInsideWsPopup = ((g_wsPopupState.load() != WS_POPUP_CLOSED) &&
                                   PtInWsPopupRectSafe(cursor)) ||
                                  EwpPtInAnyWindow(cursor);
        if ((g_dragState == DRAG_IDLE || g_dragState == DRAG_CANCELLED) &&
            lDown && !lastLDown && !clickInsideWsPopup) {

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

            // ================================================================
            // INTERNAL vs EXTERNAL DRAG SPLIT (single decision point)
            // ================================================================
            // HitTestIcon is the ONE branch that decides which of the two
            // completely separate paths this press belongs to:
            //   dockIdx >= 0  -> INTERNAL drag path (reorder / unpin an
            //                    existing pin). Source is already known --
            //                    it's g_pinnedApps[dockIdx] -- so it NEVER
            //                    resolves, probes UI Automation, opens a
            //                    process, or detects workspaces.
            //   dockIdx <  0  -> EXTERNAL drag path (drag-to-pin from
            //                    Explorer / Chrome / any app / the taskbar).
            //                    Source is UNKNOWN and only becomes known via
            //                    the 5-stage pipeline below, gated on
            //                    g_dragFromDock == false the whole way through
            //                    (see DRAG_CANDIDATE below, which the internal
            //                    path can never enter).
            // These two paths never merge back together: g_dragFromDock, set
            // exactly once here, is what every later stage checks to stay on
            // its own side of the split.
            int dockIdx = HitTestIcon(cursor);
            if (dockIdx >= 0) {
                // ---------------- INTERNAL DRAG PATH ----------------
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
                // ---------------- EXTERNAL DRAG PATH ----------------
                // Source: taskbar button or application window.
                // Stage 1 (Mouse Down): record ONLY the lightweight source
                // classification. WindowFromPoint (via GetRealWindowFromPoint)
                // is existing lightweight hit-testing and is allowed here.
                // ResolveDragSourceAtPoint, UI Automation, OpenProcess, icon
                // extraction, and Explorer workspace resolution are FORBIDDEN
                // at this stage -- every one of them is now deferred to Stage 5,
                // which only runs once a real drag has also shown dock intent
                // over a candidate that survives Stage 4 validation (see the
                // DRAG_CANDIDATE block below). This is what removes the
                // resolver/UIA/OpenProcess/icon-extraction cost from ordinary
                // clicks on Chrome, Explorer, Notepad, Terminal, etc.
                g_dragCandidateWindow = GetRealWindowFromPoint(cursor);
                g_draggedPinType = PIN_APP;
                g_dragState = DRAG_PRESS;
                DragTraceLog(L"PRESS: lightweight candidate recorded (no resolve)");
            }
        }

        // PRESS -> DRAG_REORDER or DRAG_CANDIDATE (real drag threshold crossed)
        if (g_dragState == DRAG_PRESS && lDown) {
            int dx = abs(cursor.x - g_dragStartPt.x);
            int dy = abs(cursor.y - g_dragStartPt.y);
            // Stage 2 (Real Drag Detection): use the system's own drag
            // thresholds (SM_CXDRAG / SM_CYDRAG) rather than a fixed guess --
            // these already reflect the user's pointer/DPI settings. Queried
            // once and cached; they do not change during a session.
            static const int s_dragCX = GetSystemMetrics(SM_CXDRAG);
            static const int s_dragCY = GetSystemMetrics(SM_CYDRAG);
            if (dx > s_dragCX || dy > s_dragCY) {
                // This is the LAST point where the two paths are decided --
                // g_dragFromDock was set once, at PRESS, and is not touched
                // between here and there. From this branch onward the two
                // paths share no code: INTERNAL never reaches DRAG_CANDIDATE
                // (so it can never reach Stage 4/5 either), and EXTERNAL never
                // touches g_reorderSrcIdx / g_dragFromDockIdx.
                if (g_dragFromDock) {
                    // ---------------- INTERNAL DRAG PATH ----------------
                    // Source is a pinned icon -- already known at PRESS with no
                    // resolver call, so it can transition immediately. No
                    // ResolveDragSourceAtPoint, UI Automation, OpenProcess, or
                    // workspace detection anywhere in this branch or in what it
                    // leads to (DRAG_REORDER, the rope tether, rope-break unpin,
                    // drop-outside-dock unpin -- all keyed off g_dragFromDockIdx,
                    // never off a resolved path).
                    if (ENABLE_REORDER && IsInDockZone(cursor)) {
                        // Cursor stayed inside dock -> enter reorder mode
                        g_reorderSrcIdx    = g_dragFromDockIdx;
                        g_reorderTargetIdx = g_dragFromDockIdx;
                        g_dragState        = DRAG_REORDER;
                        if (g_ghostWnd) ShowWindow(g_ghostWnd, SW_HIDE);
                        DragTraceLog(L"REORDER: started", g_draggedAppPath.c_str());
                    } else {
                        g_lockedDragPath = g_draggedAppPath;  // Hard lock  --  never re-resolved
                        g_lockedDragPinType = g_draggedPinType;
                        g_lockedExplorerHwnd = g_draggedExplorerHwnd;
                        g_dragState      = DRAG_DRAGGING;
                        DragTraceLog(L"DRAG CONFIRMED", g_draggedAppPath.c_str());
                    }
                } else {
                    // ---------------- EXTERNAL DRAG PATH ----------------
                    // Stage 3 gate: a real drag exists, but dock intent has not
                    // been shown yet. Enter the candidate state -- checked every
                    // frame below -- WITHOUT calling the resolver here. Nothing
                    // expensive has run for this drag yet. DRAG_CANDIDATE is
                    // ONLY ever entered from this else-branch, i.e. only when
                    // g_dragFromDock is false -- an internal (dock-icon) drag
                    // can structurally never reach Stage 4 or Stage 5.
                    g_dragState = DRAG_CANDIDATE;
                    DragTraceLog(L"CANDIDATE: real drag confirmed, awaiting dock intent");
                }
            }
        }

        // ================================================================
        // EXTERNAL DRAG PATH ONLY -- DRAG_CANDIDATE (Stages 3 to 5)
        // ================================================================
        // Reachable only from the g_dragFromDock == false branch above, so a
        // dock-icon (internal) drag never executes any code in this block --
        // reorder/unpin never resolves, probes UIA, opens a process, or
        // detects workspaces. A real (non-dock-source) drag is in
        // progress but has not yet shown dock intent, so still nothing
        // expensive has run. Every frame we only do a cheap geometry check
        // (IsNearDockZone / IsCursorOverTaskbar); the resolver, UI Automation,
        // OpenProcess, and icon extraction stay off until that check actually
        // passes. A drag that never approaches the dock/taskbar -- e.g. moving
        // a window across the desktop or to another monitor -- costs nothing
        // beyond this rect test for its entire lifetime and is never resolved.
        if (g_dragState == DRAG_CANDIDATE) {
            if (!lDown) {
                // Released without ever showing dock intent: a normal drag
                // elsewhere. Never resolved, never treated as a click either
                // (the real-drag threshold was already crossed).
                g_dragState = DRAG_IDLE;
                g_dragCandidateWindow = NULL;
                DragTraceLog(L"CANDIDATE: released without dock intent -- ignored");
            } else if (IsNearDockZone(cursor) || IsCursorOverTaskbar(cursor) || EwpPtInAnyWindow(cursor)) {
                // Stage 4 (Source Validation): before paying for Stage 5, check
                // whether the recorded candidate is even potentially pinnable.
                // A closed/gone window, or the desktop/shell surfaces caught by
                // IsSystemWindow, can never be pinned, so reject here and never
                // call the resolver.
                HWND candidateWnd = g_dragCandidateWindow;
                bool pinnableCandidate = candidateWnd && IsWindow(candidateWnd) &&
                                         !IsSystemWindow(candidateWnd);
                if (!pinnableCandidate) {
                    g_dragState = DRAG_CANCELLED;
                    DragTraceLog(L"REJECT: source not pinnable (Stage 4)");
                } else {
                    // Stage 5 (Expensive Resolution): reached exactly once per
                    // genuine drag-to-pin -- only after a real drag, dock
                    // intent, AND a pinnable candidate have all been confirmed.
                    //
                    // Resolve at the RECORDED START POINT (g_dragStartPt), not
                    // the current cursor position. By the time dock intent
                    // fires, the cursor is over the dock/taskbar -- resolving
                    // "cursor" there identifies whatever taskbar button or
                    // window happens to be under it NOW (e.g. Chrome's taskbar
                    // icon), not what the user actually grabbed at mouse-down
                    // (e.g. an Explorer window). The source is the window/app
                    // the drag STARTED from, so it must be resolved from where
                    // the drag started.
                    POINT resolvePt = g_dragStartPt;
                    std::wstring path = ResolveDragSourceAtPoint(resolvePt);
                    DragTraceLog(L"STAGE5: resolve", path.c_str());

                    // FIX (dock froze while the workspace feature was ON): only run
                    // the expensive IShellWindows enumerate + UI Automation probe
                    // when the cheap resolve already says the pressed window is
                    // explorer.exe. (Gate mirrors ShouldConfirmExplorerWorkspace in
                    // tests/workspace_press_gate.h.)
                    // FIX ("[DRAG] REJECT: no valid source" when pinning Explorer):
                    // ResolveDragSourceAtPoint intentionally returns "" for
                    // explorer.exe, so the gate also keys on whether the window
                    // under the cursor is owned by explorer.exe -- a cheap
                    // GetProcessPath check, no COM scan -- so an empty resolve
                    // over a real Explorer window still probes while desktop/
                    // other-app presses do not.
                    // (Gate mirrors ShouldConfirmExplorerWorkspacePress in
                    //  tests/workspace_press_gate.h.)
                    HWND workspaceHwnd = NULL;
                    bool workspaceSource = false;
                    if (ENABLE_EXPLORER_WORKSPACE_PINS) {
                        bool pressedWindowIsExplorer =
                            IsExplorerExePath(path) ||
                            IsExplorerExePath(GetProcessPath(candidateWnd));
                        // Same fix as above: probe the ORIGINAL press point, not
                        // the current (near-dock) cursor position, so the
                        // workspace check identifies the Explorer window/tab the
                        // drag actually started from.
                        workspaceSource =
                            pressedWindowIsExplorer &&
                            IsExplorerWorkspaceDragSource(resolvePt, &workspaceHwnd);
                    }
                    if (workspaceSource) {
                        path = L"Explorer workspace";
                        g_draggedPinType = PIN_WORKSPACE;
                        g_draggedExplorerHwnd = workspaceHwnd;
                        DragTraceLog(L"STAGE5: Explorer workspace");
                    } else {
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
                        // After successful resolution: dock animations, hover
                        // effects, and preview visuals start via the normal
                        // DRAG_DRAGGING rendering path below.
                        g_lockedDragPath = g_draggedAppPath;
                        g_lockedDragPinType = g_draggedPinType;
                        g_lockedExplorerHwnd = g_draggedExplorerHwnd;
                        g_dragState = DRAG_DRAGGING;
                        DragTraceLog(L"DRAG CONFIRMED (dock intent)", g_draggedAppPath.c_str());
                    }
                }
                g_dragCandidateWindow = NULL;
            }
            // else: real drag continues with no dock intent yet -- do nothing.
        }

        // DRAG_REORDER  --  update target slot and animate icons while button held
        if (g_dragState == DRAG_REORDER && lDown) {
            // FIX (#5): g_pinnedApps.size() and CalculateReorderSlot() (which
            // indexes g_pinnedApps and calls GetIconRectLocal) were read on this
            // worker thread WITHOUT g_cs. A Ctrl+Alt+P PinApp on the UI thread can
            // push_back concurrently and reallocate the vector mid-read -> torn
            // size / use-after-free. Take the lock for the read + slot computation,
            // then release it before UpdateReorderPositions() (which re-acquires
            // g_cs itself). n stays a consistent snapshot for the bounds clamp.
            int newTarget = -1;
            bool haveTarget = false;
            if (g_csInitialized) {
                EnterCriticalSection(&g_cs);
                int n = (int)g_pinnedApps.size();
                if (n > 1 && g_reorderSrcIdx >= 0 && g_reorderSrcIdx < n) {
                    newTarget = CalculateReorderSlot(cursor, n);
                    newTarget = std::max(0, std::min(newTarget, n - 1));
                    haveTarget = true;
                }
                LeaveCriticalSection(&g_cs);
            }
            if (haveTarget && newTarget != g_reorderTargetIdx) {
                g_reorderTargetIdx = newTarget;
                UpdateReorderPositions();
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
            if (!IsInDockZone(cursor, 14)) {
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
            int   dx       = abs(cursor.x - g_dragStartPt.x);
            int   dy       = abs(cursor.y - g_dragStartPt.y);
            bool  wasClick = (dx <= CLICK_MAX_MOVE_PX && dy <= CLICK_MAX_MOVE_PX);

            g_dragState = DRAG_IDLE;
            if (wasClick) {
                int  clickIdx    = HitTestIcon(cursor);
                bool inDockZone  = IsInDockZone(cursor, 0);
                bool unpinnedAll = false;
                if (ENABLE_RAPID_UNPIN_ALL && inDockZone) {
                    if (g_rapidClickCount == 0 || (now - g_rapidClickStart) > (DWORD)RAPID_CLICK_WINDOW_MS) {
                        g_rapidClickStart = now;
                        g_rapidClickCount = 1;
                    } else {
                        g_rapidClickCount++;
                    }
                    if (g_rapidClickCount >= RAPID_CLICK_THRESHOLD) {
                        g_rapidClickCount = 0;
                        g_rapidClickStart = 0;
                        g_rapidClickIndex = -1;
                        unpinnedAll = true;
                        if (g_iconsLocked) {
                            TriggerLockGlow();
                        } else {
                            UnpinAllApps();
                            TriggerLockGlowKind(LOCKGLOW_RELEASE);
                        }
                    }
                }
                if (!unpinnedAll && clickIdx >= 0) {
                    if (pendingDockClickIdx == clickIdx && (now - pendingDockClickTick) <= 260) {
                        pendingDockClickIdx  = -1;
                        pendingDockClickTick = 0;
                        if (g_iconsLocked) {
                            TriggerLockGlow();
                        } else {
                            UnpinAppByIndex(clickIdx);
                            TriggerLockGlowKind(LOCKGLOW_RELEASE);
                            DragTraceLog(L"DBLCLK: dock icon -> UNPIN");
                        }
                    } else {
                        pendingDockClickIdx  = clickIdx;
                        pendingDockClickTick = now;
                    }
                }
            }
            GhostCleanup();
        }

        // Fire single-click SmartLaunch once the double-click window (230 ms) elapses
        if (pendingDockClickIdx >= 0 && g_dragState == DRAG_IDLE && !lDown &&
            (now - pendingDockClickTick) > 230) {
            int launchIdx = pendingDockClickIdx;
            pendingDockClickIdx  = -1;
            pendingDockClickTick = 0;
            SmartLaunch(launchIdx);
        }

        // Right-click on a Left Taskbar Dock icon -> unpin/delete that icon directly
        if (rDown && !lastRDown && !clickInsideWsPopup && g_dragState == DRAG_IDLE) {
            rPressDockIdx = HitTestIcon(cursor);
            rPressDockPt  = cursor;
        }
        if (!rDown && lastRDown && rPressDockIdx >= 0) {
            int rRelIdx = HitTestIcon(cursor);
            if (rRelIdx == rPressDockIdx &&
                abs(cursor.x - rPressDockPt.x) <= CLICK_MAX_MOVE_PX &&
                abs(cursor.y - rPressDockPt.y) <= CLICK_MAX_MOVE_PX) {
                pendingDockClickIdx  = -1;
                pendingDockClickTick = 0;
                if (g_iconsLocked) {
                    TriggerLockGlow();
                } else {
                    UnpinAppByIndex(rRelIdx);
                    TriggerLockGlowKind(LOCKGLOW_RELEASE);
                    DragTraceLog(L"RCLICK: dock icon -> UNPIN");
                }
            }
            rPressDockIdx = -1;
        }

        // DRAG_CANCELLED auto-reset when mouse is released
        if (g_dragState == DRAG_CANCELLED && !lDown) {
            g_dragState = DRAG_IDLE;
            g_draggedAppPath.clear();
            GhostCleanup();
        }

        // Workspace drop-up hover/hold/dismiss state machine
        if (g_overlayWnd && IsWindow(g_overlayWnd)) {
            int wsState = g_wsPopupState.load();
            if (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0) {
                if (wsState != WS_POPUP_CLOSED)
                    PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 1, 0);
            } else {
                // Outside-click dismiss must run independently of g_dragState == DRAG_IDLE,
                // because lDown && !lastLDown transitions g_dragState to DRAG_PRESS earlier in this frame.
                if (wsState == WS_POPUP_OPEN_HOLD || wsState == WS_POPUP_OPEN_HOVER) {
                    bool freshClick = (lDown && !lastLDown) || (rDown && !lastRDown);
                    if (freshClick && !PtInWsPopupRectSafe(cursor)) {
                        PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 0, 0);
                    }
                }
            }
            if (!(g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0) && g_dragState == DRAG_IDLE) {
                int hi = g_hoverIndex.load();
                bool onGroupedWs = false;
                if (hi >= 0 && g_csInitialized) {
                    EnterCriticalSection(&g_cs);
                    if (hi < (int)g_pinnedApps.size() &&
                        g_pinnedApps[hi].type == PIN_WORKSPACE &&
                        g_pinnedApps[hi].workspacePaths.size() > 1) {
                        onGroupedWs = true;
                    }
                    LeaveCriticalSection(&g_cs);
                }
                if (onGroupedWs) {
                    if (g_wsPopupHoverPin != hi) {
                        g_wsPopupHoverPin   = hi;
                        g_wsPopupHoverSince = GetTickCount();
                    } else if (GetTickCount() - g_wsPopupHoverSince >= 120) {
                        if (wsState == WS_POPUP_CLOSED || g_wsPopupPinIdx.load() != hi)
                            PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_OPEN, (WPARAM)hi, 0);
                    }
                    g_wsPopupLingerUntil = GetTickCount() + 200;
                } else {
                    g_wsPopupHoverPin = -1;
                    if (wsState == WS_POPUP_OPEN_HOVER) {
                        if (PtInWsPopupRectSafe(cursor)) {
                            g_wsPopupLingerUntil = GetTickCount() + 200;
                        } else if (GetTickCount() > g_wsPopupLingerUntil) {
                            PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_CLOSE, 0, 0);
                        }
                    }
                }
            }
        }

        // DRAGGING  --  update ghost, nothing else may interrupt this state
        if (g_dragState == DRAG_DRAGGING) {
            if (!g_dragFromDock && g_draggedPinType == PIN_WORKSPACE && g_overlayWnd) {
                int appendTarget = HitTestWorkspaceAppendTarget(cursor);
                if (appendTarget >= 0) {
                    PostMessageW(g_overlayWnd, WM_QPD_WSPOPUP_OPEN, (WPARAM)appendTarget, 0);
                    g_wsPopupLingerUntil = GetTickCount() + 250;
                }
            }
            // Drag-to-PIN feedback glow. ONLY for a PIN drag (a NEW app dragged in,
            // g_dragFromDock == false); an already-pinned icon dragged off to unpin
            // / reorder / break its rope must NOT glow. While the icon is HELD in the
            // drop zone it shows a SUSTAINED bloom -- GREEN when the dock has room,
            // RED the moment it enters a FULL dock (pin limit), decided up front so
            // it is never a green-then-red flash. It clears when the icon leaves the
            // zone (here) or on release / drop (the drag leaves DRAG_DRAGGING and the
            // glow block below tears it down). DragGlowFor is unit-tested.
            bool nowInDropZone = IsNearDockZone(cursor);
            bool dockFull      = false;
            bool alreadyPinned = false;
            int preAppendTarget = (g_draggedPinType == PIN_WORKSPACE)
                                ? HitTestWorkspaceAppendTarget(cursor)
                                : -1;
            if (g_csInitialized) {
                EnterCriticalSection(&g_cs);   // CountPinsByType / IsPinned read g_pinnedApps
                if (g_draggedPinType == PIN_WORKSPACE) {
                    dockFull = (preAppendTarget < 0) &&
                               (CountPinsByType(PIN_WORKSPACE) >= MAX_WORKSPACE_PINS.load());
                } else {
                    dockFull = CountPinsByType(PIN_APP) >= std::max(1, MAX_PINNED_APPS);
                }
                // Re-dropping an app that is already pinned is a no-op (PinApp
                // dedups before the cap check), so an EXTERNAL drag of such an
                // app must stay calm -- no RED "dock full", no GREEN bloom -- in
                // BOTH limit states. (Dock-icon reorder/unpin is handled by the
                // g_dragFromDock term feeding IsNewPinFeedbackDrag below.)
                if (!g_dragFromDock && !g_draggedAppPath.empty()) {
                    alreadyPinned = (g_draggedPinType == PIN_WORKSPACE)
                                  ? IsWorkspacePinned(g_draggedAppPath)
                                  : IsPinned(g_draggedAppPath);
                }
                LeaveCriticalSection(&g_cs);
            }
            DragGlow dg = DragGlowFor(/*pinningDrag=*/IsNewPinFeedbackDrag(g_dragFromDock, alreadyPinned), nowInDropZone, dockFull);
            if (dg == DRAGGLOW_NONE) {
                if (g_dragGlowHold) {
                    g_dragGlowHold  = false;
                    g_lockGlowStart = 0;
                    if (g_overlayWnd) PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
                }
            } else {
                LockGlowKind k = (dg == DRAGGLOW_RED) ? LOCKGLOW_LIMIT : LOCKGLOW_RELEASE;
                if (!g_dragGlowHold || g_lockGlowKind != k) {   // (re)arm on zone entry or colour change
                    g_lockGlowKind  = k;
                    g_lockGlowMode  = LockGlowModeFromSetting(ENABLE_LOCK_ANIMATION);
                    g_dragGlowHold  = true;
                    g_lockGlowStart = GetTickCount();
                    if (g_lockGlowStart == 0) g_lockGlowStart = 1;
                }
            }
            g_dropZoneActive = nowInDropZone;
            // Present the rope FIRST, then the ghost icon. Both re-assert
            // HWND_TOPMOST every present, so the one presented LAST ends up on
            // top of the top-most band. Drawing the icon last keeps it cleanly
            // above the rope tip (the rope appears to attach behind the icon)
            // while both remain above every other window.
            UpdateGhostWindow(cursor);
            if (g_dragFromDock && g_overlayWnd)
                InvalidateRect(g_overlayWnd, NULL, FALSE);
        }

        // DRAGGING -> DROPPED / CANCELLED (mouse up)
        if ((g_dragState == DRAG_DRAGGING || g_dragState == DRAG_CANCELLED) &&
            !lDown && lastLDown) {
            if (g_dragState == DRAG_DRAGGING) {
                RECT dropZone = g_cachedDockRect;
                InflateRect(&dropZone, 8, 8);
                bool dropped = PtInRect(&dropZone, cursor) != 0;

                if (g_dragFromDock) {
                    int lockedIdx = g_dragFromDockIdx;
                    g_dragState   = DRAG_DROPPED;
                    if (!dropped) {
                        DragTraceLog(L"DROP: off dock -> UNPIN");
                        UnpinAppByIndex(lockedIdx);
                        TriggerLockGlowKind(LOCKGLOW_RELEASE);
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
                    int appendTarget = -1;
                    if (dropType == PIN_WORKSPACE) {
                        appendTarget = HitTestWorkspaceAppendTarget(cursor);
                        if (appendTarget < 0 && g_wsPopupState.load() != WS_POPUP_CLOSED &&
                            PtInWsPopupRectSafe(cursor)) {
                            appendTarget = g_wsPopupPinIdx.load();
                        }
                    }
                    if (appendTarget >= 0 && dropType == PIN_WORKSPACE) {
                        WorkspaceSnapshot snap;
                        bool allTabsInWin = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
                        if (explorerHwnd && CaptureWorkspaceSnapshot(snap, explorerHwnd, allTabsInWin) &&
                            !snap.folderPaths.empty()) {
                            AppendFoldersToWorkspace(appendTarget, snap.folderPaths);
                        }
                    } else if (dropped && dropType == PIN_WORKSPACE) {
                        DragTraceLog(L"DROP: in zone -> PIN WORKSPACE");
                        PinWorkspace(explorerHwnd);
                    } else if (dropped && !dropPath.empty()) {
                        DragTraceLog(L"DROP: in zone -> PIN", dropPath.c_str());
                        PinApp(dropPath);
                    } else if (!dropPath.empty() && dropType == PIN_APP && EwpPtInAnyWindow(cursor) &&
                               g_ewpPillWnd && IsWindow(g_ewpPillWnd)) {
                        int targetWs = -1;
                        if (g_ewpPreviewWnd && IsWindowVisible(g_ewpPreviewWnd)) {
                            RECT prc = {};
                            if (GetWindowRect(g_ewpPreviewWnd, &prc) && PtInRect(&prc, cursor))
                                targetWs = g_ewpPrevActiveWsIdx;
                        }
                        if (targetWs < 0 && g_ewpDockWnd && IsWindowVisible(g_ewpDockWnd)) {
                            RECT drc = {};
                            if (GetWindowRect(g_ewpDockWnd, &drc) && PtInRect(&drc, cursor)) {
                                POINT localPt = { cursor.x - drc.left, cursor.y - drc.top };
                                int hitWs = -1, hitSub = -1;
                                bool hitAdd = false;
                                EwpHitTestArcDock(localPt, &hitWs, &hitSub, &hitAdd);
                                if (hitWs >= 0 && !hitAdd) targetWs = hitWs;
                            }
                        }
                        DragTraceLog(L"DROP: on Edge Pill/Preview -> PIN/ADD", dropPath.c_str());
                        PostMessageW(g_ewpPillWnd, WM_EWP_DROP_APP_PATH_FWD, (WPARAM)targetWs,
                                     (LPARAM)(new std::wstring(dropPath)));
                    } else {
                        DragTraceLog(L"DROP: outside zone -> cancel");
                    }
                }
            }

            ShowWindow(g_ghostWnd, SW_HIDE);
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

                // Hover magnification & neighbour spread disabled (crisp fixed ICON_SIZE).
                app.hoverScale  = 1.0f;
                app.hoverShiftX = 0.f;

                // Smooth QPC-driven color crossfade (~260 ms)
                if (app.prevCachedBmp && app.colorAnimProgress < 1.0f) {
                    app.colorAnimProgress += g_frameDeltaMs / 260.0f;
                    if (app.colorAnimProgress >= 1.0f) {
                        app.colorAnimProgress = 1.0f;
                        delete app.prevCachedBmp;
                        app.prevCachedBmp = nullptr;
                    } else {
                        animActive = true;
                    }
                }
            }

            LeaveCriticalSection(&g_cs);
        }

        g_anyAnimationActive = animActive;
        if (animActive && localOverlay && IsWindow(localOverlay) && !g_renderQueued.exchange(true))
            PostMessageW(localOverlay, WM_QPD_RENDER_DOCK, 0, 0);

        // (Limit-flash expiry removed: the RED "dock full" glow self-expires via
        // LockGlowShouldTeardown, exactly like every other lock-glow flash.)

        // A HELD drag-to-pin glow lives only for the duration of the drag: the
        // instant the drag leaves DRAG_DRAGGING (dropped / pinned / released /
        // cancelled / rope-break) stop holding it and hide the layer.
        if (g_dragGlowHold && g_dragState != DRAG_DRAGGING) {
            g_dragGlowHold  = false;
            g_lockGlowStart = 0;
            if (g_overlayWnd) PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
        }

        if (g_lockGlowStart != 0) {
            bool dockHidden = DockHiddenForGlow(g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0, g_layoutUnsupported, g_dockLocalW);
            bool teardown = dockHidden ||
                            (!g_dragGlowHold && LockGlowShouldTeardown(true, false, now - g_lockGlowStart, (DWORD)LOCK_GLOW_MS));
            if (teardown) {
                g_dragGlowHold  = false;        // stop any held / animating lock-glow
                g_lockGlowStart = 0;
                if (g_overlayWnd) PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
            } else {
                static DWORD s_lastLockPulse = 0;
                if (now - s_lastLockPulse >= 16) {
                    s_lastLockPulse = now;
                    if (g_overlayWnd) PostMessageW(g_overlayWnd, WM_QPD_RENDER_DOCK, 0, 0);
                }
            }
        }

        // Dock position smooth glide
        // Dock position smooth glide
        if (AnimateDockPositionStep()) {
            RepositionOverlay();
            g_anyAnimationActive = true;
        }

        lastLDown = lDown;
        lastRDown = rDown;

        // ================================================================
        //  ADAPTIVE SLEEP
        //  8 ms   --  dragging or any animation active
        //  16 ms  --  brief post-active transition
        //  50 ms  --  fully idle (saves CPU)
        // ================================================================
        // NOTE: sleep on g_exitEvent instead of Sleep() so teardown is instant.
        // A plain Sleep(50) here delayed WhTool_ModUninit's worker-join by up to a
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
        // DRAG_PRESS is included deliberately: while the button is down but the
        // move threshold has not yet been crossed, a quick flick to tear an icon
        // off must be sampled at the fast cadence -- otherwise the loop sat at
        // the 16/50 ms idle cadence and the gesture ended (button up) before it
        // ever ticked in PRESS, so the drag was missed and read as a click.
        if (g_dragState == DRAG_DRAGGING || g_dragState == DRAG_REORDER ||
            g_dragState == DRAG_PRESS || g_dragState == DRAG_CANDIDATE ||
            g_anyAnimationActive || g_dockPosAnimActive ||
            g_lockGlowStart != 0) {   // keep the fast cadence smooth during a lock/unlock flash
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

    // v2.5.4 (this-round Issue 1): the lock-glow window is no longer created OR
    // destroyed on the worker. It is now a UI-thread-owned layer (prewarmed in
    // PrewarmDragEffectWindows, torn down in UiThreadProc next to the tether/vanish
    // windows); the worker only PRESENTS into it. Its DIB is freed in
    // WhTool_ModUninit. Keeping the window on a message-pumping thread is what stops
    // a broadcast SendMessage / WM_SETTINGCHANGE from hanging on this dock.

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
        Wh_Log(L"TASKBAR: auto-hide %s", nowHide ? L"ON" : L"OFF");
        RepositionOverlay();  // immediately apply show/hide decision
    }
}

static void UpdateRunningState() {
    // 1) Enumerate candidate windows -> unique PIDs (no per-window OpenProcess).
    // MEM: reuse the scan buffers across cycles. UpdateRunningState is called
    // only from the single WorkerThread, so a function-static buffer is safe and
    // avoids re-allocating this vector's backing store on every 500 ms poll
    // (clear() keeps the capacity; the elements are trivially destroyed).
    static std::vector<DWORD> pids;
    pids.clear();
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
    // MEM: same worker-thread-only reuse as `pids` above -- clear() frees each
    // string's storage but keeps the vector's element array, so we don't churn
    // the heap allocating/freeing this list on every poll cycle.
    static std::vector<std::wstring> runningPaths;
    runningPaths.clear();
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
    // CALLER MUST HOLD g_cs: this indexes g_pinnedApps[i].type and calls
    // GetIconRectLocal (which also reads g_pinnedApps), so a concurrent PinApp
    // push_back on the UI thread could reallocate the vector mid-read. (The old
    // "safe to call outside CS" note was wrong -- it reads more than g_dockCurrentX.)
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
        int curPopIdx = g_wsPopupPinIdx.load();
        if (curPopIdx == src) {
            g_wsPopupPinIdx.store(insertAt);
        } else if (src < insertAt && curPopIdx > src && curPopIdx <= insertAt) {
            g_wsPopupPinIdx.store(curPopIdx - 1);
        } else if (insertAt < src && curPopIdx >= insertAt && curPopIdx < src) {
            g_wsPopupPinIdx.store(curPopIdx + 1);
        }
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
//  MODULE ENTRY POINTS
// ============================================================
// Forward declaration  --  WhTool_ModInit calls this on any failure path to ensure
// every partially-initialised resource (HWND, HICON, HANDLE, CS) is released
// even if Windhawk does not guarantee calling WhTool_ModUninit after a FALSE return.
void WhTool_ModUninit();

// FIX-C1 (#1/#2/#8): the dedicated UI thread. It creates every top-level window
// (input owner, overlay, ghost, the pre-warmed tether/vanish layers),
// registers the pin hotkey (inside CreateOverlayWindow),
// then runs a real GetMessageW/DispatchMessageW loop so WM_PAINT / WM_MOUSE* /
// WM_MOUSEWHEEL / WM_HOTKEY are actually dispatched.
// On WM_QUIT it destroys those windows and unregisters the classes -- on the SAME
// thread that created them, which is what DestroyWindow / UnregisterHotKey /
// UnregisterClassW require. WhTool_ModInit waits on g_uiReadyEvent so the worker only
// starts once the windows exist.

// ============================================================
//  UNIFIED 4-EDGE WORKSPACE PILL, PARTIAL-CIRCLE CENTER-'+' ARC &
//  COUPLED BORDERLESS CUSTOMIZATION THUMBNAIL ENGINE
//  - Left Taskbar Quick Pin Dock stays 100% untouched as shared
//  - Renders on frame 0 (even when registry settings are not yet saved)
//    and automatically restores after Snipping Tool (Win+Shift+S) or fullscreen
//  - 100% Borderless Fill-Only UI (zero outer bezel, zero 'x' buttons, zero Pen strokes)
//  - True Partial-Circle Arc with '+' fixed at center (theta = 0)
//  - Coupled/Nested Arc + Schematic Thumbnail state (open, hold, and close together)
//  - Double-Click Customization Workflow:
//      * Double-click an Arc Card -> opens & holds the Schematic Thumbnail in Customize Mode
//      * Double-click an app block in Schematic Thumbnail (or double-click an Arc Card when held) ->
//        deletes/unpins that item and auto-reflows remaining apps to 100%
//      * Click the '+' badge inside Schematic Thumbnail -> adds the current top visible window
//        into that workspace and auto-reflows split ratios
// ============================================================
static const wchar_t* EWP_PILL_CLASS     = L"QPDockEdgePillWnd";
static const wchar_t* EWP_DOCK_CLASS     = L"QPDockEdgeHoverBarWnd";
static const wchar_t* EWP_PREVIEW_CLASS  = L"QPDockEdgeSchematicWnd";
static const wchar_t* EWP_DROPZONE_CLASS = L"QPDockEdgeDropZoneWnd";

static const UINT WM_EWP_CAPTURE_DONE    = WM_APP + 0x31;
static const UINT WM_EWP_MOVESIZE_STATE  = WM_APP + 0x32;
static const UINT WM_EWP_OBJSHOW_EVENT   = WM_APP + 0x33;
static const UINT WM_EWP_DROP_APP_PATH   = WM_APP + 0x34;

static const UINT_PTR EWP_TIMER_MOTION       = 0x6201;
static const UINT_PTR EWP_TIMER_PROX         = 0x6202;
static const UINT_PTR EWP_TIMER_TOAST        = 0x6203;
static const UINT_PTR EWP_TIMER_RESTORE      = 0x6204;
static const UINT_PTR EWP_TIMER_INIT_RENDER  = 0x6205;
static const wchar_t* const EWP_TOAST_CLASS  = L"WindhawkEdgeWorkspaceToast_Ashix";

static const int HOTKEY_EWP_TRIGGER      = 0x2800;

enum EwpEdge { EWP_EDGE_TOP = 0, EWP_EDGE_BOTTOM = 1, EWP_EDGE_LEFT = 2, EWP_EDGE_RIGHT = 3 };
enum EwpDockState { EWP_DOCK_CLOSED = 0, EWP_DOCK_OPENING, EWP_DOCK_OPEN_HOVER, EWP_DOCK_OPEN_HOLD, EWP_DOCK_CLOSING };
enum EwpTargetKind { EWP_KIND_EXE = 0, EWP_KIND_UWP = 1, EWP_KIND_PWA = 2, EWP_KIND_URL = 3 };
enum EwpDuplicatePolicy { EWP_DUP_IGNORE = 0, EWP_DUP_UPDATE = 1, EWP_DUP_ALLOW = 2 };
enum EwpCaptureFilterMode { EWP_CAP_EDGE_TOUCHING = 0, EWP_CAP_ALL_VISIBLE = 1 };

struct EwpAppLayoutEntry {
    EwpTargetKind kind = EWP_KIND_EXE;
    std::wstring  exePath;
    std::wstring  aumid;
    std::wstring  activeUrl;
    std::wstring  windowClass;
    std::wstring  displayName;
    float         nx = 0.0f;
    float         ny = 0.0f;
    float         nw = 0.5f;
    float         nh = 1.0f;
    bool          isMaximized = false;
};

struct EwpLayoutWorkspace {
    std::wstring                    id;
    std::wstring                    name;
    std::vector<EwpAppLayoutEntry>  apps;
    int                             ratioPresetState = 0;
};

struct EwpPendingRestore {
    EwpAppLayoutEntry entry;
    RECT              targetWorkRect = {};
    DWORD             expireTick = 0;
    bool              matched = false;
};

struct EwpIconCacheEntry {
    std::wstring     key;
    Gdiplus::Bitmap* bmp = nullptr;
};

static const int            HOTKEY_EWP_CAPTURE      = 0x2801;

std::atomic<bool>           g_ewpEnabled{true};
static EwpEdge              g_ewpDefaultEdge        = EWP_EDGE_LEFT;
static bool                 g_ewpRememberPos        = true;
static int                  g_ewpSizePreset         = 2;   // 2=ExtraLarge (135%) by default!
static int                  g_ewpPillThick          = 8;
static int                  g_ewpPillLen            = 96;
static const int            g_ewpPillRoundness      = 100; // Fixed 100% full smooth capsule pill
static int                  g_ewpPillToArcGap       = 10;
static int                  g_ewpArcCardSize        = 48;
static const int            g_ewpArcCardRoundness   = 40;  // Fixed clean rounded-rect card softness
static const int            g_ewpArcCardGap         = 15;  // Fixed 15px spacing along the arc
static const int            g_ewpArcCurveDepth      = 28;  // Fixed partial-circle curve depth
static int                  g_ewpArcVisibleSlots    = 4;
static const int            g_ewpArcToPreviewGap    = 10;  // Fixed safe 10px breathing gap from Arc to Schematic Preview
static const int            g_ewpPreviewWidth       = 260; // Fixed predictable Schematic Preview width (px at 96 DPI)
static const int            g_ewpPreviewHeight      = 156; // Fixed predictable Schematic Preview height (px at 96 DPI)
static const int            g_ewpPreviewRoundness   = 34;  // Fixed predictable Schematic Preview block roundness
static const int            g_ewpPreviewSplitGap    = 8;   // Fixed 8px split gap between inner app blocks
static int                  g_ewpLingerMs           = 280;
static int                  g_ewpProxPx             = 48;
static EwpCaptureFilterMode g_ewpCaptureFilter      = EWP_CAP_EDGE_TOUCHING;
static bool                 g_ewpAllowDuplicates    = false;
static EwpDuplicatePolicy   g_ewpDuplicatePolicy    = EWP_DUP_IGNORE;
static int                  g_ewpMaxApps            = 8;
static bool                 g_ewpIgnoreOccluded     = true;
static bool                 g_ewpCaptureUrl         = true;
static bool                 g_ewpReuseOpen          = true;
static std::vector<std::wstring> g_ewpUserExcluded;
static std::wstring         g_ewpToggleHk           = L"Alt+W";
static std::wstring         g_ewpCaptureHk          = L"Alt+Shift+W";

static EwpEdge              g_ewpCurEdge     = EWP_EDGE_LEFT;
static float                g_ewpOffsetRatio = 0.5f;
static HMONITOR             g_ewpMonitor     = NULL;
static std::vector<EwpLayoutWorkspace> g_ewpWorkspaces;
static std::vector<EwpIconCacheEntry>  g_ewpIconCache;

HWND                        g_ewpPillWnd     = NULL;
HWND                        g_ewpDockWnd     = NULL;
HWND                        g_ewpPreviewWnd  = NULL;
HWND                        g_ewpDropZoneWnd = NULL;
static HWINEVENTHOOK        g_ewpMoveHook    = NULL;
static HWINEVENTHOOK        g_ewpShowHook    = NULL;
static IDropTarget*         g_ewpDropTarget  = NULL;

// Coupled/Nested QPC animation state shared by BOTH the Arc Carousel and Schematic Thumbnail
static EwpDockState         g_ewpDockState    = EWP_DOCK_CLOSED;
static LONGLONG             g_ewpAnimStartQpc = 0;
static float                g_ewpAnimProg     = 0.0f;
static ULONGLONG            g_ewpLingerUntil  = 0;

// Schematic Thumbnail state:
// - g_ewpPreviewOpen: true whenever Thumbnail Preview is visible (either Read-Only on hover or Customize Mode on right-click)
// - g_ewpPreviewEditMode: false = Read-Only hover preview; true = Locked Customize Mode (single/double right-click)
static bool                 g_ewpPreviewOpen      = false;
static bool                 g_ewpPreviewEditMode  = false;
int                         g_ewpPrevActiveWsIdx  = -1;
static int                  g_ewpPrevHoverAppIdx  = -1;
static int                  g_ewpPrevHoverDelApp  = -1;
static bool                 g_ewpPrevHoverAddApp  = false;
static int                  g_ewpPrevSelectedApp  = -1;
static int                  g_ewpPrevDragAppIdx   = -1;
static POINT                g_ewpPrevPressPt      = {};
static bool                 g_ewpPrevDragging     = false;
static int                  g_ewpPrevRPressApp    = -1;
static DWORD                g_ewpPrevLastDblTick  = 0;
static int                  g_ewpPrevRenderDelApp = -2;
static bool                 g_ewpPrevRenderEditMode = false;
static uint64_t             g_ewpDockGeneration   = 1;
static HWND                 g_ewpDraggedHwnd      = NULL;

static bool                 g_ewpPillPress   = false;
static bool                 g_ewpPillDrag    = false;
static POINT                g_ewpPillPressPt = {};
static POINT                g_ewpPillAnchor  = {};
static RECT                 g_ewpPillDragRc  = {};

HWND                        g_ewpToastWnd       = NULL;
static std::wstring         g_ewpToastText;
static LONGLONG             g_ewpToastStartQpc  = 0;

static int                  g_ewpScrollIdx      = 0;  // Wheel scroll offset around the circular arc
static int                  g_ewpFocusIdx       = 0;  // Active workspace shown in coupled Schematic Thumbnail
static int                  g_ewpHoverSlot      = -1; // Hovered slot in the Arc
static int                  g_ewpHoverWsIdx     = -1; // Hovered workspace index (-1 if none)
static int                  g_ewpHoverSubApp    = -1; // Hovered sub-icon inside an Arc Card
static bool                 g_ewpHoverAdd       = false;
static bool                 g_ewpHoverArcDel    = false; // Hovered top-right 'x' badge on an Arc Card
static int                  g_ewpPressWsIdx     = -1;
static bool                 g_ewpPressIsAdd     = false;
static bool                 g_ewpPressOnCardDel = false;
static POINT                g_ewpPressPt        = {};
static int                  g_ewpRPressWsIdx    = -1;

// Persistent DIB buffer for flicker-free Schematic Thumbnail rendering
static int                  g_ewpPrevChipIdx        = -1;
static int                  g_ewpPrevRenderHoverApp = -2;
static bool                 g_ewpPrevRenderHoverAdd = false;
static int                  g_ewpPrevRenderSelApp   = -2;
static HBITMAP              g_ewpPrevDIB            = NULL;
static HDC                  g_ewpPrevDC             = NULL;
static HBITMAP              g_ewpPrevOldBmp         = NULL;
static void*                g_ewpPrevBits           = NULL;
static int                  g_ewpPrevW              = 0;
static int                  g_ewpPrevH              = 0;

static std::atomic<bool>    g_ewpMoveSizeActive{false};
static bool                 g_ewpOleActive      = false;
static bool                 g_ewpDropVis        = false;
static std::vector<EwpPendingRestore> g_ewpPendingRestores;
static HANDLE               g_ewpCapThread      = NULL;
static std::atomic<bool>    g_ewpCapBusy{false};
static int                  g_ewpAppendTargetWs = -1; // >=0 when adding an app into an existing workspace from Thumbnail '+'

static const wchar_t* const kEwpCriticalExclusions[] = {
    L"SystemSettings.exe", L"Taskmgr.exe", L"windhawk.exe", L"windhawk-mod.exe",
    L"windhawk-mod-uiaccess.exe", L"windhawk-mod-elevated.exe", L"explorer.exe",
    L"SearchHost.exe", L"StartMenuExperienceHost.exe", L"ShellExperienceHost.exe",
    L"ScreenClippingHost.exe", L"SnippingTool.exe", L"LockApp.exe", L"LogonUI.exe",
    L"TextInputHost.exe", NULL
};

static const wchar_t* const kEwpBrowsers[] = {
    L"chrome.exe", L"msedge.exe", L"brave.exe", L"vivaldi.exe", L"firefox.exe", L"opera.exe", NULL
};

static float EwpClamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
static float EwpSmoothStep(float t) { t = EwpClamp01(t); return t * t * (3.0f - 2.0f * t); }

bool EwpPtInAnyWindow(POINT pt) {
    HWND wnds[4] = { g_ewpPillWnd, g_ewpDockWnd, g_ewpPreviewWnd, g_ewpDropZoneWnd };
    for (HWND w : wnds) {
        if (w && IsWindowVisible(w)) {
            RECT rc = {};
            if (GetWindowRect(w, &rc) && PtInRect(&rc, pt)) return true;
        }
    }
    return false;
}

static BOOL CALLBACK EwpPurgeDuplicateEnumProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) return TRUE;
    wchar_t cls[128] = {};
    if (!GetClassNameW(hwnd, cls, 127)) return TRUE;
    if (wcscmp(cls, EWP_PILL_CLASS) == 0 ||
        wcscmp(cls, EWP_DOCK_CLASS) == 0 ||
        wcscmp(cls, EWP_PREVIEW_CLASS) == 0 ||
        wcscmp(cls, EWP_DROPZONE_CLASS) == 0 ||
        wcsncmp(cls, L"EdgeWorkspacePin", 16) == 0 ||
        wcsncmp(cls, L"WindhawkEdgePill", 16) == 0) {
        ShowWindow(hwnd, SW_HIDE);
        PostMessageW(hwnd, WM_CLOSE, 0, 0);
    }
    return TRUE;
}

static void EwpPurgeDuplicatePillWindows() {
    EnumWindows(EwpPurgeDuplicateEnumProc, 0);
}

static float EwpDpiScale(HWND hwnd) {
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 96u;
    if (dpi == 0) dpi = 96u;
    float mult = (g_ewpSizePreset == 1) ? 1.18f : (g_ewpSizePreset == 2 ? 1.35f : 1.0f);
    return (dpi / 96.0f) * mult;
}

static std::wstring EwpTrim(const std::wstring& s) {
    size_t b = 0, e = s.size();
    while (b < e && iswspace(s[b])) ++b;
    while (e > b && iswspace(s[e - 1])) --e;
    return s.substr(b, e - b);
}

static std::wstring EwpLower(std::wstring s) {
    for (wchar_t& c : s) c = (wchar_t)towlower(c);
    return s;
}

static bool EwpWorkspaceHasApp(const EwpLayoutWorkspace& ws,
                               const std::wstring& exePath,
                               const std::wstring& aumid = L"") {
    for (const auto& existing : ws.apps) {
        if (!exePath.empty() && !existing.exePath.empty() &&
            ExePathsMatchOrSameApp(existing.exePath, exePath)) {
            return true;
        }
        if (!aumid.empty() && !existing.aumid.empty() &&
            _wcsicmp(existing.aumid.c_str(), aumid.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

static std::wstring EwpMakeWorkspaceSignature(const EwpLayoutWorkspace& ws) {
    std::vector<std::wstring> tokens;
    tokens.reserve(ws.apps.size());
    for (const auto& a : ws.apps) {
        std::wstring id;
        if (!a.exePath.empty()) {
            const wchar_t* leaf = PathFindFileNameW(a.exePath.c_str());
            id = EwpLower(leaf && leaf[0] ? leaf : a.exePath.c_str());
        } else if (!a.aumid.empty()) {
            id = EwpLower(a.aumid);
        }
        if (!id.empty()) tokens.push_back(std::move(id));
    }
    std::sort(tokens.begin(), tokens.end());
    std::wstring sig;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) sig += L"|";
        sig += tokens[i];
    }
    return sig;
}

static void EwpClearIconCache() {
    for (auto& e : g_ewpIconCache) {
        if (e.bmp) { delete e.bmp; e.bmp = nullptr; }
    }
    g_ewpIconCache.clear();
}

static Gdiplus::Bitmap* EwpGetCachedAppIcon(const std::wstring& exePath) {
    if (exePath.empty()) return nullptr;
    std::wstring key = EwpLower(exePath);
    for (const auto& e : g_ewpIconCache) {
        if (e.key == key) return e.bmp;
    }
    HICON hIcon = LoadAppIconStrict(exePath);
    Gdiplus::Bitmap* bmp = hIcon ? BuildGdiPlusBitmapFromIcon(hIcon) : nullptr;
    if (hIcon) DestroyIcon(hIcon);
    g_ewpIconCache.push_back({ key, bmp });
    return bmp;
}

static bool EwpIsExcludedProcess(const std::wstring& path) {
    if (path.empty()) return true;
    const wchar_t* leaf = PathFindFileNameW(path.c_str());
    if (!leaf || !leaf[0]) return true;
    for (int i = 0; kEwpCriticalExclusions[i]; ++i) {
        if (_wcsicmp(leaf, kEwpCriticalExclusions[i]) == 0) return true;
    }
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    for (const auto& u : g_ewpUserExcluded) {
        if (_wcsicmp(leaf, u.c_str()) == 0) {
            if (g_csInitialized) LeaveCriticalSection(&g_cs);
            return true;
        }
    }
    if (g_csInitialized) LeaveCriticalSection(&g_cs);
    return false;
}

static bool EwpIsBrowserExe(const std::wstring& path) {
    const wchar_t* leaf = PathFindFileNameW(path.c_str());
    if (!leaf) return false;
    for (int i = 0; kEwpBrowsers[i]; ++i) {
        if (_wcsicmp(leaf, kEwpBrowsers[i]) == 0) return true;
    }
    return false;
}

static std::wstring EwpQueryAumid(HWND hwnd) {
    if (!hwnd) return L"";
    IPropertyStore* store = NULL;
    if (FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(&store))) || !store) return L"";
    PROPERTYKEY pkey = {};
    CLSIDFromString(L"{9F4C2855-9F79-4B39-A8D0-E1D42DE1D5F3}", &pkey.fmtid);
    pkey.pid = 5;
    PROPVARIANT pv;
    PropVariantInit(&pv);
    std::wstring out;
    if (SUCCEEDED(store->GetValue(pkey, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal)
        out = pv.pwszVal;
    PropVariantClear(&pv);
    store->Release();
    return out;
}

static bool EwpResolveWindowApp(HWND hwnd, std::wstring& outExe, std::wstring& outAumid) {
    outExe = GetProcessPath(hwnd);
    outAumid = EwpQueryAumid(hwnd);
    if (outExe.empty()) return false;
    if (StrStrIW(outExe.c_str(), L"ApplicationFrameHost.exe")) {
        DWORD hostPid = 0;
        GetWindowThreadProcessId(hwnd, &hostPid);
        UwpCtx ctx = { NULL, hostPid };
        EnumChildWindows(hwnd, UwpEnumProc, (LPARAM)&ctx);
        if (ctx.found) {
            std::wstring realExe = GetProcessPath(ctx.found);
            if (!realExe.empty()) outExe = realExe;
            if (outAumid.empty()) outAumid = EwpQueryAumid(ctx.found);
        }
    }
    return !outExe.empty();
}

static std::wstring EwpEscape(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        if (c == L'\\') out += L"\\\\";
        else if (c == L'|') out += L"\\p";
        else if (c == L';') out += L"\\s";
        else if (c == L'\n') out += L"\\n";
        else if (c != L'\r') out += c;
    }
    return out;
}

static std::wstring EwpUnescape(const std::wstring& s) {
    std::wstring out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == L'\\' && i + 1 < s.size()) {
            wchar_t n = s[++i];
            if (n == L'p') out += L'|';
            else if (n == L's') out += L';';
            else if (n == L'n') out += L'\n';
            else out += n;
        } else out += s[i];
    }
    return out;
}

static std::vector<std::wstring> EwpSplit(const std::wstring& s, wchar_t delim) {
    std::vector<std::wstring> parts;
    std::wstring cur;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == L'\\' && i + 1 < s.size()) {
            cur += s[i]; cur += s[++i];
        } else if (s[i] == delim) {
            parts.push_back(cur); cur.clear();
        } else cur += s[i];
    }
    parts.push_back(cur);
    return parts;
}

static void EwpSavePlacement() {
    if (!g_ewpRememberPos) return;
    Wh_SetIntValue(L"ewp_edge", (int)g_ewpCurEdge);
    Wh_SetIntValue(L"ewp_offsetPermille", (int)lroundf(EwpClamp01(g_ewpOffsetRatio) * 1000.0f));
}

static void EwpLoadPlacement() {
    if (!g_ewpRememberPos) {
        g_ewpCurEdge = g_ewpDefaultEdge;
        g_ewpOffsetRatio = 0.5f;
        return;
    }
    int e = Wh_GetIntValue(L"ewp_edge", -1);
    g_ewpCurEdge = (e >= 0 && e <= 3) ? (EwpEdge)e : g_ewpDefaultEdge;
    g_ewpOffsetRatio = EwpClamp01(Wh_GetIntValue(L"ewp_offsetPermille", 500) / 1000.0f);
}

// ============================================================
//  NON-OVERLAPPING PROPORTIONAL SPLIT NORMALIZATION
// ============================================================
static bool EwpHasOverlappingApps(const EwpLayoutWorkspace& ws) {
    int n = (int)ws.apps.size();
    if (n <= 1) return false;
    float totalArea = 0.0f;
    for (int i = 0; i < n; ++i) {
        totalArea += ws.apps[i].nw * ws.apps[i].nh;
        for (int j = i + 1; j < n; ++j) {
            float ox = std::min(ws.apps[i].nx + ws.apps[i].nw, ws.apps[j].nx + ws.apps[j].nw) -
                       std::max(ws.apps[i].nx, ws.apps[j].nx);
            float oy = std::min(ws.apps[i].ny + ws.apps[i].nh, ws.apps[j].ny + ws.apps[j].nh) -
                       std::max(ws.apps[i].ny, ws.apps[j].ny);
            if (ox > 0.04f && oy > 0.04f) return true;
        }
    }
    return (totalArea < 0.85f || totalArea > 1.12f);
}

static void EwpNormalizeNonOverlappingLayout(EwpLayoutWorkspace& ws, bool sortByScreenPos = false) {
    int n = (int)ws.apps.size();
    if (n <= 0) return;
    if (sortByScreenPos && n > 1) {
        std::stable_sort(ws.apps.begin(), ws.apps.end(), [](const EwpAppLayoutEntry& a, const EwpAppLayoutEntry& b) {
            float acx = a.nx + a.nw * 0.5f, bcx = b.nx + b.nw * 0.5f;
            if (fabsf(acx - bcx) > 0.14f) return acx < bcx;
            return (a.ny + a.nh * 0.5f) < (b.ny + b.nh * 0.5f);
        });
    }
    std::vector<float> weights(n, 1.0f);
    float sumW = 0.0f;
    for (int i = 0; i < n; ++i) {
        weights[i] = std::max(0.12f, ws.apps[i].nw * ws.apps[i].nh);
        sumW += weights[i];
    }
    if (sumW <= 0.0f) sumW = 1.0f;

    if (n == 1) {
        ws.apps[0].nx = 0.0f; ws.apps[0].ny = 0.0f;
        ws.apps[0].nw = 1.0f; ws.apps[0].nh = 1.0f;
        ws.apps[0].isMaximized = false;
    } else if (n == 2) {
        float w0 = std::max(0.24f, std::min(0.76f, weights[0] / sumW));
        ws.apps[0].nx = 0.0f; ws.apps[0].ny = 0.0f; ws.apps[0].nw = w0;        ws.apps[0].nh = 1.0f; ws.apps[0].isMaximized = false;
        ws.apps[1].nx = w0;   ws.apps[1].ny = 0.0f; ws.apps[1].nw = 1.0f - w0; ws.apps[1].nh = 1.0f; ws.apps[1].isMaximized = false;
    } else if (n == 3) {
        float w0 = std::max(0.30f, std::min(0.68f, weights[0] / sumW));
        float wRem = 1.0f - w0;
        float h1 = std::max(0.28f, std::min(0.72f, weights[1] / std::max(0.01f, weights[1] + weights[2])));
        ws.apps[0].nx = 0.0f; ws.apps[0].ny = 0.0f; ws.apps[0].nw = w0;   ws.apps[0].nh = 1.0f;        ws.apps[0].isMaximized = false;
        ws.apps[1].nx = w0;   ws.apps[1].ny = 0.0f; ws.apps[1].nw = wRem; ws.apps[1].nh = h1;          ws.apps[1].isMaximized = false;
        ws.apps[2].nx = w0;   ws.apps[2].ny = h1;   ws.apps[2].nw = wRem; ws.apps[2].nh = 1.0f - h1;   ws.apps[2].isMaximized = false;
    } else {
        int cols = (n <= 4) ? 2 : 3;
        int rows = (n + cols - 1) / cols;
        for (int i = 0; i < n; ++i) {
            int r = i / cols;
            int c = i % cols;
            int colsInRow = (r == rows - 1) ? (n - r * cols) : cols;
            ws.apps[i].nx = (float)c / (float)colsInRow;
            ws.apps[i].ny = (float)r / (float)rows;
            ws.apps[i].nw = 1.0f / (float)colsInRow;
            ws.apps[i].nh = 1.0f / (float)rows;
            ws.apps[i].isMaximized = false;
        }
    }
    ws.name = ws.apps[0].displayName;
    if (ws.apps.size() > 1) ws.name += L" +" + std::to_wstring(ws.apps.size() - 1);
}

static void EwpReflowWorkspaceRatios(EwpLayoutWorkspace& ws, int primaryAppIdx = 0, float primaryShare = 0.0f) {
    int n = (int)ws.apps.size();
    if (n <= 0) return;
    if (primaryAppIdx >= 0 && primaryAppIdx < n && primaryAppIdx != 0) {
        std::swap(ws.apps[0], ws.apps[primaryAppIdx]);
    }
    if (primaryShare > 0.15f && primaryShare < 0.85f && n >= 2) {
        ws.apps[0].nw = primaryShare;
        ws.apps[0].nh = 1.0f;
        float remShare = (1.0f - primaryShare) / (float)(n - 1);
        for (int i = 1; i < n; ++i) {
            ws.apps[i].nw = remShare;
            ws.apps[i].nh = 1.0f;
        }
    }
    EwpNormalizeNonOverlappingLayout(ws, false);
}

static void EwpSaveWorkspaces() {
    std::wstringstream ss;
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    for (const auto& ws : g_ewpWorkspaces) {
        if (ws.apps.empty()) continue;
        ss << EwpEscape(ws.id) << L"|" << EwpEscape(ws.name) << L"|";
        for (size_t i = 0; i < ws.apps.size(); ++i) {
            if (i > 0) ss << L";";
            const auto& a = ws.apps[i];
            ss << (int)a.kind << L"|" << EwpEscape(a.exePath) << L"|" << EwpEscape(a.aumid) << L"|"
               << EwpEscape(a.activeUrl) << L"|" << EwpEscape(a.windowClass) << L"|"
               << EwpEscape(a.displayName) << L"|"
               << (int)lroundf(EwpClamp01(a.nx) * 10000.0f) << L"|"
               << (int)lroundf(EwpClamp01(a.ny) * 10000.0f) << L"|"
               << (int)lroundf(EwpClamp01(a.nw) * 10000.0f) << L"|"
               << (int)lroundf(EwpClamp01(a.nh) * 10000.0f) << L"|"
               << (a.isMaximized ? 1 : 0);
        }
        ss << L"\n";
    }
    if (g_csInitialized) LeaveCriticalSection(&g_cs);
    std::wstring blob = ss.str();
    Wh_SetIntValue(L"ewp_ws_len", (int)blob.size());
    Wh_SetStringValue(L"ewp_ws_v1", blob.c_str());
}

static void EwpDeduplicateWorkspacesInMemory() {
    if (g_ewpAllowDuplicates || g_ewpDuplicatePolicy == EWP_DUP_ALLOW) return;
    std::vector<std::wstring> seenSigs;
    for (size_t i = 0; i < g_ewpWorkspaces.size();) {
        std::wstring sig = EwpMakeWorkspaceSignature(g_ewpWorkspaces[i]);
        if (!sig.empty() && std::find(seenSigs.begin(), seenSigs.end(), sig) != seenSigs.end()) {
            g_ewpWorkspaces.erase(g_ewpWorkspaces.begin() + i);
        } else {
            if (!sig.empty()) seenSigs.push_back(sig);
            ++i;
        }
    }
    if (g_ewpFocusIdx >= (int)g_ewpWorkspaces.size())
        g_ewpFocusIdx = std::max(0, (int)g_ewpWorkspaces.size() - 1);
    if (g_ewpScrollIdx >= (int)g_ewpWorkspaces.size())
        g_ewpScrollIdx = std::max(0, (int)g_ewpWorkspaces.size() - 1);
}

static void EwpLoadWorkspaces() {
    int len = Wh_GetIntValue(L"ewp_ws_len", 0);
    if (len <= 0 || len > 262144) return;
    std::vector<wchar_t> buf((size_t)len + 2, 0);
    if (Wh_GetStringValue(L"ewp_ws_v1", buf.data(), buf.size()) == 0) return;

    std::vector<EwpLayoutWorkspace> list;
    std::wstringstream ss(buf.data());
    std::wstring line;
    bool anyNormalized = false;
    while (std::getline(ss, line, L'\n')) {
        line = EwpTrim(line);
        if (line.empty()) continue;
        auto cols = EwpSplit(line, L'|');
        if (cols.size() < 3) continue;
        EwpLayoutWorkspace ws;
        ws.id = EwpUnescape(cols[0]);
        ws.name = EwpUnescape(cols[1]);
        size_t b1 = 0, b2 = 0;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == L'\\' && i + 1 < line.size()) { ++i; continue; }
            if (line[i] == L'|') { b1 = i; break; }
        }
        for (size_t i = b1 + 1; i < line.size(); ++i) {
            if (line[i] == L'\\' && i + 1 < line.size()) { ++i; continue; }
            if (line[i] == L'|') { b2 = i; break; }
        }
        if (!b2 || b2 + 1 >= line.size()) continue;
        for (const auto& tok : EwpSplit(line.substr(b2 + 1), L';')) {
            if ((int)ws.apps.size() >= g_ewpMaxApps) break;
            auto f = EwpSplit(tok, L'|');
            if (f.size() < 11) continue;
            EwpAppLayoutEntry a;
            a.kind = (EwpTargetKind)std::max(0, std::min(3, _wtoi(f[0].c_str())));
            a.exePath = EwpUnescape(f[1]);
            a.aumid = EwpUnescape(f[2]);
            a.activeUrl = EwpUnescape(f[3]);
            a.windowClass = EwpUnescape(f[4]);
            a.displayName = EwpUnescape(f[5]);
            a.nx = EwpClamp01(_wtoi(f[6].c_str()) / 10000.0f);
            a.ny = EwpClamp01(_wtoi(f[7].c_str()) / 10000.0f);
            a.nw = std::max(0.05f, EwpClamp01(_wtoi(f[8].c_str()) / 10000.0f));
            a.nh = std::max(0.05f, EwpClamp01(_wtoi(f[9].c_str()) / 10000.0f));
            a.isMaximized = (_wtoi(f[10].c_str()) != 0);
            if (!a.exePath.empty() || !a.aumid.empty()) ws.apps.push_back(std::move(a));
        }
        if (!ws.apps.empty()) {
            if (EwpHasOverlappingApps(ws)) {
                EwpNormalizeNonOverlappingLayout(ws, true);
                anyNormalized = true;
            }
            list.push_back(std::move(ws));
        }
    }
    bool changed = anyNormalized;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        size_t before = list.size();
        g_ewpWorkspaces = std::move(list);
        EwpDeduplicateWorkspacesInMemory();
        if (g_ewpWorkspaces.size() != before) changed = true;
        LeaveCriticalSection(&g_cs);
    }
    if (changed) ++g_ewpDockGeneration;
    EwpSaveWorkspaces();
}

static MONITORINFO EwpGetMonitorInfo() {
    if (!g_ewpMonitor) g_ewpMonitor = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(g_ewpMonitor, &mi)) {
        g_ewpMonitor = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfoW(g_ewpMonitor, &mi);
    }
    return mi;
}

static RECT EwpComputePillRect() {
    MONITORINFO mi = EwpGetMonitorInfo();
    UINT dpi = g_ewpPillWnd ? GetDpiForWindow(g_ewpPillWnd) : 96u;
    if (!dpi) dpi = 96u;
    float s = dpi / 96.0f;
    int aaPad = std::max(2, (int)lroundf(2.0f * s));
    int edgeMargin = std::max(1, (int)lroundf(1.5f * s));
    int thick = (int)lroundf(g_ewpPillThick * s) + aaPad * 2;
    int len   = (int)lroundf(g_ewpPillLen * s)   + aaPad * 2;
    bool horiz = (g_ewpCurEdge == EWP_EDGE_TOP || g_ewpCurEdge == EWP_EDGE_BOTTOM);
    int w = horiz ? len : thick;
    int h = horiz ? thick : len;
    int workW = std::max<LONG>(w, mi.rcWork.right - mi.rcWork.left);
    int workH = std::max<LONG>(h, mi.rcWork.bottom - mi.rcWork.top);
    RECT rc = {};
    if (horiz) {
        rc.left   = mi.rcWork.left + (int)lroundf(EwpClamp01(g_ewpOffsetRatio) * std::max(0, workW - w));
        rc.right  = rc.left + w;
        rc.top    = (g_ewpCurEdge == EWP_EDGE_TOP) ? (mi.rcWork.top + edgeMargin) : (mi.rcWork.bottom - h - edgeMargin);
        rc.bottom = rc.top + h;
    } else {
        rc.top    = mi.rcWork.top + (int)lroundf(EwpClamp01(g_ewpOffsetRatio) * std::max(0, workH - h));
        rc.bottom = rc.top + h;
        rc.left   = (g_ewpCurEdge == EWP_EDGE_LEFT) ? (mi.rcWork.left + edgeMargin) : (mi.rcWork.right - w - edgeMargin);
        rc.right  = rc.left + w;
    }
    return rc;
}

static void EwpPresentLayered(HWND hwnd, const RECT& rc, BYTE alpha,
                              void (*drawFn)(Gdiplus::Graphics&, int, int, float)) {
    if (!hwnd || !IsWindow(hwnd)) return;
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;
    HDC sdc = GetDC(NULL);
    if (!sdc) return;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = NULL;
    HBITMAP dib = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    HDC mdc = dib ? CreateCompatibleDC(sdc) : NULL;
    if (dib && mdc && bits) {
        HBITMAP old = (HBITMAP)SelectObject(mdc, dib);
        memset(bits, 0, (size_t)w * h * 4);
        {
            Gdiplus::Graphics g(mdc);
            g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
            g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
            drawFn(g, w, h, EwpDpiScale(hwnd));
        }
        POINT dst = { rc.left, rc.top }, src = { 0, 0 };
        SIZE sz = { w, h };
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
        UpdateLayeredWindow(hwnd, sdc, &dst, &sz, mdc, &src, 0, &bf, ULW_ALPHA);
        SelectObject(mdc, old);
    }
    if (mdc) DeleteDC(mdc);
    if (dib) DeleteObject(dib);
    ReleaseDC(NULL, sdc);
}

static void EwpDrawPill(Gdiplus::Graphics& g, int w, int h, float s) {
    float inset = std::max(2.0f, 2.0f * s);
    Gdiplus::RectF rc(inset, inset, std::max(2.0f, (float)w - inset * 2.0f), std::max(2.0f, (float)h - inset * 2.0f));
    float pillRadius = (std::min(rc.Width, rc.Height) * 0.5f) * (std::max(25, g_ewpPillRoundness) / 100.0f);
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, rc, pillRadius);

    bool activeOrOpen = (g_ewpDockState != EWP_DOCK_CLOSED);
    Gdiplus::SolidBrush pillFill(activeOrOpen
        ? Gdiplus::Color(215, 138, 162, 182)
        : Gdiplus::Color(185, 116, 138, 158));
    g.FillPath(&pillFill, &path);
}

static void EwpRenderPill() {
    if (!g_ewpPillWnd || !IsWindow(g_ewpPillWnd)) return;
    if (!g_ewpEnabled.load() || (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0)) {
        if (IsWindowVisible(g_ewpPillWnd)) ShowWindow(g_ewpPillWnd, SW_HIDE);
        return;
    }
    RECT rc = g_ewpPillDrag ? g_ewpPillDragRc : EwpComputePillRect();
    bool wasVisible = IsWindowVisible(g_ewpPillWnd) != FALSE;
    EwpPresentLayered(g_ewpPillWnd, rc, 255, EwpDrawPill);
    SetWindowPos(g_ewpPillWnd, HWND_TOPMOST, rc.left, rc.top,
                 rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    if (!wasVisible) {
        Wh_Log(L"EWP: pill rendered hwnd=%p edge=%d rect=(%ld,%ld %ldx%ld)",
               g_ewpPillWnd, (int)g_ewpCurEdge, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }
}

// ============================================================
//  TRUE PARTIAL-CIRCLE ARC CAROUSEL WITH CENTER-FIXED '+' BUTTON
// ============================================================
struct EwpArcSlotGeom {
    Gdiplus::RectF              cardRc;
    std::vector<Gdiplus::RectF> appIconRcs;
    int                         wsIdx    = -1;
    bool                        isCenter = false;
    bool                        isAddBtn = false;
};

static void EwpComputeCardSubIconRects(const Gdiplus::RectF& cardRc, int nApps,
                                       std::vector<Gdiplus::RectF>& outRcs) {
    outRcs.clear();
    if (nApps <= 0) return;
    if (nApps == 1) {
        float isz = cardRc.Width * 0.56f;
        outRcs.push_back(Gdiplus::RectF(
            cardRc.X + (cardRc.Width - isz) * 0.5f,
            cardRc.Y + (cardRc.Height - isz) * 0.5f,
            isz, isz));
    } else if (nApps == 2) {
        float isz = cardRc.Width * 0.40f;
        float igap = cardRc.Width * 0.06f;
        float startX = cardRc.X + (cardRc.Width - (isz * 2.0f + igap)) * 0.5f;
        float cy = cardRc.Y + (cardRc.Height - isz) * 0.5f;
        for (int a = 0; a < 2; ++a) {
            outRcs.push_back(Gdiplus::RectF(startX + a * (isz + igap), cy, isz, isz));
        }
    } else {
        int showCnt = std::min(nApps, 4);
        float isz = cardRc.Width * 0.35f;
        float igap = cardRc.Width * 0.06f;
        float startX = cardRc.X + (cardRc.Width - (isz * 2.0f + igap)) * 0.5f;
        float startY = cardRc.Y + (cardRc.Height - (isz * 2.0f + igap)) * 0.5f;
        for (int a = 0; a < showCnt; ++a) {
            int gx = a % 2, gy = a / 2;
            outRcs.push_back(Gdiplus::RectF(
                startX + gx * (isz + igap),
                startY + gy * (isz + igap),
                isz, isz));
        }
    }
}

static void EwpBuildArcSlots(float s, const std::vector<EwpLayoutWorkspace>& wsList,
                             int* outWinW, int* outWinH,
                             std::vector<EwpArcSlotGeom>* outSlots) {
    int totalWs = (int)wsList.size();
    int maxVisWs = std::max(2, std::min(6, g_ewpArcVisibleSlots));
    int visWs    = std::min(totalWs, maxVisWs);

    int leftCount  = (visWs + 1) / 2;
    int rightCount = visWs - leftCount;
    int sideReach  = std::max(1, std::max(leftCount, rightCount));
    int centerSlot = leftCount;
    int totalSlots = visWs + 1;

    float baseSz   = (float)g_ewpArcCardSize * s;
    float centerSz = baseSz * 1.06f;
    float gap      = (float)g_ewpArcCardGap * s;
    float depth    = (float)g_ewpArcCurveDepth * s;
    float pad      = 8.0f * s;

    const float kMaxTheta = 1.08f; // ~62 degrees half-angle => 124-degree visible partial circle
    float stepTheta = kMaxTheta / (float)sideReach;
    float chordStep = baseSz + gap;
    float sinStep   = std::max(0.25f, sinf(stepTheta));
    float arcRadius = chordStep / sinStep;

    float maxAlongOffset = arcRadius * sinf((float)sideReach * stepTheta);
    float span  = pad * 2.0f + maxAlongOffset * 2.0f + centerSz;
    float cross = pad * 2.0f + depth + centerSz;

    bool horiz = (g_ewpCurEdge == EWP_EDGE_TOP || g_ewpCurEdge == EWP_EDGE_BOTTOM);
    int winW = (int)ceilf(horiz ? span : cross);
    int winH = (int)ceilf(horiz ? cross : span);
    if (outWinW) *outWinW = std::max(64, winW);
    if (outWinH) *outWinH = std::max(64, winH);
    if (!outSlots) return;

    outSlots->clear();
    outSlots->reserve(totalSlots);

    float centerCoord = (horiz ? (float)winW : (float)winH) * 0.5f;
    float cosMax = cosf(kMaxTheta);
    float cosSpan = std::max(0.15f, 1.0f - cosMax);

    int scrollBase = (totalWs > maxVisWs) ? (((g_ewpScrollIdx % totalWs) + totalWs) % totalWs) : 0;
    int wsCounter = 0;

    for (int i = 0; i < totalSlots; ++i) {
        int relStep = i - centerSlot; // <0 = before '+', 0 = center '+', >0 = after '+'
        bool isAdd  = (relStep == 0);
        float sz    = isAdd ? centerSz : baseSz;

        float theta = (float)relStep * stepTheta;
        float slotCenterAlong = centerCoord + arcRadius * sinf(theta);
        float alongPos = slotCenterAlong - sz * 0.5f;

        float circleOutwardNorm = EwpClamp01((cosf(theta) - cosMax) / cosSpan);
        float outward = depth * circleOutwardNorm;

        float cx = 0.0f, cy = 0.0f;
        if (g_ewpCurEdge == EWP_EDGE_LEFT) {
            cx = pad + outward + (centerSz - sz) * 0.5f;
            cy = alongPos;
        } else if (g_ewpCurEdge == EWP_EDGE_RIGHT) {
            cx = (float)winW - pad - outward - sz - (centerSz - sz) * 0.5f;
            cy = alongPos;
        } else if (g_ewpCurEdge == EWP_EDGE_TOP) {
            cx = alongPos;
            cy = pad + outward + (centerSz - sz) * 0.5f;
        } else { // EWP_EDGE_BOTTOM
            cx = alongPos;
            cy = (float)winH - pad - outward - sz - (centerSz - sz) * 0.5f;
        }

        EwpArcSlotGeom slot;
        slot.cardRc   = Gdiplus::RectF(cx, cy, sz, sz);
        slot.isCenter = isAdd;
        slot.isAddBtn = isAdd;

        if (!isAdd && totalWs > 0) {
            slot.wsIdx = (scrollBase + wsCounter) % totalWs;
            ++wsCounter;
            EwpComputeCardSubIconRects(slot.cardRc, (int)wsList[slot.wsIdx].apps.size(), slot.appIconRcs);
        } else {
            slot.wsIdx = -1;
        }

        outSlots->push_back(std::move(slot));
    }
}

static RECT EwpComputeHoverDockRect(float ease) {
    RECT pillRc = EwpComputePillRect();
    MONITORINFO mi = EwpGetMonitorInfo();
    float s = EwpDpiScale(g_ewpDockWnd);
    std::vector<EwpLayoutWorkspace> snap;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        snap = g_ewpWorkspaces;
        LeaveCriticalSection(&g_cs);
    }
    int w = 0, h = 0;
    EwpBuildArcSlots(s, snap, &w, &h, nullptr);

    int slide = (int)lroundf((1.0f - ease) * 10.0f * s);
    int pillGap = (int)lroundf(g_ewpPillToArcGap * s);
    bool horiz = (g_ewpCurEdge == EWP_EDGE_TOP || g_ewpCurEdge == EWP_EDGE_BOTTOM);
    RECT rc = {};
    if (horiz) {
        int cx = (pillRc.left + pillRc.right) / 2;
        rc.left   = std::max<LONG>(mi.rcWork.left + 6, std::min<LONG>(cx - w / 2, mi.rcWork.right - w - 6));
        rc.right  = rc.left + w;
        rc.top    = (g_ewpCurEdge == EWP_EDGE_TOP) ? (pillRc.bottom + pillGap - slide) : (pillRc.top - pillGap - h + slide);
        rc.bottom = rc.top + h;
    } else {
        int cy = (pillRc.top + pillRc.bottom) / 2;
        rc.top    = std::max<LONG>(mi.rcWork.top + 6, std::min<LONG>(cy - h / 2, mi.rcWork.bottom - h - 6));
        rc.bottom = rc.top + h;
        rc.left   = (g_ewpCurEdge == EWP_EDGE_LEFT) ? (pillRc.right + pillGap - slide) : (pillRc.left - pillGap - w + slide);
        rc.right  = rc.left + w;
    }
    return rc;
}

static void EwpDrawIconBitmapAlpha(Gdiplus::Graphics& g, Gdiplus::Bitmap* bmp,
                                   const Gdiplus::RectF& dstRc, float alpha) {
    if (!bmp || alpha <= 0.01f) return;
    if (alpha >= 0.99f) {
        g.DrawImage(bmp, dstRc, 0.0f, 0.0f, (float)bmp->GetWidth(), (float)bmp->GetHeight(), Gdiplus::UnitPixel);
    } else {
        Gdiplus::ColorMatrix cm = {{
            {1, 0, 0, 0, 0},
            {0, 1, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, alpha, 0},
            {0, 0, 0, 0, 1}
        }};
        Gdiplus::ImageAttributes ia;
        ia.SetColorMatrix(&cm, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
        g.DrawImage(bmp, dstRc, 0.0f, 0.0f, (float)bmp->GetWidth(), (float)bmp->GetHeight(), Gdiplus::UnitPixel, &ia);
    }
}

static Gdiplus::RectF EwpComputeArcCardDelRect(const Gdiplus::RectF& cardRc, float s) {
    float badgeSz = std::max(14.0f * s, std::min(18.0f * s, cardRc.Width * 0.30f));
    float inset   = 1.5f * s;
    return Gdiplus::RectF(cardRc.GetRight() - badgeSz - inset, cardRc.Y + inset, badgeSz, badgeSz);
}

static void EwpDrawHoverDock(Gdiplus::Graphics& g, int w, int h, float s) {
    (void)w; (void)h;
    std::vector<EwpLayoutWorkspace> snap;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        snap = g_ewpWorkspaces;
        LeaveCriticalSection(&g_cs);
    }
    int total = (int)snap.size();
    std::vector<EwpArcSlotGeom> slots;
    EwpBuildArcSlots(s, snap, nullptr, nullptr, &slots);

    Gdiplus::FontFamily ff(L"Segoe UI");
    Gdiplus::Font plusFont(&ff, 18.0f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font cardDelFont(&ff, 10.5f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::StringFormat fmtCenter;
    fmtCenter.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmtCenter.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::SolidBrush whiteBr(Gdiplus::Color(250, 246, 248, 252));
    Gdiplus::SolidBrush softRedTxtBr(Gdiplus::Color(245, 255, 108, 108));

    float roundFactor = std::max(20, std::min(100, g_ewpArcCardRoundness)) / 100.0f;

    for (int i = 0; i < (int)slots.size(); ++i) {
        const auto& sl = slots[i];
        float radius = (std::min(sl.cardRc.Width, sl.cardRc.Height) * 0.5f) * roundFactor;
        Gdiplus::GraphicsPath cardPath;
        AddRoundedRectPath(cardPath, sl.cardRc, radius);

        if (sl.isAddBtn) {
            Gdiplus::SolidBrush addFill(g_ewpHoverAdd
                ? Gdiplus::Color(248, 204, 126, 88)
                : Gdiplus::Color(238, 26, 30, 38));
            g.FillPath(&addFill, &cardPath);
            g.DrawString(L"+", -1, &plusFont, sl.cardRc, &fmtCenter, &whiteBr);
            continue;
        }

        if (sl.wsIdx < 0 || sl.wsIdx >= total) continue;
        bool hot = (i == g_ewpHoverSlot);
        bool delHot = (hot && g_ewpHoverArcDel);
        bool focusedForPreview = (g_ewpPreviewOpen && sl.wsIdx == g_ewpPrevActiveWsIdx);

        Gdiplus::Color cardColor = hot
            ? Gdiplus::Color(252, 46, 38, 35)
            : (focusedForPreview ? Gdiplus::Color(246, 34, 30, 38) : Gdiplus::Color(232, 16, 18, 22));
        Gdiplus::SolidBrush cardFill(cardColor);
        g.FillPath(&cardFill, &cardPath);

        const auto& apps = snap[sl.wsIdx].apps;
        float iconAlpha = (hot || focusedForPreview) ? 1.0f : 0.88f;

        for (size_t a = 0; a < sl.appIconRcs.size() && a < apps.size(); ++a) {
            const Gdiplus::RectF& irc = sl.appIconRcs[a];
            if (hot && !delHot && (int)a == g_ewpHoverSubApp && apps.size() > 1) {
                Gdiplus::RectF hlRc(irc.X - 2.0f * s, irc.Y - 2.0f * s, irc.Width + 4.0f * s, irc.Height + 4.0f * s);
                Gdiplus::GraphicsPath hlPath;
                AddRoundedRectPath(hlPath, hlRc, 5.0f * s);
                Gdiplus::SolidBrush hlFill(Gdiplus::Color(48, 255, 255, 255));
                g.FillPath(&hlFill, &hlPath);
            }
            Gdiplus::Bitmap* bmp = EwpGetCachedAppIcon(apps[a].exePath);
            if (bmp) EwpDrawIconBitmapAlpha(g, bmp, irc, iconAlpha);
        }

        // Top-right 'x' button on every Arc Card to delete that saved workspace state easily
        Gdiplus::RectF delRc = EwpComputeArcCardDelRect(sl.cardRc, s);
        Gdiplus::SolidBrush delBg(delHot
            ? Gdiplus::Color(248, 224, 62, 62)
            : (hot ? Gdiplus::Color(225, 38, 22, 26) : Gdiplus::Color(195, 22, 18, 22)));
        g.FillEllipse(&delBg, delRc);
        g.DrawString(L"×", -1, &cardDelFont, delRc, &fmtCenter, delHot ? &whiteBr : &softRedTxtBr);
    }
}

static void EwpRenderSchematicPreview();

// Renders BOTH the Arc Carousel and its Coupled Schematic Thumbnail together in lockstep
static void EwpRenderHoverDock() {
    if (!g_ewpDockWnd || !IsWindow(g_ewpDockWnd)) return;
    if (!g_ewpEnabled.load() || g_ewpDockState == EWP_DOCK_CLOSED ||
        (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0)) {
        ShowWindow(g_ewpDockWnd, SW_HIDE);
        if (g_ewpPreviewWnd && IsWindowVisible(g_ewpPreviewWnd))
            ShowWindow(g_ewpPreviewWnd, SW_HIDE);
        return;
    }

    int total = 0;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        total = (int)g_ewpWorkspaces.size();
        LeaveCriticalSection(&g_cs);
    }
    if (total > 0) {
        if (g_ewpPreviewEditMode) {
            // Customize / Edit Mode (triggered by single or double right-click):
            // Keep g_ewpPrevActiveWsIdx locked to the selected workspace so moving the cursor
            // into the Thumbnail Preview never hijacks the workspace being customized.
            if (g_ewpPrevActiveWsIdx < 0 || g_ewpPrevActiveWsIdx >= total)
                g_ewpPrevActiveWsIdx = ((g_ewpFocusIdx % total) + total) % total;
            g_ewpFocusIdx    = g_ewpPrevActiveWsIdx;
            g_ewpPreviewOpen = true;
        } else if (g_ewpHoverWsIdx >= 0 && g_ewpHoverWsIdx < total) {
            // Read-Only Hover Preview Mode:
            // Hovering over ANY pinned Arc Card (or any of its app icons) immediately displays
            // the Thumbnail Preview in Read-Only mode.
            g_ewpFocusIdx        = g_ewpHoverWsIdx;
            g_ewpPrevActiveWsIdx = g_ewpHoverWsIdx;
            g_ewpPreviewOpen     = true;
        } else {
            g_ewpPreviewOpen     = false;
            g_ewpPrevActiveWsIdx = -1;
        }
    } else {
        g_ewpPreviewOpen     = false;
        g_ewpPreviewEditMode = false;
        g_ewpPrevActiveWsIdx = -1;
    }

    float ease = EwpSmoothStep(g_ewpAnimProg);
    BYTE alpha = (BYTE)std::max(0, std::min(255, (int)lroundf(255.0f * ease)));
    RECT rc = EwpComputeHoverDockRect(ease);

    // Avoid redundant GDI+ DIB reallocations & UpdateLayeredWindow calls when nothing changed
    static RECT     s_lastDockRc = {};
    static BYTE     s_lastAlpha  = 0;
    static int      s_lastSlot   = -99;
    static int      s_lastWs     = -99;
    static int      s_lastSub    = -99;
    static bool     s_lastAdd    = false;
    static bool     s_lastDel    = false;
    static int      s_lastPrevWs = -99;
    static bool     s_lastEdit   = false;
    static int      s_lastTotal  = -99;
    static uint64_t s_lastGen    = 0;

    bool dockVisible = IsWindowVisible(g_ewpDockWnd) != FALSE;
    bool dockDirty = !dockVisible ||
                     rc.left != s_lastDockRc.left || rc.top != s_lastDockRc.top ||
                     rc.right != s_lastDockRc.right || rc.bottom != s_lastDockRc.bottom ||
                     alpha != s_lastAlpha ||
                     g_ewpHoverSlot != s_lastSlot ||
                     g_ewpHoverWsIdx != s_lastWs ||
                     g_ewpHoverSubApp != s_lastSub ||
                     g_ewpHoverAdd != s_lastAdd ||
                     g_ewpHoverArcDel != s_lastDel ||
                     g_ewpPrevActiveWsIdx != s_lastPrevWs ||
                     g_ewpPreviewEditMode != s_lastEdit ||
                     total != s_lastTotal ||
                     g_ewpDockGeneration != s_lastGen;

    if (dockDirty) {
        EwpPresentLayered(g_ewpDockWnd, rc, alpha, EwpDrawHoverDock);
        s_lastDockRc = rc;
        s_lastAlpha  = alpha;
        s_lastSlot   = g_ewpHoverSlot;
        s_lastWs     = g_ewpHoverWsIdx;
        s_lastSub    = g_ewpHoverSubApp;
        s_lastAdd    = g_ewpHoverAdd;
        s_lastDel    = g_ewpHoverArcDel;
        s_lastPrevWs = g_ewpPrevActiveWsIdx;
        s_lastEdit   = g_ewpPreviewEditMode;
        s_lastTotal  = total;
        s_lastGen    = g_ewpDockGeneration;
    }
    if (!dockVisible && alpha > 4) {
        SetWindowPos(g_ewpDockWnd, HWND_TOPMOST, rc.left, rc.top,
                     rc.right - rc.left, rc.bottom - rc.top, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    EwpRenderSchematicPreview();
}

// ============================================================
//  COUPLED BORDERLESS SCHEMATIC THUMBNAIL (CUSTOMIZATION CANVAS)
//  - Zero 'x' buttons anywhere:
//      * Double-clicking any app block inside the Schematic Thumbnail removes/unpins
//        that item and auto-reflows remaining apps to 100%
//      * Clicking the borderless '+' badge in the bottom-right corner of the
//        Schematic Thumbnail adds the top visible window on the monitor into
//        this workspace and auto-reflows split ratios
// ============================================================
struct EwpPreviewBlockGeom {
    Gdiplus::RectF blockRc;
    int            appIdx = -1;
};

static Gdiplus::RectF EwpComputePreviewAddBtnRect(float s, int cardW, int cardH) {
    float btnSz = 20.0f * s;
    float pad   = 2.0f * s;
    float inset = 7.0f * s;
    return Gdiplus::RectF((float)cardW - pad - btnSz - inset, (float)cardH - pad - btnSz - inset, btnSz, btnSz);
}

static Gdiplus::RectF EwpComputePreviewBlockDelRect(const Gdiplus::RectF& bRc, float s) {
    float delSz = 20.0f * s;
    float inset = 7.0f * s;
    return Gdiplus::RectF(bRc.GetRight() - delSz - inset, bRc.Y + inset, delSz, delSz);
}

static void EwpBuildPreviewBlocks(float s, int cardW, int cardH,
                                  const EwpLayoutWorkspace& ws,
                                  std::vector<EwpPreviewBlockGeom>* outBlocks) {
    outBlocks->clear();
    float pad = 2.0f * s;
    float splitGap = (float)g_ewpPreviewSplitGap * s;
    float cx = pad, cy = pad;
    float cw = (float)cardW - pad * 2.0f;
    float ch = (float)cardH - pad * 2.0f;

    for (size_t i = 0; i < ws.apps.size(); ++i) {
        const auto& a = ws.apps[i];
        float leftInset   = (a.nx > 0.03f) ? (splitGap * 0.5f) : 0.0f;
        float rightInset  = (a.nx + a.nw < 0.97f) ? (splitGap * 0.5f) : 0.0f;
        float topInset    = (a.ny > 0.03f) ? (splitGap * 0.5f) : 0.0f;
        float bottomInset = (a.ny + a.nh < 0.97f) ? (splitGap * 0.5f) : 0.0f;

        float bx = cx + a.nx * cw + leftInset;
        float by = cy + a.ny * ch + topInset;
        float bw = std::max(34.0f * s, a.nw * cw - leftInset - rightInset);
        float bh = std::max(28.0f * s, a.nh * ch - topInset - bottomInset);
        Gdiplus::RectF bRc(bx, by, bw, bh);

        outBlocks->push_back({ bRc, (int)i });
    }
}

static RECT EwpComputePreviewRect(float ease);

static void EwpDrawToastBanner(Gdiplus::Graphics& g, int w, int h, float s) {
    float inset = 1.5f * s;
    Gdiplus::RectF rc(inset, inset, std::max(2.0f, (float)w - inset * 2.0f), std::max(2.0f, (float)h - inset * 2.0f));
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, rc, rc.Height * 0.5f);
    Gdiplus::SolidBrush bgBr(Gdiplus::Color(238, 18, 20, 26));
    g.FillPath(&bgBr, &path);

    Gdiplus::FontFamily ff(L"Segoe UI");
    Gdiplus::Font font(&ff, 11.5f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::SolidBrush txtBr(Gdiplus::Color(250, 244, 247, 252));
    g.DrawString(g_ewpToastText.c_str(), -1, &font, rc, &fmt, &txtBr);
}

static void EwpRenderToastBanner() {
    if (!g_ewpToastWnd || !IsWindow(g_ewpToastWnd)) return;
    if (g_ewpToastText.empty() || !g_ewpEnabled.load() || (g_fullscreenActive && FULLSCREEN_BEHAVIOR == 0)) {
        if (IsWindowVisible(g_ewpToastWnd)) ShowWindow(g_ewpToastWnd, SW_HIDE);
        return;
    }

    LARGE_INTEGER now = {}; QueryPerformanceCounter(&now);
    double freq = (double)std::max<LONGLONG>(1, g_perfFreq.QuadPart);
    float elapsedMs = (float)((double)(now.QuadPart - g_ewpToastStartQpc) * 1000.0 / freq);
    const float kInMs    = 160.0f;
    const float kHoldMs  = 1100.0f;
    const float kOutMs   = 200.0f;
    const float kTotalMs = kInMs + kHoldMs + kOutMs;

    if (elapsedMs >= kTotalMs) {
        g_ewpToastText.clear();
        if (IsWindowVisible(g_ewpToastWnd)) ShowWindow(g_ewpToastWnd, SW_HIDE);
        if (g_ewpPillWnd) KillTimer(g_ewpPillWnd, EWP_TIMER_TOAST);
        return;
    }

    float tAlpha = 1.0f;
    float slidePx = 0.0f;
    float s = EwpDpiScale(g_ewpToastWnd);
    if (elapsedMs < kInMs) {
        float u = EwpSmoothStep(elapsedMs / kInMs);
        tAlpha  = u;
        slidePx = (1.0f - u) * 6.0f * s;
    } else if (elapsedMs > (kInMs + kHoldMs)) {
        float u = EwpSmoothStep((elapsedMs - (kInMs + kHoldMs)) / kOutMs);
        tAlpha  = 1.0f - u;
        slidePx = -u * 6.0f * s;
    }

    int w = std::max((int)lroundf(156.0f * s), (int)lroundf((float)g_ewpToastText.size() * 7.8f * s + 28.0f * s));
    int h = (int)lroundf(28.0f * s);
    RECT anchorRc = (g_ewpPreviewOpen && g_ewpPrevActiveWsIdx >= 0)
                        ? EwpComputePreviewRect(1.0f)
                        : EwpComputeHoverDockRect(1.0f);
    MONITORINFO mi = EwpGetMonitorInfo();
    int tx = (anchorRc.left + anchorRc.right) / 2 - w / 2;
    int ty = anchorRc.top - h - (int)lroundf(8.0f * s) + (int)lroundf(slidePx);
    if (ty < mi.rcWork.top + 6) {
        ty = anchorRc.bottom + (int)lroundf(8.0f * s) + (int)lroundf(slidePx);
    }
    tx = std::max<LONG>(mi.rcWork.left + 6, std::min<LONG>(tx, mi.rcWork.right - w - 6));
    RECT rc = { tx, ty, tx + w, ty + h };

    BYTE alpha = (BYTE)std::max(0, std::min(255, (int)lroundf(245.0f * tAlpha)));
    EwpPresentLayered(g_ewpToastWnd, rc, alpha, EwpDrawToastBanner);
    if (!IsWindowVisible(g_ewpToastWnd) && alpha > 4) {
        SetWindowPos(g_ewpToastWnd, HWND_TOPMOST, rc.left, rc.top, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
}

static void EwpShowToast(const wchar_t* text) {
    if (!text || !text[0]) return;
    g_ewpToastText = text;
    LARGE_INTEGER now = {}; QueryPerformanceCounter(&now);
    g_ewpToastStartQpc = now.QuadPart;
    EwpRenderToastBanner();
    if (g_ewpPillWnd) SetTimer(g_ewpPillWnd, EWP_TIMER_TOAST, 16, NULL);
}

static void EwpDestroyPreviewCache() {
    if (g_ewpPrevDC && g_ewpPrevOldBmp) { SelectObject(g_ewpPrevDC, g_ewpPrevOldBmp); g_ewpPrevOldBmp = NULL; }
    if (g_ewpPrevDC)  { DeleteDC(g_ewpPrevDC);      g_ewpPrevDC  = NULL; }
    if (g_ewpPrevDIB) { DeleteObject(g_ewpPrevDIB); g_ewpPrevDIB = NULL; }
    g_ewpPrevBits = NULL;
    g_ewpPrevW = g_ewpPrevH = 0;
}

static RECT EwpComputePreviewRect(float ease) {
    float s = EwpDpiScale(g_ewpPreviewWnd);
    int cardW = (int)lroundf(g_ewpPreviewWidth * s);
    int cardH = (int)lroundf(g_ewpPreviewHeight * s);
    RECT dRc = EwpComputeHoverDockRect(ease);
    MONITORINFO mi = EwpGetMonitorInfo();
    int previewGap = (int)lroundf(g_ewpArcToPreviewGap * s);
    int slide = (int)lroundf((1.0f - ease) * 10.0f * s);

    int px = 0, py = 0;
    if (g_ewpCurEdge == EWP_EDGE_LEFT) {
        px = dRc.right + previewGap - slide;
        py = (dRc.top + dRc.bottom) / 2 - cardH / 2;
    } else if (g_ewpCurEdge == EWP_EDGE_RIGHT) {
        px = dRc.left - previewGap - cardW + slide;
        py = (dRc.top + dRc.bottom) / 2 - cardH / 2;
    } else if (g_ewpCurEdge == EWP_EDGE_TOP) {
        px = (dRc.left + dRc.right) / 2 - cardW / 2;
        py = dRc.bottom + previewGap - slide;
    } else { // EWP_EDGE_BOTTOM
        px = (dRc.left + dRc.right) / 2 - cardW / 2;
        py = dRc.top - previewGap - cardH + slide;
    }
    px = std::max<LONG>(mi.rcWork.left + 6, std::min<LONG>(px, mi.rcWork.right - cardW - 6));
    py = std::max<LONG>(mi.rcWork.top + 6,  std::min<LONG>(py, mi.rcWork.bottom - cardH - 6));
    return RECT{ px, py, px + cardW, py + cardH };
}

static void EwpRenderSchematicPreview() {
    if (!g_ewpPreviewWnd || !IsWindow(g_ewpPreviewWnd)) return;
    float ease = EwpSmoothStep(g_ewpAnimProg);

    if (!g_ewpPreviewOpen || g_ewpPrevActiveWsIdx < 0 || ease <= 0.01f || g_ewpDockState == EWP_DOCK_CLOSED) {
        if (IsWindowVisible(g_ewpPreviewWnd)) ShowWindow(g_ewpPreviewWnd, SW_HIDE);
        return;
    }

    EwpLayoutWorkspace ws;
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    if (g_ewpPrevActiveWsIdx >= (int)g_ewpWorkspaces.size()) {
        if (g_csInitialized) LeaveCriticalSection(&g_cs);
        if (IsWindowVisible(g_ewpPreviewWnd)) ShowWindow(g_ewpPreviewWnd, SW_HIDE);
        return;
    }
    ws = g_ewpWorkspaces[g_ewpPrevActiveWsIdx];
    if (g_csInitialized) LeaveCriticalSection(&g_cs);

    float s = EwpDpiScale(g_ewpPreviewWnd);
    int cardW = (int)lroundf(g_ewpPreviewWidth * s);
    int cardH = (int)lroundf(g_ewpPreviewHeight * s);

    if (!g_ewpPrevDC || !g_ewpPrevBits || g_ewpPrevW != cardW || g_ewpPrevH != cardH) {
        EwpDestroyPreviewCache();
        HDC sdc = GetDC(NULL);
        if (!sdc) return;
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = cardW;
        bi.bmiHeader.biHeight = -cardH;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        g_ewpPrevDIB = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, &g_ewpPrevBits, NULL, 0);
        g_ewpPrevDC  = g_ewpPrevDIB ? CreateCompatibleDC(sdc) : NULL;
        if (g_ewpPrevDC && g_ewpPrevDIB && g_ewpPrevBits) {
            g_ewpPrevOldBmp = (HBITMAP)SelectObject(g_ewpPrevDC, g_ewpPrevDIB);
            g_ewpPrevW = cardW;
            g_ewpPrevH = cardH;
        }
        ReleaseDC(NULL, sdc);
        g_ewpPrevChipIdx = -1;
    }
    if (!g_ewpPrevDC || !g_ewpPrevBits) return;

    int effectiveHoverApp = g_ewpPreviewEditMode ? g_ewpPrevHoverAppIdx : g_ewpHoverSubApp;
    bool needRedraw = (g_ewpPrevActiveWsIdx != g_ewpPrevChipIdx) ||
                      (effectiveHoverApp != g_ewpPrevRenderHoverApp) ||
                      (g_ewpPrevHoverDelApp != g_ewpPrevRenderDelApp) ||
                      (g_ewpPrevHoverAddApp != g_ewpPrevRenderHoverAdd) ||
                      (g_ewpPrevSelectedApp != g_ewpPrevRenderSelApp) ||
                      (g_ewpPreviewEditMode != g_ewpPrevRenderEditMode);

    if (needRedraw) {
        memset(g_ewpPrevBits, 0, (size_t)cardW * cardH * 4);
        Gdiplus::Graphics g(g_ewpPrevDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        Gdiplus::FontFamily ff(L"Segoe UI");
        Gdiplus::Font pctFont(&ff, 11.0f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::Font delFont(&ff, 12.0f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::StringFormat fmtC;
        fmtC.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmtC.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        float totalArea = 0.0f;
        for (const auto& a : ws.apps) totalArea += std::max(0.01f, a.nw * a.nh);
        if (totalArea <= 0.0f) totalArea = 1.0f;

        static const Gdiplus::Color kBlockFills[4] = {
            Gdiplus::Color(240, 38, 32, 30),
            Gdiplus::Color(240, 25, 34, 44),
            Gdiplus::Color(240, 26, 39, 34),
            Gdiplus::Color(240, 36, 30, 44)
        };
        static const Gdiplus::Color kBlockHoverFills[4] = {
            Gdiplus::Color(250, 54, 44, 41),
            Gdiplus::Color(250, 36, 49, 64),
            Gdiplus::Color(250, 36, 54, 47),
            Gdiplus::Color(250, 50, 41, 60)
        };

        std::vector<EwpPreviewBlockGeom> blocks;
        EwpBuildPreviewBlocks(s, cardW, cardH, ws, &blocks);
        Gdiplus::SolidBrush txtBr(Gdiplus::Color(252, 246, 249, 255));

        for (size_t i = 0; i < blocks.size() && i < ws.apps.size(); ++i) {
            const auto& a = ws.apps[i];
            const Gdiplus::RectF& bRc = blocks[i].blockRc;
            bool isHovered  = ((int)i == effectiveHoverApp);
            bool isSelected = (g_ewpPreviewEditMode && (int)i == g_ewpPrevSelectedApp);
            bool isDelHot   = (g_ewpPreviewEditMode && (int)i == g_ewpPrevHoverDelApp);

            float blockRadius = (std::min(bRc.Width, bRc.Height) * 0.5f) *
                                (std::max(15, std::min(100, g_ewpPreviewRoundness)) / 100.0f);
            Gdiplus::GraphicsPath bPath;
            AddRoundedRectPath(bPath, bRc, blockRadius);
            Gdiplus::SolidBrush bFill((isHovered || isSelected) ? kBlockHoverFills[i % 4] : kBlockFills[i % 4]);
            g.FillPath(&bFill, &bPath);

            // 1. Captured application icon in clearly visible state
            int pct = (int)lroundf(((a.nw * a.nh) / totalArea) * 100.0f);
            pct = std::max(1, std::min(100, pct));
            std::wstring lbl = std::to_wstring(pct) + L"%";

            float iconSz = std::min({ bRc.Width * 0.44f, bRc.Height * 0.44f, 36.0f * s });
            iconSz = std::max(16.0f * s, iconSz);
            float badgeH = 18.0f * s;
            float badgeW = std::min(bRc.Width - 6.0f * s, std::max(34.0f * s, (float)lbl.size() * 8.5f * s + 10.0f * s));

            Gdiplus::Bitmap* appBmp = EwpGetCachedAppIcon(a.exePath);
            if (appBmp) {
                float iconY = bRc.Y + std::max(4.0f * s, (bRc.Height - (iconSz + badgeH * 0.55f)) * 0.5f);
                Gdiplus::RectF iconRc(bRc.X + (bRc.Width - iconSz) * 0.5f, iconY, iconSz, iconSz);
                float iconAlpha = (isHovered || isSelected) ? 0.94f : 0.78f;
                EwpDrawIconBitmapAlpha(g, appBmp, iconRc, iconAlpha);
            }

            // 2. High-contrast borderless dark rounded pill badge for the ratio ("50%", "50%")
            float badgeX = bRc.X + (bRc.Width - badgeW) * 0.5f;
            float badgeY = bRc.Y + bRc.Height * 0.5f + (appBmp ? (iconSz * 0.14f) : (-badgeH * 0.5f));
            if (badgeY + badgeH > bRc.GetBottom() - 3.0f * s)
                badgeY = bRc.GetBottom() - badgeH - 3.0f * s;
            Gdiplus::RectF badgeRc(badgeX, badgeY, badgeW, badgeH);
            Gdiplus::GraphicsPath badgePath;
            AddRoundedRectPath(badgePath, badgeRc, badgeH * 0.5f);
            Gdiplus::SolidBrush badgeFill(isSelected
                ? Gdiplus::Color(225, 198, 118, 82)
                : Gdiplus::Color(205, 10, 12, 16));
            g.FillPath(&badgeFill, &badgePath);
            g.DrawString(lbl.c_str(), -1, &pctFont, badgeRc, &fmtC, &txtBr);

            // 3. In Customize Mode (opened via single/double right-click), render per-application 'x'
            //    delete badge in the top-right corner of each block. Matches the '+' badge shape & fill!
            if (g_ewpPreviewEditMode) {
                Gdiplus::RectF delRc = EwpComputePreviewBlockDelRect(bRc, s);
                Gdiplus::SolidBrush delFill(isDelHot
                    ? Gdiplus::Color(245, 224, 62, 62)
                    : Gdiplus::Color(195, 14, 16, 22));
                g.FillEllipse(&delFill, delRc);
                g.DrawString(L"×", -1, &delFont, delRc, &fmtC, &txtBr);
            }
        }

        // 4. In Customize Mode, show the matching circular '+' Add-App badge in the bottom-right corner
        if (g_ewpPreviewEditMode && (int)ws.apps.size() < g_ewpMaxApps) {
            Gdiplus::RectF addRc = EwpComputePreviewAddBtnRect(s, cardW, cardH);
            Gdiplus::SolidBrush addFill(g_ewpPrevHoverAddApp
                ? Gdiplus::Color(245, 204, 126, 88)
                : Gdiplus::Color(195, 14, 16, 22));
            g.FillEllipse(&addFill, addRc);
            g.DrawString(L"+", -1, &delFont, addRc, &fmtC, &txtBr);
        }

        g_ewpPrevChipIdx        = g_ewpPrevActiveWsIdx;
        g_ewpPrevRenderHoverApp = effectiveHoverApp;
        g_ewpPrevRenderDelApp   = g_ewpPrevHoverDelApp;
        g_ewpPrevRenderHoverAdd = g_ewpPrevHoverAddApp;
        g_ewpPrevRenderSelApp   = g_ewpPrevSelectedApp;
        g_ewpPrevRenderEditMode = g_ewpPreviewEditMode;
    }

    RECT prc = EwpComputePreviewRect(ease);
    BYTE alpha = (BYTE)std::max(0, std::min(255, (int)lroundf(255.0f * ease)));

    static RECT s_lastPrevRc    = {};
    static BYTE s_lastPrevAlpha = 0;
    bool prevVis = IsWindowVisible(g_ewpPreviewWnd) != FALSE;
    bool prevDirty = needRedraw || !prevVis ||
                     prc.left != s_lastPrevRc.left || prc.top != s_lastPrevRc.top ||
                     prc.right != s_lastPrevRc.right || prc.bottom != s_lastPrevRc.bottom ||
                     alpha != s_lastPrevAlpha;

    if (prevDirty) {
        HDC sdc = GetDC(NULL);
        if (sdc) {
            POINT dst = { prc.left, prc.top }, src = { 0, 0 };
            SIZE sz = { cardW, cardH };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA };
            UpdateLayeredWindow(g_ewpPreviewWnd, sdc, &dst, &sz, g_ewpPrevDC, &src, 0, &bf, ULW_ALPHA);
            s_lastPrevRc    = prc;
            s_lastPrevAlpha = alpha;
            if (!prevVis && alpha > 4) {
                SetWindowPos(g_ewpPreviewWnd, HWND_TOPMOST, prc.left, prc.top, cardW, cardH,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW);
            }
            ReleaseDC(NULL, sdc);
        }
    }
}

// Double-clicking an app block inside the Schematic Thumbnail removes/unpins that app and reflows remaining apps
static void EwpRemoveAppFromWorkspace(int wsIdx, int appIdx) {
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    if (wsIdx >= 0 && wsIdx < (int)g_ewpWorkspaces.size()) {
        auto& ws = g_ewpWorkspaces[wsIdx];
        if (appIdx >= 0 && appIdx < (int)ws.apps.size()) {
            ws.apps.erase(ws.apps.begin() + appIdx);
            if (ws.apps.empty()) {
                g_ewpWorkspaces.erase(g_ewpWorkspaces.begin() + wsIdx);
                if (g_ewpFocusIdx >= (int)g_ewpWorkspaces.size())
                    g_ewpFocusIdx = std::max(0, (int)g_ewpWorkspaces.size() - 1);
                if (g_ewpScrollIdx >= (int)g_ewpWorkspaces.size())
                    g_ewpScrollIdx = std::max(0, (int)g_ewpWorkspaces.size() - 1);
            } else {
                EwpReflowWorkspaceRatios(ws, 0, 0.0f);
            }
        }
    }
    if (g_csInitialized) LeaveCriticalSection(&g_cs);

    ++g_ewpDockGeneration;
    EwpSaveWorkspaces();
    g_ewpPrevHoverAppIdx = -1;
    g_ewpPrevHoverAddApp = false;
    g_ewpPrevSelectedApp = -1;
    g_ewpPrevChipIdx     = -1;
    g_ewpHoverWsIdx      = -1;
    EwpRenderHoverDock();
}

static void EwpArrangeAppRatioInWorkspace(int wsIdx, int appIdx) {
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    if (wsIdx >= 0 && wsIdx < (int)g_ewpWorkspaces.size()) {
        auto& ws = g_ewpWorkspaces[wsIdx];
        int n = (int)ws.apps.size();
        if (n >= 2 && appIdx >= 0 && appIdx < n) {
            static const float kRatioPresets[4] = { 0.65f, 0.75f, 0.50f, 0.35f };
            float share = kRatioPresets[ws.ratioPresetState % 4];
            ws.ratioPresetState = (ws.ratioPresetState + 1) % 4;
            EwpReflowWorkspaceRatios(ws, appIdx, share);
        }
    }
    if (g_csInitialized) LeaveCriticalSection(&g_cs);

    g_ewpFocusIdx        = wsIdx;
    g_ewpPrevActiveWsIdx = wsIdx;
    g_ewpPrevSelectedApp = 0;
    g_ewpPrevChipIdx     = -1;
    ++g_ewpDockGeneration;
    EwpSaveWorkspaces();
    EwpRenderHoverDock();
}

static int EwpHitTestPreviewBlock(POINT pt, bool* outAddApp, bool* outOnDelete = nullptr) {
    if (outAddApp)   *outAddApp   = false;
    if (outOnDelete) *outOnDelete = false;
    if (g_ewpPrevActiveWsIdx < 0) return -1;
    EwpLayoutWorkspace ws;
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    if (g_ewpPrevActiveWsIdx < (int)g_ewpWorkspaces.size())
        ws = g_ewpWorkspaces[g_ewpPrevActiveWsIdx];
    if (g_csInitialized) LeaveCriticalSection(&g_cs);
    if (ws.apps.empty()) return -1;

    float s = EwpDpiScale(g_ewpPreviewWnd);
    int cardW = (int)lroundf(g_ewpPreviewWidth * s);
    int cardH = (int)lroundf(g_ewpPreviewHeight * s);

    if (g_ewpPreviewEditMode && (int)ws.apps.size() < g_ewpMaxApps) {
        Gdiplus::RectF addRc = EwpComputePreviewAddBtnRect(s, cardW, cardH);
        RECT arc = {
            (LONG)floorf(addRc.X),
            (LONG)floorf(addRc.Y),
            (LONG)ceilf(addRc.GetRight()),
            (LONG)ceilf(addRc.GetBottom())
        };
        if (PtInRect(&arc, pt)) {
            if (outAddApp) *outAddApp = true;
            return -1;
        }
    }

    std::vector<EwpPreviewBlockGeom> blocks;
    EwpBuildPreviewBlocks(s, cardW, cardH, ws, &blocks);

    for (const auto& b : blocks) {
        RECT brc = {
            (LONG)floorf(b.blockRc.X),
            (LONG)floorf(b.blockRc.Y),
            (LONG)ceilf(b.blockRc.GetRight()),
            (LONG)ceilf(b.blockRc.GetBottom())
        };
        if (PtInRect(&brc, pt)) {
            if (g_ewpPreviewEditMode && outOnDelete) {
                Gdiplus::RectF delRc = EwpComputePreviewBlockDelRect(b.blockRc, s);
                RECT drc = {
                    (LONG)floorf(delRc.X - 2.0f),
                    (LONG)floorf(delRc.Y - 2.0f),
                    (LONG)ceilf(delRc.GetRight() + 2.0f),
                    (LONG)ceilf(delRc.GetBottom() + 2.0f)
                };
                if (PtInRect(&drc, pt)) *outOnDelete = true;
            }
            return b.appIdx;
        }
    }
    return -1;
}

// Z-order Occlusion Culling + Edge-Touching / Snapped Window Filter + Active-Tab Browser Capture
struct EwpCapWin { HWND hwnd; RECT frame; bool maxed; std::wstring exe; std::wstring aumid; std::wstring cls; };
struct EwpCapCtx { HMONITOR mon; std::vector<EwpCapWin> list; };

static float EwpUncoveredFraction(const RECT& rc, const std::vector<RECT>& occluders) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 16 || h <= 16) return 0.0f;
    if (occluders.empty()) return 1.0f;
    int clear = 0;
    for (int gy = 0; gy < 8; ++gy) {
        int py = rc.top + ((gy * 2 + 1) * h) / 16;
        for (int gx = 0; gx < 8; ++gx) {
            POINT pt = { rc.left + ((gx * 2 + 1) * w) / 16, py };
            bool cov = false;
            for (const auto& o : occluders) if (PtInRect(&o, pt)) { cov = true; break; }
            if (!cov) ++clear;
        }
    }
    return clear / 64.0f;
}

static bool EwpIsEdgeTouchingWindow(const RECT& clip, const RECT& rcWork, bool isMaximized) {
    if (isMaximized) return true;
    int workW = std::max<LONG>(1, rcWork.right - rcWork.left);
    int workH = std::max<LONG>(1, rcWork.bottom - rcWork.top);
    int tolX  = std::max<int>(24, (int)lroundf(workW * 0.022f));
    int tolY  = std::max<int>(24, (int)lroundf(workH * 0.028f));

    bool touchLeft   = (abs(clip.left   - rcWork.left)   <= tolX);
    bool touchRight  = (abs(clip.right  - rcWork.right)  <= tolX);
    bool touchTop    = (abs(clip.top    - rcWork.top)    <= tolY);
    bool touchBottom = (abs(clip.bottom - rcWork.bottom) <= tolY);
    int  edgeCount   = (touchLeft ? 1 : 0) + (touchRight ? 1 : 0) + (touchTop ? 1 : 0) + (touchBottom ? 1 : 0);

    float areaRatio = ((float)(clip.right - clip.left) * (float)(clip.bottom - clip.top)) /
                      ((float)workW * (float)workH);
    return (edgeCount >= 2 && areaRatio >= 0.12f) || (edgeCount >= 1 && areaRatio >= 0.24f);
}

static BOOL CALLBACK EwpEnumCaptureProc(HWND hwnd, LPARAM lp) {
    auto* ctx = (EwpCapCtx*)lp;
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return TRUE;
    if (hwnd == g_overlayWnd || hwnd == g_inputWnd || hwnd == g_wsPopupWnd || hwnd == g_tooltipWnd ||
        hwnd == g_ewpPillWnd || hwnd == g_ewpDockWnd || hwnd == g_ewpPreviewWnd || hwnd == g_ewpDropZoneWnd)
        return TRUE;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) || (ex & WS_EX_NOACTIVATE)) return TRUE;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0)
        return TRUE;
    if (GetWindowTextLengthW(hwnd) == 0) return TRUE;
    RECT frame = {};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame))))
        GetWindowRect(hwnd, &frame);
    if (frame.right - frame.left < 60 || frame.bottom - frame.top < 40) return TRUE;
    if (MonitorFromRect(&frame, MONITOR_DEFAULTTONEAREST) != ctx->mon) return TRUE;
    std::wstring exe, aumid;
    if (!EwpResolveWindowApp(hwnd, exe, aumid) || EwpIsExcludedProcess(exe)) return TRUE;
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, 127);
    if (IsTrueSystemWindow(hwnd)) return TRUE;
    ctx->list.push_back({ hwnd, frame, IsZoomed(hwnd) != FALSE, exe, aumid, cls });
    return TRUE;
}

static std::wstring EwpReadBrowserUrl(HWND hwnd, IUIAutomation* uia) {
    if (!hwnd || !uia) return L"";
    IUIAutomationElement* root = NULL;
    if (FAILED(uia->ElementFromHandle(hwnd, &root)) || !root) return L"";
    VARIANT v; VariantInit(&v); v.vt = VT_I4; v.lVal = UIA_EditControlTypeId;
    IUIAutomationCondition* cond = NULL;
    std::wstring url;
    if (SUCCEEDED(uia->CreatePropertyCondition(UIA_ControlTypePropertyId, v, &cond)) && cond) {
        IUIAutomationElementArray* arr = NULL;
        if (SUCCEEDED(root->FindAll(TreeScope_Descendants, cond, &arr)) && arr) {
            int len = 0; arr->get_Length(&len);
            for (int i = 0; i < std::min(len, 12); ++i) {
                IUIAutomationElement* el = NULL;
                if (SUCCEEDED(arr->GetElement(i, &el)) && el) {
                    IUIAutomationValuePattern* vp = NULL;
                    if (SUCCEEDED(el->GetCurrentPatternAs(UIA_ValuePatternId, IID_PPV_ARGS(&vp))) && vp) {
                        BSTR bv = NULL;
                        if (SUCCEEDED(vp->get_CurrentValue(&bv)) && bv) {
                            std::wstring cand = EwpTrim(bv);
                            SysFreeString(bv);
                            if (!cand.empty() && cand.find(L' ') == std::wstring::npos &&
                                (cand.rfind(L"http", 0) == 0 || cand.find(L'.') != std::wstring::npos)) {
                                if (cand.rfind(L"http://", 0) != 0 && cand.rfind(L"https://", 0) != 0)
                                    cand = L"https://" + cand;
                                url = cand;
                                vp->Release(); el->Release(); break;
                            }
                        }
                        vp->Release();
                    }
                    el->Release();
                }
            }
            arr->Release();
        }
        cond->Release();
    }
    VariantClear(&v);
    root->Release();
    return url;
}

static DWORD WINAPI EwpCaptureWorkerThread(LPVOID param) {
    HMONITOR mon = (HMONITOR)param;
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) {
        mon = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
        GetMonitorInfoW(mon, &mi);
    }
    EwpCapCtx ctx = { mon, {} };
    EnumWindows(EwpEnumCaptureProc, (LPARAM)&ctx);

    IUIAutomation* uia = NULL;
    if (g_ewpCaptureUrl) CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&uia));

    auto* outWs = new EwpLayoutWorkspace();
    int workW = std::max<LONG>(1, mi.rcWork.right - mi.rcWork.left);
    int workH = std::max<LONG>(1, mi.rcWork.bottom - mi.rcWork.top);

    auto collectPass = [&](bool requireEdgeTouching) {
        outWs->apps.clear();
        std::vector<RECT> occluders;
        for (const auto& w : ctx.list) {
            if ((int)outWs->apps.size() >= g_ewpMaxApps) break;
            RECT clip = {};
            if (!IntersectRect(&clip, &w.frame, &mi.rcWork)) continue;
            float visibleFrac = EwpUncoveredFraction(clip, occluders);
            if (g_ewpIgnoreOccluded && visibleFrac < 0.14f) continue;
            occluders.push_back(clip);

            if (requireEdgeTouching && !EwpIsEdgeTouchingWindow(clip, mi.rcWork, w.maxed))
                continue;

            // Never allow duplicate applications inside the same workspace capture
            if (EwpWorkspaceHasApp(*outWs, w.exe, w.aumid))
                continue;

            EwpAppLayoutEntry e;
            e.exePath = w.exe;
            e.aumid = w.aumid;
            e.windowClass = w.cls;
            std::wstring base = BaseNameFromPath(w.exe);
            if (base.size() > 4 && _wcsicmp(base.c_str() + base.size() - 4, L".exe") == 0) base.resize(base.size() - 4);
            e.displayName = base.empty() ? L"App" : base;
            e.isMaximized = w.maxed;
            e.nx = EwpClamp01((float)(clip.left - mi.rcWork.left) / workW);
            e.ny = EwpClamp01((float)(clip.top - mi.rcWork.top) / workH);
            float rawW = EwpClamp01((float)(clip.right - clip.left) / workW);
            float rawH = EwpClamp01((float)(clip.bottom - clip.top) / workH);
            e.nw = std::max(0.10f, rawW * sqrtf(visibleFrac));
            e.nh = std::max(0.10f, rawH * sqrtf(visibleFrac));

            if (!w.aumid.empty() && w.exe.find(L"WindowsApps") != std::wstring::npos) {
                e.kind = EWP_KIND_UWP;
            } else if (g_ewpCaptureUrl && uia && EwpIsBrowserExe(w.exe) && w.aumid.find(L"PWA") == std::wstring::npos) {
                std::wstring url = EwpReadBrowserUrl(w.hwnd, uia);
                if (!url.empty()) { e.kind = EWP_KIND_URL; e.activeUrl = url; }
            }
            outWs->apps.push_back(std::move(e));
        }
    };

    bool filterEdges = (g_ewpCaptureFilter == EWP_CAP_EDGE_TOUCHING);
    collectPass(filterEdges);
    if (outWs->apps.empty() && filterEdges) {
        collectPass(false);
    }

    if (uia) uia->Release();
    if (SUCCEEDED(hr)) CoUninitialize();

    if (!outWs->apps.empty()) {
        outWs->id = MakeWorkspaceId();
        EwpNormalizeNonOverlappingLayout(*outWs, true);
    }
    if (g_ewpPillWnd && IsWindow(g_ewpPillWnd))
        PostMessageW(g_ewpPillWnd, WM_EWP_CAPTURE_DONE, 0, (LPARAM)outWs);
    else
        delete outWs;
    g_ewpCapBusy.store(false);
    return 0;
}

static void EwpTriggerCapture(int appendToWsIdx = -1) {
    if (g_ewpCapBusy.exchange(true)) return;
    g_ewpAppendTargetWs = appendToWsIdx;
    POINT pt = {}; GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (g_ewpCapThread) { WaitForSingleObject(g_ewpCapThread, INFINITE); CloseHandle(g_ewpCapThread); g_ewpCapThread = NULL; }
    g_ewpCapThread = CreateThread(NULL, 0, EwpCaptureWorkerThread, (LPVOID)mon, 0, NULL);
    if (!g_ewpCapThread) {
        g_ewpAppendTargetWs = -1;
        g_ewpCapBusy.store(false);
    }
}

static void EwpPlaceWindowGuarded(HWND hwnd, const EwpAppLayoutEntry& a, const RECT& rcWork) {
    if (!hwnd || !IsWindow(hwnd) || IsHungAppWindow(hwnd)) return;
    int workW = std::max<LONG>(1, rcWork.right - rcWork.left);
    int workH = std::max<LONG>(1, rcWork.bottom - rcWork.top);
    RECT dst = {
        rcWork.left + (LONG)lroundf(a.nx * workW),
        rcWork.top  + (LONG)lroundf(a.ny * workH),
        rcWork.left + (LONG)lroundf((a.nx + a.nw) * workW),
        rcWork.top  + (LONG)lroundf((a.ny + a.nh) * workH)
    };
    RECT winRc = {}, frameRc = {};
    if (GetWindowRect(hwnd, &winRc) &&
        SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &frameRc, sizeof(frameRc)))) {
        dst.left   -= (frameRc.left - winRc.left);
        dst.top    -= (frameRc.top - winRc.top);
        dst.right  += (winRc.right - frameRc.right);
        dst.bottom += (winRc.bottom - frameRc.bottom);
    }
    SetLastError(0);
    if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
    else if (IsZoomed(hwnd) && !a.isMaximized) ShowWindow(hwnd, SW_SHOWNORMAL);
    if (GetLastError() == ERROR_ACCESS_DENIED) return;

    if (a.isMaximized) {
        ShowWindow(hwnd, SW_SHOWNORMAL);
        SetWindowPos(hwnd, HWND_TOP, dst.left, dst.top,
                     std::max<LONG>(120, dst.right - dst.left),
                     std::max<LONG>(80, dst.bottom - dst.top),
                     SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
        ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    } else {
        SetWindowPos(hwnd, HWND_TOP, dst.left, dst.top,
                     std::max<LONG>(120, dst.right - dst.left),
                     std::max<LONG>(80, dst.bottom - dst.top),
                     SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
    }
}

static void EwpRestoreWorkspaceSlot(int idx) {
    EwpLayoutWorkspace ws;
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    if (idx < 0 || idx >= (int)g_ewpWorkspaces.size()) {
        if (g_csInitialized) LeaveCriticalSection(&g_cs);
        return;
    }
    ws = g_ewpWorkspaces[idx];
    if (g_csInitialized) LeaveCriticalSection(&g_cs);

    POINT pt = {}; GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfoW(mon, &mi)) return;

    DWORD expire = GetTickCount() + 5000;
    for (const auto& a : ws.apps) {
        HWND running = (g_ewpReuseOpen && a.kind != EWP_KIND_URL && !a.exePath.empty())
                           ? FindRunningAppWindow(a.exePath)
                           : NULL;
        if (running) {
            EwpPlaceWindowGuarded(running, a, mi.rcWork);
        } else {
            g_ewpPendingRestores.push_back({ a, mi.rcWork, expire, false });
            if ((a.kind == EWP_KIND_UWP || a.kind == EWP_KIND_PWA) && !a.aumid.empty()) {
                std::wstring st = L"shell:AppsFolder\\" + a.aumid;
                SHELLEXECUTEINFOW sei = { sizeof(sei) };
                sei.fMask = SEE_MASK_FLAG_NO_UI; sei.lpVerb = L"open"; sei.lpFile = st.c_str(); sei.nShow = SW_SHOWNORMAL;
                ShellExecuteExW(&sei);
            } else if (!a.exePath.empty() && GetFileAttributesW(a.exePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                std::wstring args = (a.kind == EWP_KIND_URL && !a.activeUrl.empty())
                                        ? (L"--new-window \"" + a.activeUrl + L"\"") : L"";
                SHELLEXECUTEINFOW sei = { sizeof(sei) };
                sei.fMask = SEE_MASK_FLAG_NO_UI; sei.lpVerb = L"open"; sei.lpFile = a.exePath.c_str();
                sei.lpParameters = args.empty() ? NULL : args.c_str();
                sei.nShow = SW_SHOWNORMAL;
                if (!ShellExecuteExW(&sei) && !args.empty()) {
                    sei.lpParameters = NULL;
                    ShellExecuteExW(&sei);
                }
            }
        }
    }
    if (!g_ewpPendingRestores.empty() && g_ewpPillWnd)
        SetTimer(g_ewpPillWnd, EWP_TIMER_RESTORE, 250, NULL);
}

static bool EwpIsNearPill(POINT pt) {
    RECT rc = EwpComputePillRect();
    int prox = (int)lroundf(g_ewpProxPx * EwpDpiScale(g_ewpPillWnd));
    InflateRect(&rc, prox, prox);
    return PtInRect(&rc, pt) != FALSE;
}

static void EwpDrawDropZone(Gdiplus::Graphics& g, int w, int h, float s) {
    Gdiplus::RectF rc(1.0f, 1.0f, (float)w - 2.0f, (float)h - 2.0f);
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, rc, 12.0f * s);
    Gdiplus::SolidBrush fill(Gdiplus::Color(242, 24, 21, 20));
    g.FillPath(&fill, &path);
    Gdiplus::FontFamily ff(L"Segoe UI");
    Gdiplus::Font font(&ff, 11.0f * s, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::StringFormat fmt;
    fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
    fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::SolidBrush txt(Gdiplus::Color(250, 244, 236, 230));
    g.DrawString(L"Drop to Pin to Edge Workspace", -1, &font, rc, &fmt, &txt);
}

static void EwpSetDropZoneVis(bool vis) {
    if (!g_ewpDropZoneWnd || !IsWindow(g_ewpDropZoneWnd)) return;
    g_ewpDropVis = vis;
    if (!vis) { ShowWindow(g_ewpDropZoneWnd, SW_HIDE); return; }
    RECT pillRc = EwpComputePillRect();
    float s = EwpDpiScale(g_ewpDropZoneWnd);
    int w = (int)lroundf(210.0f * s), h = (int)lroundf(42.0f * s);
    int cx = (pillRc.left + pillRc.right) / 2;
    RECT rc = { cx - w / 2, pillRc.bottom + 6, cx + w / 2, pillRc.bottom + 6 + h };
    EwpPresentLayered(g_ewpDropZoneWnd, rc, 255, EwpDrawDropZone);
    SetWindowPos(g_ewpDropZoneWnd, HWND_TOPMOST, rc.left, rc.top, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

class EwpOleDropTarget final : public IDropTarget {
    LONG m_ref = 1;
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDropTarget) { *ppv = static_cast<IDropTarget*>(this); AddRef(); return S_OK; }
        *ppv = NULL; return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override { ULONG r = InterlockedDecrement(&m_ref); if (!r) delete this; return r; }
    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject*, DWORD, POINTL pt, DWORD* eff) override {
        g_ewpOleActive = true;
        bool nearPill = EwpIsNearPill({ pt.x, pt.y });
        EwpSetDropZoneVis(nearPill);
        *eff = nearPill ? DROPEFFECT_LINK : DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragOver(DWORD, POINTL pt, DWORD* eff) override {
        bool nearPill = EwpIsNearPill({ pt.x, pt.y });
        EwpSetDropZoneVis(nearPill);
        *eff = nearPill ? DROPEFFECT_LINK : DROPEFFECT_NONE;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE DragLeave() override { g_ewpOleActive = false; EwpSetDropZoneVis(false); return S_OK; }
    HRESULT STDMETHODCALLTYPE Drop(IDataObject*, DWORD, POINTL, DWORD* eff) override {
        g_ewpOleActive = false; EwpSetDropZoneVis(false); *eff = DROPEFFECT_LINK;
        EwpTriggerCapture(-1);
        return S_OK;
    }
};

static void CALLBACK EwpWinEventMoveSize(HWINEVENTHOOK, DWORD ev, HWND hwnd, LONG idObj, LONG, DWORD, DWORD) {
    if (idObj != OBJID_WINDOW || !hwnd || !g_ewpPillWnd) return;
    if (ev == EVENT_SYSTEM_MOVESIZESTART) {
        g_ewpMoveSizeActive.store(true);
        PostMessageW(g_ewpPillWnd, WM_EWP_MOVESIZE_STATE, 1, (LPARAM)hwnd);
    } else if (ev == EVENT_SYSTEM_MOVESIZEEND) {
        g_ewpMoveSizeActive.store(false);
        PostMessageW(g_ewpPillWnd, WM_EWP_MOVESIZE_STATE, 0, (LPARAM)hwnd);
    }
}

static void CALLBACK EwpWinEventObjShow(HWINEVENTHOOK, DWORD ev, HWND hwnd, LONG idObj, LONG, DWORD, DWORD) {
    if (ev == EVENT_OBJECT_SHOW && idObj == OBJID_WINDOW && hwnd && g_ewpPillWnd)
        PostMessageW(g_ewpPillWnd, WM_EWP_OBJSHOW_EVENT, 0, (LPARAM)hwnd);
}

static bool EwpParseShortcut(const std::wstring& spec, UINT& mods, UINT& vk) {
    mods = vk = 0;
    std::wstring clean = EwpTrim(spec);
    if (clean.empty() || _wcsicmp(clean.c_str(), L"none") == 0) return false;
    std::wstringstream ss(clean);
    std::wstring tok;
    while (std::getline(ss, tok, L'+')) {
        tok = EwpTrim(tok);
        if (_wcsicmp(tok.c_str(), L"Alt") == 0) mods |= MOD_ALT;
        else if (_wcsicmp(tok.c_str(), L"Ctrl") == 0) mods |= MOD_CONTROL;
        else if (_wcsicmp(tok.c_str(), L"Shift") == 0) mods |= MOD_SHIFT;
        else if (_wcsicmp(tok.c_str(), L"Win") == 0) mods |= MOD_WIN;
        else if (tok.size() == 1) vk = (UINT)towupper(tok[0]);
    }
    return (mods != 0 && vk != 0);
}

static void EwpRegisterHotkeys() {
    if (!g_ewpPillWnd) return;
    UnregisterHotKey(g_ewpPillWnd, HOTKEY_EWP_TRIGGER);
    UnregisterHotKey(g_ewpPillWnd, HOTKEY_EWP_CAPTURE);
    if (!g_ewpEnabled.load()) return;
    UINT m = 0, k = 0;
    if (EwpParseShortcut(g_ewpToggleHk, m, k))
        RegisterHotKey(g_ewpPillWnd, HOTKEY_EWP_TRIGGER, m | MOD_NOREPEAT, k);
    if (EwpParseShortcut(g_ewpCaptureHk, m, k))
        RegisterHotKey(g_ewpPillWnd, HOTKEY_EWP_CAPTURE, m | MOD_NOREPEAT, k);
}

static int EwpHitTestArcDock(POINT pt, int* outWsIdx, int* outSubAppIdx, bool* outAdd, bool* outOnCardDel) {
    if (outWsIdx)     *outWsIdx     = -1;
    if (outSubAppIdx) *outSubAppIdx = -1;
    if (outAdd)       *outAdd       = false;
    if (outOnCardDel) *outOnCardDel = false;
    float s = EwpDpiScale(g_ewpDockWnd);
    std::vector<EwpLayoutWorkspace> snap;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        snap = g_ewpWorkspaces;
        LeaveCriticalSection(&g_cs);
    }
    std::vector<EwpArcSlotGeom> slots;
    EwpBuildArcSlots(s, snap, nullptr, nullptr, &slots);
    for (int i = 0; i < (int)slots.size(); ++i) {
        const auto& sl = slots[i];
        if (!sl.isAddBtn && sl.wsIdx >= 0) {
            Gdiplus::RectF delRcF = EwpComputeArcCardDelRect(sl.cardRc, s);
            RECT delRc = {
                (LONG)floorf(delRcF.X - 3.0f),
                (LONG)floorf(delRcF.Y - 3.0f),
                (LONG)ceilf(delRcF.GetRight() + 3.0f),
                (LONG)ceilf(delRcF.GetBottom() + 3.0f)
            };
            if (PtInRect(&delRc, pt)) {
                if (outWsIdx)     *outWsIdx     = sl.wsIdx;
                if (outOnCardDel) *outOnCardDel = true;
                return i;
            }
        }
        RECT rc = {
            (LONG)floorf(sl.cardRc.X),
            (LONG)floorf(sl.cardRc.Y),
            (LONG)ceilf(sl.cardRc.GetRight()),
            (LONG)ceilf(sl.cardRc.GetBottom())
        };
        if (PtInRect(&rc, pt)) {
            if (sl.isAddBtn) {
                if (outAdd) *outAdd = true;
                return i;
            }
            if (outWsIdx) *outWsIdx = sl.wsIdx;
            if (outSubAppIdx) {
                *outSubAppIdx = 0;
                for (int a = 0; a < (int)sl.appIconRcs.size(); ++a) {
                    RECT irc = {
                        (LONG)floorf(sl.appIconRcs[a].X - 2.0f),
                        (LONG)floorf(sl.appIconRcs[a].Y - 2.0f),
                        (LONG)ceilf(sl.appIconRcs[a].GetRight() + 2.0f),
                        (LONG)ceilf(sl.appIconRcs[a].GetBottom() + 2.0f)
                    };
                    if (PtInRect(&irc, pt)) {
                        *outSubAppIdx = a;
                        break;
                    }
                }
            }
            return i;
        }
    }
    return -1;
}

static void EwpStepArcWheel(short wheelDelta) {
    int total = 0;
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        total = (int)g_ewpWorkspaces.size();
        LeaveCriticalSection(&g_cs);
    }
    int maxVisWs = std::max(2, std::min(6, g_ewpArcVisibleSlots));
    // Never rotate or swap Arc Card positions when all workspaces already fit on the Arc!
    if (total <= maxVisWs) {
        g_ewpScrollIdx = 0;
        return;
    }
    int step = (wheelDelta < 0) ? 1 : -1;
    g_ewpScrollIdx   = ((g_ewpScrollIdx + step) % total + total) % total;
    g_ewpFocusIdx    = g_ewpScrollIdx;
    g_ewpHoverSlot   = -1;
    g_ewpHoverWsIdx  = g_ewpFocusIdx;
    g_ewpHoverSubApp = -1;
    g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)std::max(320, g_ewpLingerMs);
    g_ewpPrevChipIdx = -1;
    ++g_ewpDockGeneration;
    EwpRenderHoverDock();
}

static void EwpDeleteWorkspaceAt(int wsIdx) {
    int remaining = 0;
    if (g_csInitialized) EnterCriticalSection(&g_cs);
    if (wsIdx >= 0 && wsIdx < (int)g_ewpWorkspaces.size()) {
        g_ewpWorkspaces.erase(g_ewpWorkspaces.begin() + wsIdx);
        remaining = (int)g_ewpWorkspaces.size();
        if (g_ewpFocusIdx >= remaining)
            g_ewpFocusIdx = std::max(0, remaining - 1);
        if (g_ewpScrollIdx >= remaining)
            g_ewpScrollIdx = std::max(0, remaining - 1);
    }
    if (g_csInitialized) LeaveCriticalSection(&g_cs);
    ++g_ewpDockGeneration;
    EwpSaveWorkspaces();
    g_ewpHoverSlot       = -1;
    g_ewpHoverWsIdx      = -1;
    g_ewpHoverSubApp     = -1;
    g_ewpHoverArcDel     = false;
    g_ewpPreviewOpen     = false;
    g_ewpPreviewEditMode = false;
    g_ewpPrevActiveWsIdx = -1;
    g_ewpPrevChipIdx     = -1;
    EwpRenderHoverDock();
}

static void EwpOpenWorkspaceCustomizeMode(int wsIdx, int subAppIdx) {
    if (wsIdx < 0) return;
    g_ewpPreviewOpen     = true;
    g_ewpPreviewEditMode = true;
    g_ewpDockState       = EWP_DOCK_OPEN_HOLD;
    g_ewpLingerUntil     = GetTickCount64() + (ULONGLONG)std::max(1200, g_ewpLingerMs);
    g_ewpFocusIdx        = wsIdx;
    g_ewpPrevActiveWsIdx = wsIdx;
    g_ewpPrevSelectedApp = std::max(0, subAppIdx);
    g_ewpPrevChipIdx     = -1;
    ++g_ewpDockGeneration;
    EwpRenderHoverDock();
}

// Interactive Schematic Thumbnail Window Procedure (Read-Only Hover Preview + Customize Mode):
// - In Read-Only Mode (g_ewpPreviewEditMode == false):
//     * Left-click launches/restores the workspace
//     * Right-click (single or double) upgrades into Customize Mode
// - In Customize Mode (g_ewpPreviewEditMode == true):
//     * Click 'x' or double-click any app block -> removes/unpins that app and auto-reflows remaining apps
//     * Click the corner '+' badge or drag-drop an app -> adds app into this workspace
//     * Right-click any app block -> cycles its screen split ratio (65%, 75%, 50%, 35%)
//     * Drag any app block -> swaps positions/ratios with another block
static LRESULT CALLBACK EwpPreviewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_NCHITTEST: return HTCLIENT;
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEWHEEL: {
        EwpStepArcWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        return 0;
    }
    case WM_MOUSEMOVE: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)g_ewpLingerMs;

        if (!g_ewpPreviewEditMode) {
            return 0;
        }

        if (g_ewpPrevDragAppIdx >= 0 && !g_ewpPrevDragging &&
            (abs(pt.x - g_ewpPrevPressPt.x) > 6 || abs(pt.y - g_ewpPrevPressPt.y) > 6)) {
            g_ewpPrevDragging = true;
        }
        if (g_ewpPrevDragging && g_ewpPrevActiveWsIdx >= 0) {
            bool dummyAdd = false;
            int overApp = EwpHitTestPreviewBlock(pt, &dummyAdd);
            if (overApp >= 0 && overApp != g_ewpPrevDragAppIdx) {
                if (g_csInitialized) EnterCriticalSection(&g_cs);
                if (g_ewpPrevActiveWsIdx < (int)g_ewpWorkspaces.size()) {
                    auto& apps = g_ewpWorkspaces[g_ewpPrevActiveWsIdx].apps;
                    if (g_ewpPrevDragAppIdx < (int)apps.size() && overApp < (int)apps.size()) {
                        std::swap(apps[g_ewpPrevDragAppIdx].nx, apps[overApp].nx);
                        std::swap(apps[g_ewpPrevDragAppIdx].ny, apps[overApp].ny);
                        std::swap(apps[g_ewpPrevDragAppIdx].nw, apps[overApp].nw);
                        std::swap(apps[g_ewpPrevDragAppIdx].nh, apps[overApp].nh);
                        std::swap(apps[g_ewpPrevDragAppIdx], apps[overApp]);
                        g_ewpPrevDragAppIdx  = overApp;
                        g_ewpPrevSelectedApp = overApp;
                    }
                }
                if (g_csInitialized) LeaveCriticalSection(&g_cs);
                ++g_ewpDockGeneration;
                EwpSaveWorkspaces();
                g_ewpPrevChipIdx = -1;
                EwpRenderHoverDock();
            }
            return 0;
        }

        bool addApp = false, onDel = false;
        int appIdx = EwpHitTestPreviewBlock(pt, &addApp, &onDel);
        int delIdx = (onDel && appIdx >= 0) ? appIdx : -1;
        if (appIdx != g_ewpPrevHoverAppIdx || delIdx != g_ewpPrevHoverDelApp || addApp != g_ewpPrevHoverAddApp) {
            g_ewpPrevHoverAppIdx = appIdx;
            g_ewpPrevHoverDelApp = delIdx;
            g_ewpPrevHoverAddApp = addApp;
            EwpRenderSchematicPreview();
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)std::max(500, g_ewpLingerMs);
        if (!g_ewpPreviewEditMode) {
            if (g_ewpPrevActiveWsIdx >= 0) {
                EwpRestoreWorkspaceSlot(g_ewpPrevActiveWsIdx);
            }
            return 0;
        }
        g_ewpDockState = EWP_DOCK_OPEN_HOLD;
        bool addApp = false, onDel = false;
        int appIdx = EwpHitTestPreviewBlock(pt, &addApp, &onDel);
        if (addApp && g_ewpPrevActiveWsIdx >= 0) {
            EwpTriggerCapture(g_ewpPrevActiveWsIdx);
            return 0;
        }
        if (appIdx >= 0 && onDel && g_ewpPrevActiveWsIdx >= 0) {
            g_ewpPrevDragAppIdx = -1;
            g_ewpPrevDragging   = false;
            EwpRemoveAppFromWorkspace(g_ewpPrevActiveWsIdx, appIdx);
            return 0;
        }
        if (appIdx >= 0) {
            DWORD nowTick = GetTickCount();
            if (g_ewpPrevSelectedApp == appIdx && (nowTick - g_ewpPrevLastDblTick) <= 280 && g_ewpPrevActiveWsIdx >= 0) {
                g_ewpPrevLastDblTick = 0;
                g_ewpPrevDragAppIdx  = -1;
                g_ewpPrevDragging    = false;
                EwpRemoveAppFromWorkspace(g_ewpPrevActiveWsIdx, appIdx);
                return 0;
            }
            g_ewpPrevLastDblTick = nowTick;
            g_ewpPrevSelectedApp = appIdx;
            g_ewpPrevDragAppIdx  = appIdx;
            g_ewpPrevPressPt     = pt;
            g_ewpPrevDragging    = false;
            SetCapture(hwnd);
            EwpRenderSchematicPreview();
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        if (GetCapture() == hwnd) ReleaseCapture();
        g_ewpPrevDragAppIdx = -1;
        g_ewpPrevDragging   = false;
        return 0;
    }
    case WM_LBUTTONDBLCLK: {
        if (!g_ewpPreviewEditMode) return 0;
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_ewpPrevLastDblTick = GetTickCount();
        g_ewpDockState = EWP_DOCK_OPEN_HOLD;
        bool addApp = false;
        int appIdx = EwpHitTestPreviewBlock(pt, &addApp);
        if (appIdx >= 0 && !addApp && g_ewpPrevActiveWsIdx >= 0) {
            EwpRemoveAppFromWorkspace(g_ewpPrevActiveWsIdx, appIdx);
        }
        return 0;
    }
    case WM_RBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (!g_ewpPreviewEditMode && g_ewpPrevActiveWsIdx >= 0) {
            EwpOpenWorkspaceCustomizeMode(g_ewpPrevActiveWsIdx, 0);
            return 0;
        }
        bool addApp = false;
        g_ewpPrevRPressApp = EwpHitTestPreviewBlock(pt, &addApp);
        return 0;
    }
    case WM_RBUTTONUP: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (!g_ewpPreviewEditMode && g_ewpPrevActiveWsIdx >= 0) {
            EwpOpenWorkspaceCustomizeMode(g_ewpPrevActiveWsIdx, 0);
            return 0;
        }
        bool addApp = false;
        int appIdx = EwpHitTestPreviewBlock(pt, &addApp);
        if (appIdx >= 0 && appIdx == g_ewpPrevRPressApp && g_ewpPrevActiveWsIdx >= 0) {
            EwpArrangeAppRatioInWorkspace(g_ewpPrevActiveWsIdx, appIdx);
        }
        g_ewpPrevRPressApp = -1;
        return 0;
    }
    case WM_RBUTTONDBLCLK: {
        if (g_ewpPrevActiveWsIdx >= 0) {
            EwpOpenWorkspaceCustomizeMode(g_ewpPrevActiveWsIdx, 0);
        }
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK EwpHoverDockProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_NCHITTEST: return HTCLIENT;
    case WM_ERASEBKGND: return 1;
    case WM_MOUSEWHEEL: {
        EwpStepArcWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        return 0;
    }
    case WM_MOUSEMOVE: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)g_ewpLingerMs;

        int wsIdx = -1, subApp = -1;
        bool add = false, onCardDel = false;
        int slotIdx = EwpHitTestArcDock(pt, &wsIdx, &subApp, &add, &onCardDel);
        if (slotIdx != g_ewpHoverSlot || wsIdx != g_ewpHoverWsIdx ||
            subApp != g_ewpHoverSubApp || add != g_ewpHoverAdd || onCardDel != g_ewpHoverArcDel) {
            g_ewpHoverSlot   = slotIdx;
            g_ewpHoverWsIdx  = wsIdx;
            g_ewpHoverSubApp = subApp;
            g_ewpHoverAdd    = add;
            g_ewpHoverArcDel = onCardDel;
            if (!g_ewpPreviewEditMode && wsIdx >= 0) {
                g_ewpFocusIdx    = wsIdx;
                g_ewpPrevChipIdx = -1;
            }
            EwpRenderHoverDock();
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)std::max(450, g_ewpLingerMs);
        int wsIdx = -1, subApp = -1;
        bool add = false, onCardDel = false;
        EwpHitTestArcDock(pt, &wsIdx, &subApp, &add, &onCardDel);
        g_ewpPressWsIdx     = wsIdx;
        g_ewpPressIsAdd     = add;
        g_ewpPressOnCardDel = onCardDel;
        g_ewpPressPt        = pt;
        SetCapture(hwnd);
        return 0;
    }
    case WM_LBUTTONUP: {
        if (GetCapture() == hwnd) ReleaseCapture();
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int pressedWs        = g_ewpPressWsIdx;
        bool pressedAdd      = g_ewpPressIsAdd;
        bool pressedCardDel  = g_ewpPressOnCardDel;
        POINT pressPt        = g_ewpPressPt;

        g_ewpPressWsIdx     = -1;
        g_ewpPressIsAdd     = false;
        g_ewpPressOnCardDel = false;

        int releaseWs = -1, releaseSub = -1;
        bool releaseAdd = false, releaseCardDel = false;
        EwpHitTestArcDock(pt, &releaseWs, &releaseSub, &releaseAdd, &releaseCardDel);
        bool cleanClick = (abs(pt.x - pressPt.x) <= 8 && abs(pt.y - pressPt.y) <= 8);

        if (cleanClick) {
            if (pressedAdd && releaseAdd) {
                EwpTriggerCapture(-1);
            } else if (pressedCardDel && releaseCardDel && pressedWs >= 0 && releaseWs == pressedWs) {
                // Top-right 'x' badge on Arc Card clicked -> remove saved workspace state immediately!
                EwpDeleteWorkspaceAt(pressedWs);
            } else if (!pressedCardDel && !releaseCardDel && pressedWs >= 0 && releaseWs == pressedWs) {
                // Left-clicking the Arc Card body/icons restores/launches the workspace immediately!
                g_ewpFocusIdx = pressedWs;
                EwpRestoreWorkspaceSlot(pressedWs);
                EwpRenderHoverDock();
            }
        }
        return 0;
    }
    case WM_LBUTTONDBLCLK: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int wsIdx = -1, subApp = -1;
        bool add = false, onCardDel = false;
        EwpHitTestArcDock(pt, &wsIdx, &subApp, &add, &onCardDel);
        if (wsIdx >= 0 && !add && !onCardDel) {
            EwpOpenWorkspaceCustomizeMode(wsIdx, subApp);
        }
        return 0;
    }
    case WM_RBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int wsIdx = -1, subApp = -1;
        bool add = false, onCardDel = false;
        EwpHitTestArcDock(pt, &wsIdx, &subApp, &add, &onCardDel);
        g_ewpRPressWsIdx = (!add && !onCardDel && wsIdx >= 0) ? wsIdx : -1;
        if (g_ewpRPressWsIdx >= 0) {
            // Single right-click or double right-click opens Customize Mode in Thumbnail Preview!
            EwpOpenWorkspaceCustomizeMode(g_ewpRPressWsIdx, subApp);
        }
        return 0;
    }
    case WM_RBUTTONUP: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int wsIdx = -1, subApp = -1;
        bool add = false, onCardDel = false;
        EwpHitTestArcDock(pt, &wsIdx, &subApp, &add, &onCardDel);
        if (!add && !onCardDel && wsIdx >= 0) {
            EwpOpenWorkspaceCustomizeMode(wsIdx, subApp);
        }
        g_ewpRPressWsIdx = -1;
        return 0;
    }
    case WM_RBUTTONDBLCLK: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        int wsIdx = -1, subApp = -1;
        bool add = false, onCardDel = false;
        EwpHitTestArcDock(pt, &wsIdx, &subApp, &add, &onCardDel);
        if (!add && !onCardDel && wsIdx >= 0) {
            EwpOpenWorkspaceCustomizeMode(wsIdx, subApp);
        }
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK EwpPillProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_NCHITTEST: return HTCLIENT;
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE:
        return 0;
    case WM_MOUSEWHEEL: {
        if (g_ewpDockState == EWP_DOCK_CLOSED || g_ewpDockState == EWP_DOCK_CLOSING) {
            LARGE_INTEGER now = {}; QueryPerformanceCounter(&now);
            g_ewpAnimStartQpc = now.QuadPart;
            g_ewpDockState = EWP_DOCK_OPENING;
            SetTimer(hwnd, EWP_TIMER_MOTION, 12, NULL);
        }
        EwpStepArcWheel(GET_WHEEL_DELTA_WPARAM(wParam));
        return 0;
    }
    case WM_MOUSEMOVE: {
        POINT pt = {}; GetCursorPos(&pt);
        if (g_ewpPillPress && !g_ewpPillDrag &&
            (abs(pt.x - g_ewpPillPressPt.x) > 4 || abs(pt.y - g_ewpPillPressPt.y) > 4)) {
            g_ewpPillDrag = true;
            g_ewpAnimProg = 0.0f;
            g_ewpDockState = EWP_DOCK_CLOSED;
            if (g_ewpDockWnd) ShowWindow(g_ewpDockWnd, SW_HIDE);
            if (g_ewpPreviewWnd) ShowWindow(g_ewpPreviewWnd, SW_HIDE);
        }
        if (g_ewpPillDrag) {
            g_ewpMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = EwpGetMonitorInfo();
            int w = g_ewpPillDragRc.right - g_ewpPillDragRc.left;
            int h = g_ewpPillDragRc.bottom - g_ewpPillDragRc.top;
            int nx = std::max<LONG>(mi.rcWork.left, std::min<LONG>(pt.x - g_ewpPillAnchor.x, mi.rcWork.right - w));
            int ny = std::max<LONG>(mi.rcWork.top,  std::min<LONG>(pt.y - g_ewpPillAnchor.y, mi.rcWork.bottom - h));
            g_ewpPillDragRc = { nx, ny, nx + w, ny + h };
            SetWindowPos(hwnd, HWND_TOPMOST, nx, ny, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW);
            return 0;
        }
        g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)g_ewpLingerMs;
        if (g_ewpDockState == EWP_DOCK_CLOSED || g_ewpDockState == EWP_DOCK_CLOSING) {
            LARGE_INTEGER now = {}; QueryPerformanceCounter(&now);
            LONGLONG elapsedTicks = (LONGLONG)(g_ewpAnimProg * 0.210 * (double)std::max<LONGLONG>(1, g_perfFreq.QuadPart));
            g_ewpAnimStartQpc = now.QuadPart - elapsedTicks;
            g_ewpDockState = EWP_DOCK_OPENING;
            SetTimer(hwnd, EWP_TIMER_MOTION, 12, NULL);
        }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        POINT pt = {}; GetCursorPos(&pt);
        RECT rc = EwpComputePillRect();
        g_ewpPillPress = true; g_ewpPillDrag = false;
        g_ewpPillPressPt = pt; g_ewpPillDragRc = rc;
        g_ewpPillAnchor = { pt.x - rc.left, pt.y - rc.top };
        SetCapture(hwnd);
        return 0;
    }
    case WM_LBUTTONUP: {
        if (GetCapture() == hwnd) ReleaseCapture();
        bool dragged = g_ewpPillDrag;
        g_ewpPillPress = g_ewpPillDrag = false;
        if (dragged) {
            POINT pt = {}; GetCursorPos(&pt);
            g_ewpMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi = EwpGetMonitorInfo();
            int dT = abs(pt.y - mi.rcWork.top), dB = abs(mi.rcWork.bottom - pt.y);
            int dL = abs(pt.x - mi.rcWork.left), dR = abs(mi.rcWork.right - pt.x);
            int m = std::min({ dT, dB, dL, dR });
            g_ewpCurEdge = (m == dT) ? EWP_EDGE_TOP : (m == dB ? EWP_EDGE_BOTTOM : (m == dL ? EWP_EDGE_LEFT : EWP_EDGE_RIGHT));
            int ww = std::max<LONG>(1, mi.rcWork.right - mi.rcWork.left);
            int wh = std::max<LONG>(1, mi.rcWork.bottom - mi.rcWork.top);
            g_ewpOffsetRatio = (g_ewpCurEdge == EWP_EDGE_TOP || g_ewpCurEdge == EWP_EDGE_BOTTOM)
                                   ? EwpClamp01((float)(pt.x - mi.rcWork.left) / ww)
                                   : EwpClamp01((float)(pt.y - mi.rcWork.top) / wh);
            EwpSavePlacement();
            EwpRenderPill();
        }
        return 0;
    }
    case WM_TIMER: {
        if (wParam == EWP_TIMER_INIT_RENDER) {
            KillTimer(hwnd, EWP_TIMER_INIT_RENDER);
            EwpRenderPill();
            return 0;
        }
        if (wParam == EWP_TIMER_MOTION) {
            LARGE_INTEGER now = {}; QueryPerformanceCounter(&now);
            double freq = (double)std::max<LONGLONG>(1, g_perfFreq.QuadPart);
            float ms = (float)((double)(now.QuadPart - g_ewpAnimStartQpc) * 1000.0 / freq);

            if (g_ewpDockState == EWP_DOCK_OPENING) {
                g_ewpAnimProg = EwpClamp01(ms / 210.0f);
                EwpRenderHoverDock();
                if (g_ewpAnimProg >= 0.999f) {
                    g_ewpAnimProg = 1.0f;
                    g_ewpDockState = EWP_DOCK_OPEN_HOVER;
                    g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)g_ewpLingerMs;
                }
            } else if (g_ewpDockState == EWP_DOCK_OPEN_HOVER || g_ewpDockState == EWP_DOCK_OPEN_HOLD) {
                POINT pt = {}; GetCursorPos(&pt);
                RECT pRc = EwpComputePillRect();
                RECT dRc = EwpComputeHoverDockRect(1.0f);
                RECT vRc = EwpComputePreviewRect(1.0f);
                InflateRect(&pRc, 24, 24);
                InflateRect(&dRc, 28, 28);
                InflateRect(&vRc, 28, 28);

                RECT pillDockBridge = {};
                UnionRect(&pillDockBridge, &pRc, &dRc);
                RECT dockPrevBridge = {};
                UnionRect(&dockPrevBridge, &dRc, &vRc);

                bool prevVis = IsWindowVisible(g_ewpPreviewWnd) != FALSE;
                bool insideDockOrPill = PtInRect(&pRc, pt) || PtInRect(&dRc, pt) || PtInRect(&pillDockBridge, pt);
                bool insidePreview    = prevVis && (PtInRect(&vRc, pt) || PtInRect(&dockPrevBridge, pt));
                bool inside           = insideDockOrPill || insidePreview;

                // Clear stale Arc Card hover highlight when cursor leaves the Arc Dock in Read-Only mode
                if (!PtInRect(&dRc, pt) && !insidePreview && !g_ewpPreviewEditMode &&
                    (g_ewpHoverSlot != -1 || g_ewpHoverWsIdx != -1 || g_ewpHoverAdd || g_ewpHoverArcDel)) {
                    g_ewpHoverSlot   = -1;
                    g_ewpHoverWsIdx  = -1;
                    g_ewpHoverSubApp = -1;
                    g_ewpHoverAdd    = false;
                    g_ewpHoverArcDel = false;
                    EwpRenderHoverDock();
                }

                if (inside) {
                    g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)std::max(380, g_ewpLingerMs);
                } else if (g_ewpDockState == EWP_DOCK_OPEN_HOVER && GetTickCount64() >= g_ewpLingerUntil) {
                    g_ewpAnimStartQpc = now.QuadPart;
                    g_ewpDockState = EWP_DOCK_CLOSING;
                } else if (g_ewpDockState == EWP_DOCK_OPEN_HOLD &&
                           ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000)) &&
                           !inside) {
                    g_ewpPreviewEditMode = false;
                    g_ewpPreviewOpen     = false;
                    g_ewpAnimStartQpc    = now.QuadPart;
                    g_ewpDockState       = EWP_DOCK_CLOSING;
                }
            } else if (g_ewpDockState == EWP_DOCK_CLOSING) {
                g_ewpAnimProg = EwpClamp01(1.0f - ms / 170.0f);
                EwpRenderHoverDock();
                if (g_ewpAnimProg <= 0.001f) {
                    g_ewpAnimProg        = 0.0f;
                    g_ewpDockState       = EWP_DOCK_CLOSED;
                    g_ewpPreviewOpen     = false;
                    g_ewpPreviewEditMode = false;
                    g_ewpHoverSlot       = -1;
                    g_ewpHoverWsIdx      = -1;
                    g_ewpHoverSubApp     = -1;
                    g_ewpHoverAdd        = false;
                    g_ewpHoverArcDel     = false;
                    if (g_ewpDockWnd) ShowWindow(g_ewpDockWnd, SW_HIDE);
                    if (g_ewpPreviewWnd) ShowWindow(g_ewpPreviewWnd, SW_HIDE);
                    EwpRenderPill();
                    KillTimer(hwnd, EWP_TIMER_MOTION);
                }
            }
            return 0;
        }
        if (wParam == EWP_TIMER_PROX) {
            if (!g_ewpMoveSizeActive.load() && !g_ewpOleActive) {
                KillTimer(hwnd, EWP_TIMER_PROX);
                EwpSetDropZoneVis(false);
                return 0;
            }
            POINT pt = {}; GetCursorPos(&pt);
            bool nearPill = EwpIsNearPill(pt);
            if (nearPill != g_ewpDropVis) EwpSetDropZoneVis(nearPill);
            return 0;
        }
        if (wParam == EWP_TIMER_TOAST) {
            EwpRenderToastBanner();
            return 0;
        }
        if (wParam == EWP_TIMER_RESTORE) {
            DWORD nowTick = GetTickCount();
            for (size_t i = 0; i < g_ewpPendingRestores.size();) {
                if (g_ewpPendingRestores[i].matched || (int)(nowTick - g_ewpPendingRestores[i].expireTick) >= 0)
                    g_ewpPendingRestores.erase(g_ewpPendingRestores.begin() + i);
                else ++i;
            }
            if (g_ewpPendingRestores.empty()) KillTimer(hwnd, EWP_TIMER_RESTORE);
            return 0;
        }
        break;
    }
    case WM_EWP_DROP_APP_PATH: {
        std::wstring* droppedPathPtr = (std::wstring*)lParam;
        int targetWs = (int)wParam;
        if (droppedPathPtr) {
            std::wstring exePath = *droppedPathPtr;
            delete droppedPathPtr;
            if (!exePath.empty() && !EwpIsExcludedProcess(exePath)) {
                EwpAppLayoutEntry e;
                e.kind = EWP_KIND_EXE;
                e.exePath = exePath;
                std::wstring base = BaseNameFromPath(exePath);
                if (base.size() > 4 && _wcsicmp(base.c_str() + base.size() - 4, L".exe") == 0)
                    base.resize(base.size() - 4);
                e.displayName = base.empty() ? L"App" : base;
                e.nx = 0.0f; e.ny = 0.0f; e.nw = 1.0f; e.nh = 1.0f;

                bool duplicateBlocked = false;
                if (g_csInitialized) EnterCriticalSection(&g_cs);
                if (targetWs < 0 && g_ewpPreviewOpen && g_ewpPrevActiveWsIdx >= 0 &&
                    g_ewpPrevActiveWsIdx < (int)g_ewpWorkspaces.size()) {
                    targetWs = g_ewpPrevActiveWsIdx;
                }
                if (targetWs >= 0 && targetWs < (int)g_ewpWorkspaces.size()) {
                    auto& ws = g_ewpWorkspaces[targetWs];
                    if (EwpWorkspaceHasApp(ws, e.exePath, e.aumid)) {
                        duplicateBlocked = true;
                    } else if ((int)ws.apps.size() < g_ewpMaxApps) {
                        ws.apps.push_back(e);
                        EwpReflowWorkspaceRatios(ws, 0, 0.0f);
                        g_ewpFocusIdx        = targetWs;
                        g_ewpPrevActiveWsIdx = targetWs;
                        g_ewpPreviewOpen     = true;
                        g_ewpPreviewEditMode = true;
                        g_ewpDockState       = EWP_DOCK_OPEN_HOLD;
                    }
                } else {
                    int existingIdx = -1;
                    for (int i = 0; i < (int)g_ewpWorkspaces.size(); ++i) {
                        if (g_ewpWorkspaces[i].apps.size() == 1 &&
                            EwpWorkspaceHasApp(g_ewpWorkspaces[i], e.exePath, e.aumid)) {
                            existingIdx = i;
                            break;
                        }
                    }
                    if (existingIdx >= 0) {
                        duplicateBlocked     = true;
                        g_ewpFocusIdx        = existingIdx;
                        g_ewpScrollIdx       = existingIdx;
                        g_ewpPrevActiveWsIdx = existingIdx;
                        g_ewpPreviewOpen     = true;
                        g_ewpPreviewEditMode = true;
                        g_ewpDockState       = EWP_DOCK_OPEN_HOLD;
                    } else {
                        EwpLayoutWorkspace ws;
                        ws.id = MakeWorkspaceId();
                        ws.apps.push_back(e);
                        EwpNormalizeNonOverlappingLayout(ws, false);
                        g_ewpWorkspaces.push_back(std::move(ws));
                        g_ewpFocusIdx        = (int)g_ewpWorkspaces.size() - 1;
                        g_ewpScrollIdx       = g_ewpFocusIdx;
                        g_ewpPrevActiveWsIdx = g_ewpFocusIdx;
                        g_ewpPreviewOpen     = true;
                        g_ewpPreviewEditMode = true;
                        g_ewpDockState       = EWP_DOCK_OPEN_HOLD;
                    }
                }
                if (g_csInitialized) LeaveCriticalSection(&g_cs);

                if (duplicateBlocked) {
                    EwpShowToast(L"Already in workspace");
                }
                if (g_ewpAnimProg < 1.0f) g_ewpAnimProg = 1.0f;
                g_ewpLingerUntil = GetTickCount64() + (ULONGLONG)std::max(800, g_ewpLingerMs);
                g_ewpPrevChipIdx = -1;
                ++g_ewpDockGeneration;
    EwpSaveWorkspaces();
                EwpRenderHoverDock();
            }
        }
        return 0;
    }
    case WM_EWP_MOVESIZE_STATE: {
        if (wParam != 0) {
            g_ewpDraggedHwnd = (HWND)lParam;
            SetTimer(hwnd, EWP_TIMER_PROX, 30, NULL);
        } else {
            KillTimer(hwnd, EWP_TIMER_PROX);
            HWND draggedWnd = g_ewpDraggedHwnd ? g_ewpDraggedHwnd : (HWND)lParam;
            g_ewpDraggedHwnd = NULL;
            POINT pt = {}; GetCursorPos(&pt);
            bool droppedOnEwp = (g_ewpDropVis && EwpIsNearPill(pt)) || EwpPtInAnyWindow(pt);
            EwpSetDropZoneVis(false);
            if (droppedOnEwp && draggedWnd && IsWindow(draggedWnd)) {
                HWND rootWnd = GetAncestor(draggedWnd, GA_ROOT);
                if (!rootWnd) rootWnd = draggedWnd;
                std::wstring exe, aumid;
                if (EwpResolveWindowApp(rootWnd, exe, aumid) && !EwpIsExcludedProcess(exe)) {
                    int targetWs = -1;
                    if (g_ewpPreviewWnd && IsWindowVisible(g_ewpPreviewWnd)) {
                        RECT prc = {};
                        if (GetWindowRect(g_ewpPreviewWnd, &prc) && PtInRect(&prc, pt))
                            targetWs = g_ewpPrevActiveWsIdx;
                    }
                    if (targetWs < 0 && g_ewpDockWnd && IsWindowVisible(g_ewpDockWnd)) {
                        RECT drc = {};
                        if (GetWindowRect(g_ewpDockWnd, &drc) && PtInRect(&drc, pt)) {
                            POINT localPt = { pt.x - drc.left, pt.y - drc.top };
                            int hitWs = -1, hitSub = -1;
                            bool hitAdd = false;
                            EwpHitTestArcDock(localPt, &hitWs, &hitSub, &hitAdd);
                            if (hitWs >= 0 && !hitAdd) targetWs = hitWs;
                        }
                    }
                    PostMessageW(hwnd, WM_EWP_DROP_APP_PATH, (WPARAM)targetWs,
                                 (LPARAM)(new std::wstring(exe)));
                    return 0;
                }
            }
            if (droppedOnEwp) EwpTriggerCapture(-1);
        }
        return 0;
    }
    case WM_EWP_OBJSHOW_EVENT: {
        HWND shown = (HWND)lParam;
        if (!shown || g_ewpPendingRestores.empty()) return 0;
        std::wstring exe, aumid;
        if (!EwpResolveWindowApp(shown, exe, aumid)) return 0;
        for (auto& slot : g_ewpPendingRestores) {
            if (!slot.matched && !slot.entry.exePath.empty() &&
                _wcsicmp(PathFindFileNameW(slot.entry.exePath.c_str()), PathFindFileNameW(exe.c_str())) == 0) {
                slot.matched = true;
                EwpPlaceWindowGuarded(shown, slot.entry, slot.targetWorkRect);
                break;
            }
        }
        return 0;
    }
    case WM_EWP_CAPTURE_DONE: {
        auto* ws = (EwpLayoutWorkspace*)lParam;
        int appendIdx = g_ewpAppendTargetWs;
        g_ewpAppendTargetWs = -1;
        if (ws) {
            if (ws->apps.empty()) {
                EwpShowToast(L"No visible windows");
            } else if (appendIdx >= 0) {
                // Append the top visible window that isn't already in the target workspace (NEVER duplicate!)
                bool addedOne = false;
                bool duplicateBlocked = false;
                if (g_csInitialized) EnterCriticalSection(&g_cs);
                if (appendIdx < (int)g_ewpWorkspaces.size()) {
                    auto& targetWs = g_ewpWorkspaces[appendIdx];
                    if ((int)targetWs.apps.size() < g_ewpMaxApps) {
                        for (const auto& cand : ws->apps) {
                            if (!EwpWorkspaceHasApp(targetWs, cand.exePath, cand.aumid)) {
                                targetWs.apps.push_back(cand);
                                addedOne = true;
                                break;
                            }
                        }
                        if (addedOne) {
                            EwpReflowWorkspaceRatios(targetWs, 0, 0.0f);
                        } else {
                            duplicateBlocked = true;
                        }
                    }
                    g_ewpFocusIdx        = appendIdx;
                    g_ewpPrevActiveWsIdx = appendIdx;
                    g_ewpPreviewOpen     = true;
                    g_ewpDockState       = EWP_DOCK_OPEN_HOLD;
                }
                if (g_csInitialized) LeaveCriticalSection(&g_cs);
                if (duplicateBlocked) {
                    EwpShowToast(L"Already in workspace");
                }
                g_ewpPrevChipIdx = -1;
                ++g_ewpDockGeneration;
    EwpSaveWorkspaces();
                EwpRenderHoverDock();
            } else {
                std::wstring newSig = EwpMakeWorkspaceSignature(*ws);
                bool handledAsDuplicate = false;
                if (g_csInitialized) EnterCriticalSection(&g_cs);

                if (!g_ewpAllowDuplicates && g_ewpDuplicatePolicy != EWP_DUP_ALLOW && !newSig.empty()) {
                    for (int i = 0; i < (int)g_ewpWorkspaces.size(); ++i) {
                        if (EwpMakeWorkspaceSignature(g_ewpWorkspaces[i]) == newSig) {
                            handledAsDuplicate = true;
                            g_ewpFocusIdx  = i;
                            g_ewpScrollIdx = i;
                            if (g_ewpDuplicatePolicy == EWP_DUP_UPDATE) {
                                g_ewpWorkspaces[i].apps = std::move(ws->apps);
                            }
                            break;
                        }
                    }
                }

                if (!handledAsDuplicate) {
                    g_ewpWorkspaces.push_back(std::move(*ws));
                    g_ewpFocusIdx  = (int)g_ewpWorkspaces.size() - 1;
                    g_ewpScrollIdx = g_ewpFocusIdx;
                }
                g_ewpPrevActiveWsIdx = g_ewpFocusIdx;
                if (g_csInitialized) LeaveCriticalSection(&g_cs);

                g_ewpPrevChipIdx = -1;
                ++g_ewpDockGeneration;
    EwpSaveWorkspaces();
                EwpRenderHoverDock();
            }
            delete ws;
        }
        return 0;
    }
    case WM_HOTKEY: {
        int hkId = (int)wParam;
        if (hkId == HOTKEY_EWP_TRIGGER) {
            LARGE_INTEGER now = {}; QueryPerformanceCounter(&now);
            g_ewpAnimStartQpc = now.QuadPart;
            if (g_ewpDockState == EWP_DOCK_CLOSED || g_ewpDockState == EWP_DOCK_CLOSING) {
                g_ewpDockState = EWP_DOCK_OPENING;
            } else {
                g_ewpDockState = EWP_DOCK_CLOSING;
            }
            SetTimer(hwnd, EWP_TIMER_MOTION, 12, NULL);
        } else if (hkId == HOTKEY_EWP_CAPTURE) {
            EwpTriggerCapture(-1);
        }
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void EwpApplySettingsOnUiThread() {
    ApplyCaptureAffinity(g_ewpPillWnd);
    ApplyCaptureAffinity(g_ewpDockWnd);
    ApplyCaptureAffinity(g_ewpPreviewWnd);
    ApplyCaptureAffinity(g_ewpDropZoneWnd);
    ApplyCaptureAffinity(g_ewpToastWnd);
    EwpRegisterHotkeys();
    EwpRenderPill();
    if (g_ewpDockState != EWP_DOCK_CLOSED) EwpRenderHoverDock();
}

static void EwpCreateWindowsOnUiThread() {
    if (g_ewpPillWnd && IsWindow(g_ewpPillWnd)) {
        EwpApplySettingsOnUiThread();
        return;
    }
    EwpPurgeDuplicatePillWindows();
    OleInitialize(NULL);
    HINSTANCE hInst = GetModHInstance();

    WNDCLASSEXW wcP = { sizeof(wcP) };
    wcP.style = CS_DBLCLKS; wcP.lpfnWndProc = EwpPillProc; wcP.hInstance = hInst;
    wcP.hCursor = LoadCursor(NULL, IDC_HAND); wcP.lpszClassName = EWP_PILL_CLASS;
    RegisterClassExW(&wcP);

    WNDCLASSEXW wcD = { sizeof(wcD) };
    wcD.style = CS_DBLCLKS; wcD.lpfnWndProc = EwpHoverDockProc; wcD.hInstance = hInst;
    wcD.hCursor = LoadCursor(NULL, IDC_HAND); wcD.lpszClassName = EWP_DOCK_CLASS;
    RegisterClassExW(&wcD);

    WNDCLASSEXW wcPr = { sizeof(wcPr) };
    wcPr.style = CS_DBLCLKS; wcPr.lpfnWndProc = EwpPreviewProc; wcPr.hInstance = hInst;
    wcPr.hCursor = LoadCursor(NULL, IDC_HAND); wcPr.lpszClassName = EWP_PREVIEW_CLASS;
    RegisterClassExW(&wcPr);

    WNDCLASSEXW wcDz = { sizeof(wcDz) };
    wcDz.lpfnWndProc = DefWindowProcW; wcDz.hInstance = hInst; wcDz.lpszClassName = EWP_DROPZONE_CLASS;
    RegisterClassExW(&wcDz);

    WNDCLASSEXW wcTs = { sizeof(wcTs) };
    wcTs.lpfnWndProc = DefWindowProcW; wcTs.hInstance = hInst; wcTs.lpszClassName = EWP_TOAST_CLASS;
    RegisterClassExW(&wcTs);

    g_ewpPillWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                   EWP_PILL_CLASS, L"", WS_POPUP, 0, 0, 96, 8, NULL, NULL, hInst, NULL);
    g_ewpDockWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                   EWP_DOCK_CLASS, L"", WS_POPUP, 0, 0, 200, 44, NULL, NULL, hInst, NULL);
    g_ewpPreviewWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                      EWP_PREVIEW_CLASS, L"", WS_POPUP, 0, 0, 256, 154, NULL, NULL, hInst, NULL);
    g_ewpDropZoneWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                       EWP_DROPZONE_CLASS, L"", WS_POPUP, 0, 0, 210, 42, NULL, NULL, hInst, NULL);
    g_ewpToastWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                    EWP_TOAST_CLASS, L"", WS_POPUP, 0, 0, 160, 28, NULL, NULL, hInst, NULL);

    if (g_ewpDropZoneWnd) {
        g_ewpDropTarget = new EwpOleDropTarget();
        RegisterDragDrop(g_ewpDropZoneWnd, g_ewpDropTarget);
    }
    g_ewpMoveHook = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                                    NULL, EwpWinEventMoveSize, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    g_ewpShowHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW,
                                    NULL, EwpWinEventObjShow, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    EwpApplySettingsOnUiThread();
    // Schedule an explicit post-pump render 60ms after GetMessageW starts so the pill is
    // guaranteed to composite on frame 0 even during cold DWM / monitor startup!
    if (g_ewpPillWnd) SetTimer(g_ewpPillWnd, EWP_TIMER_INIT_RENDER, 60, NULL);
}

static void EwpDestroyWindowsOnUiThread() {
    if (g_ewpMoveHook) { UnhookWinEvent(g_ewpMoveHook); g_ewpMoveHook = NULL; }
    if (g_ewpShowHook) { UnhookWinEvent(g_ewpShowHook); g_ewpShowHook = NULL; }
    if (g_ewpDropZoneWnd) {
        RevokeDragDrop(g_ewpDropZoneWnd);
        if (g_ewpDropTarget) { g_ewpDropTarget->Release(); g_ewpDropTarget = NULL; }
        DestroyWindow(g_ewpDropZoneWnd); g_ewpDropZoneWnd = NULL;
    }
    if (g_ewpToastWnd)   { DestroyWindow(g_ewpToastWnd);   g_ewpToastWnd = NULL; }
    if (g_ewpPreviewWnd) { DestroyWindow(g_ewpPreviewWnd); g_ewpPreviewWnd = NULL; }
    if (g_ewpDockWnd)    { DestroyWindow(g_ewpDockWnd);    g_ewpDockWnd = NULL; }
    if (g_ewpPillWnd) {
        UnregisterHotKey(g_ewpPillWnd, HOTKEY_EWP_TRIGGER);
        UnregisterHotKey(g_ewpPillWnd, HOTKEY_EWP_CAPTURE);
        DestroyWindow(g_ewpPillWnd); g_ewpPillWnd = NULL;
    }
    EwpDestroyPreviewCache();
    EwpClearIconCache();
    HINSTANCE hInst = GetModHInstance();
    UnregisterClassW(EWP_TOAST_CLASS,    hInst);
    UnregisterClassW(EWP_DROPZONE_CLASS, hInst);
    UnregisterClassW(EWP_PREVIEW_CLASS,  hInst);
    UnregisterClassW(EWP_DOCK_CLASS,     hInst);
    UnregisterClassW(EWP_PILL_CLASS,     hInst);
    OleUninitialize();
}

static DWORD WINAPI UiThreadProc(LPVOID) {
    // Force a message queue to exist before WhTool_ModUninit can PostThreadMessage() us.
    MSG probe;
    PeekMessageW(&probe, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    g_uiThreadId = GetCurrentThreadId();

    // v2.5.4 (this-round Issue 2): as a tool mod the host is windhawk-mod.exe, not
    // explorer.exe, so we no longer inherit Explorer's per-monitor-v2 DPI context.
    // A window inherits the DPI context of the thread that creates it, so set it
    // HERE before any dock window is created -- otherwise GetWindowRect / anchoring
    // are virtualized on scaled or mixed-DPI setups and the dock lands in the wrong
    // place with DWM-stretched bitmaps.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    bool ok = CreateInputOwnerWindow() && CreateOverlayWindow();
    InitGhostDIB();
    if (ok) ok = CreateGhostWindow();
    if (ok) {
        SyncKeyboardGestureHookOnUiThread();
        EwpCreateWindowsOnUiThread();

        // Snap position immediately so there is no fly-in from (0,0) at startup.
        if (!g_positionInitialized && g_cachedDockRect.left > 0) {
            g_dockCurrentX        = (float)g_cachedDockRect.left;
            g_dockCurrentY        = (float)g_cachedDockRect.top;
            g_dockTargetX         = g_dockCurrentX;
            g_dockTargetY         = g_dockCurrentY;
            g_positionInitialized = true;
        }

        // FIX (Issue 3 -- do-not-show-first): do NOT show either window from
        // the UI thread. Previously this force-showed both windows at (0,0)
        // 200x48 unconditionally, before a valid dock geometry existed. On
        // Windows 10 / left-aligned Windows 11 the UNSUPPORTED branch of
        // RefreshTaskbarCache had already run (its SW_HIDE calls were no-ops --
        // no window existed yet), and RepositionOverlay bails on
        // g_layoutUnsupported without moving/hiding, so the 1/255-alpha HTCLIENT
        // input window stayed parked in the top-left corner permanently,
        // swallowing clicks and popping the context menu on right-click. On a
        // cold start it flashed a mini dock in the top-left until Shell_TrayWnd
        // appeared.
        //
        // Visibility now belongs SOLELY to the worker: once RefreshTaskbarCache
        // has produced a valid centered-taskbar geometry (g_dockLocalW > 0), its
        // RepositionOverlay call shows the windows in the correct place. Until
        // then they stay hidden. We still let RepositionOverlay run here to seat
        // the position state, but it will not show anything (guarded on geometry
        // below).
        RepositionOverlay();
    }

    g_uiInitOk = ok;
    if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);

    // The guaranteed message pump the overlay/ghost windows never had.
    if (ok) {
        MSG msg;
        while (GetMessageW(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    // Teardown on the creating thread (FIX #2/#3). Runs on the normal WM_QUIT exit
    // AND on the early-failure path. Only the thread-affine window/class/hotkey work
    // happens here; the plain GDI/DIB/CS/icon frees stay in WhTool_ModUninit (which runs
    // this teardown's globals as harmless no-ops after it joins this thread). DIBs
    // are intentionally NOT deleted here to avoid a double-free with WhTool_ModUninit.
    if (g_kbHook) { UnhookWindowsHookEx(g_kbHook); g_kbHook = NULL; }
    if (g_overlayWnd && IsWindow(g_overlayWnd) && g_hotkeyKey != 0 && g_hotkeyMods != 0)
        UnregisterHotKey(g_overlayWnd, HOTKEY_PIN_ID);
    // v2.5.4 (this-round Issue 1): the lock-glow layer is now created on THIS UI
    // thread (prewarmed in PrewarmDragEffectWindows), so it is destroyed here too,
    // right next to the tether/vanish layers. The worker only presents into it and
    // is joined before this UI thread is asked to quit, so the handle is untouched
    // by the time we get here.
    EwpDestroyWindowsOnUiThread();
    if (g_wsPopupWnd) { DestroyWindow(g_wsPopupWnd);  g_wsPopupWnd  = NULL; }
    if (g_tooltipWnd) { DestroyWindow(g_tooltipWnd);  g_tooltipWnd  = NULL; }
    if (g_ghostWnd)  { DestroyWindow(g_ghostWnd);  g_ghostWnd  = NULL; }
    if (g_overlayWnd){ DestroyWindow(g_overlayWnd);g_overlayWnd= NULL; }
    if (g_inputWnd)  { DestroyWindow(g_inputWnd);  g_inputWnd  = NULL; }
    {
        HINSTANCE hInst = GetModHInstance();
        UnregisterClassW(INPUT_CLASS,           hInst);   // QPDockInputOwner
        UnregisterClassW(OVERLAY_CLASS,         hInst);   // QPDockOverlay
        UnregisterClassW(GHOST_CLASS,           hInst);   // QPDockGhost
        UnregisterClassW(L"QPDockWorkspacePopup", hInst);
        UnregisterClassW(L"QPDockTooltip",      hInst);
        UnregisterClassW(L"QPDockRenameDialog", hInst);
    }
    return 0;
}

// FIX (D2 polish): the setting read+clamp block was duplicated verbatim in both
// WhTool_ModInit and WhTool_ModSettingsChanged (~40 lines), which is exactly how the two
// drift apart (e.g. one reads a setting the other forgets). Factored into a single
// LoadSettings() helper that both call, so there is now one source of truth.
static void LoadSettings() {
    MAX_PINNED_APPS             = ReadIntSettingWithFallback(L"dock.maxPinnedApps", L"maxPinnedApps", 5);
    BASE_ICON_SIZE              = ReadIntSettingWithFallback(L"dock.iconSize", L"iconSize", 33);
    BASE_ICON_SPACING           = 12;
    DOCK_GAP_FROM_START         = ReadIntSettingWithFallback(L"dock.dockGapFromStart", L"dockGapFromStart", 6);
    SHOW_WORKSPACE_DIVIDER      = ReadIntSettingWithFallback(L"workspaces.showWorkspaceDivider", L"showWorkspaceDivider", 1) != 0;
    CORNER_ROUNDNESS            = ReadIntSettingWithFallback(L"dock.cornerRoundness", L"cornerRoundness", 40);
    SHOW_TOOLTIPS               = Wh_GetIntSetting(L"dock.showTooltips", 1) != 0;
    {
        std::wstring st = ReadStringSettingWithFallback(L"dock.dockStyle", nullptr);
        if (_wcsicmp(st.c_str(), L"glassBorder") == 0) {
            DOCK_STYLE = 0;
            HIDE_DOCK_BORDER = false;
        } else if (_wcsicmp(st.c_str(), L"glass") == 0) {
            DOCK_STYLE = 0;
            HIDE_DOCK_BORDER = ReadIntSettingWithFallback(L"dock.hideDockBorder", L"hideDockBorder", 1) != 0;
        } else {
            DOCK_STYLE = 1;
            HIDE_DOCK_BORDER = true;
        }
    }

    LIMIT_VISIBLE_PINS             = Wh_GetIntSetting(L"dock.limitVisiblePins", 0) != 0;
    MAX_VISIBLE_APPS_SETTING       = std::max(1, std::min(30, Wh_GetIntSetting(L"dock.maxVisibleApps", 10)));

    ENABLE_EXPLORER_WORKSPACE_PINS = ReadIntSettingWithFallback(L"workspaces.enableExplorerWorkspacePins", L"enableExplorerWorkspacePins", 1) != 0;
    MAX_WORKSPACE_PINS             = std::max(1, std::min(30, Wh_GetIntSetting(L"workspaces.maxWorkspacePins", 10)));
    MAX_VISIBLE_WORKSPACES_SETTING = std::max(1, std::min(30, ReadIntSettingWithFallback(L"dock.maxVisibleWorkspaces", L"workspaces.maxVisibleWorkspaces", 10)));
    MAX_PATHS_PER_WORKSPACE        = std::max(1, std::min(30, Wh_GetIntSetting(L"workspaces.maxPathsPerWorkspace", 15)));
    POPUP_VISIBLE_PATHS            = std::max(3, std::min(20, Wh_GetIntSetting(L"workspaces.popupVisiblePaths", 8)));
    OPEN_IN_SEPARATE_WINDOWS       = Wh_GetIntSetting(L"workspaces.openInSeparateWindows", 0) != 0;
    REOPEN_CLOSED_PATHS_ON_FOCUS   = Wh_GetIntSetting(L"workspaces.reopenClosedPathsOnFocus", 0) != 0;
    {
        std::wstring cm = ReadStringSettingWithFallback(L"workspaces.folderColorMode", nullptr);
        bool newOrdered = (_wcsicmp(cm.c_str(), L"ordered") == 0);
        bool modeChanged = (newOrdered != FOLDER_COLOR_MODE_ORDERED.load());
        FOLDER_COLOR_MODE_ORDERED = newOrdered;

        std::wstring rawPal = ReadStringSettingWithFallback(L"workspaces.folderColorPalette", nullptr);
        std::vector<std::wstring> parsed = ParseFolderColorPalette(rawPal.empty() ? DEFAULT_FOLDER_PALETTE : rawPal);

        std::vector<std::pair<std::wstring, std::wstring>> wsColorUpdates;
        if (g_csInitialized) EnterCriticalSection(&g_cs);
        bool palChanged = (parsed != g_folderPalette);
        g_folderPalette = std::move(parsed);

        if ((modeChanged || palChanged) && g_csInitialized && !g_folderPalette.empty()) {
            int wsOrd = 0;
            for (auto& a : g_pinnedApps) {
                if (a.type != PIN_WORKSPACE) continue;
                std::wstring prevColor = a.colorHex;
                if (a.workspacePaths.size() <= 1) {
                    a.colorHex.clear();
                } else if (FOLDER_COLOR_MODE_ORDERED.load()) {
                    a.colorHex = g_folderPalette[(size_t)wsOrd % g_folderPalette.size()];
                } else {
                    BYTE r = 0, g = 0, b = 0;
                    bool inPal = std::find(g_folderPalette.begin(), g_folderPalette.end(), a.colorHex) != g_folderPalette.end();
                    if (!inPal || !ParseHexColor(a.colorHex, r, g, b) || IsExcludedFolderColor(r, g, b)) {
                        a.colorHex.clear(); // reassigned below
                    }
                }
                if (a.colorHex != prevColor && !a.workspaceId.empty() && !a.colorHex.empty())
                    wsColorUpdates.push_back({ a.workspaceId, a.colorHex });
                wsOrd++;
            }
            if (!FOLDER_COLOR_MODE_ORDERED.load()) {
                for (auto& a : g_pinnedApps) {
                    if (a.type == PIN_WORKSPACE && a.workspacePaths.size() > 1 && a.colorHex.empty()) {
                        std::wstring picked = g_folderPalette[0];
                        for (const auto& c : g_folderPalette) {
                            bool used = false;
                            for (const auto& o : g_pinnedApps) {
                                if (o.type == PIN_WORKSPACE && _wcsicmp(o.colorHex.c_str(), c.c_str()) == 0) {
                                    used = true; break;
                                }
                            }
                            if (!used) { picked = c; break; }
                        }
                        a.colorHex = picked;
                        if (!a.workspaceId.empty())
                            wsColorUpdates.push_back({ a.workspaceId, a.colorHex });
                    }
                }
            }
        }
        if (g_csInitialized) LeaveCriticalSection(&g_cs);

        // Persist any mode/palette-triggered color changes to Windhawk storage outside g_cs
        for (const auto& upd : wsColorUpdates) {
            WorkspaceSnapshot snap;
            if (LoadWorkspaceSnapshot(upd.first, snap)) {
                snap.colorHex = upd.second;
                SaveWorkspaceSnapshot(snap);
            }
        }
    }

    ENABLE_REORDER              = ReadIntSettingWithFallback(L"interactions.enableReorder", L"enableReorder", 1) != 0;
    ENABLE_SCROLL_NAV           = ReadIntSettingWithFallback(L"interactions.enableScrollNav", L"enableScrollNav", 1) != 0;
    ENABLE_RAPID_UNPIN_ALL      = ReadIntSettingWithFallback(L"interactions.enableRapidUnpinAll", L"enableRapidUnpinAll", 0) != 0;

    ENABLE_KEY_GESTURES         = ReadIntSettingWithFallback(L"shortcuts.enableKeyGestures", L"enableKeyGestures", 0) != 0;
    TRIPLE_TAP_WINDOW_MS        = std::max(150, std::min(350, Wh_GetIntSetting(L"shortcuts.tripleTapWindowMs", 300)));
    ENABLE_LOCK_ANIMATION       = ReadIntSettingWithFallback(L"shortcuts.enableLockAnimation", L"enableLockAnimation", 1) != 0;
    g_hotkeyMods                = LoadHotkeyModifiersSetting() & (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);
    g_hotkeyKey                 = LoadHotkeyKeySetting();

    {
        std::wstring fs = ReadStringSettingWithFallback(L"advanced.fullscreenBehavior", nullptr);
        FULLSCREEN_BEHAVIOR = (_wcsicmp(fs.c_str(), L"allow") == 0) ? 1 : 0;
    }
    {
        std::wstring cb = ReadStringSettingWithFallback(L"advanced.captureBehavior", nullptr);
        CAPTURE_BEHAVIOR = (_wcsicmp(cb.c_str(), L"ignore") == 0) ? 1 : 0;
    }
    ENABLE_AUTOHIDE_SYNC        = ReadIntSettingWithFallback(L"advanced.autoHideSync", L"autoHideSync", 0) != 0;
    STARTUP_DELAY_MS            = ReadIntSettingWithFallback(L"advanced.startupDelay", L"startupDelay", 0);

    MAX_PINNED_APPS     = std::max(1,  std::min(50,   MAX_PINNED_APPS));
    BASE_ICON_SIZE      = std::max(16, std::min(48,   BASE_ICON_SIZE));
    CORNER_ROUNDNESS    = std::max(0,  std::min(100,  (int)CORNER_ROUNDNESS));
    STARTUP_DELAY_MS    = std::max(0,  std::min(3000, STARTUP_DELAY_MS));
    DOCK_GAP_FROM_START = std::max(0,  std::min(40,   DOCK_GAP_FROM_START));

    // Edge Pill & Workspace Arc + Workspace Capture & Hotkeys settings
    // IMPORTANT FIX: In Windhawk's SDK (`windhawk_api.h`), `Wh_GetIntSetting(PCWSTR valueName, ...)`
    // treats extra arguments as printf format arguments for `valueName`, NOT as a default value!
    // When `edgePin` is newly added onto an already-installed mod whose registry key exists,
    // `Wh_GetIntSetting` returns 0 for every unsaved `edgePin.*` key until the user opens and
    // saves Settings in Windhawk UI. We check `rawPillLen > 0` (`pillLength` range is 48..260)
    // to know whether `edgePin` has actually been written to the registry!
    int rawPillLen   = Wh_GetIntSetting(L"edgePin.pillLength");
    int rawVisProbe  = Wh_GetIntSetting(L"edgePin.arcVisibleSlots");
    bool edgePinSaved = (rawPillLen > 0 && rawVisProbe > 0);
    g_ewpEnabled     = edgePinSaved ? (Wh_GetIntSetting(L"edgePin.enableEdgePill") != 0) : true;
    {
        std::wstring es = ReadStringSettingWithFallback(L"edgePin.defaultEdge", nullptr);
        if (_wcsicmp(es.c_str(), L"top") == 0)         g_ewpDefaultEdge = EWP_EDGE_TOP;
        else if (_wcsicmp(es.c_str(), L"bottom") == 0) g_ewpDefaultEdge = EWP_EDGE_BOTTOM;
        else if (_wcsicmp(es.c_str(), L"right") == 0)  g_ewpDefaultEdge = EWP_EDGE_RIGHT;
        else                                           g_ewpDefaultEdge = EWP_EDGE_LEFT;
    }
    g_ewpRememberPos = edgePinSaved ? (Wh_GetIntSetting(L"edgePin.rememberWindowPosition") != 0) : true;
    {
        std::wstring ps = ReadStringSettingWithFallback(L"edgePin.windowSizePreset", nullptr);
        if (_wcsicmp(ps.c_str(), L"default") == 0 || _wcsicmp(ps.c_str(), L"standard") == 0)
            g_ewpSizePreset = 0;
        else if (_wcsicmp(ps.c_str(), L"large") == 0)
            g_ewpSizePreset = 1;
        else
            g_ewpSizePreset = 2; // Extra Large (135%) is the default!
    }
    int rawThick   = Wh_GetIntSetting(L"edgePin.pillThickness");
    int rawGap     = Wh_GetIntSetting(L"edgePin.pillToArcGap");
    int rawCardSz  = Wh_GetIntSetting(L"edgePin.arcCardSize");
    int rawVisSlot = Wh_GetIntSetting(L"edgePin.arcVisibleSlots");
    int rawLinger  = Wh_GetIntSetting(L"edgePin.hoverLingerMs");
    int rawProx    = Wh_GetIntSetting(L"edgePin.proximityRevealPx");

    g_ewpPillThick       = std::max(4,   std::min(24,  rawThick   > 0 ? rawThick   : 8));
    g_ewpPillLen         = std::max(48,  std::min(260, rawPillLen > 0 ? rawPillLen : 96));
    g_ewpPillToArcGap    = std::max(4,   std::min(48,  rawGap     > 0 ? rawGap     : 10));
    g_ewpArcCardSize     = std::max(32,  std::min(84,  rawCardSz  > 0 ? rawCardSz  : 48));
    g_ewpArcVisibleSlots = std::max(2,   std::min(6,   rawVisSlot > 0 ? rawVisSlot : 4));
    g_ewpLingerMs        = std::max(100, std::min(800, rawLinger  > 0 ? rawLinger  : 280));
    g_ewpProxPx          = std::max(16,  std::min(160, rawProx    > 0 ? rawProx    : 48));

    g_ewpToggleHk = ReadStringSettingWithFallback(L"edgeWorkspaces.toggleDockHotkey", L"hotkeys.toggleDockHotkey");
    if (g_ewpToggleHk.empty()) g_ewpToggleHk = L"Alt+W";
    g_ewpCaptureHk = ReadStringSettingWithFallback(L"edgeWorkspaces.captureWorkspaceHotkey", L"hotkeys.captureWorkspaceHotkey");
    if (g_ewpCaptureHk.empty()) g_ewpCaptureHk = L"Alt+Shift+W";

    {
        std::wstring cf = ReadStringSettingWithFallback(L"edgeWorkspaces.captureWindowFilter", L"edgePin.captureWindowFilter");
        if (_wcsicmp(cf.c_str(), L"allVisible") == 0) g_ewpCaptureFilter = EWP_CAP_ALL_VISIBLE;
        else                                          g_ewpCaptureFilter = EWP_CAP_EDGE_TOUCHING;
    }
    int rawMaxApps  = Wh_GetIntSetting(L"edgeWorkspaces.maxAppsPerWorkspace");
    bool edgeWsSaved = (rawMaxApps > 0);
    g_ewpAllowDuplicates = edgeWsSaved ? (Wh_GetIntSetting(L"edgeWorkspaces.allowDuplicateWorkspaces") != 0) : false;
    {
        std::wstring da = ReadStringSettingWithFallback(L"edgeWorkspaces.duplicateWorkspaceAction", L"edgePin.duplicateWorkspaceAction");
        if (_wcsicmp(da.c_str(), L"update") == 0)     g_ewpDuplicatePolicy = EWP_DUP_UPDATE;
        else if (_wcsicmp(da.c_str(), L"allow") == 0) g_ewpDuplicatePolicy = EWP_DUP_ALLOW;
        else                                          g_ewpDuplicatePolicy = EWP_DUP_IGNORE;
    }
    g_ewpMaxApps        = std::max(2, std::min(12, rawMaxApps > 0 ? rawMaxApps : 8));
    g_ewpIgnoreOccluded = edgeWsSaved ? (Wh_GetIntSetting(L"edgeWorkspaces.ignoreOccludedWindows") != 0) : true;
    g_ewpCaptureUrl     = edgeWsSaved ? (Wh_GetIntSetting(L"edgeWorkspaces.captureBrowserActiveTab") != 0) : true;
    g_ewpReuseOpen      = edgeWsSaved ? (Wh_GetIntSetting(L"edgeWorkspaces.reuseOpenWindows") != 0) : true;
    {
        std::wstring rawEx = ReadStringSettingWithFallback(L"edgeWorkspaces.excludedProcesses", L"edgePin.excludedProcesses");
        for (wchar_t& c : rawEx) if (c == L';' || c == L'|') c = L',';
        std::vector<std::wstring> parsedEx;
        std::wstringstream ss(rawEx);
        std::wstring item;
        while (std::getline(ss, item, L',')) {
            std::wstring tr = EwpTrim(item);
            if (!tr.empty()) parsedEx.push_back(tr);
        }
        if (g_csInitialized) EnterCriticalSection(&g_cs);
        g_ewpUserExcluded = std::move(parsedEx);
        EwpDeduplicateWorkspacesInMemory();
        if (g_csInitialized) LeaveCriticalSection(&g_cs);
    }
    g_ewpPrevChipIdx = -1;
    EwpLoadPlacement();
}

// FIX (Issue 3): the README states this mod supports Windows 11 only. On
// Windows 10 the "Start at tbr.left" heuristic makes EVERY machine resolve to
// QP_LAYOUT_UNSUPPORTED, so the mod can never draw a dock there -- yet it still
// spun up two threads and two windows. Detect the OS build with RtlGetVersion
// (the Win32 GetVersionEx is manifest-shimmed and lies on unmanifested hosts;
// RtlGetVersion reports the true build) and bail before creating anything on a
// platform the mod cannot serve. Windows 11 is build >= 22000.
static bool QpIsWindows11OrGreater() {
    typedef LONG (WINAPI *RtlGetVersion_t)(PRTL_OSVERSIONINFOW);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) return true;  // can't tell -- don't block init
    RtlGetVersion_t pRtlGetVersion =
        (RtlGetVersion_t)GetProcAddress(ntdll, "RtlGetVersion");
    if (!pRtlGetVersion) return true;  // can't tell -- don't block init
    RTL_OSVERSIONINFOW vi = {};
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (pRtlGetVersion(&vi) != 0) return true;  // STATUS_SUCCESS == 0
    return (vi.dwMajorVersion > 10) ||
           (vi.dwMajorVersion == 10 && vi.dwBuildNumber >= 22000);
}

BOOL WhTool_ModInit() {
    // Read and clamp all user settings (shared with WhTool_ModSettingsChanged).
    LoadSettings();

    // v2.5.4 (Issue 2): set per-monitor-v2 DPI before the first RefreshTaskbarCache()
    // geometry probe below, matching the UI/worker threads (tool host isn't Explorer).
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // FIX (Issue 3): Windows 11 only. On older builds the mod can never produce
    // a supported layout, so return FALSE immediately -- creating NO resources
    // (WhTool_ModUninit has nothing to tear down) rather than idling two threads and
    // two hidden windows on a platform we can't serve.
    if (!QpIsWindows11OrGreater()) {
        Wh_Log(L"INIT: unsupported OS (Windows 11 / build >= 22000 required) -- dock disabled");
        return FALSE;
    }

    // SINGLE-INSTANCE: guaranteed by the tool launcher's mutex
    // (windhawk-tool-mod_<id>); no in-mod ownership probe is needed. The dock
    // stays hidden until the worker resolves valid geometry (see RepositionOverlay).

    // FIX-A8 (#11): the optional startup delay was moved OUT of WhTool_ModInit and
    // into the worker thread (see WorkerThread) so init returns promptly and
    // never blocks Explorer startup / the Windhawk Engine thread for up to 3 s.

    {
        Gdiplus::GdiplusStartupInput gdiInput;
        if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiInput, nullptr) != Gdiplus::Ok) {
            Wh_Log(L"INIT: GdiplusStartup failed");
            return FALSE;
        }
    }

    InitializeCriticalSection(&g_cs);
    g_csInitialized = true;

    // Review #4 (Bug A): guards the outstanding launch-thread list.
    InitializeCriticalSection(&g_launchCs);
    g_launchCsInit = true;
    InitializeCriticalSection(&g_restoreExecCs);
    g_restoreExecCsInit = true;
    g_launchWorkspaceStop.store(false);   // reset in case of a reload

    g_exitEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_exitEvent) { WhTool_ModUninit(); return FALSE; }

    QueryPerformanceFrequency(&g_perfFreq);  // enable sub-ms animation timing
    g_bootStartTime = GetTickCount();
    g_systemState   = STATE_BOOT;
    g_layoutUnsupported = false;   // FIX (Issue 2): re-probe layout fresh on init

    // FIX-A8 (#11): single non-blocking geometry probe. Try once so the FIRST
    // paint uses correct geometry when the taskbar/DWM are already up (the common
    // reload case). If composition isn't ready yet (cold boot) g_dockLocalW stays
    // 0 -- but we no longer Sleep-retry on the lifecycle thread: the worker's
    // 100 ms boot poll + snap-position logic recovers the geometry within one
    // cycle, so we don't block Explorer startup / the Windhawk UI here.
    RefreshTaskbarCache();
    LoadPinnedApps();
    EwpLoadPlacement();
    EwpLoadWorkspaces();

    // FIX-C1 (#1/#2/#8): all top-level windows and the pin hotkey are now
    // created / pumped / destroyed on the dedicated UI thread
    // (UiThreadProc) so they finally have a guaranteed GetMessageW/DispatchMessageW
    // pump and are torn down on their creating thread. Windhawk lifecycle callbacks
    // (this function) run on an arbitrary, non-pumping thread, so we must own one.
    // Start the UI thread and wait for it to finish creating its windows before
    // starting the worker, which immediately expects g_overlayWnd/g_inputWnd to exist.
    // (The former SetWinEventHook was already removed in FIX-A7 -- the worker poll
    // drives every geometry update, now actually dispatched by this thread's pump.)
    g_uiReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (!g_uiReadyEvent) { WhTool_ModUninit(); return FALSE; }
    g_uiThread = CreateThread(NULL, 0, UiThreadProc, NULL, 0, NULL);
    if (!g_uiThread) { WhTool_ModUninit(); return FALSE; }
    WaitForSingleObject(g_uiReadyEvent, INFINITE);
    if (!g_uiInitOk) { WhTool_ModUninit(); return FALSE; }

    g_workerThread = CreateThread(NULL, 0, WorkerThread, NULL, 0, NULL);
    if (!g_workerThread) { WhTool_ModUninit(); return FALSE; }

    // FIX(Issue 4): removed the init-thread UpdateAutoHideState() call. The worker
    // thread was just created above, and its first poll iteration calls
    // UpdateAutoHideState() itself. Calling it here too raced the worker -- both paths
    // write g_taskbarAutoHide and call RepositionOverlay() concurrently. The worker is
    // now the single owner of the auto-hide state, so there is no startup race window.

    Wh_Log(L"INIT: v2.6.0 (1.2026.1811) OK. state=%d pinned=%d reorder=%d explorerWorkspaces=%d delay=%d hotkey=0x%X+0x%X autohide=%d",
              g_systemState, (int)g_pinnedApps.size(),
              (int)ENABLE_REORDER,
              (int)ENABLE_EXPLORER_WORKSPACE_PINS,
              STARTUP_DELAY_MS,
              (UINT)g_hotkeyMods, (UINT)g_hotkeyKey, (int)ENABLE_AUTOHIDE_SYNC);
    return TRUE;
}

void WhTool_ModUninit() {
    // FIX-A7 (#4): WinEvent hook removed -- nothing to unhook.
    // FIX-C1 (#2): the pin hotkey and every window are unregistered/destroyed by
    // the UI thread itself (UiThreadProc), on the thread that created them -- see
    // the WM_QUIT + join below. Nothing here touches those windows directly.

    // FIX-B1 (#5): signal the worker thread and wait INFINITELY for it to exit.
    // The old 2 s timeout could return while the worker was still running mod
    // code (a single loop iteration can do a cross-process UIA hit-test +
    // EnumWindows/OpenProcess per window and exceed 2 s on a loaded machine);
    // Windhawk would then FreeLibrary the image out from under the still-running
    // thread and crash Explorer. Nothing the worker touches (GDI objects, g_cs,
    // windows) is freed until AFTER this join returns -- all those frees are
    // below this block. g_exitEvent is what every WaitForSingleObject in the
    // worker loop waits on, so the join can't deadlock: the loop wakes and
    // returns immediately once the event is set.
    if (g_exitEvent) {
        SetEvent(g_exitEvent);
        if (g_workerThread) {
            WaitForSingleObject(g_workerThread, INFINITE);
            CloseHandle(g_workerThread);
            g_workerThread = NULL;
        }
    }

    // FIX-B2 (#6) + review #4: signal the launch stop flag NOW, before the UI
    // thread is quit. Because LaunchWorkspaceAsync refuses to start once this is
    // set, the UI thread's "Restore Workspace" menu path can no longer spawn a
    // launch thread that would outlive the drain below. The actual join of the
    // launch threads is deferred until AFTER the UI thread has been joined (see
    // below) so no in-flight menu handler can push a new handle after the drain.
    g_launchWorkspaceStop.store(true);
    if (g_ewpCapThread) {
        WaitForSingleObject(g_ewpCapThread, INFINITE);
        CloseHandle(g_ewpCapThread);
        g_ewpCapThread = NULL;
    }

    // FIX-C1 (#1/#2/#3): now that the worker (which may cross-thread SendMessage
    // into the overlay) has exited, ask the UI thread to quit. WM_QUIT breaks its
    // GetMessageW loop; UiThreadProc then destroys every top-level window,
    // unregisters the pin hotkey and UnregisterClassW's all 7 classes ON ITS OWN
    // (creating) thread -- the only thread allowed to. We must join it here BEFORE
    // freeing any GDI/DIB/CS resources below, and before Windhawk FreeLibrary's the
    // image, so no window proc is ever entered against unmapped code. After the join
    // g_inputWnd/g_overlayWnd/g_ghostWnd/... are NULL, so the DestroyWindow /
    // UnregisterClassW lines further down run as harmless no-ops.
    if (g_uiThread) {
        if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread   = NULL;
        g_uiThreadId = 0;
    }
    if (g_uiReadyEvent) {
        CloseHandle(g_uiReadyEvent);
        g_uiReadyEvent = NULL;
    }

    // Review #4 (Bug A): now that BOTH the worker and the UI thread are joined,
    // no code path can spawn another launch thread. Drain and join every handle
    // still outstanding before the image is unmapped, so no LaunchWorkspaceThread
    // is ever executing mod code after FreeLibrary. The stop flag was set above,
    // so each thread's LaunchWorkspace body is already short-circuited; the wait
    // just reaps them.
    if (g_launchCsInit) {
        EnterCriticalSection(&g_launchCs);
        for (HANDLE t : g_launchWorkspaceThreads) {
            WaitForSingleObject(t, INFINITE);
            CloseHandle(t);
        }
        g_launchWorkspaceThreads.clear();
        LeaveCriticalSection(&g_launchCs);
        DeleteCriticalSection(&g_launchCs);
        g_launchCsInit = false;
    }
    if (g_restoreExecCsInit) {
        DeleteCriticalSection(&g_restoreExecCs);
        g_restoreExecCsInit = false;
    }

    if (g_exitEvent) {
        CloseHandle(g_exitEvent);
        g_exitEvent = NULL;
    }

    SetHighResTimer(false);  // release 1ms timer period if still held

    // Destroy GDI resources in reverse creation order
    if (g_ghostDIB)  { DeleteObject(g_ghostDIB);  g_ghostDIB  = NULL; }
    if (g_ghostWnd)  { DestroyWindow(g_ghostWnd);  g_ghostWnd  = NULL; }
    if (g_overlayWnd){ DestroyWindow(g_overlayWnd);g_overlayWnd= NULL; }
    if (g_inputWnd)  { DestroyWindow(g_inputWnd);  g_inputWnd  = NULL; }

    // FIX-B3 (#3): unregister every window class this mod registered. A class
    // registration is NOT removed when the mod DLL unloads, so without this the
    // next load's RegisterClassExW fails with ERROR_CLASS_ALREADY_EXISTS and
    // CreateWindowExW binds to the stale class whose lpfnWndProc points into the
    // now-unmapped previous image -> crash on the first message. All windows of
    // these classes have been destroyed above, so UnregisterClassW can succeed.
    // Registered with GetModHInstance() (see the RegisterClassExW sites), so we
    // must unregister against the same module handle. (Phase C will move this to
    // the dedicated window thread that owns the classes.)
    {
        HINSTANCE hInst = GetModHInstance();
        UnregisterClassW(INPUT_CLASS,          hInst);   // QPDockInputOwner
        UnregisterClassW(OVERLAY_CLASS,        hInst);   // QPDockOverlay
        UnregisterClassW(GHOST_CLASS,          hInst);   // QPDockGhost
        UnregisterClassW(L"QPDockRenameDialog", hInst);
    }

    DestroyPaintBuffer();
    DestroyTooltipResources();
    DestroyWsPopupResources();

    // Destroy all pinned-app icon handles and free memory
    if (g_csInitialized) {
        EnterCriticalSection(&g_cs);
        for (auto& a : g_pinnedApps)
            FreePinnedAppResources(a);
        g_pinnedApps.clear();
        LeaveCriticalSection(&g_cs);
        DeleteCriticalSection(&g_cs);
        g_csInitialized = false;
    }
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

// Called by Windhawk whenever the user changes mod settings in the UI.
// Re-reads all settings, then signals the worker thread to re-run geometry
// discovery rather than calling RefreshTaskbarCache/RepositionOverlay directly
// from this arbitrary thread -- those functions touch g_fixedDockWidth,
// g_systemState, g_dockCurrentX/Y, and g_cachedDockRect which the worker owns.
void WhTool_ModSettingsChanged() {
    // Runs on an ARBITRARY Windhawk thread. Do NOT read settings or touch the
    // worker-owned geometry globals (g_dockPositionLocked / g_fixedDockWidth /
    // g_lastDpiForWidth / g_cachedDockRect) here -- doing so raced the worker's
    // reads and writes (Issue 2). Just raise the pending flag; the worker (the
    // sole owner of the settings + geometry globals) re-reads and clamps every
    // setting, resets the width/position caches, and re-applies backdrop + hotkey
    // on its very next poll. The reload is idempotent, so coalescing several rapid
    // changes into a single apply is harmless. (See the SETTINGS RELOAD block at
    // the head of WorkerThread.)
    g_settingsReloadPending = true;
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

    if (!isToolModProcess) {
        WCHAR modulePath[MAX_PATH] = {};
        if (GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath))) {
            PCWSTR baseName = PathFindFileNameW(modulePath);
            if (baseName &&
                (_wcsicmp(baseName, L"windhawk-mod.exe") == 0 ||
                 _wcsicmp(baseName, L"windhawk-mod-uiaccess.exe") == 0 ||
                 _wcsicmp(baseName, L"windhawk-mod-elevated.exe") == 0)) {
                isToolModProcess = true;
                isCurrentToolModProcess = true;
            }
        }
    }

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        // EntryPoint_Hook calls ExitThread(0) on the main thread right after Wh_ModInit,
        // which abandons thread ownership of any mutex while keeping the kernel handle alive
        // until Wh_ModUninit calls CloseHandle(g_toolModProcessMutex). Therefore, process
        // uniqueness MUST be checked via ERROR_ALREADY_EXISTS (kernel handle existence)
        // with a bounded retry loop for recompile handoffs, never via WAIT_ABANDONED!
        for (int attempt = 0; attempt < 30; ++attempt) {
            SetLastError(0);
            g_toolModProcessMutex =
                CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
            if (!g_toolModProcessMutex) {
                Wh_Log(L"CreateMutex failed");
                ExitProcess(1);
            }
            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                break;
            }
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
            Sleep(100);
        }
        if (!g_toolModProcessMutex) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ReleaseMutex(g_toolModProcessMutex);
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
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

    // If Windhawk 2.0's dedicated windhawk-mod.exe host already created the tool mutex
    // kernel object, do not spawn a duplicate tool process from windhawk.exe.
    HANDLE hExistingMutex = OpenMutexW(SYNCHRONIZE, FALSE, L"windhawk-tool-mod_" WH_MOD_ID);
    if (hExistingMutex) {
        CloseHandle(hExistingMutex);
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

    if (g_toolModProcessMutex) {
        ReleaseMutex(g_toolModProcessMutex);
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

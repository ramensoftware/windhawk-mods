// ==WindhawkMod==
// @id              multiwm
// @name            MultiWM
// @description     Lightweight, low-cortisol window manager with true per-virtual-desktop layouts. 
// @version         1.13.58
// @author          meteoni
// @github          https://github.com/Meteoni
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -luuid -ldwmapi -lversion -lgdi32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MultiWM

MultiWM is a continuation of the original [Tiling Helper](https://windhawk.net/mods/tiling-helper) by [U2X1](https://github.com/u2x1).

A lightweight, low-cortisol window manager for Windows 11 with true per-virtual-desktop layouts - 
including floating - and simple, predictable controls. 

![GIF](https://raw.githubusercontent.com/Meteoni/meteoni-assets/main/MultiWM/MultiWM.gif)

## Notes
- *Windhawk 2.0 (or higher) users can change the target process (`explorer.exe` -> `windhawk.exe, windhawk-mod.exe, windhawk-mod-uiaccess.exe`) for a better experience.*
- *MultiWM's virtual-desktop notification integration is based on [Taskbar Desktop Indicator by Simon Benedict](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-desktop-indicator.wh.cpp) (MIT).*

## Features
- **First-class floating** - use Floating as a whole-workspace layout 
or float individual windows while retaining useful geometry.
- **Automatic or Manual management** - let MultiWM continuously discover 
and migrate windows, or keep explicitly tiled groups under your control.
- **True virtual-desktop layouts** - uses Windows' existing virtual desktops 
instead of implementing a separate desktop system.
- **Event-driven** - changes are signaled through native events rather than 
continuous polling.

## Layouts
- Floating
- Master + Stack (vertical or horizontal)
- Dwindle
- Columns / Rows
- Monocle

## Getting Started
Move a tiled window to **float it** or **swap it with the window underneath**. 

By default, hotkeys are available for tiling (Alt+T), changing layouts (Alt+L), setting the master window (Alt+M),
moving windows through the logical order (Alt+, / .), floating a window (Alt+F), and switching between Automatic
and Manual management (Alt+R).

A small tray indicator shows the active workspace layout and provides quick access to layouts and management mode.


## Also check out
- [Virtual Desktop Helper by u2x1](https://windhawk.net/mods/virtual-desktop-helper)
- [Taskbar Desktop Indicator by Simon Benedict](https://windhawk.net/mods/taskbar-desktop-indicator)
- [GNOME-like dynamic virtual desktops by Giggig](https://windhawk.net/mods/gnome-dynamic-desktops)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- general:
  - DefaultWindowManagementMode: automatic
    $name: Default Window Management Mode
    $description: Automatic adds windows as they appear. Manual adds them when you choose Tile Workspace.
    $options:
      - manual: Manual
      - automatic: Automatic
  - AutomaticNewWindowPosition: last_slot
    $name: Automatic Window Insertion Position
    $description: Preferred position for new or moved windows in Automatic mode. Tries another position if needed.
    $options:
      - last_slot: Last slot
      - after_focused: After focused window
  - TrayClickAction: cycle_layout
    $name: Tray Icon Click
    $description: Action when the tray icon is left-clicked.
    $options:
      - cycle_layout: Switch layout
      - context_menu: Open context menu
  $name: General

- workspace:
  - DefaultLayout: master_stack
    $name: Fallback Layout
    $description: Starting layout when no rule matches.
    $options:
      - master_stack: Master + Stack (Vertical)
      - master_stack_h: Master + Stack (Horizontal)
      - columns: Columns
      - rows: Rows
      - dwindle: Dwindle
      - monocle: Monocle (Fullscreen)
      - floating: Floating (No tiling)
  - LayoutCycle: [master_stack, master_stack_h, dwindle, columns, rows, monocle, floating]
    $name: Layout Cycle
    $description: Layouts used by Cycle Layout, in this order.
    $options:
      - master_stack: Master + Stack (Vertical)
      - master_stack_h: Master + Stack (Horizontal)
      - columns: Columns
      - rows: Rows
      - dwindle: Dwindle
      - monocle: Monocle (Fullscreen)
      - floating: Floating (No tiling)
  - InitializationRules:
      - - Layout: ""
          $name: Initial Layout
          $description: Choose a layout and at least one desktop or monitor condition. Disabled ignores this row.
          $options:
            - "": Disabled
            - master_stack: Master + Stack (Vertical)
            - master_stack_h: Master + Stack (Horizontal)
            - columns: Columns
            - rows: Rows
            - dwindle: Dwindle
            - monocle: Monocle (Fullscreen)
            - floating: Floating (No tiling)
        - DesktopNameContains: ""
          $name: Desktop Name Contains
          #! $hideIf: {Layout: ""}
          $description: Match part of a custom desktop name, ignoring case. Blank matches any desktop.
        - DesktopNumber: 0
          $name: Desktop Number
          #! $hideIf: {Layout: ""}
          #! $min: 0
          $description: Desktop position in Task View, starting at 1. 0 matches any desktop.
        - Monitor: any
          $name: Monitor
          #! $hideIf: {Layout: ""}
          $options:
            - any: Any
            - primary: Primary
            - non_primary: Non-primary
            - specific: Specific monitor
        - MonitorId: ""
          $name: Monitor ID
          #! $hideIf: {Layout: ""}
          #! $showIf: {Monitor: specific}
          $description: Paste a monitor ID from diagnostics. Blank disables this rule.
    $name: Initialization Rules
    $description: Special handling rules for specific virtual desktops / monitors.
  - Overflow:
    - WindowLimit: 0
      $name: Auto-tiling Limit
      #! $min: 0
      $description: Maximum tiled windows per monitor on each desktop. 0 means unlimited.
    - Action: float
      $name: Overflow Action
      $description: Applies when tiled windows exceed the limit or cannot fit. Created virtual desktops stay after their windows close or MultiWM is turned off.
      $options:
        - float: Float window
        - new_desktop: New desktop after current
    - TryOtherMonitors: false
      $name: Try Other Monitors First
      $description: Try another monitor before floating or changing desktop.
    - MonitorLimits:
      - - Monitor: disabled
          $name: Monitor
          $options:
            - disabled: Rule disabled
            - primary: Primary
            - non_primary: Non-primary
            - specific: Specific monitor
        - MonitorId: ""
          $name: Monitor ID
          #! $showIf: {Monitor: specific}
          $description: Paste a monitor ID from diagnostics. Blank disables this rule.
        - Limit: 0
          $name: Auto-tiling Limit
          #! $min: 0
          #! $hideIf: {Monitor: disabled}
          $description: Limit for each matching monitor per desktop. 0 means unlimited.
      $name: Per-Monitor Limits
      $description: First matching rule wins. Other monitors use the main limit.
    $name: Overflow
    $description: Automatic mode only. Minimized and hidden windows do not count. Maximized windows count.
  - TileGap: 6
    $name: Window Gap (DPI-scaled pixels)
    #! $min: 0
    #! $max: 100
    $description: Space between tiled windows (0-100).
  - WorkspaceInsets: "6, 6, 6, 6"
    $name: Insets (DPI-scaled pixels)
    $description: Space at the left, top, right, and bottom edges. Enter four values from 0 to 500.
  - MasterPercent: 50
    $name: Master Size (%)
    #! $min: 1
    #! $max: 99
    #! $format: slider
    $description: Starting screen share for the main window in Master + Stack layouts (1-99%).
  - ReverseDwindle: false
    $name: Reverse Dwindle
    $description: Grow the Dwindle layout from the top-left instead of the bottom-right.
  - AdaptiveDwindleSlots: 2
    $name: Adaptive Dwindle Slots
    #! $min: 0
    #! $max: 64
    $description: How many splits can adjust to fit windows (0-64). 0 keeps all splits fixed.
  - AdaptiveDwindlePercentage: 15
    $name: Adaptive Dwindle Percentage
    #! $min: 0
    #! $max: 40
    #! $format: slider
    #! $hideIf: {AdaptiveDwindleSlots: 0}
    $description: How far splits may move from the middle (0-40). 15 allows 35%-65%. 0 disables adjustment.
  $name: Workspace

- appearance:
  - ShowToasts: true
    $name: Show Toasts
    $description: Show layout, mode, and error toasts.
  - FlyoutPosition: top
    $name: Toast Position
    #! $showIf: {ShowToasts: true}
    $description: Show messages at the top or bottom of the active monitor.
    $options:
      - top: Top
      - bottom: Bottom
  - FlyoutOffsetX: 0
    $name: Toast Horizontal Offset (DPI-scaled pixels)
    #! $showIf: {ShowToasts: true}
    $description: Move messages left or right. Positive values move right.
  - FlyoutOffsetY: 0
    $name: Toast Vertical Offset (DPI-scaled pixels)
    #! $showIf: {ShowToasts: true}
    $description: Move messages up or down. Positive values move down.
  - CustomIcons:
      - - Layout: master_stack
          $name: Layout
          $options:
            - master_stack: Master + Stack (Vertical)
            - master_stack_h: Master + Stack (Horizontal)
            - columns: Columns
            - rows: Rows
            - dwindle: Dwindle
            - monocle: Monocle
            - floating: Floating
        - Path: ""
          $name: Icon File (.ico)
          #! $format: filePath
    $name: Custom Tray Layout Icons
    $description: Choose a .ico file for each layout. Blank or invalid files use the built-in icon.
  $name: Appearance

- hotkeys:
  - TilingModifier: alt
    $name: Modifier
    $description: Held with every shortcut below. Alt may conflict with app menus; choose another modifier if needed.
    $options:
      - alt: Alt
      - ctrl: Ctrl
      - alt+shift: Alt + Shift
      - ctrl+alt: Ctrl + Alt
      - ctrl+shift: Ctrl + Shift
  - TileKey: "T"
    $name: Tile Workspace
    $description: Arrange windows on the current monitor. Blank disables this shortcut. Examples include T, Space, or F5.
  - LayoutKey: "L"
    $name: Cycle Layout
    $description: Switch to the next layout. Blank disables this shortcut.
  - SwapMasterKey: "M"
    $name: Set Master Window
    $description: Make the focused window the main window. Blank disables this shortcut.
  - PromoteWindowKey: ","
    $name: Promote Focused Window
    $description: Move the focused window one place earlier. Blank disables this shortcut.
  - DemoteWindowKey: "."
    $name: Demote Focused Window
    $description: Move the focused window one place later. Blank disables this shortcut.
  - ManagementModeToggleKey: "R"
    $name: Toggle Window Management Mode
    $description: Switch between Automatic and Manual mode. Blank disables this shortcut.
  - FloatFocusedKey: "F"
    $name: Float Focused Window
    $description: Remove the focused window from tiling. Blank disables this shortcut.
  - DiagnosticDumpKey: ""
    $name: Write Diagnostic Report
    $description: Save a diagnostic report to the Output Directory. Blank disables this shortcut.
  $name: Hotkeys

- diagnostics:
  - DiagnosticsOutputPath: "%USERPROFILE%\\Documents\\MultiWMDiagnostics"
    $name: Output Directory
    #! $format: folderPath
    $description: Folder for diagnostic reports. Reports include window titles and app paths; review them before sharing.
  $name: Diagnostics

- windowBehavior:
  - MouseMoveBehavior: float
    $name: Tiled Window Drag Action
    $description: What happens when you drag a tiled window.
    $options:
      - float: Float moved window
      - swap: Swap with tiled window under pointer
  - FloatingDefaultSize: "960, 640"
    $name: Floating Default Size (DPI-scaled pixels)
    $description: Starting size in Floating layouts. Enter width, height (100-4000). Automatic mode only.
  $name: Window Behavior

- windowRules:
  - Rules:
      - - Process: ""
          $name: App File Name
          $description: "Case-insensitive exact match. Leave blank to match any process. \n
            Separate multiple names with commas; escape a literal comma as \\,."
        - Class: ""
          $name: Window class
          $description: "Case-insensitive exact match. Leave blank to match any class. \n
            Separate multiple names with commas; escape a literal comma as \\,."
        - TitleContains: ""
          $name: Window title contains
          $description: "Case-insensitive substring match. Leave blank to match any title. \n
            Separate multiple fragments with commas; escape a literal comma as \\,."
        - Treatment: exclude
          $name: Treatment
          $options:
            - exclude: Exclude
            - default_to_floating: Default to floating
            - always_floating: Always floating
            - trace_to_owner: Trace to owner
            - open_on_monitor: Open on monitor / desktop
            - preserve_size_when_centering: Preserve size when centering
            - override_size_when_centering: Override size when centering
        - Monitor: any
          $name: Monitor
          #! $showIf: {Treatment: open_on_monitor}
          $description: Any keeps the current monitor.
          $options:
            - any: Any
            - primary: Primary
            - non_primary: Non-primary
            - specific: Specific monitor
        - MonitorId: ""
          $name: Monitor ID
          #! $showIf: {Treatment: open_on_monitor, Monitor: specific}
          $description: Paste a monitor ID from diagnostics. Blank disables this rule.
        - DesktopNumber: 0
          $name: Desktop Number
          #! $showIf: {Treatment: open_on_monitor}
          #! $min: 0
          $description: Position in Task View, starting at 1. Both desktop fields must match. 0 matches any number.
        - DesktopNameContains: ""
          $name: Desktop Name Contains
          #! $showIf: {Treatment: open_on_monitor}
          $description: Match part of a name, ignoring case. First match wins. Leave both desktop fields empty to stay.
        - FollowWindow: false
          $name: Follow Window
          #! $showIf: {Treatment: open_on_monitor}
          $description: Switch to the window's desktop after moving it.
        - Size: ""
          $name: Centering size override
          #! $showIf: {Treatment: override_size_when_centering}
          $description: 'Only effective when "Override size" is selected. Default-to-floating and Always-floating discovery always preserve the application''s size. Use Width, Height in DPI-scaled pixels, or "Workspace" to fill the current monitor''s usable workspace. Examples: 720, 480'
    $name: Rules
    $description: 
        "Both floating treatments center without changing the application's size. \n
        Trace to owner retargets move/size to its root owner. \n
        Every non-blank match field in a rule must match; comma-separated values within one field are alternatives, e.g. Thunderbird.exe, Librewolf.exe."
  $name: Window Rules
  $description: One shoe does not fit everyone, apparently...

- advanced:
  - ConformanceLeaseMs: 3000
    $name: Keep Windows in Place (ms)
    #! $min: 0
    #! $max: 10000
    $description: Keep windows in their tiles for this long after placement (0-10000 ms). Windows that resist will float. 0 disables this.
  - ConformanceRepairIntervalMs: 75
    $name: Position Repair Interval (ms)
    #! $min: 20
    #! $max: 2000
    #! $hideIf: {ConformanceLeaseMs: 0}
    $description: Time between attempts to keep windows in their tiles (20-2000 ms).
  - ReconcileDelayMs: 50
    $name: Window Change Delay (ms)
    #! $min: 20
    #! $max: 2000
    $description: Wait this long before handling window changes (20-2000 ms).
  - SlowApplicationDelayMs: 250
    $name: New Window Retry Delay (ms)
    #! $min: 0
    #! $max: 2000
    $description: Wait this long before checking a new window again (0-2000 ms). 0 disables retries.
  $name: Advanced
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <initguid.h>
#include <objbase.h>
#include <objectarray.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <winver.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <climits>
#include <cassert>
#include <cstdarg>
#include <deque>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <cstring>
#include <new>
#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

//=============================================================================
// Maintainer architecture map
//=============================================================================
//
// Runtime ownership
// -----------------
// OS callbacks (WinEvent + private virtual-desktop COM) never mutate workspace
// state directly. They enqueue messages onto the STA WM thread. That thread owns
// command handling, lifecycle reconciliation, workspace mutation, placement, and
// UI refresh sequencing.
//
// Event flow
// ----------
// WinEvent / VD callback
//        -> WM thread message
//        -> reconcile observed OS state into the model
//        -> arrange only affected workspaces
//        -> refresh user feedback
//
// Logical modules / reading order
// -------------------------------
// Model                    authoritative Workspace + repository invariants
// Layout                   deterministic rectangle generation
// Window observation       HWND -> value observations, eligibility, placement
// Reconcile                OS facts -> model transitions / ownership repair
// Commands                 explicit user intents over the model
// Platform::WindowEvents   WinEvent adapter + queueing only
// Platform::VirtualDesktop private shell COM adapter + notification sink
// TrayUi / Diagnostics     presentation and observability
// Bootstrap                settings, STA dispatch, Windhawk lifecycle
//
// Packaging deliberately remains one .wh.cpp translation unit for Windhawk. The
// namespaces above are the module boundaries; a future repository split should
// preserve them and amalgamate back to this single distributable source rather
// than reintroducing cross-file global ownership.
//
// Model invariants
// ----------------
// 1. Workspace::records is the authoritative set of logically known HWNDs.
// 2. Workspace::windows contains only active Tiled records, exactly once each,
//    in logical layout order.
// 3. Floating records exist in records but never consume a slot in windows.
// 4. Suspended records exist in records, do not appear in windows, and preserve
//    a saved logical slot/weight until restored or explicitly floated/forgotten.
// 5. Columns/Rows keep one grid weight per active tiled window.
//    MasterStack/MasterStackH keep one stack weight per non-master active window.
//    Dwindle keeps one outgoing ratio per active slot. The final slot's ratio
//    is dormant until another leaf follows it, and survives suspension.
// 6. A managed HWND has exactly one desktop+monitor workspace owner.
// 7. Visibility/cloak events are dirty signals; ownership comes from explicit
//    desktop/monitor queries, not from cloak state.
// 8. Management mode controls group membership policy, not the core WM pipeline:
//    Automatic admits unknown HWNDs and migrates managed HWNDs between workspaces;
//    Manual admits only through explicit commands, forgets minimized members, and
//    releases membership when a managed HWND moves to another virtual desktop.
//    Maximized/fullscreen tiled members are suspended and restore to their saved
//    logical slot when they return to a normal window state.
// 9. Floating geometry is remembered independently of tiled geometry. Tiling and
//    geometry adoption never overwrite the last meaningful floating rectangle.
//
// If a change cannot be explained in terms of those invariants, it probably
// belongs in the observation/reconciliation layer rather than layout code.

#define SAFE_RELEASE(p) \
  do {                  \
    if (p) {            \
      (p)->Release();   \
      (p) = nullptr;    \
    }                   \
  } while (0)

// Private virtual-desktop interface types are deliberately incomplete here.
// Their ABI definitions live in the Platform::VirtualDesktop section after the
// model/layout code, so maintainers encounter the WM model before shell internals.
namespace Platform::VirtualDesktop {
struct IVirtualDesktop;
struct IVirtualDesktopManagerInternal;
struct IVirtualDesktopNotificationService;
struct VirtualDesktopNotificationObject;

struct NotificationInterfaceConfig {
  IID iid{};
  int methodCount = 0;
  int currentChangedIndex = -1;
  bool currentChangedHasMonitors = false;
};

struct VirtualDesktopAbiProfile {
  IID managerInternal{};
  IID virtualDesktop{};
  bool usesHMonitor = false;
  int getDesktopsIndex = 7;
  int switchDesktopIndex = 9;
  int createDesktopIndex = 10;
  int moveDesktopIndex = -1;
  int removeDesktopIndex = 11;
  int animatedSwitchIndex = -1;
  int animationWaitIndex = -1;
  NotificationInterfaceConfig notification{};
};

bool InitializeVirtualDesktopAPI();
void CleanupVirtualDesktopAPI();
void AbandonVirtualDesktopAPIForShellRestart();
bool GetCurrentDesktopId(GUID* outGuid);
bool GetWindowDesktopIdSafe(HWND hwnd, GUID* outGuid);
bool IsWindowOnCurrentDesktopSafe(HWND hwnd, BOOL* onCurrent);
bool RegisterVirtualDesktopNotifications();
bool GetDesktopIdsInOrder(std::vector<GUID>* desktopIds);
bool CreateDesktopAfter(const GUID& sourceDesktop, GUID* desktopId, int* desktopNumber);
bool IsDesktopEmptyExcept(const GUID& desktopId, HWND allowedWindow = nullptr);
bool RemoveEmptyOverflowDesktop(const GUID& desktopId, const GUID& fallbackDesktop);
enum class WindowMovePurpose { Overflow, InitialPlacement };
bool MoveWindowToDesktop(HWND hwnd, DWORD processId, const GUID& sourceDesktop, const GUID& desktopId,
                         WindowMovePurpose purpose = WindowMovePurpose::Overflow);
bool SwitchToDesktop(HWND hwnd, DWORD processId, const GUID& desktopId, const GUID& sourceDesktop, DWORD inputTickMs,
                     WindowMovePurpose purpose = WindowMovePurpose::Overflow);

struct DesktopMetadata {
  std::wstring name;
  int number = 0;
};

DesktopMetadata ReadDesktopMetadata(const GUID& desktopId);
}  // namespace Platform::VirtualDesktop

namespace RuntimeLifecycle {
void RequestMaintenance(bool resetAttempts = false);
void RunMaintenanceNow();
void NotifyShellRestarted();
void NotifyDisplayTopologyChanged();
}  // namespace RuntimeLifecycle

// Narrow platform surface used by the WM core. The private COM implementation
// lives with the other Windows adapters near the end of the source.
using Platform::VirtualDesktop::CleanupVirtualDesktopAPI;
using Platform::VirtualDesktop::GetCurrentDesktopId;
using Platform::VirtualDesktop::GetWindowDesktopIdSafe;
using Platform::VirtualDesktop::InitializeVirtualDesktopAPI;
using Platform::VirtualDesktop::IsWindowOnCurrentDesktopSafe;

struct PendingWorkspaceArrange {
  GUID desktopId{};
  HMONITOR monitor = nullptr;
};

struct PendingWindowOverflow {
  HWND hwnd = nullptr;
  DWORD processId = 0;
  GUID sourceDesktop{};
  std::wstring sourceMonitorId;
  bool monitorFixed = false;
  uint64_t tilingGeneration = 0;
};

struct PendingOverflowSwitch {
  HWND hwnd = nullptr;
  DWORD processId = 0;
  GUID sourceDesktop{};
  GUID targetDesktop{};
  RECT sourceFrame{};
  ULONGLONG deadlineTickMs = 0;
  DWORD inputTickMs = 0;
};

struct PendingDiscoveryRetry {
  HWND hwnd = nullptr;
  DWORD processId = 0;
  ULONGLONG dueTickMs = 0;
};

//=============================================================================
// Runtime/configuration state groups
//=============================================================================

// Hotkey IDs are stable message identifiers, not user configuration.
enum HotkeyIds {
  HK_TILE = 1,
  HK_LAYOUT = 2,
  HK_SWAP_MASTER = 3,
  HK_MANAGEMENT_MODE_TOGGLE = 4,
  HK_FLOAT_FOCUSED = 5,
  HK_DIAGNOSTIC_DUMP = 6,
  HK_PROMOTE_WINDOW = 7,
  HK_DEMOTE_WINDOW = 8,
};

// Messages consumed by the serialized WM thread.
constexpr UINT WM_APP_MOVE_SIZE_END = WM_APP + 1;
constexpr UINT WM_APP_WINDOW_EVENT = WM_APP + 2;
constexpr UINT WM_APP_VIRTUAL_DESKTOP_CHANGED = WM_APP + 3;
constexpr UINT WM_APP_LAYOUT_CYCLE = WM_APP + 5;
constexpr UINT WM_APP_RECONCILE_NOW = WM_APP + 6;
constexpr UINT WM_APP_LAYOUT_SET = WM_APP + 7;
constexpr UINT WM_APP_MANAGEMENT_MODE_SET = WM_APP + 8;
constexpr UINT WM_APP_TILE_WORKSPACE = WM_APP + 9;
constexpr UINT WM_APP_FOREGROUND_CHANGED = WM_APP + 10;

enum class WmMessageDisposition {
  Handled,
  DispatchToWindows,
};

static WmMessageDisposition HandleWmThreadMessage(const MSG& msg);

enum class TileLayout { MasterStack, Columns, Rows, MasterStackH, Dwindle, Monocle, Floating, COUNT };
enum class MouseMoveBehavior { Float, Swap };
enum class WindowRuleTreatment {
  Exclude,
  DefaultToFloating,
  AlwaysFloating,
  TraceToOwner,
  FloatingPlacementOverride,
  InitialPlacement,
};
enum class ManagementMode { Manual, Automatic };
enum class AutomaticNewWindowPosition { LastSlot, AfterFocused };

static std::vector<TileLayout> MakeBuiltInLayoutCycle() {
  return {
      TileLayout::MasterStack,
      TileLayout::MasterStackH,
      TileLayout::Dwindle,
      TileLayout::Columns,
      TileLayout::Rows,
      TileLayout::Monocle,
      TileLayout::Floating};
}

struct WorkspaceInsets {
  LONG left = 6;
  LONG top = 6;
  LONG right = 6;
  LONG bottom = 6;
};

struct FloatingDefaultSize {
  LONG width = 960;
  LONG height = 640;
};

enum class RuleMonitor { Any, Primary, NonPrimary, Specific };

struct WindowRule {
  WindowRuleTreatment treatment = WindowRuleTreatment::Exclude;
  std::vector<std::wstring> processNames;
  std::vector<std::wstring> classNames;
  std::vector<std::wstring> titleFragments;
  bool preserveFloatingSize = true;
  bool useWorkspaceSize = false;
  FloatingDefaultSize floatingSizeDip{};
  RuleMonitor monitor = RuleMonitor::Any;
  std::wstring monitorId;
  int desktopNumber = 0;
  std::wstring desktopNameContains;
  bool followWindow = false;
};

struct WorkspaceInitializationRule {
  std::wstring desktopNameContains;
  int desktopNumber = 0;
  RuleMonitor monitor = RuleMonitor::Any;
  std::wstring monitorId;
  TileLayout layout = TileLayout::MasterStack;
  int settingsIndex = 0;
};

// Windhawk settings and immutable-at-runtime policy values. Settings reloads
// replace these fields; transient WM state lives in WmRuntime instead.
struct SettingsState {
  UINT reconcileDelayMs = 50;
  UINT slowApplicationDelayMs = 250;
  UINT conformanceLeaseMs = 3000;
  UINT conformanceRepairIntervalMs = 75;

  UINT tilingModifiers = MOD_ALT;
  UINT tileKey = 'T';
  UINT layoutKey = 'L';
  UINT swapMasterKey = 'M';
  UINT promoteWindowKey = VK_OEM_COMMA;
  UINT demoteWindowKey = VK_OEM_PERIOD;
  UINT managementModeToggleKey = 'R';
  UINT floatFocusedKey = 'F';
  UINT diagnosticDumpKey = 0;

  // User-facing pixel values are logical pixels at 96 DPI; scale only at a
  // concrete monitor/window boundary so stored screen geometry remains physical.
  WorkspaceInsets insetsDip{};
  LONG gapDip = 6;
  FloatingDefaultSize floatingDefaultSizeDip{};
  LONG masterPercent = 50;
  bool reverseDwindle = false;
  size_t adaptiveDwindleSlots = 2;
  LONG adaptiveDwindlePercentage = 40;

  MouseMoveBehavior mouseMoveBehavior = MouseMoveBehavior::Float;
  AutomaticNewWindowPosition automaticNewWindowPosition =
      AutomaticNewWindowPosition::LastSlot;
  TileLayout defaultLayout = TileLayout::MasterStack;
  std::vector<TileLayout> layoutCycle = MakeBuiltInLayoutCycle();
  std::vector<WorkspaceInitializationRule> workspaceInitializationRules;
  std::vector<WindowRule> windowRules;
  size_t automaticWindowLimit = 0;
  bool overflowCreateDesktop = false;
  bool overflowTryOtherMonitors = false;
  struct MonitorWindowLimit {
    RuleMonitor monitor = RuleMonitor::Specific;
    std::wstring monitorId;
    size_t limit = 0;
  };
  std::vector<MonitorWindowLimit> monitorWindowLimits;
  std::wstring diagnosticsOutputPath =
      L"%USERPROFILE%\\Documents\\MultiWMDiagnostics";
};

// Private virtual-desktop COM state. This entire object belongs to the STA WM
// thread; callback entry points only post work back to that same thread.
struct VirtualDesktopRuntime {
  IServiceProvider* serviceProvider = nullptr;
  Platform::VirtualDesktop::IVirtualDesktopManagerInternal* managerInternal = nullptr;
  IVirtualDesktopManager* desktopManager = nullptr;
  Platform::VirtualDesktop::IVirtualDesktopNotificationService* notificationService = nullptr;
  Platform::VirtualDesktop::VirtualDesktopNotificationObject* notificationObject = nullptr;
  DWORD notificationCookie = 0;
  bool notificationsRegistered = false;
  HRESULT notificationLastHr = S_OK;
  DWORD shellPid = 0;
  ULONGLONG notificationLastAttemptTickMs = 0;
  ULONGLONG notificationLastSuccessTickMs = 0;
  DWORD explorerBuild = 0;
  DWORD explorerRevision = 0;
  Platform::VirtualDesktop::VirtualDesktopAbiProfile abi{};
  bool abiResolved = false;
  LONG changeQueued = 0;
  GUID lastKnownDesktop{};
  bool lastKnownDesktopValid = false;
  bool initialized = false;
};

enum class ReconciledDesktopState {
  Unknown,
  Settled,
  TransitionPending,
};

// Serialized actor/thread state. Management mode affects admission/retention
// policy only; observation, reconciliation, resize learning, and arrangement
// remain live in both modes. Only threadId is observed outside the actor.
struct WmRuntime {
  HANDLE thread = nullptr;
  std::atomic<DWORD> threadId{0};
  HANDLE readyEvent = nullptr;

  UINT_PTR lifecycleTimer = 0;
  UINT_PTR discoveryRetryTimer = 0;
  UINT_PTR overflowTimer = 0; // Checks desktop arrival, without activating HWNDs.
  // Destroy events retire queued HWND lifetimes before deferred work runs.
  std::unordered_set<HWND> overflowWindows;
  std::vector<PendingWindowOverflow> pendingWindowOverflow;
  PendingOverflowSwitch pendingOverflowSwitch{};
  UINT_PTR maintenanceTimer = 0;
  UINT_PTR conformanceTimer = 0;
  UINT maintenanceAttempts = 0;
  bool maintenanceRunning = false;
  bool lifecycleRetryAfterPlatformRecovery = false;
  bool forceMonitorReconcile = false;
  bool comInitialized = false;
  GUID reconciledDesktop{};
  ReconciledDesktopState reconciledDesktopState =
      ReconciledDesktopState::Unknown;
  bool pendingDesktopSwitchFlyouts = false;
  std::vector<HWND> lifecycleDirtyWindows;
  std::vector<PendingDiscoveryRetry> pendingDiscoveryRetries;
  std::vector<PendingWorkspaceArrange> pendingDesktopArranges;
  // Routing is independent of management and also works in Manual mode.
  bool windowRoutingReady = false;
  std::unordered_set<HWND> windowRoutingEvaluated;
  struct PendingWindowRoute {
    DWORD processId = 0;
    GUID sourceDesktop{};
    GUID targetDesktop{};
    RuleMonitor monitor = RuleMonitor::Any;
    std::wstring monitorId;
    bool followWindow = false;
    uint64_t visibleGeneration = 0; // Counts a maximized arrival without admitting it.
  };
  std::unordered_map<HWND, PendingWindowRoute> pendingWindowRoutes;
  PendingOverflowSwitch pendingInitialPlacementSwitch{};
  ManagementMode managementMode = ManagementMode::Automatic;
};

namespace Diagnostics {

// Lifetime counters are deliberately cheap increments on the serialized WM
// actor, or on the controller only after that actor has been joined. They are
// never flushed continuously; the diagnostic hotkey snapshots them into a
// report, preserving the mod's event-driven/idle-zero-cost design.
struct Counters {
  uint64_t wmSessionStarts = 0;
  uint64_t settingsReloads = 0;

  uint64_t lifecycleEventsProcessed = 0;
  uint64_t lifecycleDestroy = 0;
  uint64_t lifecycleMinimizeStart = 0;
  uint64_t lifecycleMinimizeEnd = 0;
  uint64_t lifecycleStateChange = 0;
  uint64_t lifecycleLocationChange = 0;
  uint64_t lifecycleHide = 0;
  uint64_t lifecycleShow = 0;
  uint64_t lifecycleCloaked = 0;
  uint64_t lifecycleUncloaked = 0;
  uint64_t lifecycleScheduleRequests = 0;
  uint64_t lifecycleTimerResets = 0;
  uint64_t lifecycleReconciles = 0;
  uint64_t lifecycleDirtyWindowsProcessed = 0;
  uint64_t moveSizeEndMessages = 0;
  uint64_t trayRefreshMessages = 0;
  uint64_t userMoveGestures = 0;
  uint64_t userResizeGestures = 0;
  uint64_t userNoopGestures = 0;
  uint64_t floatingGeometryUserUpdates = 0;
  uint64_t tiledSwapActions = 0;
  uint64_t tiledFloatActions = 0;
  uint64_t dividerUpdates = 0;
  uint64_t rejectedResizeGestures = 0;

  uint64_t reconcileCalls = 0;
  uint64_t reconcileWindowsExamined = 0;
  uint64_t workspaceSaves = 0;
  uint64_t workspaceMigrations = 0;

  uint64_t enumWindowsPasses = 0;
  uint64_t enumWindowsVisited = 0;
  uint64_t tileCandidatesCollected = 0;

  uint64_t arrangeCalls = 0;
  uint64_t arrangePasses = 0;
  uint64_t layoutPlansBuilt = 0;

  uint64_t placementChecks = 0;
  uint64_t placementNoOps = 0;
  uint64_t placementPreflightStops = 0;
  uint64_t setWindowPosCalls = 0;
  uint64_t placementSuccess = 0;
  uint64_t placementSuppressedByPhysicalState = 0;
  uint64_t placementAdjusted = 0;
  uint64_t placementAccessDenied = 0;
  uint64_t placementRefused = 0;
  uint64_t placementDead = 0;
  uint64_t floatingGeometryRepairs = 0;

  uint64_t constraintProbeAttempts = 0;
  uint64_t constraintProbeSuccesses = 0;
  uint64_t constraintProbeTimeouts = 0;
  uint64_t constraintProbeFailures = 0;
  uint64_t constraintUnsatisfiablePlans = 0;
  uint64_t constraintFloats = 0;

  uint64_t virtualDesktopChangesProcessed = 0;
  uint64_t virtualDesktopFallbackChanges = 0;
  uint64_t vdApiInitAttempts = 0;
  uint64_t vdApiInitSuccesses = 0;
  uint64_t vdApiInitFailures = 0;
  uint64_t vdApiReinitializations = 0;
  uint64_t vdNotificationRegisterAttempts = 0;
  uint64_t vdNotificationRegisterSuccesses = 0;
  uint64_t vdNotificationRegisterFailures = 0;
  uint64_t runtimeMaintenanceRuns = 0;
  uint64_t runtimeMaintenanceRetries = 0;
  uint64_t runtimeMaintenanceExhausted = 0;
  uint64_t shellRestartSignals = 0;
  uint64_t displayTopologySignals = 0;
  uint64_t winEventHookInstallFailures = 0;

  uint64_t conformanceLeasesStarted = 0;
  uint64_t conformanceLeaseRepairs = 0;
  uint64_t conformanceLeaseDeferredRepairs = 0;
  uint64_t conformanceLeaseExpiredCompliant = 0;
  uint64_t conformanceLeaseExpiredFloats = 0;
  uint64_t conformanceLeaseExpiredStale = 0;

  uint64_t tileCommands = 0;
  uint64_t layoutCycleCommands = 0;
  uint64_t floatFocusedCommands = 0;
  uint64_t swapMasterCommands = 0;
  uint64_t promoteWindowCommands = 0;
  uint64_t demoteWindowCommands = 0;
  uint64_t rejectedOrderCommands = 0;
  uint64_t rejectedLayoutChanges = 0;

  uint64_t reportsWritten = 0;
  uint64_t reportWriteFailures = 0;
};

struct RecentEvent {
  ULONGLONG tickMs = 0;
  std::wstring text;
};

struct LifecycleBurst {
  ULONGLONG tickMs = 0;
  size_t dirtyWindows = 0;
  size_t pendingArranges = 0;
};

struct DiagnosticCost {
  uint64_t attempts = 0;
  ULONGLONG wallMs = 0;
  uint64_t processCpu100ns = 0;
  uint64_t wmCpu100ns = 0;
};

struct Runtime {
  ULONGLONG processStartedTickMs = 0;
  ULONGLONG wmSessionStartedTickMs = 0;
  ULONGLONG lastReportTickMs = 0;
  uint64_t reportSequence = 0;
  Counters counters{};
  Counters lastReportCounters{};
  DiagnosticCost lifetimeDiagnosticCost{};
  DiagnosticCost wmSessionDiagnosticCost{};
  std::deque<RecentEvent> significantEvents;
  std::deque<LifecycleBurst> lifecycleBursts;
};

static Runtime g_runtime;
static constexpr size_t kRecentEventLimit = 64;
static constexpr size_t kLifecycleBurstLimit = 16;

static void InitializeProcessRuntime() {
  if (!g_runtime.processStartedTickMs) {
    g_runtime.processStartedTickMs = GetTickCount64();
  }
}

static void RecordEvent(const wchar_t* format, ...) {
  InitializeProcessRuntime();
  wchar_t buffer[768]{};
  va_list args;
  va_start(args, format);
  int written = vswprintf(buffer, ARRAYSIZE(buffer), format, args);
  va_end(args);
  if (written < 0) {
    const wchar_t fallback[] = L"<diagnostic event formatting failed>";
    wcsncpy(buffer, fallback, ARRAYSIZE(buffer) - 1);
  }

  g_runtime.significantEvents.push_back({GetTickCount64(), buffer});
  while (g_runtime.significantEvents.size() > kRecentEventLimit) {
    g_runtime.significantEvents.pop_front();
  }
}

static void RecordLifecycleBurst(size_t dirtyWindows, size_t pendingArranges) {
  InitializeProcessRuntime();
  g_runtime.lifecycleBursts.push_back(
      {GetTickCount64(), dirtyWindows, pendingArranges});
  while (g_runtime.lifecycleBursts.size() > kLifecycleBurstLimit) {
    g_runtime.lifecycleBursts.pop_front();
  }
}

static void BeginWmSession() {
  InitializeProcessRuntime();
  g_runtime.wmSessionStartedTickMs = GetTickCount64();
  g_runtime.wmSessionDiagnosticCost = {};
  ++g_runtime.counters.wmSessionStarts;
  RecordEvent(L"WM session started");
}

}  // namespace Diagnostics

static SettingsState g_settings;
static VirtualDesktopRuntime g_vd;
static WmRuntime g_wm;

static bool IsManualMode() {
  return g_wm.managementMode == ManagementMode::Manual;
}

static bool IsAutomaticMode() {
  return g_wm.managementMode == ManagementMode::Automatic;
}

struct MoveSizeGesture;

static void AssertWmThread(const wchar_t* operation) {
  const DWORD expectedThread = g_wm.threadId.load(std::memory_order_acquire);
  if (!expectedThread) return;
  const DWORD currentThread = GetCurrentThreadId();
  if (currentThread == expectedThread) return;

  Wh_Log(
      L"WM-thread affinity violation in %s: expected=%lu actual=%lu",
      operation ? operation : L"<unknown>", expectedThread, currentThread);
  assert(currentThread == expectedThread);
}

namespace Model {

// Per-desktop + monitor state
struct GuidHash {
  size_t operator()(const GUID& guid) const {
    const uint32_t* data = reinterpret_cast<const uint32_t*>(&guid);
    size_t hash = 0;
    for (int i = 0; i < 4; ++i) {
      hash ^= std::hash<uint32_t>{}(data[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

struct GuidEqual {
  bool operator()(const GUID& a, const GUID& b) const { return IsEqualGUID(a, b); }
};

// Stable monitor identity used by workspace keys. HMONITOR remains a live
// routing token only and is resolved on demand at Win32 boundaries. Physical
// monitor device-interface IDs are preferred; the GDI device name is retained
// only as a compatibility fallback on systems where the interface ID query fails.
struct MonitorId {
  std::wstring deviceId;

  bool Empty() const { return deviceId.empty(); }
  explicit operator bool() const { return !Empty(); }

  bool operator==(const MonitorId& other) const {
    return deviceId == other.deviceId;
  }
  bool operator!=(const MonitorId& other) const {
    return !(*this == other);
  }

  static bool FromHMonitor(HMONITOR monitor, MonitorId* out);
  HMONITOR Resolve() const;
};

struct MonitorIdHash {
  size_t operator()(const MonitorId& monitor) const {
    return std::hash<std::wstring>{}(monitor.deviceId);
  }
};

struct DesktopMonitorKey {
  GUID desktopId{};
  MonitorId monitor;

  static bool FromHMonitor(
      const GUID& desktopId, HMONITOR hmonitor, DesktopMonitorKey* out) {
    if (!out) return false;
    MonitorId monitorId;
    if (!MonitorId::FromHMonitor(hmonitor, &monitorId)) return false;
    out->desktopId = desktopId;
    out->monitor = std::move(monitorId);
    return true;
  }

  HMONITOR ResolveMonitor() const { return monitor.Resolve(); }
};

struct DesktopMonitorKeyHash {
  size_t operator()(const DesktopMonitorKey& key) const {
    size_t hash = GuidHash{}(key.desktopId);
    hash ^= MonitorIdHash{}(key.monitor) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    return hash;
  }
};

struct DesktopMonitorKeyEqual {
  bool operator()(const DesktopMonitorKey& a, const DesktopMonitorKey& b) const {
    return IsEqualGUID(a.desktopId, b.desktopId) && a.monitor == b.monitor;
  }
};

struct SuspendedSlot {
  size_t index = 0;
  double weight = 1.0;
  bool hasWeight = false;
  bool wasMaster = false;
};

enum class ManageState {
  Tiled,
  Floating,
  Suspended,
  Ignored,
};

// Why a logically managed tiled window is temporarily absent from the active
// layout. Hidden is retained for close-to-tray behavior; maximize and fullscreen
// share the same save-slot/restore semantics.
enum class SuspensionReason {
  None,
  Hidden,
  Minimized,
  Maximized,
};

enum class PlacementResult {
  Success,
  SuppressedByPhysicalState,
  AdjustedByWindow,
  AccessDenied,
  Refused,
  Dead,
};

// Native minimum tracking constraints converted to the DWM extended-frame
// geometry used by the layout engine. A successful WM_GETMINMAXINFO report is
// authoritative solver policy. If a later probe fails, callers retain the last
// valid report rather than inventing a weaker constraint.
struct WindowConstraints {
  LONG minWidth = 1;
  LONG minHeight = 1;
  UINT dpi = 96;
  bool valid = false;
  DWORD lastProbeError = ERROR_SUCCESS;
};

static uint64_t NextTilingGeneration() {
  static uint64_t nextGeneration = 1;
  uint64_t generation = nextGeneration++;
  if (generation == 0) generation = nextGeneration++;
  return generation;
}

struct WindowRecord {
  HWND hwnd = nullptr;
  DWORD pid = 0;
  ManageState state = ManageState::Tiled;

  // Capabilities are observations, not hard policy yet. They give later rule/
  // constraint work somewhere stable to live without re-querying every branch.
  bool canMove = true;
  bool canResize = true;
  bool topmost = false;

  // Monotonic admission generation provides stable ordering for constraint
  // recovery candidates. Reordering never changes this value.
  uint64_t tilingGeneration = 0;
  // Retained through constraint-refresh replans so a pending admission can try
  // nearby insertion slots before it is floated.
  bool pendingInsertionPlacement = false;
  bool automaticOverflowMonitorFixed = false;
  // Default-floating admission can precede a usable restored rectangle. Retain
  // the centering request until discovery can place the restored HWND.
  bool pendingDefaultFloatingCenter = false;
  // Set by an Always-floating rule. The policy is retained on the record so a
  // title change cannot make an explicitly protected window tileable by accident.
  bool alwaysFloating = false;
  WindowConstraints constraints{};

  PlacementResult lastPlacementResult = PlacementResult::Success;
  RECT lastRequestedRect{};
  RECT lastObservedRect{};

  // Last meaningful floating frame in physical screen pixels. The capture DPI
  // and monitor let migration restore size sensibly across per-monitor scaling.
  RECT floatingRect{};
  HMONITOR floatingMonitor = nullptr;
  UINT floatingDpi = 96;
  bool hasFloatingRect = false;

  // Only meaningful while state == Suspended. It preserves the tiled logical
  // position/weight across non-participating physical states.
  SuspensionReason suspensionReason = SuspensionReason::None;
  SuspendedSlot savedSlot{};
  bool hasSavedSlot = false;
};

struct PlacementObservation {
  PlacementResult result = PlacementResult::Success;
  RECT requested{};
  RECT observed{};
  // True only when PlaceWindowChecked actually issued SetWindowPos. A successful
  // no-op does not create/extend a conformance lease.
  bool placementIssued = false;
};

struct WorkspaceGeometryItem {
  WindowRecord record{};
  RECT frame{};
};

// The authoritative logical model for one virtual-desktop + monitor workspace.
// All invariant-bearing storage is private: callers can inspect const views, but
// participation/order/weight changes must go through semantic operations below.
class Workspace {
 public:
  enum class PlacementAction { None, Float, Forget };

  Workspace() = default;

  TileLayout Layout() const { return layout_; }
  double MasterRatio() const { return masterRatio_; }
  const std::vector<HWND>& TiledWindows() const { return windows_; }
  const std::unordered_map<HWND, WindowRecord>& Records() const { return records_; }
  const std::vector<double>& StackWeights() const { return stackWeights_; }
  const std::vector<double>& GridWeights() const { return gridWeights_; }
  std::vector<double> DwindleRatios() const {
    if (dwindleRatios_.empty()) return {};
    return {dwindleRatios_.begin(), dwindleRatios_.end() - 1};
  }
  size_t ActiveCount() const { return windows_.size(); }
  size_t VisibleWindowCount() const;
  Workspace EmptyWithInheritedLayoutSettings() const;
  size_t RecordCount() const { return records_.size(); }
  bool Empty() const { return windows_.empty(); }

  bool HasRecord(HWND hwnd) const;
  const WindowRecord* Find(HWND hwnd) const;
  bool IsTiled(HWND hwnd) const;
  size_t TiledIndex(HWND hwnd) const;
  size_t LogicalIndex(HWND hwnd) const;
  HWND LastFocusedWindow() const { return lastFocusedWindow_; }
  HWND LastFocusedTiledWindow() const {
    return IsTiled(lastFocusedWindow_) ? lastFocusedWindow_ : nullptr;
  }
  bool RememberFocusedWindow(HWND hwnd);
  bool UpdateConstraints(HWND hwnd, const WindowConstraints& constraints);
  bool HasSuspended() const;
  bool AllTiledVisible(const std::vector<HWND>& snapshot) const;

  void SetLayout(TileLayout layout);
  bool CycleLayout(const std::vector<TileLayout>& cycle);
  void SetMasterRatio(double ratio);
  bool MakeMaster(HWND hwnd, HWND* oldMaster = nullptr);
  bool SwapTiled(HWND first, HWND second);
  bool MovePendingAdmissionToIndex(HWND hwnd, size_t targetIndex);
  bool ClearInsertionPlacement(HWND hwnd);
  bool ClearDefaultFloatingCenter(HWND hwnd);
  bool EnforceAlwaysFloating(HWND hwnd);
  bool SetDwindleRatio(size_t splitIndex, double ratio);
  bool HasSameWindowMembershipAs(const Workspace& other) const;
  bool HasSameLayoutStateAs(const Workspace& other) const;
  bool RestoreLayoutStateFrom(const Workspace& source);
  bool RestoreCompatibleActiveLayoutStateFrom(const Workspace& source);
  bool RememberFloatingGeometry(
      HWND hwnd, const RECT& frame, HMONITOR monitor, UINT dpi);

  bool ActivateTiled(HWND hwnd);
  bool AdmitFloating(WindowRecord record);
  bool AdmitTiled(WindowRecord record);
  bool AdmitTiledAfter(WindowRecord record, HWND anchor);
  bool AdmitInitial(WindowRecord record, SuspensionReason reason);
  bool AdmitInitialAfter(
      WindowRecord record, SuspensionReason reason, HWND anchor);
  bool Suspend(HWND hwnd, SuspensionReason reason);
  bool Restore(HWND hwnd);
  bool Float(HWND hwnd);
  bool Forget(HWND hwnd);
  bool ExtractForMigration(HWND hwnd, WindowRecord* outRecord);
  void AdmitMigrated(WindowRecord record, HWND anchor = nullptr);
  void MergeRememberedFloatingGeometryFrom(const Workspace& source);
  void MergeNonTiledRecordsFrom(const Workspace& source);
  void RepairForArrangement(
      const std::vector<HWND>& invalidWindows,
      const std::vector<WindowRecord>& missingActiveRecords);

  bool LearnMasterStackResize(
      const RECT& workArea, LONG gap, size_t resizedIndex,
      const ::MoveSizeGesture& gesture);
  bool LearnGridResize(
      const RECT& workArea, LONG gap, size_t resizedIndex,
      const ::MoveSizeGesture& gesture);
  bool LearnDwindleResize(
      const RECT& workArea, LONG gap, size_t resizedIndex,
      const ::MoveSizeGesture& gesture);
  PlacementAction ApplyPlacementObservation(HWND hwnd, const PlacementObservation& observation);

  bool Validate(
      const DesktopMonitorKey& key, const wchar_t* context = L"workspace",
      std::vector<std::wstring>* errors = nullptr, bool logFailures = true) const;

  static Workspace AdoptGeometry(
      TileLayout layout, const RECT& workArea, LONG gap,
      double fallbackMasterRatio,
      const std::vector<WorkspaceGeometryItem>& items,
      HWND preferredFirstWin = nullptr);

 private:
  WindowRecord& UpsertRecord(WindowRecord record);
  WindowRecord* FindMutable(HWND hwnd);
  void EnsureWeights();
  void RemoveActiveAt(size_t index, SuspendedSlot* saved = nullptr);
  size_t ActiveIndexToLogicalIndex(size_t activeIndex) const;
  size_t LogicalIndexToActiveIndex(size_t logicalIndex, HWND restoringHwnd) const;
  void CloseLogicalGapAfterRemoval(size_t removedLogicalIndex, HWND exceptHwnd = nullptr);
  size_t LogicalWindowCount() const;
  SuspendedSlot MakeAppendedSuspendedSlot() const;
  void AppendTiledWithDefaultWeight(HWND hwnd);
  void ResetSuspendedLayoutHints();
  void RememberCurrentLayoutGeometry();
  void RestoreRememberedLayoutGeometry();
  void DebugValidateMutation(const wchar_t* operation) const;

  TileLayout layout_ = TileLayout::MasterStack;
  std::vector<HWND> windows_;
  std::unordered_map<HWND, WindowRecord> records_;
  double masterRatio_ = 0.5;
  // Positional defaults inherited by an overflow-created workspace, without HWNDs.
  std::vector<double> inheritedSlotWeights_;
  std::vector<double> stackWeights_;
  std::vector<double> gridWeights_;
  std::vector<double> dwindleRatios_;

  // Active slot geometry is remembered per layout family. This makes Monocle
  // and ordinary layout cycling presentation-like with respect to the previous
  // tiling geometry: switching away and back restores the exact weights/ratios
  // when the active window order is unchanged. Membership/order changes make the
  // remembered snapshot ineligible rather than trying to reinterpret old slots.
  std::vector<HWND> rememberedMasterStackOrder_;
  std::vector<double> rememberedMasterStackWeights_;
  std::vector<HWND> rememberedGridOrder_;
  std::vector<double> rememberedGridWeights_;
  std::vector<HWND> rememberedDwindleOrder_;
  std::vector<double> rememberedDwindleRatios_;

  HWND lastFocusedWindow_ = nullptr;
};

constexpr LONG kPlacementTolerancePx = 4;

using WorkspaceMap = std::unordered_map<
    DesktopMonitorKey, Workspace, DesktopMonitorKeyHash, DesktopMonitorKeyEqual>;

// Access boundary for the workspace graph. All current callers, including
// WinEvent callbacks, run on the serialized WM thread; the repository keeps its
// workspace snapshots and reverse ownership index consistent.
class WorkspaceRepository {
 public:
  bool Load(const DesktopMonitorKey& key, Workspace* workspace) const;
  void Save(const DesktopMonitorKey& key, const Workspace& workspace);
  bool IsTracked(HWND hwnd) const;
  bool IsTiled(HWND hwnd) const;
  bool IsSuspendedMaximized(HWND hwnd) const;
  std::vector<DesktopMonitorKey> OwnersOf(HWND hwnd) const;
  std::vector<HWND> KnownWindows() const;
  std::vector<HMONITOR> MonitorsOnDesktop(const GUID& desktopId) const;
  std::vector<std::pair<DesktopMonitorKey, Workspace>> Snapshot() const;
  std::vector<std::pair<HWND, std::vector<DesktopMonitorKey>>>
      OwnershipIndexSnapshot() const;

 private:
  mutable SRWLOCK lock_ = SRWLOCK_INIT;
  WorkspaceMap states_;

  // Fast reverse ownership index used by WinEvent/reconciliation hot paths.
  // The vector normally contains exactly one key (global uniqueness invariant),
  // but retaining all keys preserves duplicate-owner repair semantics.
  std::unordered_map<HWND, std::vector<DesktopMonitorKey>> ownersByWindow_;
};


}  // namespace Model

using Model::DesktopMonitorKey;
using Model::ManageState;
using Model::PlacementObservation;
using Model::PlacementResult;
using Model::SuspendedSlot;
using Model::SuspensionReason;
using Model::WindowConstraints;
using Model::WindowRecord;
using Model::Workspace;
using Model::WorkspaceGeometryItem;
using Model::WorkspaceRepository;

struct MonitorIdentityCacheEntry {
  HMONITOR monitor = nullptr;
  Model::MonitorId id;
};

// HMONITOR <-> stable monitor identity is valid for one display-topology
// generation. DPI and work-area observations deliberately aren't cached here.
static std::vector<MonitorIdentityCacheEntry> g_monitorIdentityCache;

static void ClearMonitorIdentityCache() {
  g_monitorIdentityCache.clear();
}

struct MoveSizeTracker {
  std::unordered_map<HWND, RECT> startRects;
  std::unordered_map<HWND, RECT> endRects;
  std::unordered_map<HWND, POINT> endPoints;
};

struct WinEventHooks {
  HWINEVENTHOOK moveSize = nullptr;
  HWINEVENTHOOK minimize = nullptr;
  HWINEVENTHOOK hideDestroy = nullptr;
  HWINEVENTHOOK cloak = nullptr;
  HWINEVENTHOOK state = nullptr;
  HWINEVENTHOOK foreground = nullptr;
  HWINEVENTHOOK locationChange = nullptr;
};

static WorkspaceRepository g_workspaces;
static MoveSizeTracker g_moveSize;
static WinEventHooks g_hooks;

enum class UserLayoutMutationKind {
  Resize,
  Order,
  LayoutChange,
};

struct PendingLayoutMutation {
  Workspace previous;
  Workspace candidate;
  HWND subject = nullptr;
  UserLayoutMutationKind kind = UserLayoutMutationKind::Resize;
  const wchar_t* action = L"Layout mutation";
};

static std::unordered_map<
    DesktopMonitorKey, PendingLayoutMutation,
    Model::DesktopMonitorKeyHash, Model::DesktopMonitorKeyEqual>
    g_pendingLayoutMutations;

// Admission recovery can temporarily move the pending member and/or adaptive
// layout geometry before conformance has proved the resulting arrangement. Keep
// the pre-recovery state until that member settles so a suspend/restore race
// cannot strand the established members in geometry that existed only for the
// pending admission.
struct PendingAdmissionRecovery {
  DesktopMonitorKey key;
  Workspace baseline;
  uint64_t generation = 0;
};

static std::unordered_map<HWND, PendingAdmissionRecovery>
    g_pendingAdmissionRecoveries;

struct ConformanceLease {
  RECT expectedRect{};
  // Preserve the user action that produced this target through asynchronous
  // constraint discovery and its subsequent arrangement.
  HWND preferredTiledWindow = nullptr;
  ULONGLONG expiresAtTickMs = 0;
  // Diagnostic only. Attempt count never controls lease lifetime or policy.
  unsigned attempts = 0;
  // Reinforcement cadence. repairDueTickMs coalesces a burst, and also schedules
  // another checked attempt when the application remains nonconforming.
  ULONGLONG lastRepairTickMs = 0;
  ULONGLONG repairDueTickMs = 0;
  // Distinguishes an old copied lease from a later replacement/cancellation.
  uint64_t generation = 0;
};

struct ConformanceLeaseTracker {
  std::unordered_map<HWND, ConformanceLease> leases;
  uint64_t nextGeneration = 1;
};

static ConformanceLeaseTracker g_conformanceLeases;

enum class ConformanceLeaseReadResult {
  Missing,
  Active,
  Expired,
};

// Reads lease state without consuming expiry. Expired leases stay present until
// the WM actor performs the final passive conformance verdict; WinEvent
// observations must never silently erase the evidence needed for that action.
static ConformanceLeaseReadResult ReadConformanceLease(
    HWND hwnd, ConformanceLease* outLease) {
  AssertWmThread(L"ReadConformanceLease");
  if (!hwnd) return ConformanceLeaseReadResult::Missing;

  const ULONGLONG now = GetTickCount64();
  ConformanceLeaseReadResult result = ConformanceLeaseReadResult::Missing;
  auto it = g_conformanceLeases.leases.find(hwnd);
  if (it != g_conformanceLeases.leases.end()) {
    if (outLease) *outLease = it->second;
    result = now < it->second.expiresAtTickMs
                 ? ConformanceLeaseReadResult::Active
                 : ConformanceLeaseReadResult::Expired;
  }
  return result;
}

static bool ReadActiveConformanceLease(HWND hwnd, ConformanceLease* outLease) {
  if (!outLease) return false;
  return ReadConformanceLease(hwnd, outLease) ==
         ConformanceLeaseReadResult::Active;
}

static bool HasActiveConformanceLease(HWND hwnd) {
  ConformanceLease ignored;
  return ReadActiveConformanceLease(hwnd, &ignored);
}

// generation==0 means an unconditional cancellation (explicit user intent,
// topology/lifecycle teardown, migration, etc.). A nonzero generation lets a
// stale WM-thread observation avoid deleting a newer lease.
static bool CancelConformanceLease(HWND hwnd, uint64_t generation = 0) {
  AssertWmThread(L"CancelConformanceLease");
  if (!hwnd) return false;
  bool erased = false;
  auto it = g_conformanceLeases.leases.find(hwnd);
  if (it != g_conformanceLeases.leases.end() &&
      (generation == 0 || it->second.generation == generation)) {
    g_conformanceLeases.leases.erase(it);
    erased = true;
  }
  return erased;
}

static void ClearAllConformanceLeases() {
  AssertWmThread(L"ClearAllConformanceLeases");
  g_conformanceLeases.leases.clear();
  g_pendingLayoutMutations.clear();
  g_pendingAdmissionRecoveries.clear();
}

static size_t CountActiveConformanceLeases() {
  AssertWmThread(L"CountActiveConformanceLeases");
  const ULONGLONG now = GetTickCount64();
  size_t count = 0;
  for (const auto& kv : g_conformanceLeases.leases) {
    if (now < kv.second.expiresAtTickMs) ++count;
  }
  return count;
}

static void ScheduleNextConformanceTimer() {
  AssertWmThread(L"ScheduleNextConformanceTimer");
  if (g_wm.conformanceTimer) {
    KillTimer(nullptr, g_wm.conformanceTimer);
    g_wm.conformanceTimer = 0;
  }

  const ULONGLONG now = GetTickCount64();
  ULONGLONG earliestDue = 0;
  for (const auto& kv : g_conformanceLeases.leases) {
    const ConformanceLease& lease = kv.second;
    ULONGLONG due = lease.expiresAtTickMs;
    if (lease.repairDueTickMs && lease.repairDueTickMs < due) {
      due = lease.repairDueTickMs;
    }
    if (!earliestDue || due < earliestDue) earliestDue = due;
  }
  if (!earliestDue) return;
  const ULONGLONG remaining = earliestDue > now ? earliestDue - now : 1;
  const UINT delayMs = static_cast<UINT>(std::min<ULONGLONG>(remaining, 0x7FFFFFFFULL));
  g_wm.conformanceTimer = SetTimer(nullptr, 0, std::max<UINT>(1, delayMs), nullptr);
  if (!g_wm.conformanceTimer) {
    Wh_Log(L"Failed to schedule conformance repair/deadline timer");
  }
}

static bool DeferConformanceLeaseRepair(
    HWND hwnd, uint64_t generation, ULONGLONG dueTickMs) {
  AssertWmThread(L"DeferConformanceLeaseRepair");
  if (!hwnd || generation == 0 || !dueTickMs) return false;

  const ULONGLONG now = GetTickCount64();
  bool retained = false;
  auto it = g_conformanceLeases.leases.find(hwnd);
  if (it != g_conformanceLeases.leases.end() &&
      now < it->second.expiresAtTickMs &&
      it->second.generation == generation) {
    if (!it->second.repairDueTickMs || dueTickMs < it->second.repairDueTickMs) {
      it->second.repairDueTickMs = dueTickMs;
    }
    retained = true;
  }
  if (retained) ScheduleNextConformanceTimer();
  return retained;
}

static uint64_t AllocateConformanceLeaseGeneration() {
  AssertWmThread(L"AllocateConformanceLeaseGeneration");
  uint64_t generation = g_conformanceLeases.nextGeneration++;
  if (generation == 0) {
    generation = g_conformanceLeases.nextGeneration++;
  }
  return generation;
}

static void BeginConformanceLease(
    HWND hwnd, const RECT& expectedRect, bool scheduleRepair = false,
    HWND preferredTiledWindow = nullptr) {
  AssertWmThread(L"BeginConformanceLease");
  if (!hwnd || g_settings.conformanceLeaseMs == 0 ||
      expectedRect.right <= expectedRect.left ||
      expectedRect.bottom <= expectedRect.top) {
    CancelConformanceLease(hwnd);
    return;
  }

  const ULONGLONG now = GetTickCount64();
  auto it = g_conformanceLeases.leases.find(hwnd);

  // Re-observing the same authoritative target while its lease is still active
  // must not extend the original deadline or replenish any implicit budget.
  if (it != g_conformanceLeases.leases.end() &&
      now < it->second.expiresAtTickMs &&
      EqualRect(&it->second.expectedRect, &expectedRect)) {
    if (preferredTiledWindow) {
      it->second.preferredTiledWindow = preferredTiledWindow;
    }
    if (scheduleRepair && !it->second.repairDueTickMs) {
      it->second.repairDueTickMs = std::min(
          it->second.expiresAtTickMs,
          now + g_settings.conformanceRepairIntervalMs);
    }
    ScheduleNextConformanceTimer();
    return;
  }

  ConformanceLease lease;
  lease.expectedRect = expectedRect;
  lease.preferredTiledWindow = preferredTiledWindow;
  lease.expiresAtTickMs = now + g_settings.conformanceLeaseMs;
  if (scheduleRepair) {
    lease.repairDueTickMs = std::min(
        lease.expiresAtTickMs,
        now + g_settings.conformanceRepairIntervalMs);
  }
  lease.generation = AllocateConformanceLeaseGeneration();
  g_conformanceLeases.leases[hwnd] = lease;

  ++Diagnostics::g_runtime.counters.conformanceLeasesStarted;
  ScheduleNextConformanceTimer();
}

static bool IsCurrentConformanceLease(HWND hwnd, uint64_t generation) {
  if (!hwnd || generation == 0) return false;
  ConformanceLease current;
  return ReadActiveConformanceLease(hwnd, &current) &&
         current.generation == generation;
}

static bool RecordConformanceLeaseAttempt(
    HWND hwnd, uint64_t generation, unsigned* outAttemptNumber = nullptr) {
  AssertWmThread(L"RecordConformanceLeaseAttempt");
  if (!hwnd || generation == 0) return false;

  const ULONGLONG now = GetTickCount64();
  bool retained = false;
  auto it = g_conformanceLeases.leases.find(hwnd);
  if (it != g_conformanceLeases.leases.end() &&
      now < it->second.expiresAtTickMs &&
      it->second.generation == generation) {
    ++it->second.attempts;
    it->second.lastRepairTickMs = now;
    it->second.repairDueTickMs = 0;
    if (outAttemptNumber) *outAttemptNumber = it->second.attempts;
    retained = true;
  }
  return retained;
}

//=============================================================================
// Move/size gesture tracking + classification
//=============================================================================



enum class MoveSizeIntent {
    None,
    Move,
    Resize,
};

struct MoveSizeGesture {
  MoveSizeIntent intent = MoveSizeIntent::None;
  RECT start{};
  RECT end{};
  POINT dropPoint{};
  bool hasRects = false;
  bool hasDropPoint = false;
};

static inline bool Differs(LONG a, LONG b, LONG tol = 1) {
    return (a > b) ? (a - b > tol) : (b - a > tol);
}

// Classify the user's *gesture*, not whether position and size both happened to
// change. WinUI/DWM windows can perturb one frame edge slightly during a pure
// move, while a legitimate top/left-edge resize necessarily changes position
// and size together. Opposite edges moving together is therefore the stronger
// signal for translation.
MoveSizeIntent ClassifyMoveSizeIntent(const RECT& before, const RECT& after, LONG tol = 1) {
    const LONG dl = after.left - before.left;
    const LONG dr = after.right - before.right;
    const LONG dt = after.top - before.top;
    const LONG db = after.bottom - before.bottom;

    const LONG maxEdgeDelta = std::max(
        std::max(std::abs(dl), std::abs(dr)),
        std::max(std::abs(dt), std::abs(db)));
    if (maxEdgeDelta <= tol) {
        Wh_Log(L"Move/size intent: none");
        return MoveSizeIntent::None;
    }

    // Twice the center displacement versus actual width/height deformation.
    // Use 64-bit intermediates so adding two LONG deltas cannot overflow.
    const long long moveScore = std::max(
        std::llabs(static_cast<long long>(dl) + dr),
        std::llabs(static_cast<long long>(dt) + db));
    const long long resizeScore = std::max(
        std::llabs(static_cast<long long>(dr) - dl),
        std::llabs(static_cast<long long>(db) - dt));

    // A translation must dominate deformation by a healthy margin. A normal
    // one-edge resize produces roughly equal scores, so it remains Resize.
    if (moveScore > resizeScore * 2) {
        Wh_Log(L"Move/size intent: move (move=%lld resize=%lld)", moveScore, resizeScore);
        return MoveSizeIntent::Move;
    }

    Wh_Log(L"Move/size intent: resize (move=%lld resize=%lld)", moveScore, resizeScore);
    return MoveSizeIntent::Resize;
}

static MoveSizeGesture PeekMoveSizeGesture(HWND hwnd) {
  AssertWmThread(L"PeekMoveSizeGesture");
  MoveSizeGesture gesture;
  auto startIt = g_moveSize.startRects.find(hwnd);
  auto endIt = g_moveSize.endRects.find(hwnd);
  auto pointIt = g_moveSize.endPoints.find(hwnd);
  if (startIt != g_moveSize.startRects.end() &&
      endIt != g_moveSize.endRects.end()) {
    gesture.start = startIt->second;
    gesture.end = endIt->second;
    gesture.hasRects = true;
  }
  if (pointIt != g_moveSize.endPoints.end()) {
    gesture.dropPoint = pointIt->second;
    gesture.hasDropPoint = true;
  }
  if (gesture.hasRects) {
    gesture.intent = ClassifyMoveSizeIntent(gesture.start, gesture.end, 1);
  }
  return gesture;
}

static bool IsMoveSizeGestureInProgress(HWND hwnd) {
  AssertWmThread(L"IsMoveSizeGestureInProgress");
  if (!hwnd) return false;
  const bool started = g_moveSize.startRects.find(hwnd) != g_moveSize.startRects.end();
  const bool ended = g_moveSize.endRects.find(hwnd) != g_moveSize.endRects.end();
  return started && !ended;
}






//=============================================================================
// Configuration parsing + shared core helpers
//=============================================================================

UINT ParseModifiers(PCWSTR str) {
  UINT modifiers = 0;
  if (wcsstr(str, L"alt")) modifiers |= MOD_ALT;
  if (wcsstr(str, L"ctrl")) modifiers |= MOD_CONTROL;
  if (wcsstr(str, L"shift")) modifiers |= MOD_SHIFT;
  return modifiers;
}

static bool TryParseLayoutSetting(PCWSTR str, TileLayout* outLayout) {
  if (!str || !outLayout) return false;
  static const std::pair<PCWSTR, TileLayout> kLayoutMap[] = {
      {L"master_stack", TileLayout::MasterStack},
      {L"master_stack_h", TileLayout::MasterStackH},
      {L"columns", TileLayout::Columns},
      {L"rows", TileLayout::Rows},
      {L"dwindle", TileLayout::Dwindle},
      // Compatibility alias for configurations saved before 1.13.29.
      {L"bsp", TileLayout::Dwindle},
      {L"monocle", TileLayout::Monocle},
      {L"floating", TileLayout::Floating},
  };
  for (const auto& entry : kLayoutMap) {
    if (wcscmp(str, entry.first) == 0) {
      *outLayout = entry.second;
      return true;
    }
  }
  return false;
}

TileLayout ParseLayoutSetting(PCWSTR str) {
  TileLayout layout = TileLayout::MasterStack;
  TryParseLayoutSetting(str, &layout);
  return layout;
}

template <typename T, typename Parser>
T ReadHotkeySetting(PCWSTR name, Parser parser) {
  auto value = WindhawkUtils::StringSetting::make(name);
  if (!*value.get()) return T{};  // Blank hotkey = unused.

  T result = parser(value.get());
  if (!result) {
    Wh_Log(
        L"Invalid %ls hotkey setting '%ls'; hotkey disabled", name,
        value.get());
  }
  return result;
}

UINT ReadModifierSetting(PCWSTR name, UINT defaultVal) {
  auto value = WindhawkUtils::StringSetting::make(name);
  UINT result = ParseModifiers(value.get());
  return result ? result : defaultVal;
}

inline double ClampDouble(double value, double minVal, double maxVal) {
  if (value < minVal) return minVal;
  if (value > maxVal) return maxVal;
  return value;
}

bool GetWindowFrameRect(HWND hwnd, RECT* outRect) {
  if (!hwnd || !outRect) return false;
  if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, outRect, sizeof(*outRect)))) {
    return true;
  }
  return GetWindowRect(hwnd, outRect) != FALSE;
}

// One authoritative HWND -> physical-monitor observation for routing workspace
// ownership. Restored/maximized windows prefer the DWM frame so every caller uses
// the same monitor-boundary rule. Minimized windows deliberately fall back to
// MonitorFromWindow(), which Windows resolves from the pre-minimize rectangle.
static HMONITOR GetWindowPhysicalMonitor(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) return nullptr;

  if (!IsIconic(hwnd)) {
    RECT frame{};
    if (GetWindowFrameRect(hwnd, &frame) &&
        frame.right > frame.left && frame.bottom > frame.top) {
      HMONITOR monitor = MonitorFromRect(&frame, MONITOR_DEFAULTTONULL);
      if (monitor) return monitor;
    }
  }

  return MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
}

static bool IsLiveMonitorHandle(HMONITOR monitor) {
  if (!monitor) return false;
  MONITORINFO monitorInfo{sizeof(monitorInfo)};
  return GetMonitorInfoW(monitor, &monitorInfo) != FALSE;
}

static void NormalizeMonitorIdentityToken(std::wstring* value) {
  if (!value) return;
  std::transform(
      value->begin(), value->end(), value->begin(),
      [](wchar_t ch) { return static_cast<wchar_t>(std::towupper(ch)); });
}

bool Model::MonitorId::FromHMonitor(HMONITOR monitor, MonitorId* out) {
  if (!out || !IsLiveMonitorHandle(monitor)) return false;

  auto cached = std::find_if(
      g_monitorIdentityCache.begin(), g_monitorIdentityCache.end(),
      [&](const MonitorIdentityCacheEntry& entry) {
        return entry.monitor == monitor;
      });
  if (cached != g_monitorIdentityCache.end()) {
    *out = cached->id;
    return true;
  }

  MONITORINFOEXW monitorInfo{};
  monitorInfo.cbSize = sizeof(monitorInfo);
  if (!GetMonitorInfoW(
          monitor, reinterpret_cast<MONITORINFO*>(&monitorInfo))) {
    return false;
  }

  // This is the same stable display-interface identity source used by FancyWM:
  // map the current GDI display name (\\.\DISPLAYn) to the monitor device
  // interface exposed by EnumDisplayDevices + EDD_GET_DEVICE_INTERFACE_NAME.
  DISPLAY_DEVICEW device{};
  device.cb = sizeof(device);
  std::wstring identity;
  if (EnumDisplayDevicesW(
          monitorInfo.szDevice, 0, &device, EDD_GET_DEVICE_INTERFACE_NAME) &&
      device.DeviceID[0]) {
    identity = L"IFACE:";
    identity += device.DeviceID;
  } else if (monitorInfo.szDevice[0]) {
    // Compatibility fallback only. Normal physical monitors should have an
    // interface ID; retaining the GDI name is preferable to disabling tiling on
    // unusual virtual/RDP display providers that don't expose one.
    identity = L"GDI:";
    identity += monitorInfo.szDevice;
  } else {
    return false;
  }

  NormalizeMonitorIdentityToken(&identity);
  out->deviceId = std::move(identity);
  g_monitorIdentityCache.push_back({monitor, *out});
  return true;
}

HMONITOR Model::MonitorId::Resolve() const {
  if (deviceId.empty()) return nullptr;

  for (const auto& entry : g_monitorIdentityCache) {
    if (entry.id == *this && IsLiveMonitorHandle(entry.monitor)) {
      return entry.monitor;
    }
  }

  struct ResolveContext {
    const MonitorId* target = nullptr;
    HMONITOR result = nullptr;
  } context{this, nullptr};

  EnumDisplayMonitors(
      nullptr, nullptr,
      [](HMONITOR monitor, HDC, LPRECT, LPARAM lParam) WINAPI -> BOOL {
        auto* context = reinterpret_cast<ResolveContext*>(lParam);
        MonitorId candidate;
        if (MonitorId::FromHMonitor(monitor, &candidate) &&
            candidate == *context->target) {
          context->result = monitor;
          return FALSE;
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return context.result;
}

bool GetMonitorWorkArea(HMONITOR monitor, RECT* outRect) {
  if (!monitor || !outRect) return false;
  MONITORINFO monitorInfo = {sizeof(monitorInfo)};
  if (!GetMonitorInfoW(monitor, &monitorInfo)) return false;
  *outRect = monitorInfo.rcWork;
  return true;
}

static UINT GetMonitorEffectiveDpi(HMONITOR monitor) {
  if (!monitor || !IsLiveMonitorHandle(monitor)) return 96;

  using GetDpiForMonitorFn = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
  using GetScaleFactorForMonitorFn = HRESULT(WINAPI*)(HMONITOR, int*);
  struct DpiApis {
    GetDpiForMonitorFn getDpi = nullptr;
    GetScaleFactorForMonitorFn getScale = nullptr;
  };
  static const DpiApis apis = [] {
    DpiApis result;
    HMODULE shcore = LoadLibraryExW(
        L"Shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (shcore) {
      result.getDpi = reinterpret_cast<GetDpiForMonitorFn>(
          GetProcAddress(shcore, "GetDpiForMonitor"));
      result.getScale = reinterpret_cast<GetScaleFactorForMonitorFn>(
          GetProcAddress(shcore, "GetScaleFactorForMonitor"));
    }
    return result;
  }();

  UINT dpiX = 96;
  UINT dpiY = 96;
  // MDT_EFFECTIVE_DPI == 0. Keep the enum types local so no Shcore import
  // library is required. DPI is observed fresh at the operation boundary.
  const bool haveDpi = apis.getDpi &&
      SUCCEEDED(apis.getDpi(monitor, 0, &dpiX, &dpiY)) && dpiX;

  // Also correct a virtualized 96-DPI result when the host process awareness
  // differs from this per-monitor-aware WM thread.
  int scalePercent = 100;
  const bool haveScale = apis.getScale &&
      SUCCEEDED(apis.getScale(monitor, &scalePercent)) && scalePercent > 0;
  if (haveScale && (!haveDpi || (dpiX == 96 && scalePercent != 100))) {
    return static_cast<UINT>(MulDiv(96, scalePercent, 100));
  }
  return haveDpi ? dpiX : 96;
}

static LONG ScaleDip(LONG value, UINT dpi) {
  return MulDiv(value, static_cast<int>(dpi ? dpi : 96), 96);
}

std::vector<double> DefaultWeights(size_t count) {
  return std::vector<double>(count, 1.0);
}

static HMONITOR GetCurrentManagedWindowMonitor(HWND hwnd);
static SuspensionReason GetPhysicalSuspensionReason(HWND hwnd);
static bool WindowCanBeManaged(HWND hwnd, bool allowFixedSize = false);
static bool IsWindowTrackedInAnyState(HWND hwnd);
namespace Reconcile {
static void ScheduleLifecycleReconcile(HWND hwnd);
}
bool ContainsWindow(const std::vector<HWND>& windows, HWND hwnd) {
  for (HWND w : windows) {
    if (w == hwnd) return true;
  }
  return false;
}

bool Workspace::HasRecord(HWND hwnd) const {
  return hwnd && records_.find(hwnd) != records_.end();
}

const WindowRecord* Workspace::Find(HWND hwnd) const {
  auto it = records_.find(hwnd);
  return it == records_.end() ? nullptr : &it->second;
}

WindowRecord* Workspace::FindMutable(HWND hwnd) {
  auto it = records_.find(hwnd);
  return it == records_.end() ? nullptr : &it->second;
}

bool Workspace::IsTiled(HWND hwnd) const {
  const WindowRecord* record = Find(hwnd);
  return record && record->state == ManageState::Tiled &&
         std::find(windows_.begin(), windows_.end(), hwnd) != windows_.end();
}

size_t Workspace::TiledIndex(HWND hwnd) const {
  auto it = std::find(windows_.begin(), windows_.end(), hwnd);
  return it == windows_.end() ? static_cast<size_t>(-1)
                              : static_cast<size_t>(it - windows_.begin());
}

size_t Workspace::LogicalIndex(HWND hwnd) const {
  const WindowRecord* record = Find(hwnd);
  if (!record) return static_cast<size_t>(-1);
  if (record->state == ManageState::Suspended && record->hasSavedSlot) {
    return record->savedSlot.index;
  }
  if (record->state == ManageState::Tiled) {
    const size_t activeIndex = TiledIndex(hwnd);
    if (activeIndex != static_cast<size_t>(-1)) {
      return ActiveIndexToLogicalIndex(activeIndex);
    }
  }
  return static_cast<size_t>(-1);
}

bool Workspace::RememberFocusedWindow(HWND hwnd) {
  AssertWmThread(L"Workspace::RememberFocusedWindow");
  if (!hwnd || !HasRecord(hwnd) || lastFocusedWindow_ == hwnd) return false;
  lastFocusedWindow_ = hwnd;
  return true;
}

bool Workspace::UpdateConstraints(
    HWND hwnd, const WindowConstraints& constraints) {
  AssertWmThread(L"Workspace::UpdateConstraints");
  WindowRecord* record = FindMutable(hwnd);
  if (!record) return false;

  if (!record->tilingGeneration && record->state != ManageState::Floating) {
    record->tilingGeneration = Model::NextTilingGeneration();
  }
  const WindowConstraints& old = record->constraints;
  const bool changed =
      old.minWidth != constraints.minWidth ||
      old.minHeight != constraints.minHeight ||
      old.dpi != constraints.dpi ||
      old.valid != constraints.valid ||
      old.lastProbeError != constraints.lastProbeError;
  record->constraints = constraints;
  return changed;
}

bool Workspace::HasSuspended() const {
  return std::any_of(
      records_.begin(), records_.end(), [](const auto& kv) {
        return kv.second.state == ManageState::Suspended;
      });
}

bool Workspace::AllTiledVisible(const std::vector<HWND>& snapshot) const {
  return std::all_of(
      windows_.begin(), windows_.end(),
      [&](HWND hwnd) { return ContainsWindow(snapshot, hwnd); });
}

WindowRecord& Workspace::UpsertRecord(WindowRecord record) {
  AssertWmThread(L"Workspace::UpsertRecord");
  const HWND hwnd = record.hwnd;
  auto [it, inserted] = records_.emplace(hwnd, std::move(record));
  if (!inserted) {
    // Preserve accumulated runtime bookkeeping when merely refreshing observed
    // capabilities/identity for an already-known record.
    WindowRecord& existing = it->second;
    const DWORD pid = it->second.pid;
    const PlacementResult lastResult = existing.lastPlacementResult;
    const RECT lastRequested = existing.lastRequestedRect;
    const RECT lastObserved = existing.lastObservedRect;
    const RECT floatingRect = existing.floatingRect;
    const HMONITOR floatingMonitor = existing.floatingMonitor;
    const UINT floatingDpi = existing.floatingDpi;
    const bool hasFloatingRect = existing.hasFloatingRect;
    const uint64_t tilingGeneration = existing.tilingGeneration;
    const bool pendingInsertionPlacement = existing.pendingInsertionPlacement;
    const bool pendingDefaultFloatingCenter =
        existing.pendingDefaultFloatingCenter;
    const bool alwaysFloating = existing.alwaysFloating;
    const WindowConstraints cachedConstraints = existing.constraints;
    const ManageState state = existing.state;
    const SuspensionReason suspension = existing.suspensionReason;
    const SuspendedSlot slot = existing.savedSlot;
    const bool hasSlot = existing.hasSavedSlot;

    existing = std::move(record);
    existing.pid = existing.pid ? existing.pid : pid;
    existing.lastPlacementResult = lastResult;
    existing.lastRequestedRect = lastRequested;
    existing.lastObservedRect = lastObserved;
    existing.tilingGeneration = tilingGeneration;
    existing.pendingInsertionPlacement = pendingInsertionPlacement;
    existing.pendingDefaultFloatingCenter = pendingDefaultFloatingCenter;
    existing.alwaysFloating = alwaysFloating;
    if (!existing.constraints.valid && cachedConstraints.valid) {
      existing.constraints = cachedConstraints;
    }
    if (hasFloatingRect) {
      existing.floatingRect = floatingRect;
      existing.floatingMonitor = floatingMonitor;
      existing.floatingDpi = floatingDpi;
      existing.hasFloatingRect = true;
    }
    existing.state = state;
    existing.suspensionReason = suspension;
    existing.savedSlot = slot;
    existing.hasSavedSlot = hasSlot;
  }
  return it->second;
}

bool Workspace::ActivateTiled(HWND hwnd) {
  AssertWmThread(L"Workspace::ActivateTiled");
  WindowRecord* record = FindMutable(hwnd);
  if (!record || record->alwaysFloating) return false;

  const ManageState previousState = record->state;
  bool changed = false;
  if (record->pendingDefaultFloatingCenter) {
    record->pendingDefaultFloatingCenter = false;
    changed = true;
  }
  if (record->state != ManageState::Tiled || record->hasSavedSlot ||
      record->suspensionReason != SuspensionReason::None) {
    record->state = ManageState::Tiled;
    record->suspensionReason = SuspensionReason::None;
    record->savedSlot = {};
    record->hasSavedSlot = false;
    if (previousState == ManageState::Floating ||
        previousState == ManageState::Ignored) {
      record->tilingGeneration = Model::NextTilingGeneration();
      record->pendingInsertionPlacement = true;
      // Explicitly re-tiling a floating window is a new placement attempt, but
      // the app's last valid native minimum remains authoritative. If it changed
      // while floating, a corrected placement will trigger a bounded re-probe.
    } else if (!record->tilingGeneration) {
      record->tilingGeneration = Model::NextTilingGeneration();
    }
    changed = true;
  }
  if (!ContainsWindow(windows_, hwnd)) {
    AppendTiledWithDefaultWeight(hwnd);
    record->pendingInsertionPlacement = true;
    changed = true;
  }
  if (changed) DebugValidateMutation(L"Workspace::ActivateTiled");
  return changed;
}

bool Workspace::AdmitFloating(WindowRecord record) {
  AssertWmThread(L"Workspace::AdmitFloating");
  const HWND hwnd = record.hwnd;
  if (!hwnd || HasRecord(hwnd)) return false;

  record.state = ManageState::Floating;
  record.tilingGeneration = 0;
  record.pendingInsertionPlacement = false;
  record.suspensionReason = SuspensionReason::None;
  record.savedSlot = {};
  record.hasSavedSlot = false;
  records_.emplace(hwnd, std::move(record));
  DebugValidateMutation(L"Workspace::AdmitFloating");
  return true;
}

bool Workspace::AdmitTiled(WindowRecord record) {
  AssertWmThread(L"Workspace::AdmitTiled");
  if (!record.hwnd) return false;
  const HWND hwnd = record.hwnd;
  const bool existed = HasRecord(hwnd);
  if (!existed && !record.tilingGeneration) {
    record.tilingGeneration = Model::NextTilingGeneration();
  }
  UpsertRecord(std::move(record));
  const bool activated = ActivateTiled(hwnd);
  return !existed || activated;
}

// Fresh Automatic admission can insert relative to the retained managed focus.
// Shift suspended logical slots first so every member keeps a unique position.
bool Workspace::AdmitTiledAfter(WindowRecord record, HWND anchor) {
  AssertWmThread(L"Workspace::AdmitTiledAfter");
  const HWND hwnd = record.hwnd;
  if (!hwnd || HasRecord(hwnd) || !IsTiled(anchor)) {
    return AdmitTiled(std::move(record));
  }

  const size_t anchorActiveIndex = TiledIndex(anchor);
  if (anchorActiveIndex == static_cast<size_t>(-1)) {
    return AdmitTiled(std::move(record));
  }

  const size_t insertionLogicalIndex =
      ActiveIndexToLogicalIndex(anchorActiveIndex) + 1;
  for (auto& kv : records_) {
    WindowRecord& existing = kv.second;
    if (existing.state == ManageState::Suspended &&
        existing.hasSavedSlot &&
        existing.savedSlot.index >= insertionLogicalIndex) {
      ++existing.savedSlot.index;
    }
  }

  EnsureWeights();
  const size_t insertionActiveIndex =
      std::min(anchorActiveIndex + 1, windows_.size());
  windows_.insert(windows_.begin() + insertionActiveIndex, hwnd);

  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    gridWeights_.insert(
        gridWeights_.begin() + std::min(insertionActiveIndex, gridWeights_.size()),
        insertionActiveIndex < inheritedSlotWeights_.size() ? inheritedSlotWeights_[insertionActiveIndex] : 1.0);
  } else if ((layout_ == TileLayout::MasterStack ||
              layout_ == TileLayout::MasterStackH) &&
             insertionActiveIndex > 0) {
    const size_t stackIndex =
        std::min(insertionActiveIndex - 1, stackWeights_.size());
    stackWeights_.insert(stackWeights_.begin() + stackIndex,
        stackIndex < inheritedSlotWeights_.size() ? inheritedSlotWeights_[stackIndex] : 1.0);
  } else if (layout_ == TileLayout::Dwindle) {
    const size_t ratioIndex =
        std::min(insertionActiveIndex, dwindleRatios_.size());
    dwindleRatios_.insert(dwindleRatios_.begin() + ratioIndex,
        ratioIndex < inheritedSlotWeights_.size() ? inheritedSlotWeights_[ratioIndex] : 0.5);
  }

  record.state = ManageState::Tiled;
  record.pendingInsertionPlacement = true;
  record.suspensionReason = SuspensionReason::None;
  record.savedSlot = {};
  record.hasSavedSlot = false;
  if (!record.tilingGeneration) {
    record.tilingGeneration = Model::NextTilingGeneration();
  }
  records_.emplace(hwnd, std::move(record));
  EnsureWeights();
  DebugValidateMutation(L"Workspace::AdmitTiledAfter");
  return true;
}

bool Workspace::RememberFloatingGeometry(
    HWND hwnd, const RECT& frame, HMONITOR monitor, UINT dpi) {
  AssertWmThread(L"Workspace::RememberFloatingGeometry");
  WindowRecord* record = FindMutable(hwnd);
  if (!record || !monitor || frame.right <= frame.left ||
      frame.bottom <= frame.top) {
    return false;
  }

  const UINT normalizedDpi = dpi ? dpi : 96;
  if (record->hasFloatingRect &&
      EqualRect(&record->floatingRect, &frame) &&
      record->floatingMonitor == monitor &&
      record->floatingDpi == normalizedDpi) {
    return false;
  }

  record->floatingRect = frame;
  record->floatingMonitor = monitor;
  record->floatingDpi = normalizedDpi;
  record->hasFloatingRect = true;
  DebugValidateMutation(L"Workspace::RememberFloatingGeometry");
  return true;
}

// Pure model import: interprets an already-observed geometry snapshot as
// authoritative workspace ordering and resize weights. No Win32 queries happen
// inside Workspace.
Workspace Workspace::AdoptGeometry(
    TileLayout layout, const RECT& workArea, LONG gap,
    double fallbackMasterRatio,
    const std::vector<WorkspaceGeometryItem>& items, HWND preferredFirstWin) {
  AssertWmThread(L"Workspace::AdoptGeometry");
  Workspace workspace;
  workspace.layout_ = layout;
  for (const auto& item : items) {
    WindowRecord record = item.record;
    if (!record.tilingGeneration) {
      record.tilingGeneration = Model::NextTilingGeneration();
    }
    workspace.windows_.push_back(record.hwnd);
    workspace.records_.emplace(record.hwnd, std::move(record));
  }
  if (items.empty()) return workspace;

  if (layout == TileLayout::MasterStack || layout == TileLayout::MasterStackH) {
    const bool horizontal = layout == TileLayout::MasterStackH;
    struct WindowInfo {
      HWND hwnd;
      RECT rect;
    };
    std::vector<WindowInfo> infos;
    infos.reserve(items.size());
    for (const auto& item : items) infos.push_back({item.record.hwnd, item.frame});

    struct Candidate {
      long long score;
      long long axisPos;
      size_t index;
    };
    std::vector<Candidate> candidates;
    candidates.reserve(infos.size());

    for (size_t i = 0; i < infos.size(); ++i) {
      const RECT& r = infos[i].rect;
      long long d[4] = {
          std::llabs(static_cast<long long>(r.left) - workArea.left),
          std::llabs(static_cast<long long>(r.top) - workArea.top),
          std::llabs(static_cast<long long>(r.right) - workArea.right),
          std::llabs(static_cast<long long>(r.bottom) - workArea.bottom),
      };
      std::sort(d, d + 4);
      Candidate candidate{
          d[0] + d[1] + d[2],
          horizontal
              ? std::llabs(static_cast<long long>(r.top) - workArea.top)
              : std::llabs(static_cast<long long>(r.left) - workArea.left),
          i};
      candidates.push_back(candidate);
    }

    std::stable_sort(
        candidates.begin(), candidates.end(),
        [preferredFirstWin, &infos](const Candidate& a, const Candidate& b) {
          const bool aPreferred = preferredFirstWin && infos[a.index].hwnd == preferredFirstWin;
          const bool bPreferred = preferredFirstWin && infos[b.index].hwnd == preferredFirstWin;
          if (aPreferred != bPreferred) return aPreferred;
          if (a.score != b.score) return a.score < b.score;
          return a.axisPos < b.axisPos;
        });

    const size_t masterIndex = candidates.front().index;
    const WindowInfo master = infos[masterIndex];
    std::vector<WindowInfo> stack;
    stack.reserve(infos.size() - 1);
    for (size_t i = 0; i < infos.size(); ++i) {
      if (i != masterIndex) stack.push_back(infos[i]);
    }
    std::stable_sort(
        stack.begin(), stack.end(), [horizontal](const WindowInfo& a, const WindowInfo& b) {
          return horizontal ? a.rect.left < b.rect.left : a.rect.top < b.rect.top;
        });

    workspace.windows_.clear();
    workspace.windows_.push_back(master.hwnd);
    for (const auto& item : stack) workspace.windows_.push_back(item.hwnd);

    const LONG totalSize = horizontal ? workArea.bottom - workArea.top
                                      : workArea.right - workArea.left;
    if (totalSize > gap + 1) {
      LONG masterSize = horizontal ? master.rect.bottom - master.rect.top
                                   : master.rect.right - master.rect.left;
      if (masterSize < 1) masterSize = 1;
      workspace.masterRatio_ = ClampDouble(
          static_cast<double>(masterSize) / static_cast<double>(totalSize - gap),
          0.1, 0.9);
    } else {
      workspace.masterRatio_ = ClampDouble(fallbackMasterRatio, 0.1, 0.9);
    }

    workspace.stackWeights_.clear();
    for (const auto& item : stack) {
      LONG size = horizontal ? item.rect.right - item.rect.left
                             : item.rect.bottom - item.rect.top;
      if (size < 1) size = 1;
      workspace.stackWeights_.push_back(static_cast<double>(size));
    }
  } else if (layout == TileLayout::Columns || layout == TileLayout::Rows) {
    const bool horizontal = layout == TileLayout::Rows;
    struct WindowInfo {
      HWND hwnd;
      RECT rect;
    };
    std::vector<WindowInfo> infos;
    infos.reserve(items.size());
    for (const auto& item : items) infos.push_back({item.record.hwnd, item.frame});

    std::stable_sort(
        infos.begin(), infos.end(), [horizontal](const WindowInfo& a, const WindowInfo& b) {
          return horizontal ? a.rect.top < b.rect.top : a.rect.left < b.rect.left;
        });

    workspace.windows_.clear();
    workspace.gridWeights_.clear();
    for (const auto& item : infos) {
      workspace.windows_.push_back(item.hwnd);
      LONG size = horizontal ? item.rect.bottom - item.rect.top
                             : item.rect.right - item.rect.left;
      if (size < 1) size = 1;
      workspace.gridWeights_.push_back(static_cast<double>(size));
    }
  }

  workspace.EnsureWeights();
  workspace.DebugValidateMutation(L"Workspace::AdoptGeometry");
  return workspace;
}

namespace Layout {

std::vector<LONG> ComputeWeightedSizes(
    LONG totalSize, LONG gap, const std::vector<double>& weights,
    LONG* outEffectiveGap = nullptr) {
  size_t count = weights.size();
  std::vector<LONG> sizes(count, 0);
  if (outEffectiveGap) *outEffectiveGap = 0;
  if (count == 0 || totalSize <= 0) return sizes;

  LONG effectiveGap = 0;
  if (count > 1 && totalSize > static_cast<LONG>(count)) {
    const LONG maximumGap =
        (totalSize - static_cast<LONG>(count)) /
        static_cast<LONG>(count - 1);
    effectiveGap = std::clamp<LONG>(gap, 0, maximumGap);
  }
  if (outEffectiveGap) *outEffectiveGap = effectiveGap;

  LONG available =
      totalSize - effectiveGap * static_cast<LONG>(count - 1);
  if (available <= 0) return sizes;

  double sum = 0.0;
  for (double weight : weights) {
    sum += std::isfinite(weight) && weight > 0.0 ? weight : 1.0;
  }

  LONG used = 0;
  double remainingSum = sum;
  for (size_t i = 0; i < count; ++i) {
    double w = weights[i];
    if (!std::isfinite(w) || w <= 0.0) w = 1.0;
    LONG remainingSlots = (LONG)(count - i - 1);
    LONG size = 0;
    if (i == count - 1) {
      size = available - used;

    } else {
      double ratio = w / remainingSum;
      size = static_cast<LONG>(std::llround(static_cast<double>(available - used) * ratio));
      if (size < 1) size = 1;
      LONG maxSize = available - used - remainingSlots;
      if (size > maxSize) size = maxSize;
    }
    sizes[i] = size;
    used += size;
    remainingSum -= w;
    if (remainingSum <= 0.0) remainingSum = 1.0;
  }
  return sizes;
}

void LayoutGridWeighted(
    const RECT& area, LONG gap, size_t windowCount,
    std::vector<RECT>& outRects, bool horizontal,
    const std::vector<double>& weights) {
  outRects.resize(windowCount);
  if (windowCount == 0) return;

  const std::vector<double>* effectiveWeights = &weights;
  std::vector<double> defaultWeights;
  if (weights.size() != windowCount) {
    defaultWeights = DefaultWeights(windowCount);
    effectiveWeights = &defaultWeights;
  }

  LONG totalSize = horizontal ? (area.bottom - area.top) : (area.right - area.left);
  LONG effectiveGap = 0;
  std::vector<LONG> sizes =
      ComputeWeightedSizes(totalSize, gap, *effectiveWeights, &effectiveGap);
  LONG position = horizontal ? area.top : area.left;

  for (size_t i = 0; i < windowCount; ++i) {
    LONG size = sizes[i];
    LONG end = (i == windowCount - 1) ? (horizontal ? area.bottom : area.right) : position + size;
    outRects[i] = horizontal ? RECT{area.left, position, area.right, end} : RECT{position, area.top, end, area.bottom};
    position = end + effectiveGap;
  }
}

void LayoutMasterStackWeighted(
    const RECT& area, LONG gap, size_t windowCount,
    std::vector<RECT>& outRects, bool horizontal,
    double masterRatio, const std::vector<double>& stackWeights) {
  outRects.resize(windowCount);
  if (windowCount == 0) return;
  if (windowCount == 1) {
    outRects[0] = area;
    return;
  }

  LONG totalSize = horizontal ? (area.bottom - area.top) : (area.right - area.left);
  if (totalSize < 2) {
    std::fill(outRects.begin(), outRects.end(), area);
    return;
  }

  const LONG masterGap = std::clamp<LONG>(gap, 0, totalSize - 2);
  LONG masterSize = static_cast<LONG>(
      std::llround((totalSize - masterGap) * masterRatio));
  if (masterSize < 1) masterSize = 1;
  if (masterSize > totalSize - masterGap - 1) {
    masterSize = totalSize - masterGap - 1;
  }

  size_t stackCount = windowCount - 1;
  std::vector<double> weights = stackWeights;
  if (weights.size() != stackCount) weights = DefaultWeights(stackCount);

  if (horizontal) {
    outRects[0] = {area.left, area.top, area.right, area.top + masterSize};
    LONG stackTop = area.top + masterSize + masterGap;
    LONG stackGap = 0;
    std::vector<LONG> sizes = ComputeWeightedSizes(
        area.right - area.left, gap, weights, &stackGap);
    LONG x = area.left;
    for (size_t i = 0; i < stackCount; ++i) {
      LONG end = (i == stackCount - 1) ? area.right : x + sizes[i];
      outRects[i + 1] = {x, stackTop, end, area.bottom};
      x = end + stackGap;
    }
  } else {
    outRects[0] = {area.left, area.top, area.left + masterSize, area.bottom};
    LONG stackLeft = area.left + masterSize + masterGap;
    LONG stackGap = 0;
    std::vector<LONG> sizes = ComputeWeightedSizes(
        area.bottom - area.top, gap, weights, &stackGap);
    LONG y = area.top;
    for (size_t i = 0; i < stackCount; ++i) {
      LONG end = (i == stackCount - 1) ? area.bottom : y + sizes[i];
      outRects[i + 1] = {stackLeft, y, area.right, end};
      y = end + stackGap;
    }
  }
}

struct DwindleSplit {
  bool vertical = false;
  LONG usableSpan = 0;
  LONG firstSpan = 0;
  RECT first{};
  RECT remaining{};
};

// Ordered-chain Dwindle: each node gives its first child to startIndex and
// recursively gives its second child to all later windows. Ratios belong to
// those split nodes, so swapping windows exchanges leaves without reshaping the
// tree. Fibonacci-style axes alternate by depth, starting side-by-side;
// resizing an ancestor never rotates a descendant's divider.
void LayoutDwindle(
    const RECT& area, LONG gap, size_t startIndex, size_t count,
    const std::vector<double>& splitRatios, bool reverse,
    std::vector<RECT>& outRects,
    std::vector<DwindleSplit>* outSplits = nullptr) {
  if (count == 0) return;
  if (count == 1) {
    outRects[startIndex] = area;
    return;
  }

  const LONG width = area.right - area.left;
  const LONG height = area.bottom - area.top;
  const bool splitVertical = (startIndex % 2) == 0;

  const LONG span = splitVertical ? width : height;
  if (span < 2) {
    std::fill(
        outRects.begin() + startIndex,
        outRects.begin() + startIndex + count, area);
    return;
  }

  const LONG effectiveGap = std::clamp<LONG>(gap, 0, span - 2);
  const LONG usableSpan = span - effectiveGap;
  const double ratio = startIndex < splitRatios.size()
      ? ClampDouble(splitRatios[startIndex], 0.1, 0.9)
      : 0.5;
  const LONG firstSpan = std::clamp<LONG>(
      static_cast<LONG>(std::llround(usableSpan * ratio)),
      1, usableSpan - 1);
  RECT remaining = area;
  if (splitVertical) {
    if (reverse) {
      outRects[startIndex] = {
          area.right - firstSpan, area.top, area.right, area.bottom};
      remaining.right = outRects[startIndex].left - effectiveGap;
    } else {
      outRects[startIndex] = {
          area.left, area.top, area.left + firstSpan, area.bottom};
      remaining.left = outRects[startIndex].right + effectiveGap;
    }
  } else {
    if (reverse) {
      outRects[startIndex] = {
          area.left, area.bottom - firstSpan, area.right, area.bottom};
      remaining.bottom = outRects[startIndex].top - effectiveGap;
    } else {
      outRects[startIndex] = {
          area.left, area.top, area.right, area.top + firstSpan};
      remaining.top = outRects[startIndex].bottom + effectiveGap;
    }
  }

  if (outSplits) {
    if (outSplits->size() < startIndex + count - 1) {
      outSplits->resize(startIndex + count - 1);
    }
    (*outSplits)[startIndex] = {
        splitVertical, usableSpan, firstSpan,
        outRects[startIndex], remaining};
  }

  LayoutDwindle(
      remaining, gap, startIndex + 1, count - 1,
      splitRatios, reverse, outRects, outSplits);
}

struct ConstraintExtent {
  LONG minWidth = 1;
  LONG minHeight = 1;
};

static bool ConstraintDimensionFits(LONG value, LONG minimum) {
  return value >= minimum;
}

// Weighted allocation with the same gap-degradation semantics as the ordinary
// layout path. Relative weights remain preferences; members pin to their native
// minimum only when required to make the full span legal.
static bool ComputeConstrainedWeightedSizes(
    LONG totalSize, LONG gap, const std::vector<double>& weights,
    const std::vector<LONG>& minimums,
    std::vector<LONG>* outSizes, LONG* outEffectiveGap = nullptr) {
  if (!outSizes) return false;
  const size_t count = minimums.size();
  outSizes->assign(count, 0);
  if (outEffectiveGap) *outEffectiveGap = 0;
  if (count == 0) return true;
  if (totalSize <= 0) return false;

  LONG effectiveGap = 0;
  if (count > 1 && totalSize > static_cast<LONG>(count)) {
    const LONG maximumGap =
        (totalSize - static_cast<LONG>(count)) /
        static_cast<LONG>(count - 1);
    effectiveGap = std::clamp<LONG>(gap, 0, maximumGap);
  }
  if (outEffectiveGap) *outEffectiveGap = effectiveGap;

  const long long available64 =
      static_cast<long long>(totalSize) -
      static_cast<long long>(effectiveGap) * (count - 1);
  if (available64 < static_cast<long long>(count) ||
      available64 > LONG_MAX) {
    return false;
  }
  const LONG available = static_cast<LONG>(available64);

  std::vector<LONG> mins(count);
  std::vector<double> normalizedWeights(count, 1.0);
  long long minSum = 0;
  for (size_t i = 0; i < count; ++i) {
    mins[i] = std::max<LONG>(1, minimums[i]);
    minSum += mins[i];
    if (i < weights.size() &&
        std::isfinite(weights[i]) && weights[i] > 0.0) {
      normalizedWeights[i] = weights[i];
    }
  }
  if (minSum > available) return false;

  auto sumAt = [&](double lambda) {
    double sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
      sum += std::max(
          lambda * normalizedWeights[i], static_cast<double>(mins[i]));
    }
    return sum;
  };

  double low = 0.0;
  double high = 1.0;
  while (sumAt(high) < static_cast<double>(available) && high < 1.0e12) {
    high *= 2.0;
  }
  for (int iteration = 0; iteration < 80; ++iteration) {
    const double mid = (low + high) * 0.5;
    if (sumAt(mid) < static_cast<double>(available)) {
      low = mid;
    } else {
      high = mid;
    }
  }

  std::vector<double> raw(count);
  long long roundedSum = 0;
  for (size_t i = 0; i < count; ++i) {
    raw[i] = std::max(
        high * normalizedWeights[i], static_cast<double>(mins[i]));
    LONG rounded = static_cast<LONG>(std::floor(raw[i]));
    rounded = std::max(rounded, mins[i]);
    (*outSizes)[i] = rounded;
    roundedSum += rounded;
  }

  while (roundedSum < available) {
    size_t best = count;
    double bestFraction = -1.0;
    for (size_t i = 0; i < count; ++i) {
      const double fraction = raw[i] - std::floor(raw[i]);
      if (best == count || fraction > bestFraction) {
        best = i;
        bestFraction = fraction;
      }
    }
    if (best == count) return false;
    ++(*outSizes)[best];
    ++roundedSum;
    raw[best] = static_cast<double>((*outSizes)[best]);
  }

  while (roundedSum > available) {
    size_t best = count;
    double bestFraction = 2.0;
    for (size_t i = 0; i < count; ++i) {
      if ((*outSizes)[i] <= mins[i]) continue;
      const double fraction = raw[i] - std::floor(raw[i]);
      if (best == count || fraction < bestFraction) {
        best = i;
        bestFraction = fraction;
      }
    }
    if (best == count) return false;
    --(*outSizes)[best];
    --roundedSum;
    raw[best] = static_cast<double>((*outSizes)[best]);
  }
  return true;
}

static ConstraintExtent ToConstraintExtent(
    const Model::WindowConstraints& c) {
  const LONG minWidth =
      c.valid ? std::max<LONG>(1, c.minWidth) : 1;
  const LONG minHeight =
      c.valid ? std::max<LONG>(1, c.minHeight) : 1;
  return {minWidth, minHeight};
}

static bool ConstraintAreaFits(
    const RECT& area, const ConstraintExtent& c) {
  const LONG width = area.right - area.left;
  const LONG height = area.bottom - area.top;
  return width > 0 && height > 0 &&
         ConstraintDimensionFits(width, c.minWidth) &&
         ConstraintDimensionFits(height, c.minHeight);
}

static bool ChooseConstrainedSplit(
    LONG span, LONG gap, double preferredFirstRatio,
    LONG firstMin, LONG secondMin,
    LONG* outFirstSize, LONG* outEffectiveGap = nullptr) {
  if (!outFirstSize || span < 2) return false;
  const LONG effectiveGap = std::clamp<LONG>(gap, 0, span - 2);
  if (outEffectiveGap) *outEffectiveGap = effectiveGap;
  const LONG available = span - effectiveGap;

  const LONG low = std::max<LONG>(1, firstMin);
  const LONG high = available - std::max<LONG>(1, secondMin);
  if (low > high) return false;

  const LONG preferred = static_cast<LONG>(
      std::llround(static_cast<double>(available) * preferredFirstRatio));
  *outFirstSize = std::clamp(preferred, low, high);
  return true;
}

bool LayoutGridConstrained(
    const RECT& area, LONG gap, const std::vector<double>& weights,
    const std::vector<Model::WindowConstraints>& constraints,
    bool horizontal, std::vector<RECT>* outRects) {
  if (!outRects) return false;
  const size_t count = constraints.size();
  outRects->assign(count, RECT{});
  if (count == 0) return true;

  const LONG crossSpan =
      horizontal ? area.right - area.left : area.bottom - area.top;
  std::vector<LONG> mins(count);
  for (size_t i = 0; i < count; ++i) {
    const ConstraintExtent c = ToConstraintExtent(constraints[i]);
    if (horizontal) {
      if (!ConstraintDimensionFits(crossSpan, c.minWidth)) {
        return false;
      }
      mins[i] = c.minHeight;
    } else {
      if (!ConstraintDimensionFits(crossSpan, c.minHeight)) {
        return false;
      }
      mins[i] = c.minWidth;
    }
  }

  const LONG total =
      horizontal ? area.bottom - area.top : area.right - area.left;
  LONG effectiveGap = 0;
  std::vector<LONG> sizes;
  if (!ComputeConstrainedWeightedSizes(
          total, gap, weights, mins, &sizes, &effectiveGap)) {
    return false;
  }

  LONG position = horizontal ? area.top : area.left;
  for (size_t i = 0; i < count; ++i) {
    const LONG end =
        i + 1 == count
            ? (horizontal ? area.bottom : area.right)
            : position + sizes[i];
    (*outRects)[i] = horizontal
        ? RECT{area.left, position, area.right, end}
        : RECT{position, area.top, end, area.bottom};
    position = end + effectiveGap;
  }
  return true;
}

bool LayoutMasterStackConstrained(
    const RECT& area, LONG gap, bool horizontal, double masterRatio,
    const std::vector<double>& stackWeights,
    const std::vector<Model::WindowConstraints>& constraints,
    std::vector<RECT>* outRects) {
  if (!outRects) return false;
  const size_t count = constraints.size();
  outRects->assign(count, RECT{});
  if (count == 0) return true;
  if (count == 1) {
    if (!ConstraintAreaFits(area, ToConstraintExtent(constraints[0]))) {
      return false;
    }
    (*outRects)[0] = area;
    return true;
  }

  const ConstraintExtent master = ToConstraintExtent(constraints[0]);
  const LONG areaWidth = area.right - area.left;
  const LONG areaHeight = area.bottom - area.top;
  const LONG splitSpan = horizontal ? areaHeight : areaWidth;
  const LONG innerSpan = horizontal ? areaWidth : areaHeight;

  if (horizontal) {
    if (!ConstraintDimensionFits(areaWidth, master.minWidth)) {
      return false;
    }
  } else if (!ConstraintDimensionFits(areaHeight, master.minHeight)) {
    return false;
  }

  LONG stackSplitMin = 1;
  std::vector<LONG> innerMins;
  innerMins.reserve(count - 1);
  for (size_t i = 1; i < count; ++i) {
    const ConstraintExtent c = ToConstraintExtent(constraints[i]);
    if (horizontal) {
      stackSplitMin = std::max(stackSplitMin, c.minHeight);
      innerMins.push_back(c.minWidth);
    } else {
      stackSplitMin = std::max(stackSplitMin, c.minWidth);
      innerMins.push_back(c.minHeight);
    }
  }

  LONG masterSize = 0;
  LONG masterGap = 0;
  if (!ChooseConstrainedSplit(
          splitSpan, gap, masterRatio,
          horizontal ? master.minHeight : master.minWidth,
          stackSplitMin,
          &masterSize, &masterGap)) {
    return false;
  }

  LONG stackGap = 0;
  std::vector<LONG> innerSizes;
  if (!ComputeConstrainedWeightedSizes(
          innerSpan, gap, stackWeights, innerMins,
          &innerSizes, &stackGap)) {
    return false;
  }

  if (horizontal) {
    (*outRects)[0] = {
        area.left, area.top, area.right, area.top + masterSize};
    const LONG stackTop = area.top + masterSize + masterGap;
    LONG x = area.left;
    for (size_t i = 0; i < innerSizes.size(); ++i) {
      const LONG end =
          i + 1 == innerSizes.size() ? area.right : x + innerSizes[i];
      (*outRects)[i + 1] = {x, stackTop, end, area.bottom};
      x = end + stackGap;
    }
  } else {
    (*outRects)[0] = {
        area.left, area.top, area.left + masterSize, area.bottom};
    const LONG stackLeft = area.left + masterSize + masterGap;
    LONG y = area.top;
    for (size_t i = 0; i < innerSizes.size(); ++i) {
      const LONG end =
          i + 1 == innerSizes.size() ? area.bottom : y + innerSizes[i];
      (*outRects)[i + 1] = {stackLeft, y, area.right, end};
      y = end + stackGap;
    }
  }
  return true;
}

bool LayoutMonocleConstrained(
    const RECT& area,
    const std::vector<Model::WindowConstraints>& constraints,
    std::vector<RECT>* outRects) {
  if (!outRects) return false;
  outRects->assign(constraints.size(), area);
  for (const auto& constraint : constraints) {
    if (!ConstraintAreaFits(area, ToConstraintExtent(constraint))) {
      return false;
    }
  }
  return true;
}


}  // namespace Layout

//=============================================================================
// Window observation + eligibility
//=============================================================================

// Win32 observation lives here, outside Model. These helpers translate live HWND
// properties/geometry into value objects consumed by Workspace and reconciliation.

constexpr UINT kConstraintProbeTimeoutMs = 40;

static int GetSystemMetricForDpiCompat(int index, UINT dpi) {
  using GetSystemMetricsForDpiFn = int(WINAPI*)(int, UINT);
  static GetSystemMetricsForDpiFn fn = [] {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    return user32
        ? reinterpret_cast<GetSystemMetricsForDpiFn>(
              GetProcAddress(user32, "GetSystemMetricsForDpi"))
        : nullptr;
  }();
  return fn ? fn(index, dpi ? dpi : 96) : GetSystemMetrics(index);
}

static void GetFrameDecorationSpan(
    HWND hwnd, LONG* outWidth, LONG* outHeight) {
  if (outWidth) *outWidth = 0;
  if (outHeight) *outHeight = 0;

  RECT windowRect{};
  RECT frame{};
  if (!hwnd ||
      !GetWindowRect(hwnd, &windowRect) ||
      FAILED(DwmGetWindowAttribute(
          hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
          &frame, sizeof(frame)))) {
    return;
  }

  if (outWidth) {
    *outWidth = std::max<LONG>(
        0,
        (frame.left - windowRect.left) +
        (windowRect.right - frame.right));
  }
  if (outHeight) {
    *outHeight = std::max<LONG>(
        0,
        (frame.top - windowRect.top) +
        (windowRect.bottom - frame.bottom));
  }
}

// WM_GETMINMAXINFO can execute target-process window procedure code. Bound the
// synchronous query so a hung client cannot block the serialized WM actor. A
// failed probe remains unconstrained; a previously valid observation is retained
// by the evidence-merge path.
static WindowConstraints ProbeWindowConstraints(HWND hwnd) {
  ++Diagnostics::g_runtime.counters.constraintProbeAttempts;
  WindowConstraints constraints;

  if (!hwnd || !IsWindow(hwnd)) {
    constraints.lastProbeError = ERROR_INVALID_WINDOW_HANDLE;
    ++Diagnostics::g_runtime.counters.constraintProbeFailures;
    return constraints;
  }
  if (IsHungAppWindow(hwnd)) {
    constraints.lastProbeError = ERROR_TIMEOUT;
    ++Diagnostics::g_runtime.counters.constraintProbeTimeouts;
    return constraints;
  }

  HMONITOR monitor =
      MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  constraints.dpi = GetMonitorEffectiveDpi(monitor);

  const LONG defaultMinWidth = std::max<LONG>(
      1, GetSystemMetricForDpiCompat(SM_CXMINTRACK, constraints.dpi));
  const LONG defaultMinHeight = std::max<LONG>(
      1, GetSystemMetricForDpiCompat(SM_CYMINTRACK, constraints.dpi));
  const LONG defaultMaxWidth = std::max<LONG>(
      defaultMinWidth,
      GetSystemMetricForDpiCompat(SM_CXMAXTRACK, constraints.dpi));
  const LONG defaultMaxHeight = std::max<LONG>(
      defaultMinHeight,
      GetSystemMetricForDpiCompat(SM_CYMAXTRACK, constraints.dpi));

  MINMAXINFO mmi{};
  mmi.ptMinTrackSize = {defaultMinWidth, defaultMinHeight};
  mmi.ptMaxTrackSize = {defaultMaxWidth, defaultMaxHeight};

  DWORD_PTR result = 0;
  SetLastError(ERROR_SUCCESS);
  const LRESULT delivered =
      hwnd && IsWindow(hwnd)
          ? SendMessageTimeoutW(
                hwnd, WM_GETMINMAXINFO, 0,
                reinterpret_cast<LPARAM>(&mmi),
                SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT,
                kConstraintProbeTimeoutMs, &result)
          : 0;
  if (!delivered) {
    DWORD error = GetLastError();
    if (error == ERROR_SUCCESS) error = ERROR_GEN_FAILURE;
    constraints.lastProbeError = error;
    if (error == ERROR_TIMEOUT) {
      ++Diagnostics::g_runtime.counters.constraintProbeTimeouts;
    } else {
      ++Diagnostics::g_runtime.counters.constraintProbeFailures;
    }
    return constraints;
  }

  constraints.valid = true;
  ++Diagnostics::g_runtime.counters.constraintProbeSuccesses;

  LONG decorationWidth = 0;
  LONG decorationHeight = 0;
  GetFrameDecorationSpan(
      hwnd, &decorationWidth, &decorationHeight);

  constraints.minWidth = std::max<LONG>(
      1,
      std::max<LONG>(defaultMinWidth, mmi.ptMinTrackSize.x) -
          decorationWidth);
  constraints.minHeight = std::max<LONG>(
      1,
      std::max<LONG>(defaultMinHeight, mmi.ptMinTrackSize.y) -
          decorationHeight);

  return constraints;
}

static bool SameConstraintGeometry(
    const WindowConstraints& a, const WindowConstraints& b) {
  return a.minWidth == b.minWidth &&
         a.minHeight == b.minHeight &&
         a.dpi == b.dpi &&
         a.valid == b.valid;
}

static WindowConstraints MergeConstraintProbeWithCache(
    const WindowConstraints& cached, WindowConstraints observed) {
  if (!observed.valid && cached.valid) {
    const DWORD error = observed.lastProbeError;
    observed = cached;
    observed.lastProbeError = error;
  }
  return observed;
}

static LONG RectWidth(const RECT& rect) {
  return rect.right - rect.left;
}

static LONG RectHeight(const RECT& rect) {
  return rect.bottom - rect.top;
}

static WindowRecord MakeWindowRecord(HWND hwnd, ManageState state = ManageState::Tiled) {
  WindowRecord record;
  record.hwnd = hwnd;
  record.state = state;

  if (hwnd) {
    GetWindowThreadProcessId(hwnd, &record.pid);
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    record.canMove = (GetAncestor(hwnd, GA_ROOT) == hwnd) && !(style & WS_CHILD);
    record.canResize = (style & WS_SIZEBOX) != 0;
    record.topmost = (exStyle & WS_EX_TOPMOST) != 0;

    RECT frame{};
    HMONITOR monitor = GetWindowPhysicalMonitor(hwnd);
    // Only a participating restored window exposes meaningful natural/floating
    // geometry. Suspended admission should fall back to the configured size.
    if (monitor &&
        GetPhysicalSuspensionReason(hwnd) == SuspensionReason::None &&
        GetWindowFrameRect(hwnd, &frame) &&
        frame.right > frame.left && frame.bottom > frame.top) {
      record.floatingRect = frame;
      record.floatingMonitor = monitor;
      record.floatingDpi = GetMonitorEffectiveDpi(monitor);
      record.hasFloatingRect = true;
    }
  }

  return record;
}

// Maps an event or foreground HWND to its active tile using only unambiguous
// HWND identity and ownership relationships.
HWND ResolveToTiledWindow(HWND hwnd, const std::vector<HWND>& candidates) {
  if (!hwnd || !IsWindow(hwnd)) return nullptr;

  if (ContainsWindow(candidates, hwnd)) {
    return hwnd;
  }

  HWND rootOwner = GetAncestor(hwnd, GA_ROOTOWNER);
  if (rootOwner && ContainsWindow(candidates, rootOwner)) {
    return rootOwner;
  }

  HWND owner = GetWindow(hwnd, GW_OWNER);
  while (owner) {
    if (ContainsWindow(candidates, owner)) {
      return owner;
    }
    owner = GetWindow(owner, GW_OWNER);
  }

  return nullptr;
}

std::vector<HWND> CollectTileWindows(HMONITOR monitor) {
  std::vector<HWND> windows;
  if (!monitor) return windows;

  struct EnumContext {
    HMONITOR targetMonitor;
    std::vector<HWND>* windowList;
  } context = {monitor, &windows};

  ++Diagnostics::g_runtime.counters.enumWindowsPasses;
  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        ++Diagnostics::g_runtime.counters.enumWindowsVisited;
        auto* ctx = reinterpret_cast<EnumContext*>(lParam);
        if (GetCurrentManagedWindowMonitor(hwnd) == ctx->targetMonitor) {
          ctx->windowList->push_back(hwnd);
          ++Diagnostics::g_runtime.counters.tileCandidatesCollected;
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return windows;
}

// Deferred reconciliation often needs snapshots for several monitors at once.
// Enumerate top-level HWNDs once and bucket each eligible window by monitor instead
// of repeating EnumWindows (and its DWM/VD checks) once per monitor.
static std::vector<std::vector<HWND>> CollectTileWindowsForMonitors(
    const std::vector<HMONITOR>& monitors) {
  std::vector<std::vector<HWND>> snapshots(monitors.size());
  if (monitors.empty()) return snapshots;

  struct EnumContext {
    const std::vector<HMONITOR>* monitors;
    std::vector<std::vector<HWND>>* snapshots;
  } context = {&monitors, &snapshots};

  ++Diagnostics::g_runtime.counters.enumWindowsPasses;
  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        ++Diagnostics::g_runtime.counters.enumWindowsVisited;
        auto* ctx = reinterpret_cast<EnumContext*>(lParam);
        const HMONITOR monitor = GetCurrentManagedWindowMonitor(hwnd);
        if (!monitor) return TRUE;

        auto it = std::find(ctx->monitors->begin(), ctx->monitors->end(), monitor);
        if (it == ctx->monitors->end()) return TRUE;

        const size_t index = static_cast<size_t>(it - ctx->monitors->begin());
        (*ctx->snapshots)[index].push_back(hwnd);
        ++Diagnostics::g_runtime.counters.tileCandidatesCollected;
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return snapshots;
}

// Captures Win32 geometry/capability observations before the pure Workspace
// importer interprets them as ordering and layout weights.
static std::vector<WorkspaceGeometryItem> CaptureWorkspaceGeometry(
    const std::vector<HWND>& windows, HMONITOR monitor, const RECT& workArea) {
  std::vector<WorkspaceGeometryItem> items;
  items.reserve(windows.size());
  for (HWND hwnd : windows) {
    RECT rect{};
    if (!GetWindowFrameRect(hwnd, &rect) ||
        MonitorFromRect(&rect, MONITOR_DEFAULTTONULL) != monitor) {
      rect = workArea;
    }
    items.push_back({MakeWindowRecord(hwnd, ManageState::Tiled), rect});
  }
  return items;
}

static const wchar_t* kIgnoredWindowClasses[] = {
  L"Progman",
  L"WorkerW",
  L"Shell_TrayWnd",
  L"Shell_SecondaryTrayWnd",
  L"Windows.UI.Core.CoreWindow",
  L"OperationStatusWindow",
};

bool IsWindowCloaked(HWND hwnd) {
  BOOL cloaked = FALSE;
  DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
  return cloaked;
}

static bool IsWindowTrackedInAnyState(HWND hwnd) {
  // Exact HWND identity only; child/owned lifecycle events do not inherit the
  // managed state of their top-level/root-owner window.
  return g_workspaces.IsTracked(hwnd);
}

static bool EqualsInsensitive(const std::wstring& a, const std::wstring& b) {
  return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

static bool ContainsInsensitive(const std::wstring& text, const std::wstring& needle) {
  if (needle.empty()) return false;
  return std::search(
             text.begin(), text.end(), needle.begin(), needle.end(),
             [](wchar_t a, wchar_t b) { return std::towlower(a) == std::towlower(b); }) != text.end();
}

static std::wstring GetWindowProcessName(HWND hwnd) {
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (!pid) return {};

  HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!process) return {};

  wchar_t path[32768]{};
  DWORD chars = ARRAYSIZE(path);
  std::wstring result;
  if (QueryFullProcessImageNameW(process, 0, path, &chars) && chars) {
    const wchar_t* base = path;
    for (DWORD i = 0; i < chars; ++i) {
      if (path[i] == L'\\' || path[i] == L'/') base = path + i + 1;
    }
    result.assign(base, static_cast<size_t>((path + chars) - base));
  }
  CloseHandle(process);
  return result;
}

static const WindowRule* FindMatchingWindowRule(
    HWND hwnd, WindowRuleTreatment treatment) {
  if (!hwnd || g_settings.windowRules.empty()) return nullptr;

  std::wstring processName;
  std::wstring className;
  std::wstring title;
  bool haveProcess = false;
  bool haveClass = false;
  bool haveTitle = false;

  for (const auto& rule : g_settings.windowRules) {
    if (rule.treatment != treatment) continue;

    if (!rule.classNames.empty()) {
      if (!haveClass) {
        wchar_t buffer[256]{};
        if (GetClassNameW(hwnd, buffer, ARRAYSIZE(buffer))) className = buffer;
        haveClass = true;
      }
      if (std::none_of(
              rule.classNames.begin(), rule.classNames.end(),
              [&](const std::wstring& candidate) {
                return EqualsInsensitive(className, candidate);
              })) {
        continue;
      }
    }

    if (!rule.titleFragments.empty()) {
      if (!haveTitle) {
        wchar_t buffer[512]{};
        if (GetWindowTextW(hwnd, buffer, ARRAYSIZE(buffer))) title = buffer;
        haveTitle = true;
      }
      if (std::none_of(
              rule.titleFragments.begin(), rule.titleFragments.end(),
              [&](const std::wstring& candidate) {
                return ContainsInsensitive(title, candidate);
              })) {
        continue;
      }
    }

    if (!rule.processNames.empty()) {
      if (!haveProcess) {
        processName = GetWindowProcessName(hwnd);
        haveProcess = true;
      }
      if (std::none_of(
              rule.processNames.begin(), rule.processNames.end(),
              [&](const std::wstring& candidate) {
                return EqualsInsensitive(processName, candidate);
              })) {
        continue;
      }
    }

    return &rule;
  }
  return nullptr;
}

// Discovery waits for initial placement before assigning workspace ownership. Explicit
// admission bypasses this policy and overrides pending initial placement.
static bool IsWindowAwaitingInitialPlacement(HWND hwnd) {
  return g_wm.windowRoutingReady &&
         (g_wm.pendingWindowRoutes.count(hwnd) ||
          (!g_wm.windowRoutingEvaluated.count(hwnd) &&
           FindMatchingWindowRule(hwnd, WindowRuleTreatment::InitialPlacement)));
}

static bool HasMonitorPlacementRule(HWND hwnd) {
  const WindowRule* rule = FindMatchingWindowRule(hwnd, WindowRuleTreatment::InitialPlacement);
  return rule && (rule->monitor != RuleMonitor::Any || !rule->monitorId.empty());
}

static bool IsWindowExcludedByRules(HWND hwnd) {
  return FindMatchingWindowRule(hwnd, WindowRuleTreatment::Exclude) != nullptr;
}

static bool ShouldWindowAlwaysFloat(HWND hwnd) {
  return FindMatchingWindowRule(
             hwnd, WindowRuleTreatment::AlwaysFloating) != nullptr;
}

static bool ShouldWindowStartFloating(HWND hwnd, bool* alwaysFloating = nullptr) {
  const bool always = ShouldWindowAlwaysFloat(hwnd);
  if (alwaysFloating) *alwaysFloating = always;
  return always || FindMatchingWindowRule(
                       hwnd, WindowRuleTreatment::DefaultToFloating) != nullptr;
}

// Applies stable structural and exclusion policy only; visibility, show state,
// desktop, and monitor participation are checked separately. Tracked windows
// tolerate a temporarily missing resize style so transient app UI cannot evict them.
static bool WindowCanBeManaged(HWND hwnd, bool allowFixedSize) {
  if (!hwnd) return false;

  // Reject structurally ineligible HWNDs before evaluating configurable exclusion
  // rules. Process-name exclusions can require OpenProcess/QueryFullProcessImageName,
  // so there is no reason to pay that cost for tool/owned/child windows.
  if (GetAncestor(hwnd, GA_ROOT) != hwnd || GetWindow(hwnd, GW_OWNER)) return false;

  LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
  if (style & WS_CHILD) return false;

  // window could temporarily lose its WS_SizeBox.
  // (example: Office save/discard dialog)
  // A Default-to-floating rule is also an explicit Automatic-mode admission
  // policy for naturally fixed-size top-level windows. Always-floating is also
  // eligible during explicit Manual admission; centering never resizes either.
  if (!allowFixedSize && !(style & WS_SIZEBOX) &&
      !IsWindowTrackedInAnyState(hwnd)) {
    bool alwaysFloating = false;
    const bool startsFloating =
        ShouldWindowStartFloating(hwnd, &alwaysFloating);
    if (!alwaysFloating && !(IsAutomaticMode() && startsFloating)) return false;
  }

  LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
  if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;

  wchar_t className[64];
  if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
    for (const auto* ignoredClass : kIgnoredWindowClasses) {
      if (_wcsicmp(className, ignoredClass) == 0) return false;
    }
  }

  return !IsWindowExcludedByRules(hwnd);
}

// Returns the monitor on which an HWND can actively participate right now.
// A null result means the window is not currently tile-eligible.
static HMONITOR GetCurrentManagedWindowMonitor(HWND hwnd) {
  // window might become temporarily disabled
  // (example: VSCode unstaged commit dialog)
  if (!IsWindowTrackedInAnyState(hwnd) && !IsWindowEnabled(hwnd)) return nullptr;

  // Minimized, hidden, maximized, and fullscreen windows are not active layout
  // participants. Their logical records retain enough state to restore in place.
  if (GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None) return nullptr;
  if (!WindowCanBeManaged(hwnd)) return nullptr;
  if (IsWindowCloaked(hwnd)) return nullptr;

  // Virtual desktop filter: other desktops' windows may not yet be cloaked
  // at the moment we enumerate (race between uncloak/cloak during switch).
  // Fail closed when the desktop predicate is temporarily unavailable; the
  // centralized runtime-maintenance path will recover the COM service and retry.
  BOOL onCurrent = FALSE;
  if (!IsWindowOnCurrentDesktopSafe(hwnd, &onCurrent) || !onCurrent) {
    return nullptr;
  }

  return GetWindowPhysicalMonitor(hwnd);
}

static bool RectsNear(const RECT& a, const RECT& b, LONG tolerance = Model::kPlacementTolerancePx) {
  return !Differs(a.left, b.left, tolerance) &&
         !Differs(a.top, b.top, tolerance) &&
         !Differs(a.right, b.right, tolerance) &&
         !Differs(a.bottom, b.bottom, tolerance);
}

// Positions one observed HWND without mutating logical workspace state. The
// resulting value object is folded into Workspace separately.
static PlacementObservation PlaceWindowChecked(
    HWND hwnd, bool canMove, const RECT& targetRect,
    bool positionOnly = false) {
  ++Diagnostics::g_runtime.counters.placementChecks;
  PlacementObservation observation;
  observation.requested = targetRect;

  auto finish = [&](PlacementResult result) {
    observation.result = result;
    switch (result) {
      case PlacementResult::Success:
        ++Diagnostics::g_runtime.counters.placementSuccess;
        break;
      case PlacementResult::SuppressedByPhysicalState:
        ++Diagnostics::g_runtime.counters.placementSuppressedByPhysicalState;
        break;
      case PlacementResult::AdjustedByWindow:
        ++Diagnostics::g_runtime.counters.placementAdjusted;
        break;
      case PlacementResult::AccessDenied:
        ++Diagnostics::g_runtime.counters.placementAccessDenied;
        break;
      case PlacementResult::Refused:
        ++Diagnostics::g_runtime.counters.placementRefused;
        break;
      case PlacementResult::Dead:
        ++Diagnostics::g_runtime.counters.placementDead;
        break;
    }
    if (result != PlacementResult::Success &&
        result != PlacementResult::SuppressedByPhysicalState) {
      Diagnostics::RecordEvent(
          L"placement result=%d hwnd=%p target=[%ld,%ld,%ld,%ld]",
          static_cast<int>(result), hwnd, targetRect.left, targetRect.top,
          targetRect.right, targetRect.bottom);
    }
    return observation;
  };

  if (!hwnd || !IsWindow(hwnd)) {
    ++Diagnostics::g_runtime.counters.placementPreflightStops;
    return finish(PlacementResult::Dead);
  }
  if (!canMove) {
    ++Diagnostics::g_runtime.counters.placementPreflightStops;
    return finish(PlacementResult::Refused);
  }
  if (IsHungAppWindow(hwnd)) {
    ++Diagnostics::g_runtime.counters.placementPreflightStops;
    return finish(PlacementResult::Refused);
  }

  RECT currentFrame{};
  if (GetWindowFrameRect(hwnd, &currentFrame)) {
    observation.observed = currentFrame;
    if (RectsNear(currentFrame, targetRect)) {
      ++Diagnostics::g_runtime.counters.placementNoOps;
      return finish(PlacementResult::Success);
    }
  }

  RECT windowRect{}, extendedFrame{};
  LONG offsetLeft = 0, offsetTop = 0, offsetRight = 0, offsetBottom = 0;
  if (GetWindowRect(hwnd, &windowRect) &&
      SUCCEEDED(DwmGetWindowAttribute(
          hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extendedFrame, sizeof(extendedFrame)))) {
    offsetLeft = extendedFrame.left - windowRect.left;
    offsetTop = extendedFrame.top - windowRect.top;
    offsetRight = windowRect.right - extendedFrame.right;
    offsetBottom = windowRect.bottom - extendedFrame.bottom;
  }

  // Close the final physical-state race at the actual mutation boundary. The
  // application can maximize/fullscreen/minimize itself after arrangement's
  // earlier eligibility check but before this SetWindowPos. Treat that transition
  // as a benign suppressed placement, never as refusal/nonconformance.
  if (GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
      IsWindowCloaked(hwnd)) {
    ++Diagnostics::g_runtime.counters.placementPreflightStops;
    return finish(PlacementResult::SuppressedByPhysicalState);
  }

  SetLastError(ERROR_SUCCESS);
  ++Diagnostics::g_runtime.counters.setWindowPosCalls;
  observation.placementIssued = true;
  BOOL placed = SetWindowPos(
      hwnd, nullptr,
      targetRect.left - offsetLeft,
      targetRect.top - offsetTop,
      positionOnly
          ? 0
          : targetRect.right - targetRect.left + offsetLeft + offsetRight,
      positionOnly
          ? 0
          : targetRect.bottom - targetRect.top + offsetTop + offsetBottom,
      SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE |
          SWP_NOSENDCHANGING | (positionOnly ? SWP_NOSIZE : 0));
  if (!placed) {
    const DWORD error = GetLastError();
    return finish(error == ERROR_ACCESS_DENIED
                      ? PlacementResult::AccessDenied
                      : PlacementResult::Refused);
  }

  RECT observed{};
  if (!GetWindowFrameRect(hwnd, &observed)) {
    return finish(IsWindow(hwnd) ? PlacementResult::Refused
                                 : PlacementResult::Dead);
  }

  observation.observed = observed;
  return finish(RectsNear(observed, targetRect)
                    ? PlacementResult::Success
                    : PlacementResult::AdjustedByWindow);
}

static void ArrangeWorkspace(
    const DesktopMonitorKey& key, HWND preferredTiledWindow = nullptr);
static bool ClearSettledInsertionPlacements(
    const DesktopMonitorKey& key, Workspace& workspace);

static void ClearMoveSizeSamples(HWND hwnd) {
  AssertWmThread(L"ClearMoveSizeSamples");
  g_moveSize.startRects.erase(hwnd);
  g_moveSize.endRects.erase(hwnd);
  g_moveSize.endPoints.erase(hwnd);
}

static void ClearAllMoveSizeSamples() {
  AssertWmThread(L"ClearAllMoveSizeSamples");
  g_moveSize.startRects.clear();
  g_moveSize.endRects.clear();
  g_moveSize.endPoints.clear();
}





//=============================================================================
// Workspace repository + authoritative model
//=============================================================================

static inline long long RectAreaLL(const RECT& r) {
  long long w = (long long)r.right - (long long)r.left;
  long long h = (long long)r.bottom - (long long)r.top;
  if (w <= 0 || h <= 0) return 0;
  return w * h;
}

static inline long long WindowAreaOnWorkArea(HWND hwnd, const RECT& workArea) {
  RECT r{};
  if (!GetWindowFrameRect(hwnd, &r)) return 0;
  RECT inter{};
  if (!IntersectRect(&inter, &r, &workArea)) return 0;
  return RectAreaLL(inter);
}

// Monitor targeting policy is intentionally explicit:
// - workspace-level user commands follow the cursor, because this mod doesn't
//   maintain a separate WM "focused monitor" state;
// - window-level commands derive ownership from the focused HWND itself;
// - passive UI refreshes use the foreground window's monitor.
static HMONITOR GetCursorMonitor() {
  POINT cursorPos{};
  if (!GetCursorPos(&cursorPos)) return nullptr;
  return MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
}

static HMONITOR GetForegroundMonitor() {
  HWND foregroundWindow = GetForegroundWindow();
  return foregroundWindow ? GetWindowPhysicalMonitor(foregroundWindow) : nullptr;
}

static HMONITOR GetWorkspaceCommandMonitor() {
  HMONITOR monitor = GetCursorMonitor();
  return monitor ? monitor : GetForegroundMonitor();
}

struct WorkspaceMetrics {
  RECT workArea{};
  UINT dpi = 96;
  LONG gap = 0;
};

static bool GetWorkspaceMetrics(HMONITOR monitor, WorkspaceMetrics* out) {
  if (!monitor || !out) return false;

  RECT monitorWork{};
  if (!GetMonitorWorkArea(monitor, &monitorWork)) return false;

  const UINT dpi = GetMonitorEffectiveDpi(monitor);
  const WorkspaceInsets insets{
      ScaleDip(g_settings.insetsDip.left, dpi),
      ScaleDip(g_settings.insetsDip.top, dpi),
      ScaleDip(g_settings.insetsDip.right, dpi),
      ScaleDip(g_settings.insetsDip.bottom, dpi)};
  const RECT workArea{
      monitorWork.left + insets.left,
      monitorWork.top + insets.top,
      monitorWork.right - insets.right,
      monitorWork.bottom - insets.bottom};
  if (workArea.right <= workArea.left || workArea.bottom <= workArea.top) {
    return false;
  }

  out->workArea = workArea;
  out->dpi = dpi;
  out->gap = ScaleDip(g_settings.gapDip, dpi);
  return true;
}

static bool GetWorkspaceWorkArea(HMONITOR monitor, RECT* outWorkArea) {
  if (!outWorkArea) return false;
  WorkspaceMetrics metrics;
  if (!GetWorkspaceMetrics(monitor, &metrics)) return false;
  *outWorkArea = metrics.workArea;
  return true;
}

enum class FloatingPlacementIntent {
  PassiveRestore,
  PreserveAnchor,
  CenterWithFloatingSize,
  NewWindowCenter,
  CenterPreservingSize,
  FailureCenter,
};

struct FloatingPlacementHint {
  FloatingPlacementIntent intent = FloatingPlacementIntent::PassiveRestore;
  POINT anchor{};
};

static POINT RectCenter(const RECT& rect) {
  return {
      rect.left + (rect.right - rect.left) / 2,
      rect.top + (rect.bottom - rect.top) / 2};
}

static RECT CenteredRect(POINT center, LONG width, LONG height) {
  width = std::max<LONG>(1, width);
  height = std::max<LONG>(1, height);
  const LONG left = center.x - width / 2;
  const LONG top = center.y - height / 2;
  return {left, top, left + width, top + height};
}

static RECT ClampFloatingRectToWorkArea(RECT rect, const RECT& workArea) {
  const LONG areaWidth = std::max<LONG>(1, workArea.right - workArea.left);
  const LONG areaHeight = std::max<LONG>(1, workArea.bottom - workArea.top);
  const LONG width = std::min<LONG>(
      std::max<LONG>(1, rect.right - rect.left), areaWidth);
  const LONG height = std::min<LONG>(
      std::max<LONG>(1, rect.bottom - rect.top), areaHeight);

  LONG left = rect.left;
  LONG top = rect.top;
  if (left < workArea.left) left = workArea.left;
  if (top < workArea.top) top = workArea.top;
  if (left + width > workArea.right) left = workArea.right - width;
  if (top + height > workArea.bottom) top = workArea.bottom - height;
  return {left, top, left + width, top + height};
}

static FloatingPlacementHint MakeFloatingPlacementHintFromGesture(
    const MoveSizeGesture& gesture) {
  FloatingPlacementHint hint;
  if (gesture.intent != MoveSizeIntent::Move) return hint;

  hint.intent = FloatingPlacementIntent::PreserveAnchor;
  // Preserve the moved window's final center rather than the mouse pointer. The
  // pointer is usually on the title bar, so centering a repaired window on it
  // would visibly jump away from the user's chosen placement.
  if (gesture.hasRects) {
    hint.anchor = RectCenter(gesture.end);
  } else if (gesture.hasDropPoint) {
    hint.anchor = gesture.dropPoint;
  }
  return hint;
}

static HMONITOR ResolveRememberedFloatingMonitor(
    const WindowRecord& record) {
  if (!record.hasFloatingRect) return nullptr;
  if (IsLiveMonitorHandle(record.floatingMonitor)) {
    return record.floatingMonitor;
  }

  // HMONITOR can be invalidated by a live display-topology rebuild. The remembered
  // physical rectangle is still a useful best-effort hint inside the same session;
  // if it maps to a current monitor, treat that as the remembered boundary.
  return MonitorFromRect(
      &record.floatingRect, MONITOR_DEFAULTTONULL);
}

// Converts remembered floating geometry into a destination-monitor rectangle.
// Passive migration restores the full rectangle on the same monitor and centers
// remembered/default size on a different monitor. Drag intent preserves the
// user's anchor; explicit centering can retain either floating or current size.
static bool RepairFloatingGeometry(
    const DesktopMonitorKey& key, Workspace& workspace, HWND hwnd,
    const FloatingPlacementHint& hint) {
  const WindowRecord* record = workspace.Find(hwnd);
  if (!record || !record->canMove || !IsWindow(hwnd) ||
      GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None) {
    return false;
  }

  HMONITOR monitor = key.ResolveMonitor();
  if (!monitor) return false;

  WorkspaceMetrics metrics;
  if (!GetWorkspaceMetrics(monitor, &metrics)) return false;
  const RECT& workArea = metrics.workArea;
  const UINT targetDpi = metrics.dpi;

  RECT current{};
  const bool haveCurrent = GetWindowFrameRect(hwnd, &current) &&
                           current.right > current.left &&
                           current.bottom > current.top;

  const bool centerWithoutResize =
      hint.intent == FloatingPlacementIntent::CenterPreservingSize;
  const bool useNewWindowSize =
      hint.intent == FloatingPlacementIntent::NewWindowCenter ||
      hint.intent == FloatingPlacementIntent::FailureCenter;
  const bool centerInWorkspace =
      hint.intent == FloatingPlacementIntent::CenterWithFloatingSize ||
      useNewWindowSize || centerWithoutResize;
  if (centerWithoutResize && !haveCurrent) return false;

  const bool needsFallbackSize =
      useNewWindowSize || centerWithoutResize || !record->hasFloatingRect;
  const WindowRule* placementOverride =
      IsAutomaticMode() && needsFallbackSize && !centerWithoutResize
          ? FindMatchingWindowRule(
                hwnd, WindowRuleTreatment::FloatingPlacementOverride)
          : nullptr;
  const bool preserveCurrentSize =
      hint.intent != FloatingPlacementIntent::FailureCenter &&
      placementOverride && placementOverride->preserveFloatingSize;
  const bool useCustomSize =
      placementOverride && !placementOverride->preserveFloatingSize;

  LONG width = 0;
  LONG height = 0;
  if (centerWithoutResize || (preserveCurrentSize && haveCurrent)) {
    width = current.right - current.left;
    height = current.bottom - current.top;
  } else if (useCustomSize) {
    if (placementOverride->useWorkspaceSize) {
      width = workArea.right - workArea.left;
      height = workArea.bottom - workArea.top;
    } else {
      width = ScaleDip(placementOverride->floatingSizeDip.width, targetDpi);
      height = ScaleDip(placementOverride->floatingSizeDip.height, targetDpi);
    }
  } else if (!useNewWindowSize && record->hasFloatingRect) {
    const UINT sourceDpi = record->floatingDpi ? record->floatingDpi : 96;
    width = MulDiv(
        record->floatingRect.right - record->floatingRect.left,
        static_cast<int>(targetDpi), static_cast<int>(sourceDpi));
    height = MulDiv(
        record->floatingRect.bottom - record->floatingRect.top,
        static_cast<int>(targetDpi), static_cast<int>(sourceDpi));
  } else {
    width = ScaleDip(g_settings.floatingDefaultSizeDip.width, targetDpi);
    height = ScaleDip(g_settings.floatingDefaultSizeDip.height, targetDpi);
  }

  if (!record->canResize && haveCurrent) {
    width = current.right - current.left;
    height = current.bottom - current.top;
  }

  POINT center{};
  if (hint.intent == FloatingPlacementIntent::PreserveAnchor) {
    center = hint.anchor;
  } else if (!centerInWorkspace && record->hasFloatingRect &&
             ResolveRememberedFloatingMonitor(*record) == monitor) {
    center = RectCenter(record->floatingRect);
  } else {
    center = RectCenter(workArea);
  }

  RECT target = CenteredRect(center, width, height);
  if (!centerWithoutResize) {
    target = ClampFloatingRectToWorkArea(target, workArea);
  }
  // The window can enter a non-participating physical state while the placement
  // target is being calculated. Never overwrite it.
  if (GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None) {
    return false;
  }
  PlacementObservation observation =
      PlaceWindowChecked(hwnd, record->canMove, target, centerWithoutResize);
  if (observation.result != PlacementResult::Success &&
      observation.result != PlacementResult::AdjustedByWindow) {
    return false;
  }

  const RECT remembered =
      observation.observed.right > observation.observed.left &&
              observation.observed.bottom > observation.observed.top
          ? observation.observed
          : target;
  workspace.RememberFloatingGeometry(hwnd, remembered, monitor, targetDpi);
  ++Diagnostics::g_runtime.counters.floatingGeometryRepairs;
  return true;
}

static bool RememberCurrentFloatingGeometry(
    const DesktopMonitorKey& key, Workspace& workspace, HWND hwnd) {
  const WindowRecord* record = workspace.Find(hwnd);
  if (!record) return false;
  if (workspace.Layout() != TileLayout::Floating &&
      record->state != ManageState::Floating) {
    return false;
  }

  HMONITOR monitor = key.ResolveMonitor();
  if (!monitor) return false;

  RECT frame{};
  if (!GetWindowFrameRect(hwnd, &frame) || frame.right <= frame.left ||
      frame.bottom <= frame.top) {
    return false;
  }
  return workspace.RememberFloatingGeometry(
      hwnd, frame, monitor, GetMonitorEffectiveDpi(monitor));
}

// Entering the workspace-level Floating layout is a state restoration, not a
// per-window drag/float command. Restore each active tiled member's complete
// remembered floating rectangle; windows without memory use the configured
// centered fallback. Per-window Floating records are already user-positioned and
// deliberately remain untouched.
static void RestoreWorkspaceFloatingGeometry(
    const DesktopMonitorKey& key, Workspace& workspace) {
  if (workspace.Layout() != TileLayout::Floating) return;

  const std::vector<HWND> active = workspace.TiledWindows();
  const FloatingPlacementHint passiveRestore{};
  for (HWND hwnd : active) {
    RepairFloatingGeometry(key, workspace, hwnd, passiveRestore);
  }
}

// Resolves a monitor on the current virtual desktop to its workspace key.
static bool GetCurrentWorkspaceKey(
    HMONITOR monitor, DesktopMonitorKey* outKey) {
  if (!outKey || !IsLiveMonitorHandle(monitor)) return false;
  GUID desktopId{};
  if (g_wm.reconciledDesktopState == ReconciledDesktopState::Settled) {
    desktopId = g_wm.reconciledDesktop;
  } else {
    // Never collapse separate desktops into a zero-GUID workspace when the
    // private API is temporarily unavailable; let the caller retry after recovery.
    if (!InitializeVirtualDesktopAPI() || !GetCurrentDesktopId(&desktopId)) {
      return false;
    }
  }
  return DesktopMonitorKey::FromHMonitor(desktopId, monitor, outKey);
}

bool Workspace::Validate(
    const DesktopMonitorKey& key, const wchar_t* context,
    std::vector<std::wstring>* errors, bool logFailures) const {
  bool valid = true;
  auto fail = [&](const wchar_t* message, HWND hwnd = nullptr) {
    if (errors) {
      if (hwnd) {
        wchar_t buffer[512]{};
        swprintf(
            buffer, ARRAYSIZE(buffer), L"%ls (hwnd=%p)", message,
            reinterpret_cast<void*>(hwnd));
        errors->push_back(buffer);
      } else {
        errors->push_back(message);
      }
    }
    if (logFailures) {
      Wh_Log(
          L"Workspace invariant violation (%s, desktop=%08X monitorId=%ls hwnd=%p): %s",
          context ? context : L"workspace", key.desktopId.Data1,
          key.monitor.deviceId.c_str(), hwnd, message);
    }
    valid = false;
  };

  std::unordered_set<HWND> active;
  for (HWND hwnd : windows_) {
    if (!hwnd) fail(L"null HWND in active tiled order");
    if (!active.insert(hwnd).second) fail(L"duplicate HWND in active tiled order", hwnd);

    auto recordIt = records_.find(hwnd);
    if (recordIt == records_.end()) {
      fail(L"active tiled HWND has no WindowRecord", hwnd);
    } else if (recordIt->second.state != ManageState::Tiled) {
      fail(L"active tiled HWND does not have Tiled state", hwnd);
    }
  }

  size_t suspendedCount = 0;
  std::unordered_set<size_t> suspendedLogicalSlots;
  for (const auto& kv : records_) {
    HWND hwnd = kv.first;
    const WindowRecord& record = kv.second;
    const bool isActive = active.find(hwnd) != active.end();

    if (record.hwnd && record.hwnd != hwnd) {
      fail(L"WindowRecord hwnd field disagrees with record-map key", hwnd);
    }
    if (record.hasFloatingRect &&
        (record.floatingRect.right <= record.floatingRect.left ||
         record.floatingRect.bottom <= record.floatingRect.top ||
         !record.floatingMonitor || !record.floatingDpi)) {
      fail(L"remembered floating geometry is invalid", hwnd);
    }
    if (record.alwaysFloating && record.state != ManageState::Floating) {
      fail(L"Always-floating record is not Floating", hwnd);
    }

    switch (record.state) {
      case ManageState::Tiled:
        if (!isActive) fail(L"Tiled record is absent from active tiled order", hwnd);
        if (record.hasSavedSlot) fail(L"Tiled record retains a suspended slot", hwnd);
        break;

      case ManageState::Suspended:
        ++suspendedCount;
        if (isActive) fail(L"Suspended record appears in active tiled order", hwnd);
        if (!record.hasSavedSlot) {
          fail(L"Suspended record has no saved logical slot", hwnd);
        } else if (!suspendedLogicalSlots.insert(record.savedSlot.index).second) {
          fail(L"two suspended records share the same logical slot", hwnd);
        }
        break;

      case ManageState::Floating:
      case ManageState::Ignored:
        if (isActive) fail(L"non-tiled record appears in active tiled order", hwnd);
        if (record.hasSavedSlot) fail(L"non-suspended record retains a saved slot", hwnd);
        break;
    }
  }

  const size_t logicalCount = windows_.size() + suspendedCount;
  for (const auto& kv : records_) {
    const WindowRecord& record = kv.second;
    if (record.state == ManageState::Suspended && record.hasSavedSlot &&
        record.savedSlot.index >= logicalCount) {
      fail(L"suspended logical slot is outside workspace logical order", kv.first);
    }
  }

  if (!std::isfinite(masterRatio_) || masterRatio_ < 0.1 || masterRatio_ > 0.9) {
    fail(L"master ratio is outside the supported range");
  }

  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    if (gridWeights_.size() != windows_.size()) {
      fail(L"grid weight count does not match active tiled count");
    }
    if (!stackWeights_.empty()) fail(L"grid layout retains stack weights");
    if (!dwindleRatios_.empty()) fail(L"grid layout retains Dwindle ratios");
  } else if (layout_ == TileLayout::MasterStack ||
             layout_ == TileLayout::MasterStackH) {
    const size_t expected = windows_.empty() ? 0 : windows_.size() - 1;
    if (stackWeights_.size() != expected) {
      fail(L"stack weight count does not match non-master tiled count");
    }
    if (!gridWeights_.empty()) fail(L"master/stack layout retains grid weights");
    if (!dwindleRatios_.empty()) fail(L"master/stack layout retains Dwindle ratios");
  } else if (layout_ == TileLayout::Dwindle) {
    const size_t expected = windows_.size();
    if (dwindleRatios_.size() != expected) {
      fail(L"Dwindle outgoing ratio count does not match active slot count");
    }
    for (double ratio : dwindleRatios_) {
      if (!std::isfinite(ratio) || ratio < 0.1 || ratio > 0.9) {
        fail(L"Dwindle ratio is outside the supported range");
        break;
      }
    }
    if (!stackWeights_.empty() || !gridWeights_.empty()) {
      fail(L"Dwindle layout retains unrelated layout weights");
    }
  } else if (!stackWeights_.empty() || !gridWeights_.empty() ||
             !dwindleRatios_.empty()) {
    fail(L"weightless layout retains layout weights");
  }

  return valid;
}

void Workspace::DebugValidateMutation(const wchar_t* operation) const {
#ifdef MULTIWM_DEBUG_VALIDATE
  DesktopMonitorKey unknownKey{};
  const bool valid = Validate(unknownKey, operation ? operation : L"mutation");
  assert(valid);
#else
  (void)operation;
#endif
}

bool WorkspaceRepository::Load(
    const DesktopMonitorKey& key, Workspace* workspace) const {
  if (!workspace) return false;
  bool found = false;
  AcquireSRWLockShared(&lock_);
  auto it = states_.find(key);
  if (it != states_.end()) {
    *workspace = it->second;
    found = true;
  }
  ReleaseSRWLockShared(&lock_);
  return found;
}

void WorkspaceRepository::Save(
    const DesktopMonitorKey& key, const Workspace& workspace) {
  AssertWmThread(L"WorkspaceRepository::Save");
  ++Diagnostics::g_runtime.counters.workspaceSaves;
  workspace.Validate(key, L"repository save");

  AcquireSRWLockExclusive(&lock_);

  // Remove this workspace key from the reverse index for the previous snapshot.
  // Re-indexing the whole small workspace keeps the code simple and guarantees the
  // index follows every semantic mutation made through Workspace.
  auto oldStateIt = states_.find(key);
  if (oldStateIt != states_.end()) {
    for (const auto& record : oldStateIt->second.Records()) {
      auto ownersIt = ownersByWindow_.find(record.first);
      if (ownersIt == ownersByWindow_.end()) continue;

      auto& owners = ownersIt->second;
      owners.erase(
          std::remove_if(
              owners.begin(), owners.end(),
              [&](const DesktopMonitorKey& ownerKey) {
                return DesktopMonitorKeyEqual{}(ownerKey, key);
              }),
          owners.end());
      if (owners.empty()) ownersByWindow_.erase(ownersIt);
    }
  }

  states_[key] = workspace;

  // Add the replacement snapshot to the reverse index. Preserve multiple owner
  // keys if they exist so EnsureUniqueWindowOwnership can still repair old bugs.
  for (const auto& record : workspace.Records()) {
    auto& owners = ownersByWindow_[record.first];
    if (std::none_of(
            owners.begin(), owners.end(),
            [&](const DesktopMonitorKey& ownerKey) {
              return DesktopMonitorKeyEqual{}(ownerKey, key);
            })) {
      owners.push_back(key);
    }
  }

  ReleaseSRWLockExclusive(&lock_);
}

bool WorkspaceRepository::IsTracked(HWND hwnd) const {
  if (!hwnd) return false;
  AcquireSRWLockShared(&lock_);
  auto it = ownersByWindow_.find(hwnd);
  const bool tracked = it != ownersByWindow_.end() && !it->second.empty();
  ReleaseSRWLockShared(&lock_);
  return tracked;
}

bool WorkspaceRepository::IsTiled(HWND hwnd) const {
  if (!hwnd) return false;
  bool found = false;
  AcquireSRWLockShared(&lock_);

  auto ownersIt = ownersByWindow_.find(hwnd);
  if (ownersIt != ownersByWindow_.end()) {
    for (const auto& key : ownersIt->second) {
      auto stateIt = states_.find(key);
      if (stateIt != states_.end() && stateIt->second.IsTiled(hwnd)) {
        found = true;
        break;
      }
    }
  }

  ReleaseSRWLockShared(&lock_);
  return found;
}

bool WorkspaceRepository::IsSuspendedMaximized(HWND hwnd) const {
  if (!hwnd) return false;
  bool found = false;
  AcquireSRWLockShared(&lock_);

  auto ownersIt = ownersByWindow_.find(hwnd);
  if (ownersIt != ownersByWindow_.end()) {
    for (const auto& key : ownersIt->second) {
      auto stateIt = states_.find(key);
      if (stateIt == states_.end()) continue;
      const WindowRecord* record = stateIt->second.Find(hwnd);
      if (record && record->state == ManageState::Suspended &&
          record->suspensionReason == SuspensionReason::Maximized) {
        found = true;
        break;
      }
    }
  }

  ReleaseSRWLockShared(&lock_);
  return found;
}

std::vector<DesktopMonitorKey> WorkspaceRepository::OwnersOf(HWND hwnd) const {
  std::vector<DesktopMonitorKey> keys;
  if (!hwnd) return keys;
  AcquireSRWLockShared(&lock_);
  auto it = ownersByWindow_.find(hwnd);
  if (it != ownersByWindow_.end()) keys = it->second;
  ReleaseSRWLockShared(&lock_);
  return keys;
}

std::vector<HWND> WorkspaceRepository::KnownWindows() const {
  std::vector<HWND> known;
  AcquireSRWLockShared(&lock_);
  known.reserve(ownersByWindow_.size());
  for (const auto& entry : ownersByWindow_) {
    if (!entry.second.empty()) known.push_back(entry.first);
  }
  ReleaseSRWLockShared(&lock_);
  return known;
}

std::vector<HMONITOR> WorkspaceRepository::MonitorsOnDesktop(
    const GUID& desktopId) const {
  std::vector<Model::MonitorId> monitorIds;
  AcquireSRWLockShared(&lock_);
  for (const auto& kv : states_) {
    if (!IsEqualGUID(kv.first.desktopId, desktopId)) continue;
    if (std::find(monitorIds.begin(), monitorIds.end(), kv.first.monitor) ==
        monitorIds.end()) {
      monitorIds.push_back(kv.first.monitor);
    }
  }
  ReleaseSRWLockShared(&lock_);

  // Repository identity survives topology churn. Only return identities that
  // currently resolve to a live HMONITOR; detached workspaces remain dormant.
  std::vector<HMONITOR> monitors;
  for (const auto& monitorId : monitorIds) {
    HMONITOR monitor = monitorId.Resolve();
    if (monitor && std::find(monitors.begin(), monitors.end(), monitor) == monitors.end()) {
      monitors.push_back(monitor);
    }
  }
  return monitors;
}

std::vector<std::pair<DesktopMonitorKey, Workspace>>
WorkspaceRepository::Snapshot() const {
  std::vector<std::pair<DesktopMonitorKey, Workspace>> snapshot;
  AcquireSRWLockShared(&lock_);
  snapshot.reserve(states_.size());
  for (const auto& kv : states_) snapshot.push_back(kv);
  ReleaseSRWLockShared(&lock_);
  return snapshot;
}

std::vector<std::pair<HWND, std::vector<DesktopMonitorKey>>>
WorkspaceRepository::OwnershipIndexSnapshot() const {
  std::vector<std::pair<HWND, std::vector<DesktopMonitorKey>>> snapshot;
  AcquireSRWLockShared(&lock_);
  snapshot.reserve(ownersByWindow_.size());
  for (const auto& kv : ownersByWindow_) snapshot.push_back(kv);
  ReleaseSRWLockShared(&lock_);
  return snapshot;
}

// UI is intentionally implemented later so the complete workspace model and
// reconciliation pipeline can be read contiguously. Only this narrow surface is
// visible to the WM core.
namespace TrayUi {
static void LoadSettings();
static bool Initialize();
static void Shutdown();
static void UpdateIcon(TileLayout layout, HMONITOR monitor = nullptr);
static void ShowLayoutFlyout(TileLayout layout, HMONITOR monitor = nullptr);
static void ShowCommandFailureFlyout(
    const wchar_t* text, HMONITOR monitor = nullptr);
static void ShowManagementModeFlyout(bool automatic);
static void ShowDesktopSwitchFlyouts(const GUID& desktopId);
static void RefreshForMonitor(HMONITOR monitor);
static void RefreshDisplayedMonitor();
static void RefreshForCurrentWorkspace();
}  // namespace TrayUi



//=============================================================================
// Workspace model operations
//=============================================================================
//
// The Workspace class is the only owner of logical participation/order/weight
// invariants. Callers may inspect const views, but cannot edit the backing
// containers directly.

void Workspace::EnsureWeights() {
  if (layout_ == TileLayout::MasterStack || layout_ == TileLayout::MasterStackH) {
    const size_t stackCount = windows_.empty() ? 0 : windows_.size() - 1;
    if (stackWeights_.size() != stackCount) stackWeights_ = DefaultWeights(stackCount);
    gridWeights_.clear();
    dwindleRatios_.clear();
  } else if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    if (gridWeights_.size() != windows_.size()) gridWeights_ = DefaultWeights(windows_.size());
    stackWeights_.clear();
    dwindleRatios_.clear();
  } else if (layout_ == TileLayout::Dwindle) {
    // Keep one outgoing ratio per active slot, including a dormant ratio on the
    // final slot. The dormant value is what lets the predecessor's divider survive
    // when its successor is suspended, then become active again on restoration.
    dwindleRatios_.resize(windows_.size(), 0.5);
    stackWeights_.clear();
    gridWeights_.clear();
  } else {
    stackWeights_.clear();
    gridWeights_.clear();
    dwindleRatios_.clear();
  }
}

void Workspace::RemoveActiveAt(size_t index, SuspendedSlot* saved) {
  AssertWmThread(L"Workspace::RemoveActiveAt");
  if (index >= windows_.size()) return;

  SuspendedSlot slot{};
  slot.index = index;
  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    if (index < gridWeights_.size()) {
      slot.weight = gridWeights_[index];
      slot.hasWeight = true;
      gridWeights_.erase(gridWeights_.begin() + index);
    }
  } else if (layout_ == TileLayout::MasterStack || layout_ == TileLayout::MasterStackH) {
    if (index == 0) {
      slot.wasMaster = true;
      if (!stackWeights_.empty()) {
        slot.weight = stackWeights_.front();
        slot.hasWeight = true;
        stackWeights_.erase(stackWeights_.begin());
      }
    } else if (index - 1 < stackWeights_.size()) {
      slot.weight = stackWeights_[index - 1];
      slot.hasWeight = true;
      stackWeights_.erase(stackWeights_.begin() + (index - 1));
    }
  } else if (layout_ == TileLayout::Dwindle && !dwindleRatios_.empty()) {
    // Save the suspended logical slot's outgoing ratio. Because EnsureWeights
    // retains the final dormant ratio, removing the last active member does not
    // accidentally consume the divider owned by its predecessor.
    const size_t ratioIndex = index;
    slot.weight = dwindleRatios_[ratioIndex];
    slot.hasWeight = true;
    dwindleRatios_.erase(dwindleRatios_.begin() + ratioIndex);
  }

  windows_.erase(windows_.begin() + index);
  if (saved) *saved = slot;
  EnsureWeights();
}

size_t Workspace::ActiveIndexToLogicalIndex(size_t activeIndex) const {
  std::vector<size_t> suspendedIndices;
  for (const auto& kv : records_) {
    const WindowRecord& record = kv.second;
    if (record.state == ManageState::Suspended && record.hasSavedSlot) {
      suspendedIndices.push_back(record.savedSlot.index);
    }
  }
  std::sort(suspendedIndices.begin(), suspendedIndices.end());

  size_t logicalIndex = activeIndex;
  for (size_t suspendedIndex : suspendedIndices) {
    if (suspendedIndex <= logicalIndex) ++logicalIndex;
    else break;
  }
  return logicalIndex;
}

size_t Workspace::LogicalIndexToActiveIndex(
    size_t logicalIndex, HWND restoringHwnd) const {
  size_t suspendedBefore = 0;
  for (const auto& kv : records_) {
    const WindowRecord& record = kv.second;
    if (kv.first != restoringHwnd &&
        record.state == ManageState::Suspended &&
        record.hasSavedSlot &&
        record.savedSlot.index < logicalIndex) {
      ++suspendedBefore;
    }
  }
  return logicalIndex >= suspendedBefore ? logicalIndex - suspendedBefore : 0;
}

void Workspace::CloseLogicalGapAfterRemoval(
    size_t removedLogicalIndex, HWND exceptHwnd) {
  AssertWmThread(L"Workspace::CloseLogicalGapAfterRemoval");
  for (auto& kv : records_) {
    WindowRecord& record = kv.second;
    if (kv.first != exceptHwnd &&
        record.state == ManageState::Suspended &&
        record.hasSavedSlot &&
        record.savedSlot.index > removedLogicalIndex) {
      --record.savedSlot.index;
    }
  }
}

bool Workspace::Suspend(HWND hwnd, SuspensionReason reason) {
  AssertWmThread(L"Workspace::Suspend");
  WindowRecord* existing = FindMutable(hwnd);
  if (existing && existing->state == ManageState::Suspended) {
    if (existing->suspensionReason == reason) return false;
    if (reason == SuspensionReason::Maximized &&
        (existing->suspensionReason == SuspensionReason::Minimized ||
         existing->suspensionReason == SuspensionReason::Hidden)) {
      existing->tilingGeneration = Model::NextTilingGeneration();
    }
    existing->suspensionReason = reason;
    DebugValidateMutation(L"Workspace::Suspend(reason update)");
    return true;
  }

  auto it = std::find(windows_.begin(), windows_.end(), hwnd);
  if (it == windows_.end()) return false;

  WindowRecord* record = FindMutable(hwnd);
  if (!record) return false;
  SuspendedSlot slot{};
  const size_t activeIndex = static_cast<size_t>(it - windows_.begin());
  const size_t logicalIndex = ActiveIndexToLogicalIndex(activeIndex);
  RemoveActiveAt(activeIndex, &slot);
  slot.index = logicalIndex;

  record->state = ManageState::Suspended;
  record->suspensionReason = reason;
  record->savedSlot = slot;
  record->hasSavedSlot = true;
  DebugValidateMutation(L"Workspace::Suspend");
  return true;
}

static SuspensionReason GetPhysicalSuspensionReason(HWND hwnd) {
  if (!hwnd || !IsWindow(hwnd)) return SuspensionReason::None;
  if (IsIconic(hwnd)) return SuspensionReason::Minimized;
  DWORD cloaked = 0;
  DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
  if (!IsWindowVisible(hwnd) || (cloaked & DWM_CLOAKED_APP)) return SuspensionReason::Hidden;
  if (IsZoomed(hwnd)) return SuspensionReason::Maximized;

  // Chromium and similar apps can cover the full monitor while Windows still
  // reports the HWND as restored. Fullscreen and native maximize have identical
  // suspend/save-slot/restore semantics here.
  RECT windowRect{};
  if (GetWindowRect(hwnd, &windowRect)) {
    HMONITOR monitor = MonitorFromRect(&windowRect, MONITOR_DEFAULTTONULL);
    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    constexpr LONG kFullscreenTolerancePx = 2;
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo) &&
        !Differs(windowRect.left, monitorInfo.rcMonitor.left,
                 kFullscreenTolerancePx) &&
        !Differs(windowRect.top, monitorInfo.rcMonitor.top,
                 kFullscreenTolerancePx) &&
        !Differs(windowRect.right, monitorInfo.rcMonitor.right,
                 kFullscreenTolerancePx) &&
        !Differs(windowRect.bottom, monitorInfo.rcMonitor.bottom,
                 kFullscreenTolerancePx)) {
      return SuspensionReason::Maximized;
    }
  }
  return SuspensionReason::None;
}

// Workspace initialization needs a broader physical snapshot than normal layout
// participation. A visible top-level window can already belong to this workspace
// while minimized, maximized, or fullscreen, even though active collection
// correctly excludes it from placement.
static std::vector<HWND> CollectWorkspaceWindowsForInitialization(
    HMONITOR monitor, const GUID& desktopId) {
  std::vector<HWND> windows;
  if (!monitor) return windows;

  struct EnumContext {
    HMONITOR targetMonitor;
    GUID desktopId;
    std::vector<HWND>* windowList;
  } context = {monitor, desktopId, &windows};

  ++Diagnostics::g_runtime.counters.enumWindowsPasses;
  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        ++Diagnostics::g_runtime.counters.enumWindowsVisited;
        auto* ctx = reinterpret_cast<EnumContext*>(lParam);
        if (!IsWindowVisible(hwnd)) return TRUE;
        if (!WindowCanBeManaged(hwnd)) return TRUE;
        if (!IsWindowTrackedInAnyState(hwnd) && !IsWindowEnabled(hwnd)) return TRUE;

        GUID windowDesktop{};
        if (!GetWindowDesktopIdSafe(hwnd, &windowDesktop) ||
            !IsEqualGUID(windowDesktop, ctx->desktopId)) {
          return TRUE;
        }

        if (GetWindowPhysicalMonitor(hwnd) != ctx->targetMonitor) {
          return TRUE;
        }

        ctx->windowList->push_back(hwnd);
        ++Diagnostics::g_runtime.counters.tileCandidatesCollected;
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return windows;
}

static bool AdmitInitialObservedWindow(
    Workspace& workspace, HWND hwnd, bool useAutomaticAdmissionPolicy) {
  if (!hwnd || !IsWindow(hwnd) || workspace.HasRecord(hwnd) ||
      IsWindowTrackedInAnyState(hwnd) || IsWindowCloaked(hwnd)) {
    return false;
  }
  if (useAutomaticAdmissionPolicy && IsWindowAwaitingInitialPlacement(hwnd)) {
    return false;
  }
  const SuspensionReason reason = GetPhysicalSuspensionReason(hwnd);
  if (reason == SuspensionReason::Hidden) return false;
  bool alwaysFloating = false;
  const bool startsFloating =
      ShouldWindowStartFloating(hwnd, &alwaysFloating);
  if (alwaysFloating || (useAutomaticAdmissionPolicy && startsFloating)) {
    WindowRecord record = MakeWindowRecord(hwnd, ManageState::Floating);
    record.pendingDefaultFloatingCenter = true;
    record.alwaysFloating = alwaysFloating;
    return workspace.AdmitFloating(std::move(record));
  }
  WindowRecord record = MakeWindowRecord(hwnd);
  record.automaticOverflowMonitorFixed =
      HasMonitorPlacementRule(hwnd);
  return workspace.AdmitInitial(std::move(record), reason);
}

size_t Workspace::LogicalWindowCount() const {
  size_t count = windows_.size();
  for (const auto& kv : records_) {
    if (kv.second.state == ManageState::Suspended && kv.second.hasSavedSlot) ++count;
  }
  return count;
}

size_t Workspace::VisibleWindowCount() const {
  if (layout_ == TileLayout::Floating) return 0;
  size_t count = windows_.size();
  for (const auto& member : records_) {
    const WindowRecord& record = member.second;
    if (record.state == ManageState::Suspended && record.hasSavedSlot &&
        record.suspensionReason == SuspensionReason::Maximized) ++count;
  }
  return count;
}

Workspace Workspace::EmptyWithInheritedLayoutSettings() const {
  Workspace empty;
  empty.layout_ = layout_;
  empty.masterRatio_ = masterRatio_;
  if (layout_ == TileLayout::MasterStack || layout_ == TileLayout::MasterStackH) {
    empty.inheritedSlotWeights_ = stackWeights_;
  } else if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    empty.inheritedSlotWeights_ = gridWeights_;
  } else if (layout_ == TileLayout::Dwindle) {
    empty.inheritedSlotWeights_ = dwindleRatios_;
  }
  if (empty.inheritedSlotWeights_.size() < inheritedSlotWeights_.size()) {
    empty.inheritedSlotWeights_.insert(empty.inheritedSlotWeights_.end(),
        inheritedSlotWeights_.begin() + empty.inheritedSlotWeights_.size(), inheritedSlotWeights_.end());
  }
  return empty;
}

bool Workspace::AdmitInitial(
    WindowRecord record, SuspensionReason reason) {
  AssertWmThread(L"Workspace::AdmitInitial");
  const HWND hwnd = record.hwnd;
  if (!hwnd || HasRecord(hwnd) || reason == SuspensionReason::Hidden) return false;
  if (!record.tilingGeneration) {
    record.tilingGeneration = Model::NextTilingGeneration();
  }

  record.state = reason == SuspensionReason::None
                     ? ManageState::Tiled
                     : ManageState::Suspended;
  record.pendingInsertionPlacement = true;
  if (reason == SuspensionReason::None) {
    AppendTiledWithDefaultWeight(hwnd);
  } else {
    record.suspensionReason = reason;
    record.savedSlot = MakeAppendedSuspendedSlot();
    record.hasSavedSlot = true;
  }

  records_.emplace(hwnd, std::move(record));
  EnsureWeights();
  DebugValidateMutation(L"Workspace::AdmitInitial");
  return true;
}

bool Workspace::AdmitInitialAfter(
    WindowRecord record, SuspensionReason reason, HWND anchor) {
  AssertWmThread(L"Workspace::AdmitInitialAfter");
  if (reason == SuspensionReason::None) {
    return AdmitTiledAfter(std::move(record), anchor);
  }

  const HWND hwnd = record.hwnd;
  if (!hwnd || HasRecord(hwnd) || reason == SuspensionReason::Hidden ||
      !IsTiled(anchor)) {
    return AdmitInitial(std::move(record), reason);
  }

  const size_t anchorActiveIndex = TiledIndex(anchor);
  if (anchorActiveIndex == static_cast<size_t>(-1)) {
    return AdmitInitial(std::move(record), reason);
  }

  const size_t insertionLogicalIndex =
      ActiveIndexToLogicalIndex(anchorActiveIndex) + 1;
  for (auto& kv : records_) {
    WindowRecord& existing = kv.second;
    if (existing.state == ManageState::Suspended &&
        existing.hasSavedSlot &&
        existing.savedSlot.index >= insertionLogicalIndex) {
      ++existing.savedSlot.index;
    }
  }

  if (!record.tilingGeneration) {
    record.tilingGeneration = Model::NextTilingGeneration();
  }
  record.state = ManageState::Suspended;
  record.pendingInsertionPlacement = true;
  record.suspensionReason = reason;
  record.savedSlot.index = insertionLogicalIndex;
  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    record.savedSlot.weight = 1.0;
    record.savedSlot.hasWeight = true;
  } else if (layout_ == TileLayout::MasterStack ||
             layout_ == TileLayout::MasterStackH) {
    record.savedSlot.weight = 1.0;
    record.savedSlot.hasWeight = true;
  } else if (layout_ == TileLayout::Dwindle) {
    record.savedSlot.weight = 0.5;
    record.savedSlot.hasWeight = true;
  }
  record.hasSavedSlot = true;

  records_.emplace(hwnd, std::move(record));
  EnsureWeights();
  DebugValidateMutation(L"Workspace::AdmitInitialAfter");
  return true;
}

bool Workspace::Restore(HWND hwnd) {
  AssertWmThread(L"Workspace::Restore");
  WindowRecord* record = FindMutable(hwnd);
  if (!record || record->state != ManageState::Suspended || !record->hasSavedSlot) {
    return false;
  }

  const SuspendedSlot slot = record->savedSlot;
  const bool returnedToVisibleSet = record->suspensionReason == SuspensionReason::Minimized ||
                                   record->suspensionReason == SuspensionReason::Hidden;
  if (returnedToVisibleSet || !record->tilingGeneration) {
    record->tilingGeneration = Model::NextTilingGeneration();
  }
  const size_t index = std::min(
      LogicalIndexToActiveIndex(slot.index, hwnd), windows_.size());
  windows_.insert(windows_.begin() + index, hwnd);

  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    if (gridWeights_.size() + 1 == windows_.size()) {
      gridWeights_.insert(
          gridWeights_.begin() + std::min(index, gridWeights_.size()),
          slot.hasWeight ? slot.weight : 1.0);
    }
  } else if (layout_ == TileLayout::MasterStack || layout_ == TileLayout::MasterStackH) {
    if (slot.wasMaster) {
      if (stackWeights_.size() + 1 == windows_.size() - 1) {
        stackWeights_.insert(
            stackWeights_.begin(), slot.hasWeight ? slot.weight : 1.0);
      }
    } else if (index > 0 && stackWeights_.size() + 1 == windows_.size() - 1) {
      const size_t stackIndex = std::min(index - 1, stackWeights_.size());
      stackWeights_.insert(
          stackWeights_.begin() + stackIndex,
          slot.hasWeight ? slot.weight : 1.0);
    }
  } else if (layout_ == TileLayout::Dwindle) {
    // Reinsert the logical slot's outgoing ratio before EnsureWeights. The vector
    // deliberately includes the final dormant ratio, so a one-window intermediate
    // restore cannot discard split state needed by a later restored successor.
    const size_t ratioIndex = std::min(index, dwindleRatios_.size());
    dwindleRatios_.insert(
        dwindleRatios_.begin() + ratioIndex,
        slot.hasWeight ? ClampDouble(slot.weight, 0.1, 0.9) : 0.5);
  }

  record->state = ManageState::Tiled;
  // Preserve the saved slot as the first restore attempt, but keep the restored
  // member movable until that placement has survived conformance. If its old
  // slot became infeasible while suspended, ArrangeWorkspace can now apply the
  // same nearest-slot / Dwindle recovery used for a fresh admission instead of
  // immediately floating it.
  record->pendingInsertionPlacement = layout_ != TileLayout::Floating;
  record->suspensionReason = SuspensionReason::None;
  record->savedSlot = {};
  record->hasSavedSlot = false;
  EnsureWeights();
  DebugValidateMutation(L"Workspace::Restore");
  return true;
}

bool Workspace::Float(HWND hwnd) {
  AssertWmThread(L"Workspace::Float");
  WindowRecord* existing = FindMutable(hwnd);
  if (existing && existing->state == ManageState::Floating) return false;

  auto activeIt = std::find(windows_.begin(), windows_.end(), hwnd);
  if (activeIt != windows_.end()) {
    const size_t activeIndex = static_cast<size_t>(activeIt - windows_.begin());
    const size_t logicalIndex = ActiveIndexToLogicalIndex(activeIndex);
    RemoveActiveAt(activeIndex);
    CloseLogicalGapAfterRemoval(logicalIndex);
  } else if (existing && existing->state == ManageState::Suspended && existing->hasSavedSlot) {
    CloseLogicalGapAfterRemoval(existing->savedSlot.index, hwnd);
  }

  WindowRecord* record = FindMutable(hwnd);
  if (!record) return false;
  record->state = ManageState::Floating;
  record->pendingInsertionPlacement = false;
  record->pendingDefaultFloatingCenter = false;
  record->suspensionReason = SuspensionReason::None;
  record->savedSlot = {};
  record->hasSavedSlot = false;
  EnsureWeights();
  DebugValidateMutation(L"Workspace::Float");
  return true;
}

bool Workspace::Forget(HWND hwnd) {
  AssertWmThread(L"Workspace::Forget");
  if (lastFocusedWindow_ == hwnd) lastFocusedWindow_ = nullptr;
  auto activeIt = std::find(windows_.begin(), windows_.end(), hwnd);
  if (activeIt != windows_.end()) {
    const size_t activeIndex = static_cast<size_t>(activeIt - windows_.begin());
    const size_t logicalIndex = ActiveIndexToLogicalIndex(activeIndex);
    RemoveActiveAt(activeIndex);
    CloseLogicalGapAfterRemoval(logicalIndex);
    records_.erase(hwnd);
    DebugValidateMutation(L"Workspace::Forget(active)");
    return true;
  }

  auto recordIt = records_.find(hwnd);
  if (recordIt == records_.end()) return false;
  if (recordIt->second.state == ManageState::Suspended && recordIt->second.hasSavedSlot) {
    const size_t logicalIndex = recordIt->second.savedSlot.index;
    records_.erase(recordIt);
    CloseLogicalGapAfterRemoval(logicalIndex);
  } else {
    records_.erase(recordIt);
  }
  EnsureWeights();
  DebugValidateMutation(L"Workspace::Forget");
  return true;
}

void Workspace::AppendTiledWithDefaultWeight(HWND hwnd) {
  AssertWmThread(L"Workspace::AppendTiledWithDefaultWeight");
  EnsureWeights();
  const size_t oldCount = windows_.size();
  windows_.push_back(hwnd);
  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    gridWeights_.push_back(oldCount < inheritedSlotWeights_.size() ? inheritedSlotWeights_[oldCount] : 1.0);
  } else if ((layout_ == TileLayout::MasterStack || layout_ == TileLayout::MasterStackH) &&
             oldCount > 0) {
    stackWeights_.push_back(oldCount - 1 < inheritedSlotWeights_.size() ? inheritedSlotWeights_[oldCount - 1] : 1.0);
  } else if (layout_ == TileLayout::Dwindle) {
    dwindleRatios_.push_back(oldCount < inheritedSlotWeights_.size() ? inheritedSlotWeights_[oldCount] : 0.5);
  }
}

SuspendedSlot Workspace::MakeAppendedSuspendedSlot() const {
  SuspendedSlot slot{};
  slot.index = LogicalWindowCount();
  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    slot.weight = slot.index < inheritedSlotWeights_.size() ? inheritedSlotWeights_[slot.index] : 1.0;
    slot.hasWeight = true;
  } else if (layout_ == TileLayout::MasterStack || layout_ == TileLayout::MasterStackH) {
    if (slot.index == 0) slot.wasMaster = true;
    else {
      slot.weight = slot.index - 1 < inheritedSlotWeights_.size() ? inheritedSlotWeights_[slot.index - 1] : 1.0;
      slot.hasWeight = true;
    }
  } else if (layout_ == TileLayout::Dwindle) {
    slot.weight = slot.index < inheritedSlotWeights_.size() ? inheritedSlotWeights_[slot.index] : 0.5;
    slot.hasWeight = true;
  }
  return slot;
}

bool Workspace::ExtractForMigration(HWND hwnd, WindowRecord* outRecord) {
  AssertWmThread(L"Workspace::ExtractForMigration");
  const WindowRecord* record = Find(hwnd);
  if (!record) return false;
  if (outRecord) *outRecord = *record;
  return Forget(hwnd);
}

void Workspace::AdmitMigrated(WindowRecord record, HWND anchor) {
  AssertWmThread(L"Workspace::AdmitMigrated");
  const HWND hwnd = record.hwnd;
  if (!hwnd) return;
  if (record.state == ManageState::Tiled ||
      record.state == ManageState::Suspended) {
    // A destination workspace admission is newer than its existing members and
    // receives the first opportunity to find a viable destination slot.
    record.tilingGeneration = Model::NextTilingGeneration();
    record.pendingInsertionPlacement = true;
  }
  Forget(hwnd);

  // Automatic cross-workspace insertion follows the same destination-anchor
  // policy as fresh admission. The caller supplies no anchor for Last slot or
  // for Manual-mode migration, preserving the previous append behavior.
  if (anchor && anchor != hwnd && IsTiled(anchor)) {
    if (record.state == ManageState::Tiled) {
      AdmitTiledAfter(std::move(record), anchor);
      return;
    }

    if (record.state == ManageState::Suspended &&
        record.suspensionReason != SuspensionReason::None &&
        record.suspensionReason != SuspensionReason::Hidden) {
      const SuspensionReason reason = record.suspensionReason;
      AdmitInitialAfter(std::move(record), reason, anchor);
      return;
    }
  }

  if (record.state == ManageState::Tiled) {
    record.suspensionReason = SuspensionReason::None;
    record.savedSlot = {};
    record.hasSavedSlot = false;
    AppendTiledWithDefaultWeight(hwnd);
  } else if (record.state == ManageState::Suspended) {
    record.savedSlot = MakeAppendedSuspendedSlot();
    record.hasSavedSlot = true;
  }
  records_[hwnd] = std::move(record);
  EnsureWeights();
  DebugValidateMutation(L"Workspace::AdmitMigrated");
}

void Workspace::MergeRememberedFloatingGeometryFrom(const Workspace& source) {
  AssertWmThread(L"Workspace::MergeRememberedFloatingGeometryFrom");
  for (const auto& kv : source.records_) {
    const WindowRecord& sourceRecord = kv.second;
    if (!sourceRecord.hasFloatingRect) continue;
    WindowRecord* targetRecord = FindMutable(kv.first);
    if (!targetRecord) continue;
    targetRecord->floatingRect = sourceRecord.floatingRect;
    targetRecord->floatingMonitor = sourceRecord.floatingMonitor;
    targetRecord->floatingDpi = sourceRecord.floatingDpi;
    targetRecord->hasFloatingRect = true;
  }
  DebugValidateMutation(L"Workspace::MergeRememberedFloatingGeometryFrom");
}

void Workspace::MergeNonTiledRecordsFrom(const Workspace& source) {
  AssertWmThread(L"Workspace::MergeNonTiledRecordsFrom");
  for (const auto& kv : source.records_) {
    if (kv.second.state != ManageState::Tiled) records_[kv.first] = kv.second;
  }
  DebugValidateMutation(L"Workspace::MergeNonTiledRecordsFrom");
}

void Workspace::RememberCurrentLayoutGeometry() {
  AssertWmThread(L"Workspace::RememberCurrentLayoutGeometry");

  if (layout_ == TileLayout::MasterStack ||
      layout_ == TileLayout::MasterStackH) {
    rememberedMasterStackOrder_ = windows_;
    rememberedMasterStackWeights_ = stackWeights_;
  } else if (layout_ == TileLayout::Columns ||
             layout_ == TileLayout::Rows) {
    rememberedGridOrder_ = windows_;
    rememberedGridWeights_ = gridWeights_;
  } else if (layout_ == TileLayout::Dwindle) {
    rememberedDwindleOrder_ = windows_;
    rememberedDwindleRatios_ = dwindleRatios_;
  }
}

void Workspace::RestoreRememberedLayoutGeometry() {
  AssertWmThread(L"Workspace::RestoreRememberedLayoutGeometry");

  if (layout_ == TileLayout::MasterStack ||
      layout_ == TileLayout::MasterStackH) {
    const size_t expected = windows_.empty() ? 0 : windows_.size() - 1;
    if (rememberedMasterStackOrder_ == windows_ &&
        rememberedMasterStackWeights_.size() == expected) {
      stackWeights_ = rememberedMasterStackWeights_;
    } else {
      stackWeights_.clear();
    }
  } else if (layout_ == TileLayout::Columns ||
             layout_ == TileLayout::Rows) {
    if (rememberedGridOrder_ == windows_ &&
        rememberedGridWeights_.size() == windows_.size()) {
      gridWeights_ = rememberedGridWeights_;
    } else {
      gridWeights_.clear();
    }
  } else if (layout_ == TileLayout::Dwindle) {
    if (rememberedDwindleOrder_ == windows_ &&
        rememberedDwindleRatios_.size() == windows_.size()) {
      dwindleRatios_ = rememberedDwindleRatios_;
    } else {
      dwindleRatios_.clear();
    }
  }
}

void Workspace::ResetSuspendedLayoutHints() {
  AssertWmThread(L"Workspace::ResetSuspendedLayoutHints");
  for (auto& kv : records_) {
    WindowRecord& record = kv.second;
    if (record.state == ManageState::Suspended && record.hasSavedSlot) {
      record.savedSlot.weight = 1.0;
      record.savedSlot.hasWeight = false;
      record.savedSlot.wasMaster = false;
    }
  }
}

void Workspace::SetLayout(TileLayout layout) {
  AssertWmThread(L"Workspace::SetLayout");
  if (layout_ == layout) return;

  RememberCurrentLayoutGeometry();
  layout_ = layout;
  inheritedSlotWeights_.clear();
  ResetSuspendedLayoutHints();
  RestoreRememberedLayoutGeometry();
  EnsureWeights();
  DebugValidateMutation(L"Workspace::SetLayout");
}

bool Workspace::CycleLayout(const std::vector<TileLayout>& cycle) {
  AssertWmThread(L"Workspace::CycleLayout");
  if (cycle.empty()) return false;

  auto it = std::find(cycle.begin(), cycle.end(), layout_);
  TileLayout nextLayout = cycle.front();
  if (it != cycle.end()) {
    ++it;
    if (it != cycle.end()) nextLayout = *it;
  }
  if (nextLayout == layout_) return false;

  SetLayout(nextLayout);
  return true;
}

void Workspace::SetMasterRatio(double ratio) {
  AssertWmThread(L"Workspace::SetMasterRatio");
  masterRatio_ = ClampDouble(ratio, 0.1, 0.9);
  DebugValidateMutation(L"Workspace::SetMasterRatio");
}

bool Workspace::SetDwindleRatio(size_t splitIndex, double ratio) {
  AssertWmThread(L"Workspace::SetDwindleRatio");
  EnsureWeights();
  if (layout_ != TileLayout::Dwindle ||
      dwindleRatios_.size() < 2 || splitIndex >= dwindleRatios_.size() - 1) {
    return false;
  }

  const double clamped = ClampDouble(ratio, 0.1, 0.9);
  if (std::abs(dwindleRatios_[splitIndex] - clamped) <= 1e-9) return false;
  dwindleRatios_[splitIndex] = clamped;
  DebugValidateMutation(L"Workspace::SetDwindleRatio");
  return true;
}

bool Workspace::HasSameWindowMembershipAs(const Workspace& other) const {
  if (windows_.size() != other.windows_.size() ||
      records_.size() != other.records_.size()) {
    return false;
  }
  for (const auto& [hwnd, record] : records_) {
    auto otherIt = other.records_.find(hwnd);
    if (otherIt == other.records_.end() ||
        record.pid != otherIt->second.pid ||
        record.tilingGeneration != otherIt->second.tilingGeneration ||
        record.state != otherIt->second.state ||
        record.alwaysFloating != otherIt->second.alwaysFloating ||
        record.suspensionReason != otherIt->second.suspensionReason ||
        record.hasSavedSlot != otherIt->second.hasSavedSlot) {
      return false;
    }
  }
  return true;
}

bool Workspace::HasSameLayoutStateAs(const Workspace& other) const {
  if (!HasSameWindowMembershipAs(other)) return false;
  for (const auto& [hwnd, record] : records_) {
    const auto& saved = other.records_.at(hwnd).savedSlot;
    if (record.hasSavedSlot &&
        (record.savedSlot.index != saved.index ||
         record.savedSlot.weight != saved.weight ||
         record.savedSlot.hasWeight != saved.hasWeight ||
         record.savedSlot.wasMaster != saved.wasMaster)) return false;
  }
  return windows_ == other.windows_ &&
      layout_ == other.layout_ && masterRatio_ == other.masterRatio_ &&
      inheritedSlotWeights_ == other.inheritedSlotWeights_ &&
      stackWeights_ == other.stackWeights_ &&
      gridWeights_ == other.gridWeights_ &&
      dwindleRatios_ == other.dwindleRatios_ &&
      rememberedMasterStackOrder_ == other.rememberedMasterStackOrder_ &&
      rememberedMasterStackWeights_ == other.rememberedMasterStackWeights_ &&
      rememberedGridOrder_ == other.rememberedGridOrder_ &&
      rememberedGridWeights_ == other.rememberedGridWeights_ &&
      rememberedDwindleOrder_ == other.rememberedDwindleOrder_ &&
      rememberedDwindleRatios_ == other.rememberedDwindleRatios_;
}

bool Workspace::RestoreLayoutStateFrom(const Workspace& source) {
  AssertWmThread(L"Workspace::RestoreLayoutStateFrom");
  if (!HasSameWindowMembershipAs(source)) return false;

  windows_ = source.windows_;
  layout_ = source.layout_;
  masterRatio_ = source.masterRatio_;
  inheritedSlotWeights_ = source.inheritedSlotWeights_;
  stackWeights_ = source.stackWeights_;
  gridWeights_ = source.gridWeights_;
  dwindleRatios_ = source.dwindleRatios_;
  rememberedMasterStackOrder_ = source.rememberedMasterStackOrder_;
  rememberedMasterStackWeights_ = source.rememberedMasterStackWeights_;
  rememberedGridOrder_ = source.rememberedGridOrder_;
  rememberedGridWeights_ = source.rememberedGridWeights_;
  rememberedDwindleOrder_ = source.rememberedDwindleOrder_;
  rememberedDwindleRatios_ = source.rememberedDwindleRatios_;
  for (auto& [hwnd, record] : records_) {
    const WindowRecord& sourceRecord = source.records_.find(hwnd)->second;
    if (record.state == ManageState::Suspended) {
      record.savedSlot = sourceRecord.savedSlot;
    }
  }
  DebugValidateMutation(L"Workspace::RestoreLayoutStateFrom");
  return true;
}

// Restore only the active layout/order geometry from a compatible snapshot.
// Unlike RestoreLayoutStateFrom, this intentionally tolerates extra/missing
// inactive records. It is used when a pending admission has already left active
// participation: the established tiled membership must match, while the pending
// HWND itself may now be Suspended instead of Tiled.
bool Workspace::RestoreCompatibleActiveLayoutStateFrom(
    const Workspace& source) {
  AssertWmThread(L"Workspace::RestoreCompatibleActiveLayoutStateFrom");
  if (layout_ != source.layout_ || windows_.size() != source.windows_.size()) {
    return false;
  }

  for (HWND hwnd : windows_) {
    const WindowRecord* currentRecord = Find(hwnd);
    const WindowRecord* sourceRecord = source.Find(hwnd);
    if (!currentRecord || !sourceRecord ||
        currentRecord->state != ManageState::Tiled ||
        sourceRecord->state != ManageState::Tiled ||
        currentRecord->pid != sourceRecord->pid ||
        currentRecord->tilingGeneration != sourceRecord->tilingGeneration) {
      return false;
    }
  }
  for (HWND hwnd : source.windows_) {
    if (!IsTiled(hwnd)) return false;
  }

  windows_ = source.windows_;
  masterRatio_ = source.masterRatio_;
  inheritedSlotWeights_ = source.inheritedSlotWeights_;
  stackWeights_ = source.stackWeights_;
  gridWeights_ = source.gridWeights_;
  dwindleRatios_ = source.dwindleRatios_;
  rememberedMasterStackOrder_ = source.rememberedMasterStackOrder_;
  rememberedMasterStackWeights_ = source.rememberedMasterStackWeights_;
  rememberedGridOrder_ = source.rememberedGridOrder_;
  rememberedGridWeights_ = source.rememberedGridWeights_;
  rememberedDwindleOrder_ = source.rememberedDwindleOrder_;
  rememberedDwindleRatios_ = source.rememberedDwindleRatios_;

  // Undo logical-index shifts caused by the provisional admission for suspended
  // members that existed in both snapshots. Current-only inactive records are
  // deliberately preserved.
  for (auto& [hwnd, record] : records_) {
    if (record.state != ManageState::Suspended) continue;
    const WindowRecord* sourceRecord = source.Find(hwnd);
    if (!sourceRecord || sourceRecord->state != ManageState::Suspended ||
        !sourceRecord->hasSavedSlot) {
      continue;
    }
    record.savedSlot = sourceRecord->savedSlot;
    record.hasSavedSlot = true;
  }

  EnsureWeights();
  DebugValidateMutation(L"Workspace::RestoreCompatibleActiveLayoutStateFrom");
  return true;
}

bool Workspace::MakeMaster(HWND hwnd, HWND* oldMaster) {
  AssertWmThread(L"Workspace::MakeMaster");
  if (layout_ == TileLayout::Floating || windows_.size() < 2) return false;
  auto it = std::find(windows_.begin(), windows_.end(), hwnd);
  if (it == windows_.end() || it == windows_.begin()) return false;
  if (oldMaster) *oldMaster = windows_.front();
  std::iter_swap(windows_.begin(), it);
  DebugValidateMutation(L"Workspace::MakeMaster");
  return true;
}

bool Workspace::SwapTiled(HWND first, HWND second) {
  AssertWmThread(L"Workspace::SwapTiled");
  auto firstIt = std::find(windows_.begin(), windows_.end(), first);
  auto secondIt = std::find(windows_.begin(), windows_.end(), second);
  if (firstIt == windows_.end() || secondIt == windows_.end() || firstIt == secondIt) {
    return false;
  }
  std::iter_swap(firstIt, secondIt);
  DebugValidateMutation(L"Workspace::SwapTiled");
  return true;
}

bool Workspace::MovePendingAdmissionToIndex(HWND hwnd, size_t targetIndex) {
  AssertWmThread(L"Workspace::MovePendingAdmissionToIndex");
  auto current = std::find(windows_.begin(), windows_.end(), hwnd);
  if (current == windows_.end()) return false;

  WindowRecord* record = FindMutable(hwnd);
  const size_t currentIndex =
      static_cast<size_t>(std::distance(windows_.begin(), current));
  if (!record || !record->pendingInsertionPlacement ||
      targetIndex >= windows_.size() || targetIndex == currentIndex) {
    return false;
  }

  EnsureWeights();
  const size_t oldCount = windows_.size();
  const size_t oldLogicalIndex = ActiveIndexToLogicalIndex(currentIndex);
  windows_.erase(current);

  if ((layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) &&
      gridWeights_.size() == oldCount) {
    gridWeights_.erase(gridWeights_.begin() + currentIndex);
  } else if ((layout_ == TileLayout::MasterStack ||
              layout_ == TileLayout::MasterStackH) &&
             oldCount > 1 && stackWeights_.size() == oldCount - 1) {
    const size_t stackIndex = currentIndex == 0 ? 0 : currentIndex - 1;
    stackWeights_.erase(stackWeights_.begin() + stackIndex);
  }
  CloseLogicalGapAfterRemoval(oldLogicalIndex, hwnd);

  const size_t insertionLogicalIndex = ActiveIndexToLogicalIndex(targetIndex);
  for (auto& kv : records_) {
    WindowRecord& existing = kv.second;
    if (existing.state == ManageState::Suspended && existing.hasSavedSlot &&
        existing.savedSlot.index >= insertionLogicalIndex) {
      ++existing.savedSlot.index;
    }
  }

  windows_.insert(windows_.begin() + targetIndex, hwnd);
  if (layout_ == TileLayout::Columns || layout_ == TileLayout::Rows) {
    gridWeights_.insert(gridWeights_.begin() + targetIndex, 1.0);
  } else if ((layout_ == TileLayout::MasterStack ||
              layout_ == TileLayout::MasterStackH) &&
             oldCount > 1) {
    const size_t stackIndex = targetIndex == 0 ? 0 : targetIndex - 1;
    stackWeights_.insert(stackWeights_.begin() + stackIndex, 1.0);
  }

  EnsureWeights();
  DebugValidateMutation(L"Workspace::MovePendingAdmissionToIndex");
  return true;
}

bool Workspace::ClearInsertionPlacement(HWND hwnd) {
  AssertWmThread(L"Workspace::ClearInsertionPlacement");
  WindowRecord* record = FindMutable(hwnd);
  if (!record || !record->pendingInsertionPlacement) return false;
  record->pendingInsertionPlacement = false;
  DebugValidateMutation(L"Workspace::ClearInsertionPlacement");
  return true;
}

bool Workspace::ClearDefaultFloatingCenter(HWND hwnd) {
  AssertWmThread(L"Workspace::ClearDefaultFloatingCenter");
  WindowRecord* record = FindMutable(hwnd);
  if (!record || !record->pendingDefaultFloatingCenter) return false;
  record->pendingDefaultFloatingCenter = false;
  DebugValidateMutation(L"Workspace::ClearDefaultFloatingCenter");
  return true;
}

bool Workspace::EnforceAlwaysFloating(HWND hwnd) {
  AssertWmThread(L"Workspace::EnforceAlwaysFloating");
  WindowRecord* record = FindMutable(hwnd);
  if (!record) return false;

  bool changed = !record->alwaysFloating;
  record->alwaysFloating = true;
  if (record->state != ManageState::Floating) {
    changed |= Float(hwnd);
  }
  if (changed) DebugValidateMutation(L"Workspace::EnforceAlwaysFloating");
  return changed;
}

void Workspace::RepairForArrangement(
    const std::vector<HWND>& invalidWindows,
    const std::vector<WindowRecord>& missingActiveRecords) {
  AssertWmThread(L"Workspace::RepairForArrangement");
  for (HWND hwnd : invalidWindows) Forget(hwnd);

  for (WindowRecord record : missingActiveRecords) {
    if (!record.hwnd || !ContainsWindow(windows_, record.hwnd)) continue;
    if (!record.tilingGeneration) {
      record.tilingGeneration = Model::NextTilingGeneration();
    }
    record.state = ManageState::Tiled;
    record.suspensionReason = SuspensionReason::None;
    record.savedSlot = {};
    record.hasSavedSlot = false;
    UpsertRecord(std::move(record));
  }
  EnsureWeights();
  DebugValidateMutation(L"Workspace::RepairForArrangement");
}




static bool IsWorkspaceOnActiveDesktop(const DesktopMonitorKey& key) {
  GUID zeroGuid{};
  if (IsEqualGUID(key.desktopId, zeroGuid)) return false;

  // The reconciled desktop is actor-owned and authoritative between transition
  // signals. Avoid re-querying Explorer from high-frequency placement and
  // LOCATIONCHANGE paths; a dirty transition deliberately retains the old GUID
  // for comparison while falling through to the existing live query.
  if (g_wm.reconciledDesktopState == ReconciledDesktopState::Settled) {
    return IsEqualGUID(g_wm.reconciledDesktop, key.desktopId);
  }

  GUID currentDesktop{};
  if (!GetCurrentDesktopId(&currentDesktop)) return false;
  return IsEqualGUID(currentDesktop, key.desktopId);
}

// Repairs only model-local arrangement preconditions. OS lifecycle/ownership
// reconciliation belongs elsewhere; this function handles missed destruction and
// legacy states whose active-order record marker drifted.
static void ObserveAndRepairWorkspaceForArrangement(Workspace& workspace) {
  std::vector<HWND> invalidWindows;
  std::vector<WindowRecord> missingActiveRecords;

  for (const auto& kv : workspace.Records()) {
    if (!IsWindow(kv.first)) invalidWindows.push_back(kv.first);
  }
  for (HWND hwnd : workspace.TiledWindows()) {
    if (!workspace.HasRecord(hwnd) && IsWindow(hwnd)) {
      missingActiveRecords.push_back(MakeWindowRecord(hwnd, ManageState::Tiled));
    }
  }
  workspace.RepairForArrangement(invalidWindows, missingActiveRecords);
}

//-----------------------------------------------------------------------------
// Workspace resize-learning operations
//-----------------------------------------------------------------------------

// Refreshes the application-owned native minimum contract without introducing a
// second solver notion of "proof". Normal arrangement probes only records that
// are missing, stale-DPI, or still resolving an admission/restore. User-authored
// mutations and explicit Tile Workspace operations can force a refresh so a
// relaxed same-DPI minimum cannot remain cached indefinitely.
static bool RefreshWorkspaceNativeConstraints(
    Workspace& workspace, bool forceAll, const wchar_t* reason) {
  bool changed = false;
  for (HWND hwnd : workspace.TiledWindows()) {
    const WindowRecord* record = workspace.Find(hwnd);
    if (!record ||
        GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
        IsWindowCloaked(hwnd)) {
      continue;
    }

    const HMONITOR windowMonitor = GetWindowPhysicalMonitor(hwnd);
    const UINT currentDpi = windowMonitor
        ? GetMonitorEffectiveDpi(windowMonitor)
        : 0;
    const bool needsProbe =
        forceAll || record->pendingInsertionPlacement ||
        !record->constraints.valid ||
        (currentDpi && currentDpi != record->constraints.dpi);
    if (!needsProbe) continue;

    const WindowConstraints before = record->constraints;
    const WindowConstraints fresh = ProbeWindowConstraints(hwnd);
    const WindowConstraints refreshed =
        MergeConstraintProbeWithCache(before, fresh);
    const bool geometryChanged =
        !SameConstraintGeometry(before, refreshed);
    changed |= workspace.UpdateConstraints(hwnd, refreshed);

    if (fresh.valid && geometryChanged) {
      Diagnostics::RecordEvent(
          L"native constraint report refreshed reason=%ls hwnd=%p min=%ldx%ld dpi=%u",
          reason ? reason : L"arrange", hwnd, refreshed.minWidth,
          refreshed.minHeight, refreshed.dpi);
    }
  }
  return changed;
}

static std::vector<WindowConstraints> ActiveWindowConstraints(
    const Workspace& workspace) {
  std::vector<WindowConstraints> constraints;
  constraints.reserve(workspace.ActiveCount());
  for (HWND hwnd : workspace.TiledWindows()) {
    const WindowRecord* record = workspace.Find(hwnd);
    constraints.push_back(record ? record->constraints : WindowConstraints{});
  }
  return constraints;
}

static bool RectsSatisfyWindowConstraints(
    const std::vector<RECT>& rects,
    const std::vector<WindowConstraints>& constraints) {
  if (rects.size() != constraints.size()) return false;
  for (size_t i = 0; i < rects.size(); ++i) {
    const LONG width = rects[i].right - rects[i].left;
    const LONG height = rects[i].bottom - rects[i].top;
    const WindowConstraints& c = constraints[i];
    if (c.valid &&
        (width < std::max<LONG>(1, c.minWidth) ||
         height < std::max<LONG>(1, c.minHeight))) {
      return false;
    }
  }
  return true;
}

constexpr LONG kResizeEdgeDeltaTolerance = 1;
constexpr LONG kMinLearnedResizeSpan = 80;

static bool IsMeaningfulResizeDelta(LONG delta) {
  return std::abs(delta) > kResizeEdgeDeltaTolerance;
}

// Transfers one divider delta between adjacent logical weights while preserving
// their combined weight. Expected layout rectangles provide the pre-drag spans;
// live sibling geometry is intentionally never learned into durable state.
static bool TransferPairResizeDelta(
    std::vector<double>& weights, size_t firstWeight,
    const RECT& firstRect, const RECT& secondRect,
    bool verticalAxis, LONG delta) {
  if (!IsMeaningfulResizeDelta(delta) || firstWeight + 1 >= weights.size()) {
    return false;
  }

  const LONG firstSpan = verticalAxis ? firstRect.bottom - firstRect.top
                                      : firstRect.right - firstRect.left;
  const LONG secondSpan = verticalAxis ? secondRect.bottom - secondRect.top
                                       : secondRect.right - secondRect.left;
  const LONG pairSpan = firstSpan + secondSpan;
  if (firstSpan < 1 || secondSpan < 1 || pairSpan <= 1) return false;

  const LONG minSpan = std::min<LONG>(
      kMinLearnedResizeSpan, std::max<LONG>(1, (pairSpan - 1) / 2));
  const LONG maxFirstSpan = pairSpan - minSpan;
  const LONG newFirstSpan =
      std::clamp<LONG>(firstSpan + delta, minSpan, maxFirstSpan);
  if (newFirstSpan == firstSpan) return false;

  double pairWeight = weights[firstWeight] + weights[firstWeight + 1];
  if (!(pairWeight > 0.0) || !std::isfinite(pairWeight)) pairWeight = 2.0;

  const double firstFraction =
      static_cast<double>(newFirstSpan) / static_cast<double>(pairSpan);
  weights[firstWeight] = pairWeight * firstFraction;
  weights[firstWeight + 1] = pairWeight - weights[firstWeight];
  return true;
}

bool Workspace::LearnMasterStackResize(
    const RECT& workArea, LONG gap, size_t resizedIndex,
    const MoveSizeGesture& gesture) {
  AssertWmThread(L"Workspace::LearnMasterStackResize");
  EnsureWeights();
  const bool horizontal = layout_ == TileLayout::MasterStackH;
  const std::vector<WindowConstraints> constraints =
      ActiveWindowConstraints(*this);

  std::vector<RECT> expectedRects;
  Layout::LayoutMasterStackWeighted(
      workArea, gap, windows_.size(), expectedRects, horizontal,
      ClampDouble(masterRatio_, 0.1, 0.9), stackWeights_);
  if (!RectsSatisfyWindowConstraints(expectedRects, constraints)) {
    std::vector<RECT> constrainedRects;
    if (Layout::LayoutMasterStackConstrained(
            workArea, gap, horizontal,
            ClampDouble(masterRatio_, 0.1, 0.9), stackWeights_,
            constraints, &constrainedRects)) {
      expectedRects = std::move(constrainedRects);
    }
  }
  if (expectedRects.size() != windows_.size()) return false;

  bool changed = false;

  // Main master/stack divider.
  const LONG mainDelta = horizontal
      ? (resizedIndex == 0 ? gesture.end.bottom - gesture.start.bottom
                           : gesture.end.top - gesture.start.top)
      : (resizedIndex == 0 ? gesture.end.right - gesture.start.right
                           : gesture.end.left - gesture.start.left);

  if (windows_.size() > 1 && IsMeaningfulResizeDelta(mainDelta)) {
    const LONG totalSize = horizontal ? workArea.bottom - workArea.top
                                      : workArea.right - workArea.left;
    const LONG usableSize = totalSize - gap;
    if (usableSize > 1) {
      const LONG oldMasterSpan = horizontal
          ? expectedRects[0].bottom - expectedRects[0].top
          : expectedRects[0].right - expectedRects[0].left;
      const LONG newMasterSpan = std::clamp<LONG>(
          oldMasterSpan + mainDelta, 1, usableSize - 1);
      const double newRatio = ClampDouble(
          static_cast<double>(newMasterSpan) / static_cast<double>(usableSize),
          0.1, 0.9);
      if (std::abs(newRatio - masterRatio_) > 1e-9) {
        masterRatio_ = newRatio;
        changed = true;
      }
    }
  }

  // Stack-axis dividers.
  if (resizedIndex > 0 && stackWeights_.size() + 1 == windows_.size()) {
    const size_t stackIndex = resizedIndex - 1;
    const LONG leadingDelta = horizontal
        ? gesture.end.left - gesture.start.left
        : gesture.end.top - gesture.start.top;
    const LONG trailingDelta = horizontal
        ? gesture.end.right - gesture.start.right
        : gesture.end.bottom - gesture.start.bottom;

    if (stackIndex > 0) {
      changed |= TransferPairResizeDelta(
          stackWeights_, stackIndex - 1,
          expectedRects[resizedIndex - 1], expectedRects[resizedIndex],
          !horizontal, leadingDelta);
    }
    if (stackIndex + 1 < stackWeights_.size()) {
      changed |= TransferPairResizeDelta(
          stackWeights_, stackIndex,
          expectedRects[resizedIndex], expectedRects[resizedIndex + 1],
          !horizontal, trailingDelta);
    }
  }

  if (changed) DebugValidateMutation(L"Workspace::LearnMasterStackResize");
  return changed;
}

bool Workspace::LearnGridResize(
    const RECT& workArea, LONG gap, size_t resizedIndex,
    const MoveSizeGesture& gesture) {
  AssertWmThread(L"Workspace::LearnGridResize");
  EnsureWeights();
  const bool horizontal = layout_ == TileLayout::Rows;
  const std::vector<WindowConstraints> constraints =
      ActiveWindowConstraints(*this);

  std::vector<RECT> expectedRects;
  Layout::LayoutGridWeighted(
      workArea, gap, windows_.size(), expectedRects,
      horizontal, gridWeights_);
  if (!RectsSatisfyWindowConstraints(expectedRects, constraints)) {
    std::vector<RECT> constrainedRects;
    if (Layout::LayoutGridConstrained(
            workArea, gap, gridWeights_, constraints,
            horizontal, &constrainedRects)) {
      expectedRects = std::move(constrainedRects);
    }
  }
  if (expectedRects.size() != windows_.size()) return false;

  bool changed = false;
  const LONG leadingDelta = horizontal
      ? gesture.end.top - gesture.start.top
      : gesture.end.left - gesture.start.left;
  const LONG trailingDelta = horizontal
      ? gesture.end.bottom - gesture.start.bottom
      : gesture.end.right - gesture.start.right;

  if (resizedIndex > 0) {
    changed |= TransferPairResizeDelta(
        gridWeights_, resizedIndex - 1,
        expectedRects[resizedIndex - 1], expectedRects[resizedIndex],
        horizontal, leadingDelta);
  }
  if (resizedIndex + 1 < windows_.size()) {
    changed |= TransferPairResizeDelta(
        gridWeights_, resizedIndex,
        expectedRects[resizedIndex], expectedRects[resizedIndex + 1],
        horizontal, trailingDelta);
  }
  if (changed) DebugValidateMutation(L"Workspace::LearnGridResize");
  return changed;
}

bool Workspace::LearnDwindleResize(
    const RECT& workArea, LONG gap, size_t resizedIndex,
    const MoveSizeGesture& gesture) {
  AssertWmThread(L"Workspace::LearnDwindleResize");
  EnsureWeights();
  if (layout_ != TileLayout::Dwindle || windows_.size() < 2 ||
      resizedIndex >= windows_.size()) {
    return false;
  }

  std::vector<RECT> expectedRects(windows_.size());
  std::vector<Layout::DwindleSplit> splits;
  Layout::LayoutDwindle(
      workArea, gap, 0, windows_.size(), dwindleRatios_,
      g_settings.reverseDwindle, expectedRects, &splits);
  if (splits.size() + 1 != dwindleRatios_.size()) return false;

  const RECT& resizedRect = expectedRects[resizedIndex];
  size_t selectedVerticalSplit = static_cast<size_t>(-1);
  size_t selectedHorizontalSplit = static_cast<size_t>(-1);
  LONG selectedVerticalDelta = 0;
  LONG selectedHorizontalDelta = 0;
  for (size_t splitIndex = 0;
       splitIndex < splits.size() && splitIndex <= resizedIndex;
       ++splitIndex) {
    const Layout::DwindleSplit& split = splits[splitIndex];
    LONG firstSpanDelta = 0;
    if (split.vertical) {
      if (resizedIndex == splitIndex) {
        firstSpanDelta = g_settings.reverseDwindle
            ? -(gesture.end.left - gesture.start.left)
            : gesture.end.right - gesture.start.right;
      } else if ((!g_settings.reverseDwindle &&
                  resizedRect.left == split.remaining.left) ||
                 (g_settings.reverseDwindle &&
                  resizedRect.right == split.remaining.right)) {
        firstSpanDelta = g_settings.reverseDwindle
            ? -(gesture.end.right - gesture.start.right)
            : gesture.end.left - gesture.start.left;
      }
    } else {
      if (resizedIndex == splitIndex) {
        firstSpanDelta = g_settings.reverseDwindle
            ? -(gesture.end.top - gesture.start.top)
            : gesture.end.bottom - gesture.start.bottom;
      } else if ((!g_settings.reverseDwindle &&
                  resizedRect.top == split.remaining.top) ||
                 (g_settings.reverseDwindle &&
                  resizedRect.bottom == split.remaining.bottom)) {
        firstSpanDelta = g_settings.reverseDwindle
            ? -(gesture.end.bottom - gesture.start.bottom)
            : gesture.end.top - gesture.start.top;
      }
    }
    if (!IsMeaningfulResizeDelta(firstSpanDelta) || split.usableSpan < 2) {
      continue;
    }

    // A descendant can share an edge with more than one same-axis ancestor.
    // Keep the nearest match on each axis. A corner drag may intentionally
    // update one horizontal and one vertical divider.
    if (split.vertical) {
      selectedVerticalSplit = splitIndex;
      selectedVerticalDelta = firstSpanDelta;
    } else {
      selectedHorizontalSplit = splitIndex;
      selectedHorizontalDelta = firstSpanDelta;
    }
  }

  bool changed = false;
  auto applySelectedDivider = [&](size_t splitIndex, LONG firstSpanDelta) {
    if (splitIndex >= splits.size()) return;
    const Layout::DwindleSplit& split = splits[splitIndex];
    const LONG minimumSpan = std::min<LONG>(
        kMinLearnedResizeSpan,
        std::max<LONG>(1, (split.usableSpan - 1) / 2));
    const LONG newFirstSpan = std::clamp<LONG>(
        split.firstSpan + firstSpanDelta,
        minimumSpan, split.usableSpan - minimumSpan);
    const double newRatio = ClampDouble(
        static_cast<double>(newFirstSpan) /
            static_cast<double>(split.usableSpan),
        0.1, 0.9);
    if (std::abs(newRatio - dwindleRatios_[splitIndex]) <= 1e-9) return;

    dwindleRatios_[splitIndex] = newRatio;
    changed = true;
    Wh_Log(
        L"Dwindle divider updated slot=%zu ratio=%.6f",
        splitIndex, newRatio);
    Diagnostics::RecordEvent(
        L"dwindle divider updated hwnd=%p slot=%zu ratio=%.6f",
        windows_[resizedIndex], splitIndex, newRatio);
  };
  applySelectedDivider(selectedVerticalSplit, selectedVerticalDelta);
  applySelectedDivider(selectedHorizontalSplit, selectedHorizontalDelta);
  if (!changed) return false;

  DebugValidateMutation(L"Workspace::LearnDwindleResize");
  return true;
}

enum class LayoutPlanStatus {
  Success,
  NotApplicable,
  Unsatisfiable,
};

static LayoutPlanStatus BuildWorkspaceLayoutPlan(
    const Workspace& workspace, const RECT& workArea, LONG gap,
    std::vector<RECT>* outRects) {
  if (!outRects) return LayoutPlanStatus::Unsatisfiable;
  outRects->assign(workspace.ActiveCount(), RECT{});

  // Always begin from MultiWM's ordinary geometry. When the native constraints
  // already fit, this stays byte-for-byte on the established layout path.
  switch (workspace.Layout()) {
    case TileLayout::MasterStack:
      Layout::LayoutMasterStackWeighted(
          workArea, gap, workspace.ActiveCount(), *outRects, false,
          ClampDouble(workspace.MasterRatio(), 0.1, 0.9),
          workspace.StackWeights());
      break;

    case TileLayout::MasterStackH:
      Layout::LayoutMasterStackWeighted(
          workArea, gap, workspace.ActiveCount(), *outRects, true,
          ClampDouble(workspace.MasterRatio(), 0.1, 0.9),
          workspace.StackWeights());
      break;

    case TileLayout::Columns:
      Layout::LayoutGridWeighted(
          workArea, gap, workspace.ActiveCount(), *outRects, false,
          workspace.GridWeights());
      break;

    case TileLayout::Rows:
      Layout::LayoutGridWeighted(
          workArea, gap, workspace.ActiveCount(), *outRects, true,
          workspace.GridWeights());
      break;

    case TileLayout::Dwindle:
      Layout::LayoutDwindle(
          workArea, gap, 0, workspace.ActiveCount(),
          workspace.DwindleRatios(), g_settings.reverseDwindle, *outRects);
      break;

    case TileLayout::Monocle:
      outRects->assign(workspace.ActiveCount(), workArea);
      break;

    case TileLayout::Floating:
    case TileLayout::COUNT:
      outRects->clear();
      return LayoutPlanStatus::NotApplicable;
  }

  const std::vector<WindowConstraints> constraints =
      ActiveWindowConstraints(workspace);
  if (RectsSatisfyWindowConstraints(*outRects, constraints)) {
    return LayoutPlanStatus::Success;
  }

  bool constrainedSuccess = false;
  switch (workspace.Layout()) {
    case TileLayout::MasterStack:
      constrainedSuccess = Layout::LayoutMasterStackConstrained(
          workArea, gap, false,
          ClampDouble(workspace.MasterRatio(), 0.1, 0.9),
          workspace.StackWeights(), constraints, outRects);
      break;

    case TileLayout::MasterStackH:
      constrainedSuccess = Layout::LayoutMasterStackConstrained(
          workArea, gap, true,
          ClampDouble(workspace.MasterRatio(), 0.1, 0.9),
          workspace.StackWeights(), constraints, outRects);
      break;

    case TileLayout::Columns:
      constrainedSuccess = Layout::LayoutGridConstrained(
          workArea, gap, workspace.GridWeights(),
          constraints, false, outRects);
      break;

    case TileLayout::Rows:
      constrainedSuccess = Layout::LayoutGridConstrained(
          workArea, gap, workspace.GridWeights(),
          constraints, true, outRects);
      break;

    case TileLayout::Dwindle:
      // Dwindle ratios are tree state. Constraint handling may reassign a fresh
      // admission or fit the shared shallow-divider policy, but ordinary planning
      // never reshapes the tree around its current occupants.
      return LayoutPlanStatus::Unsatisfiable;

    case TileLayout::Monocle:
      constrainedSuccess = Layout::LayoutMonocleConstrained(
          workArea, constraints, outRects);
      break;

    case TileLayout::Floating:
    case TileLayout::COUNT:
      return LayoutPlanStatus::NotApplicable;
  }

  if (constrainedSuccess) return LayoutPlanStatus::Success;
  return LayoutPlanStatus::Unsatisfiable;
}

struct AdaptiveDwindleRatioEnvelope {
  double minimum = 0.1;
  double maximum = 0.9;
};

static bool AutomaticDwindleAdaptationEnabled() {
  return g_settings.adaptiveDwindleSlots > 0 &&
      g_settings.adaptiveDwindlePercentage > 0;
}

static AdaptiveDwindleRatioEnvelope GetAdaptiveDwindleRatioEnvelope() {
  const double deviation =
      static_cast<double>(std::clamp<LONG>(
          g_settings.adaptiveDwindlePercentage, 0, 40)) /
      100.0;
  return {0.5 - deviation, 0.5 + deviation};
}

// Admission recovery and explicit order-mutation preflight share one geometry
// fitter. Leading split nodes may move according to Adaptive Dwindle Slots and
// Adaptive Dwindle Percentage; every deeper ratio and the complete window order
// stay fixed. A percentage of zero deliberately disables automatic ratio
// changes. A manual ratio outside the adaptive envelope is retained whenever it
// is already feasible; the envelope constrains only an automatic replacement.
static bool FitAdaptiveDwindleDividers(
    Workspace& candidate, const RECT& workArea, LONG gap) {
  if (candidate.Layout() != TileLayout::Dwindle || candidate.ActiveCount() < 2 ||
      !AutomaticDwindleAdaptationEnabled()) {
    return false;
  }

  const auto& windows = candidate.TiledWindows();
  const auto constraints = ActiveWindowConstraints(candidate);
  const auto& ratios = candidate.DwindleRatios();
  const size_t adaptiveSplitCount =
      std::min(g_settings.adaptiveDwindleSlots, ratios.size());
  if (adaptiveSplitCount == 0) return false;
  const AdaptiveDwindleRatioEnvelope adaptiveEnvelope =
      GetAdaptiveDwindleRatioEnvelope();

  std::vector<SIZE> windowMinimums(windows.size(), SIZE{1, 1});
  for (size_t i = 0; i < constraints.size(); ++i) {
    const auto& constraint = constraints[i];
    if (constraint.valid) {
      windowMinimums[i].cx = std::max<LONG>(1, constraint.minWidth);
      windowMinimums[i].cy = std::max<LONG>(1, constraint.minHeight);
    }
  }

  const LONG maximumSpan = std::max(RectWidth(workArea), RectHeight(workArea));
  auto requiredSpan = [&](LONG first, LONG tail, size_t splitIndex,
                          bool adaptive) {
    const double fixedRatio = ClampDouble(ratios[splitIndex], 0.1, 0.9);
    auto fits = [&](LONG span) {
      if (span < 2) return false;
      const LONG usable = span - std::clamp<LONG>(gap, 0, span - 2);
      if (usable < 2) return false;

      if (!adaptive) {
        const LONG allocated = std::clamp<LONG>(
            static_cast<LONG>(std::llround(usable * fixedRatio)),
            1, usable - 1);
        return allocated >= first && usable - allocated >= tail;
      }

      // A manually chosen ratio outside the adaptive envelope remains a legal
      // no-op if it already satisfies both sides. Only a ratio we actually need
      // to change must land inside the configured symmetric envelope.
      const LONG currentAllocated = std::clamp<LONG>(
          static_cast<LONG>(std::llround(usable * fixedRatio)),
          1, usable - 1);
      if (currentAllocated >= first && usable - currentAllocated >= tail) {
        return true;
      }

      const LONG minimumAdaptive = std::clamp<LONG>(
          static_cast<LONG>(std::llround(
              usable * adaptiveEnvelope.minimum)), 1, usable - 1);
      const LONG maximumAdaptive = std::clamp<LONG>(
          static_cast<LONG>(std::llround(
              usable * adaptiveEnvelope.maximum)), 1, usable - 1);
      const LONG lower = std::max(first, minimumAdaptive);
      const LONG upper = std::min(usable - tail, maximumAdaptive);
      return lower <= upper;
    };

    if (!fits(maximumSpan)) return maximumSpan + 1;
    LONG low = 2;
    LONG high = maximumSpan;
    while (low < high) {
      const LONG middle = low + (high - low) / 2;
      if (fits(middle)) {
        high = middle;
      } else {
        low = middle + 1;
      }
    }
    return low;
  };

  // Minimum extent of each recursive tail. Fixed descendants preserve their
  // current ratios; adaptive descendants contribute only the space they need
  // somewhere inside the configured adaptive ratio envelope.
  std::vector<SIZE> subtreeMinimums = windowMinimums;
  for (size_t splitIndex = windows.size() - 1; splitIndex-- > 0;) {
    const SIZE first = windowMinimums[splitIndex];
    const SIZE tail = subtreeMinimums[splitIndex + 1];
    SIZE combined{
        std::max(first.cx, tail.cx),
        std::max(first.cy, tail.cy)};
    const bool adaptive = splitIndex < adaptiveSplitCount;
    if ((splitIndex % 2) == 0) {
      combined.cx = requiredSpan(first.cx, tail.cx, splitIndex, adaptive);
    } else {
      combined.cy = requiredSpan(first.cy, tail.cy, splitIndex, adaptive);
    }
    subtreeMinimums[splitIndex] = combined;
  }

  if (RectWidth(workArea) < subtreeMinimums[0].cx ||
      RectHeight(workArea) < subtreeMinimums[0].cy) {
    return false;
  }

  Workspace trial = candidate;
  RECT remaining = workArea;
  for (size_t splitIndex = 0; splitIndex < adaptiveSplitCount; ++splitIndex) {
    const bool vertical = (splitIndex % 2) == 0;
    const LONG span = vertical ? RectWidth(remaining) : RectHeight(remaining);
    if (span < 2) return false;
    const LONG usable = span - std::clamp<LONG>(gap, 0, span - 2);
    if (usable < 2) return false;

    const LONG firstMinimum = vertical
        ? windowMinimums[splitIndex].cx
        : windowMinimums[splitIndex].cy;
    const LONG tailMinimum = vertical
        ? subtreeMinimums[splitIndex + 1].cx
        : subtreeMinimums[splitIndex + 1].cy;
    const double preferred = ClampDouble(ratios[splitIndex], 0.1, 0.9);
    const LONG desired = std::clamp<LONG>(
        static_cast<LONG>(std::llround(usable * preferred)),
        1, usable - 1);
    LONG firstSpan = desired;
    if (desired < firstMinimum || usable - desired < tailMinimum) {
      const LONG minimumAdaptive = std::clamp<LONG>(
          static_cast<LONG>(std::llround(
              usable * adaptiveEnvelope.minimum)), 1, usable - 1);
      const LONG maximumAdaptive = std::clamp<LONG>(
          static_cast<LONG>(std::llround(
              usable * adaptiveEnvelope.maximum)), 1, usable - 1);
      const LONG lower = std::max(firstMinimum, minimumAdaptive);
      const LONG upper = std::min(usable - tailMinimum, maximumAdaptive);
      if (lower > upper) return false;
      firstSpan = std::clamp<LONG>(desired, lower, upper);
    }
    trial.SetDwindleRatio(
        splitIndex, static_cast<double>(firstSpan) / usable);

    std::vector<RECT> rects(windows.size());
    std::vector<Layout::DwindleSplit> splits;
    Layout::LayoutDwindle(
        workArea, gap, 0, windows.size(), trial.DwindleRatios(),
        g_settings.reverseDwindle, rects, &splits);
    if (splitIndex >= splits.size()) return false;
    remaining = splits[splitIndex].remaining;
  }

  std::vector<RECT> rects;
  if (BuildWorkspaceLayoutPlan(
          trial, workArea, gap, &rects) != LayoutPlanStatus::Success) {
    return false;
  }
  candidate = std::move(trial);
  return true;
}

static void RememberPendingLayoutMutation(
    const DesktopMonitorKey& key, const Workspace& previous,
    const Workspace& candidate, HWND subject, UserLayoutMutationKind kind,
    const wchar_t* action) {
  PendingLayoutMutation mutation;
  mutation.previous = previous;
  mutation.candidate = candidate;
  mutation.subject = subject;
  mutation.kind = kind;
  mutation.action = action ? action : L"Layout mutation";
  g_pendingLayoutMutations[key] = std::move(mutation);
}

static void DiscardPendingAdmissionRecovery(HWND hwnd) {
  if (hwnd) g_pendingAdmissionRecoveries.erase(hwnd);
}

static void RememberPendingAdmissionRecovery(
    const DesktopMonitorKey& key, HWND hwnd, const Workspace& baseline) {
  if (!hwnd) return;
  const WindowRecord* record = baseline.Find(hwnd);
  if (!record || record->state != ManageState::Tiled ||
      !record->pendingInsertionPlacement) {
    return;
  }

  auto existing = g_pendingAdmissionRecoveries.find(hwnd);
  if (existing != g_pendingAdmissionRecoveries.end() &&
      existing->second.generation == record->tilingGeneration &&
      Model::DesktopMonitorKeyEqual{}(existing->second.key, key)) {
    // Keep the earliest pre-recovery snapshot for this unsettled admission.
    return;
  }

  PendingAdmissionRecovery recovery;
  recovery.key = key;
  recovery.baseline = baseline;
  recovery.generation = record->tilingGeneration;
  g_pendingAdmissionRecoveries[hwnd] = std::move(recovery);
}

// If an admission/restore leaves active participation before its recovery has
// settled, derive the no-longer-active state from the pre-recovery snapshot and
// restore that established order/geometry into the live workspace. This makes
// adaptive ratios and sibling reassignment provisional instead of leaking into
// the N-1 layout after a minimize/hidden participation race.
static bool RestorePendingAdmissionRecoveryAfterSuspension(
    const DesktopMonitorKey& key, HWND hwnd, Workspace* workspace) {
  if (!workspace || !hwnd) return false;
  auto recoveryIt = g_pendingAdmissionRecoveries.find(hwnd);
  if (recoveryIt == g_pendingAdmissionRecoveries.end()) return false;

  PendingAdmissionRecovery recovery = recoveryIt->second;
  g_pendingAdmissionRecoveries.erase(recoveryIt);
  if (!Model::DesktopMonitorKeyEqual{}(recovery.key, key)) return false;

  const WindowRecord* currentRecord = workspace->Find(hwnd);
  const WindowRecord* baselineRecord = recovery.baseline.Find(hwnd);
  if (!currentRecord || !baselineRecord ||
      currentRecord->state != ManageState::Suspended ||
      baselineRecord->state != ManageState::Tiled ||
      currentRecord->tilingGeneration != recovery.generation ||
      baselineRecord->tilingGeneration != recovery.generation) {
    return false;
  }

  Workspace restored = recovery.baseline;
  if (!restored.Suspend(hwnd, currentRecord->suspensionReason) ||
      !workspace->RestoreCompatibleActiveLayoutStateFrom(restored)) {
    Diagnostics::RecordEvent(
        L"pending admission recovery snapshot became incompatible hwnd=%p desktop=%08X",
        hwnd, key.desktopId.Data1);
    return false;
  }

  Diagnostics::RecordEvent(
      L"pending admission recovery rolled back after suspension hwnd=%p desktop=%08X",
      hwnd, key.desktopId.Data1);
  return true;
}

static HWND FindConstraintCulprit(
    const Workspace& workspace, const std::vector<RECT>& rects,
    HWND preferred = nullptr) {
  const std::vector<WindowConstraints> constraints =
      ActiveWindowConstraints(workspace);
  auto violatesAssignedRect = [&](size_t index) {
    if (index >= constraints.size() || index >= rects.size()) return false;
    const WindowConstraints& constraint = constraints[index];
    return constraint.valid &&
        (RectWidth(rects[index]) < std::max<LONG>(1, constraint.minWidth) ||
         RectHeight(rects[index]) < std::max<LONG>(1, constraint.minHeight));
  };

  const size_t preferredIndex = workspace.TiledIndex(preferred);
  if (violatesAssignedRect(preferredIndex)) return preferred;
  for (size_t i = 0; i < constraints.size(); ++i) {
    if (violatesAssignedRect(i)) return workspace.TiledWindows()[i];
  }
  return nullptr;
}

// Shared constraint compatibility surface. Admission recovery and conflict
// reporting deliberately use the same fit predicate and augmenting-path primitive
// so diagnostics cannot disagree with the solver about whether a slot is usable.
static bool WindowConstraintFitsSlot(
    const WindowConstraints& constraint, const RECT& slot) {
  return !constraint.valid ||
      (RectWidth(slot) >= std::max<LONG>(1, constraint.minWidth) &&
       RectHeight(slot) >= std::max<LONG>(1, constraint.minHeight));
}

struct ConstraintSlotCompatibility {
  std::vector<std::vector<size_t>> slotsByWindow;
};

static ConstraintSlotCompatibility BuildConstraintSlotCompatibility(
    const std::vector<WindowConstraints>& constraints,
    const std::vector<RECT>& slots, bool preferNearbySlots) {
  ConstraintSlotCompatibility compatibility;
  compatibility.slotsByWindow.resize(constraints.size());
  for (size_t windowIndex = 0; windowIndex < constraints.size(); ++windowIndex) {
    auto& compatibleSlots = compatibility.slotsByWindow[windowIndex];
    for (size_t slotIndex = 0; slotIndex < slots.size(); ++slotIndex) {
      if (WindowConstraintFitsSlot(constraints[windowIndex], slots[slotIndex])) {
        compatibleSlots.push_back(slotIndex);
      }
    }
    if (preferNearbySlots) {
      std::stable_sort(
          compatibleSlots.begin(), compatibleSlots.end(),
          [&](size_t first, size_t second) {
            const size_t firstDistance = first > windowIndex
                ? first - windowIndex
                : windowIndex - first;
            const size_t secondDistance = second > windowIndex
                ? second - windowIndex
                : windowIndex - second;
            return firstDistance != secondDistance
                ? firstDistance < secondDistance
                : first < second;
          });
    }
  }
  return compatibility;
}

static bool TryAssignConstraintWindow(
    size_t candidateWindow,
    const ConstraintSlotCompatibility& compatibility,
    std::vector<size_t>* slotOwner, const std::vector<bool>& fixedWindows,
    std::vector<bool>* visitedSlots, bool preferFreeSlots) {
  if (!slotOwner || !visitedSlots ||
      candidateWindow >= compatibility.slotsByWindow.size()) {
    return false;
  }
  constexpr size_t kUnassigned = static_cast<size_t>(-1);
  const auto& compatibleSlots = compatibility.slotsByWindow[candidateWindow];

  if (preferFreeSlots) {
    for (size_t slotIndex : compatibleSlots) {
      if (slotIndex >= slotOwner->size() || (*visitedSlots)[slotIndex] ||
          (*slotOwner)[slotIndex] != kUnassigned) {
        continue;
      }
      (*visitedSlots)[slotIndex] = true;
      (*slotOwner)[slotIndex] = candidateWindow;
      return true;
    }
  }

  for (size_t slotIndex : compatibleSlots) {
    if (slotIndex >= slotOwner->size() || (*visitedSlots)[slotIndex]) continue;
    (*visitedSlots)[slotIndex] = true;
    const size_t displacedWindow = (*slotOwner)[slotIndex];
    if (displacedWindow == kUnassigned ||
        (displacedWindow < fixedWindows.size() &&
         !fixedWindows[displacedWindow] &&
         TryAssignConstraintWindow(
             displacedWindow, compatibility, slotOwner, fixedWindows,
             visitedSlots, preferFreeSlots))) {
      (*slotOwner)[slotIndex] = candidateWindow;
      return true;
    }
  }
  return false;
}

// Applies an already-decided active-slot permutation without changing membership.
// Suspended logical slots remain anchored; swapping active leaves changes only which
// active window occupies each currently available logical position.
static bool ApplyTiledWindowOrder(
    Workspace& workspace, const std::vector<HWND>& desiredOrder) {
  if (desiredOrder.size() != workspace.ActiveCount()) return false;
  for (size_t slotIndex = 0; slotIndex < desiredOrder.size(); ++slotIndex) {
    const HWND current = workspace.TiledWindows()[slotIndex];
    if (current != desiredOrder[slotIndex] &&
        !workspace.SwapTiled(current, desiredOrder[slotIndex])) {
      return false;
    }
  }
  return true;
}

// Layout conversion has different semantics from an explicit order mutation.
// Preserve the existing order first (the caller already tried that), but if the
// new Dwindle geometry cannot host that order, find a compatible permutation.
//
// Adaptive prefix splits are searched jointly with assignment: for a candidate
// occupant we allocate the smallest legal span that satisfies that window, which
// maximizes the recursive tail. With minimum-only constraints, any larger tail is
// never worse, so failure with this maximal tail proves that occupant cannot lead
// to a feasible continuation. Once the adaptive prefix ends, ordinary bipartite
// matching solves the remaining fixed-geometry tail in polynomial time.
static bool ReassignDwindleLayoutConversionForConstraints(
    Workspace& workspace, const RECT& workArea, LONG gap,
    size_t* outMovedWindowCount) {
  if (workspace.Layout() != TileLayout::Dwindle || workspace.ActiveCount() < 2) {
    return false;
  }

  const std::vector<HWND> windows = workspace.TiledWindows();
  const std::vector<WindowConstraints> constraints =
      ActiveWindowConstraints(workspace);
  const std::vector<double> ratios = workspace.DwindleRatios();
  const size_t count = windows.size();
  if (constraints.size() != count || ratios.size() + 1 < count) return false;

  const size_t adaptiveSplitCount = AutomaticDwindleAdaptationEnabled()
      ? std::min(
            g_settings.adaptiveDwindleSlots,
            count > 0 ? count - 1 : size_t{0})
      : 0;
  const AdaptiveDwindleRatioEnvelope adaptiveEnvelope =
      GetAdaptiveDwindleRatioEnvelope();
  std::vector<bool> used(count, false);
  std::vector<HWND> desiredOrder(count, nullptr);
  uint64_t usedMask = 0;
  size_t searchNodes = 0;
  bool searchLimitReached = false;
  constexpr size_t kMaxSearchNodes = 250000;

  struct SearchKey {
    uint64_t usedMask = 0;
    uint32_t slotIndex = 0;
    LONG width = 0;
    LONG height = 0;
    bool operator==(const SearchKey& other) const noexcept {
      return usedMask == other.usedMask && slotIndex == other.slotIndex &&
          width == other.width && height == other.height;
    }
  };
  struct SearchKeyHash {
    size_t operator()(const SearchKey& key) const noexcept {
      size_t value = std::hash<uint64_t>{}(key.usedMask);
      value ^= std::hash<uint32_t>{}(key.slotIndex) +
          0x9e3779b9u + (value << 6) + (value >> 2);
      value ^= std::hash<LONG>{}(key.width) +
          0x9e3779b9u + (value << 6) + (value >> 2);
      value ^= std::hash<LONG>{}(key.height) +
          0x9e3779b9u + (value << 6) + (value >> 2);
      return value;
    }
  };
  std::unordered_set<SearchKey, SearchKeyHash> deadStates;
  const bool canMemoize = count <= 64;

  auto makeSplit = [&](const RECT& area, size_t slotIndex, LONG firstSpan,
                       RECT* first, RECT* remaining) {
    if (!first || !remaining) return false;
    const bool vertical = (slotIndex % 2) == 0;
    const LONG span = vertical ? RectWidth(area) : RectHeight(area);
    if (span < 2) return false;
    const LONG effectiveGap = std::clamp<LONG>(gap, 0, span - 2);
    const LONG usable = span - effectiveGap;
    if (firstSpan < 1 || firstSpan >= usable) return false;

    *remaining = area;
    if (vertical) {
      if (g_settings.reverseDwindle) {
        *first = {area.right - firstSpan, area.top, area.right, area.bottom};
        remaining->right = first->left - effectiveGap;
      } else {
        *first = {area.left, area.top, area.left + firstSpan, area.bottom};
        remaining->left = first->right + effectiveGap;
      }
    } else {
      if (g_settings.reverseDwindle) {
        *first = {area.left, area.bottom - firstSpan, area.right, area.bottom};
        remaining->bottom = first->top - effectiveGap;
      } else {
        *first = {area.left, area.top, area.right, area.top + firstSpan};
        remaining->top = first->bottom + effectiveGap;
      }
    }
    return RectWidth(*remaining) > 0 && RectHeight(*remaining) > 0;
  };

  auto matchFixedTail = [&](size_t slotIndex, const RECT& area) {
    const size_t tailCount = count - slotIndex;
    if (tailCount == 0) return true;

    std::vector<size_t> remainingWindows;
    remainingWindows.reserve(tailCount);
    for (size_t windowIndex = 0; windowIndex < count; ++windowIndex) {
      if (!used[windowIndex]) remainingWindows.push_back(windowIndex);
    }
    if (remainingWindows.size() != tailCount) return false;

    std::vector<RECT> allSlots(count);
    Layout::LayoutDwindle(
        area, gap, slotIndex, tailCount, ratios,
        g_settings.reverseDwindle, allSlots);
    std::vector<RECT> tailSlots(
        allSlots.begin() + slotIndex, allSlots.end());
    std::vector<WindowConstraints> tailConstraints;
    tailConstraints.reserve(tailCount);
    for (size_t windowIndex : remainingWindows) {
      tailConstraints.push_back(constraints[windowIndex]);
    }
    const ConstraintSlotCompatibility compatibility =
        BuildConstraintSlotCompatibility(tailConstraints, tailSlots, true);
    for (const auto& compatibleSlots : compatibility.slotsByWindow) {
      if (compatibleSlots.empty()) return false;
    }

    constexpr size_t kUnassigned = static_cast<size_t>(-1);
    std::vector<size_t> slotOwner(tailCount, kUnassigned);
    std::vector<bool> fixed(tailCount, false);

    // Seed surviving exact positions first so a layout conversion disturbs as
    // little order as possible before augmenting paths are allowed to move them.
    for (size_t localWindow = 0; localWindow < tailCount; ++localWindow) {
      const size_t originalIndex = remainingWindows[localWindow];
      if (originalIndex < slotIndex) continue;
      const size_t localSlot = originalIndex - slotIndex;
      if (localSlot >= tailCount || slotOwner[localSlot] != kUnassigned) continue;
      const auto& compatibleSlots = compatibility.slotsByWindow[localWindow];
      if (std::find(
              compatibleSlots.begin(), compatibleSlots.end(), localSlot) !=
          compatibleSlots.end()) {
        slotOwner[localSlot] = localWindow;
      }
    }

    std::vector<size_t> assignmentOrder(tailCount);
    for (size_t i = 0; i < tailCount; ++i) assignmentOrder[i] = i;
    std::stable_sort(
        assignmentOrder.begin(), assignmentOrder.end(),
        [&](size_t first, size_t second) {
          const size_t firstChoices = compatibility.slotsByWindow[first].size();
          const size_t secondChoices = compatibility.slotsByWindow[second].size();
          return firstChoices != secondChoices
              ? firstChoices < secondChoices
              : remainingWindows[first] < remainingWindows[second];
        });

    for (size_t localWindow : assignmentOrder) {
      if (std::find(slotOwner.begin(), slotOwner.end(), localWindow) !=
          slotOwner.end()) {
        continue;
      }
      std::vector<bool> visitedSlots(tailCount, false);
      if (!TryAssignConstraintWindow(
              localWindow, compatibility, &slotOwner, fixed,
              &visitedSlots, true)) {
        return false;
      }
    }

    for (size_t localSlot = 0; localSlot < tailCount; ++localSlot) {
      if (slotOwner[localSlot] == kUnassigned) return false;
      desiredOrder[slotIndex + localSlot] =
          windows[remainingWindows[slotOwner[localSlot]]];
    }
    return true;
  };

  auto search = [&](auto&& self, size_t slotIndex, const RECT& area) -> bool {
    if (++searchNodes > kMaxSearchNodes) {
      searchLimitReached = true;
      return false;
    }
    if (slotIndex >= count) return true;

    SearchKey key{};
    if (canMemoize) {
      key = {usedMask, static_cast<uint32_t>(slotIndex),
             RectWidth(area), RectHeight(area)};
      if (deadStates.find(key) != deadStates.end()) return false;
    }

    if (slotIndex >= adaptiveSplitCount || slotIndex + 1 >= count) {
      const bool matched = matchFixedTail(slotIndex, area);
      if (!matched && canMemoize) deadStates.insert(key);
      return matched;
    }

    const bool vertical = (slotIndex % 2) == 0;
    const LONG span = vertical ? RectWidth(area) : RectHeight(area);
    if (span < 2) {
      if (canMemoize) deadStates.insert(key);
      return false;
    }
    const LONG effectiveGap = std::clamp<LONG>(gap, 0, span - 2);
    const LONG usable = span - effectiveGap;
    if (usable < 2) {
      if (canMemoize) deadStates.insert(key);
      return false;
    }
    const LONG minimumAdaptive = std::clamp<LONG>(
        static_cast<LONG>(std::llround(
            usable * adaptiveEnvelope.minimum)), 1, usable - 1);
    const LONG maximumAdaptive = std::clamp<LONG>(
        static_cast<LONG>(std::llround(
            usable * adaptiveEnvelope.maximum)), 1, usable - 1);
    const double currentRatio = ClampDouble(ratios[slotIndex], 0.1, 0.9);
    const LONG currentSpan = std::clamp<LONG>(
        static_cast<LONG>(std::llround(usable * currentRatio)),
        1, usable - 1);

    std::vector<size_t> candidates;
    candidates.reserve(count - slotIndex);
    for (size_t windowIndex = 0; windowIndex < count; ++windowIndex) {
      if (!used[windowIndex]) candidates.push_back(windowIndex);
    }
    std::stable_sort(
        candidates.begin(), candidates.end(),
        [&](size_t first, size_t second) {
          const size_t firstDistance = first > slotIndex
              ? first - slotIndex : slotIndex - first;
          const size_t secondDistance = second > slotIndex
              ? second - slotIndex : slotIndex - second;
          return firstDistance != secondDistance
              ? firstDistance < secondDistance : first < second;
        });

    for (size_t windowIndex : candidates) {
      const WindowConstraints& constraint = constraints[windowIndex];
      const LONG primaryMinimum = constraint.valid
          ? std::max<LONG>(
                1, vertical ? constraint.minWidth : constraint.minHeight)
          : 1;
      LONG firstSpan = usable;
      if (currentSpan >= primaryMinimum) {
        // Keeping the current manual ratio is always allowed, even when it lies
        // outside the automatic envelope.
        firstSpan = currentSpan;
      }
      const LONG adaptiveSpan = std::max(primaryMinimum, minimumAdaptive);
      if (adaptiveSpan <= maximumAdaptive) {
        // The smallest permitted automatic span maximizes the recursive tail;
        // with minimum-only constraints this is never worse for continuation.
        firstSpan = std::min(firstSpan, adaptiveSpan);
      }
      if (firstSpan >= usable) continue;

      RECT first{};
      RECT remaining{};
      if (!makeSplit(area, slotIndex, firstSpan, &first, &remaining) ||
          !WindowConstraintFitsSlot(constraint, first)) {
        continue;
      }

      used[windowIndex] = true;
      if (canMemoize) usedMask |= (uint64_t{1} << windowIndex);
      desiredOrder[slotIndex] = windows[windowIndex];
      if (self(self, slotIndex + 1, remaining)) return true;
      desiredOrder[slotIndex] = nullptr;
      if (canMemoize) usedMask &= ~(uint64_t{1} << windowIndex);
      used[windowIndex] = false;
      if (searchLimitReached) return false;
    }

    if (canMemoize) deadStates.insert(key);
    return false;
  };

  RECT normalizedArea{0, 0, RectWidth(workArea), RectHeight(workArea)};
  if (!search(search, 0, normalizedArea)) {
    if (searchLimitReached) {
      Diagnostics::RecordEvent(
          L"dwindle layout conversion search limit reached windows=%zu adaptive=%zu nodes=%zu",
          count, adaptiveSplitCount, searchNodes);
    }
    return false;
  }

  Workspace trial = workspace;
  if (!ApplyTiledWindowOrder(trial, desiredOrder)) return false;
  if (adaptiveSplitCount > 0) {
    if (!FitAdaptiveDwindleDividers(trial, workArea, gap)) return false;
  } else {
    std::vector<RECT> rects;
    ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
    if (BuildWorkspaceLayoutPlan(
            trial, workArea, gap, &rects) != LayoutPlanStatus::Success) {
      return false;
    }
  }

  size_t movedWindowCount = 0;
  for (size_t i = 0; i < count; ++i) {
    if (desiredOrder[i] != windows[i]) ++movedWindowCount;
  }
  if (outMovedWindowCount) *outMovedWindowCount = movedWindowCount;
  workspace = std::move(trial);
  return true;
}

static void ShowWindowConstraintConflict(
    const Workspace& workspace, const std::vector<RECT>& rects,
    HWND fallback, HMONITOR monitor) {
  const auto constraints = ActiveWindowConstraints(workspace);
  const ConstraintSlotCompatibility compatibility =
      BuildConstraintSlotCompatibility(constraints, rects, false);
  std::vector<bool> involved(constraints.size(), false);
  for (size_t i = 0; i < constraints.size(); ++i) {
    involved[i] = i < rects.size() &&
        !WindowConstraintFitsSlot(constraints[i], rects[i]);
  }
  // A failed matching identifies groups competing for too few slots. Follow
  // alternating paths from unmatched windows to include the other blockers,
  // even when they currently fit (e.g. Pixeval already occupies the only slot).
  if (workspace.Layout() == TileLayout::Dwindle &&
      rects.size() == constraints.size()) {
    constexpr size_t kUnassigned = static_cast<size_t>(-1);
    std::vector<size_t> owner(rects.size(), kUnassigned);
    std::vector<bool> fixed(constraints.size(), false);
    std::vector<size_t> unmatched;
    for (size_t window = 0; window < constraints.size(); ++window) {
      std::vector<bool> visited(rects.size(), false);
      if (!TryAssignConstraintWindow(
              window, compatibility, &owner, fixed, &visited, false)) {
        unmatched.push_back(window);
      }
    }
    if (!unmatched.empty()) {
      involved.assign(constraints.size(), false);
      auto include = [&](auto&& self, size_t member) -> void {
        if (member >= involved.size() || involved[member]) return;
        involved[member] = true;
        for (size_t slot : compatibility.slotsByWindow[member]) {
          if (slot < owner.size() && owner[slot] != kUnassigned) {
            self(self, owner[slot]);
          }
        }
      };
      for (size_t window : unmatched) include(include, window);
    }
  }
  std::vector<std::wstring> names;
  for (size_t i = 0; i < involved.size(); ++i) {
    if (!involved[i]) continue;
    std::wstring name = GetWindowProcessName(workspace.TiledWindows()[i]);
    if (name.size() > 4 && _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0)
      name.resize(name.size() - 4);
    if (name.empty()) name = L"A window";
    if (std::find(names.begin(), names.end(), name) == names.end())
      names.push_back(std::move(name));
  }
  if (names.empty() && fallback) {
    std::wstring name = GetWindowProcessName(fallback);
    if (name.size() > 4 && _wcsicmp(name.c_str() + name.size() - 4, L".exe") == 0)
      name.resize(name.size() - 4);
    if (!name.empty()) names.push_back(std::move(name));
  }
  std::wstring message;
  for (size_t i = 0; i < names.size(); ++i) {
    if (i) message += i + 1 == names.size() ? L" and " : L", ";
    message += names[i];
  }
  if (names.empty()) message = L"A window";
  message += names.size() > 1
      ? L" need more space in this layout" : L" needs more space";
  TrayUi::ShowCommandFailureFlyout(message.c_str(), monitor);
}

static void ReportRejectedLayoutMutation(
    const DesktopMonitorKey& key, HMONITOR monitor,
    UserLayoutMutationKind kind, const wchar_t* action, HWND subject,
    const Workspace& rejectedWorkspace, const std::vector<RECT>& rejectedRects,
    HWND culpritHint = nullptr) {
  switch (kind) {
    case UserLayoutMutationKind::Resize:
      ++Diagnostics::g_runtime.counters.rejectedResizeGestures;
      break;
    case UserLayoutMutationKind::Order:
      ++Diagnostics::g_runtime.counters.rejectedOrderCommands;
      break;
    case UserLayoutMutationKind::LayoutChange:
      ++Diagnostics::g_runtime.counters.rejectedLayoutChanges;
      break;
  }

  HWND culprit = FindConstraintCulprit(
      rejectedWorkspace, rejectedRects, subject);
  if (!culprit) culprit = culpritHint;
  ShowWindowConstraintConflict(
      rejectedWorkspace, rejectedRects, culprit, monitor);
  Wh_Log(
      L"%ls rejected by layout constraints subject=%p culprit=%p",
      action, subject, culprit);
  Diagnostics::RecordEvent(
      L"layout mutation rejected action=%ls subject=%p culprit=%p desktop=%08X",
      action, subject, culprit, key.desktopId.Data1);
}

static bool TryRollbackPendingLayoutMutation(
    const DesktopMonitorKey& key, HMONITOR monitor, Workspace* workspace,
    const std::vector<RECT>& rejectedRects, HWND culpritHint = nullptr) {
  if (!workspace) return false;
  auto mutationIt = g_pendingLayoutMutations.find(key);
  if (mutationIt == g_pendingLayoutMutations.end()) return false;
  // Constraint learning and admission recovery are allowed to adjust ratios or
  // reorder pending admissions downstream of the user's command. Those are part
  // of the same delayed transaction, not evidence that the transaction is stale.
  // Only a membership/participation change makes the saved rollback snapshot
  // unsafe to apply.
  if (!workspace->HasSameWindowMembershipAs(mutationIt->second.candidate)) {
    Diagnostics::RecordEvent(
        L"discarded stale layout mutation rollback after membership change action=%ls desktop=%08X",
        mutationIt->second.action, key.desktopId.Data1);
    g_pendingLayoutMutations.erase(mutationIt);
    return false;
  }

  PendingLayoutMutation mutation = mutationIt->second;
  Workspace restored = *workspace;
  if (!restored.RestoreLayoutStateFrom(mutation.previous)) {
    g_pendingLayoutMutations.erase(mutationIt);
    return false;
  }

  WorkspaceMetrics rollbackMetrics;
  if (!GetWorkspaceMetrics(monitor, &rollbackMetrics)) {
    g_pendingLayoutMutations.erase(mutationIt);
    return false;
  }
  std::vector<RECT> restoredRects;
  ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
  const LayoutPlanStatus restoredStatus = BuildWorkspaceLayoutPlan(
      restored, rollbackMetrics.workArea, rollbackMetrics.gap, &restoredRects);
  if (restoredStatus != LayoutPlanStatus::Success &&
      restoredStatus != LayoutPlanStatus::NotApplicable) {
    Diagnostics::RecordEvent(
        L"layout mutation rollback no longer feasible action=%ls desktop=%08X",
        mutation.action, key.desktopId.Data1);
    g_pendingLayoutMutations.erase(mutationIt);
    return false;
  }

  for (HWND hwnd : workspace->TiledWindows()) CancelConformanceLease(hwnd);
  if (restored.Layout() == TileLayout::Floating) {
    RestoreWorkspaceFloatingGeometry(key, restored);
  }
  ReportRejectedLayoutMutation(
      key, monitor, mutation.kind, mutation.action, mutation.subject,
      *workspace, rejectedRects, culpritHint);
  g_workspaces.Save(key, restored);
  *workspace = std::move(restored);
  g_pendingLayoutMutations.erase(mutationIt);
  TrayUi::UpdateIcon(workspace->Layout(), monitor);
  return true;
}

static bool HasOutstandingConformanceLease(const Workspace& workspace) {
  for (HWND hwnd : workspace.TiledWindows()) {
    if (ReadConformanceLease(hwnd, nullptr) !=
        ConformanceLeaseReadResult::Missing) {
      return true;
    }
  }
  return false;
}

static void ClearSettledLayoutMutation(
    const DesktopMonitorKey& key, const Workspace& workspace) {
  auto mutationIt = g_pendingLayoutMutations.find(key);
  if (mutationIt == g_pendingLayoutMutations.end()) return;
  if (!workspace.HasSameWindowMembershipAs(mutationIt->second.candidate)) {
    Diagnostics::RecordEvent(
        L"discarded stale layout mutation settle after membership change action=%ls desktop=%08X",
        mutationIt->second.action, key.desktopId.Data1);
    g_pendingLayoutMutations.erase(mutationIt);
    return;
  }
  if (HasOutstandingConformanceLease(workspace)) return;
  Diagnostics::RecordEvent(
      L"layout mutation settled action=%ls desktop=%08X",
      mutationIt->second.action, key.desktopId.Data1);
  g_pendingLayoutMutations.erase(mutationIt);
}


// Dwindle geometry belongs to slots, not applications. During admission only,
// assign every active window to a compatible existing slot before concluding
// that one must float. The new window stays in the candidate slot, and an
// explicit user-mutation subject stays put.
static bool ReassignPendingDwindleAdmissionForConstraints(
    Workspace& workspace, const std::vector<RECT>& slots,
    HWND pendingAdmission, HWND fixedWindow, size_t* outMovedWindowCount) {
  if (workspace.Layout() != TileLayout::Dwindle ||
      slots.size() != workspace.ActiveCount()) {
    return false;
  }

  const WindowRecord* pendingRecord = workspace.Find(pendingAdmission);
  const size_t pendingWindowIndex = workspace.TiledIndex(pendingAdmission);
  if (!pendingRecord || !pendingRecord->pendingInsertionPlacement ||
      pendingWindowIndex >= workspace.ActiveCount()) {
    return false;
  }

  const std::vector<HWND> windows = workspace.TiledWindows();
  const std::vector<WindowConstraints> constraints =
      ActiveWindowConstraints(workspace);
  const size_t count = windows.size();
  const ConstraintSlotCompatibility compatibility =
      BuildConstraintSlotCompatibility(constraints, slots, true);
  for (const auto& compatibleSlots : compatibility.slotsByWindow) {
    if (compatibleSlots.empty()) return false;
  }

  constexpr size_t kUnassigned = static_cast<size_t>(-1);
  std::vector<size_t> slotOwner(count, kUnassigned);
  std::vector<bool> fixed(count, false);
  auto pinWindow = [&](size_t windowIndex, size_t slotIndex) {
    if (windowIndex >= count || slotIndex >= count ||
        slotOwner[slotIndex] != kUnassigned) {
      return false;
    }
    const auto& compatibleSlots = compatibility.slotsByWindow[windowIndex];
    if (std::find(
            compatibleSlots.begin(), compatibleSlots.end(), slotIndex) ==
        compatibleSlots.end()) {
      return false;
    }
    slotOwner[slotIndex] = windowIndex;
    fixed[windowIndex] = true;
    return true;
  };

  if (!pinWindow(pendingWindowIndex, pendingWindowIndex)) return false;
  const size_t fixedWindowIndex = workspace.TiledIndex(fixedWindow);
  if (fixedWindow && fixedWindow != pendingAdmission &&
      !pinWindow(fixedWindowIndex, fixedWindowIndex)) {
    return false;
  }

  // Seed every compatible current placement. Starting with empty slots allowed
  // later flexible windows to evict earlier ones and shuffle the whole order.
  for (size_t i = 0; i < count; ++i) {
    const auto& compatibleSlots = compatibility.slotsByWindow[i];
    if (!fixed[i] && slotOwner[i] == kUnassigned &&
        std::find(compatibleSlots.begin(), compatibleSlots.end(), i) !=
            compatibleSlots.end()) {
      slotOwner[i] = i;
    }
  }
  std::vector<size_t> assignmentOrder;
  assignmentOrder.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    if (!fixed[i]) assignmentOrder.push_back(i);
  }
  std::stable_sort(
      assignmentOrder.begin(), assignmentOrder.end(),
      [&](size_t first, size_t second) {
        const size_t firstChoices = compatibility.slotsByWindow[first].size();
        const size_t secondChoices = compatibility.slotsByWindow[second].size();
        return firstChoices != secondChoices
            ? firstChoices < secondChoices
            : first < second;
      });

  for (size_t windowIndex : assignmentOrder) {
    if (std::find(slotOwner.begin(), slotOwner.end(), windowIndex) != slotOwner.end())
      continue;
    std::vector<bool> visitedSlots(count, false);
    if (!TryAssignConstraintWindow(
            windowIndex, compatibility, &slotOwner, fixed,
            &visitedSlots, true)) {
      return false;
    }
  }

  std::vector<HWND> desiredOrder(count);
  size_t movedWindowCount = 0;
  for (size_t slotIndex = 0; slotIndex < count; ++slotIndex) {
    if (slotOwner[slotIndex] == kUnassigned) return false;
    desiredOrder[slotIndex] = windows[slotOwner[slotIndex]];
    if (desiredOrder[slotIndex] != windows[slotIndex]) ++movedWindowCount;
  }
  if (!ApplyTiledWindowOrder(workspace, desiredOrder)) return false;
  if (outMovedWindowCount) *outMovedWindowCount = movedWindowCount;
  return movedWindowCount > 0;
}

static bool FloatWindowAfterFailure(
    const DesktopMonitorKey& key, Workspace& workspace, HWND hwnd) {
  if (!workspace.Float(hwnd)) return false;
  FloatingPlacementHint hint;
  hint.intent = FloatingPlacementIntent::FailureCenter;
  if (!RepairFloatingGeometry(key, workspace, hwnd, hint)) {
    Diagnostics::RecordEvent(
        L"failure float geometry reset failed hwnd=%p desktop=%08X",
        hwnd, key.desktopId.Data1);
  }
  return true;
}


Workspace::PlacementAction Workspace::ApplyPlacementObservation(
    HWND hwnd, const PlacementObservation& observation) {
  AssertWmThread(L"Workspace::ApplyPlacementObservation");
  WindowRecord* record = FindMutable(hwnd);
  if (!record) return PlacementAction::Forget;

  record->lastRequestedRect = observation.requested;
  record->lastObservedRect = observation.observed;
  record->lastPlacementResult = observation.result;
  DebugValidateMutation(L"Workspace::ApplyPlacementObservation");

  switch (observation.result) {
    case PlacementResult::Success:
    case PlacementResult::SuppressedByPhysicalState:
    case PlacementResult::AdjustedByWindow:
    case PlacementResult::Refused:
      return PlacementAction::None;

    case PlacementResult::AccessDenied:
      return PlacementAction::Float;

    case PlacementResult::Dead:
      return PlacementAction::Forget;
  }
  return PlacementAction::None;
}

static size_t AutomaticWindowLimitForMonitor(const Model::MonitorId& monitor) {
  MONITORINFO monitorInfo{sizeof(monitorInfo)};
  bool monitorInfoRead = false;
  bool haveMonitorInfo = false;
  for (const auto& rule : g_settings.monitorWindowLimits) {
    if (rule.monitor == RuleMonitor::Specific) {
      if (rule.monitorId == monitor.deviceId) return rule.limit;
      continue;
    }
    if (!monitorInfoRead) {
      haveMonitorInfo = GetMonitorInfoW(monitor.Resolve(), &monitorInfo);
      monitorInfoRead = true;
    }
    if (haveMonitorInfo && ((monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0) ==
                              (rule.monitor == RuleMonitor::Primary)) return rule.limit;
  }
  return g_settings.automaticWindowLimit;
}

// Arrangement releases the slot first; physical routing runs after every source
// workspace in the burst has been saved, so discovery snapshots cannot go stale
// midway through admission. A failed route therefore already has a safe fallback.
static bool FloatAutomaticOverflowWindow(
    const DesktopMonitorKey& key, Workspace& workspace, HWND hwnd,
    const wchar_t* reason) {
  const WindowRecord* record = workspace.Find(hwnd);
  if (!record) return false;
  const DWORD processId = record->pid;
  const bool monitorFixed = record->automaticOverflowMonitorFixed;
  const uint64_t generation = record->tilingGeneration;
  const bool route = IsAutomaticMode() && g_hooks.hideDestroy &&
      (g_settings.overflowCreateDesktop || g_settings.overflowTryOtherMonitors);
  CancelConformanceLease(hwnd);
  DiscardPendingAdmissionRecovery(hwnd);
  if (!(route ? workspace.Float(hwnd) : FloatWindowAfterFailure(key, workspace, hwnd))) return false;
  if (route) {
    g_wm.overflowWindows.insert(hwnd);
    g_wm.pendingWindowOverflow.push_back(
        {hwnd, processId, key.desktopId, key.monitor.deviceId, monitorFixed, generation});
    Reconcile::ScheduleLifecycleReconcile(nullptr);
  }
  Diagnostics::RecordEvent(L"automatic overflow hwnd=%p reason=%ls route=%d", hwnd, reason, route ? 1 : 0);
  return true;
}

// Renders one workspace from authoritative model state. Arrangement itself is a
// small pipeline: repair local invariants -> compute targets -> apply placement ->
// fold placement outcomes back into the model. Reflow is iterative: a pass may
// refresh a native constraint or select one constraint-removal candidate.
static void ArrangeWorkspace(
    const DesktopMonitorKey& key, HWND preferredTiledWindow) {
  ++Diagnostics::g_runtime.counters.arrangeCalls;
  if (!IsWorkspaceOnActiveDesktop(key)) return;

  Workspace workspace;
  if (!g_workspaces.Load(key, &workspace)) return;

  HMONITOR monitor = key.ResolveMonitor();
  if (!monitor) return;  // Detached monitor: keep workspace state dormant.

  WorkspaceMetrics metrics;
  if (!GetWorkspaceMetrics(monitor, &metrics)) return;
  const RECT& workArea = metrics.workArea;
  const LONG gap = metrics.gap;

  ObserveAndRepairWorkspaceForArrangement(workspace);
  if (IsAutomaticMode() && workspace.Layout() != TileLayout::Floating) {
    const size_t limit = AutomaticWindowLimitForMonitor(key.monitor);
    if (limit) {
      struct VisibleMember {
        HWND hwnd;
        DWORD processId;
        uint64_t generation;
        bool awaitingInitialPlacement;
      };
      std::vector<VisibleMember> visibleMembers;
      size_t retainedSlots = 0;
      for (const auto& member : workspace.Records()) {
        const WindowRecord& record = member.second;
        if (record.state == ManageState::Tiled ||
            (record.state == ManageState::Suspended && record.hasSavedSlot &&
             record.suspensionReason == SuspensionReason::Maximized)) {
          const SuspensionReason physicalReason = GetPhysicalSuspensionReason(record.hwnd);
          if (physicalReason == SuspensionReason::Minimized ||
              physicalReason == SuspensionReason::Hidden || IsWindowCloaked(record.hwnd)) continue;
          visibleMembers.push_back({record.hwnd, record.pid, record.tilingGeneration, false});
        }
      }
      // Copy pending identities before desktop queries, which can deliver lifecycle events.
      for (const auto& pending : g_wm.pendingWindowRoutes) {
        const auto& request = pending.second;
        if (request.visibleGeneration && IsEqualGUID(request.sourceDesktop, key.desktopId)) {
          visibleMembers.push_back({pending.first, request.processId, request.visibleGeneration, true});
        }
      }
      std::sort(visibleMembers.begin(), visibleMembers.end(),
                [](const VisibleMember& first, const VisibleMember& second) {
                  return first.generation < second.generation;
                });
      for (const VisibleMember& member : visibleMembers) {
        if (member.awaitingInitialPlacement) {
          GUID owner{};
          if (!GetWindowDesktopIdSafe(member.hwnd, &owner) || !IsEqualGUID(owner, key.desktopId)) continue;
          DWORD liveProcessId = 0;
          GetWindowThreadProcessId(member.hwnd, &liveProcessId);
          if (!IsWindow(member.hwnd) || liveProcessId != member.processId ||
              IsWindowTrackedInAnyState(member.hwnd) || IsWindowCloaked(member.hwnd) ||
              GetPhysicalSuspensionReason(member.hwnd) != SuspensionReason::Maximized ||
              GetWindowPhysicalMonitor(member.hwnd) != monitor || ShouldWindowStartFloating(member.hwnd)) continue;
          const auto pending = g_wm.pendingWindowRoutes.find(member.hwnd);
          if (pending == g_wm.pendingWindowRoutes.end() || pending->second.processId != member.processId ||
              pending->second.visibleGeneration != member.generation) continue;
          // Reserve its place in arrival order. Restoration still performs initial placement.
          ++retainedSlots;
          continue;
        }
        const bool exceedsLimit = retainedSlots >= limit;
        ++retainedSlots;
        // Visible maximized members count, but are not restored just to move them.
        if (exceedsLimit && !IsMoveSizeGestureInProgress(member.hwnd) &&
            FloatAutomaticOverflowWindow(key, workspace, member.hwnd, L"count limit")) {
          --retainedSlots;
        }
      }
    }
  }
  auto pendingMutationIt = g_pendingLayoutMutations.find(key);
  if (pendingMutationIt != g_pendingLayoutMutations.end() &&
      !workspace.HasSameWindowMembershipAs(
          pendingMutationIt->second.candidate)) {
    Diagnostics::RecordEvent(
        L"discarded stale layout mutation before arrangement after membership change action=%ls desktop=%08X",
        pendingMutationIt->second.action, key.desktopId.Data1);
    g_pendingLayoutMutations.erase(pendingMutationIt);
  }
  // Native reports are cached between arrangements. The first pass below probes
  // missing/stale reports and pending admissions/restores before geometry is solved.
  const size_t maxPasses = workspace.ActiveCount() * 5 + 8;
  bool constraintFailureReported = false;
  for (size_t pass = 0; pass < maxPasses; ++pass) {
    ++Diagnostics::g_runtime.counters.arrangePasses;
    if (workspace.Layout() == TileLayout::Floating || workspace.Empty()) {
      if (workspace.Layout() == TileLayout::Floating) {
        for (HWND hwnd : workspace.TiledWindows()) {
          CancelConformanceLease(hwnd);
          workspace.ClearInsertionPlacement(hwnd);
        }
      }
      g_workspaces.Save(key, workspace);
      ClearSettledLayoutMutation(key, workspace);
      return;
    }

    if (pass == 0) {
      RefreshWorkspaceNativeConstraints(workspace, false, L"arrange");
    }

    std::vector<RECT> rects;
    ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
    LayoutPlanStatus planStatus =
        BuildWorkspaceLayoutPlan(workspace, workArea, gap, &rects);
    if (planStatus == LayoutPlanStatus::NotApplicable) {
      g_workspaces.Save(key, workspace);
      return;
    }
    if (planStatus == LayoutPlanStatus::Unsatisfiable) {
      ++Diagnostics::g_runtime.counters.constraintUnsatisfiablePlans;
      // An explicit order mutation has already passed a full native-constraint
      // preflight before commit. If it later becomes unsatisfiable, do not
      // reinterpret the command by reshaping/reassigning it again here. The
      // delayed transaction gets first refusal: restore the previous order and
      // report the same constraint toast. Only when that saved order is no
      // longer feasible either may ordinary global constraint recovery proceed.
      if (TryRollbackPendingLayoutMutation(
              key, monitor, &workspace, rects)) {
        continue;
      }

      // A settled Dwindle workspace should preserve membership and order when a
      // DPI/work-area/native-minimum change makes its current ratios infeasible.
      // Before considering any eviction, clamp only the configured adaptive
      // divider prefix to the nearest feasible geometry. Pending admissions have
      // their own transactional recovery below and must not use this settled path.
      bool hasPendingAdmission = false;
      for (HWND hwnd : workspace.TiledWindows()) {
        const WindowRecord* record = workspace.Find(hwnd);
        if (record && record->pendingInsertionPlacement) {
          hasPendingAdmission = true;
          break;
        }
      }
      if (!hasPendingAdmission &&
          workspace.Layout() == TileLayout::Dwindle &&
          AutomaticDwindleAdaptationEnabled()) {
        Workspace adaptive = workspace;
        const std::vector<double> beforeRatios = adaptive.DwindleRatios();
        if (FitAdaptiveDwindleDividers(adaptive, workArea, gap)) {
          const auto& afterRatios = adaptive.DwindleRatios();
          const size_t ratioCount =
              std::min(beforeRatios.size(), afterRatios.size());
          size_t changedRatios = 0;
          for (size_t splitIndex = 0; splitIndex < ratioCount; ++splitIndex) {
            if (std::abs(beforeRatios[splitIndex] -
                         afterRatios[splitIndex]) <= 1e-9) {
              continue;
            }
            ++changedRatios;
            Diagnostics::RecordEvent(
                L"settled dwindle adaptive ratio changed split=%zu from=%.6f to=%.6f desktop=%08X",
                splitIndex, beforeRatios[splitIndex], afterRatios[splitIndex],
                key.desktopId.Data1);
          }

          // BuildWorkspaceLayoutPlan was unsatisfiable immediately above, so a
          // successful fitter should necessarily alter geometry. Guard anyway to
          // prevent a pathological no-op from spinning the arrangement loop.
          if (changedRatios > 0) {
            for (HWND hwnd : workspace.TiledWindows()) {
              CancelConformanceLease(hwnd);
            }
            workspace = std::move(adaptive);
            Diagnostics::RecordEvent(
                L"settled dwindle constraints recovered adaptively changes=%zu desktop=%08X",
                changedRatios, key.desktopId.Data1);
            g_workspaces.Save(key, workspace);
            continue;
          }
        }
      }

      // Keep the exact pre-recovery state. If a pending admission is moved or
      // adaptive geometry is changed below, that state remains provisional until
      // conformance settles.
      const Workspace pendingAdmissionRecoveryBaseline = workspace;

      HWND victim = nullptr;
      bool intrinsicallyImpossible = false;
      bool pendingAdmissionRejected = false;
      bool singleRemovalMakesCurrentOrderViable = false;

      // Removing another member cannot help a window whose authoritative native
      // minimum is larger than the entire usable workspace. Float that window
      // directly; reserve candidate testing for aggregate constraint conflicts.
      const LONG workspaceWidth = RectWidth(workArea);
      const LONG workspaceHeight = RectHeight(workArea);
      const std::vector<WindowConstraints> activeConstraints =
          ActiveWindowConstraints(workspace);
      uint64_t newestImpossibleGeneration = 0;
      for (size_t i = 0;
           i < workspace.TiledWindows().size() &&
           i < activeConstraints.size();
           ++i) {
        const WindowConstraints& constraint = activeConstraints[i];
        if (!constraint.valid) continue;

        const bool impossible =
            std::max<LONG>(1, constraint.minWidth) > workspaceWidth ||
            std::max<LONG>(1, constraint.minHeight) > workspaceHeight;
        if (!impossible) continue;

        const HWND candidate = workspace.TiledWindows()[i];
        if (IsAutomaticMode() && IsMoveSizeGestureInProgress(candidate)) continue;
        const WindowRecord* candidateRecord = workspace.Find(candidate);
        const uint64_t generation =
            candidateRecord ? candidateRecord->tilingGeneration : 0;
        if (!victim || generation >= newestImpossibleGeneration) {
          victim = candidate;
          newestImpossibleGeneration = generation;
        }
      }

      if (victim) {
        intrinsicallyImpossible = true;
      } else {
        struct ConstraintCandidate {
          HWND hwnd;
          uint64_t generation;
        };

        std::vector<ConstraintCandidate> candidates;
        candidates.reserve(workspace.ActiveCount());
        for (HWND candidate : workspace.TiledWindows()) {
          if (IsAutomaticMode() && IsMoveSizeGestureInProgress(candidate)) continue;
          const WindowRecord* candidateRecord = workspace.Find(candidate);
          candidates.push_back({
              candidate,
              candidateRecord ? candidateRecord->tilingGeneration : 0});
        }
        std::stable_sort(
            candidates.begin(), candidates.end(),
            [&](const ConstraintCandidate& first,
                const ConstraintCandidate& second) {
              const bool firstPreferred = first.hwnd == preferredTiledWindow;
              const bool secondPreferred = second.hwnd == preferredTiledWindow;
              if (firstPreferred != secondPreferred) return !firstPreferred;
              return first.generation > second.generation;
            });

        // A fresh admission's configured position is a preference, not a reason
        // to discard it. Once placement has proven a conflict, preserve it when
        // adaptive Dwindle ratios can make it viable, then try nearby slots before
        // allowing broader sibling reassignment.
        const TileLayout layout = workspace.Layout();
        const bool searchAlternateInsertion =
            layout == TileLayout::MasterStack ||
            layout == TileLayout::MasterStackH ||
            layout == TileLayout::Columns || layout == TileLayout::Rows ||
            layout == TileLayout::Dwindle;
        const size_t preferredTiledIndex =
            workspace.TiledIndex(preferredTiledWindow);
        Workspace viableInsertion;
        HWND repositionedAdmission = nullptr;
        size_t previousInsertionIndex = 0;
        size_t viableInsertionIndex = 0;
        bool matchedDwindleConstraints = false;
        size_t reassignedDwindleWindows = 0;
        struct DwindleRatioAdjustment {
          size_t splitIndex;
          double before;
          double after;
        };
        std::vector<DwindleRatioAdjustment> adjustedDwindleRatios;

        auto rememberDwindleRatioAdjustments = [&](const std::vector<double>& before,
                                                   const Workspace& after) {
          adjustedDwindleRatios.clear();
          const auto& afterRatios = after.DwindleRatios();
          const size_t count = std::min(before.size(), afterRatios.size());
          for (size_t splitIndex = 0; splitIndex < count; ++splitIndex) {
            if (std::abs(before[splitIndex] - afterRatios[splitIndex]) <= 1e-9) {
              continue;
            }
            adjustedDwindleRatios.push_back(
                {splitIndex, before[splitIndex], afterRatios[splitIndex]});
          }
        };

        auto preservePreferredWindow = [&](const Workspace& trial) {
          return preferredTiledIndex >= workspace.ActiveCount() ||
              trial.TiledIndex(preferredTiledWindow) == preferredTiledIndex;
        };

        auto acceptInsertion = [&](Workspace&& trial, HWND hwnd,
                                   size_t currentIndex, size_t targetIndex,
                                   size_t movedWindowCount) {
          viableInsertion = std::move(trial);
          repositionedAdmission = hwnd;
          previousInsertionIndex = currentIndex;
          viableInsertionIndex = targetIndex;
          matchedDwindleConstraints = movedWindowCount > 0;
          reassignedDwindleWindows = movedWindowCount;
        };

        if (searchAlternateInsertion && layout == TileLayout::Dwindle) {
          // Adaptive Dwindle Slots controls which leading split ratios may move,
          // not where the pending window may be placed. Preserve its saved/current
          // slot first; then walk every nearby slot before reassigning siblings.
          for (const ConstraintCandidate& candidate : candidates) {
            const WindowRecord* candidateRecord = workspace.Find(candidate.hwnd);
            if (!candidateRecord ||
                !candidateRecord->pendingInsertionPlacement ||
                candidate.hwnd == preferredTiledWindow) {
              continue;
            }

            const size_t currentIndex = workspace.TiledIndex(candidate.hwnd);
            if (currentIndex >= workspace.ActiveCount()) continue;

            auto tryAdaptiveIndex = [&](size_t targetIndex,
                                        bool tryFixedGeometryFirst) {
              Workspace trial = workspace;
              if (targetIndex != currentIndex &&
                  !trial.MovePendingAdmissionToIndex(
                      candidate.hwnd, targetIndex)) {
                return false;
              }
              if (!preservePreferredWindow(trial)) return false;

              if (tryFixedGeometryFirst) {
                std::vector<RECT> trialRects;
                ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
                if (BuildWorkspaceLayoutPlan(
                        trial, workArea, gap, &trialRects) ==
                    LayoutPlanStatus::Success) {
                  adjustedDwindleRatios.clear();
                  acceptInsertion(
                      std::move(trial), candidate.hwnd, currentIndex,
                      targetIndex, 0);
                  return true;
                }
              }

              if (!AutomaticDwindleAdaptationEnabled()) return false;
              const std::vector<double> beforeRatios = trial.DwindleRatios();
              if (!FitAdaptiveDwindleDividers(trial, workArea, gap)) {
                return false;
              }
              rememberDwindleRatioAdjustments(beforeRatios, trial);
              acceptInsertion(
                  std::move(trial), candidate.hwnd, currentIndex,
                  targetIndex, 0);
              return true;
            };

            // The current assignment is already known to fail at fixed geometry,
            // so test whether changing only eligible ratios can preserve it.
            if (tryAdaptiveIndex(currentIndex, false)) break;

            for (size_t distance = 1;
                 distance < workspace.ActiveCount() && !repositionedAdmission;
                 ++distance) {
              if (currentIndex + distance < workspace.ActiveCount() &&
                  tryAdaptiveIndex(currentIndex + distance, true)) {
                break;
              }
              if (currentIndex >= distance &&
                  tryAdaptiveIndex(currentIndex - distance, true)) {
                break;
              }
            }
            if (repositionedAdmission) break;
          }

          // Only after preserving/moving the pending member fails do we allow
          // matching to move established siblings. Nearby target slots are tried
          // before matching around the current slot to minimize disruption.
          if (!repositionedAdmission) {
            for (const ConstraintCandidate& candidate : candidates) {
              const WindowRecord* candidateRecord = workspace.Find(candidate.hwnd);
              if (!candidateRecord ||
                  !candidateRecord->pendingInsertionPlacement ||
                  candidate.hwnd == preferredTiledWindow) {
                continue;
              }

              const size_t currentIndex = workspace.TiledIndex(candidate.hwnd);
              if (currentIndex >= workspace.ActiveCount()) continue;

              auto tryMatchedIndex = [&](size_t targetIndex) {
                Workspace trial = workspace;
                if (targetIndex != currentIndex &&
                    !trial.MovePendingAdmissionToIndex(
                        candidate.hwnd, targetIndex)) {
                  return false;
                }
                if (!preservePreferredWindow(trial)) return false;

                std::vector<RECT> trialRects;
                ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
                LayoutPlanStatus trialStatus = BuildWorkspaceLayoutPlan(
                    trial, workArea, gap, &trialRects);
                size_t movedWindowCount = 0;
                if (trialStatus != LayoutPlanStatus::Success) {
                  if (!ReassignPendingDwindleAdmissionForConstraints(
                          trial, trialRects, candidate.hwnd,
                          preferredTiledWindow, &movedWindowCount)) {
                    return false;
                  }
                  ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
                  trialStatus = BuildWorkspaceLayoutPlan(
                      trial, workArea, gap, &trialRects);
                  if (trialStatus != LayoutPlanStatus::Success) {
                    if (!AutomaticDwindleAdaptationEnabled()) return false;
                    const std::vector<double> beforeRatios =
                        trial.DwindleRatios();
                    if (!FitAdaptiveDwindleDividers(
                            trial, workArea, gap)) {
                      return false;
                    }
                    rememberDwindleRatioAdjustments(beforeRatios, trial);
                  } else {
                    adjustedDwindleRatios.clear();
                  }
                } else {
                  adjustedDwindleRatios.clear();
                }

                acceptInsertion(
                    std::move(trial), candidate.hwnd, currentIndex,
                    targetIndex, movedWindowCount);
                return true;
              };

              for (size_t distance = 1;
                   distance < workspace.ActiveCount() && !repositionedAdmission;
                   ++distance) {
                if (currentIndex + distance < workspace.ActiveCount() &&
                    tryMatchedIndex(currentIndex + distance)) {
                  break;
                }
                if (currentIndex >= distance &&
                    tryMatchedIndex(currentIndex - distance)) {
                  break;
                }
              }
              if (!repositionedAdmission && tryMatchedIndex(currentIndex)) {
                break;
              }
              if (repositionedAdmission) break;
            }
          }
        } else if (searchAlternateInsertion) {
          // Other layouts have no automatic divider adaptation or matching.
          for (const ConstraintCandidate& candidate : candidates) {
            const WindowRecord* candidateRecord = workspace.Find(candidate.hwnd);
            if (!candidateRecord ||
                !candidateRecord->pendingInsertionPlacement ||
                candidate.hwnd == preferredTiledWindow) {
              continue;
            }

            const size_t currentIndex = workspace.TiledIndex(candidate.hwnd);
            if (currentIndex >= workspace.ActiveCount()) continue;

            auto tryInsertionIndex = [&](size_t targetIndex) {
              Workspace trial = workspace;
              if (!trial.MovePendingAdmissionToIndex(
                      candidate.hwnd, targetIndex) ||
                  !preservePreferredWindow(trial)) {
                return false;
              }
              std::vector<RECT> trialRects;
              ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
              if (BuildWorkspaceLayoutPlan(
                      trial, workArea, gap, &trialRects) !=
                  LayoutPlanStatus::Success) {
                return false;
              }
              adjustedDwindleRatios.clear();
              acceptInsertion(
                  std::move(trial), candidate.hwnd, currentIndex,
                  targetIndex, 0);
              return true;
            };

            for (size_t distance = 1;
                 distance < workspace.ActiveCount() && !repositionedAdmission;
                 ++distance) {
              if (currentIndex + distance < workspace.ActiveCount() &&
                  tryInsertionIndex(currentIndex + distance)) {
                break;
              }
              if (currentIndex >= distance &&
                  tryInsertionIndex(currentIndex - distance)) {
                break;
              }
            }
            if (repositionedAdmission) break;
          }
        }

        if (repositionedAdmission) {
          RememberPendingAdmissionRecovery(
              key, repositionedAdmission, pendingAdmissionRecoveryBaseline);
          workspace = std::move(viableInsertion);
          if (matchedDwindleConstraints) {
            Wh_Log(
                L"Dwindle admission reassigned %zu windows across compatible slots for HWND %p",
                reassignedDwindleWindows,
                static_cast<void*>(repositionedAdmission));
            Diagnostics::RecordEvent(
                L"dwindle admission matched all constraints hwnd=%p slot=%zu reassigned=%zu",
                repositionedAdmission, viableInsertionIndex,
                reassignedDwindleWindows);
          }
          if (!adjustedDwindleRatios.empty()) {
            Wh_Log(
                L"Dwindle admission adapted %zu split ratio(s) for HWND %p at slot %zu",
                adjustedDwindleRatios.size(),
                static_cast<void*>(repositionedAdmission),
                viableInsertionIndex);
            Diagnostics::RecordEvent(
                L"dwindle admission adapted ratios hwnd=%p slot=%zu changes=%zu",
                repositionedAdmission, viableInsertionIndex,
                adjustedDwindleRatios.size());
            for (const DwindleRatioAdjustment& adjustment :
                 adjustedDwindleRatios) {
              Wh_Log(
                  L"Dwindle adaptive split %zu changed %.6f -> %.6f for HWND %p",
                  adjustment.splitIndex, adjustment.before, adjustment.after,
                  static_cast<void*>(repositionedAdmission));
              Diagnostics::RecordEvent(
                  L"dwindle adaptive ratio changed hwnd=%p split=%zu from=%.6f to=%.6f",
                  repositionedAdmission, adjustment.splitIndex,
                  adjustment.before, adjustment.after);
            }
          }
          if (previousInsertionIndex != viableInsertionIndex) {
            Wh_Log(
                L"Constraint-aware admission repositioned HWND %p from slot %zu to %zu",
                static_cast<void*>(repositionedAdmission),
                previousInsertionIndex, viableInsertionIndex);
            Diagnostics::RecordEvent(
                L"constraint-aware admission repositioned hwnd=%p from=%zu to=%zu",
                repositionedAdmission, previousInsertionIndex,
                viableInsertionIndex);
          }
          g_workspaces.Save(key, workspace);
          continue;
        }

        // A still-pending admission/restore owns the failure it introduced.
        // Once every placement/adaptive/matching recovery above has failed, reject
        // that pending member itself; never evict an already-settled sibling to
        // make room for it.
        for (const ConstraintCandidate& candidate : candidates) {
          const WindowRecord* candidateRecord = workspace.Find(candidate.hwnd);
          if (!candidateRecord || !candidateRecord->pendingInsertionPlacement) {
            continue;
          }
          victim = candidate.hwnd;
          pendingAdmissionRejected = true;
          break;
        }

        if (!victim) {
          // Prefer a removal that makes the current order viable in one step.
          for (const ConstraintCandidate& candidate : candidates) {
            Workspace trial = workspace;
            if (!trial.Float(candidate.hwnd)) continue;

            std::vector<RECT> trialRects;
            ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
            if (BuildWorkspaceLayoutPlan(
                    trial, workArea, gap, &trialRects) !=
                LayoutPlanStatus::Success) {
              continue;
            }

            victim = candidate.hwnd;
            singleRemovalMakesCurrentOrderViable = true;
            break;
          }
        }
        // Automatic mode also resolves conflicts requiring several removals.
        // Each pass releases one member and replans the remaining group.
        if (!victim && IsAutomaticMode() && !candidates.empty()) {
          victim = candidates.front().hwnd;
        }
      }
      const WindowRecord* victimRecord = victim ? workspace.Find(victim) : nullptr;
      if (!victim || !victimRecord) {
        const HWND culprit = FindConstraintCulprit(
            workspace, rects, preferredTiledWindow);
        if (!constraintFailureReported) {
          ShowWindowConstraintConflict(
              workspace, rects, culprit, monitor);
          constraintFailureReported = true;
        }
        Diagnostics::RecordEvent(
            L"constraint conflict left membership unchanged; no single settled removal is sufficient desktop=%08X",
            key.desktopId.Data1);
        g_workspaces.Save(key, workspace);
        return;
      }

      const uint64_t generation = victimRecord->tilingGeneration;
      const WindowConstraints constraints = victimRecord->constraints;
      if (intrinsicallyImpossible) {
        Wh_Log(
          L"Native minimum exceeds workspace; floating HWND %p (generation=%llu)",
          static_cast<void*>(victim),
          static_cast<unsigned long long>(generation));
      } else if (pendingAdmissionRejected) {
        Wh_Log(
            L"Constraint conflict; rejecting pending admission HWND %p (generation=%llu)",
            static_cast<void*>(victim),
            static_cast<unsigned long long>(generation));
        Diagnostics::RecordEvent(
            L"pending admission rejected hwnd=%p generation=%llu",
            victim, static_cast<unsigned long long>(generation));
      } else if (singleRemovalMakesCurrentOrderViable) {
        Wh_Log(
            L"Current-order constraint conflict; floating single-removal candidate HWND %p (generation=%llu)",
            static_cast<void*>(victim),
            static_cast<unsigned long long>(generation));
      } else {
        Wh_Log(
            L"Constraint conflict; floating fallback HWND %p (generation=%llu)",
            static_cast<void*>(victim),
            static_cast<unsigned long long>(generation));
      }
      Diagnostics::RecordEvent(
          L"native constraints floated hwnd=%p generation=%llu intrinsic=%d singleRemovalCurrentOrder=%d preferred=%d min=%ldx%ld",
          victim, static_cast<unsigned long long>(generation),
          intrinsicallyImpossible ? 1 : 0,
          singleRemovalMakesCurrentOrderViable ? 1 : 0,
          victim == preferredTiledWindow ? 1 : 0,
          constraints.minWidth, constraints.minHeight);

      for (size_t i = 0;
           i < activeConstraints.size() &&
           i < workspace.TiledWindows().size();
           ++i) {
        const WindowConstraints& memberConstraint = activeConstraints[i];
        if (!memberConstraint.valid) continue;
        size_t compatibleSlotCount = 0;
        for (const RECT& slot : rects) {
          if (WindowConstraintFitsSlot(memberConstraint, slot)) {
            ++compatibleSlotCount;
          }
        }
        Diagnostics::RecordEvent(
            L"constraint conflict member hwnd=%p slot=%zu min=%ldx%ld compatibleSlots=%zu",
            workspace.TiledWindows()[i], i,
            memberConstraint.minWidth, memberConstraint.minHeight,
            compatibleSlotCount);
      }

      CancelConformanceLease(victim);
      const HWND culprit = FindConstraintCulprit(workspace, rects, victim);
      const bool automaticOverflow = IsAutomaticMode();
      if (!constraintFailureReported && !automaticOverflow) {
        ShowWindowConstraintConflict(workspace, rects, culprit ? culprit : victim, monitor);
        constraintFailureReported = true;
      }
      DiscardPendingAdmissionRecovery(victim);
      if (!(automaticOverflow
                ? FloatAutomaticOverflowWindow(key, workspace, victim, L"cannot fit")
                : FloatWindowAfterFailure(key, workspace, victim))) {
        g_workspaces.Save(key, workspace);
        return;
      }
      ++Diagnostics::g_runtime.counters.constraintFloats;
      g_workspaces.Save(key, workspace);
      if (workspace.Empty()) return;
      continue;
    }

    std::vector<HWND> toFloat;
    std::vector<HWND> toForget;
    bool constraintReplanRequested = false;
    bool layoutMutationRollbackRequested = false;
    const std::vector<HWND> active = workspace.TiledWindows();
    const size_t count = std::min(active.size(), rects.size());
    for (size_t i = 0; i < count; ++i) {
      const HWND hwnd = active[i];
      Workspace::PlacementAction action = Workspace::PlacementAction::None;

      if (!IsWindow(hwnd)) {
        action = Workspace::PlacementAction::Forget;
      } else {
        const WindowRecord* record = workspace.Find(hwnd);
        if (!record) {
          action = Workspace::PlacementAction::Forget;
        } else if (
            GetPhysicalSuspensionReason(hwnd) == SuspensionReason::None &&
            !IsWindowCloaked(hwnd)) {
          PlacementObservation observation =
              PlaceWindowChecked(hwnd, record->canMove, rects[i]);
          action = workspace.ApplyPlacementObservation(hwnd, observation);

          // A native minimum report is authoritative as soon as it is available.
          // If an application still corrects a legal target, re-probe in case its
          // own WM_GETMINMAXINFO contract changed dynamically. If the report is
          // unchanged, leave the event to conformance handling rather than
          // inventing a second empirical minimum-size policy.
          if (observation.result == PlacementResult::AdjustedByWindow) {
            const WindowConstraints before = record->constraints;
            const WindowConstraints fresh = ProbeWindowConstraints(hwnd);
            const WindowConstraints observed =
                MergeConstraintProbeWithCache(before, fresh);
            const bool geometryChanged =
                !SameConstraintGeometry(before, observed);
            workspace.UpdateConstraints(hwnd, observed);

            if (fresh.valid && geometryChanged) {
              action = Workspace::PlacementAction::None;
              constraintReplanRequested = true;
              CancelConformanceLease(hwnd);
              Diagnostics::RecordEvent(
                  L"placement refreshed native constraints hwnd=%p min=%ldx%ld dpi=%u",
                  hwnd, observed.minWidth, observed.minHeight, observed.dpi);
            }
          }

          if (!constraintReplanRequested) {
            if (action != Workspace::PlacementAction::None) {
              CancelConformanceLease(hwnd);
            } else {
              switch (observation.result) {
                case PlacementResult::Success:
                  if (observation.placementIssued) {
                    BeginConformanceLease(
                        hwnd, rects[i], false, preferredTiledWindow);
                  }
                  break;
                case PlacementResult::SuppressedByPhysicalState:
                  break;
                case PlacementResult::AdjustedByWindow:
                case PlacementResult::Refused:
                  BeginConformanceLease(
                      hwnd, rects[i], true, preferredTiledWindow);
                  if (g_settings.conformanceLeaseMs == 0 &&
                      TryRollbackPendingLayoutMutation(
                          key, monitor, &workspace, rects, hwnd)) {
                    layoutMutationRollbackRequested = true;
                  }
                  break;
                case PlacementResult::AccessDenied:
                case PlacementResult::Dead:
                  break;
              }
            }
          }
        }
      }

      if (layoutMutationRollbackRequested) break;
      switch (action) {
        case Workspace::PlacementAction::None:
          break;
        case Workspace::PlacementAction::Float:
          toFloat.push_back(hwnd);
          break;
        case Workspace::PlacementAction::Forget:
          toForget.push_back(hwnd);
          break;
      }
      if (constraintReplanRequested) break;
    }

    if (layoutMutationRollbackRequested) continue;
    if (constraintReplanRequested) {
      g_workspaces.Save(key, workspace);
      continue;
    }

    bool participationChanged = false;
    for (HWND hwnd : toForget) {
      CancelConformanceLease(hwnd);
      participationChanged |= workspace.Forget(hwnd);
    }
    for (HWND hwnd : toFloat) {
      const WindowRecord* record = workspace.Find(hwnd);
      if (!record || record->state != ManageState::Tiled) continue;
      Wh_Log(
          L"Window %p cannot be placed (result=%d); switching it to Floating",
          hwnd, static_cast<int>(record->lastPlacementResult));
      Diagnostics::RecordEvent(
          L"placement failure forced hwnd=%p to per-window Floating (result=%d)",
          hwnd, static_cast<int>(record->lastPlacementResult));
      CancelConformanceLease(hwnd);
      participationChanged |= FloatWindowAfterFailure(key, workspace, hwnd);
    }

    if (!participationChanged) {
      ClearSettledInsertionPlacements(key, workspace);
    }
    g_workspaces.Save(key, workspace);
    if (!participationChanged || workspace.Empty()) {
      ClearSettledLayoutMutation(key, workspace);
      return;
    }
  }

  Diagnostics::RecordEvent(
      L"arrangement safety bound exhausted desktop=%08X active=%zu",
      key.desktopId.Data1, workspace.ActiveCount());
  g_workspaces.Save(key, workspace);
  ClearSettledLayoutMutation(key, workspace);
}

// Consumes one expired generation. A user gesture, migration, or new
// authoritative placement that cancelled/replaced it wins if it happens first.
static bool TakeExpiredConformanceLease(
    HWND hwnd, uint64_t generation, ConformanceLease* outLease) {
  AssertWmThread(L"TakeExpiredConformanceLease");
  if (!hwnd || generation == 0 || !outLease) return false;

  const ULONGLONG now = GetTickCount64();
  bool taken = false;
  auto it = g_conformanceLeases.leases.find(hwnd);
  if (it != g_conformanceLeases.leases.end() &&
      it->second.generation == generation &&
      now >= it->second.expiresAtTickMs) {
    *outLease = it->second;
    g_conformanceLeases.leases.erase(it);
    taken = true;
  }
  return taken;
}

// Recomputes the target from the current workspace rather than trusting a copied
// lease blindly. This makes expiry harmless if a layout/work-area change replaced
// the authoritative tile without issuing another SetWindowPos for this HWND.
static bool GetCurrentAuthoritativeTiledRect(
    const DesktopMonitorKey& key, const Workspace& workspace, HWND hwnd,
    RECT* outRect) {
  if (!outRect || !hwnd || workspace.Layout() == TileLayout::Floating) return false;
  const WindowRecord* record = workspace.Find(hwnd);
  if (!record || record->state != ManageState::Tiled) return false;

  const size_t tiledIndex = workspace.TiledIndex(hwnd);
  if (tiledIndex == static_cast<size_t>(-1)) return false;

  HMONITOR monitor = key.ResolveMonitor();
  if (!monitor) return false;
  WorkspaceMetrics metrics;
  if (!GetWorkspaceMetrics(monitor, &metrics)) return false;

  std::vector<RECT> rects;
  if (BuildWorkspaceLayoutPlan(
          workspace, metrics.workArea, metrics.gap, &rects) !=
          LayoutPlanStatus::Success ||
      tiledIndex >= rects.size()) {
    return false;
  }
  *outRect = rects[tiledIndex];
  return true;
}

// An admission remains movable until every placement produced by its reflow has
// survived conformance. This matters because an existing sibling, not only the
// new HWND, can reveal the constraint that requires an alternate insertion slot.
static bool ClearSettledInsertionPlacements(
    const DesktopMonitorKey& key, Workspace& workspace) {
  if (HasOutstandingConformanceLease(workspace)) return false;

  bool changed = false;
  for (HWND hwnd : workspace.TiledWindows()) {
    const WindowRecord* record = workspace.Find(hwnd);
    if (!record || !record->pendingInsertionPlacement ||
        GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
        IsWindowCloaked(hwnd)) {
      continue;
    }

    RECT authoritative{};
    RECT current{};
    if (GetCurrentAuthoritativeTiledRect(
            key, workspace, hwnd, &authoritative) &&
        GetWindowFrameRect(hwnd, &current) &&
        RectsNear(current, authoritative)) {
      if (workspace.ClearInsertionPlacement(hwnd)) {
        changed = true;
        DiscardPendingAdmissionRecovery(hwnd);
      }
    }
  }
  return changed;
}

// HWND values can be reused. Conformance is allowed to act only on the same
// process identity that owns the authoritative WindowRecord; normal settled
// reconciliation remains responsible for forgetting a stale reused handle.
static bool ConformanceWindowMatchesRecordIdentity(
    HWND hwnd, const WindowRecord& record) {
  if (!hwnd || !record.pid) return false;
  DWORD livePid = 0;
  return GetWindowThreadProcessId(hwnd, &livePid) != 0 &&
         livePid != 0 && livePid == record.pid;
}

// The deadline is a final passive verdict, not one more fight with the app. If
// this exact lease still describes the current tile and the HWND is materially
// elsewhere, concede that it is unsuitable for tiling: float it and close the
// remaining layout around it. Stale/replaced state is simply discarded.
static void FinalizeExpiredConformanceLease(
    HWND hwnd, const ConformanceLease& lease) {
  AssertWmThread(L"FinalizeExpiredConformanceLease");

  auto stale = [&] {
    ++Diagnostics::g_runtime.counters.conformanceLeaseExpiredStale;
  };

  if (!IsWindow(hwnd) ||
      GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
      IsWindowCloaked(hwnd) ||
      IsMoveSizeGestureInProgress(hwnd)) {
    stale();
    return;
  }

  const std::vector<DesktopMonitorKey> owners = g_workspaces.OwnersOf(hwnd);
  if (owners.size() != 1 || !IsWorkspaceOnActiveDesktop(owners.front())) {
    stale();
    return;
  }

  Workspace workspace;
  if (!g_workspaces.Load(owners.front(), &workspace)) {
    stale();
    return;
  }

  const WindowRecord* record = workspace.Find(hwnd);
  if (!record || !ConformanceWindowMatchesRecordIdentity(hwnd, *record)) {
    stale();
    return;
  }

  RECT authoritative{};
  if (!GetCurrentAuthoritativeTiledRect(
          owners.front(), workspace, hwnd, &authoritative) ||
      !RectsNear(authoritative, lease.expectedRect)) {
    stale();
    return;
  }

  RECT current{};
  if (!GetWindowFrameRect(hwnd, &current)) {
    stale();
    return;
  }
  if (RectsNear(current, authoritative)) {
    ++Diagnostics::g_runtime.counters.conformanceLeaseExpiredCompliant;
    Diagnostics::RecordEvent(
        L"conformance lease completed compliant hwnd=%p attempts=%u",
        hwnd, lease.attempts);
    if (ClearSettledInsertionPlacements(owners.front(), workspace)) {
      g_workspaces.Save(owners.front(), workspace);
    }
    ClearSettledLayoutMutation(owners.front(), workspace);
    return;
  }

  // Expiry processing performs several live queries after its initial preflight.
  // A window that became suspended meanwhile must not be converted to Floating.
  if (GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None) {
    stale();
    return;
  }

  // The deadline is the last chance to catch a dynamic native minimum report
  // that changed after planning. If the app's own report is unchanged, remaining
  // nonconformance is not reinterpreted as a second empirical minimum policy.
  const WindowConstraints beforeConstraints = record->constraints;
  const WindowConstraints freshConstraints = ProbeWindowConstraints(hwnd);
  const WindowConstraints observed = MergeConstraintProbeWithCache(
      beforeConstraints, freshConstraints);
  if (!ConformanceWindowMatchesRecordIdentity(hwnd, *record) ||
      GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
      IsWindowCloaked(hwnd)) {
    stale();
    return;
  }
  const bool geometryChanged =
      !SameConstraintGeometry(beforeConstraints, observed);
  workspace.UpdateConstraints(hwnd, observed);
  if (freshConstraints.valid && geometryChanged) {
    Diagnostics::RecordEvent(
        L"conformance deadline refreshed native constraints hwnd=%p min=%ldx%ld dpi=%u",
        hwnd, observed.minWidth, observed.minHeight, observed.dpi);
    g_workspaces.Save(owners.front(), workspace);
    ArrangeWorkspace(owners.front(), lease.preferredTiledWindow);
    return;
  }

  const std::vector<RECT> noRejectedRects;
  if (TryRollbackPendingLayoutMutation(
          owners.front(), owners.front().ResolveMonitor(), &workspace,
          noRejectedRects, hwnd)) {
    ArrangeWorkspace(owners.front());
    return;
  }

  if (!FloatWindowAfterFailure(owners.front(), workspace, hwnd)) {
    stale();
    return;
  }

  ++Diagnostics::g_runtime.counters.conformanceLeaseExpiredFloats;
  Diagnostics::RecordEvent(
      L"conformance lease expired nonconforming; floated hwnd=%p attempts=%u",
      hwnd, lease.attempts);
  Wh_Log(
      L"Window %p remained outside its assigned tile at conformance deadline; switching it to Floating",
      hwnd);
  g_workspaces.Save(owners.front(), workspace);
  ArrangeWorkspace(owners.front(), lease.preferredTiledWindow);
}

// A tracked tiled HWND either confirms its authoritative rectangle or opens/reuses
// one bounded conformance lease. Native user gestures remain authoritative and
// are handled separately after MOVESIZEEND.
static bool HandleTiledWindowLocationChange(HWND hwnd) {
  if (IsMoveSizeGestureInProgress(hwnd)) return true;

  ConformanceLease lease;
  const ConformanceLeaseReadResult leaseState =
      ReadConformanceLease(hwnd, &lease);
  if (leaseState == ConformanceLeaseReadResult::Expired) {
    ScheduleNextConformanceTimer();
    return true;
  }
  const bool hasLease = leaseState == ConformanceLeaseReadResult::Active;

  if (!IsWindow(hwnd) ||
      GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
      IsWindowCloaked(hwnd)) {
    if (hasLease) CancelConformanceLease(hwnd, lease.generation);
    return false;
  }

  const std::vector<DesktopMonitorKey> owners = g_workspaces.OwnersOf(hwnd);
  if (owners.size() != 1 || !IsWorkspaceOnActiveDesktop(owners.front())) {
    if (hasLease) CancelConformanceLease(hwnd, lease.generation);
    return false;
  }

  Workspace workspace;
  if (!g_workspaces.Load(owners.front(), &workspace)) {
    if (hasLease) CancelConformanceLease(hwnd, lease.generation);
    return false;
  }
  const WindowRecord* record = workspace.Find(hwnd);
  if (!record || record->state != ManageState::Tiled ||
      workspace.Layout() == TileLayout::Floating ||
      !ConformanceWindowMatchesRecordIdentity(hwnd, *record)) {
    if (hasLease) CancelConformanceLease(hwnd, lease.generation);
    return false;
  }

  // A lease is target-specific. Layout/order/weight changes can alter a tile
  // without necessarily issuing SetWindowPos for this HWND, so never reinforce
  // the copied target until it is checked against the current layout plan.
  RECT authoritative{};
  if (!GetCurrentAuthoritativeTiledRect(
          owners.front(), workspace, hwnd, &authoritative)) {
    if (hasLease) CancelConformanceLease(hwnd, lease.generation);
    return false;
  }

  RECT current{};
  if (!GetWindowFrameRect(hwnd, &current)) {
    if (hasLease) CancelConformanceLease(hwnd, lease.generation);
    return false;
  }
  if (RectsNear(current, authoritative)) {
    return true;  // Our own placement/event echo; nothing to repair.
  }

  if (!hasLease || !RectsNear(authoritative, lease.expectedRect)) {
    if (hasLease) CancelConformanceLease(hwnd, lease.generation);
    BeginConformanceLease(hwnd, authoritative, true);
    return true;
  }

  const ULONGLONG now = GetTickCount64();
  if (lease.repairDueTickMs && now < lease.repairDueTickMs) {
    return true;
  }
  if (lease.lastRepairTickMs &&
      now - lease.lastRepairTickMs < g_settings.conformanceRepairIntervalMs) {
    const ULONGLONG due =
        lease.lastRepairTickMs + g_settings.conformanceRepairIntervalMs;
    // Do not simply drop a bursty snap-back event: schedule one delayed re-check
    // so a final app move inside the rate window cannot leave the tile displaced.
    if (DeferConformanceLeaseRepair(hwnd, lease.generation, due)) {
      ++Diagnostics::g_runtime.counters.conformanceLeaseDeferredRepairs;
      return true;
    }
    return false;
  }

  // A copied observation must still refer to the same active lease immediately
  // before we act. Earlier actor work can have cancelled or replaced it; the
  // generation check prevents stale work from accounting against the replacement.
  if (!IsCurrentConformanceLease(hwnd, lease.generation)) return false;

  // Re-check at the mutation boundary: Chromium can finish entering fullscreen
  // while the ownership/layout queries above are in progress.
  if (!ConformanceWindowMatchesRecordIdentity(hwnd, *record) ||
      GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
      IsWindowCloaked(hwnd)) {
    CancelConformanceLease(hwnd, lease.generation);
    return false;
  }

  // The incoming LOCATIONCHANGE says the application did not remain on its
  // authoritative tile. Re-probe its own native minimum contract before another
  // SetWindowPos, in case that contract changed dynamically.
  const WindowConstraints beforeConstraints = record->constraints;
  const WindowConstraints freshConstraints = ProbeWindowConstraints(hwnd);
  const WindowConstraints observedConstraints = MergeConstraintProbeWithCache(
      beforeConstraints, freshConstraints);
  // WM_GETMINMAXINFO executes target window code. Revalidate physical state
  // after that bounded call before either learning policy or reinforcing a tile.
  if (!ConformanceWindowMatchesRecordIdentity(hwnd, *record) ||
      GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
      IsWindowCloaked(hwnd)) {
    CancelConformanceLease(hwnd, lease.generation);
    return false;
  }
  const bool locationConstraintChanged =
      !SameConstraintGeometry(beforeConstraints, observedConstraints);
  workspace.UpdateConstraints(hwnd, observedConstraints);
  if (freshConstraints.valid && locationConstraintChanged) {
    CancelConformanceLease(hwnd, lease.generation);
    Diagnostics::RecordEvent(
        L"conformance location refreshed native constraints hwnd=%p min=%ldx%ld dpi=%u",
        hwnd, observedConstraints.minWidth, observedConstraints.minHeight,
        observedConstraints.dpi);
    g_workspaces.Save(owners.front(), workspace);
    ArrangeWorkspace(owners.front(), lease.preferredTiledWindow);
    return true;
  }

  PlacementObservation observation =
      PlaceWindowChecked(hwnd, record->canMove, authoritative);

  // Count every issued reinforcement attempt, not only successful ones. A failed
  // checked placement is still part of the lease's cadence and must not look like
  // repeated "repair=1" telemetry. This does not extend the original deadline.
  unsigned attemptNumber = lease.attempts + 1;
  const bool leaseRetained =
      RecordConformanceLeaseAttempt(hwnd, lease.generation, &attemptNumber);

  Workspace::PlacementAction action =
      workspace.ApplyPlacementObservation(hwnd, observation);


  ++Diagnostics::g_runtime.counters.conformanceLeaseRepairs;
  Diagnostics::RecordEvent(
      L"conformance lease reinforced hwnd=%p attempt=%u result=%d",
      hwnd, attemptNumber, static_cast<int>(observation.result));

  if (action == Workspace::PlacementAction::Forget) {
    CancelConformanceLease(hwnd, lease.generation);
    workspace.Forget(hwnd);
    g_workspaces.Save(owners.front(), workspace);
    ArrangeWorkspace(owners.front(), lease.preferredTiledWindow);
    return true;
  }
  if (action == Workspace::PlacementAction::Float) {
    CancelConformanceLease(hwnd, lease.generation);
    FloatWindowAfterFailure(owners.front(), workspace, hwnd);
    g_workspaces.Save(owners.front(), workspace);
    ArrangeWorkspace(owners.front(), lease.preferredTiledWindow);
    return true;
  }

  g_workspaces.Save(owners.front(), workspace);
  if (observation.result != PlacementResult::Success && leaseRetained) {
    // Adjusted/refused placements are exactly the cases where relying on another
    // LOCATIONCHANGE is unsafe: the application may already be sitting at its
    // final self-selected rectangle. Keep the lease alive and schedule one more
    // checked attempt at the configured cadence. The hard deadline performs the
    // passive final verdict and Floats only if the HWND is still materially off
    // its current tile. AccessDenied remains an immediate terminal result above.
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG due = std::min(
        lease.expiresAtTickMs,
        now + g_settings.conformanceRepairIntervalMs);
    DeferConformanceLeaseRepair(hwnd, lease.generation, due);
  }
  return true;
}

static void ProcessConformanceTimer() {
  AssertWmThread(L"ProcessConformanceTimer");
  if (g_wm.conformanceTimer) {
    KillTimer(nullptr, g_wm.conformanceTimer);
    g_wm.conformanceTimer = 0;
  }

  struct ExpiredCandidate {
    HWND hwnd = nullptr;
    uint64_t generation = 0;
  };

  const ULONGLONG now = GetTickCount64();
  std::vector<ExpiredCandidate> expired;
  std::vector<HWND> dueWindows;
  for (auto& kv : g_conformanceLeases.leases) {
    ConformanceLease& lease = kv.second;
    if (now >= lease.expiresAtTickMs) {
      expired.push_back({kv.first, lease.generation});
      continue;
    }
    if (lease.repairDueTickMs && lease.repairDueTickMs <= now) {
      dueWindows.push_back(kv.first);
      lease.repairDueTickMs = 0;
    }
  }
  for (const ExpiredCandidate& candidate : expired) {
    ConformanceLease lease;
    if (TakeExpiredConformanceLease(
            candidate.hwnd, candidate.generation, &lease)) {
      FinalizeExpiredConformanceLease(candidate.hwnd, lease);
    }
  }
  for (HWND hwnd : dueWindows) {
    HandleTiledWindowLocationChange(hwnd);
  }
  ScheduleNextConformanceTimer();
}

// Uses aggregate work-area coverage as a coarse guard against learning arbitrary
// floating geometry as an authoritative tiled layout.
static bool SnapshotLooksTiled(const std::vector<HWND>& windows, const RECT& workArea) {
  if (windows.empty()) return false;
  const long long workAreaArea = RectAreaLL(workArea);
  long long sumArea = 0;
  for (HWND w : windows) sumArea += WindowAreaOnWorkArea(w, workArea);
  const long long lo = (workAreaArea * 85) / 100;
  const long long hi = (windows.size() <= 2) ? (workAreaArea * 115) / 100 : (workAreaArea * 105) / 100;
  return sumArea >= lo && sumArea <= hi;
}

//=============================================================================
// Workspace reconciliation
//=============================================================================

namespace Platform::WindowEvents {
static bool HasTrackedMonitorOwnershipMismatch(HWND hwnd);
}

namespace Reconcile {

static bool SameWorkspaceKey(const DesktopMonitorKey& a, const DesktopMonitorKey& b);

enum class ReconcileScope : uint8_t {
  None = 0,
  Participation = 1 << 0,
  Desktop = 1 << 1,
  Monitor = 1 << 2,
  All = (1 << 0) | (1 << 1) | (1 << 2),
};

constexpr ReconcileScope operator|(ReconcileScope a, ReconcileScope b) {
  return static_cast<ReconcileScope>(
      static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr bool HasReconcileScope(ReconcileScope value, ReconcileScope flag) {
  return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
}

static void ReconcileKnownWindows(
    ReconcileScope scope, std::vector<DesktopMonitorKey>* changedKeys,
    HWND onlyHwnd = nullptr);
static bool EnsureUniqueWindowOwnership(
    HWND hwnd, const DesktopMonitorKey& targetKey,
    std::vector<DesktopMonitorKey>* changedKeys = nullptr);

static TileLayout ResolveInitialWorkspaceLayout(
    const DesktopMonitorKey& key, int* matchedRuleIndex = nullptr,
    const Platform::VirtualDesktop::DesktopMetadata* liveMetadata = nullptr) {
  if (matchedRuleIndex) *matchedRuleIndex = 0;
  Platform::VirtualDesktop::DesktopMetadata desktop;
  bool metadataRead = false;
  MONITORINFO monitorInfo{sizeof(monitorInfo)};
  const bool haveMonitorInfo =
      !g_settings.workspaceInitializationRules.empty() &&
      GetMonitorInfoW(key.ResolveMonitor(), &monitorInfo);

  for (const auto& rule : g_settings.workspaceInitializationRules) {
    if (!rule.monitorId.empty() && rule.monitorId != key.monitor.deviceId) continue;
    if (rule.monitor == RuleMonitor::Primary || rule.monitor == RuleMonitor::NonPrimary) {
      if (!haveMonitorInfo) continue;
      const bool primary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
      if (primary != (rule.monitor == RuleMonitor::Primary)) continue;
    }
    if (!rule.desktopNameContains.empty() || rule.desktopNumber != 0) {
      if (!metadataRead) {
        desktop = liveMetadata ? *liveMetadata : Platform::VirtualDesktop::ReadDesktopMetadata(key.desktopId);
        std::transform(
            desktop.name.begin(), desktop.name.end(), desktop.name.begin(),
            [](wchar_t ch) { return std::towlower(ch); });
        metadataRead = true;
      }
      if (rule.desktopNumber != 0 && rule.desktopNumber != desktop.number) continue;
      if (!rule.desktopNameContains.empty() &&
          desktop.name.find(rule.desktopNameContains) == std::wstring::npos) {
        continue;
      }
    }
    if (matchedRuleIndex) *matchedRuleIndex = rule.settingsIndex;
    return rule.layout;
  }
  return g_settings.defaultLayout;
}

static Workspace MakeDefaultWorkspace(const DesktopMonitorKey& key) {
  Workspace workspace;
  int matchedRuleIndex = 0;
  workspace.SetLayout(ResolveInitialWorkspaceLayout(key, &matchedRuleIndex));
  workspace.SetMasterRatio(g_settings.masterPercent / 100.0);
  if (matchedRuleIndex != 0) {
    Diagnostics::RecordEvent(
        L"workspace initial layout desktop=%08X monitorId=%ls rule=%d layout=%d",
        key.desktopId.Data1, key.monitor.deviceId.c_str(), matchedRuleIndex,
        static_cast<int>(workspace.Layout()));
  }
  return workspace;
}

static bool ReconcileTrackedSnapshotOwnership(
    const DesktopMonitorKey& key, const std::vector<HWND>& snapshot,
    std::vector<DesktopMonitorKey>* changedKeys) {
  bool changed = false;
  for (HWND hwnd : snapshot) {
    if (IsWindowTrackedInAnyState(hwnd) &&
        EnsureUniqueWindowOwnership(hwnd, key, changedKeys)) {
      changed = true;
    }
  }
  return changed;
}

static bool SeedInitialSnapshot(
    Workspace& workspace, const std::vector<HWND>& initialSnapshot,
    bool useAutomaticAdmissionPolicy) {
  bool changed = false;
  for (HWND hwnd : initialSnapshot) {
    changed |= AdmitInitialObservedWindow(
        workspace, hwnd, useAutomaticAdmissionPolicy);
  }
  return changed;
}

static bool RestoreSuspendedSnapshot(
    Workspace& workspace, const std::vector<HWND>& snapshot) {
  bool changed = false;
  for (HWND hwnd : snapshot) {
    const WindowRecord* record = workspace.Find(hwnd);
    if (record && record->state == ManageState::Suspended) {
      if (workspace.Restore(hwnd)) {
        changed = true;
      }
    }
  }
  return changed;
}

static bool ReadoptFloatingSnapshot(
    Workspace& workspace, const std::vector<HWND>& snapshot) {
  bool changed = false;
  for (HWND hwnd : snapshot) {
    const WindowRecord* record = workspace.Find(hwnd);
    if (record && record->state == ManageState::Floating) {
      changed |= workspace.ActivateTiled(hwnd);
    }
  }
  return changed;
}

static bool AdmitUntrackedSnapshot(
    Workspace& workspace, const std::vector<HWND>& snapshot, bool allowAdmission,
    bool useAutomaticAdmissionPolicy = false,
    std::vector<HWND>* admittedWindows = nullptr) {
  if (!allowAdmission) return false;

  const bool afterFocused =
      useAutomaticAdmissionPolicy &&
      g_settings.automaticNewWindowPosition ==
          AutomaticNewWindowPosition::AfterFocused;
  HWND anchor = afterFocused ? workspace.LastFocusedTiledWindow() : nullptr;

  bool changed = false;
  for (HWND hwnd : snapshot) {
    if (workspace.HasRecord(hwnd) || IsWindowTrackedInAnyState(hwnd) ||
        IsWindowCloaked(hwnd)) {
      continue;
    }
    if (useAutomaticAdmissionPolicy && IsWindowAwaitingInitialPlacement(hwnd)) {
      continue;
    }

    bool alwaysFloating = false;
    const bool startsFloating =
        ShouldWindowStartFloating(hwnd, &alwaysFloating);
    if (alwaysFloating || (useAutomaticAdmissionPolicy && startsFloating)) {
      WindowRecord record = MakeWindowRecord(hwnd, ManageState::Floating);
      record.pendingDefaultFloatingCenter = true;
      record.alwaysFloating = alwaysFloating;
      if (workspace.AdmitFloating(std::move(record))) {
        changed = true;
        if (admittedWindows) admittedWindows->push_back(hwnd);
      }
      continue;
    }

    bool admitted = false;
    WindowRecord record = MakeWindowRecord(hwnd, ManageState::Tiled);
    record.automaticOverflowMonitorFixed =
        HasMonitorPlacementRule(hwnd);
    if (anchor) {
      admitted = workspace.AdmitTiledAfter(std::move(record), anchor);
    } else {
      admitted = workspace.AdmitTiled(std::move(record));
    }

    if (!admitted) continue;
    changed = true;
    if (admittedWindows) admittedWindows->push_back(hwnd);

    // Preserve enumeration order when one reconciliation burst discovers several
    // windows without changing the retained user-focus anchor.
    if (afterFocused) anchor = hwnd;
  }
  return changed;
}

// Automatic discovery can observe a new window while Windows still reports it as
// minimized, maximized, or fullscreen. Such HWNDs belong to the workspace
// logically but must not enter the active tiling set until restored.
static bool AdmitUntrackedSuspendedSnapshot(
    Workspace& workspace, const std::vector<HWND>& snapshot, bool allowAdmission,
    bool useAutomaticAdmissionPolicy = false) {
  if (!allowAdmission) return false;

  const bool afterFocused =
      useAutomaticAdmissionPolicy &&
      g_settings.automaticNewWindowPosition ==
          AutomaticNewWindowPosition::AfterFocused;
  const HWND anchor = afterFocused ? workspace.LastFocusedTiledWindow() : nullptr;

  auto admitOne = [&](HWND hwnd) {
    if (workspace.HasRecord(hwnd) || IsWindowTrackedInAnyState(hwnd) ||
        IsWindowCloaked(hwnd)) {
      return false;
    }

    if (useAutomaticAdmissionPolicy && IsWindowAwaitingInitialPlacement(hwnd)) {
      return false;
    }

    const SuspensionReason reason = GetPhysicalSuspensionReason(hwnd);
    if (reason != SuspensionReason::Minimized &&
        reason != SuspensionReason::Maximized) {
      return false;
    }

    bool alwaysFloating = false;
    const bool startsFloating =
        ShouldWindowStartFloating(hwnd, &alwaysFloating);
    if (alwaysFloating || (useAutomaticAdmissionPolicy && startsFloating)) {
      WindowRecord record = MakeWindowRecord(hwnd, ManageState::Floating);
      record.pendingDefaultFloatingCenter = true;
      record.alwaysFloating = alwaysFloating;
      return workspace.AdmitFloating(std::move(record));
    }

    WindowRecord record = MakeWindowRecord(hwnd);
    record.automaticOverflowMonitorFixed =
        HasMonitorPlacementRule(hwnd);
    if (anchor) {
      return workspace.AdmitInitialAfter(std::move(record), reason, anchor);
    }
    return workspace.AdmitInitial(std::move(record), reason);
  };

  bool changed = false;
  if (anchor) {
    // Each suspended member targets the same logical point after the anchor.
    // Reverse traversal preserves the snapshot's original order.
    for (auto it = snapshot.rbegin(); it != snapshot.rend(); ++it) {
      changed |= admitOne(*it);
    }
  } else {
    for (HWND hwnd : snapshot) changed |= admitOne(hwnd);
  }
  return changed;
}

static bool AdoptSnapshotGeometryIfEligible(
    Workspace& workspace, const std::vector<HWND>& snapshot,
    const RECT& workArea, LONG gap, HMONITOR monitor) {
  if (workspace.Layout() == TileLayout::Floating || workspace.HasSuspended() ||
      !workspace.AllTiledVisible(snapshot) ||
      !SnapshotLooksTiled(workspace.TiledWindows(), workArea)) {
    return false;
  }

  const HWND preferredMaster = workspace.Empty() ? nullptr
                                                 : workspace.TiledWindows().front();
  std::vector<WorkspaceGeometryItem> geometry = CaptureWorkspaceGeometry(
      workspace.TiledWindows(), monitor, workArea);
  Workspace captured = Workspace::AdoptGeometry(
      workspace.Layout(), workArea, gap, g_settings.masterPercent / 100.0,
      geometry, preferredMaster);
  const HWND retainedFocus = workspace.LastFocusedWindow();
  captured.MergeRememberedFloatingGeometryFrom(workspace);
  captured.MergeNonTiledRecordsFrom(workspace);
  captured.RememberFocusedWindow(retainedFocus);
  workspace = std::move(captured);
  return true;
}

// Reconciles a monitor snapshot into the current workspace while preserving known
// order and inactive records. Explicit adoption can re-tile floated windows and
// learn complete tiled geometry before persisting and arranging the result.
static bool EnsureWorkspaceFromSnapshot(
    HMONITOR monitor, bool adoptCurrentGeometry,
    std::vector<DesktopMonitorKey>* ownershipChangedKeys = nullptr) {
  DesktopMonitorKey key{};
  if (!GetCurrentWorkspaceKey(monitor, &key)) return false;
  WorkspaceMetrics metrics;
  if (!GetWorkspaceMetrics(monitor, &metrics)) return false;

  const std::vector<HWND> snapshot = CollectTileWindows(monitor);
  Workspace workspace;
  const bool workspaceExisted = g_workspaces.Load(key, &workspace);
  const std::vector<HWND> initialSnapshot = workspaceExisted
      ? std::vector<HWND>{}
      : CollectWorkspaceWindowsForInitialization(monitor, key.desktopId);

  ReconcileTrackedSnapshotOwnership(
      key, workspaceExisted ? snapshot : initialSnapshot, ownershipChangedKeys);

  if (!g_workspaces.Load(key, &workspace)) workspace = MakeDefaultWorkspace(key);
  if (!workspaceExisted) {
    SeedInitialSnapshot(
        workspace, initialSnapshot,
        IsAutomaticMode() && !adoptCurrentGeometry);
  }

  // Tile Workspace may readopt current geometry, but it never bypasses native
  // application minimums. Constraint overrides belong at the application layer,
  // not in the tiling solver.

  RestoreSuspendedSnapshot(workspace, snapshot);
  if (adoptCurrentGeometry) ReadoptFloatingSnapshot(workspace, snapshot);
  AdmitUntrackedSnapshot(workspace, snapshot, true, false);

  if (adoptCurrentGeometry) {
    RefreshWorkspaceNativeConstraints(workspace, true, L"Tile Workspace");
    AdoptSnapshotGeometryIfEligible(
        workspace, snapshot, metrics.workArea, metrics.gap, monitor);
  }

  g_workspaces.Save(key, workspace);
  ArrangeWorkspace(key);
  return true;
}




// A managed HWND is allowed to have exactly one logical workspace owner.
static std::vector<DesktopMonitorKey> FindWorkspaceKeysContainingWindow(HWND hwnd) {
  return g_workspaces.OwnersOf(hwnd);
}

static void ForgetManagedWindow(HWND hwnd) {
  CancelConformanceLease(hwnd);
  const std::vector<DesktopMonitorKey> keys = g_workspaces.OwnersOf(hwnd);
  for (const auto& key : keys) {
    Workspace workspace;
    if (!g_workspaces.Load(key, &workspace)) continue;
    if (workspace.Forget(hwnd)) {
      g_workspaces.Save(key, workspace);
      ArrangeWorkspace(key);
    }
  }
  ClearMoveSizeSamples(hwnd);
}

static bool SameWorkspaceKey(const DesktopMonitorKey& a, const DesktopMonitorKey& b) {
  return a.monitor == b.monitor && IsEqualGUID(a.desktopId, b.desktopId);
}

static void AddUniqueWorkspaceKey(
    std::vector<DesktopMonitorKey>& keys, const DesktopMonitorKey& key) {
  if (!key.monitor) return;
  if (std::none_of(keys.begin(), keys.end(),
                   [&](const DesktopMonitorKey& existing) {
                     return SameWorkspaceKey(existing, key);
                   })) {
    keys.push_back(key);
  }
}

// Transfers one managed record between repositories while allowing Workspace to
// rebase its layout-specific logical slot/weight at the destination.
static bool MigrateManagedWindow(
    HWND hwnd, const DesktopMonitorKey& sourceKey, const DesktopMonitorKey& targetKey) {
  if (!hwnd || SameWorkspaceKey(sourceKey, targetKey)) return false;
  CancelConformanceLease(hwnd);
  DiscardPendingAdmissionRecovery(hwnd);

  Workspace source;
  if (!g_workspaces.Load(sourceKey, &source)) return false;
  const TileLayout sourceLayout = source.Layout();
  const bool wasFocused = source.LastFocusedWindow() == hwnd;

  WindowRecord record;
  if (!source.ExtractForMigration(hwnd, &record)) return false;
  const ManageState sourceState = record.state;
  g_workspaces.Save(sourceKey, source);

  Workspace target;
  if (!g_workspaces.Load(targetKey, &target)) {
    target = MakeDefaultWorkspace(targetKey);
  }

  HWND insertionAnchor = nullptr;
  if (IsAutomaticMode() &&
      g_settings.automaticNewWindowPosition ==
          AutomaticNewWindowPosition::AfterFocused) {
    insertionAnchor = target.LastFocusedTiledWindow();
    if (insertionAnchor == hwnd) insertionAnchor = nullptr;
  }

  target.AdmitMigrated(std::move(record), insertionAnchor);
  if (wasFocused) target.RememberFocusedWindow(hwnd);

  if (sourceLayout != TileLayout::Floating &&
      sourceState != ManageState::Floating &&
      target.Layout() == TileLayout::Floating) {
    const MoveSizeGesture gesture = PeekMoveSizeGesture(hwnd);
    const FloatingPlacementHint hint = MakeFloatingPlacementHintFromGesture(gesture);
    // Automatic migration owns passive geometry. Manual cross-desktop moves
    // never reach this path; same-desktop Manual monitor drags may repair only
    // when the completed user gesture supplies an explicit anchor.
    if (IsAutomaticMode() ||
        hint.intent == FloatingPlacementIntent::PreserveAnchor) {
      const bool repaired = RepairFloatingGeometry(targetKey, target, hwnd, hint);
      if (repaired && hint.intent == FloatingPlacementIntent::PreserveAnchor) {
        // The completed gesture has already served as migration context. Clear it
        // so a deferred desktop migration cannot leave an old drag sample behind.
        ClearMoveSizeSamples(hwnd);
      }
    }
  }

  g_workspaces.Save(targetKey, target);
  ++Diagnostics::g_runtime.counters.workspaceMigrations;
  Diagnostics::RecordEvent(
      L"workspace migration hwnd=%p sourceDesktop=%08X targetDesktop=%08X sourceMonitorId=%ls targetMonitorId=%ls",
      hwnd, sourceKey.desktopId.Data1, targetKey.desktopId.Data1,
      sourceKey.monitor.deviceId.c_str(), targetKey.monitor.deviceId.c_str());
  return true;
}

// Reconcile one tracked HWND to a single logical workspace. This mirrors the
// mature WM model: physical ownership is queried/observed globally, while the
// layout tree keeps one logical owner. If an older path already duplicated the
// HWND, prefer the record that is already in the observed workspace and remove
// every stale copy.
static bool EnsureUniqueWindowOwnership(
    HWND hwnd, const DesktopMonitorKey& targetKey,
    std::vector<DesktopMonitorKey>* changedKeys) {
  std::vector<DesktopMonitorKey> owners = FindWorkspaceKeysContainingWindow(hwnd);
  if (owners.empty()) return false;

  const bool targetAlreadyOwns = std::any_of(
      owners.begin(), owners.end(), [&](const DesktopMonitorKey& key) {
        return SameWorkspaceKey(key, targetKey);
      });

  bool changed = false;
  if (!targetAlreadyOwns) {
    const DesktopMonitorKey sourceKey = owners.front();
    if (!MigrateManagedWindow(hwnd, sourceKey, targetKey)) {
      Wh_Log(L"Failed to reconcile HWND %p to observed workspace", hwnd);
      return false;
    }

    changed = true;
    if (changedKeys) {
      AddUniqueWorkspaceKey(*changedKeys, sourceKey);
      AddUniqueWorkspaceKey(*changedKeys, targetKey);
    }
  }

  // MigrateManagedWindow removes its selected source and any pre-existing
  // destination record, but an old bug may have left more than two copies.
  // Remove every remaining non-target owner explicitly.
  for (const auto& ownerKey : owners) {
    if (SameWorkspaceKey(ownerKey, targetKey)) continue;

    Workspace ownerState;
    if (!g_workspaces.Load(ownerKey, &ownerState)) continue;
    if (ownerState.Forget(hwnd)) {
      g_workspaces.Save(ownerKey, ownerState);
      changed = true;
      if (changedKeys) AddUniqueWorkspaceKey(*changedKeys, ownerKey);
    }
  }

  if (owners.size() > 1) {
    Wh_Log(L"Repaired duplicate HWND ownership for %p (%zu logical owners)",
           hwnd, owners.size());
  }

  return changed;
}

// Refreshes only the requested ownership aspects from physical window state.
static bool GetObservedWorkspaceKey(
    HWND hwnd, const DesktopMonitorKey& fallback, ReconcileScope scope,
    DesktopMonitorKey* outKey) {
  if (!outKey) return false;

  DesktopMonitorKey observed = fallback;
  bool observedAnything = false;

  if (HasReconcileScope(scope, ReconcileScope::Desktop)) {
    GUID desktopId{};
    if (GetWindowDesktopIdSafe(hwnd, &desktopId)) {
      observed.desktopId = desktopId;
      observedAnything = true;
    }
  }

  if (HasReconcileScope(scope, ReconcileScope::Monitor)) {
    HMONITOR monitor = GetWindowPhysicalMonitor(hwnd);
    Model::MonitorId monitorId;
    if (monitor && Model::MonitorId::FromHMonitor(monitor, &monitorId)) {
      observed.monitor = std::move(monitorId);
      observedAnything = true;
    }
  }

  if (!observedAnything || !observed.monitor) return false;
  *outKey = observed;
  return true;
}

static void ForgetWindowFromOwners(
    HWND hwnd, const std::vector<DesktopMonitorKey>& ownerKeys,
    std::vector<DesktopMonitorKey>* changedKeys) {
  CancelConformanceLease(hwnd);
  DiscardPendingAdmissionRecovery(hwnd);
  for (const auto& ownerKey : ownerKeys) {
    Workspace workspace;
    if (!g_workspaces.Load(ownerKey, &workspace)) continue;
    if (workspace.Forget(hwnd)) {
      g_workspaces.Save(ownerKey, workspace);
      if (changedKeys) AddUniqueWorkspaceKey(*changedKeys, ownerKey);
    }
  }
  ClearMoveSizeSamples(hwnd);
}

static std::vector<DesktopMonitorKey> ReconcileWindowOwnership(
    HWND hwnd, ReconcileScope scope,
    std::vector<DesktopMonitorKey> ownerKeys,
    std::vector<DesktopMonitorKey>* changedKeys) {
  const ReconcileScope ownershipScope = static_cast<ReconcileScope>(
      static_cast<uint8_t>(scope) &
      (static_cast<uint8_t>(ReconcileScope::Desktop) |
       static_cast<uint8_t>(ReconcileScope::Monitor)));
  if (ownershipScope == ReconcileScope::None || ownerKeys.empty()) return ownerKeys;

  DesktopMonitorKey observedKey{};
  if (GetObservedWorkspaceKey(hwnd, ownerKeys.front(), ownershipScope, &observedKey)) {
    const bool desktopChanged =
        HasReconcileScope(ownershipScope, ReconcileScope::Desktop) &&
        !IsEqualGUID(ownerKeys.front().desktopId, observedKey.desktopId);

    // Manual groups are deliberately inert. Moving a member to another virtual
    // desktop breaks its membership instead of transplanting it into that
    // desktop's workspace. Monitor-only moves retain the existing migration
    // behavior; Automatic mode continues to migrate across both dimensions.
    if (IsManualMode() && desktopChanged) {
      ForgetWindowFromOwners(hwnd, ownerKeys, changedKeys);
      return {};
    }

    EnsureUniqueWindowOwnership(hwnd, observedKey, changedKeys);
    ownerKeys = g_workspaces.OwnersOf(hwnd);
  }
  return ownerKeys;
}

static void ReconcileWindowParticipation(
    HWND hwnd, const std::vector<DesktopMonitorKey>& ownerKeys,
    std::vector<DesktopMonitorKey>* changedKeys) {
  const SuspensionReason physicalReason = GetPhysicalSuspensionReason(hwnd);

  // In Manual mode, minimizing is an explicit departure from the managed group.
  // Forget the logical member entirely; restoring the HWND won't re-admit it
  // because Manual discovery remains closed. Maximization is intentionally
  // different: suspend it so restore returns to the exact saved slot/weight.
  if (IsManualMode() && physicalReason == SuspensionReason::Minimized) {
    ForgetWindowFromOwners(hwnd, ownerKeys, changedKeys);
    return;
  }

  for (const auto& ownerKey : ownerKeys) {
    Workspace workspace;
    if (!g_workspaces.Load(ownerKey, &workspace)) continue;

    const WindowRecord* record = workspace.Find(hwnd);
    if (!record) continue;

    SuspensionReason reason = physicalReason;
    if (reason == SuspensionReason::Maximized && !IsZoomed(hwnd) &&
        record->state == ManageState::Tiled) {
      // A borderless tile can legitimately cover the monitor when the configured
      // work area has no visible insets/gaps. If it already occupies its exact
      // authoritative tile, don't mistake that geometry for fullscreen.
      RECT authoritative{};
      RECT current{};
      if (GetCurrentAuthoritativeTiledRect(
              ownerKey, workspace, hwnd, &authoritative) &&
          GetWindowFrameRect(hwnd, &current) &&
          RectsNear(current, authoritative)) {
        reason = SuspensionReason::None;
      }
    }

    bool changed = false;
    bool restoredIntoFloatingLayout = false;
    const bool pendingInsertionBeforeSuspension =
        record->state == ManageState::Tiled &&
        record->pendingInsertionPlacement;

    if (record->state == ManageState::Tiled &&
        reason != SuspensionReason::None) {
      CancelConformanceLease(hwnd);
      ClearMoveSizeSamples(hwnd);
      const bool suspended = workspace.Suspend(hwnd, reason);
      changed |= suspended;
      if (suspended && pendingInsertionBeforeSuspension) {
        changed |= RestorePendingAdmissionRecoveryAfterSuspension(
            ownerKey, hwnd, &workspace);
      }
    } else if (record->state == ManageState::Suspended) {
      if (reason == SuspensionReason::None) {
        const bool restored = workspace.Restore(hwnd);
        changed |= restored;
        restoredIntoFloatingLayout =
            restored && workspace.Layout() == TileLayout::Floating;
      } else {
        if (workspace.Suspend(hwnd, reason)) {
          changed = true;
        }
      }
    }

    if (changed) {
      // A member that was suspended while its workspace entered Floating missed
      // the layout-transition restore. Apply the same passive remembered-geometry
      // policy when it becomes active again.
      if (restoredIntoFloatingLayout) {
        RepairFloatingGeometry(
            ownerKey, workspace, hwnd, FloatingPlacementHint{});
      }
      g_workspaces.Save(ownerKey, workspace);
      if (changedKeys) AddUniqueWorkspaceKey(*changedKeys, ownerKey);
    }
  }
}

static void ReconcileKnownWindow(
    HWND hwnd, ReconcileScope scope,
    std::vector<DesktopMonitorKey>* changedKeys) {
  std::vector<DesktopMonitorKey> ownerKeys = g_workspaces.OwnersOf(hwnd);
  if (ownerKeys.empty()) return;

  if (!IsWindow(hwnd) || IsWindowExcludedByRules(hwnd)) {
    ForgetWindowFromOwners(hwnd, ownerKeys, changedKeys);
    return;
  }

  // HWND values are reusable. A destroy event can be missed during a brief hook/
  // shell lifecycle gap and the numeric handle may later identify another process.
  // Never let the old logical record silently attach itself to that new window.
  DWORD livePid = 0;
  GetWindowThreadProcessId(hwnd, &livePid);
  bool pidMismatch = false;
  for (const auto& ownerKey : ownerKeys) {
    Workspace workspace;
    if (!g_workspaces.Load(ownerKey, &workspace)) continue;
    const WindowRecord* record = workspace.Find(hwnd);
    if (record && record->pid != 0 && livePid != 0 && record->pid != livePid) {
      pidMismatch = true;
      break;
    }
  }
  if (pidMismatch) {
    Diagnostics::RecordEvent(
        L"discarded stale HWND ownership after PID reuse hwnd=%p livePid=%lu",
        hwnd, livePid);
    ForgetWindowFromOwners(hwnd, ownerKeys, changedKeys);
    ClearMoveSizeSamples(hwnd);
    CancelConformanceLease(hwnd);
    return;
  }

  ownerKeys = ReconcileWindowOwnership(hwnd, scope, std::move(ownerKeys), changedKeys);
  if (ownerKeys.empty()) return;

  // Rules normally classify a window at admission. Also enforce Always floating
  // for an already-managed window after a settings reload; once applied, retain
  // it for that HWND's lifetime even if a title-based match later changes.
  if (ShouldWindowAlwaysFloat(hwnd)) {
    bool policyChanged = false;
    for (const auto& ownerKey : ownerKeys) {
      Workspace workspace;
      if (!g_workspaces.Load(ownerKey, &workspace) ||
          !workspace.EnforceAlwaysFloating(hwnd)) {
        continue;
      }
      policyChanged = true;
      g_workspaces.Save(ownerKey, workspace);
      if (changedKeys) AddUniqueWorkspaceKey(*changedKeys, ownerKey);
    }
    if (policyChanged) CancelConformanceLease(hwnd);
  }

  if (HasReconcileScope(scope, ReconcileScope::Participation)) {
    ReconcileWindowParticipation(hwnd, ownerKeys, changedKeys);
  }
}

// Orchestration only: choose the known HWND set, then run the explicitly scoped
// ownership/participation phases for each one.
static void ReconcileKnownWindows(
    ReconcileScope scope, std::vector<DesktopMonitorKey>* changedKeys,
    HWND onlyHwnd) {
  ++Diagnostics::g_runtime.counters.reconcileCalls;
  const std::vector<HWND> known = onlyHwnd
                                      ? std::vector<HWND>{onlyHwnd}
                                      : g_workspaces.KnownWindows();
  Diagnostics::g_runtime.counters.reconcileWindowsExamined += known.size();
  for (HWND hwnd : known) ReconcileKnownWindow(hwnd, scope, changedKeys);
}

static void AddUniqueMonitor(std::vector<HMONITOR>& monitors, HMONITOR monitor) {
  if (IsLiveMonitorHandle(monitor) &&
      std::find(monitors.begin(), monitors.end(), monitor) == monitors.end()) {
    monitors.push_back(monitor);
  }
}

static void AddPendingWorkspaceArrange(const GUID& desktopId, HMONITOR monitor) {
  if (!IsLiveMonitorHandle(monitor)) return;
  // Only if not scheduled already
  auto it = std::find_if(
      g_wm.pendingDesktopArranges.begin(), g_wm.pendingDesktopArranges.end(),
      [&](const PendingWorkspaceArrange& pending) {
        return pending.monitor == monitor && IsEqualGUID(pending.desktopId, desktopId);
      });
  if (it == g_wm.pendingDesktopArranges.end()) {
    g_wm.pendingDesktopArranges.push_back({desktopId, monitor});
  }
}

// Run before snapshots are built. A rule chooses the initial destination once;
// admission and overflow remain owned by the destination's ordinary active pass.
static void RouteNewWindows(std::vector<HMONITOR>& monitors) {
  if (!g_wm.windowRoutingReady) return;

  const HWND foregroundWindow = GetForegroundWindow();
  std::vector<HWND> candidates;
  ++Diagnostics::g_runtime.counters.enumWindowsPasses;
  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        ++Diagnostics::g_runtime.counters.enumWindowsVisited;
        if (!g_wm.windowRoutingEvaluated.count(hwnd) || g_wm.pendingWindowRoutes.count(hwnd)) {
          reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
        }
        return TRUE;
      }, reinterpret_cast<LPARAM>(&candidates));

  for (HWND hwnd : candidates) {
    // Explicit admission and user gestures override pending initial placement.
    if (IsWindowTrackedInAnyState(hwnd) || IsMoveSizeGestureInProgress(hwnd)) {
      g_wm.windowRoutingEvaluated.insert(hwnd);
      g_wm.pendingWindowRoutes.erase(hwnd);
      continue;
    }
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd) || !IsWindowEnabled(hwnd) || IsHungAppWindow(hwnd)) continue;
    if (!WindowCanBeManaged(hwnd, true)) {
      // Exclusion wins. Ineligible framework HWNDs may become eligible later.
      g_wm.pendingWindowRoutes.erase(hwnd);
      continue;
    }
    DWORD processId = 0;
    GUID sourceDesktop{};
    GetWindowThreadProcessId(hwnd, &processId);
    if (!processId || !GetWindowDesktopIdSafe(hwnd, &sourceDesktop) ||
        IsEqualGUID(sourceDesktop, GUID_NULL)) continue;

    if (!g_wm.pendingWindowRoutes.count(hwnd)) {
      if (g_wm.windowRoutingEvaluated.count(hwnd) || !IsWindow(hwnd)) continue;
      const WindowRule* rule = FindMatchingWindowRule(hwnd, WindowRuleTreatment::InitialPlacement);
      if (g_wm.windowRoutingEvaluated.count(hwnd) || !IsWindow(hwnd)) continue;
      if (!rule) {
        g_wm.windowRoutingEvaluated.insert(hwnd);
        continue;
      }
      // Copy the rule before COM calls, which may deliver lifecycle events.
      WmRuntime::PendingWindowRoute request;
      request.processId = processId;
      request.sourceDesktop = sourceDesktop;
      request.targetDesktop = sourceDesktop;
      request.monitor = rule->monitor;
      request.monitorId = rule->monitorId;
      request.followWindow = rule->followWindow;
      const int desktopNumber = rule->desktopNumber;
      const std::wstring desktopName = rule->desktopNameContains;
      if (desktopNumber || !desktopName.empty()) {
        std::vector<GUID> order;
        if (!Platform::VirtualDesktop::GetDesktopIdsInOrder(&order)) continue;
        request.targetDesktop = GUID_NULL;
        for (size_t i = 0; i < order.size(); ++i) {
          if (desktopNumber && desktopNumber != static_cast<int>(i + 1)) continue;
          if (!desktopName.empty()) {
            const auto metadata = Platform::VirtualDesktop::ReadDesktopMetadata(order[i]);
            if (!ContainsInsensitive(metadata.name, desktopName)) continue;
          }
          request.targetDesktop = order[i];
          break;
        }
      }
      DWORD livePid = 0;
      GUID liveOwner{};
      GetWindowThreadProcessId(hwnd, &livePid);
      if (g_wm.windowRoutingEvaluated.count(hwnd) || !IsWindow(hwnd) || livePid != processId ||
          !GetWindowDesktopIdSafe(hwnd, &liveOwner) || !IsEqualGUID(liveOwner, sourceDesktop)) continue;
      g_wm.windowRoutingEvaluated.insert(hwnd);
      if (IsEqualGUID(request.targetDesktop, GUID_NULL)) {
        Wh_Log(L"Initial placement desktop unavailable for HWND %p", hwnd);
        continue;
      }
      g_wm.pendingWindowRoutes.emplace(hwnd, std::move(request));
    }
    // Never retain map iterators or references across native calls.
    const auto pending = g_wm.pendingWindowRoutes.find(hwnd);
    if (pending == g_wm.pendingWindowRoutes.end()) continue;
    const auto request = pending->second;
    if (request.processId != processId || !IsEqualGUID(request.sourceDesktop, sourceDesktop)) {
      g_wm.pendingWindowRoutes.erase(hwnd); // The app or user relocated it.
      continue;
    }
    GUID currentDesktop{};
    if (!GetCurrentDesktopId(&currentDesktop) || !IsEqualGUID(currentDesktop, sourceDesktop) ||
        IsWindowCloaked(hwnd)) continue;

    const HMONITOR sourceMonitor = GetWindowPhysicalMonitor(hwnd);
    HMONITOR targetMonitor = sourceMonitor;
    AddUniqueMonitor(monitors, sourceMonitor);
    if (request.monitor == RuleMonitor::Specific) {
      Model::MonitorId identity;
      identity.deviceId = request.monitorId;
      targetMonitor = identity.Resolve();
    } else if (request.monitor == RuleMonitor::Primary || request.monitor == RuleMonitor::NonPrimary) {
      auto matchesMonitor = [&](HMONITOR monitor) {
        MONITORINFO info{sizeof(info)};
        return GetMonitorInfoW(monitor, &info) &&
            ((info.dwFlags & MONITORINFOF_PRIMARY) != 0) == (request.monitor == RuleMonitor::Primary);
      };
      if (!matchesMonitor(sourceMonitor)) {
        std::vector<std::pair<std::wstring, HMONITOR>> connected;
        EnumDisplayMonitors(nullptr, nullptr,
            [](HMONITOR monitor, HDC, LPRECT, LPARAM parameter) WINAPI -> BOOL {
              Model::MonitorId identity;
              if (Model::MonitorId::FromHMonitor(monitor, &identity)) {
                reinterpret_cast<std::vector<std::pair<std::wstring, HMONITOR>>*>(parameter)
                    ->emplace_back(identity.deviceId, monitor);
              }
              return TRUE;
            }, reinterpret_cast<LPARAM>(&connected));
        // Stable IDs give a repeatable choice among several secondary monitors.
        std::sort(connected.begin(), connected.end(), [](const auto& first, const auto& second) {
          return first.first < second.first;
        });
        targetMonitor = nullptr;
        for (const auto& candidate : connected) {
          if (matchesMonitor(candidate.second)) { targetMonitor = candidate.second; break; }
        }
      }
    }
    if (!targetMonitor) {
      Wh_Log(L"Initial placement monitor unavailable for HWND %p", hwnd);
      g_wm.pendingWindowRoutes.erase(hwnd);
      continue;
    }
    const bool changeDesktop = !IsEqualGUID(request.targetDesktop, sourceDesktop);
    if (!changeDesktop && targetMonitor == sourceMonitor) {
      // Already at its destination: normal suspended admission must still count
      // maximized windows, without waiting for an unnecessary physical move.
      g_wm.pendingWindowRoutes.erase(hwnd);
      continue;
    }
    const SuspensionReason suspensionReason = GetPhysicalSuspensionReason(hwnd);
    if (suspensionReason != SuspensionReason::None) {
      const auto pending = g_wm.pendingWindowRoutes.find(hwnd);
      if (pending != g_wm.pendingWindowRoutes.end()) {
        if (suspensionReason == SuspensionReason::Maximized) {
          if (!pending->second.visibleGeneration) pending->second.visibleGeneration = Model::NextTilingGeneration();
          AddPendingWorkspaceArrange(sourceDesktop, sourceMonitor);
        } else {
          pending->second.visibleGeneration = 0;
        }
      }
      continue;
    }
    // A frozen GUID must still exist; desktop reordering must not retarget it.
    if (changeDesktop) {
      std::vector<GUID> order;
      if (!Platform::VirtualDesktop::GetDesktopIdsInOrder(&order)) continue;
      if (std::none_of(order.begin(), order.end(), [&](const GUID& id) {
            return IsEqualGUID(id, request.targetDesktop);
          })) {
        g_wm.pendingWindowRoutes.erase(hwnd);
        continue;
      }
    }
    auto canRoute = [&] {
      const auto pending = g_wm.pendingWindowRoutes.find(hwnd);
      if (pending == g_wm.pendingWindowRoutes.end() || pending->second.processId != processId) return false;
      DWORD livePid = 0;
      GUID owner{}, current{};
      GetWindowThreadProcessId(hwnd, &livePid);
      return IsWindow(hwnd) && livePid == processId && !IsWindowTrackedInAnyState(hwnd) &&
          !IsMoveSizeGestureInProgress(hwnd) && !IsWindowCloaked(hwnd) &&
          GetPhysicalSuspensionReason(hwnd) == SuspensionReason::None &&
          GetWindowDesktopIdSafe(hwnd, &owner) && IsEqualGUID(owner, sourceDesktop) &&
          GetCurrentDesktopId(&current) && IsEqualGUID(current, sourceDesktop) &&
          g_wm.pendingWindowRoutes.count(hwnd);
    };
    LASTINPUTINFO input{sizeof(input)};
    if (!GetLastInputInfo(&input) || !canRoute()) continue;
    RECT sourceFrame{};
    if (targetMonitor != sourceMonitor) {
      RECT workArea{};
      if (!GetWindowFrameRect(hwnd, &sourceFrame) || RectWidth(sourceFrame) <= 0 ||
          RectHeight(sourceFrame) <= 0 || !GetWorkspaceWorkArea(targetMonitor, &workArea)) {
        g_wm.pendingWindowRoutes.erase(hwnd);
        continue;
      }
      const RECT target = CenteredRect(RectCenter(workArea), RectWidth(sourceFrame), RectHeight(sourceFrame));
      const auto placed = PlaceWindowChecked(hwnd, true, target, true);
      if (placed.result == PlacementResult::SuppressedByPhysicalState) continue;
      AddUniqueMonitor(monitors, GetWindowPhysicalMonitor(hwnd));
      if (placed.result != PlacementResult::Success || GetWindowPhysicalMonitor(hwnd) != targetMonitor) {
        Diagnostics::RecordEvent(L"initial placement monitor refused hwnd=%p target=%p result=%d",
                                 hwnd, targetMonitor, static_cast<int>(placed.result));
        g_wm.pendingWindowRoutes.erase(hwnd);
        continue;
      }
    }
    if (!canRoute()) continue;
    if (changeDesktop && !Platform::VirtualDesktop::MoveWindowToDesktop(
            hwnd, processId, sourceDesktop, request.targetDesktop,
            Platform::VirtualDesktop::WindowMovePurpose::InitialPlacement)) {
      // Undo only our monitor move, and only while the original request is live.
      LASTINPUTINFO latestInput{sizeof(latestInput)};
      if (targetMonitor != sourceMonitor && GetLastInputInfo(&latestInput) &&
          latestInput.dwTime == input.dwTime && canRoute()) {
        PlaceWindowChecked(hwnd, true, sourceFrame, true);
      }
      g_wm.pendingWindowRoutes.erase(hwnd);
      continue;
    }
    DWORD livePid = 0;
    GUID owner{};
    GetWindowThreadProcessId(hwnd, &livePid);
    if (!GetWindowDesktopIdSafe(hwnd, &owner) || !IsEqualGUID(owner, request.targetDesktop)) continue;
    GetWindowThreadProcessId(hwnd, &livePid);
    if (!IsWindow(hwnd) || !g_wm.pendingWindowRoutes.count(hwnd) || livePid != processId) continue;
    g_wm.pendingWindowRoutes.erase(hwnd); // Overflow must never reapply this rule.
    AddUniqueMonitor(monitors, targetMonitor);
    if (changeDesktop) {
      DesktopMonitorKey targetKey{};
      Workspace workspace;
      if (DesktopMonitorKey::FromHMonitor(request.targetDesktop, targetMonitor, &targetKey) &&
          !g_workspaces.Load(targetKey, &workspace)) {
        g_workspaces.Save(targetKey, MakeDefaultWorkspace(targetKey));
      }
      // Off-desktop windows stay unadmitted until the ordinary destination pass.
      // That pass enforces fit/limits, including when several windows arrive together.
      AddPendingWorkspaceArrange(request.targetDesktop, targetMonitor);
      if (request.followWindow && (!g_wm.pendingInitialPlacementSwitch.hwnd || hwnd == foregroundWindow)) {
        g_wm.pendingInitialPlacementSwitch =
            {hwnd, processId, sourceDesktop, request.targetDesktop, {}, 0, input.dwTime};
      }
      ScheduleLifecycleReconcile(nullptr);
    }
    Diagnostics::RecordEvent(L"initial placement hwnd=%p targetDesktop=%08X monitor=%p follow=%d",
                             hwnd, request.targetDesktop.Data1, targetMonitor, request.followWindow ? 1 : 0);
  }
}

// Switch only after snapshots have finished. No forced HWND activation is needed.
static void FollowInitiallyPlacedWindow() {
  const PendingOverflowSwitch pending = g_wm.pendingInitialPlacementSwitch;
  if (!pending.hwnd) return;
  if (Platform::VirtualDesktop::SwitchToDesktop(
          pending.hwnd, pending.processId, pending.targetDesktop, pending.sourceDesktop, pending.inputTickMs,
          Platform::VirtualDesktop::WindowMovePurpose::InitialPlacement)) {
    g_wm.reconciledDesktopState = ReconciledDesktopState::TransitionPending;
    ScheduleLifecycleReconcile(nullptr);
  }
  g_wm.pendingInitialPlacementSwitch = {};
}

// Build only the extra inactive candidates implicated by this settled lifecycle
// burst. This deliberately does not add another EnumWindows pass: normal restored
// discovery still comes from the one-pass per-monitor snapshot, while newly
// observed minimized/maximized HWNDs are classified from the already-dirty set.
static std::vector<std::vector<HWND>> CollectDirtySuspendedWindowsForMonitors(
    const std::vector<HWND>& dirtyWindows,
    const std::vector<HMONITOR>& monitors,
    const GUID& desktopId) {
  std::vector<std::vector<HWND>> snapshots(monitors.size());
  if (!IsAutomaticMode() || dirtyWindows.empty() || monitors.empty()) {
    return snapshots;
  }

  for (HWND hwnd : dirtyWindows) {
    if (!hwnd || !IsWindow(hwnd) || IsWindowTrackedInAnyState(hwnd)) continue;
    if (!IsWindowVisible(hwnd) || !WindowCanBeManaged(hwnd) || !IsWindowEnabled(hwnd)) {
      continue;
    }

    const SuspensionReason reason = GetPhysicalSuspensionReason(hwnd);
    if (reason != SuspensionReason::Minimized &&
        reason != SuspensionReason::Maximized) {
      continue;
    }

    GUID windowDesktop{};
    if (!GetWindowDesktopIdSafe(hwnd, &windowDesktop) ||
        !IsEqualGUID(windowDesktop, desktopId)) {
      continue;
    }

    const HMONITOR monitor = GetWindowPhysicalMonitor(hwnd);
    auto it = std::find(monitors.begin(), monitors.end(), monitor);
    if (it == monitors.end()) continue;

    snapshots[static_cast<size_t>(it - monitors.begin())].push_back(hwnd);
  }

  return snapshots;
}

// Current-desktop discovery has two separate jobs: reconcile already-managed
// HWND ownership according to the active management policy, and optionally admit
// truly new windows while Automatic mode is active.
static bool DiscoverWorkspaceWindows(
    HMONITOR monitor, const GUID& desktopId, const std::vector<HWND>& snapshot,
    DesktopMonitorKey* outKey,
    std::vector<DesktopMonitorKey>* ownershipChangedKeys = nullptr,
    const std::vector<HWND>* suspendedSnapshot = nullptr) {
  if (!IsLiveMonitorHandle(monitor)) return false;

  DesktopMonitorKey key{};
  if (!DesktopMonitorKey::FromHMonitor(desktopId, monitor, &key)) return false;
  if (outKey) *outKey = key;

  Workspace workspace;
  const bool workspaceExisted = g_workspaces.Load(key, &workspace);
  const bool admitUntracked = IsAutomaticMode();
  const bool initializing = !workspaceExisted && admitUntracked;
  const std::vector<HWND> initialSnapshot = initializing
      ? CollectWorkspaceWindowsForInitialization(monitor, desktopId)
      : std::vector<HWND>{};

  bool changed = ReconcileTrackedSnapshotOwnership(
      key, initializing ? initialSnapshot : snapshot, ownershipChangedKeys);

  bool hasState = g_workspaces.Load(key, &workspace);
  if (!hasState) workspace = MakeDefaultWorkspace(key);

  if (initializing &&
      SeedInitialSnapshot(workspace, initialSnapshot, true)) {
    hasState = true;
    changed = true;
  }
  changed |= RestoreSuspendedSnapshot(workspace, snapshot);

  std::vector<HWND> newlyAdmittedWindows;
  if (AdmitUntrackedSnapshot(
          workspace, snapshot, admitUntracked, true,
          &newlyAdmittedWindows)) {
    hasState = true;
    changed = true;
  }
  if (suspendedSnapshot &&
      AdmitUntrackedSuspendedSnapshot(
          workspace, *suspendedSnapshot, admitUntracked, true)) {
    hasState = true;
    changed = true;
  }

  // A default-floating HWND can be admitted during initial seeding or while it
  // is suspended. Once it appears in the active snapshot, route the retained
  // centering request through the same placement pass.
  for (HWND hwnd : snapshot) {
    const WindowRecord* record = workspace.Find(hwnd);
    if (record && record->state == ManageState::Floating &&
        record->pendingDefaultFloatingCenter &&
        !ContainsWindow(newlyAdmittedWindows, hwnd)) {
      newlyAdmittedWindows.push_back(hwnd);
    }
  }

  // Automatic discovery owns the first floating geometry of a genuinely new
  // restored window. A rule-floated admission always keeps its application-
  // selected size, including when the whole workspace is already Floating.
  // Other Floating-workspace admissions use the configured sizing policy.
  if (admitUntracked) {
    for (HWND hwnd : newlyAdmittedWindows) {
      const WindowRecord* record = workspace.Find(hwnd);
      const bool pendingDefaultCenter =
          record && record->state == ManageState::Floating &&
          record->pendingDefaultFloatingCenter;
      if (!record ||
          (workspace.Layout() != TileLayout::Floating &&
           !pendingDefaultCenter)) {
        continue;
      }

      FloatingPlacementHint newWindowHint;
      newWindowHint.intent =
          pendingDefaultCenter
              ? FloatingPlacementIntent::CenterPreservingSize
              : FloatingPlacementIntent::NewWindowCenter;
      if (RepairFloatingGeometry(key, workspace, hwnd, newWindowHint)) {
        workspace.ClearDefaultFloatingCenter(hwnd);
        changed = true;
      }
    }
  }

  // Selection used the retained pre-admission focus. Once membership is stable,
  // let a newly admitted foreground HWND become the next retained focus anchor.
  HWND foreground = GetForegroundWindow();
  if (foreground && workspace.HasRecord(foreground)) {
    workspace.RememberFocusedWindow(foreground);
  }

  if (!hasState || !changed) return false;
  g_workspaces.Save(key, workspace);
  return true;
}

// Runs the normal discovery path for one monitor. The pipeline is always active;
// DiscoverWorkspaceWindows() alone decides whether unknown HWNDs may be admitted;
// Manual-mode retention decisions are handled by the reconciliation policy.
static bool DiscoverCurrentWorkspaceOnMonitor(HMONITOR monitor) {
  if (!monitor) return false;

  GUID currentDesktop{};
  if (!InitializeVirtualDesktopAPI() || !GetCurrentDesktopId(&currentDesktop)) {
    return false;
  }

  std::vector<HMONITOR> monitors{monitor};
  RouteNewWindows(monitors);
  const auto snapshots = CollectTileWindowsForMonitors(monitors);
  bool arranged = false;
  for (size_t i = 0; i < monitors.size(); ++i) {
    DesktopMonitorKey key{};
    if (!DesktopMonitorKey::FromHMonitor(currentDesktop, monitors[i], &key)) continue;
    const bool changed =
        DiscoverWorkspaceWindows(monitors[i], currentDesktop, snapshots[i], &key);

    // An unchanged workspace may still need its existing layout reinforced.
    Workspace existing;
    if (!changed && !g_workspaces.Load(key, &existing)) continue;
    ArrangeWorkspace(key);
    arranged = true;
  }
  return arranged;
}

static void EnumerateCurrentMonitors(std::vector<HMONITOR>& monitors) {
  // This helper means "current OS topology", not "merge more monitor handles".
  monitors.clear();
  EnumDisplayMonitors(
      nullptr, nullptr,
      [](HMONITOR monitor, HDC, LPRECT, LPARAM lParam) WINAPI -> BOOL {
        auto* list = reinterpret_cast<std::vector<HMONITOR>*>(lParam);
        AddUniqueMonitor(*list, monitor);
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&monitors));
}

// During a virtual-desktop switch, DWM cloaks and uncloaks windows one at a time.
// Snapshot otherwise eligible windows by desktop GUID while deliberately ignoring
// that transient cloak bit. Known membership therefore survives the transition;
// the admission boundary defers unknown cloaked HWNDs to the settled pass.
static std::vector<HWND> GetTileWindowsAfterDesktopSwitch(
    HMONITOR monitor, const GUID& desktopId) {
  std::vector<HWND> windows;
  if (!monitor) return windows;

  struct EnumContext {
    HMONITOR targetMonitor;
    GUID desktopId;
    std::vector<HWND>* windowList;
  } context = {monitor, desktopId, &windows};

  ++Diagnostics::g_runtime.counters.enumWindowsPasses;
  EnumWindows(
      [](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        ++Diagnostics::g_runtime.counters.enumWindowsVisited;
        auto* ctx = reinterpret_cast<EnumContext*>(lParam);

        if (GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None) return TRUE;
        if (!WindowCanBeManaged(hwnd)) return TRUE;
        if (!IsWindowTrackedInAnyState(hwnd) && !IsWindowEnabled(hwnd)) return TRUE;

        GUID windowDesktop{};
        if (!GetWindowDesktopIdSafe(hwnd, &windowDesktop) ||
            !IsEqualGUID(windowDesktop, ctx->desktopId)) {
          return TRUE;
        }

        if (GetWindowPhysicalMonitor(hwnd) == ctx->targetMonitor) {
          ctx->windowList->push_back(hwnd);
          ++Diagnostics::g_runtime.counters.tileCandidatesCollected;
        }
        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return windows;
}

// Native CurrentVirtualDesktopChanged is the authoritative workspace transition
// signal. The cloak-insensitive snapshot is used only to discover new incoming
// windows; known WindowRecord membership is never rebuilt from shell visibility.
static void HandleVirtualDesktopChanged() {
  InterlockedExchange(&g_vd.changeQueued, 0);
  ++Diagnostics::g_runtime.counters.virtualDesktopChangesProcessed;

  GUID currentDesktop{};
  if (!InitializeVirtualDesktopAPI() || !GetCurrentDesktopId(&currentDesktop)) {
    // The native callback is an authoritative transition signal, but without a
    // current GUID we cannot prove that an existing placement lease still belongs
    // to the active desktop. Drop those short-lived contracts and retry normally.
    ClearAllConformanceLeases();
    ScheduleNextConformanceTimer();
    Wh_Log(L"Native VD change received but current desktop query failed");
    Diagnostics::RecordEvent(L"virtual desktop notification: current desktop query failed");
    ScheduleLifecycleReconcile(nullptr);
    return;
  }

  Diagnostics::RecordEvent(
      L"virtual desktop changed currentDesktop=%08X", currentDesktop.Data1);
  const bool desktopActuallyChanged =
      g_wm.reconciledDesktopState == ReconciledDesktopState::Unknown ||
      !IsEqualGUID(g_wm.reconciledDesktop, currentDesktop);
  if (desktopActuallyChanged) {
    // A lease describes post-placement behavior on one active desktop. Never carry
    // it across DWM's cloak/uncloak transition; the incoming settled arrange pass
    // will establish fresh leases for any placements it actually has to issue.
    ClearAllConformanceLeases();
    ScheduleNextConformanceTimer();
  }
  g_wm.reconciledDesktop = currentDesktop;
  g_wm.reconciledDesktopState = ReconciledDesktopState::Settled;
  // The visual transition belongs to the settled desktop state, not the raw COM
  // callback. Coalesce rapid switches and emit one monitor-local OSD per connected
  // display after reconciliation has finished. Duplicate callbacks for the already
  // represented desktop do not generate another visual transition.
  if (desktopActuallyChanged) g_wm.pendingDesktopSwitchFlyouts = true;

  // A native VD transition is an authoritative reason to repair desktop ownership.
  // Do not infer monitor migration here; defer visibility/window-state participation
  // until the shell's cloak/show burst has settled.
  std::vector<DesktopMonitorKey> changedKeys;
  ReconcileKnownWindows(ReconcileScope::Desktop, &changedKeys);
  for (const auto& key : changedKeys) {
    if (IsEqualGUID(key.desktopId, currentDesktop)) {
      AddPendingWorkspaceArrange(key.desktopId, key.ResolveMonitor());
    }
  }

  std::vector<HMONITOR> monitors;
  EnumerateCurrentMonitors(monitors);

  // FancyWM refreshes ownership and then unconditionally invalidates the incoming
  // desktop layout. Do the equivalent here, but defer actual placement until the
  // shell's cloak/uncloak burst settles. Existing workspaces are reinforced even
  // when their bookkeeping did not change.
  for (HMONITOR monitor : monitors) {
    DesktopMonitorKey knownKey{};
    if (!DesktopMonitorKey::FromHMonitor(currentDesktop, monitor, &knownKey)) continue;
    Workspace knownState;
    if (g_workspaces.Load(knownKey, &knownState)) {
      AddPendingWorkspaceArrange(currentDesktop, monitor);
    }
  }

  for (HMONITOR monitor : monitors) {
    std::vector<HWND> snapshot = GetTileWindowsAfterDesktopSwitch(monitor, currentDesktop);
    DesktopMonitorKey key{};
    if (DiscoverWorkspaceWindows(
            monitor, currentDesktop, snapshot, &key)) {
      // New/migrated membership is also reinforced by the same settled pass.
      AddPendingWorkspaceArrange(currentDesktop, monitor);
    }
  }

  // Coalesce the remaining shell burst for discovery/fallback and the unconditional
  // incoming-desktop reinforcement. The settled pass may also repair physical state.
  ScheduleLifecycleReconcile(nullptr);
}

// Coalesce shell visibility bursts for discovery and deferred placement. Generic
// lifecycle events are dirty signals: after the settle delay we may re-query
// desktop ownership for the affected tracked HWND, but never infer ownership
// directly from HIDE/SHOW/CLOAK itself.
static void ScheduleLifecycleReconcile(HWND hwnd) {
  ++Diagnostics::g_runtime.counters.lifecycleScheduleRequests;
  // HotkeyThreadProc owns this vector/timer; no additional lock is required.
  if (hwnd && std::find(g_wm.lifecycleDirtyWindows.begin(), g_wm.lifecycleDirtyWindows.end(), hwnd) ==
                  g_wm.lifecycleDirtyWindows.end()) {
    g_wm.lifecycleDirtyWindows.push_back(hwnd);
  }

  if (g_wm.lifecycleTimer) {
    ++Diagnostics::g_runtime.counters.lifecycleTimerResets;
    KillTimer(nullptr, g_wm.lifecycleTimer);
    g_wm.lifecycleTimer = 0;
  }

  g_wm.lifecycleTimer = SetTimer(nullptr, 0, g_settings.reconcileDelayMs, nullptr);
  if (!g_wm.lifecycleTimer) {
    Wh_Log(L"Failed to schedule lifecycle timer; queueing immediate reconciliation");
    const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
    if (threadId) {
      PostThreadMessage(threadId, WM_APP_RECONCILE_NOW, 0, 0);
    }
  }
}

static void ScheduleNextDiscoveryRetryTimer() {
  if (g_wm.discoveryRetryTimer) {
    KillTimer(nullptr, g_wm.discoveryRetryTimer);
    g_wm.discoveryRetryTimer = 0;
  }
  if (g_wm.pendingDiscoveryRetries.empty()) return;

  const ULONGLONG now = GetTickCount64();
  ULONGLONG earliestDue = g_wm.pendingDiscoveryRetries.front().dueTickMs;
  for (const auto& pending : g_wm.pendingDiscoveryRetries) {
    earliestDue = std::min(earliestDue, pending.dueTickMs);
  }
  const UINT delayMs = earliestDue > now
                           ? static_cast<UINT>(earliestDue - now)
                           : 1;
  g_wm.discoveryRetryTimer = SetTimer(nullptr, 0, delayMs, nullptr);
  if (!g_wm.discoveryRetryTimer) {
    Wh_Log(L"Failed to schedule slow-application discovery timer");
    g_wm.pendingDiscoveryRetries.clear();
  }
}

// This stage owns only untracked HWNDs. Once discovery succeeds,
// ArrangeWorkspace establishes any needed post-placement conformance lease.
static void ProcessDiscoveryRetryTimer() {
  AssertWmThread(L"ProcessDiscoveryRetryTimer");
  if (g_wm.discoveryRetryTimer) {
    KillTimer(nullptr, g_wm.discoveryRetryTimer);
    g_wm.discoveryRetryTimer = 0;
  }
  if (!IsAutomaticMode() && !g_wm.windowRoutingReady) {
    g_wm.pendingDiscoveryRetries.clear();
    return;
  }

  const ULONGLONG now = GetTickCount64();
  std::vector<PendingDiscoveryRetry> due;
  g_wm.pendingDiscoveryRetries.erase(
      std::remove_if(
          g_wm.pendingDiscoveryRetries.begin(),
          g_wm.pendingDiscoveryRetries.end(),
          [&](const PendingDiscoveryRetry& pending) {
            if (pending.dueTickMs > now) return false;
            due.push_back(pending);
            return true;
          }),
      g_wm.pendingDiscoveryRetries.end());
  ScheduleNextDiscoveryRetryTimer();

  std::vector<HMONITOR> monitors;
  for (const auto& pending : due) {
    if (!pending.hwnd || !IsWindow(pending.hwnd) ||
        IsWindowTrackedInAnyState(pending.hwnd)) {
      continue;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(pending.hwnd, &processId);
    if (!processId || processId != pending.processId) continue;
    AddUniqueMonitor(monitors,
                     GetCurrentManagedWindowMonitor(pending.hwnd));
  }
  RouteNewWindows(monitors);
  for (HMONITOR monitor : monitors) DiscoverCurrentWorkspaceOnMonitor(monitor);
  if (!g_wm.pendingWindowOverflow.empty()) ScheduleLifecycleReconcile(nullptr);
}

// Overflow work runs only after admission/arrangement has saved every source.
// Trial workspaces never change native geometry or evict destination members.
static void ProcessWindowOverflow() {
  if (g_wm.overflowTimer) {
    KillTimer(nullptr, g_wm.overflowTimer);
    g_wm.overflowTimer = 0;
  }
  if (!IsAutomaticMode()) {
    g_wm.pendingWindowOverflow.clear();
    g_wm.pendingOverflowSwitch = {};
    return;
  }

  if (g_wm.pendingWindowOverflow.empty() && !g_wm.pendingOverflowSwitch.hwnd) return;
  const ULONGLONG now = GetTickCount64();
  GUID currentDesktop{};
  const bool desktopKnown = GetCurrentDesktopId(&currentDesktop);
  bool waitForSwitch = false;
  PendingOverflowSwitch transition = g_wm.pendingOverflowSwitch;

  // Keep the repository aligned with the observed result even if a native move
  // or its rollback fails. Shell identity, never our intended target, wins.
  auto reconcileObservedOwner = [](HWND hwnd) {
    GUID observedDesktop{};
    DesktopMonitorKey observedKey{};
    if (GetWindowDesktopIdSafe(hwnd, &observedDesktop) &&
        DesktopMonitorKey::FromHMonitor(observedDesktop, GetWindowPhysicalMonitor(hwnd), &observedKey)) {
      std::vector<DesktopMonitorKey> changed;
      EnsureUniqueWindowOwnership(hwnd, observedKey, &changed);
      for (const auto& key : changed) ArrangeWorkspace(key);
    }
  };

  auto rollbackOverflowMove = [&](const PendingOverflowSwitch& pending) {
    GUID observed{};
    DWORD processId = 0;
    GetWindowThreadProcessId(pending.hwnd, &processId);
    if (!IsWindow(pending.hwnd) || processId != pending.processId ||
        !g_wm.overflowWindows.count(pending.hwnd)) {
      Platform::VirtualDesktop::RemoveEmptyOverflowDesktop(pending.targetDesktop, pending.sourceDesktop);
      return;
    }
    if (!GetWindowDesktopIdSafe(pending.hwnd, &observed) ||
        (!IsEqualGUID(observed, pending.sourceDesktop) &&
         !IsEqualGUID(observed, pending.targetDesktop))) {
      reconcileObservedOwner(pending.hwnd);
      Platform::VirtualDesktop::RemoveEmptyOverflowDesktop(pending.targetDesktop, pending.sourceDesktop);
      return;
    }
    GUID liveCurrent{};
    LASTINPUTINFO input{sizeof(input)};
    if (!GetCurrentDesktopId(&liveCurrent) || !IsEqualGUID(liveCurrent, pending.sourceDesktop) ||
        !GetLastInputInfo(&input) || input.dwTime != pending.inputTickMs ||
        IsMoveSizeGestureInProgress(pending.hwnd)) {
      reconcileObservedOwner(pending.hwnd);
      Platform::VirtualDesktop::RemoveEmptyOverflowDesktop(pending.targetDesktop, pending.sourceDesktop);
      return;
    }
    const bool alreadyHome = IsEqualGUID(observed, pending.sourceDesktop);
    const bool returned = alreadyHome || Platform::VirtualDesktop::MoveWindowToDesktop(
        pending.hwnd, pending.processId, pending.targetDesktop, pending.sourceDesktop);
    if (returned && GetWindowDesktopIdSafe(pending.hwnd, &observed) &&
        IsEqualGUID(observed, pending.sourceDesktop)) {
      PlaceWindowChecked(pending.hwnd, true, pending.sourceFrame);
    }
    reconcileObservedOwner(pending.hwnd);
    // A failed native follow uses the configured floating fallback, including
    // when rollback cannot return the window home.
    for (const auto& key : g_workspaces.OwnersOf(pending.hwnd)) {
      Workspace workspace;
      if (g_workspaces.Load(key, &workspace) && workspace.Float(pending.hwnd)) {
        g_workspaces.Save(key, workspace);
        ArrangeWorkspace(key);
      }
    }
    Platform::VirtualDesktop::RemoveEmptyOverflowDesktop(pending.targetDesktop, pending.sourceDesktop);
    Diagnostics::RecordEvent(L"overflow rollback hwnd=%p returned=%d", pending.hwnd, returned ? 1 : 0);
    if (!returned) TrayUi::ShowCommandFailureFlyout(L"Could not return the window to its desktop.");
  };

  if (transition.hwnd) {
    DWORD processId = 0;
    LASTINPUTINFO input{sizeof(input)};
    GetWindowThreadProcessId(transition.hwnd, &processId);
    GUID windowDesktop{};
    if (!IsWindow(transition.hwnd) || processId != transition.processId ||
        !g_wm.overflowWindows.count(transition.hwnd) ||
        !GetLastInputInfo(&input) || input.dwTime != transition.inputTickMs ||
        !GetWindowDesktopIdSafe(transition.hwnd, &windowDesktop) ||
        !IsEqualGUID(windowDesktop, transition.targetDesktop)) {
      transition = {}; // User input or a changed lifetime cancels the follow.
    } else if (desktopKnown && IsEqualGUID(currentDesktop, transition.targetDesktop)) {
      // Desktop arrival is enough. Let the shell reveal and focus application
      // views; neither a cloaking sample nor SetForegroundWindow proves readiness.
      Diagnostics::RecordEvent(L"overflow desktop arrived target=%08X hwnd=%p",
                               transition.targetDesktop.Data1, transition.hwnd);
      ScheduleLifecycleReconcile(transition.hwnd);
      transition = {};
    } else if ((desktopKnown && !IsEqualGUID(currentDesktop, transition.sourceDesktop)) ||
               now >= transition.deadlineTickMs) {
      const PendingOverflowSwitch failed = transition;
      transition = {};
      // A user-initiated desktop change should never be reversed.
      if (desktopKnown && IsEqualGUID(currentDesktop, failed.sourceDesktop)) {
        rollbackOverflowMove(failed);
      }
    } else {
      waitForSwitch = true;
    }
  }
  g_wm.pendingOverflowSwitch = transition;

  std::vector<PendingWindowOverflow> pending;
  pending.swap(g_wm.pendingWindowOverflow);
  const HWND foreground = GetForegroundWindow();
  LASTINPUTINFO batchInput{sizeof(batchInput)};
  GetLastInputInfo(&batchInput);
  PendingOverflowSwitch follow{};
  for (PendingWindowOverflow request : pending) {
    const HWND hwnd = request.hwnd;
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (!IsWindow(hwnd) || !processId || processId != request.processId ||
        !g_wm.overflowWindows.count(hwnd) || IsMoveSizeGestureInProgress(hwnd)) continue;
    GUID observedDesktop{};
    DesktopMonitorKey sourceKey{};
    const HMONITOR sourceMonitor = GetWindowPhysicalMonitor(hwnd);
    if (!desktopKnown || !GetWindowDesktopIdSafe(hwnd, &observedDesktop) ||
        !DesktopMonitorKey::FromHMonitor(observedDesktop, sourceMonitor, &sourceKey)) {
      g_wm.pendingWindowOverflow.push_back(std::move(request));
      continue;
    }
    if (!IsEqualGUID(observedDesktop, request.sourceDesktop) ||
        sourceKey.monitor.deviceId != request.sourceMonitorId) {
      // An app or the shell can relocate a queued window. Preserve its overflow
      // intent while rebasing ownership to the observed desktop and monitor.
      reconcileObservedOwner(hwnd);
      request.sourceDesktop = observedDesktop;
      request.sourceMonitorId = sourceKey.monitor.deviceId;
    }
    Workspace source;
    if (!g_workspaces.Load(sourceKey, &source)) continue;
    const WindowRecord* sourceRecord = source.Find(hwnd);
    if (!sourceRecord || sourceRecord->pid != processId ||
        sourceRecord->tilingGeneration != request.tilingGeneration ||
        !g_wm.overflowWindows.count(hwnd) ||
        sourceRecord->state != ManageState::Floating || sourceRecord->alwaysFloating ||
        source.Layout() == TileLayout::Floating || IsWindowExcludedByRules(hwnd) ||
        ShouldWindowAlwaysFloat(hwnd)) continue;
    if (!IsEqualGUID(currentDesktop, request.sourceDesktop) ||
        GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None || IsWindowCloaked(hwnd)) {
      // Wait for desktop arrival or restoration rather than polling hidden apps.
      g_wm.pendingWindowOverflow.push_back(std::move(request));
      continue;
    }
    RECT sourceFrame{};
    if (!WindowCanBeManaged(hwnd) || IsHungAppWindow(hwnd) ||
        !GetWindowFrameRect(hwnd, &sourceFrame)) {
      g_wm.pendingWindowOverflow.push_back(std::move(request));
      continue;
    }

    WindowRecord incoming = *sourceRecord;
    incoming.state = ManageState::Tiled;
    incoming.pendingDefaultFloatingCenter = false;
    incoming.constraints = ProbeWindowConstraints(hwnd);
    if (!incoming.constraints.valid || !incoming.constraints.dpi) {
      // A missing startup constraint report is not evidence that the app cannot
      // fit. Retry on the next lifecycle event without losing the routing request.
      Diagnostics::RecordEvent(L"overflow deferred hwnd=%p reason=constraints unavailable", hwnd);
      g_wm.pendingWindowOverflow.push_back(std::move(request));
      continue;
    }

    std::vector<HMONITOR> monitors{sourceMonitor};
    if (g_settings.overflowTryOtherMonitors && !request.monitorFixed) {
      std::vector<HMONITOR> connected;
      EnumerateCurrentMonitors(connected);
      MONITORINFO sourceInfo{sizeof(sourceInfo)};
      GetMonitorInfoW(sourceMonitor, &sourceInfo);
      auto distance = [&](HMONITOR monitor) {
        MONITORINFO info{sizeof(info)};
        if (!GetMonitorInfoW(monitor, &info)) return LLONG_MAX;
        const long long dx = static_cast<long long>(info.rcMonitor.left) + info.rcMonitor.right -
                             sourceInfo.rcMonitor.left - sourceInfo.rcMonitor.right;
        const long long dy = static_cast<long long>(info.rcMonitor.top) + info.rcMonitor.bottom -
                             sourceInfo.rcMonitor.top - sourceInfo.rcMonitor.bottom;
        return dx * dx + dy * dy;
      };
      std::stable_sort(connected.begin(), connected.end(),
                       [&](HMONITOR first, HMONITOR second) { return distance(first) < distance(second); });
      for (HMONITOR monitor : connected) AddUniqueMonitor(monitors, monitor);
    }

    // Existing desktop occupancy is irrelevant: only try monitors on the source,
    // then create one new desktop after proving its initial layout can fit.
    auto preflight = [&](const DesktopMonitorKey& key, Workspace* trial, RECT* incomingRect,
                         const Workspace* initial = nullptr) {
      const HMONITOR monitor = key.ResolveMonitor();
      WorkspaceMetrics metrics;
      if (!monitor || !GetWorkspaceMetrics(monitor, &metrics)) return false;
      if (initial) *trial = *initial;
      else if (!g_workspaces.Load(key, trial)) *trial = MakeDefaultWorkspace(key);
      trial->Forget(hwnd);
      if (trial->Layout() == TileLayout::Floating) {
        if (!initial) return false; // Another monitor must have room to tile.
        WindowRecord record = incoming;
        record.pendingInsertionPlacement = false;
        trial->AdmitFloating(std::move(record));
        *incomingRect = sourceFrame;
        return true;
      }
      if (!incoming.constraints.valid || !incoming.constraints.dpi) return false;
      const auto oldRecords = trial->Records();
      for (const auto& member : oldRecords) {
        GUID owner{};
        DWORD livePid = 0;
        GetWindowThreadProcessId(member.first, &livePid);
        if (!IsWindow(member.first) || livePid != member.second.pid) {
          trial->Forget(member.first);
        } else if (!GetWindowDesktopIdSafe(member.first, &owner)) {
          return false;
        } else if (!IsEqualGUID(owner, key.desktopId) || GetWindowPhysicalMonitor(member.first) != monitor) {
          trial->Forget(member.first);
        } else {
          SuspensionReason reason = GetPhysicalSuspensionReason(member.first);
          if (reason == SuspensionReason::Maximized && !IsZoomed(member.first) &&
              member.second.state == ManageState::Tiled) {
            RECT authoritative{}, current{};
            if (GetCurrentAuthoritativeTiledRect(key, *trial, member.first, &authoritative) &&
                GetWindowFrameRect(member.first, &current) && RectsNear(current, authoritative)) {
              reason = SuspensionReason::None;
            }
          }
          if (member.second.state == ManageState::Tiled && reason != SuspensionReason::None) {
            trial->Suspend(member.first, reason);
          } else if (member.second.state == ManageState::Suspended) {
            if (reason == SuspensionReason::None) trial->Restore(member.first);
            else trial->Suspend(member.first, reason);
          }
        }
      }
      struct CandidateEnumeration {
        const DesktopMonitorKey* key;
        HMONITOR monitor;
        Workspace* trial;
        HWND incoming;
        bool complete = true;
      } context{&key, monitor, trial, hwnd};
      ++Diagnostics::g_runtime.counters.enumWindowsPasses;
      const BOOL enumerated = initial ? TRUE : EnumWindows([](HWND candidate, LPARAM parameter) WINAPI -> BOOL {
        ++Diagnostics::g_runtime.counters.enumWindowsVisited;
        auto& context = *reinterpret_cast<CandidateEnumeration*>(parameter);
        if (candidate == context.incoming || !IsWindowVisible(candidate) ||
            !WindowCanBeManaged(candidate) || GetWindowPhysicalMonitor(candidate) != context.monitor ||
            (!IsWindowTrackedInAnyState(candidate) && !IsWindowEnabled(candidate))) return TRUE;
        DWORD cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(candidate, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
          context.complete = false;
          return TRUE;
        }
        if (cloaked) return TRUE; // Only source-desktop monitors are scanned.
        GUID desktop{};
        if (!GetWindowDesktopIdSafe(candidate, &desktop)) {
          context.complete = false;
          return TRUE;
        }
        if (!IsEqualGUID(desktop, context.key->desktopId)) return TRUE;
        if (context.trial->HasRecord(candidate)) return TRUE;
        // A stale owner must be reconciled before importing this destination;
        // copying it here would duplicate ownership or lose its floating policy.
        if (IsWindowTrackedInAnyState(candidate) || IsWindowAwaitingInitialPlacement(candidate)) {
          context.complete = false;
          return TRUE;
        }
        bool alwaysFloating = false;
        if (ShouldWindowStartFloating(candidate, &alwaysFloating)) {
          WindowRecord record = MakeWindowRecord(candidate, ManageState::Floating);
          record.alwaysFloating = alwaysFloating;
          context.trial->AdmitFloating(std::move(record));
        } else {
          context.trial->AdmitInitial(MakeWindowRecord(candidate), GetPhysicalSuspensionReason(candidate));
        }
        return TRUE;
      }, reinterpret_cast<LPARAM>(&context));
      if (!enumerated || !context.complete) return false;
      trial->Forget(hwnd);
      const size_t limit = AutomaticWindowLimitForMonitor(key.monitor);
      if (limit && trial->VisibleWindowCount() >= limit) return false;
      WindowRecord record = incoming;
      // Predict monitor-DPI constraints, then probe again after a physical move.
      if (record.constraints.dpi != metrics.dpi) {
        record.constraints.minWidth = std::max<LONG>(1, MulDiv(record.constraints.minWidth, metrics.dpi, record.constraints.dpi));
        record.constraints.minHeight = std::max<LONG>(1, MulDiv(record.constraints.minHeight, metrics.dpi, record.constraints.dpi));
        record.constraints.dpi = metrics.dpi;
      }
      const HWND anchor = g_settings.automaticNewWindowPosition == AutomaticNewWindowPosition::AfterFocused
          ? trial->LastFocusedTiledWindow() : nullptr;
      if (anchor) trial->AdmitTiledAfter(std::move(record), anchor);
      else trial->AdmitTiled(std::move(record));
      for (HWND member : trial->TiledWindows()) {
        if (member == hwnd) continue;
        const WindowConstraints constraints = ProbeWindowConstraints(member);
        if (!constraints.valid) return false;
        trial->UpdateConstraints(member, constraints);
      }
      auto fits = [&](Workspace& candidate) {
        std::vector<RECT> rects;
        if (BuildWorkspaceLayoutPlan(candidate, metrics.workArea, metrics.gap, &rects) != LayoutPlanStatus::Success &&
            !(candidate.Layout() == TileLayout::Dwindle && AutomaticDwindleAdaptationEnabled() &&
              FitAdaptiveDwindleDividers(candidate, metrics.workArea, metrics.gap))) return false;
        if (BuildWorkspaceLayoutPlan(candidate, metrics.workArea, metrics.gap, &rects) != LayoutPlanStatus::Success) return false;
        const size_t index = candidate.TiledIndex(hwnd);
        if (index >= rects.size()) return false;
        *incomingRect = rects[index];
        return true;
      };
      if (fits(*trial)) return true;
      const Workspace preferred = *trial;
      const size_t preferredIndex = preferred.TiledIndex(hwnd);
      for (size_t offset = 1; offset < preferred.ActiveCount(); ++offset) {
        for (int direction : {-1, 1}) {
          const long long index = static_cast<long long>(preferredIndex) + direction * static_cast<long long>(offset);
          if (index < 0 || static_cast<size_t>(index) >= preferred.ActiveCount()) continue;
          Workspace alternative = preferred;
          if (alternative.MovePendingAdmissionToIndex(hwnd, static_cast<size_t>(index)) && fits(alternative)) {
            *trial = std::move(alternative);
            return true;
          }
        }
      }
      return false;
    };

    bool committed = false;
    bool mutationFailed = false;
    bool retryWhenReady = false;
    // Recheck source capacity, then try other monitors and one new desktop.
    // Never create another desktop if this attempt fails or the window cannot fit.
    const size_t candidateCount = monitors.size() + (g_settings.overflowCreateDesktop ? 1 : 0);
    for (size_t index = 0; index < candidateCount; ++index) {
      const bool createDesktop = index == monitors.size();
      const HMONITOR monitor = createDesktop ? sourceMonitor : monitors[index];
      GUID desktop = request.sourceDesktop;
      GUID createdDesktop{};
      int desktopNumber = 0;
      DesktopMonitorKey targetKey{};
      Workspace initial;
      Platform::VirtualDesktop::DesktopMetadata metadata;
      auto resolveOverflowLayout = [&](const DesktopMonitorKey& key) {
        int matchedRule = 0;
        const TileLayout ruleLayout = ResolveInitialWorkspaceLayout(key, &matchedRule, &metadata);
        if (matchedRule) {
          Workspace selected;
          selected.SetLayout(ruleLayout);
          selected.SetMasterRatio(g_settings.masterPercent / 100.0);
          return selected;
        }
        return source.EmptyWithInheritedLayoutSettings();
      };
      if (createDesktop) {
        std::vector<GUID> order;
        if (!Platform::VirtualDesktop::GetDesktopIdsInOrder(&order)) break;
        auto position = std::find_if(order.begin(), order.end(),
            [&](const GUID& id) { return IsEqualGUID(id, request.sourceDesktop); });
        if (position == order.end()) break;
        metadata.number = static_cast<int>(position - order.begin()) + 2;
        // A desktop created by this operation starts without a custom name.
        targetKey = {GUID_NULL, sourceKey.monitor};
        initial = resolveOverflowLayout(targetKey);
      } else if (!DesktopMonitorKey::FromHMonitor(desktop, monitor, &targetKey)) continue;
      Workspace candidate;
      RECT targetFrame{};
      if (!preflight(targetKey, &candidate, &targetFrame, createDesktop ? &initial : nullptr)) continue;
      auto canMoveIncoming = [&] {
        GUID liveCurrent{}, liveOwner{};
        LASTINPUTINFO liveInput{sizeof(liveInput)};
        DWORD livePid = 0;
        GetWindowThreadProcessId(hwnd, &livePid);
        return GetLastInputInfo(&liveInput) &&
            (hwnd != foreground || (GetForegroundWindow() == hwnd && liveInput.dwTime == batchInput.dwTime)) &&
            IsWindow(hwnd) && livePid == processId && g_wm.overflowWindows.count(hwnd) &&
            GetCurrentDesktopId(&liveCurrent) && IsEqualGUID(liveCurrent, request.sourceDesktop) &&
            GetWindowDesktopIdSafe(hwnd, &liveOwner) && IsEqualGUID(liveOwner, request.sourceDesktop) &&
            GetWindowPhysicalMonitor(hwnd) == sourceMonitor &&
            !IsMoveSizeGestureInProgress(hwnd) && !IsWindowCloaked(hwnd) &&
            GetPhysicalSuspensionReason(hwnd) == SuspensionReason::None;
      };
      if (!canMoveIncoming()) { retryWhenReady = true; mutationFailed = true; break; }
      if (createDesktop) {
        const bool created = Platform::VirtualDesktop::CreateDesktopAfter(
            request.sourceDesktop, &createdDesktop, &desktopNumber);
        desktop = createdDesktop;
        targetKey.desktopId = desktop;
        metadata = Platform::VirtualDesktop::ReadDesktopMetadata(desktop);
        metadata.number = desktopNumber; // Native order wins over lagging registry data.
        initial = resolveOverflowLayout(targetKey);
        const bool readyAfterCreation = canMoveIncoming();
        if (!created || !readyAfterCreation ||
            !Platform::VirtualDesktop::IsDesktopEmptyExcept(desktop) ||
            !preflight(targetKey, &candidate, &targetFrame, &initial) || !canMoveIncoming()) {
          retryWhenReady = created && (!readyAfterCreation || !canMoveIncoming());
          Platform::VirtualDesktop::RemoveEmptyOverflowDesktop(createdDesktop, request.sourceDesktop);
          mutationFailed = true;
          break;
        }
      }
      // Change monitors while the HWND is still visible. Off-desktop placement
      // is suppressed by the ordinary safety gate. Verify the app's real DPI
      // contract before committing its virtual-desktop ownership.
      if (monitor != sourceMonitor) {
        const PlacementObservation placed = PlaceWindowChecked(hwnd, incoming.canMove, targetFrame);
        incoming.constraints = ProbeWindowConstraints(hwnd);
        if (placed.result != PlacementResult::Success || GetWindowPhysicalMonitor(hwnd) != monitor ||
            !incoming.constraints.valid || !preflight(targetKey, &candidate, &targetFrame, createDesktop ? &initial : nullptr)) {
          rollbackOverflowMove({hwnd, processId, request.sourceDesktop, desktop, sourceFrame, 0, batchInput.dwTime});
          mutationFailed = true;
          break;
        }
      }
      const bool differentDesktop = !IsEqualGUID(desktop, request.sourceDesktop);
      if (differentDesktop && !Platform::VirtualDesktop::MoveWindowToDesktop(
              hwnd, processId, request.sourceDesktop, desktop)) {
        rollbackOverflowMove({hwnd, processId, request.sourceDesktop, desktop, sourceFrame, 0, batchInput.dwTime});
        mutationFailed = true;
        break;
      }
      GUID actualDesktop{};
      DWORD livePid = 0;
      if (!GetWindowDesktopIdSafe(hwnd, &actualDesktop) || !IsEqualGUID(actualDesktop, desktop) ||
          GetWindowPhysicalMonitor(hwnd) != monitor) {
        rollbackOverflowMove({hwnd, processId, request.sourceDesktop, desktop, sourceFrame, 0, batchInput.dwTime});
        mutationFailed = true;
        break;
      }
      const WindowRecord* liveRecord = nullptr;
      if (!g_workspaces.Load(sourceKey, &source) ||
          !(liveRecord = source.Find(hwnd)) || liveRecord->pid != processId ||
          liveRecord->tilingGeneration != request.tilingGeneration ||
          !g_wm.overflowWindows.count(hwnd)) {
        reconcileObservedOwner(hwnd);
        if (createDesktop) Platform::VirtualDesktop::RemoveEmptyOverflowDesktop(createdDesktop, request.sourceDesktop);
        mutationFailed = true;
        break;
      }
      // Destination membership can change during the native calls. Repeat the
      // proof after movement; a new conflict rolls back only this incoming HWND.
      incoming.constraints = ProbeWindowConstraints(hwnd);
      if ((candidate.Layout() != TileLayout::Floating && !incoming.constraints.valid) ||
          (createDesktop && !Platform::VirtualDesktop::IsDesktopEmptyExcept(desktop, hwnd)) ||
          !preflight(targetKey, &candidate, &targetFrame, createDesktop ? &initial : nullptr)) {
        rollbackOverflowMove({hwnd, processId, request.sourceDesktop, desktop, sourceFrame, 0, batchInput.dwTime});
        mutationFailed = true;
        break;
      }
      LASTINPUTINFO finalInput{sizeof(finalInput)};
      GUID finalCurrent{};
      GetWindowThreadProcessId(hwnd, &livePid);
      if (!IsWindow(hwnd) || livePid != processId ||
          !g_wm.overflowWindows.count(hwnd) || IsMoveSizeGestureInProgress(hwnd) ||
          GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
          !GetLastInputInfo(&finalInput) ||
          (hwnd == foreground && finalInput.dwTime != batchInput.dwTime) ||
          !GetCurrentDesktopId(&finalCurrent) || !IsEqualGUID(finalCurrent, request.sourceDesktop) ||
          !GetWindowDesktopIdSafe(hwnd, &actualDesktop) || !IsEqualGUID(actualDesktop, desktop) ||
          GetWindowPhysicalMonitor(hwnd) != monitor) {
        reconcileObservedOwner(hwnd);
        if (createDesktop) Platform::VirtualDesktop::RemoveEmptyOverflowDesktop(createdDesktop, request.sourceDesktop);
        mutationFailed = true;
        break;
      }
      source.Forget(hwnd);
      g_workspaces.Save(sourceKey, source);
      g_workspaces.Save(targetKey, candidate);
      if (createDesktop) {
        std::vector<HMONITOR> connected;
        EnumerateCurrentMonitors(connected);
        for (HMONITOR otherMonitor : connected) {
          DesktopMonitorKey otherKey{}, otherSourceKey{};
          Workspace existing, otherSource;
          if (otherMonitor == monitor ||
              !DesktopMonitorKey::FromHMonitor(desktop, otherMonitor, &otherKey) ||
              g_workspaces.Load(otherKey, &existing) ||
              !DesktopMonitorKey::FromHMonitor(request.sourceDesktop, otherMonitor, &otherSourceKey)) continue;
          if (!g_workspaces.Load(otherSourceKey, &otherSource)) otherSource = MakeDefaultWorkspace(otherSourceKey);
          int matchedRule = 0;
          const TileLayout ruleLayout = ResolveInitialWorkspaceLayout(otherKey, &matchedRule, &metadata);
          Workspace inherited = otherSource.EmptyWithInheritedLayoutSettings();
          if (matchedRule) {
            inherited = Workspace{};
            inherited.SetLayout(ruleLayout);
            inherited.SetMasterRatio(g_settings.masterPercent / 100.0);
          }
          g_workspaces.Save(otherKey, inherited);
        }
      }
      AddPendingWorkspaceArrange(desktop, monitor);
      if (!differentDesktop) ArrangeWorkspace(targetKey);
      if (differentDesktop && hwnd == foreground) {
        follow = {hwnd, processId, request.sourceDesktop, desktop, sourceFrame, GetTickCount64() + 2000, batchInput.dwTime};
      }
      Diagnostics::RecordEvent(L"overflow placed hwnd=%p targetDesktop=%08X monitorId=%ls", hwnd, desktop.Data1, targetKey.monitor.deviceId.c_str());
      committed = true;
      break;
    }
    if (!committed && IsWindow(hwnd) && g_wm.overflowWindows.count(hwnd) &&
        (retryWhenReady || GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None ||
         IsWindowCloaked(hwnd))) {
      g_wm.pendingWindowOverflow.push_back(std::move(request));
    } else if (!committed && !mutationFailed) {
      Diagnostics::RecordEvent(L"overflow fallback hwnd=%p reason=no viable destination", hwnd);
      Workspace fallback;
      if (g_workspaces.Load(sourceKey, &fallback)) {
        FloatingPlacementHint hint;
        hint.intent = FloatingPlacementIntent::FailureCenter;
        if (RepairFloatingGeometry(sourceKey, fallback, hwnd, hint)) g_workspaces.Save(sourceKey, fallback);
      }
    }
  }

  if (follow.hwnd) {
    GUID liveCurrent{};
    LASTINPUTINFO liveInput{sizeof(liveInput)};
    GetLastInputInfo(&liveInput);
    if (liveInput.dwTime == follow.inputTickMs && IsWindow(follow.hwnd) &&
        g_wm.overflowWindows.count(follow.hwnd) &&
        GetCurrentDesktopId(&liveCurrent) && IsEqualGUID(liveCurrent, follow.sourceDesktop)) {
      if (Platform::VirtualDesktop::SwitchToDesktop(follow.hwnd, follow.processId, follow.targetDesktop, follow.sourceDesktop, follow.inputTickMs)) {
        follow.deadlineTickMs = GetTickCount64() + 2000;
        g_wm.pendingOverflowSwitch = follow;
        g_wm.reconciledDesktopState = ReconciledDesktopState::TransitionPending;
        waitForSwitch = true;
        ScheduleLifecycleReconcile(nullptr);
      } else {
        GetLastInputInfo(&liveInput);
        if (liveInput.dwTime == follow.inputTickMs && GetCurrentDesktopId(&liveCurrent) &&
            IsEqualGUID(liveCurrent, follow.sourceDesktop)) rollbackOverflowMove(follow);
      }
    }
  }
  ULONGLONG nextDue = waitForSwitch ? GetTickCount64() + 50 : 0;
  if (nextDue) {
    const ULONGLONG tick = GetTickCount64();
    g_wm.overflowTimer = SetTimer(nullptr, 0, nextDue > tick ? static_cast<UINT>(nextDue - tick) : 1, nullptr);
    if (!g_wm.overflowTimer) {
      g_wm.pendingOverflowSwitch = {};
      Wh_Log(L"Overflow switch timer unavailable; retaining observed window ownership");
    }
  }
}

// After a lifecycle burst settles, reconcile tracked-window physical and desktop
// state, discover eligible windows only on the current desktop, then arrange each
// changed or pending workspace once and refresh the tray.
static void ReconcileDeferredLifecycle() {
  ++Diagnostics::g_runtime.counters.lifecycleReconciles;
  if (g_wm.lifecycleTimer) {
    KillTimer(nullptr, g_wm.lifecycleTimer);
    g_wm.lifecycleTimer = 0;
  }

  std::vector<HWND> dirtyWindows;
  dirtyWindows.swap(g_wm.lifecycleDirtyWindows);
  Diagnostics::RecordLifecycleBurst(
      dirtyWindows.size(), g_wm.pendingDesktopArranges.size());

  // Resolve desktop identity before mutating model state. If Explorer is still
  // coming up (or restarting), preserve the dirty set and let the single runtime
  // maintenance owner recover COM before this exact work is retried.
  GUID currentDesktop{};
  if (!InitializeVirtualDesktopAPI() || !GetCurrentDesktopId(&currentDesktop)) {
    for (HWND hwnd : dirtyWindows) {
      if (hwnd && std::find(g_wm.lifecycleDirtyWindows.begin(),
                            g_wm.lifecycleDirtyWindows.end(), hwnd) ==
                      g_wm.lifecycleDirtyWindows.end()) {
        g_wm.lifecycleDirtyWindows.push_back(hwnd);
      }
    }
    g_wm.lifecycleRetryAfterPlatformRecovery = true;
    RuntimeLifecycle::RequestMaintenance(false);
    Wh_Log(L"Lifecycle reconcile deferred: current virtual desktop unavailable");
    return;
  }

  Diagnostics::g_runtime.counters.lifecycleDirtyWindowsProcessed += dirtyWindows.size();

  // Native notifications are primary, but a settled cloak/uncloak burst can
  // independently detect a missed subscription/callback. Treat an observed GUID
  // change as a real desktop transition: repair global desktop ownership and
  // reinforce all known/current monitors on the incoming desktop.
  const bool fallbackDesktopChange =
      g_wm.reconciledDesktopState != ReconciledDesktopState::Unknown &&
      !IsEqualGUID(g_wm.reconciledDesktop, currentDesktop);
  if (fallbackDesktopChange) {
    // The native callback was missed, so this is the first point where the WM can
    // prove that the active desktop changed. Discard any lease created on the old
    // desktop before doing ownership repair or incoming-workspace reinforcement.
    ClearAllConformanceLeases();
    ScheduleNextConformanceTimer();
    ++Diagnostics::g_runtime.counters.virtualDesktopFallbackChanges;
    Diagnostics::RecordEvent(
        L"virtual desktop change recovered from settled WinEvents currentDesktop=%08X",
        currentDesktop.Data1);
    // A missed native callback is also a high-confidence signal that an exhausted
    // subscription retry burst deserves one fresh attempt sequence.
    if (g_vd.abi.notification.methodCount > 0 &&
        !g_vd.notificationsRegistered) {
      RuntimeLifecycle::RequestMaintenance(true);
    }
    g_wm.pendingDesktopSwitchFlyouts = true;
  }
  g_wm.reconciledDesktop = currentDesktop;
  g_wm.reconciledDesktopState = ReconciledDesktopState::Settled;

  // Repair liveness/exclusions and settled suspended participation globally
  // before doing current-desktop discovery.
  std::vector<DesktopMonitorKey> globallyChanged;
  ReconcileKnownWindows(ReconcileScope::Participation, &globallyChanged);
  if (fallbackDesktopChange) {
    ReconcileKnownWindows(ReconcileScope::Desktop, &globallyChanged);
  }

  // Dirty tracked HWNDs also get a desktop ownership check; per-window moves do
  // not emit CurrentVirtualDesktopChanged.
  for (HWND hwnd : dirtyWindows) {
    if (!hwnd || !IsWindowTrackedInAnyState(hwnd)) continue;
    ReconcileKnownWindows(ReconcileScope::Desktop, &globallyChanged, hwnd);
  }

  std::vector<HMONITOR> monitors;
  if (fallbackDesktopChange || g_wm.forceMonitorReconcile) {
    // Authoritative desktop/topology passes derive their boundary set from the OS,
    // not from whatever workspace keys happened to exist before the transition.
    EnumerateCurrentMonitors(monitors);
    for (HMONITOR monitor : monitors) {
      DesktopMonitorKey key{};
      if (!DesktopMonitorKey::FromHMonitor(currentDesktop, monitor, &key)) continue;
      Workspace existing;
      if (g_workspaces.Load(key, &existing)) {
        AddPendingWorkspaceArrange(currentDesktop, monitor);
      }
    }
  } else {
    // Incremental lifecycle work stays cheap: start from known live workspace
    // boundaries and add only monitors implicated by this burst.
    monitors = g_workspaces.MonitorsOnDesktop(currentDesktop);
  }

  if (g_wm.forceMonitorReconcile) {
    ReconcileKnownWindows(ReconcileScope::Monitor, &globallyChanged);
    g_wm.forceMonitorReconcile = false;
  }

  // Also include monitors implicated by the shell burst so automatic discovery
  // can seed a new desktop+monitor workspace.
  for (HWND hwnd : dirtyWindows) {
    if (!hwnd || !IsWindow(hwnd)) continue;
    BOOL onCurrent = FALSE;
    if (!IsWindowOnCurrentDesktopSafe(hwnd, &onCurrent) || !onCurrent) continue;
    AddUniqueMonitor(monitors, GetWindowPhysicalMonitor(hwnd));
  }

  RouteNewWindows(monitors);

  // One top-level enumeration is enough for every monitor involved in this burst.
  std::vector<std::vector<HWND>> snapshots = CollectTileWindowsForMonitors(monitors);
  std::vector<std::vector<HWND>> suspendedSnapshots =
      CollectDirtySuspendedWindowsForMonitors(dirtyWindows, monitors, currentDesktop);

  std::vector<DesktopMonitorKey> arrangedKeys;
  for (size_t monitorIndex = 0; monitorIndex < monitors.size(); ++monitorIndex) {
    HMONITOR monitor = monitors[monitorIndex];
    DesktopMonitorKey key{};
    const std::vector<HWND>& snapshot = snapshots[monitorIndex];
    const std::vector<HWND>& suspendedSnapshot = suspendedSnapshots[monitorIndex];
    bool changed = DiscoverWorkspaceWindows(
        monitor, currentDesktop, snapshot, &key, &globallyChanged,
        &suspendedSnapshot);
    bool pendingArrange = std::any_of(
        g_wm.pendingDesktopArranges.begin(), g_wm.pendingDesktopArranges.end(),
        [&](const PendingWorkspaceArrange& pending) {
          return pending.monitor == monitor &&
                 IsEqualGUID(pending.desktopId, currentDesktop);
        });
    if (changed || pendingArrange) {
      ArrangeWorkspace(key);
      AddUniqueWorkspaceKey(arrangedKeys, key);
    }
  }

  // Admission is intentionally decided only after the settled snapshot. If a
  // plausible Automatic-mode candidate is still unknown, its first observation
  // may have raced application initialization or shell desktop registration.
  // Give it one independent re-observation without turning discovery into polling.
  if ((IsAutomaticMode() || g_wm.windowRoutingReady) &&
      g_settings.slowApplicationDelayMs) {
    bool addedRetry = false;
    const ULONGLONG dueTickMs =
        GetTickCount64() + g_settings.slowApplicationDelayMs;
    for (HWND hwnd : dirtyWindows) {
      if (hwnd && IsWindow(hwnd) && !IsWindowTrackedInAnyState(hwnd) &&
          (IsAutomaticMode() || !g_wm.windowRoutingEvaluated.count(hwnd)) &&
          GetPhysicalSuspensionReason(hwnd) == SuspensionReason::None &&
          GetAncestor(hwnd, GA_ROOT) == hwnd) {
        DWORD processId = 0;
        GetWindowThreadProcessId(hwnd, &processId);
        if (!processId) continue;

        auto existing = std::find_if(
            g_wm.pendingDiscoveryRetries.begin(),
            g_wm.pendingDiscoveryRetries.end(),
            [&](const PendingDiscoveryRetry& pending) {
              return pending.hwnd == hwnd;
            });
        if (existing != g_wm.pendingDiscoveryRetries.end()) {
          if (existing->processId == processId) continue;
          *existing = {hwnd, processId, dueTickMs};
        } else {
          g_wm.pendingDiscoveryRetries.push_back(
              {hwnd, processId, dueTickMs});
        }
        addedRetry = true;
      }
    }

    if (addedRetry) {
      ScheduleNextDiscoveryRetryTimer();
    }
  }

  for (const auto& key : globallyChanged) {
    if (std::none_of(arrangedKeys.begin(), arrangedKeys.end(),
                     [&](const DesktopMonitorKey& arranged) {
                       return SameWorkspaceKey(arranged, key);
                     })) {
      ArrangeWorkspace(key);
    }
  }

  g_wm.pendingDesktopArranges.erase(
      std::remove_if(
          g_wm.pendingDesktopArranges.begin(), g_wm.pendingDesktopArranges.end(),
          [&](const PendingWorkspaceArrange& pending) {
            return IsEqualGUID(pending.desktopId, currentDesktop);
          }),
      g_wm.pendingDesktopArranges.end());

  g_wm.lifecycleRetryAfterPlatformRecovery = false;
  if (g_wm.pendingDesktopSwitchFlyouts) {
    g_wm.pendingDesktopSwitchFlyouts = false;
    TrayUi::ShowDesktopSwitchFlyouts(currentDesktop);
  }
  ProcessWindowOverflow();
  FollowInitiallyPlacedWindow();
  // Ordinary lifecycle work updates the icon/tooltip for the monitor the tray is
  // already representing. Only a foreground event or explicit workspace command
  // retargets that single shell icon to another monitor.
  TrayUi::RefreshDisplayedMonitor();
}

static void ReconcileManagedWindowStateNow(
    HWND hwnd, ReconcileScope scope = ReconcileScope::Participation) {
  std::vector<DesktopMonitorKey> changedKeys;
  ReconcileKnownWindows(scope, &changedKeys, hwnd);
  for (const auto& key : changedKeys) ArrangeWorkspace(key);
}

// Converts queued lifecycle events into model actions: destruction is authoritative,
// state changes get immediate and settled reconciliation, while visibility and
// cloak events remain dirty signals until shell state stabilizes.
static void ProcessWindowLifecycleEvent(DWORD event, HWND hwnd) {
  ++Diagnostics::g_runtime.counters.lifecycleEventsProcessed;
  switch (event) {
    case EVENT_OBJECT_DESTROY:
      ++Diagnostics::g_runtime.counters.lifecycleDestroy;
      break;
    case EVENT_SYSTEM_MINIMIZESTART:
      ++Diagnostics::g_runtime.counters.lifecycleMinimizeStart;
      break;
    case EVENT_SYSTEM_MINIMIZEEND:
      ++Diagnostics::g_runtime.counters.lifecycleMinimizeEnd;
      break;
    case EVENT_OBJECT_STATECHANGE:
      ++Diagnostics::g_runtime.counters.lifecycleStateChange;
      break;
    case EVENT_OBJECT_LOCATIONCHANGE:
      ++Diagnostics::g_runtime.counters.lifecycleLocationChange;
      break;
    case EVENT_OBJECT_HIDE:
      ++Diagnostics::g_runtime.counters.lifecycleHide;
      break;
    case EVENT_OBJECT_SHOW:
      ++Diagnostics::g_runtime.counters.lifecycleShow;
      break;
    case EVENT_OBJECT_CLOAKED:
      ++Diagnostics::g_runtime.counters.lifecycleCloaked;
      break;
    case EVENT_OBJECT_UNCLOAKED:
      ++Diagnostics::g_runtime.counters.lifecycleUncloaked;
      break;
  }

  switch (event) {
    case EVENT_OBJECT_DESTROY:
      ForgetManagedWindow(hwnd);
      break;

    case EVENT_SYSTEM_MINIMIZESTART:
      if (IsManualMode()) {
        // Minimize is an explicit exit from an inert Manual group. Act on
        // the event itself so a very fast minimize/restore cannot outrun the
        // settled IsIconic() check and accidentally retain membership.
        ForgetManagedWindow(hwnd);
      } else {
        ReconcileManagedWindowStateNow(hwnd);
        ScheduleLifecycleReconcile(hwnd);
      }
      break;

    case EVENT_OBJECT_LOCATIONCHANGE:
      // Cross-monitor movement remains ownership migration, not conformance drift.
      // Otherwise a tiled HWND that leaves its authoritative rectangle opens or
      // reuses one bounded lease; expected SetWindowPos echoes are consumed there.
      // A native drag remains authoritative until MOVESIZEEND reconciles monitor
      // ownership for this HWND. Never probe or migrate underneath the user.
      if (IsMoveSizeGestureInProgress(hwnd)) break;
      if (!Platform::WindowEvents::HasTrackedMonitorOwnershipMismatch(hwnd) &&
          HandleTiledWindowLocationChange(hwnd)) {
        break;
      }

      // Non-tiled/state transitions and monitor migration retain ordinary
      // participation/ownership reconciliation.
      ReconcileManagedWindowStateNow(
          hwnd, ReconcileScope::Participation | ReconcileScope::Monitor);
      ScheduleLifecycleReconcile(hwnd);
      break;

    case EVENT_SYSTEM_MINIMIZEEND:
    case EVENT_OBJECT_STATECHANGE:
      // These are prompts to query actual physical state. In Automatic mode,
      // minimize/maximize/fullscreen transitions preserve membership through a
      // saved slot. Run immediately and after settle.
      ReconcileManagedWindowStateNow(hwnd);
      ScheduleLifecycleReconcile(hwnd);
      break;

    case EVENT_OBJECT_HIDE:
    case EVENT_OBJECT_SHOW:
    case EVENT_OBJECT_CLOAKED:
    case EVENT_OBJECT_UNCLOAKED:
      // These are dirty signals, not ownership decisions. The settled lifecycle
      // pass re-queries physical state and may repair this tracked HWND's VD.
      ScheduleLifecycleReconcile(hwnd);
      break;
  }
}


}  // namespace Reconcile

// Consumes and clears the WinEvent samples for one completed gesture.
// The WM core never reads these caches piecemeal after this boundary.
static MoveSizeGesture ConsumeMoveSizeGesture(HWND hwnd) {
  AssertWmThread(L"ConsumeMoveSizeGesture");
  MoveSizeGesture gesture;

  auto startIt = g_moveSize.startRects.find(hwnd);
  auto endIt = g_moveSize.endRects.find(hwnd);
  auto pointIt = g_moveSize.endPoints.find(hwnd);

  if (startIt != g_moveSize.startRects.end() && endIt != g_moveSize.endRects.end()) {
    gesture.start = startIt->second;
    gesture.end = endIt->second;
    gesture.hasRects = true;
  }
  if (pointIt != g_moveSize.endPoints.end()) {
    gesture.dropPoint = pointIt->second;
    gesture.hasDropPoint = true;
  }

  g_moveSize.startRects.erase(hwnd);
  g_moveSize.endRects.erase(hwnd);
  g_moveSize.endPoints.erase(hwnd);
  if (gesture.hasRects) {
    gesture.intent = ClassifyMoveSizeIntent(gesture.start, gesture.end, 1);
  }
  return gesture;
}

//=============================================================================
// User commands
//=============================================================================
namespace Commands {

// User-authored geometry changes are rejected before commit when cached native
// constraints already make them impossible. A short-lived transaction handles a
// minimum that an application changes only during the resulting placement.
static bool ValidateUserLayoutMutation(
    const DesktopMonitorKey& key, HMONITOR monitor,
    Workspace& candidate, HWND subject, UserLayoutMutationKind kind,
    const wchar_t* action) {
  // A same-DPI native minimum can change while no placement is occurring.
  // Refresh the committed workspace first, persist that application-owned state,
  // then mirror it into the geometry-mutated candidate before deciding whether
  // the user's operation is feasible.
  Workspace current;
  bool committedConstraintsChanged = false;
  if (g_workspaces.Load(key, &current)) {
    committedConstraintsChanged =
        RefreshWorkspaceNativeConstraints(current, true, L"user mutation");
    if (committedConstraintsChanged) {
      g_workspaces.Save(key, current);
    }
    for (HWND hwnd : candidate.TiledWindows()) {
      const WindowRecord* currentRecord = current.Find(hwnd);
      if (currentRecord) {
        candidate.UpdateConstraints(hwnd, currentRecord->constraints);
      }
    }
  }

  WorkspaceMetrics metrics;
  std::vector<RECT> rects;
  LayoutPlanStatus status = LayoutPlanStatus::Unsatisfiable;
  if (GetWorkspaceMetrics(monitor, &metrics)) {
    ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
    status = BuildWorkspaceLayoutPlan(
        candidate, metrics.workArea, metrics.gap, &rects);
    if (status == LayoutPlanStatus::Unsatisfiable &&
        candidate.Layout() == TileLayout::Dwindle) {
      if ((kind == UserLayoutMutationKind::Order ||
           kind == UserLayoutMutationKind::LayoutChange) &&
          FitAdaptiveDwindleDividers(
              candidate, metrics.workArea, metrics.gap)) {
        // Preserve the requested/current order whenever adaptive geometry alone
        // can make it feasible. Explicit order mutations stop here: they are
        // never allowed to reshuffle unrelated windows.
        Diagnostics::RecordEvent(
            kind == UserLayoutMutationKind::Order
                ? L"adaptive dwindle order preflight accommodated action=%ls"
                : L"adaptive dwindle layout conversion preserved order action=%ls",
            action ? action : L"Layout mutation");
        return true;
      }

      if (kind == UserLayoutMutationKind::LayoutChange) {
        Workspace reassigned = candidate;
        size_t movedWindowCount = 0;
        if (ReassignDwindleLayoutConversionForConstraints(
                reassigned, metrics.workArea, metrics.gap,
                &movedWindowCount)) {
          candidate = std::move(reassigned);
          Diagnostics::RecordEvent(
              L"dwindle layout conversion reassigned windows action=%ls moved=%zu",
              action ? action : L"Change layout", movedWindowCount);
          return true;
        }
      }
    }
  }
  if (status == LayoutPlanStatus::Success ||
      status == LayoutPlanStatus::NotApplicable) {
    return true;
  }

  ReportRejectedLayoutMutation(
      key, monitor, kind, action ? action : L"Layout mutation", subject,
      candidate, rects);
  // If the rejection exposed a changed native contract, reconcile the committed
  // layout only when that existing layout is itself no longer feasible. Avoid an
  // unnecessary reflow merely because the rejected candidate needed fresh data.
  if (committedConstraintsChanged) {
    WorkspaceMetrics committedMetrics;
    std::vector<RECT> committedRects;
    if (GetWorkspaceMetrics(monitor, &committedMetrics)) {
      ++Diagnostics::g_runtime.counters.layoutPlansBuilt;
      if (BuildWorkspaceLayoutPlan(
              current, committedMetrics.workArea, committedMetrics.gap,
              &committedRects) == LayoutPlanStatus::Unsatisfiable) {
        ArrangeWorkspace(key);
      }
    }
  }
  return false;
}

// Promotes the foreground window, or its tiled owner, to the master slot, then
// clears stale gesture samples for both windows and reflows the workspace.
void SwapMaster() {
  ++Diagnostics::g_runtime.counters.swapMasterCommands;
  HWND fg = GetForegroundWindow();
  if (!fg) return;

  HMONITOR monitor = GetWindowPhysicalMonitor(fg);
  if (!monitor) return;

  GUID desktopId{};
  if (!GetWindowDesktopIdSafe(fg, &desktopId)) return;

  DesktopMonitorKey key{};
  if (!DesktopMonitorKey::FromHMonitor(desktopId, monitor, &key)) return;
  Workspace workspace;
  if (!g_workspaces.Load(key, &workspace) || workspace.ActiveCount() < 2 ||
      workspace.Layout() == TileLayout::Floating) {
    return;
  }

  HWND resolved = ResolveToTiledWindow(fg, workspace.TiledWindows());
  Workspace candidate = workspace;
  HWND oldMaster = nullptr;
  if (!resolved || !candidate.MakeMaster(resolved, &oldMaster)) return;
  if (!ValidateUserLayoutMutation(
          key, monitor, candidate, resolved, UserLayoutMutationKind::Order,
          L"Set master")) {
    return;
  }

  RememberPendingLayoutMutation(
      key, workspace, candidate, resolved, UserLayoutMutationKind::Order,
      L"Set master");
  g_workspaces.Save(key, candidate);
  ClearMoveSizeSamples(oldMaster);
  ClearMoveSizeSamples(resolved);
  ArrangeWorkspace(key, resolved);
  Diagnostics::RecordEvent(
      L"Swap Master old=%p new=%p", oldMaster, resolved);
}

// Moves the focused tiled window one slot toward or away from the front of the
// workspace's logical order. The operation is deliberately non-wrapping so the
// first and last slots remain stable boundaries.
static void MoveFocusedWindowOneSlot(bool promote) {
  if (promote) {
    ++Diagnostics::g_runtime.counters.promoteWindowCommands;
  } else {
    ++Diagnostics::g_runtime.counters.demoteWindowCommands;
  }

  HWND fg = GetForegroundWindow();
  if (!fg) return;

  HMONITOR monitor = GetWindowPhysicalMonitor(fg);
  if (!monitor) return;

  GUID desktopId{};
  if (!GetWindowDesktopIdSafe(fg, &desktopId)) return;

  DesktopMonitorKey key{};
  if (!DesktopMonitorKey::FromHMonitor(desktopId, monitor, &key)) return;

  Workspace workspace;
  if (!g_workspaces.Load(key, &workspace) || workspace.ActiveCount() < 2 ||
      workspace.Layout() == TileLayout::Floating) {
    return;
  }

  HWND resolved = ResolveToTiledWindow(fg, workspace.TiledWindows());
  if (!resolved) return;

  const size_t index = workspace.TiledIndex(resolved);
  const size_t count = workspace.ActiveCount();
  if (index >= count) return;

  if ((promote && index == 0) || (!promote && index + 1 >= count)) {
    return;
  }

  const size_t otherIndex = promote ? index - 1 : index + 1;
  HWND other = workspace.TiledWindows()[otherIndex];
  Workspace candidate = workspace;
  if (!candidate.SwapTiled(resolved, other)) return;
  if (!ValidateUserLayoutMutation(
          key, monitor, candidate, resolved, UserLayoutMutationKind::Order,
          promote ? L"Promote" : L"Demote")) {
    return;
  }

  RememberPendingLayoutMutation(
      key, workspace, candidate, resolved, UserLayoutMutationKind::Order,
      promote ? L"Promote" : L"Demote");
  g_workspaces.Save(key, candidate);
  ClearMoveSizeSamples(resolved);
  ClearMoveSizeSamples(other);
  ArrangeWorkspace(key, resolved);

  Diagnostics::RecordEvent(
      promote ? L"Promote Window hwnd=%p from=%zu to=%zu"
              : L"Demote Window hwnd=%p from=%zu to=%zu",
      resolved, index, otherIndex);
}

void PromoteFocusedWindow() {
  MoveFocusedWindowOneSlot(true);
}

void DemoteFocusedWindow() {
  MoveFocusedWindowOneSlot(false);
}

void FloatFocusedWindow() {
  ++Diagnostics::g_runtime.counters.floatFocusedCommands;
  HWND hwnd = GetForegroundWindow();
  if (!hwnd) return;

  HMONITOR monitor = GetWindowPhysicalMonitor(hwnd);
  GUID desktopId{};
  if (!monitor || !GetWindowDesktopIdSafe(hwnd, &desktopId)) return;

  DesktopMonitorKey key{};
  if (!DesktopMonitorKey::FromHMonitor(desktopId, monitor, &key)) return;
  Workspace workspace;
  if (!g_workspaces.Load(key, &workspace)) return;

  // Explicit floating also cancels a queued overflow of this focused HWND.
  g_wm.overflowWindows.erase(hwnd);
  g_wm.pendingWindowOverflow.erase(
      std::remove_if(g_wm.pendingWindowOverflow.begin(), g_wm.pendingWindowOverflow.end(),
                     [hwnd](const PendingWindowOverflow& request) { return request.hwnd == hwnd; }),
      g_wm.pendingWindowOverflow.end());
  HWND tiled = ResolveToTiledWindow(hwnd, workspace.TiledWindows());
  if (!tiled) return;

  FloatingPlacementHint hint;
  hint.intent = FloatingPlacementIntent::CenterWithFloatingSize;

  CancelConformanceLease(tiled);
  DiscardPendingAdmissionRecovery(tiled);
  if (!workspace.Float(tiled)) return;
  RepairFloatingGeometry(key, workspace, tiled, hint);
  g_workspaces.Save(key, workspace);
  ArrangeWorkspace(key);
  Wh_Log(L"Focused window floated %p", tiled);
  Diagnostics::RecordEvent(L"Float Focused hwnd=%p", tiled);
}

// Manual tile: reconcile membership and explicitly adopt the user's current
// geometry. Automatic events never call this with adoptCurrentGeometry=true.
void TileWindows(HMONITOR monitor = nullptr) {
  ++Diagnostics::g_runtime.counters.tileCommands;
  if (!monitor) monitor = GetWorkspaceCommandMonitor();
  if (!monitor) return;

  std::vector<DesktopMonitorKey> changedKeys;
  Reconcile::ReconcileKnownWindows(Reconcile::ReconcileScope::All, &changedKeys);

  DesktopMonitorKey targetKey{};
  if (!GetCurrentWorkspaceKey(monitor, &targetKey)) return;
  if (!Reconcile::EnsureWorkspaceFromSnapshot(monitor, true, &changedKeys)) return;

  // Ensure other affected current workspaces also lose stale slots. Keep the
  // target workspace until after geometry adoption so manual tiling does not
  // overwrite the user's geometry before capturing it.
  for (const auto& key : changedKeys) {
    if (!Reconcile::SameWorkspaceKey(key, targetKey)) ArrangeWorkspace(key);
  }

  TrayUi::RefreshForMonitor(monitor);
  Wh_Log(L"Manual tile/global reconcile completed");
  Diagnostics::RecordEvent(L"Tile Workspace command completed monitor=%p", monitor);
}

// Advances the current workspace layout and invalidates layout-specific hints in
// suspended slots before persisting, arranging, and updating UI feedback.
void CycleCurrentWorkspaceLayout(HMONITOR monitor = nullptr) {
  ++Diagnostics::g_runtime.counters.layoutCycleCommands;
  if (!monitor) monitor = GetWorkspaceCommandMonitor();
  if (!monitor) return;
  DesktopMonitorKey key{};
  if (!GetCurrentWorkspaceKey(monitor, &key)) return;

  Workspace state;
  if (!g_workspaces.Load(key, &state)) {
    if (IsAutomaticMode()) {
      if (!Reconcile::EnsureWorkspaceFromSnapshot(monitor, false) ||
          !g_workspaces.Load(key, &state)) {
        return;
      }
    } else {
      state = Reconcile::MakeDefaultWorkspace(key);
    }
  }

  const TileLayout previousLayout = state.Layout();
  Workspace candidate = state;
  if (!candidate.CycleLayout(g_settings.layoutCycle)) return;
  if (!ValidateUserLayoutMutation(
          key, monitor, candidate, nullptr,
          UserLayoutMutationKind::LayoutChange, L"Change layout")) {
    return;
  }
  RememberPendingLayoutMutation(
      key, state, candidate, nullptr, UserLayoutMutationKind::LayoutChange,
      L"Change layout");
  state = std::move(candidate);
  if (previousLayout != TileLayout::Floating &&
      state.Layout() == TileLayout::Floating) {
    RestoreWorkspaceFloatingGeometry(key, state);
  }
  g_workspaces.Save(key, state);
  ArrangeWorkspace(key);
  TrayUi::UpdateIcon(state.Layout(), monitor);
  TrayUi::ShowLayoutFlyout(state.Layout(), monitor);
  Wh_Log(L"Workspace layout changed to %d", static_cast<int>(state.Layout()));
  Diagnostics::RecordEvent(
      L"layout cycle desktop=%08X monitorId=%ls from=%d to=%d", key.desktopId.Data1,
      key.monitor.deviceId.c_str(), static_cast<int>(previousLayout),
      static_cast<int>(state.Layout()));
}

// Direct layout selection used by the tray context menu. This is intentionally
// separate from CycleCurrentWorkspaceLayout: selecting a named layout should not
// depend on its current position in the configured cycle.
void SetCurrentWorkspaceLayout(TileLayout layout, HMONITOR monitor = nullptr) {
  if (layout == TileLayout::COUNT) return;
  if (!monitor) monitor = GetWorkspaceCommandMonitor();
  if (!monitor) return;

  DesktopMonitorKey key{};
  if (!GetCurrentWorkspaceKey(monitor, &key)) return;

  Workspace state;
  bool workspaceCreated = false;
  if (!g_workspaces.Load(key, &state)) {
    if (IsAutomaticMode()) {
      if (!Reconcile::EnsureWorkspaceFromSnapshot(monitor, false) ||
          !g_workspaces.Load(key, &state)) {
        return;
      }
    } else {
      state = Reconcile::MakeDefaultWorkspace(key);
      workspaceCreated = true;
    }
  }

  const TileLayout previousLayout = state.Layout();
  if (previousLayout == layout) {
    if (workspaceCreated) g_workspaces.Save(key, state);
    // Keep the tray view coherent even if the selected item was already active.
    TrayUi::UpdateIcon(layout, monitor);
    return;
  }

  Workspace candidate = state;
  candidate.SetLayout(layout);
  if (!ValidateUserLayoutMutation(
          key, monitor, candidate, nullptr,
          UserLayoutMutationKind::LayoutChange, L"Change layout")) {
    return;
  }
  RememberPendingLayoutMutation(
      key, state, candidate, nullptr, UserLayoutMutationKind::LayoutChange,
      L"Change layout");
  state = std::move(candidate);
  if (previousLayout != TileLayout::Floating && layout == TileLayout::Floating) {
    RestoreWorkspaceFloatingGeometry(key, state);
  }
  g_workspaces.Save(key, state);
  ArrangeWorkspace(key);
  TrayUi::UpdateIcon(layout, monitor);
  TrayUi::ShowLayoutFlyout(layout, monitor);
  Wh_Log(L"Workspace layout set to %d", static_cast<int>(layout));
  Diagnostics::RecordEvent(
      L"layout set desktop=%08X monitorId=%ls from=%d to=%d", key.desktopId.Data1,
      key.monitor.deviceId.c_str(), static_cast<int>(previousLayout),
      static_cast<int>(layout));
}

static void HandleUserMove(
    const DesktopMonitorKey& key, Workspace& workspace, HWND hwnd,
    const MoveSizeGesture& gesture) {
  if (g_settings.mouseMoveBehavior == MouseMoveBehavior::Float) {
    CancelConformanceLease(hwnd);
    DiscardPendingAdmissionRecovery(hwnd);
    if (workspace.Float(hwnd)) {
      RepairFloatingGeometry(
          key, workspace, hwnd, MakeFloatingPlacementHintFromGesture(gesture));
      g_workspaces.Save(key, workspace);
      ++Diagnostics::g_runtime.counters.tiledFloatActions;
      Wh_Log(L"User move floated managed window %p", hwnd);
      Diagnostics::RecordEvent(L"user drag floated hwnd=%p", hwnd);
    }
    ArrangeWorkspace(key);
    return;
  }

  HWND target = nullptr;
  HWND preferredTiledWindow = nullptr;
  if (workspace.IsTiled(hwnd) && gesture.hasDropPoint) {
    for (HWND candidate : workspace.TiledWindows()) {
      if (candidate == hwnd) continue;
      RECT rect{};
      if (GetWindowFrameRect(candidate, &rect) && PtInRect(&rect, gesture.dropPoint)) {
        target = candidate;
        break;
      }
    }
  }

  if (target) {
    Workspace candidate = workspace;
    if (candidate.SwapTiled(hwnd, target) &&
        ValidateUserLayoutMutation(
            key, key.ResolveMonitor(), candidate, hwnd,
            UserLayoutMutationKind::Order, L"Swap")) {
      RememberPendingLayoutMutation(
          key, workspace, candidate, hwnd, UserLayoutMutationKind::Order,
          L"Swap");
      workspace = std::move(candidate);
      preferredTiledWindow = hwnd;
      g_workspaces.Save(key, workspace);
      ++Diagnostics::g_runtime.counters.tiledSwapActions;
      Wh_Log(L"User move swapped managed windows %p and %p", hwnd, target);
      Diagnostics::RecordEvent(
          L"user drag swapped hwnd=%p target=%p", hwnd, target);
    }
  }

  // No target means the source remains tiled and simply snaps back to its slot.
  ArrangeWorkspace(key, preferredTiledWindow);
}

static void HandleUserResize(
    const DesktopMonitorKey& key, Workspace& workspace, HWND hwnd,
    const RECT& workArea, LONG gap, const MoveSizeGesture& gesture) {
  if (!gesture.hasRects) {
    ArrangeWorkspace(key);
    return;
  }

  const size_t resizedIndex = workspace.TiledIndex(hwnd);
  if (resizedIndex == static_cast<size_t>(-1)) {
    ArrangeWorkspace(key);
    return;
  }

  Workspace candidate = workspace;
  bool changed = false;
  switch (candidate.Layout()) {
    case TileLayout::MasterStack:
    case TileLayout::MasterStackH:
      changed = candidate.LearnMasterStackResize(
          workArea, gap, resizedIndex, gesture);
      break;

    case TileLayout::Columns:
    case TileLayout::Rows:
      changed = candidate.LearnGridResize(
          workArea, gap, resizedIndex, gesture);
      break;

    case TileLayout::COUNT:
      return;

    case TileLayout::Dwindle:
      changed = candidate.LearnDwindleResize(
          workArea, gap, resizedIndex, gesture);
      break;

    case TileLayout::Monocle:
    case TileLayout::Floating:
      break;
  }

  if (changed) {
    if (!ValidateUserLayoutMutation(
            key, key.ResolveMonitor(), candidate, hwnd,
            UserLayoutMutationKind::Resize, L"Resize")) {
      ArrangeWorkspace(key, hwnd);
      return;
    }
    RememberPendingLayoutMutation(
        key, workspace, candidate, hwnd, UserLayoutMutationKind::Resize,
        L"Resize");
    workspace = std::move(candidate);
    g_workspaces.Save(key, workspace);
    ++Diagnostics::g_runtime.counters.dividerUpdates;
    Wh_Log(L"User resize updated local divider state for %p", hwnd);
    Diagnostics::RecordEvent(L"user resize updated divider hwnd=%p", hwnd);
  }
  ArrangeWorkspace(key, changed ? hwnd : nullptr);
}

// Consumes one completed user gesture and routes it to the move or resize policy.
// The top-level handler deliberately contains no layout-specific math.
void ApplyUserMoveSize(HWND hwnd) {
  if (!IsWindow(hwnd)) return;

  if (GetPhysicalSuspensionReason(hwnd) != SuspensionReason::None) {
    ClearMoveSizeSamples(hwnd);
    return;
  }

  HMONITOR monitor = GetWindowPhysicalMonitor(hwnd);
  if (!monitor) return;

  GUID desktopId{};
  if (!GetWindowDesktopIdSafe(hwnd, &desktopId)) return;

  DesktopMonitorKey key{};
  if (!DesktopMonitorKey::FromHMonitor(desktopId, monitor, &key)) return;
  Workspace state;
  if (!g_workspaces.Load(key, &state)) return;

  // Classify every completed move/size gesture at the common boundary before
  // routing it to tiled or floating policy. This makes the counters describe
  // actual user gestures instead of only gestures that reached tiled handlers.
  MoveSizeGesture gesture = ConsumeMoveSizeGesture(hwnd);
  switch (gesture.intent) {
    case MoveSizeIntent::Move:
      ++Diagnostics::g_runtime.counters.userMoveGestures;
      break;
    case MoveSizeIntent::Resize:
      ++Diagnostics::g_runtime.counters.userResizeGestures;
      break;
    case MoveSizeIntent::None:
      ++Diagnostics::g_runtime.counters.userNoopGestures;
      break;
  }

  const WindowRecord* directRecord = state.Find(hwnd);
  if (directRecord &&
      (state.Layout() == TileLayout::Floating ||
       directRecord->state == ManageState::Floating)) {
    if (RememberCurrentFloatingGeometry(key, state, hwnd)) {
      g_workspaces.Save(key, state);
      if (gesture.intent != MoveSizeIntent::None) {
        ++Diagnostics::g_runtime.counters.floatingGeometryUserUpdates;
      }
    }
    return;
  }

  if (state.Layout() == TileLayout::Floating) {
    return;
  }

  WorkspaceMetrics metrics;
  if (!GetWorkspaceMetrics(monitor, &metrics)) return;

  HWND tiledHwnd = state.IsTiled(hwnd)
      ? hwnd
      : ResolveToTiledWindow(hwnd, state.TiledWindows());
  if (!tiledHwnd) {
    Wh_Log(L"Unresolved window");
    return;
  }

  switch (gesture.intent) {
    case MoveSizeIntent::None:
      ArrangeWorkspace(key);
      break;

    case MoveSizeIntent::Move:
      HandleUserMove(key, state, tiledHwnd, gesture);
      break;

    case MoveSizeIntent::Resize:
      HandleUserResize(
          key, state, tiledHwnd, metrics.workArea, metrics.gap, gesture);
      break;
  }
}

}  // namespace Commands

//=============================================================================
// Windows platform adapter: WinEvent observation + queueing
//=============================================================================
namespace Platform::WindowEvents {

void OnWindowMoveSizeEnd(HWND hwnd) {
  if (!IsWindow(hwnd)) return;
  const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
  if (!threadId) return;
  if (!PostThreadMessage(threadId, WM_APP_MOVE_SIZE_END, reinterpret_cast<WPARAM>(hwnd), 0)) {
    Wh_Log(L"Failed to queue resize event");
  }
}

static void QueueWindowEvent(DWORD event, HWND hwnd) {
  const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
  if (!threadId || !hwnd) return;
  if (!PostThreadMessage(threadId, WM_APP_WINDOW_EVENT,
                         static_cast<WPARAM>(event), reinterpret_cast<LPARAM>(hwnd))) {
    Wh_Log(L"Failed to queue lifecycle event %lu", event);
  }
}

static bool HasTrackedMonitorOwnershipMismatch(HWND hwnd) {
  const HMONITOR observed = GetWindowPhysicalMonitor(hwnd);
  Model::MonitorId observedId;
  if (!observed || !Model::MonitorId::FromHMonitor(observed, &observedId)) {
    return false;
  }

  const std::vector<DesktopMonitorKey> owners = g_workspaces.OwnersOf(hwnd);
  if (owners.empty()) return false;
  return std::any_of(
      owners.begin(), owners.end(),
      [&](const DesktopMonitorKey& owner) { return owner.monitor != observedId; });
}

// Out-of-context WinEvents are delivered on this WM thread. Keep the callback
// observation-only and post accepted work so reentrant delivery can't recursively
// mutate workspace state.
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
  AssertWmThread(L"WinEventProc");
  switch (event) {
    case EVENT_SYSTEM_FOREGROUND:
    case EVENT_SYSTEM_MOVESIZESTART:
    case EVENT_SYSTEM_MOVESIZEEND:
    case EVENT_SYSTEM_MINIMIZESTART:
    case EVENT_SYSTEM_MINIMIZEEND:
    case EVENT_OBJECT_STATECHANGE:
    case EVENT_OBJECT_LOCATIONCHANGE:
    case EVENT_OBJECT_DESTROY:
    case EVENT_OBJECT_SHOW:
    case EVENT_OBJECT_HIDE:
    case EVENT_OBJECT_CLOAKED:
    case EVENT_OBJECT_UNCLOAKED:
      break;
    default:
      return;
  }

  // Retain foreground state on the serialized WM thread. Unknown HWNDs
  // deliberately do not clear managed focus: a just-created window can become
  // foreground before Automatic discovery admits it, matching FancyWM's
  // race-safe focus semantics.
  if (event == EVENT_SYSTEM_FOREGROUND) {
    const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
    if (threadId) {
      PostThreadMessage(
          threadId, WM_APP_FOREGROUND_CHANGED,
          reinterpret_cast<WPARAM>(hwnd), 0);
    }
    return;
  }

  // For window-level events only.
  if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;

  // Opt-in rules can bridge custom frame/helper HWNDs to an already-managed root
  // owner for native move/size boundaries. Lifecycle events retain exact identity.
  if ((event == EVENT_SYSTEM_MOVESIZESTART ||
       event == EVENT_SYSTEM_MOVESIZEEND) &&
      !IsWindowTrackedInAnyState(hwnd) &&
      FindMatchingWindowRule(
          hwnd, WindowRuleTreatment::TraceToOwner)) {
    HWND rootOwner = GetAncestor(hwnd, GA_ROOTOWNER);
    if (rootOwner && IsWindowTrackedInAnyState(rootOwner)) {
      hwnd = rootOwner;
    }
  }

  const bool tracked = IsWindowTrackedInAnyState(hwnd);
  if (event == EVENT_OBJECT_DESTROY) {
    // Retire lifetime metadata when destruction is observed, before a delayed
    // model message could mistake a reused HWND for the old routed window.
    g_wm.overflowWindows.erase(hwnd);
    g_wm.pendingWindowOverflow.erase(
        std::remove_if(g_wm.pendingWindowOverflow.begin(), g_wm.pendingWindowOverflow.end(),
                       [hwnd](const PendingWindowOverflow& pending) { return pending.hwnd == hwnd; }),
        g_wm.pendingWindowOverflow.end());
    if (g_wm.pendingOverflowSwitch.hwnd == hwnd) g_wm.pendingOverflowSwitch = {};
    if (g_wm.pendingInitialPlacementSwitch.hwnd == hwnd) g_wm.pendingInitialPlacementSwitch = {};
    const bool routingKnown = g_wm.windowRoutingEvaluated.erase(hwnd) != 0;
    const bool routingPending = g_wm.pendingWindowRoutes.erase(hwnd) != 0;
    if (tracked || routingKnown || routingPending) QueueWindowEvent(event, hwnd);
    return;
  }

  // A tiled LOCATIONCHANGE is classified later on the WM actor. Expected placement
  // echoes are cheap no-ops there; unexpected same-monitor drift opens a bounded
  // conformance lease instead of being silently discarded.
  if (event == EVENT_OBJECT_LOCATIONCHANGE) {
    if (!tracked) {
      if (g_wm.pendingWindowRoutes.count(hwnd)) {
        QueueWindowEvent(event, hwnd);
        return;
      }
      // A newly created window can already be minimized, maximized, or fullscreen
      // before the ordinary discovery snapshot sees it. Queue those states for the
      // inactive-admission path.
      const SuspensionReason reason = GetPhysicalSuspensionReason(hwnd);
      if ((reason != SuspensionReason::Minimized &&
           reason != SuspensionReason::Maximized) ||
          !WindowCanBeManaged(hwnd)) {
        return;
      }
      QueueWindowEvent(event, hwnd);
      return;
    }

    const bool overflowPending = std::any_of(
        g_wm.pendingWindowOverflow.begin(), g_wm.pendingWindowOverflow.end(),
        [hwnd](const PendingWindowOverflow& pending) { return pending.hwnd == hwnd; });
    const bool stateOrLeaseNeedsObservation =
        overflowPending || g_workspaces.IsSuspendedMaximized(hwnd) ||
        HasActiveConformanceLease(hwnd) ||
        g_workspaces.IsTiled(hwnd);
    if (!stateOrLeaseNeedsObservation &&
        !HasTrackedMonitorOwnershipMismatch(hwnd)) {
      return;
    }

    QueueWindowEvent(event, hwnd);
    return;
  }

  // Unknown top-level lifecycle events are only discovery prompts. Do not apply
  // the full eligibility policy inside the callback: a newly shown framework
  // window can still be acquiring its final styles, owner, title, or desktop
  // registration. The settled snapshot remains authoritative for admission, and
  // Manual mode still blocks admission at the model boundary.
  if (!tracked) {
    const bool discoveryEvent =
        event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED ||
        event == EVENT_OBJECT_STATECHANGE ||
        event == EVENT_SYSTEM_MINIMIZESTART ||
        event == EVENT_SYSTEM_MINIMIZEEND ||
        ((event == EVENT_SYSTEM_MOVESIZESTART ||
          event == EVENT_SYSTEM_MOVESIZEEND) &&
         (g_wm.windowRoutingReady || IsAutomaticMode()));
    if (!discoveryEvent || GetAncestor(hwnd, GA_ROOT) != hwnd) {
      return;
    }
  }

  // Explicit user manipulation always wins over a transient placement lease.
  // Lease cancellation is actor-local; workspace mutation remains deferred until
  // normal message dispatch.
  if (event == EVENT_SYSTEM_MOVESIZESTART) {
    g_wm.overflowWindows.erase(hwnd);
    g_wm.pendingWindowOverflow.erase(
        std::remove_if(g_wm.pendingWindowOverflow.begin(), g_wm.pendingWindowOverflow.end(),
                       [hwnd](const PendingWindowOverflow& pending) { return pending.hwnd == hwnd; }),
        g_wm.pendingWindowOverflow.end());
    if (g_wm.pendingOverflowSwitch.hwnd == hwnd) g_wm.pendingOverflowSwitch = {};
    if (g_wm.pendingInitialPlacementSwitch.hwnd == hwnd) g_wm.pendingInitialPlacementSwitch = {};
    if (g_wm.windowRoutingReady) {
      g_wm.windowRoutingEvaluated.insert(hwnd);
      g_wm.pendingWindowRoutes.erase(hwnd);
    }
    CancelConformanceLease(hwnd);
  }

  // Untracked drags only cancel initial routing. They must not reach the
  // managed-window gesture handler or leave samples for an unrelated workspace.
  if (!tracked && (event == EVENT_SYSTEM_MOVESIZESTART ||
                   event == EVENT_SYSTEM_MOVESIZEEND)) {
    if (event == EVENT_SYSTEM_MOVESIZEEND) QueueWindowEvent(EVENT_OBJECT_SHOW, hwnd);
    return;
  }

  // Cache user move/resize boundaries. The callback only records facts; model
  // mutation and layout work happen later on the WM thread.
  if (event == EVENT_SYSTEM_MOVESIZESTART || event == EVENT_SYSTEM_MOVESIZEEND) {
    RECT r{};
    if (GetWindowFrameRect(hwnd, &r)) {
      if (event == EVENT_SYSTEM_MOVESIZESTART) {
        g_moveSize.startRects[hwnd] = r;
      } else {
        g_moveSize.endRects[hwnd] = r;
        POINT pt{};
        if (GetCursorPos(&pt)) g_moveSize.endPoints[hwnd] = pt;
      }
    }
    if (event == EVENT_SYSTEM_MOVESIZEEND) OnWindowMoveSizeEnd(hwnd);
    return;
  }

  // Accepted lifecycle events are serialized on the WM thread.
  // PostThreadMessage keeps model mutation out of the WinEvent callback.
  if (event == EVENT_OBJECT_CLOAKED || event == EVENT_OBJECT_UNCLOAKED) {
    // Older notification ABIs rely on the settled cloak/uncloak fallback to
    // discover desktop switches. Until that pass completes, preserve the current
    // behavior by making active-desktop checks query Explorer directly.
    if (g_wm.reconciledDesktopState != ReconciledDesktopState::Unknown) {
      g_wm.reconciledDesktopState =
          ReconciledDesktopState::TransitionPending;
    }
  }
  QueueWindowEvent(event, hwnd);
}

bool AllWinEventHooksInstalled() {
  return g_hooks.foreground && g_hooks.locationChange && g_hooks.moveSize &&
         g_hooks.minimize && g_hooks.hideDestroy && g_hooks.cloak && g_hooks.state;
}

bool InstallWinEventHooks() {
  AssertWmThread(L"Platform::WindowEvents::InstallWinEventHooks");
  auto noteFailure = [](const wchar_t* name) {
    ++Diagnostics::g_runtime.counters.winEventHookInstallFailures;
    Wh_Log(L"Failed to install %s WinEvent hook: %lu", name, GetLastError());
  };

  if (!g_hooks.foreground) {
    g_hooks.foreground = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_hooks.foreground) noteFailure(L"foreground");
  }

  if (!g_hooks.locationChange) {
    g_hooks.locationChange = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_hooks.locationChange) noteFailure(L"location-change");
  }

  if (!g_hooks.moveSize) {
    g_hooks.moveSize = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                                      nullptr, WinEventProc, 0, 0,
                                      WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_hooks.moveSize) noteFailure(L"move/size");
  }

  if (!g_hooks.minimize) {
    g_hooks.minimize = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND,
                                      nullptr, WinEventProc, 0, 0,
                                      WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_hooks.minimize) noteFailure(L"minimization");
  }

  if (!g_hooks.hideDestroy) {
    g_hooks.hideDestroy = SetWinEventHook(
      EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE,
      nullptr, WinEventProc, 0, 0,
      WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
    );
    if (!g_hooks.hideDestroy) noteFailure(L"hide/destroy");
  }

  if (!g_hooks.cloak) {
    g_hooks.cloak = SetWinEventHook(
      EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED,
      nullptr, WinEventProc, 0, 0,
      WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
    );
    if (!g_hooks.cloak) noteFailure(L"cloak");
  }

  if (!g_hooks.state) {
    g_hooks.state = SetWinEventHook(
        EVENT_OBJECT_STATECHANGE, EVENT_OBJECT_STATECHANGE,
        nullptr, WinEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_hooks.state) noteFailure(L"state-change");
  }

  return AllWinEventHooksInstalled();
}

void RemoveWinEventHooks() {
    if (g_hooks.foreground) {
        UnhookWinEvent(g_hooks.foreground);
        g_hooks.foreground = nullptr;
    }
    if (g_hooks.locationChange) {
        UnhookWinEvent(g_hooks.locationChange);
        g_hooks.locationChange = nullptr;
    }
    if (g_hooks.moveSize) {
        UnhookWinEvent(g_hooks.moveSize);
        g_hooks.moveSize = nullptr;
    }
    if (g_hooks.minimize) {
        UnhookWinEvent(g_hooks.minimize);
        g_hooks.minimize = nullptr;
    }
    if (g_hooks.hideDestroy) {
        UnhookWinEvent(g_hooks.hideDestroy);
        g_hooks.hideDestroy = nullptr;
    }
    if (g_hooks.cloak) {
        UnhookWinEvent(g_hooks.cloak);
        g_hooks.cloak = nullptr;
    }
    if (g_hooks.state) {
        UnhookWinEvent(g_hooks.state);
        g_hooks.state = nullptr;
    }
}

}  // namespace Platform::WindowEvents

//=============================================================================
// Windows platform adapters: private virtual desktops
//=============================================================================
//
// Everything below this point is shell/COM adaptation. The model, layout engine,
// reconciliation policy, commands, and WinEvent adapter above depend only on the
// narrow declarations near the top of the source.

namespace Platform::VirtualDesktop {

template <typename T>
T GetVTableFunction(void* pInterface, int index) {
  return reinterpret_cast<T>((*reinterpret_cast<void***>(pInterface))[index]);
}


//=============================================================================
// COM CLSIDs and IIDs for Virtual Desktop API (undocumented)
//=============================================================================

static const CLSID CLSID_ImmersiveShell = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

static const CLSID CLSID_VirtualDesktopManagerInternal = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

// Private shell service used for virtual-desktop change notifications.
// These identifiers and ABI generations mirror Windhawk's Taskbar Desktop
// Indicator, while the WM consumes them through its own message queue.
static const GUID SID_VirtualDesktopNotificationService = {
    0xA501FDEC, 0x4A09, 0x464C, {0xAE, 0x4E, 0x1B, 0x9C, 0x21, 0xB8, 0x49, 0x18}};

static const IID IID_IVirtualDesktopNotificationService = {
    0x0CD45E71, 0xD927, 0x4F15, {0x8B, 0x0A, 0x8F, 0xEF, 0x52, 0x53, 0x37, 0xBF}};

static const IID IID_IVirtualDesktopNotification_Old = {
    0xCD403E52, 0xDEED, 0x4C13, {0xB4, 0x37, 0xB9, 0x83, 0x80, 0xF2, 0xB1, 0xE8}};

static const IID IID_IVirtualDesktopNotification_Intermediate = {
    0xB287FA1C, 0x7771, 0x471A, {0xA2, 0xDF, 0x9B, 0x6B, 0x21, 0xF0, 0xD6, 0x75}};

static const IID IID_IVirtualDesktopNotification_Current = {
    0xB9E5E94D, 0x233E, 0x49AB, {0xAF, 0x5C, 0x2B, 0x45, 0x41, 0xC3, 0xAA, 0xDE}};

//=============================================================================
// Virtual Desktop private ABI identifiers
//=============================================================================

// Manager interface generations. Selection is automatic from explorer.exe's
// build; there is no user-facing Windows-version override.
static const IID IID_IVirtualDesktopManagerInternal_Win10Old = {
    0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}};
static const IID IID_IVirtualDesktopManagerInternal_Win10_20348 = {
    0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}};
static const IID IID_IVirtualDesktopManagerInternal_Win11_22000 = {
    0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}};
static const IID IID_IVirtualDesktopManagerInternal_Win11_22621 = {
    0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}};
static const IID IID_IVirtualDesktopManagerInternal_Win11_26100 = {
    0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}};

//=============================================================================
// COM Interface Definitions (undocumented, reverse-engineered)
//=============================================================================
struct IVirtualDesktop : public IUnknown {
  virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown*, BOOL*) = 0;
  virtual HRESULT STDMETHODCALLTYPE GetId(GUID*) = 0;
};

struct IVirtualDesktopManagerInternal : public IUnknown {};

MIDL_INTERFACE("0CD45E71-D927-4F15-8B0A-8FEF525337BF")
IVirtualDesktopNotificationService : public IUnknown {
 public:
  virtual HRESULT STDMETHODCALLTYPE Register(IUnknown* notification, DWORD* cookie) = 0;
  virtual HRESULT STDMETHODCALLTYPE Unregister(DWORD cookie) = 0;
};

struct VirtualDesktopNotificationObject {
  void** vtable = nullptr;
  LONG refCount = 1;
};

bool UsesHMonitorParameter() { return g_vd.abi.usesHMonitor; }


//=============================================================================
// Virtual Desktop API + native change notifications
//=============================================================================

DesktopMetadata ReadDesktopMetadata(const GUID& desktopId) {
  // Read only at the initial-layout boundary. Missing shell metadata leaves the
  // corresponding condition unmatched; it never changes existing workspaces.
  DesktopMetadata desktop;
  DWORD sessionId = 0;
  const bool haveSession = ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
  const std::wstring registryPaths[] = {
      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\VirtualDesktops",
      haveSession
          ? L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\SessionInfo\\" +
                std::to_wstring(sessionId) + L"\\VirtualDesktops"
          : std::wstring{}};

  for (const auto& path : registryPaths) {
    if (path.empty()) continue;
    DWORD bytes = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, path.c_str(), L"VirtualDesktopIDs",
                     RRF_RT_REG_BINARY, nullptr, nullptr, &bytes) != ERROR_SUCCESS ||
        bytes == 0 || bytes > 65536 || bytes % sizeof(GUID) != 0) {
      continue;
    }
    std::vector<GUID> ids(bytes / sizeof(GUID));
    if (RegGetValueW(HKEY_CURRENT_USER, path.c_str(), L"VirtualDesktopIDs",
                     RRF_RT_REG_BINARY, nullptr, ids.data(), &bytes) != ERROR_SUCCESS ||
        bytes % sizeof(GUID) != 0) {
      continue;
    }
    ids.resize(bytes / sizeof(GUID));
    auto found = std::find_if(ids.begin(), ids.end(), [&](const GUID& id) {
      return IsEqualGUID(id, desktopId);
    });
    if (found != ids.end()) {
      desktop.number = static_cast<int>(found - ids.begin()) + 1;
      break;
    }
  }

  wchar_t desktopIdText[39]{};
  if (!StringFromGUID2(desktopId, desktopIdText, ARRAYSIZE(desktopIdText))) {
    return desktop;
  }
  for (const auto& path : registryPaths) {
    if (path.empty()) continue;
    const std::wstring namePath = path + L"\\Desktops\\" + desktopIdText;
    DWORD bytes = 0;
    if (RegGetValueW(HKEY_CURRENT_USER, namePath.c_str(), L"Name",
                     RRF_RT_REG_SZ, nullptr, nullptr, &bytes) != ERROR_SUCCESS ||
        bytes == 0 || bytes > 65536 || bytes % sizeof(wchar_t) != 0) {
      continue;
    }
    std::vector<wchar_t> name(bytes / sizeof(wchar_t) + 1, L'\0');
    if (RegGetValueW(HKEY_CURRENT_USER, namePath.c_str(), L"Name",
                     RRF_RT_REG_SZ, nullptr, name.data(), &bytes) == ERROR_SUCCESS &&
        name.front() != L'\0') {
      desktop.name = name.data();
      break;
    }
  }
  return desktop;
}

static const int VTABLE_GET_CURRENT_DESKTOP = 6;

static const NotificationInterfaceConfig& GetNotificationInterfaceConfig() {
  return g_vd.abi.notification;
}

// Resolves every private VD ABI choice from one observed Explorer version.
// Known gaps are rejected instead of guessing an undocumented manager IID.
static bool SelectVirtualDesktopAbiProfile(
    DWORD build, DWORD revision, VirtualDesktopAbiProfile* outProfile) {
  if (!outProfile) return false;

  VirtualDesktopAbiProfile profile{};

  if (build < 20348) {
    profile.managerInternal = IID_IVirtualDesktopManagerInternal_Win10Old;
    profile.virtualDesktop = {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}};
    profile.usesHMonitor = false;
  } else if (build < 22000) {
    profile.managerInternal = IID_IVirtualDesktopManagerInternal_Win10_20348;
    profile.virtualDesktop = {0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}};
    profile.usesHMonitor = true;
  } else if (build < 22483 || (build == 22621 && revision < 2215)) {
    profile.managerInternal = IID_IVirtualDesktopManagerInternal_Win11_22000;
    profile.virtualDesktop = {0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}};
    profile.usesHMonitor = true;
    if (build >= 22449) {
      profile.getDesktopsIndex = 8;
      profile.switchDesktopIndex = 10;
    }
  } else if (build >= 22621 && build < 26100) {
    profile.managerInternal = IID_IVirtualDesktopManagerInternal_Win11_22621;
    profile.virtualDesktop = {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};
    profile.usesHMonitor = false;
  } else if (build >= 26100) {
    profile.managerInternal = IID_IVirtualDesktopManagerInternal_Win11_26100;
    profile.virtualDesktop = {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};
    profile.usesHMonitor = false;
    profile.animatedSwitchIndex = 22;
    profile.animationWaitIndex = 24;
  } else {
    // The old manual table did not claim a manager ABI for Insider builds
    // 22483..22620. Keep that gap explicit rather than silently guessing.
    return false;
  }

  if (build >= 22000) {
    profile.createDesktopIndex = profile.switchDesktopIndex + 1;
    if (build >= 26100) ++profile.createDesktopIndex;
    profile.moveDesktopIndex = profile.createDesktopIndex + 1;
    profile.removeDesktopIndex = profile.createDesktopIndex + 2;
  }

  // Mirrors Windhawk's Taskbar Desktop Indicator notification ABI table. The
  // architecture metadata guarantees a 64-bit caller-cleanup ABI; retain the
  // compile guard as a belt-and-braces constraint around the manual sink.
#if defined(_WIN64)
  if (build >= 22000) {
    if (build < 22483 ||
        (build == 22621 && revision < 2215)) {
      profile.notification = {IID_IVirtualDesktopNotification_Old, 13, 11, true};
    } else if (build < 22631 ||
               (build == 22631 && revision < 3085)) {
      profile.notification = {
          IID_IVirtualDesktopNotification_Intermediate, 14, 10, false};
    } else {
      profile.notification = {IID_IVirtualDesktopNotification_Current, 14, 10, false};
    }
  }
#endif

  *outProfile = profile;
  return true;
}

static bool ReadExplorerVersion(DWORD* outBuild, DWORD* outRevision) {
  if (!outBuild || !outRevision) return false;

  wchar_t explorerPath[MAX_PATH]{};
  UINT length = GetWindowsDirectoryW(explorerPath, ARRAYSIZE(explorerPath));
  static constexpr wchar_t suffix[] = L"\\explorer.exe";
  if (!length || length >= ARRAYSIZE(explorerPath) ||
      length + ARRAYSIZE(suffix) > ARRAYSIZE(explorerPath)) {
    Wh_Log(L"Failed to resolve explorer.exe path for VD ABI detection");
    return false;
  }
  memcpy(explorerPath + length, suffix, sizeof(suffix));

  DWORD ignored = 0;
  DWORD versionSize = GetFileVersionInfoSizeW(explorerPath, &ignored);
  if (!versionSize) {
    Wh_Log(L"GetFileVersionInfoSize(explorer.exe) failed: %lu", GetLastError());
    return false;
  }

  std::vector<BYTE> versionData(versionSize);
  if (!GetFileVersionInfoW(explorerPath, 0, versionSize, versionData.data())) {
    Wh_Log(L"GetFileVersionInfo(explorer.exe) failed: %lu", GetLastError());
    return false;
  }

  VS_FIXEDFILEINFO* fixedInfo = nullptr;
  UINT fixedInfoSize = 0;
  if (!VerQueryValueW(versionData.data(), L"\\", reinterpret_cast<void**>(&fixedInfo),
                      &fixedInfoSize) || !fixedInfo || fixedInfoSize < sizeof(*fixedInfo)) {
    Wh_Log(L"VerQueryValue(explorer.exe) failed");
    return false;
  }

  *outBuild = HIWORD(fixedInfo->dwFileVersionLS);
  *outRevision = LOWORD(fixedInfo->dwFileVersionLS);
  return true;
}

static bool ResolveVirtualDesktopAbiProfile() {
  DWORD build = 0;
  DWORD revision = 0;
  if (!ReadExplorerVersion(&build, &revision)) return false;

  VirtualDesktopAbiProfile profile{};
  if (!SelectVirtualDesktopAbiProfile(build, revision, &profile)) {
    Wh_Log(
        L"Unsupported Explorer virtual-desktop ABI: build=%lu revision=%lu",
        build, revision);
    return false;
  }

  g_vd.explorerBuild = build;
  g_vd.explorerRevision = revision;
  g_vd.abi = profile;
  g_vd.abiResolved = true;

  Wh_Log(
      L"Explorer virtual-desktop ABI resolved: build=%lu revision=%lu hmonitor=%d notifications=%d",
      build, revision, profile.usesHMonitor ? 1 : 0,
      profile.notification.methodCount);
  return true;
}

static bool IsCurrentNotificationInterface(REFIID riid) {
  NotificationInterfaceConfig config = GetNotificationInterfaceConfig();
  return config.methodCount > 0 && InlineIsEqualGUID(riid, config.iid);
}

static void QueueVirtualDesktopChangedNotification() {
  const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
  if (!threadId) return;

  // The callback is the transition boundary. Keep using the live shell query until
  // the queued handler has reconciled the new desktop into actor-owned state.
  if (g_wm.reconciledDesktopState != ReconciledDesktopState::Unknown) {
    g_wm.reconciledDesktopState =
        ReconciledDesktopState::TransitionPending;
  }

  // COM can emit redundant desktop notifications. Collapse them before they
  // reach the WM core, just as FancyWM coalesces layout invalidations.
  if (InterlockedExchange(&g_vd.changeQueued, 1) != 0) return;

  if (!PostThreadMessage(threadId, WM_APP_VIRTUAL_DESKTOP_CHANGED, 0, 0)) {
    InterlockedExchange(&g_vd.changeQueued, 0);
    Wh_Log(L"Failed to queue virtual desktop changed notification: %lu", GetLastError());
  }
}

static HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_QueryInterface(
    VirtualDesktopNotificationObject* self, REFIID riid, void** object) {
  if (!object) return E_POINTER;
  *object = nullptr;

  if (InlineIsEqualGUID(riid, IID_IUnknown) || IsCurrentNotificationInterface(riid)) {
    *object = self;
    InterlockedIncrement(&self->refCount);
    return S_OK;
  }
  return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE VirtualDesktopNotification_AddRef(
    VirtualDesktopNotificationObject* self) {
  return static_cast<ULONG>(InterlockedIncrement(&self->refCount));
}

static ULONG STDMETHODCALLTYPE VirtualDesktopNotification_Release(
    VirtualDesktopNotificationObject* self) {
  LONG refCount = InterlockedDecrement(&self->refCount);
  if (refCount == 0) {
    delete[] self->vtable;
    delete self;
  }
  return static_cast<ULONG>(std::max<LONG>(refCount, 0));
}

static HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_NoOp() {
  return S_OK;
}

// Newer notification ABIs pass old/new IVirtualDesktop pointers after `self`.
// We deliberately don't consume those private-interface parameters here: the
// callback is the authoritative *signal*, then the WM thread queries the current
// GUID through its already-versioned manager interface. This follows Windhawk's
// compatibility pattern and avoids COM lifetime/cross-thread pointer hazards.
static HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_CurrentChanged(
    VirtualDesktopNotificationObject*, void*, void*) {
  QueueVirtualDesktopChangedNotification();
  return S_OK;
}

// The oldest supported Windows 11 notification ABI inserts a monitor collection
// before old/new desktop arguments. The values are callback-owned COM inputs and
// are intentionally ignored rather than AddRef/Release'd.
static HRESULT STDMETHODCALLTYPE VirtualDesktopNotification_CurrentChangedWithMonitors(
    VirtualDesktopNotificationObject*, void*, void*, void*) {
  QueueVirtualDesktopChangedNotification();
  return S_OK;
}

// Builds the minimal COM sink required by the selected private ABI: IUnknown,
// one desktop-change callback, and no-op implementations for unrelated slots.
static VirtualDesktopNotificationObject* CreateVirtualDesktopNotificationObject() {
  NotificationInterfaceConfig config = GetNotificationInterfaceConfig();
  if (config.methodCount <= 0 || config.currentChangedIndex < 3 ||
      config.currentChangedIndex >= config.methodCount) {
    return nullptr;
  }

  auto* object = new (std::nothrow) VirtualDesktopNotificationObject();
  if (!object) return nullptr;

  object->vtable = new (std::nothrow) void*[config.methodCount];
  if (!object->vtable) {
    delete object;
    return nullptr;
  }

  for (int i = 0; i < config.methodCount; ++i) {
    object->vtable[i] = reinterpret_cast<void*>(&VirtualDesktopNotification_NoOp);
  }
  object->vtable[0] = reinterpret_cast<void*>(&VirtualDesktopNotification_QueryInterface);
  object->vtable[1] = reinterpret_cast<void*>(&VirtualDesktopNotification_AddRef);
  object->vtable[2] = reinterpret_cast<void*>(&VirtualDesktopNotification_Release);
  if (config.currentChangedHasMonitors) {
    object->vtable[config.currentChangedIndex] =
        reinterpret_cast<void*>(&VirtualDesktopNotification_CurrentChangedWithMonitors);
  } else {
    object->vtable[config.currentChangedIndex] =
        reinterpret_cast<void*>(&VirtualDesktopNotification_CurrentChanged);
  }
  return object;
}

// Clears local notification ownership and, when Explorer is responsive, also
// unregisters and releases the shell service. Teardown still discards the stale
// cookie when COM calls are deliberately skipped to avoid an Explorer hang.
static void ClearVirtualDesktopNotificationState(bool unregisterFromShell) {
  g_vd.notificationsRegistered = false;
  InterlockedExchange(&g_vd.changeQueued, 0);

  if (g_vd.notificationService) {
    if (unregisterFromShell && g_vd.notificationCookie) {
      HRESULT hr = g_vd.notificationService->Unregister(g_vd.notificationCookie);
      if (FAILED(hr)) {
        Wh_Log(L"Virtual desktop notification Unregister failed: 0x%08X", hr);
      }
    }

    if (unregisterFromShell) {
      g_vd.notificationService->Release();
    }
    // If Explorer is unavailable, avoid a potentially blocking COM call/release.
    // The old server is going away; importantly, discard its cookie *before* a
    // new registration so a reused cookie can never unregister the new sink.
    g_vd.notificationService = nullptr;
  }

  if (g_vd.notificationObject) {
    VirtualDesktopNotification_Release(g_vd.notificationObject);
    g_vd.notificationObject = nullptr;
  }

  g_vd.notificationCookie = 0;
}

bool RegisterVirtualDesktopNotifications() {
  if (g_vd.notificationsRegistered) return true;
  if (!g_vd.abiResolved) return false;

  NotificationInterfaceConfig config = GetNotificationInterfaceConfig();
  if (config.methodCount == 0) {
    // Win10 has no selected native notification ABI and deliberately uses settled
    // cloak/uncloak reconciliation instead.
    return true;
  }

  ++Diagnostics::g_runtime.counters.vdNotificationRegisterAttempts;
  g_vd.notificationLastAttemptTickMs = GetTickCount64();

  // Each attempt obtains a fresh ImmersiveShell provider, matching the robust
  // Taskbar Desktop Indicator pattern. Notification-service readiness can lag the
  // core VD manager during logon, so don't pin registration to the provider that
  // happened to initialize the manager first.
  IServiceProvider* notificationProvider = nullptr;
  HRESULT hr = CoCreateInstance(
      CLSID_ImmersiveShell, nullptr,
      CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IServiceProvider,
      reinterpret_cast<void**>(&notificationProvider));
  if (FAILED(hr) || !notificationProvider) {
    if (notificationProvider) notificationProvider->Release();
    g_vd.notificationLastHr = FAILED(hr) ? hr : E_NOINTERFACE;
    ++Diagnostics::g_runtime.counters.vdNotificationRegisterFailures;
    Wh_Log(L"VD notification registration: ImmersiveShell unavailable: 0x%08X", hr);
    return false;
  }

  if (g_vd.notificationService || g_vd.notificationObject ||
      g_vd.notificationCookie) {
    ClearVirtualDesktopNotificationState(true);
  }

  hr = notificationProvider->QueryService(
      SID_VirtualDesktopNotificationService, IID_IVirtualDesktopNotificationService,
      reinterpret_cast<void**>(&g_vd.notificationService));
  notificationProvider->Release();
  if (FAILED(hr) || !g_vd.notificationService) {
    SAFE_RELEASE(g_vd.notificationService);
    g_vd.notificationLastHr = FAILED(hr) ? hr : E_NOINTERFACE;
    ++Diagnostics::g_runtime.counters.vdNotificationRegisterFailures;
    Wh_Log(L"VD notification registration: service unavailable: 0x%08X", hr);
    return false;
  }

  g_vd.notificationObject = CreateVirtualDesktopNotificationObject();
  if (!g_vd.notificationObject) {
    g_vd.notificationLastHr = E_OUTOFMEMORY;
    ++Diagnostics::g_runtime.counters.vdNotificationRegisterFailures;
    SAFE_RELEASE(g_vd.notificationService);
    Wh_Log(L"VD notification registration: failed to create sink");
    return false;
  }

  DWORD cookie = 0;
  hr = g_vd.notificationService->Register(
      reinterpret_cast<IUnknown*>(g_vd.notificationObject), &cookie);
  if (FAILED(hr) || cookie == 0) {
    g_vd.notificationLastHr = FAILED(hr) ? hr : E_FAIL;
    ++Diagnostics::g_runtime.counters.vdNotificationRegisterFailures;
    Wh_Log(L"VD notification registration failed: 0x%08X cookie=%lu", hr, cookie);
    VirtualDesktopNotification_Release(g_vd.notificationObject);
    g_vd.notificationObject = nullptr;
    SAFE_RELEASE(g_vd.notificationService);
    return false;
  }

  g_vd.notificationCookie = cookie;
  g_vd.notificationsRegistered = true;
  if (!g_vd.shellPid) {
    HWND shell = GetShellWindow();
    if (shell && IsWindow(shell)) GetWindowThreadProcessId(shell, &g_vd.shellPid);
  }
  g_vd.notificationLastHr = S_OK;
  g_vd.notificationLastSuccessTickMs = GetTickCount64();
  ++Diagnostics::g_runtime.counters.vdNotificationRegisterSuccesses;
  Diagnostics::RecordEvent(
      L"virtual desktop notifications registered cookie=%lu afterAttempt=%llu",
      cookie,
      static_cast<unsigned long long>(
          Diagnostics::g_runtime.counters.vdNotificationRegisterAttempts));
  Wh_Log(L"Native virtual desktop notifications registered (cookie=%lu)", cookie);
  return true;
}

// Acquires the COM interfaces needed for virtual-desktop queries. Notification
// subscription is deliberately owned by the centralized runtime-maintenance path.
bool InitializeVirtualDesktopAPIOnce() {
  ++Diagnostics::g_runtime.counters.vdApiInitAttempts;
  if (!ResolveVirtualDesktopAbiProfile()) {
    ++Diagnostics::g_runtime.counters.vdApiInitFailures;
    return false;
  }

  HRESULT hr = CoCreateInstance(
      CLSID_ImmersiveShell, nullptr,
      CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IServiceProvider,
      reinterpret_cast<void**>(&g_vd.serviceProvider));
  if (FAILED(hr) || !g_vd.serviceProvider) {
    SAFE_RELEASE(g_vd.serviceProvider);
    Wh_Log(L"Failed to create ImmersiveShell: 0x%08X", hr);
    ++Diagnostics::g_runtime.counters.vdApiInitFailures;
    return false;
  }

  if (g_vd.explorerBuild >= 22621) {
    hr = g_vd.serviceProvider->QueryService(
        CLSID_VirtualDesktopManagerInternal, IID_IVirtualDesktopManagerInternal_Win11_26100,
        reinterpret_cast<void**>(&g_vd.managerInternal));
    if (SUCCEEDED(hr) && g_vd.managerInternal) {
      g_vd.abi.managerInternal = IID_IVirtualDesktopManagerInternal_Win11_26100;
      g_vd.abi.virtualDesktop = {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}};
      g_vd.abi.usesHMonitor = false;
      g_vd.abi.getDesktopsIndex = 7;
      g_vd.abi.switchDesktopIndex = 9;
      // MScholtes supplies separate 23H2/24H2 definitions for this same IID:
      // 24H2 inserts SwitchDesktopAndMoveForegroundView at slot 10.
      g_vd.abi.animatedSwitchIndex = g_vd.explorerBuild >= 26100 ? 22 : 21;
      g_vd.abi.animationWaitIndex = g_vd.abi.animatedSwitchIndex + 2;
      g_vd.abi.createDesktopIndex = g_vd.explorerBuild >= 26100 ? 11 : 10;
      g_vd.abi.moveDesktopIndex = g_vd.abi.createDesktopIndex + 1;
      g_vd.abi.removeDesktopIndex = g_vd.abi.createDesktopIndex + 2;
    }
  }
  if (!g_vd.managerInternal) {
    hr = g_vd.serviceProvider->QueryService(
        CLSID_VirtualDesktopManagerInternal, g_vd.abi.managerInternal,
        reinterpret_cast<void**>(&g_vd.managerInternal));
  }
  if (FAILED(hr) || !g_vd.managerInternal) {
    Wh_Log(L"Failed to get VirtualDesktopManagerInternal: 0x%08X", hr);
    SAFE_RELEASE(g_vd.managerInternal);
    SAFE_RELEASE(g_vd.serviceProvider);
    ++Diagnostics::g_runtime.counters.vdApiInitFailures;
    return false;
  }

  hr = CoCreateInstance(
      CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
      IID_IVirtualDesktopManager,
      reinterpret_cast<void**>(&g_vd.desktopManager));
  if (FAILED(hr) || !g_vd.desktopManager) {
    Wh_Log(L"Failed to create VirtualDesktopManager: 0x%08X", hr);
    SAFE_RELEASE(g_vd.desktopManager);
    SAFE_RELEASE(g_vd.managerInternal);
    SAFE_RELEASE(g_vd.serviceProvider);
    ++Diagnostics::g_runtime.counters.vdApiInitFailures;
    return false;
  }

  HWND shell = GetShellWindow();
  g_vd.shellPid = 0;
  if (shell && IsWindow(shell)) {
    GetWindowThreadProcessId(shell, &g_vd.shellPid);
  }

  ++Diagnostics::g_runtime.counters.vdApiInitSuccesses;
  return true;
}

static void ResetVirtualDesktopMetadata() {
  g_vd.abi = {};
  g_vd.abiResolved = false;
  g_vd.explorerBuild = 0;
  g_vd.explorerRevision = 0;
  g_vd.shellPid = 0;
  g_vd.lastKnownDesktop = {};
  g_vd.lastKnownDesktopValid = false;
  g_vd.notificationLastHr = S_OK;
  g_vd.notificationLastAttemptTickMs = 0;
  g_vd.notificationLastSuccessTickMs = 0;
  g_vd.initialized = false;
}

// Releases virtual-desktop COM state while Explorer is responsive. During shell
// teardown, pointers are intentionally dropped without Release to avoid hangs.
void CleanupVirtualDesktopAPI() {
  HWND hShell = GetShellWindow();
  DWORD currentShellPid = 0;
  const bool shellAvailable = hShell && IsWindow(hShell);
  if (shellAvailable) GetWindowThreadProcessId(hShell, &currentShellPid);
  const bool sameShell =
      shellAvailable && g_vd.shellPid != 0 && currentShellPid == g_vd.shellPid;

  // A replacement Explorer can already have a valid ShellWindow while our COM
  // proxies still point at the dead predecessor. Never call Unregister/Release on
  // those stale proxies merely because *some* shell is now visible.
  ClearVirtualDesktopNotificationState(sameShell);
  if (!sameShell) {
    if (g_vd.initialized || g_vd.serviceProvider || g_vd.managerInternal ||
        g_vd.desktopManager) {
      Wh_Log(L"Explorer identity changed/unavailable, abandoning stale VD COM proxies");
    }
    g_vd.desktopManager = nullptr;
    g_vd.managerInternal = nullptr;
    g_vd.serviceProvider = nullptr;
    ResetVirtualDesktopMetadata();
    return;
  }

  SAFE_RELEASE(g_vd.desktopManager);
  SAFE_RELEASE(g_vd.managerInternal);
  SAFE_RELEASE(g_vd.serviceProvider);
  ResetVirtualDesktopMetadata();
}

// TaskbarCreated means the Explorer instance backing these private proxies has
// changed. Never call stale shell proxies merely because the replacement shell is
// already visible; discard them and let the one maintenance owner reacquire all.
void AbandonVirtualDesktopAPIForShellRestart() {
  ClearVirtualDesktopNotificationState(false);
  g_vd.desktopManager = nullptr;
  g_vd.managerInternal = nullptr;
  g_vd.serviceProvider = nullptr;
  ResetVirtualDesktopMetadata();
}

bool InitializeVirtualDesktopAPI() {
  if (g_vd.initialized) return true;
  if (InitializeVirtualDesktopAPIOnce()) {
    g_vd.initialized = true;
    return true;
  }
  RuntimeLifecycle::RequestMaintenance(false);
  return false;
}

bool ReinitializeVirtualDesktopAPI() {
  ++Diagnostics::g_runtime.counters.vdApiReinitializations;
  Diagnostics::RecordEvent(L"virtual desktop core API reinitialization requested");
  CleanupVirtualDesktopAPI();
  const bool ready = InitializeVirtualDesktopAPI();
  // A previously-live COM path failed, so give the centralized recovery owner a
  // fresh retry burst. Notification registration remains owned there.
  RuntimeLifecycle::RequestMaintenance(true);
  return ready;
}

// Per-window IVirtualDesktopManager calls can legitimately reject or lose an
// individual HWND. Rebuild the shared COM stack only when the proxy/server itself
// is no longer usable.
static bool IsDeadVirtualDesktopProxy(HRESULT hr) {
  return hr == RPC_E_DISCONNECTED ||
         hr == RPC_E_CONNECTION_TERMINATED ||
         hr == RPC_E_SERVER_DIED ||
         hr == RPC_E_SERVER_DIED_DNE ||
         hr == RPC_E_INVALID_IPID ||
         hr == RPC_E_INVALID_OBJECT ||
         hr == CO_E_OBJNOTCONNECTED ||
         hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE);
}

// Invokes a private manager slot using the Windows-version-specific signature.
// A failed call forces one core-API reinitialization and retry before returning.
template <typename TResult>
HRESULT CallManagerInternal(int vtableIndex, TResult* outResult, bool enumerateDesktops = false) {
  HRESULT hr = E_NOINTERFACE;
  for (int attempt = 0; attempt < 2; ++attempt) {
    if (!g_vd.managerInternal) return hr;
    const int slot = enumerateDesktops ? g_vd.abi.getDesktopsIndex : vtableIndex;
    if (UsesHMonitorParameter()) {
      const auto function = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(g_vd.managerInternal, slot);
      hr = function(g_vd.managerInternal, nullptr, outResult);
    } else {
      const auto function = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(g_vd.managerInternal, slot);
      hr = function(g_vd.managerInternal, outResult);
    }
    if (SUCCEEDED(hr) || attempt != 0 || !ReinitializeVirtualDesktopAPI()) break;
  }
  return hr;
}

// Enumeration/movement follow the shell interfaces used by Virtual Desktop
// Helper and VD.ahk. Animation and its versioned slots follow MScholtes'
// separate VirtualDesktop11.cs and VirtualDesktop11-24H2.cs definitions.
bool GetDesktopIdsInOrder(std::vector<GUID>* desktopIds) {
  if (!desktopIds) return false;
  desktopIds->clear();
  if (!InitializeVirtualDesktopAPI()) return false;
  IObjectArray* desktops = nullptr;
  HRESULT hr = CallManagerInternal(g_vd.abi.getDesktopsIndex, &desktops, true);
  if (FAILED(hr) || !desktops) {
    SAFE_RELEASE(desktops);
    return false;
  }
  UINT count = 0;
  hr = desktops->GetCount(&count);
  for (UINT index = 0; SUCCEEDED(hr) && index < count; ++index) {
    IVirtualDesktop* desktop = nullptr;
    hr = desktops->GetAt(index, g_vd.abi.virtualDesktop, reinterpret_cast<void**>(&desktop));
    GUID id{};
    if (SUCCEEDED(hr) && desktop) hr = desktop->GetId(&id);
    else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
    SAFE_RELEASE(desktop);
    if (SUCCEEDED(hr)) desktopIds->push_back(id);
  }
  desktops->Release();
  if (FAILED(hr)) desktopIds->clear();
  return SUCCEEDED(hr);
}

// Resolve a GUID against a fresh shell array at each mutation boundary. Holding
// an ordinal or a desktop pointer across a dynamic-desktop update is unsafe.
static IVirtualDesktop* FindLiveDesktop(const GUID& desktopId) {
  if (!InitializeVirtualDesktopAPI()) return nullptr;
  IObjectArray* desktops = nullptr;
  HRESULT hr = CallManagerInternal(g_vd.abi.getDesktopsIndex, &desktops, true);
  IUnknown* const manager = g_vd.managerInternal;
  if (FAILED(hr) || !desktops) {
    SAFE_RELEASE(desktops);
    return nullptr;
  }
  UINT count = 0;
  if (FAILED(desktops->GetCount(&count))) count = 0;
  IVirtualDesktop* found = nullptr;
  for (UINT index = 0; index < count; ++index) {
    IVirtualDesktop* desktop = nullptr;
    GUID id{};
    if (SUCCEEDED(desktops->GetAt(index, g_vd.abi.virtualDesktop, reinterpret_cast<void**>(&desktop))) &&
        desktop && SUCCEEDED(desktop->GetId(&id)) && IsEqualGUID(id, desktopId)) {
      found = desktop;
      break;
    }
    SAFE_RELEASE(desktop);
  }
  desktops->Release();
  if (g_vd.managerInternal != manager) SAFE_RELEASE(found);
  return found;
}

// Read shell views rather than guessing occupancy from cloaked top-level HWNDs.
// An unregistered ApplicationFrameWindow is absent here and cannot block routing.
bool IsDesktopEmptyExcept(const GUID& desktopId, HWND allowedWindow) {
  if (!InitializeVirtualDesktopAPI()) return false;
  IUnknown* const manager = g_vd.managerInternal;
  const IID collectionIid =
      {0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};
  const IID viewIid =
      {0x372E1D3B, 0x38D3, 0x42E4, {0xA1, 0x5B, 0x8A, 0xB2, 0xB1, 0x78, 0xF5, 0x13}};
  IUnknown* collection = nullptr;
  IObjectArray* views = nullptr;
  IUnknown* allowedIdentity = nullptr;
  HRESULT hr = g_vd.serviceProvider->QueryService(
      collectionIid, collectionIid, reinterpret_cast<void**>(&collection));
  if (SUCCEEDED(hr) && collection && allowedWindow) {
    IUnknown* view = nullptr;
    hr = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HWND, IUnknown**)>(collection, 6)(
        collection, allowedWindow, &view);
    if (SUCCEEDED(hr) && view) hr = view->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&allowedIdentity));
    else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
    SAFE_RELEASE(view);
  }
  if (SUCCEEDED(hr) && collection) {
    hr = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IObjectArray**)>(collection, 3)(collection, &views);
  } else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
  UINT count = 0;
  if (SUCCEEDED(hr) && views) hr = views->GetCount(&count);
  else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
  bool empty = SUCCEEDED(hr);
  for (UINT index = 0; empty && index < count; ++index) {
    IUnknown* view = nullptr;
    IUnknown* identity = nullptr;
    GUID owner{};
    hr = views->GetAt(index, viewIid, reinterpret_cast<void**>(&view));
    if (SUCCEEDED(hr) && view) {
      // IApplicationView derives from IInspectable; GetVirtualDesktopId is slot 25.
      hr = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, GUID*)>(view, 25)(view, &owner);
      if (SUCCEEDED(hr) && IsEqualGUID(owner, desktopId)) {
        hr = view->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&identity));
        empty = SUCCEEDED(hr) && allowedIdentity && identity == allowedIdentity;
      }
    } else if (SUCCEEDED(hr)) hr = E_NOINTERFACE;
    if (FAILED(hr)) empty = false;
    SAFE_RELEASE(identity);
    SAFE_RELEASE(view);
  }
  SAFE_RELEASE(allowedIdentity);
  SAFE_RELEASE(views);
  SAFE_RELEASE(collection);
  return empty && g_vd.managerInternal == manager;
}

bool CreateDesktopAfter(const GUID& sourceDesktop, GUID* desktopId, int* desktopNumber) {
  if (!desktopId || !desktopNumber) return false;
  *desktopId = {};
  *desktopNumber = 0;
  std::vector<GUID> order;
  GUID current{};
  if (!GetDesktopIdsInOrder(&order) || !GetCurrentDesktopId(&current) ||
      !IsEqualGUID(current, sourceDesktop)) return false;
  auto source = std::find_if(order.begin(), order.end(),
      [&](const GUID& id) { return IsEqualGUID(id, sourceDesktop); });
  if (source == order.end() ||
      (g_vd.abi.moveDesktopIndex < 0 && source + 1 != order.end())) return false;
  IUnknown* const manager = g_vd.managerInternal;
  IVirtualDesktop* created = nullptr;
  // Creation is a mutation: never retry it after a lost COM response.
  HRESULT hr = UsesHMonitorParameter()
      ? GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, IVirtualDesktop**)>(manager, g_vd.abi.createDesktopIndex)(manager, nullptr, &created)
      : GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop**)>(manager, g_vd.abi.createDesktopIndex)(manager, &created);
  if (created) created->GetId(desktopId);
  SAFE_RELEASE(created);
  if (IsEqualGUID(*desktopId, GUID_NULL)) {
    Diagnostics::RecordEvent(L"overflow desktop creation returned no identity hr=%08X", hr);
    if (IsDeadVirtualDesktopProxy(hr)) RuntimeLifecycle::RequestMaintenance(true);
    return false;
  }
  if (!GetDesktopIdsInOrder(&order) || g_vd.managerInternal != manager ||
      !GetCurrentDesktopId(&current) || !IsEqualGUID(current, sourceDesktop)) return false;
  source = std::find_if(order.begin(), order.end(),
      [&](const GUID& id) { return IsEqualGUID(id, sourceDesktop); });
  auto target = std::find_if(order.begin(), order.end(),
      [&](const GUID& id) { return IsEqualGUID(id, *desktopId); });
  if (source == order.end() || target == order.end()) return false;
  if (target != source + 1) {
    if (g_vd.abi.moveDesktopIndex < 0 || !IsDesktopEmptyExcept(*desktopId) ||
        g_vd.managerInternal != manager || !GetCurrentDesktopId(&current) ||
        !IsEqualGUID(current, sourceDesktop)) return false;
    const int index = static_cast<int>(source - order.begin()) + (target < source ? 0 : 1);
    created = FindLiveDesktop(*desktopId);
    if (!created || g_vd.managerInternal != manager) { SAFE_RELEASE(created); return false; }
    hr = UsesHMonitorParameter()
        ? GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop*, HMONITOR, int)>(manager, g_vd.abi.moveDesktopIndex)(manager, created, nullptr, index)
        : GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop*, int)>(manager, g_vd.abi.moveDesktopIndex)(manager, created, index);
    created->Release();
  }
  if (!GetDesktopIdsInOrder(&order) || g_vd.managerInternal != manager) return false;
  source = std::find_if(order.begin(), order.end(),
      [&](const GUID& id) { return IsEqualGUID(id, sourceDesktop); });
  target = std::find_if(order.begin(), order.end(),
      [&](const GUID& id) { return IsEqualGUID(id, *desktopId); });
  const bool inserted = source != order.end() && target != order.end() && target == source + 1;
  if (inserted) *desktopNumber = static_cast<int>(target - order.begin()) + 1;
  Diagnostics::RecordEvent(L"overflow desktop creation target=%08X hr=%08X inserted=%d", desktopId->Data1, hr, inserted ? 1 : 0);
  if (IsDeadVirtualDesktopProxy(hr)) RuntimeLifecycle::RequestMaintenance(true);
  return inserted;
}

// Only callers holding the GUID returned by their own creation may use this.
// Never delete a selected desktop or one containing any shell application view.
bool RemoveEmptyOverflowDesktop(const GUID& desktopId, const GUID& fallbackDesktop) {
  if (IsEqualGUID(desktopId, GUID_NULL) || IsEqualGUID(desktopId, fallbackDesktop)) return false;
  IVirtualDesktop* desktop = FindLiveDesktop(desktopId);
  IUnknown* const manager = g_vd.managerInternal;
  IVirtualDesktop* fallback = FindLiveDesktop(fallbackDesktop);
  GUID current{};
  bool removed = false;
  if (desktop && fallback && IsDesktopEmptyExcept(desktopId) &&
      GetCurrentDesktopId(&current) && !IsEqualGUID(current, desktopId) &&
      g_vd.managerInternal == manager) {
    HRESULT hr = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop*, IVirtualDesktop*)>(
        manager, g_vd.abi.removeDesktopIndex)(manager, desktop, fallback);
    removed = SUCCEEDED(hr);
    Diagnostics::RecordEvent(L"overflow empty desktop cleanup target=%08X hr=%08X", desktopId.Data1, hr);
    if (IsDeadVirtualDesktopProxy(hr)) RuntimeLifecycle::RequestMaintenance(true);
  }
  SAFE_RELEASE(desktop);
  SAFE_RELEASE(fallback);
  return removed;
}

bool MoveWindowToDesktop(HWND hwnd, DWORD processId, const GUID& sourceDesktop, const GUID& desktopId,
                         WindowMovePurpose purpose) {
  auto requestPending = [&] {
    if (purpose == WindowMovePurpose::Overflow) return g_wm.overflowWindows.count(hwnd) != 0;
    const auto pending = g_wm.pendingWindowRoutes.find(hwnd);
    return pending != g_wm.pendingWindowRoutes.end() && pending->second.processId == processId &&
        IsEqualGUID(pending->second.sourceDesktop, sourceDesktop) &&
        IsEqualGUID(pending->second.targetDesktop, desktopId) && !IsMoveSizeGestureInProgress(hwnd);
  };
  if (!requestPending() || !g_vd.serviceProvider) return false;
  IVirtualDesktop* desktop = FindLiveDesktop(desktopId);
  if (!desktop) return false;
  IUnknown* const manager = g_vd.managerInternal;
  if (!manager || !g_vd.serviceProvider || !requestPending()) {
    desktop->Release();
    return false;
  }
  const IID viewCollectionIid =
      {0x1841C6D7, 0x4F9D, 0x42C0, {0xAF, 0x41, 0x87, 0x47, 0x53, 0x8F, 0x10, 0xE5}};
  IUnknown* collection = nullptr;
  IUnknown* view = nullptr;
  HRESULT hr = g_vd.serviceProvider->QueryService(
      viewCollectionIid, viewCollectionIid, reinterpret_cast<void**>(&collection));
  if (SUCCEEDED(hr) && collection) {
    const auto getView = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HWND, IUnknown**)>(collection, 6);
    hr = getView(collection, hwnd, &view);
  } else if (SUCCEEDED(hr)) {
    hr = E_NOINTERFACE;
  }
  IUnknown* pinnedApps = nullptr;
  const CLSID pinnedAppsService =
      {0xB5A399E7, 0x1C87, 0x46B8, {0x88, 0xE9, 0xFC, 0x57, 0x47, 0xB1, 0x71, 0xBD}};
  const IID pinnedAppsIid =
      {0x4CE81583, 0x1E4C, 0x4632, {0xA6, 0x21, 0x07, 0xA5, 0x35, 0x43, 0x14, 0x8F}};
  BOOL pinned = TRUE;
  if (SUCCEEDED(hr) && view) {
    hr = g_vd.managerInternal == manager && g_vd.serviceProvider
        ? g_vd.serviceProvider->QueryService(pinnedAppsService, pinnedAppsIid, reinterpret_cast<void**>(&pinnedApps))
        : E_ABORT;
    if (SUCCEEDED(hr) && pinnedApps) {
      const auto isPinned = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IUnknown*, BOOL*)>(pinnedApps, 6);
      hr = isPinned(pinnedApps, view, &pinned);
    }
    if (SUCCEEDED(hr) && pinned) hr = E_ACCESSDENIED;
  }
  SAFE_RELEASE(pinnedApps);
  BOOL canMove = FALSE;
  if (SUCCEEDED(hr) && view && g_vd.managerInternal == manager) {
    const auto canMoveView = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IUnknown*, BOOL*)>(g_vd.managerInternal, 5);
    hr = canMoveView(g_vd.managerInternal, view, &canMove);
  }
  DWORD livePid = 0;
  GUID owner{}, current{};
  const bool sourceObserved = SUCCEEDED(hr) && view && canMove &&
      GetWindowDesktopIdSafe(hwnd, &owner) && IsEqualGUID(owner, sourceDesktop);
  const bool sourceActive = purpose == WindowMovePurpose::Overflow ||
      (GetCurrentDesktopId(&current) && IsEqualGUID(current, sourceDesktop) &&
       !IsWindowTrackedInAnyState(hwnd) && !IsWindowCloaked(hwnd) &&
       GetPhysicalSuspensionReason(hwnd) == SuspensionReason::None);
  GetWindowThreadProcessId(hwnd, &livePid);
  if (sourceObserved && sourceActive && IsWindow(hwnd) && livePid == processId &&
      requestPending() && manager && g_vd.managerInternal == manager) {
    const auto moveView = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IUnknown*, IVirtualDesktop*)>(g_vd.managerInternal, 4);
    hr = moveView(g_vd.managerInternal, view, desktop);
  } else if (SUCCEEDED(hr)) {
    hr = E_ACCESSDENIED;
  }
  SAFE_RELEASE(view);
  SAFE_RELEASE(collection);
  desktop->Release();
  GUID observed{};
  // A mutation can complete despite a lost COM response. Never blindly repeat it.
  const bool ownerObserved = GetWindowDesktopIdSafe(hwnd, &observed);
  GetWindowThreadProcessId(hwnd, &livePid);
  const bool moved = IsWindow(hwnd) && livePid == processId && ownerObserved &&
                     IsEqualGUID(observed, desktopId) && requestPending();
  Diagnostics::RecordEvent(L"window desktop move hwnd=%p target=%08X hr=%08X observedSuccess=%d",
                           hwnd, desktopId.Data1, hr, moved ? 1 : 0);
  if (IsDeadVirtualDesktopProxy(hr)) RuntimeLifecycle::RequestMaintenance(true);
  return moved;
}

bool SwitchToDesktop(HWND hwnd, DWORD processId, const GUID& desktopId, const GUID& sourceDesktop, DWORD inputTickMs,
                     WindowMovePurpose purpose) {
  IVirtualDesktop* desktop = FindLiveDesktop(desktopId);
  if (!desktop) return false;
  IUnknown* const manager = g_vd.managerInternal;
  auto canFollow = [&] {
    LASTINPUTINFO input{sizeof(input)};
    GUID current{}, owner{};
    if (!GetWindowDesktopIdSafe(hwnd, &owner) || !IsEqualGUID(owner, desktopId) ||
        !GetCurrentDesktopId(&current) || !IsEqualGUID(current, sourceDesktop) ||
        !manager || g_vd.managerInternal != manager) return false;
    // Owner queries can pump destroy/user-gesture events. Check intent last.
    DWORD livePid = 0;
    GetWindowThreadProcessId(hwnd, &livePid);
    const auto& initial = g_wm.pendingInitialPlacementSwitch;
    const bool requestPending = purpose == WindowMovePurpose::Overflow
        ? g_wm.overflowWindows.count(hwnd) != 0
        : initial.hwnd == hwnd && initial.processId == processId &&
          initial.inputTickMs == inputTickMs && IsEqualGUID(initial.sourceDesktop, sourceDesktop) &&
          IsEqualGUID(initial.targetDesktop, desktopId) && !IsMoveSizeGestureInProgress(hwnd);
    return IsWindow(hwnd) && livePid == processId && requestPending &&
           GetLastInputInfo(&input) && input.dwTime == inputTickMs;
  };
  if (!canFollow()) {
    desktop->Release();
    return false;
  }
  HRESULT hr = E_NOINTERFACE;
  bool animated = false;
  if (g_vd.abi.animatedSwitchIndex >= 0) {
    // The reference waits before a new animation to avoid crashing Explorer
    // when another switch is still animating. Wait for completion before
    // ordinary workspace reconciliation resumes.
    const auto waitForAnimation = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*)>(
        g_vd.managerInternal, g_vd.abi.animationWaitIndex);
    hr = waitForAnimation(g_vd.managerInternal);
    // An existing animation can outlast the user's decision to stay elsewhere.
    if (!canFollow()) {
      desktop->Release();
      return false;
    }
    if (SUCCEEDED(hr)) {
      const auto switchAnimated = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop*)>(
          g_vd.managerInternal, g_vd.abi.animatedSwitchIndex);
      hr = switchAnimated(g_vd.managerInternal, desktop);
      animated = SUCCEEDED(hr);
      if (animated && g_vd.managerInternal == manager) {
        const HRESULT waitHr = waitForAnimation(g_vd.managerInternal);
        if (FAILED(waitHr)) Wh_Log(L"Desktop animation completion wait failed: 0x%08X", waitHr);
      }
    }
  }
  if (g_vd.managerInternal == manager &&
      (g_vd.abi.animatedSwitchIndex < 0 || hr == E_NOTIMPL || hr == E_NOINTERFACE) && canFollow()) {
    if (g_vd.abi.usesHMonitor) {
      const auto switchDesktop = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, IVirtualDesktop*)>(
          g_vd.managerInternal, g_vd.abi.switchDesktopIndex);
      hr = switchDesktop(g_vd.managerInternal, nullptr, desktop);
    } else {
      const auto switchDesktop = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, IVirtualDesktop*)>(
          g_vd.managerInternal, g_vd.abi.switchDesktopIndex);
      hr = switchDesktop(g_vd.managerInternal, desktop);
    }
  }
  desktop->Release();
  GUID current{};
  const bool accepted = SUCCEEDED(hr) || (GetCurrentDesktopId(&current) && IsEqualGUID(current, desktopId));
  Diagnostics::RecordEvent(L"window desktop switch target=%08X hr=%08X accepted=%d animated=%d", desktopId.Data1, hr, accepted ? 1 : 0, animated ? 1 : 0);
  if (IsDeadVirtualDesktopProxy(hr)) RuntimeLifecycle::RequestMaintenance(true);
  return accepted;
}

bool GetCurrentDesktopId(GUID* outGuid) {
  if (!outGuid) return false;
  if (!g_vd.managerInternal) {
    g_vd.lastKnownDesktopValid = false;
    RuntimeLifecycle::RequestMaintenance(false);
    return false;
  }

  // GetCurrentDesktop itself is already retried once by CallManagerInternal. A
  // stale IVirtualDesktop can still fail at GetId after Explorer replacement, so
  // allow one full core reacquisition for that distinct failure boundary too.
  for (int attempt = 0; attempt < 2; ++attempt) {
    IVirtualDesktop* desktop = nullptr;
    HRESULT hr = CallManagerInternal(VTABLE_GET_CURRENT_DESKTOP, &desktop);
    if (FAILED(hr) || !desktop) {
      g_vd.lastKnownDesktopValid = false;
      RuntimeLifecycle::RequestMaintenance(false);
      Wh_Log(L"GetCurrentDesktop failed: 0x%08X", hr);
      return false;
    }

    hr = desktop->GetId(outGuid);
    desktop->Release();
    if (SUCCEEDED(hr)) {
      g_vd.lastKnownDesktop = *outGuid;
      g_vd.lastKnownDesktopValid = true;
      return true;
    }

    if (attempt == 0 && ReinitializeVirtualDesktopAPI()) continue;
    Wh_Log(L"Current virtual desktop GetId failed after recovery: 0x%08X", hr);
  }

  g_vd.lastKnownDesktopValid = false;
  RuntimeLifecycle::RequestMaintenance(false);
  return false;
}

bool GetWindowDesktopIdSafe(HWND hwnd, GUID* outGuid) {
  if (!hwnd || !outGuid) return false;
  if (!g_vd.initialized || !g_vd.desktopManager) {
    RuntimeLifecycle::RequestMaintenance(false);
    return false;
  }

  HRESULT hr = g_vd.desktopManager->GetWindowDesktopId(hwnd, outGuid);
  if (FAILED(hr) && IsDeadVirtualDesktopProxy(hr) &&
      ReinitializeVirtualDesktopAPI() && g_vd.desktopManager) {
    hr = g_vd.desktopManager->GetWindowDesktopId(hwnd, outGuid);
  }
  if (FAILED(hr) && IsDeadVirtualDesktopProxy(hr)) {
    RuntimeLifecycle::RequestMaintenance(false);
  }
  return SUCCEEDED(hr);
}

bool IsWindowOnCurrentDesktopSafe(HWND hwnd, BOOL* onCurrent) {
  if (!hwnd || !onCurrent) return false;
  if (!g_vd.initialized || !g_vd.desktopManager) {
    RuntimeLifecycle::RequestMaintenance(false);
    return false;
  }

  HRESULT hr = g_vd.desktopManager->IsWindowOnCurrentVirtualDesktop(hwnd, onCurrent);
  if (FAILED(hr) && IsDeadVirtualDesktopProxy(hr) &&
      ReinitializeVirtualDesktopAPI() && g_vd.desktopManager) {
    hr = g_vd.desktopManager->IsWindowOnCurrentVirtualDesktop(hwnd, onCurrent);
  }
  if (FAILED(hr) && IsDeadVirtualDesktopProxy(hr)) {
    RuntimeLifecycle::RequestMaintenance(false);
  }
  return SUCCEEDED(hr);
}



}  // namespace Platform::VirtualDesktop

//=============================================================================
// Runtime lifecycle maintenance
//=============================================================================
//
// Keep recovery centralized. Callers may only request maintenance; this WM-STA
// owner is the only place that retries WinEvent hooks, the private VD core, and
// the native VD notification subscription. This mirrors the simple Windhawk
// tool-mod worker/message-loop pattern while avoiding one-shot startup failures.
namespace RuntimeLifecycle {

static constexpr UINT kMaintenanceRetryMs = 1000;
static constexpr UINT kMaintenanceMaxAttempts = 30;

static void CancelMaintenanceTimer() {
  if (!g_wm.maintenanceTimer) return;
  KillTimer(nullptr, g_wm.maintenanceTimer);
  g_wm.maintenanceTimer = 0;
}

static void ArmMaintenanceTimer() {
  if (g_wm.maintenanceTimer ||
      g_wm.maintenanceAttempts >= kMaintenanceMaxAttempts) {
    return;
  }

  g_wm.maintenanceTimer =
      SetTimer(nullptr, 0, kMaintenanceRetryMs, nullptr);
  if (!g_wm.maintenanceTimer) {
    Wh_Log(L"Failed to schedule runtime maintenance retry");
  }
}

void RequestMaintenance(bool resetAttempts) {
  AssertWmThread(L"RuntimeLifecycle::RequestMaintenance");

  // Retry in bounded bursts rather than polling forever. After one burst is
  // exhausted, a later *external* demand (user action/lifecycle observation)
  // starts a fresh burst automatically; calls made from inside the active
  // maintenance attempt never reset their own budget.
  const bool exhaustedAndDemandedAgain =
      !g_wm.maintenanceRunning && !g_wm.maintenanceTimer &&
      g_wm.maintenanceAttempts >= kMaintenanceMaxAttempts;
  if (resetAttempts || exhaustedAndDemandedAgain) {
    g_wm.maintenanceAttempts = 0;
  }
  g_wm.lifecycleRetryAfterPlatformRecovery = true;
  if (!g_wm.maintenanceRunning) ArmMaintenanceTimer();
}

void RunMaintenanceNow() {
  AssertWmThread(L"RuntimeLifecycle::RunMaintenanceNow");
  CancelMaintenanceTimer();
  g_wm.maintenanceRunning = true;
  ++Diagnostics::g_runtime.counters.runtimeMaintenanceRuns;

  const bool hooksReady = Platform::WindowEvents::InstallWinEventHooks();
  const bool vdCoreReady =
      g_wm.comInitialized && InitializeVirtualDesktopAPI();
  const bool notificationsReady =
      vdCoreReady && Platform::VirtualDesktop::RegisterVirtualDesktopNotifications();

  if (hooksReady && vdCoreReady && notificationsReady) {
    const bool recoveredDeferredWork =
        g_wm.lifecycleRetryAfterPlatformRecovery;
    g_wm.maintenanceAttempts = 0;
    g_wm.lifecycleRetryAfterPlatformRecovery = false;

    if (recoveredDeferredWork) {
      Diagnostics::RecordEvent(L"runtime platform maintenance recovered");
      // Startup/settings-reload may have skipped the initial current-workspace
      // arrangement while Explorer was unavailable. The recovery pass must do
      // more than rediscover membership: explicitly reinforce known current workspaces.
      g_wm.forceMonitorReconcile = true;
      Reconcile::ScheduleLifecycleReconcile(nullptr);
    }
    g_wm.maintenanceRunning = false;
    return;
  }

  g_wm.lifecycleRetryAfterPlatformRecovery = true;
  ++g_wm.maintenanceAttempts;
  if (g_wm.maintenanceAttempts < kMaintenanceMaxAttempts) {
    ++Diagnostics::g_runtime.counters.runtimeMaintenanceRetries;
    ArmMaintenanceTimer();
  } else {
    CancelMaintenanceTimer();
    ++Diagnostics::g_runtime.counters.runtimeMaintenanceExhausted;
    Diagnostics::RecordEvent(
        L"runtime maintenance retry burst exhausted attempts=%u hooks=%d vdCore=%d notifications=%d",
        g_wm.maintenanceAttempts, hooksReady ? 1 : 0, vdCoreReady ? 1 : 0,
        notificationsReady ? 1 : 0);
    Wh_Log(
        L"Runtime maintenance retry burst exhausted (hooks=%d vdCore=%d notifications=%d)",
        hooksReady ? 1 : 0, vdCoreReady ? 1 : 0,
        notificationsReady ? 1 : 0);
  }
  g_wm.maintenanceRunning = false;
}

void NotifyShellRestarted() {
  AssertWmThread(L"RuntimeLifecycle::NotifyShellRestarted");
  ++Diagnostics::g_runtime.counters.shellRestartSignals;

  // TaskbarCreated is a shell-lifecycle signal, but Explorer can occasionally
  // recreate taskbar UI inside the same process. Only abandon private COM proxies
  // when the shell process identity actually changed (or disappeared). This avoids
  // duplicating a still-live notification subscription in the same Explorer.
  HWND shell = GetShellWindow();
  DWORD currentShellPid = 0;
  if (shell && IsWindow(shell)) GetWindowThreadProcessId(shell, &currentShellPid);
  const bool shellReplaced =
      !currentShellPid || !g_vd.shellPid || currentShellPid != g_vd.shellPid;

  Diagnostics::RecordEvent(
      L"Explorer shell lifecycle signal oldPid=%lu currentPid=%lu replacement=%d",
      g_vd.shellPid, currentShellPid, shellReplaced ? 1 : 0);

  if (shellReplaced) {
    Platform::VirtualDesktop::AbandonVirtualDesktopAPIForShellRestart();
    g_wm.reconciledDesktop = {};
    g_wm.reconciledDesktopState = ReconciledDesktopState::Unknown;
  }

  // A shell replacement can move/recreate windows asynchronously. Never let a
  // pre-restart conformance rectangle fight the replacement shell.
  ClearAllConformanceLeases();
  ScheduleNextConformanceTimer();
  g_wm.forceMonitorReconcile = true;
  const bool platformNeedsRecovery =
      !Platform::WindowEvents::AllWinEventHooksInstalled() ||
      !g_vd.initialized ||
      (g_vd.abi.notification.methodCount > 0 && !g_vd.notificationsRegistered);
  if (shellReplaced || platformNeedsRecovery) RequestMaintenance(true);
  Reconcile::ScheduleLifecycleReconcile(nullptr);
}

void NotifyDisplayTopologyChanged() {
  AssertWmThread(L"RuntimeLifecycle::NotifyDisplayTopologyChanged");
  ++Diagnostics::g_runtime.counters.displayTopologySignals;
  Diagnostics::RecordEvent(L"display/work-area topology changed");
  ClearMonitorIdentityCache();

  // Every existing lease was computed against the old work-area/monitor topology.
  // Cancel it before Windows' own topology-induced LOCATIONCHANGE burst arrives.
  ClearAllConformanceLeases();
  ScheduleNextConformanceTimer();
  g_wm.pendingDesktopArranges.erase(
      std::remove_if(
          g_wm.pendingDesktopArranges.begin(), g_wm.pendingDesktopArranges.end(),
          [](const PendingWorkspaceArrange& pending) {
            return !IsLiveMonitorHandle(pending.monitor);
          }),
      g_wm.pendingDesktopArranges.end());
  g_wm.forceMonitorReconcile = true;

  // Display/work-area changes normally require only model/geometry reconciliation.
  // Do not start a COM retry burst unless platform resources are already unhealthy.
  const bool platformNeedsRecovery =
      !Platform::WindowEvents::AllWinEventHooksInstalled() ||
      !g_vd.initialized ||
      (g_vd.abi.notification.methodCount > 0 && !g_vd.notificationsRegistered);
  if (platformNeedsRecovery) RequestMaintenance(true);
  Reconcile::ScheduleLifecycleReconcile(nullptr);
}

}  // namespace RuntimeLifecycle

//=============================================================================
// Notification-area layout indicator + layout flyout
//=============================================================================
//
// UI boundary for the tool mod. This subsystem is Win32/GDI-only: no Explorer
// hooks, XAML, WinUI, or Windows App SDK dependency. The tray uses two-letter
// mode abbreviations that are fitted to the full icon canvas. The flyout is a
// small non-activating card shown only after an explicit layout change.

namespace TrayUi {

//-----------------------------------------------------------------------------
// Settings + naming
//-----------------------------------------------------------------------------

constexpr wchar_t kTrayWindowClassName[] = L"WindhawkMultiWMTrayWindow";
constexpr wchar_t kFlyoutWindowClassName[] = L"WindhawkMultiWMStatusFlyout";
constexpr UINT kTrayCallbackMessage = WM_APP + 20;
constexpr UINT kIconId = 1;
constexpr UINT kContextTileWorkspace = 41001;
constexpr UINT kContextToggleMode = 41002;
constexpr UINT kContextLayoutBase = 41100;
constexpr int kIconCanvasSize = 32;
constexpr int kTrayFontHeight = 26;
constexpr UINT_PTR kFlyoutTimerId = 1;
constexpr DWORD kFlyoutHoldMs = 850;
constexpr DWORD kFlyoutFadeMs = 180;

static HWND g_trayWindow = nullptr;
static ATOM g_trayWindowClass = 0;
static ATOM g_flyoutWindowClass = 0;
static UINT g_taskbarCreatedMessage = 0;
static bool g_iconAdded = false;
static bool g_systemLightTheme = false;
static bool g_flyoutTop = false;
static bool g_showToasts = true;
static bool g_trayClickOpensMenu = false;
static int g_flyoutOffsetX = 0;
static int g_flyoutOffsetY = 0;
static TileLayout g_displayedLayout = TileLayout::COUNT;
static GUID g_displayedDesktopId{};
static bool g_haveDisplayedDesktopId = false;
static HMONITOR g_displayedMonitor = nullptr;

struct FlyoutInstance {
  HWND hwnd = nullptr;
  HMONITOR monitor = nullptr;
  std::wstring text;
  DWORD shownTick = 0;
  DWORD holdMs = kFlyoutHoldMs;
};

static std::vector<FlyoutInstance*> g_flyouts;
static NOTIFYICONDATAW g_notifyData{};
static std::array<HICON, static_cast<size_t>(TileLayout::COUNT)> g_icons{};
static std::array<std::wstring, static_cast<size_t>(TileLayout::COUNT)>
    g_customIconPaths{};

static const wchar_t* LayoutDisplayName(TileLayout layout) {
  switch (layout) {
    case TileLayout::MasterStack: return L"Master + Stack (Vertical)";
    case TileLayout::MasterStackH: return L"Master + Stack (Horizontal)";
    case TileLayout::Columns: return L"Columns";
    case TileLayout::Rows: return L"Rows";
    case TileLayout::Dwindle: return L"Dwindle";
    case TileLayout::Monocle: return L"Monocle";
    case TileLayout::Floating: return L"Floating";
    default: return L"Unknown";
  }
}

static const wchar_t* LayoutTrayText(TileLayout layout) {
  switch (layout) {
    case TileLayout::MasterStack: return L"MV";
    case TileLayout::MasterStackH: return L"MH";
    case TileLayout::Columns: return L"CL";
    case TileLayout::Rows: return L"RW";
    case TileLayout::Dwindle: return L"DW";
    case TileLayout::Monocle: return L"MN";
    case TileLayout::Floating: return L"FL";
    default: return L"??";
  }
}

static const wchar_t* LayoutFlyoutText(TileLayout layout) {
  switch (layout) {
    case TileLayout::MasterStack: return L"Master + Stack V";
    case TileLayout::MasterStackH: return L"Master + Stack H";
    case TileLayout::Columns: return L"Columns";
    case TileLayout::Rows: return L"Rows";
    case TileLayout::Dwindle: return L"Dwindle";
    case TileLayout::Monocle: return L"Monocle";
    case TileLayout::Floating: return L"Floating";
    default: return L"Unknown";
  }
}

static void LoadSettings() {
  using WindhawkUtils::StringSetting;
  auto trayClickAction = StringSetting::make(L"general.TrayClickAction");
  g_trayClickOpensMenu = _wcsicmp(trayClickAction.get(), L"context_menu") == 0;
  g_showToasts = Wh_GetIntSetting(L"appearance.ShowToasts") != 0;
  auto position = StringSetting::make(L"appearance.FlyoutPosition");
  g_flyoutTop = _wcsicmp(position.get(), L"top") == 0;
  g_flyoutOffsetX = Wh_GetIntSetting(L"appearance.FlyoutOffsetX");
  g_flyoutOffsetY = Wh_GetIntSetting(L"appearance.FlyoutOffsetY");

  for (auto& path : g_customIconPaths) path.clear();
  for (int i = 0; i < 32; ++i) {
    auto layoutName =
        StringSetting::make(L"appearance.CustomIcons[%d].Layout", i);
    if (!*layoutName.get()) break;
    auto path = StringSetting::make(L"appearance.CustomIcons[%d].Path", i);
    size_t index = static_cast<size_t>(ParseLayoutSetting(layoutName.get()));
    if (*path.get() && index < g_customIconPaths.size() &&
        g_customIconPaths[index].empty()) {
      g_customIconPaths[index] = path.get();
    }
  }
}

//-----------------------------------------------------------------------------
// Theme-aware tray icons
//-----------------------------------------------------------------------------

static bool ReadSystemLightTheme() {
  DWORD value = 0;
  DWORD size = sizeof(value);
  if (RegGetValueW(
          HKEY_CURRENT_USER,
          L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
          L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
          &size) == ERROR_SUCCESS) {
    return value != 0;
  }
  COLORREF fallback = GetSysColor(COLOR_WINDOW);
  return GetRValue(fallback) + GetGValue(fallback) + GetBValue(fallback) >= 384;
}

static COLORREF ForegroundColor() {
  return g_systemLightTheme ? RGB(24, 24, 24) : RGB(255, 255, 255);
}

static COLORREF FlyoutBackgroundColor() {
  return g_systemLightTheme ? RGB(243, 243, 243) : RGB(36, 36, 36);
}

static HICON CreateTextIcon(TileLayout layout) {
  BITMAPINFO bmi{};
  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = kIconCanvasSize;
  bmi.bmiHeader.biHeight = -kIconCanvasSize;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  void* bits = nullptr;
  HBITMAP color = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits,
                                   nullptr, 0);
  HDC dc = color ? CreateCompatibleDC(nullptr) : nullptr;
  if (!color || !dc || !bits) {
    if (dc) DeleteDC(dc);
    if (color) DeleteObject(color);
    return nullptr;
  }

  HGDIOBJ oldBitmap = SelectObject(dc, color);
  RECT rect{0, 0, kIconCanvasSize, kIconCanvasSize};
  FillRect(dc, &rect, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
  HFONT font = CreateFontW(-kTrayFontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE,
                           FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                           FIXED_PITCH | FF_MODERN, L"Cascadia Mono");
  HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
  SetBkMode(dc, TRANSPARENT);
  SetTextColor(dc, RGB(255, 255, 255));
  DrawTextW(dc, LayoutTrayText(layout), -1, &rect,
            DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
  if (oldFont) SelectObject(dc, oldFont);
  if (font) DeleteObject(font);
  GdiFlush();

  auto* pixels = static_cast<uint32_t*>(bits);
  COLORREF fg = ForegroundColor();
  uint32_t r = GetRValue(fg), g = GetGValue(fg), b = GetBValue(fg);
  for (int i = 0; i < kIconCanvasSize * kIconCanvasSize; ++i) {
    uint32_t p = pixels[i];
    uint32_t a = std::max({p & 0xFFu, (p >> 8) & 0xFFu,
                           (p >> 16) & 0xFFu});
    pixels[i] = (a << 24) | (r << 16) | (g << 8) | b;
  }

  SelectObject(dc, oldBitmap);
  DeleteDC(dc);

  constexpr int maskStride = ((kIconCanvasSize + 31) / 32) * 4;
  std::array<BYTE, maskStride * kIconCanvasSize> maskBits{};
  HBITMAP mask = CreateBitmap(kIconCanvasSize, kIconCanvasSize, 1, 1,
                              maskBits.data());
  ICONINFO info{TRUE, 0, 0, mask, color};
  HICON icon = mask ? CreateIconIndirect(&info) : nullptr;
  if (mask) DeleteObject(mask);
  DeleteObject(color);
  return icon;
}

// Returns an owned icon, preferring the configured file and falling back to
// generated text. The icon must eventually be released with DestroyIcon.
static HICON CreateLayoutIcon(TileLayout layout) {
  size_t index = static_cast<size_t>(layout);
  if (index < g_customIconPaths.size() && !g_customIconPaths[index].empty()) {
    HICON icon = static_cast<HICON>(LoadImageW(
        nullptr, g_customIconPaths[index].c_str(), IMAGE_ICON,
        kIconCanvasSize, kIconCanvasSize, LR_LOADFROMFILE));
    if (icon) return icon;
    Wh_Log(L"Failed to load custom tray icon, using text fallback: %s",
           g_customIconPaths[index].c_str());
  }
  return CreateTextIcon(layout);
}

static void DestroyIconSet(
    std::array<HICON, static_cast<size_t>(TileLayout::COUNT)>& icons) {
  for (HICON& icon : icons) {
    if (icon) DestroyIcon(icon);
    icon = nullptr;
  }
}

static HICON GetLayoutIcon(TileLayout layout) {
  size_t index = static_cast<size_t>(layout);
  if (index < g_icons.size() && g_icons[index]) return g_icons[index];
  return LoadIconW(nullptr, IDI_APPLICATION);
}

static std::wstring MonitorTooltipLabel(HMONITOR monitor, size_t fallbackIndex) {
  MONITORINFOEXW info{};
  info.cbSize = sizeof(info);
  if (monitor && GetMonitorInfoW(monitor, reinterpret_cast<MONITORINFO*>(&info))) {
    std::wstring device = info.szDevice;
    constexpr wchar_t kDevicePrefix[] = L"\\\\.\\";
    if (device.rfind(kDevicePrefix, 0) == 0) device.erase(0, 4);
    if (!device.empty()) return device;
  }

  wchar_t fallback[32]{};
  swprintf(fallback, ARRAYSIZE(fallback), L"Monitor %zu", fallbackIndex + 1);
  return fallback;
}

static void SetTooltip(TileLayout layout) {
  std::vector<HMONITOR> monitors;
  Reconcile::EnumerateCurrentMonitors(monitors);

  // Preserve the compact historical tooltip exactly on a single-display system.
  // The richer summary is useful only when there are multiple local workspaces to
  // distinguish and would otherwise be pure visual clutter.
  std::wstring tooltip;
  if (monitors.size() <= 1) {
    tooltip = L"MultiWM - ";
    tooltip += LayoutDisplayName(layout);
  } else {
    GUID desktopId{};
    bool haveDesktop = false;
    if (g_wm.reconciledDesktopState == ReconciledDesktopState::Settled) {
      desktopId = g_wm.reconciledDesktop;
      haveDesktop = true;
    } else {
      haveDesktop = GetCurrentDesktopId(&desktopId);
    }
    if (!haveDesktop && g_haveDisplayedDesktopId) {
      desktopId = g_displayedDesktopId;
      haveDesktop = true;
    }

    if (!haveDesktop) {
      tooltip = L"MultiWM - ";
      tooltip += LayoutDisplayName(layout);
    } else {
      tooltip = L"MultiWM";
      for (size_t i = 0; i < monitors.size(); ++i) {
        const HMONITOR monitor = monitors[i];
        TileLayout monitorLayout = g_settings.defaultLayout;
        Workspace state;
        DesktopMonitorKey key{};
        if (DesktopMonitorKey::FromHMonitor(desktopId, monitor, &key)) {
          monitorLayout = g_workspaces.Load(key, &state)
              ? state.Layout() : Reconcile::ResolveInitialWorkspaceLayout(key);
        }

        tooltip += L"\n";
        // Reserve the same visual marker column for every row. A figure space
        // is much closer to the triangle glyph width than two proportional spaces.
        tooltip += monitor == g_displayedMonitor ? L"\x25B8 " : L"\u2007 ";
        tooltip += MonitorTooltipLabel(monitor, i);
        tooltip += L": ";
        tooltip += LayoutFlyoutText(monitorLayout);
      }
    }
  }

  wcsncpy(g_notifyData.szTip, tooltip.c_str(), ARRAYSIZE(g_notifyData.szTip) - 1);
  g_notifyData.szTip[ARRAYSIZE(g_notifyData.szTip) - 1] = L'\0';
}

static bool AddIconToShell() {
  if (!g_trayWindow) return false;
  if (g_displayedLayout == TileLayout::COUNT) g_displayedLayout = g_settings.defaultLayout;

  g_notifyData = {};
  g_notifyData.cbSize = sizeof(g_notifyData);
  g_notifyData.hWnd = g_trayWindow;
  g_notifyData.uID = kIconId;
  g_notifyData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
  g_notifyData.uCallbackMessage = kTrayCallbackMessage;
  g_notifyData.hIcon = GetLayoutIcon(g_displayedLayout);
  SetTooltip(g_displayedLayout);
  if (!Shell_NotifyIconW(NIM_ADD, &g_notifyData)) {
    g_iconAdded = false;
    Wh_Log(L"Tray icon NIM_ADD failed");
    return false;
  }

  g_iconAdded = true;
  g_notifyData.uVersion = NOTIFYICON_VERSION_4;
  if (!Shell_NotifyIconW(NIM_SETVERSION, &g_notifyData)) {
    Wh_Log(L"Tray icon NIM_SETVERSION failed after NIM_ADD");
  }
  return true;
}

static void UpdateIcon(TileLayout layout, HMONITOR monitor) {
  if (!g_trayWindow || layout == TileLayout::COUNT) return;

  const TileLayout previousLayout = g_displayedLayout;
  const HMONITOR previousMonitor = g_displayedMonitor;
  const HICON previousIcon = g_notifyData.hIcon;
  const std::wstring previousTooltip = g_notifyData.szTip;

  g_displayedLayout = layout;
  if (monitor) g_displayedMonitor = monitor;

  if (!g_iconAdded) {
    if (AddIconToShell()) return;

    // TaskbarCreated can be broadcast even when Explorer kept the existing tray
    // entry. In that state NIM_ADD fails because the icon already exists, so try
    // updating that surviving entry instead of permanently wedging on NIM_ADD.
    g_notifyData.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    g_notifyData.hIcon = GetLayoutIcon(layout);
    SetTooltip(layout);
    if (Shell_NotifyIconW(NIM_MODIFY, &g_notifyData)) {
      g_iconAdded = true;
      g_notifyData.uVersion = NOTIFYICON_VERSION_4;
      if (!Shell_NotifyIconW(NIM_SETVERSION, &g_notifyData)) {
        Wh_Log(L"Tray icon NIM_SETVERSION failed after NIM_MODIFY recovery");
      }
      Wh_Log(L"Tray icon recovered by NIM_MODIFY after NIM_ADD failed");
      return;
    }

    Wh_Log(L"Tray icon recovery failed: both NIM_ADD and NIM_MODIFY failed");
    return;
  }

  g_notifyData.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP;
  g_notifyData.hIcon = GetLayoutIcon(layout);
  SetTooltip(layout);
  if (previousLayout == g_displayedLayout &&
      previousMonitor == g_displayedMonitor &&
      previousIcon == g_notifyData.hIcon &&
      previousTooltip == g_notifyData.szTip) {
    return;
  }
  if (!Shell_NotifyIconW(NIM_MODIFY, &g_notifyData)) {
    // The shell may have discarded the entry while our local state still says it
    // exists. Reverse the recovery direction and recreate it with NIM_ADD.
    Wh_Log(L"Tray icon NIM_MODIFY failed; trying NIM_ADD");
    g_iconAdded = false;
    if (!AddIconToShell()) {
      Wh_Log(L"Tray icon recovery failed after NIM_MODIFY");
    }
  }
}

// TaskbarCreated is a shell/taskbar lifecycle hint, not proof that the old tray
// entry vanished. Remove any surviving entry first, then rebuild from current
// model state. UpdateIcon still has ADD/MODIFY recovery for shell-state races.
static void RecreateIconInShell() {
  if (!g_trayWindow) return;

  NOTIFYICONDATAW identity{};
  identity.cbSize = sizeof(identity);
  identity.hWnd = g_trayWindow;
  identity.uID = kIconId;
  Shell_NotifyIconW(NIM_DELETE, &identity);

  g_iconAdded = false;
  if (g_displayedLayout == TileLayout::COUNT) {
    g_displayedLayout = g_settings.defaultLayout;
  }
  UpdateIcon(g_displayedLayout, g_displayedMonitor);
}

// Repoints the live tray entry to a complete new icon set before destroying the
// old handles, so the shell never observes an HICON after it has been released.
static void RebuildIcons() {
  std::array<HICON, static_cast<size_t>(TileLayout::COUNT)> icons{};
  for (size_t i = 0; i < icons.size(); ++i) {
    icons[i] = CreateLayoutIcon(static_cast<TileLayout>(i));
  }
  auto old = g_icons;
  g_icons = icons;
  if (g_displayedLayout != TileLayout::COUNT) UpdateIcon(g_displayedLayout, g_displayedMonitor);
  DestroyIconSet(old);
}

//-----------------------------------------------------------------------------
// Compact layout flyout
//-----------------------------------------------------------------------------

static int ScaleForDpi(int value, UINT dpi) {
  return MulDiv(value, static_cast<int>(dpi ? dpi : 96), 96);
}

static HFONT CreateFlyoutFont(UINT dpi) {
  return CreateFontW(-ScaleForDpi(14, dpi), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                     FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                     CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                     DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

static FlyoutInstance* GetFlyoutInstance(HWND hwnd) {
  return reinterpret_cast<FlyoutInstance*>(
      GetWindowLongPtrW(hwnd, GWLP_USERDATA));
}

static LRESULT CALLBACK FlyoutWindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                         LPARAM lParam) {
  if (message == WM_NCCREATE) {
    auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
    auto* instance = create
        ? reinterpret_cast<FlyoutInstance*>(create->lpCreateParams)
        : nullptr;
    if (instance) {
      instance->hwnd = hwnd;
      SetWindowLongPtrW(
          hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(instance));
    }
  }

  FlyoutInstance* instance = GetFlyoutInstance(hwnd);
  switch (message) {
    case WM_NCHITTEST:
      return HTTRANSPARENT;
    case WM_ERASEBKGND:
      return 1;
    case WM_PAINT: {
      PAINTSTRUCT ps{};
      HDC dc = BeginPaint(hwnd, &ps);
      RECT client{};
      GetClientRect(hwnd, &client);
      HBRUSH background = CreateSolidBrush(FlyoutBackgroundColor());
      FillRect(dc, &client, background);
      DeleteObject(background);

      HFONT font = CreateFlyoutFont(GetDpiForWindow(hwnd));
      HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
      SetBkMode(dc, TRANSPARENT);
      SetTextColor(dc, ForegroundColor());
      const wchar_t* text = instance ? instance->text.c_str() : L"";
      DrawTextW(dc, text, -1, &client,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
      if (oldFont) SelectObject(dc, oldFont);
      if (font) DeleteObject(font);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_TIMER:
      if (wParam == kFlyoutTimerId && instance) {
        DWORD elapsed = GetTickCount() - instance->shownTick;
        if (elapsed >= instance->holdMs + kFlyoutFadeMs) {
          KillTimer(hwnd, kFlyoutTimerId);
          ShowWindow(hwnd, SW_HIDE);
          return 0;
        }
        BYTE alpha = 255;
        if (elapsed > instance->holdMs) {
          alpha = static_cast<BYTE>(255 - std::min<DWORD>(
              255, ((elapsed - instance->holdMs) * 255) / kFlyoutFadeMs));
        }
        SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
        return 0;
      }
      break;
    case WM_NCDESTROY:
      SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
      break;
  }
  return DefWindowProcW(hwnd, message, wParam, lParam);
}

static FlyoutInstance* FindFlyoutForMonitor(HMONITOR monitor) {
  for (FlyoutInstance* instance : g_flyouts) {
    if (instance && instance->monitor == monitor && instance->hwnd) {
      return instance;
    }
  }
  return nullptr;
}

static FlyoutInstance* GetOrCreateFlyoutForMonitor(HMONITOR monitor) {
  if (!monitor) return nullptr;
  if (FlyoutInstance* existing = FindFlyoutForMonitor(monitor)) return existing;

  auto* instance = new (std::nothrow) FlyoutInstance;
  if (!instance) return nullptr;
  instance->monitor = monitor;

  HINSTANCE module = GetModuleHandleW(nullptr);
  instance->hwnd = CreateWindowExW(
      WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
      kFlyoutWindowClassName, L"", WS_POPUP, 0, 0, 0, 0,
      nullptr, nullptr, module, instance);
  if (!instance->hwnd) {
    delete instance;
    return nullptr;
  }

  g_flyouts.push_back(instance);
  return instance;
}

static void ShowStatusFlyout(const wchar_t* text, HMONITOR monitor) {
  if (!g_showToasts || !text || !*text) return;
  if (!monitor) monitor = GetWorkspaceCommandMonitor();
  RECT workArea{};
  if (!monitor || !GetMonitorWorkArea(monitor, &workArea)) return;

  FlyoutInstance* instance = GetOrCreateFlyoutForMonitor(monitor);
  if (!instance || !instance->hwnd) return;
  instance->text = text;

  UINT dpi = GetMonitorEffectiveDpi(monitor);
  HFONT font = CreateFlyoutFont(dpi);
  HDC dc = GetDC(instance->hwnd);
  HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
  SIZE textSize{};
  GetTextExtentPoint32W(dc, instance->text.c_str(),
                        static_cast<int>(instance->text.size()), &textSize);
  if (oldFont) SelectObject(dc, oldFont);
  if (font) DeleteObject(font);
  ReleaseDC(instance->hwnd, dc);

  int width = std::max(ScaleForDpi(128, dpi),
                       static_cast<int>(textSize.cx) + ScaleForDpi(48, dpi));
  int height = ScaleForDpi(48, dpi);
  int x = workArea.left + ((workArea.right - workArea.left) - width) / 2 +
          ScaleForDpi(g_flyoutOffsetX, dpi);
  int edgeGap = ScaleForDpi(16, dpi);
  int y = g_flyoutTop ? workArea.top + edgeGap
                      : workArea.bottom - height - edgeGap;
  y += ScaleForDpi(g_flyoutOffsetY, dpi);

  int radius = ScaleForDpi(8, dpi);
  HRGN region = CreateRoundRectRgn(0, 0, width + 1, height + 1,
                                   radius * 2, radius * 2);
  if (region && !SetWindowRgn(instance->hwnd, region, TRUE)) DeleteObject(region);

  instance->shownTick = GetTickCount();
  // Allow roughly 75 ms per character, retaining the minimum for short hints.
  instance->holdMs = static_cast<DWORD>(std::max<size_t>(
      kFlyoutHoldMs, std::min<size_t>(instance->text.size(), 1000000) * 75));
  SetLayeredWindowAttributes(instance->hwnd, 0, 255, LWA_ALPHA);
  SetWindowPos(instance->hwnd, HWND_TOPMOST, x, y, width, height,
               SWP_NOACTIVATE | SWP_SHOWWINDOW);
  InvalidateRect(instance->hwnd, nullptr, TRUE);
  UpdateWindow(instance->hwnd);
  if (!SetTimer(instance->hwnd, kFlyoutTimerId, 16, nullptr)) {
    // A cosmetic timer failure must not leave a permanently visible overlay.
    ShowWindow(instance->hwnd, SW_HIDE);
  }
}

static void ShowLayoutFlyout(TileLayout layout, HMONITOR monitor) {
  if (layout == TileLayout::COUNT) return;
  ShowStatusFlyout(LayoutFlyoutText(layout), monitor);
}

static void ShowCommandFailureFlyout(
    const wchar_t* text, HMONITOR monitor) {
  if (!text || !*text) return;
  ShowStatusFlyout(text, monitor);
}

static void ShowManagementModeFlyout(bool automatic) {
  ShowStatusFlyout(
      automatic ? L"Automatic Mode" : L"Manual Mode",
      GetWorkspaceCommandMonitor());
}

static void ShowDesktopSwitchFlyouts(const GUID& desktopId) {
  if (!g_showToasts) return;
  std::vector<HMONITOR> monitors;
  Reconcile::EnumerateCurrentMonitors(monitors);
  for (HMONITOR monitor : monitors) {
    TileLayout layout = g_settings.defaultLayout;
    Workspace state;
    DesktopMonitorKey key{};
    if (DesktopMonitorKey::FromHMonitor(desktopId, monitor, &key)) {
      layout = g_workspaces.Load(key, &state)
          ? state.Layout() : Reconcile::ResolveInitialWorkspaceLayout(key);
    }
    ShowLayoutFlyout(layout, monitor);
  }
}

//-----------------------------------------------------------------------------
// Standard tray context menu
//-----------------------------------------------------------------------------

static HMONITOR GetContextMenuMonitor() {
  MONITORINFO info{sizeof(info)};
  if (g_displayedMonitor && GetMonitorInfoW(g_displayedMonitor, &info)) {
    return g_displayedMonitor;
  }

  HMONITOR monitor = GetForegroundMonitor();
  if (!monitor) monitor = GetCursorMonitor();
  return monitor;
}

static TileLayout GetMonitorLayoutOrDefault(HMONITOR monitor) {
  TileLayout layout = g_settings.defaultLayout;
  DesktopMonitorKey key{};
  Workspace state;
  if (monitor && GetCurrentWorkspaceKey(monitor, &key)) {
    layout = g_workspaces.Load(key, &state)
        ? state.Layout() : Reconcile::ResolveInitialWorkspaceLayout(key);
  }
  return layout;
}

// TrackPopupMenuEx runs a modal loop which otherwise retrieves and discards
// hwnd-less actor messages. Process only messages owned by the WM and leave all
// menu/system traffic to the next hook and the menu loop.
static LRESULT CALLBACK TrayMenuMessageFilterProc(
    int code, WPARAM wParam, LPARAM lParam) {
  if (code == MSGF_MENU && lParam) {
    const MSG& msg = *reinterpret_cast<const MSG*>(lParam);
    if (!msg.hwnd &&
        HandleWmThreadMessage(msg) == WmMessageDisposition::Handled) {
      return 1;
    }
  }
  return CallNextHookEx(nullptr, code, wParam, lParam);
}

static void ShowContextMenu() {
  if (!g_trayWindow) return;

  const HMONITOR monitor = GetContextMenuMonitor();
  if (!monitor) return;

  HMENU menu = CreatePopupMenu();
  if (!menu) return;

  AppendMenuW(menu, MF_STRING, kContextTileWorkspace, L"Tile Workspace");
  const ManagementMode targetMode = IsAutomaticMode()
                                        ? ManagementMode::Manual
                                        : ManagementMode::Automatic;
  AppendMenuW(
      menu, MF_STRING, kContextToggleMode,
      targetMode == ManagementMode::Manual ? L"Switch to Manual Mode"
                                           : L"Switch to Automatic Mode");

  AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

  const TileLayout currentLayout = GetMonitorLayoutOrDefault(monitor);
  for (size_t i = 0; i < g_settings.layoutCycle.size(); ++i) {
    const TileLayout layout = g_settings.layoutCycle[i];
    const UINT command = kContextLayoutBase + static_cast<UINT>(i);
    UINT flags = MF_STRING;
    if (layout == currentLayout) {
      // Only the active layout carries a marker. Disable it as selecting the
      // current layout would be a no-op, and let the native menu render it grey.
      flags |= MF_CHECKED | MF_GRAYED;
    }
    AppendMenuW(menu, flags, command, LayoutDisplayName(layout));
  }

  POINT point{};
  if (!GetCursorPos(&point)) {
    DestroyMenu(menu);
    return;
  }

  HHOOK menuMessageHook = SetWindowsHookExW(
      WH_MSGFILTER, TrayMenuMessageFilterProc, nullptr, GetCurrentThreadId());
  if (!menuMessageHook) {
    Wh_Log(L"Failed to install tray menu message filter: %lu", GetLastError());
    DestroyMenu(menu);
    return;
  }

  // Required for reliable dismissal of notification-area popup menus.
  SetForegroundWindow(g_trayWindow);
  const UINT command = TrackPopupMenuEx(
      menu, TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
      point.x, point.y, g_trayWindow, nullptr);
  if (!UnhookWindowsHookEx(menuMessageHook)) {
    Wh_Log(L"Failed to remove tray menu message filter: %lu", GetLastError());
  }
  PostMessageW(g_trayWindow, WM_NULL, 0, 0);
  DestroyMenu(menu);

  const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
  if (!threadId || !command) return;

  if (command == kContextTileWorkspace) {
    PostThreadMessageW(
        threadId, WM_APP_TILE_WORKSPACE,
        reinterpret_cast<WPARAM>(monitor), 0);
    return;
  }

  if (command == kContextToggleMode) {
    PostThreadMessageW(
        threadId, WM_APP_MANAGEMENT_MODE_SET,
        static_cast<WPARAM>(targetMode), 0);
    return;
  }

  if (command >= kContextLayoutBase) {
    const size_t index = static_cast<size_t>(command - kContextLayoutBase);
    if (index < g_settings.layoutCycle.size()) {
      PostThreadMessageW(
          threadId, WM_APP_LAYOUT_SET,
          reinterpret_cast<WPARAM>(monitor),
          static_cast<LPARAM>(g_settings.layoutCycle[index]));
    }
  }
}

//-----------------------------------------------------------------------------
// Lifecycle / shell integration
//-----------------------------------------------------------------------------

// Updates the single shell notification icon for one explicit monitor boundary.
// The icon is intentionally a view onto one workspace, while the tooltip can
// summarize all current monitor workspaces when multiple displays are connected.
static void RefreshForMonitor(HMONITOR monitor) {
  if (!monitor) return;

  TileLayout layout = g_settings.defaultLayout;
  DesktopMonitorKey key{};
  Workspace state;
  const bool haveKey = GetCurrentWorkspaceKey(monitor, &key);
  if (haveKey) {
    layout = g_workspaces.Load(key, &state)
        ? state.Layout() : Reconcile::ResolveInitialWorkspaceLayout(key);
  }

  if (haveKey) {
    g_displayedDesktopId = key.desktopId;
    g_haveDisplayedDesktopId = true;
  }
  UpdateIcon(layout, monitor);
}

static void RefreshDisplayedMonitor() {
  HMONITOR monitor = g_displayedMonitor;
  MONITORINFO info{sizeof(info)};
  if (!monitor || !GetMonitorInfoW(monitor, &info)) {
    monitor = GetForegroundMonitor();
    if (!monitor) monitor = GetCursorMonitor();
  }
  if (monitor) RefreshForMonitor(monitor);
}

// Foreground changes deliberately retarget the one physical tray icon. Explicit
// workspace commands call RefreshForMonitor directly with their cursor-selected
// monitor instead of going through this focus-driven path.
static void RefreshForCurrentWorkspace() {
  HMONITOR monitor = GetForegroundMonitor();
  if (!monitor) monitor = GetCursorMonitor();
  if (monitor) RefreshForMonitor(monitor);
}

static void HandleThemeChanged() {
  bool light = ReadSystemLightTheme();
  if (light == g_systemLightTheme) return;
  g_systemLightTheme = light;
  RebuildIcons();
  for (FlyoutInstance* instance : g_flyouts) {
    if (instance && instance->hwnd && IsWindowVisible(instance->hwnd)) {
      InvalidateRect(instance->hwnd, nullptr, TRUE);
    }
  }
}

static LRESULT CALLBACK TrayWindowProc(HWND hwnd, UINT message, WPARAM wParam,
                                       LPARAM lParam) {
  if (g_taskbarCreatedMessage && message == g_taskbarCreatedMessage) {
    RecreateIconInShell();
    RuntimeLifecycle::NotifyShellRestarted();
    return 0;
  }

  if (message == kTrayCallbackMessage) {
    const UINT event = LOWORD(lParam);
    const UINT iconId = HIWORD(lParam);
    // NOTIFYICON_VERSION_4 reports the semantic context-menu activation as
    // WM_CONTEXTMENU. Do not also react to the legacy WM_RBUTTONUP notification:
    // one physical right-click can otherwise open two menus back-to-back, and the
    // second menu can appear before the first menu's asynchronous WM command is
    // consumed by the WM actor.
    if (iconId == kIconId && event == WM_CONTEXTMENU) {
      ShowContextMenu();
      return 0;
    }
    if (iconId == kIconId &&
        (event == NIN_SELECT || event == NIN_KEYSELECT)) {
      if (g_trayClickOpensMenu) {
        ShowContextMenu();
        return 0;
      }
      // Emit the exact same action as the layout hotkey; layout mutation stays
      // serialized on the WM thread and is implemented in one place only.
      const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
      if (threadId) {
        // The tray lives on the primary taskbar, but its icon represents the
        // active/last-targeted monitor. Preserve that semantic when clicked
        // instead of letting the cursor's primary-monitor location retarget it.
        PostThreadMessageW(
            threadId, WM_APP_LAYOUT_CYCLE,
            reinterpret_cast<WPARAM>(g_displayedMonitor), 0);
      }
    }
    return 0;
  }

  switch (message) {
    case WM_DISPLAYCHANGE:
    case WM_DPICHANGED:
      RuntimeLifecycle::NotifyDisplayTopologyChanged();
      return 0;

    case WM_SETTINGCHANGE:
      HandleThemeChanged();
      if (wParam == SPI_SETWORKAREA) {
        RuntimeLifecycle::NotifyDisplayTopologyChanged();
      }
      return 0;

    case WM_THEMECHANGED:
    case WM_SYSCOLORCHANGE:
      HandleThemeChanged();
      return 0;
  }
  return DefWindowProcW(hwnd, message, wParam, lParam);
}

static bool Initialize() {
  g_systemLightTheme = ReadSystemLightTheme();
  RebuildIcons();

  HINSTANCE instance = GetModuleHandleW(nullptr);
  WNDCLASSW trayClass{};
  trayClass.lpfnWndProc = TrayWindowProc;
  trayClass.hInstance = instance;
  trayClass.lpszClassName = kTrayWindowClassName;
  g_trayWindowClass = RegisterClassW(&trayClass);
  if (!g_trayWindowClass && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

  WNDCLASSW flyoutClass{};
  flyoutClass.lpfnWndProc = FlyoutWindowProc;
  flyoutClass.hInstance = instance;
  flyoutClass.lpszClassName = kFlyoutWindowClassName;
  g_flyoutWindowClass = RegisterClassW(&flyoutClass);
  const bool haveFlyoutClass = g_flyoutWindowClass != 0 ||
                               GetLastError() == ERROR_CLASS_ALREADY_EXISTS;

  g_trayWindow = CreateWindowExW(WS_EX_TOOLWINDOW, kTrayWindowClassName, L"",
                                 WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                                 instance, nullptr);
  if (!g_trayWindow) return false;

  if (!haveFlyoutClass) {
    Wh_Log(L"Status flyout window class unavailable; tray indicator remains active");
  }

  g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
  g_displayedLayout = g_settings.defaultLayout;
  RefreshForCurrentWorkspace();
  return g_iconAdded;
}

static void Shutdown() {
  if (g_iconAdded && g_trayWindow) {
    g_notifyData.uFlags = 0;
    Shell_NotifyIconW(NIM_DELETE, &g_notifyData);
  }
  g_iconAdded = false;
  for (FlyoutInstance* instance : g_flyouts) {
    if (!instance) continue;
    if (instance->hwnd) {
      KillTimer(instance->hwnd, kFlyoutTimerId);
      DestroyWindow(instance->hwnd);
      instance->hwnd = nullptr;
    }
    delete instance;
  }
  g_flyouts.clear();
  if (g_trayWindow) {
    DestroyWindow(g_trayWindow);
    g_trayWindow = nullptr;
  }
  DestroyIconSet(g_icons);

  HINSTANCE instance = GetModuleHandleW(nullptr);
  // Unregister by name even after recovering from ERROR_CLASS_ALREADY_EXISTS,
  // where RegisterClassW didn't return an ATOM for us to remember.
  UnregisterClassW(kFlyoutWindowClassName, instance);
  UnregisterClassW(kTrayWindowClassName, instance);
  g_flyoutWindowClass = g_trayWindowClass = 0;
  g_taskbarCreatedMessage = 0;
  g_displayedLayout = TileLayout::COUNT;
  g_displayedDesktopId = {};
  g_haveDisplayedDesktopId = false;
  g_displayedMonitor = nullptr;
  g_notifyData = {};
}

}  // namespace TrayUi


namespace Diagnostics {

//=============================================================================
// On-demand diagnostic report
//=============================================================================
//
// Diagnostics are intentionally snapshot-based. The WM accumulates only cheap
// counters and a bounded in-memory event tail while running; disk I/O happens
// only when the user explicitly invokes the diagnostic hotkey.

static const wchar_t* DiagnosticLayoutName(TileLayout layout) {
  switch (layout) {
    case TileLayout::MasterStack: return L"MasterStack";
    case TileLayout::Columns: return L"Columns";
    case TileLayout::Rows: return L"Rows";
    case TileLayout::MasterStackH: return L"MasterStackH";
    case TileLayout::Dwindle: return L"Dwindle";
    case TileLayout::Monocle: return L"Monocle";
    case TileLayout::Floating: return L"Floating";
    case TileLayout::COUNT: return L"COUNT";
    default: return L"Unknown";
  }
}

static const wchar_t* DiagnosticManageStateName(ManageState state) {
  switch (state) {
    case ManageState::Tiled: return L"Tiled";
    case ManageState::Floating: return L"Floating";
    case ManageState::Suspended: return L"Suspended";
    case ManageState::Ignored: return L"Ignored";
    default: return L"Unknown";
  }
}

static const wchar_t* DiagnosticSuspensionReasonName(SuspensionReason reason) {
  switch (reason) {
    case SuspensionReason::None: return L"None";
    case SuspensionReason::Hidden: return L"Hidden";
    case SuspensionReason::Minimized: return L"Minimized";
    case SuspensionReason::Maximized: return L"Maximized";
    default: return L"Unknown";
  }
}

static const wchar_t* DiagnosticPlacementResultName(PlacementResult result) {
  switch (result) {
    case PlacementResult::Success: return L"Success";
    case PlacementResult::SuppressedByPhysicalState: return L"SuppressedByPhysicalState";
    case PlacementResult::AdjustedByWindow: return L"AdjustedByWindow";
    case PlacementResult::AccessDenied: return L"AccessDenied";
    case PlacementResult::Refused: return L"Refused";
    case PlacementResult::Dead: return L"Dead";
    default: return L"Unknown";
  }
}

static const wchar_t* DiagnosticMouseMoveBehaviorName(MouseMoveBehavior behavior) {
  return behavior == MouseMoveBehavior::Swap ? L"Swap" : L"Float";
}

class ReportBuilder {
 public:
  void Line() { text_ += L"\r\n"; }

  void Line(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    AppendFormat(format, args);
    va_end(args);
    text_ += L"\r\n";
  }

  void TextLine(const wchar_t* prefix, const std::wstring& value) {
    if (prefix) text_ += prefix;
    text_ += value;
    text_ += L"\r\n";
  }

  void Section(const wchar_t* title) {
    Line();
    Line(L"[%ls]", title);
    Line(L"------------------------------------------------------------");
  }

  const std::wstring& Text() const { return text_; }

 private:
  void AppendFormat(const wchar_t* format, va_list args) {
    size_t capacity = 512;
    while (capacity <= 65536) {
      std::vector<wchar_t> buffer(capacity);
      va_list copy;
      va_copy(copy, args);
      int written = vswprintf(buffer.data(), buffer.size(), format, copy);
      va_end(copy);
      if (written >= 0 && static_cast<size_t>(written) < buffer.size()) {
        text_.append(buffer.data(), static_cast<size_t>(written));
        return;
      }
      capacity *= 2;
    }
    text_ += L"<formatting failed>";
  }

  std::wstring text_;
};

static std::wstring DiagnosticGuidString(const GUID& guid) {
  wchar_t buffer[64]{};
  if (StringFromGUID2(guid, buffer, ARRAYSIZE(buffer)) > 0) return buffer;
  return L"{GUID_FORMAT_FAILED}";
}

static std::wstring DiagnosticRectString(const RECT& rect) {
  wchar_t buffer[128]{};
  swprintf(
      buffer, ARRAYSIZE(buffer), L"[%ld,%ld,%ld,%ld] (%ldx%ld)",
      rect.left, rect.top, rect.right, rect.bottom,
      rect.right - rect.left, rect.bottom - rect.top);
  return buffer;
}

static std::wstring DiagnosticWeightsString(const std::vector<double>& weights) {
  std::wstring result = L"[";
  for (size_t i = 0; i < weights.size(); ++i) {
    wchar_t buffer[64]{};
    swprintf(buffer, ARRAYSIZE(buffer), L"%.9g", weights[i]);
    if (i) result += L", ";
    result += buffer;
  }
  result += L"]";
  return result;
}

static std::wstring DiagnosticLayoutCycleString(const std::vector<TileLayout>& layouts) {
  std::wstring result;
  for (size_t i = 0; i < layouts.size(); ++i) {
    if (i) result += L" -> ";
    result += DiagnosticLayoutName(layouts[i]);
  }
  return result.empty() ? L"<empty>" : result;
}

static std::wstring DiagnosticSanitizeText(const wchar_t* text) {
  if (!text || !*text) return L"";
  std::wstring result = text;
  for (wchar_t& ch : result) {
    if (ch == L'\r' || ch == L'\n' || ch == L'\t') ch = L' ';
  }
  return result;
}

static const wchar_t* DiagnosticDisplayText(const std::wstring& text) {
  return text.empty() ? L"<unavailable>" : text.c_str();
}

static std::wstring DiagnosticDurationString(ULONGLONG milliseconds) {
  ULONGLONG remaining = milliseconds;
  const ULONGLONG days = remaining / 86400000ULL;
  remaining %= 86400000ULL;
  const ULONGLONG hours = remaining / 3600000ULL;
  remaining %= 3600000ULL;
  const ULONGLONG minutes = remaining / 60000ULL;
  remaining %= 60000ULL;
  const ULONGLONG seconds = remaining / 1000ULL;
  const ULONGLONG ms = remaining % 1000ULL;

  wchar_t buffer[96]{};
  if (days) {
    swprintf(
        buffer, ARRAYSIZE(buffer), L"%llud %02llu:%02llu:%02llu.%03llu",
        days, hours, minutes, seconds, ms);
  } else {
    swprintf(
        buffer, ARRAYSIZE(buffer), L"%02llu:%02llu:%02llu.%03llu",
        hours, minutes, seconds, ms);
  }
  return buffer;
}

static std::wstring DiagnosticSystemTimeString(const SYSTEMTIME& time) {
  wchar_t buffer[96]{};
  swprintf(
      buffer, ARRAYSIZE(buffer), L"%04u-%02u-%02u %02u:%02u:%02u.%03u",
      time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
      time.wSecond, time.wMilliseconds);
  return buffer;
}

static std::wstring ExpandDiagnosticPath(const std::wstring& raw) {
  if (raw.empty()) return {};

  DWORD required = ExpandEnvironmentStringsW(raw.c_str(), nullptr, 0);
  std::wstring expanded;
  if (required > 0) {
    std::vector<wchar_t> buffer(required);
    DWORD written = ExpandEnvironmentStringsW(raw.c_str(), buffer.data(), required);
    if (written > 0 && written <= required) expanded.assign(buffer.data());
  }
  if (expanded.empty()) expanded = raw;

  DWORD fullRequired = GetFullPathNameW(expanded.c_str(), 0, nullptr, nullptr);
  if (!fullRequired) return expanded;
  std::vector<wchar_t> full(fullRequired + 1);
  if (!GetFullPathNameW(expanded.c_str(), static_cast<DWORD>(full.size()), full.data(), nullptr)) {
    return expanded;
  }
  return full.data();
}

static bool EnsureDiagnosticDirectory(std::wstring path) {
  if (path.empty()) {
    SetLastError(ERROR_PATH_NOT_FOUND);
    return false;
  }
  for (wchar_t& ch : path) {
    if (ch == L'/') ch = L'\\';
  }

  while (path.size() > 3 && path.back() == L'\\') path.pop_back();

  DWORD attributes = GetFileAttributesW(path.c_str());
  if (attributes != INVALID_FILE_ATTRIBUTES) {
    if (attributes & FILE_ATTRIBUTE_DIRECTORY) return true;
    SetLastError(ERROR_DIRECTORY);
    return false;
  }

  size_t scanFrom = 0;
  if (path.size() >= 3 && path[1] == L':' && path[2] == L'\\') {
    scanFrom = 3;
  } else if (path.rfind(L"\\\\", 0) == 0) {
    // Skip the \\server\share portion of a UNC path before creating children.
    size_t serverEnd = path.find(L'\\', 2);
    if (serverEnd == std::wstring::npos) {
      SetLastError(ERROR_BAD_NETPATH);
      return false;
    }
    size_t shareEnd = path.find(L'\\', serverEnd + 1);
    scanFrom = shareEnd == std::wstring::npos ? path.size() : shareEnd + 1;
  }

  for (size_t i = scanFrom; i < path.size(); ++i) {
    if (path[i] != L'\\') continue;
    std::wstring prefix = path.substr(0, i);
    if (prefix.empty()) continue;
    DWORD prefixAttributes = GetFileAttributesW(prefix.c_str());
    if (prefixAttributes != INVALID_FILE_ATTRIBUTES) continue;
    if (!CreateDirectoryW(prefix.c_str(), nullptr) &&
        GetLastError() != ERROR_ALREADY_EXISTS) {
      return false;
    }
  }

  if (!CreateDirectoryW(path.c_str(), nullptr) &&
      GetLastError() != ERROR_ALREADY_EXISTS) {
    return false;
  }
  attributes = GetFileAttributesW(path.c_str());
  return attributes != INVALID_FILE_ATTRIBUTES &&
         (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static bool WriteUtf8DiagnosticFile(
    const std::wstring& path, const std::wstring& text, DWORD* outError) {
  if (outError) *outError = ERROR_SUCCESS;
  HANDLE file = CreateFileW(
      path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW,
      FILE_ATTRIBUTE_NORMAL, nullptr);
  if (file == INVALID_HANDLE_VALUE) {
    if (outError) *outError = GetLastError();
    return false;
  }

  DWORD writeError = ERROR_SUCCESS;
  auto writeAll = [&](const void* data, size_t size) {
    const BYTE* cursor = static_cast<const BYTE*>(data);
    while (size > 0) {
      const DWORD chunk = static_cast<DWORD>(
          std::min<size_t>(size, static_cast<size_t>(0x7ffff000)));
      DWORD chunkWritten = 0;
      if (!WriteFile(file, cursor, chunk, &chunkWritten, nullptr)) {
        writeError = GetLastError();
        if (writeError == ERROR_SUCCESS) writeError = ERROR_WRITE_FAULT;
        return false;
      }
      if (chunkWritten == 0) {
        writeError = ERROR_WRITE_FAULT;
        return false;
      }
      cursor += chunkWritten;
      size -= chunkWritten;
    }
    return true;
  };

  const BYTE bom[] = {0xEF, 0xBB, 0xBF};
  bool success = writeAll(bom, sizeof(bom));

  if (success && !text.empty()) {
    SetLastError(ERROR_SUCCESS);
    int byteCount = WideCharToMultiByte(
        CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0,
        nullptr, nullptr);
    if (byteCount <= 0) {
      writeError = GetLastError();
      if (writeError == ERROR_SUCCESS) {
        writeError = ERROR_NO_UNICODE_TRANSLATION;
      }
      success = false;
    } else {
      std::vector<char> utf8(static_cast<size_t>(byteCount));
      SetLastError(ERROR_SUCCESS);
      int converted = WideCharToMultiByte(
          CP_UTF8, 0, text.data(), static_cast<int>(text.size()), utf8.data(),
          byteCount, nullptr, nullptr);
      if (converted != byteCount) {
        writeError = GetLastError();
        if (writeError == ERROR_SUCCESS) {
          writeError = ERROR_NO_UNICODE_TRANSLATION;
        }
        success = false;
      } else {
        success = writeAll(utf8.data(), utf8.size());
      }
    }
  }

  if (!success && outError) {
    *outError = writeError != ERROR_SUCCESS ? writeError : ERROR_WRITE_FAULT;
  }
  CloseHandle(file);
  if (!success) DeleteFileW(path.c_str());
  return success;
}

static bool WriteReportToConfiguredDirectory(
    const std::wstring& report, std::wstring* outPath, DWORD* outError) {
  if (outPath) outPath->clear();
  if (outError) *outError = ERROR_SUCCESS;

  const std::wstring directory = ExpandDiagnosticPath(g_settings.diagnosticsOutputPath);
  if (!EnsureDiagnosticDirectory(directory)) {
    if (outError) *outError = GetLastError();
    return false;
  }

  SYSTEMTIME local{};
  GetLocalTime(&local);
  wchar_t baseName[128]{};
  swprintf(
      baseName, ARRAYSIZE(baseName),
      L"MultiWM-Diagnostics-%04u%02u%02u-%02u%02u%02u",
      local.wYear, local.wMonth, local.wDay, local.wHour, local.wMinute,
      local.wSecond);

  for (int suffix = 0; suffix < 1000; ++suffix) {
    std::wstring fileName = baseName;
    if (suffix) {
      fileName += L"-";
      fileName += std::to_wstring(suffix + 1);
    }
    fileName += L".txt";

    std::wstring path = directory;
    if (!path.empty() && path.back() != L'\\' && path.back() != L'/') path += L'\\';
    path += fileName;

    DWORD error = ERROR_SUCCESS;
    if (WriteUtf8DiagnosticFile(path, report, &error)) {
      if (outPath) *outPath = path;
      return true;
    }
    if (error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS) {
      if (outError) *outError = error;
      return false;
    }
  }

  if (outError) *outError = ERROR_FILE_EXISTS;
  return false;
}

struct DiagnosticWindowIdentity {
  bool hwndAlive = false;
  DWORD livePid = 0;
  bool pidMatchesRecord = false;
  std::wstring processName;
  std::wstring imagePath;
  std::wstring className;
  std::wstring title;

  bool visible = false;
  bool minimized = false;
  bool maximized = false;
  bool cloaked = false;
  bool hasFrame = false;
  RECT frame{};
  HMONITOR monitor = nullptr;
  UINT dpi = 96;
};

static DiagnosticWindowIdentity DiagnosticReadWindowIdentity(
    HWND hwnd, DWORD recordPid) {
  DiagnosticWindowIdentity identity;
  identity.hwndAlive = hwnd && IsWindow(hwnd);

  if (identity.hwndAlive) {
    GetWindowThreadProcessId(hwnd, &identity.livePid);
    identity.pidMatchesRecord = recordPid != 0 && identity.livePid == recordPid;
    identity.visible = IsWindowVisible(hwnd) != FALSE;
    identity.minimized = IsIconic(hwnd) != FALSE;
    identity.maximized = IsZoomed(hwnd) != FALSE;
    identity.cloaked = IsWindowCloaked(hwnd);
    identity.hasFrame = GetWindowFrameRect(hwnd, &identity.frame) &&
                        identity.frame.right > identity.frame.left &&
                        identity.frame.bottom > identity.frame.top;
    identity.monitor = GetWindowPhysicalMonitor(hwnd);
    if (identity.monitor) identity.dpi = GetMonitorEffectiveDpi(identity.monitor);

    wchar_t className[256]{};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) > 0) {
      identity.className = DiagnosticSanitizeText(className);
    }

    wchar_t title[512]{};
    if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) > 0) {
      identity.title = DiagnosticSanitizeText(title);
    }
  }

  DWORD queryPid = identity.livePid ? identity.livePid : recordPid;
  if (queryPid) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, queryPid);
    if (process) {
      std::vector<wchar_t> path(32768);
      DWORD chars = static_cast<DWORD>(path.size());
      if (QueryFullProcessImageNameW(process, 0, path.data(), &chars) && chars > 0) {
        identity.imagePath.assign(path.data(), chars);
        const wchar_t* base = wcsrchr(identity.imagePath.c_str(), L'\\');
        identity.processName = base ? base + 1 : identity.imagePath;
      }
      CloseHandle(process);
    }
  }

  return identity;
}

static size_t DiagnosticActiveIndex(const Workspace& state, HWND hwnd) {
  return state.TiledIndex(hwnd);
}

static size_t DiagnosticLogicalIndex(
    const Workspace& state, HWND hwnd, const WindowRecord&) {
  return state.LogicalIndex(hwnd);
}

static bool DiagnosticIsMinimizedSentinelFrame(const RECT& frame) {
  return frame.left <= -30000 || frame.top <= -30000;
}

struct DiagnosticRecordAudit {
  bool deadHwnd = false;
  bool pidMismatch = false;
  bool hasPlacementFailure = false;
};

static DiagnosticRecordAudit DiagnosticDumpWindowRecord(
    ReportBuilder& report, const Workspace& state, HWND mapHwnd,
    const WindowRecord& record, const wchar_t* prefix, size_t ordinal) {
  DiagnosticRecordAudit audit;
  const size_t activeIndex = DiagnosticActiveIndex(state, mapHwnd);
  const size_t logicalIndex = DiagnosticLogicalIndex(state, mapHwnd, record);
  const bool hasActiveIndex = activeIndex != static_cast<size_t>(-1);
  const bool hasLogicalIndex = logicalIndex != static_cast<size_t>(-1);

  report.Line(
      L"  %ls[%zu] logicalIndex=%ls activeIndex=%ls hwnd=%p",
      prefix, ordinal,
      hasLogicalIndex ? std::to_wstring(logicalIndex).c_str() : L"-",
      hasActiveIndex ? std::to_wstring(activeIndex).c_str() : L"-",
      reinterpret_cast<void*>(mapHwnd));
  report.Line(
      L"      recordHwnd=%p pid=%lu state=%ls suspension=%ls alwaysFloating=%d",
      reinterpret_cast<void*>(record.hwnd), record.pid,
      DiagnosticManageStateName(record.state),
      DiagnosticSuspensionReasonName(record.suspensionReason),
      record.alwaysFloating ? 1 : 0);

  DiagnosticWindowIdentity identity = DiagnosticReadWindowIdentity(mapHwnd, record.pid);
  audit.deadHwnd = !identity.hwndAlive;
  audit.pidMismatch = identity.hwndAlive && record.pid != 0 && !identity.pidMatchesRecord;
  audit.hasPlacementFailure =
      record.lastPlacementResult != PlacementResult::Success &&
      record.lastPlacementResult != PlacementResult::SuppressedByPhysicalState;

  report.Line(
      L"      identity: hwndAlive=%d livePid=%lu pidMatchesRecord=%d process=%ls",
      identity.hwndAlive ? 1 : 0, identity.livePid,
      identity.pidMatchesRecord ? 1 : 0,
      DiagnosticDisplayText(identity.processName));
  report.TextLine(L"      image: ", identity.imagePath.empty() ? L"<unavailable>" : identity.imagePath);
  report.TextLine(L"      class: ", identity.className.empty() ? L"<unavailable>" : identity.className);
  report.TextLine(L"      title: ", identity.title.empty() ? L"<unavailable>" : identity.title);
  report.Line(
      L"      liveState: visible=%d minimized=%d maximized=%d cloaked=%d monitor=%p dpi=%u",
      identity.visible ? 1 : 0, identity.minimized ? 1 : 0,
      identity.maximized ? 1 : 0, identity.cloaked ? 1 : 0,
      reinterpret_cast<void*>(identity.monitor), identity.dpi);
  if (identity.minimized && identity.hasFrame &&
      DiagnosticIsMinimizedSentinelFrame(identity.frame)) {
    report.Line(L"      liveFrame=<Windows minimized sentinel omitted>");
  } else if (identity.hasFrame) {
    report.Line(L"      liveFrame=%ls", DiagnosticRectString(identity.frame).c_str());
  } else {
    report.Line(L"      liveFrame=<unavailable>");
  }

  report.Line(
      L"      capabilities: move=%d resize=%d topmost=%d",
      record.canMove ? 1 : 0, record.canResize ? 1 : 0, record.topmost ? 1 : 0);
  report.Line(
      L"      constraints: generation=%llu pendingInsertion=%d pendingFloatCenter=%d valid=%d dpi=%u min=%ldx%ld timeout=%d error=%lu",
      static_cast<unsigned long long>(record.tilingGeneration),
      record.pendingInsertionPlacement ? 1 : 0,
      record.pendingDefaultFloatingCenter ? 1 : 0,
      record.constraints.valid ? 1 : 0, record.constraints.dpi,
      record.constraints.minWidth, record.constraints.minHeight,
      record.constraints.lastProbeError == ERROR_TIMEOUT ? 1 : 0,
      record.constraints.lastProbeError);
  report.Line(
      L"      placement: result=%ls",
      DiagnosticPlacementResultName(record.lastPlacementResult));
  report.Line(L"      lastRequested=%ls", DiagnosticRectString(record.lastRequestedRect).c_str());
  report.Line(L"      lastObserved =%ls", DiagnosticRectString(record.lastObservedRect).c_str());

  if (record.hasFloatingRect) {
    report.Line(
        L"      floatingGeometry=%ls dpi=%u monitor=%p",
        DiagnosticRectString(record.floatingRect).c_str(), record.floatingDpi,
        reinterpret_cast<void*>(record.floatingMonitor));
  } else {
    report.Line(L"      floatingGeometry=none");
  }

  if (record.hasSavedSlot) {
    report.Line(
        L"      savedSlot: index=%zu weight=%.9g hasWeight=%d wasMaster=%d",
        record.savedSlot.index, record.savedSlot.weight,
        record.savedSlot.hasWeight ? 1 : 0, record.savedSlot.wasMaster ? 1 : 0);
  } else {
    report.Line(L"      savedSlot: none");
  }
  return audit;
}

static uint64_t CounterDelta(uint64_t current, uint64_t previous) {
  return current >= previous ? current - previous : current;
}

static void DiagnosticCounterRow(
    ReportBuilder& report, const wchar_t* label, uint64_t current,
    uint64_t previous, ULONGLONG uptimeMs, ULONGLONG intervalMs) {
  const uint64_t delta = CounterDelta(current, previous);
  const double lifetimeRate = uptimeMs
      ? static_cast<double>(current) * 60000.0 / static_cast<double>(uptimeMs)
      : 0.0;
  const double intervalRate = intervalMs
      ? static_cast<double>(delta) * 60000.0 / static_cast<double>(intervalMs)
      : 0.0;
  report.Line(
      L"%-34ls %12llu %12llu %12.2f %12.2f",
      label, static_cast<unsigned long long>(current),
      static_cast<unsigned long long>(delta), lifetimeRate, intervalRate);
}

static void DiagnosticCounterHeader(ReportBuilder& report) {
  report.Line(L"%-34ls %12ls %12ls %12ls %12ls", L"counter", L"total", L"since dump", L"total/min", L"period/min");
  report.Line(L"%-34ls %12ls %12ls %12ls %12ls", L"----------------------------------", L"------------", L"------------", L"------------", L"------------");
}

static void DumpDerivedPerformanceIndicators(
    ReportBuilder& report, const Counters& c) {
  const double coalescingFactor = c.lifecycleReconciles
      ? static_cast<double>(c.lifecycleScheduleRequests) /
            static_cast<double>(c.lifecycleReconciles)
      : 0.0;
  const double hwndsPerReconcile = c.reconcileCalls
      ? static_cast<double>(c.reconcileWindowsExamined) /
            static_cast<double>(c.reconcileCalls)
      : 0.0;
  const double hwndsPerEnumeration = c.enumWindowsPasses
      ? static_cast<double>(c.enumWindowsVisited) /
            static_cast<double>(c.enumWindowsPasses)
      : 0.0;
  const double placementNoOpPercent = c.placementChecks
      ? static_cast<double>(c.placementNoOps) * 100.0 /
            static_cast<double>(c.placementChecks)
      : 0.0;
  const uint64_t problematicPlacements = c.placementAdjusted +
      c.placementAccessDenied + c.placementRefused + c.placementDead;
  const double placementProblemPercent = c.placementChecks
      ? static_cast<double>(problematicPlacements) * 100.0 /
            static_cast<double>(c.placementChecks)
      : 0.0;

  report.Line(L"Derived indicators:");
  report.Line(
      L"  lifecycle coalescing factor: %.2f schedule requests / settled reconcile",
      coalescingFactor);
  report.Line(
      L"  average known HWNDs examined per reconcile call: %.2f",
      hwndsPerReconcile);
  report.Line(
      L"  average top-level HWNDs visited per EnumWindows pass: %.2f",
      hwndsPerEnumeration);
  report.Line(
      L"  placement checks avoided by already-correct geometry: %.2f%%",
      placementNoOpPercent);
  report.Line(
      L"  placement checks ending adjusted/refused/dead: %.2f%%",
      placementProblemPercent);
}

static void DumpCounters(
    ReportBuilder& report, const Counters& c, const Counters& previous,
    ULONGLONG uptimeMs, ULONGLONG intervalMs) {
  report.Line(L"Event / reconciliation churn");
  DiagnosticCounterHeader(report);
  DiagnosticCounterRow(report, L"Lifecycle events processed", c.lifecycleEventsProcessed, previous.lifecycleEventsProcessed, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  destroy", c.lifecycleDestroy, previous.lifecycleDestroy, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  minimize start", c.lifecycleMinimizeStart, previous.lifecycleMinimizeStart, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  minimize end", c.lifecycleMinimizeEnd, previous.lifecycleMinimizeEnd, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  state change", c.lifecycleStateChange, previous.lifecycleStateChange, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  location change", c.lifecycleLocationChange, previous.lifecycleLocationChange, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  hide", c.lifecycleHide, previous.lifecycleHide, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  show", c.lifecycleShow, previous.lifecycleShow, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  cloaked", c.lifecycleCloaked, previous.lifecycleCloaked, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  uncloaked", c.lifecycleUncloaked, previous.lifecycleUncloaked, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Lifecycle schedule requests", c.lifecycleScheduleRequests, previous.lifecycleScheduleRequests, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Lifecycle timer resets", c.lifecycleTimerResets, previous.lifecycleTimerResets, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Settled lifecycle reconciles", c.lifecycleReconciles, previous.lifecycleReconciles, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Dirty HWNDs processed", c.lifecycleDirtyWindowsProcessed, previous.lifecycleDirtyWindowsProcessed, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Move/size end messages", c.moveSizeEndMessages, previous.moveSizeEndMessages, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Foreground/tray refreshes", c.trayRefreshMessages, previous.trayRefreshMessages, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"User move gestures", c.userMoveGestures, previous.userMoveGestures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"User resize gestures", c.userResizeGestures, previous.userResizeGestures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Unclassified/no-op gestures", c.userNoopGestures, previous.userNoopGestures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Floating geometry user updates", c.floatingGeometryUserUpdates, previous.floatingGeometryUserUpdates, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Tiled drag -> Floating actions", c.tiledFloatActions, previous.tiledFloatActions, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Tiled drag swap actions", c.tiledSwapActions, previous.tiledSwapActions, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Divider weight updates", c.dividerUpdates, previous.dividerUpdates, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Rejected resize gestures", c.rejectedResizeGestures, previous.rejectedResizeGestures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Reconcile calls", c.reconcileCalls, previous.reconcileCalls, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Known HWNDs examined", c.reconcileWindowsExamined, previous.reconcileWindowsExamined, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Workspace saves", c.workspaceSaves, previous.workspaceSaves, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Workspace migrations", c.workspaceMigrations, previous.workspaceMigrations, uptimeMs, intervalMs);

  report.Line();
  report.Line(L"Enumeration / layout / placement");
  DiagnosticCounterHeader(report);
  DiagnosticCounterRow(report, L"EnumWindows passes", c.enumWindowsPasses, previous.enumWindowsPasses, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Top-level HWNDs visited", c.enumWindowsVisited, previous.enumWindowsVisited, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Tile candidates collected", c.tileCandidatesCollected, previous.tileCandidatesCollected, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Arrange calls", c.arrangeCalls, previous.arrangeCalls, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Arrange passes", c.arrangePasses, previous.arrangePasses, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Layout plans built", c.layoutPlansBuilt, previous.layoutPlansBuilt, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Placement checks", c.placementChecks, previous.placementChecks, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Placement no-ops", c.placementNoOps, previous.placementNoOps, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Placement preflight stops", c.placementPreflightStops, previous.placementPreflightStops, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"SetWindowPos calls", c.setWindowPosCalls, previous.setWindowPosCalls, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Placement success", c.placementSuccess, previous.placementSuccess, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Suppressed by physical state", c.placementSuppressedByPhysicalState, previous.placementSuppressedByPhysicalState, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Adjusted by window", c.placementAdjusted, previous.placementAdjusted, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Placement access denied", c.placementAccessDenied, previous.placementAccessDenied, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Placement refused", c.placementRefused, previous.placementRefused, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Placement dead HWND", c.placementDead, previous.placementDead, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Floating geometry repairs", c.floatingGeometryRepairs, previous.floatingGeometryRepairs, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Constraint probes", c.constraintProbeAttempts, previous.constraintProbeAttempts, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  constraint probe successes", c.constraintProbeSuccesses, previous.constraintProbeSuccesses, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  constraint probe timeouts", c.constraintProbeTimeouts, previous.constraintProbeTimeouts, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"  constraint probe failures", c.constraintProbeFailures, previous.constraintProbeFailures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Constraint-unsatisfiable plans", c.constraintUnsatisfiablePlans, previous.constraintUnsatisfiablePlans, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Constraint-triggered floats", c.constraintFloats, previous.constraintFloats, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Conformance leases started", c.conformanceLeasesStarted, previous.conformanceLeasesStarted, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Conformance lease reinforcement attempts", c.conformanceLeaseRepairs, previous.conformanceLeaseRepairs, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Conformance lease deferred burst repairs", c.conformanceLeaseDeferredRepairs, previous.conformanceLeaseDeferredRepairs, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Conformance deadlines completed compliant", c.conformanceLeaseExpiredCompliant, previous.conformanceLeaseExpiredCompliant, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Conformance deadlines floated window", c.conformanceLeaseExpiredFloats, previous.conformanceLeaseExpiredFloats, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Conformance deadline stale/cancelled verdicts", c.conformanceLeaseExpiredStale, previous.conformanceLeaseExpiredStale, uptimeMs, intervalMs);

  report.Line();
  report.Line(L"Virtual desktop / runtime lifecycle / user commands");
  DiagnosticCounterHeader(report);
  DiagnosticCounterRow(report, L"Native VD changes processed", c.virtualDesktopChangesProcessed, previous.virtualDesktopChangesProcessed, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Fallback VD changes detected", c.virtualDesktopFallbackChanges, previous.virtualDesktopFallbackChanges, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"VD API init attempts", c.vdApiInitAttempts, previous.vdApiInitAttempts, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"VD API init successes", c.vdApiInitSuccesses, previous.vdApiInitSuccesses, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"VD API init failures", c.vdApiInitFailures, previous.vdApiInitFailures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"VD API reinitializations", c.vdApiReinitializations, previous.vdApiReinitializations, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"VD notification register attempts", c.vdNotificationRegisterAttempts, previous.vdNotificationRegisterAttempts, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"VD notification register successes", c.vdNotificationRegisterSuccesses, previous.vdNotificationRegisterSuccesses, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"VD notification register failures", c.vdNotificationRegisterFailures, previous.vdNotificationRegisterFailures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Runtime maintenance runs", c.runtimeMaintenanceRuns, previous.runtimeMaintenanceRuns, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Runtime maintenance retries", c.runtimeMaintenanceRetries, previous.runtimeMaintenanceRetries, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Runtime maintenance exhausted", c.runtimeMaintenanceExhausted, previous.runtimeMaintenanceExhausted, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Explorer restart signals", c.shellRestartSignals, previous.shellRestartSignals, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Display/work-area signals", c.displayTopologySignals, previous.displayTopologySignals, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"WinEvent hook install failures", c.winEventHookInstallFailures, previous.winEventHookInstallFailures, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Tile commands", c.tileCommands, previous.tileCommands, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Layout cycle commands", c.layoutCycleCommands, previous.layoutCycleCommands, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Float focused commands", c.floatFocusedCommands, previous.floatFocusedCommands, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Swap master commands", c.swapMasterCommands, previous.swapMasterCommands, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Promote window commands", c.promoteWindowCommands, previous.promoteWindowCommands, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Demote window commands", c.demoteWindowCommands, previous.demoteWindowCommands, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Rejected order commands", c.rejectedOrderCommands, previous.rejectedOrderCommands, uptimeMs, intervalMs);
  DiagnosticCounterRow(report, L"Rejected layout changes", c.rejectedLayoutChanges, previous.rejectedLayoutChanges, uptimeMs, intervalMs);
}

static uint64_t DiagnosticFileTime100ns(const FILETIME& value) {
  ULARGE_INTEGER ticks{};
  ticks.LowPart = value.dwLowDateTime;
  ticks.HighPart = value.dwHighDateTime;
  return ticks.QuadPart;
}

static ULONGLONG Diagnostic100nsToMilliseconds(uint64_t ticks100ns) {
  return static_cast<ULONGLONG>(ticks100ns / 10000ULL);
}

struct DiagnosticCpuSnapshot {
  bool processAvailable = false;
  bool wmAvailable = false;
  uint64_t processKernel100ns = 0;
  uint64_t processUser100ns = 0;
  uint64_t wmKernel100ns = 0;
  uint64_t wmUser100ns = 0;

  uint64_t ProcessTotal100ns() const {
    return processKernel100ns + processUser100ns;
  }

  uint64_t WmTotal100ns() const {
    return wmKernel100ns + wmUser100ns;
  }
};

static DiagnosticCpuSnapshot CaptureDiagnosticCpuSnapshot() {
  DiagnosticCpuSnapshot snapshot;
  FILETIME creation{}, exit{}, kernel{}, user{};
  if (GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user)) {
    snapshot.processAvailable = true;
    snapshot.processKernel100ns = DiagnosticFileTime100ns(kernel);
    snapshot.processUser100ns = DiagnosticFileTime100ns(user);
  }

  if (g_wm.thread &&
      GetThreadTimes(g_wm.thread, &creation, &exit, &kernel, &user)) {
    snapshot.wmAvailable = true;
    snapshot.wmKernel100ns = DiagnosticFileTime100ns(kernel);
    snapshot.wmUser100ns = DiagnosticFileTime100ns(user);
  }
  return snapshot;
}

static uint64_t DiagnosticCpuDelta100ns(
    uint64_t endValue, uint64_t startValue) {
  return endValue >= startValue ? endValue - startValue : 0;
}

static void AccumulateDiagnosticSelfCost(
    ULONGLONG startTickMs, const DiagnosticCpuSnapshot& startCpu) {
  const ULONGLONG endTickMs = GetTickCount64();
  const DiagnosticCpuSnapshot endCpu = CaptureDiagnosticCpuSnapshot();

  const ULONGLONG wallMs =
      endTickMs >= startTickMs ? endTickMs - startTickMs : 0;
  uint64_t processCpu100ns = 0;
  uint64_t wmCpu100ns = 0;
  if (startCpu.processAvailable && endCpu.processAvailable) {
    processCpu100ns = DiagnosticCpuDelta100ns(
        endCpu.ProcessTotal100ns(), startCpu.ProcessTotal100ns());
  }
  if (startCpu.wmAvailable && endCpu.wmAvailable) {
    wmCpu100ns = DiagnosticCpuDelta100ns(
        endCpu.WmTotal100ns(), startCpu.WmTotal100ns());
  }

  ++g_runtime.lifetimeDiagnosticCost.attempts;
  g_runtime.lifetimeDiagnosticCost.wallMs += wallMs;
  g_runtime.lifetimeDiagnosticCost.processCpu100ns += processCpu100ns;
  g_runtime.lifetimeDiagnosticCost.wmCpu100ns += wmCpu100ns;

  ++g_runtime.wmSessionDiagnosticCost.attempts;
  g_runtime.wmSessionDiagnosticCost.wallMs += wallMs;
  g_runtime.wmSessionDiagnosticCost.processCpu100ns += processCpu100ns;
  g_runtime.wmSessionDiagnosticCost.wmCpu100ns += wmCpu100ns;
}

static void DumpCpuUsage(
    ReportBuilder& report, ULONGLONG processUptimeMs, ULONGLONG wmSessionMs,
    const DiagnosticCpuSnapshot& snapshot) {
  if (snapshot.processAvailable) {
    const uint64_t raw100ns = snapshot.ProcessTotal100ns();
    const uint64_t priorDiagnostic100ns = std::min<uint64_t>(
        raw100ns, g_runtime.lifetimeDiagnosticCost.processCpu100ns);
    const uint64_t operational100ns = raw100ns - priorDiagnostic100ns;
    const ULONGLONG rawMs = Diagnostic100nsToMilliseconds(raw100ns);
    const ULONGLONG operationalMs =
        Diagnostic100nsToMilliseconds(operational100ns);
    const double rawAverageCore = processUptimeMs
        ? static_cast<double>(rawMs) * 100.0 /
              static_cast<double>(processUptimeMs)
        : 0.0;
    const double operationalAverageCore = processUptimeMs
        ? static_cast<double>(operationalMs) * 100.0 /
              static_cast<double>(processUptimeMs)
        : 0.0;
    report.Line(
        L"Tool process CPU raw: kernel=%ls user=%ls total=%ls average-one-core=%.3f%%",
        DiagnosticDurationString(
            Diagnostic100nsToMilliseconds(snapshot.processKernel100ns)).c_str(),
        DiagnosticDurationString(
            Diagnostic100nsToMilliseconds(snapshot.processUser100ns)).c_str(),
        DiagnosticDurationString(rawMs).c_str(), rawAverageCore);
    report.Line(
        L"Tool process CPU excluding prior diagnostic reporting: total=%ls average-one-core=%.3f%%",
        DiagnosticDurationString(operationalMs).c_str(), operationalAverageCore);
  } else {
    report.Line(L"Tool process CPU: <unavailable>");
  }

  if (snapshot.wmAvailable) {
    const uint64_t raw100ns = snapshot.WmTotal100ns();
    const uint64_t priorDiagnostic100ns = std::min<uint64_t>(
        raw100ns, g_runtime.wmSessionDiagnosticCost.wmCpu100ns);
    const uint64_t operational100ns = raw100ns - priorDiagnostic100ns;
    const ULONGLONG rawMs = Diagnostic100nsToMilliseconds(raw100ns);
    const ULONGLONG operationalMs =
        Diagnostic100nsToMilliseconds(operational100ns);
    const double rawAverageCore = wmSessionMs
        ? static_cast<double>(rawMs) * 100.0 /
              static_cast<double>(wmSessionMs)
        : 0.0;
    const double operationalAverageCore = wmSessionMs
        ? static_cast<double>(operationalMs) * 100.0 /
              static_cast<double>(wmSessionMs)
        : 0.0;
    report.Line(
        L"WM thread CPU raw: kernel=%ls user=%ls total=%ls average-one-core=%.3f%%",
        DiagnosticDurationString(
            Diagnostic100nsToMilliseconds(snapshot.wmKernel100ns)).c_str(),
        DiagnosticDurationString(
            Diagnostic100nsToMilliseconds(snapshot.wmUser100ns)).c_str(),
        DiagnosticDurationString(rawMs).c_str(), rawAverageCore);
    report.Line(
        L"WM thread CPU excluding prior diagnostic reporting: total=%ls average-one-core=%.3f%%",
        DiagnosticDurationString(operationalMs).c_str(), operationalAverageCore);
  } else {
    report.Line(L"WM thread CPU: <unavailable>");
  }
}

static void DumpPlatformHealth(ReportBuilder& report) {
  report.Line(
      L"WM thread: id=%lu COM(STA)=%d",
      g_wm.threadId.load(std::memory_order_acquire),
      g_wm.comInitialized ? 1 : 0);
  if (g_vd.lastKnownDesktopValid) {
    report.Line(
        L"Current virtual desktop: %ls",
        DiagnosticGuidString(g_vd.lastKnownDesktop).c_str());
  } else {
    report.Line(L"Current virtual desktop: <unknown>");
  }
  std::vector<HMONITOR> connectedMonitors;
  Reconcile::EnumerateCurrentMonitors(connectedMonitors);
  report.Line(
      L"Connected physical monitors: %zu", connectedMonitors.size());
  for (HMONITOR monitor : connectedMonitors) {
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    Model::MonitorId id;
    if (GetMonitorInfoW(monitor, reinterpret_cast<MONITORINFO*>(&info)) &&
        Model::MonitorId::FromHMonitor(monitor, &id)) {
      report.Line(L"  %ls primary=%d monitorId: %ls", info.szDevice,
                  (info.dwFlags & MONITORINFOF_PRIMARY) ? 1 : 0,
                  id.deviceId.c_str());
    }
  }
  report.Line(
      L"Virtual desktop API: initialized=%d abiResolved=%d Explorer=%lu.%lu usesHMonitor=%d",
      g_vd.initialized ? 1 : 0, g_vd.abiResolved ? 1 : 0,
      g_vd.explorerBuild, g_vd.explorerRevision,
      g_vd.abi.usesHMonitor ? 1 : 0);
  HWND currentShell = GetShellWindow();
  DWORD currentShellPid = 0;
  if (currentShell && IsWindow(currentShell)) {
    GetWindowThreadProcessId(currentShell, &currentShellPid);
  }
  report.Line(
      L"Explorer shell identity: boundPid=%lu currentPid=%lu match=%d",
      g_vd.shellPid, currentShellPid,
      g_vd.shellPid && currentShellPid && g_vd.shellPid == currentShellPid ? 1 : 0);
  currentShellPid = 0;
  HWND shell = GetShellWindow();
  if (shell && IsWindow(shell)) GetWindowThreadProcessId(shell, &currentShellPid);
  report.Line(
      L"Explorer shell binding: currentPid=%lu boundPid=%lu match=%d",
      currentShellPid, g_vd.shellPid,
      currentShellPid && g_vd.shellPid && currentShellPid == g_vd.shellPid ? 1 : 0);
  report.Line(
      L"VD notifications: registered=%d cookie=%lu methods=%d currentChangedIndex=%d changeQueued=%ld lastHr=0x%08X",
      g_vd.notificationsRegistered ? 1 : 0, g_vd.notificationCookie,
      g_vd.abi.notification.methodCount,
      g_vd.abi.notification.currentChangedIndex, g_vd.changeQueued,
      static_cast<unsigned int>(g_vd.notificationLastHr));
  const ULONGLONG now = GetTickCount64();
  const std::wstring notificationAttemptAge =
      g_vd.notificationLastAttemptTickMs
          ? DiagnosticDurationString(
                now >= g_vd.notificationLastAttemptTickMs
                    ? now - g_vd.notificationLastAttemptTickMs
                    : 0)
          : L"never";
  const std::wstring notificationSuccessAge =
      g_vd.notificationLastSuccessTickMs
          ? DiagnosticDurationString(
                now >= g_vd.notificationLastSuccessTickMs
                    ? now - g_vd.notificationLastSuccessTickMs
                    : 0)
          : L"never";
  report.Line(
      L"VD notification timing: lastAttemptAge=%ls lastSuccessAge=%ls",
      notificationAttemptAge.c_str(), notificationSuccessAge.c_str());
  report.Line(
      L"Runtime maintenance: running=%d timerArmed=%d attempt=%u/%u deferredLifecycle=%d forceMonitorReconcile=%d",
      g_wm.maintenanceRunning ? 1 : 0, g_wm.maintenanceTimer ? 1 : 0,
      g_wm.maintenanceAttempts, RuntimeLifecycle::kMaintenanceMaxAttempts,
      g_wm.lifecycleRetryAfterPlatformRecovery ? 1 : 0,
      g_wm.forceMonitorReconcile ? 1 : 0);
  report.Line(
      L"WinEvent hooks: foreground=%d location=%d moveSize=%d minimize=%d hideDestroy=%d cloak=%d state=%d",
      g_hooks.foreground ? 1 : 0, g_hooks.locationChange ? 1 : 0,
      g_hooks.moveSize ? 1 : 0, g_hooks.minimize ? 1 : 0,
      g_hooks.hideDestroy ? 1 : 0, g_hooks.cloak ? 1 : 0,
      g_hooks.state ? 1 : 0);

  size_t moveStarts = 0, moveEnds = 0, movePoints = 0;
  moveStarts = g_moveSize.startRects.size();
  moveEnds = g_moveSize.endRects.size();
  movePoints = g_moveSize.endPoints.size();
  report.Line(
      L"Move/size cache: starts=%zu ends=%zu endPoints=%zu", moveStarts,
      moveEnds, movePoints);
}

static void DumpRecentEvents(ReportBuilder& report, ULONGLONG now) {
  if (g_runtime.significantEvents.empty()) {
    report.Line(L"(none)");
    return;
  }

  for (const RecentEvent& event : g_runtime.significantEvents) {
    ULONGLONG sinceStart = event.tickMs >= g_runtime.processStartedTickMs
        ? event.tickMs - g_runtime.processStartedTickMs
        : 0;
    ULONGLONG age = now >= event.tickMs ? now - event.tickMs : 0;
    report.Line(
        L"+%ls  age=%ls  %ls",
        DiagnosticDurationString(sinceStart).c_str(),
        DiagnosticDurationString(age).c_str(), event.text.c_str());
  }
}

static void DumpLifecycleBursts(ReportBuilder& report, ULONGLONG now) {
  if (g_runtime.lifecycleBursts.empty()) {
    report.Line(L"(none)");
    return;
  }

  for (const LifecycleBurst& burst : g_runtime.lifecycleBursts) {
    ULONGLONG sinceStart = burst.tickMs >= g_runtime.processStartedTickMs
        ? burst.tickMs - g_runtime.processStartedTickMs
        : 0;
    ULONGLONG age = now >= burst.tickMs ? now - burst.tickMs : 0;
    report.Line(
        L"+%ls  age=%ls  dirtyWindows=%zu pendingArranges=%zu",
        DiagnosticDurationString(sinceStart).c_str(),
        DiagnosticDurationString(age).c_str(),
        burst.dirtyWindows, burst.pendingArranges);
  }
}

static void WriteDiagnosticReport() {
  InitializeProcessRuntime();
  const ULONGLONG reportStartTickMs = GetTickCount64();
  const DiagnosticCpuSnapshot reportStartCpu = CaptureDiagnosticCpuSnapshot();
  const ULONGLONG now = reportStartTickMs;
  const ULONGLONG uptimeMs = now - g_runtime.processStartedTickMs;
  const ULONGLONG wmSessionMs = g_runtime.wmSessionStartedTickMs
      ? now - g_runtime.wmSessionStartedTickMs
      : 0;
  const ULONGLONG intervalMs = g_runtime.lastReportTickMs
      ? now - g_runtime.lastReportTickMs
      : uptimeMs;
  const Counters counters = g_runtime.counters;
  const Counters previous = g_runtime.lastReportCounters;
  const uint64_t reportSequence = g_runtime.reportSequence + 1;

  std::vector<std::pair<DesktopMonitorKey, Workspace>> workspaces =
      g_workspaces.Snapshot();
  std::vector<std::pair<HWND, std::vector<DesktopMonitorKey>>> ownershipIndex =
      g_workspaces.OwnershipIndexSnapshot();
  std::sort(
      workspaces.begin(), workspaces.end(),
      [](const auto& a, const auto& b) {
        std::wstring aDesktop = DiagnosticGuidString(a.first.desktopId);
        std::wstring bDesktop = DiagnosticGuidString(b.first.desktopId);
        if (aDesktop != bDesktop) return aDesktop < bDesktop;
        return a.first.monitor.deviceId < b.first.monitor.deviceId;
      });

  size_t totalRecords = 0;
  size_t tiledCount = 0;
  size_t floatingCount = 0;
  size_t suspendedCount = 0;
  size_t ignoredCount = 0;
  std::unordered_map<HWND, size_t> ownerCounts;
  std::unordered_map<HWND, std::vector<DesktopMonitorKey>> expectedOwners;
  for (const auto& workspaceKv : workspaces) {
    for (const auto& recordKv : workspaceKv.second.Records()) {
      ++totalRecords;
      ++ownerCounts[recordKv.first];
      expectedOwners[recordKv.first].push_back(workspaceKv.first);
      switch (recordKv.second.state) {
        case ManageState::Tiled: ++tiledCount; break;
        case ManageState::Floating: ++floatingCount; break;
        case ManageState::Suspended: ++suspendedCount; break;
        case ManageState::Ignored: ++ignoredCount; break;
      }
    }
  }
  size_t duplicateOwners = 0;
  for (const auto& ownerKv : ownerCounts) {
    if (ownerKv.second > 1) ++duplicateOwners;
  }

  auto sameKeySet = [](
      const std::vector<DesktopMonitorKey>& a,
      const std::vector<DesktopMonitorKey>& b) {
    if (a.size() != b.size()) return false;
    for (const DesktopMonitorKey& key : a) {
      if (std::none_of(
              b.begin(), b.end(), [&](const DesktopMonitorKey& candidate) {
                return Model::DesktopMonitorKeyEqual{}(key, candidate);
              })) {
        return false;
      }
    }
    return true;
  };

  size_t ownershipIndexMismatches = 0;
  std::unordered_set<HWND> indexedHwnds;
  for (const auto& indexKv : ownershipIndex) {
    indexedHwnds.insert(indexKv.first);
    auto expectedIt = expectedOwners.find(indexKv.first);
    if (expectedIt == expectedOwners.end() ||
        !sameKeySet(indexKv.second, expectedIt->second)) {
      ++ownershipIndexMismatches;
    }
  }
  for (const auto& expectedKv : expectedOwners) {
    if (indexedHwnds.find(expectedKv.first) == indexedHwnds.end()) {
      ++ownershipIndexMismatches;
    }
  }

  SYSTEMTIME local{}, utc{};
  GetLocalTime(&local);
  GetSystemTime(&utc);

  ReportBuilder report;
  report.Line(L"============================================================");
  report.Line(L"MULTIWM DIAGNOSTIC REPORT");
  report.Line(L"============================================================");
  report.Line(L"Mod version: %ls", WH_MOD_VERSION);
  report.Line(L"Report sequence: %llu", static_cast<unsigned long long>(reportSequence));
  report.Line(L"Generated local: %ls", DiagnosticSystemTimeString(local).c_str());
  report.Line(L"Generated UTC:   %ls", DiagnosticSystemTimeString(utc).c_str());
  report.Line(L"Tool runtime:    %ls", DiagnosticDurationString(uptimeMs).c_str());
  report.Line(L"WM session:      %ls", DiagnosticDurationString(wmSessionMs).c_str());
  report.Line(
      L"Since last report: %ls%ls",
      DiagnosticDurationString(intervalMs).c_str(),
      g_runtime.lastReportTickMs ? L"" : L" (first report)");
  report.Line(L"PID: %lu", GetCurrentProcessId());
  report.Line(L"Privacy note: this report contains window titles and executable paths.");

  report.Section(L"Current runtime summary");
  report.Line(L"Management mode: %ls", IsAutomaticMode() ? L"Automatic" : L"Manual");
  report.Line(
      L"Workspaces=%zu records=%zu tiled=%zu suspended=%zu perWindowFloating=%zu ignored=%zu",
      workspaces.size(), totalRecords, tiledCount, suspendedCount, floatingCount,
      ignoredCount);
  report.Line(
      L"Pending lifecycle HWNDs=%zu pending workspace reinforcements=%zu duplicate-owned HWNDs=%zu",
      g_wm.lifecycleDirtyWindows.size(), g_wm.pendingDesktopArranges.size(),
      duplicateOwners);
  report.Line(
      L"Active conformance leases=%zu pending layout mutations=%zu runtime-maintenance timer=%d attempt=%u",
      CountActiveConformanceLeases(), g_pendingLayoutMutations.size(),
      g_wm.maintenanceTimer ? 1 : 0, g_wm.maintenanceAttempts);
  report.Line(
      L"Reverse ownership index: entries=%zu mismatches=%zu",
      ownershipIndex.size(), ownershipIndexMismatches);
  report.Line(
      L"WM sessions started=%llu settings reloads=%llu prior reports written=%llu prior report write failures=%llu",
      static_cast<unsigned long long>(counters.wmSessionStarts),
      static_cast<unsigned long long>(counters.settingsReloads),
      static_cast<unsigned long long>(counters.reportsWritten),
      static_cast<unsigned long long>(counters.reportWriteFailures));

  report.Section(L"Configuration snapshot");
  report.Line(L"Fallback layout: %ls", DiagnosticLayoutName(g_settings.defaultLayout));
  report.TextLine(L"Layout cycle: ", DiagnosticLayoutCycleString(g_settings.layoutCycle));
  report.Line(L"Workspace initialization rules: %zu",
              g_settings.workspaceInitializationRules.size());
  report.Line(L"Tray click: %ls; toasts: %ls",
              TrayUi::g_trayClickOpensMenu ? L"context menu" : L"switch layout",
              TrayUi::g_showToasts ? L"enabled" : L"disabled");
  report.Line(
      L"Workspace geometry (96-DPI logical px): gap=%ld insets=[%ld,%ld,%ld,%ld] floatingDefault=%ldx%ld",
      g_settings.gapDip, g_settings.insetsDip.left, g_settings.insetsDip.top,
      g_settings.insetsDip.right, g_settings.insetsDip.bottom,
      g_settings.floatingDefaultSizeDip.width,
      g_settings.floatingDefaultSizeDip.height);
  report.Line(L"Master percent: %ld", g_settings.masterPercent);
  report.Line(
      L"Adaptive Dwindle slots: %zu", g_settings.adaptiveDwindleSlots);
  report.Line(
      L"Adaptive Dwindle percentage: %ld (automatic envelope %.0f%%-%.0f%%)",
      g_settings.adaptiveDwindlePercentage,
      GetAdaptiveDwindleRatioEnvelope().minimum * 100.0,
      GetAdaptiveDwindleRatioEnvelope().maximum * 100.0);
  report.Line(L"Mouse move behavior: %ls", DiagnosticMouseMoveBehaviorName(g_settings.mouseMoveBehavior));
  report.Line(
      L"Automatic insertion position: %ls",
      g_settings.automaticNewWindowPosition ==
              AutomaticNewWindowPosition::AfterFocused
          ? L"After focused window"
          : L"Last slot");
  report.Line(L"Tiled conformance lease: %u ms", g_settings.conformanceLeaseMs);
  report.Line(
      L"Conformance repair interval: %u ms",
      g_settings.conformanceRepairIntervalMs);
  report.Line(L"Lifecycle settle delay: %u ms", g_settings.reconcileDelayMs);
  report.Line(L"Slow application discovery delay: %u ms",
              g_settings.slowApplicationDelayMs);
  report.Line(L"Window rules: %zu", g_settings.windowRules.size());
  report.Line(L"Initial window placement: ready=%d pending=%zu",
              g_wm.windowRoutingReady ? 1 : 0, g_wm.pendingWindowRoutes.size());
  report.TextLine(L"Diagnostic output setting: ", g_settings.diagnosticsOutputPath);
  report.TextLine(L"Diagnostic output resolved: ", ExpandDiagnosticPath(g_settings.diagnosticsOutputPath));

  report.Section(L"Platform / hook health");
  DumpPlatformHealth(report);

  report.Section(L"Performance and churn counters");
  DumpCpuUsage(report, uptimeMs, wmSessionMs, reportStartCpu);
  report.Line();
  DumpDerivedPerformanceIndicators(report, counters);
  report.Line();
  report.Line(
      L"Rates are normalized by tool runtime and by the interval since the previous successful report.");
  DumpCounters(report, counters, previous, uptimeMs, intervalMs);

  report.Section(L"Recent significant events (bounded in-memory tail)");
  report.Line(L"Newest events are at the bottom; at most %zu are retained.", kRecentEventLimit);
  DumpRecentEvents(report, now);

  report.Section(L"Recent lifecycle bursts (bounded in-memory tail)");
  report.Line(
      L"Newest bursts are at the bottom; at most %zu are retained.",
      kLifecycleBurstLimit);
  DumpLifecycleBursts(report, now);

  report.Section(L"Workspace / window state");

  size_t invariantFailures = 0;
  size_t deadRecords = 0;
  size_t pidMismatches = 0;
  size_t recordsWithPlacementFailures = 0;
  size_t staleMonitorWorkspaces = 0;
  size_t recordsOnStaleMonitorWorkspaces = 0;
  size_t currentDesktopRecordsOnStaleMonitorWorkspaces = 0;

  for (size_t workspaceIndex = 0; workspaceIndex < workspaces.size(); ++workspaceIndex) {
    const DesktopMonitorKey& key = workspaces[workspaceIndex].first;
    const Workspace& state = workspaces[workspaceIndex].second;
    const std::wstring desktopId = DiagnosticGuidString(key.desktopId);

    size_t workspaceTiled = 0, workspaceFloating = 0, workspaceSuspended = 0,
           workspaceIgnored = 0;
    for (const auto& recordKv : state.Records()) {
      switch (recordKv.second.state) {
        case ManageState::Tiled: ++workspaceTiled; break;
        case ManageState::Floating: ++workspaceFloating; break;
        case ManageState::Suspended: ++workspaceSuspended; break;
        case ManageState::Ignored: ++workspaceIgnored; break;
      }
    }

    const HMONITOR resolvedMonitor = key.ResolveMonitor();
    const bool pendingReinforcement = std::any_of(
        g_wm.pendingDesktopArranges.begin(), g_wm.pendingDesktopArranges.end(),
        [&](const PendingWorkspaceArrange& pending) {
          return resolvedMonitor && pending.monitor == resolvedMonitor &&
                 IsEqualGUID(pending.desktopId, key.desktopId);
        });

    const bool liveMonitor = resolvedMonitor != nullptr;
    if (!liveMonitor) {
      ++staleMonitorWorkspaces;
      recordsOnStaleMonitorWorkspaces += state.RecordCount();
      if (g_vd.lastKnownDesktopValid &&
          IsEqualGUID(key.desktopId, g_vd.lastKnownDesktop)) {
        currentDesktopRecordsOnStaleMonitorWorkspaces += state.RecordCount();
      }
    }

    std::vector<std::wstring> invariantErrors;
    const bool invariantsValid =
        state.Validate(key, L"diagnostic report", &invariantErrors, false);
    if (!invariantsValid) ++invariantFailures;

    report.Line();
    report.Line(
        L"---------------- Workspace %zu / %zu ----------------",
        workspaceIndex + 1, workspaces.size());
    report.Line(L"desktopId: %ls", desktopId.c_str());
    report.Line(
        L"currentDesktop: %ls",
        !g_vd.lastKnownDesktopValid
            ? L"unknown"
            : (IsEqualGUID(key.desktopId, g_vd.lastKnownDesktop) ? L"YES" : L"no"));
    report.Line(L"monitorId: %ls", key.monitor.deviceId.c_str());
    report.Line(L"monitor:   %p", reinterpret_cast<void*>(resolvedMonitor));

    MONITORINFOEXW monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (resolvedMonitor && GetMonitorInfoW(
            resolvedMonitor, reinterpret_cast<MONITORINFO*>(&monitorInfo))) {
      report.Line(L"device:    %ls", monitorInfo.szDevice);
      report.Line(L"monitorRect: %ls", DiagnosticRectString(monitorInfo.rcMonitor).c_str());
      report.Line(L"systemWork:  %ls", DiagnosticRectString(monitorInfo.rcWork).c_str());
      report.Line(L"monitorDpi:  %u", GetMonitorEffectiveDpi(resolvedMonitor));
      RECT configuredWork{};
      if (GetWorkspaceWorkArea(resolvedMonitor, &configuredWork)) {
        report.Line(L"configuredWork: %ls", DiagnosticRectString(configuredWork).c_str());
      }
    } else {
      report.Line(L"monitorInfo: <detached / currently unresolved>");
    }

    report.Line(
        L"layout=%ls (%d) masterRatio=%.9g",
        DiagnosticLayoutName(state.Layout()), static_cast<int>(state.Layout()),
        state.MasterRatio());
    report.Line(
        L"counts: active=%zu records=%zu tiled=%zu suspended=%zu floating=%zu ignored=%zu",
        state.ActiveCount(), state.Records().size(), workspaceTiled,
        workspaceSuspended, workspaceFloating, workspaceIgnored);
    report.Line(L"overflow: visibleTiled=%zu limit=%zu",
                state.VisibleWindowCount(),
                AutomaticWindowLimitForMonitor(key.monitor));
    report.Line(L"pendingReinforcement=%d invariants=%ls",
                pendingReinforcement ? 1 : 0,
                invariantsValid ? L"PASS" : L"FAIL");
    if (!invariantErrors.empty()) {
      report.Line(L"invariantErrors:");
      for (const std::wstring& error : invariantErrors) {
        report.Line(L"  - %ls", error.c_str());
      }
    }
    report.Line(
        L"retainedFocus=%p tiledAdmissionAnchor=%p",
        state.LastFocusedWindow(), state.LastFocusedTiledWindow());
    report.Line(L"stackWeights: %ls", DiagnosticWeightsString(state.StackWeights()).c_str());
    report.Line(L"gridWeights:  %ls", DiagnosticWeightsString(state.GridWeights()).c_str());
    report.Line(
        L"dwindleRatios: %ls",
        DiagnosticWeightsString(state.DwindleRatios()).c_str());

    struct LogicalRecordRef {
      size_t logicalIndex;
      HWND hwnd;
      const WindowRecord* record;
    };
    std::vector<LogicalRecordRef> logicalRecords;
    std::vector<std::pair<HWND, const WindowRecord*>> otherRecords;
    logicalRecords.reserve(state.Records().size());
    otherRecords.reserve(state.Records().size());

    for (const auto& recordKv : state.Records()) {
      const WindowRecord& record = recordKv.second;
      size_t logicalIndex = DiagnosticLogicalIndex(state, recordKv.first, record);
      if ((record.state == ManageState::Tiled || record.state == ManageState::Suspended) &&
          logicalIndex != static_cast<size_t>(-1)) {
        logicalRecords.push_back({logicalIndex, recordKv.first, &record});
      } else {
        otherRecords.push_back({recordKv.first, &record});
      }
    }

    std::sort(
        logicalRecords.begin(), logicalRecords.end(),
        [](const LogicalRecordRef& a, const LogicalRecordRef& b) {
          if (a.logicalIndex != b.logicalIndex) return a.logicalIndex < b.logicalIndex;
          return reinterpret_cast<uintptr_t>(a.hwnd) < reinterpret_cast<uintptr_t>(b.hwnd);
        });
    std::sort(
        otherRecords.begin(), otherRecords.end(),
        [](const auto& a, const auto& b) {
          if (a.second->state != b.second->state) {
            return static_cast<int>(a.second->state) < static_cast<int>(b.second->state);
          }
          return reinterpret_cast<uintptr_t>(a.first) < reinterpret_cast<uintptr_t>(b.first);
        });

    report.Line(L"  logicalTiledOrder: %zu", logicalRecords.size());
    if (logicalRecords.empty()) {
      report.Line(L"    (none)");
    } else {
      for (size_t i = 0; i < logicalRecords.size(); ++i) {
        DiagnosticRecordAudit audit = DiagnosticDumpWindowRecord(
            report, state, logicalRecords[i].hwnd, *logicalRecords[i].record,
            L"window", i);
        deadRecords += audit.deadHwnd ? 1 : 0;
        pidMismatches += audit.pidMismatch ? 1 : 0;
        recordsWithPlacementFailures += audit.hasPlacementFailure ? 1 : 0;
      }
    }

    report.Line(L"  nonLayoutRecords: %zu", otherRecords.size());
    if (otherRecords.empty()) {
      report.Line(L"    (none)");
    } else {
      for (size_t i = 0; i < otherRecords.size(); ++i) {
        DiagnosticRecordAudit audit = DiagnosticDumpWindowRecord(
            report, state, otherRecords[i].first, *otherRecords[i].second,
            L"record", i);
        deadRecords += audit.deadHwnd ? 1 : 0;
        pidMismatches += audit.pidMismatch ? 1 : 0;
        recordsWithPlacementFailures += audit.hasPlacementFailure ? 1 : 0;
      }
    }
  }

  report.Section(L"Pending work");
  report.Line(L"automaticOverflow: routingReady=%d limit=%zu createDesktop=%d otherMonitors=%d pending=%zu follow=%p",
              g_hooks.hideDestroy ? 1 : 0, g_settings.automaticWindowLimit,
              g_settings.overflowCreateDesktop ? 1 : 0, g_settings.overflowTryOtherMonitors ? 1 : 0,
              g_wm.pendingWindowOverflow.size(), g_wm.pendingOverflowSwitch.hwnd);
  if (g_wm.lifecycleDirtyWindows.empty()) {
    report.Line(L"pendingLifecycleWindows: none");
  } else {
    report.Line(L"pendingLifecycleWindows: %zu", g_wm.lifecycleDirtyWindows.size());
    for (size_t i = 0; i < g_wm.lifecycleDirtyWindows.size(); ++i) {
      report.Line(
          L"  [%zu] hwnd=%p", i,
          reinterpret_cast<void*>(g_wm.lifecycleDirtyWindows[i]));
    }
  }

  if (g_wm.pendingDesktopArranges.empty()) {
    report.Line(L"pendingWorkspaceReinforcements: none");
  } else {
    report.Line(
        L"pendingWorkspaceReinforcements: %zu",
        g_wm.pendingDesktopArranges.size());
    for (size_t i = 0; i < g_wm.pendingDesktopArranges.size(); ++i) {
      const std::wstring desktopId =
          DiagnosticGuidString(g_wm.pendingDesktopArranges[i].desktopId);
      report.Line(
          L"  [%zu] desktopId=%ls monitor=%p", i, desktopId.c_str(),
          reinterpret_cast<void*>(g_wm.pendingDesktopArranges[i].monitor));
    }
  }

  report.Section(L"Audit summary");
  report.Line(L"Workspace invariant failures: %zu", invariantFailures);
  report.Line(L"Duplicate-owned HWNDs:        %zu", duplicateOwners);
  report.Line(L"Ownership-index mismatches:   %zu", ownershipIndexMismatches);
  report.Line(L"Dead HWND records:            %zu", deadRecords);
  report.Line(L"Live PID mismatches:           %zu", pidMismatches);
  report.Line(L"Records with placement issue:  %zu", recordsWithPlacementFailures);

  const uint64_t lifecycleCategorySum =
      counters.lifecycleDestroy + counters.lifecycleMinimizeStart +
      counters.lifecycleMinimizeEnd + counters.lifecycleStateChange +
      counters.lifecycleLocationChange + counters.lifecycleHide +
      counters.lifecycleShow + counters.lifecycleCloaked +
      counters.lifecycleUncloaked;
  const uint64_t placementResultSum =
      counters.placementSuccess +
      counters.placementSuppressedByPhysicalState +
      counters.placementAdjusted + counters.placementAccessDenied +
      counters.placementRefused + counters.placementDead;
  const uint64_t placementPathSum =
      counters.placementNoOps + counters.placementPreflightStops +
      counters.setWindowPosCalls;
  const uint64_t constraintProbeResultSum =
      counters.constraintProbeSuccesses + counters.constraintProbeTimeouts +
      counters.constraintProbeFailures;
  const uint64_t vdInitResultSum =
      counters.vdApiInitSuccesses + counters.vdApiInitFailures;
  const uint64_t vdNotificationResultSum =
      counters.vdNotificationRegisterSuccesses +
      counters.vdNotificationRegisterFailures;
  const uint64_t priorReportResultSum =
      counters.reportsWritten + counters.reportWriteFailures;
  const uint64_t classifiedGestureSum =
      counters.userMoveGestures + counters.userResizeGestures +
      counters.userNoopGestures;

  const bool lifecycleAccountingOk =
      lifecycleCategorySum == counters.lifecycleEventsProcessed;
  const bool placementResultsOk =
      placementResultSum == counters.placementChecks;
  const bool placementPathsOk =
      placementPathSum == counters.placementChecks;
  const bool constraintProbeAccountingOk =
      constraintProbeResultSum == counters.constraintProbeAttempts;
  const bool vdInitAccountingOk =
      vdInitResultSum == counters.vdApiInitAttempts;
  const bool vdNotificationAccountingOk =
      vdNotificationResultSum == counters.vdNotificationRegisterAttempts;
  const bool reportAccountingOk =
      priorReportResultSum == g_runtime.lifetimeDiagnosticCost.attempts;
  const bool gestureAccountingOk =
      classifiedGestureSum <= counters.moveSizeEndMessages;

  report.Line();
  report.Line(
      L"Detached/unresolved monitor workspaces: %zu (records attached: %zu; current-desktop records: %zu)",
      staleMonitorWorkspaces, recordsOnStaleMonitorWorkspaces,
      currentDesktopRecordsOnStaleMonitorWorkspaces);

  report.Line();
  report.Line(L"Telemetry consistency:");
  report.Line(
      L"  lifecycle categories == lifecycle events: %ls (%llu == %llu)",
      lifecycleAccountingOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(lifecycleCategorySum),
      static_cast<unsigned long long>(counters.lifecycleEventsProcessed));
  report.Line(
      L"  placement result sum == placement checks: %ls (%llu == %llu)",
      placementResultsOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(placementResultSum),
      static_cast<unsigned long long>(counters.placementChecks));
  report.Line(
      L"  placement path sum == placement checks: %ls (%llu == %llu)",
      placementPathsOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(placementPathSum),
      static_cast<unsigned long long>(counters.placementChecks));
  report.Line(
      L"  constraint probe results == attempts: %ls (%llu == %llu)",
      constraintProbeAccountingOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(constraintProbeResultSum),
      static_cast<unsigned long long>(counters.constraintProbeAttempts));
  report.Line(
      L"  VD init successes + failures == attempts: %ls (%llu == %llu)",
      vdInitAccountingOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(vdInitResultSum),
      static_cast<unsigned long long>(counters.vdApiInitAttempts));
  report.Line(
      L"  VD notification successes + failures == attempts: %ls (%llu == %llu)",
      vdNotificationAccountingOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(vdNotificationResultSum),
      static_cast<unsigned long long>(counters.vdNotificationRegisterAttempts));
  report.Line(
      L"  prior report results == measured diagnostic attempts: %ls (%llu == %llu)",
      reportAccountingOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(priorReportResultSum),
      static_cast<unsigned long long>(
          g_runtime.lifetimeDiagnosticCost.attempts));
  report.Line(
      L"  classified gestures <= move/size end messages: %ls (%llu <= %llu)",
      gestureAccountingOk ? L"PASS" : L"FAIL",
      static_cast<unsigned long long>(classifiedGestureSum),
      static_cast<unsigned long long>(counters.moveSizeEndMessages));

  const bool allWinEventHooks =
      g_hooks.foreground && g_hooks.locationChange && g_hooks.moveSize &&
      g_hooks.minimize && g_hooks.hideDestroy && g_hooks.cloak && g_hooks.state;
  DWORD auditShellPid = 0;
  HWND auditShell = GetShellWindow();
  if (auditShell && IsWindow(auditShell)) {
    GetWindowThreadProcessId(auditShell, &auditShellPid);
  }
  const bool shellBindingHealthy =
      !g_vd.initialized ||
      (auditShellPid != 0 && g_vd.shellPid == auditShellPid);
  const bool vdCoreHealthy = g_vd.initialized && g_vd.abiResolved;
  const bool vdNotificationsHealthy =
      g_vd.abi.notification.methodCount <= 0 || g_vd.notificationsRegistered;
  const bool currentDesktopHealthy =
      !g_vd.initialized || g_vd.lastKnownDesktopValid;

  const bool structuralHealthy =
      invariantFailures == 0 && duplicateOwners == 0 &&
      ownershipIndexMismatches == 0;
  const bool runtimeWindowHealthy =
      deadRecords == 0 && pidMismatches == 0 &&
      recordsWithPlacementFailures == 0 &&
      currentDesktopRecordsOnStaleMonitorWorkspaces == 0 && allWinEventHooks &&
      shellBindingHealthy && vdCoreHealthy && vdNotificationsHealthy &&
      currentDesktopHealthy;
  const bool telemetryHealthy =
      lifecycleAccountingOk && placementResultsOk && placementPathsOk &&
      constraintProbeAccountingOk && vdInitAccountingOk &&
      vdNotificationAccountingOk && reportAccountingOk && gestureAccountingOk;
  const bool overallHealthy =
      structuralHealthy && runtimeWindowHealthy && telemetryHealthy;

  report.Line();
  report.Line(
      L"Model structural audit:       %ls",
      structuralHealthy ? L"PASS" : L"REVIEW NEEDED");
  report.Line(
      L"Runtime / window health:      %ls",
      runtimeWindowHealthy ? L"PASS" : L"REVIEW NEEDED");
  report.Line(
      L"Telemetry accounting audit:   %ls",
      telemetryHealthy ? L"PASS" : L"REVIEW NEEDED");
  report.Line(
      L"Overall diagnostic status:    %ls",
      overallHealthy ? L"PASS" : L"REVIEW NEEDED");

  const ULONGLONG buildEndTickMs = GetTickCount64();
  const DiagnosticCpuSnapshot buildEndCpu = CaptureDiagnosticCpuSnapshot();
  const ULONGLONG currentBuildWallMs =
      buildEndTickMs >= reportStartTickMs
          ? buildEndTickMs - reportStartTickMs
          : 0;
  const uint64_t currentBuildProcessCpu100ns =
      reportStartCpu.processAvailable && buildEndCpu.processAvailable
          ? DiagnosticCpuDelta100ns(
                buildEndCpu.ProcessTotal100ns(),
                reportStartCpu.ProcessTotal100ns())
          : 0;
  const uint64_t currentBuildWmCpu100ns =
      reportStartCpu.wmAvailable && buildEndCpu.wmAvailable
          ? DiagnosticCpuDelta100ns(
                buildEndCpu.WmTotal100ns(), reportStartCpu.WmTotal100ns())
          : 0;

  report.Section(L"Diagnostic reporting self-cost");
  report.Line(
      L"Prior diagnostic attempts: %llu lifetime, %llu current WM session",
      static_cast<unsigned long long>(
          g_runtime.lifetimeDiagnosticCost.attempts),
      static_cast<unsigned long long>(
          g_runtime.wmSessionDiagnosticCost.attempts));
  report.Line(
      L"Prior completed diagnostic cost (includes file writes): wall=%ls processCPU=%ls wmCPU=%ls",
      DiagnosticDurationString(
          g_runtime.lifetimeDiagnosticCost.wallMs).c_str(),
      DiagnosticDurationString(
          Diagnostic100nsToMilliseconds(
              g_runtime.lifetimeDiagnosticCost.processCpu100ns)).c_str(),
      DiagnosticDurationString(
          Diagnostic100nsToMilliseconds(
              g_runtime.lifetimeDiagnosticCost.wmCpu100ns)).c_str());
  report.Line(
      L"Current report construction so far (file write excluded): wall=%ls processCPU=%ls wmCPU=%ls",
      DiagnosticDurationString(currentBuildWallMs).c_str(),
      DiagnosticDurationString(
          Diagnostic100nsToMilliseconds(currentBuildProcessCpu100ns)).c_str(),
      DiagnosticDurationString(
          Diagnostic100nsToMilliseconds(currentBuildWmCpu100ns)).c_str());
  report.Line(
      L"Note: the next report's prior-cost totals include this report's final file-write cost.");

  report.Line();
  report.Line(L"========================== END REPORT ==========================");

  std::wstring outputPath;
  DWORD writeError = ERROR_SUCCESS;
  if (!WriteReportToConfiguredDirectory(report.Text(), &outputPath, &writeError)) {
    ++g_runtime.counters.reportWriteFailures;
    Wh_Log(
        L"Failed to write diagnostic report to '%ls' (error=%lu)",
        g_settings.diagnosticsOutputPath.c_str(), writeError);
    RecordEvent(L"diagnostic report write failed error=%lu", writeError);
    AccumulateDiagnosticSelfCost(reportStartTickMs, reportStartCpu);
    return;
  }

  ++g_runtime.counters.reportsWritten;
  ++g_runtime.reportSequence;
  g_runtime.lastReportCounters = counters;
  // Account for the successful report in the baseline so the next delta doesn't
  // misleadingly attribute this report write to unrelated runtime activity.
  g_runtime.lastReportCounters.reportsWritten = g_runtime.counters.reportsWritten;
  RecordEvent(L"diagnostic report written: %ls", outputPath.c_str());
  Wh_Log(L"Diagnostic report written: %ls", outputPath.c_str());
  AccumulateDiagnosticSelfCost(reportStartTickMs, reportStartCpu);
  // The interval baseline is the end of a successful diagnostic operation, so
  // report-generation time is not charged to the next "since dump" period.
  g_runtime.lastReportTickMs = GetTickCount64();
}

}  // namespace Diagnostics

//=============================================================================
// Settings parsing + serialized WM thread
//=============================================================================

UINT ParseHotkeyKey(PCWSTR str) {
  if (!str || !str[0]) return 0;

  static const std::pair<PCWSTR, UINT> kNamedKeys[] = {
      {L"Space", VK_SPACE},
      {L"Tab", VK_TAB},
      {L"Enter", VK_RETURN},
      {L"Return", VK_RETURN},
      {L"Esc", VK_ESCAPE},
      {L"Escape", VK_ESCAPE},
      {L"Backspace", VK_BACK},
      {L"Insert", VK_INSERT},
      {L"Ins", VK_INSERT},
      {L"Delete", VK_DELETE},
      {L"Del", VK_DELETE},
      {L"Home", VK_HOME},
      {L"End", VK_END},
      {L"PageUp", VK_PRIOR},
      {L"PgUp", VK_PRIOR},
      {L"PageDown", VK_NEXT},
      {L"PgDn", VK_NEXT},
      {L"Left", VK_LEFT},
      {L"Right", VK_RIGHT},
      {L"Up", VK_UP},
      {L"Down", VK_DOWN},
  };
  for (const auto& namedKey : kNamedKeys) {
    if (_wcsicmp(str, namedKey.first) == 0) return namedKey.second;
  }

  if ((str[0] == L'F' || str[0] == L'f') && str[1]) {
    wchar_t* end = nullptr;
    long functionKey = std::wcstol(str + 1, &end, 10);
    if (*end == L'\0' && functionKey >= 1 && functionKey <= 12) {
      return VK_F1 + static_cast<UINT>(functionKey - 1);
    }
  }

  if (str[1] != L'\0') return 0;
  wchar_t c = str[0];

  if (c >= L'A' && c <= L'Z') return c;
  if (c >= L'a' && c <= L'z') return c - L'a' + L'A';
  if (c >= L'0' && c <= L'9') return c;

  if (c == L'!') return '1';
  if (c == L'@') return '2';
  if (c == L'#') return '3';
  if (c == L'$') return '4';
  if (c == L'%') return '5';
  if (c == L'^') return '6';
  if (c == L'&') return '7';
  if (c == L'*') return '8';
  if (c == L'(') return '9';
  if (c == L')') return '0';

  if (c == L'`' || c == L'~') return VK_OEM_3;
  if (c == L'-' || c == L'_') return VK_OEM_MINUS;
  if (c == L'=' || c == L'+') return VK_OEM_PLUS;
  if (c == L'[' || c == L'{') return VK_OEM_4;
  if (c == L']' || c == L'}') return VK_OEM_6;
  if (c == L'\\' || c == L'|') return VK_OEM_5;
  if (c == L';' || c == L':') return VK_OEM_1;
  if (c == L'\'' || c == L'"') return VK_OEM_7;
  if (c == L',' || c == L'<') return VK_OEM_COMMA;
  if (c == L'.' || c == L'>') return VK_OEM_PERIOD;
  if (c == L'/' || c == L'?') return VK_OEM_2;
  if (c == L' ') return VK_SPACE;

  return 0;
}

static bool ParseWorkspaceInsetsSetting(PCWSTR text, WorkspaceInsets* outInsets) {
  if (!text || !outInsets) return false;

  LONG values[4]{};
  const wchar_t* cursor = text;

  for (size_t i = 0; i < 4; ++i) {
    while (*cursor && std::iswspace(*cursor)) ++cursor;

    wchar_t* end = nullptr;
    long value = std::wcstol(cursor, &end, 10);
    if (end == cursor || value < 0 || value > 500) return false;
    values[i] = static_cast<LONG>(value);
    cursor = end;

    while (*cursor && std::iswspace(*cursor)) ++cursor;
    if (i < 3) {
      if (*cursor != L',') return false;
      ++cursor;
    }
  }

  while (*cursor && std::iswspace(*cursor)) ++cursor;
  if (*cursor != L'\0') return false;

  *outInsets = {values[0], values[1], values[2], values[3]};
  return true;
}

static bool ParseFloatingDefaultSizeSetting(
    PCWSTR text, FloatingDefaultSize* outSize) {
  if (!text || !outSize) return false;

  LONG values[2]{};
  const wchar_t* cursor = text;
  for (size_t i = 0; i < 2; ++i) {
    while (*cursor && std::iswspace(*cursor)) ++cursor;
    wchar_t* end = nullptr;
    long value = std::wcstol(cursor, &end, 10);
    if (end == cursor || value < 100 || value > 4000) return false;
    values[i] = static_cast<LONG>(value);
    cursor = end;
    while (*cursor && std::iswspace(*cursor)) ++cursor;
    if (i == 0) {
      if (*cursor != L',') return false;
      ++cursor;
    }
  }
  while (*cursor && std::iswspace(*cursor)) ++cursor;
  if (*cursor != L'\0') return false;
  *outSize = {values[0], values[1]};
  return true;
}

void LoadSettings() {
  using WindhawkUtils::StringSetting;

  g_settings.tilingModifiers =
      ReadModifierSetting(L"hotkeys.TilingModifier", MOD_ALT);

  auto insets = StringSetting::make(L"workspace.WorkspaceInsets");
  WorkspaceInsets parsedInsets{};
  if (ParseWorkspaceInsetsSetting(insets.get(), &parsedInsets)) {
    g_settings.insetsDip = parsedInsets;
  } else {
    g_settings.insetsDip = {};
    Wh_Log(L"Invalid WorkspaceInsets setting; using 6, 6, 6, 6");
  }
  g_settings.gapDip = Wh_GetIntSetting(L"workspace.TileGap");
  if (g_settings.gapDip < 0 || g_settings.gapDip > 100) g_settings.gapDip = 6;

  auto floatingSize =
      StringSetting::make(L"windowBehavior.FloatingDefaultSize");
  FloatingDefaultSize parsedFloatingSize{};
  if (ParseFloatingDefaultSizeSetting(floatingSize.get(), &parsedFloatingSize)) {
    g_settings.floatingDefaultSizeDip = parsedFloatingSize;
  } else {
    g_settings.floatingDefaultSizeDip = {};
    Wh_Log(L"Invalid FloatingDefaultSize setting; using 960, 640");
  }
  g_settings.masterPercent = Wh_GetIntSetting(L"workspace.MasterPercent");
  if (g_settings.masterPercent < 1 || g_settings.masterPercent > 99) g_settings.masterPercent = 50;
  g_settings.reverseDwindle =
      Wh_GetIntSetting(L"workspace.ReverseDwindle") != 0;
  const int adaptiveDwindleSlots =
      Wh_GetIntSetting(L"workspace.AdaptiveDwindleSlots");
  g_settings.adaptiveDwindleSlots = static_cast<size_t>(
      std::clamp(adaptiveDwindleSlots, 0, 64));
  const int adaptiveDwindlePercentage =
      Wh_GetIntSetting(L"workspace.AdaptiveDwindlePercentage");
  g_settings.adaptiveDwindlePercentage =
      std::clamp(adaptiveDwindlePercentage, 0, 40);

  auto layout = StringSetting::make(L"workspace.DefaultLayout");
  g_settings.defaultLayout = ParseLayoutSetting(layout.get());

  g_settings.layoutCycle.clear();
  for (int i = 0; i < 32; ++i) {
    auto cycleEntry =
        StringSetting::make(L"workspace.LayoutCycle[%d]", i);
    if (!*cycleEntry.get()) break;

    TileLayout cycleLayout{};
    if (!TryParseLayoutSetting(cycleEntry.get(), &cycleLayout)) {
      Wh_Log(L"Ignoring invalid LayoutCycle[%d] entry: %s", i, cycleEntry.get());
      continue;
    }
    if (std::find(g_settings.layoutCycle.begin(), g_settings.layoutCycle.end(),
                  cycleLayout) == g_settings.layoutCycle.end()) {
      g_settings.layoutCycle.push_back(cycleLayout);
    }
  }
  if (g_settings.layoutCycle.empty()) {
    g_settings.layoutCycle = MakeBuiltInLayoutCycle();
    Wh_Log(L"LayoutCycle is empty or invalid; using the built-in layout sequence");
  }

  g_settings.automaticWindowLimit = static_cast<size_t>(
      std::max(0, Wh_GetIntSetting(L"workspace.Overflow.WindowLimit")));
  auto overflowAction = StringSetting::make(L"workspace.Overflow.Action");
  g_settings.overflowCreateDesktop =
      _wcsicmp(overflowAction.get(), L"new_desktop") == 0;
  g_settings.overflowTryOtherMonitors =
      Wh_GetIntSetting(L"workspace.Overflow.TryOtherMonitors") != 0;
  g_settings.monitorWindowLimits.clear();
  for (int i = 0;; ++i) {
    auto monitor = StringSetting::make(L"workspace.Overflow.MonitorLimits[%d].Monitor", i);
    auto monitorId = StringSetting::make(L"workspace.Overflow.MonitorLimits[%d].MonitorId", i);
    const int limit = Wh_GetIntSetting(L"workspace.Overflow.MonitorLimits[%d].Limit", i);
    if (!*monitor.get() && !*monitorId.get() && limit == 0) break;
    // Disabled rows do not terminate the list or inspect their hidden fields.
    if (!*monitor.get() || _wcsicmp(monitor.get(), L"disabled") == 0) continue;
    if (limit < 0) {
      Wh_Log(L"Ignoring negative per-monitor limit at rule %d", i + 1);
      continue;
    }
    SettingsState::MonitorWindowLimit rule;
    if (_wcsicmp(monitor.get(), L"primary") == 0) rule.monitor = RuleMonitor::Primary;
    else if (_wcsicmp(monitor.get(), L"non_primary") == 0) rule.monitor = RuleMonitor::NonPrimary;
    else if (_wcsicmp(monitor.get(), L"specific") != 0) {
      Wh_Log(L"Ignoring invalid monitor selector at limit rule %d", i + 1);
      continue;
    }
    if (rule.monitor == RuleMonitor::Specific) {
      rule.monitorId = monitorId.get();
      const size_t first = rule.monitorId.find_first_not_of(L" \t\r\n");
      if (first == std::wstring::npos) continue;
      rule.monitorId = rule.monitorId.substr(first, rule.monitorId.find_last_not_of(L" \t\r\n") - first + 1);
      NormalizeMonitorIdentityToken(&rule.monitorId);
      if (rule.monitorId.rfind(L"IFACE:", 0) != 0 && rule.monitorId.rfind(L"GDI:", 0) != 0) {
        Wh_Log(L"Ignoring invalid monitor ID at limit rule %d", i + 1);
        continue;
      }
    }
    rule.limit = static_cast<size_t>(limit);
    g_settings.monitorWindowLimits.push_back(std::move(rule));
  }

  g_settings.workspaceInitializationRules.clear();
  for (int i = 0;; ++i) {
    auto ruleLayout =
        StringSetting::make(L"workspace.InitializationRules[%d].Layout", i);
    auto desktopName = StringSetting::make(
        L"workspace.InitializationRules[%d].DesktopNameContains", i);
    auto monitor =
        StringSetting::make(L"workspace.InitializationRules[%d].Monitor", i);
    auto monitorId =
        StringSetting::make(L"workspace.InitializationRules[%d].MonitorId", i);
    const int desktopNumber = Wh_GetIntSetting(
        L"workspace.InitializationRules[%d].DesktopNumber", i);
    // Added rows can omit unchanged fields, including Monitor. Read the whole
    // row before detecting the end, as blank Layout only disables that row.
    if (!*ruleLayout.get() && !*desktopName.get() && !*monitor.get() &&
        !*monitorId.get() && desktopNumber == 0) {
      break;
    }
    if (!*ruleLayout.get()) continue;

    WorkspaceInitializationRule rule;
    rule.settingsIndex = i + 1;
    rule.desktopNumber = desktopNumber;
    if (rule.desktopNumber < 0 ||
        !TryParseLayoutSetting(ruleLayout.get(), &rule.layout)) {
      Wh_Log(L"Ignoring invalid workspace initialization rule %d", i + 1);
      continue;
    }
    if (_wcsicmp(monitor.get(), L"primary") == 0) {
      rule.monitor = RuleMonitor::Primary;
    } else if (_wcsicmp(monitor.get(), L"non_primary") == 0) {
      rule.monitor = RuleMonitor::NonPrimary;
    } else if (_wcsicmp(monitor.get(), L"specific") == 0) {
      rule.monitor = RuleMonitor::Specific;
    } else if (*monitor.get() && _wcsicmp(monitor.get(), L"any") != 0) {
      Wh_Log(L"Ignoring invalid monitor selector at workspace rule %d", i + 1);
      continue;
    }
    auto trimmed = [](PCWSTR text) {
      std::wstring value = text;
      const size_t first = value.find_first_not_of(L" \t\r\n");
      return first == std::wstring::npos ? std::wstring{}
          : value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1);
    };
    rule.desktopNameContains = trimmed(desktopName.get());
    std::transform(
        rule.desktopNameContains.begin(), rule.desktopNameContains.end(),
        rule.desktopNameContains.begin(),
        [](wchar_t ch) { return std::towlower(ch); });
    if (rule.monitor == RuleMonitor::Specific) {
      rule.monitorId = trimmed(monitorId.get());
      NormalizeMonitorIdentityToken(&rule.monitorId);
      if (rule.monitorId.empty() || (rule.monitorId.rfind(L"IFACE:", 0) != 0 &&
                                   rule.monitorId.rfind(L"GDI:", 0) != 0)) {
        Wh_Log(L"Ignoring invalid monitor ID at workspace rule %d", i + 1);
        continue;
      }
    }
    if (rule.desktopNameContains.empty() && rule.desktopNumber == 0 &&
        rule.monitor == RuleMonitor::Any && rule.monitorId.empty()) {
      continue;
    }
    g_settings.workspaceInitializationRules.push_back(std::move(rule));
  }

  auto mouseBehavior =
      StringSetting::make(L"windowBehavior.MouseMoveBehavior");
  g_settings.mouseMoveBehavior =
      _wcsicmp(mouseBehavior.get(), L"swap") == 0
          ? MouseMoveBehavior::Swap
          : MouseMoveBehavior::Float;

  auto automaticNewWindowPosition =
      StringSetting::make(L"general.AutomaticNewWindowPosition");
  g_settings.automaticNewWindowPosition =
      _wcsicmp(automaticNewWindowPosition.get(), L"after_focused") == 0
          ? AutomaticNewWindowPosition::AfterFocused
          : AutomaticNewWindowPosition::LastSlot;

  auto managementMode =
      StringSetting::make(L"general.DefaultWindowManagementMode");
  g_wm.managementMode =
      _wcsicmp(managementMode.get(), L"manual") == 0
          ? ManagementMode::Manual
          : ManagementMode::Automatic;

  int conformanceLease = Wh_GetIntSetting(L"advanced.ConformanceLeaseMs");
  if (conformanceLease < 0 || conformanceLease > 10000) conformanceLease = 3000;
  g_settings.conformanceLeaseMs = static_cast<UINT>(conformanceLease);

  int conformanceRepairInterval =
      Wh_GetIntSetting(L"advanced.ConformanceRepairIntervalMs");
  if (conformanceRepairInterval < 20 || conformanceRepairInterval > 2000) {
    conformanceRepairInterval = 75;
  }
  g_settings.conformanceRepairIntervalMs =
      static_cast<UINT>(conformanceRepairInterval);

  int reconcileDelay = Wh_GetIntSetting(L"advanced.ReconcileDelayMs");
  if (reconcileDelay < 20 || reconcileDelay > 2000) reconcileDelay = 50;
  g_settings.reconcileDelayMs = static_cast<UINT>(reconcileDelay);

  int slowApplicationDelay =
      Wh_GetIntSetting(L"advanced.SlowApplicationDelayMs");
  if (slowApplicationDelay < 0 || slowApplicationDelay > 2000) {
    slowApplicationDelay = 250;
  }
  g_settings.slowApplicationDelayMs =
      static_cast<UINT>(slowApplicationDelay);

  auto parseWindowRuleList = [](const wchar_t* text) {
    std::vector<std::wstring> values;
    std::wstring value;
    for (const wchar_t* cursor = text; ; ++cursor) {
      if (*cursor == L'\\' && cursor[1] == L',') {
        value.push_back(L',');
        ++cursor;
        continue;
      }
      if (*cursor != L',' && *cursor != L'\0') {
        value.push_back(*cursor);
        continue;
      }

      size_t first = 0;
      while (first < value.size() && std::iswspace(value[first])) ++first;
      size_t last = value.size();
      while (last > first && std::iswspace(value[last - 1])) --last;
      if (first < last) {
        values.emplace_back(value.substr(first, last - first));
      }

      value.clear();
      if (*cursor == L'\0') break;
    }
    return values;
  };

  g_settings.windowRules.clear();
  for (int i = 0;; ++i) {
    auto process =
        StringSetting::make(L"windowRules.Rules[%d].Process", i);
    auto className =
        StringSetting::make(L"windowRules.Rules[%d].Class", i);
    auto titleContains =
        StringSetting::make(L"windowRules.Rules[%d].TitleContains", i);
    auto treatment =
        StringSetting::make(L"windowRules.Rules[%d].Treatment", i);
    auto size = StringSetting::make(L"windowRules.Rules[%d].Size", i);
    auto monitor = StringSetting::make(L"windowRules.Rules[%d].Monitor", i);
    auto monitorId = StringSetting::make(L"windowRules.Rules[%d].MonitorId", i);
    auto desktopName = StringSetting::make(L"windowRules.Rules[%d].DesktopNameContains", i);
    const int desktopNumber = Wh_GetIntSetting(L"windowRules.Rules[%d].DesktopNumber", i);
    if (!*process.get() && !*className.get() && !*titleContains.get()) {
      Wh_Log(
          L"Window rule %d is blank; later rules will not be loaded",
          i + 1);
      break;
    }

    WindowRule rule;
    if (_wcsicmp(treatment.get(), L"default_to_floating") == 0) {
      rule.treatment = WindowRuleTreatment::DefaultToFloating;
    } else if (_wcsicmp(treatment.get(), L"always_floating") == 0) {
      rule.treatment = WindowRuleTreatment::AlwaysFloating;
    } else if (_wcsicmp(treatment.get(), L"trace_to_owner") == 0) {
      rule.treatment = WindowRuleTreatment::TraceToOwner;
    } else if (_wcsicmp(treatment.get(), L"open_on_monitor") == 0) {
      rule.treatment = WindowRuleTreatment::InitialPlacement;
      auto trimmed = [](PCWSTR text) {
        std::wstring value = text;
        const size_t first = value.find_first_not_of(L" \t\r\n");
        return first == std::wstring::npos ? std::wstring{}
            : value.substr(first, value.find_last_not_of(L" \t\r\n") - first + 1);
      };
      if (_wcsicmp(monitor.get(), L"primary") == 0) rule.monitor = RuleMonitor::Primary;
      else if (_wcsicmp(monitor.get(), L"non_primary") == 0) rule.monitor = RuleMonitor::NonPrimary;
      else if (_wcsicmp(monitor.get(), L"specific") == 0) rule.monitor = RuleMonitor::Specific;
      else if (*monitor.get() && _wcsicmp(monitor.get(), L"any") != 0) {
        Wh_Log(L"Ignoring invalid monitor selector at window rule %d", i + 1);
        continue;
      }
      if (rule.monitor == RuleMonitor::Specific) {
        rule.monitorId = trimmed(monitorId.get());
        NormalizeMonitorIdentityToken(&rule.monitorId);
        if (rule.monitorId.empty() || (rule.monitorId.rfind(L"IFACE:", 0) != 0 &&
                                     rule.monitorId.rfind(L"GDI:", 0) != 0)) {
          Wh_Log(L"Ignoring invalid monitor target at window rule %d", i + 1);
          continue;
        }
      }
      rule.desktopNumber = desktopNumber;
      rule.desktopNameContains = trimmed(desktopName.get());
      if (desktopNumber < 0) {
        Wh_Log(L"Ignoring invalid desktop number at window rule %d", i + 1);
        continue;
      }
      rule.followWindow = Wh_GetIntSetting(L"windowRules.Rules[%d].FollowWindow", i) != 0;
      if (rule.monitor == RuleMonitor::Any && !desktopNumber && rule.desktopNameContains.empty()) continue;
    } else if (_wcsicmp(
                   treatment.get(), L"preserve_size_when_centering") == 0) {
      rule.treatment = WindowRuleTreatment::FloatingPlacementOverride;
      rule.preserveFloatingSize = true;
    } else if (_wcsicmp(
                   treatment.get(), L"override_size_when_centering") == 0) {
      rule.treatment = WindowRuleTreatment::FloatingPlacementOverride;
      if (_wcsicmp(size.get(), L"Workspace") == 0) {
        rule.useWorkspaceSize = true;
      } else if (!ParseFloatingDefaultSizeSetting(
                     size.get(), &rule.floatingSizeDip)) {
        Wh_Log(
            L"Ignoring invalid centered size override at rule %d: %s",
            i + 1, size.get());
        continue;
      }
      rule.preserveFloatingSize = false;
    } else {
      rule.treatment = WindowRuleTreatment::Exclude;
    }
    rule.processNames = parseWindowRuleList(process.get());
    rule.classNames = parseWindowRuleList(className.get());
    rule.titleFragments = parseWindowRuleList(titleContains.get());
    if (rule.processNames.empty() && rule.classNames.empty() &&
        rule.titleFragments.empty()) {
      Wh_Log(L"Ignoring window rule %d with no usable match values", i + 1);
      continue;
    }
    g_settings.windowRules.push_back(std::move(rule));
  }

  g_settings.tileKey =
      ReadHotkeySetting<UINT>(L"hotkeys.TileKey", ParseHotkeyKey);
  g_settings.layoutKey =
      ReadHotkeySetting<UINT>(L"hotkeys.LayoutKey", ParseHotkeyKey);
  g_settings.swapMasterKey =
      ReadHotkeySetting<UINT>(L"hotkeys.SwapMasterKey", ParseHotkeyKey);
  g_settings.promoteWindowKey =
      ReadHotkeySetting<UINT>(L"hotkeys.PromoteWindowKey", ParseHotkeyKey);
  g_settings.demoteWindowKey =
      ReadHotkeySetting<UINT>(L"hotkeys.DemoteWindowKey", ParseHotkeyKey);
  g_settings.managementModeToggleKey =
      ReadHotkeySetting<UINT>(L"hotkeys.ManagementModeToggleKey", ParseHotkeyKey);
  g_settings.floatFocusedKey =
      ReadHotkeySetting<UINT>(L"hotkeys.FloatFocusedKey", ParseHotkeyKey);
  g_settings.diagnosticDumpKey =
      ReadHotkeySetting<UINT>(L"hotkeys.DiagnosticDumpKey", ParseHotkeyKey);

  auto diagnosticsPath =
      StringSetting::make(L"diagnostics.DiagnosticsOutputPath");
  if (*diagnosticsPath.get()) {
    g_settings.diagnosticsOutputPath = diagnosticsPath.get();
  } else {
    g_settings.diagnosticsOutputPath =
        L"%USERPROFILE%\\Documents\\MultiWMDiagnostics";
  }

  TrayUi::LoadSettings();
}

static void RegisterConfiguredHotkeys() {
  auto registerOne = [](
      int id, UINT modifiers, UINT key, const wchar_t* name,
      bool allowRepeat = false) {
    if (!key) return;
    const UINT registrationModifiers =
        allowRepeat ? modifiers : modifiers | MOD_NOREPEAT;
    if (!RegisterHotKey(nullptr, id, registrationModifiers, key)) {
      Wh_Log(L"Failed to register %s hotkey (id=%d error=%lu)",
             name, id, GetLastError());
    }
  };

  registerOne(HK_TILE, g_settings.tilingModifiers, g_settings.tileKey, L"Tile");
  registerOne(HK_SWAP_MASTER, g_settings.tilingModifiers,
              g_settings.swapMasterKey, L"Swap master");
  registerOne(HK_PROMOTE_WINDOW, g_settings.tilingModifiers,
              g_settings.promoteWindowKey, L"Promote window", true);
  registerOne(HK_DEMOTE_WINDOW, g_settings.tilingModifiers,
              g_settings.demoteWindowKey, L"Demote window", true);
  registerOne(HK_MANAGEMENT_MODE_TOGGLE, g_settings.tilingModifiers,
              g_settings.managementModeToggleKey, L"Management mode");
  registerOne(HK_FLOAT_FOCUSED, g_settings.tilingModifiers,
              g_settings.floatFocusedKey, L"Float focused");
  registerOne(HK_LAYOUT, g_settings.tilingModifiers,
              g_settings.layoutKey, L"Layout cycle");
  registerOne(HK_DIAGNOSTIC_DUMP, g_settings.tilingModifiers,
              g_settings.diagnosticDumpKey, L"Diagnostic dump");
}

static void UnregisterConfiguredHotkeys() {
  UnregisterHotKey(nullptr, HK_TILE);
  UnregisterHotKey(nullptr, HK_SWAP_MASTER);
  UnregisterHotKey(nullptr, HK_PROMOTE_WINDOW);
  UnregisterHotKey(nullptr, HK_DEMOTE_WINDOW);
  UnregisterHotKey(nullptr, HK_MANAGEMENT_MODE_TOGGLE);
  UnregisterHotKey(nullptr, HK_FLOAT_FOCUSED);
  UnregisterHotKey(nullptr, HK_LAYOUT);
  UnregisterHotKey(nullptr, HK_DIAGNOSTIC_DUMP);
}

static void RememberManagedForeground(HWND hwnd) {
  AssertWmThread(L"RememberManagedForeground");
  if (!hwnd) return;

  const std::vector<DesktopMonitorKey> owners = g_workspaces.OwnersOf(hwnd);
  for (const auto& key : owners) {
    Workspace workspace;
    if (!g_workspaces.Load(key, &workspace)) continue;
    if (!workspace.RememberFocusedWindow(hwnd)) continue;
    g_workspaces.Save(key, workspace);
  }
}

static void InitializeCurrentDesktopWorkspaces() {
  std::vector<HMONITOR> monitors;
  Reconcile::EnumerateCurrentMonitors(monitors);
  for (HMONITOR monitor : monitors) {
    Reconcile::DiscoverCurrentWorkspaceOnMonitor(monitor);
  }
  RememberManagedForeground(GetForegroundWindow());

  Wh_Log(
      L"Window management mode at startup: %s (%zu connected monitor%s)",
      IsManualMode() ? L"Manual" : L"Automatic", monitors.size(),
      monitors.size() == 1 ? L"" : L"s");
}

static void SetWindowManagementMode(ManagementMode mode) {
  if (g_wm.managementMode == mode) return;
  g_wm.managementMode = mode;
  const bool automatic = IsAutomaticMode();
  if (!automatic) {
    g_wm.pendingWindowOverflow.clear();
    g_wm.pendingOverflowSwitch = {};
    if (g_wm.overflowTimer) KillTimer(nullptr, g_wm.overflowTimer);
    g_wm.overflowTimer = 0;
  }
  Wh_Log(L"Window management mode: %s", automatic ? L"Automatic" : L"Manual");
  Diagnostics::RecordEvent(
      L"management mode changed to %ls", automatic ? L"Automatic" : L"Manual");
  TrayUi::ShowManagementModeFlyout(automatic);

  // Entering Automatic mode runs the normal discovery path for every monitor.
  // Known Floating records remain managed-but-excluded and are never re-adopted.
  if (automatic) {
    std::vector<HMONITOR> monitors;
    Reconcile::EnumerateCurrentMonitors(monitors);
    for (HMONITOR monitor : monitors) {
      Reconcile::DiscoverCurrentWorkspaceOnMonitor(monitor);
    }
    RememberManagedForeground(GetForegroundWindow());
    Reconcile::ScheduleLifecycleReconcile(nullptr);
  }
}

static void ToggleWindowManagementMode() {
  SetWindowManagementMode(
      IsAutomaticMode() ? ManagementMode::Manual : ManagementMode::Automatic);
}

static void HandleMoveSizeEndMessage(HWND hwnd) {
  ++Diagnostics::g_runtime.counters.moveSizeEndMessages;
  // MOVESIZEEND is an authoritative monitor-transition boundary. Repair monitor
  // ownership before interpreting final geometry, without querying desktop
  // ownership for unrelated lifecycle noise.
  std::vector<DesktopMonitorKey> changedKeys;
  Reconcile::ReconcileKnownWindows(Reconcile::ReconcileScope::Monitor, &changedKeys, hwnd);
  Commands::ApplyUserMoveSize(hwnd);
  for (const auto& key : changedKeys) ArrangeWorkspace(key);
  Reconcile::ScheduleLifecycleReconcile(hwnd);
}

// Converts one serialized thread message into a domain command. Messages not
// owned by the WM are explicitly returned to Windows so STA COM's hidden window
// continues receiving cross-apartment calls.
static WmMessageDisposition HandleWmThreadMessage(const MSG& msg) {
  if ((msg.message == WM_HOTKEY && msg.wParam != HK_DIAGNOSTIC_DUMP) ||
      msg.message == WM_APP_LAYOUT_CYCLE ||
      msg.message == WM_APP_LAYOUT_SET || msg.message == WM_APP_TILE_WORKSPACE ||
      msg.message == WM_APP_MANAGEMENT_MODE_SET) {
    g_wm.pendingOverflowSwitch = {};
  }
  switch (msg.message) {
    case WM_APP_MOVE_SIZE_END:
      HandleMoveSizeEndMessage(reinterpret_cast<HWND>(msg.wParam));
      return WmMessageDisposition::Handled;

    case WM_APP_WINDOW_EVENT:
      Reconcile::ProcessWindowLifecycleEvent(
          static_cast<DWORD>(msg.wParam), reinterpret_cast<HWND>(msg.lParam));
      return WmMessageDisposition::Handled;

    case WM_APP_VIRTUAL_DESKTOP_CHANGED:
      Reconcile::HandleVirtualDesktopChanged();
      return WmMessageDisposition::Handled;

    case WM_APP_FOREGROUND_CHANGED: {
      HWND hwnd = reinterpret_cast<HWND>(msg.wParam);
      RememberManagedForeground(hwnd);
      // Some frameworks make a top-level HWND foreground without delivering a
      // usable SHOW event. Foreground is only a discovery prompt; the settled
      // snapshot remains authoritative for eligibility and admission.
      if (IsAutomaticMode()) {
        if (!hwnd || !IsWindow(hwnd)) {
          // A rapidly replaced foreground HWND can disappear before this posted
          // message is consumed. Preserve the discovery prompt with one settled
          // global snapshot, which can observe its live replacement.
          Reconcile::ScheduleLifecycleReconcile(nullptr);
        } else if (!IsWindowTrackedInAnyState(hwnd) ||
                   std::any_of(g_wm.pendingWindowOverflow.begin(), g_wm.pendingWindowOverflow.end(),
                               [hwnd](const PendingWindowOverflow& request) { return request.hwnd == hwnd; })) {
          Reconcile::ScheduleLifecycleReconcile(hwnd);
        }
      }
      ++Diagnostics::g_runtime.counters.trayRefreshMessages;
      TrayUi::RefreshForCurrentWorkspace();
      return WmMessageDisposition::Handled;
    }

    case WM_APP_LAYOUT_CYCLE:
      Commands::CycleCurrentWorkspaceLayout(
          reinterpret_cast<HMONITOR>(msg.wParam));
      return WmMessageDisposition::Handled;

    case WM_APP_TILE_WORKSPACE:
      Commands::TileWindows(reinterpret_cast<HMONITOR>(msg.wParam));
      return WmMessageDisposition::Handled;

    case WM_APP_LAYOUT_SET:
      Commands::SetCurrentWorkspaceLayout(
          static_cast<TileLayout>(msg.lParam),
          reinterpret_cast<HMONITOR>(msg.wParam));
      return WmMessageDisposition::Handled;

    case WM_APP_MANAGEMENT_MODE_SET:
      if (msg.wParam == static_cast<WPARAM>(ManagementMode::Manual) ||
          msg.wParam == static_cast<WPARAM>(ManagementMode::Automatic)) {
        SetWindowManagementMode(static_cast<ManagementMode>(msg.wParam));
      }
      return WmMessageDisposition::Handled;

    case WM_APP_RECONCILE_NOW:
      Reconcile::ReconcileDeferredLifecycle();
      return WmMessageDisposition::Handled;

    case WM_TIMER:
      // Actor timers are created with SetTimer(nullptr, ...). A window timer can
      // use the same numeric ID and must still reach its WndProc.
      if (msg.hwnd) return WmMessageDisposition::DispatchToWindows;

      if (g_wm.lifecycleTimer &&
          static_cast<UINT_PTR>(msg.wParam) == g_wm.lifecycleTimer) {
        Reconcile::ReconcileDeferredLifecycle();
        return WmMessageDisposition::Handled;
      }
      if (g_wm.discoveryRetryTimer &&
          static_cast<UINT_PTR>(msg.wParam) == g_wm.discoveryRetryTimer) {
        Reconcile::ProcessDiscoveryRetryTimer();
        return WmMessageDisposition::Handled;
      }
      if (g_wm.overflowTimer &&
          static_cast<UINT_PTR>(msg.wParam) == g_wm.overflowTimer) {
        Reconcile::ProcessWindowOverflow();
        return WmMessageDisposition::Handled;
      }
      if (g_wm.maintenanceTimer &&
          static_cast<UINT_PTR>(msg.wParam) == g_wm.maintenanceTimer) {
        RuntimeLifecycle::RunMaintenanceNow();
        return WmMessageDisposition::Handled;
      }
      if (g_wm.conformanceTimer &&
          static_cast<UINT_PTR>(msg.wParam) == g_wm.conformanceTimer) {
        ProcessConformanceTimer();
        return WmMessageDisposition::Handled;
      }
      return WmMessageDisposition::DispatchToWindows;

    case WM_HOTKEY:
      switch (static_cast<UINT>(msg.wParam)) {
        case HK_TILE:
          Commands::TileWindows();
          break;
        case HK_LAYOUT:
          Commands::CycleCurrentWorkspaceLayout();
          break;
        case HK_SWAP_MASTER:
          Commands::SwapMaster();
          break;
        case HK_PROMOTE_WINDOW:
          Commands::PromoteFocusedWindow();
          break;
        case HK_DEMOTE_WINDOW:
          Commands::DemoteFocusedWindow();
          break;
        case HK_FLOAT_FOCUSED:
          Commands::FloatFocusedWindow();
          break;
        case HK_DIAGNOSTIC_DUMP:
          Diagnostics::WriteDiagnosticReport();
          break;
        case HK_MANAGEMENT_MODE_TOGGLE:
          ToggleWindowManagementMode();
          break;
      }
      return WmMessageDisposition::Handled;
  }

  return WmMessageDisposition::DispatchToWindows;
}

static void RunWmMessageLoop() {
  // Standard Windhawk tool-mod worker pattern: one ordinary Windows message loop
  // owns thread messages, timers, the tray windows, and STA COM dispatch.
  MSG msg{};
  for (;;) {
    const BOOL result = GetMessageW(&msg, nullptr, 0, 0);
    if (result == 0) return;  // WM_QUIT
    if (result == -1) {
      Wh_Log(L"WM worker GetMessage failed: %lu", GetLastError());
      return;
    }

    switch (HandleWmThreadMessage(msg)) {
      case WmMessageDisposition::Handled:
        break;

      case WmMessageDisposition::DispatchToWindows:
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        break;
    }
  }
}

static void CleanupWmThread() {
  // Deferred overflow is temporarily Floating while native routing waits. Return
  // those members to tiling before discarding the actor queue so settings reload
  // can enforce the new policy instead of retaining an accidental float forever.
  std::vector<PendingWindowOverflow> deferredOverflow;
  deferredOverflow.swap(g_wm.pendingWindowOverflow);
  for (const PendingWindowOverflow& pending : deferredOverflow) {
    DWORD livePid = 0;
    GetWindowThreadProcessId(pending.hwnd, &livePid);
    if (livePid != pending.processId || !g_wm.overflowWindows.count(pending.hwnd)) continue;
    for (const auto& key : g_workspaces.OwnersOf(pending.hwnd)) {
      Workspace workspace;
      if (!g_workspaces.Load(key, &workspace) || workspace.Layout() == TileLayout::Floating) continue;
      const WindowRecord* record = workspace.Find(pending.hwnd);
      if (!record || record->pid != livePid || record->state != ManageState::Floating ||
          record->alwaysFloating || record->tilingGeneration != pending.tilingGeneration) continue;
      workspace.ActivateTiled(pending.hwnd);
      const SuspensionReason reason = GetPhysicalSuspensionReason(pending.hwnd);
      if (reason != SuspensionReason::None) workspace.Suspend(pending.hwnd, reason);
      g_workspaces.Save(key, workspace);
    }
  }
  // Unhook WinEvent delivery before clearing transient callback observations so
  // no reentrant delivery can repopulate the next settings-reload session.
  UnregisterConfiguredHotkeys();
  TrayUi::Shutdown();
  Platform::WindowEvents::RemoveWinEventHooks();
  CleanupVirtualDesktopAPI();

  if (g_wm.lifecycleTimer) {
    KillTimer(nullptr, g_wm.lifecycleTimer);
    g_wm.lifecycleTimer = 0;
  }
  if (g_wm.discoveryRetryTimer) {
    KillTimer(nullptr, g_wm.discoveryRetryTimer);
    g_wm.discoveryRetryTimer = 0;
  }
  if (g_wm.maintenanceTimer) {
    KillTimer(nullptr, g_wm.maintenanceTimer);
    g_wm.maintenanceTimer = 0;
  }
  if (g_wm.conformanceTimer) {
    KillTimer(nullptr, g_wm.conformanceTimer);
    g_wm.conformanceTimer = 0;
  }
  g_wm.lifecycleDirtyWindows.clear();
  g_wm.pendingDiscoveryRetries.clear();
  g_wm.pendingDesktopArranges.clear();
  if (g_wm.overflowTimer) KillTimer(nullptr, g_wm.overflowTimer);
  g_wm.overflowTimer = 0;
  g_wm.overflowWindows.clear();
  g_wm.pendingWindowOverflow.clear();
  g_wm.pendingOverflowSwitch = {};
  g_wm.windowRoutingReady = false;
  g_wm.windowRoutingEvaluated.clear();
  g_wm.pendingWindowRoutes.clear();
  g_wm.pendingInitialPlacementSwitch = {};
  g_wm.maintenanceAttempts = 0;
  g_wm.maintenanceRunning = false;
  g_wm.lifecycleRetryAfterPlatformRecovery = false;
  g_wm.forceMonitorReconcile = false;
  g_wm.reconciledDesktop = {};
  g_wm.reconciledDesktopState = ReconciledDesktopState::Unknown;
  g_wm.pendingDesktopSwitchFlyouts = false;
  ClearAllConformanceLeases();
  ClearAllMoveSizeSamples();
  ClearMonitorIdentityCache();
  InterlockedExchange(&g_vd.changeQueued, 0);
}

// Owns the STA apartment and delegates startup, serialized dispatch, and cleanup
// to narrow helpers. All workspace mutations reachable from this thread remain
// serialized; foreign STA messages are dispatched back to Windows.
DWORD WINAPI HotkeyThreadProc(LPVOID) {
  SetThreadDpiAwarenessContext(
    DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  const DWORD threadId = GetCurrentThreadId();
  g_wm.threadId.store(threadId, std::memory_order_release);
  Diagnostics::BeginWmSession();
  Wh_Log(L"Hotkey thread started, thread ID: %lu", threadId);

  // Readiness means the worker exists and owns a Windows message queue. Signal it
  // before any shell/COM acquisition so the controller can always request WM_QUIT
  // without waiting on Explorer readiness.
  MSG msg{};
  PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);
  if (g_wm.readyEvent) SetEvent(g_wm.readyEvent);

  HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
  g_wm.comInitialized = SUCCEEDED(coHr);
  Wh_Log(L"CoInitializeEx result: 0x%08X", coHr);

  if (g_wm.comInitialized) {
    RuntimeLifecycle::RunMaintenanceNow();
  } else {
    Platform::WindowEvents::InstallWinEventHooks();
    Wh_Log(L"COM initialization failed; virtual desktop integration unavailable for this WM session");
  }

  // Existing HWNDs, including hidden windows and windows on other desktops,
  // form a baseline before discovery or hook-driven placement can route them.
  if (std::any_of(g_settings.windowRules.begin(), g_settings.windowRules.end(),
                  [](const WindowRule& rule) {
                    return rule.treatment == WindowRuleTreatment::InitialPlacement;
                  })) {
    ++Diagnostics::g_runtime.counters.enumWindowsPasses;
    g_wm.windowRoutingReady = g_hooks.hideDestroy && EnumWindows(
        [](HWND hwnd, LPARAM) WINAPI -> BOOL {
          ++Diagnostics::g_runtime.counters.enumWindowsVisited;
          g_wm.windowRoutingEvaluated.insert(hwnd);
          return TRUE;
        }, 0) != FALSE;
    if (!g_wm.windowRoutingReady) Wh_Log(L"Initial placement baseline failed");
  }

  Reconcile::ScheduleLifecycleReconcile(nullptr);
  RegisterConfiguredHotkeys();
  InitializeCurrentDesktopWorkspaces();

  if (!TrayUi::Initialize()) {
    Wh_Log(L"Tiling layout tray icon unavailable; window management remains active");
  }

  RunWmMessageLoop();

  // Stop callbacks from queueing new work as soon as the actor stops consuming
  // messages. Cleanup then unhooks delivery before clearing callback observations.
  g_wm.threadId.store(0, std::memory_order_release);
  CleanupWmThread();

  if (g_wm.comInitialized) CoUninitialize();
  g_wm.comInitialized = false;
  return 0;
}

bool StartHotkeyThread() {
  if (g_wm.thread) {
    Wh_Log(L"Refusing to start a second WM actor");
    return false;
  }

  g_wm.readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
  if (!g_wm.readyEvent) {
    Wh_Log(L"Failed to create WM readiness event: %lu", GetLastError());
    return false;
  }

  g_wm.thread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);
  if (!g_wm.thread) {
    Wh_Log(L"Failed to create WM thread: %lu", GetLastError());
    CloseHandle(g_wm.readyEvent);
    g_wm.readyEvent = nullptr;
    return false;
  }

  HANDLE waits[] = {g_wm.readyEvent, g_wm.thread};
  DWORD waitResult = WaitForMultipleObjects(ARRAYSIZE(waits), waits, FALSE, INFINITE);
  const bool ready = waitResult == WAIT_OBJECT_0;
  if (!ready) {
    Wh_Log(L"WM thread exited before signaling readiness (wait=%lu)", waitResult);
  }

  CloseHandle(g_wm.readyEvent);
  g_wm.readyEvent = nullptr;

  if (!ready) {
    WaitForSingleObject(g_wm.thread, INFINITE);
    CloseHandle(g_wm.thread);
    g_wm.thread = nullptr;
    g_wm.threadId.store(0, std::memory_order_release);
    return false;
  }
  return true;
}

bool StopHotkeyThread() {
  if (!g_wm.thread) return true;

  const DWORD threadId = g_wm.threadId.load(std::memory_order_acquire);
  if (threadId && !PostThreadMessage(threadId, WM_QUIT, 0, 0)) {
    Wh_Log(L"Failed to post WM_QUIT to WM thread: %lu", GetLastError());
  }

  const DWORD waitResult = WaitForSingleObject(g_wm.thread, 5000);
  if (waitResult != WAIT_OBJECT_0) {
    // During settings reload, retaining these handles prevents a second WM owner.
    // During WhTool_ModUninit, the standard tool-mod wrapper exits the dedicated
    // process immediately after the callback returns.
    Wh_Log(L"WM thread did not stop cleanly (wait=%lu); leaving the existing actor intact", waitResult);
    return false;
  }

  CloseHandle(g_wm.thread);
  g_wm.thread = nullptr;
  g_wm.threadId.store(0, std::memory_order_release);
  if (g_wm.readyEvent) {
    CloseHandle(g_wm.readyEvent);
    g_wm.readyEvent = nullptr;
  }
  return true;
}

//=============================================================================
// Windhawk Tool Mod Entry Points
//=============================================================================

BOOL WhTool_ModInit() {
  Diagnostics::InitializeProcessRuntime();
  Wh_Log(L"MultiWM mod initializing...");
  LoadSettings();
  if (!StartHotkeyThread()) {
    Wh_Log(L"Failed to start hotkey thread");
    return FALSE;
  }
  Wh_Log(L"MultiWM mod initialized successfully");
  return TRUE;
}

void WhTool_ModUninit() {
  Wh_Log(L"MultiWM mod uninitializing...");
  if (!StopHotkeyThread()) {
    // The standard tool-mod wrapper exits this dedicated process immediately
    // after WhTool_ModUninit returns, so never create a second owner here.
    Wh_Log(L"WM worker did not join before tool-process shutdown");
  }
  Wh_Log(L"MultiWM mod uninitialized");
}

void WhTool_ModSettingsChanged() {
  Wh_Log(L"Settings changed, reloading...");

  // Join the old diagnostics/model owner before touching lifetime telemetry.
  // If it cannot stop, keep the old worker/settings rather than allowing two WM
  // actors to overlap inside the dedicated tool-mod process.
  if (!StopHotkeyThread()) {
    Wh_Log(L"Settings reload aborted because the existing WM worker is still alive");
    return;
  }

  ++Diagnostics::g_runtime.counters.settingsReloads;
  Diagnostics::RecordEvent(L"settings reload requested");

  // Settings reload changes defaults/policies but does not destroy per-desktop
  // workspace identity, layout mode, ordering, or learned ratios.
  LoadSettings();
  if (!StartHotkeyThread()) {
    Wh_Log(L"Failed to restart WM worker after settings change");
  }
}

//=============================================================================
// Windhawk tool mod implementation for mods which don't need to inject into other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// This mod launches a dedicated copy of its configured host executable.
//=============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

// Ends only the dedicated worker's bootstrap thread at its normal entry point;
// the already-started hotkey thread keeps the tool process alive.
void WINAPI EntryPoint_Hook() {
  Wh_Log(L">");
  ExitThread(0);
}

// Routes each host executable instance into its tool-mod role. Normal instances act
// as launchers; only the worker tagged with this mod ID enforces singleton
// ownership and initializes the tiling service.
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

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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

// Launcher-side phase: starts a copy of the current host executable tagged as this mod's
// dedicated tool process, then releases the returned process and thread handles.
void Wh_ModAfterInit() {
  if (!g_isToolModProcessLauncher) {
    return;
  }

  WCHAR currentProcessPath[MAX_PATH];
  switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
    case 0:
    case ARRAYSIZE(currentProcessPath):
      Wh_Log(L"GetModuleFileName failed");
      return;
  }

  WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
  swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

  HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
  if (!kernelModule) {
    kernelModule = GetModuleHandle(L"kernel32.dll");
    if (!kernelModule) {
      Wh_Log(L"No kernelbase.dll/kernel32.dll");
      return;
    }
  }

  using CreateProcessInternalW_t = BOOL(WINAPI*)(
      HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
      LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
      LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
      PHANDLE hRestrictedUserToken);
  CreateProcessInternalW_t pCreateProcessInternalW =
      (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
  if (!pCreateProcessInternalW) {
    Wh_Log(L"No CreateProcessInternalW");
    return;
  }

  STARTUPINFO si{
      .cb = sizeof(STARTUPINFO),
      .dwFlags = STARTF_FORCEOFFFEEDBACK,
  };
  PROCESS_INFORMATION pi;
  if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
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

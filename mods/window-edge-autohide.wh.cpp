// ==WindhawkMod==
// @id              window-edge-autohide
// @name            Window Edge Auto-Hide
// @description     Dock the focused window to a screen edge, where it hides until you hover the thin strip it leaves
// @version         1.0.1
// @author          Molko
// @github          https://github.com/roeseth
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Edge Auto-Hide

Dock the focused window to a screen edge. It slides off the screen and leaves a
thin strip visible. Hover the strip to bring the window back in front of other
windows. Drag a docked window by its title bar to slide it along its edge.

## Default hotkeys
- Win+Ctrl+Left / Right / Up / Down: dock the focused window to that edge
- Win+Ctrl+Delete: stop auto-hiding the focused window
- Win+Ctrl+Shift+Delete: stop auto-hiding every docked window

Each hotkey is a set of modifier keys plus one key, picked in the settings.
The default Win+Ctrl+Left and Win+Ctrl+Right take over the Windows shortcuts
that switch virtual desktops. Pick other keys, or None, to get the Windows
behavior back.

## Games and full-screen apps
Pointing at a strip doesn't reveal its window in three cases. Each has its own
setting, and all three are on by default:
- The app in front runs in exclusive full screen on the window's monitor.
- The app in front covers the window's whole monitor with a window that has no
  title bar or border, such as a borderless game or a browser in full screen.
- The pointer is hidden. A game hides it while the mouse turns the camera, and
  a quick flick can still carry it to the screen edge.

An app list overrides these per app. As a blacklist, it keeps windows hidden
whenever a listed app is in front, also when that app runs in a window. As a
whitelist, a listed app never keeps windows hidden.

## Compatibility

### Stardock Groupy
Windows grouped with Stardock Groupy dock as one group, tab bar included.
Switching tabs keeps the group docked, a window added to a docked group joins
it, and a window taken out of the group is released where you drop it.

A group docked to the top edge leaves no strip and doesn't slide. Groupy
shrinks a window that moves above the top of the screen, so the hidden group
waits off screen below its monitor. Point at the top edge where the group was
docked to bring it back. With another monitor below, there is no room to hide
the group, and it doesn't dock to the top.

## Notes
- Windows of apps that run as administrator, including Windhawk's own window,
  can't be docked. The mod runs without administrator rights, and Windows
  doesn't pass it the hotkey while such a window has focus.
- Reloading or updating the mod keeps docked windows docked. Disabling it, or
  closing Windhawk, slides them back on screen after a few seconds. If
  the mod's process is killed instead, hidden windows stay at the edge, and
  the mod docks them again when it next starts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dockLeft:
  - modifiers: WinCtrl
    $name: Modifier keys
    $options:
    - WinCtrl: Win + Ctrl
    - WinAlt: Win + Alt
    - WinShift: Win + Shift
    - CtrlAlt: Ctrl + Alt
    - CtrlShift: Ctrl + Shift
    - AltShift: Alt + Shift
    - WinCtrlAlt: Win + Ctrl + Alt
    - WinCtrlShift: Win + Ctrl + Shift
    - WinAltShift: Win + Alt + Shift
    - CtrlAltShift: Ctrl + Alt + Shift
    - WinCtrlAltShift: Win + Ctrl + Alt + Shift
    - Win: Win
    - Ctrl: Ctrl
    - Alt: Alt
  - key: Left
    $name: Key
    $options:
    - none: None
    - Left: Left arrow
    - Right: Right arrow
    - Up: Up arrow
    - Down: Down arrow
    - Delete: Delete
    - Backspace: Backspace
    - Insert: Insert
    - Home: Home
    - End: End
    - PageUp: Page Up
    - PageDown: Page Down
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - A: A
    - B: B
    - C: C
    - D: D
    - E: E
    - F: F
    - G: G
    - H: H
    - I: I
    - J: J
    - K: K
    - L: L
    - M: M
    - N: N
    - O: O
    - P: P
    - Q: Q
    - R: R
    - S: S
    - T: T
    - U: U
    - V: V
    - W: W
    - X: X
    - Y: Y
    - Z: Z
  $name: Dock left hotkey
  $description: >-
    Docks the focused window to the left screen edge. The default takes over the
    Windows shortcut that switches virtual desktops. Pick None as the key to
    turn it off.
- dockRight:
  - modifiers: WinCtrl
    $name: Modifier keys
    $options:
    - WinCtrl: Win + Ctrl
    - WinAlt: Win + Alt
    - WinShift: Win + Shift
    - CtrlAlt: Ctrl + Alt
    - CtrlShift: Ctrl + Shift
    - AltShift: Alt + Shift
    - WinCtrlAlt: Win + Ctrl + Alt
    - WinCtrlShift: Win + Ctrl + Shift
    - WinAltShift: Win + Alt + Shift
    - CtrlAltShift: Ctrl + Alt + Shift
    - WinCtrlAltShift: Win + Ctrl + Alt + Shift
    - Win: Win
    - Ctrl: Ctrl
    - Alt: Alt
  - key: Right
    $name: Key
    $options:
    - none: None
    - Left: Left arrow
    - Right: Right arrow
    - Up: Up arrow
    - Down: Down arrow
    - Delete: Delete
    - Backspace: Backspace
    - Insert: Insert
    - Home: Home
    - End: End
    - PageUp: Page Up
    - PageDown: Page Down
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - A: A
    - B: B
    - C: C
    - D: D
    - E: E
    - F: F
    - G: G
    - H: H
    - I: I
    - J: J
    - K: K
    - L: L
    - M: M
    - N: N
    - O: O
    - P: P
    - Q: Q
    - R: R
    - S: S
    - T: T
    - U: U
    - V: V
    - W: W
    - X: X
    - Y: Y
    - Z: Z
  $name: Dock right hotkey
  $description: >-
    Docks the focused window to the right screen edge. The default takes over
    the Windows shortcut that switches virtual desktops. Pick None as the key to
    turn it off.
- dockTop:
  - modifiers: WinCtrl
    $name: Modifier keys
    $options:
    - WinCtrl: Win + Ctrl
    - WinAlt: Win + Alt
    - WinShift: Win + Shift
    - CtrlAlt: Ctrl + Alt
    - CtrlShift: Ctrl + Shift
    - AltShift: Alt + Shift
    - WinCtrlAlt: Win + Ctrl + Alt
    - WinCtrlShift: Win + Ctrl + Shift
    - WinAltShift: Win + Alt + Shift
    - CtrlAltShift: Ctrl + Alt + Shift
    - WinCtrlAltShift: Win + Ctrl + Alt + Shift
    - Win: Win
    - Ctrl: Ctrl
    - Alt: Alt
  - key: Up
    $name: Key
    $options:
    - none: None
    - Left: Left arrow
    - Right: Right arrow
    - Up: Up arrow
    - Down: Down arrow
    - Delete: Delete
    - Backspace: Backspace
    - Insert: Insert
    - Home: Home
    - End: End
    - PageUp: Page Up
    - PageDown: Page Down
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - A: A
    - B: B
    - C: C
    - D: D
    - E: E
    - F: F
    - G: G
    - H: H
    - I: I
    - J: J
    - K: K
    - L: L
    - M: M
    - N: N
    - O: O
    - P: P
    - Q: Q
    - R: R
    - S: S
    - T: T
    - U: U
    - V: V
    - W: W
    - X: X
    - Y: Y
    - Z: Z
  $name: Dock top hotkey
  $description: >-
    Docks the focused window to the top screen edge. Pick None as the key to
    turn it off.
- dockBottom:
  - modifiers: WinCtrl
    $name: Modifier keys
    $options:
    - WinCtrl: Win + Ctrl
    - WinAlt: Win + Alt
    - WinShift: Win + Shift
    - CtrlAlt: Ctrl + Alt
    - CtrlShift: Ctrl + Shift
    - AltShift: Alt + Shift
    - WinCtrlAlt: Win + Ctrl + Alt
    - WinCtrlShift: Win + Ctrl + Shift
    - WinAltShift: Win + Alt + Shift
    - CtrlAltShift: Ctrl + Alt + Shift
    - WinCtrlAltShift: Win + Ctrl + Alt + Shift
    - Win: Win
    - Ctrl: Ctrl
    - Alt: Alt
  - key: Down
    $name: Key
    $options:
    - none: None
    - Left: Left arrow
    - Right: Right arrow
    - Up: Up arrow
    - Down: Down arrow
    - Delete: Delete
    - Backspace: Backspace
    - Insert: Insert
    - Home: Home
    - End: End
    - PageUp: Page Up
    - PageDown: Page Down
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - A: A
    - B: B
    - C: C
    - D: D
    - E: E
    - F: F
    - G: G
    - H: H
    - I: I
    - J: J
    - K: K
    - L: L
    - M: M
    - N: N
    - O: O
    - P: P
    - Q: Q
    - R: R
    - S: S
    - T: T
    - U: U
    - V: V
    - W: W
    - X: X
    - Y: Y
    - Z: Z
  $name: Dock bottom hotkey
  $description: >-
    Docks the focused window to the bottom screen edge. Pick None as the key to
    turn it off.
- release:
  - modifiers: WinCtrl
    $name: Modifier keys
    $options:
    - WinCtrl: Win + Ctrl
    - WinAlt: Win + Alt
    - WinShift: Win + Shift
    - CtrlAlt: Ctrl + Alt
    - CtrlShift: Ctrl + Shift
    - AltShift: Alt + Shift
    - WinCtrlAlt: Win + Ctrl + Alt
    - WinCtrlShift: Win + Ctrl + Shift
    - WinAltShift: Win + Alt + Shift
    - CtrlAltShift: Ctrl + Alt + Shift
    - WinCtrlAltShift: Win + Ctrl + Alt + Shift
    - Win: Win
    - Ctrl: Ctrl
    - Alt: Alt
  - key: Delete
    $name: Key
    $options:
    - none: None
    - Left: Left arrow
    - Right: Right arrow
    - Up: Up arrow
    - Down: Down arrow
    - Delete: Delete
    - Backspace: Backspace
    - Insert: Insert
    - Home: Home
    - End: End
    - PageUp: Page Up
    - PageDown: Page Down
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - A: A
    - B: B
    - C: C
    - D: D
    - E: E
    - F: F
    - G: G
    - H: H
    - I: I
    - J: J
    - K: K
    - L: L
    - M: M
    - N: N
    - O: O
    - P: P
    - Q: Q
    - R: R
    - S: S
    - T: T
    - U: U
    - V: V
    - W: W
    - X: X
    - Y: Y
    - Z: Z
  $name: Release hotkey
  $description: >-
    Stops auto-hiding the focused window and leaves it shown at its edge. Pick
    None as the key to turn it off.
- releaseAll:
  - modifiers: WinCtrlShift
    $name: Modifier keys
    $options:
    - WinCtrl: Win + Ctrl
    - WinAlt: Win + Alt
    - WinShift: Win + Shift
    - CtrlAlt: Ctrl + Alt
    - CtrlShift: Ctrl + Shift
    - AltShift: Alt + Shift
    - WinCtrlAlt: Win + Ctrl + Alt
    - WinCtrlShift: Win + Ctrl + Shift
    - WinAltShift: Win + Alt + Shift
    - CtrlAltShift: Ctrl + Alt + Shift
    - WinCtrlAltShift: Win + Ctrl + Alt + Shift
    - Win: Win
    - Ctrl: Ctrl
    - Alt: Alt
  - key: Delete
    $name: Key
    $options:
    - none: None
    - Left: Left arrow
    - Right: Right arrow
    - Up: Up arrow
    - Down: Down arrow
    - Delete: Delete
    - Backspace: Backspace
    - Insert: Insert
    - Home: Home
    - End: End
    - PageUp: Page Up
    - PageDown: Page Down
    - F1: F1
    - F2: F2
    - F3: F3
    - F4: F4
    - F5: F5
    - F6: F6
    - F7: F7
    - F8: F8
    - F9: F9
    - F10: F10
    - F11: F11
    - F12: F12
    - A: A
    - B: B
    - C: C
    - D: D
    - E: E
    - F: F
    - G: G
    - H: H
    - I: I
    - J: J
    - K: K
    - L: L
    - M: M
    - N: N
    - O: O
    - P: P
    - Q: Q
    - R: R
    - S: S
    - T: T
    - U: U
    - V: V
    - W: W
    - X: X
    - Y: Y
    - Z: Z
  $name: Release all hotkey
  $description: >-
    Stops auto-hiding every docked window and leaves each one shown at its edge.
    Pick None as the key to turn it off.
- keepFocusedShown: false
  $name: Keep a focused window shown
  $description: >-
    When on, a revealed window stays shown while it has focus, and switching to
    a hidden window reveals it. It hides once focus is elsewhere and the pointer
    is away. When off, a window hides as soon as the pointer leaves it, even
    while it has focus.
- slideDuration: 120
  $name: Slide duration (ms)
  $description: >-
    How long a window takes to slide out of or back onto the screen, in
    milliseconds (up to 2000). 0 turns the slide off.
- revealSize: 6
  $name: Visible strip (pixels)
  $description: >-
    How much of a hidden window stays on screen, in pixels at 100% scaling.
    Hover it to reveal the window.
- stayHidden:
  - exclusive: true
    $name: In exclusive full screen
    $description: >-
      The app in front has taken over the display, as most games do in their
      Fullscreen display mode.
  - borderless: true
    $name: In borderless full screen
    $description: >-
      The app in front covers the whole monitor with a window that has no
      title bar or border, such as a borderless game or a browser in full
      screen.
  - pointerHidden: true
    $name: While the pointer is hidden
    $description: >-
      A game hides the pointer while the mouse turns the camera, and a quick
      flick can still carry it to the screen edge.
  $name: Keep windows hidden
  $description: >-
    In these cases, pointing at a strip doesn't reveal its window. The two
    full-screen cases apply to windows docked on the monitor that the app
    covers.
- appListMode: blacklist
  $name: App list type
  $description: >-
    With Blacklist, pointing at a strip never reveals its window while a listed
    app is in front, also when the app runs in a window. With Whitelist, a
    listed app never keeps windows hidden, full screen or not. Apps that aren't
    listed follow the settings above.
  $options:
  - blacklist: Blacklist
  - whitelist: Whitelist
- appList:
  - ""
  $name: App list
  $description: >-
    Executable names such as game.exe, one per entry, without the folder.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

enum class Edge { Left, Top, Right, Bottom };
// Released: no longer docked, still sliding to its shown spot; topmost is
// restored and the entry dropped once the slide ends.
enum class Phase { Hidden, Shown, Dragging, Released };

using Clock = std::chrono::steady_clock;  // GetTickCount's 16 ms steps would show as judder at 240 Hz

struct Motion {
  POINT from, to;  // visible frame's top-left
  Clock::time_point start;
  std::chrono::milliseconds duration;
  // The target belongs to this phase and edge. It is kept for the whole slide:
  // a group's tab bar trails the window by a few ms, which would otherwise
  // nudge the target every frame and restart the slide.
  Phase phase;
  Edge edge;
  POINT sent;  // last position requested
  Clock::time_point sentAt;
};

// A docked unit: one window, or a whole Groupy group moved through its active tab.
struct Docked {
  HWND hwnd;  // the window moved: the window itself, or its group's active tab
  Edge edge;
  Phase phase;
  DWORD leftAt;     // GetTickCount when the pointer left a Shown window; 0 while inside
  bool wasTopmost;  // WS_EX_TOPMOST before docking, restored on release
  bool clipping;    // a caption drag is in progress and the cursor is clipped to the slide line
  std::optional<Motion> motion;
  HWND group;                 // GroupOf(hwnd), or nullptr
  std::vector<HWND> members;  // the group's tabs last tick; one that leaves is released
  POINT lastAt;               // hwnd's window rect top-left last time it was measured
};

// Groupy (Stardock) has no API; this is read from the window properties it
// sets (Groupy 2, measured 2026-11-26). A tab's GP_ROWNER is its group's owner
// window and GP_LINK the tab bar above it; the active tab is the one not
// cloaked. Groupy moves the tab bar and the other tabs after the active one
// within 10 ms, spreads topmost across the group, and clears both properties
// when a window leaves.

// The live window that property `name` of `hwnd` holds, or nullptr. `name` is
// a string or the MAKEINTATOM of its atom; atom 0 means no window has it.
HWND PropWindow(HWND hwnd, PCWSTR name) {
  HWND window = name ? static_cast<HWND>(GetProp(hwnd, name)) : nullptr;
  return window && IsWindow(window) ? window : nullptr;
}

// Returns the window that stands for the tab's group. Not every group has an
// owner window (measured: a two-tab group without one), so the tab bar, which
// every group has, stands in.
HWND GroupOf(HWND hwnd, PCWSTR owner = L"GP_ROWNER", PCWSTR link = L"GP_LINK") {
  HWND group = PropWindow(hwnd, owner);
  return group ? group : PropWindow(hwnd, link);
}

bool Cloaked(HWND hwnd) {
  DWORD cloaked = 0;
  return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

struct GroupScan {
  HWND group;
  PCWSTR owner, link;  // the two properties, as atoms
  std::vector<HWND> members;
};

BOOL CALLBACK AddGroupMember(HWND hwnd, LPARAM param) {
  auto* scan = reinterpret_cast<GroupScan*>(param);
  if (GroupOf(hwnd, scan->owner, scan->link) == scan->group) scan->members.push_back(hwnd);
  return TRUE;
}

// GetProp looks a name's atom up on every call, which over every top-level
// window is a third of the scan (measured: 695 windows, 1.5 ms by name,
// 1.0 ms by atom), so the atoms are looked up once per scan.
std::vector<HWND> MembersOf(HWND group) {
  GroupScan scan{group, MAKEINTATOM(GlobalFindAtom(L"GP_ROWNER")), MAKEINTATOM(GlobalFindAtom(L"GP_LINK")), {}};
  EnumWindows(AddGroupMember, reinterpret_cast<LPARAM>(&scan));
  return scan.members;
}

HWND ActiveOf(const std::vector<HWND>& members) {
  auto it = std::ranges::find_if(members, [](HWND m) { return !Cloaked(m); });
  return it == members.end() ? nullptr : *it;
}

HWND TabBarOf(HWND hwnd) {
  HWND bar = PropWindow(hwnd, L"GP_LINK");
  return bar && IsWindowVisible(bar) && !Cloaked(bar) ? bar : nullptr;
}

// Groupy pushes the top of a group's active tab down, 50 px at a time, for as
// long as that top is at or above the top of the work area (measured: from any
// spot above it, on screen or off, until the window has no height left). So a
// group docked at the top can't hide past that edge. Hidden, it waits below
// its monitor instead, wholly off screen, and it jumps between there and the
// top edge rather than slide through the band where Groupy pushes it down. Its
// strip is then only the place to point at.
bool ParksBelow(const Docked& d) {
  return d.group && d.edge == Edge::Top;
}

RECT ParkedFrame(RECT frame, const RECT& monitor) {
  OffsetRect(&frame, 0, monitor.bottom - frame.top);
  return frame;
}

// Docking any tab docks its whole group, moved through the active tab.
Docked Unit(HWND hwnd, Edge edge, Phase phase) {
  Docked d{.hwnd = hwnd, .edge = edge, .phase = phase, .group = GroupOf(hwnd)};
  if (d.group) {
    d.members = MembersOf(d.group);
    if (HWND active = ActiveOf(d.members)) d.hwnd = active;
  }
  return d;
}

struct Hotkey {
  UINT modifiers;
  UINT vk;  // 0: disabled
};

// Actions 0-3 dock to static_cast<Edge>(action).
constexpr PCWSTR kHotkeySettings[] = {L"dockLeft", L"dockTop", L"dockRight",
                                      L"dockBottom", L"release", L"releaseAll"};
constexpr WPARAM kReleaseAction = 4;
constexpr WPARAM kReleaseAllAction = 5;

constexpr DWORD kTickMs = 50;
constexpr DWORD kHideDelayMs = 300;
// Finding the tabs of a docked Groupy group takes a scan of every window.
// On every tick that cost 2 % of a core per group (measured), so it runs
// once in this many ticks, and SyncGroup checks the tabs it knows in between.
constexpr int kGroupScanTicks = 20;
// A docked window carries its dock in this window property, so the dock
// outlives the process: a property set from here stays after this process
// exits and goes away with its window (measured). See DockMark for the value.
constexpr PCWSTR kDockProp = L"WindowEdgeAutoHide.Dock";
// Held by a new tool process while it waits for the running one to exit.
constexpr PCWSTR kNextInstance = L"windhawk-tool-mod-next_" WH_MOD_ID;
// How long an unloading instance waits for a new one before it takes the
// unload for the mod being disabled.
constexpr DWORD kHandoverMs = 3000;
// A busy app may still hold a finished slide's last move; only slide it again
// once it has had this long to apply it.
constexpr std::chrono::seconds kResendAfter{1};
// One move in flight per window: the next goes once the window shows the last,
// or after this long when an app clamps the position itself. Posting every
// frame let a browser window playing video fall behind and replay the slide.
constexpr std::chrono::milliseconds kPostTimeout{100};
constexpr int kMaxSlideMs = 2000;
// Async, so a hung target app can't block the worker long enough for Windows
// to drop the low-level keyboard hook.
constexpr UINT kMoveFlags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;

enum : UINT { WM_APP_ACTION = WM_APP, WM_APP_SETTINGS };

// Everything below is owned by the worker thread; other threads only post to it.
struct {
  Hotkey hotkeys[std::size(kHotkeySettings)];
  bool keepFocusedShown;
  int revealSize;
  std::chrono::milliseconds slide;
  bool hideInExclusive;
  bool hideInBorderless;
  bool hideWhilePointerHidden;
  bool whitelist;  // the app list exempts its apps; otherwise it keeps windows hidden in them
  std::vector<std::wstring> apps;  // executable names
} g_settings;

std::vector<Docked> g_docked;
RECT g_clip;  // slide line of the caption drag in progress; there is only one cursor
UINT_PTR g_timer;
bool g_pointerHidden;  // when last seen away from a hidden window's strip

// Windows revealed together are raised one at a time, bottom first, so they
// keep their order: async raises to different apps land in any order
// (measured: two sent at once kept their order 0 times in 20). The next goes
// once the last shows above it, or after kPostTimeout, so an app slower than
// that to apply its raise can still be overtaken.
struct {
  std::vector<HWND> queue;  // bottom first
  HWND sent;                // last raised, nullptr before the first
  Clock::time_point sentAt;
} g_raise;

HANDLE g_thread;
DWORD g_threadId;

// The ids of the key dropdown in the settings; anything else (None) is 0.
UINT KeyCode(std::wstring_view key) {
  static constexpr std::pair<std::wstring_view, UINT> kKeys[] = {
      {L"Left", VK_LEFT},     {L"Right", VK_RIGHT},    {L"Up", VK_UP},         {L"Down", VK_DOWN},
      {L"Delete", VK_DELETE}, {L"Backspace", VK_BACK}, {L"Insert", VK_INSERT}, {L"Home", VK_HOME},
      {L"End", VK_END},       {L"PageUp", VK_PRIOR},   {L"PageDown", VK_NEXT}, {L"F1", VK_F1},
      {L"F2", VK_F2},         {L"F3", VK_F3},          {L"F4", VK_F4},         {L"F5", VK_F5},
      {L"F6", VK_F6},         {L"F7", VK_F7},          {L"F8", VK_F8},         {L"F9", VK_F9},
      {L"F10", VK_F10},       {L"F11", VK_F11},        {L"F12", VK_F12},
  };
  for (auto [name, vk] : kKeys) {
    if (name == key) return vk;
  }
  return key.size() == 1 && key[0] >= L'A' && key[0] <= L'Z' ? key[0] : 0;
}

// The ids of the modifier dropdown: Win, Ctrl, Alt and Shift run together, as
// in WinCtrl. Anything else is 0.
UINT ModifierCode(std::wstring_view id) {
  static constexpr std::pair<std::wstring_view, UINT> kParts[] = {
      {L"Win", MOD_WIN}, {L"Ctrl", MOD_CONTROL}, {L"Alt", MOD_ALT}, {L"Shift", MOD_SHIFT}};
  UINT modifiers = 0;
  while (!id.empty()) {
    auto part = std::ranges::find_if(kParts, [&](const auto& p) { return id.starts_with(p.first); });
    if (part == std::end(kParts)) return 0;
    modifiers |= part->second;
    id.remove_prefix(part->first.size());
  }
  return modifiers;
}

void LoadSettings() {
  for (size_t i = 0; i < std::size(kHotkeySettings); i++) {
    PCWSTR name = kHotkeySettings[i];
    PCWSTR modifiers = Wh_GetStringSetting(L"%s.modifiers", name);
    PCWSTR key = Wh_GetStringSetting(L"%s.key", name);
    Hotkey hotkey = {ModifierCode(modifiers), KeyCode(key)};
    // A bare or Shift-only key would be swallowed everywhere it's typed. The
    // dropdown offers neither, but a stale or hand-edited setting can.
    if (hotkey.vk && !(hotkey.modifiers & ~MOD_SHIFT)) {
      Wh_Log(L"%s needs a modifier besides Shift, that hotkey is disabled", name);
      hotkey.vk = 0;
    }
    g_settings.hotkeys[i] = hotkey;
    Wh_FreeStringSetting(modifiers);
    Wh_FreeStringSetting(key);
  }
  g_settings.keepFocusedShown = Wh_GetIntSetting(L"keepFocusedShown") != 0;
  g_settings.revealSize = std::max(1, Wh_GetIntSetting(L"revealSize"));
  g_settings.slide = std::chrono::milliseconds(std::clamp(Wh_GetIntSetting(L"slideDuration"), 0, kMaxSlideMs));

  g_settings.hideInExclusive = Wh_GetIntSetting(L"stayHidden.exclusive") != 0;
  g_settings.hideInBorderless = Wh_GetIntSetting(L"stayHidden.borderless") != 0;
  g_settings.hideWhilePointerHidden = Wh_GetIntSetting(L"stayHidden.pointerHidden") != 0;
  PCWSTR mode = Wh_GetStringSetting(L"appListMode");
  g_settings.whitelist = wcscmp(mode, L"whitelist") == 0;
  Wh_FreeStringSetting(mode);
  g_settings.apps.clear();
  for (int i = 0;; i++) {
    PCWSTR app = Wh_GetStringSetting(L"appList[%d]", i);
    bool last = !*app;
    if (!last) g_settings.apps.push_back(app);
    Wh_FreeStringSetting(app);
    if (last) break;
  }
}

UINT HeldModifiers() {
  auto held = [](int vk) { return GetAsyncKeyState(vk) < 0; };
  return (held(VK_LWIN) || held(VK_RWIN) ? MOD_WIN : 0) | (held(VK_CONTROL) ? MOD_CONTROL : 0) |
         (held(VK_MENU) ? MOD_ALT : 0) | (held(VK_SHIFT) ? MOD_SHIFT : 0);
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
  if (code == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
    DWORD vk = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam)->vkCode;
    for (WPARAM action = 0; action < std::size(g_settings.hotkeys); action++) {
      const Hotkey& hotkey = g_settings.hotkeys[action];
      if (hotkey.vk && hotkey.vk == vk && hotkey.modifiers == HeldModifiers()) {
        PostThreadMessage(GetCurrentThreadId(), WM_APP_ACTION, action, 0);
        return 1;
      }
    }
  }
  return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Releasing Win after a swallowed key would open Start, and releasing Alt would
// open the app's menu bar; an unassigned key in between cancels both.
void SendMaskKey() {
  INPUT inputs[2] = {};
  for (INPUT& input : inputs) {
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = 0xE8;
  }
  inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
  SendInput(2, inputs, sizeof(INPUT));
}

// Hidden keeps `strip` px inside the monitor rect rather than the work area, so
// the hidden part never covers a taskbar on that edge.
RECT DesiredFrame(const RECT& frame, const RECT& monitor, const RECT& work, Edge edge, Phase phase, int strip) {
  if (phase == Phase::Dragging) return frame;

  LONG width = frame.right - frame.left;
  LONG height = frame.bottom - frame.top;
  LONG x = std::max(work.left, std::min(frame.left, work.right - width));
  LONG y = std::max(work.top, std::min(frame.top, work.bottom - height));
  bool shown = phase != Phase::Hidden;
  switch (edge) {
    case Edge::Left:
      x = shown ? work.left : monitor.left + strip - width;
      break;
    case Edge::Right:
      x = shown ? work.right - width : monitor.right - strip;
      break;
    case Edge::Top:
      y = shown ? work.top : monitor.top + strip - height;
      break;
    case Edge::Bottom:
      y = shown ? work.bottom - height : monitor.bottom - strip;
      break;
  }
  return {x, y, x + width, y + height};
}

struct Placement {
  RECT window;
  RECT frame;  // visible frame, without the invisible resize borders of Win10/11 windows
  RECT monitor;
  RECT work;
  int strip;
  // A DPI-unaware window on a scaled monitor can only sit where its rounded
  // logical position lands (measured: odd y targets miss by 1 px at 150 %, a
  // hop onto 125 % by 2 px), so moving until exact would resend the move.
  int slack;
  // The edge belongs to another monitor than the one the window is on: a
  // DPI-unaware window sliding across a scale change gets its position
  // rescaled mid-slide (measured 626 -> 521 at 150 -> 125 %), so it jumps.
  bool hop;
};

// Hung windows are skipped: posted moves would pile up in their message queue.
// A released window still gets its one slide back on screen, even hidden or hung.
bool Movable(const Docked& d) {
  if (IsIconic(d.hwnd) || IsZoomed(d.hwnd)) return false;
  return d.phase == Phase::Released || (IsWindowVisible(d.hwnd) && !IsHungAppWindow(d.hwnd));
}

// Ease-out cubic: quick to leave, gentle landing, never past `to`.
POINT SlideAt(POINT from, POINT to, double t) {
  double e = 1 - std::pow(1 - std::clamp(t, 0.0, 1.0), 3);
  return {from.x + std::lround((to.x - from.x) * e), from.y + std::lround((to.y - from.y) * e)};
}

// Windows pushes a window that straddles the bottom of a top taskbar down
// below it, topmost or not (measured: every top above the work area with the
// bottom below it moved to the work area's top; a bottom taskbar leaves such
// windows alone). A slide under a top taskbar skips that stretch: a hide jumps
// fully into the taskbar band and slides the rest, a reveal lands at once, and
// so does a window hidden at the top that is docked to another edge. Only the
// top edge was measured; a left taskbar may need the same.
POINT SkipTopBand(POINT pos, POINT from, POINT to, LONG height, const RECT& work, const RECT& monitor) {
  if (work.top <= monitor.top) return pos;
  if (pos.y >= work.top || pos.y + height <= work.top) return pos;
  return {pos.x, to.y > from.y ? to.y : work.top - height};
}

// Hops to the neighbour past the docked edge until that edge is an outer one,
// so the hidden part of the window never shows on another monitor.
HMONITOR OuterMonitor(HMONITOR monitor, const RECT& frame, Edge edge, MONITORINFO* info) {
  POINT center = {(frame.left + frame.right) / 2, (frame.top + frame.bottom) / 2};
  while (true) {
    info->cbSize = sizeof(*info);
    if (!GetMonitorInfo(monitor, info)) return nullptr;

    const RECT& m = info->rcMonitor;
    POINT beyond = {std::clamp(center.x, m.left, m.right - 1), std::clamp(center.y, m.top, m.bottom - 1)};
    switch (edge) {
      case Edge::Left:
        beyond.x = m.left - 1;
        break;
      case Edge::Right:
        beyond.x = m.right;
        break;
      case Edge::Top:
        beyond.y = m.top - 1;
        break;
      case Edge::Bottom:
        beyond.y = m.bottom;
        break;
    }

    HMONITOR next = MonitorFromPoint(beyond, MONITOR_DEFAULTTONULL);
    if (!next) return monitor;
    monitor = next;
  }
}

std::optional<Placement> Measure(const Docked& d) {
  Placement p;
  if (!Movable(d) || !GetWindowRect(d.hwnd, &p.window)) return std::nullopt;
  if (FAILED(DwmGetWindowAttribute(d.hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &p.frame, sizeof(p.frame)))) {
    p.frame = p.window;
  }
  // A group's outline includes its tab bar, so the bar hides too and a top
  // dock keeps it out from under a top taskbar. Moving the outline moves the
  // active tab by the same amount; Groupy brings the rest.
  RECT bar;
  if (HWND tabBar = d.group ? TabBarOf(d.hwnd) : nullptr; tabBar && GetWindowRect(tabBar, &bar)) {
    UnionRect(&p.frame, &p.frame, &bar);
  }

  // A parked group lies wholly off screen, where the nearest monitor can be a
  // neighbour that reaches lower (measured within 50 px of one). The monitor
  // just above its outline is the one it is docked on, parked or shown.
  POINT above = {(p.frame.left + p.frame.right) / 2, p.frame.top - 1};
  HMONITOR on = ParksBelow(d) ? MonitorFromPoint(above, MONITOR_DEFAULTTONEAREST)
                              : MonitorFromWindow(d.hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO info;
  HMONITOR monitor = OuterMonitor(on, p.frame, d.edge, &info);
  if (!monitor) return std::nullopt;
  p.hop = ParksBelow(d) || monitor != on;
  p.monitor = info.rcMonitor;
  p.work = info.rcWork;

  // Not GetDpiForWindow: it reports 96 for a DPI-unaware window on a scaled monitor.
  UINT dpiX, dpiY;
  if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) dpiX = USER_DEFAULT_SCREEN_DPI;
  p.strip = MulDiv(g_settings.revealSize, dpiX, USER_DEFAULT_SCREEN_DPI);
  p.slack = (dpiX + USER_DEFAULT_SCREEN_DPI - 1) / USER_DEFAULT_SCREEN_DPI;
  return p;
}

bool SetTopmost(HWND hwnd, bool topmost) {
  if (SetWindowPos(hwnd, topmost ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS)) {
    return true;
  }
  Wh_Log(L"SetWindowPos(%p, topmost=%d) failed: %lu", hwnd, topmost, GetLastError());
  return false;
}

bool Topmost(HWND hwnd) { return (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0; }

bool Above(HWND a, HWND b) {
  for (HWND w = GetWindow(b, GW_HWNDPREV); w; w = GetWindow(w, GW_HWNDPREV)) {
    if (w == a) return true;
  }
  return false;
}

void RaiseNext() {
  if (g_raise.queue.empty()) return;
  HWND front = g_raise.queue.front();
  Clock::time_point now = Clock::now();
  if (g_raise.sent && !Above(g_raise.sent, front) && now - g_raise.sentAt < kPostTimeout) return;
  g_raise.queue.erase(g_raise.queue.begin());
  // Released while it waited: the raise could land after its topmost is cleared.
  if (std::ranges::none_of(g_docked, [&](const Docked& d) { return d.hwnd == front && d.phase != Phase::Released; })) {
    return;
  }
  SetTopmost(front, true);
  g_raise.sent = front;
  g_raise.sentAt = now;
}

double Progress(const Motion& m, Clock::time_point now) {
  if (m.duration.count() == 0) return 1;
  return std::chrono::duration<double>(now - m.start) / m.duration;
}

bool Near(POINT a, POINT b, int slack = 0) {
  return std::abs(a.x - b.x) <= slack && std::abs(a.y - b.y) <= slack;
}

bool Arrived(const Motion& m) { return Near(m.sent, m.to); }

// A motion stays after its last frame until the window lands, so a busy app
// isn't sent the same slide again while it still holds the last move. An app
// that refuses the position (pulls itself back on screen) gets a fresh slide
// every kResendAfter.
bool MotionStands(const Motion& m, Phase phase, Edge edge, bool landed, Clock::time_point now) {
  if (m.phase != phase || m.edge != edge) return false;
  if (Progress(m, now) < 1 || !Arrived(m)) return true;
  return !landed && now - m.start < m.duration + kResendAfter;
}

bool ShouldPost(const Motion& m, POINT pos, POINT at, int slack, Clock::time_point now) {
  if (Near(pos, m.sent)) return false;
  bool caughtUp = Near(at, m.sent, slack);
  return caughtUp || now - m.sentAt >= kPostTimeout;
}

void Converge(Docked& d) {
  std::optional<Placement> p = Measure(d);
  if (!p) {
    d.motion.reset();
    return;
  }

  // Frozen while dragging: when a drag pulls a tab out of a two-tab group, the
  // window still where the dock was is the one that stays docked.
  if (d.phase != Phase::Dragging) d.lastAt = {p->window.left, p->window.top};
  RECT target = DesiredFrame(p->frame, p->monitor, p->work, d.edge, d.phase, p->strip);
  if (d.phase == Phase::Hidden && ParksBelow(d)) {
    target = ParkedFrame(target, p->monitor);
    // With a monitor there, the group would show on it instead of hiding.
    if (MonitorFromRect(&target, MONITOR_DEFAULTTONULL)) {
      Wh_Log(L"No free space below the monitor to hide group %p docked at the top, released", d.hwnd);
      d.phase = Phase::Released;
      target = DesiredFrame(p->frame, p->monitor, p->work, d.edge, d.phase, p->strip);
    }
  }
  POINT at = {p->frame.left, p->frame.top};
  POINT to = {target.left, target.top};
  bool landed = Near(at, to, p->slack);

  Clock::time_point now = Clock::now();
  if (d.motion && !MotionStands(*d.motion, d.phase, d.edge, landed, now)) d.motion.reset();
  // A hop starts at its target: one jump, still guarded by MotionStands.
  if (!d.motion && !landed) {
    d.motion = Motion{p->hop ? to : at, to, now, g_settings.slide, d.phase, d.edge, at, now};
  }
  if (!d.motion) return;

  POINT pos = SkipTopBand(SlideAt(d.motion->from, d.motion->to, Progress(*d.motion, now)), d.motion->from,
                          d.motion->to, p->frame.bottom - p->frame.top, p->work, p->monitor);
  if (ShouldPost(*d.motion, pos, at, p->slack, now)) {
    if (!SetWindowPos(d.hwnd, nullptr, pos.x + p->window.left - p->frame.left, pos.y + p->window.top - p->frame.top, 0,
                      0, kMoveFlags)) {
      Wh_Log(L"SetWindowPos(%p) failed: %lu", d.hwnd, GetLastError());
    }
    d.motion->sent = pos;
    d.motion->sentAt = now;
  }
}

// Until the last position is posted, which may wait on a slow app.
bool Sliding(const Docked& d) {
  return d.motion && (Progress(*d.motion, Clock::now()) < 1 || !Arrived(*d.motion));
}

bool AnySliding() { return std::ranges::any_of(g_docked, [](const Docked& d) { return Sliding(d); }); }

// Paced by the compositor (4 ms at 240 Hz) rather than the 16 ms timer.
void SlideFrame() {
  DwmFlush();
  for (Docked& d : g_docked) {
    if (Sliding(d)) Converge(d);
  }
  RaiseNext();
}

// Once per tick rather than per frame: the async request takes a frame or more
// to show up in the style.
void KeepTopmost(const Docked& d) {
  if (d.phase != Phase::Released && Movable(d) && !Topmost(d.hwnd)) SetTopmost(d.hwnd, true);
}

// Groupy spreads topmost to every tab and the tab bar, and puts it back on a
// tab cleared alone, so a group is cleared everywhere at once.
void ClearTopmost(const Docked& d) {
  if (IsWindow(d.hwnd)) SetTopmost(d.hwnd, false);
  for (HWND m : d.members) {
    if (m != d.hwnd && IsWindow(m)) SetTopmost(m, false);
  }
  if (HWND bar = PropWindow(d.hwnd, L"GP_LINK")) SetTopmost(bar, false);
}

// 0 reads as no property.
INT_PTR DockMark(Edge edge, bool wasTopmost) { return 1 + static_cast<int>(edge) + 4 * wasTopmost; }

std::optional<std::pair<Edge, bool>> ReadDockMark(INT_PTR mark) {
  if (mark < 1 || mark > 8) return std::nullopt;
  return std::pair{static_cast<Edge>((mark - 1) & 3), ((mark - 1) & 4) != 0};
}

// On the window moved and on every tab of its group, so a tab that outlives
// the others is found too. Once per tick, like KeepTopmost, as tabs join and
// the active one changes.
void KeepMarked(const Docked& d) {
  if (d.phase == Phase::Released) return;
  HANDLE mark = reinterpret_cast<HANDLE>(DockMark(d.edge, d.wasTopmost));
  auto set = [&](HWND hwnd) {
    if (GetProp(hwnd, kDockProp) != mark) SetProp(hwnd, kDockProp, mark);
  };
  set(d.hwnd);
  for (HWND m : d.members) set(m);
}

void Unmark(const Docked& d) {
  RemoveProp(d.hwnd, kDockProp);
  for (HWND m : d.members) RemoveProp(m, kDockProp);
}

void Unclip(Docked& d) {
  if (d.clipping) ClipCursor(nullptr);
  d.clipping = false;
}

// Drops released windows once their last frame is sent, and closed windows.
// A released window keeps its mark until here, so an instance killed in the
// middle of a release leaves a window the next one docks again, not one
// stuck off screen. A group whose active tab closed lives on while another
// tab does: the next sync picks it, also when Groupy dissolved the group
// over the last one.
bool Finished(const Docked& d) {
  if (d.phase == Phase::Released) {
    if (IsWindow(d.hwnd) && Sliding(d)) return false;
    Unmark(d);
    if (!d.wasTopmost) ClearTopmost(d);
    return true;
  }
  if (IsWindow(d.hwnd)) return false;
  bool tabLeft = std::ranges::any_of(d.members, [](HWND m) { return IsWindow(m) != 0; });
  if (d.group && (IsWindow(d.group) || tabLeft)) return false;
  if (d.clipping) ClipCursor(nullptr);
  return true;
}

// Any tab of a docked group, or its tab bar, finds the group.
Docked* Find(HWND hwnd) {
  HWND group = GroupOf(hwnd);
  auto it = std::ranges::find_if(g_docked, [&](const Docked& d) {
    return d.hwnd == hwnd || (group && d.group == group) ||
           (d.group && hwnd == static_cast<HWND>(GetProp(d.hwnd, L"GP_LINK")));
  });
  return it == g_docked.end() ? nullptr : &*it;
}

// Walks the whole owner chain rather than only GA_ROOTOWNER, which skips a
// docked window that is itself owned (e.g. by a hidden app window).
Docked* FindOwning(HWND hwnd) {
  for (HWND w = GetAncestor(hwnd, GA_ROOT); w; w = GetWindow(w, GW_OWNER)) {
    if (Docked* d = Find(w)) return d;
  }
  return nullptr;
}

void UpdateTimer() {
  if (!g_docked.empty() && !g_timer) {
    g_timer = SetTimer(nullptr, 0, kTickMs, nullptr);
  } else if (g_docked.empty() && g_timer) {
    KillTimer(nullptr, g_timer);
    g_timer = 0;
  }
}

// Start, Task View and other shell windows have neither; app windows,
// including ones that draw their own title bar, have at least one.
bool Framed(HWND hwnd) {
  LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
  return (style & WS_CAPTION) == WS_CAPTION || (style & WS_THICKFRAME);
}

bool DesktopOrTaskbar(HWND hwnd) {
  WCHAR className[64] = L"";
  GetClassName(hwnd, className, ARRAYSIZE(className));
  for (PCWSTR shellClass : {L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
    if (_wcsicmp(className, shellClass) == 0) return true;
  }
  return false;
}

enum class Fullscreen { No, Borderless, Exclusive };

// How the window fills `monitor`. Windows reports exclusive full screen, for
// old and flip-model swap chains alike. It reports the same busy state for a
// borderless game as for an ordinary window (all measured), so borderless
// means a window without a frame that covers the monitor to within
// Placement::slack.
Fullscreen FullscreenOn(HWND hwnd, const RECT& monitor, int slack) {
  RECT rect;
  if (!GetWindowRect(hwnd, &rect) || rect.left > monitor.left + slack || rect.top > monitor.top + slack ||
      rect.right < monitor.right - slack || rect.bottom < monitor.bottom - slack) {
    return Fullscreen::No;
  }
  // The desktop covers every monitor. A window with a frame covers its monitor
  // when it is maximized where the taskbar hides itself, or tiled over a
  // monitor without a taskbar.
  if (DesktopOrTaskbar(hwnd) || Framed(hwnd)) return Fullscreen::No;

  QUERY_USER_NOTIFICATION_STATE state;
  bool exclusive = SUCCEEDED(SHQueryUserNotificationState(&state)) && state == QUNS_RUNNING_D3D_FULL_SCREEN;
  return exclusive ? Fullscreen::Exclusive : Fullscreen::Borderless;
}

// Whether the window's app is on the app list. An app whose name can't be
// read, such as one guarded by an anti-cheat driver, counts as not listed.
bool Listed(HWND hwnd) {
  if (g_settings.apps.empty()) return false;

  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!process) return false;
  WCHAR path[MAX_PATH];
  DWORD length = ARRAYSIZE(path);
  bool read = QueryFullProcessImageName(process, 0, path, &length);
  CloseHandle(process);
  if (!read) return false;

  PCWSTR slash = wcsrchr(path, L'\\');
  PCWSTR name = slash ? slash + 1 : path;
  return std::ranges::any_of(g_settings.apps,
                             [&](const std::wstring& app) { return _wcsicmp(app.c_str(), name) == 0; });
}

// A game hides the pointer while the mouse turns the camera (measured in a
// shooter: no cursor, and the pointer confined to a monitor whose edges a quick
// flick still reaches). Touch input suppresses the pointer; that doesn't count.
bool PointerHidden() {
  CURSORINFO info = {sizeof(info)};
  return GetCursorInfo(&info) && !(info.flags & CURSOR_SUPPRESSED) &&
         (!(info.flags & CURSOR_SHOWING) || !info.hCursor);
}

// Whether pointing at a strip leaves its window hidden. The app list
// decides for its apps, the other settings for the rest.
bool StopsReveal(bool listed, Fullscreen fullscreen, bool pointerHidden) {
  if (listed) return !g_settings.whitelist;
  return (fullscreen == Fullscreen::Exclusive && g_settings.hideInExclusive) ||
         (fullscreen == Fullscreen::Borderless && g_settings.hideInBorderless) ||
         (pointerHidden && g_settings.hideWhilePointerHidden);
}

// Returns whether the pointer just revealed the window.
bool Step(Docked& d, POINT pt, HWND foreground, DWORD now) {
  // MOVESIZEEND never comes while the app is stuck in its move loop; the
  // pointer must not stay on the slide line after the button is up.
  // GetAsyncKeyState reads physical buttons, so both cover swapped buttons.
  if (d.clipping && GetAsyncKeyState(VK_LBUTTON) >= 0 && GetAsyncKeyState(VK_RBUTTON) >= 0) Unclip(d);

  std::optional<Placement> p = Measure(d);
  if (!p) return false;

  RECT strip = DesiredFrame(p->frame, p->monitor, p->work, d.edge, Phase::Hidden, p->strip);
  IntersectRect(&strip, &strip, &p->monitor);

  switch (d.phase) {
    case Phase::Hidden:
      if (!PtInRect(&strip, pt)) break;
      // A docked window in front never keeps itself hidden.
      if (FindOwning(foreground) != &d &&
          StopsReveal(Listed(foreground), FullscreenOn(foreground, p->monitor, p->slack), g_pointerHidden)) {
        break;
      }
      d.phase = Phase::Shown;
      return true;

    case Phase::Shown: {
      // The strip counts as inside: with a taskbar on the docked edge it lies
      // outside the shown frame, and a pointer resting there would flicker.
      bool inside = PtInRect(&p->frame, pt) || PtInRect(&strip, pt) || FindOwning(WindowFromPoint(pt)) == &d;
      if (inside || (g_settings.keepFocusedShown && FindOwning(foreground) == &d)) {
        d.leftAt = 0;
      } else if (!d.leftAt) {
        d.leftAt = now ? now : 1;
      } else if (now - d.leftAt >= kHideDelayMs) {
        d.phase = Phase::Hidden;
        d.leftAt = 0;
      }
      break;
    }

    case Phase::Dragging: {
      // The system move loop may set its own clip after our async event.
      RECT clip;
      if (d.clipping && GetClipCursor(&clip) && !EqualRect(&clip, &g_clip)) ClipCursor(&g_clip);
      break;
    }

    case Phase::Released:
      break;
  }
  return false;
}

LONGLONG Area(const RECT& r) { return LONGLONG(r.right - r.left) * (r.bottom - r.top); }

struct OnScreen {
  RECT frame;
  LONGLONG area;
};

// Without a DC the callback gets whole monitor rects, not their overlap with the clip.
BOOL CALLBACK AddOnScreenArea(HMONITOR, HDC, LPRECT monitor, LPARAM param) {
  auto* acc = reinterpret_cast<OnScreen*>(param);
  RECT part;
  if (IntersectRect(&part, monitor, &acc->frame)) acc->area += Area(part);
  return TRUE;
}

LONGLONG OnScreenArea(const RECT& frame) {
  OnScreen acc{frame, 0};
  EnumDisplayMonitors(nullptr, &frame, AddOnScreenArea, reinterpret_cast<LPARAM>(&acc));
  return acc.area;
}

struct Leaver {
  HWND hwnd;
  Edge edge;
};

// Keeps a docked group current: the active tab is the one moved, a window that
// joins moves with the group, and one that leaves is released. A docked
// window that gets grouped brings its group into the dock. When the last other
// tab leaves, Groupy dissolves the group; the window still at the dock stays.
//
// The scan that finds a tab that joined runs when `scan` is set, or when the
// known tabs show a change: one left or closed, or none of them is the active
// one, as when a new tab is. A tab that joins without becoming active is
// found at the next scan.
void SyncGroup(Docked& d, std::vector<Leaver>& leavers, bool scan) {
  if (!d.group) {
    d.group = GroupOf(d.hwnd);
    if (!d.group) return;
  }

  HWND active = ActiveOf(d.members);
  if (!scan && active && std::ranges::all_of(d.members, [&](HWND m) { return GroupOf(m) == d.group; })) {
    d.hwnd = active;
    return;
  }

  std::vector<HWND> now = MembersOf(d.group);
  std::vector<HWND> gone;
  for (HWND m : d.members) {
    if (IsWindow(m) && std::ranges::find(now, m) == now.end()) gone.push_back(m);
  }
  // A tab pulled out of the bar leaves mid-drag (measured), so its drag-end
  // event no longer reaches the group; end the group's drag here.
  if (std::ranges::find(gone, d.hwnd) != gone.end() && d.phase == Phase::Dragging) {
    Unclip(d);
    d.phase = Phase::Shown;
  }

  if (now.empty()) {
    auto distance = [&](HWND m) {
      RECT r{};
      GetWindowRect(m, &r);
      return std::abs(r.left - d.lastAt.x) + std::abs(r.top - d.lastAt.y);
    };
    if (auto stays = std::ranges::min_element(gone, {}, distance); stays != gone.end()) {
      d.hwnd = *stays;
      gone.erase(stays);
    }
    d.group = nullptr;
    d.members.clear();
  } else {
    d.members = std::move(now);
    if (HWND active = ActiveOf(d.members)) d.hwnd = active;
  }

  for (HWND m : gone) leavers.push_back({m, d.edge});
}

// A dragged-out tab sits where it was dropped, so only the topmost and the
// mark the group gave it go; one left off screen slides back first.
void ReleaseLeaver(const Leaver& l) {
  RECT frame;
  if (!GetWindowRect(l.hwnd, &frame)) return;
  if (OnScreenArea(frame) * 2 >= Area(frame)) {
    RemoveProp(l.hwnd, kDockProp);
    SetTopmost(l.hwnd, false);
  } else {
    g_docked.push_back(Unit(l.hwnd, l.edge, Phase::Released));
  }
}

void Tick() {
  std::erase_if(g_docked, Finished);

  static int ticks;
  bool scan = ticks++ % kGroupScanTicks == 0;
  std::vector<Leaver> leavers;
  for (Docked& d : g_docked) {
    if (d.phase != Phase::Released) SyncGroup(d, leavers, scan);
  }
  for (const Leaver& l : leavers) ReleaseLeaver(l);

  POINT pt;
  if (GetCursorPos(&pt)) {
    HWND foreground = GetForegroundWindow();
    DWORD now = GetTickCount();
    std::vector<HWND> revealed;
    for (Docked& d : g_docked) {
      if (Step(d, pt, foreground, now)) revealed.push_back(d.hwnd);
      Converge(d);
      KeepTopmost(d);
      KeepMarked(d);
    }
    std::ranges::sort(revealed, [](HWND a, HWND b) { return Above(b, a); });
    g_raise.queue.insert(g_raise.queue.end(), revealed.begin(), revealed.end());
    // Not read over a hidden window's strip: there the pointer belongs to
    // that window's app and says nothing about the app in front, and a flick
    // that rests on the strip would pass for a pointer that was never hidden.
    Docked* under = FindOwning(WindowFromPoint(pt));
    if (!under || under->phase != Phase::Hidden) g_pointerHidden = PointerHidden();
  }
  RaiseNext();

  UpdateTimer();
}

void Dock(Edge edge) {
  HWND hwnd = GetAncestor(GetForegroundWindow(), GA_ROOT);
  if (!hwnd || hwnd == GetShellWindow() || !IsWindowVisible(hwnd)) return;
  if (DesktopOrTaskbar(hwnd) || !Framed(hwnd)) return;

  if (IsZoomed(hwnd)) ShowWindowAsync(hwnd, SW_RESTORE);

  Docked* d = Find(hwnd);
  if (!d) {
    Docked unit = Unit(hwnd, edge, Phase::Hidden);
    unit.wasTopmost = Topmost(unit.hwnd);
    // Fails for a window we can't move, such as an elevated one.
    if (!SetTopmost(unit.hwnd, true)) return;
    d = &g_docked.emplace_back(std::move(unit));
    UpdateTimer();
  }

  d->edge = edge;
  d->phase = Phase::Hidden;
  d->leftAt = 0;
  KeepMarked(*d);  // not at the next tick: a reload may come first
  Converge(*d);
}

// A minimized or maximized window can't slide, and would restore to the hidden
// spot it was in; move its restore rectangle to the shown spot instead. The
// rectangle is relative to its monitor's work area (measured: off screen
// coordinates by a top taskbar's height). It includes the invisible resize
// borders, so the frame lands a few px off the edge.
void ReleasePlacement(const Docked& d) {
  WINDOWPLACEMENT wp = {sizeof(wp)};
  MONITORINFO info = {sizeof(info)};
  if (!GetWindowPlacement(d.hwnd, &wp) ||
      !GetMonitorInfo(MonitorFromWindow(d.hwnd, MONITOR_DEFAULTTONEAREST), &info)) {
    return;
  }

  LONG dx = info.rcWork.left - info.rcMonitor.left;
  LONG dy = info.rcWork.top - info.rcMonitor.top;
  RECT normal = wp.rcNormalPosition;
  OffsetRect(&normal, dx, dy);
  RECT shown = DesiredFrame(normal, info.rcMonitor, info.rcWork, d.edge, Phase::Shown, 0);
  // A group docked at the top rests its tab bar's height below the edge; on
  // the edge itself Groupy would cut it (see ParksBelow). A parked rectangle
  // goes back up by what it went down, and one under the edge stays.
  if (ParksBelow(d)) {
    shown = normal;
    if (normal.top >= info.rcMonitor.bottom) OffsetRect(&shown, 0, info.rcWork.top - info.rcMonitor.bottom);
  }
  if (EqualRect(&shown, &normal)) return;

  OffsetRect(&shown, -dx, -dy);
  wp.rcNormalPosition = shown;
  wp.flags |= WPF_ASYNCWINDOWPLACEMENT;
  if (wp.showCmd == SW_SHOWMINIMIZED) wp.showCmd = SW_SHOWMINNOACTIVE;
  if (!SetWindowPlacement(d.hwnd, &wp)) Wh_Log(L"SetWindowPlacement(%p) failed: %lu", d.hwnd, GetLastError());
}

void Release(Docked& d) {
  Unclip(d);
  d.phase = Phase::Released;
  if (IsIconic(d.hwnd) || IsZoomed(d.hwnd)) ReleasePlacement(d);
  Converge(d);
}

// Docks again what a reloaded or killed instance left docked, at the edges as
// the screens are now. A window that is on screen, because it was shown or
// was moved back by hand, goes to its edge and hides once the pointer and
// the focus are away. A marked tab brings its whole group.
BOOL CALLBACK AdoptIfMarked(HWND hwnd, LPARAM) {
  auto mark = ReadDockMark(reinterpret_cast<INT_PTR>(GetProp(hwnd, kDockProp)));
  if (!mark || Find(hwnd)) return TRUE;
  Docked unit = Unit(hwnd, mark->first, Phase::Hidden);
  unit.wasTopmost = mark->second;
  // As in Dock: fails for a window this instance can't move.
  if (!SetTopmost(unit.hwnd, true)) return TRUE;
  RECT rect;
  if (GetWindowRect(unit.hwnd, &rect) && OnScreenArea(rect) * 2 >= Area(rect)) unit.phase = Phase::Shown;
  g_docked.push_back(std::move(unit));
  return TRUE;
}

void AdoptMarked() {
  EnumWindows(AdoptIfMarked, 0);
  for (Docked& d : g_docked) Converge(d);
  UpdateTimer();
}

void StartDrag(Docked& started) {
  started.phase = Phase::Dragging;
  started.leftAt = 0;
  started.motion.reset();

  std::optional<Placement> p = Measure(started);
  POINT pt;
  if (!p || !GetCursorPos(&pt)) return;

  // Physical coordinates even for a DPI-unaware window: Windows scales this
  // message for the receiver (measured: a converted point hit-tests as nothing).
  // A drag on a Groupy tab bar runs the active tab's move loop with the pointer
  // above the tab, so it hit-tests as nothing and isn't clipped: that drag may
  // be pulling a tab out of the group, and EndDrag snaps the rest back.
  HWND hwnd = started.hwnd;
  DWORD_PTR hit;
  if (!SendMessageTimeout(hwnd, WM_NCHITTEST, 0, MAKELPARAM(pt.x, pt.y), SMTO_ABORTIFHUNG, 50, &hit) ||
      hit != HTCAPTION) {
    return;
  }
  // MOVESIZEEND can be delivered while SendMessageTimeout waits, and its
  // EndDrag can add a unit, which moves the others in memory, so this one
  // is looked up again.
  Docked* d = Find(hwnd);
  if (!d || d->phase != Phase::Dragging) return;

  // Anchored to the shown position rather than the raw cursor: the event is
  // async, so the window may already be a few px off its edge, and the first
  // clipped move pulls it back.
  const RECT& f = p->frame;
  const RECT& w = p->work;
  RECT shown = DesiredFrame(f, p->monitor, w, d->edge, Phase::Shown, p->strip);
  if (d->edge == Edge::Left || d->edge == Edge::Right) {
    LONG x = pt.x + shown.left - f.left;
    LONG top = pt.y + w.top - f.top;
    g_clip = {x, top, x + 1, std::max(top, pt.y + w.bottom - f.bottom) + 1};
  } else {
    LONG y = pt.y + shown.top - f.top;
    LONG left = pt.x + w.left - f.left;
    g_clip = {left, y, std::max(left, pt.x + w.right - f.right) + 1, y + 1};
  }
  d->clipping = ClipCursor(&g_clip);
}

// Re-reads a group first: a tab dropped outside it has left, and must not be
// snapped back to the edge with the group.
void EndDrag(Docked& d) {
  Unclip(d);
  d.phase = Phase::Shown;
  std::vector<Leaver> leavers;
  SyncGroup(d, leavers, true);
  Converge(d);
  for (const Leaver& l : leavers) ReleaseLeaver(l);  // may grow g_docked; d is not used after
}

void CALLBACK OnWinEvent(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
  if (idObject != OBJID_WINDOW) return;

  if (event == EVENT_SYSTEM_FOREGROUND) {
    Docked* d = FindOwning(hwnd);
    if (d && g_settings.keepFocusedShown && d->phase == Phase::Hidden) {
      d->phase = Phase::Shown;
      Converge(*d);
    }
  } else if (Docked* d = Find(hwnd); d && d->phase != Phase::Released) {
    // A tab just clicked to grab the group is active before the next tick says so.
    if (d->group && GroupOf(hwnd) == d->group) d->hwnd = hwnd;
    if (event == EVENT_SYSTEM_MOVESIZESTART) {
      StartDrag(*d);
    } else {
      EndDrag(*d);
    }
  }

  // Callbacks run inside GetMessage, which doesn't return for them; wake the
  // loop so a slide started here gets its frames now, not at the next tick.
  if (AnySliding()) PostThreadMessage(GetCurrentThreadId(), WM_NULL, 0, 0);
}

void OnAction(WPARAM action) {
  if (g_settings.hotkeys[action].modifiers & (MOD_WIN | MOD_ALT)) SendMaskKey();
  if (action == kReleaseAllAction) {
    for (Docked& d : g_docked) Release(d);
  } else if (action == kReleaseAction) {
    if (Docked* d = FindOwning(GetForegroundWindow())) Release(*d);
  } else {
    Dock(static_cast<Edge>(action));
  }
}

void HandleMessage(const MSG& msg) {
  switch (msg.message) {
    case WM_TIMER:
      Tick();
      break;
    case WM_APP_ACTION:
      OnAction(msg.wParam);
      break;
    case WM_APP_SETTINGS:
      LoadSettings();
      break;
  }
}

// Windhawk unloads the mod the same way for a reload and for a disable. A
// reload starts a new tool process, which holds kNextInstance while it waits
// for this one to exit (Wh_ModInit). So does a duplicate that another
// launcher starts: it takes the windows over the same way, and releases
// them when it is unloaded in turn.
bool NextInstanceWaits() {
  constexpr DWORD kStepMs = 50;
  for (DWORD waited = 0; waited <= kHandoverMs; waited += kStepMs) {
    HANDLE next = OpenMutex(SYNCHRONIZE, FALSE, kNextInstance);
    // Denied: it runs as administrator, this process doesn't.
    if (next || GetLastError() == ERROR_ACCESS_DENIED) {
      if (next) CloseHandle(next);
      return true;
    }
    Sleep(kStepMs);
  }
  return false;
}

DWORD WINAPI WorkerThread(LPVOID readyEvent) {
  SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  MSG msg;
  PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
  LoadSettings();

  HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(nullptr), 0);
  if (!keyboardHook) Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());

  HWINEVENTHOOK eventHooks[] = {
      SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, OnWinEvent, 0, 0,
                      WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS),
      SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, nullptr, OnWinEvent, 0, 0,
                      WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS),
  };
  for (HWINEVENTHOOK hook : eventHooks) {
    if (!hook) Wh_Log(L"SetWinEventHook failed");
  }

  SetEvent(readyEvent);
  AdoptMarked();

  while (true) {
    if (!AnySliding()) {
      if (GetMessage(&msg, nullptr, 0, 0) <= 0) break;
      HandleMessage(msg);
    } else if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) break;
      HandleMessage(msg);
    } else {
      SlideFrame();
    }
  }

  // Unhooked first: the rest pumps no messages, so a live keyboard hook would
  // hold up every keystroke system-wide until it ends.
  for (HWINEVENTHOOK hook : eventHooks) {
    if (hook) UnhookWinEvent(hook);
  }
  if (keyboardHook) UnhookWindowsHookEx(keyboardHook);

  // A release under way ends first, so its window is not docked again.
  for (Docked& d : g_docked) Unclip(d);
  auto releasing = [](const Docked& d) { return d.phase == Phase::Released && Sliding(d); };
  while (std::ranges::any_of(g_docked, releasing)) SlideFrame();
  std::erase_if(g_docked, Finished);

  // A reload leaves the docked windows as they are, marked, for the new
  // instance to dock again.
  if (!g_docked.empty() && NextInstanceWaits()) {
    Wh_Log(L"Leaving %d docked windows to the new instance", static_cast<int>(g_docked.size()));
    return 0;
  }

  // Disabled, or Windhawk is closing: every window slides back on screen,
  // which ends within its duration plus a post timeout or two.
  for (Docked& d : g_docked) Release(d);
  while (AnySliding()) SlideFrame();
  std::erase_if(g_docked, Finished);
  UpdateTimer();
  return 0;
}

BOOL WhTool_ModInit() {
  HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
  g_thread = CreateThread(nullptr, 0, WorkerThread, readyEvent, 0, &g_threadId);
  if (!g_thread) {
    Wh_Log(L"CreateThread failed: %lu", GetLastError());
    CloseHandle(readyEvent);
    return FALSE;
  }
  // The worker's queue must exist before Uninit posts WM_QUIT to it.
  WaitForSingleObject(readyEvent, INFINITE);
  CloseHandle(readyEvent);
  return TRUE;
}

void WhTool_ModSettingsChanged() { PostThreadMessage(g_threadId, WM_APP_SETTINGS, 0, 0); }

void WhTool_ModUninit() {
  PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
  WaitForSingleObject(g_thread, INFINITE);
  CloseHandle(g_thread);
}

//=============================================================================
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk-mods/pull/1916
//
// The mod will load and run in a dedicated windhawk.exe process.
//=============================================================================

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() { ExitThread(0); }

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
    // Changed from the shared tool-mod code: a reload starts this process while
    // the old one still runs. Holding kNextInstance tells the old one to leave
    // its windows docked for this one, which waits for it to exit instead of
    // not running at all. As in the shared code, the test is whether the
    // mutex exists, not who owns it. The thread that creates it exits right
    // away (EntryPoint_Hook), so the mutex reads as abandoned while its
    // process runs on (measured), and a second process that waited for
    // ownership would get it and run alongside the first.
    constexpr DWORD kWaitMs = 3000;
    // After kWaitMs this process no longer says that it waits. An old one
    // that had just seen it has exited by the end of this, so none leaves
    // its windows to a process that then gives up.
    constexpr DWORD kLastChanceMs = 1000;
    constexpr DWORD kStepMs = 50;
    HANDLE next = nullptr;
    for (DWORD waited = 0;; waited += kStepMs) {
      if (waited >= kWaitMs && next) {
        CloseHandle(next);
        next = nullptr;
      } else if (waited < kWaitMs && !next) {
        // Fails while an elevated process holds it, which says the same.
        next = CreateMutex(nullptr, FALSE, kNextInstance);
      }
      g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
      DWORD error = GetLastError();
      // Denied: the process that holds it runs as administrator, this one doesn't.
      bool running = g_toolModProcessMutex ? error == ERROR_ALREADY_EXISTS : error == ERROR_ACCESS_DENIED;
      if (!running) break;
      if (g_toolModProcessMutex) CloseHandle(g_toolModProcessMutex);
      if (waited >= kWaitMs + kLastChanceMs) {
        Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
        ExitProcess(1);
      }
      Sleep(kStepMs);
    }
    if (next) CloseHandle(next);
    if (!g_toolModProcessMutex) {
      Wh_Log(L"CreateMutex failed");
      ExitProcess(1);
    }

    if (!WhTool_ModInit()) {
      ExitProcess(1);
    }

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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

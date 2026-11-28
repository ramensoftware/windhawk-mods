// ==WindhawkMod==
// @id              multi-monitor-window-controls
// @name            Multi-Monitor Window Controls
// @description     Keyboard shortcuts to minimize and restore windows one at a time, jump focus between monitors with an outline flash, swap or move windows between monitors, undo moves, and save and restore window layouts
// @version         2.2.1
// @author          Momin
// @github          https://github.com/mominmansoor
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshcore -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Multi-Monitor Window Controls

Keyboard control over windows on a multi-monitor setup. Every shortcut can be
turned off or changed to other keys in the mod settings.

## Default shortcuts

| Shortcut | What it does |
|---|---|
| Right Ctrl + Down | Minimize the top window on the active monitor, so the one under it shows. Press again for the next one. |
| Right Ctrl + Up | Bring back the last window minimized this way, on top. One per press. |
| Double tap Right Shift | Move focus to the next window you can see, left to right across your monitors. |
| Right Ctrl + Right Shift + Up / Down | Move focus to the next / previous window you can see. |
| Right Ctrl + Left / Right | Move the active window to the monitor on that side. |
| Right Ctrl + Right Shift + Left / Right | Move all windows on the active monitor to the monitor on that side. |
| Right Ctrl + Right Shift + Space | Swap all windows between the active monitor and its neighbour. |
| Right Ctrl + Right Shift + Backspace | Undo the last move, swap or layout restore. Up to 10 steps. |
| Right Ctrl + Right Shift + K | Save the layout of all windows on all monitors. |
| Right Ctrl + Right Shift + L | Restore the saved layout. |
| Right Ctrl + Right Shift + Enter | Maximize the focused window, or put it back (into its snapped spot too). |

When focus switches, a short outline flashes around the window that got it
(or the whole monitor, if you prefer). It can also flash when a window is
moved or restored, so you can see where it went. Color, opacity and width are
settings. The mouse cursor can optionally follow focus too.

## Details

- **Minimize and restore** work like a stack: the windows come back in the
  reverse order they were minimized. Windows closed or restored by hand in the
  meantime are skipped.
- **Focus switching** steps through every window you can see, left to right
  across your monitors and wrapping around. Windows snapped side by side each
  get a turn; windows hidden behind others are skipped. A monitor with nothing
  open is a stop too: the desktop gets focus and the whole monitor is
  outlined. A double
  tap only counts if the key is tapped twice quickly with nothing else pressed
  in between, so typing capital letters doesn't trigger it.
- **Moving a window**: maximized and borderless fullscreen windows stay that
  way on the new monitor. Other windows keep their size (scaled for the
  monitor's DPI), shrink to fit if needed, and are centered. When moving all
  windows, each window keeps its relative position instead, and focus stays on
  the window that had it.
- Left / Right pick the nearest monitor whose center is on that side. Swap
  uses the monitor on the right, or on the left if there's none.
- **Undo** puts every window touched by the last move, swap or layout restore
  back exactly where it was, including snapped and maximized windows.
- **Layouts** remember which monitor every window is on, its position and
  whether it's maximized, and survive restarts. Windows are matched by app
  and title, so they're found again after an app is reopened. Windows that
  aren't open, or belonged to a monitor that's not connected, are skipped.
  Optionally, the layout is restored automatically when a monitor is
  reconnected.

## Similar mods

[Move Window to Monitor](https://windhawk.net/mods/move-window-to-monitor),
[Move and Gather Windows](https://windhawk.net/mods/gather-windows-to-monitor)
and [Snap Commander](https://windhawk.net/mods/snap-commander) can also move
windows between monitors. If that's all you need, use one of them. This mod is
built around the parts they don't have: minimizing and restoring windows one at
a time, switching keyboard focus between monitors (including a double tap key)
with a visible outline, swapping monitors, undoing moves and saved layouts. Moving windows is included so all multi-monitor
shortcuts can share one modifier key, such as Right Ctrl alone.

The move logic started from Move Window to Monitor by TomberWolf. Thanks also
to sultansaad-dev.

## Notes

- The mod uses a low-level keyboard hook, because RegisterHotKey can't tell
  Right Ctrl from Left Ctrl or detect a double tap. A key press is only blocked
  when it matches a shortcut **and** that shortcut has something to do (for
  example, Right Ctrl + Left passes through to the app when there's no monitor
  on the left). Nothing is recorded or sent anywhere.
- Windows doesn't let normal programs control windows running as
  administrator (for example Task Manager), so shortcuts do nothing on those.
- When a shortcut uses Alt, the mod sends an unassigned key (0xE8) so the
  app's menu bar doesn't open when you release Alt.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- minimizeTop:
  - enabled: true
    $name: Enabled
  - key: Down
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: none
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Minimize the top window
  $description: >-
    Default: Right Ctrl + Down. Minimizes the window on top so the one under
    it shows. Press again to minimize the next one.
- restoreMinimized:
  - enabled: true
    $name: Enabled
  - key: Up
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: none
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Restore the last minimized window
  $description: >-
    Default: Right Ctrl + Up. Brings back the windows minimized with the
    shortcut above, one per press, most recent first.
- focusNext:
  - enabled: true
    $name: Enabled
  - key: Up
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Focus the next window
  $description: >-
    Default: Right Ctrl + Right Shift + Up. Moves focus to the next window you
    can see, left to right across your monitors.
- focusPrevious:
  - enabled: true
    $name: Enabled
  - key: Down
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Focus the previous window
  $description: >-
    Default: Right Ctrl + Right Shift + Down. Same as above, in the other
    direction.
- toggleMaximize:
  - enabled: true
    $name: Enabled
  - key: Enter
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Maximize or restore the focused window
  $description: >-
    Default: Right Ctrl + Right Shift + Enter. Maximizes the focused window.
    Press again to put it back, into its snapped spot too.
- moveWindowLeft:
  - enabled: true
    $name: Enabled
  - key: Left
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: none
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Move the window to the left monitor
  $description: >-
    Default: Right Ctrl + Left. Sends the focused window to the monitor on the
    left.
- moveWindowRight:
  - enabled: true
    $name: Enabled
  - key: Right
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: none
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Move the window to the right monitor
  $description: >-
    Default: Right Ctrl + Right. Sends the focused window to the monitor on
    the right.
- moveAllLeft:
  - enabled: true
    $name: Enabled
  - key: Left
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Move all windows to the left monitor
  $description: >-
    Default: Right Ctrl + Right Shift + Left. Sends every window on the
    current monitor to the monitor on the left.
- moveAllRight:
  - enabled: true
    $name: Enabled
  - key: Right
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Move all windows to the right monitor
  $description: >-
    Default: Right Ctrl + Right Shift + Right. Sends every window on the
    current monitor to the monitor on the right.
- swapMonitors:
  - enabled: true
    $name: Enabled
  - key: Space
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Swap monitors
  $description: >-
    Default: Right Ctrl + Right Shift + Space. Swaps all windows between the
    current monitor and its neighbour.
- undoMove:
  - enabled: true
    $name: Enabled
  - key: Backspace
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Undo the last move
  $description: >-
    Default: Right Ctrl + Right Shift + Backspace. Puts windows back where
    they were before the last move, swap or layout restore. Up to 10 steps.
- saveLayout:
  - enabled: true
    $name: Enabled
  - key: K
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Save the window layout
  $description: >-
    Default: Right Ctrl + Right Shift + K. Remembers where every window on
    every monitor is. Kept after restarts.
- restoreLayout:
  - enabled: true
    $name: Enabled
  - key: L
    $name: Key
    $description: >-
      A letter, a digit, Left, Right, Up, Down, PageUp, PageDown, Home, End,
      Insert, Delete, Space, Tab, Enter, Backspace, Escape, F1 to F24 or
      Numpad0 to Numpad9.
  - modifier: rctrl
    $name: Modifier
    $options:
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  - extraModifier: rshift
    $name: Second modifier
    $options:
    - none: None
    - rctrl: Right Ctrl
    - lctrl: Left Ctrl
    - ctrl: Ctrl (either side)
    - ralt: Right Alt
    - lalt: Left Alt
    - alt: Alt (either side)
    - rshift: Right Shift
    - lshift: Left Shift
    - shift: Shift (either side)
  $name: Restore the saved window layout
  $description: >-
    Default: Right Ctrl + Right Shift + L. Puts windows back where the saved
    layout has them.
- doubleTapKey: rshift
  $name: Double tap to switch monitor focus
  $description: >-
    Tap this key twice quickly to move focus to the next visible window. Turn it
    off if an app you use has its own double tap, for example double Shift in
    JetBrains IDEs.
  $options:
  - none: "Off"
  - rshift: Right Shift
  - lshift: Left Shift
  - rctrl: Right Ctrl
  - lctrl: Left Ctrl
  - ralt: Right Alt
  - lalt: Left Alt
- mouseFollowsFocus: off
  $name: Mouse follows focus
  $description: >-
    Move the mouse cursor when focus switches to another monitor or a window
    is restored.
  $options:
  - "off": "Off"
  - monitor: To the center of the monitor
  - window: To the center of the window
- autoRestoreLayout: false
  $name: Restore the saved layout when a monitor is connected
  $description: >-
    A few seconds after a monitor is connected, put windows back where the
    saved layout has them.
- focusOutline:
  - enabled: true
    $name: Flash an outline when focus switches
  - onMoveAndRestore: true
    $name: Also flash when a window is moved or restored
  - target: window
    $name: Outline around
    $options:
    - window: The window
    - monitor: The whole monitor
  - color: white
    $name: Color
    $options:
    - white: White
    - red: Red
    - orange: Orange
    - yellow: Yellow
    - lime: Lime
    - green: Green
    - cyan: Cyan
    - blue: Blue
    - purple: Purple
    - pink: Pink
    - black: Black
  - opacity: 100
    $name: Opacity (%)
    $description: From 10 (faint) to 100 (solid).
  - width: 3
    $name: Width
    $description: >-
      Thickness at 100% display scaling, scaled up on high DPI monitors.
      Recommended 1 to 5.
  $name: Focus outline
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <dwmapi.h>
#include <shellscalingapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <string>
#include <vector>

#ifndef WPF_ASYNCWINDOWPLACEMENT
#define WPF_ASYNCWINDOWPLACEMENT 0x0004
#endif

// ── settings ──────────────────────────────────────────────────────────────────

enum Action {
    kMoveWindowLeft,
    kMoveWindowRight,
    kMoveAllLeft,
    kMoveAllRight,
    kMinimizeTop,
    kRestoreMinimized,
    kFocusNext,
    kFocusPrevious,
    kSwapMonitors,
    kUndoMove,
    kSaveLayout,
    kRestoreLayout,
    kToggleMaximize,
    kActionCount,
};

static const PCWSTR kActionSettingNames[kActionCount] = {
    L"moveWindowLeft", L"moveWindowRight",  L"moveAllLeft", L"moveAllRight",
    L"minimizeTop",    L"restoreMinimized", L"focusNext",   L"focusPrevious",
    L"swapMonitors",   L"undoMove",         L"saveLayout",  L"restoreLayout",
    L"toggleMaximize",
};

// Which side(s) of a modifier family (Ctrl, Shift, Alt) must be held.
enum Side : BYTE { kNone, kLeft, kRight, kEither, kBoth };

struct Shortcut {
    UINT vk = 0;  // 0 = shortcut off
    BYTE ctrl = kNone;
    BYTE shift = kNone;
    BYTE alt = kNone;
};

struct Settings {
    Shortcut shortcuts[kActionCount];
    UINT doubleTapVk = 0;  // 0 = off
    bool outlineEnabled = true;
    COLORREF outlineColor = RGB(255, 255, 255);
    BYTE outlineAlpha = 255;
    int outlineWidth = 3;
    bool outlineOnMove = true;
    bool outlineOnWindow = true;  // false = whole monitor
    int mouseFollow = 0;          // 0 off, 1 monitor center, 2 window center
    bool autoRestoreLayout = false;
};

// Everything below runs on the mod's single worker thread (the keyboard hook,
// the window procedures and the actions), so no locking is needed.
static Settings g_settings;

static void AddModifier(Shortcut& s, PCWSTR name) {
    static const struct {
        PCWSTR name;
        int family;
        BYTE side;
    } table[] = {
        {L"lctrl", 0, kLeft},  {L"rctrl", 0, kRight},  {L"ctrl", 0, kEither},
        {L"lshift", 1, kLeft}, {L"rshift", 1, kRight}, {L"shift", 1, kEither},
        {L"lalt", 2, kLeft},   {L"ralt", 2, kRight},   {L"alt", 2, kEither},
    };
    for (const auto& e : table) {
        if (wcscmp(name, e.name) != 0) continue;
        BYTE& f = e.family == 0 ? s.ctrl : e.family == 1 ? s.shift : s.alt;
        if (f == kNone || f == e.side) {
            f = e.side;
        } else if (f == kEither || e.side == kEither) {
            f = kEither;
        } else {
            f = kBoth;  // left and right of the same family
        }
        return;
    }
}

static UINT ParseKey(PCWSTR name) {
    if (!name[0]) return 0;

    if (!name[1]) {
        const wchar_t c = towupper(name[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) return c;
    }

    static const struct {
        PCWSTR name;
        UINT vk;
    } named[] = {
        {L"Left", VK_LEFT},       {L"Right", VK_RIGHT},
        {L"Up", VK_UP},           {L"Down", VK_DOWN},
        {L"PageUp", VK_PRIOR},    {L"PageDown", VK_NEXT},
        {L"Home", VK_HOME},       {L"End", VK_END},
        {L"Insert", VK_INSERT},   {L"Delete", VK_DELETE},
        {L"Space", VK_SPACE},     {L"Tab", VK_TAB},
        {L"Enter", VK_RETURN},    {L"Backspace", VK_BACK},
        {L"Escape", VK_ESCAPE},
    };
    for (const auto& k : named) {
        if (_wcsicmp(name, k.name) == 0) return k.vk;
    }

    if ((name[0] == L'F' || name[0] == L'f') && iswdigit(name[1])) {
        const int n = _wtoi(name + 1);
        if (n >= 1 && n <= 24) return VK_F1 + n - 1;
    }

    if (_wcsnicmp(name, L"Numpad", 6) == 0 && iswdigit(name[6]) && !name[7]) {
        return VK_NUMPAD0 + (name[6] - L'0');
    }

    Wh_Log(L"Unknown key name \"%ls\"", name);
    return 0;
}

static void LoadSettings() {
    using WindhawkUtils::StringSetting;
    Settings s;

    for (int i = 0; i < kActionCount; i++) {
        const PCWSTR id = kActionSettingNames[i];
        Shortcut& sc = s.shortcuts[i];
        if (!Wh_GetIntSetting(L"%ls.enabled", id)) continue;
        sc.vk = ParseKey(StringSetting::make(L"%ls.key", id));
        AddModifier(sc, StringSetting::make(L"%ls.modifier", id));
        AddModifier(sc, StringSetting::make(L"%ls.extraModifier", id));
    }

    static const struct {
        PCWSTR name;
        UINT vk;
    } tapKeys[] = {
        {L"rshift", VK_RSHIFT},  {L"lshift", VK_LSHIFT}, {L"rctrl", VK_RCONTROL},
        {L"lctrl", VK_LCONTROL}, {L"ralt", VK_RMENU},    {L"lalt", VK_LMENU},
    };
    StringSetting tap = StringSetting::make(L"doubleTapKey");
    for (const auto& t : tapKeys) {
        if (wcscmp(tap, t.name) == 0) s.doubleTapVk = t.vk;
    }

    static const struct {
        PCWSTR name;
        COLORREF color;
    } colors[] = {
        {L"white", RGB(255, 255, 255)}, {L"red", RGB(255, 40, 40)},
        {L"orange", RGB(255, 140, 0)},  {L"yellow", RGB(255, 230, 0)},
        {L"lime", RGB(160, 255, 0)},    {L"green", RGB(0, 200, 70)},
        {L"cyan", RGB(0, 230, 255)},    {L"blue", RGB(40, 100, 255)},
        {L"purple", RGB(160, 70, 255)}, {L"pink", RGB(255, 100, 190)},
        {L"black", RGB(0, 0, 0)},
    };
    s.outlineEnabled = Wh_GetIntSetting(L"focusOutline.enabled") != 0;
    s.outlineOnMove = Wh_GetIntSetting(L"focusOutline.onMoveAndRestore") != 0;
    s.outlineOnWindow =
        wcscmp(StringSetting::make(L"focusOutline.target"), L"monitor") != 0;
    StringSetting mouse = StringSetting::make(L"mouseFollowsFocus");
    s.mouseFollow = wcscmp(mouse, L"monitor") == 0  ? 1
                    : wcscmp(mouse, L"window") == 0 ? 2
                                                    : 0;
    s.autoRestoreLayout = Wh_GetIntSetting(L"autoRestoreLayout") != 0;
    StringSetting color = StringSetting::make(L"focusOutline.color");
    for (const auto& c : colors) {
        if (wcscmp(color, c.name) == 0) s.outlineColor = c.color;
    }
    const int opacity =
        std::clamp(Wh_GetIntSetting(L"focusOutline.opacity"), 10, 100);
    s.outlineAlpha = (BYTE)(255 * opacity / 100);
    s.outlineWidth = std::clamp(Wh_GetIntSetting(L"focusOutline.width"), 1, 20);

    g_settings = s;
}

// ── monitors ──────────────────────────────────────────────────────────────────

struct MonitorInfo {
    HMONITOR hMon;
    RECT rcMon;
    RECT rcWork;
    UINT dpi;
};

// Sorted left to right, then top to bottom. Refreshed before every action and
// when the display layout changes, so the keyboard hook can check it cheaply.
static std::vector<MonitorInfo> g_monitors;

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT, LPARAM) {
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi)) return TRUE;
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    g_monitors.push_back({hMon, mi.rcMonitor, mi.rcWork, dpiX});
    return TRUE;
}

static void RefreshMonitors() {
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, 0);
    std::sort(g_monitors.begin(), g_monitors.end(),
              [](const MonitorInfo& a, const MonitorInfo& b) {
                  if (a.rcMon.left != b.rcMon.left)
                      return a.rcMon.left < b.rcMon.left;
                  return a.rcMon.top < b.rcMon.top;
              });
}

static int FindMonitorIndex(HMONITOR hMon) {
    for (int i = 0; i < (int)g_monitors.size(); i++) {
        if (g_monitors[i].hMon == hMon) return i;
    }
    return -1;
}

// The nearest monitor whose center is to the left (-1) or right (+1) of the
// given monitor's center, or -1 if there is none.
static int FindNeighbor(int cur, int direction) {
    if (cur < 0) return -1;
    const RECT& c = g_monitors[cur].rcMon;
    const int cx = (c.left + c.right) / 2, cy = (c.top + c.bottom) / 2;
    int best = -1, bestScore = 0;
    for (int i = 0; i < (int)g_monitors.size(); i++) {
        const RECT& r = g_monitors[i].rcMon;
        const int dx = (r.left + r.right) / 2 - cx;
        const int dy = (r.top + r.bottom) / 2 - cy;
        if (dx * direction <= 0) continue;
        const int score = abs(dx) + abs(dy);
        if (best < 0 || score < bestScore) {
            best = i;
            bestScore = score;
        }
    }
    return best;
}

// WINDOWPLACEMENT uses workspace coordinates, which are offset from screen
// coordinates by the primary monitor's work area origin.
static POINT GetWorkspaceOffset() {
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY),
                        &mi)) {
        return {0, 0};
    }
    return {mi.rcWork.left, mi.rcWork.top};
}

// ── windows ───────────────────────────────────────────────────────────────────

static bool IsShellClass(HWND hwnd) {
    wchar_t cls[64] = {};
    GetClassName(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
           wcscmp(cls, L"Shell_TrayWnd") == 0 ||
           wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

// Store (UWP) apps such as WhatsApp can report their inner CoreWindow as the
// foreground window. The ApplicationFrameWindow around it is the one that can
// be moved and minimized.
static BOOL CALLBACK FindUwpFrameProc(HWND frame, LPARAM lParam) {
    auto* pair = reinterpret_cast<HWND*>(lParam);  // [0] core, [1] frame
    wchar_t cls[64] = {};
    GetClassName(frame, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"ApplicationFrameWindow") == 0 &&
        FindWindowEx(frame, nullptr, L"Windows.UI.Core.CoreWindow", nullptr) ==
            pair[0]) {
        pair[1] = frame;
        return FALSE;
    }
    return TRUE;
}

static HWND GetActiveTopLevelWindow() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return nullptr;
    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd) return nullptr;

    wchar_t cls[64] = {};
    GetClassName(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        HWND pair[2] = {hwnd, nullptr};
        EnumWindows(FindUwpFrameProc, reinterpret_cast<LPARAM>(pair));
        if (pair[1]) hwnd = pair[1];
    }

    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsShellClass(hwnd)) {
        return nullptr;
    }
    return hwnd;
}

// The monitor of the active window, or the one under the mouse cursor.
static HMONITOR GetSourceMonitor() {
    if (HWND active = GetActiveTopLevelWindow()) {
        return MonitorFromWindow(active, MONITOR_DEFAULTTONEAREST);
    }
    POINT pt;
    if (!GetCursorPos(&pt)) return nullptr;
    return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
}

static bool IsWindowCloaked(HWND hwnd) {
    // Windows on other virtual desktops (and suspended UWP windows) are cloaked.
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked;
}

// A normal, visible app window: not minimized, cloaked, untitled, a tool
// window, the desktop or the taskbar.
static bool IsEligibleWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;
    const LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) && !(ex & WS_EX_APPWINDOW)) return false;
    if (ex & WS_EX_NOACTIVATE) return false;
    if (GetWindowTextLength(hwnd) == 0) return false;
    if (IsWindowCloaked(hwnd) || IsShellClass(hwnd)) return false;
    return true;
}

struct CollectContext {
    HMONITOR hMon;  // nullptr = all monitors
    std::vector<HWND> windows;
};

static BOOL CALLBACK CollectWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<CollectContext*>(lParam);
    if (IsEligibleWindow(hwnd) &&
        (!ctx->hMon ||
         MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) == ctx->hMon)) {
        ctx->windows.push_back(hwnd);
    }
    return TRUE;
}

// Eligible windows on a monitor (or on all monitors for nullptr), top to
// bottom in z-order.
static std::vector<HWND> GetWindows(HMONITOR hMon) {
    CollectContext ctx{hMon, {}};
    EnumWindows(CollectWindowsProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.windows;
}

// The visible edges of a window, without the invisible resize borders.
static RECT GetVisibleRect(HWND hwnd) {
    RECT rc;
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc,
                                     sizeof(rc)))) {
        GetWindowRect(hwnd, &rc);
    }
    return rc;
}

static void ActivateWindow(HWND hwnd) {
    // An empty input event counts as input from this process, which allows it
    // to set the foreground window. It presses no key.
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(hwnd);
}

// ── outline flash and mouse follow ────────────────────────────────────────────

static const wchar_t kFlashClass[] = L"WH_MultiMonitorWindowControls_Flash";
static const UINT_PTR kFlashTimerId = 1;
static const int kFlashHoldTicks = 9;   // about 150 ms
static const int kFlashFadeTicks = 15;  // about 250 ms
static const COLORREF kFlashTransparentColor = RGB(1, 2, 3);
static HWND g_flashWindow;
static int g_flashTick;
static int g_flashThickness;

static LRESULT CALLBACK FlashWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH clear = CreateSolidBrush(kFlashTransparentColor);
            FillRect(dc, &rc, clear);
            DeleteObject(clear);

            const int t = g_flashThickness;
            const RECT edges[] = {
                {rc.left, rc.top, rc.right, rc.top + t},
                {rc.left, rc.bottom - t, rc.right, rc.bottom},
                {rc.left, rc.top, rc.left + t, rc.bottom},
                {rc.right - t, rc.top, rc.right, rc.bottom},
            };
            HBRUSH outline = CreateSolidBrush(g_settings.outlineColor);
            for (const RECT& edge : edges) FillRect(dc, &edge, outline);
            DeleteObject(outline);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_TIMER: {
            g_flashTick++;
            if (g_flashTick > kFlashHoldTicks + kFlashFadeTicks) {
                DestroyWindow(hwnd);
            } else if (g_flashTick > kFlashHoldTicks) {
                const int a = g_settings.outlineAlpha;
                const int faded =
                    a - a * (g_flashTick - kFlashHoldTicks) / kFlashFadeTicks;
                SetLayeredWindowAttributes(hwnd, kFlashTransparentColor,
                                           (BYTE)faded,
                                           LWA_COLORKEY | LWA_ALPHA);
            }
            return 0;
        }
        case WM_DESTROY:
            if (g_flashWindow == hwnd) g_flashWindow = nullptr;
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void FlashOutline(const RECT& rc, UINT dpi) {
    if (g_flashWindow) DestroyWindow(g_flashWindow);

    HWND hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT |
                                   WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
                                   WS_EX_NOACTIVATE,
                               kFlashClass, nullptr, WS_POPUP, rc.left, rc.top,
                               rc.right - rc.left, rc.bottom - rc.top, nullptr,
                               nullptr, GetModuleHandle(nullptr), nullptr);
    if (!hwnd) return;

    g_flashWindow = hwnd;
    g_flashTick = 0;
    g_flashThickness = MulDiv(g_settings.outlineWidth, dpi, 96);
    SetLayeredWindowAttributes(hwnd, kFlashTransparentColor,
                               g_settings.outlineAlpha,
                               LWA_COLORKEY | LWA_ALPHA);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(hwnd);
    SetTimer(hwnd, kFlashTimerId, 16, nullptr);
}

static void FlashMonitor(const MonitorInfo& mon) {
    FlashOutline(mon.rcMon, mon.dpi);
}

// Flashes the outline around a window (or its monitor, per the settings)
// and/or moves the mouse cursor there.
static void HighlightWindow(HWND hwnd, bool flash, bool moveMouse) {
    if (!IsWindow(hwnd)) return;
    RECT visible = GetVisibleRect(hwnd);
    const int idx =
        FindMonitorIndex(MonitorFromRect(&visible, MONITOR_DEFAULTTONEAREST));
    if (idx < 0) return;
    const MonitorInfo& mon = g_monitors[idx];

    if (flash) {
        if (g_settings.outlineOnWindow &&
            IntersectRect(&visible, &visible, &mon.rcMon)) {
            FlashOutline(visible, mon.dpi);
        } else {
            FlashMonitor(mon);
        }
    }

    if (moveMouse && g_settings.mouseFollow) {
        const RECT& r = g_settings.mouseFollow == 2 ? visible : mon.rcWork;
        SetCursorPos((r.left + r.right) / 2, (r.top + r.bottom) / 2);
    }
}

// Moves and restores are asynchronous, so the window's new position is only
// known a moment later. The main window's timer then calls HighlightWindow.
static const UINT_PTR kHighlightTimerId = 2;
static const UINT kHighlightDelayMs = 150;
static HWND g_highlightWindow;
static bool g_highlightFlash;
static bool g_highlightMouse;

static void HighlightWindowLater(HWND timerWnd, HWND hwnd, bool flash,
                                 bool moveMouse) {
    if (!flash && !(moveMouse && g_settings.mouseFollow)) return;
    g_highlightWindow = hwnd;
    g_highlightFlash = flash;
    g_highlightMouse = moveMouse;
    SetTimer(timerWnd, kHighlightTimerId, kHighlightDelayMs, nullptr);
}

// After moving several windows, focus goes back to the window that had it
// (maximizing activates windows), once the moves have been processed.
static const UINT_PTR kRefocusTimerId = 1;
static const UINT kRefocusDelayMs = 300;
static HWND g_refocusWindow;

static void RefocusLater(HWND timerWnd, HWND hwnd) {
    if (!hwnd) return;
    g_refocusWindow = hwnd;
    SetTimer(timerWnd, kRefocusTimerId, kRefocusDelayMs, nullptr);
}

// ── window states and undo ────────────────────────────────────────────────────

struct WindowState {
    HWND hwnd;
    RECT normal;  // restored position, in workspace coordinates
    RECT rect;    // current position, in screen coordinates
    bool maximized;
};

static bool CaptureWindowState(HWND hwnd, WindowState& state) {
    WINDOWPLACEMENT wp = {sizeof(wp)};
    if (IsIconic(hwnd) || !GetWindowPlacement(hwnd, &wp) ||
        !GetWindowRect(hwnd, &state.rect)) {
        return false;
    }
    state.hwnd = hwnd;
    state.normal = wp.rcNormalPosition;
    state.maximized = IsZoomed(hwnd) != FALSE;
    return true;
}

// Puts a window back into a captured state, asynchronously and without
// activating it. Snapped windows are restored by their exact rect.
static void ApplyWindowState(const WindowState& state) {
    HWND hwnd = state.hwnd;
    WINDOWPLACEMENT wp = {sizeof(wp)};
    if (!IsWindow(hwnd) || IsIconic(hwnd) || !GetWindowPlacement(hwnd, &wp)) {
        return;
    }
    wp.flags = WPF_ASYNCWINDOWPLACEMENT;
    wp.rcNormalPosition = state.normal;

    if (state.maximized) {
        // Windows maximizes onto the monitor that holds the restored rect.
        wp.showCmd = SW_SHOWNOACTIVATE;
        SetWindowPlacement(hwnd, &wp);
        wp.showCmd = SW_SHOWMAXIMIZED;
        SetWindowPlacement(hwnd, &wp);
        return;
    }

    if (IsZoomed(hwnd)) {
        wp.showCmd = SW_SHOWNOACTIVATE;
        SetWindowPlacement(hwnd, &wp);
    }
    SetWindowPos(hwnd, nullptr, state.rect.left, state.rect.top,
                 state.rect.right - state.rect.left,
                 state.rect.bottom - state.rect.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

struct UndoStep {
    std::vector<WindowState> windows;
    HWND active;
};

static const size_t kMaxUndoSteps = 10;
static std::vector<UndoStep> g_undoSteps;

static void RecordUndo(const std::vector<HWND>& windows, HWND active) {
    UndoStep step{{}, active};
    for (HWND hwnd : windows) {
        WindowState state;
        if (CaptureWindowState(hwnd, state)) step.windows.push_back(state);
    }
    if (step.windows.empty()) return;
    if (g_undoSteps.size() == kMaxUndoSteps) {
        g_undoSteps.erase(g_undoSteps.begin());
    }
    g_undoSteps.push_back(std::move(step));
}

static void UndoLastMove(HWND timerWnd) {
    if (g_undoSteps.empty()) return;
    const UndoStep step = std::move(g_undoSteps.back());
    g_undoSteps.pop_back();
    Wh_Log(L"Undoing a move of %d window(s)", (int)step.windows.size());
    for (const WindowState& state : step.windows) ApplyWindowState(state);
    RefocusLater(timerWnd, step.active);
}

// ── moving windows ────────────────────────────────────────────────────────────

// All calls are asynchronous, so a hung app can't block the mod.
// bulk: keep the window's relative position and don't activate it.
static void MoveWindowToMonitor(HWND hwnd, const MonitorInfo& src,
                                const MonitorInfo& dst, bool bulk) {
    WINDOWPLACEMENT wp = {sizeof(wp)};
    RECT rcWin;
    if (!GetWindowPlacement(hwnd, &wp) || !GetWindowRect(hwnd, &rcWin)) return;

    const bool maximized = IsZoomed(hwnd) != FALSE;

    // Borderless fullscreen (F11 in a browser, video players): cover the new
    // monitor.
    if (!maximized && EqualRect(&rcWin, &src.rcMon)) {
        SetWindowPos(hwnd, nullptr, dst.rcMon.left, dst.rcMon.top,
                     dst.rcMon.right - dst.rcMon.left,
                     dst.rcMon.bottom - dst.rcMon.top,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
        return;
    }

    const int dstW = dst.rcWork.right - dst.rcWork.left;
    const int dstH = dst.rcWork.bottom - dst.rcWork.top;

    // The restored size if maximized, else the current size, scaled for DPI
    // and shrunk to fit.
    const RECT& rcSize = maximized ? wp.rcNormalPosition : rcWin;
    int w = MulDiv(rcSize.right - rcSize.left, dst.dpi, src.dpi);
    int h = MulDiv(rcSize.bottom - rcSize.top, dst.dpi, src.dpi);
    w = std::min(w, dstW);
    h = std::min(h, dstH);

    int x, y;
    if (bulk) {
        // Map the window's center proportionally onto the new work area.
        const int srcW = std::max(1L, src.rcWork.right - src.rcWork.left);
        const int srcH = std::max(1L, src.rcWork.bottom - src.rcWork.top);
        const int cx = (rcWin.left + rcWin.right) / 2 - src.rcWork.left;
        const int cy = (rcWin.top + rcWin.bottom) / 2 - src.rcWork.top;
        x = dst.rcWork.left + MulDiv(cx, dstW, srcW) - w / 2;
        y = dst.rcWork.top + MulDiv(cy, dstH, srcH) - h / 2;
        x = std::clamp(x, (int)dst.rcWork.left, (int)dst.rcWork.right - w);
        y = std::clamp(y, (int)dst.rcWork.top, (int)dst.rcWork.bottom - h);
    } else {
        x = dst.rcWork.left + (dstW - w) / 2;
        y = dst.rcWork.top + (dstH - h) / 2;
    }

    if (maximized) {
        // Put the restored rect on the new monitor, then maximize: Windows
        // maximizes onto the monitor that holds the restored rect.
        const POINT off = GetWorkspaceOffset();
        wp.rcNormalPosition = {x - off.x, y - off.y, x - off.x + w,
                               y - off.y + h};
        wp.flags |= WPF_ASYNCWINDOWPLACEMENT;
        wp.showCmd = bulk ? SW_SHOWNOACTIVATE : SW_SHOWNORMAL;
        SetWindowPlacement(hwnd, &wp);
        wp.showCmd = SW_SHOWMAXIMIZED;
        SetWindowPlacement(hwnd, &wp);
        return;
    }

    // Applied twice: apps that handle DPI changes can resize themselves right
    // after the first move.
    const HWND insertAfter = bulk ? HWND_TOP : nullptr;
    const UINT flags =
        SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS | (bulk ? 0 : SWP_NOZORDER);
    SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);
    SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);
}

// Moves windows bottom to top, so their stacking order is kept.
static void MoveWindows(const std::vector<HWND>& windows, int src, int dst) {
    for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
        MoveWindowToMonitor(*it, g_monitors[src], g_monitors[dst], true);
    }
}

static void MoveActiveWindow(HWND timerWnd, int direction) {
    HWND hwnd = GetActiveTopLevelWindow();
    if (!hwnd) return;
    const int src =
        FindMonitorIndex(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
    const int dst = FindNeighbor(src, direction);
    if (dst < 0) return;
    Wh_Log(L"Moving the active window from monitor %d to %d", src, dst);
    RecordUndo({hwnd}, hwnd);
    MoveWindowToMonitor(hwnd, g_monitors[src], g_monitors[dst], false);
    HighlightWindowLater(timerWnd, hwnd, g_settings.outlineOnMove, false);
}

static void MoveAllWindows(HWND timerWnd, int direction) {
    const int src = FindMonitorIndex(GetSourceMonitor());
    const int dst = FindNeighbor(src, direction);
    if (dst < 0) return;

    const HWND active = GetActiveTopLevelWindow();
    const auto windows = GetWindows(g_monitors[src].hMon);
    Wh_Log(L"Moving %d window(s) from monitor %d to %d", (int)windows.size(),
           src, dst);
    RecordUndo(windows, active);
    MoveWindows(windows, src, dst);
    RefocusLater(timerWnd, active);
}

// The neighbour on the right, or on the left if there's none.
static int FindSwapPartner(int src) {
    const int right = FindNeighbor(src, 1);
    return right >= 0 ? right : FindNeighbor(src, -1);
}

static void SwapMonitors(HWND timerWnd) {
    const int a = FindMonitorIndex(GetSourceMonitor());
    const int b = FindSwapPartner(a);
    if (b < 0) return;

    const HWND active = GetActiveTopLevelWindow();
    const auto onA = GetWindows(g_monitors[a].hMon);
    const auto onB = GetWindows(g_monitors[b].hMon);
    Wh_Log(L"Swapping monitors %d and %d", a, b);

    std::vector<HWND> all = onA;
    all.insert(all.end(), onB.begin(), onB.end());
    RecordUndo(all, active);

    MoveWindows(onB, b, a);
    MoveWindows(onA, a, b);
    RefocusLater(timerWnd, active);
}

// ── saved layouts ─────────────────────────────────────────────────────────────

// One line per window, fields separated by tabs:
// app path, class, title, maximized, restored rect, current rect.
static const PCWSTR kLayoutValueName = L"layout";
static const size_t kLayoutMaxChars = 1 << 16;
static bool g_hasSavedLayout;

struct WindowIdentity {
    std::wstring app;
    std::wstring cls;
    std::wstring title;
};

static std::wstring Sanitize(std::wstring s) {
    for (wchar_t& c : s) {
        if (c == L'\t' || c == L'\n' || c == L'\r') c = L' ';
    }
    return s;
}

static WindowIdentity GetWindowIdentity(HWND hwnd) {
    WindowIdentity id;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (HANDLE process =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        wchar_t path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageName(process, 0, path, &size)) {
            id.app = path;
        }
        CloseHandle(process);
    }

    wchar_t buffer[256];
    if (GetClassName(hwnd, buffer, ARRAYSIZE(buffer))) id.cls = buffer;
    if (GetWindowText(hwnd, buffer, ARRAYSIZE(buffer))) id.title = buffer;

    id.app = Sanitize(id.app);
    id.cls = Sanitize(id.cls);
    id.title = Sanitize(id.title);
    return id;
}

struct SavedWindow {
    WindowIdentity id;
    WindowState state;  // hwnd unused
};

static void SaveLayout() {
    std::wstring data;
    int count = 0;
    for (HWND hwnd : GetWindows(nullptr)) {
        WindowState s;
        if (!CaptureWindowState(hwnd, s)) continue;
        const WindowIdentity id = GetWindowIdentity(hwnd);
        wchar_t numbers[160];
        swprintf_s(numbers, L"\t%d\t%ld,%ld,%ld,%ld\t%ld,%ld,%ld,%ld\n",
                   s.maximized ? 1 : 0, s.normal.left, s.normal.top,
                   s.normal.right, s.normal.bottom, s.rect.left, s.rect.top,
                   s.rect.right, s.rect.bottom);
        const std::wstring line =
            id.app + L"\t" + id.cls + L"\t" + id.title + numbers;
        if (data.size() + line.size() >= kLayoutMaxChars) break;
        data += line;
        count++;
    }

    if (!Wh_SetStringValue(kLayoutValueName, data.c_str())) {
        Wh_Log(L"Saving the layout failed");
        return;
    }
    g_hasSavedLayout = count > 0;
    Wh_Log(L"Saved a layout of %d window(s)", count);

    // Confirm with a flash on the active monitor.
    if (g_settings.outlineEnabled) {
        const int idx = FindMonitorIndex(GetSourceMonitor());
        if (idx >= 0) FlashMonitor(g_monitors[idx]);
    }
}

static std::vector<SavedWindow> LoadLayout() {
    std::vector<SavedWindow> layout;
    std::vector<wchar_t> buffer(kLayoutMaxChars);
    if (!Wh_GetStringValue(kLayoutValueName, buffer.data(), buffer.size())) {
        return layout;
    }

    const wchar_t* p = buffer.data();
    while (*p) {
        const wchar_t* end = wcschr(p, L'\n');
        const std::wstring line(p, end ? end - p : wcslen(p));
        p = end ? end + 1 : p + line.size();

        // Split into the 6 fields.
        std::wstring fields[6];
        size_t start = 0;
        int n = 0;
        for (; n < 6; n++) {
            const size_t tab = line.find(L'\t', start);
            fields[n] = line.substr(start, tab - start);
            if (tab == std::wstring::npos) break;
            start = tab + 1;
        }
        if (n != 5) continue;

        SavedWindow w{{fields[0], fields[1], fields[2]}, {}};
        RECT& nr = w.state.normal;
        RECT& cr = w.state.rect;
        if (swscanf_s(fields[4].c_str(), L"%ld,%ld,%ld,%ld", &nr.left, &nr.top,
                      &nr.right, &nr.bottom) != 4 ||
            swscanf_s(fields[5].c_str(), L"%ld,%ld,%ld,%ld", &cr.left, &cr.top,
                      &cr.right, &cr.bottom) != 4) {
            continue;
        }
        w.state.maximized = fields[3] == L"1";
        layout.push_back(std::move(w));
    }
    return layout;
}

static void RestoreLayout(HWND timerWnd) {
    std::vector<SavedWindow> layout = LoadLayout();
    if (layout.empty()) return;

    const auto windows = GetWindows(nullptr);
    std::vector<WindowIdentity> ids;
    for (HWND hwnd : windows) ids.push_back(GetWindowIdentity(hwnd));

    // Match each open window to a saved one: first by app, class and title,
    // then by app and class only (titles change, for example browser tabs).
    std::vector<bool> used(layout.size(), false);
    std::vector<int> match(windows.size(), -1);
    for (int pass = 0; pass < 2; pass++) {
        for (size_t i = 0; i < windows.size(); i++) {
            if (match[i] >= 0) continue;
            for (size_t j = 0; j < layout.size(); j++) {
                const WindowIdentity& s = layout[j].id;
                if (used[j] || s.app != ids[i].app || s.cls != ids[i].cls ||
                    (pass == 0 && s.title != ids[i].title)) {
                    continue;
                }
                used[j] = true;
                match[i] = (int)j;
                break;
            }
        }
    }

    std::vector<HWND> moved;
    std::vector<WindowState> targets;
    for (size_t i = 0; i < windows.size(); i++) {
        if (match[i] < 0) continue;
        WindowState target = layout[match[i]].state;
        // Skip windows whose monitor isn't connected.
        if (!MonitorFromRect(&target.rect, MONITOR_DEFAULTTONULL)) continue;
        target.hwnd = windows[i];
        moved.push_back(windows[i]);
        targets.push_back(target);
    }
    if (moved.empty()) return;

    const HWND active = GetActiveTopLevelWindow();
    RecordUndo(moved, active);
    // Bottom to top, so the stacking order is kept.
    for (auto it = targets.rbegin(); it != targets.rend(); ++it) {
        ApplyWindowState(*it);
    }
    Wh_Log(L"Restored the layout of %d window(s)", (int)moved.size());
    RefocusLater(timerWnd, active);
}

// ── minimize and restore one window at a time ─────────────────────────────────

// Windows minimized by the shortcut, most recent last.
static std::vector<HWND> g_minimizedStack;

static bool HasRestorableWindow() {
    for (HWND hwnd : g_minimizedStack) {
        if (IsWindow(hwnd) && IsIconic(hwnd)) return true;
    }
    return false;
}

static void MinimizeTopWindow() {
    HWND top = GetActiveTopLevelWindow();
    if (!top) return;

    // The next window down on the same monitor gets focus.
    HWND next = nullptr;
    for (HWND hwnd :
         GetWindows(MonitorFromWindow(top, MONITOR_DEFAULTTONEAREST))) {
        if (hwnd != top) {
            next = hwnd;
            break;
        }
    }

    if (!ShowWindowAsync(top, next ? SW_SHOWMINNOACTIVE : SW_MINIMIZE)) {
        Wh_Log(L"Could not minimize the window (running as administrator?)");
        return;
    }
    g_minimizedStack.push_back(top);
    if (next) ActivateWindow(next);
}

static void RestoreMinimizedWindow(HWND timerWnd) {
    while (!g_minimizedStack.empty()) {
        HWND hwnd = g_minimizedStack.back();
        g_minimizedStack.pop_back();
        // Skip windows closed or restored by hand in the meantime.
        if (!IsWindow(hwnd) || !IsIconic(hwnd)) continue;
        ShowWindowAsync(hwnd, SW_RESTORE);
        ActivateWindow(hwnd);
        HighlightWindowLater(timerWnd, hwnd, g_settings.outlineOnMove, true);
        return;
    }
}

// ── maximize ──────────────────────────────────────────────────────────────────

// Maximizes the active window, or restores it if it's maximized. Windows
// remembers a snapped window's spot, so it goes back there.
static void ToggleMaximize(HWND timerWnd) {
    HWND hwnd = GetActiveTopLevelWindow();
    if (!hwnd) return;
    if (!ShowWindowAsync(hwnd, IsZoomed(hwnd) ? SW_RESTORE : SW_MAXIMIZE)) {
        Wh_Log(L"Could not maximize the window (running as administrator?)");
        return;
    }
    HighlightWindowLater(timerWnd, hwnd, g_settings.outlineOnMove, false);
}

// ── focus switching ───────────────────────────────────────────────────────────

// Windows you can actually see: at least 40% of the window is on a monitor
// and not covered by windows above it. Side by side (snapped) windows each
// count, a window hidden behind another doesn't. Sorted left to right by
// monitor, then by position on the monitor.
// A monitor with no visible windows is a stop too (hwnd is nullptr): focusing
// it gives focus to the desktop and outlines the monitor.
struct VisibleWindow {
    HWND hwnd;
    int monitor;
    RECT rect;
};

static std::vector<VisibleWindow> GetVisibleWindows() {
    const int kGrid = 5;  // sample points per side
    std::vector<VisibleWindow> visible;
    std::vector<RECT> above;
    for (HWND hwnd : GetWindows(nullptr)) {  // top to bottom
        const RECT rc = GetVisibleRect(hwnd);
        const int w = rc.right - rc.left, h = rc.bottom - rc.top;
        int seen = 0;
        for (int gx = 0; gx < kGrid && w > 0 && h > 0; gx++) {
            for (int gy = 0; gy < kGrid; gy++) {
                const POINT pt = {rc.left + (2 * gx + 1) * w / (2 * kGrid),
                                  rc.top + (2 * gy + 1) * h / (2 * kGrid)};
                if (!MonitorFromPoint(pt, MONITOR_DEFAULTTONULL)) continue;
                bool covered = false;
                for (const RECT& r : above) {
                    if (PtInRect(&r, pt)) {
                        covered = true;
                        break;
                    }
                }
                if (!covered) seen++;
            }
        }
        above.push_back(rc);
        if (seen * 100 >= 40 * kGrid * kGrid) {
            const int mon = FindMonitorIndex(
                MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST));
            visible.push_back({hwnd, mon, rc});
        }
    }
    for (int i = 0; i < (int)g_monitors.size(); i++) {
        if (std::none_of(visible.begin(), visible.end(),
                         [i](const VisibleWindow& v) { return v.monitor == i; })) {
            visible.push_back({nullptr, i, g_monitors[i].rcMon});
        }
    }
    std::sort(visible.begin(), visible.end(),
              [](const VisibleWindow& a, const VisibleWindow& b) {
                  if (a.monitor != b.monitor) return a.monitor < b.monitor;
                  if (a.rect.left != b.rect.left)
                      return a.rect.left < b.rect.left;
                  return a.rect.top < b.rect.top;
              });
    return visible;
}

// The empty monitor that was focused last, to know where the cycle is while
// the desktop has focus.
static HMONITOR g_focusedEmptyMonitor;

// Moves focus to the next (+1) or previous (-1) visible window, left to right
// across all monitors, wrapping around.
static void FocusAdjacentWindow(int direction) {
    const auto visible = GetVisibleWindows();
    const int n = (int)visible.size();
    if (n == 0) return;

    const HWND active = GetActiveTopLevelWindow();
    HWND foreground = GetForegroundWindow();
    const bool desktopFocused =
        !foreground || IsShellClass(GetAncestor(foreground, GA_ROOT));

    int target = -1;
    for (int i = 0; i < n; i++) {
        const bool current =
            visible[i].hwnd
                ? visible[i].hwnd == active
                : desktopFocused &&
                      g_monitors[visible[i].monitor].hMon ==
                          g_focusedEmptyMonitor;
        if (current) {
            target = ((i + direction) % n + n) % n;
            break;
        }
    }

    if (target < 0) {
        // The active window isn't visible (or there's none): go to the first
        // window on the next monitor in that direction.
        const int cur = FindMonitorIndex(GetSourceMonitor());
        if (direction > 0) {
            target = 0;
            for (int i = 0; i < n; i++) {
                if (visible[i].monitor > cur) {
                    target = i;
                    break;
                }
            }
        } else {
            target = n - 1;
            for (int i = n - 1; i >= 0; i--) {
                if (visible[i].monitor < cur) {
                    target = i;
                    break;
                }
            }
        }
    }

    const VisibleWindow& stop = visible[target];
    if (!stop.hwnd) {
        const MonitorInfo& mon = g_monitors[stop.monitor];
        g_focusedEmptyMonitor = mon.hMon;
        if (HWND desktop = GetShellWindow()) ActivateWindow(desktop);
        if (g_settings.outlineEnabled) FlashMonitor(mon);
        if (g_settings.mouseFollow) {
            SetCursorPos((mon.rcWork.left + mon.rcWork.right) / 2,
                         (mon.rcWork.top + mon.rcWork.bottom) / 2);
        }
        return;
    }
    if (stop.hwnd == active) return;
    g_focusedEmptyMonitor = nullptr;
    ActivateWindow(stop.hwnd);
    HighlightWindow(stop.hwnd, g_settings.outlineEnabled, true);
}

// ── keyboard hook ─────────────────────────────────────────────────────────────

static const UINT WM_APP_ACTION = WM_APP + 1;  // wParam: Action
static const UINT WM_APP_RELOAD_SETTINGS = WM_APP + 2;

static std::atomic<HWND> g_mainWindow;
static HHOOK g_keyboardHook;

// The key of a shortcut that fired. Its auto-repeats and key-up are blocked
// too, unless a later key-down of it was passed on.
static DWORD g_blockedKey;

// Double tap state.
static const DWORD kTapMaxHoldMs = 250;  // longest press that counts as a tap
static const DWORD kDoubleTapMs = 400;   // longest gap between the two taps
static bool g_tapKeyDown;
static bool g_tapInterrupted;  // another key was pressed during the tap
static DWORD g_tapPressTime;
static DWORD g_lastTapTime;

static bool KeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

static bool SideMatches(BYTE need, bool left, bool right) {
    switch (need) {
        case kNone:
            return !left && !right;
        case kLeft:
            return left && !right;
        case kRight:
            return right && !left;
        case kEither:
            return left || right;
        default:
            return left && right;
    }
}

// The shortcut's modifiers are held, and no others.
static bool ModifiersMatch(const Shortcut& s) {
    if (KeyDown(VK_LWIN) || KeyDown(VK_RWIN)) return false;

    bool lctrl = KeyDown(VK_LCONTROL);
    const bool ralt = KeyDown(VK_RMENU);
    // AltGr keyboard layouts press a fake Left Ctrl along with Right Alt.
    if (ralt && s.ctrl == kNone && s.alt != kNone && s.alt != kLeft) {
        lctrl = false;
    }

    return SideMatches(s.ctrl, lctrl, KeyDown(VK_RCONTROL)) &&
           SideMatches(s.shift, KeyDown(VK_LSHIFT), KeyDown(VK_RSHIFT)) &&
           SideMatches(s.alt, KeyDown(VK_LMENU), ralt);
}

static int FindMatchingShortcut(DWORD vk) {
    for (int i = 0; i < kActionCount; i++) {
        const Shortcut& s = g_settings.shortcuts[i];
        // A shortcut without modifiers would block normal typing.
        if (s.vk != vk) continue;
        if (s.ctrl == kNone && s.shift == kNone && s.alt == kNone) continue;
        if (ModifiersMatch(s)) return i;
    }
    return -1;
}

// Whether the action would do something right now. If not, the key press is
// passed on to the app (for example Ctrl + Left word navigation when there's
// no monitor on the left).
static bool ActionHasTarget(int action) {
    switch (action) {
        case kMoveWindowLeft:
        case kMoveWindowRight: {
            HWND hwnd = GetActiveTopLevelWindow();
            if (!hwnd) return false;
            const int cur = FindMonitorIndex(
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
            return FindNeighbor(cur, action == kMoveWindowLeft ? -1 : 1) >= 0;
        }
        case kMoveAllLeft:
        case kMoveAllRight:
            return FindNeighbor(FindMonitorIndex(GetSourceMonitor()),
                                action == kMoveAllLeft ? -1 : 1) >= 0;
        case kMinimizeTop:
            return GetActiveTopLevelWindow() != nullptr;
        case kRestoreMinimized:
            return HasRestorableWindow();
        case kFocusNext:
        case kFocusPrevious:
            return GetVisibleWindows().size() >= 2;
        case kSwapMonitors:
            return FindSwapPartner(FindMonitorIndex(GetSourceMonitor())) >= 0;
        case kUndoMove:
            return !g_undoSteps.empty();
        case kSaveLayout:
            return true;
        case kRestoreLayout:
            return g_hasSavedLayout;
        case kToggleMaximize:
            return GetActiveTopLevelWindow() != nullptr;
    }
    return false;
}

// When a blocked key was pressed with Alt held, the app would see Alt pressed
// and released alone and open its menu bar. An unassigned key in between
// prevents that.
static void SendMenuMaskKey() {
    INPUT in[2] = {};
    in[0].type = in[1].type = INPUT_KEYBOARD;
    in[0].ki.wVk = in[1].ki.wVk = 0xE8;
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static void TrackDoubleTap(DWORD vk, bool down, bool up) {
    const UINT tapVk = g_settings.doubleTapVk;
    if (!tapVk) return;

    if (vk != tapVk) {
        if (down) {
            // Any other key cancels a tap in progress or a pending first tap.
            if (g_tapKeyDown) g_tapInterrupted = true;
            g_lastTapTime = 0;
        }
        return;
    }

    if (down) {
        if (!g_tapKeyDown) {  // ignore auto-repeat
            g_tapKeyDown = true;
            g_tapInterrupted = false;
            g_tapPressTime = GetTickCount();
        }
        return;
    }
    if (!up) return;

    g_tapKeyDown = false;
    const DWORD now = GetTickCount();
    if (g_tapInterrupted || now - g_tapPressTime > kTapMaxHoldMs) {
        g_lastTapTime = 0;
    } else if (g_lastTapTime && now - g_lastTapTime <= kDoubleTapMs) {
        g_lastTapTime = 0;
        PostMessage(g_mainWindow, WM_APP_ACTION, kFocusNext, 0);
    } else {
        g_lastTapTime = now;
    }
}

static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    // Ignore synthetic input, including the mod's own.
    if (kb->flags & LLKHF_INJECTED) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const DWORD vk = kb->vkCode;
    const bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    const bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
    TrackDoubleTap(vk, down, up);

    if (up) {
        if (vk == g_blockedKey) {
            g_blockedKey = 0;
            return 1;
        }
    } else if (down) {
        const int action = FindMatchingShortcut(vk);
        if (action >= 0 && vk == g_blockedKey) {
            return 1;  // auto-repeat of a shortcut that already fired
        }
        if (action >= 0 && ActionHasTarget(action)) {
            g_blockedKey = vk;
            PostMessage(g_mainWindow, WM_APP_ACTION, action, 0);
            if (KeyDown(VK_MENU)) SendMenuMaskKey();
            return 1;
        }
        // This key-down is passed on (for example the modifier was released
        // while the key auto-repeats), so its key-up must be passed on too.
        if (vk == g_blockedKey) g_blockedKey = 0;
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ── worker thread ─────────────────────────────────────────────────────────────

static const wchar_t kMainClass[] = L"WH_MultiMonitorWindowControls";

static void RunAction(HWND hwnd, int action) {
    RefreshMonitors();
    switch (action) {
        case kMoveWindowLeft:
            MoveActiveWindow(hwnd, -1);
            break;
        case kMoveWindowRight:
            MoveActiveWindow(hwnd, 1);
            break;
        case kMoveAllLeft:
            MoveAllWindows(hwnd, -1);
            break;
        case kMoveAllRight:
            MoveAllWindows(hwnd, 1);
            break;
        case kMinimizeTop:
            MinimizeTopWindow();
            break;
        case kRestoreMinimized:
            RestoreMinimizedWindow(hwnd);
            break;
        case kFocusNext:
            FocusAdjacentWindow(1);
            break;
        case kFocusPrevious:
            FocusAdjacentWindow(-1);
            break;
        case kSwapMonitors:
            SwapMonitors(hwnd);
            break;
        case kUndoMove:
            UndoLastMove(hwnd);
            break;
        case kSaveLayout:
            SaveLayout();
            break;
        case kRestoreLayout:
            RestoreLayout(hwnd);
            break;
        case kToggleMaximize:
            ToggleMaximize(hwnd);
            break;
    }
}

// When a monitor is connected, Windows shuffles windows around for a moment.
// The saved layout is applied once that has settled.
static const UINT_PTR kAutoRestoreTimerId = 3;
static const UINT kAutoRestoreDelayMs = 3000;

// A hidden top-level window (not message-only, so it also receives display
// change broadcasts).
static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam) {
    switch (msg) {
        case WM_APP_ACTION:
            RunAction(hwnd, (int)wParam);
            return 0;
        case WM_APP_RELOAD_SETTINGS:
            LoadSettings();
            return 0;
        case WM_DISPLAYCHANGE: {
            const size_t before = g_monitors.size();
            RefreshMonitors();
            if (g_settings.autoRestoreLayout && g_hasSavedLayout &&
                g_monitors.size() > before) {
                SetTimer(hwnd, kAutoRestoreTimerId, kAutoRestoreDelayMs,
                         nullptr);
            }
            return 0;
        }
        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETWORKAREA) RefreshMonitors();
            return 0;
        case WM_TIMER:
            KillTimer(hwnd, wParam);
            if (wParam == kRefocusTimerId) {
                if (IsWindow(g_refocusWindow)) {
                    ActivateWindow(g_refocusWindow);
                    HighlightWindow(g_refocusWindow, g_settings.outlineOnMove,
                                    false);
                }
                g_refocusWindow = nullptr;
            } else if (wParam == kHighlightTimerId) {
                HighlightWindow(g_highlightWindow, g_highlightFlash,
                                g_highlightMouse);
                g_highlightWindow = nullptr;
            } else if (wParam == kAutoRestoreTimerId) {
                RefreshMonitors();
                RestoreLayout(hwnd);
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static HANDLE g_thread;
static DWORD g_threadId;

static DWORD WINAPI WorkerThread(LPVOID) {
    LoadSettings();
    RefreshMonitors();
    g_hasSavedLayout = !LoadLayout().empty();

    const HINSTANCE instance = GetModuleHandle(nullptr);
    WNDCLASSEX wc = {sizeof(wc)};
    wc.hInstance = instance;
    wc.lpfnWndProc = MainWndProc;
    wc.lpszClassName = kMainClass;
    RegisterClassEx(&wc);
    wc.lpfnWndProc = FlashWndProc;
    wc.lpszClassName = kFlashClass;
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, kMainClass, nullptr, WS_POPUP,
                               0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    if (hwnd) {
        g_mainWindow = hwnd;
        g_keyboardHook =
            SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, instance, 0);
        if (!g_keyboardHook) {
            Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        }

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0) > 0) {
            DispatchMessage(&msg);
        }

        if (g_keyboardHook) UnhookWindowsHookEx(g_keyboardHook);
        if (g_flashWindow) DestroyWindow(g_flashWindow);
        g_mainWindow = nullptr;
        DestroyWindow(hwnd);
    } else {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
    }

    UnregisterClass(kMainClass, instance);
    UnregisterClass(kFlashClass, instance);
    return 0;
}

BOOL WhTool_ModInit() {
    g_thread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_threadId);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    // Settings are read on the worker thread, where the hook uses them.
    if (HWND hwnd = g_mainWindow) PostMessage(hwnd, WM_APP_RELOAD_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_thread, 3000);
    CloseHandle(g_thread);
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

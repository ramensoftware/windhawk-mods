// ==WindhawkMod==
// @id              win-x-hotcorners
// @name            Win-X Hot Corners
// @description     macOS-style hot corners & edges for Windows with full multi-monitor support — trigger actions instantly when your cursor hits any screen corner or edge
// @version         3.5.0
// @author          lost_husky
// @github          https://github.com/DhakadG
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lcomctl32 -lpowrprof -lshell32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win-X Hot Corners

macOS-style hot corners **and screen edges** for Windows 10/11 with **full
multi-monitor support**.

Instantly trigger configurable actions when your cursor reaches any screen
corner or edge. Configure different actions for each zone on each monitor
independently.

Inspired by [WinXCorners](https://github.com/vhanla/winxcorners), rebuilt as a
Windhawk mod.

## Features

- **Bounded latency** — a dedicated detection thread samples the cursor every
  ~16 ms. Nothing can starve it: not a busy explorer, not an elevated
  foreground app, not a slow action.
- **Zero impact on the rest of the system** — no global mouse hook, so your
  games and apps keep their input path to themselves.
- **Monitors identified by name** — zones bind to a display's friendly name
  (e.g. `Dell U2720Q`), so rearranging your desktop or changing which display
  is primary never reshuffles your configuration.
- **Per-monitor DPI correct** — detection runs per-monitor-DPI-aware, so
  zones land in the right place on mixed-scaling setups.
- **Screen edges** — trigger actions on the top, bottom, left, or right edge
  of any monitor.
- **Configurable zone size**, optional dwell delay, and a cooldown timer.
- **Fullscreen protection** — auto-detects games, presentations, and D3D
  fullscreen apps.
- **Drag protection** — zones don't fire while a mouse button is held.
- **App exclusions** — blacklist processes that need corner/edge clicks.

## Available Actions

**Switching**

| Action | Description |
|--------|-------------|
| Task View | Overview of all windows (Win+Tab) |
| Switch to Last Window | Jump straight back to the previous window (Alt+Tab) |
| Task Switcher | Persistent switcher you can click through (Ctrl+Alt+Tab) |
| Virtual Desktop — Next / Previous / New | Move between or create desktops |

**Windows**

| Action | Description |
|--------|-------------|
| Show Desktop | Toggle desktop visibility |
| Hide Other Windows | Minimize all except the active window (Win+Home) |
| Minimize Active Window | Win+Down |
| Maximize Active Window | Win+Up |
| Snap Window Left / Right | Win+Left / Win+Right |
| Close Active Window | Alt+F4 |

**System**

| Action | Description |
|--------|-------------|
| Start Menu | Open the Start menu |
| Search | Win+S |
| Settings | Win+I |
| File Explorer | Win+E |
| Quick Settings | Win+A |
| Notification Center | Win+N |
| Clipboard History | Win+V |
| Screenshot / Snip | Win+Shift+S |
| Project / Second Screen | Win+P |
| Task Manager | Open Task Manager |
| Mute Volume | Toggle system mute |

**Power**

| Action | Description |
|--------|-------------|
| Lock Computer | Lock the workstation instantly |
| Sleep | Put the computer to sleep |
| Turn Off Monitors | Power off all displays |
| Start Screen Saver | Activate the screen saver |

**Custom**

| Action | Description |
|--------|-------------|
| Virtual Key Press | Send any key combination, or several in sequence |
| Custom Command | Launch any executable, path, or URL |
| Nothing | Disabled (default) |

## Identifying your monitors

Set **Monitor** to the display's friendly name. The exact names for your
displays are written to this mod's log every time it loads or your display
layout changes — open the log and look for lines like:

```
Monitor 1 [PRIMARY] id='Dell U2720Q' device=\\.\DISPLAY1 (0,0)-(3840,2160)
Monitor 2           id='BOE0998'     device=\\.\DISPLAY2 (3840,0)-(5760,1080)
```

Copy the text inside the quotes. If you own two identical displays they get a
` #2`, ` #3` suffix so each stays separately configurable.

Special values:

- `*` — applies to every monitor (use this for one shared configuration)
- *(empty)* — falls back to the numeric **Monitor Number** field, for
  configurations carried over from v2.x

Resolution is **per zone**: a name-matched entry wins over `*` for the zones
it defines, and `*` supplies the rest. So you can put a shared config on `*`
and override just one corner on one display.

## A note on inner corners

Windows lets the pointer cross freely between adjacent monitors, so corners
along a shared boundary have nothing to stop the cursor against. They are hard
to hit on purpose and easy to hit by accident. Prefer the outer perimeter of
your desktop arrangement — the pointer physically stops there, which is what
makes hot corners feel reliable on macOS.

## Virtual Key Press Format

One combination is `Modifier+Key`. Separate several with semicolons and they
are sent **one after another**, not merged into a single chord — `Ctrl+C;Alt+Tab`
sends Ctrl+C, then Alt+Tab.

**Modifiers:** Ctrl, Alt, Shift, Win
**Keys:** A-Z, 0-9, F1-F24, Enter, Space, Tab, Escape, Home, End, Delete,
Left, Right, Up, Down, etc.

**Examples:** `Ctrl+Shift+Esc`, `Alt+F4`, `Win+L`

## Custom Command Format

Any executable path, file, folder, or URL. Environment variables like
`%AppData%` are expanded automatically.

**Examples:**
- `notepad.exe`
- `C:\Windows\System32\cmd.exe`
- `%AppData%\my_script.bat`
- `https://example.com`

Prefix with `uac;` to request elevation: `uac;cmd.exe`

## App Exclusions

Semicolon-separated list of process names (case-insensitive).
Hot corners are disabled when any excluded process is the foreground window.

**Example:** `photoshop.exe;premiere.exe;blender.exe`

## What's New in v3.4.0

- **16 new actions**, so common shortcuts no longer need a Virtual Key Press:
  Switch to Last Window (Alt+Tab), Task Switcher (Ctrl+Alt+Tab), Minimize,
  Maximize, Snap Left/Right, Close Window, File Explorer, Settings, Search,
  Clipboard History, Screenshot, Project, and Virtual Desktop Next/Previous/New.

## What's New in v3.3.0

- **Extended keys are now flagged correctly.** Arrow keys, Home/End/PgUp/PgDn,
  Insert/Delete, the Win keys, right Ctrl/Alt and the media keys sit on the
  E0-prefixed part of the keyboard. Injected without `KEYEVENTF_EXTENDEDKEY`
  they resolve to their numpad twins, so a Virtual Key Press using
  `Win+Right` (snap) or any arrow/navigation key silently did the wrong thing.
- **Unquoted paths containing spaces now launch.** `C:\Program Files\Tool\t.exe -x`
  was split into `C:\Program` plus arguments and failed with file-not-found.
  The executable is now rebuilt token by token until it names a real file.
  Bare commands (`notepad.exe`) are untouched so the shell still resolves them
  via PATH, and quoted paths behave exactly as before.
- **Windows 11 shell surfaces hosted outside explorer.exe** — Start
  (`StartMenuExperienceHost.exe`), Search (`SearchHost.exe`), Action Center —
  are no longer mistaken for fullscreen apps. Matched by exact process name,
  not a `*Host.exe` suffix rule, which would also match ordinary applications.

## What's New in v3.2.0 — stability

Spamming the zones could make the whole desktop stagger. Four causes, all fixed:

- **Stuck modifier keys.** Key injection used to release any modifier you were
  physically holding and re-press it afterwards. If you let go in between, that
  re-press had no matching release and the modifier stayed logically held down
  for the rest of the session — a stuck Win or Ctrl key breaks every app at
  once. Removed: every key the mod presses is now released in the same batch.
- **Burst injection.** A sweep across several zones queued a burst of shell
  commands that the worker replayed back-to-back — four Win+Tab injections in
  40 ms in one report. There is now a hard 250 ms floor between any two
  actions, and the queue holds 2 instead of 8 so a backlog is dropped rather
  than replayed late.
- **Logging on the hot path.** Every trigger wrote to `OutputDebugString`,
  which takes a machine-wide lock — that stalls *other* Windhawk mods, not
  just this one. Per-trigger logging is now off by default (**Verbose
  logging**). Errors and startup info are still always logged.
- **Unsafe teardown.** If a thread was still busy (e.g. blocked on a UAC
  prompt) the mod freed the locks it was using. It now leaks them instead —
  the process exits immediately after, so a leak is free and a use-after-free
  is not.

Also: the display-layout poll ran 5 system calls every 16 ms; it now runs
twice a second, leaving the tick to one cursor read plus a few comparisons.

## What's New in v3.1.0

- **Fixed: zones stopped working while Task View / Start / Search was open.**
  Those are shell surfaces that exactly match the monitor size, so the
  fullscreen-app guard classified them as fullscreen games and suppressed
  every trigger until they closed. v2.x was immune only by accident — it ran
  inside explorer.exe and skipped windows belonging to its own process, and
  Task View *is* explorer.exe. Moving to a dedicated process silently
  invalidated that test. The guard now compares against the shell's process,
  which is what was always meant.
- Removed a 150 ms diagnostic delay per action that could back up the queue.
- Zone sizes are clamped per monitor so the eight zones can never overlap:
  a corner larger than half the screen, or an edge thicker than the corner
  box, previously made one zone shadow another (only the first was reachable).

## What's New in v3.0.2

- **Zones no longer fire when you just pass through them.** A corner cannot be
  reached without crossing the edge strip beside it, so moving into a corner
  fired the edge action ~30 ms before the corner one. With a toggle bound to
  both (Task View, Show Desktop) that opened and instantly re-closed — looking
  exactly like the mod did nothing. The new **Pass-through guard** setting
  (80 ms) requires the cursor to settle, so only the zone you stop in fires.
- `SendInput` failures are now logged instead of silently discarded.
- Each trigger logs the foreground window before and after, so a cancelled
  toggle is visible in the log rather than looking like success.

## What's New in v3.0.1

- Fixed the mod never loading. v3.0.0 kept the `@architecture x86-64` line
  from v2.x, but `windhawk.exe` is a 32-bit process — so the mod was only ever
  built as a 64-bit DLL and Windhawk had nothing to inject. No logs, no zones,
  no sign of life. Tool mods must not restrict architecture.

## What's New in v3.0.0

- **Runs as a dedicated process** instead of injecting into explorer.exe.
  Detection no longer shares a thread with the taskbar — that shared thread
  was why zones fired seconds late or not at all.
- **Polled detection replaces the low-level mouse hook.** The old hook was
  installed on explorer's taskbar thread; whenever that thread was busy,
  Windows skipped the callback and could silently remove the hook. Polling is
  immune to that, to UIPI, and to elevated foreground apps — and it stops
  routing every mouse event on the system through explorer.
- **Monitors identified by friendly name**, not by position in a sorted list.
- **Per-monitor DPI awareness pinned explicitly**, fixing zones landing at
  the wrong coordinates on mixed-scaling multi-monitor setups.
- **Actions run on a worker thread**, so a slow launch can never delay
  detection.
- Display-layout changes are picked up even when Windows doesn't send
  `WM_DISPLAYCHANGE` (docking, monitor wake, RDP reconnect).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CornerSize: 6
  $name: Corner activation size (pixels)
  $description: >-
    How many pixels from the screen corner activate the hot corner.
    Smaller values require more precision; larger values are easier to hit.
- EdgeSize: 6
  $name: Edge activation size (pixels)
  $description: >-
    How many pixels along the screen edge activate the hot edge.
    Only applies to edge zones, not corners.
- ActivationDelay: 0
  $name: Activation delay (ms)
  $description: >-
    Delay in milliseconds before the action triggers. 0 = instant.
    Use 200-500 ms to prevent accidental triggers.
- SettleMs: 80
  $name: Pass-through guard (ms)
  $description: >-
    How long the cursor must stay in a zone before it counts as entered.
    You cannot reach a corner without crossing the edge next to it, so
    without this a fast move into a corner fires the edge action too — and
    if both are a toggle (like Task View) they cancel out and nothing seems
    to happen. 80 ms is imperceptible and blocks pass-through firing.
    Set to 0 only if you use corners or edges but never both.
- ShowMonitorNames: true
  $name: List my monitors in the log
  $description: >-
    Writes your connected displays to this mod's log every time it loads or
    your display layout changes, so you can copy a name straight into the
    Monitor field above instead of guessing it. Harmless to leave on - it
    only writes a few lines, and only when something actually changes.
- VerboseLogging: false
  $name: Verbose logging
  $description: >-
    Log every trigger and key injection. Off by default: these logs go through
    OutputDebugString, which takes a system-wide lock, so logging on every
    trigger can stutter other Windhawk mods. Turn on only while diagnosing.
    Errors and startup information are always logged.
- CooldownMs: 300
  $name: Cooldown between triggers (ms)
  $description: >-
    Minimum time between sequential triggers of the same zone.
    Prevents double-firing when cursor twitches. 0 = no cooldown.
- DisableOnFullscreen: true
  $name: Disable on fullscreen apps
  $description: >-
    Don't trigger hot corners when a fullscreen application (game, video,
    presentation) is the foreground window.
- DisableDuringDrag: true
  $name: Disable during mouse drag
  $description: >-
    Don't trigger hot corners while any mouse button is held down.
- ExcludedProcesses: ""
  $name: Excluded processes (blacklist)
  $description: >-
    Semicolon-separated list of process names. Hot corners are disabled
    when any of these processes is the foreground window.
    Example: photoshop.exe;premiere.exe;blender.exe
- MonitorCorners:
  - - MonitorId: ""
      $name: Monitor
      $description: >-
        The display's friendly name, e.g. Dell U2720Q. The exact names for
        your displays are written to this mod's log every time it loads.
        Use * for all monitors. Leave empty to fall back to the numeric
        Monitor Number field below.
    - Monitor: 0
      $name: Monitor Number (legacy fallback)
      $description: >-
        Only used when Monitor above is left empty. 0 = all monitors,
        1 = primary, 2 = second, etc. Prefer the name field — numbers shift
        when you rearrange displays.
    - TopLeft: ACTION_NOTHING
      $name: Top-Left Corner
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - TopLeftArgs: ""
      $name: Top-Left Args
      $description: >-
        For Virtual Key Press (e.g. Ctrl+Shift+Esc) or Custom Command
        (e.g. notepad.exe) only.
    - TopRight: ACTION_NOTHING
      $name: Top-Right Corner
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - TopRightArgs: ""
      $name: Top-Right Args
      $description: >-
        For Virtual Key Press or Custom Command only.
    - BottomLeft: ACTION_NOTHING
      $name: Bottom-Left Corner
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - BottomLeftArgs: ""
      $name: Bottom-Left Args
      $description: >-
        For Virtual Key Press or Custom Command only.
    - BottomRight: ACTION_NOTHING
      $name: Bottom-Right Corner
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - BottomRightArgs: ""
      $name: Bottom-Right Args
      $description: >-
        For Virtual Key Press or Custom Command only.
    - EdgeTop: ACTION_NOTHING
      $name: Top Edge
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - EdgeTopArgs: ""
      $name: Top Edge Args
      $description: >-
        For Virtual Key Press or Custom Command only.
    - EdgeBottom: ACTION_NOTHING
      $name: Bottom Edge
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - EdgeBottomArgs: ""
      $name: Bottom Edge Args
      $description: >-
        For Virtual Key Press or Custom Command only.
    - EdgeLeft: ACTION_NOTHING
      $name: Left Edge
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - EdgeLeftArgs: ""
      $name: Left Edge Args
      $description: >-
        For Virtual Key Press or Custom Command only.
    - EdgeRight: ACTION_NOTHING
      $name: Right Edge
      $options:
      - ACTION_NOTHING: Nothing
      - ACTION_SHOW_DESKTOP: Show Desktop
      - ACTION_TASK_VIEW: Task View (Win+Tab)
      - ACTION_SCREENSAVER: Start Screen Saver
      - ACTION_MONITORS_OFF: Turn Off Monitors
      - ACTION_QUICK_SETTINGS: Quick Settings (Win+A)
      - ACTION_NOTIFICATION_CENTER: Notification Center (Win+N)
      - ACTION_START_MENU: Start Menu
      - ACTION_HIDE_OTHERS: Hide Other Windows (Win+Home)
      - ACTION_MUTE: Mute Volume
      - ACTION_TASK_MANAGER: Task Manager
      - ACTION_LOCK: Lock Computer
      - ACTION_SLEEP: Sleep
      - ACTION_SWITCH_LAST: Switch to Last Window (Alt+Tab)
      - ACTION_TASK_SWITCHER: Task Switcher (Ctrl+Alt+Tab)
      - ACTION_MINIMIZE: Minimize Active Window (Win+Down)
      - ACTION_MAXIMIZE: Maximize Active Window (Win+Up)
      - ACTION_SNAP_LEFT: Snap Window Left (Win+Left)
      - ACTION_SNAP_RIGHT: Snap Window Right (Win+Right)
      - ACTION_CLOSE_WINDOW: Close Active Window (Alt+F4)
      - ACTION_FILE_EXPLORER: File Explorer (Win+E)
      - ACTION_SETTINGS: Settings (Win+I)
      - ACTION_SEARCH: Search (Win+S)
      - ACTION_CLIPBOARD: Clipboard History (Win+V)
      - ACTION_SCREENSHOT: Screenshot / Snip (Win+Shift+S)
      - ACTION_PROJECT: Project / Second Screen (Win+P)
      - ACTION_VDESK_NEXT: Virtual Desktop - Next
      - ACTION_VDESK_PREV: Virtual Desktop - Previous
      - ACTION_VDESK_NEW: Virtual Desktop - New
      - ACTION_SEND_KEYPRESS: Virtual Key Press
      - ACTION_START_PROCESS: Custom Command
    - EdgeRightArgs: ""
      $name: Right Edge Args
      $description: >-
        For Virtual Key Press or Custom Command only.
  $name: Monitor Corner & Edge Configuration
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <commctrl.h> // windhawk_utils.h needs SUBCLASSPROC from here
#include <initializer_list>
#include <powrprof.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cwctype>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Undocumented Shell command ID for IShellDispatch::ToggleDesktop
#define SHELL_TRAY_TOGGLE_DESKTOP 407

// =====================================================================
// Enums & Types
// =====================================================================

enum class CornerAction
{
    Nothing,
    ShowDesktop,
    TaskView,
    ScreenSaver,
    MonitorsOff,
    QuickSettings,
    NotificationCenter,
    StartMenu,
    HideOthers,
    Mute,
    TaskManager,
    Lock,
    Sleep,
    SwitchLastWindow,
    TaskSwitcher,
    MinimizeWindow,
    MaximizeWindow,
    SnapLeft,
    SnapRight,
    CloseWindow,
    FileExplorer,
    SettingsApp,
    Search,
    ClipboardHistory,
    Screenshot,
    ProjectDisplay,
    VDesktopNext,
    VDesktopPrev,
    VDesktopNew,
    SendKeypress,
    StartProcess,
};

// Zone IDs: 0-3 = corners, 4-7 = edges
enum Zone
{
    ZONE_TOP_LEFT = 0,
    ZONE_TOP_RIGHT = 1,
    ZONE_BOTTOM_LEFT = 2,
    ZONE_BOTTOM_RIGHT = 3,
    ZONE_EDGE_TOP = 4,
    ZONE_EDGE_BOTTOM = 5,
    ZONE_EDGE_LEFT = 6,
    ZONE_EDGE_RIGHT = 7,
    ZONE_COUNT = 8,
};

struct ZoneConfig
{
    CornerAction action = CornerAction::Nothing;
    std::wstring args;
    std::function<void()> executor;
};

struct MonitorZoneConfig
{
    std::wstring monitorId; // friendly name, "*" for all, or "" to use index
    int monitorIndex = 0;   // legacy fallback, only used when monitorId empty
    ZoneConfig zones[ZONE_COUNT];
};

struct MonitorInfo
{
    HMONITOR handle;
    RECT rcMonitor;
    bool isPrimary;
    int index;           // 1-based, legacy ordinal assigned after sorting
    std::wstring device; // GDI name, e.g. \\.\DISPLAY1
    std::wstring id;     // friendly name, suffixed if duplicated
};

// A resolved, ready-to-fire zone. Owns its executor so the whole set can be
// published as an immutable snapshot and read without touching g_settings.
struct HitZone
{
    RECT rect;
    std::function<void()> exec;
    std::wstring label; // "Dell U2720Q TopLeft -> TaskView", for logging
};

// The detection loop reads nothing but this snapshot, so it never touches
// g_settings and never takes a lock on the timing-critical path.
struct ZoneSet
{
    std::vector<HitZone> zones;
    int activationDelay = 0;
    int settleMs = 80;
    int cooldownMs = 0;
    bool disableDuringDrag = true;
};

// =====================================================================
// Globals
// =====================================================================

static struct
{
    int cornerSize = 6;
    int edgeSize = 6;
    int activationDelay = 0;
    int settleMs = 80;
    int cooldownMs = 300;
    bool disableOnFullscreen = true;
    bool disableDuringDrag = true;
    std::vector<std::wstring> excludedProcesses;
    std::vector<MonitorZoneConfig> monitorConfigs;
} g_settings;

// g_settings is written by Windhawk's thread and read by the detection thread
// only when it rebuilds zones. Everything the detection loop needs at runtime
// lives in the published ZoneSet instead, so this is never taken per tick.
static CRITICAL_SECTION g_settingsLock;

// Published zone snapshot. Swapped wholesale on rebuild; readers take a
// shared_ptr copy, so an in-flight tick can never see a half-rebuilt vector.
static CRITICAL_SECTION g_zonesLock;
static std::shared_ptr<const ZoneSet> g_zones;

static std::vector<MonitorInfo> g_monitors; // detection thread only

// Detection thread
static constexpr DWORD kTickMs = 16;
static HANDLE g_hDetectThread = nullptr;
static DWORD g_dwDetectThreadId = 0;
static HWND g_hDetectWnd = nullptr;
static HANDLE g_hStopEvent = nullptr;

// Per-fire logging is opt-in. Wh_Log goes through OutputDebugString, which
// takes a machine-wide mutex; spamming it from a hot path serialises every
// other process that logs — including the other Windhawk mods.
static bool g_verboseLog = false;

// Prints the monitor list at load and on display changes, so names can be
// copied into the Monitor setting instead of guessed. Cheap - once per event.
static bool g_showMonitorNames = true;

// Hard floor between any two actions, whatever the zone or the cooldown
// setting. Actions are user-visible shell operations (Task View, Show Desktop,
// launching a process); replaying a queued burst of them back-to-back is what
// made the desktop stagger. This is a safety limit, not a preference, so it is
// deliberately not configurable.
static constexpr ULONGLONG kMinFireIntervalMs = 250;
static ULONGLONG g_lastAnyFireTick = 0;

// Action worker thread
static HANDLE g_hWorkerThread = nullptr;
static HANDLE g_hWorkEvent = nullptr;
static CRITICAL_SECTION g_queueLock;
static std::deque<HitZone> g_queue;
// Small on purpose: the rate limiter should stop bursts before they queue, so
// a backlog means something went wrong. Dropping is safer than replaying a
// pile of stale shell commands seconds after the user made the gesture.
static constexpr size_t kMaxQueue = 2;

// Detection state (detection thread only)
static int g_activeZone = -1;
static ULONGLONG g_enterTick = 0;
static bool g_firedThisEntry = false;
static std::vector<ULONGLONG> g_lastFireTick;

// Cached display topology, for catching layout changes Windows doesn't
// announce (docking, monitor wake, RDP reconnect).
static int g_topoCount = -1;
static RECT g_topoVirtual = {};

static constexpr UINT WM_APP_REBUILD = WM_APP + 1;

// Forward declarations
static void LoadSettings();
static const wchar_t *ZoneToString(Zone z);
static const wchar_t *ActionToString(CornerAction a);

// =====================================================================
// String Utilities
// =====================================================================

static std::wstring TrimStr(const std::wstring &s)
{
    auto start = s.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos)
        return L"";
    auto end = s.find_last_not_of(L" \t\r\n");
    return s.substr(start, end - start + 1);
}

static std::wstring ToUpperStr(const std::wstring &s)
{
    std::wstring r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::towupper);
    return r;
}

static std::wstring ToLowerStr(const std::wstring &s)
{
    std::wstring r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::towlower);
    return r;
}

// =====================================================================
// DPI
// =====================================================================

// Monitor rects and GetCursorPos return true physical coordinates only when
// the calling thread is per-monitor aware. On a mixed-scaling setup a
// system-aware thread gets virtualized rects instead, which puts the zones at
// the wrong coordinates on the secondary display.
static void PinThreadDpiPerMonitorV2()
{
    using pfnSetThreadDpiAwarenessContext = HANDLE(WINAPI *)(HANDLE);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32)
        return;
    auto pSet = (pfnSetThreadDpiAwarenessContext)GetProcAddress(
        hUser32, "SetThreadDpiAwarenessContext");
    if (pSet)
        pSet((HANDLE)-4); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
}

// =====================================================================
// Key Parsing (for Virtual Key Press action)
// =====================================================================

static const std::unordered_map<std::wstring, UINT> g_modifierMap = {
    {L"ALT", MOD_ALT},
    {L"CTRL", MOD_CONTROL},
    {L"SHIFT", MOD_SHIFT},
    {L"WIN", MOD_WIN},
};

static const std::unordered_map<std::wstring, WORD> g_vkMap = {
    // Letters
    {L"A", 0x41},
    {L"B", 0x42},
    {L"C", 0x43},
    {L"D", 0x44},
    {L"E", 0x45},
    {L"F", 0x46},
    {L"G", 0x47},
    {L"H", 0x48},
    {L"I", 0x49},
    {L"J", 0x4A},
    {L"K", 0x4B},
    {L"L", 0x4C},
    {L"M", 0x4D},
    {L"N", 0x4E},
    {L"O", 0x4F},
    {L"P", 0x50},
    {L"Q", 0x51},
    {L"R", 0x52},
    {L"S", 0x53},
    {L"T", 0x54},
    {L"U", 0x55},
    {L"V", 0x56},
    {L"W", 0x57},
    {L"X", 0x58},
    {L"Y", 0x59},
    {L"Z", 0x5A},
    // Numbers
    {L"0", 0x30},
    {L"1", 0x31},
    {L"2", 0x32},
    {L"3", 0x33},
    {L"4", 0x34},
    {L"5", 0x35},
    {L"6", 0x36},
    {L"7", 0x37},
    {L"8", 0x38},
    {L"9", 0x39},
    // Function keys
    {L"F1", VK_F1},
    {L"F2", VK_F2},
    {L"F3", VK_F3},
    {L"F4", VK_F4},
    {L"F5", VK_F5},
    {L"F6", VK_F6},
    {L"F7", VK_F7},
    {L"F8", VK_F8},
    {L"F9", VK_F9},
    {L"F10", VK_F10},
    {L"F11", VK_F11},
    {L"F12", VK_F12},
    {L"F13", VK_F13},
    {L"F14", VK_F14},
    {L"F15", VK_F15},
    {L"F16", VK_F16},
    {L"F17", VK_F17},
    {L"F18", VK_F18},
    {L"F19", VK_F19},
    {L"F20", VK_F20},
    {L"F21", VK_F21},
    {L"F22", VK_F22},
    {L"F23", VK_F23},
    {L"F24", VK_F24},
    // Common keys
    {L"ENTER", VK_RETURN},
    {L"RETURN", VK_RETURN},
    {L"TAB", VK_TAB},
    {L"SPACE", VK_SPACE},
    {L"BACKSPACE", VK_BACK},
    {L"ESCAPE", VK_ESCAPE},
    {L"ESC", VK_ESCAPE},
    {L"DELETE", VK_DELETE},
    {L"DEL", VK_DELETE},
    {L"INSERT", VK_INSERT},
    {L"HOME", VK_HOME},
    {L"END", VK_END},
    {L"PAGEUP", VK_PRIOR},
    {L"PAGEDOWN", VK_NEXT},
    {L"LEFT", VK_LEFT},
    {L"RIGHT", VK_RIGHT},
    {L"UP", VK_UP},
    {L"DOWN", VK_DOWN},
    {L"PRINTSCREEN", VK_SNAPSHOT},
    {L"PAUSE", VK_PAUSE},
    {L"CAPSLOCK", VK_CAPITAL},
    {L"NUMLOCK", VK_NUMLOCK},
    {L"SCROLLLOCK", VK_SCROLL},
    // Numpad
    {L"NUMPAD0", VK_NUMPAD0},
    {L"NUMPAD1", VK_NUMPAD1},
    {L"NUMPAD2", VK_NUMPAD2},
    {L"NUMPAD3", VK_NUMPAD3},
    {L"NUMPAD4", VK_NUMPAD4},
    {L"NUMPAD5", VK_NUMPAD5},
    {L"NUMPAD6", VK_NUMPAD6},
    {L"NUMPAD7", VK_NUMPAD7},
    {L"NUMPAD8", VK_NUMPAD8},
    {L"NUMPAD9", VK_NUMPAD9},
    {L"MULTIPLY", VK_MULTIPLY},
    {L"ADD", VK_ADD},
    {L"SUBTRACT", VK_SUBTRACT},
    {L"DECIMAL", VK_DECIMAL},
    {L"DIVIDE", VK_DIVIDE},
    // Media
    {L"VOLUMEMUTE", VK_VOLUME_MUTE},
    {L"VOLUMEUP", VK_VOLUME_UP},
    {L"VOLUMEDOWN", VK_VOLUME_DOWN},
    {L"MEDIAPLAYPAUSE", VK_MEDIA_PLAY_PAUSE},
    {L"MEDIANEXT", VK_MEDIA_NEXT_TRACK},
    {L"MEDIAPREV", VK_MEDIA_PREV_TRACK},
    {L"MEDIASTOP", VK_MEDIA_STOP},
    // Specific modifier VKs for keypress use
    {L"LWIN", VK_LWIN},
    {L"RWIN", VK_RWIN},
    {L"LSHIFT", VK_LSHIFT},
    {L"RSHIFT", VK_RSHIFT},
    {L"LCTRL", VK_LCONTROL},
    {L"RCTRL", VK_RCONTROL},
    {L"LALT", VK_LMENU},
    {L"RALT", VK_RMENU},
};

// Parses "Modifier+Key" into VK codes (modifiers first, key last), returning
// one vector per semicolon-separated combo so they can be sent in sequence.
// Flattening them into a single vector would turn "Ctrl+C;Alt+Tab" into one
// Ctrl+Alt+C+Tab chord instead of two separate keystrokes.
static std::vector<std::vector<WORD>> ParseKeyCombo(const std::wstring &input)
{
    std::vector<std::vector<WORD>> allCombos;
    std::wstring remaining = input;

    while (!remaining.empty())
    {
        std::wstring combo;
        auto semi = remaining.find(L';');
        if (semi != std::wstring::npos)
        {
            combo = TrimStr(remaining.substr(0, semi));
            remaining = remaining.substr(semi + 1);
        }
        else
        {
            combo = TrimStr(remaining);
            remaining.clear();
        }

        if (combo.empty())
            continue;

        std::vector<WORD> modifiers;
        WORD mainKey = 0;

        // Split on '+'
        size_t start = 0;
        while (start < combo.size())
        {
            auto plus = combo.find(L'+', start);
            std::wstring token;
            if (plus != std::wstring::npos)
            {
                token = TrimStr(combo.substr(start, plus - start));
                start = plus + 1;
            }
            else
            {
                token = TrimStr(combo.substr(start));
                start = combo.size();
            }

            if (token.empty())
                continue;
            std::wstring upper = ToUpperStr(token);

            // Check if it's a modifier
            auto modIt = g_modifierMap.find(upper);
            if (modIt != g_modifierMap.end())
            {
                if (modIt->second == MOD_CONTROL)
                    modifiers.push_back(VK_LCONTROL);
                else if (modIt->second == MOD_ALT)
                    modifiers.push_back(VK_LMENU);
                else if (modIt->second == MOD_SHIFT)
                    modifiers.push_back(VK_LSHIFT);
                else if (modIt->second == MOD_WIN)
                    modifiers.push_back(VK_LWIN);
                continue;
            }

            // Check VK map
            auto vkIt = g_vkMap.find(upper);
            if (vkIt != g_vkMap.end())
            {
                mainKey = vkIt->second;
            }
            else
            {
                Wh_Log(L"Unknown key token: '%s'", token.c_str());
            }
        }

        std::vector<WORD> keys = modifiers;
        if (mainKey)
            keys.push_back(mainKey);
        if (!keys.empty())
            allCombos.push_back(std::move(keys));
    }

    return allCombos;
}

// =====================================================================
// Monitor Identity
// =====================================================================

// Maps GDI device name (\\.\DISPLAY1) -> monitor friendly name.
// Friendly names come from the display's EDID and are stable across reboots,
// unlike enumeration order, which changes whenever displays are rearranged or
// a different display is made primary.
static void QueryMonitorFriendlyNames(
    std::unordered_map<std::wstring, std::wstring> &out)
{
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                    &modeCount) != ERROR_SUCCESS)
    {
        Wh_Log(L"GetDisplayConfigBufferSizes failed");
        return;
    }

    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                           &modeCount, modes.data(), nullptr) != ERROR_SUCCESS)
    {
        Wh_Log(L"QueryDisplayConfig failed");
        return;
    }

    for (UINT32 i = 0; i < pathCount; i++)
    {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME src = {};
        src.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        src.header.size = sizeof(src);
        src.header.adapterId = paths[i].sourceInfo.adapterId;
        src.header.id = paths[i].sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&src.header) != ERROR_SUCCESS)
            continue;

        DISPLAYCONFIG_TARGET_DEVICE_NAME tgt = {};
        tgt.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        tgt.header.size = sizeof(tgt);
        tgt.header.adapterId = paths[i].targetInfo.adapterId;
        tgt.header.id = paths[i].targetInfo.id;
        if (DisplayConfigGetDeviceInfo(&tgt.header) != ERROR_SUCCESS)
            continue;

        std::wstring name = tgt.monitorFriendlyDeviceName;

        // Internal panels (laptop screens) often report an empty friendly
        // name. Fall back to the model token in the device path, which looks
        // like \\?\DISPLAY#BOE0998#5&...
        if (name.empty())
        {
            std::wstring path = tgt.monitorDevicePath;
            size_t first = path.find(L'#');
            if (first != std::wstring::npos)
            {
                size_t second = path.find(L'#', first + 1);
                if (second != std::wstring::npos)
                    name = path.substr(first + 1, second - first - 1);
            }
        }

        if (!name.empty())
            out[src.viewGdiDeviceName] = name;
    }
}

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT,
                                     LPARAM lParam)
{
    auto *monitors = reinterpret_cast<std::vector<MonitorInfo> *>(lParam);
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, (MONITORINFO *)&mi))
    {
        MonitorInfo info;
        info.handle = hMonitor;
        info.rcMonitor = mi.rcMonitor;
        info.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        info.index = 0;
        info.device = mi.szDevice;
        monitors->push_back(std::move(info));
    }
    return TRUE;
}

// Must run on the detection thread (per-monitor DPI context).
static void RefreshMonitors()
{
    g_monitors.clear();
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        (LPARAM)&g_monitors);

    // Ordinals are kept only as a legacy fallback for v2.x configs.
    std::sort(g_monitors.begin(), g_monitors.end(),
              [](const MonitorInfo &a, const MonitorInfo &b)
              {
                  if (a.isPrimary != b.isPrimary)
                      return a.isPrimary;
                  if (a.rcMonitor.left != b.rcMonitor.left)
                      return a.rcMonitor.left < b.rcMonitor.left;
                  return a.rcMonitor.top < b.rcMonitor.top;
              });

    std::unordered_map<std::wstring, std::wstring> friendly;
    QueryMonitorFriendlyNames(friendly);

    std::unordered_map<std::wstring, int> nameUses;
    for (int i = 0; i < (int)g_monitors.size(); i++)
    {
        MonitorInfo &m = g_monitors[i];
        m.index = i + 1;

        auto it = friendly.find(m.device);
        m.id = (it != friendly.end()) ? it->second : m.device;

        // Two identical displays report the same EDID name. Suffix the
        // duplicates so each keeps its own configurable identity.
        int n = ++nameUses[ToLowerStr(m.id)];
        if (n > 1)
            m.id += L" #" + std::to_wstring(n);
    }

    if (!g_showMonitorNames)
        return;

    // Printed so the name can be copied straight into the Monitor setting,
    // rather than guessed. Once per load and per display change only.
    Wh_Log(L" ");
    Wh_Log(L"+-- Your monitors ---------------------------------------");
    Wh_Log(L"|  Copy a name below into this mod's \"Monitor\" setting.");
    Wh_Log(L"|  Use  *  to apply one configuration to every monitor.");
    Wh_Log(L"|");
    for (const auto &m : g_monitors)
    {
        Wh_Log(L"|   %d. \"%s\"%s   %ld x %ld  at (%ld, %ld)", m.index,
               m.id.c_str(), m.isPrimary ? L"   [primary]" : L"",
               m.rcMonitor.right - m.rcMonitor.left,
               m.rcMonitor.bottom - m.rcMonitor.top, m.rcMonitor.left,
               m.rcMonitor.top);
    }
    Wh_Log(L"+--------------------------------------------------------");
    Wh_Log(L" ");
}

// =====================================================================
// Fullscreen / Exclusion Gates (worker thread only)
// =====================================================================

// Task View, the Start menu, Search, Action Center and the desktop are shell
// surfaces that are legitimately monitor-sized, so the bounds test below
// mistakes them for fullscreen apps and blocks every trigger while they are
// open. v2.x excluded them with `fgPid == GetCurrentProcessId()` because it
// ran inside explorer.exe; once the mod moved to its own process that test
// silently stopped matching anything. Compare against the *shell's* pid
// instead, which is what was actually meant.
// Lowercase exe name of the window's owning process, or empty on failure.
static std::wstring ProcessNameOfWindow(HWND hwnd)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0)
        return L"";

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc)
        return L"";

    WCHAR exePath[MAX_PATH];
    DWORD pathLen = MAX_PATH;
    BOOL ok = QueryFullProcessImageNameW(hProc, 0, exePath, &pathLen);
    CloseHandle(hProc);
    if (!ok)
        return L"";

    const wchar_t *fileName = wcsrchr(exePath, L'\\');
    return fileName ? fileName + 1 : exePath;
}

static bool IsShellUiWindow(HWND hwnd)
{
    // Not cached: explorer restarts change the pid, and this only runs when
    // a zone actually fires.
    HWND hShell = GetShellWindow();
    if (hShell)
    {
        DWORD shellPid = 0, fgPid = 0;
        GetWindowThreadProcessId(hShell, &shellPid);
        GetWindowThreadProcessId(hwnd, &fgPid);
        if (shellPid && fgPid == shellPid)
            return true;
    }

    WCHAR cls[64];
    if (GetClassName(hwnd, cls, ARRAYSIZE(cls)) <= 0)
        return false;

    // Belt and braces for shell UI hosted outside explorer.exe.
    static const wchar_t *kShellClasses[] = {
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"WorkerW",
        L"Progman",
        L"MultitaskingViewFrame",         // Task View, Windows 10
        L"XamlExplorerHostIslandWindow",  // Task View / Alt-Tab, Windows 11
    };
    for (const wchar_t *k : kShellClasses)
    {
        if (_wcsicmp(cls, k) == 0)
            return true;
    }

    // Windows 11 moved several shell surfaces out of explorer.exe, so the
    // shell-pid test above misses them. Matched by exact process name rather
    // than a "*Host.exe" suffix rule, which would also swallow ordinary
    // applications that happen to be named that way.
    //
    // Deliberately NOT keyed on the Windows.UI.Core.CoreWindow class: every
    // UWP app uses it, so excluding that class would stop a genuinely
    // fullscreen UWP game or video player from being detected as fullscreen.
    static const wchar_t *kShellHostProcesses[] = {
        L"StartMenuExperienceHost.exe",
        L"SearchHost.exe",
        L"SearchApp.exe",
        L"ShellExperienceHost.exe",
        L"ShellHost.exe",
        L"TextInputHost.exe",
    };
    std::wstring proc = ProcessNameOfWindow(hwnd);
    if (!proc.empty())
    {
        for (const wchar_t *p : kShellHostProcesses)
        {
            if (_wcsicmp(proc.c_str(), p) == 0)
                return true;
        }
    }

    return false;
}

static bool IsFullScreenAppActive()
{
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)))
    {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN ||
            state == QUNS_PRESENTATION_MODE)
            return true;
    }

    // Fallback: exact bounds match for apps that go fullscreen without
    // D3D exclusive mode (browser F11, video players).
    HWND hFgWnd = GetForegroundWindow();
    if (!hFgWnd || hFgWnd == GetDesktopWindow() || hFgWnd == GetShellWindow())
        return false;

    if (IsShellUiWindow(hFgWnd))
        return false;

    RECT rcWnd;
    if (!GetWindowRect(hFgWnd, &rcWnd))
        return false;

    HMONITOR hMon = MonitorFromWindow(hFgWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    // True fullscreen apps match the monitor exactly. Maximized desktop
    // apps overhang by ~8px due to DWM drop shadows — they won't match.
    return (rcWnd.left == mi.rcMonitor.left &&
            rcWnd.top == mi.rcMonitor.top &&
            rcWnd.right == mi.rcMonitor.right &&
            rcWnd.bottom == mi.rcMonitor.bottom);
}

static bool IsForegroundAppExcluded(const std::vector<std::wstring> &excluded)
{
    if (excluded.empty())
        return false;

    HWND hFg = GetForegroundWindow();
    if (!hFg)
        return false;

    // OpenProcess + QueryFullProcessImageName is the expensive part and the
    // foreground window rarely changes between triggers, so cache the
    // resolved name — not the verdict, which would go stale when the
    // blacklist is edited.
    static HWND cachedHwnd = nullptr;
    static std::wstring cachedName;

    if (hFg != cachedHwnd)
    {
        cachedHwnd = hFg;
        cachedName.clear();

        DWORD pid = 0;
        GetWindowThreadProcessId(hFg, &pid);
        if (pid == 0)
            return false;

        HANDLE hProc =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!hProc)
            return false;

        WCHAR exePath[MAX_PATH];
        DWORD pathLen = MAX_PATH;
        BOOL ok = QueryFullProcessImageNameW(hProc, 0, exePath, &pathLen);
        CloseHandle(hProc);
        if (!ok)
            return false;

        const wchar_t *fileName = wcsrchr(exePath, L'\\');
        cachedName = fileName ? fileName + 1 : exePath;
    }

    if (cachedName.empty())
        return false;

    for (const auto &name : excluded)
    {
        if (_wcsicmp(cachedName.c_str(), name.c_str()) == 0)
        {
            if (g_verboseLog)
                Wh_Log(L"[EXCLUDE] Foreground app '%s' is blacklisted",
                       cachedName.c_str());
            return true;
        }
    }
    return false;
}

// =====================================================================
// Action Implementations
// =====================================================================

// Sends key-down for all VKs in order, then key-up in reverse order, as one
// atomic SendInput batch.
//
// Earlier versions also released any modifier the user was physically holding
// and re-pressed it afterwards. That was removed deliberately: re-pressing is
// only correct if the key is still held when the restore runs. If the user let
// go in between — or the restore SendInput failed, or the thread was torn down
// between the two calls — a key-down was injected with no matching key-up and
// the modifier stayed logically stuck down for the whole session. A stuck Win
// or Ctrl key breaks every application at once, which is far worse than the
// problem it solved (a held Shift leaking into the injected combo).
// Every key this function presses, it releases in the same batch.
// Keys that live on the E0-prefixed part of the keyboard. Injected without
// KEYEVENTF_EXTENDEDKEY they resolve to their numpad twins, so Win+Right
// (snap) or a custom combo using arrows/Home/End would do nothing or move the
// caret instead. Verified that adding the flag to VK_LWIN does not disturb
// Win+Tab, which is the combo already in use.
static bool IsExtendedKey(WORD vk)
{
    switch (vk)
    {
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_LEFT:
    case VK_RIGHT:
    case VK_UP:
    case VK_DOWN:
    case VK_LWIN:
    case VK_RWIN:
    case VK_APPS:
    case VK_DIVIDE:
    case VK_NUMLOCK:
    case VK_RMENU:
    case VK_RCONTROL:
    case VK_SNAPSHOT:
    case VK_VOLUME_MUTE:
    case VK_VOLUME_DOWN:
    case VK_VOLUME_UP:
    case VK_MEDIA_NEXT_TRACK:
    case VK_MEDIA_PREV_TRACK:
    case VK_MEDIA_STOP:
    case VK_MEDIA_PLAY_PAUSE:
        return true;
    default:
        return false;
    }
}

static void SendKeys(const std::vector<WORD> &vks)
{
    if (vks.empty())
        return;

    size_t n = vks.size();
    std::vector<INPUT> inputs(n * 2);

    for (size_t i = 0; i < n; i++)
    {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = vks[i];
        inputs[i].ki.dwFlags =
            IsExtendedKey(vks[i]) ? KEYEVENTF_EXTENDEDKEY : 0;
    }
    for (size_t i = 0; i < n; i++)
    {
        // The flag must be derived from the key this entry actually releases,
        // not from vks[i] — the release order is reversed, so using the wrong
        // index would tag the extended bit onto the wrong key.
        WORD vk = vks[n - 1 - i];
        inputs[n + i].type = INPUT_KEYBOARD;
        inputs[n + i].ki.wVk = vk;
        inputs[n + i].ki.dwFlags =
            (IsExtendedKey(vk) ? KEYEVENTF_EXTENDEDKEY : 0) | KEYEVENTF_KEYUP;
    }

    // Failures are always reported — a short SendInput means the action
    // silently did nothing. Success is logged only under verbose logging;
    // see g_verboseLog for why this is not on by default.
    SetLastError(0);
    UINT sent = SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
    if (sent != inputs.size())
    {
        Wh_Log(L"SendInput FAILED: sent %u/%u, err=%lu (sizeof(INPUT)=%d)",
               sent, (UINT)inputs.size(), GetLastError(), (int)sizeof(INPUT));
    }
    else if (g_verboseLog)
    {
        Wh_Log(L"SendInput ok: %u events, first vk=0x%02X", sent,
               (unsigned)vks[0]);
    }
}

// Convenience overload for initializer lists
static void SendKeys(std::initializer_list<WORD> vks)
{
    std::vector<WORD> v(vks);
    SendKeys(v);
}

static void ActionShowDesktop()
{
    // Re-found when the handle goes stale, so this survives explorer restarts.
    static HWND hTray = nullptr;
    if (!hTray || !IsWindow(hTray))
        hTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTray)
    {
        Wh_Log(L"Show Desktop: Shell_TrayWnd not found");
        return;
    }
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hTray, WM_COMMAND,
                        MAKELONG(SHELL_TRAY_TOGGLE_DESKTOP, 0), 0,
                        SMTO_ABORTIFHUNG, 2000, &result);
}

static void ActionTaskView() { SendKeys({VK_LWIN, VK_TAB}); }

static void ActionScreenSaver()
{
    HWND hTarget = GetForegroundWindow();
    if (!hTarget)
        hTarget = GetDesktopWindow();
    PostMessage(hTarget, WM_SYSCOMMAND, SC_SCREENSAVE, 0);
}

static void ActionMonitorsOff()
{
    HWND hTarget = GetForegroundWindow();
    if (!hTarget)
        hTarget = GetDesktopWindow();
    PostMessage(hTarget, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
}

static void ActionQuickSettings() { SendKeys({VK_LWIN, 'A'}); }
static void ActionNotificationCenter() { SendKeys({VK_LWIN, 'N'}); }
static void ActionStartMenu() { SendKeys({VK_LWIN}); }
static void ActionHideOthers() { SendKeys({VK_LWIN, VK_HOME}); }
static void ActionMute() { SendKeys({VK_VOLUME_MUTE}); }

static void ActionTaskManager()
{
    WCHAR sysDir[MAX_PATH];
    GetSystemDirectory(sysDir, MAX_PATH);
    std::wstring path = std::wstring(sysDir) + L"\\Taskmgr.exe";

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = path.c_str();
    sei.nShow = SW_SHOW;
    if (!ShellExecuteEx(&sei))
    {
        DWORD err = GetLastError();
        if (err != ERROR_CANCELLED)
            Wh_Log(L"Failed to open Task Manager: %lu", err);
    }
}

// Alt+Tab pressed and released as one batch performs the "quick swap" back to
// the previously focused window rather than opening the persistent switcher.
static void ActionSwitchLastWindow() { SendKeys({VK_LMENU, VK_TAB}); }

// Ctrl+Alt+Tab opens the switcher and leaves it up, so it can be navigated
// with the mouse after the cursor triggered it.
static void ActionTaskSwitcher() { SendKeys({VK_LCONTROL, VK_LMENU, VK_TAB}); }

static void ActionMinimizeWindow() { SendKeys({VK_LWIN, VK_DOWN}); }
static void ActionMaximizeWindow() { SendKeys({VK_LWIN, VK_UP}); }
static void ActionSnapLeft() { SendKeys({VK_LWIN, VK_LEFT}); }
static void ActionSnapRight() { SendKeys({VK_LWIN, VK_RIGHT}); }
static void ActionCloseWindow() { SendKeys({VK_LMENU, VK_F4}); }
static void ActionFileExplorer() { SendKeys({VK_LWIN, 'E'}); }
static void ActionSettingsApp() { SendKeys({VK_LWIN, 'I'}); }
static void ActionSearch() { SendKeys({VK_LWIN, 'S'}); }
static void ActionClipboardHistory() { SendKeys({VK_LWIN, 'V'}); }
static void ActionScreenshot() { SendKeys({VK_LWIN, VK_LSHIFT, 'S'}); }
static void ActionProjectDisplay() { SendKeys({VK_LWIN, 'P'}); }
static void ActionVDesktopNext() { SendKeys({VK_LWIN, VK_LCONTROL, VK_RIGHT}); }
static void ActionVDesktopPrev() { SendKeys({VK_LWIN, VK_LCONTROL, VK_LEFT}); }
static void ActionVDesktopNew() { SendKeys({VK_LWIN, VK_LCONTROL, 'D'}); }

static void ActionLock() { LockWorkStation(); }

static void ActionSleep()
{
    // SetSuspendState(hibernate, force, disableWakeEvent)
    SetSuspendState(FALSE, FALSE, FALSE);
}

static void ActionStartProcess(const std::wstring &command)
{
    if (command.empty())
        return;

    std::wstring cmd = command;
    std::wstring verb = L"open";

    // Expand environment variables (%AppData%, %USERPROFILE%, etc.)
    WCHAR expandedBuf[4096];
    DWORD expandedLen = ExpandEnvironmentStringsW(cmd.c_str(), expandedBuf,
                                                  ARRAYSIZE(expandedBuf));
    if (expandedLen > 0 && expandedLen <= ARRAYSIZE(expandedBuf))
        cmd = expandedBuf;

    if (cmd.length() > 4 && _wcsnicmp(cmd.c_str(), L"uac;", 4) == 0)
    {
        verb = L"runas";
        cmd = TrimStr(cmd.substr(4));
    }

    // CommandLineToArgvW handles unquoted paths with spaces correctly.
    std::wstring exe, params;
    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(cmd.c_str(), &argc);
    if (argv && argc > 0)
    {
        exe = argv[0];
        int firstArg = 1;

        // CommandLineToArgvW splits on spaces, so an unquoted path such as
        // C:\Program Files\Tool\tool.exe -x yields "C:\Program" plus the rest
        // as arguments and the launch fails. Glue tokens back onto the
        // executable until one names a file that exists.
        //
        // Guarded on argv[0] not resolving, so bare commands keep working:
        // "notepad.exe" is not a file relative to us, but the shell resolves
        // it via App Paths/PATH, and we must not disturb that.
        if (argc > 1 &&
            GetFileAttributesW(exe.c_str()) == INVALID_FILE_ATTRIBUTES)
        {
            std::wstring joined = exe;
            for (int i = 1; i < argc; i++)
            {
                joined += L' ';
                joined += argv[i];
                if (GetFileAttributesW(joined.c_str()) !=
                    INVALID_FILE_ATTRIBUTES)
                {
                    exe = joined;
                    firstArg = i + 1;
                    break;
                }
            }
        }

        for (int i = firstArg; i < argc; i++)
        {
            if (!params.empty())
                params += L' ';
            if (wcschr(argv[i], L' '))
            {
                params += L'"';
                params += argv[i];
                params += L'"';
            }
            else
            {
                params += argv[i];
            }
        }
        LocalFree(argv);
    }
    else
    {
        exe = cmd;
    }

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = verb.c_str();
    sei.lpFile = exe.c_str();
    sei.lpParameters = params.empty() ? nullptr : params.c_str();
    sei.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteEx(&sei))
    {
        DWORD err = GetLastError();
        if (err != ERROR_CANCELLED)
            Wh_Log(L"ShellExecuteEx failed: %lu", err);
    }
}

// =====================================================================
// Action Type Parsing
// =====================================================================

static CornerAction ParseActionType(const std::wstring &raw)
{
    static const std::unordered_map<std::wstring, CornerAction> map = {
        {L"ACTION_NOTHING", CornerAction::Nothing},
        {L"ACTION_SHOW_DESKTOP", CornerAction::ShowDesktop},
        {L"ACTION_TASK_VIEW", CornerAction::TaskView},
        {L"ACTION_SCREENSAVER", CornerAction::ScreenSaver},
        {L"ACTION_MONITORS_OFF", CornerAction::MonitorsOff},
        {L"ACTION_QUICK_SETTINGS", CornerAction::QuickSettings},
        {L"ACTION_NOTIFICATION_CENTER", CornerAction::NotificationCenter},
        {L"ACTION_START_MENU", CornerAction::StartMenu},
        {L"ACTION_HIDE_OTHERS", CornerAction::HideOthers},
        {L"ACTION_MUTE", CornerAction::Mute},
        {L"ACTION_TASK_MANAGER", CornerAction::TaskManager},
        {L"ACTION_LOCK", CornerAction::Lock},
        {L"ACTION_SLEEP", CornerAction::Sleep},
        {L"ACTION_SWITCH_LAST", CornerAction::SwitchLastWindow},
        {L"ACTION_TASK_SWITCHER", CornerAction::TaskSwitcher},
        {L"ACTION_MINIMIZE", CornerAction::MinimizeWindow},
        {L"ACTION_MAXIMIZE", CornerAction::MaximizeWindow},
        {L"ACTION_SNAP_LEFT", CornerAction::SnapLeft},
        {L"ACTION_SNAP_RIGHT", CornerAction::SnapRight},
        {L"ACTION_CLOSE_WINDOW", CornerAction::CloseWindow},
        {L"ACTION_FILE_EXPLORER", CornerAction::FileExplorer},
        {L"ACTION_SETTINGS", CornerAction::SettingsApp},
        {L"ACTION_SEARCH", CornerAction::Search},
        {L"ACTION_CLIPBOARD", CornerAction::ClipboardHistory},
        {L"ACTION_SCREENSHOT", CornerAction::Screenshot},
        {L"ACTION_PROJECT", CornerAction::ProjectDisplay},
        {L"ACTION_VDESK_NEXT", CornerAction::VDesktopNext},
        {L"ACTION_VDESK_PREV", CornerAction::VDesktopPrev},
        {L"ACTION_VDESK_NEW", CornerAction::VDesktopNew},
        {L"ACTION_SEND_KEYPRESS", CornerAction::SendKeypress},
        {L"ACTION_START_PROCESS", CornerAction::StartProcess},
    };

    auto it = map.find(ToUpperStr(TrimStr(raw)));
    return (it != map.end()) ? it->second : CornerAction::Nothing;
}

static const wchar_t *ActionToString(CornerAction a)
{
    switch (a)
    {
    case CornerAction::Nothing: return L"Nothing";
    case CornerAction::ShowDesktop: return L"Show Desktop";
    case CornerAction::TaskView: return L"Task View";
    case CornerAction::ScreenSaver: return L"Screen Saver";
    case CornerAction::MonitorsOff: return L"Turn Off Monitors";
    case CornerAction::QuickSettings: return L"Quick Settings";
    case CornerAction::NotificationCenter: return L"Notification Center";
    case CornerAction::StartMenu: return L"Start Menu";
    case CornerAction::HideOthers: return L"Hide Other Windows";
    case CornerAction::Mute: return L"Mute";
    case CornerAction::TaskManager: return L"Task Manager";
    case CornerAction::Lock: return L"Lock Computer";
    case CornerAction::Sleep: return L"Sleep";
    case CornerAction::SwitchLastWindow: return L"Switch to Last Window";
    case CornerAction::TaskSwitcher: return L"Task Switcher";
    case CornerAction::MinimizeWindow: return L"Minimize Window";
    case CornerAction::MaximizeWindow: return L"Maximize Window";
    case CornerAction::SnapLeft: return L"Snap Left";
    case CornerAction::SnapRight: return L"Snap Right";
    case CornerAction::CloseWindow: return L"Close Window";
    case CornerAction::FileExplorer: return L"File Explorer";
    case CornerAction::SettingsApp: return L"Settings";
    case CornerAction::Search: return L"Search";
    case CornerAction::ClipboardHistory: return L"Clipboard History";
    case CornerAction::Screenshot: return L"Screenshot";
    case CornerAction::ProjectDisplay: return L"Project";
    case CornerAction::VDesktopNext: return L"Virtual Desktop Next";
    case CornerAction::VDesktopPrev: return L"Virtual Desktop Previous";
    case CornerAction::VDesktopNew: return L"Virtual Desktop New";
    case CornerAction::SendKeypress: return L"Virtual Key Press";
    case CornerAction::StartProcess: return L"Custom Command";
    }
    return L"Unknown";
}

static const wchar_t *ZoneToString(Zone z)
{
    switch (z)
    {
    case ZONE_TOP_LEFT: return L"Top-left corner";
    case ZONE_TOP_RIGHT: return L"Top-right corner";
    case ZONE_BOTTOM_LEFT: return L"Bottom-left corner";
    case ZONE_BOTTOM_RIGHT: return L"Bottom-right corner";
    case ZONE_EDGE_TOP: return L"Top edge";
    case ZONE_EDGE_BOTTOM: return L"Bottom edge";
    case ZONE_EDGE_LEFT: return L"Left edge";
    case ZONE_EDGE_RIGHT: return L"Right edge";
    default: return L"None";
    }
}

// Creates an action executor from action type and args
static std::function<void()> MakeExecutor(CornerAction action,
                                          const std::wstring &args)
{
    switch (action)
    {
    case CornerAction::Nothing: return nullptr;
    case CornerAction::ShowDesktop: return ActionShowDesktop;
    case CornerAction::TaskView: return ActionTaskView;
    case CornerAction::ScreenSaver: return ActionScreenSaver;
    case CornerAction::MonitorsOff: return ActionMonitorsOff;
    case CornerAction::QuickSettings: return ActionQuickSettings;
    case CornerAction::NotificationCenter: return ActionNotificationCenter;
    case CornerAction::StartMenu: return ActionStartMenu;
    case CornerAction::HideOthers: return ActionHideOthers;
    case CornerAction::Mute: return ActionMute;
    case CornerAction::TaskManager: return ActionTaskManager;
    case CornerAction::Lock: return ActionLock;
    case CornerAction::Sleep: return ActionSleep;
    case CornerAction::SwitchLastWindow: return ActionSwitchLastWindow;
    case CornerAction::TaskSwitcher: return ActionTaskSwitcher;
    case CornerAction::MinimizeWindow: return ActionMinimizeWindow;
    case CornerAction::MaximizeWindow: return ActionMaximizeWindow;
    case CornerAction::SnapLeft: return ActionSnapLeft;
    case CornerAction::SnapRight: return ActionSnapRight;
    case CornerAction::CloseWindow: return ActionCloseWindow;
    case CornerAction::FileExplorer: return ActionFileExplorer;
    case CornerAction::SettingsApp: return ActionSettingsApp;
    case CornerAction::Search: return ActionSearch;
    case CornerAction::ClipboardHistory: return ActionClipboardHistory;
    case CornerAction::Screenshot: return ActionScreenshot;
    case CornerAction::ProjectDisplay: return ActionProjectDisplay;
    case CornerAction::VDesktopNext: return ActionVDesktopNext;
    case CornerAction::VDesktopPrev: return ActionVDesktopPrev;
    case CornerAction::VDesktopNew: return ActionVDesktopNew;
    case CornerAction::SendKeypress:
    {
        auto combos = ParseKeyCombo(args);
        if (combos.empty())
        {
            Wh_Log(L"SendKeypress: no valid keys parsed from '%s'",
                   args.c_str());
            return nullptr;
        }
        return [combos]()
        {
            for (const auto &keys : combos)
                SendKeys(keys);
        };
    }
    case CornerAction::StartProcess:
    {
        std::wstring cmd = TrimStr(args);
        if (cmd.empty())
        {
            Wh_Log(L"StartProcess: empty command");
            return nullptr;
        }
        return [cmd]() { ActionStartProcess(cmd); };
    }
    }
    return nullptr;
}

// =====================================================================
// Zone Building
// =====================================================================

// Resolves one zone for one monitor. A name-matched config wins; otherwise a
// wildcard config supplies the action. Resolution is per zone, so a wildcard
// entry can cover most zones while one display overrides a single corner.
// Caller must hold g_settingsLock.
static const ZoneConfig *ResolveZone(const MonitorInfo &mon, Zone zone)
{
    // 1. Exact friendly-name match
    for (const auto &cfg : g_settings.monitorConfigs)
    {
        if (cfg.monitorId.empty() || cfg.monitorId == L"*")
            continue;
        if (_wcsicmp(cfg.monitorId.c_str(), mon.id.c_str()) != 0)
            continue;
        const auto &zc = cfg.zones[zone];
        if (zc.action != CornerAction::Nothing && zc.executor)
            return &zc;
    }

    // 2. Legacy numeric ordinal, for configs carried over from v2.x
    for (const auto &cfg : g_settings.monitorConfigs)
    {
        if (!cfg.monitorId.empty() || cfg.monitorIndex != mon.index)
            continue;
        const auto &zc = cfg.zones[zone];
        if (zc.action != CornerAction::Nothing && zc.executor)
            return &zc;
    }

    // 3. Wildcard
    for (const auto &cfg : g_settings.monitorConfigs)
    {
        if (cfg.monitorId != L"*" &&
            !(cfg.monitorId.empty() && cfg.monitorIndex == 0))
            continue;
        const auto &zc = cfg.zones[zone];
        if (zc.action != CornerAction::Nothing && zc.executor)
            return &zc;
    }

    return nullptr;
}

// Builds an immutable snapshot containing only zones that actually do
// something, with the action already resolved. The detection loop therefore
// does nothing per tick but compare rectangles.
static std::shared_ptr<const ZoneSet> BuildZoneSet()
{
    auto set = std::make_shared<ZoneSet>();

    EnterCriticalSection(&g_settingsLock);

    int csCfg = g_settings.cornerSize > 0 ? g_settings.cornerSize : 1;
    int esCfg = g_settings.edgeSize > 0 ? g_settings.edgeSize : 1;

    set->activationDelay = g_settings.activationDelay;
    set->settleMs = g_settings.settleMs;
    set->cooldownMs = g_settings.cooldownMs;
    set->disableDuringDrag = g_settings.disableDuringDrag;

    for (const auto &mon : g_monitors)
    {
        const RECT &r = mon.rcMonitor;

        // Clamp per monitor so the eight zones are provably disjoint and the
        // hit test's first-match-wins can never hide one behind another:
        //   - a corner bigger than half the screen would overlap its opposite
        //   - an edge strip thicker than the corner box would overlap the
        //     perpendicular edge (EdgeTop vs EdgeLeft), and only whichever
        //     was added first would ever fire
        int span = (r.right - r.left) < (r.bottom - r.top)
                       ? (r.right - r.left)
                       : (r.bottom - r.top);
        int cs = csCfg;
        if (cs > span / 2)
            cs = span / 2;
        if (cs < 1)
            cs = 1;
        int es = esCfg > cs ? cs : esCfg;

        if (cs != csCfg || es != esCfg)
        {
            Wh_Log(L"  [%s] sizes clamped: corner %d->%d, edge %d->%d",
                   mon.id.c_str(), csCfg, cs, esCfg, es);
        }

        auto add = [&](Zone z, RECT rect)
        {
            const ZoneConfig *zc = ResolveZone(mon, z);
            if (!zc)
                return;
            HitZone hz;
            hz.rect = rect;
            hz.exec = zc->executor;
            hz.label = mon.id + L" " + ZoneToString(z) + L" -> " +
                       ActionToString(zc->action);
            set->zones.push_back(std::move(hz));
        };

        // Corners
        add(ZONE_TOP_LEFT, {r.left, r.top, r.left + cs, r.top + cs});
        add(ZONE_TOP_RIGHT, {r.right - cs, r.top, r.right, r.top + cs});
        add(ZONE_BOTTOM_LEFT, {r.left, r.bottom - cs, r.left + cs, r.bottom});
        add(ZONE_BOTTOM_RIGHT,
            {r.right - cs, r.bottom - cs, r.right, r.bottom});

        // Edges — the side strip with the corner zones carved out
        if (r.left + cs < r.right - cs)
        {
            add(ZONE_EDGE_TOP,
                {r.left + cs, r.top, r.right - cs, r.top + es});
            add(ZONE_EDGE_BOTTOM,
                {r.left + cs, r.bottom - es, r.right - cs, r.bottom});
        }
        if (r.top + cs < r.bottom - cs)
        {
            add(ZONE_EDGE_LEFT,
                {r.left, r.top + cs, r.left + es, r.bottom - cs});
            add(ZONE_EDGE_RIGHT,
                {r.right - es, r.top + cs, r.right, r.bottom - cs});
        }
    }

    LeaveCriticalSection(&g_settingsLock);

    if (set->zones.empty())
    {
        Wh_Log(L"No zones are active - every zone is set to \"Nothing\", or "
               L"no configuration matches a connected monitor.");
    }
    else
    {
        Wh_Log(L"Active zones (%d)   corner %dpx, edge %dpx:",
               (int)set->zones.size(), csCfg, esCfg);
        for (const auto &z : set->zones)
            Wh_Log(L"   %s", z.label.c_str());
    }

    return set;
}

// Detection thread only.
static void RebuildZones()
{
    RefreshMonitors();
    auto set = BuildZoneSet();

    EnterCriticalSection(&g_zonesLock);
    g_zones = set;
    LeaveCriticalSection(&g_zonesLock);

    g_activeZone = -1;
    g_firedThisEntry = false;
    g_lastAnyFireTick = 0;
    g_lastFireTick.assign(set->zones.size(), 0);

    g_topoCount = GetSystemMetrics(SM_CMONITORS);
    g_topoVirtual = {GetSystemMetrics(SM_XVIRTUALSCREEN),
                     GetSystemMetrics(SM_YVIRTUALSCREEN),
                     GetSystemMetrics(SM_CXVIRTUALSCREEN),
                     GetSystemMetrics(SM_CYVIRTUALSCREEN)};
}

// WM_DISPLAYCHANGE is not delivered for every layout change that matters —
// docking, monitor wake, and RDP reconnect frequently skip it, leaving zones
// at coordinates that no longer exist. These metrics are a few cheap reads.
static bool TopologyChanged()
{
    RECT v = {GetSystemMetrics(SM_XVIRTUALSCREEN),
              GetSystemMetrics(SM_YVIRTUALSCREEN),
              GetSystemMetrics(SM_CXVIRTUALSCREEN),
              GetSystemMetrics(SM_CYVIRTUALSCREEN)};
    return GetSystemMetrics(SM_CMONITORS) != g_topoCount ||
           memcmp(&v, &g_topoVirtual, sizeof(v)) != 0;
}

// =====================================================================
// Action Worker Thread
// =====================================================================

static void EnqueueAction(const HitZone &hz)
{
    EnterCriticalSection(&g_queueLock);
    if (g_queue.size() < kMaxQueue)
        g_queue.push_back(hz);
    LeaveCriticalSection(&g_queueLock);
    SetEvent(g_hWorkEvent);
}

static DWORD WINAPI ActionWorkerThread(LPVOID)
{
    HANDLE waits[2] = {g_hStopEvent, g_hWorkEvent};
    for (;;)
    {
        DWORD r = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (r == WAIT_OBJECT_0 || r == WAIT_FAILED)
            break;

        for (;;)
        {
            HitZone job;
            bool have = false;
            EnterCriticalSection(&g_queueLock);
            if (!g_queue.empty())
            {
                job = std::move(g_queue.front());
                g_queue.pop_front();
                have = true;
            }
            LeaveCriticalSection(&g_queueLock);
            if (!have)
                break;

            // The gates live here, not on the detection thread, so a slow
            // SHQueryUserNotificationState or OpenProcess can never delay the
            // next cursor sample.
            bool checkFullscreen;
            std::vector<std::wstring> excluded;
            EnterCriticalSection(&g_settingsLock);
            checkFullscreen = g_settings.disableOnFullscreen;
            excluded = g_settings.excludedProcesses;
            LeaveCriticalSection(&g_settingsLock);

            // Suppression is normal, not an error — logging it every time
            // would spam hardest in exactly the case it matters least
            // (holding a corner while a fullscreen game has focus).
            if (checkFullscreen && IsFullScreenAppActive())
            {
                if (g_verboseLog)
                    Wh_Log(L"SKIP (fullscreen): %s", job.label.c_str());
                continue;
            }
            if (IsForegroundAppExcluded(excluded))
            {
                if (g_verboseLog)
                    Wh_Log(L"SKIP (excluded app): %s", job.label.c_str());
                continue;
            }

            if (g_verboseLog)
            {
                WCHAR fgClass[64] = L"?";
                GetClassName(GetForegroundWindow(), fgClass,
                             ARRAYSIZE(fgClass));
                Wh_Log(L"FIRE: %s (foreground '%s')", job.label.c_str(),
                       fgClass);
            }

            // Calling an empty std::function throws, and an exception escaping
            // this thread would kill it silently — every zone would stop
            // working with no indication why.
            if (job.exec)
                job.exec();
        }
    }
    return 0;
}

// =====================================================================
// Detection
// =====================================================================

static bool AnyMouseButtonDown()
{
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_MBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) ||
           (GetAsyncKeyState(VK_XBUTTON2) & 0x8000);
}

static void DetectTick()
{
    std::shared_ptr<const ZoneSet> zones;
    EnterCriticalSection(&g_zonesLock);
    zones = g_zones;
    LeaveCriticalSection(&g_zonesLock);

    if (!zones || zones->zones.empty())
        return;

    POINT pt;
    if (!GetCursorPos(&pt))
        return;

    int idx = -1;
    for (size_t i = 0; i < zones->zones.size(); i++)
    {
        const RECT &z = zones->zones[i].rect;
        if (pt.x >= z.left && pt.x < z.right && pt.y >= z.top &&
            pt.y < z.bottom)
        {
            idx = (int)i;
            break;
        }
    }

    ULONGLONG now = GetTickCount64();

    // Enter/leave edge detection, the same shape macOS uses: a zone fires
    // once on entry and re-arms only after the cursor leaves it.
    if (idx != g_activeZone)
    {
        g_activeZone = idx;
        g_enterTick = now;
        g_firedThisEntry = false;
    }

    if (idx < 0 || g_firedThisEntry)
        return;

    // Suppress for the whole visit, not just this tick — otherwise releasing
    // a drag inside a corner would immediately trigger it.
    if (zones->disableDuringDrag && AnyMouseButtonDown())
    {
        g_firedThisEntry = true;
        return;
    }

    // A corner cannot be reached without crossing the edge strip next to it,
    // so a fast transit would fire the edge and then the corner milliseconds
    // apart. With a toggle action bound to both (e.g. Task View) that opens
    // and instantly re-closes, looking exactly like nothing happened.
    // Requiring the cursor to settle means a pass-through never fires; only
    // the zone you actually stop in does.
    int dwell = zones->activationDelay > zones->settleMs ? zones->activationDelay
                                                         : zones->settleMs;
    if (dwell > 0 && (now - g_enterTick) < (ULONGLONG)dwell)
        return;

    if (zones->cooldownMs > 0 && idx < (int)g_lastFireTick.size())
    {
        ULONGLONG last = g_lastFireTick[idx];
        if (last != 0 && (now - last) < (ULONGLONG)zones->cooldownMs)
        {
            g_firedThisEntry = true;
            return;
        }
    }

    // Global floor across all zones. The per-zone cooldown alone does not stop
    // a sweep through several different zones from queueing a burst.
    if (g_lastAnyFireTick != 0 &&
        (now - g_lastAnyFireTick) < kMinFireIntervalMs)
    {
        g_firedThisEntry = true;
        return;
    }

    g_firedThisEntry = true;
    g_lastAnyFireTick = now;
    if (idx < (int)g_lastFireTick.size())
        g_lastFireTick[idx] = now;

    EnqueueAction(zones->zones[idx]);
}

// =====================================================================
// Detection Thread
// =====================================================================

static LRESULT CALLBACK DetectWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam)
{
    if (uMsg == WM_DISPLAYCHANGE || uMsg == WM_APP_REBUILD)
    {
        if (uMsg == WM_DISPLAYCHANGE)
            Wh_Log(L"WM_DISPLAYCHANGE — rebuilding zones");
        else
            Wh_Log(L"Settings changed — rebuilding zones");
        RebuildZones();
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI DetectThread(LPVOID)
{
    // Everything that reads coordinates must agree on the DPI context.
    PinThreadDpiPerMonitorV2();

    const wchar_t *kClass = L"WindhawkHotCornersDetect";
    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = DetectWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kClass;
    RegisterClass(&wc);

    // Must be a top-level window, not HWND_MESSAGE — message-only windows do
    // not receive the WM_DISPLAYCHANGE broadcast.
    g_hDetectWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kClass,
                                  nullptr, WS_POPUP, 0, 0, 0, 0, nullptr,
                                  nullptr, hInst, nullptr);
    if (!g_hDetectWnd)
    {
        Wh_Log(L"Failed to create detection window");
        UnregisterClass(kClass, hInst);
        return 1;
    }

    RebuildZones();

    constexpr ULONGLONG kTopoCheckMs = 500;
    ULONGLONG lastTopoCheck = GetTickCount64();

    for (;;)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                goto done;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Display layout changes are a human-scale event; checking twice a
        // second is plenty and keeps the 16 ms tick down to one GetCursorPos
        // plus a few rectangle compares.
        ULONGLONG nowTick = GetTickCount64();
        if (nowTick - lastTopoCheck >= kTopoCheckMs)
        {
            lastTopoCheck = nowTick;
            if (TopologyChanged())
            {
                Wh_Log(L"Display topology changed — rebuilding");
                RebuildZones();
            }
        }

        DetectTick();

        if (WaitForSingleObject(g_hStopEvent, kTickMs) == WAIT_OBJECT_0)
            break;
    }

done:
    DestroyWindow(g_hDetectWnd);
    g_hDetectWnd = nullptr;
    UnregisterClass(kClass, hInst);
    Wh_Log(L"Detection thread exiting");
    return 0;
}

// =====================================================================
// Settings
// =====================================================================

static void LoadSettings()
{
    using WindhawkUtils::StringSetting;

    EnterCriticalSection(&g_settingsLock);

    g_settings.cornerSize = Wh_GetIntSetting(L"CornerSize");
    if (g_settings.cornerSize < 1)
        g_settings.cornerSize = 6;

    g_settings.edgeSize = Wh_GetIntSetting(L"EdgeSize");
    if (g_settings.edgeSize < 1)
        g_settings.edgeSize = 6;

    g_settings.activationDelay = Wh_GetIntSetting(L"ActivationDelay");
    if (g_settings.activationDelay < 0)
        g_settings.activationDelay = 0;

    g_settings.settleMs = Wh_GetIntSetting(L"SettleMs");
    if (g_settings.settleMs < 0)
        g_settings.settleMs = 0;

    g_settings.cooldownMs = Wh_GetIntSetting(L"CooldownMs");
    if (g_settings.cooldownMs < 0)
        g_settings.cooldownMs = 0;

    g_settings.disableOnFullscreen = Wh_GetIntSetting(L"DisableOnFullscreen");
    g_settings.disableDuringDrag = Wh_GetIntSetting(L"DisableDuringDrag");
    g_verboseLog = Wh_GetIntSetting(L"VerboseLogging") != 0;
    g_showMonitorNames = Wh_GetIntSetting(L"ShowMonitorNames") != 0;

    // Excluded processes (semicolon-separated, case-insensitive)
    g_settings.excludedProcesses.clear();
    {
        std::wstring remaining =
            std::wstring(StringSetting::make(L"ExcludedProcesses").get());
        while (!remaining.empty())
        {
            auto semi = remaining.find(L';');
            std::wstring token;
            if (semi != std::wstring::npos)
            {
                token = TrimStr(remaining.substr(0, semi));
                remaining = remaining.substr(semi + 1);
            }
            else
            {
                token = TrimStr(remaining);
                remaining.clear();
            }
            if (!token.empty())
                g_settings.excludedProcesses.push_back(ToLowerStr(token));
        }
    }

    g_settings.monitorConfigs.clear();

    static const wchar_t *zoneKeys[ZONE_COUNT] = {
        L"TopLeft", L"TopRight",   L"BottomLeft", L"BottomRight",
        L"EdgeTop", L"EdgeBottom", L"EdgeLeft",   L"EdgeRight",
    };

    for (int i = 0; i < 16; i++)
    {
        wchar_t keyBuf[128];

        // An empty action string means there is no entry at this index.
        _snwprintf_s(keyBuf, _countof(keyBuf), _TRUNCATE,
                     L"MonitorCorners[%d].TopLeft", i);
        auto probe = std::wstring(StringSetting::make(keyBuf).get());
        if (probe.empty())
            break;

        MonitorZoneConfig cfg;

        _snwprintf_s(keyBuf, _countof(keyBuf), _TRUNCATE,
                     L"MonitorCorners[%d].MonitorId", i);
        cfg.monitorId =
            TrimStr(std::wstring(StringSetting::make(keyBuf).get()));

        _snwprintf_s(keyBuf, _countof(keyBuf), _TRUNCATE,
                     L"MonitorCorners[%d].Monitor", i);
        cfg.monitorIndex = Wh_GetIntSetting(keyBuf);

        bool hasAnyAction = false;
        for (int z = 0; z < ZONE_COUNT; z++)
        {
            _snwprintf_s(keyBuf, _countof(keyBuf), _TRUNCATE,
                         L"MonitorCorners[%d].%s", i, zoneKeys[z]);
            auto actionStr = std::wstring(StringSetting::make(keyBuf).get());

            _snwprintf_s(keyBuf, _countof(keyBuf), _TRUNCATE,
                         L"MonitorCorners[%d].%sArgs", i, zoneKeys[z]);
            auto argsStr = std::wstring(StringSetting::make(keyBuf).get());

            CornerAction action = ParseActionType(actionStr);
            cfg.zones[z].action = action;
            cfg.zones[z].args = argsStr;
            cfg.zones[z].executor = MakeExecutor(action, argsStr);

            if (action != CornerAction::Nothing)
                hasAnyAction = true;
        }

        if (!hasAnyAction)
            continue;

        // The resolved result is printed by BuildZoneSet as "Active zones";
        // repeating every zone here just doubled the noise.
        Wh_Log(L"Configuration %d applies to: %s", i + 1,
               cfg.monitorId.empty()
                   ? (cfg.monitorIndex == 0
                          ? L"every monitor (legacy: Monitor Number 0)"
                          : L"a monitor by number (legacy)")
                   : cfg.monitorId.c_str());

        g_settings.monitorConfigs.push_back(std::move(cfg));
    }

    Wh_Log(L"Sizes: corner %dpx, edge %dpx.  Timing: delay %dms, "
           L"pass-through guard %dms, cooldown %dms.",
           g_settings.cornerSize, g_settings.edgeSize,
           g_settings.activationDelay, g_settings.settleMs,
           g_settings.cooldownMs);
    Wh_Log(L"Skip while fullscreen: %s.  Skip while dragging: %s.  "
           L"Excluded apps: %d.",
           g_settings.disableOnFullscreen ? L"yes" : L"no",
           g_settings.disableDuringDrag ? L"yes" : L"no",
           (int)g_settings.excludedProcesses.size());

    LeaveCriticalSection(&g_settingsLock);
}

// =====================================================================
// Tool Mod Entry Points
// =====================================================================

BOOL WhTool_ModInit()
{
    Wh_Log(L"Win-X Hot Corners v" WH_MOD_VERSION);

    InitializeCriticalSection(&g_settingsLock);
    InitializeCriticalSection(&g_zonesLock);
    InitializeCriticalSection(&g_queueLock);

    g_hStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_hWorkEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_hStopEvent || !g_hWorkEvent)
    {
        Wh_Log(L"Failed to create events");
        return FALSE;
    }

    LoadSettings();

    g_hWorkerThread =
        CreateThread(nullptr, 0, ActionWorkerThread, nullptr, 0, nullptr);
    if (!g_hWorkerThread)
    {
        Wh_Log(L"Failed to create worker thread");
        return FALSE;
    }

    g_hDetectThread =
        CreateThread(nullptr, 0, DetectThread, nullptr, 0, &g_dwDetectThreadId);
    if (!g_hDetectThread)
    {
        Wh_Log(L"Failed to create detection thread");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    LoadSettings();
    // The zone rebuild has to happen on the detection thread, which owns the
    // DPI context and the monitor list. Post, never send — Windhawk's thread
    // must not block on ours.
    if (g_hDetectWnd)
        PostMessage(g_hDetectWnd, WM_APP_REBUILD, 0, 0);
}

void WhTool_ModUninit()
{
    if (g_hStopEvent)
        SetEvent(g_hStopEvent);

    if (g_dwDetectThreadId)
        PostThreadMessage(g_dwDetectThreadId, WM_QUIT, 0, 0);

    bool allStopped = true;

    if (g_hDetectThread)
    {
        if (WaitForSingleObject(g_hDetectThread, 3000) == WAIT_TIMEOUT)
        {
            Wh_Log(L"Detection thread exit timed out");
            allStopped = false;
        }
        CloseHandle(g_hDetectThread);
        g_hDetectThread = nullptr;
    }

    if (g_hWorkerThread)
    {
        // Can legitimately still be inside ShellExecuteEx (a UAC prompt keeps
        // it there indefinitely).
        if (WaitForSingleObject(g_hWorkerThread, 3000) == WAIT_TIMEOUT)
        {
            Wh_Log(L"Worker thread exit timed out");
            allStopped = false;
        }
        CloseHandle(g_hWorkerThread);
        g_hWorkerThread = nullptr;
    }

    // Freeing objects a live thread is still using is undefined behaviour
    // inside the host process. If a thread refused to stop, leak instead:
    // this path is immediately followed by ExitProcess, so the leak costs
    // nothing and cannot crash anything on the way out.
    if (!allStopped)
    {
        Wh_Log(L"A thread did not stop; leaking sync objects rather than "
               L"freeing them from under it");
        return;
    }

    if (g_hStopEvent)
    {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = nullptr;
    }
    if (g_hWorkEvent)
    {
        CloseHandle(g_hWorkEvent);
        g_hWorkEvent = nullptr;
    }

    DeleteCriticalSection(&g_queueLock);
    DeleteCriticalSection(&g_zonesLock);
    DeleteCriticalSection(&g_settingsLock);

    Wh_Log(L"Uninit done");
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook()
{
    ExitThread(0);
}

BOOL Wh_ModInit()
{
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0)
    {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv)
    {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++)
    {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0)
        {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++)
    {
        if (wcscmp(argv[i], L"-tool-mod") == 0)
        {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0)
            {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded)
    {
        return FALSE;
    }

    if (isCurrentToolModProcess)
    {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex)
        {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            CloseHandle(g_toolModProcessMutex);
            ExitProcess(1);
        }

        if (!WhTool_ModInit())
        {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER *dosHeader =
            (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS *ntHeaders =
            (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess)
    {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit()
{
    if (!g_isToolModProcessLauncher)
    {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    DWORD length = GetModuleFileName(nullptr, currentProcessPath,
                                     ARRAYSIZE(currentProcessPath));
    if (length == 0 || length == ARRAYSIZE(currentProcessPath))
    {
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[MAX_PATH * 2];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule)
    {
        kernelModule = GetModuleHandle(L"kernel32.dll");
    }
    if (!kernelModule)
    {
        Wh_Log(L"GetModuleHandle failed");
        return;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI *)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW)
    {
        Wh_Log(L"GetProcAddress failed");
        return;
    }

    STARTUPINFO si = {sizeof(si)};
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi = {0};
    if (pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                nullptr, nullptr, &si, &pi, nullptr))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        Wh_Log(L"CreateProcessInternalW failed");
    }
}

void Wh_ModSettingsChanged()
{
    if (g_isToolModProcessLauncher)
    {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit()
{
    if (g_isToolModProcessLauncher)
    {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

// ==WindhawkMod==
// @id              win-x-hotcorners
// @name            Win-X Hot Corners
// @description     macOS-style hot corners & edges for Windows with full multi-monitor support — trigger actions instantly when your cursor hits any screen corner or edge
// @version         4.1.1
// @author          lost_husky
// @github          https://github.com/DhakadG
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lcomctl32 -lgdi32 -lpowrprof -lshell32 -luser32
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

### Related mods

If all you want is one specific behaviour, a smaller mod may suit you better:

- **edge-hot-corner-desktop-switch** — hovering the left or right screen edge
  switches virtual desktop. This mod does that too, as one of the actions you
  can assign, but if it is the only thing you are after that one is far
  simpler.
- **hotcorner-hotkeys** — sends a key combination from a corner. Different
  trigger model: it dispatches on a hotkey rather than on hover.

## Features

- **Bounded latency** — a dedicated detection thread samples the cursor every
  ~16 ms, and only idles when nothing could fire anyway. Nothing can starve it:
  not a busy explorer, not an elevated foreground app, not a slow action.
- **Zero impact on the rest of the system** — no global mouse hook, so your
  games and apps keep their input path to themselves.
- **Monitors identified by name** — zones bind to a display's friendly name
  (e.g. `Dell U2720Q`), so rearranging your desktop or changing which display
  is primary never reshuffles your configuration. (Two displays of the same
  model are the one exception — see *Identifying your monitors*.)
- **Per-monitor DPI correct** — detection runs per-monitor-DPI-aware, so
  zones land in the right place on mixed-scaling setups.
- **Screen edges** — trigger actions on the top, bottom, left, or right edge
  of any monitor.
- **Configurable zone size**, optional dwell delay, and a cooldown timer.
- **Fullscreen protection** — auto-detects games, presentations, and D3D
  fullscreen apps.
- **Drag protection** — zones don't fire while a mouse button is held.
- **App exclusions** — blacklist processes that need corner/edge clicks.

## Configuring — look in your system tray, not here

**This mod has no Settings page.** Everything is configured from the tray icon
it adds, next to the clock:

- **Left-click** — turn the hot corners on or off.
- **Right-click** — a quick menu: suspend for a while, skip while an app is
  fullscreen, skip while dragging.
- **Right-click → Zones & settings...** — the dashboard, where zones, timings
  and everything else live.

The dashboard is a normal window with a live preview of your screen, so you
click the corner you want and pick its action. Every field explains itself on
hover.

This is deliberate. Twelve zones on each of up to eight displays, each with a
forty-entry action list and six timing overrides, is not something a settings
form can present without becoming a tree nobody can navigate — and a Windhawk
mod cannot write its own settings from code, so any change made in the mod's
own UI could not be written back. Two places to configure one thing, disagreeing
the moment you touched either. Now there is one.

If the tray icon is hidden, it is in the overflow area — drag it onto the
taskbar to keep it there.

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
| Lock and Turn Off Monitors | Lock, then blank the displays |
| Keep Awake On / Off | Suspend or restore screensaver and sleep |
| Sleep | Put the computer to sleep |
| Turn Off Monitors | Power off all displays |
| Start Screen Saver | Activate the screen saver |

**Custom**

| Action | Description |
|--------|-------------|
| Virtual Key Press | Send any key combination, or several in sequence |
| Alternate Key Press | Two combinations, fired alternately (`Alt+S | Alt+H`) |
| Alternate Command | Two commands, fired alternately |
| Custom Command | Launch any executable, path, or URL |
| Nothing | Disabled (default) |

## Identifying your monitors

The dashboard's monitor selector lists your displays by name, so normally there
is nothing to identify. The names also go to this mod's log every time it loads
or your display layout changes, which is the place to check when a display is
unplugged and you want to know which configuration belonged to it:

```
Monitor 1 [PRIMARY] id='Dell U2720Q' device=\\.\DISPLAY1 (0,0)-(3840,2160)
Monitor 2           id='BOE0998'     device=\\.\DISPLAY2 (3840,0)-(5760,1080)
```

Copy the text inside the quotes. If you own two identical displays they get a
` #2`, ` #3` suffix so each stays separately configurable. Those suffixes are
handed out in listed order — primary first, then left to right — so unlike the
names themselves they are not fixed: making the other twin primary swaps which
one is ` #2`, and swaps the configuration with it. Check the log after such a
change.

The selector's first entry, **All monitors** (`*`), applies one configuration
to every display.

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

## Alternate Key Press / Alternate Command

Two actions separated by `|`. The zone fires the left one, then the right
one, then the left again — the "different action on the second trigger" case:

```
Alt+S | Alt+H            one corner shows notes, then hides them
notepad.exe | calc.exe
```

Each side accepts everything the single-action version does, so
`Ctrl+C;Ctrl+V | Alt+Tab` is valid. Every zone alternates independently, and
the position resets whenever settings or the display layout change.

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

# Changelog

## What's New in v4.1.1

Review fixes on top of v4.1.0, one of them a real race.

- **Fixed: a reload could briefly publish an empty zone set.** Loading the
  configuration was two steps — write the defaults, then lay the stored values
  over them — each taking the settings lock separately. In the gap the mod held
  a complete, plausible, *wrong* configuration with no zones in it. Four of the
  six things that trigger a reload run on a different thread from the detection
  loop, and that loop re-checks the display layout twice a second, so a rebuild
  landing in the gap would arm nothing and log "No zones are active". Loading is
  now one transaction: the whole configuration is built off to the side and
  swapped in under a single lock.
- **That lock is also held for far less time.** It used to cover several hundred
  value-store reads, an action built per zone, and two log writes — while the
  detection thread, the action worker and the tray menu all waited on it.
- **The adaptive poll rate is gone.** v4.1.0 eased the sampling interval off
  while the cursor was far from every zone. Whichever way that decision is
  made, it is made from a sample taken *before* the user starts moving, so a
  flick could cross a zone entirely between two samples. On the outer perimeter
  that costs a late trigger, because the pointer stops against the screen edge —
  but a corner shared with a second monitor has no edge to stop against, and
  there it was a lost one. Detection is back to a flat 16 ms whenever a zone
  could fire; it still idles at 100 ms when the mod is switched off, suspended,
  or has no zones armed, where nothing can be missed because nothing can fire.
- **The dashboard's Reset button said "Reset to Windhawk settings".** There is
  no Windhawk settings page to go back to. It now says what it does.
- **The monitor list in the log** still told you to copy a name into the
  "Monitor" setting, which no longer exists.

## What's New in v4.1.0

**The Settings page is gone. Everything is in the tray icon now.**

- **One place to configure this mod, not two.** The Windhawk Settings page has
  been removed entirely. Twelve zones per display, each with a forty-entry
  action list and six timing overrides, had grown into a tree that was faster
  to give up on than to navigate — and because a mod cannot write its own
  settings from code, anything you changed in the dashboard could never be
  written back to it. The two disagreed the moment you touched either one. The
  dashboard won: right-click the tray icon → **Zones & settings...**
- **If you configured this mod on the Settings page, set it up again from the
  dashboard.** Configurations already saved from the dashboard are untouched.
- **Reset actually resets.** The dashboard's Reset button left the numeric
  options — sizes, delays, cooldown — behind in storage: still applied, no
  longer shown anywhere. It now clears everything it can write.
- **Fixed: per-display zones could be attributed to the wrong display.** The
  editor matched a stored configuration to the monitor selector by list
  position rather than by monitor, so a configuration written for one display
  could appear under **All monitors** — and saving it then fired it on every
  display.
- **Verbose logging is gone as a setting.** Every log it gated was once per
  trigger, never on the polling path, so there was nothing to gate. Suppressed
  triggers now log once per run instead of once per cooldown, which is what the
  setting was really protecting you from.
- **The tray menu's reset** now says what it does — it clears the three toggles
  above it, and nothing else. Wiping your zones is the dashboard's Reset
  button, which asks first.
- **The options are grouped.** Fourteen fields in one flat column, in no
  particular order, is a wall. They now sit under four headings — how big the
  zones are, when a zone fires, when to stay out of the way, everything else —
  and every field still explains itself on hover.

**Behaviour**

- **A fullscreen app now only silences the display it is on.** Launching a game
  on one monitor used to disable the hot corners on *every* monitor, which is
  the opposite of why anyone owns a second screen. When Windows will not say
  which display is involved, the old behaviour still applies and all displays
  are suppressed.
- **A split edge alternates as one edge.** With a centre action assigned, an
  edge becomes two strips either side of the centre — but it is still one edge,
  and **Alternate Key Press** / **Alternate Command** kept a separate A/B
  position for each half. Walking into the left strip and then the right gave
  you A twice. They now share one position.
- **The detection thread idles when nothing can fire.** It sampled the cursor
  every 16 ms even with the mod switched off, suspended, or with no zones
  armed. Those three cases now tick at 100 ms. *(v4.1.0 also eased off while
  the cursor was merely far away; v4.1.1 removed that — see below.)*

## What's New in v4.0.5

- **Fixed: "Turn Off Monitors", "Start Screen Saver" and "Lock and Turn Off
  Monitors" could stall every other action behind them.** They ask every
  window on the desktop to act, and the old call waited up to half a second
  for *each* one — so a single slow application could tie up the action
  thread for seconds while the mod appeared to have stopped responding. The
  request is now sent without waiting.
- Added a note on which smaller mods overlap with this one, so it is easier to
  tell which is the right tool.
- Internal tidying: dropped snapshot fields nothing read, the theme helper
  resolves its entry point once instead of on every control, and the tray
  icon's mask is explicitly zeroed rather than left undefined.

## What's New in v4.0.4

- **Fixed: disabling or reloading the mod with the settings window open could
  crash Windhawk.** Shutdown stopped every thread except the one running that
  window, then freed the locks it was still using. It now closes the window
  and waits for it, and if it will not close, leaves the locks alone rather
  than pulling them out from under it.
- **Fixed: a failed key injection could leave a modifier stuck down.** If the
  key presses went through but the releases did not, the key stayed logically
  held for the rest of the session — a stuck Win or Ctrl that nothing on
  screen explains. The releases are now replayed after a partial send.
- **Fixed: opening the settings window leaked an icon every time.**
- Saving a configuration for a display beyond the eighth now says so in the
  log instead of appearing to work and losing the edit on the next reload.
- A Custom Command of just `uac;` with nothing after it no longer tries to
  relaunch Windhawk itself with elevation.
- Settings shared between the tray, the settings window and the detection
  loop are read and written atomically.

## What's New in v4.0.3

- **Fixed: the mod would not load at all.** "Require a modifier key" offered a
  dropdown whose values were numbers, and Windhawk only accepts text values for
  a dropdown — it refused to parse the settings before any of the mod ran. The
  choices are now stored by name (`none`, `ctrl`, `alt`, `shift`, `win`); a
  configuration saved by an earlier version keeps working.

## What's New in v4.0.2

- **Fixed: zones could stop matching after a display change.** Windows reports
  a layout that changed while it was being read as a buffer error, and that is
  precisely when this runs — a layout change is what calls it. The mod treated
  that as fatal and dropped every display name for the rebuild, leaving zones
  bound to a name unmatched until the next display change. It now re-reads
  instead.
- Documentation corrections: two identical displays are the one case where
  changing the primary display can move a configuration, because the ` #2`
  suffix follows listed order rather than the display itself. That is now
  stated instead of implied otherwise.

## What's New in v4.0.1

- **Fixed: stray text showing through the per-zone settings panel.** The
  preview's hover card was drawn into the same strip of the window that the
  per-zone fields occupy, so it was never readable and its lines leaked out
  through the gaps between the fields. The card is gone — the panel below the
  preview already shows those values for the selected zone, and clicking a
  zone in the preview selects it.
- **Fixed: tooltips were unreadable dark-on-dark.** A themed tooltip silently
  ignores the colour messages that were meant to restyle it, so it kept the
  system colours while the rest of the dashboard followed the palette.
  Tooltips now use the dashboard's own background, text colour and font, in
  both light and dark themes.
- The dashboard window now clips its children, so nothing painted by the
  window can leave residue underneath a control.
- **Fixed a crash risk when opening the dashboard.** It listed your monitors
  by reading the detection thread's own monitor table, which that thread
  clears and rebuilds whenever the display layout is re-checked — freeing the
  name strings mid-read. The names now travel inside the same immutable
  snapshot the detection loop already uses.

## What's New in v4.0.0

- **Per-zone settings.** Size, delay, pass-through guard, knock window,
  cooldown and required modifier can each be overridden for a single zone.
  Blank means inherit, so existing configurations behave exactly as before.
- The dashboard follows the system light/dark theme instead of being fixed
  dark, which made it near-unreadable on a light desktop.
- Preview fixes: edges are split around their centre blocks, so hovering a
  centre no longer highlights the whole edge.

## What's New in v3.9.0

- **Settings dashboard**, opened from the tray icon: all twelve zones per
  monitor, the global options, and a clickable preview of your screen.
  Windhawk's own settings page stays available and can be restored at any
  time with *Reset to Windhawk settings*.

## What's New in v3.8.0

- **Tray icon** with enable/suspend controls and quick access to the log.

## What's New in v3.7.0

- **Alternating actions** — one zone that runs two different actions on
  successive triggers, written as `first|second` in the argument field.
- **Keep zones off the taskbar**, so an edge zone stops at the work area
  instead of fighting the taskbar's peek-at-desktop strip.

## What's New in v3.6.0

- **Knock to activate** — require entering a zone twice in quick succession.
- **Modifier gating** — zones stay inert unless a chosen key is held.
- **Edge-centre zones**, plus four new actions.
- v3.6.1: the display actually blanks after *Lock and Turn Off Monitors*.
  `WM_SYSCOMMAND` was posted to the foreground window, which is null once
  `LockWorkStation` has switched desktop; it is now broadcast.

## What's New in v3.5.0

- First public release.

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

#include <windows.h>

#include <commctrl.h>
#include <windowsx.h>   // GET_X_LPARAM / GET_Y_LPARAM // windhawk_utils.h needs SUBCLASSPROC from here
#include <initializer_list>
#include <powrprof.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
// swprintf_s / _wtoi / memcmp are used directly; they only reached this file
// through <windows.h> before.
#include <cstdio>
#include <cstdlib>
#include <cstring>
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
    VDesktopClose,
    LockAndMonitorsOff,
    KeepAwakeOn,
    KeepAwakeOff,
    AlternateKeypress,
    AlternateCommand,
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
    ZONE_CENTER_TOP = 8,
    ZONE_CENTER_BOTTOM = 9,
    ZONE_CENTER_LEFT = 10,
    ZONE_CENTER_RIGHT = 11,
    ZONE_COUNT = 12,
};

// Per-zone overrides. Every numeric field uses -1 for "inherit the global
// value", so an untouched zone behaves exactly as it did before per-zone
// settings existed and old configurations keep working unchanged.
struct ZoneTuning
{
    int size = -1;      // corner square / edge strip thickness, px
    int delay = -1;     // activation delay, ms
    int settle = -1;    // pass-through guard, ms
    int knock = -1;     // knock window, ms
    int cooldown = -1;  // per-zone cooldown, ms
    int modifier = -1;  // 0 none, 1 Ctrl, 2 Alt, 3 Shift, 4 Win
};

struct ZoneConfig
{
    CornerAction action = CornerAction::Nothing;
    std::wstring args;
    std::function<void()> executor;
    ZoneTuning tuning;
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
    RECT rcWork;   // monitor minus taskbar and any docked appbars
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
    std::wstring label;  // "Dell U2720Q Top-left corner -> Task View"

    // Resolved once at build time - zone override if set, otherwise the
    // global. The detection loop therefore never has to know that per-zone
    // settings exist.
    int delay = 0;
    int settle = 80;
    int knock = 0;
    int cooldown = 300;
    int modifier = 0;
    Zone zone = ZONE_TOP_LEFT;

    // Which display this zone lives on, so the fullscreen guard can suppress
    // the display a game is actually on and leave the others alone.
    // ponytail: an HMONITOR, not a rect. It is only stale between a display
    // change and the rebuild that follows it, and a stale one simply fails to
    // match, which errs towards firing rather than towards silence.
    HMONITOR monitor = nullptr;
};

// The detection loop reads nothing but this snapshot, so it never touches
// g_settings and never takes a lock on the timing-critical path.
struct ZoneSet
{
    std::vector<HitZone> zones;
    // Friendly names of the displays this set was built from. They ride in the
    // snapshot so the dashboard thread can list monitors without reading
    // g_monitors, which the detection thread clears and refills underneath it.
    std::vector<std::wstring> monitorNames;
    // Only what the detection loop actually reads. The timings live on each
    // HitZone, resolved at build time, which is the whole point of resolving
    // them there; a second copy here would just be a second thing to keep
    // in step.
    bool disableDuringDrag = true;
};

// =====================================================================
// Globals
// =====================================================================

// Named, not anonymous, so ReloadConfig can build a whole configuration in a
// local and swap it in under one lock acquisition.
struct ModSettings
{
    int cornerSize = 6;
    int edgeSize = 6;
    int activationDelay = 0;
    int settleMs = 80;
    int knockWindowMs = 0;   // 0 = knock mode off
    int requireModifier = 0; // 0 none, 1 Ctrl, 2 Alt, 3 Shift, 4 Win
    int centerZonePercent = 20;
    bool avoidTaskbar = false;
    int cooldownMs = 300;
    bool disableOnFullscreen = true;
    bool disableDuringDrag = true;
    std::vector<std::wstring> excludedProcesses;
    std::vector<MonitorZoneConfig> monitorConfigs;
};
static ModSettings g_settings;

// g_settings is written by Windhawk's thread and read by the detection thread
// only when it rebuilds zones. Everything the detection loop needs at runtime
// lives in the published ZoneSet instead, so this is never taken per tick.
static CRITICAL_SECTION g_settingsLock;

// Published zone snapshot. Swapped wholesale on rebuild; readers take a
// shared_ptr copy, so an in-flight tick can never see a half-rebuilt vector.
static CRITICAL_SECTION g_zonesLock;
static std::shared_ptr<const ZoneSet> g_zones;

static std::vector<MonitorInfo> g_monitors; // detection thread only

// Detection thread. 16 ms whenever a zone could fire - see DetectTick for why
// there is no cleverness here. kIdleTickMs is used only on the paths where
// nothing can fire at all: the mod switched off, suspended, or no zones armed.
static constexpr DWORD kTickMs = 16;
static constexpr DWORD kIdleTickMs = 100;
static HANDLE g_hDetectThread = nullptr;
static DWORD g_dwDetectThreadId = 0;
static HWND g_hDetectWnd = nullptr;
static HANDLE g_hStopEvent = nullptr;

// Prints the monitor list at load and on display changes, so names can be
// copied into the monitor selector instead of guessed. Cheap - once per event.
static std::atomic<bool> g_showMonitorNames{true};

// Master switch and temporary suspend, both driven from the tray icon.
// Written by the tray thread and read by the detection thread every tick;
// atomic because this builds as 32-bit, where a plain 64-bit read can tear.
static std::atomic<bool> g_trayEnabled{true};
static std::atomic<ULONGLONG> g_suspendUntil{0};

// How long to wait after locking before blanking the display. Hardware
// dependent - the secure-desktop switch takes longer on some machines, and
// blanking before it settles just wakes the display again.
static std::atomic<int> g_lockBlankDelayMs{1200};

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
static bool g_knockSatisfied = true;
static std::vector<ULONGLONG> g_lastFireTick;
// When each zone was last left, for knock detection.
static std::vector<ULONGLONG> g_lastExitTick;

// Cached display topology, for catching layout changes Windows doesn't
// announce (docking, monitor wake, RDP reconnect).
static int g_topoCount = -1;
static RECT g_topoVirtual = {};
// Also tracked, so moving or auto-hiding the taskbar rebuilds the zones when
// they are being built from the work area.
static RECT g_topoWorkArea = {};

static constexpr UINT WM_APP_REBUILD = WM_APP + 1;

// Forward declarations
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
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;

    // The display layout can change between sizing the buffers and filling
    // them — which is exactly when this runs, since a layout change is what
    // calls it. The API reports that as ERROR_INSUFFICIENT_BUFFER and expects
    // the caller to size and query again. Without the retry every friendly
    // name is lost for that rebuild, and every zone bound to a name silently
    // stops matching until the next display change.
    // ponytail: 3 attempts. A layout that changes three times inside one
    // rebuild will fix itself on the next WM_DISPLAYCHANGE anyway.
    LONG qc = ERROR_INSUFFICIENT_BUFFER;
    for (int attempt = 0; attempt < 3 && qc == ERROR_INSUFFICIENT_BUFFER;
         attempt++)
    {
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                        &modeCount) != ERROR_SUCCESS)
        {
            Wh_Log(L"GetDisplayConfigBufferSizes failed");
            return;
        }

        paths.assign(pathCount, DISPLAYCONFIG_PATH_INFO{});
        modes.assign(modeCount, DISPLAYCONFIG_MODE_INFO{});
        qc = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                                &modeCount, modes.data(), nullptr);
    }
    if (qc != ERROR_SUCCESS)
    {
        Wh_Log(L"QueryDisplayConfig failed: %ld", qc);
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
        info.rcWork = mi.rcWork;
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
    Wh_Log(L"|  These are the displays in the dashboard's monitor selector.");
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

// "Something is fullscreen, but on which display" cannot always be answered.
// Suppressing everywhere is the safe reading, and is what this mod did on every
// display before it learned to tell them apart.
static HMONITOR const kAllMonitors = (HMONITOR)(INT_PTR)-1;

// The display a fullscreen app is on, or nullptr if nothing is fullscreen.
// A game on one screen used to disable the hot corners on every screen, which
// is the opposite of why anyone owns a second monitor.
static HMONITOR FullScreenMonitor()
{
    HWND hFgWnd = GetForegroundWindow();

    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)))
    {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN ||
            state == QUNS_PRESENTATION_MODE)
            return hFgWnd ? MonitorFromWindow(hFgWnd, MONITOR_DEFAULTTONEAREST)
                          : kAllMonitors;
    }

    // Fallback: exact bounds match for apps that go fullscreen without
    // D3D exclusive mode (browser F11, video players).
    if (!hFgWnd || hFgWnd == GetDesktopWindow() || hFgWnd == GetShellWindow())
        return nullptr;

    if (IsShellUiWindow(hFgWnd))
        return nullptr;

    RECT rcWnd;
    if (!GetWindowRect(hFgWnd, &rcWnd))
        return nullptr;

    HMONITOR hMon = MonitorFromWindow(hFgWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(hMon, &mi))
        return nullptr;

    // True fullscreen apps match the monitor exactly. Maximized desktop
    // apps overhang by ~8px due to DWM drop shadows — they won't match.
    bool full = (rcWnd.left == mi.rcMonitor.left &&
                 rcWnd.top == mi.rcMonitor.top &&
                 rcWnd.right == mi.rcMonitor.right &&
                 rcWnd.bottom == mi.rcMonitor.bottom);
    return full ? hMon : nullptr;
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
        // Not logged here: the one caller reports the skip, with the zone that
        // was suppressed, which is the part worth knowing.
        if (_wcsicmp(cachedName.c_str(), name.c_str()) == 0)
            return true;
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

    // A short SendInput means the action silently did nothing, so it is always
    // reported. Logging here is safe because it is once per trigger, not once
    // per poll: Wh_Log goes through OutputDebugString and takes a system-wide
    // lock, which would matter on a hot path and does not on this one.
    SetLastError(0);
    UINT sent = SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
    if (sent != inputs.size())
    {
        Wh_Log(L"SendInput FAILED: sent %u/%u, err=%lu (sizeof(INPUT)=%d)",
               sent, (UINT)inputs.size(), GetLastError(), (int)sizeof(INPUT));

        // A short send can stop between a key-down and its matching key-up,
        // which leaves that key logically held for the rest of the session —
        // a stuck Win or Ctrl the user cannot clear without logging out. The
        // release events are already built as the second half of the batch,
        // and releasing a key that is already up does nothing, so replay all
        // of them. Nothing was pressed if nothing was sent.
        if (sent > 0)
            SendInput((UINT)n, inputs.data() + n, sizeof(INPUT));
    }
    else
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

// SC_MONITORPOWER and SC_SCREENSAVE only take effect when they reach a window
// that passes them to DefWindowProc, which is what hands them to the power
// manager.
//
// Posting to GetForegroundWindow is unreliable: an application is free to
// swallow WM_SYSCOMMAND, and after LockWorkStation the input desktop has
// switched to Winlogon, so from our desktop it returns null and the old
// fallback posted to GetDesktopWindow — which handles nothing at all. That is
// why the display stayed awake after locking.
//
// Broadcasting reaches every top-level window, so at least one will route it.
// Runs on the worker thread, so the blocking call cannot delay detection.
// Broadcast, because posting to the foreground window does not work: after
// LockWorkStation there is no foreground window on this desktop, and the
// GetDesktopWindow fallback handles nothing.
//
// SendNotifyMessage rather than SendMessageTimeout: a broadcast applies the
// timeout to *every* top-level window in turn, so one slow process could hold
// the worker thread for seconds and stall every queued action behind it.
// SendNotifyMessage returns immediately for windows owned by other threads,
// which is all of them here.
static void BroadcastSysCommand(WPARAM command, LPARAM param)
{
    SendNotifyMessageW(HWND_BROADCAST, WM_SYSCOMMAND, command, param);
}

static void ActionScreenSaver()
{
    BroadcastSysCommand(SC_SCREENSAVE, 0);
}

static void ActionMonitorsOff()
{
    BroadcastSysCommand(SC_MONITORPOWER, (LPARAM)2);
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
static void ActionVDesktopClose() { SendKeys({VK_LWIN, VK_LCONTROL, VK_F4}); }

// Lock first, then blank. Blanking first tends to wake the display straight
// back up, because the switch to the lock screen counts as activity.
static void ActionLockAndMonitorsOff()
{
    LockWorkStation();

    // The lock transition itself counts as activity, so blanking too soon
    // just wakes the display straight back up. How long the switch to the
    // secure desktop takes varies by machine, hence the setting.
    Sleep((DWORD)g_lockBlankDelayMs.load());
    BroadcastSysCommand(SC_MONITORPOWER, (LPARAM)2);
}

// SetThreadExecutionState is per-thread and only holds while that thread
// lives, so these must run on the action worker — which they do, and which
// stays alive for the whole session. The worker clears the state on exit.
static void ActionKeepAwakeOn()
{
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED |
                            ES_SYSTEM_REQUIRED);
    Wh_Log(L"Keep awake ON - screensaver, display sleep and system sleep are "
           L"suppressed until you trigger Keep Awake Off.");
}

static void ActionKeepAwakeOff()
{
    SetThreadExecutionState(ES_CONTINUOUS);
    Wh_Log(L"Keep awake OFF - normal power behaviour restored.");
}

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

    // "uac;" with nothing after it would reach CommandLineToArgvW with an
    // empty string, which returns the *host process* path as argv[0] — so a
    // stray prefix would relaunch windhawk.exe elevated.
    if (cmd.empty())
        return;

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
        {L"ACTION_VDESK_CLOSE", CornerAction::VDesktopClose},
        {L"ACTION_LOCK_MONITORS_OFF", CornerAction::LockAndMonitorsOff},
        {L"ACTION_KEEP_AWAKE_ON", CornerAction::KeepAwakeOn},
        {L"ACTION_KEEP_AWAKE_OFF", CornerAction::KeepAwakeOff},
        {L"ACTION_ALTERNATE_KEYPRESS", CornerAction::AlternateKeypress},
        {L"ACTION_ALTERNATE_COMMAND", CornerAction::AlternateCommand},
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
    case CornerAction::VDesktopClose: return L"Virtual Desktop Close";
    case CornerAction::LockAndMonitorsOff: return L"Lock and Turn Off Monitors";
    case CornerAction::KeepAwakeOn: return L"Keep Awake On";
    case CornerAction::KeepAwakeOff: return L"Keep Awake Off";
    case CornerAction::AlternateKeypress: return L"Alternate Key Press";
    case CornerAction::AlternateCommand: return L"Alternate Command";
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
    case ZONE_CENTER_TOP: return L"Top edge centre";
    case ZONE_CENTER_BOTTOM: return L"Bottom edge centre";
    case ZONE_CENTER_LEFT: return L"Left edge centre";
    case ZONE_CENTER_RIGHT: return L"Right edge centre";
    default: return L"None";
    }
}

// Splits "a | b" into its two halves. Both must be non-empty.
static bool SplitAlternate(const std::wstring &args, std::wstring &first,
                           std::wstring &second)
{
    auto bar = args.find(L'|');
    if (bar == std::wstring::npos)
        return false;
    first = TrimStr(args.substr(0, bar));
    second = TrimStr(args.substr(bar + 1));
    return !first.empty() && !second.empty();
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
    case CornerAction::VDesktopClose: return ActionVDesktopClose;
    case CornerAction::LockAndMonitorsOff: return ActionLockAndMonitorsOff;
    case CornerAction::KeepAwakeOn: return ActionKeepAwakeOn;
    case CornerAction::KeepAwakeOff: return ActionKeepAwakeOff;
    // Alternating actions.
    //
    // Deliberately built as two extra action types rather than by giving every
    // zone a second action + args. That would have doubled a settings block
    // that is already twelve zones long, for a feature most zones will never
    // use. Here the two halves live in the existing Args field, split on "|",
    // so nothing about the zone structure, the hit test or the detection loop
    // changes at all.
    //
    // The alternation flag is a shared_ptr captured by the lambda, so it
    // travels with the executor and needs no per-zone state anywhere else.
    // It resets whenever the zone set is rebuilt — settings change, display
    // change — which is the documented behaviour.
    case CornerAction::AlternateKeypress:
    {
        std::wstring left, right;
        if (!SplitAlternate(args, left, right))
        {
            Wh_Log(L"Alternate Key Press: expected two combinations separated "
                   L"by | , for example  Alt+S | Alt+H");
            return nullptr;
        }
        auto first = ParseKeyCombo(left);
        auto second = ParseKeyCombo(right);
        if (first.empty() || second.empty())
        {
            Wh_Log(L"Alternate Key Press: could not parse both sides of '%s'",
                   args.c_str());
            return nullptr;
        }
        auto useSecond = std::make_shared<bool>(false);
        return [first, second, useSecond]()
        {
            const auto &combos = *useSecond ? second : first;
            *useSecond = !*useSecond;
            for (const auto &keys : combos)
                SendKeys(keys);
        };
    }
    case CornerAction::AlternateCommand:
    {
        std::wstring left, right;
        if (!SplitAlternate(args, left, right))
        {
            Wh_Log(L"Alternate Command: expected two commands separated by | ");
            return nullptr;
        }
        auto useSecond = std::make_shared<bool>(false);
        return [left, right, useSecond]()
        {
            const std::wstring &cmd = *useSecond ? right : left;
            *useSecond = !*useSecond;
            ActionStartProcess(cmd);
        };
    }
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

    set->disableDuringDrag = g_settings.disableDuringDrag;

    for (const auto &mon : g_monitors)
        set->monitorNames.push_back(mon.id);

    for (const auto &mon : g_monitors)
    {
        // rcWork is the monitor minus the taskbar and any docked appbars, so
        // using it is a complete fix for zones colliding with the taskbar,
        // including the taskbar's own "peek at desktop" strip.
        const RECT &r =
            g_settings.avoidTaskbar ? mon.rcWork : mon.rcMonitor;
        LONG centrePct = g_settings.centerZonePercent;

        int span = (r.right - r.left) < (r.bottom - r.top)
                       ? (r.right - r.left)
                       : (r.bottom - r.top);

        // Each zone may set its own size; -1 means inherit. Corners clamp to
        // half the screen, then each edge clamps to the smaller of the two
        // corners it runs between - that single rule is what keeps all twelve
        // zones disjoint no matter what sizes are chosen.
        auto zoneCfg = [&](Zone z) { return ResolveZone(mon, z); };
        auto sizeOf = [&](Zone z, int fallback) -> int
        {
            const ZoneConfig *zc = zoneCfg(z);
            int v = (zc && zc->tuning.size > 0) ? zc->tuning.size : fallback;
            if (v < 1)
                v = 1;
            if (v > span / 2)
                v = span / 2;
            if (v < 1)
                v = 1;
            return v;
        };

        int csTL = sizeOf(ZONE_TOP_LEFT, csCfg);
        int csTR = sizeOf(ZONE_TOP_RIGHT, csCfg);
        int csBL = sizeOf(ZONE_BOTTOM_LEFT, csCfg);
        int csBR = sizeOf(ZONE_BOTTOM_RIGHT, csCfg);

        auto edgeThickness = [&](Zone z, int a, int b) -> int
        {
            int v = sizeOf(z, esCfg);
            int cap = a < b ? a : b;
            return v > cap ? cap : v;
        };
        int esTop = edgeThickness(ZONE_EDGE_TOP, csTL, csTR);
        int esBot = edgeThickness(ZONE_EDGE_BOTTOM, csBL, csBR);
        int esLeft = edgeThickness(ZONE_EDGE_LEFT, csTL, csBL);
        int esRight = edgeThickness(ZONE_EDGE_RIGHT, csTR, csBR);

        // An edge with a centre zone becomes two rectangles, but it is still one
        // edge: walking into the left half and then the right must give A then
        // B, not A then A. MakeExecutor builds a fresh flip flag on every call,
        // so build one per zone here and hand out copies - copying a
        // std::function copies the captured shared_ptr, and that shared flag is
        // exactly the state the two halves need to agree on. Per monitor, so
        // two displays still alternate independently.
        std::function<void()> altExec[ZONE_COUNT];

        auto add = [&](Zone z, RECT rect)
        {
            const ZoneConfig *zc = ResolveZone(mon, z);
            if (!zc)
                return;
            HitZone hz;
            hz.rect = rect;
            hz.zone = z;
            hz.monitor = mon.handle;

            if (zc->action == CornerAction::AlternateKeypress ||
                zc->action == CornerAction::AlternateCommand)
            {
                if (!altExec[z])
                    altExec[z] = MakeExecutor(zc->action, zc->args);
                hz.exec = altExec[z];
                if (!hz.exec)
                    return;
            }
            else
            {
                hz.exec = zc->executor;
            }

            const ZoneTuning &tn = zc->tuning;
            hz.delay = tn.delay >= 0 ? tn.delay : g_settings.activationDelay;
            hz.settle = tn.settle >= 0 ? tn.settle : g_settings.settleMs;
            hz.knock = tn.knock >= 0 ? tn.knock : g_settings.knockWindowMs;
            hz.cooldown = tn.cooldown >= 0 ? tn.cooldown : g_settings.cooldownMs;
            hz.modifier =
                tn.modifier >= 0 ? tn.modifier : g_settings.requireModifier;

            hz.label = mon.id + L" " + ZoneToString(z) + L" -> " +
                       ActionToString(zc->action);
            set->zones.push_back(std::move(hz));
        };

        add(ZONE_TOP_LEFT, {r.left, r.top, r.left + csTL, r.top + csTL});
        add(ZONE_TOP_RIGHT, {r.right - csTR, r.top, r.right, r.top + csTR});
        add(ZONE_BOTTOM_LEFT, {r.left, r.bottom - csBL, r.left + csBL, r.bottom});
        add(ZONE_BOTTOM_RIGHT,
            {r.right - csBR, r.bottom - csBR, r.right, r.bottom});

        // Edges run between the two corners they touch, split around a centre
        // zone when one is configured.
        auto addEdge = [&](Zone edge, Zone centre, bool horizontal, LONG lo,
                           LONG hi, LONG nearSide, LONG farSide)
        {
            auto rectFor = [&](LONG a, LONG b) -> RECT
            {
                return horizontal ? RECT{a, nearSide, b, farSide}
                                  : RECT{nearSide, a, farSide, b};
            };

            const ZoneConfig *centreCfg = ResolveZone(mon, centre);
            LONG sp = hi - lo;
            LONG width = sp * centrePct / 100;

            if (!centreCfg || width < 1 || width >= sp || sp <= 0)
            {
                if (sp > 0)
                    add(edge, rectFor(lo, hi));
                return;
            }

            LONG mid = lo + sp / 2;
            LONG cLo = mid - width / 2;
            LONG cHi = cLo + width;

            if (cLo > lo)
                add(edge, rectFor(lo, cLo));
            add(centre, rectFor(cLo, cHi));
            if (cHi < hi)
                add(edge, rectFor(cHi, hi));
        };

        addEdge(ZONE_EDGE_TOP, ZONE_CENTER_TOP, true, r.left + csTL,
                r.right - csTR, r.top, r.top + esTop);
        addEdge(ZONE_EDGE_BOTTOM, ZONE_CENTER_BOTTOM, true, r.left + csBL,
                r.right - csBR, r.bottom - esBot, r.bottom);
        addEdge(ZONE_EDGE_LEFT, ZONE_CENTER_LEFT, false, r.top + csTL,
                r.bottom - csBL, r.left, r.left + esLeft);
        addEdge(ZONE_EDGE_RIGHT, ZONE_CENTER_RIGHT, false, r.top + csTR,
                r.bottom - csBR, r.right - esRight, r.right);
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
    g_lastExitTick.assign(set->zones.size(), 0);
    g_knockSatisfied = true;

    g_topoCount = GetSystemMetrics(SM_CMONITORS);
    g_topoVirtual = {GetSystemMetrics(SM_XVIRTUALSCREEN),
                     GetSystemMetrics(SM_YVIRTUALSCREEN),
                     GetSystemMetrics(SM_CXVIRTUALSCREEN),
                     GetSystemMetrics(SM_CYVIRTUALSCREEN)};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &g_topoWorkArea, 0);
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
    RECT work = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);

    return GetSystemMetrics(SM_CMONITORS) != g_topoCount ||
           memcmp(&v, &g_topoVirtual, sizeof(v)) != 0 ||
           memcmp(&work, &g_topoWorkArea, sizeof(work)) != 0;
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
        {
            // Release any keep-awake request this thread was holding, so
            // unloading the mod never leaves the machine unable to sleep.
            SetThreadExecutionState(ES_CONTINUOUS);
            break;
        }

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

            // Suppression is normal, not an error, and it spams hardest in the
            // case it matters least: parking the cursor in a corner while a
            // fullscreen game has focus re-queues the job every cooldown.
            // Report the first skip of a run, then stay quiet until something
            // actually fires. Only this thread touches the flag.
            static bool skipLogged = false;
            HMONITOR fsMon = checkFullscreen ? FullScreenMonitor() : nullptr;
            if (fsMon && (fsMon == kAllMonitors || fsMon == job.monitor))
            {
                if (!skipLogged)
                    Wh_Log(L"SKIP (fullscreen): %s", job.label.c_str());
                skipLogged = true;
                continue;
            }
            if (IsForegroundAppExcluded(excluded))
            {
                if (!skipLogged)
                    Wh_Log(L"SKIP (excluded app): %s", job.label.c_str());
                skipLogged = true;
                continue;
            }
            skipLogged = false;

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

// 0 none, 1 Ctrl, 2 Alt, 3 Shift, 4 Win
static bool RequiredModifierHeld(int which)
{
    switch (which)
    {
    case 1: return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    case 2: return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    case 3: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    case 4: return (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
                   (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    default: return true;  // no modifier required
    }
}

static bool AnyMouseButtonDown()
{
    return (GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_RBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_MBUTTON) & 0x8000) ||
           (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) ||
           (GetAsyncKeyState(VK_XBUTTON2) & 0x8000);
}

// Returns how long the caller should wait before ticking again.
static DWORD DetectTick()
{
    if (!g_trayEnabled.load() || GetTickCount64() < g_suspendUntil.load())
        return kIdleTickMs;

    std::shared_ptr<const ZoneSet> zones;
    EnterCriticalSection(&g_zonesLock);
    zones = g_zones;
    LeaveCriticalSection(&g_zonesLock);

    if (!zones || zones->zones.empty())
        return kIdleTickMs;

    POINT pt;
    if (!GetCursorPos(&pt))
        return kIdleTickMs;

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

    // Full rate whenever a zone could fire, with no exceptions. Two attempts at
    // easing off while the cursor was far away are gone: both scheduled a long
    // sleep from a sample taken *before* the user started moving, so a flick
    // could cross a zone entirely between two samples. On the outer perimeter
    // that only costs a late trigger, because the pointer stops against the
    // screen edge - but a corner shared with a second monitor has no edge to
    // stop against, and there it was a lost one. No polling interval can close
    // that; only an event source could, and 16 ms of GetCursorPos plus a dozen
    // rectangle compares was never the cost worth taking the risk for.
    ULONGLONG now = GetTickCount64();
    const DWORD next = kTickMs;

    // Enter/leave edge detection, the same shape macOS uses: a zone fires
    // once on entry and re-arms only after the cursor leaves it.
    if (idx != g_activeZone)
    {
        // Record when we left the previous zone, so a quick return to it can
        // be recognised as the second half of a knock.
        if (g_activeZone >= 0 && g_activeZone < (int)g_lastExitTick.size())
            g_lastExitTick[g_activeZone] = now;

        g_activeZone = idx;
        g_enterTick = now;
        g_firedThisEntry = false;

        // Knock mode: a single entry never fires. The zone only arms when it
        // is re-entered soon after being left.
        int knockMs = (idx >= 0) ? zones->zones[idx].knock : 0;
        g_knockSatisfied =
            knockMs <= 0 || idx < 0 ||
            (idx < (int)g_lastExitTick.size() && g_lastExitTick[idx] != 0 &&
             (now - g_lastExitTick[idx]) <= (ULONGLONG)knockMs);
    }

    if (idx < 0 || g_firedThisEntry)
        return next;

    if (!g_knockSatisfied)
        return next;

    // Checked every tick rather than on entry, so the zone becomes live the
    // moment the modifier goes down while the cursor is already parked.
    if (!RequiredModifierHeld(zones->zones[idx].modifier))
        return next;

    // Suppress for the whole visit, not just this tick — otherwise releasing
    // a drag inside a corner would immediately trigger it.
    const HitZone &hz = zones->zones[idx];

    if (zones->disableDuringDrag && AnyMouseButtonDown())
    {
        g_firedThisEntry = true;
        return next;
    }

    // A corner cannot be reached without crossing the edge strip next to it,
    // so a fast transit would fire the edge and then the corner milliseconds
    // apart. With a toggle action bound to both (e.g. Task View) that opens
    // and instantly re-closes, looking exactly like nothing happened.
    // Requiring the cursor to settle means a pass-through never fires; only
    // the zone you actually stop in does.
    int dwell = hz.delay > hz.settle ? hz.delay : hz.settle;
    if (dwell > 0 && (now - g_enterTick) < (ULONGLONG)dwell)
        return next;

    if (hz.cooldown > 0 && idx < (int)g_lastFireTick.size())
    {
        ULONGLONG last = g_lastFireTick[idx];
        if (last != 0 && (now - last) < (ULONGLONG)hz.cooldown)
        {
            g_firedThisEntry = true;
            return next;
        }
    }

    // Global floor across all zones. The per-zone cooldown alone does not stop
    // a sweep through several different zones from queueing a burst.
    if (g_lastAnyFireTick != 0 &&
        (now - g_lastAnyFireTick) < kMinFireIntervalMs)
    {
        g_firedThisEntry = true;
        return next;
    }

    g_firedThisEntry = true;
    g_lastAnyFireTick = now;
    if (idx < (int)g_lastFireTick.size())
        g_lastFireTick[idx] = now;

    EnqueueAction(zones->zones[idx]);
    return next;
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

        if (WaitForSingleObject(g_hStopEvent, DetectTick()) == WAIT_OBJECT_0)
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
// Tray Icon
// =====================================================================
//
// Runs on its own thread with its own window and message loop. That is not
// decoration: TrackPopupMenu is modal and does not return until the menu
// closes, so hosting it on the detection thread would freeze every zone for
// as long as the menu is open.
//
// The icon is also the only way in: this mod has no Windhawk settings page, so
// left-click toggles, right-click is the quick menu, and the menu's first item
// opens the dashboard where everything else lives.

static HANDLE g_hTrayThread = nullptr;
static DWORD g_dwTrayThreadId = 0;
static HWND g_hTrayWnd = nullptr;
static UINT g_taskbarCreatedMsg = 0;
static constexpr UINT WM_APP_TRAY = WM_APP + 10;
static constexpr UINT_PTR kTrayIconId = 1;

// A stable GUID gives the icon an identity of its own. Without one it is keyed
// on the host executable, which for a Windhawk tool mod is windhawk.exe and is
// therefore shared with every other tray-owning mod — so Windows cannot tell
// our show/hide preference apart from theirs.
static const GUID kTrayIconGuid = {
    0x7c15402a, 0xfbae, 0x41d8,
    {0xad, 0x28, 0x46, 0xad, 0x02, 0x40, 0x8d, 0x73}};

// Set once NIM_ADD succeeds. NIF_GUID also validates the registering
// executable's path, so if it is ever rejected we fall back to plain uID
// identity rather than losing the icon entirely.
static bool g_trayUseGuid = true;

// NOTIFYICON_VERSION_4 changes what the shell sends:
//   left-click  -> NIN_SELECT      (rather than WM_LBUTTONUP)
//   right-click -> WM_CONTEXTMENU  (rather than WM_RBUTTONUP)
//   keyboard    -> NIN_KEYSELECT
// and it packs the cursor position into wParam. Both the old and new forms are
// accepted below so the icon behaves identically if the version call fails.
#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif
#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT (WM_USER + 1)
#endif
#ifndef NOTIFYICON_VERSION_4
#define NOTIFYICON_VERSION_4 4
#endif
#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif

enum TrayCommand
{
    IDM_SETTINGS = 100,
    IDM_ENABLED,
    IDM_SUSPEND_15,
    IDM_SUSPEND_30,
    IDM_SUSPEND_60,
    IDM_RESUME,
    IDM_FULLSCREEN,
    IDM_DRAG,
    IDM_CLEAR_OVERRIDES,
    IDM_ABOUT,
};

// Defined further down with the dashboard; the tray menu needs it earlier.
static void OpenDashboard();

// -1 means "no override, use the Windhawk setting"
static const wchar_t *kOvrEnabled = L"ovr_enabled";
static const wchar_t *kOvrFullscreen = L"ovr_fullscreen";
static const wchar_t *kOvrDrag = L"ovr_drag";

static HICON MakeTrayIcon(bool enabled)
{
    int sz = GetSystemMetrics(SM_CXSMICON);
    if (sz < 8)
        sz = 16;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = sz;
    bmi.bmiHeader.biHeight = -sz;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void *bits = nullptr;
    HDC hdc = GetDC(nullptr);
    HBITMAP hColor =
        CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);
    if (!hColor || !bits)
    {
        if (hColor)
            DeleteObject(hColor);
        return nullptr;
    }

    // A screen outline with one corner lit. Mid-tone colours so it stays
    // legible on both a light and a dark taskbar.
    DWORD *px = static_cast<DWORD *>(bits);
    const DWORD frame = enabled ? 0xFFB0B0B0 : 0xFF707070;
    const DWORD accent = enabled ? 0xFF4CC2FF : 0xFF707070;
    int block = sz / 3;
    if (block < 3)
        block = 3;

    for (int y = 0; y < sz; y++)
    {
        for (int x = 0; x < sz; x++)
        {
            DWORD c = 0;  // transparent
            if (x == 0 || y == 0 || x == sz - 1 || y == sz - 1)
                c = frame;
            if (x < block && y < block)
                c = accent;
            px[y * sz + x] = c;
        }
    }

    // Explicitly zeroed. Transparency here comes from the colour bitmap's
    // alpha channel, so undefined mask bits happen to work, but "happens to"
    // is not a property worth relying on.
    std::vector<BYTE> maskBits(((sz + 15) / 16) * 2 * sz, 0);
    HBITMAP hMask = CreateBitmap(sz, sz, 1, 1, maskBits.data());
    ICONINFO ii = {};
    ii.fIcon = TRUE;
    ii.hbmColor = hColor;
    ii.hbmMask = hMask;
    HICON hIcon = CreateIconIndirect(&ii);

    DeleteObject(hColor);
    if (hMask)
        DeleteObject(hMask);
    return hIcon;
}

static void FillTrayIconData(NOTIFYICONDATAW &nid)
{
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hTrayWnd;
    nid.uID = (UINT)kTrayIconId;
    if (g_trayUseGuid)
    {
        nid.uFlags |= NIF_GUID;
        nid.guidItem = kTrayIconGuid;
    }
}

static void UpdateTrayIcon(bool add)
{
    if (!g_hTrayWnd)
        return;

    bool active = g_trayEnabled && GetTickCount64() >= g_suspendUntil.load();

    NOTIFYICONDATAW nid = {};
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    FillTrayIconData(nid);
    nid.uCallbackMessage = WM_APP_TRAY;
    nid.hIcon = MakeTrayIcon(active);
    wcscpy_s(nid.szTip, active ? L"Win-X Hot Corners - active"
                               : L"Win-X Hot Corners - paused");

    BOOL ok = Shell_NotifyIconW(add ? NIM_ADD : NIM_MODIFY, &nid);

    // NIF_GUID ties the icon to the registering executable's path. If the
    // shell rejects it, drop the GUID and retry rather than silently ending up
    // with no icon at all.
    if (!ok && add && g_trayUseGuid)
    {
        Wh_Log(L"Tray: GUID identity refused, falling back to plain icon id");
        g_trayUseGuid = false;
        NOTIFYICONDATAW retry = {};
        retry.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
        FillTrayIconData(retry);
        retry.uCallbackMessage = WM_APP_TRAY;
        retry.hIcon = nid.hIcon;
        wcscpy_s(retry.szTip, nid.szTip);
        ok = Shell_NotifyIconW(NIM_ADD, &retry);
    }

    // Opt into the modern notification behaviour. Only meaningful right after
    // the icon is added.
    if (ok && add)
    {
        NOTIFYICONDATAW ver = {};
        ver.uFlags = 0;
        FillTrayIconData(ver);
        ver.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &ver);
    }

    if (nid.hIcon)
        DestroyIcon(nid.hIcon);
}

// =====================================================================
// Dashboard persistence
// =====================================================================
//
// This is the whole of the mod's persistence. There is no Windhawk settings
// page to fall back on, so what is written here is what the mod runs with, and
// a key that has never been written falls back to the default ReloadConfig set.
//
// One key per field rather than a packed string: wordy, but it survives partial
// writes and is readable if anything ever needs debugging by hand.

static constexpr int kMaxGuiConfigs = 8;

static std::wstring GetStrValue(const wchar_t *name)
{
    WCHAR buf[512] = {};
    if (Wh_GetStringValue(name, buf, ARRAYSIZE(buf)) == 0)
        return L"";
    return buf;
}

static std::wstring GuiKey(int cfg, int zone, const wchar_t *what)
{
    wchar_t k[64];
    if (zone < 0)
        _snwprintf_s(k, _countof(k), _TRUNCATE, L"g%d.%s", cfg, what);
    else
        _snwprintf_s(k, _countof(k), _TRUNCATE, L"g%d.z%d.%s", cfg, zone, what);
    return k;
}

// The saved zone layout, or an empty vector if there is not one yet. Returns it
// rather than assigning g_settings.monitorConfigs: this reads several hundred
// value-store keys and builds a std::function per zone, and holding
// g_settingsLock across all of that would stall the detection thread, the
// action worker and the tray menu.
static std::vector<MonitorZoneConfig> ReadDashboardZones()
{
    std::vector<MonitorZoneConfig> configs;
    if (Wh_GetIntValue(L"gui_active", 0) == 0)
        return configs;

    for (int i = 0; i < kMaxGuiConfigs; i++)
    {
        std::wstring id = GetStrValue(GuiKey(i, -1, L"id").c_str());
        if (id.empty())
            continue;

        MonitorZoneConfig cfg;
        cfg.monitorId = (id == L"(unused)") ? L"" : id;
        cfg.monitorIndex = 0;

        bool any = false;
        for (int z = 0; z < ZONE_COUNT; z++)
        {
            std::wstring a = GetStrValue(GuiKey(i, z, L"a").c_str());
            std::wstring g = GetStrValue(GuiKey(i, z, L"g").c_str());
            CornerAction act = ParseActionType(a);
            cfg.zones[z].action = act;
            cfg.zones[z].args = g;
            cfg.zones[z].executor = MakeExecutor(act, g);
            cfg.zones[z].tuning.size = Wh_GetIntValue(GuiKey(i, z, L"sz").c_str(), -1);
            cfg.zones[z].tuning.delay = Wh_GetIntValue(GuiKey(i, z, L"dl").c_str(), -1);
            cfg.zones[z].tuning.settle = Wh_GetIntValue(GuiKey(i, z, L"gd").c_str(), -1);
            cfg.zones[z].tuning.knock = Wh_GetIntValue(GuiKey(i, z, L"kn").c_str(), -1);
            cfg.zones[z].tuning.cooldown = Wh_GetIntValue(GuiKey(i, z, L"cd").c_str(), -1);
            cfg.zones[z].tuning.modifier = Wh_GetIntValue(GuiKey(i, z, L"md").c_str(), -1);
            if (act != CornerAction::Nothing)
                any = true;
        }
        if (any)
            configs.push_back(std::move(cfg));
    }
    return configs;
}

// Back to a fresh install. -1 is the "never written" marker every reader tests
// for, so this is a reset rather than a write of zeroes.
static void ClearStoredConfig()
{
    // Every scalar the dashboard and the tray can write. Missing one here used
    // to leave it behind after a Reset, still applied but no longer visible.
    for (const wchar_t *k :
         {kOvrEnabled, kOvrFullscreen, kOvrDrag, L"ovr_corner", L"ovr_edge",
          L"ovr_delay", L"ovr_settle", L"ovr_knock", L"ovr_cooldown",
          L"ovr_centre", L"ovr_modifier", L"ovr_lockblank", L"ovr_taskbar",
          L"ovr_monnames"})
        Wh_SetIntValue(k, -1);
    Wh_SetStringValue(L"ovr_excluded", L"");

    Wh_SetIntValue(L"gui_active", 0);
    for (int i = 0; i < kMaxGuiConfigs; i++)
    {
        Wh_SetStringValue(GuiKey(i, -1, L"id").c_str(), L"");
        for (int z = 0; z < ZONE_COUNT; z++)
        {
            Wh_SetStringValue(GuiKey(i, z, L"a").c_str(), L"");
            Wh_SetStringValue(GuiKey(i, z, L"g").c_str(), L"");
            for (const wchar_t *k : {L"sz", L"dl", L"gd", L"kn", L"cd", L"md"})
                Wh_SetIntValue(GuiKey(i, z, k).c_str(), -1);
        }
    }
}

// The whole of this mod's configuration, read and applied as one transaction.
//
// Built into a local first, with no lock held: this reads every key in the
// value store and constructs a std::function per zone, and three other threads
// want g_settingsLock while that happens - the detection thread in
// BuildZoneSet, the action worker before every trigger, and the tray thread
// building its menu. The lock is taken once, for the swap.
//
// Doing it in two phases (defaults, then the stored values on top) left a
// window in which g_settings was a complete, plausible, *wrong* configuration:
// a rebuild landing in that window published an empty zone set and logged "No
// zones are active". Four of the six callers run on a thread other than the
// one that starts the mod, so that window was reachable.
//
// Keys still carry the historical "ovr_" prefix - back when there was a
// Windhawk settings page these were overrides on top of it. They are the
// configuration itself now; the prefix stays so an existing setup is not
// orphaned. An unset key reads back as -1, which fails every range test below,
// so a value the user has never touched simply keeps its default.
static void ReloadConfig()
{
    ModSettings s;   // the member initialisers are the defaults

    int v = Wh_GetIntValue(kOvrEnabled, -1);
    g_trayEnabled = (v < 0) ? true : (v != 0);

    v = Wh_GetIntValue(kOvrFullscreen, -1);
    if (v >= 0)
        s.disableOnFullscreen = (v != 0);

    v = Wh_GetIntValue(kOvrDrag, -1);
    if (v >= 0)
        s.disableDuringDrag = (v != 0);

    auto pull = [](const wchar_t *k, int &dst, int lo, int hi)
    {
        int x = Wh_GetIntValue(k, -1);
        if (x >= lo && x <= hi)
            dst = x;
    };
    pull(L"ovr_corner", s.cornerSize, 1, 500);
    pull(L"ovr_edge", s.edgeSize, 1, 500);
    pull(L"ovr_delay", s.activationDelay, 0, 10000);
    pull(L"ovr_settle", s.settleMs, 0, 10000);
    pull(L"ovr_knock", s.knockWindowMs, 0, 10000);
    pull(L"ovr_cooldown", s.cooldownMs, 0, 60000);
    pull(L"ovr_centre", s.centerZonePercent, 1, 90);
    pull(L"ovr_modifier", s.requireModifier, 0, 4);

    int x = Wh_GetIntValue(L"ovr_taskbar", -1);
    if (x >= 0)
        s.avoidTaskbar = (x != 0);

    // Via a local: pull takes int&, and these two globals are atomic because
    // other threads read them while this runs.
    int lockBlank = 1200;
    pull(L"ovr_lockblank", lockBlank, 0, 10000);
    g_lockBlankDelayMs = lockBlank;
    x = Wh_GetIntValue(L"ovr_monnames", -1);
    g_showMonitorNames = (x < 0) ? true : (x != 0);

    // s starts empty, so clearing the field in the dashboard genuinely clears
    // the list - no "if the stored string is non-empty" guard needed.
    std::wstring rest = GetStrValue(L"ovr_excluded");
    while (!rest.empty())
    {
        auto semi = rest.find(L';');
        std::wstring tok;
        if (semi != std::wstring::npos)
        {
            tok = TrimStr(rest.substr(0, semi));
            rest = rest.substr(semi + 1);
        }
        else
        {
            tok = TrimStr(rest);
            rest.clear();
        }
        if (!tok.empty())
            s.excludedProcesses.push_back(ToLowerStr(tok));
    }

    s.monitorConfigs = ReadDashboardZones();

    // Copied out before the move, so the summary can be logged after the lock
    // is released - Wh_Log goes through OutputDebugString and takes a
    // machine-wide lock of its own.
    const int zoneCount = (int)s.monitorConfigs.size();
    const int cs = s.cornerSize, es = s.edgeSize, dl = s.activationDelay;
    const int st = s.settleMs, cd = s.cooldownMs;
    const int nx = (int)s.excludedProcesses.size();
    const bool fs = s.disableOnFullscreen, dg = s.disableDuringDrag;

    EnterCriticalSection(&g_settingsLock);
    g_settings = std::move(s);
    LeaveCriticalSection(&g_settingsLock);

    if (zoneCount)
        Wh_Log(L"Using the dashboard's zone layout (%d configuration%s)",
               zoneCount, zoneCount == 1 ? L"" : L"s");
    Wh_Log(L"Sizes: corner %dpx, edge %dpx.  Timing: delay %dms, "
           L"pass-through guard %dms, cooldown %dms.",
           cs, es, dl, st, cd);
    Wh_Log(L"Skip while fullscreen: %s.  Skip while dragging: %s.  "
           L"Excluded apps: %d.",
           fs ? L"yes" : L"no", dg ? L"yes" : L"no", nx);
}

static void ShowTrayMenu(POINT pt)
{
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu)
        return;

    bool suspended = GetTickCount64() < g_suspendUntil.load();

    AppendMenuW(hMenu, MF_STRING, IDM_SETTINGS, L"Zones && settings...");
    SetMenuDefaultItem(hMenu, IDM_SETTINGS, FALSE);
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING | (g_trayEnabled ? MF_CHECKED : 0),
                IDM_ENABLED, L"Hot corners enabled");

    HMENU hSuspend = CreatePopupMenu();
    AppendMenuW(hSuspend, MF_STRING, IDM_SUSPEND_15, L"15 minutes");
    AppendMenuW(hSuspend, MF_STRING, IDM_SUSPEND_30, L"30 minutes");
    AppendMenuW(hSuspend, MF_STRING, IDM_SUSPEND_60, L"1 hour");
    if (suspended)
    {
        AppendMenuW(hSuspend, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hSuspend, MF_STRING, IDM_RESUME, L"Resume now");
    }
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSuspend,
                suspended ? L"Suspended..." : L"Suspend for");

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

    EnterCriticalSection(&g_settingsLock);
    bool fs = g_settings.disableOnFullscreen;
    bool drag = g_settings.disableDuringDrag;
    LeaveCriticalSection(&g_settingsLock);

    AppendMenuW(hMenu, MF_STRING | (fs ? MF_CHECKED : 0), IDM_FULLSCREEN,
                L"Skip while an app is fullscreen");
    AppendMenuW(hMenu, MF_STRING | (drag ? MF_CHECKED : 0), IDM_DRAG,
                L"Skip while dragging the mouse");

    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_CLEAR_OVERRIDES,
                L"Reset these toggles");
    AppendMenuW(hMenu, MF_STRING | MF_DISABLED, IDM_ABOUT,
                L"Win-X Hot Corners " WH_MOD_VERSION);

    // Required so the menu dismisses when the user clicks elsewhere.
    SetForegroundWindow(g_hTrayWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hTrayWnd, nullptr);
    PostMessage(g_hTrayWnd, WM_NULL, 0, 0);

    DestroyMenu(hMenu);
}

// The zone snapshot carries drag/settle, so a change there needs a rebuild.
static void RequestRebuild()
{
    if (g_hDetectWnd)
        PostMessage(g_hDetectWnd, WM_APP_REBUILD, 0, 0);
}

static void HandleTrayCommand(UINT id)
{
    switch (id)
    {
    case IDM_SETTINGS:
        OpenDashboard();
        return;

    case IDM_ENABLED:
        g_trayEnabled = !g_trayEnabled;
        Wh_SetIntValue(kOvrEnabled, g_trayEnabled ? 1 : 0);
        g_suspendUntil = 0;
        Wh_Log(L"Tray: hot corners %s", g_trayEnabled ? L"enabled" : L"disabled");
        break;

    case IDM_SUSPEND_15:
    case IDM_SUSPEND_30:
    case IDM_SUSPEND_60:
    {
        int mins = (id == IDM_SUSPEND_15) ? 15 : (id == IDM_SUSPEND_30) ? 30 : 60;
        g_suspendUntil = GetTickCount64() + (ULONGLONG)mins * 60 * 1000;
        Wh_Log(L"Tray: suspended for %d minutes", mins);
        break;
    }

    case IDM_RESUME:
        g_suspendUntil = 0;
        Wh_Log(L"Tray: resumed");
        break;

    case IDM_FULLSCREEN:
    {
        EnterCriticalSection(&g_settingsLock);
        g_settings.disableOnFullscreen = !g_settings.disableOnFullscreen;
        int v = g_settings.disableOnFullscreen ? 1 : 0;
        LeaveCriticalSection(&g_settingsLock);
        Wh_SetIntValue(kOvrFullscreen, v);
        break;
    }

    case IDM_DRAG:
    {
        EnterCriticalSection(&g_settingsLock);
        g_settings.disableDuringDrag = !g_settings.disableDuringDrag;
        int v = g_settings.disableDuringDrag ? 1 : 0;
        LeaveCriticalSection(&g_settingsLock);
        Wh_SetIntValue(kOvrDrag, v);
        RequestRebuild();
        break;
    }

    // Only the three toggles in this menu, and the suspend timer. Wiping the
    // zone layout from a menu item with no confirmation would be a trap; the
    // dashboard's Reset button does that, behind a prompt.
    case IDM_CLEAR_OVERRIDES:
        Wh_SetIntValue(kOvrEnabled, -1);
        Wh_SetIntValue(kOvrFullscreen, -1);
        Wh_SetIntValue(kOvrDrag, -1);
        g_suspendUntil = 0;
        ReloadConfig();
        RequestRebuild();
        Wh_Log(L"Tray: toggles reset to defaults");
        break;

    default:
        return;
    }

    UpdateTrayIcon(false);
}

// =====================================================================
// Settings dashboard
// =====================================================================
//
// A real window rather than a context menu, so every setting is reachable
// without opening Windhawk. Runs on its own thread with its own message loop
// and IsDialogMessage, so Tab/arrow navigation works and nothing it does can
// stall detection.
//
// Mica is requested where the OS supports it, but classic Win32 controls paint
// opaque, so the window is also fully dark-themed — that, not the backdrop, is
// what makes it look native.

static HANDLE g_hDashThread = nullptr;
static HWND g_hDashWnd = nullptr;

// The dashboard followed the system theme in neither direction before: it was
// hard-coded dark, so on a light desktop the text was near-invisible. The
// palette is now built from the user's actual preference and rebuilt when they
// change it.
struct Palette
{
    COLORREF bg, panel, text, dim, field, fieldText, border, accent, accentText;
};
static Palette g_pal;
static bool g_lightTheme = false;

static bool SystemUsesLightTheme()
{
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                      L"Personalize",
                      0, KEY_QUERY_VALUE, &k) != ERROR_SUCCESS)
        return false;
    DWORD v = 0, cb = sizeof(v), type = 0;
    bool light = false;
    if (RegQueryValueExW(k, L"AppsUseLightTheme", nullptr, &type, (LPBYTE)&v,
                         &cb) == ERROR_SUCCESS &&
        type == REG_DWORD)
        light = (v != 0);
    RegCloseKey(k);
    return light;
}

static void BuildPalette()
{
    g_lightTheme = SystemUsesLightTheme();
    if (g_lightTheme)
    {
        g_pal = {RGB(243, 243, 243), RGB(251, 251, 251), RGB(26, 26, 26),
                 RGB(95, 95, 95),    RGB(255, 255, 255), RGB(26, 26, 26),
                 RGB(214, 214, 214), RGB(0, 95, 184),    RGB(255, 255, 255)};
    }
    else
    {
        g_pal = {RGB(32, 32, 32),    RGB(43, 43, 43),   RGB(255, 255, 255),
                 RGB(170, 170, 170), RGB(45, 45, 45),   RGB(255, 255, 255),
                 RGB(70, 70, 70),    RGB(76, 194, 255), RGB(0, 0, 0)};
    }
}

enum DashId
{
    IDC_MONITOR = 1000,
    IDC_PAGE_ZONES,
    IDC_PAGE_OPTIONS,
    IDC_SAVE,
    IDC_CANCEL,
    IDC_RESET,
    IDC_ZONE_ACTION = 1100,           // + zone index
    IDC_ZONE_ARGS = 1200,             // + zone index
    IDC_HDR_ZONE = 1250,
    IDC_HDR_ACTION,
    IDC_HDR_ARGS,
    IDC_TZ_TITLE,
    IDC_TZ_HINT,
    IDC_TZ_FIRST = 1260,   // six per-zone override fields
    IDC_TZ_SIZE = IDC_TZ_FIRST,
    IDC_TZ_DELAY,
    IDC_TZ_SETTLE,
    IDC_TZ_KNOCK,
    IDC_TZ_COOLDOWN,
    IDC_TZ_MODIFIER,
    IDC_TZ_LAST,
    // This order is the order they appear on the Options page, and it has to
    // stay in step with kOpts: DashSetInt indexes hOpt by (id - IDC_OPT_FIRST).
    // A static_assert next to kOpts enforces it.
    IDC_OPT_FIRST = 1300,
    IDC_CORNER = IDC_OPT_FIRST,
    IDC_EDGE,
    IDC_CENTREPCT,
    IDC_DELAY,
    IDC_SETTLE,
    IDC_KNOCK,
    IDC_COOLDOWN,
    IDC_MODIFIER,
    IDC_EXCLUDED,
    IDC_CB_FULLSCREEN,
    IDC_CB_DRAG,
    IDC_CB_TASKBAR,
    IDC_LOCKBLANK,
    IDC_CB_MONNAMES,
    IDC_OPT_LAST,
    IDC_OPT_SECTION,   // shared by every group heading; they carry no state
};

struct DashState
{
    UINT dpi = 96;
    HFONT hFont = nullptr;
    HBRUSH hBg = nullptr;
    HBRUSH hField = nullptr;
    HBRUSH hPanel = nullptr;
    bool showZones = true;
    int hoverZone = -1;   // zone highlighted in the preview
    int selZone = 0;      // zone whose per-zone settings are being edited
    ZoneTuning tuning[ZONE_COUNT];   // the slot currently on screen
    int cfgIndex = 0;   // which slot in the value store we are editing

    // Every slot the window has touched, so switching displays does not throw
    // the previous display's edits away and Save can write all of them. The
    // controls only ever show one slot; this is where the other ones live.
    struct Slot
    {
        bool loaded = false;
        int action[ZONE_COUNT] = {};
        std::wstring args[ZONE_COUNT];
        ZoneTuning tuning[ZONE_COUNT];
    };
    std::vector<Slot> slots;

    HWND hMonitor = nullptr;
    HWND hZoneLabel[ZONE_COUNT] = {};
    HWND hZoneAction[ZONE_COUNT] = {};
    HWND hZoneArgs[ZONE_COUNT] = {};
    HWND hOpt[IDC_OPT_LAST - IDC_OPT_FIRST] = {};
    HWND hOptLabel[IDC_OPT_LAST - IDC_OPT_FIRST] = {};
    HWND hOptSection[IDC_OPT_LAST - IDC_OPT_FIRST] = {};   // null except at a
                                                           // group heading
    HWND hHdrZone = nullptr, hHdrAction = nullptr, hHdrArgs = nullptr;
    HWND hTzTitle = nullptr, hTzHint = nullptr;
    HWND hTz[IDC_TZ_LAST - IDC_TZ_FIRST] = {};
    HWND hTzLabel[IDC_TZ_LAST - IDC_TZ_FIRST] = {};
    HWND hTip = nullptr;
    HWND hPageZones = nullptr, hPageOptions = nullptr;
    HWND hSave = nullptr, hCancel = nullptr, hReset = nullptr;
    // WM_SETICON does not take ownership; whoever created the icon destroys it.
    HICON hIcon = nullptr;
};

static int Sc(int px, UINT dpi) { return MulDiv(px, (int)dpi, 96); }

// Layout metrics at 96 DPI. The window is sized *from* these rather than the
// other way round — the first version guessed a window size and the button bar
// ended up on top of the last rows.
namespace Lay
{
constexpr int Pad = 16;
constexpr int Gap = 8;
constexpr int RowH = 30;
constexpr int CheckH = 26;
constexpr int SecH = 30;   // group heading plus the air above it
constexpr int TabH = 28;
constexpr int CtlH = 24;
constexpr int BtnH = 32;
constexpr int LblW = 128;
constexpr int CmbW = 210;
constexpr int ArgW = 176;
constexpr int OptLblW = 200;
constexpr int OptCtlW = 190;
constexpr int DiagW = 250;
constexpr int DiagH = 150;
constexpr int HdrH = 22;      // column-header row
constexpr int TzRowH = 26;    // per-zone override row
constexpr int TzPanelH = 14 + 20 + 20 + 6 * TzRowH;

constexpr int LeftBlockW = Pad + LblW + Gap + CmbW + Gap + ArgW;   // 546
constexpr int ClientW = LeftBlockW + Gap + DiagW + Pad;            // 820

// Zones page: tabs, monitor combo, twelve rows.
constexpr int ZonesLeftH = Pad + TabH + Gap + CtlH + Gap + HdrH + ZONE_COUNT * RowH;
constexpr int ZonesRightH = Pad + TabH + Gap + CtlH + Gap + DiagH + TzPanelH;
constexpr int ZonesH = ZonesLeftH > ZonesRightH ? ZonesLeftH : ZonesRightH;
// Options page: ten labelled controls, five checkboxes.
// Ten labelled controls, four checkboxes, four group headings. kOpts is
// declared further down, so a static_assert beside it holds this honest.
constexpr int OptionsH = Pad + TabH + Gap + 10 * RowH + 4 * CheckH + 4 * SecH;

constexpr int ContentH = ZonesH > OptionsH ? ZonesH : OptionsH;
constexpr int ClientH = ContentH + Gap * 2 + BtnH + Pad;
}  // namespace Lay

// Where the little screen preview sits, and where each zone sits inside it.
static RECT DashDiagramRect(UINT dpi)
{
    RECT r;
    r.left = Sc(Lay::LeftBlockW + Lay::Gap, dpi);
    r.top = Sc(Lay::Pad + Lay::TabH + Lay::Gap + Lay::CtlH + Lay::Gap, dpi);
    r.right = r.left + Sc(Lay::DiagW, dpi);
    r.bottom = r.top + Sc(Lay::DiagH, dpi);
    return r;
}

// Proportions inside the preview, mirroring how the real zones are built:
// corners in the four corners, edges along the sides with the corners carved
// out, and a centre block in the middle of each edge.
// Proportions inside the preview, mirroring how the real zones are built.
// The edges are split around the centre blocks rather than drawn through
// them - previously they overlapped, so hovering a centre highlighted the
// whole edge and the centre punched a hole in it.
static RECT ZoneRectInDiagram(Zone z, const RECT &d, bool secondHalf)
{
    int w = d.right - d.left, h = d.bottom - d.top;
    int c = (w < h ? w : h) / 6;         // corner block
    int t = c / 2;                       // edge thickness
    int cw = w / 5, ch = h / 5;          // centre block extent
    int cx0 = d.left + w / 2 - cw / 2, cx1 = cx0 + cw;
    int cy0 = d.top + h / 2 - ch / 2, cy1 = cy0 + ch;

    switch (z)
    {
    case ZONE_TOP_LEFT:      return {d.left, d.top, d.left + c, d.top + c};
    case ZONE_TOP_RIGHT:     return {d.right - c, d.top, d.right, d.top + c};
    case ZONE_BOTTOM_LEFT:   return {d.left, d.bottom - c, d.left + c, d.bottom};
    case ZONE_BOTTOM_RIGHT:  return {d.right - c, d.bottom - c, d.right, d.bottom};

    case ZONE_EDGE_TOP:
        return secondHalf ? RECT{cx1, d.top, d.right - c, d.top + t}
                          : RECT{d.left + c, d.top, cx0, d.top + t};
    case ZONE_EDGE_BOTTOM:
        return secondHalf ? RECT{cx1, d.bottom - t, d.right - c, d.bottom}
                          : RECT{d.left + c, d.bottom - t, cx0, d.bottom};
    case ZONE_EDGE_LEFT:
        return secondHalf ? RECT{d.left, cy1, d.left + t, d.bottom - c}
                          : RECT{d.left, d.top + c, d.left + t, cy0};
    case ZONE_EDGE_RIGHT:
        return secondHalf ? RECT{d.right - t, cy1, d.right, d.bottom - c}
                          : RECT{d.right - t, d.top + c, d.right, cy0};

    case ZONE_CENTER_TOP:    return {cx0, d.top, cx1, d.top + t};
    case ZONE_CENTER_BOTTOM: return {cx0, d.bottom - t, cx1, d.bottom};
    case ZONE_CENTER_LEFT:   return {d.left, cy0, d.left + t, cy1};
    case ZONE_CENTER_RIGHT:  return {d.right - t, cy0, d.right, cy1};
    default:                 return {0, 0, 0, 0};
    }
}

// An edge occupies two rectangles once a centre is carved out of it, so both
// have to be drawn and both have to be hit-tested.
static bool ZoneHasTwoParts(Zone z)
{
    return z == ZONE_EDGE_TOP || z == ZONE_EDGE_BOTTOM || z == ZONE_EDGE_LEFT ||
           z == ZONE_EDGE_RIGHT;
}

// subIdList is nullptr for "keep the default part list", which is what naming
// a theme (DarkMode_CFD) wants. Disabling theming outright is the exception:
// SetWindowTheme only stops theming a control when *both* strings are empty,
// and a still-themed control ignores colour messages such as
// TTM_SETTIPBKCOLOR — which is why the tooltip stayed light.
// Resolved once: theming runs about forty times while the dashboard is built,
// and the LoadLibraryEx fallback used to take a reference every time without
// ever releasing it.
static HMODULE UxTheme()
{
    static HMODULE ux = []() -> HMODULE
    {
        HMODULE m = GetModuleHandleW(L"uxtheme.dll");
        if (!m)
            m = LoadLibraryExW(L"uxtheme.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        return m;
    }();
    return ux;
}

static void ThemeControl(HWND h, const wchar_t *theme,
                         const wchar_t *subIdList = nullptr)
{
    using Fn = HRESULT(WINAPI *)(HWND, LPCWSTR, LPCWSTR);
    static Fn fn =
        UxTheme() ? reinterpret_cast<Fn>(
                        GetProcAddress(UxTheme(), "SetWindowTheme"))
                  : nullptr;
    if (fn)
        fn(h, theme, subIdList);
}

// Dark mode for the standard controls is only reachable through uxtheme
// exports that have no names, just ordinals. This matters more than it looks:
// naming a control's theme "DarkMode_CFD" does *nothing* until the process has
// asked for dark mode here first. Without these two calls the combo boxes,
// edits, buttons and check boxes stay on the light theme and paint their own
// text in near-black — over the dark background this window draws — and no
// amount of WM_CTLCOLOR* can override it, because the theme does that drawing,
// not the parent.
//
// Ordinal 135 is SetPreferredAppMode on 1903 and later, and AllowDarkModeForApp
// on 1809; both take an int and both do the right thing with ForceDark.
// Ordinal 133 is AllowDarkModeForWindow. Missing ordinals just leave the window
// light, which is exactly the old behaviour.
enum DarkAppMode
{
    kAppModeDefault = 0,
    kAppModeAllowDark = 1,
    kAppModeForceDark = 2,
    kAppModeForceLight = 3,
};

static void SetProcessDarkMode(bool dark)
{
    using Fn = int(WINAPI *)(int);
    static Fn fn = UxTheme() ? reinterpret_cast<Fn>(GetProcAddress(
                                   UxTheme(), MAKEINTRESOURCEA(135)))
                             : nullptr;
    if (fn)
        fn(dark ? kAppModeForceDark : kAppModeForceLight);
}

static void AllowDarkModeForControl(HWND h, bool dark)
{
    using Fn = BOOL(WINAPI *)(HWND, BOOL);
    static Fn fn = UxTheme() ? reinterpret_cast<Fn>(GetProcAddress(
                                   UxTheme(), MAKEINTRESOURCEA(133)))
                             : nullptr;
    if (fn)
        fn(h, dark ? TRUE : FALSE);
}

// The theme class a control needs depends on what it is: the "common file
// dialog" classes cover combo boxes and edits, Explorer's cover buttons and
// check boxes. Getting this wrong is silent - the control simply stays light.
static void ApplyControlTheme(HWND h, const wchar_t *cls)
{
    if (!h)
        return;

    AllowDarkModeForControl(h, !g_lightTheme);

    if (_wcsicmp(cls, L"BUTTON") == 0)
        ThemeControl(h, g_lightTheme ? L"Explorer" : L"DarkMode_Explorer");
    else
        ThemeControl(h, g_lightTheme ? L"CFD" : L"DarkMode_CFD");

    SendMessageW(h, WM_THEMECHANGED, 0, 0);
}

// Dark title bar, and Mica where the build supports it. Both are no-ops on
// older Windows, so neither needs a version check.
static void ApplyModernFrame(HWND hWnd)
{
    HMODULE dwm = GetModuleHandleW(L"dwmapi.dll");
    bool loaded = false;
    if (!dwm)
    {
        dwm = LoadLibraryExW(L"dwmapi.dll", nullptr,
                             LOAD_LIBRARY_SEARCH_SYSTEM32);
        loaded = true;
    }
    if (!dwm)
        return;

    using Fn = HRESULT(WINAPI *)(HWND, DWORD, LPCVOID, DWORD);
    auto fn = reinterpret_cast<Fn>(GetProcAddress(dwm, "DwmSetWindowAttribute"));
    if (fn)
    {
        BOOL dark = g_lightTheme ? FALSE : TRUE;
        fn(hWnd, 20, &dark, sizeof(dark));   // DWMWA_USE_IMMERSIVE_DARK_MODE
        int backdrop = 2;                    // DWMSBT_MAINWINDOW (Mica)
        fn(hWnd, 38, &backdrop, sizeof(backdrop));
    }
    if (loaded)
        FreeLibrary(dwm);
}

static const wchar_t *kActionIds[] = {
    L"ACTION_NOTHING",        L"ACTION_SHOW_DESKTOP",
    L"ACTION_TASK_VIEW",      L"ACTION_SWITCH_LAST",
    L"ACTION_TASK_SWITCHER",  L"ACTION_START_MENU",
    L"ACTION_SEARCH",         L"ACTION_SETTINGS",
    L"ACTION_FILE_EXPLORER",  L"ACTION_QUICK_SETTINGS",
    L"ACTION_NOTIFICATION_CENTER", L"ACTION_CLIPBOARD",
    L"ACTION_SCREENSHOT",     L"ACTION_PROJECT",
    L"ACTION_TASK_MANAGER",   L"ACTION_MUTE",
    L"ACTION_MINIMIZE",       L"ACTION_MAXIMIZE",
    L"ACTION_SNAP_LEFT",      L"ACTION_SNAP_RIGHT",
    L"ACTION_CLOSE_WINDOW",   L"ACTION_HIDE_OTHERS",
    L"ACTION_VDESK_NEXT",     L"ACTION_VDESK_PREV",
    L"ACTION_VDESK_NEW",      L"ACTION_VDESK_CLOSE",
    L"ACTION_LOCK",           L"ACTION_LOCK_MONITORS_OFF",
    L"ACTION_MONITORS_OFF",   L"ACTION_SLEEP",
    L"ACTION_SCREENSAVER",    L"ACTION_KEEP_AWAKE_ON",
    L"ACTION_KEEP_AWAKE_OFF", L"ACTION_SEND_KEYPRESS",
    L"ACTION_ALTERNATE_KEYPRESS", L"ACTION_START_PROCESS",
    L"ACTION_ALTERNATE_COMMAND",
};
static constexpr int kActionCount = ARRAYSIZE(kActionIds);

// Per-zone override fields. Blank means "inherit the global value", which is
// why every one of these can be left empty.
struct TzDef
{
    int id;
    const wchar_t *label;
    const wchar_t *tip;
};
static const TzDef kTz[] = {
    {IDC_TZ_SIZE, L"Size (px)",
     L"How big this corner square or edge strip is, in pixels. Leave blank to "
     L"use the global corner/edge size."},
    {IDC_TZ_DELAY, L"Delay (ms)",
     L"How long the cursor must sit in this zone before it fires. 0 is "
     L"immediate. Blank inherits the global activation delay."},
    {IDC_TZ_SETTLE, L"Guard (ms)",
     L"Stops this zone firing when you merely pass through it on the way "
     L"somewhere else. Blank inherits the global pass-through guard."},
    {IDC_TZ_KNOCK, L"Knock (ms)",
     L"Require entering this zone twice within this many milliseconds, like "
     L"knocking. 0 disables it. Blank inherits the global setting."},
    {IDC_TZ_COOLDOWN, L"Cooldown (ms)",
     L"Minimum gap before this zone can fire again. Blank inherits the global "
     L"cooldown."},
    {IDC_TZ_MODIFIER, L"Modifier",
     L"Only fire this zone while the chosen key is held. Inherit uses the "
     L"global setting."},
};
static constexpr int kTzCount = ARRAYSIZE(kTz);

struct OptDef
{
    int id;
    const wchar_t *label;
    bool isCheck;
    const wchar_t *tip;
    // Heading drawn above this row, or nullptr to continue the current group.
    // Fourteen fields in one flat column was a wall; four short groups is the
    // same information you can actually scan.
    const wchar_t *section;
};
static constexpr OptDef kOpts[] = {
    {IDC_CORNER, L"Corner size (px)", false, L"Default size of the four corner squares. Individual corners can override this on the Zones page.", L"How big the zones are"},
    {IDC_EDGE, L"Edge size (px)", false, L"Default thickness of the edge strips. An edge is always clamped to the smaller of the two corners it runs between.", nullptr},
    {IDC_CENTREPCT, L"Centre zone width (%)", false, L"How much of an edge the centre zone takes. Only affects edges where a centre action is assigned.", nullptr},

    {IDC_DELAY, L"Activation delay (ms)", false, L"How long the cursor must dwell before a zone fires. 0 is immediate.", L"When a zone fires"},
    {IDC_SETTLE, L"Pass-through guard (ms)", false, L"Stops a zone firing when you only cross it. You cannot reach a corner without crossing the edge beside it, so without this both would fire.", nullptr},
    {IDC_KNOCK, L"Knock window (ms, 0 = off)", false, L"Require entering a zone twice in quick succession, like knocking. The strongest guard against accidental triggers.", nullptr},
    {IDC_COOLDOWN, L"Cooldown (ms)", false, L"Minimum gap before the same zone can fire again.", nullptr},
    {IDC_MODIFIER, L"Require modifier", false, L"Zones stay inert unless this key is held. Individual zones can override it.", nullptr},

    {IDC_EXCLUDED, L"Excluded processes", false, L"Semicolon-separated executable names. While one of them is in the foreground, no zone fires. Example: photoshop.exe;blender.exe", L"When to stay out of the way"},
    {IDC_CB_FULLSCREEN, L"Skip while an app is fullscreen", true, L"Ignore zones while a game or video is fullscreen - on that display only, so a game on one screen no longer disables the others. Shell surfaces such as Task View and Start do not count.", nullptr},
    {IDC_CB_DRAG, L"Skip while dragging the mouse", true, L"Ignore zones while any mouse button is held, so dragging a window into a corner does not trigger it.", nullptr},
    {IDC_CB_TASKBAR, L"Keep zones off the taskbar", true, L"Build zones from the desktop work area, so they stop at the taskbar instead of fighting its peek-at-desktop strip.", nullptr},

    {IDC_LOCKBLANK, L"Blank delay after lock (ms)", false, L"Only used by the Lock and Turn Off Monitors action. Locking counts as activity, so blanking too early just wakes the display.", L"Everything else"},
    {IDC_CB_MONNAMES, L"List my monitors in the log", true, L"Writes your display names to the log so they can be copied into the monitor selector.", nullptr},
};
static constexpr int kOptCount = ARRAYSIZE(kOpts);

// DashSetInt / DashGetInt reach a control as hOpt[id - IDC_OPT_FIRST], so
// reordering one of these two lists without the other silently writes the wrong
// field. Cheaper to catch here than to notice as "the cooldown box edits the
// corner size".
static constexpr bool OptIdsMatchOrder()
{
    for (int i = 0; i < kOptCount; i++)
        if (kOpts[i].id != IDC_OPT_FIRST + i)
            return false;
    return true;
}
static_assert(kOptCount == IDC_OPT_LAST - IDC_OPT_FIRST,
              "kOpts and the DashId option range have different lengths");
static_assert(OptIdsMatchOrder(),
              "kOpts must list the option ids in DashId order");

// The window is sized from Lay::OptionsH before kOpts is visible, so measure
// the real page here and refuse to build if the two have drifted apart -
// otherwise adding an option just pushes the last row under the button bar.
static constexpr int OptionsPageH()
{
    int h = Lay::Pad + Lay::TabH + Lay::Gap;
    for (const auto &o : kOpts)
    {
        if (o.section)
            h += Lay::SecH;
        h += o.isCheck ? Lay::CheckH : Lay::RowH;
    }
    return h;
}
static_assert(OptionsPageH() == Lay::OptionsH,
              "Lay::OptionsH no longer matches kOpts");

// A free function with CALLBACK, not a lambda: a non-capturing lambda decays
// to a cdecl function pointer, while WNDENUMPROC is __stdcall. They happen to
// be interchangeable in 64-bit builds, but this mod is compiled 32-bit where
// the calling conventions genuinely differ, so the lambda will not convert.
static BOOL CALLBACK DashSetChildFont(HWND hChild, LPARAM lParam)
{
    SendMessageW(hChild, WM_SETFONT, (WPARAM)lParam, TRUE);
    return TRUE;
}

static void DashSetInt(DashState *s, int id, int v)
{
    wchar_t b[32];
    _snwprintf_s(b, _countof(b), _TRUNCATE, L"%d", v);
    SetWindowTextW(s->hOpt[id - IDC_OPT_FIRST], b);
}

static int DashGetInt(DashState *s, int id, int fallback)
{
    wchar_t b[32] = {};
    GetWindowTextW(s->hOpt[id - IDC_OPT_FIRST], b, ARRAYSIZE(b));
    if (!b[0])
        return fallback;
    return _wtoi(b);
}

static void DashLayout(HWND hWnd, DashState *s)
{
    UINT d = s->dpi;
    const int pad = Sc(Lay::Pad, d), gap = Sc(Lay::Gap, d);
    int y;

    // Show only the active page. Doing this first means the button bar below
    // is positioned against a known set of visible controls.
    for (int z = 0; z < ZONE_COUNT; z++)
    {
        int sw = s->showZones ? SW_SHOW : SW_HIDE;
        ShowWindow(s->hZoneLabel[z], sw);
        ShowWindow(s->hZoneAction[z], sw);
        ShowWindow(s->hZoneArgs[z], sw);
    }
    ShowWindow(s->hMonitor, s->showZones ? SW_SHOW : SW_HIDE);
    {
        int sw = s->showZones ? SW_SHOW : SW_HIDE;
        ShowWindow(s->hHdrZone, sw);
        ShowWindow(s->hHdrAction, sw);
        ShowWindow(s->hHdrArgs, sw);
        ShowWindow(s->hTzTitle, sw);
        ShowWindow(s->hTzHint, sw);
        for (int i = 0; i < kTzCount; i++)
        {
            ShowWindow(s->hTz[i], sw);
            ShowWindow(s->hTzLabel[i], sw);
        }
    }
    for (int i = 0; i < kOptCount; i++)
    {
        int sw = s->showZones ? SW_HIDE : SW_SHOW;
        ShowWindow(s->hOpt[i], sw);
        if (s->hOptLabel[i])
            ShowWindow(s->hOptLabel[i], sw);
        if (s->hOptSection[i])
            ShowWindow(s->hOptSection[i], sw);
    }

    SetWindowPos(s->hPageZones, nullptr, pad, pad, Sc(96, d), Sc(Lay::TabH, d),
                 SWP_NOZORDER);
    SetWindowPos(s->hPageOptions, nullptr, pad + Sc(104, d), pad, Sc(96, d),
                 Sc(Lay::TabH, d), SWP_NOZORDER);

    y = pad + Sc(Lay::TabH, d) + gap;

    if (s->showZones)
    {
        SetWindowPos(s->hMonitor, nullptr, pad, y,
                     Sc(Lay::LblW + Lay::Gap + Lay::CmbW, d), Sc(320, d),
                     SWP_NOZORDER);
        y += Sc(Lay::CtlH, d) + gap;

        // Column headers
        SetWindowPos(s->hHdrZone, nullptr, pad, y, Sc(Lay::LblW, d),
                     Sc(18, d), SWP_NOZORDER);
        SetWindowPos(s->hHdrAction, nullptr, pad + Sc(Lay::LblW + Lay::Gap, d),
                     y, Sc(Lay::CmbW, d), Sc(18, d), SWP_NOZORDER);
        SetWindowPos(s->hHdrArgs, nullptr,
                     pad + Sc(Lay::LblW + Lay::Gap + Lay::CmbW + Lay::Gap, d), y,
                     Sc(Lay::ArgW, d), Sc(18, d), SWP_NOZORDER);
        y += Sc(Lay::HdrH, d);

        for (int z = 0; z < ZONE_COUNT; z++)
        {
            int rowY = y + z * Sc(Lay::RowH, d);
            SetWindowPos(s->hZoneLabel[z], nullptr, pad, rowY + Sc(5, d),
                         Sc(Lay::LblW, d), Sc(20, d), SWP_NOZORDER);
            SetWindowPos(s->hZoneAction[z], nullptr,
                         pad + Sc(Lay::LblW + Lay::Gap, d), rowY,
                         Sc(Lay::CmbW, d), Sc(320, d), SWP_NOZORDER);
            SetWindowPos(s->hZoneArgs[z], nullptr,
                         pad + Sc(Lay::LblW + Lay::Gap + Lay::CmbW + Lay::Gap, d),
                         rowY, Sc(Lay::ArgW, d), Sc(Lay::CtlH, d), SWP_NOZORDER);
        }
    }
    else
    {
        for (int i = 0; i < kOptCount; i++)
        {
            if (s->hOptSection[i])
            {
                SetWindowPos(s->hOptSection[i], nullptr, pad, y + Sc(8, d),
                             Sc(Lay::OptLblW + Lay::Gap + Lay::OptCtlW, d),
                             Sc(20, d), SWP_NOZORDER);
                y += Sc(Lay::SecH, d);
            }
            if (kOpts[i].isCheck)
            {
                SetWindowPos(s->hOpt[i], nullptr, pad, y,
                             Sc(Lay::OptLblW + Lay::Gap + Lay::OptCtlW, d),
                             Sc(22, d), SWP_NOZORDER);
                y += Sc(Lay::CheckH, d);
            }
            else
            {
                SetWindowPos(s->hOptLabel[i], nullptr, pad, y + Sc(5, d),
                             Sc(Lay::OptLblW, d), Sc(20, d), SWP_NOZORDER);
                bool combo = (kOpts[i].id == IDC_MODIFIER);
                bool wide = (kOpts[i].id == IDC_EXCLUDED);
                SetWindowPos(s->hOpt[i], nullptr,
                             pad + Sc(Lay::OptLblW + Lay::Gap, d), y,
                             Sc(wide ? Lay::OptCtlW + Lay::DiagW : Lay::OptCtlW, d),
                             combo ? Sc(200, d) : Sc(Lay::CtlH, d), SWP_NOZORDER);
                y += Sc(Lay::RowH, d);
            }
        }
    }

    if (s->showZones)
    {
        RECT dg = DashDiagramRect(d);
        int px = dg.left;
        int py = dg.bottom + Sc(14, d);
        int pw = dg.right - dg.left;
        SetWindowPos(s->hTzTitle, nullptr, px, py, pw, Sc(18, d), SWP_NOZORDER);
        py += Sc(20, d);
        SetWindowPos(s->hTzHint, nullptr, px, py, pw, Sc(16, d), SWP_NOZORDER);
        py += Sc(20, d);
        int lw = Sc(96, d);
        for (int i = 0; i < kTzCount; i++)
        {
            SetWindowPos(s->hTzLabel[i], nullptr, px, py + Sc(4, d), lw,
                         Sc(18, d), SWP_NOZORDER);
            bool combo = (kTz[i].id == IDC_TZ_MODIFIER);
            SetWindowPos(s->hTz[i], nullptr, px + lw + Sc(6, d), py,
                         pw - lw - Sc(6, d), combo ? Sc(180, d) : Sc(22, d),
                         SWP_NOZORDER);
            py += Sc(Lay::TzRowH, d);
        }
    }

    // Button bar sits below the taller of the two pages, always, so it can
    // never land on top of a control.
    RECT rc;
    GetClientRect(hWnd, &rc);
    int by = rc.bottom - pad - Sc(Lay::BtnH, d);
    int bx = pad;
    SetWindowPos(s->hSave, nullptr, bx, by, Sc(130, d), Sc(Lay::BtnH, d),
                 SWP_NOZORDER);
    bx += Sc(130 + Lay::Gap, d);
    SetWindowPos(s->hCancel, nullptr, bx, by, Sc(90, d), Sc(Lay::BtnH, d),
                 SWP_NOZORDER);
    bx += Sc(90 + Lay::Gap, d);
    SetWindowPos(s->hReset, nullptr, bx, by, Sc(210, d), Sc(Lay::BtnH, d),
                 SWP_NOZORDER);

    InvalidateRect(hWnd, nullptr, TRUE);
}

// Screen preview: a rectangle with the twelve zones drawn where they actually
// sit, filled when something is assigned to them. Clicking one jumps to its
// row, which is far quicker than reading down a list of twelve labels.
static void DashPaintDiagram(HWND hWnd, DashState *s, HDC hdc)
{
    if (!s->showZones)
        return;

    UINT d = s->dpi;
    RECT dg = DashDiagramRect(d);

    HFONT old = (HFONT)SelectObject(hdc, s->hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_pal.dim);
    RECT cap = {dg.left, dg.top - Sc(20, d), dg.right, dg.top - Sc(2, d)};
    DrawTextW(hdc, L"Your screen — click a zone to jump to it", -1, &cap,
              DT_LEFT | DT_SINGLELINE);

    HBRUSH screenBrush = CreateSolidBrush(RGB(24, 24, 24));
    FillRect(hdc, &dg, screenBrush);
    DeleteObject(screenBrush);

    HPEN pen = CreatePen(PS_SOLID, Sc(1, d), RGB(90, 90, 90));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH hollow = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hollow);
    Rectangle(hdc, dg.left, dg.top, dg.right, dg.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    HBRUSH set = CreateSolidBrush(RGB(76, 194, 255));
    HBRUSH unset = CreateSolidBrush(RGB(58, 58, 58));
    HBRUSH sel = CreateSolidBrush(RGB(255, 190, 80));

    for (int z = 0; z < ZONE_COUNT; z++)
    {
        int idx = (int)SendMessageW(s->hZoneAction[z], CB_GETCURSEL, 0, 0);
        HBRUSH b = (z == s->hoverZone) ? sel : (idx > 0 ? set : unset);
        int parts = ZoneHasTwoParts((Zone)z) ? 2 : 1;
        for (int p = 0; p < parts; p++)
        {
            RECT r = ZoneRectInDiagram((Zone)z, dg, p == 1);
            if (r.right > r.left && r.bottom > r.top)
                FillRect(hdc, &r, b);
        }
    }

    DeleteObject(set);
    DeleteObject(unset);
    DeleteObject(sel);

    // ponytail: no hover card. It used to be drawn from dg.bottom+10 to
    // dg.bottom+112 — the exact strip the per-zone panel's controls occupy, so
    // it was never readable and its leftovers showed through the gaps between
    // the fields. The panel below already shows the same numbers for the
    // selected zone, and clicking a zone in the preview selects it.

    SelectObject(hdc, old);
}
// Fills the zone controls from whichever configuration slot is selected.
// Pushes the selected zone's overrides into the six panel fields.
static void DashShowZoneTuning(DashState *s)
{
    const ZoneTuning &tn = s->tuning[s->selZone];
    auto put = [&](int id, int v)
    {
        wchar_t b[24] = L"";
        if (v >= 0)
            _snwprintf_s(b, _countof(b), _TRUNCATE, L"%d", v);
        SetWindowTextW(s->hTz[id - IDC_TZ_FIRST], b);
    };
    put(IDC_TZ_SIZE, tn.size);
    put(IDC_TZ_DELAY, tn.delay);
    put(IDC_TZ_SETTLE, tn.settle);
    put(IDC_TZ_KNOCK, tn.knock);
    put(IDC_TZ_COOLDOWN, tn.cooldown);
    SendMessageW(s->hTz[IDC_TZ_MODIFIER - IDC_TZ_FIRST], CB_SETCURSEL,
                 tn.modifier < 0 ? 0 : tn.modifier + 1, 0);

    std::wstring title =
        std::wstring(L"Settings for ") + ZoneToString((Zone)s->selZone);
    SetWindowTextW(s->hTzTitle, title.c_str());
}

// Reads the six panel fields back into the selected zone. Blank stays -1,
// which is what "inherit the global value" is stored as.
static void DashCaptureZoneTuning(DashState *s)
{
    ZoneTuning &tn = s->tuning[s->selZone];
    auto get = [&](int id) -> int
    {
        wchar_t b[24] = {};
        GetWindowTextW(s->hTz[id - IDC_TZ_FIRST], b, ARRAYSIZE(b));
        if (!b[0])
            return -1;
        return _wtoi(b);
    };
    tn.size = get(IDC_TZ_SIZE);
    tn.delay = get(IDC_TZ_DELAY);
    tn.settle = get(IDC_TZ_SETTLE);
    tn.knock = get(IDC_TZ_KNOCK);
    tn.cooldown = get(IDC_TZ_COOLDOWN);
    int m = (int)SendMessageW(s->hTz[IDC_TZ_MODIFIER - IDC_TZ_FIRST],
                              CB_GETCURSEL, 0, 0);
    tn.modifier = (m <= 0) ? -1 : m - 1;
}

// The monitor name a combo entry stands for. Entry 0 is the wildcard.
static std::wstring DashSlotMonitorId(DashState *s, int index)
{
    if (index <= 0)
        return L"*";
    wchar_t buf[256] = {};
    if (SendMessageW(s->hMonitor, CB_GETLBTEXT, index, (LPARAM)buf) == CB_ERR)
        return L"";
    return buf;
}

// Reads persistence into a slot, once. Everything after that comes from the
// in-memory copy, so an edit is never re-read over.
static void DashFillSlotFromStore(DashState *s, int index)
{
    if (index < 0 || index >= (int)s->slots.size() || s->slots[index].loaded)
        return;

    DashState::Slot &sl = s->slots[index];
    sl.loaded = true;

    // The value store is the only place a zone can come from now. The old
    // second source - seeding from the Windhawk settings page - went with the
    // page itself, and with it the bug where the seed was picked by combo
    // position instead of by monitor, so one display's configuration showed up
    // under "All monitors" and then fired on every display.
    for (int z = 0; z < ZONE_COUNT; z++)
    {
        std::wstring act = GetStrValue(GuiKey(index, z, L"a").c_str());

        sl.action[z] = 0;
        for (int a = 0; a < kActionCount; a++)
        {
            if (act == kActionIds[a])
            {
                sl.action[z] = a;
                break;
            }
        }
        sl.args[z] = GetStrValue(GuiKey(index, z, L"g").c_str());

        ZoneTuning &tn = sl.tuning[z];
        tn.size = Wh_GetIntValue(GuiKey(index, z, L"sz").c_str(), -1);
        tn.delay = Wh_GetIntValue(GuiKey(index, z, L"dl").c_str(), -1);
        tn.settle = Wh_GetIntValue(GuiKey(index, z, L"gd").c_str(), -1);
        tn.knock = Wh_GetIntValue(GuiKey(index, z, L"kn").c_str(), -1);
        tn.cooldown = Wh_GetIntValue(GuiKey(index, z, L"cd").c_str(), -1);
        tn.modifier = Wh_GetIntValue(GuiKey(index, z, L"md").c_str(), -1);
    }
}

// Controls -> the slot they were showing.
static void DashCaptureSlot(DashState *s)
{
    if (s->cfgIndex < 0 || s->cfgIndex >= (int)s->slots.size())
        return;

    DashCaptureZoneTuning(s);
    DashState::Slot &sl = s->slots[s->cfgIndex];
    for (int z = 0; z < ZONE_COUNT; z++)
    {
        sl.action[z] =
            (int)SendMessageW(s->hZoneAction[z], CB_GETCURSEL, 0, 0);
        if (sl.action[z] < 0)
            sl.action[z] = 0;
        wchar_t buf[512] = {};
        GetWindowTextW(s->hZoneArgs[z], buf, ARRAYSIZE(buf));
        sl.args[z] = buf;
        sl.tuning[z] = s->tuning[z];
    }
    sl.loaded = true;
}

// The slot -> the controls.
static void DashShowSlot(DashState *s)
{
    if (s->cfgIndex < 0 || s->cfgIndex >= (int)s->slots.size())
        return;

    const DashState::Slot &sl = s->slots[s->cfgIndex];
    for (int z = 0; z < ZONE_COUNT; z++)
    {
        SendMessageW(s->hZoneAction[z], CB_SETCURSEL, sl.action[z], 0);
        SetWindowTextW(s->hZoneArgs[z], sl.args[z].c_str());
        s->tuning[z] = sl.tuning[z];
    }
    DashShowZoneTuning(s);
}

// Switches the window to whichever display the combo now shows, keeping what
// was on screen for the previous one.
static void DashLoadZones(DashState *s)
{
    int sel = (int)SendMessageW(s->hMonitor, CB_GETCURSEL, 0, 0);
    if (sel < 0)
        sel = 0;

    if (sel != s->cfgIndex)
        DashCaptureSlot(s);

    s->cfgIndex = sel;
    if (sel >= (int)s->slots.size())
        s->slots.resize(sel + 1);

    DashFillSlotFromStore(s, sel);
    DashShowSlot(s);
}

static void DashLoad(HWND hWnd, DashState *s)
{
    // Monitor selector: one slot per detected display, plus a wildcard.
    SendMessageW(s->hMonitor, CB_RESETCONTENT, 0, 0);
    SendMessageW(s->hMonitor, CB_ADDSTRING, 0, (LPARAM)L"All monitors  ( * )");
    // Take a reference to the snapshot under the lock, then read it outside:
    // it is immutable and shared_ptr keeps it alive even if the detection
    // thread publishes a new one mid-loop. Reading g_monitors directly here
    // was a use-after-free — RefreshMonitors clears that vector and frees
    // every id string while this thread walks it.
    EnterCriticalSection(&g_zonesLock);
    std::shared_ptr<const ZoneSet> snap = g_zones;
    LeaveCriticalSection(&g_zonesLock);
    std::vector<std::wstring> names;
    if (snap)
        names = snap->monitorNames;
    for (const auto &n : names)
        SendMessageW(s->hMonitor, CB_ADDSTRING, 0, (LPARAM)n.c_str());
    SendMessageW(s->hMonitor, CB_SETCURSEL, 0, 0);

    // One slot per combo entry: the wildcard plus every detected display.
    s->slots.assign(names.size() + 1, DashState::Slot{});
    s->cfgIndex = 0;

    EnterCriticalSection(&g_settingsLock);
    DashSetInt(s, IDC_CORNER, g_settings.cornerSize);
    DashSetInt(s, IDC_EDGE, g_settings.edgeSize);
    DashSetInt(s, IDC_DELAY, g_settings.activationDelay);
    DashSetInt(s, IDC_SETTLE, g_settings.settleMs);
    DashSetInt(s, IDC_KNOCK, g_settings.knockWindowMs);
    DashSetInt(s, IDC_COOLDOWN, g_settings.cooldownMs);
    DashSetInt(s, IDC_CENTREPCT, g_settings.centerZonePercent);
    DashSetInt(s, IDC_LOCKBLANK, g_lockBlankDelayMs);
    SendMessageW(s->hOpt[IDC_MODIFIER - IDC_OPT_FIRST], CB_SETCURSEL,
                 g_settings.requireModifier, 0);
    std::wstring excl;
    for (size_t i = 0; i < g_settings.excludedProcesses.size(); i++)
    {
        if (i)
            excl += L";";
        excl += g_settings.excludedProcesses[i];
    }
    SetWindowTextW(s->hOpt[IDC_EXCLUDED - IDC_OPT_FIRST], excl.c_str());
    CheckDlgButton(hWnd, IDC_CB_FULLSCREEN,
                   g_settings.disableOnFullscreen ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hWnd, IDC_CB_DRAG,
                   g_settings.disableDuringDrag ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hWnd, IDC_CB_TASKBAR,
                   g_settings.avoidTaskbar ? BST_CHECKED : BST_UNCHECKED);
    LeaveCriticalSection(&g_settingsLock);
    CheckDlgButton(hWnd, IDC_CB_MONNAMES,
                   g_showMonitorNames ? BST_CHECKED : BST_UNCHECKED);

    DashLoadZones(s);
}

static void DashSave(HWND hWnd, DashState *s)
{
    // Fold what is on screen back into its slot, then write every slot the
    // window has touched. Writing only the visible one meant a two-monitor
    // setup could not be configured in a single session: the other display's
    // edits were still in memory and never reached the store.
    DashCaptureSlot(s);

    for (int sel = 0; sel < (int)s->slots.size(); sel++)
    {
        const DashState::Slot &sl = s->slots[sel];
        if (!sl.loaded)
            continue;

        // A machine with more displays than the value store has slots would
        // write keys that ReadDashboardZones' kMaxGuiConfigs loop never reads
        // back — the edit would vanish on reload with nothing said.
        if (sel >= kMaxGuiConfigs)
        {
            Wh_Log(L"Dashboard: display %d is beyond the %d configuration "
                   L"slots; not saved",
                   sel, kMaxGuiConfigs);
            continue;
        }

        std::wstring monName = DashSlotMonitorId(s, sel);
        Wh_SetStringValue(GuiKey(sel, -1, L"id").c_str(), monName.c_str());
        for (int z = 0; z < ZONE_COUNT; z++)
        {
            int idx = sl.action[z];
            if (idx < 0 || idx >= kActionCount)
                idx = 0;
            Wh_SetStringValue(GuiKey(sel, z, L"a").c_str(), kActionIds[idx]);
            Wh_SetStringValue(GuiKey(sel, z, L"g").c_str(), sl.args[z].c_str());

            const ZoneTuning &tn = sl.tuning[z];
            Wh_SetIntValue(GuiKey(sel, z, L"sz").c_str(), tn.size);
            Wh_SetIntValue(GuiKey(sel, z, L"dl").c_str(), tn.delay);
            Wh_SetIntValue(GuiKey(sel, z, L"gd").c_str(), tn.settle);
            Wh_SetIntValue(GuiKey(sel, z, L"kn").c_str(), tn.knock);
            Wh_SetIntValue(GuiKey(sel, z, L"cd").c_str(), tn.cooldown);
            Wh_SetIntValue(GuiKey(sel, z, L"md").c_str(), tn.modifier);
        }
    }
    Wh_SetIntValue(L"gui_active", 1);

    // Global options.
    Wh_SetIntValue(L"ovr_corner", DashGetInt(s, IDC_CORNER, 6));
    Wh_SetIntValue(L"ovr_edge", DashGetInt(s, IDC_EDGE, 6));
    Wh_SetIntValue(L"ovr_delay", DashGetInt(s, IDC_DELAY, 0));
    Wh_SetIntValue(L"ovr_settle", DashGetInt(s, IDC_SETTLE, 80));
    Wh_SetIntValue(L"ovr_knock", DashGetInt(s, IDC_KNOCK, 0));
    Wh_SetIntValue(L"ovr_cooldown", DashGetInt(s, IDC_COOLDOWN, 300));
    Wh_SetIntValue(L"ovr_centre", DashGetInt(s, IDC_CENTREPCT, 20));
    Wh_SetIntValue(L"ovr_lockblank", DashGetInt(s, IDC_LOCKBLANK, 1200));
    Wh_SetIntValue(L"ovr_modifier",
                   (int)SendMessageW(s->hOpt[IDC_MODIFIER - IDC_OPT_FIRST],
                                     CB_GETCURSEL, 0, 0));
    wchar_t excl[512] = {};
    GetWindowTextW(s->hOpt[IDC_EXCLUDED - IDC_OPT_FIRST], excl,
                   ARRAYSIZE(excl));
    Wh_SetStringValue(L"ovr_excluded", excl);
    Wh_SetIntValue(kOvrFullscreen, IsDlgButtonChecked(hWnd, IDC_CB_FULLSCREEN));
    Wh_SetIntValue(kOvrDrag, IsDlgButtonChecked(hWnd, IDC_CB_DRAG));
    Wh_SetIntValue(L"ovr_taskbar", IsDlgButtonChecked(hWnd, IDC_CB_TASKBAR));
    Wh_SetIntValue(L"ovr_monnames", IsDlgButtonChecked(hWnd, IDC_CB_MONNAMES));

    ReloadConfig();
    RequestRebuild();
    UpdateTrayIcon(false);
    Wh_Log(L"Dashboard: settings saved and applied");
}

static LRESULT CALLBACK DashWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam)
{
    DashState *s = (DashState *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

    switch (uMsg)
    {
    case WM_CREATE:
    {
        auto *cs = (CREATESTRUCTW *)lParam;
        s = (DashState *)cs->lpCreateParams;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)s);

        s->dpi = 96;
        {
            HMODULE u = GetModuleHandleW(L"user32.dll");
            using Fn = UINT(WINAPI *)(HWND);
            if (auto fn = (Fn)GetProcAddress(u, "GetDpiForWindow"))
                s->dpi = fn(hWnd);
            if (!s->dpi)
                s->dpi = 96;
        }

        LOGFONTW lf = {};
        lf.lfHeight = -MulDiv(9, (int)s->dpi, 72);
        lf.lfWeight = FW_NORMAL;
        wcscpy_s(lf.lfFaceName, L"Segoe UI");
        s->hFont = CreateFontIndirectW(&lf);
        BuildPalette();
        s->hBg = CreateSolidBrush(g_pal.bg);
        s->hField = CreateSolidBrush(g_pal.field);
        s->hPanel = CreateSolidBrush(g_pal.panel);

        // Every control is themed here rather than at its call site: the six
        // scattered calls this replaces covered the combo boxes and edits and
        // missed every button and check box, which is why those kept painting
        // black text on the dark background.
        auto mk = [&](const wchar_t *cls, const wchar_t *txt, DWORD style,
                      int id) -> HWND
        {
            HWND h = CreateWindowExW(0, cls, txt, WS_CHILD | style, 0, 0, 10,
                                     10, hWnd, (HMENU)(INT_PTR)id,
                                     cs->hInstance, nullptr);
            if (h)
            {
                SendMessageW(h, WM_SETFONT, (WPARAM)s->hFont, TRUE);
                ApplyControlTheme(h, cls);
            }
            return h;
        };

        s->hPageZones = mk(L"BUTTON", L"Zones", BS_PUSHBUTTON | WS_VISIBLE |
                                                    WS_TABSTOP, IDC_PAGE_ZONES);
        s->hPageOptions = mk(L"BUTTON", L"Options",
                             BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP,
                             IDC_PAGE_OPTIONS);
        s->hMonitor = mk(L"COMBOBOX", nullptr,
                         CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                         IDC_MONITOR);

        for (int z = 0; z < ZONE_COUNT; z++)
        {
            s->hZoneLabel[z] = mk(L"STATIC", ZoneToString((Zone)z), SS_LEFT, 0);
            s->hZoneAction[z] =
                mk(L"COMBOBOX", nullptr,
                   CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP,
                   IDC_ZONE_ACTION + z);
            for (int a = 0; a < kActionCount; a++)
            {
                SendMessageW(s->hZoneAction[z], CB_ADDSTRING, 0,
                             (LPARAM)ActionToString(ParseActionType(kActionIds[a])));
            }
            s->hZoneArgs[z] = mk(L"EDIT", nullptr,
                                 WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                                 IDC_ZONE_ARGS + z);
        }

        for (int i = 0; i < kOptCount; i++)
        {
            s->hOptSection[i] =
                kOpts[i].section
                    ? mk(L"STATIC", kOpts[i].section, SS_LEFT, IDC_OPT_SECTION)
                    : nullptr;

            if (kOpts[i].isCheck)
            {
                s->hOpt[i] = mk(L"BUTTON", kOpts[i].label,
                                BS_AUTOCHECKBOX | WS_TABSTOP, kOpts[i].id);
                s->hOptLabel[i] = nullptr;
            }
            else
            {
                s->hOptLabel[i] = mk(L"STATIC", kOpts[i].label, SS_LEFT, 0);
                if (kOpts[i].id == IDC_MODIFIER)
                {
                    s->hOpt[i] = mk(L"COMBOBOX", nullptr,
                                    CBS_DROPDOWNLIST | WS_TABSTOP,
                                    kOpts[i].id);
                    const wchar_t *mods[] = {L"None", L"Ctrl", L"Alt", L"Shift",
                                             L"Win"};
                    for (auto m : mods)
                        SendMessageW(s->hOpt[i], CB_ADDSTRING, 0, (LPARAM)m);
                }
                else
                {
                    s->hOpt[i] = mk(L"EDIT", nullptr,
                                    WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
                                    kOpts[i].id);
                }
            }
        }

        // Column headers - the argument field previously had no label at all.
        s->hHdrZone = mk(L"STATIC", L"Zone", SS_LEFT, IDC_HDR_ZONE);
        s->hHdrAction = mk(L"STATIC", L"Action", SS_LEFT, IDC_HDR_ACTION);
        s->hHdrArgs = mk(L"STATIC", L"Argument / command", SS_LEFT, IDC_HDR_ARGS);

        s->hTzTitle = mk(L"STATIC", L"Settings for this zone", SS_LEFT,
                         IDC_TZ_TITLE);
        s->hTzHint = mk(L"STATIC", L"Leave blank to use the global value.",
                        SS_LEFT, IDC_TZ_HINT);
        for (int i = 0; i < kTzCount; i++)
        {
            s->hTzLabel[i] = mk(L"STATIC", kTz[i].label, SS_LEFT, 0);
            if (kTz[i].id == IDC_TZ_MODIFIER)
            {
                s->hTz[i] = mk(L"COMBOBOX", nullptr,
                               CBS_DROPDOWNLIST | WS_TABSTOP, kTz[i].id);
                const wchar_t *mv[] = {L"Inherit", L"None", L"Ctrl",
                                       L"Alt",     L"Shift", L"Win"};
                for (auto v : mv)
                    SendMessageW(s->hTz[i], CB_ADDSTRING, 0, (LPARAM)v);
            }
            else
            {
                s->hTz[i] = mk(L"EDIT", nullptr,
                               WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER |
                                   WS_TABSTOP,
                               kTz[i].id);
            }
        }

        s->hSave = mk(L"BUTTON", L"Save and Apply",
                      BS_DEFPUSHBUTTON | WS_VISIBLE | WS_TABSTOP, IDC_SAVE);
        s->hCancel = mk(L"BUTTON", L"Close",
                        BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP, IDC_CANCEL);
        s->hReset = mk(L"BUTTON", L"Reset everything to defaults",
                       BS_PUSHBUTTON | WS_VISIBLE | WS_TABSTOP, IDC_RESET);

        // One tooltip control serving every field. Descriptions live next to
        // the field definitions so a new setting cannot be added without one.
        s->hTip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                  WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, 0,
                                  0, 0, hWnd, nullptr, cs->hInstance, nullptr);
        if (s->hTip)
        {
            SendMessageW(s->hTip, TTM_SETMAXTIPWIDTH, 0, Sc(320, s->dpi));
            SendMessageW(s->hTip, WM_SETFONT, (WPARAM)s->hFont, TRUE);
            // A themed tooltip ignores TTM_SETTIPBKCOLOR/TEXTCOLOR outright, so
            // it kept the system tooltip colours while the rest of the window
            // followed the palette. Both strings must be empty or theming is
            // not actually switched off.
            ThemeControl(s->hTip, L"", L"");
            SendMessageW(s->hTip, TTM_SETTIPBKCOLOR, (WPARAM)g_pal.panel, 0);
            SendMessageW(s->hTip, TTM_SETTIPTEXTCOLOR, (WPARAM)g_pal.text, 0);
            auto tip = [&](HWND ctl, const wchar_t *text)
            {
                if (!ctl || !text)
                    return;
                TTTOOLINFOW ti = {};
                ti.cbSize = sizeof(ti);
                ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
                ti.hwnd = hWnd;
                ti.uId = (UINT_PTR)ctl;
                ti.lpszText = const_cast<LPWSTR>(text);
                SendMessageW(s->hTip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
            };
            for (int i = 0; i < kOptCount; i++)
            {
                tip(s->hOpt[i], kOpts[i].tip);
                tip(s->hOptLabel[i], kOpts[i].tip);
            }
            for (int i = 0; i < kTzCount; i++)
            {
                tip(s->hTz[i], kTz[i].tip);
                tip(s->hTzLabel[i], kTz[i].tip);
            }
            tip(s->hMonitor,
                L"Which display these zones belong to. Use * to apply one "
                L"configuration to every monitor.");
            tip(s->hHdrArgs,
                L"Extra input for the chosen action: a key combination for "
                L"Virtual Key Press, a path or URL for Custom Command, or two "
                L"of either separated by | for the Alternate actions.");
            tip(s->hReset,
                L"Discard every zone and option and start again from the "
                L"defaults. Asks first.");
        }

        s->hIcon = MakeTrayIcon(true);
        if (HICON ic = s->hIcon)
        {
            SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)ic);
            SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)ic);
        }
        ApplyModernFrame(hWnd);
        DashLoad(hWnd, s);
        DashLayout(hWnd, s);
        return 0;
    }

    case WM_ERASEBKGND:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect((HDC)wParam, &rc,
                 s && s->hBg ? s->hBg : (HBRUSH)GetStockObject(BLACK_BRUSH));
        return 1;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (s)
            DashPaintDiagram(hWnd, s, hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    {
        if (!s || !s->showZones)
            break;
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        RECT dg = DashDiagramRect(s->dpi);
        int hit = -1;
        // Centres are tested first: they sit inside the span an edge would
        // otherwise claim, and the first match wins.
        static const Zone order[ZONE_COUNT] = {
            ZONE_TOP_LEFT,      ZONE_TOP_RIGHT,     ZONE_BOTTOM_LEFT,
            ZONE_BOTTOM_RIGHT,  ZONE_CENTER_TOP,    ZONE_CENTER_BOTTOM,
            ZONE_CENTER_LEFT,   ZONE_CENTER_RIGHT,  ZONE_EDGE_TOP,
            ZONE_EDGE_BOTTOM,   ZONE_EDGE_LEFT,     ZONE_EDGE_RIGHT};
        for (int oi = 0; oi < ZONE_COUNT && hit < 0; oi++)
        {
            Zone z = order[oi];
            int parts = ZoneHasTwoParts(z) ? 2 : 1;
            for (int p = 0; p < parts; p++)
            {
                RECT r = ZoneRectInDiagram(z, dg, p == 1);
                if (PtInRect(&r, pt))
                {
                    hit = (int)z;
                    break;
                }
            }
        }
        if (uMsg == WM_MOUSEMOVE)
        {
            if (hit != s->hoverZone)
            {
                s->hoverZone = hit;
                // Only the preview changes, so only the preview is repainted.
                InvalidateRect(hWnd, &dg, TRUE);
            }
        }
        else if (hit >= 0)
        {
            DashCaptureZoneTuning(s);
            s->selZone = hit;
            DashShowZoneTuning(s);
            SetFocus(s->hZoneAction[hit]);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;
    }

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORBTN:
        if (s)
        {
            // A group heading introduces the fields under it rather than
            // labelling one of them, and the per-zone hint is an aside. Both
            // read better set apart from the body text.
            int cid = GetDlgCtrlID((HWND)lParam);
            SetTextColor((HDC)wParam, cid == IDC_OPT_SECTION ? g_pal.accent
                                      : cid == IDC_TZ_HINT   ? g_pal.dim
                                                             : g_pal.text);
            SetBkColor((HDC)wParam, g_pal.bg);
            return (LRESULT)s->hBg;
        }
        break;

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        if (s)
        {
            SetTextColor((HDC)wParam, g_pal.fieldText);
            SetBkColor((HDC)wParam, g_pal.field);
            return (LRESULT)s->hField;
        }
        break;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        if (id == IDC_PAGE_ZONES || id == IDC_PAGE_OPTIONS)
        {
            s->showZones = (id == IDC_PAGE_ZONES);
            DashLayout(hWnd, s);
            return 0;
        }
        if (id == IDC_MONITOR && HIWORD(wParam) == CBN_SELCHANGE)
        {
            DashLoadZones(s);
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;
        }
        // (int) because IDC_ZONE_ACTION is a DashId and ZONE_COUNT is a Zone:
        // C++20 deprecates arithmetic between two different enumeration types,
        // and the mod repository's CI fails the build on any warning.
        if (id >= IDC_ZONE_ACTION && id < IDC_ZONE_ACTION + (int)ZONE_COUNT)
        {
            if (HIWORD(wParam) == CBN_SELCHANGE ||
                HIWORD(wParam) == CBN_SETFOCUS)
            {
                if (HIWORD(wParam) == CBN_SETFOCUS)
                {
                    DashCaptureZoneTuning(s);
                    s->selZone = id - IDC_ZONE_ACTION;
                    DashShowZoneTuning(s);
                }
                InvalidateRect(hWnd, nullptr, TRUE);
                return 0;
            }
        }
        if (id == IDC_SAVE)
        {
            DashSave(hWnd, s);
            return 0;
        }
        if (id == IDC_CANCEL)
        {
            DestroyWindow(hWnd);
            return 0;
        }
        if (id == IDC_RESET)
        {
            if (MessageBoxW(hWnd,
                            L"Discard every zone and option and start again "
                            L"from the defaults?",
                            L"Win-X Hot Corners", MB_YESNO | MB_ICONQUESTION) ==
                IDYES)
            {
                ClearStoredConfig();
                ReloadConfig();
                RequestRebuild();
                DashLoad(hWnd, s);
            }
            return 0;
        }
        break;
    }

    case WM_DPICHANGED:
    {
        s->dpi = HIWORD(wParam);
        if (s->hFont)
            DeleteObject(s->hFont);
        LOGFONTW lf = {};
        lf.lfHeight = -MulDiv(9, (int)s->dpi, 72);
        wcscpy_s(lf.lfFaceName, L"Segoe UI");
        s->hFont = CreateFontIndirectW(&lf);
        EnumChildWindows(hWnd, DashSetChildFont, (LPARAM)s->hFont);
        RECT *r = (RECT *)lParam;
        SetWindowPos(hWnd, nullptr, r->left, r->top, r->right - r->left,
                     r->bottom - r->top, SWP_NOZORDER | SWP_NOACTIVATE);
        DashLayout(hWnd, s);
        return 0;
    }

    case WM_SIZE:
        if (s)
            DashLayout(hWnd, s);
        return 0;

    case WM_DESTROY:
        if (s)
        {
            if (s->hFont)
                DeleteObject(s->hFont);
            if (s->hBg)
                DeleteObject(s->hBg);
            if (s->hField)
                DeleteObject(s->hField);
            if (s->hPanel)
                DeleteObject(s->hPanel);
            if (s->hIcon)
                DestroyIcon(s->hIcon);
        }
        g_hDashWnd = nullptr;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI DashThread(LPVOID)
{
    PinThreadDpiPerMonitorV2();

    // TOOLTIPS_CLASS lives in comctl32 and needs the library initialised.
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_WIN95_CLASSES};
    InitCommonControlsEx(&icc);

    const wchar_t *kClass = L"WindhawkHotCornersDash";
    HINSTANCE hInst = GetModuleHandle(nullptr);

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = DashWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kClass;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    DashState state;

    UINT dpi = 96;
    {
        HMODULE u = GetModuleHandleW(L"user32.dll");
        using Fn = UINT(WINAPI *)(void);
        if (auto fn = (Fn)GetProcAddress(u, "GetDpiForSystem"))
            dpi = fn();
        if (!dpi)
            dpi = 96;
    }

    // Dark mode must be asked for before the window and its controls exist:
    // the theme classes applied to each control later have no effect until the
    // process has opted in, and opting in afterwards does not repaint what has
    // already been created.
    BuildPalette();
    SetProcessDarkMode(!g_lightTheme);

    // WS_CLIPCHILDREN so the parent can never paint inside a child's rectangle.
    // Without it, anything drawn in WM_PAINT that lands under a control stays
    // on screen as residue, because the child is not repainted to cover it.
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX |
                  WS_CLIPCHILDREN;
    RECT need = {0, 0, Sc(Lay::ClientW, dpi), Sc(Lay::ClientH, dpi)};
    AdjustWindowRectEx(&need, style, FALSE, 0);
    int w = need.right - need.left;
    int h = need.bottom - need.top;
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    HWND hWnd = CreateWindowExW(0, kClass, L"Win-X Hot Corners — Settings",
                                style, x, y, w, h, nullptr, nullptr, hInst,
                                &state);

    if (!hWnd)
    {
        UnregisterClassW(kClass, hInst);
        return 1;
    }

    AllowDarkModeForControl(hWnd, !g_lightTheme);

    g_hDashWnd = hWnd;
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        if (!IsDialogMessageW(hWnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnregisterClassW(kClass, hInst);
    return 0;
}

static void OpenDashboard()
{
    if (g_hDashWnd && IsWindow(g_hDashWnd))
    {
        // Already open — bring it forward rather than making a second one.
        ShowWindow(g_hDashWnd, SW_RESTORE);
        SetForegroundWindow(g_hDashWnd);
        return;
    }
    if (g_hDashThread)
    {
        // Recycle the handle only once the previous thread has actually gone.
        // Closing it while that thread still runs throws away the only way to
        // wait for it during uninit, and starts a second dashboard besides.
        if (WaitForSingleObject(g_hDashThread, 0) != WAIT_OBJECT_0)
            return;
        CloseHandle(g_hDashThread);
        g_hDashThread = nullptr;
    }
    g_hDashThread = CreateThread(nullptr, 0, DashThread, nullptr, 0, nullptr);
}

static LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam)
{
    if (uMsg == WM_APP_TRAY)
    {
        // Under NOTIFYICON_VERSION_4 the event is in the low word of lParam
        // and the cursor position is in wParam; under the legacy version
        // lParam is the event on its own. Reading LOWORD(lParam) is correct
        // for both.
        UINT ev = LOWORD(lParam);
        if (ev == WM_CONTEXTMENU || ev == WM_RBUTTONUP)
        {
            POINT pt = {GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam)};
            if (pt.x == 0 && pt.y == 0)
                GetCursorPos(&pt);  // legacy version does not supply coords
            ShowTrayMenu(pt);
        }
        else if (ev == NIN_SELECT || ev == NIN_KEYSELECT || ev == WM_LBUTTONUP)
        {
            OpenDashboard();
        }
        return 0;
    }
    if (uMsg == WM_COMMAND)
    {
        HandleTrayCommand(LOWORD(wParam));
        return 0;
    }
    // Explorer restarted and threw away every tray icon; put ours back.
    if (g_taskbarCreatedMsg && uMsg == g_taskbarCreatedMsg)
    {
        UpdateTrayIcon(true);
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI TrayThread(LPVOID)
{
    const wchar_t *kClass = L"WindhawkHotCornersTray";
    HINSTANCE hInst = GetModuleHandle(nullptr);

    // Registered before the window exists so an Explorer restart racing our
    // startup cannot be missed.
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASS wc = {};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = kClass;
    RegisterClass(&wc);

    g_hTrayWnd = CreateWindowEx(WS_EX_TOOLWINDOW, kClass, nullptr, WS_POPUP, 0,
                                0, 0, 0, nullptr, nullptr, hInst, nullptr);
    if (!g_hTrayWnd)
    {
        Wh_Log(L"Tray: failed to create the icon's window");
        UnregisterClass(kClass, hInst);
        return 1;
    }

    UpdateTrayIcon(true);


    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    NOTIFYICONDATAW nid = {};
    FillTrayIconData(nid);
    Shell_NotifyIconW(NIM_DELETE, &nid);

    DestroyWindow(g_hTrayWnd);
    g_hTrayWnd = nullptr;
    UnregisterClass(kClass, hInst);
    return 0;
}

// =====================================================================
// Tool Mod Entry Points
// =====================================================================

BOOL WhTool_ModInit()
{
    Wh_Log(L"Win-X Hot Corners v" WH_MOD_VERSION);
    Wh_Log(L"This mod has no Settings page. Right-click its tray icon (next to "
           L"the clock) and choose \"Zones & settings...\".");

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

    ReloadConfig();

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

    // Non-fatal: losing the tray icon should not take the hot corners with it.
    g_hTrayThread =
        CreateThread(nullptr, 0, TrayThread, nullptr, 0, &g_dwTrayThreadId);
    if (!g_hTrayThread)
        Wh_Log(L"Tray icon unavailable (thread creation failed)");

    return TRUE;
}

void WhTool_ModSettingsChanged()
{
    ReloadConfig();
    UpdateTrayIcon(false);
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

    if (g_dwTrayThreadId)
        PostThreadMessage(g_dwTrayThreadId, WM_QUIT, 0, 0);

    if (g_hTrayThread)
    {
        if (WaitForSingleObject(g_hTrayThread, 3000) == WAIT_TIMEOUT)
            Wh_Log(L"Tray thread exit timed out");
        CloseHandle(g_hTrayThread);
        g_hTrayThread = nullptr;
    }

    if (g_dwDetectThreadId)
        PostThreadMessage(g_dwDetectThreadId, WM_QUIT, 0, 0);

    bool allStopped = true;

    // The dashboard is a window with its own message loop on its own thread,
    // and it takes g_settingsLock and g_zonesLock. Nothing below may free
    // those while it is alive. Its loop only ends when its window does, so
    // close the window rather than signalling the stop event.
    if (g_hDashWnd)
        PostMessage(g_hDashWnd, WM_CLOSE, 0, 0);

    if (g_hDashThread)
    {
        if (WaitForSingleObject(g_hDashThread, 3000) == WAIT_TIMEOUT)
        {
            Wh_Log(L"Dashboard thread exit timed out");
            allStopped = false;
        }
        CloseHandle(g_hDashThread);
        g_hDashThread = nullptr;
    }

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

// ==WindhawkMod==
// @id              win-x-hotcorners
// @name            Win-X Hot Corners
// @description     macOS-style hot corners & edges for Windows with full multi-monitor support — trigger actions instantly when your cursor hits any screen corner or edge
// @version         4.4.2
// @author          lost_husky
// @github          https://github.com/DhakadG
// @donateUrl       https://ko-fi.com/losthusky_
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lgdi32 -lole32 -lpowrprof -lshell32 -luser32
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

- **Bounded latency** — a dedicated detection thread asks to sample the cursor
  every 16 ms (one system timer tick, so 16-31 ms in practice) and only idles
  when nothing could fire anyway. Nothing can starve it:
  not a busy explorer, not an elevated foreground app, not a slow action.
- **Zero impact on the rest of the system** — no global mouse hook, so your
  games and apps keep their input path to themselves.
- **Monitors identified by name** — zones bind to a display's friendly name
  (e.g. `Dell U2720Q`), so rearranging your desktop or changing which display
  is primary never reshuffles your configuration. (Two displays of the same
  model are the one exception — see *Identifying your monitors*.)
- **Per-monitor DPI correct** — detection runs per-monitor-DPI-aware, so
  zones land in the right place on mixed-scaling setups.
- **Screen edges, in three parts** — each edge is split into a start, a middle
  and an end, configured separately. Give neighbouring parts the same action and
  they merge back into one, so you get whichever of `ABC`, `AAB`, `ABB` or `AAA`
  you want without a mode switch. See *Dividing an edge*.
- **Knock to trigger** — set a knock window on any zone, corners included, and a
  single arrival does nothing: you have to leave and come back within the window.
  Handy on a corner you brush past often.
- **Configurable zone size**, optional dwell delay, and a cooldown timer.
- **Fullscreen protection** — auto-detects games, presentations, and D3D
  fullscreen apps.
- **Drag protection** — zones don't fire while a mouse button is held.
- **App exclusions** — blacklist processes that need corner/edge clicks.

## Configuring

**Settings page** — zones, timings and everything else. Add a display, then add
the zones you want on it. A zone you do not list does nothing.

**Tray icon**, next to the clock:

- **Left-click** — turn the hot corners on or off. The icon dims when they are off.
- **Right-click** — suspend for a while, or reset the enable state.
- **Right-click → Zones & settings...** — the dashboard: a tab per display,
  a picture of that screen with each zone showing what it does, and the timings
  actually in effect for whichever zone you point at.

### Upgrading from 4.1.x

Earlier versions had no settings page and stored the layout themselves. Your
zones keep running untouched, so nothing breaks on upgrade — everything else on
the settings page is live immediately. Re-enter the zones there when you are
ready; the moment one display is listed, the old layout is ignored.

The startup log names any globals you had customised in the old version, since
those now come from the settings page and need setting again there.

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

## Dividing an edge

Every edge runs between the two corners it touches and is divided into three
segments — for the top and bottom edge those are **left, centre, right**; for
the left and right edge, **top, middle, bottom**. The centre segment's width is
the *Edge centre width* setting, as a percentage of the edge.

You do not choose a "mode". Configure the three segments and the mod works out
the shape:

| You set | You get |
| --- | --- |
| All three the same | One zone spanning the whole edge |
| Left + centre same, right different | Two zones |
| All three different | Three zones |
| Only the centre | One zone in the middle of the edge |
| Left and right same, centre unset | Two zones with a dead gap between them |

Neighbouring segments merge only when they are *identical* — same action, same
arguments, same overrides. Two segments running the same action with different
cooldowns stay separate, because a merged zone could only carry one cooldown.

Merging is not cosmetic. Three separate zones all running Task View would each
re-arm as the pointer crossed a seam, so sliding along the edge would fire it
repeatedly; one merged zone fires once.

## Knock to trigger

Set **Knock window** on a zone — globally, or per zone — and a single arrival
never fires it. The zone arms only when you leave and come back within that many
milliseconds. `0` turns it off.

This works on every zone, corners included, so a corner you keep brushing past
on the way to something else can be made deliberate. 400 ms is a comfortable
double-knock; below about 200 ms it starts to need intent.

## Identifying your monitors

The dashboard has one tab per display, labelled with its name, so normally there
is nothing to identify — a display that is not plugged in keeps its tab, marked
with a dot. The names also go to this mod's log every time it loads or your
display layout changes, which is the place to copy an exact name from when you
are filling in the settings page:

```
+-- Your monitors ---------------------------------------
|  Copy a name into the "Display" field on this mod's settings page.
|  Use  *  there to apply one configuration to every monitor.
|
|   1. "Dell U2720Q"   [primary]   3840 x 2160  at (0, 0)
|   2. "BOE0998"                   1920 x 1080  at (3840, 0)
+--------------------------------------------------------
```

Copy the text inside the quotes. If you own two identical displays they get a
` #2`, ` #3` suffix so each stays separately configurable. Those suffixes are
handed out in listed order — primary first, then left to right — so unlike the
names themselves they are not fixed: making the other twin primary swaps which
one is ` #2`, and swaps the configuration with it. Check the log after such a
change.

Putting `*` in the **Display** field applies one configuration to every screen.

Resolution is **per zone**: a name-matched entry wins over `*` for the zones
it defines, and `*` supplies the rest. So you can put a shared config on `*`
and override just one corner on one display. To exclude a screen from a shared
zone rather than replace it, set that zone to **Disabled here** — leaving it
unset means "not configured", which falls through to `*`.

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

## What's New in v4.4.2

- **The tray icon stops saying "paused" when a suspension ends.** The corners
  came back on their own, and the menu was right, but the icon stayed dimmed
  for the rest of the session — the one piece of feedback you actually look at.
- **New action: Disabled here.** Setting a zone to this on a named display
  excludes that screen from a shared `*` zone. Leaving a zone unset still means
  "not configured" and still falls through to `*`; there was previously no way
  to say the other thing.
- **The dashboard no longer marks a zone broken when the wildcard rescues it.**
  A display-specific zone with unparseable arguments is skipped in favour of
  the `*` entry, and the preview now follows that same order instead of
  reporting a zone as dead when it fires perfectly well.
- **Clearing the last zone on the settings page no longer resurrects a pre-4.2
  layout.** The handover is one-way once the settings page has owned the
  configuration.
- Display changes and taskbar moves queue a rebuild rather than doing it inside
  the broadcast handler, so the sender is not blocked and a burst of work-area
  messages collapses into one rebuild instead of resetting every cooldown
  several times.
- The dashboard's tab strip no longer runs off the window edge with several
  displays; labels ellipsise and every tab stays clickable.
- Readme: removed a paragraph that still argued against having a settings page,
  and corrected the monitor-list sample and the references to a "selector" the
  dashboard no longer has.

## What's New in v4.4.1

Two places where a value you changed still had no effect, and two where the
window could tell you something untrue.

- **Every setting on the page is live immediately after upgrading.** The globals
  used to be applied only once you had listed a display there, so anyone coming
  from 4.1.x had a page full of fields that did nothing — change *Cooldown* from
  300 to 800, see nothing happen. The page owns them all now; only the zone list
  still falls back to a stored layout. The startup log names any globals you had
  customised in the old version so you know what to set again.
- **The tray's "skip while fullscreen" and "skip while dragging" toggles are
  gone.** They wrote a key that only the upgrade path read, so toggling one
  worked until the next reload put it back. They are settings, and a mod cannot
  write its own settings, so the tray copy could only ever drift. They live on
  the settings page alone. Enable and suspend stay — those have no page
  equivalent.
- **Left-click on the tray icon toggles the hot corners**, which is what the
  readme always said it did; it had been opening the dashboard. The icon dims
  when they are off, and the dashboard is still the menu's first item.
- **A zone whose arguments cannot be read is marked, not drawn as working.** A
  key combination that does not parse, or an Alternate action missing its `|`,
  used to appear on the preview like any other configured zone while never
  firing.
- **The tray menu anchors to the icon even when Windows refuses the GUID
  registration**, which is the one case the anchoring was added for.
- **Show Desktop uses the documented `IShellDispatch4::ToggleDesktop`** instead
  of an undocumented private message to a window the mod does not own. The old
  route stays as a fallback.
- **Moving or auto-hiding a taskbar on a secondary display rebuilds the zones.**
  The polled check only ever saw the primary display's work area, so with
  *Keep zones out of the taskbar* on, that display's zones stayed put.
- Dropped the comctl32 dependency — nothing used it after the 4.3.0 rewrite.
- Clearing one display's name on the settings page no longer drops every
  display listed after it.
- Clicking the tray icon immediately after closing the dashboard reopens it
  rather than doing nothing.

## What's New in v4.4.0

- **Every edge is now three separate zones** — start, middle and end. The two
  outer stretches used to share one configuration, so `ABC` was impossible.
- **Neighbouring segments with the same action merge back into one.** That is
  what makes `AAA`, `AAB`, `ABB` and `ABC` all expressible without a mode
  switch, and it is not just tidiness: three adjacent zones sharing an action
  would each re-arm as the pointer crossed a seam, firing repeatedly as you
  slid along the edge. Segments merge only when they are identical — same
  action, arguments and overrides.
- **Your existing layout still works.** A stored edge fills both outer
  segments, and the middle too when no centre was configured, which reproduces
  the old geometry exactly — the three identical segments coalesce straight
  back into the single edge-wide zone you had.
- **Knock works on corners.** It always did — it is a per-zone setting — but
  nothing said so. There is a section about it in the readme now.
- **The tray menu opens at the tray icon** instead of wherever the pointer
  happened to be, and follows the taskbar to whichever edge it is docked on.
- **New icon**, and the dashboard window has one again — it lost its icon in
  4.3.0 when the window was rewritten.

## What's New in v4.3.0

The dashboard is now a picture of your configuration rather than a form.

- **A tab per display**, labelled with its name and carrying a count of how many
  zones you have set up on it, so "did I configure anything on that screen?" is
  answerable at a glance. A display that is not plugged in keeps its tab, marked
  with a dot — its configuration is still real.
- **The screen is drawn at its real aspect ratio**, so a portrait or ultrawide
  display looks like one.
- **Each zone shows what it does.** Side strips read vertically, which is the
  only way a name like "Notification Centre" fits inside one.
- **Point at a zone and the panel below shows every timing in effect for it** —
  size, delay, pass-through guard, knock, cooldown and modifier — with each one
  marked *global* or *this zone*. A zone inherits five numbers it never used to
  show, so "why did this one fire late?" was unanswerable without reading the
  configuration by hand. Click to pin a zone, click again to release it.
- **Zones taken from the "All displays" configuration say so**, instead of
  looking like they belong to the display you are viewing.
- The editing controls are gone. Everything is configured on the settings page,
  and the window follows it live — change a setting in Windhawk and the picture
  updates without reopening.

## What's New in v4.2.1

**The settings page is back.** It went away in 4.1.0 because forty settings in
one flat list was unusable. Windhawk v2 renders them as collapsible groups, so
that reason is gone.

- **Zones, timings and everything else are configured in Settings.** Add a
  display, then add only the zones you want on it — no more scrolling past
  twelve entries to reach the one you use.
- **Nothing breaks on upgrade.** A layout saved by 4.1.x keeps running exactly
  as it did, globals included, and the log says so at startup. It is ignored
  only once you list a display on the settings page.
- **The dashboard no longer saves.** It was a second surface writing the same
  store, which is what caused the configuration-swapping bug in 4.1.4. It now
  tells you to edit in Settings rather than reporting a save that did nothing.

## What's New in v4.1.4

Review fixes, the first of which could lose a display's configuration.

- **The editor bound each configuration to a list position instead of to a
  display.** Detection has always matched zones by display name, but the
  dashboard read and wrote whichever stored group sat at the same index as the
  entry in its dropdown. That list is ordered primary first, then left to right,
  so making another display primary or unplugging one renumbered it — and the
  editor would then show one display's zones under another's name, and overwrite
  them there on save. Each entry is now matched to its stored group by name, a
  display configured for the first time takes the lowest free group, and a
  display that is not currently plugged in keeps the group it owns.
- **Custom Command could silently fail for URLs, shortcuts and folders.** Those
  go through shell extensions that need COM, and the worker thread running them
  had no apartment. It initialises one now, and both `ShellExecuteEx` calls ask
  the shell not to return before it is finished, since that thread has no
  message loop.
- **A cooldown consumed the visit.** Walking into a zone shortly after anything
  else fired and then parking there never fired at all — you had to leave and
  come back. A cooldown is now a wait rather than a refusal, so staying put
  outlasts it.
- **Lock and blank the display no longer blocks unload** for up to ten seconds
  while it waits out the blanking delay.
- Argument fields stop input at the length the mod can actually store, rather
  than truncating past it in silence.
- The readme no longer claims a 16 ms sample rate without qualification: the
  wait expires on a system timer tick, so it is 16-31 ms in practice.
- Dropped an unused header, a brush that was created and destroyed but never
  used, and a monitor-matching branch that could never be reached.

## What's New in v4.1.3

- **Fixed a crash on unload.** Every thread is given three seconds to stop
  before the mod frees the locks they share, and a thread that misses that
  deadline makes the mod leak them instead — freeing something a live thread is
  still using is undefined behaviour inside Windhawk's own process. The tray
  thread was the one exception: its timeout was logged and then ignored, so a
  tray thread that had not finished could have its locks deleted underneath it.
  It now counts like the others. Waits also treat a failed wait as "still
  running" rather than as success.
- **The tray's on/off toggle is serialised with the rest.** It wrote its key
  outside the reload lock, so a reload running at that moment could put the old
  value back and leave the switch disagreeing with what was stored.

## What's New in v4.1.2

Two more review fixes, both about threads.

- **Two reloads could finish out of order.** Reloading builds the whole
  configuration off to the side and swaps it in at the end — which is what keeps
  the settings lock held briefly, but meant two reloads started close together
  could publish in the wrong order, leaving the live configuration stale against
  what was actually saved. Saving from the dashboard, resetting, and the tray's
  own toggles now hold one lock across both the write and the reload that reads
  it back, so a reload always sees a settled store.
- **A failed cursor read slowed detection to the idle rate.** `GetCursorPos`
  failing is transient, and the zones are still armed while it does — so the
  right answer is to try again on the next 16 ms tick, not to wait 100 ms. Only
  the states where nothing can fire at all use the idle rate, which is what the
  documentation already claimed.

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

## Support

Windhawk renders the `@donateUrl` in the metadata block as a Donate button in the mod
listing, but it only takes one link, so the rest live here.

- [Ko-fi](https://ko-fi.com/losthusky_)
- [Buy Me a Coffee](https://www.buymeacoffee.com/losthusky_)
- [Playto](https://www.playto.so/losthusky_)

<!-- Plain links rather than badge images on purpose: this repository only allows
     images from i.imgur.com and raw.githubusercontent.com, so a shields.io badge
     would not render. To add a platform, add a bullet. -->
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displays:
  - - monitor: "*"
      $name: Display
      $description: >-
        The display's friendly name, exactly as this mod prints it in the log,
        or  *  to apply this configuration to every display. Open the log, or
        the dashboard's tab strip, to see the names.
    - zones:
      - - zone: TOP_LEFT
          $name: Zone
          $options:
          - TOP_LEFT: Top-left corner
          - TOP_RIGHT: Top-right corner
          - BOTTOM_LEFT: Bottom-left corner
          - BOTTOM_RIGHT: Bottom-right corner
          - TOP_START: "Top edge: left"
          - TOP_MIDDLE: "Top edge: centre"
          - TOP_END: "Top edge: right"
          - BOTTOM_START: "Bottom edge: left"
          - BOTTOM_MIDDLE: "Bottom edge: centre"
          - BOTTOM_END: "Bottom edge: right"
          - LEFT_START: "Left edge: top"
          - LEFT_MIDDLE: "Left edge: middle"
          - LEFT_END: "Left edge: bottom"
          - RIGHT_START: "Right edge: top"
          - RIGHT_MIDDLE: "Right edge: middle"
          - RIGHT_END: "Right edge: bottom"
        - action: ACTION_NOTHING
          $name: Action
          $options:
          - ACTION_NOTHING: Nothing
          - ACTION_DISABLED_HERE: "Disabled here (ignore the * entry)"
          - ACTION_SHOW_DESKTOP: Show desktop
          - ACTION_TASK_VIEW: Task View
          - ACTION_SCREENSAVER: Start screensaver
          - ACTION_MONITORS_OFF: Turn monitors off
          - ACTION_QUICK_SETTINGS: Quick Settings
          - ACTION_NOTIFICATION_CENTER: Notification Centre
          - ACTION_START_MENU: Start menu
          - ACTION_HIDE_OTHERS: Hide other windows
          - ACTION_MUTE: Mute
          - ACTION_TASK_MANAGER: Task Manager
          - ACTION_LOCK: Lock
          - ACTION_SLEEP: Sleep
          - ACTION_SWITCH_LAST: Switch to last window
          - ACTION_TASK_SWITCHER: Task switcher
          - ACTION_MINIMIZE: Minimise window
          - ACTION_MAXIMIZE: Maximise window
          - ACTION_SNAP_LEFT: Snap left
          - ACTION_SNAP_RIGHT: Snap right
          - ACTION_CLOSE_WINDOW: Close window
          - ACTION_FILE_EXPLORER: File Explorer
          - ACTION_SETTINGS: Settings
          - ACTION_SEARCH: Search
          - ACTION_CLIPBOARD: Clipboard history
          - ACTION_SCREENSHOT: Screenshot
          - ACTION_PROJECT: Project display
          - ACTION_VDESK_NEXT: Next virtual desktop
          - ACTION_VDESK_PREV: Previous virtual desktop
          - ACTION_VDESK_NEW: New virtual desktop
          - ACTION_VDESK_CLOSE: Close virtual desktop
          - ACTION_LOCK_MONITORS_OFF: Lock and turn monitors off
          - ACTION_KEEP_AWAKE_ON: Keep awake on
          - ACTION_KEEP_AWAKE_OFF: Keep awake off
          - ACTION_SEND_KEYPRESS: Send key press
          - ACTION_ALTERNATE_KEYPRESS: Alternate key press
          - ACTION_START_PROCESS: Custom command
          - ACTION_ALTERNATE_COMMAND: Alternate command
        - args: ""
          $name: Arguments
          $description: >-
            Only used by some actions. Send key press takes a combination such
            as  Ctrl+Shift+Esc . Custom command takes an executable, path or
            URL. The two Alternate actions take both halves separated by a
            vertical bar, for example  Alt+S | Alt+H .
        - size: -1
          $name: Size override (px)
          $description: >-
            Corner square, or edge strip thickness. -1 keeps the global value.
        - delay: -1
          $name: Activation delay override (ms)
          $description: -1 keeps the global value.
        - settle: -1
          $name: Pass-through guard override (ms)
          $description: -1 keeps the global value.
        - knock: -1
          $name: Knock window override (ms)
          $description: -1 keeps the global value. 0 disables knock mode.
        - cooldown: -1
          $name: Cooldown override (ms)
          $description: -1 keeps the global value.
        - modifier: INHERIT
          $name: Modifier override
          $options:
          - INHERIT: Keep the global value
          - NONE: None
          - CTRL: Ctrl
          - ALT: Alt
          - SHIFT: Shift
          - WIN: Win
      $name: Zones
      $description: >-
        One entry per zone you want active on this display. A zone you do not
        list does nothing. Listing the same zone twice uses the first entry.

        Each edge is three segments. Give them three different actions for
        three separate zones, or give neighbouring segments the same action
        and they merge into one - so all three alike is a single edge-wide
        zone, and left+centre alike with a different right is two.

        "Merge" means identical: same action, same arguments and the same
        overrides. Two segments running the same action with different
        cooldowns stay separate.
  $name: Displays
  $description: >-
    Leave this empty to keep using a layout saved by an older version of the
    mod. As soon as you add a display here, this list takes over completely.

- cornerSize: 6
  $name: Corner size (px)
  $description: How far into the corner the pointer has to reach.
- edgeSize: 6
  $name: Edge thickness (px)
- centerPercent: 20
  $name: Edge centre width (%)
  $description: How much of each edge the centre zone occupies.

- delay: 0
  $name: Activation delay (ms)
  $description: How long the pointer must rest in a zone before it fires.
- settle: 80
  $name: Pass-through guard (ms)
  $description: >-
    Ignores a zone the pointer is merely travelling through. Raise it if
    actions fire while you are moving to something else.
- knock: 0
  $name: Knock window (ms)
  $description: >-
    0 disables knock mode. Above 0, the zone has to be entered twice within
    this window before it fires.
- cooldown: 300
  $name: Cooldown (ms)
  $description: Minimum gap between two firings of the same zone.
- requireModifier: NONE
  $name: Required modifier
  $description: A key that must be held for any zone to fire.
  $options:
  - NONE: None
  - CTRL: Ctrl
  - ALT: Alt
  - SHIFT: Shift
  - WIN: Win

- disableOnFullscreen: true
  $name: Skip while a window is fullscreen
  $description: Only suppresses the display the fullscreen window is on.
- disableDuringDrag: true
  $name: Skip while dragging
- avoidTaskbar: false
  $name: Keep zones out of the taskbar
  $description: >-
    Shrinks each display's zones to its working area, so a corner sitting
    behind the taskbar cannot be reached.
- excluded: ""
  $name: Excluded applications
  $description: >-
    Semicolon-separated executable names, for example  game.exe; vlc.exe .
    Zones do nothing while one of these is in the foreground.

- lockBlankDelay: 1200
  $name: Lock-then-blank delay (ms)
  $description: >-
    How long "Lock and turn monitors off" waits after locking before blanking.
- showMonitorNames: true
  $name: List display names in the log
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <windowsx.h>   // GET_X_LPARAM / GET_Y_LPARAM
#include <initializer_list>
#include <powrprof.h>
#include <shellapi.h>
#include <shldisp.h>   // IShellDispatch4 / CLSID_Shell, for Show Desktop
#include <windhawk_api.h>

#include <algorithm>
#include <atomic>
// swprintf_s and memcmp are used directly; they only reached this file
// through <windows.h> before.
#include <cstdio>
#include <cstring>
#include <cwchar>
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
    // Explicitly off on this display, as opposed to "not configured". Only the
    // second falls through to the "*" entry, so this is what lets one screen
    // opt out of a shared zone without abandoning the wildcard everywhere.
    DisabledHere,
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

// Zone IDs: 0-3 corners, then each edge as three independently configurable
// segments running between the two corners it touches.
//
// Before 4.4 an edge was one zone plus an optional centre, so the two outer
// stretches always did the same thing. Splitting them into start/middle/end
// makes every pattern expressible - ABC, AAB, ABB - and setting all three the
// same gives one edge-wide zone, because BuildZoneSet coalesces neighbouring
// segments that resolve identically. That coalescing matters: three separate
// zones carrying one action would each re-arm as the pointer crossed a seam.
enum Zone
{
    ZONE_TOP_LEFT = 0,
    ZONE_TOP_RIGHT = 1,
    ZONE_BOTTOM_LEFT = 2,
    ZONE_BOTTOM_RIGHT = 3,

    ZONE_TOP_START = 4,      // left-hand stretch of the top edge
    ZONE_TOP_MIDDLE = 5,
    ZONE_TOP_END = 6,        // right-hand stretch

    ZONE_BOTTOM_START = 7,
    ZONE_BOTTOM_MIDDLE = 8,
    ZONE_BOTTOM_END = 9,

    ZONE_LEFT_START = 10,    // upper stretch of the left edge
    ZONE_LEFT_MIDDLE = 11,
    ZONE_LEFT_END = 12,      // lower stretch

    ZONE_RIGHT_START = 13,
    ZONE_RIGHT_MIDDLE = 14,
    ZONE_RIGHT_END = 15,

    ZONE_COUNT = 16,
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
    std::wstring monitorId;   // friendly name, or "*" for all displays
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
    // An HMONITOR, not a rect. It is only stale between a display
    // change and the rebuild that follows it, and a stale one simply fails to
    // match, which errs towards firing rather than towards silence.
    HMONITOR monitor = nullptr;
};

// The detection loop reads nothing but this snapshot, so it never touches
// g_settings and never takes a lock on the timing-critical path.
struct ZoneSet
{
    std::vector<HitZone> zones;
    // The displays this set was built from. They ride in the snapshot so the
    // dashboard thread can list monitors without reading g_monitors, which the
    // detection thread clears and refills underneath it. The size comes along
    // because the dashboard draws each screen at its real aspect ratio.
    struct MonitorSummary
    {
        std::wstring id;
        int width = 0;
        int height = 0;
        bool primary = false;
    };
    std::vector<MonitorSummary> monitors;
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

// Serialises a whole write-then-reload, so two of them cannot interleave.
// ReloadConfig builds its result outside g_settingsLock on purpose - that is
// what keeps the hold short - but it means two reloads racing could publish out
// of order and leave the older one winning, with the runtime configuration
// stale against what is actually stored. The dashboard's Save, the dashboard's
// Reset and the tray's reset take this around their value-store writes *and*
// the reload that follows, so a reload always sees a settled store. Recursive
// by nature, which is why those callers can hold it across ReloadConfig's own
// acquisition.
//
// Order is always g_reloadLock then g_settingsLock; nothing takes them the
// other way round.
static CRITICAL_SECTION g_reloadLock;

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

// Prints the monitor list at load and on display changes, so a name can be
// copied into the settings page instead of guessed. Cheap - once per event.
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
    // 3 attempts. A layout that changes three times inside one
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

    // Printed so the name can be copied straight into the settings page rather
    // than guessed. Once per load and per display change only.
    Wh_Log(L" ");
    Wh_Log(L"+-- Your monitors ---------------------------------------");
    Wh_Log(L"|  Copy a name into the \"Display\" field on this mod's settings "
           L"page.");
    Wh_Log(L"|  Use  *  there to apply one configuration to every monitor.");
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
        cachedName = ProcessNameOfWindow(hFg);
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
    // IShellDispatch::ToggleDesktop is the documented way to do this. The old
    // route - WM_COMMAND 407 to Shell_TrayWnd - is an undocumented private
    // message sent to a window this mod does not own, and the value is only
    // whatever that class happens to use today. The worker thread has a COM
    // apartment, so this is available where it is called from.
    // ToggleDesktop is on IShellDispatch4, not on the base interface.
    IShellDispatch4 *shell = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_Shell, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&shell));
    if (SUCCEEDED(hr) && shell)
    {
        hr = shell->ToggleDesktop();
        shell->Release();
        if (SUCCEEDED(hr))
            return;
    }

    // Kept as a fallback rather than deleted: ToggleDesktop needs an apartment,
    // and the private message is what worked before it.
    Wh_Log(L"Show Desktop: IShellDispatch unavailable (0x%08X), using the "
           L"shell message instead",
           hr);

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
    // NOASYNC because the worker thread has no message loop: without it the
    // call can return before the shell is finished, and the thread is torn
    // down at uninit.
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
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
    //
    // Waiting on the stop event rather than sleeping: this can be up to ten
    // seconds, and an unload during it would otherwise sit out the whole delay
    // and then push WhTool_ModUninit into its three-second timeout path.
    if (WaitForSingleObject(g_hStopEvent, (DWORD)g_lockBlankDelayMs.load()) ==
        WAIT_OBJECT_0)
        return;
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
    // NOASYNC because the worker thread has no message loop: without it the
    // call can return before the shell is finished, and the thread is torn
    // down at uninit.
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
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
        {L"ACTION_DISABLED_HERE", CornerAction::DisabledHere},
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
    case CornerAction::DisabledHere: return L"Disabled here";
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
    case ZONE_TOP_START: return L"Top edge, left";
    case ZONE_TOP_MIDDLE: return L"Top edge, centre";
    case ZONE_TOP_END: return L"Top edge, right";
    case ZONE_BOTTOM_START: return L"Bottom edge, left";
    case ZONE_BOTTOM_MIDDLE: return L"Bottom edge, centre";
    case ZONE_BOTTOM_END: return L"Bottom edge, right";
    case ZONE_LEFT_START: return L"Left edge, top";
    case ZONE_LEFT_MIDDLE: return L"Left edge, middle";
    case ZONE_LEFT_END: return L"Left edge, bottom";
    case ZONE_RIGHT_START: return L"Right edge, top";
    case ZONE_RIGHT_MIDDLE: return L"Right edge, middle";
    case ZONE_RIGHT_END: return L"Right edge, bottom";
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
    case CornerAction::DisabledHere: return nullptr;
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
// Two segments of an edge are interchangeable only if everything about them
// matches - the same action with a different cooldown still has to stay two
// zones, because the cooldown is what the merged zone would have to carry.
static bool SameZoneConfig(const ZoneConfig *a, const ZoneConfig *b)
{
    if (a == b)
        return true;
    if (!a || !b)
        return false;
    if (a->action != b->action || a->args != b->args)
        return false;
    const ZoneTuning &x = a->tuning, &y = b->tuning;
    return x.size == y.size && x.delay == y.delay && x.settle == y.settle &&
           x.knock == y.knock && x.cooldown == y.cooldown &&
           x.modifier == y.modifier;
}

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
        // An explicit opt-out stops here rather than falling through, which is
        // the whole difference between it and leaving the zone unset.
        if (zc.action == CornerAction::DisabledHere)
            return nullptr;
        if (zc.action != CornerAction::Nothing && zc.executor)
            return &zc;
    }

    // 2. Wildcard
    for (const auto &cfg : g_settings.monitorConfigs)
    {
        if (cfg.monitorId != L"*")
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
    {
        ZoneSet::MonitorSummary ms;
        ms.id = mon.id;
        ms.width = mon.rcMonitor.right - mon.rcMonitor.left;
        ms.height = mon.rcMonitor.bottom - mon.rcMonitor.top;
        ms.primary = mon.isPrimary;
        set->monitors.push_back(std::move(ms));
    }

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
        // An edge is one strip however many segments it carries, so its
        // thickness comes from the first segment that asks for one.
        auto edgeThickness3 = [&](Zone a, Zone b, Zone c, int lim1,
                                  int lim2) -> int
        {
            for (Zone z : {a, b, c})
            {
                const ZoneConfig *zc = zoneCfg(z);
                if (zc && zc->tuning.size > 0)
                    return edgeThickness(z, lim1, lim2);
            }
            return edgeThickness(a, lim1, lim2);
        };
        int esTop = edgeThickness3(ZONE_TOP_START, ZONE_TOP_MIDDLE,
                                   ZONE_TOP_END, csTL, csTR);
        int esBot = edgeThickness3(ZONE_BOTTOM_START, ZONE_BOTTOM_MIDDLE,
                                   ZONE_BOTTOM_END, csBL, csBR);
        int esLeft = edgeThickness3(ZONE_LEFT_START, ZONE_LEFT_MIDDLE,
                                    ZONE_LEFT_END, csTL, csBL);
        int esRight = edgeThickness3(ZONE_RIGHT_START, ZONE_RIGHT_MIDDLE,
                                     ZONE_RIGHT_END, csTR, csBR);

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
                // zc->executor is deliberately not used for these two: each
                // copy carries its own flip flag, and the segments of one edge
                // have to share one. The stored executor still earns its keep
                // as the dashboard's "do the arguments parse?" test.
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

        // Edges run between the two corners they touch, divided into three
        // independently configurable segments.
        auto addEdge = [&](Zone zStart, Zone zMiddle, Zone zEnd,
                           bool horizontal, LONG lo, LONG hi, LONG nearSide,
                           LONG farSide)
        {
            LONG sp = hi - lo;
            if (sp <= 0)
                return;

            auto rectFor = [&](LONG a, LONG b) -> RECT
            {
                return horizontal ? RECT{a, nearSide, b, farSide}
                                  : RECT{nearSide, a, farSide, b};
            };

            LONG width = sp * centrePct / 100;
            if (width < 1)
                width = 1;
            if (width > sp)
                width = sp;
            LONG cLo = lo + sp / 2 - width / 2;
            LONG cHi = cLo + width;
            if (cLo < lo)
                cLo = lo;
            if (cHi > hi)
                cHi = hi;

            const Zone seg[3] = {zStart, zMiddle, zEnd};
            const LONG bound[4] = {lo, cLo, cHi, hi};

            // Coalesce neighbouring segments that resolve to the same thing.
            // Setting all three alike therefore produces one edge-wide zone
            // rather than three, which matters for more than tidiness: three
            // adjacent zones sharing an action would each re-arm - and re-fire
            // - as the pointer crossed a seam.
            int i = 0;
            while (i < 3)
            {
                const ZoneConfig *zc = ResolveZone(mon, seg[i]);
                int j = i + 1;
                while (j < 3 && SameZoneConfig(zc, ResolveZone(mon, seg[j])))
                    j++;
                if (zc && bound[j] > bound[i])
                    add(seg[i], rectFor(bound[i], bound[j]));
                i = j;
            }
        };

        addEdge(ZONE_TOP_START, ZONE_TOP_MIDDLE, ZONE_TOP_END, true,
                r.left + csTL, r.right - csTR, r.top, r.top + esTop);
        addEdge(ZONE_BOTTOM_START, ZONE_BOTTOM_MIDDLE, ZONE_BOTTOM_END, true,
                r.left + csBL, r.right - csBR, r.bottom - esBot, r.bottom);
        addEdge(ZONE_LEFT_START, ZONE_LEFT_MIDDLE, ZONE_LEFT_END, false,
                r.top + csTL, r.bottom - csBL, r.left, r.left + esLeft);
        addEdge(ZONE_RIGHT_START, ZONE_RIGHT_MIDDLE, ZONE_RIGHT_END, false,
                r.top + csTR, r.bottom - csBR, r.right - esRight, r.right);
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
    // ShellExecuteEx reaches shell extensions through COM for URLs, .lnk and
    // .url files, folders and protocol handlers, so a Custom Command pointing
    // at any of those failed on this thread while it had no apartment - and
    // failed silently apart from a log line.
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

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
    CoUninitialize();
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
        return kTickMs;   // transient, and zones are armed - do not slow down

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

    // Neither cooldown sets g_firedThisEntry: they are a wait, not a refusal.
    // Marking the visit spent meant that walking into a corner shortly after
    // anything else fired and then parking there never fired at all - you had
    // to leave and come back. Falling through lets the dwell outlast the
    // cooldown, which is what parking in a corner is asking for.
    if (hz.cooldown > 0 && idx < (int)g_lastFireTick.size())
    {
        ULONGLONG last = g_lastFireTick[idx];
        if (last != 0 && (now - last) < (ULONGLONG)hz.cooldown)
            return next;
    }

    // Global floor across all zones. The per-zone cooldown alone does not stop
    // a sweep through several different zones from queueing a burst.
    if (g_lastAnyFireTick != 0 &&
        (now - g_lastAnyFireTick) < kMinFireIntervalMs)
        return next;

    // The cooldowns are stamped here, before the fullscreen and excluded-app
    // gates run on the worker, so a suppressed trigger still consumes them.
    // That is deliberate: those gates call SHQueryUserNotificationState and
    // OpenProcess, and putting either on the 16 ms sampling path to save a
    // cooldown the user cannot perceive would be the wrong trade. Rolling the
    // stamps back from the worker is not an option either - this vector
    // belongs to the detection thread.
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
    if (uMsg == WM_APP_REBUILD)
    {
        RebuildZones();
        return 0;
    }
    // Both of these arrive as a SendMessage broadcast, so rebuilding inline
    // blocks the sender for a QueryDisplayConfig, an EnumDisplayMonitors and
    // the whole monitor log dump. SPI_SETWORKAREA also arrives several times
    // in a row when a taskbar moves, and every rebuild resets each zone's
    // cooldown and alternation position. Posting returns at once and coalesces
    // naturally, because the loop drains its queue before the next tick.
    //
    // The work-area case is here at all because the polled topology check
    // reads SPI_GETWORKAREA, which only ever reports the primary display - a
    // taskbar moved on a secondary one was invisible to it.
    if (uMsg == WM_DISPLAYCHANGE ||
        (uMsg == WM_SETTINGCHANGE && wParam == SPI_SETWORKAREA))
    {
        Wh_Log(uMsg == WM_DISPLAYCHANGE
                   ? L"WM_DISPLAYCHANGE — queueing a zone rebuild"
                   : L"Work area changed — queueing a zone rebuild");
        PostMessage(hWnd, WM_APP_REBUILD, 0, 0);
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
// Left-click toggles the hot corners, right-click is the quick menu, and the
// menu's first item opens the dashboard. Everything configurable lives on the
// Windhawk settings page; the icon carries only runtime state.

static HANDLE g_hTrayThread = nullptr;
static DWORD g_dwTrayThreadId = 0;
static HWND g_hTrayWnd = nullptr;
static UINT g_taskbarCreatedMsg = 0;
static constexpr UINT WM_APP_TRAY = WM_APP + 10;
static constexpr UINT_PTR kTrayIconId = 1;
// Fires once when a suspension expires, purely so the icon stops saying
// "paused". Detection needs no timer - it re-reads the deadline every tick.
static constexpr UINT_PTR kSuspendTimerId = 2;

static void CancelSuspendTimer();

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
// Written by UpdateTrayIcon, which runs on the tray thread, on Windhawk's
// thread via WhTool_ModSettingsChanged, and at startup.
static std::atomic<bool> g_trayUseGuid{true};

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
    IDM_CLEAR_OVERRIDES,
    IDM_ABOUT,
};

// Defined further down with the dashboard; the tray menu needs it earlier.
static void OpenDashboard();

// Whether the hot corners are on. Runtime state rather than a setting - it has
// no equivalent on the settings page, which is why the tray still owns it.
// -1 means "never written", which reads as on.
static const wchar_t *kOvrEnabled = L"ovr_enabled";

// A screen outline with the top-left corner lit and the other three marked,
// which is the mod in one glyph. Drawn rather than shipped as a resource:
// Windhawk mods are a single source file, so there is nowhere to put an .ico.
static HICON MakeHotCornerIcon(int sz, bool enabled)
{
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

    // Mid-tone colours so it stays legible on both a light and a dark taskbar.
    DWORD *px = static_cast<DWORD *>(bits);
    const DWORD frame = enabled ? 0xFFB4B4B4 : 0xFF6E6E6E;
    const DWORD accent = enabled ? 0xFF4CC2FF : 0xFF6E6E6E;
    const DWORD dim = enabled ? 0xFF7A8A94 : 0xFF5A5A5A;

    int bord = sz >= 32 ? 2 : 1;              // outline thickness
    int arm = sz / 3;                         // length of a corner mark
    if (arm < 3)
        arm = 3;
    int thick = sz >= 32 ? 3 : 2;             // thickness of a corner mark

    for (int y = 0; y < sz; y++)
    {
        for (int x = 0; x < sz; x++)
        {
            DWORD c = 0;   // transparent

            bool onFrame = x < bord || y < bord || x >= sz - bord ||
                           y >= sz - bord;
            if (onFrame)
                c = frame;

            // Three corners get an L-shaped bracket, so the glyph reads as
            // "corners" rather than as a window with a coloured tab.
            auto bracket = [&](bool right, bool bottom) -> bool
            {
                int dx = right ? sz - 1 - x : x;
                int dy = bottom ? sz - 1 - y : y;
                return (dx < arm && dy < thick) || (dy < arm && dx < thick);
            };

            if (bracket(true, false) || bracket(false, true) ||
                bracket(true, true))
                c = dim;

            // The top-left one is filled solid: that is the corner the mod is
            // named for, and a lit corner beats a fourth identical bracket.
            if (x < arm && y < arm)
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

static HICON MakeTrayIcon(bool enabled)
{
    return MakeHotCornerIcon(GetSystemMetrics(SM_CXSMICON), enabled);
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
// Reading a pre-4.2 layout
// =====================================================================
//
// Nothing writes any of this any more - the settings page is the only store.
// It is read once per reload, and only when the settings page has no displays
// on it, so that upgrading from a version that had no settings page does not
// silently discard a zone layout that took effort to enter.
//
// One key per field rather than a packed string, which is what those versions
// wrote: wordy, but readable if it ever needs debugging by hand.

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
// =====================================================================
// Windhawk settings page
// =====================================================================
//
// The settings page is the configuration surface; the dashboard only shows
// what it produced. Two surfaces writing the same store is what produced the
// 4.1.3 and 4.1.4 bugs, where the dashboard addressed configurations by list
// position and overwrote the wrong one.
//
// Precedence, decided once per reload:
//
//   1. Displays configured on the settings page win outright.
//   2. Otherwise, a layout saved by an older version still runs, so upgrading
//      never silently wipes a configuration that took effort to enter.
//   3. Otherwise, settings supply the globals and no zones are active.

static int ParseZoneName(const std::wstring &s)
{
    static const struct
    {
        const wchar_t *name;
        Zone zone;
    } kMap[] = {
        {L"TOP_LEFT", ZONE_TOP_LEFT},
        {L"TOP_RIGHT", ZONE_TOP_RIGHT},
        {L"BOTTOM_LEFT", ZONE_BOTTOM_LEFT},
        {L"BOTTOM_RIGHT", ZONE_BOTTOM_RIGHT},
        {L"TOP_START", ZONE_TOP_START},
        {L"TOP_MIDDLE", ZONE_TOP_MIDDLE},
        {L"TOP_END", ZONE_TOP_END},
        {L"BOTTOM_START", ZONE_BOTTOM_START},
        {L"BOTTOM_MIDDLE", ZONE_BOTTOM_MIDDLE},
        {L"BOTTOM_END", ZONE_BOTTOM_END},
        {L"LEFT_START", ZONE_LEFT_START},
        {L"LEFT_MIDDLE", ZONE_LEFT_MIDDLE},
        {L"LEFT_END", ZONE_LEFT_END},
        {L"RIGHT_START", ZONE_RIGHT_START},
        {L"RIGHT_MIDDLE", ZONE_RIGHT_MIDDLE},
        {L"RIGHT_END", ZONE_RIGHT_END},
    };
    for (const auto &m : kMap)
        if (s == m.name)
            return (int)m.zone;
    return -1;
}

// Windhawk only accepts $options on a string setting, so the modifier arrives
// by name. -1 is the "inherit" marker the zone tuning already uses; the global
// has no INHERIT option, so its caller maps a negative result onto None.
static int ParseModifierName(const std::wstring &s)
{
    if (s == L"NONE")
        return 0;
    if (s == L"CTRL")
        return 1;
    if (s == L"ALT")
        return 2;
    if (s == L"SHIFT")
        return 3;
    if (s == L"WIN")
        return 4;
    return -1;   // INHERIT, empty, or anything unrecognised
}

// Windhawk has no array-length call, so both loops run until the first
// entry whose required string comes back empty - the standard idiom, and
// what makes an empty Display name terminate the list.
static std::wstring GetSettingStr(const wchar_t *fmt, int a, int b = -1)
{
    PCWSTR raw = (b < 0) ? Wh_GetStringSetting(fmt, a)
                         : Wh_GetStringSetting(fmt, a, b);
    std::wstring out = raw ? raw : L"";
    Wh_FreeStringSetting(raw);
    return out;
}

// For the settings that are not inside an array, where passing an index into a
// format string with no %d in it just read like a bug.
static std::wstring GetSettingStr(const wchar_t *name)
{
    PCWSTR raw = Wh_GetStringSetting(name);
    std::wstring out = raw ? raw : L"";
    Wh_FreeStringSetting(raw);
    return out;
}

static std::vector<MonitorZoneConfig> ReadSettingsZones()
{
    std::vector<MonitorZoneConfig> configs;

    // Scanned to a fixed bound rather than stopping at the first blank, so
    // clearing one display's name does not silently drop every entry after it.
    // Reading past the end of a Windhawk array just returns empty strings.
    for (int i = 0; i < 64; i++)
    {
        std::wstring id = TrimStr(GetSettingStr(L"displays[%d].monitor", i));
        if (id.empty())
            continue;

        MonitorZoneConfig cfg;
        cfg.monitorId = id;

        bool seen[ZONE_COUNT] = {};
        bool any = false;

        for (int z = 0;; z++)
        {
            std::wstring zname =
                GetSettingStr(L"displays[%d].zones[%d].zone", i, z);
            if (zname.empty())
                break;

            int zi = ParseZoneName(zname);
            std::wstring aname =
                GetSettingStr(L"displays[%d].zones[%d].action", i, z);
            std::wstring args =
                GetSettingStr(L"displays[%d].zones[%d].args", i, z);

            // An unknown zone name, or a repeat of one already filled in,
            // is skipped rather than allowed to overwrite.
            if (zi < 0 || seen[zi])
                continue;
            seen[zi] = true;

            CornerAction act = ParseActionType(aname);
            cfg.zones[zi].action = act;
            cfg.zones[zi].args = args;
            cfg.zones[zi].executor = MakeExecutor(act, args);
            cfg.zones[zi].tuning.size =
                Wh_GetIntSetting(L"displays[%d].zones[%d].size", i, z);
            cfg.zones[zi].tuning.delay =
                Wh_GetIntSetting(L"displays[%d].zones[%d].delay", i, z);
            cfg.zones[zi].tuning.settle =
                Wh_GetIntSetting(L"displays[%d].zones[%d].settle", i, z);
            cfg.zones[zi].tuning.knock =
                Wh_GetIntSetting(L"displays[%d].zones[%d].knock", i, z);
            cfg.zones[zi].tuning.cooldown =
                Wh_GetIntSetting(L"displays[%d].zones[%d].cooldown", i, z);
            cfg.zones[zi].tuning.modifier = ParseModifierName(
                GetSettingStr(L"displays[%d].zones[%d].modifier", i, z));

            if (act != CornerAction::Nothing)
                any = true;
        }

        if (any)
            configs.push_back(std::move(cfg));
    }

    return configs;
}

// Every global, straight off the settings page.
static void ApplySettingsGlobals(ModSettings &s)
{
    auto clamp = [](int v, int lo, int hi)
    { return v < lo ? lo : (v > hi ? hi : v); };

    s.cornerSize = clamp(Wh_GetIntSetting(L"cornerSize"), 1, 500);
    s.edgeSize = clamp(Wh_GetIntSetting(L"edgeSize"), 1, 500);
    s.centerZonePercent = clamp(Wh_GetIntSetting(L"centerPercent"), 1, 90);
    s.activationDelay = clamp(Wh_GetIntSetting(L"delay"), 0, 10000);
    s.settleMs = clamp(Wh_GetIntSetting(L"settle"), 0, 10000);
    s.knockWindowMs = clamp(Wh_GetIntSetting(L"knock"), 0, 10000);
    s.cooldownMs = clamp(Wh_GetIntSetting(L"cooldown"), 0, 60000);
    int mod = ParseModifierName(GetSettingStr(L"requireModifier"));
    s.requireModifier = (mod < 0) ? 0 : mod;
    s.disableOnFullscreen = Wh_GetIntSetting(L"disableOnFullscreen") != 0;
    s.disableDuringDrag = Wh_GetIntSetting(L"disableDuringDrag") != 0;
    s.avoidTaskbar = Wh_GetIntSetting(L"avoidTaskbar") != 0;

    g_lockBlankDelayMs = clamp(Wh_GetIntSetting(L"lockBlankDelay"), 0, 10000);
    g_showMonitorNames = Wh_GetIntSetting(L"showMonitorNames") != 0;

    std::wstring rest = GetSettingStr(L"excluded");
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
}

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
        cfg.monitorId = id;

        // The store predates the edge split, so its twelve slots have to be
        // spread across sixteen. A stored edge fills both outer segments; it
        // also fills the middle when nothing was stored for the centre, which
        // is what reproduces the old geometry exactly - back then an edge with
        // no centre configured was one rectangle spanning the whole edge, and
        // three identical segments coalesce back into exactly that.
        static const struct
        {
            int edge;                 // legacy slot 4-7
            int centre;               // legacy slot 8-11
            Zone start, middle, end;
        } kSpread[] = {
            {4, 8, ZONE_TOP_START, ZONE_TOP_MIDDLE, ZONE_TOP_END},
            {5, 9, ZONE_BOTTOM_START, ZONE_BOTTOM_MIDDLE, ZONE_BOTTOM_END},
            {6, 10, ZONE_LEFT_START, ZONE_LEFT_MIDDLE, ZONE_LEFT_END},
            {7, 11, ZONE_RIGHT_START, ZONE_RIGHT_MIDDLE, ZONE_RIGHT_END},
        };

        auto readSlot = [&](int z, ZoneConfig &dst)
        {
            std::wstring a = GetStrValue(GuiKey(i, z, L"a").c_str());
            std::wstring g = GetStrValue(GuiKey(i, z, L"g").c_str());
            CornerAction act = ParseActionType(a);
            dst.action = act;
            dst.args = g;
            dst.executor = MakeExecutor(act, g);
            dst.tuning.size = Wh_GetIntValue(GuiKey(i, z, L"sz").c_str(), -1);
            dst.tuning.delay = Wh_GetIntValue(GuiKey(i, z, L"dl").c_str(), -1);
            dst.tuning.settle = Wh_GetIntValue(GuiKey(i, z, L"gd").c_str(), -1);
            dst.tuning.knock = Wh_GetIntValue(GuiKey(i, z, L"kn").c_str(), -1);
            dst.tuning.cooldown = Wh_GetIntValue(GuiKey(i, z, L"cd").c_str(), -1);
            dst.tuning.modifier = Wh_GetIntValue(GuiKey(i, z, L"md").c_str(), -1);
            return act != CornerAction::Nothing;
        };

        bool any = false;
        for (int z = ZONE_TOP_LEFT; z <= ZONE_BOTTOM_RIGHT; z++)
            any |= readSlot(z, cfg.zones[z]);

        for (const auto &sp : kSpread)
        {
            ZoneConfig edge, centre;
            bool hasEdge = readSlot(sp.edge, edge);
            bool hasCentre = readSlot(sp.centre, centre);

            if (hasEdge)
            {
                cfg.zones[sp.start] = edge;
                cfg.zones[sp.end] = edge;
                if (!hasCentre)
                    cfg.zones[sp.middle] = edge;
                any = true;
            }
            if (hasCentre)
            {
                cfg.zones[sp.middle] = centre;
                any = true;
            }
        }

        if (any)
            configs.push_back(std::move(cfg));
    }
    return configs;
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
// The settings page owns every global. The value store is read only for the
// enable flag and, on the upgrade path, for a zone layout saved before the
// settings page came back - never for anything a page field also controls,
// because the two would then disagree with nothing on screen saying which won.
static void ReloadConfig()
{
    EnterCriticalSection(&g_reloadLock);

    ModSettings s;   // the member initialisers are the defaults

    int v = Wh_GetIntValue(kOvrEnabled, -1);
    g_trayEnabled = (v < 0) ? true : (v != 0);

    s.monitorConfigs = ReadSettingsZones();
    const bool fromSettings = !s.monitorConfigs.empty();

    // The settings page always owns the globals. Applying them only on the
    // settings path meant that anyone upgrading had a page full of fields that
    // did nothing and no way to tell: change Cooldown from 300 to 800, see
    // nothing happen. Only the zone list falls back to the stored layout.
    ApplySettingsGlobals(s);

    // The handover is one-way. Without this, a user who lists a display and
    // later sets its zones back to Nothing drops out of fromSettings and the
    // pre-4.2 layout comes back from the dead, which is not what clearing a
    // zone asks for.
    if (fromSettings && Wh_GetIntValue(L"settings_owner", 0) == 0)
        Wh_SetIntValue(L"settings_owner", 1);

    bool legacy = false;
    std::wstring dropped;

    if (!fromSettings && Wh_GetIntValue(L"settings_owner", 0) == 0 &&
        Wh_GetIntValue(L"gui_active", 0) != 0)
    {
        legacy = true;
        s.monitorConfigs = ReadDashboardZones();

        // The stored globals are no longer applied, so name the ones that were
        // customised. Reverting them silently would look like the mod losing a
        // setting; naming them says exactly what to re-enter and where.
        static const struct
        {
            const wchar_t *key;
            const wchar_t *name;
        } kOldGlobals[] = {
            {L"ovr_corner", L"Corner size"},
            {L"ovr_edge", L"Edge thickness"},
            {L"ovr_centre", L"Edge centre width"},
            {L"ovr_delay", L"Activation delay"},
            {L"ovr_settle", L"Pass-through guard"},
            {L"ovr_knock", L"Knock window"},
            {L"ovr_cooldown", L"Cooldown"},
            {L"ovr_modifier", L"Required modifier"},
            {L"ovr_taskbar", L"Keep zones out of the taskbar"},
            {L"ovr_lockblank", L"Lock-then-blank delay"},
            {L"ovr_monnames", L"List display names in the log"},
            {L"ovr_fullscreen", L"Skip while fullscreen"},
            {L"ovr_drag", L"Skip while dragging"},
        };
        for (const auto &o : kOldGlobals)
        {
            if (Wh_GetIntValue(o.key, -1) < 0)
                continue;
            if (!dropped.empty())
                dropped += L", ";
            dropped += o.name;
        }
        if (!GetStrValue(L"ovr_excluded").empty())
        {
            if (!dropped.empty())
                dropped += L", ";
            dropped += L"Excluded applications";
        }
    }

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

    if (legacy)
    {
        Wh_Log(L"Zones are coming from a layout saved by an older version (%d "
               L"configuration%s). Everything else on this mod's Settings page "
               L"is live. Re-enter the zones there to take them over - as soon "
               L"as one display is listed, this layout is ignored.",
               zoneCount, zoneCount == 1 ? L"" : L"s");
        if (!dropped.empty())
            Wh_Log(L"These were customised in the old version and are now taken "
                   L"from the Settings page instead, so set them again there if "
                   L"you still want them: %s.",
                   dropped.c_str());
    }
    else if (zoneCount)
    {
        Wh_Log(L"Using the Settings page (%d display%s configured)",
               zoneCount, zoneCount == 1 ? L"" : L"s");
    }
    else
    {
        Wh_Log(L"No displays are configured. Open this mod's Settings page and "
               L"add one, then add the zones you want.");
    }
    Wh_Log(L"Sizes: corner %dpx, edge %dpx.  Timing: delay %dms, "
           L"pass-through guard %dms, cooldown %dms.",
           cs, es, dl, st, cd);
    Wh_Log(L"Skip while fullscreen: %s.  Skip while dragging: %s.  "
           L"Excluded apps: %d.",
           fs ? L"yes" : L"no", dg ? L"yes" : L"no", nx);

    LeaveCriticalSection(&g_reloadLock);
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

    // "Skip while fullscreen" and "Skip while dragging" used to live here. They
    // are settings, not runtime state, and a mod cannot write its own settings,
    // so the tray copy could only ever diverge - toggling one worked until the
    // next reload put it back. They belong to the settings page alone now;
    // enable and suspend stay because neither has a page equivalent.
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_CLEAR_OVERRIDES,
                L"Reset enable and suspend");
    AppendMenuW(hMenu, MF_STRING | MF_DISABLED, IDM_ABOUT,
                L"Win-X Hot Corners " WH_MOD_VERSION);

    // Anchor to the tray icon rather than to the pointer. A menu that opens
    // wherever the cursor happened to be lands at a different place every
    // time; anchoring puts it in the one spot muscle memory expects, which is
    // what every other tray menu does.
    UINT align = TPM_RIGHTBUTTON;
    // Mirror however the icon was actually registered. Passing the GUID after
    // UpdateTrayIcon has fallen back to plain uID identity makes the lookup
    // fail, and the menu quietly stops anchoring - the one case this exists for.
    NOTIFYICONIDENTIFIER nii = {sizeof(nii)};
    nii.hWnd = g_hTrayWnd;
    if (g_trayUseGuid)
        nii.guidItem = kTrayIconGuid;
    else
        nii.uID = (UINT)kTrayIconId;
    RECT icon = {};
    if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &icon)))
    {
        // Open from the icon's outer edge, away from whichever screen edge the
        // taskbar is docked against, so the menu never covers the icon.
        APPBARDATA abd = {sizeof(abd)};
        UINT edge = ABE_BOTTOM;
        if (SHAppBarMessage(ABM_GETTASKBARPOS, &abd))
            edge = abd.uEdge;

        switch (edge)
        {
        case ABE_TOP:
            pt.x = (icon.left + icon.right) / 2;
            pt.y = icon.bottom;
            align |= TPM_CENTERALIGN | TPM_TOPALIGN;
            break;
        case ABE_LEFT:
            pt.x = icon.right;
            pt.y = (icon.top + icon.bottom) / 2;
            align |= TPM_LEFTALIGN | TPM_VCENTERALIGN;
            break;
        case ABE_RIGHT:
            pt.x = icon.left;
            pt.y = (icon.top + icon.bottom) / 2;
            align |= TPM_RIGHTALIGN | TPM_VCENTERALIGN;
            break;
        default:
            pt.x = (icon.left + icon.right) / 2;
            pt.y = icon.top;
            align |= TPM_CENTERALIGN | TPM_BOTTOMALIGN;
            break;
        }
    }

    // Required so the menu dismisses when the user clicks elsewhere.
    SetForegroundWindow(g_hTrayWnd);
    TrackPopupMenu(hMenu, align, pt.x, pt.y, 0, g_hTrayWnd, nullptr);
    PostMessage(g_hTrayWnd, WM_NULL, 0, 0);

    DestroyMenu(hMenu);
}

// The zone snapshot carries drag/settle, so a change there needs a rebuild.
static void RequestRebuild()
{
    if (g_hDetectWnd)
        PostMessage(g_hDetectWnd, WM_APP_REBUILD, 0, 0);
}

static void CancelSuspendTimer()
{
    if (g_hTrayWnd)
        KillTimer(g_hTrayWnd, kSuspendTimerId);
}

static void HandleTrayCommand(UINT id)
{
    switch (id)
    {
    case IDM_SETTINGS:
        OpenDashboard();
        return;

    // Same pairing as the two guards below: ReloadConfig reads kOvrEnabled and
    // republishes g_trayEnabled from it, so a reload straddling this toggle
    // could put the flag back and leave it disagreeing with the store.
    case IDM_ENABLED:
        EnterCriticalSection(&g_reloadLock);
        g_trayEnabled = !g_trayEnabled;
        Wh_SetIntValue(kOvrEnabled, g_trayEnabled ? 1 : 0);
        LeaveCriticalSection(&g_reloadLock);
        g_suspendUntil = 0;
        CancelSuspendTimer();
        Wh_Log(L"Tray: hot corners %s", g_trayEnabled ? L"enabled" : L"disabled");
        break;

    case IDM_SUSPEND_15:
    case IDM_SUSPEND_30:
    case IDM_SUSPEND_60:
    {
        int mins = (id == IDM_SUSPEND_15) ? 15 : (id == IDM_SUSPEND_30) ? 30 : 60;
        g_suspendUntil = GetTickCount64() + (ULONGLONG)mins * 60 * 1000;
        // Detection re-reads the deadline every tick, so the corners come back
        // on their own - but nothing redrew the icon, which stayed dimmed and
        // still said "paused" for the rest of the session. Suspension is
        // exactly when the icon is the thing being looked at.
        if (g_hTrayWnd)
            SetTimer(g_hTrayWnd, kSuspendTimerId,
                     (UINT)mins * 60 * 1000 + 1000, nullptr);
        Wh_Log(L"Tray: suspended for %d minutes", mins);
        break;
    }

    case IDM_RESUME:
        g_suspendUntil = 0;
        CancelSuspendTimer();
        Wh_Log(L"Tray: resumed");
        break;

    // Only what this menu owns: the enable flag and the suspend timer. Wiping
    // the zone layout from a menu item with no confirmation would be a trap,
    // and everything else now lives on the settings page.
    case IDM_CLEAR_OVERRIDES:
        EnterCriticalSection(&g_reloadLock);
        Wh_SetIntValue(kOvrEnabled, -1);
        g_suspendUntil = 0;
        CancelSuspendTimer();
        ReloadConfig();
        LeaveCriticalSection(&g_reloadLock);
        RequestRebuild();
        Wh_Log(L"Tray: enable and suspend reset to defaults");
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
    IDC_CLOSE = 1000,
};

// A resolved, read-only view of one zone. The dashboard shows what is actually
// in effect, so it resolves exactly the way ResolveZone does - per zone, own
// configuration first, then the wildcard. Anything less and the picture would
// be able to disagree with what fires.
struct ZoneView
{
    CornerAction action = CornerAction::Nothing;
    std::wstring args;
    ZoneTuning tuning;          // -1 in a field means "inherited"
    bool fromWildcard = false;
    bool invalid = false;       // configured, but its arguments do not parse
};

struct DisplayView
{
    std::wstring id;
    int width = 0;
    int height = 0;
    bool primary = false;
    bool present = true;        // attached right now
    bool wildcard = false;      // the "*" configuration
    ZoneView zones[ZONE_COUNT];
    int configured = 0;
};

struct DashState
{
    UINT dpi = 96;
    HFONT hFont = nullptr;
    HFONT hFontBold = nullptr;
    HFONT hFontSmall = nullptr;
    HFONT hFontVert = nullptr;   // rotated 90 degrees, for the side strips
    HBRUSH hBg = nullptr;
    HICON hIcon = nullptr;     // title bar / Alt-Tab, big
    HICON hIconSm = nullptr;   // title bar, small

    std::vector<DisplayView> displays;
    ModSettings globals;         // what an inherited value resolves to
    bool legacy = false;         // running a layout saved before 4.2

    int activeTab = 0;
    int hoverZone = -1;
    int selZone = -1;
    std::vector<RECT> tabRects;  // filled while painting, used for hit testing

    HWND hClose = nullptr;
};

static int Sc(int px, UINT dpi) { return MulDiv(px, (int)dpi, 96); }

// Layout metrics at 96 DPI. The window is sized from these rather than the
// other way round.
namespace Lay
{
constexpr int Pad = 18;
constexpr int Gap = 12;
constexpr int TabH = 36;
constexpr int BtnH = 30;
constexpr int BtnW = 96;

constexpr int DiagAreaW = 560;   // the box is fitted inside this, and centred
constexpr int DiagAreaH = 300;
constexpr int DiagCapH = 20;     // the "3840 x 2160 - primary" caption under it

constexpr int DetHeadH = 56;     // zone name, action, setting key
constexpr int DetRowH = 21;
constexpr int DetHdrH = 20;
constexpr int DetailH = DetHeadH + DetHdrH + 6 * DetRowH;

constexpr int ClientW = DiagAreaW + Pad * 2;
constexpr int ClientH = Pad + TabH + Gap + DiagAreaH + DiagCapH + Gap +
                        DetailH + Gap + BtnH + Pad;
}  // namespace Lay

// The area the screen box is fitted into.
static RECT DashDiagramArea(UINT dpi)
{
    RECT r;
    r.left = Sc(Lay::Pad, dpi);
    r.top = Sc(Lay::Pad + Lay::TabH + Lay::Gap, dpi);
    r.right = r.left + Sc(Lay::DiagAreaW, dpi);
    r.bottom = r.top + Sc(Lay::DiagAreaH, dpi);
    return r;
}

// The screen box itself: the display's real aspect ratio, fitted inside the
// area and centred. A portrait or ultrawide display therefore looks like one
// instead of being forced into a fixed rectangle.
static RECT DashDiagramRect(const DashState *s)
{
    RECT a = DashDiagramArea(s->dpi);
    int aw = a.right - a.left, ah = a.bottom - a.top;

    int mw = 16, mh = 9;
    if (s->activeTab >= 0 && s->activeTab < (int)s->displays.size())
    {
        const DisplayView &d = s->displays[s->activeTab];
        if (d.width > 0 && d.height > 0)
        {
            mw = d.width;
            mh = d.height;
        }
    }

    // Fit, never stretch: whichever axis runs out first sets the scale.
    int w = aw, h = (int)((LONGLONG)aw * mh / mw);
    if (h > ah)
    {
        h = ah;
        w = (int)((LONGLONG)ah * mw / mh);
    }

    RECT r;
    r.left = a.left + (aw - w) / 2;
    r.top = a.top + (ah - h) / 2;
    r.right = r.left + w;
    r.bottom = r.top + h;
    return r;
}

// Proportions inside the preview. Deliberately not to scale: the real zones are
// a few pixels thick and would be invisible. What has to be true is that the
// blocks tile the border exactly, with no overlap and no gap.
//
// The corner block stays square by deriving from the shorter side. The edge
// band is thick enough to hold a label - at the old c/2 a side strip was too
// narrow and the text spilled outside the screen box.
//
// Every zone is exactly one rectangle now that an edge is three segments
// rather than one zone wrapped around a centre, which is what removed the
// old two-part special case and the overlap bug that came with it.
static RECT ZoneRectInDiagram(Zone z, const RECT &d)
{
    int w = d.right - d.left, h = d.bottom - d.top;
    int c = (w < h ? w : h) / 5;         // corner block, square
    int t = c * 62 / 100;                // edge band thickness
    int cw = w * 22 / 100, ch = h * 22 / 100;   // middle segment extent
    int cx0 = d.left + w / 2 - cw / 2, cx1 = cx0 + cw;
    int cy0 = d.top + h / 2 - ch / 2, cy1 = cy0 + ch;

    switch (z)
    {
    case ZONE_TOP_LEFT:      return {d.left, d.top, d.left + c, d.top + c};
    case ZONE_TOP_RIGHT:     return {d.right - c, d.top, d.right, d.top + c};
    case ZONE_BOTTOM_LEFT:   return {d.left, d.bottom - c, d.left + c, d.bottom};
    case ZONE_BOTTOM_RIGHT:  return {d.right - c, d.bottom - c, d.right, d.bottom};

    case ZONE_TOP_START:     return {d.left + c, d.top, cx0, d.top + t};
    case ZONE_TOP_MIDDLE:    return {cx0, d.top, cx1, d.top + t};
    case ZONE_TOP_END:       return {cx1, d.top, d.right - c, d.top + t};

    case ZONE_BOTTOM_START:  return {d.left + c, d.bottom - t, cx0, d.bottom};
    case ZONE_BOTTOM_MIDDLE: return {cx0, d.bottom - t, cx1, d.bottom};
    case ZONE_BOTTOM_END:    return {cx1, d.bottom - t, d.right - c, d.bottom};

    case ZONE_LEFT_START:    return {d.left, d.top + c, d.left + t, cy0};
    case ZONE_LEFT_MIDDLE:   return {d.left, cy0, d.left + t, cy1};
    case ZONE_LEFT_END:      return {d.left, cy1, d.left + t, d.bottom - c};

    case ZONE_RIGHT_START:   return {d.right - t, d.top + c, d.right, cy0};
    case ZONE_RIGHT_MIDDLE:  return {d.right - t, cy0, d.right, cy1};
    case ZONE_RIGHT_END:     return {d.right - t, cy1, d.right, d.bottom - c};

    default:                 return {0, 0, 0, 0};
    }
}

static bool ZoneIsCorner(Zone z) { return z <= ZONE_BOTTOM_RIGHT; }

// The left and right strips are tall and narrow. Rotating the text is the only
// way a label such as "Notification Centre" fits inside one.
static bool ZoneIsVertical(Zone z)
{
    return z >= ZONE_LEFT_START && z <= ZONE_RIGHT_END;
}

// subIdList is nullptr for "keep the default part list", which is what naming
// a theme (DarkMode_CFD) wants. Disabling theming outright is the exception:
// SetWindowTheme only stops theming a control when *both* strings are empty,
// and a still-themed control ignores colour messages entirely.
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

// =====================================================================
// Reading the live configuration
// =====================================================================
//
// The dashboard reads what ReloadConfig already resolved, not the value store.
// That is what makes it correct for a settings-page layout and a pre-4.2 one
// without knowing which is in use, and it is why it can no longer drift from
// what actually fires.

static const wchar_t *kTuningNames[] = {
    L"Size", L"Activation delay", L"Pass-through guard",
    L"Knock window", L"Cooldown", L"Modifier",
};

// Pulls one tuning field out by index, keeping the six rows and the six struct
// members from being wired up twice.
static int TuningField(const ZoneTuning &t, int i)
{
    switch (i)
    {
    case 0: return t.size;
    case 1: return t.delay;
    case 2: return t.settle;
    case 3: return t.knock;
    case 4: return t.cooldown;
    default: return t.modifier;
    }
}

// The global an unset field falls back to. Size is the odd one out: a corner
// and an edge inherit different globals.
static int GlobalField(const ModSettings &g, int i, Zone z)
{
    switch (i)
    {
    case 0: return (z <= ZONE_BOTTOM_RIGHT) ? g.cornerSize : g.edgeSize;
    case 1: return g.activationDelay;
    case 2: return g.settleMs;
    case 3: return g.knockWindowMs;
    case 4: return g.cooldownMs;
    default: return g.requireModifier;
    }
}

static std::wstring FormatTuning(int i, int value)
{
    wchar_t buf[64];
    if (i == 0)
        _snwprintf_s(buf, _countof(buf), _TRUNCATE, L"%d px", value);
    else if (i == 5)
    {
        static const wchar_t *kMods[] = {L"None", L"Ctrl", L"Alt", L"Shift",
                                         L"Win"};
        return (value >= 0 && value < 5) ? kMods[value] : L"None";
    }
    else if (i == 3 && value == 0)
        return L"off";
    else
        _snwprintf_s(buf, _countof(buf), _TRUNCATE, L"%d ms", value);
    return buf;
}

// Mirrors ResolveZone: the display's own configuration first, then the
// wildcard, decided per zone rather than per display.
static void DashFillZones(DisplayView &dv,
                          const std::vector<MonitorZoneConfig> &cfgs)
{
    for (int z = 0; z < ZONE_COUNT; z++)
    {
        const ZoneConfig *hit = nullptr;
        // An entry that names an action but produced no executor - an
        // unparseable key combination, or an Alternate action missing its "|".
        // ResolveZone skips those and keeps looking, so this has to as well:
        // marking the zone broken when the wildcard actually rescues it would
        // be the picture disagreeing with what fires, in the one direction
        // that matters most.
        const ZoneConfig *broken = nullptr;
        bool wild = false;
        bool disabled = false;

        if (!dv.wildcard)
        {
            for (const auto &cfg : cfgs)
            {
                if (cfg.monitorId.empty() || cfg.monitorId == L"*")
                    continue;
                if (_wcsicmp(cfg.monitorId.c_str(), dv.id.c_str()) != 0)
                    continue;
                const ZoneConfig &zc = cfg.zones[z];
                if (zc.action == CornerAction::DisabledHere)
                {
                    disabled = true;
                    break;
                }
                if (zc.action == CornerAction::Nothing)
                    continue;
                if (zc.executor)
                {
                    hit = &zc;
                    break;
                }
                if (!broken)
                    broken = &zc;
            }
        }

        if (!hit && !disabled)
        {
            for (const auto &cfg : cfgs)
            {
                if (cfg.monitorId != L"*")
                    continue;
                const ZoneConfig &zc = cfg.zones[z];
                if (zc.action == CornerAction::Nothing ||
                    zc.action == CornerAction::DisabledHere)
                    continue;
                if (zc.executor)
                {
                    hit = &zc;
                    wild = !dv.wildcard;
                    break;
                }
                if (!broken)
                    broken = &zc;
            }
        }

        if (disabled)
        {
            dv.zones[z].action = CornerAction::DisabledHere;
            dv.configured++;
            continue;
        }

        // Nothing resolved, so the unparseable entry really is what the user
        // gets - nothing at all. That is worth saying out loud.
        if (!hit)
        {
            if (!broken)
                continue;
            hit = broken;
            dv.zones[z].invalid = true;
        }

        dv.zones[z].action = hit->action;
        dv.zones[z].args = hit->args;
        dv.zones[z].tuning = hit->tuning;
        dv.zones[z].fromWildcard = wild;
        dv.configured++;
    }
}

static void DashBuildSnapshot(DashState *s)
{
    s->displays.clear();
    s->tabRects.clear();

    // Attached displays, with their geometry, come from the published zone
    // snapshot - g_monitors belongs to the detection thread, which clears and
    // refills it underneath anyone else.
    std::shared_ptr<const ZoneSet> snap;
    EnterCriticalSection(&g_zonesLock);
    snap = g_zones;
    LeaveCriticalSection(&g_zonesLock);

    std::vector<MonitorZoneConfig> cfgs;
    {
        EnterCriticalSection(&g_settingsLock);
        cfgs = g_settings.monitorConfigs;
        s->globals = g_settings;
        LeaveCriticalSection(&g_settingsLock);
    }
    s->globals.monitorConfigs.clear();   // not needed, and it is not small

    if (snap)
    {
        for (const auto &m : snap->monitors)
        {
            DisplayView dv;
            dv.id = m.id;
            dv.width = m.width;
            dv.height = m.height;
            dv.primary = m.primary;
            dv.present = true;
            DashFillZones(dv, cfgs);
            s->displays.push_back(std::move(dv));
        }
    }

    // A configuration for a display that is not plugged in right now is still
    // real and still worth seeing, so it gets a tab of its own.
    for (const auto &cfg : cfgs)
    {
        if (cfg.monitorId.empty() || cfg.monitorId == L"*")
            continue;
        bool known = false;
        for (const auto &dv : s->displays)
            if (_wcsicmp(dv.id.c_str(), cfg.monitorId.c_str()) == 0)
                known = true;
        if (known)
            continue;

        DisplayView dv;
        dv.id = cfg.monitorId;
        dv.present = false;
        DashFillZones(dv, cfgs);
        s->displays.push_back(std::move(dv));
    }

    for (const auto &cfg : cfgs)
    {
        if (cfg.monitorId != L"*")
            continue;
        DisplayView dv;
        dv.id = L"All displays";
        dv.wildcard = true;
        DashFillZones(dv, cfgs);
        s->displays.push_back(std::move(dv));
        break;
    }

    if (s->activeTab >= (int)s->displays.size())
        s->activeTab = 0;
}

// =====================================================================
// Painting
// =====================================================================

static HFONT DashMakeFont(UINT dpi, int pt, bool bold, int escapement)
{
    LOGFONTW lf = {};
    lf.lfHeight = -Sc(pt, dpi);
    lf.lfWeight = bold ? FW_SEMIBOLD : FW_NORMAL;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = CLEARTYPE_QUALITY;
    lf.lfEscapement = escapement;
    lf.lfOrientation = escapement;
    wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"Segoe UI");
    return CreateFontIndirectW(&lf);
}

static void DashPaintTabs(DashState *s, HDC hdc, const RECT &client)
{
    UINT d = s->dpi;
    s->tabRects.clear();

    int x = Sc(Lay::Pad, d);
    int top = Sc(Lay::Pad, d);
    int h = Sc(Lay::TabH, d);
    const int strip = client.right - Sc(Lay::Pad, d) * 2;
    const int n = (int)s->displays.size();

    HFONT old = (HFONT)SelectObject(hdc, s->hFont);
    SetBkMode(hdc, TRANSPARENT);

    // Natural widths first. Laid out unbounded, four or five displays - or two
    // long EDID names plus the wildcard tab - ran off the right edge, and the
    // tabs past it were drawn outside the window and unreachable by mouse.
    // Nothing here scrolls, so instead every tab is capped at an equal share
    // and its label ellipsised; the badge and the dot always survive.
    std::vector<int> width((size_t)n, 0);
    int total = 0;
    for (int i = 0; i < n; i++)
    {
        const DisplayView &dv = s->displays[i];
        wchar_t count[16];
        _snwprintf_s(count, _countof(count), _TRUNCATE, L"%d", dv.configured);

        SIZE ts = {}, cs = {};
        SelectObject(hdc, i == s->activeTab ? s->hFontBold : s->hFont);
        GetTextExtentPoint32W(hdc, dv.id.c_str(), (int)dv.id.size(), &ts);
        SelectObject(hdc, s->hFontSmall);
        GetTextExtentPoint32W(hdc, count, (int)wcslen(count), &cs);

        int dot = dv.present ? 0 : Sc(14, d);
        width[(size_t)i] =
            Sc(12, d) + dot + ts.cx + Sc(8, d) + cs.cx + Sc(12, d) + Sc(12, d);
        total += width[(size_t)i];
    }

    const int cap = (n > 0 && total > strip) ? strip / n : 0;

    for (int i = 0; i < n; i++)
    {
        const DisplayView &dv = s->displays[i];
        bool active = (i == s->activeTab);

        wchar_t count[16];
        _snwprintf_s(count, _countof(count), _TRUNCATE, L"%d", dv.configured);

        SIZE ts = {}, cs = {};
        SelectObject(hdc, active ? s->hFontBold : s->hFont);
        GetTextExtentPoint32W(hdc, dv.id.c_str(), (int)dv.id.size(), &ts);
        SelectObject(hdc, s->hFontSmall);
        GetTextExtentPoint32W(hdc, count, (int)wcslen(count), &cs);

        int dot = dv.present ? 0 : Sc(14, d);
        int badge = cs.cx + Sc(12, d);
        int wTab = width[(size_t)i];
        if (cap > 0 && wTab > cap)
            wTab = cap;

        RECT tr = {x, top, x + wTab, top + h};
        s->tabRects.push_back(tr);

        int tx = tr.left + Sc(12, d);

        // A saved configuration for a display that is not plugged in is not an
        // error, but it should not read as live either.
        if (!dv.present)
        {
            int r = Sc(3, d);
            int cy = tr.top + h / 2;
            HBRUSH db = CreateSolidBrush(g_pal.dim);
            RECT dr = {tx, cy - r, tx + 2 * r, cy + r};
            FillRect(hdc, &dr, db);
            DeleteObject(db);
            tx += dot;
        }

        // The badge is reserved out of the label's space rather than pushed
        // off the end, so a truncated tab still shows its count.
        int labelRight = tr.right - Sc(12, d) - badge - Sc(8, d);
        if (labelRight < tx)
            labelRight = tx;
        SelectObject(hdc, active ? s->hFontBold : s->hFont);
        SetTextColor(hdc, active ? g_pal.text : g_pal.dim);
        RECT lr = {tx, tr.top, labelRight, tr.bottom};
        DrawTextW(hdc, dv.id.c_str(), -1, &lr,
                  DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
        tx = labelRight + Sc(8, d);

        // Configured-zone count, so "have I set anything up on that screen?"
        // is answerable without clicking through.
        RECT br = {tx, tr.top + h / 2 - Sc(9, d), tx + badge,
                   tr.top + h / 2 + Sc(9, d)};
        HBRUSH bb = CreateSolidBrush(active ? g_pal.accent : g_pal.field);
        FillRect(hdc, &br, bb);
        DeleteObject(bb);
        SelectObject(hdc, s->hFontSmall);
        SetTextColor(hdc, active ? g_pal.accentText : g_pal.dim);
        DrawTextW(hdc, count, -1, &br, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

        if (active)
        {
            RECT ul = {tr.left + Sc(8, d), tr.bottom - Sc(2, d),
                       tr.right - Sc(8, d), tr.bottom};
            HBRUSH ab = CreateSolidBrush(g_pal.accent);
            FillRect(hdc, &ul, ab);
            DeleteObject(ab);
        }

        x += wTab;
    }

    // Hairline under the whole strip.
    RECT sep = {Sc(Lay::Pad, d), top + h, client.right - Sc(Lay::Pad, d),
                top + h + Sc(1, d)};
    HBRUSH sb = CreateSolidBrush(g_pal.border);
    FillRect(hdc, &sep, sb);
    DeleteObject(sb);

    SelectObject(hdc, old);
}

static void DashPaintDiagram(DashState *s, HDC hdc)
{
    UINT d = s->dpi;
    RECT dg = DashDiagramRect(s);

    HFONT old = (HFONT)SelectObject(hdc, s->hFont);
    SetBkMode(hdc, TRANSPARENT);

    HBRUSH screen = CreateSolidBrush(g_pal.field);
    FillRect(hdc, &dg, screen);
    DeleteObject(screen);

    HPEN pen = CreatePen(PS_SOLID, Sc(1, d), g_pal.border);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH hollow = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hollow);
    Rectangle(hdc, dg.left, dg.top, dg.right, dg.bottom);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    if (s->displays.empty())
    {
        SetTextColor(hdc, g_pal.dim);
        RECT t = dg;
        DrawTextW(hdc,
                  L"No displays are configured.\n\n"
                  L"Open this mod's Settings page in Windhawk, add a display, "
                  L"then add the zones you want on it.",
                  -1, &t, DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL);
        SelectObject(hdc, old);
        return;
    }

    const DisplayView &dv = s->displays[s->activeTab];

    HBRUSH set = CreateSolidBrush(g_pal.accent);
    HBRUSH unset = CreateSolidBrush(g_pal.border);
    HBRUSH hot = CreateSolidBrush(g_lightTheme ? RGB(0, 70, 140)
                                               : RGB(120, 215, 255));
    // A zone that is configured but cannot fire needs to look wrong at a
    // glance, not merely be explained in the panel below.
    HBRUSH broken = CreateSolidBrush(g_lightTheme ? RGB(176, 60, 40)
                                                  : RGB(220, 110, 90));

    for (int z = 0; z < ZONE_COUNT; z++)
    {
        const ZoneView &zv = dv.zones[z];
        bool on = zv.action != CornerAction::Nothing;
        bool live = (z == s->hoverZone || z == s->selZone);
        HBRUSH b = live ? hot : (zv.invalid ? broken : (on ? set : unset));

        RECT widest = ZoneRectInDiagram((Zone)z, dg);
        if (widest.right <= widest.left || widest.bottom <= widest.top)
            continue;
        FillRect(hdc, &widest, b);

        if (!on)
            continue;

        SetTextColor(hdc, live ? (g_lightTheme ? RGB(255, 255, 255)
                                               : RGB(0, 0, 0))
                               : g_pal.accentText);

        const wchar_t *name = ActionToString(zv.action);

        if (ZoneIsVertical((Zone)z))
        {
            // A rotated font draws from a baseline rather than into a rect, so
            // the text is measured and placed by hand. DT_* alignment does not
            // apply to escapement.
            SelectObject(hdc, s->hFontVert);
            SIZE ts = {};
            GetTextExtentPoint32W(hdc, name, (int)wcslen(name), &ts);
            int bh = widest.bottom - widest.top;
            int cx = (widest.left + widest.right) / 2;
            int cy = (widest.top + widest.bottom) / 2;
            // Clip to the strip so a long name cannot escape the screen box.
            IntersectClipRect(hdc, widest.left, widest.top, widest.right,
                              widest.bottom);
            // At 90 degrees the advance runs up the screen and the glyphs hang
            // to the left of the baseline, so the baseline sits at the right of
            // the band and the run starts at its bottom.
            int y = cy + (ts.cx > bh ? bh / 2 : ts.cx / 2);
            TextOutW(hdc, cx + ts.cy / 2, y, name, (int)wcslen(name));
            SelectClipRgn(hdc, nullptr);
        }
        else
        {
            SelectObject(hdc, s->hFontSmall);
            RECT t = widest;
            InflateRect(&t, -Sc(3, d), -Sc(2, d));
            // Corners are square and small, so a long name wraps and then
            // clips; edges are wide and short, so they ellipsise on one line.
            DrawTextW(hdc, name, -1, &t,
                      ZoneIsCorner((Zone)z)
                          ? (DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL)
                          : (DT_CENTER | DT_SINGLELINE | DT_VCENTER |
                             DT_END_ELLIPSIS));
        }
    }

    DeleteObject(set);
    DeleteObject(unset);
    DeleteObject(hot);
    DeleteObject(broken);

    // Caption under the box.
    SelectObject(hdc, s->hFontSmall);
    SetTextColor(hdc, g_pal.dim);
    wchar_t cap[160];
    if (dv.wildcard)
        wcscpy_s(cap, _countof(cap),
                       L"Applies to any display without its own configuration");
    else if (!dv.present)
        _snwprintf_s(cap, _countof(cap), _TRUNCATE, L"%s  -  not connected",
                     dv.id.c_str());
    else
        _snwprintf_s(cap, _countof(cap), _TRUNCATE, L"%d x %d%s", dv.width,
                     dv.height, dv.primary ? L"  -  primary" : L"");

    RECT cr = {DashDiagramArea(d).left, dg.bottom + Sc(4, d),
               DashDiagramArea(d).right, dg.bottom + Sc(Lay::DiagCapH, d)};
    DrawTextW(hdc, cap, -1, &cr, DT_CENTER | DT_SINGLELINE);

    SelectObject(hdc, old);
}

static void DashPaintDetail(DashState *s, HDC hdc, const RECT &client)
{
    UINT d = s->dpi;
    int left = Sc(Lay::Pad, d);
    int right = client.right - Sc(Lay::Pad, d);
    int top = Sc(Lay::Pad + Lay::TabH + Lay::Gap + Lay::DiagAreaH +
                     Lay::DiagCapH + Lay::Gap,
                 d);

    HFONT old = (HFONT)SelectObject(hdc, s->hFont);
    SetBkMode(hdc, TRANSPARENT);

    RECT sep = {left, top, right, top + Sc(1, d)};
    HBRUSH sb = CreateSolidBrush(g_pal.border);
    FillRect(hdc, &sep, sb);
    DeleteObject(sb);

    int y = top + Sc(10, d);
    // Hover wins while the pointer is over a zone; a click pins one so the
    // panel still reads once the pointer moves away.
    int zone = (s->hoverZone >= 0) ? s->hoverZone : s->selZone;

    if (s->displays.empty() || zone < 0)
    {
        SelectObject(hdc, s->hFontSmall);
        SetTextColor(hdc, g_pal.dim);
        RECT r = {left, y, right, y + Sc(40, d)};
        DrawTextW(hdc,
                  s->displays.empty()
                      ? L"Nothing to show yet."
                      : L"Hover or click a zone to see its action and the "
                        L"settings actually in effect for it.",
                  -1, &r, DT_LEFT | DT_WORDBREAK);
        SelectObject(hdc, old);
        return;
    }

    const DisplayView &dv = s->displays[s->activeTab];
    const ZoneView &zv = dv.zones[zone];

    SelectObject(hdc, s->hFontBold);
    SetTextColor(hdc, g_pal.text);
    RECT r = {left, y, right, y + Sc(18, d)};
    DrawTextW(hdc, ZoneToString((Zone)zone), -1, &r, DT_LEFT | DT_SINGLELINE);
    y += Sc(19, d);

    SelectObject(hdc, s->hFontSmall);
    SetTextColor(hdc, g_pal.dim);
    wchar_t line[512];
    if (zv.action == CornerAction::Nothing)
    {
        wcscpy_s(line, _countof(line),
                       L"Not set - this zone does nothing.");
    }
    else if (zv.action == CornerAction::AlternateKeypress ||
             zv.action == CornerAction::AlternateCommand)
    {
        std::wstring a, b;
        if (SplitAlternate(zv.args, a, b))
            _snwprintf_s(line, _countof(line), _TRUNCATE,
                         L"%s - alternates between  %s  and  %s",
                         ActionToString(zv.action), a.c_str(), b.c_str());
        else
            _snwprintf_s(line, _countof(line), _TRUNCATE,
                         L"%s - needs two halves separated by |",
                         ActionToString(zv.action));
    }
    else if (!zv.args.empty())
    {
        _snwprintf_s(line, _countof(line), _TRUNCATE, L"%s  -  %s",
                     ActionToString(zv.action), zv.args.c_str());
    }
    else
    {
        wcscpy_s(line, _countof(line), ActionToString(zv.action));
    }
    r = {left, y, right, y + Sc(18, d)};
    DrawTextW(hdc, line, -1, &r, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
    y += Sc(18, d);

    if (zv.invalid)
    {
        SetTextColor(hdc, g_lightTheme ? RGB(176, 60, 40) : RGB(220, 110, 90));
        r = {left, y, right, y + Sc(16, d)};
        DrawTextW(hdc,
                  L"This zone will not fire - its arguments could not be read.",
                  -1, &r, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
        SetTextColor(hdc, g_pal.dim);
    }
    else if (zv.fromWildcard)
    {
        r = {left, y, right, y + Sc(16, d)};
        DrawTextW(hdc, L"Inherited from the \"All displays\" configuration", -1,
                  &r, DT_LEFT | DT_SINGLELINE);
    }
    y += Sc(19, d);

    if (zv.action == CornerAction::Nothing)
    {
        SelectObject(hdc, old);
        return;
    }

    // Column headings.
    int cLabel = left;
    int cValue = left + Sc(190, d);
    int cFrom = left + Sc(300, d);

    SetTextColor(hdc, g_pal.dim);
    r = {cLabel, y, cValue, y + Sc(16, d)};
    DrawTextW(hdc, L"SETTING", -1, &r, DT_LEFT | DT_SINGLELINE);
    r = {cValue, y, cFrom, y + Sc(16, d)};
    DrawTextW(hdc, L"IN EFFECT", -1, &r, DT_LEFT | DT_SINGLELINE);
    r = {cFrom, y, right, y + Sc(16, d)};
    DrawTextW(hdc, L"FROM", -1, &r, DT_LEFT | DT_SINGLELINE);
    y += Sc(Lay::DetHdrH, d);

    // A zone silently inherits five numbers. Showing which ones were
    // overridden is the whole point of the panel - "why did this one fire
    // late?" was previously unanswerable without reading the store by hand.
    for (int i = 0; i < 6; i++)
    {
        int own = TuningField(zv.tuning, i);
        bool overridden = own >= 0;
        int value = overridden ? own : GlobalField(s->globals, i, (Zone)zone);
        std::wstring text = FormatTuning(i, value);

        SetTextColor(hdc, g_pal.dim);
        r = {cLabel, y, cValue, y + Sc(Lay::DetRowH, d)};
        DrawTextW(hdc, kTuningNames[i], -1, &r, DT_LEFT | DT_SINGLELINE);

        SetTextColor(hdc, g_pal.text);
        r = {cValue, y, cFrom, y + Sc(Lay::DetRowH, d)};
        DrawTextW(hdc, text.c_str(), -1, &r, DT_LEFT | DT_SINGLELINE);

        SetTextColor(hdc, overridden ? g_pal.accent : g_pal.dim);
        r = {cFrom, y, right, y + Sc(Lay::DetRowH, d)};
        DrawTextW(hdc, overridden ? L"this zone" : L"global", -1, &r,
                  DT_LEFT | DT_SINGLELINE);

        y += Sc(Lay::DetRowH, d);
    }

    SelectObject(hdc, old);
}

// =====================================================================
// Hit testing
// =====================================================================

static int DashHitZone(DashState *s, POINT pt)
{
    if (s->displays.empty())
        return -1;
    RECT dg = DashDiagramRect(s);
    for (int z = 0; z < ZONE_COUNT; z++)
    {
        RECT r = ZoneRectInDiagram((Zone)z, dg);
        if (PtInRect(&r, pt))
            return z;
    }
    return -1;
}

static int DashHitTab(DashState *s, POINT pt)
{
    for (int i = 0; i < (int)s->tabRects.size(); i++)
        if (PtInRect(&s->tabRects[i], pt))
            return i;
    return -1;
}

// Posted when the configuration changes underneath an open window.
#define WM_APP_DASH_REFRESH (WM_APP + 21)

static void DashMakeFonts(DashState *s)
{
    if (s->hFont)
        DeleteObject(s->hFont);
    if (s->hFontBold)
        DeleteObject(s->hFontBold);
    if (s->hFontSmall)
        DeleteObject(s->hFontSmall);
    if (s->hFontVert)
        DeleteObject(s->hFontVert);

    s->hFont = DashMakeFont(s->dpi, 12, false, 0);
    s->hFontBold = DashMakeFont(s->dpi, 12, true, 0);
    s->hFontSmall = DashMakeFont(s->dpi, 11, false, 0);
    // 900 tenths of a degree: bottom-to-top, for the left and right strips.
    s->hFontVert = DashMakeFont(s->dpi, 11, false, 900);
}

static LRESULT CALLBACK DashWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam)
{
    DashState *s = (DashState *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

    switch (uMsg)
    {
    case WM_NCCREATE:
    {
        auto *cs = (CREATESTRUCTW *)lParam;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    case WM_CREATE:
    {
        s = (DashState *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

        UINT dpi = 96;
        {
            HMODULE u = GetModuleHandleW(L"user32.dll");
            using Fn = UINT(WINAPI *)(HWND);
            if (auto fn = (Fn)GetProcAddress(u, "GetDpiForWindow"))
                if (UINT v = fn(hWnd))
                    dpi = v;
        }
        s->dpi = dpi;

        DashMakeFonts(s);
        s->hBg = CreateSolidBrush(g_pal.bg);

        s->hClose = CreateWindowExW(
            0, L"BUTTON", L"Close",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 0, 0,
            hWnd, (HMENU)(INT_PTR)IDC_CLOSE, GetModuleHandle(nullptr), nullptr);
        SendMessageW(s->hClose, WM_SETFONT, (WPARAM)s->hFont, TRUE);
        ApplyControlTheme(s->hClose, L"BUTTON");

        // Title bar and Alt-Tab. WM_SETICON does not take ownership, so both
        // handles are kept on the state and destroyed with the window.
        s->hIcon = MakeHotCornerIcon(GetSystemMetrics(SM_CXICON), true);
        s->hIconSm = MakeHotCornerIcon(GetSystemMetrics(SM_CXSMICON), true);
        if (s->hIcon)
            SendMessageW(hWnd, WM_SETICON, ICON_BIG, (LPARAM)s->hIcon);
        if (s->hIconSm)
            SendMessageW(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)s->hIconSm);

        ApplyModernFrame(hWnd);
        DashBuildSnapshot(s);
        return 0;
    }

    case WM_APP_DASH_REFRESH:
        if (s)
        {
            DashBuildSnapshot(s);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;

    case WM_ACTIVATE:
        // Rebuilt whenever the window comes forward, so editing in Windhawk and
        // switching back shows the new configuration without a reopen.
        if (s && LOWORD(wParam) != WA_INACTIVE)
        {
            DashBuildSnapshot(s);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        break;   // DefWindowProc still has focus work to do

    case WM_SIZE:
        if (s && s->hClose)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            UINT d = s->dpi;
            int w = Sc(Lay::BtnW, d), h = Sc(Lay::BtnH, d);
            SetWindowPos(s->hClose, nullptr, rc.right - Sc(Lay::Pad, d) - w,
                         rc.bottom - Sc(Lay::Pad, d) - h, w, h, SWP_NOZORDER);
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;   // WM_PAINT paints every pixel, into a back buffer

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);

        // Double-buffered: the diagram is redrawn on every mouse move, and
        // painting it straight to the window flickers badly.
        HDC mem = CreateCompatibleDC(hdc);
        HBITMAP bmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(mem, bmp);

        FillRect(mem, &rc, s->hBg);
        DashPaintTabs(s, mem, rc);
        DashPaintDiagram(s, mem);
        DashPaintDetail(s, mem, rc);

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);

        SelectObject(mem, oldBmp);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        int z = DashHitZone(s, pt);
        if (z != s->hoverZone)
        {
            s->hoverZone = z;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
        TrackMouseEvent(&tme);
        return 0;
    }

    case WM_MOUSELEAVE:
        if (s->hoverZone != -1)
        {
            s->hoverZone = -1;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        return 0;

    case WM_LBUTTONDOWN:
    {
        POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        int tab = DashHitTab(s, pt);
        if (tab >= 0)
        {
            if (tab != s->activeTab)
            {
                s->activeTab = tab;
                s->selZone = -1;
                s->hoverZone = -1;
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }
        int z = DashHitZone(s, pt);
        s->selZone = (z == s->selZone) ? -1 : z;
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;
    }

    case WM_KEYDOWN:
        // Left/right walk the tab strip, which is what a tab strip should do.
        if ((wParam == VK_LEFT || wParam == VK_RIGHT) && !s->displays.empty())
        {
            int n = (int)s->displays.size();
            s->activeTab =
                (s->activeTab + (wParam == VK_RIGHT ? 1 : n - 1)) % n;
            s->selZone = -1;
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;
        }
        if (wParam == VK_ESCAPE)
        {
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_CTLCOLORBTN:
    case WM_CTLCOLORSTATIC:
        SetTextColor((HDC)wParam, g_pal.text);
        SetBkColor((HDC)wParam, g_pal.bg);
        return (LRESULT)s->hBg;

    case WM_COMMAND:
        // IDCANCEL is what IsDialogMessage turns Escape into.
        if (LOWORD(wParam) == IDC_CLOSE || LOWORD(wParam) == IDCANCEL)
        {
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_DPICHANGED:
    {
        s->dpi = HIWORD(wParam);
        DashMakeFonts(s);
        SendMessageW(s->hClose, WM_SETFONT, (WPARAM)s->hFont, TRUE);
        RECT *nr = (RECT *)lParam;
        SetWindowPos(hWnd, nullptr, nr->left, nr->top, nr->right - nr->left,
                     nr->bottom - nr->top, SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;
    }

    case WM_SETTINGCHANGE:
        // The user switched between light and dark while the window was open.
        if (lParam && wcscmp((const wchar_t *)lParam, L"ImmersiveColorSet") == 0)
        {
            BuildPalette();
            if (s->hBg)
                DeleteObject(s->hBg);
            s->hBg = CreateSolidBrush(g_pal.bg);
            ApplyModernFrame(hWnd);
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        return 0;

    case WM_DESTROY:
        if (s)
        {
            if (s->hFont)
                DeleteObject(s->hFont);
            if (s->hFontBold)
                DeleteObject(s->hFontBold);
            if (s->hFontSmall)
                DeleteObject(s->hFontSmall);
            if (s->hFontVert)
                DeleteObject(s->hFontVert);
            if (s->hBg)
                DeleteObject(s->hBg);
            if (s->hIcon)
                DestroyIcon(s->hIcon);
            if (s->hIconSm)
                DestroyIcon(s->hIconSm);
            s->hFont = s->hFontBold = s->hFontSmall = s->hFontVert = nullptr;
            s->hBg = nullptr;
            s->hIcon = s->hIconSm = nullptr;
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

    HWND hWnd = CreateWindowExW(0, kClass,
                                L"Win-X Hot Corners — Zones & settings", style,
                                x, y, w, h, nullptr, nullptr, hInst, &state);

    if (!hWnd)
    {
        UnregisterClassW(kClass, hInst);
        return 1;
    }

    AllowDarkModeForControl(hWnd, !g_lightTheme);

    // The frame above was sized from the system DPI, but the layout inside is
    // computed from the window's own. They agree on the usual setup and can
    // disagree if the primary display's scaling changed without a sign-out, so
    // resize to whatever the window actually landed on.
    {
        HMODULE u = GetModuleHandleW(L"user32.dll");
        using Fn = UINT(WINAPI *)(HWND);
        UINT real = dpi;
        if (auto fn = (Fn)GetProcAddress(u, "GetDpiForWindow"))
            if (UINT v = fn(hWnd))
                real = v;
        if (real != dpi)
        {
            RECT want = {0, 0, Sc(Lay::ClientW, real), Sc(Lay::ClientH, real)};
            AdjustWindowRectEx(&want, style, FALSE, 0);
            SetWindowPos(hWnd, nullptr, 0, 0, want.right - want.left,
                         want.bottom - want.top,
                         SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
        }
    }

    g_hDashWnd = hWnd;
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    // Focus the window itself rather than the Close button, so the arrow keys
    // reach WM_KEYDOWN and can walk the tab strip. Tab still moves to Close.
    SetFocus(hWnd);

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
        //
        // Waited on briefly rather than abandoned: the window has just been
        // closed and the thread is already unwinding, so this returns almost
        // at once. Returning immediately meant that clicking the tray icon
        // right after closing the dashboard silently did nothing.
        if (WaitForSingleObject(g_hDashThread, 500) != WAIT_OBJECT_0)
        {
            Wh_Log(L"The previous dashboard window is still closing; try again "
                   L"in a moment.");
            return;
        }
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
            // Toggle rather than open the dashboard. The icon already shows
            // enabled state, so the feedback is immediate, and the dashboard
            // is the menu's default item - which leaves the one-click action
            // for the thing you actually do repeatedly.
            HandleTrayCommand(IDM_ENABLED);
        }
        return 0;
    }
    if (uMsg == WM_COMMAND)
    {
        HandleTrayCommand(LOWORD(wParam));
        return 0;
    }
    // The suspension has run out; repaint the icon so it stops saying "paused".
    if (uMsg == WM_TIMER && wParam == kSuspendTimerId)
    {
        KillTimer(hWnd, kSuspendTimerId);
        UpdateTrayIcon(false);
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

    CancelSuspendTimer();
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
    Wh_Log(L"Zones and timings live on this mod's Settings page. Right-click "
           L"its tray icon (next to the clock) and choose \"Zones & "
           L"settings...\" to see what they add up to.");

    InitializeCriticalSection(&g_settingsLock);
    InitializeCriticalSection(&g_reloadLock);
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
    // An open dashboard is showing the configuration that just changed.
    if (g_hDashWnd)
        PostMessage(g_hDashWnd, WM_APP_DASH_REFRESH, 0, 0);
}

void WhTool_ModUninit()
{
    // Declared before the first wait, and every wait feeds it. The tray wait
    // used to sit above this and only log its timeout - so a tray thread still
    // blocked on g_reloadLock, or still inside its menu, did not stop the
    // critical sections below from being deleted out from under it.
    bool allStopped = true;

    // Only a clean exit counts. WAIT_FAILED means the wait itself broke, which
    // says nothing about whether the thread is finished, so treat it the same
    // as a timeout rather than as success.
    auto waitFor = [&allStopped](HANDLE h, const wchar_t *what)
    {
        if (WaitForSingleObject(h, 3000) != WAIT_OBJECT_0)
        {
            Wh_Log(L"%s did not exit cleanly", what);
            allStopped = false;
        }
    };

    if (g_hStopEvent)
        SetEvent(g_hStopEvent);

    if (g_dwTrayThreadId)
        PostThreadMessage(g_dwTrayThreadId, WM_QUIT, 0, 0);

    if (g_hTrayThread)
    {
        waitFor(g_hTrayThread, L"Tray thread");
        CloseHandle(g_hTrayThread);
        g_hTrayThread = nullptr;
    }

    if (g_dwDetectThreadId)
        PostThreadMessage(g_dwDetectThreadId, WM_QUIT, 0, 0);

    // The dashboard is a window with its own message loop on its own thread,
    // and it takes g_settingsLock and g_zonesLock. Nothing below may free
    // those while it is alive. Its loop only ends when its window does, so
    // close the window rather than signalling the stop event.
    if (g_hDashWnd)
        PostMessage(g_hDashWnd, WM_CLOSE, 0, 0);

    if (g_hDashThread)
    {
        waitFor(g_hDashThread, L"Dashboard thread");
        CloseHandle(g_hDashThread);
        g_hDashThread = nullptr;
    }

    if (g_hDetectThread)
    {
        waitFor(g_hDetectThread, L"Detection thread");
        CloseHandle(g_hDetectThread);
        g_hDetectThread = nullptr;
    }

    if (g_hWorkerThread)
    {
        // Can legitimately still be inside ShellExecuteEx (a UAC prompt keeps
        // it there indefinitely).
        waitFor(g_hWorkerThread, L"Worker thread");
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
    DeleteCriticalSection(&g_reloadLock);
    DeleteCriticalSection(&g_settingsLock);

    Wh_Log(L"Uninit done");
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

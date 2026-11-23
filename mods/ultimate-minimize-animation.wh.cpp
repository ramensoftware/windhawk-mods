// ==WindhawkMod==
// @id              ultimate-minimize-animation
// @name            Ultimate Minimize Animation
// @description     The macOS genie and 16 more effects (Scale, Fluid, Black Hole, Glass, Thanos Snap, Pixel Melt...) for minimize, restore, launch and close, with mid-flight reversal, the window's real shadow and an Alt+Tab animation
// @version         4.0.0
// @author          NairoDorian
// @github          https://github.com/NairoDorian
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -lmsimg32 -lole32 -loleaut32 -luuid -lshell32 -lwinmm
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Ultimate Minimize Animation

The "ultimate" macOS-style window animation mod: seventeen effects that every
animation can use, taskbar-position awareness (bottom **and** top taskbars,
including the "Taskbar on top" Windhawk mod), multi-monitor support, a spring
bounce on restore, app launch animations, auto-hide taskbar unhiding, close
animations for normal **and** special windows, and an Alt+Tab switch animation.

Definitely not inspired from MacOS.

## 🚀 What's New in V4 (Precision Edition)

V4 is built on V3.9 and keeps everything it fixed, with the same setting keys.

**Change your mind mid-animation.** Restore a window while it is still
minimizing (or minimize it while it is still restoring) and the running animation
turns around from where it is, like macOS. V3.9 popped the window back in at once
while the minimize ghost kept falling over it. Every effect reverses, and any
number of quick toggles land on the state you asked for last.

**One Genie.** The two genies of V3 (Modern and Classic) looked alike because they
are two ways of drawing the same shape. V4 has one Genie, drawn by the analytic
scanline renderer (derrunk's True Genie, the faster of the two: 0.5-0.9 ms a frame
where the Direct2D Modern engine needed 6.5-7.5 ms and ran at half rate on a
240 Hz screen). New **Genie bend phase** setting; the tip width setting now
applies to it. Mipmap levels are blended (trilinear), so there is no visible band
where the shrinking window switches to a smaller level. The old `genie_modern` /
`genie_classic` values both select the Genie.

**Clean start, even motion.**
- The animation clock starts on the frame that is on screen. In V3.9 the first
  frame waited 2-4 refreshes for the compositor while time already ran, so the
  first visible step of every animation was 2-4x bigger than the others.
- Frames stay on a vblank grid at the monitor's exact refresh rate (240.000 /
  165.000 / 59.94 Hz from the display configuration, not a rounded integer); a
  late frame snaps back to the grid instead of losing it.
- When a frame cannot fit one refresh (a huge window at 240 Hz), the animation
  steps every 2 refreshes - an even 120 fps - instead of alternating 1-2-1-2.

**Task Manager animates whole.** Only its left menu used to animate; the rest
vanished. Windows' DPI conversion returned a wrong corner for Task Manager (a
system-DPI-aware window hosting modern XAML panels), and the snapshot covered the
left 42 % of the window. V4 maps the frame from the window's own rectangle and
checks the result on all four sides; the taskbar target of such apps uses the same
mapping.

**Nothing pops.**
- **The window keeps its shadow** while it animates (when Windows draws window
  shadows): it follows the genie row by row, moves with Swell / Pop / Scale and
  fades with the other effects. The shape was measured on Windows 11, not guessed.
  New setting: *Window shadow during animations*.
- **Mica / acrylic windows** (Notepad, Task Manager...) are captured as they are on
  screen instead of see-through or black.
- A capture missing a fifth of the window (content the capture API cannot read)
  is replaced by a screen grab instead of vanishing without an animation.
- The restore bounce is one spring model that ends exactly at rest.

**Four new effects** (in every effect selector):
- **Scale** - macOS's other minimize effect: the window shrinks straight into its
  taskbar button.
- **Fluid** - the window turns into a droplet that pours into the taskbar button
  (after kivsak's Fluid Window Engine).
- **Glass** - the window breaks into irregular shards that spin and fall.
- **Pixel Melt** - the window drips down like wet paint (after ReDrag's Windows
  Animations).
- **Cyber Glitch** now splits its colour channels on torn rows.

**Smaller**
- A labelled taskbar button ("never combine") receives the genie on its icon, not
  in the middle of its label.
- Special windows: never below 8 px (a 1 x 1 helper window of Windhawk's used to
  "animate"); popups without a redirection surface (the taskbar's hover tooltips)
  are captured from the screen at once, and tiny popups hold their thread for at
  most 8 ms instead of 34 ms.
- The mod's own ghost windows skip the close path when they are destroyed (V3.9
  ran every one through it, which filled the close log).
- Telemetry shows where the click time goes (capture, setup, frame 0, the
  compositor's flush), the first gap apart from the steady-state frames, the
  cadence and the number of reversals; a launch is labelled `launch`.
- Removed with the Direct2D engine: the *Modern genie tile count* and *Modern
  genie renderer* settings, and every use of Direct2D, Direct3D and
  DirectComposition (the presentation cost measured 0.1-0.7 ms a frame: rendering
  is what costs, and that is on the CPU).

## 🩹 What's New in V3.9 (Fixes Edition)

Same effects and the same settings as V3.8: this release fixes what a full review
of the V3.8 code found, and brings in the robustness work of the macOS Minimize
Animation fork 3.2.1.

**Effects that were cut short or clipped**
- **Long close effects were cut off.** The closing window is held while its effect
  plays, but the hold lasted the *Close duration* setting (+ 750 ms), while Thanos
  Snap, Square Shatter, Perlin Dissolve, Ink Splash, Glitch, Mirage, TV Off, Poof
  and Swell play 1.2-2.6x longer. With a longer close duration (Thanos at 500 ms
  and up) an application's last window was destroyed mid-effect and the effect
  vanished with the process. The hold now follows the effect's real length.
- **Square Shatter shards stopped at an invisible wall.** The canvas around the
  window was smaller than the farthest a shard flies (its sideways spread was not
  counted), so shards were cut at a rectangle mid-screen while still half visible.
  Thanos Snap dust drifting up, and aimed Thanos / Shatter streams passing beside
  the taskbar button, had the same problem on a smaller scale.
- **Perlin Dissolve** dropped whole blocks at a monitor edge on its first frame
  (a window partly off screen); the blocks are now cut at the edge instead.

**Close and Alt+Tab**
- **Borderless windows you close yourself get the close effect again.** V3.8 sent
  every caption-less, frameless window without a system menu to the special-window
  path (meant for splash screens) - including borderless games and launchers closed
  with Alt+F4 or from the taskbar. That path never holds the application, so when it
  was the last window the effect died with the process. A window that receives a
  close request (X, Alt+F4, the taskbar's Close) now always takes the close path; a
  splash screen that simply vanishes still plays the special effect.
- **No Alt+Tab animation after Alt+F4.** Closing a hide-to-tray application with
  Alt+F4 changed the foreground while Alt was still down, which looked like an
  Alt+Tab and played the switch animation on the next window. A session now only
  starts from a window that is still on screen and not closing.
- **Alt+Tab to a minimized window** could, in a narrow race, play the switch fade
  instead of the restore effect. A window the mod is already hiding for an animation
  is skipped.

**Robustness (from the macOS Minimize Animation fork 3.2.1)**
- A window larger than 64 megapixels, or one parked entirely off screen, plays the
  system animation instead of asking for a multi-hundred-MB snapshot.
- Every animation canvas is clipped to the desktop: a window partly off screen no
  longer allocates (and uploads) the invisible part.
- Out of memory while setting up an effect ends that animation cleanly instead of
  raising an exception inside the application.
- The DirectComposition presenter (Modern genie, GPU option) no longer releases the
  shared GPU device under a second animation when the device is lost.

**Smaller**
- Destroying child controls costs less: the special-window class exclusion list is
  only consulted when it has entries.
- Telemetry labels special-window animations as `special`.

## 🎆 What's New in V3.8 (Effects Edition)

**One effect catalog, every selector.** Each of the three effect selectors -
**Minimize / restore / launch**, **Close** and **Special windows** - lists every
effect, plus **Random**:

| Effect | Look |
|---|---|
| Genie: Modern | the Direct2D mesh genie into the taskbar button |
| Genie: Classic | the scanline genie with bilinear AA and mipmaps |
| Black Hole | the spiral wormhole vortex |
| **Square Shatter** | the window bursts into square shards |
| **Thanos Snap** | a disintegration wave turns the window to dust |
| **Perlin Dissolve** | an acid-burn noise dissolve |
| **Cyber Glitch** | digital tearing and break-up, a teleport |
| **Retro TV Off** | a CRT power-down: squash, beam, dot |
| **Ink Splash** | an organic ink blot swallows the window |
| **Mirage** | a wavy heat-shimmer fade |
| **Swell** | expand and fade |
| **Pop** | a quick bubble shrink (made for small popups) |
| **Fade Up** | the Fluent flyout dismiss |
| **Poof** | a puff of dust drifting up |

- Restore and launch play the minimize effect **in reverse**: shards fly back
  together, dust reassembles, the TV turns back on. A window always restores with
  the effect it minimized with, even with Random on.
- **Genie as a close effect** funnels into the app's taskbar button; on a special
  window it funnels into the spot the popup opened from.
- In-place effects need no dock, so they also animate on **left / right
  taskbars**; a genie on a side taskbar falls back to Swell.
- With **Aim at the taskbar** on, Thanos Snap and Square Shatter fly into the
  taskbar button on minimize and out of it on restore.
- Particle effects take longer than the duration setting by design (about 2.4x
  for Shatter / Perlin, 2.6x for Thanos), so one duration feels right for every
  effect.

**Special windows are back - and wider than V3.5.** V1 to V3.5 played the close
animation on small popups, info bubbles and dropdowns; V3.6 dropped them. V3.8
brings them back as their own feature, with their own effect (default **Pop**)
and duration, and goes further: tool windows, tooltips, menus drawn by apps,
splash screens and **child windows** (a docked panel, an MDI document, a task
pane, a find bar) are included, at any size. A special window:

- animates when it is hidden *or* destroyed (Chromium and WPF reuse popups by
  hiding them), once, and a popup shown again at once cancels its ghost;
- never holds the application (V3.5 froze the popup's thread for the whole
  wormhole), and a burst is rate-limited (4 at once, 8 per second);
- keeps its place in the z-order: its ghost never paints over a window that was
  in front of it;
- replaces Windows' own fade for `AnimateWindow` popups (comctl32 tooltips)
  instead of stacking on top of it.

Child windows animate on an explicit close (`DestroyWindow`) only - hiding a
child (every tab switch) does not, unless **Also when child windows are hidden**
is on. Content hosts that browsers recreate during navigation, and File
Explorer's own child windows, never animate.

**Alt+Tab switch.** The window you switch to scales up from 94% and fades in
over 200 ms, drawn from a live DWM thumbnail (the content keeps moving).
Cancelling Alt+Tab and switching to a minimized window (the restore effect plays
instead) do not trigger it.

**Modern genie on DirectComposition (experimental, off by default).** On a high
refresh-rate display the Modern genie spent most of each frame copying the
Direct2D result back to memory and up again (`UpdateLayeredWindow`). With
**Modern genie renderer** set to a GPU option, the same Direct2D drawing goes to
a DirectComposition swap chain on a D3D11 device: no copies, vblank-locked
presents. D3D11 and DirectComposition are loaded only on first use; a cold
process uses the classic path for its first animation while the device warms
up; any failure falls back to the classic path.

**Smaller fixes**
- The close-path log (`close hwnd=...`) is off unless **Log close decisions** is
  on; a new **Log special-window decisions** shows why a window did or did not
  animate.
- Splash screens (no caption, no frame, no system menu) now take the special
  effect instead of the main close effect.

## 🎯 What's New in V3.7 (Precision & Performance Edition)

**V3.7.1:** new **Close animation duration (ms)** setting (50-2000, default 250).
The Black Hole close used to share the minimize / restore duration; it now has its
own, and the hold on the closing window follows it.

No new effects: the same three engines and the same settings, made faster, smoother,
sharper and more correct. Every raster change below was checked against the V3.6
code with a differential test (identical output, byte for byte, where the picture
is meant to be unchanged).

**Smoother**
- **Drift-free frame pacing.** V3.6 waited "one interval minus render time" from the
  start of every frame, so each timer wake-up's lateness was added to the period and
  the animation slowly walked out of phase with the display (measured: 58.5 fps on a
  60 Hz screen, 220 fps on 240 Hz, 172 ms of drift over 400 frames). Frames are now
  scheduled on absolute deadlines (frame *k* is due at *t0 + k x interval*); a frame
  that overruns re-bases instead of bursting. Measured drift: under 1 ms.
- The 1 ms system timer resolution is no longer held at all on any Windows that has
  the high-resolution timer (only the fallback pins it).
- Animation threads now service their message queue every frame, so a cross-thread
  `SendMessage` to a ghost (accessibility tools, shell broadcasts) no longer blocks
  its sender for the length of the animation; the click-time wait for the first
  frame does the same.

**Faster on the click**
- **Zero-copy snapshots.** V3.6 allocated a second bitmap and copied the whole
  snapshot into it under the global lock at every minimize (about 32 MB for a 4K
  window), and copied it back at every restore. A finished minimize now hands the
  bitmap itself to the restore cache and a restore adopts it: no copies, no second
  allocation.
- **The taskbar lookup overlaps the capture** instead of being added to it, and it
  is about twice as fast (below).
- Classic builds its mip pyramid **after** the first frame is on screen (falling
  animations never sample it in frame 0) and fills it **in parallel** (4K pyramid:
  8.2 ms -> 2.5 ms, and only 0.03 ms of it stays on the first-frame path); Black Hole
  no longer makes a full-size second copy of the snapshot before its first frame.

**Faster per frame**
- **Classic** hands rows to its workers in small chunks from a shared counter
  (work-stealing) instead of fixed bands, so a slow or busy core cannot stall the
  frame. The worker pool is **rationed across processes** with a named semaphore: a
  Win+M storm of many Classic genies no longer launches dozens of high-priority
  threads.
- **Black Hole** is multi-core: block destinations (the `sinf`/`cosf` per block) are
  computed in parallel, then blocks are binned into 64-row destination bands and
  blended per band in their original order - the output is identical to V3.6, and
  3-4x faster per frame (measured on 12 logical processors: 17.3 ms -> 4.3 ms at
  1080p, 18.8 ms -> 4.8 ms at 1440p).

**Sharper**
- **Modern: mip-filtered mesh.** A single bilinear brush shimmers when the window is
  squeezed far below its size. Modern now samples from a mip pyramid (the same one
  Classic has had since 3.3), picking the level per mesh row for that row's
  minification. An un-minified window is unchanged.
- **Real window corners.** DWM rounds the corners at composition; the captured
  surface is square. The snapshot now gets the same rounding (8 px, or 4 px for
  small-round windows, scaled to DPI; none when maximized, on Windows 10 or when the
  app opted out), 4x4 supersampled and premultiplied-safe, for all three engines.
  Modern's silhouette starts from that real radius instead of a fixed 12 px, so the
  first and last frame no longer pop and a maximized window no longer gets nibbled
  corners.
- **Black Hole restore bounce** is bilinear instead of nearest-neighbour
  (`StretchBlt` with `COLORONCOLOR`), so the pop no longer stair-steps text.

**Targeting**
- **Language-independent taskbar targeting.** On Windows 11 a taskbar button's
  UI Automation id is its application id, exe path or AppUserModelID. The lookup
  now asks Explorer only for real task buttons (class
  `Taskbar.TaskListButtonAutomationPeer`: Start, Search, Task View and every tray
  icon never leave Explorer) and matches the window's AppUserModelID / package id /
  image path against that id, with the localized-name scoring as a fallback. It no
  longer depends on English button names, and it can no longer pick a tray icon
  whose name happens to contain the app's name.
- **Searches only where the buttons are.** Explorer's UI Automation `FindAll` costs
  almost the same however few elements it returns, because it walks the XAML tree;
  from the taskbar root that includes the whole system tray. The task buttons are
  plain children of the `TaskbarFrame` element, so the lookup finds that element
  and searches only its children, with the name, id and rectangle of every button
  returned by one cached request. Measured on the live Windows 11 taskbar: 53-67 ms
  per cold lookup in V3.6, 24-31 ms now, with identical coordinates - well inside
  the 80 ms a first minimize may wait for it. (The old whole-tray search remains
  the fallback for shells without a `TaskbarFrame`.)
- **Stale-slot detection by layout.** A cached dock position stays trusted while the
  set of windows that own a taskbar button (and the taskbar alignment) is unchanged,
  and is refreshed in the background as soon as it changes. V3.6 simply re-resolved
  every two seconds.

**Correctness**
- **Black Hole could leave a window invisible.** On a close that is not held (for
  example closing from a taskbar thumbnail), the real window was cloaked and then
  also faded to alpha 0 from inside the render loop; if the close was refused
  ("Save changes?" -> Cancel), the window came back uncloaked but fully transparent.
  The redundant fade is gone.
- **DPI-unaware and system-aware applications.** DWM's extended frame is always in
  physical pixels while `GetWindowRect`, monitor rects and the ghost are in the
  process' virtualized space. V3.6 mixed them, shifting the snapshot and misplacing
  the animation for such apps on scaled monitors. They are now mapped into one space
  (and cross-checked), and per-monitor-aware processes (Explorer and every modern
  app) take an untouched fast path.
- **Launch animation hides the window with a DWM cloak** instead of
  `WS_EX_LAYERED` + alpha 0, which Tauri / Slint / winit and flip-model windows
  silently undid (the window flashed before the genie). Kept as the fallback for the
  first Firefox-family window (a cloak there can make Gecko open a second window),
  for windows the app cloaked itself, and for windows DWM will not cloak.
- **Closing Chrome / Edge / Electron** from the foreground now grabs the screen
  first: at close time `PrintWindow` can return only the title-bar band.
- **Screen-grab eligibility** compares visible frames instead of window rectangles
  (the invisible resize border made every snapped neighbour "overlap"), ignores
  minimized windows that stay `WS_VISIBLE`, and ignores Explorer's transparent XAML
  island host.
- Explorer's own surfaces (lock / power transitions, flyouts, its taskbar tab proxy
  for browser windows) no longer play a wormhole: inside `explorer.exe` only File
  Explorer windows count as application windows.
- `WM_SHOWWINDOW` counts as proof of life for the close guard (show paths that are
  neither `ShowWindow` nor `SetWindowPos`).

**Lighter in every process**
- **No Direct2D until it is used.** `d2d1.dll` is no longer linked or loaded at
  startup; the factory is created on the first Modern animation.
- Chromium / Electron helper processes (`--type=renderer|gpu-process|utility|...`)
  and session-0 services load nothing and hook nothing.
- Destroying a window no longer takes the global cache lock for child controls, or
  at all in a process that has never animated anything.

**Diagnostics.** New **Log performance telemetry** setting (off by default): one
line per animation with click -> snapshot time (and the time spent waiting for the
taskbar lookup), click -> first-frame latency, frame count, late frames, longest
frame period, and render / present cost. Nothing is logged per frame.

## 🔧 What's New in V3.6 (Correctness & Robustness Edition)

Same three engines, same look - a long list of real bugs fixed, plus the small
robustness pieces the other genie mods got right:

- **Classic genie keeps its alpha:** the scanline rasterizer now carries the
  premultiplied alpha channel through the mip pyramid, the bilinear sampler and
  the edge coverage. Windows 11 rounded corners no longer render as black
  wedges and translucent (Mica / acrylic) windows stay see-through - in
  Classic as well as Modern. A bilinear weight typo (`wy` for `wx` on the
  bottom row) that caused a faint shimmer is fixed too.
- **No more end-of-restore blink:** the real window is revealed *before* the
  ghost is destroyed (as every ancestor of this mod did), so there is never a
  composed frame with neither on screen.
- **Modern engine:** the dirty-clear rectangle no longer mixes canvas and
  screen coordinates (it used to clear most of the canvas every frame), the
  1 px bottom-row clip is gone, `dock_offset_y` is honored (it was silently
  overridden by the taskbar rect), a divide-by-zero in the lamp curve is
  guarded, a software Direct2D target is tried when the hardware one fails,
  and if Direct2D is unavailable altogether the animation falls back to the
  Classic engine instead of vanishing.
- **Black Hole:** the canvas gets a rotation margin so spiralling blocks are no
  longer clipped at the window edge, the ghost is non-activating like the other
  engines, and the bounce blit no longer runs a halftone filter over the alpha
  channel.
- **Every engine:** allocation failures (huge windows, GDI exhaustion) are
  checked and unwound instead of dereferencing NULL; only the dirty rectangle
  is handed to the compositor each frame (`UpdateLayeredWindowIndirect`); the
  genie ghost is slotted directly *below* the taskbar in z-order so the funnel
  tip flows under the icons rather than over them; the frame pacer reads the
  refresh rate of the monitor the animation plays on, not the primary's.
- **Capture:** screen `BitBlt` fallbacks are only used when nothing overlaps
  the window (and never for a cloaked window on the restore path, which used
  to capture the wallpaper); a capture that fails outright aborts the
  animation cleanly instead of playing an opaque black rectangle; terminal
  detection uses exact process names (`cmd`, `wt`, `conhost`...) instead of
  substrings; the window is repainted before an uncached restore capture.
- **Close path:** the closing thread is only held when the ghost would die
  with it (the window's own thread, last visible window of its process) -
  Explorer's taskbar no longer freezes for a close from a thumbnail; the hold
  pumps sent messages so nothing can deadlock against it; an application that
  closes asynchronously no longer pops back on screen for a beat after the
  wormhole; a refused close ("Save changes?" -> Cancel) is uncloaked as soon
  as its prompt appears; hidden windows with a stale guard re-arm.
- **Hide-to-tray stash:** the full-window `PrintWindow` that used to run on
  *every* hide of *every* top-level window (popup menus, tooltips, combo
  dropdowns, IME windows) is now gated on a real close candidate (root,
  unowned or captioned, not hung, same thread), throttled, bounded in count and
  bytes, and honors the exclusion list.
- **Targeting:** the per-window dock position expires and is refreshed in the
  background instead of being pinned forever (stale after a taskbar reflow);
  a UI Automation miss no longer poisons the cache with the fallback guess and
  is negatively cached for 10 s per process; a cursor resting on the tray
  during a keyboard minimize no longer becomes the permanent target; side
  taskbars are detected as such (they used to register as "top").
- **Hooks:** `DefWindowProcA` / `SendMessageA` are hooked so ANSI applications
  (Delphi, VB6, old MFC) animate too; `SW_FORCEMINIMIZE` and restore-to-
  maximized are recognised; `WM_NCDESTROY` cleans up caches for windows that
  never route `WM_DESTROY` through `DefWindowProc`; `AnimateWindow(AW_HIDE)`,
  `ShowWindowAsync(SW_HIDE)` and `SetWindowPlacement(SW_HIDE)` stash a
  snapshot like `ShowWindow(SW_HIDE)` does; hidden or already-iconic windows
  are never animated; excluded programs are rejected *before* the window is
  cloaked or the taskbar is focused, so they no longer blink or lose focus.
- **Auto-hide taskbar:** the taskbar is only revealed (and focus only borrowed)
  when it is actually hidden and the cursor is not already over it; focus is
  handed back only if the taskbar still owns it; the deferred minimize no
  longer blocks the worker in a synchronous `SendMessage`; the engines wait for
  the taskbar to slide in before reading its rect (top taskbars aimed off
  screen).
- **Lifetime:** the exclusion list matches `my.app.exe` (name was cut at the
  first dot), an empty process name is never cached, the ex-style restore
  only clears the bit the mod set, the snapshot cache is capped at 64 MB and
  keeps entries for 15 minutes instead of 60 seconds, the unload join budget
  covers a full animation, and the Direct2D factory is not released under a
  thread that failed to join.

## ⚡ What's New in V3.5 (Performance & Reliability Edition)

Same animations, same settings - substantially less work per frame:

- **Row-exact Modern mesh:** the genie mesh is generated from two curve
  evaluations per row instead of one per vertex, mesh bounds fall out of the
  same pass, and the Direct2D layer is bounded to the mesh instead of the whole
  canvas. Verified pixel-equivalent to sub-millipixel accuracy.
- **Instant capture:** the window snapshot path (the code that runs the moment
  you click minimize) was rewritten from per-pixel bounds tests plus up to three
  full-image passes into a single clipped pass.
- **Compositor-rate frame pacing:** animations render one frame per displayed
  frame instead of two, timed by a high-resolution timer at the true refresh
  rate.
- **Multi-core Classic genie:** the True Genie scanline rasterizer once again
  renders in parallel bands on a worker pool (derrunk's design) - measured 2.7x
  faster on 4K-class windows, from missing 60 Hz to comfortable headroom.
- **Deterministic Black Hole close:** the close is held until the wormhole
  finishes, guarded by a cross-process, self-expiring window-property stamp -
  one close, one animation, even for apps that close asynchronously.
- **Lighter footprint:** 1 ms system timer resolution is now held only while an
  animation is on screen (not for the mod's whole lifetime in every process),
  window caches validate owner PIDs and sweep dead handles, and a 64-to-32-bit
  truncation of every DefWindowProcW result was fixed.

## ✨ What's New in V3.3.x

- **🚀 App Launch Animation (Experimental):** Plays a rising genie when an application
  window first opens.
- **👁️ Auto-Hide Taskbar Unhide:** Briefly reveals an auto-hidden taskbar during minimize
  so the genie has a visible dock icon to flow into, then smoothly returns afterwards.
- **📐 Customizable Funnel Dock Width:** New setting `modern_funnel_width` allows
  customizing the exact pixel width of the genie neck entering the dock (8–120 px).
- **🎛️ Dynamic Mesh Quality Scaling:** `tile_count` directly scales Direct2D mesh
  tessellation and curvature resolution from 8 (ultra-fast) to 96 (ultra-fine cinema).
- **🌟 Authentic True Genie Scanline Engine (Classic):** Authentic Perlin smootherstep ($C^2$)
  continuous curve analytical rasterization with fixed-point 16.16 bilinear interpolation,
  subpixel edge coverage (`Premul`), and 4-level mip pyramids (`BuildMips`) for zero shimmering on 4K.
- **🍎 macOS Sequoia Dynamic Silhouette Physics:** Direct2D outline geometry
  dynamically scales its corner radius ($12\text{ px} \to 2\text{ px}$) as the window
  funnels into the dock, preventing blunt tip distortion and creating a seamless organic curve.
- **🌀 Isotropic 2D Spring Bounce:** Restore animations feature dynamic horizontal
  and vertical overshoot spring oscillation with expanded 2D dirty-rect clearance.
- **🖥️ Full Multi-Monitor & Top Taskbar Auto-Detection:** Automatically maps and
  animates into primary and secondary taskbars, including top-dock setups (with full
  support for "Taskbar on top for Windows 11").
- **🏎️ High-Resolution Frame Pacer:** QPC-timed waitable-timer pacer tuned for
  high-refresh rate displays (120Hz, 144Hz, 165Hz, 240Hz, 360Hz) and Variable Refresh Rate
  (G-Sync / FreeSync) displays.
- **💾 Memory-Efficient Snapshot Cache:** Automated aging sweeps and smart memory management
  cut background snapshot RAM consumption by up to 75% across multi-monitor setups.
- **🌊 Time-Gated Tail Dissolve:** Polynomial curve keeps the window solid while necking down,
  smoothly dissolving into the dock icon only at the final moment.
- **🛡️ Mid-Flight Restore & Window Visibility Guard:** Checks `IsIconic(hWnd)` before
  cloaking or minimizing, preventing windows from ever becoming stuck invisible if restored
  mid-animation.

## Effects (V4)

| Effect | Look |
|---|---|
| **Genie** | the macOS genie into the taskbar button (bend into a tunnel, slide through it) |
| **Scale** | the macOS scale effect: straight into the taskbar button |
| **Fluid** | a droplet pours into the taskbar button |
| **Black Hole** | the spiral wormhole vortex |
| **Glass** | irregular shards spin and fall |
| **Square Shatter** | the window bursts into square shards |
| **Thanos Snap** | a disintegration wave turns the window to dust |
| **Perlin Dissolve** | an acid-burn noise dissolve |
| **Pixel Melt** | the window drips down like wet paint |
| **Cyber Glitch** | digital tearing, colour split and break-up, a teleport |
| **Retro TV Off** | a CRT power-down: squash, beam, dot |
| **Ink Splash** | an organic ink blot swallows the window |
| **Mirage** | a wavy heat-shimmer fade |
| **Swell** | expand and fade |
| **Pop** | a quick bubble shrink (made for small popups) |
| **Fade Up** | the Fluent flyout dismiss |
| **Poof** | a puff of dust drifting up |

## Top taskbar support

The mod **auto-detects** whether the taskbar sits at the bottom or the top of
each monitor by reading the taskbar window rect - so it works out of the box
with the ["Taskbar on top for Windows 11"](https://windhawk.net/mods/taskbar-on-top-fork)
mod. The Genie, Scale and Fluid aim into a top taskbar exactly like they do a
bottom one. If auto-detection ever gets it wrong, override it with the
**Taskbar position** setting.

## Extra features

- **App launch** - play a genie when an application window first opens (experimental).
- **Unhide taskbar** - auto-reveals an auto-hidden taskbar during minimize.
- **Restore bounce** - restores finish with a springy overshoot pop instead
  of stopping dead (strength and duration configurable in the settings).
- **Black Hole close** - with **Animate window close** on, closing a window
  (X button / Alt+F4) swallows it into the wormhole instead of the instant
  vanish.
- Everything the base mod had: window-only premultiplied-alpha capture (Mica /
  acrylic windows are taken from the screen when they are uncovered), UI
  Automation taskbar-button targeting with per-window and per-process X/Y caches,
  first-frame sync, excluded programs, and safe unload.

## Known issues

- Left/right (side) taskbars: the Genie and Fluid fall back to Swell there, and
  Scale shrinks into the window's centre; every other effect plays where the
  window is.
- A window shadow is drawn only when Windows draws window shadows ("Show shadows
  under windows" in the performance options) and is matched to the Windows 11
  shadow of an active window; inactive windows have a slightly smaller one.
- Restoring a window while its minimize is still playing turns the animation
  around; this also works for a launch interrupted by a minimize. A minimize that
  reveals an auto-hidden taskbar is not reversible (its window is only minimized
  when the genie lands).
- UWP / WinUI frame windows (`ApplicationFrameWindow`) cannot be captured for a
  close animation (their content is not reachable through `PrintWindow`), so
  closing them plays the system animation.
- Taskbar-button targeting matches the window's application id / executable path
  against the button's id (V3.7); only when no id matches does it fall back to
  scoring the buttons' accessible names, which are localized, so a mis-targeted
  genie remains possible for such windows.
- The app-launch animation is experimental (see its setting description).
- Windows with `WS_EX_NOREDIRECTIONBITMAP` (WinUI / DirectComposition hosts) are still
  excluded from the launch animation.
- Some Firefox-based browsers (e.g. Zen) have been reported to loop the
  animation - add them to **Excluded programs** if that happens.

## Credits

- **Abdullah Masood** - the original MacOS Minimize Animation mod and the classic genie renderer.
- **Potassiumuncher** - the Direct2D genie engine of V3 (lamp curve, mesh warp, capture fixes);
  its look lives on in the V4 Genie.
- **kivsak** - the Black Hole wormhole, the spring bounce, the dock Y targeting ideas; in V4 the
  Fluid droplet and the Scale and Glass effect ideas (Fluid Window Engine Pro 2.0.1).
- **derrunk** - authentic True Genie Perlin smootherstep scanline engine, bilinear AA, and mipmaps:
  the V4 Genie, including its bend-phase setting.
- **ReDrag** - in V3.8 the Square Shatter, Thanos Snap, Perlin Dissolve, Cyber Glitch, Retro TV Off,
  Ink Splash, Mirage and Swell effects and the Alt+Tab switch design (Windows Animations 1.3.7, MIT);
  before that the async UI Automation timeout architecture, smart capture patterns, and stability hardening;
  in V3.7 also the absolute-deadline frame pacer, the DWM corner-radius rules and corner mask, the
  screen-grab eligibility rules, the helper-process early-out and the taskbar layout-signature idea
  (from Windows Animations 1.3.7); in V4 Pixel Melt and the design of mid-flight min / restore
  reversal (`RetargetLiveMinRestore`).
- **MrCrustyToast** and **Abdullah Masood** (macOS Minimize Animation fork 3.1.7 - 3.2.0) - the DWM-cloak
  launch hide with its Gecko / app-cloak fallbacks, the foreground close screen grab, and the
  `explorer.exe` shell-surface rule.
- **lolstijl** - the original genie mod this genre grew from.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- minimize:
  - minimize_animation: true
    $name: Animate window minimize
    $description: Play the effect when a window is minimized to the taskbar.
  - restore_animation: true
    $name: Animate window restore
    $description: Play the effect in reverse when a window is restored from the taskbar.
  - launch_animation: false
    $name: Animate app launch (experimental)
    $description: >-
      Play the restore effect when an application window first opens. Experimental:
      may briefly flash the window before animating, may not catch borderless apps,
      and can occasionally fire on dialog windows.
  - effect: genie
    $name: Minimize / restore / launch effect
    $description: >-
      Every effect can be used. Genie, Scale and Fluid fly into the taskbar button; the
      others play where the window is. Restore and launch play the effect in reverse,
      a window always restores with the effect it minimized with, and a restore clicked
      while the minimize is still playing turns the running animation around.
    $options:
    - genie: Genie - the macOS genie into the taskbar button
    - scale: Scale - the macOS scale effect into the taskbar button
    - fluid: Fluid - a droplet pours into the taskbar button
    - blackhole: Black Hole - spiral vortex
    - glass: Glass - shards spin and fall
    - shatter: Square Shatter - explosion
    - thanos: Thanos Snap - disintegration wave
    - perlin: Perlin Dissolve - acid burn
    - pixel_melt: Pixel Melt - dripping paint
    - glitch: Cyber Glitch - digital teleport
    - tv_off: Retro TV Off - CRT power-down
    - ink_splash: Ink Splash - organic blot
    - mirage: Mirage - wavy shimmer
    - swell: Swell - expand and fade
    - pop: Pop - quick bubble shrink
    - fade_up: Fade Up - Fluent dismiss
    - poof: Poof - puff of dust
    - random: Random - every effect once per cycle
  - duration_ms: 250
    $name: Duration (ms)
    $description: >-
      Base duration, 50-2000. Particle effects take longer by design (Shatter and
      Perlin about 2.4x, Thanos 2.6x, Glass 2.2x, Ink Splash and Pixel Melt 2x, Glitch
      and Mirage 1.6x, Fluid 1.4x).
  - aim_at_taskbar: true
    $name: Aim at the taskbar
    $description: >-
      Thanos Snap and Square Shatter fly into the taskbar button on minimize (and out
      of it on restore) instead of blowing away from the window.
  - genie_bend_pct: 40
    $name: Genie bend phase (%)
    $description: >-
      How much of the genie is spent bending the window into its tunnel before it
      slides through it, 15-70 (derrunk's True Genie). Lower = an earlier, faster
      slide; higher = a longer, more pronounced bend.
  - genie_tip_width: 8
    $name: Genie tip width (px)
    $description: Width of the window neck where it enters the taskbar button (8-120 px).
  - bounce_strength: 50
    $name: Restore bounce strength (0-100)
    $description: Springy overshoot at the end of a genie, Scale or Black Hole restore. 0 disables it.
  - bounce_duration_ms: 200
    $name: Restore bounce duration (ms)
    $description: How long the restore bounce lasts. Clamped to 0-1000.
  $name: Minimize, restore and launch
  $description: The effect played when windows minimize, restore and open.
- close:
  - close_animation: true
    $name: Animate window close
    $description: Play the effect when an application window closes (X button, Alt+F4, taskbar).
  - effect: blackhole
    $name: Close effect
    $description: >-
      Every effect can be used. A genie funnels into the app's taskbar button. The
      closing window is held until the effect ends only when it is the application's
      last window, so the effect is never cut short.
    $options:
    - genie: Genie - the macOS genie into the taskbar button
    - scale: Scale - the macOS scale effect into the taskbar button
    - fluid: Fluid - a droplet pours into the taskbar button
    - blackhole: Black Hole - spiral vortex
    - glass: Glass - shards spin and fall
    - shatter: Square Shatter - explosion
    - thanos: Thanos Snap - disintegration wave
    - perlin: Perlin Dissolve - acid burn
    - pixel_melt: Pixel Melt - dripping paint
    - glitch: Cyber Glitch - digital teleport
    - tv_off: Retro TV Off - CRT power-down
    - ink_splash: Ink Splash - organic blot
    - mirage: Mirage - wavy shimmer
    - swell: Swell - expand and fade
    - pop: Pop - quick bubble shrink
    - fade_up: Fade Up - Fluent dismiss
    - poof: Poof - puff of dust
    - random: Random - every effect once per cycle
  - duration_ms: 250
    $name: Duration (ms)
    $description: >-
      Base close duration, 50-2000 (particle effects take longer, see above). For the
      last window of an application this also delays its shutdown by that long.
  $name: Close
  $description: The effect played when an application window closes.
- special:
  - special_animation: true
    $name: Animate special windows
    $description: >-
      Popups, info bubbles, dropdowns, autocomplete lists, flyouts, menus drawn by
      apps, tooltips, notification popups and splash screens (the V1-V3.5 behaviour,
      widened). Never holds the application; at most 4 at once and 8 per second.
  - effect: pop
    $name: Special-window effect
    $description: Every effect can be used, or the same one as the close effect.
    $options:
    - same_as_close: Same as the close effect
    - genie: Genie - the macOS genie into the taskbar button
    - scale: Scale - the macOS scale effect into the taskbar button
    - fluid: Fluid - a droplet pours into the taskbar button
    - blackhole: Black Hole - spiral vortex
    - glass: Glass - shards spin and fall
    - shatter: Square Shatter - explosion
    - thanos: Thanos Snap - disintegration wave
    - perlin: Perlin Dissolve - acid burn
    - pixel_melt: Pixel Melt - dripping paint
    - glitch: Cyber Glitch - digital teleport
    - tv_off: Retro TV Off - CRT power-down
    - ink_splash: Ink Splash - organic blot
    - mirage: Mirage - wavy shimmer
    - swell: Swell - expand and fade
    - pop: Pop - quick bubble shrink
    - fade_up: Fade Up - Fluent dismiss
    - poof: Poof - puff of dust
    - random: Random - every effect once per cycle
  - duration_ms: 160
    $name: Duration (ms)
    $description: Base duration for special windows, 50-1000.
  - tool_windows: true
    $name: Include tool windows
    $description: Windows with the tool-window style (palettes, tooltips, many notification popups).
  - child_windows: true
    $name: Include child windows
    $description: >-
      A child window (a docked panel, an MDI document, a task pane, a find bar)
      animates when the application closes it. Content hosts that browsers recreate
      while navigating, and File Explorer's own child windows, never animate.
  - child_hide: false
    $name: Also when child windows are hidden
    $description: >-
      Off by default: applications hide child windows constantly (every tab switch),
      so this plays a close effect on every tab switch.
  - min_size_px: 0
    $name: Minimum size (px)
    $description: Skip special windows smaller than this in either direction. 0 = no size limit.
  - excluded_classes: [""]
    $name: Excluded window classes
    $description: >-
      Window class names that should never animate as special windows (exact match,
      case-insensitive). Turn on "Log special-window decisions" to see the classes.
  $name: Special windows
  $description: Popups, bubbles, dropdowns, menus, tooltips, splash screens, tool and child windows.
- effects:
  - particle_block_size: 5
    $name: Particle size (px)
    $description: >-
      Block size of Square Shatter, Thanos Snap and Perlin Dissolve, 1-100. Very
      large windows raise it automatically so a frame stays under ~400 000 particles.
  - blackhole_twist: 25
    $name: Black Hole spiral twist (0-50)
    $description: How sharply the wormhole spirals the window blocks toward the vortex center.
  - blackhole_quality: "2"
    $name: Black Hole particle size
    $description: Block size of the wormhole particles.
    $options:
    - "1": Large blocks (fast)
    - "2": Medium blocks
    - "3": Fine blocks (slow)
  - window_shadow: true
    $name: Window shadow during animations
    $description: >-
      Draws the window's drop shadow under the animation (it follows the genie and
      fades with the other effects), so the shadow no longer vanishes on the first
      frame of a minimize or pops back at the end of a restore. Off for maximized
      windows and windows without a shadow.
  $name: Effect tuning
- switch:
  - switch_animation: true
    $name: Animate Alt+Tab switching
    $description: >-
      The window selected with Alt+Tab scales up and fades in, drawn from a live DWM
      thumbnail. Cancelling Alt+Tab does not animate; a minimized window plays its
      restore effect instead.
  - duration_ms: 200
    $name: Duration (ms)
    $description: Alt+Tab switch duration, 50-1000.
  - start_scale: 94
    $name: Start scale (%)
    $description: Size the window starts from, 80-99 % of its real size.
  $name: Alt+Tab switch
- taskbar:
  - multi_monitor: true
    $name: Multi-monitor support (experimental)
    $description: >-
      Play the animation on the monitor the window is on and target that monitor's
      taskbar, instead of always the primary monitor's.
  - taskbar_position: auto
    $name: Taskbar position
    $description: >-
      Where the taskbar sits. Auto detects it per monitor (works with the "Taskbar on
      top for Windows 11" mod). Left/right side taskbars play in-place effects; the
      Genie and Fluid fall back to Swell there.
    $options:
    - auto: Auto-detect
    - top: Top
    - bottom: Bottom
  - dock_offset_y: 0
    $name: Dock Y offset (px)
    $description: Vertical nudge of where genies land on the taskbar icon. Negative = higher. -100..100.
  - unhide_taskbar: true
    $name: Unhide taskbar (for taskbar auto-hide)
    $description: >-
      If the taskbar is set to auto-hide, briefly reveal it during a minimize so the
      genie has a visible dock to flow into.
  - unhide_duration_ms: 450
    $name: Taskbar unhide duration (ms)
    $description: How long to keep the taskbar revealed on the auto-hide path. 0-5000.
  $name: Taskbar
- excluded_programs: [""]
  $name: Excluded programs
  $description: >-
    Programs whose windows should never animate (one per entry, e.g.
    WindowsTerminal.exe - with or without .exe, or a full path). This matches the
    process that OWNS the window, wherever the animation is triggered from, which
    Windhawk's own process exclusion list cannot do.
- diagnostics:
  - performance_telemetry: false
    $name: Log performance telemetry
    $description: >-
      One compact line per animation in the Windhawk log: where the click time went
      (capture, taskbar lookup, setup, first-frame wait), frames, late frames, the
      cadence (frames shown for 1 / 2 / 3+ refreshes), render and present cost.
      Nothing is logged per frame.
  - close_log: false
    $name: Log close decisions
    $description: One log line per close event explaining which rule skipped or started it.
  - special_log: false
    $name: Log special-window decisions
    $description: >-
      One log line per disappearing special-window candidate with its class, styles
      and the verdict. Use it to find classes for "Excluded window classes".
  $name: Diagnostics
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <new>
#include <windows.h>
#if defined(__x86_64__) || defined(__i386__)
#define MAC_HAVE_SSE2 1
#include <emmintrin.h>
#else
#define MAC_HAVE_SSE2 0
#endif
#include <dwmapi.h>
#include <stdint.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <mutex>
#include <vector>
#include <string>
#include <algorithm>
#include <thread>
#include <cwctype>
#include <climits>
#include <memory>
#include <uiautomation.h>
#include <shellapi.h>
#include <shlobj.h>
#include <propsys.h>
#include <appmodel.h>
#include <mmsystem.h>

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

#ifndef DWM_CLOAKED_APP
#define DWM_CLOAKED_APP 0x00000001
#endif
#ifndef DWM_CLOAKED_SHELL
#define DWM_CLOAKED_SHELL 0x00000002
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif

#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

#define PI 3.14159265358979323846f
#define COMPAT_TASKBAR_HEIGHT 70
#define COMPAT_ICON_SIZE      33
#define MAX_SNAPSHOT_CACHE 8
#define MAX_SNAPSHOT_BYTES (64ull * 1024 * 1024)
#define SNAPSHOT_MAX_AGE_MS (15ull * 60 * 1000)
#define CLOSE_GUARD_REARM_MS 5000
#define PENDING_CLOSE_MAX_AGE_MS 10000
#define PENDING_CLOSE_MAX_ENTRIES 4
#define PENDING_CLOSE_MAX_BYTES (64ull * 1024 * 1024)
#define PENDING_CLOSE_THROTTLE_MS 500
// Per-window dock positions are trusted without a refresh while the taskbar's
// layout signature (see MacGenieTaskbarSignature) is unchanged AND the entry is
// younger than ICON_POS_TRUST_MS (a safety net for changes the signature cannot
// see, e.g. a pin reorder). A changed signature, or an older entry, is served
// from cache while a background refresh runs.
#define ICON_POS_TRUST_MS 30000
#define UIA_NEGATIVE_CACHE_MS 10000
// The slow pyramid levels / Classic worker pool are rationed across processes
// through a named semaphore so a Win+M storm cannot oversubscribe the CPU.
#define GENIE_BUDGET_SEM_NAME L"Local\\UltimateGenieBudget_v37"
// More concurrent animations than this (Win+M on a crowded desktop) fall back
// to the system animation rather than spawning a render thread (plus, for
// Classic, a worker pool) per window.
#define MAX_CONCURRENT_ANIMS 8
// Unload join budget. Every render loop polls g_unloading once per frame and
// leaves within a frame or two, so this only has to cover a frame, a teardown and
// a UI Automation worker parked in COM - not a whole animation (effects can last
// up to 5 s since 3.8).
#define UNLOAD_JOIN_BUDGET_MS 4000
// Largest snapshot or animation canvas, in pixels: 64 MP (256 MB at 32 bpp).
// Two 8K monitors side by side still fit; it only stops pathological sizes (a
// window tens of thousands of pixels wide, or parked far off screen), which then
// get the system animation instead (macOS Minimize Animation fork 3.2.1).
#define MACGENIE_MAX_CANVAS_PIXELS (64LL * 1024 * 1024)

// Where the taskbar sits for the monitor an animation targets.
enum DockPos {
    DOCK_BOTTOM = 0,
    DOCK_TOP = 1,
    DOCK_SIDE = 2   // left/right taskbar - fallback to system
};

// "Taskbar position" setting values.
enum TaskbarSetting {
    TB_AUTO = 0,
    TB_SET_TOP = 1,
    TB_SET_BOTTOM = 2
};

struct BHBlock {
    float r0, a0;   // radius + angle in un-warped window
    int x, y;       // top-left corner in snapshot
};

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original = nullptr;

typedef LRESULT (WINAPI *DefWindowProcA_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcA_t DefWindowProcA_Original = nullptr;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original = nullptr;

typedef BOOL (WINAPI *ShowWindowAsync_t)(HWND hWnd, int nCmdShow);
ShowWindowAsync_t ShowWindowAsync_Original = nullptr;

typedef BOOL (WINAPI *SetWindowPlacement_t)(HWND hWnd, const WINDOWPLACEMENT* lpwndpl);
SetWindowPlacement_t SetWindowPlacement_Original = nullptr;

typedef BOOL (WINAPI *CloseWindow_t)(HWND hWnd);
CloseWindow_t CloseWindow_Original = nullptr;

typedef BOOL (WINAPI *SetWindowPos_t)(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                                      int cx, int cy, UINT uFlags);
SetWindowPos_t SetWindowPos_Original = nullptr;

typedef LRESULT (WINAPI *SendMessageW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
SendMessageW_t SendMessageW_Original = nullptr;

typedef LRESULT (WINAPI *SendMessageA_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
SendMessageA_t SendMessageA_Original = nullptr;

typedef BOOL (WINAPI *DestroyWindow_t)(HWND hWnd);
DestroyWindow_t DestroyWindow_Original = nullptr;

typedef BOOL (WINAPI *AnimateWindow_t)(HWND hWnd, DWORD dwTime, DWORD dwFlags);
AnimateWindow_t AnimateWindow_Original = nullptr;

// DPI helper typedef
typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
static GetDpiForMonitor_t g_pfnGetDpiForMonitor = nullptr;
static HMODULE g_hShcore = nullptr;

// -------------------------------------------------------------------------
// Telemetry (Diagnostics setting). Off by default. When on, one line per
// animation is written in MacGenieFinishAnimation; nothing is logged per frame
// and every probe below is a branch on a per-animation bool when off.
// -------------------------------------------------------------------------
static std::atomic<bool> g_telemetry{false};

struct MacGenieTelemetry {
    LONGLONG qpcFreq = 0;
    LONGLONG qHookEntry = 0;      // StartMacGenieAnim entry (the user's click)
    LONGLONG qCaptureDone = 0;    // snapshot ready, render thread about to start
    LONGLONG qThreadStart = 0;    // render thread entry
    LONGLONG qSetupDone = 0;      // V4: ghost, canvas, pool, precalc ready
    LONGLONG qFrame0Shown = 0;    // V4: frame 0 handed to the compositor
    LONGLONG qFirstFrame = 0;     // frame 0 composited (first-frame flush returned)
    LONGLONG qLastFrameStart = 0;
    double uiaWaitMs = 0;
    double intervalMs = 0;        // pacer interval of the animation's monitor
    double renderSumMs = 0, renderMaxMs = 0;
    double presentSumMs = 0, presentMaxMs = 0;
    double periodMaxMs = 0;       // steady state: the first gap is apart (V4)
    double firstGapMs = 0;        // frame 0 -> frame 1 (includes the first-frame flush)
    uint32_t frames = 0, late = 0;
    uint32_t cadence[4] = {};     // frames shown for 1 / 2 / 3+ refreshes (index 1..3)
    int strideMax = 1;            // largest even-cadence stride the pacer used
    int reversals = 0;            // mid-flight direction changes
    const wchar_t* engine = L"?";
};

struct MacGenieAnimData {
    HWND hRealWnd;
    HBITMAP hBitmap;      // 32-bit top-down premultiplied DIB snapshot (w x h)
    void* pBits;          // its bits
    RECT targetRect;      // extended-frame rect
    HMONITOR hMon;        // target monitor
    int width;
    int height;
    int targetDockX;      // taskbar button X
    int targetDockY;      // taskbar button Y
    DockPos dockPos;      // DOCK_BOTTOM or DOCK_TOP
    BOOL isRising;        // FALSE = minimize, TRUE = restore
    BOOL isClosing;       // close animation (Black Hole)
    LONG_PTR originalExStyle;
    BOOL hiddenByCloak;
    HANDLE hFirstFrameShown;
    // Signalled once a close animation has finished playing. The thread that is
    // processing the close waits on it, so the window outlives the wormhole.
    HANDLE hCloseFinished;
    int durationMs;
    int bounceStr;
    int bounceDurMs;
    int bhTwist;
    int bhQuality;
    int funnelWidth;
    int prevBandTop;
    int prevBandBottom;
    int prevBandLeft;
    int prevBandRight;
    BOOL requestedUnhide;
    HWND hNextApp;
    int unhideDurationMs;
    BOOL deferredMinimize;
    // Set by the engine once it has revealed the real window itself (rising
    // path, before the ghost is destroyed) so the finisher does not repeat it.
    BOOL revealDone;
    int dockOffsetY;
    // Taskbar focus was borrowed for the auto-hide reveal and must be handed
    // back - but only if the taskbar still owns it when the animation ends.
    BOOL taskbarFocusBorrowed;
    // Radius (in this process' coordinate space) of the window's DWM-rounded
    // corners; 0 = square (maximized, Win10, DONOTROUND). Drives the Modern
    // engine's silhouette so the first/last frame matches the real window.
    float cornerRadiusPx;
    // Snapshot is handed to the restore cache by the falling engine at the end
    // instead of being copied into a second bitmap at the start.
    BOOL cacheSnapshotOnFinish;
    // V3.8
    int fx;                 // FxId played by this animation
    int ctx;                // FxContext
    BOOL special;           // a special window (popup, tool / child window, splash)
    HWND hInsertAfter;      // special: the window that was above the source (its z-slot)
    BOOL ghostTopmost;      // special: the source was topmost
    BOOL aim;               // in-place particles aim at the taskbar dock
    BOOL telOn;
    MacGenieTelemetry tel;
    // V4: mid-flight reversal. A hook that sees a restore while this minimize
    // plays (or a minimize while this restore plays) does the native state change
    // under the cloak and writes the wanted direction here; the render thread
    // turns the running animation around at its next frame (MacAnimApplyReversal).
    BOOL reversible;          // a minimize / restore / launch (never close or special)
    volatile LONG wantDir;    // -1 = no request, 0 = falling, 1 = rising
    BOOL undoHideOnFallEnd;   // reversed to falling: lift the restore's hide at the end
    BOOL isLaunch;            // telemetry label
    BOOL shadow;              // V4: draw the window shadow under the ghost
};

struct MacGenieLaunchData {
    HWND hWnd;
    LONG_PTR originalExStyle;
    BOOL hiddenByCloak;    // TRUE = DWM cloak, FALSE = WS_EX_LAYERED + alpha 0
};

struct SnapCache { HBITMAP hBmp; void* pBits; int w; int h; ULONGLONG tick; };

// Per-window dock position with the tick it was resolved at, so a stale slot
// (taskbar reflowed after another app opened or closed) is refreshed instead
// of being served for the window's whole lifetime.
// `sig` is the taskbar layout signature the position was resolved under.
struct IconPosEntry { POINT pt; ULONGLONG tick; uint64_t sig; };

// --- CACHES & THREADS ---
static std::unordered_map<HWND, SnapCache> g_SnapshotCache;
static std::deque<HWND> g_SnapshotOrder;
static size_t g_SnapshotBytes = 0;                 // guarded by g_CacheMutex
static std::unordered_map<HWND, IconPosEntry> g_IconPositions;
static std::unordered_map<std::wstring, POINT> g_ProcessIconPositions;
// Negative cache: processes whose windows have no taskbar button (or whose
// button UI Automation cannot see) fail the lookup every time; without this
// every minimize of such a window would stall for the whole wait budget.
static std::unordered_map<std::wstring, ULONGLONG> g_ProcessUiaNegativeUntil;
// The owning PID is stored with the name and re-checked on every hit: HWND
// values are recycled by Windows, so a bare HWND-keyed name cache can hand a
// new window the previous owner's process name (wrong exclusion decisions,
// wrong taskbar-button targeting). GetWindowThreadProcessId is a cheap handle
// probe - the expensive OpenProcess + QueryFullProcessImageNameW is what the
// cache exists to avoid.
struct ProcNameEntry { DWORD pid; std::wstring name; std::wstring path; };
static std::unordered_map<HWND, ProcNameEntry> g_ProcessNameCache;
static std::unordered_set<HWND> g_AnimActive;
// V4: the running reversible animation of a window (minimize / restore /
// launch), so a hook can turn it around. Guarded by g_CacheMutex; an entry is
// removed by MacGenieFinishAnimation before the data is freed.
struct MacGenieAnimData;
static std::unordered_map<HWND, MacGenieAnimData*> g_AnimData;
// Set by MacGenieLaunchThread around its StartMacGenieAnim call (telemetry label).
static thread_local bool t_macLaunchStart = false;
static std::unordered_set<HWND> g_LaunchSeen;
// Windows the launch path made layered (alpha 0) and their original ex-style,
// so an unload mid-launch can put them back.
static std::unordered_map<HWND, LONG_PTR> g_LaunchLayered;
// The close-animation guard lives in a window property (L"GenieCloseAnim"), not
// in a per-process map: one close routinely fires hooks in more than one process
// (Explorer sending WM_CLOSE cross-process, plus the app's own DefWindowProc
// chain), and window properties are the only state all of them share. The
// property value is the GetTickCount low word of when the animation started
// (forced nonzero), so the guard also expires by age on its own and dies with
// the window.

// The stash also remembers the geometry the snapshot was taken at: once the
// window is hidden its extended-frame bounds may no longer be readable, and a
// size mismatch used to trigger a PrintWindow of the hidden window (blank).
struct MacGeniePendingClose { HBITMAP hBmp; void* pBits; int w; int h; ULONGLONG tick; RECT rect; };
static std::unordered_map<HWND, MacGeniePendingClose> g_PendingClose;
static size_t g_PendingCloseBytes = 0;             // guarded by g_CacheMutex

static std::mutex g_CacheMutex;
// Set the first time any per-window cache is written. The destroy / show hooks
// run for EVERY window of EVERY process the mod is loaded into; while this is
// false (a process that never animated anything) they skip g_CacheMutex and the
// six map lookups entirely.
static std::atomic<bool> g_cachesTouched{false};
static std::mutex g_WorkerThreadsMutex;
static std::vector<HANDLE> g_WorkerThreads;
static bool g_joinTimedOut = false;                // set by JoinWorkerThreads

// --- SETTINGS ---
static std::atomic<int> g_durationMs{250};
static std::atomic<int> g_closeDurationMs{250};
static std::atomic<bool> g_minimizeAnimation{true};
static std::atomic<bool> g_openAnimation{true};
static std::atomic<bool> g_launchAnimation{false};
static std::atomic<bool> g_multiMonitor{true};
static std::atomic<bool> g_unhideEnabled{true};
static std::atomic<int>  g_unhideDurationMs{450};
static std::atomic<int> g_modernFunnelWidth{8};   // genie tip width
static std::atomic<int> g_genieBendPct{40};
static std::atomic<bool> g_windowShadow{true};
static std::atomic<int> g_taskbarSetting{TB_AUTO};
static std::atomic<int> g_dockOffsetY{0};
static std::atomic<int> g_blackholeTwist{25};
static std::atomic<int> g_blackholeQuality{2};
static std::atomic<bool> g_blackholeClose{true};
static std::atomic<int> g_bounceStr{50};
static std::atomic<int> g_bounceDur{200};
static std::vector<std::wstring> g_ExcludedPrograms;

static std::atomic<bool> g_unloading{false};


// -------------------------------------------------------------------------
// V3.8: the effect catalog. Every selector (minimize / restore / launch,
// close, special windows) can pick any effect; `docked` effects (the genies)
// funnel into a dock point, the others play where the window is.
// -------------------------------------------------------------------------
// V4: the two genies (3.x "Modern" and "Classic") are one Genie, drawn by the
// analytic scanline renderer; Scale, Glass, Pixel Melt and Fluid are new.
enum FxId : int {
    FX_GENIE = 0, FX_BLACKHOLE,
    FX_SHATTER, FX_THANOS, FX_PERLIN, FX_GLITCH, FX_TV_OFF,
    FX_INK, FX_MIRAGE, FX_SWELL, FX_POP, FX_FADE_UP, FX_POOF,
    FX_SCALE, FX_GLASS, FX_MELT, FX_FLUID,
    FX_COUNT,
    FX_RANDOM = 100,
    FX_SAME_AS_CLOSE = 101
};

enum FxContext : int { CTX_MINIMIZE = 0, CTX_RESTORE = 1, CTX_CLOSE = 2, CTX_SPECIAL = 3 };

struct FxInfo {
    const wchar_t* key;      // setting value
    const wchar_t* telName;  // telemetry engine name
    bool docked;             // funnels into a dock point (taskbar button / popup anchor)
    int durPct;              // pacing: % of the context's duration setting
};

static const FxInfo kFxInfo[FX_COUNT] = {
    { L"genie",         L"genie",     true,  100 },
    { L"blackhole",     L"blackhole", false, 100 },
    { L"shatter",       L"shatter",   false, 240 },
    { L"thanos",        L"thanos",    false, 260 },
    { L"perlin",        L"perlin",    false, 240 },
    { L"glitch",        L"glitch",    false, 160 },
    { L"tv_off",        L"tv_off",    false, 140 },
    { L"ink_splash",    L"ink",       false, 200 },
    { L"mirage",        L"mirage",    false, 160 },
    { L"swell",         L"swell",     false, 120 },
    { L"pop",           L"pop",       false, 100 },
    { L"fade_up",       L"fade_up",   false, 100 },
    { L"poof",          L"poof",      false, 140 },
    { L"scale",         L"scale",     false, 100 },
    { L"glass",         L"glass",     false, 220 },
    { L"pixel_melt",    L"melt",      false, 200 },
    { L"fluid",         L"fluid",     true,  140 },
};

static std::atomic<int>  g_fxMinimize{FX_GENIE};
static std::atomic<int>  g_fxClose{FX_BLACKHOLE};
static std::atomic<int>  g_fxSpecial{FX_POP};
static std::atomic<bool> g_aimAtTaskbar{true};
static std::atomic<int>  g_particleBlock{5};
static std::atomic<bool> g_specialEnabled{true};
static std::atomic<int>  g_specialDurationMs{160};
static std::atomic<bool> g_specialToolWindows{true};
static std::atomic<bool> g_specialChildWindows{true};
static std::atomic<bool> g_specialChildHide{false};
static std::atomic<int>  g_specialMinSize{0};
static std::vector<std::wstring> g_SpecialExcludedClasses;   // lower-case; guarded by g_CacheMutex
// Its size, readable without the lock: the special-window check runs for every
// destroyed visible child control, and the list is almost always empty.
static std::atomic<int>  g_specialExcludedCount{0};
static std::atomic<int>  g_specialActive{0};                 // special ghosts in flight (this process)
static std::atomic<bool> g_switchEnabled{true};
static std::atomic<int>  g_switchDurationMs{200};
static std::atomic<int>  g_switchStartScale{94};
static std::atomic<bool> g_closeLog{false};
static std::atomic<bool> g_specialLog{false};

// Forward declaration
static void MacGenieUndoRisingHide(HWND hWnd, LONG_PTR originalExStyle, BOOL cloakHidden);
static void MacGenieSetDwmTransitions(HWND hWnd, BOOL enable);
static void MacGenieSetCloak(HWND hWnd, BOOL cloak);
static void MacGenieDisarmCloseGuardOnShow(HWND hWnd);
HWND FindTaskbarForMonitor(HMONITOR hMon);

// Helper: safe worker thread creation and cleanup
static bool StartWorkerThread(LPTHREAD_START_ROUTINE proc, void* param) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    for (size_t i = g_WorkerThreads.size(); i-- > 0;) {
        if (WaitForSingleObject(g_WorkerThreads[i], 0) == WAIT_OBJECT_0) {
            CloseHandle(g_WorkerThreads[i]);
            g_WorkerThreads.erase(g_WorkerThreads.begin() + i);
        }
    }
    HANDLE hThread = CreateThread(NULL, 0, proc, param, 0, NULL);
    if (!hThread) return false;
    g_WorkerThreads.push_back(hThread);
    return true;
}

// Joins every registered worker within one overall budget. Every loop in the
// mod polls g_unloading, so the wait normally returns in a frame or two; the
// budget only matters for a UIA worker parked inside COM. If it does run out,
// g_joinTimedOut tells Wh_ModUninit not to release resources a straggler may
// still be using.
static void JoinWorkerThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        threads.swap(g_WorkerThreads);
    }
    const ULONGLONG deadline = GetTickCount64() + UNLOAD_JOIN_BUDGET_MS;
    for (size_t i = 0; i < threads.size(); i += MAXIMUM_WAIT_OBJECTS) {
        const DWORD count = (DWORD)std::min<size_t>(MAXIMUM_WAIT_OBJECTS, threads.size() - i);
        ULONGLONG now = GetTickCount64();
        DWORD remaining = (now < deadline) ? (DWORD)(deadline - now) : 0;
        DWORD r = WaitForMultipleObjects(count, threads.data() + i, TRUE, remaining);
        if (r == WAIT_TIMEOUT || r == WAIT_FAILED) g_joinTimedOut = true;
    }
    for (HANDLE h : threads) CloseHandle(h);
}

// -------------------------------------------------------------------------
// Fast Math & Interpolation Helpers
// -------------------------------------------------------------------------
static inline int MacClampInt(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline float MacClampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static inline int MacMax(int a, int b) { return a > b ? a : b; }
static inline int MacMin(int a, int b) { return a < b ? a : b; }
static inline int imin(int a, int b)   { return a < b ? a : b; }
static inline int imax(int a, int b)   { return a > b ? a : b; }
static inline double dmin(double a, double b) { return a < b ? a : b; }
static inline double dmax(double a, double b) { return a > b ? a : b; }
static inline double Clamp01(double x)        { return x < 0.0 ? 0.0 : (x > 1.0 ? 1.0 : x); }

// Perlin Smootherstep (C2 continuous curve)
static inline double Smoother(double x) {
    if (x <= 0.0) return 0.0;
    if (x >= 1.0) return 1.0;
    return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

// The Classic rasterizer works on PREMULTIPLIED BGRA throughout. The snapshot
// keeps its alpha (rounded corners, Mica / acrylic), so every helper below has
// to carry all four channels: V3 dropped alpha in the lerp and the mip filter
// and OR'd 0xFF000000 back on output, which painted Windows 11 rounded-corner
// pixels as opaque black wedges and made translucent windows opaque.

// Scales all four premultiplied channels by a coverage a256 in 0..256 (256 =
// unchanged). Used for sub-pixel edge coverage and the tail fade.
static inline DWORD CovMulP(DWORD c, unsigned a256) {
    if (a256 >= 256) return c;
    DWORD rb = ((c & 0x00FF00FF) * a256) >> 8;
    DWORD ag = (((c >> 8) & 0x00FF00FF) * a256) >> 8;
    return (rb & 0x00FF00FF) | ((ag & 0x00FF00FF) << 8);
}

// Lerp premultiplied 32bpp with 8-bit weight (w: 0..255), A/G and R/B pairs in
// parallel.
static inline DWORD Lerp8(DWORD a, DWORD b, int w) {
    DWORD rb = ((a & 0x00FF00FF) * (DWORD)(256 - w) + (b & 0x00FF00FF) * (DWORD)w) >> 8;
    DWORD ag = (((a >> 8) & 0x00FF00FF) * (DWORD)(256 - w) + ((b >> 8) & 0x00FF00FF) * (DWORD)w) >> 8;
    return (rb & 0x00FF00FF) | ((ag & 0x00FF00FF) << 8);
}

static inline DWORD SampleBilin(const DWORD* r0, const DWORD* r1, int mw,
                                int fx16, int fxMax, int wy) {
    int f = fx16;
    if (f < 0) f = 0; else if (f > fxMax) f = fxMax;
    int x0 = f >> 16;
    int wx = (f >> 8) & 0xFF;
    int x1 = (x0 + 1 < mw) ? x0 + 1 : x0;
    DWORD top = wx ? Lerp8(r0[x0], r0[x1], wx) : r0[x0];
    if (!wy) return top;
    // The bottom row is blended horizontally with wx, like the top row. V3.5
    // used wy here (a typo against derrunk's original), which misregistered
    // the bottom sample whenever the vertical weight was nonzero.
    DWORD bot = wx ? Lerp8(r1[x0], r1[x1], wx) : r1[x0];
    return Lerp8(top, bot, wy);
}

// V4: SSE2 version of a run of SampleBilin calls along a row (2 pixels per
// step). Windows 10 requires SSE2, so it is always available. The arithmetic is
// Lerp8's exactly - per channel (a * (256 - w) + b * w) >> 8 in 16-bit lanes,
// which cannot overflow (at most 255 * 256) - so the output is bit-identical to
// the scalar loop (tests\v4_test.cpp checks it on random inputs).
#if MAC_HAVE_SSE2
static inline __m128i MacLerp16(__m128i a, __m128i b, __m128i w, __m128i iw) {
    return _mm_srli_epi16(_mm_add_epi16(_mm_mullo_epi16(a, iw), _mm_mullo_epi16(b, w)), 8);
}

#endif  // MAC_HAVE_SSE2

// Tests flip this to compare the SIMD paths against the scalar ones.
static bool g_macNoSimd = false;

static void SampleBilinRunSse2(DWORD* out, int n, const DWORD* r0, const DWORD* r1, int mw,
                               int fx16, int dfx16, int fxMax, int wy) {
#if !MAC_HAVE_SSE2
    // No SSE2 on this architecture (ARM64): the scalar loop, same output.
    for (int k = 0; k < n; k++) out[k] = SampleBilin(r0, r1, mw, fx16 + k * dfx16, fxMax, wy);
#else
    const __m128i zero = _mm_setzero_si128();
    const __m128i k256 = _mm_set1_epi16(256);
    const __m128i vwy = _mm_set1_epi16((short)wy), viwy = _mm_set1_epi16((short)(256 - wy));
    int k = 0;
    for (; k + 2 <= n; k += 2) {
        int fa = fx16 + k * dfx16, fb = fa + dfx16;
        if (fa < 0) fa = 0; else if (fa > fxMax) fa = fxMax;
        if (fb < 0) fb = 0; else if (fb > fxMax) fb = fxMax;
        const int xa = fa >> 16, xb = fb >> 16;
        const int wa = (fa >> 8) & 0xFF, wb = (fb >> 8) & 0xFF;
        const int xa1 = (xa + 1 < mw) ? xa + 1 : xa, xb1 = (xb + 1 < mw) ? xb + 1 : xb;
        const __m128i w = _mm_set_epi16((short)wb, (short)wb, (short)wb, (short)wb,
                                        (short)wa, (short)wa, (short)wa, (short)wa);
        const __m128i iw = _mm_sub_epi16(k256, w);
        const __m128i p00 = _mm_unpacklo_epi8(_mm_set_epi32(0, 0, (int)r0[xb], (int)r0[xa]), zero);
        const __m128i p01 = _mm_unpacklo_epi8(_mm_set_epi32(0, 0, (int)r0[xb1], (int)r0[xa1]), zero);
        __m128i top = MacLerp16(p00, p01, w, iw);
        if (wy) {
            const __m128i p10 = _mm_unpacklo_epi8(_mm_set_epi32(0, 0, (int)r1[xb], (int)r1[xa]), zero);
            const __m128i p11 = _mm_unpacklo_epi8(_mm_set_epi32(0, 0, (int)r1[xb1], (int)r1[xa1]), zero);
            const __m128i bot = MacLerp16(p10, p11, w, iw);
            top = MacLerp16(top, bot, vwy, viwy);
        }
        _mm_storel_epi64((__m128i*)(out + k), _mm_packus_epi16(top, zero));
    }
    for (; k < n; k++) out[k] = SampleBilin(r0, r1, mw, fx16 + k * dfx16, fxMax, wy);
#endif
}

struct MipLevel {
    int w = 0, h = 0;
    const DWORD* p = nullptr;
    std::unique_ptr<DWORD[]> store;   // levels >= 1 only; left uninitialised
};

// Mip pyramid (2x2 box filter, premultiplied BGRA, up to 5 levels).
//
// Split in two so the expensive half can be scheduled: MipsPlan lays the pyramid
// out and allocates the level buffers WITHOUT touching them (new DWORD[n] is
// uninitialised - V3.6's std::vector::resize zero-filled every level first),
// MipsFillRows then fills any row range of a level from the one above it. Rows of
// a level are independent, so the Classic engine fans a level out across its
// worker pool, and the pyramid is built after the first frame is already on
// screen instead of while the user's UI thread waits for it.
static void MipsPlan(const DWORD* src, int W, int H, std::vector<MipLevel>& mips) {
    mips.clear();
    mips.reserve(5);   // no reallocation: the references below stay valid
    MipLevel m0;
    m0.w = W; m0.h = H; m0.p = src;
    mips.push_back(std::move(m0));

    while ((int)mips.size() < 5) {
        const int nw = mips.back().w / 2, nh = mips.back().h / 2;
        if (nw < 24 || nh < 24) break;
        MipLevel d;
        d.w = nw; d.h = nh;
        d.store.reset(new (std::nothrow) DWORD[(size_t)nw * nh]);
        if (!d.store) break;           // out of memory: a shorter pyramid is fine
        d.p = d.store.get();
        mips.push_back(std::move(d));
    }
}

static void MipsFillRows(const std::vector<MipLevel>& mips, int lvl, int y0, int y1) {
    const MipLevel& s = mips[lvl - 1];
    const MipLevel& d = mips[lvl];
    const int nw = d.w;
    for (int y = y0; y < y1; y++) {
        const DWORD* r0 = s.p + (size_t)(2 * y)     * s.w;
        const DWORD* r1 = s.p + (size_t)(2 * y + 1) * s.w;
        DWORD* o = d.store.get() + (size_t)y * nw;
        for (int x = 0; x < nw; x++) {
            DWORD a = r0[2 * x], b = r0[2 * x + 1];
            DWORD c = r1[2 * x], e = r1[2 * x + 1];
            DWORD rb = (((a & 0xFF00FF) + (b & 0xFF00FF) + (c & 0xFF00FF) + (e & 0xFF00FF)) >> 2) & 0xFF00FF;
            DWORD ag = ((((a >> 8) & 0xFF00FF) + ((b >> 8) & 0xFF00FF) +
                         ((c >> 8) & 0xFF00FF) + ((e >> 8) & 0xFF00FF)) >> 2) & 0xFF00FF;
            o[x] = rb | (ag << 8);
        }
    }
}

// Single-threaded convenience: plan + fill everything (Modern engine, tests).
[[maybe_unused]] static void BuildMips(const DWORD* src, int W, int H, std::vector<MipLevel>& mips) {
    MipsPlan(src, W, H, mips);
    for (int l = 1; l < (int)mips.size(); l++) MipsFillRows(mips, l, 0, mips[l].h);
}

// -------------------------------------------------------------------------
// Parallel row pool. One implementation behind every multi-core loop in the mod
// (Classic scanlines, mip levels, Black Hole passes). Work is handed out in small
// chunks from a shared atomic counter, so a slow core (an E-core, a preempted
// thread) simply claims fewer chunks instead of stalling a statically-sized
// band, and the calling thread works too. Workers park on auto-reset events
// between jobs.
// -------------------------------------------------------------------------
typedef void (*PoolFn)(void* ctx, int begin, int end);

struct ParallelPool {
    struct Slot { ParallelPool* pool; HANDLE hGo; HANDLE hThread; };
    Slot slots[6] = {};
    int nWorkers = 0;
    HANDLE hDone = nullptr;           // auto-reset: "every worker left the job"
    volatile LONG pending = 0;
    volatile LONG nextRow = 0;
    LONG endRow = 0;
    LONG chunk = 16;
    PoolFn fn = nullptr;
    void* ctx = nullptr;
    volatile LONG quit = 0;
};

static void PoolDrain(ParallelPool* p) {
    for (;;) {
        const LONG y = InterlockedExchangeAdd(&p->nextRow, p->chunk);
        if (y >= p->endRow) break;
        const LONG e = (y + p->chunk < p->endRow) ? y + p->chunk : p->endRow;
        p->fn(p->ctx, (int)y, (int)e);
    }
}

static DWORD WINAPI PoolWorker(LPVOID lp) {
    ParallelPool::Slot* s = (ParallelPool::Slot*)lp;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    for (;;) {
        WaitForSingleObject(s->hGo, INFINITE);
        ParallelPool* p = s->pool;
        if (p->quit) return 0;
        PoolDrain(p);
        if (InterlockedDecrement(&p->pending) == 0) SetEvent(p->hDone);
    }
}

// Cross-process rationing of worker threads (see GENIE_BUDGET_SEM_NAME). A slot
// is taken per animation that wants a pool; without one the animation renders
// single-threaded, which is exactly what it did before pools existed. A failure
// to create/open the semaphore (AppContainer, integrity-level mismatch) simply
// means no rationing.
static HANDLE MacGenieAcquireBudgetSlot(int* slotsOut) {
    *slotsOut = 0;
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    int cores = (int)si.dwNumberOfProcessors;
    int want = cores / 2 - 1;
    if (want > 6) want = 6;
    if (want <= 0) return NULL;
    // Total worker slots machine-wide: roughly half the cores.
    LONG total = cores / 2;
    if (total < 2) total = 2;
    HANDLE h = CreateSemaphoreW(NULL, total, total, GENIE_BUDGET_SEM_NAME);
    if (!h) { *slotsOut = want; return NULL; }       // unrationed
    int got = 0;
    while (got < want && WaitForSingleObject(h, 0) == WAIT_OBJECT_0) got++;
    *slotsOut = got;
    return h;   // released by MacGenieReleaseBudgetSlot
}

static void MacGenieReleaseBudgetSlot(HANDLE h, int slots) {
    if (!h) return;
    if (slots > 0) ReleaseSemaphore(h, slots, NULL);
    CloseHandle(h);
}

static void PoolInit(ParallelPool* p, int workers) {
    p->hDone = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!p->hDone) return;
    if (workers > 6) workers = 6;
    for (int i = 0; i < workers; i++) {
        ParallelPool::Slot& sl = p->slots[i];
        sl.pool = p;
        sl.hGo = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (!sl.hGo) break;
        sl.hThread = CreateThread(NULL, 0, PoolWorker, &sl, 0, NULL);
        if (!sl.hThread) { CloseHandle(sl.hGo); break; }
        p->nWorkers = i + 1;
    }
}

// Runs fn over [begin, end) on the pool plus the calling thread; returns when
// every row is done. Falls back to a plain loop for small jobs or no workers.
static void PoolRun(ParallelPool* p, PoolFn fn, void* ctx, int begin, int end,
                    int chunk, int minRowsForParallel) {
    if (end <= begin) return;
    if (p->nWorkers <= 0 || (end - begin) < minRowsForParallel) {
        fn(ctx, begin, end);
        return;
    }
    p->fn = fn; p->ctx = ctx;
    p->endRow = end; p->chunk = chunk > 0 ? chunk : 16;
    p->nextRow = begin;
    p->pending = p->nWorkers;
    for (int i = 0; i < p->nWorkers; i++) SetEvent(p->slots[i].hGo);
    PoolDrain(p);
    WaitForSingleObject(p->hDone, INFINITE);
}

static void PoolShutdown(ParallelPool* p) {
    p->quit = 1;
    for (int i = 0; i < p->nWorkers; i++) SetEvent(p->slots[i].hGo);
    for (int i = 0; i < p->nWorkers; i++) {
        WaitForSingleObject(p->slots[i].hThread, 500);
        CloseHandle(p->slots[i].hThread);
        CloseHandle(p->slots[i].hGo);
    }
    p->nWorkers = 0;
    if (p->hDone) { CloseHandle(p->hDone); p->hDone = nullptr; }
}

// Mip level jobs for PoolRun.
struct MipJob { const std::vector<MipLevel>* mips; int lvl; };
static void MipJobFn(void* c, int y0, int y1) {
    MipJob* j = (MipJob*)c;
    MipsFillRows(*j->mips, j->lvl, y0, y1);
}

// Fills levels [from, n) of a planned pyramid on the pool.
static void MipsFillParallel(ParallelPool* pool, const std::vector<MipLevel>& mips, int from) {
    for (int l = from; l < (int)mips.size(); l++) {
        MipJob job = { &mips, l };
        PoolRun(pool, MipJobFn, &job, 0, mips[l].h, 32, 256);
    }
}

// -------------------------------------------------------------------------
// Settings Loader
// -------------------------------------------------------------------------
static int MacFxParse(PCWSTR s, int def, bool allowSameAsClose) {
    if (!s || !*s) return def;
    if (wcscmp(s, L"random") == 0) return FX_RANDOM;
    if (allowSameAsClose && wcscmp(s, L"same_as_close") == 0) return FX_SAME_AS_CLOSE;
    // 3.x keys: the two genies are one in V4.
    if (wcscmp(s, L"genie_modern") == 0 || wcscmp(s, L"genie_classic") == 0) return FX_GENIE;
    for (int i = 0; i < FX_COUNT; i++) {
        if (wcscmp(s, kFxInfo[i].key) == 0) return i;
    }
    return def;
}

static int MacGetFxSetting(PCWSTR name, int def, bool allowSameAsClose) {
    PCWSTR s = Wh_GetStringSetting(name);
    const int v = MacFxParse(s, def, allowSameAsClose);
    Wh_FreeStringSetting(s);
    return v;
}

// Trimmed, lower-cased copy of a list entry. For programs: reduced to the file
// name if a path was given and stripped of a trailing ".exe".
static std::wstring MacGenieNormalizeEntry(PCWSTR raw, bool program) {
    std::wstring s = raw ? raw : L"";
    size_t b = s.find_first_not_of(L" \t\r\n");
    size_t e = s.find_last_not_of(L" \t\r\n");
    s = (b == std::wstring::npos) ? L"" : s.substr(b, e - b + 1);
    if (program) {
        size_t slash = s.find_last_of(L"\\/");
        if (slash != std::wstring::npos) s = s.substr(slash + 1);
    }
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    if (program && s.size() > 4 && s.compare(s.size() - 4, 4, L".exe") == 0) s.resize(s.size() - 4);
    return s;
}

void MacGenieLoadSettings() {
    // Minimize, restore and launch
    g_durationMs.store(MacClampInt(Wh_GetIntSetting(L"minimize.duration_ms"), 50, 2000),
                       std::memory_order_relaxed);
    g_minimizeAnimation.store(Wh_GetIntSetting(L"minimize.minimize_animation") != 0, std::memory_order_relaxed);
    g_openAnimation.store(Wh_GetIntSetting(L"minimize.restore_animation") != 0, std::memory_order_relaxed);
    g_launchAnimation.store(Wh_GetIntSetting(L"minimize.launch_animation") != 0, std::memory_order_relaxed);
    g_fxMinimize.store(MacGetFxSetting(L"minimize.effect", FX_GENIE, false), std::memory_order_relaxed);
    g_aimAtTaskbar.store(Wh_GetIntSetting(L"minimize.aim_at_taskbar") != 0, std::memory_order_relaxed);

    int funnelW = Wh_GetIntSetting(L"minimize.genie_tip_width");
    if (funnelW <= 0) funnelW = 8;
    g_modernFunnelWidth.store(MacClampInt(funnelW, 8, 120), std::memory_order_relaxed);
    {
        int bend = Wh_GetIntSetting(L"minimize.genie_bend_pct");
        if (bend <= 0) bend = 40;
        g_genieBendPct.store(MacClampInt(bend, 15, 70), std::memory_order_relaxed);
    }
    g_bounceStr.store(MacClampInt(Wh_GetIntSetting(L"minimize.bounce_strength"), 0, 100), std::memory_order_relaxed);
    g_bounceDur.store(MacClampInt(Wh_GetIntSetting(L"minimize.bounce_duration_ms"), 0, 1000), std::memory_order_relaxed);

    // Close
    g_blackholeClose.store(Wh_GetIntSetting(L"close.close_animation") != 0, std::memory_order_relaxed);
    g_fxClose.store(MacGetFxSetting(L"close.effect", FX_BLACKHOLE, false), std::memory_order_relaxed);
    g_closeDurationMs.store(MacClampInt(Wh_GetIntSetting(L"close.duration_ms"), 50, 2000),
                            std::memory_order_relaxed);

    // Special windows
    g_specialEnabled.store(Wh_GetIntSetting(L"special.special_animation") != 0, std::memory_order_relaxed);
    g_fxSpecial.store(MacGetFxSetting(L"special.effect", FX_POP, true), std::memory_order_relaxed);
    g_specialDurationMs.store(MacClampInt(Wh_GetIntSetting(L"special.duration_ms"), 50, 1000),
                              std::memory_order_relaxed);
    g_specialToolWindows.store(Wh_GetIntSetting(L"special.tool_windows") != 0, std::memory_order_relaxed);
    g_specialChildWindows.store(Wh_GetIntSetting(L"special.child_windows") != 0, std::memory_order_relaxed);
    g_specialChildHide.store(Wh_GetIntSetting(L"special.child_hide") != 0, std::memory_order_relaxed);
    g_specialMinSize.store(MacClampInt(Wh_GetIntSetting(L"special.min_size_px"), 0, 4000), std::memory_order_relaxed);

    // Effect tuning
    g_particleBlock.store(MacClampInt(Wh_GetIntSetting(L"effects.particle_block_size"), 1, 100),
                          std::memory_order_relaxed);
    g_blackholeTwist.store(MacClampInt(Wh_GetIntSetting(L"effects.blackhole_twist"), 0, 50), std::memory_order_relaxed);
    // A string setting, since a setting with $options must be a string.
    {
        PCWSTR q = Wh_GetStringSetting(L"effects.blackhole_quality");
        int quality = (q && *q) ? _wtoi(q) : 2;
        Wh_FreeStringSetting(q);
        g_blackholeQuality.store(MacClampInt(quality, 1, 3), std::memory_order_relaxed);
    }
    g_windowShadow.store(Wh_GetIntSetting(L"effects.window_shadow") != 0, std::memory_order_relaxed);

    // Alt+Tab
    g_switchEnabled.store(Wh_GetIntSetting(L"switch.switch_animation") != 0, std::memory_order_relaxed);
    g_switchDurationMs.store(MacClampInt(Wh_GetIntSetting(L"switch.duration_ms"), 50, 1000), std::memory_order_relaxed);
    g_switchStartScale.store(MacClampInt(Wh_GetIntSetting(L"switch.start_scale"), 80, 99), std::memory_order_relaxed);

    // Taskbar
    g_multiMonitor.store(Wh_GetIntSetting(L"taskbar.multi_monitor") != 0, std::memory_order_relaxed);
    {
        PCWSTR tpos = Wh_GetStringSetting(L"taskbar.taskbar_position");
        if (wcscmp(tpos, L"top") == 0) {
            g_taskbarSetting.store(TB_SET_TOP, std::memory_order_relaxed);
        } else if (wcscmp(tpos, L"bottom") == 0) {
            g_taskbarSetting.store(TB_SET_BOTTOM, std::memory_order_relaxed);
        } else {
            g_taskbarSetting.store(TB_AUTO, std::memory_order_relaxed);
        }
        Wh_FreeStringSetting(tpos);
    }
    g_dockOffsetY.store(MacClampInt(Wh_GetIntSetting(L"taskbar.dock_offset_y"), -100, 100), std::memory_order_relaxed);
    g_unhideEnabled.store(Wh_GetIntSetting(L"taskbar.unhide_taskbar") != 0, std::memory_order_relaxed);
    g_unhideDurationMs.store(MacClampInt(Wh_GetIntSetting(L"taskbar.unhide_duration_ms"), 0, 5000),
                             std::memory_order_relaxed);

    // Diagnostics
    g_telemetry.store(Wh_GetIntSetting(L"diagnostics.performance_telemetry") != 0, std::memory_order_relaxed);
    g_closeLog.store(Wh_GetIntSetting(L"diagnostics.close_log") != 0, std::memory_order_relaxed);
    g_specialLog.store(Wh_GetIntSetting(L"diagnostics.special_log") != 0, std::memory_order_relaxed);

    // Exclusion entries are normalised once here, so matching compares plain
    // base names: "Foo.exe", "foo", " foo.exe " and "C:\Apps\Foo.exe" all mean
    // the same thing, and "my.app.exe" keeps its inner dot.
    std::vector<std::wstring> excluded;
    for (int i = 0;; i++) {
        PCWSTR prog = Wh_GetStringSetting(L"excluded_programs[%d]", i);
        const bool has = *prog;
        if (has) {
            std::wstring s = MacGenieNormalizeEntry(prog, true);
            if (!s.empty()) excluded.push_back(std::move(s));
        }
        Wh_FreeStringSetting(prog);
        if (!has) break;
    }
    std::vector<std::wstring> classes;
    for (int i = 0;; i++) {
        PCWSTR cls = Wh_GetStringSetting(L"special.excluded_classes[%d]", i);
        const bool has = *cls;
        if (has) {
            std::wstring s = MacGenieNormalizeEntry(cls, false);
            if (!s.empty()) classes.push_back(std::move(s));
        }
        Wh_FreeStringSetting(cls);
        if (!has) break;
    }
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_ExcludedPrograms.swap(excluded);
        g_SpecialExcludedClasses.swap(classes);
        g_specialExcludedCount.store((int)g_SpecialExcludedClasses.size(), std::memory_order_relaxed);
    }
}

// -------------------------------------------------------------------------
// V3.8: effect resolution. Random draws from a shuffle bag (every effect once
// per cycle, never the same effect twice across a cycle boundary), one bag per
// selector. The effect a window minimized with is stored on the window
// (property "GenieFx") so its restore plays the same effect in reverse, even
// with Random on or after the setting changed.
// -------------------------------------------------------------------------
struct FxBag { int order[FX_COUNT]; int pos; int last; uint32_t rng; };
static FxBag g_fxBags[3];          // minimize, close, special
static std::mutex g_fxBagMutex;

static int MacFxDrawRandom(int bagIndex) {
    std::lock_guard<std::mutex> lock(g_fxBagMutex);
    FxBag& b = g_fxBags[bagIndex];
    if (!b.rng) {
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        b.rng = (uint32_t)q.QuadPart ^ (GetCurrentProcessId() * 2654435761u) ^ ((uint32_t)bagIndex * 0x9E3779B9u);
        if (!b.rng) b.rng = 1;
        b.pos = FX_COUNT;
        b.last = -1;
    }
    if (b.pos >= FX_COUNT) {
        for (int i = 0; i < FX_COUNT; i++) b.order[i] = i;
        for (int i = FX_COUNT - 1; i > 0; i--) {
            b.rng ^= b.rng << 13;
            b.rng ^= b.rng >> 17;
            b.rng ^= b.rng << 5;
            const int j = (int)(b.rng % (uint32_t)(i + 1));
            std::swap(b.order[i], b.order[j]);
        }
        if (b.order[0] == b.last) std::swap(b.order[0], b.order[FX_COUNT - 1]);
        b.pos = 0;
    }
    b.last = b.order[b.pos++];
    return b.last;
}

static int MacFxResolve(int ctx, HWND hWnd) {
    int sel = FX_GENIE;
    int bag = 0;
    switch (ctx) {
    case CTX_RESTORE: {
        const INT_PTR stored = hWnd ? (INT_PTR)GetPropW(hWnd, L"GenieFx") : 0;
        if (stored > 0 && stored <= FX_COUNT) {
            RemovePropW(hWnd, L"GenieFx");
            return (int)stored - 1;
        }
        sel = g_fxMinimize.load(std::memory_order_relaxed);
        break;
    }
    case CTX_MINIMIZE:
        sel = g_fxMinimize.load(std::memory_order_relaxed);
        break;
    case CTX_CLOSE:
        sel = g_fxClose.load(std::memory_order_relaxed);
        bag = 1;
        break;
    case CTX_SPECIAL:
        sel = g_fxSpecial.load(std::memory_order_relaxed);
        if (sel == FX_SAME_AS_CLOSE) return MacFxResolve(CTX_CLOSE, hWnd);
        bag = 2;
        break;
    default:
        break;
    }
    if (sel == FX_RANDOM) sel = MacFxDrawRandom(bag);
    if (sel < 0 || sel >= FX_COUNT) sel = (ctx == CTX_CLOSE) ? FX_BLACKHOLE : FX_GENIE;
    return sel;
}

static void MacFxRememberForRestore(HWND hWnd, int fx) {
    if (hWnd && fx >= 0 && fx < FX_COUNT) SetPropW(hWnd, L"GenieFx", (HANDLE)(INT_PTR)(fx + 1));
}

// The context's duration setting scaled by the effect's pacing.
static int MacFxDurationMs(int fx, int baseMs) {
    const int pct = (fx >= 0 && fx < FX_COUNT) ? kFxInfo[fx].durPct : 100;
    return MacClampInt((int)((long long)baseMs * pct / 100), 50, 5000);
}

// -------------------------------------------------------------------------
// Process & Exclusion Cache
// -------------------------------------------------------------------------
// Returns the lower-cased executable base name without its ".exe" (the last
// extension only - "my.app.exe" -> "my.app"). An empty result (OpenProcess
// refused: elevated or protected process) is NOT cached, so a later call can
// still succeed and exclusions / targeting are not silently wrong for the
// window's whole lifetime.
std::wstring GetProcessNameCached(HWND hWnd, std::wstring* pathOut = nullptr) {
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hWnd, &ownerPid);
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_ProcessNameCache.find(hWnd);
        if (it != g_ProcessNameCache.end() && it->second.pid == ownerPid) {
            if (pathOut) *pathOut = it->second.path;
            return it->second.name;
        }
    }
    std::wstring procNameLower = L"";
    std::wstring pathLower = L"";
    if (ownerPid) {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ownerPid);
        if (hProc) {
            WCHAR exePath[MAX_PATH] = {0};
            DWORD exePathLen = MAX_PATH;
            if (QueryFullProcessImageNameW(hProc, 0, exePath, &exePathLen)) {
                pathLower = exePath;
                std::transform(pathLower.begin(), pathLower.end(), pathLower.begin(), ::towlower);
                WCHAR* name = wcsrchr(exePath, L'\\');
                procNameLower = name ? (name + 1) : exePath;
                std::transform(procNameLower.begin(), procNameLower.end(), procNameLower.begin(), ::towlower);
                size_t dotPos = procNameLower.rfind(L'.');
                if (dotPos != std::wstring::npos && dotPos > 0 &&
                    procNameLower.compare(dotPos, std::wstring::npos, L".exe") == 0) {
                    procNameLower.resize(dotPos);
                }
            }
            CloseHandle(hProc);
        }
    }
    if (!procNameLower.empty()) {
        g_cachesTouched.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_ProcessNameCache[hWnd] = { ownerPid, procNameLower, pathLower };
    }
    if (pathOut) *pathOut = pathLower;
    return procNameLower;
}

// Periodic sweep of the HWND-keyed caches. Their only per-window cleanup is the
// WM_DESTROY branch of the DefWindowProcW hook, and plenty of windows never
// route WM_DESTROY through DefWindowProc - those entries used to live for the
// process lifetime, growing the maps and, worse, ambushing recycled HWNDs with
// stale data (a suppressed launch animation, a genie aimed at a dead window's
// dock slot). Runs at most every 30 seconds, from the animation-start path.
#define CACHE_SWEEP_INTERVAL_MS 30000
static ULONGLONG g_lastCacheSweep = 0;   // guarded by g_CacheMutex

static void MacGenieCacheEraseLocked(HWND hWnd);

static void MacGenieSweepWindowCachesLocked() {
    ULONGLONG now = GetTickCount64();
    if (now - g_lastCacheSweep < CACHE_SWEEP_INTERVAL_MS) return;
    g_lastCacheSweep = now;
    for (auto it = g_ProcessNameCache.begin(); it != g_ProcessNameCache.end();) {
        if (!IsWindow(it->first)) it = g_ProcessNameCache.erase(it); else ++it;
    }
    for (auto it = g_IconPositions.begin(); it != g_IconPositions.end();) {
        if (!IsWindow(it->first)) it = g_IconPositions.erase(it); else ++it;
    }
    for (auto it = g_LaunchSeen.begin(); it != g_LaunchSeen.end();) {
        if (!IsWindow(*it)) it = g_LaunchSeen.erase(it); else ++it;
    }
    for (auto it = g_LaunchLayered.begin(); it != g_LaunchLayered.end();) {
        if (!IsWindow(it->first)) it = g_LaunchLayered.erase(it); else ++it;
    }
    for (auto it = g_ProcessUiaNegativeUntil.begin(); it != g_ProcessUiaNegativeUntil.end();) {
        if (now >= it->second) it = g_ProcessUiaNegativeUntil.erase(it); else ++it;
    }
    // Snapshots of dead windows used to wait for the next falling insert.
    std::vector<HWND> deadSnaps;
    for (auto& pair : g_SnapshotCache) {
        if (!IsWindow(pair.first)) deadSnaps.push_back(pair.first);
    }
    for (HWND h : deadSnaps) MacGenieCacheEraseLocked(h);
}

bool MacGenieIsExcluded(HWND hWnd) {
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_ExcludedPrograms.empty()) return false;
    }
    std::wstring baseNoExt = GetProcessNameCached(hWnd);
    if (baseNoExt.empty()) return false;

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (const std::wstring& e : g_ExcludedPrograms) {
        if (e == baseNoExt) return true;
    }
    return false;
}

// -------------------------------------------------------------------------
// DWM & Window State Helpers
// -------------------------------------------------------------------------
void MacGenieSetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

void MacGenieSetCloak(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}

// Undoes the alpha-0 hide of the launch path. Only the WS_EX_LAYERED bit the
// mod added is cleared - writing the whole saved ex-style back (as V3 did)
// discarded any bit the application changed in the meantime (a topmost
// toggle, a WS_EX_LAYERED it set itself).
static void MacGenieRestoreLayered(HWND hWnd, LONG_PTR originalExStyle) {
    if (!IsWindow(hWnd)) return;
    SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
    if (!(originalExStyle & WS_EX_LAYERED)) {
        LONG_PTR cur = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
        if (cur & WS_EX_LAYERED) SetWindowLongPtrW(hWnd, GWL_EXSTYLE, cur & ~WS_EX_LAYERED);
    }
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    g_LaunchLayered.erase(hWnd);
}

static void MacGenieUndoRisingHide(HWND hWnd, LONG_PTR originalExStyle, BOOL cloakHidden) {
    if (cloakHidden) {
        MacGenieSetCloak(hWnd, FALSE);
    } else {
        MacGenieRestoreLayered(hWnd, originalExStyle);
    }
    MacGenieSetDwmTransitions(hWnd, TRUE);
}

// Is the window cloaked by the shell (another virtual desktop, suspended UWP)?
// The mod's own cloak sets DWM_CLOAKED_APP and must not count.
static bool MacGenieIsShellCloaked(HWND hWnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        return (cloaked & DWM_CLOAKED_SHELL) != 0;
    }
    return false;
}

static bool MacGenieIsAnyCloaked(HWND hWnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        return cloaked != 0;
    }
    return false;
}

static inline bool MacGenieIsOwnThreadWindow(HWND hWnd) {
    return GetWindowThreadProcessId(hWnd, NULL) == GetCurrentThreadId();
}

static inline uint32_t MacPremultiplyBlend(uint32_t px, float a) {
    if (a >= 0.997f) return px;
    if (a <= 0.003f) return 0;
    uint32_t a256 = (uint32_t)(a * 256.0f);
    uint32_t origA = (px >> 24) & 0xFF;
    if (origA == 0 && (px & 0xFFFFFF) != 0) origA = 255;
    uint32_t newA = (origA * a256) >> 8;
    uint32_t r = (((px >> 16) & 0xFF) * a256) >> 8;
    uint32_t g = (((px >>  8) & 0xFF) * a256) >> 8;
    uint32_t b = ( (px        & 0xFF) * a256) >> 8;
    return (newA << 24) | (r << 16) | (g << 8) | b;
}

static inline uint32_t MacAlphaBlendPixels(uint32_t dst, uint32_t src_premult) {
    if (dst == 0) return src_premult;
    uint32_t srcA = src_premult >> 24;
    if (srcA == 255) return src_premult;
    if (srcA == 0)   return dst;
    uint32_t invA = 255 - srcA;
    uint32_t outA = srcA + ((((dst >> 24)        ) * invA) >> 8);
    uint32_t outR = ((src_premult >> 16) & 0xFF) + ((((dst >> 16) & 0xFF) * invA) >> 8);
    uint32_t outG = ((src_premult >>  8) & 0xFF) + ((((dst >>  8) & 0xFF) * invA) >> 8);
    uint32_t outB = ( src_premult        & 0xFF) + ((( dst        & 0xFF) * invA) >> 8);
    return (outA << 24) | (outR << 16) | (outG << 8) | outB;
}

// V4: one spring model for every bounce (the genie / Black Hole / Scale restore).
// An underdamped spring released at its rest position with some velocity:
//   x(t) = A * exp(-zeta * w0 * t) * sin(wd * t),  wd = w0 * sqrt(1 - zeta^2),
// with t normalised to the bounce duration. 3.9's ad-hoc curve was this spring
// with zeta ~ 0.55 (decay 4.5, wd 2.2 pi), but it ended off rest: the last frame
// snapped by up to 0.06 % of the window. Here wd puts the end exactly on a zero
// crossing, and a short taper takes the remaining velocity out, so the bounce ends
// at exactly the window's size. `strength` keeps 3.9's meaning (0-100: an
// overshoot of up to 18 % of the size).
static inline float MacSpring(float t, float amplitude, float zeta, float halfCycles) {
    if (t <= 0.0f || t >= 1.0f || amplitude == 0.0f) return 0.0f;
    const float wd = PI * halfCycles;                     // ends on a zero crossing
    const float w0 = wd / sqrtf(1.0f - zeta * zeta);
    const float env = expf(-zeta * w0 * t);
    const float taper = 1.0f - powf(t, 6.0f);              // no residual velocity
    return amplitude * env * sinf(wd * t) * taper;
}

static inline float MacSpringBounce(float tB, float strength) {
    const float impact = (strength / 100.0f) * 0.18f;
    // zeta 0.55 and two half-cycles: the 3.9 curve's feel (decay ~4.5), ending at rest.
    return 1.0f + MacSpring(tB, impact, 0.55f, 2.0f) * 1.08f;
}

// Frame pacer, locked to the compositor.
//
// Every engine presents through UpdateLayeredWindow, which DWM picks up once per
// vblank - a second update inside the same interval simply overwrites the first
// before it is ever displayed. The old pacer ran a free-running timer at twice
// the refresh rate, so roughly half of every animation's frames were rendered
// and thrown away, and the surviving half sampled the animation at a phase that
// drifted against vblank (up to half an interval of jitter, i.e. visible
// micro-judder).
//
// So the pacer now targets the true refresh rate instead of twice it, using the
// high-resolution waitable timer.
//
// It deliberately does NOT pace with DwmFlush, even though that would align the
// phase to vblank exactly. DwmFlush parks the calling thread in a cross-process
// wait on DWM, and this thread both owns the ghost window and, on the close
// path, lives inside the application process that is tearing down - a
// window-owning thread that blocks cannot pump messages, and blocking there
// during teardown cost the Black Hole close animation entirely. A thread-local
// timer wait has neither problem. DwmFlush stays where it was before: once on
// the first frame (to guarantee the ghost is composited before the real window
// is hidden) and once on the last.
//
// V3.7: absolute deadlines. V3.6 waited (interval - time spent rendering) from
// the start of every frame, so each timer wake-up's lateness was added to the
// period and the sampling phase walked against vblank. The deadline now advances
// by exactly one interval per frame (frame k is due at t0 + k * interval); a
// frame that overran its deadline rebases to "now" instead of bursting to catch
// up. Progress is time-based, so the picture is unchanged - only its cadence is.
//
// V4: deadlines are anchored on a vblank. The first-frame flush returns right
// after a composition; that instant is the anchor, and frame k is due at
// anchor + k * interval with the monitor's EXACT rate (QueryDisplayConfig's
// rational refresh, 59.94 / 143.98 / 239.76 Hz, instead of the integer
// dmDisplayFrequency). A frame that overran skips to the next vblank-aligned
// deadline instead of rebasing to "now" (3.9 lost the alignment for the rest of
// the animation after its first overrun). When a frame cannot fit one refresh,
// the pacer steps every 2 (then 3, 4) refreshes - an even 120 fps on 240 Hz
// instead of a 1-2-1-2 alternation, which reads as stutter.
struct MacGenieFramePacer {
    HANDLE hTimer = nullptr;
    double intervalMs = 1000.0 / 60.0;
    LONGLONG freq = 0;            // QPC ticks per second
    LONGLONG intervalTicks = 0;
    LONGLONG nextDeadline = 0;    // QPC time the next frame may start
    bool highRes = false;         // CREATE_WAITABLE_TIMER_HIGH_RESOLUTION worked
    bool heldTimerRes = false;    // fallback: holds a timeBeginPeriod(1) ref
    LONGLONG anchor = 0;          // V4: a vblank-aligned instant
    LONGLONG k = 0;               // V4: index of the next deadline
    LONGLONG lastWake = 0;        // V4: when the current frame started
    int stride = 1;               // V4: refreshes per frame
    int strideMax = 1;
    double cost[8] = {};          // V4: the last frames' cost (ms)
    int costN = 0;
};

// 1ms scheduler resolution, held only while an animation is actually running.
// The mod used to call timeBeginPeriod(1) in Wh_ModInit and hold it for its
// whole lifetime - in every process, since the mod loads everywhere - which
// pins the system timer at 1ms around the clock. Windows refcounts
// begin/end pairs, so the edge-triggered refcount below stays correct even
// when animations overlap or interleave across threads.
static std::atomic<int> g_timerResRefs{0};

static void MacGenieBeginTimerRes() {
    if (g_timerResRefs.fetch_add(1, std::memory_order_relaxed) == 0) {
        timeBeginPeriod(1);
    }
}

static void MacGenieEndTimerRes() {
    if (g_timerResRefs.fetch_sub(1, std::memory_order_relaxed) == 1) {
        timeEndPeriod(1);
    }
}

// The refresh rate comes from the monitor the animation actually plays on.
// DwmGetCompositionTimingInfo(NULL) reports the primary's rate only, so a 60 Hz
// secondary next to a 144 Hz primary was paced at 144 (more than half of its
// frames rendered and discarded), and the reverse case was choppy.
// The monitor's exact refresh rate (a rational number) from the display
// configuration, cached for a few seconds per display. 0 when unknown.
struct MacRefreshCacheEntry { std::wstring device; double hz; ULONGLONG tick; };
static std::mutex g_refreshCacheMutex;
static std::vector<MacRefreshCacheEntry> g_refreshCache;

static double MacGenieExactRefreshHz(HMONITOR hMon) {
    MONITORINFOEXW mix = {};
    mix.cbSize = sizeof(mix);
    if (!hMon || !GetMonitorInfoW(hMon, &mix)) return 0.0;
    const ULONGLONG now = GetTickCount64();
    {
        std::lock_guard<std::mutex> lock(g_refreshCacheMutex);
        for (const auto& e : g_refreshCache) {
            if (e.device == mix.szDevice && now - e.tick < 10000) return e.hz;
        }
    }
    double hz = 0.0;
    UINT32 nPaths = 0, nModes = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &nPaths, &nModes) == ERROR_SUCCESS &&
        nPaths > 0 && nPaths < 64 && nModes < 256) {
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(nPaths);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(nModes ? nModes : 1);
        if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &nPaths, paths.data(), &nModes, modes.data(), nullptr) ==
            ERROR_SUCCESS) {
            for (UINT32 i = 0; i < nPaths && hz == 0.0; i++) {
                DISPLAYCONFIG_SOURCE_DEVICE_NAME src = {};
                src.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
                src.header.size = sizeof(src);
                src.header.adapterId = paths[i].sourceInfo.adapterId;
                src.header.id = paths[i].sourceInfo.id;
                if (DisplayConfigGetDeviceInfo(&src.header) != ERROR_SUCCESS) continue;
                if (wcscmp(src.viewGdiDeviceName, mix.szDevice) != 0) continue;
                const DISPLAYCONFIG_RATIONAL r = paths[i].targetInfo.refreshRate;
                if (r.Denominator) {
                    const double v = (double)r.Numerator / (double)r.Denominator;
                    if (v >= 23.0 && v <= 1000.0) hz = v;
                }
            }
        }
    }
    std::lock_guard<std::mutex> lock(g_refreshCacheMutex);
    for (auto& e : g_refreshCache) {
        if (e.device == mix.szDevice) { e.hz = hz; e.tick = now; return hz; }
    }
    if (g_refreshCache.size() < 16) g_refreshCache.push_back({ mix.szDevice, hz, now });
    return hz;
}

static void MacGenieCreateFramePacer(MacGenieFramePacer* pacer, HMONITOR hMon) {
    pacer->intervalMs = 1000.0 / 60.0;
    bool haveRate = false;
    {
        const double hz = MacGenieExactRefreshHz(hMon);
        if (hz >= 23.0) {
            pacer->intervalMs = 1000.0 / hz;
            haveRate = true;
        }
    }
    if (!haveRate && hMon) {
        MONITORINFOEXW mix = {};
        mix.cbSize = sizeof(mix);
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        if (GetMonitorInfoW(hMon, &mix) &&
            EnumDisplaySettingsW(mix.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
            dm.dmDisplayFrequency >= 50 && dm.dmDisplayFrequency <= 500) {
            pacer->intervalMs = 1000.0 / (double)dm.dmDisplayFrequency;
            haveRate = true;
        }
    }
    if (!haveRate) {
        DWM_TIMING_INFO ti = {};
        ti.cbSize = sizeof(ti);
        if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &ti)) && ti.rateRefresh.uiDenominator) {
            double hz = (double)ti.rateRefresh.uiNumerator / ti.rateRefresh.uiDenominator;
            if (hz >= 50.0 && hz <= 500.0) {
                pacer->intervalMs = 1000.0 / hz;
            }
        }
    }
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    pacer->freq = f.QuadPart;
    pacer->intervalTicks = (LONGLONG)(pacer->intervalMs * (double)f.QuadPart / 1000.0 + 0.5);
    if (pacer->intervalTicks < 1) pacer->intervalTicks = 1;

    pacer->hTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_MODIFY_STATE | SYNCHRONIZE);
    pacer->highRes = (pacer->hTimer != nullptr);
    if (!pacer->hTimer) {
        pacer->hTimer = CreateWaitableTimerExW(nullptr, nullptr, 0,
                                               TIMER_MODIFY_STATE | SYNCHRONIZE);
    }
    // A high-resolution timer needs no help from the scheduler tick. Only the
    // fallback (pre-1803 Windows, or a refused flag) pins the 1 ms resolution -
    // V3.6 pinned it for every animation.
    if (!pacer->highRes) {
        MacGenieBeginTimerRes();
        pacer->heldTimerRes = true;
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    pacer->nextDeadline = now.QuadPart + pacer->intervalTicks;
}

static void MacGenieDestroyFramePacer(MacGenieFramePacer* pacer) {
    if (pacer->hTimer) {
        CloseHandle(pacer->hTimer);
        pacer->hTimer = nullptr;
    }
    if (pacer->heldTimerRes) {
        MacGenieEndTimerRes();
        pacer->heldTimerRes = false;
    }
}

// Call right after the first-frame DwmFlush: the thread has just been released
// at a vblank, which becomes the anchor; the next frame is due one interval later.
static void MacGeniePacerRebase(MacGenieFramePacer* pacer) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    pacer->anchor = now.QuadPart;
    pacer->k = 0;
    pacer->lastWake = now.QuadPart;
    pacer->nextDeadline = now.QuadPart + pacer->intervalTicks;
}

// Sleeps until the next frame is due (V4: on the vblank grid, every `stride`
// refreshes), then records the wake-up as the start of the next frame.
static void MacGeniePaceFrame(MacGenieFramePacer* pacer) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    if (!pacer->anchor) {
        // Not anchored yet (no first-frame flush happened): behave like 3.9.
        pacer->anchor = now.QuadPart;
        pacer->k = 0;
    }
    // Frame cost = wake-up -> here (message drain, render, present).
    if (pacer->lastWake && pacer->freq) {
        const double ms = (double)(now.QuadPart - pacer->lastWake) * 1000.0 / (double)pacer->freq;
        pacer->cost[pacer->costN % 8] = ms;
        pacer->costN++;
        if (pacer->costN >= 8 && (pacer->costN % 8) == 0) {
            // ~p90 of the last 8: the second largest.
            double a = 0.0, b = 0.0;
            for (double c : pacer->cost) {
                if (c > a) { b = a; a = c; } else if (c > b) { b = c; }
            }
            const double iv = pacer->intervalMs;
            if (b > 0.85 * iv * pacer->stride && pacer->stride < 4) {
                pacer->stride++;
            } else if (pacer->stride > 1 && b < 0.70 * iv * (pacer->stride - 1)) {
                pacer->stride--;
            }
            if (pacer->stride > pacer->strideMax) pacer->strideMax = pacer->stride;
        }
    }
    pacer->k += pacer->stride;
    LONGLONG deadline = pacer->anchor + pacer->k * pacer->intervalTicks;
    if (deadline <= now.QuadPart) {
        // Overran: the next deadline on the vblank grid, never a burst.
        const LONGLONG elapsed = now.QuadPart - pacer->anchor;
        pacer->k = elapsed / pacer->intervalTicks + 1;
        deadline = pacer->anchor + pacer->k * pacer->intervalTicks;
    }
    pacer->nextDeadline = deadline;
    for (;;) {
        QueryPerformanceCounter(&now);
        const LONGLONG remaining = pacer->nextDeadline - now.QuadPart;
        if (remaining <= 0) break;
        if (!pacer->hTimer) {
            Sleep(1);
            continue;
        }
        // 100 ns units, rounded up so an early wake never spins.
        LONGLONG due100ns = (remaining * 10000000LL + pacer->freq - 1) / pacer->freq;
        if (due100ns < 1) due100ns = 1;
        LARGE_INTEGER due;
        due.QuadPart = -due100ns;
        if (!SetWaitableTimer(pacer->hTimer, &due, 0, nullptr, nullptr, FALSE) ||
            WaitForSingleObject(pacer->hTimer, 100) != WAIT_OBJECT_0) {
            // Timer unusable: degrade to a yielding wait instead of spinning.
            CloseHandle(pacer->hTimer);
            pacer->hTimer = nullptr;
            if (!pacer->heldTimerRes) {
                MacGenieBeginTimerRes();
                pacer->heldTimerRes = true;
            }
        }
        // The timer may fire a fraction early: loop re-checks QPC and re-arms.
    }
    pacer->lastWake = now.QuadPart;
}

// -------------------------------------------------------------------------
// V4: the animation clock, shared by every engine.
//  - It starts on the frame that is ON SCREEN: frame 0 (progress 0) is drawn,
//    composited (the first-frame flush), and only then does time start, so
//    frame 1 is exactly one refresh into the animation. 3.9 started the clock
//    before frame 0, and the 2-4 refreshes frame 0 took to appear made the first
//    visible step 2-4x bigger than the others in every animation (your log:
//    late=1, period-max 9-17 ms).
//  - A reversal remaps time so the picture continues from the same shape:
//    falling at progress p becomes rising at 1 - p, and back.
// -------------------------------------------------------------------------
struct MacAnimClock {
    LONGLONG freq = 1;
    LONGLONG start = 0;
};

static void MacAnimClockInit(MacAnimClock* c) {
    LARGE_INTEGER f, q;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&q);
    c->freq = f.QuadPart > 0 ? f.QuadPart : 1;
    c->start = q.QuadPart;
}

static inline double MacAnimClockMs(const MacAnimClock* c, LONGLONG now) {
    return (double)(now - c->start) * 1000.0 / (double)c->freq;
}

static void MacAnimClockStartNow(MacAnimClock* c) {
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    c->start = q.QuadPart;
}

// Applies a direction change a hook asked for. Returns true when the animation
// turned around; `now` is the frame's start time.
static bool MacAnimApplyReversal(MacAnimClock* c, MacGenieAnimData* d, double animDurMs, LONGLONG now) {
    if (!d->reversible) return false;
    const LONG want = InterlockedExchange(&d->wantDir, -1);
    if (want < 0) return false;
    const BOOL rising = want != 0;
    if (rising == d->isRising) return false;
    double p = animDurMs > 0.0 ? MacAnimClockMs(c, now) / animDurMs : 1.0;
    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;                     // (a restore's bounce counts as its end)
    const double newMs = animDurMs * (1.0 - p);
    c->start = now - (LONGLONG)llround(newMs * (double)c->freq / 1000.0);
    d->isRising = rising;
    if (rising) {
        // The hook restored the window under a cloak: reveal it at the end.
        d->hiddenByCloak = TRUE;
        d->revealDone = FALSE;
        d->undoHideOnFallEnd = FALSE;
        d->deferredMinimize = FALSE;
        d->cacheSnapshotOnFinish = FALSE;
        d->ctx = CTX_RESTORE;
    } else {
        // The hook minimized the window natively while it was still hidden for
        // the restore: that hide is lifted when the minimize ends.
        d->undoHideOnFallEnd = TRUE;
        d->revealDone = FALSE;
        d->cacheSnapshotOnFinish = TRUE;
        d->ctx = CTX_MINIMIZE;
    }
    if (d->telOn) d->tel.reversals++;
    return true;
}

// Drains the render thread's message queue. Sent messages are only delivered
// while a thread is inside a message API, and every ghost-owning thread used to
// sit in a timer wait for the whole animation: a cross-thread SendMessage to the
// ghost (accessibility clients, shell broadcasts) blocked its sender until the
// animation ended. Bounded so a message flood can never stall a frame.
static void MacGenieDrainMessages() {
    MSG msg;
    for (int i = 0; i < 32 && PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE); i++) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

// Telemetry probes. All of them are no-ops unless the per-animation flag is set.
static inline LONGLONG MacTelMark(const MacGenieAnimData* d) {
    if (!d->telOn) return 0;
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    return q.QuadPart;
}

static void MacTelFrame(MacGenieAnimData* d, LONGLONG frameStart, LONGLONG renderEnd,
                        LONGLONG presentEnd) {
    if (!d->telOn) return;
    MacGenieTelemetry& t = d->tel;
    const double f = (double)t.qpcFreq / 1000.0;   // ticks per ms
    if (f <= 0.0) return;
    const double render = (double)(renderEnd - frameStart) / f;
    const double present = (double)(presentEnd - renderEnd) / f;
    t.renderSumMs += render;   if (render > t.renderMaxMs) t.renderMaxMs = render;
    t.presentSumMs += present; if (present > t.presentMaxMs) t.presentMaxMs = present;
    if (t.qLastFrameStart) {
        const double period = (double)(frameStart - t.qLastFrameStart) / f;
        if (t.frames == 1) {
            // frame 0 -> 1 includes the first-frame flush: reported apart (V4).
            t.firstGapMs = period;
        } else {
            if (period > t.periodMaxMs) t.periodMaxMs = period;
            if (t.intervalMs > 0.0) {
                if (period > t.intervalMs * 1.5) t.late++;
                int q = (int)(period / t.intervalMs + 0.5);
                if (q < 1) q = 1;
                if (q > 3) q = 3;
                t.cadence[q]++;
            }
        }
    }
    t.qLastFrameStart = frameStart;
    t.frames++;
}

static void MacTelFirstFrame(MacGenieAnimData* d) {
    if (d->telOn && !d->tel.qFirstFrame) d->tel.qFirstFrame = MacTelMark(d);
}

// V4 latency stages: the render thread is set up (ghost, canvas, pool, precalc),
// and frame 0 has been handed to the compositor (before the first-frame flush).
static void MacTelSetupDone(MacGenieAnimData* d) {
    if (d->telOn && !d->tel.qSetupDone) d->tel.qSetupDone = MacTelMark(d);
}

static void MacTelFrame0Shown(MacGenieAnimData* d) {
    if (d->telOn && !d->tel.qFrame0Shown) d->tel.qFrame0Shown = MacTelMark(d);
}

static void MacTelBeginThread(MacGenieAnimData* d, const wchar_t* engine,
                              const MacGenieFramePacer* pacer) {
    if (!d->telOn) return;
    d->tel.engine = engine;
    d->tel.intervalMs = pacer ? pacer->intervalMs : 0.0;
    if (!d->tel.qThreadStart) d->tel.qThreadStart = MacTelMark(d);
}

static void MacTelLog(const MacGenieAnimData* d) {
    if (!d->telOn) return;
    const MacGenieTelemetry& t = d->tel;
    const double f = (double)t.qpcFreq / 1000.0;
    if (f <= 0.0) return;
    const double capture = t.qCaptureDone ? (double)(t.qCaptureDone - t.qHookEntry) / f : 0.0;
    const double threadStart = (t.qThreadStart && t.qCaptureDone)
        ? (double)(t.qThreadStart - t.qCaptureDone) / f : 0.0;
    const double firstFrame = t.qFirstFrame ? (double)(t.qFirstFrame - t.qHookEntry) / f : 0.0;
    const double setup = (t.qSetupDone && t.qThreadStart) ? (double)(t.qSetupDone - t.qThreadStart) / f : 0.0;
    const double frame0 = (t.qFrame0Shown && t.qSetupDone) ? (double)(t.qFrame0Shown - t.qSetupDone) / f : 0.0;
    const double flush = (t.qFirstFrame && t.qFrame0Shown) ? (double)(t.qFirstFrame - t.qFrame0Shown) / f : 0.0;
    const double n = t.frames ? (double)t.frames : 1.0;
    const wchar_t* kind = d->special ? L"special"
                        : d->isClosing ? L"close"
                        : d->isLaunch ? (d->isRising ? L"launch" : L"launch>minimize")
                        : (d->isRising ? L"restore" : L"minimize");
    Wh_Log(L"[perf] %s %s hwnd=%p src=%dx%d interval=%.2fms | click->snapshot=%.1fms (taskbar lookup %.1fms) "
           L"-> thread=%.1fms setup=%.1fms frame0=%.1fms flush=%.1fms | click->first-frame=%.1fms | "
           L"frames=%u late=%u first-gap=%.1fms period-max=%.1fms cadence=%u/%u/%u stride-max=%d reversals=%d | "
           L"render avg/max=%.2f/%.2fms present avg/max=%.2f/%.2fms",
           t.engine, kind, (void*)d->hRealWnd, d->width, d->height, t.intervalMs,
           capture, t.uiaWaitMs, threadStart, setup, frame0, flush, firstFrame,
           t.frames, t.late, t.firstGapMs, t.periodMaxMs, t.cadence[1], t.cadence[2], t.cadence[3],
           t.strideMax, t.reversals,
           t.renderSumMs / n, t.renderMaxMs, t.presentSumMs / n, t.presentMaxMs);
}

// Presents a frame of a ghost window. When a dirty rect is given only that
// area is uploaded to the compositor (UpdateLayeredWindowIndirect + prcDirty,
// derrunk's design); the first frame and any frame whose constant alpha
// changed pass NULL to refresh the whole surface.
static void MacGeniePresent(HWND hGhost, HDC hScreenDC, POINT ptDst, SIZE sz,
                            HDC hdcSrc, BYTE constAlpha, const RECT* dirty) {
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, constAlpha, AC_SRC_ALPHA };
    UPDATELAYEREDWINDOWINFO ulw = {};
    ulw.cbSize = sizeof(ulw);
    ulw.hdcDst = hScreenDC;
    ulw.pptDst = &ptDst;
    ulw.psize = &sz;
    ulw.hdcSrc = hdcSrc;
    ulw.pptSrc = &ptSrc;
    ulw.crKey = 0;
    ulw.pblend = &bf;
    ulw.dwFlags = ULW_ALPHA;
    RECT rc;
    if (dirty) {
        rc = *dirty;
        if (rc.left < 0) rc.left = 0;
        if (rc.top < 0) rc.top = 0;
        if (rc.right > sz.cx) rc.right = sz.cx;
        if (rc.bottom > sz.cy) rc.bottom = sz.cy;
        if (rc.right <= rc.left || rc.bottom <= rc.top) {
            rc.left = 0; rc.top = 0; rc.right = 1; rc.bottom = 1;
        }
        ulw.prcDirty = &rc;
    }
    if (!UpdateLayeredWindowIndirect(hGhost, &ulw)) {
        // Older shells reject prcDirty for a not-yet-presented surface; fall
        // back to a whole-surface update rather than dropping the frame.
        UpdateLayeredWindow(hGhost, hScreenDC, &ptDst, &sz, hdcSrc, &ptSrc, 0, &bf, ULW_ALPHA);
    }
}

// Shows a genie ghost for the first time. Instead of floating above the taskbar
// (WS_EX_TOPMOST over everything, so the funnel tip painted over the icons) the
// ghost is slotted directly below the taskbar window in z-order, as
// Potassiumuncher's engine did. The ghost keeps WS_EX_TOPMOST, so it still
// covers every application window; only the taskbar sits above it.
static void MacGenieShowGhostUnderTaskbar(HWND hGhost, HMONITOR hMon) {
    // Ghost windows go through the ORIGINAL APIs so they never re-enter the
    // mod's own hooks (a stash, a launch check) from the animation thread.
    SetWindowPos_t pfnSetWindowPos = SetWindowPos_Original ? SetWindowPos_Original : SetWindowPos;
    ShowWindow_t pfnShowWindow = ShowWindow_Original ? ShowWindow_Original : ShowWindow;
    SetPropW(hGhost, L"NonRudeHWND", (HANDLE)TRUE);
    HWND hTray = FindTaskbarForMonitor(hMon);
    if (hTray && IsWindow(hTray) &&
        pfnSetWindowPos(hGhost, hTray, 0, 0, 0, 0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW)) {
        return;
    }
    pfnShowWindow(hGhost, SW_SHOWNOACTIVATE);
}

static void MacGenieShowGhostTopmost(HWND hGhost) {
    ShowWindow_t pfnShowWindow = ShowWindow_Original ? ShowWindow_Original : ShowWindow;
    pfnShowWindow(hGhost, SW_SHOWNOACTIVATE);
}


// V3.8: ghost style. A special window's ghost is topmost only if its source
// was: it takes the source's place in the z-order (MacGenieShowGhostForData),
// so it never paints over a window that was in front of the popup / panel.
static DWORD MacGenieGhostExStyle(const MacGenieAnimData* d) {
    DWORD ex = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
    if (!d->special || d->ghostTopmost) ex |= WS_EX_TOPMOST;
    return ex;
}

// First show of any engine's ghost. Main windows: under the taskbar (genie,
// aimed particles) or topmost (in-place effects), as in 3.7. Special windows:
// the source's own z-slot, right below the window that was above it.
static void MacGenieShowGhostForData(HWND hGhost, const MacGenieAnimData* d, bool underTaskbar) {
    SetPropW(hGhost, L"UltimateGenieGhost", (HANDLE)1);
    if (d->special) {
        SetWindowPos_t pfnSetWindowPos = SetWindowPos_Original ? SetWindowPos_Original : SetWindowPos;
        HWND after = d->hInsertAfter;
        if (!after || after == hGhost || !IsWindow(after)) after = d->ghostTopmost ? HWND_TOPMOST : HWND_TOP;
        if (pfnSetWindowPos(hGhost, after, 0, 0, 0, 0,
                            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW)) {
            return;
        }
        MacGenieShowGhostTopmost(hGhost);
        return;
    }
    if (underTaskbar) MacGenieShowGhostUnderTaskbar(hGhost, d->hMon);
    else MacGenieShowGhostTopmost(hGhost);
}

// A main-window close cloaks the real window once the ghost's first frame is on
// screen (3.7 did this for the Black Hole only; every close effect now does).
// Special windows are never cloaked: they hide / die right after the first frame.
static void MacGenieCloakClosingSource(MacGenieAnimData* data) {
    if (!data->isClosing || data->special || g_unloading.load(std::memory_order_relaxed)) return;
    if (IsWindow(data->hRealWnd) && IsWindowVisible(data->hRealWnd)) {
        MacGenieSetCloak(data->hRealWnd, TRUE);
        data->hiddenByCloak = TRUE;
    }
}

// A special window shown again (its show hook cleared "GenieSpecialAnim") is
// proof of life: its ghost stops at once instead of playing over the window.
static bool MacGenieSpecialCancelled(const MacGenieAnimData* d) {
    if (!d->special) return false;
    const HWND h = d->hRealWnd;
    if (!IsWindow(h) || !IsWindowVisible(h)) return false;
    return GetPropW(h, L"GenieSpecialAnim") == NULL;
}

// -------------------------------------------------------------------------
// Snapshot Cache Management (bounded by entry count AND bytes)
// -------------------------------------------------------------------------
static void MacGenieSnapshotBytesSub(size_t bytes) {
    if (g_SnapshotBytes >= bytes) g_SnapshotBytes -= bytes;
    else g_SnapshotBytes = 0;
}

static void MacGenieCacheEraseLocked(HWND hWnd) {
    auto it = g_SnapshotCache.find(hWnd);
    if (it != g_SnapshotCache.end()) {
        MacGenieSnapshotBytesSub((size_t)it->second.w * it->second.h * 4);
        DeleteObject(it->second.hBmp);
        g_SnapshotCache.erase(it);
    }
    auto ord = std::find(g_SnapshotOrder.begin(), g_SnapshotOrder.end(), hWnd);
    if (ord != g_SnapshotOrder.end()) g_SnapshotOrder.erase(ord);
}

// Removes hWnd's entry WITHOUT deleting its bitmap: ownership moves to the
// caller (the restore path adopts the cached DIB as its working snapshot instead
// of allocating a second one and copying every pixel across under the lock).
static bool MacGenieCacheTakeLocked(HWND hWnd, SnapCache* out) {
    auto it = g_SnapshotCache.find(hWnd);
    if (it == g_SnapshotCache.end()) return false;
    *out = it->second;
    MacGenieSnapshotBytesSub((size_t)out->w * out->h * 4);
    g_SnapshotCache.erase(it);
    auto ord = std::find(g_SnapshotOrder.begin(), g_SnapshotOrder.end(), hWnd);
    if (ord != g_SnapshotOrder.end()) g_SnapshotOrder.erase(ord);
    return true;
}

static void MacGenieCacheInsertLocked(HWND hWnd, const SnapCache& c) {
    g_cachesTouched.store(true, std::memory_order_relaxed);
    ULONGLONG now = GetTickCount64();
    // The restore snapshot exists precisely for windows that stay minimized a
    // while, so the age limit is minutes, not the 60 s V3 used (which forced a
    // PrintWindow of the still-cloaked window at restore - blank for many
    // GPU-rendered apps). Count and byte caps bound the memory instead.
    for (auto it = g_SnapshotOrder.begin(); it != g_SnapshotOrder.end();) {
        auto sit = g_SnapshotCache.find(*it);
        if (!IsWindow(*it) || (sit != g_SnapshotCache.end() && now - sit->second.tick > SNAPSHOT_MAX_AGE_MS)) {
            if (sit != g_SnapshotCache.end()) {
                MacGenieSnapshotBytesSub((size_t)sit->second.w * sit->second.h * 4);
                DeleteObject(sit->second.hBmp);
                g_SnapshotCache.erase(sit);
            }
            it = g_SnapshotOrder.erase(it);
        } else {
            ++it;
        }
    }
    MacGenieCacheEraseLocked(hWnd);
    g_SnapshotCache[hWnd] = c;
    g_SnapshotOrder.push_back(hWnd);
    g_SnapshotBytes += (size_t)c.w * c.h * 4;
    while ((g_SnapshotOrder.size() > MAX_SNAPSHOT_CACHE || g_SnapshotBytes > MAX_SNAPSHOT_BYTES) &&
           g_SnapshotOrder.size() > 1) {
        HWND victim = g_SnapshotOrder.front();
        g_SnapshotOrder.pop_front();
        auto vt = g_SnapshotCache.find(victim);
        if (vt != g_SnapshotCache.end()) {
            MacGenieSnapshotBytesSub((size_t)vt->second.w * vt->second.h * 4);
            DeleteObject(vt->second.hBmp);
            g_SnapshotCache.erase(vt);
        }
    }
}

// End-of-animation disposal of the snapshot bitmap.
//
// V3.6 captured the window, then - at the click, on the UI thread, with the
// global mutex held - allocated a second DIB and memcpy'd the whole snapshot
// into it for the restore cache (~32 MB for a maximized 4K window), and the
// restore path copied it back out the same way. No engine ever writes to the
// snapshot, so a finished minimize now simply hands the bitmap itself to the
// cache, and a restore adopts the cached bitmap as its own: zero copies, and the
// lock is held for a map insert only.
static void MacGenieReleaseSnapshot(MacGenieAnimData* data) {
    HBITMAP hBmp = data->hBitmap;
    if (!hBmp) return;
    void* bits = data->pBits;
    data->hBitmap = NULL;
    data->pBits = nullptr;
    if (data->cacheSnapshotOnFinish && bits && data->width > 0 && data->height > 0 &&
        !g_unloading.load(std::memory_order_relaxed) && IsWindow(data->hRealWnd)) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        MacGenieCacheInsertLocked(data->hRealWnd,
            { hBmp, bits, data->width, data->height, GetTickCount64() });
        return;
    }
    DeleteObject(hBmp);
}

static bool MacGenieShouldAnimate(HWND hWnd) {
    if (!hWnd) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) return false;
    // A window the shell has cloaked (another virtual desktop, a suspended UWP
    // app) is not on screen; animating it would ghost an invisible window.
    if (MacGenieIsShellCloaked(hWnd)) return false;

    RECT r;
    if (IsIconic(hWnd)) {
        WINDOWPLACEMENT wp;
        wp.length = sizeof(wp);
        if (!GetWindowPlacement(hWnd, &wp)) return false;
        r = wp.rcNormalPosition;
    } else if (!GetWindowRect(hWnd, &r)) {
        return false;
    }
    if ((r.right - r.left) < 40 || (r.bottom - r.top) < 40) return false;
    return true;
}

// -------------------------------------------------------------------------
// Monitor & Taskbar Geometry
// -------------------------------------------------------------------------
HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hMainTray = FindWindowW(L"Shell_TrayWnd", NULL);
    HMONITOR mainMon = MonitorFromWindow(hMainTray, MONITOR_DEFAULTTOPRIMARY);
    if (hMon == mainMon || !hMon) return hMainTray;

    HWND hSecTray = NULL;
    while ((hSecTray = FindWindowExW(NULL, hSecTray, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        if (MonitorFromWindow(hSecTray, MONITOR_DEFAULTTONULL) == hMon) {
            return hSecTray;
        }
    }
    return hMainTray;
}

static DockPos GetDockPosForMonitor(HMONITOR hMon) {
    int setting = g_taskbarSetting.load(std::memory_order_relaxed);
    if (setting == TB_SET_TOP) return DOCK_TOP;
    if (setting == TB_SET_BOTTOM) return DOCK_BOTTOM;

    HWND hTray = FindTaskbarForMonitor(hMon);
    RECT tb;
    if (!hTray || !GetWindowRect(hTray, &tb)) {
        return DOCK_BOTTOM;
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfoW(hMon, &mi)) {
        return DOCK_BOTTOM;
    }

    // A left/right taskbar spans the monitor's full height, so its top edge
    // also touches the monitor's top: the old edge tests classified it as a
    // TOP dock and DOCK_SIDE was unreachable. A taller-than-wide bar is a side
    // bar; otherwise the bar's midpoint decides (3.1.4's heuristic), which is
    // immune to a taskbar that is currently slid off-screen by auto-hide.
    if ((tb.bottom - tb.top) > (tb.right - tb.left)) return DOCK_SIDE;
    const LONG tbMid = tb.top + (tb.bottom - tb.top) / 2;
    const LONG monMid = mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2;
    return (tbMid < monMid) ? DOCK_TOP : DOCK_BOTTOM;
}

// -------------------------------------------------------------------------
// DPI space
//
// Every geometry API the mod uses (GetWindowRect, GetMonitorInfo, GetCursorPos,
// CreateWindowEx / UpdateLayeredWindow for the ghost, UI Automation) answers in
// the coordinate space of the calling thread's DPI awareness - EXCEPT
// DwmGetWindowAttribute(DWMWA_EXTENDED_FRAME_BOUNDS), which is always physical.
// In a per-monitor-aware process (Explorer, every modern app) the two agree. In
// a DPI-unaware or system-aware process (legacy Win32, Java, some games) on a
// scaled monitor they do not, and V3.6 mixed them: offsets between the window
// rect and the extended frame were garbage and the ghost was cut and placed
// wrongly. All the helpers below are identity (and cost one pointer test) for
// per-monitor-aware threads. The entry points are resolved dynamically so an
// older Windows 10 build can never fail to load the mod over a missing export.
// -------------------------------------------------------------------------
typedef DPI_AWARENESS_CONTEXT (WINAPI *GetThreadDpiAwarenessContext_t)(void);
typedef DPI_AWARENESS (WINAPI *GetAwarenessFromDpiAwarenessContext_t)(DPI_AWARENESS_CONTEXT);
typedef DPI_AWARENESS_CONTEXT (WINAPI *SetThreadDpiAwarenessContext_t)(DPI_AWARENESS_CONTEXT);
typedef UINT (WINAPI *GetDpiForSystem_t)(void);
static GetThreadDpiAwarenessContext_t g_pfnGetThreadDpiCtx = nullptr;
static GetAwarenessFromDpiAwarenessContext_t g_pfnGetAwareness = nullptr;
static SetThreadDpiAwarenessContext_t g_pfnSetThreadDpiCtx = nullptr;
static GetDpiForSystem_t g_pfnGetDpiForSystem = nullptr;

static void MacGenieResolveDpiApis() {
    HMODULE u = GetModuleHandleW(L"user32.dll");
    if (!u) return;
    g_pfnGetThreadDpiCtx = (GetThreadDpiAwarenessContext_t)GetProcAddress(u, "GetThreadDpiAwarenessContext");
    g_pfnGetAwareness = (GetAwarenessFromDpiAwarenessContext_t)GetProcAddress(u, "GetAwarenessFromDpiAwarenessContext");
    g_pfnSetThreadDpiCtx = (SetThreadDpiAwarenessContext_t)GetProcAddress(u, "SetThreadDpiAwarenessContext");
    g_pfnGetDpiForSystem = (GetDpiForSystem_t)GetProcAddress(u, "GetDpiForSystem");
}

static DPI_AWARENESS MacGenieThreadAwareness() {
    if (!g_pfnGetThreadDpiCtx || !g_pfnGetAwareness) return DPI_AWARENESS_PER_MONITOR_AWARE;
    return g_pfnGetAwareness(g_pfnGetThreadDpiCtx());
}

// True when the calling thread's coordinates are virtualized (unaware / system
// aware) - i.e. when physical values (DWM bounds, per-monitor UIA) need mapping.
static bool MacGenieThreadIsVirtualized() {
    const DPI_AWARENESS a = MacGenieThreadAwareness();
    return a == DPI_AWARENESS_UNAWARE || a == DPI_AWARENESS_SYSTEM_AWARE;
}

// Physical -> this process' coordinate space, for a window owned by this
// process. Foreign windows are animated from Explorer, whose space is physical.
static bool MacGenieOwnProcessWindow(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    return pid == GetCurrentProcessId();
}

// V4: physical (per-monitor) -> the calling thread's space, derived from the
// window's OWN rect read in both spaces: GetWindowRect as this thread sees it, and
// again under a moment of per-monitor awareness. Within one window the mapping is
// linear (a DPI-virtualized window is scaled about its monitor), so the two rects
// give it exactly.
//
// 3.9 used PhysicalToLogicalPointForPerMonitorDPI for this. For a mixed-mode
// window - Task Manager is a system-aware top level hosting per-monitor XAML
// islands - Windows mapped the frame's bottom-right corner (1578, 1081) to
// (860, 1081), and 3.9's containment check let the result through: the snapshot
// held the left 42 % of the window (the navigation menu) and the rest vanished
// without an animation.
struct MacGenieSpaceMap {
    double sx = 1.0, sy = 1.0, ox = 0.0, oy = 0.0;
    bool identity = true;
};

static bool MacGenieWindowRectPhysical(HWND hWnd, RECT* out) {
    if (!g_pfnSetThreadDpiCtx) return false;
    DPI_AWARENESS_CONTEXT old = g_pfnSetThreadDpiCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if (!old) old = g_pfnSetThreadDpiCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
    if (!old) return false;
    const BOOL ok = GetWindowRect(hWnd, out);
    g_pfnSetThreadDpiCtx(old);
    return ok != FALSE;
}

// The map from physical coordinates into the calling thread's space for hWnd;
// identity for per-monitor-aware threads. False = no mapping is possible.
static bool MacGeniePhysToLogicalMap(HWND hWnd, MacGenieSpaceMap* m) {
    *m = MacGenieSpaceMap{};
    if (!MacGenieThreadIsVirtualized()) return true;
    RECT logical, phys;
    if (!hWnd || !GetWindowRect(hWnd, &logical) || !MacGenieWindowRectPhysical(hWnd, &phys)) return false;
    const double pw = phys.right - phys.left, ph = phys.bottom - phys.top;
    const double lw = logical.right - logical.left, lh = logical.bottom - logical.top;
    if (pw < 1.0 || ph < 1.0 || lw < 1.0 || lh < 1.0) return false;
    if (EqualRect(&logical, &phys)) return true;           // same DPI: nothing to map
    m->identity = false;
    m->sx = lw / pw;
    m->sy = lh / ph;
    m->ox = (double)logical.left - (double)phys.left * m->sx;
    m->oy = (double)logical.top - (double)phys.top * m->sy;
    return true;
}

static void MacGenieMapPoint(const MacGenieSpaceMap& m, POINT* pt) {
    if (m.identity) return;
    pt->x = (LONG)lround((double)pt->x * m.sx + m.ox);
    pt->y = (LONG)lround((double)pt->y * m.sy + m.oy);
}

static void MacGenieMapRect(const MacGenieSpaceMap& m, RECT* rc) {
    if (m.identity) return;
    POINT a = { rc->left, rc->top }, b = { rc->right, rc->bottom };
    MacGenieMapPoint(m, &a);
    MacGenieMapPoint(m, &b);
    rc->left = a.x; rc->top = a.y; rc->right = b.x; rc->bottom = b.y;
}

// True when `frame` can be the visible frame of a window whose rect is winRect:
// inside it, and differing from it on EVERY side by at most the invisible resize
// border (a few px, scaled; 3.9 only checked containment, which let a frame cut
// to 42 % through). `scale` = the space's DPI scale.
static bool MacGenieFrameMatchesWindow(const RECT& frame, const RECT& winRect, float scale) {
    if (frame.right <= frame.left || frame.bottom <= frame.top) return false;
    const LONG tol = 2;
    const LONG slack = (LONG)(24.0f * (scale > 0.5f ? scale : 1.0f)) + 2;
    if (frame.left < winRect.left - tol || frame.top < winRect.top - tol ||
        frame.right > winRect.right + tol || frame.bottom > winRect.bottom + tol) {
        return false;
    }
    return frame.left - winRect.left <= slack && winRect.right - frame.right <= slack &&
           frame.top - winRect.top <= slack && winRect.bottom - frame.bottom <= slack;
}

static float GetMonitorDpiScale(HMONITOR hMon);

// The DWM extended frame of a window in the CALLING THREAD'S coordinate space,
// validated against GetWindowRect: the frame can only differ from the window
// rect by the invisible resize border, so a mapped rect that does not match on
// every side means the mapping did not apply, and the plain window rect is used
// (the snapshot then carries a transparent border, but the whole window).
static bool MacGenieGetExtendedFrame(HWND hWnd, const RECT& winRect, RECT* out) {
    RECT ext;
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &ext, sizeof(ext)))) {
        return false;
    }
    MacGenieSpaceMap map;
    if (!MacGeniePhysToLogicalMap(hWnd, &map)) return false;
    MacGenieMapRect(map, &ext);
    const float scale = GetMonitorDpiScale(MonitorFromRect(&winRect, MONITOR_DEFAULTTONEAREST));
    if (!MacGenieFrameMatchesWindow(ext, winRect, scale)) return false;
    *out = ext;
    return true;
}

// Scale of the coordinate space the CALLING thread draws in (1.0 at 96 dpi): an
// unaware thread always works at 96, a system-aware one at the system DPI, only a
// per-monitor-aware one at the monitor's DPI. Used to size the fallback dock
// target, the icon-height geometry and the corner radius.
static float GetMonitorDpiScale(HMONITOR hMon) {
    const DPI_AWARENESS a = MacGenieThreadAwareness();
    if (a == DPI_AWARENESS_UNAWARE) return 1.0f;
    if (a == DPI_AWARENESS_SYSTEM_AWARE) {
        return g_pfnGetDpiForSystem ? g_pfnGetDpiForSystem() / 96.0f : 1.0f;
    }
    UINT dpiX = 96, dpiY = 96;
    if (g_pfnGetDpiForMonitor && hMon) {
        if (FAILED(g_pfnGetDpiForMonitor(hMon, 0, &dpiX, &dpiY)) || dpiY == 0) dpiY = 96;
    }
    return dpiY / 96.0f;
}

// Radius, in the calling thread's coordinate space, of the corners DWM rounds on
// this window (0 = square). Rules from ReDrag's Windows Animations 1.3.7: no
// rounding when maximized, on Windows 10 (the attribute does not exist),
// DWMWCP_DONOTROUND, or - for the DEFAULT preference - on caption-less or
// layered windows; ROUNDSMALL is 4 px, everything else 8 px, scaled to DPI.
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
static int MacGenieWindowCornerRadiusPx(HWND hWnd, HMONITOR hMon, LONG_PTR originalExStyle) {
    if (!hWnd || IsZoomed(hWnd)) return 0;
    int preference = 0;   // DWMWCP_DEFAULT
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                                     &preference, sizeof(preference))) ||
        preference == 1 /* DWMWCP_DONOTROUND */) {
        return 0;
    }
    if (preference == 0) {
        const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        if (!(style & WS_CAPTION) || (originalExStyle & WS_EX_LAYERED)) return 0;
    }
    const float logical = (preference == 3 /* DWMWCP_ROUNDSMALL */) ? 4.0f : 8.0f;
    const int r = (int)(logical * GetMonitorDpiScale(hMon) + 0.5f);
    return r < 1 ? 1 : r;
}

// Limits the alpha of the four corners of a premultiplied-BGRA snapshot to a
// rounded-rectangle coverage (4x4 supersampled). PrintWindow and screen grabs
// return the square window surface; DWM applies the rounding at composition. A
// pure clamp (alpha only ever goes down, RGB scaled by the same ratio so the
// edge stays premultiplied-correct) so it is idempotent and safe to run on a
// snapshot that is already rounded.
static void MacGenieApplyRoundedCornerMask(DWORD* px, int width, int height, int radius) {
    if (!px || width <= 0 || height <= 0 || radius <= 0) return;
    if (radius > width / 2) radius = width / 2;
    if (radius > height / 2) radius = height / 2;
    if (radius <= 0) return;

    auto limit = [](DWORD& p, unsigned maskA) {
        const unsigned oldA = p >> 24;
        if (maskA >= oldA) return;
        if (!maskA || !oldA) { p = 0; return; }
        const unsigned b = p & 0xFF, g = (p >> 8) & 0xFF, r = (p >> 16) & 0xFF;
        auto sc = [&](unsigned c) { return (c * maskA + oldA / 2) / oldA; };
        p = (maskA << 24) | (sc(r) << 16) | (sc(g) << 8) | sc(b);
    };

    const float R = (float)radius;
    for (int y = 0; y < radius; y++) {
        for (int x = 0; x < radius; x++) {
            int inside = 0;
            for (int sy = 0; sy < 4; sy++) {
                const float dy = R - ((float)y + (sy + 0.5f) * 0.25f);
                for (int sx = 0; sx < 4; sx++) {
                    const float dx = R - ((float)x + (sx + 0.5f) * 0.25f);
                    if (dx * dx + dy * dy <= R * R) inside++;
                }
            }
            if (inside == 16) continue;
            const unsigned maskA = (unsigned)((inside * 255 + 8) / 16);
            limit(px[(size_t)y * width + x], maskA);
            limit(px[(size_t)y * width + (width - 1 - x)], maskA);
            limit(px[(size_t)(height - 1 - y) * width + x], maskA);
            limit(px[(size_t)(height - 1 - y) * width + (width - 1 - x)], maskA);
        }
    }
}

// Is the (auto-hide) taskbar currently slid in? A hidden bar keeps only a
// couple of pixels inside the monitor; a visible one shows its full height.
static bool MacGenieIsTaskbarExpanded(HWND hTray, const RECT& mon) {
    RECT tb;
    if (!hTray || !GetWindowRect(hTray, &tb)) return false;
    RECT vis;
    if (!IntersectRect(&vis, &tb, &mon)) return false;
    return (vis.bottom - vis.top) > 8 && (vis.right - vis.left) > 8;
}

static bool MacGenieIsCursorOverTaskbar(HWND hTray) {
    POINT pt;
    RECT tb;
    if (!hTray || !GetCursorPos(&pt) || !GetWindowRect(hTray, &tb)) return false;
    return PtInRect(&tb, pt) != FALSE;
}

// Waits (bounded) for an auto-hidden taskbar to slide in before its rect is
// used for geometry. Reading it right after SetForegroundWindow(hTray) - as V3
// did - saw the 2 px hidden strip, which aimed a top-taskbar genie above the
// screen.
static void MacGenieWaitTaskbarExpanded(HWND hTray, const RECT& mon, DWORD maxMs) {
    ULONGLONG deadline = GetTickCount64() + maxMs;
    while (!MacGenieIsTaskbarExpanded(hTray, mon) && GetTickCount64() < deadline &&
           !g_unloading.load(std::memory_order_relaxed)) {
        Sleep(10);
    }
}

// Fallback dock Y (used until UI Automation finds the real button): the centre
// of the taskbar's visible band, clamped into the monitor. V3 used an unscaled
// 33 px constant from the monitor edge, which was off on scaled displays and
// could land above a slid-out top taskbar.
static int GetDockIconCenterY(HMONITOR hMon, const RECT& mon, DockPos dockPos) {
    const int iconHalf = (int)(COMPAT_ICON_SIZE * GetMonitorDpiScale(hMon) * 0.5f);
    HWND hTray = FindTaskbarForMonitor(hMon);
    RECT tb;
    if (hTray && GetWindowRect(hTray, &tb) && MacGenieIsTaskbarExpanded(hTray, mon)) {
        RECT vis;
        IntersectRect(&vis, &tb, &mon);
        int y = vis.top + (vis.bottom - vis.top) / 2;
        return MacClampInt(y, mon.top + iconHalf, mon.bottom - iconHalf);
    }
    if (dockPos == DOCK_TOP) return mon.top + iconHalf;
    return mon.bottom - iconHalf;
}

// -------------------------------------------------------------------------
// Asynchronous UI Automation Taskbar Dock Resolution (Timeout-Hardened)
// -------------------------------------------------------------------------
struct UiaPending {
    volatile LONG refs = 2;
    HANDLE done = nullptr;
    POINT target = {0, 0};
    bool found = false;
    void Release() {
        if (InterlockedDecrement(&refs) == 0) {
            if (done) CloseHandle(done);
            delete this;
        }
    }
};

struct UiaTask {
    HWND hWndApp;
    std::wstring titleLower;
    std::wstring procNameLower;
    std::wstring procHintLower;
    std::wstring procPathLower;     // full image path, lower-cased
    std::wstring processKey;
    HMONITOR hMon;
    POINT fallback;
    uint64_t sig = 0;               // taskbar layout signature at request time
    bool mapToLogical = false;      // app thread is DPI-virtualized: map UIA's
                                    // physical result into its coordinate space
    MacGenieSpaceMap logMap;        // ... with this map, built on the app thread (V4)
    UiaPending* pending = nullptr;
};

static std::wstring MakeProcessDockKey(const std::wstring& procNameLower, HMONITOR hMon) {
    if (procNameLower.empty()) return L"";
    if (!hMon) return procNameLower;
    return procNameLower + L"_" + std::to_wstring(reinterpret_cast<size_t>(hMon));
}

static std::wstring ProcHintForUia(const std::wstring& procNameLower) {
    if (procNameLower == L"chrome") return L"google chrome";
    if (procNameLower == L"msedge") return L"microsoft edge";
    if (procNameLower == L"firefox") return L"firefox";
    if (procNameLower == L"brave") return L"brave";
    if (procNameLower == L"opera") return L"opera";
    if (procNameLower == L"vivaldi") return L"vivaldi";
    if (procNameLower == L"discord") return L"discord";
    if (procNameLower == L"spotify") return L"spotify";
    if (procNameLower == L"code") return L"visual studio code";
    if (procNameLower == L"devenv") return L"visual studio";
    return procNameLower;
}

// -------------------------------------------------------------------------
// Taskbar layout signature
//
// A taskbar button's slot only moves when the set of buttons changes (an app
// opened or closed, a window moved to another desktop / monitor), the shell's
// alignment changed, or a pin was reordered. V3.6 re-resolved a window's slot
// every 2 s of age whether or not anything had happened; V3.7 hashes the set of
// windows that own a taskbar button (+ the alignment value) and serves a cached
// slot for as long as the hash is unchanged. A changed hash means the cached slot
// may be stale: it is still served (the click must not wait) while a background
// lookup refreshes it. ICON_POS_TRUST_MS bounds the one thing the hash cannot
// see, a pin reorder.
// -------------------------------------------------------------------------
struct TaskbarSigAcc { uint64_t x; uint64_t n; };

static BOOL CALLBACK MacGenieTaskbarSigEnumProc(HWND h, LPARAM lp) {
    if (!IsWindowVisible(h)) return TRUE;
    const LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) && !(ex & WS_EX_APPWINDOW)) return TRUE;
    if (GetWindow(h, GW_OWNER) && !(ex & WS_EX_APPWINDOW)) return TRUE;
    if (MacGenieIsShellCloaked(h)) return TRUE;      // another virtual desktop
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    uint64_t v = (uint64_t)(uintptr_t)h ^ ((uint64_t)pid << 32) ^
                 ((uint64_t)(uintptr_t)MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST) << 17);
    v ^= v >> 30; v *= 0xbf58476d1ce4e5b9ULL;
    v ^= v >> 27; v *= 0x94d049bb133111ebULL;
    v ^= v >> 31;
    TaskbarSigAcc* acc = (TaskbarSigAcc*)lp;
    acc->x ^= v;
    acc->n++;
    return TRUE;
}

static uint64_t MacGenieTaskbarSignature(DWORD alignment) {
    TaskbarSigAcc acc = { 0, 0 };
    EnumWindows(MacGenieTaskbarSigEnumProc, (LPARAM)&acc);
    return acc.x ^ (acc.n * 0x9e3779b97f4a7c15ULL) ^ ((uint64_t)alignment << 56);
}

// -------------------------------------------------------------------------
// Application identity (language-independent targeting)
//
// A Windows 11 taskbar button's UI Automation AutomationId is
// "Appid: <AppUserModelID | exe path | known-folder-relative exe path>" and
// its name is localized ("Bloc-notes epingle"). Matching the id instead of the
// name works on every display language. Verified on Windows 11 (26300): task
// buttons have ClassName "Taskbar.TaskListButtonAutomationPeer", Start / Search /
// Task View are plain "ToggleButton", tray icons are "SystemTray.*".
// -------------------------------------------------------------------------
static std::wstring MacGenieLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), ::towlower);
    return s;
}

static std::wstring MacGenieWindowAumidLower(HWND hWnd) {
    std::wstring out;
    IPropertyStore* ps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, __uuidof(IPropertyStore), (void**)&ps)) && ps) {
        // PKEY_AppUserModel_ID {9F4C2855-9F79-4B39-A8D0-E1D42DE1D5F3}, 5
        PROPERTYKEY key = { { 0x9F4C2855, 0x9F79, 0x4B39,
                              { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 5 };
        PROPVARIANT pv;
        PropVariantInit(&pv);
        if (SUCCEEDED(ps->GetValue(key, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) {
            out = pv.pwszVal;
        }
        PropVariantClear(&pv);
        ps->Release();
    }
    return MacGenieLower(out);
}

typedef LONG (WINAPI *GetApplicationUserModelId_t)(HANDLE, UINT32*, PWSTR);

static std::wstring MacGenieProcessAumidLower(HWND hWnd) {
    static GetApplicationUserModelId_t pfn = (GetApplicationUserModelId_t)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "GetApplicationUserModelId");
    if (!pfn) return L"";
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return L"";
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h) return L"";
    WCHAR buf[256];
    UINT32 len = 256;
    std::wstring out;
    if (pfn(h, &len, buf) == ERROR_SUCCESS && len > 1) out.assign(buf, len - 1);
    CloseHandle(h);
    return MacGenieLower(out);
}

// Does the (lower-cased, "appid:"-stripped) button id name this image path? The
// shell writes classic apps either as the full path or as "{KNOWNFOLDER-GUID}\rest".
static bool MacGenieIdMatchesPath(const std::wstring& id, const std::wstring& pathLower) {
    if (id.empty() || pathLower.empty()) return false;
    if (id == pathLower) return true;
    if (id[0] == L'{') {
        size_t close = id.find(L"}\\");
        if (close != std::wstring::npos) {
            const std::wstring rest = id.substr(close + 1);        // "\Derivative\...exe"
            if (rest.size() > 4 && pathLower.size() >= rest.size() &&
                pathLower.compare(pathLower.size() - rest.size(), rest.size(), rest) == 0) {
                return true;
            }
        }
    }
    return false;
}

template <class T> struct UiaPtr {
    T* p = nullptr;
    UiaPtr() = default;
    UiaPtr(const UiaPtr&) = delete;
    UiaPtr& operator=(const UiaPtr&) = delete;
    ~UiaPtr() { reset(); }
    void reset() { if (p) { p->Release(); p = nullptr; } }
    T** operator&() { return &p; }
    T* operator->() const { return p; }
    explicit operator bool() const { return p != nullptr; }
};

// Resolves the taskbar button centre for t->hWndApp, in the app thread's
// coordinate space. Returns false when no button matched.
static bool UiaResolveTaskbarButton(UiaTask* t, POINT* out) {
    UiaPtr<IUIAutomation> automation;
    {
        IUIAutomation2* a2 = nullptr;
        HRESULT hrUia = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER,
                                         __uuidof(IUIAutomation2), (void**)&a2);
        if (SUCCEEDED(hrUia) && a2) {
            a2->put_ConnectionTimeout(1000);
            a2->put_TransactionTimeout(2000);
            automation.p = a2;                       // takes over the reference
        } else {
            CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER,
                             __uuidof(IUIAutomation), (void**)&automation.p);
        }
    }
    if (!automation) return false;

    HWND hTray = FindTaskbarForMonitor(t->hMon);
    if (!hTray) return false;
    UiaPtr<IUIAutomationElement> tray;
    if (FAILED(automation->ElementFromHandle(hTray, &tray)) || !tray) return false;
    if (g_unloading.load(std::memory_order_relaxed)) return false;

    // One batched cache request: Name + AutomationId + BoundingRectangle come
    // back with the element array in a single cross-process round trip, instead
    // of V3.6's separate call per element per property. (Measured: the search
    // itself dominates, see the scope note below - the cache request is the
    // smaller part of the saving.)
    UiaPtr<IUIAutomationCacheRequest> cache;
    if (SUCCEEDED(automation->CreateCacheRequest(&cache)) && cache) {
        cache->AddProperty(UIA_NamePropertyId);
        cache->AddProperty(UIA_AutomationIdPropertyId);
        cache->AddProperty(UIA_BoundingRectanglePropertyId);
    } else {
        cache.reset();
    }
    const bool cached = (bool)cache;

    auto findAllIn = [&](IUIAutomationElement* root, TreeScope scope, IUIAutomationCondition* cond,
                         IUIAutomationElementArray** arr) -> bool {
        *arr = nullptr;
        const HRESULT h = cached
            ? root->FindAllBuildCache(scope, cond, cache.p, arr)
            : root->FindAll(scope, cond, arr);
        return SUCCEEDED(h) && *arr;
    };
    auto findAll = [&](IUIAutomationCondition* cond, IUIAutomationElementArray** arr) -> bool {
        return findAllIn(tray.p, TreeScope_Descendants, cond, arr);
    };

    UiaPtr<IUIAutomationElementArray> arr;
    int length = 0;
    bool classFiltered = false;

    // 1) Only real task buttons (server-side filter: Start, Search, Task View,
    //    Widgets and every tray icon never leave Explorer). Language-independent.
    //
    //    Where the lookup runs matters more than how it is filtered: a
    //    Descendants search from the taskbar root makes Explorer walk the whole
    //    XAML tree including the system tray and costs ~45 ms on a 44-element
    //    tree (measured, Windows 11 26300), while the task buttons are plain
    //    children of the "TaskbarFrame" element. Finding that element (~6 ms) and
    //    searching only its children (~10-15 ms with the cache request) is 2-3x
    //    faster. If the frame cannot be found the whole-tray search runs as before.
    {
        UiaPtr<IUIAutomationCondition> classCond;
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_BSTR;
        v.bstrVal = SysAllocString(L"Taskbar.TaskListButtonAutomationPeer");
        if (v.bstrVal) {
            automation->CreatePropertyCondition(UIA_ClassNamePropertyId, v, &classCond);
            VariantClear(&v);
        }
        if (classCond) {
            UiaPtr<IUIAutomationElement> frame;
            {
                UiaPtr<IUIAutomationCondition> frameCond;
                VARIANT fv;
                VariantInit(&fv);
                fv.vt = VT_BSTR;
                fv.bstrVal = SysAllocString(L"TaskbarFrame");
                if (fv.bstrVal) {
                    automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, fv, &frameCond);
                    VariantClear(&fv);
                }
                if (frameCond) tray->FindFirst(TreeScope_Descendants, frameCond.p, &frame);
            }
            if (frame && findAllIn(frame.p, TreeScope_Children, classCond.p, &arr.p)) {
                arr->get_Length(&length);
                classFiltered = length > 0;
            }
            if (!classFiltered) {
                arr.reset(); length = 0;
                if (findAll(classCond.p, &arr.p)) {
                    arr->get_Length(&length);
                    classFiltered = length > 0;
                }
            }
        }
        if (!classFiltered) { arr.reset(); length = 0; }
    }
    // 2) Older shells: every button / list item, scored by name (V3.6 behaviour).
    if (!arr) {
        UiaPtr<IUIAutomationCondition> pButtonCond, pListItemCond, pOrCond;
        VARIANT varBtn{}; varBtn.vt = VT_I4; varBtn.lVal = UIA_ButtonControlTypeId;
        automation->CreatePropertyCondition(UIA_ControlTypePropertyId, varBtn, &pButtonCond);
        VARIANT varList{}; varList.vt = VT_I4; varList.lVal = UIA_ListItemControlTypeId;
        automation->CreatePropertyCondition(UIA_ControlTypePropertyId, varList, &pListItemCond);
        if (pButtonCond && pListItemCond) {
            automation->CreateOrCondition(pButtonCond.p, pListItemCond.p, &pOrCond);
        }
        if (!pOrCond || !findAll(pOrCond.p, &arr.p)) return false;
        arr->get_Length(&length);
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfoW(t->hMon, &mi)) {
        // A zeroed rect made monRight 0, so no button could ever pass the
        // "left of the tray area" test.
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0;
        mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    const int monRight = mi.rcMonitor.right;

    const std::wstring aumidWin = MacGenieWindowAumidLower(t->hWndApp);
    const std::wstring aumidProc = MacGenieProcessAumidLower(t->hWndApp);

    int bestScore = 0;
    POINT best = { 0, 0 };
    for (int i = 0; i < length; i++) {
        if (g_unloading.load(std::memory_order_relaxed)) break;
        UiaPtr<IUIAutomationElement> item;
        if (FAILED(arr->GetElement(i, &item)) || !item) continue;

        BSTR nameB = nullptr, idB = nullptr;
        if (cached) { item->get_CachedName(&nameB); item->get_CachedAutomationId(&idB); }
        else        { item->get_CurrentName(&nameB); item->get_CurrentAutomationId(&idB); }
        const std::wstring uiaNameLower = nameB ? MacGenieLower(nameB) : L"";
        std::wstring idLower = idB ? MacGenieLower(idB) : L"";
        if (nameB) SysFreeString(nameB);
        if (idB) SysFreeString(idB);

        // "Appid: <id>" -> "<id>"
        if (idLower.compare(0, 6, L"appid:") == 0) {
            idLower.erase(0, 6);
            const size_t b = idLower.find_first_not_of(L' ');
            idLower = (b == std::wstring::npos) ? L"" : idLower.substr(b);
        }

        int score = 0;
        if (!idLower.empty()) {
            // Exact identity: the window's AppUserModelID, the process' packaged
            // id, or the image path. Beats any name heuristic, in any language.
            if ((!aumidWin.empty() && idLower == aumidWin) ||
                (!aumidProc.empty() && idLower == aumidProc) ||
                MacGenieIdMatchesPath(idLower, t->procPathLower)) {
                score += 3000;
            } else if (t->procNameLower.size() >= 4 &&
                       idLower.find(t->procNameLower) != std::wstring::npos) {
                score += 600;
            }
        }

        if (!uiaNameLower.empty()) {
            if (t->titleLower == uiaNameLower) score += 1000;
            if (!t->titleLower.empty() && t->titleLower.find(uiaNameLower) != std::wstring::npos) score += 500;
            if (!t->titleLower.empty() && uiaNameLower.find(t->titleLower) != std::wstring::npos) score += 500;
            if (!t->procNameLower.empty() && uiaNameLower.find(t->procNameLower) != std::wstring::npos) score += 400;
            if (!t->procHintLower.empty() && t->procHintLower != t->procNameLower &&
                uiaNameLower.find(t->procHintLower) != std::wstring::npos) {
                score += 900;
            }

            std::wstring currentWord;
            for (wchar_t c : t->titleLower) {
                if (iswalnum(c)) currentWord += c;
                else {
                    if (currentWord.length() >= 4 && uiaNameLower.find(currentWord) != std::wstring::npos) score += 50;
                    currentWord.clear();
                }
            }
            if (currentWord.length() >= 4 && uiaNameLower.find(currentWord) != std::wstring::npos) score += 50;

            // Name-based exclusions are only needed when the class filter could
            // not already remove these buttons (older shells, English labels).
            if (!classFiltered) {
                if (uiaNameLower.find(L"start") != std::wstring::npos) score -= 500;
                if (uiaNameLower.find(L"search") != std::wstring::npos) score -= 500;
                if (uiaNameLower.find(L"task view") != std::wstring::npos) score -= 500;
                if (uiaNameLower.find(L"widgets") != std::wstring::npos) score -= 500;
            }
        }

        if (score >= 400 && score > bestScore) {
            RECT bRect;
            const HRESULT hrR = cached ? item->get_CachedBoundingRectangle(&bRect)
                                       : item->get_CurrentBoundingRectangle(&bRect);
            if (SUCCEEDED(hrR) && bRect.right > bRect.left && bRect.left < monRight - 50) {
                bestScore = score;
                const LONG bw = bRect.right - bRect.left, bh = bRect.bottom - bRect.top;
                best.x = bRect.left + bw / 2;
                best.y = bRect.top + bh / 2;
                // V4: a button with its label shown ("never combine") is much
                // wider than tall and draws its icon at the leading edge: land
                // on the icon, not on the middle of the label.
                if (bh > 0 && bw > bh * 3 / 2) best.x = bRect.left + bh / 2;
            }
        }
    }
    if (bestScore <= 0) return false;

    // The lookup thread ran per-monitor-aware, so `best` is physical. Bring it
    // into the app thread's space (identity unless that thread is virtualized).
    if (t->mapToLogical) MacGenieMapPoint(t->logMap, &best);
    *out = best;
    return true;
}

DWORD WINAPI UiaWorkerThread(LPVOID lpParam) {
    UiaTask* t = (UiaTask*)lpParam;
    UiaPending* pending = t->pending;
    struct UiaCleanupGuard {
        UiaTask* task;
        UiaPending* pending;
        ~UiaCleanupGuard() {
            if (pending) {
                if (pending->done) SetEvent(pending->done);
                pending->Release();
            }
            delete task;
        }
    } guard{ t, pending };

    if (g_unloading.load(std::memory_order_relaxed)) return 0;

    POINT target = t->fallback;
    bool uiaFound = false;

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    const bool coInit = (hr == S_OK || hr == S_FALSE);

    if (hr == S_OK || hr == S_FALSE || hr == RPC_E_CHANGED_MODE) {
        // Ask UI Automation for PHYSICAL coordinates (a thread answers in its own
        // DPI space); the result is mapped into the app's space afterwards.
        DPI_AWARENESS_CONTEXT prevCtx = nullptr;
        if (t->mapToLogical && g_pfnSetThreadDpiCtx) {
            prevCtx = g_pfnSetThreadDpiCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
        uiaFound = UiaResolveTaskbarButton(t, &target);
        if (prevCtx && g_pfnSetThreadDpiCtx) g_pfnSetThreadDpiCtx(prevCtx);
    }
    if (coInit) CoUninitialize();

    if (g_unloading.load(std::memory_order_relaxed)) return 0;

    if (uiaFound) {
        if (pending) {
            pending->target = target;
            pending->found = true;
        }
        g_cachesTouched.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        // Never seed a dead or recycled handle: the window may have been
        // destroyed (and its cache entry erased) while UIA was running.
        if (IsWindow(t->hWndApp)) g_IconPositions[t->hWndApp] = { target, GetTickCount64(), t->sig };
        if (!t->processKey.empty()) {
            g_ProcessIconPositions[t->processKey] = target;
            g_ProcessUiaNegativeUntil.erase(t->processKey);
        }
    } else {
        // A miss is NOT written into the per-window cache (V3 did, which pinned
        // the fallback guess for the window's lifetime and never retried).
        // Instead the process is negatively cached for a while so windows
        // without a taskbar button do not pay the wait budget on every
        // minimize.
        g_cachesTouched.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (!t->processKey.empty()) {
            g_ProcessUiaNegativeUntil[t->processKey] = GetTickCount64() + UIA_NEGATIVE_CACHE_MS;
        }
    }
    return 0;
}

// Resolves the taskbar-button centre for a window, in two steps so the lookup
// can overlap the window capture (V3.6 ran them back to back on the UI thread).
//
// Cache policy (V3.7): a per-window hit resolved under the CURRENT taskbar
// layout signature (and younger than ICON_POS_TRUST_MS) is returned as is, with
// no lookup at all. Any other hit is still returned immediately - the click must
// not wait - but a background lookup is dispatched to refresh it, so a slot that
// moved (an app opened or closed on a centred Win11 taskbar) is right again from
// the next minimize on. Only a window with nothing cached waits for the lookup,
// and only for what is left of an 80 ms budget after the capture has run.
struct TaskbarLookup {
    POINT soft = { 0, 0 };            // best answer available without waiting
    UiaPending* pending = nullptr;    // non-null: a result may still arrive
    ULONGLONG deadline = 0;           // tick the wait for it ends
};

static TaskbarLookup GetTaskbarButtonPos_Begin(HWND hWndApp, POINT fallback, HMONITOR hMon,
                                               uint64_t sig) {
    TaskbarLookup lk;
    std::wstring procPathLower;
    std::wstring procNameLower = GetProcessNameCached(hWndApp, &procPathLower);
    std::wstring processKey = MakeProcessDockKey(procNameLower, hMon);
    POINT hwndCached = {0, 0};
    POINT processCached = {0, 0};
    bool haveHwndCache = false;
    bool hwndFresh = false;
    bool haveProcessCache = false;
    bool negativeCached = false;

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        const ULONGLONG now = GetTickCount64();
        auto it = g_IconPositions.find(hWndApp);
        if (it != g_IconPositions.end()) {
            hwndCached = it->second.pt;
            haveHwndCache = true;
            hwndFresh = (it->second.sig == sig) && (now - it->second.tick) < ICON_POS_TRUST_MS;
        }
        if (!processKey.empty()) {
            auto pit = g_ProcessIconPositions.find(processKey);
            if (pit != g_ProcessIconPositions.end()) {
                processCached = pit->second;
                haveProcessCache = true;
            }
            auto nit = g_ProcessUiaNegativeUntil.find(processKey);
            if (nit != g_ProcessUiaNegativeUntil.end() && now < nit->second) {
                negativeCached = true;
            }
        }
    }

    if (haveHwndCache && hwndFresh) { lk.soft = hwndCached; return lk; }

    POINT softFallback = haveHwndCache ? hwndCached : (haveProcessCache ? processCached : fallback);
    lk.soft = softFallback;
    if (negativeCached) return lk;

    WCHAR titleW[512] = {0};
    GetWindowTextW(hWndApp, titleW, 512);
    std::wstring titleLower = MacGenieLower(titleW);
    const std::wstring procHintLower = ProcHintForUia(procNameLower);

    auto* pending = new UiaPending{};
    pending->done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    pending->target = softFallback;
    if (!pending->done) {
        pending->Release();
        pending->Release();
        return lk;
    }

    auto* task = new UiaTask{};
    task->hWndApp = hWndApp;
    task->titleLower = titleLower;
    task->procNameLower = procNameLower;
    task->procHintLower = procHintLower;
    task->procPathLower = procPathLower;
    task->processKey = processKey;
    task->hMon = hMon;
    task->fallback = softFallback;
    task->sig = sig;
    task->mapToLogical = MacGenieThreadIsVirtualized() && g_pfnSetThreadDpiCtx &&
                         MacGenieOwnProcessWindow(hWndApp) &&
                         MacGeniePhysToLogicalMap(hWndApp, &task->logMap);
    task->pending = pending;
    if (!StartWorkerThread(UiaWorkerThread, task)) {
        delete task;
        pending->Release();
        pending->Release();
        return lk;
    }

    // Something usable is already in hand -> pure background refresh; only a
    // cold window waits (bounded) for the answer.
    if (haveHwndCache || haveProcessCache) {
        pending->Release();
        return lk;
    }
    lk.pending = pending;
    lk.deadline = GetTickCount64() + 80;
    return lk;
}

// Waits for whatever is left of the budget and releases the lookup. *waitedMs
// (optional) receives the time actually spent blocked, for telemetry.
static POINT GetTaskbarButtonPos_Finish(TaskbarLookup* lk, double* waitedMs = nullptr) {
    POINT result = lk->soft;
    if (waitedMs) *waitedMs = 0.0;
    if (lk->pending) {
        const ULONGLONG now = GetTickCount64();
        const DWORD left = (now < lk->deadline) ? (DWORD)(lk->deadline - now) : 0;
        const ULONGLONG t0 = GetTickCount64();
        const DWORD wait = WaitForSingleObject(lk->pending->done, left);
        if (waitedMs) *waitedMs = (double)(GetTickCount64() - t0);
        if (wait == WAIT_OBJECT_0 && lk->pending->found) {
            result = lk->pending->target;
        }
        lk->pending->Release();
        lk->pending = nullptr;
    }
    return result;
}

static RECT GetGenieMonitorUnion(const RECT& winRect, HMONITOR hDockMon) {
    RECT winMon;
    HMONITOR hWinMon = MonitorFromRect(&winRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO wmi = {};
    wmi.cbSize = sizeof(wmi);
    if (hWinMon && GetMonitorInfoW(hWinMon, &wmi)) {
        winMon = wmi.rcMonitor;
    } else {
        winMon = winRect;
    }

    RECT dockMon;
    MONITORINFO dmi = {};
    dmi.cbSize = sizeof(dmi);
    if (hDockMon && GetMonitorInfoW(hDockMon, &dmi)) {
        dockMon = dmi.rcMonitor;
    } else {
        dockMon = winMon;
    }

    RECT u;
    u.left   = (winMon.left   < dockMon.left)   ? winMon.left   : dockMon.left;
    u.top    = (winMon.top    < dockMon.top)    ? winMon.top    : dockMon.top;
    u.right  = (winMon.right  > dockMon.right)  ? winMon.right  : dockMon.right;
    u.bottom = (winMon.bottom > dockMon.bottom) ? winMon.bottom : dockMon.bottom;
    return u;
}

static void ClampGenieCanvas(int& left, int& top, int& right, int& bottom,
                             const RECT& winRect, int dockX, int dockHalfW,
                             const RECT& monUnion) {
    if (left   < monUnion.left)   left   = monUnion.left;
    if (right  > monUnion.right)  right  = monUnion.right;
    if (top    < monUnion.top)    top    = monUnion.top;
    if (bottom > monUnion.bottom) bottom = monUnion.bottom;

    if (left  > winRect.left)  left  = winRect.left;
    if (right < winRect.right) right = winRect.right;
    if (top   > winRect.top)   top   = winRect.top;
    if (bottom < winRect.bottom) bottom = winRect.bottom;
    // The dock landing must fit: the funnel can be up to 120 px wide, the old
    // fixed +-16 clipped anything wider.
    if (dockHalfW < 16) dockHalfW = 16;
    if (left  > dockX - dockHalfW) left  = dockX - dockHalfW;
    if (right < dockX + dockHalfW) right = dockX + dockHalfW;
}

// V3.9 (macOS Minimize Animation fork 3.2.1): every engine runs this after its own
// clamp. The clamps above deliberately widen the canvas back to the whole window,
// so a window partly (or far) off screen made a canvas covering pixels no monitor
// shows - every engine already clips what falls outside its canvas, so dropping
// them changes nothing on screen. False when nothing visible is left or the
// canvas is still over MACGENIE_MAX_CANVAS_PIXELS: the engine then aborts and the
// operation completes without an animation.
static bool MacGenieClipCanvasToDesktop(int& left, int& top, int& right, int& bottom) {
    const int vsLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int vsTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int vsWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int vsHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (vsWidth > 0 && vsHeight > 0) {
        if (left   < vsLeft)            left   = vsLeft;
        if (top    < vsTop)             top    = vsTop;
        if (right  > vsLeft + vsWidth)  right  = vsLeft + vsWidth;
        if (bottom > vsTop + vsHeight)  bottom = vsTop + vsHeight;
    }
    if (right <= left || bottom <= top) return false;
    return (long long)(right - left) * (bottom - top) <= MACGENIE_MAX_CANVAS_PIXELS;
}

// Can a window with this visible frame / window rect be animated at all? Not when
// the snapshot would be over the size cap, or when no part of it is on a monitor
// (an application parking a window off screen): there is nothing to see.
static bool MacGenieSnapshotSizeOk(const RECT& frame, const RECT& winRect) {
    const long long fw = frame.right - frame.left, fh = frame.bottom - frame.top;
    const long long ww = winRect.right - winRect.left, wh = winRect.bottom - winRect.top;
    if (fw <= 0 || fh <= 0 || ww <= 0 || wh <= 0) return false;
    if (fw * fh > MACGENIE_MAX_CANVAS_PIXELS || ww * wh > MACGENIE_MAX_CANVAS_PIXELS) return false;
    return MonitorFromRect(&frame, MONITOR_DEFAULTTONULL) != NULL;
}

// Reveal-before-teardown for the restore path. Every ancestor of this mod
// uncloaked the real window BEFORE destroying the ghost; V3.5 did it the other
// way round in all three engines, which left one composed frame with neither
// ghost nor window on screen (a blink at the end of every restore). Engines
// call this right before DestroyWindow(hGhost); the finisher then skips it.
static void MacGenieRevealBeforeGhostGone(MacGenieAnimData* data) {
    if (data->isClosing || !data->isRising || data->revealDone) return;
    if (IsWindow(data->hRealWnd)) {
        MacGenieUndoRisingHide(data->hRealWnd, data->originalExStyle, data->hiddenByCloak);
        DwmFlush();
    }
    data->revealDone = TRUE;
}

// Hands taskbar focus back after an auto-hide reveal - but only if the taskbar
// still owns it. V3 re-foregrounded unconditionally, yanking focus away from
// whatever the user had clicked in the meantime.
static void MacGenieReturnTaskbarFocus(MacGenieAnimData* data) {
    if (!data->taskbarFocusBorrowed || g_unloading.load(std::memory_order_relaxed)) return;
    HWND hTray = FindTaskbarForMonitor(data->hMon);
    if (!hTray || GetForegroundWindow() != hTray) return;
    if (data->hNextApp && IsWindow(data->hNextApp) && IsWindowVisible(data->hNextApp) &&
        !IsIconic(data->hNextApp)) {
        SetForegroundWindow(data->hNextApp);
    } else {
        HWND hProgman = FindWindowW(L"Progman", NULL);
        if (hProgman) SetForegroundWindow(hProgman);
    }
}

static void MacGenieFinishAnimation(MacGenieAnimData* data) {
    // V4: leave the reversal registry first. A hook that asked for a reversal
    // after the render loop had already ended left the window in a state this
    // animation must still resolve (pendingDir below); none can ask after this.
    LONG pendingDir = -1;
    if (data->reversible) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_AnimData.find(data->hRealWnd);
        if (it != g_AnimData.end() && it->second == data) g_AnimData.erase(it);
        pendingDir = InterlockedExchange(&data->wantDir, -1);
    }
    if (!data->isClosing) {
        if (data->isRising) {
            if (!data->revealDone) {
                MacGenieUndoRisingHide(data->hRealWnd, data->originalExStyle, data->hiddenByCloak);
            }
        } else if (pendingDir == 1 && IsWindow(data->hRealWnd)) {
            // A restore came as this minimize ended: the hook restored the window
            // under a cloak. Show it.
            MacGenieUndoRisingHide(data->hRealWnd, data->originalExStyle, TRUE);
        } else if (data->undoHideOnFallEnd && IsWindow(data->hRealWnd)) {
            // Reversed from a restore: the window was minimized under the restore's
            // hide. Lift it, or the window would stay invisible when restored later.
            MacGenieUndoRisingHide(data->hRealWnd, data->originalExStyle, data->hiddenByCloak);
            MacFxRememberForRestore(data->hRealWnd, data->fx);
        }
        // A plain falling animation never hid the real window, so there is
        // nothing to undo here. V3 ran the rising undo when the window was
        // found non-iconic at the end ("mid-flight restore"), which sent
        // SetLayeredWindowAttributes + a whole-ex-style rewrite to a window
        // the mod had never touched; a mid-flight restore already uncloaks
        // itself on its own (blocked rising) path.
    }

    if (!data->isClosing && !data->deferredMinimize) {
        MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
    }

    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
        data->hFirstFrameShown = NULL;
    }

    if (data->deferredMinimize) {
        int unhideMs = data->unhideDurationMs;
        int animMs = data->durationMs;
        if (data->requestedUnhide && unhideMs > animMs) {
            int remaining = unhideMs - animMs;
            while (remaining > 0 && !g_unloading.load(std::memory_order_relaxed)) {
                int chunk = remaining < 20 ? remaining : 20;
                Sleep(chunk);
                remaining -= chunk;
            }
        }

        if (!g_unloading.load(std::memory_order_relaxed) && IsWindow(data->hRealWnd)) {
            // The real minimize, now that the ghost has landed. Posted rather
            // than sent: V3 parked this worker in SendMessageTimeout against
            // an application thread that may be busy, and a bypass property
            // cannot even be set on an elevated window from a medium-IL
            // process. The GenieBypass property stays for the in-process
            // hooks; g_AnimActive still holds the window as a second guard.
            SetPropW(data->hRealWnd, L"GenieBypass", (HANDLE)1);
            if (ShowWindowAsync_Original) ShowWindowAsync_Original(data->hRealWnd, SW_MINIMIZE);
            else ShowWindowAsync(data->hRealWnd, SW_MINIMIZE);
            for (int i = 0; i < 50 && IsWindow(data->hRealWnd) && !IsIconic(data->hRealWnd) &&
                            !g_unloading.load(std::memory_order_relaxed); i++) {
                Sleep(10);
            }
            RemovePropW(data->hRealWnd, L"GenieBypass");
        }
        if (IsWindow(data->hRealWnd)) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
            MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
        }
        MacGenieReturnTaskbarFocus(data);
    }

    if (data->special) {
        // Special ghosts are counted apart and never sit in g_AnimActive.
        g_specialActive.fetch_sub(1, std::memory_order_relaxed);
    } else {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }

    // Release the thread that is holding the close. Done last, and outside the
    // cache lock, so the window is only destroyed once the ghost is gone.
    if (data->hCloseFinished) {
        SetEvent(data->hCloseFinished);
        CloseHandle(data->hCloseFinished);
        data->hCloseFinished = NULL;
    }

    MacTelLog(data);
    delete data;
}

// Shared bail-out for an engine that could not set up its surface (huge
// canvas, GDI handle exhaustion): the window must not stay hidden or with
// transitions disabled, and the snapshot must be freed.
static void MacGenieAbortEngine(MacGenieAnimData* data) {
    if (data->hBitmap) { DeleteObject(data->hBitmap); data->hBitmap = NULL; data->pBits = nullptr; }
    MacGenieFinishAnimation(data);
}

DWORD WINAPI MacGenieAnimThreadClassic(LPVOID lpParam);

// -------------------------------------------------------------------------
// V4: the window shadow under the ghost.
//
// DWM draws a drop shadow around every framed window, but a ghost is a layered
// popup without one: in 3.9 the shadow vanished on the first frame of every
// minimize and close and popped back at the end of every restore. The ghost now
// draws it. The shape is MEASURED, not guessed: tests\shadow_measure.cpp put a
// window over a white and then a black backdrop on Windows 11 (build 26300, 125 %,
// active window) and read the shadow's alpha outside each side of the frame. It
// is not one blur - tight at the top (0.11 -> 0 in 40 px), wider at the sides
// (0.17 -> 0 in 60 px), long underneath (0.31 -> 0 in 104 px) - so the measured
// profiles are used as tables, scaled to the monitor's DPI, and combined
// separably at the corners: a(x, y) = side(dx) * vertical(dy) / plateau.
//
// It is composited BEHIND whatever the effect drew (only alpha grows: the shadow
// is black, premultiplied), and only in a band around the frame, so it costs a
// fraction of a frame. Drawn only when Windows itself draws window shadows
// ("Show shadows under windows"), for framed, non-maximized windows.
// -------------------------------------------------------------------------
#define MAC_SHADOW_MEASURE_SCALE 1.25f
#define MAC_SHADOW_PLATEAU 0.34f
static const float kShadowD[]      = { 0, 2, 4, 6, 8, 12, 16, 20, 24, 28, 32, 36, 40, 48, 56, 64, 72, 80, 88, 96, 104 };
static const float kShadowTop[]    = { .110f, .094f, .086f, .071f, .063f, .047f, .031f, .024f, .012f, .008f, .004f,
                                       .004f, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static const float kShadowBottom[] = { .306f, .294f, .290f, .275f, .271f, .247f, .227f, .212f, .192f, .173f, .157f,
                                       .137f, .125f, .090f, .067f, .043f, .027f, .016f, .008f, .004f, 0 };
static const float kShadowSide[]   = { .169f, .157f, .145f, .133f, .118f, .094f, .078f, .059f, .043f, .031f, .024f,
                                       .016f, .008f, .004f, .004f, 0, 0, 0, 0, 0, 0 };

struct MacShadow {
    bool on = false;
    int reachTop = 0, reachBottom = 0, reachSide = 0;   // px at this DPI
    int inner = 0;                                       // px inside the frame (rounded corners)
    std::vector<uint16_t> top, bottom, side;             // alpha in 1/65535 per px of distance
};

static float MacShadowSample(const float* table, float d125) {
    const int n = (int)(sizeof(kShadowD) / sizeof(kShadowD[0]));
    if (d125 <= 0.0f) return table[0];
    for (int i = 1; i < n; i++) {
        if (d125 <= kShadowD[i]) {
            const float t = (d125 - kShadowD[i - 1]) / (kShadowD[i] - kShadowD[i - 1]);
            return table[i - 1] + (table[i] - table[i - 1]) * t;
        }
    }
    return 0.0f;
}

static void MacShadowInit(MacShadow* sh, float dpiScale) {
    if (!(dpiScale >= 0.5f)) dpiScale = 1.0f;
    const float k = MAC_SHADOW_MEASURE_SCALE / dpiScale;   // our px -> measured px
    auto build = [&](const float* table, std::vector<uint16_t>& out) -> int {
        out.clear();
        for (int d = 0; d < 400; d++) {
            const float a = MacShadowSample(table, ((float)d + 0.5f) * k);
            if (a <= 0.0f && d > 0) break;
            out.push_back((uint16_t)(a * 65535.0f + 0.5f));
        }
        return (int)out.size();
    };
    sh->reachTop = build(kShadowTop, sh->top);
    sh->reachBottom = build(kShadowBottom, sh->bottom);
    sh->reachSide = build(kShadowSide, sh->side);
    sh->inner = (int)(10.0f * dpiScale) + 1;
    sh->on = true;
}

// Windows draws a drop shadow around this window right now (the user setting,
// a framed window, not maximized). Popups and tool windows have other shadows.
static bool MacGenieWindowHasDwmShadow(HWND hWnd) {
    BOOL shadows = FALSE;
    if (!SystemParametersInfoW(SPI_GETDROPSHADOW, 0, &shadows, 0) || !shadows) return false;
    if (IsZoomed(hWnd)) return false;
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const LONG_PTR ex = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (style & WS_CHILD) return false;
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP)) return false;
    if (!(style & (WS_CAPTION | WS_THICKFRAME))) return false;
    BOOL nc = FALSE;
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_NCRENDERING_ENABLED, &nc, sizeof(nc))) || !nc) return false;
    // Full screen (borderless games, kiosk windows): no shadow is visible.
    HMONITOR m = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    RECT wr;
    if (GetMonitorInfoW(m, &mi) && GetWindowRect(hWnd, &wr) &&
        wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
        wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom) {
        return false;
    }
    return true;
}

// Adds a (0..65535) black coverage BEHIND the premultiplied pixel.
static inline void MacShadowBehind(uint32_t* p, uint32_t a16) {
    const uint32_t d = *p;
    const uint32_t da = d >> 24;
    if (da >= 255 || !a16) return;
    const uint32_t add = ((a16 >> 8) * (255 - da) + 127) / 255;
    *p = (d & 0x00FFFFFFu) | ((da + add) << 24);
}

// One canvas row of the shadow of a horizontal span [xL, xR) (canvas px).
// vert = the vertical factor (65535 = inside the frame vertically); the
// interior of the span is skipped unless `outside` (the row is above / below
// the frame, where nothing covers the shadow).
static void MacShadowRow(uint32_t* row, int bw, float xL, float xR, uint32_t vert, uint32_t opacity255,
                         const MacShadow& sh, bool outside) {
    if (!vert || !opacity255 || xR <= xL) return;
    // alpha = side(dx) * vert * opacity: side() is the measured alpha beside the
    // frame, vert the vertical factor normalised to 1 inside the frame (MacShadowVert).
    const uint64_t k = (uint64_t)vert * opacity255;                     // / (65535 * 255)
    auto coverage = [&](int dx) -> uint32_t {
        const uint64_t s = dx < (int)sh.side.size() ? sh.side[(size_t)dx] : 0u;
        return (uint32_t)(s * k / (65535ull * 255ull));
    };
    // Inside the frame horizontally: the plateau.
    const uint32_t mid = (uint32_t)((uint64_t)(MAC_SHADOW_PLATEAU * 65535.0f) * k / (65535ull * 255ull));
    const int l0 = MacMax(0, (int)floorf(xL) - sh.reachSide);
    const int r1 = MacMin(bw, (int)ceilf(xR) + sh.reachSide);
    if (outside) {
        // Above / below the frame: the whole extent.
        for (int x = l0; x < r1; x++) {
            const float c = (float)x + 0.5f;
            const uint32_t a = c < xL ? coverage((int)(xL - c)) : (c >= xR ? coverage((int)(c - xR)) : mid);
            MacShadowBehind(row + x, a);
        }
        return;
    }
    // Beside the frame: the left and right bands (a few px inside, for corners).
    const int lEnd = MacMin(r1, (int)ceilf(xL) + sh.inner);
    for (int x = l0; x < lEnd; x++) {
        const float c = (float)x + 0.5f;
        MacShadowBehind(row + x, c < xL ? coverage((int)(xL - c)) : mid);
    }
    const int rBeg = MacMax(lEnd, (int)floorf(xR) - sh.inner);
    for (int x = rBeg; x < r1; x++) {
        const float c = (float)x + 0.5f;
        MacShadowBehind(row + x, c >= xR ? coverage((int)(c - xR)) : mid);
    }
}

// The vertical factor (0..65535) of a row at canvas y for a frame [y0, y1).
static inline uint32_t MacShadowVert(const MacShadow& sh, float y, float y0, float y1, bool* outside) {
    *outside = false;
    if (y < y0) {
        *outside = true;
        const int d = (int)(y0 - y);
        if (d >= (int)sh.top.size()) return 0;
        return (uint32_t)((double)sh.top[(size_t)d] / MAC_SHADOW_PLATEAU);
    }
    if (y >= y1) {
        *outside = true;
        const int d = (int)(y - y1);
        if (d >= (int)sh.bottom.size()) return 0;
        return (uint32_t)MacMin(65535.0, (double)sh.bottom[(size_t)d] / MAC_SHADOW_PLATEAU);
    }
    return 65535u;
}

// The shadow of a rectangle (canvas px), rows [rowBegin, rowEnd), behind
// what is already in the canvas.
static void MacShadowRect(uint32_t* dst, int bw, int bh, float x0, float y0, float x1, float y1, float opacity,
                          const MacShadow& sh, int rowBegin, int rowEnd) {
    if (!sh.on || opacity <= 0.003f || x1 <= x0 || y1 <= y0) return;
    const uint32_t op255 = (uint32_t)(MacClampf(opacity, 0.0f, 1.0f) * 255.0f + 0.5f);
    const int ya = MacMax(MacMax(0, rowBegin), (int)floorf(y0) - sh.reachTop);
    const int yb = MacMin(MacMin(bh, rowEnd), (int)ceilf(y1) + sh.reachBottom);
    for (int y = ya; y < yb; y++) {
        bool outside;
        const uint32_t vert = MacShadowVert(sh, (float)y + 0.5f, y0, y1, &outside);
        MacShadowRow(dst + (size_t)y * bw, bw, x0, x1, vert, op255, sh, outside);
    }
}

// The canvas box a rectangle's shadow can touch.
static RECT MacShadowBox(const MacShadow& sh, float x0, float y0, float x1, float y1) {
    return RECT{ (LONG)floorf(x0) - sh.reachSide, (LONG)floorf(y0) - sh.reachTop,
                 (LONG)ceilf(x1) + sh.reachSide, (LONG)ceilf(y1) + sh.reachBottom };
}

// -------------------------------------------------------------------------
// Classic Genie: True Genie Scanline Engine with Bilinear AA & Mipmaps
//
// Every scanline writes only its own row of the surface, so the rasterizer
// splits a frame into horizontal bands rendered concurrently on a small
// per-animation worker pool (derrunk's original design, dropped in V3). The
// animation thread renders the last band itself and waits for the rest.
// -------------------------------------------------------------------------

// Read-only per-frame context shared by all render workers.
struct ClassicFrameCtx {
    DWORD* base;
    int totalW, minX, maxX, minY, maxY;
    const std::vector<MipLevel>* mips;
    // How many pyramid levels are FILLED (the plan allocates them all up front,
    // a falling animation fills levels >= 1 after its first frame is up).
    int mipLevels;
    int W;
    double T, B, L, R, dockL, dockR, G;
    bool topDock;
    // Per-frame fields, written by the animation thread before each dispatch.
    double b, bandTop, bandH, bandBot, scaleY, fade;
    float bScale;
    const MacShadow* shadow;   // V4: null = no shadow
};

// Renders scanlines [y0, y1). This is byte-for-byte the loop body that used to
// live inline in the frame loop - only the enclosing locals moved into the
// context. Each scanline touches exactly one row of the surface, so bands
// never overlap and need no synchronization.
// One mip level's sampling state for one output row.
struct GenieRowTap {
    const DWORD* r0;
    const DWORD* r1;
    int mw, fxMax, wy, fx16, dfx16;
};

static inline void GenieRowTapSetup(GenieRowTap* t, const MipLevel& M, double v, double xs, double xL,
                                    double width) {
    const int mw = M.w, mh = M.h;
    double fy = v * (double)mh - 0.5;
    if (fy < 0.0) fy = 0.0; else if (fy > mh - 1) fy = (double)(mh - 1);
    int y0i = (int)fy;
    int wy = (int)((fy - y0i) * 256.0 + 0.5);
    if (wy >= 256) { y0i++; wy = 0; }
    if (y0i > mh - 1) { y0i = mh - 1; wy = 0; }
    const int y1i = (y0i + 1 < mh) ? y0i + 1 : y0i;
    t->r0 = M.p + (size_t)y0i * mw;
    t->r1 = M.p + (size_t)y1i * mw;
    t->mw = mw;
    t->fxMax = (mw - 1) << 16;
    t->wy = wy;
    t->dfx16 = (int)(((double)mw / width) * 65536.0 + 0.5);
    t->fx16 = (int)((((xs + 0.5 - xL) / width) * (double)mw - 0.5) * 65536.0);
}

// Renders scanlines [y0, y1). Each scanline touches exactly one row of the
// surface, so bands never overlap and need no synchronization.
//
// V4: trilinear mip filtering. 3.9 switched to the next pyramid level at a hard
// 50 % threshold, which could show as a band across the shrinking window (the
// rows on either side of the threshold sampled different levels). Each row now
// has a fractional level of detail - log2 of its minification, biased by -0.5 so
// the window stays as sharp as before at moderate shrink - and blends the two
// nearest levels. Rows at an integer level (all of them at full size, so p = 0
// stays the snapshot byte for byte) take the single-level path unchanged.
static void ClassicRenderBand(const ClassicFrameCtx* c, int y0, int y1) {
    const std::vector<MipLevel>& mips = *c->mips;
    const int W = c->W, minX = c->minX, maxX = c->maxX, minY = c->minY, maxY = c->maxY;
    const int totalW = c->totalW;
    const double T = c->T, B = c->B, L = c->L, R = c->R;
    const double dockL = c->dockL, dockR = c->dockR, G = c->G;
    const double b = c->b, bandTop = c->bandTop, bandH = c->bandH, bandBot = c->bandBot;
    const double scaleY = c->scaleY, fade = c->fade;
    const bool topDock = c->topDock;
    const float bScale = c->bScale;
    DWORD* base = c->base;

    // The silhouette's span on the row through yc (screen coordinates).
    auto spanAt = [&](double yc, double* xL, double* xR) {
        if (bScale != 1.0f) {
            const double cx = (L + R) * 0.5, wd = (double)W * bScale;
            *xL = cx - wd * 0.5;
            *xR = cx + wd * 0.5;
        } else {
            const double s = topDock ? Clamp01((B - yc) / G) : Clamp01((yc - T) / G);
            const double e = b * Smoother(s);
            *xL = L + (dockL - L) * e;
            *xR = R + (dockR - R) * e;
        }
    };
    const MacShadow* sh = c->shadow;
    const uint32_t shOp = sh ? (uint32_t)(fade * 255.0 + 0.5) : 0u;

    for (int yi = y0; yi < y1; yi++) {
        if (yi < minY || yi >= maxY) continue;
        double yc = (double)yi + 0.5;

        double covY = dmin((double)yi + 1.0, bandBot) - dmax((double)yi, bandTop);
        if (covY <= 0.0) {
            // V4: above / below the window - only its shadow, from the nearest row.
            if (sh && shOp) {
                double sxL, sxR;
                spanAt(yc < bandTop ? bandTop + 0.5 : bandBot - 0.5, &sxL, &sxR);
                bool outside;
                const uint32_t vert = MacShadowVert(*sh, (float)yc, (float)bandTop, (float)bandBot, &outside);
                MacShadowRow((uint32_t*)(base + (size_t)(yi - minY) * totalW), totalW,
                             (float)(sxL - minX), (float)(sxR - minX), vert, shOp, *sh, true);
            }
            continue;
        }
        if (covY > 1.0) covY = 1.0;
        double covRow = covY * fade;

        double xL, xR, width;
        spanAt(yc, &xL, &xR);
        width = xR - xL;
        if (width < 1.0) width = 1.0;

        // Level of detail from the more minified axis.
        const double sMin = dmin(width / (double)W, scaleY);
        int lvl = 0;
        int blend = 0;                        // 0..255 weight of level lvl + 1
        if (sMin < 0.7071 && c->mipLevels > 1) {
            double lod = -log2(dmax(sMin, 1e-6)) - 0.5;
            const double maxLod = (double)(c->mipLevels - 1);
            if (lod > maxLod) lod = maxLod;
            if (lod < 0.0) lod = 0.0;
            lvl = (int)lod;
            if (lvl > c->mipLevels - 1) lvl = c->mipLevels - 1;
            if (lvl < c->mipLevels - 1) blend = (int)((lod - lvl) * 256.0);
            if (blend > 255) blend = 255;
        }

        const double v = Clamp01((yc - bandTop) / bandH);
        int xs = imax((int)floor(xL), minX);
        int xe = imin((int)ceil(xR),  maxX);
        if (xe <= xs) {
            if (sh && shOp) {
                MacShadowRow((uint32_t*)(base + (size_t)(yi - minY) * totalW), totalW,
                             (float)(xL - minX), (float)(xR - minX), 65535u, shOp, *sh, false);
            }
            continue;
        }

        GenieRowTap ta, tb;
        GenieRowTapSetup(&ta, mips[lvl], v, (double)xs, xL, width);
        if (blend) GenieRowTapSetup(&tb, mips[lvl + 1], v, (double)xs, xL, width);
        DWORD* out = base + (size_t)(yi - minY) * totalW;

        // Samples are premultiplied BGRA straight from the snapshot; coverage
        // scales all four channels. (V3 OR'd alpha to 255, which turned the
        // transparent rounded-corner pixels into black wedges.)
        auto sample = [&](int step) -> DWORD {
            const DWORD pa = SampleBilin(ta.r0, ta.r1, ta.mw, ta.fx16 + step * ta.dfx16, ta.fxMax, ta.wy);
            if (!blend) return pa;
            const DWORD pb = SampleBilin(tb.r0, tb.r1, tb.mw, tb.fx16 + step * tb.dfx16, tb.fxMax, tb.wy);
            return Lerp8(pa, pb, blend);
        };

        // Left subpixel edge
        {
            DWORD pix = sample(0);
            double cx = dmin((double)xs + 1.0, xR) - dmax((double)xs, xL);
            double cov = covRow * Clamp01(cx);
            out[xs - minX] = (cov >= 0.999) ? pix : CovMulP(pix, (unsigned)(cov * 256.0 + 0.5));
        }

        // Inner span
        DWORD* op = out + (xs + 1 - minX);
        const int nInner = (xe - 1) - (xs + 1);
        if (!blend) {
            // Single level: the 3.9 loop's output, byte for byte (SSE2, V4).
            int fxc = ta.fx16 + ta.dfx16;
            if (covRow >= 0.999) {
                if (g_macNoSimd) {
                    for (int k = 0; k < nInner; k++, fxc += ta.dfx16)
                        *op++ = SampleBilin(ta.r0, ta.r1, ta.mw, fxc, ta.fxMax, ta.wy);
                } else if (nInner > 0) {
                    SampleBilinRunSse2(op, nInner, ta.r0, ta.r1, ta.mw, fxc, ta.dfx16, ta.fxMax, ta.wy);
                }
            } else {
                unsigned a256 = (unsigned)(covRow * 256.0 + 0.5);
                for (int k = 0; k < nInner; k++, fxc += ta.dfx16)
                    *op++ = CovMulP(SampleBilin(ta.r0, ta.r1, ta.mw, fxc, ta.fxMax, ta.wy), a256);
            }
        } else {
            int fa = ta.fx16 + ta.dfx16, fb = tb.fx16 + tb.dfx16;
            const unsigned a256 = (covRow >= 0.999) ? 256u : (unsigned)(covRow * 256.0 + 0.5);
            for (int k = 0; k < nInner; k++, fa += ta.dfx16, fb += tb.dfx16) {
                const DWORD pa = SampleBilin(ta.r0, ta.r1, ta.mw, fa, ta.fxMax, ta.wy);
                const DWORD pb = SampleBilin(tb.r0, tb.r1, tb.mw, fb, tb.fxMax, tb.wy);
                *op++ = CovMulP(Lerp8(pa, pb, blend), a256);
            }
        }

        // Right subpixel edge
        if (xe - 1 > xs) {
            DWORD pix = sample(xe - 1 - xs);
            double cx = dmin((double)xe, xR) - dmax((double)(xe - 1), xL);
            double cov = covRow * Clamp01(cx);
            out[xe - 1 - minX] = (cov >= 0.999) ? pix : CovMulP(pix, (unsigned)(cov * 256.0 + 0.5));
        }
        // V4: the window's shadow beside this row, behind what was just drawn.
        if (sh && shOp) {
            MacShadowRow((uint32_t*)out, totalW, (float)(xL - minX), (float)(xR - minX), 65535u, shOp, *sh, false);
        }
    }
}

// Scanline job for the shared ParallelPool. Chunks of rows are claimed from an
// atomic counter, so the (very uneven) per-row cost balances itself across fast
// and slow cores without changing a single output byte: every row is rendered
// independently into its own row of the surface.
static void ClassicBandJob(void* c, int y0, int y1) {
    ClassicRenderBand((const ClassicFrameCtx*)c, y0, y1);
}

DWORD WINAPI MacGenieAnimThreadClassic(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    MacTelBeginThread(data, L"classic", nullptr);

    const int W = data->width;
    const int H = data->height;
    const double L = (double)data->targetRect.left;
    const double R = (double)data->targetRect.right;
    const double T = (double)data->targetRect.top;
    const double B = (double)data->targetRect.bottom;
    double dockYSeed = (double)data->targetDockY;
    if (data->requestedUnhide) {
        // The dock target was resolved while the auto-hidden taskbar was
        // still slid out; wait for it and re-derive the Y from its real rect.
        MONITORINFO mi = {};
        mi.cbSize = sizeof(MONITORINFO);
        HWND hTray = FindTaskbarForMonitor(data->hMon);
        if (hTray && GetMonitorInfoW(data->hMon, &mi)) {
            MacGenieWaitTaskbarExpanded(hTray, mi.rcMonitor, 150);
            dockYSeed = (double)(GetDockIconCenterY(data->hMon, mi.rcMonitor, data->dockPos)
                                 + data->dockOffsetY);
        }
    }
    const double Dy = dockYSeed;
    const double Dx = (double)data->targetDockX;

    double iconW = (double)data->funnelWidth;
    if (iconW < 8.0) iconW = 8.0;
    const double dockL = Dx - iconW * 0.5;
    const double dockR = Dx + iconW * 0.5;
    const bool topDock = (data->dockPos == DOCK_TOP);

    double G = topDock ? (B - Dy) : (Dy - T);
    if (G < 48.0) G = 48.0;

    // derrunk's two phases: bend into the tunnel, then slide through it, overlapping
    // by 30 % of the bend (V4: the bend share is the "Genie bend phase" setting).
    const double bendEnd = MacClampInt(g_genieBendPct.load(std::memory_order_relaxed), 15, 70) / 100.0;
    const double slideStart = bendEnd * 0.70;

    // V4: the shadow's reach widens the canvas.
    MacShadow shadow;
    if (data->shadow) MacShadowInit(&shadow, GetMonitorDpiScale(data->hMon));
    const int PAD = shadow.on ? MacMax(32, MacMax(shadow.reachBottom, shadow.reachSide) + 2) : 32;
    int minX = (int)floor(dmin(L, dockL)) - PAD;
    int maxX = (int)ceil (dmax(R, dockR)) + PAD;
    int minY = (int)floor(dmin(dmin(T, B), Dy)) - PAD;
    int maxY = (int)ceil (dmax(dmax(T + G, B), Dy)) + PAD;

    // The restore bounce plays only while rising; a reversible animation may end
    // up rising, so its canvas always has room for the overshoot (V4).
    const float bounceStr = (float)data->bounceStr;
    const double bounceDurSet = (bounceStr > 0.01f) ? (double)MacMax(0, data->bounceDurMs) : 0.0;
    if (bounceDurSet > 1.0 && (data->isRising || data->reversible)) {
        float maxOvershoot = (bounceStr / 100.0f) * 0.18f;
        int padBounce = (int)(MacMax(W, H) * maxOvershoot + 1);
        minX -= padBounce; maxX += padBounce;
        minY -= padBounce; maxY += padBounce;
    }

    // Keep the canvas on the monitors involved (a 4K window with a big bounce
    // pad would otherwise allocate well past the screen).
    {
        RECT monUnion = GetGenieMonitorUnion(data->targetRect, data->hMon);
        int l = minX, t = minY, r = maxX, b = maxY;
        ClampGenieCanvas(l, t, r, b, data->targetRect, (int)Dx, (int)(iconW * 0.5) + 8, monUnion);
        if (!MacGenieClipCanvasToDesktop(l, t, r, b)) {
            MacGenieAbortEngine(data);
            return 0;
        }
        minX = l; minY = t; maxX = r; maxY = b;
    }

    int totalW = maxX - minX; if (totalW < 1) totalW = 1;
    int totalH = maxY - minY; if (totalH < 1) totalH = 1;

    HDC hScreenDC = GetDC(NULL);
    HDC hSurfDC   = hScreenDC ? CreateCompatibleDC(hScreenDC) : NULL;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = totalW;
    bmi.bmiHeader.biHeight      = -totalH;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = NULL;
    HBITMAP hSurf = hSurfDC ? CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0) : NULL;
    HWND hGhost = NULL;
    if (hSurf && pBits) {
        hGhost = CreateWindowExW(
            MacGenieGhostExStyle(data),
            L"STATIC", NULL, WS_POPUP,
            minX, minY, totalW, totalH, NULL, NULL, NULL, NULL);
    }
    if (!hGhost) {
        // Surface or ghost creation failed (V3 dereferenced the NULL bits).
        if (hSurf) DeleteObject(hSurf);
        if (hSurfDC) DeleteDC(hSurfDC);
        if (hScreenDC) ReleaseDC(NULL, hScreenDC);
        MacGenieAbortEngine(data);
        return 0;
    }
    HBITMAP hOldSurf = (HBITMAP)SelectObject(hSurfDC, hSurf);
    DWORD* base = (DWORD*)pBits;
    memset(pBits, 0, (size_t)totalW * totalH * 4);

    // Worker pool: half the cores minus one, capped at 6, and rationed across
    // processes (MacGenieAcquireBudgetSlot) so a Win+M storm of Classic genies
    // cannot oversubscribe the machine. The animation thread works too. Zero
    // workers (small machines, no budget left, event failure) is the plain
    // single-threaded path.
    int budgetSlots = 0;
    HANDLE hBudget = MacGenieAcquireBudgetSlot(&budgetSlots);
    ParallelPool pool;
    if (budgetSlots > 0) PoolInit(&pool, budgetSlots);

    // The pyramid is PLANNED here (O(levels): uninitialised buffers) but only
    // level 0 is usable. A falling animation starts at full width, which never
    // samples a lower level, so levels >= 1 are filled - in parallel - while the
    // first frame waits for its vblank, instead of before it (the user's UI
    // thread is blocked on that first frame). A rising animation starts fully
    // narrowed and needs the whole pyramid for frame 0.
    std::vector<MipLevel> mips;
    MipsPlan((const DWORD*)data->pBits, W, H, mips);
    bool mipsFilled = false;
    if (data->isRising) {
        MipsFillParallel(&pool, mips, 1);
        mipsFilled = true;
    }

    // Constant context for the whole animation.
    ClassicFrameCtx fctx = {};
    fctx.base = base;
    fctx.totalW = totalW;
    fctx.minX = minX;  fctx.maxX = maxX;
    fctx.minY = minY;  fctx.maxY = maxY;
    fctx.mips = &mips;
    fctx.mipLevels = mipsFilled ? (int)mips.size() : 1;
    fctx.W = W;
    fctx.T = T;  fctx.B = B;
    fctx.L = L;  fctx.R = R;
    fctx.dockL = dockL;  fctx.dockR = dockR;
    fctx.G = G;
    fctx.topDock = topDock;
    fctx.shadow = shadow.on ? &shadow : nullptr;

    const double animDur = (double)data->durationMs;

    MacAnimClock clock;
    MacAnimClockInit(&clock);
    LARGE_INTEGER qpcNow;

    MacGenieFramePacer framePacer;
    MacGenieCreateFramePacer(&framePacer, data->hMon);
    if (data->telOn) data->tel.intervalMs = framePacer.intervalMs;

    BOOL firstFrame = TRUE;
    int prevY0 = 0, prevY1 = 0;
    int prevX0 = 0, prevX1 = 0;
    MacTelSetupDone(data);

    for (;;) {
        MacGenieDrainMessages();
        QueryPerformanceCounter(&qpcNow);
        const LARGE_INTEGER qpcFrameStart = qpcNow;
        if (!firstFrame && MacAnimApplyReversal(&clock, data, animDur, qpcNow.QuadPart) && !mipsFilled) {
            MipsFillParallel(&pool, mips, 1);   // a rising frame may need the whole pyramid
            mipsFilled = true;
            fctx.mipLevels = (int)mips.size();
        }
        const double bounceDur = data->isRising ? bounceDurSet : 0.0;
        const double totalDur = animDur + (bounceDur > 1.0 ? bounceDur : 0.0);
        double elapsedMs = MacAnimClockMs(&clock, qpcNow.QuadPart);
        BOOL lastFrame = (elapsedMs >= totalDur);

        double p = elapsedMs / animDur;
        if (p > 1.0) p = 1.0;
        double t = data->isRising ? (1.0 - p) : p;

        float bScale = 1.0f;
        if (bounceDur > 1.0 && elapsedMs >= animDur) {
            float tB = MacClampf((float)((elapsedMs - animDur) / bounceDur), 0.0f, 1.0f);
            bScale = MacSpringBounce(tB, bounceStr);
        }

        // Two-phase Perlin smootherstep curve
        double b = Smoother(Clamp01(t / bendEnd));
        double m = Smoother(Clamp01((t - slideStart) / (1.0 - slideStart)));

        double stretched = H + (G - H) * b;
        double bandTop, bandH, bandBot;
        if (bScale != 1.0f) {
            double cy = (T + B) * 0.5;
            bandH = (double)H * bScale;
            if (bandH < 1.0) bandH = 1.0;
            bandTop = cy - bandH * 0.5;
            bandBot = cy + bandH * 0.5;
        } else if (topDock) {
            bandBot = B - m * G;
            bandH   = stretched * (1.0 - m);
            if (bandH < 1.0) bandH = 1.0;
            bandTop = bandBot - bandH;
        } else {
            bandTop = T + m * G;
            bandH   = stretched * (1.0 - m);
            if (bandH < 1.0) bandH = 1.0;
            bandBot = bandTop + bandH;
        }

        int iy0 = imax((int)floor(bandTop), minY);
        int iy1 = imin((int)ceil(bandBot),  maxY);

        int cx0 = minX, cx1 = minX;
        if (iy1 > iy0) {
            if (bScale != 1.0f) {
                double cx = (L + R) * 0.5;
                double curW = (double)W * bScale;
                cx0 = imax((int)floor(cx - curW * 0.5), minX);
                cx1 = imin((int)ceil (cx + curW * 0.5), maxX);
            } else {
                double sA = topDock ? Clamp01((B - (double)iy0) / G) : Clamp01(((double)iy0 + 0.5 - T) / G);
                double sB = topDock ? Clamp01((B - (double)iy1) / G) : Clamp01(((double)iy1 - 0.5 - T) / G);
                double eA = b * Smoother(sA), eB = b * Smoother(sB);
                double xlA = L + (dockL - L) * eA, xlB = L + (dockL - L) * eB;
                double xrA = R + (dockR - R) * eA, xrB = R + (dockR - R) * eB;
                cx0 = imax((int)floor(dmin(xlA, xlB)), minX);
                cx1 = imin((int)ceil (dmax(xrA, xrB)), maxX);
            }
            if (cx1 < cx0) cx1 = cx0;
        }

        // V4: the shadow reaches past the window on every side.
        int shY0 = iy0, shY1 = iy1;
        if (shadow.on && iy1 > iy0) {
            shY0 = imax(minY, iy0 - shadow.reachTop);
            shY1 = imin(maxY, iy1 + shadow.reachBottom);
            cx0 = imax(minX, cx0 - shadow.reachSide);
            cx1 = imin(maxX, cx1 + shadow.reachSide);
        }
        int curY0 = shY0 - minY, curY1 = imax(shY1 - minY, shY0 - minY);
        int curX0 = cx0 - minX, curX1 = cx1 - minX;

        // Dirty rectangle 2D clear
        int uy0 = imin(prevY0, curY0), uy1 = imax(prevY1, curY1);
        int ux0 = imin(prevX0, curX0), ux1 = imax(prevX1, curX1);
        for (int r = uy0; r < uy1; r++) {
            if (r >= 0 && r < totalH && ux1 > ux0) {
                memset(base + (size_t)r * totalW + ux0, 0, (size_t)(ux1 - ux0) * 4);
            }
        }
        prevY0 = curY0; prevY1 = curY1;
        prevX0 = curX0; prevX1 = curX1;

        double scaleY = bandH / (double)H;
        double fade = (t > 0.88) ? (1.0 - (t - 0.88) / 0.12) : 1.0;
        if (fade < 0.0) fade = 0.0;

        // Publish this frame's parameters, then rasterize on the pool. The
        // workers only ever run inside PoolRun, so the context is never written
        // while anyone reads it.
        fctx.b = b;
        fctx.bandTop = bandTop;
        fctx.bandH = bandH;
        fctx.bandBot = bandBot;
        fctx.scaleY = scaleY;
        fctx.fade = fade;
        fctx.bScale = bScale;

        // Small bands stay on this thread: the hand-off costs more than it saves.
        PoolRun(&pool, ClassicBandJob, &fctx, shY0, shY1, 16, 128);
        const LONGLONG telRenderEnd = MacTelMark(data);

        POINT ptDst = { minX, minY };
        SIZE  sz    = { totalW, totalH };
        // Only the union of the previous and current band goes to DWM.
        RECT dirty = { ux0, uy0, imax(ux1, ux0 + 1), imax(uy1, uy0 + 1) };
        MacGeniePresent(hGhost, hScreenDC, ptDst, sz, hSurfDC, 255, firstFrame ? nullptr : &dirty);

        if (firstFrame) {
            MacGenieShowGhostForData(hGhost, data, true);
            MacTelFrame0Shown(data);
        }
        MacTelFrame(data, qpcFrameStart.QuadPart, telRenderEnd, MacTelMark(data));

        if (lastFrame) {
            DwmFlush();
            break;
        }
        if (g_unloading.load(std::memory_order_relaxed)) break;
        if (MacGenieSpecialCancelled(data)) break;

        // This flush must stay ahead of the first-frame signal, and being a full
        // vblank wait it already paces this frame.
        bool paced = false;
        if (firstFrame) {
            // (Falling only) fill the lower pyramid levels while the compositor
            // takes the first frame; see the comment where the pyramid is planned.
            if (!mipsFilled) {
                MipsFillParallel(&pool, mips, 1);
                mipsFilled = true;
                fctx.mipLevels = (int)mips.size();
            }
            DwmFlush();
            paced = true;
            firstFrame = FALSE;
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
            MacTelFirstFrame(data);
            MacGeniePacerRebase(&framePacer);
            MacAnimClockStartNow(&clock);   // frame 0 is on screen: time starts now
            MacGenieCloakClosingSource(data);
        }

        if (!paced) MacGeniePaceFrame(&framePacer);
    }
    if (data->telOn) data->tel.strideMax = framePacer.strideMax;

    // Wind the render pool down before the surface and mips it references go
    // away, and give its budget slots back.
    PoolShutdown(&pool);
    MacGenieReleaseBudgetSlot(hBudget, budgetSlots);

    MacGenieDestroyFramePacer(&framePacer);
    SelectObject(hSurfDC, hOldSurf);
    DeleteObject(hSurf);
    DeleteDC(hSurfDC);
    ReleaseDC(NULL, hScreenDC);
    MacGenieReleaseSnapshot(data);
    MacGenieRevealBeforeGhostGone(data);
    DestroyWindow(hGhost);

    MacGenieFinishAnimation(data);
    return 0;
}

// -------------------------------------------------------------------------
// Black Hole Engine: Optimized Dirty-Band Particle Vortex
//
// V3.7: the per-frame block pass runs on the shared ParallelPool in two steps
// that reproduce V3.6's output exactly:
//   A. every block's destination is computed independently (the sinf / cosf per
//      block that dominated the old loop) - parallel by block index;
//   B. blocks are binned (stable, so original order is kept) into 64-row
//      destination bands and each band is blended by one worker. A pixel belongs
//      to exactly one band and sees its blocks in the original order, so the
//      over-compositing result is identical to the single-threaded loop.
// -------------------------------------------------------------------------
#define BH_BAND_ROWS 64

struct BHPassCtx {
    const BHBlock* blocks;
    int* dxs;                       // block destination X, INT_MIN = culled
    int* dys;
    float cx, cy, twistForce, pull, invMaxR;
    int bSize, boundW, boundH, W, H;
    const uint32_t* srcPx;
    uint32_t* dstPx;
    float particleAlpha;
    bool fastPath;
    const int* binStart;            // nBands + 1 offsets into binList
    const int* binList;
};

static void BHDestJob(void* c, int i0, int i1) {
    const BHPassCtx* x = (const BHPassCtx*)c;
    for (int i = i0; i < i1; i++) {
        const BHBlock& blk = x->blocks[i];
        float r_new = blk.r0 * (1.0f - x->pull);
        float a_new = blk.a0 + x->twistForce * x->pull * (1.5f - blk.r0 * x->invMaxR);
        int destX = (int)(x->cx + cosf(a_new) * r_new) - x->bSize / 2;
        int destY = (int)(x->cy + sinf(a_new) * r_new) - x->bSize / 2;
        if (destX + x->bSize <= 0 || destX >= x->boundW ||
            destY + x->bSize <= 0 || destY >= x->boundH) {
            x->dxs[i] = INT_MIN;
            x->dys[i] = INT_MIN;
        } else {
            x->dxs[i] = destX;
            x->dys[i] = destY;
        }
    }
}

// Rows of `blk` that land on the canvas: [destY + by0, destY + by1) (empty when
// by0 >= by1) and the matching column range - the same clipping V3.6 did inline.
static inline bool BHClipBlock(const BHPassCtx* x, const BHBlock& blk, int destX, int destY,
                               int* by0, int* by1, int* bx0, int* bx1) {
    *by0 = (destY < 0) ? -destY : 0;
    *by1 = x->bSize;
    if (blk.y + *by1 > x->H) *by1 = x->H - blk.y;
    if (destY + *by1 > x->boundH) *by1 = x->boundH - destY;
    *bx0 = (destX < 0) ? -destX : 0;
    *bx1 = x->bSize;
    if (blk.x + *bx1 > x->W) *bx1 = x->W - blk.x;
    if (destX + *bx1 > x->boundW) *bx1 = x->boundW - destX;
    return !(*bx0 >= *bx1 || *by0 >= *by1);
}

static void BHBlendJob(void* c, int band0, int band1) {
    const BHPassCtx* x = (const BHPassCtx*)c;
    for (int band = band0; band < band1; band++) {
        const int rowLo = band * BH_BAND_ROWS;
        const int rowHi = imin(x->boundH, rowLo + BH_BAND_ROWS);
        for (int k = x->binStart[band]; k < x->binStart[band + 1]; k++) {
            const int i = x->binList[k];
            const BHBlock& blk = x->blocks[i];
            const int destX = x->dxs[i], destY = x->dys[i];
            int by0, by1, bx0, bx1;
            if (!BHClipBlock(x, blk, destX, destY, &by0, &by1, &bx0, &bx1)) continue;
            const int rlo = imax(by0, rowLo - destY);
            const int rhi = imin(by1, rowHi - destY);
            for (int by = rlo; by < rhi; by++) {
                uint32_t* dstRow = x->dstPx + (size_t)(destY + by) * x->boundW + destX;
                const uint32_t* srcRow = x->srcPx + (size_t)(blk.y + by) * x->W + blk.x;
                if (x->fastPath) {
                    for (int bx = bx0; bx < bx1; bx++) {
                        uint32_t px = srcRow[bx];
                        if (px) dstRow[bx] = MacAlphaBlendPixels(dstRow[bx], px);
                    }
                } else {
                    for (int bx = bx0; bx < bx1; bx++) {
                        uint32_t px = srcRow[bx];
                        if (!px) continue;
                        uint32_t pm = MacPremultiplyBlend(px, x->particleAlpha);
                        if (pm) dstRow[bx] = MacAlphaBlendPixels(dstRow[bx], pm);
                    }
                }
            }
        }
    }
}

// Bins the visible blocks of the current frame by the destination bands their
// rows touch (stable counting sort: each bin keeps the original block order) and
// reports the canvas rows the frame touches. binStart must have nBands + 1
// entries, binCount nBands + 1; binList is (re)sized here.
static void BHBinBlocks(const BHPassCtx& pass, int nBlocks, int nBands,
                        std::vector<int>& binStart, std::vector<int>& binCount,
                        std::vector<int>& binList, int* curMinY, int* curMaxY) {
    std::fill(binCount.begin(), binCount.end(), 0);
    for (int i = 0; i < nBlocks; i++) {
        const int destY = pass.dys[i];
        if (destY == INT_MIN) continue;
        *curMinY = imin(*curMinY, destY);
        *curMaxY = imax(*curMaxY, destY + pass.bSize);
        int by0, by1, bx0, bx1;
        if (!BHClipBlock(&pass, pass.blocks[i], pass.dxs[i], destY, &by0, &by1, &bx0, &bx1)) continue;
        const int b0 = (destY + by0) / BH_BAND_ROWS;
        const int b1 = (destY + by1 - 1) / BH_BAND_ROWS;
        for (int b = b0; b <= b1; b++) binCount[b]++;
    }
    int total = 0;
    for (int b = 0; b < nBands; b++) { binStart[b] = total; total += binCount[b]; }
    binStart[nBands] = total;
    binList.assign((size_t)total, 0);
    std::fill(binCount.begin(), binCount.end(), 0);
    for (int i = 0; i < nBlocks; i++) {
        const int destY = pass.dys[i];
        if (destY == INT_MIN) continue;
        int by0, by1, bx0, bx1;
        if (!BHClipBlock(&pass, pass.blocks[i], pass.dxs[i], destY, &by0, &by1, &bx0, &bx1)) continue;
        const int b0 = (destY + by0) / BH_BAND_ROWS;
        const int b1 = (destY + by1 - 1) / BH_BAND_ROWS;
        for (int b = b0; b <= b1; b++) binList[(size_t)binStart[b] + binCount[b]++] = i;
    }
}

// Bilinear scale of the (premultiplied BGRA) snapshot onto the canvas for the
// restore bounce. V3.6 used StretchBlt(COLORONCOLOR), which is nearest-neighbour:
// the pop visibly stair-stepped text. Writes only the rows [dstY, dstY + curH)
// that fall on the canvas; the canvas there was cleared by the dirty-band clear.
static void BHScaleBilinear(uint32_t* canvas, int canvasW, int canvasH,
                            int dstX, int dstY, int curW, int curH,
                            const uint32_t* src, int W, int H) {
    if (curW <= 0 || curH <= 0) return;
    const int yLo = imax(0, dstY), yHi = imin(canvasH, dstY + curH);
    const int xLo = imax(0, dstX), xHi = imin(canvasW, dstX + curW);
    if (yLo >= yHi || xLo >= xHi) return;
    const int fxMax = (W - 1) << 16;
    const double stepX = (double)W / (double)curW;
    const int dfx16 = (int)(stepX * 65536.0 + 0.5);
    for (int yy = yLo; yy < yHi; yy++) {
        double fy = ((yy - dstY) + 0.5) * (double)H / (double)curH - 0.5;
        if (fy < 0.0) fy = 0.0; else if (fy > H - 1) fy = (double)(H - 1);
        int y0 = (int)fy;
        int wy = (int)((fy - y0) * 256.0 + 0.5);
        if (wy >= 256) { y0++; wy = 0; }
        if (y0 > H - 1) { y0 = H - 1; wy = 0; }
        const int y1 = (y0 + 1 < H) ? y0 + 1 : y0;
        const uint32_t* r0 = src + (size_t)y0 * W;
        const uint32_t* r1 = src + (size_t)y1 * W;
        uint32_t* out = canvas + (size_t)yy * canvasW + xLo;
        int fx = (int)(((((double)(xLo - dstX) + 0.5) * stepX) - 0.5) * 65536.0);
        for (int xx = xLo; xx < xHi; xx++, fx += dfx16) {
            *out++ = SampleBilin((const DWORD*)r0, (const DWORD*)r1, W, fx, fxMax, wy);
        }
    }
}

DWORD WINAPI MacGenieBlackHoleThread(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    MacTelBeginThread(data, L"blackhole", nullptr);

    const int W = data->width;
    const int H = data->height;

    const float bounceStr = (float)data->bounceStr;
    const double bounceDurSet = (bounceStr > 0.01f) ? (double)MacMax(0, data->bounceDurMs) : 0.0;
    int padBounce = 0;
    if (bounceDurSet > 1.0 && (data->isRising || data->reversible)) {
        float maxOvershoot = (bounceStr / 100.0f) * 0.18f;
        padBounce = (int)(MacMax(W, H) * maxOvershoot + 1);
    }

    // Canvas margin. The vortex rotates blocks around the window centre, so a
    // block that starts in a far corner swings well outside the window rect
    // mid-flight (on a 16:9 window, hundreds of pixels above and below). V3
    // sized the canvas to the window rect and clipped them. Pad out to the
    // full rotation circle, then clip to the monitors involved.
    float maxRPad = sqrtf((W / 2.0f) * (W / 2.0f) + (H / 2.0f) * (H / 2.0f));
    int padX = (int)ceilf(maxRPad - W / 2.0f) + 16 + padBounce;
    int padY = (int)ceilf(maxRPad - H / 2.0f) + 16 + padBounce;
    MacShadow shadow;                       // V4
    if (data->shadow) {
        MacShadowInit(&shadow, GetMonitorDpiScale(data->hMon));
        padX = MacMax(padX, shadow.reachSide + padBounce + 2);
        padY = MacMax(padY, MacMax(shadow.reachTop, shadow.reachBottom) + padBounce + 2);
    }
    int boundLeft   = data->targetRect.left - padX;
    int boundTop    = data->targetRect.top  - padY;
    int boundRight  = data->targetRect.right + padX;
    int boundBottom = data->targetRect.bottom + padY;
    {
        RECT monUnion = GetGenieMonitorUnion(data->targetRect, data->hMon);
        if (boundLeft   < monUnion.left)   boundLeft   = monUnion.left;
        if (boundTop    < monUnion.top)    boundTop    = monUnion.top;
        if (boundRight  > monUnion.right)  boundRight  = monUnion.right;
        if (boundBottom > monUnion.bottom) boundBottom = monUnion.bottom;
        if (boundLeft   > data->targetRect.left)   boundLeft   = data->targetRect.left;
        if (boundTop    > data->targetRect.top)    boundTop    = data->targetRect.top;
        if (boundRight  < data->targetRect.right)  boundRight  = data->targetRect.right;
        if (boundBottom < data->targetRect.bottom) boundBottom = data->targetRect.bottom;
    }
    if (!MacGenieClipCanvasToDesktop(boundLeft, boundTop, boundRight, boundBottom)) {
        MacGenieAbortEngine(data);
        return 0;
    }
    int boundW = boundRight - boundLeft;
    int boundH = boundBottom - boundTop;
    if (boundW < 1) boundW = 1;
    if (boundH < 1) boundH = 1;

    HWND hGhost = CreateWindowExW(
        MacGenieGhostExStyle(data),
        L"STATIC", NULL, WS_POPUP,
        boundLeft, boundTop, boundW, boundH,
        NULL, NULL, NULL, NULL);

    HDC hScreenDC = GetDC(NULL);
    // The snapshot DIB is read in place: V3.6 BitBlt'ed it into a second W x H
    // DIB first (a full-size allocation + copy before the first frame) although
    // nothing ever writes to the snapshot.
    HDC hSrcDC = hScreenDC ? CreateCompatibleDC(hScreenDC) : NULL;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = boundW;
    bmi.bmiHeader.biHeight = -boundH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    BYTE* pBits = NULL;
    HBITMAP hCanvas = hSrcDC ? CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0) : NULL;
    HDC hCanvasDC = hCanvas ? CreateCompatibleDC(hScreenDC) : NULL;

    if (!hGhost || !data->pBits || !pBits || !hCanvasDC) {
        // Allocation failure (V3 memset the NULL canvas). Unwind everything.
        if (hCanvasDC) DeleteDC(hCanvasDC);
        if (hCanvas) DeleteObject(hCanvas);
        if (hSrcDC) DeleteDC(hSrcDC);
        if (hScreenDC) ReleaseDC(NULL, hScreenDC);
        if (hGhost) DestroyWindow(hGhost);
        MacGenieAbortEngine(data);
        return 0;
    }

    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);
    HBITMAP hOldCanvas = (HBITMAP)SelectObject(hCanvasDC, hCanvas);
    const int canvasStride = boundW * 4;

    float cx = (float)(data->targetRect.left + W / 2) - (float)boundLeft;
    float cy = (float)(data->targetRect.top  + H / 2) - (float)boundTop;
    float twistForce = (float)data->bhTwist / 2.0f;
    const uint32_t* srcPx = (const uint32_t*)data->pBits;
    uint32_t* dstPx = (uint32_t*)pBits;
    float maxR = sqrtf((W / 2.0f) * (W / 2.0f) + (H / 2.0f) * (H / 2.0f));
    if (maxR < 1.0f) maxR = 1.0f;
    float invMaxR = 1.0f / maxR;

    int bSize = (data->bhQuality == 1) ? 12 : (data->bhQuality == 3 ? 3 : 6);
    int pixels = W * H;
    if      (pixels > 6000000) bSize = MacMax(bSize, bSize * 2);
    else if (pixels > 2500000) bSize = MacMax(bSize, (bSize * 3) / 2);
    else if (pixels > 1500000) bSize = MacMax(bSize, bSize + 2);

    // Every per-animation buffer is allocated up front, so running out of memory
    // ends the animation here instead of throwing inside the host application
    // (V3.9). binList never grows past this reserve: a block is at most 24 px, so
    // it touches at most two 64-row bands.
    const int nBands = (boundH + BH_BAND_ROWS - 1) / BH_BAND_ROWS;
    std::vector<BHBlock> blocks;
    std::vector<int> dxs, dys, binStart, binCount, binList;
    float wcx = W / 2.0f, wcy = H / 2.0f;
    try {
        blocks.reserve((size_t)(W / bSize + 1) * (H / bSize + 1));
        for (int y = 0; y < H; y += bSize) {
            float dy = (y + bSize / 2.0f) - wcy;
            for (int x = 0; x < W; x += bSize) {
                float dx = (x + bSize / 2.0f) - wcx;
                BHBlock b;
                b.r0 = sqrtf(dx * dx + dy * dy);
                b.a0 = atan2f(dy, dx);
                b.x = x; b.y = y;
                blocks.push_back(b);
            }
        }
        dxs.resize(blocks.size());
        dys.resize(blocks.size());
        binStart.resize((size_t)nBands + 1);
        binCount.resize((size_t)nBands + 1);
        binList.reserve(blocks.size() * 2);
    } catch (const std::bad_alloc&) {
        SelectObject(hCanvasDC, hOldCanvas);
        SelectObject(hSrcDC, hOldSrc);
        DeleteObject(hCanvas);
        DeleteDC(hCanvasDC);
        DeleteDC(hSrcDC);
        ReleaseDC(NULL, hScreenDC);
        DestroyWindow(hGhost);
        MacGenieAbortEngine(data);
        return 0;
    }
    const int nBlocks = (int)blocks.size();

    // Worker pool, only worth its hand-off for a few thousand blocks.
    int budgetSlots = 0;
    HANDLE hBudget = NULL;
    ParallelPool pool;
    if (nBlocks >= 4000) {
        hBudget = MacGenieAcquireBudgetSlot(&budgetSlots);
        if (budgetSlots > 0) PoolInit(&pool, budgetSlots);
    }

    BHPassCtx pass = {};
    pass.blocks = blocks.data();
    pass.dxs = dxs.data();
    pass.dys = dys.data();
    pass.cx = cx; pass.cy = cy;
    pass.twistForce = twistForce;
    pass.invMaxR = invMaxR;
    pass.bSize = bSize;
    pass.boundW = boundW; pass.boundH = boundH;
    pass.W = W; pass.H = H;
    pass.srcPx = srcPx;
    pass.dstPx = dstPx;
    pass.binStart = binStart.data();

    const double animDur = (double)data->durationMs;

    MacGenieFramePacer framePacer;
    MacGenieCreateFramePacer(&framePacer, data->hMon);
    if (data->telOn) data->tel.intervalMs = framePacer.intervalMs;
    MacAnimClock clock;
    MacAnimClockInit(&clock);
    LARGE_INTEGER qpcNow;

    int prevMinY = 0, prevMaxY = boundH - 1;
    BOOL firstFrame = TRUE;
    MacTelSetupDone(data);
    for (;;) {
        MacGenieDrainMessages();
        QueryPerformanceCounter(&qpcNow);
        const LARGE_INTEGER qpcFrameStart = qpcNow;
        if (!firstFrame) MacAnimApplyReversal(&clock, data, animDur, qpcNow.QuadPart);
        const double bounceDur = data->isRising ? bounceDurSet : 0.0;
        const double totalDur = animDur + bounceDur;
        double elapsedMs = MacAnimClockMs(&clock, qpcNow.QuadPart);
        BOOL lastFrame = (elapsedMs >= totalDur);

        // Dirty-band zeroing
        if (firstFrame) {
            memset(pBits, 0, (size_t)boundW * 4 * boundH);
        } else {
            int curClearTop = MacMax(0, prevMinY);
            int curClearBottom = MacMin(boundH - 1, prevMaxY);
            if (curClearTop <= curClearBottom) {
                memset(pBits + (size_t)curClearTop * canvasStride, 0,
                       (size_t)(curClearBottom - curClearTop + 1) * canvasStride);
            }
        }

        int curMinY = boundH, curMaxY = 0;

        if (elapsedMs <= animDur || !data->isRising) {
            float p = MacClampf((float)(elapsedMs / animDur), 0.0f, 1.0f);
            float t = data->isRising ? (1.0f - p) : p;
            float pull = powf(t, 2.5f);
            float blockAlpha = 1.0f;
            if (t > 0.7f) blockAlpha = 1.0f - (t - 0.7f) / 0.3f;

            float baseAlpha = 1.0f - MacClampf((t - 0.30f) / 0.25f, 0.0f, 1.0f);
            baseAlpha = baseAlpha * baseAlpha;

            const int baseDstX = (int)(cx - wcx);
            const int baseDstY = (int)(cy - wcy);
            if (baseAlpha > 0.02f) {
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)(baseAlpha * 255.0f), AC_SRC_ALPHA };
                AlphaBlend(hCanvasDC, baseDstX, baseDstY, W, H,
                           hSrcDC, 0, 0, W, H, bf);
                GdiFlush();
                curMinY = MacMin(curMinY, baseDstY);
                curMaxY = MacMax(curMaxY, baseDstY + H);
            }

            float particleVisibility = MacClampf((t - 0.30f) / 0.25f, 0.0f, 1.0f);
            float particleAlpha = blockAlpha * particleVisibility;

            if (pull > 0.05f && particleAlpha > 0.02f) {
                BYTE pa255 = (BYTE)(particleAlpha * 255.0f);
                pass.pull = pull;
                pass.particleAlpha = particleAlpha;
                pass.fastPath = (pa255 == 255);

                // A. destinations (parallel by block index)
                PoolRun(&pool, BHDestJob, &pass, 0, nBlocks, 2048, 8192);

                // Bin the visible blocks by destination band (also yields the
                // rows this frame touches).
                BHBinBlocks(pass, nBlocks, nBands, binStart, binCount, binList,
                            &curMinY, &curMaxY);
                pass.binList = binList.data();

                // B. blend (parallel by destination band)
                PoolRun(&pool, BHBlendJob, &pass, 0, nBands, 1, 3);
            }
            GdiFlush();
            // V4: the window's shadow fades with the unwarped window.
            if (shadow.on && baseAlpha > 0.02f) {
                MacShadowRect(dstPx, boundW, boundH, (float)baseDstX, (float)baseDstY, (float)(baseDstX + W),
                              (float)(baseDstY + H), baseAlpha, shadow, 0, boundH);
                curMinY = MacMin(curMinY, baseDstY - shadow.reachTop);
                curMaxY = MacMax(curMaxY, baseDstY + H + shadow.reachBottom);
            }
        } else {
            float tB = MacClampf((float)((elapsedMs - animDur) / bounceDur), 0.0f, 1.0f);
            float scale = MacSpringBounce(tB, bounceStr);
            int curW = MacMax(2, (int)(W * scale));
            int curH = MacMax(2, (int)(H * scale));
            int bDstX = (int)(cx - curW / 2.0f);
            int bDstY = (int)(cy - curH / 2.0f);
            GdiFlush();
            BHScaleBilinear(dstPx, boundW, boundH, bDstX, bDstY, curW, curH, srcPx, W, H);
            curMinY = MacMin(curMinY, bDstY);
            curMaxY = MacMax(curMaxY, bDstY + curH);
            if (shadow.on) {
                MacShadowRect(dstPx, boundW, boundH, (float)bDstX, (float)bDstY, (float)(bDstX + curW),
                              (float)(bDstY + curH), 1.0f, shadow, 0, boundH);
                curMinY = MacMin(curMinY, bDstY - shadow.reachTop);
                curMaxY = MacMax(curMaxY, bDstY + curH + shadow.reachBottom);
            }
        }
        const LONGLONG telRenderEnd = MacTelMark(data);

        // Rows touched this frame or cleared from the last one.
        const int dirtyTop = MacMax(0, MacMin(prevMinY, curMinY));
        const int dirtyBottom = MacMin(boundH, MacMax(prevMaxY + 1, curMaxY));
        prevMinY = MacMax(0, curMinY);
        prevMaxY = MacMin(boundH - 1, curMaxY);

        POINT ptDst = { boundLeft, boundTop };
        SIZE  sz    = { boundW, boundH };
        RECT dirty = { 0, dirtyTop, boundW, MacMax(dirtyBottom, dirtyTop + 1) };
        MacGeniePresent(hGhost, hScreenDC, ptDst, sz, hCanvasDC, 255, firstFrame ? nullptr : &dirty);

        if (firstFrame) {
            MacGenieShowGhostForData(hGhost, data, false);
            MacTelFrame0Shown(data);
        }
        MacTelFrame(data, qpcFrameStart.QuadPart, telRenderEnd, MacTelMark(data));

        if (lastFrame) {
            DwmFlush();
            break;
        }
        if (g_unloading.load(std::memory_order_relaxed)) break;
        if (MacGenieSpecialCancelled(data)) break;

        // Same as the other engines: the first-frame flush is a full vblank wait,
        // so it stands in for this frame's pacing.
        bool paced = false;
        if (firstFrame) {
            DwmFlush();
            paced = true;
            firstFrame = FALSE;
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
            MacTelFirstFrame(data);
            MacGeniePacerRebase(&framePacer);
            MacAnimClockStartNow(&clock);   // frame 0 is on screen: time starts now
            if (data->isClosing && !data->special && !g_unloading.load(std::memory_order_relaxed) &&
                IsWindow(data->hRealWnd) && IsWindowVisible(data->hRealWnd)) {
                MacGenieSetCloak(data->hRealWnd, TRUE);
                data->hiddenByCloak = TRUE;
            }
        }

        // V3.6 also faded the REAL window out here on a non-held close. It was
        // already cloaked by the first frame (above), so the fade changed nothing
        // visible - but it added WS_EX_LAYERED and an alpha of 0 to a window of
        // another thread / process from inside the render loop, and when that
        // close was then refused ("Save changes?" -> Cancel) the survivor
        // watchdog uncloaked the window and left it fully transparent. Removed.

        if (!paced) MacGeniePaceFrame(&framePacer);
    }
    if (data->telOn) data->tel.strideMax = framePacer.strideMax;

    PoolShutdown(&pool);
    MacGenieReleaseBudgetSlot(hBudget, budgetSlots);

    MacGenieDestroyFramePacer(&framePacer);
    SelectObject(hCanvasDC, hOldCanvas);
    SelectObject(hSrcDC, hOldSrc);
    DeleteObject(hCanvas);
    MacGenieReleaseSnapshot(data);
    DeleteDC(hCanvasDC);
    DeleteDC(hSrcDC);
    ReleaseDC(NULL, hScreenDC);
    MacGenieRevealBeforeGhostGone(data);
    DestroyWindow(hGhost);

    MacGenieFinishAnimation(data);
    return 0;
}

// -------------------------------------------------------------------------
// V3.8 Effect Engine: the in-place effects
//
// Square Shatter, Thanos Snap, Perlin Dissolve, Cyber Glitch, Retro TV Off, Ink
// Splash, Mirage and Swell follow the math of ReDrag's Windows Animations 1.3.7
// (MIT); Pop, Fade Up and Poof are new, tuned for small windows.
//
// Every effect is a pure function of s, where s = 0 is the whole window and
// s = 1 is gone. A minimize or close plays s = 0 -> 1, a restore or launch
// s = 1 -> 0. Two rules hold for all of them:
//  - s = 0 reproduces the snapshot byte for byte, so the first frame of a
//    minimize and the reveal at the end of a restore never pop (ReDrag's Cyber
//    Glitch already tore rows at progress 0; here every displacement fades in);
//  - nothing carries over from one frame to the next, so reverse playback is
//    exact (ReDrag's particle lists were pruned for forward playback only).
// Improvements over the original: distances scale with the monitor DPI and
// with small windows, scaling is bilinear (it was nearest-neighbour), Mirage
// and Ink Splash are per pixel instead of 4-8 px blocks, particles composite
// "over" instead of overwriting each other, and the heavy passes run on the
// shared worker pool with a deterministic (band-ordered) result.
// -------------------------------------------------------------------------
#define FX_BAND_ROWS 64
#define FX_MAX_PARTICLES 400000

// V4 Glass: one shard's transform for the current frame.
struct FxGlassFrame {
    float f, ox, oy, cs, sn, k, alpha;
    int x0, y0, x1, y1;          // destination box (canvas)
    int n;                       // destination polygon: inward unit normals and offsets
    float nx[14], ny[14], off[14];
};

struct FxState {
    MacGenieAnimData* data = nullptr;
    int fx = FX_SWELL;
    const uint32_t* src = nullptr;      // premultiplied snapshot, stride W
    int W = 0, H = 0;
    uint32_t* dst = nullptr;            // premultiplied canvas, stride bw
    int bw = 0, bh = 0;
    int ox = 0, oy = 0;                 // snapshot top-left in canvas pixels
    float dpi = 1.0f;
    ParallelPool* pool = nullptr;
    // particle grid
    int block = 5, cols = 0, rows = 0, nParticles = 0;
    std::vector<float> pa, pb, pc, pd, pStart;
    std::vector<int> destX, destY;
    std::vector<uint8_t> destA;
    std::vector<int> binStart, binCount, binList;
    int nBands = 0;
    // Ink Splash distance field
    std::vector<float> field;
    int fw = 0, fh = 0;
    float fstep = 8.0f, fmin = 0.0f, fmax = 1.0f;
    // Mirage displacement
    std::vector<float> colBase, rowBase;
    std::vector<int> colU;
    // taskbar aim (dock relative to the window centre)
    bool aim = false;
    float aimX = 0.0f, aimY = 0.0f;
    float travelScale = 1.0f;
    // per-frame job parameters
    float s = 0.0f;
    float fa = 0.0f, fb = 0.0f, fc = 0.0f;
    int ia = 0, ib = 0, ic = 0, id = 0;
    int rx0 = 0, rx1 = 0;
    volatile LONG bbMinX = 0, bbMinY = 0, bbMaxX = 0, bbMaxY = 0;
    // The last frame drew particles: the next one erases exactly those blocks
    // (destA / destX / destY) instead of their whole, mostly empty, bounding box.
    bool particlesDrawn = false;
    // V4
    float xf[5] = {};                    // last transform: canvas x0, y0, x1, y1, alpha
    bool xfValid = false;
    bool toDock = false;                 // Scale / Fluid fly into the dock (aimX / aimY)
    std::vector<MipLevel> mips;          // Scale / Fluid: the snapshot's pyramid
    std::shared_ptr<void> extra;         // Glass: BSP, shards, owner map
    std::vector<FxGlassFrame> glassFrame;
    const void* extraFrame = nullptr;    // per-frame parameters for the jobs
};

static inline float FxClamp01(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
static inline float FxSmooth(float e0, float e1, float x) {
    const float t = FxClamp01((x - e0) / (e1 - e0));
    return t * t * (3.0f - 2.0f * t);
}
static inline uint32_t FxHash(uint32_t x, uint32_t y) { return (x * 73856093u) ^ (y * 19349663u); }
static inline float FxHashF(int x, int y) {
    uint32_t h = ((uint32_t)x * 374761393u) ^ ((uint32_t)y * 668265263u);
    h = (h ^ (h >> 13)) * 1274126177u;
    return (float)(h & 0xFFFFu) / 65535.0f;
}

// c * a / 255 for a premultiplied pixel, exactly rounded, two channels at a time.
static inline uint32_t FxMul(uint32_t c, uint32_t a255) {
    if (a255 >= 255) return c;
    if (!a255) return 0;
    uint32_t rb = (c & 0x00FF00FFu) * a255 + 0x00800080u;
    rb = ((rb + ((rb >> 8) & 0x00FF00FFu)) >> 8) & 0x00FF00FFu;
    uint32_t ag = ((c >> 8) & 0x00FF00FFu) * a255 + 0x00800080u;
    ag = ((ag + ((ag >> 8) & 0x00FF00FFu)) >> 8) & 0x00FF00FFu;
    return rb | (ag << 8);
}

// Premultiplied s over d. Channels saturate: a capture is not always strictly
// premultiplied, and an overflow would bleed into the next channel.
static inline uint32_t FxOver(uint32_t d, uint32_t s) {
    const uint32_t sa = s >> 24;
    if (sa >= 255 || !d) return s;
    if (!s) return d;
    const uint32_t r = FxMul(d, 255 - sa);
    // Two channels per 32-bit lane pair; an overflowing lane sets its bit 8,
    // which is turned into 0xFF (saturation) before masking.
    uint32_t rb = (r & 0x00FF00FFu) + (s & 0x00FF00FFu);
    uint32_t ag = ((r >> 8) & 0x00FF00FFu) + ((s >> 8) & 0x00FF00FFu);
    uint32_t sat = rb & 0x01000100u;
    rb = (rb | (sat - (sat >> 8))) & 0x00FF00FFu;
    sat = ag & 0x01000100u;
    ag = (ag | (sat - (sat >> 8))) & 0x00FF00FFu;
    return rb | (ag << 8);
}

static inline uint32_t FxPx(const uint32_t* src, int W, int H, int x, int y) {
    return ((unsigned)x < (unsigned)W && (unsigned)y < (unsigned)H) ? src[(size_t)y * W + x] : 0u;
}

// Bilinear sample at (u, v) in source pixels (pixel centres at +0.5). Outside
// the image the samples are transparent, which antialiases the silhouette of a
// scaled window. At integer-aligned positions it returns the pixel exactly.
static inline uint32_t FxSampleBilinear(const uint32_t* src, int W, int H, float u, float v) {
    const float fx = u - 0.5f, fy = v - 0.5f;
    const float flx = floorf(fx), fly = floorf(fy);
    const int x0 = (int)flx, y0 = (int)fly;
    const int wx = (int)((fx - flx) * 256.0f);
    const int wy = (int)((fy - fly) * 256.0f);
    uint32_t top = FxPx(src, W, H, x0, y0);
    if (wx) top = Lerp8(top, FxPx(src, W, H, x0 + 1, y0), wx);
    if (!wy) return top;
    uint32_t bot = FxPx(src, W, H, x0, y0 + 1);
    if (wx) bot = Lerp8(bot, FxPx(src, W, H, x0 + 1, y0 + 1), wx);
    return Lerp8(top, bot, wy);
}

static inline RECT FxEmpty() { return RECT{0, 0, 0, 0}; }
static inline bool FxIsEmpty(const RECT& r) { return r.right <= r.left || r.bottom <= r.top; }

static RECT FxClip(const FxState& st, int l, int t, int r, int b) {
    RECT o = { MacMax(0, l), MacMax(0, t), MacMin(st.bw, r), MacMin(st.bh, b) };
    if (FxIsEmpty(o)) o = FxEmpty();
    return o;
}

static RECT FxWindowRect(const FxState& st, int padX, int padY) {
    return FxClip(st, st.ox - padX, st.oy - padY, st.ox + st.W + padX, st.oy + st.H + padY);
}

static void FxClear(FxState& st, const RECT& r) {
    if (FxIsEmpty(r)) return;
    const size_t n = (size_t)(r.right - r.left) * 4;
    for (int y = r.top; y < r.bottom; y++) memset(st.dst + (size_t)y * st.bw + r.left, 0, n);
}

static void FxAtomicMin(volatile LONG* p, LONG v) {
    LONG cur = *p;
    while (v < cur) {
        const LONG prev = InterlockedCompareExchange(p, v, cur);
        if (prev == cur) break;
        cur = prev;
    }
}

static void FxAtomicMax(volatile LONG* p, LONG v) {
    LONG cur = *p;
    while (v > cur) {
        const LONG prev = InterlockedCompareExchange(p, v, cur);
        if (prev == cur) break;
        cur = prev;
    }
}

// --- copy / scale / translate (identity, Swell, Pop, Fade Up) -------------
static void FxJobCopy(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    const uint32_t a = (uint32_t)st->ia;
    int x0 = 0, x1 = st->W;
    if (st->ox < 0) x0 = -st->ox;
    if (st->ox + x1 > st->bw) x1 = st->bw - st->ox;
    if (x1 <= x0) return;
    for (int y = y0; y < y1; y++) {
        const int sy = y - st->oy;
        if (sy < 0 || sy >= st->H) continue;
        const uint32_t* s = st->src + (size_t)sy * st->W;
        uint32_t* d = st->dst + (size_t)y * st->bw + st->ox;
        if (a >= 255) {
            memcpy(d + x0, s + x0, (size_t)(x1 - x0) * 4);
        } else {
            for (int x = x0; x < x1; x++) d[x] = FxMul(s[x], a);
        }
    }
}

// Interior pixels step the source position in 16.16 fixed point (it is linear
// along a row) and sample without bounds checks; only the 1-2 px silhouette
// takes the float path with its transparent outside.
//
// V3.9: all positions are taken relative to the WINDOW (fb / fc = the scale
// centre in window pixels, the fixed-point walk anchored at window column 0), not
// to the canvas. 3.8 anchored the walk at the canvas' first drawn column, so the
// same frame came out a rounding step different depending on where the canvas
// started - which the desktop clip now varies.
static void FxJobTransform(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    const float inv = st->fa, cxw = st->fb, cyw = st->fc;
    const int W = st->W, H = st->H, ox = st->ox, oy = st->oy;
    const float hw = W * 0.5f, hh = H * 0.5f;
    const float maxU = (float)W + 0.5f, maxV = (float)H + 0.5f;
    const uint32_t a = (uint32_t)st->ia;
    const double uw0 = (0.5 - (double)cxw) * (double)inv + (double)hw - 0.5;   // u at window column 0
    const int64_t step16 = (int64_t)llround((double)inv * 65536.0);
    const int64_t start16 = (int64_t)llround(uw0 * 65536.0) + (int64_t)(st->rx0 - ox) * step16;
    const int64_t interiorEnd = (int64_t)(W - 1) << 16;
    for (int y = y0; y < y1; y++) {
        const float v = ((float)(y - oy) + 0.5f - cyw) * inv + hh;
        if (v <= -0.5f || v >= maxV) continue;
        uint32_t* d = st->dst + (size_t)y * st->bw;
        const float vy = v - 0.5f;
        const float fly = floorf(vy);
        const int sy = (int)fly;
        const int wy = (int)((vy - fly) * 256.0f);
        const bool rowInterior = sy >= 0 && (sy < H - 1 || (sy == H - 1 && wy == 0));
        const uint32_t* r0 = rowInterior ? st->src + (size_t)sy * W : nullptr;
        const uint32_t* r1 = (rowInterior && sy + 1 < H) ? r0 + W : r0;
        int64_t u16 = start16;
        // V4: the run of interior pixels goes through the SSE2 sampler (the same
        // arithmetic as the scalar path below, bit for bit), then the alpha.
        int runA = st->rx1, runB = st->rx1;
        if (!g_macNoSimd && rowInterior && step16 > 0 && interiorEnd > 0 && interiorEnd < ((int64_t)1 << 31)) {
            // first x with u16 >= 0, first x with u16 >= interiorEnd
            const int64_t nA = start16 >= 0 ? 0 : (-start16 + step16 - 1) / step16;
            const int64_t nB = start16 >= interiorEnd ? 0 : (interiorEnd - start16 + step16 - 1) / step16;
            runA = (int)MacMin((int64_t)st->rx1, (int64_t)st->rx0 + nA);
            runB = (int)MacMin((int64_t)st->rx1, (int64_t)st->rx0 + nB);
            if (runB > runA) {
                const int64_t ua = start16 + (int64_t)(runA - st->rx0) * step16;
                SampleBilinRunSse2((DWORD*)(d + runA), runB - runA, (const DWORD*)r0, (const DWORD*)r1, W,
                                   (int)ua, (int)step16, (W - 1) << 16, wy);
                if (a < 255) for (int x = runA; x < runB; x++) d[x] = FxMul(d[x], a);
            } else {
                runA = runB = st->rx1;
            }
        }
        for (int x = st->rx0; x < st->rx1; x++, u16 += step16) {
            if (x == runA && runB > runA) {
                u16 += (int64_t)(runB - 1 - x) * step16;
                x = runB - 1;
                continue;
            }
            uint32_t px;
            if (rowInterior && u16 >= 0 && u16 < interiorEnd) {
                const int sx = (int)(u16 >> 16);
                const int wx = (int)((u16 >> 8) & 0xFF);
                px = wx ? Lerp8(r0[sx], r0[sx + 1], wx) : r0[sx];
                if (wy) {
                    const uint32_t bot = wx ? Lerp8(r1[sx], r1[sx + 1], wx) : r1[sx];
                    px = Lerp8(px, bot, wy);
                }
            } else {
                const float u = ((float)(x - ox) + 0.5f - cxw) * inv + hw;
                if (u <= -0.5f || u >= maxU) continue;
                px = FxSampleBilinear(st->src, W, H, u, v);
            }
            if (a < 255) px = FxMul(px, a);
            d[x] = px;
        }
    }
}

// V4: the rectangle a transform drew the window into (for its shadow).
static void FxNoteTransform(FxState& st, float scale, float offX, float offY, float alpha) {
    const float cx = (float)st.ox + st.W * 0.5f + offX, cy = (float)st.oy + st.H * 0.5f + offY;
    st.xf[0] = cx - st.W * 0.5f * scale;
    st.xf[1] = cy - st.H * 0.5f * scale;
    st.xf[2] = cx + st.W * 0.5f * scale;
    st.xf[3] = cy + st.H * 0.5f * scale;
    st.xf[4] = FxClamp01(alpha);
    st.xfValid = true;
}

static RECT FxRenderTransform(FxState& st, float scale, float offX, float offY, float alpha) {
    FxNoteTransform(st, scale, offX, offY, alpha);
    const int a = (int)(FxClamp01(alpha) * 255.0f + 0.5f);
    if (a <= 0 || scale < 0.01f) return FxEmpty();
    st.ia = a;
    if (scale == 1.0f && offX == 0.0f && offY == 0.0f) {
        const RECT r = FxWindowRect(st, 0, 0);
        if (!FxIsEmpty(r)) PoolRun(st.pool, FxJobCopy, &st, r.top, r.bottom, 32, 256);
        return r;
    }
    const float cxw = st.W * 0.5f + offX;          // scale centre, window pixels
    const float cyw = st.H * 0.5f + offY;
    const float cx = (float)st.ox + cxw;            // ... and canvas pixels (the box)
    const float cy = (float)st.oy + cyw;
    const float hw = st.W * 0.5f * scale, hh = st.H * 0.5f * scale;
    const RECT r = FxClip(st, (int)floorf(cx - hw) - 1, (int)floorf(cy - hh) - 1,
                          (int)ceilf(cx + hw) + 2, (int)ceilf(cy + hh) + 2);
    if (FxIsEmpty(r)) return r;
    st.fa = 1.0f / scale;
    st.fb = cxw;
    st.fc = cyw;
    st.rx0 = r.left;
    st.rx1 = r.right;
    PoolRun(st.pool, FxJobTransform, &st, r.top, r.bottom, 16, 128);
    return r;
}

static RECT FxRenderIdentity(FxState& st) { return FxRenderTransform(st, 1.0f, 0.0f, 0.0f, 1.0f); }

static RECT FxRenderSwell(FxState& st, float s) {
    const float k = 1.0f - s;
    const float e = 1.0f - k * k * k;
    return FxRenderTransform(st, 1.0f + 0.18f * e, 0.0f, 0.0f, 1.0f - e);
}

static RECT FxRenderPop(FxState& st, float s) {
    const float k = 1.0f - s;
    return FxRenderTransform(st, 1.0f - 0.14f * s * s, 0.0f, 0.0f, k * k);
}

static RECT FxRenderFadeUp(FxState& st, float s) {
    const float k = 1.0f - s;
    const float e = 1.0f - k * k;
    return FxRenderTransform(st, 1.0f, 0.0f, -8.0f * st.dpi * e, 1.0f - FxSmooth(0.0f, 1.0f, s));
}

// --- Retro TV Off ---------------------------------------------------------
// ia = dstY (canvas), ib = dstH, fa = content alpha, fb = brightness
static void FxJobSquash(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    const int dstY = st->ia, dstH = st->ib, W = st->W, H = st->H;
    const float alpha = st->fa, bright = st->fb;
    const float lift = bright > 1.0f ? (bright - 1.0f) * 0.65f : 0.0f;
    int x0 = 0, x1 = W;
    if (st->ox < 0) x0 = -st->ox;
    if (st->ox + x1 > st->bw) x1 = st->bw - st->ox;
    for (int dy = y0; dy < y1; dy++) {
        const int outY = dstY + dy;
        if (outY < 0 || outY >= st->bh) continue;
        const float srcYF = (((float)dy + 0.5f) * (float)H) / (float)dstH - 0.5f;
        int ya = (int)floorf(srcYF);
        float fy = srcYF - (float)ya;
        int yb = ya + 1;
        if (ya < 0) { ya = 0; fy = 0.0f; }
        if (yb >= H) yb = H - 1;
        if (ya >= H) ya = H - 1;
        const uint32_t* r0 = st->src + (size_t)ya * W;
        const uint32_t* r1 = st->src + (size_t)yb * W;
        uint32_t* d = st->dst + (size_t)outY * st->bw + st->ox;
        const float w0 = 1.0f - fy, w1 = fy;
        for (int x = x0; x < x1; x++) {
            const uint32_t p0 = r0[x], p1 = r1[x];
            float b = ((float)(p0 & 0xFF) * w0 + (float)(p1 & 0xFF) * w1) * bright;
            float g = ((float)((p0 >> 8) & 0xFF) * w0 + (float)((p1 >> 8) & 0xFF) * w1) * bright;
            float r = ((float)((p0 >> 16) & 0xFF) * w0 + (float)((p1 >> 16) & 0xFF) * w1) * bright;
            float aa = ((float)(p0 >> 24) * w0 + (float)(p1 >> 24) * w1) * alpha;
            if (lift > 0.0f) {
                b += (255.0f - b) * lift;
                g += (255.0f - g) * lift;
                r += (220.0f - r) * lift;
            }
            const uint32_t bi = b > 255.0f ? 255u : (uint32_t)b;
            const uint32_t gi = g > 255.0f ? 255u : (uint32_t)g;
            const uint32_t ri = r > 255.0f ? 255u : (uint32_t)r;
            const uint32_t ai = aa > 255.0f ? 255u : (uint32_t)aa;
            d[x] = (ai << 24) | (ri << 16) | (gi << 8) | bi;
        }
    }
}

static void FxDrawBeam(FxState& st, float cx, float cy, float halfLen, float intensity) {
    if (intensity <= 0.02f || halfLen < 0.25f) return;
    if (intensity > 1.0f) intensity = 1.0f;
    const float k = st.dpi;
    const float tip = 1.75f * k;
    const int x0 = MacMax(0, (int)floorf(cx - halfLen - tip));
    const int x1 = MacMin(st.bw - 1, (int)ceilf(cx + halfLen + tip));
    const int y0 = MacMax(0, (int)floorf(cy - 2.0f * k));
    const int y1 = MacMin(st.bh - 1, (int)ceilf(cy + 2.0f * k));
    for (int y = y0; y <= y1; ++y) {
        const float ady = fabsf((float)y + 0.5f - cy) / k;
        float gy = 0.0f;
        if (ady < 0.55f) gy = 1.0f;
        else if (ady < 1.55f) gy = 1.0f - (ady - 0.55f);
        if (gy <= 0.0f) continue;
        uint32_t* row = st.dst + (size_t)y * st.bw;
        for (int x = x0; x <= x1; ++x) {
            const float adx = fabsf((float)x + 0.5f - cx);
            float gx = 1.0f;
            if (adx > halfLen) {
                const float t = (adx - halfLen) / tip;
                if (t >= 1.0f) continue;
                gx = 1.0f - t;
            }
            const float a = gy * gx * intensity;
            if (a < 0.02f) continue;
            const uint32_t b = (uint32_t)fminf(255.0f, (210.0f + 45.0f * gy) * a);
            const uint32_t g = (uint32_t)fminf(255.0f, (235.0f + 20.0f * gy) * a);
            const uint32_t r = (uint32_t)fminf(255.0f, 255.0f * a);
            const uint32_t aa = (uint32_t)fminf(255.0f, 255.0f * a);
            row[x] = (aa << 24) | (r << 16) | (g << 8) | b;
        }
    }
}

static void FxDrawDot(FxState& st, float cx, float cy, float intensity) {
    if (intensity <= 0.02f) return;
    if (intensity > 1.0f) intensity = 1.0f;
    const float k = st.dpi;
    const int x0 = MacMax(0, (int)floorf(cx - 1.5f * k)), x1 = MacMin(st.bw - 1, (int)ceilf(cx + 1.5f * k));
    const int y0 = MacMax(0, (int)floorf(cy - 1.5f * k)), y1 = MacMin(st.bh - 1, (int)ceilf(cy + 1.5f * k));
    for (int y = y0; y <= y1; ++y) {
        uint32_t* row = st.dst + (size_t)y * st.bw;
        for (int x = x0; x <= x1; ++x) {
            const float dx = ((float)x + 0.5f - cx) / k, dy = ((float)y + 0.5f - cy) / k;
            const float d2 = dx * dx + dy * dy;
            float w = 0.0f;
            if (d2 <= 0.35f) w = 1.0f;
            else if (d2 <= 1.6f) w = 1.0f - (d2 - 0.35f) / 1.25f;
            if (w <= 0.0f) continue;
            const uint32_t v = (uint32_t)(255.0f * w * intensity);
            row[x] = (v << 24) | (v << 16) | (v << 8) | v;
        }
    }
}

static RECT FxRenderTvOff(FxState& st, float s) {
    if (s <= 0.0f) return FxRenderIdentity(st);
    const float squashEnd = 0.42f, zipEnd = 0.78f;
    const float k = st.dpi;
    const float cx = (float)st.ox + st.W * 0.5f;
    const float cy = (float)st.oy + st.H * 0.5f;
    const int beamPad = (int)(3.0f * k) + 2;
    if (s < squashEnd) {
        const float u = s / squashEnd;
        const float punch = u * u * u;
        const float minH = 2.0f * k;
        const float dstHf = (float)st.H * (1.0f - punch) + minH * punch;
        const int dstH = MacMax(2, (int)(dstHf + 0.5f));
        // floorf, not (int): for a window starting above the canvas (clipped to
        // the desktop) the value is negative, and truncation moved the squash
        // down a row there (V3.9).
        const int dstY = (int)floorf(cy - (float)dstH * 0.5f);
        const float handoff = 10.0f * k;
        float contentAlpha = 1.0f, beamAlpha = 0.0f;
        if ((float)dstH < handoff) {
            contentAlpha = FxClamp01(((float)dstH - minH) / (handoff - minH));
            beamAlpha = 1.0f - contentAlpha;
        }
        if (contentAlpha > 0.02f) {
            st.ia = dstY;
            st.ib = dstH;
            st.fa = contentAlpha;
            st.fb = 1.0f + punch * 0.85f;
            PoolRun(st.pool, FxJobSquash, &st, 0, dstH, 16, 128);
        }
        if (beamAlpha > 0.02f) FxDrawBeam(st, cx, cy, st.W * 0.5f, beamAlpha);
        return FxClip(st, st.ox - beamPad, dstY - beamPad, st.ox + st.W + beamPad, dstY + dstH + beamPad);
    }
    if (s < zipEnd) {
        const float u = (s - squashEnd) / (zipEnd - squashEnd);
        const float ease = u * u;
        const float halfLen = st.W * 0.5f * (1.0f - ease) + 1.0f * ease;
        FxDrawBeam(st, cx, cy, halfLen, 1.0f);
        return FxClip(st, (int)floorf(cx - halfLen) - beamPad, (int)floorf(cy) - beamPad,
                      (int)ceilf(cx + halfLen) + beamPad + 1, (int)floorf(cy) + beamPad + 1);
    }
    const float u = (s - zipEnd) / (1.0f - zipEnd);
    const float decay = 1.0f - u;
    if (decay > 0.02f) FxDrawDot(st, cx, cy, decay * decay);
    const int icx = (int)floorf(cx), icy = (int)floorf(cy);
    return FxClip(st, icx - beamPad, icy - beamPad, icx + beamPad + 1, icy + beamPad + 1);
}

// --- Cyber Glitch ---------------------------------------------------------
// One pixel with its red channel taken from k px to the left and its blue from
// k px to the right, clamped to the pixel's own alpha (premultiplied).
static inline uint32_t FxGlitchSplit(const uint32_t* s, int x, int W, int k) {
    const uint32_t p = s[x];
    const uint32_t pr = (x - k >= 0) ? s[x - k] : 0u;
    const uint32_t pb = (x + k < W) ? s[x + k] : 0u;
    const uint32_t a = p >> 24;
    uint32_t r = (pr >> 16) & 0xFF, b = pb & 0xFF;
    if (r > a) r = a;
    if (b > a) b = a;
    return (p & 0xFF00FF00u) | (r << 16) | b;
}

// ia..id = hash seeds, fa = tear envelope, fb = alpha, fc = break-up cut
static void FxJobGlitch(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    const int W = st->W, H = st->H;
    const uint32_t a = (uint32_t)(st->fb * 255.0f + 0.5f);
    for (int y = y0; y < y1; y++) {
        const int dy = st->oy + y;
        if (dy < 0 || dy >= st->bh) continue;
        const int lineId = (int)((float)y / (float)H * 70.0f);
        const float bandMask = (FxHashF(lineId, st->ia) >= 0.55f) ? 1.0f : 0.0f;
        const float tear = (FxHashF((int)((float)lineId * 1.7f), st->ib) - 0.5f) * 0.10f * bandMask;
        const float micro = (FxHashF((int)((float)y / (float)H * 220.0f), st->ic) - 0.5f) * 0.025f;
        const int base = st->ox + (int)((tear + micro) * st->fa * (float)W);
        int x0 = 0, x1 = W;
        if (base < 0) x0 = -base;
        if (base + x1 > st->bw) x1 = st->bw - base;
        if (x1 <= x0) continue;
        const uint32_t* s = st->src + (size_t)y * W;
        uint32_t* d = st->dst + (size_t)dy * st->bw + base;
        // V4: torn rows split their colour channels (red left, blue right) by up
        // to 2 dp, growing with the tear envelope - 0 at s = 0, so frame 0 is
        // still the snapshot.
        const int split = (bandMask > 0.0f) ? (int)(2.5f * st->dpi * st->fa + 0.5f) : 0;
        if (split > 0) {
            for (int x = x0; x < x1; x++) {
                if (st->fc < 0.92f && FxHashF(x, y + st->id * 17) > st->fc) continue;
                const uint32_t p = FxGlitchSplit(s, x, W, split);
                d[x] = (a >= 255) ? p : FxMul(p, a);
            }
        } else if (st->fc >= 0.92f) {
            if (a >= 255) memcpy(d + x0, s + x0, (size_t)(x1 - x0) * 4);
            else for (int x = x0; x < x1; x++) d[x] = FxMul(s[x], a);
        } else {
            for (int x = x0; x < x1; x++) {
                if (FxHashF(x, y + st->id * 17) > st->fc) continue;
                d[x] = (a >= 255) ? s[x] : FxMul(s[x], a);
            }
        }
    }
}

static RECT FxRenderGlitch(FxState& st, float s) {
    if (s <= 0.0f) return FxRenderIdentity(st);
    const float p = s * s * (3.0f - 2.0f * s);
    float alphaBase = 1.0f - FxSmooth(0.0f, 1.0f, p);
    alphaBase *= 1.0f - FxSmooth(0.55f, 1.0f, p);
    if (alphaBase <= 0.01f) return FxEmpty();
    st.ia = (int)floorf(p * 18.0f);
    st.ib = (int)floorf(p * 32.0f);
    st.ic = (int)floorf(p * 64.0f);
    st.id = (int)floorf(p * 22.0f);
    st.fa = FxSmooth(0.0f, 0.15f, s);
    st.fb = alphaBase;
    st.fc = 1.0f - p * 0.92f;
    PoolRun(st.pool, FxJobGlitch, &st, 0, st.H, 16, 128);
    return FxWindowRect(st, (int)(st.W * 0.0625f) + 2, 0);
}

// --- Mirage ---------------------------------------------------------------
static void FxPrecalcMirage(FxState& st) {
    st.colBase.resize((size_t)st.W);
    st.rowBase.resize((size_t)st.H);
    st.colU.resize((size_t)st.W);
    for (int x = 0; x < st.W; x++) {
        st.colBase[x] = 0.035f * cosf(62.0f * ((float)x + 0.5f) / (float)st.W) * (float)st.W;
    }
    for (int y = 0; y < st.H; y++) {
        st.rowBase[y] = 0.035f * sinf(62.0f * ((float)y + 0.5f) / (float)st.H) * (float)st.H;
    }
}

// fa = displacement amount, ia = alpha; colU = this frame's source column of
// every destination column (16.16, clamped), since the column offset depends
// on x only and the row offset on y only.
static void FxJobMirage(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    const float inv = st->fa;
    const uint32_t a = (uint32_t)st->ia;
    const int W = st->W, H = st->H;
    const int fxMax = (W - 1) << 16;
    const int* colU = st->colU.data();
    int x0 = 0, x1 = W;
    if (st->ox < 0) x0 = -st->ox;
    if (st->ox + x1 > st->bw) x1 = st->bw - st->ox;
    for (int y = y0; y < y1; y++) {
        const int dy = st->oy + y;
        if (dy < 0 || dy >= st->bh) continue;
        float vy = (float)y + inv * st->rowBase[y];
        if (vy < 0.0f) vy = 0.0f; else if (vy > (float)(H - 1)) vy = (float)(H - 1);
        const int sy = (int)vy;
        const int wy = (int)((vy - (float)sy) * 256.0f);
        const DWORD* r0 = (const DWORD*)(st->src + (size_t)sy * W);
        const DWORD* r1 = (sy + 1 < H) ? r0 + W : r0;
        uint32_t* d = st->dst + (size_t)dy * st->bw + st->ox;
        for (int x = x0; x < x1; x++) {
            uint32_t px = (uint32_t)SampleBilin(r0, r1, W, colU[x], fxMax, wy);
            if (a < 255) px = FxMul(px, a);
            d[x] = px;
        }
    }
}

static RECT FxRenderMirage(FxState& st, float s) {
    const float q = FxSmooth(0.0f, 1.0f, 1.0f - s);
    const float inv = 1.0f - q;
    if (inv <= 0.0f) return FxRenderTransform(st, 1.0f, 0.0f, 0.0f, q);
    const int a = (int)(q * 255.0f + 0.5f);
    if (a <= 0) return FxEmpty();
    st.fa = inv;
    st.ia = a;
    const int fxMax = (st.W - 1) << 16;
    for (int x = 0; x < st.W; x++) {
        const int u16 = (int)lroundf(((float)x + inv * st.colBase[x]) * 65536.0f);
        st.colU[x] = u16 < 0 ? 0 : (u16 > fxMax ? fxMax : u16);
    }
    PoolRun(st.pool, FxJobMirage, &st, 0, st.H, 16, 128);
    return FxWindowRect(st, 0, 0);
}

// --- Ink Splash -----------------------------------------------------------
static void FxPrecalcInk(FxState& st) {
    auto fractf = [](float x) -> float { return x - floorf(x); };
    auto hash = [&](float px, float py) -> float { return fractf(sinf(px * 127.1f + py * 311.7f) * 43758.5453f); };
    auto noise = [&](float x, float y) -> float {
        const float ix = floorf(x), iy = floorf(y);
        float fx = x - ix, fy = y - iy;
        fx = fx * fx * (3.0f - 2.0f * fx);
        fy = fy * fy * (3.0f - 2.0f * fy);
        const float a = hash(ix, iy), b = hash(ix + 1.0f, iy);
        const float c = hash(ix, iy + 1.0f), d = hash(ix + 1.0f, iy + 1.0f);
        return a * (1.0f - fx) * (1.0f - fy) + b * fx * (1.0f - fy) + c * (1.0f - fx) * fy + d * fx * fy;
    };
    auto fbm = [&](float x, float y) -> float {
        float v = 0.0f, amp = 0.5f;
        for (int i = 0; i < 4; ++i) {
            v += amp * noise(x, y);
            x *= 2.1f;
            y *= 2.1f;
            amp *= 0.5f;
        }
        return v;
    };
    const float aspect = (float)st.W / (float)MacMax(1, st.H);
    st.fstep = (float)MacMax(8, MacMin(st.W, st.H) / 48);
    st.fw = (int)((float)st.W / st.fstep) + 2;
    st.fh = (int)((float)st.H / st.fstep) + 2;
    st.field.resize((size_t)st.fw * st.fh);
    st.fmin = 1e9f;
    st.fmax = -1e9f;
    for (int iy = 0; iy < st.fh; ++iy) {
        for (int ix = 0; ix < st.fw; ++ix) {
            const float uvx = (float)ix * st.fstep / (float)st.W;
            const float uvy = (float)iy * st.fstep / (float)st.H;
            const float blob = fbm(uvx * 3.5f, uvy * 3.5f);
            const float fingers = fbm(uvx * 14.0f, uvy * 14.0f);
            const float distortion = (blob - 0.5f) * 0.5f + (fingers - 0.5f) * 0.18f;
            const float cx = (uvx - 0.5f) * aspect, cy = uvy - 0.5f;
            const float f = sqrtf(cx * cx + cy * cy) + distortion;
            st.field[(size_t)iy * st.fw + ix] = f;
            if (f < st.fmin) st.fmin = f;
            if (f > st.fmax) st.fmax = f;
        }
    }
}

// fa = ink boundary
static void FxJobInk(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    const float invStep = 1.0f / st->fstep, boundary = st->fa;
    int x0 = 0, x1 = st->W;
    if (st->ox < 0) x0 = -st->ox;
    if (st->ox + x1 > st->bw) x1 = st->bw - st->ox;
    for (int y = y0; y < y1; y++) {
        const int dy = st->oy + y;
        if (dy < 0 || dy >= st->bh) continue;
        const float gy = ((float)y + 0.5f) * invStep;
        int iy = (int)gy;
        if (iy > st->fh - 2) iy = st->fh - 2;
        const float ty = FxClamp01(gy - (float)iy);
        const float* f0 = st->field.data() + (size_t)iy * st->fw;
        const float* f1 = f0 + st->fw;
        const uint32_t* s = st->src + (size_t)y * st->W;
        uint32_t* d = st->dst + (size_t)dy * st->bw + st->ox;
        // One field cell at a time: the interpolated field inside a cell stays
        // between its four corners, so a cell entirely inside the ink is copied
        // and one entirely outside is skipped; only the cells the boundary
        // crosses are evaluated per pixel.
        int x = x0;
        while (x < x1) {
            int ix = (int)(((float)x + 0.5f) * invStep);
            if (ix > st->fw - 2) ix = st->fw - 2;
            int xEnd = (ix >= st->fw - 2) ? x1 : (int)ceilf((float)(ix + 1) * st->fstep - 0.5f);
            if (xEnd > x1) xEnd = x1;
            if (xEnd <= x) xEnd = x + 1;
            const float a0 = f0[ix], a1 = f0[ix + 1], b0 = f1[ix], b1 = f1[ix + 1];
            const float lo = fminf(fminf(a0, a1), fminf(b0, b1));
            const float hi = fmaxf(fmaxf(a0, a1), fmaxf(b0, b1));
            if (boundary - hi >= 0.04f) {
                memcpy(d + x, s + x, (size_t)(xEnd - x) * 4);
            } else if (lo - boundary < 0.04f) {
                for (int px0 = x; px0 < xEnd; px0++) {
                    const float gx = ((float)px0 + 0.5f) * invStep;
                    const float tx = FxClamp01(gx - (float)ix);
                    const float top = a0 + (a1 - a0) * tx;
                    const float bot = b0 + (b1 - b0) * tx;
                    const float f = top + (bot - top) * ty;
                    float t = (boundary - f + 0.04f) * 12.5f;
                    if (t <= 0.0f) continue;
                    uint32_t px = s[px0];
                    if (t < 1.0f) {
                        t = t * t * (3.0f - 2.0f * t);
                        px = FxMul(px, (uint32_t)(t * 255.0f + 0.5f));
                    }
                    d[px0] = px;
                }
            }
            x = xEnd;
        }
    }
}

static RECT FxRenderInk(FxState& st, float s) {
    const float q = 1.0f - s;
    const float boundary = (st.fmin - 0.1f) + (st.fmax - st.fmin + 0.2f) * q;
    if (boundary - st.fmax >= 0.04f) return FxRenderIdentity(st);
    if (boundary + 0.04f <= st.fmin) return FxEmpty();
    st.fa = boundary;
    PoolRun(st.pool, FxJobInk, &st, 0, st.H, 16, 128);
    return FxWindowRect(st, 0, 0);
}

// --- Perlin Dissolve ------------------------------------------------------
static void FxPrecalcPerlin(FxState& st) {
    auto lattice = [](int ix, int iy) -> float {
        const uint32_t h = ((uint32_t)ix * 73856093u) ^ ((uint32_t)iy * 19349663u);
        return (float)(h % 10000u) / 10000.0f;
    };
    const float scale = 150.0f * st.dpi;
    st.pStart.assign((size_t)st.nParticles, 0.0f);
    size_t i = 0;
    for (int by = 0; by < st.rows; by++) {
        for (int bx = 0; bx < st.cols; bx++, i++) {
            float sum = 0.0f, amp = 0.5f, freq = 1.0f / scale;
            for (int oct = 0; oct < 3; oct++) {
                const float tx = (float)(bx * st.block) * freq, ty = (float)(by * st.block) * freq;
                const int ix = (int)floorf(tx), iy = (int)floorf(ty);
                float fx = tx - (float)ix, fy = ty - (float)iy;
                fx = fx * fx * (3.0f - 2.0f * fx);
                fy = fy * fy * (3.0f - 2.0f * fy);
                const float top = lattice(ix, iy) * (1.0f - fx) + lattice(ix + 1, iy) * fx;
                const float bot = lattice(ix, iy + 1) * (1.0f - fx) + lattice(ix + 1, iy + 1) * fx;
                sum += amp * (top * (1.0f - fy) + bot * fy);
                amp *= 0.5f;
                freq *= 2.0f;
            }
            st.pStart[i] = sum * 0.9f;
        }
    }
}

static void FxJobPerlin(void* c, int r0, int r1) {
    FxState* st = (FxState*)c;
    const float s = st->s;
    const int block = st->block;
    for (int by = r0; by < r1; by++) {
        const int sy0 = by * block;
        const int bhh = MacMin(block, st->H - sy0);
        const float* start = st->pStart.data() + (size_t)by * st->cols;
        for (int bx = 0; bx < st->cols; bx++) {
            const float local = (s - start[bx]) * (1.0f / 0.15f);
            if (local >= 1.0f) continue;
            const uint32_t a = local <= 0.0f ? 255u : (uint32_t)((1.0f - local) * 255.0f + 0.5f);
            if (!a) continue;
            const int sx0 = bx * block;
            const int bww = MacMin(block, st->W - sx0);
            const int dx = st->ox + sx0;
            // A block straddling the canvas edge (window partly off screen) is
            // cut at the edge; 3.8 skipped it whole, so it vanished at once.
            const int cx0 = (dx < 0) ? -dx : 0;
            const int cx1 = (dx + bww > st->bw) ? st->bw - dx : bww;
            if (cx1 <= cx0) continue;
            for (int r = 0; r < bhh; r++) {
                const int dy = st->oy + sy0 + r;
                if (dy < 0 || dy >= st->bh) continue;
                const uint32_t* sp = st->src + (size_t)(sy0 + r) * st->W + sx0 + cx0;
                uint32_t* dp = st->dst + (size_t)dy * st->bw + (dx + cx0);
                const int span = cx1 - cx0;
                if (a >= 255) memcpy(dp, sp, (size_t)span * 4);
                else for (int x = 0; x < span; x++) dp[x] = FxMul(sp[x], a);
            }
        }
    }
}

static RECT FxRenderPerlin(FxState& st, float s) {
    if (s <= 0.0f) return FxRenderIdentity(st);
    if ((int)st.pStart.size() != st.nParticles) return FxEmpty();
    st.s = s;
    PoolRun(st.pool, FxJobPerlin, &st, 0, st.rows, 4, 32);
    return FxWindowRect(st, 0, 0);
}

// --- particles: Thanos Snap, Square Shatter, Poof --------------------------
static void FxPrecalcParticles(FxState& st) {
    const size_t n = (size_t)st.nParticles;
    st.pa.resize(n); st.pb.resize(n); st.pc.resize(n); st.pd.resize(n); st.pStart.resize(n);
    st.destX.resize(n); st.destY.resize(n); st.destA.assign(n, 0);
    // The per-frame band lists, sized once here (inside the caller's bad_alloc
    // guard) so a frame never allocates: a block touches at most
    // 2 + block / 64 of the 64-row bands.
    const size_t bands = (size_t)(st.bh + FX_BAND_ROWS - 1) / FX_BAND_ROWS + 1;
    st.binCount.reserve(bands);
    st.binStart.reserve(bands);
    st.binList.reserve(n * (size_t)(2 + st.block / FX_BAND_ROWS));
    const float W = (float)st.W, H = (float)st.H;
    const float wcx = W * 0.5f, wcy = H * 0.5f;
    const float maxDist = W + H;
    size_t i = 0;
    for (int by = 0; by < st.rows; by++) {
        for (int bx = 0; bx < st.cols; bx++, i++) {
            const int srcX = bx * st.block, srcY = by * st.block;
            const uint32_t hash = FxHash((uint32_t)srcX, (uint32_t)srcY);
            const float hx = (float)srcX + st.block * 0.5f, hy = (float)srcY + st.block * 0.5f;
            // vector from this particle's home to the dock (aimed effects)
            const float toDockX = st.aimX - (hx - wcx), toDockY = st.aimY - (hy - wcy);
            if (st.fx == FX_THANOS) {
                const float distBR = (W - (float)srcX) + (H - (float)srcY);
                float start = distBR / maxDist * 0.6f + ((float)(hash % 100) / 100.0f - 0.5f) * 0.15f;
                st.pStart[i] = start < 0.0f ? 0.0f : (start > 0.65f ? 0.65f : start);
                const float nX = (float)((hash >> 4) % 200) / 100.0f - 1.0f;
                const float nY = (float)((hash >> 8) % 200) / 100.0f - 1.0f;
                if (st.aim) {
                    // Sucked into the taskbar button: each grain flies to the dock,
                    // with a little lateral swirl so the stream is not a straight line.
                    st.pa[i] = toDockX * 0.95f + nX * W * 0.05f;
                    st.pb[i] = toDockY * 0.95f + nY * H * 0.05f;
                    st.pc[i] = -toDockY * 0.12f * nY;
                    st.pd[i] = toDockX * 0.12f * nY;
                } else {
                    const float swirl = ((float)srcY / H - 0.5f) * 2.0f;
                    st.pa[i] = W * 0.5f + nX * (W * 0.2f);
                    st.pb[i] = H * 0.2f + swirl * (H * 0.3f) + nY * (H * 0.3f);
                    st.pc[i] = nY * (W * 0.4f);
                    st.pd[i] = -nX * (H * 0.4f);
                }
            } else if (st.fx == FX_SHATTER) {
                const float force = (float)((hash >> 8) % 100) / 100.0f;
                const float nX = (float)(hash % 2000) / 1000.0f - 1.0f;
                const float nY = (float)((hash >> 4) % 2000) / 1000.0f - 1.0f;
                float dirX = hx - wcx, dirY = hy - wcy;
                float travel = (200.0f + force * 1000.0f) * st.travelScale;
                if (st.aim) {
                    dirX = toDockX;
                    dirY = toDockY;
                    travel = sqrtf(dirX * dirX + dirY * dirY) * (0.9f + 0.2f * force);
                }
                const float len = sqrtf(dirX * dirX + dirY * dirY);
                if (len > 0.1f) { dirX /= len; dirY /= len; }
                st.pa[i] = dirX;
                st.pb[i] = dirY;
                st.pc[i] = travel;
                st.pd[i] = nX * (st.aim ? 0.15f : 0.4f);
                st.pStart[i] = nY * (st.aim ? 0.15f : 0.4f);
            } else {   // FX_POOF
                const float force = (float)((hash >> 8) % 100) / 100.0f;
                const float nX = (float)(hash % 2000) / 1000.0f - 1.0f;
                const float rise = (0.35f + 0.65f * force) * fmaxf(H, 80.0f * st.dpi) * 0.9f;
                st.pStart[i] = (float)((hash >> 12) % 1000) / 1000.0f * 0.5f;
                st.pa[i] = nX * W * 0.18f;
                st.pb[i] = -rise * 0.6f;
                st.pc[i] = -rise * 0.4f;
                st.pd[i] = 0.0f;
            }
        }
    }
}

// Pass A, parallel by particle: where each particle lands this frame.
static void FxJobParticlesA(void* c, int i0, int i1) {
    FxState* st = (FxState*)c;
    const float s = st->s;
    const int block = st->block, cols = st->cols;
    LONG mnX = LONG_MAX, mnY = LONG_MAX, mxX = LONG_MIN, mxY = LONG_MIN;
    for (int i = i0; i < i1; i++) {
        int dx = st->ox + (i % cols) * block;
        int dy = st->oy + (i / cols) * block;
        uint32_t a = 255;
        if (st->fx == FX_SHATTER) {
            const float travel = st->fa * st->pc[i];
            dx += (int)(st->pa[i] * travel + st->pd[i] * travel);
            dy += (int)(st->pb[i] * travel + st->pStart[i] * travel);
            a = (uint32_t)st->ia;
        } else if (st->fx == FX_THANOS) {
            // Starts reach 0.65 and a grain lives 0.4: ReDrag's timeline ran to
            // 1.05, so the latest grains were still at 12% when the ghost went.
            // Compressing it by 1/1.05 makes the last frame empty.
            const float local = (s * 1.05f - st->pStart[i]) * (1.0f / 0.4f);
            if (local >= 1.0f) {
                a = 0;
            } else if (local > 0.0f) {
                dx += (int)(st->pa[i] * local + st->pc[i] * local * local);
                dy += (int)(st->pb[i] * local + st->pd[i] * local * local);
                a = (uint32_t)((1.0f - local) * 255.0f + 0.5f);
            }
        } else {   // FX_POOF
            const float local = (s - st->pStart[i]) * (1.0f / 0.5f);
            if (local >= 1.0f) {
                a = 0;
            } else if (local > 0.0f) {
                dx += (int)(st->pa[i] * local);
                dy += (int)(st->pb[i] * local + st->pc[i] * local * local);
                const float k = 1.0f - local;
                a = (uint32_t)(k * k * 255.0f + 0.5f);
            }
        }
        if (!a || dx >= st->bw || dy >= st->bh || dx + block <= 0 || dy + block <= 0) {
            st->destA[i] = 0;
            continue;
        }
        st->destX[i] = dx;
        st->destY[i] = dy;
        st->destA[i] = (uint8_t)a;
        if (dx < mnX) mnX = dx;
        if (dy < mnY) mnY = dy;
        if (dx + block > mxX) mxX = dx + block;
        if (dy + block > mxY) mxY = dy + block;
    }
    if (mnX <= mxX) {
        FxAtomicMin(&st->bbMinX, mnX);
        FxAtomicMin(&st->bbMinY, mnY);
        FxAtomicMax(&st->bbMaxX, mxX);
        FxAtomicMax(&st->bbMaxY, mxY);
    }
}

static inline void FxBlitParticle(FxState* st, int i, int clipY0, int clipY1) {
    const int block = st->block;
    const int sx = (i % st->cols) * block, sy = (i / st->cols) * block;
    const int bwB = MacMin(block, st->W - sx), bhB = MacMin(block, st->H - sy);
    const int dx = st->destX[i], dy = st->destY[i];
    const uint32_t a = st->destA[i];
    const int ya = MacMax(dy, clipY0), yb = MacMin(dy + bhB, clipY1);
    const int xa = MacMax(dx, 0), xb = MacMin(dx + bwB, st->bw);
    if (xa >= xb) return;
    for (int y = ya; y < yb; y++) {
        const uint32_t* sp = st->src + (size_t)(sy + y - dy) * st->W + sx + (xa - dx);
        uint32_t* dp = st->dst + (size_t)y * st->bw + xa;
        for (int x = 0; x < xb - xa; x++) {
            uint32_t px = sp[x];
            if (a < 255) px = FxMul(px, a);
            dp[x] = FxOver(dp[x], px);
        }
    }
}

// Pass B, parallel by 64-row destination band: each band composites its
// particles in the original order, so the result is the serial one.
static void FxJobParticlesB(void* c, int b0, int b1) {
    FxState* st = (FxState*)c;
    for (int b = b0; b < b1; b++) {
        const int y0 = b * FX_BAND_ROWS, y1 = MacMin(st->bh, y0 + FX_BAND_ROWS);
        for (int k = st->binStart[b]; k < st->binStart[b + 1]; k++) FxBlitParticle(st, st->binList[k], y0, y1);
    }
}

// Erases the blocks the previous frame drew (parallel; overlapping blocks are
// zeroed twice, which is harmless) and forgets them.
static void FxJobParticlesClear(void* c, int i0, int i1) {
    FxState* st = (FxState*)c;
    const int block = st->block;
    for (int i = i0; i < i1; i++) {
        if (!st->destA[i]) continue;
        const int dx = st->destX[i], dy = st->destY[i];
        const int xa = MacMax(dx, 0), xb = MacMin(dx + block, st->bw);
        const int ya = MacMax(dy, 0), yb = MacMin(dy + block, st->bh);
        for (int y = ya; y < yb && xa < xb; y++) memset(st->dst + (size_t)y * st->bw + xa, 0, (size_t)(xb - xa) * 4);
        st->destA[i] = 0;
    }
}

static RECT FxRenderParticles(FxState& st, float s) {
    st.particlesDrawn = false;
    if (s <= 0.0f) return FxRenderIdentity(st);
    if ((int)st.destA.size() != st.nParticles || st.nParticles <= 0) return FxEmpty();
    st.s = s;
    st.particlesDrawn = true;
    if (st.fx == FX_SHATTER) {
        const float k = 1.0f - s;
        st.fa = 1.0f - k * k * k * k * k;
        const float fade = st.aim ? 1.0f - s * s * s * s : k;
        st.ia = (int)(fade * 255.0f + 0.5f);
        if (st.ia <= 0) return FxEmpty();
    }
    st.bbMinX = LONG_MAX; st.bbMinY = LONG_MAX; st.bbMaxX = LONG_MIN; st.bbMaxY = LONG_MIN;
    PoolRun(st.pool, FxJobParticlesA, &st, 0, st.nParticles, 2048, 8192);
    if (st.bbMinX >= st.bbMaxX || st.bbMinY >= st.bbMaxY) return FxEmpty();
    if (st.pool && st.pool->nWorkers > 0) {
        // Stable counting sort of the visible particles into destination bands.
        st.nBands = (st.bh + FX_BAND_ROWS - 1) / FX_BAND_ROWS;
        st.binCount.assign((size_t)st.nBands + 1, 0);
        for (int i = 0; i < st.nParticles; i++) {
            if (!st.destA[i]) continue;
            const int b0 = MacMax(0, st.destY[i]) / FX_BAND_ROWS;
            const int b1 = (MacMin(st.bh, st.destY[i] + st.block) - 1) / FX_BAND_ROWS;
            for (int b = b0; b <= b1; b++) st.binCount[b]++;
        }
        st.binStart.resize((size_t)st.nBands + 1);
        int total = 0;
        for (int b = 0; b < st.nBands; b++) { st.binStart[b] = total; total += st.binCount[b]; st.binCount[b] = st.binStart[b]; }
        st.binStart[st.nBands] = total;
        st.binList.resize((size_t)total);
        for (int i = 0; i < st.nParticles; i++) {
            if (!st.destA[i]) continue;
            const int b0 = MacMax(0, st.destY[i]) / FX_BAND_ROWS;
            const int b1 = (MacMin(st.bh, st.destY[i] + st.block) - 1) / FX_BAND_ROWS;
            for (int b = b0; b <= b1; b++) st.binList[st.binCount[b]++] = i;
        }
        PoolRun(st.pool, FxJobParticlesB, &st, 0, st.nBands, 1, 2);
    } else {
        for (int i = 0; i < st.nParticles; i++) {
            if (st.destA[i]) FxBlitParticle(&st, i, 0, st.bh);
        }
    }
    return FxClip(st, (int)st.bbMinX, (int)st.bbMinY, (int)st.bbMaxX, (int)st.bbMaxY);
}

// --- dispatch ---------------------------------------------------------------
// -------------------------------------------------------------------------
// V4 effects: Scale, Pixel Melt, Glass, Fluid
//
// Same rules as the 3.8 effects: a pure function of s (0 = the window, 1 =
// gone), identity at s = 0 byte for byte, nothing carried between frames (so
// every one reverses exactly), deterministic on the worker pool.
// -------------------------------------------------------------------------

// Mip pyramid of the snapshot for effects that shrink the whole window (Scale,
// Fluid): bilinear sampling alone shimmers below half size.
static void FxEnsureMips(FxState& st) {
    if (!st.mips.empty()) return;
    MipsPlan((const DWORD*)st.src, st.W, st.H, st.mips);
    MipsFillParallel(st.pool, st.mips, 1);
}

// Pyramid level for a minification `scale` (1 = full size), -0.5 lod bias.
static int FxMipLevelFor(const FxState& st, float scale) {
    if (st.mips.size() < 2 || scale >= 0.7071f) return 0;
    float lod = -log2f(fmaxf(scale, 1e-4f)) - 0.5f;
    int l = (int)lod;
    if (l < 0) l = 0;
    if (l > (int)st.mips.size() - 1) l = (int)st.mips.size() - 1;
    return l;
}

// FxRenderTransform, sampling a mip level when the window is small. The scale
// centre and the canvas box stay in full-size window pixels; only the source
// the job reads (and the step through it) change.
static RECT FxRenderTransformMip(FxState& st, float scale, float offX, float offY, float alpha) {
    const int lvl = FxMipLevelFor(st, scale);
    if (lvl == 0) return FxRenderTransform(st, scale, offX, offY, alpha);
    FxNoteTransform(st, scale, offX, offY, alpha);
    const int a = (int)(FxClamp01(alpha) * 255.0f + 0.5f);
    if (a <= 0 || scale < 0.002f) return FxEmpty();
    st.ia = a;
    const float cxw = st.W * 0.5f + offX;
    const float cyw = st.H * 0.5f + offY;
    const float cx = (float)st.ox + cxw;
    const float cy = (float)st.oy + cyw;
    const float hw = st.W * 0.5f * scale, hh = st.H * 0.5f * scale;
    const RECT r = FxClip(st, (int)floorf(cx - hw) - 1, (int)floorf(cy - hh) - 1,
                          (int)ceilf(cx + hw) + 2, (int)ceilf(cy + hh) + 2);
    if (FxIsEmpty(r)) return r;
    const MipLevel& M = st.mips[(size_t)lvl];
    const float ratio = (float)M.w / (float)st.W;
    // The job maps canvas -> source with one inverse scale and the source's own
    // half size, around the scale centre; express all of it in level pixels.
    const uint32_t* src0 = st.src;
    const int W0 = st.W, H0 = st.H;
    st.src = (const uint32_t*)M.p;
    st.W = M.w;
    st.H = M.h;
    st.fa = ratio / scale;
    // Canvas pixel x maps to level u = ((x - ox) + 0.5 - cxw) * inv + W_l / 2; the
    // job computes ((x - ox) + 0.5 - fb) * inv + W_l / 2, so fb = cxw stays as is.
    // u_l = u_0 * ratio; with inv_l = ratio / scale the job's "+ W_l / 2" equals
    // "W_0 / 2 * ratio", so the scale centre is used as is.
    st.fb = cxw;
    st.fc = cyw;
    st.rx0 = r.left;
    st.rx1 = r.right;
    PoolRun(st.pool, FxJobTransform, &st, r.top, r.bottom, 16, 128);
    st.src = src0;
    st.W = W0;
    st.H = H0;
    return r;
}

// --- Scale (the macOS scale effect) -------------------------------------
// The window shrinks straight into the taskbar button, keeping its shape, and
// fades only at the very end. Without a dock (close, special windows, a side
// taskbar) it shrinks into its own centre.
static RECT FxRenderScale(FxState& st, float s) {
    if (s <= 0.0f) return FxRenderIdentity(st);
    // macOS uses an ease-in-out curve for both directions.
    const float e = s < 0.5f ? 4.0f * s * s * s : 1.0f - powf(-2.0f * s + 2.0f, 3.0f) * 0.5f;
    float endScale = 0.2f, offX = 0.0f, offY = 0.0f;
    if (st.toDock) {
        // Lands at roughly the size of a taskbar icon.
        endScale = MacClampf(32.0f * st.dpi / (float)MacMax(st.W, st.H), 0.01f, 0.5f);
        offX = st.aimX * e;
        offY = st.aimY * e;
    }
    const float scale = 1.0f + (endScale - 1.0f) * e;
    const float alpha = 1.0f - FxSmooth(st.toDock ? 0.82f : 0.45f, 1.0f, s);
    return FxRenderTransformMip(st, scale, offX, offY, alpha);
}

// --- Pixel Melt (ReDrag's Windows Animations 1.3.7) ----------------------
// Columns drip down like wet paint: 120 px strips get a lag and a speed from
// 1-D fbm noise, smoothly interpolated per 6 px column, so neighbouring columns
// melt together. Rows are sampled bilinearly (ReDrag stretched by nearest
// rows); each job owns whole columns, so the pool never overlaps.
#define FX_MELT_STRIP_PX 120
#define FX_MELT_COL_PX 6

static float FxMeltHash01(int ix) {
    uint32_t h = (uint32_t)ix * 747796405u + 2891336453u;
    h = (h ^ (h >> 14)) * 277803737u;
    h ^= h >> 13;
    return (float)(h & 0xFFFFFFu) / 16777215.0f;
}

static float FxMeltFbm(float x) {
    float v = 0.0f, a = 0.5f, n = 0.0f;
    for (int o = 0; o < 4; ++o) {
        const int i = (int)floorf(x);
        float f = x - (float)i;
        f = f * f * (3.0f - 2.0f * f);
        v += a * (FxMeltHash01(i) * (1.0f - f) + FxMeltHash01(i + 1) * f);
        n += a;
        x = x * 2.03f + 17.0f;
        a *= 0.5f;
    }
    return v / fmaxf(0.001f, n);
}

static void FxPrecalcMelt(FxState& st) {
    const int strips = (st.W + FX_MELT_STRIP_PX - 1) / FX_MELT_STRIP_PX;
    std::vector<float> lag((size_t)strips), spd((size_t)strips);
    for (int i = 0; i < strips; ++i) {
        const float a = FxMeltFbm((float)i * 0.85f);
        const float b = FxMeltFbm((float)i * 1.7f + 9.1f);
        float n = FxClamp01(0.6f * a + 0.4f * b);
        n = n * n * (3.0f - 2.0f * n);
        lag[(size_t)i] = 0.02f + n * 0.52f;
        spd[(size_t)i] = 0.4f + (1.0f - n) * 0.85f;
    }
    // Limit the step between neighbouring strips, both directions, three passes.
    auto limit = [&](std::vector<float>& v, float maxStep) {
        for (int pass = 0; pass < 3; ++pass) {
            for (int i = 1; i < strips; ++i) {
                const float d = v[(size_t)i] - v[(size_t)i - 1];
                if (d > maxStep) v[(size_t)i] = v[(size_t)i - 1] + maxStep;
                if (d < -maxStep) v[(size_t)i] = v[(size_t)i - 1] - maxStep;
            }
            for (int i = strips - 2; i >= 0; --i) {
                const float d = v[(size_t)i] - v[(size_t)i + 1];
                if (d > maxStep) v[(size_t)i] = v[(size_t)i + 1] + maxStep;
                if (d < -maxStep) v[(size_t)i] = v[(size_t)i + 1] - maxStep;
            }
        }
    };
    limit(lag, 0.08f);
    limit(spd, 0.12f);
    // Per 6 px column, smoothstep-interpolated between strip centres.
    const int cols = (st.W + FX_MELT_COL_PX - 1) / FX_MELT_COL_PX;
    st.colBase.resize((size_t)cols);
    st.rowBase.resize((size_t)cols);
    for (int c = 0; c < cols; ++c) {
        const float px = (float)(c * FX_MELT_COL_PX) + 0.5f * (float)MacMin(FX_MELT_COL_PX, st.W - c * FX_MELT_COL_PX);
        const float f = px / (float)FX_MELT_STRIP_PX - 0.5f;
        int i0 = (int)floorf(f);
        float u = f - (float)i0;
        if (i0 < 0) { i0 = 0; u = 0.0f; }
        int i1 = i0 + 1;
        if (i0 >= strips) i0 = strips - 1;
        if (i1 >= strips) i1 = strips - 1;
        u = u * u * (3.0f - 2.0f * u);
        st.colBase[(size_t)c] = lag[(size_t)i0] * (1.0f - u) + lag[(size_t)i1] * u;    // lag
        st.rowBase[(size_t)c] = spd[(size_t)i0] * (1.0f - u) + spd[(size_t)i1] * u;    // speed
    }
}

// Job over column indices [c0, c1). fa = s.
static void FxJobMelt(void* c, int c0, int c1) {
    FxState* st = (FxState*)c;
    const float s = st->fa;
    const int W = st->W, H = st->H;
    for (int ci = c0; ci < c1; ++ci) {
        const int x0 = ci * FX_MELT_COL_PX, x1 = MacMin(W, x0 + FX_MELT_COL_PX);
        const float lag = st->colBase[(size_t)ci], spd = st->rowBase[(size_t)ci];
        float fall = 0.0f, stretch = 1.0f, alpha = 1.0f;
        if (s > lag) {
            float t = (s - lag) / fmaxf(0.2f, 1.0f - lag);
            if (t > 1.0f) t = 1.0f;
            t = t * t * t;
            fall = t * (float)H * (0.75f + 0.95f * spd);
            stretch = 1.0f + t * (0.3f + 0.7f * spd);
            if (t > 0.55f) alpha = powf(1.0f - (t - 0.55f) / 0.45f, 1.2f);
        }
        if (alpha <= 0.01f) continue;
        const uint32_t a255 = (uint32_t)(alpha * 255.0f + 0.5f);
        const int dx0 = MacMax(x0, -st->ox), dx1 = MacMin(x1, st->bw - st->ox);
        if (dx1 <= dx0) continue;
        if (fall == 0.0f) {
            for (int y = 0; y < H; ++y) {
                const int oy = st->oy + y;
                if (oy < 0 || oy >= st->bh) continue;
                const uint32_t* sp = st->src + (size_t)y * W;
                uint32_t* dp = st->dst + (size_t)oy * st->bw + st->ox;
                for (int x = dx0; x < dx1; ++x) dp[x] = (a255 >= 255) ? sp[x] : FxMul(sp[x], a255);
            }
            continue;
        }
        // Window coordinates (the canvas may start anywhere after the desktop clip).
        const float dstH = (float)H * stretch;
        const int yA = MacMax(0, st->oy + (int)floorf(fall)), yB = MacMin(st->bh, st->oy + (int)ceilf(fall + dstH));
        for (int oy = yA; oy < yB; ++oy) {
            const float v = (((float)(oy - st->oy) + 0.5f - fall) / dstH) * (float)H - 0.5f;
            if (v < -0.5f || v > (float)H - 0.5f) continue;
            const float fv = floorf(v);
            int r0 = (int)fv, r1 = r0 + 1;
            int wy = (int)((v - fv) * 256.0f);
            if (r0 < 0) { r0 = 0; wy = 0; }
            if (r1 >= H) r1 = H - 1;
            if (r0 >= H) r0 = H - 1;
            const uint32_t* s0 = st->src + (size_t)r0 * W;
            const uint32_t* s1 = st->src + (size_t)r1 * W;
            uint32_t* dp = st->dst + (size_t)oy * st->bw + st->ox;
            for (int x = dx0; x < dx1; ++x) {
                uint32_t p = wy ? Lerp8(s0[x], s1[x], wy) : s0[x];
                dp[x] = (a255 >= 255) ? p : FxMul(p, a255);
            }
        }
    }
}

static RECT FxRenderMelt(FxState& st, float s) {
    if (s <= 0.0f) return FxRenderIdentity(st);
    st.fa = s;
    const int cols = (int)st.colBase.size();
    PoolRun(st.pool, FxJobMelt, &st, 0, cols, 8, 32);
    return FxClip(st, st.ox, st.oy, st.ox + st.W, st.bh);
}

// --- Glass -----------------------------------------------------------------
// Irregular glass: the window is cut by random lines (a BSP) into convex shards
// that break away from an impact point, spin and fall. Every source pixel is
// owned by exactly one shard (the BSP decides), so a frame where shards have not
// moved yet is a plain copy - identity at s = 0 - and a moving shard is sampled
// bilinearly from its own pixels only, which antialiases its edges. Rendering is
// split into 64-row destination bands; each band composites the shards in a
// fixed order, so the pool's output equals the single-threaded one.
#define FX_GLASS_MAX_SHARDS 96
#define FX_GLASS_MAX_VERTS 14

struct FxGlassNode { float a, b, c; int child[2]; };   // child < 0: leaf -(id + 1)
struct FxGlassShard {
    float vx[FX_GLASS_MAX_VERTS], vy[FX_GLASS_MAX_VERTS];
    int n;
    float cx, cy;            // centroid (window pixels)
    float delay, dirX, dirY, spin, fallK;
    int bx0, by0, bx1, by1;  // source bounding box
};

static int FxGlassClip(const float* ix, const float* iy, int n, float a, float b, float c, bool keepPos,
                       float* ox, float* oy) {
    int m = 0;
    for (int i = 0; i < n; ++i) {
        const int j = (i + 1) % n;
        const float di = a * ix[i] + b * iy[i] + c, dj = a * ix[j] + b * iy[j] + c;
        const bool inI = keepPos ? di >= 0.0f : di < 0.0f;
        const bool inJ = keepPos ? dj >= 0.0f : dj < 0.0f;
        if (inI && m < FX_GLASS_MAX_VERTS) { ox[m] = ix[i]; oy[m] = iy[i]; ++m; }
        if (inI != inJ && m < FX_GLASS_MAX_VERTS) {
            const float t = di / (di - dj);
            ox[m] = ix[i] + (ix[j] - ix[i]) * t;
            oy[m] = iy[i] + (iy[j] - iy[i]) * t;
            ++m;
        }
    }
    return m;
}

static float FxGlassArea(const float* x, const float* y, int n) {
    float a = 0.0f;
    for (int i = 0; i < n; ++i) {
        const int j = (i + 1) % n;
        a += x[i] * y[j] - x[j] * y[i];
    }
    return fabsf(a) * 0.5f;
}

struct FxGlassData {
    std::vector<FxGlassNode> nodes;
    std::vector<FxGlassShard> shards;
    std::vector<uint8_t> owner;       // W x H shard id per source pixel
    std::vector<int> order;           // draw order
};

static FxGlassData* FxGlass(FxState& st) { return (FxGlassData*)st.extra.get(); }

// The owner map, one row at a time by SPANS: along a row every BSP line is
// crossed at most once, so the row splits into a handful of runs that are
// filled with memset (3.x-style per-pixel traversal cost 23 ms at 4K).
static void FxGlassFillSpan(const FxGlassNode* nodes, int n, uint8_t* row, int x0, int x1, float py) {
    while (x1 > x0) {
        const FxGlassNode& nd = nodes[n];
        // d(x) at pixel centres is linear in x: d = a * (x + 0.5) + b * py + c
        const float k0 = nd.b * py + nd.c;
        const float d0 = nd.a * ((float)x0 + 0.5f) + k0;
        const float d1 = nd.a * ((float)(x1 - 1) + 0.5f) + k0;
        const bool p0 = d0 >= 0.0f, p1 = d1 >= 0.0f;
        if (p0 == p1) {
            const int ch = nd.child[p0 ? 0 : 1];
            if (ch < 0) { memset(row + x0, -ch - 1, (size_t)(x1 - x0)); return; }
            n = ch;
            continue;
        }
        // First pixel whose sign differs from x0's; checked exactly around the
        // analytic crossing so it matches a per-pixel test.
        int xs = (nd.a != 0.0f) ? (int)floorf(-k0 / nd.a - 0.5f) : x0;
        if (xs < x0) xs = x0;
        if (xs > x1) xs = x1;
        auto pos = [&](int x) { return nd.a * ((float)x + 0.5f) + k0 >= 0.0f; };
        while (xs > x0 && pos(xs - 1) != p0) xs--;
        while (xs < x1 && pos(xs) == p0) xs++;
        const int chA = nd.child[p0 ? 0 : 1], chB = nd.child[p0 ? 1 : 0];
        if (chA < 0) memset(row + x0, -chA - 1, (size_t)(xs - x0));
        else FxGlassFillSpan(nodes, chA, row, x0, xs, py);
        x0 = xs;
        if (chB < 0) { memset(row + x0, -chB - 1, (size_t)(x1 - x0)); return; }
        n = chB;
    }
}

static void FxJobGlassOwner(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    FxGlassData* g = FxGlass(*st);
    for (int y = y0; y < y1; ++y) {
        FxGlassFillSpan(g->nodes.data(), 0, g->owner.data() + (size_t)y * st->W, 0, st->W, (float)y + 0.5f);
    }
}

static void FxPrecalcGlass(FxState& st) {
    std::unique_ptr<FxGlassData> g(new FxGlassData());
    const float W = (float)st.W, H = (float)st.H;
    int target = (int)(sqrtf(W * H) / 38.0f);
    target = MacClampInt(target, st.data->special ? 8 : 14, FX_GLASS_MAX_SHARDS);
    uint32_t rng = 0x9E3779B9u ^ ((uint32_t)st.W * 2654435761u) ^ ((uint32_t)st.H * 40503u);
    auto rnd = [&]() -> float {
        rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5;
        return (float)(rng & 0xFFFFFF) / 16777215.0f;
    };
    // Work list of leaves: polygon + the node slot that points at it.
    struct Leaf { float x[FX_GLASS_MAX_VERTS], y[FX_GLASS_MAX_VERTS]; int n; int parent; int side; };
    std::vector<Leaf> leaves;
    leaves.reserve(FX_GLASS_MAX_SHARDS);
    Leaf root = {};
    root.x[0] = 0; root.y[0] = 0; root.x[1] = W; root.y[1] = 0;
    root.x[2] = W; root.y[2] = H; root.x[3] = 0; root.y[3] = H;
    root.n = 4; root.parent = -1; root.side = 0;
    leaves.push_back(root);
    // The impact: near the centre (a little random).
    const float impX = W * (0.42f + 0.16f * rnd()), impY = H * (0.40f + 0.16f * rnd());
    int attempts = target * 12;
    while ((int)leaves.size() < target && attempts-- > 0) {
        int best = -1;
        float bestA = 0.0f;
        for (int i = 0; i < (int)leaves.size(); ++i) {
            const float a = FxGlassArea(leaves[i].x, leaves[i].y, leaves[i].n);
            // Prefer cutting pieces near the impact: finer shards there.
            float cx = 0, cy = 0;
            for (int k = 0; k < leaves[i].n; ++k) { cx += leaves[i].x[k]; cy += leaves[i].y[k]; }
            cx /= leaves[i].n; cy /= leaves[i].n;
            const float d = hypotf(cx - impX, cy - impY) / hypotf(W, H);
            const float score = a * (1.6f - d);
            if (score > bestA) { bestA = score; best = i; }
        }
        if (best < 0) break;
        Leaf L = leaves[(size_t)best];
        if (FxGlassArea(L.x, L.y, L.n) < 64.0f) break;
        float cx = 0, cy = 0, minX = 1e9f, maxX = -1e9f, minY = 1e9f, maxY = -1e9f;
        for (int k = 0; k < L.n; ++k) {
            cx += L.x[k]; cy += L.y[k];
            minX = fminf(minX, L.x[k]); maxX = fmaxf(maxX, L.x[k]);
            minY = fminf(minY, L.y[k]); maxY = fmaxf(maxY, L.y[k]);
        }
        cx /= L.n; cy /= L.n;
        const float px = cx + (rnd() - 0.5f) * 0.4f * (maxX - minX);
        const float py = cy + (rnd() - 0.5f) * 0.4f * (maxY - minY);
        // Cut across the longer side, +- 50 degrees, so shards stay chunky.
        float ang = (maxX - minX) > (maxY - minY) ? PI * 0.5f : 0.0f;
        ang += (rnd() - 0.5f) * 1.75f;
        const float a = cosf(ang), b = sinf(ang), c = -(a * px + b * py);
        Leaf A = {}, Bl = {};
        A.n = FxGlassClip(L.x, L.y, L.n, a, b, c, true, A.x, A.y);
        Bl.n = FxGlassClip(L.x, L.y, L.n, a, b, c, false, Bl.x, Bl.y);
        if (A.n < 3 || Bl.n < 3 || FxGlassArea(A.x, A.y, A.n) < 16.0f || FxGlassArea(Bl.x, Bl.y, Bl.n) < 16.0f) {
            continue;   // a degenerate cut: try another (bounded by `attempts`)
        }
        FxGlassNode nd;
        nd.a = a; nd.b = b; nd.c = c;
        nd.child[0] = nd.child[1] = -1;
        const int ni = (int)g->nodes.size();
        g->nodes.push_back(nd);
        if (L.parent >= 0) g->nodes[(size_t)L.parent].child[L.side] = ni;
        A.parent = ni; A.side = 0;
        Bl.parent = ni; Bl.side = 1;
        leaves[(size_t)best] = A;
        leaves.push_back(Bl);
    }
    if (g->nodes.empty()) {
        // A window too small to cut: one shard, a single "always" node.
        FxGlassNode nd = { 0.0f, 0.0f, 1.0f, { -1, -1 } };
        g->nodes.push_back(nd);
        leaves[0].parent = 0;
        leaves[0].side = 0;
    }
    // Leaves -> shards; point the BSP at them.
    const float diag = hypotf(W, H);
    const float k = st.dpi * MacClampf(diag / 1400.0f, 0.35f, 1.0f);
    g->shards.resize(leaves.size());
    for (size_t i = 0; i < leaves.size(); ++i) {
        const Leaf& L = leaves[i];
        FxGlassShard& sh = g->shards[i];
        sh.n = L.n;
        float cx = 0, cy = 0;
        float minX = 1e9f, maxX = -1e9f, minY = 1e9f, maxY = -1e9f;
        for (int v = 0; v < L.n; ++v) {
            sh.vx[v] = L.x[v]; sh.vy[v] = L.y[v];
            cx += L.x[v]; cy += L.y[v];
            minX = fminf(minX, L.x[v]); maxX = fmaxf(maxX, L.x[v]);
            minY = fminf(minY, L.y[v]); maxY = fmaxf(maxY, L.y[v]);
        }
        sh.cx = cx / L.n; sh.cy = cy / L.n;
        sh.bx0 = MacMax(0, (int)floorf(minX) - 1); sh.by0 = MacMax(0, (int)floorf(minY) - 1);
        sh.bx1 = MacMin(st.W, (int)ceilf(maxX) + 1); sh.by1 = MacMin(st.H, (int)ceilf(maxY) + 1);
        const float dx = sh.cx - impX, dy = sh.cy - impY;
        const float dist = hypotf(dx, dy) / diag;           // 0 .. ~0.7
        sh.delay = MacClampf(dist * 0.55f + rnd() * 0.12f, 0.0f, 0.55f);
        const float len = fmaxf(1.0f, hypotf(dx, dy));
        const float push = (90.0f + 260.0f * (1.0f - fminf(1.0f, dist * 1.6f))) * k;
        sh.dirX = dx / len * push + (rnd() - 0.5f) * 60.0f * k;
        sh.dirY = dy / len * push * 0.6f - (40.0f + 60.0f * rnd()) * k;   // a little pop upwards
        sh.spin = (rnd() - 0.5f) * 3.2f;
        sh.fallK = (0.9f + 0.5f * rnd()) * H * 1.1f;
        g->nodes[(size_t)L.parent].child[L.side] = -(int)i - 1;
    }
    g->owner.resize((size_t)st.W * st.H);
    // Draw order: the shards that break first are drawn last (on top).
    g->order.resize(g->shards.size());
    for (size_t i = 0; i < g->order.size(); ++i) g->order[i] = (int)i;
    std::sort(g->order.begin(), g->order.end(), [&](int a, int b) {
        return g->shards[(size_t)a].delay > g->shards[(size_t)b].delay ||
               (g->shards[(size_t)a].delay == g->shards[(size_t)b].delay && a < b);
    });
    st.extra.reset(g.release());
    PoolRun(st.pool, FxJobGlassOwner, &st, 0, st.H, 32, 128);
}


// The part of row yc (canvas) inside the shard's destination polygon shrunk by
// `inset` px (negative = grown). Convex polygon: the intersection of its edges'
// half-planes. Returns false when empty.
static inline bool FxGlassRowSpan(const FxGlassFrame& f, float yc, float inset, float* xa, float* xb) {
    float lo = -1e9f, hi = 1e9f;
    for (int i = 0; i < f.n; ++i) {
        const float rhs = inset - f.ny[i] * yc + f.off[i];       // nx * x >= rhs
        if (f.nx[i] > 1e-6f) lo = fmaxf(lo, rhs / f.nx[i]);
        else if (f.nx[i] < -1e-6f) hi = fminf(hi, rhs / f.nx[i]);
        else if (rhs > 0.0f) return false;
    }
    *xa = lo;
    *xb = hi;
    return hi > lo;
}

static void FxJobGlassBand(void* c, int band0, int band1) {
    FxState* st = (FxState*)c;
    FxGlassData* g = FxGlass(*st);
    const FxGlassFrame* fr = (const FxGlassFrame*)st->extraFrame;
    const int W = st->W, H = st->H;
    const uint8_t* owner = g->owner.data();
    for (int band = band0; band < band1; ++band) {
        const int by0 = band * FX_BAND_ROWS, by1 = MacMin(st->bh, by0 + FX_BAND_ROWS);
        for (int oi : g->order) {
            const FxGlassShard& sh = g->shards[(size_t)oi];
            const FxGlassFrame& f = fr[oi];
            if (f.alpha <= 0.004f) continue;
            const int y0 = MacMax(by0, f.y0), y1 = MacMin(by1, f.y1);
            if (y1 <= y0 || f.x1 <= f.x0) continue;
            const uint32_t a255 = (uint32_t)(f.alpha * 255.0f + 0.5f);
            const uint8_t id = (uint8_t)oi;
            // Canvas -> source (window pixels): rotate back about the moved
            // centroid, unscale. Affine in x, evaluated from the WINDOW column so
            // the result does not depend on where the canvas starts.
            const float inv = 1.0f / f.k;
            const float dux = f.cs * inv, dvx = -f.sn * inv;
            for (int y = y0; y < y1; ++y) {
                const float yc = (float)(y - st->oy) + 0.5f;            // window row centre
                float oa, ob, ia, ib;
                if (!FxGlassRowSpan(f, yc, -1.0f, &oa, &ob)) continue;
                int xA = MacMax(MacMax(f.x0, 0), (int)floorf(oa - 0.5f) + st->ox);
                int xB = MacMin(MacMin(f.x1, st->bw), (int)ceilf(ob - 0.5f) + 1 + st->ox);
                if (xB <= xA) continue;
                int iA = xB, iB = xB;
                if (FxGlassRowSpan(f, yc, 2.0f, &ia, &ib)) {
                    iA = MacMax(xA, (int)ceilf(ia - 0.5f) + st->ox);
                    iB = MacMin(xB, (int)floorf(ib - 0.5f) + 1 + st->ox);
                    if (iB < iA) iB = iA;
                }
                uint32_t* dp = st->dst + (size_t)y * st->bw;
                const float wy = yc - (sh.cy + f.oy);
                const float wxw = 0.5f - (sh.cx + f.ox);                         // + window column
                const float u0 = (f.cs * wxw + f.sn * wy) * inv + sh.cx - 0.5f;   // u at window column 0
                const float v0 = (-f.sn * wxw + f.cs * wy) * inv + sh.cy - 0.5f;
                if (f.f <= 0.0f) {
                    // Not moving yet: a copy of its own pixels (exact: frame 0).
                    const int sy = y - st->oy;
                    if (sy < 0 || sy >= H) continue;
                    const uint32_t* sp = st->src + (size_t)sy * W - st->ox;
                    const uint8_t* op = owner + (size_t)sy * W - st->ox;
                    for (int x = MacMax(xA, st->ox); x < MacMin(xB, st->ox + W); ++x) {
                        if (op[x] != id) continue;
                        dp[x] = FxOver(dp[x], a255 >= 255 ? sp[x] : FxMul(sp[x], a255));
                    }
                    continue;
                }
                for (int x = xA; x < xB; ++x) {
                    const float xw = (float)(x - st->ox);
                    const float u = u0 + xw * dux, v = v0 + xw * dvx;
                    const float fu = floorf(u), fv = floorf(v);
                    const int x0 = (int)fu, y0s = (int)fv;
                    if ((unsigned)x0 >= (unsigned)(W - 1) || (unsigned)y0s >= (unsigned)(H - 1)) {
                        if (x0 < -1 || x0 >= W || y0s < -1 || y0s >= H) continue;
                    }
                    const float ax = u - fu, ay = v - fv;
                    uint32_t px;
                    const bool inner = x >= iA && x < iB && (unsigned)x0 < (unsigned)(W - 1) &&
                                       (unsigned)y0s < (unsigned)(H - 1);
                    if (inner) {
                        // >= 2 px inside the shard: all four texels are its own.
                        const uint32_t* r0 = st->src + (size_t)y0s * W + x0;
                        const int iwx = (int)(ax * 256.0f), iwy = (int)(ay * 256.0f);
                        px = Lerp8(Lerp8(r0[0], r0[1], iwx), Lerp8(r0[W], r0[W + 1], iwx), iwy);
                        if (a255 < 255) px = FxMul(px, a255);
                    } else {
                        float cb = 0, cg = 0, cr = 0, ca = 0;
                        bool any = false;
                        for (int t = 0; t < 4; ++t) {
                            const int tx = x0 + (t & 1), ty = y0s + (t >> 1);
                            if ((unsigned)tx >= (unsigned)W || (unsigned)ty >= (unsigned)H) continue;
                            if (owner[(size_t)ty * W + tx] != id) continue;
                            const float w = ((t & 1) ? ax : 1.0f - ax) * ((t >> 1) ? ay : 1.0f - ay);
                            const uint32_t p = st->src[(size_t)ty * W + tx];
                            cb += w * (float)(p & 0xFF); cg += w * (float)((p >> 8) & 0xFF);
                            cr += w * (float)((p >> 16) & 0xFF); ca += w * (float)(p >> 24);
                            any = true;
                        }
                        if (!any) continue;
                        const float m = f.alpha;
                        const uint32_t pa = (uint32_t)fminf(255.0f, ca * m + 0.5f);
                        if (!pa) continue;
                        const uint32_t pb = (uint32_t)fminf(255.0f, cb * m + 0.5f), pg = (uint32_t)fminf(255.0f, cg * m + 0.5f);
                        const uint32_t pr = (uint32_t)fminf(255.0f, cr * m + 0.5f);
                        px = (pa << 24) | (pr << 16) | (pg << 8) | pb;
                    }
                    dp[x] = FxOver(dp[x], px);
                }
            }
        }
    }
}

static RECT FxRenderGlass(FxState& st, float s) {
    if (s <= 0.0f) return FxRenderIdentity(st);
    FxGlassData* g = FxGlass(st);
    if (!g) return FxRenderIdentity(st);
    const size_t n = g->shards.size();
    if (st.glassFrame.size() != n) st.glassFrame.resize(n);
    FxGlassFrame* fr = (FxGlassFrame*)st.glassFrame.data();
    LONG minX = st.bw, minY = st.bh, maxX = 0, maxY = 0;
    for (size_t i = 0; i < n; ++i) {
        const FxGlassShard& sh = g->shards[i];
        FxGlassFrame& f = fr[i];
        const float t = FxClamp01((s - sh.delay) / fmaxf(0.2f, 1.0f - sh.delay));
        f.f = t;
        const float e = 1.0f - (1.0f - t) * (1.0f - t);             // flight eases out
        f.ox = sh.dirX * e;
        f.oy = sh.dirY * e + sh.fallK * t * t;                       // gravity
        const float ang = sh.spin * t;
        f.cs = cosf(ang);
        f.sn = sinf(ang);
        f.k = 1.0f - 0.18f * t;
        f.alpha = 1.0f - FxSmooth(0.5f, 1.0f, t);
        // Destination polygon (canvas) and box of the moved shard.
        float bx0 = 1e9f, by0 = 1e9f, bx1 = -1e9f, by1 = -1e9f;
        float PX[FX_GLASS_MAX_VERTS], PY[FX_GLASS_MAX_VERTS];
        for (int v = 0; v < sh.n; ++v) {
            const float rx = (sh.vx[v] - sh.cx) * f.k, ry = (sh.vy[v] - sh.cy) * f.k;
            // Window coordinates: nothing below depends on where the canvas starts.
            const float X = sh.cx + f.ox + f.cs * rx - f.sn * ry;
            const float Y = sh.cy + f.oy + f.sn * rx + f.cs * ry;
            PX[v] = X; PY[v] = Y;
            bx0 = fminf(bx0, X); by0 = fminf(by0, Y); bx1 = fmaxf(bx1, X); by1 = fmaxf(by1, Y);
        }
        // Inward unit normals (the polygons keep the window rect's clockwise order
        // on screen, and rotation keeps it): n = (-ey, ex) / |e|.
        f.n = sh.n;
        for (int v = 0; v < sh.n; ++v) {
            const int w2 = (v + 1) % sh.n;
            const float ex = PX[w2] - PX[v], ey = PY[w2] - PY[v];
            const float len = fmaxf(1e-6f, sqrtf(ex * ex + ey * ey));
            f.nx[v] = -ey / len;
            f.ny[v] = ex / len;
            f.off[v] = f.nx[v] * PX[v] + f.ny[v] * PY[v];
        }
        f.x0 = MacMax(0, (int)floorf(bx0) - 2 + st.ox); f.y0 = MacMax(0, (int)floorf(by0) - 2 + st.oy);
        f.x1 = MacMin(st.bw, (int)ceilf(bx1) + 2 + st.ox); f.y1 = MacMin(st.bh, (int)ceilf(by1) + 2 + st.oy);
        if (f.alpha > 0.004f && f.x1 > f.x0 && f.y1 > f.y0) {
            minX = MacMin(minX, f.x0); minY = MacMin(minY, f.y0);
            maxX = MacMax(maxX, f.x1); maxY = MacMax(maxY, f.y1);
        }
    }
    if (maxX <= minX || maxY <= minY) return FxEmpty();
    st.extraFrame = st.glassFrame.data();
    const int bands = (st.bh + FX_BAND_ROWS - 1) / FX_BAND_ROWS;
    PoolRun(st.pool, FxJobGlassBand, &st, minY / FX_BAND_ROWS, MacMin(bands, (maxY + FX_BAND_ROWS - 1) / FX_BAND_ROWS),
            1, 3);
    return FxClip(st, minX, minY, maxX, maxY);
}

// --- Fluid (after kivsak's Fluid Window Engine) ---------------------------
// The window turns into a soft droplet that pours into the taskbar button: a
// rounded head (the window, shrinking and rounding into a drop), a tail that
// stretches back to the dock and then lets go, smoothly fused with the head.
// Drawn from a signed distance field with 1 px antialiasing; the head samples
// the window (mipmapped), the tail continues its nearest edge.
struct FxFluidFrame {
    float hx, hy;            // head centre (canvas)
    float hw, hh, hr;        // head half size and corner radius
    float tx, ty;            // tail end (canvas)
    float rHead, rTip;       // tail radius at the head and at its tip
    float k;                 // smooth-union radius
    float alpha;
    float u0, v0, du, dv;    // head box -> level-0 source texel mapping
    int lvl;
};

static inline float FxSmin(float a, float b, float k) {
    if (k <= 0.0f) return fminf(a, b);
    const float h = FxClamp01(0.5f + 0.5f * (b - a) / k);
    return b + (a - b) * h - k * h * (1.0f - h);
}

static void FxJobFluid(void* c, int y0, int y1) {
    FxState* st = (FxState*)c;
    const FxFluidFrame& f = *(const FxFluidFrame*)st->extraFrame;
    const uint32_t* src = st->src;
    int SW = st->W, SH = st->H;
    if (!st->mips.empty()) {
        const MipLevel& M = st->mips[(size_t)f.lvl];
        src = (const uint32_t*)M.p;
        SW = M.w;
        SH = M.h;
    }
    const float lr = (float)SW / (float)st->W;       // level / full-size texels
    const float tdx = f.tx - f.hx, tdy = f.ty - f.hy;
    const float tl2 = tdx * tdx + tdy * tdy;
    // Deep inside the head (past its corner radius) the distance field is -1 or
    // less whatever the tail does: coverage 1, so those pixels only sample.
    const float inHW = f.hw - f.hr - 1.0f, inHH = f.hh - f.hr - 1.0f;
    const uint32_t alpha255 = (uint32_t)(f.alpha * 255.0f + 0.5f);
    for (int y = y0; y < y1; ++y) {
        uint32_t* dp = st->dst + (size_t)y * st->bw;
        const float py = (float)(y - st->oy) + 0.5f;          // window coordinates
        int ix0 = st->rx1, ix1 = st->rx1;                      // interior run of this row
        if (inHW > 0.0f && inHH > 0.0f && fabsf(py - f.hy) < inHH) {
            ix0 = MacMax(st->rx0, st->ox + (int)ceilf(f.hx - inHW - 0.5f));
            ix1 = MacMin(st->rx1, st->ox + (int)floorf(f.hx + inHW - 0.5f) + 1);
            if (ix1 < ix0) ix1 = ix0;
        }
        if (ix1 > ix0 && alpha255) {
            const float hv = MacClampf((py - (f.hy - f.hh)) / (2.0f * f.hh), 0.0f, 1.0f);
            const float sy = MacClampf((f.v0 + hv * f.dv) * lr, 0.5f, SH - 0.5f) - 0.5f;
            const int r0i = MacMin(SH - 1, (int)sy), r1i = MacMin(SH - 1, r0i + 1);
            const int wy = (int)((sy - (float)r0i) * 256.0f);
            const uint32_t* r0 = src + (size_t)r0i * SW;
            const uint32_t* r1 = src + (size_t)r1i * SW;
            const float du = f.du * lr / (2.0f * f.hw);
            // u at window column 0, stepped from there: independent of the canvas.
            const double uw0 = ((double)0.5 - (double)(f.hx - f.hw)) * (double)du + (double)(f.u0 * lr) - 0.5;
            const int64_t step = (int64_t)llround((double)du * 65536.0);
            const int64_t base16 = (int64_t)llround(uw0 * 65536.0);
            const int64_t maxU = (int64_t)(SW - 1) << 16;
            for (int x = ix0; x < ix1; ++x) {
                const int64_t u16 = base16 + (int64_t)(x - st->ox) * step;
                const int64_t uc = u16 < 0 ? 0 : (u16 > maxU ? maxU : u16);
                const int sx = (int)(uc >> 16), wx = (int)((uc >> 8) & 0xFF);
                const int sx1 = MacMin(SW - 1, sx + 1);
                uint32_t p = wx ? Lerp8(r0[sx], r0[sx1], wx) : r0[sx];
                if (wy) p = Lerp8(p, wx ? Lerp8(r1[sx], r1[sx1], wx) : r1[sx], wy);
                dp[x] = FxMul(p, alpha255);
            }
        }
        for (int x = st->rx0; x < st->rx1; ++x) {
            if (x == ix0 && ix1 > ix0) { x = ix1 - 1; continue; }
            const float px = (float)(x - st->ox) + 0.5f;
            // head: rounded box
            const float qx = fabsf(px - f.hx) - (f.hw - f.hr), qy = fabsf(py - f.hy) - (f.hh - f.hr);
            const float dHead = hypotf(fmaxf(qx, 0.0f), fmaxf(qy, 0.0f)) + fminf(fmaxf(qx, qy), 0.0f) - f.hr;
            // tail: tapered capsule head -> tip
            float dTail = 1e9f, tu = 0.0f;
            if (tl2 > 1.0f) {
                tu = FxClamp01(((px - f.hx) * tdx + (py - f.hy) * tdy) / tl2);
                const float cx = f.hx + tdx * tu, cy = f.hy + tdy * tu;
                const float w = 1.0f - tu;
                const float r = f.rTip + (f.rHead - f.rTip) * w * w;
                dTail = hypotf(px - cx, py - cy) - r;
            }
            const float d = FxSmin(dHead, dTail, f.k);
            const float cov = FxClamp01(0.5f - d);
            if (cov <= 0.0f) continue;
            // Colour: the head's texel; the tail continues the nearest head edge.
            float hu = MacClampf((px - (f.hx - f.hw)) / (2.0f * f.hw), 0.0f, 1.0f);
            float hv = MacClampf((py - (f.hy - f.hh)) / (2.0f * f.hh), 0.0f, 1.0f);
            const float sx = (f.u0 + hu * f.du) * lr, sy = (f.v0 + hv * f.dv) * lr;
            uint32_t p = FxSampleBilinear(src, SW, SH, MacClampf(sx, 0.5f, SW - 0.5f), MacClampf(sy, 0.5f, SH - 0.5f));
            // The tail's far half fades out.
            float a = cov * f.alpha;
            if (dTail < dHead) a *= 1.0f - FxSmooth(0.5f, 1.0f, tu);
            const uint32_t a255 = (uint32_t)(a * 255.0f + 0.5f);
            if (!a255) continue;
            dp[x] = FxMul(p, a255);
        }
    }
}

static RECT FxRenderFluid(FxState& st, float s) {
    if (s <= 0.0f) return FxRenderIdentity(st);
    const float t = 1.0f - s;                               // 1 = window, 0 = in the dock
    const float W = (float)st.W, H = (float)st.H;
    // Everything in WINDOW coordinates, so the frame does not depend on where the
    // (desktop-clipped) canvas starts.
    const float sx0 = W * 0.5f, sy0 = H * 0.5f;                                  // window centre
    const float dx = sx0 + st.aimX, dy = sy0 + st.aimY;                          // dock
    auto easeOutQuart = [](float x) { const float k = 1.0f - x; return 1.0f - k * k * k * k; };
    auto easeInQuart = [](float x) { return x * x * x * x; };
    const float headE = easeOutQuart(t);
    FxFluidFrame f;
    f.hx = dx + (sx0 - dx) * headE;
    f.hy = dy + (sy0 - dy) * headE;
    const float S = 0.05f + 0.95f * headE;
    const float tw = W * S, th = H * S;
    const float path = hypotf(sx0 - dx, sy0 - dy);
    const float maxR = fminf(220.0f * st.dpi, 30.0f * st.dpi + headE * fminf(path * 0.2f, 160.0f * st.dpi));
    const float circle = fminf(sqrtf(tw * th) * 0.5f, maxR) * 2.0f;
    const float morphStart = 0.8f;
    float morph = 0.0f;
    if (t > morphStart) {
        const float u = (t - morphStart) / (1.0f - morphStart);
        morph = -(cosf(PI * u) - 1.0f) * 0.5f;
    }
    const float cw = circle + (tw - circle) * morph, ch = circle + (th - circle) * morph;
    f.hw = fmaxf(0.5f, cw * 0.5f);
    f.hh = fmaxf(0.5f, ch * 0.5f);
    const float minR = fminf(f.hw, f.hh);
    const float cornerEnd = fmaxf(4.0f * S, (float)st.data->cornerRadiusPx);
    f.hr = fminf(minR, (minR * (1.0f - morph) + cornerEnd * morph));
    // Tail: anchored at the dock until 40 %, then catching up with the head.
    const float tailStart = 0.4f;
    const float tailT = t > tailStart ? (t - tailStart) / (1.0f - tailStart) : 0.0f;
    const float tailE = easeInQuart(tailT);
    f.tx = dx + (f.hx - dx) * tailE;
    f.ty = dy + (f.hy - dy) * tailE;
    f.rHead = minR * 0.62f;
    f.rTip = 2.4f * st.dpi;
    f.k = fmaxf(2.0f, minR * 0.35f);
    f.alpha = FxSmooth(0.0f, 0.07f, t);
    // Head box -> source: the whole window, squeezed into the drop.
    f.u0 = 0.0f; f.v0 = 0.0f; f.du = W; f.dv = H;
    f.lvl = FxMipLevelFor(st, fminf(cw / W, ch / H));
    st.extraFrame = &f;
    const float pad = f.rHead + 3.0f;
    RECT box = FxClip(st,
        st.ox + (int)floorf(fminf(f.hx - f.hw, fminf(f.tx, f.hx) - pad)) - 2,
        st.oy + (int)floorf(fminf(f.hy - f.hh, fminf(f.ty, f.hy) - pad)) - 2,
        st.ox + (int)ceilf(fmaxf(f.hx + f.hw, fmaxf(f.tx, f.hx) + pad)) + 2,
        st.oy + (int)ceilf(fmaxf(f.hy + f.hh, fmaxf(f.ty, f.hy) + pad)) + 2);
    if (FxIsEmpty(box) || f.alpha <= 0.0f) return FxEmpty();
    st.rx0 = box.left;
    st.rx1 = box.right;
    PoolRun(st.pool, FxJobFluid, &st, box.top, box.bottom, 16, 128);
    return box;
}

static RECT FxRender(FxState& st, float s) {
    s = FxClamp01(s);
    switch (st.fx) {
    case FX_SCALE:  return FxRenderScale(st, s);
    case FX_GLASS:  return FxRenderGlass(st, s);
    case FX_MELT:   return FxRenderMelt(st, s);
    case FX_FLUID:  return FxRenderFluid(st, s);
    case FX_SHATTER:
    case FX_THANOS:
    case FX_POOF:   return FxRenderParticles(st, s);
    case FX_PERLIN: return FxRenderPerlin(st, s);
    case FX_GLITCH: return FxRenderGlitch(st, s);
    case FX_TV_OFF: return FxRenderTvOff(st, s);
    case FX_INK:    return FxRenderInk(st, s);
    case FX_MIRAGE: return FxRenderMirage(st, s);
    case FX_POP:    return FxRenderPop(st, s);
    case FX_FADE_UP: return FxRenderFadeUp(st, s);
    case FX_SWELL:
    default:        return FxRenderSwell(st, s);
    }
}

// One frame: erase what the previous frame drew, then draw this one.
static RECT FxFrame(FxState& st, float s, const RECT& prevBB, bool first) {
    if (!first) {
        if (st.particlesDrawn) {
            PoolRun(st.pool, FxJobParticlesClear, &st, 0, st.nParticles, 2048, 8192);
        } else {
            FxClear(st, prevBB);
        }
    }
    return FxRender(st, s);
}

// Block size and particle grid. Very large windows raise the block size so a
// frame stays under FX_MAX_PARTICLES; small special windows lower it so a
// 200 px bubble still breaks into enough pieces.
static void FxSetupGrid(FxState& st) {
    int block = MacClampInt(g_particleBlock.load(std::memory_order_relaxed), 1, 100);
    if (st.data->special) {
        const int m = MacMax(st.W, st.H);
        if (m < 300) block = MacMin(block, 2);
        else if (m < 800) block = MacMin(block, 3);
    }
    if (st.fx == FX_POOF) block = MacMax(2, MacMin(st.W, st.H) / 120);
    const double count = ((double)st.W / block) * ((double)st.H / block);
    if (count > (double)FX_MAX_PARTICLES) {
        block = (int)ceil(sqrt((double)st.W * (double)st.H / (double)FX_MAX_PARTICLES));
    }
    st.block = MacMax(1, block);
    st.cols = (st.W + st.block - 1) / st.block;
    st.rows = (st.H + st.block - 1) / st.block;
    st.nParticles = st.cols * st.rows;
}

static void FxPrecalc(FxState& st) {
    switch (st.fx) {
    case FX_SHATTER:
    case FX_THANOS:
    case FX_POOF:
        FxSetupGrid(st);
        FxPrecalcParticles(st);
        break;
    case FX_PERLIN:
        FxSetupGrid(st);
        FxPrecalcPerlin(st);
        break;
    case FX_INK:
        FxPrecalcInk(st);
        break;
    case FX_MIRAGE:
        FxPrecalcMirage(st);
        break;
    case FX_GLASS:
        FxPrecalcGlass(st);
        break;
    case FX_MELT:
        FxPrecalcMelt(st);
        break;
    case FX_SCALE:
    case FX_FLUID:
        FxEnsureMips(st);
        break;
    default:
        break;
    }
}

// Canvas margin around the window for the effect's motion. (Aimed particles also
// get the dock and the spread around their flight path, see MacGenieFxThread.)
static void FxPadding(const FxState& st, int* l, int* t, int* r, int* b) {
    const int W = st.W, H = st.H;
    const float k = st.dpi;
    switch (st.fx) {
    case FX_SHATTER: {
        // A shard moves travel * (direction + (pd, pStart)) with |direction| = 1,
        // |pd|, |pStart| <= 0.4 and travel <= 1200 * travelScale: up to 1.57x
        // travel. 3.8 padded 1x travel, so the far shards were cut at a rectangle
        // while still half visible.
        // (+ one block: the grid is set up later; the governor never makes a
        // block bigger than max(setting, 13) px.)
        const int blk = MacMax(16, MacClampInt(g_particleBlock.load(std::memory_order_relaxed), 1, 100));
        const int p = st.aim ? 16 : (int)(1200.0f * 1.5657f * st.travelScale) + blk + 16;
        *l = *t = *r = *b = p;
        break;
    }
    case FX_THANOS:
        // Unaimed dust moves x in [-0.1W, 1.1W] and y in [-0.8H, 1.2H] (wind plus
        // curl at local = 1); 3.8 padded the top by 0.5H only.
        if (st.aim) { *l = *t = *r = *b = 16; }
        else { *l = W / 2 + 8; *r = (int)(W * 1.5f) + 8; *t = (int)(H * 0.85f) + 8; *b = (int)(H * 1.5f) + 8; }
        break;
    case FX_POOF:
        *l = *r = (int)(W * 0.25f) + 16;
        *t = (int)(MacMax(H, (int)(80.0f * k)) * 1.2f) + 16;
        *b = 16;
        break;
    case FX_GLITCH: *l = *r = (int)(W * 0.0625f) + 8; *t = *b = 2; break;
    case FX_TV_OFF: *l = *r = *t = *b = (int)(4.0f * k) + 2; break;
    case FX_SWELL:  *l = *r = (int)(W * 0.1f) + 2; *t = *b = (int)(H * 0.1f) + 2; break;
    case FX_FADE_UP: *l = *r = *b = 2; *t = (int)(10.0f * k) + 2; break;
    case FX_GLASS: {
        // Flight <= ~380 px (dpi, size scaled), a spinning shard's half extent,
        // and up to 1.54 H of fall.
        const float kk = k * MacClampf(sqrtf((float)W * W + (float)H * H) / 1400.0f, 0.35f, 1.0f);
        *l = *r = (int)(420.0f * kk) + W / 3 + 16;
        *t = (int)(170.0f * kk) + H / 4 + 16;
        *b = (int)(1.6f * H) + (int)(420.0f * kk) + 16;
        break;
    }
    case FX_MELT:   *l = *r = *t = 2; *b = (int)(2.2f * H) + 8; break;
    default:        *l = *t = *r = *b = 2; break;
    }
}

DWORD WINAPI MacGenieFxThread(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    const int fx = (data->fx >= FX_SHATTER && data->fx < FX_COUNT) ? data->fx : FX_SWELL;
    MacTelBeginThread(data, kFxInfo[fx].telName, nullptr);

    const int W = data->width;
    const int H = data->height;
    if (!data->pBits || W <= 0 || H <= 0) {
        MacGenieAbortEngine(data);
        return 0;
    }

    FxState st;
    st.data = data;
    st.fx = fx;
    st.src = (const uint32_t*)data->pBits;
    st.W = W;
    st.H = H;
    st.dpi = GetMonitorDpiScale(data->hMon);
    if (!(st.dpi >= 0.5f)) st.dpi = 1.0f;
    const float wcx = (float)data->targetRect.left + W * 0.5f;
    const float wcy = (float)data->targetRect.top + H * 0.5f;
    st.aim = data->aim && (fx == FX_THANOS || fx == FX_SHATTER);
    // V4: Fluid always pours into its dock (taskbar button / popup anchor); Scale
    // when it has one (minimize / restore).
    st.toDock = (fx == FX_FLUID) || (fx == FX_SCALE && data->aim);
    if (st.aim || st.toDock) {
        st.aimX = (float)data->targetDockX - wcx;
        st.aimY = (float)data->targetDockY - wcy;
    }
    st.travelScale = st.dpi * MacClampf((float)MacMax(W, H) / 900.0f, 0.25f, 1.0f);

    // Canvas: the window plus the effect's margin (plus the dock for aimed
    // particles), kept on the monitors involved but always containing the window.
    int padL, padT, padR, padB;
    FxPadding(st, &padL, &padT, &padR, &padB);
    MacShadow shadow;                       // V4
    if (data->shadow) {
        MacShadowInit(&shadow, st.dpi);
        padL = MacMax(padL, shadow.reachSide + 2);
        padR = MacMax(padR, shadow.reachSide + 2);
        padT = MacMax(padT, shadow.reachTop + 2);
        padB = MacMax(padB, shadow.reachBottom + 2);
    }
    int boundLeft   = data->targetRect.left - padL;
    int boundTop    = data->targetRect.top - padT;
    int boundRight  = data->targetRect.right + padR;
    int boundBottom = data->targetRect.bottom + padB;
    if (st.aim || st.toDock) {
        // The stream ends up to ~10 % past the dock and spreads sideways by up
        // to ~20 % of the flight (Shatter: travel x 1.1 plus +-0.15 lateral;
        // Thanos: 0.95 x the flight plus a 0.12 curl): 3.8's fixed 48 px box
        // around the dock clipped the outer grains of a long stream.
        const float flight = sqrtf(st.aimX * st.aimX + st.aimY * st.aimY);
        const int m = (int)(48.0f * st.dpi + 0.25f * flight);
        boundLeft   = MacMin(boundLeft,   data->targetDockX - m);
        boundTop    = MacMin(boundTop,    data->targetDockY - m);
        boundRight  = MacMax(boundRight,  data->targetDockX + m);
        boundBottom = MacMax(boundBottom, data->targetDockY + m);
    }
    {
        RECT monUnion = GetGenieMonitorUnion(data->targetRect, data->hMon);
        if (boundLeft   < monUnion.left)   boundLeft   = monUnion.left;
        if (boundTop    < monUnion.top)    boundTop    = monUnion.top;
        if (boundRight  > monUnion.right)  boundRight  = monUnion.right;
        if (boundBottom > monUnion.bottom) boundBottom = monUnion.bottom;
        if (boundLeft   > data->targetRect.left)   boundLeft   = data->targetRect.left;
        if (boundTop    > data->targetRect.top)    boundTop    = data->targetRect.top;
        if (boundRight  < data->targetRect.right)  boundRight  = data->targetRect.right;
        if (boundBottom < data->targetRect.bottom) boundBottom = data->targetRect.bottom;
    }
    // (Every effect job handles a window that starts left of / above the canvas:
    // st.ox / st.oy may be negative after this.)
    if (!MacGenieClipCanvasToDesktop(boundLeft, boundTop, boundRight, boundBottom)) {
        MacGenieAbortEngine(data);
        return 0;
    }
    st.bw = MacMax(1, boundRight - boundLeft);
    st.bh = MacMax(1, boundBottom - boundTop);
    st.ox = data->targetRect.left - boundLeft;
    st.oy = data->targetRect.top - boundTop;

    HWND hGhost = CreateWindowExW(MacGenieGhostExStyle(data), L"STATIC", NULL, WS_POPUP,
                                  boundLeft, boundTop, st.bw, st.bh, NULL, NULL, NULL, NULL);
    HDC hScreenDC = GetDC(NULL);
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = st.bw;
    bmi.bmiHeader.biHeight = -st.bh;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* pCanvas = nullptr;
    HBITMAP hCanvas = hScreenDC ? CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pCanvas, NULL, 0) : NULL;
    HDC hCanvasDC = (hCanvas && pCanvas) ? CreateCompatibleDC(hScreenDC) : NULL;
    if (!hGhost || !pCanvas || !hCanvasDC) {
        if (hCanvasDC) DeleteDC(hCanvasDC);
        if (hCanvas) DeleteObject(hCanvas);
        if (hScreenDC) ReleaseDC(NULL, hScreenDC);
        if (hGhost) DestroyWindow(hGhost);
        MacGenieAbortEngine(data);
        return 0;
    }
    HBITMAP hOldCanvas = (HBITMAP)SelectObject(hCanvasDC, hCanvas);
    st.dst = (uint32_t*)pCanvas;
    memset(pCanvas, 0, (size_t)st.bw * st.bh * 4);

    // Worker pool for the per-pixel and particle passes on large windows,
    // rationed across processes like the Classic / Black Hole pools.
    int budgetSlots = 0;
    HANDLE hBudget = NULL;
    ParallelPool pool;
    if ((size_t)W * (size_t)H >= 250000) {
        hBudget = MacGenieAcquireBudgetSlot(&budgetSlots);
        if (budgetSlots > 0) PoolInit(&pool, budgetSlots);
    }
    st.pool = &pool;
    // The particle / field / displacement tables are the only large allocations
    // of an effect (up to ~11 MB at 400 000 particles); out of memory ends this
    // animation instead of throwing inside the host application (V3.9).
    bool precalcOk = true;
    try {
        FxPrecalc(st);
    } catch (const std::bad_alloc&) {
        precalcOk = false;
    }
    if (!precalcOk) {
        PoolShutdown(&pool);
        MacGenieReleaseBudgetSlot(hBudget, budgetSlots);
        SelectObject(hCanvasDC, hOldCanvas);
        DeleteDC(hCanvasDC);
        DeleteObject(hCanvas);
        ReleaseDC(NULL, hScreenDC);
        DestroyWindow(hGhost);
        MacGenieAbortEngine(data);
        return 0;
    }

    const double dur = (double)MacMax(50, data->durationMs);
    MacGenieFramePacer framePacer;
    MacGenieCreateFramePacer(&framePacer, data->hMon);
    if (data->telOn) data->tel.intervalMs = framePacer.intervalMs;
    MacAnimClock clock;
    MacAnimClockInit(&clock);
    LARGE_INTEGER qpcNow;
    MacTelSetupDone(data);

    // Aimed particles slide under the taskbar like the genie; the others play
    // above everything (special windows: in their own z-slot).
    const bool underTaskbar = (st.aim || st.toDock) && !data->special;
    RECT prevBB = { 0, 0, st.bw, st.bh };
    RECT prevShadow = FxEmpty();
    BOOL firstFrame = TRUE;
    for (;;) {
        MacGenieDrainMessages();
        QueryPerformanceCounter(&qpcNow);
        const LARGE_INTEGER qpcFrameStart = qpcNow;
        if (!firstFrame) MacAnimApplyReversal(&clock, data, dur, qpcNow.QuadPart);
        const double elapsedMs = MacAnimClockMs(&clock, qpcNow.QuadPart);
        const BOOL lastFrame = (elapsedMs >= dur);
        const float p = MacClampf((float)(elapsedMs / dur), 0.0f, 1.0f);
        const float s = data->isRising ? 1.0f - p : p;

        // V4: last frame's shadow goes first (particle frames only erase their blocks).
        if (!firstFrame && !FxIsEmpty(prevShadow)) FxClear(st, prevShadow);
        st.xfValid = false;
        RECT bb = FxFrame(st, s, prevBB, firstFrame != FALSE);
        RECT shBox = FxEmpty();
        if (shadow.on) {
            // Transforms carry the shadow with the window; the others let it fade
            // over the first quarter of the effect.
            float x0 = (float)st.ox, y0 = (float)st.oy, x1 = (float)(st.ox + W), y1 = (float)(st.oy + H);
            float op = 1.0f - FxSmooth(0.0f, 0.25f, s);
            if (st.xfValid) { x0 = st.xf[0]; y0 = st.xf[1]; x1 = st.xf[2]; y1 = st.xf[3]; op = st.xf[4]; }
            if (op > 0.003f) {
                MacShadowRect(st.dst, st.bw, st.bh, x0, y0, x1, y1, op, shadow, 0, st.bh);
                const RECT b = MacShadowBox(shadow, x0, y0, x1, y1);
                shBox = FxClip(st, b.left, b.top, b.right, b.bottom);
                if (!FxIsEmpty(shBox)) {
                    if (FxIsEmpty(bb)) bb = shBox;
                    else UnionRect(&bb, &bb, &shBox);
                }
            }
        }
        GdiFlush();
        const LONGLONG telRenderEnd = MacTelMark(data);

        RECT dirty = prevBB;
        if (!FxIsEmpty(bb)) {
            if (FxIsEmpty(dirty)) dirty = bb;
            else UnionRect(&dirty, &dirty, &bb);
        }
        if (FxIsEmpty(dirty)) dirty = RECT{ 0, 0, 1, 1 };
        POINT ptDst = { boundLeft, boundTop };
        SIZE sz = { st.bw, st.bh };
        MacGeniePresent(hGhost, hScreenDC, ptDst, sz, hCanvasDC, 255, firstFrame ? nullptr : &dirty);
        prevBB = bb;
        prevShadow = shBox;
        if (firstFrame) {
            MacGenieShowGhostForData(hGhost, data, underTaskbar);
            MacTelFrame0Shown(data);
        }
        MacTelFrame(data, qpcFrameStart.QuadPart, telRenderEnd, MacTelMark(data));

        if (lastFrame) {
            DwmFlush();
            break;
        }
        if (g_unloading.load(std::memory_order_relaxed)) break;
        if (MacGenieSpecialCancelled(data)) break;

        // The first-frame flush is a full vblank wait: it is this frame's pacing.
        bool paced = false;
        if (firstFrame) {
            DwmFlush();
            paced = true;
            firstFrame = FALSE;
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
            MacTelFirstFrame(data);
            MacGeniePacerRebase(&framePacer);
            MacAnimClockStartNow(&clock);   // frame 0 is on screen: time starts now
            MacGenieCloakClosingSource(data);
        }
        if (!paced) MacGeniePaceFrame(&framePacer);
    }
    if (data->telOn) data->tel.strideMax = framePacer.strideMax;

    PoolShutdown(&pool);
    MacGenieReleaseBudgetSlot(hBudget, budgetSlots);
    MacGenieDestroyFramePacer(&framePacer);
    SelectObject(hCanvasDC, hOldCanvas);
    DeleteDC(hCanvasDC);
    DeleteObject(hCanvas);
    ReleaseDC(NULL, hScreenDC);
    MacGenieReleaseSnapshot(data);
    MacGenieRevealBeforeGhostGone(data);
    DestroyWindow(hGhost);

    MacGenieFinishAnimation(data);
    return 0;
}

// The render thread for an effect.
static LPTHREAD_START_ROUTINE MacFxThreadFor(int fx) {
    switch (fx) {
    case FX_GENIE:         return MacGenieAnimThreadClassic;
    case FX_BLACKHOLE:     return MacGenieBlackHoleThread;
    default:               return MacGenieFxThread;
    }
}

// -------------------------------------------------------------------------
// Unified Snapshot & Capture Engine
// -------------------------------------------------------------------------
// Crops a raw capture (rawW x rawH, in window-rect space) into the extended-frame
// snapshot (w x h) whose top-left sits at (offsetX, offsetY) in the source.
//
// This runs on the caller's UI thread at the instant the user clicks minimize, so
// it is the one place in the mod where wasted work is felt as input lag. The
// source sub-rectangle is constant for the whole image, so it is derived once up
// front rather than re-tested four times per pixel: rows outside it collapse to a
// single memset, and inside a row only the left and right margins need clearing.
// The alpha survey rides along in the same pass.
//
// V4: the survey also reports whether a fifth of the window (any of 5 vertical or
// 5 horizontal bands, edges excluded) came back fully transparent: a capture with
// a hole, which a screen grab then replaces when it can (see CaptureNow).
static void MacGenieCropCapture(const DWORD* src, int rawW, int rawH,
                                DWORD* dst, int w, int h,
                                int offsetX, int offsetY, bool forceOpaque,
                                bool* emptyBandOut = nullptr) {
    if (emptyBandOut) *emptyBandOut = false;
    if (!src || !dst || w <= 0 || h <= 0) return;

    // Destination band that maps inside the source bitmap. Both edges are
    // clamped into [0, w] / [0, h]: real windows only ever offset by a few
    // pixels of invisible resize border, but an offset larger than the
    // destination would otherwise run the row fills past the end of a row.
    int x0 = (offsetX < 0) ? -offsetX : 0; if (x0 > w) x0 = w;
    int y0 = (offsetY < 0) ? -offsetY : 0; if (y0 > h) y0 = h;
    int x1 = rawW - offsetX; if (x1 > w) x1 = w; if (x1 < x0) x1 = x0;
    int y1 = rawH - offsetY; if (y1 > h) y1 = h; if (y1 < y0) y1 = y0;

    const size_t rowBytes = (size_t)w * 4;
    if (y0 > 0) memset(dst, 0, (size_t)y0 * rowBytes);
    if (y1 < h) memset(dst + (size_t)y1 * w, 0, (size_t)(h - y1) * rowBytes);

    const int span = x1 - x0;
    size_t inBounds = 0, zeroAlpha = 0;
    // Band survey (V4): 5 column bands and 5 row bands over the inner 94 %.
    const bool survey = emptyBandOut && !forceOpaque && w >= 40 && h >= 40;
    const int mx = w * 3 / 100, my = h * 3 / 100;
    int colEdge[6];
    for (int k = 0; k <= 5; k++) colEdge[k] = mx + (int)((long long)(w - 2 * mx) * k / 5);
    size_t colZero[5] = {}, colTot[5] = {}, rowZero[5] = {}, rowTot[5] = {};

    for (int y = y0; y < y1; y++) {
        DWORD* d = dst + (size_t)y * w;
        if (x0 > 0) memset(d, 0, (size_t)x0 * 4);
        if (x1 < w) memset(d + x1, 0, (size_t)(w - x1) * 4);
        if (span <= 0) continue;
        const DWORD* s = src + (size_t)(y + offsetY) * rawW + (size_t)(x0 + offsetX);
        d += x0;
        if (forceOpaque) {
            for (int i = 0; i < span; i++) d[i] = s[i] | 0xFF000000u;
            continue;
        }
        size_t rowZ = 0;
        for (int i = 0; i < span; i++) {
            const DWORD p = s[i];
            if (p >> 24) {
                d[i] = p;
            } else {
                d[i] = 0;
                rowZ++;
            }
        }
        zeroAlpha += rowZ;
        inBounds += (size_t)span;
        if (survey && y >= my && y < h - my) {
            const int rb = (int)((long long)(y - my) * 5 / MacMax(1, h - 2 * my));
            if (rb >= 0 && rb < 5) { rowZero[rb] += rowZ; rowTot[rb] += (size_t)span; }
            for (int k = 0; k < 5; k++) {
                const int a = MacMax(colEdge[k], x0), b = MacMin(colEdge[k + 1], x1);
                for (int x = a; x < b; x++) colZero[k] += (dst[(size_t)y * w + x] == 0);
                if (b > a) colTot[k] += (size_t)(b - a);
            }
        }
    }
    if (survey) {
        for (int k = 0; k < 5; k++) {
            if ((colTot[k] && colZero[k] * 10 >= colTot[k] * 9) ||
                (rowTot[k] && rowZero[k] * 10 >= rowTot[k] * 9)) {
                *emptyBandOut = true;
            }
        }
    }

    // A capture that came back mostly zero-alpha has no usable alpha channel
    // (PrintWindow renders some windows as plain BGR). Re-read the source for
    // those pixels and make them opaque instead of invisible.
    if (forceOpaque || inBounds == 0 || zeroAlpha * 2 <= inBounds) return;
    for (int y = y0; y < y1; y++) {
        const DWORD* s = src + (size_t)(y + offsetY) * rawW + (size_t)(x0 + offsetX);
        DWORD* d = dst + (size_t)y * w + x0;
        for (int i = 0; i < span; i++) {
            if (!(s[i] >> 24)) d[i] = 0xFF000000u | (s[i] & 0x00FFFFFFu);
        }
    }
}

// V4: the window has a DWM system backdrop (Mica, acrylic, tabbed): its capture
// would have no backdrop.
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
static bool MacGenieHasSystemBackdrop(HWND hWnd) {
    int type = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type))) && type > 1) {
        return true;   // 2 = Mica, 3 = acrylic, 4 = tabbed
    }
    BOOL mica = FALSE;   // Windows 11 21H2: the undocumented DWMWA_MICA_EFFECT
    return SUCCEEDED(DwmGetWindowAttribute(hWnd, 1029, &mica, sizeof(mica))) && mica;
}

// Console hosts render through their own swap chain and PrintWindow tends to
// return an empty client area for them, so a screen grab is preferred. Exact
// process names: the old substring test ("cmd", "terminal") also matched
// cmder, acmdsvc and anything else containing the letters.
static bool ShouldUseBitBlt(HWND hWnd) {
    std::wstring proc = GetProcessNameCached(hWnd);
    static const wchar_t* const kConsoleHosts[] = {
        L"cmd", L"windowsterminal", L"wt", L"conhost", L"openconsole", L"powershell", L"pwsh"
    };
    for (const wchar_t* k : kConsoleHosts) {
        if (proc == k) return true;
    }
    WCHAR name[256];
    if (GetClassNameW(hWnd, name, 256)) {
        if (wcsstr(name, L"ConsoleWindowClass") || wcsstr(name, L"CASCADIA")) return true;
    }
    return false;
}

// Can the window be copied off the screen without baking in something else?
// Walks the z-order above the target (bounded) and refuses when any visible,
// uncloaked, non-fully-transparent window overlaps the capture rect - the
// taskbar excepted. Without this a screen BitBlt during Win+D captured the
// previous window's ghost, and a cloaked target captured the wallpaper.
//
// V3.7 (from ReDrag's Windows Animations 1.3.7):
//  - candidates are compared by their VISIBLE bounds (DWM extended frame), not
//    GetWindowRect, whose ~7 px invisible resize border made every snapped
//    neighbour "overlap" and refuse the grab;
//  - minimized windows are skipped: Chromium / Electron windows stay WS_VISIBLE
//    when minimized and keep their old full-size bounds, but contribute no pixels;
//  - Explorer's transparent XAML island host is skipped: it stays visible with a
//    large bookkeeping rectangle after the surface it hosted has gone clear.
// In a DPI-virtualized thread the extended frame (always physical) cannot be
// compared with `captureRect`, so the window rect is used as before.
static bool MacGenieIsTransparentXamlHost(HWND hWnd, LONG_PTR exStyle) {
    constexpr LONG_PTR need = WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP |
                              WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT;
    if ((exStyle & need) != need) return false;
    WCHAR cls[64] = {};
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
           _wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0;
}

static bool MacCanCaptureWindowFromScreen(HWND hWnd, const RECT& captureRect, HWND hTaskbar) {
    if (!hWnd || !IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    if (MacGenieIsAnyCloaked(hWnd)) return false;
    const bool useExtendedFrame = !MacGenieThreadIsVirtualized();
    HWND candidate = GetWindow(hWnd, GW_HWNDPREV);
    for (int remaining = 512; candidate && remaining > 0; --remaining) {
        const HWND next = GetWindow(candidate, GW_HWNDPREV);
        if (IsWindowVisible(candidate) && !IsIconic(candidate) && candidate != hTaskbar &&
            !MacGenieIsAnyCloaked(candidate)) {
            const LONG_PTR cex = GetWindowLongPtrW(candidate, GWL_EXSTYLE);
            bool fullyTransparent = false;
            if (cex & WS_EX_LAYERED) {
                COLORREF key = 0; BYTE alpha = 255; DWORD flags = 0;
                fullyTransparent = GetLayeredWindowAttributes(candidate, &key, &alpha, &flags) &&
                                   (flags & LWA_ALPHA) && alpha == 0;
            }
            if (!fullyTransparent && !MacGenieIsTransparentXamlHost(candidate, cex)) {
                RECT cr, overlap;
                bool have = false;
                if (useExtendedFrame &&
                    SUCCEEDED(DwmGetWindowAttribute(candidate, DWMWA_EXTENDED_FRAME_BOUNDS, &cr, sizeof(cr)))) {
                    have = true;
                }
                if (!have) have = GetWindowRect(candidate, &cr) != FALSE;
                if (have && IntersectRect(&overlap, &captureRect, &cr)) {
                    return false;
                }
            }
        }
        candidate = next;
    }
    if (candidate) return false;   // walk did not terminate: be conservative
    return true;
}

static bool MacGenieIsExplorerProcess() {
    static const bool isExplorer = [] {
        WCHAR path[MAX_PATH];
        DWORD len = GetModuleFileNameW(NULL, path, MAX_PATH);
        if (!len || len >= MAX_PATH) return false;
        const WCHAR* name = wcsrchr(path, L'\\');
        return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
    }();
    return isExplorer;
}

// Structural filter shared by the hide-stash and the close path: is this a
// window whose close is worth a wormhole? Rejects children, tool windows,
// non-root windows, owned caption-less popups (menus, combo dropdowns,
// tooltips, IME candidate lists), the shell's own surfaces, UWP frames (their
// content is not reachable through PrintWindow), our ghosts, and hung windows
// (PrintWindow on those blocks the caller indefinitely).
//
// closeRequested: the window received a close request (WM_CLOSE / SC_CLOSE: the
// X button, Alt+F4, the taskbar's Close). The splash-screen rule below is then
// skipped - splash screens vanish on their own and never get one, while
// borderless games and launchers the user closes do, and on the never-held
// special path their effect died with the process (V3.8).
static bool MacGenieIsCloseCandidate(HWND hWnd, bool closeRequested = false) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) return false;
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) return false;
    if (GetWindow(hWnd, GW_OWNER) && (style & WS_CAPTION) != WS_CAPTION) return false;
    wchar_t cls[64];
    if (GetClassNameW(hWnd, cls, 64)) {
        static const wchar_t* const kSkip[] = {
            L"STATIC", L"#32768", L"tooltips_class32", L"ComboLBox",
            L"Xaml_WindowedPopupClass", L"Windows.UI.Core.CoreWindow",
            L"ApplicationFrameWindow", L"ForegroundStaging",
            L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"Progman", L"WorkerW",
            L"Windows.UI.Composition.DesktopWindowContentBridge",
            // Explorer's thin taskbar tab proxy for browser windows (it would
            // play a second, sliver-sized wormhole) and its transparent XAML host.
            L"Windows.Internal.Shell.TabProxyWindow", L"XamlExplorerHostIslandWindow"
        };
        for (const wchar_t* k : kSkip) {
            if (wcscmp(cls, k) == 0) return false;
        }
        if (wcsstr(cls, L"Popup") || wcsstr(cls, L"ToolTip") || wcsstr(cls, L"Tooltip")) return false;
        // Inside explorer.exe only File Explorer windows are application windows:
        // the shell's own full-monitor surfaces (lock / power transitions, flyouts,
        // the desktop) look like large unowned windows and would otherwise play a
        // whole-screen wormhole and hold a shell thread for it.
        if (MacGenieIsExplorerProcess() &&
            _wcsicmp(cls, L"CabinetWClass") != 0 && _wcsicmp(cls, L"ExploreWClass") != 0) {
            return false;
        }
    }
    // V3.8: a splash screen (root, unowned, no caption / frame / system menu)
    // is a special window and takes the special-window effect.
    if (!closeRequested && g_specialEnabled.load(std::memory_order_relaxed) && !GetWindow(hWnd, GW_OWNER) &&
        (style & WS_CAPTION) != WS_CAPTION &&
        !(style & (WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX))) {
        return false;
    }
    if (IsHungAppWindow(hWnd)) return false;
    return true;
}

static void MacGeniePendingCloseEraseLocked(std::unordered_map<HWND, MacGeniePendingClose>::iterator it) {
    const size_t bytes = (size_t)it->second.w * it->second.h * 4;
    if (g_PendingCloseBytes >= bytes) g_PendingCloseBytes -= bytes; else g_PendingCloseBytes = 0;
    DeleteObject(it->second.hBmp);
    g_PendingClose.erase(it);
}

static void MacGeniePendingCloseSweepLocked() {
    ULONGLONG now = GetTickCount64();
    for (auto it = g_PendingClose.begin(); it != g_PendingClose.end();) {
        if (now - it->second.tick > PENDING_CLOSE_MAX_AGE_MS || !IsWindow(it->first)) {
            MacGeniePendingCloseEraseLocked(it++);
        } else {
            ++it;
        }
    }
    // Bound the stash: oldest out first.
    while (g_PendingClose.size() > PENDING_CLOSE_MAX_ENTRIES ||
           (g_PendingCloseBytes > PENDING_CLOSE_MAX_BYTES && g_PendingClose.size() > 1)) {
        auto oldest = g_PendingClose.begin();
        for (auto it = g_PendingClose.begin(); it != g_PendingClose.end(); ++it) {
            if (it->second.tick < oldest->second.tick) oldest = it;
        }
        MacGeniePendingCloseEraseLocked(oldest);
    }
}

static void MacGeniePendingCloseErase(HWND hWnd) {
    if (!hWnd) return;
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    auto it = g_PendingClose.find(hWnd);
    if (it != g_PendingClose.end()) MacGeniePendingCloseEraseLocked(it);
}

// Snapshot a window that is about to be hidden, in case the hide turns out to
// be the first half of a close (browsers, Electron apps and tray apps hide
// first and destroy later).
//
// This used to run a full PrintWindow for EVERY hide of every >= 40 px
// top-level window in every process - popup menus, tooltips, combo dropdowns,
// IME windows, splash screens - and stash unbounded bitmaps for 10 s each. It
// is now gated on a real close candidate, on the window's own thread (a
// cross-thread PrintWindow can block on the owner), throttled per window, and
// bounded in entries and bytes.
static bool MacGenieStashPendingClose(HWND hWnd) {
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    if (!g_blackholeClose.load(std::memory_order_relaxed)) return false;
    if (!hWnd || IsIconic(hWnd) || !IsWindowVisible(hWnd)) return false;
    if (!MacGenieIsCloseCandidate(hWnd)) return false;
    if (!MacGenieIsOwnThreadWindow(hWnd)) return false;
    if (!MacGenieShouldAnimate(hWnd)) return false;
    if (MacGenieIsExcluded(hWnd)) return false;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_PendingClose.find(hWnd);
        if (it != g_PendingClose.end() &&
            GetTickCount64() - it->second.tick < PENDING_CLOSE_THROTTLE_MS) {
            return false;   // a fresh stash exists (hide/show flicker)
        }
    }

    RECT winRect;
    if (!GetWindowRect(hWnd, &winRect)) return false;
    RECT rect = winRect, extRect;
    if (MacGenieGetExtendedFrame(hWnd, winRect, &extRect)) {
        rect = extRect;
    }
    int w = rect.right - rect.left, h = rect.bottom - rect.top;
    int offsetX = rect.left - winRect.left, offsetY = rect.top - winRect.top;
    int rawW = winRect.right - winRect.left, rawH = winRect.bottom - winRect.top;
    if (w <= 0 || h <= 0 || rawW <= 0 || rawH <= 0) return false;
    if (!MacGenieSnapshotSizeOk(rect, winRect)) return false;

    HDC hScreenDC = GetDC(NULL);
    if (!hScreenDC) return false;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* pBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hBmp || !pBits) {
        if (hBmp) DeleteObject(hBmp);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    BITMAPINFO bmiRaw = bmi;
    bmiRaw.bmiHeader.biWidth = rawW;
    bmiRaw.bmiHeader.biHeight = -rawH;
    void* pRaw = nullptr;
    HBITMAP hRaw = CreateDIBSection(hScreenDC, &bmiRaw, DIB_RGB_COLORS, &pRaw, NULL, 0);
    HDC hRawDC = hRaw ? CreateCompatibleDC(hScreenDC) : NULL;
    bool ok = false;
    if (hRaw && pRaw && hRawDC) {
        HBITMAP hOldRaw = (HBITMAP)SelectObject(hRawDC, hRaw);
        if (PrintWindow(hWnd, hRawDC, PW_RENDERFULLCONTENT)) {
            GdiFlush();
            MacGenieCropCapture((const DWORD*)pRaw, rawW, rawH, (DWORD*)pBits, w, h,
                                offsetX, offsetY, /*forceOpaque=*/false);
            MacGenieApplyRoundedCornerMask(
                (DWORD*)pBits, w, h,
                MacGenieWindowCornerRadiusPx(hWnd, MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                                             GetWindowLongPtrW(hWnd, GWL_EXSTYLE)));
            ok = true;
        }
        SelectObject(hRawDC, hOldRaw);
    }
    if (hRawDC) DeleteDC(hRawDC);
    if (hRaw) DeleteObject(hRaw);
    ReleaseDC(NULL, hScreenDC);
    if (!ok) {
        DeleteObject(hBmp);
        return false;
    }

    {
        g_cachesTouched.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto old = g_PendingClose.find(hWnd);
        if (old != g_PendingClose.end()) MacGeniePendingCloseEraseLocked(old);
        g_PendingClose[hWnd] = { hBmp, pBits, w, h, GetTickCount64(), rect };
        g_PendingCloseBytes += (size_t)w * h * 4;
        MacGeniePendingCloseSweepLocked();
    }
    return true;
}

// -------------------------------------------------------------------------
// Animation Starter & Dispatcher
// -------------------------------------------------------------------------
bool StartMacGenieAnim(HWND hWnd, BOOL rising, LONG_PTR originalExStyle,
                        BOOL cloakHidden = FALSE,
                        BOOL isClosing = FALSE,
                        MacGeniePendingClose* pendingClose = nullptr,
                        BOOL deferredMinimize = FALSE,
                        BOOL requestedUnhide = FALSE,
                        HWND hNextApp = NULL,
                        HANDLE* outCloseWait = nullptr,
                        int* outDurationMs = nullptr) {
    MacGenieTelemetry tel;
    const bool telOn = g_telemetry.load(std::memory_order_relaxed);
    if (telOn) {
        LARGE_INTEGER f, q;
        QueryPerformanceFrequency(&f);
        QueryPerformanceCounter(&q);
        tel.qpcFreq = f.QuadPart;
        tel.qHookEntry = q.QuadPart;
    }
    if (outCloseWait) *outCloseWait = NULL;
    if (MacGenieIsExcluded(hWnd)) {
        if (!isClosing) {
            if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
            else MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        return false;
    }

    RECT winRect;
    if (!GetWindowRect(hWnd, &winRect)) {
        if (!isClosing) {
            if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
            else MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        return false;
    }
    RECT rect = winRect, extRect;
    if (pendingClose) {
        // The window is already hidden: use the geometry the stash was taken
        // at. Its extended-frame bounds may no longer be readable, and a size
        // mismatch used to trigger a PrintWindow of the hidden window.
        rect = pendingClose->rect;
    } else if (MacGenieGetExtendedFrame(hWnd, winRect, &extRect)) {
        // In this thread's coordinate space and validated against the window
        // rect (see MacGenieGetExtendedFrame): V3.6 took the always-physical
        // DWM value as is, which a DPI-virtualized process cannot use.
        rect = extRect;
    }
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int offsetX = rect.left - winRect.left;
    int offsetY = rect.top - winRect.top;
    int rawW = winRect.right - winRect.left;
    int rawH = winRect.bottom - winRect.top;

    // V3.9: also refused - with the system animation - when the snapshot would be
    // over 64 MP or the window is entirely off screen (MacGenieSnapshotSizeOk).
    if (w <= 0 || h <= 0 || rawW <= 0 || rawH <= 0 || !MacGenieSnapshotSizeOk(rect, winRect)) {
        if (!isClosing) {
            if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
            else MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        return false;
    }

    bool blocked = g_unloading.load(std::memory_order_relaxed);
    bool handedOver = false;
    if (!blocked) {
        g_cachesTouched.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        MacGenieSweepWindowCachesLocked();
        // V4: a restore while this window's minimize (or an earlier restore) is
        // still playing goes to THAT animation: a minimize turns around from where
        // it is, a restore simply continues. The caller has restored the window
        // under a cloak, which the running animation lifts at its end. 3.9 refused
        // the second animation and uncloaked the window at once: it popped in
        // under a ghost that kept falling into the taskbar.
        auto run = (rising && !isClosing && cloakHidden) ? g_AnimData.find(hWnd) : g_AnimData.end();
        if (run != g_AnimData.end() && run->second->reversible) {
            InterlockedExchange(&run->second->wantDir, 1);
            handedOver = true;
        } else if (g_AnimActive.size() >= MAX_CONCURRENT_ANIMS && !g_AnimActive.count(hWnd)) {
            blocked = true;   // too many in flight: this one takes the system animation
        } else {
            blocked = !g_AnimActive.insert(hWnd).second;
            if (!blocked) g_LaunchSeen.insert(hWnd);
        }
    }
    if (handedOver) return true;
    if (blocked) {
        if (!isClosing) {
            if (rising) {
                MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
            } else {
                bool owned;
                {
                    std::lock_guard<std::mutex> lock(g_CacheMutex);
                    owned = g_AnimActive.count(hWnd) != 0;
                }
                if (!owned) MacGenieSetDwmTransitions(hWnd, TRUE);
            }
        }
        return false;
    }

    bool multiMon = g_multiMonitor.load(std::memory_order_relaxed);
    HMONITOR hMon = multiMon
        ? MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0;
        mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    int monWidth = mi.rcMonitor.right - mi.rcMonitor.left;

    // V3.8: which effect plays. A minimize remembers it on the window so the
    // restore plays the same one in reverse.
    const int fxCtx = isClosing ? CTX_CLOSE : (rising ? CTX_RESTORE : CTX_MINIMIZE);
    int fx = MacFxResolve(fxCtx, hWnd);

    DockPos dockPos = DOCK_BOTTOM;
    int learnedTargetX = rect.left + w / 2;
    int learnedTargetY = rect.top + h / 2;
    TaskbarLookup lookup;
    bool haveLookup = false;
    // Genies need the taskbar button; so do particles aimed at it. Everything
    // else plays in place and skips the lookup (and its UI Automation cost).
    bool needDock = kFxInfo[fx].docked ||
                    (!isClosing && (fx == FX_THANOS || fx == FX_SHATTER) &&
                     g_aimAtTaskbar.load(std::memory_order_relaxed)) ||
                    (!isClosing && fx == FX_SCALE);   // V4: Scale always lands on the button
    if (needDock) {
        dockPos = GetDockPosForMonitor(hMon);
        if (dockPos == DOCK_SIDE) {
            // A genie needs a top / bottom dock: on a side taskbar the window
            // plays Swell in place instead of the system animation (3.7).
            if (kFxInfo[fx].docked) fx = FX_SWELL;
            needDock = false;
            dockPos = DOCK_BOTTOM;
        }
    }
    if (fxCtx == CTX_MINIMIZE) MacFxRememberForRestore(hWnd, fx);
    if (needDock) {

        DWORD alignVal = 1, dataSize = sizeof(alignVal);
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &alignVal, &dataSize);
        learnedTargetX = (alignVal == 0) ? (mi.rcMonitor.left + 160)
                                         : (mi.rcMonitor.left + monWidth / 2);
        learnedTargetY = GetDockIconCenterY(hMon, mi.rcMonitor, dockPos);
        const uint64_t sig = MacGenieTaskbarSignature(alignVal);

        // Cursor on this monitor's (visible) taskbar: the user is clicking a
        // taskbar button, and the cursor is the most precise target there is.
        // Cached with a short life like any other resolution - V3 trusted the
        // raw cursor whenever it was outside the work area (over the clock,
        // during a keyboard minimize) and pinned it for the window's lifetime.
        POINT pt = {0, 0};
        RECT trayRect = {0, 0, 0, 0};
        HWND hTrayCur = FindTaskbarForMonitor(hMon);
        const bool cursorOnTray =
            GetCursorPos(&pt) && hTrayCur && GetWindowRect(hTrayCur, &trayRect) &&
            PtInRect(&trayRect, pt) && MacGenieIsTaskbarExpanded(hTrayCur, mi.rcMonitor);

        if (cursorOnTray) {
            learnedTargetX = pt.x;
            learnedTargetY = pt.y;
            g_cachesTouched.store(true, std::memory_order_relaxed);
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            g_IconPositions[hWnd] = { pt, GetTickCount64(), sig };
        } else {
            // Starts the lookup and returns at once; it is awaited (for what is
            // left of its budget) after the snapshot has been taken, so the
            // two overlap instead of adding up on the click.
            lookup = GetTaskbarButtonPos_Begin(hWnd, {learnedTargetX, learnedTargetY}, hMon, sig);
            haveLookup = true;
        }
    }

    // (nothrow: this runs inside the application's own hook call.)
    MacGenieAnimData* data = new (std::nothrow) MacGenieAnimData();
    if (!data) {
        if (lookup.pending) { lookup.pending->Release(); lookup.pending = nullptr; }
        if (!isClosing) {
            if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
            else MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
        return false;
    }
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->hMon = hMon;
    data->width = w;
    data->height = h;
    data->isRising = rising;
    data->targetDockX = learnedTargetX;
    data->targetDockY = learnedTargetY;
    data->dockPos = dockPos;
    data->isClosing = isClosing;
    data->fx = fx;
    data->ctx = fxCtx;
    data->aim = needDock && !kFxInfo[fx].docked;
    data->originalExStyle = originalExStyle;
    data->hiddenByCloak = cloakHidden;
    data->hFirstFrameShown = NULL;
    data->hCloseFinished = NULL;
    data->durationMs = MacFxDurationMs(fx, isClosing ? g_closeDurationMs.load(std::memory_order_relaxed)
                                                     : g_durationMs.load(std::memory_order_relaxed));
    data->bounceStr = g_bounceStr.load(std::memory_order_relaxed);
    data->bounceDurMs = g_bounceDur.load(std::memory_order_relaxed);
    data->bhTwist = g_blackholeTwist.load(std::memory_order_relaxed);
    data->bhQuality = g_blackholeQuality.load(std::memory_order_relaxed);
    data->funnelWidth = g_modernFunnelWidth.load(std::memory_order_relaxed);
    data->requestedUnhide = requestedUnhide;
    data->hNextApp = hNextApp;
    data->unhideDurationMs = g_unhideDurationMs.load(std::memory_order_relaxed);
    data->deferredMinimize = deferredMinimize;
    data->prevBandTop = -1;
    data->prevBandBottom = -2;
    data->prevBandLeft = 0;
    data->prevBandRight = 0;
    data->pBits = nullptr;
    data->hBitmap = NULL;
    data->revealDone = FALSE;
    data->dockOffsetY = g_dockOffsetY.load(std::memory_order_relaxed);
    data->taskbarFocusBorrowed = requestedUnhide;
    data->cacheSnapshotOnFinish = (!rising && !isClosing);
    data->telOn = telOn;
    data->wantDir = -1;
    data->isLaunch = t_macLaunchStart;
    data->shadow = g_windowShadow.load(std::memory_order_relaxed) && MacGenieWindowHasDwmShadow(hWnd);
    // Reversible: every minimize / restore / launch except the deferred minimize
    // of the auto-hide reveal (that window is not minimized until the end).
    data->reversible = !isClosing && !deferredMinimize;
    // The real corner radius of this window in this thread's coordinate space
    // (0 = square). It both rounds a freshly captured snapshot and seeds the
    // Modern engine's silhouette.
    data->cornerRadiusPx = (float)MacGenieWindowCornerRadiusPx(hWnd, hMon, originalExStyle);

    HDC hScreenDC = GetDC(NULL);

    // Every failure after this point unwinds through here.
    auto bail = [&]() -> bool {
        if (hScreenDC) { ReleaseDC(NULL, hScreenDC); hScreenDC = NULL; }
        if (lookup.pending) { lookup.pending->Release(); lookup.pending = nullptr; }
        if (!isClosing) {
            if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
            else MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        if (data->hBitmap) DeleteObject(data->hBitmap);
        delete data;
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
        return false;
    };
    if (!hScreenDC) return bail();

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // A cloaked window (the restore path cloaks before showing; the shell
    // cloaks for other desktops) is not on screen: a screen grab of it is
    // the wallpaper. Never blit the screen for those, nor on any rising path.
    const bool screenAllowed = !rising && !cloakHidden && !MacGenieIsAnyCloaked(hWnd);
    const bool preferScreen =
        ShouldUseBitBlt(hWnd) ||
        // A closing foreground window is on top, so a screen grab IS the window -
        // and at close time Chromium's PrintWindow can return only its title-bar
        // band (found by MrCrustyToast, also done by ReDrag's Windows Animations).
        (isClosing && GetForegroundWindow() == hWnd) ||
        // V4: Mica / acrylic. PrintWindow has no backdrop, so 3.9 animated these
        // see-through (or black, after the zero-alpha fixup); the composed
        // screen is exactly what was on screen (Notepad, Task Manager...).
        (screenAllowed && MacGenieHasSystemBackdrop(hWnd));
    const bool sameThread = MacGenieIsOwnThreadWindow(hWnd);
    HWND hCaptureTaskbar = FindTaskbarForMonitor(hMon);

    auto CopySnapshot = [&](void* srcBits, bool fromScreen, bool* emptyBand) {
        // A screen copy has no meaningful alpha, so it is forced opaque; a
        // PrintWindow result keeps its alpha (with MacGenieCropCapture's
        // "mostly zero alpha -> BGR-only capture" fixup).
        MacGenieCropCapture((const DWORD*)srcBits, rawW, rawH,
                            (DWORD*)data->pBits, w, h,
                            offsetX, offsetY, fromScreen, emptyBand);
        // Both capture paths return the square window surface; DWM rounds the
        // corners at composition. Bake the same rounding into the snapshot so
        // the ghost matches the real window on its very first frame.
        MacGenieApplyRoundedCornerMask((DWORD*)data->pBits, w, h, (int)data->cornerRadiusPx);
    };

    // Returns false when nothing could be captured; the caller then aborts
    // rather than animating an opaque black rectangle (what a zeroed DIB
    // became after the alpha fixup).
    auto CaptureNow = [&]() -> bool {
        HDC hTempDC = CreateCompatibleDC(hScreenDC);
        if (!hTempDC) return false;
        BITMAPINFO bmiTemp = bmi;
        bmiTemp.bmiHeader.biWidth = rawW;
        bmiTemp.bmiHeader.biHeight = -rawH;
        void* pTempBits = nullptr;
        HBITMAP hTempBmp = CreateDIBSection(hScreenDC, &bmiTemp, DIB_RGB_COLORS, &pTempBits, NULL, 0);
        if (!hTempBmp || !pTempBits) {
            if (hTempBmp) DeleteObject(hTempBmp);
            DeleteDC(hTempDC);
            return false;
        }
        HBITMAP hOldTempBmp = (HBITMAP)SelectObject(hTempDC, hTempBmp);

        auto TryScreenBlit = [&]() -> BOOL {
            if (!screenAllowed) return FALSE;
            // Occlusion is judged on the VISIBLE frame (rect), not the window
            // rect with its invisible resize border.
            if (!MacCanCaptureWindowFromScreen(hWnd, rect, hCaptureTaskbar)) return FALSE;
            return BitBlt(hTempDC, 0, 0, rawW, rawH, hScreenDC, winRect.left, winRect.top, SRCCOPY);
        };

        BOOL captured = FALSE;
        bool fromScreen = false;
        if (preferScreen) {
            captured = TryScreenBlit();
            fromScreen = captured != FALSE;
            if (!captured) captured = PrintWindow(hWnd, hTempDC, PW_RENDERFULLCONTENT);
        } else {
            if (rising && sameThread) {
                // Uncached restore: the window was shown (cloaked) a moment
                // ago and may not have painted yet. Same thread only - on a
                // foreign thread this would marshal synchronously.
                RedrawWindow(hWnd, NULL, NULL,
                             RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
            }
            captured = PrintWindow(hWnd, hTempDC, PW_RENDERFULLCONTENT);
            if (!captured) {
                captured = TryScreenBlit();
                fromScreen = captured != FALSE;
            }
        }
        GdiFlush();
        bool emptyBand = false;
        if (captured) CopySnapshot(pTempBits, fromScreen, fromScreen ? nullptr : &emptyBand);
        // V4: a capture with a fully transparent fifth (content PrintWindow could
        // not read: DirectComposition children, a cut frame) would make that part
        // of the window vanish without an animation; the screen has it all.
        // Not for layered windows, whose transparency is real.
        if (captured && emptyBand && !(originalExStyle & WS_EX_LAYERED) && TryScreenBlit()) {
            GdiFlush();
            CopySnapshot(pTempBits, true, nullptr);
        }
        SelectObject(hTempDC, hOldTempBmp);
        DeleteObject(hTempBmp);
        DeleteDC(hTempDC);
        return captured != FALSE;
    };

    auto AllocSnapshotDib = [&]() -> bool {
        data->hBitmap = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &data->pBits, NULL, 0);
        if (!data->hBitmap || !data->pBits) {
            if (data->hBitmap) DeleteObject(data->hBitmap);
            data->hBitmap = NULL;
            data->pBits = nullptr;
            return false;
        }
        return true;
    };

    bool snapshotReady = false;
    if (rising) {
        // Adopt the bitmap the earlier minimize left in the cache: no second
        // allocation and no 30 MB copy under the global lock (V3.6 did both).
        SnapCache cached = {};
        bool haveCached;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            haveCached = MacGenieCacheTakeLocked(hWnd, &cached);
        }
        if (haveCached) {
            if (cached.w == w && cached.h == h && cached.pBits) {
                data->hBitmap = cached.hBmp;
                data->pBits = cached.pBits;
                snapshotReady = true;
            } else {
                DeleteObject(cached.hBmp);     // window was resized: recapture
            }
        }
    }
    if (!snapshotReady) {
        if (!AllocSnapshotDib()) return bail();
        if (rising) {
            snapshotReady = CaptureNow();
        } else if (pendingClose) {
            if (pendingClose->pBits && pendingClose->w == w && pendingClose->h == h) {
                memcpy(data->pBits, pendingClose->pBits, (size_t)w * h * 4);
                snapshotReady = true;
            }
            // Otherwise: never re-capture a hidden window (blank).
        } else {
            snapshotReady = CaptureNow();
        }
    }

    ReleaseDC(NULL, hScreenDC);
    hScreenDC = NULL;

    if (!snapshotReady) return bail();

    // The taskbar lookup ran while the snapshot was taken; collect it now.
    if (haveLookup) {
        double waited = 0.0;
        const POINT dockTarget = GetTaskbarButtonPos_Finish(&lookup, &waited);
        learnedTargetX = dockTarget.x;
        learnedTargetY = dockTarget.y;
        tel.uiaWaitMs = waited;
    }
    if (needDock) {
        // The user's vertical nudge is applied last, on top of whatever
        // resolved the target (cached positions are stored without it).
        learnedTargetY += g_dockOffsetY.load(std::memory_order_relaxed);
    }
    data->targetDockX = learnedTargetX;
    data->targetDockY = learnedTargetY;

    HANDLE hFirstShown = NULL;
    if (!rising && !isClosing) {
        hFirstShown = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (hFirstShown &&
            !DuplicateHandle(GetCurrentProcess(), hFirstShown, GetCurrentProcess(),
                             &data->hFirstFrameShown, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            data->hFirstFrameShown = NULL;
        }
    }
    bool waitForFirstFrame = (data->hFirstFrameShown != NULL);

    HANDLE hCloseWait = NULL;
    if (isClosing && outCloseWait) {
        hCloseWait = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (hCloseWait &&
            !DuplicateHandle(GetCurrentProcess(), hCloseWait, GetCurrentProcess(),
                             &data->hCloseFinished, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            data->hCloseFinished = NULL;
        }
        if (!data->hCloseFinished && hCloseWait) {
            CloseHandle(hCloseWait);
            hCloseWait = NULL;
        }
    }

    LPTHREAD_START_ROUTINE renderProc = MacFxThreadFor(fx);
    // (Modern creates the Direct2D factory lazily on its own thread and falls
    // back to the Classic engine, which needs GDI alone, if that fails.)

    if (telOn) {
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        tel.qCaptureDone = q.QuadPart;
        data->tel = tel;
    }

    // Read before the hand-off: `data` belongs to the render thread afterwards.
    const int effectiveDurationMs = data->durationMs;
    if (data->reversible) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimData[hWnd] = data;
    }
    if (StartWorkerThread(renderProc, data)) {
        if (outDurationMs) *outDurationMs = effectiveDurationMs;
        // `data` belongs to the render thread from here on.
        if (hFirstShown) {
            if (waitForFirstFrame) {
                // Sent messages are still delivered while the ghost is brought
                // up (the render thread, DWM and UI Automation clients may all
                // SendMessage to this thread): V3.6 waited blind.
                const ULONGLONG deadline = GetTickCount64() + 200;
                for (;;) {
                    const ULONGLONG now = GetTickCount64();
                    if (now >= deadline) break;
                    const DWORD r = MsgWaitForMultipleObjectsEx(
                        1, &hFirstShown, (DWORD)(deadline - now), QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
                    if (r != WAIT_OBJECT_0 + 1) break;      // signalled, timed out or failed
                    MSG msg;
                    PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
                }
            }
            CloseHandle(hFirstShown);
        }
        if (outCloseWait) *outCloseWait = hCloseWait;
        return true;
    }

    if (rising) {
        MacGenieUndoRisingHide(hWnd, data->originalExStyle, data->hiddenByCloak);
    } else {
        MacGenieSetDwmTransitions(hWnd, TRUE);
    }
    if (hFirstShown) CloseHandle(hFirstShown);
    if (hCloseWait) CloseHandle(hCloseWait);
    if (data->hCloseFinished) CloseHandle(data->hCloseFinished);
    if (data->hFirstFrameShown) CloseHandle(data->hFirstFrameShown);
    DeleteObject(data->hBitmap);
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_AnimData.find(hWnd);
        if (it != g_AnimData.end() && it->second == data) g_AnimData.erase(it);
        g_AnimActive.erase(hWnd);
    }
    delete data;
    return false;
}

// -------------------------------------------------------------------------
// V3.8: special windows - popups, info bubbles, dropdowns, autocomplete lists,
// flyouts, app-drawn menus, tooltips, notification popups, splash screens, tool
// windows and child windows, at any size.
//
// V1-V3.5 played the close animation on these (their only gates were "not a
// child, not a tool window, at least 40 px"); 3.6's MacGenieIsCloseCandidate
// removed them. They come back on their own path, which never touches the main
// close path (MacGenieTryCloseAnimation) or its GenieCloseAnim guard:
//  - a window that is not a normal application window (MacGenieIsCloseCandidate
//    false) animates with the special effect when it is hidden OR destroyed,
//    whichever comes first (Chromium and WPF reuse popups by hiding them);
//  - a child window only on an explicit DestroyWindow (special.child_hide adds
//    hides): apps hide children constantly, on every tab switch;
//  - the application is never held (V3.5 froze the popup's thread for the
//    whole wormhole): the hide / destroy waits at most ~2 vblanks for the
//    ghost's first frame, then proceeds;
//  - its own guard, "GenieSpecialAnim": a show clears it, and a ghost whose
//    window is shown again stops at once (MacGenieSpecialCancelled);
//  - bounded: 4 ghosts at once and 8 per second per process.
// -------------------------------------------------------------------------
#define SPECIAL_MAX_ACTIVE 4
#define SPECIAL_RATE_PER_SEC 8.0
#define SPECIAL_FIRST_FRAME_WAIT_MS 34
// V4: tiny popups (taskbar tooltips: 96-129 x 52 px in your log) wait at most
// this long - Explorer's taskbar thread spent 10-17 ms per tooltip in 3.9.
#define SPECIAL_TINY_POPUP_HEIGHT 64
#define SPECIAL_TINY_FIRST_FRAME_WAIT_MS 8
#define SPECIAL_MIN_SIDE_PX 8

static std::mutex g_specialRateMutex;
static double g_specialTokens = SPECIAL_RATE_PER_SEC;
static ULONGLONG g_specialTokenTick = 0;

static bool MacGenieSpecialTakeToken() {
    std::lock_guard<std::mutex> lock(g_specialRateMutex);
    const ULONGLONG now = GetTickCount64();
    if (g_specialTokenTick) {
        g_specialTokens += (double)(now - g_specialTokenTick) * SPECIAL_RATE_PER_SEC / 1000.0;
        if (g_specialTokens > SPECIAL_RATE_PER_SEC) g_specialTokens = SPECIAL_RATE_PER_SEC;
    }
    g_specialTokenTick = now;
    if (g_specialTokens < 1.0) return false;
    g_specialTokens -= 1.0;
    return true;
}

static void MacSpecialLog(HWND hWnd, const wchar_t* cls, LONG_PTR style, LONG_PTR ex, const wchar_t* verdict) {
    if (!g_specialLog.load(std::memory_order_relaxed)) return;
    Wh_Log(L"special hwnd=%p class=%s style=%08X ex=%08X: %s", (void*)hWnd, cls ? cls : L"?",
           (unsigned)(style & 0xFFFFFFFF), (unsigned)(ex & 0xFFFFFFFF), verdict);
}

static bool MacClassIn(const wchar_t* cls, const wchar_t* const* list, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (wcscmp(cls, list[i]) == 0) return true;
    }
    return false;
}

// Not windows to a user, or shell surfaces that would misfire.
static const wchar_t* const kSpecialDeny[] = {
    L"STATIC", L"SysShadow", L"IME", L"MSCTFIME UI", L"SysDragImage", L"ForegroundStaging",
    L"#32769", L"Ghost", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", L"Progman", L"WorkerW",
    L"TaskListThumbnailWnd", L"Windows.UI.Core.CoreWindow", L"ApplicationFrameWindow",
    L"XamlExplorerHostIslandWindow", L"Windows.Internal.Shell.TabProxyWindow",
    L"Windows.UI.Composition.DesktopWindowContentBridge", L"MultitaskingViewFrame", L"TaskSwitcherWnd",
};
// Child windows that applications destroy and recreate during normal use.
static const wchar_t* const kSpecialChildDeny[] = {
    L"Chrome_RenderWidgetHostHWND", L"Intermediate D3D Window", L"MozillaCompositorWindowClass",
    L"SHELLDLL_DefView", L"DirectUIHWND", L"CtrlNotifySink",
};
// Inside explorer.exe only these (non-child) special windows animate: the
// shell's own full-monitor surfaces must never play an effect.
static const wchar_t* const kSpecialExplorerAllow[] = {
    L"Xaml_WindowedPopupClass", L"tooltips_class32", L"#32768", L"ComboLBox",
};

static bool MacGenieSpecialClassExcluded(const wchar_t* cls) {
    // (No lock and no string copy for the usual empty list.)
    if (g_specialExcludedCount.load(std::memory_order_relaxed) == 0) return false;
    std::wstring lc = cls;
    std::transform(lc.begin(), lc.end(), lc.begin(), ::towlower);
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (const std::wstring& e : g_SpecialExcludedClasses) {
        if (e == lc) return true;
    }
    return false;
}

static bool MacGenieAllBlack(const DWORD* p, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (p[i] & 0x00FFFFFF) return false;
    }
    return true;
}

// Snapshot ladder. Children: the screen first when their top-level window is
// unobstructed over them (exactly what the user sees), then PrintWindow.
// Popups: PrintWindow first (keeps alpha), validated - UpdateLayeredWindow
// popups often come back black - then the screen. Never a black rectangle.
static bool MacGenieCaptureSpecial(HWND hWnd, HWND root, bool child, const RECT& winRect, const RECT& rect,
                                   HMONITOR hMon, HDC hScreenDC, DWORD* out, int w, int h, bool screenFirst) {
    const int rawW = winRect.right - winRect.left, rawH = winRect.bottom - winRect.top;
    const int offX = rect.left - winRect.left, offY = rect.top - winRect.top;
    if (rawW <= 0 || rawH <= 0) return false;
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = rawW;
    bmi.bmiHeader.biHeight = -rawH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* raw = nullptr;
    HBITMAP hRaw = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &raw, NULL, 0);
    HDC dc = (hRaw && raw) ? CreateCompatibleDC(hScreenDC) : NULL;
    if (!dc) {
        if (hRaw) DeleteObject(hRaw);
        return false;
    }
    HBITMAP hOld = (HBITMAP)SelectObject(dc, hRaw);
    HWND hTray = FindTaskbarForMonitor(hMon);
    auto screen = [&]() -> bool {
        if (!MacCanCaptureWindowFromScreen(root, rect, hTray)) return false;
        if (!BitBlt(dc, 0, 0, rawW, rawH, hScreenDC, winRect.left, winRect.top, SRCCOPY)) return false;
        GdiFlush();
        MacGenieCropCapture((const DWORD*)raw, rawW, rawH, out, w, h, offX, offY, /*forceOpaque=*/true);
        return true;
    };
    auto print = [&]() -> bool {
        memset(raw, 0, (size_t)rawW * rawH * 4);
        if (!PrintWindow(hWnd, dc, PW_RENDERFULLCONTENT)) return false;
        GdiFlush();
        MacGenieCropCapture((const DWORD*)raw, rawW, rawH, out, w, h, offX, offY, /*forceOpaque=*/false);
        return !MacGenieAllBlack(out, (size_t)w * h);
    };
    // Children are drawn into their parent: the screen is what the user sees. A
    // WS_EX_NOREDIRECTIONBITMAP popup (XAML islands: Explorer's taskbar tooltips)
    // has no redirection surface for PrintWindow to read, so 3.9 paid for a failed
    // PrintWindow before every screen grab (V4).
    const bool ok = (child || screenFirst) ? (screen() || print()) : (print() || screen());
    SelectObject(dc, hOld);
    DeleteDC(dc);
    DeleteObject(hRaw);
    return ok;
}

// Genie on a special window: funnel into the point it opened from - the edge
// nearest the cursor when the cursor is close, otherwise the top centre.
static void MacGenieSpecialAnchor(const RECT& r, int* ax, int* ay, DockPos* pos) {
    const int cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    POINT pt = {0, 0};
    bool nearCursor = false;
    if (GetCursorPos(&pt)) {
        const int dx = pt.x < r.left ? r.left - pt.x : (pt.x > r.right ? pt.x - r.right : 0);
        const int dy = pt.y < r.top ? r.top - pt.y : (pt.y > r.bottom ? pt.y - r.bottom : 0);
        nearCursor = dx <= 200 && dy <= 200;
    }
    *ax = nearCursor ? MacClampInt(pt.x, r.left, r.right) : cx;
    if (nearCursor && pt.y > cy) {
        *ay = r.bottom;
        *pos = DOCK_BOTTOM;
    } else {
        *ay = r.top;
        *pos = DOCK_TOP;
    }
}

// Called from every hide / destroy hook before the main path. Returns true when
// a special-window ghost was started (the caller then lets the hide / destroy
// proceed; for AnimateWindow it replaces the system animation).
static bool MacGenieTrySpecialClose(HWND hWnd, bool fromDestroy) {
    if (!hWnd || g_unloading.load(std::memory_order_relaxed) ||
        !g_specialEnabled.load(std::memory_order_relaxed)) {
        return false;
    }
    // Cheap rejects first: these hooks see every hide and destroy in the process.
    const LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    const bool child = (style & WS_CHILD) != 0;
    if (child) {
        if (!g_specialChildWindows.load(std::memory_order_relaxed)) return false;
        if (!fromDestroy && !g_specialChildHide.load(std::memory_order_relaxed)) return false;
    }
    if (!(style & WS_VISIBLE)) return false;
    if (!MacGenieIsOwnThreadWindow(hWnd)) return false;      // capture must not marshal
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    const LONG_PTR ex = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (GetPropW(hWnd, L"UltimateGenieGhost")) return false;

    wchar_t cls[128] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    if (MacClassIn(cls, kSpecialDeny, ARRAYSIZE(kSpecialDeny))) return false;
    // A normal application window takes the main close path.
    if (!child && MacGenieIsCloseCandidate(hWnd)) return false;
    if (!child && (ex & WS_EX_TOOLWINDOW) && !g_specialToolWindows.load(std::memory_order_relaxed)) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, tool windows are off");
        return false;
    }
    if (MacGenieIsExplorerProcess()) {
        const bool allowed = !child && (MacClassIn(cls, kSpecialExplorerAllow, ARRAYSIZE(kSpecialExplorerAllow)) ||
                                        wcsstr(cls, L"Popup") || wcsstr(cls, L"ToolTip") || wcsstr(cls, L"Tooltip"));
        if (!allowed) return false;
    }
    if (child && (MacClassIn(cls, kSpecialChildDeny, ARRAYSIZE(kSpecialChildDeny)) ||
                  wcsncmp(cls, L"Windows.UI.Composition.", 23) == 0)) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, content host child");
        return false;
    }
    if (MacGenieSpecialClassExcluded(cls)) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, excluded class");
        return false;
    }

    // The top-level window it lives in (itself for a popup).
    HWND root = child ? GetAncestor(hWnd, GA_ROOT) : hWnd;
    if (!root || IsIconic(root) || MacGenieIsAnyCloaked(root)) return false;
    if (GetPropW(root, L"GenieCloseAnim")) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, part of a main window's close");
        return false;
    }
    if (!child) {
        HWND owner = GetWindow(hWnd, GW_OWNER);
        // (A hidden owner is fine: tray apps own their notification popups with one.)
        if (owner && GetPropW(owner, L"GenieCloseAnim")) {
            MacSpecialLog(hWnd, cls, style, ex, L"skipped, owner is closing");
            return false;
        }
    }
    if (ex & WS_EX_LAYERED) {
        COLORREF key = 0;
        BYTE alpha = 255;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(hWnd, &key, &alpha, &flags) && (flags & LWA_ALPHA) && alpha == 0) {
            return false;   // already invisible (the app faded it itself)
        }
    }
    const DWORD stamp = (DWORD)(ULONG_PTR)GetPropW(hWnd, L"GenieSpecialAnim");
    if (stamp && (DWORD)GetTickCount64() - stamp < 50) return false;   // re-entry of the same close
    if (!child && IsHungAppWindow(hWnd)) return false;
    if (MacGenieIsExcluded(hWnd)) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, excluded program");
        return false;
    }

    RECT winRect;
    if (!GetWindowRect(hWnd, &winRect)) return false;
    RECT rect = winRect, extRect;
    if (!child && MacGenieGetExtendedFrame(hWnd, winRect, &extRect)) rect = extRect;
    const int w = rect.right - rect.left, h = rect.bottom - rect.top;
    if (w <= 0 || h <= 0) return false;
    if (!MacGenieSnapshotSizeOk(rect, winRect)) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, off screen or over the 64 MP size cap");
        return false;
    }
    const int minSize = g_specialMinSize.load(std::memory_order_relaxed);
    if (minSize > 0 && (w < minSize || h < minSize)) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, smaller than the minimum size");
        return false;
    }
    // V4: never below 8 px a side, whatever the setting says. 3.9 animated a
    // 1 x 1 helper window of Windhawk's own (35 frames of a one-pixel ghost).
    if (w < SPECIAL_MIN_SIDE_PX || h < SPECIAL_MIN_SIDE_PX) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, under 8 px");
        return false;
    }
    if (g_specialActive.load(std::memory_order_relaxed) >= SPECIAL_MAX_ACTIVE || !MacGenieSpecialTakeToken()) {
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, budget (4 at once, 8 per second)");
        return false;
    }

    MacGenieTelemetry tel;
    const bool telOn = g_telemetry.load(std::memory_order_relaxed);
    if (telOn) {
        LARGE_INTEGER f, q;
        QueryPerformanceFrequency(&f);
        QueryPerformanceCounter(&q);
        tel.qpcFreq = f.QuadPart;
        tel.qHookEntry = q.QuadPart;
    }

    HMONITOR hMon = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    HDC hScreenDC = GetDC(NULL);
    if (!hScreenDC) return false;
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    const bool captured = hBmp && bits &&
        MacGenieCaptureSpecial(hWnd, root, child, winRect, rect, hMon, hScreenDC, (DWORD*)bits, w, h,
                               (ex & WS_EX_NOREDIRECTIONBITMAP) != 0);
    ReleaseDC(NULL, hScreenDC);
    if (!captured) {
        if (hBmp) DeleteObject(hBmp);
        MacSpecialLog(hWnd, cls, style, ex, L"skipped, nothing could be captured");
        return false;
    }

    const int fx = MacFxResolve(CTX_SPECIAL, hWnd);
    MacGenieAnimData* data = new (std::nothrow) MacGenieAnimData();
    if (!data) {
        DeleteObject(hBmp);
        return false;
    }
    data->hRealWnd = hWnd;
    data->hBitmap = hBmp;
    data->pBits = bits;
    data->targetRect = rect;
    data->hMon = hMon;
    data->width = w;
    data->height = h;
    data->isRising = FALSE;
    data->isClosing = TRUE;
    data->special = TRUE;
    data->ctx = CTX_SPECIAL;
    data->fx = fx;
    data->originalExStyle = ex;
    data->hiddenByCloak = FALSE;
    data->durationMs = MacFxDurationMs(fx, g_specialDurationMs.load(std::memory_order_relaxed));
    data->bounceStr = 0;
    data->bounceDurMs = 0;
    data->bhTwist = g_blackholeTwist.load(std::memory_order_relaxed);
    data->bhQuality = (MacMax(w, h) < 900) ? 3 : g_blackholeQuality.load(std::memory_order_relaxed);
    data->funnelWidth = MacClampInt(MacMin(g_modernFunnelWidth.load(std::memory_order_relaxed), w / 4), 8, 120);
    data->prevBandTop = -1;
    data->prevBandBottom = -2;
    data->cacheSnapshotOnFinish = FALSE;
    data->cornerRadiusPx = child ? 0.0f : (float)MacGenieWindowCornerRadiusPx(hWnd, hMon, ex);
    data->hInsertAfter = GetWindow(root, GW_HWNDPREV);
    data->ghostTopmost = (GetWindowLongPtrW(root, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    data->telOn = telOn;
    if (kFxInfo[fx].docked) {
        int ax, ay;
        DockPos pos;
        MacGenieSpecialAnchor(rect, &ax, &ay, &pos);
        data->targetDockX = ax;
        data->targetDockY = ay;
        data->dockPos = pos;
    } else {
        data->targetDockX = (rect.left + rect.right) / 2;
        data->targetDockY = (rect.top + rect.bottom) / 2;
        data->dockPos = DOCK_BOTTOM;
    }
    MacGenieApplyRoundedCornerMask((DWORD*)bits, w, h, (int)data->cornerRadiusPx);
    if (telOn) {
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        tel.qCaptureDone = q.QuadPart;
        data->tel = tel;
    }

    HANDLE hFirstShown = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (hFirstShown &&
        !DuplicateHandle(GetCurrentProcess(), hFirstShown, GetCurrentProcess(),
                         &data->hFirstFrameShown, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
        data->hFirstFrameShown = NULL;
    }
    SetPropW(hWnd, L"GenieSpecialAnim", (HANDLE)(ULONG_PTR)((DWORD)GetTickCount64() | 1));
    g_specialActive.fetch_add(1, std::memory_order_relaxed);
    if (!StartWorkerThread(MacFxThreadFor(fx), data)) {
        g_specialActive.fetch_sub(1, std::memory_order_relaxed);
        RemovePropW(hWnd, L"GenieSpecialAnim");
        if (data->hFirstFrameShown) CloseHandle(data->hFirstFrameShown);
        if (hFirstShown) CloseHandle(hFirstShown);
        DeleteObject(hBmp);
        delete data;
        return false;
    }
    // At most ~2 vblanks for the ghost to be on screen before the window goes,
    // pumping sent messages meanwhile; never the whole animation.
    if (hFirstShown) {
        const ULONGLONG deadline = GetTickCount64() +
            (h < SPECIAL_TINY_POPUP_HEIGHT ? SPECIAL_TINY_FIRST_FRAME_WAIT_MS : SPECIAL_FIRST_FRAME_WAIT_MS);
        for (;;) {
            const ULONGLONG now = GetTickCount64();
            if (now >= deadline) break;
            const DWORD r = MsgWaitForMultipleObjectsEx(1, &hFirstShown, (DWORD)(deadline - now),
                                                        QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
            if (r != WAIT_OBJECT_0 + 1) break;
            MSG msg;
            PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
        }
        CloseHandle(hFirstShown);
    }
    MacSpecialLog(hWnd, cls, style, ex, fromDestroy ? L"animated (destroy)" : L"animated (hide)");
    return true;
}

// -------------------------------------------------------------------------
// V3.8: Alt+Tab switch animation (design after ReDrag's Windows Animations 1.3.7)
//
// - The shell's explorer.exe runs a tracker: an out-of-context foreground
//   WinEvent hook. A foreground change while Alt is held starts a session
//   (the window Alt+Tab started from, a new generation) in a small shared
//   mapping, kept fresh every 16 ms while Alt stays down.
// - Every GUI process runs a listener for its own windows, started lazily on
//   its first top-level show. When one of them becomes foreground right after a
//   session (<= 400 ms), is not the source (that is a cancel), and nobody has
//   consumed the generation yet, it plays the switch animation.
// - The animation is a live DWM thumbnail of the target on a ghost window,
//   scaling from switch.start_scale to 100 % while fading in; the target itself
//   is cloaked meanwhile. A minimized target plays its restore effect instead.
// -------------------------------------------------------------------------
#define SWITCH_SHARED_NAME L"Local\\UltimateGenieSwitch_v38"
#define SWITCH_SHARED_MAGIC 0x55475338
#define SWITCH_SESSION_MS 400

struct MacSwitchShared {
    volatile LONG magic;
    volatile LONG generation;
    volatile LONG consumed;
    volatile LONG source;       // HWND Alt+Tab started from (HWNDs are 32-bit significant)
    volatile LONG lastTick;     // refreshed every 16 ms while Alt is held
};

static HANDLE g_hSwitchMap = NULL;
static MacSwitchShared* g_pSwitch = nullptr;
static std::mutex g_switchMapMutex;
static ULONGLONG g_switchMapRetryTick = 0;          // guarded by g_switchMapMutex
static std::atomic<DWORD> g_switchTrackerTid{0};
static std::atomic<DWORD> g_switchListenerTid{0};
static std::atomic<int> g_switchTrackerState{0};    // 0 none, 1 starting, 2 running, 3 not the shell / failed
static std::atomic<int> g_switchListenerState{0};   // 0 none, 1 started, 3 failed
static HWND g_switchLastFg = NULL;                  // tracker thread only
static bool g_switchSessionOn = false;              // tracker thread only
static UINT_PTR g_switchTimer = 0;                  // tracker thread only

static inline LONG MacHwndToLong(HWND h) { return (LONG)(LONG_PTR)h; }

static MacSwitchShared* MacSwitchMap(bool create) {
    std::lock_guard<std::mutex> lock(g_switchMapMutex);
    if (g_pSwitch) return g_pSwitch;
    const ULONGLONG now = GetTickCount64();
    if (!create && now < g_switchMapRetryTick) return nullptr;
    HANDLE h = create
        ? CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(MacSwitchShared), SWITCH_SHARED_NAME)
        : OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, SWITCH_SHARED_NAME);
    if (!h) {
        g_switchMapRetryTick = now + 2000;
        return nullptr;
    }
    void* view = MapViewOfFile(h, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(MacSwitchShared));
    if (!view) {
        CloseHandle(h);
        g_switchMapRetryTick = now + 2000;
        return nullptr;
    }
    g_hSwitchMap = h;
    g_pSwitch = (MacSwitchShared*)view;
    if (create) InterlockedExchange(&g_pSwitch->magic, SWITCH_SHARED_MAGIC);
    return g_pSwitch;
}

static void MacSwitchUnmap() {
    std::lock_guard<std::mutex> lock(g_switchMapMutex);
    if (g_pSwitch) { UnmapViewOfFile((void*)g_pSwitch); g_pSwitch = nullptr; }
    if (g_hSwitchMap) { CloseHandle(g_hSwitchMap); g_hSwitchMap = NULL; }
}

struct MacSwitchData { HWND hWnd; int durationMs; int startScale; };

static DWORD WINAPI MacSwitchAnimThread(LPVOID lp) {
    MacSwitchData* sd = (MacSwitchData*)lp;
    const HWND hWnd = sd->hWnd;
    const double totalMs = (double)MacClampInt(sd->durationMs, 50, 1000);
    const float startScale = (float)MacClampInt(sd->startScale, 80, 99) / 100.0f;
    delete sd;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

    HWND hGhost = NULL;
    HTHUMBNAIL thumb = NULL;
    RECT wr = {};
    bool ok = IsWindow(hWnd) && GetWindowRect(hWnd, &wr) != FALSE;
    const int gw = wr.right - wr.left, gh = wr.bottom - wr.top;
    ok = ok && gw > 0 && gh > 0;
    if (ok) {
        hGhost = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE |
                                     WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT,
                                 L"STATIC", NULL, WS_POPUP, wr.left, wr.top, gw, gh, NULL, NULL, NULL, NULL);
    }
    if (hGhost) {
        SetPropW(hGhost, L"UltimateGenieGhost", (HANDLE)1);
        if (FAILED(DwmRegisterThumbnail(hGhost, hWnd, &thumb))) thumb = NULL;
    }
    if (thumb) {
        // An invisible first frame, committed before the clock starts, so setup
        // latency cannot eat the beginning of the fade.
        DWM_THUMBNAIL_PROPERTIES tp = {};
        tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_RECTDESTINATION;
        tp.fVisible = TRUE;
        tp.opacity = 0;
        tp.rcDestination = { 0, 0, gw, gh };
        if (SUCCEEDED(DwmUpdateThumbnailProperties(thumb, &tp))) {
            ShowWindow_t pfnShow = ShowWindow_Original ? ShowWindow_Original : ShowWindow;
            pfnShow(hGhost, SW_SHOWNOACTIVATE);
            DwmFlush();
            LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
            QueryPerformanceFrequency(&qpcFreq);
            QueryPerformanceCounter(&qpcStart);
            int telFrames = 0;                       // V4: a [perf] line for Alt+Tab too
            double telLastMs = 0.0, telPeriodMax = 0.0;
            for (;;) {
                MacGenieDrainMessages();
                QueryPerformanceCounter(&qpcNow);
                const double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
                if (telFrames++ > 0 && elapsedMs - telLastMs > telPeriodMax) telPeriodMax = elapsedMs - telLastMs;
                telLastMs = elapsedMs;
                const bool last = elapsedMs >= totalMs;
                const float p = last ? 1.0f : (float)(elapsedMs / totalMs);
                const float k = 1.0f - p;
                const float ease = 1.0f - k * k * k;
                const float scale = startScale + (1.0f - startScale) * ease;
                // The destination is the whole window rect scaled about its centre,
                // so the last frame is exactly the real window (no jump on uncloak).
                const int tw = (int)((float)gw * scale + 0.5f), th = (int)((float)gh * scale + 0.5f);
                const int tx = (gw - tw) / 2, ty = (gh - th) / 2;
                tp.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_RECTDESTINATION;
                tp.opacity = (BYTE)(255.0f * ease + 0.5f);
                tp.rcDestination = { tx, ty, tx + tw, ty + th };
                DwmUpdateThumbnailProperties(thumb, &tp);
                if (last || g_unloading.load(std::memory_order_relaxed) || !IsWindow(hWnd)) break;
                DwmFlush();
            }
            if (g_telemetry.load(std::memory_order_relaxed)) {
                Wh_Log(L"[perf] switch alt+tab hwnd=%p src=%dx%d | frames=%d in %.0fms period-max=%.1fms",
                       (void*)hWnd, gw, gh, telFrames, telLastMs, telPeriodMax);
            }
        }
    }
    if (IsWindow(hWnd)) MacGenieSetCloak(hWnd, FALSE);
    DwmFlush();
    if (thumb) DwmUnregisterThumbnail(thumb);
    if (hGhost) (DestroyWindow_Original ? DestroyWindow_Original : DestroyWindow)(hGhost);
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
    }
    return 0;
}

// A window of this process just became foreground: was it picked with Alt+Tab?
static void MacSwitchOnForeground(HWND hWnd) {
    if (!g_switchEnabled.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) return;
    if (!hWnd || GetForegroundWindow() != hWnd) return;
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return;
    if (GetPropW(hWnd, L"UltimateGenieGhost") || GetWindow(hWnd, GW_OWNER)) return;
    // V3.9: a window that is cloaked is already being animated (the restore path
    // cloaks before it un-minimizes, so between the restore and its g_AnimActive
    // entry 3.8 could start a switch fade instead of the restore effect) or is on
    // another virtual desktop.
    if (MacGenieIsAnyCloaked(hWnd)) return;
    if (!MacGenieIsCloseCandidate(hWnd) || MacGenieIsShellCloaked(hWnd)) return;
    {
        // A fullscreen window (game, video) is left alone.
        RECT wr;
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        if (GetWindowRect(hWnd, &wr) &&
            GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &mi) &&
            EqualRect(&wr, &mi.rcMonitor) &&
            (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CAPTION) != WS_CAPTION) {
            return;
        }
    }
    MacSwitchShared* sh = MacSwitchMap(false);
    if (!sh || sh->magic != SWITCH_SHARED_MAGIC) return;
    const LONG gen = sh->generation;
    if (!gen) return;
    if ((LONG)(GetTickCount() - (DWORD)sh->lastTick) > SWITCH_SESSION_MS) return;
    if (sh->source == MacHwndToLong(hWnd)) return;     // back to the source: a cancel
    const LONG prev = sh->consumed;
    if (prev == gen || InterlockedCompareExchange(&sh->consumed, gen, prev) != prev) return;
    if (MacGenieIsExcluded(hWnd)) return;
    {
        g_cachesTouched.store(true, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (!g_AnimActive.insert(hWnd).second) return;
    }
    MacGenieSetCloak(hWnd, TRUE);
    MacSwitchData* sd = new (std::nothrow) MacSwitchData{ hWnd, g_switchDurationMs.load(std::memory_order_relaxed),
                                                          g_switchStartScale.load(std::memory_order_relaxed) };
    if (!sd || !StartWorkerThread(MacSwitchAnimThread, sd)) {
        delete sd;
        MacGenieSetCloak(hWnd, FALSE);
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
    }
}

static bool MacSwitchIsSourceCandidate(HWND h) {
    if (!h || !IsWindow(h) || GetWindow(h, GW_OWNER)) return false;
    if (GetWindowLongPtrW(h, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return false;
    wchar_t cls[64] = {};
    if (!GetClassNameW(h, cls, ARRAYSIZE(cls))) return false;
    if (_wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0 || _wcsicmp(cls, L"MultitaskingViewFrame") == 0 ||
        _wcsicmp(cls, L"TaskSwitcherWnd") == 0 || _wcsicmp(cls, L"ForegroundStaging") == 0) {
        return false;
    }
    return true;
}

static void CALLBACK MacSwitchTrackerProc(HWINEVENTHOOK, DWORD event, HWND hWnd, LONG idObject,
                                          LONG idChild, DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hWnd) return;
    if (g_unloading.load(std::memory_order_relaxed) || !g_switchEnabled.load(std::memory_order_relaxed)) return;
    MacSwitchShared* sh = g_pSwitch;
    const bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    if (sh) {
        if (!alt) {
            if (MacSwitchIsSourceCandidate(hWnd)) g_switchLastFg = hWnd;
        } else if (!g_switchSessionOn) {
            HWND src = g_switchLastFg;
            // V3.9: the source must still be on screen and not closing. Alt+F4 on
            // a hide-to-tray application (or one held by the close effect) moves
            // the foreground while Alt is still down; 3.8 took that for an Alt+Tab
            // and played the switch animation on whatever window got the focus.
            // (Only a YOUNG close guard counts: after a refused close the guard
            // stays on the living window until its next close or show.)
            const DWORD guard = src ? (DWORD)(ULONG_PTR)GetPropW(src, L"GenieCloseAnim") : 0;
            const bool closing = guard && (DWORD)GetTickCount64() - guard < CLOSE_GUARD_REARM_MS;
            if (src && src != hWnd && IsWindow(src) && IsWindowVisible(src) && !IsIconic(src) &&
                !MacGenieIsAnyCloaked(src) && !closing) {
                InterlockedExchange(&sh->source, MacHwndToLong(src));
                if (InterlockedIncrement(&sh->generation) == 0) InterlockedIncrement(&sh->generation);
                InterlockedExchange(&sh->lastTick, (LONG)(GetTickCount() | 1));
                g_switchSessionOn = true;
                if (!g_switchTimer) g_switchTimer = SetTimer(NULL, 0, 16, NULL);
            }
        } else {
            InterlockedExchange(&sh->lastTick, (LONG)(GetTickCount() | 1));
        }
    }
    // File Explorer windows live in this very process.
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) MacSwitchOnForeground(hWnd);
}

static DWORD WINAPI MacSwitchTrackerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);   // create the queue first
    g_switchTrackerTid.store(GetCurrentThreadId(), std::memory_order_release);
    // Wait (bounded) for the shell window: only the shell's explorer tracks.
    bool shell = false;
    for (int i = 0; i < 240 && !g_unloading.load(std::memory_order_relaxed); i++) {
        HWND hShell = GetShellWindow();
        if (hShell) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hShell, &pid);
            shell = (pid == GetCurrentProcessId());
            break;
        }
        Sleep(250);
    }
    if (!shell || g_unloading.load(std::memory_order_relaxed) || !MacSwitchMap(true)) {
        g_switchTrackerState.store(3, std::memory_order_release);
        return 0;
    }
    HWND fg = GetForegroundWindow();
    if (MacSwitchIsSourceCandidate(fg)) g_switchLastFg = fg;
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
                                         MacSwitchTrackerProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!hook) {
        g_switchTrackerState.store(3, std::memory_order_release);
        return 0;
    }
    g_switchTrackerState.store(2, std::memory_order_release);
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_TIMER && !msg.hwnd && g_switchTimer && msg.wParam == g_switchTimer) {
            if ((GetAsyncKeyState(VK_MENU) & 0x8000) && g_pSwitch) {
                InterlockedExchange(&g_pSwitch->lastTick, (LONG)(GetTickCount() | 1));
            } else {
                KillTimer(NULL, g_switchTimer);
                g_switchTimer = 0;
                g_switchSessionOn = false;
            }
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_switchTimer) { KillTimer(NULL, g_switchTimer); g_switchTimer = 0; }
    UnhookWinEvent(hook);
    return 0;
}

static void CALLBACK MacSwitchListenerProc(HWINEVENTHOOK, DWORD event, HWND hWnd, LONG idObject,
                                           LONG idChild, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_FOREGROUND && idObject == OBJID_WINDOW && idChild == CHILDID_SELF && hWnd) {
        MacSwitchOnForeground(hWnd);
    }
}

static DWORD WINAPI MacSwitchListenerThread(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    g_switchListenerTid.store(GetCurrentThreadId(), std::memory_order_release);
    if (g_unloading.load(std::memory_order_relaxed)) return 0;
    HWINEVENTHOOK hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL,
                                         MacSwitchListenerProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    if (!hook) {
        g_switchListenerState.store(3, std::memory_order_release);
        return 0;
    }
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnhookWinEvent(hook);
    return 0;
}

// The shell's explorer tracker is started from Wh_ModInit / a settings change.
static void MacSwitchStartTracker() {
    if (!g_switchEnabled.load(std::memory_order_relaxed) || !MacGenieIsExplorerProcess()) return;
    int expected = 0;
    if (!g_switchTrackerState.compare_exchange_strong(expected, 1)) return;
    if (!StartWorkerThread(MacSwitchTrackerThread, nullptr)) g_switchTrackerState.store(3);
}

// Called from the show hooks: the first top-level window this process shows
// starts its listener (processes that never show a window never get one).
static void MacSwitchEnsureListener() {
    if (g_switchListenerState.load(std::memory_order_relaxed) != 0) return;
    if (!g_switchEnabled.load(std::memory_order_relaxed) || g_unloading.load(std::memory_order_relaxed)) return;
    if (g_switchTrackerState.load(std::memory_order_relaxed) == 2) return;   // the tracker serves this process
    int expected = 0;
    if (!g_switchListenerState.compare_exchange_strong(expected, 1)) return;
    if (!StartWorkerThread(MacSwitchListenerThread, nullptr)) g_switchListenerState.store(3);
}

// Wh_ModInit: an application that was already running when the mod loaded may
// not show a window again for a long time; if it has one on screen, listen now.
struct MacSwitchWindowSearch { DWORD pid; bool found; };
static BOOL CALLBACK MacSwitchFindOwnWindowProc(HWND h, LPARAM lp) {
    MacSwitchWindowSearch* s = (MacSwitchWindowSearch*)lp;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != s->pid || !IsWindowVisible(h)) return TRUE;
    if (GetWindowLongPtrW(h, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    s->found = true;
    return FALSE;
}

static void MacSwitchListenIfWindows() {
    if (!g_switchEnabled.load(std::memory_order_relaxed) || MacGenieIsExplorerProcess()) return;
    MacSwitchWindowSearch s = { GetCurrentProcessId(), false };
    EnumWindows(MacSwitchFindOwnWindowProc, (LPARAM)&s);
    if (s.found) MacSwitchEnsureListener();
}

static void MacSwitchStopThreads() {
    const DWORD t1 = g_switchTrackerTid.load(std::memory_order_acquire);
    if (t1) PostThreadMessageW(t1, WM_QUIT, 0, 0);
    const DWORD t2 = g_switchListenerTid.load(std::memory_order_acquire);
    if (t2) PostThreadMessageW(t2, WM_QUIT, 0, 0);
}

// -------------------------------------------------------------------------
// App-Launch Animation Helpers (Experimental)
// -------------------------------------------------------------------------
static bool MacGenieIsLaunchWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    if (exStyle & WS_EX_NOACTIVATE) return false;
    // An UpdateLayeredWindow window would lose its content when switched to
    // LWA_ALPHA, and a composition-swapchain (WinUI / DirectComposition)
    // window breaks when made layered at all.
    if (exStyle & (WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP)) return false;
    if (!(style & WS_CAPTION)) return false;
    HWND hOwner = GetWindow(hWnd, GW_OWNER);
    if (hOwner && IsWindow(hOwner)) return false;
    HWND hRoot = GetAncestor(hWnd, GA_ROOT);
    if (hRoot && hRoot != hWnd) return false;
    // Same size gate as every other path (dropped in V3: 30 px caption
    // windows got a launch genie).
    return MacGenieShouldAnimate(hWnd);
}

DWORD WINAPI MacGenieLaunchThread(LPVOID lpParam) {
    MacGenieLaunchData* ld = (MacGenieLaunchData*)lpParam;
    HWND hWnd = ld->hWnd;
    LONG_PTR originalExStyle = ld->originalExStyle;
    const BOOL hiddenByCloak = ld->hiddenByCloak;
    delete ld;

    // UWP / packaged apps are cloaked by the shell behind their splash for a
    // while - wait (bounded) for THAT cloak to lift. When the mod cloaked the
    // window itself, its own cloak reports as DWM_CLOAKED_APP and is masked out.
    const UINT ownCloak = hiddenByCloak ? (UINT)DWM_CLOAKED_APP : 0;
    Sleep(60);
    for (int i = 0; i < 30; ++i) {
        if (!IsWindow(hWnd) || g_unloading.load(std::memory_order_relaxed)) break;
        UINT cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) ||
            !(cloaked & ~ownCloak)) {
            break;
        }
        Sleep(50);
    }

    if (g_unloading.load(std::memory_order_relaxed) ||
        !IsWindow(hWnd) || IsIconic(hWnd) || !IsWindowVisible(hWnd)) {
        if (IsWindow(hWnd)) {
            // Undo exactly the hide that was used.
            MacGenieUndoRisingHide(hWnd, originalExStyle, hiddenByCloak);
        }
        return 0;
    }

    t_macLaunchStart = true;
    const bool launched = StartMacGenieAnim(hWnd, TRUE, originalExStyle, hiddenByCloak);
    t_macLaunchStart = false;
    if (launched) {
        // The engine's reveal restores the ex-style; drop the unload record.
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_LaunchLayered.erase(hWnd);
    }
    return 0;
}

// Firefox-family browsers (Firefox, Zen, Floorp... all MozillaWindowClass): is
// hWnd the process' first browser window? Hiding that one with a DWM cloak before
// its first show can cross Gecko's first-window / session-restore boundary and
// make it open a second real window (found and worked around the same way by
// ReDrag's Windows Animations and fork 3.1.7); it keeps the layered hide.
struct MacGenieGeckoSearch { HWND self; DWORD pid; bool found; };

static BOOL CALLBACK MacGenieFindOtherGeckoWindow(HWND w, LPARAM lp) {
    MacGenieGeckoSearch* s = (MacGenieGeckoSearch*)lp;
    if (w == s->self || (!IsWindowVisible(w) && !IsIconic(w)) || GetAncestor(w, GA_ROOT) != w) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    WCHAR cls[64];
    if (pid == s->pid && GetClassNameW(w, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"MozillaWindowClass") == 0) {
        s->found = true;
        return FALSE;
    }
    return TRUE;
}

static bool MacGenieIsFirstGeckoWindow(HWND hWnd) {
    WCHAR cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"MozillaWindowClass") != 0) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    MacGenieGeckoSearch search = { hWnd, pid, false };
    EnumWindows(MacGenieFindOtherGeckoWindow, (LPARAM)&search);
    return !search.found;
}

// Hides the window *before* the real show call so neither the window nor the
// system's open animation appears ahead of the genie. Writes the TRUE pre-hide
// ex-style to *origExOut and which hide was used to *cloakedOut.
//
// The hide is a DWM cloak, not WS_EX_LAYERED + alpha 0 (V3.6): the layered hide
// failed on winit / tao windows (Slint, Tauri), which rewrite GWL_EXSTYLE from
// their own flags right after the show and so drop WS_EX_LAYERED - the window
// appeared and the genie played over it - and Windows strips the bit from flip-
// model swap-chain windows. A cloak is a DWM-level hide that no style rewrite
// touches, and PrintWindow still captures a cloaked window for the snapshot. The
// layered hide stays where a cloak is wrong: the first Gecko window (see above),
// a window the app already cloaked itself (WPF / WinUI white-flash workarounds -
// our uncloak would reveal it early), and a window DWM refuses to cloak.
static bool MacGenieLaunchPrepare(HWND hWnd, int nCmdShow, LONG_PTR* origExOut,
                                  BOOL* cloakedOut) {
    *cloakedOut = FALSE;
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    if (!g_launchAnimation.load(std::memory_order_relaxed)) return false;
    if (nCmdShow != SW_SHOW && nCmdShow != SW_SHOWNORMAL &&
        nCmdShow != SW_SHOWDEFAULT && nCmdShow != SW_SHOWMAXIMIZED) return false;
    if (IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    // Foreign-thread windows: SetWindowLongPtr / SetLayeredWindowAttributes
    // would marshal synchronously to the owner from inside a hook.
    if (!MacGenieIsOwnThreadWindow(hWnd)) return false;
    if (!MacGenieIsLaunchWindow(hWnd)) return false;
    // Excluded programs used to be hidden (alpha 0) here and only rejected in
    // StartMacGenieAnim - an invisible flash on every launch.
    if (MacGenieIsExcluded(hWnd)) return false;
    g_cachesTouched.store(true, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (!g_LaunchSeen.insert(hWnd).second) return false;
    }
    MacGenieSetDwmTransitions(hWnd, FALSE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    *origExOut = exStyle;

    UINT cloakedNow = 0;
    const bool appCloaked =
        SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloakedNow, sizeof(cloakedNow))) &&
        (cloakedNow & DWM_CLOAKED_APP);
    if (!appCloaked && !MacGenieIsFirstGeckoWindow(hWnd)) {
        BOOL cloak = TRUE;
        if (SUCCEEDED(DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak)))) {
            *cloakedOut = TRUE;
            return true;
        }
    }

    // Layered fallback (V3.6 behaviour); recorded so an unload can put it back.
    g_cachesTouched.store(true, std::memory_order_relaxed);
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_LaunchLayered[hWnd] = exStyle;
    }
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    return true;
}

static void MacGenieLaunchCommit(HWND hWnd, LONG_PTR originalExStyle, BOOL cloaked) {
    MacGenieLaunchData* ld = new (std::nothrow) MacGenieLaunchData();
    if (!ld) {
        MacGenieUndoRisingHide(hWnd, originalExStyle, cloaked);
        return;
    }
    ld->hWnd = hWnd;
    ld->originalExStyle = originalExStyle;
    ld->hiddenByCloak = cloaked;
    if (!StartWorkerThread(MacGenieLaunchThread, ld)) {
        delete ld;
        MacGenieUndoRisingHide(hWnd, originalExStyle, cloaked);
    }
}

// Forgets everything cached about a window that is going away.
static void MacGenieForgetWindowLocked(HWND hWnd) {
    MacGenieCacheEraseLocked(hWnd);
    g_IconPositions.erase(hWnd);
    g_LaunchSeen.erase(hWnd);
    g_LaunchLayered.erase(hWnd);
    g_ProcessNameCache.erase(hWnd);
    auto pit = g_PendingClose.find(hWnd);
    if (pit != g_PendingClose.end()) MacGeniePendingCloseEraseLocked(pit);
}

static inline bool MacIsMinimizeShowCmd(int cmd) {
    return cmd == SW_MINIMIZE || cmd == SW_SHOWMINIMIZED ||
           cmd == SW_SHOWMINNOACTIVE || cmd == SW_FORCEMINIMIZE;
}

// Commands that take an iconic window back to the screen. SW_SHOW and
// SW_SHOWNOACTIVATE are deliberately absent: neither restores an iconic
// window, so animating them would play a rising ghost over a window that
// stays minimized.
static inline bool MacIsRestoreShowCmd(int cmd) {
    return cmd == SW_RESTORE || cmd == SW_SHOWNORMAL ||
           cmd == SW_MAXIMIZE || cmd == SW_SHOWMAXIMIZED;
}

// Auto-hide taskbar: should it be revealed for this minimize? Returns true
// only when the bar is really hidden (not already slid in, cursor not over
// it) AND focus could be handed to it. V3 stole focus whenever auto-hide was
// merely enabled - including when the user was clicking the (visible) taskbar
// button - and re-foregrounded another window afterwards regardless of what
// the user did in the meantime.
static bool MacGenieCheckUnhideTaskbar(HWND hWnd, HWND* hNextOut) {
    if (hNextOut) *hNextOut = NULL;
    if (!g_unhideEnabled.load(std::memory_order_relaxed)) return false;

    HMONITOR hMon = g_multiMonitor.load(std::memory_order_relaxed)
        ? MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    HWND hTray = FindTaskbarForMonitor(hMon);
    if (!hTray) return false;

    APPBARDATA abd = {};
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = hTray;
    UINT uState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    if (!(uState & ABS_AUTOHIDE)) return false;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) return false;
    if (MacGenieIsTaskbarExpanded(hTray, mi.rcMonitor)) return false;
    if (MacGenieIsCursorOverTaskbar(hTray)) return false;

    HWND hwndIter = GetWindow(hWnd, GW_HWNDNEXT);
    while (hwndIter) {
        if (IsWindowVisible(hwndIter) && !IsIconic(hwndIter) &&
            GetAncestor(hwndIter, GA_ROOT) == hwndIter &&
            GetWindowTextLengthW(hwndIter) > 0 &&
            hwndIter != hWnd && hwndIter != hTray &&
            !MacGenieIsAnyCloaked(hwndIter)) {
            LONG_PTR exStyle = GetWindowLongPtrW(hwndIter, GWL_EXSTYLE);
            if (!(exStyle & WS_EX_TOOLWINDOW)) {
                if (hNextOut) *hNextOut = hwndIter;
                break;
            }
        }
        hwndIter = GetWindow(hwndIter, GW_HWNDNEXT);
    }
    return SetForegroundWindow(hTray) != FALSE;
}

// Single entry point for every minimize trigger. Returns true when the
// minimize was DEFERRED (auto-hide reveal path: the window is cloaked and the
// worker performs the real minimize once the genie has landed) - the caller
// must then NOT perform the native minimize itself.
static bool MacGenieTryMinimizeAnim(HWND hWnd, bool allowUnhide) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    if (g_cachesTouched.load(std::memory_order_relaxed)) {
        // V4: a minimize while this window's restore (or launch) is still playing
        // turns that animation around from where it is. The caller's native
        // minimize then runs under the restore's hide (DWM transitions are still
        // off), which the reversed animation lifts when it ends. 3.9 let the
        // window vanish while its restore ghost kept rising.
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_AnimData.find(hWnd);
        if (it != g_AnimData.end() && it->second->reversible) {
            InterlockedExchange(&it->second->wantDir, 0);
            return false;
        }
    }
    if (!g_minimizeAnimation.load(std::memory_order_relaxed)) return false;
    // Hidden or already-iconic windows (an app starting with SW_SHOWMINNOACTIVE,
    // a redundant SW_MINIMIZE) have nothing to animate; V3's ShowWindow and
    // SC_MINIMIZE paths lacked this and ghosted a blank capture.
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    if (GetPropW(hWnd, L"GenieBypass")) return false;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_AnimActive.count(hWnd)) return false;   // our own deferred minimize, or a rising anim in flight
    }
    if (!MacGenieShouldAnimate(hWnd)) return false;
    if (MacGenieIsExcluded(hWnd)) return false;
    if (IsHungAppWindow(hWnd)) return false;

    MacGenieSetDwmTransitions(hWnd, FALSE);

    HWND hNext = NULL;
    if (allowUnhide && MacGenieCheckUnhideTaskbar(hWnd, &hNext)) {
        if (StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                              FALSE, FALSE, nullptr, TRUE, TRUE, hNext)) {
            MacGenieSetCloak(hWnd, TRUE);
            return true;
        }
        // The reveal was requested but the animation refused: give focus back.
        HWND hTray = FindTaskbarForMonitor(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST));
        if (hTray && GetForegroundWindow() == hTray) SetForegroundWindow(hWnd);
        MacGenieSetDwmTransitions(hWnd, FALSE);
    }
    StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
    return false;
}

// Restore trigger shared by ShowWindow / SetWindowPlacement / SC_RESTORE.
// Returns true when the window was cloaked and the caller must run the
// original call and then MacGenieCommitRestoreAnim.
static bool MacGenieCanPrepareRestore(HWND hWnd) {
    if (!g_openAnimation.load(std::memory_order_relaxed)) return false;
    if (!hWnd || !IsWindow(hWnd) || !IsIconic(hWnd)) return false;
    if (!MacGenieShouldAnimate(hWnd)) return false;
    // Exclusion is checked BEFORE cloaking: V3 cloaked, showed, and only then
    // let StartMacGenieAnim reject the window - a visible blink.
    if (MacGenieIsExcluded(hWnd)) return false;
    if (IsHungAppWindow(hWnd)) return false;
    return true;
}

// -------------------------------------------------------------------------
// Hooks & Message Interception
// -------------------------------------------------------------------------
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE) {
        if (!MacGenieTrySpecialClose(hWnd, /*fromDestroy=*/false)) MacGenieStashPendingClose(hWnd);
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (MacIsMinimizeShowCmd(nCmdShow)) {
        if (MacGenieTryMinimizeAnim(hWnd, /*allowUnhide=*/true)) {
            return TRUE;   // deferred: the worker minimizes once the genie lands
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    // Any other command shows the window: abandoned close, disarm its guard.
    MacGenieDisarmCloseGuardOnShow(hWnd);
    MacSwitchEnsureListener();

    if (MacIsRestoreShowCmd(nCmdShow) && IsIconic(hWnd)) {
        if (MacGenieCanPrepareRestore(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            MacGenieSetCloak(hWnd, TRUE);
            BOOL res = ShowWindow_Original(hWnd, nCmdShow);
            StartMacGenieAnim(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return res;
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    LONG_PTR launchOrigEx;
    BOOL launchCloaked;
    if (MacGenieLaunchPrepare(hWnd, nCmdShow, &launchOrigEx, &launchCloaked)) {
        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        MacGenieLaunchCommit(hWnd, launchOrigEx, launchCloaked);
        return res;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

// Close-path tracing. Close animations fire at most a few times a minute, so the
// logging is free in practice, and it is the only way to tell from the outside
// which gate rejected a close. Windhawk only surfaces these when mod logging is
// switched on.
#define MAC_CLOSE_LOG(reason) \
    do { if (g_closeLog.load(std::memory_order_relaxed)) Wh_Log(L"close hwnd=%p: " reason, (void*)hWnd); } while (0)

// Hold the closing thread until the wormhole has played out.
//
// This is what makes the close animation deterministic. Previously the mod
// started the animation and immediately let the close proceed, so the window was
// destroyed - and, when it was the application's last window, the whole process
// torn down, taking the animation thread with it - while the wormhole was still
// on its opening frames. The animation was racing the application's own shutdown
// and usually lost; it only ever appeared to work for applications that happened
// to linger. The window is already cloaked by the animation's first frame, so
// nothing is on screen during this wait except the wormhole itself.
//
// The budget is bounded so a wedged animation can never stop a window closing.
// V3.9: it is the effect's REAL length (the close duration setting scaled by the
// effect's pacing, see MacFxDurationMs) plus slack. 3.8 used the bare setting, so
// Thanos Snap (2.6x), Shatter / Perlin (2.4x), Ink (2x) ... were cut off by the
// process exit whenever the setting was long enough for the difference to pass
// the 750 ms slack.
//
// The wait pumps SENT messages (MsgWaitForMultipleObjectsEx + QS_SENDMESSAGE):
// this thread owns the window, and anything that SendMessage()s it during the
// hold - DWM, UI Automation clients, the mod's own animation thread - would
// otherwise deadlock until the budget ran out. (V3.5 had to special-case one
// such caller, the alpha fade's WM_STYLECHANGING; this removes the class.)
static void MacGenieHoldForCloseAnimation(HANDLE hWait, int effectDurationMs) {
    if (!hWait) return;
    if (effectDurationMs <= 0) effectDurationMs = g_closeDurationMs.load(std::memory_order_relaxed);
    const DWORD budget = (DWORD)MacClampInt(effectDurationMs, 50, 5000) + 750;
    const ULONGLONG deadline = GetTickCount64() + budget;
    for (;;) {
        const ULONGLONG now = GetTickCount64();
        if (now >= deadline) break;
        const DWORD r = MsgWaitForMultipleObjectsEx(1, &hWait, (DWORD)(deadline - now),
                                                    QS_SENDMESSAGE, MWMO_INPUTAVAILABLE);
        if (r != WAIT_OBJECT_0 + 1) break;   // signalled, timed out, or failed
        MSG msg;
        PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
    }
    CloseHandle(hWait);
}

// Should the closing thread be held for the animation at all? Only when the
// ghost would otherwise die with it: the window belongs to THIS thread and it
// is the process's last visible top-level window, so the close will tear the
// process (and the worker rendering the ghost) down. V3.5 held every trigger,
// including Explorer's cross-process WM_CLOSE from a taskbar thumbnail, which
// froze the taskbar for the animation, and a multi-window app's UI thread for
// a close its process survives anyway. (This is V1's predicate, restored.)
struct MacGenieLastWindowCtx { HWND self; DWORD pid; bool found; };

static BOOL CALLBACK MacGenieLastWindowEnumProc(HWND h, LPARAM lp) {
    MacGenieLastWindowCtx* c = (MacGenieLastWindowCtx*)lp;
    if (h == c->self || !IsWindowVisible(h)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != c->pid) return TRUE;
    if (GetWindowLongPtrW(h, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindow(h, GW_OWNER)) return TRUE;
    c->found = true;
    return FALSE;
}

static bool MacGenieCloseShouldHold(HWND hWnd) {
    if (!MacGenieIsOwnThreadWindow(hWnd)) return false;
    MacGenieLastWindowCtx ctx = { hWnd, GetCurrentProcessId(), false };
    EnumWindows(MacGenieLastWindowEnumProc, (LPARAM)&ctx);
    return !ctx.found;
}

// Guard accessors. The property carries the tick of the last close animation;
// nonzero is forced with |1 (a <=1ms error against a 1500ms window).
static void MacGenieStampCloseGuard(HWND hWnd) {
    DWORD stamp = (DWORD)GetTickCount64() | 1;
    SetPropW(hWnd, L"GenieCloseAnim", (HANDLE)(ULONG_PTR)stamp);
}

static DWORD MacGenieCloseGuardAgeMs(HWND hWnd) {
    DWORD stamp = (DWORD)(ULONG_PTR)GetPropW(hWnd, L"GenieCloseAnim");
    if (!stamp) return MAXDWORD;
    return (DWORD)GetTickCount64() - stamp;   // unsigned math survives wrap
}

// Proof of life: a window being shown again is positive evidence that the
// earlier close was abandoned (a tray window re-opened, a refused close whose
// app re-presents the window), so the next close is a genuinely new one and
// deserves its own animation.
// Only a window that is currently HIDDEN being shown counts as proof of life.
// A redundant SW_SHOW on an already-visible window (frameworks do this during
// shutdown) used to disarm the guard and let the later DestroyWindow replay
// the wormhole. A window in one of our own animations is never disarmed.
static void MacGenieDisarmCloseGuardOnShow(HWND hWnd) {
    // V3.8: a special window shown again stops its ghost (MacGenieSpecialCancelled).
    if (hWnd && GetPropW(hWnd, L"GenieSpecialAnim")) RemovePropW(hWnd, L"GenieSpecialAnim");
    if (!hWnd || !GetPropW(hWnd, L"GenieCloseAnim")) return;
    if (IsWindowVisible(hWnd)) return;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_AnimActive.count(hWnd)) return;
    }
    RemovePropW(hWnd, L"GenieCloseAnim");
    // The earlier close may have cloaked it; a re-shown window must be seen.
    MacGenieSetCloak(hWnd, FALSE);
    MacGenieSetDwmTransitions(hWnd, TRUE);
}

// Immediate uncloak, for the one case that is unambiguous: DestroyWindow
// itself failed, so the window stays and must be visible.
static void MacGenieUncloakCloseSurvivor(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    if (MacGenieCloseGuardAgeMs(hWnd) == MAXDWORD) return;   // we never cloaked it
    MacGenieSetCloak(hWnd, FALSE);
    MacGenieSetDwmTransitions(hWnd, TRUE);
}

// Survivor watchdog. After a close animation the real window is cloaked. Two
// very different things can follow that look identical from the hook:
//   - the close was refused (a "save changes?" prompt the user cancelled), or
//   - the application closes asynchronously: it returned from WM_CLOSE with
//     the window still up and will hide/destroy it a moment later (Electron
//     apps, browsers, most modern frameworks).
// V3.5 uncloaked synchronously as soon as the close message returned, which
// made every asynchronous closer pop back on screen for a beat before its
// deferred destroy - and, for a modal "save?" prompt, only uncloaked AFTER the
// prompt was dismissed (the message does not return until then), so the user
// saw the dialog over an invisible main window.
//
// This worker instead watches the window: it uncloaks the moment the window is
// disabled by a modal prompt (refused close), stays quiet while the window is
// on its way out (hidden / destroyed), and gives up and uncloaks a window that
// is still plainly visible after a generous horizon, so nothing can stay
// invisible forever.
#define CLOSE_SURVIVOR_HORIZON_MS 3000

static DWORD WINAPI MacGenieCloseSurvivorThread(LPVOID lp) {
    HWND hWnd = (HWND)lp;
    const ULONGLONG deadline = GetTickCount64() + CLOSE_SURVIVOR_HORIZON_MS;
    for (;;) {
        if (g_unloading.load(std::memory_order_relaxed)) return 0;
        if (!IsWindow(hWnd)) return 0;                                  // closed
        if (MacGenieCloseGuardAgeMs(hWnd) == MAXDWORD) return 0;        // disarmed by a show
        if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return 0;        // hiding: close in flight
        if (!IsWindowEnabled(hWnd)) break;                              // modal prompt: refused
        HWND hPopup = GetWindow(hWnd, GW_ENABLEDPOPUP);
        if (hPopup && hPopup != hWnd && IsWindowVisible(hPopup)) break; // owned prompt: refused
        if (GetTickCount64() >= deadline) break;
        Sleep(25);
    }
    if (IsWindow(hWnd) && IsWindowVisible(hWnd)) {
        MacGenieSetCloak(hWnd, FALSE);
        MacGenieSetDwmTransitions(hWnd, TRUE);
    }
    return 0;
}

static void MacGenieStartCloseSurvivorWatch(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    if (!StartWorkerThread(MacGenieCloseSurvivorThread, (void*)hWnd)) {
        MacGenieUncloakCloseSurvivor(hWnd);
    }
}

static void MacGenieTryCloseAnimation(HWND hWnd, bool fromDestroy = false) {
    if (g_unloading.load(std::memory_order_relaxed)) return;
    if (!g_blackholeClose.load(std::memory_order_relaxed)) return;
    if (!hWnd || !IsWindow(hWnd)) return;
    // Cheap rejects first: DestroyWindow runs this for every child control.
    if (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD) return;
    // V3.9: a close message is an explicit close request, which a splash screen
    // never gets - a borderless window the user closes keeps the main path (see
    // MacGenieIsCloseCandidate). A bare DestroyWindow still applies the rule.
    if (!MacGenieIsCloseCandidate(hWnd, /*closeRequested=*/!fromDestroy)) {
        MAC_CLOSE_LOG(L"skipped, not a close candidate");
        return;
    }
    if (MacGenieIsExcluded(hWnd)) {
        MAC_CLOSE_LOG(L"skipped, excluded program");
        return;
    }

    // One close, one animation - this single guard gates BOTH routes below.
    //
    // A single close commonly reaches this function several times: on the close
    // message while the window is still up, again from DestroyWindow once the
    // application gets around to it, and possibly from more than one process
    // (Explorer sends WM_CLOSE cross-process). Every trigger consults the same
    // window property, so whichever fired first wins and the rest fall through
    // here.
    const DWORD guardAge = MacGenieCloseGuardAgeMs(hWnd);
    const bool windowHidden = !IsWindowVisible(hWnd) || IsIconic(hWnd);
    if (guardAge != MAXDWORD) {
        // Arm-once semantics: while the guard exists, every further trigger is
        // the SAME close still working its way out, however long that takes.
        // No fixed age can separate a slow shutdown from a new close - Electron
        // apps routinely keep their window visible for seconds after the
        // wormhole while they save state, and their final DestroyWindow used to
        // outlive the old 1.5s cooldown and replay the animation. The guard is
        // instead dropped on proof of life: the window being SHOWN again
        // disarms it immediately (tray re-open, restore), and a long visible
        // horizon below re-arms after a refused close ("save changes?" ->
        // Cancel), the one case that never produces a show event.
        if (windowHidden) {
            // A guard that survived a hide/show cycle the hooks did not see
            // (a show path that is neither ShowWindow nor SetWindowPos) would
            // otherwise suppress every future close of this window. A fresh
            // stash next to a stale guard is a new close.
            bool freshStash = false;
            if (guardAge > CLOSE_GUARD_REARM_MS) {
                std::lock_guard<std::mutex> lock(g_CacheMutex);
                auto it = g_PendingClose.find(hWnd);
                freshStash = (it != g_PendingClose.end() &&
                              GetTickCount64() - it->second.tick < PENDING_CLOSE_MAX_AGE_MS);
            }
            if (!freshStash) {
                MAC_CLOSE_LOG(L"skipped, already animated - window hidden, close still in flight");
                return;
            }
            RemovePropW(hWnd, L"GenieCloseAnim");
        } else {
        // A DestroyWindow trigger is by definition the tail end of a close, not
        // a user starting a new one - the re-arm horizon never applies to it.
        // (A refused-then-reclosed window always re-triggers via WM_CLOSE or
        // SC_CLOSE, never via a bare DestroyWindow.)
        if (fromDestroy) {
            MAC_CLOSE_LOG(L"skipped, already animated - destroy of the same close");
            return;
        }
        if (guardAge < CLOSE_GUARD_REARM_MS) {
            MAC_CLOSE_LOG(L"skipped, already animated for this close");
            return;
        }
        RemovePropW(hWnd, L"GenieCloseAnim");
        // Visible well past the horizon: the earlier close was refused and the
        // user is closing again - a genuinely new close, animate it.
        }
    }

    if (windowHidden) {
        MacGeniePendingClose pending = {};
        bool havePending = false;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            MacGeniePendingCloseSweepLocked();
            auto it = g_PendingClose.find(hWnd);
            if (it != g_PendingClose.end()) {
                pending = it->second;
                const size_t bytes = (size_t)pending.w * pending.h * 4;
                if (g_PendingCloseBytes >= bytes) g_PendingCloseBytes -= bytes; else g_PendingCloseBytes = 0;
                g_PendingClose.erase(it);
                havePending = true;
            }
        }
        if (!havePending) {
            MAC_CLOSE_LOG(L"skipped, hidden/iconic with no stashed snapshot");
            return;
        }
        // Stamp before starting so a concurrent trigger from another process
        // already sees the guard while the animation is being set up.
        MacGenieStampCloseGuard(hWnd);
        HANDLE hWait = NULL;
        HANDLE* pWait = MacGenieCloseShouldHold(hWnd) ? &hWait : nullptr;
        int effectMs = 0;
        bool started = StartMacGenieAnim(hWnd, FALSE,
            GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
            FALSE, TRUE, &pending, FALSE, FALSE, NULL, pWait, &effectMs);
        DeleteObject(pending.hBmp);
        if (started) {
            MAC_CLOSE_LOG(L"started from stashed snapshot");
            MacGenieHoldForCloseAnimation(hWait, effectMs);
            // Re-stamp now that the hold is over: the cooldown should time the
            // application's shutdown from the end of the animation, not from
            // before it started.
            if (IsWindow(hWnd)) MacGenieStampCloseGuard(hWnd);
        } else {
            MAC_CLOSE_LOG(L"StartMacGenieAnim refused the stashed snapshot");
            RemovePropW(hWnd, L"GenieCloseAnim");
        }
        return;
    }

    if (!MacGenieShouldAnimate(hWnd)) {
        MAC_CLOSE_LOG(L"skipped, MacGenieShouldAnimate said no");
        return;
    }

    MacGenieStampCloseGuard(hWnd);
    HANDLE hWait = NULL;
    HANDLE* pWait = MacGenieCloseShouldHold(hWnd) ? &hWait : nullptr;
    int effectMs = 0;
    if (StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                          FALSE, TRUE, nullptr, FALSE, FALSE, NULL, pWait, &effectMs)) {
        if (pWait) MAC_CLOSE_LOG(L"started (holding the closing thread)");
        else MAC_CLOSE_LOG(L"started");
        MacGenieHoldForCloseAnimation(hWait, effectMs);
        // Re-stamp now that the hold is over: the cooldown times the
        // application's shutdown from the end of the animation.
        if (IsWindow(hWnd)) MacGenieStampCloseGuard(hWnd);
        // The window is (or is about to be) cloaked by the wormhole's first
        // frame; the watchdog decides when a survivor gets to be seen again.
        MacGenieStartCloseSurvivorWatch(hWnd);
    } else {
        MAC_CLOSE_LOG(L"StartMacGenieAnim refused to start");
        RemovePropW(hWnd, L"GenieCloseAnim");
    }
}

static inline bool MacIsCloseMessage(UINT Msg, WPARAM wParam) {
    return Msg == WM_CLOSE || (Msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE);
}

static LRESULT WINAPI SendMessageW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (MacIsCloseMessage(Msg, wParam)) MacGenieTryCloseAnimation(hWnd);
    return SendMessageW_Original(hWnd, Msg, wParam, lParam);
}

// ANSI twin: old Win32 applications (Delphi, VB6, ANSI MFC) close through here.
static LRESULT WINAPI SendMessageA_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (MacIsCloseMessage(Msg, wParam)) MacGenieTryCloseAnimation(hWnd);
    return SendMessageA_Original(hWnd, Msg, wParam, lParam);
}

BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    // V4: our own ghost windows (destroyed at the end of every animation) skip
    // the close paths: 3.9 classified and logged every one of them.
    if (hWnd && GetPropW(hWnd, L"UltimateGenieGhost")) return DestroyWindow_Original(hWnd);
    // Child controls are destroyed by the thousand and are never animated, never
    // stashed and never cached: they skip everything below. (V3.6 took the
    // global cache mutex and ran six map erases for every one of them.)
    const bool topLevel = hWnd && !(GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD);
    // DestroyWindow only succeeds from the window's own thread; a foreign
    // caller would just get FALSE, so do not capture (cross-thread
    // PrintWindow) or hold for it.
    // V3.8: special windows (and child windows) first; a normal application
    // window is rejected there at once and takes the main close path.
    if (hWnd && !MacGenieTrySpecialClose(hWnd, /*fromDestroy=*/true) &&
        topLevel && MacGenieIsOwnThreadWindow(hWnd)) {
        MacGenieTryCloseAnimation(hWnd, /*fromDestroy=*/true);
    }
    BOOL res = DestroyWindow_Original(hWnd);
    if (!res) {
        // Destroy practically never fails, but if it does the window must not
        // be left cloaked by the animation that just ran.
        if (topLevel) MacGenieUncloakCloseSurvivor(hWnd);
    } else if (topLevel && g_cachesTouched.load(std::memory_order_relaxed)) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        MacGenieForgetWindowLocked(hWnd);
    }
    return res;
}

// Shared body of the DefWindowProcW / DefWindowProcA hooks (the messages the
// mod cares about carry no string parameters, so the two are identical apart
// from the original they forward to).
//
// Must return LRESULT, not BOOL: the hook stands in for DefWindowProc, whose
// result is pointer-sized. Returning BOOL truncated every DefWindowProcW result
// to 32 bits for every window in every hooked process (fixed in 3.5).
static LRESULT MacGenieDefWindowProcCommon(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam,
                                           DefWindowProcW_t original) {
    // WM_NCDESTROY too: plenty of windows handle WM_DESTROY themselves and
    // never pass it on, but WM_NCDESTROY almost always reaches DefWindowProc.
    if (Msg == WM_DESTROY || Msg == WM_NCDESTROY) {
        // Only top-level windows ever carry the close-guard property or a cache
        // entry; and a process that never animated anything has no entries at
        // all, so it never even takes the mutex.
        if (!(GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD)) {
            RemovePropW(hWnd, L"GenieCloseAnim");
            if (GetPropW(hWnd, L"GenieFx")) RemovePropW(hWnd, L"GenieFx");
            if (GetPropW(hWnd, L"GenieSpecialAnim")) RemovePropW(hWnd, L"GenieSpecialAnim");
            if (g_cachesTouched.load(std::memory_order_relaxed)) {
                std::lock_guard<std::mutex> lock(g_CacheMutex);
                MacGenieForgetWindowLocked(hWnd);
            }
        }
    }

    // A hidden window being shown is proof of life for the close guard (tray
    // re-open, restore): the next close is a genuinely new one. WM_SHOWWINDOW
    // arrives before the window turns visible, so MacGenieDisarmCloseGuardOnShow's
    // "currently hidden" test holds. Catches show paths that are neither
    // ShowWindow nor SetWindowPos (fork 3.2.0).
    if (Msg == WM_SHOWWINDOW && wParam) {
        MacGenieDisarmCloseGuardOnShow(hWnd);
    }

    if (MacIsCloseMessage(Msg, wParam)) {
        MacGenieTryCloseAnimation(hWnd);
        return original(hWnd, Msg, wParam, lParam);
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            if (MacGenieTryMinimizeAnim(hWnd, /*allowUnhide=*/true)) {
                return 0;   // deferred (auto-hide reveal): the worker minimizes
            }
            return original(hWnd, Msg, wParam, lParam);
        }
        if ((cmd == SC_RESTORE || cmd == SC_MAXIMIZE) && IsIconic(hWnd) &&
            MacGenieCanPrepareRestore(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            MacGenieSetCloak(hWnd, TRUE);
            LRESULT res = original(hWnd, Msg, wParam, lParam);
            StartMacGenieAnim(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return res;
        }
    }
    return original(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    return MacGenieDefWindowProcCommon(hWnd, Msg, wParam, lParam, DefWindowProcW_Original);
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    return MacGenieDefWindowProcCommon(hWnd, Msg, wParam, lParam, DefWindowProcA_Original);
}

BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE) {
        if (!MacGenieTrySpecialClose(hWnd, /*fromDestroy=*/false)) MacGenieStashPendingClose(hWnd);
        return ShowWindowAsync_Original(hWnd, nCmdShow);
    }
    if (MacIsMinimizeShowCmd(nCmdShow)) {
        // Asynchronous: the deferred (unhide) variant cannot be honored here,
        // so the plain genie plays and the native minimize proceeds.
        MacGenieTryMinimizeAnim(hWnd, /*allowUnhide=*/false);
    } else {
        MacGenieDisarmCloseGuardOnShow(hWnd);
        LONG_PTR launchOrigEx;
        BOOL launchCloaked;
        if (MacGenieLaunchPrepare(hWnd, nCmdShow, &launchOrigEx, &launchCloaked)) {
            BOOL res = ShowWindowAsync_Original(hWnd, nCmdShow);
            MacGenieLaunchCommit(hWnd, launchOrigEx, launchCloaked);
            return res;
        }
    }
    return ShowWindowAsync_Original(hWnd, nCmdShow);
}

BOOL WINAPI SetWindowPlacement_Hook(HWND hWnd, const WINDOWPLACEMENT* lpwndpl) {
    if (!lpwndpl) return SetWindowPlacement_Original(hWnd, lpwndpl);
    const UINT showCmd = lpwndpl->showCmd;
    if (showCmd == SW_HIDE) {
        if (!MacGenieTrySpecialClose(hWnd, /*fromDestroy=*/false)) MacGenieStashPendingClose(hWnd);
    } else if (MacIsMinimizeShowCmd((int)showCmd)) {
        MacGenieTryMinimizeAnim(hWnd, /*allowUnhide=*/false);
    } else if (MacIsRestoreShowCmd((int)showCmd) && IsIconic(hWnd)) {
        MacGenieDisarmCloseGuardOnShow(hWnd);
        if (MacGenieCanPrepareRestore(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            MacGenieSetCloak(hWnd, TRUE);
            BOOL res = SetWindowPlacement_Original(hWnd, lpwndpl);
            StartMacGenieAnim(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return res;
        }
    }
    return SetWindowPlacement_Original(hWnd, lpwndpl);
}

BOOL WINAPI CloseWindow_Hook(HWND hWnd) {
    MacGenieTryMinimizeAnim(hWnd, /*allowUnhide=*/false);
    return CloseWindow_Original(hWnd);
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    if (uFlags & SWP_HIDEWINDOW) {
        if (!MacGenieTrySpecialClose(hWnd, /*fromDestroy=*/false)) MacGenieStashPendingClose(hWnd);
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
    }
    if (uFlags & SWP_SHOWWINDOW) {
        MacSwitchEnsureListener();
        if (g_cachesTouched.load(std::memory_order_relaxed)) MacGeniePendingCloseErase(hWnd);
        MacGenieDisarmCloseGuardOnShow(hWnd);
        LONG_PTR launchOrigEx;
        BOOL launchCloaked;
        if (MacGenieLaunchPrepare(hWnd, SW_SHOW, &launchOrigEx, &launchCloaked)) {
            BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
            MacGenieLaunchCommit(hWnd, launchOrigEx, launchCloaked);
            return res;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// AnimateWindow(AW_HIDE) is another way applications hide a window before a
// close (or to the tray); stash a snapshot like the other hide paths.
static BOOL WINAPI AnimateWindow_Hook(HWND hWnd, DWORD dwTime, DWORD dwFlags) {
    if (dwFlags & AW_HIDE) {
        if (MacGenieTrySpecialClose(hWnd, /*fromDestroy=*/false)) {
            // V3.8: the special effect replaces the system's own fade / slide
            // (comctl32 tooltips) instead of playing on top of it.
            ShowWindow_t pfnShow = ShowWindow_Original ? ShowWindow_Original : ShowWindow;
            pfnShow(hWnd, SW_HIDE);
            return TRUE;
        }
        MacGenieStashPendingClose(hWnd);
    }
    return AnimateWindow_Original(hWnd, dwTime, dwFlags);
}

// -------------------------------------------------------------------------
// Windhawk Mod Lifecycle Entry Points
// -------------------------------------------------------------------------
// Processes that can never own a window this mod animates. The mod is injected
// into every process, so each of these skips all eleven hooks and the settings
// load (ReDrag's Windows Animations 1.3.7 does the same):
//  - session 0 (services are isolated from the interactive desktop);
//  - Chromium / Electron helper roles (renderer, gpu-process, utility, crashpad,
//    ...): those reuse the application's executable but never own its top-level
//    window. The browser / main process has no --type= role and stays enabled.
static bool MacGenieIsNonInteractiveSession() {
    DWORD sessionId = 0;
    return ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0;
}

static bool MacGenieIsNonWindowHostingHelper() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) return false;
    bool helper = false;
    for (int i = 1; i < argc && !helper; ++i) {
        const wchar_t* argument = argv[i];
        const wchar_t* role = nullptr;
        if (_wcsnicmp(argument, L"--type=", 7) == 0) {
            role = argument + 7;
        } else if (_wcsicmp(argument, L"--type") == 0 && i + 1 < argc) {
            role = argv[++i];
        }
        if (role) {
            static const wchar_t* const kRoles[] = {
                L"renderer", L"gpu-process", L"utility", L"crashpad-handler",
                L"plugin", L"ppapi", L"zygote", L"nacl-loader", L"nacl-broker",
            };
            for (const wchar_t* r : kRoles) {
                if (_wcsicmp(role, r) == 0) { helper = true; break; }
            }
        }
        if (_wcsicmp(argument, L"--node-ipc") == 0 ||
            _wcsicmp(argument, L"--headless") == 0 ||
            _wcsicmp(argument, L"--headless=new") == 0 ||
            _wcsicmp(argument, L"--embedded-browser-webview=1") == 0 ||
            _wcsicmp(argument, L"--embedded-browser-edgeview=1") == 0) {
            helper = true;
        }
    }
    LocalFree(argv);
    return helper;
}

BOOL Wh_ModInit() {
    if (MacGenieIsNonInteractiveSession() || MacGenieIsNonWindowHostingHelper()) {
        return FALSE;
    }
    MacGenieLoadSettings();
    MacGenieResolveDpiApis();

    // Dynamically load GetDpiForMonitor once
    g_hShcore = LoadLibraryExW(L"Shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_hShcore) {
        g_pfnGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(g_hShcore, "GetDpiForMonitor");
    }
    // (V4 uses no Direct2D, Direct3D or DirectComposition at all: every engine
    // renders on the CPU with GDI presentation.)

    struct HookSpec { const wchar_t* name; void* target; void* hook; void** original; };
    const HookSpec hooks[] = {
        { L"DefWindowProcW",     (void*)DefWindowProcW,     (void*)DefWindowProcW_Hook,     (void**)&DefWindowProcW_Original },
        { L"DefWindowProcA",     (void*)DefWindowProcA,     (void*)DefWindowProcA_Hook,     (void**)&DefWindowProcA_Original },
        { L"ShowWindow",         (void*)ShowWindow,         (void*)ShowWindow_Hook,         (void**)&ShowWindow_Original },
        { L"ShowWindowAsync",    (void*)ShowWindowAsync,    (void*)ShowWindowAsync_Hook,    (void**)&ShowWindowAsync_Original },
        { L"SetWindowPlacement", (void*)SetWindowPlacement, (void*)SetWindowPlacement_Hook, (void**)&SetWindowPlacement_Original },
        { L"CloseWindow",        (void*)CloseWindow,        (void*)CloseWindow_Hook,        (void**)&CloseWindow_Original },
        { L"SetWindowPos",       (void*)SetWindowPos,       (void*)SetWindowPos_Hook,       (void**)&SetWindowPos_Original },
        { L"SendMessageW",       (void*)SendMessageW,       (void*)SendMessageW_Hook,       (void**)&SendMessageW_Original },
        { L"SendMessageA",       (void*)SendMessageA,       (void*)SendMessageA_Hook,       (void**)&SendMessageA_Original },
        { L"DestroyWindow",      (void*)DestroyWindow,      (void*)DestroyWindow_Hook,      (void**)&DestroyWindow_Original },
        { L"AnimateWindow",      (void*)AnimateWindow,      (void*)AnimateWindow_Hook,      (void**)&AnimateWindow_Original },
    };
    for (const HookSpec& h : hooks) {
        if (!Wh_SetFunctionHook(h.target, h.hook, h.original)) {
            Wh_Log(L"Failed to hook %s", h.name);
        }
    }
    // Alt+Tab: the shell's explorer.exe tracks sessions (no-op elsewhere); an
    // application already on screen starts listening for its windows now.
    MacSwitchStartTracker();
    MacSwitchListenIfWindows();
    return TRUE;
}

void Wh_ModSettingsChanged() {
    MacGenieLoadSettings();
    MacSwitchStartTracker();
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_relaxed);
    MacSwitchStopThreads();     // their message loops end on WM_QUIT
    JoinWorkerThreads();
}

void Wh_ModUninit() {
    if (g_hShcore) {
        FreeLibrary(g_hShcore);
        g_hShcore = nullptr;
        g_pfnGetDpiForMonitor = nullptr;
    }

    // If a worker did not join in time it may still be inside Direct2D; a
    // leaked factory beats a use-after-free in the host process.
    if (!g_joinTimedOut) {
        MacSwitchUnmap();
    }

    // Safety sweep: uncloak and restore DWM transitions for any window that was in-flight
    std::vector<HWND> activeHwnds;
    std::vector<std::pair<HWND, LONG_PTR>> launchLayered;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        for (HWND hWnd : g_AnimActive) {
            activeHwnds.push_back(hWnd);
        }
        g_AnimActive.clear();
        for (auto& pair : g_LaunchLayered) launchLayered.push_back(pair);
        g_LaunchLayered.clear();
    }
    for (HWND hWnd : activeHwnds) {
        if (IsWindow(hWnd)) {
            MacGenieSetCloak(hWnd, FALSE);
            MacGenieSetDwmTransitions(hWnd, TRUE);
            RemovePropW(hWnd, L"GenieCloseAnim");
            RemovePropW(hWnd, L"GenieBypass");
        }
    }
    // Windows the launch path had made layered/transparent: put them back
    // (V3 forced alpha 255 but left WS_EX_LAYERED on them).
    for (auto& pair : launchLayered) {
        if (IsWindow(pair.first)) {
            SetLayeredWindowAttributes(pair.first, 0, 255, LWA_ALPHA);
            if (!(pair.second & WS_EX_LAYERED)) {
                LONG_PTR cur = GetWindowLongPtrW(pair.first, GWL_EXSTYLE);
                if (cur & WS_EX_LAYERED) SetWindowLongPtrW(pair.first, GWL_EXSTYLE, cur & ~WS_EX_LAYERED);
            }
            MacGenieSetDwmTransitions(pair.first, TRUE);
        }
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second.hBmp);
    }
    g_SnapshotCache.clear();
    g_SnapshotOrder.clear();
    g_SnapshotBytes = 0;
    g_IconPositions.clear();
    g_ProcessIconPositions.clear();
    g_ProcessUiaNegativeUntil.clear();
    g_ProcessNameCache.clear();
    g_LaunchSeen.clear();
    for (auto& pair : g_PendingClose) DeleteObject(pair.second.hBmp);
    g_PendingClose.clear();
    g_PendingCloseBytes = 0;
}
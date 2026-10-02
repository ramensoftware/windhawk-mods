// ==WindhawkMod==
// @id              cursor-trail
// @name            Cursor trail
// @description     A fully customizable cursor trail overlay for the Windows desktop.
// @version         1.0
// @author          Ulrizza
// @github          https://github.com/Ulrizza
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -lole32 -lgdi32 -lshell32 -lwindowscodecs -lwinmm -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WindHawk - Cursor Trail

A fully customizable cursor trail for the Windows desktop.  

## Intro

I always liked the original Windows cursor trail, but it was too limited, so I made this one. I personally enjoy it very much so I want to share it 😊

If you see any bug or want more settings/features, add an issue on the [Github repo](https://github.com/Ulrizza/WindHawk-CursorTrail)!  
I cannot promise you anything because I don't have much free time but I enjoyed making this plugin so I'll do my best 🤘

**Disclamer:** I used AI to make this project, I just wanted a custom cursor trail and now I have it so I'm happy. If you are botherded by that, just don't install it.

## Installation

1. Install [Windhawk](https://windhawk.net/).
2. Install **Cursor trail** from the Windhawk mods catalog, or import the source
   (`CursorTrail.cpp`) from this repository via the Windhawk mod editor.
3. On Windows 11, also install the companion **Cursor trail helper - always on top**
   mod so the trail draws above the taskbar and Start menu.

## The two styles

- **Simple line**: a polyline that follows the cursor; its width, color, and
  opacity can change from head to tail.
- **Cursor ghost**: faded copies of the cursor image, each latched at the spot
  where it spawned (they stay put and only fade out). Each copy keeps the exact
  cursor image from when it was sampled, so an image change (e.g. arrow to
  I-beam) appears gradually along the trail.

## Features

- **Time based** vs **Size based** trails.
- **Timing**: tail duration and inactivity timeout.
- **Size**: stroke width (Simple line) and copy size (Cursor ghost).
- **Color**: head-to-tail gradients, blend width, and ghost
  recoloring.
- **Hotkey**: toggle the trail on/off with a customizable global hotkey.

## Settings

### Style

Type of trail.

`style: simple_line`: a line whose width, color, and opacity can change along its length  
![style = simple_line](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/style.simple_line.gif)

`style: cursor_ghost`: faded copies of the cursor image, each latched where it spawned  
![style = cursor_ghost](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/style.cursor_ghost.gif)

### Simple line options

Applies when Style is Simple line.

#### Trail mode

How the trail disappears.

`simpleLineOptions.trail_mode: time_based`: each part of the trail fades after the Tail duration  
![simpleLineOptions.trail_mode = time_based](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.trail_mode.time_based.gif)

`simpleLineOptions.trail_mode: size_based`: keeps a fixed trail length even when the cursor stops  
![simpleLineOptions.trail_mode = size_based](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.trail_mode.size_based.gif)

#### Time based

Applies when Trail mode is Time based.

##### Tail duration

How long each trail segment stays visible, in milliseconds. Minimum 20.

`100`  
![simpleLineOptions.timeBased.tail_duration = 100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.timeBased.tail_duration.100.gif)

`300`  
![simpleLineOptions.timeBased.tail_duration = 300](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.timeBased.tail_duration.300.gif)

`1000`  
![simpleLineOptions.timeBased.tail_duration = 1000](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.timeBased.tail_duration.1000.gif)

#### Size based

Applies when Trail mode is Size based.

##### Tail length

Maximum trail length in pixels. Minimum 20.

`100`  
![simpleLineOptions.sizeBased.tail_size = 100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.sizeBased.tail_size.100.gif)

`500`  
![simpleLineOptions.sizeBased.tail_size = 500](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.sizeBased.tail_size.500.gif)

`1000`  
![simpleLineOptions.sizeBased.tail_size = 1000](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.sizeBased.tail_size.1000.gif)

##### Timeout

Milliseconds of inactivity before the trail starts fading (using the Time based tail duration). 0 = trail always visible.

`0`  
![simpleLineOptions.sizeBased.timeout = 0](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.sizeBased.timeout.0.gif)

`500`  
![simpleLineOptions.sizeBased.timeout = 500](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.sizeBased.timeout.500.gif)

`1000`  
![simpleLineOptions.sizeBased.timeout = 1000](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.sizeBased.timeout.1000.gif)

#### Width

##### Values (head to tail)

Comma-separated stroke widths in pixels from head to tail (e.g. "2,1" for a tapered trail, or "10,1,10,1" for a pulsing trail). Each value gets an equal share; repeat to widen (e.g. "2,2,2,2,1" = 80% at 2, 20% at 1). Minimum 1.

`5`  
![simpleLineOptions.width.values = 5](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.width.values.5.gif)

`5,0`  
![simpleLineOptions.width.values = 5,0](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.width.values.5.0.gif)

`1,1,1,10`  
![simpleLineOptions.width.values = 1,1,1,10](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.width.values.1.1.1.10.gif)

`10,1,10,1,10,1`  
![simpleLineOptions.width.values = 10,1,10,1,10,1](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.width.values.10.1.10.1.10.1.gif)

#### Color

##### Values (head to tail)

Single hex (RRGGBB without #, e.g. 000000 for black) or comma-separated list for a gradient from head to tail (e.g. 000000,FF0000,FFFFFF for black->red->white). Each color gets an equal share; repeat to widen. Invalid entries fall back to black.

`FF0000`  
![simpleLineOptions.color.values = FF0000](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.values.ff0000.gif)

`FF0000,FF7F00,FFFF00,7FFF00,00FF00,00FFFF,0000FF,4B0082,8B00FF`  
![simpleLineOptions.color.values = FF0000,FF7F00,FFFF00,7FFF00,00FF00,00FFFF,0000FF,4B0082,8B00FF](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.values.rainbow.gif)

##### Blend width

Percentage of each transition spent blending (0 = pure bands, 100 = full gradient). 50 with red,blue gives 25% hard red, 50% blend, 25% hard blue. Only applies with two or more colors.

`0`  
![simpleLineOptions.color.blend_width = 0](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.blend_width.0.png)  
![simpleLineOptions.color.blend_width = 0](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.blend_width.0.gif)  

`50`  
![simpleLineOptions.color.blend_width = 50](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.blend_width.50.png)  
![simpleLineOptions.color.blend_width = 50](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.blend_width.50.gif)  

`100`  
![simpleLineOptions.color.blend_width = 100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.blend_width.100.png)  
![simpleLineOptions.color.blend_width = 100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.color.blend_width.100.gif)  

#### Opacity

##### Values (head to tail)

Comma-separated opacity percentages (0-100) from head to tail (e.g. "100,0" for full fade, or "100,0,100" for a pulse). Each value gets an equal share; repeat to widen. Leave one value for uniform opacity.

`100`  
![simpleLineOptions.opacity.values = 100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.opacity.values.100.gif)

`100,0`  
![simpleLineOptions.opacity.values = 100,0](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.opacity.values.100.0.gif)

`100,0,100`  
![simpleLineOptions.opacity.values = 100,0,100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.opacity.values.100.0.100.gif)

#### Antialiasing

Smooth the trail edges.

`on`  
![simpleLineOptions.antialiasing = on](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.antialiasing.on.gif)

`off`  
![simpleLineOptions.antialiasing = off](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.antialiasing.off.gif)

### Cursor ghost options

Applies when Style is Cursor ghost.

#### Trail mode

How the copies disappear.

`ghostOptions.trail_mode: time_based`: each copy fades after the Tail duration  
![ghostOptions.trail_mode = time_based](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.trail_mode.time_based.gif)

`ghostOptions.trail_mode: size_based`: keeps a fixed number of copies even when the cursor stops  
![ghostOptions.trail_mode = size_based](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.trail_mode.size_based.gif)

#### Time based

Applies when Trail mode is Time based.

##### Tail duration

How long each cursor copy stays visible, in milliseconds. Minimum 20.

`100`  
![ghostOptions.timeBased.tail_duration = 100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.timeBased.tail_duration.100.gif)

`300`  
![ghostOptions.timeBased.tail_duration = 300](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.timeBased.tail_duration.300.gif)

`1000`  
![ghostOptions.timeBased.tail_duration = 1000](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.timeBased.tail_duration.1000.gif)

#### Size based

Applies when Trail mode is Size based.

##### Copies

Number of cursor copies in the trail (Size based mode). Minimum 2, maximum 512.

`5`  
![ghostOptions.sizeBased.tail_size = 5](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.sizeBased.tail_size.5.gif)

`20`  
![ghostOptions.sizeBased.tail_size = 20](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.sizeBased.tail_size.20.gif)

`50`  
![ghostOptions.sizeBased.tail_size = 50](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.sizeBased.tail_size.50.gif)

##### Timeout

Milliseconds of inactivity before the trail starts fading (using the Time based tail duration). 0 = trail always visible.

`500`  
![ghostOptions.sizeBased.timeout = 500](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.sizeBased.timeout.500.gif)
 
#### Copy spacing

Extra distance in pixels added between cursor copies (0 = automatic, based on the copy count). Maximum 200.

`10`  
![ghostOptions.spacing = 10](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.spacing.10.gif)

`25`  
![ghostOptions.spacing = 25](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.spacing.25.gif)

#### Size

##### Values (head to tail)

Comma-separated size multipliers from head to tail (1 = same size, 0.8 = 80%, 2 = twice). Each value gets an equal share; repeat to widen (e.g. "1,0.5,1,0.5"). Avoid values above 1 (upscaled copies look pixelated); use the Windows cursor size setting to enlarge the cursor.

`1`  
![ghostOptions.size.values = 1](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.size.values.1.png)

`1,0`  
![ghostOptions.size.values = 1,0](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.size.values.1.0.png)

`1,0.2,1,0.2`  
![ghostOptions.size.values = 1,0.2,1,0.2](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.size.values.1.02.1.02.png)

#### Opacity

##### Values (head to tail)

Comma-separated opacity percentages (0-100) from head to tail (e.g. "100,0" for full fade). Each value gets an equal share; repeat to widen.

`100,0`  
![ghostOptions.opacity.values = 100,0](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.opacity.values.100.0.png)

`100,20,100,20`  
![ghostOptions.opacity.values = 100,20,100,20](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.opacity.values.100.20.100.20.png)

#### Color

##### Values (head to tail)

Leave empty to keep the cursor's own colors. Otherwise a single hex (RRGGBB without #) or comma-separated list for a gradient from head to tail; pixels matching the Replace color are recolored to this value (FFFFFF makes white copies). Each color gets an equal share; repeat to widen. Invalid entries fall back to black. A single color disables Blend width.

`(empty)`  
![ghostOptions.color.values = empty](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.values.empty.black.gif)  
![ghostOptions.color.values = empty](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.values.empty.pink.gif)  
![ghostOptions.color.values = empty](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.values.empty.green.gif)  

`ff0000`  
![ghostOptions.color.values = ff0000](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.values.ff0000.gif)

`ff00ff,00ffff,ffff00`  
![ghostOptions.color.values = ff00ff,00ffff,ffff00](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.values.ff00ff.00ffff.ffff00.gif)

##### Blend width

Percentage of each transition spent blending (0 = pure bands, 100 = full gradient). Only applies with two or more colors. (same principle as for the trail)

##### Replace

Only used when Color > Values is set.

###### Mode

Which cursor pixels to recolor.

`ghostOptions.color.replace.mode: auto`: the cursor's enclosed center color (ignoring the outline/contour; falls back to the largest area when nothing is enclosed)  
![ghostOptions.color.replace.mode = auto](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.replace.mode.auto.gif)

`ghostOptions.color.replace.mode: whole`: every non-transparent pixel  
![ghostOptions.color.replace.mode = whole](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.replace.mode.whole.gif)

`ghostOptions.color.replace.mode: custom`: the Custom color defined in the next setting (see below for examples) 

###### Custom color

Original cursor color to replace with the Values color (used when Mode is Custom). Set 000000 to recolor a black cursor body, or FFFFFF to recolor a white outline.

`ffffff`  
![ghostOptions.color.replace.mode = custom](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.replace.mode.custom.ffffff.gif)  
`f7bb0e`    
![ghostOptions.color.replace.mode = custom](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.replace.mode.custom.f7bb0e.gif)  
`000000`    
![ghostOptions.color.replace.mode = custom](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/ghostOptions.color.replace.mode.custom.000000.gif)

### Enable/disable hotkey

#### Key

Press to toggle the trail on or off. Format: Modifier+Key (e.g. Ctrl+Alt+T). Modifiers: Ctrl, Alt, Shift, Win. At least one modifier is required. Leave empty to disable the hotkey.

#### Animation

Show a circle animation when the hotkey toggles the trail.    
![hotkeyOptions.animate = on](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/hotkeyOptions.animate.gif)

The animation is a circle outline (2px, in the cursor's color, centered on the trail start and following the cursor): it grows and fades out when disabling, and shrinks and fades in when enabling.  

### Trail offset
Fine-tune the trail origin horizontally in pixels (auto-centered by default, 0 = no adjustment)

`0 0`  
![tail_offset.x = -10](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/tail_offset.0.0.gif)

`15 15`  
![tail_offset.y = +10](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/tail_offset.15.15.gif)

## Above the taskbar and Start menu

On Windows 11 the trail is drawn under the taskbar and Start menu. Install
the companion **Cursor trail helper - always on top** mod to lift it above
both (it needs a one-time Win-key press).

## Architecture

The main mod is a single translation unit (`CursorTrail.cpp`); the optional always-on-top helper (see [Above the taskbar](#above-the-taskbar-and-start-menu)) is a separate mod in `CursorTrailHelperAlwaysOnTop.cpp`. This section describes the main mod. All state is file-scope, grouped into five struct instances:

| Instance | Type | Purpose |
|---|---|---|
| `settings` | `Settings` | Parsed settings (tail geometry, style, width/color/opacity, trail offset). Written by `LoadSettings()`, read by all threads. |
| `cursor` | `CursorState` | Cursor geometry cache: `centerOffset`/`visualOffset` (mutex-protected) plus render-thread-only debug dims and the per-`HCURSOR` `geomCache`. |
| `origin` | `OriginTransition` | Poll-thread-owned ease-in-out state for the trail-origin glide on cursor-image change. |
| `render` | `RenderResources` | Direct2D factory/target/brushes, stroke style, the cached backbuffer, and the per-`HCURSOR` `cursorBitmapCache` (tinted bitmap variants) used by the ghost style. Render-thread-only. |
| `runtime` | `Runtime` | Overlay window/threads, the `history` deque, atomics, multimedia timer, and per-frame render state. |

### Threads

- **Overlay thread** (`OverlayThreadProc`) — creates the `WS_EX_LAYERED` topmost window, runs the message loop, and does all Direct2D rendering via `SmearTimerProc`.
- **Poll thread** (`PollThreadProc`) — samples the cursor every 1 ms and pushes decimated samples into `runtime.history`.
- **Multimedia timer** (`MMTimerCallback`, a system thread) — posts `WM_TIMER` at ~125 Hz to wake the overlay thread. It never touches Direct2D directly.

### Locking model

- `runtime.historyMutex` protects `runtime.history` (poll + render threads).
- `cursor.offsetMutex` protects `cursor.centerOffset` / `cursor.visualOffset` (written by render thread, read by poll thread).
- Lock order is always `runtime.historyMutex` → `cursor.offsetMutex`.
- `runtime.isGameRunning`, `runtime.cursorHidden`, `runtime.renderScheduled`, and `runtime.trailEnabled` are atomics.
- `origin.*`, `render.*`, and the cursor debug dimensions are single-thread owned (see table above).

### Render pipeline

`SmearTimerProc` is a thin orchestrator that delegates to helpers, in order:

1. `EnsureBackbuffer` / `EnsureRenderTarget` — (re)create the backbuffer bitmap and D2D render target.
2. `BuildTrailPoints` — snapshot `runtime.history`; the line style spatially decimates it, while the ghost style emits every latched copy in order, producing a parallel per-point `HCURSOR` list and (ghost only) a per-point opacity `ratio` list.
3. `ChaikinSmooth` (Simple line only) — two-pass corner smoothing; the ghost style draws its latched copies directly so its copy count matches the setting.
4. `ComputeTrailBBox` — trail bounding box plus stroke-width (line) or cursor-size (ghost) margin.
5. `RenderTrail` — dispatch to the active style renderer (`RenderSimpleLineStyle` or `RenderCursorGhostStyle`); both paint tail → head so the newest part stays on top at self-crossings.
6. `RenderToggleEffect` — optional enable/disable hotkey circle (2px outline, centered on the trail head, follows the cursor).
7. `DrawDebug` — optional white/red outline boxes plus a green trail-start marker.
8. `BlitOverlay` — dirty-rect tracking plus `UpdateLayeredWindow`.
9. `PruneCursorCaches` (ghost only) — drop cached cursor geometry/bitmaps no longer referenced by the trail.

### Settings & blending

- `LoadSettings` uses `ReadStringSetting`, `ParseFloatList`, `SplitAndTrim`, and `ParseHexColor`, and precomputes color band boundaries (`settings.colorBandStart`/`colorBandEnd`) and opacity alphas (`settings.opacityValues`, stored as 0–1) so the hot path does no parsing or per-frame allocation. `LoadCommonTrailSettings(prefix)` reads the settings shared by both styles (trail mode, tail duration/size, timeout, opacity) from `ghostOptions` or `simpleLineOptions`, and `LoadColorSettings(prefix, defaultColor)` reads the per-style color gradient into `settings.activeColorsRGB` (and precomputes the ghost tint samples into `settings.ghostTints`); `TrailPointBudget`/`AutoPointSpacing` hold the shared point-count and spacing formulas.
- `GetBlendedColor`, `InterpolateValues`, and `InterpolateOpacity` are allocation-free; `Ease` applies the smoothstep easing curve.

### Cursor geometry

`UpdateCursorCenterOffset` caches, per `HCURSOR`: the bitmap-center offset (for the debug boxes), the visible-pixel-center offset (the line-style trail origin), the hotspot, the alpha-trimmed visible bounds, and the DPI scale. It is rebuilt only when the cursor handle changes. `ComputeCursorGeom` holds the shared computation; `GetCursorGeom` lazily computes geometry for any cursor handle still referenced by the trail (so the ghost style can draw older images after an image change). Ghost samples store the raw cursor hotspot, and the ghost renderer anchors each copy by its own image's hotspot, so a copy lands exactly where that cursor image was — independent of the render thread's offset refresh. The ghost style also builds and caches D2D bitmaps per `HCURSOR` via `EnsureCursorBitmap`, rendering the cursor with `DrawIconEx` at the on-screen pixel size (color + mask + anti-aliased alpha) so copies are blitted 1:1 without resampling. The `ghostOptions.color` gradient (when set) is baked into the cached pixels by sampling it at `kGhostTintSteps` ratios (one bitmap per sample; an empty color list yields a single untinted variant), and `GetCursorBitmap(hCursor, ratio)` selects the nearest variant for each copy. `color.replace.mode` picks the pixels to recolor: `whole` swaps every non-transparent pixel; `auto` splits each pixel between two reference colors — the replace color (the cursor's enclosed center color from `AnalyzeCursorColors`, falling back to the largest region when nothing is enclosed) and a keep color (the largest boundary/outline region) — swapping pixels closer to the replace color with a small softness band around the midpoint; `custom` replaces pixels whose color matches `color.replace.custom` (soft falloff so anti-aliased edges blend), leaving every other color untouched. Both caches are released with the render target.

### Lifecycle

- `WhTool_ModInit` — `LoadSettings()` then spawns `OverlayThreadProc`.
- `WhTool_ModSettingsChanged` — `LoadSettings()`, then posts `kMsgApplyHotkey` to the overlay window to re-register the hotkey on its thread.
- `WhTool_ModUninit` — signals the poll thread, kills the timer, and posts `WM_QUIT`.
- The overlay thread registers the `hotkeyOptions.key` setting (`ApplyHotkey`) right after creating the window and unregisters it before destroying the window. `WM_HOTKEY` flips `runtime.trailEnabled`, which suppresses sampling/rendering like the fullscreen-game path does, and (when `hotkeyOptions.animate` is on) calls `StartToggleEffect` to play the circle animation centered on the trail head (disable: grows + fades out, ease in; enable: shrinks + fades in, ease out; 400 ms, 2px outline, diameter 6× the cursor, colored by `GetCursorColor`'s ghost-`auto` pick, following the cursor).
- The `Wh_ModInit` / `Wh_ModAfterInit` / `Wh_ModUninit` block at the bottom of the file is Windhawk's tool-mod launcher boilerplate and should be left as-is.

## Licence

This project is licensed under the MIT License. You're free to use, modify, and
redistribute the code as long as you keep the original copyright notice and
credit the author (Ulrizza). See [LICENSE](LICENSE) for the full text.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- style: simple_line
  $name: Style
  $description: |-
    Type of trail:
    - Simple line: a line whose width, color, and opacity can change along its length
    - Cursor ghost: faded copies of the cursor image, each latched where it spawned
  $options:
  - simple_line: Simple line
  - cursor_ghost: Cursor ghost
- simpleLineOptions:
  - trail_mode: "time_based"
    $name: Trail mode
    $description: |-
      How the trail disappears:
      - Time based: each part of the trail fades after the Tail duration
      - Size based: keeps a fixed trail length even when the cursor stops
    $options:
    - time_based: Time based
    - size_based: Size based
  - timeBased:
    - tail_duration: 500
      $name: Tail duration
      $description: How long each trail segment stays visible, in milliseconds. Minimum 20.
    $name: Time based
    $description: Applies when Trail mode is Time based.
  - sizeBased:
    - tail_size: 2000
      $name: Tail length
      $description: Maximum trail length in pixels. Minimum 20.
    - timeout: 2000
      $name: Timeout
      $description: Milliseconds of inactivity before the trail starts fading (using the Time based tail duration). 0 = trail always visible.
    $name: Size based
    $description: Applies when Trail mode is Size based.
  - width:
    - values: "2,1"
      $name: Values (head to tail)
      $description: "Comma-separated stroke widths in pixels from head to tail (e.g. \"2,1\" for a tapered trail, or \"10,1,10,1\" for a pulsing trail). Each value gets an equal share; repeat to widen (e.g. \"2,2,2,2,1\" = 80% at 2, 20% at 1). Minimum 1."
    $name: Width
  - color:
    - values: "00A2FF,8B00FF"
      $name: Values (head to tail)
      $description: "Single hex (RRGGBB without #, e.g. 000000 for black) or comma-separated list for a gradient from head to tail (e.g. 000000,FF0000,FFFFFF for black->red->white). Each color gets an equal share; repeat to widen. Invalid entries fall back to black."
    - blend_width: 100
      $name: Blend width
      $description: Percentage of each transition spent blending (0 = pure bands, 100 = full gradient). 50 with red,blue gives 25% hard red, 50% blend, 25% hard blue. Only applies with two or more colors.
    $name: Color
  - opacity:
    - values: "100,80"
      $name: Values (head to tail)
      $description: Comma-separated opacity percentages (0-100) from head to tail (e.g. "100,0" for full fade, or "100,0,100" for a pulse). Each value gets an equal share; repeat to widen. Leave one value for uniform opacity.
    $name: Opacity
  - antialiasing: true
    $name: Antialiasing
    $description: Smooth the trail edges.
  $description: Applies when Style is Simple line.
  $name: Simple line options
- ghostOptions:
  - trail_mode: "time_based"
    $name: Trail mode
    $description: |-
      How the copies disappear:
      - Time based: each copy fades after the Tail duration
      - Size based: keeps a fixed number of copies even when the cursor stops
    $options:
    - time_based: Time based
    - size_based: Size based
  - timeBased:
    - tail_duration: 300
      $name: Tail duration
      $description: How long each cursor copy stays visible, in milliseconds. Minimum 20.
    $name: Time based
    $description: Applies when Trail mode is Time based.
  - sizeBased:
    - tail_size: 20
      $name: Copies
      $description: Number of cursor copies in the trail (Size based mode). Minimum 2, maximum 512.
    - timeout: 2000
      $name: Timeout
      $description: Milliseconds of inactivity before the trail starts fading (using the Time based tail duration). 0 = trail always visible.
    $name: Size based
    $description: Applies when Trail mode is Size based.
  - spacing: 10
    $name: Copy spacing
    $description: Extra distance in pixels added between cursor copies (0 = automatic, based on the copy count). Maximum 200.
  - size:
    - values: "1,0"
      $name: Values (head to tail)
      $description: "Comma-separated size multipliers from head to tail (1 = same size, 0.8 = 80%, 2 = twice). Each value gets an equal share; repeat to widen (e.g. \"1,0.5,1,0.5\"). Avoid values above 1 (upscaled copies look pixelated); use the Windows cursor size setting to enlarge the cursor."
    $name: Size
  - opacity:
    - values: "50,20"
      $name: Values (head to tail)
      $description: Comma-separated opacity percentages (0-100) from head to tail (e.g. "100,0" for full fade). Each value gets an equal share; repeat to widen.
    $name: Opacity
  - color:
    - values: ""
      $name: Values (head to tail)
      $description: "Leave empty to keep the cursor's own colors. Otherwise a single hex (RRGGBB without #) or comma-separated list for a gradient from head to tail; pixels matching the Replace color are recolored to this value (FFFFFF makes white copies). Each color gets an equal share; repeat to widen. Invalid entries fall back to black. A single color disables Blend width."
    - blend_width: 100
      $name: Blend width
      $description: Percentage of each transition spent blending (0 = pure bands, 100 = full gradient). Only applies with two or more colors.
    - replace:
      - mode: "auto"
        $name: Mode
        $description: |-
          Which cursor pixels to recolor:
          - Auto: the cursor's enclosed center color (ignoring the outline/contour; falls back to the largest area when nothing is enclosed)
          - Whole: every non-transparent pixel
          - Custom: the Custom color below
        $options:
        - auto: Auto (center color)
        - whole: Whole cursor
        - custom: Custom
      - custom: "FFFFFF"
        $name: Custom color
        $description: "Original cursor color to replace with the Values color (used when Mode is Custom). Set 000000 to recolor a black cursor body, or FFFFFF to recolor a white outline."
      $name: Replace
      $description: Only used when Color > Values is set.
    $name: Color
  $description: Applies when Style is Cursor ghost.
  $name: Cursor ghost options
- hotkeyOptions:
  - key: ""
    $name: Key
    $description: >-
      Press to toggle the trail on or off. Format: Modifier+Key (e.g. Ctrl+Alt+T).
      Modifiers: Ctrl, Alt, Shift, Win. At least one modifier is required.
      Leave empty to disable the hotkey.
  - animate: true
    $name: Animation
    $description: Show a circle animation when the hotkey toggles the trail.
  $name: Enable/disable hotkey
- tail_offset:
  - x: 0
    $name: X
    $description: Fine-tune the trail origin horizontally in pixels (auto-centered by default, 0 = no adjustment)
  - y: 0
    $name: Y
    $description: Fine-tune the trail origin vertically in pixels (auto-centered by default, 0 = no adjustment)
  $name: Trail offset
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <math.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <wincodec.h>
#include <mmsystem.h>
#include <algorithm>
#include <atomic>
#include <cwctype>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <windhawk_utils.h>

// Position sample used by the polling thread and spatial decimation in the render loop.
struct Sample {
    POINT pos;
    DWORD t;  // Timestamp captured at sampling time (timeGetTime())
    HCURSOR cursor;  // Cursor image active at sampling time (frozen per-copy image)
};

// Global state variables

// Number of tint samples baked across the ghost color gradient. A flat color
// needs only one; otherwise the continuous gradient is quantized to this many
// steps (higher = smoother, more bitmaps per cursor image).
static const int kGhostTintSteps = 64;

// Recoloring swaps a pixel into the tint when it is closer to the replace color
// than to the keep color (Auto), or when it matches the custom color (Custom).
// kGhostSplitSoftness is the RGB-distance band over which the swap ramps, so
// anti-aliased transitions blend instead of leaving a hard edge.
static const float kGhostSplitSoftness = 0.2f;

// Dominant-color detection: pixels at least this opaque are grouped into
// connected regions, and neighboring pixels whose straight color is within this
// distance of the seed join the region. The two largest regions' colors are
// used as the reference colors for recoloring.
static const BYTE  kGhostRegionMinAlpha = 128;
static const float kGhostRegionTol = 0.2f;

// Bumped by LoadSettings whenever parsed settings change, so the render thread
// can invalidate caches (e.g. tinted cursor bitmaps) without comparing values.
static std::atomic<unsigned> g_settingsVersion{0};

struct Rgb { float r, g, b; };

// How ghost copies choose which cursor pixels to recolor.
enum GhostReplaceMode { GHOST_REPLACE_AUTO, GHOST_REPLACE_CUSTOM, GHOST_REPLACE_WHOLE };

// Parsed settings, written by LoadSettings and read by all threads.
struct Settings {
    int   tailOffsetX = 0, tailOffsetY = 0;
    int   tailDuration = 1000;
    bool  sizeBased = false;                          // trail mode: size_based vs time_based
    int   tailSize = 2000;
    DWORD sizeTimeout = 0;
    bool  antialiasing = true;
    bool  debugShowOutline = false;                    // test-only; hidden from the settings UI
    bool  debugShowTailPreview = false;                // test-only; hidden from the settings UI
    bool  isGhost = false;                            // active style is cursor_ghost
    int   ghostSpacing = 0;                           // extra px between ghost copies
    float ghostSpawnDist = 2.0f;                      // px cursor must travel before a new ghost is stamped
    std::vector<float> ghostSizes;                    // per-copy size multipliers, head → tail
    float ghostSizeMax = 1.0f;                        // largest size multiplier (min 1), for the bbox
    bool  ghostTintActive = false;                    // ghost color list is non-empty (recolor copies)
    std::vector<Rgb> ghostTints;                      // precomputed tint per baked variant (head → tail)
    GhostReplaceMode ghostReplaceMode = GHOST_REPLACE_AUTO;  // which pixels to recolor
    Rgb   ghostReplaceColor = { 1.0f, 1.0f, 1.0f };   // Custom: original cursor color to swap for the tint

    std::wstring    activeStyle = L"simple_line";

    std::vector<float> simpleLineWidths;              // parsed width values, one per stop
    std::vector<Rgb>   activeColorsRGB;               // pre-parsed colors of the active style
    int   colorBlendWidth = 0;
    float colorBlendHalf = 0.0f;
    std::vector<float> colorBandStart;                // precomputed pure-band boundaries
    std::vector<float> colorBandEnd;
    std::vector<float> opacityValues;                 // parsed opacity alphas (0.0-1.0), shared by both styles
};

// Per-cursor geometry, cached per HCURSOR so the ghost style can place copies
// of the exact image that was on screen at sample time. Render-thread-only.
struct CursorGeom {
    bool  valid = false;
    int   bmWidth = 0, bmHeight = 0;
    float dpiScaleX = 1.0f, dpiScaleY = 1.0f;
    int   hotspotX = 0, hotspotY = 0;  // hotspot in native bitmap pixels
    POINT centerOffset = { 0, 0 };   // hotspot → bitmap center (scaled)
    POINT visualOffset = { 0, 0 };   // hotspot → visible-pixel center (scaled)
    float visCenterX = 0.0f, visCenterY = 0.0f;  // visible center in bitmap pixels
    bool  visibleValid = false;
    int   visLeft = 0, visTop = 0, visRight = 0, visBottom = 0;
};

// Cursor geometry cache. The center/visual offsets are shared with the poll
// thread via offsetMutex; the debug dimensions and geomCache below are
// render-thread-only.
struct CursorState {
    HCURSOR cachedCursor = NULL;
    POINT   centerOffset = { 0, 0 };  // hotspot → bitmap center (anchors debug boxes)
    POINT   visualOffset = { 0, 0 };  // hotspot → visible-pixel center (trail origin)
    std::mutex offsetMutex;                 // protects center/visual offsets (read by poll thread)

    int   bmWidth = 0, bmHeight = 0;        // bitmap dims + DPI scale for the debug boxes
    float dpiScaleX = 1.0f, dpiScaleY = 1.0f;
    bool  visibleValid = false;             // visible (alpha-trimmed) bounds
    int   visLeft = 0, visTop = 0, visRight = 0, visBottom = 0;

    std::unordered_map<HCURSOR, CursorGeom> geomCache;  // render-thread-only per-cursor geometry
};

// Ease-in-out origin transition state (poll-thread-owned).
struct OriginTransition {
    float fromX = 0.0f, fromY = 0.0f;       // value at transition start
    POINT target = { 0, 0 };                // current target offset
    float smoothedOffsetX = 0.0f, smoothedOffsetY = 0.0f;
    DWORD startTime = 0;                    // time_based transition start
    float progressDist = 0.0f;              // size_based distance travelled
    bool  transitioning = false;
    bool  initialized = false;
    POINT lastCursorPos = { 0, 0 };         // for distance accumulation
    bool  lastCursorValid = false;
};

// Direct2D + backbuffer resources.
// A cursor image pre-scaled to its on-screen pixel size, so the ghost style can
// blit it 1:1 instead of resampling it every frame.
struct CachedCursorBitmap {
    ID2D1Bitmap* bitmap = nullptr;
    UINT width = 0, height = 0;    // pixel size of the cached (scaled) bitmap
    int  targetW = 0, targetH = 0; // requested on-screen size, for invalidation
};

// All tinted variants of one cursor image. The ghost color gradient is sampled
// at kGhostTintSteps ratios (0 = head, 1 = tail) and baked into a bitmap per
// sample; a flat color yields a single variant.
struct CachedCursorVariants {
    int  targetW = 0, targetH = 0;               // requested on-screen size
    unsigned colorVersion = 0;                   // settings version the tints were built from
    std::vector<CachedCursorBitmap> variants;    // one per sampled tint
};

struct RenderResources {
    ID2D1Factory*         pD2DFactory = nullptr;
    ID2D1DCRenderTarget*  pDCRenderTarget = nullptr;
    ID2D1SolidColorBrush* pSimpleLineBrush = nullptr;
    ID2D1SolidColorBrush* pEffectBrush = nullptr;   // enable/disable toggle circle
    ID2D1StrokeStyle*     pStrokeStyle = nullptr;
    // DEBUG brushes (temporary): white = bitmap bounds, red = visible pixels, green = trail start.
    ID2D1SolidColorBrush* pDebugBrush = nullptr;
    ID2D1SolidColorBrush* pDebugBrushRed = nullptr;
    ID2D1SolidColorBrush* pDebugBrushGreen = nullptr;

    // Cursor ghost style: per-HCURSOR D2D bitmaps, built lazily when a new
    // cursor image appears and released with the render target.
    std::unordered_map<HCURSOR, CachedCursorVariants> cursorBitmapCache;

    HDC     hdcMem = NULL;                  // cached backbuffer
    HBITMAP hBitmap = NULL;
    int     cachedVW = 0, cachedVH = 0;
};

// Runtime state: threads, window, history, atomics, timer, and frame state.
struct Runtime {
    HWND   overlayHwnd = NULL;
    HANDLE threadHandle = NULL;
    DWORD  overlayThreadId = 0;
    std::deque<Sample> history;
    std::mutex historyMutex;                  // protects history (poll + render threads)
    HANDLE pollThread = NULL;
    HANDLE pollStopEvent = NULL;
    std::atomic<bool> isGameRunning{false};   // set by render thread, read by poll thread
    std::atomic<bool> cursorHidden{false};    // set by render thread, read by poll thread
    std::atomic<bool> renderScheduled{false}; // set by MMTimerCallback, cleared by overlay thread
    std::atomic<bool> trailEnabled{true};     // toggled by the enable/disable hotkey; read by both threads
    int     sampleRate = 1;                   // polling interval in ms
    MMRESULT mmTimerId = 0;

    DWORD lastMovementTime = 0;               // poll-thread-owned
    POINT lastCursorPos = { 0, 0 };           // previous raw cursor position (movement tracking)
    bool  lastCursorValid = false;
    bool  isFading = false;

    RECT  prevDirtyRect = { 0, 0, 0, 0 };     // render-thread-only frame state
    bool  hasPrevDirty = false;
    bool  needsClear = false;
    DWORD lastFullscreenCheck = 0;
};

// Transient circle effect played when the enable/disable hotkey toggles the
// trail. Owned by the overlay thread (started on WM_HOTKEY, drawn in
// SmearTimerProc), so it needs no locking. The circle follows the cursor while
// it plays:
//   disable: radius 0 -> R, alpha 1 -> 0, ease in  (grows, fades out)
//   enable:  radius R -> 0, alpha 0 -> 1, ease out (shrinks, fades in)
struct ToggleEffect {
    bool  active = false;
    bool  enabling = false;   // true = enable animation, false = disable animation
    DWORD startTime = 0;      // timeGetTime() at trigger
    Rgb   color = { 1.0f, 1.0f, 1.0f };  // cursor color, captured at trigger
};

// Toggle-effect timing/size. The circle is a 2px outline whose diameter is
// kEffectDiameterFactor times the cursor's on-screen size.
static const DWORD kEffectDurationMs = 400;
static const float kEffectDiameterFactor = 6.0f;
static const float kEffectStrokeWidth = 2.0f;

ToggleEffect toggleEffect;

Settings         settings;
CursorState      cursor;
OriginTransition origin;
RenderResources  render;
Runtime          runtime;

static std::vector<std::wstring> SplitAndTrim(const std::wstring& input) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start <= input.size()) {
        size_t comma = input.find(L',', start);
        std::wstring token = (comma == std::wstring::npos)
            ? input.substr(start)
            : input.substr(start, comma - start);
        size_t first = token.find_first_not_of(L" \t");
        size_t last = token.find_last_not_of(L" \t");
        if (first != std::wstring::npos && last != std::wstring::npos) {
            token = token.substr(first, last - first + 1);
        } else {
            token = L"";
        }
        if (!token.empty()) {
            result.push_back(token);
        }
        if (comma == std::wstring::npos) break;
        start = comma + 1;
    }
    return result;
}

// Parses a hex color like L"RRGGBB" or L"#RRGGBB" into float components [0.0–1.0].
// Returns false on failure; r/g/b are left unchanged on success only.
static bool ParseHexColor(const std::wstring& hex, float& r, float& g, float& b) {
    std::wstring s = hex;
    if (!s.empty() && s[0] == L'#') s = s.substr(1);
    if (s.length() != 6) return false;
    auto hexVal = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        return -1;
    };
    int r0 = hexVal(s[0]); int r1 = hexVal(s[1]);
    int g0 = hexVal(s[2]); int g1 = hexVal(s[3]);
    int b0 = hexVal(s[4]); int b1 = hexVal(s[5]);
    if (r0 < 0 || r1 < 0 || g0 < 0 || g1 < 0 || b0 < 0 || b1 < 0) return false;
    int rv = r0 * 16 + r1;
    int gv = g0 * 16 + g1;
    int bv = b0 * 16 + b1;
    r = rv / 255.0f;
    g = gv / 255.0f;
    b = bv / 255.0f;
    return true;
}

// Reads a string setting, returning def when the setting is missing or empty.
// The RAII WindhawkUtils::StringSetting frees the string on destruction; the
// returned copy is owned by the caller.
static std::wstring ReadStringSetting(const wchar_t* key, const std::wstring& def) {
    auto setting = WindhawkUtils::StringSetting::make(key);
    PCWSTR s = setting.get();
    std::wstring r = (s && *s) ? std::wstring(s) : def;
    return r;
}

// --- Enable/disable hotkey -------------------------------------------------
// One global hotkey toggles the trail. It is registered on the overlay window,
// which already pumps messages, so WM_HOTKEY is delivered on the overlay
// thread. MOD_NOREPEAT suppresses auto-repeat while the combo is held.
static const int kHotkeyId = 1;
static const UINT kMsgApplyHotkey = WM_APP + 1;

// Splits on '+', trimming spaces and dropping empty tokens.
static std::vector<std::wstring> SplitOnPlus(const std::wstring& input) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (true) {
        size_t plus = input.find(L'+', start);
        std::wstring token = (plus == std::wstring::npos)
            ? input.substr(start)
            : input.substr(start, plus - start);
        size_t first = token.find_first_not_of(L" \t");
        size_t last = token.find_last_not_of(L" \t");
        if (first != std::wstring::npos && last != std::wstring::npos) {
            result.push_back(token.substr(first, last - first + 1));
        }
        if (plus == std::wstring::npos) break;
        start = plus + 1;
    }
    return result;
}

// Maps an uppercased key name (e.g. "A", "F5", "SPACE") to a virtual-key code,
// or 0 when unknown.
static UINT VkFromKeyName(const std::wstring& name) {
    if (name.size() == 1) {
        wchar_t c = name[0];
        if (c >= L'A' && c <= L'Z') return (UINT)c;
        if (c >= L'0' && c <= L'9') return (UINT)c;
    }

    if (name.size() >= 2 && name[0] == L'F') {
        int n = 0;
        bool ok = true;
        for (size_t i = 1; i < name.size(); ++i) {
            if (name[i] < L'0' || name[i] > L'9') { ok = false; break; }
            n = n * 10 + (name[i] - L'0');
        }
        if (ok && n >= 1 && n <= 24) return VK_F1 + (UINT)(n - 1);
    }

    static const struct { const wchar_t* name; UINT vk; } kNamedKeys[] = {
        { L"BACKSPACE", VK_BACK },     { L"TAB", VK_TAB },
        { L"ENTER", VK_RETURN },       { L"RETURN", VK_RETURN },
        { L"ESC", VK_ESCAPE },         { L"ESCAPE", VK_ESCAPE },
        { L"SPACE", VK_SPACE },        { L"INSERT", VK_INSERT },
        { L"DELETE", VK_DELETE },      { L"DEL", VK_DELETE },
        { L"HOME", VK_HOME },          { L"END", VK_END },
        { L"PAGEUP", VK_PRIOR },       { L"PAGEDOWN", VK_NEXT },
        { L"UP", VK_UP },              { L"DOWN", VK_DOWN },
        { L"LEFT", VK_LEFT },          { L"RIGHT", VK_RIGHT },
        { L"CAPSLOCK", VK_CAPITAL },   { L"NUMLOCK", VK_NUMLOCK },
        { L"SCROLLLOCK", VK_SCROLL },  { L"PRINTSCREEN", VK_SNAPSHOT },
        { L"PAUSE", VK_PAUSE },        { L"APPS", VK_APPS },
        { L"MINUS", VK_OEM_MINUS },    { L"PLUS", VK_OEM_PLUS },
        { L"COMMA", VK_OEM_COMMA },    { L"PERIOD", VK_OEM_PERIOD },
        { L"SLASH", VK_OEM_2 },        { L"SEMICOLON", VK_OEM_1 },
        { L"QUOTE", VK_OEM_7 },        { L"BACKTICK", VK_OEM_3 },
        { L"LBRACKET", VK_OEM_4 },     { L"BACKSLASH", VK_OEM_5 },
        { L"RBRACKET", VK_OEM_6 },
    };
    for (const auto& k : kNamedKeys) {
        if (name == k.name) return k.vk;
    }

    // Single punctuation/OEM characters, resolved against the current layout.
    if (name.size() == 1) {
        SHORT vk = VkKeyScanW(name[0]);
        if (vk != -1) return (UINT)(vk & 0xFF);
    }

    return 0;
}

// Parses "Modifier+...+Key" into modifiers and a virtual key. Returns false for
// an empty or unknown string. At least one modifier and exactly one key are
// required, so a bare key is never grabbed system-wide.
static bool ParseHotkey(const std::wstring& str, UINT& modifiersOut, UINT& vkOut) {
    UINT modifiers = 0;
    UINT vk = 0;

    for (std::wstring part : SplitOnPlus(str)) {
        for (wchar_t& c : part) c = (wchar_t)towupper((wint_t)c);

        if (part == L"CTRL" || part == L"CONTROL") { modifiers |= MOD_CONTROL; continue; }
        if (part == L"ALT")   { modifiers |= MOD_ALT; continue; }
        if (part == L"SHIFT") { modifiers |= MOD_SHIFT; continue; }
        if (part == L"WIN" || part == L"WINDOWS") { modifiers |= MOD_WIN; continue; }

        if (vk != 0) return false;  // more than one non-modifier key

        vk = VkFromKeyName(part);
        if (vk == 0) {
            // Numeric virtual key, e.g. "0x70".
            try {
                size_t pos = 0;
                unsigned long n = std::stoul(part, &pos, 0);
                if (pos == part.size() && n > 0 && n < 0x100) {
                    vk = (UINT)n;
                } else {
                    return false;
                }
            } catch (...) {
                return false;
            }
        }
    }

    if (modifiers == 0 || vk == 0) return false;
    modifiersOut = modifiers;
    vkOut = vk;
    return true;
}

// (Re)registers the hotkey from the current setting on the overlay window. Must
// run on the overlay thread, which owns the window and its message queue.
static void ApplyHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, kHotkeyId);

    std::wstring hotkey = ReadStringSetting(L"hotkeyOptions.key", L"");
    if (hotkey.empty()) {
        Wh_Log(L"Hotkey disabled (empty setting)");
        return;
    }

    UINT modifiers = 0, vk = 0;
    if (!ParseHotkey(hotkey, modifiers, vk)) {
        Wh_Log(L"Failed to parse hotkey '%s' (use Modifier+Key, e.g. Ctrl+Alt+T)",
               hotkey.c_str());
        return;
    }

    if (RegisterHotKey(hwnd, kHotkeyId, modifiers | MOD_NOREPEAT, vk)) {
        Wh_Log(L"Registered hotkey '%s'", hotkey.c_str());
    } else {
        Wh_Log(L"RegisterHotKey failed for '%s': %u (another app may use it)",
               hotkey.c_str(), GetLastError());
    }
}

// Parses a comma-separated list of floats into out. Each value is clamped to
// [minV, maxV]; invalid entries become onError. Falls back to defaultToken when
// the setting is empty.
static void ParseFloatList(const wchar_t* key, const std::wstring& defaultToken,
                           float minV, float maxV, float onError,
                           std::vector<float>& out) {
    out.clear();
    std::wstring raw = ReadStringSetting(key, L"");
    std::vector<std::wstring> tokens = raw.empty() ? std::vector<std::wstring>() : SplitAndTrim(raw);
    if (tokens.empty()) tokens.push_back(defaultToken);
    for (const auto& tok : tokens) {
        try {
            float v = std::stof(tok);
            if (v < minV) v = minV;
            if (v > maxV) v = maxV;
            out.push_back(v);
        } catch (...) {
            out.push_back(onError);
        }
    }
}

// Trail point/copy budget for the active style: the tail size in size_based
// mode, the tail duration otherwise. Always at least 2.
static size_t TrailPointBudget() {
    size_t n = settings.sizeBased ? (size_t)settings.tailSize : (size_t)settings.tailDuration;
    return (n < 2) ? 2 : n;
}

// Auto spacing between trail points for a given budget: smaller for longer
// trails, clamped to [2, 6] px. Shared by the line decimation and the ghost
// spawn distance.
static float AutoPointSpacing(size_t kMaxPoints) {
    if (kMaxPoints < 2) kMaxPoints = 2;
    float d = 6.0f * (10.0f / (float)kMaxPoints);
    if (d < 2.0f) d = 2.0f;
    if (d > 6.0f) d = 6.0f;
    return d;
}

// Reads the settings shared by both styles (trail mode, tail duration/size,
// timeout, opacity) from the style's settings prefix ("ghostOptions" or
// "simpleLineOptions"). The two styles keep independent values.
static void LoadCommonTrailSettings(const wchar_t* prefix) {
    auto key = [prefix](const wchar_t* suffix) { return std::wstring(prefix) + L"." + suffix; };
    settings.sizeBased    = ReadStringSetting(key(L"trail_mode").c_str(), L"time_based") == L"size_based";
    settings.tailDuration = Wh_GetIntSetting(key(L"timeBased.tail_duration").c_str());
    settings.tailSize     = Wh_GetIntSetting(key(L"sizeBased.tail_size").c_str());
    settings.sizeTimeout  = Wh_GetIntSetting(key(L"sizeBased.timeout").c_str());

    ParseFloatList(key(L"opacity.values").c_str(), L"100", 0.0f, 100.0f, 100.0f, settings.opacityValues);
    for (float& v : settings.opacityValues) v /= 100.0f;
}

// Interpolates the active style's color gradient at the given ratio (defined
// after LoadSettings; forward-declared so the tint samples can be precomputed).
static void GetBlendedColor(float ratio, float& r, float& g, float& b);

// Parses the color settings (values, blend width) for a style
// prefix into settings.activeColorsRGB and precomputes the pure-band
// boundaries used by GetBlendedColor. The two styles keep independent values.
// Returns false when the values list is empty and defaultColor is null, meaning
// "no color set" (the caller keeps the original appearance). A non-null
// defaultColor is substituted for an empty list.
static bool LoadColorSettings(const wchar_t* prefix, const wchar_t* defaultColor) {
    auto key = [prefix](const wchar_t* suffix) { return std::wstring(prefix) + L"." + suffix; };

    settings.activeColorsRGB.clear();
    settings.colorBandStart.clear();
    settings.colorBandEnd.clear();
    {
        std::wstring raw = ReadStringSetting(key(L"color.values").c_str(), L"");
        std::vector<std::wstring> colorTokens = raw.empty()
            ? std::vector<std::wstring>() : SplitAndTrim(raw);
        if (colorTokens.empty()) {
            if (!defaultColor) {
                settings.colorBlendHalf = 0.0f;
                return false;
            }
            colorTokens.push_back(defaultColor);
        }
        for (const auto& hex : colorTokens) {
            Rgb rgb;
            if (!ParseHexColor(hex, rgb.r, rgb.g, rgb.b)) {
                rgb = { 0, 0, 0 };
            }
            settings.activeColorsRGB.push_back(rgb);
        }
    }

    settings.colorBlendWidth = Wh_GetIntSetting(key(L"color.blend_width").c_str());
    if (settings.colorBlendWidth < 0) settings.colorBlendWidth = 0;
    if (settings.colorBlendWidth > 100) settings.colorBlendWidth = 100;

    // Precompute pure-band boundaries for GetBlendedColor.
    settings.colorBlendHalf = (settings.colorBlendWidth / 100.0f) / 2.0f;
    settings.colorBandStart.clear();
    settings.colorBandEnd.clear();
    {
        size_t N = settings.activeColorsRGB.size();
        settings.colorBandStart.reserve(N);
        settings.colorBandEnd.reserve(N);
        for (size_t i = 0; i < N; ++i) {
            float start = (i == 0) ? 0.0f : (float)i / (float)N + settings.colorBlendHalf;
            float end = (i == N - 1) ? 1.0f : (float)(i + 1) / (float)N - settings.colorBlendHalf;
            settings.colorBandStart.push_back(start);
            settings.colorBandEnd.push_back(end);
        }
    }
    return true;
}

void LoadSettings() {
    settings.tailOffsetX = Wh_GetIntSetting(L"tail_offset.x");
    settings.tailOffsetY = Wh_GetIntSetting(L"tail_offset.y");

    // Test-only debug features, hidden from the settings UI (the `debug` group
    // was removed from the ==WindhawkModSettings== metadata). Flip these to
    // true to exercise the debug outline / tail preview during development.
    settings.debugShowOutline = false;
    settings.debugShowTailPreview = false;

    settings.activeStyle = ReadStringSetting(L"style", L"simple_line");

    // RG-3: unknown style value → fallback to simple_line
    if (settings.activeStyle != L"simple_line" && settings.activeStyle != L"cursor_ghost") {
        settings.activeStyle = L"simple_line";
    }
    settings.isGhost = (settings.activeStyle == L"cursor_ghost");

    if (settings.isGhost) {
        // Cursor ghost has its own trail options. sizeBased.tail_size is the
        // number of cursor copies (not pixels); tailDuration drives the fade.
        LoadCommonTrailSettings(L"ghostOptions");

        settings.ghostSpacing = Wh_GetIntSetting(L"ghostOptions.spacing");
        if (settings.ghostSpacing < 0) settings.ghostSpacing = 0;
        if (settings.ghostSpacing > 200) settings.ghostSpacing = 200;
        settings.antialiasing = true;                  // ghost always uses linear bitmap interpolation

        // Distance the cursor must travel before a new ghost copy is stamped.
        // Ghosts are latched at their spawn position, so this controls the gap
        // between copies (same auto-gap formula the line decimation uses).
        settings.ghostSpawnDist = AutoPointSpacing(TrailPointBudget()) + (float)settings.ghostSpacing;
        if (settings.ghostSpawnDist < 1.0f) settings.ghostSpawnDist = 1.0f;

        // Per-copy size multipliers (head → tail), applied like the line width.
        ParseFloatList(L"ghostOptions.size.values", L"1", 0.05f, 10.0f, 1.0f, settings.ghostSizes);
        settings.ghostSizeMax = 1.0f;
        for (float s : settings.ghostSizes) {
            if (s > settings.ghostSizeMax) settings.ghostSizeMax = s;
        }

        // Per-copy recolor gradient. Empty values mean "keep the cursor's own
        // colors"; any non-empty list recolors the copies (FFFFFF makes white).
        bool ghostHasColor = LoadColorSettings(L"ghostOptions", nullptr);
        settings.ghostTintActive = ghostHasColor;
        settings.ghostTints.clear();
        if (ghostHasColor) {
            // Precompute the tint for each baked bitmap variant (ratio 0 = head,
            // 1 = tail) so EnsureCursorBitmap does no per-frame sampling.
            settings.ghostTints.resize((size_t)kGhostTintSteps);
            for (int i = 0; i < kGhostTintSteps; ++i) {
                float ratio = (float)i / (float)(kGhostTintSteps - 1);
                GetBlendedColor(ratio, settings.ghostTints[i].r,
                                settings.ghostTints[i].g, settings.ghostTints[i].b);
            }
        }

        // Which pixels the tint replaces: Auto uses the cursor's dominant color
        // (largest connected region), Custom uses the custom color below, and
        // Whole recolors every non-transparent pixel.
        std::wstring replaceMode = ReadStringSetting(L"ghostOptions.color.replace.mode", L"auto");
        if (replaceMode == L"custom")     settings.ghostReplaceMode = GHOST_REPLACE_CUSTOM;
        else if (replaceMode == L"whole") settings.ghostReplaceMode = GHOST_REPLACE_WHOLE;
        else                              settings.ghostReplaceMode = GHOST_REPLACE_AUTO;

        settings.ghostReplaceColor = { 1.0f, 1.0f, 1.0f };
        {
            std::wstring replaceHex = ReadStringSetting(L"ghostOptions.color.replace.custom", L"FFFFFF");
            Rgb rgb;
            if (ParseHexColor(replaceHex, rgb.r, rgb.g, rgb.b)) {
                settings.ghostReplaceColor = rgb;
            }
        }
    } else {
        LoadCommonTrailSettings(L"simpleLineOptions");
        LoadColorSettings(L"simpleLineOptions", L"000000");
        settings.ghostTints.clear();

        settings.antialiasing = Wh_GetIntSetting(L"simpleLineOptions.antialiasing") != 0;
    }

    if (settings.tailDuration < 20) settings.tailDuration = 20;
    if (settings.isGhost) {
        // tail_size is a copy count for the ghost style.
        if (settings.tailSize < 2) settings.tailSize = 2;
        if (settings.tailSize > 512) settings.tailSize = 512;
    } else {
        if (settings.tailSize < 20) settings.tailSize = 20;
    }
    if (settings.sizeTimeout < 0) settings.sizeTimeout = 0;

    // Parse width values (min 1, no upper clamp)
    ParseFloatList(L"simpleLineOptions.width.values", L"1", 1.0f, 1e30f, 1.0f, settings.simpleLineWidths);

    // Signal the render thread that caches derived from settings are stale.
    g_settingsVersion.fetch_add(1, std::memory_order_relaxed);
}

static void FreeIconInfoBitmaps(ICONINFO& ii) {
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);
}

// Reads a DWORD from the registry. Returns 0 and sets found=false on failure.
static DWORD ReadRegDword(HKEY root, const wchar_t* subKey, const wchar_t* valueName, bool& found) {
    found = false;
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(root, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) return 0;
    DWORD data = 0, size = sizeof(data), type = 0;
    if (RegQueryValueExW(hKey, valueName, nullptr, &type, (BYTE*)&data, &size) == ERROR_SUCCESS &&
        type == REG_DWORD && size == sizeof(data)) {
        found = true;
    }
    RegCloseKey(hKey);
    return data;
}

// Returns the true on-screen size (physical pixels) of the cursor on the monitor
// under the pointer. The Windows cursor-size setting is stored in the registry
// as a DPI-independent base size (HKCU\Control Panel\Cursors\CursorBaseSize,
// default 32); the system scales it by the monitor DPI. SM_CXCURSOR/SM_CYCURSOR
// only report the nominal default and ignore the setting, so they are used only
// as a fallback.
static void GetActualCursorSize(int& cx, int& cy) {
    UINT dpiX = 96, dpiY = 96;
    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (hMon) {
            GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        }
    }

    bool found = false;
    DWORD base = ReadRegDword(HKEY_CURRENT_USER, L"Control Panel\\Cursors",
                              L"CursorBaseSize", found);
    if (found && base > 0) {
        cx = (int)((double)base * dpiX / 96.0 + 0.5);
        cy = (int)((double)base * dpiY / 96.0 + 0.5);
    } else {
        cx = GetSystemMetricsForDpi(SM_CXCURSOR, dpiX);
        cy = GetSystemMetricsForDpi(SM_CYCURSOR, dpiY);
    }
}

static void ResolveCursorBitmapDimensions(ICONINFO& ii, int& bmWidth, int& bmHeight, HBITMAP& hbmToUse) {
    bmWidth = 0; bmHeight = 0; hbmToUse = NULL;
    if (ii.hbmColor) {
        BITMAP bm = {};
        if (GetObject(ii.hbmColor, sizeof(bm), &bm)) {
            bmWidth = bm.bmWidth;
            bmHeight = bm.bmHeight;
            hbmToUse = ii.hbmColor;
        }
    }
    if (!hbmToUse) {
        BITMAP bm = {};
        if (ii.hbmMask && GetObject(ii.hbmMask, sizeof(bm), &bm)) {
            bmWidth = bm.bmWidth;
            bmHeight = bm.bmHeight / 2;
            hbmToUse = ii.hbmMask;
        }
    }
}

// Computes the visible (non-transparent) pixel bounds of a cursor HBITMAP by
// scanning its alpha channel (threshold 8 ignores faint anti-aliased edges).
// Bounds are in bitmap pixel coordinates (right/bottom exclusive). Uses WIC
// only, so it doesn't require a Direct2D render target.
static void ComputeVisibleBounds(HBITMAP hbm, int& left, int& top, int& right, int& bottom, bool& valid) {
    left = top = right = bottom = 0;
    valid = false;

    IWICImagingFactory* pWicFactory = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&pWicFactory));
    if (FAILED(hr) || !pWicFactory) return;

    IWICBitmap* pWicBitmap = nullptr;
    hr = pWicFactory->CreateBitmapFromHBITMAP(hbm, NULL,
        WICBitmapUsePremultipliedAlpha, &pWicBitmap);
    if (SUCCEEDED(hr) && pWicBitmap) {
        UINT w = 0, h = 0;
        pWicBitmap->GetSize(&w, &h);
        IWICFormatConverter* pConv = nullptr;
        if (SUCCEEDED(pWicFactory->CreateFormatConverter(&pConv)) && pConv) {
            if (SUCCEEDED(pConv->Initialize(pWicBitmap, GUID_WICPixelFormat32bppBGRA,
                    WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
                UINT stride = w * 4;
                std::vector<BYTE> pixels((size_t)stride * h);
                if (SUCCEEDED(pConv->CopyPixels(nullptr, stride,
                        (UINT)pixels.size(), pixels.data()))) {
                    int minX = (int)w, minY = (int)h, maxX = -1, maxY = -1;
                    for (UINT y = 0; y < h; ++y) {
                        const BYTE* row = pixels.data() + (size_t)y * stride;
                        for (UINT x = 0; x < w; ++x) {
                            if (row[x * 4 + 3] > 8) {
                                if ((int)x < minX) minX = (int)x;
                                if ((int)x > maxX) maxX = (int)x;
                                if ((int)y < minY) minY = (int)y;
                                if ((int)y > maxY) maxY = (int)y;
                            }
                        }
                    }
                    if (maxX >= minX && maxY >= minY) {
                        left = minX; top = minY; right = maxX + 1; bottom = maxY + 1;
                        valid = true;
                    }
                }
            }
            pConv->Release();
        }
        pWicBitmap->Release();
    }
    pWicFactory->Release();
}

// Computes geometry for one cursor image (bitmap dims, DPI scale, hotspot to
// bitmap/visible center). Returns false if the cursor cannot be queried.
static bool ComputeCursorGeom(HCURSOR hCursor, CursorGeom& g) {
    g = CursorGeom();
    if (!hCursor) return false;

    ICONINFO ii = { };
    if (!GetIconInfo(hCursor, &ii)) return false;

    int bmWidth = 0, bmHeight = 0;
    HBITMAP hbmToUse = NULL;
    ResolveCursorBitmapDimensions(ii, bmWidth, bmHeight, hbmToUse);

    if (bmWidth > 0 && bmHeight > 0) {
        int actualX = 0, actualY = 0;
        GetActualCursorSize(actualX, actualY);
        float sx = (float)actualX / (float)bmWidth;
        float sy = (float)actualY / (float)bmHeight;

        g.bmWidth = bmWidth;
        g.bmHeight = bmHeight;
        g.dpiScaleX = sx;
        g.dpiScaleY = sy;
        g.hotspotX = (int)ii.xHotspot;
        g.hotspotY = (int)ii.yHotspot;

        // Bitmap center (anchors the debug outline boxes).
        g.centerOffset.x = (int)(((bmWidth / 2.0f) - (int)ii.xHotspot) * sx + 0.5f);
        g.centerOffset.y = (int)(((bmHeight / 2.0f) - (int)ii.yHotspot) * sy + 0.5f);

        // Visible-pixel center (trail origin). Falls back to the bitmap center.
        ComputeVisibleBounds(hbmToUse, g.visLeft, g.visTop,
                             g.visRight, g.visBottom, g.visibleValid);
        float visCx = bmWidth / 2.0f;
        float visCy = bmHeight / 2.0f;
        if (g.visibleValid) {
            visCx = (g.visLeft + g.visRight) / 2.0f;
            visCy = (g.visTop + g.visBottom) / 2.0f;
        }
        g.visCenterX = visCx;
        g.visCenterY = visCy;
        g.visualOffset.x = (int)((visCx - (int)ii.xHotspot) * sx + 0.5f);
        g.visualOffset.y = (int)((visCy - (int)ii.yHotspot) * sy + 0.5f);
        g.valid = true;
    }

    FreeIconInfoBitmaps(ii);
    return g.valid;
}

// Update the cached geometry for the current cursor. The current cursor's
// geometry is also published in the scalar cursor.* fields for the poll thread
// (trail origin) and the debug overlay; the full per-cursor geometry lives in
// cursor.geomCache so the ghost style can place older cursor images too.
void UpdateCursorCenterOffset() {
    std::lock_guard<std::mutex> lock(cursor.offsetMutex);

    CURSORINFO ci = { sizeof(CURSORINFO) };
    if (!GetCursorInfo(&ci) || !(ci.flags & CURSOR_SHOWING) || !ci.hCursor) {
        runtime.cursorHidden.store(true);
        cursor.centerOffset = { 0, 0 };
        cursor.visualOffset = { 0, 0 };
        cursor.visibleValid = false;
        cursor.bmWidth = 0;
        cursor.bmHeight = 0;
        cursor.cachedCursor = NULL;
        return;
    }
    runtime.cursorHidden.store(false);

    if (ci.hCursor == cursor.cachedCursor) {
        return;  // same cursor as last frame, reuse cached geometry
    }

    CursorGeom g;
    if (!ComputeCursorGeom(ci.hCursor, g)) {
        cursor.centerOffset = { 0, 0 };
        cursor.visualOffset = { 0, 0 };
        cursor.visibleValid = false;
        cursor.bmWidth = 0;
        cursor.bmHeight = 0;
        cursor.cachedCursor = NULL;
        return;
    }

    cursor.geomCache[ci.hCursor] = g;

    cursor.centerOffset = g.centerOffset;
    cursor.visualOffset = g.visualOffset;
    cursor.bmWidth = g.bmWidth;
    cursor.bmHeight = g.bmHeight;
    cursor.dpiScaleX = g.dpiScaleX;
    cursor.dpiScaleY = g.dpiScaleY;
    cursor.visibleValid = g.visibleValid;
    cursor.visLeft = g.visLeft;
    cursor.visTop = g.visTop;
    cursor.visRight = g.visRight;
    cursor.visBottom = g.visBottom;

    cursor.cachedCursor = ci.hCursor;
}

// Look up geometry for any cursor handle, computing and caching it on first use
// (so old cursor images referenced by the trail still resolve). Returns false
// when the handle is null or cannot be queried (e.g. already destroyed).
static bool GetCursorGeom(HCURSOR hCursor, CursorGeom& out) {
    if (!hCursor) return false;
    auto it = cursor.geomCache.find(hCursor);
    if (it != cursor.geomCache.end()) {
        if (!it->second.valid) return false;
        out = it->second;
        return true;
    }
    CursorGeom g;
    if (ComputeCursorGeom(hCursor, g)) {
        cursor.geomCache.emplace(hCursor, g);
        out = g;
        return true;
    }
    cursor.geomCache.emplace(hCursor, CursorGeom());  // remember failure
    return false;
}

// Euclidean RGB distance between a straight color and a reference color.
static float RgbDistance(float r, float g, float b, const Rgb& c) {
    float dr = r - c.r, dg = g - c.g, db = b - c.b;
    return sqrtf(dr * dr + dg * dg + db * db);
}

// Result of AnalyzeCursorColors: the cursor's significant colors.
struct CursorColorAnalysis {
    Rgb  dominant = { 1.0f, 1.0f, 1.0f };  // largest region overall (by area)
    Rgb  runnerUp = { 1.0f, 1.0f, 1.0f };  // second largest region overall
    Rgb  interior = { 1.0f, 1.0f, 1.0f };  // largest region that never touches transparency
    Rgb  boundary = { 1.0f, 1.0f, 1.0f };  // largest region that touches transparency
    bool hasInterior = false;
    bool hasBoundary = false;
};

// Analyzes the cursor's colors by grouping sufficiently opaque pixels into
// connected regions (neighbors join when their straight color is close to the
// region's seed color). Regions are classified by whether they touch the
// transparent background: the enclosed "interior" is the cursor's center/fill,
// while a "boundary" region is part of the contour/outline. Used to pick the
// ghost recolor references.
static CursorColorAnalysis AnalyzeCursorColors(const BYTE* src, int w, int h) {
    CursorColorAnalysis result;
    size_t n = (size_t)w * h;
    std::vector<BYTE> alpha(n);
    std::vector<float> sr(n), sg(n), sb(n);
    for (size_t i = 0; i < n; ++i) {
        const BYTE* s = src + i * 4;
        BYTE a = s[3];
        alpha[i] = a;
        if (a == 0) { sr[i] = sg[i] = sb[i] = 0.0f; continue; }
        float inv = 1.0f / (float)a;
        float cr = s[2] * inv, cg = s[1] * inv, cb = s[0] * inv;  // straight
        sr[i] = (cr > 1.0f) ? 1.0f : cr;
        sg[i] = (cg > 1.0f) ? 1.0f : cg;
        sb[i] = (cb > 1.0f) ? 1.0f : cb;
    }

    std::vector<int> label(n, -1);
    std::vector<int> queue;
    size_t bestCount = 0, secondCount = 0;
    size_t bestInteriorCount = 0, bestBoundaryCount = 0;
    Rgb best = { 1.0f, 1.0f, 1.0f };
    Rgb second = { 1.0f, 1.0f, 1.0f };

    for (size_t start = 0; start < n; ++start) {
        if (label[start] >= 0 || alpha[start] < kGhostRegionMinAlpha) continue;

        const float tr = sr[start], tg = sg[start], tb = sb[start];
        queue.clear();
        queue.push_back((int)start);
        label[start] = (int)start;

        size_t count = 0;
        bool touchesOutside = false;
        double sumR = 0.0, sumG = 0.0, sumB = 0.0;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int idx = queue[qi];
            int x = idx % w, y = idx / w;
            ++count;
            sumR += sr[idx]; sumG += sg[idx]; sumB += sb[idx];

            // 4-connected region growth.
            const int nb[4][2] = { { x - 1, y }, { x + 1, y }, { x, y - 1 }, { x, y + 1 } };
            for (int k = 0; k < 4; ++k) {
                int nx = nb[k][0], ny = nb[k][1];
                if (nx < 0 || ny < 0 || nx >= w || ny >= h) { touchesOutside = true; continue; }
                int ni = ny * w + nx;
                if (alpha[ni] < kGhostRegionMinAlpha) { touchesOutside = true; continue; }
                if (label[ni] >= 0) continue;
                float dr = sr[ni] - tr, dg = sg[ni] - tg, db = sb[ni] - tb;
                if (dr * dr + dg * dg + db * db > kGhostRegionTol * kGhostRegionTol) continue;
                label[ni] = (int)start;
                queue.push_back(ni);
            }

            // 8-neighbor transparency contact, so diagonal edges count as contour.
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || ny < 0 || nx >= w || ny >= h ||
                        alpha[ny * w + nx] < kGhostRegionMinAlpha) {
                        touchesOutside = true;
                    }
                }
            }
        }

        Rgb avg = { (float)(sumR / (double)count),
                    (float)(sumG / (double)count),
                    (float)(sumB / (double)count) };
        if (count > bestCount) {
            secondCount = bestCount; second = best;
            bestCount = count; best = avg;
        } else if (count > secondCount) {
            secondCount = count; second = avg;
        }

        if (touchesOutside) {
            if (count > bestBoundaryCount) {
                bestBoundaryCount = count;
                result.boundary = avg;
                result.hasBoundary = true;
            }
        } else if (count > bestInteriorCount) {
            bestInteriorCount = count;
            result.interior = avg;
            result.hasInterior = true;
        }
    }
    result.dominant = best;
    result.runnerUp = (secondCount > 0) ? second : best;
    return result;
}

// Returns the cursor's representative color: the enclosed center color when one
// exists (the same pick the ghost "auto" replace mode uses), otherwise the
// largest region. Renders the cursor to a DIB via DrawIconEx (color + mask +
// anti-aliased alpha), the same path EnsureCursorBitmap uses.
static bool GetCursorColor(HCURSOR hCursor, Rgb& out) {
    if (!hCursor) return false;

    int targetW = 32, targetH = 32;
    CursorGeom g;
    if (GetCursorGeom(hCursor, g) && g.bmWidth > 0 && g.bmHeight > 0) {
        targetW = (int)floorf(g.bmWidth * g.dpiScaleX + 0.5f);
        targetH = (int)floorf(g.bmHeight * g.dpiScaleY + 0.5f);
    }
    if (targetW <= 0 || targetH <= 0) return false;

    BITMAPINFO bmi = { };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = targetW;
    bmi.bmiHeader.biHeight = -targetH;   // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hDib = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hDib || !pBits) {
        if (hDib) DeleteObject(hDib);
        return false;
    }

    HDC hdcMem = CreateCompatibleDC(NULL);
    if (!hdcMem) { DeleteObject(hDib); return false; }
    HGDIOBJ hOld = SelectObject(hdcMem, hDib);
    ZeroMemory(pBits, (size_t)targetW * targetH * 4);
    DrawIconEx(hdcMem, 0, 0, hCursor, targetW, targetH, 0, NULL, DI_NORMAL);
    SelectObject(hdcMem, hOld);
    DeleteDC(hdcMem);

    CursorColorAnalysis ca = AnalyzeCursorColors((const BYTE*)pBits, targetW, targetH);
    out = ca.hasInterior ? ca.interior : ca.dominant;

    DeleteObject(hDib);
    return true;
}

// Build and cache the recolored D2D bitmaps for a cursor handle, rendered at
// the requested on-screen pixel size (render-thread only). The precomputed
// ghost gradient samples (settings.ghostTints) are each baked into their own
// bitmap; an empty color list yields a single untinted variant. The pixels to
// swap are chosen by AnalyzeCursorColors: Auto prefers the enclosed center
// (falling back to the largest region), Custom uses settings.ghostReplaceColor,
// and Whole swaps every non-transparent pixel. The entry is rebuilt when the
// target size or the settings version changes; a failed build leaves the entry
// with null bitmaps so it is not retried.
static void EnsureCursorBitmap(HCURSOR hCursor, int targetW, int targetH) {
    if (!hCursor || !render.pDCRenderTarget) return;

    unsigned version = g_settingsVersion.load(std::memory_order_relaxed);
    auto it = render.cursorBitmapCache.find(hCursor);
    if (it != render.cursorBitmapCache.end()) {
        CachedCursorVariants& c = it->second;
        if (c.targetW == targetW && c.targetH == targetH && c.colorVersion == version)
            return;  // already built for this size and color
        for (auto& v : c.variants) {
            if (v.bitmap) v.bitmap->Release();
        }
        it->second = CachedCursorVariants();
    } else {
        it = render.cursorBitmapCache.emplace(hCursor, CachedCursorVariants()).first;
    }

    CachedCursorVariants& entry = it->second;
    entry.targetW = targetW;
    entry.targetH = targetH;
    entry.colorVersion = version;

    // When recoloring is off, keep a single untinted copy of the cursor image;
    // otherwise bake one bitmap per sampled tint.
    bool tinting = settings.ghostTintActive && !settings.ghostTints.empty();
    const std::vector<Rgb>& tints = settings.ghostTints;
    size_t variantCount = tinting ? tints.size() : 1;
    entry.variants.resize(variantCount);

    if (targetW <= 0 || targetH <= 0) return;

    // Render the cursor with GDI (the same path Windows uses) at the exact
    // on-screen size. DrawIconEx applies the color bitmap AND the mask and
    // produces correct premultiplied, anti-aliased alpha — reconstructing the
    // color bitmap alone does not, which made scaled copies look pixelated.
    BITMAPINFO bmi = { };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = targetW;
    bmi.bmiHeader.biHeight = -targetH;   // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hDib = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    if (!hDib || !pBits) {
        if (hDib) DeleteObject(hDib);
        return;
    }

    HDC hdcMem = CreateCompatibleDC(NULL);
    if (!hdcMem) { DeleteObject(hDib); return; }
    HGDIOBJ hOld = SelectObject(hdcMem, hDib);
    ZeroMemory(pBits, (size_t)targetW * targetH * 4);
    DrawIconEx(hdcMem, 0, 0, hCursor, targetW, targetH, 0, NULL, DI_NORMAL);
    SelectObject(hdcMem, hOld);
    DeleteDC(hdcMem);

    IWICImagingFactory* pWicFactory = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&pWicFactory));
    if (SUCCEEDED(hr) && pWicFactory) {
        size_t pixelBytes = (size_t)targetW * targetH * 4;
        size_t pixelCount = (size_t)targetW * targetH;

        // Per-pixel recolor weight, computed once (independent of the per-copy
        // tint): 1 = swap to the tint, 0 = keep the original color. Whole mode
        // swaps every non-transparent pixel. Otherwise a pixel is swapped when
        // it is closer to the "replace" color (A) than to the "keep" color (B),
        // with a softness band around the midpoint so anti-aliased transitions
        // blend. Straight channels are clamped to [0,1] so matching also works
        // for straight-alpha cursor data.
        std::vector<float> weight;
        if (tinting) {
            weight.assign(pixelCount, 0.0f);
            const BYTE* src = (const BYTE*)pBits;

            if (settings.ghostReplaceMode == GHOST_REPLACE_WHOLE) {
                for (size_t i = 0; i < pixelCount; ++i) {
                    if (src[i * 4 + 3] != 0) weight[i] = 1.0f;
                }
            } else if (settings.ghostReplaceMode == GHOST_REPLACE_CUSTOM) {
                // Custom: replace only pixels close to the custom color, keeping
                // every other color untouched. A soft falloff blends anti-aliased
                // edges between the matched color and its neighbors.
                const Rgb replaceColor = settings.ghostReplaceColor;
                float softness = kGhostSplitSoftness;
                if (softness < 0.01f) softness = 0.01f;
                for (size_t i = 0; i < pixelCount; ++i) {
                    const BYTE* s = src + i * 4;
                    BYTE a = s[3];
                    if (a == 0) continue;
                    float inv = 1.0f / (float)a;
                    float sr = s[2] * inv, sg = s[1] * inv, sb = s[0] * inv;  // straight
                    if (sr > 1.0f) sr = 1.0f;
                    if (sg > 1.0f) sg = 1.0f;
                    if (sb > 1.0f) sb = 1.0f;
                    float dA = RgbDistance(sr, sg, sb, replaceColor);
                    float w = 1.0f - dA / softness;
                    if (w < 0.0f) w = 0.0f;
                    if (w > 1.0f) w = 1.0f;
                    weight[i] = w;
                }
            } else {
                // Auto: split each pixel between the enclosed center (replace)
                // and the contour/outline (keep), so only the fill is recolored.
                CursorColorAnalysis ca = AnalyzeCursorColors(src, targetW, targetH);

                Rgb replaceColor, keepColor;
                // Prefer the enclosed center over the contour. If every
                // region touches the background, fall back to the largest.
                if (ca.hasInterior) {
                    replaceColor = ca.interior;
                    keepColor = ca.hasBoundary ? ca.boundary : ca.runnerUp;
                } else {
                    replaceColor = ca.dominant;
                    keepColor = ca.runnerUp;
                }

                float refDist = RgbDistance(replaceColor.r, replaceColor.g, replaceColor.b, keepColor);
                bool twoColor = refDist > 0.05f;
                // Keep the softness band below half the reference distance so a
                // pure replace-color pixel always reaches a full swap.
                float softness = kGhostSplitSoftness;
                if (softness > refDist * 0.5f) softness = refDist * 0.5f;
                if (softness < 0.01f) softness = 0.01f;
                for (size_t i = 0; i < pixelCount; ++i) {
                    const BYTE* s = src + i * 4;
                    BYTE a = s[3];
                    if (a == 0) continue;
                    if (!twoColor) { weight[i] = 1.0f; continue; }
                    float inv = 1.0f / (float)a;
                    float sr = s[2] * inv, sg = s[1] * inv, sb = s[0] * inv;  // straight
                    if (sr > 1.0f) sr = 1.0f;
                    if (sg > 1.0f) sg = 1.0f;
                    if (sb > 1.0f) sb = 1.0f;
                    float dA = RgbDistance(sr, sg, sb, replaceColor);
                    float dB = RgbDistance(sr, sg, sb, keepColor);
                    float w = 0.5f + (dB - dA) / (2.0f * softness);
                    if (w < 0.0f) w = 0.0f;
                    if (w > 1.0f) w = 1.0f;
                    weight[i] = w;
                }
            }
        }

        std::vector<BYTE> scratch(pixelBytes);
        for (size_t vi = 0; vi < variantCount; ++vi) {
            const BYTE* src = (const BYTE*)pBits;
            BYTE* dst = scratch.data();
            if (!tinting) {
                memcpy(dst, src, pixelBytes);  // keep the cursor's own colors
            } else {
                // Replace the matched pixels with the tint. Interpolating in
                // premultiplied space avoids unpremultiplying per variant:
                // out = src + (tint*alpha - src) * weight.
                const Rgb& tint = tints[vi];
                const float tb = tint.b, tg = tint.g, tr = tint.r;
                for (size_t i = 0; i < pixelCount; ++i) {
                    const BYTE* s = src + i * 4;
                    BYTE* d = dst + i * 4;
                    float w = weight[i];
                    BYTE a = s[3];
                    d[0] = (BYTE)(s[0] + (tb * a - s[0]) * w + 0.5f);
                    d[1] = (BYTE)(s[1] + (tg * a - s[1]) * w + 0.5f);
                    d[2] = (BYTE)(s[2] + (tr * a - s[2]) * w + 0.5f);
                    d[3] = a;
                }
            }

            IWICBitmap* pWicBitmap = nullptr;
            hr = pWicFactory->CreateBitmapFromMemory(
                (UINT)targetW, (UINT)targetH, GUID_WICPixelFormat32bppPBGRA,
                (UINT)targetW * 4, (UINT)pixelBytes,
                scratch.data(), &pWicBitmap);
            if (SUCCEEDED(hr) && pWicBitmap) {
                ID2D1Bitmap* pBitmap = nullptr;
                hr = render.pDCRenderTarget->CreateBitmapFromWicBitmap(pWicBitmap, nullptr, &pBitmap);
                if (SUCCEEDED(hr) && pBitmap) {
                    D2D1_SIZE_U px = pBitmap->GetPixelSize();
                    entry.variants[vi].bitmap = pBitmap;
                    entry.variants[vi].width = px.width;
                    entry.variants[vi].height = px.height;
                }
                pWicBitmap->Release();
            }
        }
        pWicFactory->Release();
    }

    DeleteObject(hDib);
}

// Returns the cached tinted bitmap for a cursor handle at the given trail ratio
// (0 = head, 1 = tail), or nullptr. The ratio selects the nearest baked variant.
static const CachedCursorBitmap* GetCursorBitmap(HCURSOR hCursor, float ratio) {
    auto it = render.cursorBitmapCache.find(hCursor);
    if (it == render.cursorBitmapCache.end()) return nullptr;
    const CachedCursorVariants& c = it->second;
    if (c.variants.empty()) return nullptr;
    size_t n = c.variants.size();
    size_t idx = (n > 1) ? (size_t)floorf(ratio * (float)(n - 1) + 0.5f) : 0;
    if (idx > n - 1) idx = n - 1;
    if (!c.variants[idx].bitmap) return nullptr;
    return &c.variants[idx];
}

bool IsGameRunning() {
    HWND hwnd = GetForegroundWindow();
    
    if (!hwnd || hwnd == GetDesktopWindow()) {
        return false;
    }

    // Cache the desktop worker handles so we don't spam the Windows string table search literally 60 times a second
    static HWND s_hwndProgman = FindWindowW(L"Progman", NULL);
    static HWND s_hwndWorkerW = FindWindowW(L"WorkerW", NULL);
    
    if (hwnd == s_hwndProgman || hwnd == s_hwndWorkerW) {
        return false;
    }

    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN) {
            return true;
        }
    }

    RECT rcApp;
    GetWindowRect(hwnd, &rcApp);
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfo(hMonitor, &mi)) {
        bool isFullscreen = (rcApp.left <= mi.rcMonitor.left &&
                             rcApp.top <= mi.rcMonitor.top &&
                             rcApp.right >= mi.rcMonitor.right &&
                             rcApp.bottom >= mi.rcMonitor.bottom);
        if (isFullscreen) {
            RECT rcClip;
            if (GetClipCursor(&rcClip)) {
                int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                int vH = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                if ((rcClip.right - rcClip.left) < vW || (rcClip.bottom - rcClip.top) < vH) {
                    return true;
                }
            }
            
            CURSORINFO ci = { sizeof(CURSORINFO) };
            if (GetCursorInfo(&ci) && ci.flags == 0) {
                return true;
            }
        }
    }
    return false;
}

// Applies the smoothstep easing curve to t (expected in [0,1]).
static float Ease(float t) {
    return t * t * (3.0f - 2.0f * t);
}

// Rounds a float to the nearest LONG, away from zero.
static LONG RoundToLong(float v) {
    return (LONG)(v + (v >= 0.0f ? 0.5f : -0.5f));
}

// Blend model: each of the N-1 transitions gets a blend zone of width
// (blendWidth/100), centered on the boundary. Outside blend zones, pure
// colors; inside, the configured easing curve. With more than two colors the
// zones overlap, merging into a multi-color gradient. Reads the precomputed
// band boundaries from LoadSettings.
static void GetBlendedColor(float ratio, float& r, float& g, float& b) {
    const std::vector<Rgb>& colors = settings.activeColorsRGB;
    r = 0.0f; g = 0.0f; b = 0.0f;
    if (colors.empty()) return;
    if (colors.size() == 1) {
        r = colors[0].r; g = colors[0].g; b = colors[0].b;
        return;
    }

    int N = (int)colors.size();
    float half = settings.colorBlendHalf;

    // Check pure bands first
    for (int i = 0; i < N; ++i) {
        if (settings.colorBandEnd[i] > settings.colorBandStart[i] &&
            ratio >= settings.colorBandStart[i] && ratio <= settings.colorBandEnd[i]) {
            r = colors[i].r; g = colors[i].g; b = colors[i].b;
            return;
        }
    }

    // Ratio falls in one or more blend zones — find which ones
    int firstZone = -1, lastZone = -1;
    for (int i = 0; i < N - 1; ++i) {
        float center = (float)(i + 1) / (float)N;
        float start = center - half;
        float end   = center + half;
        if (ratio >= start && ratio <= end) {
            if (firstZone < 0) firstZone = i;
            lastZone = i;
        }
    }

    // Fallback — shouldn't happen, but just in case
    if (firstZone < 0) {
        r = colors[0].r; g = colors[0].g; b = colors[0].b;
        return;
    }

    float zoneStart = (float)(firstZone + 1) / (float)N - half;
    float zoneEnd   = (float)(lastZone + 1) / (float)N + half;
    if (zoneStart < 0.0f) zoneStart = 0.0f;
    if (zoneEnd > 1.0f) zoneEnd = 1.0f;
    float span = zoneEnd - zoneStart;
    if (span <= 0.0f) {
        r = colors[firstZone].r; g = colors[firstZone].g; b = colors[firstZone].b;
        return;
    }
    float frac = (ratio - zoneStart) / span;

    frac = Ease(frac);

    // Colors are evenly spaced within the blend region, so the index and
    // fraction are computed directly (no position array allocation).
    int numColors = lastZone - firstZone + 2;
    float scaled = frac * (float)(numColors - 1);
    size_t idx = (size_t)scaled;
    if (idx > (size_t)(numColors - 2)) idx = (size_t)(numColors - 2);
    float f = scaled - (float)idx;

    const auto& c0 = colors[firstZone + (int)idx];
    const auto& c1 = colors[firstZone + (int)idx + 1];
    r = c0.r + (c1.r - c0.r) * f;
    g = c0.g + (c1.g - c0.g) * f;
    b = c0.b + (c1.b - c0.b) * f;
}

// Interpolates a single float value across equal shares, using smoothstep
// (same as the color blend easing). Stops are evenly spaced, so the index and
// fraction are computed directly without building a position array.
static float InterpolateValues(const std::vector<float>& values,
                              float ratio) {
    if (values.empty()) return 1.0f;
    if (values.size() == 1) return values[0];

    size_t n = values.size();
    if (ratio <= 0.0f) return values.front();
    if (ratio >= 1.0f) return values.back();

    float scaled = ratio * (float)(n - 1);
    size_t idx = (size_t)scaled;
    if (idx > n - 2) idx = n - 2;
    float frac = Ease(scaled - (float)idx);

    return values[idx] + (values[idx + 1] - values[idx]) * frac;
}

static float InterpolateOpacity(float ratio) {
    float alpha = InterpolateValues(settings.opacityValues, ratio);
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    return alpha;
}




// Render the "Simple line" style: a thin polyline following the smoothed trail,
// with per-segment opacity fading from full opacity at the head (smoothed[0], nearest cursor)
// to transparent at the tail (smoothed.back()). Width, color and max opacity are user-configurable.
void RenderSimpleLineStyle(const std::vector<D2D1_POINT_2F>& smoothed) {
    if (smoothed.size() < 2) return;
    if (!render.pDCRenderTarget) return;

    if (!render.pSimpleLineBrush) {
        render.pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f), &render.pSimpleLineBrush);
        if (!render.pSimpleLineBrush) return;
    }
    if (!render.pStrokeStyle && render.pD2DFactory) {
        D2D1_STROKE_STYLE_PROPERTIES props = D2D1::StrokeStyleProperties(
            D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_ROUND, 10.0f,
            D2D1_DASH_STYLE_SOLID, 0.0f);
        render.pD2DFactory->CreateStrokeStyle(&props, nullptr, 0, &render.pStrokeStyle);
    }

    size_t segCount = smoothed.size() - 1;
    // Paint tail -> head so the newest segment is drawn last and stays on top
    // wherever the trail crosses itself.
    for (size_t i = segCount; i-- > 0;) {
        float ratio = (segCount > 1) ? (float)i / (float)(segCount - 1) : 0.0f;
        float alpha = InterpolateOpacity(ratio);
        float strokeWidth = InterpolateValues(settings.simpleLineWidths, ratio);
        if (strokeWidth < 0.5f) strokeWidth = 0.5f;

        float gr, gg, gb;
        GetBlendedColor(ratio, gr, gg, gb);

        render.pSimpleLineBrush->SetColor(D2D1::ColorF(gr * alpha, gg * alpha, gb * alpha, alpha));
        render.pDCRenderTarget->DrawLine(smoothed[i], smoothed[i + 1], render.pSimpleLineBrush,
                                    strokeWidth, render.pStrokeStyle);
    }
}

// Render the "Cursor ghost" style: a faded copy of the cursor image at every
// trail point. Each copy uses the exact cursor bitmap that was on screen when
// that point was sampled, so an image change (arrow -> I-beam) shows the old
// image toward the tail and the new one at the head, with a hard boundary.
void RenderCursorGhostStyle(const std::vector<D2D1_POINT_2F>& smoothed,
                            const std::vector<HCURSOR>& cursors,
                            const std::vector<float>& ratios) {
    if (smoothed.empty()) return;
    if (!render.pDCRenderTarget) return;

    size_t n = smoothed.size();
    // Paint tail -> head so the newest copy is drawn last and stays on top
    // wherever the trail crosses itself.
    for (size_t i = n; i-- > 0;) {
        HCURSOR h = (i < cursors.size()) ? cursors[i] : NULL;

        CursorGeom g;
        if (!GetCursorGeom(h, g)) continue;

        // On-screen pixel size for this cursor image (it may be smaller or larger
        // than the native bitmap, e.g. when the Windows cursor size is changed).
        int targetW = (int)floorf(g.bmWidth * g.dpiScaleX + 0.5f);
        int targetH = (int)floorf(g.bmHeight * g.dpiScaleY + 0.5f);
        if (targetW < 1) targetW = g.bmWidth;
        if (targetH < 1) targetH = g.bmHeight;

        float ratio = (i < ratios.size()) ? ratios[i] : 0.0f;

        EnsureCursorBitmap(h, targetW, targetH);
        const CachedCursorBitmap* cb = GetCursorBitmap(h, ratio);
        if (!cb) continue;

        float alpha = InterpolateOpacity(ratio);
        if (alpha <= 0.0f) continue;

        // Per-copy size multiplier, interpolated like the line width.
        float scale = InterpolateValues(settings.ghostSizes, ratio);
        if (scale <= 0.0f) continue;
        float drawW = (float)cb->width * scale;
        float drawH = (float)cb->height * scale;

        // Place the bitmap by its own hotspot so the copy lands exactly where
        // the cursor image was. Using the sample's own image geometry (rather
        // than the render thread's current offset) keeps the first copies of a
        // new cursor image aligned right after an image change. The hotspot is
        // scaled too, so resizing does not move the copy.
        float sx = (float)cb->width / (float)g.bmWidth;
        float sy = (float)cb->height / (float)g.bmHeight;
        float hx = g.hotspotX * sx * scale;
        float hy = g.hotspotY * sy * scale;

        // Snap to the pixel grid. The hotspot is often a half-pixel, which
        // would otherwise make DrawBitmap resample the cursor and blur the copy.
        float left = floorf(smoothed[i].x - hx + 0.5f);
        float top  = floorf(smoothed[i].y - hy + 0.5f);
        D2D1_RECT_F destRect = D2D1::RectF(left, top, left + drawW, top + drawH);
        D2D1_RECT_F srcRect = D2D1::RectF(0.0f, 0.0f,
                                          (FLOAT)cb->width, (FLOAT)cb->height);

        // Nearest neighbor keeps 1:1 copies crisp; linear avoids blocky edges
        // when a copy is scaled.
        D2D1_BITMAP_INTERPOLATION_MODE interp =
            (fabsf(scale - 1.0f) < 0.001f)
                ? D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                : D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;
        render.pDCRenderTarget->DrawBitmap(cb->bitmap, &destRect, alpha, interp, &srcRect);
    }
}

// Releases the brushes and render target owned by the current render target.
// render.pStrokeStyle is factory-owned, so it survives target recreation.
static void ReleaseRenderTargetResources() {
    if (render.pSimpleLineBrush) { render.pSimpleLineBrush->Release(); render.pSimpleLineBrush = nullptr; }
    if (render.pEffectBrush) { render.pEffectBrush->Release(); render.pEffectBrush = nullptr; }
    if (render.pDebugBrush) { render.pDebugBrush->Release(); render.pDebugBrush = nullptr; }
    if (render.pDebugBrushRed) { render.pDebugBrushRed->Release(); render.pDebugBrushRed = nullptr; }
    if (render.pDebugBrushGreen) { render.pDebugBrushGreen->Release(); render.pDebugBrushGreen = nullptr; }
    for (auto& kv : render.cursorBitmapCache) {
        for (auto& v : kv.second.variants) {
            if (v.bitmap) v.bitmap->Release();
        }
    }
    render.cursorBitmapCache.clear();
    if (render.pDCRenderTarget) { render.pDCRenderTarget->Release(); render.pDCRenderTarget = nullptr; }
}

// Drops cached cursor geometry/bitmaps that are neither the live cursor nor
// referenced by the current frame's trail. Guards against HCURSOR handle reuse
// after a custom cursor is destroyed.
static void PruneCursorCaches(const std::vector<HCURSOR>& used) {
    HCURSOR live = cursor.cachedCursor;
    auto keep = [&](HCURSOR h) {
        return h == live || std::find(used.begin(), used.end(), h) != used.end();
    };
    for (auto it = cursor.geomCache.begin(); it != cursor.geomCache.end(); ) {
        if (keep(it->first)) ++it;
        else it = cursor.geomCache.erase(it);
    }
    for (auto it = render.cursorBitmapCache.begin(); it != render.cursorBitmapCache.end(); ) {
        if (keep(it->first)) { ++it; }
        else {
            for (auto& v : it->second.variants) {
                if (v.bitmap) v.bitmap->Release();
            }
            it = render.cursorBitmapCache.erase(it);
        }
    }
}

// Expands a bounding box to include the given rect, or initializes it if unset.
static void GrowBBox(RECT& bbox, bool& hasBBox, LONG l, LONG t, LONG r, LONG b) {
    if (hasBBox) {
        if (l < bbox.left) bbox.left = l;
        if (t < bbox.top) bbox.top = t;
        if (r > bbox.right) bbox.right = r;
        if (b > bbox.bottom) bbox.bottom = b;
    } else {
        bbox.left = l; bbox.top = t; bbox.right = r; bbox.bottom = b;
        hasBBox = true;
    }
}

// Time-based eviction: drop samples older than settings.tailDuration, then cap the
// total count. Caller must hold runtime.historyMutex.
static void EvictByTime(DWORD now) {
    while (!runtime.history.empty() && (now - runtime.history.back().t) > (DWORD)settings.tailDuration)
        runtime.history.pop_back();
    const size_t kMaxSamples = (size_t)(settings.tailDuration);
    while (runtime.history.size() > kMaxSamples)
        runtime.history.pop_back();
}

// High-frequency cursor polling thread.
// Runs at runtime.sampleRate ms intervals (default 1 ms), pushes sampled positions
// into runtime.history when the trail is active. All D2D operations remain on the
// overlay/render thread — this thread only touches runtime.history (under mutex),
// GetCursorPos, and the atomic flags.
DWORD WINAPI PollThreadProc(LPVOID) {
    // Match the overlay thread's DPI awareness so coordinate spaces agree.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // Wait on the stop event with a runtime.sampleRate ms timeout to drive the loop.
    while (WaitForSingleObject(runtime.pollStopEvent, runtime.sampleRate) == WAIT_TIMEOUT) {
        // Respect the game-running flag set by SmearTimerProc, and the
        // enable/disable hotkey state.
        if (runtime.isGameRunning.load() || !runtime.trailEnabled.load()) continue;

        POINT pt;
        if (!GetCursorPos(&pt)) continue;

        // Snapshot the cursor image so each ghost copy keeps the exact image
        // that was on screen at sample time.
        CURSORINFO ci = { sizeof(CURSORINFO) };
        HCURSOR sampleCursor = NULL;
        if (GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING) && ci.hCursor) {
            sampleCursor = ci.hCursor;
        }

        // Compute the canvas offset (virtual-screen origin).
        int vX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vY = GetSystemMetrics(SM_YVIRTUALSCREEN);

        {
            std::lock_guard<std::mutex> lock(runtime.historyMutex);

            // === EVICTION — runs every tick, regardless of cursor movement ===
            // Must happen before the duplicate-skip so that old samples expire
            // even when the cursor is stationary (duplicate-skip would otherwise
            // continue before reaching eviction, freezing the trail).
            DWORD now = timeGetTime();

            // Track raw cursor movement so the size-based timeout resets on any
            // motion (for both styles), independent of how often samples are
            // pushed. Must run before the eviction below so it sees the update.
            if (runtime.lastCursorValid &&
                (pt.x != runtime.lastCursorPos.x || pt.y != runtime.lastCursorPos.y)) {
                runtime.lastMovementTime = now;
            }
            runtime.lastCursorPos = pt;
            runtime.lastCursorValid = true;

            if (settings.sizeBased) {
                if ((settings.sizeTimeout > 0 && runtime.lastMovementTime > 0 &&
                     now - runtime.lastMovementTime > settings.sizeTimeout) ||
                    runtime.cursorHidden.load()) {
                    if (!runtime.isFading) {
                        runtime.isFading = true;
                        size_t n = runtime.history.size();
                        if (n > 1) {
                            size_t idx = 0;
                            for (auto it = runtime.history.rbegin(); it != runtime.history.rend(); ++it, ++idx) {
                                it->t = now - settings.tailDuration + (DWORD)((float)idx / (float)(n - 1) * settings.tailDuration);
                            }
                        }
                    }
                    EvictByTime(now);
                } else {
                    runtime.isFading = false;
                    if (settings.isGhost) {
                        // Ghost size_based keeps a fixed copy count. Samples are
                        // already spaced by ghostSpawnDist at push time, so cap the
                        // history to settings.tailSize copies; the oldest drops as
                        // new copies are stamped. When the cursor stops, no new
                        // copies are pushed, so the existing ones stay put.
                        while (runtime.history.size() > (size_t)settings.tailSize)
                            runtime.history.pop_back();
                    } else {
                        // Distance-based eviction: walk from head (newest)
                        // backwards, accumulating pixel distance. Pop
                        // everything past where cumulative > settings.tailSize.
                        // This makes trail length independent of mouse DPI
                        // and cursor speed.
                        double cumulative = 0.0;
                        for (size_t i = 1; i < runtime.history.size(); ++i) {
                            double dx = (double)runtime.history[i].pos.x - (double)runtime.history[i-1].pos.x;
                            double dy = (double)runtime.history[i].pos.y - (double)runtime.history[i-1].pos.y;
                            cumulative += sqrt(dx * dx + dy * dy);
                            if (cumulative > settings.tailSize) {
                                while (runtime.history.size() > i)
                                    runtime.history.pop_back();
                                break;
                            }
                        }
                    }
                }
            } else {
                EvictByTime(now);
            }

            // === CURSOR HIDDEN — stop sampling so the trail fades out ===
            // Eviction above has already run, so the trail retracts over the
            // tail duration (size-based re-timestamped by the fade path). No
            // new samples are pushed until the cursor is shown again.
            if (runtime.cursorHidden.load()) {
                origin.lastCursorValid = false;
                continue;
            }

            // === TRAIL ORIGIN — choose the offset for this sample ===
            // The "Trail origin on cursor change" setting was removed; the
            // origin always glides smoothly to the cursor's visual center so the
            // trail head lands correctly after a cursor image change
            // (arrow → I-beam).
            POINT originOffset;
            if (settings.isGhost) {
                // Ghost copies are latched at the raw cursor position; the
                // renderer anchors each image by its own hotspot, so no trail
                // origin offset is applied here. This avoids using a stale
                // offset from the previous cursor image right after a change.
                originOffset = { 0, 0 };
            } else {
                POINT target;
                {
                    std::lock_guard<std::mutex> offsetLock(cursor.offsetMutex);
                    target = cursor.visualOffset;
                }

                // Accumulate raw cursor travel (screen px) for size_based.
                float dist = 0.0f;
                if (origin.lastCursorValid) {
                    float ddx = (float)(pt.x - origin.lastCursorPos.x);
                    float ddy = (float)(pt.y - origin.lastCursorPos.y);
                    dist = sqrtf(ddx * ddx + ddy * ddy);
                }
                origin.lastCursorPos = pt;
                origin.lastCursorValid = true;

                if (!origin.initialized) {
                    // Snap to the current offset on first use (no glide from 0,0).
                    origin.smoothedOffsetX = (float)target.x;
                    origin.smoothedOffsetY = (float)target.y;
                    origin.target = target;
                    origin.initialized = true;
                } else if (!origin.transitioning &&
                           (target.x != origin.target.x || target.y != origin.target.y)) {
                    // Target changed — start an ease-in-out transition from the
                    // current smoothed value.
                    origin.fromX = origin.smoothedOffsetX;
                    origin.fromY = origin.smoothedOffsetY;
                    origin.target = target;
                    origin.startTime = now;
                    origin.progressDist = 0.0f;
                    origin.transitioning = true;
                }

                // Advance progress: time-driven (time_based) or distance-driven
                // (size_based), over a third of the configured tail value.
                if (origin.transitioning) {
                    float p;
                    if (settings.sizeBased) {
                        origin.progressDist += dist;
                        float len = (float)(settings.tailSize / 3);
                        if (len < 1.0f) len = 1.0f;
                        p = origin.progressDist / len;
                    } else {
                        float len = (float)(settings.tailDuration / 3);
                        if (len < 1.0f) len = 1.0f;
                        p = (float)(now - origin.startTime) / len;
                    }
                    if (p > 1.0f) p = 1.0f;
                    float e = Ease(p);
                    origin.smoothedOffsetX = origin.fromX + (origin.target.x - origin.fromX) * e;
                    origin.smoothedOffsetY = origin.fromY + (origin.target.y - origin.fromY) * e;
                    if (p >= 1.0f) origin.transitioning = false;
                } else {
                    origin.smoothedOffsetX = (float)target.x;
                    origin.smoothedOffsetY = (float)target.y;
                }

                originOffset.x = RoundToLong(origin.smoothedOffsetX);
                originOffset.y = RoundToLong(origin.smoothedOffsetY);
            }

            POINT newPt = { pt.x + originOffset.x - vX,
                            pt.y + originOffset.y - vY };

            // === SPAWN / DUPLICATE-SKIP — only blocks push, not eviction ===
            // Eviction has already run above, so it is safe to continue here.
            if (!runtime.history.empty()) {
                if (settings.isGhost) {
                    // Ghost copies are latched at their spawn position, so only
                    // stamp a new copy once the cursor has travelled far enough
                    // from the last one. This keeps existing copies fixed in
                    // place instead of sliding with the cursor.
                    float dx = (float)(newPt.x - runtime.history.front().pos.x);
                    float dy = (float)(newPt.y - runtime.history.front().pos.y);
                    if (dx * dx + dy * dy < settings.ghostSpawnDist * settings.ghostSpawnDist)
                        continue;
                } else if (runtime.history.front().pos.x == newPt.x &&
                           runtime.history.front().pos.y == newPt.y) {
                    // Cursor hasn't moved since last sample — skip push.
                    continue;
                }
            }

            // === PUSH ===
            Sample s;
            s.pos = newPt;
            s.t = now;
            s.cursor = sampleCursor;
            runtime.history.push_front(s);
            runtime.lastMovementTime = now;
            runtime.isFading = false;
        }
    }
    return 0;
}

// Forward declaration: MMTimerCallback is defined later (before
// OverlayThreadProc). Forward-declare so the compiler knows the signature.
void CALLBACK MMTimerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

// Allocates/recreates the backbuffer bitmap when the screen size changes.
// Rebuilding the bitmap invalidates the render target, so it is released here.
static void EnsureBackbuffer(HDC hdcScreen, int vW, int vH) {
    if (!render.hBitmap || render.cachedVW != vW || render.cachedVH != vH) {
        if (render.hBitmap) DeleteObject(render.hBitmap);
        if (render.hdcMem) DeleteDC(render.hdcMem);

        render.hdcMem = CreateCompatibleDC(hdcScreen);
        render.hBitmap = CreateCompatibleBitmap(hdcScreen, vW, vH);
        SelectObject(render.hdcMem, render.hBitmap);

        render.cachedVW = vW;
        render.cachedVH = vH;

        if (render.pDCRenderTarget) {
            ReleaseRenderTargetResources();
        }
    }
}

// Creates the Direct2D render target and its brushes if not already present.
static void EnsureRenderTarget() {
    if (!render.pDCRenderTarget && render.pD2DFactory) {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT
        );

        render.pD2DFactory->CreateDCRenderTarget(&props, &render.pDCRenderTarget);
        if (render.pDCRenderTarget) {
            render.pDCRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White), &render.pDebugBrush);
            render.pDCRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Red), &render.pDebugBrushRed);
            render.pDCRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Lime), &render.pDebugBrushGreen);
            render.pDCRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White), &render.pEffectBrush);
        }
    }
}

// Snapshot runtime.history and build spatially-decimated trail points into
// smoothed, with a parallel cursor-handle vector (frozen per-copy images).
static void BuildTrailPoints(const POINT& pt, int vX, int vY,
                             std::vector<D2D1_POINT_2F>& smoothed,
                             std::vector<HCURSOR>& cursors,
                             std::vector<float>& ratios) {
    size_t kMaxPoints = TrailPointBudget();

    smoothed.reserve(kMaxPoints);
    cursors.reserve(kMaxPoints);

    // The front of the deque is the most recent sample (captured by the poll
    // thread at ~1ms intervals with the cursor-center offset already applied).
    // Using runtime.history.front() as the head guarantees monotonic ordering.
    std::lock_guard<std::mutex> lock(runtime.historyMutex);
    if (runtime.history.empty()) {
        // No history yet — fall back to the render thread's cursor position.
        // Read the origin offset under cursor.offsetMutex (nested inside
        // runtime.historyMutex — consistent lock order everywhere).
        POINT originOffset = { 0, 0 };
        if (!settings.isGhost) {
            std::lock_guard<std::mutex> offsetLock(cursor.offsetMutex);
            originOffset = cursor.visualOffset;
        }
        POINT headPt = { pt.x + originOffset.x - vX,
                         pt.y + originOffset.y - vY };
        smoothed.push_back(D2D1::Point2F(
            (float)headPt.x + settings.tailOffsetX,
            (float)headPt.y + settings.tailOffsetY));
        cursors.push_back(cursor.cachedCursor);
        if (settings.isGhost) ratios.push_back(0.0f);
        return;
    }

    if (settings.isGhost) {
        // Ghost copies are latched at their spawn position by the poll thread
        // (spaced by ghostSpawnDist), so draw every history sample directly and
        // apply no render-time decimation. That keeps each copy fixed in place
        // instead of sliding along with the moving head.
        DWORD now = timeGetTime();
        size_t n = runtime.history.size();
        ratios.reserve(n);
        size_t i = 0;
        for (const auto& s : runtime.history) {
            smoothed.push_back(D2D1::Point2F(
                (float)s.pos.x + settings.tailOffsetX,
                (float)s.pos.y + settings.tailOffsetY));
            cursors.push_back(s.cursor);

            float ratio;
            if (settings.sizeBased) {
                // Fixed copy count: fade head -> tail by list position.
                ratio = (n > 1) ? (float)i / (float)(n - 1) : 0.0f;
            } else {
                // Time based: fade by age so copies disappear in place.
                DWORD age = now - s.t;
                ratio = (float)age / (float)settings.tailDuration;
                if (ratio > 1.0f) ratio = 1.0f;
            }
            ratios.push_back(ratio);
            ++i;
        }
        return;
    }

    // Adaptive min distance: smaller for longer trails so decimation keeps
    // enough waypoints. Shared formula with the ghost spawn distance.
    float kMinDist = AutoPointSpacing(kMaxPoints);

    // Point 0: newest poll sample (front of deque).
    smoothed.push_back(D2D1::Point2F(
        (float)runtime.history.front().pos.x + settings.tailOffsetX,
        (float)runtime.history.front().pos.y + settings.tailOffsetY));
    cursors.push_back(runtime.history.front().cursor);

    // Spatial decimation: keep only points at least kMinDist pixels apart,
    // producing evenly-spaced waypoints for consistent Chaikin smoothing.
    D2D1_POINT_2F prev = smoothed[0];
    for (const auto& s : runtime.history) {
        if (smoothed.size() >= kMaxPoints) break;
        float sx = (float)s.pos.x + settings.tailOffsetX;
        float sy = (float)s.pos.y + settings.tailOffsetY;
        float dx = sx - prev.x;
        float dy = sy - prev.y;
        if (dx * dx + dy * dy >= kMinDist * kMinDist) {
            smoothed.push_back(D2D1::Point2F(sx, sy));
            cursors.push_back(s.cursor);
            prev = smoothed.back();
        }
    }
}

// Chaikin subdivision: smooths corners by inserting intermediate points,
// roughly doubling count per iteration (2 iterations). Used by the Simple line
// style; the ghost style stamps the decimated points directly so its copy count
// matches the configured size.
static void ChaikinSmooth(std::vector<D2D1_POINT_2F>& smoothed) {
    for (int iter = 0; iter < 2; ++iter) {
        if (smoothed.size() < 3) break;
        std::vector<D2D1_POINT_2F> next_s;
        next_s.reserve(smoothed.size() * 2);
        next_s.push_back(smoothed.front());
        for (size_t i = 0; i < smoothed.size() - 1; ++i) {
            D2D1_POINT_2F p0 = smoothed[i];
            D2D1_POINT_2F p1 = smoothed[i + 1];
            next_s.push_back(D2D1::Point2F(2.0f / 3.0f * p0.x + 1.0f / 3.0f * p1.x, 2.0f / 3.0f * p0.y + 1.0f / 3.0f * p1.y));
            next_s.push_back(D2D1::Point2F(1.0f / 3.0f * p0.x + 2.0f / 3.0f * p1.x, 1.0f / 3.0f * p0.y + 2.0f / 3.0f * p1.y));
        }
        next_s.push_back(smoothed.back());
        smoothed.swap(next_s);
    }
}

// Computes the trail's bounding box, expanded for stroke width / cursor size.
static void ComputeTrailBBox(const std::vector<D2D1_POINT_2F>& smoothed,
                             const std::vector<HCURSOR>& cursors, RECT& bbox) {
    float minX = smoothed[0].x, maxX = smoothed[0].x;
    float minY = smoothed[0].y, maxY = smoothed[0].y;
    for (const auto& p : smoothed) {
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    int margin = 32;
    if (settings.isGhost) {
        // Expand by the largest on-screen cursor image in the trail.
        float maxDim = 0.0f;
        for (HCURSOR h : cursors) {
            CursorGeom g;
            if (!GetCursorGeom(h, g)) continue;
            float w = g.bmWidth * g.dpiScaleX;
            float hgt = g.bmHeight * g.dpiScaleY;
            if (w > maxDim) maxDim = w;
            if (hgt > maxDim) maxDim = hgt;
        }
        if (maxDim < 1.0f) maxDim = 64.0f;
        maxDim *= settings.ghostSizeMax;
        margin = (int)maxDim + 16;
    } else if (!settings.simpleLineWidths.empty()) {
        float maxW = settings.simpleLineWidths[0];
        for (float w : settings.simpleLineWidths) {
            if (w > maxW) maxW = w;
        }
        margin = (int)maxW + 16;
    }
    bbox.left   = (LONG)minX - margin;
    bbox.top    = (LONG)minY - margin;
    bbox.right  = (LONG)maxX + margin;
    bbox.bottom = (LONG)maxY + margin;
}

// Dispatches to the active style renderer and updates the clear flag.
static void RenderTrail(const std::vector<D2D1_POINT_2F>& smoothed,
                        const std::vector<HCURSOR>& cursors,
                        const std::vector<float>& ratios) {
    if (smoothed.size() >= 2) {
        if (settings.isGhost) {
            RenderCursorGhostStyle(smoothed, cursors, ratios);
        } else {
            RenderSimpleLineStyle(smoothed);
        }
        // Future styles: add else-if branches here, e.g.
        // else if (settings.activeStyle == L"glow") { RenderGlowStyle(smoothed); }
        runtime.needsClear = true;
    } else {
        runtime.needsClear = false;
    }
}

// Draws the debug outline boxes and trail-start marker (when enabled).
static void DrawDebug(const POINT& pt, int vX, int vY,
                      const std::vector<D2D1_POINT_2F>& smoothed,
                      RECT& bbox, bool& hasBBox) {
    // White/red outline boxes around the detected cursor bitmap and its
    // visible (alpha-trimmed) pixels.
    if (settings.debugShowOutline && cursor.bmWidth > 0 && cursor.bmHeight > 0) {
        POINT centerOffset;
        {
            std::lock_guard<std::mutex> offsetLock(cursor.offsetMutex);
            centerOffset = cursor.centerOffset;
        }
        float boxW = cursor.bmWidth * cursor.dpiScaleX;
        float boxH = cursor.bmHeight * cursor.dpiScaleY;
        float boxCx = (float)(pt.x + centerOffset.x - vX);
        float boxCy = (float)(pt.y + centerOffset.y - vY);
        D2D1_RECT_F debugBox = D2D1::RectF(boxCx - boxW / 2.0f,
                                           boxCy - boxH / 2.0f,
                                           boxCx + boxW / 2.0f,
                                           boxCy + boxH / 2.0f);
        if (render.pDebugBrush) {
            render.pDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
            render.pDCRenderTarget->DrawRectangle(debugBox, render.pDebugBrush, 2.0f);
        }
        if (cursor.visibleValid && render.pDebugBrushRed) {
            float baseX = boxCx - boxW / 2.0f;
            float baseY = boxCy - boxH / 2.0f;
            D2D1_RECT_F visBox = D2D1::RectF(
                baseX + cursor.visLeft * cursor.dpiScaleX,
                baseY + cursor.visTop * cursor.dpiScaleY,
                baseX + cursor.visRight * cursor.dpiScaleX,
                baseY + cursor.visBottom * cursor.dpiScaleY);
            render.pDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
            render.pDCRenderTarget->DrawRectangle(visBox, render.pDebugBrushRed, 2.0f);
        }
        GrowBBox(bbox, hasBBox,
                 (LONG)debugBox.left - 1, (LONG)debugBox.top - 1,
                 (LONG)debugBox.right + 1, (LONG)debugBox.bottom + 1);
    }

    // Green "+" marking the exact trail start (head point).
    if (settings.debugShowOutline && !smoothed.empty() && render.pDebugBrushGreen) {
        float hx = smoothed[0].x;
        float hy = smoothed[0].y;
        const float half = 8.0f;
        render.pDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
        render.pDCRenderTarget->DrawLine(D2D1::Point2F(hx - half, hy),
                                    D2D1::Point2F(hx + half, hy), render.pDebugBrushGreen, 2.0f);
        render.pDCRenderTarget->DrawLine(D2D1::Point2F(hx, hy - half),
                                    D2D1::Point2F(hx, hy + half), render.pDebugBrushGreen, 2.0f);
        GrowBBox(bbox, hasBBox,
                 (LONG)hx - (LONG)half - 1, (LONG)hy - (LONG)half - 1,
                 (LONG)hx + (LONG)half + 1, (LONG)hy + (LONG)half + 1);
    }
}

// Blits the backbuffer to the overlay window via UpdateLayeredWindow, using a
// dirty rect when possible and falling back to full screen otherwise. Skips the
// blit entirely when there's nothing to show (prevents full-screen compositor
// updates every frame when the cursor is stationary).
static void BlitOverlay(HWND hwnd, HDC hdcScreen, int vX, int vY, int vW, int vH,
                        const RECT& curBBox, bool hasCurBBox) {
    if (!hasCurBBox && !runtime.hasPrevDirty && !runtime.needsClear) {
        ReleaseDC(NULL, hdcScreen);
        return;
    }

    // Dirty rect = union of current + previous bounding boxes, in backbuffer
    // coords (origin at 0,0 = virtual screen origin).
    RECT dirtyRect;
    bool useDirtyRect = false;

    if (hasCurBBox && runtime.hasPrevDirty) {
        dirtyRect.left   = (curBBox.left   < runtime.prevDirtyRect.left)   ? curBBox.left   : runtime.prevDirtyRect.left;
        dirtyRect.top    = (curBBox.top    < runtime.prevDirtyRect.top)    ? curBBox.top    : runtime.prevDirtyRect.top;
        dirtyRect.right  = (curBBox.right  > runtime.prevDirtyRect.right)  ? curBBox.right  : runtime.prevDirtyRect.right;
        dirtyRect.bottom = (curBBox.bottom > runtime.prevDirtyRect.bottom) ? curBBox.bottom : runtime.prevDirtyRect.bottom;
        useDirtyRect = true;
    } else if (hasCurBBox) {
        dirtyRect = curBBox;
        useDirtyRect = true;
    } else if (runtime.hasPrevDirty) {
        // No current trail, but previous frame had one — erase it.
        dirtyRect = runtime.prevDirtyRect;
        useDirtyRect = true;
    }

    if (useDirtyRect) {
        if (dirtyRect.left < 0) dirtyRect.left = 0;
        if (dirtyRect.top < 0) dirtyRect.top = 0;
        if (dirtyRect.right > vW) dirtyRect.right = vW;
        if (dirtyRect.bottom > vH) dirtyRect.bottom = vH;

        // 768x768 cap — if exceeded, fall back to full screen.
        int dirtyW = dirtyRect.right - dirtyRect.left;
        int dirtyH = dirtyRect.bottom - dirtyRect.top;
        if (dirtyW > 768 || dirtyH > 768 || dirtyW <= 0 || dirtyH <= 0) {
            useDirtyRect = false;
        }
    }

    // Update previous-frame tracking for the next tick.
    if (hasCurBBox) {
        runtime.prevDirtyRect = curBBox;
        runtime.hasPrevDirty = true;
    } else if (!runtime.needsClear) {
        runtime.prevDirtyRect = { 0, 0, 0, 0 };
        runtime.hasPrevDirty = false;
    }

    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    if (useDirtyRect) {
        POINT ptPos = { vX + dirtyRect.left, vY + dirtyRect.top };
        SIZE sizeWnd = { dirtyRect.right - dirtyRect.left,
                         dirtyRect.bottom - dirtyRect.top };
        POINT ptSrc = { dirtyRect.left, dirtyRect.top };
        UpdateLayeredWindow(hwnd, hdcScreen, &ptPos, &sizeWnd,
                            render.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    } else {
        POINT ptPos = { vX, vY };
        SIZE sizeWnd = { vW, vH };
        POINT ptSrc = { 0, 0 };
        UpdateLayeredWindow(hwnd, hdcScreen, &ptPos, &sizeWnd,
                            render.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    }

    ReleaseDC(NULL, hdcScreen);
}

// Starts the enable/disable circle effect. Runs on the overlay thread (from
// WM_HOTKEY). The cursor color is captured now; the center follows the cursor
// while the effect plays.
static void StartToggleEffect(bool enabling) {
    if (!Wh_GetIntSetting(L"hotkeyOptions.animate")) {
        return;  // animation disabled
    }

    toggleEffect.enabling = enabling;
    toggleEffect.startTime = timeGetTime();
    toggleEffect.color = { 1.0f, 1.0f, 1.0f };

    CURSORINFO ci = { sizeof(CURSORINFO) };
    if (GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING) && ci.hCursor) {
        Rgb color;
        if (GetCursorColor(ci.hCursor, color)) {
            toggleEffect.color = color;
        }
    }

    toggleEffect.active = true;
}

// Draws the toggle circle (2px outline) and grows bbox/hasBBox to cover it.
// The center is the trail head (smoothed[0]), so it matches the start of the
// trail exactly (including the origin glide and the ghost hotspot anchor), and
// it follows the cursor because the head is rebuilt every frame.
static void RenderToggleEffect(const std::vector<D2D1_POINT_2F>& smoothed,
                               RECT& bbox, bool& hasBBox) {
    if (!toggleEffect.active) return;
    if (!render.pDCRenderTarget || !render.pEffectBrush) return;
    if (smoothed.empty()) return;

    DWORD now = timeGetTime();
    float p = (float)(now - toggleEffect.startTime) / (float)kEffectDurationMs;
    if (p >= 1.0f) {
        toggleEffect.active = false;
        runtime.needsClear = true;   // erase the last circle on the next frame
        return;
    }
    if (p < 0.0f) p = 0.0f;

    // Cursor on-screen size; the circle diameter is 6x that, so the radius is
    // half of 6x.
    float cursorSize = 32.0f;
    if (cursor.bmWidth > 0 && cursor.bmHeight > 0) {
        float w = cursor.bmWidth * cursor.dpiScaleX;
        float h = cursor.bmHeight * cursor.dpiScaleY;
        cursorSize = (w > h) ? w : h;
    }
    float maxRadius = cursorSize * kEffectDiameterFactor * 0.5f;

    // Enable: ease out; disable: ease in. Kept local (not via Ease), which is
    // reserved for the smoothstep color/value blending.
    float eased = toggleEffect.enabling ? (1.0f - (1.0f - p) * (1.0f - p))
                                        : (p * p);
    float radius = toggleEffect.enabling ? maxRadius * (1.0f - eased)
                                         : maxRadius * eased;
    float alpha = toggleEffect.enabling ? eased : (1.0f - eased);

    float cx = smoothed[0].x;
    float cy = smoothed[0].y;

    const Rgb& c = toggleEffect.color;
    render.pEffectBrush->SetColor(D2D1::ColorF(c.r * alpha, c.g * alpha, c.b * alpha, alpha));
    render.pDCRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    render.pDCRenderTarget->DrawEllipse(
        D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius),
        render.pEffectBrush, kEffectStrokeWidth);

    float margin = radius + kEffectStrokeWidth + 1.0f;
    GrowBBox(bbox, hasBBox,
             (LONG)(cx - margin), (LONG)(cy - margin),
             (LONG)(cx + margin), (LONG)(cy + margin));
}

// --- Debug: static tail preview --------------------------------------------
// Draws a fixed horizontal preview of the trail's appearance to the left of the
// cursor, so it can be tuned without moving the mouse. It reuses the style
// renderers, so width/color/opacity/size/replace and the gradients all match the
// real trail; only the animated (position/age) parts are omitted.
static const float kPreviewLength = 500.0f;
static const float kPreviewOffsetY = 50.0f;
static const int   kPreviewLinePoints = 64;
static const int   kPreviewMaxCopies = 512;

static void RenderTailPreview(const POINT& pt, int vX, int vY,
                              RECT& bbox, bool& hasBBox) {
    if (!render.pDCRenderTarget) return;

    // Head sits flush at the cursor's x; the tail extends leftward.
    float rightX = (float)(pt.x - vX);
    float cy = (float)(pt.y - kPreviewOffsetY - vY);

    if (settings.isGhost) {
        HCURSOR h = cursor.cachedCursor;
        if (!h) return;
        CursorGeom g;
        if (!GetCursorGeom(h, g)) return;

        // Copy count: the configured Copies when size-based, otherwise as many
        // as fit over the preview length at the configured spawn spacing.
        int count;
        if (settings.sizeBased) {
            count = settings.tailSize;
        } else {
            float spacing = settings.ghostSpawnDist;
            if (spacing < 1.0f) spacing = 1.0f;
            count = (int)(kPreviewLength / spacing) + 1;
        }
        if (count < 2) count = 2;
        if (count > kPreviewMaxCopies) count = kPreviewMaxCopies;

        std::vector<D2D1_POINT_2F> points;
        std::vector<HCURSOR> cursors;
        std::vector<float> ratios;
        points.reserve(count);
        cursors.reserve(count);
        ratios.reserve(count);
        for (int i = 0; i < count; ++i) {
            float t = (float)i / (float)(count - 1);
            // Head at the right end, tail to the left.
            points.push_back(D2D1::Point2F(rightX - t * kPreviewLength, cy));
            cursors.push_back(h);
            ratios.push_back(t);
        }
        RenderCursorGhostStyle(points, cursors, ratios);

        float w = g.bmWidth * g.dpiScaleX * settings.ghostSizeMax;
        float hh = g.bmHeight * g.dpiScaleY * settings.ghostSizeMax;
        GrowBBox(bbox, hasBBox,
                 (LONG)(rightX - kPreviewLength - w), (LONG)(cy - hh),
                 (LONG)(rightX + w), (LONG)(cy + hh));
    } else {
        float maxWidth = 1.0f;
        for (float w : settings.simpleLineWidths) {
            if (w > maxWidth) maxWidth = w;
        }

        std::vector<D2D1_POINT_2F> points;
        points.reserve(kPreviewLinePoints);
        for (int i = 0; i < kPreviewLinePoints; ++i) {
            float t = (float)i / (float)(kPreviewLinePoints - 1);
            points.push_back(D2D1::Point2F(rightX - t * kPreviewLength, cy));
        }
        RenderSimpleLineStyle(points);

        float margin = maxWidth * 0.5f + 2.0f;
        GrowBBox(bbox, hasBBox,
                 (LONG)(rightX - kPreviewLength), (LONG)(cy - margin),
                 (LONG)(rightX), (LONG)(cy + margin));
    }

    runtime.needsClear = true;
}

VOID CALLBACK SmearTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(idEvent);

    POINT pt;
    GetCursorPos(&pt);

    // Periodically refresh the game-running detection and publish the result
    // to the polling thread via the atomic flag.
    if (dwTime - runtime.lastFullscreenCheck > 500) {
        bool gameNow = IsGameRunning();
        runtime.isGameRunning.store(gameNow);
        runtime.lastFullscreenCheck = dwTime;
    }

    // Read local copies of the atomic flags for consistent use within this
    // frame. The trail is suppressed (history cleared, overlay wiped) while a
    // fullscreen game runs or the enable/disable hotkey has it turned off.
    bool gameRunning = runtime.isGameRunning.load();
    bool suppress = gameRunning || !runtime.trailEnabled.load();
    bool effectActive = toggleEffect.active;
    // The static tail preview is independent of the enable/disable hotkey, but
    // stays paused while a fullscreen game runs.
    bool previewActive = settings.debugShowTailPreview && !gameRunning;

    int vX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vH = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;

    if (suppress) {
        // Keep the cursor geometry fresh so the toggle circle and the tail
        // preview can follow the cursor even while the trail itself is off.
        if (effectActive || previewActive) {
            UpdateCursorCenterOffset();
        }
        {
            std::lock_guard<std::mutex> lock(runtime.historyMutex);
            bool wasEmpty = runtime.history.empty();
            runtime.history.clear();
            if (wasEmpty && !runtime.needsClear && !effectActive && !previewActive) {
                return;
            }
        }
    } else {
        // Trail active — update cursor appearance caches. The poll thread
        // owns trail-origin selection (smooth glide to the cursor's visual
        // center) and sample accumulation.
        UpdateCursorCenterOffset();
    }

    // Snapshot the current history size to decide whether to draw.
    bool historyEmpty;
    {
        std::lock_guard<std::mutex> lock(runtime.historyMutex);
        historyEmpty = runtime.history.empty();
    }

    if (!historyEmpty || runtime.needsClear || effectActive || previewActive ||
        (settings.debugShowOutline && cursor.bmWidth > 0 && cursor.bmHeight > 0)) {
        HDC hdcScreen = GetDC(NULL);

        EnsureBackbuffer(hdcScreen, vW, vH);
        EnsureRenderTarget();

        RECT curBBox = { 0, 0, 0, 0 };
        bool hasCurBBox = false;

        if (render.pDCRenderTarget) {
            RECT rc = { 0, 0, vW, vH };
            render.pDCRenderTarget->BindDC(render.hdcMem, &rc);

            render.pDCRenderTarget->BeginDraw();
            render.pDCRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
            render.pDCRenderTarget->SetAntialiasMode(settings.antialiasing
                ? D2D1_ANTIALIAS_MODE_PER_PRIMITIVE
                : D2D1_ANTIALIAS_MODE_ALIASED);

            // Cache the live cursor's bitmap while it is still valid, so older
            // copies keep their exact image after the cursor changes.
            if (settings.isGhost && cursor.cachedCursor) {
                int tw = (int)floorf(cursor.bmWidth * cursor.dpiScaleX + 0.5f);
                int th = (int)floorf(cursor.bmHeight * cursor.dpiScaleY + 0.5f);
                EnsureCursorBitmap(cursor.cachedCursor, tw, th);
            }

            std::vector<D2D1_POINT_2F> smoothed;
            std::vector<HCURSOR> cursors;
            std::vector<float> ratios;
            BuildTrailPoints(pt, vX, vY, smoothed, cursors, ratios);

            if (smoothed.size() >= 2) {
                // Ghost stamps the decimated points directly so the copy count
                // matches the configured size; the line style smooths corners.
                if (!settings.isGhost) {
                    ChaikinSmooth(smoothed);
                }
                ComputeTrailBBox(smoothed, cursors, curBBox);
                hasCurBBox = true;
            }
            RenderTrail(smoothed, cursors, ratios);

            RenderToggleEffect(smoothed, curBBox, hasCurBBox);

            if (previewActive) {
                RenderTailPreview(pt, vX, vY, curBBox, hasCurBBox);
            }

            DrawDebug(pt, vX, vY, smoothed, curBBox, hasCurBBox);

            HRESULT hr = render.pDCRenderTarget->EndDraw();
            if (hr == D2DERR_RECREATE_TARGET) {
                ReleaseRenderTargetResources();
            }
            if (settings.isGhost) {
                PruneCursorCaches(cursors);
            }
        }

        BlitOverlay(hwnd, hdcScreen, vX, vY, vW, vH, curBBox, hasCurBBox);
    }
}

// Custom window proc for the overlay. Handles WM_TIMER (posted by the
// multimedia timer callback) by calling SmearTimerProc directly. Also handles
// the enable/disable hotkey (WM_HOTKEY) and hotkey re-registration on settings
// change (kMsgApplyHotkey). All other messages go to DefWindowProc. This keeps
// all rendering on the overlay thread while using the multimedia timer for
// non-coalesced wakeups.
LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_TIMER) {
        // Clear before rendering so a frame that arrives while this one is still
        // in flight can queue exactly one more render (bounded, no backlog).
        runtime.renderScheduled.store(false);
        SmearTimerProc(hwnd, uMsg, wParam, GetTickCount());
        return 0;
    }
    if (uMsg == WM_HOTKEY) {
        if ((int)wParam == kHotkeyId) {
            bool enabled = !runtime.trailEnabled.load();
            runtime.trailEnabled.store(enabled);
            StartToggleEffect(enabled);
            Wh_Log(L"Trail %s via hotkey", enabled ? L"enabled" : L"disabled");
        }
        return 0;
    }
    if (uMsg == kMsgApplyHotkey) {
        ApplyHotkey(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// Multimedia timer callback — runs on a system-managed thread. Does NOT
// call any D2D/window APIs directly. Just PostMessages the overlay window
// to wake the message loop on the overlay thread, which then runs
// SmearTimerProc via OverlayWndProc.
void CALLBACK MMTimerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
    UNREFERENCED_PARAMETER(uTimerID);
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(dwUser);
    UNREFERENCED_PARAMETER(dw1);
    UNREFERENCED_PARAMETER(dw2);

    // Coalesce: only post if no render is already pending. PostMessage does not
    // coalesce like SetTimer, so an unthrottled post every 8ms builds an
    // unbounded WM_TIMER backlog whenever a frame runs long.
    if (runtime.overlayHwnd && !runtime.renderScheduled.exchange(true)) {
        if (!PostMessage(runtime.overlayHwnd, WM_TIMER, 1, 0)) {
            // Window is gone or queue failed; clear so future renders aren't
            // permanently blocked.
            runtime.renderScheduled.store(false);
        }
    }
}

DWORD WINAPI OverlayThreadProc(LPVOID lpParam) {
    UNREFERENCED_PARAMETER(lpParam);

    // Direct2D demands COM to be initialized on this thread before it will talk to us
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // Tell Windows we aren't a blurry legacy piece of shit so mixed-DPI monitors don't fuck up the math
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &render.pD2DFactory);

    HINSTANCE hInstance = GetModuleHandle(NULL);
    // Keep in sync with kOverlayClass in the companion "Cursor trail helper -
    // always on top" mod, which finds this window to raise it above the taskbar.
    const wchar_t CLASS_NAME[] = L"SmearFrameOverlayClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = OverlayWndProc; 
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int screenW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenH = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;

    runtime.overlayHwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        CLASS_NAME,
        L"SmearOverlay",
        WS_POPUP,
        screenX, screenY, screenW, screenH,
        NULL, NULL, hInstance, NULL
    );

    if (!runtime.overlayHwnd) return 0;

    ShowWindow(runtime.overlayHwnd, SW_SHOWNA);

    // Register the enable/disable hotkey (no-op when the setting is empty).
    ApplyHotkey(runtime.overlayHwnd);

    // Start the high-frequency cursor polling thread.
    // The stop event is a manual-reset event, initially non-signalled.
    runtime.pollStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (runtime.pollStopEvent) {
        runtime.pollThread = CreateThread(NULL, 0, PollThreadProc, NULL, 0, NULL);
    }

    // Use a multimedia timer instead of SetTimer. Multimedia timers have ~1ms
    // resolution and are not coalesced like WM_TIMER, giving smoother animation
    // under load. The callback PostMessages the overlay window, keeping all
    // rendering on this thread.
    timeBeginPeriod(1);
    // Fixed render interval (8ms = ~125Hz). The render rate setting was removed
    // because it has no visible effect after decoupling sampling from rendering.
    // The poll thread samples at 1ms independently; this timer only controls
    // how often the overlay is redrawn.
    const int kRenderIntervalMs = 8;
    runtime.mmTimerId = timeSetEvent(kRenderIntervalMs, 1, MMTimerCallback, 0, TIME_PERIODIC);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Stop the polling thread gracefully before tearing down D2D resources.
    if (runtime.pollStopEvent) {
        SetEvent(runtime.pollStopEvent);
    }
    if (runtime.pollThread) {
        WaitForSingleObject(runtime.pollThread, 200);
        CloseHandle(runtime.pollThread);
        runtime.pollThread = NULL;
    }
    if (runtime.pollStopEvent) {
        CloseHandle(runtime.pollStopEvent);
        runtime.pollStopEvent = NULL;
    }

    // Clean up our massive GPU footprint before checking out
    ReleaseRenderTargetResources();
    if (render.pStrokeStyle) { render.pStrokeStyle->Release(); render.pStrokeStyle = nullptr; }
    if (render.pD2DFactory) { render.pD2DFactory->Release(); render.pD2DFactory = nullptr; }

    if (render.hBitmap) DeleteObject(render.hBitmap);
    if (render.hdcMem) DeleteDC(render.hdcMem);

    // Kill the multimedia timer if still running (may have been killed
    // already by WhTool_ModUninit) and restore default timer resolution.
    if (runtime.mmTimerId) {
        timeKillEvent(runtime.mmTimerId);
        runtime.mmTimerId = 0;
    }
    timeEndPeriod(1);

    UnregisterHotKey(runtime.overlayHwnd, kHotkeyId);
    DestroyWindow(runtime.overlayHwnd);
    UnregisterClass(CLASS_NAME, hInstance);

    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    runtime.threadHandle = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, &runtime.overlayThreadId);
    return TRUE;
}

void WhTool_ModUninit() {
    // Signal the polling thread to stop. The overlay thread's cleanup block
    // will also signal it and wait, but signalling here first ensures the poll
    // thread begins shutting down before the overlay window's WM_QUIT is posted.
    if (runtime.pollStopEvent) {
        SetEvent(runtime.pollStopEvent);
    }

    // Kill the multimedia timer BEFORE posting WM_QUIT. The timer posts
    // WM_TIMER messages at 125Hz; if left running, they flood the message
    // queue and starve WM_QUIT, causing the unload to hang forever.
    if (runtime.mmTimerId) {
        timeKillEvent(runtime.mmTimerId);
        runtime.mmTimerId = 0;
        Sleep(20);  // Let any in-flight MMTimerCallback fire and complete
    }

    if (runtime.overlayThreadId) {
        PostThreadMessage(runtime.overlayThreadId, WM_QUIT, 0, 0);
    }
    if (runtime.threadHandle) {
        DWORD waitResult = WaitForSingleObject(runtime.threadHandle, 5000);
        if (waitResult == WAIT_TIMEOUT) {
            // Safety net: if the overlay thread didn't exit cleanly in 5s,
            // force-terminate to avoid hanging Windhawk's unload.
            TerminateThread(runtime.threadHandle, 0);
        }
        CloseHandle(runtime.threadHandle);
        runtime.threadHandle = NULL;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();

    // Re-register the hotkey on the overlay thread, which owns the window and
    // its message queue (RegisterHotKey/UnregisterHotKey are thread-bound).
    if (runtime.overlayHwnd) {
        PostMessage(runtime.overlayHwnd, kMsgApplyHotkey, 0, 0);
    }
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
    timeBeginPeriod(1);

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
    timeEndPeriod(1);

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
// ==WindhawkMod==
// @id                  tourne-table-desktop-audio-visualizer
// @name                Tourne'Table [Audio Visualizer]
// @description         A real-time audio visualizer for the Windows desktop. Advanced settings without sacrificing resource efficiency. Near-headless rendering with CPU optimization for audio capture.
// @description:ru-RU   Аудиовизуализатор реального времени для рабочего стола Windows. Расширенные настройки без ущерба для экономии ресурсов. Практически безинтерфейсный (near-headless) поток рендеринга с оптимизацией процессора для захвата звука.
// @version             1.0.0
// @author              USER-TOURNE
// @github              https://github.com/USER-TOURNE
// @donateUrl           https://ko-fi.com/tourne
// @license             MIT
// @include             explorer.exe
// @architecture        x86-64
// @compilerOptions     -ldxgi -ld2d1 -ld3d11 -ldcomp -ldwmapi -ldwrite -lgdi32 -lshcore -lshlwapi -lole32 -lshell32 -lksuser -lwindowscodecs -lruntimeobject -lwindowsapp -luuid -luser32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# `Tourne'Table` **[Audio Visualizer]**
![Full Documentation Here](https://github.com/USER-TOURNE/Windhawk-Mods/blob/main/CHANGED%20READ%20ME%20%26%20UPDATE%20PUSH/README.md)
![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Windhawk-Mods/main/GIF/11.gif)

*Oscilloscope shape running live audio - bottom-left placement, blurred panel, single-pixel border.*

> **A real-time audio visualizer that lives on your Windows desktop.** 
> Built on the foundation of Salyts' Desktop Audio Visualizer, rebuilt around performance.

Play music. Bars dance on your wallpaper. That's the whole idea.

It listens to **whatever your PC is already playing** - Spotify, YouTube, a game, a call - and draws it behind your desktop icons. No virtual audio cable, no drivers, nothing to configure. It just picks up your system audio.

The original worked, but it ran *hot*. This project rebuilt how it draws itself: **same job, roughly half the CPU, 18 °C cooler**, plus a pile of new shapes, colors and controls.

## ABOUT THIS PROJECT

Built with an emphasis on lower resource consumption, more efficient rendering, better frame pacing, expanded visualization options, dynamic album-art integration, native Windows media info, deeper customization, and power-conscious idle behavior.

The goal is simple:

> **Make the desktop move with the music - without making the CPU move mountains to do it.**

---

## ◈ PERFORMANCE AT A GLANCE

| Metric | Salyts Original | Tourne'Table | Change |
|:--|--:|--:|--:|
| **Total CPU usage** | 13.77 % | **5.95 %** | **−56.8 %** |
| **Peak single-thread** | 78.06 % | **34.86 %** | **−55.3 %** |
| **CPU package power** | 53.09 W | **32.31 W** | **−39.1 %** |
| **CPU package temp** | 57.5 °C | **39.7 °C** | **−17.8 °C** |
| **Peak power draw** | 93.24 W | **40.44 W** | **−53 W** |

**Measured on an Intel Core Ultra 265KF:** CPU package temperature averaged **39.7 °C** *(peak 50 °C)*, with core temperatures averaging **35.4 °C**. The WPA trace puts the mod's own cost at **2.36 % of a single core**. When audio stops, rendering stops - not "slows down," *stops*.

![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Windhawk-Mods/main/GIF/10.gif)

*Oscilloscope shape running live audio - bottom-left placement, blurred panel, single-pixel border.*

Full methodology, raw numbers and honest caveats are further down.

---

## ◆ VISUAL STYLES

### 8 Shapes

| Shape | What it does |
|:--|:--|
| **Stereo** | Classic equalizer bars. Low notes left, high notes right. |
| **Mountain** | Peaks in the middle, tapers toward both edges. |
| **Mirror** | The opposite - grows from the outside edges inward. |
| **Wave** | Normal bars with a slow ripple rolling through them. |
| **Breathe** | A gentle swell that rises with the music instead of jumping. |
| **Dots** | Stacked dots instead of solid bars - old LED-meter look. |
| **Radial** | Bars shoot outward from a center point, like a sunburst. |
| **Oscilloscope** | A single line tracing the actual sound wave. |

![Oscilloscope closeup](https://raw.githubusercontent.com/USER-TOURNE/Windhawk-Mods/main/GIF/8.gif)

*Closeup of the same setup - the waveform trace drawn as a continuous line.*

### 9 Color Modes

| Mode | What it does |
|:--|:--|
| **Solid** | One flat color. |
| **Gradient** | Fades between two colors across the bars. |
| **Reactive Gradient** | Shifts toward the second color as things get *louder*. |
| **Windows Accent** | Matches your Windows accent color, updating instantly. |
| **Album Art** | Pulls the dominant color from the playing track's cover. |
| **Dynamic Album** | Gradient between the cover art's two strongest colors. |
| **Acrylic** | Grows more opaque the louder it gets - near-transparent in silence. |
| **Rainbow Cycle** | Continuously cycling hue, adjustable speed. |
| **Tourne** | Built-in teal → red gradient from my personal palette. |

### Plus

- **2 orientations** - bars grow vertically or horizontally
- **3 anchors** - grow from the Bottom, the Top, or **both directions from the Middle**
- **Peak Hold Caps** - thin markers hang at each bar's recent peak and slowly fall *(classic hardware EQ)*
- **Beat Flash** - bars brighten on detected bass hits, on top of any color mode
- **Multiband Oscilloscope** - the waveform tints toward whichever part of the spectrum is loudest

---

## ♪ NOW PLAYING DISPLAY

Shows the current **artist and title** above the visualizer, pulled from the same Windows media session that powers your volume popup. Works with Spotify, browsers, and most media players.

Fades in on track change, fades out after a configurable delay. Custom color, font family and size.

---

![Oscilloscope closeup](https://raw.githubusercontent.com/USER-TOURNE/Windhawk-Mods/main/GIF/7.gif)

*Zoomed view of the waveform trace.*

## ◐ THE PALETTE

This project follows my personal theming palette - reflected in the repo screenshots. It can be changed to any hex / RGB / RGBA value you want.

| Name | Hex | RGB |
|:--|:--|:--|
| Green | `#15ffa2` | `21, 255, 162` |
| Red | `#ff8f8f` | `255, 143, 143` |
| Pale Green | `#64aa89` | `100, 170, 137` |
| Alt Pale Green | `#6ba6a0` | `107, 166, 160` |
| Pale Red | `#a95d5d` | `169, 93, 93` |
| Dark Teal *(background)* | `#121c21` | `18, 28, 33` |

The built-in **Tourne** color mode is drawn from these.

---

# ✦ EVERY SETTING, EXPLAINED

**You don't need any of this to use the mod.** The defaults work. This is for when you want to tweak something and you're wondering what a word means.

## Appearance

**Shape** - Which of the 8 styles above to draw.

**Orientation** - *Horizontal* = a row of bars growing up and down. *Vertical* = a column growing left and right.

**Bar Count** - How many bars. More = finer detail, wider visualizer. Range **1–2048** (enough to span a 4K or ultrawide screen).

**Bar Width** - How fat each bar is, in pixels.

**Bar Gap** - Space between bars, in pixels. Set to `0` and they touch.

**Bar Max Size** - How tall a bar gets at full volume. This is the overall height of the visualizer.

**Bar Idle Size** - How tall bars sit in silence. `0` makes them vanish completely; a few pixels leaves a thin resting line.

**Bar Corner Radius** - How rounded the bar corners are. One number rounds all four equally, or give four numbers separated by spaces for individual control: `top-left top-right bottom-right bottom-left`. Example: `5 5 0 0` rounds only the top.

**Color Mode** - Which of the 9 coloring styles above to use.

**Color** - The color used in Solid mode. Format is `#AARRGGBB` or `#RRGGBB` - that's **A**lpha (transparency), then **R**ed, **G**reen, **B**lue in hex. Lower the first two digits to make it see-through.

**Gradient Color 1 / 2** - Start and end colors for the gradient modes.

**Sensitivity** - How hard the bars react. Too low and quiet music barely moves them; too high and everything slams to max. Range 0–300. Turn it down for bass-heavy tracks, up for quiet recordings.

**EQ Preset** - Which frequencies get emphasized *visually*. Doesn't touch your actual audio.
`Default` no adjustment · `Bass` boosts lows · `Rock` boosts mids and highs · `Pop` heavy on highs · `Jazz` warmer, gentler highs · `Electronic` boosts bass and treble, scoops the middle.

**FFT Size** - How finely sound gets analyzed. An FFT is the math that splits audio into separate frequencies - think of it as sorting sound into buckets by pitch. More buckets = finer detail, slightly more CPU.
`1024` fastest, plenty for most · `2048` / `4096` noticeably crisper · `8192` maximum detail.

> **Note - this is an admittedly 'beta' implementation for now.** Everything up to `8192` runs near-flawlessly with almost no overhead, *with the caveat that you're using anything other than the Oscilloscope shape.*
>
> **One more note:** the higher the FFT Size, the more accurate the Sensitivity slider becomes for your specific audio setup. The correlation generally runs: **as FFT Size goes up, your Sensitivity will need to go up too.** For now that means fine-tuning Sensitivity per EQ Preset *and* per FFT Size, for your particular placement, size and personal adjustments.

**Frequency Scale** - How the frequency range spreads across the bars. This matters more than it sounds.
- **Log** - the natural-feeling default. Gives bass and treble roughly equal visual space, matching how humans hear pitch.
- **Linear** - spreads by raw Hz. Since most musical energy lives low, this crams all the action into a sliver on the left and leaves the right mostly dead. Technically accurate, visually dull.
- **Mel** - uses the *mel scale*, built from research on how people actually perceive pitch. Like Log, tuned to human hearing.

**Peak Frequency Readout** - Shows the loudest note as a live number, e.g. `1.2 kHz`.

**Peak Readout Position** - Where that number sits. Horizontal: Left / Center / Right. Vertical: Above / Top / Middle / Bottom / Below.
> **Above** and **Below** place it fully *outside* the bars so it never overlaps the visualization.

**Multiband Oscilloscope Coloring** - Only affects the Oscilloscope shape. The line tints toward whatever part of the spectrum is loudest - warm for bass, green for mids, blue for treble. Overrides Color Mode for that shape.

**Anchor** - Which edge bars grow from. `Bottom` rise upward *(classic)* · `Top` hang downward · `Middle` grow **both directions** from a center line.

**Peak Hold Caps** - Leaves a thin marker floating at each bar's recent peak, which slowly drifts down.

**Beat Flash** + **Intensity** - Flashes bars brighter on bass hits. Intensity controls how hard.

**Rainbow Cycle Speed** - How fast the rainbow rotates. Rainbow mode only.

**Now Playing Text** - Toggles the artist/title display.

**Now Playing Color / Font / Font Size** - Styling for that text. The font must be **installed on your system** - type the exact family name. Windows silently falls back to a default on a typo rather than erroring, so double-check spelling if nothing changes.
> The Peak Frequency Readout shares these same font settings.

**Now Playing Display Seconds** - How long the text stays up after a track change before fading.

## Position

**Horizontal Position** - Left-to-right placement as a percentage. `0` hard left, `50` centered, `100` hard right.

**Vertical Position** - Top-to-bottom, same idea. `0` top, `100` bottom.

**Monitor** - Which screen to draw on. `1` is your first monitor.

## Background

**Enabled** - Draws a panel behind the bars. Turn off for bars floating directly on the wallpaper.

**Color** - Panel color in `#AARRGGBB`. The alpha controls transparency.

**Padding** - Breathing room between the bars and the panel edge.

**Corner Radius** - How rounded the panel corners are. Same one-or-four-value rules as bar radius.

**Blur** - Frosted-glass blur of your wallpaper behind the panel. `0` disables.
> This used to be the single most expensive setting in the mod. It's now computed once and cached, so it's essentially free per frame.

**Border Size / Border Color** - A thin outline around the panel. `0` for none.

## Performance

**Target FPS** - How many times per second it redraws. Higher = smoother, more CPU. Little point exceeding your monitor's refresh rate.

**Pause On Fullscreen** - Stops completely when a fullscreen app runs. Detects both true fullscreen *(games)* and borderless windows. Since the visualizer lives on the desktop it's invisible anyway - this just stops it burning power. Resumes automatically.

**Pause When Silent (seconds)** - After this long without audio, drops to a trickle instead of full speed. `0` disables.

**Auto-Hide When Idle** + **Delay** - Fades out entirely after prolonged silence. Once fully faded it **stops rendering completely** - not just invisible, genuinely doing nothing until audio returns.

**Pause When Covered** - Stops rendering *and* audio capture while fully hidden behind another window.
> **Off by default.** Reliably detecting "am I covered?" on Windows 11 is genuinely tricky - the shell is full of invisible windows that report themselves as visible. The check is deliberately conservative (only a fully-covering, real application window counts), but if the visualizer ever vanishes when it shouldn't, this is the switch to flip.

---

![Oscilloscope closeup](https://raw.githubusercontent.com/USER-TOURNE/Windhawk-Mods/main/GIF/4.gif)

*Zoomed view at a different scale.*

# ▲ WHAT ACTUALLY GOT FIXED

In rough order of measured impact.

### 1. Precision frame pacing - the biggest single win

The original paced itself with `DwmFlush()`, which blocks until the monitor's next refresh. That meant the render thread woke **on every vertical blank, forever** - 60, 144, 240+ times a second - regardless of target FPS, whether anything needed redrawing, or whether the visualizer was even visible.

This barely registers as CPU% in Task Manager, because the thread is blocked, not spinning. But every wake-up drags a core out of deep idle. Do that continuously and the core never settles into its efficient sleep states - which reads as a small, permanent bump in package power and temperature. The classic "low usage, still runs warm" signature.

> **Note:** this becomes exponentially more noticeable on AMD architecture.
>
> **Note:** also exponentially more noticeable if you have **C-States disabled** in your BIOS or elsewhere. Shoutout to Process Lasso, Core Director, Park Control and HWiNFO64 for helping me debug why the hell all my E-cores were sitting at 65–70 °C when they were supposed to be idle during initial testing with Salyts' original mod.

**Fixed with** a high-resolution waitable timer firing only at the configured rate. Plain `Sleep()` wasn't good enough - it's quantized to ~15.6 ms, which would turn a 60 FPS target into stuttery 30–40 FPS.

### 2. Pre-rendered background blur

A Gaussian blur is a full-image convolution - the most expensive thing Direct2D does in this scene. The original recomputed it **from scratch every frame**, despite its input (your wallpaper) never changing.

**Fixed by** computing it exactly once into a cached bitmap, then just copying that each frame. The cache covers only the widget's bounding box, replacing roughly **8 MB of video memory with tens of KB**. It re-bakes automatically if the widget moves or resizes.

### 3. Widget-sized render surface

The render surface spanned the **entire desktop** even though the visualizer occupies a thin strip. Every frame cleared and presented millions of untouched pixels, with two full-desktop buffers parked in VRAM.

**Fixed by** sizing the surface to the widget's bounding box and offsetting the composition layer to position it. Cuts per-frame pixel work and VRAM by roughly an order of magnitude.

### 4. Cached geometry

The background panel and border were rebuilt from scratch every frame - allocating a path geometry, constructing four lines and four arcs by hand, then discarding it. Now rebuilt only when size, padding, radii or border width actually change. In normal use, almost never.

### 5. Cached monitor lookup

Every frame called `EnumDisplayMonitors()` - a real round-trip through the display driver stack - to work out which monitor to draw on. Now resolved once and cached, refreshed on display change.

### 6. Reduced frame latency

DXGI queues up to three frames ahead by default. For a passive widget that's pure latency and power draw with no upside. Capped at **1**.

### 7. Genuine idle shutdown

**Auto-Hide** now stops rendering completely once faded - presents one blank frame, then exits the render path entirely. **Pause When Covered** stops rendering and capture while hidden, checked once per second rather than per frame.

---

![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Windhawk-Mods/main/GIF/2.gif)

*Oscilloscope shape running live audio - bottom-left placement, blurred panel, single-pixel border.*

# ▦ THE FULL BENCHMARK DATA

Two independent measurement methods, both on an **Intel Core Ultra 265KF**.

## Method 1 - HWiNFO64 sensors

Identical 3-minute runs: 1 min silent → 1 min 30 s audio → 30 s silent.

### Stability - the less obvious win

Averages only tell half the story. The **spikiness** dropped even harder:

| Metric | Before | After |
|:--|--:|--:|
| CPU - median | 11.40 % | **5.50 %** |
| CPU - 95th percentile | 23.97 % | **8.10 %** |
| CPU - maximum | 34.40 % | **12.20 %** |
| CPU - standard deviation | 5.36 | **1.45** |
| Power - 95th percentile | 71.32 W | **36.78 W** |
| Power - maximum | 93.24 W | **40.44 W** |
| Power - standard deviation | 9.85 | **2.57** |

Standard deviation fell ~73 % on CPU and ~74 % on power. The original wasn't just heavier on average - it worked in **bursts**, and bursts are what drive thermal spikes and fan ramping.

That matches the root cause the profiler found: a render thread waking on every vsync, and a full-image blur re-evaluated every frame. Both bursty, repetitive workloads - exactly the profile that produces this variance.

### GPU - unchanged, as expected

| Metric | Before | After |
|:--|--:|--:|
| GPU core load | 7.66 % | 7.92 % |
| GPU D3D usage | 7.41 % | 7.20 % |
| GPU power | 21.81 W | 21.24 W |
| GPU temperature | 35.6 °C | 36.5 °C |

**These differences are inside measurement noise - don't read them as real changes in either direction.** A flat GPU reading is exactly the right outcome here: this workload was never GPU-bound. It sits at 7–8 % in both builds. The blur fix moved work off the CPU-side Direct2D path; it was never going to show as a GPU reduction at this scale.

I'm still working on the GPU side - I'd like both CPU and GPU sitting at a 3 % ceiling. It's already better than these numbers show; `.etl` traces are just enormous and parsing them means fighting a Windows tool currently stranded in a dead preview branch. Forgive me.

### Memory

Sensor logs only report system-wide memory, which includes every other application running - so those totals say nothing useful about this mod and aren't reproduced here.

What *is* known, from the changes themselves: the cached blur dropped from a full-desktop bitmap to a widget-sized one - roughly **8 MB of video memory replaced by tens of KB** at 1080p - and the render surface went from two full-desktop buffers to two widget-sized ones, cutting that allocation by an order of magnitude.

## Method 2 - Windows Performance Analyzer

Same protocol, normalized per second of runtime:

| | Salyts Original | Tourne'Table |
|:--|--:|--:|
| CPU time attributed to mod | 7,736.60 ms | 4,251.18 ms |
| Trace duration | 176.74 s | ~180 s |
| **Normalized cost** | **43.77 ms/sec** | **23.62 ms/sec** |
| As % of one core | 4.38 % | **2.36 %** |
| **Reduction** | - | **−46.0 %** |

### Why −46 % understates it, and how I handicapped myself to show the gains ♥

The two exports came from different WPA tables measuring different things:

- **Salyts'** is the *Sampled* table grouped by Module. It counts only samples where the CPU was executing **inside his DLL itself** - *exclusive* time. It does **not** include time his code spent inside `d2d1.dll` doing the actual drawing.
- **Tourne'Table's** is the *Precise* table with call stacks - *inclusive* time, counting everything downstream, D2D and kernel included.

Since the overwhelming majority of this workload's cost lives inside `d2d1.dll` rather than the mod's own logic, the original's true inclusive cost would be substantially higher than 7,736 ms. **My all-in number is being compared against his self-time-only number, and still comes out 46 % lower.**

### And my build was doing *a lot* more work

I ran an **older build of mine** for these tests - one with unfixed and notably half-implemented features, and considerably fewer optimization passes behind it than the current release.

It was still carrying all of the following, none of which exist in Salyts' original that it was tested against:

- **FFT size 2048** - double the original's fixed 1024, so twice the samples per analysis pass
- **Mel frequency scaling** - extra per-bar warp computation every frame
- **Peak hold caps** - additional per-bar state and draw calls
- **Beat flash** - per-frame transient detection and color modulation
- **Now Playing text** - live DirectWrite text rendering
- **Reactive gradient** vs. the original's flat solid color

Everything else was closely matched: 99 bars, 2 px wide, 1 px gap, middle anchor, ~144 target FPS, blur 33 vs 35, 1 px border, same position. **`pauseWhenObscured` was off**, so the newest optimization contributed nothing here.

### A miss I'll own: Auto-Hide wasn't helping

My test run had Auto-Hide on with a 45-second delay, which fired during both silent stretches. That did **not** give me an unfair advantage - quite the opposite. It was a broken implementation I slapped together on no sleep, and it *cost* me efficiency during my own benchmark. Oops.

The version I benchmarked faded opacity toward zero but **kept rendering the full scene underneath**, plus an extra `PushLayer`/`PopLayer` pair. Once faded it was doing strictly *more* work while showing nothing.

I genuinely missed an obvious optimization before running the comparison. **It's fixed now** - when the scene is fully transparent, rendering is skipped entirely instead of drawn and then hidden. It was built that way originally to shave frame time on scene wake, and I clawed that 0.08 ms back elsewhere.

## Honest caveats

The large deltas are far outside anything noise could explain, but the methodology has real limits:

1. **System-wide, not process-isolated** - HWiNFO measures the whole machine, and the two runs were ~3 minutes apart. The CPU/power/thermal deltas are far too large to be explained this way, but these aren't clean attributions to the mod alone. *(This applies to the HWiNFO numbers only - the WPA traces cover exactly where sensor testing falls short.)*
2. **Small sample size** - ~37 samples per run. Fine for headline effects, not enough to resolve anything under a few percent.
3. **Single run each** - no repeats, so run-to-run variance is unknown.
4. **I handicapped myself on conditions.** Salyts' build was tested on a fully idle system. Mine was tested while screen recording, opening and closing windows, and actively working in applications - which meant Windows 11 did Windows 11 things and spiked clocks via its newer app-launch optimizations.

---

![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Windhawk-Mods/main/GIF/3.gif)

*Oscilloscope shape running live audio - bottom-left placement, blurred panel, single-pixel border.*

## ♥ CREDITS

**[USER-TOURNE](https://github.com/USER-TOURNE)** - Author and maintainer: performance work, new features, benchmarking and documentation.

**[Salyts](https://github.com/Salyts)** - Original author of Desktop Audio Visualizer. This project exists because the foundation was good enough to be worth optimizing. Author of his own mod and repo; a contributor toward this one.

**[GR0UD](https://github.com/GR0UD)** - Audio visualizer code the original was adapted from. Author of his own work; a contributor toward this one.

### A coincidence worth acknowledging

While I was midway through my initial testing, **NeiZ** (author/maintainer, with **SuperSmile123** contributing) released [Desktop Audio Visualizer Plus](https://github.com/ramensoftware/windhawk-mods/commit/e01d0d0dbd6204804235831fd7f68821e4614028) (`neiz-supersmile-audio-visualizer`). I had planned to publish my own commit that same night - holy coincidence.

His release spurred another round of testing on my end, and I've since gone through roughly twelve more iterations. I didn't want to ship something that essentially achieved what I was already going for, especially if they'd figuratively led me out to pasture to put a bullet in me - aha.

To be explicit about attribution: **I did not borrow from or reference NeiZ or SuperSmile123's work at any point**, other than benchmarking theirs for efficiency to decide whether continuing development was worth it. Thanks to them and their contributors regardless.

---

## ◘ LICENSE

Released under the **MIT License**.

```
MIT License

Copyright (c) 2026 USER-TOURNE

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

### In the spirit of the WTFPL

> **0. You just do what the fuck you want to.**
>
> Take it. Fork it. Gut it. Rewrite the parts I got wrong. Ship it. Sell it.
> Learn something from it and never speak to me again.
>
> The only thing MIT actually asks is that the copyright line rides along -
> keep the notice, and we're square.
>
> *(I went with MIT over the real WTFPL for one boring reason: this thing
> injects a DLL into `explorer.exe`, and MIT comes with a warranty disclaimer.
> The WTFPL does not. Same energy, fewer ways for my life to get complicated.)*

### Third-party notices

This project builds on upstream work by **Salyts** and **GR0UD**. Their original
code carries its own license terms - if you redistribute this, carry their notices
along with mine.

---

## ❖ SUPPORT

If this saved you some degrees, some watts, or just made your desktop nicer to
look at, you can throw something in the hat. Entirely optional, genuinely appreciated.

**[ko-fi.com/tourne](https://ko-fi.com/tourne)**
*/
// ==/WindhawkModReadme==
 
// ==WindhawkModSettings==
/*
- appearance:
    - shape: stereo
      $name: Shape
      $description: Visual style of the bars
      $options:
        - stereo: Stereo
        - mountain: Mountain
        - mirror: Mirror
        - wave: Wave
        - breathe: Breathe
        - dots: Dots
        - radial: Radial
        - oscilloscope: Oscilloscope
    - orientation: horizontal
      $name: Orientation
      $options:
        - horizontal: Horizontal
        - vertical: Vertical
    - barCount: 32
      $name: Bar Count
    - barWidth: 6
      $name: Bar Width
    - barGap: 4
      $name: Bar Gap
    - barMaxSize: 140
      $name: Bar Max Size
    - barIdleSize: 4
      $name: Bar Idle Size
    - barCornerRadius: '3'
      $name: Bar Corner Radius
      $description: One value for all corners, or four space-separated values for top-left, top-right, bottom-right, bottom-left
    - colorMode: solid
      $name: Color Mode
      $options:
        - solid: Solid
        - gradient: Gradient
        - reactive_gradient: Reactive Gradient
        - accent: Windows Accent Color
        - album_art: Album Art Color
        - dynamic_album: Dynamic Album Gradient
        - acrylic: Acrylic
        - rainbow: Rainbow Cycle
        - tourne: Tourne (Teal/Red)
    - color: '#FFFFFFFF'
      $name: Color
      $description: Used when Color Mode is Solid. Format is #AARRGGBB or #RRGGBB
    - gradientColor1: '#FF1ED760'
      $name: Gradient Color 1
    - gradientColor2: '#FF00B4FF'
      $name: Gradient Color 2
    - sensitivity: 150
      $name: Sensitivity
    - eqPreset: default
      $name: EQ Preset
      $options:
        - default: Default
        - bass: Bass
        - rock: Rock
        - pop: Pop
        - jazz: Jazz
        - electronic: Electronic
    - fftSize: '1024'
      $name: FFT Size
      $description: Higher values give finer frequency detail at a small extra CPU cost
      $options:
        - '1024': 1024 (Fastest)
        - '2048': '2048'
        - '4096': '4096'
        - '8192': 8192 (Most Detailed)
    - freqScale: log
      $name: Frequency Scale
      $description: How bar position maps to frequency across the spectrum
      $options:
        - log: Log (natural, matches previous behavior)
        - linear: Linear (Hz-even spacing)
        - mel: Mel (perceptual pitch spacing)
    - peakFreqEnabled: false
      $name: Peak Frequency Readout
      $description: Shows the current dominant frequency as a small numeric overlay
    - peakFreqAlignH: right
      $name: Peak Readout - Horizontal Position
      $options:
        - left: Left
        - center: Center
        - right: Right
    - peakFreqAlignV: top
      $name: Peak Readout - Vertical Position
      $description: Above and Below place it fully clear of the bars or waveform
      $options:
        - above: Above (outside)
        - top: Top (inside)
        - middle: Middle
        - bottom: Bottom (inside)
        - below: Below (outside)
    - oscilloscopeMultibandEnabled: false
      $name: Multiband Oscilloscope Coloring
      $description: Tints the Oscilloscope trace by which part of the spectrum (low/mid/high) is currently dominant, overriding Color Mode for that shape
    - verticalAnchor: bottom
      $name: Anchor
      $description: Where bars grow from
      $options:
        - top: Top
        - middle: Middle
        - bottom: Bottom
    - peakHoldEnabled: false
      $name: Peak Hold Caps
      $description: Draw a thin cap that hangs at each bar's recent peak
    - beatFlashEnabled: false
      $name: Beat Flash
      $description: Brighten bars on detected bass hits
    - beatFlashIntensity: 80
      $name: Beat Flash Intensity
    - rainbowSpeed: 40
      $name: Rainbow Cycle Speed
      $description: Only used when Color Mode is Rainbow Cycle
    - nowPlayingEnabled: false
      $name: Now Playing Text
      $description: Show artist and title above the visualizer, using Windows media session info
    - nowPlayingColor: '#FFFFFFFF'
      $name: Now Playing Text Color
    - nowPlayingFont: Segoe UI
      $name: Now Playing Font
      $description: Font family name, must be installed on your system (e.g. a Nerd Font for glyph support)
    - nowPlayingFontSize: 16
      $name: Now Playing Font Size
    - nowPlayingDisplaySeconds: 6
      $name: Now Playing Display Seconds
      $description: How long the text stays visible after a track changes, before fading out
  $name: Appearance
- position:
    - horizontalPosition: 50
      $name: Horizontal Position
      $description: 0-100, percentage across the monitor's work area
    - verticalPosition: 88
      $name: Vertical Position
      $description: 0-100, percentage down the monitor's work area
    - monitor: 1
      $name: Monitor
      $description: 1-based monitor index
  $name: Position
- background:
    - enabled: true
      $name: Enabled
    - color: '#60000000'
      $name: Color
    - padding: 24
      $name: Padding
    - cornerRadius: '14'
      $name: Corner Radius
    - blur: 0
      $name: Blur
    - borderSize: 0
      $name: Border Size
    - borderColor: '#40FFFFFF'
      $name: Border Color
  $name: Background
- performance:
    - targetFps: 60
      $name: Target FPS
    - pauseOnFullscreen: true
      $name: Pause On Fullscreen
    - pauseWhenSilentSeconds: 10
      $name: Pause When Silent (seconds)
      $description: 0 disables this behavior
    - autoHideEnabled: false
      $name: Auto-Hide When Idle
      $description: Fades the whole visualizer out after prolonged silence, instead of just idling
    - autoHideDelaySeconds: 15
      $name: Auto-Hide Delay (seconds)
    - pauseWhenObscured: false
      $name: Pause When Covered
      $description: Stops rendering entirely while the visualizer is fully hidden behind another window, since nothing it draws would be visible anyway
  $name: Performance
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d2d1_1.h>
#include <d2d1effects.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <wrl/client.h>
#include <wincodec.h>
#include <dwrite.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Storage::Streams;

using Microsoft::WRL::ComPtr;

#define TIMER_ID_MSG_DISPLAY_CHANGE 1
#define TIMER_ID_MSG_RECREATE_OVERLAY 2
#define TIMER_ID_MSG_WALLPAPER_REFRESH 3
#define TIMER_ID_MSG_FULLSCREEN_WATCH 4

#define WM_APP_CLEANUP (WM_APP + 1)
#define WM_APP_SETTINGS_CHANGED (WM_APP + 2)
#define WM_APP_RENDER_TICK (WM_APP + 3)
#define OVERLAY_WINDOW_CLASS (L"DesktopAudioVisOverlay_" WH_MOD_ID)
#define MESSAGE_WINDOW_CLASS (L"DesktopAudioVisMessage_" WH_MOD_ID)

enum class VizShape { Stereo, Mountain, Mirror, Wave, Breathe, Dots, Radial, Oscilloscope };
enum class VizColorMode { Solid, Gradient, ReactiveGradient, Accent, AlbumArt, DynamicAlbum, Acrylic, RainbowCycle, Tourne };
enum class VizEQ { Default, Bass, Rock, Pop, Jazz, Electronic };
enum class VizOrientation { Horizontal, Vertical };
enum class VizAnchor { Top, Middle, Bottom };
enum class VizFreqScale { Log, Linear, Mel };
enum class VizTextAlignH { Left, Center, Right };
enum class VizTextAlignV { Above, Top, Middle, Bottom, Below };

struct Settings {
    VizShape shape = VizShape::Stereo;
    VizOrientation orientation = VizOrientation::Horizontal;
    int barCount = 32;
    int barWidth = 6;
    int barGap = 4;
    int barMaxSize = 140;
    int barIdleSize = 4;
    float barRadiusTL = 3.0f, barRadiusTR = 3.0f, barRadiusBR = 3.0f, barRadiusBL = 3.0f;
    VizColorMode colorMode = VizColorMode::Solid;
    BYTE colorA = 255, colorR = 255, colorG = 255, colorB = 255;
    BYTE grad1A = 255, grad1R = 30, grad1G = 215, grad1B = 96;
    BYTE grad2A = 255, grad2R = 0, grad2G = 180, grad2B = 255;
    int sensitivity = 150;
    VizEQ eq = VizEQ::Default;

    int horizontalPosition = 50;
    int verticalPosition = 88;
    int monitor = 1;
    VizAnchor verticalAnchor = VizAnchor::Bottom;

    bool backgroundEnabled = true;
    BYTE bgA = 0x60, bgR = 0, bgG = 0, bgB = 0;
    int bgPadding = 24;
    float bgRadiusTL = 14.0f, bgRadiusTR = 14.0f, bgRadiusBR = 14.0f, bgRadiusBL = 14.0f;
    int bgBlur = 0;
    int bgBorderSize = 0;
    BYTE borderA = 0x40, borderR = 255, borderG = 255, borderB = 255;

    int targetFps = 60;
    bool pauseOnFullscreen = true;
    int pauseWhenSilentSeconds = 10;

    bool peakHoldEnabled = false;
    bool beatFlashEnabled = false;
    int beatFlashIntensity = 80;
    int rainbowSpeed = 40;

    bool nowPlayingEnabled = false;
    BYTE nowPlayingA = 255, nowPlayingR = 255, nowPlayingG = 255, nowPlayingB = 255;
    std::wstring nowPlayingFont = L"Segoe UI";
    int nowPlayingFontSize = 16;
    int nowPlayingDisplaySeconds = 6;

    bool autoHideEnabled = false;
    int autoHideDelaySeconds = 15;
    bool pauseWhenObscured = false;

    int fftSize = 1024;
    VizFreqScale freqScale = VizFreqScale::Log;
    bool peakFreqEnabled = false;
    VizTextAlignH peakFreqAlignH = VizTextAlignH::Right;
    VizTextAlignV peakFreqAlignV = VizTextAlignV::Top;
    bool oscilloscopeMultibandEnabled = false;
};

// A 4K display fits ~1280 bars at 2 px wide with 1 px gaps, and ultrawides more
// still, so the cap has room above that. Each bar costs a handful of floats.
constexpr int VIZ_BARS_MAX = 2048;
constexpr int VIZ_FFT_SIZE_MAX = 8192;
constexpr int VIZ_NUM_BANDS = 7;
constexpr float VIZ_PI = 3.14159265f;

// Frequency-band edges (Hz), shared between BuildLogBins (capture thread) and
// HzToBandPos (render thread) so the Log/Linear/Mel scale mapping always
// warps onto the exact same band boundaries the audio analysis actually uses.
constexpr float VIZ_FREQ_EDGES[VIZ_NUM_BANDS + 1] = {
    20.f, 120.f, 300.f, 800.f, 2500.f, 6000.f, 14000.f, 20000.f};

// Which EQ zone (0=low, 1=mid, 2=high) each band belongs to. Shared between
// the capture thread's EQ preset weighting and the multiband oscilloscope
// coloring so both agree on the same low/mid/high split.
constexpr int VIZ_BAND_EQ_ZONE[VIZ_NUM_BANDS] = {0, 0, 1, 1, 2, 2, 2};

Settings g_settings;

std::atomic<bool> g_lazyInitialized{false};
std::atomic<bool> g_initSucceeded{false};
std::atomic<bool> g_unloading{false};

ComPtr<ID3D11Device> g_d3dDevice;
ComPtr<IDXGIDevice> g_dxgiDevice;
ComPtr<IDXGIFactory2> g_dxgiFactory;
ComPtr<ID2D1Factory1> g_d2dFactory;
ComPtr<ID2D1Device> g_d2dDevice;

HWND g_messageWnd;

std::atomic<HWND> g_overlayWnd{nullptr};
ComPtr<IDXGISwapChain1> g_swapChain;
ComPtr<ID2D1DeviceContext> g_dc;
ComPtr<IDCompositionDevice> g_compositionDevice;
ComPtr<IDCompositionTarget> g_compositionTarget;
ComPtr<IDCompositionVisual> g_compositionVisual;
ComPtr<ID2D1SolidColorBrush> g_barBrush;
ComPtr<ID2D1SolidColorBrush> g_barBrush2;
ComPtr<ID2D1SolidColorBrush> g_backgroundBrush;
ComPtr<ID2D1SolidColorBrush> g_borderBrush;
ComPtr<ID2D1Bitmap> g_wallpaperBitmap;
ComPtr<ID2D1Effect> g_blurEffect;
// Pre-rendered result of the Gaussian blur. The blur is a full-image convolution
// and by far the most expensive Direct2D operation in the scene, but its input
// (the wallpaper) doesn't change frame to frame -- so it's evaluated once here
// and then simply blitted each frame instead of being recomputed.
ComPtr<ID2D1Bitmap1> g_blurredBitmap;

ComPtr<ID2D1PathGeometry> g_bgGeoCache;
ComPtr<ID2D1GeometryGroup> g_borderRingCache;
D2D1_RECT_F g_bgGeoCacheRect = {-1.f, -1.f, -1.f, -1.f};
float g_bgGeoCacheRadii[4] = {-1.f, -1.f, -1.f, -1.f};
int g_borderCacheBorderSize = -1;

ComPtr<ID2D1StrokeStyle> g_roundCapStrokeStyle;
ComPtr<IDWriteFactory> g_dwriteFactory;
ComPtr<IDWriteTextFormat> g_dwriteTextFormat;
ComPtr<ID2D1SolidColorBrush> g_nowPlayingBrush;
int g_dwriteTextFormatFontSize = -1;
std::wstring g_dwriteTextFormatFontName;

float g_dpiScale = 1.0f;
FILETIME g_lastWallpaperTime = {};
HMONITOR g_cachedMonitor = nullptr;

std::atomic<bool> g_captureRunning{false};
std::thread* g_captureThread = nullptr;
HANDLE g_captureEvent = nullptr;
std::atomic<bool> g_deviceChanged{false};
std::atomic<float> g_bands[VIZ_NUM_BANDS] = {};

std::thread* g_renderThread = nullptr;
std::atomic<bool> g_renderThreadRunning{false};
std::atomic<bool> g_renderTickPending{false};

float g_hannWindow[VIZ_FFT_SIZE_MAX] = {};
float g_twiddleRe[VIZ_FFT_SIZE_MAX / 2] = {};
float g_twiddleIm[VIZ_FFT_SIZE_MAX / 2] = {};
int g_logBinStart[VIZ_NUM_BANDS + 1] = {};

float g_vizPeak[VIZ_BARS_MAX] = {};
float g_vizTarget[VIZ_BARS_MAX] = {};
float g_vizPeakHold[VIZ_BARS_MAX] = {};
float g_vizBreatheEnv = 0.f;

constexpr int VIZ_WAVE_SAMPLES = 256;
std::atomic<float> g_waveform[VIZ_WAVE_SAMPLES] = {};
std::atomic<float> g_beatPulse{0.f};
std::atomic<float> g_dominantFreqHz{0.f};

// Screen-space bounds of the last drawn frame, used by the occlusion check so
// it can test the region we actually occupy rather than the whole monitor.
std::atomic<LONG> g_drawRectL{0}, g_drawRectT{0}, g_drawRectR{0}, g_drawRectB{0};
std::atomic<bool> g_drawRectValid{false};
// Set once the auto-hide fade has reached full transparency and a single blank
// frame has been presented. While set, the render path exits immediately.
bool g_autoHideBlanked = false;

// Current swap chain dimensions and composition-visual offset. The swap chain
// covers only the widget's bounding box rather than the whole desktop, so these
// are tracked to detect when a settings/display change requires a resize.
UINT  g_swapChainWidth = 0, g_swapChainHeight = 0;
float g_visualOffsetX = 0.f, g_visualOffsetY = 0.f;

std::mutex g_nowPlayingMutex;
std::wstring g_nowPlayingDisplay;
std::atomic<ULONGLONG> g_nowPlayingChangedTick{0};

static float VIZ_SEEDS[VIZ_BARS_MAX] = {};

void BuildVizSeeds() {
    unsigned int state = 0x12345678u;
    for (int i = 0; i < VIZ_BARS_MAX; i++) {
        state = state * 1664525u + 1013904223u;
        float t = (float)(state >> 8) / (float)(1u << 24);
        VIZ_SEEDS[i] = 0.25f + t * 1.20f;
    }
}

std::atomic<bool> g_fullscreenPaused{false};
std::atomic<ULONGLONG> g_lastAudibleTickMs{0};
bool g_slowMode = false;

static std::atomic<DWORD> g_albumArtColor{0xFFFFFFFF};
static std::atomic<DWORD> g_albumArtColorSecondary{0xFFAAAAAA};
static std::atomic<bool>  g_albumArtColorReady{false};
static std::atomic<bool>  g_albumArtFetchPending{false};
static std::atomic<DWORD> g_accentColorCache{0xFF0078D4};

static HANDLE g_gsmtcStopEvent = nullptr;
// std::thread is not a nullable/handle-like type: there is no assignment that
// empties it, and move-assigning over a joinable() thread calls std::terminate().
// The optional gives the bare [[clang::no_destroy]] attribute something it can
// legitimately be applied to.
[[clang::no_destroy]] static std::optional<std::thread> g_gsmtcThread;

// Guards g_albumArtThread. The GSMTC event handlers run on WinRT thread-pool
// threads and can race Wh_ModUninit for ownership of this pointer.
static std::mutex g_albumArtThreadMutex;
static std::thread* g_albumArtThread = nullptr;

// Thread timer armed from the CreateWindowExW hook. Must be a global rather than
// a function-local static: the TIMERPROC points into the mod image, so if the mod
// unloads inside the 1s window the next WM_TIMER dispatch jumps into unmapped
// memory and takes explorer.exe with it. A thread timer can only be killed from
// the thread that set it, so uninit kills it via RunFromWindowThread.
UINT_PTR g_createOverlayTimer = 0;

// Animation time base. GetTickCount64() alone overflows a float's 24-bit
// mantissa after ~4.7 hours of uptime, at which point Wave/Breathe/Rainbow
// visibly quantize; measuring from a baseline captured at init keeps the
// delta small enough to stay exact.
ULONGLONG g_startTick = 0;

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                static const UINT kM =
                    RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == kM) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }
    return module;
}

float GetMonitorDpiScale(HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        return dpiX / 96.0f;
    }
    return 1.0f;
}

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR monitorResult = nullptr;
    int currentMonitorId = 0;

    auto monitorEnumProc = [&monitorResult, &currentMonitorId,
                            monitorId](HMONITOR hMonitor) -> BOOL {
        if (currentMonitorId == monitorId) {
            monitorResult = hMonitor;
            return FALSE;
        }
        currentMonitorId++;
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

bool ParseColorHex(PCWSTR colorStr, BYTE* a, BYTE* r, BYTE* g, BYTE* b) {
    if (!colorStr || !*colorStr) {
        return false;
    }
    if (*colorStr == L'#') {
        colorStr++;
    }
    size_t len = wcslen(colorStr);
    unsigned int value = 0;
    for (size_t i = 0; i < len; i++) {
        WCHAR c = colorStr[i];
        int digit;
        if (c >= L'0' && c <= L'9') digit = c - L'0';
        else if (c >= L'A' && c <= L'F') digit = c - L'A' + 10;
        else if (c >= L'a' && c <= L'f') digit = c - L'a' + 10;
        else return false;
        value = (value << 4) | digit;
    }
    if (len == 6) {
        *a = 255;
        *r = (value >> 16) & 0xFF;
        *g = (value >> 8) & 0xFF;
        *b = value & 0xFF;
    } else if (len == 8) {
        *a = (value >> 24) & 0xFF;
        *r = (value >> 16) & 0xFF;
        *g = (value >> 8) & 0xFF;
        *b = value & 0xFF;
    } else {
        return false;
    }
    return true;
}

bool IsFolderViewWnd(HWND hWnd) {
    WCHAR buffer[64];

    if (!GetClassName(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SysListView32")) {
        return false;
    }
    if (!GetWindowText(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"FolderView")) {
        return false;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (!hParentWnd) return false;
    if (!GetClassName(hParentWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SHELLDLL_DefView")) {
        return false;
    }
    if (GetWindowTextLength(hParentWnd) > 0) return false;

    HWND hParentWnd2 = GetAncestor(hParentWnd, GA_PARENT);
    if (!hParentWnd2) return false;
    if ((!GetClassName(hParentWnd2, buffer, ARRAYSIZE(buffer)) ||
         _wcsicmp(buffer, L"Progman")) &&
        hParentWnd2 != GetShellWindow()) {
        return false;
    }

    return true;
}

HWND GetWorkerW() {
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (!hProgman) return nullptr;

    DWORD progmanProcessId = 0;
    GetWindowThreadProcessId(hProgman, &progmanProcessId);
    if (progmanProcessId != GetCurrentProcessId()) return nullptr;

    SendMessage(hProgman, 0x052C, 0xD, 0);
    SendMessage(hProgman, 0x052C, 0xD, 1);

    HWND hWorkerW = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
                return TRUE;
            }
            HWND hWorker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
            if (hWorker) {
                *(HWND*)lParam = hWorker;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&hWorkerW);

    if (!hWorkerW) {
        SendMessage(hProgman, 0x052C, 0, 0);
        EnumWindows(
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                if (!FindWindowEx(hWnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
                    return TRUE;
                }
                HWND hWorker = FindWindowEx(nullptr, hWnd, L"WorkerW", nullptr);
                if (hWorker) {
                    *(HWND*)lParam = hWorker;
                    return FALSE;
                }
                return TRUE;
            },
            (LPARAM)&hWorkerW);
    }

    if (!hWorkerW) {
        hWorkerW = FindWindowEx(hProgman, nullptr, L"WorkerW", nullptr);
    }
    if (!hWorkerW) {
        hWorkerW = hProgman;
    }

    return hWorkerW;
}

bool IsWindowFullscreen(HWND hwnd, HMONITOR targetMonitor = nullptr) {
    if (!hwnd || !IsWindowVisible(hwnd)) return false;

    RECT windowRect;
    if (!GetWindowRect(hwnd, &windowRect)) return false;

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (targetMonitor && monitor != targetMonitor) return false;

    MONITORINFO monitorInfo = {sizeof(MONITORINFO)};
    if (!GetMonitorInfo(monitor, &monitorInfo)) return false;

    return windowRect.left <= monitorInfo.rcMonitor.left &&
           windowRect.top <= monitorInfo.rcMonitor.top &&
           windowRect.right >= monitorInfo.rcMonitor.right &&
           windowRect.bottom >= monitorInfo.rcMonitor.bottom;
}

bool IsFullscreenOrGameActive() {
    // g_cachedMonitor is maintained by CreateSwapChainResources/HandleDisplayChange.
    // This runs on the UI thread once a second with pauseOnFullscreen on by
    // default, so re-running EnumDisplayMonitors here would undo the caching.
    HMONITOR targetMonitor = g_cachedMonitor;
    if (!targetMonitor) targetMonitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);

    HWND hwndForeground = GetForegroundWindow();
    if (!hwndForeground) return false;

    WCHAR className[256];
    if (GetClassName(hwndForeground, className, ARRAYSIZE(className))) {
        if (_wcsicmp(className, L"Progman") == 0 ||
            _wcsicmp(className, L"WorkerW") == 0 ||
            _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            return false;
        }
    }

    HMONITOR foregroundMonitor = MonitorFromWindow(hwndForeground, MONITOR_DEFAULTTONEAREST);
    if (foregroundMonitor != targetMonitor) return false;

    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        if (state == QUNS_RUNNING_D3D_FULL_SCREEN || state == QUNS_PRESENTATION_MODE)
            return true;
    }

    if (IsWindowFullscreen(hwndForeground, targetMonitor))
        return true;

    return false;
}

// Returns true when the region the visualizer occupies is fully covered by some
// ordinary window. The widget lives on the desktop (behind icons), so any
// maximized app hides it completely -- yet without this check we'd keep running
// the full render path at the target frame rate drawing pixels nobody can see.
// Deliberately conservative: only *full* containment counts, so partial overlap
// never causes the visualizer to vanish while it's still partly visible.
bool IsVisualizerOccluded() {
    if (!g_drawRectValid.load(std::memory_order_relaxed)) return false;

    RECT viz;
    viz.left   = g_drawRectL.load(std::memory_order_relaxed);
    viz.top    = g_drawRectT.load(std::memory_order_relaxed);
    viz.right  = g_drawRectR.load(std::memory_order_relaxed);
    viz.bottom = g_drawRectB.load(std::memory_order_relaxed);
    if (viz.right <= viz.left || viz.bottom <= viz.top) return false;

    struct EnumCtx {
        RECT viz;
        bool occluded;
    } ctx{viz, false};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* c = reinterpret_cast<EnumCtx*>(lParam);

            if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return TRUE;

            // Only genuine application windows count as covering the desktop.
            // This is the standard "would it show up in Alt-Tab" test, and it is
            // what makes this check reliable on Windows 11: the shell is full of
            // XAML islands, composition bridges and flyout hosts (Quick Settings,
            // media popups, the taskbar's own sub-windows) that report themselves
            // as visible, often span large areas, and hide nothing at all.
            if (hWnd != GetAncestor(hWnd, GA_ROOTOWNER)) return TRUE;

            LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
            if (exStyle & WS_EX_TOOLWINDOW) return TRUE;
            if (exStyle & WS_EX_TRANSPARENT) return TRUE;
            if (exStyle & WS_EX_NOACTIVATE) return TRUE;

            // A real app window has a caption or is a normal overlapped window.
            LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
            if (style & WS_CHILD) return TRUE;

            WCHAR cls[64] = {};
            if (GetClassName(hWnd, cls, ARRAYSIZE(cls))) {
                if (_wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
                    _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
                    _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
                    _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
                    _wcsicmp(cls, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
                    _wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
                    _wcsicmp(cls, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
                    _wcsicmp(cls, L"ForegroundStaging") == 0 ||
                    _wcsicmp(cls, L"MultitaskingViewFrame") == 0) {
                    return TRUE;
                }
            }

            // An untitled window is almost always infrastructure rather than a
            // real application the user is looking at.
            if (GetWindowTextLength(hWnd) == 0) return TRUE;

            // Skip DWM-cloaked windows. This is the important one: on Windows
            // 10/11 a great many UWP/XAML windows report IsWindowVisible() ==
            // TRUE while being entirely invisible (cloaked), and several of them
            // are sized to the full screen. Without this check they register as
            // covering the visualizer and hide it while nothing is really there.
            int cloaked = 0;
            if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked,
                                                sizeof(cloaked))) && cloaked) {
                return TRUE;
            }

            RECT wr;
            if (!GetWindowRect(hWnd, &wr)) return TRUE;

            // Ignore degenerate/zero-area windows.
            if (wr.right <= wr.left || wr.bottom <= wr.top) return TRUE;

            if (wr.left <= c->viz.left && wr.top <= c->viz.top &&
                wr.right >= c->viz.right && wr.bottom >= c->viz.bottom) {
                c->occluded = true;
                // Name the culprit so a false positive can be identified from
                // the log rather than guessed at.
                WCHAR title[128] = {};
                GetWindowText(hWnd, title, ARRAYSIZE(title));
                Wh_Log(L"Occluded by class='%s' title='%s' rect=(%ld,%ld,%ld,%ld)",
                       cls, title, wr.left, wr.top, wr.right, wr.bottom);
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    return ctx.occluded;
}

void RefreshAccentColorCache() {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Accent",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD color = 0, size = sizeof(color), type = 0;
        if (RegQueryValueExW(hKey, L"AccentColorMenu", nullptr, &type,
                             (LPBYTE)&color, &size) == ERROR_SUCCESS && type == REG_DWORD) {
            RegCloseKey(hKey);
            BYTE r = (color >>  0) & 0xFF;
            BYTE g = (color >>  8) & 0xFF;
            BYTE b = (color >> 16) & 0xFF;
            g_accentColorCache.store(0xFF000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | b,
                                     std::memory_order_relaxed);
            return;
        }
        if (RegQueryValueExW(hKey, L"AccentColor", nullptr, &type,
                             (LPBYTE)&color, &size) == ERROR_SUCCESS && type == REG_DWORD) {
            RegCloseKey(hKey);
            BYTE r = (color >>  0) & 0xFF;
            BYTE g = (color >>  8) & 0xFF;
            BYTE b = (color >> 16) & 0xFF;
            g_accentColorCache.store(0xFF000000 | ((DWORD)r << 16) | ((DWORD)g << 8) | b,
                                     std::memory_order_relaxed);
            return;
        }
        RegCloseKey(hKey);
    }

    DWORD color = 0; BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque)))
        g_accentColorCache.store(0xFF000000 | (color & 0x00FFFFFF), std::memory_order_relaxed);
}

DWORD GetWindowsAccentColor() {
    return g_accentColorCache.load(std::memory_order_relaxed);
}

void FetchAlbumArtColorAsync() {
    // Called from GSMTC handlers running on WinRT thread-pool threads. Revoking
    // those handlers during teardown does not drain one already in flight, so
    // without this check a callback could spawn a fresh thread after uninit has
    // already joined and deleted the previous one - a double delete on the raw
    // pointer, plus a new thread running straight into the unload.
    if (g_unloading.load()) return;

    bool expected = false;
    if (!g_albumArtFetchPending.compare_exchange_strong(expected, true))
        return;

    std::lock_guard<std::mutex> ownerLock(g_albumArtThreadMutex);

    // Re-check: uninit may have set g_unloading while we waited on the mutex.
    if (g_unloading.load()) {
        g_albumArtFetchPending.store(false);
        return;
    }

    if (g_albumArtThread) {
        if (g_albumArtThread->joinable())
            g_albumArtThread->join();
        delete g_albumArtThread;
        g_albumArtThread = nullptr;
    }

    g_albumArtThread = new std::thread([]() {
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);

            // Cancellation the worker can actually observe. Each await below can
            // block for seconds if a media app is unresponsive, so unloading is
            // re-checked between them to keep teardown bounded.
            auto cancelled = [] { return g_unloading.load(); };
            auto bail = [] { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); };

            if (cancelled()) { bail(); return; }
            auto mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (!mgr || cancelled()) { bail(); return; }
            auto session = mgr.GetCurrentSession();
            if (!session || cancelled()) { bail(); return; }

            auto props = session.TryGetMediaPropertiesAsync().get();
            if (!props || cancelled()) { bail(); return; }

            {
                std::wstring title(props.Title());
                std::wstring artist(props.Artist());
                std::wstring display = artist.empty() ? title
                                      : title.empty()  ? artist
                                                        : (artist + L" - " + title);
                if (!display.empty()) {
                    std::lock_guard<std::mutex> lock(g_nowPlayingMutex);
                    if (g_nowPlayingDisplay != display) {
                        g_nowPlayingDisplay = display;
                        g_nowPlayingChangedTick.store(GetTickCount64(), std::memory_order_relaxed);
                    }
                }
            }

            auto thumbRef = props.Thumbnail();
            if (!thumbRef || cancelled()) { bail(); return; }

            auto stream = thumbRef.OpenReadAsync().get();
            if (!stream || cancelled()) { bail(); return; }

            UINT64 sz = stream.Size();
            if (sz == 0 || sz > 4 * 1024 * 1024) { bail(); return; }

            DataReader reader(stream);
            reader.LoadAsync((UINT32)sz).get();
            if (cancelled()) { bail(); return; }
            std::vector<BYTE> thumbBytes((size_t)sz);
            reader.ReadBytes(winrt::array_view<BYTE>(thumbBytes));
            reader.DetachStream();

            IWICImagingFactory* pFactory = nullptr;
            if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                        IID_PPV_ARGS(&pFactory))) || !pFactory) {
                winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return;
            }
            IStream* pStream = SHCreateMemStream(thumbBytes.data(), (UINT)thumbBytes.size());
            if (!pStream) { pFactory->Release(); winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            IWICBitmapDecoder* pDecoder = nullptr;
            IWICBitmapFrameDecode* pFrame = nullptr;
            IWICFormatConverter* pConv = nullptr;
            std::vector<BYTE> pixels;
            int imgW = 0, imgH = 0;

            if (SUCCEEDED(pFactory->CreateDecoderFromStream(pStream, nullptr,
                    WICDecodeMetadataCacheOnDemand, &pDecoder)) &&
                SUCCEEDED(pDecoder->GetFrame(0, &pFrame)) &&
                SUCCEEDED(pFactory->CreateFormatConverter(&pConv))) {
                if (SUCCEEDED(pConv->Initialize(pFrame, GUID_WICPixelFormat32bppBGRA,
                        WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut))) {
                    UINT w = 0, h = 0;
                    pConv->GetSize(&w, &h);
                    if (w > 0 && h > 0) {
                        pixels.resize((size_t)w * h * 4);
                        if (SUCCEEDED(pConv->CopyPixels(nullptr, w * 4,
                                (UINT)pixels.size(), pixels.data()))) {
                            imgW = (int)w; imgH = (int)h;
                        }
                    }
                }
            }
            if (pConv)    pConv->Release();
            if (pFrame)   pFrame->Release();
            if (pDecoder) pDecoder->Release();
            pStream->Release();
            pFactory->Release();

            if (!pixels.empty()) {
                struct Bucket { uint32_t r=0,g=0,b=0,n=0; };
                Bucket buckets[16][16][16]{};
                for (int y = 0; y < imgH; y += 4) {
                    for (int x = 0; x < imgW; x += 4) {
                        size_t idx = ((size_t)y * imgW + x) * 4;
                        if (idx + 4 > pixels.size()) continue;
                        BYTE pb = pixels[idx], pg = pixels[idx+1], pr = pixels[idx+2];
                        int luma = (pr * 299 + pg * 587 + pb * 114) / 1000;
                        if (luma < 24 || luma > 235) continue;
                        auto& bk = buckets[pr >> 4][pg >> 4][pb >> 4];
                        bk.r += pr; bk.g += pg; bk.b += pb; bk.n++;
                    }
                }

                struct Cand { float w; BYTE r,g,b; };
                std::vector<Cand> cands;
                cands.reserve(64);
                for (int R = 0; R < 16; R++) for (int G = 0; G < 16; G++) for (int B = 0; B < 16; B++) {
                    auto& bk = buckets[R][G][B];
                    if (bk.n < 8) continue;
                    float fr = bk.r/(float)bk.n/255.f, fg = bk.g/(float)bk.n/255.f, fb = bk.b/(float)bk.n/255.f;
                    float mx = std::max({fr,fg,fb}), mn = std::min({fr,fg,fb});
                    float sat = mx > 0 ? (mx - mn) / mx : 0;
                    cands.push_back({ bk.n * (0.3f + sat),
                                     (BYTE)(fr*255), (BYTE)(fg*255), (BYTE)(fb*255) });
                }

                if (!cands.empty()) {
                    std::sort(cands.begin(), cands.end(),
                              [](const Cand& a, const Cand& b){ return a.w > b.w; });

                    BYTE pR = cands[0].r, pG = cands[0].g, pB = cands[0].b;

                    BYTE sR = pR, sG = pG, sB = pB;
                    for (auto& c : cands) {
                        int dr = (int)c.r - (int)pR;
                        int dg = (int)c.g - (int)pG;
                        int db = (int)c.b - (int)pB;
                        if (dr*dr + dg*dg + db*db > 3264) {
                            sR = c.r; sG = c.g; sB = c.b;
                            break;
                        }
                    }

                    DWORD col  = 0xFF000000 | ((DWORD)pR << 16) | ((DWORD)pG << 8) | pB;
                    DWORD col2 = 0xFF000000 | ((DWORD)sR << 16) | ((DWORD)sG << 8) | sB;
                    g_albumArtColor.store(col,  std::memory_order_relaxed);
                    g_albumArtColorSecondary.store(col2, std::memory_order_relaxed);
                    g_albumArtColorReady.store(true, std::memory_order_relaxed);
                }
            }
        } catch (...) {}
        try { winrt::uninit_apartment(); } catch (...) {}
        g_albumArtFetchPending.store(false);
    });
}

static winrt::event_token g_gsmtcMediaPropsToken{};
static winrt::event_token g_gsmtcSessionToken{};
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSessionManager g_gsmtcMgr{ nullptr };
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSession        g_gsmtcSession{ nullptr };

void SetupGsmtcSessionListener() {
    if (!g_gsmtcMgr) return;
    try {
        if (g_gsmtcSession) {
            try { g_gsmtcSession.MediaPropertiesChanged(g_gsmtcMediaPropsToken); } catch (...) {}
            g_gsmtcSession = nullptr;
        }
        g_gsmtcSession = g_gsmtcMgr.GetCurrentSession();
        if (!g_gsmtcSession) return;
        g_gsmtcMediaPropsToken = g_gsmtcSession.MediaPropertiesChanged(
            [](auto const&, auto const&) {
                if (g_settings.colorMode == VizColorMode::AlbumArt ||
                    g_settings.colorMode == VizColorMode::DynamicAlbum ||
                    g_settings.nowPlayingEnabled)
                    FetchAlbumArtColorAsync();
            });
    } catch (...) {}
}

void InitGsmtcListener() {
    if (g_gsmtcStopEvent) return;
    g_gsmtcStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_gsmtcStopEvent) return;

    g_gsmtcThread.emplace([]() {
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            g_gsmtcMgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (!g_gsmtcMgr) { winrt::uninit_apartment(); return; }

            g_gsmtcSessionToken = g_gsmtcMgr.CurrentSessionChanged(
                [](auto const&, auto const&) {
                    SetupGsmtcSessionListener();
                    if (g_settings.colorMode == VizColorMode::AlbumArt ||
                        g_settings.colorMode == VizColorMode::DynamicAlbum ||
                        g_settings.nowPlayingEnabled)
                        FetchAlbumArtColorAsync();
                });

            SetupGsmtcSessionListener();
            if (g_settings.colorMode == VizColorMode::AlbumArt ||
                g_settings.colorMode == VizColorMode::DynamicAlbum ||
                g_settings.nowPlayingEnabled)
                FetchAlbumArtColorAsync();
        } catch (...) {}

        if (g_gsmtcStopEvent)
            WaitForSingleObject(g_gsmtcStopEvent, INFINITE);

        try {
            if (g_gsmtcSession) {
                g_gsmtcSession.MediaPropertiesChanged(g_gsmtcMediaPropsToken);
            }
            if (g_gsmtcMgr) {
                g_gsmtcMgr.CurrentSessionChanged(g_gsmtcSessionToken);
            }
            g_gsmtcSession = nullptr;
            g_gsmtcMgr     = nullptr;
            winrt::uninit_apartment();
        } catch (...) {}
    });
}

static bool g_gsmtcStarted = false;

void BuildHannWindow(int n) {
    for (int i = 0; i < n; i++)
        g_hannWindow[i] = 0.5f * (1.f - cosf(2.f * VIZ_PI * i / (n - 1)));
}

void BuildTwiddleFactors(int n) {
    for (int i = 0; i < n / 2; i++) {
        float ang = -2.0f * VIZ_PI * i / n;
        g_twiddleRe[i] = cosf(ang);
        g_twiddleIm[i] = sinf(ang);
    }
}

void BuildLogBins(UINT32 sampleRate, int fftSize) {
    for (int b = 0; b <= VIZ_NUM_BANDS; b++) {
        int bin = (int)(VIZ_FREQ_EDGES[b] * fftSize / (float)sampleRate);
        g_logBinStart[b] = std::max(1, std::min(fftSize / 2 - 1, bin));
    }
}

// Converts a target frequency (Hz) into an equivalent fractional band index
// (0..VIZ_NUM_BANDS-1) by inverse-interpolating against the shared band-edge
// table. This lets a warped frequency curve (log/linear/mel) still be sampled
// through the existing 7-band energy data without touching the analysis side.
float HzToBandPos(float hz) {
    for (int b = 0; b < VIZ_NUM_BANDS; b++) {
        if (hz <= VIZ_FREQ_EDGES[b + 1]) {
            float lo = VIZ_FREQ_EDGES[b], hi = VIZ_FREQ_EDGES[b + 1];
            float frac = (hi > lo) ? (hz - lo) / (hi - lo) : 0.f;
            return (float)b + std::max(0.f, std::min(1.f, frac));
        }
    }
    return (float)(VIZ_NUM_BANDS - 1);
}

void VizFFT(std::vector<float>& re, std::vector<float>& im) {
    int n = (int)re.size();
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) {
            std::swap(re[i], re[j]);
            std::swap(im[i], im[j]);
        }
    }
    for (int len = 2; len <= n; len <<= 1) {
        int halfLen = len / 2;
        int stride = n / len;
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < halfLen; j++) {
                float wRe = g_twiddleRe[j * stride];
                float wIm = g_twiddleIm[j * stride];
                float uRe = re[i + j], uIm = im[i + j];
                float vRe = re[i + j + halfLen] * wRe - im[i + j + halfLen] * wIm;
                float vIm = re[i + j + halfLen] * wIm + im[i + j + halfLen] * wRe;
                re[i + j] = uRe + vRe;
                im[i + j] = uIm + vIm;
                re[i + j + halfLen] = uRe - vRe;
                im[i + j + halfLen] = uIm - vIm;
            }
        }
    }
}

struct VizEQMul { float low, mid, high; };
VizEQMul GetVizEQMultipliers(VizEQ eq) {
    switch (eq) {
    case VizEQ::Bass: return {2.0f, 0.6f, 0.4f};
    case VizEQ::Rock: return {1.3f, 1.5f, 1.2f};
    case VizEQ::Pop: return {0.8f, 1.2f, 1.8f};
    case VizEQ::Jazz: return {1.1f, 0.8f, 0.6f};
    case VizEQ::Electronic: return {1.7f, 0.6f, 1.7f};
    default: return {1.0f, 1.0f, 1.0f};
    }
}

class VizEndpointNotificationClient : public IMMNotificationClient {
   public:
    virtual ~VizEndpointNotificationClient() = default;

    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_ref); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG ref = InterlockedDecrement(&m_ref);
        if (ref == 0) delete this;
        return ref;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole, LPCWSTR) override {
        if (flow == eRender) g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override {
        g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override {
        g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override {
        g_deviceChanged.store(true, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override {
        return S_OK;
    }

   private:
    LONG m_ref = 1;
};

bool VizInitAudioClient(IMMDeviceEnumerator* pEnum, ComPtr<IAudioClient>& pClient,
                        ComPtr<IAudioCaptureClient>& pCapture, UINT32& sampleRate,
                        UINT32& channels, bool& isFloat, HANDLE hEvent) {
    pClient.Reset();
    pCapture.Reset();

    ComPtr<IMMDevice> pDev;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &pDev))) return false;

    ComPtr<IAudioClient> pC;
    if (FAILED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                              (void**)pC.GetAddressOf())))
        return false;

    WAVEFORMATEX* pwfx = nullptr;
    pC->GetMixFormat(&pwfx);
    if (!pwfx) return false;

    sampleRate = pwfx->nSamplesPerSec;
    channels = pwfx->nChannels;
    isFloat = (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
              (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
               reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->SubFormat ==
                   KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);

    HRESULT hr = pC->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        200000, 0, pwfx, nullptr);
    CoTaskMemFree(pwfx);
    if (FAILED(hr)) return false;

    if (hEvent) pC->SetEventHandle(hEvent);

    ComPtr<IAudioCaptureClient> pCap;
    if (FAILED(pC->GetService(__uuidof(IAudioCaptureClient), (void**)pCap.GetAddressOf())))
        return false;

    if (FAILED(pC->Start())) return false;

    pClient = pC;
    pCapture = pCap;
    return true;
}

void VizCaptureThreadProc() {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    ComPtr<IMMDeviceEnumerator> pEnum;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                __uuidof(IMMDeviceEnumerator), (void**)pEnum.GetAddressOf()))) {
        g_captureRunning.store(false);
        CoUninitialize();
        return;
    }

    auto* notifyClient = new VizEndpointNotificationClient();
    bool notifyRegistered = SUCCEEDED(pEnum->RegisterEndpointNotificationCallback(notifyClient));

    ComPtr<IAudioClient> pClient;
    ComPtr<IAudioCaptureClient> pCapture;
    UINT32 sampleRate = 48000, channels = 2;
    bool isFloat = true;

    static constexpr int RING_CAP = VIZ_FFT_SIZE_MAX * 4;
    std::vector<float> ringBuf(RING_CAP, 0.f);
    int ringHead = 0, ringCount = 0;
    std::vector<float> re, im;

    float bandEnv[VIZ_NUM_BANDS] = {};
    static constexpr float GRAVITY[VIZ_NUM_BANDS] = {0.018f, 0.020f, 0.022f, 0.025f,
                                                     0.030f, 0.036f, 0.042f};

    int currentFftSize = 0;
    auto ValidFftSize = [](int n) -> int {
        return (n == 1024 || n == 2048 || n == 4096 || n == 8192) ? n : 1024;
    };
    auto ApplyFftSize = [&](int newSize) {
        currentFftSize = newSize;
        BuildHannWindow(newSize);
        BuildTwiddleFactors(newSize);
        re.assign(newSize, 0.f);
        im.assign(newSize, 0.f);
        ringHead = 0;
        ringCount = 0;
        for (int b = 0; b < VIZ_NUM_BANDS; b++) {
            bandEnv[b] = 0.f;
            g_bands[b].store(0.f, std::memory_order_relaxed);
        }
        BuildLogBins(sampleRate, newSize);
    };

    g_deviceChanged.store(false, std::memory_order_relaxed);
    VizInitAudioClient(pEnum.Get(), pClient, pCapture, sampleRate, channels, isFloat,
                       g_captureEvent);
    ApplyFftSize(ValidFftSize(g_settings.fftSize));

    ULONGLONG lastReinitAttempt = GetTickCount64() - 1000;

    while (g_captureRunning.load(std::memory_order_relaxed)) {
        if (g_captureEvent)
            WaitForSingleObject(g_captureEvent, 20);
        else
            Sleep(8);

        bool needsReinit = g_deviceChanged.exchange(false, std::memory_order_relaxed) || !pClient;
        if (needsReinit) {
            ULONGLONG now = GetTickCount64();
            if (now - lastReinitAttempt >= 500) {
                lastReinitAttempt = now;
                if (pClient) pClient->Stop();
                ringHead = 0;
                ringCount = 0;
                for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                    bandEnv[b] = 0.f;
                    g_bands[b].store(0.f, std::memory_order_relaxed);
                }
                if (VizInitAudioClient(pEnum.Get(), pClient, pCapture, sampleRate, channels,
                                       isFloat, g_captureEvent))
                    BuildLogBins(sampleRate, currentFftSize);
            }
        }

        int wantedFftSize = ValidFftSize(g_settings.fftSize);
        if (wantedFftSize != currentFftSize) {
            ApplyFftSize(wantedFftSize);
        }

        if (!pCapture) continue;

        UINT32 packetSize = 0;
        HRESULT hr = pCapture->GetNextPacketSize(&packetSize);
        if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
            g_deviceChanged.store(true, std::memory_order_relaxed);
            continue;
        }
        if (FAILED(hr) || packetSize == 0) {
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                bandEnv[b] = std::max(0.f, bandEnv[b] - GRAVITY[b]);
                g_bands[b].store(bandEnv[b], std::memory_order_relaxed);
            }
            continue;
        }

        while (packetSize > 0) {
            BYTE* pData = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;
            HRESULT hrBuf = pCapture->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
            if (hrBuf == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_deviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hrBuf)) break;

            if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData && numFrames > 0) {
                if (isFloat) {
                    float* src = reinterpret_cast<float*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++) mono += src[f * channels + c];
                        mono /= (float)channels;
                        ringBuf[ringHead] = mono;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP) ringCount++;
                    }
                } else {
                    INT16* src = reinterpret_cast<INT16*>(pData);
                    for (UINT32 f = 0; f < numFrames; f++) {
                        float mono = 0.f;
                        for (UINT32 c = 0; c < channels; c++)
                            mono += src[f * channels + c] / 32768.f;
                        mono /= (float)channels;
                        ringBuf[ringHead] = mono;
                        ringHead = (ringHead + 1) % RING_CAP;
                        if (ringCount < RING_CAP) ringCount++;
                    }
                }
            }
            pCapture->ReleaseBuffer(numFrames);
            hr = pCapture->GetNextPacketSize(&packetSize);
            if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_deviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hr)) break;
        }

        while (ringCount >= currentFftSize) {
            int readStart = (ringHead - ringCount + RING_CAP) % RING_CAP;

            // Raw waveform snapshot for the oscilloscope shape. Skipped entirely
            // unless that shape is actually selected, to avoid needless per-chunk work.
            if (g_settings.shape == VizShape::Oscilloscope) {
                int wstep = std::max(1, currentFftSize / VIZ_WAVE_SAMPLES);
                for (int w = 0; w < VIZ_WAVE_SAMPLES; w++) {
                    int idx = (readStart + w * wstep) % RING_CAP;
                    g_waveform[w].store(ringBuf[idx], std::memory_order_relaxed);
                }
            }

            for (int i = 0; i < currentFftSize; i++) {
                re[i] = ringBuf[(readStart + i) % RING_CAP] * g_hannWindow[i];
                im[i] = 0.f;
            }
            ringCount -= currentFftSize / 2;
            VizFFT(re, im);

            float t_sens = g_settings.sensitivity / 100.0f;
            float sliderGain = (t_sens <= 1.0f) ? 0.25f + t_sens * t_sens * 2.75f
                                                : 3.0f + (t_sens - 1.0f) * 4.0f;
            auto eq = GetVizEQMultipliers(g_settings.eq);

            static constexpr float BAND_SENSITIVITY[VIZ_NUM_BANDS] = {
                0.30f, 0.22f, 0.12f, 0.06f, 0.030f, 0.018f, 0.010f};

            float maxMag = 0.f;
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                int bStart = g_logBinStart[b];
                int bEnd = g_logBinStart[b + 1];
                if (bEnd <= bStart) bEnd = bStart + 1;

                float sumSq = 0.f;
                int count = 0;
                for (int k = bStart; k < bEnd; k++) {
                    sumSq += re[k] * re[k] + im[k] * im[k];
                    count++;
                }
                float rms = (count > 0) ? sqrtf(sumSq / (float)count) : 0.f;
                float eqM = (VIZ_BAND_EQ_ZONE[b] == 0)   ? eq.low
                            : (VIZ_BAND_EQ_ZONE[b] == 1) ? eq.mid
                                                         : eq.high;
                float mag = std::max(
                    0.f, std::min(1.f, (rms / (currentFftSize * 0.5f)) / BAND_SENSITIVITY[b] *
                                           sliderGain * eqM));

                bandEnv[b] = (mag >= bandEnv[b]) ? mag : std::max(0.f, bandEnv[b] - GRAVITY[b]);
                g_bands[b].store(bandEnv[b], std::memory_order_relaxed);
                maxMag = std::max(maxMag, bandEnv[b]);
            }

            if (maxMag > 0.03f) {
                g_lastAudibleTickMs.store(GetTickCount64(), std::memory_order_relaxed);
            }

            if (g_settings.beatFlashEnabled) {
                static float prevBassEnv = 0.f;
                if (bandEnv[0] - prevBassEnv > 0.12f) {
                    g_beatPulse.store(1.0f, std::memory_order_relaxed);
                }
                prevBassEnv = bandEnv[0];
            }

            // Dominant-frequency readout: only searched when the setting is on,
            // and only over the audible range actually representable at this
            // sample rate/FFT size, so this never runs by default.
            if (g_settings.peakFreqEnabled) {
                int nyquistBin = currentFftSize / 2;
                int loBin = std::max(1, (int)(20.f * currentFftSize / (float)sampleRate));
                int hiBin = std::min(nyquistBin - 1,
                                     (int)(20000.f * currentFftSize / (float)sampleRate));
                int bestBin = -1;
                float bestMag = 0.f;
                for (int k = loBin; k <= hiBin; k++) {
                    float mag = re[k] * re[k] + im[k] * im[k];
                    if (mag > bestMag) { bestMag = mag; bestBin = k; }
                }
                if (bestBin > 0 && sqrtf(bestMag) / (currentFftSize * 0.5f) > 0.02f) {
                    float hz = (float)bestBin * (float)sampleRate / (float)currentFftSize;
                    g_dominantFreqHz.store(hz, std::memory_order_relaxed);
                }
            }
        }
    }

    if (pClient) pClient->Stop();
    if (notifyRegistered) pEnum->UnregisterEndpointNotificationCallback(notifyClient);
    notifyClient->Release();
    CoUninitialize();
}

void StartVizCaptureThread() {
    if (g_captureRunning.load()) return;
    if (g_captureThread) {
        if (g_captureThread->joinable()) g_captureThread->join();
        delete g_captureThread;
        g_captureThread = nullptr;
    }
    if (!g_captureEvent) g_captureEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_captureRunning.store(true);
    g_captureThread = new std::thread(VizCaptureThreadProc);
    HANDLE hCaptureThread = g_captureThread->native_handle();
    SetThreadDescription(hCaptureThread, L"TourneTable-Capture");
}

void StopVizCaptureThread() {
    g_captureRunning.store(false);
    if (g_captureEvent) SetEvent(g_captureEvent);
    if (g_captureThread) {
        if (g_captureThread->joinable()) g_captureThread->join();
        delete g_captureThread;
        g_captureThread = nullptr;
    }
    if (g_captureEvent) {
        CloseHandle(g_captureEvent);
        g_captureEvent = nullptr;
    }
    for (int i = 0; i < VIZ_NUM_BANDS; i++) g_bands[i].store(0.f);
}

void UpdateVisualizerTargets() {
    const int vizBars = std::max(1, std::min(g_settings.barCount, VIZ_BARS_MAX));

    float bands[VIZ_NUM_BANDS];
    float masterPeak = 0.f;
    for (int i = 0; i < VIZ_NUM_BANDS; i++) {
        bands[i] = g_bands[i].load(std::memory_order_relaxed);
        masterPeak = std::max(masterPeak, bands[i]);
    }

    auto eq = GetVizEQMultipliers(g_settings.eq);

    auto sampleBands = [&](float t) -> float {
        float pos = t * (VIZ_NUM_BANDS - 1);
        int lo = (int)pos;
        int hi = std::min(lo + 1, VIZ_NUM_BANDS - 1);
        return bands[lo] * (1.f - (pos - (float)lo)) + bands[hi] * (pos - (float)lo);
    };
    auto eqForT = [&](float t) -> float {
        return (t < 0.33f) ? eq.low : (t < 0.66f) ? eq.mid : eq.high;
    };

    auto warpT = [&](float pos) -> float {
        pos = std::max(0.f, std::min(1.f, pos));
        switch (g_settings.freqScale) {
            case VizFreqScale::Linear: {
                float hz = 20.f + pos * (20000.f - 20.f);
                return HzToBandPos(hz) / (float)(VIZ_NUM_BANDS - 1);
            }
            case VizFreqScale::Mel: {
                auto melOf = [](float f) { return 2595.f * log10f(1.f + f / 700.f); };
                float melMin = melOf(20.f), melMax = melOf(20000.f);
                float mel = melMin + pos * (melMax - melMin);
                float hz = 700.f * (powf(10.f, mel / 2595.f) - 1.f);
                return HzToBandPos(hz) / (float)(VIZ_NUM_BANDS - 1);
            }
            default:  // Log -- matches the original band-index-linear mapping
                return pos;
        }
    };

    float t = (float)(GetTickCount64() - g_startTick) * 0.001f;
    float center = (vizBars - 1) * 0.5f;

    for (int i = 0; i < vizBars; i++) {
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;

        switch (g_settings.shape) {
            case VizShape::Stereo:
                { float wt = warpT(freqT); target = sampleBands(wt) * eqForT(wt); }
                break;
            case VizShape::Mountain: {
                float dist = fabsf((float)i - center) / std::max(1.f, center);
                float wd = warpT(dist);
                float energy = sampleBands(wd) * eqForT(wd);
                float taper = 1.6f - dist * 0.9f;
                target = std::max(0.f, std::min(1.f, (energy + masterPeak * (0.2f - dist * 0.12f)) * taper));
                break;
            }
            case VizShape::Mirror: {
                float mirT = 1.f - fabsf((float)i - center) / std::max(1.f, center);
                float wm = warpT(mirT);
                float energy = sampleBands(wm) * eqForT(wm);
                target = std::max(0.f, std::min(1.f, (energy + masterPeak * (0.1f + mirT * 0.12f)) * 1.3f));
                break;
            }
            case VizShape::Wave: {
                float phase = (float)i * (2.f * VIZ_PI / (float)vizBars);
                float wave = 0.55f + 0.45f * sinf(t * 3.5f - phase);
                float wf = warpT(freqT);
                float energy = sampleBands(wf) * eqForT(wf);
                target = std::max(0.f, std::min(1.f, energy * wave + masterPeak * 0.15f));
                break;
            }
            case VizShape::Breathe: {
                if (i == 0) {
                    float k = (masterPeak > g_vizBreatheEnv) ? 0.04f : 0.015f;
                    g_vizBreatheEnv += (masterPeak - g_vizBreatheEnv) * k;
                }
                float rate = 0.55f + VIZ_SEEDS[i % VIZ_BARS_MAX] * 0.18f;
                float inhale = 0.5f + 0.5f * sinf(t * rate + VIZ_SEEDS[i % VIZ_BARS_MAX] * 1.2f);
                target = std::max(0.f, std::min(1.f, inhale * (0.12f + g_vizBreatheEnv * 0.88f)));
                break;
            }
            case VizShape::Dots:
                { float wt = warpT(freqT); target = sampleBands(wt) * eqForT(wt); }
                break;
            case VizShape::Radial:
                { float wt = warpT(freqT); target = sampleBands(wt) * eqForT(wt); }
                break;
            case VizShape::Oscilloscope:
                target = 0.f;  // drawn directly from the raw waveform buffer, not per-bar targets
                break;
        }

        g_vizTarget[i] = std::max(0.f, std::min(1.f, target));
    }
}

struct RGBA { BYTE a, r, g, b; };

RGBA LerpColor(RGBA a, RGBA b, float t) {
    auto L = [](BYTE x, BYTE y, float tt) -> BYTE {
        return (BYTE)((int)x + (int)((float)((int)y - (int)x) * tt));
    };
    return {L(a.a, b.a, t), L(a.r, b.r, t), L(a.g, b.g, t), L(a.b, b.b, t)};
}

RGBA HSVtoRGB(float h, float s, float v, BYTE alpha) {
    h = fmodf(h, 360.f);
    if (h < 0) h += 360.f;
    float c = v * s;
    float x = c * (1.f - fabsf(fmodf(h / 60.0f, 2.f) - 1.f));
    float m = v - c;
    float r, g, b;
    if (h < 60)       { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else              { r = c; g = 0; b = x; }
    return {alpha, (BYTE)((r + m) * 255.f), (BYTE)((g + m) * 255.f), (BYTE)((b + m) * 255.f)};
}

bool InitDirectX() {
    HRESULT hr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0,
                           D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: 0x%08X", hr);
        return false;
    }

    hr = g_d3dDevice.As(&g_dxgiDevice);
    if (FAILED(hr)) return false;

    {
        ComPtr<IDXGIDevice1> dxgiDevice1;
        if (SUCCEEDED(g_dxgiDevice.As(&dxgiDevice1)) && dxgiDevice1) {
            dxgiDevice1->SetMaximumFrameLatency(1);
        }
    }

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_dxgiFactory));
    if (FAILED(hr)) return false;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&g_d2dFactory));
    if (FAILED(hr)) return false;

    hr = g_d2dFactory->CreateDevice(g_dxgiDevice.Get(), &g_d2dDevice);
    if (FAILED(hr)) return false;

    D2D1_STROKE_STYLE_PROPERTIES capProps = D2D1::StrokeStyleProperties(
        D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND);
    g_d2dFactory->CreateStrokeStyle(&capProps, nullptr, 0, &g_roundCapStrokeStyle);

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)g_dwriteFactory.GetAddressOf());

    return true;
}

void UninitDirectX() {
    g_dwriteTextFormat.Reset();
    g_dwriteFactory.Reset();
    g_roundCapStrokeStyle.Reset();
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
    g_dwriteTextFormatFontSize = -1;
    g_dwriteTextFormatFontName.clear();
}

bool RecreateVisualResources();

// Geometry of the visualizer, resolved once and used both to size the swap
// chain and to draw into it.
//
// The swap chain used to span the entire desktop even though the widget
// occupies a thin strip of it, which meant every frame cleared and presented
// millions of untouched pixels and held two full-screen buffers in VRAM. It is
// now sized to the widget's bounding box, with the composition visual offset to
// position it -- so drawing happens in local coordinates starting at the box.
struct VizLayout {
    float originX, originY;         // virtual-screen coords of the box's top-left
    UINT  width, height;            // swap chain size, in pixels
    float blockX, blockY;           // bar-group origin, in LOCAL coords
    float totalWidth, totalHeight;  // bar-group extent
    float textSide;                 // horizontal room reserved for overlay text
    float textTop;                  // vertical room reserved above the bars
    float textBottom;               // vertical room reserved below the bars
};

bool ComputeVizLayout(VizLayout* out) {
    if (!out) return false;

    int barCount  = std::max(1, std::min(g_settings.barCount, VIZ_BARS_MAX));
    float barW    = (float)std::max(1, g_settings.barWidth) * g_dpiScale;
    float barGap  = (float)std::max(0, g_settings.barGap) * g_dpiScale;
    float maxSize = (float)std::max(2, g_settings.barMaxSize) * g_dpiScale;

    bool horizontal = (g_settings.orientation == VizOrientation::Horizontal);
    float barsThickness = barCount * barW + (barCount - 1) * barGap;

    float totalWidth, totalHeight;
    if (g_settings.shape == VizShape::Radial) {
        totalWidth = totalHeight = maxSize * 2.0f;
    } else {
        totalWidth  = horizontal ? barsThickness : maxSize;
        totalHeight = horizontal ? maxSize       : barsThickness;
    }

    HMONITOR monitor = g_cachedMonitor;
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return false;

    int vsx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vsy = GetSystemMetrics(SM_YVIRTUALSCREEN);

    float waLeft   = (float)(mi.rcWork.left   - vsx);
    float waTop    = (float)(mi.rcWork.top    - vsy);
    float workWidth  = (float)(mi.rcWork.right  - mi.rcWork.left);
    float workHeight = (float)(mi.rcWork.bottom - mi.rcWork.top);

    float blockX = waLeft + (workWidth  - totalWidth)  * (g_settings.horizontalPosition / 100.0f);
    float blockY = waTop  + (workHeight - totalHeight) * (g_settings.verticalPosition   / 100.0f);

    float pad = g_settings.backgroundEnabled ? (float)g_settings.bgPadding * g_dpiScale : 0.f;

    // Safety margin so nothing gets clipped at the edges of the smaller target:
    // stroked shapes (oscilloscope, radial) centre their line on the path and so
    // bleed half a stroke outward, and the radial shape's spokes reach further
    // than its nominal box (inner radius plus full bar length).
    float margin = std::max(4.0f * g_dpiScale, barW);
    if (g_settings.shape == VizShape::Radial) margin += maxSize * 0.15f;

    // Text overlays can sit outside the bars, so the box reserves room for them.
    // Without this they fall outside the render surface and get clipped away.
    float fontPx = (float)std::max(6, g_settings.nowPlayingFontSize) * g_dpiScale;
    float textTop = 0.f, textBottom = 0.f, textSide = 0.f;

    if (g_settings.nowPlayingEnabled) {
        textTop  = std::max(textTop, fontPx * 1.6f + 8.0f * g_dpiScale);
        textSide = std::max(textSide, 100.0f * g_dpiScale);
    }
    if (g_settings.peakFreqEnabled) {
        float pfH = fontPx * 1.4f + 4.0f * g_dpiScale;
        if (g_settings.peakFreqAlignV == VizTextAlignV::Above)
            textTop = std::max(textTop, pfH);
        else if (g_settings.peakFreqAlignV == VizTextAlignV::Below)
            textBottom = std::max(textBottom, pfH);
        textSide = std::max(textSide, 60.0f * g_dpiScale);
    }

    float insetL = pad + margin + textSide;
    float insetT = pad + margin + textTop;
    float insetR = pad + margin + textSide;
    float insetB = pad + margin + textBottom;

    float rawOriginX = blockX - insetL;
    float rawOriginY = blockY - insetT;

    // The composition visual's offset MUST land on whole pixels. A fractional
    // offset makes DirectComposition resample the entire surface bilinearly,
    // which softens every edge and reads as an unwanted blur -- even with the
    // blur effect switched off entirely.
    //
    // The offset is floored to an integer and the leftover fraction is folded
    // back into the local drawing origin, so the widget still lands exactly
    // where the position settings ask for, without resampling the surface.
    out->originX = floorf(rawOriginX);
    out->originY = floorf(rawOriginY);

    float fracX = rawOriginX - out->originX;
    float fracY = rawOriginY - out->originY;

    out->blockX      = insetL + fracX;   // local coords
    out->blockY      = insetT + fracY;
    out->totalWidth  = totalWidth;
    out->totalHeight = totalHeight;
    out->textSide    = textSide;
    out->textTop     = textTop;
    out->textBottom  = textBottom;

    // +1 px of slack absorbs the sub-pixel offset folded in above.
    out->width  = (UINT)std::max(1.0f, ceilf(totalWidth  + insetL + insetR) + 1.0f);
    out->height = (UINT)std::max(1.0f, ceilf(totalHeight + insetT + insetB) + 1.0f);
    return true;
}

bool CreateSwapChainResources() {
    HRESULT hr;

    // Monitor and DPI must be resolved before the layout, since bar sizes are
    // DPI-scaled and the box is positioned against the monitor's work area.
    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    g_cachedMonitor = monitor;
    g_dpiScale = GetMonitorDpiScale(monitor);

    VizLayout layout;
    if (!ComputeVizLayout(&layout)) return false;

    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Width = layout.width;
    scd.Height = layout.height;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.Scaling = DXGI_SCALING_STRETCH;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

    hr = g_dxgiFactory->CreateSwapChainForComposition(g_dxgiDevice.Get(), &scd, nullptr,
                                                      &g_swapChain);
    if (FAILED(hr)) return false;

    hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_dc);
    if (FAILED(hr)) return false;

    ComPtr<IDXGISurface2> surface;
    hr = g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface));
    if (FAILED(hr)) return false;

    D2D1_BITMAP_PROPERTIES1 bitmapProperties = {};
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bitmapProperties.bitmapOptions =
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

    ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProperties, &targetBitmap);
    if (FAILED(hr)) return false;

    g_dc->SetTarget(targetBitmap.Get());

    hr = DCompositionCreateDevice(g_dxgiDevice.Get(), IID_PPV_ARGS(&g_compositionDevice));
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->CreateTargetForHwnd(g_overlayWnd, TRUE, &g_compositionTarget);
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->CreateVisual(&g_compositionVisual);
    if (FAILED(hr)) return false;

    hr = g_compositionVisual->SetContent(g_swapChain.Get());
    if (FAILED(hr)) return false;

    // Position the (now much smaller) swap chain where the widget belongs.
    // The overlay window spans the whole virtual desktop starting at its origin,
    // so window-client coordinates match the virtual-screen-relative coordinates
    // the layout produces.
    g_compositionVisual->SetOffsetX(layout.originX);
    g_compositionVisual->SetOffsetY(layout.originY);

    hr = g_compositionTarget->SetRoot(g_compositionVisual.Get());
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->Commit();
    if (FAILED(hr)) return false;

    g_swapChainWidth  = layout.width;
    g_swapChainHeight = layout.height;
    g_visualOffsetX   = layout.originX;
    g_visualOffsetY   = layout.originY;

    return RecreateVisualResources();
}

// Re-sizes and re-positions the swap chain when the widget's bounding box
// changes (settings edits, display changes). Does nothing when the box is
// unchanged, so it is cheap to call defensively.
void UpdateSwapChainForLayout() {
    if (!g_swapChain || !g_dc || !g_compositionVisual) return;

    VizLayout layout;
    if (!ComputeVizLayout(&layout)) return;

    bool sizeChanged = (layout.width != g_swapChainWidth || layout.height != g_swapChainHeight);
    bool moved = (fabsf(layout.originX - g_visualOffsetX) > 0.5f ||
                  fabsf(layout.originY - g_visualOffsetY) > 0.5f);
    if (!sizeChanged && !moved) return;

    if (sizeChanged) {
        g_dc->SetTarget(nullptr);
        if (FAILED(g_swapChain->ResizeBuffers(0, layout.width, layout.height,
                                              DXGI_FORMAT_UNKNOWN, 0)))
            return;

        ComPtr<IDXGISurface2> surface;
        if (FAILED(g_swapChain->GetBuffer(0, IID_PPV_ARGS(&surface)))) return;

        D2D1_BITMAP_PROPERTIES1 bp = {};
        bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

        ComPtr<ID2D1Bitmap1> targetBitmap;
        if (FAILED(g_dc->CreateBitmapFromDxgiSurface(surface.Get(), bp, &targetBitmap))) return;

        g_dc->SetTarget(targetBitmap.Get());
        g_swapChainWidth  = layout.width;
        g_swapChainHeight = layout.height;
    }

    if (moved) {
        g_compositionVisual->SetOffsetX(layout.originX);
        g_compositionVisual->SetOffsetY(layout.originY);
        g_visualOffsetX = layout.originX;
        g_visualOffsetY = layout.originY;
    }

    if (g_compositionDevice) g_compositionDevice->Commit();

    // The cached blur is baked for one specific box, so a box that has moved or
    // resized leaves it showing the wrong slice of wallpaper. Re-bake via the
    // existing wallpaper-refresh timer rather than inline, since re-capturing
    // involves a present and a DwmFlush.
    if (g_messageWnd && g_settings.backgroundEnabled && g_settings.bgBlur > 0) {
        SetTimer(g_messageWnd, TIMER_ID_MSG_WALLPAPER_REFRESH, 200, nullptr);
    }
}

FILETIME GetWallpaperFileTime() {
    WCHAR path[MAX_PATH] = {};
    SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0);
    FILETIME ft = {};
    HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        GetFileTime(hFile, nullptr, nullptr, &ft);
        CloseHandle(hFile);
    }
    return ft;
}

void CaptureWallpaperBitmap() {
    g_wallpaperBitmap.Reset();
    if (!g_overlayWnd || !g_dc || !g_swapChain) return;

    g_lastWallpaperTime = GetWallpaperFileTime();

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
    g_dc->EndDraw();
    g_swapChain->Present(1, 0);
    DwmFlush();

    HWND hParent = GetParent(g_overlayWnd);
    if (!hParent) return;

    RECT rc;
    GetClientRect(hParent, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;

    HWND hSource = FindWindow(L"Progman", nullptr);
    if (!hSource) hSource = hParent;

    HDC hdcScreen = GetDC(nullptr);
    if (!hdcScreen) return;
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    if (!hdcMem) { ReleaseDC(nullptr, hdcScreen); return; }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    if (!hBmp) { DeleteDC(hdcMem); ReleaseDC(nullptr, hdcScreen); return; }

    HGDIOBJ hOldBmp = SelectObject(hdcMem, hBmp);

    if (!PrintWindow(hSource, hdcMem, 0x02 /*PW_RENDERFULLCONTENT*/)) {
        SelectObject(hdcMem, hOldBmp);
        DeleteObject(hBmp);
        DeleteDC(hdcMem);
        ReleaseDC(nullptr, hdcScreen);
        return;
    }
    GdiFlush();

    BYTE* pixels = static_cast<BYTE*>(pvBits);
    for (int i = 0; i < w * h; i++) pixels[i * 4 + 3] = 255;

    D2D1_BITMAP_PROPERTIES bitmapProps = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    g_dc->CreateBitmap(D2D1::SizeU(w, h), pvBits, w * 4, bitmapProps, &g_wallpaperBitmap);

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

void ReleaseVisualResources() {
    g_blurEffect.Reset();
    g_blurredBitmap.Reset();
    g_wallpaperBitmap.Reset();
    g_borderBrush.Reset();
    g_backgroundBrush.Reset();
    g_barBrush.Reset();
    g_barBrush2.Reset();
    g_bgGeoCache.Reset();
    g_borderRingCache.Reset();
    g_bgGeoCacheRect = D2D1::RectF(-1, -1, -1, -1);
    g_bgGeoCacheRadii[0] = g_bgGeoCacheRadii[1] =
        g_bgGeoCacheRadii[2] = g_bgGeoCacheRadii[3] = -1.f;
    g_borderCacheBorderSize = -1;
    g_nowPlayingBrush.Reset();
}

void ReleaseSwapChainResources() {
    ReleaseVisualResources();
    g_compositionVisual.Reset();
    g_compositionTarget.Reset();
    g_compositionDevice.Reset();
    g_dc.Reset();
    g_swapChain.Reset();
}

bool RecreateVisualResources() {
    HRESULT hr;

    D2D1_COLOR_F barColor = D2D1::ColorF(g_settings.colorR / 255.0f, g_settings.colorG / 255.0f,
                                         g_settings.colorB / 255.0f, g_settings.colorA / 255.0f);
    hr = g_dc->CreateSolidColorBrush(barColor, &g_barBrush);
    if (FAILED(hr)) return false;

    D2D1_COLOR_F barColor2 = D2D1::ColorF(g_settings.grad2R / 255.0f, g_settings.grad2G / 255.0f,
                                          g_settings.grad2B / 255.0f, g_settings.grad2A / 255.0f);
    g_dc->CreateSolidColorBrush(barColor2, &g_barBrush2);

    if (g_settings.backgroundEnabled) {
        D2D1_COLOR_F bgColor = D2D1::ColorF(g_settings.bgR / 255.0f, g_settings.bgG / 255.0f,
                                            g_settings.bgB / 255.0f, g_settings.bgA / 255.0f);
        g_dc->CreateSolidColorBrush(bgColor, &g_backgroundBrush);

        if (g_settings.bgBlur > 0) {
            CaptureWallpaperBitmap();
            if (g_wallpaperBitmap) {
                hr = g_dc->CreateEffect(CLSID_D2D1GaussianBlur, &g_blurEffect);
                if (SUCCEEDED(hr)) {
                    g_blurEffect->SetInput(0, g_wallpaperBitmap.Get());
                    g_blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
                                           (FLOAT)g_settings.bgBlur);
                    g_blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
                                           D2D1_BORDER_MODE_HARD);

                    // Evaluate the blur exactly once, into a bitmap covering only
                    // the widget's bounding box rather than the whole desktop.
                    //
                    // The blur still SAMPLES the full-size wallpaper (it has to --
                    // a Gaussian reads neighbouring pixels from beyond the box's
                    // edges), but only the box-sized result is kept. At 1080p that
                    // is roughly 8 MB of video memory replaced by tens of KB.
                    VizLayout blurLayout;
                    if (ComputeVizLayout(&blurLayout)) {
                        D2D1_SIZE_U blurSize = D2D1::SizeU(blurLayout.width, blurLayout.height);

                        D2D1_BITMAP_PROPERTIES1 blurProps = {};
                        blurProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
                        blurProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
                        blurProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;

                        if (SUCCEEDED(g_dc->CreateBitmap(blurSize, nullptr, 0, blurProps,
                                                          &g_blurredBitmap))) {
                            ComPtr<ID2D1Image> savedTarget;
                            g_dc->GetTarget(&savedTarget);
                            g_dc->SetTarget(g_blurredBitmap.Get());
                            g_dc->BeginDraw();
                            g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
                            // Shift the wallpaper so the slice under the widget
                            // lands at the origin of this small target.
                            g_dc->SetTransform(D2D1::Matrix3x2F::Translation(
                                -blurLayout.originX, -blurLayout.originY));
                            g_dc->DrawImage(g_blurEffect.Get());
                            g_dc->SetTransform(D2D1::Matrix3x2F::Identity());
                            HRESULT hrBlur = g_dc->EndDraw();
                            g_dc->SetTarget(savedTarget.Get());
                            if (FAILED(hrBlur)) g_blurredBitmap.Reset();
                        }
                    }
                }
            }
            // Once the blur is baked, neither the effect graph nor the full-size
            // source wallpaper bitmap is needed again -- releasing both frees the
            // memory they were holding for the life of the overlay.
            if (g_blurredBitmap) {
                g_blurEffect.Reset();
                g_wallpaperBitmap.Reset();
            }
        }

        if (g_settings.bgBorderSize > 0) {
            D2D1_COLOR_F borderColor =
                D2D1::ColorF(g_settings.borderR / 255.0f, g_settings.borderG / 255.0f,
                            g_settings.borderB / 255.0f, g_settings.borderA / 255.0f);
            g_dc->CreateSolidColorBrush(borderColor, &g_borderBrush);
        }
    }

    if (g_settings.nowPlayingEnabled || g_settings.peakFreqEnabled) {
        D2D1_COLOR_F npColor =
            D2D1::ColorF(g_settings.nowPlayingR / 255.0f, g_settings.nowPlayingG / 255.0f,
                        g_settings.nowPlayingB / 255.0f, g_settings.nowPlayingA / 255.0f);
        g_dc->CreateSolidColorBrush(npColor, &g_nowPlayingBrush);

        // The user's locale rather than a hardcoded en-us: this drives font
        // fallback and shaping for non-Latin track titles.
        WCHAR localeName[LOCALE_NAME_MAX_LENGTH] = L"en-us";
        GetUserDefaultLocaleName(localeName, ARRAYSIZE(localeName));

        int fontSize = std::max(6, g_settings.nowPlayingFontSize);
        if (g_dwriteFactory && (!g_dwriteTextFormat || g_dwriteTextFormatFontSize != fontSize ||
                                 g_dwriteTextFormatFontName != g_settings.nowPlayingFont)) {
            g_dwriteTextFormat.Reset();
            HRESULT hrText = g_dwriteFactory->CreateTextFormat(
                g_settings.nowPlayingFont.c_str(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                (FLOAT)fontSize * g_dpiScale, localeName, &g_dwriteTextFormat);
            if (SUCCEEDED(hrText)) {
                g_dwriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                g_dwriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                g_dwriteTextFormatFontSize = fontSize;
                g_dwriteTextFormatFontName = g_settings.nowPlayingFont;
            }
        }
    }

    return true;
}


void FillRoundedRectPerCorner(ID2D1DeviceContext* dc, ID2D1Factory1* factory,
                               const D2D1_RECT_F& r, ID2D1Brush* brush,
                               float rTL, float rTR, float rBR, float rBL) {
    if (rTL == rTR && rTR == rBR && rBR == rBL) {
        float clampedR = std::min(rTL, std::min(r.right - r.left, r.bottom - r.top) / 2.0f);
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(r, clampedR, clampedR), brush);
        return;
    }

    float w = r.right  - r.left;
    float h = r.bottom - r.top;

    rTL = std::min(rTL, std::min(w, h) / 2.0f);
    rTR = std::min(rTR, std::min(w, h) / 2.0f);
    rBR = std::min(rBR, std::min(w, h) / 2.0f);
    rBL = std::min(rBL, std::min(w, h) / 2.0f);

    ComPtr<ID2D1PathGeometry> geo;
    if (FAILED(factory->CreatePathGeometry(&geo))) return;

    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(geo->Open(&sink))) return;

    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure(D2D1::Point2F(r.left + rTL, r.top), D2D1_FIGURE_BEGIN_FILLED);

    sink->AddLine(D2D1::Point2F(r.right - rTR, r.top));
    if (rTR > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right, r.top + rTR),
            D2D1::SizeF(rTR, rTR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.right, r.bottom - rBR));
    if (rBR > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right - rBR, r.bottom),
            D2D1::SizeF(rBR, rBR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left + rBL, r.bottom));
    if (rBL > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left, r.bottom - rBL),
            D2D1::SizeF(rBL, rBL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left, r.top + rTL));
    if (rTL > 0)
        sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left + rTL, r.top),
            D2D1::SizeF(rTL, rTL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();

    dc->FillGeometry(geo.Get(), brush);
}

HRESULT CreateRoundedRectPath(ID2D1Factory1* factory, const D2D1_RECT_F& r,
                               float rTL, float rTR, float rBR, float rBL,
                               ID2D1PathGeometry** outGeo) {
    float w = r.right - r.left, h = r.bottom - r.top;
    float maxR = std::min(w, h) / 2.0f;
    rTL = std::min(rTL, maxR); rTR = std::min(rTR, maxR);
    rBR = std::min(rBR, maxR); rBL = std::min(rBL, maxR);

    ComPtr<ID2D1PathGeometry> geo;
    HRESULT hr = factory->CreatePathGeometry(&geo);
    if (FAILED(hr)) return hr;

    ComPtr<ID2D1GeometrySink> sink;
    hr = geo->Open(&sink);
    if (FAILED(hr)) return hr;

    sink->SetFillMode(D2D1_FILL_MODE_WINDING);
    sink->BeginFigure(D2D1::Point2F(r.left + rTL, r.top), D2D1_FIGURE_BEGIN_FILLED);

    sink->AddLine(D2D1::Point2F(r.right - rTR, r.top));
    if (rTR > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right, r.top + rTR),
        D2D1::SizeF(rTR, rTR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.right, r.bottom - rBR));
    if (rBR > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.right - rBR, r.bottom),
        D2D1::SizeF(rBR, rBR), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left + rBL, r.bottom));
    if (rBL > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left, r.bottom - rBL),
        D2D1::SizeF(rBL, rBL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->AddLine(D2D1::Point2F(r.left, r.top + rTL));
    if (rTL > 0) sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(r.left + rTL, r.top),
        D2D1::SizeF(rTL, rTL), 0.0f, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    hr = sink->Close();
    if (FAILED(hr)) return hr;

    *outGeo = geo.Detach();
    return S_OK;
}

bool RectsApproxEqual(const D2D1_RECT_F& a, const D2D1_RECT_F& b) {
    auto eq = [](float x, float y) { return fabsf(x - y) < 0.01f; };
    return eq(a.left, b.left) && eq(a.top, b.top) &&
           eq(a.right, b.right) && eq(a.bottom, b.bottom);
}

void RenderVisualizer() {
    if (g_unloading || !g_dc || !g_swapChain) return;

    float sceneAlpha = 1.0f;
    if (g_settings.autoHideEnabled) {
        ULONGLONG idleMs = GetTickCount64() - g_lastAudibleTickMs.load(std::memory_order_relaxed);
        ULONGLONG delayMs = (ULONGLONG)std::max(0, g_settings.autoHideDelaySeconds) * 1000ULL;
        constexpr ULONGLONG kFadeMs = 1500;
        if (idleMs > delayMs) {
            ULONGLONG fadeElapsed = idleMs - delayMs;
            sceneAlpha = (fadeElapsed >= kFadeMs) ? 0.f : 1.0f - (float)fadeElapsed / (float)kFadeMs;
        }
    }

    if (sceneAlpha <= 0.001f) {
        // Fully faded out. Present one blank frame to clear whatever was last
        // shown, then skip the render path entirely until audio returns --
        // there is no point drawing a scene at full detail and then making it
        // invisible.
        if (g_autoHideBlanked) return;
        g_dc->BeginDraw();
        g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
        g_dc->EndDraw();
        g_swapChain->Present(0, 0);
        g_autoHideBlanked = true;
        return;
    }
    g_autoHideBlanked = false;

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    UpdateVisualizerTargets();

    int barCount = std::max(1, std::min(g_settings.barCount, VIZ_BARS_MAX));
    float barW = (float)std::max(1, g_settings.barWidth) * g_dpiScale;
    float barGap = (float)std::max(0, g_settings.barGap) * g_dpiScale;
    float maxSize = (float)std::max(2, g_settings.barMaxSize) * g_dpiScale;
    float idleSize = (float)std::max(0, g_settings.barIdleSize) * g_dpiScale;
    float rTL = g_settings.barRadiusTL * g_dpiScale;
    float rTR = g_settings.barRadiusTR * g_dpiScale;
    float rBR = g_settings.barRadiusBR * g_dpiScale;
    float rBL = g_settings.barRadiusBL * g_dpiScale;

    float attack = 0.55f, decay = 0.18f;
    switch (g_settings.shape) {
        case VizShape::Stereo:  attack = 0.72f; decay = 0.22f; break;
        case VizShape::Mirror:  attack = 0.52f; decay = 0.20f; break;
        case VizShape::Wave:    attack = 0.34f; decay = 0.17f; break;
        case VizShape::Breathe: attack = 0.20f; decay = 0.11f; break;
        default: break;
    }

    bool horizontal = (g_settings.orientation == VizOrientation::Horizontal);
    
    float barsThickness = barCount * barW + (barCount - 1) * barGap;
    
    float groupThickness = barsThickness;
    float groupExtent    = maxSize;

    float totalWidth, totalHeight;
    if (g_settings.shape == VizShape::Radial) {
        totalWidth = totalHeight = maxSize * 2.0f;
    } else {
        totalWidth  = horizontal ? groupThickness : groupExtent;
        totalHeight = horizontal ? groupExtent    : groupThickness;
    }

    float animTime = (float)(GetTickCount64() - g_startTick) * 0.001f;
    {
        float pulse = g_beatPulse.load(std::memory_order_relaxed);
        if (pulse > 0.f) g_beatPulse.store(std::max(0.f, pulse - 0.08f), std::memory_order_relaxed);
    }

    bool useFadeLayer = sceneAlpha < 0.999f;

    VizLayout layout;
    if (ComputeVizLayout(&layout)) {
        if (useFadeLayer) {
            g_dc->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr,
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), sceneAlpha), nullptr);
        }

        // Drawing happens in swap-chain-local coordinates. The swap chain covers
        // only the widget's bounding box, and the composition visual carries the
        // offset that places it on screen.
        float blockX = layout.blockX;
        float blockY = layout.blockY;
        totalWidth   = layout.totalWidth;
        totalHeight  = layout.totalHeight;

        // Publish the bounds of what is actually VISIBLE, for the occlusion
        // check -- not the full render surface. The surface reserves invisible
        // insets (a safety margin, and room above/beside the bars for the Now
        // Playing label), and near a screen edge those insets push the surface
        // off-screen entirely. Testing that larger box would mean no window
        // could ever fully contain it, and the check would never fire.
        {
            int virtualScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
            int virtualScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
            float pad = g_settings.backgroundEnabled
                            ? (float)g_settings.bgPadding * g_dpiScale : 0.f;
            float visL = layout.originX + blockX - pad;
            float visT = layout.originY + blockY - pad;
            float visR = layout.originX + blockX + totalWidth + pad;
            float visB = layout.originY + blockY + totalHeight + pad;
            g_drawRectL.store((LONG)visL + virtualScreenX, std::memory_order_relaxed);
            g_drawRectT.store((LONG)visT + virtualScreenY, std::memory_order_relaxed);
            g_drawRectR.store((LONG)visR + virtualScreenX, std::memory_order_relaxed);
            g_drawRectB.store((LONG)visB + virtualScreenY, std::memory_order_relaxed);
            g_drawRectValid.store(true, std::memory_order_relaxed);
        }

        if (g_backgroundBrush) {
            float padding = (float)g_settings.bgPadding * g_dpiScale;
            float bgWidth  = totalWidth  + 2 * padding;
            float bgHeight = totalHeight + 2 * padding;

            float bgTL = g_settings.bgRadiusTL * g_dpiScale;
            float bgTR = g_settings.bgRadiusTR * g_dpiScale;
            float bgBR = g_settings.bgRadiusBR * g_dpiScale;
            float bgBL = g_settings.bgRadiusBL * g_dpiScale;

            D2D1_RECT_F bgRect = D2D1::RectF(blockX - padding, blockY - padding,
                                              blockX + totalWidth + padding,
                                              blockY + totalHeight + padding);

            bool bgDirty = !g_bgGeoCache ||
                           !RectsApproxEqual(bgRect, g_bgGeoCacheRect) ||
                           g_bgGeoCacheRadii[0] != bgTL || g_bgGeoCacheRadii[1] != bgTR ||
                           g_bgGeoCacheRadii[2] != bgBR || g_bgGeoCacheRadii[3] != bgBL;

            if (bgDirty) {
                g_bgGeoCache.Reset();
                g_borderRingCache.Reset();
                CreateRoundedRectPath(g_d2dFactory.Get(), bgRect, bgTL, bgTR, bgBR, bgBL,
                                      &g_bgGeoCache);
                g_bgGeoCacheRect = bgRect;
                g_bgGeoCacheRadii[0] = bgTL; g_bgGeoCacheRadii[1] = bgTR;
                g_bgGeoCacheRadii[2] = bgBR; g_bgGeoCacheRadii[3] = bgBL;
            }

            ID2D1PathGeometry* bgGeo = g_bgGeoCache.Get();

            if (g_blurredBitmap && bgGeo) {
                g_dc->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), bgGeo),
                                nullptr);
                // The cached blur is already box-sized and box-aligned, so this
                // is a straight 1:1 blit with no scaling or resampling.
                g_dc->DrawBitmap(g_blurredBitmap.Get());
                g_dc->PopLayer();
            }

            if (bgGeo)
                g_dc->FillGeometry(bgGeo, g_backgroundBrush.Get());

            if (g_borderBrush) {
                float bw = std::min((float)g_settings.bgBorderSize * g_dpiScale,
                                    std::min(bgWidth, bgHeight) / 2.0f);

                if (bgDirty || g_borderCacheBorderSize != g_settings.bgBorderSize) {
                    g_borderRingCache.Reset();

                    D2D1_RECT_F innerRect = D2D1::RectF(bgRect.left + bw, bgRect.top + bw,
                                                         bgRect.right - bw, bgRect.bottom - bw);
                    float iTL = std::max(0.0f, bgTL - bw);
                    float iTR = std::max(0.0f, bgTR - bw);
                    float iBR = std::max(0.0f, bgBR - bw);
                    float iBL = std::max(0.0f, bgBL - bw);

                    ComPtr<ID2D1PathGeometry> innerGeo;
                    CreateRoundedRectPath(g_d2dFactory.Get(), innerRect, iTL, iTR, iBR, iBL,
                                          &innerGeo);

                    if (bgGeo && innerGeo) {
                        ID2D1Geometry* geos[] = {bgGeo, innerGeo.Get()};
                        g_d2dFactory->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE, geos, 2,
                                                          &g_borderRingCache);
                    }
                    g_borderCacheBorderSize = g_settings.bgBorderSize;
                }

                if (g_borderRingCache)
                    g_dc->FillGeometry(g_borderRingCache.Get(), g_borderBrush.Get());
            }
        }

        RGBA c1{g_settings.colorA, g_settings.colorR, g_settings.colorG, g_settings.colorB};
        {
            if (g_settings.colorMode == VizColorMode::Accent) {
                DWORD dw = GetWindowsAccentColor();
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            } else if (g_settings.colorMode == VizColorMode::AlbumArt) {
                DWORD dw = g_albumArtColor.load(std::memory_order_relaxed);
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            } else if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
                DWORD dw = g_albumArtColor.load(std::memory_order_relaxed);
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            }
        }
        RGBA c2{g_settings.grad2A, g_settings.grad2R, g_settings.grad2G, g_settings.grad2B};
        RGBA cGrad1{g_settings.grad1A, g_settings.grad1R, g_settings.grad1G, g_settings.grad1B};
        if (g_settings.colorMode == VizColorMode::Tourne) {
            cGrad1 = {255, 20, 184, 166};
            c2     = {255, 200, 29, 51};
        }
        if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
            DWORD dw = g_albumArtColorSecondary.load(std::memory_order_relaxed);
            c2    = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            cGrad1 = c1;
        }

        if (g_settings.shape == VizShape::Dots) {
            float dotR  = barW * 0.5f;
            float step  = barW + barGap;
            float dTL = g_settings.barRadiusTL * g_dpiScale;
            float dTR = g_settings.barRadiusTR * g_dpiScale;
            float dBR = g_settings.barRadiusBR * g_dpiScale;
            float dBL = g_settings.barRadiusBL * g_dpiScale;

            for (int i = 0; i < barCount; i++) {
                float tgt = g_vizTarget[i], cur = g_vizPeak[i];
                float nxt = cur + (tgt - cur) * ((tgt > cur) ? attack : decay);
                g_vizPeak[i] = (fabsf(nxt - cur) > 0.0005f) ? nxt : tgt;
                float fac = std::max(0.f, g_vizPeak[i]);
                float colSize = idleSize + fac * std::max(0.f, maxSize - idleSize);

                if (colSize < 0.5f) continue;

                RGBA col = c1;
                if (g_settings.colorMode == VizColorMode::Gradient ||
                    g_settings.colorMode == VizColorMode::Tourne)
                    col = LerpColor(cGrad1, c2, (barCount > 1) ? (float)i/(barCount-1) : 0.f);
                else if (g_settings.colorMode == VizColorMode::ReactiveGradient)
                    col = LerpColor(cGrad1, c2, fac);
                else if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
                    float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                    float freqT = std::min(1.f, t * 0.6f + fac * 0.4f);
                    col = LerpColor(cGrad1, c2, freqT);
                } else if (g_settings.colorMode == VizColorMode::RainbowCycle) {
                    float hue = fmodf(animTime * g_settings.rainbowSpeed +
                                       (barCount > 1 ? (float)i/(barCount-1) : 0.f) * 360.f, 360.f);
                    col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(30, std::min(180, (int)(150.f * fac + 30.f)));
                    col = {aa, c1.r, c1.g, c1.b};
                }
                if (g_settings.beatFlashEnabled) {
                    float pulse = g_beatPulse.load(std::memory_order_relaxed);
                    if (pulse > 0.001f) {
                        float boost = 1.0f + pulse * (g_settings.beatFlashIntensity / 100.0f);
                        col.r = (BYTE)std::min(255.f, col.r * boost);
                        col.g = (BYTE)std::min(255.f, col.g * boost);
                        col.b = (BYTE)std::min(255.f, col.b * boost);
                    }
                }
                g_barBrush->SetColor(D2D1::ColorF(col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f));

                int numDots = (step > 0.5f) ? (int)(colSize / step) : 1;
                numDots = std::max(1, numDots);

                auto drawDot = [&](float cx2, float cy2) {
                    D2D1_RECT_F r = D2D1::RectF(cx2 - dotR, cy2 - dotR, cx2 + dotR, cy2 + dotR);
                    FillRoundedRectPerCorner(g_dc.Get(), g_d2dFactory.Get(), r,
                                             g_barBrush.Get(), dTL, dTR, dBR, dBL);
                };

                for (int d = 0; d < numDots; d++) {
                    if (horizontal) {
                        float cx2 = blockX + i * step + dotR;
                        switch (g_settings.verticalAnchor) {
                            case VizAnchor::Top: {
                                float cy2 = blockY + d * step + dotR;
                                if (cy2 - dotR > blockY + maxSize) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                            case VizAnchor::Middle: {
                                float centerY = blockY + maxSize * 0.5f;
                                if (d == 0) {
                                    drawDot(cx2, centerY);
                                } else {
                                    float cy2up   = centerY - d * step;
                                    float cy2down = centerY + d * step;
                                    bool upOk   = cy2up   - dotR >= blockY;
                                    bool downOk = cy2down + dotR <= blockY + maxSize;
                                    if (upOk)   drawDot(cx2, cy2up);
                                    if (downOk) drawDot(cx2, cy2down);
                                    if (!upOk && !downOk) goto next_dot_h;
                                }
                                break;
                            }
                            default: {
                                float cy2 = blockY + maxSize - d * step - dotR;
                                if (cy2 + dotR < blockY) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                        }
                        continue;
                        next_dot_h: break;
                    } else {
                        float cy2 = blockY + i * step + dotR;
                        switch (g_settings.verticalAnchor) {
                            case VizAnchor::Top: {
                                float cx2 = blockX + groupExtent - d * step - dotR;
                                if (cx2 + dotR < blockX) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                            case VizAnchor::Middle: {
                                float centerX = blockX + groupExtent * 0.5f;
                                if (d == 0) {
                                    drawDot(centerX, cy2);
                                } else {
                                    float cx2r = centerX + d * step;
                                    float cx2l = centerX - d * step;
                                    bool rOk = cx2r + dotR <= blockX + groupExtent;
                                    bool lOk = cx2l - dotR >= blockX;
                                    if (rOk) drawDot(cx2r, cy2);
                                    if (lOk) drawDot(cx2l, cy2);
                                    if (!rOk && !lOk) goto next_dot_v;
                                }
                                break;
                            }
                            default: {
                                float cx2 = blockX + d * step + dotR;
                                if (cx2 - dotR > blockX + groupExtent) break;
                                drawDot(cx2, cy2);
                                break;
                            }
                        }
                        continue;
                        next_dot_v: break;
                    }
                }
            }
        } else if (g_settings.shape == VizShape::Radial) {
            float centerX = blockX + totalWidth * 0.5f;
            float centerY = blockY + totalHeight * 0.5f;
            float innerR = maxSize * 0.15f;
            float strokeW = std::max(1.0f, barW);

            for (int i = 0; i < barCount; i++) {
                float tgt = g_vizTarget[i], cur = g_vizPeak[i];
                float next = cur + (tgt - cur) * ((tgt > cur) ? attack : decay);
                g_vizPeak[i] = (fabsf(next - cur) > 0.0005f) ? next : tgt;
                float fac = std::max(0.f, g_vizPeak[i]);
                float len = idleSize + fac * std::max(0.f, maxSize - idleSize);
                if (len < 0.5f) continue;

                RGBA col = c1;
                if (g_settings.colorMode == VizColorMode::Gradient ||
                    g_settings.colorMode == VizColorMode::Tourne)
                    col = LerpColor(cGrad1, c2, (barCount > 1) ? (float)i/(barCount-1) : 0.f);
                else if (g_settings.colorMode == VizColorMode::ReactiveGradient)
                    col = LerpColor(cGrad1, c2, fac);
                else if (g_settings.colorMode == VizColorMode::RainbowCycle) {
                    float hue = fmodf(animTime * g_settings.rainbowSpeed +
                                       (float)i / std::max(1, barCount) * 360.f, 360.f);
                    col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(30, std::min(180, (int)(150.f * fac + 30.f)));
                    col = {aa, c1.r, c1.g, c1.b};
                }
                if (g_settings.beatFlashEnabled) {
                    float pulse = g_beatPulse.load(std::memory_order_relaxed);
                    if (pulse > 0.001f) {
                        float boost = 1.0f + pulse * (g_settings.beatFlashIntensity / 100.0f);
                        col.r = (BYTE)std::min(255.f, col.r * boost);
                        col.g = (BYTE)std::min(255.f, col.g * boost);
                        col.b = (BYTE)std::min(255.f, col.b * boost);
                    }
                }
                g_barBrush->SetColor(D2D1::ColorF(col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f));

                float angle = (float)i / (float)barCount * 2.0f * VIZ_PI - (VIZ_PI * 0.5f);
                float dx = cosf(angle), dy = sinf(angle);
                D2D1_POINT_2F p0 = {centerX + dx * innerR, centerY + dy * innerR};
                D2D1_POINT_2F p1 = {centerX + dx * (innerR + len), centerY + dy * (innerR + len)};
                g_dc->DrawLine(p0, p1, g_barBrush.Get(), strokeW, g_roundCapStrokeStyle.Get());
            }
        } else if (g_settings.shape == VizShape::Oscilloscope) {
            float ampScale = maxSize * 0.5f;
            float centerY = blockY + totalHeight * 0.5f;
            float wstep = (VIZ_WAVE_SAMPLES > 1) ? totalWidth / (float)(VIZ_WAVE_SAMPLES - 1)
                                                  : totalWidth;

            RGBA col = c1;
            if (g_settings.colorMode == VizColorMode::Gradient ||
                g_settings.colorMode == VizColorMode::Tourne)
                col = LerpColor(cGrad1, c2, 0.5f);
            else if (g_settings.colorMode == VizColorMode::RainbowCycle) {
                float hue = fmodf(animTime * g_settings.rainbowSpeed, 360.f);
                col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
            }
            if (g_settings.oscilloscopeMultibandEnabled) {
                // Blends 3 reference colors (low/mid/high) by how much energy is
                // currently in each EQ zone, so the trace tints toward whichever
                // part of the spectrum is dominant right now. This overrides
                // whatever the color mode above picked, matching how RainbowCycle
                // and Tourne already take priority for this shape.
                float lowE = 0.f, midE = 0.f, highE = 0.f;
                for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                    float e = g_bands[b].load(std::memory_order_relaxed);
                    if (VIZ_BAND_EQ_ZONE[b] == 0) lowE += e;
                    else if (VIZ_BAND_EQ_ZONE[b] == 1) midE += e;
                    else highE += e;
                }
                float total = lowE + midE + highE;
                const RGBA lowCol{255, 255, 90, 60}, midCol{255, 120, 220, 90}, highCol{255, 90, 180, 255};
                if (total > 0.001f) {
                    col.r = (BYTE)((lowCol.r * lowE + midCol.r * midE + highCol.r * highE) / total);
                    col.g = (BYTE)((lowCol.g * lowE + midCol.g * midE + highCol.g * highE) / total);
                    col.b = (BYTE)((lowCol.b * lowE + midCol.b * midE + highCol.b * highE) / total);
                } else {
                    col = midCol;
                }
            }
            if (g_settings.beatFlashEnabled) {
                float pulse = g_beatPulse.load(std::memory_order_relaxed);
                if (pulse > 0.001f) {
                    float boost = 1.0f + pulse * (g_settings.beatFlashIntensity / 100.0f);
                    col.r = (BYTE)std::min(255.f, col.r * boost);
                    col.g = (BYTE)std::min(255.f, col.g * boost);
                    col.b = (BYTE)std::min(255.f, col.b * boost);
                }
            }
            g_barBrush->SetColor(D2D1::ColorF(col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f));

            float strokeW = std::max(1.0f, barW * 0.5f);
            float s0 = g_waveform[0].load(std::memory_order_relaxed);
            D2D1_POINT_2F prev = {blockX, centerY - s0 * ampScale};
            for (int w = 1; w < VIZ_WAVE_SAMPLES; w++) {
                float sample = g_waveform[w].load(std::memory_order_relaxed);
                D2D1_POINT_2F pt = {blockX + w * wstep, centerY - sample * ampScale};
                g_dc->DrawLine(prev, pt, g_barBrush.Get(), strokeW);
                prev = pt;
            }
        }
        else {
            for (int i = 0; i < barCount; i++) {
                float tgt = g_vizTarget[i], cur = g_vizPeak[i];
                float next = cur + (tgt - cur) * ((tgt > cur) ? attack : decay);
                g_vizPeak[i] = (fabsf(next - cur) > 0.0005f) ? next : tgt;

                float fac = std::max(0.f, g_vizPeak[i]);
                float size = idleSize + fac * std::max(0.f, maxSize - idleSize);

                RGBA col = c1;
                if (g_settings.colorMode == VizColorMode::Gradient ||
                    g_settings.colorMode == VizColorMode::Tourne) {
                    float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                    col = LerpColor(cGrad1, c2, t);
                } else if (g_settings.colorMode == VizColorMode::ReactiveGradient) {
                    col = LerpColor(cGrad1, c2, fac);
                } else if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
                    float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                    float freqT = std::min(1.f, t * 0.6f + fac * 0.4f);
                    col = LerpColor(cGrad1, c2, freqT);
                } else if (g_settings.colorMode == VizColorMode::RainbowCycle) {
                    float t = (barCount > 1) ? (float)i / (barCount - 1) : 0.f;
                    float hue = fmodf(animTime * g_settings.rainbowSpeed + t * 360.f, 360.f);
                    col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(30, std::min(180, (int)(150.f * fac + 30.f)));
                    col = {aa, c1.r, c1.g, c1.b};
                }
                if (g_settings.beatFlashEnabled) {
                    float pulse = g_beatPulse.load(std::memory_order_relaxed);
                    if (pulse > 0.001f) {
                        float boost = 1.0f + pulse * (g_settings.beatFlashIntensity / 100.0f);
                        col.r = (BYTE)std::min(255.f, col.r * boost);
                        col.g = (BYTE)std::min(255.f, col.g * boost);
                        col.b = (BYTE)std::min(255.f, col.b * boost);
                    }
                }

                D2D1_COLOR_F d2dCol = D2D1::ColorF(col.r / 255.0f, col.g / 255.0f, col.b / 255.0f,
                                                   col.a / 255.0f);
                g_barBrush->SetColor(d2dCol);

                float holdSize = idleSize;
                if (g_settings.peakHoldEnabled) {
                    g_vizPeakHold[i] = (fac >= g_vizPeakHold[i])
                        ? fac : std::max(0.f, g_vizPeakHold[i] - 0.012f);
                    holdSize = idleSize + g_vizPeakHold[i] * std::max(0.f, maxSize - idleSize);
                } else {
                    g_vizPeakHold[i] = 0.f;
                }
                float capThickness = std::max(1.5f, 2.0f * g_dpiScale);

                // Recolour the cap brush to match this bar. Without this the caps
                // are stuck on whatever Gradient Color 2 happens to be, which
                // ignores Solid/Rainbow/Album/Accent/Acrylic completely. A
                // brightened version of the bar colour keeps them readable
                // against the bar they sit on.
                if (g_settings.peakHoldEnabled && g_barBrush2) {
                    auto lift = [](BYTE c) -> float {
                        return std::min(1.0f, (c / 255.0f) * 1.35f + 0.12f);
                    };
                    g_barBrush2->SetColor(D2D1::ColorF(lift(col.r), lift(col.g), lift(col.b),
                                                       col.a / 255.0f));
                }

                D2D1_RECT_F barRect;
                if (horizontal) {
                    float x = blockX + i * (barW + barGap);
                    float y, yBottom, capY;
                    switch (g_settings.verticalAnchor) {
                        case VizAnchor::Top:
                            y = blockY;
                            yBottom = blockY + size;
                            capY = blockY + holdSize;
                            break;
                        case VizAnchor::Middle:
                            y = blockY + (maxSize - size) / 2.0f;
                            yBottom = y + size;
                            capY = blockY + (maxSize - holdSize) / 2.0f;
                            break;
                        default:
                            y = blockY + (maxSize - size);
                            yBottom = blockY + maxSize;
                            capY = blockY + (maxSize - holdSize);
                            break;
                    }
                    barRect = D2D1::RectF(x, y, x + barW, yBottom);
                    FillRoundedRectPerCorner(g_dc.Get(), g_d2dFactory.Get(), barRect,
                                             g_barBrush.Get(), rTL, rTR, rBR, rBL);
                    if (g_settings.peakHoldEnabled && holdSize > 0.5f) {
                        g_dc->FillRectangle(D2D1::RectF(x, capY - capThickness * 0.5f,
                                                        x + barW, capY + capThickness * 0.5f),
                                            g_barBrush2.Get());
                    }
                } else {
                    float y = blockY + i * (barW + barGap);
                    float x, xRight, capX;
                    switch (g_settings.verticalAnchor) {
                        case VizAnchor::Top:
                            xRight = blockX + groupExtent;
                            x = xRight - size;
                            capX = blockX + groupExtent - holdSize;
                            break;
                        case VizAnchor::Middle: {
                            float cx = blockX + groupExtent / 2.0f;
                            x = cx - size / 2.0f;
                            xRight = cx + size / 2.0f;
                            capX = cx - holdSize / 2.0f;
                            break;
                        }
                        default:
                            x = blockX;
                            xRight = blockX + size;
                            capX = blockX + holdSize;
                            break;
                    }
                    barRect = D2D1::RectF(x, y, xRight, y + barW);
                    FillRoundedRectPerCorner(g_dc.Get(), g_d2dFactory.Get(), barRect,
                                             g_barBrush.Get(), rTL, rTR, rBR, rBL);
                    if (g_settings.peakHoldEnabled && holdSize > 0.5f) {
                        g_dc->FillRectangle(D2D1::RectF(capX - capThickness * 0.5f, y,
                                                        capX + capThickness * 0.5f, y + barW),
                                            g_barBrush2.Get());
                    }
                }
            }
        }

        if (g_settings.nowPlayingEnabled && g_dwriteTextFormat && g_nowPlayingBrush) {
            ULONGLONG changedAt = g_nowPlayingChangedTick.load(std::memory_order_relaxed);
            ULONGLONG npElapsed = GetTickCount64() - changedAt;
            ULONGLONG showMs = (ULONGLONG)std::max(0, g_settings.nowPlayingDisplaySeconds) * 1000ULL;
            constexpr ULONGLONG kNpFadeMs = 800;
            float npAlpha = 0.f;
            if (changedAt != 0 && npElapsed < showMs + kNpFadeMs) {
                npAlpha = (npElapsed < showMs) ? 1.0f
                                                : 1.0f - (float)(npElapsed - showMs) / (float)kNpFadeMs;
            }
            if (npAlpha > 0.01f) {
                std::wstring displayText;
                {
                    std::lock_guard<std::mutex> lock(g_nowPlayingMutex);
                    displayText = g_nowPlayingDisplay;
                }
                if (!displayText.empty()) {
                    g_nowPlayingBrush->SetColor(D2D1::ColorF(
                        g_settings.nowPlayingR / 255.0f, g_settings.nowPlayingG / 255.0f,
                        g_settings.nowPlayingB / 255.0f, (g_settings.nowPlayingA / 255.0f) * npAlpha));
                    float npMargin = 8.0f * g_dpiScale;
                    float npHeight = (float)std::max(6, g_settings.nowPlayingFontSize) * g_dpiScale * 1.6f;
                    D2D1_RECT_F npRect = D2D1::RectF(
                        blockX - layout.textSide, blockY - npHeight - npMargin,
                        blockX + totalWidth + layout.textSide, blockY - npMargin);
                    g_dc->DrawText(displayText.c_str(), (UINT32)displayText.length(),
                                   g_dwriteTextFormat.Get(), npRect, g_nowPlayingBrush.Get());
                }
            }
        }

        if (g_settings.peakFreqEnabled && g_dwriteTextFormat && g_nowPlayingBrush) {
            float hz = g_dominantFreqHz.load(std::memory_order_relaxed);
            if (hz > 0.f) {
                wchar_t freqText[32];
                if (hz >= 1000.f)
                    swprintf_s(freqText, L"%.1f kHz", hz / 1000.f);
                else
                    swprintf_s(freqText, L"%.0f Hz", hz);

                g_nowPlayingBrush->SetColor(D2D1::ColorF(
                    g_settings.nowPlayingR / 255.0f, g_settings.nowPlayingG / 255.0f,
                    g_settings.nowPlayingB / 255.0f, g_settings.nowPlayingA / 255.0f));
                float pfMargin = 4.0f * g_dpiScale;
                float pfHeight = (float)std::max(6, g_settings.nowPlayingFontSize) * g_dpiScale * 1.4f;
                float pfWidth  = std::min(120.0f * g_dpiScale, totalWidth + 2.0f * layout.textSide);

                // Horizontal placement. Left/Right hug the bar group's edges and
                // may sit slightly outside it, which the reserved side inset covers.
                float pfX;
                switch (g_settings.peakFreqAlignH) {
                    case VizTextAlignH::Left:
                        pfX = blockX - layout.textSide;
                        break;
                    case VizTextAlignH::Center:
                        pfX = blockX + (totalWidth - pfWidth) * 0.5f;
                        break;
                    default:  // Right
                        pfX = blockX + totalWidth + layout.textSide - pfWidth;
                        break;
                }

                // Vertical placement. Above/Below sit fully clear of the bars,
                // using the room reserved for them in the layout.
                float pfY;
                switch (g_settings.peakFreqAlignV) {
                    case VizTextAlignV::Above:
                        pfY = blockY - pfHeight - pfMargin;
                        break;
                    case VizTextAlignV::Middle:
                        pfY = blockY + (totalHeight - pfHeight) * 0.5f;
                        break;
                    case VizTextAlignV::Bottom:
                        pfY = blockY + totalHeight - pfHeight - pfMargin;
                        break;
                    case VizTextAlignV::Below:
                        pfY = blockY + totalHeight + pfMargin;
                        break;
                    default:  // Top
                        pfY = blockY + pfMargin;
                        break;
                }

                D2D1_RECT_F pfRect = D2D1::RectF(pfX, pfY, pfX + pfWidth, pfY + pfHeight);
                size_t freqLen = wcslen(freqText);
                g_dc->DrawText(freqText, (UINT32)freqLen, g_dwriteTextFormat.Get(), pfRect,
                               g_nowPlayingBrush.Get());
            }
        }

        if (useFadeLayer) {
            g_dc->PopLayer();
        }
    }

    g_dc->EndDraw();
    // Sync interval 0: frame pacing is already handled by the waitable timer
    // in RenderThreadProc. Waiting on vblank here as well would park the
    // shell's UI thread inside Present for a slice of every frame.
    g_swapChain->Present(0, 0);
}

void RenderThreadProc() {
    ULONGLONG lastRenderTick = 0;
    ULONGLONG lastSuccessfulPostTick = 0;

    // A high-resolution waitable timer gives sub-millisecond wait precision,
    // unlike Sleep() which is quantized to the system timer tick (~15.6ms by
    // default). This keeps frame pacing smooth at the target FPS without
    // waking the thread on every vsync the way DwmFlush() did.
    HANDLE hTimer = CreateWaitableTimerExW(nullptr, nullptr,
        CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!hTimer) {
        hTimer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
    }

    auto preciseWait = [hTimer](DWORD ms) {
        if (hTimer) {
            LARGE_INTEGER due;
            due.QuadPart = -(LONGLONG)ms * 10000LL;  // 100ns units, negative = relative
            if (SetWaitableTimer(hTimer, &due, 0, nullptr, nullptr, FALSE)) {
                WaitForSingleObject(hTimer, INFINITE);
                return;
            }
        }
        Sleep(ms);
    };

    while (g_renderThreadRunning.load(std::memory_order_relaxed)) {
        HWND overlayWnd = g_overlayWnd.load(std::memory_order_relaxed);
        bool paused = g_fullscreenPaused.load(std::memory_order_relaxed);

        if (!overlayWnd || paused) {
            lastRenderTick = 0;
            preciseWait(150);
            continue;
        }

        int fps = std::max(1, g_settings.targetFps);
        UINT interval = 1000 / (UINT)fps;

        if (g_settings.pauseWhenSilentSeconds > 0) {
            ULONGLONG lastAudible = g_lastAudibleTickMs.load(std::memory_order_relaxed);
            ULONGLONG idleMs = GetTickCount64() - lastAudible;
            g_slowMode = idleMs > (ULONGLONG)g_settings.pauseWhenSilentSeconds * 1000ULL;
            if (g_slowMode) interval = 200;
        } else {
            g_slowMode = false;
        }

        ULONGLONG now = GetTickCount64();
        ULONGLONG elapsed = (lastRenderTick == 0) ? interval : (now - lastRenderTick);
        if (elapsed < interval) {
            preciseWait((DWORD)(interval - elapsed));
            continue;
        }
        lastRenderTick = now;

        if (!g_renderThreadRunning.load(std::memory_order_relaxed) || g_unloading.load())
            break;

        bool expected = false;
        if (g_renderTickPending.compare_exchange_strong(expected, true)) {
            PostMessage(overlayWnd, WM_APP_RENDER_TICK, 0, 0);
            lastSuccessfulPostTick = now;
        } else if (now - lastSuccessfulPostTick > 1000) {
            Wh_Log(L"Render tick flag was stuck, forced a reset");
            g_renderTickPending.store(false, std::memory_order_relaxed);
            lastSuccessfulPostTick = now;
        }
    }

    if (hTimer) CloseHandle(hTimer);
}

void StartRenderThread() {
    if (g_renderThread) return;
    g_renderThreadRunning.store(true, std::memory_order_relaxed);
    g_renderThread = new std::thread(RenderThreadProc);
    HANDLE hRenderThread = g_renderThread->native_handle();
    SetThreadDescription(hRenderThread, L"TourneTable-Render");
}

void StopRenderThread() {
    g_renderThreadRunning.store(false, std::memory_order_relaxed);
    if (g_renderThread) {
        if (g_renderThread->joinable()) g_renderThread->join();
        delete g_renderThread;
        g_renderThread = nullptr;
    }
    g_renderTickPending.store(false, std::memory_order_relaxed);
}

void PauseForFullscreen() {
    if (g_fullscreenPaused.exchange(true)) return;
    Wh_Log(L"Pausing visualizer: not visible");
    StopVizCaptureThread();
    if (g_dc && g_swapChain) {
        g_dc->BeginDraw();
        g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
        g_dc->EndDraw();
        g_swapChain->Present(1, 0);
    }
}

void ResumeFromFullscreen() {
    if (!g_fullscreenPaused.exchange(false)) return;
    Wh_Log(L"Resuming visualizer: visible again");
    StartVizCaptureThread();
    if (g_overlayWnd) {
        RenderVisualizer();
    }
}

void HandleDisplayChange() {
    if (!g_overlayWnd) return;

    HWND hWorkerW = GetParent(g_overlayWnd);
    if (!hWorkerW) return;

    RECT rc;
    GetWindowRect(hWorkerW, &rc);
    SetWindowPos(g_overlayWnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    g_cachedMonitor = monitor;
    float newDpiScale = GetMonitorDpiScale(monitor);
    if (newDpiScale != g_dpiScale) {
        ReleaseSwapChainResources();
        CreateSwapChainResources();
        RenderVisualizer();
    } else {
        // Same DPI, but the work area may have moved or resized, which shifts
        // where the widget's box belongs.
        UpdateSwapChainForLayout();
        RenderVisualizer();
    }

    if (g_messageWnd && g_settings.backgroundEnabled && g_settings.bgBlur > 0) {
        SetTimer(g_messageWnd, TIMER_ID_MSG_WALLPAPER_REFRESH, 500, nullptr);
    }
}

void CreateOverlayWindow();
void ApplySettingsChanged();

LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_APP_RENDER_TICK:
            g_renderTickPending.store(false, std::memory_order_relaxed);
            if (!g_unloading && !g_fullscreenPaused.load()) {
                RenderVisualizer();
            }
            return 0;

        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (!(wp->flags & SWP_NOSIZE) && !g_unloading) {
                // The swap chain is sized to the widget, not to this window, so
                // a window resize only matters insofar as it changes where the
                // widget's box lands.
                UpdateSwapChainForLayout();
                if (!g_fullscreenPaused.load()) RenderVisualizer();
            }
            break;
        }

        case WM_DESTROY:
            ReleaseSwapChainResources();
            g_overlayWnd = nullptr;
            if (!g_unloading && g_messageWnd) {
                SetTimer(g_messageWnd, TIMER_ID_MSG_RECREATE_OVERLAY, 200, nullptr);
            }
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;

        case WM_APP_SETTINGS_CHANGED:
            ApplySettingsChanged();
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DISPLAYCHANGE:
            if (!g_unloading) {
                SetTimer(hWnd, TIMER_ID_MSG_DISPLAY_CHANGE, 200, nullptr);
            }
            return 0;

        case WM_SETTINGCHANGE: {
            RefreshAccentColorCache();

            if (g_overlayWnd && !g_unloading && g_settings.backgroundEnabled &&
                g_settings.bgBlur > 0) {
                FILETIME ft = GetWallpaperFileTime();
                if (CompareFileTime(&ft, &g_lastWallpaperTime) != 0) {
                    SetTimer(hWnd, TIMER_ID_MSG_WALLPAPER_REFRESH, 2000, nullptr);
                }
            }
            return 0;
        }

        case WM_DWMCOLORIZATIONCOLORCHANGED:
            RefreshAccentColorCache();
            return 0;

        case WM_TIMER:
            if (g_unloading) return 0;
            if (wParam == TIMER_ID_MSG_DISPLAY_CHANGE) {
                KillTimer(hWnd, TIMER_ID_MSG_DISPLAY_CHANGE);
                HandleDisplayChange();
            } else if (wParam == TIMER_ID_MSG_RECREATE_OVERLAY) {
                KillTimer(hWnd, TIMER_ID_MSG_RECREATE_OVERLAY);
                CreateOverlayWindow();
            } else if (wParam == TIMER_ID_MSG_WALLPAPER_REFRESH) {
                KillTimer(hWnd, TIMER_ID_MSG_WALLPAPER_REFRESH);
                if (g_overlayWnd && g_settings.backgroundEnabled && g_settings.bgBlur > 0) {
                    ReleaseVisualResources();
                    RecreateVisualResources();
                    if (!g_fullscreenPaused.load()) RenderVisualizer();
                }
            } else if (wParam == TIMER_ID_MSG_FULLSCREEN_WATCH) {
                bool shouldPause = false;

                if (g_settings.pauseOnFullscreen && IsFullscreenOrGameActive())
                    shouldPause = true;

                // Occlusion is evaluated on this same 1s timer rather than per
                // frame -- EnumWindows is far too heavy to run at frame rate,
                // and a second of latency on hide/show is imperceptible.
                if (!shouldPause && g_settings.pauseWhenObscured && IsVisualizerOccluded())
                    shouldPause = true;

                if (shouldPause) {
                    PauseForFullscreen();
                } else {
                    ResumeFromFullscreen();
                }
            }
            return 0;

        case WM_DESTROY:
            g_messageWnd = nullptr;
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool g_overlayClassRegistered = false;

bool RegisterOverlayWindowClass() {
    if (g_overlayClassRegistered) return true;
    WNDCLASS wc = {};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = OVERLAY_WINDOW_CLASS;
    if (!RegisterClass(&wc)) return false;
    g_overlayClassRegistered = true;
    return true;
}

void UnregisterOverlayWindowClass() {
    if (g_overlayClassRegistered) {
        UnregisterClass(OVERLAY_WINDOW_CLASS, GetCurrentModuleHandle());
        g_overlayClassRegistered = false;
    }
}

bool EnsureLazyInitialized() {
    if (g_lazyInitialized.exchange(true)) return g_initSucceeded;

    if (!InitDirectX()) {
        Wh_Log(L"InitDirectX failed");
        return false;
    }

    if (!g_settings.pauseOnFullscreen || !IsFullscreenOrGameActive()) {
        StartVizCaptureThread();
    } else {
        Wh_Log(L"Fullscreen active at startup, starting paused");
        g_fullscreenPaused.store(true);
    }

    StartRenderThread();

    g_initSucceeded = true;
    return true;
}

void CreateOverlayWindow() {
    if (g_overlayWnd) return;
    if (!EnsureLazyInitialized()) return;

    HWND hWorkerW = GetWorkerW();
    if (!hWorkerW) {
        Wh_Log(L"Failed to find WorkerW");
        return;
    }

    if (!RegisterOverlayWindowClass()) return;

    HINSTANCE hInstance = GetCurrentModuleHandle();

    RECT rc;
    GetWindowRect(hWorkerW, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    g_overlayWnd = CreateWindowEx(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, OVERLAY_WINDOW_CLASS,
        nullptr, WS_CHILD | WS_VISIBLE, 0, 0, width, height, hWorkerW, nullptr, hInstance,
        nullptr);
    if (!g_overlayWnd) return;

    // A pending flag left set from a previous overlay window (e.g. one that just
    // got destroyed and is being recreated) would otherwise permanently block the
    // render thread from ever posting another tick -- nothing clears a flag whose
    // matching message was queued for a now-dead HWND.
    g_renderTickPending.store(false, std::memory_order_relaxed);

    if (!g_gsmtcStarted) {
        InitGsmtcListener();
        g_gsmtcStarted = true;
    }

    if (CreateSwapChainResources()) {
        if (!g_fullscreenPaused.load()) {
            RenderVisualizer();
        }
    }
}

bool g_messageClassRegistered = false;

bool RegisterMessageWindowClass() {
    if (g_messageClassRegistered) return true;
    WNDCLASS wc = {};
    wc.lpfnWndProc = MessageWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = MESSAGE_WINDOW_CLASS;
    if (!RegisterClass(&wc)) return false;
    g_messageClassRegistered = true;
    return true;
}

void UnregisterMessageWindowClass() {
    if (g_messageClassRegistered) {
        UnregisterClass(MESSAGE_WINDOW_CLASS, GetCurrentModuleHandle());
        g_messageClassRegistered = false;
    }
}

void CreateMessageWindow() {
    if (g_messageWnd) return;
    if (!RegisterMessageWindowClass()) return;

    HINSTANCE hInstance = GetCurrentModuleHandle();
    g_messageWnd = CreateWindowEx(0, MESSAGE_WINDOW_CLASS, nullptr, 0, 0, 0, 0, 0, nullptr,
                                  nullptr, hInstance, nullptr);
    if (g_messageWnd) {
        SetTimer(g_messageWnd, TIMER_ID_MSG_FULLSCREEN_WATCH, 1000, nullptr);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
                                         nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !IsFolderViewWnd(hWnd)) return hWnd;

    g_createOverlayTimer = SetTimer(nullptr, g_createOverlayTimer, 1000,
                                    [](HWND, UINT, UINT_PTR idEvent, DWORD) {
        KillTimer(nullptr, idEvent);
        g_createOverlayTimer = 0;
        CreateOverlayWindow();
        CreateMessageWindow();
    });

    return hWnd;
}

void LoadSettings() {
    PCWSTR shape = Wh_GetStringSetting(L"appearance.shape");
    g_settings.shape = (wcscmp(shape, L"mountain") == 0)     ? VizShape::Mountain
                       : (wcscmp(shape, L"mirror") == 0)     ? VizShape::Mirror
                       : (wcscmp(shape, L"wave") == 0)       ? VizShape::Wave
                       : (wcscmp(shape, L"breathe") == 0)    ? VizShape::Breathe
                       : (wcscmp(shape, L"dots") == 0)       ? VizShape::Dots
                       : (wcscmp(shape, L"radial") == 0)     ? VizShape::Radial
                       : (wcscmp(shape, L"oscilloscope") == 0) ? VizShape::Oscilloscope
                                                               : VizShape::Stereo;
    Wh_FreeStringSetting(shape);

    PCWSTR orientation = Wh_GetStringSetting(L"appearance.orientation");
    g_settings.orientation =
        (wcscmp(orientation, L"vertical") == 0) ? VizOrientation::Vertical : VizOrientation::Horizontal;
    Wh_FreeStringSetting(orientation);

    g_settings.barCount = std::clamp(Wh_GetIntSetting(L"appearance.barCount"), 1, VIZ_BARS_MAX);
    g_settings.barWidth = std::max(1, Wh_GetIntSetting(L"appearance.barWidth"));
    g_settings.barGap = std::max(0, Wh_GetIntSetting(L"appearance.barGap"));
    g_settings.barMaxSize = std::max(2, Wh_GetIntSetting(L"appearance.barMaxSize"));
    g_settings.barIdleSize = std::max(0, Wh_GetIntSetting(L"appearance.barIdleSize"));

    PCWSTR barCornerRadiusStr = Wh_GetStringSetting(L"appearance.barCornerRadius");
    {
        float v[4] = {3.0f, 3.0f, 3.0f, 3.0f};
        float parsed[4] = {};
        int n = swscanf_s(barCornerRadiusStr, L"%f %f %f %f", &parsed[0], &parsed[1], &parsed[2], &parsed[3]);
        // Documented contract is one value or four. Anything else (e.g. "5 5")
        // previously mixed parsed values with hardcoded defaults; treat it as
        // invalid and keep all four defaults instead.
        if (n == 1) {
            v[0] = v[1] = v[2] = v[3] = parsed[0];
        } else if (n == 4) {
            for (int k = 0; k < 4; k++) v[k] = parsed[k];
        }
        for (int k = 0; k < 4; k++) v[k] = std::max(0.0f, v[k]);
        g_settings.barRadiusTL = v[0];
        g_settings.barRadiusTR = v[1];
        g_settings.barRadiusBR = v[2];
        g_settings.barRadiusBL = v[3];
    }
    Wh_FreeStringSetting(barCornerRadiusStr);

    PCWSTR colorMode = Wh_GetStringSetting(L"appearance.colorMode");
    g_settings.colorMode = (wcscmp(colorMode, L"gradient") == 0)          ? VizColorMode::Gradient
                           : (wcscmp(colorMode, L"reactive_gradient") == 0) ? VizColorMode::ReactiveGradient
                           : (wcscmp(colorMode, L"accent") == 0)            ? VizColorMode::Accent
                           : (wcscmp(colorMode, L"album_art") == 0)         ? VizColorMode::AlbumArt
                           : (wcscmp(colorMode, L"dynamic_album") == 0)     ? VizColorMode::DynamicAlbum
                           : (wcscmp(colorMode, L"acrylic") == 0)           ? VizColorMode::Acrylic
                           : (wcscmp(colorMode, L"rainbow") == 0)           ? VizColorMode::RainbowCycle
                           : (wcscmp(colorMode, L"tourne") == 0)            ? VizColorMode::Tourne
                                                                             : VizColorMode::Solid;
    Wh_FreeStringSetting(colorMode);

    PCWSTR color = Wh_GetStringSetting(L"appearance.color");
    if (!ParseColorHex(color, &g_settings.colorA, &g_settings.colorR, &g_settings.colorG,
                       &g_settings.colorB)) {
        g_settings.colorA = g_settings.colorR = g_settings.colorG = g_settings.colorB = 255;
    }
    Wh_FreeStringSetting(color);

    PCWSTR grad1 = Wh_GetStringSetting(L"appearance.gradientColor1");
    if (!ParseColorHex(grad1, &g_settings.grad1A, &g_settings.grad1R, &g_settings.grad1G,
                       &g_settings.grad1B)) {
        g_settings.grad1A = 255; g_settings.grad1R = 30; g_settings.grad1G = 215; g_settings.grad1B = 96;
    }
    Wh_FreeStringSetting(grad1);

    PCWSTR grad2 = Wh_GetStringSetting(L"appearance.gradientColor2");
    if (!ParseColorHex(grad2, &g_settings.grad2A, &g_settings.grad2R, &g_settings.grad2G,
                       &g_settings.grad2B)) {
        g_settings.grad2A = 255; g_settings.grad2R = 0; g_settings.grad2G = 180; g_settings.grad2B = 255;
    }
    Wh_FreeStringSetting(grad2);

    g_settings.sensitivity = std::clamp(Wh_GetIntSetting(L"appearance.sensitivity"), 0, 300);

    PCWSTR eq = Wh_GetStringSetting(L"appearance.eqPreset");
    g_settings.eq = (wcscmp(eq, L"bass") == 0)       ? VizEQ::Bass
                   : (wcscmp(eq, L"rock") == 0)       ? VizEQ::Rock
                   : (wcscmp(eq, L"pop") == 0)        ? VizEQ::Pop
                   : (wcscmp(eq, L"jazz") == 0)       ? VizEQ::Jazz
                   : (wcscmp(eq, L"electronic") == 0) ? VizEQ::Electronic
                                                       : VizEQ::Default;
    Wh_FreeStringSetting(eq);

    g_settings.horizontalPosition = std::clamp(Wh_GetIntSetting(L"position.horizontalPosition"), 0, 100);
    g_settings.verticalPosition = std::clamp(Wh_GetIntSetting(L"position.verticalPosition"), 0, 100);
    g_settings.monitor = std::max(1, Wh_GetIntSetting(L"position.monitor"));

    PCWSTR verticalAnchor = Wh_GetStringSetting(L"appearance.verticalAnchor");
    g_settings.verticalAnchor = (wcscmp(verticalAnchor, L"top")    == 0) ? VizAnchor::Top
                              : (wcscmp(verticalAnchor, L"middle") == 0) ? VizAnchor::Middle
                                                                         : VizAnchor::Bottom;
    Wh_FreeStringSetting(verticalAnchor);

    g_settings.backgroundEnabled = Wh_GetIntSetting(L"background.enabled") != 0;

    PCWSTR bgColor = Wh_GetStringSetting(L"background.color");
    if (!ParseColorHex(bgColor, &g_settings.bgA, &g_settings.bgR, &g_settings.bgG, &g_settings.bgB)) {
        g_settings.bgA = 0x60; g_settings.bgR = g_settings.bgG = g_settings.bgB = 0;
    }
    Wh_FreeStringSetting(bgColor);

    g_settings.bgPadding = std::max(0, Wh_GetIntSetting(L"background.padding"));

    PCWSTR bgCornerRadiusStr = Wh_GetStringSetting(L"background.cornerRadius");
    {
        float v[4] = {14.0f, 14.0f, 14.0f, 14.0f};
        float parsed[4] = {};
        int n = swscanf_s(bgCornerRadiusStr, L"%f %f %f %f", &parsed[0], &parsed[1], &parsed[2], &parsed[3]);
        // Documented contract is one value or four. Anything else (e.g. "5 5")
        // previously mixed parsed values with hardcoded defaults; treat it as
        // invalid and keep all four defaults instead.
        if (n == 1) {
            v[0] = v[1] = v[2] = v[3] = parsed[0];
        } else if (n == 4) {
            for (int k = 0; k < 4; k++) v[k] = parsed[k];
        }
        for (int k = 0; k < 4; k++) v[k] = std::max(0.0f, v[k]);
        g_settings.bgRadiusTL = v[0];
        g_settings.bgRadiusTR = v[1];
        g_settings.bgRadiusBR = v[2];
        g_settings.bgRadiusBL = v[3];
    }
    Wh_FreeStringSetting(bgCornerRadiusStr);
    g_settings.bgBlur = std::max(0, Wh_GetIntSetting(L"background.blur"));
    g_settings.bgBorderSize = std::max(0, Wh_GetIntSetting(L"background.borderSize"));

    PCWSTR borderColor = Wh_GetStringSetting(L"background.borderColor");
    if (!ParseColorHex(borderColor, &g_settings.borderA, &g_settings.borderR, &g_settings.borderG,
                       &g_settings.borderB)) {
        g_settings.borderA = 0x40; g_settings.borderR = g_settings.borderG = g_settings.borderB = 255;
    }
    Wh_FreeStringSetting(borderColor);

    g_settings.targetFps = std::max(1, Wh_GetIntSetting(L"performance.targetFps"));
    g_settings.pauseOnFullscreen = Wh_GetIntSetting(L"performance.pauseOnFullscreen") != 0;
    g_settings.pauseWhenSilentSeconds = std::max(0, Wh_GetIntSetting(L"performance.pauseWhenSilentSeconds"));

    g_settings.peakHoldEnabled = Wh_GetIntSetting(L"appearance.peakHoldEnabled") != 0;
    g_settings.beatFlashEnabled = Wh_GetIntSetting(L"appearance.beatFlashEnabled") != 0;
    g_settings.beatFlashIntensity = std::clamp(Wh_GetIntSetting(L"appearance.beatFlashIntensity"), 0, 300);
    g_settings.rainbowSpeed = std::clamp(Wh_GetIntSetting(L"appearance.rainbowSpeed"), 1, 300);

    g_settings.nowPlayingEnabled = Wh_GetIntSetting(L"appearance.nowPlayingEnabled") != 0;
    PCWSTR nowPlayingColor = Wh_GetStringSetting(L"appearance.nowPlayingColor");
    if (!ParseColorHex(nowPlayingColor, &g_settings.nowPlayingA, &g_settings.nowPlayingR,
                       &g_settings.nowPlayingG, &g_settings.nowPlayingB)) {
        g_settings.nowPlayingA = g_settings.nowPlayingR = g_settings.nowPlayingG =
            g_settings.nowPlayingB = 255;
    }
    Wh_FreeStringSetting(nowPlayingColor);
    PCWSTR nowPlayingFont = Wh_GetStringSetting(L"appearance.nowPlayingFont");
    g_settings.nowPlayingFont = *nowPlayingFont ? nowPlayingFont : L"Segoe UI";
    Wh_FreeStringSetting(nowPlayingFont);
    g_settings.nowPlayingFontSize = std::max(6, Wh_GetIntSetting(L"appearance.nowPlayingFontSize"));
    g_settings.nowPlayingDisplaySeconds =
        std::max(0, Wh_GetIntSetting(L"appearance.nowPlayingDisplaySeconds"));

    g_settings.autoHideEnabled = Wh_GetIntSetting(L"performance.autoHideEnabled") != 0;
    g_settings.autoHideDelaySeconds = std::max(0, Wh_GetIntSetting(L"performance.autoHideDelaySeconds"));
    g_settings.pauseWhenObscured = Wh_GetIntSetting(L"performance.pauseWhenObscured") != 0;

    PCWSTR fftSizeStr = Wh_GetStringSetting(L"appearance.fftSize");
    int fftSize = _wtoi(fftSizeStr);
    g_settings.fftSize = (fftSize == 1024 || fftSize == 2048 || fftSize == 4096 || fftSize == 8192)
                              ? fftSize : 1024;
    Wh_FreeStringSetting(fftSizeStr);

    PCWSTR freqScale = Wh_GetStringSetting(L"appearance.freqScale");
    g_settings.freqScale = (wcscmp(freqScale, L"linear") == 0) ? VizFreqScale::Linear
                          : (wcscmp(freqScale, L"mel") == 0)   ? VizFreqScale::Mel
                                                                : VizFreqScale::Log;
    Wh_FreeStringSetting(freqScale);

    g_settings.peakFreqEnabled = Wh_GetIntSetting(L"appearance.peakFreqEnabled") != 0;

    PCWSTR pfH = Wh_GetStringSetting(L"appearance.peakFreqAlignH");
    g_settings.peakFreqAlignH = (wcscmp(pfH, L"left") == 0)   ? VizTextAlignH::Left
                              : (wcscmp(pfH, L"center") == 0) ? VizTextAlignH::Center
                                                              : VizTextAlignH::Right;
    Wh_FreeStringSetting(pfH);

    PCWSTR pfV = Wh_GetStringSetting(L"appearance.peakFreqAlignV");
    g_settings.peakFreqAlignV = (wcscmp(pfV, L"above") == 0)  ? VizTextAlignV::Above
                              : (wcscmp(pfV, L"middle") == 0) ? VizTextAlignV::Middle
                              : (wcscmp(pfV, L"bottom") == 0) ? VizTextAlignV::Bottom
                              : (wcscmp(pfV, L"below") == 0)  ? VizTextAlignV::Below
                                                              : VizTextAlignV::Top;
    Wh_FreeStringSetting(pfV);
    g_settings.oscilloscopeMultibandEnabled =
        Wh_GetIntSetting(L"appearance.oscilloscopeMultibandEnabled") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // Built here rather than on the capture thread: UpdateVisualizerTargets()
    // reads VIZ_SEEDS from the render thread, and if capture never starts (e.g.
    // pauseOnFullscreen with a fullscreen app already foreground at init) the
    // array would stay zeroed and every bar would breathe in lockstep.
    BuildVizSeeds();

    g_startTick = GetTickCount64();

    RefreshAccentColorCache();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hWorkerW = GetWorkerW();
    if (hWorkerW) {
        RunFromWindowThread(
            hWorkerW,
            [](void*) {
                CreateOverlayWindow();
                CreateMessageWindow();
            },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    // Kill the pending overlay-creation timer before anything else. It must be
    // killed from the thread that set it, which is the desktop/shell thread.
    if (HWND hProgman = FindWindow(L"Progman", nullptr)) {
        RunFromWindowThread(hProgman, [](void*) {
            if (g_createOverlayTimer) {
                KillTimer(nullptr, g_createOverlayTimer);
                g_createOverlayTimer = 0;
            }
        }, nullptr);
    }

    StopRenderThread();

    if (g_overlayWnd) SendMessage(g_overlayWnd, WM_APP_CLEANUP, 0, 0);
    if (g_messageWnd) SendMessage(g_messageWnd, WM_APP_CLEANUP, 0, 0);

    UnregisterOverlayWindowClass();
    UnregisterMessageWindowClass();

    StopVizCaptureThread();
    UninitDirectX();

    if (g_gsmtcStopEvent) {
        SetEvent(g_gsmtcStopEvent);
    }
    if (g_gsmtcThread && g_gsmtcThread->joinable()) {
        g_gsmtcThread->join();
    }
    g_gsmtcThread.reset();
    if (g_gsmtcStopEvent) {
        CloseHandle(g_gsmtcStopEvent);
        g_gsmtcStopEvent = nullptr;
    }

    // Joined unconditionally. Windhawk FreeLibrarys the mod the moment this
    // returns, so a detached thread whose code and return address live in the
    // mod image would crash the host. g_unloading (checked between the worker's
    // awaits) is what keeps this bounded instead of a timeout-and-walk-away.
    {
        std::lock_guard<std::mutex> ownerLock(g_albumArtThreadMutex);
        if (g_albumArtThread) {
            if (g_albumArtThread->joinable()) {
                g_albumArtThread->join();
            }
            delete g_albumArtThread;
            g_albumArtThread = nullptr;
        }
    }
    g_gsmtcStarted = false;
}

void ApplySettingsChanged() {
    Wh_Log(L">");

    int oldMonitor = g_settings.monitor;
    VizColorMode oldColorMode = g_settings.colorMode;

    LoadSettings();

    if ((g_settings.colorMode == VizColorMode::AlbumArt ||
         g_settings.colorMode == VizColorMode::DynamicAlbum) &&
        ((oldColorMode != VizColorMode::AlbumArt &&
          oldColorMode != VizColorMode::DynamicAlbum) || !g_albumArtColorReady.load()))
        FetchAlbumArtColorAsync();
    else if (g_settings.nowPlayingEnabled)
        FetchAlbumArtColorAsync();

    if (!g_lazyInitialized || !g_initSucceeded) return;

    if (!g_fullscreenPaused.load()) {
        if (g_settings.pauseOnFullscreen && IsFullscreenOrGameActive()) {
            PauseForFullscreen();
        }
    } else if (!g_settings.pauseOnFullscreen) {
        ResumeFromFullscreen();
    }

    if (!g_overlayWnd) return;

    UpdateSwapChainForLayout();

    ReleaseVisualResources();
    RecreateVisualResources();

    if (oldMonitor != g_settings.monitor) HandleDisplayChange();

    if (!g_fullscreenPaused.load()) {
        RenderVisualizer();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_overlayWnd) {
        SendMessage(g_overlayWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else {
        ApplySettingsChanged();
    }
}

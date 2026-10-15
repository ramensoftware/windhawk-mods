// ==WindhawkMod==
// @id                  tourne-table-desktop-audio-visualizer
// @name                Tourne'Table [Audio Visualizer]
// @description         A real-time audio visualizer for the Windows desktop: every bar its own frequency band, a Direct3D 11 renderer that idles to nothing, live right-click settings, My Styles, a Media Card and shareable setups.
// @description:ru-RU   Аудиовизуализатор реального времени для рабочего стола Windows. Расширенные настройки без ущерба для экономии ресурсов. Практически безинтерфейсный (near-headless) поток рендеринга с оптимизацией процессора для захвата звука.
// @version             2.1.0
// @author              USER-TOURNE
// @github              https://github.com/USER-TOURNE
// @donateUrl           https://ko-fi.com/tourne
// @license             MIT
// @include             windhawk.exe
// @compilerOptions     -lcabinet -ldxgi -ld2d1 -ld3d11 -ldcomp -ldwmapi -ldwrite -lgdi32 -lshcore -lshlwapi -lole32 -lshell32 -lksuser -lwindowscodecs -lruntimeobject -lwindowsapp -luuid -luser32 -ladvapi32 -lcomctl32 -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Tourne'Table - a ghost in your desktop's shell](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/tourne-header.png)

# `Tourne'Table` **[Audio Visualizer]**

![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/11.gif)

*The Oscilloscope shape running live audio: bottom-left placement, blurred panel, single-pixel border.*

> **A real-time audio visualizer that lives on your Windows desktop.**
> Built on the foundation of Salyts' Desktop Audio Visualizer, rebuilt around performance.

Play music. Bars dance on your wallpaper. That's the whole idea.

It listens to **whatever your PC is already playing** (Spotify, YouTube, a game, a call) and draws it behind your desktop icons. No virtual audio cable, no drivers, nothing to configure. It just picks up your system audio.

It is built to be cheap to run. The render thread wakes only at the frame rate you ask for, the wallpaper blur is computed once instead of every frame, and the drawing surface is sized to the widget rather than the whole desktop, so at a 144 FPS target the visualizer costs about **a third of one CPU core** and **one degree** of CPU package temperature while it plays, and nothing at all while it doesn't.

## ABOUT THIS PROJECT

Built with an emphasis on lower resource consumption, more efficient rendering, better frame pacing, expanded visualization options, dynamic album-art integration, native Windows media info, deeper customization, and power-conscious idle behavior.

The goal is simple:

> **Make the desktop move with the music, without making the CPU move mountains to do it.**

## Runs as its own process

Tourne'Table doesn't live inside `explorer.exe`. It runs as its own dedicated process using Windhawk's tool-mod pattern, so you'll see it as its own entry in Task Manager, separate from the shell. Restarting `explorer.exe` doesn't kill it. It just reattaches behind the desktop icons once the shell is back. Requires a Windhawk build with tool-mod support (1.7.3+, including the 2.0 alpha line).

---

## ✧ NEW IN 2.0 AND 2.1

2.1 replaces 1.x. If you ran one of the 2.0 test builds from GitHub, turn it off: only one Tourne'Table should run. If you also use **Tourne Desktop** (my separate desktop app, not part of Windhawk), its taskbar counts as the taskbar for Place In = Taskbar, and it can show, hide or pin the panel through the window message `TourneTable.Panel`. The performance figures further down were measured on 1.x and haven't been re-measured on 2.x yet.

**Share your style.** Right-click > My Styles > Style Editor > Share: **Copy code** puts your setup on the clipboard as one line for any chat, **Save file** writes a `.ttstyle` file, and **Community** opens the setups people have shared in [the Tourne'Style repo](https://github.com/USER-TOURNE/Tourne-Style/tree/main/community) in your browser (bring one in with **Paste code** or **Open file**; the mod itself never goes online) or opens a ready GitHub issue to share yours. Your audio device, hotkeys, file paths and hardware never go in a setup. Browse them all on the [community page](https://github.com/USER-TOURNE/Tourne-Style/blob/main/community/README.md).

**Every bar is its own measurement now.** Up to 1.5 the analysis split the sound into 7 fixed bands and every bar interpolated between those 7 numbers, so Bar Count and FFT Size changed how many bars you saw, not how much they knew. The new **Precision** engine gives each bar its own frequency band with its own edges, measured straight from the spectrum:

- bass comes from long, finely resolved windows and treble from short, fast ones (three resolution tiers), so a 1/24-octave bar at 50 Hz is a real reading rather than a smear;
- band layouts on Log, Linear, Mel, Bark or ERB scales, the standard **IEC 61260** fractional-octave bands every RTA uses, or one bar per **musical note**;
- **A / C / Z weighting** (IEC 61672), a dB-per-octave **tilt**, RMS or peak detection, and a choice of FFT window;
- **ballistics on real time**, so bars move at the same speed at 60, 144 or 240 FPS, with presets for an analyzer, a VU meter and both broadcast PPMs, and gravity peak caps;
- **loudness** (ITU-R BS.1770-5 / EBU R128: momentary, short-term, integrated), 4x-oversampled **true peak**, PLR and stereo **correlation** in the readout, and a new **Goniometer** shape.

The 1.4 analysis is still there as **Analysis Engine = Classic**.

**You choose where the work runs.** **Workload** is GPU, CPU or Hybrid: Hybrid analyses on the CPU and draws on the GPU (the cheapest split almost everywhere), GPU does both on the graphics chip, CPU does both on the processor.

**A new renderer built to cost next to nothing.** **Renderer = Direct3D 11** draws everything from one tiny shader and a buffer of bar heights, **skips any frame where nothing moved**, keeps the text on its own surface that only redraws when the text changes, and tells Windows when the panel is opaque so it copies instead of blending. When audio stops, drawing stops, and after a few more seconds of silence the audio stream itself stops (**Deep Idle**). The 1.5 Direct2D path is still there for comparison.

**Target FPS 0** matches your display's refresh rate, whatever it is.

**Right-click for quick settings.** Shape, colours, analysis, readout, overlays and **which audio device it listens to**, any output, input or virtual device (VB-Audio, Voicemeeter, an Ableton return), all live, without opening Windhawk. Plus a **Terminal** shape (text characters: columns, a waterfall or text meters) and a **media widget** layout: title and artist inside the panel, pixel-sharp text, a track progress bar and media controls pinned to the panel.

**Seven new styles.** LED Meter, Line Spectrum, Polar Bloom, Spectrogram, VU Needles (real IEC VU ballistics), Stereo Field and Particles, all in the Shape list and the right-click menu, plus **Reflection**, a fading mirror under the bars.

**Glow and Bloom.** A soft halo around every bar, worked out in the same shader pass that draws it, and a lens-style bloom from a quarter-size blur that only runs on frames that change. Direct3D 11 renderer.

**A Media Card.** Media Controls > Layout = Card: album art with previous / play / next on hover, a seek bar, one-click output switching and a volume slider. The output button changes the **Windows default output** for every app (through the same undocumented Windows interface sound-switcher tools use), and the slider sets the **system master volume**; both act only when you click, drag or scroll them, and both stay as set after the mod is turned off, like any change made in Windows' own sound settings. Theme its background, border, corner radius, art size and accent (icon colour, custom, album art or your Windows accent).

**My Styles.** Right-click > My Styles > Style Editor: mix a base style, colours, bar sizes, reflection, glow and bloom while the visualizer previews it live, save it under a name, and pick it from the menu any time.

**Finer looks.** Outline and Shadow around every bar; bar modifiers Hollow, Dashed, Tilt, Afterimage (a slowly falling trail) and Mirror Gap; scale numbers on Spectrogram and VU Needles; album colours that ease in; any Unicode character in Terminal. Reflection now works on both renderers.

**Exact sizes.** Bar width, gap, height, padding, borders, radii, fonts and offsets take decimals down to 0.01, and so do Sensitivity, Smoothing, Input Gain, Blur, Glow, Bloom, Shadow, Reflection, Afterimage, Beat Flash Intensity, Rainbow Speed and the dock, split and snap distances (turn Pixel Snap off to keep the fractions). The Style Editor's sliders step by 0.01 with the arrow keys.

**Placement.** Drag snaps to the screen's edges and centre, the taskbar, the tray and the Start button (hold Shift to place freely). Position > Place In = Taskbar puts it inside the taskbar. Position > Dock To App keeps the visualizer beside an app's window as it moves, and Split Into Two Pieces cuts it in two, e.g. either side of the taskbar's centred icons, at almost no cost.

**Media.** Click to Seek on the progress bar (off by default, with an app list), custom icons on the Media Card, and Media Controls > Stacking (always on top, or a normal window apps can cover).

---

## ◈ PERFORMANCE AT A GLANCE

Measured on an **Intel Core Ultra 265KF** (8 P-cores plus 12 E-cores), running a 120-bar oscilloscope at a 144 FPS target with background blur on, and with media controls, the peak-frequency readout, peak hold and beat flash all enabled.

Every figure below is the **change against an idle baseline**, captured back to back in the same session with the same music playing in both, so what you are reading is the cost of the mod rather than whatever else the machine happened to be doing.

| Metric | Cost of running it |
|:--|--:|
| Total CPU usage | **+1.6 percentage points** *(about 0.3 of one core)* |
| Peak single-thread | **+10.1 points** |
| CPU package power | **+7.2 W** |
| CPU package temperature | **+1.0 °C** |

Medians across 543 samples with it running and 384 without, at 1.25 s intervals. Medians rather than averages because both captures contained brief unrelated background spikes, and a median is not moved by them.

When audio stops, rendering stops, not "slows down," *stops*.

Full methodology, raw traces and caveats live in [the bench notes](https://github.com/USER-TOURNE/Tourne-Style/tree/main/windhawk-mods/tourne-table/bench) rather than on this page.

![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/10.gif)

*Bottom-left placement against a dark wallpaper, with the Now Playing label sitting above the panel.*

---

## ◆ VISUAL STYLES

### 9 Shapes

| Shape | What it does |
|:--|:--|
| **Stereo** | Classic equalizer bars. Low notes left, high notes right. |
| **Mountain** | Peaks in the middle, tapers toward both edges. |
| **Mirror** | The opposite, grows from the outside edges inward. |
| **Wave** | Normal bars with a slow ripple rolling through them. |
| **Breathe** | A gentle swell that rises with the music instead of jumping. |
| **Dots** | Stacked dots instead of solid bars, old LED-meter look. |
| **Radial** | Bars shoot outward from a center point, like a sunburst. |
| **Oscilloscope** | A single line tracing the actual sound wave. |
| **Goniometer** | The stereo field: every sample plotted with mono straight up and width sideways, with a correlation bar underneath. *(2.0)* |

![Oscilloscope closeup](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/8.gif)

*Closeup: the waveform drawn as one continuous line, with the Now Playing label and media buttons alongside.*

### 9 Color Modes

| Mode | What it does |
|:--|:--|
| **Solid** | One flat color. |
| **Gradient** | Fades between two colors across the bars. |
| **Reactive Gradient** | Shifts toward the second color as things get *louder*. |
| **Windows Accent** | Matches your Windows accent color, updating instantly. |
| **Album Art** | Pulls the dominant color from the playing track's cover. |
| **Dynamic Album** | Gradient between the cover art's two strongest colors. |
| **Acrylic** | Grows more opaque the louder it gets, invisible in silence. |
| **Rainbow Cycle** | Continuously cycling hue, adjustable speed. |
| **Tourne** | Built-in teal → red gradient from my personal palette. |

### Plus

- **2 orientations**: bars grow vertically or horizontally
- **3 anchors**: grow from the Bottom, the Top, or **both directions from the Middle**
- **Peak Hold Caps**: thin markers hang at each bar's recent peak and slowly fall *(classic hardware EQ)*
- **Beat Flash**: bars brighten on detected bass hits, on top of any color mode
- **Multiband Oscilloscope**: the waveform tints toward whichever part of the spectrum is loudest

---

## ♪ NOW PLAYING DISPLAY

Shows the current **artist and title** above the visualizer, pulled from the same Windows media session that powers your volume popup. Works with Spotify, browsers, and most media players.

Fades in on track change, fades out after a configurable delay. Custom color, font family and size.

---

![Oscilloscope closeup](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/7.gif)

*A narrower panel. The trace scales to whatever width the bar settings give it.*

## ◐ THE PALETTE

This project follows my personal theming palette: reflected in the repo screenshots. It can be changed to any hex / RGB / RGBA value you want.

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

## ☰ RIGHT-CLICK MENU *(2.0)*

Right-click the visualizer for quick settings, applied the moment you pick them:

- **Audio Source**: the default output (what you hear), the default input, or **any active device by name**, outputs and inputs, virtual ones included. VB-Audio Cable and Matrix, Voicemeeter buses, an Ableton return routed to its own device: if Windows lists it, it's there. Outputs are captured by loopback, inputs recorded directly.
- **Shape**, **Terminal Style**, **Color Mode**, **Analysis Engine**, **Band Layout**, **Weighting**, **Ballistics**, **Readout**, **Workload**, **Renderer**, **Target FPS**.
- Toggles for Peak Hold Caps, Beat Flash, Now Playing, the Track Progress Bar, Media Controls, Pixel-Sharp Text, **Pixel Snap** and **Subpixel Nudges**.
- **Pause Visualizer**, which blanks it and stops the audio stream until you right-click the same spot again (or the media strip) and untick it.
- **Copy Quick Settings** and **Reset Quick Settings**.

Windhawk lets a mod read its settings but not write them, so a menu choice is kept as the mod's own saved value and laid over the settings page. The page keeps showing what's underneath, which is why the menu says how many quick settings are active. **Copy Quick Settings** puts them on the clipboard as `name = value` lines, worded as the menu shows them (for example `Shape = Terminal`, `Peak Hold Caps = On`), so you can make them permanent on the settings page. **Reset Quick Settings** then hands everything back to the page.

The menu only opens where the visualizer is actually showing on the desktop, never through a window that covers it, and a right-click there doesn't open the desktop's own menu. If you'd rather keep a plain right-click for the desktop, set **Right-Click Menu** (Interaction) to **Ctrl + Right-Click**. Right-clicking the media strip opens the same menu.

---

## ◫ SHARP AT EVERY SIZE, OR BETWEEN PIXELS *(2.0)*

**Why it could look soft.** Every size was scaled by your display scaling and every position was a percentage of the screen, both kept as exact fractions. At 125% a 2 px bar is 2.5 px wide, and a Position of 50% can put the visualizer at x = 812.37. A bar that starts or ends partway through a pixel gets an antialiased edge, so some sizes came out crisp and others blurry.

**Pixel Snap** *(Position, on by default)* rounds the position and every scaled size (bar width, gap, max size, panel padding) to whole pixels, so every bar edge, the panel and the text sit exactly on the pixel grid at any size and any scaling. Bar heights stay fractional, which keeps their motion smooth. The process also declares per-monitor DPI awareness on each of its own threads now, so Windows never stretches the picture on a monitor whose scaling differs from your main one.

**Subpixel placement** is Pixel Snap **off**. The picture can then sit between pixels, so it moves by fractions of a pixel, at the cost of slightly soft edges. Physically, a crisp edge and a position between two pixels can't both happen.

- **Keyboard Move Fine Step** *(Interaction)*: 1/2, 1/4, 1/8 or 1/16 px.
- Hold **Keyboard Move Fine Key** with the move modifier, or tick **Subpixel Nudges** in the right-click menu to make every nudge fine.
- Fine steps move the visualizer. The media strip and the text overlays still move whole pixels, since a window and a text box can't sit between them.
- With Pixel Snap on, fine nudges still add up: four quarter-pixel presses move the picture one pixel.

---

## ▣ THE TERMINAL SHAPE *(2.0)*

**Shape = Terminal** draws the visualizer as text: a grid of characters in a monospace font, in three styles.

- **Columns**: every bar a column of characters (`#` by default) that turns the **Hot Color** above **Hot Threshold**, with the peak cap as a character of its own. The cava / btop look.
- **Waterfall**: a spectrogram in characters. Each line is one moment and each column one band, with the character picked from **Waterfall Ramp** by level and the newest line on top.
- **Meters**: text meters, `Bass:    [######     48%]`, for bass, mid, treble and the loudest band.

It runs on the same per-bar levels as every other shape, so the engine, band layout, weighting and ballistics all apply. With the Direct3D 11 renderer the characters come from a glyph atlas baked once, and the whole grid is one instanced draw: a few hundred cells cost the same as a few hundred bars. Pick a pixel font and **Text Rendering = Pixel** for hard-edged characters.

---

## ◰ RECIPES

**A compact media widget** (coral bars on a dark teal panel, the title over the artist on the right, controls at the top left, a progress bar underneath):

| Setting | Value |
|:--|:--|
| Shape / Bar Count / Bar Width / Bar Gap | Stereo / `64` / `2` / `2` |
| Bar Max Size / Idle Size / Corner Radius | `36` / `2` / `0` |
| Color Mode / Color | Solid / `#FFFF8F8F` |
| Background Color / Padding / Corner Radius | `#FF121C21` / `12 12 52 12` / `4` |
| Background Border Size / Color | `1` / `#FF2A3A40` |
| Now Playing | on, Display Seconds `0`, Layout **Two lines**, Placement **Inside the panel, top**, Alignment **Right** |
| Now Playing Color / Artist Color | `#FFFF8F8F` / `#FF64AA89` |
| Font / Text Rendering | a pixel font (e.g. Press Start 2P at `8`) / **Pixel** |
| Track Progress | on, Below the panel, Height `2`, Gap `6`, Color `#FFFF8F8F`, Track `#FF64AA89` |
| Media Controls | on, Icon Size `14`, Spacing `8`, Icon Color `#FF64AA89`, Anchor **Panel, top left**, Insets `10` / `10` |

The two colours are the Red and Pale Green of the palette above, and the panel is its Dark Teal.

**A terminal readout** (green and red characters on black):

| Setting | Value |
|:--|:--|
| Shape | Terminal |
| Terminal Style | Columns, Waterfall or Meters |
| Font / Size | Consolas or Cascadia Mono / `12` |
| Background Color / Corner Radius | `#FF000000` / `0` |
| Peak Hold Caps | on (Columns) |

---

# ✦ EVERY SETTING, EXPLAINED

**You don't need any of this to use the mod.** The defaults work. This is for when you want to tweak something and you're wondering what a word means.

## Appearance

**Shape**: Which of the 8 styles above to draw.

**Orientation**: *Horizontal* = a row of bars growing up and down. *Vertical* = a column growing left and right.

**Bar Count**: How many bars. More = finer detail, wider visualizer. Range **1-2048** (enough to span a 4K or ultrawide screen).

**Bar Width**: How fat each bar is, in pixels.

**Bar Gap**: Space between bars, in pixels. Set to `0` and they touch.

**Bar Max Size**: How tall a bar gets at full volume. This is the overall height of the visualizer.

**Bar Idle Size**: How tall bars sit in silence. `0` makes them vanish completely; a few pixels leaves a thin resting line.

**Bar Corner Radius**: How rounded the bar corners are. One number rounds all four equally, or give four numbers separated by spaces for individual control: `top-left top-right bottom-right bottom-left`. Example: `5 5 0 0` rounds only the top.

**Color Mode**: Which of the 9 coloring styles above to use.

**Color**: The color used in Solid mode. Format is `#AARRGGBB` or `#RRGGBB`: that's **A**lpha (transparency), then **R**ed, **G**reen, **B**lue in hex. Lower the first two digits to make it see-through.

**Gradient Color 1 / 2**: Start and end colors for the gradient modes.

**Sensitivity**: How hard the bars react. Too low and quiet music barely moves them; too high and everything slams to max. Range 0-300. Turn it down for bass-heavy tracks, up for quiet recordings.

**Input Gain (dB)**: A fixed level trim on the captured audio, applied before everything else. Range -24 to +24, default 0.

> **Why this exists.** Loopback capture sees the mix *after* each app's own volume slider but *before* the Windows master slider. So if you keep Spotify at 40% and the system at 80%, the visualizer only ever sees that 40%, and turning the system up does not help it. Input Gain is the control that does. It is also the only setting that scales the **Oscilloscope** waveform directly, which otherwise has no level control of its own. Auto Gain scales the trace too when it is on, and the trace clips flat rather than running off the panel.
>
> **It moves the idle thresholds with it.** Pause When Silent and Auto-Hide When Idle both key off how far the bars are moving, so raising Input Gain lowers the level that counts as silence, and lowering it raises that level. Sensitivity has always behaved the same way for the same reason. If you push Input Gain a long way up, expect the mod to consider quieter things "playing".

**Auto Gain**: Adapts the level continuously so quiet sources still fill the bars, instead of retuning Sensitivity per app or per track. Off by default.

It only ever boosts, never cuts, so loud material behaves exactly as it does with this off. Through silence it holds its last value rather than winding up, which means the noise floor is never lifted and **idle shutdown still works normally**. The idle test reads the un-boosted level against the same threshold it always used, so turning Auto Gain on neither extends nor shortens how long the mod stays awake.

Two things it does affect, worth knowing before you go hunting for them:

- Because it normalizes the loudest band to a fixed target, **Sensitivity stops doing much for quiet material** while it is on. That is inherent to how a levelling gain works, not a bug.
- On the **Oscilloscope**, a lower Sensitivity produces a *larger* trace, since a smaller measured peak asks for a bigger boost.

**Auto Gain Max Boost (dB)**: The ceiling on that lift. Range 0-24, default 12. Lower it if quiet passages are being flattened more than you want, raise it if a very quiet source still will not fill the bars.

**EQ Preset**: Which frequencies get emphasized *visually*. Doesn't touch your actual audio.
`Default` no adjustment · `Bass` boosts lows · `Rock` boosts mids and highs · `Pop` heavy on highs · `Jazz` warmer, gentler highs · `Electronic` boosts bass and treble, scoops the middle.

**FFT Size**: How finely sound gets analyzed. An FFT is the math that splits audio into separate frequencies: think of it as sorting sound into buckets by pitch. More buckets = finer detail, slightly more CPU.
`1024` fastest, plenty for most · `2048` / `4096` noticeably crisper · `8192` maximum detail.

> **With the Precision engine (2.0)** this is the size of each of the three resolution tiers, and bass detail comes from the tiers rather than from raising this. `2048` *(the new default)* is the sweet spot. Sensitivity no longer has to be retuned per FFT Size either: levels are calibrated in dBFS, so a change of FFT Size changes resolution, not height.

> **The Oscilloscope no longer cares what this is set to.** It used to take its trace over a window of exactly one FFT block, so raising FFT Size stretched the trace to cover more time and updated it less often, which is why `8192` used to look so rough on that shape. The trace now runs on its own time base, set by **Oscilloscope Time Window** below. Pick FFT Size for the bars and leave the scope out of it.
>
> **One more note:** the higher the FFT Size, the more accurate the Sensitivity slider becomes for your specific audio setup. The correlation generally runs: **as FFT Size goes up, your Sensitivity will need to go up too.** For now that means fine-tuning Sensitivity per EQ Preset *and* per FFT Size, for your particular placement, size and personal adjustments.

**Frequency Scale**: How the frequency range spreads across the bars (with Band Layout = Frequency Scale). This matters more than it sounds.
- **Log**: the natural-feeling default. Gives bass and treble roughly equal visual space, matching how humans hear pitch.
- **Linear**: spreads by raw Hz. Since most musical energy lives low, this crams all the action into a sliver on the left and leaves the right mostly dead. Technically accurate, visually dull.
- **Mel**: uses the *mel scale*, built from research on how people actually perceive pitch. Like Log, tuned to human hearing.
- **Bark**: the ear's critical bands (Traunmueller's formula): more room for the midrange, where hearing is most finely tuned.
- **ERB**: equivalent rectangular bandwidths (Glasberg and Moore), the widths of the ear's own auditory filters. Between Log and Bark.

**Peak Frequency Readout**: Shows the loudest note as a live number, e.g. `1.2 kHz`, or loudness figures instead (see **Readout Content** under Analysis).

**Peak Readout Position**: Where that number sits. Horizontal: Left / Center / Right. Vertical: Above / Top / Middle / Bottom / Below.
> **Above** and **Below** place it fully *outside* the bars so it never overlaps the visualization.

**Multiband Oscilloscope Coloring**: Only affects the Oscilloscope shape. The line tints toward whatever part of the spectrum is loudest: warm for bass, green for mids, blue for treble. Overrides Color Mode for that shape.

**Oscilloscope Time Window (ms)**: How much audio the trace shows end to end. Range 5-250, default `24`.

> This is the scope's time base, and it is the setting that decides what the trace *looks* like. Small values stretch out individual wave cycles, so a bass note reads as a few big smooth curves. Large values pack more of the signal into the same width, so it reads as a rolling envelope. Around `20-30` is the hardware-scope look; **`150-180` is close to what FFT Size `8192` used to give**.
>
> It is deliberately independent of **FFT Size**. The trace is sampled on its own clock at roughly 100 updates a second whatever the FFT is doing, peak-sampled rather than point-sampled so it neither aliases nor loses amplitude as the window widens, and aligned to a rising zero crossing so the waveform holds still instead of sliding sideways.

**Oscilloscope Damping**: Slows how fast the trace changes *shape*, without slowing how often it is *drawn*. Range 0-100, default `0`.

> These are two different things and the distinction is the whole point. The trace is still redrawn every frame at your Target FPS, so nothing gets choppier. What damping changes is how far each point is allowed to move per frame, so the waveform evolves slowly enough to follow with your eyes instead of being a new picture every frame.
>
> This is what makes a wide **Time Window** legible rather than busy. On its own a 170 ms window is a lot of information changing fast; damped, it becomes a shape you can actually track. Try `150` window with `60-80` damping.
>
> It costs nothing. The easing is a single multiply per point on the render thread, and it does not touch the capture path or the frame rate.

**Anchor**: Which edge bars grow from. `Bottom` rise upward *(classic)* · `Top` hang downward · `Middle` grow **both directions** from a center line.

**Peak Hold Caps**: Leaves a thin marker floating at each bar's recent peak, which slowly drifts down.

**Beat Flash** + **Intensity**: Flashes bars brighter on bass hits. Intensity controls how hard.

**Rainbow Cycle Speed**: How fast the rainbow rotates. Rainbow mode only.

**Now Playing Text**: Toggles the artist/title display.

**Now Playing Color / Font / Font Size**: Styling for that text. The font must be **installed on your system**: type the exact family name. Windows silently falls back to a default on a typo rather than erroring, so double-check spelling if nothing changes.
> The Peak Frequency Readout shares these same font settings.

**Now Playing Display Seconds**: How long the text stays up after a track change before fading.

## Analysis (Precision Engine)

Everything here applies to **Analysis Engine = Precision**, the default. With **Classic** the bars come from the 1.4 analysis and these settings are ignored.

**Analysis Engine**: **Precision** gives every bar its own frequency band, measured from the spectrum. **Classic** is the 1.4 analysis: 7 fixed bands the bars interpolate between. Kept for comparison, and because some people will prefer its looser look.

**Band Layout**: How the spectrum is cut into bars.
- **Frequency Scale** *(default)*: **Bar Count** bars spread from Min to Max Frequency on the **Frequency Scale** set under Appearance.
- **IEC 61260**: the standard fractional-octave bands (base-10, centred on 1 kHz) that every real-time analyzer uses. **Octave Fraction** picks the width.
- **Musical**: one bar per note of equal temperament (or per quarter tone), tuned to **Tuning (A4)**.

IEC and Musical decide how many bars there are, so **Bar Count** is ignored for them: 1/3 octave from 20 Hz to 20 kHz is 31 bars, 1/6 is 60, 1/12 is 120 and 1/24 is 240.

**Octave Fraction**: 1/1, 1/3, 1/6, 1/12 or 1/24 octave. 1/3 is the classic RTA; 1/12 is a semitone. Musical only uses 1/12 and 1/24.

**Min / Max Frequency**: The analysed range. Max is capped at 21 kHz, below the Nyquist limit of every common output device.

**Tuning (A4)**: Reference pitch for Musical, 400 to 480 Hz.

**Weighting**: **Z** is flat. **A** follows the ear at quiet listening levels and pulls the bass right down; **C** is the gentler loud-level curve. Both from IEC 61672-1, added per band, free.

**Tilt (dB per octave)**: A slope around 1 kHz.

> **What 0 means here.** Bars are measured as band power and, by default, referenced to a 1/3-octave band (see Level Reference), so **pink noise already reads flat at 0**, the way it does on a hardware RTA. Most music falls a little faster than pink. **1.5** *(default)* makes a typical modern mix read level, the same thing SPAN's 4.5 dB/oct default does on a plain FFT display (an FFT display needs 3 dB/oct just to flatten pink noise; this one doesn't).

**Detector**: **RMS** sums the power of every frequency in the band, the right reading for music and noise. **Peak** shows the band's loudest single frequency, closer to what a tone reads on a meter.

**Level Reference**: **Third-octave** *(default)* scales every band to the power a 1/3-octave band at the same centre would hold. Changing Bar Count or Band Layout then changes how fine the bars are, not how tall. **Band** shows each band's true power, so wide bars read higher than narrow ones.

**FFT Window**: **Hann** is the general-purpose choice. **Blackman-Harris** has 92 dB of dynamic range, so quiet detail next to a loud tone stays visible. **Flat-top** reads a tone's exact level wherever it falls between bins, at the cost of resolution. **Hamming** has a narrower peak and more leakage.

**Bass Detail**: The resolution tiers. The audio is analysed as captured, decimated by 4, and decimated by 16, each with the same FFT size, and each bar is measured from the fastest tier that can resolve it. **High** *(default)* uses all three, which is what makes fine bass bars real; **Standard** stops at 4; **Off** is a single FFT.

> **Why tiers instead of a bigger FFT.** Bin spacing is sample rate over FFT size, 23 Hz at 48 kHz and 2048. A 1/24-octave band at 50 Hz is 1.4 Hz wide, so one FFT would need to be enormous to resolve it, and an enormous FFT reacts slowly everywhere, treble included. Decimating first gives the bass a long window and the treble a short one. At FFT Size 2048 the three windows are 43, 171 and 683 ms long, and the two deeper tiers are only re-analysed when enough new audio has arrived to make it worthwhile.

**Channel**: Which signal the spectrum comes from. **Mix** averages all channels, as before. **Mid** is what both speakers share; **Side** is their difference, which shows exactly where a mix's stereo width lives.

**Display Floor / Ceiling (dBFS)**: The levels that draw as an empty and a full bar, before Sensitivity. A full-scale sine is 0 dBFS; loud modern music sits around -20 dBFS per third-octave band. Defaults -72 and -12.

> **Sensitivity** still works with Precision, as a gain: each step from the default 150 is 0.2 dB, so the slider covers -30 to +30 dB. **Sensitivity Curve** still shapes the top of the range, **Auto Gain** lifts quiet sources toward 85% of the range (boost only, held through silence), **EQ Preset** becomes a dB gain on the low, mid and high zones, and **Motion Smoothing** slows whichever Ballistics you pick, up to 6x.

**Ballistics**: How bars rise and fall. All of them run on elapsed time, so the motion is the same at any frame rate.
- **Snappy** *(default)*: closest to the 1.4 feel.
- **Smooth**: slower, easier to follow.
- **Analyzer**: falls a steady 20 dB per second, like a hardware RTA.
- **VU**: 300 ms integration.
- **PPM, EBU (Type II)** and **PPM, DIN (Type I)**: the IEC 60268-10 return rates (24 dB in 2.8 s, 20 dB in 1.5 s).
- **Custom**: **Custom Attack (ms)** and **Custom Release (dB/s)**.

**Peak Hold Time / Peak Fall**: With **Peak Hold Caps** on, how long a cap hangs, then whether it falls with **Gravity** (accelerating, the classic analyzer look) or at a steady **Linear** rate.

**Readout Content**: What the readout set up under **Peak Frequency Readout** shows.
- **Peak frequency**: as before.
- **Loudness**: **M**omentary (400 ms), **S**hort-term (3 s) and gated **I**ntegrated loudness in LUFS, per ITU-R BS.1770-5 and EBU R128.
- **Loudness, true peak, PLR, correlation**: adds 4x-oversampled true peak (dBTP), PLR (true peak minus integrated loudness) and stereo correlation (+1 mono, 0 wide, -1 out of phase).
- **Both**: peak frequency and loudness.

The loudness meters only run while they are on screen.

**Reset Integrated Loudness On Track Change**: Starts a new integrated measurement with each track, so the figure is per track.

> **What loopback can't see.** The mod hears what Windows mixes, after each app's own volume slider and before the master slider. Players in **WASAPI exclusive** or **ASIO** mode bypass that mix entirely, so the visualizer shows nothing while they play, and loudness figures describe the mix, not the file.

## Position

**Horizontal Position**: Left-to-right placement as a percentage. `0` hard left, `50` centered, `100` hard right.

**Vertical Position**: Top-to-bottom, same idea. `0` top, `100` bottom.

**Monitor**: Which screen to draw on. `1` is your first monitor.

## Background

**Enabled**: Draws a panel behind the bars. Turn off for bars floating directly on the wallpaper.

**Color**: Panel color in `#AARRGGBB`. The alpha controls transparency.

**Padding**: Breathing room between the bars and the panel edge.

**Corner Radius**: How rounded the panel corners are. Same one-or-four-value rules as bar radius.

**Blur**: Frosted-glass blur of your wallpaper behind the panel. `0` disables.
> This used to be the single most expensive setting in the mod. It's now computed once and cached, so it's essentially free per frame.

**Border Size / Border Color**: A thin outline around the panel. `0` for none.

## Performance

**Target FPS**: How many times per second it redraws. Higher = smoother, more CPU. Little point exceeding your monitor's refresh rate. **`0` matches the refresh rate**, whatever it is. With the Direct3D 11 renderer a frame where nothing moved is skipped entirely, so a high target costs far less than it used to in quiet or steady passages.

**Pause On Fullscreen**: Stops completely when a fullscreen app runs. Detects both true fullscreen *(games)* and borderless windows. Since the visualizer lives on the desktop it's invisible anyway. This just stops it burning power. Resumes automatically.

**Pause When Silent (seconds)**: After this long without audio, drops to a trickle instead of full speed: it wakes only when audio arrives or four times a second, and with Direct3D 11 presents nothing unless something changes. `0` disables (and Deep Idle with it).

**Deep Idle** *(2.0)*: Five seconds after Pause When Silent kicks in, stops the audio stream itself and reads Windows' own peak meter four times a second instead, restarting the moment anything plays. A running capture stream registers an audio power request, which can keep the PC from sleeping (`powercfg /requests` shows it); a stopped one doesn't. On by default.

**Auto-Hide When Idle** + **Delay**: Fades out entirely after prolonged silence. Once fully faded it **stops rendering completely**, not just invisible, genuinely doing nothing until audio returns.

**Pause When Covered**: Stops rendering *and* audio capture while fully hidden behind another window.
> **Off by default.** Reliably detecting "am I covered?" on Windows 11 is genuinely tricky: the shell is full of invisible windows that report themselves as visible. The check is deliberately conservative (only a fully-covering, real application window counts), but if the visualizer ever vanishes when it shouldn't, this is the switch to flip.

## Hardware

There are two jobs in this mod, and they can run on different silicon: **analysing** the audio (the FFTs that turn sound into frequency bands) and **drawing** the result.

**Workload**: Where the work runs.
- **Hybrid** *(default)*: analysis on the CPU, drawing on the GPU. Analysis is a fraction of a percent of one core, and the GPU draws for almost nothing, so this is the cheapest split on nearly every PC.
- **GPU**: the analysis runs on the graphics chip as well, in four small compute passes that feed the drawing directly, so the CPU only copies audio and nothing comes back each frame but 8 bytes of statistics. Needs the Precision engine and the Direct3D 11 renderer, and uses at most FFT Size 4096. Worth trying on a strong iGPU; measure it against Hybrid rather than assume.
- **CPU**: analysis and drawing both on the processor (drawing in software, through WARP). The GPU is left with nothing but Windows' own compositing. Fine for a small widget; a full-width strip at a high frame rate is real CPU work.

**Drawing Device**: Which GPU draws.
- **Auto** *(default)*: the GPU your chosen monitor is plugged into. On a laptop with two GPUs, or a desktop with the iGPU left on, drawing on the *other* one means every frame gets copied across to the GPU that scans it out, which is exactly the kind of uneven frame time this mod is trying to avoid. Auto asks Windows which GPU owns that screen and uses it, and follows if the answer changes.
- **Integrated GPU**: the power-saving GPU, the same one Windows picks for "Power saving" in Graphics settings. Falls back to Auto, with a warning, if there isn't one (F and KF processors have no iGPU, and desktops often switch it off in the BIOS).
- **Discrete GPU**: the high-performance GPU. Same fallback.
- **CPU**: software rendering, the same as Workload = CPU.

If the GPU is ever lost mid-session (a driver update, a driver reset, a laptop switching GPUs), the mod rebuilds itself on whatever is available instead of freezing until restart.

**Renderer**:
- **Direct3D 11** *(default)*: every bar, dot, cap, spoke and scope segment is one instanced quad from a tiny vertex shader that reads the bar heights straight from a buffer. The CPU writes two numbers per bar and issues a handful of draw calls per frame, whatever the bar count, and the pixel shader computes exact antialiased edges, so shapes look like Direct2D's. On top of that:
  - **Frames where nothing moved are skipped.** The picture is summarised in a hash (bar heights to a quarter pixel, colours, the trace) and compared with the last frame shown. If it matches, nothing is drawn and nothing is presented, so Windows has nothing to recompose.
  - **The text lives on its own surface**, redrawn only when the text or its fade changes. The animated surface covers just the panel, not the room reserved around it for text.
  - **The panel can be opaque** (see Opaque Panel).
- **Direct2D**: the 1.5 path, unchanged, including Smooth Mode's batching. Kept for comparison, and used automatically, with a warning, if Direct3D 11 can't start (a GPU below feature level 11.0, or `d3dcompiler_47.dll` missing).

**Opaque Panel**: When the background panel can't show anything behind it (**Blur** on, or a panel colour with full alpha) and **Auto-Hide** is off, the Direct3D 11 renderer creates the panel's surface with alpha ignored and lets the compositor cut the rounded corners. Windows then copies those pixels instead of blending them every frame; Chromium measured the same switch at 3.12 W against 2.28 W. **Off** always blends, for comparison.

**Smooth Mode**: Frame timing locked to the display, and motion on elapsed time. **Auto** *(default)* turns it on when drawing runs on an integrated GPU or the CPU; **On** works on any GPU and is worth trying on a discrete one too; **Off** is the 1.4 timing. What it changes:

- **Frame timing locked to the display.** The 1.4 loop asks for a frame every `1000 / Target FPS` milliseconds and lets the timer's wake-up latency pile up, which is why 144 requested measured 141.7 delivered. On a 144 Hz panel that is a couple of refreshes every second with no new frame: a repeat, which you see as a hitch. Smooth Mode schedules from Windows' own composition clock instead, places every frame half a refresh away from the deadline on either side, and never accumulates drift.
- **Target FPS rounded to an even fraction of the refresh rate.** 60 FPS on a 144 Hz screen means frames held for 2, 3, 2, 3 refreshes, which reads as judder however precise the timer is. Smooth Mode runs the nearest whole fraction instead: 60 becomes 72 on 144 Hz, 48 if you set it to 50, 60 on a 60 Hz screen. Asking for more than the refresh rate gets the refresh rate rather than frames nobody can see.
- **Motion that doesn't depend on frame timing.** With the Classic engine, bar rise and fall, peak caps, beat flash and scope damping were all "move this fraction per frame", so a late frame visibly slowed everything down for a moment. They now scale by how much time actually passed. (The Precision engine runs on elapsed time whatever this is set to.)
- **A proper animation clock.** Wave, Breathe and Rainbow ran off a millisecond counter that only ticks every 15.6 ms, stored in a float that loses precision the longer the PC has been on. They now run off the high-resolution clock. (Always, with the Direct3D 11 renderer.)
- **With the Direct2D renderer:** the background baked once into one image, all bars in one sprite batch, and cached text layouts.

---

![Oscilloscope closeup](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/4.gif)

*The same shape in a warm colour: the Oscilloscope picks up Color Mode like every other shape.*

# ▲ WHERE THE EFFICIENCY COMES FROM

In rough order of measured impact.

### 1. Precision frame pacing: the biggest single win

The obvious way to pace a desktop widget is `DwmFlush()`, which blocks until the monitor's next refresh. That wakes the render thread **on every vertical blank, forever** (60, 144, 240+ times a second) regardless of the target FPS, whether anything needs redrawing, or whether the visualizer is even visible.

This barely registers as CPU% in Task Manager, because the thread is blocked, not spinning. But every wake-up drags a core out of deep idle. Do that continuously and the core never settles into its efficient sleep states, which reads as a small, permanent bump in package power and temperature. The classic "low usage, still runs warm" signature.

> **Note:** this becomes exponentially more noticeable on AMD architecture.
>
> **Note:** also exponentially more noticeable if you have **C-States disabled** in your BIOS or elsewhere. Shoutout to Process Lasso, Core Director, Park Control and HWiNFO64 for helping me work out why all my E-cores were sitting at 65-70 °C when they were supposed to be idle.

**Instead:** a high-resolution waitable timer firing only at the configured rate. Plain `Sleep()` isn't good enough. It's quantized to ~15.6 ms, which would turn a 60 FPS target into stuttery 30-40 FPS.

### 2. Pre-rendered background blur

A Gaussian blur is a full-image convolution: the most expensive thing Direct2D does in this scene. Recomputing it every frame is pure waste, because its input (your wallpaper) never changes.

**Instead:** it is computed exactly once into a cached bitmap, and each frame just copies that. The cache covers only the widget's bounding box, which is **tens of KB of video memory rather than several MB**. It re-bakes automatically if the widget moves or resizes.

### 3. Widget-sized render surface

The visualizer occupies a thin strip, so a desktop-spanning render surface would clear and present millions of untouched pixels every frame, and park two full-desktop buffers in VRAM.

**Instead:** the surface is sized to the widget's bounding box, and the composition layer is offset to position it. That is roughly an order of magnitude less per-frame pixel work and VRAM.

### 4. Cached geometry

Building the background panel and border means allocating a path geometry and constructing four lines and four arcs by hand. It is rebuilt only when size, padding, radii or border width actually change (in normal use, almost never) rather than every frame.

### 5. Cached monitor lookup

`EnumDisplayMonitors()` is a real round-trip through the display driver stack. The monitor is resolved once and cached, refreshed on display change, instead of being looked up per frame.

### 6. Reduced frame latency

DXGI queues up to three frames ahead by default. For a passive widget that's pure latency and power draw with no upside. Capped at **1**.

### 7. Genuine idle shutdown

**Auto-Hide** now stops rendering completely once faded: presents one blank frame, then exits the render path entirely. **Pause When Covered** stops rendering and capture while hidden, checked once per second rather than per frame.

### 8. One wake per frame, for audio and video together *(2.0)*

1.5 woke two threads to get audio onto the screen: a capture thread on every WASAPI packet (about 100 times a second while anything played) and a pacing thread once per frame. 2.0 merges them: once per frame, one thread drains whatever audio has arrived, analyses it and hands the frame over. At 60 FPS that is roughly 100 fewer wake-ups a second.

### 9. Frames that didn't change are never presented *(2.0, Direct3D 11 renderer)*

Every present hands Windows' compositor a new buffer and wakes it to blend that rectangle again, even when the pixels are identical. The renderer now compares a summary of the frame with the last one shown and skips both the draw and the present when nothing moved by a quarter of a pixel. During sustained notes, quiet passages and every idle state, that is most frames, and a compositor with nothing to do goes back to sleep.

### 10. Three steps of idle *(2.0)*

- **Playing**: one wake per frame.
- **Silent** (after Pause When Silent): wakes only when audio arrives or four times a second, and presents nothing unless something changes.
- **Deep Idle** (five seconds later): the loopback stream itself is stopped, and Windows' own peak meter is read four times a second instead. A running capture stream registers an audio power request, which can hold the PC out of sleep (check `powercfg /requests`); a stopped one doesn't. The engine thread also asks Windows for efficiency-core scheduling (EcoQoS) while idle, and leaves the choice to Windows while playing.

### 11. Less for the compositor to blend *(2.0, Direct3D 11 renderer)*

The animated surface covers only the panel; the text has a surface of its own that only changes when the text does. And when the panel is opaque (Blur on, or a solid colour), Windows is told so, and copies those pixels instead of blending them.


---

![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/2.gif)

*The full-width strip in a red colour mode.*

# ▦ ON THE NUMBERS

The figures above come from HWiNFO64 sensor logging: two captures taken back to back in one session, one with the mod running and one with it disabled, with the same music playing throughout both so the only thing that changed was the mod itself. 543 samples running, 384 disabled, at 1.25 s intervals.

The full write-up (raw tables, the method, run-to-run variance, and an honest account of where the measurements fall short) is in [the bench notes](https://github.com/USER-TOURNE/Tourne-Style/tree/main/windhawk-mods/tourne-table/bench). It does not belong on a catalog page, so it is not reproduced here.

The caveats worth stating up front: **HWiNFO measures the whole machine, not this process alone.** A number quoted as the mod's cost therefore includes the work the mod causes elsewhere, in the compositor and the graphics driver, not only the time spent in its own threads. That makes it larger than a per-process trace would show, and it is the honest figure to quote, because it is what the machine actually pays. Both captures also contained short unrelated background spikes, which is why the numbers above are medians rather than averages.

---

![Tourne'Table Audio Visualizer](https://raw.githubusercontent.com/USER-TOURNE/Tourne-Style/main/windhawk-mods/tourne-table/media/3.gif)

*Near-silence: the trace flattens out. When audio stops entirely, the render loop stops with it.*

## ♥ CREDITS

**[USER-TOURNE](https://github.com/USER-TOURNE)**: Author and maintainer: performance work, new features, benchmarking and documentation.

**[Salyts](https://github.com/Salyts)**: Original author of Desktop Audio Visualizer. This project exists because the foundation was good enough to be worth optimizing. Author of his own mod and repo; and a base for this one.

**[GR0UD](https://github.com/GR0UD)**: Audio visualizer code the original was adapted from. Author of his own work; and the base for Salyts.

### A coincidence worth acknowledging

While I was midway through my initial testing, **NeiZ** (author/maintainer, with **SuperSmile123** contributing) released [Desktop Audio Visualizer Plus](https://github.com/ramensoftware/windhawk-mods/commit/e01d0d0dbd6204804235831fd7f68821e4614028) (`neiz-supersmile-audio-visualizer`). I had planned to publish my own commit that same night: holy coincidence.

His release spurred another round of testing on my end, and I've since gone through roughly twelve more iterations. I didn't want to ship something that essentially achieved what I was already going for, especially if they'd figuratively led me out to pasture to put a bullet in me: aha.

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
> The only thing MIT actually asks is that the copyright line rides along: 
> keep the notice, and we're square.
>
> *(I went with MIT over the real WTFPL for one boring reason: this thing
> runs a DLL loaded by Windhawk, and MIT comes with a warranty disclaimer.
> The WTFPL does not. Same energy, fewer ways for my life to get complicated.)*

### Third-party notices

This project builds on upstream work by **Salyts** and **GR0UD**. Their original
code carries its own license terms: if you redistribute this, carry their notices
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
        - goniometer: Goniometer (stereo field)
        - terminal: Terminal (text characters, see the Terminal section)
        - led: LED Meter (segmented, green / amber / red)
        - line: Line Spectrum (filled curve, glowing edge)
        - bloom: Polar Bloom (Radial as one filled shape)
        - spectrogram: Spectrogram (scrolling colour history)
        - vu: VU Needles (two analog meters, L and R)
        - stereo_field: Stereo Field (left above, right below)
        - particles: Particles (bars plus sparks on each beat)
    - reflection: '0'
      $name: Reflection
      $description: 0-100. Mirrors the bars onto a floor beneath them, fading out over this percentage of Bar Max Size. Horizontal bars anchored to the bottom only, with the bar shapes, LED Meter, Line Spectrum and Particles. Both renderers (Direct2D draws it on the CPU, so it costs a little more there)
    - fxGlow: '0'
      $name: Glow
      $description: 0-100. A soft halo around each bar, dot, line and spark, worked out in the same shader pass that draws them, so it costs next to nothing. Direct3D 11 renderer only
    - fxGlowRadius: '6'
      $name: Glow Radius
      $description: 1-32 pixels. How far the halo reaches. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - fxBloom: '0'
      $name: Bloom
      $description: 0-100. Light bleeding out of the whole picture, like a camera lens, from a blurred copy at a quarter of the size added back on top. A little GPU work, and only on frames that change. Direct3D 11 renderer only
    - fxBloomRadius: '16'
      $name: Bloom Radius
      $description: 4-64 pixels. How far the light spreads. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - fxOutlineWidth: '0'
      $name: Outline Width
      $description: 0-16 pixels, decimals allowed (0.5, 1.25). A line just inside each bar's edge, so bars keep their size. 0 = off. Direct3D 11 renderer only
    - fxOutlineColor: '#B0FFFFFF'
      $name: Outline Color
      $description: '#AARRGGBB or #RRGGBB. The outline fades with the bar it belongs to'
    - fxShadow: '0'
      $name: Shadow
      $description: 0-100. A drop shadow behind each bar, dot and line, worked out in the same shader pass as Glow, so it costs next to nothing. Both renderers
    - fxShadowColor: '#000000'
      $name: Shadow Color
      $description: '#RRGGBB'
    - fxShadowX: '2'
      $name: Shadow Offset X
      $description: -32 to 32 pixels, decimals allowed. Positive moves it right
    - fxShadowY: '3'
      $name: Shadow Offset Y
      $description: -32 to 32 pixels, decimals allowed. Positive moves it down
    - fxShadowSoftness: '4'
      $name: Shadow Softness
      $description: 0-32 pixels, decimals allowed. 0 = a hard edge
    - barHollow: false
      $name: Hollow Bars
      $description: Bars as outlines only, the inside left clear. Bar shapes (Stereo, Mountain, Mirror, Wave, Breathe). Direct3D 11 renderer
    - barHollowWidth: '1.5'
      $name: Hollow Line Width
      $description: Pixels, decimals allowed
    - barDash: '0'
      $name: Dash Length
      $description: Pixels, decimals allowed. Cuts the bars into dashes along their length, fixed to the base line so they stay put as a bar grows. 0 = solid. Bar shapes, Direct3D 11 renderer
    - barDashGap: '2'
      $name: Dash Gap
      $description: Pixels between dashes, decimals allowed
    - barTilt: '0'
      $name: Tilt
      $description: -60 to 60 degrees, decimals allowed. Leans every bar from its base line, like italics; peak caps and the Afterimage lean with them. Bar shapes, Direct3D 11 renderer
    - barMirrorGap: '0'
      $name: Mirror Gap
      $description: Pixels, decimals allowed. With Anchor = Middle the bars already grow both ways from a centre line; this pulls the two halves apart, like a waveform with a spine. Bar shapes, Direct3D 11 renderer
    - afterimage: '0'
      $name: Afterimage
      $description: 0-100. A see-through trail behind each bar that jumps up with it and sinks back slowly. With Peak Hold on, the caps sit on the trail. Bar shapes, Direct3D 11 renderer; keeps the analysis on the CPU (Hybrid)
    - afterimageLength: '1.2'
      $name: Afterimage Length
      $description: Seconds for a full-height trail to sink away, decimals allowed
    - orientation: horizontal
      $name: Orientation
      $description: Whether bars run left-to-right or bottom-to-top
      $options:
        - horizontal: Horizontal
        - vertical: Vertical
    - barCount: 32
      $name: Bar Count
      $description: How many bars are drawn across the visualizer
    - barWidth: '6'
      $name: Bar Width
      $description: Thickness of each bar, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - barGap: '4'
      $name: Bar Gap
      $description: Space between bars, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - barMaxSize: '140'
      $name: Bar Max Size
      $description: Maximum height (or length, if vertical) a bar can reach at full volume, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - barIdleSize: '4'
      $name: Bar Idle Size
      $description: Minimum height bars keep when there's no audio, so the visualizer never looks completely flat. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - barCornerRadius: '3'
      $name: Bar Corner Radius
      $description: One value for all corners, or four space-separated values for top-left, top-right, bottom-right, bottom-left
    - colorMode: solid
      $name: Color Mode
      $description: How bars are colored. See the Color, Gradient Color 1/2 and Rainbow Cycle Speed settings below for the modes that use them
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
      $description: 'Used when Color Mode is Solid. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - gradientColor1: '#FF1ED760'
      $name: Gradient Color 1
      $description: 'Start color for Gradient and Reactive Gradient modes. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - gradientColor2: '#FF00B4FF'
      $name: Gradient Color 2
      $description: 'End color for Gradient and Reactive Gradient modes. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - sensitivity: '150'
      $name: Sensitivity
      $description: How hard the bars react to audio. Range 0-300. Past a point, raising this mostly makes quiet passages louder rather than making loud passages hit any harder, since loud signal is already at max bar height, see Sensitivity Curve
    - sensitivityCurve: knee
      $name: Sensitivity Curve
      $description: How the top of the Sensitivity range is handled once a band's signal would otherwise overshoot max bar height. Exponential is smoothest but softens loud hits earliest; Knee keeps quiet-to-moderate signal untouched and only compresses the loud end; Power is the cheapest and gives the most headroom before compressing
      $options:
        - exponential: Exponential (Soft)
        - knee: Knee (Balanced)
        - power: Power (Headroom)
    - inputGain: '0'
      $name: Input Gain (dB)
      $description: A fixed level adjustment applied to the captured audio before anything else. Range -24 to +24, where 0 leaves the signal exactly as captured. Loopback capture sees each app's own volume slider but not the Windows master slider, so a music app sitting at 40% reads as quiet no matter how loud the system sounds. This is the control for that. Unlike Sensitivity it also scales the Oscilloscope waveform
    - autoGain: false
      $name: Auto Gain
      $description: Continuously adapts the level so quiet sources still fill the bars, without retuning Sensitivity per app or per track. Boost only, so loud material behaves exactly as it does with this off. Held steady through silence, which means the noise floor is never lifted and idle shutdown still works normally
    - autoGainMaxBoost: '12'
      $name: Auto Gain Max Boost (dB)
      $description: The ceiling on how far Auto Gain may lift a quiet signal. Range 0 to 24. Lower it if quiet passages are getting flattened more than you want, raise it if a very quiet source still will not fill the bars. Only used when Auto Gain is on
    - smoothing: '0'
      $name: Motion Smoothing
      $description: Slows down how quickly bars rise and fall toward the target level, at the cost of a bit of lag. 0 is the original snappy response; higher values trade responsiveness for a steadier, easier-to-read motion -- helpful when bars are small on the desktop and fast jitter is hard to track
    - eqPreset: default
      $name: EQ Preset
      $description: Reshapes how much each frequency range (bass/mid/treble) is boosted, tuned per genre
      $options:
        - default: Default
        - bass: Bass
        - rock: Rock
        - pop: Pop
        - jazz: Jazz
        - electronic: Electronic
    - fftSize: '2048'
      $name: FFT Size
      $description: Higher values give finer frequency detail at a small extra CPU cost. With the Precision engine this is the size of each of its three resolution tiers, so bass detail comes from the tiers rather than from raising this; 2048 is the sweet spot. Workload GPU uses at most 4096
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
        - bark: Bark (critical bands, Traunmueller)
        - erb: ERB (auditory filter bandwidths, Glasberg and Moore)
    - peakFreqEnabled: false
      $name: Peak Frequency Readout
      $description: Shows the current dominant frequency as a small numeric overlay
    - peakFreqAlignH: right
      $name: Peak Readout - Horizontal Position
      $description: Where the frequency readout sits left-to-right
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
    - oscilloscopeWindowMs: '24'
      $name: Oscilloscope Time Window (ms)
      $description: How much audio the trace shows end to end, in milliseconds. Range 5 to 250. Low values stretch out individual wave cycles and look like a hardware scope; high values compress more of the signal into the same width and read as a rolling envelope. This is a real time base, independent of FFT Size, so changing frequency resolution no longer changes what the trace looks like. Around 150 to 180 is close to what FFT Size 8192 used to give. Only affects the Oscilloscope shape
    - oscilloscopeDamping: '0'
      $name: Oscilloscope Damping
      $description: Slows how fast the trace changes shape, without slowing how often it is drawn. Range 0 to 100. 0 is the raw trace, redrawn every frame. Higher values ease each point toward its new position so the waveform moves slowly enough to follow with your eyes, which is what makes a wide time window legible rather than busy. Pair it with a high Time Window. Costs nothing and does not change your frame rate. Only affects the Oscilloscope shape
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
    - peakHoldColor: '#FF00B4FF'
      $name: Peak Hold Cap Color
      $description: 'Color of the peak hold caps. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b). Independent of Color Mode -- caps are always drawn in this color, even with Rainbow/Album Art/Accent/Acrylic'
    - beatFlashEnabled: false
      $name: Beat Flash
      $description: Flash bars toward Beat Flash Color on detected bass hits
    - beatFlashColor: '#FFFFFFFF'
      $name: Beat Flash Color
      $description: 'Color bars flash toward on a beat. Accepts #AARRGGBB, #RRGGBB, rgba(r, g, b, a) with alpha 0-1, or rgb(r, g, b). Alpha controls how strong the flash gets even at full intensity -- lower it for a subtler tint instead of a full color swap'
    - beatFlashIntensity: '80'
      $name: Beat Flash Intensity
      $description: How quickly a beat reaches full Beat Flash Color. Only used when Beat Flash is on
    - rainbowSpeed: '40'
      $name: Rainbow Cycle Speed
      $description: Only used when Color Mode is Rainbow Cycle
    - nowPlayingEnabled: false
      $name: Now Playing Text
      $description: Show artist and title above the visualizer, using Windows media session info
    - nowPlayingColor: '#FFFFFFFF'
      $name: Now Playing Text Color
      $description: 'Color of the artist/title text. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - nowPlayingFont: Segoe UI
      $name: Now Playing Font
      $description: Font family name, must be installed on your system (e.g. a Nerd Font for glyph support)
    - nowPlayingFontSize: '16'
      $name: Now Playing Font Size
      $description: Text size, in points. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - nowPlayingDisplaySeconds: '6'
      $name: Now Playing Display Seconds
      $description: How long the text stays visible after a track changes, before fading out. 0 keeps it on screen all the time, like a media widget
    - nowPlayingOffsetX: '0'
      $name: Now Playing Offset X
      $description: Shifts the artist/title text sideways from where it normally sits, in pixels. Negative moves it left, positive right. It still travels with the visualizer -- this only changes where it sits relative to it, which is how you move it clear of the background panel. Can also be nudged live with the keyboard (Interaction, move target 2). A keyboard nudge ADDS to this number rather than replacing it, so whatever you type here always counts. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - nowPlayingOffsetY: '0'
      $name: Now Playing Offset Y
      $description: Shifts the artist/title text up or down from where it normally sits, in pixels. Negative moves it up, positive down. As with Offset X, a keyboard nudge adds to this rather than replacing it. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - nowPlayingBgColor: '#00000000'
      $name: Now Playing Background
      $description: 'A panel drawn behind the artist/title text, sized to the text itself rather than to the visualizer. Fully transparent by default, meaning no panel. Give it some alpha to make the text readable over a busy wallpaper without having to enlarge the main Background panel. It fades in and out with the text. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - nowPlayingBgPadding: '6'
      $name: Now Playing Background Padding
      $description: Space between the text and the edge of its panel, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - nowPlayingBgCornerRadius: '6'
      $name: Now Playing Background Corner Radius
      $description: Roundness of the text panel's corners, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - nowPlayingBgBorderSize: '0'
      $name: Now Playing Background Border Size
      $description: Outline thickness around the text panel, in pixels. 0 disables it. Draws whether or not the panel itself has any fill, so an outline on its own is possible. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - nowPlayingBgBorderColor: '#40FFFFFF'
      $name: Now Playing Background Border Color
      $description: 'Color of the text panel''s outline. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - peakFreqOffsetX: '0'
      $name: Peak Readout Offset X
      $description: Shifts the frequency readout sideways from its chosen alignment, in pixels. Negative moves it left, positive right. Use this for fine placement -- the alignment options above put it in the right general area, this puts it exactly where you want it. Can also be nudged live with the keyboard (Interaction, move target 3), which adds to this number rather than replacing it. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - peakFreqOffsetY: '0'
      $name: Peak Readout Offset Y
      $description: Shifts the frequency readout up or down from its chosen alignment, in pixels. Negative moves it up, positive down. As with Offset X, a keyboard nudge adds to this rather than replacing it. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - peakFreqBgColor: '#00000000'
      $name: Peak Readout Background
      $description: 'A panel drawn behind the frequency readout, sized to the text itself. Fully transparent by default, meaning no panel. Particularly useful for this one, since the readout sits over the bars on the inside alignments and can be hard to read against them. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - peakFreqBgPadding: '6'
      $name: Peak Readout Background Padding
      $description: Space between the readout and the edge of its panel, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - peakFreqBgCornerRadius: '6'
      $name: Peak Readout Background Corner Radius
      $description: Roundness of the readout panel's corners, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - peakFreqBgBorderSize: '0'
      $name: Peak Readout Background Border Size
      $description: Outline thickness around the readout panel, in pixels. 0 disables it. Draws whether or not the panel itself has any fill. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - peakFreqBgBorderColor: '#40FFFFFF'
      $name: Peak Readout Background Border Color
      $description: 'Color of the readout panel''s outline. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - nowPlayingLayout: one_line
      $name: Now Playing Layout
      $description: One line ("Artist - Title") or two (the title, with the artist underneath in its own colour)
      $options:
        - one_line: One line
        - two_lines: Two lines (title, then artist)
    - nowPlayingPlacement: above
      $name: Now Playing Placement
      $description: Above the bars, as before, or inside the background panel, in the padding above or below the bars, as wide as the bars so Left and Right line up with them. For the inside placements give the panel enough padding on that side (Background, Padding) to hold the text
      $options:
        - above: Above the visualizer
        - panel_top: Inside the panel, top
        - panel_bottom: Inside the panel, bottom
    - nowPlayingAlign: center
      $name: Now Playing Alignment
      $options:
        - center: Center
        - left: Left
        - right: Right
    - nowPlayingArtistColor: '#B3FFFFFF'
      $name: Now Playing Artist Color
      $description: 'Colour of the artist: the second line in the two-line layout, the "Artist" part of the one-line layout. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - textRendering: smooth
      $name: Text Rendering
      $description: Smooth antialiases text. Pixel draws it without antialiasing, on whole pixels, for pixel fonts (e.g. Press Start 2P, Silkscreen, Pixelify Sans, installed in Windows) at a multiple of their design size. Applies to the Now Playing text, the readout and the Terminal shape
      $options:
        - smooth: Smooth
        - pixel: Pixel (no antialiasing)
  $name: Appearance
- audio:
    - source: default_output
      $name: Audio Source
      $description: What the visualizer listens to. The default output is what you hear (as before). An input is a microphone, a line-in or the recording side of a virtual cable. Or pick a device by name, which covers any output or input, virtual ones included (VB-Audio Cable / Matrix, Voicemeeter, an Ableton return routed to its own device). The right-click menu lists every active device and switches live
      $options:
        - default_output: Default output (what you hear)
        - default_input: Default input
        - named: A device by name (below)
    - deviceName: ''
      $name: Device Name
      $description: 'Part of the device''s name, as Windows Sound settings show it, e.g. "VB-Audio Matrix" or "CABLE Output". Outputs are captured by loopback, inputs recorded directly. If it isn''t connected the default output stands in, with a heads-up, until it is. Only used when Audio Source is "A device by name"'
  $name: Audio Source
- progress:
    - enabled: false
      $name: Track Progress Bar
      $description: A thin bar showing how far into the current track you are, from the same Windows media session as Now Playing. Works with players that report their position (Spotify, most browsers, foobar2000, MusicBee and more)
    - placement: below
      $name: Placement
      $options:
        - below: Below the panel
        - above: Above the panel
        - panel_bottom: Inside the panel, under the bars
    - height: '2'
      $name: Height
      $description: In pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - gap: '6'
      $name: Gap
      $description: Distance from the panel (or, inside it, from the bars), in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - color: '#FFFFFFFF'
      $name: Color
      $description: 'The played part. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - trackColor: '#40FFFFFF'
      $name: Track Color
      $description: 'The rest of the bar. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - clickToSeek: false
      $name: Click to Seek
      $description: Click or drag along the bar to jump in the track. Only where the desktop itself is under the cursor, never through a window, and the drag key still moves the visualizer. Off by default so the bar never takes clicks meant for the desktop
    - seekAppsMode: except
      $name: Seek App List
      $description: How the app list below applies. Apps count while they are running
      $options:
        - except: Seek, except while these apps run
        - only: Seek only while these apps run
    - seekApps: ''
      $name: Seek Apps
      $description: 'Program names, separated by commas, e.g. "League of Legends, obs64, Spotify". ".exe" is optional. Empty = the list does nothing'
  $name: Track Progress
- terminal:
    - style: columns
      $name: Terminal Style
      $description: Only used when Shape is Terminal. Columns draws every bar as a column of characters. Waterfall is a spectrogram in characters, newest line on top. Meters prints Bass, Mid, Treble and Volume as text meters
      $options:
        - columns: Columns
        - waterfall: Waterfall
        - meters: Meters
    - font: Consolas
      $name: Font
      $description: A monospace font installed in Windows (Consolas, Cascadia Mono, a Nerd Font, a pixel font)
    - fontSize: '14'
      $name: Font Size
      $description: In points. Sets the cell size, and with it the size of the whole grid; Bar Width, Gap and Max Size don't apply to this shape. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - rows: 16
      $name: Rows
      $description: Height of Columns and Waterfall, in lines. Columns are one per bar (Bar Count, or the band layout)
    - meterColumns: 40
      $name: Meter Width
      $description: Width of the Meters style, in characters
    - hotThreshold: '75'
      $name: Hot Threshold (%)
      $description: Above this share of full height a character turns the Hot Color
    - columnGlyph: '#'
      $name: Column Glyph
      $description: 'The character columns and meters are built from. Any one character: ASCII, Unicode symbols (█ ▓ ● ◆ ★), box drawing, Nerd Font icons or emoji, as long as the font has it'
    - peakGlyph: '-'
      $name: Peak Glyph
      $description: The peak cap in Columns (with Peak Hold on). Any one character, as above
    - ramp: ' .:-=+*#%@'
      $name: Waterfall Ramp
      $description: 'Characters from quiet to loud for the Waterfall, any characters, e.g. " ░▒▓█" or " ·•●". Up to 128 different non-ASCII characters across all three settings'
    - scrollRate: '20'
      $name: Waterfall Speed
      $description: Lines per second
    - lowColor: '#FF33FF66'
      $name: Color
    - highColor: '#FFFF3B3B'
      $name: Hot Color
    - dimColor: '#FF1E6B34'
      $name: Dim Color
      $description: The faintest level of the Waterfall
    - labelColor: '#FFB8FFB8'
      $name: Label Color
      $description: The Meters labels
  $name: Terminal
- analysis:
    - engine: precision
      $name: Analysis Engine
      $description: Precision gives every bar its own frequency band, measured straight from the spectrum, with bass from long high-resolution windows and treble from short fast ones. Classic is the 1.4 analysis, 7 bands that the bars interpolate between, kept for comparison and for anyone who prefers its look. Everything else in this section only applies to Precision
      $options:
        - precision: Precision (every bar its own band)
        - classic: Classic (1.4, 7 bands)
    - bandLayout: scale
      $name: Band Layout
      $description: How the spectrum is cut into bars. Frequency Scale spreads Bar Count bars across Min to Max Frequency on the Frequency Scale chosen above. IEC 61260 uses the standard fractional-octave bands every RTA uses (Octave Fraction picks 1/1 to 1/24), and Musical gives one bar per note of equal temperament. Both of those decide the number of bars themselves, so Bar Count is ignored for them
      $options:
        - scale: Frequency Scale (uses Bar Count)
        - iec: IEC 61260 fractional-octave bands
        - musical: Musical notes (equal temperament)
    - octaveFraction: '6'
      $name: Octave Fraction
      $description: Bandwidth for the IEC 61260 and Musical layouts. 1/3 is the classic RTA; 1/12 is one bar per semitone; 1/24 is a quarter tone, about 240 bars from 20 Hz to 20 kHz. Musical only offers 1/12 and 1/24; anything coarser counts as 1/12 there
      $options:
        - '1': 1/1 octave
        - '3': 1/3 octave
        - '6': 1/6 octave
        - '12': 1/12 octave (semitone)
        - '24': 1/24 octave (quarter tone)
    - minFrequency: '20'
      $name: Min Frequency (Hz)
      $description: Bottom of the analysed range. Range 10 to 1000
    - maxFrequency: '20000'
      $name: Max Frequency (Hz)
      $description: Top of the analysed range. Range 1000 to 21000; anything above the device's Nyquist limit is trimmed automatically
    - tuningA4: '440'
      $name: Tuning (A4, Hz)
      $description: Reference pitch for the Musical layout. Range 400 to 480
    - weighting: z
      $name: Weighting
      $description: Frequency weighting from IEC 61672-1, added per band. Z is flat, the honest view of the signal. A follows the ear's sensitivity at quiet listening levels and pulls the bass right down; C is the gentler loud-level curve. Costs nothing
      $options:
        - z: Z (flat)
        - a: A (IEC 61672)
        - c: C (IEC 61672)
    - tilt: '1.5'
      $name: Tilt (dB per octave)
      $description: A slope around 1 kHz. Bars are referenced to a 1/3-octave band (see Level Reference), so pink noise already reads flat at 0. Most music falls a little faster than pink, and 1.5 makes a typical modern mix look level, the same thing SPAN's 4.5 dB/oct default does on a plain FFT display
      $options:
        - '0': 0 (pink noise flat)
        - '1.5': 1.5 (typical mix flat)
        - '3': '3'
        - '4.5': '4.5'
        - '6': '6'
    - detector: rms
      $name: Detector
      $description: RMS adds up the power of every frequency in the band, the right reading for music and noise. Peak shows the band's loudest single frequency, closer to what a tone measures
      $options:
        - rms: RMS (band power)
        - peak: Peak (loudest bin)
    - levelReference: third_octave
      $name: Level Reference
      $description: Third-octave scales every band to the power a 1/3-octave band at the same centre would hold, so bar heights don't change when you change Bar Count or Band Layout, and pink noise reads flat. Band shows each band's true power, so wide bars read higher than narrow ones
      $options:
        - third_octave: Third-octave (independent of bar count)
        - band: Band (true band power)
    - window: hann
      $name: FFT Window
      $description: Hann is the general-purpose choice. Blackman-Harris trades a little resolution for 92 dB of dynamic range, so quiet detail next to loud tones stays visible. Flat-top reads the exact level of a tone wherever it falls, at the cost of resolution. Hamming has a narrower peak and more leakage
      $options:
        - hann: Hann
        - hamming: Hamming
        - blackman_harris: Blackman-Harris (high dynamic range)
        - flat_top: Flat-top (accurate tone levels)
    - bassDetail: '2'
      $name: Bass Detail
      $description: The resolution tiers. Each bar is measured from the fastest tier that can resolve it. High adds a tier decimated by 16, which is what makes 1/12 and 1/24-octave bass bars real measurements; Standard stops at 4; Off is a single FFT, like the classic engine
      $options:
        - '2': High (3 tiers)
        - '1': Standard (2 tiers)
        - '0': Off (single FFT)
    - channel: mix
      $name: Channel
      $description: Which signal the spectrum is taken from. Mix is all channels averaged, as before. Mid is what both speakers share, Side is the difference between them, which shows exactly where the stereo width in a mix lives
      $options:
        - mix: Mix (all channels)
        - left: Left
        - right: Right
        - mid: Mid (L+R)
        - side: Side (L-R)
    - dbFloor: '-72'
      $name: Display Floor (dBFS)
      $description: The level that draws as an empty bar. Range -120 to -20. Lower shows quieter detail, at the cost of noise and hiss showing too
    - dbCeiling: '-12'
      $name: Display Ceiling (dBFS)
      $description: The level that draws as a full bar, before Sensitivity. Range -60 to 0. A full-scale sine is 0 dBFS; loud modern music sits around -20 per third-octave band
    - ballistics: snappy
      $name: Ballistics
      $description: How bars rise and fall, all on real time, so they move the same at any frame rate. Snappy is closest to the 1.4 feel. Analyzer falls a steady 20 dB per second like a hardware RTA. VU integrates over 300 ms. The two PPMs follow IEC 60268-10 Type I (DIN) and Type II (EBU) return rates. Custom uses the two settings below. Motion Smoothing above slows whichever you pick
      $options:
        - snappy: Snappy
        - smooth: Smooth
        - analyzer: Analyzer (20 dB/s)
        - vu: VU
        - ppm_ebu: PPM, EBU (Type II)
        - ppm_din: PPM, DIN (Type I)
        - custom: Custom
    - attackMs: '10'
      $name: Custom Attack (ms)
      $description: Rise time constant for Ballistics = Custom. Range 1 to 500
    - releaseDbPerSecond: '20'
      $name: Custom Release (dB/s)
      $description: Fall rate for Ballistics = Custom. Range 1 to 200
    - peakHoldMs: '500'
      $name: Peak Hold Time (ms)
      $description: How long a peak cap hangs before it falls. Range 0 to 5000. Only used with Peak Hold Caps on
    - peakFall: gravity
      $name: Peak Fall
      $description: Gravity accelerates like a dropped object, the classic analyzer look. Linear falls at a steady rate
      $options:
        - gravity: Gravity
        - linear: Linear
    - readout: frequency
      $name: Readout Content
      $description: What the readout set up under Peak Frequency Readout shows. Loudness is ITU-R BS.1770-5 / EBU R128 momentary (400 ms), short-term (3 s) and gated integrated loudness. Full adds 4x-oversampled true peak, PLR (true peak minus integrated loudness) and stereo correlation (+1 mono, 0 wide, -1 out of phase). The loudness meters only run while they are shown
      $options:
        - frequency: Peak frequency
        - loudness: Loudness (LUFS M / S / I)
        - loudness_full: Loudness, true peak, PLR, correlation
        - both: Peak frequency and loudness
    - loudnessResetOnTrack: true
      $name: Reset Integrated Loudness On Track Change
      $description: Starts a new integrated measurement whenever the playing track changes, so the figure is per track. Off measures from when the mod started
  $name: Analysis (Precision Engine)
- position:
    - host: desktop
      $name: Place In
      $description: Desktop keeps the visualizer on the wallpaper layer, behind your apps. Taskbar puts it inside the taskbar, above it, and the positions below then mean a percentage across and down the taskbar instead of the screen. It never takes clicks there; the taskbar under it still works, except that the right-click menu and drag answer over the visualizer's own box. Panel gives it a small window of its own, the size of the visualizer, that floats with your apps; drag it to move it, drag an edge or corner to scale it, and pin or hide it from the right-click menu (the positions below don't apply there)
      $options:
        - desktop: Desktop (behind apps)
        - taskbar: Taskbar
        - panel: Panel (a window of its own)
    - panelBackdrop: acrylic
      $name: Panel Backdrop
      $description: Place In = Panel only. What shows behind the visualizer in its window. Acrylic and Mica need Windows 11; on Windows 10 it stays clear
      $options:
        - none: Clear
        - acrylic: Acrylic (frosted)
        - mica: Mica
    - panelHotkey: ''
      $name: Panel Hotkey
      $description: Place In = Panel only. Shows or hides the panel from anywhere, e.g. Ctrl+Alt+V or Win+Shift+F9. Empty for none
    - horizontalPosition: '50'
      $name: Horizontal Position
      $description: 0-100, percentage across the monitor's work area. Decimals are allowed (e.g. 50.25) for fine-grained placement -- useful since a whole-number step can be a big jump in pixels on a large monitor. IMPORTANT -- once you have moved the visualizer by keyboard or by dragging, that saved position REPLACES this field, and editing this will appear to do nothing until you clear it with the move modifier + Home (see Interaction)
    - verticalPosition: '88'
      $name: Vertical Position
      $description: 0-100, percentage down the monitor's work area. Decimals are allowed (e.g. 88.5) for fine-grained placement. Same caveat as Horizontal Position -- a saved keyboard/drag position takes priority over this field until it is cleared with the move modifier + Home
    - monitor: 1
      $name: Monitor
      $description: 1-based monitor index
    - pixelSnap: true
      $name: Pixel Snap
      $description: Puts the panel, every bar, the text and the grid on whole pixels, so edges stay razor sharp at any size and any display scaling (at 125% or 150%, sizes otherwise land on half pixels and blur). Turn off for subpixel placement, which moves the picture by fractions of a pixel at the cost of antialiased, slightly soft edges. Bar heights move smoothly either way
    - dockApp: ''
      $name: Dock To App
      $description: 'Program name, e.g. "Spotify" or "obs64" (".exe" optional). The visualizer then sits beside that app''s main window and follows it as it moves; the Now Playing text and readout come along. While the app is closed or minimized it goes back to its usual place. It stays on the desktop layer, so it shows beside the window, not on top of it. Empty = not docked'
    - dockSide: below
      $name: Dock Side
      $options:
        - below: Below the window
        - above: Above the window
        - left: Left of the window
        - right: Right of the window
    - dockAlign: center
      $name: Dock Alignment
      $description: Where along that side it sits
      $options:
        - start: Start (left or top)
        - center: Centre
        - end: End (right or bottom)
    - dockGap: '0'
      $name: Dock Gap
      $description: Pixels between the window and the visualizer. Negative overlaps
    - dockShift: '0'
      $name: Dock Shift
      $description: Pixels to slide it along the side from the alignment above
    - split: false
      $name: Split Into Two Pieces
      $description: Cuts the visualizer in two and moves the second piece by the gap and shift below, e.g. to sit either side of the taskbar's centred icons or a monitor bezel. Works with every shape and both renderers and costs next to nothing (Windows moves the piece; nothing is drawn twice). The background panel is cut at the same place. Drag, the right-click menu and Click to Seek answer on the first piece only
    - splitAt: '50'
      $name: Split Point
      $description: Percent of the way along where the cut goes. With bars it lands in the gap between two bars
    - splitGap: '200'
      $name: Split Gap
      $description: Pixels the second piece moves along the visualizer (right, or down when vertical). Negative moves it back
    - splitShift: '0'
      $name: Split Shift
      $description: Pixels the second piece moves across it (down, or right when vertical)
  $name: Position
- interaction:
    - keyMoveEnabled: false
      $name: Enable Keyboard Move
      $description: Off by default. While it is off the mod doesn't watch the keyboard at all. Hold the modifier below and tap a direction key to nudge the visualizer one pixel at a time, the way a window manager moves a tiled window. This is the precise way to place it -- it lands exactly where you put it, with no cursor to keep up with. Hold the Fast Key as well to jump by the larger step instead. The same combo with 1, 2, 3 or 4 switches what the direction keys steer -- 1 the visualizer, 2 the Now Playing text, 3 the frequency readout, 4 the Media Controls strip -- so everything movable is placed with one shortcut, and the choice sticks until you change it. Modifier + Home resets whichever is selected. Worth knowing -- the visualizer and the strip store a percentage that REPLACES their Position settings, so those fields stop having any effect until you reset; the two text overlays instead store a delta ADDED to their Offset settings, so those fields always still count. Also, if the strip is currently parked by Hide When Covered, nudging it still works but you will not see it move until it comes back
    - keyMoveModifier: ctrl_alt
      $name: Keyboard Move Modifier
      $description: Held down while tapping a direction key. Use a two-key combo. This mod swallows the keypress outright, so the app underneath does not fall back to its own behaviour -- it simply never hears about it. With a one-key modifier that means real losses -- Ctrl with WASD eats Ctrl+A and, worse, Ctrl+S, so a save you think you made silently does not happen. Ctrl with arrows eats word-by-word cursor movement. Pick a one-key modifier and the mod will warn you on load listing exactly what you gave up
      $options:
        - ctrl_alt: Ctrl + Alt
        - ctrl_shift: Ctrl + Shift
        - alt_shift: Alt + Shift
        - win_alt: Win + Alt
        - win_shift: Win + Shift
        - ctrl: Ctrl
        - alt: Alt
        - shift: Shift
        - win: Win
    - keyMoveKeys: both
      $name: Keyboard Move Direction Keys
      $options:
        - both: Arrow Keys and WASD
        - arrows: Arrow Keys only
        - wasd: WASD only
    - keyMoveStep: '1'
      $name: Keyboard Move Step
      $description: How far one press moves the visualizer, in pixels. 1 gives true pixel-by-pixel placement. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - keyMoveFastStep: '10'
      $name: Keyboard Move Fast Step
      $description: How far one press moves it while the Fast Key below is also held, in pixels. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - keyMoveFineStep: '0.25'
      $name: Keyboard Move Fine Step
      $description: A subpixel step for the visualizer, used while the Fine Key is held or with Subpixel Nudges ticked in the right-click menu. Only shows as movement with Pixel Snap off (Position); with it on, the position still accumulates and the picture moves a whole pixel at a time
      $options:
        - '0.5': 1/2 pixel
        - '0.25': 1/4 pixel
        - '0.125': 1/8 pixel
        - '0.0625': 1/16 pixel
    - keyMoveFineKey: none
      $name: Keyboard Move Fine Key
      $description: Held alongside the modifier for the fine step. Must not be one of the modifier's keys or the Fast Key. None leaves fine steps to the right-click menu's Subpixel Nudges
      $options:
        - none: None
        - shift: Shift
        - ctrl: Ctrl
        - alt: Alt
        - win: Win
    - keyMoveFastKey: shift
      $name: Keyboard Move Fast Key
      $description: Held alongside the modifier to use the larger step. Must not be one of the keys already used by the modifier above
      $options:
        - shift: Shift
        - ctrl: Ctrl
        - alt: Alt
        - win: Win
        - none: None (disable fast step)
    - dragEnabled: false
      $name: Enable Drag-to-Move
      $description: Hold the modifier + mouse button below anywhere over the visualizer and drag to reposition it. Bar rendering pauses for the duration of the drag -- only the background/border box (if Background is enabled) moves. Double-click the same combo without dragging to clear a dragged position. Note this rides a global mouse hook and has to repaint the whole visualizer to keep up with the cursor, so on a heavy shape or a high bar count it can feel sluggish next to the keyboard move above -- which is why it's off by default now
    - dragSnap: true
      $name: Snap While Dragging
      $description: While dragging, the visualizer's edges and centre catch on the screen's edges and centre, the taskbar's edge, the tray and the Start button. Hold Shift (Alt when Shift is the drag key) to place it freely
    - dragSnapDistance: '8'
      $name: Snap Distance
      $description: How close, in pixels, an edge has to come before it snaps
    - dragModifier: ctrl
      $name: Drag Modifier Key
      $description: Held together with the mouse button below to start a drag
      $options:
        - none: None
        - ctrl: Ctrl
        - alt: Alt
        - shift: Shift
        - win: Win
    - dragButton: middle
      $name: Drag Mouse Button
      $options:
        - left: Left Click
        - middle: Middle Click
        - right: Right Click
    - contextMenu: right_click
      $name: Right-Click Menu
      $description: Right-click the visualizer for a menu of quick settings (shape, colours, analysis, audio source, overlays, pause). It only opens where the visualizer is actually showing on the desktop, never through a window covering it. Ctrl + Right-Click leaves a plain right-click to the desktop
      $options:
        - right_click: Right-Click
        - ctrl_right_click: Ctrl + Right-Click
        - 'off': 'Off'
  $name: Interaction
- media_controls:
    - enabled: false
      $name: Enabled
      $description: Adds a small Previous / Play-Pause / Next control strip, wired to whatever app is currently playing media (Spotify, a browser, etc.) via the same native Windows media session API the Now Playing text reads from. Unlike the visualizer itself, this strip sits on top of other windows rather than behind the desktop icons, since it has to actually receive your clicks
    - iconColor: '#FFFFFFFF'
      $name: Icon Color
      $description: 'Color for the built-in Previous/Play/Pause/Next glyphs, used whenever a custom icon path below is left blank. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - iconPrevPath: ''
      $name: Previous Icon Path
      $description: Full path to a local image file (PNG, JPG, BMP, or ICO -- transparency supported) to use for the Previous button. Leave blank to use the built-in icon
    - iconPlayPath: ''
      $name: Play Icon Path
      $description: Full path to a local image file for the Play button, shown while paused/stopped. Leave blank to use the built-in icon
    - iconPausePath: ''
      $name: Pause Icon Path
      $description: Full path to a local image file for the Pause button, shown while something is playing. Leave blank to use the built-in icon
    - iconNextPath: ''
      $name: Next Icon Path
      $description: Full path to a local image file for the Next button. Leave blank to use the built-in icon
    - iconSize: '32'
      $name: Icon Size
      $description: Size of each button, in pixels (square). Decimals allowed; the media controls round to whole pixels
    - iconSpacing: '14'
      $name: Icon Spacing
      $description: Gap between buttons, in pixels. Decimals allowed; the media controls round to whole pixels
    - plateColor: '#00000000'
      $name: Backing Plate Color
      $description: 'A panel drawn behind the whole icon strip. Fully transparent by default, so your icons sit directly on the wallpaper with nothing behind them. Raise the alpha if pale icons are hard to pick out against a light wallpaper -- e.g. #8C141414 for a soft dark plate. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - platePadding: '0'
      $name: Backing Plate Padding
      $description: Breathing room between the icons and the edge of the backing plate, in pixels. This grows the strip itself rather than shrinking the icons, so raising it never makes the buttons smaller or harder to click. Note that it grows the strip whether or not the plate is visible, which very slightly shifts where the Horizontal/Vertical Position percentages land. Decimals allowed; the media controls round to whole pixels
    - plateCornerRadius: '8'
      $name: Backing Plate Corner Radius
      $description: Roundness of the backing plate corners, in pixels. Only used when the plate color above has some alpha. Decimals allowed; the media controls round to whole pixels
    - plateBorderSize: '0'
      $name: Backing Plate Border Size
      $description: Outline thickness around the backing plate, in pixels. 0 disables it. The border draws whether or not the plate itself has any fill, so you can have an outline on its own with nothing behind the icons. Decimals allowed; the media controls round to whole pixels
    - plateBorderColor: '#40FFFFFF'
      $name: Backing Plate Border Color
      $description: 'Color of the backing plate outline. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - stacking: top
      $name: Stacking
      $description: Whether the strip or card stays above other windows
      $options:
        - top: Always on top (covers apps, even ones run as administrator)
        - normal: Normal window (apps you click on cover it)
    - hideWhenCovered: false
      $name: Hide When Covered
      $description: The strip is topmost, so by default it stays on screen over whatever else you have open. Turn this on to have it get out of the way while a real application window sits underneath it, and come back when that window moves or closes. Note that while it is parked out of the way it is genuinely hidden, so keyboard nudges (move target 4) still apply but you will not see them land until it comes back. Coverage is re-checked once a second, not instantly
    - coveredThresholdPercent: '50'
      $name: Hide When Covered - Threshold
      $description: How much of the strip has to be underneath a window before it hides. Only used when Hide When Covered is on
      $options:
        - '5': 5% covered
        - '10': 10% covered
        - '15': 15% covered
        - '20': 20% covered
        - '25': 25% covered
        - '30': 30% covered
        - '35': 35% covered
        - '40': 40% covered
        - '45': 45% covered
        - '50': 50% covered
        - '55': 55% covered
        - '60': 60% covered
        - '65': 65% covered
        - '70': 70% covered
        - '75': 75% covered
        - '80': 80% covered
        - '85': 85% covered
        - '90': 90% covered
        - '95': 95% covered
        - '100': 100% covered (fully hidden)
    - horizontalPosition: '50'
      $name: Horizontal Position
      $description: 0-100, percentage across the monitor's work area. Decimals are allowed (e.g. 50.25) for precise placement, independent of where the visualizer itself sits. The strip can also be nudged a pixel at a time with the keyboard -- see Interaction, where it is move target 4. IMPORTANT -- once nudged, that saved position REPLACES this field, and editing this will appear to do nothing until you clear it with the move modifier + 4 then Home
    - verticalPosition: '95'
      $name: Vertical Position
      $description: 0-100, percentage down the monitor's work area. Decimals allowed. Same caveat as Horizontal Position -- a saved keyboard position takes priority over this field until it is cleared
    - anchor: screen
      $name: Anchor
      $description: Screen places the strip with the two Position percentages above. A panel corner pins it inside that corner of the visualizer's background panel instead, so it moves with the visualizer (drags included) and the Position settings are ignored. Give the panel enough padding on that side to hold it
      $options:
        - screen: Screen (Position settings)
        - panel_top_left: Panel, top left
        - panel_top_right: Panel, top right
        - panel_bottom_left: Panel, bottom left
        - panel_bottom_right: Panel, bottom right
    - anchorOffsetX: '8'
      $name: Anchor Inset X
      $description: Distance in from the panel's left or right edge, in pixels. Only used with a panel anchor. Decimals allowed; the media controls round to whole pixels
    - anchorOffsetY: '8'
      $name: Anchor Inset Y
      $description: Distance in from the panel's top or bottom edge, in pixels. Only used with a panel anchor. Decimals allowed; the media controls round to whole pixels
    - layout: strip
      $name: Layout
      $description: Strip is the three buttons. Card is a small media card in the same place, sized from Icon Size and Icon Spacing - the album art (hover it for previous / play / next), a seek bar (click or drag), a speaker button that switches the Windows default output device for all apps in one click, and a volume slider that sets the system master volume (drag it, or scroll anywhere on the card)
      $options:
        - strip: Strip (three buttons)
        - card: Card (art, seek, output, volume)
    - cardBackground: '#9E0A0A0D'
      $name: Card Background
      $description: 'Card only. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b). Kept just above fully transparent at the least, so the card always takes clicks'
    - cardBorderColor: '#00FFFFFF'
      $name: Card Border Color
      $description: 'Card only. Same formats as Card Background'
    - cardBorderSize: '0'
      $name: Card Border Size
      $description: Card only. Pixels, drawn inward from the edge. Decimals allowed; the media controls round to whole pixels
    - cardCornerRadius: '12'
      $name: Card Corner Radius
      $description: Card only. Pixels. The album art's corners follow it. Decimals allowed; the media controls round to whole pixels
    - cardArtSize: '0'
      $name: Card Art Size
      $description: Card only. Width of the album art in pixels, which sets the card's width. 0 sizes it from Icon Size and Icon Spacing. Decimals allowed; the media controls round to whole pixels
    - cardAccent: icon
      $name: Card Accent
      $description: Card only. Colour of the seek and volume fills and their knobs
      $options:
        - icon: Icon Color
        - custom: Card Accent Color
        - album: Album art
        - windows: Windows accent
    - cardAccentColor: '#FFFFFFFF'
      $name: Card Accent Color
      $description: 'Card only, with Card Accent = Card Accent Color. Same formats as Card Background'
  $name: Media Controls
- background:
    - enabled: true
      $name: Enabled
      $description: Draws a rounded panel behind the visualizer
    - color: '#60000000'
      $name: Color
      $description: 'Panel fill color. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
    - padding: '24'
      $name: Padding
      $description: Space between the bars/waveform and the panel edge, in pixels. One value for all sides, or four space-separated values for left, right, top, bottom -- so you can grow the panel more on one side without moving the bars or resizing anything else. Negative values shrink that side inward instead, past the bars if pushed far enough
    - cornerRadius: '14'
      $name: Corner Radius
      $description: Roundness of the panel corners, in pixels. One value for all corners, or four space-separated values for top-left, top-right, bottom-right, bottom-left
    - blur: '0'
      $name: Blur
      $description: Gaussian blur strength behind the panel, in pixels. 0 disables it. Computed once and cached, not redrawn every frame, so raising this has minimal ongoing CPU cost
    - borderSize: '0'
      $name: Border Size
      $description: Panel border thickness, in pixels. 0 disables it. Decimals allowed; turn Pixel Snap (Position) off to keep the fractions
    - borderColor: '#40FFFFFF'
      $name: Border Color
      $description: 'Panel border color. Format is #AARRGGBB, #RRGGBB, rgba(r, g, b, a), or rgb(r, g, b)'
  $name: Background
- performance:
    - targetFps: 60
      $name: Target FPS
      $description: Caps how often the visualizer redraws itself. 0 matches your display's refresh rate, whatever it is. With the Direct3D 11 renderer a frame where nothing moved is skipped entirely, so a high target costs far less than it used to during quiet or steady passages
    - pauseOnFullscreen: true
      $name: Pause On Fullscreen
      $description: Stops rendering while any window is fullscreen, since the visualizer would be hidden behind it anyway
    - pauseWhenSilentSeconds: '10'
      $name: Pause When Silent (seconds)
      $description: After this long without audio, drawing drops to a trickle that presents nothing unless something actually changes. 0 disables this behavior, and Deep Idle with it
    - deepIdle: true
      $name: Deep Idle
      $description: Five seconds after Pause When Silent kicks in, stops the audio stream itself and watches Windows' own peak meter four times a second instead, restarting the moment anything plays. A running capture stream registers an audio power request, which can keep the PC from sleeping; a stopped one doesn't. Turn off only if the visualizer is slow to wake for a very quiet source
    - perfStats: false
      $name: Performance Stats
      $description: Every 30 seconds, writes one line to the Windhawk log (turn on Enable logging) with what the visualizer actually cost - engine wakes and milliseconds, analyses and FFTs, render ticks, frames presented versus skipped, text redraws, compositor commits and buffer uploads, and time spent playing / trickling / in deep idle. All per second, so settings and PCs compare directly. Counting is always on and costs a few atomic adds per frame; this only switches the log line
    - autoHideEnabled: false
      $name: Auto-Hide When Idle
      $description: Fades the whole visualizer out after prolonged silence, instead of just idling
    - autoHideDelaySeconds: '15'
      $name: Auto-Hide Delay (seconds)
      $description: How long to wait after audio goes silent before fading the visualizer out. Only used when Auto-Hide When Idle is on
    - pauseWhenObscured: false
      $name: Pause When Covered
      $description: Stops rendering entirely while the visualizer is hidden behind another window, since nothing it draws would be visible anyway. How much of it has to be covered before that kicks in is set below
    - obscuredThresholdPercent: '100'
      $name: Pause When Covered - Threshold
      $description: How much of the visualizer's box (bars plus background padding) has to be underneath real application windows before rendering pauses. 100% means it only pauses when completely hidden, which is the safest setting and the old behavior. Lower values pause sooner and save more power, at the risk of stopping while a sliver of it is still peeking out. Coverage is measured as actual area across every covering window combined, not per-window, so two half-covering windows count as fully covered
      $options:
        - '5': 5% covered
        - '10': 10% covered
        - '15': 15% covered
        - '20': 20% covered
        - '25': 25% covered
        - '30': 30% covered
        - '35': 35% covered
        - '40': 40% covered
        - '45': 45% covered
        - '50': 50% covered
        - '55': 55% covered
        - '60': 60% covered
        - '65': 65% covered
        - '70': 70% covered
        - '75': 75% covered
        - '80': 80% covered
        - '85': 85% covered
        - '90': 90% covered
        - '95': 95% covered
        - '100': 100% covered (fully hidden)
  $name: Performance
- hardware:
    - workload: hybrid
      $name: Workload
      $description: Where the work runs. Hybrid analyses on the CPU, which is a fraction of a percent of one core, and draws on the GPU, the cheapest split on almost every PC. GPU moves the analysis onto the graphics chip too, so the CPU only copies audio. CPU does everything on the processor, drawing included (software rendering), leaving the GPU nothing but Windows' own compositing.
      $options:
        - hybrid: Hybrid - CPU analysis, GPU drawing (recommended)
        - gpu: GPU - analysis and drawing on the GPU
        - cpu: CPU - everything on the processor
    - renderDevice: auto
      $name: Drawing Device
      $description: Which GPU draws. Auto uses the one your chosen monitor is plugged into, which avoids copying every frame from one GPU to another and is the right answer almost everywhere. Integrated and Discrete force a GPU type, and fall back to Auto (with a warning) if this PC does not have one. CPU draws in software, the same as Workload = CPU
      $options:
        - auto: Auto (GPU driving the monitor)
        - integrated: Integrated GPU (lowest power)
        - discrete: Discrete GPU (highest performance)
        - cpu: CPU (software rendering)
    - renderer: d3d11
      $name: Renderer
      $description: Direct3D 11 draws everything with one tiny shader from a buffer of bar heights, skips any frame where nothing moved, keeps the text on its own surface that only redraws when the text changes, and tells Windows when the panel is opaque so it copies instead of blending. Direct2D is the 1.5 path, unchanged, including Smooth Mode's batching; it is kept for comparison and as the automatic fallback
      $options:
        - d3d11: Direct3D 11 (recommended)
        - direct2d: Direct2D (1.5 path)
    - opaquePanel: auto
      $name: Opaque Panel
      $description: When the background panel can't show anything behind it (Blur on, or a fully opaque colour) and Auto-Hide is off, the Direct3D 11 renderer marks it opaque so Windows composes it without blending, with the rounded corners cut by the compositor. Off always blends, for comparison
      $options:
        - auto: Auto
        - 'off': 'Off'
    - smoothMode: auto
      $name: Smooth Mode
      $description: Frame timing locked to the display's refresh so every frame lands on its own refresh, and motion that scales with real elapsed time. Target FPS is rounded to an even fraction of the refresh rate (60 on a 144 Hz screen runs at 72) because that is what makes motion even. With the Direct2D renderer it also bakes the panel and batches the bars. Auto turns it on when drawing runs on an integrated GPU or the CPU. On works with any GPU. Off is the 1.4 timing
      $options:
        - auto: Auto (integrated GPU or CPU drawing)
        - 'on': 'On'
        - 'off': Off (1.4 timing)
  $name: Hardware
- validation:
    - showErrors: true
      $name: Warn About Invalid Settings
      $description: Several settings here are free-text fields -- colors, positions, padding, corner radii, icon paths. Windhawk will happily save a typo in one of them, and the mod then quietly falls back to a default, so the setting looks saved but does nothing. With this on, a summary window appears listing exactly which fields couldn't be read, what you typed, and what format was expected. Turn it off if you'd rather it stayed silent -- the same list still goes to the Windhawk mod log either way
  $name: Settings Validation
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <d2d1_1.h>
// d2d1_3 for ID2D1DeviceContext3 / ID2D1SpriteBatch (Smooth Mode's batched
// bars), dxgi1_6 for EnumAdapterByGpuPreference (Drawing Device). Both are
// only used after a runtime QueryInterface, so older Windows still loads.
#include <d2d1_3.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_3.h>
#include <dxgi1_6.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <wrl/client.h>
#include <wincodec.h>
#include <dwrite.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <compressapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <mutex>
#include <map>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
using namespace winrt::Windows::Media::Control;
using namespace winrt::Windows::Storage::Streams;

using Microsoft::WRL::ComPtr;

// ---- Shared setups: the layer an imported setup sits in -------------------------------------
// Every setting is read through here. An imported setup's values (the mod value "sharedSetup", as
// key=value lines) come first, then the settings page; quick settings still go on top of both.
// Reset Quick Settings clears it, and the settings page's values come back.
std::map<std::wstring, std::wstring> g_setupLayer;
bool g_setupLayerLoaded = false;

void VizSetupLoad() {
    g_setupLayerLoaded = true;
    g_setupLayer.clear();
    std::vector<WCHAR> buf(32768);
    if (!Wh_GetStringValue(L"sharedSetup", buf.data(), buf.size())) return;
    std::wstring s = buf.data();
    for (size_t pos = 0; pos < s.size();) {
        size_t nl = s.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = s.size();
        std::wstring line = s.substr(pos, nl - pos);
        size_t eq = line.find(L'=');
        if (eq != std::wstring::npos && eq > 0) g_setupLayer[line.substr(0, eq)] = line.substr(eq + 1);
        pos = nl + 1;
    }
}

void VizSetupSave() {
    std::wstring s;
    for (const auto& kv : g_setupLayer) s += kv.first + L"=" + kv.second + L"\n";
    Wh_SetStringValue(L"sharedSetup", s.c_str());
}

const std::wstring* VizSetupFind(PCWSTR name) {
    if (!g_setupLayerLoaded) VizSetupLoad();
    auto it = g_setupLayer.find(name);
    return it == g_setupLayer.end() ? nullptr : &it->second;
}

int VizSettingInt(PCWSTR name) {
    if (const std::wstring* v = VizSetupFind(name)) return *v == L"true" ? 1 : *v == L"false" ? 0 : _wtoi(v->c_str());
    return Wh_GetIntSetting(name);
}

PCWSTR VizSettingString(PCWSTR name) {
    std::wstring v;
    if (const std::wstring* found = VizSetupFind(name)) {
        v = *found;
    } else if (PCWSTR s = Wh_GetStringSetting(name)) {
        v = s;
        Wh_FreeStringSetting(s);
    }
    WCHAR* out = new WCHAR[v.size() + 1];
    wcscpy_s(out, v.size() + 1, v.c_str());
    return out;
}

void VizSettingFree(PCWSTR s) { delete[] s; }

// From here on, settings come through the layer: call VizSetting* instead of Wh_Get*Setting.

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif

#define TIMER_ID_MSG_DISPLAY_CHANGE 1
#define TIMER_ID_MSG_RECREATE_OVERLAY 2
#define TIMER_ID_MSG_WALLPAPER_REFRESH 3
#define TIMER_ID_MSG_FULLSCREEN_WATCH 4
#define TIMER_ID_MSG_SAVE_POSITION 5
#define TIMER_ID_MSG_FONT_RECHECK 6
#define TIMER_ID_MSG_REBUILD_DEVICE 7

#define WM_APP_CLEANUP (WM_APP + 1)
#define WM_APP_SETTINGS_CHANGED (WM_APP + 2)
#define WM_APP_RENDER_TICK (WM_APP + 3)
#define WM_APP_MEDIA_REPAINT (WM_APP + 4)
#define WM_APP_REQUEST_SAVE_POSITION (WM_APP + 5)
#define WM_APP_FORCE_REDRAW (WM_APP + 6)
// Hardware section. A worker thread or the render path (device
// loss) can't touch the settings-issue list or rebuild D3D objects itself, so
// both hand the job to the message window, which lives on the UI thread.
#define WM_APP_HW_NOTICE (WM_APP + 7)        // lParam: heap std::wstring*, owned by receiver
#define WM_APP_REBUILD_DEVICE (WM_APP + 8)   // wParam: 1 = device was lost
#define WM_APP_CONTEXT_MENU (WM_APP + 9)     // wParam, lParam: screen x, y
#define WM_APP_PANEL_FIT (WM_APP + 10)       // Place In = Panel: size the window to the box
#define WM_APP_SEEK_AT (WM_APP + 12)         // Click to Seek; wParam: screen x
#define OVERLAY_WINDOW_CLASS (L"DesktopAudioVisOverlay_" WH_MOD_ID)
#define MESSAGE_WINDOW_CLASS (L"DesktopAudioVisMessage_" WH_MOD_ID)
#define MEDIA_WINDOW_CLASS (L"DesktopAudioVisMedia_" WH_MOD_ID)

enum class VizShape { Stereo, Mountain, Mirror, Wave, Breathe, Dots, Radial, Oscilloscope, Goniometer, Terminal };
enum class VizColorMode { Solid, Gradient, ReactiveGradient, Accent, AlbumArt, DynamicAlbum, Acrylic, RainbowCycle, Tourne };
enum class VizEQ { Default, Bass, Rock, Pop, Jazz, Electronic };
enum class VizSensitivityCurve { Exponential, Knee, Power };
enum class VizOrientation { Horizontal, Vertical };
enum class VizAnchor { Top, Middle, Bottom };
enum class VizFreqScale { Log, Linear, Mel, Bark, Erb };
enum class VizTextAlignH { Left, Center, Right };
enum class VizTextAlignV { Above, Top, Middle, Bottom, Below };
enum class VizDragModifier { None, Ctrl, Alt, Shift, Win };
enum class VizDragButton { Left, Middle, Right };
// Bit flags rather than an enum of named combos, so "is every key in the combo
// currently down" is one mask test and "does the fast key overlap the modifier"
// is one AND -- which is what the settings validator checks for.
enum VizModKeyFlags : unsigned {
    VIZ_MOD_NONE  = 0,
    VIZ_MOD_CTRL  = 1u << 0,
    VIZ_MOD_ALT   = 1u << 1,
    VIZ_MOD_SHIFT = 1u << 2,
    VIZ_MOD_WIN   = 1u << 3,
};
enum class VizKeyMoveKeys { Both, Arrows, Wasd };
// What the keyboard-move keys are currently steering. Switched live with the
// move modifier + 1/2/3, so the same combo places all three pieces without
// needing a separate shortcut per piece.
enum class VizMoveTarget { Visualizer, NowPlaying, PeakFreq, MediaControls };
// Hardware section.
enum class VizRenderDevice { Auto, Integrated, Discrete, Cpu };
enum class VizSmoothMode { Auto, On, Off };
// 2.0. Where the work runs; which analysis core; which renderer.
enum class VizWorkload { Hybrid, Gpu, Cpu };
enum class VizRenderer { D3D11, Direct2D };
enum class VizOpaquePanel { Auto, Off };
enum class VizEngineKind { Precision, Classic };
enum class VizBandLayout { Scale, Iec, Musical };
enum class VizWeighting { Z, A, C };
enum class VizDetector { Rms, Peak };
enum class VizLevelRef { ThirdOctave, Band };
enum class VizWindowKind { Hann, Hamming, BlackmanHarris, FlatTop };
enum class VizChannel { Mix, Left, Right, Mid, Side };
enum class VizBallisticsPreset { Snappy, Smooth, Analyzer, Vu, PpmEbu, PpmDin, Custom };
enum class VizPeakFall { Gravity, Linear };
enum class VizReadout { Frequency, Loudness, LoudnessFull, Both };
enum class VizTermStyle { Columns, Waterfall, Meters };
enum class VizNpLayout { OneLine, TwoLines };
enum class VizNpPlacement { Above, PanelTop, PanelBottom };
enum class VizProgressPlacement { Below, Above, PanelBottom };
enum class VizMediaAnchor { Screen, PanelTopLeft, PanelTopRight, PanelBottomLeft, PanelBottomRight };
enum class VizContextMenu { RightClick, CtrlRightClick, Off };
enum class VizStyle { None, Led, Line, Bloom, Spectrogram, Vu, SplitLR, Particles };

struct Settings {
    VizShape shape = VizShape::Stereo;
    VizOrientation orientation = VizOrientation::Horizontal;
    int barCount = 32;
    float barWidth = 6;
    float barGap = 4;
    float barMaxSize = 140;
    float barIdleSize = 4;
    float barRadiusTL = 3.0f, barRadiusTR = 3.0f, barRadiusBR = 3.0f, barRadiusBL = 3.0f;
    VizColorMode colorMode = VizColorMode::Solid;
    BYTE colorA = 255, colorR = 255, colorG = 255, colorB = 255;
    BYTE grad1A = 255, grad1R = 30, grad1G = 215, grad1B = 96;
    BYTE grad2A = 255, grad2R = 0, grad2G = 180, grad2B = 255;
    float sensitivity = 150;
    VizSensitivityCurve sensitivityCurve = VizSensitivityCurve::Knee;
    float inputGainDb = 0.0f;
    bool autoGain = false;
    float autoGainMaxDb = 12.0f;
    float smoothing = 0;
    VizEQ eq = VizEQ::Default;

    float horizontalPosition = 50.0f;
    float verticalPosition = 88.0f;
    int monitor = 1;
    VizAnchor verticalAnchor = VizAnchor::Bottom;

    bool keyMoveEnabled = false;
    unsigned keyMoveModifier = VIZ_MOD_CTRL | VIZ_MOD_ALT;
    VizKeyMoveKeys keyMoveKeys = VizKeyMoveKeys::Both;
    float keyMoveStep = 1;
    float keyMoveFastStep = 10;
    unsigned keyMoveFastKey = VIZ_MOD_SHIFT;
    float keyMoveFineStep = 0.25f;           // px, for subpixel nudges
    unsigned keyMoveFineKey = VIZ_MOD_NONE;  // held for a fine step
    bool keyMoveFine = false;                // every nudge fine (right-click menu)
    bool pixelSnap = true;

    bool dragEnabled = false;
    bool hostTaskbar = false;         // Position > Place In = Taskbar
    bool hostPanel = false;           // Position > Place In = Panel
    int panelBackdrop = 1;            // 0 clear, 1 acrylic, 2 mica
    std::wstring panelHotkey;
    bool splitEnabled = false;        // show the visualizer as two pieces
    float splitAt = 50.f;             // % of the bars in the first piece
    float splitGap = 0.f, splitShift = 0.f;  // logical px: along, and across
    std::wstring dockApp;             // lower case, without .exe; empty = not docked
    int dockSide = 0;                 // 0 below, 1 above, 2 left, 3 right
    int dockAlign = 1;                // 0 start, 1 centre, 2 end
    float dockGap = 0.f, dockShift = 0.f;  // logical px
    bool dragSnap = true;             // snap to screen and taskbar lines while dragging
    float dragSnapDistance = 8.f;     // logical px
    VizDragModifier dragModifier = VizDragModifier::Ctrl;
    VizDragButton dragButton = VizDragButton::Middle;

    bool showSettingsErrors = true;

    bool backgroundEnabled = true;
    BYTE bgA = 0x60, bgR = 0, bgG = 0, bgB = 0;
    float bgPaddingL = 24, bgPaddingR = 24, bgPaddingT = 24, bgPaddingB = 24;
    float bgRadiusTL = 14.0f, bgRadiusTR = 14.0f, bgRadiusBR = 14.0f, bgRadiusBL = 14.0f;
    float bgBlur = 0;
    float bgBorderSize = 0;
    BYTE borderA = 0x40, borderR = 255, borderG = 255, borderB = 255;

    int targetFps = 60;
    bool pauseOnFullscreen = true;
    float pauseWhenSilentSeconds = 10;

    bool peakHoldEnabled = false;
    BYTE peakHoldA = 255, peakHoldR = 0, peakHoldG = 180, peakHoldB = 255;
    bool beatFlashEnabled = false;
    BYTE beatFlashA = 255, beatFlashR = 255, beatFlashG = 255, beatFlashB = 255;
    float beatFlashIntensity = 80;
    float rainbowSpeed = 40;

    bool nowPlayingEnabled = false;
    BYTE nowPlayingA = 255, nowPlayingR = 255, nowPlayingG = 255, nowPlayingB = 255;
    std::wstring nowPlayingFont = L"Segoe UI";
    float nowPlayingFontSize = 16;
    float nowPlayingDisplaySeconds = 6;
    float nowPlayingOffsetY = 0;
    float nowPlayingOffsetX = 0;
    float peakFreqOffsetY = 0;
    float peakFreqOffsetX = 0;

    // Per-overlay text panels. Alpha 0 on the fill means "no panel"; the border
    // is independent, so an outline with nothing behind it is a valid look.
    BYTE npBgA = 0, npBgR = 0, npBgG = 0, npBgB = 0;
    float npBgBorderSize = 0;
    float npBgCornerRadius = 6;
    float npBgPadding = 6;
    BYTE npBgBorderA = 0x40, npBgBorderR = 255, npBgBorderG = 255, npBgBorderB = 255;

    BYTE pfBgA = 0, pfBgR = 0, pfBgG = 0, pfBgB = 0;
    float pfBgBorderSize = 0;
    float pfBgCornerRadius = 6;
    float pfBgPadding = 6;
    BYTE pfBgBorderA = 0x40, pfBgBorderR = 255, pfBgBorderG = 255, pfBgBorderB = 255;

    bool autoHideEnabled = false;
    float autoHideDelaySeconds = 15;
    bool pauseWhenObscured = false;
    int obscuredThresholdPercent = 100;

    int fftSize = 1024;
    VizFreqScale freqScale = VizFreqScale::Log;
    bool peakFreqEnabled = false;
    VizTextAlignH peakFreqAlignH = VizTextAlignH::Right;
    VizTextAlignV peakFreqAlignV = VizTextAlignV::Top;
    bool oscilloscopeMultibandEnabled = false;
    float oscilloscopeWindowMs = 24;
    float oscilloscopeDamping = 0;

    bool mediaControlsEnabled = false;
    BYTE mediaIconColorA = 255, mediaIconColorR = 255, mediaIconColorG = 255, mediaIconColorB = 255;
    std::wstring mediaIconPrevPath;
    std::wstring mediaIconPlayPath;
    std::wstring mediaIconPausePath;
    std::wstring mediaIconNextPath;
    float mediaIconSize = 32;
    float mediaIconSpacing = 14;
    BYTE mediaPlateA = 0, mediaPlateR = 0, mediaPlateG = 0, mediaPlateB = 0;
    float mediaPlatePadding = 0;
    bool mediaCard = false;  // Media Controls > Layout = Card (2.1)
    BYTE cardBgA = 158, cardBgR = 10, cardBgG = 10, cardBgB = 13;
    BYTE cardBorderA = 0, cardBorderR = 255, cardBorderG = 255, cardBorderB = 255;
    float cardArtSize = 0;
    float cardRadius = 12;
    float cardBorderSize = 0;
    int cardAccentSource = 0;  // 0 icon colour, 1 custom, 2 album art, 3 Windows accent
    BYTE cardAccentA = 255, cardAccentR = 255, cardAccentG = 255, cardAccentB = 255;
    float mediaPlateCornerRadius = 8;
    float mediaPlateBorderSize = 0;
    BYTE mediaPlateBorderA = 0x40, mediaPlateBorderR = 255, mediaPlateBorderG = 255,
         mediaPlateBorderB = 255;
    bool mediaHideWhenCovered = false;
    bool mediaOnTop = true;           // the strip / card stays above other windows
    int mediaCoveredThresholdPercent = 50;
    float mediaHorizontalPosition = 50.0f;
    float mediaVerticalPosition = 95.0f;

    VizRenderDevice renderDevice = VizRenderDevice::Auto;
    VizSmoothMode smoothMode = VizSmoothMode::Auto;
    VizWorkload workload = VizWorkload::Hybrid;
    VizRenderer renderer = VizRenderer::D3D11;
    VizOpaquePanel opaquePanel = VizOpaquePanel::Auto;
    bool deepIdle = true;

    // Analysis (2.0).
    VizEngineKind engine = VizEngineKind::Precision;
    VizBandLayout bandLayout = VizBandLayout::Scale;
    int octaveFraction = 6;
    float minFreq = 20, maxFreq = 20000;
    float tuningA4 = 440.f;
    VizWeighting weighting = VizWeighting::Z;
    float tiltDbPerOct = 1.5f;
    VizDetector detector = VizDetector::Rms;
    VizLevelRef levelRef = VizLevelRef::ThirdOctave;
    VizWindowKind window = VizWindowKind::Hann;
    int bassDetail = 2;
    VizChannel channel = VizChannel::Mix;
    float dbFloor = -72, dbCeiling = -12;
    VizBallisticsPreset ballistics = VizBallisticsPreset::Snappy;
    float attackMs = 10, releaseDbPerSec = 20;
    float peakHoldMs = 500;
    VizPeakFall peakFall = VizPeakFall::Gravity;
    VizReadout readout = VizReadout::Frequency;
    bool loudnessResetOnTrack = true;
    int layoutBandCount = 0;  // bars implied by an IEC / musical layout, 0 = use Bar Count

    // Audio source (2.0): "" / default_output, default_input, id:<endpoint>, name:<text>.
    std::wstring audioSourceKey;

    // Styles (2.1): picked from the Shape list on top of an internal shape.
    VizStyle style = VizStyle::None;
    float reflection = 0;  // %, of Bar Max Size
    float fxGlow = 0, fxBloom = 0;
    float fxBloomRadius = 16;
    float fxGlowRadius = 6;  // FX (2.1)
    float fxOutlineWidth = 0.f;  // px
    BYTE fxOutlineA = 0xB0, fxOutlineR = 255, fxOutlineG = 255, fxOutlineB = 255;
    float fxShadow = 0;  // %
    BYTE fxShadowR = 0, fxShadowG = 0, fxShadowB = 0;
    float fxShadowX = 2.f, fxShadowY = 3.f, fxShadowSoft = 4.f;  // px
    bool barHollow = false;
    float barHollowWidth = 1.5f, barDash = 0.f, barDashGap = 2.f, barTilt = 0.f;  // px, px, px, degrees
    float barMirrorGap = 0.f;  // px
    float afterimage = 0;  // %
    float afterimageSeconds = 1.2f;

    // Media widget (2.0).
    VizNpLayout npLayout = VizNpLayout::OneLine;
    VizNpPlacement npPlacement = VizNpPlacement::Above;
    VizTextAlignH npAlign = VizTextAlignH::Center;
    BYTE npArtistA = 0xB3, npArtistR = 255, npArtistG = 255, npArtistB = 255;
    bool textPixel = false;
    bool progressEnabled = false;
    bool progressSeek = false;        // Click to Seek
    bool progressSeekOnlyListed = false;  // the app list includes rather than excludes
    std::vector<std::wstring> progressSeekApps;  // lower case, without .exe
    VizProgressPlacement progressPlacement = VizProgressPlacement::Below;
    float progressGap = 6;
    float progressHeight = 2;
    BYTE progressA = 255, progressR = 255, progressG = 255, progressB = 255;
    BYTE progressTrackA = 0x40, progressTrackR = 255, progressTrackG = 255, progressTrackB = 255;
    VizMediaAnchor mediaAnchor = VizMediaAnchor::Screen;
    float mediaAnchorOffsetY = 8;
    float mediaAnchorOffsetX = 8;
    VizContextMenu contextMenu = VizContextMenu::RightClick;

    // Terminal shape (2.0).
    VizTermStyle termStyle = VizTermStyle::Columns;
    std::wstring termFont = L"Consolas";
    float termFontSize = 14;
    int termRows = 16;
    int termMeterColumns = 40;
    float termHotThreshold = 75;
    wchar_t termColumnGlyph = L'#';
    wchar_t termPeakGlyph = L'-';
    std::wstring termRamp = L" .:-=+*#%@";
    float termScrollRate = 20;
    BYTE termDimA = 255, termDimR = 0x1E, termDimG = 0x6B, termDimB = 0x34;
    BYTE termLowA = 255, termLowR = 0x33, termLowG = 0xFF, termLowB = 0x66;
    BYTE termHighA = 255, termHighR = 0xFF, termHighG = 0x3B, termHighB = 0x3B;
    BYTE termLabelA = 255, termLabelR = 0xB8, termLabelG = 0xFF, termLabelB = 0xB8;
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
HWND g_mediaWnd;
std::atomic<bool> g_mediaIsPlaying{false};

// Runtime position override for the media strip, same shape as the one the
// visualizer uses: percentages of the work area, replacing the Media Controls
// Position settings once a keyboard nudge has moved it. Declared up here rather
// than with the rest of the move code because RepositionAndRepaintMediaControls
// -- which is defined well before that -- has to read it.
std::atomic<bool> g_mediaOverrideActive{false};
std::atomic<float> g_mediaOverrideH{50.0f};
std::atomic<float> g_mediaOverrideV{95.0f};

std::atomic<HWND> g_overlayWnd{nullptr};
// Place In = Panel: whether the current overlay window is the panel,
// and the panel's own state (kept in the "panel" mod value).
bool g_overlayInPanel = false;
// RegisterWindowMessage("TourneTable.Panel"): wParam 0 toggles the panel, 1 shows, 2 hides, 3 toggles
// Keep on Top. Tourne Desktop sends it from its taskbar; any other launcher or script can too.
UINT g_panelMsg = 0;
bool g_panelHidden = false;
bool g_panelPinned = false;
float g_panelZoom = 1.0f;
RECT g_panelRect{0, 0, 0, 0};  // empty until placed once
int g_panelSizingW = 0, g_panelSizingH = 0;  // non-zero while an edge is being dragged
ComPtr<IDXGISwapChain1> g_swapChain;
ComPtr<ID2D1DeviceContext> g_dc;
ComPtr<IDCompositionDevice> g_compositionDevice;
ComPtr<IDCompositionTarget> g_compositionTarget;
ComPtr<IDCompositionVisual> g_compositionVisual;
// Since 2.0 the target's root is an empty container with the text surface
// (g_compositionVisual) as a child, so the Direct3D 11 renderer can slide its
// panel surface in underneath it.
ComPtr<IDCompositionVisual> g_rootVisual;
// Split into two pieces (2.1): the composition target's real root. It holds
// g_rootVisual (the first piece, clipped at the cut) and, while splitting, a
// second container showing the same swap chains again, clipped to the other
// side and offset. DWM does the moving; nothing is drawn twice.
ComPtr<IDCompositionVisual> g_splitTop;
struct VizSplitState {
    ComPtr<IDCompositionVisual> p2, text2, gfx2;
    ComPtr<IDCompositionRectangleClip> c1, c2;
    bool built = false, on = false, gfxIn = false, gClip = false, vert = false;
    float line = 0.f, dx = 0.f, dy = 0.f, tx = 0.f, ty = 0.f, gx = 0.f, gy = 0.f;
    IUnknown* textC = nullptr;
    IUnknown* gfxC = nullptr;
} g_split;
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
float g_borderCacheBorderSize = -1.f;

ComPtr<ID2D1StrokeStyle> g_roundCapStrokeStyle;
ComPtr<IDWriteFactory> g_dwriteFactory;
ComPtr<IDWriteTextFormat> g_dwriteTextFormat;
ComPtr<ID2D1SolidColorBrush> g_nowPlayingBrush;
ComPtr<ID2D1SolidColorBrush> g_npArtistBrush;
ComPtr<ID2D1SolidColorBrush> g_progressBrush;
ComPtr<ID2D1SolidColorBrush> g_textPanelBrush;
float g_dwriteTextFormatFontSize = -1.f;
std::wstring g_dwriteTextFormatFontName;

static const IID kCLSID_D2D1GaussianBlur = {
    0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

// ---- Hardware state ----------------------------------------------------------
// What the current D3D device actually landed on, which is not always what
// was asked for: Integrated on a desktop with no iGPU falls back, and a failed
// hardware device falls back to WARP. Smooth Mode's Auto keys off this rather
// than off the setting, so it follows the hardware that is really drawing.
struct VizRenderAdapterInfo {
    bool valid = false;
    bool warp = false;        // software rasterizer (CPU drawing)
    bool integrated = false;  // shares system memory with the CPU
    LUID luid = {};
    std::wstring name;
};
VizRenderAdapterInfo g_renderAdapter;

// Resolved Smooth Mode. Read by the render thread for pacing and by the UI
// thread for drawing, so it is an atomic rather than a settings field.
std::atomic<bool> g_smoothActive{false};

// Device-loss recovery is rate-limited so a driver that keeps failing can't
// put the UI thread into a rebuild loop.
std::atomic<bool> g_deviceRebuildQueued{false};
ULONGLONG g_lastDeviceRebuildTick = 0;

// ---- Smooth Mode resources ---------------------------------------------------
// All device-dependent, all released with the other visual resources.
//
// The background plate is the blurred wallpaper slice, the panel fill and the
// border ring rendered once into a bitmap. The 1.4 path re-does all three
// every frame, and the blur goes through a layer with a geometric mask, which
// on an integrated GPU means an offscreen allocation plus extra full-surface
// passes per frame for pixels that never change.
ComPtr<ID2D1Bitmap1> g_plateBitmap;
D2D1_RECT_F g_plateRect = {0.f, 0.f, 0.f, 0.f};

// Batched bars. One small atlas holds a pre-antialiased cap/body/cap strip of
// the bar shape; each frame becomes a single DrawSpriteBatch call, instead of
// one FillRoundedRectangle per bar (or, with per-corner radii, one brand new
// path geometry per bar, per frame).
ComPtr<ID2D1DeviceContext3> g_dc3;
ComPtr<ID2D1SpriteBatch> g_spriteBatch;
ComPtr<ID2D1Bitmap1> g_spriteAtlas;
struct VizAtlasKey {
    int kind = -1;  // 0 = bars, 1 = dots
    bool horizontal = true;
    float thick = 0.f, rTL = 0.f, rTR = 0.f, rBR = 0.f, rBL = 0.f, frac = 0.f;
    bool operator==(const VizAtlasKey& o) const {
        return kind == o.kind && horizontal == o.horizontal && thick == o.thick &&
               rTL == o.rTL && rTR == o.rTR && rBR == o.rBR && rBL == o.rBL && frac == o.frac;
    }
};
VizAtlasKey g_spriteAtlasKey;
// Atlas geometry, in atlas pixels. capA is the leading cap (top for vertical
// bars, left for horizontal ones), capB the trailing cap.
struct VizAtlasLayout {
    UINT pad = 1, capA = 0, mid = 3, capB = 0, across = 0;  // across = thickness incl. padding
    UINT along = 0;                                         // pad + capA + mid + capB + pad
} g_spriteAtlasLayout;
std::vector<D2D1_RECT_F> g_spriteDst;
std::vector<D2D1_RECT_U> g_spriteSrc;
std::vector<D2D1_COLOR_F> g_spriteCol;

// Text layouts, cached per overlay. DrawText builds a fresh layout (font
// fallback, shaping, line breaking) on every call; the strings here change a
// few times a minute at most, so Smooth Mode keeps the last one.
struct VizTextLayoutCache {
    std::wstring text;
    float w = -1.f, h = -1.f;
    ComPtr<IDWriteTextLayout> layout;
    void Reset() { text.clear(); w = h = -1.f; layout.Reset(); }
};
VizTextLayoutCache g_npLayoutCache, g_pfLayoutCache;

// Per-frame motion scale for Smooth Mode: 1.0 when the frame arrived exactly
// one Target-FPS interval after the last, larger when it was late. Written and
// read on the UI thread only.
float g_frameScale = 1.0f;

inline LONGLONG VizQpcFreq() {
    static LONGLONG s_freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f.QuadPart;
    }();
    return s_freq;
}

// Animation clock for Smooth Mode: seconds since the first call, from QPC.
//
// The 1.4 path drives Wave, Breathe and Rainbow from
// (float)GetTickCount64() * 0.001f. That has two problems, and both show up as
// stepping rather than motion. GetTickCount64 only moves every 15.6 ms, so at
// 144 FPS two frames in three repeat the previous animation phase. And a float
// holding a millisecond uptime runs out of mantissa: after a day of uptime the
// value can only change in 8 ms steps, after ten days in 64 ms steps, so the
// longer the PC has been on, the choppier those shapes get. This clock is QPC
// in a double, and callers reduce phase * time modulo one cycle while still in
// double (VizClockPhase) before narrowing, so it stays exact however long the
// mod has been running.
inline double VizClockSeconds() {
    static LONGLONG s_start = [] {
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        return q.QuadPart;
    }();
    LARGE_INTEGER q;
    QueryPerformanceCounter(&q);
    return (double)(q.QuadPart - s_start) / (double)VizQpcFreq();
}

// clock * rate, reduced modulo `cycle` in double precision.
inline float VizClockPhase(double clock, double rate, double cycle) {
    return (float)fmod(clock * rate, cycle);
}

// Converts a per-frame easing factor tuned at the Target FPS into the one
// that covers the time that actually passed. At g_frameScale == 1 this is the
// identity, so a frame that arrives on time moves exactly as in 1.4; a late
// frame catches up instead of visibly slowing the motion down.
inline float VizEaseForFrame(float perFrame) {
    if (g_frameScale == 1.0f) return perFrame;
    perFrame = std::clamp(perFrame, 0.0f, 1.0f);
    return 1.0f - powf(1.0f - perFrame, g_frameScale);
}

float g_dpiScale = 1.0f;

// A size in settings pixels, DPI-scaled, and with Pixel Snap on rounded to a
// whole device pixel so edges land on the pixel grid.
inline float VizPx(float v) {
    float p = v * g_dpiScale;
    return g_settings.pixelSnap ? roundf(p) : p;
}
FILETIME g_lastWallpaperTime = {};
std::atomic<HMONITOR> g_cachedMonitor{nullptr};

std::atomic<bool> g_deviceChanged{false};

// Per-band energy and oscilloscope waveform are each produced as a whole array
// by the capture thread and consumed as a whole array by the render thread.
// A plain std::atomic per element only guarantees each float is torn-free on
// its own; the render thread could still read some elements from one capture
// iteration and the rest from the next, visible as a tiny inconsistency in
// the drawn trace at high refresh rates. A seqlock makes the entire array
// publish/read atomic as a unit instead, with no lock contention.
std::atomic<uint32_t> g_bandsSeq{0};
float g_bandsData[VIZ_NUM_BANDS] = {};

void PublishBands(const float (&src)[VIZ_NUM_BANDS]) {
    uint32_t seq = g_bandsSeq.load(std::memory_order_relaxed);
    g_bandsSeq.store(seq + 1, std::memory_order_release);
    memcpy(g_bandsData, src, sizeof(g_bandsData));
    g_bandsSeq.store(seq + 2, std::memory_order_release);
}

void ReadBands(float (&dst)[VIZ_NUM_BANDS]) {
    for (;;) {
        uint32_t seq1 = g_bandsSeq.load(std::memory_order_acquire);
        if (seq1 & 1) continue;
        memcpy(dst, g_bandsData, sizeof(dst));
        std::atomic_thread_fence(std::memory_order_acquire);
        if (seq1 == g_bandsSeq.load(std::memory_order_relaxed)) return;
    }
}

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

std::atomic<uint32_t> g_waveformSeq{0};
float g_waveformData[VIZ_WAVE_SAMPLES] = {};

void PublishWaveform(const float (&src)[VIZ_WAVE_SAMPLES]) {
    uint32_t seq = g_waveformSeq.load(std::memory_order_relaxed);
    g_waveformSeq.store(seq + 1, std::memory_order_release);
    memcpy(g_waveformData, src, sizeof(g_waveformData));
    g_waveformSeq.store(seq + 2, std::memory_order_release);
}

void ReadWaveform(float (&dst)[VIZ_WAVE_SAMPLES]) {
    for (;;) {
        uint32_t seq1 = g_waveformSeq.load(std::memory_order_acquire);
        if (seq1 & 1) continue;
        memcpy(dst, g_waveformData, sizeof(dst));
        std::atomic_thread_fence(std::memory_order_acquire);
        if (seq1 == g_waveformSeq.load(std::memory_order_relaxed)) return;
    }
}

std::atomic<float> g_beatPulse{0.f};
std::atomic<float> g_dominantFreqHz{0.f};

// Screen-space bounds of the last drawn frame, used by the occlusion check so
// it can test the region we actually occupy rather than the whole monitor.
std::atomic<LONG> g_drawRectL{0}, g_drawRectT{0}, g_drawRectR{0}, g_drawRectB{0};
// Dock to an app window: the window's visible frame in screen pixels, written
// on the UI thread and read by layout on the render thread.
std::atomic<bool> g_dockActive{false};
std::atomic<LONG> g_dockL{0}, g_dockT{0}, g_dockR{0}, g_dockB{0};
void VizDockPlace(float boxW, float boxH, float* x, float* y);
// The progress bar's hit area for Click to Seek, in screen pixels (render thread
// writes, the input hook reads).
std::atomic<LONG> g_seekRectL{0}, g_seekRectT{0}, g_seekRectR{0}, g_seekRectB{0};
std::atomic<bool> g_seekRectValid{false};
std::atomic<bool> g_drawRectValid{false};
// Set once the auto-hide fade has reached full transparency and a single blank
// frame has been presented. While set, the render path exits immediately.
bool g_autoHideBlanked = false;
// For other threads (the right-click hook): true while Auto-Hide has the scene
// at zero alpha, so nothing is visible inside the still-valid draw rect. Set by
// the render thread every frame, from the same sceneAlpha the renderers use.
std::atomic<bool> g_vizSceneHidden{false};
// True while the right-click menu's TrackPopupMenuEx loop runs (message-window
// thread); the hook passes every click through while it is set.
std::atomic<bool> g_menuOpen{false};

// Current swap chain dimensions and composition-visual offset. The swap chain
// covers only the widget's bounding box rather than the whole desktop, so these
// are tracked to detect when a settings/display change requires a resize.
UINT  g_swapChainWidth = 0, g_swapChainHeight = 0;
float g_visualOffsetX = 0.f, g_visualOffsetY = 0.f;

std::mutex g_nowPlayingMutex;
std::wstring g_nowPlayingDisplay;
std::wstring g_nowPlayingTitle, g_nowPlayingArtist;  // the parts, for the two-line layout

// ---- track timeline: begin
// Track timeline from the media session, for the progress bar: start, end and
// position in 100 ns units, and the tick at which the position was current.
// Between updates the bar extrapolates while playing, since most players only
// report the position on a seek or a state change.
//
// Writers are WinRT thread-pool callbacks (two can run at once), serialised by
// g_tlWriteMutex; each write is tagged with the session generation it was read
// for, so a late event from a session that has since been replaced is dropped.
// The render thread reads without locking, through the g_tlSeq sequence count
// (odd while a write is in progress), so it never sees a position paired with
// another update's tick.
std::atomic<bool> g_tlValid{false};
std::atomic<int64_t> g_tlStart{0}, g_tlEnd{0}, g_tlPos{0};
std::atomic<ULONGLONG> g_tlTick{0};
std::atomic<bool> g_tlRunning{false};  // extrapolating: the session reported Playing
std::atomic<uint32_t> g_tlSeq{0};
std::atomic<uint32_t> g_tlGen{0};
std::mutex g_tlWriteMutex;

static void VizTlWriteBegin() {
    g_tlSeq.store(g_tlSeq.load(std::memory_order_relaxed) + 1, std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_release);
}
static void VizTlWriteEnd() {
    g_tlSeq.store(g_tlSeq.load(std::memory_order_relaxed) + 1, std::memory_order_release);
}

// A new media session is being hooked up: forget the old timeline and return
// the generation the new session's events must carry.
uint32_t VizTimelineNewSource() {
    std::lock_guard<std::mutex> lock(g_tlWriteMutex);
    uint32_t gen = g_tlGen.load(std::memory_order_relaxed) + 1;
    VizTlWriteBegin();
    g_tlGen.store(gen, std::memory_order_relaxed);
    g_tlValid.store(false, std::memory_order_relaxed);
    g_tlRunning.store(false, std::memory_order_relaxed);
    g_tlPos.store(0, std::memory_order_relaxed);
    g_tlTick.store(GetTickCount64(), std::memory_order_relaxed);
    VizTlWriteEnd();
    return gen;
}

// Playback status. On a Playing <-> Paused change the position so far is
// folded in and the clock restarts from now, so the bar neither jumps back
// when pausing nor forward by the length of the pause when resuming (players
// often leave the timeline itself untouched across a pause).
void VizTimelineSetPlaying(uint32_t gen, bool playing) {
    std::lock_guard<std::mutex> lock(g_tlWriteMutex);
    if (gen != g_tlGen.load(std::memory_order_relaxed)) return;
    g_mediaIsPlaying.store(playing, std::memory_order_relaxed);
    bool running = g_tlRunning.load(std::memory_order_relaxed);
    if (running == playing) return;
    ULONGLONG now = GetTickCount64();
    int64_t pos = g_tlPos.load(std::memory_order_relaxed);
    if (running) pos += (int64_t)(now - g_tlTick.load(std::memory_order_relaxed)) * 10000;
    VizTlWriteBegin();
    g_tlPos.store(pos, std::memory_order_relaxed);
    g_tlTick.store(now, std::memory_order_relaxed);
    g_tlRunning.store(playing, std::memory_order_relaxed);
    VizTlWriteEnd();
}

// A fresh timeline (TimelinePropertiesChanged, or the first read of a
// session). ageMs is how old the position already is (from LastUpdatedTime);
// it only counts while playing, since a paused position doesn't move.
void VizTimelineSet(uint32_t gen, int64_t start, int64_t end, int64_t pos, int64_t ageMs) {
    std::lock_guard<std::mutex> lock(g_tlWriteMutex);
    if (gen != g_tlGen.load(std::memory_order_relaxed)) return;
    if (ageMs < 0 || ageMs > 6LL * 3600 * 1000) ageMs = 0;  // unset or nonsense
    if (!g_tlRunning.load(std::memory_order_relaxed)) ageMs = 0;
    VizTlWriteBegin();
    g_tlStart.store(start, std::memory_order_relaxed);
    g_tlEnd.store(end, std::memory_order_relaxed);
    g_tlPos.store(pos, std::memory_order_relaxed);
    g_tlTick.store(GetTickCount64() - (ULONGLONG)ageMs, std::memory_order_relaxed);
    g_tlValid.store(end > start, std::memory_order_relaxed);
    VizTlWriteEnd();
}

void VizTimelineInvalidate(uint32_t gen) {
    std::lock_guard<std::mutex> lock(g_tlWriteMutex);
    if (gen != g_tlGen.load(std::memory_order_relaxed)) return;
    VizTlWriteBegin();
    g_tlValid.store(false, std::memory_order_relaxed);
    VizTlWriteEnd();
}

// 0..1 along the track, or -1 for no bar. Render thread.
float VizTrackProgress() {
    static std::atomic<float> s_last{-1.f};  // if a writer is preempted mid-update
    bool valid = false, running = false;
    int64_t start = 0, end = 0, pos = 0;
    ULONGLONG tick = 0;
    for (int tries = 0;; tries++) {
        if (tries == 64) return s_last.load(std::memory_order_relaxed);
        uint32_t s1 = g_tlSeq.load(std::memory_order_acquire);
        if (s1 & 1u) continue;
        valid = g_tlValid.load(std::memory_order_relaxed);
        start = g_tlStart.load(std::memory_order_relaxed);
        end = g_tlEnd.load(std::memory_order_relaxed);
        pos = g_tlPos.load(std::memory_order_relaxed);
        tick = g_tlTick.load(std::memory_order_relaxed);
        running = g_tlRunning.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if (g_tlSeq.load(std::memory_order_relaxed) == s1) break;
    }
    float result = -1.f;
    double dur = (double)(end - start);
    if (valid && dur > 0.0) {
        double p = (double)(pos - start);
        if (running) {
            // Modular difference, so a tick set back past boot by an age still works.
            int64_t elapsedMs = (int64_t)(GetTickCount64() - tick);
            if (elapsedMs > 0) p += (double)elapsedMs * 10000.0;
        }
        result = (float)std::clamp(p / dur, 0.0, 1.0);
    }
    s_last.store(result, std::memory_order_relaxed);
    return result;
}
// ---- track timeline: end
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
// std::optional rather than a bare std::thread: there is no assignment that
// empties a std::thread (move-assigning over a joinable one calls
// std::terminate), so the optional is what gives teardown a join() + reset()
// pair. This is the documented form for a global worker thread.
[[clang::no_destroy]] static std::optional<std::thread> g_gsmtcThread;
static std::thread* g_albumArtThread = nullptr;

// Media Card (2.1): the cover, box-filtered down to at most 160 px, straight
// alpha BGRA as WIC decodes it. Written by the album-art thread, read by the
// media window's paint.
std::mutex g_artTileMutex;
std::vector<BYTE> g_artTile;
int g_artTileW = 0, g_artTileH = 0;
std::atomic<int64_t> g_mediaSeekTicks{0};  // seek target for media command 3, 100 ns units
inline bool VizCardWantsArt() { return g_settings.mediaControlsEnabled && g_settings.mediaCard; }

void VizStoreArtTile(const BYTE* px, int w, int h) {
    std::vector<BYTE> out;
    int ow = 0, oh = 0;
    if (px && w > 0 && h > 0) {
        int f = std::max(1, (std::max(w, h) + 159) / 160);
        ow = std::max(1, w / f);
        oh = std::max(1, h / f);
        out.resize((size_t)ow * oh * 4);
        for (int y = 0; y < oh; y++)
            for (int x = 0; x < ow; x++)
                for (int k = 0; k < 4; k++) {
                    unsigned sum = 0;
                    for (int yy = 0; yy < f; yy++)
                        for (int xx = 0; xx < f; xx++) sum += px[((size_t)(y * f + yy) * w + (x * f + xx)) * 4 + k];
                    out[((size_t)y * ow + x) * 4 + k] = (BYTE)(sum / (unsigned)(f * f));
                }
    }
    {
        std::lock_guard<std::mutex> lock(g_artTileMutex);
        if (out.empty() && g_artTile.empty()) return;
        g_artTile.swap(out);
        g_artTileW = ow;
        g_artTileH = oh;
    }
    if (g_mediaWnd && g_settings.mediaCard) PostMessage(g_mediaWnd, WM_APP_MEDIA_REPAINT, 0, 0);
}
// Guards the pointer above: callers arrive from WinRT callbacks, the GSMTC
// thread and the UI thread, and the worker can clear the pending flag before
// the caller has stored its new thread.
static std::mutex g_albumArtThreadMutex;

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
        // __stdcall is required: windhawk.exe is 32-bit on Windhawk 1.x, and
        // there a capture-less lambda otherwise converts to a __cdecl pointer
        // that will not bind to MONITORENUMPROC. On x86-64 there is only one
        // calling convention, so this is a no-op there.
        [](HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) __stdcall -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

// ---- DXCore: adapter classification -------------------------------------------
//
// DXGI can enumerate GPUs but can't say which one is integrated. DXCore can. It ships in the
// box from Windows 10 2004 on, but the toolchain's headers don't carry it, so
// it is loaded at runtime and the handful of methods used here are declared
// by hand, in vtable order, from the Windows SDK's dxcore_interface.h. Only
// the leading methods are declared, and nothing past the last one is called.
//
// None of this runs per frame: it runs when the device is created.
namespace ttdxcore {

enum : uint32_t {
    kPropInstanceLuid = 0,
    kPropDriverDescription = 2,
    kPropIsIntegrated = 12,
};

struct IAdapter : public IUnknown {
    virtual bool STDMETHODCALLTYPE IsValid() = 0;
    virtual bool STDMETHODCALLTYPE IsAttributeSupported(REFGUID attributeGUID) = 0;
    virtual bool STDMETHODCALLTYPE IsPropertySupported(uint32_t property) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(uint32_t property, size_t bufferSize,
                                                  void* propertyData) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertySize(uint32_t property, size_t* bufferSize) = 0;
};

struct IAdapterList : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetAdapter(uint32_t index, REFIID riid, void** ppvAdapter) = 0;
    virtual uint32_t STDMETHODCALLTYPE GetAdapterCount() = 0;
};

struct IAdapterFactory : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateAdapterList(uint32_t numAttributes,
                                                        const GUID* filterAttributes,
                                                        REFIID riid, void** ppvAdapterList) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAdapterByLuid(const LUID& adapterLUID, REFIID riid,
                                                       void** ppvAdapter) = 0;
};

static const GUID kIID_Factory = {
    0x78ee5945, 0xc36e, 0x4b13, {0xa6, 0x69, 0x00, 0x5d, 0xd1, 0x1c, 0x0f, 0x06}};
static const GUID kIID_Adapter = {
    0xf0db4c7f, 0xfe5a, 0x42a2, {0xbd, 0x62, 0xf2, 0xa6, 0xcf, 0x6f, 0xc8, 0x3e}};

typedef HRESULT(WINAPI* CreateFactoryFn)(REFIID riid, void** ppvFactory);

inline ComPtr<IAdapterFactory> CreateFactory() {
    // Kept loaded for the life of the process: it's a system DLL, and
    // FreeLibrary on it buys nothing.
    static HMODULE s_module = LoadLibraryExW(L"dxcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    ComPtr<IAdapterFactory> factory;
    if (!s_module) return factory;
    auto create = (CreateFactoryFn)(void*)GetProcAddress(s_module, "DXCoreCreateAdapterFactory");
    if (!create) return factory;
    if (FAILED(create(kIID_Factory, (void**)factory.GetAddressOf()))) factory.Reset();
    return factory;
}

// 1 = integrated, 0 = discrete, -1 = DXCore can't say (pre-2004 Windows, or a
// LUID it doesn't know).
inline int IsIntegrated(const LUID& luid) {
    ComPtr<IAdapterFactory> factory = CreateFactory();
    if (!factory) return -1;
    ComPtr<IAdapter> adapter;
    if (FAILED(factory->GetAdapterByLuid(luid, kIID_Adapter, (void**)adapter.GetAddressOf())) ||
        !adapter) {
        return -1;
    }
    bool integrated = false;
    if (FAILED(adapter->GetProperty(kPropIsIntegrated, sizeof(integrated), &integrated))) return -1;
    return integrated ? 1 : 0;
}

inline std::wstring AdapterDescription(IAdapter* adapter) {
    size_t size = 0;
    if (FAILED(adapter->GetPropertySize(kPropDriverDescription, &size)) || size == 0 || size > 1024)
        return L"";
    std::string narrow(size, '\0');
    if (FAILED(adapter->GetProperty(kPropDriverDescription, size, narrow.data()))) return L"";
    narrow.resize(strnlen(narrow.c_str(), size));
    int wlen = MultiByteToWideChar(CP_ACP, 0, narrow.c_str(), (int)narrow.size(), nullptr, 0);
    std::wstring wide(wlen > 0 ? wlen : 0, L'\0');
    if (wlen > 0) MultiByteToWideChar(CP_ACP, 0, narrow.c_str(), (int)narrow.size(), wide.data(), wlen);
    return wide;
}

}  // namespace ttdxcore

bool ParseColorHex(PCWSTR colorStr, BYTE* a, BYTE* r, BYTE* g, BYTE* b) {
    if (!colorStr || !*colorStr) {
        return false;
    }
    while (*colorStr == L' ' || *colorStr == L'\t') colorStr++;

    if (_wcsnicmp(colorStr, L"rgba", 4) == 0 || _wcsnicmp(colorStr, L"rgb", 3) == 0) {
        const WCHAR* p = wcschr(colorStr, L'(');
        if (!p) return false;
        p++;
        float v[4] = {0.f, 0.f, 0.f, 1.0f};
        int n = swscanf_s(p, L"%f , %f , %f , %f", &v[0], &v[1], &v[2], &v[3]);
        if (n < 3) return false;
        *r = (BYTE)std::clamp((int)std::lround(v[0]), 0, 255);
        *g = (BYTE)std::clamp((int)std::lround(v[1]), 0, 255);
        *b = (BYTE)std::clamp((int)std::lround(v[2]), 0, 255);
        // Alpha is CSS-style 0-1, but 0-255 is tolerated too in case someone
        // carries over an integer alpha from elsewhere.
        float alpha = (n == 4) ? v[3] : 1.0f;
        if (alpha > 1.0f) alpha /= 255.0f;
        *a = (BYTE)std::clamp((int)std::lround(alpha * 255.0f), 0, 255);
        return true;
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

// ---- Settings validation ---------------------------------------------------
// Windhawk's settings UI has no notion of "this field is malformed" -- it takes
// whatever you type into a free-text box and saves it. Every free-text setting
// here (colors, positions, padding, corner radii, icon paths) used to fall back
// to a default on a parse failure without saying so, which means a typo looks
// exactly like a saved setting that simply doesn't do anything. These helpers
// collect every field that failed to parse during a LoadSettings() pass so the
// mod can hand the user one summary of what it couldn't read.

std::vector<std::wstring> g_settingsIssues;

void ReportSettingIssue(PCWSTR group, PCWSTR name, PCWSTR typed, PCWSTR expected,
                        PCWSTR usedInstead) {
    // Built by concatenation rather than into a fixed buffer -- an icon path can
    // run to MAX_PATH on its own and the surrounding text is longer still.
    PCWSTR shown = (typed && *typed) ? typed : L"(blank)";
    std::wstring line = std::wstring(group) + L" \x25B8 " + name +
                        L"\r\n      you typed:  " + shown +
                        L"\r\n      expected:   " + expected +
                        L"\r\n      using:      " + usedInstead;
    g_settingsIssues.push_back(std::move(line));
    Wh_Log(L"[Settings] INVALID %s/%s value=\"%s\" expected=%s using=%s",
           group, name, shown, expected, usedInstead);
}

// Same channel as ReportSettingIssue, for a setting that parsed perfectly well
// but is going to do something you probably didn't intend. There's no "wrong
// value" to quote here, so this is free-form.
void ReportSettingWarning(PCWSTR group, PCWSTR name, const std::wstring& detail) {
    std::wstring line = std::wstring(group) + L" \x25B8 " + name + L"\r\n      " + detail;
    g_settingsIssues.push_back(std::move(line));
    Wh_Log(L"[Settings] WARNING %s/%s -- %s", group, name, line.c_str());
}

// Reads a free-text setting that must be a single number in [lo, hi]. Rejects
// trailing junk ("50px", "50 50") rather than silently taking the leading
// number, since that's exactly the kind of near-miss that looks like it worked.
// A size or offset: a number with decimals, given as text ('1.25'; a comma
// works as the decimal point too). Up to 2.0 these were whole-number
// settings, and a value saved then is still stored as a number, which reads
// back here as empty text: that number is used until the setting is saved
// again.
float ReadSizeSetting(PCWSTR key, float lo, float hi, float def) {
    PCWSTR str = VizSettingString(key);
    float v = def;
    bool ok = false;
    if (str && *str) {
        WCHAR buf[64];
        wcsncpy_s(buf, str, _TRUNCATE);
        for (WCHAR* c = buf; *c; c++)
            if (*c == L',') *c = L'.';
        WCHAR* end = nullptr;
        double d = wcstod(buf, &end);
        if (end != buf && std::isfinite(d)) {
            v = (float)d;
            ok = true;
        }
    }
    VizSettingFree(str);
    if (!ok) {
        int n = VizSettingInt(key);
        if (n != 0) v = (float)n;
    }
    return std::clamp(v, lo, hi);
}

float ReadNumberSetting(PCWSTR key, PCWSTR group, PCWSTR name, float def, float lo, float hi) {
    PCWSTR str = VizSettingString(key);
    float result = def;

    WCHAR* end = nullptr;
    double parsed = (str && *str) ? wcstod(str, &end) : 0.0;
    bool parsedAnything = (str && end && end != str);
    if (parsedAnything) {
        while (*end == L' ' || *end == L'\t') end++;
    }

    if (!parsedAnything || *end) {
        WCHAR expected[128], using_[64];
        swprintf_s(expected, L"a single number from %g to %g (decimals allowed)", lo, hi);
        swprintf_s(using_, L"%g", def);
        ReportSettingIssue(group, name, str, expected, using_);
    } else if (parsed < lo || parsed > hi) {
        WCHAR expected[128], using_[64];
        swprintf_s(expected, L"a number from %g to %g", lo, hi);
        result = std::clamp((float)parsed, lo, hi);
        swprintf_s(using_, L"%g (clamped into range)", result);
        ReportSettingIssue(group, name, str, expected, using_);
    } else {
        result = (float)parsed;
    }

    VizSettingFree(str);
    return result;
}

// Reads a color setting, reporting the exact accepted formats on failure.
void ReadColorSetting(PCWSTR key, PCWSTR group, PCWSTR name, BYTE defA, BYTE defR, BYTE defG,
                      BYTE defB, BYTE* a, BYTE* r, BYTE* g, BYTE* b) {
    PCWSTR str = VizSettingString(key);
    if (!ParseColorHex(str, a, r, g, b)) {
        *a = defA; *r = defR; *g = defG; *b = defB;
        WCHAR using_[64];
        swprintf_s(using_, L"#%02X%02X%02X%02X", defA, defR, defG, defB);
        ReportSettingIssue(group, name, str,
                           L"#AARRGGBB, #RRGGBB, rgba(r, g, b, a) or rgb(r, g, b)", using_);
    }
    VizSettingFree(str);
}

// Reads a "one value, or four space-separated values" box (padding, corner
// radii). Anything other than exactly 1 or 4 numbers is a mistake worth saying
// out loud -- two values in particular reads as if it might mean
// horizontal/vertical, and it doesn't.
void ReadQuadSetting(PCWSTR key, PCWSTR group, PCWSTR name, PCWSTR meaning, float def,
                     bool allowNegative, float out[4]) {
    PCWSTR str = VizSettingString(key);
    out[0] = out[1] = out[2] = out[3] = def;

    float v[4] = {def, def, def, def};
    int n = (str && *str) ? swscanf_s(str, L"%f %f %f %f", &v[0], &v[1], &v[2], &v[3]) : 0;

    if (n == 1 || n == 4) {
        if (n == 1) v[1] = v[2] = v[3] = v[0];
        for (int k = 0; k < 4; k++) out[k] = allowNegative ? v[k] : std::max(0.0f, v[k]);
    } else {
        WCHAR expected[256], using_[64];
        swprintf_s(expected, L"one number for all sides, or four space-separated numbers for %s",
                   meaning);
        swprintf_s(using_, L"%g on all sides", def);
        ReportSettingIssue(group, name, str, expected, using_);
    }

    VizSettingFree(str);
}

// Reads one of the "how much has to be covered" dropdowns. Stored as a string
// rather than an int because Windhawk only accepts $options on string settings
// -- same as the FFT Size dropdown. Picking from a list means it can't normally
// be wrong, but a hand-edited settings file still can be.
int ReadThresholdPercentSetting(PCWSTR key, PCWSTR group, PCWSTR name, int def) {
    PCWSTR str = VizSettingString(key);
    int v = (str && *str) ? _wtoi(str) : 0;

    if (v < 5 || v > 100) {
        WCHAR using_[32];
        swprintf_s(using_, L"%d%%", def);
        ReportSettingIssue(group, name, str, L"a percentage from 5 to 100, in steps of 5",
                           using_);
        v = def;
    }

    VizSettingFree(str);
    return v;
}

// Reads an optional path to an image file. Blank is valid and means "use the
// built-in glyph" -- but a path that's set and wrong is silent breakage, since
// the fallback looks identical to having left it blank.
std::wstring ReadIconPathSetting(PCWSTR key, PCWSTR group, PCWSTR name) {
    PCWSTR str = VizSettingString(key);
    std::wstring path = str ? str : L"";

    // Users paste paths out of Explorer's address bar or "Copy as path", both of
    // which can bring quotes along; strip them rather than calling it invalid.
    while (!path.empty() && (path.front() == L'"' || path.front() == L' ')) path.erase(path.begin());
    while (!path.empty() && (path.back() == L'"' || path.back() == L' ')) path.pop_back();

    if (!path.empty()) {
        DWORD attr = GetFileAttributes(path.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES) {
            ReportSettingIssue(group, name, path.c_str(),
                               L"a full path to an existing image file (PNG, JPG, BMP or ICO)",
                               L"the built-in icon");
            path.clear();
        } else if (attr & FILE_ATTRIBUTE_DIRECTORY) {
            ReportSettingIssue(group, name, path.c_str(),
                               L"a path to an image file, not to a folder",
                               L"the built-in icon");
            path.clear();
        }
    }

    VizSettingFree(str);
    return path;
}

// A font that really is installed can still enumerate as missing right after a
// cold boot: mods start before the font service has finished registering
// everything, so the check runs against an incomplete list and warns about a
// font that works perfectly the moment anything draws with it.
//
// So a failed check does not report straight away. It arms a one-second retry
// on the message window and only reports if the font is still missing once the
// system has had time to settle. A genuinely wrong name still gets flagged,
// just ten seconds later than it used to.
static bool g_fontCheckPending = false;
static int g_fontCheckAttempts = 0;
static constexpr int FONT_CHECK_MAX_ATTEMPTS = 10;

// Checks a font family name is actually installed. A missing font falls back to
// whatever DirectWrite substitutes, which is silently not what was asked for.
bool IsFontInstalled(const std::wstring& family) {
    if (family.empty()) return false;
    HDC hdc = GetDC(nullptr);
    if (!hdc) return true;  // can't tell -- don't cry wolf

    LOGFONT lf = {};
    lf.lfCharSet = DEFAULT_CHARSET;
    lstrcpyn(lf.lfFaceName, family.c_str(), LF_FACESIZE);

    bool found = false;
    EnumFontFamiliesEx(
        hdc, &lf,
        [](const LOGFONT*, const TEXTMETRIC*, DWORD, LPARAM param) __stdcall -> int {
            *reinterpret_cast<bool*>(param) = true;
            return 0;
        },
        reinterpret_cast<LPARAM>(&found), 0);

    ReleaseDC(nullptr, hdc);
    return found;
}

PCWSTR ModKeyFlagsName(unsigned flags) {
    switch (flags) {
        case VIZ_MOD_CTRL:  return L"Ctrl";
        case VIZ_MOD_ALT:   return L"Alt";
        case VIZ_MOD_SHIFT: return L"Shift";
        case VIZ_MOD_WIN:   return L"Win";
        case VIZ_MOD_CTRL | VIZ_MOD_ALT:   return L"Ctrl + Alt";
        case VIZ_MOD_CTRL | VIZ_MOD_SHIFT: return L"Ctrl + Shift";
        case VIZ_MOD_ALT  | VIZ_MOD_SHIFT: return L"Alt + Shift";
        case VIZ_MOD_WIN  | VIZ_MOD_ALT:   return L"Win + Alt";
        case VIZ_MOD_WIN  | VIZ_MOD_SHIFT: return L"Win + Shift";
    }
    return L"None";
}

// Shown on its own thread: this runs from LoadSettings, which is called on the
// UI thread out of the settings-changed message, and a modal box there would
// wedge rendering until the user clicked OK.
DWORD WINAPI SettingsIssueDialogThread(LPVOID param) {
    std::wstring* text = (std::wstring*)param;
    MessageBox(nullptr, text->c_str(), L"Tourne'Table - Settings Problems",
               MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
    delete text;
    return 0;
}

void FlushSettingsIssues() {
    if (g_settingsIssues.empty()) return;

    std::wstring body =
        L"Tourne'Table found something worth flagging in your settings.\r\n\r\n"
        L"Anything listed as \"you typed / expected\" couldn't be read at all, so a fallback "
        L"value is being used -- the setting is saved, it just isn't doing anything. Anything "
        L"else is a heads-up about a setting that works exactly as configured but may not do "
        L"what you expect.\r\n\r\n";
    for (const auto& issue : g_settingsIssues) {
        body += L"  \x2022  " + issue + L"\r\n\r\n";
    }
    body += L"(Turn this off under Settings Validation \x25B8 Warn About Invalid Settings.)";

    g_settingsIssues.clear();

    if (!g_settings.showSettingsErrors) return;

    HANDLE h = CreateThread(nullptr, 0, SettingsIssueDialogThread,
                            new std::wstring(std::move(body)), 0, nullptr);
    if (h) CloseHandle(h);
}

HWND GetWorkerW() {
    HWND hProgman = FindWindow(L"Progman", nullptr);
    if (!hProgman) return nullptr;

    // Progman belongs to explorer.exe, not to us -- this mod now runs as its
    // own process (see the tool-mod boilerplate at the end of this file), so
    // there's no same-process check to make here. SendMessage/FindWindowEx
    // work fine across process boundaries; this is the same technique
    // standalone desktop-overlay tools use to reach behind the icons without
    // ever injecting into explorer.
    SendMessage(hProgman, 0x052C, 0xD, 0);
    SendMessage(hProgman, 0x052C, 0xD, 1);

    HWND hWorkerW = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) __stdcall -> BOOL {
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
            [](HWND hWnd, LPARAM lParam) __stdcall -> BOOL {
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
    HMONITOR targetMonitor = GetMonitorById(g_settings.monitor - 1);
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
// Returns how much of `target` (in virtual-screen coordinates) is underneath
// real application windows, as a percentage of its area.
//
// Coverage is accumulated into a GDI region rather than tested per window, so
// several windows that each hide part of the box add up correctly instead of
// each being judged on its own -- two windows covering opposite halves read as
// 100%, which is what you see on screen. GetRegionData hands back a set of
// non-overlapping rectangles, so summing their areas double-counts nothing.
int ComputeRectCoveragePercent(const RECT& targetIn) {
    if (targetIn.right <= targetIn.left || targetIn.bottom <= targetIn.top) return 0;

    // Judge only the part that is actually on a screen.
    //
    // Area hanging off an edge can never intersect a window rect, so leaving it
    // in the denominator permanently caps the result below 100%. At the default
    // "100% covered" threshold that makes Pause When Covered unreachable for any
    // visualizer positioned partly off-screen, which is a common placement along
    // the bottom edge, and the feature simply appears to do nothing.
    RECT screen;
    screen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    screen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    screen.right = screen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    screen.bottom = screen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    RECT target;
    if (!IntersectRect(&target, &targetIn, &screen)) {
        // Nothing of it is on screen at all. Reported as uncovered rather than
        // fully covered, staying with this function's existing bias: a wrong
        // answer that keeps drawing is a far smaller problem than one that makes
        // the visualizer vanish.
        return 0;
    }

    HRGN coveredRgn = CreateRectRgn(0, 0, 0, 0);
    if (!coveredRgn) return 0;

    struct EnumCtx {
        RECT viz;
        HRGN covered;
    } ctx{target, coveredRgn};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) __stdcall -> BOOL {
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
                    _wcsnicmp(cls, L"TourneTaskbar", 13) == 0 ||
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

            RECT hit;
            if (!IntersectRect(&hit, &wr, &c->viz)) return TRUE;

            HRGN piece = CreateRectRgn(hit.left, hit.top, hit.right, hit.bottom);
            if (piece) {
                CombineRgn(c->covered, c->covered, piece, RGN_OR);
                DeleteObject(piece);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    long long coveredArea = 0;
    DWORD dataSize = GetRegionData(coveredRgn, 0, nullptr);
    if (dataSize) {
        std::vector<BYTE> buf(dataSize);
        RGNDATA* rd = reinterpret_cast<RGNDATA*>(buf.data());
        if (GetRegionData(coveredRgn, dataSize, rd) == dataSize) {
            const RECT* rects = reinterpret_cast<const RECT*>(rd->Buffer);
            for (DWORD i = 0; i < rd->rdh.nCount; i++) {
                coveredArea += (long long)(rects[i].right - rects[i].left) *
                               (long long)(rects[i].bottom - rects[i].top);
            }
        }
    }
    DeleteObject(coveredRgn);

    long long totalArea = (long long)(target.right - target.left) *
                          (long long)(target.bottom - target.top);
    if (totalArea <= 0) return 0;
    return (int)std::min<long long>(100, (coveredArea * 100) / totalArea);
}

bool IsVisualizerOccluded() {
    if (!g_drawRectValid.load(std::memory_order_relaxed)) return false;

    RECT viz;
    viz.left   = g_drawRectL.load(std::memory_order_relaxed);
    viz.top    = g_drawRectT.load(std::memory_order_relaxed);
    viz.right  = g_drawRectR.load(std::memory_order_relaxed);
    viz.bottom = g_drawRectB.load(std::memory_order_relaxed);

    int threshold = std::clamp(g_settings.obscuredThresholdPercent, 5, 100);
    int covered = ComputeRectCoveragePercent(viz);
    bool occluded = covered >= threshold;

    // Logged on the transition only -- this runs once a second, and a line per
    // second for as long as a window happens to be open would bury everything
    // else in the log.
    static bool s_lastOccluded = false;
    if (occluded != s_lastOccluded) {
        Wh_Log(L"[Viz] %s: %d%% covered (threshold %d%%)",
               occluded ? L"occluded" : L"visible again", covered, threshold);
        s_lastOccluded = occluded;
    }
    return occluded;
}

// Same measurement, aimed at the media strip's own rect. The strip is topmost,
// so nothing ever covers it on screen -- "covered" here means a real window is
// sitting underneath it, which is when it's in the way rather than useful.
bool IsMediaStripCovered() {
    if (!g_mediaWnd) return false;

    RECT strip;
    if (!GetWindowRect(g_mediaWnd, &strip)) return false;

    int threshold = std::clamp(g_settings.mediaCoveredThresholdPercent, 5, 100);
    return ComputeRectCoveragePercent(strip) >= threshold;
}

// Windows' taskbar, or Tourne Desktop's (see VizMenuHook).
bool VizIsTaskbarWindow(HWND w) {
    WCHAR cls[64] = {};
    if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) return false;
    return wcscmp(cls, L"Shell_TrayWnd") == 0 || wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
           wcsncmp(cls, L"TourneTaskbar", 13) == 0;
}

// Whether a visible taskbar overlaps rc. With aboveStrip, only one stacked
// above the media window counts. A taskbar is topmost, so media controls placed
// over one have to be topmost too, and come back up when the taskbar is raised
// (a click on it does that), or clicks meant for them land on the taskbar.
bool VizTaskbarOverlaps(const RECT& rc, bool aboveStrip) {
    auto hits = [&](HWND w) {
        RECT r, x;
        return IsWindowVisible(w) && VizIsTaskbarWindow(w) && GetWindowRect(w, &r) && IntersectRect(&x, &r, &rc);
    };
    if (aboveStrip) {
        if (!g_mediaWnd) return false;
        for (HWND w = GetWindow(g_mediaWnd, GW_HWNDPREV); w; w = GetWindow(w, GW_HWNDPREV))
            if (hits(w)) return true;
        return false;
    }
    struct Ctx {
        decltype(hits)* fn;
        bool hit;
    } ctx{&hits, false};
    EnumWindows(
        [](HWND w, LPARAM lp) __stdcall -> BOOL {
            auto* c = (Ctx*)lp;
            if (!(*c->fn)(w)) return TRUE;
            c->hit = true;
            return FALSE;
        },
        (LPARAM)&ctx);
    return ctx.hit;
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
    bool expected = false;
    if (!g_albumArtFetchPending.compare_exchange_strong(expected, true))
        return;

    std::lock_guard<std::mutex> threadLock(g_albumArtThreadMutex);
    if (g_albumArtThread) {
        if (g_albumArtThread->joinable())
            g_albumArtThread->join();
        delete g_albumArtThread;
        g_albumArtThread = nullptr;
    }

    g_albumArtThread = new std::thread([]() {
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
            auto mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (!mgr) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }
            auto session = mgr.GetCurrentSession();
            if (!session) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            auto props = session.TryGetMediaPropertiesAsync().get();
            if (!props) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            {
                std::wstring title(props.Title());
                std::wstring artist(props.Artist());
                std::wstring display = artist.empty() ? title
                                      : title.empty()  ? artist
                                                        : (artist + L" - " + title);
                if (!display.empty()) {
                    std::lock_guard<std::mutex> lock(g_nowPlayingMutex);
                    g_nowPlayingTitle = title;
                    g_nowPlayingArtist = artist;
                    if (g_nowPlayingDisplay != display) {
                        g_nowPlayingDisplay = display;
                        g_nowPlayingChangedTick.store(GetTickCount64(), std::memory_order_relaxed);
                    }
                }
            }

            auto thumbRef = props.Thumbnail();
            if (!thumbRef) { VizStoreArtTile(nullptr, 0, 0); winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            auto stream = thumbRef.OpenReadAsync().get();
            if (!stream) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            UINT64 sz = stream.Size();
            if (sz == 0 || sz > 4 * 1024 * 1024) { winrt::uninit_apartment(); g_albumArtFetchPending.store(false); return; }

            DataReader reader(stream);
            reader.LoadAsync((UINT32)sz).get();
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

            if (!pixels.empty()) VizStoreArtTile(pixels.data(), imgW, imgH);
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
                    if (g_mediaWnd && g_settings.mediaCard) PostMessage(g_mediaWnd, WM_APP_MEDIA_REPAINT, 0, 0);
                }
            }
        } catch (...) {}
        try { winrt::uninit_apartment(); } catch (...) {}
        g_albumArtFetchPending.store(false);
    });
}

static winrt::event_token g_gsmtcMediaPropsToken{};
static winrt::event_token g_gsmtcPlaybackToken{};
static winrt::event_token g_gsmtcTimelineToken{};
static winrt::event_token g_gsmtcSessionToken{};
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSessionManager g_gsmtcMgr{ nullptr };
[[clang::no_destroy]] static GlobalSystemMediaTransportControlsSession        g_gsmtcSession{ nullptr };

// g_gsmtcSession, g_gsmtcMgr and the session's event tokens are only touched
// under this lock: by SetupGsmtcSessionListener (the GSMTC thread at start, a
// WinRT thread-pool thread on every CurrentSessionChanged) and by shutdown.
// The session's own event handlers never read them; they get the session as
// `sender` and the generation it was hooked up with, and a handler still
// running for a replaced session has its result dropped by the timeline.
static std::mutex g_gsmtcSessionMutex;

void RefreshMediaPlaybackStatus(GlobalSystemMediaTransportControlsSession const& session, uint32_t gen) {
    if (!session) return;
    try {
        auto info = session.GetPlaybackInfo();
        bool playing = info && info.PlaybackStatus() ==
            GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
        bool was = g_mediaIsPlaying.load(std::memory_order_relaxed);
        VizTimelineSetPlaying(gen, playing);  // also sets g_mediaIsPlaying
        // Some players send PlaybackInfoChanged every second or so with
        // nothing changed; only a play/pause change needs the controls redrawn.
        if (playing == was) return;
    } catch (...) {}
    if (g_mediaWnd) PostMessage(g_mediaWnd, WM_APP_MEDIA_REPAINT, 0, 0);
}

// Reads the session's timeline for the progress bar. LastUpdatedTime says how
// old the position already is; a player that leaves it unset gets age 0.
void RefreshMediaTimeline(GlobalSystemMediaTransportControlsSession const& session, uint32_t gen) {
    if (!session) return;
    try {
        auto tl = session.GetTimelineProperties();
        if (!tl) {
            VizTimelineInvalidate(gen);
            return;
        }
        int64_t start = tl.StartTime().count(), end = tl.EndTime().count(), pos = tl.Position().count();
        int64_t ageMs = std::chrono::duration_cast<std::chrono::milliseconds>(winrt::clock::now() -
                                                                              tl.LastUpdatedTime())
                            .count();
        VizTimelineSet(gen, start, end, pos, ageMs);
    } catch (...) {
        VizTimelineInvalidate(gen);
    }
}

void SetupGsmtcSessionListener() {
    std::lock_guard<std::mutex> lock(g_gsmtcSessionMutex);
    if (!g_gsmtcMgr) return;
    try {
        if (g_gsmtcSession) {
            try { g_gsmtcSession.MediaPropertiesChanged(g_gsmtcMediaPropsToken); } catch (...) {}
            try { g_gsmtcSession.PlaybackInfoChanged(g_gsmtcPlaybackToken); } catch (...) {}
            try { g_gsmtcSession.TimelinePropertiesChanged(g_gsmtcTimelineToken); } catch (...) {}
            g_gsmtcSession = nullptr;
        }
        // From here on, events from the old session are ignored.
        const uint32_t gen = VizTimelineNewSource();
        GlobalSystemMediaTransportControlsSession session = g_gsmtcMgr.GetCurrentSession();
        g_gsmtcSession = session;
        if (!session) return;
        g_gsmtcMediaPropsToken = session.MediaPropertiesChanged(
            [](auto const&, auto const&) {
                if (g_settings.colorMode == VizColorMode::AlbumArt ||
                    g_settings.colorMode == VizColorMode::DynamicAlbum ||
                    g_settings.nowPlayingEnabled || VizCardWantsArt())
                    FetchAlbumArtColorAsync();
            });
        // Pausing / resuming folds the position in (VizTimelineSetPlaying);
        // the timeline itself is only re-read when the player changes it.
        g_gsmtcPlaybackToken = session.PlaybackInfoChanged(
            [gen](GlobalSystemMediaTransportControlsSession const& sender, auto const&) {
                RefreshMediaPlaybackStatus(sender, gen);
            });
        g_gsmtcTimelineToken = session.TimelinePropertiesChanged(
            [gen](GlobalSystemMediaTransportControlsSession const& sender, auto const&) {
                RefreshMediaTimeline(sender, gen);
            });
        // Status first, so the first timeline read knows whether its age counts.
        RefreshMediaPlaybackStatus(session, gen);
        RefreshMediaTimeline(session, gen);
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
                        g_settings.nowPlayingEnabled || VizCardWantsArt())
                        FetchAlbumArtColorAsync();
                });

            SetupGsmtcSessionListener();
            if (g_settings.colorMode == VizColorMode::AlbumArt ||
                g_settings.colorMode == VizColorMode::DynamicAlbum ||
                g_settings.nowPlayingEnabled || VizCardWantsArt())
                FetchAlbumArtColorAsync();
        } catch (...) {}

        if (g_gsmtcStopEvent)
            WaitForSingleObject(g_gsmtcStopEvent, INFINITE);

        try {
            if (g_gsmtcMgr) g_gsmtcMgr.CurrentSessionChanged(g_gsmtcSessionToken);
        } catch (...) {}
        {
            std::lock_guard<std::mutex> lock(g_gsmtcSessionMutex);
            if (g_gsmtcSession) {
                try { g_gsmtcSession.MediaPropertiesChanged(g_gsmtcMediaPropsToken); } catch (...) {}
                try { g_gsmtcSession.PlaybackInfoChanged(g_gsmtcPlaybackToken); } catch (...) {}
                try { g_gsmtcSession.TimelinePropertiesChanged(g_gsmtcTimelineToken); } catch (...) {}
            }
            g_gsmtcSession = nullptr;
            g_gsmtcMgr     = nullptr;
        }
        try { winrt::uninit_apartment(); } catch (...) {}
    });
}

static bool g_gsmtcStarted = false;

// ---- Media control buttons -------------------------------------------------
// A small always-on-top, clickable strip (Previous / Play-Pause / Next), wired
// to whichever app is currently playing media via the same GSMTC session used
// for Now Playing. This lives in its own layered window rather than the main
// overlay's swap chain: the overlay is WS_EX_TRANSPARENT on purpose (it sits
// behind the desktop icons and must never eat clicks), so a window that needs
// to actually receive clicks has to be a separate, non-transparent surface.

std::thread* g_mediaCmdThread = nullptr;
std::atomic<bool> g_mediaCmdPending{false};
std::atomic<int> g_mediaCmdQueued{-1};

void SendMediaCommand(int cmd) {
    g_mediaCmdQueued.store(cmd);

    bool expected = false;
    if (!g_mediaCmdPending.compare_exchange_strong(expected, true))
        return;

    if (g_mediaCmdThread) {
        if (g_mediaCmdThread->joinable())
            g_mediaCmdThread->join();
        delete g_mediaCmdThread;
        g_mediaCmdThread = nullptr;
    }

    // The thread runs every command queued while it works (the latest one, if
    // several came in), so a seek on release isn't dropped while the seek on
    // press is still in flight.
    g_mediaCmdThread = new std::thread([]() {
        try { winrt::init_apartment(winrt::apartment_type::multi_threaded); } catch (...) {}
        for (;;) {
            int cmd = g_mediaCmdQueued.exchange(-1);
            if (cmd < 0) {
                g_mediaCmdPending.store(false);
                // A command that arrived just now, after its caller saw us still busy.
                if (g_mediaCmdQueued.load() >= 0 && !g_mediaCmdPending.exchange(true)) continue;
                break;
            }
            try {
                auto mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
                if (mgr) {
                    auto session = mgr.GetCurrentSession();
                    if (session) {
                        if (cmd == 0)      session.TrySkipPreviousAsync().get();
                        else if (cmd == 1) session.TryTogglePlayPauseAsync().get();
                        else if (cmd == 2) session.TrySkipNextAsync().get();
                        else if (cmd == 3)
                            session.TryChangePlaybackPositionAsync(g_mediaSeekTicks.load(std::memory_order_relaxed)).get();
                    }
                }
            } catch (...) {}
        }
        try { winrt::uninit_apartment(); } catch (...) {}
    });
}

// icon slot order: 0=prev, 1=play, 2=pause, 3=next. An empty vector means "no
// custom icon loaded for this slot -- draw the built-in glyph instead."
std::vector<BYTE> g_mediaIconPixels[4];
int g_mediaIconLoadedSize = 0;

float GetMediaControlsDpiScale() {
    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    return GetMonitorDpiScale(monitor);
}

// Decodes a user-supplied image file to a square, premultiplied 32bpp BGRA
// buffer at targetSize -- the format UpdateLayeredWindow's AC_SRC_ALPHA blend
// expects. Returns false (and clears outPixels) on any failure, so callers can
// fall back to the built-in glyph rather than showing a blank button.
bool LoadMediaIconFromFile(const std::wstring& path, int targetSize, std::vector<BYTE>& outPixels) {
    outPixels.clear();
    if (path.empty() || targetSize <= 0) return false;

    IWICImagingFactory* pFactory = nullptr;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&pFactory))) || !pFactory) {
        return false;
    }

    bool ok = false;
    IWICBitmapDecoder* pDecoder = nullptr;
    IWICBitmapFrameDecode* pFrame = nullptr;
    IWICFormatConverter* pConv = nullptr;
    IWICBitmapScaler* pScaler = nullptr;

    if (SUCCEEDED(pFactory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnDemand, &pDecoder)) &&
        SUCCEEDED(pDecoder->GetFrame(0, &pFrame)) &&
        SUCCEEDED(pFactory->CreateFormatConverter(&pConv)) &&
        SUCCEEDED(pConv->Initialize(pFrame, GUID_WICPixelFormat32bppBGRA,
            WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeMedianCut)) &&
        SUCCEEDED(pFactory->CreateBitmapScaler(&pScaler)) &&
        SUCCEEDED(pScaler->Initialize(pConv, (UINT)targetSize, (UINT)targetSize,
            WICBitmapInterpolationModeFant))) {
        outPixels.resize((size_t)targetSize * targetSize * 4);
        if (SUCCEEDED(pScaler->CopyPixels(nullptr, targetSize * 4, (UINT)outPixels.size(),
                outPixels.data()))) {
            for (size_t i = 0; i + 3 < outPixels.size(); i += 4) {
                BYTE a = outPixels[i + 3];
                outPixels[i + 0] = (BYTE)(outPixels[i + 0] * a / 255);
                outPixels[i + 1] = (BYTE)(outPixels[i + 1] * a / 255);
                outPixels[i + 2] = (BYTE)(outPixels[i + 2] * a / 255);
            }
            ok = true;
        }
    }

    if (pScaler)  pScaler->Release();
    if (pConv)    pConv->Release();
    if (pFrame)   pFrame->Release();
    if (pDecoder) pDecoder->Release();
    pFactory->Release();

    if (!ok) outPixels.clear();
    return ok;
}

// LoadSettings already rejected paths that don't exist. A path that exists but
// still won't decode means the file isn't an image format WIC can read, and
// that's worth saying out loud rather than silently drawing the built-in glyph.
void LoadMediaIconSlot(const std::wstring& path, int sizePx, std::vector<BYTE>& out, PCWSTR label) {
    if (!LoadMediaIconFromFile(path, sizePx, out) && !path.empty()) {
        Wh_Log(L"[Media] %s icon could not be decoded: %s", label, path.c_str());
    }
}

void RecreateMediaControlResources() {
    float dpiScale = GetMediaControlsDpiScale();
    int sizePx = std::max(1, (int)std::lround(g_settings.mediaIconSize * dpiScale));
    if (g_settings.mediaCard) {
        // The card draws its controls at a quarter of the art tile; load the
        // icons at that size so they are not stretched up from the strip size.
        int sp = std::max(0, (int)std::lround(g_settings.mediaIconSpacing * dpiScale));
        int tile = g_settings.cardArtSize > 0 ? std::max(24, (int)std::lround(g_settings.cardArtSize * dpiScale))
                                              : sizePx * 3 + sp * 2;
        sizePx = std::max(8, tile / 4);
    }
    g_mediaIconLoadedSize = sizePx;
    LoadMediaIconSlot(g_settings.mediaIconPrevPath, sizePx, g_mediaIconPixels[0], L"Previous");
    LoadMediaIconSlot(g_settings.mediaIconPlayPath, sizePx, g_mediaIconPixels[1], L"Play");
    LoadMediaIconSlot(g_settings.mediaIconPausePath, sizePx, g_mediaIconPixels[2], L"Pause");
    LoadMediaIconSlot(g_settings.mediaIconNextPath, sizePx, g_mediaIconPixels[3], L"Next");
}

// Source-over blend of one premultiplied BGRA pixel onto another. Everything in
// this buffer has to stay premultiplied because it goes straight to
// UpdateLayeredWindow's AC_SRC_ALPHA blend, which assumes exactly that.
inline void BlendPremultipliedOver(BYTE* dst, BYTE sb, BYTE sg, BYTE sr, BYTE sa) {
    if (sa == 255) {
        dst[0] = sb; dst[1] = sg; dst[2] = sr; dst[3] = sa;
        return;
    }
    if (sa == 0) return;
    unsigned inv = 255u - sa;
    dst[0] = (BYTE)(sb + (dst[0] * inv + 127) / 255);
    dst[1] = (BYTE)(sg + (dst[1] * inv + 127) / 255);
    dst[2] = (BYTE)(sr + (dst[2] * inv + 127) / 255);
    dst[3] = (BYTE)(sa + (dst[3] * inv + 127) / 255);
}

bool PointInTriangle(float px, float py, float ax, float ay, float bx, float by, float cx, float cy) {
    float d1 = (px - bx) * (ay - by) - (ax - bx) * (py - by);
    float d2 = (px - cx) * (by - cy) - (bx - cx) * (py - cy);
    float d3 = (px - ax) * (cy - ay) - (cx - ax) * (py - ay);
    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(hasNeg && hasPos);
}

// kind: 0=prev ("<<"), 1=play, 2=pause ("||"), 3=next (">>"). Rasterized
// directly (2x2 supersampled for a soft edge) rather than pulled in through a
// vector library, since it's just four simple shapes and this keeps the mod
// dependency-free for the case where the user hasn't picked their own icons.
void DrawBuiltinGlyph(BYTE* buf, int stride, int originX, int originY, int size, int kind) {
    float margin = size * 0.24f;
    float cy = size * 0.5f;
    float trisize = (size - 2 * margin) * 0.5f;
    BYTE ir = g_settings.mediaIconColorR, ig = g_settings.mediaIconColorG,
         ib = g_settings.mediaIconColorB, ia = g_settings.mediaIconColorA;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float cov = 0.f;
            const int SS = 2;
            for (int sy = 0; sy < SS; sy++) {
                for (int sx = 0; sx < SS; sx++) {
                    float px = x + (sx + 0.5f) / SS;
                    float py = y + (sy + 0.5f) / SS;
                    bool inside = false;
                    switch (kind) {
                        case 1:
                            inside = PointInTriangle(px, py, margin, margin, margin, size - margin,
                                                      size - margin, cy);
                            break;
                        case 2: {
                            float barW = size * 0.16f, gap = size * 0.12f;
                            float b1x0 = size * 0.5f - gap * 0.5f - barW, b1x1 = size * 0.5f - gap * 0.5f;
                            float b2x0 = size * 0.5f + gap * 0.5f, b2x1 = size * 0.5f + gap * 0.5f + barW;
                            bool inBand = (py >= margin && py <= size - margin);
                            inside = inBand && ((px >= b1x0 && px <= b1x1) || (px >= b2x0 && px <= b2x1));
                            break;
                        }
                        case 0:
                            inside = PointInTriangle(px, py, size - margin, margin, size - margin,
                                                      size - margin, size - margin - trisize, cy) ||
                                     PointInTriangle(px, py, size - margin - trisize, margin,
                                                      size - margin - trisize, size - margin,
                                                      size - margin - 2 * trisize, cy);
                            break;
                        case 3:
                        default:
                            inside = PointInTriangle(px, py, margin, margin, margin, size - margin,
                                                      margin + trisize, cy) ||
                                     PointInTriangle(px, py, margin + trisize, margin,
                                                      margin + trisize, size - margin,
                                                      margin + 2 * trisize, cy);
                            break;
                    }
                    if (inside) cov += 1.0f / (SS * SS);
                }
            }
            if (cov <= 0.f) continue;
            float a = (ia / 255.0f) * cov;
            BYTE* p = buf + (size_t)(originY + y) * stride + (size_t)(originX + x) * 4;
            BlendPremultipliedOver(p, (BYTE)std::lround(ib * a), (BYTE)std::lround(ig * a),
                                   (BYTE)std::lround(ir * a), (BYTE)std::lround(a * 255.0f));
        }
    }
}

void BlitIcon(BYTE* buf, int stride, int originX, int originY, int size,
              const std::vector<BYTE>& src) {
    if ((int)src.size() < size * size * 4) return;
    for (int y = 0; y < size; y++) {
        const BYTE* s = src.data() + (size_t)y * size * 4;
        BYTE* d = buf + (size_t)(originY + y) * stride + (size_t)originX * 4;
        for (int x = 0; x < size; x++, s += 4, d += 4) {
            // Composited rather than copied: a straight memcpy would stamp the
            // icon's fully transparent pixels over the backing plate, punching
            // an icon-shaped hole through it instead of letting the plate show
            // through where the icon has nothing.
            BlendPremultipliedOver(d, s[0], s[1], s[2], s[3]);
        }
    }
}

// x/y are the strip's screen position. They're passed in rather than left for
// UpdateLayeredWindow to infer, because its "pass NULL and I'll use the current
// values" shortcuts are only documented as valid when the position and size
// aren't changing -- and here they usually are, since the window is born 1x1
// and every settings change can resize it. Handing it both explicitly is what
// makes the strip reliably appear instead of staying an invisible stub.
// Breathing room between the icon row and the edge of the backing plate. This
// grows the strip window itself rather than shrinking the icons, so raising it
// never makes the controls smaller or harder to hit. Applied whether or not the
// plate is actually visible, so that padding, fill and border stay independent
// of one another -- with the default of 0 that costs existing setups nothing.
int GetMediaPlatePaddingPx() {
    return std::max(0, (int)std::lround(g_settings.mediaPlatePadding * GetMediaControlsDpiScale()));
}

// ---- Media Card (2.1) -------------------------------------------------------------------
// Media Controls > Layout = Card turns the three-button strip into a small
// card, after the Rainmeter media modules:
//   album art   the current track's cover; hover it for prev / play / next
//   seek bar    the track position; click or drag it to seek
//   output      the speaker button lists the outputs, one click switches the
//               Windows default (the visualizer follows if it listens to it)
//   volume      drag the slider, or scroll anywhere on the card
// Its look has its own settings: background, border, corner radius, art
// size, and an accent (for the seek and volume fills) taken from the icon
// colour, a colour of your own, the album art or the Windows accent.
// It is the same layered window as the strip, painted in software, and only
// repainted when something on it changes: hover, a click, a new cover, the
// volume, or the seek bar moving by a whole pixel (checked once a second
// while a track plays).

float VizCardGetVolume();                 // default output, 0..1, or -1
void VizCardSetVolume(float v);
void VizCardOutputMenu(HWND hWnd, POINT screenPt);

constexpr UINT_PTR kCardTimer = 0x7701;

bool VizCardActive() { return g_settings.mediaCard; }

struct VizCardGeom {
    int pad, tile, gap, progY, progH, rowY, rowH, w, h;
    float dpi;
};

VizCardGeom VizCardLayout() {
    VizCardGeom g;
    g.dpi = GetMediaControlsDpiScale();
    int s = std::max(1, (int)std::lround(g_settings.mediaIconSize * g.dpi));
    int sp = std::max(0, (int)std::lround(g_settings.mediaIconSpacing * g.dpi));
    g.pad = std::max(GetMediaPlatePaddingPx(), (int)std::lround(8 * g.dpi));
    g.tile = g_settings.cardArtSize > 0 ? std::max(24, (int)std::lround(g_settings.cardArtSize * g.dpi)) : s * 3 + sp * 2;
    g.gap = std::max(4, (int)std::lround(7 * g.dpi));
    g.progH = std::max(3, (int)std::lround(3 * g.dpi));
    g.progY = g.pad + g.tile + g.gap;
    g.rowH = std::max((int)std::lround(18 * g.dpi), (int)std::lround(s * 0.6f));
    g.rowY = g.progY + g.progH + g.gap;
    g.w = g.tile + g.pad * 2;
    g.h = g.rowY + g.rowH + g.pad;
    return g;
}

void VizCardSize(int* w, int* h) {
    VizCardGeom g = VizCardLayout();
    *w = g.w;
    *h = g.h;
}

namespace {
enum CardPart { kPartNone, kPartArt, kPartSeek, kPartSpeaker, kPartVolume };
bool s_cardHover = false, s_cardTracking = false;
int s_cardDrag = kPartNone;
int s_cardHoverPart = kPartNone;
float s_cardSeekPreview = -1.f;  // while dragging the seek bar
float s_cardVolume = -1.f;
int s_cardProgPx = -1;

// Straight-alpha colour over the premultiplied buffer, with coverage.
// A control glyph on the card: the custom icon for that slot when one is set
// (Media Controls > icon paths), resampled to the card's size, else the
// built-in shape. Slots: 0 previous, 1 play, 2 pause, 3 next.
void CardGlyph(BYTE* buf, int stride, int x, int y, int s, int slot) {
    const auto& src = g_mediaIconPixels[slot];
    const int n = g_mediaIconLoadedSize;
    if (n <= 0 || s <= 0 || (int)src.size() < n * n * 4) {
        DrawBuiltinGlyph(buf, stride, x, y, s, slot);
        return;
    }
    const float k = (float)n / (float)s;
    for (int dy = 0; dy < s; dy++)
        for (int dx = 0; dx < s; dx++) {
            float acc[4] = {0, 0, 0, 0};
            if (k > 1.f) {
                // Shrinking: average the source pixels this one covers.
                int x0 = (int)(dx * k), x1 = std::min(n, std::max(x0 + 1, (int)ceilf((dx + 1) * k)));
                int y0 = (int)(dy * k), y1 = std::min(n, std::max(y0 + 1, (int)ceilf((dy + 1) * k)));
                for (int sy = y0; sy < y1; sy++)
                    for (int sx = x0; sx < x1; sx++)
                        for (int c = 0; c < 4; c++) acc[c] += src[((size_t)sy * n + sx) * 4 + c];
                float inv = 1.f / (float)((x1 - x0) * (y1 - y0));
                for (float& v : acc) v *= inv;
            } else {
                // Growing: bilinear.
                float u = (dx + 0.5f) * k - 0.5f, v = (dy + 0.5f) * k - 0.5f;
                int x0 = std::clamp((int)floorf(u), 0, n - 1), y0 = std::clamp((int)floorf(v), 0, n - 1);
                int x1 = std::min(x0 + 1, n - 1), y1 = std::min(y0 + 1, n - 1);
                float fx = std::clamp(u - x0, 0.f, 1.f), fy = std::clamp(v - y0, 0.f, 1.f);
                for (int c = 0; c < 4; c++) {
                    auto px = [&](int xx, int yy) { return (float)src[((size_t)yy * n + xx) * 4 + c]; };
                    acc[c] = (px(x0, y0) * (1 - fx) + px(x1, y0) * fx) * (1 - fy) +
                             (px(x0, y1) * (1 - fx) + px(x1, y1) * fx) * fy;
                }
            }
            BlendPremultipliedOver(buf + (size_t)(y + dy) * stride + (size_t)(x + dx) * 4, (BYTE)lroundf(acc[0]),
                                   (BYTE)lroundf(acc[1]), (BYTE)lroundf(acc[2]), (BYTE)lroundf(acc[3]));
        }
}

void CardBlend(BYTE* p, float r, float g, float b, float a) {
    if (a <= 0.f) return;
    BlendPremultipliedOver(p, (BYTE)std::lround(b * a * 255.f), (BYTE)std::lround(g * a * 255.f),
                           (BYTE)std::lround(r * a * 255.f), (BYTE)std::lround(a * 255.f));
}

// Coverage of a pixel by a rounded rect (signed distance, one pixel of AA).
float CardRoundCov(float px, float py, float l, float t, float r, float b, float rad) {
    float cx = (l + r) * 0.5f, cy = (t + b) * 0.5f, hx = (r - l) * 0.5f, hy = (b - t) * 0.5f;
    rad = std::min({rad, hx, hy});
    float qx = fabsf(px - cx) - hx + rad, qy = fabsf(py - cy) - hy + rad;
    float d = sqrtf(std::max(qx, 0.f) * std::max(qx, 0.f) + std::max(qy, 0.f) * std::max(qy, 0.f)) +
              std::min(std::max(qx, qy), 0.f) - rad;
    return std::clamp(0.5f - d, 0.f, 1.f);
}

void CardFillRound(BYTE* buf, int stride, int W, int H, float l, float t, float r, float b, float rad, float cr,
                   float cg, float cb, float ca) {
    int x0 = std::max(0, (int)floorf(l)), x1 = std::min(W, (int)ceilf(r));
    int y0 = std::max(0, (int)floorf(t)), y1 = std::min(H, (int)ceilf(b));
    for (int y = y0; y < y1; y++)
        for (int x = x0; x < x1; x++) {
            float cov = CardRoundCov(x + 0.5f, y + 0.5f, l, t, r, b, rad);
            if (cov > 0.f) CardBlend(buf + (size_t)y * stride + (size_t)x * 4, cr, cg, cb, ca * cov);
        }
}

// A small speaker: body, cone, and one or two sound waves by volume.
void CardSpeaker(BYTE* buf, int stride, int ox, int oy, int sz, float vol, float ir, float ig, float ib, float ia) {
    float s = (float)sz;
    for (int y = 0; y < sz; y++)
        for (int x = 0; x < sz; x++) {
            float cov = 0.f;
            for (int k = 0; k < 4; k++) {
                float px = x + ((k & 1) + 0.5f) / 2.f, py = y + ((k >> 1) + 0.5f) / 2.f;
                float u = px / s, v = py / s;
                bool in = (u >= 0.14f && u <= 0.3f && v >= 0.38f && v <= 0.62f) ||
                          PointInTriangle(px, py, s * 0.3f, s * 0.38f, s * 0.3f, s * 0.62f, s * 0.5f, s * 0.2f) ||
                          PointInTriangle(px, py, s * 0.3f, s * 0.62f, s * 0.5f, s * 0.8f, s * 0.5f, s * 0.2f);
                float dx = u - 0.5f, dy = v - 0.5f, rr = sqrtf(dx * dx + dy * dy);
                bool arcs = dx > 0.05f && fabsf(dy) < dx * 1.1f;
                if (arcs && vol > 0.01f && fabsf(rr - 0.2f) < 0.035f) in = true;
                if (arcs && vol > 0.5f && fabsf(rr - 0.33f) < 0.035f) in = true;
                if (in) cov += 0.25f;
            }
            if (cov > 0.f) CardBlend(buf + (size_t)(oy + y) * stride + (size_t)(ox + x) * 4, ir, ig, ib, ia * cov);
        }
}

int CardHit(const VizCardGeom& g, int x, int y) {
    int slack = (int)std::lround(5 * g.dpi);
    if (x >= g.pad && x < g.pad + g.tile && y >= g.pad && y < g.pad + g.tile) return kPartArt;
    if (x >= g.pad && x < g.pad + g.tile && y >= g.progY - slack && y < g.progY + g.progH + slack) return kPartSeek;
    if (y >= g.rowY && y < g.rowY + g.rowH) {
        if (x >= g.pad && x < g.pad + g.rowH) return kPartSpeaker;
        if (x >= g.pad + g.rowH && x < g.pad + g.tile + slack) return kPartVolume;
    }
    return kPartNone;
}

float CardSliderFrac(const VizCardGeom& g, int x) {
    float l = (float)(g.pad + g.rowH + g.gap), r = (float)(g.pad + g.tile) - 6.f * g.dpi;
    return std::clamp((x - l) / std::max(1.f, r - l), 0.f, 1.f);
}

void CardRepaint() {
    if (g_mediaWnd) PostMessage(g_mediaWnd, WM_APP_MEDIA_REPAINT, 0, 0);
}

// Seek to a fraction of the track, through the media command thread.
void CardSeek(float frac) {
    int64_t start = g_tlStart.load(std::memory_order_relaxed), end = g_tlEnd.load(std::memory_order_relaxed);
    if (!g_tlValid.load(std::memory_order_relaxed) || end <= start) return;
    g_mediaSeekTicks.store(start + (int64_t)((double)(end - start) * std::clamp(frac, 0.f, 1.f)),
                           std::memory_order_relaxed);
    SendMediaCommand(3);
}
}  // namespace

void VizPaintCard(BYTE* buf, int stride, int W, int H) {
    VizCardGeom g = VizCardLayout();
    const float ir = g_settings.mediaIconColorR / 255.f, ig = g_settings.mediaIconColorG / 255.f,
                ib = g_settings.mediaIconColorB / 255.f, ia = g_settings.mediaIconColorA / 255.f;
    // Accent: the seek and volume fills and their knobs.
    float ar = ir, ag = ig, ab = ib, aa = ia;
    if (g_settings.cardAccentSource == 1) {
        ar = g_settings.cardAccentR / 255.f;
        ag = g_settings.cardAccentG / 255.f;
        ab = g_settings.cardAccentB / 255.f;
        aa = g_settings.cardAccentA / 255.f;
    } else if (g_settings.cardAccentSource >= 2) {
        DWORD dw = g_settings.cardAccentSource == 2 ? g_albumArtColor.load(std::memory_order_relaxed)
                                                    : GetWindowsAccentColor();
        ar = ((dw >> 16) & 0xFF) / 255.f;
        ag = ((dw >> 8) & 0xFF) / 255.f;
        ab = (dw & 0xFF) / 255.f;
        aa = 1.f;
    }
    // Background, then the border drawn as a ring inside the edge. The
    // background is never fully transparent: a layered window lets clicks
    // through pixels with zero alpha, and the card should take every click
    // inside it.
    const float cardR = g_settings.cardRadius * g.dpi;
    CardFillRound(buf, stride, W, H, 0.f, 0.f, (float)W, (float)H, cardR, g_settings.cardBgR / 255.f,
                  g_settings.cardBgG / 255.f, g_settings.cardBgB / 255.f, std::max(1, (int)g_settings.cardBgA) / 255.f);
    if (g_settings.cardBorderSize > 0 && g_settings.cardBorderA > 0) {
        float bw = std::min(g_settings.cardBorderSize * g.dpi, std::min(W, H) * 0.5f);
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++) {
                float px = x + 0.5f, py = y + 0.5f;
                float ring = CardRoundCov(px, py, 0.f, 0.f, (float)W, (float)H, cardR) -
                             CardRoundCov(px, py, bw, bw, W - bw, H - bw, std::max(0.f, cardR - bw));
                if (ring > 0.f)
                    CardBlend(buf + (size_t)y * stride + (size_t)x * 4, g_settings.cardBorderR / 255.f,
                              g_settings.cardBorderG / 255.f, g_settings.cardBorderB / 255.f,
                              g_settings.cardBorderA / 255.f * ring);
            }
    }

    // Album art, scaled bilinearly into the tile with rounded corners.
    const float tl = (float)g.pad, tt = (float)g.pad, ts = (float)g.tile;
    const float rad = std::max(0.f, cardR - g.pad * 0.5f);  // follows the card's corners
    bool art = false;
    {
        std::lock_guard<std::mutex> lock(g_artTileMutex);
        if (!g_artTile.empty() && g_artTileW > 0 && g_artTileH > 0) {
            art = true;
            const int aw = g_artTileW, ah = g_artTileH;
            for (int y = 0; y < g.tile; y++)
                for (int x = 0; x < g.tile; x++) {
                    float cov = CardRoundCov(tl + x + 0.5f, tt + y + 0.5f, tl, tt, tl + ts, tt + ts, rad);
                    if (cov <= 0.f) continue;
                    float u = (x + 0.5f) / ts * aw - 0.5f, v = (y + 0.5f) / ts * ah - 0.5f;
                    int x0 = std::clamp((int)floorf(u), 0, aw - 1), y0 = std::clamp((int)floorf(v), 0, ah - 1);
                    int x1 = std::min(x0 + 1, aw - 1), y1 = std::min(y0 + 1, ah - 1);
                    float fx = std::clamp(u - x0, 0.f, 1.f), fy = std::clamp(v - y0, 0.f, 1.f);
                    float c[4];
                    for (int k = 0; k < 4; k++) {
                        auto px = [&](int xx, int yy) { return g_artTile[((size_t)yy * aw + xx) * 4 + k] / 255.f; };
                        c[k] = (px(x0, y0) * (1 - fx) + px(x1, y0) * fx) * (1 - fy) +
                               (px(x0, y1) * (1 - fx) + px(x1, y1) * fx) * fy;
                    }
                    CardBlend(buf + (size_t)(g.pad + y) * stride + (size_t)(g.pad + x) * 4, c[2], c[1], c[0], c[3] * cov);
                }
        }
    }
    if (!art) CardFillRound(buf, stride, W, H, tl, tt, tl + ts, tt + ts, rad, 1.f, 1.f, 1.f, 0.06f);

    // Controls over the art: on hover, or always when there is no cover.
    if (!art || s_cardHoverPart == kPartArt) {
        if (art) CardFillRound(buf, stride, W, H, tl, tt, tl + ts, tt + ts, rad, 0.f, 0.f, 0.f, 0.45f);
        int gs = std::max(8, g.tile / 4);
        int gy = g.pad + (g.tile - gs) / 2;
        bool playing = g_mediaIsPlaying.load(std::memory_order_relaxed);
        int slot = g.tile / 3;
        CardGlyph(buf, stride, g.pad + (slot - gs) / 2, gy, gs, 0);
        CardGlyph(buf, stride, g.pad + slot + (slot - gs) / 2, gy, gs, playing ? 2 : 1);
        CardGlyph(buf, stride, g.pad + 2 * slot + (slot - gs) / 2, gy, gs, 3);
    }

    // Seek bar.
    float prog = s_cardSeekPreview >= 0.f ? s_cardSeekPreview : VizTrackProgress();
    bool seekHot = s_cardHoverPart == kPartSeek || s_cardDrag == kPartSeek;
    float ph = seekHot ? g.progH + 2.f * g.dpi : (float)g.progH;
    float py = g.progY + g.progH * 0.5f - ph * 0.5f;
    CardFillRound(buf, stride, W, H, tl, py, tl + ts, py + ph, ph * 0.5f, ir, ig, ib, ia * 0.22f);
    if (prog >= 0.f) {
        float px = tl + ts * std::clamp(prog, 0.f, 1.f);
        CardFillRound(buf, stride, W, H, tl, py, std::max(px, tl + ph), py + ph, ph * 0.5f, ar, ag, ab, aa * 0.9f);
        if (seekHot) {
            float kr = ph * 1.1f;
            CardFillRound(buf, stride, W, H, px - kr, py + ph * 0.5f - kr, px + kr, py + ph * 0.5f + kr, kr, ar, ag, ab, aa);
        }
        s_cardProgPx = (int)lroundf(ts * std::clamp(prog, 0.f, 1.f));
    }

    // Output button and volume slider.
    s_cardVolume = VizCardGetVolume();
    int spk = g.rowH;
    float spA = s_cardHoverPart == kPartSpeaker ? ia : ia * 0.8f;
    CardSpeaker(buf, stride, g.pad, g.rowY, spk, std::max(0.f, s_cardVolume), ir, ig, ib, spA);
    if (s_cardVolume >= 0.f) {
        float l = (float)(g.pad + g.rowH + g.gap), r = (float)(g.pad + g.tile) - 6.f * g.dpi;
        float cy = g.rowY + g.rowH * 0.5f, th = std::max(2.f, 3.f * g.dpi);
        CardFillRound(buf, stride, W, H, l, cy - th * 0.5f, r, cy + th * 0.5f, th * 0.5f, ir, ig, ib, ia * 0.22f);
        float kx = l + (r - l) * s_cardVolume;
        CardFillRound(buf, stride, W, H, l, cy - th * 0.5f, std::max(kx, l + th), cy + th * 0.5f, th * 0.5f, ar, ag, ab, aa * 0.9f);
        float kr = (s_cardHoverPart == kPartVolume || s_cardDrag == kPartVolume) ? 6.f * g.dpi : 4.5f * g.dpi;
        CardFillRound(buf, stride, W, H, kx - kr, cy - kr, kx + kr, cy + kr, kr, ar, ag, ab, aa);
    }
}

// Mouse and timer messages for the card. Returns true when handled.
bool VizCardMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    VizCardGeom g = VizCardLayout();
    int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
    switch (msg) {
        case WM_MOUSEMOVE: {
            if (!s_cardTracking) {
                TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
                s_cardTracking = TrackMouseEvent(&tme) != FALSE;
            }
            s_cardHover = true;
            int part = s_cardDrag != kPartNone ? s_cardDrag : CardHit(g, x, y);
            bool repaint = part != s_cardHoverPart;
            s_cardHoverPart = part;
            if (s_cardDrag == kPartVolume) {
                VizCardSetVolume(CardSliderFrac(g, x));
                repaint = true;
            } else if (s_cardDrag == kPartSeek) {
                s_cardSeekPreview = std::clamp((x - g.pad) / (float)std::max(1, g.tile), 0.f, 1.f);
                repaint = true;
            }
            if (repaint) CardRepaint();
            return true;
        }
        case WM_MOUSELEAVE:
            s_cardTracking = false;
            s_cardHover = false;
            if (s_cardDrag == kPartNone && s_cardHoverPart != kPartNone) {
                s_cardHoverPart = kPartNone;
                CardRepaint();
            }
            return true;
        case WM_LBUTTONDOWN: {
            int part = CardHit(g, x, y);
            if (part == kPartSeek || part == kPartVolume) {
                s_cardDrag = part;
                SetCapture(hWnd);
                if (part == kPartVolume) VizCardSetVolume(CardSliderFrac(g, x));
                else s_cardSeekPreview = std::clamp((x - g.pad) / (float)std::max(1, g.tile), 0.f, 1.f);
                CardRepaint();
            }
            return true;
        }
        case WM_LBUTTONUP: {
            int drag = s_cardDrag;
            s_cardDrag = kPartNone;
            if (GetCapture() == hWnd) ReleaseCapture();
            if (drag == kPartSeek) {
                CardSeek(s_cardSeekPreview);
                s_cardSeekPreview = -1.f;
                CardRepaint();
                return true;
            }
            if (drag == kPartVolume) {
                CardRepaint();
                return true;
            }
            int part = CardHit(g, x, y);
            if (part == kPartArt) {
                int third = std::clamp((x - g.pad) * 3 / std::max(1, g.tile), 0, 2);
                SendMediaCommand(third);
            } else if (part == kPartSpeaker) {
                POINT pt;
                GetCursorPos(&pt);
                VizCardOutputMenu(hWnd, pt);
                CardRepaint();
            }
            return true;
        }
        case WM_MOUSEWHEEL: {
            float v = VizCardGetVolume();
            if (v >= 0.f) {
                VizCardSetVolume(v + 0.02f * (GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA));
                CardRepaint();
            }
            return true;
        }
        case WM_TIMER:
            if (wParam != kCardTimer) return false;
            // Repaint only when the seek bar moved by a pixel, or the volume
            // changed from elsewhere (the volume flyout, a keyboard key).
            if (g_mediaIsPlaying.load(std::memory_order_relaxed) && s_cardDrag == kPartNone) {
                float p = VizTrackProgress();
                if (p >= 0.f && (int)lroundf(g.tile * std::clamp(p, 0.f, 1.f)) != s_cardProgPx) CardRepaint();
            }
            if (fabsf(VizCardGetVolume() - s_cardVolume) > 0.004f) CardRepaint();
            return true;
    }
    return false;
}

// The card's once-a-second check runs only while the card is showing.
void VizCardTimer(HWND hWnd, bool on) {
    static bool s_on = false;
    if (on == s_on || !hWnd) return;
    s_on = on;
    if (on) SetTimer(hWnd, kCardTimer, 1000, nullptr);
    else KillTimer(hWnd, kCardTimer);
}

void PaintMediaControls(int x, int y, int width, int height) {
    if (!g_mediaWnd || width <= 0 || height <= 0) return;

    HDC screenDC = GetDC(nullptr);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP dib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screenDC);
    if (!dib || !bits) {
        if (dib) DeleteObject(dib);
        DeleteDC(memDC);
        return;
    }

    memset(bits, 0, (size_t)width * height * 4);
    BYTE* buf = (BYTE*)bits;
    int stride = width * 4;

    if (VizCardActive()) {
        VizPaintCard(buf, stride, width, height);
    } else {
    // Optional backing plate behind the whole strip, with an optional outline.
    // v0.5.2 drew a plate unconditionally as a fix for pale icons vanishing
    // against a pale wallpaper, which meant icons that were meant to sit
    // transparently on the desktop always had a dark box behind them. Both
    // pieces are settings now and both default to nothing.
    if (g_settings.mediaPlateA > 0 ||
        (g_settings.mediaPlateBorderSize > 0 && g_settings.mediaPlateBorderA > 0)) {
        float dpiScale = GetMediaControlsDpiScale();
        float radius = std::min({(float)std::max(0.f, g_settings.mediaPlateCornerRadius) * dpiScale,
                                 width * 0.5f, height * 0.5f});
        // Border draws inward from the edge, so it can never be thicker than
        // half the strip without the two sides meeting in the middle.
        float bw = std::min({(float)std::max(0.f, g_settings.mediaPlateBorderSize) * dpiScale,
                             width * 0.5f, height * 0.5f});
        bool wantBorder = bw > 0.f && g_settings.mediaPlateBorderA > 0;

        float innerRadius = std::max(0.0f, radius - bw);
        float fillAlpha = g_settings.mediaPlateA / 255.0f;
        float borderAlpha = g_settings.mediaPlateBorderA / 255.0f;

        // Coverage of a rounded rect inset by `inset` on every side: clamp the
        // sample into the rect shrunk by its corner radius, and whatever
        // distance is left over is the distance to the nearest corner circle's
        // centre (zero anywhere along the straight-edged middle).
        auto insideRounded = [&](float px, float py, float inset, float r) {
            float l = inset, t = inset, rt = width - inset, b = height - inset;
            if (px < l || py < t || px > rt || py > b) return false;
            float nx = std::clamp(px, l + r, rt - r);
            float ny = std::clamp(py, t + r, b - r);
            float dx = px - nx, dy = py - ny;
            return dx * dx + dy * dy <= r * r;
        };

        for (int y = 0; y < height; y++) {
            BYTE* row = buf + (size_t)y * stride;
            for (int x = 0; x < width; x++) {
                // 2x2 supersampled, matching how the built-in glyphs are
                // rasterized, so rounded corners don't come out jagged.
                float outerCov = 0.f, innerCov = 0.f;
                const int SS = 2;
                for (int sy = 0; sy < SS; sy++) {
                    for (int sx = 0; sx < SS; sx++) {
                        float px = x + (sx + 0.5f) / SS;
                        float py = y + (sy + 0.5f) / SS;
                        if (insideRounded(px, py, 0.f, radius)) outerCov += 1.0f / (SS * SS);
                        if (wantBorder && insideRounded(px, py, bw, innerRadius))
                            innerCov += 1.0f / (SS * SS);
                    }
                }
                if (outerCov <= 0.f) continue;

                BYTE* p = row + (size_t)x * 4;

                // Fill covers the whole plate including under the border, so a
                // translucent border blends over the fill rather than cutting a
                // hole in it.
                if (g_settings.mediaPlateA > 0) {
                    float a = fillAlpha * outerCov;
                    p[0] = (BYTE)std::lround(g_settings.mediaPlateB * a);
                    p[1] = (BYTE)std::lround(g_settings.mediaPlateG * a);
                    p[2] = (BYTE)std::lround(g_settings.mediaPlateR * a);
                    p[3] = (BYTE)std::lround(a * 255.0f);
                }

                if (wantBorder) {
                    float ringCov = std::max(0.0f, outerCov - innerCov);
                    if (ringCov > 0.f) {
                        float a = borderAlpha * ringCov;
                        BlendPremultipliedOver(
                            p, (BYTE)std::lround(g_settings.mediaPlateBorderB * a),
                            (BYTE)std::lround(g_settings.mediaPlateBorderG * a),
                            (BYTE)std::lround(g_settings.mediaPlateBorderR * a),
                            (BYTE)std::lround(a * 255.0f));
                    }
                }
            }
        }
    }

    // The icons occupy the content box -- the window inset by the plate padding
    // -- not the whole window.
    int pad = GetMediaPlatePaddingPx();
    int contentW = std::max(1, width  - pad * 2);
    int contentH = std::max(1, height - pad * 2);

    // Custom icons are rasterized once at a fixed size, so they can only be
    // blitted at exactly that size; anything else would read the source with the
    // wrong stride and come out scrambled. If a settings change has left them
    // out of step with the box they now have to fit in, fall back to the
    // built-in glyphs for this frame -- the reload that follows will restore
    // them at the right size.
    bool iconsFit = g_mediaIconLoadedSize > 0 && g_mediaIconLoadedSize <= contentH &&
                    g_mediaIconLoadedSize * 3 <= contentW;
    int size = iconsFit ? g_mediaIconLoadedSize : contentH;
    int spacing = std::max(0, (contentW - size * 3) / 2);
    int xPrev = pad;
    int xPlay = pad + size + spacing;
    int xNext = pad + 2 * size + 2 * spacing;
    int iconY = pad + (contentH - size) / 2;

    bool playing = g_mediaIsPlaying.load(std::memory_order_relaxed);

    if (iconsFit && !g_mediaIconPixels[0].empty())
        BlitIcon(buf, stride, xPrev, iconY, size, g_mediaIconPixels[0]);
    else DrawBuiltinGlyph(buf, stride, xPrev, iconY, size, 0);

    const auto& playPauseSrc = playing ? g_mediaIconPixels[2] : g_mediaIconPixels[1];
    if (iconsFit && !playPauseSrc.empty())
        BlitIcon(buf, stride, xPlay, iconY, size, playPauseSrc);
    else DrawBuiltinGlyph(buf, stride, xPlay, iconY, size, playing ? 2 : 1);

    if (iconsFit && !g_mediaIconPixels[3].empty())
        BlitIcon(buf, stride, xNext, iconY, size, g_mediaIconPixels[3]);
    else DrawBuiltinGlyph(buf, stride, xNext, iconY, size, 3);

    }  // strip

    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, dib);

    POINT ptSrc = {0, 0};
    POINT ptDst = {x, y};
    SIZE  szWnd = {width, height};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    BOOL ulwOk = UpdateLayeredWindow(g_mediaWnd, nullptr, &ptDst, &szWnd, memDC, &ptSrc, 0,
                                     &blend, ULW_ALPHA);
    if (!ulwOk) {
        Wh_Log(L"[Media] UpdateLayeredWindow failed, error=%lu xywh=(%d,%d,%d,%d)",
               GetLastError(), x, y, width, height);
    }

    SelectObject(memDC, oldBmp);
    DeleteObject(dib);
    DeleteDC(memDC);
}

// Set while the strip is being kept out of the way by Hide When Covered, so a
// repaint triggered by something else (a track change, a playback state flip)
// doesn't pop it back on screen over the window it just got out from under.
bool g_mediaHiddenByCover = false;

void RepositionAndRepaintMediaControls() {
    if (!g_mediaWnd) return;
    if (g_overlayInPanel && g_panelHidden) {
        VizCardTimer(g_mediaWnd, false);
        ShowWindow(g_mediaWnd, SW_HIDE);
        return;
    }
    if (!g_settings.mediaControlsEnabled) {
        VizCardTimer(g_mediaWnd, false);
        ShowWindow(g_mediaWnd, SW_HIDE);
        return;
    }
    VizCardTimer(g_mediaWnd, VizCardActive());
    if (g_mediaHiddenByCover && g_settings.mediaHideWhenCovered) return;

    float dpiScale = GetMediaControlsDpiScale();
    int size = std::max(1, (int)std::lround(g_settings.mediaIconSize * dpiScale));
    int spacing = std::max(0, (int)std::lround(g_settings.mediaIconSpacing * dpiScale));
    int pad = GetMediaPlatePaddingPx();
    int width = size * 3 + spacing * 2 + pad * 2;
    int height = size + pad * 2;
    if (VizCardActive()) VizCardSize(&width, &height);

    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return;

    int workWidth = mi.rcWork.right - mi.rcWork.left;
    int workHeight = mi.rcWork.bottom - mi.rcWork.top;

    bool override_ = g_mediaOverrideActive.load(std::memory_order_relaxed);
    float hPercent = override_ ? g_mediaOverrideH.load(std::memory_order_relaxed)
                               : g_settings.mediaHorizontalPosition;
    float vPercent = override_ ? g_mediaOverrideV.load(std::memory_order_relaxed)
                               : g_settings.mediaVerticalPosition;

    int x = mi.rcWork.left + (int)std::lround((workWidth - width) * (hPercent / 100.0f));
    int y = mi.rcWork.top + (int)std::lround((workHeight - height) * (vPercent / 100.0f));

    // Anchored to the visualizer's panel (2.0): a corner of the panel as last
    // drawn, inset by the anchor offsets, replacing the position above. The
    // panel publishes its rect every frame and re-posts this when it moves.
    if (g_settings.mediaAnchor != VizMediaAnchor::Screen && g_drawRectValid.load(std::memory_order_relaxed)) {
        LONG pl = g_drawRectL.load(std::memory_order_relaxed), pt = g_drawRectT.load(std::memory_order_relaxed);
        LONG pr = g_drawRectR.load(std::memory_order_relaxed), pb = g_drawRectB.load(std::memory_order_relaxed);
        int ox = (int)std::lround(g_settings.mediaAnchorOffsetX * dpiScale);
        int oy = (int)std::lround(g_settings.mediaAnchorOffsetY * dpiScale);
        bool right = g_settings.mediaAnchor == VizMediaAnchor::PanelTopRight ||
                     g_settings.mediaAnchor == VizMediaAnchor::PanelBottomRight;
        bool bottom = g_settings.mediaAnchor == VizMediaAnchor::PanelBottomLeft ||
                      g_settings.mediaAnchor == VizMediaAnchor::PanelBottomRight;
        x = right ? (int)pr - ox - width : (int)pl + ox;
        y = bottom ? (int)pb - oy - height : (int)pt + oy;
    }

    Wh_Log(L"[Media] Reposition xywh=(%d,%d,%d,%d) work=(%ld,%ld,%ld,%ld) monitor=%d dpiScale=%.2f",
           x, y, width, height, mi.rcWork.left, mi.rcWork.top, mi.rcWork.right, mi.rcWork.bottom,
           g_settings.monitor, dpiScale);

    // Paint before showing: UpdateLayeredWindow sets the position, size and
    // content in one shot, so by the time the window is shown there's already
    // something in it. A layered window that has never been given content is
    // invisible, which is indistinguishable from "the mod isn't working."
    PaintMediaControls(x, y, width, height);

    // Stacking: only touch the z-order when it is wrong. Raising it on every
    // repaint put the card back over its own open menus.
    const bool isTop = (GetWindowLongPtr(g_mediaWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    const RECT want{x, y, x + width, y + height};
    const bool overTaskbar = VizTaskbarOverlaps(want, false);
    const bool wantTop = (g_settings.mediaOnTop || overTaskbar) && !g_menuOpen.load(std::memory_order_acquire);
    const bool buried = wantTop && overTaskbar && VizTaskbarOverlaps(want, true);
    UINT zFlags = (isTop == wantTop && !buried) ? SWP_NOZORDER : 0;
    BOOL posOk = SetWindowPos(g_mediaWnd, wantTop ? HWND_TOPMOST : HWND_NOTOPMOST, x, y, width, height,
                              SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOOWNERZORDER | zFlags);

    RECT actualRect{};
    GetWindowRect(g_mediaWnd, &actualRect);
    Wh_Log(L"[Media] SetWindowPos ok=%d actualRect=(%ld,%ld,%ld,%ld) visible=%d topmost=%d",
           (int)posOk, actualRect.left, actualRect.top, actualRect.right, actualRect.bottom,
           (int)IsWindowVisible(g_mediaWnd),
           (int)((GetWindowLongPtr(g_mediaWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0));
}

// While one of the mod's menus is open the strip / card leaves the topmost band
// (menus are topmost too, and the later raise would win), then returns.
void VizMediaMenuBegin() {
    g_menuOpen.store(true, std::memory_order_release);
    if (g_mediaWnd && (GetWindowLongPtr(g_mediaWnd, GWL_EXSTYLE) & WS_EX_TOPMOST))
        SetWindowPos(g_mediaWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}
void VizMediaMenuEnd() {
    g_menuOpen.store(false, std::memory_order_release);
    if (g_mediaWnd && g_settings.mediaOnTop && IsWindowVisible(g_mediaWnd))
        SetWindowPos(g_mediaWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

LRESULT CALLBACK MediaWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (VizCardActive() && VizCardMessage(hWnd, uMsg, wParam, lParam)) return 0;
    switch (uMsg) {
        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam);
            float dpiScale = GetMediaControlsDpiScale();
            int size = std::max(1, (int)std::lround(g_settings.mediaIconSize * dpiScale));
            int spacing = std::max(0, (int)std::lround(g_settings.mediaIconSpacing * dpiScale));
            // The icons are inset by the plate padding (see PaintMediaControls),
            // so the hit test has to start from there as well. Testing from 0
            // instead puts every button's clickable area `pad` pixels left of
            // where the button is actually drawn -- and once padding exceeds the
            // gap between icons, that lands on the wrong button entirely.
            int pad = GetMediaPlatePaddingPx();

            int cmd = -1;
            if (x >= pad && x < pad + size) cmd = 0;
            else if (x >= pad + size + spacing && x < pad + 2 * size + spacing) cmd = 1;
            else if (x >= pad + 2 * size + 2 * spacing &&
                     x < pad + 3 * size + 2 * spacing) cmd = 2;

            if (cmd >= 0) SendMediaCommand(cmd);
            return 0;
        }

        case WM_APP_MEDIA_REPAINT:
            RepositionAndRepaintMediaControls();
            return 0;

        // Right-click on the strip opens the same quick-settings menu as the
        // visualizer (handy when the visualizer is paused from it), with the
        // same Ctrl requirement when Right-Click Menu is Ctrl + Right-Click.
        case WM_RBUTTONUP:
            if (g_settings.contextMenu == VizContextMenu::CtrlRightClick && !(wParam & MK_CONTROL)) break;
            if (g_messageWnd && g_settings.contextMenu != VizContextMenu::Off) {
                POINT pt;
                GetCursorPos(&pt);
                PostMessage(g_messageWnd, WM_APP_CONTEXT_MENU, (WPARAM)pt.x, (LPARAM)pt.y);
            }
            return 0;

        case WM_DESTROY:
            g_mediaWnd = nullptr;
            return 0;

        case WM_APP_CLEANUP:
            DestroyWindow(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool g_mediaClassRegistered = false;

bool RegisterMediaWindowClass() {
    if (g_mediaClassRegistered) return true;
    WNDCLASS wc = {};
    wc.lpfnWndProc = MediaWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = MEDIA_WINDOW_CLASS;
    wc.hCursor = LoadCursor(nullptr, IDC_HAND);
    if (!RegisterClass(&wc)) return false;
    g_mediaClassRegistered = true;
    return true;
}

void UnregisterMediaWindowClass() {
    if (g_mediaClassRegistered) {
        UnregisterClass(MEDIA_WINDOW_CLASS, GetCurrentModuleHandle());
        g_mediaClassRegistered = false;
    }
}

void CreateMediaControlWindow() {
    if (g_mediaWnd) return;
    if (!RegisterMediaWindowClass()) {
        Wh_Log(L"[Media] RegisterMediaWindowClass failed, error %u", GetLastError());
        return;
    }

    HINSTANCE hInstance = GetCurrentModuleHandle();
    g_mediaWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        MEDIA_WINDOW_CLASS, nullptr, WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInstance, nullptr);
    if (!g_mediaWnd) {
        Wh_Log(L"[Media] CreateWindowEx failed, error %u", GetLastError());
        return;
    }
    Wh_Log(L"[Media] Window created hwnd=%p enabled=%d iconSize=%.2f spacing=%.2f pos=(%.2f,%.2f) monitor=%d",
           (void*)g_mediaWnd, (int)g_settings.mediaControlsEnabled, (double)g_settings.mediaIconSize,
           (double)g_settings.mediaIconSpacing, g_settings.mediaHorizontalPosition,
           g_settings.mediaVerticalPosition, g_settings.monitor);

    RecreateMediaControlResources();
    RepositionAndRepaintMediaControls();
}

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

// ---- Precision analysis core (Analysis Engine = Precision) ---------------------
//
// Everything in here is plain C++ with no Windows dependency, so it can be
// compiled and tested on its own against a reference implementation. The mod
// pastes it in verbatim.
//
// What it replaces, and why. The 1.4 analysis ran one complex FFT over real
// data, summed it into 7 fixed bands, and then every bar interpolated between
// those 7 numbers. Bar Count and FFT Size changed how many bars were drawn,
// not how much the bars knew. This core gives every bar its own frequency band
// with its own edges, measured straight from the spectrum:
//
//   * a real-input FFT (an N/2-point complex FFT plus a post-twiddle), which
//     is half the work of running a complex FFT on real data;
//   * three resolution tiers: the signal as captured, decimated by 4 and
//     decimated by 16, each analysed with the same FFT size. Bass bands come
//     from the long, finely resolved windows, treble from the short, fast ones,
//     so a 1/24-octave bar at 50 Hz is a real measurement instead of a smear;
//   * band layouts on log / linear / mel / Bark / ERB scales, IEC 61260-1
//     fractional-octave bands, or equal-tempered musical notes;
//   * A / C / Z weighting (IEC 61672-1) and a dB-per-octave tilt, added per
//     band in dB, which costs nothing;
//   * frame-rate-independent ballistics: every coefficient is 1 - exp(-dt/tau)
//     from the time that actually passed, so 60, 144 and 240 Hz all move the
//     bars at the same speed;
//   * BS.1770-5 loudness (momentary, short-term, gated integrated), 4x
//     oversampled true peak, and a stereo correlation meter.
//
// Cost, at FFT Size 2048 and 144 FPS: one 1024-point complex FFT per frame for
// the top tier, the lower tiers only when enough new decimated samples have
// arrived to be worth it (every 1.5 and 6 frames). Well under 1% of one core.
namespace ttdsp {

constexpr double kPi = 3.14159265358979323846;

// ---- Small helpers ----------------------------------------------------------

inline bool IsPow2(int n) { return n > 0 && (n & (n - 1)) == 0; }
inline int Log2i(int n) {
    int l = 0;
    while ((1 << l) < n) l++;
    return l;
}
inline float DbFromPower(double p) { return (float)(10.0 * log10(p > 1e-30 ? p : 1e-30)); }

// The same, without a libm call: 10 log10(p) = 10 log10(2) log2(p), with
// log2(p) = e + log2(m) from the float's exponent and a mantissa m folded
// into [sqrt(1/2), sqrt(2)), and log2(m) = (2 / ln 2) atanh(t), t = (m-1)/(m+1),
// |t| <= 0.1716, summed to t^9. The first term left out is
// (2/ln2) t^11/11 < 1.0e-9, so the error is float rounding only: under
// 1e-5 dB over the whole range (test_dsp checks 1e-4 dB against log10).
// Used for the per-band levels, which run once per band per FFT; the bars
// are drawn from these at 0.1 dB granularity at best.
inline float FastDbFromPower(double pd) {
    float p = (float)(pd > 1e-30 ? pd : 1e-30);
    uint32_t bits;
    memcpy(&bits, &p, 4);
    int e = (int)((bits >> 23) & 255) - 127;
    bits = (bits & 0x007FFFFFu) | 0x3F800000u;  // m in [1, 2)
    float m;
    memcpy(&m, &bits, 4);
    if (m > 1.41421356f) {
        m *= 0.5f;
        e++;
    }
    float t = (m - 1.f) / (m + 1.f), t2 = t * t;
    float poly = t * (1.f + t2 * (1.f / 3.f + t2 * (1.f / 5.f + t2 * (1.f / 7.f + t2 * (1.f / 9.f)))));
    // 10 log10(2) = 3.0102999566; 2 / ln 2 x that = 8.6858896381.
    return 3.0102999566f * (float)e + 8.6858896381f * poly;
}

// Newest-first audio history. Push is one store; Latest copies the newest n
// samples, oldest first, into a linear buffer for windowing.
class Ring {
public:
    void Init(int capacity) {
        buf_.assign((size_t)std::max(1, capacity), 0.f);
        head_ = 0;
        count_ = 0;
        total_ = 0;
    }
    void Clear() {
        std::fill(buf_.begin(), buf_.end(), 0.f);
        head_ = 0;
        count_ = 0;
    }
    void Push(float v) {
        buf_[head_] = v;
        head_ = (head_ + 1 == (int)buf_.size()) ? 0 : head_ + 1;
        if (count_ < (int)buf_.size()) count_++;
        total_++;
    }
    // Zero-padded at the front when fewer than n samples exist yet.
    void Latest(float* out, int n) const {
        int cap = (int)buf_.size();
        int have = std::min(n, count_);
        int pad = n - have;
        for (int i = 0; i < pad; i++) out[i] = 0.f;
        int start = head_ - have;
        if (start < 0) start += cap;
        for (int i = 0; i < have; i++) {
            out[pad + i] = buf_[start];
            if (++start == cap) start = 0;
        }
    }
    float At(int ageFromNewest) const {  // 0 = newest
        int cap = (int)buf_.size();
        int i = head_ - 1 - ageFromNewest;
        while (i < 0) i += cap;
        return buf_[i % cap];
    }
    int Capacity() const { return (int)buf_.size(); }
    int Count() const { return count_; }
    unsigned long long Total() const { return total_; }

private:
    std::vector<float> buf_;
    int head_ = 0, count_ = 0;
    unsigned long long total_ = 0;
};

// ---- Windows ------------------------------------------------------------------
//
// Periodic ("DFT-even") forms, which are the right ones for spectral analysis:
// the symmetric forms used for filter design put the window's last sample on
// top of the next period's first and widen the main lobe slightly. Figures are
// from F. J. Harris (1978).
//
//   Hann              -31.5 dB sidelobes, ENBW 1.50 bins   the general default
//   Hamming           -43 dB,             ENBW 1.36        narrower main lobe
//   Blackman-Harris   -92 dB,             ENBW 2.00        high dynamic range
//   Flat-top          scallop ~0.01 dB,   ENBW ~3.8        accurate tone levels
//   Rectangular       -13 dB,             ENBW 1.00        reference only
enum class WindowKind { Hann, Hamming, BlackmanHarris, FlatTop, Rectangular };

struct WindowSums {
    double sum = 0.0;    // sum of w[n]   = N x coherent gain
    double sumSq = 0.0;  // sum of w[n]^2 = N x incoherent (power) gain
};

inline WindowSums BuildWindow(WindowKind kind, int n, float* out) {
    double a[5] = {1, 0, 0, 0, 0};
    switch (kind) {
        case WindowKind::Hann: a[0] = 0.5; a[1] = 0.5; break;
        case WindowKind::Hamming: a[0] = 0.54; a[1] = 0.46; break;
        case WindowKind::BlackmanHarris:
            a[0] = 0.35875; a[1] = 0.48829; a[2] = 0.14128; a[3] = 0.01168;
            break;
        case WindowKind::FlatTop:
            a[0] = 0.21557895; a[1] = 0.41663158; a[2] = 0.277263158;
            a[3] = 0.083578947; a[4] = 0.006947368;
            break;
        default: break;
    }
    WindowSums s;
    for (int i = 0; i < n; i++) {
        double x = 2.0 * kPi * i / n;
        double w = a[0] - a[1] * cos(x) + a[2] * cos(2 * x) - a[3] * cos(3 * x) + a[4] * cos(4 * x);
        out[i] = (float)w;
        s.sum += w;
        s.sumSq += w * w;
    }
    return s;
}

// ---- Real-input FFT -------------------------------------------------------------
//
// N real samples are packed as N/2 complex values z[k] = x[2k] + i x[2k+1],
// transformed with an N/2-point radix-2 FFT, and unpacked with one post-twiddle
// pass:
//   X[k] = E[k] + W^k O[k],  E = (Z[k] + conj Z[m-k]) / 2,
//                            O = (Z[k] - conj Z[m-k]) / 2i,   W = e^(-2 pi i / N)
// Output is bins 0..N/2 inclusive.
class RealFft {
public:
    bool Init(int n) {
        if (!IsPow2(n) || n < 16) return false;
        n_ = n;
        m_ = n / 2;
        int lg = Log2i(m_);
        rev_.resize(m_);
        for (int i = 0; i < m_; i++) {
            int r = 0;
            for (int b = 0; b < lg; b++)
                if (i & (1 << b)) r |= 1 << (lg - 1 - b);
            rev_[i] = r;
        }
        twr_.resize(m_ / 2);
        twi_.resize(m_ / 2);
        for (int k = 0; k < m_ / 2; k++) {
            double a = -2.0 * kPi * k / m_;
            twr_[k] = (float)cos(a);
            twi_[k] = (float)sin(a);
        }
        pr_.resize(m_ + 1);
        pi_.resize(m_ + 1);
        for (int k = 0; k <= m_; k++) {
            double a = -2.0 * kPi * k / n_;
            pr_[k] = (float)cos(a);
            pi_[k] = (float)sin(a);
        }
        zr_.assign(m_, 0.f);
        zi_.assign(m_, 0.f);
        return true;
    }
    int Size() const { return n_; }

    void Forward(const float* in, float* re, float* im) {
        const int m = m_;
        for (int k = 0; k < m; k++) {
            int r = rev_[k];
            zr_[r] = in[2 * k];
            zi_[r] = in[2 * k + 1];
        }
        for (int len = 2; len <= m; len <<= 1) {
            int half = len >> 1;
            int stride = m / len;
            for (int i = 0; i < m; i += len) {
                for (int j = 0; j < half; j++) {
                    float wr = twr_[j * stride], wi = twi_[j * stride];
                    int a = i + j, b = a + half;
                    float vr = zr_[b] * wr - zi_[b] * wi;
                    float vi = zr_[b] * wi + zi_[b] * wr;
                    zr_[b] = zr_[a] - vr;
                    zi_[b] = zi_[a] - vi;
                    zr_[a] += vr;
                    zi_[a] += vi;
                }
            }
        }
        for (int k = 0; k <= m; k++) {
            int ka = (k == m) ? 0 : k;
            int kb = (k == 0) ? 0 : m - k;
            float ar = zr_[ka], ai = zi_[ka];
            float br = zr_[kb], bi = -zi_[kb];  // conj Z[m-k]
            float er = 0.5f * (ar + br), ei = 0.5f * (ai + bi);
            float dr = ar - br, di = ai - bi;
            float orr = 0.5f * di, oi = -0.5f * dr;  // (a - b) / 2i
            re[k] = er + pr_[k] * orr - pi_[k] * oi;
            im[k] = ei + pr_[k] * oi + pi_[k] * orr;
        }
    }

private:
    int n_ = 0, m_ = 0;
    std::vector<int> rev_;
    std::vector<float> twr_, twi_, pr_, pi_, zr_, zi_;
};

// ---- Half-band decimator (by 2) ---------------------------------------------------
//
// A half-band lowpass has every even-offset tap except the centre at zero, so
// only about a quarter of the taps cost a multiply, and it only has to be
// evaluated for every second input sample. Kaiser-windowed, 59 taps, beta 9:
// about 90 dB of stopband from 0.3 fs, passband flat to 0.2 fs. After the
// decimation that leaves the output clean up to 0.8 of its own Nyquist, which
// is exactly the band the tier selector below allows each tier to serve.
class HalfBand {
public:
    static constexpr int kTaps = 59;  // 4k + 3, so the outermost taps are non-zero
    static constexpr int kMid = kTaps / 2;

    HalfBand() {
        double beta = 9.0;
        auto bessel0 = [](double x) {
            double sum = 1.0, term = 1.0;
            for (int k = 1; k < 50; k++) {
                term *= (x / (2.0 * k)) * (x / (2.0 * k));
                sum += term;
                if (term < 1e-12 * sum) break;
            }
            return sum;
        };
        double denom = bessel0(beta);
        double gain = 0.0;
        for (int i = 0; i < kTaps; i++) {
            int n = i - kMid;
            double h = (n == 0) ? 0.5 : sin(kPi * n / 2.0) / (kPi * n);
            double r = (double)n / kMid;
            double w = bessel0(beta * sqrt(std::max(0.0, 1.0 - r * r))) / denom;
            h *= w;
            if (n != 0 && (n % 2) == 0) h = 0.0;
            coef_[i] = h;
            gain += h;
        }
        // Unity gain at DC exactly.
        for (int i = 0; i < kTaps; i++) coef_[i] /= gain;
        // Odd-offset taps, folded by symmetry: odd_[j] multiplies x[mid - (2j+1)] + x[mid + (2j+1)].
        for (int j = 0; j < kOdd; j++) odd_[j] = (float)coef_[kMid + 2 * j + 1];
        center_ = (float)coef_[kMid];
        Reset();
    }
    void Reset() {
        for (int i = 0; i < 2 * kTaps; i++) hist_[i] = 0.f;
        pos_ = 0;
        phase_ = 0;
    }
    // Returns true when an output sample was produced (every second call).
    bool Push(float x, float* out) {
        // Doubled history so the newest kTaps samples are always contiguous.
        hist_[pos_] = x;
        hist_[pos_ + kTaps] = x;
        pos_ = (pos_ + 1 == kTaps) ? 0 : pos_ + 1;
        phase_ ^= 1;
        if (phase_) return false;
        const float* h = hist_ + pos_;  // h[0] oldest ... h[kTaps-1] newest
        float acc = center_ * h[kMid];
        for (int j = 0; j < kOdd; j++) acc += odd_[j] * (h[kMid - (2 * j + 1)] + h[kMid + (2 * j + 1)]);
        *out = acc;
        return true;
    }
    double Coef(int i) const { return coef_[i]; }

private:
    static constexpr int kOdd = (kMid + 1) / 2;
    double coef_[kTaps];
    float odd_[kOdd];
    float center_ = 0.5f;
    float hist_[2 * kTaps];
    int pos_ = 0, phase_ = 0;
};

// ---- Band layouts -------------------------------------------------------------------

enum class BandLayout { Scale, Iec, Musical };
enum class FreqScale { Log, Linear, Mel, Bark, Erb };

struct Band {
    double f1 = 0, f2 = 0, fc = 0;  // edges and centre, Hz
};

inline double ScaleFwd(FreqScale s, double f) {
    switch (s) {
        case FreqScale::Linear: return f;
        case FreqScale::Mel: return 2595.0 * log10(1.0 + f / 700.0);
        case FreqScale::Bark: return 26.81 * f / (1960.0 + f) - 0.53;  // Traunmueller
        case FreqScale::Erb: return 21.4 * log10(1.0 + 0.00437 * f);  // Glasberg & Moore
        default: return log(std::max(f, 1e-3));
    }
}
inline double ScaleInv(FreqScale s, double u) {
    switch (s) {
        case FreqScale::Linear: return u;
        case FreqScale::Mel: return 700.0 * (pow(10.0, u / 2595.0) - 1.0);
        case FreqScale::Bark: return 1960.0 * (u + 0.53) / (26.28 - u);
        case FreqScale::Erb: return (pow(10.0, u / 21.4) - 1.0) / 0.00437;
        default: return exp(u);
    }
}

// n bands evenly spaced on the chosen scale between fmin and fmax. Each band's
// centre is the midpoint of its edges in the scale's own units, so on Log it is
// the geometric mean.
inline std::vector<Band> ScaleBands(FreqScale s, double fmin, double fmax, int n) {
    std::vector<Band> out;
    n = std::max(1, n);
    double u0 = ScaleFwd(s, fmin), u1 = ScaleFwd(s, fmax);
    for (int i = 0; i < n; i++) {
        double a = u0 + (u1 - u0) * i / n;
        double b = u0 + (u1 - u0) * (i + 1) / n;
        Band bd;
        bd.f1 = ScaleInv(s, a);
        bd.f2 = ScaleInv(s, b);
        bd.fc = ScaleInv(s, 0.5 * (a + b));
        out.push_back(bd);
    }
    return out;
}

// IEC 61260-1:2014 / ANSI S1.11, base-10 system. G = 10^(3/10), reference
// 1000 Hz, bandwidth designator b (1 = octave, 3 = third-octave, ...).
//   b odd:  fm = fr G^(x/b)
//   b even: fm = fr G^((2x+1)/(2b))
//   edges:  fm G^(-1/2b), fm G^(+1/2b)
// A band is kept when its exact midband frequency is within a quarter of a
// band of [fmin, fmax], so the band labelled "20 Hz" (exactly 19.95 Hz) is in
// a 20 Hz - 20 kHz range, the way any RTA shows it.
inline std::vector<Band> IecBands(int b, double fmin, double fmax) {
    std::vector<Band> out;
    b = std::max(1, b);
    const double G = pow(10.0, 0.3);
    const double tol = pow(G, 1.0 / (4.0 * b));
    for (int x = -10 * b; x <= 10 * b; x++) {
        double fm = (b % 2) ? 1000.0 * pow(G, (double)x / b) : 1000.0 * pow(G, (2.0 * x + 1.0) / (2.0 * b));
        if (fm < fmin / tol || fm > fmax * tol) continue;
        Band bd;
        bd.fc = fm;
        bd.f1 = fm * pow(G, -1.0 / (2.0 * b));
        bd.f2 = fm * pow(G, 1.0 / (2.0 * b));
        out.push_back(bd);
    }
    return out;
}

// Equal temperament: one band per note (stepsPerOctave 12) or per quarter tone
// (24), centred on fm = A4 x 2^(k / steps), edges half a step either side.
inline std::vector<Band> MusicalBands(int stepsPerOctave, double a4, double fmin, double fmax) {
    std::vector<Band> out;
    int steps = (stepsPerOctave >= 24) ? 24 : 12;
    double half = pow(2.0, 0.5 / steps);
    int kLo = (int)floor(steps * log2(fmin / a4)) - 1;
    int kHi = (int)ceil(steps * log2(fmax / a4)) + 1;
    for (int k = kLo; k <= kHi; k++) {
        double fm = a4 * pow(2.0, (double)k / steps);
        if (fm < fmin * 0.9999 || fm > fmax * 1.0001) continue;
        Band bd;
        bd.fc = fm;
        bd.f1 = fm / half;
        bd.f2 = fm * half;
        out.push_back(bd);
    }
    return out;
}

// ---- Weighting curves (IEC 61672-1), dB at f -------------------------------------
inline double AWeightDb(double f) {
    double f2 = f * f;
    double ra = (12194.0 * 12194.0 * f2 * f2) /
                ((f2 + 20.6 * 20.6) * sqrt((f2 + 107.7 * 107.7) * (f2 + 737.9 * 737.9)) *
                 (f2 + 12194.0 * 12194.0));
    return 20.0 * log10(std::max(ra, 1e-30)) + 2.00;
}
inline double CWeightDb(double f) {
    double f2 = f * f;
    double rc = (12194.0 * 12194.0 * f2) / ((f2 + 20.6 * 20.6) * (f2 + 12194.0 * 12194.0));
    return 20.0 * log10(std::max(rc, 1e-30)) + 0.06;
}

enum class Weighting { Z, A, C };
enum class Detector { Rms, Peak };
enum class LevelRef { ThirdOctave, Band };

// ---- Spectrum engine ----------------------------------------------------------------
//
// Owns the three tiers (rings, decimators, FFTs), the per-band bin maps and
// the per-band dB offsets. Analyze() turns the newest window of each tier into
// one dBFS figure per band.
//
// Level calibration. Both detectors read a full-scale sine as 0 dBFS:
//   RMS:  mean square from the bins, 2 sum|X|^2 / (N sum w^2), which is
//         Parseval's theorem for a windowed block, plus 3.01 dB so a sine's
//         0.5 mean square reads 0 dB. Correct for noise and tones alike, as
//         long as the band covers the tone's main lobe.
//   Peak: the largest bin, 2|X| / sum w. Exact for a tone on a bin centre,
//         low by the window's scallop loss between bins.
// With Level Reference = third-octave, RMS bands are scaled to the power a
// 1/3-octave band at the same centre would hold (+10 log10(BW13 / BW)). That
// makes pink noise read flat and makes bar heights independent of how many
// bars there are, which is what a 1/3-octave RTA shows.
class SpectrumEngine {
public:
    struct Config {
        int sampleRate = 48000;
        int fftSize = 2048;
        int maxTier = 2;  // 0 = single resolution, 1 = add /4, 2 = add /4 and /16
        WindowKind window = WindowKind::Hann;
        BandLayout layout = BandLayout::Scale;
        FreqScale scale = FreqScale::Log;
        int octaveFraction = 6;     // IEC b, or 12 / 24 for Musical
        double fmin = 20.0, fmax = 20000.0;
        double a4 = 440.0;
        int bars = 32;              // used by Scale layout
        int maxBands = 2048;
        Weighting weighting = Weighting::Z;
        double tiltDbPerOct = 0.0;  // pivot 1 kHz
        Detector detector = Detector::Rms;
        LevelRef levelRef = LevelRef::ThirdOctave;
        double zoneDb[3] = {0, 0, 0};  // EQ preset, low (<300 Hz) / mid / high (>2.5 kHz)
    };

    struct BandMap {
        int tier = 0;
        int k0 = 1, k1 = 1;      // inclusive bin range
        float w0 = 1.f, w1 = 1.f; // weight of the first / last bin (partial overlap)
        float offsetDb = 0.f;    // calibration + weighting + tilt + EQ + level reference
        float peakOffsetDb = 0.f;// same for the Peak detector (no bandwidth term)
    };

    bool Configure(const Config& c) {
        cfg_ = c;
        int n = c.fftSize;
        if (!IsPow2(n) || n < 256) n = 2048;
        cfg_.fftSize = n;
        cfg_.maxTier = std::clamp(c.maxTier, 0, 2);
        double nyq = 0.5 * cfg_.sampleRate;
        double fmax = std::min(cfg_.fmax, nyq * 0.98);
        double fmin = std::clamp(cfg_.fmin, 1.0, fmax * 0.5);

        // Bands.
        switch (cfg_.layout) {
            case BandLayout::Iec: bands_ = IecBands(cfg_.octaveFraction, fmin, fmax); break;
            case BandLayout::Musical:
                bands_ = MusicalBands(cfg_.octaveFraction, cfg_.a4, fmin, fmax);
                break;
            default: bands_ = ScaleBands(cfg_.scale, fmin, fmax, cfg_.bars); break;
        }
        if ((int)bands_.size() > cfg_.maxBands) bands_.resize(cfg_.maxBands);
        if (bands_.empty()) bands_ = ScaleBands(FreqScale::Log, fmin, fmax, 1);
        for (auto& b : bands_) {  // a band can't extend past what any tier can see
            b.f2 = std::min(b.f2, nyq * 0.98);
            b.f1 = std::min(b.f1, b.f2 * 0.999);
        }

        window_.resize(n);
        WindowSums ws = BuildWindow(cfg_.window, n, window_.data());
        fft_.Init(n);
        re_.assign(n / 2 + 1, 0.f);
        im_.assign(n / 2 + 1, 0.f);
        scratch_.assign(n, 0.f);
        for (int t = 0; t < 3; t++) {
            ring_[t].Init(n * 2);
            power_[t].assign(n / 2 + 1, 0.f);
            fresh_[t] = 0;
            valid_[t] = false;
        }
        for (auto& d : dec_) d.Reset();

        // Calibration constants (see the class comment).
        const double rmsCal = 10.0 * log10(2.0 / (n * ws.sumSq)) + 10.0 * log10(2.0);
        const double peakCal = 20.0 * log10(2.0 / ws.sum);
        peakCal_ = peakCal;
        // Hop per tier before its FFT is worth redoing: the top tier every
        // frame, the decimated ones once N/16 new samples have arrived.
        hop_[0] = 1;
        hop_[1] = hop_[2] = std::max(16, n / 16);

        maps_.assign(bands_.size(), BandMap());
        tierUsed_[0] = true;
        tierUsed_[1] = tierUsed_[2] = false;
        // Half-width of the window's main lobe, in bins. A tone spreads that
        // far either side of its bin, so a band has to be at least a full
        // lobe wide (both halves) before a tone in the next band over stops
        // leaking into it.
        double lobe = 2.0;
        switch (cfg_.window) {
            case WindowKind::BlackmanHarris: lobe = 4.0; break;
            case WindowKind::FlatTop: lobe = 5.0; break;
            case WindowKind::Rectangular: lobe = 1.0; break;
            default: break;
        }
        for (size_t i = 0; i < bands_.size(); i++) {
            const Band& b = bands_[i];
            BandMap& m = maps_[i];
            double bw = std::max(b.f2 - b.f1, 1e-6);
            // Tier: the least decimated (fastest) one whose main lobe fits
            // inside the band, as long as the band sits inside that tier's
            // clean passband. Falls back to the finest tier the passband
            // allows, which is the best resolution there is.
            int tier = 0;
            for (int t = 0; t <= cfg_.maxTier; t++) {
                double fs = TierRate(t);
                if (b.f2 > 0.4 * fs) break;
                tier = t;
                if (2.0 * lobe * fs / n <= bw) break;
            }
            m.tier = tier;
            tierUsed_[tier] = true;
            double df = TierRate(tier) / n;
            double x1 = b.f1 / df, x2 = b.f2 / df;  // edges in bins
            int k0 = (int)floor(x1 + 0.5), k1 = (int)floor(x2 + 0.5);
            k0 = std::clamp(k0, 1, n / 2);
            k1 = std::clamp(k1, k0, n / 2);
            m.k0 = k0;
            m.k1 = k1;
            if (k0 == k1) {
                m.w0 = (float)std::clamp(x2 - x1, 0.0, 1.0);
                m.w1 = m.w0;
            } else {
                m.w0 = (float)std::clamp((k0 + 0.5) - x1, 0.0, 1.0);
                m.w1 = (float)std::clamp(x2 - (k1 - 0.5), 0.0, 1.0);
            }
            double off = 0.0;
            if (cfg_.weighting == Weighting::A) off += AWeightDb(b.fc);
            else if (cfg_.weighting == Weighting::C) off += CWeightDb(b.fc);
            off += cfg_.tiltDbPerOct * log2(b.fc / 1000.0);
            int zone = (b.fc < 300.0) ? 0 : (b.fc < 2500.0) ? 1 : 2;
            off += cfg_.zoneDb[zone];
            double refTerm = 0.0;
            if (cfg_.levelRef == LevelRef::ThirdOctave) {
                double bw13 = b.fc * (pow(2.0, 1.0 / 6.0) - pow(2.0, -1.0 / 6.0));
                refTerm = 10.0 * log10(bw13 / bw);
            }
            m.offsetDb = (float)(rmsCal + off + refTerm);
            m.peakOffsetDb = (float)(peakCal + off);
        }
        levelsDb_.assign(bands_.size(), -200.f);
        configured_ = true;
        return true;
    }

    // Mono samples in. Feeds the top tier directly and the decimator cascade
    // for the others (only as deep as a band actually needs).
    void Push(const float* x, int count) {
        if (!configured_) return;
        int deepest = tierUsed_[2] ? 2 : tierUsed_[1] ? 1 : 0;
        for (int i = 0; i < count; i++) {
            float v = x[i];
            ring_[0].Push(v);
            fresh_[0]++;
            if (deepest < 1) continue;
            float a, b2;
            if (!dec_[0].Push(v, &a)) continue;
            if (!dec_[1].Push(a, &b2)) continue;
            ring_[1].Push(b2);
            fresh_[1]++;
            if (deepest < 2) continue;
            float c, d;
            if (!dec_[2].Push(b2, &c)) continue;
            if (!dec_[3].Push(c, &d)) continue;
            ring_[2].Push(d);
            fresh_[2]++;
        }
    }

    // Runs the FFT for each tier that has enough new data, then rebuilds the
    // band levels. Returns true if anything was recomputed.
    //
    // fftOverride lets the caller run the transform somewhere else:
    // it gets the windowed block and must fill re/im with bins 0..N/2, or
    // return false to fall back to the CPU FFT.
    template <typename FftOverride>
    bool Analyze(bool force, FftOverride&& fftOverride) {
        if (!configured_) return false;
        const int n = cfg_.fftSize;
        bool any = false;
        lastFfts_ = 0;
        for (int t = 0; t < 3; t++) {
            if (!tierUsed_[t]) continue;
            if (!force && fresh_[t] < hop_[t]) continue;
            fresh_[t] = 0;
            ring_[t].Latest(scratch_.data(), n);
            for (int i = 0; i < n; i++) scratch_[i] *= window_[i];
            if (!fftOverride(scratch_.data(), n, re_.data(), im_.data()))
                fft_.Forward(scratch_.data(), re_.data(), im_.data());
            float* p = power_[t].data();
            for (int k = 0; k <= n / 2; k++) p[k] = re_[k] * re_[k] + im_[k] * im_[k];
            valid_[t] = true;
            any = true;
            lastFfts_++;
        }
        if (!any) return false;
        const bool peakDet = cfg_.detector == Detector::Peak;
        for (size_t i = 0; i < maps_.size(); i++) {
            const BandMap& m = maps_[i];
            const float* p = power_[m.tier].data();
            double v;
            if (peakDet) {
                float mx = 0.f;
                for (int k = m.k0; k <= m.k1; k++) mx = std::max(mx, p[k]);
                v = (double)FastDbFromPower(mx) + m.peakOffsetDb;  // 10log10(|X|^2) = 20log10|X|
            } else {
                double s;
                if (m.k0 == m.k1) {
                    s = p[m.k0] * m.w0;
                } else {
                    s = p[m.k0] * m.w0 + p[m.k1] * m.w1;
                    for (int k = m.k0 + 1; k < m.k1; k++) s += p[k];
                }
                v = (double)FastDbFromPower(s) + m.offsetDb;
            }
            levelsDb_[i] = (float)v;
        }
        return true;
    }
    bool Analyze(bool force) {
        return Analyze(force, [](const float*, int, float*, float*) { return false; });
    }
    int LastFftCount() const { return lastFfts_; }  // transforms the last Analyze ran

    // Loudest bin between fmin and fmax, refined by a parabola through the
    // log magnitudes of the bin and its neighbours (accurate to a fraction of
    // a bin for a steady tone). Uses the finest tier that covers each range.
    //
    // The search runs on power, not dB: the dB figure is a monotonic function
    // of the bin power plus one constant (PeakCal, the same for every tier),
    // so the loudest local maximum is the same one. 1.x/2.0 took a log10 and
    // re-summed the whole window (PeakCal()) for every bin of every tier, about
    // 6 million additions per frame at FFT 2048; now the logs are taken only
    // for a new best bin and its two neighbours.
    double DominantHz(double minLevelDbfs) const {
        double bestDb = -1e9, bestHz = 0.0;
        float bestP = -1.f;
        const int n = cfg_.fftSize;
        for (int t = 0; t < 3; t++) {
            if (!valid_[t]) continue;
            double fs = TierRate(t);
            double df = fs / n;
            // This tier owns [lo, hi): above the next tier's passband edge.
            double hi = (t == 0) ? std::min(cfg_.fmax, 0.49 * fs) : 0.4 * fs;
            double lo = (t < 2 && valid_[t + 1]) ? 0.4 * TierRate(t + 1) : cfg_.fmin;
            int k0 = std::max(2, (int)ceil(lo / df));
            int k1 = std::min(n / 2 - 2, (int)floor(hi / df));
            const float* p = power_[t].data();
            for (int k = k0; k <= k1; k++) {
                if (p[k] > bestP && p[k] >= p[k - 1] && p[k] >= p[k + 1]) {
                    bestP = p[k];
                    double a = DbFromPower(p[k - 1]), b = DbFromPower(p[k]), c = DbFromPower(p[k + 1]);
                    double den = a - 2 * b + c;
                    double delta = (fabs(den) > 1e-9) ? 0.5 * (a - c) / den : 0.0;
                    bestDb = b + peakCal_;
                    bestHz = (k + std::clamp(delta, -0.5, 0.5)) * df;
                }
            }
        }
        return (bestDb >= minLevelDbfs) ? bestHz : 0.0;
    }

    double TierRate(int t) const { return cfg_.sampleRate / (double)(1 << (2 * t)); }
    int NumBands() const { return (int)bands_.size(); }
    const std::vector<Band>& Bands() const { return bands_; }
    const std::vector<BandMap>& Maps() const { return maps_; }
    const float* LevelsDb() const { return levelsDb_.data(); }
    const Config& Cfg() const { return cfg_; }
    bool TierUsed(int t) const { return tierUsed_[t]; }
    const Ring& TierRing(int t) const { return ring_[t]; }
    const float* Window() const { return window_.data(); }
    double PeakCal() const { return peakCal_; }
    // For the GPU workload: the newest window of each tier, unwindowed, and
    // whether the tier has moved on enough to be re-transformed.
    bool TakeTierBlock(int t, float* out, bool force) {
        if (!tierUsed_[t]) return false;
        if (!force && fresh_[t] < hop_[t]) return false;
        fresh_[t] = 0;
        ring_[t].Latest(out, cfg_.fftSize);
        return true;
    }

private:
    Config cfg_;
    bool configured_ = false;
    double peakCal_ = 0.0;
    std::vector<Band> bands_;
    std::vector<BandMap> maps_;
    std::vector<float> window_, re_, im_, scratch_, levelsDb_;
    std::vector<float> power_[3];
    Ring ring_[3];
    HalfBand dec_[4];
    int fresh_[3] = {0, 0, 0};
    int hop_[3] = {1, 1, 1};
    bool valid_[3] = {false, false, false};
    bool tierUsed_[3] = {true, false, false};
    RealFft fft_;
    int lastFfts_ = 0;
};

// ---- Display mapping and ballistics ----------------------------------------------------
//
// Level in dBFS -> bar height 0..1 across [floor, ceiling], then ballistics in
// that (dB-linear) domain, which is how meters behave: the same number of dB
// per second falls the same distance anywhere on the scale.
//
// Every coefficient is computed from dt, the time since the previous update:
//   rise / exponential fall: y += (x - y) (1 - e^(-dt / tau))
//   linear fall:             y  = max(x, y - rate dt)
// so a frame that runs late moves further, by exactly the right amount.
enum class ReleaseKind { Exponential, Linear };

struct Ballistics {
    double attackMs = 10.0;
    ReleaseKind release = ReleaseKind::Exponential;
    double releaseMs = 70.0;      // tau, for Exponential
    double releaseDbPerSec = 20;  // for Linear
};

inline Ballistics BallisticsPreset(int preset) {
    // 0 snappy (closest to the 1.4 feel), 1 smooth, 2 analyzer, 3 VU,
    // 4 EBU / IEC 60268-10 Type II PPM, 5 DIN / Type I PPM.
    Ballistics b;
    switch (preset) {
        case 1: b.attackMs = 25; b.releaseMs = 180; break;
        case 2: b.attackMs = 10; b.release = ReleaseKind::Linear; b.releaseDbPerSec = 20; break;
        case 3: b.attackMs = 65; b.releaseMs = 65; break;  // 300 ms to 99%
        case 4: b.attackMs = 10; b.release = ReleaseKind::Linear; b.releaseDbPerSec = 24.0 / 2.8; break;
        case 5: b.attackMs = 5; b.release = ReleaseKind::Linear; b.releaseDbPerSec = 20.0 / 1.5; break;
        default: b.attackMs = 10; b.releaseMs = 70; break;
    }
    return b;
}

enum class Curve { Exponential, Knee, Power, Linear };

inline float ApplyCurve(Curve c, float x) {
    if (x <= 0.f) return 0.f;
    switch (c) {
        // Soft from the start, scaled so full scale still reaches the top.
        case Curve::Exponential: return std::min(1.f, (1.f - expf(-2.f * x)) / (1.f - expf(-2.f)));
        case Curve::Power: return std::min(1.f, powf(x, 0.6f));
        case Curve::Linear: return std::min(1.f, x);
        default: {
            const float knee = 0.7f;
            return (x <= knee) ? x : knee + (1.f - knee) * tanhf((x - knee) / (1.f - knee));
        }
    }
}

struct DisplayMap {
    float floorDb = -72.f, ceilDb = -12.f;
    float gainDb = 0.f;  // sensitivity + auto gain
    Curve curve = Curve::Knee;
    float ToNorm(float db) const {
        float x = (db + gainDb - floorDb) / std::max(1.f, ceilDb - floorDb);
        return std::clamp(ApplyCurve(curve, x), 0.f, 1.f);
    }
};

// The coefficients depend only on dt and the preset, so they are worked out
// once per frame (two exp() calls) instead of once per band; applying them is
// the same arithmetic as before, so the result is bit-identical.
struct BallisticsCoef {
    double attack = 0.0, release = 0.0;  // 1 - e^(-dt / tau)
    float fall = 0.f;                    // linear release, in display units this frame
    bool linear = false;
};
inline BallisticsCoef BallisticsCoefs(double dt, const Ballistics& b, float rangeDb) {
    BallisticsCoef c;
    c.attack = 1.0 - exp(-dt * 1000.0 / std::max(0.1, b.attackMs));
    c.linear = b.release == ReleaseKind::Linear;
    if (c.linear) c.fall = (float)(b.releaseDbPerSec * dt / std::max(1.f, rangeDb));
    else c.release = 1.0 - exp(-dt * 1000.0 / std::max(0.1, b.releaseMs));
    return c;
}
// One state per band.
inline float BallisticsApply(float y, float x, const BallisticsCoef& c) {
    if (x > y) return (float)(y + (x - y) * c.attack);
    if (c.linear) return std::max(x, y - c.fall);
    return (float)(y + (x - y) * c.release);
}
inline float BallisticsStep(float y, float x, double dt, const Ballistics& b, float rangeDb) {
    return BallisticsApply(y, x, BallisticsCoefs(dt, b, rangeDb));
}

// Peak hold per bar: hangs for holdMs, then falls with gravity (accelerating,
// like a dropped object) or at a constant rate.
struct PeakHold {
    float level = 0.f, timer = 0.f, vel = 0.f;
};
inline void PeakHoldStep(PeakHold& p, float x, double dt, double holdMs, bool gravity, float gPerSec2,
                         float linPerSec) {
    if (x >= p.level) {
        p.level = x;
        p.timer = 0.f;
        p.vel = 0.f;
        return;
    }
    p.timer += (float)dt;
    if (p.timer * 1000.f < holdMs) return;
    if (gravity) {
        p.vel += gPerSec2 * (float)dt;
        p.level -= p.vel * (float)dt;
    } else {
        p.level -= linPerSec * (float)dt;
    }
    if (p.level < x) {
        p.level = x;
        p.vel = 0.f;
    }
    if (p.level < 0.f) p.level = 0.f;
}

// ---- Biquad (transposed direct form II) --------------------------------------------------
struct Biquad {
    double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
    double z1 = 0, z2 = 0;
    inline double Run(double x) {
        double y = b0 * x + z1;
        z1 = b1 * x - a1 * y + z2;
        z2 = b2 * x - a2 * y;
        return y;
    }
    void Reset() { z1 = z2 = 0; }
};

// BS.1770 K-weighting for any sample rate, from the analogue prototypes (the
// libebur128 / pyloudnorm derivation). At 48 kHz this reproduces the
// coefficients printed in the Recommendation to about 1e-8.
inline void KWeightingFilters(double fs, Biquad& shelf, Biquad& hp) {
    {
        const double f0 = 1681.974450955533, G = 3.999843853973347, Q = 0.7071752369554196;
        double K = tan(kPi * f0 / fs);
        double Vh = pow(10.0, G / 20.0);
        double Vb = pow(Vh, 0.4996667741545416);
        double a0 = 1.0 + K / Q + K * K;
        shelf.b0 = (Vh + Vb * K / Q + K * K) / a0;
        shelf.b1 = 2.0 * (K * K - Vh) / a0;
        shelf.b2 = (Vh - Vb * K / Q + K * K) / a0;
        shelf.a1 = 2.0 * (K * K - 1.0) / a0;
        shelf.a2 = (1.0 - K / Q + K * K) / a0;
    }
    {
        const double f0 = 38.13547087602444, Q = 0.5003270373238773;
        double K = tan(kPi * f0 / fs);
        double a0 = 1.0 + K / Q + K * K;
        hp.b0 = 1.0;
        hp.b1 = -2.0;
        hp.b2 = 1.0;
        hp.a1 = 2.0 * (K * K - 1.0) / a0;
        hp.a2 = (1.0 - K / Q + K * K) / a0;
    }
}

// ---- Loudness and true peak (ITU-R BS.1770-5 / EBU R128) --------------------------------
//
//   K-weighting -> mean square per channel -> sum with channel weights G_i
//   L = -0.691 + 10 log10(sum G_i z_i)   LUFS
// Momentary = 400 ms, short-term = 3 s (EBU Tech 3341), integrated = the mean
// of every 400 ms block (75% overlap, so one every 100 ms) above the absolute
// gate (-70 LUFS) and the relative gate (10 LU below the ungated mean).
// Integrated uses a 0.1 LU histogram of block loudness, the libebur128 trick,
// so memory and time stay constant however long a track runs.
//
// True peak: 4x oversampling through a 64-tap polyphase interpolator
// (Kaiser-windowed sinc, cut off at the original Nyquist), the maximum
// absolute value over every original and interpolated sample, in dBTP.
//
// Two savings, neither of which changes a reading:
//   * SetTruePeak(false) leaves the interpolator out entirely (the readout
//     only shows true peak in its full form);
//   * a window that provably can't raise the peak isn't interpolated. Phase
//     p's output is a_p = sum_j h_p[j] x[j] over the kTpTaps newest samples,
//     so |a_p| <= sum_j |h_p[j]| |x[j]| <= S max_j |x[j]|, S = max_p sum_j
//     |h_p[j]|. Computed in float, a 16-term dot product is off by at most
//     gamma_16 = 16u / (1 - 16u) < 2^-19 of that bound (u = 2^-24), plus
//     underflow terms that only matter below 1e-30. tpBound_ is S (1 + 2^-12)
//     rounded up, which covers both that and the rounding of |x| tpBound_.
//     So if every sample in the window had |x| tpBound_ <= truePeak_ when it
//     arrived, every float a_p the loop would compute is <= truePeak_ then,
//     and truePeak_ only grows until Reset(): max(truePeak_, |a_p|) would
//     leave it unchanged, and skipping the evaluation is exact. hot_[c] counts
//     the pushes for which a sample that failed that test is still inside
//     channel c's window. On music near its own peak this saves little; on
//     anything quieter than the peak so far (most of a track, fades, silence)
//     nearly all of it.
class LoudnessMeter {
public:
    static constexpr int kMaxCh = 8;
    static constexpr int kTpTaps = 16;  // per phase

    void Configure(int fs, int channels, const float* weights) {
        fs_ = std::max(8000, fs);
        ch_ = std::clamp(channels, 1, kMaxCh);
        for (int c = 0; c < ch_; c++) {
            w_[c] = weights ? weights[c] : 1.f;
            KWeightingFilters(fs_, shelf_[c], hp_[c]);
        }
        subLen_ = fs_ / 10;
        // Interpolator.
        const int L = 4, taps = L * kTpTaps;
        double beta = 7.0;
        auto bessel0 = [](double x) {
            double s = 1.0, t = 1.0;
            for (int k = 1; k < 50; k++) {
                t *= (x / (2.0 * k)) * (x / (2.0 * k));
                s += t;
            }
            return s;
        };
        double den = bessel0(beta);
        for (int i = 0; i < taps; i++) {
            double n = i - (taps - 1) / 2.0;
            double x = n / L;
            double h = (fabs(x) < 1e-12) ? 1.0 : sin(kPi * x) / (kPi * x);
            double r = n / ((taps - 1) / 2.0);
            h *= bessel0(beta * sqrt(std::max(0.0, 1.0 - r * r))) / den;
            // Phase p uses taps i = p + L j.
            tp_[i % L][i / L] = (float)h;
        }
        double S = 0.0;
        for (int p = 0; p < L; p++) {
            double sp = 0.0;
            for (int j = 0; j < kTpTaps; j++) sp += fabs((double)tp_[p][j]);
            S = std::max(S, sp);
        }
        tpBound_ = std::nextafter((float)(S * (1.0 + 1.0 / 4096.0)), 1e30f);
        Reset();
    }
    void Reset() {
        for (int c = 0; c < ch_; c++) {
            shelf_[c].Reset();
            hp_[c].Reset();
            for (int j = 0; j < kTpTaps; j++) tpHist_[c][j] = 0.f;
            hot_[c] = 0;
        }
        tpPos_ = 0;
        subAcc_ = 0.0;
        subCount_ = 0;
        for (int i = 0; i < 30; i++) sub_[i] = 0.0;
        subHead_ = 0;
        subFilled_ = 0;
        for (int i = 0; i < kHist; i++) {
            histCount_[i] = 0;
            histEnergy_[i] = 0.0;
        }
        truePeak_ = 0.f;
        blocks_ = 0;
    }
    // Off: no true peak at all (TruePeakDb reads -200). Switching clears the
    // true-peak state, so a meter turned back on starts a clean measurement.
    void SetTruePeak(bool on) {
        if (on == tpOn_) return;
        tpOn_ = on;
        for (int c = 0; c < kMaxCh; c++) {
            for (int j = 0; j < kTpTaps; j++) tpHist_[c][j] = 0.f;
            hot_[c] = 0;
        }
        truePeak_ = 0.f;
    }
    bool TruePeakOn() const { return tpOn_; }

    // Interleaved frames.
    void Process(const float* x, int frames, int stride) {
        for (int f = 0; f < frames; f++) {
            const float* s = x + (size_t)f * stride;
            double e = 0.0;
            for (int c = 0; c < ch_; c++) {
                double v = hp_[c].Run(shelf_[c].Run(s[c]));
                e += w_[c] * v * v;
            }
            if (tpOn_) TruePeakFrame(s);
            subAcc_ += e;
            if (++subCount_ >= subLen_) EndSubBlock();
        }
    }
    double Momentary() const { return Window(4); }
    double ShortTerm() const { return Window(30); }
    double Integrated() const {
        // Absolute gate already applied on entry; relative gate here.
        double sumE = 0.0;
        long long n = 0;
        for (int i = 0; i < kHist; i++) {
            sumE += histEnergy_[i];
            n += histCount_[i];
        }
        if (n == 0) return -HUGE_VAL;
        double ungated = -0.691 + 10.0 * log10(sumE / n);
        double rel = ungated - 10.0;
        int start = std::clamp((int)ceil((rel - kHistMin) * 10.0), 0, kHist);
        sumE = 0.0;
        n = 0;
        for (int i = start; i < kHist; i++) {
            sumE += histEnergy_[i];
            n += histCount_[i];
        }
        if (n == 0) return -HUGE_VAL;
        return -0.691 + 10.0 * log10(sumE / n);
    }
    double TruePeakDb() const { return 20.0 * log10(std::max(1e-10f, truePeak_)); }
    long long Blocks() const { return blocks_; }
    long long TpEvaluations() const { return tpEvals_; }  // channel-frames interpolated (tests, bench)

private:
    static constexpr int kHist = 800;  // -70 .. +10 LUFS in 0.1 LU bins
    static constexpr double kHistMin = -70.0;

    // True peak for one frame: shift the original samples in, evaluate the 4
    // phases where they could matter (see the class comment).
    void TruePeakFrame(const float* s) {
        for (int c = 0; c < ch_; c++) tpHist_[c][tpPos_] = s[c];
        for (int c = 0; c < ch_; c++) {
            float ax = fabsf(s[c]);
            truePeak_ = std::max(truePeak_, ax);
            if (ax * tpBound_ > truePeak_ || (ax != 0.f && truePeak_ < 1e-30f)) hot_[c] = kTpTaps;
            if (hot_[c] == 0) continue;
            hot_[c]--;
            tpEvals_++;
            for (int p = 0; p < 4; p++) {
                float acc = 0.f;
                int idx = tpPos_;
                for (int j = 0; j < kTpTaps; j++) {
                    acc += tp_[p][j] * tpHist_[c][idx];
                    idx = (idx == 0) ? kTpTaps - 1 : idx - 1;
                }
                truePeak_ = std::max(truePeak_, fabsf(acc));
            }
        }
        tpPos_ = (tpPos_ + 1 == kTpTaps) ? 0 : tpPos_ + 1;
    }

    double Window(int subs) const {
        if (subFilled_ < subs) return -HUGE_VAL;
        double s = 0.0;
        int i = subHead_;
        for (int k = 0; k < subs; k++) {
            i = (i == 0) ? 29 : i - 1;
            s += sub_[i];
        }
        double ms = s / ((double)subs * subLen_);
        return -0.691 + 10.0 * log10(std::max(ms, 1e-20));
    }
    void EndSubBlock() {
        sub_[subHead_] = subAcc_;
        subHead_ = (subHead_ + 1) % 30;
        if (subFilled_ < 30) subFilled_++;
        subAcc_ = 0.0;
        subCount_ = 0;
        if (subFilled_ >= 4) {
            double l = Window(4);
            if (l > kHistMin) {
                int bin = std::clamp((int)((l - kHistMin) * 10.0), 0, kHist - 1);
                histCount_[bin]++;
                histEnergy_[bin] += pow(10.0, (l + 0.691) / 10.0);
                blocks_++;
            }
        }
    }

    int fs_ = 48000, ch_ = 2, subLen_ = 4800;
    float w_[kMaxCh] = {};
    Biquad shelf_[kMaxCh], hp_[kMaxCh];
    float tp_[4][kTpTaps] = {};
    float tpHist_[kMaxCh][kTpTaps] = {};
    int tpPos_ = 0;
    float truePeak_ = 0.f;
    float tpBound_ = 2.f;          // S (1 + 2^-12), see the class comment
    int hot_[kMaxCh] = {};
    bool tpOn_ = true;
    long long tpEvals_ = 0;
    double subAcc_ = 0.0;
    int subCount_ = 0;
    double sub_[30] = {};
    int subHead_ = 0, subFilled_ = 0;
    int histCount_[kHist] = {};
    double histEnergy_[kHist] = {};
    long long blocks_ = 0;
};

// Stereo correlation, r = sum LR / sqrt(sum L^2 sum R^2), each sum an
// exponential average with time constant tau (SPAN averages over 500 ms).
// +1 mono, 0 unrelated, -1 out of phase.
class Correlation {
public:
    void Configure(int fs, double tauMs) {
        k_ = 1.0 - exp(-1000.0 / (std::max(1.0, tauMs) * std::max(1, fs)));
        Reset();
    }
    void Reset() { lr_ = ll_ = rr_ = 0.0; }
    // n frames of exact silence in one step: each Push(0, 0) scales the
    // three sums by (1 - k), so n of them scale by (1 - k)^n (equal to the
    // loop up to double rounding; the ratio Value() reads is unchanged).
    void PushSilence(int n) {
        if (n <= 0) return;
        double d = pow(1.0 - k_, n);
        lr_ *= d;
        ll_ *= d;
        rr_ *= d;
    }
    void Push(float l, float r) {
        lr_ += (l * (double)r - lr_) * k_;
        ll_ += (l * (double)l - ll_) * k_;
        rr_ += (r * (double)r - rr_) * k_;
    }
    double Value() const {
        double d = sqrt(ll_ * rr_);
        return (d > 1e-12) ? std::clamp(lr_ / d, -1.0, 1.0) : 0.0;
    }

private:
    double k_ = 0.0, lr_ = 0.0, ll_ = 0.0, rr_ = 0.0;
};

}  // namespace ttdsp

// ---- Performance Stats (2.0) -----------------------------------------------------------
//
// Cheap counters for the claims this mod makes about its own cost, so they can
// be checked on any PC rather than taken on trust. Every counter is one relaxed
// atomic add; the timers are two QueryPerformanceCounter reads. With
// Performance -> Performance Stats on, the engine thread writes one line to the
// Windhawk log every 30 seconds:
//
//   [Perf] 30.0 s: engine 144.0 wakes/s, 0.11 ms avg | analyses 100.0/s, FFTs 117.3/s |
//          ticks 144.0/s, render 0.06 ms avg | presents 98.2/s, skipped 45.8/s, text 0.3/s |
//          commits 0.0/s, maps 98.2/s | idle: playing 100% trickle 0% deep 0%
//
// The figures are per second of wall time, so they compare directly across
// settings, renderers and machines.

enum VizPerfCounter : int {
    kPerfEngineWakes,   // engine loop iterations
    kPerfAnalyses,      // frames that ran the spectrum analysis
    kPerfFfts,          // FFTs computed (all tiers)
    kPerfRenderTicks,   // render ticks handled on the UI thread
    kPerfPresents,      // panel surface presents (Direct3D 11) or frames drawn (Direct2D)
    kPerfSkipped,       // frames skipped because nothing changed
    kPerfTextPresents,  // text surface presents
    kPerfCommits,       // DirectComposition commits from the render path
    kPerfMaps,          // dynamic buffer uploads (Map / Unmap pairs)
    kPerfIdlePlaying,   // engine wakes spent in each idle state
    kPerfIdleTrickle,
    kPerfIdleDeep,
    kPerfCount
};
std::atomic<uint32_t> g_perfCount[kPerfCount];
std::atomic<uint64_t> g_perfEngineTicks{0}, g_perfRenderTicks{0};  // QPC ticks
std::atomic<bool> g_perfStatsEnabled{false};

inline void VizPerf(VizPerfCounter c, uint32_t n = 1) { g_perfCount[c].fetch_add(n, std::memory_order_relaxed); }

// Adds the time between construction and destruction to an accumulator.
struct VizPerfScope {
    std::atomic<uint64_t>& acc;
    LARGE_INTEGER t0;
    explicit VizPerfScope(std::atomic<uint64_t>& a) : acc(a) { QueryPerformanceCounter(&t0); }
    ~VizPerfScope() {
        LARGE_INTEGER t1;
        QueryPerformanceCounter(&t1);
        acc.fetch_add((uint64_t)(t1.QuadPart - t0.QuadPart), std::memory_order_relaxed);
    }
};

// Called by the engine thread once per loop. Logs and resets every 30 s.
void VizPerfMaybeLog() {
    static LARGE_INTEGER s_start = {}, s_freq = {};
    if (!s_freq.QuadPart) QueryPerformanceFrequency(&s_freq);
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    if (!s_start.QuadPart) {
        s_start = now;
        return;
    }
    double secs = (double)(now.QuadPart - s_start.QuadPart) / (double)s_freq.QuadPart;
    if (secs < 30.0) return;
    s_start = now;
    uint32_t c[kPerfCount];
    for (int i = 0; i < kPerfCount; i++) c[i] = g_perfCount[i].exchange(0, std::memory_order_relaxed);
    uint64_t eng = g_perfEngineTicks.exchange(0, std::memory_order_relaxed);
    uint64_t ren = g_perfRenderTicks.exchange(0, std::memory_order_relaxed);
    if (!g_perfStatsEnabled.load(std::memory_order_relaxed)) return;
    auto rate = [&](int i) { return c[i] / secs; };
    auto avgMs = [&](uint64_t t, uint32_t n) { return n ? (double)t * 1000.0 / (double)s_freq.QuadPart / n : 0.0; };
    double idleTotal = (double)std::max<uint32_t>(1, c[kPerfIdlePlaying] + c[kPerfIdleTrickle] + c[kPerfIdleDeep]);
    Wh_Log(L"[Perf] %.1f s: engine %.1f wakes/s, %.3f ms avg | analyses %.1f/s, FFTs %.1f/s | ticks %.1f/s, "
           L"render %.3f ms avg | presents %.1f/s, skipped %.1f/s, text %.1f/s | commits %.1f/s, maps %.1f/s | "
           L"idle: playing %.0f%% trickle %.0f%% deep %.0f%%",
           secs, rate(kPerfEngineWakes), avgMs(eng, c[kPerfEngineWakes]), rate(kPerfAnalyses), rate(kPerfFfts),
           rate(kPerfRenderTicks), avgMs(ren, c[kPerfRenderTicks]), rate(kPerfPresents), rate(kPerfSkipped),
           rate(kPerfTextPresents), rate(kPerfCommits), rate(kPerfMaps), 100.0 * c[kPerfIdlePlaying] / idleTotal,
           100.0 * c[kPerfIdleTrickle] / idleTotal, 100.0 * c[kPerfIdleDeep] / idleTotal);
}

// ---- Audio source ---------------------------------------------------------------------
//
// Up to 2.0 the mod always listened to the default playback device. Anyone who
// routes audio through virtual devices (VB-Audio Matrix / Voicemeeter / VAIO
// cables, an Ableton return on its own device, ...) had nothing to show, since
// the default device was silent. Now any active endpoint can be the source:
//
//   * an output device, captured the way the default one always was
//     (WASAPI loopback: what is being played to it), or
//   * an input device: a microphone, a line-in, or the capture side of a
//     virtual cable, read as an ordinary recording stream.
//
// The source is a short key string:
//   ""  / "default_output"   the default playback device (the old behaviour)
//   "default_input"          the default recording device
//   "id:<endpoint id>"       one exact device (what the right-click menu stores)
//   "name:<text>"            the first device whose name contains <text>
//                            (what the Windhawk setting stores, since a person
//                            can type a name but not an endpoint id)

static const PROPERTYKEY kPKEY_DeviceFriendlyName = {
    {0xa45c254e, 0xdf1c, 0x4efd, {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}}, 14};

std::wstring VizDeviceName(IMMDevice* d) {
    std::wstring out;
    ComPtr<IPropertyStore> ps;
    if (!d || FAILED(d->OpenPropertyStore(STGM_READ, &ps)) || !ps) return out;
    PROPVARIANT v;
    PropVariantInit(&v);
    if (SUCCEEDED(ps->GetValue(kPKEY_DeviceFriendlyName, &v)) && v.vt == VT_LPWSTR && v.pwszVal) out = v.pwszVal;
    PropVariantClear(&v);
    return out;
}

std::wstring VizDeviceId(IMMDevice* d) {
    std::wstring out;
    LPWSTR id = nullptr;
    if (d && SUCCEEDED(d->GetId(&id)) && id) {
        out = id;
        CoTaskMemFree(id);
    }
    return out;
}

bool VizDeviceIsRender(IMMDevice* d) {
    ComPtr<IMMEndpoint> ep;
    EDataFlow flow = eRender;
    if (d && SUCCEEDED(d->QueryInterface(__uuidof(IMMEndpoint), (void**)ep.GetAddressOf())) && ep)
        ep->GetDataFlow(&flow);
    return flow == eRender;
}

static bool ContainsNoCase(const std::wstring& hay, const std::wstring& needle) {
    if (needle.empty()) return false;
    auto it = std::search(hay.begin(), hay.end(), needle.begin(), needle.end(),
                          [](wchar_t a, wchar_t b) { return towlower(a) == towlower(b); });
    return it != hay.end();
}

struct VizAudioEndpoint {
    std::wstring id, name;
    bool render = true;
};

// Every active endpoint, outputs first. Used by the right-click menu.
std::vector<VizAudioEndpoint> VizListAudioEndpoints(IMMDeviceEnumerator* e) {
    std::vector<VizAudioEndpoint> out;
    if (!e) return out;
    for (EDataFlow flow : {eRender, eCapture}) {
        ComPtr<IMMDeviceCollection> col;
        if (FAILED(e->EnumAudioEndpoints(flow, DEVICE_STATE_ACTIVE, &col)) || !col) continue;
        UINT n = 0;
        col->GetCount(&n);
        for (UINT i = 0; i < n; i++) {
            ComPtr<IMMDevice> d;
            if (FAILED(col->Item(i, &d)) || !d) continue;
            VizAudioEndpoint ep;
            ep.id = VizDeviceId(d.Get());
            ep.name = VizDeviceName(d.Get());
            ep.render = (flow == eRender);
            if (ep.name.empty()) ep.name = ep.id;
            out.push_back(ep);
        }
    }
    return out;
}

// ---- Media Card (2.1): output switching and volume -------------------------------------
// Windows has no public call to change the default output; IPolicyConfig is
// the interface the Sound control panel itself uses, unchanged since Windows
// 7 (only SetDefaultEndpoint is called; the slots before it keep the vtable
// order).
MIDL_INTERFACE("f8679f50-850a-41cf-9c72-430f290290c8")
IVizPolicyConfig : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetMixFormat(PCWSTR, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceFormat(PCWSTR, INT, WAVEFORMATEX**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetDeviceFormat(PCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDeviceFormat(PCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProcessingPeriod(PCWSTR, INT, PINT64, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProcessingPeriod(PCWSTR, PINT64) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetShareMode(PCWSTR, void*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPropertyValue(PCWSTR, const PROPERTYKEY&, PROPVARIANT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDefaultEndpoint(PCWSTR, ERole) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEndpointVisibility(PCWSTR, INT) = 0;
};
static const CLSID kCLSID_VizPolicyConfigClient = {0x870af99c, 0x171d, 0x4f9e, {0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9}};
static const IID kIID_VizPolicyConfig = {0xf8679f50, 0x850a, 0x41cf, {0x9c, 0x72, 0x43, 0x0f, 0x29, 0x02, 0x90, 0xc8}};

namespace {
// The default output's volume control, kept between calls and fetched again
// after a switch, a failure, or every ten seconds (so a default changed
// elsewhere is picked up). Media window thread only.
ComPtr<IAudioEndpointVolume> s_cardVol;
ULONGLONG s_cardVolTick = 0;

// COM on the calling thread for the duration of a call, if it isn't already.
struct VizComScope {
    bool owned;
    VizComScope() : owned(SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))) {}
    ~VizComScope() {
        if (owned) CoUninitialize();
    }
};

IAudioEndpointVolume* CardVolume() {
    ULONGLONG now = GetTickCount64();
    if (s_cardVol && now - s_cardVolTick < 10000) return s_cardVol.Get();
    s_cardVol.Reset();
    s_cardVolTick = now;
    ComPtr<IMMDeviceEnumerator> e;
    ComPtr<IMMDevice> d;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&e))) ||
        FAILED(e->GetDefaultAudioEndpoint(eRender, eMultimedia, &d)) ||
        FAILED(d->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)s_cardVol.GetAddressOf())))
        s_cardVol.Reset();
    return s_cardVol.Get();
}
}  // namespace

float VizCardGetVolume() {
    VizComScope com;
    IAudioEndpointVolume* v = CardVolume();
    float level = -1.f;
    if (v && FAILED(v->GetMasterVolumeLevelScalar(&level))) {
        s_cardVol.Reset();
        level = -1.f;
    }
    return level;
}

void VizCardSetVolume(float level) {
    VizComScope com;
    IAudioEndpointVolume* v = CardVolume();
    if (v) v->SetMasterVolumeLevelScalar(std::clamp(level, 0.f, 1.f), nullptr);
}

// One click: the outputs, the current default ticked; picking one makes it
// the default for every role, as the Sound settings page does.
void VizCardOutputMenu(HWND hWnd, POINT pt) {
    VizComScope com;
    ComPtr<IMMDeviceEnumerator> e;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&e)))) return;
    std::wstring current;
    {
        ComPtr<IMMDevice> d;
        if (SUCCEEDED(e->GetDefaultAudioEndpoint(eRender, eMultimedia, &d))) current = VizDeviceId(d.Get());
    }
    std::vector<VizAudioEndpoint> outs;
    for (auto& ep : VizListAudioEndpoints(e.Get()))
        if (ep.render) outs.push_back(ep);
    if (outs.empty()) return;
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    for (size_t i = 0; i < outs.size(); i++)
        AppendMenuW(menu, MF_STRING | (outs[i].id == current ? MF_CHECKED : 0), 1 + i, outs[i].name.c_str());
    SetForegroundWindow(hWnd);  // so a click elsewhere closes the menu
    VizMediaMenuBegin();
    UINT cmd = (UINT)TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_BOTTOMALIGN, pt.x, pt.y, hWnd, nullptr);
    VizMediaMenuEnd();
    PostMessage(hWnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
    if (cmd < 1 || cmd > outs.size() || outs[cmd - 1].id == current) return;
    ComPtr<IVizPolicyConfig> pc;
    if (FAILED(CoCreateInstance(kCLSID_VizPolicyConfigClient, nullptr, CLSCTX_ALL, kIID_VizPolicyConfig,
                                (void**)pc.GetAddressOf())) || !pc) {
        Wh_Log(L"[Media] output switch unavailable");
        return;
    }
    for (ERole role : {eConsole, eMultimedia, eCommunications}) pc->SetDefaultEndpoint(outs[cmd - 1].id.c_str(), role);
    s_cardVol.Reset();  // the volume now belongs to the new output
}

// Resolves a source key to a device. Falls back to the default output (and
// says so through *fellBack) when the asked-for device isn't there, so a
// device that was unplugged or renamed leaves the visualizer working.
ComPtr<IMMDevice> VizResolveAudioDevice(IMMDeviceEnumerator* e, const std::wstring& key, bool* loopback,
                                        bool* fellBack) {
    ComPtr<IMMDevice> d;
    *fellBack = false;
    *loopback = true;
    if (!e) return d;
    if (key == L"default_input") {
        if (SUCCEEDED(e->GetDefaultAudioEndpoint(eCapture, eConsole, &d)) && d) {
            *loopback = false;
            return d;
        }
        *fellBack = true;
    } else if (key.rfind(L"id:", 0) == 0) {
        if (SUCCEEDED(e->GetDevice(key.c_str() + 3, &d)) && d) {
            DWORD state = 0;
            if (SUCCEEDED(d->GetState(&state)) && state == DEVICE_STATE_ACTIVE) {
                *loopback = VizDeviceIsRender(d.Get());
                return d;
            }
        }
        d.Reset();
        *fellBack = true;
    } else if (key.rfind(L"name:", 0) == 0) {
        std::wstring want = key.substr(5);
        for (const auto& ep : VizListAudioEndpoints(e)) {
            if (ContainsNoCase(ep.name, want) && SUCCEEDED(e->GetDevice(ep.id.c_str(), &d)) && d) {
                *loopback = ep.render;
                return d;
            }
        }
        d.Reset();
        *fellBack = true;
    }
    if (SUCCEEDED(e->GetDefaultAudioEndpoint(eRender, eConsole, &d))) *loopback = true;
    return d;
}

// The source in effect, as the engine thread reads it. Written on the UI
// thread by VizPublishAudioSource (from LoadSettings); a change flags a
// reopen the same way a device change does.
std::mutex g_audioSourceMutex;
std::wstring g_audioSourceKeyShared;
std::atomic<bool> g_audioOnFallback{false};

std::wstring VizAudioSourceKey() {
    std::lock_guard<std::mutex> lock(g_audioSourceMutex);
    return g_audioSourceKeyShared;
}

// Whether a change of Windows' default device for `flow` affects the stream:
// only when the source is that default, or the chosen device is missing and
// the default output is standing in for it.
bool VizSourceFollowsDefault(EDataFlow flow) {
    std::wstring key = VizAudioSourceKey();
    if (flow == eCapture) return key == L"default_input";
    return key.empty() || key == L"default_output" || g_audioOnFallback.load(std::memory_order_relaxed);
}

// A readable name for a source key, for messages.
std::wstring VizAudioSourceLabel(const std::wstring& key) {
    if (key.empty() || key == L"default_output") return L"default output";
    if (key == L"default_input") return L"default input";
    if (key.rfind(L"name:", 0) == 0) return L"\"" + key.substr(5) + L"\"";
    return L"the device picked from the right-click menu";
}

// Reported through the message window as an Audio settings warning (the
// receiver frees the string).
void VizPostAudioNotice(const std::wstring& msg) {
    Wh_Log(L"[Audio] %s", msg.c_str());
    HWND wnd = g_messageWnd;
    if (!wnd) return;
    auto* heap = new std::wstring(msg);
    if (!PostMessage(wnd, WM_APP_HW_NOTICE, 1, (LPARAM)heap)) delete heap;
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
        if ((flow == eRender || flow == eCapture) && VizSourceFollowsDefault(flow))
            g_deviceChanged.store(true, std::memory_order_relaxed);
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

// ---- Audio engine: capture, analysis and what the renderer reads -----------------
//
// 1.5 ran two threads to get audio onto the screen. A capture thread woke on
// every WASAPI event, about 100 times a second while anything played, and a
// pacing thread woke once per frame to post the render tick. Every wake is a
// core leaving a sleep state, which is the cost this mod has always been built
// around, so 2.0 merges them: the pacing thread also owns the loopback client,
// and once per frame it drains whatever audio has arrived, analyses it, and
// posts the tick. Audio and video share one wake.
//
// That only works because loopback is drained by polling rather than by the
// event, so the client is opened with a 500 ms buffer (1.5 asked for 20 ms,
// which was fine for an event every 10 ms and is not for a frame every 33 ms
// at a 30 FPS target). The event is still registered; it is what the thread
// waits on while idle, because WASAPI only signals it when audio arrives.
//
// Idle has three steps now:
//   playing       one wake per frame, as above.
//   trickle       Pause When Silent reached. Loopback: the audio event wakes
//                 the thread, which only drains the packet unless it is
//                 louder than the audible level (then it analyses and draws
//                 at once, so waking up costs no latency); otherwise one
//                 analysis + render tick every 250 ms. An app holding a silent
//                 stream open signals the event 100 times a second, and those
//                 wakes now cost a drain each instead of a full frame. An
//                 input device signals every device period whatever it hears,
//                 so for one the thread just polls every 250 ms.
//   deep idle     5 s after that, loopback only and only with a working peak
//                 meter: the stream is stopped and the endpoint's peak meter
//                 is read 4 times a second instead. A running capture stream
//                 registers an audio power request, which can hold the PC out
//                 of sleep; a stopped one doesn't. A meter reading above the
//                 audible level (the same -70 dBFS the engine uses, Input Gain
//                 included) only restarts the stream and goes back to trickle
//                 for a 1 s look; the engine's own test then decides whether
//                 it is playing. An input device stays in trickle: once its
//                 stream is stopped, its peak meter can read 0 for good.

enum class VizIdleState { Playing, Trickle, Deep };

// The one "is anything playing" level, -70 dBFS on the mono mix after Input
// Gain. The engine needs this and a bar above 2% to call audio audible; deep
// idle wakes on the endpoint meter crossing it. The meter reads the loudest
// channel before Input Gain, and |mono mix| <= the loudest channel, so meter
// x gain is never below what the engine sees: nothing the engine would call
// audible can sleep through deep idle.
constexpr float kVizAudibleLin = 0.000316f;

// IAudioMeterInformation (endpointvolume.h), declared here because the mingw
// headers only forward-declare it. Methods in vtable order, from the Windows
// SDK; only GetPeakValue is called.
struct IVizAudioMeter : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* pdwHardwareSupportMask) = 0;
};
static const IID kIID_IAudioMeterInformation = {
    0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

// Precision bands, written by the engine thread once per frame, read by the
// UI thread once per drawn frame. Same seqlock pattern as the 7 classic bands.
struct VizBandFrame {
    int count = 0;
    float level[VIZ_BARS_MAX] = {};  // display height 0..1, after ballistics
    float zone[3] = {};              // summed level per EQ zone (low / mid / high)
    float zoneCount[3] = {};         // bands in each zone, for averages (Terminal meters)
};
std::atomic<uint32_t> g_bandFrameSeq{0};
VizBandFrame g_bandFrame;

void PublishBandFrame(const VizBandFrame& f) {
    uint32_t seq = g_bandFrameSeq.load(std::memory_order_relaxed);
    g_bandFrameSeq.store(seq + 1, std::memory_order_release);
    std::atomic_thread_fence(std::memory_order_release);
    g_bandFrame.count = f.count;
    memcpy(g_bandFrame.level, f.level, sizeof(float) * (size_t)std::clamp(f.count, 0, VIZ_BARS_MAX));
    memcpy(g_bandFrame.zone, f.zone, sizeof(f.zone));
    memcpy(g_bandFrame.zoneCount, f.zoneCount, sizeof(f.zoneCount));
    g_bandFrameSeq.store(seq + 2, std::memory_order_release);
}

void ReadBandFrame(VizBandFrame& dst) {
    for (;;) {
        uint32_t seq1 = g_bandFrameSeq.load(std::memory_order_acquire);
        if (seq1 & 1) continue;
        dst.count = std::clamp(g_bandFrame.count, 0, VIZ_BARS_MAX);
        memcpy(dst.level, g_bandFrame.level, sizeof(float) * (size_t)dst.count);
        memcpy(dst.zone, g_bandFrame.zone, sizeof(dst.zone));
        memcpy(dst.zoneCount, g_bandFrame.zoneCount, sizeof(dst.zoneCount));
        std::atomic_thread_fence(std::memory_order_acquire);
        if (seq1 == g_bandFrameSeq.load(std::memory_order_relaxed)) return;
    }
}

// What the engine thread needs from the settings, copied as one value on the
// UI thread (LoadSettings) so the engine never reads half of a settings change.
struct VizEngineConfig {
    bool precision = true;
    VizWorkload workload = VizWorkload::Hybrid;
    ttdsp::SpectrumEngine::Config spec;
    ttdsp::Ballistics ball;
    ttdsp::DisplayMap disp;
    float sensDb = 0.f;
    bool autoGain = false;
    float autoGainMaxDb = 12.f;
    VizChannel channel = VizChannel::Mix;
    bool wantDominant = false;
    bool wantLoudness = false;
    bool wantTruePeak = false;     // Loudness (full) readout: the 4x oversampler
    bool wantCorrelation = false;  // Loudness (full) readout or the Goniometer
    bool wantGonio = false;
    bool beat = false;
    bool loudnessResetOnTrack = true;
};
std::mutex g_engineCfgMutex;
VizEngineConfig g_engineCfg;
std::atomic<int> g_engineCfgGen{1};

// GPU workload feed: the band layout (changes rarely) and the newest
// unwindowed block of each tier (changes every frame). The UI thread uploads
// both; the GPU does the rest.
struct VizGpuFeed {
    std::mutex m;
    int layoutSerial = 0;
    int n = 0;
    double rate[3] = {48000, 12000, 3000};
    bool used[3] = {true, false, false};
    double peakCal = 0.0;
    std::vector<ttdsp::SpectrumEngine::BandMap> maps;
    std::vector<ttdsp::Band> bands;
    std::vector<float> window;
    std::vector<float> blocks;  // 3 x n
    unsigned dirty = 0;         // tiers with a new block the UI hasn't taken
} g_gpuFeed;

// Loudness, true peak and correlation, for the readout.
struct VizMeterValues {
    double momentary = -HUGE_VAL, shortTerm = -HUGE_VAL, integrated = -HUGE_VAL;
    double truePeak = -HUGE_VAL, correlation = 0.0;
    bool valid = false;
};
std::mutex g_meterMutex;
VizMeterValues g_meters;

// Goniometer: the newest stereo samples as (side, mid) pairs.
std::mutex g_gonioMutex;
std::vector<float> g_gonioXY;
std::atomic<uint32_t> g_gonioSerial{0};
std::atomic<uint32_t> g_vizStereoRate{48000};  // sample rate of g_gonioXY, for Stereo Field

// Engine thread control.
std::atomic<bool> g_captureWanted{false};
std::atomic<bool> g_audioOpen{false};
HANDLE g_engineWake = nullptr;    // auto-reset: settings, pause, resume, unload
HANDLE g_engineClosed = nullptr;  // manual-reset: set while the stream is closed
std::atomic<int> g_idleState{(int)VizIdleState::Playing};

// Opens the chosen audio source (see VizResolveAudioDevice): an output
// device by loopback, as 1.5 always did with the default one, or an input
// device as a plain recording stream. Same buffer and outputs as before: the
// channel mask for loudness weighting, and the device itself for the
// deep-idle peak meter. *fellBack says the chosen device wasn't there, or was
// there but couldn't be opened (*openFailed: typically a DAW holding it in
// exclusive mode); either way the default output stands in for it.
static bool VizOpenAudioClientOn(IMMDevice* pDev, bool loopback, ComPtr<IAudioClient>& pClient,
                                 ComPtr<IAudioCaptureClient>& pCapture, UINT32& sampleRate, UINT32& channels,
                                 bool& isFloat, DWORD& channelMask, HANDLE hEvent) {
    ComPtr<IAudioClient> pC;
    if (FAILED(pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)pC.GetAddressOf())))
        return false;

    WAVEFORMATEX* pwfx = nullptr;
    pC->GetMixFormat(&pwfx);
    if (!pwfx) return false;

    UINT32 sr = pwfx->nSamplesPerSec, ch = pwfx->nChannels;
    bool fl = (pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) ||
              (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
               reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
    DWORD mask = (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
                     ? reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx)->dwChannelMask
                     : 0;

    // 500 ms, in 100 ns units.
    HRESULT hr = pC->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                (loopback ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0u) | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                5000000, 0, pwfx, nullptr);
    CoTaskMemFree(pwfx);
    if (FAILED(hr)) return false;

    if (hEvent) pC->SetEventHandle(hEvent);

    ComPtr<IAudioCaptureClient> pCap;
    if (FAILED(pC->GetService(__uuidof(IAudioCaptureClient), (void**)pCap.GetAddressOf()))) return false;

    if (FAILED(pC->Start())) return false;

    sampleRate = sr;
    channels = ch;
    isFloat = fl;
    channelMask = mask;
    pClient = pC;
    pCapture = pCap;
    return true;
}

bool VizInitAudioClient(IMMDeviceEnumerator* pEnum, ComPtr<IMMDevice>& pDevOut,
                        ComPtr<IAudioClient>& pClient, ComPtr<IAudioCaptureClient>& pCapture,
                        UINT32& sampleRate, UINT32& channels, bool& isFloat, DWORD& channelMask,
                        HANDLE hEvent, bool* loopbackOut, bool* fellBack, bool* openFailed) {
    pClient.Reset();
    pCapture.Reset();
    pDevOut.Reset();
    *openFailed = false;

    bool loopback = true;
    std::wstring key = VizAudioSourceKey();
    ComPtr<IMMDevice> pDev = VizResolveAudioDevice(pEnum, key, &loopback, fellBack);
    if (!pDev) return false;
    if (VizOpenAudioClientOn(pDev.Get(), loopback, pClient, pCapture, sampleRate, channels, isFloat, channelMask,
                             hEvent)) {
        *loopbackOut = loopback;
        pDevOut = pDev;
        return true;
    }
    // The chosen device is there but won't open. This used to retry the
    // same device every 500 ms for as long as it stayed busy, showing
    // nothing; now the default output stands in (once per attempt), and the
    // caller says so and checks back on the chosen one now and then.
    if (*fellBack || key.empty() || key == L"default_output") return false;
    ComPtr<IMMDevice> def;
    if (FAILED(pEnum->GetDefaultAudioEndpoint(eRender, eConsole, &def)) || !def) return false;
    if (!VizOpenAudioClientOn(def.Get(), true, pClient, pCapture, sampleRate, channels, isFloat, channelMask,
                              hEvent))
        return false;
    Wh_Log(L"[Audio] %s would not open; using the default output instead", VizDeviceName(pDev.Get()).c_str());
    *fellBack = true;
    *openFailed = true;
    *loopbackOut = true;
    pDevOut = def;
    return true;
}

// Whether the chosen source can be opened now, without disturbing the stream
// that is standing in for it: a shared-mode Initialize on a client that is
// released straight away (it is never started, so nothing is captured).
static bool VizProbeAudioSource(IMMDeviceEnumerator* pEnum) {
    bool loopback = true, fellBack = false;
    ComPtr<IMMDevice> d = VizResolveAudioDevice(pEnum, VizAudioSourceKey(), &loopback, &fellBack);
    if (!d || fellBack) return false;
    ComPtr<IAudioClient> c;
    if (FAILED(d->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)c.GetAddressOf()))) return false;
    WAVEFORMATEX* pwfx = nullptr;
    c->GetMixFormat(&pwfx);
    if (!pwfx) return false;
    HRESULT hr = c->Initialize(AUDCLNT_SHAREMODE_SHARED, loopback ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0u, 5000000, 0,
                               pwfx, nullptr);
    CoTaskMemFree(pwfx);
    return SUCCEEDED(hr);
}

class VizEngine {
public:
    // ---- Thread lifetime --------------------------------------------------------
    void ThreadInit() {
        BuildVizSeeds();
        if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                    __uuidof(IMMDeviceEnumerator), (void**)enum_.GetAddressOf()))) {
            enum_.Reset();
        }
        if (enum_) {
            notify_ = new VizEndpointNotificationClient();
            notifyRegistered_ = SUCCEEDED(enum_->RegisterEndpointNotificationCallback(notify_));
        }
        audioEvent_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        ring_.assign(RING_CAP, 0.f);
    }
    void ThreadExit() {
        Close();
        if (enum_ && notify_ && notifyRegistered_) enum_->UnregisterEndpointNotificationCallback(notify_);
        if (notify_) notify_->Release();
        notify_ = nullptr;
        enum_.Reset();
        if (audioEvent_) CloseHandle(audioEvent_);
        audioEvent_ = nullptr;
    }
    HANDLE AudioEvent() const { return audioEvent_; }
    bool IsOpen() const { return (bool)client_; }
    bool IsStopped() const { return stopped_; }
    // Loopback (an output device) or a recording stream (an input device),
    // as opened. The idle ladder treats the two differently.
    bool IsLoopback() const { return loopback_; }
    // A peak meter that answered its last read. Deep idle needs one.
    bool MeterOk() const { return meter_ && meterOk_; }

    // Opens the stream if it isn't, or reopens it after a device change. A
    // source that won't open is retried after 0.5 s, then 1, 2, 4 ... up to
    // 30 s, and straight away again after any device change. While the
    // default output is standing in for a chosen device that wouldn't open,
    // the chosen one is probed every 30 s and taken back when it's free.
    void EnsureOpen() {
        bool changed = g_deviceChanged.exchange(false, std::memory_order_relaxed);
        ULONGLONG now = GetTickCount64();
        if (changed) retryMs_ = kRetryMinMs;
        if (client_ && !changed) {
            if (!openFallback_ || now - lastProbe_ < kProbeMs) return;
            lastProbe_ = now;
            if (!VizProbeAudioSource(enum_.Get())) return;
            Wh_Log(L"[Audio] the chosen source opens again; switching back to it");
            retryMs_ = kRetryMinMs;
            lastReinit_ = 0;
        }
        if (now - lastReinit_ < retryMs_) {
            if (changed) g_deviceChanged.store(true, std::memory_order_relaxed);
            return;
        }
        lastReinit_ = now;
        Close();
        if (!enum_) return;
        UINT32 sr = 48000, ch = 2;
        bool fl = true;
        DWORD mask = 0;
        bool loopback = true, fellBack = false, openFailed = false;
        std::wstring source = VizAudioSourceKey();
        if (VizInitAudioClient(enum_.Get(), device_, client_, capture_, sr, ch, fl, mask, audioEvent_, &loopback,
                               &fellBack, &openFailed)) {
            sampleRate_ = sr;
            channels_ = ch;
            isFloat_ = fl;
            channelMask_ = mask;
            loopback_ = loopback;
            stopped_ = false;
            retryMs_ = kRetryMinMs;
            openFallback_ = openFailed;
            lastProbe_ = now;
            meter_.Reset();
            device_->Activate(kIID_IAudioMeterInformation, CLSCTX_ALL, nullptr, (void**)meter_.GetAddressOf());
            float probe = 0.f;
            meterOk_ = meter_ && SUCCEEDED(meter_->GetPeakValue(&probe));
            ResetAnalysis();
            configuredGen_ = 0;  // sample rate may have changed: rebuild the precision engine
            g_audioOpen.store(true);
            if (g_engineClosed) ResetEvent(g_engineClosed);
            Wh_Log(L"[Audio] %s open (%s): %u Hz, %u ch, %s", loopback ? L"loopback" : L"capture",
                   VizDeviceName(device_.Get()).c_str(), sr, ch, fl ? L"float" : L"int16");
            // Said once per source: a missing device is worth one heads-up,
            // not one per reconnect attempt.
            if (fellBack && source != warnedSource_) {
                warnedSource_ = source;
                if (openFailed)
                    VizPostAudioNotice(L"The chosen audio source (" + VizAudioSourceLabel(source) +
                                       L") is connected but couldn't be opened (another app may be using it in "
                                       L"exclusive mode), so the visualizer is listening to the default output "
                                       L"until it can.");
                else
                    VizPostAudioNotice(L"The chosen audio source (" + VizAudioSourceLabel(source) +
                                       L") isn't connected or enabled, so the visualizer is listening to the "
                                       L"default output until it comes back.");
            } else if (!fellBack) {
                warnedSource_.clear();
            }
            g_audioOnFallback.store(fellBack, std::memory_order_relaxed);
        } else {
            ULONGLONG was = retryMs_;
            retryMs_ = std::min<ULONGLONG>(retryMs_ * 2, kRetryMaxMs);
            if (retryMs_ != was) Wh_Log(L"[Audio] no audio source could be opened; next try in %llu ms", retryMs_);
        }
    }

    void Close() {
        if (client_) client_->Stop();
        capture_.Reset();
        client_.Reset();
        meter_.Reset();
        meterOk_ = false;
        device_.Reset();
        stopped_ = false;
        ResetAnalysis();
        g_audioOpen.store(false);
        if (g_engineClosed) SetEvent(g_engineClosed);
    }

    // Deep idle: stop the stream, keep the client (restarting it is instant).
    // Loopback only: an input device's meter may read nothing once our
    // stream is stopped, which would strand it in deep idle.
    void StopStream() {
        if (client_ && !stopped_ && loopback_) {
            client_->Stop();
            stopped_ = true;
            Wh_Log(L"[Idle] deep idle: loopback stopped, watching the peak meter");
        }
    }
    void StartStream() {
        if (client_ && stopped_) {
            client_->Start();
            stopped_ = false;
            lastPacketQpc_ = 0;
            Wh_Log(L"[Idle] loopback restarted");
        }
    }
    // The endpoint's own peak meter, 0..1 (the loudest channel, before Input
    // Gain). Reading it costs one COM call and needs no stream of ours to be
    // running. False when there is no meter or the read failed; the caller
    // then must not trust silence (and MeterOk() turns false, so deep idle
    // isn't entered again on this stream).
    bool ReadMeter(float* peak) {
        *peak = 0.f;
        if (!meter_) return false;
        HRESULT hr = meter_->GetPeakValue(peak);
        meterOk_ = SUCCEEDED(hr);
        return meterOk_;
    }

    // ---- Per frame ------------------------------------------------------------------
    // Drains whatever has arrived into the analysis inputs (rings, meters)
    // without analysing it. Cheap: what trickle does on each audio event.
    // Returns true when the audio since the last Frame() got loud enough to
    // be worth looking at now: above the audible level and 6 dB above what
    // the last frame saw, so a steady noise floor above -70 dBFS (a hum on a
    // virtual cable, an app's dither) doesn't turn every event into a frame,
    // while music starting still does at once.
    bool Pump() {
        SyncConfig();
        if (client_ && !stopped_) pendingGot_ += Drain();
        float trigger = std::max(kVizAudibleLin, 2.f * lastFramePeak_);
        return pendingGot_ > 0 && blockPeak_ > trigger;
    }

    // Drains the stream, runs the analysis the settings ask for, and
    // publishes everything the renderer reads. dt is the time since the last
    // call, which the ballistics and the classic silence decay are scaled by.
    // Returns false when nothing the renderer reads has changed (settled
    // digital silence), so an idle caller can skip the render tick.
    bool Frame(double dt) {
        Pump();
        dt = std::clamp(dt, 0.0005, 0.25);
        int got = pendingGot_;
        bool changed = PublishScope();
        if (cfg_.precision) changed |= PrecisionFrame(dt, got);
        else {
            ClassicFrame(dt, got);
            changed = true;
        }
        if (cfg_.wantLoudness || cfg_.wantGonio) changed |= PublishMeters();
        if (cfg_.wantGonio && got > 0) {
            PublishGonio();
            changed = true;
        }
        lastFramePeak_ = blockPeak_;
        blockPeak_ = 0.f;
        pendingGot_ = 0;
        return changed;
    }

private:
    static constexpr int RING_CAP = VIZ_FFT_SIZE_MAX * 4;
    static constexpr ULONGLONG kRetryMinMs = 500, kRetryMaxMs = 30000, kProbeMs = 30000;

    void ResetAnalysis() {
        std::fill(ring_.begin(), ring_.end(), 0.f);
        ringHead_ = ringCount_ = 0;
        for (int b = 0; b < VIZ_NUM_BANDS; b++) bandEnv_[b] = 0.f;
        PublishBands(bandEnv_);
        currentFftSize_ = 0;
        std::fill(std::begin(bandLevel_), std::end(bandLevel_), 0.f);
        VizBandFrame empty;
        PublishBandFrame(empty);
        lastPacketQpc_ = 0;
        blockPeak_ = lastFramePeak_ = 0.f;
        pendingGot_ = 0;
        zeroRun_ = 0;
        silentFinal_ = settled_ = gpuSilentFlushed_ = false;
    }

    // Picks up a settings change. The precision engine also has to be rebuilt
    // when the sample rate changes, which EnsureOpen signals by clearing the
    // generation it last built for.
    void SyncConfig() {
        int gen = g_engineCfgGen.load(std::memory_order_acquire);
        if (gen == configuredGen_) return;
        configuredGen_ = gen;
        {
            std::lock_guard<std::mutex> lock(g_engineCfgMutex);
            cfg_ = g_engineCfg;
        }
        cfg_.spec.sampleRate = (int)sampleRate_;
        cfg_.spec.maxBands = VIZ_BARS_MAX;
        if (cfg_.precision) {
            spec_.Configure(cfg_.spec);
            const auto& bands = spec_.Bands();
            for (size_t b = 0; b < bands.size() && b < (size_t)VIZ_BARS_MAX; b++) {
                double fc = bands[b].fc;
                bandZone_[b] = (fc < 300.0) ? 0 : (fc < 2500.0) ? 1 : 2;
                bandBass_[b] = fc < 150.0;
            }
            std::fill(std::begin(bandLevel_), std::end(bandLevel_), 0.f);
            agcDb_ = 0.f;
            loudEnvDb_ = -200.f;
            if (cfg_.workload == VizWorkload::Gpu) {
                std::lock_guard<std::mutex> lock(g_gpuFeed.m);
                g_gpuFeed.layoutSerial++;
                g_gpuFeed.n = spec_.Cfg().fftSize;
                for (int t = 0; t < 3; t++) {
                    g_gpuFeed.rate[t] = spec_.TierRate(t);
                    g_gpuFeed.used[t] = spec_.TierUsed(t);
                }
                g_gpuFeed.peakCal = spec_.PeakCal();
                g_gpuFeed.maps = spec_.Maps();
                g_gpuFeed.bands = bands;
                g_gpuFeed.window.assign(spec_.Window(), spec_.Window() + g_gpuFeed.n);
                g_gpuFeed.blocks.assign((size_t)3 * g_gpuFeed.n, 0.f);
                g_gpuFeed.dirty = 7;
            }
        }
        // Loudness weights from the channel mask: BS.1770 counts the
        // surrounds 1.41 times, leaves LFE out, and treats everything else as
        // a front channel.
        float w[ttdsp::LoudnessMeter::kMaxCh];
        for (UINT32 c = 0, bit = 0; c < (UINT32)ttdsp::LoudnessMeter::kMaxCh; c++) {
            w[c] = 1.f;
            if (!channelMask_) continue;
            while (bit < 32 && !(channelMask_ & (1u << bit))) bit++;
            DWORD sp = (bit < 32) ? (1u << bit) : 0;
            bit++;
            if (sp == SPEAKER_LOW_FREQUENCY) w[c] = 0.f;
            else if (sp == SPEAKER_BACK_LEFT || sp == SPEAKER_BACK_RIGHT || sp == SPEAKER_SIDE_LEFT ||
                     sp == SPEAKER_SIDE_RIGHT)
                w[c] = 1.41f;
        }
        loudness_.Configure((int)sampleRate_, (int)std::min<UINT32>(channels_, ttdsp::LoudnessMeter::kMaxCh), w);
        loudness_.SetTruePeak(cfg_.wantTruePeak);
        correlation_.Configure((int)sampleRate_, 300.0);
        // Digital silence (see PrecisionFrame): zeros enough to fill the
        // deepest tier's whole ring (2 N at fs / 16) plus every decimator's
        // history, after which every input the analysis can see is zero.
        {
            int deepest = 0;
            if (cfg_.precision) deepest = spec_.TierUsed(2) ? 2 : spec_.TierUsed(1) ? 1 : 0;
            silentNeeded_ = ((long long)2 * std::max(256, spec_.Cfg().fftSize) << (2 * deepest)) + 1024;
            zeroRun_ = 0;
            silentFinal_ = settled_ = gpuSilentFlushed_ = false;
        }
        lastMeters_.valid = false;  // publish the meters on the next frame whatever they read
        lastTrackTick_ = g_nowPlayingChangedTick.load(std::memory_order_relaxed);
    }

    // ---- Drain ----------------------------------------------------------------------
    // Returns the number of frames that arrived (silent packets included).
    int Drain() {
        UINT32 packetSize = 0;
        HRESULT hr = capture_->GetNextPacketSize(&packetSize);
        if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
            g_deviceChanged.store(true, std::memory_order_relaxed);
            return 0;
        }
        if (FAILED(hr)) return 0;

        // Input Gain is folded into the mixdown, as in 1.5, so it reaches the
        // scope, the bars and the meters alike.
        float inputGainLin = (g_settings.inputGainDb == 0.0f) ? 1.0f : powf(10.f, g_settings.inputGainDb / 20.f);
        UINT32 ch = std::max<UINT32>(1, channels_);
        float monoScale = inputGainLin / (float)ch;
        int total = 0;  // blockPeak_ accumulates until Frame() takes it

        while (packetSize > 0) {
            BYTE* pData = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;
            HRESULT hrBuf = capture_->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
            if (hrBuf == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_deviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hrBuf)) break;
            bool silent = (flags & AUDCLNT_BUFFERFLAGS_SILENT) || !pData;
            if (numFrames > 0) {
                total += (int)numFrames;
                ConvertPacket(silent ? nullptr : pData, numFrames, ch, inputGainLin, monoScale);
            }
            capture_->ReleaseBuffer(numFrames);
            hr = capture_->GetNextPacketSize(&packetSize);
            if (hr == AUDCLNT_E_DEVICE_INVALIDATED) {
                g_deviceChanged.store(true, std::memory_order_relaxed);
                break;
            }
            if (FAILED(hr)) break;
        }
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        if (total > 0) lastPacketQpc_ = q.QuadPart;
        return total;
    }

    // One packet: the classic mono ring (scope and classic bands), the
    // precision engine's chosen channel, loudness, correlation, goniometer.
    // A silent packet (pData null) is real silence of known length: it is fed
    // to the precision engine and the meters as zeros, so they decay on time,
    // while the classic ring, as in 1.5, is left alone. Each consumer only
    // gets work done for it when it is in use: the interleaved copy for
    // loudness, L/R correlation for the full loudness readout or the
    // Goniometer, the stereo history for the Goniometer.
    void ConvertPacket(const BYTE* pData, UINT32 frames, UINT32 ch, float gain, float monoScale) {
        const bool prec = cfg_.precision;
        const bool loud = cfg_.wantLoudness;
        const bool corr = cfg_.wantCorrelation;
        const bool gonio = cfg_.wantGonio;
        if (prec) mono_.resize(frames);
        if (loud) inter_.resize((size_t)frames * ch);
        if (gonio) stereo_.resize((size_t)frames * 2);
        if (!pData) {
            if (prec) {
                // Once the analysis holds nothing but zeros, more zeros change
                // none of its state, so they needn't be pushed.
                if (zeroRun_ < silentNeeded_) {
                    std::fill(mono_.begin(), mono_.end(), 0.f);
                    spec_.Push(mono_.data(), (int)frames);
                }
                zeroRun_ += frames;
            }
            if (loud) {
                std::fill(inter_.begin(), inter_.end(), 0.f);
                loudness_.Process(inter_.data(), (int)frames, (int)ch);
            }
            if (corr) correlation_.PushSilence((int)frames);
            if (gonio) {
                std::fill(stereo_.begin(), stereo_.end(), 0.f);
                AppendGonio(frames);
            }
            return;
        }
        const float* f32 = reinterpret_cast<const float*>(pData);
        const INT16* i16 = reinterpret_cast<const INT16*>(pData);
        int lastNonZero = -1;
        for (UINT32 f = 0; f < frames; f++) {
            float l = 0.f, r = 0.f, sum = 0.f;
            for (UINT32 c = 0; c < ch; c++) {
                float v = isFloat_ ? f32[f * ch + c] : i16[f * ch + c] / 32768.f;
                sum += v;
                if (loud) inter_[(size_t)f * ch + c] = v * gain;
                if (c == 0) l = v;
                if (c == 1) r = v;
            }
            if (ch == 1) r = l;
            float mono = sum * monoScale;
            ring_[ringHead_] = mono;
            ringHead_ = (ringHead_ + 1) % RING_CAP;
            if (ringCount_ < RING_CAP) ringCount_++;
            blockPeak_ = std::max(blockPeak_, fabsf(mono));
            l *= gain;
            r *= gain;
            if (prec) {
                float v;
                switch (cfg_.channel) {
                    case VizChannel::Left: v = l; break;
                    case VizChannel::Right: v = r; break;
                    case VizChannel::Mid: v = 0.5f * (l + r); break;
                    case VizChannel::Side: v = 0.5f * (l - r); break;
                    default: v = mono; break;
                }
                mono_[f] = v;
                if (v != 0.f) lastNonZero = (int)f;
            }
            if (corr) correlation_.Push(l, r);
            if (gonio) {
                stereo_[2 * f] = l;
                stereo_[2 * f + 1] = r;
            }
        }
        // One clock read per packet (1.5 read it per sample).
        if (frames > 0) {
            lastAudioTick_ = GetTickCount64();
            scopeFlatPublished_ = false;
        }
        if (prec) {
            if (lastNonZero >= 0) {
                if (zeroRun_ >= silentNeeded_) wakeFromSilence_ = true;
                zeroRun_ = (long long)frames - 1 - lastNonZero;
            } else {
                zeroRun_ += frames;
            }
            spec_.Push(mono_.data(), (int)frames);
        }
        if (loud) loudness_.Process(inter_.data(), (int)frames, (int)ch);
        if (gonio) AppendGonio(frames);
    }

    void AppendGonio(UINT32 frames) {
        // Keep the newest 2048 stereo frames.
        const size_t keep = 2048;
        gonioPending_.insert(gonioPending_.end(), stereo_.begin(), stereo_.begin() + (size_t)frames * 2);
        if (gonioPending_.size() > keep * 2)
            gonioPending_.erase(gonioPending_.begin(), gonioPending_.end() - keep * 2);
    }

    // ---- Oscilloscope trace (1.5, unchanged apart from where it lives) ---------------
    // Returns whether a new trace was published.
    bool PublishScope() {
        if (g_settings.shape != VizShape::Oscilloscope) return false;
        static constexpr double SCOPE_HOLD_MS = 60.0;
        static constexpr double SCOPE_FADE_MS = 140.0;
        double sinceAudioMs = (double)(GetTickCount64() - lastAudioTick_);
        float staleFade = 1.0f;
        if (sinceAudioMs > SCOPE_HOLD_MS)
            staleFade = std::max(0.0f, 1.0f - (float)((sinceAudioMs - SCOPE_HOLD_MS) / SCOPE_FADE_MS));
        if (staleFade <= 0.0f && scopeFlatPublished_) return false;

        auto ringAt = [&](int i) -> float {
            int m = i % RING_CAP;
            if (m < 0) m += RING_CAP;
            return ring_[m];
        };
        float windowMs = std::clamp(g_settings.oscilloscopeWindowMs, 5.f, 250.f);
        int span = (int)((double)sampleRate_ * (double)windowMs / 1000.0);
        span = std::clamp(span, VIZ_WAVE_SAMPLES, RING_CAP / 2);
        // Trigger on the nearest rising zero crossing before the window's
        // natural start, so the trace holds still (see the 1.4 notes).
        int start = ringHead_ - span;
        int searchSpan = std::min(span / 2, RING_CAP - span - 1);
        for (int k = 1; k <= searchSpan; k++) {
            if (ringAt(start - k - 1) <= 0.f && ringAt(start - k) > 0.f) {
                start -= k;
                break;
            }
        }
        float waveSnap[VIZ_WAVE_SAMPLES];
        float gain = cfg_.precision ? 1.0f : agcGain_;
        for (int w = 0; w < VIZ_WAVE_SAMPLES; w++) {
            int from = start + (int)((long long)w * span / VIZ_WAVE_SAMPLES);
            int to = start + (int)((long long)(w + 1) * span / VIZ_WAVE_SAMPLES);
            if (to <= from) to = from + 1;
            float peak = 0.f;
            for (int s = from; s < to; s++) {
                float v = ringAt(s);
                if (fabsf(v) > fabsf(peak)) peak = v;
            }
            waveSnap[w] = std::clamp(peak * gain * staleFade, -1.0f, 1.0f);
        }
        PublishWaveform(waveSnap);
        scopeFlatPublished_ = (staleFade <= 0.0f);
        return true;
    }

    // ---- Classic engine (1.4 / 1.5 numbers, unchanged) -----------------------------------
    void ClassicFrame(double dt, int got) {
        static constexpr float GRAVITY[VIZ_NUM_BANDS] = {0.018f, 0.020f, 0.022f, 0.025f,
                                                         0.030f, 0.036f, 0.042f};
        int wanted = (g_settings.fftSize == 1024 || g_settings.fftSize == 2048 || g_settings.fftSize == 4096 ||
                      g_settings.fftSize == 8192)
                         ? g_settings.fftSize
                         : 1024;
        if (wanted != currentFftSize_) {
            currentFftSize_ = wanted;
            BuildHannWindow(wanted);
            BuildTwiddleFactors(wanted);
            re_.assign(wanted, 0.f);
            im_.assign(wanted, 0.f);
            ringHead_ = ringCount_ = 0;
            for (int b = 0; b < VIZ_NUM_BANDS; b++) bandEnv_[b] = 0.f;
            PublishBands(bandEnv_);
            BuildLogBins(sampleRate_, wanted);
        }
        if (got == 0) {
            // 1.5 applied this once per capture wake, which with no audio was
            // every 20 ms. Wakes are per frame now, so the step is scaled to
            // the same 20 ms to keep the fall speed exactly as it was.
            float k = (float)(dt / 0.020);
            for (int b = 0; b < VIZ_NUM_BANDS; b++) bandEnv_[b] = std::max(0.f, bandEnv_[b] - GRAVITY[b] * k);
            PublishBands(bandEnv_);
            return;
        }

        static constexpr float AGC_TARGET = 0.85f, AGC_ATTACK = 0.35f, AGC_RELEASE = 0.012f,
                               AGC_GLIDE = 0.04f, AGC_FLOOR = 0.010f, AGC_IDLE_AUDIBLE = 0.030f;
        while (ringCount_ >= currentFftSize_) {
            int readStart = (ringHead_ - ringCount_ + RING_CAP) % RING_CAP;
            for (int i = 0; i < currentFftSize_; i++) {
                re_[i] = ring_[(readStart + i) % RING_CAP] * g_hannWindow[i];
                im_[i] = 0.f;
            }
            ringCount_ -= currentFftSize_ / 2;
            VizFFT(re_, im_);

            float t_sens = g_settings.sensitivity / 100.0f;
            float sliderGain = (t_sens <= 1.0f) ? 0.25f + t_sens * t_sens * 2.75f : 3.0f + (t_sens - 1.0f) * 4.0f;
            auto eq = GetVizEQMultipliers(g_settings.eq);
            static constexpr float BAND_SENSITIVITY[VIZ_NUM_BANDS] = {0.30f, 0.22f, 0.12f, 0.06f,
                                                                       0.030f, 0.018f, 0.010f};
            float rawBand[VIZ_NUM_BANDS];
            float blockPeak = 0.f;
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                int bStart = g_logBinStart[b];
                int bEnd = g_logBinStart[b + 1];
                if (bEnd <= bStart) bEnd = bStart + 1;
                float sumSq = 0.f;
                int count = 0;
                for (int k = bStart; k < bEnd; k++) {
                    sumSq += re_[k] * re_[k] + im_[k] * im_[k];
                    count++;
                }
                float rms = (count > 0) ? sqrtf(sumSq / (float)count) : 0.f;
                float eqM = (VIZ_BAND_EQ_ZONE[b] == 0) ? eq.low : (VIZ_BAND_EQ_ZONE[b] == 1) ? eq.mid : eq.high;
                float rawGained = (rms / (currentFftSize_ * 0.5f)) / BAND_SENSITIVITY[b] * sliderGain * eqM;
                rawBand[b] = std::max(0.f, rawGained);
                blockPeak = std::max(blockPeak, rawBand[b]);
            }
            bool blockAudible = (blockPeak >= AGC_FLOOR);
            bool blockIdleAudible = (blockPeak >= AGC_IDLE_AUDIBLE);
            if (g_settings.autoGain) {
                if (blockAudible) {
                    float k = (blockPeak > agcPeakEnv_) ? AGC_ATTACK : AGC_RELEASE;
                    agcPeakEnv_ += (blockPeak - agcPeakEnv_) * k;
                    float maxBoost = powf(10.f, g_settings.autoGainMaxDb / 20.f);
                    float want = (agcPeakEnv_ > 1e-6f) ? (AGC_TARGET / agcPeakEnv_) : maxBoost;
                    want = std::clamp(want, 1.f, maxBoost);
                    agcGain_ += (want - agcGain_) * AGC_GLIDE;
                }
            } else {
                agcPeakEnv_ = 0.f;
                agcGain_ = 1.f;
            }
            float maxMag = 0.f;
            for (int b = 0; b < VIZ_NUM_BANDS; b++) {
                float rawGained = rawBand[b] * agcGain_;
                float mag;
                switch (g_settings.sensitivityCurve) {
                    case VizSensitivityCurve::Exponential: mag = 1.f - expf(-rawGained); break;
                    case VizSensitivityCurve::Power: mag = std::min(1.f, powf(rawGained, 0.6f)); break;
                    case VizSensitivityCurve::Knee:
                    default: {
                        constexpr float knee = 0.7f;
                        mag = (rawGained <= knee) ? rawGained
                                                  : knee + (1.f - knee) * tanhf((rawGained - knee) / (1.f - knee));
                        break;
                    }
                }
                mag = std::max(0.f, std::min(1.f, mag));
                bandEnv_[b] = (mag >= bandEnv_[b]) ? mag : std::max(0.f, bandEnv_[b] - GRAVITY[b]);
                maxMag = std::max(maxMag, bandEnv_[b]);
            }
            PublishBands(bandEnv_);
            if (maxMag > 0.03f && (!g_settings.autoGain || blockIdleAudible))
                g_lastAudibleTickMs.store(GetTickCount64(), std::memory_order_relaxed);
            if (g_settings.beatFlashEnabled) {
                if (bandEnv_[0] - prevBassEnv_ > 0.12f) g_beatPulse.store(1.0f, std::memory_order_relaxed);
                prevBassEnv_ = bandEnv_[0];
            }
            if (g_settings.peakFreqEnabled) {
                int nyquistBin = currentFftSize_ / 2;
                int loBin = std::max(1, (int)(20.f * currentFftSize_ / (float)sampleRate_));
                int hiBin = std::min(nyquistBin - 1, (int)(20000.f * currentFftSize_ / (float)sampleRate_));
                int bestBin = -1;
                float bestMag = 0.f;
                for (int k = loBin; k <= hiBin; k++) {
                    float mag = re_[k] * re_[k] + im_[k] * im_[k];
                    if (mag > bestMag) {
                        bestMag = mag;
                        bestBin = k;
                    }
                }
                if (bestBin > 0 && sqrtf(bestMag) / (currentFftSize_ * 0.5f) > 0.02f)
                    g_dominantFreqHz.store((float)bestBin * (float)sampleRate_ / (float)currentFftSize_,
                                           std::memory_order_relaxed);
            }
        }
    }

    // ---- Precision engine --------------------------------------------------------------
    // Digital silence. Once every sample the analysis can see is an exact
    // zero (zeroRun_ >= silentNeeded_), one last forced analysis gives the
    // levels of pure zeros, and after that there is nothing for an FFT to
    // find: no FFTs run until a non-zero sample arrives. The bars keep their
    // ballistics until they are within 1e-4 of the floor (a tenth of a pixel
    // on a 1000 px bar), are then put exactly on it and published once, and
    // from there a frame does nothing at all and reports no change. The first
    // non-zero sample forces every tier to be analysed on the next frame, so
    // the bass tiers don't wait out a hop after the silence.
    // Returns whether anything the renderer reads was published.
    bool PrecisionFrame(double dt, int got) {
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        // Stream starved (nothing at all, not even silent packets, for more
        // than 40 ms): that is silence too, so feed its length in as zeros and
        // the spectrum decays on time instead of freezing on the last note.
        if (got == 0 && client_ && !stopped_) {
            double since = lastPacketQpc_ ? (double)(q.QuadPart - lastPacketQpc_) / (double)VizQpcFreq() : 1.0;
            if (since > 0.040) {
                int n = std::min((int)(dt * sampleRate_), cfg_.spec.fftSize);
                if (n > 0) {
                    if (zeroRun_ < silentNeeded_) {
                        zeros_.assign((size_t)n, 0.f);
                        spec_.Push(zeros_.data(), n);
                    }
                    zeroRun_ += n;
                }
            }
        }
        const bool silent = zeroRun_ >= silentNeeded_;
        const bool force = wakeFromSilence_;
        wakeFromSilence_ = false;
        if (!silent) silentFinal_ = settled_ = gpuSilentFlushed_ = false;

        if (cfg_.workload == VizWorkload::Gpu) {
            // The GPU does the analysis. Here: hand over fresh blocks (in
            // silence, the all-zero ones once and then nothing), and judge
            // silence from the samples themselves.
            if (!silent || !gpuSilentFlushed_) {
                bool take = force || silent;
                std::lock_guard<std::mutex> lock(g_gpuFeed.m);
                int n = g_gpuFeed.n;
                if (n > 0 && g_gpuFeed.blocks.size() >= (size_t)3 * n) {
                    for (int t = 0; t < 3; t++)
                        if (spec_.TakeTierBlock(t, &g_gpuFeed.blocks[(size_t)t * n], take)) g_gpuFeed.dirty |= 1u << t;
                    if (silent) gpuSilentFlushed_ = true;
                }
            }
            if (got > 0 && blockPeak_ > kVizAudibleLin)
                g_lastAudibleTickMs.store(GetTickCount64(), std::memory_order_relaxed);
            return true;  // the renderer runs the ballistics, so it always has work
        }
        if (silent && settled_) return false;

        bool fresh = false;
        if (!silentFinal_) {
            fresh = spec_.Analyze(force || silent, [](const float*, int, float*, float*) { return false; });
            if (silent) silentFinal_ = true;
            if (fresh) {
                VizPerf(kPerfAnalyses);
                VizPerf(kPerfFfts, (uint32_t)spec_.LastFftCount());
            }
        }

        const int nb = std::min(spec_.NumBands(), VIZ_BARS_MAX);
        const float* db = spec_.LevelsDb();
        const float range = std::max(1.f, cfg_.disp.ceilDb - cfg_.disp.floorDb);
        ttdsp::DisplayMap disp = cfg_.disp;
        disp.gainDb = cfg_.sensDb + agcDb_;
        // Attack / release coefficients once per frame, not once per band.
        const ttdsp::BallisticsCoef bc = ttdsp::BallisticsCoefs(dt, cfg_.ball, range);
        float maxX = 0.f, rawMax = -300.f, bass = 0.f, maxLevel = 0.f;
        VizBandFrame& out = frame_;
        out.count = nb;
        out.zone[0] = out.zone[1] = out.zone[2] = 0.f;
        out.zoneCount[0] = out.zoneCount[1] = out.zoneCount[2] = 0.f;
        for (int b = 0; b < nb; b++) {
            float x = disp.ToNorm(db[b]);
            bandLevel_[b] = ttdsp::BallisticsApply(bandLevel_[b], x, bc);
            out.level[b] = bandLevel_[b];
            out.zone[bandZone_[b]] += bandLevel_[b];
            out.zoneCount[bandZone_[b]] += 1.f;
            maxX = std::max(maxX, x);
            maxLevel = std::max(maxLevel, bandLevel_[b]);
            rawMax = std::max(rawMax, db[b]);
            if (bandBass_[b]) bass = std::max(bass, x);
        }
        if (silent && silentFinal_ && maxLevel < 1e-4f) {
            for (int b = 0; b < nb; b++) bandLevel_[b] = out.level[b] = 0.f;
            out.zone[0] = out.zone[1] = out.zone[2] = 0.f;
            bassSlow_ = 0.f;
            settled_ = true;
            PublishBandFrame(out);
            return true;
        }

        // Silence, judged on both the samples and the drawn level, so neither
        // a quiet hiss nor a display range set very low can hold the mod awake.
        bool audible = got > 0 && blockPeak_ > kVizAudibleLin && maxX > 0.02f;
        if (audible) g_lastAudibleTickMs.store(GetTickCount64(), std::memory_order_relaxed);

        // Auto Gain: lift the loudest band toward 85% of the range. Boost
        // only, held through silence, smoothed over about 0.4 s.
        if (cfg_.autoGain) {
            if (audible) {
                float target = cfg_.disp.floorDb + 0.85f * range;
                float loud = rawMax + cfg_.sensDb;
                float k = 1.f - expf(-(float)dt / ((loud > loudEnvDb_) ? 0.03f : 2.0f));
                loudEnvDb_ = (loudEnvDb_ < -150.f) ? loud : loudEnvDb_ + (loud - loudEnvDb_) * k;
                float want = std::clamp(target - loudEnvDb_, 0.f, cfg_.autoGainMaxDb);
                agcDb_ += (want - agcDb_) * (1.f - expf(-(float)dt / 0.4f));
            }
        } else {
            agcDb_ = 0.f;
        }

        // Beat: bass pulling ahead of its own 60 ms average. A difference of
        // two envelopes rather than "rose since the last block", so it fires
        // the same way at any frame rate.
        bassSlow_ += (bass - bassSlow_) * (1.f - expf(-(float)dt / 0.06f));
        if (cfg_.beat && bass - bassSlow_ > 0.12f) g_beatPulse.store(1.0f, std::memory_order_relaxed);

        if (cfg_.wantDominant && fresh) {
            double hz = spec_.DominantHz(-70.0);
            if (hz > 0.0) g_dominantFreqHz.store((float)hz, std::memory_order_relaxed);
        }
        PublishBandFrame(out);
        return true;
    }

    // Returns whether any reading changed since the last publish.
    bool PublishMeters() {
        // A new track starts a new integrated measurement.
        ULONGLONG tick = g_nowPlayingChangedTick.load(std::memory_order_relaxed);
        if (cfg_.loudnessResetOnTrack && tick != lastTrackTick_) {
            lastTrackTick_ = tick;
            loudness_.Reset();
        }
        VizMeterValues v;
        v.momentary = loudness_.Momentary();
        v.shortTerm = loudness_.ShortTerm();
        v.integrated = loudness_.Integrated();
        v.truePeak = loudness_.TruePeakDb();
        v.correlation = correlation_.Value();
        v.valid = true;
        bool changed = !(v.momentary == lastMeters_.momentary && v.shortTerm == lastMeters_.shortTerm &&
                         v.integrated == lastMeters_.integrated && v.truePeak == lastMeters_.truePeak &&
                         v.correlation == lastMeters_.correlation && lastMeters_.valid);
        lastMeters_ = v;
        if (!changed) return false;
        std::lock_guard<std::mutex> lock(g_meterMutex);
        g_meters = v;
        return true;
    }

    void PublishGonio() {
        {
            std::lock_guard<std::mutex> lock(g_gonioMutex);
            size_t n = gonioPending_.size() / 2;
            g_gonioXY.resize(n * 2);
            // (side, mid) = ((L - R) / 2, (L + R) / 2): mono runs straight up.
            for (size_t i = 0; i < n; i++) {
                float l = gonioPending_[2 * i], r = gonioPending_[2 * i + 1];
                g_gonioXY[2 * i] = 0.5f * (l - r);
                g_gonioXY[2 * i + 1] = 0.5f * (l + r);
            }
        }
        gonioPending_.clear();
        g_vizStereoRate.store(sampleRate_, std::memory_order_relaxed);
        g_gonioSerial.fetch_add(1, std::memory_order_release);
    }

    // WASAPI
    ComPtr<IMMDeviceEnumerator> enum_;
    VizEndpointNotificationClient* notify_ = nullptr;
    bool notifyRegistered_ = false;
    ComPtr<IMMDevice> device_;
    ComPtr<IAudioClient> client_;
    ComPtr<IAudioCaptureClient> capture_;
    ComPtr<IVizAudioMeter> meter_;
    HANDLE audioEvent_ = nullptr;
    UINT32 sampleRate_ = 48000, channels_ = 2;
    bool isFloat_ = true;
    DWORD channelMask_ = 0;
    bool stopped_ = false;
    ULONGLONG lastReinit_ = 0;
    std::wstring warnedSource_;  // the source last reported missing
    LONGLONG lastPacketQpc_ = 0;
    float blockPeak_ = 0.f;      // mono-mix peak since the last Frame(), after Input Gain
    float lastFramePeak_ = 0.f;  // the same, for the frame before
    int pendingGot_ = 0;         // frames drained (by Pump) since the last Frame()
    bool loopback_ = true;
    bool meterOk_ = false;
    ULONGLONG retryMs_ = kRetryMinMs;
    bool openFallback_ = false;  // the default output stands in for a source that wouldn't open
    ULONGLONG lastProbe_ = 0;

    // Shared by both engines: the classic mono ring also feeds the scope.
    std::vector<float> ring_;
    int ringHead_ = 0, ringCount_ = 0;
    ULONGLONG lastAudioTick_ = 0;
    bool scopeFlatPublished_ = false;

    // Classic
    std::vector<float> re_, im_;
    int currentFftSize_ = 0;
    float bandEnv_[VIZ_NUM_BANDS] = {};
    float agcPeakEnv_ = 0.f, agcGain_ = 1.f, prevBassEnv_ = 0.f;

    // Precision
    VizEngineConfig cfg_;
    int configuredGen_ = 0;
    ttdsp::SpectrumEngine spec_;
    float bandLevel_[VIZ_BARS_MAX] = {};
    unsigned char bandZone_[VIZ_BARS_MAX] = {};
    bool bandBass_[VIZ_BARS_MAX] = {};
    float agcDb_ = 0.f, loudEnvDb_ = -200.f, bassSlow_ = 0.f;
    VizBandFrame frame_;
    std::vector<float> mono_, zeros_;
    long long zeroRun_ = 0, silentNeeded_ = 1LL << 62;  // digital silence, see PrecisionFrame
    bool silentFinal_ = false, settled_ = false, gpuSilentFlushed_ = false, wakeFromSilence_ = false;

    // Meters
    std::vector<float> inter_, stereo_, gonioPending_;
    ttdsp::LoudnessMeter loudness_;
    ttdsp::Correlation correlation_;
    VizMeterValues lastMeters_;
    ULONGLONG lastTrackTick_ = 0;
};

// The capture "thread" is now a state of the engine thread. These keep their
// 1.5 names and meaning for the callers (pause on fullscreen, resume, unload):
// Start asks for the stream, Stop asks for it to be closed and waits until it
// is. Closing clears the bars (Close -> ResetAnalysis), on the engine thread:
// the bars have exactly one writer, because two publishers racing on the
// seqlock can leave its count odd, and every reader would then spin forever.
void StartVizCaptureThread() {
    g_captureWanted.store(true);
    if (g_engineWake) SetEvent(g_engineWake);
}

void StopVizCaptureThread() {
    g_captureWanted.store(false);
    if (g_engineWake) SetEvent(g_engineWake);
    if (g_engineClosed && g_audioOpen.load()) WaitForSingleObject(g_engineClosed, 1500);
}

int VizEffectiveBarCount();

void UpdateVisualizerTargets() {
    const int vizBars = VizEffectiveBarCount();

    float bands[VIZ_NUM_BANDS];
    ReadBands(bands);
    float masterPeak = 0.f;
    for (int i = 0; i < VIZ_NUM_BANDS; i++) {
        masterPeak = std::max(masterPeak, bands[i]);
    }

    auto eq = GetVizEQMultipliers(g_settings.eq);

    auto sampleBands = [&](float t) -> float {
        // warpT can return a hair over 1 (HzToBandPos gives VIZ_NUM_BANDS at the top edge).
        float pos = std::clamp(t, 0.f, 1.f) * (VIZ_NUM_BANDS - 1);
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
            case VizFreqScale::Bark:
            case VizFreqScale::Erb: {
                ttdsp::FreqScale s = (g_settings.freqScale == VizFreqScale::Bark) ? ttdsp::FreqScale::Bark
                                                                                   : ttdsp::FreqScale::Erb;
                double u0 = ttdsp::ScaleFwd(s, 20.0), u1 = ttdsp::ScaleFwd(s, 20000.0);
                float hz = (float)ttdsp::ScaleInv(s, u0 + pos * (u1 - u0));
                return HzToBandPos(hz) / (float)(VIZ_NUM_BANDS - 1);
            }
            default:  // Log -- matches the original band-index-linear mapping
                return pos;
        }
    };

    // Smooth Mode swaps the animation clock (see VizClockSeconds). With it
    // off, phaseOf(rate) is exactly the 1.4 expression t * rate.
    const bool smoothClock = g_smoothActive.load(std::memory_order_relaxed);
    const double clock = smoothClock ? VizClockSeconds() : 0.0;
    float t = smoothClock ? 0.f : (float)GetTickCount64() * 0.001f;
    auto phaseOf = [&](float rate) -> float {
        return smoothClock ? VizClockPhase(clock, (double)rate, 2.0 * 3.14159265358979323846)
                           : t * rate;
    };
    float center = (vizBars - 1) * 0.5f;

    for (int i = 0; i < vizBars; i++) {
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;

        switch (g_settings.shape) {
            case VizShape::Stereo:
            case VizShape::Terminal:
                target = sampleBands(warpT(freqT)) * eqForT(warpT(freqT));
                break;
            case VizShape::Mountain: {
                float dist = fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(warpT(dist)) * eqForT(warpT(dist));
                float taper = 1.6f - dist * 0.9f;
                target = std::max(0.f, std::min(1.f, (energy + masterPeak * (0.2f - dist * 0.12f)) * taper));
                break;
            }
            case VizShape::Mirror: {
                float mirT = 1.f - fabsf((float)i - center) / std::max(1.f, center);
                float energy = sampleBands(warpT(mirT)) * eqForT(warpT(mirT));
                target = std::max(0.f, std::min(1.f, (energy + masterPeak * (0.1f + mirT * 0.12f)) * 1.3f));
                break;
            }
            case VizShape::Wave: {
                float phase = (float)i * (2.f * VIZ_PI / (float)vizBars);
                float wave = 0.55f + 0.45f * sinf(phaseOf(3.5f) - phase);
                float energy = sampleBands(warpT(freqT)) * eqForT(warpT(freqT));
                target = std::max(0.f, std::min(1.f, energy * wave + masterPeak * 0.15f));
                break;
            }
            case VizShape::Breathe: {
                if (i == 0) {
                    float k = (masterPeak > g_vizBreatheEnv) ? 0.04f : 0.015f;
                    if (smoothClock) k = VizEaseForFrame(k);
                    g_vizBreatheEnv += (masterPeak - g_vizBreatheEnv) * k;
                }
                float rate = 0.55f + VIZ_SEEDS[i % VIZ_BARS_MAX] * 0.18f;
                float inhale = 0.5f + 0.5f * sinf(phaseOf(rate) + VIZ_SEEDS[i % VIZ_BARS_MAX] * 1.2f);
                target = std::max(0.f, std::min(1.f, inhale * (0.12f + g_vizBreatheEnv * 0.88f)));
                break;
            }
            case VizShape::Dots:
                target = sampleBands(warpT(freqT)) * eqForT(warpT(freqT));
                break;
            case VizShape::Radial:
                target = sampleBands(warpT(freqT)) * eqForT(warpT(freqT));
                break;
            case VizShape::Oscilloscope:
            case VizShape::Goniometer:
                target = 0.f;  // drawn directly from the raw waveform buffer, not per-bar targets
                break;
        }

        g_vizTarget[i] = std::max(0.f, std::min(1.f, target));
    }
}

// ---- Bar levels for the frame being drawn ------------------------------------------
//
// Both renderers draw from the same two arrays: g_vizPeak (each bar's height,
// 0..1) and g_vizPeakHold (its peak cap). This is the one place they are
// worked out, for either engine, so Direct2D and Direct3D 11 can't drift
// apart.
//
//   Classic:   UpdateVisualizerTargets interpolates the 7 bands, then each bar
//              eases toward its target with the per-shape attack / decay, and
//              the caps fall 0.012 per frame. Numbers exactly as in 1.5.
//   Precision: each bar already has its own band with real ballistics, so the
//              Shape mapping is applied directly and the caps hold, then fall
//              with gravity, on elapsed time.

ttdsp::PeakHold g_peakState[VIZ_BARS_MAX];
float g_frameDt = 1.0f / 60.0f;  // seconds since the previous drawn frame (UI thread)
VizBandFrame g_drawBands;        // the UI thread's copy of the latest precision bands

int VizEffectiveBarCount() {
    if (g_settings.engine == VizEngineKind::Precision && g_settings.bandLayout != VizBandLayout::Scale &&
        g_settings.layoutBandCount > 0)
        return std::clamp(g_settings.layoutBandCount, 1, VIZ_BARS_MAX);
    return std::max(1, std::min(g_settings.barCount, VIZ_BARS_MAX));
}

// Wave and Breathe modulation for bar i, from the high-resolution clock. Used
// by the CPU mapping below and uploaded per frame for the GPU one.
float VizShapeMod(int i, int n, double clock) {
    if (g_settings.shape == VizShape::Wave) {
        float phase = (float)i * (2.f * VIZ_PI / (float)std::max(1, n));
        return 0.55f + 0.45f * sinf(VizClockPhase(clock, 3.5, 2.0 * 3.14159265358979323846) - phase);
    }
    if (g_settings.shape == VizShape::Breathe) {
        float seed = VIZ_SEEDS[i % VIZ_BARS_MAX];
        double rate = 0.55 + seed * 0.18;
        return 0.5f + 0.5f * sinf(VizClockPhase(clock, rate, 2.0 * 3.14159265358979323846) + seed * 1.2f);
    }
    return 1.0f;
}

void UpdatePrecisionTargets(int vizBars) {
    ReadBandFrame(g_drawBands);
    const VizBandFrame& f = g_drawBands;
    const int nb = f.count;
    float master = 0.f;
    for (int b = 0; b < nb; b++) master = std::max(master, f.level[b]);

    // Breathe's slow envelope, on elapsed time: the 1.5 per-frame constants
    // 0.04 / 0.015 at 60 FPS are time constants of 0.41 s / 1.10 s.
    {
        float tau = (master > g_vizBreatheEnv) ? 0.408f : 1.103f;
        g_vizBreatheEnv += (master - g_vizBreatheEnv) * (1.f - expf(-g_frameDt / tau));
    }
    auto sample = [&](float t) -> float {
        if (nb <= 0) return 0.f;
        float pos = std::clamp(t, 0.f, 1.f) * (float)(nb - 1);
        int lo = (int)pos;
        int hi = std::min(lo + 1, nb - 1);
        float fr = pos - (float)lo;
        return f.level[lo] * (1.f - fr) + f.level[hi] * fr;
    };
    const double clock = VizClockSeconds();
    float center = (vizBars - 1) * 0.5f;
    for (int i = 0; i < vizBars; i++) {
        float freqT = (vizBars > 1) ? (float)i / (float)(vizBars - 1) : 0.5f;
        float target = 0.f;
        switch (g_settings.shape) {
            case VizShape::Mountain: {
                float dist = fabsf((float)i - center) / std::max(1.f, center);
                target = (sample(dist) + master * (0.2f - dist * 0.12f)) * (1.6f - dist * 0.9f);
                break;
            }
            case VizShape::Mirror: {
                float mirT = 1.f - fabsf((float)i - center) / std::max(1.f, center);
                target = (sample(mirT) + master * (0.1f + mirT * 0.12f)) * 1.3f;
                break;
            }
            case VizShape::Wave:
                target = sample(freqT) * VizShapeMod(i, vizBars, clock) + master * 0.15f;
                break;
            case VizShape::Breathe:
                target = VizShapeMod(i, vizBars, clock) * (0.12f + g_vizBreatheEnv * 0.88f);
                break;
            case VizShape::Oscilloscope:
            case VizShape::Goniometer:
                target = 0.f;
                break;
            default:
                target = sample(freqT);
                break;
        }
        g_vizTarget[i] = std::clamp(target, 0.f, 1.f);
    }
}

void VizStepAfterimage(int barCount);  // Afterimage (2.1), with the styles

void VizComputeBarFrame() {
    const int barCount = VizEffectiveBarCount();
    if (g_settings.engine == VizEngineKind::Precision) {
        UpdatePrecisionTargets(barCount);
        const bool gravity = g_settings.peakFall == VizPeakFall::Gravity;
        for (int i = 0; i < barCount; i++) {
            g_vizPeak[i] = g_vizTarget[i];
            if (g_settings.peakHoldEnabled) {
                // Gravity 2 bar-heights / s^2 drops a full-height cap in 1 s;
                // linear falls a full height in 1.4 s, close to 1.5's
                // 0.012 per 60 FPS frame.
                ttdsp::PeakHoldStep(g_peakState[i], g_vizPeak[i], g_frameDt, (double)g_settings.peakHoldMs,
                                    gravity, 2.0f, 0.72f);
                g_vizPeakHold[i] = g_peakState[i].level;
            } else {
                g_peakState[i] = ttdsp::PeakHold();
                g_vizPeakHold[i] = 0.f;
            }
        }
        VizStepAfterimage(barCount);
        return;
    }

    UpdateVisualizerTargets();
    float attack = 0.55f, decay = 0.18f;
    switch (g_settings.shape) {
        case VizShape::Stereo: attack = 0.72f; decay = 0.22f; break;
        case VizShape::Mirror: attack = 0.52f; decay = 0.20f; break;
        case VizShape::Wave: attack = 0.34f; decay = 0.17f; break;
        case VizShape::Breathe: attack = 0.20f; decay = 0.11f; break;
        default: break;
    }
    if (g_settings.smoothing > 0) {
        float smoothFactor = 1.0f - (g_settings.smoothing / 100.0f) * 0.9f;
        attack *= smoothFactor;
        decay *= smoothFactor;
    }
    attack = VizEaseForFrame(attack);
    decay = VizEaseForFrame(decay);
    for (int i = 0; i < barCount; i++) {
        float tgt = g_vizTarget[i], cur = g_vizPeak[i];
        float next = cur + (tgt - cur) * ((tgt > cur) ? attack : decay);
        g_vizPeak[i] = (fabsf(next - cur) > 0.0005f) ? next : tgt;
        float fac = std::max(0.f, g_vizPeak[i]);
        if (g_settings.peakHoldEnabled) {
            g_vizPeakHold[i] =
                (fac >= g_vizPeakHold[i]) ? fac : std::max(0.f, g_vizPeakHold[i] - 0.012f * g_frameScale);
        } else {
            g_vizPeakHold[i] = 0.f;
        }
    }
    VizStepAfterimage(barCount);
}

// Copies what the engine thread needs out of the settings, as one value, and
// tells it. UI thread, from LoadSettings.
bool VizStyleNeedsCpuBars();
// Set by the Direct3D 11 renderer when the GPU analysis passes can't start on
// this device, so the engine analyses on the CPU instead. UI thread only.
bool g_gpuAnalysisFailed = false;

void VizPublishEngineConfig() {
    VizEngineConfig c;
    c.precision = g_settings.engine == VizEngineKind::Precision;
    c.workload = g_settings.workload;
    // GPU analysis needs the Precision engine, the Direct3D 11 renderer, a
    // style whose bars are drawn from GPU levels, and passes that started;
    // otherwise it is Hybrid (LoadSettings or the renderer has said why). The
    // same test as the renderer's gpuWork, or the engine would publish nothing
    // and the bars would sit at idle height.
    if (c.workload == VizWorkload::Gpu &&
        (!c.precision || g_settings.renderer != VizRenderer::D3D11 || g_settings.shape == VizShape::Terminal ||
         VizStyleNeedsCpuBars() || g_gpuAnalysisFailed))
        c.workload = VizWorkload::Hybrid;
    auto& s = c.spec;
    s.fftSize = g_settings.fftSize;
    if (c.workload == VizWorkload::Gpu) s.fftSize = std::min(s.fftSize, 4096);  // GPU FFT limit
    s.maxTier = g_settings.bassDetail;
    s.window = (ttdsp::WindowKind)(int)g_settings.window;
    s.layout = (ttdsp::BandLayout)(int)g_settings.bandLayout;
    s.scale = (ttdsp::FreqScale)(int)g_settings.freqScale;
    s.octaveFraction = g_settings.octaveFraction;
    s.fmin = g_settings.minFreq;
    s.fmax = g_settings.maxFreq;
    s.a4 = g_settings.tuningA4;
    s.bars = std::max(1, std::min(g_settings.barCount, VIZ_BARS_MAX));
    s.weighting = (ttdsp::Weighting)(int)g_settings.weighting;
    s.tiltDbPerOct = g_settings.tiltDbPerOct;
    s.detector = (ttdsp::Detector)(int)g_settings.detector;
    s.levelRef = (ttdsp::LevelRef)(int)g_settings.levelRef;
    // EQ Preset as dB per zone, from the 1.4 multipliers.
    auto eq = GetVizEQMultipliers(g_settings.eq);
    s.zoneDb[0] = 20.0 * log10(std::max(0.01f, eq.low));
    s.zoneDb[1] = 20.0 * log10(std::max(0.01f, eq.mid));
    s.zoneDb[2] = 20.0 * log10(std::max(0.01f, eq.high));

    if (g_settings.ballistics == VizBallisticsPreset::Custom) {
        c.ball.attackMs = g_settings.attackMs;
        c.ball.release = ttdsp::ReleaseKind::Linear;
        c.ball.releaseDbPerSec = g_settings.releaseDbPerSec;
    } else {
        c.ball = ttdsp::BallisticsPreset((int)g_settings.ballistics);
    }
    // Motion Smoothing slows whichever ballistics are in use, up to 6x.
    double slow = 1.0 + g_settings.smoothing / 20.0;
    c.ball.attackMs *= slow;
    c.ball.releaseMs *= slow;
    c.ball.releaseDbPerSec /= slow;

    c.disp.floorDb = (float)g_settings.dbFloor;
    c.disp.ceilDb = (float)g_settings.dbCeiling;
    c.disp.curve = (ttdsp::Curve)(int)g_settings.sensitivityCurve;
    // Sensitivity keeps its 0-300 range and its default of 150: here it is a
    // gain of 0.2 dB per step around the default, -30 to +30 dB.
    c.sensDb = (g_settings.sensitivity - 150) * 0.2f;
    c.autoGain = g_settings.autoGain;
    c.autoGainMaxDb = g_settings.autoGainMaxDb;
    c.channel = g_settings.channel;
    bool readout = g_settings.peakFreqEnabled;
    c.wantDominant = readout && (g_settings.readout == VizReadout::Frequency || g_settings.readout == VizReadout::Both);
    c.wantLoudness = readout && g_settings.readout != VizReadout::Frequency;
    // True peak and correlation are only shown by the full loudness readout
    // (correlation also under the Goniometer), so only then are they measured.
    c.wantTruePeak = readout && g_settings.readout == VizReadout::LoudnessFull;
    c.wantGonio = g_settings.shape == VizShape::Goniometer || g_settings.style == VizStyle::Vu ||
                  g_settings.style == VizStyle::SplitLR;
    c.wantCorrelation = c.wantTruePeak || c.wantGonio;

    // Oscilloscope and Goniometer draw no bars, but the bands still feed the
    // colour zones (low < 300 Hz < mid < 2.5 kHz < high, used as ratios), the
    // beat (< 150 Hz) and, for the Frequency readout, the peak search. 24
    // bands cover all of that instead of up to 2048: on the user's own scale,
    // so each zone keeps its share of the bands and the colours mix as before
    // (IEC and musical layouts, which are logarithmic, become Log; 24 log
    // bands are 0.4 octave each, 7 of them under 150 Hz for the beat). The
    // bass tiers are dropped too unless the Frequency readout is on: without
    // them its resolution in the bass falls to one top-tier bin (fs / FFT
    // size, 23 Hz at 2048) and it can't report below about 2 bins (47 Hz),
    // which is why they stay when it's shown. GPU analysis keeps the full
    // layout (its band buffers follow the bar layout).
    if (c.precision && c.workload != VizWorkload::Gpu &&
        (g_settings.shape == VizShape::Oscilloscope || g_settings.shape == VizShape::Goniometer)) {
        if (s.layout != ttdsp::BandLayout::Scale) s.scale = ttdsp::FreqScale::Log;
        s.layout = ttdsp::BandLayout::Scale;
        s.bars = 24;
        if (!c.wantDominant) s.maxTier = 0;
    }
    c.beat = g_settings.beatFlashEnabled || g_settings.style == VizStyle::Particles;
    c.loudnessResetOnTrack = g_settings.loudnessResetOnTrack;
    {
        std::lock_guard<std::mutex> lock(g_engineCfgMutex);
        g_engineCfg = c;
    }
    g_engineCfgGen.fetch_add(1, std::memory_order_acq_rel);
    // Audio source: a different one reopens the stream on the engine thread.
    {
        std::lock_guard<std::mutex> lock(g_audioSourceMutex);
        std::wstring key = g_settings.audioSourceKey == L"default_output" ? L"" : g_settings.audioSourceKey;
        if (key != g_audioSourceKeyShared) {
            g_audioSourceKeyShared = key;
            g_deviceChanged.store(true, std::memory_order_relaxed);
        }
    }
    if (g_engineWake) SetEvent(g_engineWake);
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

// ---- Drawing Device -------------------------------------------------------------
//
// The 1.4 build created its device on "whatever D3D picks", which is adapter 0.
// On a desktop that is normally the GPU the monitor hangs off, but on a laptop
// with a hybrid GPU, or a desktop with the iGPU left enabled, it can be the
// other one -- and then every frame is rendered on one GPU and copied across
// to the one that scans it out. That copy is the classic source of uneven
// frame times on integrated graphics, so Auto now asks the question directly:
// which adapter owns the output this widget is on?

bool VizAdapterIsSoftware(const DXGI_ADAPTER_DESC1& d) {
    // The flag covers WARP; the IDs cover the Basic Render Driver on systems
    // where it doesn't set the flag.
    return (d.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0 ||
           (d.VendorId == 0x1414 && d.DeviceId == 0x8c);
}

bool VizAdapterIsIntegrated(const DXGI_ADAPTER_DESC1& d) {
    int fromDxcore = ttdxcore::IsIntegrated(d.AdapterLuid);
    if (fromDxcore >= 0) return fromDxcore == 1;
    // Pre-2004 Windows: integrated GPUs report little or no dedicated memory
    // (Intel 128 MB; AMD APUs a BIOS carve-out, usually 512 MB). An APU with a
    // bigger carve-out reads as discrete here, which only changes what Smooth
    // Mode's Auto does -- On still forces it.
    return d.DedicatedVideoMemory <= (SIZE_T)512 * 1024 * 1024;
}

bool VizLuidEqual(const LUID& a, const LUID& b) {
    return a.LowPart == b.LowPart && a.HighPart == b.HighPart;
}

// The hardware adapter with an output on `monitor`, or null.
ComPtr<IDXGIAdapter1> VizAdapterForMonitor(IDXGIFactory1* factory, HMONITOR monitor) {
    for (UINT i = 0;; i++) {
        ComPtr<IDXGIAdapter1> adapter;
        if (factory->EnumAdapters1(i, &adapter) == DXGI_ERROR_NOT_FOUND) break;
        if (!adapter) continue;
        DXGI_ADAPTER_DESC1 d;
        if (FAILED(adapter->GetDesc1(&d)) || VizAdapterIsSoftware(d)) continue;
        for (UINT j = 0;; j++) {
            ComPtr<IDXGIOutput> output;
            if (adapter->EnumOutputs(j, &output) == DXGI_ERROR_NOT_FOUND) break;
            if (!output) continue;
            DXGI_OUTPUT_DESC od;
            if (SUCCEEDED(output->GetDesc(&od)) && od.Monitor == monitor) return adapter;
        }
    }
    return nullptr;
}

// The first integrated (or discrete) hardware adapter, in the order Windows
// itself uses for its power-saving / high-performance GPU preference.
ComPtr<IDXGIAdapter1> VizAdapterOfKind(IDXGIFactory1* factory, bool wantIntegrated) {
    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(&factory6))) && factory6) {
        DXGI_GPU_PREFERENCE pref = wantIntegrated ? DXGI_GPU_PREFERENCE_MINIMUM_POWER
                                                  : DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
        for (UINT i = 0;; i++) {
            ComPtr<IDXGIAdapter1> adapter;
            if (FAILED(factory6->EnumAdapterByGpuPreference(i, pref, IID_PPV_ARGS(&adapter))) ||
                !adapter)
                break;
            DXGI_ADAPTER_DESC1 d;
            if (FAILED(adapter->GetDesc1(&d)) || VizAdapterIsSoftware(d)) continue;
            if (VizAdapterIsIntegrated(d) == wantIntegrated) return adapter;
        }
        return nullptr;
    }
    // Windows 10 before 1803: no preference ordering, so classify each one.
    for (UINT i = 0;; i++) {
        ComPtr<IDXGIAdapter1> adapter;
        if (factory->EnumAdapters1(i, &adapter) == DXGI_ERROR_NOT_FOUND) break;
        if (!adapter) continue;
        DXGI_ADAPTER_DESC1 d;
        if (FAILED(adapter->GetDesc1(&d)) || VizAdapterIsSoftware(d)) continue;
        if (VizAdapterIsIntegrated(d) == wantIntegrated) return adapter;
    }
    return nullptr;
}

struct VizAdapterChoice {
    ComPtr<IDXGIAdapter1> adapter;  // null with warp=false means "let D3D pick"
    bool warp = false;
    std::wstring warning;           // set when the request couldn't be honoured
};

HMONITOR VizTargetMonitor() {
    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    return monitor;
}

VizAdapterChoice VizPickRenderAdapter(IDXGIFactory1* factory, VizRenderDevice want,
                                      HMONITOR monitor) {
    VizAdapterChoice choice;
    if (want == VizRenderDevice::Cpu || g_settings.workload == VizWorkload::Cpu) {
        choice.warp = true;
        return choice;
    }
    if (want == VizRenderDevice::Integrated || want == VizRenderDevice::Discrete) {
        bool integrated = (want == VizRenderDevice::Integrated);
        choice.adapter = VizAdapterOfKind(factory, integrated);
        if (!choice.adapter) {
            choice.warning =
                integrated
                    ? L"No integrated GPU was found, so drawing is on Auto instead. On a desktop "
                      L"the iGPU is often switched off in the BIOS, and F / KF processors don't "
                      L"have one at all."
                    : L"No discrete GPU was found, so drawing is on Auto instead.";
        }
    }
    if (!choice.adapter) choice.adapter = VizAdapterForMonitor(factory, monitor);
    return choice;
}

void VizUpdateSmoothActive() {
    bool on = g_settings.smoothMode == VizSmoothMode::On ||
              (g_settings.smoothMode == VizSmoothMode::Auto && g_renderAdapter.valid &&
               (g_renderAdapter.integrated || g_renderAdapter.warp));
    bool was = g_smoothActive.exchange(on);
    if (was != on) Wh_Log(L"[Hardware] Smooth Mode %s", on ? L"on" : L"off");
}

bool InitDirectX() {
    HRESULT hr;

    // The factory comes first now: picking an adapter needs it.
    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&g_dxgiFactory));
    if (FAILED(hr)) return false;

    VizAdapterChoice choice =
        VizPickRenderAdapter(g_dxgiFactory.Get(), g_settings.renderDevice, VizTargetMonitor());

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    if (choice.warp) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags, nullptr, 0,
                               D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    } else if (choice.adapter) {
        // An explicit adapter requires DRIVER_TYPE_UNKNOWN.
        hr = D3D11CreateDevice(choice.adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags,
                               nullptr, 0, D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    } else {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0,
                               D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
    }
    if (FAILED(hr) && !choice.warp) {
        // Last resort rather than no visualizer at all: a GPU in the middle of
        // a driver update, or a remote session without one.
        Wh_Log(L"D3D11CreateDevice on hardware failed: 0x%08X, falling back to WARP", hr);
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags, nullptr, 0,
                               D3D11_SDK_VERSION, &g_d3dDevice, nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            if (!choice.warning.empty()) choice.warning += L"\r\n      ";
            choice.warning += L"The GPU wouldn't create a device, so drawing fell back to the CPU.";
        }
    }
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

    // Record what the device actually landed on.
    g_renderAdapter = VizRenderAdapterInfo();
    {
        ComPtr<IDXGIAdapter> used;
        ComPtr<IDXGIAdapter1> used1;
        DXGI_ADAPTER_DESC1 d;
        if (SUCCEEDED(g_dxgiDevice->GetAdapter(&used)) && used && SUCCEEDED(used.As(&used1)) &&
            SUCCEEDED(used1->GetDesc1(&d))) {
            g_renderAdapter.valid = true;
            g_renderAdapter.luid = d.AdapterLuid;
            g_renderAdapter.name = d.Description;
            g_renderAdapter.warp = VizAdapterIsSoftware(d);
            g_renderAdapter.integrated = !g_renderAdapter.warp && VizAdapterIsIntegrated(d);
        }
    }
    Wh_Log(L"[Hardware] drawing on %s (%s)",
           g_renderAdapter.name.empty() ? L"unknown adapter" : g_renderAdapter.name.c_str(),
           g_renderAdapter.warp         ? L"CPU / WARP"
           : g_renderAdapter.integrated ? L"integrated GPU"
                                        : L"discrete GPU");
    if (g_renderAdapter.valid && !g_renderAdapter.warp) {
        ComPtr<IDXGIAdapter1> owner = VizAdapterForMonitor(g_dxgiFactory.Get(), VizTargetMonitor());
        DXGI_ADAPTER_DESC1 od;
        if (owner && SUCCEEDED(owner->GetDesc1(&od)) &&
            !VizLuidEqual(od.AdapterLuid, g_renderAdapter.luid)) {
            Wh_Log(L"[Hardware] note: the monitor is driven by %s, so each frame is copied across "
                   L"GPUs. Drawing Device = Auto avoids that.",
                   od.Description);
        }
    }
    if (!choice.warning.empty()) {
        ReportSettingWarning(L"Hardware", L"Drawing Device", choice.warning);
        FlushSettingsIssues();
    }
    VizUpdateSmoothActive();

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&g_d2dFactory));
    if (FAILED(hr)) return false;

    hr = g_d2dFactory->CreateDevice(g_dxgiDevice.Get(), &g_d2dDevice);
    if (FAILED(hr)) return false;

    D2D1_STROKE_STYLE_PROPERTIES capProps = D2D1::StrokeStyleProperties(
        D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND);
    g_d2dFactory->CreateStrokeStyle(&capProps, nullptr, 0, &g_roundCapStrokeStyle);

    // Device-independent, so a device rebuild keeps it.
    if (!g_dwriteFactory) {
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                            (IUnknown**)g_dwriteFactory.GetAddressOf());
    }

    return true;
}

// Would the current settings put the device on a different adapter than the
// one it is on? Asked on settings and display changes; a fresh factory, so a
// GPU that was just plugged in, enabled or removed is seen.
bool VizRenderDeviceNeedsRebuild() {
    if (!g_d3dDevice) return false;
    ComPtr<IDXGIFactory2> factory;
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)))) return false;
    VizAdapterChoice choice =
        VizPickRenderAdapter(factory.Get(), g_settings.renderDevice, VizTargetMonitor());
    if (choice.warp != g_renderAdapter.warp) return true;
    if (choice.warp || !choice.adapter) return false;
    DXGI_ADAPTER_DESC1 d;
    if (FAILED(choice.adapter->GetDesc1(&d))) return false;
    return !g_renderAdapter.valid || !VizLuidEqual(d.AdapterLuid, g_renderAdapter.luid);
}

void ReleaseSwapChainResources();
bool CreateSwapChainResources();
void RenderVisualizer();
namespace ttgfx {
void ReleaseDevice();
void ReleaseSurface();
void SplitSync();
void OnSettingsChanged();
void PresentBlank();
}  // namespace ttgfx

// Tears the whole D3D/D2D stack down and builds it again on whatever adapter
// the settings now resolve to. UI thread only. Used for a Drawing Device
// change and for recovering from a lost device (driver update, TDR, a hybrid
// laptop switching GPUs), which the 1.4 build had no answer to -- the
// visualizer simply froze until the mod was restarted.
void VizRebuildRenderDevice(PCWSTR reason) {
    Wh_Log(L"[Hardware] rebuilding the render device: %s", reason);
    g_lastDeviceRebuildTick = GetTickCount64();

    ReleaseSwapChainResources();
    ttgfx::ReleaseDevice();
    g_roundCapStrokeStyle.Reset();
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
    g_dxgiFactory.Reset();

    if (!InitDirectX()) {
        Wh_Log(L"[Hardware] InitDirectX failed during rebuild");
        return;
    }
    if (g_overlayWnd && CreateSwapChainResources() && !g_fullscreenPaused.load()) {
        RenderVisualizer();
    }
}

// Called with the results of a frame's EndDraw and Present. Device loss shows
// up there first; the rebuild itself is posted rather than run inline, since
// this is called from inside the render path.
void VizCheckDeviceLost(HRESULT hrEndDraw, HRESULT hrPresent) {
    bool lost = hrEndDraw == D2DERR_RECREATE_TARGET || hrPresent == DXGI_ERROR_DEVICE_REMOVED ||
                hrPresent == DXGI_ERROR_DEVICE_RESET;
    if (!lost || !g_messageWnd) return;
    if (!g_deviceRebuildQueued.exchange(true)) {
        HRESULT reason = g_d3dDevice ? g_d3dDevice->GetDeviceRemovedReason() : S_OK;
        Wh_Log(L"[Hardware] device lost (EndDraw 0x%08X, Present 0x%08X, reason 0x%08X)", hrEndDraw,
               hrPresent, reason);
        PostMessage(g_messageWnd, WM_APP_REBUILD_DEVICE, 1, 0);
    }
}

void UninitDirectX() {
    ttgfx::ReleaseDevice();
    g_dwriteTextFormat.Reset();
    g_dwriteFactory.Reset();
    g_roundCapStrokeStyle.Reset();
    g_d2dDevice.Reset();
    g_d2dFactory.Reset();
    g_dxgiFactory.Reset();
    g_dxgiDevice.Reset();
    g_d3dDevice.Reset();
    g_dwriteTextFormatFontSize = -1.f;
    g_dwriteTextFormatFontName.clear();
}

bool RecreateVisualResources();

float VizReadoutWidthEstimate();

// Geometry of the visualizer, resolved once and used both to size the swap
// chain and to draw into it.
//
// The swap chain used to span the entire desktop even though the widget
// occupies a thin strip of it, which meant every frame cleared and presented
// millions of untouched pixels and held two full-screen buffers in VRAM. It is
// now sized to the widget's bounding box, with the composition visual offset to
// position it -- so drawing happens in local coordinates starting at the box.
// Whether the current overlay window was made for the taskbar (it has to be
// recreated when Place In changes).
bool g_overlayInTaskbar = false;

// The taskbar on a monitor (the main one or a secondary one), or null.
HWND VizTaskbarFor(HMONITOR monitor) {
    for (PCWSTR cls : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"})
        for (HWND tb = FindWindowEx(nullptr, nullptr, cls, nullptr); tb; tb = FindWindowEx(nullptr, tb, cls, nullptr))
            if (MonitorFromWindow(tb, MONITOR_DEFAULTTONULL) == monitor) return tb;
    return nullptr;
}

// The area the Position percentages span: the work area, or in taskbar mode
// the taskbar itself.
RECT VizPlaceArea(HMONITOR monitor, const MONITORINFO& mi) {
    RECT panel;
    if (g_overlayInPanel && g_overlayWnd && GetWindowRect(g_overlayWnd, &panel)) return panel;
    if (g_settings.hostTaskbar) {
        RECT r;
        HWND tb = VizTaskbarFor(monitor);
        if (tb && GetWindowRect(tb, &r) && r.right > r.left && r.bottom > r.top) return r;
    }
    return mi.rcWork;
}

// The monitor's DPI, times the panel's own zoom in Place In = Panel.
float VizHostDpiScale(HMONITOR monitor) {
    return GetMonitorDpiScale(monitor) * (g_settings.hostPanel ? g_panelZoom : 1.0f);
}

// Where layout coordinates count from: the virtual screen's top-left, or the
// panel window's own in Place In = Panel.
POINT VizLayoutOrigin() {
    RECT r;
    if (g_overlayInPanel && g_overlayWnd && GetWindowRect(g_overlayWnd, &r)) return POINT{r.left, r.top};
    return POINT{GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN)};
}

struct VizLayout {
    float originX, originY;         // virtual-screen coords of the box's top-left
    UINT  width, height;            // swap chain size, in pixels
    float blockX, blockY;           // bar-group origin, in LOCAL coords
    float totalWidth, totalHeight;  // bar-group extent
    float textSide;                 // horizontal room reserved for overlay text
    float textAnchorSide;           // where text hangs off the bars; offset-independent
    float textTop;                  // vertical room reserved above the bars
    float textBottom;               // vertical room reserved below the bars
};

// ---- Drag-to-move ----------------------------------------------------------
// Ctrl (or whichever modifier/button is configured) + a click-drag over the
// visualizer repositions it. Windhawk mods can only READ settings, not write
// them back, so a dragged position can't land in the Position % fields the
// settings UI shows -- instead it's kept as a runtime override here (which
// ComputeVizLayout consults ahead of the Position settings) and persisted via
// Wh_SetStringValue so it survives restarts silently. Settings are read-only
// to a mod, but mod-owned values are not, and Windhawk removes them with the
// mod rather than leaving anything behind.
std::atomic<bool> g_dragOverrideActive{false};
std::atomic<float> g_dragOverrideH{50.0f};
std::atomic<float> g_dragOverrideV{50.0f};
// True only while a drag is actively in progress -- tells the render path to
// freeze the bars and show just the background/border box moving.
std::atomic<bool> g_dragRenderPauseActive{false};

// Live keyboard nudges for the two text overlays, in physical pixels. Unlike
// the visualizer's position -- where the override replaces the Position
// percentages outright -- these are a delta *on top of* the Offset settings, so
// a value typed into settings is never silently ignored because a nudge exists.
// A delta of zero means "no nudge", which needs no separate active flag.
std::atomic<int> g_npNudgeX{0}, g_npNudgeY{0};
std::atomic<int> g_pfNudgeX{0}, g_pfNudgeY{0};

float EffectiveNowPlayingOffsetX() {
    return (float)g_settings.nowPlayingOffsetX * g_dpiScale +
           (float)g_npNudgeX.load(std::memory_order_relaxed);
}
float EffectiveNowPlayingOffsetY() {
    return (float)g_settings.nowPlayingOffsetY * g_dpiScale +
           (float)g_npNudgeY.load(std::memory_order_relaxed);
}
float EffectivePeakFreqOffsetX() {
    return (float)g_settings.peakFreqOffsetX * g_dpiScale +
           (float)g_pfNudgeX.load(std::memory_order_relaxed);
}
float EffectivePeakFreqOffsetY() {
    return (float)g_settings.peakFreqOffsetY * g_dpiScale +
           (float)g_pfNudgeY.load(std::memory_order_relaxed);
}

// Runtime placements go through Wh_SetStringValue / Wh_GetStringValue, which
// are per-mod, work on portable Windhawk, and are removed along with the mod.
//
// v1.2.0 also carried a one-shot import of a file the pre-catalog builds kept
// under %LOCALAPPDATA%. Nothing in the catalog ever wrote that file, so it has
// been removed here as agreed during the 1.2.0 review.
void LoadPositionOverride() {
    WCHAR buf[192] = {};
    Wh_GetStringValue(L"positionOverride", buf, ARRAYSIZE(buf));
    if (!buf[0]) return;

    // Two floats is the original format; the four trailing ints carrying the
    // text nudges came later, so a value written by an older build still parses
    // and simply leaves the nudges at zero. A negative percentage is the
    // sentinel for "no position override" -- needed now that the record can
    // exist purely to hold text nudges, and an old one never contains one.
    float hPct = -1.0f, vPct = -1.0f;
    int npX = 0, npY = 0, pfX = 0, pfY = 0;
    float mediaH = -1.0f, mediaV = -1.0f;
    int n = swscanf_s(buf, L"%f %f %d %d %d %d %f %f", &hPct, &vPct, &npX, &npY, &pfX, &pfY,
                      &mediaH, &mediaV);
    if (n >= 2 && hPct >= 0.0f && vPct >= 0.0f) {
        g_dragOverrideH.store(std::clamp(hPct, 0.0f, 100.0f), std::memory_order_relaxed);
        g_dragOverrideV.store(std::clamp(vPct, 0.0f, 100.0f), std::memory_order_relaxed);
        g_dragOverrideActive.store(true, std::memory_order_relaxed);
    }
    if (n >= 6) {
        g_npNudgeX.store(npX, std::memory_order_relaxed);
        g_npNudgeY.store(npY, std::memory_order_relaxed);
        g_pfNudgeX.store(pfX, std::memory_order_relaxed);
        g_pfNudgeY.store(pfY, std::memory_order_relaxed);
    }
    if (n >= 8 && mediaH >= 0.0f && mediaV >= 0.0f) {
        g_mediaOverrideH.store(std::clamp(mediaH, 0.0f, 100.0f), std::memory_order_relaxed);
        g_mediaOverrideV.store(std::clamp(mediaV, 0.0f, 100.0f), std::memory_order_relaxed);
        g_mediaOverrideActive.store(true, std::memory_order_relaxed);
    }
}

// One value holds all three runtime placements, so any change rewrites the
// whole record rather than touching a field -- and when everything is back to
// its default the value is cleared outright, so nothing lingers to be reloaded.
void PersistOverrideState() {
    bool posActive = g_dragOverrideActive.load(std::memory_order_relaxed);
    bool mediaActive = g_mediaOverrideActive.load(std::memory_order_relaxed);
    int npX = g_npNudgeX.load(std::memory_order_relaxed);
    int npY = g_npNudgeY.load(std::memory_order_relaxed);
    int pfX = g_pfNudgeX.load(std::memory_order_relaxed);
    int pfY = g_pfNudgeY.load(std::memory_order_relaxed);

    if (!posActive && !mediaActive && !npX && !npY && !pfX && !pfY) {
        Wh_SetStringValue(L"positionOverride", L"");
        return;
    }

    WCHAR buf[192];
    int len = swprintf_s(buf, L"%.4f %.4f %d %d %d %d %.4f %.4f",
                         posActive ? g_dragOverrideH.load(std::memory_order_relaxed) : -1.0f,
                         posActive ? g_dragOverrideV.load(std::memory_order_relaxed) : -1.0f,
                         npX, npY, pfX, pfY,
                         mediaActive ? g_mediaOverrideH.load(std::memory_order_relaxed) : -1.0f,
                         mediaActive ? g_mediaOverrideV.load(std::memory_order_relaxed) : -1.0f);
    if (len > 0) Wh_SetStringValue(L"positionOverride", buf);
}

void RequestPositionOverrideSave();  // defined with the keyboard-nudge helpers

// Both callers reach this from a low-level input hook: Ctrl+Alt+Home through
// ResetMoveTarget, and the double-click-in-place path in DragMouseHookProc.
// Everything in a WH_KEYBOARD_LL / WH_MOUSE_LL callback blocks all system input
// until it returns, so the write goes through the same deferred save the nudge
// paths already use rather than calling Wh_SetStringValue inline.
void ClearPositionOverride() {
    g_dragOverrideActive.store(false, std::memory_order_relaxed);
    RequestPositionOverrideSave();
}

UINT DragButtonDownMsg() {
    switch (g_settings.dragButton) {
        case VizDragButton::Left:  return WM_LBUTTONDOWN;
        case VizDragButton::Right: return WM_RBUTTONDOWN;
        default:                   return WM_MBUTTONDOWN;
    }
}

UINT DragButtonUpMsg() {
    switch (g_settings.dragButton) {
        case VizDragButton::Left:  return WM_LBUTTONUP;
        case VizDragButton::Right: return WM_RBUTTONUP;
        default:                   return WM_MBUTTONUP;
    }
}

bool DragModifierHeld() {
    switch (g_settings.dragModifier) {
        case VizDragModifier::None:  return true;
        case VizDragModifier::Ctrl:  return (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        case VizDragModifier::Alt:   return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        case VizDragModifier::Shift: return (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        case VizDragModifier::Win:   return (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
                                            (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    }
    return false;
}

// ---- Terminal shape -----------------------------------------------------------------
//
// The visualizer drawn as text: a grid of character cells in a monospace
// font, each cell one glyph in one of five colours. Three styles:
//
//   Columns    every bar a column of glyphs (Terminal Column Glyph), turning
//              the hot colour above Hot Threshold, with the peak cap as a
//              glyph of its own. The cava / btop look.
//   Waterfall  a spectrogram in characters: each row is one moment, each
//              column one band, the glyph picked from Terminal Glyph Ramp by
//              level, newest row on top, scrolling at Terminal Scroll Rate.
//   Meters     text meters, "Bass:    [||||||        48%]", for bass, mid,
//              treble and the loudest band.
//
// The grid is built here on the CPU from the same per-bar levels the other
// shapes draw (so every engine, layout and ballistics setting applies), then
// drawn by the Direct3D 11 renderer from a baked glyph atlas in one instanced
// call, or by Direct2D as text runs.

constexpr int VIZ_TERM_MAX_CELLS = 65536;

struct VizTermGrid {
    int cols = 0, rows = 0;
    std::vector<uint32_t> cells;  // char | colour << 8; colours: 0 dim, 1 low, 2 high, 3 label, 4 peak
};
VizTermGrid g_termGrid;
std::vector<float> g_termHistory;  // waterfall: rows x cols levels, row 0 newest
// Bumped by VizBuildTermGrid whenever the grid's size or any cell changes
// (Waterfall scrolls included), so the renderer can tell a changed grid from an
// unchanged one without hashing 65,536 cells every tick.
uint32_t g_termGridSerial = 0;
// Custom Terminal glyphs: code 128 + k draws g_termCustomGlyphs[k].
std::vector<std::wstring> g_termCustomGlyphs;

// The code for one character (a code point, plus a following emoji
// presentation selector), adding it to the custom glyphs when it isn't ASCII.
// 0 when there is no room left.
wchar_t VizTermCodeFor(const std::wstring& glyph) {
    if (glyph.size() == 1 && glyph[0] >= 32 && glyph[0] < 127) return glyph[0];
    for (size_t k = 0; k < g_termCustomGlyphs.size(); k++)
        if (g_termCustomGlyphs[k] == glyph) return (wchar_t)(128 + k);
    if (g_termCustomGlyphs.size() >= 128) return 0;
    g_termCustomGlyphs.push_back(glyph);
    return (wchar_t)(128 + g_termCustomGlyphs.size() - 1);
}

// Splits text into characters as VizTermCodeFor takes them.
std::vector<std::wstring> VizTermSplitGlyphs(const std::wstring& s) {
    std::vector<std::wstring> out;
    for (size_t i = 0; i < s.size();) {
        size_t n = (IS_HIGH_SURROGATE(s[i]) && i + 1 < s.size() && IS_LOW_SURROGATE(s[i + 1])) ? 2 : 1;
        if (i + n < s.size() && s[i + n] == 0xFE0F) n++;
        if (s[i] >= 32 || n > 1) out.push_back(s.substr(i, n));
        i += n;
    }
    return out;
}

std::wstring VizTermGlyphText(uint32_t code) {
    if (code >= 128 && code - 128 < g_termCustomGlyphs.size()) return g_termCustomGlyphs[code - 128];
    return std::wstring(1, (wchar_t)code);
}
float g_termScrollAcc = 0.f;

// Cell size for the terminal font, in whole pixels so glyphs land 1:1.
ComPtr<IDWriteTextFormat> g_termFormat;
std::wstring g_termFormatFont;
float g_termFormatPx = -1.f;
int g_termCellW = 8, g_termCellH = 16;

bool VizTermEnsureFormat() {
    float px = (float)std::clamp(g_settings.termFontSize, 6.f, 96.f) * g_dpiScale;
    if (g_termFormat && g_termFormatFont == g_settings.termFont && g_termFormatPx == px) return true;
    g_termFormat.Reset();
    if (!g_dwriteFactory) return false;
    if (FAILED(g_dwriteFactory->CreateTextFormat(g_settings.termFont.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                                                 DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, px, L"en-us",
                                                 &g_termFormat)))
        return false;
    g_termFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    g_termFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    g_termFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    g_termFormatFont = g_settings.termFont;
    g_termFormatPx = px;
    // Cell = the advance of "M" and the font's line height, rounded up. For a
    // proportional font every glyph still gets an M-wide cell, which reads as
    // monospaced; a real monospace font is the intended use.
    ComPtr<IDWriteTextLayout> lay;
    DWRITE_TEXT_METRICS m{};
    if (SUCCEEDED(g_dwriteFactory->CreateTextLayout(L"M", 1, g_termFormat.Get(), 1000.f, 1000.f, &lay)) && lay &&
        SUCCEEDED(lay->GetMetrics(&m))) {
        g_termCellW = std::max(1, (int)ceilf(m.widthIncludingTrailingWhitespace - 0.01f));
        g_termCellH = std::max(1, (int)ceilf(m.height - 0.01f));
    } else {
        g_termCellW = std::max(1, (int)ceilf(px * 0.6f));
        g_termCellH = std::max(1, (int)ceilf(px * 1.2f));
    }
    return true;
}

void VizTermGridSize(int* cols, int* rows) {
    if (g_settings.termStyle == VizTermStyle::Meters) {
        *cols = std::clamp(g_settings.termMeterColumns, 20, 200);
        *rows = 4;
    } else {
        *cols = VizEffectiveBarCount();
        *rows = std::clamp(g_settings.termRows, 2, 128);
    }
    while (*cols * *rows > VIZ_TERM_MAX_CELLS && *rows > 2) (*rows)--;
}

// The grid's size in pixels: what the layout reserves for this shape instead
// of a bar group. Bar Width, Gap and Max Size don't apply; the font does.
void VizTermBox(float* w, float* h) {
    int cols = 0, rows = 0;
    VizTermGridSize(&cols, &rows);
    VizTermEnsureFormat();
    *w = (float)(cols * g_termCellW);
    *h = (float)(rows * g_termCellH);
}

static inline uint32_t TermCell(wchar_t c, int color) {
    uint32_t ch = ((c >= 32 && c < 127) || (c >= 128 && c < 256)) ? (uint32_t)c : (uint32_t)'?';
    return ch | ((uint32_t)color << 8);
}

// Levels for the Meters style: bass / mid / treble as the average drawn level
// of each EQ zone, and "Volume" as the loudest band.
void VizTermMeterLevels(float out[4]) {
    if (g_settings.engine == VizEngineKind::Precision) {
        const VizBandFrame& f = g_drawBands;
        float mx = 0.f;
        for (int b = 0; b < f.count; b++) mx = std::max(mx, f.level[b]);
        for (int z = 0; z < 3; z++) out[z] = (f.zoneCount[z] > 0.f) ? f.zone[z] / f.zoneCount[z] : 0.f;
        out[3] = mx;
    } else {
        float b[VIZ_NUM_BANDS];
        ReadBands(b);
        float n[3] = {0, 0, 0};
        out[0] = out[1] = out[2] = out[3] = 0.f;
        for (int i = 0; i < VIZ_NUM_BANDS; i++) {
            out[VIZ_BAND_EQ_ZONE[i]] += b[i];
            n[VIZ_BAND_EQ_ZONE[i]] += 1.f;
            out[3] = std::max(out[3], b[i]);
        }
        for (int z = 0; z < 3; z++) out[z] = n[z] > 0.f ? out[z] / n[z] : 0.f;
    }
    for (int i = 0; i < 4; i++) out[i] = std::clamp(out[i], 0.f, 1.f);
}

// ---- waterfall scroll: begin
// Scrolls the waterfall history (rows x cols, row 0 newest) down by `steps`
// rows and writes the current levels (`newest`, clamped at 0) into row 0. The
// rows a multi-row step opens up between the new row 0 and the previous newest
// row are moments the frame skipped over: they are filled by interpolating
// between the two, so a long frame or a fast Scroll Rate leaves neither stale
// lines nor blank stripes. A step of the whole height or more leaves no older
// row to keep, so every row starts again from the current levels.
void VizTermScrollHistory(float* hist, int rows, int cols, int steps, const float* newest) {
    if (!hist || rows <= 0 || cols <= 0) return;
    const size_t rowLen = (size_t)cols;
    if (steps >= rows) {
        for (int c = 0; c < cols; c++) hist[c] = std::max(0.f, newest[c]);
        for (int r = 1; r < rows; r++) memcpy(hist + (size_t)r * rowLen, hist, sizeof(float) * rowLen);
        return;
    }
    if (steps > 0) {
        memmove(hist + (size_t)steps * rowLen, hist, sizeof(float) * (size_t)(rows - steps) * rowLen);
        const float* prev = hist + (size_t)steps * rowLen;  // the previous newest row
        for (int r = 1; r < steps; r++) {
            const float t = (float)r / (float)steps;
            float* row = hist + (size_t)r * rowLen;
            for (int c = 0; c < cols; c++) {
                float now = std::max(0.f, newest[c]);
                row[c] = now + (prev[c] - now) * t;
            }
        }
    }
    for (int c = 0; c < cols; c++) hist[c] = std::max(0.f, newest[c]);
}
// ---- waterfall scroll: end

void VizBuildTermGridCells();
void VizBuildTermGrid() {
    // The previous grid, kept to compare against: a straight memcmp of at most
    // 256 KB, several times cheaper than hashing it, and the copy is only
    // taken when something changed.
    static std::vector<uint32_t> s_prev;
    static int s_cols = -1, s_rows = -1;
    VizBuildTermGridCells();
    const VizTermGrid& g = g_termGrid;
    if (g.cols != s_cols || g.rows != s_rows || g.cells != s_prev) {
        g_termGridSerial++;
        s_cols = g.cols;
        s_rows = g.rows;
        s_prev = g.cells;
    }
}

void VizBuildTermGridCells() {
    VizTermGrid& g = g_termGrid;
    VizTermGridSize(&g.cols, &g.rows);
    g.cells.assign((size_t)g.cols * g.rows, TermCell(L' ', 0));
    const float hot = std::clamp(g_settings.termHotThreshold, 1.f, 100.f) / 100.0f;
    auto at = [&](int c, int r) -> uint32_t& { return g.cells[(size_t)r * g.cols + c]; };

    if (g_settings.termStyle == VizTermStyle::Meters) {
        static const wchar_t* kLabels[4] = {L"Bass:", L"Mid:", L"Treble:", L"Volume:"};
        float lv[4];
        VizTermMeterLevels(lv);
        const int labelW = 9, barN = std::max(1, g.cols - labelW - 6);
        for (int r = 0; r < 4; r++) {
            int c = 0;
            for (const wchar_t* p = kLabels[r]; *p && c < labelW; p++) at(c++, r) = TermCell(*p, 3);
            c = labelW;
            at(c++, r) = TermCell(L'[', 1);
            int filled = (int)lroundf(lv[r] * barN);
            for (int j = 0; j < barN; j++)
                at(c++, r) = (j < filled) ? TermCell(g_settings.termColumnGlyph, ((j + 1) > hot * barN) ? 2 : 1)
                                          : TermCell(L' ', 0);
            wchar_t pct[8];
            swprintf_s(pct, L"%3d%%", (int)lroundf(lv[r] * 100.f));
            for (const wchar_t* p = pct; *p && c < g.cols - 1; p++) at(c++, r) = TermCell(*p, 1);
            if (c < g.cols) at(c, r) = TermCell(L']', 1);
        }
        return;
    }

    if (g_settings.termStyle == VizTermStyle::Waterfall) {
        const std::wstring& ramp = g_settings.termRamp;
        const int nr = (int)ramp.size();
        size_t need = (size_t)g.cols * g.rows;
        if (g_termHistory.size() != need) {
            g_termHistory.assign(need, 0.f);
            g_termScrollAcc = 0.f;
        }
        // Scroll by whole rows at the configured rate; the newest row always
        // shows the current levels, so the top line stays live between steps.
        g_termScrollAcc += g_frameDt * std::clamp(g_settings.termScrollRate, 1.f, 120.f);
        int steps = std::min((int)g_termScrollAcc, g.rows);
        g_termScrollAcc -= (float)(int)g_termScrollAcc;
        VizTermScrollHistory(g_termHistory.data(), g.rows, g.cols, steps, g_vizPeak);
        for (int r = 0; r < g.rows; r++) {
            for (int c = 0; c < g.cols; c++) {
                float v = g_termHistory[(size_t)r * g.cols + c];
                if (nr <= 0 || v < 0.02f) continue;
                int idx = std::clamp((int)(v * (nr - 1) + 0.5f), 0, nr - 1);
                wchar_t ch = ramp[idx];
                if (ch == L' ') continue;
                at(c, r) = TermCell(ch, v > hot ? 2 : (idx == 0 ? 0 : 1));
            }
        }
        return;
    }

    // Columns.
    for (int c = 0; c < g.cols; c++) {
        float v = std::max(0.f, g_vizPeak[c]) * g.rows;
        int full = std::min(g.rows, (int)v);
        float frac = v - (float)full;
        for (int k = 0; k < full; k++) {
            int r = g.rows - 1 - k;
            at(c, r) = TermCell(g_settings.termColumnGlyph, ((float)(k + 1) / g.rows > hot) ? 2 : 1);
        }
        if (full < g.rows && frac >= 0.5f) {
            int r = g.rows - 1 - full;
            at(c, r) = TermCell(L'.', ((float)(full + 1) / g.rows > hot) ? 2 : 1);
        }
        if (g_settings.peakHoldEnabled) {
            int pr = (int)lroundf(g_vizPeakHold[c] * g.rows);
            if (pr > full && pr >= 1 && pr <= g.rows) at(c, g.rows - pr) = TermCell(g_settings.termPeakGlyph, 4);
        }
    }
}

// Terminal palette as straight-alpha floats, in cell colour order.
void VizTermPalette(float out[5][4]) {
    const BYTE* c[5][4] = {
        {&g_settings.termDimA, &g_settings.termDimR, &g_settings.termDimG, &g_settings.termDimB},
        {&g_settings.termLowA, &g_settings.termLowR, &g_settings.termLowG, &g_settings.termLowB},
        {&g_settings.termHighA, &g_settings.termHighR, &g_settings.termHighG, &g_settings.termHighB},
        {&g_settings.termLabelA, &g_settings.termLabelR, &g_settings.termLabelG, &g_settings.termLabelB},
        {&g_settings.peakHoldA, &g_settings.peakHoldR, &g_settings.peakHoldG, &g_settings.peakHoldB}};
    for (int i = 0; i < 5; i++) {
        out[i][0] = *c[i][1] / 255.f;
        out[i][1] = *c[i][2] / 255.f;
        out[i][2] = *c[i][3] / 255.f;
        out[i][3] = *c[i][0] / 255.f;
    }
}

// Direct2D fallback: each row as runs of same-coloured text.
void VizDrawTermGridD2D(float originX, float originY) {
    if (!VizTermEnsureFormat() || !g_barBrush) return;
    const VizTermGrid& g = g_termGrid;
    float pal[5][4];
    VizTermPalette(pal);
    D2D1_TEXT_ANTIALIAS_MODE prev = g_dc->GetTextAntialiasMode();
    g_dc->SetTextAntialiasMode(g_settings.textPixel ? D2D1_TEXT_ANTIALIAS_MODE_ALIASED
                                                    : D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    std::wstring run;
    for (int r = 0; r < g.rows; r++) {
        int c = 0;
        while (c < g.cols) {
            uint32_t cell = g.cells[(size_t)r * g.cols + c];
            if ((cell & 255u) <= 32u) {
                c++;
                continue;
            }
            int color = (int)((cell >> 8) & 7u);
            int start = c;
            run.clear();
            while (c < g.cols) {
                uint32_t k = g.cells[(size_t)r * g.cols + c];
                bool blank = (k & 255u) <= 32u;
                if (!blank && (int)((k >> 8) & 7u) != color) break;
                if (blank) run.push_back(L' ');
                else run += VizTermGlyphText(k & 255u);
                c++;
            }
            while (!run.empty() && run.back() == L' ') run.pop_back();
            int ci = std::min(color, 4);
            g_barBrush->SetColor(D2D1::ColorF(pal[ci][0], pal[ci][1], pal[ci][2], pal[ci][3]));
            float x = originX + start * g_termCellW, y = originY + r * g_termCellH;
            g_dc->DrawText(run.c_str(), (UINT32)run.size(), g_termFormat.Get(),
                           D2D1::RectF(x, y, x + (float)(run.size() + 1) * g_termCellW, y + g_termCellH),
                           g_barBrush.Get());
        }
    }
    g_dc->SetTextAntialiasMode(prev);
}

// Hit-tests against the bounds of the last drawn frame (already tracked for
// the occlusion check), so "over the visualizer" means the box as it's
// actually being shown right now, including mid-drag.
bool PointInVisualizerBounds(POINT pt) {
    if (!g_drawRectValid.load(std::memory_order_relaxed)) return false;
    LONG l = g_drawRectL.load(std::memory_order_relaxed);
    LONG t = g_drawRectT.load(std::memory_order_relaxed);
    LONG r = g_drawRectR.load(std::memory_order_relaxed);
    LONG b = g_drawRectB.load(std::memory_order_relaxed);
    return pt.x >= l && pt.x < r && pt.y >= t && pt.y < b;
}

// ---- Styles (2.1) ---------------------------------------------------------------------
// Eight new looks, picked from the Shape list like the shapes. Each one rides
// on an existing shape underneath (Bloom on Radial, the rest on Stereo), so
// the bar maths, layout, panel and settings all keep working unchanged, and
// a style only adds what it really needs:
//   LED Meter      segmented bars, green / amber / red, peak segment held
//   Line Spectrum  a smooth filled curve through the bar tops, glowing edge
//   Polar Bloom    the Radial bars joined into one filled shape
//   Spectrogram    a scrolling colour history of the bars, with a legend
//   VU Needles     two analog L / R meters with real VU ballistics
//   Stereo Field   the left channel above the centre line, the right below
//   Particles      the bars, plus sparks thrown off their tops on each beat
// Reflection (Appearance) mirrors the bar styles onto the floor beneath them.
//
// Spectrogram, Stereo Field and Particles need the bar levels on the CPU, so
// with them the analysis runs there (Hybrid), as with Terminal.

static const float kVizSpecRate = 60.f;  // Spectrogram rows per second
constexpr int kVizSpecRows = 256;        // rows kept (the texture's height)
constexpr int kVizSparkMax = 512;

bool VizParseShape(PCWSTR v, VizShape* shape, VizStyle* style) {
    struct Entry { const wchar_t* name; VizShape shape; VizStyle style; };
    static const Entry kEntries[] = {
        {L"stereo", VizShape::Stereo, VizStyle::None},         {L"mountain", VizShape::Mountain, VizStyle::None},
        {L"mirror", VizShape::Mirror, VizStyle::None},         {L"wave", VizShape::Wave, VizStyle::None},
        {L"breathe", VizShape::Breathe, VizStyle::None},       {L"dots", VizShape::Dots, VizStyle::None},
        {L"radial", VizShape::Radial, VizStyle::None},         {L"oscilloscope", VizShape::Oscilloscope, VizStyle::None},
        {L"goniometer", VizShape::Goniometer, VizStyle::None}, {L"terminal", VizShape::Terminal, VizStyle::None},
        {L"led", VizShape::Stereo, VizStyle::Led},             {L"line", VizShape::Stereo, VizStyle::Line},
        {L"bloom", VizShape::Radial, VizStyle::Bloom},         {L"spectrogram", VizShape::Stereo, VizStyle::Spectrogram},
        {L"vu", VizShape::Stereo, VizStyle::Vu},               {L"stereo_field", VizShape::Stereo, VizStyle::SplitLR},
        {L"particles", VizShape::Stereo, VizStyle::Particles},
    };
    for (const Entry& e : kEntries) {
        if (v && wcscmp(v, e.name) == 0) {
            *shape = e.shape;
            *style = e.style;
            return true;
        }
    }
    *shape = VizShape::Stereo;
    *style = VizStyle::None;
    return false;
}

// Position in the right-click menu's Shape list: the ten shapes, then the styles.
int VizShapeMenuIndex() {
    return g_settings.style != VizStyle::None ? 9 + (int)g_settings.style : (int)g_settings.shape;
}

bool VizStyleNeedsCpuBars() {
    VizStyle s = g_settings.style;
    return s == VizStyle::Spectrogram || s == VizStyle::SplitLR || s == VizStyle::Particles ||
           g_settings.afterimage > 0;  // its trail is worked out with the bars, on the CPU
}

// Afterimage (2.1): each bar's trail jumps up with it and falls back over
// Afterimage Length.
float g_vizGhost[VIZ_BARS_MAX] = {};

void VizStepAfterimage(int barCount) {
    if (g_settings.afterimage <= 0) return;
    float fall = g_frameDt / std::max(0.1f, g_settings.afterimageSeconds);
    for (int i = 0; i < barCount && i < VIZ_BARS_MAX; i++) {
        float lv = std::max(0.f, g_vizPeak[i]);
        g_vizGhost[i] = std::max(lv, g_vizGhost[i] - fall);
    }
}

// Reflection: horizontal bars standing on the bottom edge only, where there
// is a floor to reflect in.
bool VizReflectionActive() {
    if (g_settings.reflection <= 0) return false;
    if (g_settings.orientation != VizOrientation::Horizontal) return false;
    if (g_settings.verticalAnchor != VizAnchor::Bottom) return false;
    VizStyle st = g_settings.style;
    if (!(st == VizStyle::None || st == VizStyle::Led || st == VizStyle::Line || st == VizStyle::Particles)) return false;
    VizShape s = g_settings.shape;
    return s == VizShape::Stereo || s == VizShape::Mountain || s == VizShape::Mirror || s == VizShape::Wave ||
           s == VizShape::Breathe || s == VizShape::Dots;
}

float VizReflectionDepth(float maxSize) { return maxSize * std::clamp(g_settings.reflection, 0.f, 100.f) / 100.f; }

// ---- Scale numbers -----------------------------------------------------------------------
// The Spectrogram legend's quarter ticks and the VU faces carry numbers. They
// are Direct2D text: on the Direct3D renderer they go on the text surface,
// which only redraws when something on it changes, so a still scale costs
// nothing per frame.
namespace {
ComPtr<IDWriteTextFormat> s_scaleFmt;
float s_scaleFmtPx = 0.f;

IDWriteTextFormat* VizScaleFormat(float px) {
    if (!g_dwriteFactory) return nullptr;
    if (!s_scaleFmt || fabsf(s_scaleFmtPx - px) > 0.01f) {
        s_scaleFmt.Reset();
        if (FAILED(g_dwriteFactory->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                                     DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, px, L"",
                                                     &s_scaleFmt)))
            return nullptr;
        s_scaleFmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        s_scaleFmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        s_scaleFmtPx = px;
    }
    return s_scaleFmt.Get();
}
}  // namespace

float VizScaleLabelPx(float maxSize) { return std::clamp(maxSize * 0.07f, 8.f * g_dpiScale, 13.f * g_dpiScale); }

bool VizStyleHasScale() { return g_settings.style == VizStyle::Spectrogram || g_settings.style == VizStyle::Vu; }

// `thick` is the bars' span across the time axis (Spectrogram only).
void VizDrawStyleScale(float blockX, float blockY, float maxSize, float thick, bool horizontal) {
    if (!g_dc || !g_barBrush || !VizStyleHasScale()) return;
    ID2D1SolidColorBrush* b = g_barBrush.Get();
    WCHAR s[24];
    if (g_settings.style == VizStyle::Spectrogram) {
        float px = VizScaleLabelPx(maxSize);
        IDWriteTextFormat* fmt = VizScaleFormat(px);
        if (!fmt) return;
        fmt->SetTextAlignment(horizontal ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_CENTER);
        b->SetColor(D2D1::ColorF(0.92f, 0.92f, 0.92f, 0.85f));
        // Precision maps Display Floor..Ceiling straight onto the bar height,
        // so the ticks are dB; Classic has no fixed dB scale, so percent.
        bool db = g_settings.engine == VizEngineKind::Precision;
        float side = 3.f * g_dpiScale + 6.f * g_dpiScale;
        for (int k = 0; k <= 4; k++) {
            float q = k / 4.f;
            if (db)
                swprintf_s(s, k == 4 ? L"%d dB" : L"%d",
                           (int)lroundf(g_settings.dbFloor + q * (g_settings.dbCeiling - g_settings.dbFloor)));
            else
                swprintf_s(s, L"%d%%", k * 25);
            D2D1_RECT_F r;
            if (horizontal) {
                float y = std::clamp(blockY + maxSize - q * maxSize, blockY + px * 0.6f, blockY + maxSize - px * 0.6f);
                float x = blockX + thick + side + 3.f * g_dpiScale;
                r = D2D1::RectF(x, y - px, x + px * 4.f, y + px);
            } else {
                float x = std::clamp(blockX + q * maxSize, blockX + px * 1.2f, blockX + maxSize - px * 1.2f);
                float y = blockY + thick + side + 2.f * g_dpiScale;
                r = D2D1::RectF(x - px * 2.5f, y, x + px * 2.5f, y + px * 1.3f);
            }
            g_dc->DrawText(s, (UINT32)wcslen(s), fmt, r, b, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        return;
    }
    // VU: the classic face numbers, inside the tick arc, red above 0 VU.
    float mh = maxSize, mw = maxSize * 1.5f, gap = 8.f * g_dpiScale;
    float px = std::max(7.f * g_dpiScale, mh * 0.075f);
    IDWriteTextFormat* fmt = VizScaleFormat(px);
    if (!fmt) return;
    fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    const float marks[7] = {-20, -10, -7, -5, -3, 0, 3};
    for (int m = 0; m < 2; m++) {
        float ox = blockX + (horizontal ? m * (mw + gap) : 0.f), oy = blockY + (horizontal ? 0.f : m * (mh + gap));
        float pvx = ox + mw * 0.5f, pvy = oy + mh * 0.9f, R = mh * 0.68f * 0.74f;
        for (float dbm : marks) {
            float p = (powf(10.f, dbm / 20.f) - 0.1f) / (1.41254f - 0.1f);
            float an = (-48.f + 96.f * p) * VIZ_PI / 180.f;
            float x = pvx + sinf(an) * R, y = pvy - cosf(an) * R;
            swprintf_s(s, dbm > 0 ? L"+%d" : L"%d", (int)fabsf(dbm));
            b->SetColor(dbm > 0 ? D2D1::ColorF(1.f, 0.27f, 0.23f, 0.95f) : D2D1::ColorF(0.92f, 0.92f, 0.9f, 0.85f));
            g_dc->DrawText(s, (UINT32)wcslen(s), fmt, D2D1::RectF(x - px * 1.5f, y - px * 0.7f, x + px * 1.5f, y + px * 0.7f),
                           b, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
    }
}

// The styles' extra room, applied after the shapes have sized the box.
void VizStyleBox(float* w, float* h, float maxSize, bool horizontal) {
    switch (g_settings.style) {
        case VizStyle::Vu: {
            float mw = maxSize * 1.5f, gap = 8.f * g_dpiScale;
            *w = horizontal ? mw * 2.f + gap : mw;
            *h = horizontal ? maxSize : maxSize * 2.f + gap;
            break;
        }
        case VizStyle::Spectrogram: {
            float legend = 3.f * g_dpiScale + 6.f * g_dpiScale, px = VizScaleLabelPx(maxSize);
            if (horizontal) *w += legend + 3.f * g_dpiScale + px * 3.4f;  // plus the scale numbers
            else *h += legend + 2.f * g_dpiScale + px * 1.3f;
            break;
        }
        default: break;
    }
    if (VizReflectionActive()) *h += VizReflectionDepth(maxSize);
}

// ---- Per-frame data, on the render thread ----------------------------------------------
float g_vizSplitL[VIZ_BARS_MAX] = {}, g_vizSplitR[VIZ_BARS_MAX] = {};
float g_vizVu[4] = {};  // needle L, R (0..1 of the scale), peak LED L, R
std::vector<uint8_t> g_vizSpecRing;  // bars x kVizSpecRows, newest row at g_vizSpecHead
int g_vizSpecW = 0, g_vizSpecHead = 0;
uint32_t g_vizSpecSerial = 0;  // rows pushed so far
float g_vizSparkBuf[kVizSparkMax * 4] = {};
int g_vizSparkCount = 0;
uint32_t g_vizSparkSerial = 0;

namespace {
struct VizSpark { float x, y, vx, vy, life, band, r; };
std::vector<VizSpark> s_sparks;
double s_styleClock = 0.0, s_specAcc = 0.0;
uint32_t s_stereoSerial = 0;
float s_hist[2][2048] = {};  // newest stereo samples, L and R
int s_histPos = 0;
float s_vuIn[2] = {}, s_vuPos[2] = {-0.03f, -0.03f}, s_vuVel[2] = {};
double s_vuLastBlock = 0.0;
float s_lastPulse = 0.f;
uint32_t s_rng = 2463534242u;

float Rand01() {
    s_rng ^= s_rng << 13;
    s_rng ^= s_rng >> 17;
    s_rng ^= s_rng << 5;
    return (float)(s_rng & 0xFFFFFF) / 16777216.f;
}

// VU scale position (0 at -20 VU, 1 at +3 VU) of a linear level, with
// 0 VU = -18 dBFS. Real VU meters are close to linear in voltage, so the
// scale's marks crowd toward the top as they do here.
float VuPosOf(float rms) { return std::clamp((rms * 7.943f - 0.1f) / (1.41254f - 0.1f), -0.03f, 1.08f); }

// New stereo samples from the engine (the Goniometer feed): VU input levels
// and peak LEDs, and the history Stereo Field analyses.
void TakeStereo(double now) {
    uint32_t serial = g_gonioSerial.load(std::memory_order_acquire);
    if (serial == s_stereoSerial) {
        if (now - s_vuLastBlock > 0.25) s_vuIn[0] = s_vuIn[1] = 0.f;  // nothing new: the signal stopped
        return;
    }
    s_stereoSerial = serial;
    s_vuLastBlock = now;
    double ms[2] = {0, 0};
    float pk[2] = {0, 0};
    size_t n = 0;
    {
        std::lock_guard<std::mutex> lock(g_gonioMutex);
        n = g_gonioXY.size() / 2;
        for (size_t i = 0; i < n; i++) {
            float side = g_gonioXY[2 * i], mid = g_gonioXY[2 * i + 1];
            float l = mid + side, r = mid - side;
            ms[0] += (double)l * l;
            ms[1] += (double)r * r;
            pk[0] = std::max(pk[0], fabsf(l));
            pk[1] = std::max(pk[1], fabsf(r));
            s_hist[0][s_histPos] = l;
            s_hist[1][s_histPos] = r;
            s_histPos = (s_histPos + 1) & 2047;
        }
    }
    if (!n) return;
    for (int c = 0; c < 2; c++) {
        s_vuIn[c] = VuPosOf((float)sqrt(ms[c] / (double)n));
        if (pk[c] >= 0.708f) g_vizVu[2 + c] = 1.f;  // -3 dBFS sample peak
    }
}

// VU ballistics, IEC 60268-17: a second-order needle, 99 % of a step in
// 300 ms with 1.5 % overshoot (zeta 0.8, omega 13.1 rad/s).
void StepVu(float dt) {
    const float wn = 13.1f, z = 0.8f;
    int steps = std::max(1, (int)ceilf(dt / 0.002f));
    float h = dt / steps;
    for (int c = 0; c < 2; c++) {
        for (int s = 0; s < steps; s++) {
            float a = wn * wn * (s_vuIn[c] - s_vuPos[c]) - 2.f * z * wn * s_vuVel[c];
            s_vuVel[c] += a * h;
            s_vuPos[c] += s_vuVel[c] * h;
        }
        s_vuPos[c] = std::clamp(s_vuPos[c], -0.04f, 1.1f);
        g_vizVu[c] = s_vuPos[c];
        g_vizVu[2 + c] = std::max(0.f, g_vizVu[2 + c] - dt / 0.6f);
    }
}

// Stereo Field: a 2048-point spectrum per channel, log-spaced bars from
// 30 Hz, 72 dB of range. Only when new samples came in.
void StepSplit(int bars, float dt, bool fresh) {
    static ttdsp::RealFft fft;
    static std::vector<float> win, buf, re, im;
    static float target[2][VIZ_BARS_MAX] = {};
    const int N = 2048;
    if (fft.Size() != N) {
        fft.Init(N);
        win.resize(N);
        for (int i = 0; i < N; i++) win[i] = 0.5f - 0.5f * cosf(2.f * VIZ_PI * i / (N - 1));
        buf.resize(N);
        re.resize(N / 2 + 1);
        im.resize(N / 2 + 1);
    }
    if (fresh) {
        float sr = (float)std::max<uint32_t>(8000, g_vizStereoRate.load(std::memory_order_relaxed));
        float fmax = std::min(18000.f, sr * 0.45f), fmin = 30.f;
        const double norm = 1.0 / ((N / 4.0) * (N / 4.0));  // full-scale sine through Hann = 0 dB
        for (int c = 0; c < 2; c++) {
            for (int i = 0; i < N; i++) buf[i] = s_hist[c][(s_histPos + i) & 2047] * win[i];
            fft.Forward(buf.data(), re.data(), im.data());
            for (int b = 0; b < bars; b++) {
                float f0 = fmin * powf(fmax / fmin, (float)b / bars), f1 = fmin * powf(fmax / fmin, (float)(b + 1) / bars);
                int k0 = std::clamp((int)(f0 * N / sr), 1, N / 2), k1 = std::clamp((int)(f1 * N / sr), k0, N / 2);
                double p = 0;
                for (int k = k0; k <= k1; k++) p = std::max(p, (double)re[k] * re[k] + (double)im[k] * im[k]);
                float db = 10.f * log10f((float)std::max(p * norm, 1e-12));
                target[c][b] = std::clamp((db + 72.f) / 66.f, 0.f, 1.f);
            }
        }
    }
    float att = 1.f - expf(-dt / 0.012f);
    float fall = dt * 1.4f;
    for (int b = 0; b < bars; b++) {
        float* lv[2] = {&g_vizSplitL[b], &g_vizSplitR[b]};
        for (int c = 0; c < 2; c++) {
            float t = target[c][b];
            *lv[c] = t > *lv[c] ? *lv[c] + (t - *lv[c]) * att : std::max(t, *lv[c] - fall);
        }
    }
}

// Spectrogram: a row of the bars' levels every 1/60 s into the ring.
void StepSpectrogram(int bars, float dt) {
    if (bars != g_vizSpecW || (int)g_vizSpecRing.size() != bars * kVizSpecRows) {
        g_vizSpecW = bars;
        g_vizSpecRing.assign((size_t)bars * kVizSpecRows, 0);
        g_vizSpecHead = 0;
        g_vizSpecSerial += kVizSpecRows;  // a full re-upload
        s_specAcc = 0.0;
    }
    s_specAcc += dt * kVizSpecRate;
    int rows = std::min(8, (int)s_specAcc);
    s_specAcc -= rows;
    for (int r = 0; r < rows; r++) {
        g_vizSpecHead = (g_vizSpecHead + 1) % kVizSpecRows;
        uint8_t* row = &g_vizSpecRing[(size_t)g_vizSpecHead * bars];
        for (int i = 0; i < bars; i++) row[i] = (uint8_t)lroundf(std::clamp(g_vizPeak[i], 0.f, 1.f) * 255.f);
        g_vizSpecSerial++;
    }
}

// Particles: on each beat, sparks leave the tops of the louder bars, thrown
// no higher than the box, and fall back under gravity.
void StepSparks(int bars, float dt) {
    const bool horizontal = g_settings.orientation == VizOrientation::Horizontal;
    const bool top = g_settings.verticalAnchor == VizAnchor::Top;
    const float barW = std::max(1.f, VizPx((float)std::max(1.f, g_settings.barWidth)));
    const float barGap = VizPx((float)std::max(0.f, g_settings.barGap));
    const float maxSize = std::max(2.f, VizPx((float)std::max(2.f, g_settings.barMaxSize)));
    const float idle = VizPx((float)std::max(0.f, g_settings.barIdleSize));
    const float G = 900.f * g_dpiScale;
    // Growth direction of the bars, in block coordinates.
    const float gx = horizontal ? 0.f : (top ? -1.f : 1.f);
    const float gy = horizontal ? (top ? 1.f : -1.f) : 0.f;
    float pulse = g_beatPulse.load(std::memory_order_relaxed);
    if (pulse > s_lastPulse + 0.3f && bars > 0) {
        for (int k = 0; k < 18 && (int)s_sparks.size() < kVizSparkMax; k++) {
            int i = std::min(bars - 1, (int)(Rand01() * bars));
            for (int t = 0; t < 4 && g_vizPeak[i] < 0.25f; t++) i = std::min(bars - 1, (int)(Rand01() * bars));
            float len = idle + std::max(0.f, g_vizPeak[i]) * std::max(0.f, maxSize - idle);
            float along = i * (barW + barGap) + barW * 0.5f;
            float base = horizontal ? (top ? 0.f : maxSize) : (top ? maxSize : 0.f);
            float tip = base + (horizontal ? gy : gx) * len;
            float room = std::max(0.f, maxSize - len);
            float v = sqrtf(2.f * G * room) * (0.45f + 0.55f * Rand01());
            float side = (Rand01() - 0.5f) * 80.f * g_dpiScale;
            VizSpark s;
            s.x = horizontal ? along : tip;
            s.y = horizontal ? tip : along;
            s.vx = gx * v + (horizontal ? side : 0.f);
            s.vy = gy * v + (horizontal ? 0.f : side);
            s.life = 1.f;
            s.band = (float)i;
            s.r = std::min(15.f, (1.2f + 1.4f * Rand01()) * g_dpiScale);
            s_sparks.push_back(s);
        }
    }
    s_lastPulse = pulse;
    const float boxW = horizontal ? bars * (barW + barGap) - barGap : maxSize;
    const float boxH = horizontal ? maxSize : bars * (barW + barGap) - barGap;
    size_t out = 0;
    for (size_t k = 0; k < s_sparks.size(); k++) {
        VizSpark s = s_sparks[k];
        s.vx -= gx * G * dt;
        s.vy -= gy * G * dt;
        s.x += s.vx * dt;
        s.y += s.vy * dt;
        s.life -= dt / 1.1f;
        bool inside = s.x >= -s.r && s.y >= -s.r && s.x <= boxW + s.r && s.y <= boxH + s.r;
        if (s.life > 0.f && inside) s_sparks[out++] = s;
    }
    s_sparks.resize(out);
    g_vizSparkCount = (int)out;
    for (size_t k = 0; k < out; k++) {
        const VizSpark& s = s_sparks[k];
        g_vizSparkBuf[k * 4] = s.x;
        g_vizSparkBuf[k * 4 + 1] = s.y;
        g_vizSparkBuf[k * 4 + 2] = s.life;
        g_vizSparkBuf[k * 4 + 3] = s.band * 16.f + s.r;
    }
    if (out) g_vizSparkSerial++;
}
}  // namespace

// Called once per drawn frame, after VizComputeBarFrame.
void VizStylesFrame() {
    VizStyle st = g_settings.style;
    if (st == VizStyle::None || st == VizStyle::Led || st == VizStyle::Line || st == VizStyle::Bloom) return;
    double now = VizClockSeconds();
    float dt = s_styleClock > 0.0 ? (float)std::clamp(now - s_styleClock, 0.0, 0.1) : 1.f / 60.f;
    s_styleClock = now;
    int bars = VizEffectiveBarCount();
    if (st == VizStyle::Vu || st == VizStyle::SplitLR) {
        uint32_t before = s_stereoSerial;
        TakeStereo(now);
        if (st == VizStyle::Vu) StepVu(dt);
        else StepSplit(bars, dt, s_stereoSerial != before);
    } else if (st == VizStyle::Spectrogram) {
        StepSpectrogram(bars, dt);
    } else if (st == VizStyle::Particles) {
        StepSparks(bars, dt);
    }
}

// ---- Direct2D (the 1.5 path, and the fallback) ---------------------------------------------
// The same looks drawn with plain Direct2D calls. Fine for a fallback; the
// Direct3D 11 renderer is the one to use for these (and the only one that
// draws Reflection).
namespace {
RGBA StyleBarColor(int i, int n, float fac, RGBA c1, RGBA cGrad1, RGBA c2, float rainbowBase, bool radial) {
    RGBA col = c1;
    VizColorMode m = g_settings.colorMode;
    float t = n > 1 ? (float)i / (n - 1) : 0.f;
    if (m == VizColorMode::Gradient || m == VizColorMode::Tourne) col = LerpColor(cGrad1, c2, t);
    else if (m == VizColorMode::ReactiveGradient) col = LerpColor(cGrad1, c2, fac);
    else if (m == VizColorMode::DynamicAlbum && !radial) col = LerpColor(cGrad1, c2, std::min(1.f, t * 0.6f + fac * 0.4f));
    else if (m == VizColorMode::RainbowCycle)
        col = HSVtoRGB(fmodf(rainbowBase + (radial ? (float)i / std::max(1, n) : t) * 360.f, 360.f), 0.85f, 1.0f, c1.a);
    if (m == VizColorMode::Acrylic) col = {(BYTE)std::clamp((int)(180.f * fac), 0, 180), c1.r, c1.g, c1.b};
    return col;
}

void SetBrush(RGBA c, float alphaScale = 1.f) {
    g_barBrush->SetColor(D2D1::ColorF(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f * alphaScale));
}

RGBA SpecColorCpu(float v) {
    v = std::clamp(v, 0.f, 1.f);
    static const float stops[5][3] = {{0, 0, 0}, {0.25f, 0.02f, 0.45f}, {0.85f, 0.15f, 0.35f}, {1, 0.6f, 0.1f}, {1, 1, 0.85f}};
    int k = std::min(3, (int)(v * 4.f));
    float f = v * 4.f - k;
    auto ch = [&](int c) { return (BYTE)lroundf((stops[k][c] + (stops[k + 1][c] - stops[k][c]) * f) * 255.f); };
    return {(BYTE)lroundf(std::clamp(v * 2.5f, 0.f, 1.f) * 255.f), ch(0), ch(1), ch(2)};
}
}  // namespace

// Draws the current style and returns true, or returns false for the shape
// path to draw (Particles draw their sparks here, then let the bars draw).
bool VizDrawStyleD2D(float blockX, float blockY, float totalWidth, float totalHeight, int barCount, float barW,
                     float barGap, float maxSize, float idleSize, bool horizontal, RGBA c1, RGBA cGrad1, RGBA c2,
                     float rainbowBase) {
    const VizStyle st = g_settings.style;
    if (st == VizStyle::None || !g_dc || !g_barBrush) return false;
    const float range = std::max(0.f, maxSize - idleSize);
    const bool top = g_settings.verticalAnchor == VizAnchor::Top;
    auto lenOf = [&](float lev) { return idleSize + std::max(0.f, lev) * range; };
    // Rect of bar i between distances a and b from its base edge (SpanRect in the shader).
    auto span = [&](int i, float a, float b) {
        float lead = i * (barW + barGap);
        if (horizontal) {
            float x = blockX + lead;
            if (top) return D2D1::RectF(x, blockY + a, x + barW, blockY + b);
            float base = blockY + maxSize;
            return D2D1::RectF(x, base - b, x + barW, base - a);
        }
        float y = blockY + lead;
        if (top) return D2D1::RectF(blockX + maxSize - b, y, blockX + maxSize - a, y + barW);
        return D2D1::RectF(blockX + a, y, blockX + b, y + barW);
    };

    if (st == VizStyle::Particles) {
        for (int k = 0; k < g_vizSparkCount; k++) {
            const float* p = &g_vizSparkBuf[k * 4];
            int band = std::min(std::max(barCount - 1, 0), (int)(p[3] / 16.f));
            float r = p[3] - band * 16.f;
            SetBrush(StyleBarColor(band, barCount, 1.f, c1, cGrad1, c2, rainbowBase, false), p[2]);
            g_dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(blockX + p[0], blockY + p[1]), r, r), g_barBrush.Get());
        }
        return false;
    }

    if (st == VizStyle::Led) {
        float segH = std::max(2.f * g_dpiScale, roundf(barW * 0.5f));
        float step = segH + std::max(1.f, roundf(1.5f * g_dpiScale));
        int segs = std::max(1, (int)((maxSize + step - segH) / step));
        for (int i = 0; i < barCount; i++) {
            float lit = lenOf(g_vizPeak[i]) / step;
            float hold = g_settings.peakHoldEnabled ? lenOf(g_vizPeakHold[i]) : 0.f;
            int holdSeg = hold > 0.5f ? std::min(segs - 1, (int)(hold / step)) : -1;
            for (int s = 0; s < segs; s++) {
                float t = (s + 0.5f) / segs;
                RGBA col = g_settings.colorMode == VizColorMode::Solid
                               ? (t >= 0.85f ? RGBA{255, 255, 59, 48} : t >= 0.6f ? RGBA{255, 255, 176, 0} : RGBA{255, 56, 227, 107})
                               : StyleBarColor(i, barCount, g_vizPeak[i], c1, cGrad1, c2, rainbowBase, false);
                bool on = s + 0.5f <= lit || s == holdSeg;
                SetBrush(col, on ? 1.f : 0.1f);
                g_dc->FillRectangle(span(i, s * step, s * step + segH), g_barBrush.Get());
            }
        }
        return true;
    }

    if (st == VizStyle::SplitLR) {
        for (int i = 0; i < barCount; i++) {
            float lead = i * (barW + barGap);
            for (int c = 0; c < 2; c++) {
                float lev = c ? g_vizSplitR[i] : g_vizSplitL[i];
                float s = lenOf(lev) * 0.5f;
                if (s < 0.25f) continue;
                SetBrush(StyleBarColor(i, barCount, lev, c1, cGrad1, c2, rainbowBase, false));
                D2D1_RECT_F r;
                if (horizontal) {
                    float x = blockX + lead, cy = blockY + maxSize * 0.5f;
                    r = c ? D2D1::RectF(x, cy + 0.5f, x + barW, cy + 0.5f + s) : D2D1::RectF(x, cy - 0.5f - s, x + barW, cy - 0.5f);
                } else {
                    float y = blockY + lead, cx = blockX + maxSize * 0.5f;
                    r = c ? D2D1::RectF(cx + 0.5f, y, cx + 0.5f + s, y + barW) : D2D1::RectF(cx - 0.5f - s, y, cx - 0.5f, y + barW);
                }
                g_dc->FillRectangle(r, g_barBrush.Get());
            }
        }
        return true;
    }

    if (st == VizStyle::Line || st == VizStyle::Bloom) {
        ComPtr<ID2D1PathGeometry> geo;
        ComPtr<ID2D1GeometrySink> sink;
        if (!g_d2dFactory || barCount < 3 || FAILED(g_d2dFactory->CreatePathGeometry(&geo)) || FAILED(geo->Open(&sink)))
            return true;
        RGBA col = StyleBarColor(barCount / 2, barCount, 0.5f, c1, cGrad1, c2, rainbowBase, st == VizStyle::Bloom);
        if (st == VizStyle::Line) {
            float sg = horizontal ? (top ? 1.f : -1.f) : (top ? -1.f : 1.f);
            float base = horizontal ? (top ? blockY : blockY + maxSize) : (top ? blockX + maxSize : blockX);
            float start = horizontal ? blockX : blockY;
            auto val = [&](int i) { return lenOf(g_vizPeak[std::clamp(i, 0, barCount - 1)]); };
            auto pt = [&](float along, float v) {
                float c = base + sg * std::clamp(v, 0.f, maxSize);
                return horizontal ? D2D1::Point2F(along, c) : D2D1::Point2F(c, along);
            };
            sink->BeginFigure(pt(start, 0.f), D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(pt(start, val(0)));
            const int sub = 4;
            for (int k = 0; k + 1 < barCount; k++) {
                float p0 = val(k - 1), p1 = val(k), p2 = val(k + 1), p3 = val(k + 2);
                for (int j = 1; j <= sub; j++) {
                    float t = (float)j / sub, t2 = t * t, t3 = t2 * t;
                    float v = 0.5f * (2 * p1 + (p2 - p0) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 + (3 * p1 - p0 - 3 * p2 + p3) * t3);
                    sink->AddLine(pt(start + (k + t) * (barW + barGap) + barW * 0.5f, v));
                }
            }
            float end = start + barCount * (barW + barGap) - barGap;
            sink->AddLine(pt(end, val(barCount - 1)));
            sink->AddLine(pt(end, 0.f));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        } else {
            float cx = blockX + totalWidth * 0.5f, cy = blockY + totalHeight * 0.5f, innerR = maxSize * 0.15f;
            for (int i = 0; i < barCount; i++) {
                float a = (float)i / barCount * 2.f * VIZ_PI - VIZ_PI * 0.5f, r = innerR + lenOf(g_vizPeak[i]);
                D2D1_POINT_2F p = D2D1::Point2F(cx + cosf(a) * r, cy + sinf(a) * r);
                if (i == 0) sink->BeginFigure(p, D2D1_FIGURE_BEGIN_FILLED);
                else sink->AddLine(p);
            }
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        }
        sink->Close();
        SetBrush(col, st == VizStyle::Line ? 0.35f : 0.55f);
        g_dc->FillGeometry(geo.Get(), g_barBrush.Get());
        SetBrush(col);
        g_dc->DrawGeometry(geo.Get(), g_barBrush.Get(), std::max(1.5f, barW * 0.2f));
        return true;
    }

    if (st == VizStyle::Spectrogram) {
        static ComPtr<ID2D1Bitmap> bmp;
        static ID2D1DeviceContext* bmpDc = nullptr;
        static uint32_t bmpSerial = 0;
        static std::vector<uint32_t> px;
        int w = g_vizSpecW, rows = kVizSpecRows;
        if (w <= 0) return true;
        float thick = barCount * (barW + barGap) - barGap;
        if (!bmp || bmpDc != g_dc.Get() || bmp->GetPixelSize().width != (UINT32)w) {
            bmp.Reset();
            bmpDc = g_dc.Get();
            D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            if (FAILED(g_dc->CreateBitmap(D2D1::SizeU(w, rows), nullptr, 0, bp, &bmp))) return true;
            bmpSerial = g_vizSpecSerial - 1;
        }
        if (bmpSerial != g_vizSpecSerial) {  // newest row first
            bmpSerial = g_vizSpecSerial;
            px.resize((size_t)w * rows);
            for (int a = 0; a < rows; a++) {
                const uint8_t* src = &g_vizSpecRing[(size_t)((g_vizSpecHead - a + rows) % rows) * w];
                for (int i = 0; i < w; i++) {
                    RGBA c = g_settings.colorMode == VizColorMode::Solid ? SpecColorCpu(src[i] / 255.f)
                                                                         : LerpColor(cGrad1, c2, src[i] / 255.f);
                    if (g_settings.colorMode != VizColorMode::Solid) c.a = (BYTE)std::min(255, src[i] * 5 / 2);
                    float al = c.a / 255.f * c1.a / 255.f;
                    px[(size_t)a * w + i] = ((uint32_t)lroundf(al * 255.f) << 24) | ((uint32_t)lroundf(c.r * al) << 16) |
                                            ((uint32_t)lroundf(c.g * al) << 8) | (uint32_t)lroundf(c.b * al);
                }
            }
            bmp->CopyFromMemory(nullptr, px.data(), w * 4);
        }
        float vis = std::min((float)rows, maxSize);
        D2D1_RECT_F dst = horizontal ? D2D1::RectF(blockX, blockY, blockX + thick, blockY + maxSize)
                                     : D2D1::RectF(blockX, blockY, blockX + maxSize, blockY + thick);
        if (horizontal) {
            g_dc->DrawBitmap(bmp.Get(), dst, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, D2D1::RectF(0, 0, (float)w, vis));
        } else {  // time runs left to right: rotate the image a quarter turn
            D2D1_MATRIX_3X2_F old;
            g_dc->GetTransform(&old);
            D2D1_POINT_2F o = D2D1::Point2F(blockX, blockY);
            g_dc->SetTransform(D2D1::Matrix3x2F(0, 1, 1, 0, 0, 0) * D2D1::Matrix3x2F::Translation(o.x, o.y) * old);
            g_dc->DrawBitmap(bmp.Get(), D2D1::RectF(0, 0, thick, maxSize), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
                             D2D1::RectF(0, 0, (float)w, vis));
            g_dc->SetTransform(old);
        }
        // Legend, hot end at the top (right when vertical), quarter ticks.
        float gap = 3.f * g_dpiScale, lw = 6.f * g_dpiScale;
        for (int k = 0; k < 32; k++) {
            float v = (k + 0.5f) / 32.f;
            RGBA c = g_settings.colorMode == VizColorMode::Solid ? SpecColorCpu(v) : LerpColor(cGrad1, c2, v);
            c.a = std::max<BYTE>(c.a, 89);  // the legend stays readable at the quiet end
            SetBrush(c);
            float a0 = k / 32.f, a1 = (k + 1) / 32.f;
            D2D1_RECT_F r = horizontal ? D2D1::RectF(dst.right + gap, dst.bottom - a1 * maxSize, dst.right + gap + lw, dst.bottom - a0 * maxSize)
                                       : D2D1::RectF(dst.left + a0 * maxSize, dst.bottom + gap, dst.left + a1 * maxSize, dst.bottom + gap + lw);
            g_dc->FillRectangle(r, g_barBrush.Get());
        }
        VizDrawStyleScale(blockX, blockY, maxSize, thick, horizontal);
        return true;
    }

    if (st == VizStyle::Vu) {
        float mh = maxSize, mw = maxSize * 1.5f, gap = 8.f * g_dpiScale;
        const float marks[11] = {-20, -10, -7, -5, -3, -2, -1, 0, 1, 2, 3};
        auto posOf = [](float db) { return (powf(10.f, db / 20.f) - 0.1f) / (1.41254f - 0.1f); };
        ID2D1SolidColorBrush* b = g_barBrush.Get();
        for (int m = 0; m < 2; m++) {
            float ox = blockX + (horizontal ? m * (mw + gap) : 0.f), oy = blockY + (horizontal ? 0.f : m * (mh + gap));
            float pvx = ox + mw * 0.5f, pvy = oy + mh * 0.9f, R = mh * 0.68f, lw = std::max(1.2f, mh * 0.016f);
            auto at = [&](float p, float r) {
                float an = (-48.f + 96.f * p) * VIZ_PI / 180.f;
                return D2D1::Point2F(pvx + sinf(an) * r, pvy - cosf(an) * r);
            };
            b->SetColor(D2D1::ColorF(0.05f, 0.05f, 0.06f, 0.6f));
            g_dc->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(ox, oy, ox + mw, oy + mh), mh * 0.08f, mh * 0.08f), b);
            for (int k = 0; k < 11; k++) {
                float p = posOf(marks[k]);
                b->SetColor(marks[k] > 0 ? D2D1::ColorF(1.f, 0.27f, 0.23f, 0.95f) : D2D1::ColorF(0.92f, 0.92f, 0.9f, 0.85f));
                g_dc->DrawLine(at(p, (k == 0 || k == 7) ? R * 0.84f : R * 0.9f), at(p, R), b, lw);
            }
            for (int k = 0; k < 16; k++) {
                bool hot = (k + 0.5f) / 16.f > posOf(0.f);
                b->SetColor(hot ? D2D1::ColorF(1.f, 0.27f, 0.23f, 0.95f) : D2D1::ColorF(0.92f, 0.92f, 0.9f, 0.85f));
                g_dc->DrawLine(at(k / 16.f, R * 0.9f), at((k + 1) / 16.f, R * 0.9f), b, hot ? lw * 1.8f : lw, g_roundCapStrokeStyle.Get());
            }
            SetBrush(RGBA{255, c1.r, c1.g, c1.b});
            g_dc->DrawLine(at(g_vizVu[m], R * 0.12f), at(g_vizVu[m], R * 1.02f), b, std::max(1.6f, mh * 0.024f),
                           g_roundCapStrokeStyle.Get());
            b->SetColor(D2D1::ColorF(0.25f, 0.25f, 0.27f, 1.f));
            g_dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pvx, pvy), mh * 0.05f, mh * 0.05f), b);
            b->SetColor(D2D1::ColorF(1.f, 0.18f, 0.12f, 0.18f + 0.82f * std::clamp(g_vizVu[2 + m], 0.f, 1.f)));
            g_dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ox + mw - mh * 0.12f, oy + mh * 0.12f), mh * 0.045f, mh * 0.045f), b);
        }
        VizDrawStyleScale(blockX, blockY, maxSize, 0.f, horizontal);
        return true;
    }
    return false;
}

// ---- Reflection and Shadow on the Direct2D renderer -------------------------------------
// The bars go into an offscreen bitmap the size of the target, which is then
// drawn up to three times: a Shadow effect of it, offset; the bitmap as is;
// and mirrored about the base line through a layer whose opacity fades from
// 40 % to nothing over the reflection depth, as on Direct3D 11. Skipped for
// the frames where the whole scene fades in or out, since the target can't
// change under a pushed layer.
namespace {
ComPtr<ID2D1Bitmap1> s_reflBmp;
ComPtr<ID2D1Image> s_reflOld;
ComPtr<ID2D1LinearGradientBrush> s_reflFade;
ComPtr<ID2D1Effect> s_shadowFx;
ID2D1DeviceContext* s_reflDc = nullptr;
float s_reflDpi = 96.f;
const CLSID kVizClsidShadow = {0xC67EA361, 0x1863, 0x4E69, {0x89, 0xDB, 0x69, 0x5D, 0x3E, 0x9A, 0x5B, 0x6B}};
}  // namespace

bool VizFxD2DBegin(bool fadeLayer) {
    if (fadeLayer || !g_dc || !(VizReflectionActive() || g_settings.fxShadow > 0)) return false;
    ComPtr<ID2D1Image> old;
    g_dc->GetTarget(&old);
    ComPtr<ID2D1Bitmap1> tb;
    if (!old || FAILED(old.As(&tb))) return false;
    D2D1_SIZE_U sz = tb->GetPixelSize();
    if (!s_reflBmp || s_reflDc != g_dc.Get() || s_reflBmp->GetPixelSize().width != sz.width ||
        s_reflBmp->GetPixelSize().height != sz.height) {
        s_reflBmp.Reset();
        s_reflFade.Reset();
        s_shadowFx.Reset();
        float dx = 96.f, dy = 96.f;
        tb->GetDpi(&dx, &dy);
        s_reflDpi = dx > 0.f ? dx : 96.f;
        D2D1_BITMAP_PROPERTIES1 bp = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), dx, dy);
        if (FAILED(g_dc->CreateBitmap(sz, nullptr, 0, bp, &s_reflBmp))) return false;
        D2D1_GRADIENT_STOP stops[2] = {{0.f, D2D1::ColorF(0, 0, 0, 0.4f)}, {1.f, D2D1::ColorF(0, 0, 0, 0.f)}};
        ComPtr<ID2D1GradientStopCollection> sc;
        if (FAILED(g_dc->CreateGradientStopCollection(stops, 2, &sc)) ||
            FAILED(g_dc->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, 1)),
                                                   sc.Get(), &s_reflFade))) {
            s_reflBmp.Reset();
            return false;
        }
        if (SUCCEEDED(g_dc->CreateEffect(kVizClsidShadow, &s_shadowFx))) s_shadowFx->SetInput(0, s_reflBmp.Get());
        s_reflDc = g_dc.Get();
    }
    s_reflOld = old;
    g_dc->SetTarget(s_reflBmp.Get());
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
    return true;
}

// `baseY` is the bars' base line in the drawing's own coordinates.
void VizFxD2DEnd(float baseY, float depth) {
    g_dc->SetTarget(s_reflOld.Get());
    s_reflOld.Reset();
    D2D1_MATRIX_3X2_F old;
    g_dc->GetTransform(&old);
    g_dc->SetTransform(D2D1::IdentityMatrix());  // the bitmap is already in target space
    if (g_settings.fxShadow > 0 && s_shadowFx) {
        const float toDip = 96.f / s_reflDpi * g_dpiScale;  // settings px -> target DIPs
        s_shadowFx->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, std::max(0.01f, g_settings.fxShadowSoft * 0.5f * toDip));
        s_shadowFx->SetValue(D2D1_SHADOW_PROP_COLOR,
                             D2D1::Vector4F(g_settings.fxShadowR / 255.f, g_settings.fxShadowG / 255.f,
                                            g_settings.fxShadowB / 255.f, g_settings.fxShadow / 100.f));
        g_dc->DrawImage(s_shadowFx.Get(), D2D1::Point2F(g_settings.fxShadowX * toDip, g_settings.fxShadowY * toDip));
    }
    g_dc->DrawImage(s_reflBmp.Get());
    float base = old._22 * baseY + old._32, d = depth * fabsf(old._22);
    if (VizReflectionActive() && d >= 1.f) {
        D2D1_SIZE_F ts = g_dc->GetSize();
        s_reflFade->SetStartPoint(D2D1::Point2F(0, base));
        s_reflFade->SetEndPoint(D2D1::Point2F(0, base + d));
        g_dc->PushLayer(D2D1::LayerParameters1(D2D1::RectF(0, base, ts.width, base + d), nullptr,
                                               D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), 1.f,
                                               s_reflFade.Get()),
                        nullptr);
        g_dc->SetTransform(D2D1::Matrix3x2F::Scale(1.f, -1.f, D2D1::Point2F(0, base)));
        g_dc->DrawImage(s_reflBmp.Get());
        g_dc->SetTransform(D2D1::IdentityMatrix());
        g_dc->PopLayer();
    }
    g_dc->SetTransform(old);
}

// ---- Album colours ease in -------------------------------------------------------------
// A new cover's colours fade in over 0.6 s instead of jumping (smoothstep).
// Render thread only. It moves only while frames are drawing, which they are
// while a track plays.
DWORD VizAlbumColorShown(int which) {
    static DWORD from[2], to[2], shown[2];
    static ULONGLONG t0[2];
    static bool init[2];
    DWORD target = (which ? g_albumArtColorSecondary : g_albumArtColor).load(std::memory_order_relaxed);
    ULONGLONG now = GetTickCount64();
    if (!init[which]) {
        init[which] = true;
        from[which] = to[which] = shown[which] = target;
    }
    if (target != to[which]) {
        from[which] = shown[which];
        to[which] = target;
        t0[which] = now;
    }
    float t = std::clamp((float)(now - t0[which]) / 600.f, 0.f, 1.f);
    t = t * t * (3.f - 2.f * t);
    DWORD out = 0;
    for (int sh = 0; sh < 32; sh += 8) {
        float a = (float)((from[which] >> sh) & 0xFF), b = (float)((to[which] >> sh) & 0xFF);
        out |= (DWORD)lroundf(a + (b - a) * t) << sh;
    }
    shown[which] = out;
    return out;
}

bool ComputeVizLayout(VizLayout* out) {
    if (!out) return false;

    int barCount  = VizEffectiveBarCount();
    float barW    = std::max(1.f, VizPx((float)std::max(1.f, g_settings.barWidth)));
    float barGap  = VizPx((float)std::max(0.f, g_settings.barGap));
    float maxSize = std::max(2.f, VizPx((float)std::max(2.f, g_settings.barMaxSize)));

    bool horizontal = (g_settings.orientation == VizOrientation::Horizontal);
    float barsThickness = barCount * barW + (barCount - 1) * barGap;

    float totalWidth, totalHeight;
    if (g_settings.shape == VizShape::Terminal) {
        VizTermBox(&totalWidth, &totalHeight);
    } else if (g_settings.shape == VizShape::Radial || g_settings.shape == VizShape::Goniometer) {
        totalWidth = totalHeight = maxSize * 2.0f;
    } else {
        totalWidth  = horizontal ? barsThickness : maxSize;
        totalHeight = horizontal ? maxSize       : barsThickness;
    }
    VizStyleBox(&totalWidth, &totalHeight, maxSize, horizontal);

    HMONITOR monitor = g_cachedMonitor;
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return false;

    const POINT layoutOrigin = VizLayoutOrigin();
    int vsx = layoutOrigin.x;
    int vsy = layoutOrigin.y;

    const RECT area = VizPlaceArea(monitor, mi);
    float waLeft   = (float)(area.left   - vsx);
    float waTop    = (float)(area.top    - vsy);
    float workWidth  = (float)(area.right  - area.left);
    float workHeight = (float)(area.bottom - area.top);

    bool dragOverride = g_dragOverrideActive.load(std::memory_order_relaxed) && !g_overlayInPanel;
    float hPercent = dragOverride ? g_dragOverrideH.load(std::memory_order_relaxed)
                                   : g_settings.horizontalPosition;
    float vPercent = dragOverride ? g_dragOverrideV.load(std::memory_order_relaxed)
                                   : g_settings.verticalPosition;

    float blockX = waLeft + (workWidth  - totalWidth)  * (hPercent / 100.0f);
    float blockY = waTop  + (workHeight - totalHeight) * (vPercent / 100.0f);
    if (!g_overlayInPanel && !g_settings.dockApp.empty() && g_dockActive.load(std::memory_order_relaxed)) {
        // Docked: beside the app's window, kept inside the work area like any
        // other position.
        float dx, dy;
        VizDockPlace(totalWidth, totalHeight, &dx, &dy);
        blockX = std::clamp(dx - vsx, waLeft, std::max(waLeft, waLeft + workWidth - totalWidth));
        blockY = std::clamp(dy - vsy, waTop, std::max(waTop, waTop + workHeight - totalHeight));
    }
    // Pixel Snap: the block on a whole pixel. With the sizes above already
    // whole, every bar edge and the panel then sit exactly on the grid.
    if (g_settings.pixelSnap) {
        blockX = roundf(blockX);
        blockY = roundf(blockY);
    }

    float padL = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingL) : 0.f;
    float padR = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingR) : 0.f;
    float padT = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingT) : 0.f;
    float padB = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingB) : 0.f;

    // Safety margin so nothing gets clipped at the edges of the smaller target:
    // stroked shapes (oscilloscope, radial) centre their line on the path and so
    // bleed half a stroke outward, and the radial shape's spokes reach further
    // than its nominal box (inner radius plus full bar length).
    float margin = std::max(4.0f * g_dpiScale, barW);
    if (g_settings.shape == VizShape::Radial) margin += maxSize * 0.15f;

    // Text overlays can sit outside the bars, so the box reserves room for them.
    // Without this they fall outside the render surface and get clipped away.
    float fontPx = (float)std::max(6.f, g_settings.nowPlayingFontSize) * g_dpiScale;
    float textTop = 0.f, textBottom = 0.f, textSide = 0.f;

    // textAnchorSide is where the draw code hangs the text off the bar group and
    // must not move when an offset changes -- otherwise nudging the Now Playing
    // text sideways would drag the frequency readout along with it, since both
    // anchor off the same number. textSide is the room actually reserved on the
    // render surface, which does have to grow by the offsets or offset text
    // simply walks off the surface and disappears.
    float textAnchorSide = 0.f;
    float extraSide = 0.f;

    // Reserved room is rounded up to a coarse step rather than tracking the
    // offset exactly. The swap chain is sized from this, and a keyboard nudge
    // is one pixel per press with key repeat behind it -- following every pixel
    // would mean a DXGI buffer resize dozens of times a second while someone is
    // simply holding an arrow key down.
    auto reserveFor = [](float offset) { return std::ceil(offset / 32.0f) * 32.0f; };

    // A text panel grows past the text by its padding and border, so that has to
    // be reserved too or the panel gets clipped at the edge of the surface while
    // the text inside it stays visible.
    float npPanel = (float)(g_settings.npBgPadding + g_settings.npBgBorderSize) * g_dpiScale;
    float pfPanel = (float)(g_settings.pfBgPadding + g_settings.pfBgBorderSize) * g_dpiScale;

    if (g_settings.nowPlayingEnabled) {
        float npOffX = reserveFor(std::abs(EffectiveNowPlayingOffsetX()) + npPanel);
        float npOffY = reserveFor(std::abs(EffectiveNowPlayingOffsetY()) + npPanel);
        if (g_settings.npPlacement == VizNpPlacement::Above) {
            float lines = (g_settings.npLayout == VizNpLayout::TwoLines) ? 2.7f : 1.6f;
            textTop = std::max(textTop, fontPx * lines + 8.0f * g_dpiScale + npOffY);
            textAnchorSide = std::max(textAnchorSide, 100.0f * g_dpiScale);
        } else {
            // Inside the panel, in the padding band (VizDrawTextOverlays): Panel
            // Bottom starts at the bars' bottom edge and Panel Top at the panel's
            // top edge, both npHeight tall. Padding smaller than the text (or
            // the background off, padding 0) lets it hang past the panel's
            // bottom: reserve that overflow, plus the offset room as before.
            float npHeight = fontPx * ((g_settings.npLayout == VizNpLayout::TwoLines) ? 2.7f : 1.6f);
            float overflow = (g_settings.npPlacement == VizNpPlacement::PanelBottom)
                                 ? npHeight - padB
                                 : npHeight - padT - totalHeight - padB;
            textTop = std::max(textTop, npOffY);
            textBottom = std::max(textBottom, std::max(0.f, overflow) + npOffY);
        }
        textBottom = std::max(textBottom, npOffY);
        extraSide  = std::max(extraSide, npOffX);
    }
    if (g_settings.progressEnabled) {
        float need = (float)(std::max(1.f, g_settings.progressHeight) + g_settings.progressGap) * g_dpiScale +
                     2.0f * g_dpiScale;
        if (g_settings.progressPlacement == VizProgressPlacement::Above) textTop += need;
        else if (g_settings.progressPlacement == VizProgressPlacement::Below) textBottom = std::max(textBottom, need);
        // Panel Bottom: drawn at the bars' bottom edge + gap (VizProgressRect),
        // so whatever of gap + height the bottom padding doesn't cover.
        else textBottom = std::max(textBottom, std::max(0.f, need - padB));
    }
    if (g_settings.peakFreqEnabled) {
        float pfOffX = reserveFor(std::abs(EffectivePeakFreqOffsetX()) + pfPanel);
        float pfOffY = reserveFor(std::abs(EffectivePeakFreqOffsetY()) + pfPanel);
        float pfH = fontPx * 1.4f + 4.0f * g_dpiScale;
        if (g_settings.peakFreqAlignV == VizTextAlignV::Above)
            textTop = std::max(textTop, pfH + pfOffY);
        else if (g_settings.peakFreqAlignV == VizTextAlignV::Below)
            textBottom = std::max(textBottom, pfH + pfOffY);
        else {
            // Inside placements still need room once an offset can push them
            // past the bars in either direction.
            textTop    = std::max(textTop, pfOffY);
            textBottom = std::max(textBottom, pfOffY);
        }
        textAnchorSide = std::max(textAnchorSide, 60.0f * g_dpiScale);
        // A loudness readout is a whole line of figures: reserve enough either
        // side for it to sit centred on the bars without being clipped.
        float wide = VizReadoutWidthEstimate();
        if (wide > 0.f) textAnchorSide = std::max(textAnchorSide, (wide - totalWidth) * 0.5f + 8.0f * g_dpiScale);
        extraSide = std::max(extraSide, pfOffX);
    }

    textSide = textAnchorSide + extraSide;

    float insetL = padL + margin + textSide;
    float insetT = padT + margin + textTop;
    float insetR = padR + margin + textSide;
    float insetB = padB + margin + textBottom;

    float rawOriginX = blockX - insetL;
    float rawOriginY = blockY - insetT;
    if (g_overlayInPanel) {
        // The whole box (bars, padding and the room kept for text) centred in
        // the panel, which is sized to it (VizPanelFit).
        rawOriginX = waLeft + (workWidth - (totalWidth + insetL + insetR)) * 0.5f;
        rawOriginY = waTop + (workHeight - (totalHeight + insetT + insetB)) * 0.5f;
    }

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
    out->textSide       = textSide;
    out->textAnchorSide = textAnchorSide;
    out->textTop        = textTop;
    out->textBottom     = textBottom;

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
    g_dpiScale = VizHostDpiScale(monitor);

    VizLayout layout;
    if (!ComputeVizLayout(&layout)) return false;
    // The panel takes the box's size once the box is known.
    if (g_overlayInPanel && g_overlayWnd) PostMessage(g_overlayWnd, WM_APP_PANEL_FIT, 0, 0);

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

    // Sprite batches arrived with Windows 10 1607. Without them Smooth Mode
    // still paces and bakes the background; bars just draw the 1.4 way.
    g_dc3.Reset();
    g_spriteBatch.Reset();
    if (SUCCEEDED(g_dc.As(&g_dc3)) && g_dc3) {
        if (FAILED(g_dc3->CreateSpriteBatch(&g_spriteBatch))) {
            g_spriteBatch.Reset();
            g_dc3.Reset();
        }
    }

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

    hr = g_compositionDevice->CreateVisual(&g_rootVisual);
    if (FAILED(hr)) return false;

    hr = g_compositionDevice->CreateVisual(&g_compositionVisual);
    if (FAILED(hr)) return false;

    hr = g_rootVisual->AddVisual(g_compositionVisual.Get(), TRUE, nullptr);
    if (FAILED(hr)) return false;

    hr = g_compositionVisual->SetContent(g_swapChain.Get());
    if (FAILED(hr)) return false;

    // Position the (now much smaller) swap chain where the widget belongs.
    // The overlay window spans the whole virtual desktop starting at its origin,
    // so window-client coordinates match the virtual-screen-relative coordinates
    // the layout produces.
    g_compositionVisual->SetOffsetX(layout.originX);
    g_compositionVisual->SetOffsetY(layout.originY);

    hr = g_compositionDevice->CreateVisual(&g_splitTop);
    if (FAILED(hr)) return false;
    hr = g_splitTop->AddVisual(g_rootVisual.Get(), FALSE, nullptr);
    if (FAILED(hr)) return false;
    g_split = {};

    hr = g_compositionTarget->SetRoot(g_splitTop.Get());
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

    if (sizeChanged && g_overlayInPanel && g_overlayWnd) PostMessage(g_overlayWnd, WM_APP_PANEL_FIT, 0, 0);

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

// Session state for an in-progress drag. Touched only from the low-level mouse
// hook, which only ever runs on the UI thread that installed it -- no locking
// needed here, unlike the atomics above that the render thread also reads.
bool g_dragInProgress = false;
bool g_dragMoved = false;
POINT g_dragStartCursor{};
float g_dragStartH = 50.0f, g_dragStartV = 50.0f;
ULONGLONG g_dragLastClickTick = 0;
POINT g_dragLastClickPos{};

void VizCollectSnapLines();

void BeginDrag(POINT pt) {
    g_dragInProgress = true;
    g_dragMoved = false;
    g_dragStartCursor = pt;
    g_dragStartH = g_dragOverrideActive.load(std::memory_order_relaxed)
                       ? g_dragOverrideH.load(std::memory_order_relaxed)
                       : g_settings.horizontalPosition;
    g_dragStartV = g_dragOverrideActive.load(std::memory_order_relaxed)
                       ? g_dragOverrideV.load(std::memory_order_relaxed)
                       : g_settings.verticalPosition;
    g_dragRenderPauseActive.store(true, std::memory_order_relaxed);
    VizCollectSnapLines();

    Wh_Log(L"[Drag] BEGIN cursor=(%d,%d) startH=%.2f startV=%.2f",
           pt.x, pt.y, g_dragStartH, g_dragStartV);
}

// The distance, in physical pixels, that the visualizer's box can travel across
// the work area on each axis -- i.e. what 0%..100% of a Position setting spans.
// Mirrors the sizing math in ComputeVizLayout so a pixel delta converts to the
// same percent delta that layout will turn back into pixels.
bool GetVizTravelRange(float* travelX, float* travelY) {
    HMONITOR monitor = g_cachedMonitor;
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return false;

    int barCount = VizEffectiveBarCount();
    float barW = std::max(1.f, VizPx((float)std::max(1.f, g_settings.barWidth)));
    float barGap = VizPx((float)std::max(0.f, g_settings.barGap));
    float maxSize = std::max(2.f, VizPx((float)std::max(2.f, g_settings.barMaxSize)));
    bool horizontal = (g_settings.orientation == VizOrientation::Horizontal);
    float barsThickness = barCount * barW + (barCount - 1) * barGap;
    float totalWidth, totalHeight;
    if (g_settings.shape == VizShape::Terminal) {
        VizTermBox(&totalWidth, &totalHeight);
    } else if (g_settings.shape == VizShape::Radial || g_settings.shape == VizShape::Goniometer) {
        totalWidth = totalHeight = maxSize * 2.0f;
    } else {
        totalWidth  = horizontal ? barsThickness : maxSize;
        totalHeight = horizontal ? maxSize       : barsThickness;
    }
    VizStyleBox(&totalWidth, &totalHeight, maxSize, horizontal);

    const RECT area = VizPlaceArea(monitor, mi);
    *travelX = (float)(area.right  - area.left) - totalWidth;
    *travelY = (float)(area.bottom - area.top)  - totalHeight;
    return true;
}

// ---- Drag snapping ---------------------------------------------------------
// Lines the box can catch on while it is dragged, in screen pixels. Gathered once
// when a drag starts (the taskbar does not move mid-drag), then each move only
// compares a handful of numbers.
std::vector<float> g_snapLinesX, g_snapLinesY;

// ---- Dock to an app window -------------------------------------------------
HWND g_dockTarget = nullptr;
HWINEVENTHOOK g_dockHook = nullptr;
std::wstring g_dockFor;  // the app name the current target was found for

void VizDockRedraw() {
    if (g_overlayWnd) PostMessage(g_overlayWnd, WM_APP_FORCE_REDRAW, 0, 0);
}

// Reads the target's frame; false while it is minimized, hidden or gone.
bool VizDockReadRect() {
    HWND w = g_dockTarget;
    RECT r{};
    bool ok = w && IsWindow(w) && IsWindowVisible(w) && !IsIconic(w);
    if (ok && FAILED(DwmGetWindowAttribute(w, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r))))
        ok = GetWindowRect(w, &r) != 0;
    ok = ok && r.right > r.left && r.bottom > r.top;
    bool was = g_dockActive.load(std::memory_order_relaxed);
    if (ok) {
        bool moved = r.left != g_dockL.load(std::memory_order_relaxed) || r.top != g_dockT.load(std::memory_order_relaxed) ||
                     r.right != g_dockR.load(std::memory_order_relaxed) || r.bottom != g_dockB.load(std::memory_order_relaxed);
        g_dockL.store(r.left, std::memory_order_relaxed);
        g_dockT.store(r.top, std::memory_order_relaxed);
        g_dockR.store(r.right, std::memory_order_relaxed);
        g_dockB.store(r.bottom, std::memory_order_relaxed);
        g_dockActive.store(true, std::memory_order_relaxed);
        if (moved || !was) VizDockRedraw();
    } else if (was) {
        g_dockActive.store(false, std::memory_order_relaxed);
        VizDockRedraw();
    }
    return ok;
}

void CALLBACK VizDockEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (hwnd == g_dockTarget && idObject == OBJID_WINDOW && idChild == CHILDID_SELF) VizDockReadRect();
}

void VizDockStop() {
    if (g_dockHook) UnhookWinEvent(g_dockHook);
    g_dockHook = nullptr;
    g_dockTarget = nullptr;
    if (g_dockActive.exchange(false)) VizDockRedraw();
}

static bool VizProcessNameIs(DWORD pid, const std::wstring& want) {
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return false;
    WCHAR path[MAX_PATH];
    DWORD n = MAX_PATH;
    bool match = false;
    if (QueryFullProcessImageNameW(p, 0, path, &n)) {
        std::wstring name = path;
        size_t slash = name.find_last_of(L"\\/");
        if (slash != std::wstring::npos) name = name.substr(slash + 1);
        for (auto& ch : name) ch = (WCHAR)towlower(ch);
        if (name.size() > 4 && name.compare(name.size() - 4, 4, L".exe") == 0) name.resize(name.size() - 4);
        match = name == want;
    }
    CloseHandle(p);
    return match;
}

struct VizDockFind { const std::wstring* want; HWND found; DWORD lastPid; };

BOOL CALLBACK VizDockEnumProc(HWND w, LPARAM lp) {
    auto* f = (VizDockFind*)lp;
    if (!IsWindowVisible(w) || GetWindow(w, GW_OWNER) || IsIconic(w)) return TRUE;
    if (GetWindowLongPtr(w, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindowTextLengthW(w) == 0) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (!pid || pid == f->lastPid || pid == GetCurrentProcessId()) return TRUE;
    f->lastPid = pid;
    if (!VizProcessNameIs(pid, *f->want)) return TRUE;
    f->found = w;
    return FALSE;
}

// Called on the 1 s watch timer: finds the app's main window when there is none
// yet (the app may start later), and watches it move through a WinEvent hook
// scoped to that one process, so following it costs nothing while it sits still.
void VizDockRefresh() {
    const std::wstring& want = g_settings.dockApp;
    if (want != g_dockFor) { VizDockStop(); g_dockFor = want; }
    if (want.empty()) return;
    if (g_dockTarget && IsWindow(g_dockTarget)) { VizDockReadRect(); return; }
    VizDockStop();

    VizDockFind f{&want, nullptr, 0};
    EnumWindows(VizDockEnumProc, (LPARAM)&f);
    if (!f.found) return;

    g_dockTarget = f.found;
    DWORD pid = 0;
    GetWindowThreadProcessId(f.found, &pid);
    g_dockHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                                 VizDockEventProc, pid, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    Wh_Log(L"[Dock] following %s (hwnd=%p hook=%p)", want.c_str(), f.found, g_dockHook);
    VizDockReadRect();
}

// Where the docked box goes, in screen pixels, for a box of the given size.
void VizDockPlace(float boxW, float boxH, float* x, float* y) {
    float l = (float)g_dockL.load(std::memory_order_relaxed), t = (float)g_dockT.load(std::memory_order_relaxed);
    float r = (float)g_dockR.load(std::memory_order_relaxed), b = (float)g_dockB.load(std::memory_order_relaxed);
    float gap = g_settings.dockGap * g_dpiScale, shift = g_settings.dockShift * g_dpiScale;
    int side = g_settings.dockSide, align = g_settings.dockAlign;
    if (side <= 1) {
        *x = (align == 0 ? l : align == 2 ? r - boxW : (l + r - boxW) * 0.5f) + shift;
        *y = side == 0 ? b + gap : t - boxH - gap;
    } else {
        *y = (align == 0 ? t : align == 2 ? b - boxH : (t + b - boxH) * 0.5f) + shift;
        *x = side == 3 ? r + gap : l - boxW - gap;
    }
}

void VizSnapAddWindow(HWND w, bool xOnly) {
    RECT r;
    if (!w || !IsWindowVisible(w) || !GetWindowRect(w, &r) || r.right <= r.left || r.bottom <= r.top) return;
    g_snapLinesX.push_back((float)r.left);
    g_snapLinesX.push_back((float)r.right);
    g_snapLinesX.push_back((r.left + r.right) * 0.5f);
    if (xOnly) return;
    g_snapLinesY.push_back((float)r.top);
    g_snapLinesY.push_back((float)r.bottom);
}

void VizCollectSnapLines() {
    g_snapLinesX.clear();
    g_snapLinesY.clear();
    if (!g_settings.dragSnap) return;
    HMONITOR monitor = g_cachedMonitor;
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return;
    const RECT wk = VizPlaceArea(monitor, mi);
    g_snapLinesX = {(float)wk.left, (float)wk.right, (wk.left + wk.right) * 0.5f};
    g_snapLinesY = {(float)wk.top, (float)wk.bottom, (wk.top + wk.bottom) * 0.5f};

    // This monitor's taskbar: its edge, then the tray and the Start button along it.
    for (PCWSTR cls : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        for (HWND tb = FindWindowEx(nullptr, nullptr, cls, nullptr); tb;
             tb = FindWindowEx(nullptr, tb, cls, nullptr)) {
            if (MonitorFromWindow(tb, MONITOR_DEFAULTTONULL) != monitor) continue;
            RECT r;
            if (GetWindowRect(tb, &r)) {
                g_snapLinesY.push_back((float)r.top);
                g_snapLinesY.push_back((float)r.bottom);
            }
            VizSnapAddWindow(FindWindowEx(tb, nullptr, L"TrayNotifyWnd", nullptr), true);
            VizSnapAddWindow(FindWindowEx(tb, nullptr, L"Start", nullptr), true);
        }
    }
}

bool VizSnapBypassHeld() {
    int key = g_settings.dragModifier == VizDragModifier::Shift ? VK_MENU : VK_SHIFT;
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

// Moves a box [lo, lo+size] so its nearest edge or centre sits on a line within
// reach. Returns the adjusted lo.
float VizSnapAxis(float lo, float size, const std::vector<float>& lines, float reach) {
    float best = reach + 1.f, shift = 0.f;
    for (float line : lines) {
        for (float a : {lo, lo + size * 0.5f, lo + size}) {
            float d = line - a;
            if (std::abs(d) < best) { best = std::abs(d); shift = d; }
        }
    }
    return best <= reach ? lo + shift : lo;
}

void VizApplyDragSnap(float* h, float* v, float travelX, float travelY) {
    if (!g_settings.dragSnap || g_snapLinesX.empty() || VizSnapBypassHeld()) return;
    HMONITOR monitor = g_cachedMonitor;
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return;
    const RECT wk = VizPlaceArea(monitor, mi);
    float reach = g_settings.dragSnapDistance * g_dpiScale;
    if (travelX > 1.0f) {
        float boxW = (float)(wk.right - wk.left) - travelX;
        float lo = VizSnapAxis(wk.left + travelX * *h / 100.0f, boxW, g_snapLinesX, reach);
        *h = std::clamp((lo - wk.left) / travelX * 100.0f, 0.0f, 100.0f);
    }
    if (travelY > 1.0f) {
        float boxH = (float)(wk.bottom - wk.top) - travelY;
        float lo = VizSnapAxis(wk.top + travelY * *v / 100.0f, boxH, g_snapLinesY, reach);
        *v = std::clamp((lo - wk.top) / travelY * 100.0f, 0.0f, 100.0f);
    }
}

void UpdateDrag(POINT pt) {
    if (!g_dragInProgress) return;

    if (std::abs(pt.x - g_dragStartCursor.x) > 3 || std::abs(pt.y - g_dragStartCursor.y) > 3)
        g_dragMoved = true;

    float travelX = 0.f, travelY = 0.f;
    if (!GetVizTravelRange(&travelX, &travelY)) return;

    float dxPercent = (travelX > 1.0f) ? ((float)(pt.x - g_dragStartCursor.x) / travelX) * 100.0f : 0.f;
    float dyPercent = (travelY > 1.0f) ? ((float)(pt.y - g_dragStartCursor.y) / travelY) * 100.0f : 0.f;

    float newH = std::clamp(g_dragStartH + dxPercent, 0.0f, 100.0f);
    float newV = std::clamp(g_dragStartV + dyPercent, 0.0f, 100.0f);
    VizApplyDragSnap(&newH, &newV, travelX, travelY);
    g_dragOverrideH.store(newH, std::memory_order_relaxed);
    g_dragOverrideV.store(newV, std::memory_order_relaxed);
    g_dragOverrideActive.store(true, std::memory_order_relaxed);

    static int s_dragLogCounter = 0;
    if ((++s_dragLogCounter % 15) == 0) {
        Wh_Log(L"[Drag] MOVE cursor=(%d,%d) travel=(%.1f,%.1f) delta%%=(%.2f,%.2f) newHV=(%.2f,%.2f) monitorCached=%d",
               pt.x, pt.y, travelX, travelY, dxPercent, dyPercent,
               g_dragOverrideH.load(std::memory_order_relaxed),
               g_dragOverrideV.load(std::memory_order_relaxed),
               g_cachedMonitor != nullptr);
    }

    // Deliberately NOT calling UpdateSwapChainForLayout() here. RenderVisualizer
    // already calls it once per tick, right before drawing -- doing it again
    // here, from the hook thread, on every single mouse-move (which can fire
    // far more often than the render tick during a fast drag) is exactly what
    // let the window's on-screen position race ahead of what actually got
    // drawn inside it.
}

void EndDrag() {
    if (!g_dragInProgress) return;
    g_dragInProgress = false;
    g_dragRenderPauseActive.store(false, std::memory_order_relaxed);

    float finalH = g_dragOverrideH.load(std::memory_order_relaxed);
    float finalV = g_dragOverrideV.load(std::memory_order_relaxed);
    Wh_Log(L"[Drag] END moved=%d finalHV=(%.2f,%.2f) overrideActive=%d",
           (int)g_dragMoved, finalH, finalV, (int)g_dragOverrideActive.load(std::memory_order_relaxed));

    // Deferred like the other hook paths: a WH_MOUSE_LL callback holds up all
    // input until it returns, so no storage write happens in here.
    if (g_dragMoved) {
        RequestPositionOverrideSave();
    }
}

// ---- Keyboard move ---------------------------------------------------------
// The precise way to place the visualizer: hold a modifier, tap a direction,
// and the box steps by an exact number of pixels. Nothing here has to track a
// moving cursor or keep up with the render loop -- each keypress is one
// discrete jump, applied to the same position override the drag path writes,
// and picked up by whatever the next frame happens to be.

// Asks the message window to (re)start a short timer that saves the override.
// Key repeat can fire dozens of nudges a second, so the write is deferred until
// the user stops moving rather than run on every keystroke.
void RequestPositionOverrideSave() {
    if (g_messageWnd) PostMessage(g_messageWnd, WM_APP_REQUEST_SAVE_POSITION, 0, 0);
}

bool ModKeysHeld(unsigned flags) {
    if (flags == VIZ_MOD_NONE) return false;
    // Right Alt is AltGr on many layouts, which Windows reports as Ctrl + Alt
    // (AltGr+2 is @ on a Nordic keyboard). Never treat it as a modifier, so
    // typing those characters always reaches the app.
    if (GetAsyncKeyState(VK_RMENU) & 0x8000) return false;
    if ((flags & VIZ_MOD_CTRL)  && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) return false;
    if ((flags & VIZ_MOD_ALT)   && !(GetAsyncKeyState(VK_LMENU)   & 0x8000)) return false;
    if ((flags & VIZ_MOD_SHIFT) && !(GetAsyncKeyState(VK_SHIFT)   & 0x8000)) return false;
    if ((flags & VIZ_MOD_WIN)   && !((GetAsyncKeyState(VK_LWIN) & 0x8000) ||
                                     (GetAsyncKeyState(VK_RWIN) & 0x8000))) return false;
    return true;
}

// Maps a virtual key to a direction, honouring which key set the user enabled.
// Returns false for anything that isn't a direction key we handle.
bool KeyMoveDirection(DWORD vk, int* dx, int* dy) {
    bool arrows = g_settings.keyMoveKeys != VizKeyMoveKeys::Wasd;
    bool wasd   = g_settings.keyMoveKeys != VizKeyMoveKeys::Arrows;

    *dx = *dy = 0;
    if (arrows) {
        switch (vk) {
            case VK_LEFT:  *dx = -1; return true;
            case VK_RIGHT: *dx =  1; return true;
            case VK_UP:    *dy = -1; return true;
            case VK_DOWN:  *dy =  1; return true;
        }
    }
    if (wasd) {
        switch (vk) {
            case 'A': *dx = -1; return true;
            case 'D': *dx =  1; return true;
            case 'W': *dy = -1; return true;
            case 'S': *dy =  1; return true;
        }
    }
    return false;
}

// Moves the visualizer by an exact pixel delta. The override is stored as a
// percentage (that's what ComputeVizLayout consumes), so the delta is converted
// through the same travel range layout uses -- which makes the round trip land
// on the pixel asked for rather than near it.
// Which piece the direction keys currently steer. Touched only from the input
// hook thread, and read by nothing else.
VizMoveTarget g_keyMoveTarget = VizMoveTarget::Visualizer;

void NudgeVisualizerPx(float dxPx, float dyPx) {
    float travelX = 0.f, travelY = 0.f;
    if (!GetVizTravelRange(&travelX, &travelY)) return;

    bool active = g_dragOverrideActive.load(std::memory_order_relaxed);
    float h = active ? g_dragOverrideH.load(std::memory_order_relaxed)
                     : g_settings.horizontalPosition;
    float v = active ? g_dragOverrideV.load(std::memory_order_relaxed)
                     : g_settings.verticalPosition;

    if (dxPx != 0.f && travelX > 1.0f) h = std::clamp(h + (dxPx / travelX) * 100.0f, 0.0f, 100.0f);
    if (dyPx != 0.f && travelY > 1.0f) v = std::clamp(v + (dyPx / travelY) * 100.0f, 0.0f, 100.0f);

    g_dragOverrideH.store(h, std::memory_order_relaxed);
    g_dragOverrideV.store(v, std::memory_order_relaxed);
    g_dragOverrideActive.store(true, std::memory_order_relaxed);

    // Rendering may well be idle right now -- repositioning is something people
    // do with nothing playing -- so ask for one frame explicitly instead of
    // waiting for a tick that isn't coming.
    if (g_overlayWnd) PostMessage(g_overlayWnd, WM_APP_FORCE_REDRAW, 0, 0);
    RequestPositionOverrideSave();
}

// The media strip's own travel range, mirroring the sizing math in
// RepositionAndRepaintMediaControls so a pixel delta converts to the same
// percent delta that repositioning will turn back into pixels.
bool GetMediaTravelRange(float* travelX, float* travelY) {
    float dpiScale = GetMediaControlsDpiScale();
    int size = std::max(1, (int)std::lround(g_settings.mediaIconSize * dpiScale));
    int spacing = std::max(0, (int)std::lround(g_settings.mediaIconSpacing * dpiScale));
    int pad = GetMediaPlatePaddingPx();
    int width = size * 3 + spacing * 2 + pad * 2;
    int height = size + pad * 2;
    if (VizCardActive()) VizCardSize(&width, &height);

    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return false;

    *travelX = (float)(mi.rcWork.right - mi.rcWork.left - width);
    *travelY = (float)(mi.rcWork.bottom - mi.rcWork.top - height);
    return true;
}

// Moves the media strip by an exact pixel delta. Percentage-based like the
// visualizer rather than a pixel offset like the text overlays, because the
// strip is placed against the monitor rather than against anything else.
void NudgeMediaControlsPx(int dxPx, int dyPx) {
    float travelX = 0.f, travelY = 0.f;
    if (!GetMediaTravelRange(&travelX, &travelY)) return;

    bool active = g_mediaOverrideActive.load(std::memory_order_relaxed);
    float h = active ? g_mediaOverrideH.load(std::memory_order_relaxed)
                     : g_settings.mediaHorizontalPosition;
    float v = active ? g_mediaOverrideV.load(std::memory_order_relaxed)
                     : g_settings.mediaVerticalPosition;

    if (dxPx && travelX > 1.0f) h = std::clamp(h + (dxPx / travelX) * 100.0f, 0.0f, 100.0f);
    if (dyPx && travelY > 1.0f) v = std::clamp(v + (dyPx / travelY) * 100.0f, 0.0f, 100.0f);

    g_mediaOverrideH.store(h, std::memory_order_relaxed);
    g_mediaOverrideV.store(v, std::memory_order_relaxed);
    g_mediaOverrideActive.store(true, std::memory_order_relaxed);

    // Posted rather than called: the strip's window belongs to the UI thread
    // and repositioning it means touching UpdateLayeredWindow, which has no
    // business happening on this hook thread.
    if (g_mediaWnd) PostMessage(g_mediaWnd, WM_APP_MEDIA_REPAINT, 0, 0);
    RequestPositionOverrideSave();
}

// Nudges one of the text overlays. These move in raw pixels relative to the
// visualizer rather than as a percentage of the screen, because that's what
// "put the title just above the panel" actually means -- they're positioned
// against the box, not against the monitor.
void NudgeTextOverlayPx(VizMoveTarget target, int dxPx, int dyPx) {
    std::atomic<int>* ax = nullptr;
    std::atomic<int>* ay = nullptr;
    if (target == VizMoveTarget::NowPlaying) {
        ax = &g_npNudgeX; ay = &g_npNudgeY;
    } else if (target == VizMoveTarget::PeakFreq) {
        ax = &g_pfNudgeX; ay = &g_pfNudgeY;
    } else {
        return;
    }

    // Capped rather than unbounded: every pixel of offset widens the render
    // surface by the same amount (see ComputeVizLayout), so an accidental
    // key-repeat run shouldn't be able to inflate it without limit.
    constexpr int kMaxNudge = 4000;
    ax->store(std::clamp(ax->load(std::memory_order_relaxed) + dxPx, -kMaxNudge, kMaxNudge),
              std::memory_order_relaxed);
    ay->store(std::clamp(ay->load(std::memory_order_relaxed) + dyPx, -kMaxNudge, kMaxNudge),
              std::memory_order_relaxed);

    if (g_overlayWnd) PostMessage(g_overlayWnd, WM_APP_FORCE_REDRAW, 0, 0);
    RequestPositionOverrideSave();
}

// Zeroes the live nudge for whatever is selected. For the visualizer that means
// throwing away the whole saved position; for the text overlays it means
// dropping back to whatever their Offset settings say.
void ResetMoveTarget(VizMoveTarget target) {
    switch (target) {
        case VizMoveTarget::NowPlaying:
            g_npNudgeX.store(0, std::memory_order_relaxed);
            g_npNudgeY.store(0, std::memory_order_relaxed);
            RequestPositionOverrideSave();
            break;
        case VizMoveTarget::PeakFreq:
            g_pfNudgeX.store(0, std::memory_order_relaxed);
            g_pfNudgeY.store(0, std::memory_order_relaxed);
            RequestPositionOverrideSave();
            break;
        case VizMoveTarget::MediaControls:
            g_mediaOverrideActive.store(false, std::memory_order_relaxed);
            RequestPositionOverrideSave();
            if (g_mediaWnd) PostMessage(g_mediaWnd, WM_APP_MEDIA_REPAINT, 0, 0);
            return;  // nothing about the visualizer's own surface changed
        default:
            ClearPositionOverride();
            break;
    }
    if (g_overlayWnd) PostMessage(g_overlayWnd, WM_APP_FORCE_REDRAW, 0, 0);
}

// Keys whose press the hook swallowed; only their releases are swallowed, so
// an app never sees a release without its press or a press without its release.
bool g_keyMoveSwallowed[256] = {};  // input hook thread only

LRESULT CALLBACK MoveKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
        if (isUp && kb->vkCode < 256 && g_keyMoveSwallowed[kb->vkCode]) {
            g_keyMoveSwallowed[kb->vkCode] = false;
            return 1;
        }
    }
    if (nCode == HC_ACTION && !g_unloading.load(std::memory_order_relaxed) &&
        g_settings.keyMoveEnabled) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

        if (isDown && kb->vkCode < 256 && !g_fullscreenPaused.load(std::memory_order_relaxed) &&
            ModKeysHeld(g_settings.keyMoveModifier)) {
            int dx = 0, dy = 0;
            if (KeyMoveDirection(kb->vkCode, &dx, &dy)) {
                if (isDown) {
                    bool fast = g_settings.keyMoveFastKey != VIZ_MOD_NONE &&
                                ModKeysHeld(g_settings.keyMoveFastKey);
                    float fstep = fast ? g_settings.keyMoveFastStep : g_settings.keyMoveStep;
                    int step = std::max(1, (int)lroundf(fstep));  // the strip and the text: whole pixels
                    // Fine: a fraction of a pixel. Only the visualizer can sit
                    // between pixels; the strip and the text move whole pixels.
                    bool fine = !fast && (g_settings.keyMoveFine ||
                                          (g_settings.keyMoveFineKey != VIZ_MOD_NONE &&
                                           ModKeysHeld(g_settings.keyMoveFineKey)));
                    float vstep = fine ? g_settings.keyMoveFineStep : fstep;
                    if (g_keyMoveTarget == VizMoveTarget::Visualizer) {
                        NudgeVisualizerPx(dx * vstep, dy * vstep);
                    } else if (g_keyMoveTarget == VizMoveTarget::MediaControls) {
                        NudgeMediaControlsPx(dx * step, dy * step);
                    } else {
                        NudgeTextOverlayPx(g_keyMoveTarget, dx * step, dy * step);
                    }
                }
                g_keyMoveSwallowed[kb->vkCode] = true;  // its release is swallowed too
                return 1;
            }

            // Same combo + 1/2/3/4 picks what the direction keys steer. One
            // combo places every movable piece rather than needing a shortcut
            // each, and the choice sticks until it's changed again.
            if (kb->vkCode >= '1' && kb->vkCode <= '4') {
                if (isDown) {
                    g_keyMoveTarget = (kb->vkCode == '2') ? VizMoveTarget::NowPlaying
                                    : (kb->vkCode == '3') ? VizMoveTarget::PeakFreq
                                    : (kb->vkCode == '4') ? VizMoveTarget::MediaControls
                                                          : VizMoveTarget::Visualizer;
                    Wh_Log(L"[KeyMove] target = %s",
                           g_keyMoveTarget == VizMoveTarget::NowPlaying    ? L"Now Playing text"
                           : g_keyMoveTarget == VizMoveTarget::PeakFreq    ? L"frequency readout"
                           : g_keyMoveTarget == VizMoveTarget::MediaControls ? L"media controls"
                                                                            : L"visualizer");
                }
                g_keyMoveSwallowed[kb->vkCode] = true;
                return 1;
            }

            // Same combo + Home resets whatever is selected: the visualizer
            // hands placement back to the Position percentages, a text overlay
            // drops back to whatever its Offset settings say. There's no
            // settings-UI way to do this, since mods can't write settings back.
            if (kb->vkCode == VK_HOME) {
                if (isDown) {
                    ResetMoveTarget(g_keyMoveTarget);
                    Wh_Log(L"[KeyMove] reset target %d", (int)g_keyMoveTarget);
                }
                g_keyMoveSwallowed[kb->vkCode] = true;
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// True when what is under `pt` is the desktop the visualizer lives on (its
// WorkerW / Progman layer), not an application window covering it. Window
// lookup and class names only, no messages: this runs inside a low-level
// mouse hook, where anything slow holds up all input.
bool VizDesktopUnderPoint(POINT pt) {
    HWND h = WindowFromPoint(pt);
    if (!h) return false;
    HWND root = GetAncestor(h, GA_ROOT);
    if (!root) root = h;
    WCHAR cls[32] = {};
    GetClassNameW(root, cls, ARRAYSIZE(cls));
    if (g_overlayInPanel && root == g_overlayWnd) return true;
    // Tourne Desktop (the author's separate desktop app, not in the catalog) can draw its own taskbar over
    // Windows' one; the empty space between its segments takes clicks, so it counts as the taskbar too.
    if (g_settings.hostTaskbar && (wcscmp(cls, L"Shell_TrayWnd") == 0 || wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
                                   wcsncmp(cls, L"TourneTaskbar", 13) == 0))
        return true;
    return wcscmp(cls, L"WorkerW") == 0 || wcscmp(cls, L"Progman") == 0;
}

std::atomic<bool> g_userPaused{false};  // Pause Visualizer, from the right-click menu
bool g_menuButtonDown = false;          // hook thread only

// Right-click over the visualizer opens the quick-settings menu (Interaction,
// Right-Click Menu). The press and release are both swallowed, so the
// desktop's own menu doesn't open as well; a drag bound to the right button
// with its modifier held keeps priority.
bool VizMenuHook(WPARAM wParam, const MSLLHOOKSTRUCT* info) {
    if (g_settings.contextMenu == VizContextMenu::Off) return false;
    // Our menu is up: everything goes through, so a click elsewhere (either
    // button) dismisses it the normal way. Only the release of a press this
    // hook already swallowed is swallowed too, so the desktop never gets an
    // unpaired button-up.
    if (g_menuOpen.load(std::memory_order_acquire)) {
        if (wParam == WM_RBUTTONUP && g_menuButtonDown) {
            g_menuButtonDown = false;
            return true;
        }
        return false;
    }
    if (wParam == WM_RBUTTONDOWN) {
        g_menuButtonDown = false;
        if (g_settings.dragEnabled && g_settings.dragButton == VizDragButton::Right && DragModifierHeld())
            return false;
        if (g_settings.contextMenu == VizContextMenu::CtrlRightClick && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
            return false;
        // Hidden for a fullscreen app or a covering window: nothing to click.
        if (g_fullscreenPaused.load(std::memory_order_relaxed) && !g_userPaused.load(std::memory_order_relaxed))
            return false;
        // Faded out by Auto-Hide: the draw rect is still valid, but there is
        // nothing visible there, so the click belongs to the desktop. (Pause
        // Visualizer still catches it: that is how it gets unticked.)
        if (g_vizSceneHidden.load(std::memory_order_relaxed) && !g_userPaused.load(std::memory_order_relaxed))
            return false;
        if (!PointInVisualizerBounds(info->pt) || !VizDesktopUnderPoint(info->pt)) return false;
        g_menuButtonDown = true;
        return true;
    }
    if (wParam == WM_RBUTTONUP && g_menuButtonDown) {
        g_menuButtonDown = false;
        if (g_messageWnd) PostMessage(g_messageWnd, WM_APP_CONTEXT_MENU, (WPARAM)info->pt.x, (LPARAM)info->pt.y);
        return true;
    }
    return false;
}

// ---- Click to Seek (2.1) -----------------------------------------------------------------
// A left press on the progress bar jumps there; dragging and releasing jumps
// again where it's let go. The cursor moves freely meanwhile (moves are
// never swallowed). The hook only decides and posts: the app list is checked
// by the message window's 1-second timer (a process snapshot every 3 s, only
// while Click to Seek has a list), and the seek itself runs on that thread.
std::atomic<bool> g_seekAppsAllowed{true};

void VizSeekAppsRefresh() {
    const auto& apps = g_settings.progressSeekApps;
    if (!g_settings.progressSeek || apps.empty()) {
        g_seekAppsAllowed.store(true, std::memory_order_relaxed);
        return;
    }
    static ULONGLONG s_tick = 0;
    ULONGLONG now = GetTickCount64();
    if (s_tick != 0 && now - s_tick < 3000) return;
    s_tick = now;
    bool anyRunning = false;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe = {sizeof(pe)};
        for (BOOL more = Process32FirstW(snap, &pe); more && !anyRunning; more = Process32NextW(snap, &pe)) {
            std::wstring n = pe.szExeFile;
            for (auto& ch : n) ch = (WCHAR)towlower(ch);
            if (n.size() > 4 && n.compare(n.size() - 4, 4, L".exe") == 0) n.resize(n.size() - 4);
            for (const auto& a : apps)
                if (n == a) anyRunning = true;
        }
        CloseHandle(snap);
    }
    g_seekAppsAllowed.store(g_settings.progressSeekOnlyListed ? anyRunning : !anyRunning, std::memory_order_relaxed);
}

void VizSeekAtX(LONG x) {
    LONG l = g_seekRectL.load(std::memory_order_relaxed), r = g_seekRectR.load(std::memory_order_relaxed);
    if (r > l) CardSeek((float)(x - l) / (float)(r - l));
}

void VizPostSeek(LONG x) {
    if (g_messageWnd) PostMessage(g_messageWnd, WM_APP_SEEK_AT, (WPARAM)(int)x, 0);
}

bool VizSeekHook(WPARAM wParam, const MSLLHOOKSTRUCT* info) {
    static bool s_seeking = false;
    if (s_seeking) {
        if (wParam == WM_LBUTTONUP) {
            s_seeking = false;
            VizPostSeek(info->pt.x);
            return true;
        }
        // A press while we think the button is still down: its release was
        // lost (Win+L, a UAC prompt), so the earlier seek is over.
        if (wParam != WM_LBUTTONDOWN) return false;
        s_seeking = false;
    }
    if (wParam != WM_LBUTTONDOWN || !g_settings.progressSeek || !g_seekRectValid.load(std::memory_order_relaxed))
        return false;
    if (g_settings.dragEnabled && DragButtonDownMsg() == WM_LBUTTONDOWN && DragModifierHeld()) return false;
    if (g_fullscreenPaused.load(std::memory_order_relaxed) || g_vizSceneHidden.load(std::memory_order_relaxed))
        return false;
    POINT pt = info->pt;
    if (pt.x < g_seekRectL.load(std::memory_order_relaxed) || pt.x >= g_seekRectR.load(std::memory_order_relaxed) ||
        pt.y < g_seekRectT.load(std::memory_order_relaxed) || pt.y >= g_seekRectB.load(std::memory_order_relaxed))
        return false;
    if (!g_tlValid.load(std::memory_order_relaxed) || !g_seekAppsAllowed.load(std::memory_order_relaxed) ||
        !VizDesktopUnderPoint(pt))
        return false;
    s_seeking = true;
    VizPostSeek(pt.x);
    return true;
}

LRESULT CALLBACK DragMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && !g_unloading.load(std::memory_order_relaxed) &&
        VizMenuHook(wParam, (const MSLLHOOKSTRUCT*)lParam))
        return 1;
    if (nCode == HC_ACTION && !g_unloading.load(std::memory_order_relaxed) &&
        VizSeekHook(wParam, (const MSLLHOOKSTRUCT*)lParam))
        return 1;
    if (nCode == HC_ACTION && !g_unloading.load(std::memory_order_relaxed) &&
        g_settings.dragEnabled && !g_overlayInPanel) {
        MSLLHOOKSTRUCT* info = (MSLLHOOKSTRUCT*)lParam;
        UINT downMsg = DragButtonDownMsg();
        UINT upMsg = DragButtonUpMsg();

        if (!g_dragInProgress) {
            if (wParam == downMsg && DragModifierHeld() && PointInVisualizerBounds(info->pt) &&
                !g_fullscreenPaused.load(std::memory_order_relaxed) &&
                !g_vizSceneHidden.load(std::memory_order_relaxed) && VizDesktopUnderPoint(info->pt)) {
                BeginDrag(info->pt);
                return 1;
            }
        } else {
            // Moves go through: the cursor travels with the visualizer, and
            // UpdateDrag works from the absolute position against the start.
            if (wParam == WM_MOUSEMOVE) {
                UpdateDrag(info->pt);
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
            // A second press of the drag button: the release of the first one
            // never came (see VizDesktopSwitchProc), so that drag is over.
            if (wParam == downMsg) {
                EndDrag();
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
            if (wParam == upMsg) {
                POINT pt = info->pt;
                bool wasDrag = g_dragMoved;
                EndDrag();

                if (!wasDrag) {
                    // Not an actual drag -- a plain click-release of the combo.
                    // Two of those close together in place clear a saved
                    // override, since there's no settings-UI way to do it.
                    ULONGLONG now = GetTickCount64();
                    int dx = std::abs(pt.x - g_dragLastClickPos.x);
                    int dy = std::abs(pt.y - g_dragLastClickPos.y);
                    if (g_dragLastClickTick != 0 &&
                        now - g_dragLastClickTick < GetDoubleClickTime() && dx < 6 && dy < 6) {
                        // Not calling UpdateSwapChainForLayout() directly here:
                        // this hook now runs on its own dedicated thread (see
                        // InitInputHooks), so touching the swap chain from here
                        // would race with the render tick that owns it. The
                        // very next tick already re-derives layout from
                        // g_dragOverrideActive and will pick this up on its own.
                        ClearPositionOverride();
                        g_dragLastClickTick = 0;
                    } else {
                        g_dragLastClickTick = now;
                        g_dragLastClickPos = pt;
                    }
                }
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

HHOOK g_dragMouseHook = nullptr;
HHOOK g_moveKeyboardHook = nullptr;
HANDLE g_inputHookThread = nullptr;
DWORD g_inputHookThreadId = 0;

// Win+L, Ctrl+Alt+Del and UAC prompts switch to a secure desktop, and a button
// released there never reaches the mouse hook. A drag left running would keep
// moving the visualizer with the cursor after unlocking, so a desktop switch
// ends it. Runs on the input hook thread.
void CALLBACK VizDesktopSwitchProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
    if (g_dragInProgress) EndDrag();
}
constexpr UINT WM_APP_KEYMOVE_SYNC = WM_APP + 90;  // thread message to the input hook thread

// Both low-level input hooks run on this thread, deliberately kept away from
// the UI thread that also handles WM_APP_RENDER_TICK. Direct2D's Present() can
// block that thread for real time each frame; a hook living there has its
// queued input backed up behind every present, which is what made drag feel
// like pulling the cursor through mud. Windows also silently drops a
// low-level hook whose thread doesn't service messages promptly, so this
// thread does nothing but pump.
DWORD WINAPI InputHookThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HINSTANCE hInst = (HINSTANCE)GetCurrentModuleHandle();

    g_dragMouseHook = SetWindowsHookEx(WH_MOUSE_LL, DragMouseHookProc, hInst, 0);
    HWINEVENTHOOK desktopSwitch = SetWinEventHook(EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH, nullptr,
                                                  VizDesktopSwitchProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_dragMouseHook) {
        Wh_Log(L"[Drag] SetWindowsHookEx(WH_MOUSE_LL) failed, error=%lu", GetLastError());
    }

    // The keyboard hook exists only while Keyboard Move is on; turning it on
    // or off in the settings posts WM_APP_KEYMOVE_SYNC here.
    auto syncKeyboardHook = [hInst] {
        bool want = g_settings.keyMoveEnabled;
        if (want && !g_moveKeyboardHook) {
            g_moveKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, MoveKeyboardHookProc, hInst, 0);
            if (!g_moveKeyboardHook)
                Wh_Log(L"[KeyMove] SetWindowsHookEx(WH_KEYBOARD_LL) failed, error=%lu", GetLastError());
        } else if (!want && g_moveKeyboardHook) {
            UnhookWindowsHookEx(g_moveKeyboardHook);
            g_moveKeyboardHook = nullptr;
        }
    };
    syncKeyboardHook();

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == WM_APP_KEYMOVE_SYNC) {
            syncKeyboardHook();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Low-level hooks have to be removed by the thread that installed them,
    // which is why this happens here rather than in UninitInputHooks.
    if (desktopSwitch) UnhookWinEvent(desktopSwitch);
    if (g_dragMouseHook) {
        UnhookWindowsHookEx(g_dragMouseHook);
        g_dragMouseHook = nullptr;
    }
    if (g_moveKeyboardHook) {
        UnhookWindowsHookEx(g_moveKeyboardHook);
        g_moveKeyboardHook = nullptr;
    }
    return 0;
}

// Settings changed: let the input thread add or remove the keyboard hook
// (a low-level hook has to be set and removed by the thread that owns it).
void VizSyncKeyboardHook() {
    if (g_inputHookThreadId) PostThreadMessage(g_inputHookThreadId, WM_APP_KEYMOVE_SYNC, 0, 0);
}

void InitInputHooks() {
    if (g_inputHookThread) return;
    g_inputHookThread = CreateThread(nullptr, 0, InputHookThreadProc, nullptr, 0,
                                     &g_inputHookThreadId);
    if (g_inputHookThread) {
        SetThreadDescription(g_inputHookThread, L"TourneTable-InputHooks");
    }
}

void UninitInputHooks() {
    if (g_inputHookThread) {
        PostThreadMessage(g_inputHookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_inputHookThread, 3000);
        CloseHandle(g_inputHookThread);
        g_inputHookThread = nullptr;
        g_inputHookThreadId = 0;
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
    ttgfx::PresentBlank();
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
    g_npArtistBrush.Reset();
    g_progressBrush.Reset();
    g_textPanelBrush.Reset();
    // Smooth Mode caches. Every settings change and wallpaper re-bake comes
    // through here, so none of them needs its own invalidation for colors,
    // fonts or blur -- only for geometry, which is checked per frame.
    g_plateBitmap.Reset();
    g_plateRect = D2D1::RectF(0, 0, 0, 0);
    g_spriteAtlas.Reset();
    g_spriteAtlasKey = VizAtlasKey();
    g_npLayoutCache.Reset();
    g_pfLayoutCache.Reset();
    ttgfx::OnSettingsChanged();
}

void ReleaseSwapChainResources() {
    ReleaseVisualResources();
    ttgfx::ReleaseSurface();
    g_spriteBatch.Reset();
    g_dc3.Reset();
    g_compositionVisual.Reset();
    g_split = {};
    g_splitTop.Reset();
    g_rootVisual.Reset();
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

    D2D1_COLOR_F peakHoldColor = D2D1::ColorF(g_settings.peakHoldR / 255.0f, g_settings.peakHoldG / 255.0f,
                                               g_settings.peakHoldB / 255.0f, g_settings.peakHoldA / 255.0f);
    g_dc->CreateSolidColorBrush(peakHoldColor, &g_barBrush2);

    if (g_settings.backgroundEnabled) {
        D2D1_COLOR_F bgColor = D2D1::ColorF(g_settings.bgR / 255.0f, g_settings.bgG / 255.0f,
                                            g_settings.bgB / 255.0f, g_settings.bgA / 255.0f);
        g_dc->CreateSolidColorBrush(bgColor, &g_backgroundBrush);

        if (g_settings.bgBlur > 0) {
            CaptureWallpaperBitmap();
            if (g_wallpaperBitmap) {
                hr = g_dc->CreateEffect(kCLSID_D2D1GaussianBlur, &g_blurEffect);
                if (SUCCEEDED(hr)) {
                    g_blurEffect->SetInput(0, g_wallpaperBitmap.Get());
                    g_blurEffect->SetValue(0, (FLOAT)g_settings.bgBlur);
                    g_blurEffect->SetValue(2, (UINT32)1);

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

    if (g_settings.progressEnabled) g_dc->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &g_progressBrush);

    if (g_settings.nowPlayingEnabled || g_settings.peakFreqEnabled) {
        // One scratch brush shared by both text panels and both their borders.
        // Each use sets its colour first -- four separate brushes would be four
        // objects to keep in step with four settings for no benefit, since the
        // draws are strictly sequential.
        g_dc->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &g_textPanelBrush);

        D2D1_COLOR_F npColor =
            D2D1::ColorF(g_settings.nowPlayingR / 255.0f, g_settings.nowPlayingG / 255.0f,
                        g_settings.nowPlayingB / 255.0f, g_settings.nowPlayingA / 255.0f);
        g_dc->CreateSolidColorBrush(npColor, &g_nowPlayingBrush);
        g_dc->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &g_npArtistBrush);

        float fontSize = std::max(6.f, g_settings.nowPlayingFontSize);
        if (g_dwriteFactory && (!g_dwriteTextFormat || g_dwriteTextFormatFontSize != fontSize ||
                                 g_dwriteTextFormatFontName != g_settings.nowPlayingFont)) {
            g_dwriteTextFormat.Reset();
            WCHAR localeName[LOCALE_NAME_MAX_LENGTH];
            if (GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0) {
                wcscpy_s(localeName, L"en-us");
            }
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

// The colours and geometry of one text overlay's panel, bundled so the two
// callers don't each pass a dozen loose arguments.
struct TextPanelStyle {
    BYTE fillA, fillR, fillG, fillB;
    BYTE borderA, borderR, borderG, borderB;
    float padding, cornerRadius, borderSize;
};

// Draws a text overlay, optionally on a panel fitted to the text.
//
// `box` is the layout rectangle the text is centred within, which is much wider
// than the text itself -- so the panel can't just use it, or it would stretch
// the full width of the reserved area. Measuring the text means building a text
// layout, which is exactly what DrawText does internally anyway; when a panel is
// wanted we keep that layout and draw from it, so nothing is measured twice.
//
// fadeAlpha scales the whole thing, so a panel fades in and out with the Now
// Playing text instead of popping.
void DrawOverlayText(PCWSTR text, UINT32 len, const D2D1_RECT_F& box, ID2D1Brush* textBrush,
                     const TextPanelStyle& style, float fadeAlpha,
                     VizTextLayoutCache* cache = nullptr, bool noWrap = false) {
    bool wantFill = style.fillA > 0;
    bool wantBorder = style.borderSize > 0 && style.borderA > 0;

    ComPtr<IDWriteTextLayout> layout;

    // Smooth Mode passes a cache: the layout is rebuilt only when the string or
    // the box changes, and then serves both the panel measurement and the
    // draw. Drawing a layout at the box origin is what DrawText does
    // internally, so the result is identical.
    if (cache && g_dwriteFactory && g_dwriteTextFormat) {
        float boxW = box.right - box.left;
        float boxH = box.bottom - box.top;
        bool same = cache->layout && cache->w == boxW && cache->h == boxH &&
                    cache->text.size() == len && wmemcmp(cache->text.data(), text, len) == 0;
        if (!same) {
            cache->Reset();
            if (SUCCEEDED(g_dwriteFactory->CreateTextLayout(text, len, g_dwriteTextFormat.Get(), boxW,
                                                            boxH, &cache->layout)) &&
                cache->layout) {
                // A readout line is never broken over two lines: the box is
                // only one line tall, so a wrapped second line would be lost.
                if (noWrap) cache->layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                cache->text.assign(text, len);
                cache->w = boxW;
                cache->h = boxH;
            } else {
                cache->Reset();
            }
        }
        layout = cache->layout;
    }

    if ((wantFill || wantBorder) && g_dwriteFactory && g_textPanelBrush) {
        float boxW = box.right - box.left;
        float boxH = box.bottom - box.top;
        DWRITE_TEXT_METRICS m{};
        if ((layout || SUCCEEDED(g_dwriteFactory->CreateTextLayout(
                           text, len, g_dwriteTextFormat.Get(), boxW, boxH, &layout))) &&
            layout && SUCCEEDED(layout->GetMetrics(&m)) && m.width > 0.f && m.height > 0.f) {
            float pad = (float)style.padding * g_dpiScale;
            D2D1_RECT_F r = D2D1::RectF(box.left + m.left - pad, box.top + m.top - pad,
                                        box.left + m.left + m.width + pad,
                                        box.top + m.top + m.height + pad);

            float maxRadius = std::min((r.right - r.left) * 0.5f, (r.bottom - r.top) * 0.5f);
            float rad = std::min((float)style.cornerRadius * g_dpiScale, maxRadius);

            if (wantFill) {
                g_textPanelBrush->SetColor(D2D1::ColorF(
                    style.fillR / 255.0f, style.fillG / 255.0f, style.fillB / 255.0f,
                    (style.fillA / 255.0f) * fadeAlpha));
                g_dc->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad),
                                           g_textPanelBrush.Get());
            }

            if (wantBorder) {
                // DrawRoundedRectangle strokes centred on the path, so the rect
                // is inset by half the stroke to keep the whole border inside
                // the panel rather than straddling its edge.
                float bw = std::min((float)style.borderSize * g_dpiScale, maxRadius);
                D2D1_RECT_F sr = D2D1::RectF(r.left + bw * 0.5f, r.top + bw * 0.5f,
                                             r.right - bw * 0.5f, r.bottom - bw * 0.5f);
                float srad = std::max(0.0f, rad - bw * 0.5f);
                g_textPanelBrush->SetColor(D2D1::ColorF(
                    style.borderR / 255.0f, style.borderG / 255.0f, style.borderB / 255.0f,
                    (style.borderA / 255.0f) * fadeAlpha));
                g_dc->DrawRoundedRectangle(D2D1::RoundedRect(sr, srad, srad),
                                           g_textPanelBrush.Get(), bw);
            }
        }
    }

    if (layout) {
        g_dc->DrawTextLayout(D2D1::Point2F(box.left, box.top), layout.Get(), textBrush);
    } else {
        g_dc->DrawText(text, len, g_dwriteTextFormat.Get(), box, textBrush);
    }
}

bool RectsApproxEqual(const D2D1_RECT_F& a, const D2D1_RECT_F& b) {
    auto eq = [](float x, float y) { return fabsf(x - y) < 0.01f; };
    return eq(a.left, b.left) && eq(a.top, b.top) &&
           eq(a.right, b.right) && eq(a.bottom, b.bottom);
}

// ---- Smooth Mode: background plate --------------------------------------------

// The three background layers, drawn into any context. The 1.4 path calls this
// on the frame's own context every frame; Smooth Mode calls it once into the
// cached plate.
void VizDrawBackgroundLayers(ID2D1DeviceContext* dc, ID2D1Geometry* bgGeo, ID2D1Geometry* ring,
                             ID2D1Brush* fill, ID2D1Brush* border) {
    if (g_blurredBitmap && bgGeo) {
        dc->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), bgGeo), nullptr);
        // The cached blur is already box-sized and box-aligned, so this
        // is a straight 1:1 blit with no scaling or resampling.
        dc->DrawBitmap(g_blurredBitmap.Get());
        dc->PopLayer();
    }
    if (bgGeo && fill) dc->FillGeometry(bgGeo, fill);
    if (ring && border) dc->FillGeometry(ring, border);
}

// Renders the background into g_plateBitmap. The plate covers the panel's
// pixel-aligned bounds plus a 2 px margin for the antialiased edge, and is
// drawn back at an integer position, so the per-frame blit never resamples.
//
// It renders on a context of its own rather than by retargeting the frame's
// context, because it can be called mid-frame, possibly inside the auto-hide
// fade layer, where switching targets isn't allowed.
bool VizBakeBackgroundPlate(const D2D1_RECT_F& bgRect, ID2D1Geometry* bgGeo, ID2D1Geometry* ring) {
    if (!g_d2dDevice || !g_backgroundBrush) return false;

    D2D1_RECT_F pr = D2D1::RectF(floorf(bgRect.left) - 2.f, floorf(bgRect.top) - 2.f,
                                 ceilf(bgRect.right) + 2.f, ceilf(bgRect.bottom) + 2.f);
    UINT w = (UINT)std::max(1.f, pr.right - pr.left);
    UINT h = (UINT)std::max(1.f, pr.bottom - pr.top);

    ComPtr<ID2D1DeviceContext> bake;
    if (FAILED(g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &bake))) return false;

    // Same size as last time (a drag, say) reuses the bitmap.
    ComPtr<ID2D1Bitmap1> bmp = g_plateBitmap;
    if (!bmp || bmp->GetPixelSize().width != w || bmp->GetPixelSize().height != h) {
        D2D1_BITMAP_PROPERTIES1 props = {};
        props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
        bmp.Reset();
        if (FAILED(bake->CreateBitmap(D2D1::SizeU(w, h), nullptr, 0, props, &bmp))) return false;
    }

    // Brushes are made on the bake context with the frame brushes' colors,
    // which RecreateVisualResources set from the settings.
    ComPtr<ID2D1SolidColorBrush> fill, border;
    bake->CreateSolidColorBrush(g_backgroundBrush->GetColor(), &fill);
    if (g_borderBrush) bake->CreateSolidColorBrush(g_borderBrush->GetColor(), &border);

    bake->SetTarget(bmp.Get());
    bake->BeginDraw();
    bake->Clear(D2D1::ColorF(0, 0, 0, 0));
    bake->SetTransform(D2D1::Matrix3x2F::Translation(-pr.left, -pr.top));
    VizDrawBackgroundLayers(bake.Get(), bgGeo, ring, fill.Get(), border.Get());
    HRESULT hr = bake->EndDraw();
    bake->SetTarget(nullptr);
    if (FAILED(hr)) {
        g_plateBitmap.Reset();
        return false;
    }
    g_plateBitmap = bmp;
    g_plateRect = pr;
    return true;
}

// ---- Smooth Mode: batched bars --------------------------------------------------
//
// The atlas holds one antialiased bar shape, white, with a transparent pixel of
// padding all round. For bars it is stored as three strips along the bar's
// length: the leading cap (padding, the rounded corners, one solid row), a
// three-row solid middle, and the trailing cap. Each bar is then three sprites:
// the caps drawn 1:1 at the bar's ends and one middle row stretched to fill the
// rest, all in one draw call with per-bar color.
//
// Two details keep it looking like the 1.4 path rather than a sprite approxi-
// mation. Each cap strip carries one solid row past the corner, so even a
// square-cornered bar has its end edge inside the texture, where bilinear
// filtering gives it a subpixel-accurate antialiased edge as it moves. And the
// shape is baked at the bars' common subpixel offset across the bar, so when
// bar positions share it (whole-pixel bar width and gap, which is every
// 100% / 200% scale setup) the sprites land on whole pixels and the sides come
// out exactly as D2D's own antialiasing draws them.

bool VizEnsureSpriteAtlas(const VizAtlasKey& key) {
    if (!g_dc3 || !g_spriteBatch || !g_d2dDevice || !g_d2dFactory) return false;
    if (g_spriteAtlas && key == g_spriteAtlasKey) return true;
    g_spriteAtlas.Reset();

    const float t = std::max(1.0f, key.thick);
    const float maxR = t * 0.5f;
    const float rTL = std::min(key.rTL, maxR), rTR = std::min(key.rTR, maxR);
    const float rBR = std::min(key.rBR, maxR), rBL = std::min(key.rBL, maxR);

    VizAtlasLayout L;
    L.pad = 1;
    L.mid = 3;
    D2D1_RECT_F shape;
    UINT bmpW, bmpH;
    if (key.kind == 1) {
        // A dot is drawn whole: one sprite each, no stretching.
        L.capA = L.capB = 0;
        L.across = L.along = (UINT)ceilf(t) + 2 * L.pad;
        shape = D2D1::RectF((float)L.pad, (float)L.pad, L.pad + t, L.pad + t);
        bmpW = bmpH = L.across;
    } else if (key.horizontal) {
        // Upright bars: caps are top (TL/TR) and bottom (BL/BR).
        L.capA = (UINT)ceilf(std::max(rTL, rTR));
        L.capB = (UINT)ceilf(std::max(rBL, rBR));
        L.across = (UINT)ceilf(t + key.frac) + 2 * L.pad;
        L.along = L.pad + L.capA + L.mid + L.capB + L.pad;
        shape = D2D1::RectF(L.pad + key.frac, (float)L.pad, L.pad + key.frac + t,
                            (float)(L.along - L.pad));
        bmpW = L.across;
        bmpH = L.along;
    } else {
        // Sideways bars: caps are left (TL/BL) and right (TR/BR).
        L.capA = (UINT)ceilf(std::max(rTL, rBL));
        L.capB = (UINT)ceilf(std::max(rTR, rBR));
        L.across = (UINT)ceilf(t + key.frac) + 2 * L.pad;
        L.along = L.pad + L.capA + L.mid + L.capB + L.pad;
        shape = D2D1::RectF((float)L.pad, L.pad + key.frac, (float)(L.along - L.pad),
                            L.pad + key.frac + t);
        bmpW = L.along;
        bmpH = L.across;
    }

    ComPtr<ID2D1DeviceContext> bake;
    if (FAILED(g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &bake))) return false;
    D2D1_BITMAP_PROPERTIES1 props = {};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
    ComPtr<ID2D1Bitmap1> atlas;
    if (FAILED(bake->CreateBitmap(D2D1::SizeU(bmpW, bmpH), nullptr, 0, props, &atlas))) return false;
    ComPtr<ID2D1SolidColorBrush> white;
    if (FAILED(bake->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f), &white))) return false;

    bake->SetTarget(atlas.Get());
    bake->BeginDraw();
    bake->Clear(D2D1::ColorF(0, 0, 0, 0));
    // The same routine the 1.4 path draws each bar with, so corner shapes and
    // radius clamping match exactly.
    FillRoundedRectPerCorner(bake.Get(), g_d2dFactory.Get(), shape, white.Get(), rTL, rTR, rBR, rBL);
    HRESULT hr = bake->EndDraw();
    bake->SetTarget(nullptr);
    if (FAILED(hr)) return false;

    g_spriteAtlas = atlas;
    g_spriteAtlasKey = key;
    g_spriteAtlasLayout = L;
    return true;
}

inline D2D1_COLOR_F VizPremultiplied(BYTE a, BYTE r, BYTE g, BYTE b) {
    // Sprite colors multiply the (premultiplied) atlas texel component by
    // component, so they have to be premultiplied too for a translucent bar
    // color to blend correctly.
    float fa = a / 255.0f;
    return D2D1::ColorF(r / 255.0f * fa, g / 255.0f * fa, b / 255.0f * fa, fa);
}

inline void VizClearSprites() {
    g_spriteDst.clear();
    g_spriteSrc.clear();
    g_spriteCol.clear();
}

inline void VizPushSprite(const D2D1_RECT_F& dst, const D2D1_RECT_U& src, const D2D1_COLOR_F& col) {
    g_spriteDst.push_back(dst);
    g_spriteSrc.push_back(src);
    g_spriteCol.push_back(col);
}

// Queues the three sprites for one bar whose rectangle is r -- the same
// rectangle the 1.4 path would fill. `horizontal` is the Orientation setting
// (upright bars in a horizontal row).
void VizQueueBarSprites(const D2D1_RECT_F& r, bool horizontal, const D2D1_COLOR_F& col) {
    const VizAtlasLayout& L = g_spriteAtlasLayout;
    const float frac = g_spriteAtlasKey.frac;
    const float pad = (float)L.pad;
    // Effective cap lengths include the one solid row each strip carries.
    const float ca = (float)L.capA + 1.f;
    const float cb = (float)L.capB + 1.f;

    float a0 = horizontal ? r.top : r.left;     // start along the bar's length
    float a1 = horizontal ? r.bottom : r.right; // end along the bar's length
    float len = a1 - a0;
    if (len <= 0.01f) return;
    float s = (ca + cb > len) ? len / (ca + cb) : 1.f;

    float c0 = (horizontal ? r.left : r.top) - frac - pad;  // atlas column/row 0
    float c1 = c0 + (float)L.across;

    UINT leadEnd = L.pad + L.capA + 1;  // exclusive
    UINT bodyRow = leadEnd;             // the middle of the three solid rows
    UINT trailStart = bodyRow + 1;

    auto dstRect = [&](float along0, float along1) {
        return horizontal ? D2D1::RectF(c0, along0, c1, along1) : D2D1::RectF(along0, c0, along1, c1);
    };
    auto srcRect = [&](UINT along0, UINT along1) {
        D2D1_RECT_U u;
        if (horizontal) {
            u.left = 0; u.right = L.across; u.top = along0; u.bottom = along1;
        } else {
            u.left = along0; u.right = along1; u.top = 0; u.bottom = L.across;
        }
        return u;
    };

    VizPushSprite(dstRect(a0 - pad * s, a0 + ca * s), srcRect(0, leadEnd), col);
    float bodyFrom = a0 + ca * s, bodyTo = a1 - cb * s;
    if (bodyTo - bodyFrom > 0.001f) VizPushSprite(dstRect(bodyFrom, bodyTo), srcRect(bodyRow, bodyRow + 1), col);
    VizPushSprite(dstRect(a1 - cb * s, a1 + pad * s), srcRect(trailStart, L.along), col);
}

// One dot centred on (cx, cy).
void VizQueueDotSprite(float cx, float cy, float dotR, const D2D1_COLOR_F& col) {
    const VizAtlasLayout& L = g_spriteAtlasLayout;
    float x0 = cx - dotR - (float)L.pad, y0 = cy - dotR - (float)L.pad;
    D2D1_RECT_U src = {0, 0, L.across, L.along};
    VizPushSprite(D2D1::RectF(x0, y0, x0 + (float)L.across, y0 + (float)L.along), src, col);
}

// Draws everything queued, in one call.
void VizFlushSprites() {
    UINT32 n = (UINT32)g_spriteDst.size();
    if (!n || !g_dc3 || !g_spriteBatch || !g_spriteAtlas) return;
    g_spriteBatch->Clear();
    if (FAILED(g_spriteBatch->AddSprites(n, g_spriteDst.data(), g_spriteSrc.data(), g_spriteCol.data(),
                                         nullptr, sizeof(D2D1_RECT_F), sizeof(D2D1_RECT_U),
                                         sizeof(D2D1_COLOR_F), 0))) {
        return;
    }
    // Sprite batches only draw in aliased mode. The atlas already carries the
    // antialiasing, and bilinear sampling carries it across subpixel positions.
    D2D1_ANTIALIAS_MODE prev = g_dc->GetAntialiasMode();
    g_dc->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    g_dc3->DrawSpriteBatch(g_spriteBatch.Get(), 0, n, g_spriteAtlas.Get(),
                           D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1_SPRITE_OPTIONS_NONE);
    g_dc->SetAntialiasMode(prev);
}

// ---- Direct3D 11 renderer (Renderer = Direct3D 11) -----------------------------------
//
// Why a second renderer, when Smooth Mode already batched the bars. Direct2D
// tessellates on the CPU and validates and saves Direct3D state around every
// batch; for a few hundred moving shapes that per-call CPU work, not the
// GPU, is what the 1.5 measurements kept pointing at. This renderer draws
// every bar, dot, cap, spoke and scope segment as an instanced quad from one
// tiny vertex shader that reads the bar heights straight from a buffer: the
// CPU writes about 16 KB per frame (two floats per bar) and issues a handful
// of draw calls, whatever the bar count. The pixel shader computes exact
// antialiased coverage analytically, so edges look like Direct2D's.
//
// Three further savings come from how the frame reaches the screen:
//
//   * Skip unchanged frames. A hash of everything that decides the picture
//     (bar heights to a quarter pixel, colours, the trace) is compared with
//     the last frame presented; if nothing moved, nothing is drawn or
//     presented, so DWM has nothing to recompose. During steady or quiet
//     passages and every idle state that is most frames.
//   * Two surfaces instead of one. The animated swap chain covers only the
//     panel, not the room reserved around it for text; the text lives on the
//     old full-size surface, which is redrawn only when the text or its fade
//     changes. DWM's per-frame work scales with the area that changes.
//   * Opaque panel. When the panel can't show anything behind it (Blur on, or
//     a fully opaque colour), its swap chain is created with alpha ignored and
//     the rounded corners come from a DirectComposition clip. DWM then copies
//     instead of blending, which Chromium measured as 3.12 W against 2.28 W.
//
// The same shaders also carry the GPU Workload: four compute passes run the
// whole precision analysis on the GPU and write the bar buffer the vertex
// shader reads, so nothing comes back to the CPU but an 8-byte stats block,
// read two frames late without waiting.
namespace ttgfx {

static const char kShaderSource[] =
R"HLSL(#define TT_CBUFFER(n, r) cbuffer n : register(r)
#define TT_CBUFFER_END
#define REG(r) : register(r)
#define SEM(s) : s
#define NUMTHREADS(x, y, z) [numthreads(x, y, z)]
#define GROUPSHARED groupshared
#define NOINTERP nointerpolation
#define BARRIER GroupMemoryBarrierWithGroupSync()
#define OUT(T) out T
// ---- Constant buffers --------------------------------------------------------
// Mirrored field for field by VizGfxFrameCB / VizGfxPassCB / VizGfxCsCB on the
// C++ side; every row is 16 bytes so HLSL packing and C++ layout agree.

TT_CBUFFER(FrameCB, b0) {
    float2 fViewport;  // swap chain size, px
    float2 fBlock;     // bar group top-left, px
    float fBarW, fBarGap, fMaxSize, fIdleSize;
    float4 fRadii;     // bar corners TL TR BR BL, px
    float4 fDotRadii;  // dot corners TL TR BR BL, px
    uint fBarCount, fShape, fVertical, fAnchor;     // anchor: 0 top, 1 middle, 2 bottom
    uint fColorMode, fFlags, fMaxDots, fDotSlots;  // flags: 1 peak caps, 2 beat flash, 4 multiband scope
    float4 fC1, fGrad1, fC2, fPeakColor, fBeatColor;  // straight alpha, 0..1
    float fBeatIntensity, fRainbowBase, fSceneAlpha, fCapThickness;
    float fDotStep, fDotR, fInnerR, fStrokeW;
    float2 fCenter;
    float fAmpScale, fSweepLen;
    float fScopeCenter, fSweepOrigin, fWStep, fGonioR;
    float4 fPlateRect;
    float4 fScopeColor;
    float fGonioDot, fCorrY, fCorrH, fCorr;
    // Terminal shape: palette (dim, low, high, label, peak), grid origin and
    // cell size in px, grid and glyph-atlas dimensions.
    float4 fTermColors[5];
    float4 fTermGeom;   // origin x, origin y, cell width, cell height
    float4 fTermAtlas;  // atlas width, atlas height, -, -
    uint fTermCols, fTermRows, fTermAtlasCols, fTermPad;
    // Styles (2.1): which one (0 none, 1 LED, 2 Line, 3 Bloom, 4 Spectrogram,
    // 5 VU, 6 Stereo Field, 7 Particles), LED segments per bar, Line
    // subdivisions per bar gap, Spectrogram history rows in use.
    uint fStyle, fSegs, fSubdiv, fSpecRows;
    // Reflection: base line y, direction (+1 down), depth px, start opacity.
    float fReflBase, fReflDir, fReflDepth, fReflAlpha;
    // LED segment pitch and height, Line glow radius and fill opacity.
    float fSegStep, fSegH, fGlowR, fFillA;
    float4 fVu;     // VU needle L, R (0..1 of the scale), peak LED L, R (0..1)
    float4 fVuBox;  // one meter's width, height, offset of the second meter x, y
    uint fSpecW, fSpecHead, fSpecTex, fSpecPad;  // bars per row, newest row, rows in the texture
    // FX (2.1): Glow strength 0..1 and radius px, Bloom strength 0..1 and
    // radius px; texel size of the quarter-res bloom target and of the scene.
    float fFxGlow, fFxGlowR, fFxBloom, fFxBloomR;
    float4 fFxTexel;
    // Outline and Shadow (FX, 2.1): outline colour (straight alpha), shadow
    // colour (alpha = strength), then outline width, shadow offset x, y and
    // softness, all px.
    float4 fFxLineColor;
    float4 fFxShadowColor;
    float fFxLineW, fFxShadowX, fFxShadowY, fFxShadowSoft;
    // Bar modifiers (2.1): dash and gap length px, tilt (shear per px of
    // height), the base line it leans from; Afterimage opacity, Hollow line
    // width px, Mirror gap px (Middle anchor), flags: 1 hollow, 2 dashed,
    // 4 tilted, 8 mirror gap.
    float fBarDash, fBarDashGap, fBarTiltK, fBarPivot;
    float fGhostA, fHollowW, fMirrorGap;
    uint fModFlags;
}
TT_CBUFFER_END

TT_CBUFFER(PassCB, b1) {
    uint pPass, pCount, pPad0, pPad1;
}
TT_CBUFFER_END

TT_CBUFFER(CsCB, b2) {
    uint cN, cHalf, cLog2Half, cNumBands;
    uint cNumBars, cTierCount, cTierList, cDetector;  // tier list: 2 bits per dispatched group;
                                                     // tier count: bits 8-10 = tiers in use
    float cFloorDb, cRangeDb, cSensDb, cDt;
    float cAttackMs, cReleaseMs, cReleaseDbPerSec, cAutoGainMaxDb;
    uint cReleaseLinear, cCurve, cShape, cAutoGain;
    float cPeakHoldMs, cGravity, cLinFall, cBarRangePx;
    uint cPeakGravity, cDomOn, cPad1, cPad2;
    float cTierRate0, cTierRate1, cTierRate2, cPeakCal;
    float cFmin, cFmax, cBreatheUp, cBreatheDown;  // breathe env taus, seconds
}
TT_CBUFFER_END
// ---- Resources -----------------------------------------------------------------

// Drawing (vertex / pixel shaders).
StructuredBuffer<float2> gBars REG(t0);     // per bar: level 0..1, peak cap 0..1
StructuredBuffer<float4> gGlobals REG(t1);  // [0] beat pulse, master peak, breathe env, -
                                            // [1] EQ-zone energy low, mid, high, -
StructuredBuffer<float> gWave REG(t2);      // oscilloscope trace, 256 samples, -1..1
StructuredBuffer<float4> gPoints REG(t3);   // goniometer: side, mid, alpha, -
Texture2D<float4> gPlate REG(t4);           // baked background panel, premultiplied
SamplerState gSamp REG(s0);
StructuredBuffer<uint> gCells REG(t10);     // Terminal: glyph cells only, char | colour << 8 | grid index << 16
Texture2D<float4> gGlyphs REG(t11);         // Terminal: printable ASCII baked white, premultiplied
Texture2D<float> gSpec REG(t12);            // Spectrogram: bar levels, one row per 1/60 s, ring
Texture2D<float4> gFxSrc REG(t13);          // FX: the texture a bloom pass reads
Texture2D<float4> gFxBloom REG(t14);        // FX: the blurred bloom, for the composite
SamplerState gLin REG(s1);                  // FX: bilinear, clamped

// Analysis (compute shaders, Workload = GPU).
StructuredBuffer<float> gTierIn REG(t5);     // 3 x N newest samples, one block per tier
StructuredBuffer<float> gWindow REG(t6);     // N window coefficients
StructuredBuffer<float4> gBandDesc REG(t7);  // per band: tier + 4 * EQ zone, k0, k1, centre Hz
StructuredBuffer<float4> gBandOff REG(t8);   // per band: RMS offset dB, peak offset dB, w0, w1
StructuredBuffer<float> gShapeMod REG(t9);   // per bar: wave / breathe modulation from the CPU
RWStructuredBuffer<float> gPower REG(u0);    // 3 x (N/2 + 1) power spectra
RWStructuredBuffer<float4> gBandState REG(u1);  // per band: level, raw dB, fast, slow
RWStructuredBuffer<float2> gBarsOut REG(u2);    // = gBars
RWStructuredBuffer<float4> gBarAux REG(u3);     // per bar: peak, hold timer, fall velocity, -
RWStructuredBuffer<float4> gGlobalsOut REG(u4); // = gGlobals, plus [2] auto gain state
RWStructuredBuffer<uint> gStats REG(u5);        // [0] dominant Hz (float bits), [1] max delta px (bits)

#define TT_PI 3.14159265358979f

// ---- Shared helpers ------------------------------------------------------------

float4 Hsv(float h, float s, float v, float a) {
    h = h - 360.0f * floor(h / 360.0f);
    float c = v * s;
    float hp = h / 60.0f;
    float x = c * (1.0f - abs(hp - 2.0f * floor(hp / 2.0f) - 1.0f));
    float m = v - c;
    float r = 0.0f, g = 0.0f, b = 0.0f;
    if (h < 60.0f) { r = c; g = x; }
    else if (h < 120.0f) { r = x; g = c; }
    else if (h < 180.0f) { g = c; b = x; }
    else if (h < 240.0f) { g = x; b = c; }
    else if (h < 300.0f) { r = x; b = c; }
    else { r = c; b = x; }
    return float4(r + m, g + m, b + m, a);
}

float4 BeatFlash(float4 col) {
    if ((fFlags & 2u) == 0u) return col;
    float pulse = gGlobals[0].x;
    if (pulse <= 0.001f) return col;
    float blend = min(1.0f, pulse * fBeatIntensity * fBeatColor.w);
    return float4(col.x + (fBeatColor.x - col.x) * blend, col.y + (fBeatColor.y - col.y) * blend,
                  col.z + (fBeatColor.z - col.z) * blend, col.w);
}

// The Color Mode rules, exactly as the Direct2D path applies them per bar.
// Modes: 0 solid, 1 gradient, 2 reactive, 3 accent, 4 album, 5 dynamic album,
// 6 acrylic, 7 rainbow, 8 Tourne. Accent / album / Tourne arrive already
// resolved into fC1 / fGrad1 / fC2.
float4 BarColor(uint i, float fac, bool radial) {
    uint n = fBarCount;
    float t = (n > 1u) ? (float)i / (float)(n - 1u) : 0.0f;
    float4 col = fC1;
    uint m = fColorMode;
    if (m == 1u || m == 8u) col = lerp(fGrad1, fC2, t);
    else if (m == 2u) col = lerp(fGrad1, fC2, fac);
    else if (m == 5u && !radial) col = lerp(fGrad1, fC2, min(1.0f, t * 0.6f + fac * 0.4f));
    else if (m == 7u) {
        float tr = radial ? (float)i / (float)max(n, 1u) : t;
        col = Hsv(fRainbowBase + tr * 360.0f, 0.85f, 1.0f, fC1.w);
    }
    if (m == 6u) col = float4(fC1.x, fC1.y, fC1.z, min(180.0f, floor(180.0f * fac)) / 255.0f);
    return BeatFlash(col);
}

float4 ScopeColor() {
    float4 col = fScopeColor;
    if ((fFlags & 4u) != 0u) {
        float4 z = gGlobals[1];
        float total = z.x + z.y + z.z;
        if (total > 0.001f) {
            col = float4((1.0f * z.x + 0.4706f * z.y + 0.3529f * z.z) / total,
                         (0.3529f * z.x + 0.8627f * z.y + 0.7059f * z.z) / total,
                         (0.2353f * z.x + 0.3529f * z.y + 1.0f * z.z) / total, 1.0f);
        } else {
            col = float4(0.4706f, 0.8627f, 0.3529f, 1.0f);
        }
    }
    return BeatFlash(col);
}

float4 Premul(float4 c) { return float4(c.x * c.w, c.y * c.w, c.z * c.w, c.w); }

// ---- Drawing -------------------------------------------------------------------
//
// Every primitive is one instanced quad. The vertex shader works out where it
// goes from the bar state and the frame constants (the same geometry rules as
// the Direct2D path), the pixel shader computes exact antialiased coverage from
// a signed distance: saturate(0.5 - d) is the true area coverage for an
// axis-aligned edge at any subpixel position, and within a hair of it for the
// rounded corners and the capsules.

struct Prim {
    uint kind;    // 0 rounded rect, 1 capsule, 2 textured plate, 3 skip, 4 glyph
    float4 a;     // rect: left top right bottom / capsule: ax ay bx by
    float4 radii; // rect: TL TR BR BL / capsule: x = radius
    float4 color; // straight alpha
};

Prim NoPrim() {
    Prim p;
    p.kind = 3u;
    p.a = float4(0.0f, 0.0f, 0.0f, 0.0f);
    p.radii = float4(0.0f, 0.0f, 0.0f, 0.0f);
    p.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return p;
}

Prim RectPrim(float l, float t, float r, float b, float4 radii, float4 color) {
    Prim p;
    p.kind = 0u;
    p.a = float4(l, t, r, b);
    p.radii = radii;
    p.color = color;
    if (r - l <= 0.0f || b - t <= 0.0f) p.kind = 3u;
    return p;
}

Prim CapsulePrim(float ax, float ay, float bx, float by, float radius, float4 color) {
    Prim p;
    p.kind = 1u;
    p.a = float4(ax, ay, bx, by);
    p.radii = float4(radius, 0.0f, 0.0f, 0.0f);
    p.color = color;
    return p;
}

float BarRange() { return max(0.0f, fMaxSize - fIdleSize); }

Prim BarPrimAt(uint i, float fac, float alpha) {
    float size = fIdleSize + fac * BarRange();
    float lead = (float)i * (fBarW + fBarGap);
    float4 col = BarColor(i, fac, false);
    col.w = col.w * alpha;
    if (fVertical == 0u) {
        float x = fBlock.x + lead;
        float y0, y1;
        if (fAnchor == 0u) { y0 = fBlock.y; y1 = fBlock.y + size; }
        else if (fAnchor == 1u) { y0 = fBlock.y + (fMaxSize - size) * 0.5f; y1 = y0 + size; }
        else { y0 = fBlock.y + (fMaxSize - size); y1 = fBlock.y + fMaxSize; }
        return RectPrim(x, y0, x + fBarW, y1, fRadii, col);
    }
    float y = fBlock.y + lead;
    float x0, x1;
    if (fAnchor == 0u) { x1 = fBlock.x + fMaxSize; x0 = x1 - size; }
    else if (fAnchor == 1u) { float cx = fBlock.x + fMaxSize * 0.5f; x0 = cx - size * 0.5f; x1 = cx + size * 0.5f; }
    else { x0 = fBlock.x; x1 = fBlock.x + size; }
    return RectPrim(x0, y, x1, y + fBarW, fRadii, col);
}

Prim BarPrim(uint i) {
    if (i >= fBarCount) return NoPrim();
    return BarPrimAt(i, max(0.0f, gBars[i].x), 1.0f);
}

// Afterimage (2.1): the bar again at its slowly falling trail level (carried
// where the peak hold usually is), see-through, drawn before the bar.
Prim GhostPrim(uint i) {
    if (i >= fBarCount || fGhostA <= 0.0f) return NoPrim();
    return BarPrimAt(i, max(0.0f, gBars[i].y), fGhostA);
}


Prim CapPrim(uint i) {
    if (i >= fBarCount || (fFlags & 1u) == 0u) return NoPrim();
    float hold = fIdleSize + gBars[i].y * BarRange();
    if (hold <= 0.5f) return NoPrim();
    float lead = (float)i * (fBarW + fBarGap);
    float h = fCapThickness * 0.5f;
    float4 z = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (fVertical == 0u) {
        float x = fBlock.x + lead;
        float cy;
        if (fAnchor == 0u) cy = fBlock.y + hold;
        else if (fAnchor == 1u) cy = fBlock.y + (fMaxSize - hold) * 0.5f;
        else cy = fBlock.y + (fMaxSize - hold);
        return RectPrim(x, cy - h, x + fBarW, cy + h, z, fPeakColor);
    }
    float y = fBlock.y + lead;
    float cx;
    if (fAnchor == 0u) cx = fBlock.x + fMaxSize - hold;
    else if (fAnchor == 1u) cx = fBlock.x + fMaxSize * 0.5f - hold * 0.5f;
    else cx = fBlock.x + hold;
    return RectPrim(cx - h, y, cx + h, y + fBarW, z, fPeakColor);
}

// One dot. Slots per bar: Middle anchor uses 2 * maxDots + 1 (centre, then
// alternating up / down pairs), the other anchors maxDots.
Prim DotPrim(uint id) {
    uint slots = max(fDotSlots, 1u);
    uint i = id / slots;
    uint s = id - i * slots;
    if (i >= fBarCount) return NoPrim();
    float fac = max(0.0f, gBars[i].x);
    float colSize = fIdleSize + fac * BarRange();
    if (colSize < 0.5f) return NoPrim();
    int numDots = (fDotStep > 0.5f) ? (int)(colSize / fDotStep) : 1;
    numDots = max(1, numDots);
    uint d = s;
    bool second = false;  // Middle: the downward (or leftward) dot of a pair
    if (fAnchor == 1u && s > 0u) { d = (s + 1u) / 2u; second = (s & 1u) == 0u; }
    if ((int)d >= numDots) return NoPrim();
    float r = fDotR;
    float lead = (float)i * fDotStep + r;
    float fd = (float)d * fDotStep;
    float cx, cy;
    if (fVertical == 0u) {
        cx = fBlock.x + lead;
        if (fAnchor == 0u) {
            cy = fBlock.y + fd + r;
            if (cy - r > fBlock.y + fMaxSize) return NoPrim();
        } else if (fAnchor == 1u) {
            float c = fBlock.y + fMaxSize * 0.5f;
            if (d == 0u) cy = c;
            else if (!second) { cy = c - fd; if (cy - r < fBlock.y) return NoPrim(); }
            else { cy = c + fd; if (cy + r > fBlock.y + fMaxSize) return NoPrim(); }
        } else {
            cy = fBlock.y + fMaxSize - fd - r;
            if (cy + r < fBlock.y) return NoPrim();
        }
    } else {
        cy = fBlock.y + lead;
        if (fAnchor == 0u) {
            cx = fBlock.x + fMaxSize - fd - r;
            if (cx + r < fBlock.x) return NoPrim();
        } else if (fAnchor == 1u) {
            float c = fBlock.x + fMaxSize * 0.5f;
            if (d == 0u) cx = c;
            else if (!second) { cx = c + fd; if (cx + r > fBlock.x + fMaxSize) return NoPrim(); }
            else { cx = c - fd; if (cx - r < fBlock.x) return NoPrim(); }
        } else {
            cx = fBlock.x + fd + r;
            if (cx - r > fBlock.x + fMaxSize) return NoPrim();
        }
    }
    return RectPrim(cx - r, cy - r, cx + r, cy + r, fDotRadii, BarColor(i, fac, false));
}

Prim RadialPrim(uint i) {
    if (i >= fBarCount) return NoPrim();
    float fac = max(0.0f, gBars[i].x);
    float len = fIdleSize + fac * BarRange();
    if (len < 0.5f) return NoPrim();
    float ang = (float)i / (float)fBarCount * 2.0f * TT_PI - TT_PI * 0.5f;
    float dx = cos(ang), dy = sin(ang);
    return CapsulePrim(fCenter.x + dx * fInnerR, fCenter.y + dy * fInnerR,
                       fCenter.x + dx * (fInnerR + len), fCenter.y + dy * (fInnerR + len),
                       fStrokeW * 0.5f, BarColor(i, fac, true));
}

float2 ScopePoint(uint w) {
    float along = fSweepOrigin + (float)w * fWStep;
    float across = fScopeCenter + gWave[w] * fAmpScale * ((fVertical == 0u) ? -1.0f : 1.0f);
    return (fVertical == 0u) ? float2(along, across) : float2(across, along);
}

Prim ScopePrim(uint w) {
    if (w >= 255u) return NoPrim();
    float2 a = ScopePoint(w);
    float2 b = ScopePoint(w + 1u);
    return CapsulePrim(a.x, a.y, b.x, b.y, max(1.0f, fBarW * 0.5f) * 0.5f, ScopeColor());
}

Prim GonioPrim(uint j) {
    float4 p = gPoints[j];
    if (p.z <= 0.002f) return NoPrim();
    float x = fCenter.x + clamp(p.x, -1.0f, 1.0f) * fGonioR;
    float y = fCenter.y - clamp(p.y, -1.0f, 1.0f) * fGonioR;
    float4 col = ScopeColor();
    col.w = col.w * p.z;
    float r = fGonioDot;
    return RectPrim(x - r, y - r, x + r, y + r, float4(r, r, r, r), col);
}

Prim CorrPrim(uint j) {
    float halfW = fGonioR;
    float y0 = fCorrY, y1 = fCorrY + fCorrH;
    float4 col = ScopeColor();
    float rr = fCorrH * 0.5f;
    if (j == 0u) {
        col.w = col.w * 0.25f;
        return RectPrim(fCenter.x - halfW, y0, fCenter.x + halfW, y1, float4(rr, rr, rr, rr), col);
    }
    float c = fCorr;
    float x = fCenter.x + clamp(c, -1.0f, 1.0f) * halfW;
    float hw = max(1.5f, fCorrH * 0.5f);
    return RectPrim(x - hw, y0 - 1.0f, x + hw, y1 + 1.0f, float4(hw, hw, hw, hw), col);
}

// One terminal cell: a glyph from the atlas (printable ASCII from 32, laid
// out fTermAtlasCols to a row, one cell each), in one of five palette
// colours. Cells are whole pixels and drawn 1:1, so pixel fonts stay sharp.
// Only cells with a glyph are uploaded, each as char | colour << 8 |
// grid index << 16, so the instance count is the number of glyphs.
Prim TermPrim(uint id) {
    uint cols = max(fTermCols, 1u);
    uint cell = gCells[id];
    uint ch = cell & 255u;  // 33-126 ASCII, 128-255 the custom glyphs (2.1)
    uint idx = cell >> 16u;
    if (ch <= 32u || idx >= cols * fTermRows) return NoPrim();
    uint ci = min((cell >> 8u) & 7u, 4u);
    uint col = idx % cols;
    uint row = idx / cols;
    float cw = fTermGeom.z, chh = fTermGeom.w;
    float x0 = fTermGeom.x + (float)col * cw;
    float y0 = fTermGeom.y + (float)row * chh;
    uint g = ch - 32u;
    uint ac = max(fTermAtlasCols, 1u);
    float gx = (float)(g % ac), gy = (float)(g / ac);
    Prim p;
    p.kind = 4u;
    p.a = float4(x0, y0, x0 + cw, y0 + chh);
    p.radii = float4(gx * cw / fTermAtlas.x, gy * chh / fTermAtlas.y, (gx + 1.0f) * cw / fTermAtlas.x,
                     (gy + 1.0f) * chh / fTermAtlas.y);
    p.color = fTermColors[ci];
    return p;
}

// ---- Styles (2.1) ----------------------------------------------------------------
//
// Each style is one more pass over the same instanced quad. Bars, peak caps
// and the frame constants are shared with the shapes above, so a style costs
// one draw call and no new per-frame upload unless it has data of its own
// (Spectrogram rows, Stereo Field levels, Particles).

// The part of bar i that lies between distances a and b from its base edge.
// Middle anchor is drawn from the bottom (or left) edge here: segmented and
// filled styles read as meters, and a meter grows from one end.
float4 SpanRect(uint i, float a, float b) {
    float lead = (float)i * (fBarW + fBarGap);
    if (fVertical == 0u) {
        float x = fBlock.x + lead;
        if (fAnchor == 0u) return float4(x, fBlock.y + a, x + fBarW, fBlock.y + b);
        float base = fBlock.y + fMaxSize;
        return float4(x, base - b, x + fBarW, base - a);
    }
    float y = fBlock.y + lead;
    if (fAnchor == 0u) {
        float base = fBlock.x + fMaxSize;
        return float4(base - b, y, base - a, y + fBarW);
    }
    return float4(fBlock.x + a, y, fBlock.x + b, y + fBarW);
}

Prim RectPrimV(float4 r, float4 radii, float4 color) { return RectPrim(r.x, r.y, r.z, r.w, radii, color); }

// LED Meter: green to 60 %, amber to 85 %, red above, like a hardware meter.
// Unlit segments stay faintly visible; the peak-hold segment stays lit.
float4 LedZone(float t) {
    if (t >= 0.85f) return float4(1.0f, 0.23f, 0.19f, 1.0f);
    if (t >= 0.6f) return float4(1.0f, 0.69f, 0.0f, 1.0f);
    return float4(0.22f, 0.89f, 0.42f, 1.0f);
}

Prim LedPrim(uint id) {
    uint segs = max(fSegs, 1u);
    uint i = id / segs;
    uint s = id % segs;
    if (i >= fBarCount) return NoPrim();
    float fac = max(0.0f, gBars[i].x);
    float lit = (fIdleSize + fac * BarRange()) / max(fSegStep, 1.0f);
    float4 col = (fColorMode == 0u) ? BeatFlash(LedZone(((float)s + 0.5f) / (float)segs)) : BarColor(i, fac, false);
    bool on = (float)s + 0.5f <= lit;
    if (!on && (fFlags & 1u) != 0u) {
        float hold = fIdleSize + gBars[i].y * BarRange();
        on = hold > 0.5f && (uint)min(floor(hold / max(fSegStep, 1.0f)), (float)(segs - 1u)) == s;
    }
    if (!on) col.w = col.w * 0.1f;
    float a = (float)s * fSegStep;
    return RectPrimV(SpanRect(i, a, a + fSegH), fRadii, col);
}

// Line Spectrum: a Catmull-Rom curve through the bar tops, filled down to the
// base and lit along its edge. Each instance is one column between two
// curve points (kind 5); columns tile exactly in x, so the translucent fill
// and glow are never blended twice where they meet.
float LineSign() {
    if (fVertical == 0u) return (fAnchor == 0u) ? 1.0f : -1.0f;
    return (fAnchor == 0u) ? -1.0f : 1.0f;
}
float LineBase() {
    if (fVertical == 0u) return (fAnchor == 0u) ? fBlock.y : fBlock.y + fMaxSize;
    return (fAnchor == 0u) ? fBlock.x + fMaxSize : fBlock.x;
}
float LineVal(int i) {
    int n = (int)fBarCount;
    i = clamp(i, 0, n - 1);
    return fIdleSize + max(0.0f, gBars[(uint)i].x) * BarRange();
}
float LineAlong(float k) {
    return ((fVertical == 0u) ? fBlock.x : fBlock.y) + k * (fBarW + fBarGap) + fBarW * 0.5f;
}
float CatRom(float p0, float p1, float p2, float p3, float t) {
    float t2 = t * t, t3 = t2 * t;
    return 0.5f * (2.0f * p1 + (p2 - p0) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                   (3.0f * p1 - p0 - 3.0f * p2 + p3) * t3);
}

Prim LinePrim(uint id) {
    uint sub = max(fSubdiv, 1u);
    uint seg = id / sub;
    uint j = id % sub;
    if (fBarCount < 2u || seg + 1u >= fBarCount) return NoPrim();
    int k = (int)seg;
    float p0 = LineVal(k - 1), p1 = LineVal(k), p2 = LineVal(k + 1), p3 = LineVal(k + 2);
    float t0 = (float)j / (float)sub, t1 = (float)(j + 1u) / (float)sub;
    float v0 = clamp(CatRom(p0, p1, p2, p3, t0), 0.0f, fMaxSize);
    float v1 = clamp(CatRom(p0, p1, p2, p3, t1), 0.0f, fMaxSize);
    float u0 = LineAlong((float)seg + t0), u1 = LineAlong((float)seg + t1);
    // The first and last columns reach the ends of the bar row.
    float start = (fVertical == 0u) ? fBlock.x : fBlock.y;
    if (seg == 0u && j == 0u) u0 = start;
    if (seg + 2u == fBarCount && j + 1u == sub) u1 = start + (float)fBarCount * (fBarW + fBarGap) - fBarGap;
    float base = LineBase(), sg = LineSign();
    Prim p;
    p.kind = 5u;
    p.a = float4(u0, base + sg * v0, u1, base + sg * v1);
    p.radii = float4(base, max(1.5f, fBarW * 0.2f), fGlowR, fFillA);
    p.color = BarColor(seg + ((t0 >= 0.5f) ? 1u : 0u), max(0.0f, gBars[seg].x), false);
    return p;
}

// Polar Bloom: the Radial bars joined into one filled flower. One wedge per
// bar (kind 6) from the centre to this bar's tip and the next one's. The
// two straight sides are shared with the neighbours and tested with the
// same expression from both sides, so every pixel lands in exactly one
// wedge; only the outer edge is antialiased.
float Cross2(float ax, float ay, float bx, float by) { return ax * by - ay * bx; }

Prim BloomPrim(uint i) {
    uint n = fBarCount;
    if (n < 3u || i >= n) return NoPrim();
    uint j = (i + 1u) % n;
    float li = fIdleSize + max(0.0f, gBars[i].x) * BarRange();
    float lj = fIdleSize + max(0.0f, gBars[j].x) * BarRange();
    float ai = (float)i / (float)n * 2.0f * TT_PI - TT_PI * 0.5f;
    float aj = (float)j / (float)n * 2.0f * TT_PI - TT_PI * 0.5f;
    float ri = fInnerR + li, rj = fInnerR + lj;
    Prim p;
    p.kind = 6u;
    p.a = float4(fCenter.x + cos(ai) * ri, fCenter.y + sin(ai) * ri, fCenter.x + cos(aj) * rj,
                 fCenter.y + sin(aj) * rj);
    p.radii = float4(max(1.0f, fBarW * 0.35f), 0.0f, 0.0f, 0.0f);
    p.color = BarColor(i, max(0.0f, gBars[i].x), true);
    return p;
}

// Spectrogram: one quad (kind 7) reading a history texture, a row of bar
// levels per 1/60 s, newest at the top (left when vertical), plus a colour
// legend with quarter ticks beside it.
float4 SpecColor(float v) {
    v = saturate(v);
    float4 c;
    if (fColorMode != 0u) {
        c = lerp(fGrad1, fC2, v);
    } else if (v < 0.25f) {
        c = lerp(float4(0.0f, 0.0f, 0.0f, 1.0f), float4(0.25f, 0.02f, 0.45f, 1.0f), v * 4.0f);
    } else if (v < 0.5f) {
        c = lerp(float4(0.25f, 0.02f, 0.45f, 1.0f), float4(0.85f, 0.15f, 0.35f, 1.0f), (v - 0.25f) * 4.0f);
    } else if (v < 0.75f) {
        c = lerp(float4(0.85f, 0.15f, 0.35f, 1.0f), float4(1.0f, 0.6f, 0.1f, 1.0f), (v - 0.5f) * 4.0f);
    } else {
        c = lerp(float4(1.0f, 0.6f, 0.1f, 1.0f), float4(1.0f, 1.0f, 0.85f, 1.0f), (v - 0.75f) * 4.0f);
    }
    c.w = saturate(v * 2.5f) * fC1.w;
    return c;
}

Prim SpecPrim(uint id) {
    if (id > 1u || fSpecW == 0u) return NoPrim();
    float thick = (float)fBarCount * (fBarW + fBarGap) - fBarGap;
    float w = (fVertical == 0u) ? thick : fMaxSize;
    float h = (fVertical == 0u) ? fMaxSize : thick;
    Prim p;
    p.kind = 7u;
    p.radii = float4((float)id, 0.0f, 0.0f, 0.0f);
    p.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (id == 0u) {
        p.a = float4(fBlock.x, fBlock.y, fBlock.x + w, fBlock.y + h);
    } else if (fVertical == 0u) {
        float x = fBlock.x + w + fVuBox.x;
        p.a = float4(x, fBlock.y, x + fVuBox.y, fBlock.y + h);
    } else {
        float y = fBlock.y + h + fVuBox.x;
        p.a = float4(fBlock.x, y, fBlock.x + w, y + fVuBox.y);
    }
    return p;
}

// VU Needles: two analog meters (L, R) built from rects and capsules, 32
// instances each: face, 11 scale marks, a 16-piece arc (red from 0 VU),
// needle, pivot and peak LED. The needle positions come from the CPU with
// real VU ballistics (99 % in 300 ms, 1.5 % overshoot).
float VuPos(float db) { return (pow(10.0f, db / 20.0f) - 0.1f) / (1.41254f - 0.1f); }
float VuMark(uint k) {
    if (k == 0u) return -20.0f;
    if (k == 1u) return -10.0f;
    if (k == 2u) return -7.0f;
    if (k == 3u) return -5.0f;
    if (k == 4u) return -3.0f;
    if (k == 5u) return -2.0f;
    if (k == 6u) return -1.0f;
    if (k == 7u) return 0.0f;
    if (k == 8u) return 1.0f;
    if (k == 9u) return 2.0f;
    return 3.0f;
}

Prim VuPrim(uint id) {
    uint m = id / 32u, k = id % 32u;
    if (m > 1u) return NoPrim();
    float ox = fBlock.x + (float)m * fVuBox.z, oy = fBlock.y + (float)m * fVuBox.w;
    float w = fVuBox.x, h = fVuBox.y;
    float pvx = ox + w * 0.5f, pvy = oy + h * 0.9f;
    float R = h * 0.68f;
    float lw = max(0.6f, h * 0.008f);
    float4 ink = float4(0.92f, 0.92f, 0.9f, 0.85f);
    float4 red = float4(1.0f, 0.27f, 0.23f, 0.95f);
    float zero = VuPos(0.0f);
    if (k == 0u) {
        float rr = h * 0.08f;
        return RectPrim(ox, oy, ox + w, oy + h, float4(rr, rr, rr, rr), float4(0.05f, 0.05f, 0.06f, 0.6f));
    }
    if (k <= 11u) {
        float db = VuMark(k - 1u);
        float an = (-48.0f + 96.0f * VuPos(db)) * TT_PI / 180.0f;
        float dx = sin(an), dy = -cos(an);
        float r0 = (k == 1u || k == 8u) ? R * 0.84f : R * 0.9f;
        return CapsulePrim(pvx + dx * r0, pvy + dy * r0, pvx + dx * R, pvy + dy * R, lw, db > 0.0f ? red : ink);
    }
    if (k <= 27u) {
        float t0 = (float)(k - 12u) / 16.0f, t1 = (float)(k - 11u) / 16.0f;
        float a0 = (-48.0f + 96.0f * t0) * TT_PI / 180.0f, a1 = (-48.0f + 96.0f * t1) * TT_PI / 180.0f;
        bool hot = (t0 + t1) * 0.5f > zero;
        float ra = R * 0.9f;
        return CapsulePrim(pvx + sin(a0) * ra, pvy - cos(a0) * ra, pvx + sin(a1) * ra, pvy - cos(a1) * ra,
                           hot ? lw * 1.8f : lw, hot ? red : ink);
    }
    float pos = (m == 0u) ? fVu.x : fVu.y;
    if (k == 28u) {
        float an = (-48.0f + 96.0f * pos) * TT_PI / 180.0f;
        float dx = sin(an), dy = -cos(an);
        float4 col = BeatFlash(fC1);
        col.w = 1.0f;
        return CapsulePrim(pvx + dx * R * 0.12f, pvy + dy * R * 0.12f, pvx + dx * R * 1.02f, pvy + dy * R * 1.02f,
                           max(0.8f, h * 0.012f), col);
    }
    if (k == 29u) {
        float r = h * 0.05f;
        return RectPrim(pvx - r, pvy - r, pvx + r, pvy + r, float4(r, r, r, r), float4(0.25f, 0.25f, 0.27f, 1.0f));
    }
    if (k == 30u) {
        float led = (m == 0u) ? fVu.z : fVu.w;
        float r = h * 0.045f;
        float cx = ox + w - h * 0.12f, cy = oy + h * 0.12f;
        return RectPrim(cx - r, cy - r, cx + r, cy + r, float4(r, r, r, r),
                        float4(1.0f, 0.18f, 0.12f, lerp(0.18f, 1.0f, saturate(led))));
    }
    return NoPrim();
}

// Stereo Field: left channel grows up from the centre line, right channel
// down (left and right when vertical). gBars holds L in x and R in y.
Prim SplitPrim(uint id) {
    uint i = id >> 1u, ch = id & 1u;
    if (i >= fBarCount) return NoPrim();
    float lev = max(0.0f, (ch == 0u) ? gBars[i].x : gBars[i].y);
    float s = (fIdleSize + lev * BarRange()) * 0.5f;
    if (s < 0.25f) return NoPrim();
    float lead = (float)i * (fBarW + fBarGap);
    float4 col = BarColor(i, lev, false);
    if (fVertical == 0u) {
        float x = fBlock.x + lead, cy = fBlock.y + fMaxSize * 0.5f;
        if (ch == 0u) return RectPrim(x, cy - 0.5f - s, x + fBarW, cy - 0.5f, fRadii, col);
        return RectPrim(x, cy + 0.5f, x + fBarW, cy + 0.5f + s, fRadii, col);
    }
    float y = fBlock.y + lead, cx = fBlock.x + fMaxSize * 0.5f;
    if (ch == 0u) return RectPrim(cx - 0.5f - s, y, cx - 0.5f, y + fBarW, fRadii, col);
    return RectPrim(cx + 0.5f, y, cx + 0.5f + s, y + fBarW, fRadii, col);
}

// Particles: sparks thrown off the bar tops on each beat, simulated on the
// CPU and uploaded as (x, y, alpha, bar * 16 + radius) relative to the block.
Prim SparkPrim(uint j) {
    float4 p = gPoints[j];
    if (p.z <= 0.002f) return NoPrim();
    float band = floor(p.w / 16.0f);
    float r = p.w - band * 16.0f;
    float x = fBlock.x + p.x, y = fBlock.y + p.y;
    float4 col = BarColor(min((uint)band, max(fBarCount, 1u) - 1u), 1.0f, false);
    col.w = col.w * saturate(p.z);
    return RectPrim(x - r, y - r, x + r, y + r, float4(r, r, r, r), col);
}

// Reflection: the pass mirrored about the base line, faded out over
// fReflDepth in the pixel shader (flag 16 on the kind).
Prim ReflectPrim(Prim p) {
    float B2 = 2.0f * fReflBase;
    uint kb = p.kind & 15u;  // the glow flag may be set
    if (kb == 0u) {
        p.a = float4(p.a.x, B2 - p.a.w, p.a.z, B2 - p.a.y);
        p.radii = float4(p.radii.w, p.radii.z, p.radii.y, p.radii.x);
    } else if (kb == 5u) {
        p.a = float4(p.a.x, B2 - p.a.y, p.a.z, B2 - p.a.w);
        p.radii.x = B2 - p.radii.x;
    } else {
        return NoPrim();
    }
    p.color.w = p.color.w * fReflAlpha;
    p.kind = p.kind | 16u;
    return p;
}

Prim BuildPrim(uint passId, uint id) {
    if (passId == 0u) {
        Prim p = RectPrim(fPlateRect.x, fPlateRect.y, fPlateRect.z, fPlateRect.w,
                          float4(0.0f, 0.0f, 0.0f, 0.0f), float4(1.0f, 1.0f, 1.0f, 1.0f));
        if (p.kind == 0u) p.kind = 2u;
        return p;
    }
    if (passId == 1u) return BarPrim(id);
    if (passId == 2u) return CapPrim(id);
    if (passId == 3u) return DotPrim(id);
    if (passId == 4u) return RadialPrim(id);
    if (passId == 5u) return ScopePrim(id);
    if (passId == 6u) return GonioPrim(id);
    if (passId == 7u) return CorrPrim(id);
    if (passId == 8u) return TermPrim(id);
    if (passId == 9u) return LedPrim(id);
    if (passId == 10u) return LinePrim(id);
    if (passId == 11u) return BloomPrim(id);
    if (passId == 12u) return SpecPrim(id);
    if (passId == 13u) return VuPrim(id);
    if (passId == 14u) return SplitPrim(id);
    if (passId == 15u) return SparkPrim(id);
    if (passId == 16u) return GhostPrim(id);
    return NoPrim();
}

struct VsOut {
    float4 pos SEM(SV_Position);
    float2 pix SEM(TEXCOORD0);
    NOINTERP float4 shape SEM(TEXCOORD1);
    NOINTERP float4 radii SEM(TEXCOORD2);
    NOINTERP float4 color SEM(TEXCOORD3);
    NOINTERP uint kind SEM(TEXCOORD4);  // low 4 bits: Prim kind; 16: reflected; 32: FX; 64: dash/hollow; 128: tilt
};

// Quad corner `vid` (triangle strip 0..3) of the primitive's bounds. The
// bounds reach one pixel past the shape so its antialiased edge is covered;
// the plate is drawn exactly 1:1.
VsOut EmitVertex(Prim p, uint vid) {
    VsOut o;
    float2 lo, hi;
    uint kb = p.kind & 15u;
    if (kb == 5u) {
        float c0 = min(min(p.a.y, p.a.w), p.radii.x) - p.radii.z * 2.0f - 1.0f;
        float c1 = max(max(p.a.y, p.a.w), p.radii.x) + p.radii.z * 2.0f + 1.0f;
        lo = (fVertical == 0u) ? float2(p.a.x - 1.0f, c0) : float2(c0, p.a.x - 1.0f);
        hi = (fVertical == 0u) ? float2(p.a.z + 1.0f, c1) : float2(c1, p.a.z + 1.0f);
    } else if (kb == 6u) {
        lo = float2(min(fCenter.x, min(p.a.x, p.a.z)) - 1.5f, min(fCenter.y, min(p.a.y, p.a.w)) - 1.5f);
        hi = float2(max(fCenter.x, max(p.a.x, p.a.z)) + 1.5f, max(fCenter.y, max(p.a.y, p.a.w)) + 1.5f);
    } else if (kb == 1u) {
        float r = p.radii.x + 1.0f;
        lo = float2(min(p.a.x, p.a.z) - r, min(p.a.y, p.a.w) - r);
        hi = float2(max(p.a.x, p.a.z) + r, max(p.a.y, p.a.w) + r);
    } else if (kb == 2u || kb == 4u || kb == 7u) {
        lo = float2(p.a.x, p.a.y);
        hi = float2(p.a.z, p.a.w);
    } else {
        lo = float2(p.a.x - 1.0f, p.a.y - 1.0f);
        hi = float2(p.a.z + 1.0f, p.a.w + 1.0f);
    }
    if ((p.kind & 128u) != 0u) {  // Tilt: the sheared shape's reach
        if (fVertical == 0u) {
            float d0 = fBarTiltK * (fBarPivot - lo.y), d1 = fBarTiltK * (fBarPivot - hi.y);
            lo.x = lo.x + min(min(d0, d1), 0.0f) - 1.0f;
            hi.x = hi.x + max(max(d0, d1), 0.0f) + 1.0f;
        } else {
            float d0 = fBarTiltK * (lo.x - fBarPivot), d1 = fBarTiltK * (hi.x - fBarPivot);
            lo.y = lo.y + min(min(d0, d1), 0.0f) - 1.0f;
            hi.y = hi.y + max(max(d0, d1), 0.0f) + 1.0f;
        }
    }
    if ((p.kind & 32u) != 0u) {  // room for the glow and the shadow
        float gr = max(fFxGlow > 0.0f ? fFxGlowR * 2.0f : 0.0f,
                       fFxShadowColor.w > 0.0f ? fFxShadowSoft + max(abs(fFxShadowX), abs(fFxShadowY)) + 1.0f : 0.0f);
        lo = float2(lo.x - gr, lo.y - gr);
        hi = float2(hi.x + gr, hi.y + gr);
    }
    float cx = ((vid & 1u) != 0u) ? 1.0f : 0.0f;
    float cy = ((vid & 2u) != 0u) ? 1.0f : 0.0f;
    float2 pix = float2(lo.x + (hi.x - lo.x) * cx, lo.y + (hi.y - lo.y) * cy);
    o.pix = pix;
    o.pos = float4(pix.x / fViewport.x * 2.0f - 1.0f, 1.0f - pix.y / fViewport.y * 2.0f, 0.0f, 1.0f);
    if (p.kind == 3u) o.pos = float4(-2.0f, -2.0f, 0.0f, 1.0f);  // all four corners equal: no area
    o.shape = p.a;
    o.radii = p.radii;
    o.color = Premul(p.color);
    o.kind = p.kind;
    return o;
}

VsOut VSMain(uint vid SEM(SV_VertexID), uint iid SEM(SV_InstanceID)) {
    Prim p = BuildPrim(pPass, iid);
    // Glow: passes created with pPad1 = 1, rects and capsules only.
    if (pPad1 != 0u && (fFxGlow > 0.0f || fFxLineW > 0.0f || fFxShadowColor.w > 0.0f) && (p.kind == 0u || p.kind == 1u))
        p.kind = p.kind | 32u;
    // Bar modifiers: the bars and their Afterimage; the caps tilt with them.
    bool barPass = pPass == 1u || pPass == 16u;
    if (barPass && (fModFlags & 11u) != 0u && (p.kind & 15u) <= 1u)
        p.kind = p.kind | 64u;
    if ((barPass || pPass == 2u) && (fModFlags & 4u) != 0u && (p.kind & 15u) <= 1u)
        p.kind = p.kind | 128u;
    if (pPad0 != 0u) p = ReflectPrim(p);
    return EmitVertex(p, vid);
}

float4 LineShade(VsOut i) {
    float u = (fVertical == 0u) ? i.pix.x : i.pix.y;
    float c = (fVertical == 0u) ? i.pix.y : i.pix.x;
    float4 a = i.shape;
    float base = i.radii.x;
    float xcov = saturate(min(u + 0.5f, a.z) - max(u - 0.5f, a.x));
    float k = (a.w - a.y) / max(a.z - a.x, 1e-4f);
    float cl = a.y + k * (u - a.x);
    float sb = (base >= cl) ? 1.0f : -1.0f;
    float d = (c - cl) / sqrt(1.0f + k * k);  // perpendicular distance to the curve
    float dd = d * sb;                         // positive toward the base
    float depth = max(abs(base - cl), 1.0f);
    float fill = saturate(dd + 0.5f) * saturate((base - c) * sb + 0.5f) * i.radii.w *
                 lerp(1.0f, 0.15f, saturate(dd / depth));
    float edge = saturate(i.radii.y * 0.5f + 0.5f - abs(d));
    float glow = 0.0f;
    if (i.radii.z > 0.0f) glow = 0.4f * exp(-(d * d) / (i.radii.z * i.radii.z * 0.5f));
    float alpha = 1.0f - (1.0f - edge) * (1.0f - fill) * (1.0f - glow);
    return i.color * (alpha * xcov);
}

float4 BloomShade(VsOut i) {
    float px = i.pix.x - fCenter.x, py = i.pix.y - fCenter.y;
    float dix = i.shape.x - fCenter.x, diy = i.shape.y - fCenter.y;
    float djx = i.shape.z - fCenter.x, djy = i.shape.w - fCenter.y;
    if (!(Cross2(dix, diy, px, py) >= 0.0f && Cross2(djx, djy, px, py) < 0.0f)) return float4(0.0f, 0.0f, 0.0f, 0.0f);
    float ex = djx - dix, ey = djy - diy;
    float el = max(sqrt(ex * ex + ey * ey), 1e-4f);
    float dOut = Cross2(ex, ey, px - dix, py - diy) / el;  // distance inside the outer edge
    float cov = saturate(dOut + 0.5f);
    float r = sqrt(px * px + py * py) / max(sqrt(dix * dix + diy * diy), 1.0f);
    float fill = lerp(0.2f, 0.75f, saturate(r));
    float rim = saturate(i.radii.x + 0.5f - dOut);
    return i.color * (max(fill, rim) * cov);
}

float4 SpecShade(VsOut i) {
    float rx = saturate((i.pix.x - i.shape.x) / max(1.0f, i.shape.z - i.shape.x));
    float ry = saturate((i.pix.y - i.shape.y) / max(1.0f, i.shape.w - i.shape.y));
    if (i.radii.x > 0.5f) {  // legend: hot end at the top (right when vertical)
        float v = (fVertical == 0u) ? 1.0f - ry : rx;
        float len = (fVertical == 0u) ? i.shape.w - i.shape.y : i.shape.z - i.shape.x;
        float q = v * 4.0f;
        bool tick = abs(q - floor(q + 0.5f)) * len * 0.25f < 0.5f;
        float4 c = SpecColor(v);
        c.w = max(c.w, 0.35f);
        if (tick) c = float4(1.0f, 1.0f, 1.0f, 0.9f);
        return Premul(c);
    }
    float along = (fVertical == 0u) ? rx : ry;
    float age = (fVertical == 0u) ? ry : rx;
    uint w = max(fSpecW, 1u), rows = max(fSpecRows, 1u), tex = max(fSpecTex, 1u);
    uint bar = min((uint)(along * (float)w), w - 1u);
    uint ago = min((uint)(age * (float)rows), rows - 1u);
    uint row = (fSpecHead + tex - ago) % tex;
    return Premul(SpecColor(gSpec.Load(int3((int)bar, (int)row, 0))));
}

// Coverage of the pixel centred at `pix` by a rectangle with a separate radius
// per corner (TL TR BR BL, y down), each clamped to half the shorter side as
// Direct2D does.
//
// The straight edges use the exact overlap of the pixel with the rectangle on
// each axis, multiplied: that is the true area for any rectangle at any
// subpixel position, including bars thinner than a pixel and square corners
// (where a distance field would give min(x, y) instead of x * y, up to a
// quarter of a pixel wrong). Inside a rounded corner's square the arc takes
// over, from the distance to the corner circle.
float RectCoverage(float2 pix, float4 rect, float4 radii) {
    float ox = saturate(min(pix.x + 0.5f, rect.z) - max(pix.x - 0.5f, rect.x));
    float oy = saturate(min(pix.y + 0.5f, rect.w) - max(pix.y - 0.5f, rect.y));
    float cov = ox * oy;
    float2 c = float2((rect.x + rect.z) * 0.5f, (rect.y + rect.w) * 0.5f);
    float2 h = float2((rect.z - rect.x) * 0.5f, (rect.w - rect.y) * 0.5f);
    float2 p = float2(pix.x - c.x, pix.y - c.y);
    float r = (p.x > 0.0f) ? ((p.y > 0.0f) ? radii.z : radii.y) : ((p.y > 0.0f) ? radii.w : radii.x);
    r = clamp(r, 0.0f, min(h.x, h.y));
    float qx = abs(p.x) - h.x + r;
    float qy = abs(p.y) - h.y + r;
    if (r > 0.0f && qx > 0.0f && qy > 0.0f) {
        float d = length(float2(qx, qy)) - r;
        cov = min(cov, saturate(0.5f - d));
    }
    return cov;
}

float SdCapsule(float2 pix, float4 seg, float radius) {
    float2 pa = float2(pix.x - seg.x, pix.y - seg.y);
    float2 ba = float2(seg.z - seg.x, seg.w - seg.y);
    float bb = dot(ba, ba);
    float h = (bb > 1e-8f) ? saturate(dot(pa, ba) / bb) : 0.0f;
    return length(float2(pa.x - ba.x * h, pa.y - ba.y * h)) - radius;
}

// Glow (FX, 2.1): signed distance to a rounded rect (negative inside), so
// the glow can fall off with the distance outside the shape.
float SdRoundRect(float2 pix, float4 rect, float4 radii) {
    float cx = (rect.x + rect.z) * 0.5f, cy = (rect.y + rect.w) * 0.5f;
    float hx = (rect.z - rect.x) * 0.5f, hy = (rect.w - rect.y) * 0.5f;
    float px = pix.x - cx, py = pix.y - cy;
    float r = (px > 0.0f) ? ((py > 0.0f) ? radii.z : radii.y) : ((py > 0.0f) ? radii.w : radii.x);
    r = clamp(r, 0.0f, min(hx, hy));
    float qx = abs(px) - hx + r, qy = abs(py) - hy + r;
    return length(float2(max(qx, 0.0f), max(qy, 0.0f))) + min(max(qx, qy), 0.0f) - r;
}

float ShapeCoverage(VsOut i, uint kb) {
    if (kb == 1u) return saturate(0.5f - SdCapsule(i.pix, i.shape, i.radii.x));
    return RectCoverage(i.pix, i.shape, i.radii);
}

// The bar's own pixels, with Hollow (only a band just inside the edge) and
// Dashed (gaps along the growth direction, fixed to the base line so the
// dashes stay still while the bar grows) applied.
float Coverage(VsOut i, uint kb) {
    float cov = ShapeCoverage(i, kb);
    if ((i.kind & 64u) == 0u) return cov;
    if ((fModFlags & 1u) != 0u) {
        float d = (kb == 1u) ? SdCapsule(i.pix, i.shape, i.radii.x) : SdRoundRect(i.pix, i.shape, i.radii);
        float w = max(fHollowW, 0.25f);
        cov = min(cov, saturate(0.5f - (abs(d + w * 0.5f) - w * 0.5f)));
    }
    if ((fModFlags & 2u) != 0u) {
        float period = max(fBarDash + fBarDashGap, 1.0f);
        float t = (fVertical == 0u) ? abs(i.pix.y - fBarPivot) : abs(i.pix.x - fBarPivot);
        float u = t - floor(t / period) * period;
        float m = saturate(fBarDash - u + 0.5f) * saturate(u + 0.5f) + saturate(u - period + 0.5f);
        cov = cov * saturate(m);
    }
    if ((fModFlags & 8u) != 0u) {  // Mirror: the two halves pulled apart from the centre line
        float t = (fVertical == 0u) ? abs(i.pix.y - fBarPivot) : abs(i.pix.x - fBarPivot);
        cov = cov * saturate(t - fMirrorGap * 0.5f + 0.5f);
    }
    return cov;
}

// Tilt: the pixel back in the unsheared bar's space.
float2 Untilt(float2 pix) {
    if (fVertical == 0u) return float2(pix.x - fBarTiltK * (fBarPivot - pix.y), pix.y);
    return float2(pix.x, pix.y - fBarTiltK * (pix.x - fBarPivot));
}


// Glow, Outline and Shadow together, from one signed distance (and one more
// for the shadow's offset copy). Outline is a band just inside the edge, so
// it never changes a bar's size; the shadow sits behind the bar's own pixels.
float4 FxShade(VsOut i, uint kb) {
    float d = (kb == 1u) ? SdCapsule(i.pix, i.shape, i.radii.x) : SdRoundRect(i.pix, i.shape, i.radii);
    float cov = Coverage(i, kb);
    float4 col = i.color * cov;
    if (fFxLineW > 0.0f) {
        float o = saturate(0.5f - (abs(d + fFxLineW * 0.5f) - fFxLineW * 0.5f)) * fFxLineColor.w * i.color.w;
        col = col * (1.0f - o) + float4(fFxLineColor.x, fFxLineColor.y, fFxLineColor.z, 1.0f) * o;
    }
    if (fFxGlow > 0.0f && d > 0.0f) {
        float g = exp(-(d * d) / max(fFxGlowR * fFxGlowR * 0.5f, 0.01f));
        col = col + i.color * ((1.0f - cov) * fFxGlow * 0.65f * g);
    }
    if (fFxShadowColor.w > 0.0f) {
        float2 sp = float2(i.pix.x - fFxShadowX, i.pix.y - fFxShadowY);
        float ds = (kb == 1u) ? SdCapsule(sp, i.shape, i.radii.x) : SdRoundRect(sp, i.shape, i.radii);
        float s = saturate((fFxShadowSoft * 0.5f + 0.5f - ds) / (fFxShadowSoft + 1.0f)) * fFxShadowColor.w * i.color.w;
        col = col + float4(fFxShadowColor.x, fFxShadowColor.y, fFxShadowColor.z, 1.0f) * (s * (1.0f - col.w));
    }
    return col;
}

float4 PSMain(VsOut iIn) SEM(SV_Target) {
    VsOut i = iIn;
    if ((i.kind & 128u) != 0u) i.pix = Untilt(i.pix);
    uint kb = i.kind & 15u;
    float fade = 1.0f;
    if ((i.kind & 16u) != 0u) fade = saturate(1.0f - (i.pix.y - fReflBase) * fReflDir / max(fReflDepth, 1.0f));
    if (kb == 5u) return LineShade(i) * (fade * fSceneAlpha);
    if (kb == 6u) return BloomShade(i) * fSceneAlpha;
    if (kb == 7u) return SpecShade(i) * fSceneAlpha;
    if (kb == 2u) {
        float2 uv = float2((i.pix.x - i.shape.x) / max(1.0f, i.shape.z - i.shape.x),
                           (i.pix.y - i.shape.y) / max(1.0f, i.shape.w - i.shape.y));
        return gPlate.SampleLevel(gSamp, uv, 0.0f) * fSceneAlpha;
    }
    if (kb == 4u) {
        float fx = (i.pix.x - i.shape.x) / max(1.0f, i.shape.z - i.shape.x);
        float fy = (i.pix.y - i.shape.y) / max(1.0f, i.shape.w - i.shape.y);
        float2 uv = float2(i.radii.x + (i.radii.z - i.radii.x) * fx, i.radii.y + (i.radii.w - i.radii.y) * fy);
        float cov = gGlyphs.SampleLevel(gSamp, uv, 0.0f).w;
        return i.color * (cov * fSceneAlpha);
    }
    if ((i.kind & 32u) != 0u) return FxShade(i, kb) * (fade * fSceneAlpha);
    float cov = Coverage(i, kb);
    return i.color * (cov * fade * fSceneAlpha);
}

// ---- Bloom (FX, 2.1) ---------------------------------------------------------------
// The bars are drawn into a scene texture, which is box-filtered down to a
// quarter of its size, blurred there in two 9-tap Gaussian passes (five
// bilinear reads each), and added back over the scene as light. All four
// passes are one full-surface triangle and run only on frames that draw.
struct FxOut {
    float4 pos SEM(SV_Position);
    float2 uv SEM(TEXCOORD0);
};

FxOut VSFull(uint vid SEM(SV_VertexID)) {
    FxOut o;
    float u = (vid == 1u) ? 2.0f : 0.0f;
    float v = (vid == 2u) ? 2.0f : 0.0f;
    o.uv = float2(u, v);
    o.pos = float4(u * 2.0f - 1.0f, 1.0f - v * 2.0f, 0.0f, 1.0f);
    return o;
}

// Scene to quarter size: four bilinear reads cover the 4 x 4 block.
float4 PSBloomDown(FxOut i) SEM(SV_Target) {
    float tx = fFxTexel.z, ty = fFxTexel.w;
    float4 a = gFxSrc.SampleLevel(gLin, float2(i.uv.x - tx, i.uv.y - ty), 0.0f);
    float4 b = gFxSrc.SampleLevel(gLin, float2(i.uv.x + tx, i.uv.y - ty), 0.0f);
    float4 c = gFxSrc.SampleLevel(gLin, float2(i.uv.x - tx, i.uv.y + ty), 0.0f);
    float4 d = gFxSrc.SampleLevel(gLin, float2(i.uv.x + tx, i.uv.y + ty), 0.0f);
    return (a + b + c + d) * 0.25f;
}

float4 BloomBlur(float2 uv, float dx, float dy) {
    // Weights of a 9-tap Gaussian folded into 5 bilinear reads.
    float4 s = gFxSrc.SampleLevel(gLin, uv, 0.0f) * 0.2270270f;
    s = s + gFxSrc.SampleLevel(gLin, float2(uv.x + dx * 1.3846154f, uv.y + dy * 1.3846154f), 0.0f) * 0.3162162f;
    s = s + gFxSrc.SampleLevel(gLin, float2(uv.x - dx * 1.3846154f, uv.y - dy * 1.3846154f), 0.0f) * 0.3162162f;
    s = s + gFxSrc.SampleLevel(gLin, float2(uv.x + dx * 3.2307692f, uv.y + dy * 3.2307692f), 0.0f) * 0.0702703f;
    s = s + gFxSrc.SampleLevel(gLin, float2(uv.x - dx * 3.2307692f, uv.y - dy * 3.2307692f), 0.0f) * 0.0702703f;
    return s;
}

// The step between taps grows with Bloom Radius (in quarter-size texels).
float BloomStep() { return max(1.0f, fFxBloomR / 16.0f); }
float4 PSBloomH(FxOut i) SEM(SV_Target) { return BloomBlur(i.uv, fFxTexel.x * BloomStep(), 0.0f); }
float4 PSBloomV(FxOut i) SEM(SV_Target) { return BloomBlur(i.uv, 0.0f, fFxTexel.y * BloomStep()); }

// Scene plus its bloom as light, then blended "over" whatever is beneath
// (the plate). Premultiplied: the bloom adds colour and some coverage.
float4 PSBloomComposite(FxOut i) SEM(SV_Target) {
    float4 sc = gFxSrc.SampleLevel(gLin, i.uv, 0.0f);
    float4 bl = gFxBloom.SampleLevel(gLin, i.uv, 0.0f) * (fFxBloom * 1.5f);
    return float4(sc.x + bl.x, sc.y + bl.y, sc.z + bl.z, saturate(sc.w + bl.w * (1.0f - sc.w)));
}


// ---- Analysis on the GPU (Workload = GPU) ----------------------------------------
//
// The same maths as ttdsp, split into four dispatches:
//   CsFft    one group per tier that has new samples: window, real FFT
//            (N/2-point complex radix-2 in group shared memory, then the
//            real-input post-twiddle), power spectrum.
//   CsBands  one thread per band: bin sum or peak, dB with the band's offset,
//            display mapping, ballistics.
//   CsReduce one group: master peak, EQ-zone energies, beat onset, breathe
//            envelope, auto gain, dominant frequency.
//   CsShape  one thread per bar: Shape mapping, peak hold, motion for the
//            skip-unchanged-frames test.
// Nothing comes back to the CPU each frame except an 8-byte stats block, read
// two frames late without waiting.

GROUPSHARED float2 gsFft[2048];
GROUPSHARED float4 gsRed[256];
GROUPSHARED float4 gsRed2[256];

uint BitRev(uint v, uint bits) { return reversebits(v) >> (32u - bits); }

NUMTHREADS(256, 1, 1)
void CsFft(uint3 gid SEM(SV_GroupID), uint3 tid3 SEM(SV_GroupThreadID)) {
    uint tid = tid3.x;
    uint tier = (cTierList >> (2u * gid.x)) & 3u;
    uint n = cN, m = cHalf;
    uint base = tier * n;
    for (uint k = tid; k < m; k += 256u) {
        float x0 = gTierIn[base + 2u * k] * gWindow[2u * k];
        float x1 = gTierIn[base + 2u * k + 1u] * gWindow[2u * k + 1u];
        gsFft[BitRev(k, cLog2Half)] = float2(x0, x1);
    }
    BARRIER;
    for (uint len = 2u; len <= m; len <<= 1u) {
        uint halfLen = len >> 1u;
        for (uint j = tid; j < m / 2u; j += 256u) {
            uint grp = j / halfLen;
            uint pos = j - grp * halfLen;
            uint a = grp * len + pos;
            uint b = a + halfLen;
            float ang = -2.0f * TT_PI * (float)pos / (float)len;
            float wr = cos(ang), wi = sin(ang);
            float2 za = gsFft[a];
            float2 zb = gsFft[b];
            float vr = zb.x * wr - zb.y * wi;
            float vi = zb.x * wi + zb.y * wr;
            gsFft[a] = float2(za.x + vr, za.y + vi);
            gsFft[b] = float2(za.x - vr, za.y - vi);
        }
        BARRIER;
    }
    uint outBase = tier * (m + 1u);
    for (uint k2 = tid; k2 <= m; k2 += 256u) {
        uint ka = (k2 == m) ? 0u : k2;
        uint kb = (k2 == 0u) ? 0u : m - k2;
        float2 za = gsFft[ka];
        float2 zb = gsFft[kb];
        float br = zb.x, bi = -zb.y;
        float er = 0.5f * (za.x + br), ei = 0.5f * (za.y + bi);
        float dr = za.x - br, di = za.y - bi;
        float orr = 0.5f * di, oi = -0.5f * dr;
        float ang = -2.0f * TT_PI * (float)k2 / (float)n;
        float pr = cos(ang), pim = sin(ang);
        float re = er + pr * orr - pim * oi;
        float im = ei + pr * oi + pim * orr;
        gPower[outBase + k2] = re * re + im * im;
    }
}

float CurveOf(float x) {
    if (x <= 0.0f) return 0.0f;
    if (cCurve == 0u) return min(1.0f, (1.0f - exp(-2.0f * x)) / (1.0f - exp(-2.0f)));
    if (cCurve == 2u) return min(1.0f, pow(x, 0.6f));
    if (cCurve == 3u) return min(1.0f, x);
    float knee = 0.7f;
    return (x <= knee) ? x : knee + (1.0f - knee) * tanh((x - knee) / (1.0f - knee));
}

float Ballistic(float y, float x) {
    if (x > y) return y + (x - y) * (1.0f - exp(-cDt * 1000.0f / max(0.1f, cAttackMs)));
    if (cReleaseLinear != 0u) return max(x, y - cReleaseDbPerSec * cDt / max(1.0f, cRangeDb));
    return y + (x - y) * (1.0f - exp(-cDt * 1000.0f / max(0.1f, cReleaseMs)));
}

float DbOf(float p) { return 10.0f * log10(max(p, 1e-30f)); }

NUMTHREADS(64, 1, 1)
void CsBands(uint3 did SEM(SV_DispatchThreadID)) {
    uint b = did.x;
    if (b >= cNumBands) return;
    float4 d = gBandDesc[b];
    float4 off = gBandOff[b];
    uint tier = ((uint)d.x) & 3u;
    uint k0 = (uint)d.y, k1 = (uint)d.z;
    uint pb = tier * (cHalf + 1u);
    float db;
    if (cDetector == 1u) {
        float mx = 0.0f;
        for (uint k = k0; k <= k1; k++) mx = max(mx, gPower[pb + k]);
        db = DbOf(mx) + off.y;
    } else {
        float s;
        if (k0 == k1) s = gPower[pb + k0] * off.z;
        else {
            s = gPower[pb + k0] * off.z + gPower[pb + k1] * off.w;
            for (uint k = k0 + 1u; k < k1; k++) s += gPower[pb + k];
        }
        db = DbOf(s) + off.x;
    }
    float gain = cSensDb + gGlobalsOut[2].x;  // auto gain from the previous frame
    float x = clamp(CurveOf((db + gain - cFloorDb) / max(1.0f, cRangeDb)), 0.0f, 1.0f);
    float4 st = gBandState[b];
    st.x = Ballistic(st.x, x);
    st.y = db;
    gBandState[b] = st;
}

// Owned range of tier t for the dominant-frequency search: above the next
// tier's clean passband, below this one's.
void TierRange(uint t, OUT(float) lo, OUT(float) hi, OUT(float) fs) {
    fs = (t == 0u) ? cTierRate0 : ((t == 1u) ? cTierRate1 : cTierRate2);
    hi = (t == 0u) ? min(cFmax, 0.49f * fs) : 0.4f * fs;
    float nextFs = (t == 0u) ? cTierRate1 : ((t == 1u) ? cTierRate2 : 0.0f);
    lo = (nextFs > 0.0f) ? 0.4f * nextFs : cFmin;
}

NUMTHREADS(256, 1, 1)
void CsReduce(uint3 tid3 SEM(SV_GroupThreadID)) {
    uint tid = tid3.x;
    // Pass 1: master peak, zone energies, bass, loudest raw band (auto gain).
    float mx = 0.0f, zl = 0.0f, zm = 0.0f, zh = 0.0f, bass = 0.0f, rawMax = -300.0f;
    for (uint b = tid; b < cNumBands; b += 256u) {
        float4 st = gBandState[b];
        uint zone = ((uint)gBandDesc[b].x) >> 2u;
        mx = max(mx, st.x);
        if (zone == 0u) zl += st.x;
        else if (zone == 1u) zm += st.x;
        else zh += st.x;
        if (zone == 0u && gBandDesc[b].w < 150.0f) bass = max(bass, st.x);
        rawMax = max(rawMax, st.y);
    }
    gsRed[tid] = float4(mx, zl, zm, zh);
    gsRed2[tid] = float4(bass, rawMax, 0.0f, 0.0f);
    BARRIER;
    for (uint s = 128u; s > 0u; s >>= 1u) {
        if (tid < s) {
            float4 a = gsRed[tid], c = gsRed[tid + s];
            gsRed[tid] = float4(max(a.x, c.x), a.y + c.y, a.z + c.z, a.w + c.w);
            float4 a2 = gsRed2[tid], c2 = gsRed2[tid + s];
            gsRed2[tid] = float4(max(a2.x, c2.x), max(a2.y, c2.y), 0.0f, 0.0f);
        }
        BARRIER;
    }
    float4 red = gsRed[0];
    float4 red2 = gsRed2[0];
    BARRIER;

    // Pass 2: dominant frequency. Each thread keeps its best local maximum.
    float bestDb = -1e9f;
    float bestBin = 0.0f;
    float bestTier = 0.0f;
    if (cDomOn != 0u) {
        for (uint t = 0u; t < 3u; t++) {
            float lo, hi, fs;
            TierRange(t, lo, hi, fs);
            if (fs <= 0.0f) continue;
            if (t > 0u && ((cTierCount >> (8u + t)) & 1u) == 0u) continue;  // tier unused
            float df = fs / (float)cN;
            uint k0 = max(2u, (uint)ceil(lo / df));
            uint k1 = min(cHalf - 2u, (uint)floor(hi / df));
            uint pb = t * (cHalf + 1u);
            for (uint k = k0 + tid; k <= k1; k += 256u) {
                float p = gPower[pb + k];
                if (p >= gPower[pb + k - 1u] && p >= gPower[pb + k + 1u]) {
                    float dbv = DbOf(p) + cPeakCal;
                    if (dbv > bestDb) { bestDb = dbv; bestBin = (float)k; bestTier = (float)t; }
                }
            }
        }
    }
    gsRed[tid] = float4(bestDb, bestBin, bestTier, 0.0f);
    BARRIER;
    for (uint s2 = 128u; s2 > 0u; s2 >>= 1u) {
        if (tid < s2) {
            float4 a = gsRed[tid], c = gsRed[tid + s2];
            if (c.x > a.x) gsRed[tid] = c;
        }
        BARRIER;
    }

    if (tid == 0u) {
        float4 g0 = gGlobalsOut[0];
        float4 g2 = gGlobalsOut[2];
        // Beat: the bass envelope pulling ahead of its own 60 ms average.
        float fast = red2.x;
        float slow = g2.z + (fast - g2.z) * (1.0f - exp(-cDt / 0.06f));
        float pulse = max(0.0f, g0.x - 4.8f * cDt);
        if (fast - slow > 0.12f) pulse = 1.0f;
        // Breathe envelope, as the Direct2D path eases it, from elapsed time.
        float env = g0.z;
        float tau = (red.x > env) ? cBreatheUp : cBreatheDown;
        env += (red.x - env) * (1.0f - exp(-cDt / max(0.01f, tau)));
        // Auto gain: lift the loudest band toward 85% of the range, boost only,
        // frozen through silence.
        float ag = g2.x;
        if (cAutoGain != 0u) {
            float loud = red2.y + cSensDb;
            float target = cFloorDb + 0.85f * cRangeDb;
            if (loud > cFloorDb + 0.1f * cRangeDb) {
                float want = clamp(target - loud, 0.0f, cAutoGainMaxDb);
                ag += (want - ag) * (1.0f - exp(-cDt / 0.4f));
            }
        } else {
            ag = 0.0f;
        }
        gGlobalsOut[0] = float4(pulse, red.x, env, 0.0f);
        float4 g1 = gGlobalsOut[1];
        gGlobalsOut[1] = float4(red.y, red.z, red.w, g1.w);
        gGlobalsOut[2] = float4(ag, 0.0f, slow, 0.0f);
        float hz = 0.0f;
        float4 best = gsRed[0];
        if (cDomOn != 0u && best.x > -90.0f) {
            uint t = (uint)best.z;
            uint k = (uint)best.y;
            float lo, hi, fs;
            TierRange(t, lo, hi, fs);
            uint pb = t * (cHalf + 1u);
            float a = DbOf(gPower[pb + k - 1u]), bq = DbOf(gPower[pb + k]), c = DbOf(gPower[pb + k + 1u]);
            float den = a - 2.0f * bq + c;
            float delta = (abs(den) > 1e-9f) ? clamp(0.5f * (a - c) / den, -0.5f, 0.5f) : 0.0f;
            hz = ((float)k + delta) * fs / (float)cN;
        }
        gStats[0] = asuint(hz);
        gStats[1] = 0u;
    }
}

float SampleBands(float t) {
    float pos = clamp(t, 0.0f, 1.0f) * (float)(cNumBands - 1u);
    uint lo = (uint)pos;
    uint hi = min(lo + 1u, cNumBands - 1u);
    float f = pos - (float)lo;
    return gBandState[lo].x * (1.0f - f) + gBandState[hi].x * f;
}

NUMTHREADS(64, 1, 1)
void CsShape(uint3 did SEM(SV_DispatchThreadID)) {
    uint i = did.x;
    if (i >= cNumBars) return;
    uint n = cNumBars;
    float master = gGlobalsOut[0].y;
    float freqT = (n > 1u) ? (float)i / (float)(n - 1u) : 0.5f;
    float center = (float)(n - 1u) * 0.5f;
    float target = 0.0f;
    uint shape = cShape;
    if (shape == 1u) {  // Mountain
        float dist = abs((float)i - center) / max(1.0f, center);
        float e = SampleBands(dist);
        target = (e + master * (0.2f - dist * 0.12f)) * (1.6f - dist * 0.9f);
    } else if (shape == 2u) {  // Mirror
        float mirT = 1.0f - abs((float)i - center) / max(1.0f, center);
        target = (SampleBands(mirT) + master * (0.1f + mirT * 0.12f)) * 1.3f;
    } else if (shape == 3u) {  // Wave
        target = SampleBands(freqT) * gShapeMod[i] + master * 0.15f;
    } else if (shape == 4u) {  // Breathe
        target = gShapeMod[i] * (0.12f + gGlobalsOut[0].z * 0.88f);
    } else if (shape == 7u || shape == 8u) {
        target = 0.0f;
    } else {
        target = SampleBands(freqT);
    }
    target = clamp(target, 0.0f, 1.0f);

    float4 aux = gBarAux[i];  // peak, timer, velocity, previous level
    float pk = aux.x, timer = aux.y, vel = aux.z;
    if (target >= pk) { pk = target; timer = 0.0f; vel = 0.0f; }
    else {
        timer += cDt;
        if (timer * 1000.0f >= cPeakHoldMs) {
            if (cPeakGravity != 0u) { vel += cGravity * cDt; pk -= vel * cDt; }
            else pk -= cLinFall * cDt;
            if (pk < target) { pk = target; vel = 0.0f; }
            pk = max(pk, 0.0f);
        }
    }
    float moved = max(abs(target - aux.w), abs(pk - gBarsOut[i].y)) * cBarRangePx;
    gBarAux[i] = float4(pk, timer, vel, target);
    gBarsOut[i] = float2(target, pk);
    InterlockedMax(gStats[1], asuint(moved));
}
)HLSL";

#pragma pack(push, 4)
struct FrameCB {
    float viewport[2], block[2];
    float barW, barGap, maxSize, idleSize;
    float radii[4];
    float dotRadii[4];
    uint32_t barCount, shape, vertical, anchor;
    uint32_t colorMode, flags, maxDots, dotSlots;
    float c1[4], grad1[4], c2[4], peakColor[4], beatColor[4];
    float beatIntensity, rainbowBase, sceneAlpha, capThickness;
    float dotStep, dotR, innerR, strokeW;
    float center[2];
    float ampScale, sweepLen;
    float scopeCenter, sweepOrigin, wstep, gonioR;
    float plateRect[4];
    float scopeColor[4];
    float gonioDot, corrY, corrH, corr;
    float termColors[5][4];
    float termGeom[4];   // origin x, origin y, cell width, cell height
    float termAtlas[4];  // atlas width, atlas height, -, -
    uint32_t termCols, termRows, termAtlasCols, termPad;
    uint32_t style, segs, subdiv, specRows;
    float reflBase, reflDir, reflDepth, reflAlpha;
    float segStep, segH, glowR, fillA;
    float vu[4];
    float vuBox[4];
    uint32_t specW, specHead, specTex, specPad;
    float fxGlow, fxGlowR, fxBloom, fxBloomR;
    float fxTexel[4];
    float fxLineColor[4];
    float fxShadowColor[4];
    float fxLineW, fxShadowX, fxShadowY, fxShadowSoft;
    float barDash, barDashGap, barTiltK, barPivot;
    float ghostA, hollowW, mirrorGap;
    uint32_t modFlags;
};
struct PassCB {
    uint32_t pass, count, pad0, pad1;
};
struct CsCB {
    uint32_t n, half, log2Half, numBands;
    uint32_t numBars, tierCount, tierList, detector;
    float floorDb, rangeDb, sensDb, dt;
    float attackMs, releaseMs, releaseDbPerSec, autoGainMaxDb;
    uint32_t releaseLinear, curve, shape, autoGain;
    float peakHoldMs, gravity, linFall, barRangePx;
    uint32_t peakGravity, domOn, pad1, pad2;
    float tierRate0, tierRate1, tierRate2, peakCal;
    float fmin, fmax, breatheUp, breatheDown;
};
#pragma pack(pop)
static_assert(sizeof(FrameCB) == 39 * 16, "FrameCB must match tt_cb.hlsl");
static_assert(sizeof(PassCB) == 16, "PassCB must match tt_cb.hlsl");
static_assert(sizeof(CsCB) == 9 * 16, "CsCB must match tt_cb.hlsl");

enum Pass : uint32_t {
    kPlate = 0, kBars, kCaps, kDots, kRadial, kScope, kGonio, kCorr, kTerm,
    kLed, kLine, kBloom, kSpectro, kVu, kSplit, kSpark, kGhost, kPassCount
};
constexpr int kMaxPoints = 8192;
constexpr int kGonioFrames = 6;  // persistence: this frame and the five before it

// D3DCompile, resolved from d3dcompiler_47.dll, which ships with Windows 10
// and later. Linking it would make the whole mod fail to load on a system
// without it; loaded like this, only this renderer is unavailable there.
typedef HRESULT(WINAPI* PFN_D3DCompile)(LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO*, ID3DInclude*,
                                        LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**);

struct State {
    // Bytecode survives device rebuilds; it doesn't depend on the device.
    ComPtr<ID3DBlob> vsCode, psCode, csCode[4];
    bool compileTried = false, compileOk = false, csCompileTried = false, csCompileOk = false;
    std::wstring compileError;

    // Device objects.
    bool deviceReady = false;
    ComPtr<ID3D11DeviceContext> ctx;
    ComPtr<ID3D11VertexShader> vs;
    ComPtr<ID3D11PixelShader> ps;
    ComPtr<ID3D11ComputeShader> cs[4];
    ComPtr<ID3D11Buffer> frameCB, passCB[kPassCount], passCBRefl[kPassCount], csCB;
    ComPtr<ID3D11Buffer> barsDyn, globalsDyn, waveDyn, pointsDyn;
    ComPtr<ID3D11ShaderResourceView> barsDynSRV, globalsDynSRV, waveSRV, pointsSRV;
    ComPtr<ID3D11SamplerState> sampler;
    ComPtr<ID3D11BlendState> blend;
    ComPtr<ID3D11BlendState> blendOff;  // the plate: it is the first thing drawn, so it only overwrites
    // FX (2.1). The bloom shaders compile on first use; its targets follow
    // the surface size: the scene, and two quarter-size ping-pong buffers.
    bool fxCompileTried = false, fxCompileOk = false;
    ComPtr<ID3D11VertexShader> vsFull;
    ComPtr<ID3D11PixelShader> psFx[4];  // down, blur H, blur V, composite
    ComPtr<ID3D11SamplerState> samplerLin;
    ComPtr<ID3D11Texture2D> fxTex[3];  // scene, quarter A, quarter B
    ComPtr<ID3D11RenderTargetView> fxRtv[3];
    ComPtr<ID3D11ShaderResourceView> fxSrv[3];
    UINT fxW = 0, fxH = 0;
    ComPtr<ID3D11RasterizerState> raster;

    // What each dynamic buffer holds, so one whose contents haven't changed
    // isn't mapped again (WRITE_DISCARD is a driver call and a buffer rename
    // every time). Cleared whenever the buffers are (re)created.
    bool uploadsValid = false;
    uint64_t barsKey = 0, globalsKey = 0, waveKey = 0;
    uint32_t pointsSerial = 0;
    int pointsCount = 0;
    bool cellsValid = false;
    uint32_t cellsSerial = 0;
    uint64_t cellsShape = 0;
    UINT termCount = 0;  // non-blank cells uploaded (the Terminal draw's instance count)
    uint32_t sparkSerial = 0;
    // Spectrogram history (2.1): bars x kVizSpecRows, R8, one row per 1/60 s.
    ComPtr<ID3D11Texture2D> specTex;
    ComPtr<ID3D11ShaderResourceView> specSRV;
    int specW = 0, specHead = 0;
    uint32_t specSerial = 0;
    bool frameCBValid = false;
    FrameCB frameCBLast = {};

    // Panel surface.
    ComPtr<IDXGISwapChain1> sc;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<IDCompositionVisual> visual;
    ComPtr<IDCompositionRectangleClip> clip;
    UINT w = 0, h = 0;
    int offX = 0, offY = 0;  // in layout-local pixels
    bool opaque = false;
    bool visualAttached = false;
    // DirectComposition properties as last committed, so a still panel
    // costs no Commit (each one is a batch sent to DWM).
    bool dcValid = false;
    float dcOffX = 0.f, dcOffY = 0.f;
    bool dcClipOn = false;
    float dcClip[8] = {};  // left, top, right, bottom, radii TL TR BR BL

    // Baked background, the size of the panel surface.
    ComPtr<ID3D11Texture2D> plateTex;
    ComPtr<ID3D11ShaderResourceView> plateSRV;
    bool plateValid = false;
    uint64_t plateKey = 0;

    // GPU workload.
    bool gpuReady = false;
    int gpuSerial = -1, gpuN = 0, gpuBands = 0;
    ComPtr<ID3D11Buffer> tierIn, window, bandDesc, bandOff, shapeMod, power, bandState, barsGpu, barAux,
        globalsGpu, stats, staging[3];
    ComPtr<ID3D11ShaderResourceView> tierInSRV, windowSRV, bandDescSRV, bandOffSRV, shapeModSRV, barsGpuSRV,
        globalsGpuSRV;
    ComPtr<ID3D11UnorderedAccessView> powerUAV, bandStateUAV, barsGpuUAV, barAuxUAV, globalsGpuUAV, statsUAV;
    std::vector<float> tierScratch;
    unsigned frameNo = 0;
    float lastMaxDeltaPx = 1e9f;
    bool gpuWarned = false;

    // Present-on-change.
    uint64_t lastHash = 0;
    bool forcePresent = true;
    uint64_t textKey = 0;
    bool textForce = true;
    bool textDetached = false;  // the text surface is off its visual (nothing to show)
    bool blankPresented = false;

    // Terminal shape: the cell grid and the baked glyph atlas, created on
    // first use so nobody who never picks the shape pays for them.
    ComPtr<ID3D11Buffer> cellsDyn;
    ComPtr<ID3D11ShaderResourceView> cellsSRV;
    ComPtr<ID3D11Texture2D> glyphTex;
    ComPtr<ID3D11ShaderResourceView> glyphSRV;
    uint64_t glyphKey = 0;
    UINT glyphW = 0, glyphH = 0;

    // Goniometer persistence.
    std::vector<float> gonioHist[kGonioFrames];
    uint32_t gonioSerial = 0;
    int gonioHead = 0;

    bool warned = false;
};
State g;

// ---- Hashing (present-on-change) -------------------------------------------------------
inline void Mix(uint64_t& h, uint64_t v) {
    h ^= v + 0x9E3779B97F4A7C15ull + (h << 6) + (h >> 2);
}
// Quantised: v to the nearest 1/q. Rounded with a plain conversion rather than
// llroundf, a library call: about a third off hashing 256 bars and caps.
inline void MixF(uint64_t& h, float v, float q) {
    float r = v * q;
    Mix(h, (uint64_t)(int64_t)(r + (r >= 0.f ? 0.5f : -0.5f)));
}

// ---- Setup -------------------------------------------------------------------------------
bool Compile() {
    if (g.compileTried) return g.compileOk;
    g.compileTried = true;
    HMODULE lib = LoadLibraryExW(L"d3dcompiler_47.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto fn = lib ? (PFN_D3DCompile)(void*)GetProcAddress(lib, "D3DCompile") : nullptr;
    if (!fn) {
        g.compileError = L"d3dcompiler_47.dll isn't available, so the Direct3D 11 renderer can't build its shaders.";
        return false;
    }
    auto one = [&](const char* entry, const char* target, ComPtr<ID3DBlob>& out) -> bool {
        ComPtr<ID3DBlob> err;
        // Optimization level 3 (1 << 15).
        HRESULT hr = fn(kShaderSource, sizeof(kShaderSource) - 1, "tourne-table.hlsl", nullptr, nullptr, entry, target,
                        1u << 15, 0, &out, &err);
        if (FAILED(hr)) {
            std::wstring msg = L"Shader " + std::wstring(entry, entry + strlen(entry)) + L" failed to compile";
            if (err && err->GetBufferPointer()) {
                const char* s = (const char*)err->GetBufferPointer();
                msg += L": ";
                msg += std::wstring(s, s + strnlen(s, 600));
            }
            g.compileError = msg;
            Wh_Log(L"[D3D11] %s", msg.c_str());
            return false;
        }
        return true;
    };
    g.compileOk = one("VSMain", "vs_5_0", g.vsCode) && one("PSMain", "ps_5_0", g.psCode);
    // The compute passes compile on first use of Workload = GPU.
    g.csCompileTried = false;
    return g.compileOk;
}

bool CompileCompute() {
    if (g.csCompileTried) return g.csCompileOk;
    g.csCompileTried = true;
    HMODULE lib = LoadLibraryExW(L"d3dcompiler_47.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto fn = lib ? (PFN_D3DCompile)(void*)GetProcAddress(lib, "D3DCompile") : nullptr;
    if (!fn) return g.csCompileOk = false;
    const char* entries[4] = {"CsFft", "CsBands", "CsReduce", "CsShape"};
    for (int i = 0; i < 4; i++) {
        ComPtr<ID3DBlob> err;
        if (FAILED(fn(kShaderSource, sizeof(kShaderSource) - 1, "tourne-table.hlsl", nullptr, nullptr, entries[i],
                      "cs_5_0", 1u << 15, 0, &g.csCode[i], &err))) {
            if (err) Wh_Log(L"[D3D11] %S failed: %S", entries[i], (const char*)err->GetBufferPointer());
            return g.csCompileOk = false;
        }
    }
    return g.csCompileOk = true;
}

HRESULT MakeStructured(UINT stride, UINT count, bool dynamic, bool uav, const void* init, ComPtr<ID3D11Buffer>& buf,
                       ComPtr<ID3D11ShaderResourceView>* srv, ComPtr<ID3D11UnorderedAccessView>* uavOut) {
    D3D11_BUFFER_DESC d = {};
    d.ByteWidth = stride * count;
    d.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    d.BindFlags = D3D11_BIND_SHADER_RESOURCE | (uav ? D3D11_BIND_UNORDERED_ACCESS : 0);
    d.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    d.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    d.StructureByteStride = stride;
    D3D11_SUBRESOURCE_DATA sd = {init, 0, 0};
    HRESULT hr = g_d3dDevice->CreateBuffer(&d, init ? &sd : nullptr, &buf);
    if (FAILED(hr)) return hr;
    if (srv) {
        hr = g_d3dDevice->CreateShaderResourceView(buf.Get(), nullptr, srv->ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
    }
    if (uavOut) {
        hr = g_d3dDevice->CreateUnorderedAccessView(buf.Get(), nullptr, uavOut->ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
    }
    return S_OK;
}

HRESULT MakeCB(UINT size, bool dynamic, const void* init, ComPtr<ID3D11Buffer>& buf) {
    D3D11_BUFFER_DESC d = {};
    d.ByteWidth = size;
    d.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
    d.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    d.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    D3D11_SUBRESOURCE_DATA sd = {init, 0, 0};
    return g_d3dDevice->CreateBuffer(&d, init ? &sd : nullptr, &buf);
}

bool EnsureDevice() {
    if (g.deviceReady) return true;
    if (!g_d3dDevice) return false;
    if (g_d3dDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0) {
        g.compileError = L"This GPU's Direct3D feature level is below 11.0, which the Direct3D 11 renderer needs.";
        return false;
    }
    if (!Compile()) return false;
    g_d3dDevice->GetImmediateContext(&g.ctx);
    if (FAILED(g_d3dDevice->CreateVertexShader(g.vsCode->GetBufferPointer(), g.vsCode->GetBufferSize(), nullptr, &g.vs)) ||
        FAILED(g_d3dDevice->CreatePixelShader(g.psCode->GetBufferPointer(), g.psCode->GetBufferSize(), nullptr, &g.ps)))
        return false;
    if (FAILED(MakeCB(sizeof(FrameCB), true, nullptr, g.frameCB))) return false;
    for (uint32_t p = 0; p < kPassCount; p++) {
        // pPad1 = 1: this pass's rects and capsules take the Glow FX.
        const uint32_t glow = (p == kBars || p == kCaps || p == kDots || p == kRadial || p == kScope || p == kLed ||
                               p == kSplit || p == kSpark || p == kGhost) ? 1u : 0u;
        PassCB pc = {p, 0, 0, glow};
        if (FAILED(MakeCB(sizeof(PassCB), false, &pc, g.passCB[p]))) return false;
        // The same pass mirrored for Reflection: pPad0 = 1.
        PassCB pr = {p, 0, 1, glow};
        if (FAILED(MakeCB(sizeof(PassCB), false, &pr, g.passCBRefl[p]))) return false;
    }
    if (FAILED(MakeStructured(8, VIZ_BARS_MAX, true, false, nullptr, g.barsDyn, &g.barsDynSRV, nullptr)) ||
        FAILED(MakeStructured(16, 4, true, false, nullptr, g.globalsDyn, &g.globalsDynSRV, nullptr)) ||
        FAILED(MakeStructured(4, VIZ_WAVE_SAMPLES, true, false, nullptr, g.waveDyn, &g.waveSRV, nullptr)) ||
        FAILED(MakeStructured(16, kMaxPoints, true, false, nullptr, g.pointsDyn, &g.pointsSRV, nullptr)))
        return false;

    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    if (FAILED(g_d3dDevice->CreateSamplerState(&sd, &g.sampler))) return false;

    // Premultiplied alpha, like every other surface in this mod.
    D3D11_BLEND_DESC bd = {};
    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(g_d3dDevice->CreateBlendState(&bd, &g.blend))) return false;
    // The plate is drawn first, over a cleared target: premultiplied "over"
    // onto zero is the source itself, so blending only adds a read of the
    // whole target. Without it the plate just writes.
    bd.RenderTarget[0].BlendEnable = FALSE;
    if (FAILED(g_d3dDevice->CreateBlendState(&bd, &g.blendOff))) return false;
    g.uploadsValid = false;
    g.cellsValid = false;
    g.frameCBValid = false;

    D3D11_RASTERIZER_DESC rd = {};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    if (FAILED(g_d3dDevice->CreateRasterizerState(&rd, &g.raster))) return false;

    g.deviceReady = true;
    Wh_Log(L"[D3D11] renderer ready (feature level %X)", (unsigned)g_d3dDevice->GetFeatureLevel());
    return true;
}

void ReleaseGpuAnalysis() {
    g.gpuReady = false;
    g.gpuSerial = -1;
    g.tierIn.Reset(); g.window.Reset(); g.bandDesc.Reset(); g.bandOff.Reset(); g.shapeMod.Reset();
    g.power.Reset(); g.bandState.Reset(); g.barsGpu.Reset(); g.barAux.Reset(); g.globalsGpu.Reset();
    g.stats.Reset();
    for (auto& s : g.staging) s.Reset();
    g.tierInSRV.Reset(); g.windowSRV.Reset(); g.bandDescSRV.Reset(); g.bandOffSRV.Reset(); g.shapeModSRV.Reset();
    g.barsGpuSRV.Reset(); g.globalsGpuSRV.Reset();
    g.powerUAV.Reset(); g.bandStateUAV.Reset(); g.barsGpuUAV.Reset(); g.barAuxUAV.Reset(); g.globalsGpuUAV.Reset();
    g.statsUAV.Reset();
}

// The text surface (g_compositionVisual: the Direct2D swap chain, the size of
// the whole widget box) sits over the panel. Empty, DWM would still read and
// blend it in every frame it composes there, so while there is no text it is
// taken off its visual, and put back right after the first frame drawn on it.
void ShowTextSurface(bool show) {
    if (show != g.textDetached) return;  // already so
    if (!g_compositionVisual || !g_compositionDevice || (show && !g_swapChain)) {
        g.textDetached = false;
        return;
    }
    if (FAILED(g_compositionVisual->SetContent(show ? (IUnknown*)g_swapChain.Get() : nullptr))) return;
    g_compositionDevice->Commit();
    VizPerf(kPerfCommits);
    g.textDetached = !show;
}

// Split into two pieces: keeps the second piece's copies of the text and panel
// surfaces in step with the originals, and the cut where the settings put it.
// Runs every tick but commits only when something changed.
void SplitSync() {
    auto& s = g_split;
    if (!g_compositionDevice || !g_splitTop || !g_rootVisual || !g_compositionVisual) return;
    bool want = g_settings.splitEnabled && (g_settings.splitGap != 0.f || g_settings.splitShift != 0.f);
    VizLayout L;
    if (want && !ComputeVizLayout(&L)) want = false;
    bool dirty = false;
    if (!want) {
        if (s.on) {
            g_rootVisual->SetClip((IDCompositionClip*)nullptr);
            g_splitTop->RemoveVisual(s.p2.Get());
            s.on = false;
            g_compositionDevice->Commit();
            VizPerf(kPerfCommits);
        }
        return;
    }
    if (!s.built) {
        if (FAILED(g_compositionDevice->CreateVisual(&s.p2)) || FAILED(g_compositionDevice->CreateVisual(&s.text2)) ||
            FAILED(g_compositionDevice->CreateVisual(&s.gfx2)) ||
            FAILED(g_compositionDevice->CreateRectangleClip(&s.c1)) ||
            FAILED(g_compositionDevice->CreateRectangleClip(&s.c2)) ||
            FAILED(s.p2->AddVisual(s.text2.Get(), FALSE, nullptr)))
            return;
        s.gfx2->SetBorderMode(DCOMPOSITION_BORDER_MODE_SOFT);
        s.p2->SetClip(s.c2.Get());
        s.built = true;
        s.line = NAN;
    }
    if (!s.on) {
        if (FAILED(g_splitTop->AddVisual(s.p2.Get(), TRUE, g_rootVisual.Get()))) return;
        g_rootVisual->SetClip(s.c1.Get());
        s.on = true;
        dirty = true;
    }

    // Where the cut goes, in overlay pixels. Bar shapes cut in the middle of
    // the gap between two bars; the rest at that fraction of the box.
    const VizShape sh = g_settings.shape;
    const bool vert = g_settings.orientation == VizOrientation::Vertical && sh != VizShape::Radial &&
                      sh != VizShape::Goniometer && sh != VizShape::Terminal;
    const bool barish = sh != VizShape::Radial && sh != VizShape::Goniometer && sh != VizShape::Terminal &&
                        sh != VizShape::Oscilloscope;
    float start = vert ? g_visualOffsetY + L.blockY : g_visualOffsetX + L.blockX;
    float len = vert ? L.totalHeight : L.totalWidth;
    float line = start + len * g_settings.splitAt / 100.f;
    if (barish) {
        int n = VizEffectiveBarCount();
        float bw = std::max(1.f, VizPx(std::max(1.f, g_settings.barWidth)));
        float gap = VizPx(std::max(0.f, g_settings.barGap));
        if (n >= 2) {
            int k = std::clamp((int)lroundf(n * g_settings.splitAt / 100.f), 1, n - 1);
            line = start + k * (bw + gap) - gap * 0.5f;
        }
    }
    float along = g_settings.splitGap * g_dpiScale, across = g_settings.splitShift * g_dpiScale;
    float dx = vert ? across : along, dy = vert ? along : across;
    if (line != s.line || dx != s.dx || dy != s.dy || vert != s.vert) {
        const float big = 1e6f;
        s.c1->SetLeft(-big); s.c1->SetTop(-big); s.c1->SetRight(big); s.c1->SetBottom(big);
        s.c2->SetLeft(-big); s.c2->SetTop(-big); s.c2->SetRight(big); s.c2->SetBottom(big);
        if (vert) { s.c1->SetBottom(line); s.c2->SetTop(line); }
        else      { s.c1->SetRight(line);  s.c2->SetLeft(line); }
        s.p2->SetOffsetX(dx);
        s.p2->SetOffsetY(dy);
        s.line = line; s.dx = dx; s.dy = dy; s.vert = vert;
        dirty = true;
    }

    // The text surface again (the whole scene, on the Direct2D renderer).
    IUnknown* tc = g.textDetached ? nullptr : (IUnknown*)g_swapChain.Get();
    if (tc != s.textC) { s.text2->SetContent(tc); s.textC = tc; dirty = true; }
    if (g_visualOffsetX != s.tx || g_visualOffsetY != s.ty) {
        s.text2->SetOffsetX(g_visualOffsetX);
        s.text2->SetOffsetY(g_visualOffsetY);
        s.tx = g_visualOffsetX; s.ty = g_visualOffsetY;
        dirty = true;
    }

    // The Direct3D 11 panel surface again, sharing its clip object.
    bool gin = g.visualAttached && g.sc;
    if (gin) {
        if (!s.gfxIn) {
            if (FAILED(s.p2->AddVisual(s.gfx2.Get(), FALSE, s.text2.Get()))) return;
            s.gfxIn = true;
            dirty = true;
        }
        if ((IUnknown*)g.sc.Get() != s.gfxC) { s.gfx2->SetContent(g.sc.Get()); s.gfxC = g.sc.Get(); dirty = true; }
        if (g.dcOffX != s.gx || g.dcOffY != s.gy) {
            s.gfx2->SetOffsetX(g.dcOffX);
            s.gfx2->SetOffsetY(g.dcOffY);
            s.gx = g.dcOffX; s.gy = g.dcOffY;
            dirty = true;
        }
        if (g.dcClipOn != s.gClip) {
            s.gfx2->SetClip(g.dcClipOn ? (IDCompositionClip*)g.clip.Get() : nullptr);
            s.gClip = g.dcClipOn;
            dirty = true;
        }
    } else if (s.gfxIn) {
        s.p2->RemoveVisual(s.gfx2.Get());
        s.gfx2->SetContent(nullptr);
        s.gfxC = nullptr;
        s.gfxIn = false;
        dirty = true;
    }
    if (dirty) {
        g_compositionDevice->Commit();
        VizPerf(kPerfCommits);
    }
}

// The panel surface and its visual. Called before the composition device goes.
void ReleaseSurface() {
    ShowTextSurface(true);  // the Direct2D path draws everything on the text surface
    if (!g.sc && !g.visual && !g.plateTex) return;  // nothing to release (called every Direct2D frame)
    if (g.visual && g.visualAttached && g_rootVisual) {
        g_rootVisual->RemoveVisual(g.visual.Get());
        if (g_compositionDevice) {
            g_compositionDevice->Commit();
            VizPerf(kPerfCommits);
        }
    }
    g.visualAttached = false;
    g.dcValid = false;
    g.dcClipOn = false;
    g.rtv.Reset();
    g.sc.Reset();
    g.clip.Reset();
    g.visual.Reset();
    g.plateTex.Reset();
    g.plateSRV.Reset();
    g.plateValid = false;
    for (int k = 0; k < 3; k++) {
        g.fxTex[k].Reset();
        g.fxRtv[k].Reset();
        g.fxSrv[k].Reset();
    }
    g.fxW = g.fxH = 0;
    g.w = g.h = 0;
    g.forcePresent = true;
    g.textForce = true;
}

// Everything that belongs to the D3D device (device lost, Drawing Device
// changed, unload). The compiled bytecode is kept.
void ReleaseDevice() {
    ReleaseSurface();
    ReleaseGpuAnalysis();
    g.vs.Reset(); g.ps.Reset();
    for (auto& c : g.cs) c.Reset();
    g.frameCB.Reset(); g.csCB.Reset();
    for (auto& p : g.passCB) p.Reset();
    for (auto& p : g.passCBRefl) p.Reset();
    g.barsDyn.Reset(); g.globalsDyn.Reset(); g.waveDyn.Reset(); g.pointsDyn.Reset();
    g.barsDynSRV.Reset(); g.globalsDynSRV.Reset(); g.waveSRV.Reset(); g.pointsSRV.Reset();
    g.cellsDyn.Reset(); g.cellsSRV.Reset(); g.glyphTex.Reset(); g.glyphSRV.Reset();
    g.specTex.Reset(); g.specSRV.Reset(); g.specW = 0;
    g.glyphKey = 0;
    g.sampler.Reset(); g.blend.Reset(); g.blendOff.Reset(); g.raster.Reset();
    g.vsFull.Reset(); g.samplerLin.Reset();
    for (auto& ps : g.psFx) ps.Reset();
    g.fxCompileTried = false;
    g.uploadsValid = g.cellsValid = g.frameCBValid = false;
    if (g.ctx) g.ctx->ClearState();
    g.ctx.Reset();
    g.deviceReady = false;
}

void OnSettingsChanged() {
    g.plateValid = false;
    g.forcePresent = true;
    g.textForce = true;
    g.warned = false;
    g.gpuWarned = false;
}

// Is this renderer the one drawing? Falls back to Direct2D, once, with a
// warning, if it can't be set up.
bool Active() {
    if (g_settings.renderer != VizRenderer::D3D11) return false;
    if (EnsureDevice()) return true;
    if (!g.warned) {
        g.warned = true;
        ReportSettingWarning(L"Hardware", L"Renderer",
                             (g.compileError.empty() ? std::wstring(L"The Direct3D 11 renderer couldn't start")
                                                     : g.compileError) +
                                 L" Drawing with Direct2D instead.");
        FlushSettingsIssues();
    }
    return false;
}

// ---- Panel surface ------------------------------------------------------------------------
bool EnsureSurface(int offX, int offY, UINT w, UINT h, bool opaque, const D2D1_RECT_F& clipRect,
                   const float clipRadii[4], float layoutOriginX, float layoutOriginY) {
    if (!g_compositionDevice || !g_rootVisual) return false;
    w = std::max(1u, w);
    h = std::max(1u, h);
    bool recreate = !g.sc || opaque != g.opaque;
    bool dirty = false;  // something DirectComposition needs to be told
    if (recreate) {
        dirty = true;
        g.rtv.Reset();
        g.sc.Reset();
        DXGI_SWAP_CHAIN_DESC1 scd = {};
        scd.Width = w;
        scd.Height = h;
        scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        scd.SampleDesc.Count = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount = 2;
        scd.Scaling = DXGI_SCALING_STRETCH;
        scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scd.AlphaMode = opaque ? DXGI_ALPHA_MODE_IGNORE : DXGI_ALPHA_MODE_PREMULTIPLIED;
        if (FAILED(g_dxgiFactory->CreateSwapChainForComposition(g_dxgiDevice.Get(), &scd, nullptr, &g.sc))) return false;
        g.w = w;
        g.h = h;
        g.opaque = opaque;
        if (!g.visual && FAILED(g_compositionDevice->CreateVisual(&g.visual))) return false;
        g.visual->SetContent(g.sc.Get());
        g.visual->SetBorderMode(DCOMPOSITION_BORDER_MODE_SOFT);
    } else if (w != g.w || h != g.h) {
        g.rtv.Reset();
        if (FAILED(g.sc->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0))) return false;
        g.w = w;
        g.h = h;
        dirty = true;
    }
    if (!g.rtv) {
        ComPtr<ID3D11Texture2D> back;
        if (FAILED(g.sc->GetBuffer(0, IID_PPV_ARGS(&back)))) return false;
        if (FAILED(g_d3dDevice->CreateRenderTargetView(back.Get(), nullptr, &g.rtv))) return false;
        g.forcePresent = true;
        g.plateValid = false;
    }
    if (!g.visualAttached) {
        // Below the text surface, so readouts placed over the bars stay on top.
        if (FAILED(g_rootVisual->AddVisual(g.visual.Get(), FALSE, g_compositionVisual.Get()))) return false;
        g.visualAttached = true;
        dirty = true;
    }
    // Offset and clip are set, and committed, only when they change: a
    // Commit sends a batch to DWM even when every value in it is the same,
    // and this runs on every render tick, including the ones the
    // skip-unchanged-frames test then doesn't draw.
    const float ox = layoutOriginX + (float)offX, oy = layoutOriginY + (float)offY;
    if (dirty || !g.dcValid || ox != g.dcOffX || oy != g.dcOffY) {
        g.visual->SetOffsetX(ox);
        g.visual->SetOffsetY(oy);
        g.dcOffX = ox;
        g.dcOffY = oy;
        dirty = true;
    }
    const float clipNow[8] = {clipRect.left, clipRect.top, clipRect.right, clipRect.bottom,
                              clipRadii[0],  clipRadii[1], clipRadii[2],   clipRadii[3]};
    if (opaque) {
        if (!g.clip && FAILED(g_compositionDevice->CreateRectangleClip(&g.clip))) return false;
        if (dirty || !g.dcValid || !g.dcClipOn || memcmp(clipNow, g.dcClip, sizeof(clipNow)) != 0) {
            g.clip->SetLeft(clipRect.left);
            g.clip->SetTop(clipRect.top);
            g.clip->SetRight(clipRect.right);
            g.clip->SetBottom(clipRect.bottom);
            g.clip->SetTopLeftRadiusX(clipRadii[0]);
            g.clip->SetTopLeftRadiusY(clipRadii[0]);
            g.clip->SetTopRightRadiusX(clipRadii[1]);
            g.clip->SetTopRightRadiusY(clipRadii[1]);
            g.clip->SetBottomRightRadiusX(clipRadii[2]);
            g.clip->SetBottomRightRadiusY(clipRadii[2]);
            g.clip->SetBottomLeftRadiusX(clipRadii[3]);
            g.clip->SetBottomLeftRadiusY(clipRadii[3]);
            g.visual->SetClip(g.clip.Get());
            memcpy(g.dcClip, clipNow, sizeof(clipNow));
            g.dcClipOn = true;
            dirty = true;
        }
    } else if (dirty || !g.dcValid || g.dcClipOn) {
        g.visual->SetClip((IDCompositionClip*)nullptr);
        g.dcClipOn = false;
        dirty = true;
    }
    if (offX != g.offX || offY != g.offY) g.forcePresent = true;
    g.offX = offX;
    g.offY = offY;
    if (dirty) {
        g_compositionDevice->Commit();
        VizPerf(kPerfCommits);
        g.dcValid = true;
    }
    return true;
}

// Presents one transparent frame on the panel surface (auto-hide, wallpaper
// capture). Cheap, and only ever done once per state change.
void PresentBlank() {
    if (!g.sc || !g.rtv || !g.ctx) return;
    const float zero[4] = {0, 0, 0, 0};
    g.ctx->ClearRenderTargetView(g.rtv.Get(), zero);
    HRESULT hr = g.sc->Present(0, 0);
    VizCheckDeviceLost(S_OK, hr);
    g.forcePresent = true;
}

// ---- Background plate ------------------------------------------------------------------
// Baked once into a texture the size of the panel surface: blurred wallpaper,
// panel fill and border, exactly as the Direct2D path composes them. For the
// opaque panel the fill is square and the border's outer edge is pushed a
// pixel out, because the DirectComposition clip draws the rounded edge.
bool BakePlate(const D2D1_RECT_F& bgRect, const float radii[4]) {
    if (!g_d2dDevice || !g_backgroundBrush || !g_d2dFactory) return false;
    if (!g.plateTex || [&] {
            D3D11_TEXTURE2D_DESC d;
            g.plateTex->GetDesc(&d);
            return d.Width != g.w || d.Height != g.h;
        }()) {
        g.plateTex.Reset();
        g.plateSRV.Reset();
        D3D11_TEXTURE2D_DESC td = {};
        td.Width = g.w;
        td.Height = g.h;
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        if (FAILED(g_d3dDevice->CreateTexture2D(&td, nullptr, &g.plateTex))) return false;
        if (FAILED(g_d3dDevice->CreateShaderResourceView(g.plateTex.Get(), nullptr, &g.plateSRV))) return false;
    }
    ComPtr<IDXGISurface> surf;
    if (FAILED(g.plateTex.As(&surf))) return false;
    ComPtr<ID2D1DeviceContext> bake;
    if (FAILED(g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &bake))) return false;
    D2D1_BITMAP_PROPERTIES1 bp = {};
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    ComPtr<ID2D1Bitmap1> target;
    if (FAILED(bake->CreateBitmapFromDxgiSurface(surf.Get(), bp, &target))) return false;

    ComPtr<ID2D1SolidColorBrush> fill, border;
    bake->CreateSolidColorBrush(g_backgroundBrush->GetColor(), &fill);
    if (g_borderBrush) bake->CreateSolidColorBrush(g_borderBrush->GetColor(), &border);

    // Panel geometry in this surface's coordinates.
    D2D1_RECT_F r = D2D1::RectF(bgRect.left - g.offX, bgRect.top - g.offY, bgRect.right - g.offX,
                                bgRect.bottom - g.offY);
    ComPtr<ID2D1PathGeometry> geo, ringOuter, ringInner;
    ComPtr<ID2D1GeometryGroup> ring;
    CreateRoundedRectPath(g_d2dFactory.Get(), r, radii[0], radii[1], radii[2], radii[3], &geo);
    if (border) {
        float bw = std::min((float)g_settings.bgBorderSize * g_dpiScale,
                            std::min(r.right - r.left, r.bottom - r.top) / 2.0f);
        float grow = g.opaque ? 1.0f : 0.0f;
        D2D1_RECT_F outer = D2D1::RectF(r.left - grow, r.top - grow, r.right + grow, r.bottom + grow);
        CreateRoundedRectPath(g_d2dFactory.Get(), outer, radii[0] + grow, radii[1] + grow, radii[2] + grow,
                              radii[3] + grow, &ringOuter);
        D2D1_RECT_F inner = D2D1::RectF(r.left + bw, r.top + bw, r.right - bw, r.bottom - bw);
        CreateRoundedRectPath(g_d2dFactory.Get(), inner, std::max(0.f, radii[0] - bw), std::max(0.f, radii[1] - bw),
                              std::max(0.f, radii[2] - bw), std::max(0.f, radii[3] - bw), &ringInner);
        if (ringOuter && ringInner) {
            ID2D1Geometry* geos[] = {ringOuter.Get(), ringInner.Get()};
            g_d2dFactory->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE, geos, 2, &ring);
        }
    }

    bake->SetTarget(target.Get());
    bake->BeginDraw();
    bake->Clear(D2D1::ColorF(0, 0, 0, 0));
    if (g.opaque) {
        // Everything square: the clip rounds it. Blur first, the fill on top.
        if (g_blurredBitmap) {
            bake->SetTransform(D2D1::Matrix3x2F::Translation(-(float)g.offX, -(float)g.offY));
            bake->DrawBitmap(g_blurredBitmap.Get());
            bake->SetTransform(D2D1::Matrix3x2F::Identity());
        }
        bake->FillRectangle(D2D1::RectF(0, 0, (float)g.w, (float)g.h), fill.Get());
        if (ring && border) bake->FillGeometry(ring.Get(), border.Get());
    } else {
        if (g_blurredBitmap && geo) {
            bake->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geo.Get()), nullptr);
            bake->SetTransform(D2D1::Matrix3x2F::Translation(-(float)g.offX, -(float)g.offY));
            bake->DrawBitmap(g_blurredBitmap.Get());
            bake->SetTransform(D2D1::Matrix3x2F::Identity());
            bake->PopLayer();
        }
        if (geo) bake->FillGeometry(geo.Get(), fill.Get());
        if (ring && border) bake->FillGeometry(ring.Get(), border.Get());
    }
    HRESULT hr = bake->EndDraw();
    bake->SetTarget(nullptr);
    if (FAILED(hr)) return false;
    g.plateValid = true;
    return true;
}

// ---- GPU workload --------------------------------------------------------------------------
// 1 ready, 0 not yet (the engine hasn't published a band layout; drawing
// shows idle bars until it does), -1 failed on this device.
int EnsureGpuAnalysis() {
    if (!CompileCompute()) return -1;
    for (int i = 0; i < 4; i++) {
        if (!g.cs[i] && FAILED(g_d3dDevice->CreateComputeShader(g.csCode[i]->GetBufferPointer(),
                                                                 g.csCode[i]->GetBufferSize(), nullptr, &g.cs[i])))
            return -1;
    }
    if (!g.csCB && FAILED(MakeCB(sizeof(CsCB), true, nullptr, g.csCB))) return -1;

    std::lock_guard<std::mutex> lock(g_gpuFeed.m);
    if (g_gpuFeed.n <= 0 || g_gpuFeed.maps.empty()) return 0;
    if (g.gpuReady && g.gpuSerial == g_gpuFeed.layoutSerial) return 1;

    ReleaseGpuAnalysis();
    const int n = g_gpuFeed.n, half = n / 2, nb = (int)g_gpuFeed.maps.size();
    std::vector<float> desc((size_t)nb * 4), off((size_t)nb * 4);
    for (int b = 0; b < nb; b++) {
        const auto& m = g_gpuFeed.maps[b];
        double fc = g_gpuFeed.bands[b].fc;
        int zone = (fc < 300.0) ? 0 : (fc < 2500.0) ? 1 : 2;
        desc[b * 4 + 0] = (float)(m.tier + 4 * zone);
        desc[b * 4 + 1] = (float)m.k0;
        desc[b * 4 + 2] = (float)m.k1;
        desc[b * 4 + 3] = (float)fc;
        off[b * 4 + 0] = m.offsetDb;
        off[b * 4 + 1] = m.peakOffsetDb;
        off[b * 4 + 2] = m.w0;
        off[b * 4 + 3] = m.w1;
    }
    std::vector<float> zerosA((size_t)std::max(nb, VIZ_BARS_MAX) * 4, 0.f);
    bool ok = SUCCEEDED(MakeStructured(4, 3 * n, false, false, g_gpuFeed.blocks.data(), g.tierIn, &g.tierInSRV, nullptr)) &&
              SUCCEEDED(MakeStructured(4, n, false, false, g_gpuFeed.window.data(), g.window, &g.windowSRV, nullptr)) &&
              SUCCEEDED(MakeStructured(16, nb, false, false, desc.data(), g.bandDesc, &g.bandDescSRV, nullptr)) &&
              SUCCEEDED(MakeStructured(16, nb, false, false, off.data(), g.bandOff, &g.bandOffSRV, nullptr)) &&
              SUCCEEDED(MakeStructured(4, VIZ_BARS_MAX, true, false, nullptr, g.shapeMod, &g.shapeModSRV, nullptr)) &&
              SUCCEEDED(MakeStructured(4, 3 * (half + 1), false, true, nullptr, g.power, nullptr, &g.powerUAV)) &&
              SUCCEEDED(MakeStructured(16, nb, false, true, zerosA.data(), g.bandState, nullptr, &g.bandStateUAV)) &&
              SUCCEEDED(MakeStructured(8, VIZ_BARS_MAX, false, true, zerosA.data(), g.barsGpu, &g.barsGpuSRV, &g.barsGpuUAV)) &&
              SUCCEEDED(MakeStructured(16, VIZ_BARS_MAX, false, true, zerosA.data(), g.barAux, nullptr, &g.barAuxUAV)) &&
              SUCCEEDED(MakeStructured(16, 4, false, true, zerosA.data(), g.globalsGpu, &g.globalsGpuSRV, &g.globalsGpuUAV)) &&
              SUCCEEDED(MakeStructured(4, 4, false, true, zerosA.data(), g.stats, nullptr, &g.statsUAV));
    if (ok) {
        // Same size and structured flags as the stats buffer, so CopyResource
        // between them is valid on every driver.
        for (auto& s : g.staging) {
            D3D11_BUFFER_DESC d = {};
            d.ByteWidth = 16;
            d.Usage = D3D11_USAGE_STAGING;
            d.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            d.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            d.StructureByteStride = 4;
            if (FAILED(g_d3dDevice->CreateBuffer(&d, nullptr, &s))) ok = false;
        }
    }
    if (!ok) {
        ReleaseGpuAnalysis();
        return -1;
    }
    g.gpuN = n;
    g.gpuBands = nb;
    g.gpuSerial = g_gpuFeed.layoutSerial;
    g.tierScratch.assign((size_t)3 * n, 0.f);
    g_gpuFeed.dirty = 7;
    g.gpuReady = true;
    g.lastMaxDeltaPx = 1e9f;
    Wh_Log(L"[GPU] analysis on the GPU: %d bands, FFT %d", nb, n);
    return 1;
}

// Runs the four analysis passes. Leaves barsGpu / globalsGpu holding this
// frame's bars for the draw.
void RunGpuAnalysis(int bars, float barRangePx) {
    unsigned dirty = 0;
    int n = g.gpuN;
    double rates[3];
    double peakCal;
    {
        std::lock_guard<std::mutex> lock(g_gpuFeed.m);
        // The layout can change between EnsureGpuAnalysis and here; the
        // blocks then have a different size, so take nothing this frame.
        if (g_gpuFeed.layoutSerial == g.gpuSerial && g_gpuFeed.blocks.size() >= (size_t)3 * n) {
            dirty = g_gpuFeed.dirty;
            g_gpuFeed.dirty = 0;
        }
        if (dirty) memcpy(g.tierScratch.data(), g_gpuFeed.blocks.data(), sizeof(float) * 3 * (size_t)n);
        for (int t = 0; t < 3; t++) rates[t] = g_gpuFeed.rate[t];
        peakCal = g_gpuFeed.peakCal;
    }
    uint32_t tierList = 0, tierCount = 0, used = 0;
    for (int t = 0; t < 3; t++) {
        if (!(dirty & (1u << t))) continue;
        D3D11_BOX box = {(UINT)(t * n * 4), 0, 0, (UINT)((t + 1) * n * 4), 1, 1};
        g.ctx->UpdateSubresource(g.tierIn.Get(), 0, &box, &g.tierScratch[(size_t)t * n], 0, 0);
        tierList |= (uint32_t)t << (2 * tierCount);
        tierCount++;
    }
    {
        std::lock_guard<std::mutex> lock(g_gpuFeed.m);
        for (int t = 0; t < 3; t++)
            if (g_gpuFeed.used[t]) used |= 1u << t;
    }

    VizEngineConfig cfg;
    {
        std::lock_guard<std::mutex> lock(g_engineCfgMutex);
        cfg = g_engineCfg;
    }
    D3D11_MAPPED_SUBRESOURCE ms;
    if (SUCCEEDED(g.ctx->Map(g.csCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
        CsCB c = {};
        c.n = (uint32_t)n;
        c.half = (uint32_t)n / 2;
        c.log2Half = (uint32_t)ttdsp::Log2i(n / 2);
        c.numBands = (uint32_t)g.gpuBands;
        c.numBars = (uint32_t)bars;
        c.tierCount = tierCount | (used << 8);
        c.tierList = tierList;
        c.detector = cfg.spec.detector == ttdsp::Detector::Peak ? 1u : 0u;
        c.floorDb = cfg.disp.floorDb;
        c.rangeDb = std::max(1.f, cfg.disp.ceilDb - cfg.disp.floorDb);
        c.sensDb = cfg.sensDb;
        c.dt = std::clamp(g_frameDt, 0.0005f, 0.25f);
        c.attackMs = (float)cfg.ball.attackMs;
        c.releaseMs = (float)cfg.ball.releaseMs;
        c.releaseDbPerSec = (float)cfg.ball.releaseDbPerSec;
        c.autoGainMaxDb = cfg.autoGainMaxDb;
        c.releaseLinear = cfg.ball.release == ttdsp::ReleaseKind::Linear ? 1u : 0u;
        c.curve = (uint32_t)cfg.disp.curve;
        c.shape = (uint32_t)g_settings.shape;
        c.autoGain = cfg.autoGain ? 1u : 0u;
        c.peakHoldMs = (float)g_settings.peakHoldMs;
        c.gravity = 2.0f;
        c.linFall = 0.72f;
        c.barRangePx = barRangePx;
        c.peakGravity = g_settings.peakFall == VizPeakFall::Gravity ? 1u : 0u;
        c.domOn = cfg.wantDominant ? 1u : 0u;
        c.tierRate0 = (float)rates[0];
        c.tierRate1 = (float)rates[1];
        c.tierRate2 = (float)rates[2];
        c.peakCal = (float)peakCal;
        c.fmin = (float)cfg.spec.fmin;
        c.fmax = (float)cfg.spec.fmax;
        c.breatheUp = 0.408f;
        c.breatheDown = 1.103f;
        memcpy(ms.pData, &c, sizeof(c));
        g.ctx->Unmap(g.csCB.Get(), 0);
    }
    if (g_settings.shape == VizShape::Wave || g_settings.shape == VizShape::Breathe) {
        if (SUCCEEDED(g.ctx->Map(g.shapeMod.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            float* m = (float*)ms.pData;
            double clock = VizClockSeconds();
            for (int i = 0; i < bars; i++) m[i] = VizShapeMod(i, bars, clock);
            g.ctx->Unmap(g.shapeMod.Get(), 0);
        }
    }

    ID3D11ShaderResourceView* srvs[5] = {g.tierInSRV.Get(), g.windowSRV.Get(), g.bandDescSRV.Get(),
                                         g.bandOffSRV.Get(), g.shapeModSRV.Get()};
    ID3D11UnorderedAccessView* uavs[6] = {g.powerUAV.Get(), g.bandStateUAV.Get(), g.barsGpuUAV.Get(),
                                          g.barAuxUAV.Get(), g.globalsGpuUAV.Get(), g.statsUAV.Get()};
    ID3D11Buffer* cbs[1] = {g.csCB.Get()};
    g.ctx->CSSetShaderResources(5, 5, srvs);
    g.ctx->CSSetUnorderedAccessViews(0, 6, uavs, nullptr);
    g.ctx->CSSetConstantBuffers(2, 1, cbs);
    if (tierCount) {
        g.ctx->CSSetShader(g.cs[0].Get(), nullptr, 0);
        g.ctx->Dispatch(tierCount, 1, 1);
    }
    g.ctx->CSSetShader(g.cs[1].Get(), nullptr, 0);
    g.ctx->Dispatch((UINT)(g.gpuBands + 63) / 64, 1, 1);
    g.ctx->CSSetShader(g.cs[2].Get(), nullptr, 0);
    g.ctx->Dispatch(1, 1, 1);
    g.ctx->CSSetShader(g.cs[3].Get(), nullptr, 0);
    g.ctx->Dispatch((UINT)(bars + 63) / 64, 1, 1);
    ID3D11UnorderedAccessView* nullU[6] = {};
    ID3D11ShaderResourceView* nullS[5] = {};
    g.ctx->CSSetUnorderedAccessViews(0, 6, nullU, nullptr);
    g.ctx->CSSetShaderResources(5, 5, nullS);
    g.ctx->CSSetShader(nullptr, nullptr, 0);

    // Stats: copy this frame's, read the one from two frames ago if it's
    // ready, never wait for it.
    unsigned slot = g.frameNo % 3;
    g.ctx->CopyResource(g.staging[slot].Get(), g.stats.Get());
    if (g.frameNo >= 2) {
        unsigned old = (g.frameNo + 1) % 3;
        if (SUCCEEDED(g.ctx->Map(g.staging[old].Get(), 0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &ms))) {
            uint32_t v[2];
            memcpy(v, ms.pData, 8);
            g.ctx->Unmap(g.staging[old].Get(), 0);
            float hz, delta;
            memcpy(&hz, &v[0], 4);
            memcpy(&delta, &v[1], 4);
            if (hz > 0.f) g_dominantFreqHz.store(hz, std::memory_order_relaxed);
            g.lastMaxDeltaPx = delta;
        }
    }
    g.frameNo++;
}

// ---- Goniometer points (shared with the Direct2D fallback) --------------------------------
// Newest samples as this frame's points, plus the previous five frames' at
// falling alpha: persistence without a second render target.
int BuildGonioPoints(float* out4, int maxPoints) {
    uint32_t serial = g_gonioSerial.load(std::memory_order_acquire);
    if (serial != g.gonioSerial) {
        g.gonioSerial = serial;
        g.gonioHead = (g.gonioHead + 1) % kGonioFrames;
        std::lock_guard<std::mutex> lock(g_gonioMutex);
        // At most 512 points per frame, evenly strided.
        size_t n = g_gonioXY.size() / 2;
        size_t step = std::max<size_t>(1, n / 512);
        auto& h = g.gonioHist[g.gonioHead];
        h.clear();
        for (size_t i = 0; i < n; i += step) {
            h.push_back(g_gonioXY[2 * i]);
            h.push_back(g_gonioXY[2 * i + 1]);
        }
    }
    int count = 0;
    float alpha = 1.0f;
    for (int age = 0; age < kGonioFrames; age++) {
        const auto& h = g.gonioHist[(g.gonioHead - age + kGonioFrames) % kGonioFrames];
        // Scaled by sqrt(2) so a full-scale mono signal reaches the top.
        for (size_t i = 0; i + 1 < h.size() && count < maxPoints; i += 2) {
            out4[count * 4 + 0] = h[i] * 1.41421356f;
            out4[count * 4 + 1] = h[i + 1] * 1.41421356f;
            out4[count * 4 + 2] = alpha * 0.85f;
            out4[count * 4 + 3] = 0.f;
            count++;
        }
        alpha *= 0.55f;
    }
    return count;
}

// ---- Terminal shape ----------------------------------------------------------------------
//
// Printable ASCII (32-126) baked once, white, into a 16 x 6 atlas of cells
// exactly the size of a grid cell, so the pixel shader samples it 1:1 with
// point filtering: a pixel font stays pixel-exact, and the whole grid is one
// instanced draw. Rebaked only when the font, its size or the text rendering
// mode changes.
constexpr UINT kAtlasCols = 16, kAtlasRows = 14;  // ASCII, then 128 custom glyphs

bool EnsureTermResources() {
    if (!g.cellsDyn) {
        if (FAILED(MakeStructured(4, VIZ_TERM_MAX_CELLS, true, false, nullptr, g.cellsDyn, &g.cellsSRV, nullptr)))
            return false;
        g.cellsValid = false;
    }
    if (!VizTermEnsureFormat() || !g_d2dDevice) return false;
    uint64_t key = 1469598103934665603ull;
    for (wchar_t c : g_termFormatFont) Mix(key, (uint64_t)c);
    MixF(key, g_termFormatPx, 64.f);
    Mix(key, (uint64_t)g_termCellW * 4096u + (uint64_t)g_termCellH);
    Mix(key, g_settings.textPixel ? 1u : 0u);
    for (const auto& gl : g_termCustomGlyphs)
        for (wchar_t c : gl) Mix(key, 0x10000u + (uint64_t)c);
    if (g.glyphSRV && key == g.glyphKey) return true;
    g.glyphSRV.Reset();
    g.glyphTex.Reset();
    UINT w = kAtlasCols * (UINT)g_termCellW, h = kAtlasRows * (UINT)g_termCellH;
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = w;
    td.Height = h;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    if (FAILED(g_d3dDevice->CreateTexture2D(&td, nullptr, &g.glyphTex))) return false;
    ComPtr<IDXGISurface> surf;
    ComPtr<ID2D1DeviceContext> dc;
    ComPtr<ID2D1Bitmap1> target;
    ComPtr<ID2D1SolidColorBrush> white;
    if (FAILED(g.glyphTex.As(&surf)) ||
        FAILED(g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &dc)))
        return false;
    D2D1_BITMAP_PROPERTIES1 bp = {};
    bp.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
    bp.dpiX = bp.dpiY = 96.f;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    if (FAILED(dc->CreateBitmapFromDxgiSurface(surf.Get(), &bp, &target)) ||
        FAILED(dc->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 1.f), &white)))
        return false;
    dc->SetTarget(target.Get());
    dc->SetTextAntialiasMode(g_settings.textPixel ? D2D1_TEXT_ANTIALIAS_MODE_ALIASED
                                                  : D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    dc->BeginDraw();
    dc->Clear(D2D1::ColorF(0, 0, 0, 0));
    for (wchar_t c = 33; c < 127; c++) {
        UINT i = (UINT)(c - 32);
        // Cell column and row in the atlas (whole numbers on purpose), then pixels.
        UINT col = i % kAtlasCols, row = i / kAtlasCols;
        float x = (float)(col * (UINT)g_termCellW), y = (float)(row * (UINT)g_termCellH);
        dc->DrawText(&c, 1, g_termFormat.Get(), D2D1::RectF(x, y, x + g_termCellW, y + g_termCellH), white.Get(),
                     D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    for (UINT k = 0; k < (UINT)g_termCustomGlyphs.size() && k < 128; k++) {
        UINT i = 96 + k;  // code 128 + k
        UINT col = i % kAtlasCols, row = i / kAtlasCols;
        float x = (float)(col * (UINT)g_termCellW), y = (float)(row * (UINT)g_termCellH);
        const std::wstring& gl = g_termCustomGlyphs[k];
        dc->DrawText(gl.c_str(), (UINT32)gl.size(), g_termFormat.Get(),
                     D2D1::RectF(x, y, x + g_termCellW, y + g_termCellH), white.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }
    if (FAILED(dc->EndDraw())) {
        g.glyphTex.Reset();
        return false;
    }
    dc->SetTarget(nullptr);
    if (FAILED(g_d3dDevice->CreateShaderResourceView(g.glyphTex.Get(), nullptr, &g.glyphSRV))) {
        g.glyphTex.Reset();
        return false;
    }
    g.glyphKey = key;
    g.glyphW = w;
    g.glyphH = h;
    g.forcePresent = true;
    return true;
}

// ---- The frame -------------------------------------------------------------------------------
struct FrameInputs {
    const VizLayout* layout;
    D2D1_RECT_F bgRect;       // layout-local; valid when hasPanel
    bool hasPanel;
    float bgRadii[4];
    float sceneAlpha;
    bool dragPause;
    RGBA c1, cGrad1, c2;
    float rainbowBase;
    float scopeDisp[VIZ_WAVE_SAMPLES];
    float zones[3];
    float correlation;
};

// Bloom: shaders on first use, targets at the surface size. False leaves
// the frame without bloom.
bool EnsureFx() {
    if (!g.fxCompileTried) {
        g.fxCompileTried = true;
        g.fxCompileOk = false;
        HMODULE lib = LoadLibraryExW(L"d3dcompiler_47.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        auto fn = lib ? (PFN_D3DCompile)(void*)GetProcAddress(lib, "D3DCompile") : nullptr;
        if (!fn) return false;
        auto one = [&](const char* entry, const char* target, ComPtr<ID3DBlob>& out) {
            ComPtr<ID3DBlob> err;
            HRESULT hr = fn(kShaderSource, sizeof(kShaderSource) - 1, "tourne-table.hlsl", nullptr, nullptr, entry,
                            target, 1u << 15, 0, &out, &err);
            if (FAILED(hr) && err) Wh_Log(L"[D3D11] %S failed: %S", entry, (const char*)err->GetBufferPointer());
            return SUCCEEDED(hr);
        };
        ComPtr<ID3DBlob> vsb, psb[4];
        const char* ps[4] = {"PSBloomDown", "PSBloomH", "PSBloomV", "PSBloomComposite"};
        if (!one("VSFull", "vs_5_0", vsb)) return false;
        for (int k = 0; k < 4; k++)
            if (!one(ps[k], "ps_5_0", psb[k])) return false;
        if (FAILED(g_d3dDevice->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &g.vsFull)))
            return false;
        for (int k = 0; k < 4; k++)
            if (FAILED(g_d3dDevice->CreatePixelShader(psb[k]->GetBufferPointer(), psb[k]->GetBufferSize(), nullptr,
                                                      &g.psFx[k])))
                return false;
        D3D11_SAMPLER_DESC sd = {};
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sd.MaxLOD = D3D11_FLOAT32_MAX;
        if (FAILED(g_d3dDevice->CreateSamplerState(&sd, &g.samplerLin))) return false;
        g.fxCompileOk = true;
    }
    if (!g.fxCompileOk) return false;
    if (g.fxTex[0] && g.fxW == g.w && g.fxH == g.h) return true;
    for (int k = 0; k < 3; k++) {
        g.fxTex[k].Reset();
        g.fxRtv[k].Reset();
        g.fxSrv[k].Reset();
        D3D11_TEXTURE2D_DESC td = {};
        td.Width = k ? std::max(1u, g.w / 4) : g.w;
        td.Height = k ? std::max(1u, g.h / 4) : g.h;
        td.MipLevels = td.ArraySize = 1;
        td.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;  // smooth gradients through the blur
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        if (FAILED(g_d3dDevice->CreateTexture2D(&td, nullptr, &g.fxTex[k])) ||
            FAILED(g_d3dDevice->CreateRenderTargetView(g.fxTex[k].Get(), nullptr, &g.fxRtv[k])) ||
            FAILED(g_d3dDevice->CreateShaderResourceView(g.fxTex[k].Get(), nullptr, &g.fxSrv[k]))) {
            g.fxTex[0].Reset();
            return false;
        }
    }
    g.fxW = g.w;
    g.fxH = g.h;
    return true;
}

// Returns false if nothing could be drawn this way (the caller then uses the
// Direct2D path for this frame).
bool Render(const FrameInputs& in) {
    if (!g.ctx) return false;
    const VizLayout& L = *in.layout;
    const bool horizontal = g_settings.orientation == VizOrientation::Horizontal;
    const VizShape shape = g_settings.shape;
    const int bars = VizEffectiveBarCount();
    const float barW = std::max(1.f, VizPx((float)std::max(1.f, g_settings.barWidth)));
    const float barGap = VizPx((float)std::max(0.f, g_settings.barGap));
    const float maxSize = std::max(2.f, VizPx((float)std::max(2.f, g_settings.barMaxSize)));
    const float idleSize = VizPx((float)std::max(0.f, g_settings.barIdleSize));
    const bool roundShape = shape == VizShape::Radial || shape == VizShape::Goniometer;
    const bool term = shape == VizShape::Terminal;

    // ---- Surface: the panel, or the bars plus their bleed without one -----
    float margin = std::max(4.0f * g_dpiScale, barW);
    if (shape == VizShape::Radial) margin += maxSize * 0.15f;
    if (g_settings.style == VizStyle::Line) margin += 6.f * g_dpiScale;  // the edge glow
    const float fxGlowR = g_settings.fxGlow > 0 ? g_settings.fxGlowRadius * g_dpiScale : 0.f;
    const float fxBloomR = g_settings.fxBloom > 0 ? g_settings.fxBloomRadius * g_dpiScale : 0.f;
    const bool fxShadow = g_settings.fxShadow > 0;
    const float fxShadowReach = fxShadow ? (g_settings.fxShadowSoft + std::max(fabsf(g_settings.fxShadowX), fabsf(g_settings.fxShadowY))) *
                                               g_dpiScale + 1.f
                                         : 0.f;
    margin += std::max({fxGlowR * 2.f, fxBloomR, fxShadowReach});
    if (fabsf(g_settings.barTilt) > 0.01f)  // tilted bars lean out past the block
        margin += maxSize * fabsf(tanf(g_settings.barTilt * 3.14159265f / 180.f));  // room for the light and shade to spread
    D2D1_RECT_F content = D2D1::RectF(L.blockX - margin, L.blockY - margin, L.blockX + L.totalWidth + margin,
                                      L.blockY + L.totalHeight + margin);
    D2D1_RECT_F want = content;
    bool opaque = false;
    if (in.hasPanel) {
        const D2D1_RECT_F& b = in.bgRect;
        bool plateOpaque = (g_settings.bgBlur > 0 && g_blurredBitmap) || g_settings.bgA == 255;
        // Everything drawn has to sit inside the panel for the clip not to cut
        // it: bars and their antialiased edge, stroked shapes' half stroke.
        float bleed = roundShape ? margin : (shape == VizShape::Oscilloscope ? barW : 1.0f);
        bool inside = b.left <= L.blockX - bleed && b.top <= L.blockY - bleed &&
                      b.right >= L.blockX + L.totalWidth + bleed && b.bottom >= L.blockY + L.totalHeight + bleed;
        opaque = g_settings.opaquePanel == VizOpaquePanel::Auto && plateOpaque && inside &&
                 !g_settings.autoHideEnabled;
        want = opaque ? b
                      : D2D1::RectF(std::min(b.left - 2.f, content.left), std::min(b.top - 2.f, content.top),
                                    std::max(b.right + 2.f, content.right), std::max(b.bottom + 2.f, content.bottom));
    }
    int offX = (int)floorf(want.left), offY = (int)floorf(want.top);
    UINT w = (UINT)std::max(1.f, ceilf(want.right) - (float)offX);
    UINT h = (UINT)std::max(1.f, ceilf(want.bottom) - (float)offY);
    D2D1_RECT_F clipRect = D2D1::RectF(in.bgRect.left - offX, in.bgRect.top - offY, in.bgRect.right - offX,
                                       in.bgRect.bottom - offY);
    if (!EnsureSurface(offX, offY, w, h, opaque, clipRect, in.bgRadii, L.originX, L.originY)) return false;

    // ---- Auto-hide fully faded: one blank frame, then nothing --------------
    if (in.sceneAlpha <= 0.001f) {
        if (!g.blankPresented) {
            PresentBlank();
            g.blankPresented = true;
            VizPerf(kPerfPresents);
        } else {
            VizPerf(kPerfSkipped);
        }
        return true;
    }
    g.blankPresented = false;

    // ---- Plate -----------------------------------------------------------------
    if (in.hasPanel) {
        uint64_t key = 1469598103934665603ull;
        MixF(key, in.bgRect.left, 64.f); MixF(key, in.bgRect.top, 64.f);
        MixF(key, in.bgRect.right, 64.f); MixF(key, in.bgRect.bottom, 64.f);
        for (int i = 0; i < 4; i++) MixF(key, in.bgRadii[i], 64.f);
        Mix(key, (uint64_t)(uintptr_t)g_blurredBitmap.Get());
        Mix(key, g.opaque);
        Mix(key, g.w * 65536u + g.h);
        if (!g.plateValid || key != g.plateKey) {
            g.plateKey = key;
            if (!BakePlate(in.bgRect, in.bgRadii)) g.plateValid = false;
            g.forcePresent = true;
        }
    }

    // ---- Bar state ---------------------------------------------------------------
    // The Terminal grid is built on the CPU from the bar levels, so that
    // shape keeps the analysis there (Hybrid).
    const bool gpuWork = g_settings.workload == VizWorkload::Gpu && g_settings.engine == VizEngineKind::Precision &&
                         !in.dragPause && !term && !VizStyleNeedsCpuBars();
    bool gpuOk = false;
    if (gpuWork) {
        int st = EnsureGpuAnalysis();
        gpuOk = st > 0;
        if ((st < 0) != g_gpuAnalysisFailed && st != 0) {
            g_gpuAnalysisFailed = st < 0;
            VizPublishEngineConfig();
        }
        if (st < 0 && !g.gpuWarned) {
            g.gpuWarned = true;
            ReportSettingWarning(L"Hardware", L"Workload",
                                 L"The GPU analysis passes couldn't start on this device, so analysis runs on the "
                                 L"CPU (Hybrid) instead.");
            FlushSettingsIssues();
        }
        if (gpuOk) RunGpuAnalysis(bars, std::max(0.f, maxSize - idleSize));
    }

    // ---- Frame constants -----------------------------------------------------------
    FrameCB f = {};
    f.viewport[0] = (float)g.w;
    f.viewport[1] = (float)g.h;
    f.block[0] = L.blockX - (float)g.offX;
    f.block[1] = L.blockY - (float)g.offY;
    f.barW = barW;
    f.barGap = barGap;
    f.maxSize = maxSize;
    f.idleSize = idleSize;
    float rr[4] = {g_settings.barRadiusTL * g_dpiScale, g_settings.barRadiusTR * g_dpiScale,
                   g_settings.barRadiusBR * g_dpiScale, g_settings.barRadiusBL * g_dpiScale};
    memcpy(f.radii, rr, sizeof(rr));
    memcpy(f.dotRadii, rr, sizeof(rr));
    f.barCount = (uint32_t)bars;
    f.shape = (uint32_t)shape;
    f.vertical = horizontal ? 0u : 1u;
    f.anchor = g_settings.verticalAnchor == VizAnchor::Top ? 0u : g_settings.verticalAnchor == VizAnchor::Middle ? 1u : 2u;
    f.colorMode = (uint32_t)g_settings.colorMode;
    f.flags = (g_settings.peakHoldEnabled ? 1u : 0u) | (g_settings.beatFlashEnabled ? 2u : 0u) |
              (g_settings.oscilloscopeMultibandEnabled ? 4u : 0u);
    auto setCol = [](float* d, RGBA c) {
        d[0] = c.r / 255.f;
        d[1] = c.g / 255.f;
        d[2] = c.b / 255.f;
        d[3] = c.a / 255.f;
    };
    setCol(f.c1, in.c1);
    setCol(f.grad1, in.cGrad1);
    setCol(f.c2, in.c2);
    setCol(f.peakColor, RGBA{g_settings.peakHoldA, g_settings.peakHoldR, g_settings.peakHoldG, g_settings.peakHoldB});
    setCol(f.beatColor, RGBA{g_settings.beatFlashA, g_settings.beatFlashR, g_settings.beatFlashG, g_settings.beatFlashB});
    f.beatIntensity = g_settings.beatFlashIntensity / 100.0f;
    // Only Rainbow Cycle reads the hue clock; leaving it at 0 otherwise keeps
    // the frame constants unchanged (and un-uploaded) from frame to frame.
    f.rainbowBase = g_settings.colorMode == VizColorMode::RainbowCycle ? in.rainbowBase : 0.f;
    f.sceneAlpha = in.sceneAlpha;
    f.capThickness = std::max(1.5f, 2.0f * g_dpiScale);
    f.dotStep = barW + barGap;
    f.dotR = barW * 0.5f;
    f.maxDots = (uint32_t)ceilf(maxSize / std::max(0.5f, f.dotStep)) + 1u;
    f.dotSlots = (f.anchor == 1u) ? f.maxDots * 2u + 1u : f.maxDots;
    f.innerR = maxSize * 0.15f;
    f.strokeW = std::max(1.0f, barW);
    f.center[0] = f.block[0] + L.totalWidth * 0.5f;
    f.center[1] = f.block[1] + L.totalHeight * 0.5f;
    f.ampScale = maxSize * 0.5f;
    f.sweepLen = horizontal ? L.totalWidth : L.totalHeight;
    f.scopeCenter = horizontal ? f.block[1] + L.totalHeight * 0.5f : f.block[0] + L.totalWidth * 0.5f;
    f.sweepOrigin = horizontal ? f.block[0] : f.block[1];
    f.wstep = f.sweepLen / (float)(VIZ_WAVE_SAMPLES - 1);
    f.gonioR = maxSize * 0.85f;
    f.gonioDot = std::max(0.75f, barW * 0.2f);
    f.corrH = std::max(2.0f, 3.0f * g_dpiScale);
    f.corrY = f.center[1] + maxSize * 0.92f - f.corrH;
    f.corr = shape == VizShape::Goniometer ? in.correlation : 0.f;
    // Scope colour: the Colour Mode rules for a single line, as in 1.5.
    {
        RGBA col = in.c1;
        if (g_settings.colorMode == VizColorMode::Gradient || g_settings.colorMode == VizColorMode::Tourne)
            col = LerpColor(in.cGrad1, in.c2, 0.5f);
        else if (g_settings.colorMode == VizColorMode::RainbowCycle)
            col = HSVtoRGB(fmodf(in.rainbowBase, 360.f), 0.85f, 1.0f, in.c1.a);
        setCol(f.scopeColor, col);
    }
    // Terminal: palette, grid origin on a whole pixel (glyphs land 1:1), cell
    // size, atlas size.
    bool termReady = false;
    if (term && !in.dragPause) {
        termReady = EnsureTermResources() && !g_termGrid.cells.empty();
        VizTermPalette(f.termColors);
        f.termGeom[0] = floorf(f.block[0] + 0.5f);
        f.termGeom[1] = floorf(f.block[1] + 0.5f);
        f.termGeom[2] = (float)g_termCellW;
        f.termGeom[3] = (float)g_termCellH;
        f.termAtlas[0] = (float)g.glyphW;
        f.termAtlas[1] = (float)g.glyphH;
        f.termCols = (uint32_t)g_termGrid.cols;
        f.termRows = (uint32_t)g_termGrid.rows;
        f.termAtlasCols = kAtlasCols;
    }
    f.plateRect[0] = 0.f;
    f.plateRect[1] = 0.f;
    f.plateRect[2] = (float)g.w;
    f.plateRect[3] = (float)g.h;

    // Styles (2.1): only the constants the chosen style reads, so the others
    // stay zero and never change (or re-upload) the frame constants.
    const VizStyle style = g_settings.style;
    const bool split = style == VizStyle::SplitLR;
    f.style = (uint32_t)style;
    if (style == VizStyle::Led) {
        f.segH = std::max(2.f * g_dpiScale, roundf(barW * 0.5f));
        f.segStep = f.segH + std::max(1.f, roundf(1.5f * g_dpiScale));
        f.segs = (uint32_t)std::max(1, (int)((maxSize + f.segStep - f.segH) / f.segStep));
    } else if (style == VizStyle::Line) {
        f.subdiv = 4;
        f.glowR = 5.f * g_dpiScale;
        f.fillA = 0.35f;
    } else if (style == VizStyle::Vu) {
        // Quantised to 1/2048 of the scale, far below a pixel: a resting
        // needle leaves the frame constants, and so the frame, unchanged.
        for (int k = 0; k < 4; k++) f.vu[k] = roundf(g_vizVu[k] * 2048.f) / 2048.f;
        float mw = maxSize * 1.5f, gap = 8.f * g_dpiScale;
        f.vuBox[0] = mw;
        f.vuBox[1] = maxSize;
        f.vuBox[2] = horizontal ? mw + gap : 0.f;
        f.vuBox[3] = horizontal ? 0.f : maxSize + gap;
    } else if (style == VizStyle::Spectrogram) {
        f.vuBox[0] = 3.f * g_dpiScale;  // legend gap and width
        f.vuBox[1] = 6.f * g_dpiScale;
        f.specW = (uint32_t)g_vizSpecW;
        f.specTex = (uint32_t)kVizSpecRows;
        f.specRows = (uint32_t)std::clamp((int)maxSize, 1, kVizSpecRows);  // one row per pixel
        f.specHead = (uint32_t)g_vizSpecHead;
    }
    f.fxGlow = g_settings.fxGlow / 100.f;
    f.fxGlowR = fxGlowR;
    f.fxBloom = g_settings.fxBloom / 100.f;
    f.fxBloomR = fxBloomR;
    if (g_settings.fxOutlineWidth > 0.f && g_settings.fxOutlineA > 0) {
        f.fxLineW = g_settings.fxOutlineWidth * g_dpiScale;
        f.fxLineColor[0] = g_settings.fxOutlineR / 255.f;
        f.fxLineColor[1] = g_settings.fxOutlineG / 255.f;
        f.fxLineColor[2] = g_settings.fxOutlineB / 255.f;
        f.fxLineColor[3] = g_settings.fxOutlineA / 255.f;
    }
    {
        // Bar modifiers (2.1).
        const bool horiz = g_settings.orientation == VizOrientation::Horizontal;
        const int anchor = (int)f.anchor;
        if (horiz) f.barPivot = anchor == 0 ? f.block[1] : anchor == 1 ? f.block[1] + maxSize * 0.5f : f.block[1] + maxSize;
        else f.barPivot = anchor == 0 ? f.block[0] + maxSize : anchor == 1 ? f.block[0] + maxSize * 0.5f : f.block[0];
        uint32_t mf = 0;
        if (g_settings.barHollow) {
            mf |= 1u;
            f.hollowW = std::max(0.25f, g_settings.barHollowWidth * g_dpiScale);
        }
        if (g_settings.barDash > 0.f) {
            mf |= 2u;
            f.barDash = g_settings.barDash * g_dpiScale;
            f.barDashGap = g_settings.barDashGap * g_dpiScale;
        }
        if (fabsf(g_settings.barTilt) > 0.01f) {
            mf |= 4u;
            f.barTiltK = tanf(g_settings.barTilt * 3.14159265f / 180.f);
        }
        if (g_settings.barMirrorGap > 0.f && anchor == 1) {
            mf |= 8u;
            f.mirrorGap = g_settings.barMirrorGap * g_dpiScale;
        }
        f.modFlags = mf;
        f.ghostA = g_settings.afterimage / 100.f;
    }
    if (fxShadow) {
        f.fxShadowColor[0] = g_settings.fxShadowR / 255.f;
        f.fxShadowColor[1] = g_settings.fxShadowG / 255.f;
        f.fxShadowColor[2] = g_settings.fxShadowB / 255.f;
        f.fxShadowColor[3] = g_settings.fxShadow / 100.f;
        f.fxShadowX = g_settings.fxShadowX * g_dpiScale;
        f.fxShadowY = g_settings.fxShadowY * g_dpiScale;
        f.fxShadowSoft = g_settings.fxShadowSoft * g_dpiScale;
    }
    if (f.fxBloom > 0.f) {
        UINT qw = std::max(1u, g.w / 4), qh = std::max(1u, g.h / 4);
        f.fxTexel[0] = 1.f / qw;
        f.fxTexel[1] = 1.f / qh;
        f.fxTexel[2] = 1.f / g.w;
        f.fxTexel[3] = 1.f / g.h;
    }
    const bool refl = VizReflectionActive();
    if (refl) {
        f.reflBase = f.block[1] + maxSize;
        f.reflDir = 1.f;
        f.reflDepth = VizReflectionDepth(maxSize);
        f.reflAlpha = 0.4f;
    }

    // ---- Did anything change? ---------------------------------------------------------
    // Hashed from the CPU-side inputs first; the dynamic buffers are mapped
    // only when the frame will actually be drawn, and then only the ones
    // whose contents changed since their last upload.
    const float rangePx = std::max(0.f, maxSize - idleSize);
    float pulse = g_beatPulse.load(std::memory_order_relaxed);
    uint64_t hash = 1469598103934665603ull;
    Mix(hash, g.w * 65536u + g.h);
    Mix(hash, (uint64_t)g.offX * 65536u + (uint64_t)(uint32_t)g.offY);
    Mix(hash, g.plateKey);
    Mix(hash, g.plateValid);
    MixF(hash, in.sceneAlpha, 255.f);
    MixF(hash, f.block[0], 64.f);
    MixF(hash, f.block[1], 64.f);
    Mix(hash, (uint64_t)shape * 131u + (uint64_t)g_settings.colorMode);
    for (int k = 0; k < 4; k++) {
        MixF(hash, f.c1[k], 255.f);
        MixF(hash, f.grad1[k], 255.f);
        MixF(hash, f.c2[k], 255.f);
    }
    if (g_settings.colorMode == VizColorMode::RainbowCycle) MixF(hash, in.rainbowBase, 2.f);
    if (g_settings.beatFlashEnabled) MixF(hash, pulse, 128.f);
    Mix(hash, in.dragPause);
    Mix(hash, (uint64_t)style * 1009u + (uint64_t)lroundf(g_settings.reflection * 100.f));
    for (int k = 0; k < 4; k++) MixF(hash, f.vu[k], 2048.f);
    Mix(hash, (uint64_t)lroundf(g_settings.fxGlow * 100.f) * 1000003u + (uint64_t)lroundf(g_settings.fxBloom * 100.f) * 1009u);
    MixF(hash, g_settings.fxGlowRadius + g_settings.fxBloomRadius * 257.f, 64.f);
    MixF(hash, f.fxLineW, 64.f);
    for (int k = 0; k < 4; k++) MixF(hash, f.fxLineColor[k] + f.fxShadowColor[k] * 7.f, 1024.f);
    MixF(hash, f.fxShadowX + f.fxShadowY * 1013.f + f.fxShadowSoft * 7919.f, 64.f);

    const bool drawBars = !in.dragPause;
    const bool cpuBars = !gpuOk && drawBars;
    // Bars: what is drawn, to a quarter pixel. The same key decides whether
    // barsDyn needs new contents: a change smaller than that can't be seen.
    uint64_t barsKey = 0, globalsKey = 0;
    // Globals hold only what the shader reads: the beat pulse when Beat Flash
    // is on, the zone energies when the multiband colour is (both are gated
    // by fFlags there). Anything else would change, and re-upload, every frame.
    const float glPulse = g_settings.beatFlashEnabled ? pulse : 0.f;
    const bool glZones = g_settings.oscilloscopeMultibandEnabled;
    const float glZ[3] = {glZones ? in.zones[0] : 0.f, glZones ? in.zones[1] : 0.f, glZones ? in.zones[2] : 0.f};
    // Stereo Field puts the left channel's levels where the bars go and the
    // right channel's where the peak caps go.
    const float* barLv = split ? g_vizSplitL : g_vizPeak;
    // Afterimage carries its trail levels where the peak hold usually goes.
    const float* barHv = split ? g_vizSplitR : (g_settings.afterimage > 0 ? g_vizGhost : g_vizPeakHold);
    if (cpuBars) {
        barsKey = 1469598103934665603ull;
        Mix(barsKey, (uint64_t)bars * 2u + (split ? 1u : 0u));
        const bool caps = g_settings.peakHoldEnabled || split || g_settings.afterimage > 0;
        for (int i = 0; i < bars; i++) {
            MixF(barsKey, std::max(0.f, barLv[i]) * rangePx, 4.f);
            if (caps) MixF(barsKey, barHv[i] * rangePx, 4.f);
        }
        Mix(hash, barsKey);
        globalsKey = 1469598103934665603ull;
        uint32_t bits[4];
        memcpy(&bits[0], &glPulse, 4);
        memcpy(&bits[1], glZ, 12);
        for (uint32_t b : bits) Mix(globalsKey, b);
        if (g_settings.oscilloscopeMultibandEnabled)
            for (int z = 0; z < 3; z++) MixF(hash, in.zones[z], 64.f);
    } else if (gpuOk) {
        // GPU bars aren't visible from here; the stats block says how far the
        // last frames moved, two frames late.
        Mix(hash, g.lastMaxDeltaPx >= 0.25f ? (uint64_t)g.frameNo : 0ull);
    }
    uint64_t waveKey = 0;
    if (shape == VizShape::Oscilloscope && drawBars) {
        waveKey = 1469598103934665603ull;
        for (int i = 0; i < VIZ_WAVE_SAMPLES; i++) MixF(waveKey, in.scopeDisp[i] * f.ampScale, 4.f);
        Mix(hash, waveKey);
    }
    // Terminal: VizBuildTermGrid bumps g_termGridSerial only when a cell
    // actually changed, so one number stands for all 65,536 of them.
    const uint64_t cellsShape = ((uint64_t)(uint32_t)g_termGrid.cols << 32) | (uint32_t)g_termGrid.rows;
    if (termReady) {
        Mix(hash, g.glyphKey);
        Mix(hash, cellsShape);
        Mix(hash, (uint64_t)g_termGridSerial);
        for (int c = 0; c < 5; c++)
            for (int k = 0; k < 4; k++) MixF(hash, f.termColors[c][k], 255.f);
    }
    const bool gonio = shape == VizShape::Goniometer && drawBars;
    const uint32_t gonioSerial = g_gonioSerial.load(std::memory_order_acquire);
    if (gonio) {
        Mix(hash, gonioSerial);
        MixF(hash, in.correlation, 256.f);
    }

    const bool sparks = style == VizStyle::Particles && drawBars;
    if (sparks) {
        Mix(hash, g_vizSparkSerial);
        Mix(hash, (uint64_t)g_vizSparkCount);
    }
    const bool spec = style == VizStyle::Spectrogram && drawBars && g_vizSpecW > 0;
    if (spec) Mix(hash, g_vizSpecSerial);

    if (!g.forcePresent && hash == g.lastHash) {  // nothing changed: no upload, no draw, no present
        VizPerf(kPerfSkipped);
        return true;
    }
    g.lastHash = hash;
    g.forcePresent = false;

    // ---- Uploads (CPU paths), only what changed ---------------------------------------
    D3D11_MAPPED_SUBRESOURCE ms;
    if (cpuBars) {
        if (!g.uploadsValid || barsKey != g.barsKey) {
            if (SUCCEEDED(g.ctx->Map(g.barsDyn.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
                float* p = (float*)ms.pData;
                for (int i = 0; i < bars; i++) {
                    p[2 * i] = std::max(0.f, barLv[i]);
                    p[2 * i + 1] = barHv[i];
                }
                g.ctx->Unmap(g.barsDyn.Get(), 0);
                VizPerf(kPerfMaps);
                g.barsKey = barsKey;
            }
        }
        if (!g.uploadsValid || globalsKey != g.globalsKey) {
            if (SUCCEEDED(g.ctx->Map(g.globalsDyn.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
                float gl[16] = {glPulse, 0, 0, 0, glZ[0], glZ[1], glZ[2], 0};
                memcpy(ms.pData, gl, sizeof(gl));
                g.ctx->Unmap(g.globalsDyn.Get(), 0);
                VizPerf(kPerfMaps);
                g.globalsKey = globalsKey;
            }
        }
    }
    if (waveKey && (!g.uploadsValid || waveKey != g.waveKey)) {
        if (SUCCEEDED(g.ctx->Map(g.waveDyn.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            memcpy(ms.pData, in.scopeDisp, sizeof(float) * VIZ_WAVE_SAMPLES);
            g.ctx->Unmap(g.waveDyn.Get(), 0);
            VizPerf(kPerfMaps);
            g.waveKey = waveKey;
        }
    }
    if (termReady && (!g.cellsValid || g.cellsSerial != g_termGridSerial || g.cellsShape != cellsShape)) {
        // Only the cells with a glyph, each as char | colour << 8 | index << 16:
        // blanks cost neither upload nor a vertex-shader instance.
        const size_t n = std::min(g_termGrid.cells.size(), (size_t)VIZ_TERM_MAX_CELLS);
        if (SUCCEEDED(g.ctx->Map(g.cellsDyn.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            uint32_t* out = (uint32_t*)ms.pData;
            const uint32_t* src = g_termGrid.cells.data();
            UINT k = 0;
            for (size_t i = 0; i < n; i++) {  // branch-free: k <= i, always in bounds
                uint32_t cell = src[i];
                out[k] = (cell & 0xFFFFu) | ((uint32_t)i << 16);
                k += ((cell & 255u) > 32u) ? 1u : 0u;
            }
            g.ctx->Unmap(g.cellsDyn.Get(), 0);
            VizPerf(kPerfMaps);
            g.termCount = k;
            g.cellsSerial = g_termGridSerial;
            g.cellsShape = cellsShape;
            g.cellsValid = true;
        }
    }
    int points = g.pointsCount;
    if (gonio && (!g.uploadsValid || gonioSerial != g.pointsSerial)) {
        // The persistence history only moves when the engine publishes a new
        // block, so the points only need rebuilding then.
        if (SUCCEEDED(g.ctx->Map(g.pointsDyn.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            points = BuildGonioPoints((float*)ms.pData, kMaxPoints);
            g.ctx->Unmap(g.pointsDyn.Get(), 0);
            VizPerf(kPerfMaps);
            g.pointsCount = points;
            g.pointsSerial = gonioSerial;
        }
    }
    if (sparks && (!g.uploadsValid || g.sparkSerial != g_vizSparkSerial)) {
        if (SUCCEEDED(g.ctx->Map(g.pointsDyn.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            memcpy(ms.pData, g_vizSparkBuf, sizeof(float) * 4 * (size_t)g_vizSparkCount);
            g.ctx->Unmap(g.pointsDyn.Get(), 0);
            VizPerf(kPerfMaps);
            g.sparkSerial = g_vizSparkSerial;
            g.pointsSerial = 0xFFFFFFFFu;  // the Goniometer's points are gone
        }
    }
    bool specReady = false;
    if (spec) {
        const int W = g_vizSpecW, R = kVizSpecRows;
        if (!g.specTex || g.specW != W) {
            g.specTex.Reset();
            g.specSRV.Reset();
            D3D11_TEXTURE2D_DESC td = {};
            td.Width = (UINT)W;
            td.Height = (UINT)R;
            td.MipLevels = td.ArraySize = 1;
            td.Format = DXGI_FORMAT_R8_UNORM;
            td.SampleDesc.Count = 1;
            td.Usage = D3D11_USAGE_DEFAULT;
            td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            if (SUCCEEDED(g_d3dDevice->CreateTexture2D(&td, nullptr, &g.specTex)) &&
                SUCCEEDED(g_d3dDevice->CreateShaderResourceView(g.specTex.Get(), nullptr, &g.specSRV))) {
                g.specW = W;
                g.specSerial = g_vizSpecSerial - (uint32_t)R;  // everything is new
            } else {
                g.specTex.Reset();
                g.specSRV.Reset();
            }
        }
        if (g.specTex) {
            // Only the rows pushed since the last upload: a row is a few
            // hundred bytes, the whole history a few hundred kilobytes.
            uint32_t fresh = g_vizSpecSerial - g.specSerial;
            if (fresh >= (uint32_t)R) {
                g.ctx->UpdateSubresource(g.specTex.Get(), 0, nullptr, g_vizSpecRing.data(), (UINT)W, 0);
            } else {
                for (uint32_t k = fresh; k > 0; k--) {
                    int row = (g_vizSpecHead - (int)k + 1 + R) % R;
                    D3D11_BOX box = {0, (UINT)row, 0, (UINT)W, (UINT)row + 1, 1};
                    g.ctx->UpdateSubresource(g.specTex.Get(), 0, &box, &g_vizSpecRing[(size_t)row * W], (UINT)W, 0);
                }
            }
            if (fresh) VizPerf(kPerfMaps);
            g.specSerial = g_vizSpecSerial;
            g.specHead = g_vizSpecHead;
            specReady = true;
        }
    }
    g.uploadsValid = true;

    if (!g.frameCBValid || memcmp(&f, &g.frameCBLast, sizeof(f)) != 0) {
        if (SUCCEEDED(g.ctx->Map(g.frameCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            memcpy(ms.pData, &f, sizeof(f));
            g.ctx->Unmap(g.frameCB.Get(), 0);
            VizPerf(kPerfMaps);
            memcpy(&g.frameCBLast, &f, sizeof(f));  // bytes, padding included, for the memcmp above
            g.frameCBValid = true;
        }
    }

    // ---- Draw -----------------------------------------------------------------------
    // The plate is the size of the surface and drawn first, 1:1, without
    // blending, so it writes every pixel (transparent outside the panel):
    // a clear before it would only be overwritten.
    const bool drawPlate = in.hasPanel && g.plateValid;
    ID3D11RenderTargetView* rtv = g.rtv.Get();
    g.ctx->OMSetRenderTargets(1, &rtv, nullptr);
    if (!drawPlate) {
        const float zero[4] = {0, 0, 0, 0};
        g.ctx->ClearRenderTargetView(rtv, zero);
    }
    D3D11_VIEWPORT vp = {0, 0, (float)g.w, (float)g.h, 0, 1};
    g.ctx->RSSetViewports(1, &vp);
    g.ctx->RSSetState(g.raster.Get());
    g.ctx->OMSetDepthStencilState(nullptr, 0);
    g.ctx->IASetInputLayout(nullptr);
    g.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    g.ctx->VSSetShader(g.vs.Get(), nullptr, 0);
    g.ctx->PSSetShader(g.ps.Get(), nullptr, 0);
    ID3D11ShaderResourceView* vsSrv[4] = {gpuOk ? g.barsGpuSRV.Get() : g.barsDynSRV.Get(),
                                          gpuOk ? g.globalsGpuSRV.Get() : g.globalsDynSRV.Get(), g.waveSRV.Get(),
                                          g.pointsSRV.Get()};
    g.ctx->VSSetShaderResources(0, 4, vsSrv);
    ID3D11ShaderResourceView* psSrv[5] = {nullptr, nullptr, nullptr, nullptr, g.plateSRV.Get()};
    g.ctx->PSSetShaderResources(0, 5, psSrv);
    if (termReady) {
        ID3D11ShaderResourceView* cells = g.cellsSRV.Get();
        ID3D11ShaderResourceView* glyphs = g.glyphSRV.Get();
        g.ctx->VSSetShaderResources(10, 1, &cells);
        g.ctx->PSSetShaderResources(11, 1, &glyphs);
    }
    if (specReady) {
        ID3D11ShaderResourceView* sv = g.specSRV.Get();
        g.ctx->PSSetShaderResources(12, 1, &sv);
    }
    ID3D11SamplerState* smp = g.sampler.Get();
    g.ctx->PSSetSamplers(0, 1, &smp);
    ID3D11Buffer* fcb = g.frameCB.Get();
    g.ctx->VSSetConstantBuffers(0, 1, &fcb);
    g.ctx->PSSetConstantBuffers(0, 1, &fcb);
    auto draw = [&](Pass p, UINT count) {
        if (!count) return;
        ID3D11Buffer* pcb = g.passCB[p].Get();
        g.ctx->VSSetConstantBuffers(1, 1, &pcb);
        g.ctx->DrawInstanced(4, count, 0, 0);
    };
    if (drawPlate) {
        g.ctx->OMSetBlendState(g.blendOff.Get(), nullptr, 0xffffffff);
        draw(kPlate, 1);
    }
    g.ctx->OMSetBlendState(g.blend.Get(), nullptr, 0xffffffff);
    // Bloom: everything after the plate goes into the scene texture first.
    const bool bloom = drawBars && f.fxBloom > 0.f && EnsureFx();
    if (bloom) {
        const float zero[4] = {0, 0, 0, 0};
        ID3D11RenderTargetView* srt = g.fxRtv[0].Get();
        g.ctx->OMSetRenderTargets(1, &srt, nullptr);
        g.ctx->ClearRenderTargetView(srt, zero);
    }
    // Reflection first, under the bars it mirrors.
    auto drawRefl = [&](Pass p, UINT count) {
        if (!count) return;
        ID3D11Buffer* pcb = g.passCBRefl[p].Get();
        g.ctx->VSSetConstantBuffers(1, 1, &pcb);
        g.ctx->DrawInstanced(4, count, 0, 0);
    };
    const UINT lineCount = bars > 1 ? (UINT)(bars - 1) * f.subdiv : 0u;
    if (drawBars && refl) {
        if (style == VizStyle::Led) drawRefl(kLed, (UINT)bars * f.segs);
        else if (style == VizStyle::Line) drawRefl(kLine, lineCount);
        else if (shape == VizShape::Dots) drawRefl(kDots, (UINT)bars * f.dotSlots);
        else {
            if (g_settings.afterimage > 0) drawRefl(kGhost, (UINT)bars);
            drawRefl(kBars, (UINT)bars);
            if (g_settings.peakHoldEnabled) drawRefl(kCaps, (UINT)bars);
        }
    }
    bool styled = drawBars;
    if (drawBars) {
        switch (style) {
            case VizStyle::Led: draw(kLed, (UINT)bars * f.segs); break;
            case VizStyle::Line: draw(kLine, lineCount); break;
            case VizStyle::Bloom: draw(kBloom, (UINT)bars); break;
            case VizStyle::Spectrogram:
                if (specReady) draw(kSpectro, 2);
                break;
            case VizStyle::Vu: draw(kVu, 64); break;
            case VizStyle::SplitLR: draw(kSplit, (UINT)bars * 2u); break;
            default: styled = false; break;
        }
    }
    if (drawBars && !styled) {
        switch (shape) {
            case VizShape::Dots: draw(kDots, (UINT)bars * f.dotSlots); break;
            case VizShape::Radial: draw(kRadial, (UINT)bars); break;
            case VizShape::Terminal:
                if (termReady) draw(kTerm, g.termCount);
                break;
            case VizShape::Oscilloscope: draw(kScope, VIZ_WAVE_SAMPLES - 1); break;
            case VizShape::Goniometer:
                draw(kGonio, (UINT)points);
                draw(kCorr, 2);
                break;
            default:
                if (g_settings.afterimage > 0) draw(kGhost, (UINT)bars);
                draw(kBars, (UINT)bars);
                if (g_settings.peakHoldEnabled) draw(kCaps, (UINT)bars);
                break;
        }
        if (sparks) draw(kSpark, (UINT)g_vizSparkCount);
    }
    if (bloom) {
        // Down to a quarter, blur across, blur down, then the scene plus its
        // light over the plate.
        ID3D11ShaderResourceView* none[2] = {};
        auto pass = [&](int rt, int src, int ps, UINT vw, UINT vh) {
            g.ctx->PSSetShaderResources(13, 2, none);
            ID3D11RenderTargetView* t = rt < 0 ? rtv : g.fxRtv[rt].Get();
            g.ctx->OMSetRenderTargets(1, &t, nullptr);
            D3D11_VIEWPORT v = {0, 0, (float)vw, (float)vh, 0, 1};
            g.ctx->RSSetViewports(1, &v);
            ID3D11ShaderResourceView* srcs[2] = {g.fxSrv[src].Get(), rt < 0 ? g.fxSrv[1].Get() : nullptr};
            g.ctx->PSSetShaderResources(13, 2, srcs);
            g.ctx->PSSetShader(g.psFx[ps].Get(), nullptr, 0);
            g.ctx->Draw(3, 0);
        };
        const UINT qw = std::max(1u, g.w / 4), qh = std::max(1u, g.h / 4);
        g.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g.ctx->VSSetShader(g.vsFull.Get(), nullptr, 0);
        ID3D11SamplerState* lin = g.samplerLin.Get();
        g.ctx->PSSetSamplers(1, 1, &lin);
        g.ctx->OMSetBlendState(g.blendOff.Get(), nullptr, 0xffffffff);
        pass(1, 0, 0, qw, qh);
        pass(2, 1, 1, qw, qh);
        pass(1, 2, 2, qw, qh);
        g.ctx->OMSetBlendState(g.blend.Get(), nullptr, 0xffffffff);
        pass(-1, 0, 3, g.w, g.h);
        g.ctx->PSSetShaderResources(13, 2, none);
    }
    ID3D11ShaderResourceView* nullSrv[5] = {};
    g.ctx->VSSetShaderResources(0, 4, nullSrv);
    g.ctx->PSSetShaderResources(0, 5, nullSrv);
    if (termReady) {
        g.ctx->VSSetShaderResources(10, 1, nullSrv);
        g.ctx->PSSetShaderResources(11, 1, nullSrv);
    }
    if (specReady) g.ctx->PSSetShaderResources(12, 1, nullSrv);

    HRESULT hr = g.sc->Present(0, 0);
    VizCheckDeviceLost(S_OK, hr);
    VizPerf(kPerfPresents);
    return true;
}

}  // namespace ttgfx

// ---- Shared by both renderers ---------------------------------------------------------

// Publishes the bounds of what is actually visible, for the occlusion check
// (see the note where 1.4 introduced this).
bool VizProgressRect(const VizLayout& layout, D2D1_RECT_F* out);

void VizPublishDrawRect(const VizLayout& layout) {
    const POINT layoutOrigin = VizLayoutOrigin();
    int virtualScreenX = layoutOrigin.x;
    int virtualScreenY = layoutOrigin.y;
    {
        // Click to Seek: the bar, made at least 10 px tall to hit.
        D2D1_RECT_F pr;
        bool ok = g_settings.progressEnabled && g_settings.progressSeek && VizProgressRect(layout, &pr);
        if (ok) {
            float cy = (pr.top + pr.bottom) * 0.5f, hh = std::max((pr.bottom - pr.top) * 0.5f, 5.f * g_dpiScale);
            g_seekRectL.store((LONG)lroundf(layout.originX + pr.left) + virtualScreenX, std::memory_order_relaxed);
            g_seekRectR.store((LONG)lroundf(layout.originX + pr.right) + virtualScreenX, std::memory_order_relaxed);
            g_seekRectT.store((LONG)lroundf(layout.originY + cy - hh) + virtualScreenY, std::memory_order_relaxed);
            g_seekRectB.store((LONG)lroundf(layout.originY + cy + hh) + virtualScreenY, std::memory_order_relaxed);
        }
        g_seekRectValid.store(ok, std::memory_order_relaxed);
    }
    float padL = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingL) : 0.f;
    float padR = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingR) : 0.f;
    float padT = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingT) : 0.f;
    float padB = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingB) : 0.f;
    float visL = layout.originX + layout.blockX - padL;
    float visT = layout.originY + layout.blockY - padT;
    float visR = layout.originX + layout.blockX + layout.totalWidth + padR;
    float visB = layout.originY + layout.blockY + layout.totalHeight + padB;
    LONG l = (LONG)visL + virtualScreenX, t = (LONG)visT + virtualScreenY;
    LONG r = (LONG)visR + virtualScreenX, b = (LONG)visB + virtualScreenY;
    bool moved = !g_drawRectValid.load(std::memory_order_relaxed) ||
                 l != g_drawRectL.load(std::memory_order_relaxed) ||
                 t != g_drawRectT.load(std::memory_order_relaxed) ||
                 r != g_drawRectR.load(std::memory_order_relaxed) ||
                 b != g_drawRectB.load(std::memory_order_relaxed);
    g_drawRectL.store(l, std::memory_order_relaxed);
    g_drawRectT.store(t, std::memory_order_relaxed);
    g_drawRectR.store(r, std::memory_order_relaxed);
    g_drawRectB.store(b, std::memory_order_relaxed);
    g_drawRectValid.store(true, std::memory_order_relaxed);
    // Media controls anchored to the panel follow it (a drag, a settings
    // change, a nudge). Only on an actual move, so a still panel costs nothing.
    if (moved && g_mediaWnd && g_settings.mediaControlsEnabled && g_settings.mediaAnchor != VizMediaAnchor::Screen)
        PostMessage(g_mediaWnd, WM_APP_MEDIA_REPAINT, 0, 0);
}

// The background panel's rectangle (layout-local) and corner radii, with the
// same guard as the Direct2D path against negative padding inverting it.
void VizPanelRect(const VizLayout& layout, D2D1_RECT_F* out, float radii[4]) {
    float padL = VizPx((float)g_settings.bgPaddingL);
    float padR = VizPx((float)g_settings.bgPaddingR);
    float padT = VizPx((float)g_settings.bgPaddingT);
    float padB = VizPx((float)g_settings.bgPaddingB);
    D2D1_RECT_F r = D2D1::RectF(layout.blockX - padL, layout.blockY - padT, layout.blockX + layout.totalWidth + padR,
                                layout.blockY + layout.totalHeight + padB);
    if (r.right - r.left < 1.0f) {
        float mid = (r.left + r.right) * 0.5f;
        r.left = mid - 0.5f;
        r.right = mid + 0.5f;
    }
    if (r.bottom - r.top < 1.0f) {
        float mid = (r.top + r.bottom) * 0.5f;
        r.top = mid - 0.5f;
        r.bottom = mid + 0.5f;
    }
    *out = r;
    radii[0] = g_settings.bgRadiusTL * g_dpiScale;
    radii[1] = g_settings.bgRadiusTR * g_dpiScale;
    radii[2] = g_settings.bgRadiusBR * g_dpiScale;
    radii[3] = g_settings.bgRadiusBL * g_dpiScale;
}

// The oscilloscope trace after Oscilloscope Damping (1.5 semantics, moved out
// of the Direct2D path so both renderers ease it the same way).
float g_scopeDisp[VIZ_WAVE_SAMPLES] = {};
void VizUpdateScopeTrace() {
    float waveSnap[VIZ_WAVE_SAMPLES];
    ReadWaveform(waveSnap);
    float scopeEase = (g_settings.oscilloscopeDamping > 0) ? 1.0f - (g_settings.oscilloscopeDamping / 100.0f) * 0.95f
                                                           : 1.0f;
    if (scopeEase < 1.0f) scopeEase = VizEaseForFrame(scopeEase);
    for (int w = 0; w < VIZ_WAVE_SAMPLES; w++) g_scopeDisp[w] += (waveSnap[w] - g_scopeDisp[w]) * scopeEase;
}

// Energy per EQ zone, for the multiband oscilloscope colour.
void VizZoneEnergies(float out[3]) {
    out[0] = out[1] = out[2] = 0.f;
    if (g_settings.engine == VizEngineKind::Precision) {
        for (int z = 0; z < 3; z++) out[z] = g_drawBands.zone[z];
        return;
    }
    float bandsSnap[VIZ_NUM_BANDS];
    ReadBands(bandsSnap);
    for (int b = 0; b < VIZ_NUM_BANDS; b++) out[VIZ_BAND_EQ_ZONE[b]] += bandsSnap[b];
}

// Colour Mode inputs, resolved the way the Direct2D path does it.
void VizResolveColors(RGBA* c1, RGBA* cGrad1, RGBA* c2) {
    *c1 = {g_settings.colorA, g_settings.colorR, g_settings.colorG, g_settings.colorB};
    if (g_settings.colorMode == VizColorMode::Accent) {
        DWORD dw = GetWindowsAccentColor();
        *c1 = {0xFF, (BYTE)((dw >> 16) & 0xFF), (BYTE)((dw >> 8) & 0xFF), (BYTE)(dw & 0xFF)};
    } else if (g_settings.colorMode == VizColorMode::AlbumArt || g_settings.colorMode == VizColorMode::DynamicAlbum) {
        DWORD dw = VizAlbumColorShown(0);
        *c1 = {0xFF, (BYTE)((dw >> 16) & 0xFF), (BYTE)((dw >> 8) & 0xFF), (BYTE)(dw & 0xFF)};
    }
    *c2 = {g_settings.grad2A, g_settings.grad2R, g_settings.grad2G, g_settings.grad2B};
    *cGrad1 = {g_settings.grad1A, g_settings.grad1R, g_settings.grad1G, g_settings.grad1B};
    if (g_settings.colorMode == VizColorMode::Tourne) {
        *cGrad1 = {255, 20, 184, 166};
        *c2 = {255, 200, 29, 51};
    }
    if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
        DWORD dw = VizAlbumColorShown(1);
        *c2 = {0xFF, (BYTE)((dw >> 16) & 0xFF), (BYTE)((dw >> 8) & 0xFF), (BYTE)(dw & 0xFF)};
        *cGrad1 = *c1;
    }
}

// ---- Text overlays --------------------------------------------------------------------
//
// The Now Playing label and the readout, worked out once per frame as plain
// values (VizBuildTextFrame) and drawn from them (VizDrawTextOverlays). The
// split is what lets the Direct3D 11 renderer redraw the text surface only
// when one of those values changes.
struct VizTextFrame {
    std::wstring np;            // one line: "Artist - Title"
    std::wstring npTitle, npArtist;
    float npAlpha = 0.f;
    float progress = -1.f;      // track position 0..1, or -1 for no bar
    std::wstring pf;
    bool pfWide = false;  // a loudness readout: wider box, never wrapped
};

static void AppendLufs(std::wstring& s, const wchar_t* label, double v) {
    wchar_t b[32];
    if (std::isfinite(v) && v > -70.0)
        swprintf_s(b, L"%s %.1f", label, v);
    else
        swprintf_s(b, L"%s --", label);
    if (!s.empty()) s += L"  ";
    s += b;
}

// The readout as shown. Rebuilt ten times a second, as a hardware meter
// refreshes its display: momentary loudness only moves every 100 ms anyway
// (its blocks are 100 ms), while the dominant frequency and the correlation
// change on every analysis, which used to redraw and re-present the whole
// text surface on nearly every frame. The frequency also holds within one
// display step (1 Hz below 1 kHz, 0.1 kHz above) so a tone sitting between
// two values doesn't flicker; what is shown is never more than one step
// and 100 ms away from the live value.
struct VizReadoutCache {
    std::wstring text;
    bool wide = false;
    bool valid = false;
    int mode = -1;
    ULONGLONG at = 0;
    float hzShown = 0.f;
};
VizReadoutCache g_readoutCache;
constexpr ULONGLONG kVizReadoutMs = 100;

static void VizFormatReadout(VizReadoutCache& rc) {
    float hz = g_dominantFreqHz.load(std::memory_order_relaxed);
    if (hz <= 0.f) {
        rc.hzShown = 0.f;
    } else {
        float step = (rc.hzShown >= 1000.f) ? 100.f : 1.f;  // one step of the shown format
        if (rc.hzShown <= 0.f || (hz >= 1000.f) != (rc.hzShown >= 1000.f) || fabsf(hz - rc.hzShown) >= step)
            rc.hzShown = hz;
    }
    wchar_t freq[32] = L"";
    if (rc.hzShown > 0.f) {
        if (rc.hzShown >= 1000.f) swprintf_s(freq, L"%.1f kHz", rc.hzShown / 1000.f);
        else swprintf_s(freq, L"%.0f Hz", rc.hzShown);
    }
    VizReadout r = g_settings.readout;
    std::wstring& s = rc.text;
    s.clear();
    if (r == VizReadout::Frequency) {
        s = freq;
        rc.wide = false;
        return;
    }
    VizMeterValues m;
    {
        std::lock_guard<std::mutex> lock(g_meterMutex);
        m = g_meters;
    }
    if (r == VizReadout::Both && freq[0]) s = freq;
    AppendLufs(s, L"M", m.momentary);
    AppendLufs(s, L"S", m.shortTerm);
    AppendLufs(s, L"I", m.integrated);
    s += L" LUFS";
    if (r == VizReadout::LoudnessFull) {
        wchar_t b[96];
        double plr = (std::isfinite(m.truePeak) && std::isfinite(m.integrated) && m.integrated > -70.0)
                         ? m.truePeak - m.integrated
                         : NAN;
        if (std::isfinite(m.truePeak) && m.truePeak > -100.0)
            swprintf_s(b, L"  TP %.1f dBTP", m.truePeak);
        else
            swprintf_s(b, L"  TP --");
        s += b;
        if (std::isfinite(plr)) swprintf_s(b, L"  PLR %.1f", plr);
        else swprintf_s(b, L"  PLR --");
        s += b;
        swprintf_s(b, L"  r %+.2f", m.correlation);
        s += b;
    }
    rc.wide = true;
}

void VizBuildTextFrame(VizTextFrame& t) {
    // Cleared rather than replaced, so a frame kept from tick to tick (the
    // Direct3D 11 path) reuses its strings' storage instead of reallocating.
    t.np.clear();
    t.npTitle.clear();
    t.npArtist.clear();
    t.npAlpha = 0.f;
    t.progress = -1.f;
    t.pf.clear();
    t.pfWide = false;
    if (g_settings.nowPlayingEnabled && g_dwriteTextFormat && g_nowPlayingBrush) {
        ULONGLONG changedAt = g_nowPlayingChangedTick.load(std::memory_order_relaxed);
        ULONGLONG npElapsed = GetTickCount64() - changedAt;
        ULONGLONG showMs = (ULONGLONG)(std::max(0.f, g_settings.nowPlayingDisplaySeconds) * 1000.f);
        constexpr ULONGLONG kNpFadeMs = 800;
        if (changedAt != 0 && showMs == 0)
            t.npAlpha = 1.0f;  // Display Seconds 0: always shown, a widget rather than a toast
        else if (changedAt != 0 && npElapsed < showMs + kNpFadeMs)
            t.npAlpha = (npElapsed < showMs) ? 1.0f : 1.0f - (float)(npElapsed - showMs) / (float)kNpFadeMs;
        if (t.npAlpha > 0.01f) {
            std::lock_guard<std::mutex> lock(g_nowPlayingMutex);
            t.np = g_nowPlayingDisplay;
            t.npTitle = g_nowPlayingTitle;
            t.npArtist = g_nowPlayingArtist;
        }
    }
    if (g_settings.progressEnabled && g_progressBrush) t.progress = VizTrackProgress();
    if (g_settings.peakFreqEnabled && g_dwriteTextFormat && g_nowPlayingBrush) {
        VizReadoutCache& rc = g_readoutCache;
        ULONGLONG now = GetTickCount64();
        if (!rc.valid || rc.mode != (int)g_settings.readout || now - rc.at >= kVizReadoutMs) {
            VizFormatReadout(rc);
            rc.valid = true;
            rc.mode = (int)g_settings.readout;
            rc.at = now;
        }
        t.pf = rc.text;
        t.pfWide = rc.wide;
    } else {
        g_readoutCache.valid = false;
    }
}

// Rough single-line width for the room a wide readout needs, used when sizing
// the layout before any text has been measured.
float VizReadoutWidthEstimate() {
    float fontPx = (float)std::max(6.f, g_settings.nowPlayingFontSize) * g_dpiScale;
    int chars = 0;
    switch (g_settings.readout) {
        case VizReadout::Loudness: chars = 32; break;
        case VizReadout::LoudnessFull: chars = 70; break;
        case VizReadout::Both: chars = 42; break;
        default: return 0.f;
    }
    return fontPx * 0.58f * chars;
}

// Where the track progress bar goes, layout-local. False when it isn't shown.
bool VizProgressRect(const VizLayout& layout, D2D1_RECT_F* out) {
    float h = (float)std::max(1.f, g_settings.progressHeight) * g_dpiScale;
    float gap = (float)g_settings.progressGap * g_dpiScale;
    D2D1_RECT_F panel;
    float radii[4];
    VizPanelRect(layout, &panel, radii);
    bool hasPanel = g_settings.backgroundEnabled;
    float l = hasPanel ? panel.left : layout.blockX, r = hasPanel ? panel.right : layout.blockX + layout.totalWidth;
    float top = hasPanel ? panel.top : layout.blockY, bottom = hasPanel ? panel.bottom : layout.blockY + layout.totalHeight;
    float y;
    switch (g_settings.progressPlacement) {
        case VizProgressPlacement::Above: y = top - gap - h; break;
        case VizProgressPlacement::PanelBottom:
            // Inside the panel, under the bars, as wide as the bars.
            l = layout.blockX;
            r = layout.blockX + layout.totalWidth;
            y = layout.blockY + layout.totalHeight + gap;
            break;
        default: y = bottom + gap; break;
    }
    // Whole pixels: a 2 px bar on a half pixel would read as a 3 px smear.
    *out = D2D1::RectF(roundf(l), roundf(y), roundf(r), roundf(y) + roundf(h));
    return out->right > out->left;
}

void VizDrawTextOverlays(const VizTextFrame& t, const VizLayout& layout, bool smooth) {
    const float blockX = layout.blockX, blockY = layout.blockY;
    const float totalWidth = layout.totalWidth, totalHeight = layout.totalHeight;
    const bool pixel = g_settings.textPixel;
    // Pixel-sharp text: no antialiasing, and every box on a whole pixel, so a
    // pixel font at its design size lands exactly on the grid.
    g_dc->SetTextAntialiasMode(pixel ? D2D1_TEXT_ANTIALIAS_MODE_ALIASED : D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
    auto snap = [&](D2D1_RECT_F r) {
        if (!pixel && !g_settings.pixelSnap) return r;
        float dx = roundf(r.left) - r.left, dy = roundf(r.top) - r.top;
        return D2D1::RectF(r.left + dx, r.top + dy, r.right + dx, r.bottom + dy);
    };

    if (t.progress >= 0.f && g_progressBrush) {
        D2D1_RECT_F pr;
        if (VizProgressRect(layout, &pr)) {
            g_progressBrush->SetColor(D2D1::ColorF(g_settings.progressTrackR / 255.f, g_settings.progressTrackG / 255.f,
                                                   g_settings.progressTrackB / 255.f, g_settings.progressTrackA / 255.f));
            g_dc->FillRectangle(pr, g_progressBrush.Get());
            float fillR = pr.left + roundf((pr.right - pr.left) * std::clamp(t.progress, 0.f, 1.f));
            if (fillR > pr.left) {
                g_progressBrush->SetColor(D2D1::ColorF(g_settings.progressR / 255.f, g_settings.progressG / 255.f,
                                                       g_settings.progressB / 255.f, g_settings.progressA / 255.f));
                g_dc->FillRectangle(D2D1::RectF(pr.left, pr.top, fillR, pr.bottom), g_progressBrush.Get());
            }
        }
    }

    if (!t.np.empty() && t.npAlpha > 0.01f) {
        g_nowPlayingBrush->SetColor(D2D1::ColorF(g_settings.nowPlayingR / 255.0f, g_settings.nowPlayingG / 255.0f,
                                                 g_settings.nowPlayingB / 255.0f,
                                                 (g_settings.nowPlayingA / 255.0f) * t.npAlpha));
        if (g_npArtistBrush)
            g_npArtistBrush->SetColor(D2D1::ColorF(g_settings.npArtistR / 255.0f, g_settings.npArtistG / 255.0f,
                                                   g_settings.npArtistB / 255.0f,
                                                   (g_settings.npArtistA / 255.0f) * t.npAlpha));
        const float fontPx = (float)std::max(6.f, g_settings.nowPlayingFontSize) * g_dpiScale;
        const bool two = g_settings.npLayout == VizNpLayout::TwoLines && !t.npTitle.empty() && !t.npArtist.empty();
        // Two lines: title, then artist. One line: "Artist - Title", the
        // artist part in the artist colour.
        std::wstring text = two ? t.npTitle + L"\n" + t.npArtist : t.np;
        UINT32 artistAt = 0, artistLen = 0;
        if (two) {
            artistAt = (UINT32)t.npTitle.size() + 1;
            artistLen = (UINT32)t.npArtist.size();
        } else if (!t.npArtist.empty() && !t.npTitle.empty() && t.np.rfind(t.npArtist, 0) == 0) {
            artistLen = (UINT32)t.npArtist.size();
        }
        float npMargin = 8.0f * g_dpiScale;
        float npHeight = fontPx * (two ? 2.7f : 1.6f);
        float npOffX = EffectiveNowPlayingOffsetX();
        float npOffY = EffectiveNowPlayingOffsetY();
        D2D1_RECT_F npRect;
        if (g_settings.npPlacement == VizNpPlacement::Above) {
            float npBottom = blockY - npMargin;
            // A progress bar placed Above sits over the panel; Now Playing
            // then goes above it rather than through it. The layout already
            // reserves room for both (ComputeVizLayout adds the bar's height
            // to the space above). Settings-based, so the label doesn't jump
            // when a track's timeline appears or goes.
            D2D1_RECT_F pr;
            if (g_settings.progressEnabled && g_progressBrush &&
                g_settings.progressPlacement == VizProgressPlacement::Above && VizProgressRect(layout, &pr))
                npBottom = std::min(npBottom, pr.top - npMargin);
            npRect = D2D1::RectF(blockX - layout.textAnchorSide, npBottom - npHeight,
                                 blockX + totalWidth + layout.textAnchorSide, npBottom);
        } else {
            // Inside the panel: in the band of padding above (or below) the
            // bars, as wide as the bars, so Left / Right line up with them.
            float padT = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingT) : 0.f;
            float padB = g_settings.backgroundEnabled ? VizPx((float)g_settings.bgPaddingB) : 0.f;
            float y = (g_settings.npPlacement == VizNpPlacement::PanelTop)
                          ? blockY - padT + std::max(0.f, (padT - npHeight) * 0.5f)
                          : blockY + totalHeight + std::max(0.f, (padB - npHeight) * 0.5f);
            npRect = D2D1::RectF(blockX, y, blockX + totalWidth, y + npHeight);
        }
        npRect = snap(D2D1::RectF(npRect.left + npOffX, npRect.top + npOffY, npRect.right + npOffX,
                                  npRect.bottom + npOffY));
        TextPanelStyle npPanel{g_settings.npBgA, g_settings.npBgR, g_settings.npBgG, g_settings.npBgB,
                               g_settings.npBgBorderA, g_settings.npBgBorderR, g_settings.npBgBorderG,
                               g_settings.npBgBorderB, g_settings.npBgPadding, g_settings.npBgCornerRadius,
                               g_settings.npBgBorderSize};
        // Build the layout here (alignment, the artist's colour) and hand it
        // to DrawOverlayText through its cache, which then measures the panel
        // from it and draws it as is.
        static uint64_t s_npKey = 0;
        uint64_t key = (uint64_t)g_settings.npAlign * 7u + (uint64_t)two * 3u + (uint64_t)artistLen * 131u +
                       (uint64_t)(uintptr_t)g_npArtistBrush.Get();
        float boxW = npRect.right - npRect.left, boxH = npRect.bottom - npRect.top;
        VizTextLayoutCache& c = g_npLayoutCache;
        bool same = c.layout && s_npKey == key && c.w == boxW && c.h == boxH && c.text == text;
        if (!same && g_dwriteFactory && g_dwriteTextFormat) {
            c.Reset();
            if (SUCCEEDED(g_dwriteFactory->CreateTextLayout(text.c_str(), (UINT32)text.size(), g_dwriteTextFormat.Get(),
                                                            boxW, boxH, &c.layout)) &&
                c.layout) {
                c.layout->SetTextAlignment(g_settings.npAlign == VizTextAlignH::Left    ? DWRITE_TEXT_ALIGNMENT_LEADING
                                           : g_settings.npAlign == VizTextAlignH::Right ? DWRITE_TEXT_ALIGNMENT_TRAILING
                                                                                        : DWRITE_TEXT_ALIGNMENT_CENTER);
                c.layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                if (artistLen && g_npArtistBrush) {
                    DWRITE_TEXT_RANGE r{artistAt, artistLen};
                    c.layout->SetDrawingEffect(g_npArtistBrush.Get(), r);
                    if (two) c.layout->SetFontWeight(DWRITE_FONT_WEIGHT_NORMAL, r);
                }
                c.text = text;
                c.w = boxW;
                c.h = boxH;
                s_npKey = key;
            } else {
                c.Reset();
            }
        }
        (void)smooth;
        DrawOverlayText(text.c_str(), (UINT32)text.size(), npRect, g_nowPlayingBrush.Get(), npPanel, t.npAlpha,
                        &g_npLayoutCache);
    }
    if (!t.pf.empty()) {
        g_nowPlayingBrush->SetColor(D2D1::ColorF(g_settings.nowPlayingR / 255.0f, g_settings.nowPlayingG / 255.0f,
                                                 g_settings.nowPlayingB / 255.0f, g_settings.nowPlayingA / 255.0f));
        float pfMargin = 4.0f * g_dpiScale;
        float pfHeight = (float)std::max(6.f, g_settings.nowPlayingFontSize) * g_dpiScale * 1.4f;
        float pfWidth = t.pfWide ? totalWidth + 2.0f * layout.textAnchorSide
                                 : std::min(120.0f * g_dpiScale, totalWidth + 2.0f * layout.textAnchorSide);
        float pfOffX = EffectivePeakFreqOffsetX();
        float pfOffY = EffectivePeakFreqOffsetY();
        float pfX;
        switch (g_settings.peakFreqAlignH) {
            case VizTextAlignH::Left: pfX = blockX - layout.textAnchorSide; break;
            case VizTextAlignH::Center: pfX = blockX + (totalWidth - pfWidth) * 0.5f; break;
            default: pfX = blockX + totalWidth + layout.textAnchorSide - pfWidth; break;
        }
        pfX += pfOffX;
        float pfY;
        switch (g_settings.peakFreqAlignV) {
            case VizTextAlignV::Above: pfY = blockY - pfHeight - pfMargin; break;
            case VizTextAlignV::Middle: pfY = blockY + (totalHeight - pfHeight) * 0.5f; break;
            case VizTextAlignV::Bottom: pfY = blockY + totalHeight - pfHeight - pfMargin; break;
            case VizTextAlignV::Below: pfY = blockY + totalHeight + pfMargin; break;
            default: pfY = blockY + pfMargin; break;
        }
        pfY += pfOffY;
        D2D1_RECT_F pfRect = snap(D2D1::RectF(pfX, pfY, pfX + pfWidth, pfY + pfHeight));
        TextPanelStyle pfPanel{g_settings.pfBgA, g_settings.pfBgR, g_settings.pfBgG, g_settings.pfBgB,
                               g_settings.pfBgBorderA, g_settings.pfBgBorderR, g_settings.pfBgBorderG,
                               g_settings.pfBgBorderB, g_settings.pfBgPadding, g_settings.pfBgCornerRadius,
                               g_settings.pfBgBorderSize};
        DrawOverlayText(t.pf.c_str(), (UINT32)t.pf.length(), pfRect, g_nowPlayingBrush.Get(), pfPanel, 1.0f,
                        (smooth || t.pfWide) ? &g_pfLayoutCache : nullptr, t.pfWide);
    }
}

// ---- Direct3D 11 frame ------------------------------------------------------------------
// Returns false to fall back to the Direct2D path for this frame.
bool RenderVisualizerD3D(float sceneAlpha) {
    VizPerfScope perfScope(g_perfRenderTicks);
    VizPerf(kPerfRenderTicks);
    VizLayout layout;
    if (!ComputeVizLayout(&layout)) {
        ttgfx::ShowTextSurface(true);  // the Direct2D path draws this frame, on the text surface
        return false;
    }
    VizPublishDrawRect(layout);
    const bool dragPause = g_dragRenderPauseActive.load(std::memory_order_relaxed);

    ttgfx::FrameInputs in = {};
    in.layout = &layout;
    in.sceneAlpha = sceneAlpha;
    in.dragPause = dragPause;
    in.hasPanel = g_settings.backgroundEnabled && g_backgroundBrush;
    if (in.hasPanel) VizPanelRect(layout, &in.bgRect, in.bgRadii);

    if (sceneAlpha > 0.001f) {
        {
            float pulse = g_beatPulse.load(std::memory_order_relaxed);
            if (pulse > 0.f)
                g_beatPulse.store(std::max(0.f, pulse - 0.08f * g_frameScale), std::memory_order_relaxed);
        }
        if (!dragPause) {
            VizComputeBarFrame();
            if (g_settings.shape == VizShape::Oscilloscope) VizUpdateScopeTrace();
            if (g_settings.shape == VizShape::Terminal) VizBuildTermGrid();
        VizStylesFrame();
        }
        VizResolveColors(&in.c1, &in.cGrad1, &in.c2);
        in.rainbowBase = VizClockPhase(VizClockSeconds(), (double)g_settings.rainbowSpeed, 360.0);
        memcpy(in.scopeDisp, g_scopeDisp, sizeof(in.scopeDisp));
        VizZoneEnergies(in.zones);
        if (g_settings.shape == VizShape::Goniometer) {  // only the correlation bar reads it
            std::lock_guard<std::mutex> lock(g_meterMutex);
            in.correlation = (float)g_meters.correlation;
        }
    }
    if (!ttgfx::Render(in)) {
        ttgfx::ShowTextSurface(true);
        return false;
    }

    // Text surface: redrawn only when what it shows changes.
    static VizTextFrame tf;  // kept, so its strings keep their storage
    if (sceneAlpha > 0.001f && !dragPause) {
        VizBuildTextFrame(tf);
    } else {
        tf.np.clear();
        tf.npTitle.clear();
        tf.npArtist.clear();
        tf.npAlpha = 0.f;
        tf.progress = -1.f;
        tf.pf.clear();
        tf.pfWide = false;
    }
    uint64_t key = 1469598103934665603ull;
    for (wchar_t c : tf.np) ttgfx::Mix(key, (uint64_t)c);
    ttgfx::MixF(key, tf.npAlpha, 255.f);
    for (wchar_t c : tf.pf) ttgfx::Mix(key, (uint64_t)c);
    for (wchar_t c : tf.npArtist) ttgfx::Mix(key, (uint64_t)c);
    // The progress bar redraws the text surface once per pixel it grows.
    if (tf.progress >= 0.f) {
        D2D1_RECT_F pr;
        float wpx = VizProgressRect(layout, &pr) ? pr.right - pr.left : 0.f;
        ttgfx::Mix(key, 1000003u + (uint64_t)lroundf(std::clamp(tf.progress, 0.f, 1.f) * wpx));
    }
    ttgfx::MixF(key, sceneAlpha, 255.f);
    ttgfx::MixF(key, layout.blockX, 64.f);
    ttgfx::MixF(key, layout.blockY, 64.f);
    ttgfx::Mix(key, (uint64_t)g_swapChainWidth * 65536u + g_swapChainHeight);
    // Scale numbers (Spectrogram, VU): fixed by the settings and the size.
    const bool scale = VizStyleHasScale();
    const bool scaleHorizontal = g_settings.orientation == VizOrientation::Horizontal;
    const float scaleMax = std::max(2.f, VizPx((float)std::max(2.f, g_settings.barMaxSize)));
    const int scaleBars = VizEffectiveBarCount();
    const float scaleBarW = std::max(1.f, VizPx((float)std::max(1.f, g_settings.barWidth)));
    const float scaleGap = VizPx((float)std::max(0.f, g_settings.barGap));
    const float scaleThick = scaleBars * (scaleBarW + scaleGap) - scaleGap;
    if (scale) {
        ttgfx::Mix(key, 7u + (uint64_t)g_settings.style * 131u + (uint64_t)lroundf((g_settings.dbFloor + 200) * 100) * 1009u +
                            (uint64_t)lroundf((g_settings.dbCeiling + 200) * 100) * 65537u + (uint64_t)g_settings.engine);
        ttgfx::MixF(key, scaleMax, 64.f);
        ttgfx::MixF(key, scaleThick, 64.f);
        ttgfx::Mix(key, scaleHorizontal ? 1u : 2u);
    }
    if (!ttgfx::g.textForce && key == ttgfx::g.textKey) return true;
    ttgfx::g.textKey = key;
    ttgfx::g.textForce = false;
    // Nothing to show: take the surface off rather than present a clear one.
    const bool textEmpty = tf.pf.empty() && tf.progress < 0.f && (tf.np.empty() || tf.npAlpha <= 0.01f)
                           && !(scale && sceneAlpha > 0.001f);
    if (textEmpty) {
        ttgfx::ShowTextSurface(false);
        return true;
    }
    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
    bool fade = sceneAlpha < 0.999f;
    if (fade)
        g_dc->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                              D2D1::IdentityMatrix(), sceneAlpha),
                        nullptr);
    VizDrawTextOverlays(tf, layout, true);
    if (scale && sceneAlpha > 0.001f) VizDrawStyleScale(layout.blockX, layout.blockY, scaleMax, scaleThick, scaleHorizontal);
    if (fade) g_dc->PopLayer();
    HRESULT hrEnd = g_dc->EndDraw();
    HRESULT hrPresent = g_swapChain->Present(0, 0);
    VizCheckDeviceLost(hrEnd, hrPresent);
    VizPerf(kPerfTextPresents);
    ttgfx::ShowTextSurface(true);  // after the present, so it comes back with this frame on it
    return true;
}

void RenderVisualizer() {
    if (g_unloading || !g_dc || !g_swapChain) return;

    // Keeps the swap chain's on-screen position (the composition visual's
    // offset) and the content about to be drawn inside it derived from the
    // very same layout snapshot. Repositioning separately -- e.g. from the
    // drag hook on every mouse-move, independently of this tick -- let the
    // window race ahead of what got drawn inside it during a fast drag, so
    // the box and its background visibly fell out of sync until the next
    // tick caught up. Cheap to call defensively when nothing has moved.
    UpdateSwapChainForLayout();
    ttgfx::SplitSync();

    // Smooth Mode: how late is this frame, relative to the Target FPS interval
    // the motion constants were tuned against? Measured against the user's
    // Target FPS rather than the display-locked interval actually being run,
    // so rounding 60 up to 72 on a 144 Hz panel doesn't make bars faster.
    const bool smooth = g_smoothActive.load(std::memory_order_relaxed);
    {
        static LONGLONG s_prevFrameQpc = 0;
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        g_frameScale = 1.0f;
        double dtMs = s_prevFrameQpc ? (double)(q.QuadPart - s_prevFrameQpc) * 1000.0 / (double)VizQpcFreq()
                                     : 1000.0 / 60.0;
        if (smooth && s_prevFrameQpc) {
            double refMs = 1000.0 / (double)std::max(1, g_settings.targetFps > 0 ? g_settings.targetFps : 60);
            g_frameScale = (float)std::clamp(dtMs / refMs, 0.25, 4.0);
        }
        // Seconds, for everything that runs on elapsed time (precision peak
        // caps, the breathe envelope, GPU ballistics). Long gaps (idle,
        // pause) are clamped so nothing jumps when drawing resumes.
        g_frameDt = (float)std::clamp(dtMs / 1000.0, 0.0005, 0.25);
        s_prevFrameQpc = q.QuadPart;
    }

    float sceneAlpha = 1.0f;
    if (g_settings.autoHideEnabled) {
        ULONGLONG idleMs = GetTickCount64() - g_lastAudibleTickMs.load(std::memory_order_relaxed);
        ULONGLONG delayMs = (ULONGLONG)(std::max(0.f, g_settings.autoHideDelaySeconds) * 1000.f);
        constexpr ULONGLONG kFadeMs = 1500;
        if (idleMs > delayMs) {
            ULONGLONG fadeElapsed = idleMs - delayMs;
            sceneAlpha = (fadeElapsed >= kFadeMs) ? 0.f : 1.0f - (float)fadeElapsed / (float)kFadeMs;
        }
    }

    g_vizSceneHidden.store(sceneAlpha <= 0.001f, std::memory_order_relaxed);

    // Direct3D 11 renderer (2.0). Falls through to the Direct2D path below if
    // it isn't selected, or can't run on this device.
    if (ttgfx::Active()) {
        if (RenderVisualizerD3D(sceneAlpha)) {
            g_autoHideBlanked = sceneAlpha <= 0.001f;
            return;
        }
    } else {
        ttgfx::ReleaseSurface();
    }

    if (sceneAlpha <= 0.001f) {
        // Fully faded out. Present one blank frame to clear whatever was last
        // shown, then skip the render path entirely until audio returns --
        // there is no point drawing a scene at full detail and then making it
        // invisible.
        if (g_autoHideBlanked) return;
        g_dc->BeginDraw();
        g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));
        HRESULT hrEnd = g_dc->EndDraw();
        HRESULT hrPresent = g_swapChain->Present(0, 0);
        VizCheckDeviceLost(hrEnd, hrPresent);
        g_autoHideBlanked = true;
        return;
    }
    g_autoHideBlanked = false;

    g_dc->BeginDraw();
    g_dc->Clear(D2D1::ColorF(0, 0, 0, 0));

    // Every bar's height and peak cap for this frame, for either engine (see
    // VizComputeBarFrame). The loops below only draw them.
    if (!g_dragRenderPauseActive.load(std::memory_order_relaxed)) {
        VizComputeBarFrame();
        if (g_settings.shape == VizShape::Terminal) VizBuildTermGrid();
        VizStylesFrame();
    }

    int barCount = VizEffectiveBarCount();
    float barW = std::max(1.f, VizPx((float)std::max(1.f, g_settings.barWidth)));
    float barGap = VizPx((float)std::max(0.f, g_settings.barGap));
    float maxSize = std::max(2.f, VizPx((float)std::max(2.f, g_settings.barMaxSize)));
    float idleSize = VizPx((float)std::max(0.f, g_settings.barIdleSize));
    float rTL = g_settings.barRadiusTL * g_dpiScale;
    float rTR = g_settings.barRadiusTR * g_dpiScale;
    float rBR = g_settings.barRadiusBR * g_dpiScale;
    float rBL = g_settings.barRadiusBL * g_dpiScale;

    bool horizontal = (g_settings.orientation == VizOrientation::Horizontal);
    
    float barsThickness = barCount * barW + (barCount - 1) * barGap;
    
    float groupThickness = barsThickness;
    float groupExtent    = maxSize;

    float totalWidth, totalHeight;
    if (g_settings.shape == VizShape::Terminal) {
        VizTermBox(&totalWidth, &totalHeight);
    } else if (g_settings.shape == VizShape::Radial || g_settings.shape == VizShape::Goniometer) {
        totalWidth = totalHeight = maxSize * 2.0f;
    } else {
        totalWidth  = horizontal ? groupThickness : groupExtent;
        totalHeight = horizontal ? groupExtent    : groupThickness;
    }
    VizStyleBox(&totalWidth, &totalHeight, maxSize, horizontal);

    float animTime = (float)GetTickCount64() * 0.001f;
    // Rainbow hue offset, degrees. With Smooth Mode off this is the 1.4
    // expression exactly; with it on, it comes off the QPC clock and is reduced
    // in double before narrowing (see VizClockSeconds).
    const float rainbowBase =
        smooth ? VizClockPhase(VizClockSeconds(), (double)g_settings.rainbowSpeed, 360.0)
               : animTime * g_settings.rainbowSpeed;
    {
        float pulse = g_beatPulse.load(std::memory_order_relaxed);
        if (pulse > 0.f)
            g_beatPulse.store(std::max(0.f, pulse - 0.08f * g_frameScale), std::memory_order_relaxed);
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

        VizPublishDrawRect(layout);

        if (g_backgroundBrush) {
            float padL = VizPx((float)g_settings.bgPaddingL);
            float padR = VizPx((float)g_settings.bgPaddingR);
            float padT = VizPx((float)g_settings.bgPaddingT);
            float padB = VizPx((float)g_settings.bgPaddingB);
            float bgWidth  = totalWidth  + padL + padR;
            float bgHeight = totalHeight + padT + padB;

            float bgTL = g_settings.bgRadiusTL * g_dpiScale;
            float bgTR = g_settings.bgRadiusTR * g_dpiScale;
            float bgBR = g_settings.bgRadiusBR * g_dpiScale;
            float bgBL = g_settings.bgRadiusBL * g_dpiScale;

            D2D1_RECT_F bgRect = D2D1::RectF(blockX - padL, blockY - padT,
                                              blockX + totalWidth + padR,
                                              blockY + totalHeight + padB);

            // Negative padding can shrink a side past the bars entirely. Once
            // opposite sides would cross over and invert the rect, D2D just
            // draws nothing -- so instead of letting that happen, hold the box
            // open to a sliver on whichever side is collapsing.
            if (bgRect.right - bgRect.left < 1.0f) {
                float mid = (bgRect.left + bgRect.right) * 0.5f;
                bgRect.left = mid - 0.5f;
                bgRect.right = mid + 0.5f;
            }
            if (bgRect.bottom - bgRect.top < 1.0f) {
                float mid = (bgRect.top + bgRect.bottom) * 0.5f;
                bgRect.top = mid - 0.5f;
                bgRect.bottom = mid + 0.5f;
            }
            bgWidth  = bgRect.right  - bgRect.left;
            bgHeight = bgRect.bottom - bgRect.top;

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

            // The border ring's geometry is now built before anything is drawn
            // rather than between the fill and the border draw. Building a
            // geometry isn't drawing, so the 1.4 output is unchanged -- but the
            // plate below needs every piece in hand before it bakes.
            bool ringRebuilt = false;
            if (g_borderBrush) {
                float bw = std::min((float)g_settings.bgBorderSize * g_dpiScale,
                                    std::min(bgWidth, bgHeight) / 2.0f);

                if (bgDirty || g_borderCacheBorderSize != g_settings.bgBorderSize) {
                    ringRebuilt = true;
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
            }
            ID2D1Geometry* ring = g_borderBrush ? g_borderRingCache.Get() : nullptr;

            // Smooth Mode: a single blit of the cached plate. It is re-baked
            // only when the panel's geometry changes; color, blur
            // and border-size changes all reset it through
            // ReleaseVisualResources. If it can't be baked for any reason,
            // the 1.4 path below draws instead.
            bool drawnFromPlate = false;
            if (smooth) {
                D2D1_RECT_F want = D2D1::RectF(floorf(bgRect.left) - 2.f, floorf(bgRect.top) - 2.f,
                                               ceilf(bgRect.right) + 2.f, ceilf(bgRect.bottom) + 2.f);
                bool fresh = g_plateBitmap && !bgDirty && !ringRebuilt &&
                             RectsApproxEqual(want, g_plateRect);
                if (fresh || VizBakeBackgroundPlate(bgRect, bgGeo, ring)) {
                    g_dc->DrawBitmap(g_plateBitmap.Get(), g_plateRect, 1.0f,
                                     D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
                    drawnFromPlate = true;
                }
            }
            if (!drawnFromPlate) {
                VizDrawBackgroundLayers(g_dc.Get(), bgGeo, ring, g_backgroundBrush.Get(),
                                        g_borderBrush.Get());
            }
        }

        if (g_dragRenderPauseActive.load(std::memory_order_relaxed)) {
            // Mid-drag: only the background/border box (just drawn above,
            // already at its new position) should be visible while it's being
            // moved -- freeze the bars in place rather than animate them
            // under a box that's being repositioned.
            static int s_dragTickLogCounter = 0;
            if ((++s_dragTickLogCounter % 10) == 0) {
                Wh_Log(L"[Drag] TICK originXY=(%.1f,%.1f) blockXY=(%.1f,%.1f) HV=(%.2f,%.2f)",
                       layout.originX, layout.originY, layout.blockX, layout.blockY,
                       g_dragOverrideH.load(std::memory_order_relaxed),
                       g_dragOverrideV.load(std::memory_order_relaxed));
            }
            if (useFadeLayer) g_dc->PopLayer();
            HRESULT hrEnd = g_dc->EndDraw();
            HRESULT hrPresent = g_swapChain->Present(0, 0);
            VizCheckDeviceLost(hrEnd, hrPresent);
            return;
        }

        RGBA c1{g_settings.colorA, g_settings.colorR, g_settings.colorG, g_settings.colorB};
        {
            if (g_settings.colorMode == VizColorMode::Accent) {
                DWORD dw = GetWindowsAccentColor();
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            } else if (g_settings.colorMode == VizColorMode::AlbumArt) {
                DWORD dw = VizAlbumColorShown(0);
                c1 = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            } else if (g_settings.colorMode == VizColorMode::DynamicAlbum) {
                DWORD dw = VizAlbumColorShown(0);
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
            DWORD dw = VizAlbumColorShown(1);
            c2    = {0xFF, (BYTE)((dw>>16)&0xFF), (BYTE)((dw>>8)&0xFF), (BYTE)(dw&0xFF)};
            cGrad1 = c1;
        }

        const bool reflD2D = VizFxD2DBegin(useFadeLayer);
        if (VizDrawStyleD2D(blockX, blockY, totalWidth, totalHeight, barCount, barW, barGap, maxSize, idleSize,
                            horizontal, c1, cGrad1, c2, rainbowBase)) {
            // drawn by the style
        } else if (g_settings.shape == VizShape::Dots) {
            float dotR  = barW * 0.5f;
            float step  = barW + barGap;
            float dTL = g_settings.barRadiusTL * g_dpiScale;
            float dTR = g_settings.barRadiusTR * g_dpiScale;
            float dBR = g_settings.barRadiusBR * g_dpiScale;
            float dBL = g_settings.barRadiusBL * g_dpiScale;

            // Smooth Mode: every dot becomes one sprite of a pre-drawn dot, and
            // the whole field goes out in one call. This is the shape that
            // gains the most -- a modest setup is well over a thousand dots,
            // each a separate rounded-rect fill (or, with per-corner radii, a
            // separate path geometry) on the 1.4 path.
            VizAtlasKey dotKey;
            dotKey.kind = 1;
            dotKey.thick = barW;
            dotKey.rTL = dTL; dotKey.rTR = dTR; dotKey.rBR = dBR; dotKey.rBL = dBL;
            const bool dotSprites = smooth && VizEnsureSpriteAtlas(dotKey);
            if (dotSprites) VizClearSprites();
            D2D1_COLOR_F dotCol = D2D1::ColorF(0, 0, 0, 0);

            for (int i = 0; i < barCount; i++) {
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
                    float hue = fmodf(rainbowBase +
                                       (barCount > 1 ? (float)i/(barCount-1) : 0.f) * 360.f, 360.f);
                    col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(0, std::min(180, (int)(180.f * fac)));
                    col = {aa, c1.r, c1.g, c1.b};
                }
                if (g_settings.beatFlashEnabled) {
                    float pulse = g_beatPulse.load(std::memory_order_relaxed);
                    if (pulse > 0.001f) {
                        float blend = std::min(1.0f, pulse * (g_settings.beatFlashIntensity / 100.0f) *
                                                      (g_settings.beatFlashA / 255.0f));
                        col.r = (BYTE)(col.r + (g_settings.beatFlashR - (int)col.r) * blend);
                        col.g = (BYTE)(col.g + (g_settings.beatFlashG - (int)col.g) * blend);
                        col.b = (BYTE)(col.b + (g_settings.beatFlashB - (int)col.b) * blend);
                    }
                }
                if (dotSprites)
                    dotCol = VizPremultiplied(col.a, col.r, col.g, col.b);
                else
                    g_barBrush->SetColor(D2D1::ColorF(col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f));

                int numDots = (step > 0.5f) ? (int)(colSize / step) : 1;
                numDots = std::max(1, numDots);

                auto drawDot = [&](float cx2, float cy2) {
                    if (dotSprites) {
                        VizQueueDotSprite(cx2, cy2, dotR, dotCol);
                        return;
                    }
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
            if (dotSprites) VizFlushSprites();
        } else if (g_settings.shape == VizShape::Radial) {
            float centerX = blockX + totalWidth * 0.5f;
            float centerY = blockY + totalHeight * 0.5f;
            float innerR = maxSize * 0.15f;
            float strokeW = std::max(1.0f, barW);

            for (int i = 0; i < barCount; i++) {
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
                    float hue = fmodf(rainbowBase +
                                       (float)i / std::max(1, barCount) * 360.f, 360.f);
                    col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(0, std::min(180, (int)(180.f * fac)));
                    col = {aa, c1.r, c1.g, c1.b};
                }
                if (g_settings.beatFlashEnabled) {
                    float pulse = g_beatPulse.load(std::memory_order_relaxed);
                    if (pulse > 0.001f) {
                        float blend = std::min(1.0f, pulse * (g_settings.beatFlashIntensity / 100.0f) *
                                                      (g_settings.beatFlashA / 255.0f));
                        col.r = (BYTE)(col.r + (g_settings.beatFlashR - (int)col.r) * blend);
                        col.g = (BYTE)(col.g + (g_settings.beatFlashG - (int)col.g) * blend);
                        col.b = (BYTE)(col.b + (g_settings.beatFlashB - (int)col.b) * blend);
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
            // The trace sweeps along the group's LONG axis (time) and deflects
            // across its short one (amplitude), so it has to follow the
            // orientation. Vertical is the horizontal layout rotated 90 degrees
            // clockwise: the sweep runs top-to-bottom and "up" becomes "right".
            float ampScale = maxSize * 0.5f;
            float sweepLen = horizontal ? totalWidth : totalHeight;
            float center   = horizontal ? blockY + totalHeight * 0.5f
                                        : blockX + totalWidth  * 0.5f;
            float sweepOrigin = horizontal ? blockX : blockY;
            float wstep = (VIZ_WAVE_SAMPLES > 1) ? sweepLen / (float)(VIZ_WAVE_SAMPLES - 1)
                                                 : sweepLen;

            RGBA col = c1;
            if (g_settings.colorMode == VizColorMode::Gradient ||
                g_settings.colorMode == VizColorMode::Tourne)
                col = LerpColor(cGrad1, c2, 0.5f);
            else if (g_settings.colorMode == VizColorMode::RainbowCycle) {
                float hue = fmodf(rainbowBase, 360.f);
                col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
            }
            if (g_settings.oscilloscopeMultibandEnabled) {
                // Blends 3 reference colors (low/mid/high) by how much energy is
                // currently in each EQ zone, so the trace tints toward whichever
                // part of the spectrum is dominant right now. This overrides
                // whatever the color mode above picked, matching how RainbowCycle
                // and Tourne already take priority for this shape.
                float zones[3];
                VizZoneEnergies(zones);
                float lowE = zones[0], midE = zones[1], highE = zones[2];
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
                    float blend = std::min(1.0f, pulse * (g_settings.beatFlashIntensity / 100.0f) *
                                                  (g_settings.beatFlashA / 255.0f));
                    col.r = (BYTE)(col.r + (g_settings.beatFlashR - (int)col.r) * blend);
                    col.g = (BYTE)(col.g + (g_settings.beatFlashG - (int)col.g) * blend);
                    col.b = (BYTE)(col.b + (g_settings.beatFlashB - (int)col.b) * blend);
                }
            }
            g_barBrush->SetColor(D2D1::ColorF(col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f));

            float strokeW = std::max(1.0f, barW * 0.5f);

            // Trace damping, eased on the RENDER thread rather than the capture
            // thread, and deliberately so. The capture side publishes a new
            // trace about 100 times a second; easing toward it here means every
            // frame draws a different trace, so the shape evolves smoothly at
            // whatever the Target FPS is instead of stepping between published
            // snapshots.
            //
            // This is what makes a slow trace legible. Damping it lets the eye
            // lock onto a shape and follow it, where an undamped trace at a
            // wide window is a new picture every frame. It is only valid
            // because the capture side triggers on a zero crossing: without
            // that, point w would mean a different phase each time and easing
            // per index would smear the waveform into mush rather than slow it
            // down.
            //
            // Same shape as the bars' Motion Smoothing above, scaled so even
            // full damping keeps moving rather than freezing.
            VizUpdateScopeTrace();
            const float* s_scopeDisp = g_scopeDisp;

            auto wavePoint = [&](int w) -> D2D1_POINT_2F {
                float along  = sweepOrigin + w * wstep;
                float across = center + s_scopeDisp[w] * ampScale * (horizontal ? -1.0f : 1.0f);
                return horizontal ? D2D1::Point2F(along, across)
                                  : D2D1::Point2F(across, along);
            };
            D2D1_POINT_2F prev = wavePoint(0);
            for (int w = 1; w < VIZ_WAVE_SAMPLES; w++) {
                D2D1_POINT_2F pt = wavePoint(w);
                g_dc->DrawLine(prev, pt, g_barBrush.Get(), strokeW);
                prev = pt;
            }
        }
        else if (g_settings.shape == VizShape::Goniometer) {
            // The Direct2D fallback for the goniometer: the same points the
            // Direct3D 11 renderer draws, as small squares, and the
            // correlation bar. Fine for a fallback; the Direct3D 11 renderer
            // is the one to use for this shape.
            static std::vector<float> s_pts(ttgfx::kMaxPoints * 4);
            int n = ttgfx::BuildGonioPoints(s_pts.data(), ttgfx::kMaxPoints);
            RGBA col = c1;
            if (g_settings.colorMode == VizColorMode::Gradient || g_settings.colorMode == VizColorMode::Tourne)
                col = LerpColor(cGrad1, c2, 0.5f);
            float cx = blockX + totalWidth * 0.5f, cy = blockY + totalHeight * 0.5f;
            float R = maxSize * 0.85f, d = std::max(0.75f, barW * 0.2f);
            for (int k = 0; k < n; k++) {
                float x = cx + std::clamp(s_pts[k * 4], -1.f, 1.f) * R;
                float y = cy - std::clamp(s_pts[k * 4 + 1], -1.f, 1.f) * R;
                g_barBrush->SetColor(D2D1::ColorF(col.r / 255.f, col.g / 255.f, col.b / 255.f,
                                                  col.a / 255.f * s_pts[k * 4 + 2]));
                g_dc->FillRectangle(D2D1::RectF(x - d, y - d, x + d, y + d), g_barBrush.Get());
            }
            float corr;
            {
                std::lock_guard<std::mutex> lock(g_meterMutex);
                corr = (float)g_meters.correlation;
            }
            float ch = std::max(2.0f, 3.0f * g_dpiScale), cyBar = cy + maxSize * 0.92f - ch;
            g_barBrush->SetColor(D2D1::ColorF(col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f * 0.25f));
            g_dc->FillRectangle(D2D1::RectF(cx - R, cyBar, cx + R, cyBar + ch), g_barBrush.Get());
            g_barBrush->SetColor(D2D1::ColorF(col.r / 255.f, col.g / 255.f, col.b / 255.f, col.a / 255.f));
            float mx = cx + std::clamp(corr, -1.f, 1.f) * R;
            g_dc->FillRectangle(D2D1::RectF(mx - 1.5f, cyBar - 1.f, mx + 1.5f, cyBar + ch + 1.f), g_barBrush.Get());
        }
        else if (g_settings.shape == VizShape::Terminal) {
            // Direct2D fallback: the grid as runs of text. The Direct3D 11
            // renderer draws it from a glyph atlas in one call instead.
            VizDrawTermGridD2D(floorf(blockX + 0.5f), floorf(blockY + 0.5f));
        }
        else {
            // Smooth Mode: bars go out as one sprite batch (see
            // VizEnsureSpriteAtlas). The atlas is baked at the bar row's
            // subpixel offset across the bars, quantized so float noise in the
            // layout can't force a re-bake every frame.
            VizAtlasKey barKey;
            barKey.kind = 0;
            barKey.horizontal = horizontal;
            barKey.thick = barW;
            barKey.rTL = rTL; barKey.rTR = rTR; barKey.rBR = rBR; barKey.rBL = rBL;
            {
                float lead = horizontal ? blockX : blockY;
                float frac = roundf((lead - floorf(lead)) * 256.f) / 256.f;
                barKey.frac = (frac >= 1.f) ? 0.f : frac;
            }
            const bool barSprites = smooth && VizEnsureSpriteAtlas(barKey);
            // Peak caps stay ordinary antialiased rectangles, drawn after the
            // batch so they still sit on top of their bars.
            static std::vector<D2D1_RECT_F> s_capRects;
            if (barSprites) {
                VizClearSprites();
                s_capRects.clear();
            }

            for (int i = 0; i < barCount; i++) {
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
                    float hue = fmodf(rainbowBase + t * 360.f, 360.f);
                    col = HSVtoRGB(hue, 0.85f, 1.0f, c1.a);
                }
                if (g_settings.colorMode == VizColorMode::Acrylic) {
                    BYTE aa = (BYTE)std::max(0, std::min(180, (int)(180.f * fac)));
                    col = {aa, c1.r, c1.g, c1.b};
                }
                if (g_settings.beatFlashEnabled) {
                    float pulse = g_beatPulse.load(std::memory_order_relaxed);
                    if (pulse > 0.001f) {
                        float blend = std::min(1.0f, pulse * (g_settings.beatFlashIntensity / 100.0f) *
                                                      (g_settings.beatFlashA / 255.0f));
                        col.r = (BYTE)(col.r + (g_settings.beatFlashR - (int)col.r) * blend);
                        col.g = (BYTE)(col.g + (g_settings.beatFlashG - (int)col.g) * blend);
                        col.b = (BYTE)(col.b + (g_settings.beatFlashB - (int)col.b) * blend);
                    }
                }

                D2D1_COLOR_F d2dCol = D2D1::ColorF(col.r / 255.0f, col.g / 255.0f, col.b / 255.0f,
                                                   col.a / 255.0f);
                D2D1_COLOR_F spriteCol = d2dCol;
                if (barSprites)
                    spriteCol = VizPremultiplied(col.a, col.r, col.g, col.b);
                else
                    g_barBrush->SetColor(d2dCol);

                // Draws one bar body: queued as sprites in Smooth Mode, filled
                // directly otherwise. Peak caps likewise queue or draw.
                auto fillBar = [&](const D2D1_RECT_F& r) {
                    if (barSprites)
                        VizQueueBarSprites(r, horizontal, spriteCol);
                    else
                        FillRoundedRectPerCorner(g_dc.Get(), g_d2dFactory.Get(), r,
                                                 g_barBrush.Get(), rTL, rTR, rBR, rBL);
                };
                auto fillCap = [&](const D2D1_RECT_F& r) {
                    if (barSprites)
                        s_capRects.push_back(r);
                    else
                        g_dc->FillRectangle(r, g_barBrush2.Get());
                };

                float holdSize = idleSize;
                if (g_settings.peakHoldEnabled)
                    holdSize = idleSize + g_vizPeakHold[i] * std::max(0.f, maxSize - idleSize);
                float capThickness = std::max(1.5f, 2.0f * g_dpiScale);

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
                    fillBar(barRect);
                    if (g_settings.peakHoldEnabled && holdSize > 0.5f) {
                        fillCap(D2D1::RectF(x, capY - capThickness * 0.5f,
                                            x + barW, capY + capThickness * 0.5f));
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
                    fillBar(barRect);
                    if (g_settings.peakHoldEnabled && holdSize > 0.5f) {
                        fillCap(D2D1::RectF(capX - capThickness * 0.5f, y,
                                            capX + capThickness * 0.5f, y + barW));
                    }
                }
            }

            if (barSprites) {
                VizFlushSprites();
                for (const auto& cap : s_capRects) g_dc->FillRectangle(cap, g_barBrush2.Get());
            }
        }
        if (reflD2D) VizFxD2DEnd(blockY + maxSize, VizReflectionDepth(maxSize));

        {
            VizTextFrame tf;
            VizBuildTextFrame(tf);
            VizDrawTextOverlays(tf, layout, smooth);
        }

        if (useFadeLayer) {
            g_dc->PopLayer();
        }
    }

    HRESULT hrEnd = g_dc->EndDraw();
    // Sync interval 0. On a DirectComposition swap chain DWM owns presentation
    // timing, and the waitable timer below already paces the loop -- asking
    // Present to block for a vblank on top of that only parks the render thread.
    // Measured across 219 five-second windows: time in Present drops by a third
    // and windows where the thread parks for >8% of wall time fall from 18% to 4%.
    //
    // Smooth Mode keeps sync interval 0 for the same reason. What it changes is
    // WHEN this runs: the render thread schedules each frame half a refresh
    // after a vblank (see RenderThreadProc), so each present lands in the
    // middle of a refresh interval, far from the edge where some frames would
    // make DWM's next composition and some would miss it.
    HRESULT hrPresent = g_swapChain->Present(0, 0);
    VizCheckDeviceLost(hrEnd, hrPresent);
}

// ---- Smooth Mode: display-locked frame pacing -----------------------------------
//
// Why the 1.4 timer isn't enough on its own, using the 1.1.0 changelog's own
// measurement: 144 FPS requested, 141.7 delivered, the gap being timer wake
// latency added to every interval. On a 144 Hz panel that is two or three
// refreshes a second that get no new frame, so the previous one is shown
// twice -- a stutter you see several times a second. A 60 FPS target on a
// 144 Hz panel is worse in a different way: 144 / 60 = 2.4, so frames are held
// for 2, then 3, then 2 refreshes, which reads as judder however accurate the
// timer is.
//
// So in Smooth Mode the schedule comes from DWM's composition clock instead of
// from "interval since the last frame":
//   * the frame rate is the refresh rate divided by a whole number, the one
//     nearest the Target FPS, so every frame is shown for the same number of
//     refreshes;
//   * every wake is placed half a refresh after a vblank, which is as far as a
//     present can get from DWM's composition deadline on either side, so
//     ordinary scheduling jitter can't push some frames to the next refresh;
//   * deadlines advance on an absolute timeline, so wake latency doesn't
//     accumulate into drift.
// It still wakes once per frame, never once per vblank, so the idle-power
// behaviour the README measures is kept. DWM is asked for its clock twice a
// second, not per frame.
struct VizVsyncSchedule {
    // Smallest time >= t of the form vblank + (k + phase) * period.
    static LONGLONG AlignUp(LONGLONG t, LONGLONG vblank, LONGLONG period, double phase) {
        LONGLONG base = vblank + (LONGLONG)(phase * (double)period);
        LONGLONG d = t - base;
        LONGLONG k = (d >= 0) ? (d + period - 1) / period : -((-d) / period);
        return base + k * period;
    }
    // Refreshes per frame: the whole number nearest refreshHz / targetFps, >= 1.
    static int Divisor(double targetFps, LONGLONG period, LONGLONG qpcFreq) {
        double refreshHz = (double)qpcFreq / (double)period;
        long long n = llround(refreshHz / std::max(1.0, targetFps));
        return (int)std::clamp(n, 1LL, 1000LL);
    }
    // The deadline after one that was due at `due`, re-snapped to mid-refresh
    // so a drifting or re-read vblank reference is absorbed gradually. If the
    // thread has fallen more than a frame behind, it rejoins at the next slot
    // instead of rendering a burst to catch up.
    static LONGLONG Next(LONGLONG due, LONGLONG now, LONGLONG vblank, LONGLONG period, int divisor) {
        LONGLONG next = AlignUp(due + (LONGLONG)divisor * period - period / 2, vblank, period, 0.5);
        if (next <= now) next = AlignUp(now + 1, vblank, period, 0.5);
        return next;
    }
};

// Thread-scoped quality of service. While playing, Windows schedules the
// engine thread as it sees fit; while idle it asks for EcoQoS, so the trickle and the meter reads run on
// efficiency cores at low clocks. Thread-scoped on purpose: nothing about the
// process changes. SetThreadInformation is resolved at runtime (Windows 8+;
// the power-throttling class needs Windows 10 1709, and older builds simply
// return an error, which is ignored).
void VizThreadEcoQoS(bool eco) {
    struct PowerThrottlingState {
        ULONG Version, ControlMask, StateMask;
    };
    typedef BOOL(WINAPI * SetThreadInformationFn)(HANDLE, int, LPVOID, DWORD);
    static SetThreadInformationFn fn =
        (SetThreadInformationFn)(void*)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "SetThreadInformation");
    if (!fn) return;
    // While playing this lets the system manage the thread instead of opting
    // out of throttling (HighQoS), so it isn't forced onto performance cores
    // and boost clocks. Measured at 144 fps: about 0.03 ms of engine work per
    // wake and well under 1 ms of rendering per frame, against a 6.9 ms frame.
    PowerThrottlingState st = {1, eco ? 0x1u : 0x0u /* EXECUTION_SPEED */, eco ? 0x1u : 0x0u};
    fn(GetCurrentThread(), 3 /* ThreadPowerThrottling */, &st, sizeof(st));
}

// The engine thread: frame pacing (as 1.5), and since 2.0 also audio capture
// and analysis (see "Audio engine" above). One wake per frame while playing,
// none at all in deep idle except four meter reads a second.
void RenderThreadProc() {
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    VizEngine engine;
    engine.ThreadInit();

    ULONGLONG lastSuccessfulPostTick = 0;
    int ecoState = -1;  // -1 unknown, 0 system-managed, 1 EcoQoS
    auto setEco = [&](bool eco) {
        if (ecoState == (eco ? 1 : 0)) return;
        ecoState = eco ? 1 : 0;
        VizThreadEcoQoS(eco);
    };

    // Smooth Mode pacing state (see VizVsyncSchedule).
    LONGLONG vsPeriod = 0, vsVblank = 0, vsLastSync = 0, vsDue = 0;
    LONGLONG absDue = 0;

    LARGE_INTEGER qpcFreq;
    QueryPerformanceFrequency(&qpcFreq);
    LONGLONG lastRenderQpc = 0;
    LONGLONG lastEngineQpc = 0;  // for the analysis dt

    HANDLE hTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                           TIMER_ALL_ACCESS);
    if (!hTimer) hTimer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);

    // Waits `ms`, but returns early if the wake event is set (settings change,
    // pause, unload), so none of those has to wait out a frame.
    auto preciseWait = [hTimer](double ms) {
        if (ms <= 0.0) return;
        if (hTimer) {
            LARGE_INTEGER due;
            due.QuadPart = -(LONGLONG)(ms * 10000.0);
            if (due.QuadPart == 0) due.QuadPart = -1;
            if (SetWaitableTimer(hTimer, &due, 0, nullptr, nullptr, FALSE)) {
                HANDLE hs[2] = {hTimer, g_engineWake};
                WaitForMultipleObjects(g_engineWake ? 2 : 1, hs, FALSE, INFINITE);
                return;
            }
        }
        Sleep((DWORD)std::ceil(ms));
    };

    auto postTick = [&](HWND overlayWnd, ULONGLONG now) {
        bool expected = false;
        if (g_renderTickPending.compare_exchange_strong(expected, true)) {
            PostMessage(overlayWnd, WM_APP_RENDER_TICK, 0, 0);
            lastSuccessfulPostTick = now;
        } else if (now - lastSuccessfulPostTick > 1000) {
            Wh_Log(L"[Viz] Render tick flag was stuck, forced a reset");
            g_renderTickPending.store(false, std::memory_order_relaxed);
            lastSuccessfulPostTick = now;
        }
    };
    auto engineFrame = [&]() -> bool {
        LARGE_INTEGER q;
        QueryPerformanceCounter(&q);
        double dt = lastEngineQpc ? (double)(q.QuadPart - lastEngineQpc) / (double)qpcFreq.QuadPart : 1.0 / 60.0;
        lastEngineQpc = q.QuadPart;
        VizPerfScope perfScope(g_perfEngineTicks);
        return engine.Frame(dt);
    };

    // Idle ladder state (see "Idle has three steps" in the audio engine).
    ULONGLONG trickleDue = 0;        // next 250 ms trickle frame
    ULONGLONG deepHoldUntil = 0;     // after a meter wake: stay in trickle until then
    bool meterProbe = false;         // a meter wake whose 1 s look hasn't ended yet
    float meterProbePeak = 0.f;      // the reading that caused it, after Input Gain
    float meterWake = kVizAudibleLin;
    ULONGLONG meterWakeRaisedUntil = 0;

    while (g_renderThreadRunning.load(std::memory_order_relaxed)) {
        VizPerfMaybeLog();
        VizPerf(kPerfEngineWakes);
        VizPerf((VizPerfCounter)(kPerfIdlePlaying + std::clamp(g_idleState.load(std::memory_order_relaxed), 0, 2)));
        HWND overlayWnd = g_overlayWnd.load(std::memory_order_relaxed);
        bool paused = g_fullscreenPaused.load(std::memory_order_relaxed);
        bool wanted = g_captureWanted.load(std::memory_order_relaxed);

        // Not visible, or capture not wanted: close the stream and sleep until
        // something changes. 1.5 woke every 150 ms here; this waits on the
        // wake event, with a slow timeout as a safety net.
        if (!overlayWnd || paused || !wanted) {
            if (engine.IsOpen() || g_audioOpen.load()) engine.Close();
            setEco(true);
            lastRenderQpc = lastEngineQpc = 0;
            vsDue = absDue = 0;
            g_idleState.store((int)VizIdleState::Playing);
            WaitForSingleObject(g_engineWake, overlayWnd ? 1000 : 250);
            continue;
        }

        engine.EnsureOpen();

        ULONGLONG now = GetTickCount64();
        int fps = g_settings.targetFps;
        double refreshHz = 0.0;
        if (vsPeriod > 0) refreshHz = (double)qpcFreq.QuadPart / (double)vsPeriod;
        if (fps <= 0) fps = (refreshHz > 0.0) ? (int)llround(refreshHz) : 60;  // 0 = match the display
        fps = std::max(1, fps);
        double intervalMs = 1000.0 / (double)fps;

        // ---- Idle ladder ----------------------------------------------------------
        ULONGLONG idleMs = now - g_lastAudibleTickMs.load(std::memory_order_relaxed);
        ULONGLONG silentAfter = (ULONGLONG)(std::max(0.f, g_settings.pauseWhenSilentSeconds) * 1000.f);
        bool silent = g_settings.pauseWhenSilentSeconds > 0 && idleMs > silentAfter;
        // An auto-hide fade in progress still wants full frames.
        bool fading = false;
        if (g_settings.autoHideEnabled) {
            ULONGLONG d = (ULONGLONG)(std::max(0.f, g_settings.autoHideDelaySeconds) * 1000.f);
            fading = idleMs > d && idleMs < d + 2000;
        }
        g_slowMode = silent && !fading;

        if (g_slowMode) {
            setEco(true);
            lastRenderQpc = 0;
            vsDue = absDue = 0;
            // Deep idle: loopback only, with a meter that works, and not
            // during the 1 s look after a meter wake.
            bool deep = g_settings.deepIdle && engine.IsOpen() && engine.IsLoopback() && engine.MeterOk() &&
                        idleMs > silentAfter + 5000 && now >= deepHoldUntil;
            if (deep) {
                if (meterProbe) {
                    // The look found nothing the engine calls audible (else
                    // we'd be playing): a steady floor between the meter's
                    // threshold and the engine's test. Wake only 6 dB above
                    // it for the next 30 s, so it can't cycle
                    // deep -> trickle -> deep four times a second.
                    meterProbe = false;
                    meterWake = std::clamp(2.f * meterProbePeak, kVizAudibleLin, 0.1f);
                    meterWakeRaisedUntil = now + 30000;
                    Wh_Log(L"[Idle] meter wake was a noise floor (%.1f dBFS); waking above %.1f dBFS for 30 s",
                           20.0 * log10(std::max(1e-9f, meterProbePeak)), 20.0 * log10(meterWake));
                }
                if (g_idleState.exchange((int)VizIdleState::Deep) != (int)VizIdleState::Deep) {
                    // One last tick so the renderer can settle on its final frame.
                    postTick(overlayWnd, now);
                }
                engine.StopStream();  // idempotent; also covers a reopen after a device change
                WaitForSingleObject(g_engineWake, 250);
                if (now > meterWakeRaisedUntil) meterWake = kVizAudibleLin;
                float peak = 0.f;
                bool ok = engine.ReadMeter(&peak);
                float gain = (g_settings.inputGainDb == 0.0f) ? 1.0f : powf(10.f, g_settings.inputGainDb / 20.f);
                if (!ok || peak * gain > meterWake) {
                    // A failed read (MeterOk() is now false, so no more deep
                    // idle on this stream) or something above the audible
                    // level: back to trickle, stream running, and let the
                    // engine's own test decide whether this is playing.
                    engine.StartStream();
                    g_idleState.store((int)VizIdleState::Trickle);
                    lastEngineQpc = 0;
                    trickleDue = 0;
                    if (ok) {
                        deepHoldUntil = GetTickCount64() + 1000;
                        meterProbe = true;
                        meterProbePeak = peak * gain;
                    }
                }
                continue;
            }
            if (engine.IsStopped()) engine.StartStream();
            g_idleState.store((int)VizIdleState::Trickle);
            // Loopback: wake on audio (only signalled while something renders
            // to the device), the wake event, or the next 250 ms frame. An
            // input device signals every device period, sound or not, so for
            // one the thread just polls at the trickle rate.
            ULONGLONG t0 = GetTickCount64();
            if (trickleDue == 0 || trickleDue > t0 + 250) trickleDue = t0;
            DWORD timeout = (DWORD)(trickleDue > t0 ? trickleDue - t0 : 0);
            HANDLE hs[2];
            DWORD nh = 0;
            if (engine.IsLoopback() && engine.AudioEvent()) hs[nh++] = engine.AudioEvent();
            if (g_engineWake) hs[nh++] = g_engineWake;
            if (timeout > 0) {
                if (nh) WaitForMultipleObjects(nh, hs, FALSE, timeout);
                else Sleep(timeout);
            }
            if (!g_renderThreadRunning.load(std::memory_order_relaxed) || g_unloading.load()) break;
            // Drain on every wake (cheap); analyse and draw only when it got
            // loud, or when the 250 ms frame is due.
            bool loud = engine.Pump();
            ULONGLONG t1 = GetTickCount64();
            if (loud || t1 >= trickleDue) {
                trickleDue = t1 + 250;
                // Always tick: the bars may have settled while a peak cap is
                // still falling, and the renderer's skip test makes a tick
                // with nothing new cost no present.
                engineFrame();
                postTick(overlayWnd, t1);
            }
            continue;
        }
        if (engine.IsStopped()) engine.StartStream();
        g_idleState.store((int)VizIdleState::Playing);
        setEco(false);
        trickleDue = 0;
        meterProbe = false;
        deepHoldUntil = 0;
        meterWake = kVizAudibleLin;
        meterWakeRaisedUntil = 0;

        LARGE_INTEGER nowQpc;
        QueryPerformanceCounter(&nowQpc);

        // ---- Pacing (1.5) ---------------------------------------------------------
        bool scheduled = false;
        if (g_smoothActive.load(std::memory_order_relaxed)) {
            if (vsPeriod == 0 || nowQpc.QuadPart - vsLastSync > qpcFreq.QuadPart / 2) {
                DWM_TIMING_INFO ti = {};
                ti.cbSize = sizeof(ti);
                if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &ti)) &&
                    ti.qpcRefreshPeriod >= (UINT64)(qpcFreq.QuadPart / 1000) &&
                    ti.qpcRefreshPeriod <= (UINT64)(qpcFreq.QuadPart / 20) && ti.qpcVBlank > 0) {
                    vsPeriod = (LONGLONG)ti.qpcRefreshPeriod;
                    vsVblank = (LONGLONG)ti.qpcVBlank;
                } else {
                    vsPeriod = 0;
                }
                vsLastSync = nowQpc.QuadPart;
            }
            if (vsPeriod > 0) {
                if (vsDue == 0) vsDue = VizVsyncSchedule::AlignUp(nowQpc.QuadPart, vsVblank, vsPeriod, 0.5);
                if (nowQpc.QuadPart < vsDue) {
                    preciseWait((double)(vsDue - nowQpc.QuadPart) * 1000.0 / (double)qpcFreq.QuadPart);
                    continue;
                }
                int divisor = VizVsyncSchedule::Divisor((double)fps, vsPeriod, qpcFreq.QuadPart);
                vsDue = VizVsyncSchedule::Next(vsDue, nowQpc.QuadPart, vsVblank, vsPeriod, divisor);
                absDue = 0;
            } else {
                LONGLONG interval = (LONGLONG)((double)qpcFreq.QuadPart * intervalMs / 1000.0);
                if (absDue == 0) absDue = nowQpc.QuadPart;
                if (nowQpc.QuadPart < absDue) {
                    preciseWait((double)(absDue - nowQpc.QuadPart) * 1000.0 / (double)qpcFreq.QuadPart);
                    continue;
                }
                absDue += interval;
                if (absDue <= nowQpc.QuadPart) absDue = nowQpc.QuadPart + interval;
                vsDue = 0;
            }
            lastRenderQpc = nowQpc.QuadPart;
            scheduled = true;
        } else {
            vsDue = absDue = 0;
            // Target FPS 0 needs the refresh rate even without Smooth Mode.
            if (g_settings.targetFps <= 0 && (vsPeriod == 0 || nowQpc.QuadPart - vsLastSync > qpcFreq.QuadPart * 2)) {
                DWM_TIMING_INFO ti = {};
                ti.cbSize = sizeof(ti);
                if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &ti)) && ti.qpcRefreshPeriod > 0)
                    vsPeriod = (LONGLONG)ti.qpcRefreshPeriod;
                vsLastSync = nowQpc.QuadPart;
            }
        }

        if (!scheduled) {
            double elapsedMs = (lastRenderQpc == 0)
                                   ? intervalMs
                                   : (double)(nowQpc.QuadPart - lastRenderQpc) * 1000.0 / (double)qpcFreq.QuadPart;
            if (elapsedMs < intervalMs) {
                preciseWait(intervalMs - elapsedMs);
                continue;
            }
            lastRenderQpc = nowQpc.QuadPart;
        }

        if (!g_renderThreadRunning.load(std::memory_order_relaxed) || g_unloading.load()) break;

        engineFrame();
        postTick(overlayWnd, GetTickCount64());
    }

    engine.ThreadExit();
    if (hTimer) CloseHandle(hTimer);
    if (SUCCEEDED(comHr)) CoUninitialize();
}

void StartRenderThread() {
    if (g_renderThread) return;
    if (!g_engineWake) g_engineWake = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_engineClosed) g_engineClosed = CreateEvent(nullptr, TRUE, TRUE, nullptr);
    g_renderThreadRunning.store(true, std::memory_order_relaxed);
    g_renderThread = new std::thread(RenderThreadProc);
    HANDLE hRenderThread = g_renderThread->native_handle();
    SetThreadDescription(hRenderThread, L"TourneTable-Engine");
}

void StopRenderThread() {
    g_renderThreadRunning.store(false, std::memory_order_relaxed);
    if (g_engineWake) SetEvent(g_engineWake);
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
        ttgfx::PresentBlank();
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

// ---- Place In = Panel -------------------------------------------------

void VizPanelLoad() {
    WCHAR buf[160] = {};
    Wh_GetStringValue(L"panel", buf, ARRAYSIZE(buf));
    int x = 0, y = 0, w = 0, h = 0, pinned = 0, hidden = 0;
    float zoom = 1.0f;
    if (swscanf_s(buf, L"%d %d %d %d %f %d %d", &x, &y, &w, &h, &zoom, &pinned, &hidden) >= 5 && w > 0 && h > 0) {
        g_panelRect = RECT{x, y, x + w, y + h};
        g_panelZoom = std::clamp(zoom, 0.25f, 6.0f);
        g_panelPinned = pinned != 0;
        g_panelHidden = hidden != 0;
    }
}

void VizPanelSave() {
    if (!g_overlayInPanel || !g_overlayWnd) return;
    RECT r;
    if (!GetWindowRect(g_overlayWnd, &r)) return;
    g_panelRect = r;
    WCHAR buf[160];
    swprintf_s(buf, L"%d %d %d %d %.4f %d %d", (int)r.left, (int)r.top, (int)(r.right - r.left),
               (int)(r.bottom - r.top), g_panelZoom, g_panelPinned ? 1 : 0, g_panelHidden ? 1 : 0);
    Wh_SetStringValue(L"panel", buf);
}

// Acrylic or Mica behind the box (Windows 11 22H2 and later; elsewhere the
// attribute is refused and the panel stays clear).
void VizPanelBackdrop() {
    if (!g_overlayInPanel || !g_overlayWnd) return;
    int type = g_settings.panelBackdrop == 2 ? 2 : g_settings.panelBackdrop == 1 ? 3 : 1;  // DWMSBT_*
    MARGINS margins = {type == 1 ? 0 : -1, type == 1 ? 0 : -1, type == 1 ? 0 : -1, type == 1 ? 0 : -1};
    DwmExtendFrameIntoClientArea(g_overlayWnd, &margins);
    BOOL dark = TRUE;
    DwmSetWindowAttribute(g_overlayWnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &dark, sizeof(dark));
    int corners = 2;  // DWMWCP_ROUND
    DwmSetWindowAttribute(g_overlayWnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &corners, sizeof(corners));
    DwmSetWindowAttribute(g_overlayWnd, 38 /* DWMWA_SYSTEMBACKDROP_TYPE */, &type, sizeof(type));
}

// "Ctrl+Alt+V", "Win+Shift+F9", "Ctrl+Space": modifiers and one key.
bool VizParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    size_t at = 0;
    while (at < text.size()) {
        size_t plus = text.find(L'+', at);
        std::wstring part = text.substr(at, plus == std::wstring::npos ? std::wstring::npos : plus - at);
        while (!part.empty() && iswspace(part.front())) part.erase(part.begin());
        while (!part.empty() && iswspace(part.back())) part.pop_back();
        for (auto& c : part) c = (WCHAR)towlower(c);
        if (part == L"ctrl" || part == L"control") *mods |= MOD_CONTROL;
        else if (part == L"alt") *mods |= MOD_ALT;
        else if (part == L"shift") *mods |= MOD_SHIFT;
        else if (part == L"win") *mods |= MOD_WIN;
        else if (part.size() == 1 && iswalnum(part[0])) *vk = (UINT)towupper(part[0]);
        else if (part.size() >= 2 && part.size() <= 3 && part[0] == L'f' && iswdigit(part[1])) {
            int n = _wtoi(part.c_str() + 1);
            if (n < 1 || n > 24) return false;
            *vk = VK_F1 + n - 1;
        } else if (part == L"space") *vk = VK_SPACE;
        else if (part == L"pause") *vk = VK_PAUSE;
        else if (part == L"insert") *vk = VK_INSERT;
        else if (part == L"home") *vk = VK_HOME;
        else if (part == L"end") *vk = VK_END;
        else return false;
        if (plus == std::wstring::npos) break;
        at = plus + 1;
    }
    return *vk != 0;
}

constexpr int kVizPanelHotkeyId = 0x7454;

// The Panel Hotkey, on the message window (re-done when settings change).
void VizPanelHotkey() {
    if (!g_messageWnd) return;
    UnregisterHotKey(g_messageWnd, kVizPanelHotkeyId);
    UINT mods, vk;
    if (!g_settings.hostPanel || g_settings.panelHotkey.empty()) return;
    if (!VizParseHotkey(g_settings.panelHotkey, &mods, &vk)) {
        Wh_Log(L"[Panel] hotkey '%s' isn't one this understands", g_settings.panelHotkey.c_str());
        return;
    }
    if (!RegisterHotKey(g_messageWnd, kVizPanelHotkeyId, mods | MOD_NOREPEAT, vk))
        Wh_Log(L"[Panel] hotkey '%s' is taken by another program", g_settings.panelHotkey.c_str());
}

void VizPanelShow(bool show) {
    if (!g_overlayInPanel || !g_overlayWnd) return;
    g_panelHidden = !show;
    if (show) {
        ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);
        // (a fullscreen app or Pause Visualizer stays paused: the 1 s check)
        if (!g_userPaused.load()) ResumeFromFullscreen();
    } else {
        ShowWindow(g_overlayWnd, SW_HIDE);
        PauseForFullscreen();
    }
    if (g_mediaWnd) RepositionAndRepaintMediaControls();
    VizPanelSave();
}

void VizPanelPin(bool pinned) {
    if (!g_overlayInPanel || !g_overlayWnd) return;
    g_panelPinned = pinned;
    SetWindowPos(g_overlayWnd, pinned ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    VizPanelSave();
}

// The window takes the box's size, around the same centre.
void VizPanelFit() {
    if (!g_overlayInPanel || !g_overlayWnd || g_panelSizingW) return;
    VizLayout layout;
    if (!ComputeVizLayout(&layout)) return;
    RECT r;
    if (!GetWindowRect(g_overlayWnd, &r)) return;
    if (r.right - r.left == (LONG)layout.width && r.bottom - r.top == (LONG)layout.height) return;
    int cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    SetWindowPos(g_overlayWnd, nullptr, cx - (int)layout.width / 2, cy - (int)layout.height / 2, (int)layout.width,
                 (int)layout.height, SWP_NOZORDER | SWP_NOACTIVATE);
    VizPanelSave();
}

// A panel left off every screen (a monitor unplugged) comes back to the main one.
void VizPanelKeepOnScreen() {
    if (!g_overlayInPanel || !g_overlayWnd) return;
    RECT r;
    if (!GetWindowRect(g_overlayWnd, &r) || MonitorFromRect(&r, MONITOR_DEFAULTTONULL)) return;
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY), &mi)) return;
    int w = r.right - r.left, h = r.bottom - r.top;
    SetWindowPos(g_overlayWnd, nullptr, (mi.rcWork.left + mi.rcWork.right - w) / 2, (mi.rcWork.top + mi.rcWork.bottom - h) / 2,
                 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    VizPanelSave();
}

void VizCreatePanelWindow(HINSTANCE hInstance) {
    if (!g_panelMsg) g_panelMsg = RegisterWindowMessageW(L"TourneTable.Panel");
    VizPanelLoad();
    RECT r = g_panelRect;
    if (r.right <= r.left || r.bottom <= r.top) {
        // First time: a guess, centred on the chosen monitor; the box sets the real size.
        HMONITOR m = GetMonitorById(g_settings.monitor - 1);
        if (!m) m = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi{.cbSize = sizeof(mi)};
        GetMonitorInfo(m, &mi);
        int w = 480, h = 220;
        r.left = (mi.rcWork.left + mi.rcWork.right - w) / 2;
        r.top = (mi.rcWork.top + mi.rcWork.bottom - h) / 2;
        r.right = r.left + w;
        r.bottom = r.top + h;
    }
    g_overlayInPanel = true;  // (before it exists: its first WM_NCCALCSIZE asks)
    g_overlayInTaskbar = false;
    g_overlayWnd = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW | (g_panelPinned ? WS_EX_TOPMOST : 0),
                                  OVERLAY_WINDOW_CLASS, L"Tourne'Table", WS_POPUP | WS_THICKFRAME, r.left, r.top,
                                  r.right - r.left, r.bottom - r.top, nullptr, nullptr, hInstance, nullptr);
    if (!g_overlayWnd) {
        g_overlayInPanel = false;
        return;
    }
    VizPanelBackdrop();
    if (!g_panelHidden) ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);
    VizPanelKeepOnScreen();
    VizPanelHotkey();
}

// Within a few pixels of an edge: that edge (to resize); anywhere else moves it.
LRESULT VizPanelHitTest(HWND hWnd, LPARAM lParam) {
    POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    RECT r;
    GetWindowRect(hWnd, &r);
    int band = MulDiv(6, (int)GetDpiForWindow(hWnd), 96);
    bool left = pt.x < r.left + band, right = pt.x >= r.right - band;
    bool top = pt.y < r.top + band, bottom = pt.y >= r.bottom - band;
    if (top && left) return HTTOPLEFT;
    if (top && right) return HTTOPRIGHT;
    if (bottom && left) return HTBOTTOMLEFT;
    if (bottom && right) return HTBOTTOMRIGHT;
    if (left) return HTLEFT;
    if (right) return HTRIGHT;
    if (top) return HTTOP;
    if (bottom) return HTBOTTOM;
    return HTCAPTION;
}

// Resizing keeps the box's shape.
void VizPanelKeepAspect(UINT edge, RECT* r) {
    if (g_panelSizingW <= 0 || g_panelSizingH <= 0) return;
    float aspect = (float)g_panelSizingW / (float)g_panelSizingH;
    int minW = std::max(40, (int)lroundf(40.0f * aspect));
    if (r->right - r->left < minW) {
        if (edge == WMSZ_LEFT || edge == WMSZ_TOPLEFT || edge == WMSZ_BOTTOMLEFT) r->left = r->right - minW;
        else r->right = r->left + minW;
    }
    int w = r->right - r->left, h = r->bottom - r->top;
    if (edge == WMSZ_TOP || edge == WMSZ_BOTTOM) {
        w = (int)lroundf(h * aspect);
        r->right = r->left + w;
    } else {
        h = (int)lroundf(w / aspect);
        if (edge == WMSZ_TOP || edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT) r->top = r->bottom - h;
        else r->bottom = r->top + h;
    }
}

// A resize ends: the zoom follows the new width, and the box is drawn again at it.
void VizPanelEndSizing() {
    if (!g_overlayInPanel || !g_overlayWnd) return;
    RECT r;
    int startW = g_panelSizingW;
    g_panelSizingW = g_panelSizingH = 0;
    if (startW > 0 && GetWindowRect(g_overlayWnd, &r) && r.right - r.left != startW) {
        g_panelZoom = std::clamp(g_panelZoom * (float)(r.right - r.left) / (float)startW, 0.25f, 6.0f);
        Wh_Log(L"[Panel] zoom %.2f", g_panelZoom);
        ReleaseSwapChainResources();
        if (CreateSwapChainResources() && !g_fullscreenPaused.load()) RenderVisualizer();
    }
    VizPanelFit();
    VizPanelSave();
}

void HandleDisplayChange() {
    if (g_mediaWnd) RepositionAndRepaintMediaControls();

    if (!g_overlayWnd) return;

    if (g_overlayInPanel) {
        VizPanelKeepOnScreen();
    } else if (g_overlayInTaskbar) {
        // A top-level window over the whole virtual screen (see CreateOverlayWindow).
        SetWindowPos(g_overlayWnd, nullptr, GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN),
                     GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN),
                     SWP_NOZORDER | SWP_NOACTIVATE);
    } else {
        HWND hWorkerW = GetParent(g_overlayWnd);
        if (!hWorkerW) return;

        RECT rc;
        GetWindowRect(hWorkerW, &rc);
        SetWindowPos(g_overlayWnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }

    HMONITOR monitor = GetMonitorById(g_settings.monitor - 1);
    if (!monitor) monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);
    g_cachedMonitor = monitor;
    float newDpiScale = VizHostDpiScale(monitor);
    if (VizRenderDeviceNeedsRebuild()) {
        // A GPU was added, removed or enabled, or the widget's monitor now
        // hangs off a different one: Auto follows it. The rebuild re-reads
        // DPI and redraws on its own.
        VizRebuildRenderDevice(L"display configuration changed");
    } else if (newDpiScale != g_dpiScale) {
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
    if (g_overlayInPanel && g_panelMsg && uMsg == g_panelMsg) {
        if (wParam == 0) VizPanelShow(g_panelHidden);
        else if (wParam == 1) VizPanelShow(true);
        else if (wParam == 2) VizPanelShow(false);
        else if (wParam == 3) VizPanelPin(!g_panelPinned);
        return g_panelHidden ? 0 : 1;
    }
    if (g_overlayInPanel) {
        switch (uMsg) {
            case WM_NCCALCSIZE:
                if (wParam) return 0;  // all of it is client: no frame drawn
                break;
            case WM_NCHITTEST:
                return VizPanelHitTest(hWnd, lParam);
            case WM_NCLBUTTONDBLCLK:
                return 0;  // (no maximize)
            case WM_SYSCOMMAND:
                if ((wParam & 0xFFF0) == SC_MAXIMIZE || (wParam & 0xFFF0) == SC_MINIMIZE) return 0;
                break;
            case WM_ENTERSIZEMOVE: {
                RECT r;
                GetWindowRect(hWnd, &r);
                g_panelSizingW = r.right - r.left;
                g_panelSizingH = r.bottom - r.top;
                break;
            }
            case WM_SIZING:
                VizPanelKeepAspect((UINT)wParam, (RECT*)lParam);
                return TRUE;
            case WM_EXITSIZEMOVE:
                VizPanelEndSizing();
                break;
            case WM_CONTEXTMENU:
                // (a right-click the mouse hook didn't take: off the box while resizing)
                if (g_messageWnd && g_settings.contextMenu != VizContextMenu::Off &&
                    (g_settings.contextMenu == VizContextMenu::RightClick || (GetKeyState(VK_CONTROL) & 0x8000)))
                    PostMessage(g_messageWnd, WM_APP_CONTEXT_MENU, (WPARAM)GET_X_LPARAM(lParam), (LPARAM)GET_Y_LPARAM(lParam));
                return 0;
            case WM_CLOSE:
                VizPanelShow(false);  // (Alt+F4 hides it; the menu, hotkey or bar button bring it back)
                return 0;
            case WM_APP_PANEL_FIT:
                VizPanelFit();
                return 0;
        }
    }
    switch (uMsg) {
        case WM_APP_RENDER_TICK:
            g_renderTickPending.store(false, std::memory_order_relaxed);
            if (!g_unloading && !g_fullscreenPaused.load()) {
                RenderVisualizer();
            }
            return 0;

        case WM_WINDOWPOSCHANGED: {
            const WINDOWPOS* wp = (const WINDOWPOS*)lParam;
            if (g_overlayInPanel && !(wp->flags & SWP_NOMOVE) && !g_unloading) {
                VizLayout layout;
                if (ComputeVizLayout(&layout)) VizPublishDrawRect(layout);
            }
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

        // A keyboard nudge changed where the box belongs. Rendering is often
        // idle while someone is placing it (no audio playing), so the move
        // would otherwise not show up until the next time music started.
        case WM_APP_FORCE_REDRAW:
            if (!g_unloading && !g_fullscreenPaused.load()) {
                RenderVisualizer();
            }
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ---- Right-click menu: live settings ---------------------------------------------------
//
// Windhawk mods can read their settings but not write them, so anything
// changed from this menu is kept as a mod-owned value (Wh_SetStringValue, the
// same store the dragged position uses) and laid over the Windhawk settings
// every time they load. The menu's last item clears them all, handing control
// back to the settings page. Until then the settings page shows the values
// underneath, which is why the menu says how many quick settings are active.

std::vector<std::pair<std::wstring, std::wstring>> g_menuOverrides;

void VizLoadMenuOverrides() {
    g_menuOverrides.clear();
    WCHAR buf[4096] = {};
    if (!Wh_GetStringValue(L"menuOverrides", buf, ARRAYSIZE(buf))) return;
    std::wstring s = buf;
    size_t pos = 0;
    while (pos < s.size()) {
        size_t nl = s.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = s.size();
        std::wstring line = s.substr(pos, nl - pos);
        size_t eq = line.find(L'=');
        if (eq != std::wstring::npos && eq > 0) g_menuOverrides.emplace_back(line.substr(0, eq), line.substr(eq + 1));
        pos = nl + 1;
    }
}

void VizSaveMenuOverrides() {
    std::wstring s;
    for (const auto& kv : g_menuOverrides) s += kv.first + L"=" + kv.second + L"\n";
    Wh_SetStringValue(L"menuOverrides", s.c_str());
}

void VizSetMenuOverride(const std::wstring& key, const std::wstring& value) {
    for (auto& kv : g_menuOverrides)
        if (kv.first == key) {
            kv.second = value;
            VizSaveMenuOverrides();
            return;
        }
    g_menuOverrides.emplace_back(key, value);
    VizSaveMenuOverrides();
}

// Option tables shared by the menu (labels) and the overrides (values). The
// values are the same strings the settings YAML uses.
struct VizMenuOption {
    const wchar_t* value;
    const wchar_t* label;
};
static const VizMenuOption kShapes[] = {
    {L"stereo", L"Stereo"},     {L"mountain", L"Mountain"},         {L"mirror", L"Mirror"},
    {L"wave", L"Wave"},         {L"breathe", L"Breathe"},           {L"dots", L"Dots"},
    {L"radial", L"Radial"},     {L"oscilloscope", L"Oscilloscope"}, {L"goniometer", L"Goniometer"},
    {L"terminal", L"Terminal"},
    // Styles (2.1), in VizStyle order after the shapes (see VizShapeMenuIndex).
    {L"led", L"LED Meter"},     {L"line", L"Line Spectrum"},        {L"bloom", L"Polar Bloom"},
    {L"spectrogram", L"Spectrogram"}, {L"vu", L"VU Needles"},       {L"stereo_field", L"Stereo Field"},
    {L"particles", L"Particles"}};
static const VizMenuOption kColorModes[] = {
    {L"solid", L"Solid"},          {L"gradient", L"Gradient"},           {L"reactive_gradient", L"Reactive Gradient"},
    {L"accent", L"Windows Accent"}, {L"album_art", L"Album Art"},         {L"dynamic_album", L"Dynamic Album"},
    {L"acrylic", L"Acrylic"},      {L"rainbow", L"Rainbow Cycle"},       {L"tourne", L"Tourne"}};
static const VizMenuOption kTermStyles[] = {
    {L"columns", L"Columns"}, {L"waterfall", L"Waterfall"}, {L"meters", L"Meters"}};
static const VizMenuOption kEngines[] = {{L"precision", L"Precision"}, {L"classic", L"Classic (1.4)"}};
static const VizMenuOption kLayouts[] = {
    {L"scale", L"Frequency Scale (Bar Count)"}, {L"iec:3", L"IEC 1/3 octave"},  {L"iec:6", L"IEC 1/6 octave"},
    {L"iec:12", L"IEC 1/12 octave"},            {L"iec:24", L"IEC 1/24 octave"}, {L"musical:12", L"Musical notes"},
    {L"musical:24", L"Musical quarter tones"}};
static const VizMenuOption kWeightings[] = {{L"z", L"Z (flat)"}, {L"a", L"A"}, {L"c", L"C"}};
static const VizMenuOption kBallistics[] = {
    {L"snappy", L"Snappy"}, {L"smooth", L"Smooth"},       {L"analyzer", L"Analyzer (20 dB/s)"},
    {L"vu", L"VU"},         {L"ppm_ebu", L"PPM, EBU"},    {L"ppm_din", L"PPM, DIN"}};
static const VizMenuOption kWorkloads[] = {
    {L"hybrid", L"Hybrid (CPU analysis, GPU drawing)"}, {L"gpu", L"GPU"}, {L"cpu", L"CPU"}};
static const VizMenuOption kRenderers[] = {{L"d3d11", L"Direct3D 11"}, {L"direct2d", L"Direct2D (1.5)"}};
static const VizMenuOption kFps[] = {{L"0", L"Match display"}, {L"30", L"30"}, {L"60", L"60"},
                                     {L"120", L"120"},         {L"144", L"144"}, {L"240", L"240"}};
static const VizMenuOption kReadouts[] = {
    {L"off", L"Off"},           {L"frequency", L"Peak frequency"}, {L"loudness", L"Loudness"},
    {L"loudness_full", L"Loudness, true peak, PLR, correlation"}, {L"both", L"Peak frequency and loudness"}};

// Applied at the end of LoadSettings, before anything derived from settings.
void VizApplyMenuOverrides() {
    for (const auto& kv : g_menuOverrides) {
        const std::wstring& k = kv.first;
        PCWSTR v = kv.second.c_str();
        auto is = [&](PCWSTR s) { return wcscmp(v, s) == 0; };
        if (k == L"shape") {
            VizParseShape(v, &g_settings.shape, &g_settings.style);
        } else if (k == L"colorMode") {
            g_settings.colorMode = is(L"gradient") ? VizColorMode::Gradient
                                 : is(L"reactive_gradient") ? VizColorMode::ReactiveGradient
                                 : is(L"accent") ? VizColorMode::Accent : is(L"album_art") ? VizColorMode::AlbumArt
                                 : is(L"dynamic_album") ? VizColorMode::DynamicAlbum
                                 : is(L"acrylic") ? VizColorMode::Acrylic : is(L"rainbow") ? VizColorMode::RainbowCycle
                                 : is(L"tourne") ? VizColorMode::Tourne : VizColorMode::Solid;
        } else if (k == L"termStyle") {
            g_settings.termStyle = is(L"waterfall") ? VizTermStyle::Waterfall
                                 : is(L"meters") ? VizTermStyle::Meters : VizTermStyle::Columns;
        } else if (k == L"engine") {
            g_settings.engine = is(L"classic") ? VizEngineKind::Classic : VizEngineKind::Precision;
        } else if (k == L"bandLayout") {
            std::wstring s = v;
            size_t c = s.find(L':');
            std::wstring kind = s.substr(0, c);
            int frac = (c == std::wstring::npos) ? 0 : _wtoi(s.c_str() + c + 1);
            g_settings.bandLayout = (kind == L"iec") ? VizBandLayout::Iec
                                  : (kind == L"musical") ? VizBandLayout::Musical : VizBandLayout::Scale;
            if (frac == 1 || frac == 3 || frac == 6 || frac == 12 || frac == 24) g_settings.octaveFraction = frac;
        } else if (k == L"weighting") {
            g_settings.weighting = is(L"a") ? VizWeighting::A : is(L"c") ? VizWeighting::C : VizWeighting::Z;
        } else if (k == L"ballistics") {
            g_settings.ballistics = is(L"smooth") ? VizBallisticsPreset::Smooth
                                  : is(L"analyzer") ? VizBallisticsPreset::Analyzer
                                  : is(L"vu") ? VizBallisticsPreset::Vu : is(L"ppm_ebu") ? VizBallisticsPreset::PpmEbu
                                  : is(L"ppm_din") ? VizBallisticsPreset::PpmDin : VizBallisticsPreset::Snappy;
        } else if (k == L"workload") {
            g_settings.workload = is(L"gpu") ? VizWorkload::Gpu : is(L"cpu") ? VizWorkload::Cpu
                                                     : VizWorkload::Hybrid;
        } else if (k == L"renderer") {
            g_settings.renderer = is(L"direct2d") ? VizRenderer::Direct2D : VizRenderer::D3D11;
        } else if (k == L"targetFps") {
            g_settings.targetFps = std::clamp(_wtoi(v), 0, 1000);
        } else if (k == L"readout") {
            g_settings.peakFreqEnabled = !is(L"off");
            if (!is(L"off"))
                g_settings.readout = is(L"loudness") ? VizReadout::Loudness
                                   : is(L"loudness_full") ? VizReadout::LoudnessFull
                                   : is(L"both") ? VizReadout::Both : VizReadout::Frequency;
        } else if (k == L"audioSource") {
            g_settings.audioSourceKey = v;
        } else if (k == L"peakHold") {
            g_settings.peakHoldEnabled = is(L"1");
        } else if (k == L"beatFlash") {
            g_settings.beatFlashEnabled = is(L"1");
        } else if (k == L"nowPlaying") {
            g_settings.nowPlayingEnabled = is(L"1");
        } else if (k == L"progress") {
            g_settings.progressEnabled = is(L"1");
        } else if (k == L"mediaControls") {
            g_settings.mediaControlsEnabled = is(L"1");
        } else if (k == L"pixelText") {
            g_settings.textPixel = is(L"1");
        } else if (k == L"pixelSnap") {
            g_settings.pixelSnap = is(L"1");
        } else if (k == L"fineNudge") {
            g_settings.keyMoveFine = is(L"1");
        // The look, from the Style Editor and saved styles (2.1).
        } else if (k == L"barWidth") {
            g_settings.barWidth = std::clamp((float)_wtof(v), 0.1f, 400.f);
        } else if (k == L"barGap") {
            g_settings.barGap = std::clamp((float)_wtof(v), 0.f, 400.f);
        } else if (k == L"barMaxSize") {
            g_settings.barMaxSize = std::clamp((float)_wtof(v), 2.f, 4000.f);
        } else if (k == L"barRadius") {
            float rad = std::clamp((float)_wtof(v), 0.f, 100.f);
            g_settings.barRadiusTL = g_settings.barRadiusTR = g_settings.barRadiusBR = g_settings.barRadiusBL = rad;
        } else if (k == L"reflection") {
            g_settings.reflection = std::clamp((float)_wtof(v), 0.f, 100.f);
        } else if (k == L"fxGlow") {
            g_settings.fxGlow = std::clamp((float)_wtof(v), 0.f, 100.f);
        } else if (k == L"fxGlowRadius") {
            g_settings.fxGlowRadius = std::clamp((float)_wtof(v), 0.5f, 32.f);
        } else if (k == L"fxBloom") {
            g_settings.fxBloom = std::clamp((float)_wtof(v), 0.f, 100.f);
        } else if (k == L"fxBloomRadius") {
            g_settings.fxBloomRadius = std::clamp((float)_wtof(v), 4.f, 64.f);
        } else if (k == L"color") {
            ParseColorHex(v, &g_settings.colorA, &g_settings.colorR, &g_settings.colorG, &g_settings.colorB);
        } else if (k == L"grad1") {
            ParseColorHex(v, &g_settings.grad1A, &g_settings.grad1R, &g_settings.grad1G, &g_settings.grad1B);
        } else if (k == L"grad2") {
            ParseColorHex(v, &g_settings.grad2A, &g_settings.grad2R, &g_settings.grad2G, &g_settings.grad2B);
        }
    }
}

// What the menu shows as checked: the current effective value as a string.
static std::wstring CurrentValue(const std::wstring& key) {
    auto pick = [](const VizMenuOption* opts, size_t n, int index) -> std::wstring {
        return (index >= 0 && (size_t)index < n) ? opts[index].value : L"";
    };
    if (key == L"shape") return pick(kShapes, ARRAYSIZE(kShapes), VizShapeMenuIndex());
    if (key == L"colorMode") return pick(kColorModes, ARRAYSIZE(kColorModes), (int)g_settings.colorMode);
    if (key == L"termStyle") return pick(kTermStyles, ARRAYSIZE(kTermStyles), (int)g_settings.termStyle);
    if (key == L"engine") return pick(kEngines, ARRAYSIZE(kEngines), (int)g_settings.engine);
    if (key == L"weighting") return pick(kWeightings, ARRAYSIZE(kWeightings), (int)g_settings.weighting);
    if (key == L"ballistics") return pick(kBallistics, ARRAYSIZE(kBallistics), (int)g_settings.ballistics);
    if (key == L"workload") return pick(kWorkloads, ARRAYSIZE(kWorkloads), (int)g_settings.workload);
    if (key == L"renderer") return pick(kRenderers, ARRAYSIZE(kRenderers), (int)g_settings.renderer);
    if (key == L"targetFps") return std::to_wstring(g_settings.targetFps);
    if (key == L"bandLayout") {
        if (g_settings.bandLayout == VizBandLayout::Scale) return L"scale";
        return std::wstring(g_settings.bandLayout == VizBandLayout::Iec ? L"iec:" : L"musical:") +
               std::to_wstring(g_settings.octaveFraction);
    }
    if (key == L"readout") {
        if (!g_settings.peakFreqEnabled) return L"off";
        return g_settings.readout == VizReadout::Loudness       ? L"loudness"
               : g_settings.readout == VizReadout::LoudnessFull ? L"loudness_full"
               : g_settings.readout == VizReadout::Both         ? L"both"
                                                                : L"frequency";
    }
    return L"";
}

enum : UINT {
    kMenuToggleBase = 100,  // + index into kToggles
    kMenuPause = 190,
    kMenuReset = 191,
    kMenuCopy = 192,
    kMenuPanelPin = 193,
    kMenuPanelHide = 194,
    kMenuDefaultOut = 200,
    kMenuDefaultIn = 201,
    kMenuDeviceBase = 300,   // + endpoint index
    kMenuChoiceBase = 1000,  // + group * 100 + option
    kMenuStyleEditor = 4999,
    kMenuPresetBase = 5000,  // + saved style index
};

// Saved styles and the Style Editor.
std::vector<std::wstring> VizStylePresetNames();
bool VizApplyStylePreset(const std::wstring& name);
void VizOpenStyleEditor();
struct VizMenuGroup {
    const wchar_t* key;
    const wchar_t* label;
    const VizMenuOption* opts;
    size_t n;
};
static const VizMenuGroup kGroups[] = {
    {L"shape", L"Shape", kShapes, ARRAYSIZE(kShapes)},
    {L"termStyle", L"Terminal Style", kTermStyles, ARRAYSIZE(kTermStyles)},
    {L"colorMode", L"Color Mode", kColorModes, ARRAYSIZE(kColorModes)},
    {L"engine", L"Analysis Engine", kEngines, ARRAYSIZE(kEngines)},
    {L"bandLayout", L"Band Layout", kLayouts, ARRAYSIZE(kLayouts)},
    {L"weighting", L"Weighting", kWeightings, ARRAYSIZE(kWeightings)},
    {L"ballistics", L"Ballistics", kBallistics, ARRAYSIZE(kBallistics)},
    {L"readout", L"Readout", kReadouts, ARRAYSIZE(kReadouts)},
    {L"workload", L"Workload", kWorkloads, ARRAYSIZE(kWorkloads)},
    {L"renderer", L"Renderer", kRenderers, ARRAYSIZE(kRenderers)},
    {L"targetFps", L"Target FPS", kFps, ARRAYSIZE(kFps)},
};
struct VizMenuToggle {
    const wchar_t* key;
    const wchar_t* label;
    bool* field;
};

// Windhawk has no way for a mod to open or fill in its settings page, so this
// puts the active quick settings on the clipboard as "key = value" lines to
// carry over by hand. Keys and values are written as the menu shows them
// ("Peak Hold Caps = On", "Audio Source = <device name>"), close to the
// settings page's own labels, not as the internal override strings.
static void VizCopyMenuOverrides(const VizMenuToggle* toggles, size_t nToggles,
                                 const std::vector<VizAudioEndpoint>& eps) {
    std::wstring s;
    for (const auto& kv : g_menuOverrides) {
        std::wstring key = kv.first, value = kv.second;
        for (const VizMenuGroup& grp : kGroups) {
            if (kv.first != grp.key) continue;
            key = grp.label;
            for (size_t i = 0; i < grp.n; i++)
                if (kv.second == grp.opts[i].value) value = grp.opts[i].label;
        }
        for (size_t i = 0; i < nToggles; i++)
            if (kv.first == toggles[i].key) {
                key = toggles[i].label;
                value = (kv.second == L"1") ? L"On" : L"Off";
            }
        if (kv.first == L"audioSource") {
            key = L"Audio Source";
            if (kv.second == L"default_output") value = L"Default output";
            else if (kv.second == L"default_input") value = L"Default input";
            else
                for (const auto& ep : eps)
                    if (kv.second == L"id:" + ep.id) value = ep.name;
        }
        s += key + L" = " + value + L"\r\n";
    }
    if (s.empty() || !OpenClipboard(g_messageWnd)) return;
    EmptyClipboard();
    const size_t bytes = (s.size() + 1) * sizeof(wchar_t);
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    bool owned = false;  // true once the clipboard owns `mem`
    if (mem) {
        if (void* p = GlobalLock(mem)) {
            memcpy(p, s.c_str(), bytes);
            GlobalUnlock(mem);
            owned = SetClipboardData(CF_UNICODETEXT, mem) != nullptr;
        }
        if (!owned) GlobalFree(mem);
    }
    CloseClipboard();
}

void VizShowContextMenu(POINT pt) {
    // One menu at a time: a second request (e.g. the media strip, posted while
    // this menu's modal loop is dispatching messages) would nest and fail.
    if (!g_messageWnd || g_menuOpen.load(std::memory_order_acquire)) return;
    const VizMenuToggle toggles[] = {
        {L"peakHold", L"Peak Hold Caps", &g_settings.peakHoldEnabled},
        {L"beatFlash", L"Beat Flash", &g_settings.beatFlashEnabled},
        {L"nowPlaying", L"Now Playing Text", &g_settings.nowPlayingEnabled},
        {L"progress", L"Track Progress Bar", &g_settings.progressEnabled},
        {L"mediaControls", L"Media Controls", &g_settings.mediaControlsEnabled},
        {L"pixelText", L"Pixel-Sharp Text", &g_settings.textPixel},
        {L"pixelSnap", L"Pixel Snap (sharp edges)", &g_settings.pixelSnap},
        {L"fineNudge", L"Subpixel Nudges (keyboard)", &g_settings.keyMoveFine},
    };

    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING | MF_GRAYED, 0, L"Tourne'Table");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    // Audio source.
    ComPtr<IMMDeviceEnumerator> en;
    CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                     (void**)en.GetAddressOf());
    std::vector<VizAudioEndpoint> eps = VizListAudioEndpoints(en.Get());
    const std::wstring& src = g_settings.audioSourceKey;
    HMENU dev = CreatePopupMenu();
    AppendMenuW(dev, MF_STRING | ((src.empty() || src == L"default_output") ? MF_CHECKED : 0), kMenuDefaultOut,
                L"Default output (what you hear)");
    AppendMenuW(dev, MF_STRING | (src == L"default_input" ? MF_CHECKED : 0), kMenuDefaultIn, L"Default input");
    bool any[2] = {false, false};
    for (int pass = 0; pass < 2; pass++) {
        for (size_t i = 0; i < eps.size() && i < 600; i++) {
            if (eps[i].render != (pass == 0)) continue;
            if (!any[pass]) {
                AppendMenuW(dev, MF_SEPARATOR, 0, nullptr);
                AppendMenuW(dev, MF_STRING | MF_GRAYED, 0,
                            pass == 0 ? L"Outputs (captured by loopback)" : L"Inputs (mics, line-in, virtual cables)");
                any[pass] = true;
            }
            bool on = src == L"id:" + eps[i].id;
            AppendMenuW(dev, MF_STRING | (on ? MF_CHECKED : 0), kMenuDeviceBase + (UINT)i, eps[i].name.c_str());
        }
    }
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)dev, L"Audio Source");

    // My Styles: saved looks, one click each, and the editor that makes them.
    std::vector<std::wstring> presets = VizStylePresetNames();
    HMENU mine = CreatePopupMenu();
    for (size_t i = 0; i < presets.size() && i < 200; i++)
        AppendMenuW(mine, MF_STRING, kMenuPresetBase + (UINT)i, presets[i].c_str());
    if (presets.empty()) AppendMenuW(mine, MF_STRING | MF_GRAYED, 0, L"(none saved yet)");
    AppendMenuW(mine, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(mine, MF_STRING, kMenuStyleEditor, L"Style Editor...");
    AppendMenuW(menu, MF_POPUP, (UINT_PTR)mine, L"My Styles");

    for (size_t gi = 0; gi < ARRAYSIZE(kGroups); gi++) {
        const VizMenuGroup& grp = kGroups[gi];
        if (wcscmp(grp.key, L"termStyle") == 0 && g_settings.shape != VizShape::Terminal) continue;
        std::wstring cur = CurrentValue(grp.key);
        HMENU sub = CreatePopupMenu();
        for (size_t oi = 0; oi < grp.n; oi++)
            AppendMenuW(sub, MF_STRING | (cur == grp.opts[oi].value ? MF_CHECKED : 0),
                        kMenuChoiceBase + (UINT)(gi * 100 + oi), grp.opts[oi].label);
        AppendMenuW(menu, MF_POPUP, (UINT_PTR)sub, grp.label);
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    for (size_t ti = 0; ti < ARRAYSIZE(toggles); ti++)
        AppendMenuW(menu, MF_STRING | (*toggles[ti].field ? MF_CHECKED : 0), kMenuToggleBase + (UINT)ti,
                    toggles[ti].label);
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (g_userPaused.load() ? MF_CHECKED : 0), kMenuPause, L"Pause Visualizer");
    if (g_overlayInPanel) {
        AppendMenuW(menu, MF_STRING | (g_panelPinned ? MF_CHECKED : 0), kMenuPanelPin, L"Keep Panel on Top");
        AppendMenuW(menu, MF_STRING, kMenuPanelHide, L"Hide Panel");
    }
    std::wstring reset = L"Reset Quick Settings";
    if (!g_menuOverrides.empty()) reset += L" (" + std::to_wstring(g_menuOverrides.size()) + L" active)";
    AppendMenuW(menu, MF_STRING | (g_menuOverrides.empty() ? MF_GRAYED : 0), kMenuCopy, L"Copy Quick Settings");
    if (!g_setupLayerLoaded) VizSetupLoad();
    if (!g_setupLayer.empty()) reset += L" and Setup";
    AppendMenuW(menu, MF_STRING | (g_menuOverrides.empty() && g_setupLayer.empty() ? MF_GRAYED : 0), kMenuReset, reset.c_str());

    // A menu only closes on an outside click when its owner is foreground.
    // The right-click went to the desktop (the hook swallowed it), so Windows
    // would refuse SetForegroundWindow. An empty SendInput makes this process
    // the one that sent the last input, which allows it, without attaching to
    // another app's input queue (that could hang if the app isn't responding).
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(g_messageWnd);

    // While the menu is up the mouse hook passes every click through, so a
    // right-click elsewhere closes it instead of being swallowed.
    VizMediaMenuBegin();
    UINT cmd = (UINT)TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY, pt.x, pt.y, g_messageWnd,
                                      nullptr);
    VizMediaMenuEnd();
    PostMessage(g_messageWnd, WM_NULL, 0, 0);
    DestroyMenu(menu);  // destroys the submenus with it
    if (!cmd) return;

    if (cmd == kMenuStyleEditor) {
        VizOpenStyleEditor();
        return;
    } else if (cmd >= kMenuPresetBase && cmd < kMenuPresetBase + presets.size()) {
        if (!VizApplyStylePreset(presets[cmd - kMenuPresetBase])) return;
    } else if (cmd == kMenuCopy) {
        VizCopyMenuOverrides(toggles, ARRAYSIZE(toggles), eps);
        return;
    } else if (cmd == kMenuReset) {
        g_menuOverrides.clear();
        if (!g_setupLayerLoaded) VizSetupLoad();
        g_setupLayer.clear();
        VizSetupSave();
        VizSaveMenuOverrides();
    } else if (cmd == kMenuPanelPin) {
        VizPanelPin(!g_panelPinned);
        return;
    } else if (cmd == kMenuPanelHide) {
        VizPanelShow(false);
        return;
    } else if (cmd == kMenuPause) {
        g_userPaused.store(!g_userPaused.load());
        if (g_userPaused.load()) PauseForFullscreen();
        else ResumeFromFullscreen();
        return;
    } else if (cmd == kMenuDefaultOut) {
        VizSetMenuOverride(L"audioSource", L"default_output");
    } else if (cmd == kMenuDefaultIn) {
        VizSetMenuOverride(L"audioSource", L"default_input");
    } else if (cmd >= kMenuDeviceBase && cmd < kMenuDeviceBase + eps.size()) {
        VizSetMenuOverride(L"audioSource", L"id:" + eps[cmd - kMenuDeviceBase].id);
    } else if (cmd >= kMenuToggleBase && cmd < kMenuToggleBase + ARRAYSIZE(toggles)) {
        const VizMenuToggle& t = toggles[cmd - kMenuToggleBase];
        VizSetMenuOverride(t.key, *t.field ? L"0" : L"1");
    } else if (cmd >= kMenuChoiceBase) {
        UINT gi = (cmd - kMenuChoiceBase) / 100, oi = (cmd - kMenuChoiceBase) % 100;
        if (gi >= ARRAYSIZE(kGroups) || oi >= kGroups[gi].n) return;
        VizSetMenuOverride(kGroups[gi].key, kGroups[gi].opts[oi].value);
    } else {
        return;
    }
    ApplySettingsChanged();
}

// ---- Style Editor and saved styles (2.1) -------------------------------------------------
// A small window, opened from the right-click menu (My Styles > Style
// Editor...), that builds a look out of the pieces every style is made of:
// the base style, colours, bar size and spacing, corner radius, peak caps,
// Reflection, Glow and Bloom. Every change goes through the same quick
// settings the menu uses and applies at once, so the visualizer itself is the
// live preview. A look can be saved under a name and comes back in one click
// from My Styles.
//
// Saved styles are mod-owned values like the quick settings: "stylePresets"
// lists the names, "stylePreset:<name>" holds one look as key=value lines.

HWND g_styleEditorWnd = nullptr;
static bool g_styleEditorClassRegistered = false;
static const wchar_t kStyleEditorClass[] = L"TourneTableStyleEditor";

// The keys that make up a look, and their current effective values.
static const wchar_t* const kLookKeys[] = {L"shape",      L"colorMode", L"color",       L"grad1",    L"grad2",
                                           L"peakHold",   L"beatFlash", L"barWidth",    L"barGap",   L"barMaxSize",
                                           L"barRadius",  L"reflection", L"fxGlow",     L"fxGlowRadius",
                                           L"fxBloom",    L"fxBloomRadius"};

static std::wstring VizHexColor(BYTE a, BYTE r, BYTE g, BYTE b) {
    wchar_t buf[16];
    swprintf(buf, 16, L"#%02X%02X%02X%02X", a, r, g, b);
    return buf;
}

static // A size as text: '6', '1.25', never '6.000000'.
std::wstring VizNumText(float v) {
    WCHAR b[32];
    swprintf_s(b, L"%g", std::round(v * 100.f) / 100.f);
    return b;
}

std::wstring VizLookValue(const std::wstring& key) {
    const Settings& s = g_settings;
    if (key == L"shape" || key == L"colorMode") return CurrentValue(key);
    if (key == L"color") return VizHexColor(s.colorA, s.colorR, s.colorG, s.colorB);
    if (key == L"grad1") return VizHexColor(s.grad1A, s.grad1R, s.grad1G, s.grad1B);
    if (key == L"grad2") return VizHexColor(s.grad2A, s.grad2R, s.grad2G, s.grad2B);
    if (key == L"peakHold") return s.peakHoldEnabled ? L"1" : L"0";
    if (key == L"beatFlash") return s.beatFlashEnabled ? L"1" : L"0";
    if (key == L"barWidth") return VizNumText(s.barWidth);
    if (key == L"barGap") return VizNumText(s.barGap);
    if (key == L"barMaxSize") return VizNumText(s.barMaxSize);
    if (key == L"barRadius") return VizNumText(s.barRadiusTL);
    if (key == L"reflection") return VizNumText(s.reflection);
    if (key == L"fxGlow") return VizNumText(s.fxGlow);
    if (key == L"fxGlowRadius") return VizNumText(s.fxGlowRadius);
    if (key == L"fxBloom") return VizNumText(s.fxBloom);
    if (key == L"fxBloomRadius") return VizNumText(s.fxBloomRadius);
    return L"";
}

// ---- Saved styles ----
static std::wstring VizReadValue(const std::wstring& name) {
    WCHAR buf[4096] = {};
    if (!Wh_GetStringValue(name.c_str(), buf, ARRAYSIZE(buf))) return L"";
    return buf;
}

std::vector<std::wstring> VizStylePresetNames() {
    std::vector<std::wstring> out;
    std::wstring s = VizReadValue(L"stylePresets");
    size_t pos = 0;
    while (pos < s.size()) {
        size_t nl = s.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = s.size();
        if (nl > pos) out.push_back(s.substr(pos, nl - pos));
        pos = nl + 1;
    }
    return out;
}

static void VizWritePresetNames(const std::vector<std::wstring>& names) {
    std::wstring s;
    for (const auto& n : names) s += n + L"\n";
    Wh_SetStringValue(L"stylePresets", s.c_str());
}

// Letters, digits, spaces, - and _, at most 40: safe as part of a value name.
static std::wstring VizCleanPresetName(const std::wstring& in) {
    std::wstring out;
    for (wchar_t c : in) {
        if (out.size() >= 40) break;
        if (iswalnum(c) || c == L' ' || c == L'-' || c == L'_') out += c;
    }
    while (!out.empty() && out.back() == L' ') out.pop_back();
    while (!out.empty() && out.front() == L' ') out.erase(out.begin());
    return out;
}

static bool VizSaveStylePreset(const std::wstring& rawName) {
    std::wstring name = VizCleanPresetName(rawName);
    if (name.empty()) return false;
    std::wstring body;
    for (const wchar_t* k : kLookKeys) body += std::wstring(k) + L"=" + VizLookValue(k) + L"\n";
    Wh_SetStringValue((L"stylePreset:" + name).c_str(), body.c_str());
    std::vector<std::wstring> names = VizStylePresetNames();
    if (std::find(names.begin(), names.end(), name) == names.end()) {
        names.push_back(name);
        VizWritePresetNames(names);
    }
    return true;
}

static void VizDeleteStylePreset(const std::wstring& name) {
    std::vector<std::wstring> names = VizStylePresetNames();
    names.erase(std::remove(names.begin(), names.end(), name), names.end());
    VizWritePresetNames(names);
    Wh_SetStringValue((L"stylePreset:" + name).c_str(), L"");
}

// Lays a saved look over the current settings, as quick settings.
bool VizApplyStylePreset(const std::wstring& name) {
    std::wstring body = VizReadValue(L"stylePreset:" + name);
    if (body.empty()) return false;
    size_t pos = 0;
    while (pos < body.size()) {
        size_t nl = body.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = body.size();
        std::wstring line = body.substr(pos, nl - pos);
        size_t eq = line.find(L'=');
        if (eq != std::wstring::npos && eq > 0) {
            std::wstring k = line.substr(0, eq);
            for (const wchar_t* lk : kLookKeys)
                if (k == lk) VizSetMenuOverride(k, line.substr(eq + 1));
        }
        pos = nl + 1;
    }
    return true;
}

// ---- Sharing setups ----
// A setup is plain text anyone can read: a first line, its name and the parts it carries, then the
// settings in those parts that differ from the defaults, then quick settings as quick.<key>. It goes
// around as a file (.ttstyle), as a one-line code (TT2: and the text packed, for chats), or through
// the community list (a folder of setups on GitHub, read only when its button is pressed). The audio
// device, hotkeys, file paths, hardware and how it is moved or dragged never go in one.

enum : unsigned { kPartLook = 1, kPartMotion = 2, kPartExtras = 4, kPartPlace = 8, kPartsAll = 15 };
struct VizSetupKey {
    const wchar_t* name;
    const wchar_t* def;
    bool text;
    unsigned part;
};
static const VizSetupKey kSetupKeys[] = {
    {L"appearance.shape", L"stereo", true, 1},
    {L"appearance.reflection", L"0", true, 1},
    {L"appearance.fxGlow", L"0", true, 1},
    {L"appearance.fxGlowRadius", L"6", true, 1},
    {L"appearance.fxBloom", L"0", true, 1},
    {L"appearance.fxBloomRadius", L"16", true, 1},
    {L"appearance.fxOutlineWidth", L"0", true, 1},
    {L"appearance.fxOutlineColor", L"#B0FFFFFF", true, 1},
    {L"appearance.fxShadow", L"0", true, 1},
    {L"appearance.fxShadowColor", L"#000000", true, 1},
    {L"appearance.fxShadowX", L"2", true, 1},
    {L"appearance.fxShadowY", L"3", true, 1},
    {L"appearance.fxShadowSoftness", L"4", true, 1},
    {L"appearance.barHollow", L"0", false, 1},
    {L"appearance.barHollowWidth", L"1.5", true, 1},
    {L"appearance.barDash", L"0", true, 1},
    {L"appearance.barDashGap", L"2", true, 1},
    {L"appearance.barTilt", L"0", true, 1},
    {L"appearance.barMirrorGap", L"0", true, 1},
    {L"appearance.afterimage", L"0", true, 1},
    {L"appearance.afterimageLength", L"1.2", true, 1},
    {L"appearance.orientation", L"horizontal", true, 1},
    {L"appearance.barCount", L"32", false, 1},
    {L"appearance.barWidth", L"6", true, 1},
    {L"appearance.barGap", L"4", true, 1},
    {L"appearance.barMaxSize", L"140", true, 1},
    {L"appearance.barIdleSize", L"4", true, 1},
    {L"appearance.barCornerRadius", L"3", true, 1},
    {L"appearance.colorMode", L"solid", true, 1},
    {L"appearance.color", L"#FFFFFFFF", true, 1},
    {L"appearance.gradientColor1", L"#FF1ED760", true, 1},
    {L"appearance.gradientColor2", L"#FF00B4FF", true, 1},
    {L"appearance.sensitivity", L"150", true, 2},
    {L"appearance.sensitivityCurve", L"knee", true, 2},
    {L"appearance.inputGain", L"0", true, 2},
    {L"appearance.autoGain", L"0", false, 2},
    {L"appearance.autoGainMaxBoost", L"12", true, 2},
    {L"appearance.smoothing", L"0", true, 2},
    {L"appearance.eqPreset", L"default", true, 2},
    {L"appearance.fftSize", L"2048", true, 2},
    {L"appearance.freqScale", L"log", true, 2},
    {L"appearance.peakFreqEnabled", L"0", false, 4},
    {L"appearance.peakFreqAlignH", L"right", true, 4},
    {L"appearance.peakFreqAlignV", L"top", true, 4},
    {L"appearance.oscilloscopeMultibandEnabled", L"0", false, 2},
    {L"appearance.oscilloscopeWindowMs", L"24", true, 2},
    {L"appearance.oscilloscopeDamping", L"0", true, 2},
    {L"appearance.verticalAnchor", L"bottom", true, 1},
    {L"appearance.peakHoldEnabled", L"0", false, 1},
    {L"appearance.peakHoldColor", L"#FF00B4FF", true, 1},
    {L"appearance.beatFlashEnabled", L"0", false, 1},
    {L"appearance.beatFlashColor", L"#FFFFFFFF", true, 1},
    {L"appearance.beatFlashIntensity", L"80", true, 1},
    {L"appearance.rainbowSpeed", L"40", true, 1},
    {L"appearance.nowPlayingEnabled", L"0", false, 4},
    {L"appearance.nowPlayingColor", L"#FFFFFFFF", true, 4},
    {L"appearance.nowPlayingFont", L"Segoe UI", true, 4},
    {L"appearance.nowPlayingFontSize", L"16", true, 4},
    {L"appearance.nowPlayingDisplaySeconds", L"6", true, 4},
    {L"appearance.nowPlayingOffsetX", L"0", true, 4},
    {L"appearance.nowPlayingOffsetY", L"0", true, 4},
    {L"appearance.nowPlayingBgColor", L"#00000000", true, 4},
    {L"appearance.nowPlayingBgPadding", L"6", true, 4},
    {L"appearance.nowPlayingBgCornerRadius", L"6", true, 4},
    {L"appearance.nowPlayingBgBorderSize", L"0", true, 4},
    {L"appearance.nowPlayingBgBorderColor", L"#40FFFFFF", true, 4},
    {L"appearance.peakFreqOffsetX", L"0", true, 4},
    {L"appearance.peakFreqOffsetY", L"0", true, 4},
    {L"appearance.peakFreqBgColor", L"#00000000", true, 4},
    {L"appearance.peakFreqBgPadding", L"6", true, 4},
    {L"appearance.peakFreqBgCornerRadius", L"6", true, 4},
    {L"appearance.peakFreqBgBorderSize", L"0", true, 4},
    {L"appearance.peakFreqBgBorderColor", L"#40FFFFFF", true, 4},
    {L"appearance.nowPlayingLayout", L"one_line", true, 4},
    {L"appearance.nowPlayingPlacement", L"above", true, 4},
    {L"appearance.nowPlayingAlign", L"center", true, 4},
    {L"appearance.nowPlayingArtistColor", L"#B3FFFFFF", true, 4},
    {L"appearance.textRendering", L"smooth", true, 1},
    {L"progress.enabled", L"0", false, 4},
    {L"progress.placement", L"below", true, 4},
    {L"progress.height", L"2", true, 4},
    {L"progress.gap", L"6", true, 4},
    {L"progress.color", L"#FFFFFFFF", true, 4},
    {L"progress.trackColor", L"#40FFFFFF", true, 4},
    {L"terminal.style", L"columns", true, 1},
    {L"terminal.font", L"Consolas", true, 1},
    {L"terminal.fontSize", L"14", true, 1},
    {L"terminal.rows", L"16", false, 1},
    {L"terminal.meterColumns", L"40", false, 1},
    {L"terminal.hotThreshold", L"75", true, 1},
    {L"terminal.columnGlyph", L"#", true, 1},
    {L"terminal.peakGlyph", L"-", true, 1},
    {L"terminal.ramp", L" .:-=+*#%@", true, 1},
    {L"terminal.scrollRate", L"20", true, 1},
    {L"terminal.lowColor", L"#FF33FF66", true, 1},
    {L"terminal.highColor", L"#FFFF3B3B", true, 1},
    {L"terminal.dimColor", L"#FF1E6B34", true, 1},
    {L"terminal.labelColor", L"#FFB8FFB8", true, 1},
    {L"analysis.engine", L"precision", true, 2},
    {L"analysis.bandLayout", L"scale", true, 2},
    {L"analysis.octaveFraction", L"6", true, 2},
    {L"analysis.minFrequency", L"20", true, 2},
    {L"analysis.maxFrequency", L"20000", true, 2},
    {L"analysis.tuningA4", L"440", true, 2},
    {L"analysis.weighting", L"z", true, 2},
    {L"analysis.tilt", L"1.5", true, 2},
    {L"analysis.detector", L"rms", true, 2},
    {L"analysis.levelReference", L"third_octave", true, 2},
    {L"analysis.window", L"hann", true, 2},
    {L"analysis.bassDetail", L"2", true, 2},
    {L"analysis.channel", L"mix", true, 2},
    {L"analysis.dbFloor", L"-72", true, 2},
    {L"analysis.dbCeiling", L"-12", true, 2},
    {L"analysis.ballistics", L"snappy", true, 2},
    {L"analysis.attackMs", L"10", true, 2},
    {L"analysis.releaseDbPerSecond", L"20", true, 2},
    {L"analysis.peakHoldMs", L"500", true, 2},
    {L"analysis.peakFall", L"gravity", true, 2},
    {L"analysis.readout", L"frequency", true, 4},
    {L"analysis.loudnessResetOnTrack", L"1", false, 4},
    {L"position.host", L"desktop", true, 8},
    {L"position.panelBackdrop", L"acrylic", true, 8},
    {L"position.horizontalPosition", L"50", true, 8},
    {L"position.verticalPosition", L"88", true, 8},
    {L"position.pixelSnap", L"1", false, 8},
    {L"position.dockSide", L"below", true, 8},
    {L"position.dockAlign", L"center", true, 8},
    {L"position.dockGap", L"0", true, 8},
    {L"position.dockShift", L"0", true, 8},
    {L"position.split", L"0", false, 8},
    {L"position.splitAt", L"50", true, 8},
    {L"position.splitGap", L"200", true, 8},
    {L"position.splitShift", L"0", true, 8},
    {L"media_controls.enabled", L"0", false, 4},
    {L"media_controls.iconColor", L"#FFFFFFFF", true, 4},
    {L"media_controls.iconSize", L"32", true, 4},
    {L"media_controls.iconSpacing", L"14", true, 4},
    {L"media_controls.plateColor", L"#00000000", true, 4},
    {L"media_controls.platePadding", L"0", true, 4},
    {L"media_controls.plateCornerRadius", L"8", true, 4},
    {L"media_controls.plateBorderSize", L"0", true, 4},
    {L"media_controls.plateBorderColor", L"#40FFFFFF", true, 4},
    {L"media_controls.stacking", L"top", true, 8},
    {L"media_controls.hideWhenCovered", L"0", false, 8},
    {L"media_controls.coveredThresholdPercent", L"50", true, 8},
    {L"media_controls.horizontalPosition", L"50", true, 8},
    {L"media_controls.verticalPosition", L"95", true, 8},
    {L"media_controls.anchor", L"screen", true, 8},
    {L"media_controls.anchorOffsetX", L"8", true, 8},
    {L"media_controls.anchorOffsetY", L"8", true, 8},
    {L"media_controls.layout", L"strip", true, 4},
    {L"media_controls.cardBackground", L"#9E0A0A0D", true, 4},
    {L"media_controls.cardBorderColor", L"#00FFFFFF", true, 4},
    {L"media_controls.cardBorderSize", L"0", true, 4},
    {L"media_controls.cardCornerRadius", L"12", true, 4},
    {L"media_controls.cardArtSize", L"0", true, 4},
    {L"media_controls.cardAccent", L"icon", true, 4},
    {L"media_controls.cardAccentColor", L"#FFFFFFFF", true, 4},
    {L"background.enabled", L"1", false, 1},
    {L"background.color", L"#60000000", true, 1},
    {L"background.padding", L"24", true, 1},
    {L"background.cornerRadius", L"14", true, 1},
    {L"background.blur", L"0", true, 1},
    {L"background.borderSize", L"0", true, 1},
    {L"background.borderColor", L"#40FFFFFF", true, 1},
    {L"performance.targetFps", L"60", false, 2},
};

struct VizQuickPart {
    const wchar_t* key;
    unsigned part;
};
static const VizQuickPart kQuickParts[] = {
    {L"shape", kPartLook},        {L"colorMode", kPartLook},    {L"color", kPartLook},       {L"grad1", kPartLook},
    {L"grad2", kPartLook},        {L"peakHold", kPartLook},     {L"beatFlash", kPartLook},   {L"barWidth", kPartLook},
    {L"barGap", kPartLook},       {L"barMaxSize", kPartLook},   {L"barRadius", kPartLook},   {L"reflection", kPartLook},
    {L"fxGlow", kPartLook},       {L"fxGlowRadius", kPartLook}, {L"fxBloom", kPartLook},     {L"fxBloomRadius", kPartLook},
    {L"termStyle", kPartLook},    {L"pixelText", kPartLook},    {L"engine", kPartMotion},    {L"bandLayout", kPartMotion},
    {L"weighting", kPartMotion},  {L"ballistics", kPartMotion}, {L"targetFps", kPartMotion}, {L"nowPlaying", kPartExtras},
    {L"progress", kPartExtras},   {L"mediaControls", kPartExtras}, {L"readout", kPartExtras}, {L"pixelSnap", kPartPlace},
};

static const wchar_t kCommunityFolder[] = L"https://github.com/USER-TOURNE/Tourne-Style/tree/main/community/setups";
static const wchar_t kCommunityIssue[] = L"https://github.com/USER-TOURNE/Tourne-Style/issues/new";

unsigned VizQuickPartOf(const std::wstring& key) {
    for (const auto& q : kQuickParts)
        if (key == q.key) return q.part;
    return 0;  // (audio source, hardware, nudging: the person's own)
}

const VizSetupKey* VizSetupKeyOf(const std::wstring& name) {
    for (const auto& k : kSetupKeys)
        if (name == k.name) return &k;
    return nullptr;
}

std::wstring VizPartsText(unsigned parts) {
    std::wstring s;
    const wchar_t* names[] = {L"look", L"motion", L"extras", L"place"};
    for (int i = 0; i < 4; i++)
        if (parts & (1u << i)) s += (s.empty() ? L"" : L",") + std::wstring(names[i]);
    return s;
}

unsigned VizPartsFrom(const std::wstring& s) {
    unsigned parts = 0;
    const wchar_t* names[] = {L"look", L"motion", L"extras", L"place"};
    for (int i = 0; i < 4; i++)
        if (s.find(names[i]) != std::wstring::npos) parts |= 1u << i;
    return parts;
}

std::wstring VizPartsWords(unsigned parts) {
    std::wstring s;
    const wchar_t* names[] = {L"Look", L"Motion", L"Extras", L"Place"};
    for (int i = 0; i < 4; i++)
        if (parts & (1u << i)) s += (s.empty() ? L"" : L", ") + std::wstring(names[i]);
    return s;
}

std::string VizShareUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n > 0 ? n : 0, '\0');
    if (n > 0) WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}

std::wstring VizShareWide(const std::string& s) {
    size_t skip = s.size() >= 3 && (unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBB && (unsigned char)s[2] == 0xBF ? 3 : 0;
    if (s.size() <= skip) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data() + skip, (int)(s.size() - skip), nullptr, 0);
    std::wstring w(n > 0 ? n : 0, L'\0');
    if (n > 0) MultiByteToWideChar(CP_UTF8, 0, s.data() + skip, (int)(s.size() - skip), w.data(), n);
    return w;
}

std::wstring VizOneLine(std::wstring v) {
    for (auto& c : v)
        if (c == L'\r' || c == L'\n') c = L' ';
    return v.substr(0, 1024);
}

// What a setting is now (an imported setup's value, else the settings page's), as text.
std::wstring VizSettingNow(const VizSetupKey& k) {
    if (!k.text) return std::to_wstring(VizSettingInt(k.name));
    PCWSTR s = VizSettingString(k.name);
    std::wstring v = s ? s : L"";
    VizSettingFree(s);
    if (v.empty() && *k.def) {  // (a size saved as a whole number before 2.1)
        int n = VizSettingInt(k.name);
        if (n) v = std::to_wstring(n);
    }
    return v;
}

std::wstring VizExportSetup(unsigned parts, const std::wstring& name) {
    std::wstring t = L"Tourne'Table setup 1\n";
    if (!name.empty()) t += L"name=" + VizOneLine(name) + L"\n";
    t += L"parts=" + VizPartsText(parts) + L"\n";
    for (const auto& k : kSetupKeys) {
        if (!(k.part & parts)) continue;
        std::wstring v = VizSettingNow(k);
        bool same = k.text ? v == k.def : _wtoi(v.c_str()) == _wtoi(k.def);
        if (!same) t += std::wstring(k.name) + L"=" + VizOneLine(v) + L"\n";
    }
    for (const auto& kv : g_menuOverrides)
        if (VizQuickPartOf(kv.first) & parts) t += L"quick." + kv.first + L"=" + VizOneLine(kv.second) + L"\n";
    return t;
}

struct VizSetup {
    bool ok = false;
    std::wstring name, author;
    unsigned parts = 0;
    std::vector<std::pair<std::wstring, std::wstring>> values;
};

// Reads a setup out of text: a file, a code's contents, or a GitHub issue with it pasted in (lines
// before its first line, ``` fences and # notes are skipped).
VizSetup VizParseSetup(const std::wstring& text) {
    VizSetup s;
    bool started = false;
    for (size_t pos = 0; pos < text.size();) {
        size_t nl = text.find(L'\n', pos);
        if (nl == std::wstring::npos) nl = text.size();
        std::wstring line = text.substr(pos, nl - pos);
        pos = nl + 1;
        while (!line.empty() && iswspace(line.back())) line.pop_back();
        while (!line.empty() && iswspace(line.front())) line.erase(line.begin());
        if (!started) {
            started = line.rfind(L"Tourne'Table setup", 0) == 0;
            s.ok = started;
            continue;
        }
        if (line.empty() || line[0] == L'#' || line.rfind(L"```", 0) == 0) continue;
        size_t eq = line.find(L'=');
        if (eq == std::wstring::npos || eq == 0) continue;
        std::wstring key = line.substr(0, eq), value = line.substr(eq + 1, 1024);
        if (key == L"name") s.name = value.substr(0, 60);
        else if (key == L"author") s.author = value.substr(0, 40);
        else if (key == L"parts") s.parts = VizPartsFrom(value);
        else if (s.values.size() < 2000) s.values.emplace_back(key, value);
    }
    if (s.ok && !s.parts) s.parts = kPartLook;
    return s;
}

// Lays a setup's parts over the settings: each setting in them takes the setup's value or the default,
// and quick settings in them give way to the setup's own.
void VizApplySetup(const VizSetup& s, unsigned parts) {
    if (!g_setupLayerLoaded) VizSetupLoad();
    for (const auto& k : kSetupKeys)
        if (k.part & parts) g_setupLayer[k.name] = k.def;
    g_menuOverrides.erase(std::remove_if(g_menuOverrides.begin(), g_menuOverrides.end(),
                                         [&](const auto& kv) { return (VizQuickPartOf(kv.first) & parts) != 0; }),
                          g_menuOverrides.end());
    for (const auto& kv : s.values) {
        if (kv.first.rfind(L"quick.", 0) == 0) {
            std::wstring key = kv.first.substr(6);
            if (VizQuickPartOf(key) & parts) g_menuOverrides.emplace_back(key, kv.second);
        } else if (const VizSetupKey* k = VizSetupKeyOf(kv.first)) {
            if (k->part & parts) g_setupLayer[k->name] = kv.second;
        }
    }
    VizSetupSave();
    VizSaveMenuOverrides();
}

std::wstring VizBase64(const std::string& in) {
    static const char* abc = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    std::wstring out;
    for (size_t i = 0; i < in.size(); i += 3) {
        unsigned v = (unsigned char)in[i] << 16;
        if (i + 1 < in.size()) v |= (unsigned char)in[i + 1] << 8;
        if (i + 2 < in.size()) v |= (unsigned char)in[i + 2];
        out += abc[(v >> 18) & 63];
        out += abc[(v >> 12) & 63];
        if (i + 1 < in.size()) out += abc[(v >> 6) & 63];
        if (i + 2 < in.size()) out += abc[v & 63];
    }
    return out;
}

bool VizUnbase64(const std::wstring& in, std::string* out) {
    unsigned v = 0;
    int bits = 0;
    for (wchar_t c : in) {
        int d = c >= L'A' && c <= L'Z' ? c - L'A' : c >= L'a' && c <= L'z' ? c - L'a' + 26 : c >= L'0' && c <= L'9' ? c - L'0' + 52
              : c == L'-' || c == L'+' ? 62 : c == L'_' || c == L'/' ? 63 : -1;
        if (d < 0) {
            if (iswspace(c) || c == L'=') continue;
            return false;
        }
        v = (v << 6) | d;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out->push_back((char)((v >> bits) & 0xFF));
        }
    }
    return true;
}

// Windows' own compression (XPRESS with Huffman): a setup's text packs to about a third.
bool VizPack(const std::string& in, std::string* out) {
    COMPRESSOR_HANDLE c = nullptr;
    if (!CreateCompressor(COMPRESS_ALGORITHM_XPRESS_HUFF, nullptr, &c)) return false;
    SIZE_T need = 0;
    Compress(c, (PVOID)in.data(), in.size(), nullptr, 0, &need);  // (PVOID: MinGW's header isn't const)
    out->resize(need);
    bool ok = need && Compress(c, (PVOID)in.data(), in.size(), out->data(), need, &need);
    if (ok) out->resize(need);
    CloseCompressor(c);
    return ok;
}

bool VizUnpack(const std::string& in, std::string* out) {
    DECOMPRESSOR_HANDLE d = nullptr;
    if (!CreateDecompressor(COMPRESS_ALGORITHM_XPRESS_HUFF, nullptr, &d)) return false;
    SIZE_T need = 0;
    Decompress(d, (PVOID)in.data(), in.size(), nullptr, 0, &need);
    bool ok = need && need <= (1 << 20);
    if (ok) {
        out->resize(need);
        ok = Decompress(d, (PVOID)in.data(), in.size(), out->data(), need, &need);
        if (ok) out->resize(need);
    }
    CloseDecompressor(d);
    return ok;
}

std::wstring VizShareCode(const std::wstring& text) {
    std::string raw = VizShareUtf8(text), packed;
    if (VizPack(raw, &packed)) return L"TT2:" + VizBase64(packed);
    return L"TT2T:" + VizBase64(raw);  // (unpacked: longer, the same setup)
}

// A code back to its text; anything else is taken as the text itself.
std::wstring VizSetupText(std::wstring given) {
    while (!given.empty() && iswspace(given.front())) given.erase(given.begin());
    while (!given.empty() && iswspace(given.back())) given.pop_back();
    std::string bytes, text;
    if (given.rfind(L"TT2:", 0) == 0 && VizUnbase64(given.substr(4), &bytes) && VizUnpack(bytes, &text)) return VizShareWide(text);
    if (given.rfind(L"TT2T:", 0) == 0 && VizUnbase64(given.substr(5), &bytes)) return VizShareWide(bytes);
    return given;
}

void VizClipboardSet(HWND owner, const std::wstring& text) {
    if (!OpenClipboard(owner)) return;
    EmptyClipboard();
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, (text.size() + 1) * sizeof(wchar_t));
    if (mem) {
        memcpy(GlobalLock(mem), text.c_str(), (text.size() + 1) * sizeof(wchar_t));
        GlobalUnlock(mem);
        if (!SetClipboardData(CF_UNICODETEXT, mem)) GlobalFree(mem);
    }
    CloseClipboard();
}

std::wstring VizClipboardGet(HWND owner) {
    std::wstring out;
    if (!OpenClipboard(owner)) return out;
    if (HANDLE h = GetClipboardData(CF_UNICODETEXT)) {
        if (const wchar_t* p = (const wchar_t*)GlobalLock(h)) {
            out.assign(p, wcsnlen(p, 1 << 20));
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
    return out;
}

// Imports a setup (a code, a file's text, a community one) into the parts both it and the ticked boxes
// have, shows it, and keeps it only if asked to; otherwise everything goes back as it was.
bool VizImportSetup(HWND owner, const std::wstring& given, unsigned boxes) {
    VizSetup s = VizParseSetup(VizSetupText(given));
    if (!s.ok) {
        MessageBoxW(owner, L"That isn't a Tourne'Table setup. Copy a TT2: code, or open a .ttstyle file.", L"Tourne'Table",
                    MB_OK | MB_ICONWARNING);
        return false;
    }
    unsigned parts = s.parts & boxes;
    if (!parts) {
        std::wstring why = L"This setup carries " + VizPartsWords(s.parts) + L", and none of those boxes is ticked.";
        MessageBoxW(owner, why.c_str(), L"Tourne'Table", MB_OK | MB_ICONINFORMATION);
        return false;
    }
    if (!g_setupLayerLoaded) VizSetupLoad();
    auto layer = g_setupLayer;
    auto quick = g_menuOverrides;
    VizApplySetup(s, parts);
    ApplySettingsChanged();
    std::wstring ask = L"Keep " + (s.name.empty() ? std::wstring(L"this setup") : L"\"" + s.name + L"\"") +
                       (s.author.empty() ? L"" : L" by " + s.author) + L"?\n\nIt changed " + VizPartsWords(parts) +
                       L". No puts yours back.";
    if (MessageBoxW(owner, ask.c_str(), L"Tourne'Table", MB_YESNO | MB_ICONQUESTION) == IDYES) return true;
    g_setupLayer = layer;
    g_menuOverrides = quick;
    VizSetupSave();
    VizSaveMenuOverrides();
    ApplySettingsChanged();
    return false;
}

bool VizSaveSetupFile(HWND owner, const std::wstring& text, const std::wstring& name) {
    WCHAR path[MAX_PATH] = {};
    wcsncpy_s(path, (name.empty() ? std::wstring(L"My setup") : name).c_str(), _TRUNCATE);
    OPENFILENAMEW ofn = {sizeof(ofn)};
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"Tourne'Table setups (*.ttstyle)\0*.ttstyle\0All files\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"ttstyle";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetSaveFileNameW(&ofn)) return false;
    std::string bytes = VizShareUtf8(text);
    HANDLE f = CreateFileW(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    DWORD put = 0;
    bool ok = WriteFile(f, bytes.data(), (DWORD)bytes.size(), &put, nullptr) && put == bytes.size();
    CloseHandle(f);
    return ok;
}

bool VizOpenSetupFile(HWND owner, std::wstring* text) {
    WCHAR path[MAX_PATH] = {};
    OPENFILENAMEW ofn = {sizeof(ofn)};
    ofn.hwndOwner = owner;
    ofn.lpstrFilter = L"Tourne'Table setups (*.ttstyle)\0*.ttstyle\0Text\0*.txt\0All files\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (!GetOpenFileNameW(&ofn)) return false;
    HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    std::string bytes(256 * 1024, '\0');
    DWORD got = 0;
    bool ok = ReadFile(f, bytes.data(), (DWORD)bytes.size(), &got, nullptr);
    CloseHandle(f);
    bytes.resize(got);
    *text = VizShareWide(bytes);
    return ok;
}

// ---- The community list: setups in a folder of the Tourne-Style repository on GitHub ----
// The mod never downloads anything itself: Community opens the folder in the browser, and a setup
// found there comes in with Paste code or Open file.

std::wstring VizUrlEncode(const std::wstring& w) {
    std::string s = VizShareUtf8(w);
    std::wstring out;
    for (unsigned char c : s) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out += (wchar_t)c;
        } else {
            wchar_t b[4];
            swprintf(b, 4, L"%%%02X", c);
            out += b;
        }
    }
    return out;
}

// Opens a new GitHub issue with the setup in it, for the person to send; Tourne adds the good ones
// to the community folder.
void VizShareToCommunity(HWND owner, unsigned parts, const std::wstring& name) {
    std::wstring text = VizExportSetup(parts, name);
    std::wstring head = std::wstring(kCommunityIssue) + L"?title=" + VizUrlEncode(L"Setup: " + name) + L"&body=";
    std::wstring url = head + VizUrlEncode(L"Made in Tourne'Table. A screenshot helps people pick it.\n\n```\n" + text + L"```\n");
    if (url.size() > 7000) {  // (too long for a link: it goes on the clipboard instead)
        VizClipboardSet(owner, text);
        url = head + VizUrlEncode(L"Made in Tourne'Table. Paste the setup here (it's on your clipboard):\n\n```\n\n```\n");
    }
    ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

// ---- The editor window ----
namespace {
enum : int {
    kIdPreset = 100, kIdLoad, kIdSave, kIdDelete, kIdShape, kIdColorMode, kIdColor, kIdGrad1, kIdGrad2,
    kIdPeak, kIdBeat, kIdSliderBase = 200,  // + slider index; value labels at + 100
};
constexpr UINT_PTR kApplyTimer = 1;
enum : int {
    kIdShareCopy = 400, kIdSharePaste, kIdShareCommunity, kIdShareSave, kIdShareOpen,
    kIdPartLook, kIdPartMotion, kIdPartExtras, kIdPartPlace,
};
constexpr UINT_PTR kShareTimer = 2;

// The ticked parts (Look, Motion, Extras, Place).
unsigned EditorParts(HWND hWnd) {
    unsigned parts = 0;
    for (int i = 0; i < 4; i++)
        if (IsDlgButtonChecked(hWnd, kIdPartLook + i) == BST_CHECKED) parts |= 1u << i;
    return parts;
}

// What a shared setup is called: the Saved style box's name, or "My setup".
std::wstring EditorSetupName(HWND hWnd) {
    WCHAR name[64] = {};
    GetDlgItemTextW(hWnd, kIdPreset, name, ARRAYSIZE(name));
    std::wstring n = VizCleanPresetName(name);
    return n.empty() ? L"My setup" : n;
}

struct EditorSlider {
    const wchar_t* key;
    const wchar_t* label;
    int lo, hi;
    int scale;  // slider steps per unit: 100 moves in hundredths
};
// Every slider steps by 0.01 (arrow keys), 0.1 (Page Up / Down) or freely by dragging.
const EditorSlider kSliders[] = {
    {L"barWidth", L"Bar width", 1, 40, 100},       {L"barGap", L"Bar gap", 0, 30, 100},
    {L"barMaxSize", L"Height", 10, 400, 100},      {L"barRadius", L"Corner radius", 0, 20, 100},
    {L"reflection", L"Reflection", 0, 100, 100},   {L"fxGlow", L"Glow", 0, 100, 100},
    {L"fxGlowRadius", L"Glow radius", 1, 32, 100}, {L"fxBloom", L"Bloom", 0, 100, 100},
    {L"fxBloomRadius", L"Bloom radius", 4, 64, 100},
};
HFONT s_editorFont = nullptr;
COLORREF s_customColors[16] = {};

// Settings changed from the editor: stored at once, applied at most every
// 80 ms while a slider is dragged (a full settings apply per mouse move
// would be wasted work), and at once on release.
void EditorApplySoon(HWND hWnd) { SetTimer(hWnd, kApplyTimer, 80, nullptr); }
void EditorApplyNow(HWND hWnd) {
    KillTimer(hWnd, kApplyTimer);
    ApplySettingsChanged();
}

void EditorFillPresets(HWND hWnd, const std::wstring& select) {
    HWND cb = GetDlgItem(hWnd, kIdPreset);
    SendMessageW(cb, CB_RESETCONTENT, 0, 0);
    for (const auto& n : VizStylePresetNames()) SendMessageW(cb, CB_ADDSTRING, 0, (LPARAM)n.c_str());
    SetWindowTextW(cb, select.c_str());
}

// Puts the current effective values into every control.
void EditorSync(HWND hWnd) {
    auto selectValue = [&](int id, const VizMenuOption* opts, size_t n, const std::wstring& v) {
        for (size_t i = 0; i < n; i++)
            if (v == opts[i].value) SendDlgItemMessageW(hWnd, id, CB_SETCURSEL, i, 0);
    };
    selectValue(kIdShape, kShapes, ARRAYSIZE(kShapes), VizLookValue(L"shape"));
    selectValue(kIdColorMode, kColorModes, ARRAYSIZE(kColorModes), VizLookValue(L"colorMode"));
    CheckDlgButton(hWnd, kIdPeak, g_settings.peakHoldEnabled ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hWnd, kIdBeat, g_settings.beatFlashEnabled ? BST_CHECKED : BST_UNCHECKED);
    for (int i = 0; i < (int)ARRAYSIZE(kSliders); i++) {
        std::wstring v = VizLookValue(kSliders[i].key);
        SendDlgItemMessageW(hWnd, kIdSliderBase + i, TBM_SETPOS, TRUE, lroundf((float)_wtof(v.c_str()) * kSliders[i].scale));
        SetDlgItemTextW(hWnd, kIdSliderBase + 100 + i, v.c_str());
    }
}

void EditorPickColor(HWND hWnd, const wchar_t* key) {
    BYTE a = 255, r = 255, g = 255, b = 255;
    ParseColorHex(VizLookValue(key).c_str(), &a, &r, &g, &b);
    CHOOSECOLORW cc = {sizeof(cc)};
    cc.hwndOwner = hWnd;
    cc.rgbResult = RGB(r, g, b);
    cc.lpCustColors = s_customColors;
    cc.Flags = CC_RGBINIT | CC_FULLOPEN;
    if (!ChooseColorW(&cc)) return;
    VizSetMenuOverride(key, VizHexColor(a, GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult)));
    EditorApplyNow(hWnd);
}

void EditorBuild(HWND hWnd) {
    const UINT dpi = GetDpiForWindow(hWnd);
    auto px = [&](int v) { return MulDiv(v, dpi ? dpi : 96, 96); };
    NONCLIENTMETRICSW ncm = {sizeof(ncm)};
    if (!s_editorFont && SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0)) {
        ncm.lfMessageFont.lfHeight = -px(12);
        s_editorFont = CreateFontIndirectW(&ncm.lfMessageFont);
    }
    HINSTANCE inst = GetCurrentModuleHandle();
    int y = px(12);
    const int L = px(12), labelW = px(96), ctlX = L + labelW, ctlW = px(260), rowH = px(30);
    auto add = [&](const wchar_t* cls, const wchar_t* text, DWORD style, int x, int yy, int w, int h, int id) {
        HWND c = CreateWindowExW(0, cls, text, WS_CHILD | WS_VISIBLE | style, x, yy, w, h, hWnd,
                                 (HMENU)(INT_PTR)id, inst, nullptr);
        if (c && s_editorFont) SendMessageW(c, WM_SETFONT, (WPARAM)s_editorFont, FALSE);
        return c;
    };
    auto label = [&](const wchar_t* text) { add(L"STATIC", text, SS_LEFT, L, y + px(5), labelW, px(20), -1); };

    label(L"Saved style");
    add(L"COMBOBOX", L"", CBS_DROPDOWN | CBS_AUTOHSCROLL | WS_VSCROLL | WS_TABSTOP, ctlX, y, px(118), px(200), kIdPreset);
    add(L"BUTTON", L"Load", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(122), y, px(44), px(24), kIdLoad);
    add(L"BUTTON", L"Save", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(170), y, px(44), px(24), kIdSave);
    add(L"BUTTON", L"Delete", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(218), y, px(50), px(24), kIdDelete);
    y += rowH + px(6);
    label(L"Share");
    add(L"BUTTON", L"Copy code", BS_PUSHBUTTON | WS_TABSTOP, ctlX, y, px(84), px(24), kIdShareCopy);
    add(L"BUTTON", L"Paste code", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(88), y, px(84), px(24), kIdSharePaste);
    add(L"BUTTON", L"Community", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(176), y, px(84), px(24), kIdShareCommunity);
    y += rowH;
    add(L"BUTTON", L"Save file...", BS_PUSHBUTTON | WS_TABSTOP, ctlX, y, px(84), px(24), kIdShareSave);
    add(L"BUTTON", L"Open file...", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(88), y, px(84), px(24), kIdShareOpen);
    y += rowH;
    label(L"Parts");
    add(L"BUTTON", L"Look", BS_AUTOCHECKBOX | WS_TABSTOP, ctlX, y, px(58), px(22), kIdPartLook);
    add(L"BUTTON", L"Motion", BS_AUTOCHECKBOX | WS_TABSTOP, ctlX + px(62), y, px(66), px(22), kIdPartMotion);
    add(L"BUTTON", L"Extras", BS_AUTOCHECKBOX | WS_TABSTOP, ctlX + px(132), y, px(62), px(22), kIdPartExtras);
    add(L"BUTTON", L"Place", BS_AUTOCHECKBOX | WS_TABSTOP, ctlX + px(198), y, px(62), px(22), kIdPartPlace);
    y += rowH + px(6);
    label(L"Base style");
    HWND shape = add(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, ctlX, y, ctlW, px(320), kIdShape);
    for (const auto& o : kShapes) SendMessageW(shape, CB_ADDSTRING, 0, (LPARAM)o.label);
    y += rowH;
    label(L"Color mode");
    HWND cm = add(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, ctlX, y, ctlW, px(320), kIdColorMode);
    for (const auto& o : kColorModes) SendMessageW(cm, CB_ADDSTRING, 0, (LPARAM)o.label);
    y += rowH;
    label(L"Colors");
    add(L"BUTTON", L"Color", BS_PUSHBUTTON | WS_TABSTOP, ctlX, y, px(80), px(24), kIdColor);
    add(L"BUTTON", L"Gradient start", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(86), y, px(90), px(24), kIdGrad1);
    add(L"BUTTON", L"Gradient end", BS_PUSHBUTTON | WS_TABSTOP, ctlX + px(182), y, px(78), px(24), kIdGrad2);
    y += rowH;
    add(L"BUTTON", L"Peak caps", BS_AUTOCHECKBOX | WS_TABSTOP, ctlX, y, px(110), px(22), kIdPeak);
    add(L"BUTTON", L"Beat flash", BS_AUTOCHECKBOX | WS_TABSTOP, ctlX + px(120), y, px(110), px(22), kIdBeat);
    y += rowH;
    for (int i = 0; i < (int)ARRAYSIZE(kSliders); i++) {
        label(kSliders[i].label);
        HWND tb = add(TRACKBAR_CLASSW, L"", TBS_HORZ | TBS_NOTICKS | WS_TABSTOP, ctlX - px(4), y, ctlW - px(44), px(26),
                      kIdSliderBase + i);
        // MIN / MAX rather than SETRANGE: positions above 32767 don't fit its packed words.
        SendMessageW(tb, TBM_SETRANGEMIN, FALSE, kSliders[i].lo * kSliders[i].scale);
        SendMessageW(tb, TBM_SETRANGEMAX, FALSE, kSliders[i].hi * kSliders[i].scale);
        SendMessageW(tb, TBM_SETLINESIZE, 0, 1);
        SendMessageW(tb, TBM_SETPAGESIZE, 0, kSliders[i].scale / 10);
        add(L"STATIC", L"", SS_RIGHT, ctlX + ctlW - px(44), y + px(5), px(44), px(20), kIdSliderBase + 100 + i);
        y += px(28);
    }
    y += px(4);
    add(L"STATIC",
        L"Changes apply at once. Reset Quick Settings in the right-click menu returns to the settings page. "
        L"Reflection, Glow and Bloom need the Direct3D 11 renderer. Share carries the ticked parts; your audio "
        L"device, hotkeys and file paths stay out. Community opens GitHub in your browser.",
        SS_LEFT, L, y, labelW + ctlW, px(80), -1);
    y += px(88);
    RECT rc = {0, 0, labelW + ctlW + L * 2, y};
    AdjustWindowRectExForDpi(&rc, GetWindowLongW(hWnd, GWL_STYLE), FALSE, GetWindowLongW(hWnd, GWL_EXSTYLE), dpi);
    SetWindowPos(hWnd, nullptr, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
    WCHAR parts[64] = {};
    unsigned ticked = Wh_GetStringValue(L"shareParts", parts, ARRAYSIZE(parts)) ? VizPartsFrom(parts)
                                                                               : (kPartLook | kPartMotion | kPartExtras);
    for (int i = 0; i < 4; i++) CheckDlgButton(hWnd, kIdPartLook + i, ticked & (1u << i) ? BST_CHECKED : BST_UNCHECKED);
    EditorFillPresets(hWnd, L"");
    EditorSync(hWnd);
}

LRESULT CALLBACK StyleEditorProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            EditorBuild(hWnd);
            return 0;
        case WM_HSCROLL: {
            HWND tb = (HWND)lParam;
            int i = GetDlgCtrlID(tb) - kIdSliderBase;
            if (i < 0 || i >= (int)ARRAYSIZE(kSliders)) break;
            int v = (int)SendMessageW(tb, TBM_GETPOS, 0, 0);
            std::wstring text = VizNumText((float)v / kSliders[i].scale);
            SetDlgItemTextW(hWnd, kIdSliderBase + 100 + i, text.c_str());
            VizSetMenuOverride(kSliders[i].key, text);
            if (LOWORD(wParam) == TB_THUMBTRACK) EditorApplySoon(hWnd);
            else EditorApplyNow(hWnd);
            return 0;
        }
        case WM_TIMER:
            if (wParam == kApplyTimer) EditorApplyNow(hWnd);
            if (wParam == kShareTimer) {
                KillTimer(hWnd, kShareTimer);
                SetDlgItemTextW(hWnd, kIdShareCopy, L"Copy code");
            }
            return 0;
        case WM_COMMAND: {
            int id = LOWORD(wParam), code = HIWORD(wParam);
            if ((id == kIdShape || id == kIdColorMode) && code == CBN_SELCHANGE) {
                int sel = (int)SendDlgItemMessageW(hWnd, id, CB_GETCURSEL, 0, 0);
                const VizMenuOption* opts = id == kIdShape ? kShapes : kColorModes;
                size_t n = id == kIdShape ? ARRAYSIZE(kShapes) : ARRAYSIZE(kColorModes);
                if (sel >= 0 && (size_t)sel < n) {
                    VizSetMenuOverride(id == kIdShape ? L"shape" : L"colorMode", opts[sel].value);
                    EditorApplyNow(hWnd);
                }
            } else if ((id == kIdPeak || id == kIdBeat) && code == BN_CLICKED) {
                VizSetMenuOverride(id == kIdPeak ? L"peakHold" : L"beatFlash",
                                   IsDlgButtonChecked(hWnd, id) == BST_CHECKED ? L"1" : L"0");
                EditorApplyNow(hWnd);
            } else if (id == kIdColor || id == kIdGrad1 || id == kIdGrad2) {
                EditorPickColor(hWnd, id == kIdColor ? L"color" : id == kIdGrad1 ? L"grad1" : L"grad2");
            } else if (id >= kIdPartLook && id <= kIdPartPlace && code == BN_CLICKED) {
                Wh_SetStringValue(L"shareParts", VizPartsText(EditorParts(hWnd)).c_str());
            } else if (id == kIdShareCopy || id == kIdShareSave) {
                unsigned parts = EditorParts(hWnd);
                if (!parts) {
                    MessageBeep(MB_ICONWARNING);
                    return 0;
                }
                std::wstring text = VizExportSetup(parts, EditorSetupName(hWnd));
                if (id == kIdShareSave) {
                    VizSaveSetupFile(hWnd, text, EditorSetupName(hWnd));
                } else {
                    VizClipboardSet(hWnd, VizShareCode(text));
                    SetDlgItemTextW(hWnd, kIdShareCopy, L"Copied");
                    SetTimer(hWnd, kShareTimer, 1500, nullptr);
                }
            } else if (id == kIdSharePaste || id == kIdShareOpen) {
                std::wstring text;
                if (id == kIdSharePaste) text = VizClipboardGet(hWnd);
                else if (!VizOpenSetupFile(hWnd, &text)) return 0;
                VizImportSetup(hWnd, text, EditorParts(hWnd));
                EditorSync(hWnd);
            } else if (id == kIdShareCommunity) {
                HMENU m = CreatePopupMenu();
                AppendMenuW(m, MF_STRING, 1001, L"Browse Community Setups (opens GitHub)");
                AppendMenuW(m, MF_STRING, 1000, L"Share Mine to the Community...");
                RECT rc = {};
                GetWindowRect(GetDlgItem(hWnd, kIdShareCommunity), &rc);
                UINT cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, rc.left, rc.bottom, 0, hWnd, nullptr);
                DestroyMenu(m);
                if (cmd == 1000) {
                    unsigned parts = EditorParts(hWnd);
                    if (parts) VizShareToCommunity(hWnd, parts, EditorSetupName(hWnd));
                    else MessageBeep(MB_ICONWARNING);
                } else if (cmd == 1001) {
                    ShellExecuteW(nullptr, L"open", kCommunityFolder, nullptr, nullptr, SW_SHOWNORMAL);
                }
            } else if (id == kIdLoad || id == kIdSave || id == kIdDelete) {
                WCHAR name[64] = {};
                GetDlgItemTextW(hWnd, kIdPreset, name, ARRAYSIZE(name));
                std::wstring n = VizCleanPresetName(name);
                if (n.empty()) {
                    MessageBeep(MB_ICONWARNING);
                    return 0;
                }
                if (id == kIdSave) {
                    VizSaveStylePreset(n);
                    EditorFillPresets(hWnd, n);
                } else if (id == kIdDelete) {
                    VizDeleteStylePreset(n);
                    EditorFillPresets(hWnd, L"");
                } else if (VizApplyStylePreset(n)) {
                    EditorApplyNow(hWnd);
                    EditorSync(hWnd);
                }
            }
            return 0;
        }
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hWnd, kApplyTimer);
            KillTimer(hWnd, kShareTimer);
            g_styleEditorWnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}
}  // namespace

// Opens the editor near the visualizer, or brings it forward if it is open.
// Runs on the message window's thread, whose loop then serves it.
void VizOpenStyleEditor() {
    if (g_styleEditorWnd) {
        SetForegroundWindow(g_styleEditorWnd);
        return;
    }
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_BAR_CLASSES};
    InitCommonControlsEx(&icc);
    if (!g_styleEditorClassRegistered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.lpfnWndProc = StyleEditorProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wc.lpszClassName = kStyleEditorClass;
        if (!RegisterClassExW(&wc)) return;
        g_styleEditorClassRegistered = true;
    }
    POINT at = {CW_USEDEFAULT, CW_USEDEFAULT};
    if (g_drawRectValid.load(std::memory_order_relaxed)) {
        at.x = g_drawRectL.load(std::memory_order_relaxed);
        at.y = std::max<LONG>(0, g_drawRectT.load(std::memory_order_relaxed) - 600);
    }
    g_styleEditorWnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kStyleEditorClass, L"Tourne'Table Style Editor",
                                       WS_CAPTION | WS_SYSMENU | WS_POPUP, at.x, at.y, 400, 600, nullptr, nullptr,
                                       GetCurrentModuleHandle(), nullptr);
    if (!g_styleEditorWnd) return;
    ShowWindow(g_styleEditorWnd, SW_SHOW);
    SetForegroundWindow(g_styleEditorWnd);
}

void UnregisterStyleEditorClass() {
    if (s_editorFont) {
        DeleteObject(s_editorFont);
        s_editorFont = nullptr;
    }
    if (g_styleEditorClassRegistered) {
        UnregisterClassW(kStyleEditorClass, GetCurrentModuleHandle());
        g_styleEditorClassRegistered = false;
    }
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

        // Key repeat can fire dozens of nudges a second, so restart a short
        // timer instead and write once the user stops moving. SetTimer with an
        // existing id re-arms it.
        case WM_APP_REQUEST_SAVE_POSITION:
            if (!g_unloading) {
                SetTimer(hWnd, TIMER_ID_MSG_SAVE_POSITION, 700, nullptr);
            }
            return 0;

        // A worker thread has something to tell the user. It
        // arrives here so it can go through the same settings-problems summary
        // as everything else, which is only safe on this thread.
        case WM_APP_HW_NOTICE: {
            auto* msg = reinterpret_cast<std::wstring*>(lParam);
            if (msg) {
                if (!g_unloading) {
                    if (wParam == 1) ReportSettingWarning(L"Audio", L"Audio Source", *msg);
                    else ReportSettingWarning(L"Hardware", L"Audio Analysis Device", *msg);
                    FlushSettingsIssues();
                }
                delete msg;
            }
            return 0;
        }

        // Device lost (wParam 1), posted from the render path. Rebuilds are
        // spaced at least two seconds apart so a GPU that keeps failing can't
        // spin this thread.
        case WM_APP_REBUILD_DEVICE:
            g_deviceRebuildQueued.store(false);
            if (g_unloading || !g_lazyInitialized || !g_initSucceeded) return 0;
            if (GetTickCount64() - g_lastDeviceRebuildTick < 2000) {
                SetTimer(hWnd, TIMER_ID_MSG_REBUILD_DEVICE, 2000, nullptr);
            } else {
                VizRebuildRenderDevice(wParam ? L"device lost" : L"requested");
            }
            return 0;

        case WM_TIMER:
            if (g_unloading) return 0;
            if (wParam == TIMER_ID_MSG_DISPLAY_CHANGE) {
                KillTimer(hWnd, TIMER_ID_MSG_DISPLAY_CHANGE);
                HandleDisplayChange();
            } else if (wParam == TIMER_ID_MSG_RECREATE_OVERLAY) {
                KillTimer(hWnd, TIMER_ID_MSG_RECREATE_OVERLAY);
                CreateOverlayWindow();
                // Running as our own process now (see the tool-mod boilerplate at
                // the end of this file), there's no explorer-side hook to tell us
                // the moment WorkerW becomes available -- keep polling for it
                // instead of giving up after a single attempt.
                if (!g_overlayWnd && !g_unloading) {
                    SetTimer(hWnd, TIMER_ID_MSG_RECREATE_OVERLAY, 1000, nullptr);
                }
            } else if (wParam == TIMER_ID_MSG_WALLPAPER_REFRESH) {
                KillTimer(hWnd, TIMER_ID_MSG_WALLPAPER_REFRESH);
                if (g_overlayWnd && g_settings.backgroundEnabled && g_settings.bgBlur > 0) {
                    ReleaseVisualResources();
                    RecreateVisualResources();
                    if (!g_fullscreenPaused.load()) RenderVisualizer();
                }
            } else if (wParam == TIMER_ID_MSG_REBUILD_DEVICE) {
                KillTimer(hWnd, TIMER_ID_MSG_REBUILD_DEVICE);
                if (g_lazyInitialized && g_initSucceeded) VizRebuildRenderDevice(L"device lost (retry)");
            } else if (wParam == TIMER_ID_MSG_SAVE_POSITION) {
                KillTimer(hWnd, TIMER_ID_MSG_SAVE_POSITION);
                PersistOverrideState();
            } else if (wParam == TIMER_ID_MSG_FONT_RECHECK) {
                // Re-run the font lookup that failed during settings load. A
                // cold boot resolves this within a few seconds; a name that is
                // actually wrong never will, and gets reported once the
                // attempts run out.
                if (!g_fontCheckPending || !g_settings.nowPlayingEnabled) {
                    g_fontCheckPending = false;
                    KillTimer(hWnd, TIMER_ID_MSG_FONT_RECHECK);
                } else if (IsFontInstalled(g_settings.nowPlayingFont)) {
                    Wh_Log(L"Now Playing Font resolved after %d s, no issue to report",
                           g_fontCheckAttempts + 1);
                    g_fontCheckPending = false;
                    KillTimer(hWnd, TIMER_ID_MSG_FONT_RECHECK);
                } else if (++g_fontCheckAttempts >= FONT_CHECK_MAX_ATTEMPTS) {
                    g_fontCheckPending = false;
                    KillTimer(hWnd, TIMER_ID_MSG_FONT_RECHECK);
                    ReportSettingIssue(
                        L"Appearance", L"Now Playing Font", g_settings.nowPlayingFont.c_str(),
                        L"the name of a font installed on this PC, exactly as Windows spells it",
                        L"whatever Windows substitutes (usually Segoe UI)");
                    // ReportSettingIssue only queues the line. LoadSettings
                    // normally flushes at its end, but this report happens long
                    // after that has returned, so without flushing here the
                    // warning would sit in the vector until the next settings
                    // change cleared it unseen. Safe from this thread: the flush
                    // honours showSettingsErrors and puts the dialog on a thread
                    // of its own.
                    FlushSettingsIssues();
                }
            } else if (wParam == TIMER_ID_MSG_FULLSCREEN_WATCH) {
                VizSeekAppsRefresh();
                // The media strip is a plain layered window living alongside
                // whatever else is topmost, and it's the one piece of this mod
                // that has to survive in that crowd -- another app taking
                // topmost, or the window going away with the shell, would
                // otherwise leave it gone with nothing to bring it back. This
                // second-granularity check is cheap and makes it self-healing.
                if (g_settings.mediaControlsEnabled) {
                    if (!g_mediaWnd) {
                        Wh_Log(L"[Media] window missing, recreating");
                        CreateMediaControlWindow();
                    } else {
                        if (g_settings.mediaHideWhenCovered) {
                            // Measured against the strip's own rect on the same
                            // timer as the visualizer's occlusion check, for the
                            // same reason: EnumWindows is far too heavy to run
                            // any more often than this.
                            bool covered = IsMediaStripCovered();
                            if (covered && !g_mediaHiddenByCover) {
                                Wh_Log(L"[Media] covered, hiding strip");
                                g_mediaHiddenByCover = true;
                                ShowWindow(g_mediaWnd, SW_HIDE);
                            } else if (!covered && g_mediaHiddenByCover) {
                                Wh_Log(L"[Media] uncovered, showing strip");
                                g_mediaHiddenByCover = false;
                                RepositionAndRepaintMediaControls();
                            }
                        } else if (g_mediaHiddenByCover) {
                            // The setting was turned off while the strip was
                            // parked out of the way.
                            g_mediaHiddenByCover = false;
                            RepositionAndRepaintMediaControls();
                        }

                        // Still worth re-asserting even with Hide When Covered
                        // on -- that only accounts for the times we hid it
                        // ourselves, not for another app taking topmost.
                        if (!g_mediaHiddenByCover && !g_menuOpen.load(std::memory_order_acquire) &&
                            !(g_overlayInPanel && g_panelHidden) &&
                            (!IsWindowVisible(g_mediaWnd) ||
                             (g_settings.mediaOnTop &&
                              !(GetWindowLongPtr(g_mediaWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) ||
                             [] {
                                 RECT r;
                                 return GetWindowRect(g_mediaWnd, &r) && VizTaskbarOverlaps(r, true);
                             }())) {
                            Wh_Log(L"[Media] window not visible/topmost, re-asserting");
                            RepositionAndRepaintMediaControls();
                        }
                    }
                }

                VizDockRefresh();

                bool shouldPause = g_userPaused.load(std::memory_order_relaxed);

                if (g_settings.pauseOnFullscreen && IsFullscreenOrGameActive())
                    shouldPause = true;

                // Occlusion is evaluated on this same 1s timer rather than per
                // frame -- EnumWindows is far too heavy to run at frame rate,
                // and a second of latency on hide/show is imperceptible.
                if (g_overlayInPanel && g_panelHidden) shouldPause = true;  // nothing to draw into

                if (!shouldPause && g_settings.pauseWhenObscured && !g_settings.hostTaskbar && !g_overlayInPanel &&
                    IsVisualizerOccluded())
                    shouldPause = true;

                if (shouldPause) {
                    PauseForFullscreen();
                } else {
                    ResumeFromFullscreen();
                }
            }
            return 0;

        case WM_APP_SETTINGS_CHANGED:
            ApplySettingsChanged();
            return 0;

        case WM_HOTKEY:
            if (wParam == kVizPanelHotkeyId && !g_unloading) VizPanelShow(g_panelHidden);
            return 0;

        case WM_APP_CONTEXT_MENU:
            if (!g_unloading) VizShowContextMenu(POINT{(LONG)(int)wParam, (LONG)(int)lParam});
            return 0;

        case WM_APP_SEEK_AT:
            if (!g_unloading) VizSeekAtX((LONG)(int)wParam);
            return 0;


        case WM_DESTROY:
            VizDockStop();
            s_cardVol.Reset();  // a COM object of this thread's apartment: released before CoUninitialize
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

    HWND hWorkerW = nullptr, hTaskbar = nullptr;
    if (g_settings.hostPanel) {
        // (a window of its own: nothing to find)
    } else if (g_settings.hostTaskbar) {
        HMONITOR m = GetMonitorById(g_settings.monitor - 1);
        if (!m) m = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
        hTaskbar = VizTaskbarFor(m);
        if (!hTaskbar) hTaskbar = FindWindow(L"Shell_TrayWnd", nullptr);
        if (!hTaskbar) {
            Wh_Log(L"Failed to find the taskbar");
            return;
        }
    } else {
        hWorkerW = GetWorkerW();
        if (!hWorkerW) {
            Wh_Log(L"Failed to find WorkerW");
            return;
        }
    }

    if (!RegisterOverlayWindowClass()) return;

    HINSTANCE hInstance = GetCurrentModuleHandle();

    if (g_settings.hostPanel) {
        VizCreatePanelWindow(hInstance);
        if (!g_overlayWnd) return;
    } else if (hTaskbar) {
        // Owned by the taskbar, so it always stays above it; layered and
        // transparent, so every click goes through to the taskbar under it.
        g_overlayWnd = CreateWindowEx(
            WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW |
                WS_EX_TOPMOST,
            OVERLAY_WINDOW_CLASS, nullptr, WS_POPUP, GetSystemMetrics(SM_XVIRTUALSCREEN),
            GetSystemMetrics(SM_YVIRTUALSCREEN), GetSystemMetrics(SM_CXVIRTUALSCREEN),
            GetSystemMetrics(SM_CYVIRTUALSCREEN), hTaskbar, nullptr, hInstance, nullptr);
        if (!g_overlayWnd) return;
        SetLayeredWindowAttributes(g_overlayWnd, 0, 255, LWA_ALPHA);
        ShowWindow(g_overlayWnd, SW_SHOWNOACTIVATE);
        g_overlayInTaskbar = true;
        g_overlayInPanel = false;
    } else {
        RECT rc;
        GetWindowRect(hWorkerW, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        g_overlayWnd = CreateWindowEx(
            WS_EX_NOREDIRECTIONBITMAP | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, OVERLAY_WINDOW_CLASS,
            nullptr, WS_CHILD | WS_VISIBLE, 0, 0, width, height, hWorkerW, nullptr, hInstance,
            nullptr);
        if (!g_overlayWnd) return;
        g_overlayInTaskbar = false;
        g_overlayInPanel = false;
    }
    Wh_Log(L"[Host] overlay in %s", g_overlayInPanel ? L"panel" : g_overlayInTaskbar ? L"taskbar" : L"desktop");

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
        // Settings are loaded before this window exists on the init path, so a
        // font check that already failed has nowhere to arm its retry until now.
        if (g_fontCheckPending) {
            SetTimer(g_messageWnd, TIMER_ID_MSG_FONT_RECHECK, 1000, nullptr);
        }
    }
}

void LoadSettings() {
    g_settingsIssues.clear();
    VizLoadMenuOverrides();
    g_settings.showSettingsErrors = VizSettingInt(L"validation.showErrors") != 0;

    PCWSTR shape = VizSettingString(L"appearance.shape");
    VizParseShape(shape, &g_settings.shape, &g_settings.style);
    VizSettingFree(shape);
    g_settings.reflection = ReadSizeSetting(L"appearance.reflection", 0.f, 100.f, 0.f);
    g_settings.fxGlow = ReadSizeSetting(L"appearance.fxGlow", 0.f, 100.f, 0.f);
    g_settings.fxGlowRadius = ReadSizeSetting(L"appearance.fxGlowRadius", 0.5f, 32.f, 6.f);
    g_settings.fxBloom = ReadSizeSetting(L"appearance.fxBloom", 0.f, 100.f, 0.f);
    g_settings.fxBloomRadius = ReadSizeSetting(L"appearance.fxBloomRadius", 4.f, 64.f, 16.f);
    g_settings.fxOutlineWidth = ReadNumberSetting(L"appearance.fxOutlineWidth", L"Appearance", L"Outline Width", 0.f, 0.f, 16.f);
    {
        BYTE dummy = 0;
        ReadColorSetting(L"appearance.fxOutlineColor", L"Appearance", L"Outline Color", 0xB0, 255, 255, 255,
                         &g_settings.fxOutlineA, &g_settings.fxOutlineR, &g_settings.fxOutlineG, &g_settings.fxOutlineB);
        ReadColorSetting(L"appearance.fxShadowColor", L"Appearance", L"Shadow Color", 255, 0, 0, 0,
                         &dummy, &g_settings.fxShadowR, &g_settings.fxShadowG, &g_settings.fxShadowB);
    }
    g_settings.fxShadow = ReadSizeSetting(L"appearance.fxShadow", 0.f, 100.f, 0.f);
    g_settings.fxShadowX = ReadNumberSetting(L"appearance.fxShadowX", L"Appearance", L"Shadow Offset X", 2.f, -32.f, 32.f);
    g_settings.fxShadowY = ReadNumberSetting(L"appearance.fxShadowY", L"Appearance", L"Shadow Offset Y", 3.f, -32.f, 32.f);
    g_settings.fxShadowSoft = ReadNumberSetting(L"appearance.fxShadowSoftness", L"Appearance", L"Shadow Softness", 4.f, 0.f, 32.f);
    g_settings.barHollow = VizSettingInt(L"appearance.barHollow") != 0;
    g_settings.barHollowWidth = ReadSizeSetting(L"appearance.barHollowWidth", 0.25f, 16.f, 1.5f);
    g_settings.barDash = ReadSizeSetting(L"appearance.barDash", 0.f, 200.f, 0.f);
    g_settings.barDashGap = ReadSizeSetting(L"appearance.barDashGap", 0.f, 200.f, 2.f);
    g_settings.barTilt = ReadSizeSetting(L"appearance.barTilt", -60.f, 60.f, 0.f);
    g_settings.barMirrorGap = ReadSizeSetting(L"appearance.barMirrorGap", 0.f, 400.f, 0.f);
    g_settings.afterimage = ReadSizeSetting(L"appearance.afterimage", 0.f, 100.f, 0.f);
    g_settings.afterimageSeconds = ReadSizeSetting(L"appearance.afterimageLength", 0.1f, 10.f, 1.2f);

    PCWSTR orientation = VizSettingString(L"appearance.orientation");
    g_settings.orientation =
        (wcscmp(orientation, L"vertical") == 0) ? VizOrientation::Vertical : VizOrientation::Horizontal;
    VizSettingFree(orientation);

    g_settings.barCount = std::clamp(VizSettingInt(L"appearance.barCount"), 1, VIZ_BARS_MAX);
    g_settings.barWidth = ReadSizeSetting(L"appearance.barWidth", 0.1f, 400.f, 6.f);
    g_settings.barGap = ReadSizeSetting(L"appearance.barGap", 0.f, 400.f, 4.f);
    g_settings.barMaxSize = ReadSizeSetting(L"appearance.barMaxSize", 2.f, 4000.f, 140.f);
    g_settings.barIdleSize = ReadSizeSetting(L"appearance.barIdleSize", 0.f, 400.f, 4.f);

    {
        float v[4];
        ReadQuadSetting(L"appearance.barCornerRadius", L"Appearance", L"Bar Corner Radius",
                        L"top-left, top-right, bottom-right, bottom-left", 3.0f, false, v);
        g_settings.barRadiusTL = v[0];
        g_settings.barRadiusTR = v[1];
        g_settings.barRadiusBR = v[2];
        g_settings.barRadiusBL = v[3];
    }

    PCWSTR colorMode = VizSettingString(L"appearance.colorMode");
    g_settings.colorMode = (wcscmp(colorMode, L"gradient") == 0)          ? VizColorMode::Gradient
                           : (wcscmp(colorMode, L"reactive_gradient") == 0) ? VizColorMode::ReactiveGradient
                           : (wcscmp(colorMode, L"accent") == 0)            ? VizColorMode::Accent
                           : (wcscmp(colorMode, L"album_art") == 0)         ? VizColorMode::AlbumArt
                           : (wcscmp(colorMode, L"dynamic_album") == 0)     ? VizColorMode::DynamicAlbum
                           : (wcscmp(colorMode, L"acrylic") == 0)           ? VizColorMode::Acrylic
                           : (wcscmp(colorMode, L"rainbow") == 0)           ? VizColorMode::RainbowCycle
                           : (wcscmp(colorMode, L"tourne") == 0)            ? VizColorMode::Tourne
                                                                             : VizColorMode::Solid;
    VizSettingFree(colorMode);

    ReadColorSetting(L"appearance.color", L"Appearance", L"Color", 255, 255, 255, 255,
                     &g_settings.colorA, &g_settings.colorR, &g_settings.colorG, &g_settings.colorB);
    ReadColorSetting(L"appearance.gradientColor1", L"Appearance", L"Gradient Color 1",
                     255, 30, 215, 96,
                     &g_settings.grad1A, &g_settings.grad1R, &g_settings.grad1G, &g_settings.grad1B);
    ReadColorSetting(L"appearance.gradientColor2", L"Appearance", L"Gradient Color 2",
                     255, 0, 180, 255,
                     &g_settings.grad2A, &g_settings.grad2R, &g_settings.grad2G, &g_settings.grad2B);

    g_settings.sensitivity = ReadSizeSetting(L"appearance.sensitivity", 0.f, 300.f, 150.f);

    PCWSTR sensCurve = VizSettingString(L"appearance.sensitivityCurve");
    g_settings.sensitivityCurve = (wcscmp(sensCurve, L"exponential") == 0) ? VizSensitivityCurve::Exponential
                                 : (wcscmp(sensCurve, L"power") == 0)      ? VizSensitivityCurve::Power
                                                                           : VizSensitivityCurve::Knee;
    VizSettingFree(sensCurve);

    g_settings.inputGainDb =
        ReadSizeSetting(L"appearance.inputGain", -24.f, 24.f, 0.f);
    g_settings.autoGain = VizSettingInt(L"appearance.autoGain") != 0;
    g_settings.autoGainMaxDb =
        ReadSizeSetting(L"appearance.autoGainMaxBoost", 0.f, 24.f, 12.f);

    g_settings.smoothing = ReadSizeSetting(L"appearance.smoothing", 0.f, 100.f, 0.f);

    PCWSTR eq = VizSettingString(L"appearance.eqPreset");
    g_settings.eq = (wcscmp(eq, L"bass") == 0)       ? VizEQ::Bass
                   : (wcscmp(eq, L"rock") == 0)       ? VizEQ::Rock
                   : (wcscmp(eq, L"pop") == 0)        ? VizEQ::Pop
                   : (wcscmp(eq, L"jazz") == 0)       ? VizEQ::Jazz
                   : (wcscmp(eq, L"electronic") == 0) ? VizEQ::Electronic
                                                       : VizEQ::Default;
    VizSettingFree(eq);

    g_settings.horizontalPosition = ReadNumberSetting(L"position.horizontalPosition", L"Position",
                                                      L"Horizontal Position", 50.0f, 0.0f, 100.0f);
    g_settings.verticalPosition = ReadNumberSetting(L"position.verticalPosition", L"Position",
                                                    L"Vertical Position", 88.0f, 0.0f, 100.0f);
    g_settings.monitor = std::max(1, VizSettingInt(L"position.monitor"));

    g_settings.keyMoveEnabled = VizSettingInt(L"interaction.keyMoveEnabled") != 0;
    VizSyncKeyboardHook();

    PCWSTR keyMoveModifier = VizSettingString(L"interaction.keyMoveModifier");
    g_settings.keyMoveModifier =
        (wcscmp(keyMoveModifier, L"ctrl_shift") == 0) ? (VIZ_MOD_CTRL | VIZ_MOD_SHIFT)
        : (wcscmp(keyMoveModifier, L"alt_shift") == 0) ? (VIZ_MOD_ALT | VIZ_MOD_SHIFT)
        : (wcscmp(keyMoveModifier, L"win_alt") == 0)   ? (VIZ_MOD_WIN | VIZ_MOD_ALT)
        : (wcscmp(keyMoveModifier, L"win_shift") == 0) ? (VIZ_MOD_WIN | VIZ_MOD_SHIFT)
        : (wcscmp(keyMoveModifier, L"ctrl") == 0)      ? (unsigned)VIZ_MOD_CTRL
        : (wcscmp(keyMoveModifier, L"alt") == 0)       ? (unsigned)VIZ_MOD_ALT
        : (wcscmp(keyMoveModifier, L"shift") == 0)     ? (unsigned)VIZ_MOD_SHIFT
        : (wcscmp(keyMoveModifier, L"win") == 0)       ? (unsigned)VIZ_MOD_WIN
                                                       : (VIZ_MOD_CTRL | VIZ_MOD_ALT);
    VizSettingFree(keyMoveModifier);

    PCWSTR keyMoveKeys = VizSettingString(L"interaction.keyMoveKeys");
    g_settings.keyMoveKeys = (wcscmp(keyMoveKeys, L"arrows") == 0) ? VizKeyMoveKeys::Arrows
                            : (wcscmp(keyMoveKeys, L"wasd") == 0)  ? VizKeyMoveKeys::Wasd
                                                                   : VizKeyMoveKeys::Both;
    VizSettingFree(keyMoveKeys);

    g_settings.keyMoveStep = ReadSizeSetting(L"interaction.keyMoveStep", 0.01f, 500.f, 1.f);
    g_settings.keyMoveFastStep = ReadSizeSetting(L"interaction.keyMoveFastStep", 0.01f, 500.f, 10.f);

    PCWSTR keyMoveFastKey = VizSettingString(L"interaction.keyMoveFastKey");
    g_settings.keyMoveFastKey = (wcscmp(keyMoveFastKey, L"ctrl") == 0)  ? (unsigned)VIZ_MOD_CTRL
                               : (wcscmp(keyMoveFastKey, L"alt") == 0)  ? (unsigned)VIZ_MOD_ALT
                               : (wcscmp(keyMoveFastKey, L"win") == 0)  ? (unsigned)VIZ_MOD_WIN
                               : (wcscmp(keyMoveFastKey, L"none") == 0) ? (unsigned)VIZ_MOD_NONE
                                                                        : (unsigned)VIZ_MOD_SHIFT;
    VizSettingFree(keyMoveFastKey);
    {
        PCWSTR v = VizSettingString(L"interaction.keyMoveFineKey");
        g_settings.keyMoveFineKey = (wcscmp(v, L"shift") == 0) ? (unsigned)VIZ_MOD_SHIFT
                                  : (wcscmp(v, L"ctrl") == 0)  ? (unsigned)VIZ_MOD_CTRL
                                  : (wcscmp(v, L"alt") == 0)   ? (unsigned)VIZ_MOD_ALT
                                  : (wcscmp(v, L"win") == 0)   ? (unsigned)VIZ_MOD_WIN
                                                               : (unsigned)VIZ_MOD_NONE;
        VizSettingFree(v);
        v = VizSettingString(L"interaction.keyMoveFineStep");
        float f = v ? (float)_wtof(v) : 0.f;
        g_settings.keyMoveFineStep = (f > 0.f) ? std::clamp(f, 1.0f / 64.0f, 1.0f) : 0.25f;
        VizSettingFree(v);
        g_settings.keyMoveFine = false;  // a quick setting only
        g_settings.pixelSnap = VizSettingInt(L"position.pixelSnap") != 0;
    }

    // A single-key modifier turns everyday shortcuts into visualizer moves, and
    // this hook swallows the keypress outright -- so the app underneath doesn't
    // fall back to its own behaviour, it just never hears about it. Ctrl+S
    // quietly not saving is the kind of thing you find out about much later,
    // which makes this worth saying up front rather than leaving to be
    // discovered.
    if (g_settings.keyMoveEnabled) {
        unsigned m = g_settings.keyMoveModifier;
        bool singleKey = m != 0 && (m & (m - 1)) == 0;
        if (singleKey) {
            std::wstring modName = ModKeyFlagsName(m);
            std::wstring detail =
                L"A one-key modifier means this mod swallows ordinary shortcuts. With " +
                modName + L" you will lose ";

            bool wasd = g_settings.keyMoveKeys != VizKeyMoveKeys::Arrows;
            bool arrows = g_settings.keyMoveKeys != VizKeyMoveKeys::Wasd;
            if (wasd) {
                detail += modName + L"+W, +A, +S and +D";
                if (m == VIZ_MOD_CTRL) detail += L" (so no Select All, and no Save)";
            }
            if (arrows) {
                if (wasd) detail += L", plus ";
                detail += modName + L"+arrow keys (word-by-word cursor movement)";
            }
            detail += L", and 1-4 and Home alongside it.\r\n      "
                      L"Everything still works -- this is only a heads-up. Switch to a two-key "
                      L"combo such as Ctrl + Alt, or set Direction Keys to Arrow Keys only, to "
                      L"get those shortcuts back.";

            ReportSettingWarning(L"Interaction", L"Keyboard Move Modifier", detail);
        }
    }

    // A fast key that's already part of the modifier can never be "additionally
    // held", so the fast step would silently never fire. Say so instead.
    if (g_settings.keyMoveFastKey && (g_settings.keyMoveFastKey & g_settings.keyMoveModifier)) {
        WCHAR expected[192];
        swprintf_s(expected, L"a key that isn't already part of the modifier (%s)",
                   ModKeyFlagsName(g_settings.keyMoveModifier));
        ReportSettingIssue(L"Interaction", L"Keyboard Move Fast Key",
                           ModKeyFlagsName(g_settings.keyMoveFastKey), expected,
                           L"no fast step at all");
        g_settings.keyMoveFastKey = VIZ_MOD_NONE;
    }

    g_settings.dragEnabled = VizSettingInt(L"interaction.dragEnabled") != 0;
    {
        PCWSTR host = VizSettingString(L"position.host");
        g_settings.hostTaskbar = wcscmp(host, L"taskbar") == 0;
        g_settings.hostPanel = wcscmp(host, L"panel") == 0;
        VizSettingFree(host);
        PCWSTR backdrop = VizSettingString(L"position.panelBackdrop");
        g_settings.panelBackdrop = wcscmp(backdrop, L"none") == 0 ? 0 : wcscmp(backdrop, L"mica") == 0 ? 2 : 1;
        VizSettingFree(backdrop);
        PCWSTR hotkey = VizSettingString(L"position.panelHotkey");
        g_settings.panelHotkey = hotkey ? hotkey : L"";
        VizSettingFree(hotkey);
    }
    g_settings.splitEnabled = VizSettingInt(L"position.split") != 0;
    g_settings.splitAt = ReadSizeSetting(L"position.splitAt", 1.f, 99.f, 50.f);
    g_settings.splitGap = ReadSizeSetting(L"position.splitGap", -8000.f, 8000.f, 200.f);
    g_settings.splitShift = ReadSizeSetting(L"position.splitShift", -8000.f, 8000.f, 0.f);
    {
        PCWSTR app = VizSettingString(L"position.dockApp");
        std::wstring a = app ? app : L"";
        VizSettingFree(app);
        while (!a.empty() && iswspace(a.back())) a.pop_back();
        size_t b = 0;
        while (b < a.size() && iswspace(a[b])) b++;
        a = a.substr(b);
        for (auto& ch : a) ch = (WCHAR)towlower(ch);
        if (a.size() > 4 && a.compare(a.size() - 4, 4, L".exe") == 0) a.resize(a.size() - 4);
        g_settings.dockApp = a;
        PCWSTR side = VizSettingString(L"position.dockSide");
        g_settings.dockSide = !wcscmp(side, L"above") ? 1 : !wcscmp(side, L"left") ? 2 : !wcscmp(side, L"right") ? 3 : 0;
        VizSettingFree(side);
        PCWSTR align = VizSettingString(L"position.dockAlign");
        g_settings.dockAlign = !wcscmp(align, L"start") ? 0 : !wcscmp(align, L"end") ? 2 : 1;
        VizSettingFree(align);
        g_settings.dockGap = ReadSizeSetting(L"position.dockGap", -400.f, 400.f, 0.f);
        g_settings.dockShift = ReadSizeSetting(L"position.dockShift", -4000.f, 4000.f, 0.f);
    }
    g_settings.dragSnap = VizSettingInt(L"interaction.dragSnap") != 0;
    g_settings.dragSnapDistance = ReadSizeSetting(L"interaction.dragSnapDistance", 1.f, 64.f, 8.f);

    PCWSTR dragModifier = VizSettingString(L"interaction.dragModifier");
    g_settings.dragModifier = (wcscmp(dragModifier, L"none") == 0)  ? VizDragModifier::None
                             : (wcscmp(dragModifier, L"alt") == 0)   ? VizDragModifier::Alt
                             : (wcscmp(dragModifier, L"shift") == 0) ? VizDragModifier::Shift
                             : (wcscmp(dragModifier, L"win") == 0)   ? VizDragModifier::Win
                                                                     : VizDragModifier::Ctrl;
    VizSettingFree(dragModifier);

    PCWSTR dragButton = VizSettingString(L"interaction.dragButton");
    g_settings.dragButton = (wcscmp(dragButton, L"left") == 0)  ? VizDragButton::Left
                           : (wcscmp(dragButton, L"right") == 0) ? VizDragButton::Right
                                                                  : VizDragButton::Middle;
    VizSettingFree(dragButton);

    PCWSTR verticalAnchor = VizSettingString(L"appearance.verticalAnchor");
    g_settings.verticalAnchor = (wcscmp(verticalAnchor, L"top")    == 0) ? VizAnchor::Top
                              : (wcscmp(verticalAnchor, L"middle") == 0) ? VizAnchor::Middle
                                                                         : VizAnchor::Bottom;
    VizSettingFree(verticalAnchor);

    g_settings.backgroundEnabled = VizSettingInt(L"background.enabled") != 0;

    ReadColorSetting(L"background.color", L"Background", L"Color", 0x60, 0, 0, 0,
                     &g_settings.bgA, &g_settings.bgR, &g_settings.bgG, &g_settings.bgB);

    {
        // Negative values are allowed on purpose -- they shrink that side of the
        // box inward, past the bars if pushed far enough. Extreme values are
        // clamped later, at the point the box's actual rect is built, rather
        // than here where the bar/panel size isn't known yet.
        float v[4];
        ReadQuadSetting(L"background.padding", L"Background", L"Padding",
                        L"left, right, top, bottom", 24.0f, true, v);
        g_settings.bgPaddingL = v[0];
        g_settings.bgPaddingR = v[1];
        g_settings.bgPaddingT = v[2];
        g_settings.bgPaddingB = v[3];
    }

    {
        float v[4];
        ReadQuadSetting(L"background.cornerRadius", L"Background", L"Corner Radius",
                        L"top-left, top-right, bottom-right, bottom-left", 14.0f, false, v);
        g_settings.bgRadiusTL = v[0];
        g_settings.bgRadiusTR = v[1];
        g_settings.bgRadiusBR = v[2];
        g_settings.bgRadiusBL = v[3];
    }
    g_settings.bgBlur = ReadSizeSetting(L"background.blur", 0.f, 250.f, 0.f);
    g_settings.bgBorderSize = ReadSizeSetting(L"background.borderSize", 0.f, 100.f, 0.f);

    ReadColorSetting(L"background.borderColor", L"Background", L"Border Color", 0x40, 255, 255, 255,
                     &g_settings.borderA, &g_settings.borderR, &g_settings.borderG,
                     &g_settings.borderB);

    // 0 = match the display (the engine thread reads the refresh rate).
    g_settings.targetFps = std::max(0, VizSettingInt(L"performance.targetFps"));
    g_settings.pauseOnFullscreen = VizSettingInt(L"performance.pauseOnFullscreen") != 0;
    g_settings.pauseWhenSilentSeconds = ReadSizeSetting(L"performance.pauseWhenSilentSeconds", 0.f, 86400.f, 10.f);
    g_settings.deepIdle = VizSettingInt(L"performance.deepIdle") != 0;
    g_perfStatsEnabled.store(VizSettingInt(L"performance.perfStats") != 0, std::memory_order_relaxed);

    g_settings.peakHoldEnabled = VizSettingInt(L"appearance.peakHoldEnabled") != 0;

    ReadColorSetting(L"appearance.peakHoldColor", L"Appearance", L"Peak Hold Cap Color",
                     255, 0, 180, 255,
                     &g_settings.peakHoldA, &g_settings.peakHoldR, &g_settings.peakHoldG,
                     &g_settings.peakHoldB);
    g_settings.beatFlashEnabled = VizSettingInt(L"appearance.beatFlashEnabled") != 0;

    ReadColorSetting(L"appearance.beatFlashColor", L"Appearance", L"Beat Flash Color",
                     255, 255, 255, 255,
                     &g_settings.beatFlashA, &g_settings.beatFlashR, &g_settings.beatFlashG,
                     &g_settings.beatFlashB);

    g_settings.beatFlashIntensity = ReadSizeSetting(L"appearance.beatFlashIntensity", 0.f, 300.f, 80.f);
    g_settings.rainbowSpeed = ReadSizeSetting(L"appearance.rainbowSpeed", 1.f, 300.f, 40.f);

    g_settings.nowPlayingEnabled = VizSettingInt(L"appearance.nowPlayingEnabled") != 0;
    ReadColorSetting(L"appearance.nowPlayingColor", L"Appearance", L"Now Playing Text Color",
                     255, 255, 255, 255,
                     &g_settings.nowPlayingA, &g_settings.nowPlayingR, &g_settings.nowPlayingG,
                     &g_settings.nowPlayingB);
    PCWSTR nowPlayingFont = VizSettingString(L"appearance.nowPlayingFont");
    g_settings.nowPlayingFont = (nowPlayingFont && *nowPlayingFont) ? nowPlayingFont : L"Segoe UI";
    // Only worth checking when the text is actually going to be drawn -- an
    // unused font name being wrong isn't something to interrupt anyone over.
    //
    // A failure here is not reported yet: on a cold boot the font service may
    // not have registered everything, so this arms the retry instead and the
    // warning only happens if it is still missing once things have settled.
    if (g_settings.nowPlayingEnabled && !IsFontInstalled(g_settings.nowPlayingFont)) {
        g_fontCheckPending = true;
        g_fontCheckAttempts = 0;
        if (g_messageWnd) SetTimer(g_messageWnd, TIMER_ID_MSG_FONT_RECHECK, 1000, nullptr);
    } else {
        g_fontCheckPending = false;
        if (g_messageWnd) KillTimer(g_messageWnd, TIMER_ID_MSG_FONT_RECHECK);
    }
    VizSettingFree(nowPlayingFont);
    g_settings.nowPlayingFontSize = ReadSizeSetting(L"appearance.nowPlayingFontSize", 6.f, 400.f, 16.f);
    g_settings.nowPlayingDisplaySeconds =
        ReadSizeSetting(L"appearance.nowPlayingDisplaySeconds", 0.f, 86400.f, 6.f);

    // Clamped rather than free: each pixel of offset widens the render surface
    // by the same amount, so a stray extra zero shouldn't silently cost memory
    // and fill rate for a surface mostly full of nothing.
    g_settings.nowPlayingOffsetX = ReadSizeSetting(L"appearance.nowPlayingOffsetX", -4000.f, 4000.f, 0.f);
    g_settings.nowPlayingOffsetY = ReadSizeSetting(L"appearance.nowPlayingOffsetY", -4000.f, 4000.f, 0.f);
    g_settings.peakFreqOffsetX = ReadSizeSetting(L"appearance.peakFreqOffsetX", -4000.f, 4000.f, 0.f);
    g_settings.peakFreqOffsetY = ReadSizeSetting(L"appearance.peakFreqOffsetY", -4000.f, 4000.f, 0.f);

    ReadColorSetting(L"appearance.nowPlayingBgColor", L"Appearance", L"Now Playing Background",
                     0, 0, 0, 0,
                     &g_settings.npBgA, &g_settings.npBgR, &g_settings.npBgG, &g_settings.npBgB);
    g_settings.npBgPadding = ReadSizeSetting(L"appearance.nowPlayingBgPadding", 0.f, 200.f, 6.f);
    g_settings.npBgCornerRadius = ReadSizeSetting(L"appearance.nowPlayingBgCornerRadius", 0.f, 200.f, 6.f);
    g_settings.npBgBorderSize = ReadSizeSetting(L"appearance.nowPlayingBgBorderSize", 0.f, 100.f, 0.f);
    ReadColorSetting(L"appearance.nowPlayingBgBorderColor", L"Appearance",
                     L"Now Playing Background Border Color", 0x40, 255, 255, 255,
                     &g_settings.npBgBorderA, &g_settings.npBgBorderR,
                     &g_settings.npBgBorderG, &g_settings.npBgBorderB);

    ReadColorSetting(L"appearance.peakFreqBgColor", L"Appearance", L"Peak Readout Background",
                     0, 0, 0, 0,
                     &g_settings.pfBgA, &g_settings.pfBgR, &g_settings.pfBgG, &g_settings.pfBgB);
    g_settings.pfBgPadding = ReadSizeSetting(L"appearance.peakFreqBgPadding", 0.f, 200.f, 6.f);
    g_settings.pfBgCornerRadius = ReadSizeSetting(L"appearance.peakFreqBgCornerRadius", 0.f, 200.f, 6.f);
    g_settings.pfBgBorderSize = ReadSizeSetting(L"appearance.peakFreqBgBorderSize", 0.f, 100.f, 0.f);
    ReadColorSetting(L"appearance.peakFreqBgBorderColor", L"Appearance",
                     L"Peak Readout Background Border Color", 0x40, 255, 255, 255,
                     &g_settings.pfBgBorderA, &g_settings.pfBgBorderR,
                     &g_settings.pfBgBorderG, &g_settings.pfBgBorderB);

    g_settings.autoHideEnabled = VizSettingInt(L"performance.autoHideEnabled") != 0;
    g_settings.autoHideDelaySeconds = ReadSizeSetting(L"performance.autoHideDelaySeconds", 0.f, 86400.f, 15.f);
    g_settings.pauseWhenObscured = VizSettingInt(L"performance.pauseWhenObscured") != 0;
    g_settings.obscuredThresholdPercent =
        ReadThresholdPercentSetting(L"performance.obscuredThresholdPercent", L"Performance",
                                    L"Pause When Covered - Threshold", 100);

    PCWSTR fftSizeStr = VizSettingString(L"appearance.fftSize");
    int fftSize = _wtoi(fftSizeStr);
    g_settings.fftSize = (fftSize == 1024 || fftSize == 2048 || fftSize == 4096 || fftSize == 8192)
                              ? fftSize : 2048;
    VizSettingFree(fftSizeStr);

    PCWSTR freqScale = VizSettingString(L"appearance.freqScale");
    g_settings.freqScale = (wcscmp(freqScale, L"linear") == 0) ? VizFreqScale::Linear
                          : (wcscmp(freqScale, L"mel") == 0)   ? VizFreqScale::Mel
                          : (wcscmp(freqScale, L"bark") == 0)  ? VizFreqScale::Bark
                          : (wcscmp(freqScale, L"erb") == 0)   ? VizFreqScale::Erb
                                                                : VizFreqScale::Log;
    VizSettingFree(freqScale);

    g_settings.peakFreqEnabled = VizSettingInt(L"appearance.peakFreqEnabled") != 0;

    PCWSTR pfH = VizSettingString(L"appearance.peakFreqAlignH");
    g_settings.peakFreqAlignH = (wcscmp(pfH, L"left") == 0)   ? VizTextAlignH::Left
                              : (wcscmp(pfH, L"center") == 0) ? VizTextAlignH::Center
                                                              : VizTextAlignH::Right;
    VizSettingFree(pfH);

    PCWSTR pfV = VizSettingString(L"appearance.peakFreqAlignV");
    g_settings.peakFreqAlignV = (wcscmp(pfV, L"above") == 0)  ? VizTextAlignV::Above
                              : (wcscmp(pfV, L"middle") == 0) ? VizTextAlignV::Middle
                              : (wcscmp(pfV, L"bottom") == 0) ? VizTextAlignV::Bottom
                              : (wcscmp(pfV, L"below") == 0)  ? VizTextAlignV::Below
                                                              : VizTextAlignV::Top;
    VizSettingFree(pfV);
    g_settings.oscilloscopeMultibandEnabled =
        VizSettingInt(L"appearance.oscilloscopeMultibandEnabled") != 0;
    g_settings.oscilloscopeWindowMs =
        ReadSizeSetting(L"appearance.oscilloscopeWindowMs", 5.f, 250.f, 24.f);
    g_settings.oscilloscopeDamping =
        ReadSizeSetting(L"appearance.oscilloscopeDamping", 0.f, 100.f, 0.f);

    g_settings.mediaControlsEnabled = VizSettingInt(L"media_controls.enabled") != 0;

    ReadColorSetting(L"media_controls.iconColor", L"Media Controls", L"Icon Color",
                     255, 255, 255, 255,
                     &g_settings.mediaIconColorA, &g_settings.mediaIconColorR,
                     &g_settings.mediaIconColorG, &g_settings.mediaIconColorB);

    g_settings.mediaIconPrevPath =
        ReadIconPathSetting(L"media_controls.iconPrevPath", L"Media Controls", L"Previous Icon Path");
    g_settings.mediaIconPlayPath =
        ReadIconPathSetting(L"media_controls.iconPlayPath", L"Media Controls", L"Play Icon Path");
    g_settings.mediaIconPausePath =
        ReadIconPathSetting(L"media_controls.iconPausePath", L"Media Controls", L"Pause Icon Path");
    g_settings.mediaIconNextPath =
        ReadIconPathSetting(L"media_controls.iconNextPath", L"Media Controls", L"Next Icon Path");

    g_settings.mediaIconSize = ReadSizeSetting(L"media_controls.iconSize", 8.f, 256.f, 32.f);
    {
        PCWSTR layout = VizSettingString(L"media_controls.layout");
        g_settings.mediaCard = layout && wcscmp(layout, L"card") == 0;
        VizSettingFree(layout);
    }
    ReadColorSetting(L"media_controls.cardBackground", L"Media Controls", L"Card Background", 158, 10, 10, 13,
                     &g_settings.cardBgA, &g_settings.cardBgR, &g_settings.cardBgG, &g_settings.cardBgB);
    ReadColorSetting(L"media_controls.cardBorderColor", L"Media Controls", L"Card Border Color", 0, 255, 255, 255,
                     &g_settings.cardBorderA, &g_settings.cardBorderR, &g_settings.cardBorderG, &g_settings.cardBorderB);
    g_settings.cardBorderSize = ReadSizeSetting(L"media_controls.cardBorderSize", 0.f, 20.f, 0.f);
    g_settings.cardRadius = ReadSizeSetting(L"media_controls.cardCornerRadius", 0.f, 64.f, 12.f);
    g_settings.cardArtSize = ReadSizeSetting(L"media_controls.cardArtSize", 0.f, 600.f, 0.f);
    {
        PCWSTR acc = VizSettingString(L"media_controls.cardAccent");
        g_settings.cardAccentSource = !acc ? 0 : wcscmp(acc, L"custom") == 0 ? 1 : wcscmp(acc, L"album") == 0 ? 2
                                    : wcscmp(acc, L"windows") == 0 ? 3 : 0;
        VizSettingFree(acc);
    }
    ReadColorSetting(L"media_controls.cardAccentColor", L"Media Controls", L"Card Accent Color", 255, 255, 255, 255,
                     &g_settings.cardAccentA, &g_settings.cardAccentR, &g_settings.cardAccentG, &g_settings.cardAccentB);
    g_settings.mediaIconSpacing = ReadSizeSetting(L"media_controls.iconSpacing", 0.f, 200.f, 14.f);

    ReadColorSetting(L"media_controls.plateColor", L"Media Controls", L"Backing Plate Color",
                     0, 0, 0, 0,
                     &g_settings.mediaPlateA, &g_settings.mediaPlateR,
                     &g_settings.mediaPlateG, &g_settings.mediaPlateB);
    g_settings.mediaPlateCornerRadius = ReadSizeSetting(L"media_controls.plateCornerRadius", 0.f, 200.f, 8.f);
    g_settings.mediaPlatePadding = ReadSizeSetting(L"media_controls.platePadding", 0.f, 200.f, 0.f);
    g_settings.mediaPlateBorderSize = ReadSizeSetting(L"media_controls.plateBorderSize", 0.f, 100.f, 0.f);
    ReadColorSetting(L"media_controls.plateBorderColor", L"Media Controls",
                     L"Backing Plate Border Color", 0x40, 255, 255, 255,
                     &g_settings.mediaPlateBorderA, &g_settings.mediaPlateBorderR,
                     &g_settings.mediaPlateBorderG, &g_settings.mediaPlateBorderB);
    g_settings.mediaHideWhenCovered = VizSettingInt(L"media_controls.hideWhenCovered") != 0;
    {
        PCWSTR z = VizSettingString(L"media_controls.stacking");
        g_settings.mediaOnTop = wcscmp(z, L"normal") != 0;
        VizSettingFree(z);
    }
    g_settings.mediaCoveredThresholdPercent =
        ReadThresholdPercentSetting(L"media_controls.coveredThresholdPercent", L"Media Controls",
                                    L"Hide When Covered - Threshold", 50);

    g_settings.mediaHorizontalPosition =
        ReadNumberSetting(L"media_controls.horizontalPosition", L"Media Controls",
                          L"Horizontal Position", 50.0f, 0.0f, 100.0f);
    g_settings.mediaVerticalPosition =
        ReadNumberSetting(L"media_controls.verticalPosition", L"Media Controls",
                          L"Vertical Position", 95.0f, 0.0f, 100.0f);

    PCWSTR renderDevice = VizSettingString(L"hardware.renderDevice");
    g_settings.renderDevice = (wcscmp(renderDevice, L"integrated") == 0) ? VizRenderDevice::Integrated
                            : (wcscmp(renderDevice, L"discrete") == 0)   ? VizRenderDevice::Discrete
                            : (wcscmp(renderDevice, L"cpu") == 0)        ? VizRenderDevice::Cpu
                                                                         : VizRenderDevice::Auto;
    VizSettingFree(renderDevice);

    // ---- 2.0: Workload, Renderer, Opaque Panel --------------------------------------
    {
        PCWSTR v = VizSettingString(L"hardware.workload");
        g_settings.workload = (wcscmp(v, L"gpu") == 0)   ? VizWorkload::Gpu
                            : (wcscmp(v, L"cpu") == 0)   ? VizWorkload::Cpu
                                                         : VizWorkload::Hybrid;
        VizSettingFree(v);
        v = VizSettingString(L"hardware.renderer");
        g_settings.renderer = (wcscmp(v, L"direct2d") == 0) ? VizRenderer::Direct2D : VizRenderer::D3D11;
        VizSettingFree(v);
        v = VizSettingString(L"hardware.opaquePanel");
        g_settings.opaquePanel = (wcscmp(v, L"off") == 0) ? VizOpaquePanel::Off : VizOpaquePanel::Auto;
        VizSettingFree(v);
    }

    // ---- 2.0: Analysis ------------------------------------------------------------------
    {
        auto str = [](PCWSTR key, auto&& fn) {
            PCWSTR v = VizSettingString(key);
            fn(v ? v : L"");
            VizSettingFree(v);
        };
        str(L"analysis.engine", [](PCWSTR v) {
            g_settings.engine = (wcscmp(v, L"classic") == 0) ? VizEngineKind::Classic : VizEngineKind::Precision;
        });
        str(L"analysis.bandLayout", [](PCWSTR v) {
            g_settings.bandLayout = (wcscmp(v, L"iec") == 0)       ? VizBandLayout::Iec
                                  : (wcscmp(v, L"musical") == 0)   ? VizBandLayout::Musical
                                                                   : VizBandLayout::Scale;
        });
        str(L"analysis.octaveFraction", [](PCWSTR v) {
            int f = _wtoi(v);
            g_settings.octaveFraction = (f == 1 || f == 3 || f == 6 || f == 12 || f == 24) ? f : 6;
        });
        g_settings.minFreq = ReadSizeSetting(L"analysis.minFrequency", 10.f, 1000.f, 20.f);
        g_settings.maxFreq = ReadSizeSetting(L"analysis.maxFrequency", 1000.f, 21000.f, 20000.f);
        if (g_settings.maxFreq <= g_settings.minFreq * 2) g_settings.maxFreq = std::max(1000.f, g_settings.minFreq * 4);
        {
            float a4 = ReadSizeSetting(L"analysis.tuningA4", 0.f, 1000.f, 440.f);
            g_settings.tuningA4 = (a4 >= 400.f && a4 <= 480.f) ? a4 : 440.f;
        }
        str(L"analysis.weighting", [](PCWSTR v) {
            g_settings.weighting = (wcscmp(v, L"a") == 0) ? VizWeighting::A
                                 : (wcscmp(v, L"c") == 0) ? VizWeighting::C
                                                          : VizWeighting::Z;
        });
        str(L"analysis.tilt", [](PCWSTR v) {
            float t = (float)_wtof(v);
            g_settings.tiltDbPerOct = (*v) ? std::clamp(t, 0.f, 6.f) : 1.5f;
        });
        str(L"analysis.detector", [](PCWSTR v) {
            g_settings.detector = (wcscmp(v, L"peak") == 0) ? VizDetector::Peak : VizDetector::Rms;
        });
        str(L"analysis.levelReference", [](PCWSTR v) {
            g_settings.levelRef = (wcscmp(v, L"band") == 0) ? VizLevelRef::Band : VizLevelRef::ThirdOctave;
        });
        str(L"analysis.window", [](PCWSTR v) {
            g_settings.window = (wcscmp(v, L"hamming") == 0)           ? VizWindowKind::Hamming
                              : (wcscmp(v, L"blackman_harris") == 0)   ? VizWindowKind::BlackmanHarris
                              : (wcscmp(v, L"flat_top") == 0)          ? VizWindowKind::FlatTop
                                                                       : VizWindowKind::Hann;
        });
        str(L"analysis.bassDetail", [](PCWSTR v) {
            g_settings.bassDetail = (*v) ? std::clamp(_wtoi(v), 0, 2) : 2;
        });
        str(L"analysis.channel", [](PCWSTR v) {
            g_settings.channel = (wcscmp(v, L"left") == 0)    ? VizChannel::Left
                               : (wcscmp(v, L"right") == 0)   ? VizChannel::Right
                               : (wcscmp(v, L"mid") == 0)     ? VizChannel::Mid
                               : (wcscmp(v, L"side") == 0)    ? VizChannel::Side
                                                              : VizChannel::Mix;
        });
        g_settings.dbFloor = ReadSizeSetting(L"analysis.dbFloor", -120.f, -20.f, -72.f);
        g_settings.dbCeiling = ReadSizeSetting(L"analysis.dbCeiling", -60.f, 0.f, -12.f);
        if (g_settings.dbCeiling - g_settings.dbFloor < 10) {
            ReportSettingIssue(L"Analysis", L"Display Ceiling (dBFS)", VizNumText(g_settings.dbCeiling).c_str(),
                               L"at least 10 dB above Display Floor", L"Display Floor + 10");
            g_settings.dbCeiling = g_settings.dbFloor + 10;
        }
        str(L"analysis.ballistics", [](PCWSTR v) {
            g_settings.ballistics = (wcscmp(v, L"smooth") == 0)     ? VizBallisticsPreset::Smooth
                                  : (wcscmp(v, L"analyzer") == 0)   ? VizBallisticsPreset::Analyzer
                                  : (wcscmp(v, L"vu") == 0)         ? VizBallisticsPreset::Vu
                                  : (wcscmp(v, L"ppm_ebu") == 0)    ? VizBallisticsPreset::PpmEbu
                                  : (wcscmp(v, L"ppm_din") == 0)    ? VizBallisticsPreset::PpmDin
                                  : (wcscmp(v, L"custom") == 0)     ? VizBallisticsPreset::Custom
                                                                    : VizBallisticsPreset::Snappy;
        });
        g_settings.attackMs = ReadSizeSetting(L"analysis.attackMs", 1.f, 500.f, 10.f);
        g_settings.releaseDbPerSec = ReadSizeSetting(L"analysis.releaseDbPerSecond", 1.f, 200.f, 20.f);
        g_settings.peakHoldMs = ReadSizeSetting(L"analysis.peakHoldMs", 0.f, 5000.f, 500.f);
        str(L"analysis.peakFall", [](PCWSTR v) {
            g_settings.peakFall = (wcscmp(v, L"linear") == 0) ? VizPeakFall::Linear : VizPeakFall::Gravity;
        });
        str(L"analysis.readout", [](PCWSTR v) {
            g_settings.readout = (wcscmp(v, L"loudness") == 0)        ? VizReadout::Loudness
                               : (wcscmp(v, L"loudness_full") == 0)   ? VizReadout::LoudnessFull
                               : (wcscmp(v, L"both") == 0)            ? VizReadout::Both
                                                                      : VizReadout::Frequency;
        });
        g_settings.loudnessResetOnTrack = VizSettingInt(L"analysis.loudnessResetOnTrack") != 0;

        // ---- 2.0: audio source, media widget, Terminal ------------------------------
        str(L"audio.source", [](PCWSTR v) {
            g_settings.audioSourceKey = (wcscmp(v, L"default_input") == 0) ? L"default_input"
                                      : (wcscmp(v, L"named") == 0)         ? L"named"
                                                                           : L"";
        });
        if (g_settings.audioSourceKey == L"named") {
            PCWSTR raw = VizSettingString(L"audio.deviceName");
            std::wstring name = raw ? raw : L"";
            VizSettingFree(raw);
            while (!name.empty() && iswspace(name.front())) name.erase(name.begin());
            while (!name.empty() && iswspace(name.back())) name.pop_back();
            if (name.empty()) {
                ReportSettingIssue(L"Audio", L"Device Name", L"",
                                   L"part of a device's name, as Windows' Sound settings show it",
                                   L"the default output");
                g_settings.audioSourceKey.clear();
            } else {
                g_settings.audioSourceKey = L"name:" + name;
            }
        }

        str(L"appearance.nowPlayingLayout", [](PCWSTR v) {
            g_settings.npLayout = (wcscmp(v, L"two_lines") == 0) ? VizNpLayout::TwoLines : VizNpLayout::OneLine;
        });
        str(L"appearance.nowPlayingPlacement", [](PCWSTR v) {
            g_settings.npPlacement = (wcscmp(v, L"panel_top") == 0)      ? VizNpPlacement::PanelTop
                                   : (wcscmp(v, L"panel_bottom") == 0)   ? VizNpPlacement::PanelBottom
                                                                         : VizNpPlacement::Above;
        });
        str(L"appearance.nowPlayingAlign", [](PCWSTR v) {
            g_settings.npAlign = (wcscmp(v, L"left") == 0)    ? VizTextAlignH::Left
                               : (wcscmp(v, L"right") == 0)   ? VizTextAlignH::Right
                                                              : VizTextAlignH::Center;
        });
        ReadColorSetting(L"appearance.nowPlayingArtistColor", L"Appearance", L"Now Playing Artist Color", 0xB3, 255,
                         255, 255, &g_settings.npArtistA, &g_settings.npArtistR, &g_settings.npArtistG,
                         &g_settings.npArtistB);
        str(L"appearance.textRendering", [](PCWSTR v) { g_settings.textPixel = wcscmp(v, L"pixel") == 0; });

        g_settings.progressEnabled = VizSettingInt(L"progress.enabled") != 0;
        g_settings.progressSeek = VizSettingInt(L"progress.clickToSeek") != 0;
        str(L"progress.seekAppsMode", [](PCWSTR v) { g_settings.progressSeekOnlyListed = wcscmp(v, L"only") == 0; });
        str(L"progress.seekApps", [](PCWSTR v) {
            g_settings.progressSeekApps.clear();
            std::wstring cur;
            for (const WCHAR* c = v;; c++) {
                if (!*c || *c == L',' || *c == L';' || *c == L'\n') {
                    while (!cur.empty() && iswspace(cur.back())) cur.pop_back();
                    size_t b = 0;
                    while (b < cur.size() && iswspace(cur[b])) b++;
                    cur = cur.substr(b);
                    for (auto& ch : cur) ch = (WCHAR)towlower(ch);
                    if (cur.size() > 4 && cur.compare(cur.size() - 4, 4, L".exe") == 0) cur.resize(cur.size() - 4);
                    if (!cur.empty()) g_settings.progressSeekApps.push_back(cur);
                    cur.clear();
                    if (!*c) break;
                } else {
                    cur += *c;
                }
            }
        });
        str(L"progress.placement", [](PCWSTR v) {
            g_settings.progressPlacement = (wcscmp(v, L"above") == 0)          ? VizProgressPlacement::Above
                                         : (wcscmp(v, L"panel_bottom") == 0)   ? VizProgressPlacement::PanelBottom
                                                                               : VizProgressPlacement::Below;
        });
        g_settings.progressHeight = ReadSizeSetting(L"progress.height", 0.25f, 40.f, 2.f);
        g_settings.progressGap = ReadSizeSetting(L"progress.gap", 0.f, 200.f, 6.f);
        ReadColorSetting(L"progress.color", L"Track Progress", L"Color", 255, 255, 255, 255, &g_settings.progressA,
                         &g_settings.progressR, &g_settings.progressG, &g_settings.progressB);
        ReadColorSetting(L"progress.trackColor", L"Track Progress", L"Track Color", 0x40, 255, 255, 255,
                         &g_settings.progressTrackA, &g_settings.progressTrackR, &g_settings.progressTrackG,
                         &g_settings.progressTrackB);

        str(L"media_controls.anchor", [](PCWSTR v) {
            g_settings.mediaAnchor = (wcscmp(v, L"panel_top_left") == 0)       ? VizMediaAnchor::PanelTopLeft
                                   : (wcscmp(v, L"panel_top_right") == 0)      ? VizMediaAnchor::PanelTopRight
                                   : (wcscmp(v, L"panel_bottom_left") == 0)    ? VizMediaAnchor::PanelBottomLeft
                                   : (wcscmp(v, L"panel_bottom_right") == 0)   ? VizMediaAnchor::PanelBottomRight
                                                                               : VizMediaAnchor::Screen;
        });
        g_settings.mediaAnchorOffsetX = ReadSizeSetting(L"media_controls.anchorOffsetX", -2000.f, 2000.f, 8.f);
        g_settings.mediaAnchorOffsetY = ReadSizeSetting(L"media_controls.anchorOffsetY", -2000.f, 2000.f, 8.f);
        str(L"interaction.contextMenu", [](PCWSTR v) {
            g_settings.contextMenu = (wcscmp(v, L"ctrl_right_click") == 0) ? VizContextMenu::CtrlRightClick
                                   : (wcscmp(v, L"off") == 0)              ? VizContextMenu::Off
                                                                           : VizContextMenu::RightClick;
        });

        str(L"terminal.style", [](PCWSTR v) {
            g_settings.termStyle = (wcscmp(v, L"waterfall") == 0) ? VizTermStyle::Waterfall
                                 : (wcscmp(v, L"meters") == 0)    ? VizTermStyle::Meters
                                                                  : VizTermStyle::Columns;
        });
        str(L"terminal.font", [](PCWSTR v) { g_settings.termFont = *v ? v : L"Consolas"; });
        g_settings.termFontSize = ReadSizeSetting(L"terminal.fontSize", 6.f, 96.f, 14.f);
        g_settings.termRows = std::clamp(VizSettingInt(L"terminal.rows"), 2, 128);
        g_settings.termMeterColumns = std::clamp(VizSettingInt(L"terminal.meterColumns"), 20, 200);
        g_settings.termHotThreshold = ReadSizeSetting(L"terminal.hotThreshold", 1.f, 100.f, 75.f);
        g_settings.termScrollRate = ReadSizeSetting(L"terminal.scrollRate", 1.f, 120.f, 20.f);
        // Any one character each (2.1): ASCII as itself, anything else as a
        // custom glyph code. The font has to have it.
        g_termCustomGlyphs.clear();
        auto glyph = [](PCWSTR key, PCWSTR name, wchar_t def) {
            PCWSTR v = VizSettingString(key);
            std::vector<std::wstring> gs = VizTermSplitGlyphs(v ? v : L"");
            wchar_t c = 0;
            if (!gs.empty() && gs[0] != L" ") c = VizTermCodeFor(gs[0]);
            if (!c) {
                WCHAR d[2] = {def, 0};
                ReportSettingIssue(L"Terminal", name, v ? v : L"", L"one character", d);
                c = def;
            }
            VizSettingFree(v);
            return c;
        };
        g_settings.termColumnGlyph = glyph(L"terminal.columnGlyph", L"Column Glyph", L'#');
        g_settings.termPeakGlyph = glyph(L"terminal.peakGlyph", L"Peak Glyph", L'-');
        str(L"terminal.ramp", [](PCWSTR v) {
            std::wstring r;
            for (const auto& gl : VizTermSplitGlyphs(v))
                if (wchar_t c = VizTermCodeFor(gl)) r.push_back(c);
            g_settings.termRamp = r.size() >= 2 ? r : L" .:-=+*#%@";
        });
        ReadColorSetting(L"terminal.dimColor", L"Terminal", L"Dim Color", 255, 0x1E, 0x6B, 0x34, &g_settings.termDimA,
                         &g_settings.termDimR, &g_settings.termDimG, &g_settings.termDimB);
        ReadColorSetting(L"terminal.lowColor", L"Terminal", L"Color", 255, 0x33, 0xFF, 0x66, &g_settings.termLowA,
                         &g_settings.termLowR, &g_settings.termLowG, &g_settings.termLowB);
        ReadColorSetting(L"terminal.highColor", L"Terminal", L"Hot Color", 255, 0xFF, 0x3B, 0x3B, &g_settings.termHighA,
                         &g_settings.termHighR, &g_settings.termHighG, &g_settings.termHighB);
        ReadColorSetting(L"terminal.labelColor", L"Terminal", L"Label Color", 255, 0xB8, 0xFF, 0xB8,
                         &g_settings.termLabelA, &g_settings.termLabelR, &g_settings.termLabelG,
                         &g_settings.termLabelB);

        // Quick settings from the right-click menu sit on top of all of the
        // above (and are cleared from the same menu).
        VizApplyMenuOverrides();
        if (g_settings.workload == VizWorkload::Gpu && VizStyleNeedsCpuBars()) {
            ReportSettingWarning(L"Hardware", L"Workload",
                                 L"Spectrogram, Stereo Field and Particles work from the bar levels on the CPU, "
                                 L"so with them the analysis runs on the CPU (Hybrid).");
        }
        if (g_settings.workload == VizWorkload::Gpu && g_settings.shape == VizShape::Terminal) {
            ReportSettingWarning(L"Hardware", L"Workload",
                                 L"The Terminal shape is built on the CPU from the bar levels, so with it the "
                                 L"analysis runs on the CPU (Hybrid).");
        }

        // The IEC and musical layouts decide the bar count. Worked out here at
        // 48 kHz; the range is capped at 21 kHz, below every common device's
        // Nyquist limit, so the count is the same on 44.1 kHz.
        g_settings.layoutBandCount = 0;
        if (g_settings.engine == VizEngineKind::Precision && g_settings.bandLayout != VizBandLayout::Scale) {
            std::vector<ttdsp::Band> b =
                (g_settings.bandLayout == VizBandLayout::Iec)
                    ? ttdsp::IecBands(g_settings.octaveFraction, g_settings.minFreq, g_settings.maxFreq)
                    : ttdsp::MusicalBands(g_settings.octaveFraction, g_settings.tuningA4, g_settings.minFreq,
                                          g_settings.maxFreq);
            g_settings.layoutBandCount = std::clamp((int)b.size(), 1, VIZ_BARS_MAX);
        }

        // Combinations that can't be honoured, said out loud.
        if (g_settings.workload == VizWorkload::Gpu && g_settings.engine == VizEngineKind::Classic) {
            ReportSettingWarning(L"Hardware", L"Workload",
                                 L"GPU analysis is the Precision engine on the GPU. With the Classic engine the "
                                 L"analysis stays on the CPU (Hybrid).");
        }
        if (g_settings.workload == VizWorkload::Gpu && g_settings.renderer == VizRenderer::Direct2D) {
            ReportSettingWarning(L"Hardware", L"Workload",
                                 L"GPU analysis needs the Direct3D 11 renderer, so with Direct2D it runs on the "
                                 L"CPU (Hybrid).");
        }
    }

    PCWSTR smoothMode = VizSettingString(L"hardware.smoothMode");
    g_settings.smoothMode = (wcscmp(smoothMode, L"on") == 0)    ? VizSmoothMode::On
                          : (wcscmp(smoothMode, L"off") == 0)   ? VizSmoothMode::Off
                                                                : VizSmoothMode::Auto;
    VizSettingFree(smoothMode);

    g_gpuAnalysisFailed = false;  // tried again with the new settings
    VizPublishEngineConfig();
    // Smooth Mode's Auto depends on the device, which may not exist yet on the
    // first load; InitDirectX resolves it again once it does.
    VizUpdateSmoothActive();

    FlushSettingsIssues();
}

HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;

DWORD WINAPI UiThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    // This mod runs as its own dedicated process (see the tool-mod boilerplate
    // at the end of this file). WhTool_ModInit's own calling thread does not
    // survive -- the tool-mod launcher hooks the process's real entry point
    // and exits that thread once startup continues past mod init -- so every
    // window, and the message loop that keeps them alive, has to live on a
    // thread we spin up and own ourselves rather than whatever thread called
    // WhTool_ModInit.
    // Apartment-threaded, because this thread owns windows and pumps messages.
    // Without this, every CoCreateInstance made from here fails outright with
    // CO_E_NOTINITIALIZED -- which is why custom Media Controls icon paths
    // never loaded: WIC's imaging factory is created on this thread, silently
    // failed to activate, and fell back to the built-in glyph every time.
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(comHr)) {
        Wh_Log(L"CoInitializeEx on UI thread failed, hr=0x%08X", comHr);
    }

    LoadPositionOverride();

    CreateMessageWindow();
    CreateMediaControlWindow();
    CreateOverlayWindow();
    if (!g_overlayWnd && g_messageWnd) {
        SetTimer(g_messageWnd, TIMER_ID_MSG_RECREATE_OVERLAY, 1000, nullptr);
    }

    InitInputHooks();

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (SUCCEEDED(comHr)) CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");

    // Running injected into explorer.exe, this was inherited for free -- the
    // shell process is always Per-Monitor-V2 DPI aware. Running as our own
    // standalone process now, nothing declares that for us, so without this
    // call every GetMonitorInfo/GetDpiForMonitor-based position and size
    // calculation in the mod (the visualizer's own box, and the media
    // controls window) risks running against a DPI-virtualized view of the
    // desktop instead of true physical pixels on any scaled monitor.
    if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
        Wh_Log(L"SetProcessDpiAwarenessContext failed, error %u", GetLastError());
    }

    LoadSettings();

    RefreshAccentColorCache();

    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    StopRenderThread();

    if (g_overlayWnd) SendMessage(g_overlayWnd, WM_APP_CLEANUP, 0, 0);
    if (g_styleEditorWnd) SendMessage(g_styleEditorWnd, WM_CLOSE, 0, 0);
    if (g_messageWnd) SendMessage(g_messageWnd, WM_APP_CLEANUP, 0, 0);
    if (g_mediaWnd) SendMessage(g_mediaWnd, WM_APP_CLEANUP, 0, 0);

    UnregisterOverlayWindowClass();
    UnregisterMessageWindowClass();
    UnregisterMediaWindowClass();
    UnregisterStyleEditorClass();

    StopVizCaptureThread();
    if (g_engineWake) {
        CloseHandle(g_engineWake);
        g_engineWake = nullptr;
    }
    if (g_engineClosed) {
        CloseHandle(g_engineClosed);
        g_engineClosed = nullptr;
    }
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

    std::lock_guard<std::mutex> albumThreadLock(g_albumArtThreadMutex);
    if (g_albumArtThread) {
        if (g_albumArtThread->joinable()) {
            HANDLE hThread = g_albumArtThread->native_handle();
            if (WaitForSingleObject(hThread, 3000) == WAIT_OBJECT_0) {
                g_albumArtThread->join();
            } else {
                g_albumArtThread->detach();
            }
        }
        delete g_albumArtThread;
        g_albumArtThread = nullptr;
    }

    if (g_mediaCmdThread) {
        if (g_mediaCmdThread->joinable()) {
            HANDLE hThread = g_mediaCmdThread->native_handle();
            if (WaitForSingleObject(hThread, 3000) == WAIT_OBJECT_0) {
                g_mediaCmdThread->join();
            } else {
                g_mediaCmdThread->detach();
            }
        }
        delete g_mediaCmdThread;
        g_mediaCmdThread = nullptr;
    }

    g_gsmtcStarted = false;

    UninitInputHooks();

    if (g_uiThreadId) {
        PostThreadMessage(g_uiThreadId, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

void ApplySettingsChanged() {
    Wh_Log(L">");

    int oldMonitor = g_settings.monitor;
    VizColorMode oldColorMode = g_settings.colorMode;

    LoadSettings();

    // Place In changed: the window itself has to be made again (WM_DESTROY
    // schedules the new one).
    if (g_overlayWnd && (g_settings.hostTaskbar != g_overlayInTaskbar || g_settings.hostPanel != g_overlayInPanel)) {
        Wh_Log(L"[Host] Place In changed, recreating the overlay");
        PostMessage(g_overlayWnd, WM_APP_CLEANUP, 0, 0);
    }
    VizPanelBackdrop();
    VizPanelHotkey();

    // Creation at startup can fail (or the window can have been destroyed
    // since), and until now nothing retried -- turning Media Controls on in
    // settings then looked like it did nothing at all.
    if (!g_mediaWnd) {
        CreateMediaControlWindow();
    } else {
        // Cleared unconditionally so that turning Hide When Covered off, or
        // moving the strip somewhere clear, brings it straight back instead of
        // waiting for the next watch tick to notice.
        g_mediaHiddenByCover = false;
        RecreateMediaControlResources();
        RepositionAndRepaintMediaControls();
    }

    if ((g_settings.colorMode == VizColorMode::AlbumArt ||
         g_settings.colorMode == VizColorMode::DynamicAlbum) &&
        ((oldColorMode != VizColorMode::AlbumArt &&
          oldColorMode != VizColorMode::DynamicAlbum) || !g_albumArtColorReady.load()))
        FetchAlbumArtColorAsync();
    else if (g_settings.nowPlayingEnabled || VizCardWantsArt())
        FetchAlbumArtColorAsync();

    if (!g_lazyInitialized || !g_initSucceeded) return;

    if (!g_fullscreenPaused.load()) {
        if (g_settings.pauseOnFullscreen && IsFullscreenOrGameActive()) {
            PauseForFullscreen();
        }
    } else if (!g_settings.pauseOnFullscreen && !g_userPaused.load()) {
        ResumeFromFullscreen();
    }

    // Drawing Device changed, or (on Auto) the Monitor setting moved the
    // widget onto a screen driven by another GPU. The rebuild creates the
    // swap chain and visual resources from the new settings and redraws, so
    // nothing below needs to run after it.
    if (VizRenderDeviceNeedsRebuild()) {
        VizRebuildRenderDevice(L"Drawing Device or monitor changed");
        return;
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

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");

    // Has to land on the UI thread: it can create or resize windows that thread
    // owns. The overlay is preferred, but it's the one window that legitimately
    // may not exist yet (it waits on WorkerW), so fall back to the message
    // window -- which is created first and always there -- rather than running
    // this inline on Windhawk's own settings thread.
    if (g_overlayWnd) {
        SendMessage(g_overlayWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else if (g_messageWnd) {
        SendMessage(g_messageWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else {
        ApplySettingsChanged();
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
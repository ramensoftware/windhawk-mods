// ==WindhawkMod==
// @id              round-everything
// @name            Round Everything Interface
// @description     Turns the taskbar into an acrylic or solid frame around a rounded screen, in the active window's color, rounds the monitor corners, and gives every window - maximized included - one corner radius, a border color and a shadow
// @version         2.0.0
// @author          Deen-0x
// @github          https://github.com/Deen-0x
// @include         explorer.exe
// @include         dwm.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lgdi32 -lshell32 -lshcore -ldwmapi -lole32 -loleaut32 -lruntimeobject -ld3d11
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Round Everything Interface

Turns the taskbar into a frame around the whole screen. The taskbar keeps its
rectangular shape, and the other three screen edges get a band of the same
look - a solid color or a gradient, acrylic (a live blur of what is behind, tinted) or the
taskbar's own light or dark acrylic - of a configurable thickness. The
visible "screen" becomes a rounded rectangle that sits flush
against the taskbar, with one configurable corner radius. The frame can take
the color of the active window's title bar, easing from one window's to the
next. Inside dwm.exe, optionally, every window gets the same corner radius,
and maximized windows can keep a border and a shadow.

Works with the taskbar on any edge (top, bottom, left, right) and on every
monitor. A monitor without a taskbar ("Show my taskbar on all displays" off)
gets the frame on all four edges.

## Themes

Pick a **Theme** and you are done: each one sets everything - the frame, its
fill, the bevel, the inner shadow, the monitor corners, and the window
corners, borders and shadows inside `dwm.exe`.

* **Slate** (the default) - a dark solid frame that takes the active window's
  title-bar color, a soft inner shadow, rounded windows with shadows.
* **Colored film** - a thin acrylic film in the active window's color, a
  soft inner shadow, rounded windows with borders.
* **Frost ink** - a thicker acrylic frame with a lit bevel along its
  edge, blurred and tinted with the window's color.
* **Crescents fade** - the acrylic film along the taskbar's edge and the
  one opposite only; the two sides stay open. Rounded windows with borders.
* **Modern Windows** - the taskbar's own light or dark acrylic with a fine
  rim, Windows' 8 px corners on every window, maximized ones bordered.
* **Rectangular frame** - a square frame in the active window's color with a
  bright edge line, square windows with borders.
* **Crescent adaptive** - no frame to speak of: the rounded screen corners in
  the active window's color, a hairline along them and the taskbar's edge,
  rounded windows with borders.

The few values worth changing on a theme without leaving it are under
**Overrides** - frame thickness, screen corner radius, monitor corner radius, frame color and its
source, fill, work-area inset, window corner radius - each applying only when
set. **General** (DPI scaling, monitors, battery saver) applies with every theme.
Everything else in the settings belongs to the **Custom** theme, which uses
every setting as it is. To start a Custom theme from a ready-made one, apply
that theme: the mod logs its values as Custom theme settings, in the form
Windhawk's settings editor takes in its text (YAML) mode - paste them in and
tweak.

## How it works

The mod runs inside the taskbar's `explorer.exe` and creates one always-on-top
composition window per monitor, lying under the taskbar as well: the frame
shape - the taskbar's band and three bands of the configured thickness, with
the rounded corners - is rasterized once (anti-aliased) as a mask over the
fill; the inner area is fully transparent and lets the mouse through, while
the frame is solid to the mouse, like the taskbar. No explorer function is
hooked: the taskbar's XAML tree is reached through a few `taskbar.dll` symbol
lookups (no XAML diagnostics, so it coexists with Taskbar Styler); the one
hook in this process is on the `dwmapi.dll` call the taskbar makes for Aero
Peek, to learn which window is being peeked.

* **Fill** – *Custom acrylic* makes the frame and the taskbar one live
  acrylic surface: whatever is behind them - wallpaper, live wallpaper,
  windows - is blurred by DWM in real time (the `HostBackdropBrush` of
  Windows' own flyouts), optionally saturated, then tinted with the frame
  color. *Tint opacity* lays the color over the blur; *Luminosity opacity*
  makes the color's brightness replace the content's while its hues stay
  (Windows' acrylic uses 85-96 here, which keeps the surface readable over
  any wallpaper); a fine *noise* grain finishes it. The acrylic frame lies
  under the taskbar too, whose own background (`Taskbar.TaskbarBackground`)
  is made transparent, so the four sides are one surface without a seam. It
  stops one pixel short of the monitor edges (a topmost window covering a
  whole monitor is a full-screen app to the shell, which would lower the
  taskbar); that one-pixel ring is the bezel line, and the frame thickness is
  measured from its inside, as if the ring were the screen's edge. *Solid*
  (the default) is the same surface filled with the plain frame color at the
  chosen opacity - under the taskbar too, so the bevel and the corners run
  along the taskbar's edge as along the others. *Windows' default* is the
  taskbar's own acrylic look - the light or the dark tint, following the
  Windows mode as it changes - and ignores the frame color and the acrylic
  values. The taskbar's own background is restored when the mod is turned
  off.
* **Frame color** – one color, a gradient of several at an angle of your
  choice, or a pair for light and dark mode (each may be a gradient), so
  the frame follows the Windows mode. `accent` - or an empty frame color -
  is the Windows accent color in a shade that keeps the taskbar's labels
  readable: `SystemAccentColorDark2` on a dark taskbar (white labels; the
  shade Windows uses for an accent-colored taskbar) and
  `SystemAccentColorLight2` on a light one (black labels; the shade WinUI
  pairs with black text). The frame follows when the accent color or the
  mode changes.
* **Frame color source** – instead of the one frame color, each monitor's
  frame can take the color of the *title bar* of the maximized window on
  it: the window's fourth row inside the rounded screen (past the inner
  shadow where the title bar is tall enough), read off the screen the way
  Top Taskbar Bleed reads it - the app is never asked to render - and
  reduced to its dominant color (caption text and buttons are too few
  pixels to count; columns covered by a dialog or a flyout are left out, no
  row is read while a thumbnail preview hangs over it, nor while a window
  is still zooming in or out with the window animations on). A window seen
  before gets the color it read as the last time the moment it takes over,
  before it has even finished zooming in; the readings then confirm it. A
  row that differs from the color in use is read again every frame until
  two readings agree; with no maximized window the frame color is used. Hovering a taskbar thumbnail
  until Aero Peek shows the window gives the frame that window's color for
  as long as it shows, if it is a maximized window (or one minimized from
  maximized). The color goes wherever the frame color goes - the solid fill, the
  acrylic's tint and luminosity, the taskbar's background - and eases from
  one color to the next over a third of a second rather than switching
  (the acrylic's colors are animated by the compositor, the solid
  fill and the taskbar follow step by step). A light title bar on a
  dark-mode taskbar would leave white labels on a light panel (and the
  other way round), so each taskbar is switched to the theme matching its
  frame color - light color, light theme - and back to its own theme (the
  current Windows mode's) when the frame color is the frame color again. Per taskbar, through XAML's
  `RequestedTheme`, so the Windows mode itself is untouched - the mechanism
  is Top Taskbar Bleed's.
* **Acrylic glass bevel** (off by default) – the frame's inner edge is lit
  as a rounded bevel of acrylic glass: a specular highlight, a Fresnel rim
  and an edge line in a color of your choice, from one or two lights (one
  from each side, so both the top and the bottom edge catch a highlight)
  whose directions and strengths, the bevel's width and curvature are
  configurable (an edge shade darkens the acrylic glass towards the edge
  instead). It is rasterized once with the frame, for either fill, so it
  costs nothing while the screen changes.
* Frame and taskbar come and go together. When the mod starts, the frame
  appears in the same instant as the taskbar's new look. When the
  taskbar moves, resizes or rebuilds (changing its position, a taskbar mod
  reloading) its background turns transparent and the frame goes away the
  instant the taskbar window moves; once everything has settled - taskbar
  at its new edge and uncloaked, its corners in place, the work area
  reserved - the taskbar paints its background again and, from inside that
  same rendering pass, reveals the frame (re-shaped for the new edge), so
  the two reach the screen in one composition. When the mod is turned off
  the frame disappears as the taskbar is handed back.
* The frame stays visible during Aero Peek (hovering a taskbar thumbnail or
  the show-desktop corner), like the taskbar itself.
* **Task View** (Win+Tab) – the frame goes away and the taskbar's background
  and monitor corners turn transparent while it is open; everything comes
  back together when it closes.
* **Full-screen apps** – the frame hides on a monitor while a full-screen
  window (game, video, F11 browser) is showing there, and comes back
  afterwards. When the shell takes the taskbar off the top of the z-order
  for something it counts as full-screen that the mod doesn't - a screen
  recorder's full-screen overlay, say - the frame follows the taskbar down
  and stays right under it, so the icons stay on top and the frame keeps
  working.
* **Work area** – the work area is pushed inwards to the rounded screen, so
  maximized and snapped windows stay inside the frame and nothing is hidden
  under it. This is done with one invisible appbar per frame edge (the same
  mechanism the taskbar uses), so the shell reserves the space itself; the
  one on the taskbar's edge covers the taskbar's own strip (see the
  auto-hide note below). The appbars are removed and the work area returns
  to normal when the mod is turned off. *Extra work-area inset* shrinks it
  further, on all four sides, so a strip of wallpaper stays visible between
  the frame/taskbar and your windows.
* **Monitor corners** – a one-pixel black line runs along all four screen
  edges, and with a monitor corner radius above 0 the four outer corners of
  each monitor are rounded too, so the whole screen reads as a rounded panel with a bezel:
  the sliver outside a quarter circle, tangent to that line, is painted
  black. The two corners away from the taskbar are part of the frame itself
  (the line is four one-pixel windows around the monitor); the two on the
  taskbar, and the line along it, are XAML shapes added inside the taskbar
  (which lies above everything), click-through, removed when the mod is
  disabled - with an auto-hide taskbar, which takes its shapes with it when
  it slides out, the frame paints those two corners as well.
* **Inner shadow** – the frame and the taskbar can cast a soft shadow onto
  the screen content along all four edges of the rounded screen, following
  its corners, so the screen reads as recessed behind the frame. Blur, color,
  opacity and offset are configurable (the offset works like a CSS inset
  box-shadow: a positive horizontal offset thickens the shadow on the left
  edge, a positive vertical one on the top edge). The shadow is a
  click-through layer right in front of the frame (which must not blur
  it), never over the taskbar.
* **Taskbar border line** – the taskbar draws a 1px line along its inner edge
  (`BackgroundStroke`), which shows up as a seam between the taskbar and the
  frame. The mod sets that line's opacity to 0 and restores it when disabled.
* Offsets, radii and the blur are in pixels at 100% scaling and scaled per
  monitor (configurable).
* The fills are rendered by DWM through Windows.UI.Composition; if that
  can't be set up on a machine, the mod logs why and falls back to a plain
  layered frame that stops at the taskbar, whose background is then painted
  in the frame color. Like Windows' own acrylic, the acrylic fills are solid
  (the frame color) while transparency effects are off in the system's
  personalization settings or, with *Solid while battery saver is on*, while
  battery saver is on, and come back when they change.

## Window corners (dwm.exe)

Windows 11 rounds normal windows with a fixed 8 px radius and draws maximized
windows square. With any option of this group on, the mod also runs inside
`dwm.exe` and hooks the corner-radius decision
(`CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle`). With a **Window
corner radius** set, every window that Windows would round - normal, snapped
or maximized - gets that one radius (0 draws them square; -1 leaves the
corners to Windows), and maximized windows are rounded too, so they sit in
the rounded screen like cards. With **Border on maximized windows** and
**Shadow on maximized windows** on - with or without a radius - maximized
windows also keep the one-pixel
border Windows drops while a window is maximized, and the topmost one on each
monitor keeps the drop shadow (the border color and shadow style DWM hands
its border are swapped for a floating window's, in
`CWindowBorder::SetBorderParameters`), so they sit in the frame like floating
windows without stacked maximized windows piling their shadows up. Windows that opt out of rounding (`DONOTROUND`) and
small-radius surfaces (menus, tooltips) are left alone. Changes apply at
once: DWM is asked to re-evaluate every window (no window's own corner
preference is touched). Full-screen windows share the maximized path in DWM,
so they get rounded corners as well. The corners stay rounded while a
window animates, too: Windows clips a window being maximized, restored,
minimized or brought back from the taskbar with a clip of its own
(`CTopLevelWindow3D::UpdateAnimatedResources`), eased to square whenever the
animation starts or ends maximized; the mod keeps its radius on that clip
for the whole transition, and on a restore - which stretches the restored
window from its own size up to the maximized one - fits the window's own
clip to the stretch, so its corners don't widen with it.

**Custom border colors** replaces the gray one-pixel border Windows draws
around every window with colors of your own - one for the active window,
one for the others - in `#AARRGGBB` form, so the border can be as faint or
as opaque as you like, or `accent` for the Windows accent color (followed
within a second as it changes; the explorer.exe part hands it over, since
dwm.exe runs under an account of its own whose settings aren't yours). It applies
in the same border hook to floating, snapped and (with the option above)
maximized windows; menus, tooltips and windows that hid their own border are
left alone. Turn off *Show accent color on title bars and window borders* in
Windows' personalization settings if you use it, as that colors the same
border. The
[Window Border Customizer](https://windhawk.net/mods/window-border-customizer)
mod colors borders as well; use one mod or the other for border colors, not
both. The colors here are set inside DWM's own border update for the exact
window (identified through its window data, not by matching rectangles), so
they apply the instant a window is activated, maximized or restored, with no
re-evaluation of other windows, and they share one decision with the
maximized shadow - a maximized window's border and shadow are set together
in one update.

**This part needs one manual step.** Windhawk doesn't inject into `dwm.exe`
by default (it is on Windhawk's list of critical system processes), so the
window-corner part does nothing until you add `dwm.exe` to the **Process
inclusion list** in Windhawk's advanced settings. The frame, the monitor
corners and the work area don't depend on it and work right away. `udwm.dll`
symbols are downloaded by Windhawk on first use.

This does the same job as the *Rounded corners for maximized and snapped
windows* option of the
[Custom Window Corner Radius](https://windhawk.net/mods/custom-corner-radius)
mod, through the same hook - the mechanism is the one Alexey Lavrinenko
contributed there; use one mod or the other for window corners, not both.

## Notes

* Shell flyouts (Start, notification center, Task View, …) live above the
  frame, so they are never covered.
* The frame is solid to the mouse, like the taskbar: clicks on it (and on
  the extra work-area inset strip) go nowhere, which keeps the invisible
  resize borders of maximized windows underneath from being hit.
* With *Automatically hide the taskbar* on, the frame and the work area stay
  exactly as they are with the taskbar docked: the band the taskbar sits on
  stays part of the frame, the work area keeps clear of it, and the taskbar
  slides in and out over it - only its icons come and go. The mod keeps an
  invisible appbar of its own over the taskbar's strip at all times: a
  docked taskbar reserves the same strip,
  so nothing changes, and when auto-hide releases it the strip stays
  reserved, so no window moves; mods that toggle auto-hide themselves, such
  as Taskbar auto-hide when maximized, work the same way. The plain layered
  fallback frame stops at the taskbar's strip, so there the wallpaper shows
  while the taskbar is hidden.
* A frame thickness of 0 with a radius gives you just the rounded screen corners
  against the taskbar (the *Crescent adaptive* theme, give or take a hairline).

## Credits

The taskbar-thread helpers (`RunFromWindowThreadTimeout`,
`FindCurrentProcessTaskbarWnd`) and the `taskbar.dll` XAML-root lookup are
ported from [Taskbar multi-row](https://windhawk.net/mods/taskbar-multirow)
by m417z, licensed under the GNU GPL v3.0. This mod is published under the
same license.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: slate
  $name: Theme
  $description: A ready-made theme sets everything - frame, fill, bevel, shadow, corners, the window corners, borders and shadows inside dwm.exe - and the settings further down are ignored, except the few under "Overrides". Custom uses every setting below as it is.
  $options:
  - slate: Slate - a dark solid frame in the active window's color, soft inner shadow, rounded windows with shadows (the default)
  - film: Colored film - a thin acrylic film in the active window's color, soft inner shadow, rounded windows with borders
  - ink: Frost ink - a thicker acrylic frame with a lit bevel along its edge, blurred and tinted with the window's color
  - fade: Crescents fade - the acrylic film along the taskbar's edge and the opposite one only, the sides open, rounded windows with borders
  - modern: Modern Windows - the taskbar's own light or dark acrylic with a fine rim, Windows' 8 px corners on every window, maximized ones bordered
  - rectangle: Rectangular frame - a square frame in the active window's color with a bright edge line, square windows with borders
  - crescent: Crescent adaptive - no frame to speak of, just the rounded screen corners in the active window's color with a hairline along them and the taskbar's edge, rounded windows with borders
  - custom: Custom - every setting below, as it is
- overrides:
  - offset: -1
    $name: Frame thickness (px)
    $description: How far the frame extends inwards from the three screen edges without the taskbar, measured from the inside of the one-pixel bezel line. -1 keeps the theme's.
  - radius: -1
    $name: Screen corner radius (px)
    $description: Radius of the rounded screen's corners. -1 keeps the theme's.
  - monitorCornerRadius: -1
    $name: Monitor corner radius (px)
    $description: Radius of the monitor's four outer corners; 0 leaves them square. -1 keeps the theme's.
  - customColor: ""
    $name: Frame color
    $description: "The frame's color instead of the theme's, in the same form as the Custom theme's frame color - #RRGGBB, `accent`, a gradient of several colors separated by commas, a light and a dark one separated by | - and the frame then shows it rather than the title bar's color (set the source below to Title bar to keep that and use this color only where there is no maximized window). Empty keeps the theme's."
  - gradientAngle: -1
    $name: Gradient angle (deg)
    $description: Direction of a gradient frame color - 0 runs left to right, 90 top to bottom. -1 keeps the theme's.
  - colorSource: theme
    $name: Frame color source
    $description: Whether the frame takes the active window's title bar color (and the taskbar its theme) or stays the frame color.
    $options:
    - theme: The theme's
    - titlebar: Title bar
    - custom: The frame color
  - fill: theme
    $name: Fill
    $description: Solid and Windows' default are complete on their own; Custom acrylic uses the "Custom acrylic values" group below (the theme's own acrylic values are set aside).
    $options:
    - theme: The theme's
    - solid: Solid
    - acrylic: Custom acrylic
    - system: Windows' default
  - workAreaInset: -1
    $name: Extra work-area inset (px)
    $description: A strip of wallpaper left between the frame and your windows, on all four sides. -1 keeps the theme's.
  - windowCornerRadius: -2
    $name: Window corner radius (px)
    $description: The radius of every window's corners, inside dwm.exe. -2 keeps the theme's; -1 is Windows' own; 0 draws windows square.
  $name: Overrides
  $description: The few values worth changing on a ready-made theme without going Custom. Each applies only when set; with the Custom theme they are ignored.
- general:
  - scaleWithDpi: true
    $name: Scale sizes with the monitor DPI
    $description: Treat offsets, radii and the blur as values at 100% scaling and scale them for each monitor.
  - monitors: all
    $name: Monitors
    $description: With the primary monitor only, the other monitors keep their own taskbar, and their windows keep Windows' corners, border and shadow.
    $options:
    - all: Every monitor
    - primary: Primary monitor only
  - batterySaverSolid: true
    $name: Solid while battery saver is on
    $description: Like Windows' own acrylic, the acrylic fills turn into the solid fill (the frame color) while battery saver is on, and come back when it is off.
  $name: General
  $description: Applies with every theme.
- frame:
  - offset: 16
    $name: Frame thickness (px)
    $description: How far the frame extends inwards from the three screen edges that don't carry the taskbar, measured from the inside of the one-pixel bezel line (0 leaves no frame along those edges, 1 one pixel of it). The taskbar edge gets no thickness, so the rounded screen sits flush against the taskbar.
  - edgeOffsetsClockwise: "-1,-1,-1,-1"
    $name: Per-edge thickness overrides
    $description: Four values in pixels - the taskbar's edge first, then the other three clockwise (with the taskbar at the bottom, for example, bottom,left,top,right; a value of 24 first leaves a 24 px band between the taskbar and the rounded screen). A value of 0 or more overrides the frame thickness of that edge; -1 keeps the default. Monitors without a taskbar use the frame thickness on all four edges.
  - radius: 28
    $name: Screen corner radius (px)
    $description: Radius of the four corners of the rounded screen, including the two where the frame meets the taskbar.
  - customColor: "#3C3C3C"
    $name: Frame color
    $description: "The frame's color - the solid fill, or the acrylic's tint, under the taskbar too. #RRGGBB for one color; several separated by commas for a gradient (#FF0000,#00FF00); a color for light mode and one for dark mode separated by | (#F3F3F3|#202020), each of which can be a gradient (light1,light2|dark1,dark2); `accent` anywhere for the Windows accent color, in a shade that keeps the taskbar's labels readable - darker on a dark taskbar (the shade Windows itself uses for an accent-colored taskbar), lighter on a light one. Empty uses the accent color. A gradient runs through the solid fill and the custom acrylic's tint and luminosity alike; Windows' default keeps its own colors."
  - gradientAngle: 0
    $name: Gradient angle (deg)
    $description: Direction of a gradient frame color - 0 runs left to right, 90 top to bottom, 45 from the top-left corner to the bottom-right one.
  - colorSource: titlebar
    $name: Frame color source
    $description: Where the frame color comes from. "Title bar" replaces the frame color above, on each monitor with a maximized window, with the color of that window's title bar (its fourth row of pixels, as on screen) - the solid fill, the acrylic's tint and luminosity, and the taskbar's background follow it, easing from one color to the next, and the taskbar switches to the light or the dark theme to stay readable on it. Ignored by the Windows' default fill.
    $options:
    - custom: The frame color above
    - titlebar: Title bar - the maximized window's title bar color, the frame color when there is none
  - opacity: 100
    $name: Opacity (%)
    $description: Solid fill only - opacity of the frame, under the taskbar too (the taskbar's background is transparent, so the two are one surface).
  - fill: solid
    $name: Fill
    $description: What the frame - and the taskbar's background with it - is filled with. Solid and Windows' default are complete on their own; Custom acrylic is the one to pick to tune the acrylic yourself, with the values under "Custom acrylic values" below.
    $options:
    - acrylic: Custom acrylic - a live blur of what is behind the frame and the taskbar, tinted with the frame color, tuned with the values below
    - solid: Solid - the frame color at the opacity above
    - system: Windows' default - the taskbar's own acrylic look, light or dark with the Windows mode
  - workAreaInset: 12
    $name: Extra work-area inset (px)
    $description: Shrinks the work area this many extra pixels inwards beyond the colored frame, on all four sides (next to the taskbar too), leaving a strip of wallpaper around maximized and snapped windows. 0 ends the work area exactly at the frame's inner edge.
  $name: Custom theme - frame
- customAcrylic:
  - blur: 0
    $name: Extra blur (px)
    $description: Blur added on top of what DWM already applies - the backdrop DWM hands to a window is blurred to begin with, about as much as Windows' own acrylic, and there is no sharper one to be had. 0 keeps just DWM's blur; 30 doubles up to the full acrylic softness.
  - tintOpacity: 0
    $name: Tint opacity (%)
    $description: How much of the frame color is laid over the blurred content. 0 shows the blur as it is, 100 hides it under the color.
  - luminosityOpacity: 50
    $name: Luminosity opacity (%)
    $description: How much the frame color's brightness replaces the brightness of the blurred content while its hues stay - what keeps Windows' acrylic readable over any wallpaper (it uses 85-96). 0 leaves the content's brightness alone.
  - saturation: 80
    $name: Saturation (%)
    $description: Saturation of the blurred content before it is tinted. 100 leaves it alone; Windows' acrylic boosts it to 125.
  - noiseOpacity: 0
    $name: Noise opacity (%)
    $description: Strength of the fine grain laid over the surface (Windows' acrylic uses 2). 0 turns it off.
  - noiseDensity: 0
    $name: Noise density (%)
    $description: Share of the pixels that carry a grain.
  $name: Custom theme - custom acrylic values
  $description: Used by the Custom acrylic fill only. Windows' default brings its own values; Solid has none.
- bevel:
  - enabled: false
    $name: Lit bevel
    $description: Lights the frame along its inner edge as a rounded bevel of acrylic glass - a specular highlight, a Fresnel rim and an edge line - so the frame reads as a thick pane of acrylic glass. Rasterized once with the frame; costs nothing while the screen changes.
  - width: 8
    $name: Bevel width (px)
  - curvature: 20
    $name: Curvature (x0.1)
    $description: Profile of the bevel - 10 is linear, 20 a rounder dome, 5 a sharp lip.
  - light: 70
    $name: Light (%)
    $description: Strength of the main light. It lights the inner edges that face it - from the top, the screen's bottom edge (and a bottom taskbar's top edge).
  - lightAngle: 90
    $name: Light angle (deg)
    $description: 0 lights from the right, 90 from the top, 135 from the top-left.
  - light2: 70
    $name: Second light (%)
    $description: Strength of a second light, for the edges the main one can't reach - from below, the screen's top edge (and a top taskbar's bottom edge). 100 lights both edges alike; 0 turns it off.
  - light2Angle: 270
    $name: Second light angle (deg)
  - specular: 50
    $name: Specular (%)
  - shininess: 55
    $name: Specular tightness
  - fresnel: 50
    $name: Fresnel rim (%)
  - edgeLine: 5
    $name: Edge line (%)
    $description: A thin line right at the inner edge; the percentage is its opacity.
  - edgeLineColor: "#FFFFFF"
    $name: Edge line color
    $description: Hex color (#RRGGBB) of the edge line. White reads as a highlight; a tint of the frame color as a rim.
  - edgeShade: 80
    $name: Edge shade (%)
    $description: Darkens the acrylic glass towards its inner edge.
  $name: Custom theme - acrylic glass bevel
- innerShadow:
  - enabled: true
    $name: Inner shadow
    $description: A soft shadow cast by the frame and the taskbar onto the screen content, along all four edges of the rounded screen.
  - blur: 12
    $name: Blur (px)
    $description: How far the shadow fades into the screen (up to 60). 0 turns it off.
  - opacity: 100
    $name: Opacity (%)
  - color: "#000000"
    $name: Color
    $description: Hex color (#RRGGBB).
  - offsetX: 0
    $name: Horizontal offset (px)
    $description: Shifts the shadow like a CSS inset box-shadow - a positive value makes it thicker on the left edge and thinner on the right, a negative value the other way round (-60 to 60).
  - offsetY: 0
    $name: Vertical offset (px)
    $description: Positive values make the shadow thicker on the top edge and thinner on the bottom; negative values the other way round (-60 to 60).
  $name: Custom theme - inner shadow
- monitorCorners:
  - radius: 20
    $name: Monitor corner radius (px)
    $description: Rounds the four outer corners of each monitor (over the taskbar as well) so the whole screen looks like a rounded panel. 0 leaves them square.
  $name: Custom theme - monitor corners
- windowCorners:
  - radius: 20
    $name: Window corner radius (px)
    $description: One corner radius, at 100% scaling, for every window Windows would round - normal, snapped and maximized, the last of which Windows draws square. -1 leaves the corners to Windows; 0 draws every window square. The options below work with either. This group runs inside dwm.exe, which Windhawk only injects into once dwm.exe is on the process inclusion list in its advanced settings (udwm.dll symbols are downloaded on first use).
  - maximizedShadow: false
    $name: Shadow on maximized windows
    $description: Windows drops a window's drop shadow while it is maximized. With this on, maximized (and snapped) windows keep the drop shadow floating windows have - the topmost one on each monitor, so the shadows of maximized windows stacked on one monitor don't pile up.
  - maximizedBorder: false
    $name: Border on maximized windows
    $description: Windows drops a window's one-pixel border while it is maximized. With this on, maximized (and snapped) windows keep the border floating windows have - which suits windows sitting inside the frame.
  - borderColors: false
    $name: Custom border colors
    $description: Color every window's one-pixel border yourself - floating, snapped and maximized windows alike (maximized ones need "Border on maximized windows") - with the two colors below instead of the gray Windows uses. Menus and tooltips are left alone.
  - activeBorderColor: accent
    $name: Active window border color
    $description: "#AARRGGBB (the first pair is opacity, 00 to FF), #RRGGBB for opaque, or `accent` for the Windows accent color."
  - inactiveBorderColor: "#3C3C3C"
    $name: Inactive window border color
    $description: Same format. Leave empty to use the active color for inactive windows too.
  $name: Custom theme - windows (DWM)
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <gdiplus.h>
#include <inspectable.h>
#include <d3d11.h>
#include <d2d1_1.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace wf = winrt::Windows::Foundation;
namespace wgd = winrt::Windows::Graphics::DirectX;
namespace wge = winrt::Windows::Graphics::Effects;
namespace wu = winrt::Windows::UI;
namespace ws = winrt::Windows::System;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuvm = winrt::Windows::UI::ViewManagement;
namespace wucd = winrt::Windows::UI::Composition::Desktop;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;

namespace {

enum Edge : int { kEdgeLeft = 0, kEdgeTop = 1, kEdgeRight = 2, kEdgeBottom = 3 };
constexpr int kEdgeNone = -1;  // monitor without a taskbar
// The theme forced on a taskbar for the title-bar color source.
enum WantedTheme { kThemeNone = 0, kThemeLight = 1, kThemeDark = 2 };
int DetectEdge(const RECT& taskbar, const RECT& monitor);  // defined with the layout code
enum Corner : int { kCornerTL = 0, kCornerTR = 1, kCornerBR = 2, kCornerBL = 3 };

constexpr WCHAR kFrameClassName[] = L"RoundEverything_Frame";
// The monitor corner ears and the bezel ring: black, like a panel's bezel.
constexpr COLORREF kMonitorCornerColor = RGB(0, 0, 0);
constexpr WCHAR kCtrlClassName[] = L"RoundEverything_Ctrl";
constexpr WCHAR kRefreshClassName[] = L"RoundEverything_Refresh";
constexpr WCHAR kAppBarClassName[] = L"RoundEverything_AppBar";
// The Windows accent color (0xFFRRGGBB), published by the explorer.exe part
// as a property of the taskbar window for the dwm.exe part: DWM runs as its
// own account (Window Manager\DWM-n), whose HKCU isn't the user's, so
// dwmapi's colorization color read there never follows the user's accent.
constexpr WCHAR kAccentProp[] = L"RoundEverything_Accent";
// Windows' "Show accent color on title bars and window borders" and the
// color DWM paints those borders with (HKCU\...\DWM ColorPrevalence and
// AccentColor - the user's, so read by the explorer.exe part): the color as
// 0x__RRGGBB, the top byte 0xFF while the option is on and 0x01 while it is
// off; absent when not known.
constexpr WCHAR kBorderAccentProp[] = L"RoundEverything_BorderAccent";
// Messages to the mod's control window (all handled on the mod's UI thread).
constexpr UINT kMsgReload = WM_APP + 1;            // settings changed (or anything else
                                                   // that wants a tick now)
constexpr UINT kMsgAppBarCallback = WM_APP + 2;    // ABN_* notifications to the appbar windows
constexpr UINT kMsgTaskbarEvent = WM_APP + 3;      // a taskbar / Task View window showed, hid or
                                                   // moved, or the foreground changed
constexpr UINT kMsgPassResult = WM_APP + 4;        // a XAML pass finished on the taskbar thread
constexpr UINT kMsgTaskbarReady = WM_APP + 5;      // a taskbar's tree is whole and rendered
constexpr UINT kMsgTaskbarMoving = WM_APP + 6;     // a taskbar window just moved/resized
constexpr UINT kMsgTaskbarRecolored = WM_APP + 7;  // the taskbar's background is painted again
constexpr UINT kMsgTaskbarZOrder = WM_APP + 8;     // a taskbar window changed its place in the z-order
// A recolor request to the taskbar thread is waited for at most this long
// before the frame shows anyway (a taskbar thread still busy after a layout
// change answers late).
constexpr ULONGLONG kRecolorWaitMaxMs = 1000;
// A frame waiting for its taskbar (to appear together with it) shows anyway
// after this long, whatever is still missing.
constexpr ULONGLONG kTaskbarWaitMaxMs = 2500;
// Tick rate while a frame waits for its taskbar, so it appears within a
// frame of the taskbar being uncloaked.
constexpr UINT kWaitTickIntervalMs = 16;
// A re-created taskbar window is cloaked (DWM) until its tree reports the
// ears, stroke and fill in place, at most this long.
constexpr ULONGLONG kVeilMaxMs = 1500;
// A request to the taskbar thread that got no answer in this long is
// considered lost (the window went away) and is sent again.
constexpr ULONGLONG kTaskbarCallLostMs = 2000;
// The regular tick is a fallback: taskbar, Task View and foreground changes
// arrive as events and tick right away.
constexpr UINT_PTR kTimerId = 1;
constexpr UINT kTickIntervalMs = 500;
// Routine corrections (a frame put back in its z-order place) are logged at
// most this often, so a log stays readable.
constexpr ULONGLONG kRoutineLogMs = 60000;
// An event that arrives while the previous tick is still fresh is coalesced
// into one tick a moment later, so a burst (an auto-hide slide) costs one
// tick and its last event is never dropped.
constexpr UINT_PTR kCoalesceTimerId = 2;
constexpr UINT kCoalesceDelayMs = 16;
// After a window event (an activation, a minimize, a move) the acrylic
// frames' backdrops are refreshed every frame for this long: DWM's own
// changes behind the frame (a shadow handed over or following a dragged
// window, a minimize animation) go on for up to a few hundred milliseconds
// after the event, and a slower pace showed them in steps (see
// RefreshBackdrops).
constexpr UINT_PTR kRefreshTimerId = 3;
constexpr UINT kRefreshIntervalMs = 16;
constexpr ULONGLONG kRefreshBurstMs = 1000;
// A frame color from a title bar eases from the old one to the new over
// this long (an ease-out: most of the way quickly, the rest
// gently), stepped by this timer for a layered frame and the taskbar's
// background; the acrylic frame's colors are animated by the compositor.
constexpr UINT_PTR kColorTimerId = 4;
constexpr UINT kColorStepMs = 33;
constexpr ULONGLONG kColorAnimMs = 350;
// How often the appbar thread re-reads auto-hide and the primary taskbar's
// edge from the shell when no taskbar event or ABN_* notification asked.
constexpr ULONGLONG kShellStateRefreshMs = 2000;
// A pass that answered "not found" (the taskbar's tree is being rebuilt) is
// sent again after this long.
constexpr ULONGLONG kPassRetryMs = 100;

// How often the slow, cross-thread maintenance passes re-check their work.
constexpr ULONGLONG kWorkAreaRetryMs = 1000;
constexpr ULONGLONG kTaskbarXamlRetryMs = 2000;  // stroke, fill and corner ears

struct Settings {
    int offset = 16;
    int edgeOffset[4] = {-1, -1, -1, -1};  // the taskbar's edge, then clockwise
    int radius = 28;
    bool monitorCorners = true;
    int monitorCornerRadius = 20;
    bool scaleWithDpi = true;
    COLORREF customColor = RGB(0x3C, 0x3C, 0x3C);  // the frame color (a gradient's average)
    std::vector<COLORREF> fillColors{RGB(0x3C, 0x3C, 0x3C)};  // the gradient's colors, or the one
    std::wstring colorText = L"#3C3C3C";  // the frame color as given (for the theme YAML)
    int gradientAngle = 0;                // degrees: 0 left to right, 90 top to bottom
    int colorSource = 0;         // colorSource: 0 custom, 1 the maximized window's title bar
    int opacity = 100;
    bool backdrop = false;       // fill: acrylic or system - the bands show the blurred backdrop
    bool systemLook = false;     // fill: system - follows the Windows mode
    int backdropBlur = 0;        // px at 100%
    int tintOpacity = 0;         // %
    int luminosityOpacity = 50;  // %
    int saturation = 80;         // %
    int noiseOpacity = 0;        // %
    int noiseDensity = 0;        // %
    bool batterySaverSolid = true;  // batterySaverSolid
    bool bevel = false;          // bevel.*
    int bevelWidth = 8;          // px at 100%
    int bevelCurvature = 20;     // x0.1
    int light = 70;              // %
    int lightAngle = 90;         // degrees
    int light2 = 70;             // %
    int light2Angle = 270;       // degrees
    int specular = 50;           // %
    int shininess = 55;
    int fresnel = 50;            // %
    int edgeLine = 5;            // %
    COLORREF edgeLineColor = RGB(255, 255, 255);
    int edgeShade = 80;          // %
    bool shadow = true;
    int shadowBlur = 12;
    int shadowOpacity = 100;
    COLORREF shadowColor = RGB(0, 0, 0);
    int shadowOffsetX = 0;
    int shadowOffsetY = 0;
    int workAreaInset = 12;
    bool primaryOnly = false;
};

// The frame is one always-on-top window per monitor: a composition window
// that lies under the taskbar as well, whose background is then transparent,
// filled with the blurred backdrop or the frame color (see the Acrylic
// section) - or, where composition can't be set up, a layered window along
// the three edges next to the taskbar, with the taskbar itself, its
// background painted in the frame color, as the fourth side. Frame and
// taskbar are shown and hidden together: the frame
// waits for the taskbar's tree to be whole and rendered (and the taskbar to
// be uncloaked by the shell) before it appears, and goes away the moment
// the taskbar starts moving or is cloaked. A monitor without a taskbar gets
// the frame on all four edges.
struct Layout {
    HMONITOR monitor = nullptr;
    HWND taskbar = nullptr;  // null on a monitor without a (live) taskbar window
    RECT monitorRect{};
    RECT taskbarRect{};   // the taskbar strip, where the taskbar sits when shown (empty when
                          // there is none)
    RECT baseWorkArea{};  // monitor minus the taskbar strip
    RECT overlayRect{};   // the frame window
    bool autoHide = false;  // the taskbar slides in and out over the strip
    RECT innerRect{};     // the rounded "screen" rectangle, screen coordinates
    int edge = kEdgeNone;
    UINT dpi = 96;
    int offset[4] = {0, 0, 0, 0};  // left, top, right, bottom (scaled px)
    int radius = 0;                // rounded-screen corner radius (scaled px)
    int workAreaInset = 0;         // extra work-area shrink beyond the frame (scaled px)
    int monitorCornerRadius = 0;   // outer monitor corner radius (scaled px), 0 = off
    int shadowBlur = 0;            // inner shadow blur (scaled px), 0 = off
    int shadowOffsetX = 0;         // inner shadow offset (scaled px)
    int shadowOffsetY = 0;
    float backdropBlur = 0.0f;     // acrylic blur (scaled px)
    int bevelWidth = 0;            // lit bevel width (scaled px), 0 = off
    bool composition = false;      // the frame is a composition window (one pixel in from the monitor)
    std::wstring signature;        // this monitor's part of the layout signature
};

// One invisible appbar reserving a strip of the work area on one edge.
struct AppBar {
    HWND hwnd = nullptr;
    UINT edge = ABE_BOTTOM;
    int thickness = 0;
    bool anchored = false;  // kept at the monitor's edge, over the taskbar (see SetAppBarPosition)
};

// Rasterized shape of one frame window, built once per layout. A repaint
// only recolors it, so a color change is cheap.
struct FrameSurface {
    int width = 0;
    int height = 0;
    HDC memDc = nullptr;
    HBITMAP dib = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    DWORD* pixels = nullptr;        // the DIB bits, premultiplied BGRA, top-down
    std::vector<BYTE> coverage;     // frame anti-aliased coverage per pixel
    std::vector<DWORD> staticLayer; // color-independent pixels (monitor corner ears,
                                    // hit-test strip), premultiplied; 0 = none

    FrameSurface() = default;
    FrameSurface(const FrameSurface&) = delete;
    FrameSurface& operator=(const FrameSurface&) = delete;
    ~FrameSurface() {
        if (memDc) {
            SelectObject(memDc, oldBitmap);
            DeleteDC(memDc);
        }
        if (dib) {
            DeleteObject(dib);
        }
    }
};

// The composition side of a composition frame (see the Acrylic section):
// the frame as a composition window, with the bands showing the blurred
// backdrop (the acrylic fills) or the frame color (the solid fill).
struct CompositionFrame {
    HWND owner = nullptr;  // hidden; an owned window may be topmost while (almost) covering a monitor
    HWND strips[4] = {};   // the one-pixel bezel ring the frame leaves uncovered
    RECT ringRect{};       // the monitor rectangle the ring was made for
    wucd::DesktopWindowTarget target{nullptr};
    wuc::ContainerVisual root{nullptr};
    wuc::CompositionEffectBrush effectBrush{nullptr};    // backdrop -> blur -> ... -> tint
    wuc::CompositionDrawingSurface maskSurface{nullptr};
    wuc::CompositionDrawingSurface overlaySurface{nullptr};  // ears and noise
    bool built = false;  // surfaces and sprites are in place for the current layout
    // The effect factories behind the brushes: a graph loads asynchronously,
    // so whether the engine accepted it is only known later (CheckEffectGraphs).
    std::vector<wuc::CompositionEffectFactory> factories;
    bool graphsChecked = false;
    int graphTicksPending = 0;  // ticks spent waiting for the factories to load
    // DWM's host backdrop, one per frame window (the backdrop fills).
    wuc::CompositionBackdropBrush hostBackdrop{nullptr};
    // The color the frame shows instead (the solid fill), animatable - or,
    // for a gradient frame color, a linear gradient whose stops are.
    wuc::CompositionColorBrush colorBrush{nullptr};
    wuc::CompositionLinearGradientBrush gradientBrush{nullptr};
    // Every gradient stop the frame shows - the solid fill's, or the acrylic
    // bands' tint and luminosity inputs - with the frame color it stands for
    // and its alpha, so a color change animates them all.
    struct GradientStop {
        wuc::CompositionColorGradientStop stop{nullptr};
        size_t index = 0;   // into the frame color's colors
        uint8_t alpha = 255;
    };
    std::vector<GradientStop> gradientStops;
    // The acrylic graph taking the gradient as inputs: one brush per band is
    // made from it, each fed gradients in that band's coordinates.
    wuc::CompositionEffectFactory gradientFactory{nullptr};
    // The band sprites' input rectangles (window coordinates), and under
    // each, at the bottom of the z-order, the window whose repaint has DWM
    // read the backdrop there again (see RefreshBackdrops).
    std::vector<RECT> backdropRects;
    HWND refreshers[4] = {};
    // The effect graph's color properties the compositor can animate
    // ("LuminosityColor.Color", "TintColor.Color"), when the full graph is
    // in use.
    std::vector<winrt::hstring> colorProperties;
};

// One slice of the inner shadow: a click-through layered window along one
// edge of the rounded screen, as wide as the band where the shadow is
// non-zero, with that band's pixels.
struct ShadowSlice {
    RECT rect{};  // screen coordinates; empty: no slice
    HWND hwnd = nullptr;
    std::unique_ptr<FrameSurface> surface;
};
constexpr int kShadowSlices = 4;

struct Overlay {
    Layout layout;
    HWND hwnd = nullptr;
    // Set for an acrylic frame (either fill): hwnd is then a composition
    // window (owned, one pixel short of the monitor, under the taskbar) and
    // the shadow windows sit above it rather than below. Null for the
    // layered frame of a machine without composition.
    std::unique_ptr<CompositionFrame> comp;
    std::unique_ptr<FrameSurface> surface;
    // The inner shadow: separate, click-through layered windows along the
    // edges of the rounded screen (never one covering it: a layered window
    // over the whole screen keeps DWM from presenting the windows under it
    // directly), kept next to the frame in z-order and shown, hidden and
    // revealed with it. Separate because a layered window's painted pixels
    // catch the mouse, and the shadow lies over the screen content.
    ShadowSlice shadow[kShadowSlices];
    bool shadowBuilt = false;  // the slices' surfaces are built for the current layout
    bool shown = false;
    // Waiting for the taskbar: the frame stays off the screen from the
    // moment its taskbar changes (moves, is cloaked, is new) until the
    // taskbar's tree has reported whole-and-rendered after that moment and
    // the shell has uncloaked it - so both appear in the same frame.
    bool waiting = false;
    ULONGLONG waitSince = 0;
    ULONGLONG recolorRequestTick = 0;  // when the recolor was asked for (0: not yet)
    UINT recolorSerial = 0;            // which request (its answer is waited for)
    // The frame window is visible at alpha 0 and registered in g_reveals: the
    // taskbar thread presents it together with the recolor (see BeginReveal).
    bool revealPending = false;
    // Task View: frame hidden and taskbar background transparent while it
    // is open; both come back together when it closes.
    bool taskViewHidden = false;
    ULONGLONG taskViewRecolorTick = 0;
    UINT taskViewRecolorSerial = 0;
    bool fullscreen = false;
    HWND fullscreenWnd = nullptr;  // the window the frame is hidden for
    int fullscreenTicks = 0;  // consecutive ticks the monitor looked full-screen
    bool painted = false;
    COLORREF paintedColor = 0;  // what the frame was last painted with
    // The color as shown right now, the one it is heading for, and the
    // animation between them (see AnimateFrameColor).
    COLORREF displayColor = 0;
    COLORREF colorTarget = 0;
    bool colorAnimating = false;
    COLORREF animFrom = 0;
    COLORREF animTo = 0;
    ULONGLONG animStart = 0;
    // Presentation alpha (UpdateLayeredWindow's constant alpha; cloaked or
    // not for an acrylic frame): 0 takes the frame off the screen before
    // its window is hidden or destroyed.
    BYTE presentAlpha = 255;
    // The taskbar background last asked for on this monitor (a change is
    // applied at once).
    bool fillAsked = false;
    DWORD lastFillArgb = 0;
    bool themeAsked = false;
    int themeWanted = kThemeNone;  // the taskbar theme asked for, for the hysteresis
};

// The appbars reserving the work area on one monitor.
struct AppBarSet {
    HMONITOR monitor = nullptr;
    RECT baseWorkArea{};
    int thickness[4] = {0, 0, 0, 0};  // left, top, right, bottom
    int taskbarEdge = kEdgeNone;
    std::vector<AppBar> bars;
    RECT workAreaAfterApply{};  // rcWork observed right after the last ABM_SETPOS pass
};

// What the UI thread wants reserved on one monitor (published every tick).
struct AppBarWish {
    HMONITOR monitor = nullptr;
    RECT baseWorkArea{};
    int thickness[4] = {0, 0, 0, 0};
    int taskbarEdge = kEdgeNone;  // the edge whose appbar is anchored over the taskbar
};

// What the appbar thread has in place on one monitor.
struct AppBarStatus {
    HMONITOR monitor = nullptr;
    RECT baseWorkArea{};
};

// What the appbar thread last learned from the shell.
struct ShellState {
    bool autoHide = false;
    int primaryEdge = -1;  // ABE_* of the primary taskbar, -1 unknown
    RECT primaryRect{};    // the taskbar rect that edge was reported for
};

// One taskbar-side pass (stroke, ears, fill) on one taskbar.
struct PassState {
    bool done = false;          // last answer: applied
    bool outstanding = false;   // a request is on the taskbar thread
    ULONGLONG sentTick = 0;
    ULONGLONG answerTick = 0;
    ULONGLONG doneTick = 0;     // when it was last confirmed applied
    UINT serial = 0;
};
enum XamlPass { kPassStroke = 0, kPassCorners = 1, kPassFill = 2, kPassTheme = 3, kPassCount = 4 };

enum class HostProcess { Explorer, Dwm };

// The ready-made themes: each sets the frame, the fill, the bevel, the inner
// shadow, the monitor corners and the dwm.exe options outright. The same
// table serves both processes. The "custom" theme leaves every setting as
// it is.
struct Bevel {
    bool enabled = false;
    int width = 8, curvature = 20;
    int light = 70, lightAngle = 90, light2 = 70, light2Angle = 270;
    int specular = 50, shininess = 55, fresnel = 50;
    int edgeLine = 5, edgeShade = 80;  // the edge line white
};
struct ThemeDef {
    PCWSTR id;
    int offset;
    int edgeOffset[4] = {-1, -1, -1, -1};  // the taskbar's edge, then clockwise; -1 the offset
    int radius;
    COLORREF color;
    int colorSource;  // 0 the frame color, 1 the title bar
    int fill;         // 0 solid, 1 custom acrylic, 2 Windows' default
    int blur, tint, luminosity, saturation, noiseOpacity, noiseDensity;
    Bevel bevel;
    bool shadow;
    int shadowBlur;
    int monitorCornerRadius;  // 0: square monitor corners
    int workAreaInset;
    int windowRadius;  // -1 Windows' own corners, 0 square
    bool maximizedShadow, maximizedBorder;
};
// A bevel reduced to its rim: no lighting, the full Fresnel edge, an edge
// shade - the minimum width and curvature the settings allow.
constexpr Bevel kRimBevel{.enabled = true, .width = 1, .curvature = 1, .light = 0, .lightAngle = 0,
                          .light2 = 0, .light2Angle = 0, .specular = 0, .shininess = 1,
                          .fresnel = 100, .edgeLine = 0, .edgeShade = 80};
constexpr ThemeDef kThemes[] = {
    {.id = L"slate", .offset = 16, .radius = 28, .color = RGB(0x3C, 0x3C, 0x3C), .colorSource = 1,
     .fill = 0, .blur = 0, .tint = 0, .luminosity = 50, .saturation = 80, .noiseOpacity = 0,
     .noiseDensity = 0, .bevel = {}, .shadow = true, .shadowBlur = 12, .monitorCornerRadius = 20,
     .workAreaInset = 12, .windowRadius = 20, .maximizedShadow = true,
     .maximizedBorder = false},
    {.id = L"film", .offset = 16, .radius = 28, .color = RGB(0x3C, 0x3C, 0x3C), .colorSource = 1,
     .fill = 1, .blur = 0, .tint = 0, .luminosity = 50, .saturation = 80, .noiseOpacity = 0,
     .noiseDensity = 0, .bevel = {}, .shadow = true, .shadowBlur = 12, .monitorCornerRadius = 20,
     .workAreaInset = 12, .windowRadius = 20, .maximizedShadow = false,
     .maximizedBorder = true},
    {.id = L"ink", .offset = 20, .radius = 32, .color = RGB(0x3C, 0x3C, 0x3C), .colorSource = 1,
     .fill = 1, .blur = 12, .tint = 20, .luminosity = 40, .saturation = 110, .noiseOpacity = 2,
     .noiseDensity = 100, .bevel = {.enabled = true}, .shadow = true, .shadowBlur = 16,
     .monitorCornerRadius = 24, .workAreaInset = 12, .windowRadius = 24,
     .maximizedShadow = true, .maximizedBorder = false},
    // Only the taskbar's edge and the one opposite carry the frame; the two
    // between them sit at the screen edge.
    {.id = L"fade", .offset = 16, .edgeOffset = {-1, 0, -1, 0}, .radius = 20,
     .color = RGB(0x3C, 0x3C, 0x3C), .colorSource = 1, .fill = 1, .blur = 0, .tint = 0,
     .luminosity = 50, .saturation = 80, .noiseOpacity = 0, .noiseDensity = 0, .bevel = {},
     .shadow = false, .shadowBlur = 12, .monitorCornerRadius = 16,
     .workAreaInset = 8, .windowRadius = 16, .maximizedShadow = false, .maximizedBorder = true},
    {.id = L"modern", .offset = 16, .radius = 10, .color = RGB(0x3C, 0x3C, 0x3C), .colorSource = 1,
     .fill = 2, .blur = 0, .tint = 0, .luminosity = 50, .saturation = 80, .noiseOpacity = 0,
     .noiseDensity = 0, .bevel = kRimBevel, .shadow = false, .shadowBlur = 12,
     .monitorCornerRadius = 0, .workAreaInset = 12, .windowRadius = 8,
     .maximizedShadow = false, .maximizedBorder = true},
    {.id = L"rectangle", .offset = 16, .radius = 0, .color = RGB(0x3C, 0x3C, 0x3C),
     .colorSource = 1, .fill = 0, .blur = 0, .tint = 0, .luminosity = 50, .saturation = 80,
     .noiseOpacity = 0, .noiseDensity = 0,
     .bevel = {.enabled = true, .width = 1, .curvature = 1, .light = 0, .lightAngle = 0,
               .light2 = 0, .light2Angle = 0, .specular = 0, .shininess = 1, .fresnel = 100,
               .edgeLine = 50, .edgeShade = 100},
     .shadow = false, .shadowBlur = 12, .monitorCornerRadius = 0,
     .workAreaInset = 0, .windowRadius = 0, .maximizedShadow = false, .maximizedBorder = true},
    // A one-pixel frame inside the bezel ring: the edge line runs along it,
    // around the rounded screen corners and along the taskbar's edge.
    {.id = L"crescent", .offset = 1, .radius = 20, .color = RGB(0x3C, 0x3C, 0x3C), .colorSource = 1,
     .fill = 0, .blur = 0, .tint = 0, .luminosity = 50, .saturation = 80, .noiseOpacity = 0,
     .noiseDensity = 0,
     .bevel = {.enabled = true, .width = 1, .curvature = 1, .light = 0, .lightAngle = 0,
               .light2 = 0, .light2Angle = 0, .specular = 0, .shininess = 1, .fresnel = 0,
               .edgeLine = 50, .edgeShade = 0},
     .shadow = false, .shadowBlur = 12, .monitorCornerRadius = 20,
     .workAreaInset = 0, .windowRadius = 20, .maximizedShadow = false, .maximizedBorder = true},
};
const ThemeDef* FindTheme(PCWSTR id) {
    for (const ThemeDef& theme : kThemes) {
        if (wcscmp(id, theme.id) == 0) {
            return &theme;
        }
    }
    return nullptr;  // "custom", or anything else
}

Settings g_settings;
// The color a monitor's frame is painted with: the frame color, or the
// maximized window's title bar's (defined with the color source code).
COLORREF FrameColorFor(const Overlay& overlay);
void Tick(bool forceRebuild);
HostProcess g_host = HostProcess::Explorer;
HMODULE g_module = nullptr;
HANDLE g_thread = nullptr;
HANDLE g_stopEvent = nullptr;

// Shell calls (SHAppBarMessage) are synchronous messages to the taskbar
// window. A taskbar that is relaying out (a layout change keeps its thread
// busy for a second or more) blocks such a call until it is responsive
// again - which is exactly when the frame must not be stalled. The UI
// thread therefore never makes them: it publishes wishes, the appbar thread
// reconciles them and reports back what is in place.
HANDLE g_appBarThread = nullptr;
HANDLE g_appBarWake = nullptr;         // UI thread -> appbar thread: wishes changed
HANDLE g_appBarStop = nullptr;         // UI thread -> appbar thread: remove everything and exit
HANDLE g_taskbarRestored = nullptr;    // taskbar thread -> UI thread: hand-back is on screen
HANDLE g_shellStateReady = nullptr;    // first shell state fetched
std::mutex g_shellMutex;               // guards the four below
std::vector<AppBarWish> g_appBarWishes;
std::vector<AppBarStatus> g_appBarStatus;
ShellState g_shellState;
std::atomic<bool> g_appBarsDirty{false};    // ABN_POSCHANGED: re-query positions
std::atomic<bool> g_shellStateDirty{true};  // re-read auto-hide / primary edge
std::atomic<HWND> g_ctrlWnd{nullptr};
std::atomic<bool> g_reloadRequested{false};
std::atomic<bool> g_unloading{false};
bool g_taskbarXamlAvailable = false;  // taskbar.dll symbols resolved

// State owned by the mod's UI thread. The frames (and, with the acrylic
// fill, their composition objects) are released by that thread at unload;
// not destroyed with the image, so that an explorer exit without an unload
// never releases thread-affine composition objects from the exiting thread.
[[clang::no_destroy]] std::optional<std::vector<Overlay>> g_overlays{std::in_place};
std::unordered_map<HWND, PassState> g_passState[kPassCount];
UINT g_requestSerial = 0;  // tags requests to the taskbar thread; stale answers are dropped
// The last usable taskbar seen on each monitor, to tell a window that moved
// or resized from one that was re-created (which gets the DWM veil below).
struct SeenTaskbar {
    HWND hwnd = nullptr;
    RECT rect{};
    int edge = kEdgeNone;
};
std::unordered_map<HMONITOR, SeenTaskbar> g_seenTaskbars;

// A taskbar window that was just re-created is cloaked (DWM) until its tree
// reports everything in place for its edge (or kVeilMaxMs pass), so it never
// shows with square corners. A window that merely moved is veiled from inside
// its own thread instead (its content at opacity 0, see TaskbarWants).
struct Veil {
    ULONGLONG since = 0;
    int edge = kEdgeNone;
};
std::unordered_map<HWND, Veil> g_veils;
// When each taskbar last reported its tree whole and rendered, and for which
// edge; and when it last reported its background painted again (UI thread).
struct TaskbarReady {
    ULONGLONG tick = 0;
    int edge = kEdgeNone;
};
std::unordered_map<HWND, TaskbarReady> g_taskbarReady;
// Recolor requests are numbered, so the answer to an earlier request (a
// Task View closing right before a layout change, say) can't pass for the
// answer to a later one. The last serial each taskbar reported rendered:
std::unordered_map<HWND, UINT> g_taskbarRecolored;
UINT g_recolorSerial = 0;

// A frame window handed to the taskbar thread to be revealed in the same
// rendering pass as the taskbar's color (the handshake that puts both on the
// screen in one composition). Keyed by taskbar. The UI thread registers and
// withdraws entries and holds the lock whenever it touches a frame's bitmap;
// the taskbar thread holds it while presenting from that bitmap.
struct Reveal {
    UINT serial = 0;  // the recolor request it belongs to
    HWND frame = nullptr;  // already placed and sized by the UI thread
    HDC memDc = nullptr;
    int width = 0;
    int height = 0;
    // The shadow slices, if any (same treatment).
    struct {
        HWND hwnd = nullptr;
        HDC memDc = nullptr;
        int width = 0;
        int height = 0;
    } shadow[kShadowSlices];
    // An acrylic frame is revealed by uncloaking it (and its bezel ring)
    // instead of a layered-window update.
    bool composition = false;
    HWND strips[4] = {};
};
std::mutex g_revealMutex;
std::unordered_map<HWND, Reveal> g_reveals;
bool g_fastTick = false;  // the tick timer runs at kWaitTickIntervalMs

// A taskbar window the shell has cloaked stays part of the layout (at its
// current rect, so the frame follows a move at once) for this long; one that
// stays cloaked longer has been taken away (taskbar on another display
// turned off) and its monitor gets the four-sided frame.
constexpr ULONGLONG kCloakGraceMs = 1500;
std::unordered_map<HWND, ULONGLONG> g_cloakedSince;
std::wstring g_layoutSignature;
bool g_inTick = false;
bool g_taskbarEventPending = false;  // a kMsgTaskbarEvent is queued
// Set by the events that can change which window's title bar the frame
// shows, or what it shows - an activation, a maximize or restore, a
// minimize starting or ending, a move/resize ending, Aero Peek - and taken
// by the kMsgTaskbarEvent handler to start the fast readings. A window being
// dragged or a taskbar sliding only ticks.
std::atomic<bool> g_titleEventPending{false};
// When the last such event happened: a window zooming into place, or out of
// it, shows on screen as DWM's animation for a moment, and the row read off
// the screen then shows what is behind it (see ReadTitleBar).
std::atomic<ULONGLONG> g_titleEventTick{0};
ULONGLONG g_lastTickTime = 0;        // when the last tick ran (throttles event-driven ticks)
ULONGLONG g_threadStartTick = 0;
bool g_xamlCornersOwned = false;  // corner ears were added to at least one taskbar

// Composition, owned by the mod's UI thread; null when it couldn't be set
// up, which leaves the layered frames. Released by that thread at unload,
// never by the image's destructors (see g_overlays).
[[clang::no_destroy]] wuc::Compositor g_compositor{nullptr};
[[clang::no_destroy]] wuc::CompositionGraphicsDevice g_graphicsDevice{nullptr};
[[clang::no_destroy]] wuvm::UISettings g_uiSettings{nullptr};
std::atomic<bool> g_overlaysComposition{false};  // the frames in g_overlays are composition windows
std::atomic<bool> g_overlaysBackdrop{false};     // their bands show the backdrop (else the color)
std::atomic<bool> g_compositionUnavailable{false};  // building a composition frame failed: layered frames
std::atomic<bool> g_backdropUnavailable{false};     // the effect graph was rejected: the color fill
std::atomic<bool> g_advancedEffects{true};  // the system's transparency effects are on

// One frame, one piece: whenever composition is available, the frame on a
// monitor is a composition window spanning the monitor, under the taskbar
// (whose background is made transparent), with either fill - the bands
// show the blurred backdrop, or the frame color. The layered frame that
// stops at the taskbar, with the taskbar painted in the color, is the
// fallback for a machine where composition can't be set up.
bool UsesComposition() {
    return g_compositor && g_graphicsDevice && !g_compositionUnavailable;
}
// Like Windows' own acrylic, the bands show the color rather than the
// backdrop while transparency effects are off or (unless the option says
// otherwise) battery saver is on.
std::atomic<bool> g_batterySaver{false};
bool UsesBackdrop() {
    return UsesComposition() && g_settings.backdrop && !g_backdropUnavailable && g_advancedEffects &&
           !(g_batterySaver && g_settings.batterySaverSolid);
}

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

HostProcess DetectHostProcess() {
    WCHAR path[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (len == 0 || len >= ARRAYSIZE(path)) {
        return HostProcess::Explorer;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    return _wcsicmp(name, L"dwm.exe") == 0 ? HostProcess::Dwm : HostProcess::Explorer;
}

HMODULE GetCurrentModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetCurrentModule), &module);
    return module;
}

UINT GetWindowDpi(HWND hwnd) {
    UINT dpi = GetDpiForWindow(hwnd);
    return dpi ? dpi : 96;
}

UINT GetMonitorDpi(HMONITOR monitor) {
    UINT dpiX = 0, dpiY = 0;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX) {
        return dpiX;
    }
    return 96;
}

int ScaleForDpi(int value, UINT dpi) {
    if (!g_settings.scaleWithDpi || dpi == 96) {
        return value;
    }
    return MulDiv(value, static_cast<int>(dpi), 96);
}

void SetThreadPerMonitorDpiAware() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

void CloakWindow(HWND hwnd, bool cloak) {
    BOOL value = cloak ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &value, sizeof(value));
}

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    return SUCCEEDED(hr) && cloaked != 0;
}

bool RectContains(const RECT& outer, const RECT& inner) {
    return outer.left <= inner.left && outer.top <= inner.top &&
           outer.right >= inner.right && outer.bottom >= inner.bottom;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

COLORREF ParseHexColor(PCWSTR text, COLORREF fallback) {
    std::wstring s(text);
    size_t begin = s.find_first_not_of(L" \t\r\n");
    size_t end = s.find_last_not_of(L" \t\r\n");
    if (begin == std::wstring::npos) {
        return fallback;
    }
    s = s.substr(begin, end - begin + 1);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() != 6) {
        return fallback;
    }

    wchar_t* parseEnd = nullptr;
    unsigned long value = wcstoul(s.c_str(), &parseEnd, 16);
    if (!parseEnd || *parseEnd != L'\0') {
        return fallback;
    }

    return RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
}

// Four comma-separated values (the taskbar's edge, then clockwise). Missing
// or unparsable entries fall back to -1 (default behavior for that edge).
void ParseEdgeOffsets(PCWSTR text, int out[4]) {
    for (int i = 0; i < 4; i++) {
        out[i] = -1;
    }

    std::wstring_view rest(text);
    for (int i = 0; i < 4 && !rest.empty(); i++) {
        size_t comma = rest.find(L',');
        std::wstring_view token = rest.substr(0, comma);
        rest = comma == std::wstring_view::npos ? std::wstring_view() : rest.substr(comma + 1);

        size_t begin = token.find_first_not_of(L" \t");
        if (begin == std::wstring_view::npos) {
            continue;
        }
        token = token.substr(begin);

        std::wstring number(token);
        wchar_t* end = nullptr;
        long value = wcstol(number.c_str(), &end, 10);
        if (end == number.c_str()) {
            continue;  // not a number: keep -1
        }
        out[i] = value < 0 ? -1 : static_cast<int>(std::min<long>(value, 10000));
    }
}

// The Windows mode (Settings > Personalization > Colors > "Choose your
// default Windows mode"), which the taskbar's own colors follow.
bool SystemUsesLightTheme() {
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value != 0;
}
// The plain Windows accent color for the dwm.exe part (see kAccentProp), set
// on the primary taskbar window at the regular tick rate - the taskbar may
// have been re-created - and removed when the mod unloads. The fast ticks
// while a frame waits for its taskbar skip it.
void PublishAccentColor(ULONGLONG now) {
    static ULONGLONG lastPublished = 0;
    if (lastPublished && now - lastPublished < kTickIntervalMs - 50) {
        return;
    }
    lastPublished = now;
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        return;
    }
    uint32_t argb = 0;
    bool known = false;
    if (g_uiSettings) {
        try {
            wu::Color c = g_uiSettings.GetColorValue(wuvm::UIColorType::Accent);
            argb = 0xFF000000u | (static_cast<uint32_t>(c.R) << 16) |
                   (static_cast<uint32_t>(c.G) << 8) | c.B;
            known = true;
        } catch (winrt::hresult_error const&) {
        }
    }
    if (!known) {
        DWORD color = 0;
        BOOL opaque = FALSE;
        if (FAILED(DwmGetColorizationColor(&color, &opaque))) {
            return;
        }
        argb = 0xFF000000u | (color & 0x00FFFFFF);  // the user's own, read as the user
    }
    if (reinterpret_cast<ULONG_PTR>(GetPropW(taskbar, kAccentProp)) != argb) {
        SetPropW(taskbar, kAccentProp, reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(argb)));
    }

    DWORD prevalence = 0, accent = 0, size = sizeof(DWORD);
    ULONG_PTR border = 0;  // not known
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"AccentColor",
                     RRF_RT_REG_DWORD, nullptr, &accent, &size) == ERROR_SUCCESS) {
        size = sizeof(DWORD);
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM", L"ColorPrevalence",
                     RRF_RT_REG_DWORD, nullptr, &prevalence, &size);
        // AccentColor is 0xAABBGGRR.
        border = (prevalence ? 0xFF000000u : 0x01000000u) | ((accent & 0xFF) << 16) |
                 (accent & 0xFF00) | ((accent >> 16) & 0xFF);
    }
    if (reinterpret_cast<ULONG_PTR>(GetPropW(taskbar, kBorderAccentProp)) != border) {
        if (border) {
            SetPropW(taskbar, kBorderAccentProp, reinterpret_cast<HANDLE>(border));
        } else {
            RemovePropW(taskbar, kBorderAccentProp);
        }
    }
}

// The Windows accent color in a shade that suits the taskbar's own labels,
// which stay white on a dark taskbar and black on a light one. WinUI's
// accent fills (AccentFillColorDefaultBrush: SystemAccentColorLight2 in dark
// mode, SystemAccentColorDark1 in light mode) carry the *opposite* text
// color (TextOnAccentFillColorPrimary: black on Light2, white on Dark1), so
// they don't fit here. A dark taskbar gets SystemAccentColorDark2 - the
// shade the shell itself uses for the taskbar when the accent is shown on
// it (AccentPalette: Dark1 Start, Dark2 taskbar, Dark3 the taskbar under
// transparency) - and a light one SystemAccentColorLight2, the shade WinUI
// pairs with black text (the shell never colors a light taskbar). Before
// composition is set up (no UISettings yet), the accent as dwmapi reports it.
COLORREF CurrentAccentColor() {
    if (g_uiSettings) {
        try {
            wu::Color c = g_uiSettings.GetColorValue(SystemUsesLightTheme()
                                                         ? wuvm::UIColorType::AccentLight2
                                                         : wuvm::UIColorType::AccentDark2);
            return RGB(c.R, c.G, c.B);
        } catch (winrt::hresult_error const&) {
        }
    }
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (FAILED(DwmGetColorizationColor(&color, &opaque))) {
        return RGB(0x3C, 0x3C, 0x3C);
    }
    return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
}

// What the frame color as given depends on, so the regular tick can tell
// when it must be read again: the Windows mode (a light|dark pair, or the
// accent's shade) and the accent color (`accent`, or the empty frame color).
struct ColorDependencies {
    bool mode = false;
    bool accent = false;
};

// A frame color as given: "#RRGGBB" or "accent", several of them separated
// by commas for a gradient, and a light and a dark one separated by "|"
// (each may be a gradient). Empty is the accent color. The colors for the
// current Windows mode go into `out`; false (and `out` untouched) when none
// of them could be read.
bool ParseFillColors(PCWSTR text, std::vector<COLORREF>& out, ColorDependencies* deps) {
    std::wstring spec(text ? text : L"");
    auto trim = [](std::wstring t) {
        size_t b = t.find_first_not_of(L" \t\r\n'\"");
        size_t e = t.find_last_not_of(L" \t\r\n'\"");
        return b == std::wstring::npos ? std::wstring() : t.substr(b, e - b + 1);
    };
    spec = trim(spec);
    std::vector<COLORREF> colors;
    bool usesMode = false, usesAccent = false;
    if (spec.empty()) {
        colors.push_back(CurrentAccentColor());
        usesAccent = true;
    } else {
        size_t bar = spec.find(L'|');
        if (bar != std::wstring::npos) {
            usesMode = true;
            spec = SystemUsesLightTheme() ? spec.substr(0, bar) : spec.substr(bar + 1);
        }
        for (size_t at = 0; at <= spec.size();) {
            size_t comma = spec.find(L',', at);
            std::wstring token = trim(spec.substr(at, comma == std::wstring::npos
                                                          ? std::wstring::npos
                                                          : comma - at));
            if (_wcsicmp(token.c_str(), L"accent") == 0) {
                colors.push_back(CurrentAccentColor());
                usesAccent = true;
            } else if (!token.empty()) {
                COLORREF c = ParseHexColor(token.c_str(), 0xFFFFFFFF);
                if (c != 0xFFFFFFFF) {
                    colors.push_back(c);
                }
            }
            if (comma == std::wstring::npos) {
                break;
            }
            at = comma + 1;
        }
    }
    if (colors.empty()) {
        return false;
    }
    out = std::move(colors);
    if (deps) {
        // The accent's shade follows the mode, so an accent depends on both.
        deps->mode = deps->mode || usesMode || usesAccent;
        deps->accent = deps->accent || usesAccent;
    }
    return true;
}

// The average of a gradient's colors: what the acrylic's tint, the layered
// frame, the taskbar's background and the theme switch use.
COLORREF AverageColor(const std::vector<COLORREF>& colors) {
    if (colors.empty()) {
        return RGB(0x3C, 0x3C, 0x3C);
    }
    unsigned r = 0, g = 0, b = 0;
    for (COLORREF c : colors) {
        r += GetRValue(c);
        g += GetGValue(c);
        b += GetBValue(c);
    }
    size_t n = colors.size();
    return RGB((r + n / 2) / n, (g + n / 2) / n, (b + n / 2) / n);
}

// What the loaded frame color depends on (the "Windows' default" fill
// depends on the mode too), and the mode and accent color at load time: the
// regular tick reloads the settings when either has changed since (the
// control window is message-only, so the theme-change broadcast doesn't
// reach it).
ColorDependencies g_colorDeps;
bool g_colorLoadedLight = false;
COLORREF g_colorLoadedAccent = 0;

// The fill setting's value as an index: 0 solid, 1 custom acrylic,
// 2 Windows' default (anything else - "frosty", a fill since removed - is solid).
int FillIndex(PCWSTR value) {
    if (wcscmp(value, L"acrylic") == 0) {
        return 1;
    }
    if (wcscmp(value, L"system") == 0) {
        return 2;
    }
    return 0;
}

// A ready-made theme as the Custom theme's settings, in Windhawk's YAML, so
// it can be pasted into the settings editor's text mode and tweaked from
// there (a mod can't write its own settings). Logged when a theme is first
// applied in the session and whenever another theme is picked - not for
// every settings load (an override changed, the Windows mode changed).
std::wstring HexColor(COLORREF c) {
    WCHAR text[16];
    swprintf_s(text, L"#%02X%02X%02X", GetRValue(c), GetGValue(c), GetBValue(c));
    return text;
}
std::wstring g_loggedTheme;  // the theme last logged; cleared while Custom is picked
void LogThemeAsYaml(const ThemeDef& theme, const Settings& s, int fill) {
    if (g_loggedTheme == theme.id) {
        return;
    }
    g_loggedTheme = theme.id;
    static const PCWSTR fillNames[] = {L"solid", L"acrylic", L"system"};
    std::wstring y;
    y += L"theme: custom\n";
    y += L"overrides:\n  offset: -1\n  radius: -1\n  monitorCornerRadius: -1\n  customColor: ''\n  gradientAngle: -1\n  colorSource: theme\n"
         L"  fill: theme\n  workAreaInset: -1\n  windowCornerRadius: -2\n";
    y += L"general:\n";
    y += L"  scaleWithDpi: " + std::wstring(s.scaleWithDpi ? L"true" : L"false") + L"\n";
    y += L"  monitors: " + std::wstring(s.primaryOnly ? L"primary" : L"all") + L"\n";
    y += L"  batterySaverSolid: " + std::wstring(s.batterySaverSolid ? L"true" : L"false") + L"\n";
    y += L"frame:\n";
    y += L"  offset: " + std::to_wstring(s.offset) + L"\n";
    y += L"  edgeOffsetsClockwise: '" + std::to_wstring(s.edgeOffset[0]) + L"," +
         std::to_wstring(s.edgeOffset[1]) + L"," + std::to_wstring(s.edgeOffset[2]) + L"," +
         std::to_wstring(s.edgeOffset[3]) + L"'\n";
    y += L"  radius: " + std::to_wstring(s.radius) + L"\n";
    y += L"  customColor: '" + s.colorText + L"'\n";
    y += L"  gradientAngle: " + std::to_wstring(s.gradientAngle) + L"\n";
    y += L"  colorSource: " + std::wstring(s.colorSource ? L"titlebar" : L"custom") + L"\n";
    y += L"  opacity: " + std::to_wstring(s.opacity) + L"\n";
    y += L"  fill: " + std::wstring(fillNames[std::clamp(fill, 0, 2)]) + L"\n";
    y += L"  workAreaInset: " + std::to_wstring(s.workAreaInset) + L"\n";
    y += L"customAcrylic:\n";
    y += L"  blur: " + std::to_wstring(s.backdropBlur) + L"\n";
    y += L"  tintOpacity: " + std::to_wstring(s.tintOpacity) + L"\n";
    y += L"  luminosityOpacity: " + std::to_wstring(s.luminosityOpacity) + L"\n";
    y += L"  saturation: " + std::to_wstring(s.saturation) + L"\n";
    y += L"  noiseOpacity: " + std::to_wstring(s.noiseOpacity) + L"\n";
    y += L"  noiseDensity: " + std::to_wstring(s.noiseDensity) + L"\n";
    y += L"bevel:\n";
    y += L"  enabled: " + std::wstring(s.bevel ? L"true" : L"false") + L"\n";
    y += L"  width: " + std::to_wstring(s.bevelWidth) + L"\n";
    y += L"  curvature: " + std::to_wstring(s.bevelCurvature) + L"\n";
    y += L"  light: " + std::to_wstring(s.light) + L"\n";
    y += L"  lightAngle: " + std::to_wstring(s.lightAngle) + L"\n";
    y += L"  light2: " + std::to_wstring(s.light2) + L"\n";
    y += L"  light2Angle: " + std::to_wstring(s.light2Angle) + L"\n";
    y += L"  specular: " + std::to_wstring(s.specular) + L"\n";
    y += L"  shininess: " + std::to_wstring(s.shininess) + L"\n";
    y += L"  fresnel: " + std::to_wstring(s.fresnel) + L"\n";
    y += L"  edgeLine: " + std::to_wstring(s.edgeLine) + L"\n";
    y += L"  edgeLineColor: '" + HexColor(s.edgeLineColor) + L"'\n";
    y += L"  edgeShade: " + std::to_wstring(s.edgeShade) + L"\n";
    y += L"innerShadow:\n";
    y += L"  enabled: " + std::wstring(s.shadow ? L"true" : L"false") + L"\n";
    y += L"  blur: " + std::to_wstring(s.shadowBlur) + L"\n";
    y += L"  opacity: " + std::to_wstring(s.shadowOpacity) + L"\n";
    y += L"  color: '" + HexColor(s.shadowColor) + L"'\n";
    y += L"  offsetX: " + std::to_wstring(s.shadowOffsetX) + L"\n";
    y += L"  offsetY: " + std::to_wstring(s.shadowOffsetY) + L"\n";
    y += L"monitorCorners:\n";
    y += L"  radius: " + std::to_wstring(s.monitorCornerRadius) + L"\n";
    y += L"windowCorners:\n";
    y += L"  radius: " + std::to_wstring(theme.windowRadius) + L"\n";
    y += L"  maximizedShadow: " + std::wstring(theme.maximizedShadow ? L"true" : L"false") + L"\n";
    y += L"  maximizedBorder: " + std::wstring(theme.maximizedBorder ? L"true" : L"false") + L"\n";
    y += L"  borderColors: false\n  activeBorderColor: accent\n"
         L"  inactiveBorderColor: '#3C3C3C'\n";

    // Ready to copy: each part starts on a line of its own. A log message
    // has a length limit, so the block goes out in parts of whole sections;
    // the log's own prefix lines between them are to be left out.
    Wh_Log(L"theme \"%s\" as Custom theme settings (YAML) - copy the lines below, "
           L"without the log prefix lines:",
           theme.id);
    {
        const std::wstring& body = y;
        std::wstring part;
        for (size_t at = 0; at < body.size();) {
            // One top-level section: its key line and the indented lines under it.
            size_t end = body.find(L'\n', at);
            end = end == std::wstring::npos ? body.size() : end + 1;
            while (end < body.size() && body[end] == L' ') {
                size_t next = body.find(L'\n', end);
                end = next == std::wstring::npos ? body.size() : next + 1;
            }
            if (!part.empty() && part.size() + (end - at) > 700) {
                if (part.back() == L'\n') {
                    part.pop_back();
                }
                Wh_Log(L"\n%s", part.c_str());
                part.clear();
            }
            part.append(body, at, end - at);
            at = end;
        }
        if (!part.empty()) {
            if (part.back() == L'\n') {
                part.pop_back();
            }
            Wh_Log(L"\n%s", part.c_str());
        }
    }
}

void LoadSettings() {
    Settings s;

    // The custom theme's values, read first; a ready-made theme replaces
    // most of them below, and the overrides come last.
    s.offset = std::max(0, Wh_GetIntSetting(L"frame.offset"));

    // A new key for 2.0.0: the order changed from left,top,right,bottom to
    // taskbar edge first, then clockwise, so 1.0.0 values aren't reread
    // with the new meaning.
    auto edgeOffsets = WindhawkUtils::StringSetting::make(L"frame.edgeOffsetsClockwise");
    ParseEdgeOffsets(edgeOffsets.get(), s.edgeOffset);

    s.radius = std::max(0, Wh_GetIntSetting(L"frame.radius"));

    // One setting: a radius of 0 is the corners off ("monitorCorners.enabled"
    // is no longer read).
    s.monitorCornerRadius = std::max(0, Wh_GetIntSetting(L"monitorCorners.radius"));

    s.scaleWithDpi = Wh_GetIntSetting(L"general.scaleWithDpi") != 0;

    ColorDependencies deps;
    auto customColor = WindhawkUtils::StringSetting::make(L"frame.customColor");
    s.colorText = customColor.get();
    if (!ParseFillColors(customColor.get(), s.fillColors, &deps)) {
        s.fillColors = {RGB(0x3C, 0x3C, 0x3C)};
    }
    s.gradientAngle = ((Wh_GetIntSetting(L"frame.gradientAngle") % 360) + 360) % 360;
    auto colorSource = WindhawkUtils::StringSetting::make(L"frame.colorSource");
    s.colorSource = wcscmp(colorSource.get(), L"titlebar") == 0 ? 1 : 0;

    s.opacity = std::clamp(Wh_GetIntSetting(L"frame.opacity"), 0, 100);

    auto fillSetting = WindhawkUtils::StringSetting::make(L"frame.fill");
    int fill = FillIndex(fillSetting.get());
    s.backdropBlur = std::clamp(Wh_GetIntSetting(L"customAcrylic.blur"), 0, 250);
    s.tintOpacity = std::clamp(Wh_GetIntSetting(L"customAcrylic.tintOpacity"), 0, 100);
    s.luminosityOpacity = std::clamp(Wh_GetIntSetting(L"customAcrylic.luminosityOpacity"), 0, 100);
    s.saturation = std::clamp(Wh_GetIntSetting(L"customAcrylic.saturation"), 0, 300);
    s.noiseOpacity = std::clamp(Wh_GetIntSetting(L"customAcrylic.noiseOpacity"), 0, 100);
    s.noiseDensity = std::clamp(Wh_GetIntSetting(L"customAcrylic.noiseDensity"), 0, 100);

    s.bevel = Wh_GetIntSetting(L"bevel.enabled") != 0;
    s.bevelWidth = std::clamp(Wh_GetIntSetting(L"bevel.width"), 1, 200);
    s.bevelCurvature = std::clamp(Wh_GetIntSetting(L"bevel.curvature"), 1, 100);
    s.light = std::clamp(Wh_GetIntSetting(L"bevel.light"), 0, 100);
    s.lightAngle = Wh_GetIntSetting(L"bevel.lightAngle");
    s.light2 = std::clamp(Wh_GetIntSetting(L"bevel.light2"), 0, 100);
    s.light2Angle = Wh_GetIntSetting(L"bevel.light2Angle");
    s.specular = std::clamp(Wh_GetIntSetting(L"bevel.specular"), 0, 100);
    s.shininess = std::clamp(Wh_GetIntSetting(L"bevel.shininess"), 1, 200);
    s.fresnel = std::clamp(Wh_GetIntSetting(L"bevel.fresnel"), 0, 100);
    s.edgeLine = std::clamp(Wh_GetIntSetting(L"bevel.edgeLine"), 0, 100);
    auto edgeLineColor = WindhawkUtils::StringSetting::make(L"bevel.edgeLineColor");
    s.edgeLineColor = ParseHexColor(edgeLineColor.get(), RGB(255, 255, 255));
    s.edgeShade = std::clamp(Wh_GetIntSetting(L"bevel.edgeShade"), 0, 100);

    s.shadow = Wh_GetIntSetting(L"innerShadow.enabled") != 0;
    s.shadowBlur = std::clamp(Wh_GetIntSetting(L"innerShadow.blur"), 0, 60);
    s.shadowOpacity = std::clamp(Wh_GetIntSetting(L"innerShadow.opacity"), 0, 100);
    auto shadowColor = WindhawkUtils::StringSetting::make(L"innerShadow.color");
    s.shadowColor = ParseHexColor(shadowColor.get(), RGB(0, 0, 0));
    s.shadowOffsetX = std::clamp(Wh_GetIntSetting(L"innerShadow.offsetX"), -60, 60);
    s.shadowOffsetY = std::clamp(Wh_GetIntSetting(L"innerShadow.offsetY"), -60, 60);

    s.batterySaverSolid = Wh_GetIntSetting(L"general.batterySaverSolid") != 0;
    s.workAreaInset = std::max(0, Wh_GetIntSetting(L"frame.workAreaInset"));

    auto monitors = WindhawkUtils::StringSetting::make(L"general.monitors");
    s.primaryOnly = wcscmp(monitors.get(), L"primary") == 0;

    // A ready-made theme: its values over the custom ones.
    auto themeSetting = WindhawkUtils::StringSetting::make(L"theme");
    const ThemeDef* theme = FindTheme(themeSetting.get());
    std::wstring applied;  // the overrides in effect, for the log
    if (theme) {
        // The overrides are read first: a fill override of "Custom acrylic"
        // keeps the custom acrylic values rather than the theme's.
        int offset = Wh_GetIntSetting(L"overrides.offset");
        int radius = Wh_GetIntSetting(L"overrides.radius");
        int cornerRadius = Wh_GetIntSetting(L"overrides.monitorCornerRadius");
        auto color = WindhawkUtils::StringSetting::make(L"overrides.customColor");
        std::vector<COLORREF> overrideColors;
        ColorDependencies overrideDeps;
        bool colorOverridden = color.get() && *color.get() &&
                               ParseFillColors(color.get(), overrideColors, &overrideDeps);
        int angle = Wh_GetIntSetting(L"overrides.gradientAngle");
        auto source = WindhawkUtils::StringSetting::make(L"overrides.colorSource");
        int sourceOverride = wcscmp(source.get(), L"titlebar") == 0 ? 1
                             : wcscmp(source.get(), L"custom") == 0 ? 0
                                                                      : -1;
        auto fillOverride = WindhawkUtils::StringSetting::make(L"overrides.fill");
        int fillWanted = (*fillOverride.get() && wcscmp(fillOverride.get(), L"theme") != 0)
                             ? FillIndex(fillOverride.get())
                             : -1;
        int inset = Wh_GetIntSetting(L"overrides.workAreaInset");

        s.offset = theme->offset;
        for (int k = 0; k < 4; k++) {
            s.edgeOffset[k] = theme->edgeOffset[k];
        }
        s.radius = theme->radius;
        s.fillColors = {theme->color};
        s.colorText = HexColor(theme->color);
        s.gradientAngle = 0;
        deps = {};
        s.colorSource = theme->colorSource;
        fill = theme->fill;
        s.opacity = 100;
        if (fillWanted != 1) {
            s.backdropBlur = theme->blur;
            s.tintOpacity = theme->tint;
            s.luminosityOpacity = theme->luminosity;
            s.saturation = theme->saturation;
            s.noiseOpacity = theme->noiseOpacity;
            s.noiseDensity = theme->noiseDensity;
        }
        s.bevel = theme->bevel.enabled;
        s.bevelWidth = theme->bevel.width;
        s.bevelCurvature = theme->bevel.curvature;
        s.light = theme->bevel.light;
        s.lightAngle = theme->bevel.lightAngle;
        s.light2 = theme->bevel.light2;
        s.light2Angle = theme->bevel.light2Angle;
        s.specular = theme->bevel.specular;
        s.shininess = theme->bevel.shininess;
        s.fresnel = theme->bevel.fresnel;
        s.edgeLine = theme->bevel.edgeLine;
        s.edgeLineColor = RGB(255, 255, 255);
        s.edgeShade = theme->bevel.edgeShade;
        s.shadow = theme->shadow;
        s.shadowBlur = theme->shadowBlur;
        s.shadowOpacity = 100;
        s.shadowColor = RGB(0, 0, 0);
        s.shadowOffsetX = 0;
        s.shadowOffsetY = 0;
        s.monitorCornerRadius = theme->monitorCornerRadius;
        s.workAreaInset = theme->workAreaInset;
        LogThemeAsYaml(*theme, s, theme->fill);  // the theme itself, before the overrides

        // The overrides, each where set. A frame color override is meant to
        // be seen: it also takes the color source off the title bar, unless
        // the source override says otherwise.
        if (offset >= 0) {
            s.offset = offset;
            applied += L" offset " + std::to_wstring(offset);
        }
        if (radius >= 0) {
            s.radius = radius;
            applied += L" radius " + std::to_wstring(radius);
        }
        if (cornerRadius >= 0) {
            s.monitorCornerRadius = cornerRadius;
            applied += L" monitor corners " + std::to_wstring(cornerRadius);
        }
        if (colorOverridden) {
            s.fillColors = std::move(overrideColors);
            s.colorText = color.get();
            deps = overrideDeps;
            s.colorSource = 0;
            applied += L" color";
        }
        if (angle >= 0) {
            s.gradientAngle = angle % 360;
            applied += L" angle " + std::to_wstring(angle);
        }
        if (sourceOverride >= 0) {
            s.colorSource = sourceOverride;
            applied += sourceOverride ? L" source=titlebar" : L" source=custom";
        }
        if (fillWanted >= 0) {
            fill = fillWanted;
            applied += L" fill=" + std::wstring(fillOverride.get());
        }
        if (inset >= 0) {
            s.workAreaInset = inset;
            applied += L" inset " + std::to_wstring(inset);
        }
    }
    if (!theme) {
        g_loggedTheme.clear();  // picking a theme again logs it again
    }
    Wh_Log(L"theme: %s%s%s", theme ? theme->id : L"custom", applied.empty() ? L"" : L", overrides:",
           applied.c_str());
    s.monitorCorners = s.monitorCornerRadius > 0;
    s.customColor = AverageColor(s.fillColors);

    s.backdrop = fill != 0;
    s.systemLook = fill == 2;
    if (s.systemLook) {
        // The taskbar's own acrylic, over DWM's blur as it comes. Dark: the
        // values matched against the dark taskbar. Light: Windows' light
        // acrylic lets more of the content's color and brightness through
        // (its tint is lighter and thinner), so less tint and luminosity,
        // with the saturation boost Windows' acrylic applies.
        bool light = SystemUsesLightTheme();
        s.customColor = light ? RGB(0xF3, 0xF3, 0xF3) : RGB(0x20, 0x20, 0x20);
        s.fillColors = {s.customColor};
        deps.mode = true;
        s.backdropBlur = 0;
        s.tintOpacity = light ? 5 : 40;
        s.luminosityOpacity = light ? 88 : 90;
        s.saturation = light ? 160 : 100;
        s.noiseOpacity = 2;
        s.noiseDensity = 100;
    }

    g_colorDeps = deps;
    g_colorLoadedLight = SystemUsesLightTheme();
    g_colorLoadedAccent = CurrentAccentColor();
    g_settings = s;
    g_compositionUnavailable = false;  // a new configuration gets a new try
    g_backdropUnavailable = false;
}

// ---------------------------------------------------------------------------
// Taskbar-thread helpers (from taskbar-multirow by m417z, GPL-3.0)
// ---------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

// Runs `proc` synchronously on the thread that owns `hWnd`. With a timeout,
// gives up after that long (the taskbar thread may be busy rebuilding its
// XAML); the parameter block is then intentionally leaked, since the thread
// could still pick the message up.
bool RunFromWindowThreadTimeout(HWND hWnd, RunFromWindowThreadProc_t proc, void* procParam,
                                DWORD timeoutMs) {
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
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    bool ok = true;
    if (timeoutMs == 0) {
        RUN_FROM_WINDOW_THREAD_PARAM param;
        param.proc = proc;
        param.procParam = procParam;
        SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    } else {
        auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{proc, procParam};
        DWORD_PTR result = 0;
        if (SendMessageTimeoutW(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)param,
                                SMTO_BLOCK | SMTO_ABORTIFHUNG, timeoutMs, &result)) {
            delete param;
        } else {
            ok = false;  // timed out: `param` is leaked on purpose
        }
    }

    UnhookWindowsHookEx(hook);

    return ok;
}

// ---- Fire-and-forget calls on a window's thread -----------------------------
// Everything the mod does to the taskbar's XAML tree during normal operation
// is posted: a WH_GETMESSAGE hook on the taskbar thread runs the procedure
// when the message is retrieved, and the answer comes back as a message to
// the mod's control window. The mod's UI thread never waits on the taskbar
// thread, so a taskbar busy with a relayout (which can take seconds and run
// nested message loops) can't stall it.
struct AsyncCall {
    std::function<void()> fn;
};

UINT g_asyncMsg = 0;
std::mutex g_asyncHookMutex;
struct ThreadHooks {
    HHOOK getMessage = nullptr;   // runs posted procedures
    HHOOK callWndProc = nullptr;  // sees the taskbar window's own messages (moves)
};
std::unordered_map<DWORD, ThreadHooks> g_asyncHooks;  // thread id -> hooks

// Defined with the taskbar-side code: a taskbar window just moved/resized.
void OnTaskbarWindowPosChanged(HWND hwnd, const WINDOWPOS* pos);
bool IsTaskbarWindow(HWND hwnd);  // defined with the mod's thread code
std::atomic<bool> g_zOrderEventPending{false};  // a kMsgTaskbarZOrder is queued

LRESULT CALLBACK TaskbarCallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == WM_WINDOWPOSCHANGED && cwp->lParam) {
            auto* pos = reinterpret_cast<const WINDOWPOS*>(cwp->lParam);
            try {
                OnTaskbarWindowPosChanged(cwp->hwnd, pos);
            } catch (...) {
            }
            // The acrylic frame has to stay under its taskbar: a z-order
            // change of a taskbar window is reported to the UI thread, which
            // puts the frame back under it if needed. Nothing is done here:
            // this thread must never wait on the UI thread.
            if (!(pos->flags & SWP_NOZORDER) && g_overlaysComposition && !g_unloading &&
                IsTaskbarWindow(cwp->hwnd)) {
                HWND ctrl = g_ctrlWnd.load();
                if (ctrl && !g_zOrderEventPending.exchange(true) &&
                    !PostMessageW(ctrl, kMsgTaskbarZOrder, 0, 0)) {
                    g_zOrderEventPending = false;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

LRESULT CALLBACK AsyncGetMessageHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam & PM_REMOVE)) {
        MSG* msg = reinterpret_cast<MSG*>(lParam);
        if (msg->message == g_asyncMsg && msg->lParam) {
            std::unique_ptr<AsyncCall> call(reinterpret_cast<AsyncCall*>(msg->lParam));
            msg->message = WM_NULL;
            msg->lParam = 0;
            try {
                call->fn();
            } catch (...) {
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Installs the two hooks on a window's thread once (both stay until unload).
bool EnsureThreadHooks(DWORD threadId) {
    std::lock_guard<std::mutex> lock(g_asyncHookMutex);
    if (g_asyncHooks.find(threadId) != g_asyncHooks.end()) {
        return true;
    }
    ThreadHooks hooks;
    hooks.getMessage = SetWindowsHookEx(WH_GETMESSAGE, AsyncGetMessageHook, nullptr, threadId);
    if (!hooks.getMessage) {
        Wh_Log(L"SetWindowsHookEx (WH_GETMESSAGE) failed: %u", GetLastError());
        return false;
    }
    hooks.callWndProc = SetWindowsHookEx(WH_CALLWNDPROC, TaskbarCallWndProcHook, nullptr, threadId);
    if (!hooks.callWndProc) {
        Wh_Log(L"SetWindowsHookEx (WH_CALLWNDPROC) failed: %u", GetLastError());
    }
    g_asyncHooks[threadId] = hooks;
    return true;
}

// The acrylic frame needs the z-order hook on its taskbar's thread even when
// nothing is posted there.
void EnsureTaskbarHooks(HWND taskbar) {
    if (DWORD threadId = taskbar ? GetWindowThreadProcessId(taskbar, nullptr) : 0) {
        EnsureThreadHooks(threadId);
    }
}

bool PostToWindowThread(HWND hWnd, std::function<void()> fn) {
    if (!g_asyncMsg) {
        g_asyncMsg = RegisterWindowMessage(L"Windhawk_PostToWindowThread_" WH_MOD_ID);
    }
    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        fn();
        return true;
    }
    if (!EnsureThreadHooks(threadId)) {
        return false;
    }
    auto* call = new AsyncCall{std::move(fn)};
    if (!PostMessageW(hWnd, g_asyncMsg, 0, reinterpret_cast<LPARAM>(call))) {
        delete call;
        return false;
    }
    return true;
}

void RemoveAsyncHooks() {
    std::lock_guard<std::mutex> lock(g_asyncHookMutex);
    for (auto& entry : g_asyncHooks) {
        if (entry.second.getMessage) {
            UnhookWindowsHookEx(entry.second.getMessage);
        }
        if (entry.second.callWndProc) {
            UnhookWindowsHookEx(entry.second.callWndProc);
        }
    }
    g_asyncHooks.clear();
}

// ---- taskbar.dll: XAML root of a taskbar window ----------------------------

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

wux::XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 && b[5] == 0x83 &&
            b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD && p[2] == 0x910003FD &&
            (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] + taskbarElementIUnknownOffset);

    wux::FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {  // null while the host is still being set up
        taskbarElementIUnknown->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                               winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

wux::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;  // taskbar still being constructed
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite != CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

wux::XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;  // taskbar still being constructed
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0;
         *(void**)taskBandForTaskListWndSite != CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite, taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// XAML root of a Shell_TrayWnd / Shell_SecondaryTrayWnd window; nullptr otherwise.
wux::XamlRoot XamlRootOfTaskbarWindow(HWND hWnd) {
    WCHAR cls[32];
    if (!GetClassName(hWnd, cls, ARRAYSIZE(cls))) {
        return nullptr;
    }
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0) {
        return GetTaskbarXamlRoot(hWnd);
    }
    if (_wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return GetSecondaryTaskbarXamlRoot(hWnd);
    }
    return nullptr;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    // Address lookups only - nothing is actually hooked.
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed (taskbar.dll)");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Taskbar XAML tree helpers (taskbar thread only)
// ---------------------------------------------------------------------------

wux::FrameworkElement FindDescendantByName(wux::DependencyObject const& root,
                                           std::wstring_view name, int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto fe = child.try_as<wux::FrameworkElement>(); fe && fe.Name() == name) {
            return fe;
        }
        if (auto found = FindDescendantByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

wux::DependencyObject FindDescendantByClassName(wux::DependencyObject const& root,
                                                std::wstring_view className, int depth) {
    if (!root || depth < 0) {
        return nullptr;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (winrt::get_class_name(child) == className) {
            return child;
        }
        if (auto found = FindDescendantByClassName(child, className, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// Root element of a taskbar window's XAML tree (its ScrollViewer), or null.
wux::UIElement TaskbarXamlContent(HWND taskbar) {
    auto xamlRoot = XamlRootOfTaskbarWindow(taskbar);
    return xamlRoot ? xamlRoot.Content() : nullptr;
}

// ... > Taskbar.TaskbarFrame > RootGrid > Taskbar.TaskbarBackground#BackgroundControl
wux::DependencyObject FindTaskbarBackground(HWND taskbar) {
    auto content = TaskbarXamlContent(taskbar);
    return content ? FindDescendantByClassName(content, L"Taskbar.TaskbarBackground", 12)
                   : nullptr;
}

// COM identity comparison (a Brush and a SolidColorBrush projection of the same
// object hold different interface pointers).
bool SameObject(winrt::Windows::Foundation::IInspectable const& a,
                winrt::Windows::Foundation::IInspectable const& b) {
    if (!a || !b) {
        return !a && !b;
    }
    return a.as<winrt::Windows::Foundation::IUnknown>() ==
           b.as<winrt::Windows::Foundation::IUnknown>();
}

// What the mod wants applied on each taskbar, kept on the taskbar thread so
// a LayoutUpdated handler can re-apply it inside the taskbar's own layout
// pass after a tree rebuild (see "Keeping the taskbar-side tweaks" below).
struct TaskbarWants {
    int theme = kThemeNone;  // the theme forced on the taskbar's root (title-bar color source)
    bool strokeHidden = false;
    bool fill = false;
    DWORD fillArgb = 0;
    bool ears = false;
    int edge = kEdgeNone;
    float radiusDips = 0.0f;
    float earInsetDips = 0.0f;  // the bezel ring the arcs start from (acrylic)
    DWORD earRgb = 0;
    bool lastReady = false;  // everything wanted was in place at the last check
    int loggedEdge = kEdgeNone;  // the edge the last "tree whole" line was logged for
    // The background is kept transparent - from the start, and again from
    // the moment the window moves in a layout change - until the UI thread
    // asks for the color once everything has settled, together with showing
    // the frame. So the taskbar never colors ahead of the frame.
    bool fillSuspended = true;
    // XAML-level veil: the root content is kept at opacity 0 from the moment
    // the window moves to another edge (or the tree is found not whole) until
    // the tree has been whole across two rendered frames, so the first frame
    // the taskbar shows again is one with the ears on the right corners.
    bool hidden = false;
    ULONGLONG hiddenSince = 0;
    bool lastWhole = false;         // the last layout pass found the tree whole
    unsigned changedAtRender = 0;   // g_renderCounter when something was last changed
    winrt::weak_ref<wux::FrameworkElement> watched;  // root content the handler is on
    winrt::event_token layoutToken{};
    winrt::weak_ref<wux::XamlRoot> rootWatched;      // XamlRoot the Changed handler is on
    winrt::event_token rootToken{};
};

// Keyed by taskbar HWND. Touched only on the taskbar thread. Not destroyed
// with the image (like g_strokes, g_fills and g_xamlCorners below), so no
// XAML reference is ever released from the wrong thread; the taskbar thread
// empties it at unload (RestoreTaskbarsOnTaskbarThread).
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, TaskbarWants>> g_wants{std::in_place};

// ---------------------------------------------------------------------------
// Taskbar border line (BackgroundStroke) - taskbar thread only
// ---------------------------------------------------------------------------

struct StrokeState {
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
    bool hidden = false;
};

// Keyed by taskbar HWND. Touched only on the taskbar thread (see g_wants).
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, StrokeState>> g_strokes{std::in_place};

// Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundStroke
wux::FrameworkElement FindBackgroundStroke(HWND taskbar) {
    if (auto background = FindTaskbarBackground(taskbar)) {
        if (auto stroke = FindDescendantByName(background, L"BackgroundStroke", 4)) {
            return stroke;
        }
    }
    auto content = TaskbarXamlContent(taskbar);
    return content ? FindDescendantByName(content, L"BackgroundStroke", 16) : nullptr;
}

struct StrokeOp {
    HWND taskbar;
    bool found;    // out
    bool changed;  // out: something had to be (re)applied
};

// Hides (opacity 0) the border line of one taskbar, re-applying it if something
// reset it since. Records the original opacity the first time.
void WINAPI HideStrokeOnTaskbarThread(void* param) try {
    auto* op = static_cast<StrokeOp*>(param);
    op->found = false;
    op->changed = false;
    if (g_unloading) {
        return;
    }

    auto& strokes = *g_strokes;
    auto it = strokes.find(op->taskbar);
    wux::FrameworkElement stroke = it != strokes.end() ? it->second.element.get() : nullptr;
    if (stroke && !stroke.IsLoaded()) {
        stroke = nullptr;  // left over from a tree the taskbar has rebuilt
    }

    if (!stroke) {
        stroke = FindBackgroundStroke(op->taskbar);
        if (!stroke) {
            return;
        }
        if (it == strokes.end() || !SameObject(stroke, it->second.element.get())) {
            // A new element: record what it looks like before we touch it.
            // (The one we knew, found again after reporting itself unloaded,
            // keeps its recorded original - its current value is ours.)
            StrokeState state;
            state.element = stroke;
            state.originalOpacity = stroke.Opacity();
            it = strokes.insert_or_assign(op->taskbar, state).first;
            Wh_Log(L"taskbar %p: BackgroundStroke found (opacity %.2f)", op->taskbar,
                   state.originalOpacity);
        }
    }

    op->found = true;
    if (stroke.Opacity() != 0.0) {
        if (!it->second.hidden) {
            it->second.originalOpacity = stroke.Opacity();
        }
        stroke.Opacity(0.0);
        it->second.hidden = true;
        op->changed = true;
    }
} catch (...) {
    Wh_Log(L"HideStrokeOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// Restores every border line we hid and forgets them.
void WINAPI RestoreStrokesOnTaskbarThread(void*) try {
    for (auto& entry : *g_wants) {
        entry.second.strokeHidden = false;
    }
    for (auto& entry : *g_strokes) {
        StrokeState& state = entry.second;
        if (!state.hidden) {
            continue;
        }
        if (auto stroke = state.element.get()) {
            stroke.Opacity(state.originalOpacity);
            Wh_Log(L"taskbar %p: BackgroundStroke restored (opacity %.2f)", entry.first,
                   state.originalOpacity);
        }
    }
    g_strokes->clear();
} catch (...) {
    Wh_Log(L"RestoreStrokesOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// ---------------------------------------------------------------------------
// Taskbar background fill (the frame color on the taskbar) - taskbar thread only
// ---------------------------------------------------------------------------

struct FillState {
    winrt::weak_ref<wux::FrameworkElement> target;  // Rectangle#BackgroundFill, or the
                                                    // TaskbarBackground control itself
    bool isRectangle = false;
    bool hadLocalValue = false;
    wuxm::Brush originalBrush{nullptr};
    wuxm::SolidColorBrush ourBrush{nullptr};
    bool applied = false;
};

// Keyed by taskbar HWND. Touched only on the taskbar thread (see g_wants).
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, FillState>> g_fills{std::in_place};

// The element that paints the taskbar background:
// Taskbar.TaskbarBackground#BackgroundControl > Grid > Rectangle#BackgroundFill
bool FindBackgroundFillTarget(HWND taskbar, FillState& state) {
    auto background = FindTaskbarBackground(taskbar);
    if (!background) {
        return false;
    }

    if (auto fill = FindDescendantByName(background, L"BackgroundFill", 4)) {
        if (fill.try_as<wuxs::Rectangle>()) {
            state.target = fill;
            state.isRectangle = true;
            return true;
        }
    }

    // No fill rectangle (changed template?): paint the control itself.
    if (auto control = background.try_as<wuxc::Control>()) {
        state.target = control;
        state.isRectangle = false;
        return true;
    }
    return false;
}

wuxm::Brush GetTargetBrush(wux::FrameworkElement const& target, bool isRectangle) {
    if (isRectangle) {
        return target.as<wuxs::Rectangle>().Fill();
    }
    return target.as<wuxc::Control>().Background();
}

void SetTargetBrush(wux::FrameworkElement const& target, bool isRectangle,
                    wuxm::Brush const& brush) {
    if (isRectangle) {
        target.as<wuxs::Rectangle>().Fill(brush);
    } else {
        target.as<wuxc::Control>().Background(brush);
    }
}

wux::DependencyProperty TargetBrushProperty(bool isRectangle) {
    return isRectangle ? wuxs::Shape::FillProperty() : wuxc::Control::BackgroundProperty();
}

struct FillOp {
    HWND taskbar;
    DWORD argb;    // AARRGGBB
    bool found;    // out
    bool changed;  // out: something had to be (re)applied
};

// Paints one taskbar's background with our solid brush, re-applying it if the
// taskbar (or another mod) swapped the brush since. Records the original the
// first time.
void WINAPI ApplyFillOnTaskbarThread(void* param) try {
    auto* op = static_cast<FillOp*>(param);
    op->found = false;
    op->changed = false;
    if (g_unloading) {
        return;
    }

    auto& fills = *g_fills;
    auto it = fills.find(op->taskbar);
    wux::FrameworkElement target = it != fills.end() ? it->second.target.get() : nullptr;
    if (target && !target.IsLoaded()) {
        target = nullptr;  // left over from a tree the taskbar has rebuilt
    }

    if (!target) {
        FillState state;
        if (!FindBackgroundFillTarget(op->taskbar, state)) {
            return;
        }
        target = state.target.get();
        if (it == fills.end() || !SameObject(target, it->second.target.get())) {
            // A new element (the one we knew, found again after reporting
            // itself unloaded, keeps its recorded original brush).
            it = fills.insert_or_assign(op->taskbar, state).first;
            Wh_Log(L"taskbar %p: background target found (%s)", op->taskbar,
                   state.isRectangle ? L"Rectangle#BackgroundFill" : L"TaskbarBackground control");
        }
    }

    FillState& state = it->second;
    op->found = true;

    wu::Color color{static_cast<uint8_t>(op->argb >> 24), static_cast<uint8_t>(op->argb >> 16),
                    static_cast<uint8_t>(op->argb >> 8), static_cast<uint8_t>(op->argb)};

    if (!state.ourBrush) {
        state.ourBrush = wuxm::SolidColorBrush(color);
    } else if (state.ourBrush.Color() != color) {
        state.ourBrush.Color(color);
        op->changed = true;
    }

    wuxm::Brush current = GetTargetBrush(target, state.isRectangle);
    if (!state.applied) {
        auto local = target.ReadLocalValue(TargetBrushProperty(state.isRectangle));
        state.hadLocalValue = local != wux::DependencyProperty::UnsetValue();
        state.originalBrush = current;
    }
    if (!SameObject(current, state.ourBrush)) {
        SetTargetBrush(target, state.isRectangle, state.ourBrush);
        state.applied = true;
        op->changed = true;
    }
} catch (...) {
    Wh_Log(L"ApplyFillOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// Puts one taskbar's background back, if we painted it.
void RestoreFillOnTaskbarThread(HWND taskbar, FillState& state) {
    if (!state.applied) {
        return;
    }
    state.applied = false;
    auto target = state.target.get();
    if (!target) {
        return;
    }
    if (!SameObject(GetTargetBrush(target, state.isRectangle), state.ourBrush)) {
        return;  // someone else already replaced our brush; leave theirs
    }
    if (state.hadLocalValue && state.originalBrush) {
        SetTargetBrush(target, state.isRectangle, state.originalBrush);
    } else {
        target.ClearValue(TargetBrushProperty(state.isRectangle));
    }
    Wh_Log(L"taskbar %p: background restored", taskbar);
}

// Puts every taskbar background we painted back and forgets them.
void WINAPI RestoreFillsOnTaskbarThread(void*) try {
    for (auto& entry : *g_wants) {
        entry.second.fill = false;
        entry.second.fillSuspended = false;
    }
    for (auto& entry : *g_fills) {
        RestoreFillOnTaskbarThread(entry.first, entry.second);
    }
    g_fills->clear();
} catch (...) {
    Wh_Log(L"RestoreFillsOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// ---------------------------------------------------------------------------
// Monitor corners on the taskbar side: XAML shapes inside the taskbar
// (taskbar thread only). A separate window would have to fight the taskbar
// for z-order; a child of the taskbar's own tree moves, hides and raises
// with it for free.
// ---------------------------------------------------------------------------

struct XamlCornerState {
    winrt::weak_ref<wuxc::Grid> host;
    wuxs::Path ear[2]{nullptr, nullptr};
    // The bezel ring inside the taskbar: its row along the taskbar's outer
    // edge, and its columns along the taskbar's two ends (which lie on the
    // monitor's other edges).
    wuxs::Rectangle line[3]{nullptr, nullptr, nullptr};
    int edge = -1;             // what the ears are configured for
    float radiusDips = 0.0f;
    float insetDips = 0.0f;
    wu::Color color{0, 0, 0, 0};
    bool placed = true;        // host laid out at the window's size (for the log)
};

// Keyed by taskbar HWND. Touched only on the taskbar thread (see g_wants).
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, XamlCornerState>> g_xamlCorners{
    std::in_place};

// The two monitor corners that lie on the taskbar's edge.
void TaskbarEdgeCorners(int edge, int out[2]) {
    switch (edge) {
        case kEdgeTop:
            out[0] = kCornerTL;
            out[1] = kCornerTR;
            break;
        case kEdgeBottom:
            out[0] = kCornerBL;
            out[1] = kCornerBR;
            break;
        case kEdgeLeft:
            out[0] = kCornerTL;
            out[1] = kCornerBL;
            break;
        default:
            out[0] = kCornerTR;
            out[1] = kCornerBR;
            break;
    }
}

// The edge the taskbar window is on right now, from its own rect. During a
// layout change the window has already moved while the mod's posted request
// (carrying the edge the UI thread saw) may still be queued; the ears must
// follow the window, not the request.
int LiveTaskbarEdge(HWND taskbar, int fallback) {
    RECT rect{};
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    if (!GetWindowRect(taskbar, &rect) || rect.right <= rect.left || rect.bottom <= rect.top ||
        !monitor || !GetMonitorInfoW(monitor, &mi)) {
        return fallback;
    }
    return DetectEdge(rect, mi.rcMonitor);
}

// A Grid that spans the whole taskbar window: the parent of
// Taskbar.TaskbarFrame (Styler themes shrink the frame, not its parent), or
// the frame's RootGrid as a fallback.
wuxc::Grid FindCornerHost(HWND taskbar) {
    auto content = TaskbarXamlContent(taskbar);
    if (!content) {
        return nullptr;
    }
    auto frame = FindDescendantByClassName(content, L"Taskbar.TaskbarFrame", 12);
    if (!frame) {
        return nullptr;
    }
    if (auto parent = wuxm::VisualTreeHelper::GetParent(frame)) {
        if (auto grid = parent.try_as<wuxc::Grid>()) {
            return grid;
        }
    }
    if (auto rootGrid = FindDescendantByName(frame, L"RootGrid", 2)) {
        return rootGrid.try_as<wuxc::Grid>();
    }
    return nullptr;
}

// An ear (DIPs): the square at the corner minus the quarter circle of radius `r`
// whose arc is tangent to the edges `inset` in from the window's corner -
// the inner edge of the one-pixel bezel ring, where the frame's own ears'
// arcs start too.
void ConfigureEar(wuxs::Path const& ear, int corner, float r, float inset) {
    bool left = corner == kCornerTL || corner == kCornerBL;
    bool top = corner == kCornerTL || corner == kCornerTR;
    float size = r + inset;

    wuxm::GeometryGroup group;
    group.FillRule(wuxm::FillRule::EvenOdd);
    wuxm::RectangleGeometry square;
    square.Rect(winrt::Windows::Foundation::Rect{0.0f, 0.0f, size, size});
    wuxm::EllipseGeometry circle;
    circle.Center(winrt::Windows::Foundation::Point{left ? size : 0.0f, top ? size : 0.0f});
    circle.RadiusX(r);
    circle.RadiusY(r);
    group.Children().Append(square);
    group.Children().Append(circle);

    ear.Data(group);
    ear.Width(size);
    ear.Height(size);
    ear.HorizontalAlignment(left ? wux::HorizontalAlignment::Left
                                 : wux::HorizontalAlignment::Right);
    ear.VerticalAlignment(top ? wux::VerticalAlignment::Top : wux::VerticalAlignment::Bottom);
    ear.Margin(wux::Thickness{0, 0, 0, 0});
    ear.IsHitTestVisible(false);
    wuxc::Canvas::SetZIndex(ear, 10000);
}

// The bezel ring's lines inside the taskbar, each `thickness` DIPs (one
// device pixel) thick along a whole side of the taskbar window: `which` 0
// is the taskbar's outer edge, 1 and 2 its two ends - the sides that run
// from the outer edge inwards and lie on the monitor's other two edges.
void ConfigureBezelLine(wuxs::Rectangle const& line, int edge, float thickness, int which) {
    double automatic = std::numeric_limits<double>::quiet_NaN();
    bool taskbarHorizontal = edge == kEdgeTop || edge == kEdgeBottom;
    bool lineHorizontal = which == 0 ? taskbarHorizontal : !taskbarHorizontal;
    line.Width(lineHorizontal ? automatic : thickness);
    line.Height(lineHorizontal ? thickness : automatic);
    wux::HorizontalAlignment h = wux::HorizontalAlignment::Stretch;
    wux::VerticalAlignment v = wux::VerticalAlignment::Stretch;
    if (which == 0) {
        if (taskbarHorizontal) {
            v = edge == kEdgeTop ? wux::VerticalAlignment::Top : wux::VerticalAlignment::Bottom;
        } else {
            h = edge == kEdgeLeft ? wux::HorizontalAlignment::Left
                                  : wux::HorizontalAlignment::Right;
        }
    } else if (taskbarHorizontal) {
        h = which == 1 ? wux::HorizontalAlignment::Left : wux::HorizontalAlignment::Right;
    } else {
        v = which == 1 ? wux::VerticalAlignment::Top : wux::VerticalAlignment::Bottom;
    }
    line.HorizontalAlignment(h);
    line.VerticalAlignment(v);
    line.Margin(wux::Thickness{0, 0, 0, 0});
    line.IsHitTestVisible(false);
    wuxc::Canvas::SetZIndex(line, 10000);
}

void SetEarColor(wuxs::Shape const& ear, wu::Color color) {
    auto brush = ear.Fill() ? ear.Fill().try_as<wuxm::SolidColorBrush>() : nullptr;
    if (brush) {
        brush.Color(color);
    } else {
        ear.Fill(wuxm::SolidColorBrush(color));
    }
}

struct XamlCornerOp {
    HWND taskbar;
    int edge;
    float radiusDips;  // 0 = remove
    float insetDips;
    DWORD rgb;         // COLORREF
    bool found;        // out
    bool changed;      // out: ears were added or reconfigured
    bool placed;       // out: the ears' host spans the window, so they sit on its corners
};

void RemoveEars(XamlCornerState& state) {
    auto host = state.host.get();
    auto remove = [&](wux::UIElement const& element) {
        if (host) {
            uint32_t index = 0;
            if (host.Children().IndexOf(element, index)) {
                host.Children().RemoveAt(index);
            }
        }
    };
    for (auto& ear : state.ear) {
        if (ear) {
            remove(ear);
            ear = nullptr;
        }
    }
    for (auto& line : state.line) {
        if (line) {
            remove(line);
            line = nullptr;
        }
    }
}

// Adds / updates the two ears of one taskbar, re-adding them if the taskbar
// rebuilt its tree since.
void WINAPI ApplyXamlCornersOnTaskbarThread(void* param) try {
    auto* op = static_cast<XamlCornerOp*>(param);
    op->found = false;
    op->changed = false;
    op->placed = false;
    if (g_unloading) {
        return;
    }

    auto& corners = *g_xamlCorners;
    auto it = corners.find(op->taskbar);
    // (A radius of 0 - monitor corners off - keeps the bezel lines, with
    // each ear reduced to the line's corner pixel.)

    wuxc::Grid host = it != corners.end() ? it->second.host.get() : nullptr;
    bool attached = host && host.IsLoaded() && it->second.ear[0] && it->second.ear[0].Parent() &&
                    it->second.ear[1] && it->second.ear[1].Parent() && it->second.line[0] &&
                    it->second.line[0].Parent();
    if (!attached) {
        if (it != corners.end()) {
            RemoveEars(it->second);
            corners.erase(it);
        }
        host = FindCornerHost(op->taskbar);
        if (!host) {
            return;
        }
        XamlCornerState state;
        state.host = host;
        for (int i = 0; i < 2; i++) {
            wuxs::Path ear;
            ear.Name(i == 0 ? L"RoundEverythingCornerA" : L"RoundEverythingCornerB");
            host.Children().Append(ear);
            state.ear[i] = ear;
        }
        static constexpr PCWSTR kLineNames[3] = {L"RoundEverythingBezel", L"RoundEverythingBezelA",
                                                 L"RoundEverythingBezelB"};
        for (int i = 0; i < 3; i++) {
            wuxs::Rectangle line;
            line.Name(kLineNames[i]);
            host.Children().Append(line);
            state.line[i] = line;
        }
        it = corners.insert_or_assign(op->taskbar, state).first;
        op->changed = true;
        Wh_Log(L"taskbar %p: corner ears added to %s", op->taskbar,
               host.Name().empty() ? L"(unnamed grid)" : host.Name().c_str());
    }

    // Geometry and color are only touched when something changed; this runs
    // every couple of seconds and on every layout pass of the taskbar.
    XamlCornerState& state = it->second;
    int edge = LiveTaskbarEdge(op->taskbar, op->edge);
    if (state.edge != edge || state.radiusDips != op->radiusDips ||
        state.insetDips != op->insetDips) {
        int edgeCorners[2];
        TaskbarEdgeCorners(edge, edgeCorners);
        for (int i = 0; i < 2; i++) {
            ConfigureEar(state.ear[i], edgeCorners[i], op->radiusDips, op->insetDips);
        }
        for (int i = 0; i < 3; i++) {
            ConfigureBezelLine(state.line[i], edge, op->insetDips, i);
        }
        if (state.edge != edge) {
            Wh_Log(L"taskbar %p: corner ears configured for edge %d", op->taskbar, edge);
        }
        state.edge = edge;
        state.radiusDips = op->radiusDips;
        state.insetDips = op->insetDips;
        op->changed = true;
    }
    wu::Color color{255, GetRValue(op->rgb), GetGValue(op->rgb), GetBValue(op->rgb)};
    if (state.color != color) {
        for (auto const& ear : state.ear) {
            SetEarColor(ear, color);
        }
        for (auto const& line : state.line) {
            SetEarColor(line, color);
        }
        state.color = color;
        op->changed = true;
    }
    op->found = true;

    // The ears sit on the host's corners; they are only where the window's
    // corners are once the host has been laid out at the window's size
    // (during a layout change it keeps the old size for a pass or two).
    RECT client{};
    if (GetClientRect(op->taskbar, &client)) {
        double scale = 1.0;
        if (auto root = host.XamlRoot()) {
            scale = root.RasterizationScale();
        }
        double hostW = host.ActualWidth() * scale;
        double hostH = host.ActualHeight() * scale;
        double winW = client.right - client.left;
        double winH = client.bottom - client.top;
        op->placed = std::fabs(hostW - winW) <= 2.0 && std::fabs(hostH - winH) <= 2.0;
        if (!op->placed && state.placed) {
            Wh_Log(L"taskbar %p: ears' host %.0fx%.0f vs window %.0fx%.0f, not placed yet",
                   op->taskbar, hostW, hostH, winW, winH);
        }
        state.placed = op->placed;
    }
} catch (...) {
    Wh_Log(L"ApplyXamlCornersOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

void WINAPI RemoveXamlCornersOnTaskbarThread(void*) try {
    for (auto& entry : *g_wants) {
        entry.second.ears = false;
    }
    for (auto& entry : *g_xamlCorners) {
        RemoveEars(entry.second);
    }
    g_xamlCorners->clear();
} catch (...) {
    Wh_Log(L"RemoveXamlCornersOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// ---------------------------------------------------------------------------
// Layout: monitors, taskbars, frame geometry
// ---------------------------------------------------------------------------

struct MonitorEntry {
    HMONITOR handle;
    RECT rect;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<MonitorEntry>*>(lParam);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, &mi)) {
        list->push_back({hMonitor, mi.rcMonitor});
    }
    return TRUE;
}

BOOL CALLBACK SecondaryTaskbarEnumProc(HWND hwnd, LPARAM lParam) {
    WCHAR className[64];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    }
    return TRUE;
}

int DetectEdge(const RECT& taskbar, const RECT& monitor) {
    LONG width = taskbar.right - taskbar.left;
    LONG height = taskbar.bottom - taskbar.top;
    if (width >= height) {
        LONG centerY = (taskbar.top + taskbar.bottom) / 2;
        return centerY < (monitor.top + monitor.bottom) / 2 ? kEdgeTop : kEdgeBottom;
    }
    LONG centerX = (taskbar.left + taskbar.right) / 2;
    return centerX < (monitor.left + monitor.right) / 2 ? kEdgeLeft : kEdgeRight;
}

// Where the taskbar sits when shown: its window moved to the monitor's edge
// and clipped to the monitor. A docked taskbar is already there; an auto-hide
// one slides off the edge, keeping its size, and the strip stays put so the
// frame and the work area don't follow the slide.
RECT DockedStrip(const RECT& taskbar, int edge, const RECT& monitor) {
    RECT strip = monitor;
    LONG thickness = (edge == kEdgeTop || edge == kEdgeBottom) ? taskbar.bottom - taskbar.top
                                                                : taskbar.right - taskbar.left;
    switch (edge) {
        case kEdgeTop:
            strip.bottom = std::min(monitor.bottom, monitor.top + thickness);
            break;
        case kEdgeBottom:
            strip.top = std::max(monitor.top, monitor.bottom - thickness);
            break;
        case kEdgeLeft:
            strip.right = std::min(monitor.right, monitor.left + thickness);
            break;
        default:
            strip.left = std::max(monitor.left, monitor.right - thickness);
            break;
    }
    return strip;
}

void AppendRect(std::wstring& s, const RECT& r) {
    s += std::to_wstring(r.left) + L',' + std::to_wstring(r.top) + L',' +
         std::to_wstring(r.right) + L',' + std::to_wstring(r.bottom);
}

// ---- DWM veil for a re-created taskbar window (mod UI thread) ---------------

void StartVeil(HWND taskbar, int edge, ULONGLONG now) {
    Veil& veil = g_veils[taskbar];
    bool fresh = veil.since == 0;
    veil.since = now;
    veil.edge = edge;
    CloakWindow(taskbar, true);
    Wh_Log(L"taskbar %p: %s (edge %d)", taskbar, fresh ? L"veiled" : L"veil renewed", edge);
}

void LiftVeil(HWND taskbar, PCWSTR why) {
    auto it = g_veils.find(taskbar);
    if (it == g_veils.end()) {
        return;
    }
    if (IsWindow(taskbar)) {
        CloakWindow(taskbar, false);
    }
    Wh_Log(L"taskbar %p: veil lifted (%s)", taskbar, why);
    g_veils.erase(it);
}

// Each tick: a veil that has run its course is lifted; one the shell has
// uncloaked in the meantime is put back.
void MaintainVeils(ULONGLONG now) {
    std::vector<HWND> expired;
    for (auto& entry : g_veils) {
        if (!IsWindow(entry.first) || now - entry.second.since >= kVeilMaxMs) {
            expired.push_back(entry.first);
        } else if (!IsWindowCloaked(entry.first)) {
            CloakWindow(entry.first, true);
            Wh_Log(L"taskbar %p: re-cloaked", entry.first);
        }
    }
    for (HWND taskbar : expired) {
        LiftVeil(taskbar, L"timed out");
    }
}

void LiftAllVeils() {
    while (!g_veils.empty()) {
        LiftVeil(g_veils.begin()->first, L"unload");
    }
}

// kMsgTaskbarReady: the taskbar's tree is whole and rendered, with ears for
// `edge`.
void OnTaskbarReady(WPARAM wParam, LPARAM lParam) {
    HWND taskbar = reinterpret_cast<HWND>(lParam);
    int edge = static_cast<int>(wParam) - 1;
    g_taskbarReady[taskbar] = {GetTickCount64(), edge};
    auto it = g_veils.find(taskbar);
    if (it == g_veils.end()) {
        return;
    }
    if (edge != kEdgeNone && edge != it->second.edge) {
        Wh_Log(L"taskbar %p: tree whole for edge %d, veil is for edge %d; waiting", taskbar, edge,
               it->second.edge);
        return;
    }
    LiftVeil(taskbar, L"tree whole");
}

// Computes one layout per monitor (with or without a taskbar). Returns a
// signature string that changes whenever the geometry changes.
std::wstring ComputeLayouts(std::vector<Layout>& layouts) {
    layouts.clear();
    ULONGLONG now = GetTickCount64();

    std::vector<MonitorEntry> monitors;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&monitors));

    HWND primaryTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    std::vector<HWND> taskbars;
    if (primaryTaskbar) {
        taskbars.push_back(primaryTaskbar);
    }
    EnumWindows(SecondaryTaskbarEnumProc, reinterpret_cast<LPARAM>(&taskbars));

    // Auto-hide state and the primary taskbar's edge come from the appbar
    // thread (never a blocking shell call here). The reported edge is only
    // trusted while the taskbar still has the rect it was reported for;
    // during a move the rect decides.
    ShellState shell;
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        shell = g_shellState;
    }
    bool autoHide = shell.autoHide;
    int primaryEdge = -1;
    RECT primaryRectNow{};
    if (shell.primaryEdge >= 0 && primaryTaskbar && GetWindowRect(primaryTaskbar, &primaryRectNow) &&
        EqualRect(&primaryRectNow, &shell.primaryRect)) {
        primaryEdge = shell.primaryEdge;
    }

    std::wstring signature = autoHide ? L"autohide;" : L"docked;";

    for (const MonitorEntry& mon : monitors) {
        HWND taskbar = nullptr;
        for (HWND candidate : taskbars) {
            if (MonitorFromWindow(candidate, MONITOR_DEFAULTTONEAREST) == mon.handle) {
                taskbar = candidate;
                break;
            }
        }
        if (g_settings.primaryOnly && taskbar != primaryTaskbar) {
            continue;
        }

        // The taskbar strip, as it is right now. A taskbar window that is
        // hidden, cloaked (a secondary taskbar after "show my taskbar on all
        // displays" is turned off) or empty is no taskbar: the monitor gets
        // the four-sided frame until a usable taskbar is back, and the frame
        // re-shapes the moment that happens.
        int edge = kEdgeNone;
        RECT taskbarRect{};
        bool usable = false;
        bool cloakedTooLong = false;
        if (taskbar) {
            if (IsWindowCloaked(taskbar) && !g_veils.count(taskbar)) {
                ULONGLONG& since = g_cloakedSince[taskbar];
                if (since == 0) {
                    since = now;
                }
                cloakedTooLong = now - since >= kCloakGraceMs;
            } else {
                g_cloakedSince.erase(taskbar);
            }
        }
        // An auto-hide taskbar that has slid out is hidden (the shell hides
        // the window once it is off the screen): still the taskbar of this
        // monitor, on the strip it left - the frame and the work area stay.
        bool shown = taskbar && IsWindowVisible(taskbar);
        if (taskbar && GetWindowRect(taskbar, &taskbarRect) && (shown || autoHide) &&
            !cloakedTooLong && taskbarRect.right > taskbarRect.left &&
            taskbarRect.bottom > taskbarRect.top) {
            usable = true;
            SeenTaskbar& seen = g_seenTaskbars[mon.handle];
            if (!shown && seen.hwnd == taskbar && seen.edge != kEdgeNone &&
                !IsRectEmpty(&seen.rect)) {
                edge = seen.edge;
                taskbarRect = seen.rect;
            } else {
                edge = (taskbar == primaryTaskbar && primaryEdge >= 0)
                           ? primaryEdge
                           : DetectEdge(taskbarRect, mon.rect);
                // An auto-hide taskbar's slide doesn't count as a move: the
                // strip it slides over is what the layout is built on.
                taskbarRect = DockedStrip(taskbarRect, edge, mon.rect);
            }
            if (seen.hwnd != taskbar || !EqualRect(&seen.rect, &taskbarRect) || seen.edge != edge) {
                Wh_Log(L"taskbar %p: %s (%ld,%ld,%ld,%ld), edge %d%s", taskbar,
                       seen.hwnd == taskbar ? L"moved/resized" : L"in use", taskbarRect.left,
                       taskbarRect.top, taskbarRect.right, taskbarRect.bottom, edge,
                       autoHide ? L" (auto-hide)" : L"");
                // A re-created window is cloaked until its tree is whole (not
                // at startup, where the taskbar has been fine all along). A
                // window that merely moved is handled inside its own thread:
                // the content stays at opacity 0 until the tree is whole.
                if (seen.hwnd != taskbar && g_taskbarXamlAvailable &&
                    now - g_threadStartTick >= 3000) {
                    StartVeil(taskbar, edge, now);
                }
                seen = {taskbar, taskbarRect, edge};
            }
        } else {
            auto seenIt = g_seenTaskbars.find(mon.handle);
            if (seenIt != g_seenTaskbars.end() && seenIt->second.hwnd &&
                !IsRectEmpty(&seenIt->second.rect)) {
                // Remembered (not erased): the same window coming back is a
                // move, not a new window.
                seenIt->second.rect = RECT{};
                seenIt->second.edge = kEdgeNone;
                Wh_Log(L"monitor %p: no usable taskbar (%s)", mon.handle,
                       !taskbar ? L"no window" : !IsWindowVisible(taskbar) ? L"hidden"
                                                 : cloakedTooLong ? L"cloaked for too long"
                                                                  : L"empty");
            }
            taskbar = nullptr;
            taskbarRect = RECT{};
        }

        // The taskbar's strip is taken off the monitor whether the taskbar
        // is docked or slides in and out of it (auto-hide): the frame and
        // the work area are the same either way, and an auto-hide taskbar
        // slides over the frame's band.
        RECT base = mon.rect;
        if (usable) {
            switch (edge) {
                case kEdgeTop:
                    base.top = taskbarRect.bottom;
                    break;
                case kEdgeBottom:
                    base.bottom = taskbarRect.top;
                    break;
                case kEdgeLeft:
                    base.left = taskbarRect.right;
                    break;
                default:
                    base.right = taskbarRect.left;
                    break;
            }
        }

        if (base.right - base.left < 4 || base.bottom - base.top < 4) {
            continue;
        }

        // The layered frame covers the area next to the taskbar.
        RECT overlayRect = base;

        // The acrylic frame lies under the taskbar as well (one surface, no
        // seam) and stops one pixel short of every monitor edge: a topmost
        // window whose rectangle contains the monitor is a full-screen app
        // to the shell. The ring left over is painted by the bezel windows.
        bool composition = UsesComposition();
        if (composition) {
            overlayRect = mon.rect;
            InflateRect(&overlayRect, -1, -1);
        }

        Layout layout;
        layout.composition = composition;
        layout.monitor = mon.handle;
        layout.taskbar = taskbar;
        layout.monitorRect = mon.rect;
        layout.taskbarRect = usable ? taskbarRect : RECT{};
        layout.autoHide = usable && autoHide;
        layout.baseWorkArea = base;
        layout.overlayRect = overlayRect;
        layout.edge = edge;
        layout.dpi = taskbar ? GetWindowDpi(taskbar) : GetMonitorDpi(mon.handle);

        // Per-edge thickness: the override if set, else the uniform thickness
        // on the free edges and 0 on the taskbar edge. The overrides are
        // given from the taskbar's edge clockwise (top > right > bottom >
        // left), so they only apply where there is a taskbar; a monitor
        // without one gets the uniform offset on all four edges.
        static const int kClockwise[4] = {kEdgeTop, kEdgeRight, kEdgeBottom, kEdgeLeft};
        int start = 0;
        while (edge != kEdgeNone && kClockwise[start] != edge) {
            start++;
        }
        for (int e = 0; e < 4; e++) {
            int value = -1;
            if (edge != kEdgeNone) {
                for (int k = 0; k < 4; k++) {
                    if (kClockwise[(start + k) % 4] == e) {
                        value = g_settings.edgeOffset[k];
                    }
                }
            }
            if (value < 0) {
                value = (e == edge) ? 0 : g_settings.offset;
            }
            layout.offset[e] = ScaleForDpi(value, layout.dpi);
            // The offsets are measured from the inside of the one-pixel bezel
            // ring, not from the monitor's edge: the ring is taken off the
            // screen, and an offset of 1 leaves one pixel of frame inside it.
            bool onMonitorEdge = e == kEdgeLeft     ? base.left == mon.rect.left
                                 : e == kEdgeTop    ? base.top == mon.rect.top
                                 : e == kEdgeRight  ? base.right == mon.rect.right
                                                    : base.bottom == mon.rect.bottom;
            if (onMonitorEdge) {
                layout.offset[e] += 1;
            }
        }
        layout.workAreaInset = ScaleForDpi(g_settings.workAreaInset, layout.dpi);

        int maxCornerRadius = static_cast<int>(
            std::min(mon.rect.right - mon.rect.left, mon.rect.bottom - mon.rect.top) / 4);
        layout.monitorCornerRadius =
            g_settings.monitorCorners
                ? std::clamp(ScaleForDpi(g_settings.monitorCornerRadius, layout.dpi), 0,
                             maxCornerRadius)
                : 0;
        if (g_settings.shadow && g_settings.shadowBlur > 0 && g_settings.shadowOpacity > 0) {
            layout.shadowBlur = std::max(1, ScaleForDpi(g_settings.shadowBlur, layout.dpi));
            layout.shadowOffsetX = ScaleForDpi(g_settings.shadowOffsetX, layout.dpi);
            layout.shadowOffsetY = ScaleForDpi(g_settings.shadowOffsetY, layout.dpi);
        }
        layout.backdropBlur = static_cast<float>(ScaleForDpi(g_settings.backdropBlur, layout.dpi));
        layout.bevelWidth = g_settings.bevel ? std::max(1, ScaleForDpi(g_settings.bevelWidth, layout.dpi)) : 0;

        RECT inner = base;
        inner.left += layout.offset[kEdgeLeft];
        inner.top += layout.offset[kEdgeTop];
        inner.right -= layout.offset[kEdgeRight];
        inner.bottom -= layout.offset[kEdgeBottom];
        if (inner.right - inner.left < 2) {
            LONG cx = (base.left + base.right) / 2;
            inner.left = cx - 1;
            inner.right = cx + 1;
        }
        if (inner.bottom - inner.top < 2) {
            LONG cy = (base.top + base.bottom) / 2;
            inner.top = cy - 1;
            inner.bottom = cy + 1;
        }
        layout.innerRect = inner;

        int maxRadius =
            static_cast<int>(std::min(inner.right - inner.left, inner.bottom - inner.top) / 2);
        layout.radius = std::clamp(ScaleForDpi(g_settings.radius, layout.dpi), 0, maxRadius);

        std::wstring entry = std::to_wstring(
            static_cast<unsigned long long>(reinterpret_cast<ULONG_PTR>(mon.handle)));
        entry += L':';
        AppendRect(entry, base);
        entry += L':';
        AppendRect(entry, overlayRect);
        entry += L':';
        AppendRect(entry, layout.taskbarRect);
        entry += L':' + std::to_wstring(edge) + L':' + std::to_wstring(layout.dpi) + L':' +
                 std::to_wstring(reinterpret_cast<ULONG_PTR>(taskbar)) +
                 (layout.autoHide ? L":autohide;" : L";");
        layout.signature = entry;
        signature += entry;

        layouts.push_back(layout);
    }

    return signature;
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void AddRoundedRectPath(Gdiplus::GraphicsPath& path, const RECT& rect, int radius) {
    using Gdiplus::REAL;
    REAL l = static_cast<REAL>(rect.left);
    REAL t = static_cast<REAL>(rect.top);
    REAL r = static_cast<REAL>(rect.right);
    REAL b = static_cast<REAL>(rect.bottom);
    REAL rad = static_cast<REAL>(radius);

    if (rad <= 0) {
        path.AddRectangle(Gdiplus::RectF(l, t, r - l, b - t));
        return;
    }

    REAL d = 2 * rad;
    path.StartFigure();
    path.AddArc(l, t, d, d, 180.0f, 90.0f);          // top-left
    path.AddArc(r - d, t, d, d, 270.0f, 90.0f);      // top-right
    path.AddArc(r - d, b - d, d, d, 0.0f, 90.0f);    // bottom-right
    path.AddArc(l, b - d, d, d, 90.0f, 90.0f);       // bottom-left
    path.CloseFigure();
}

// Renders a transparent PARGB bitmap of the frame window's size through
// `draw` and returns its pixels. Used for the one-time rasterizations.
bool RenderToPixels(int width, int height, std::vector<DWORD>& out,
                    void (*draw)(Gdiplus::Graphics&, const Layout&), const Layout& layout) {
    out.assign(static_cast<size_t>(width) * height, 0);
    Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppPARGB,
                           reinterpret_cast<BYTE*>(out.data()));
    Gdiplus::Graphics graphics(&bitmap);
    if (graphics.GetLastStatus() != Gdiplus::Ok) {
        return false;
    }
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    // Pixel centers at .5 so integer-aligned edges are fully covered.
    // Without this the first row/column of the bitmap only gets 50%
    // coverage and shows up as a seam next to the taskbar / screen edge.
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    draw(graphics, layout);
    graphics.Flush(Gdiplus::FlushIntentionSync);
    return true;
}

// Frame = whole window minus the inner rounded rectangle, in opaque white:
// the alpha channel is the coverage mask.
void DrawFrameShape(Gdiplus::Graphics& graphics, const Layout& layout) {
    int width = layout.overlayRect.right - layout.overlayRect.left;
    int height = layout.overlayRect.bottom - layout.overlayRect.top;

    // The outer rectangle overshoots the bitmap by a pixel so its edges can
    // never be anti-aliased.
    Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
    path.AddRectangle(Gdiplus::Rect(-1, -1, width + 2, height + 2));

    RECT inner = layout.innerRect;
    OffsetRect(&inner, -layout.overlayRect.left, -layout.overlayRect.top);
    AddRoundedRectPath(path, inner, layout.radius);

    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
    graphics.FillPath(&brush, &path);
}

// Paints the sliver outside a quarter circle at each of a rectangle's corners
// that coincides with a monitor corner.
void DrawMonitorEars(Gdiplus::Graphics& graphics, const RECT& rect, const Layout& layout) {
    int cr = layout.monitorCornerRadius;
    if (cr <= 0) {
        return;
    }
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    const RECT& m = layout.monitorRect;
    auto closeTo = [](LONG a, LONG b) { return a - b <= 1 && b - a <= 1; };
    bool atLeft = closeTo(rect.left, m.left), atRight = closeTo(rect.right, m.right);
    bool atTop = closeTo(rect.top, m.top), atBottom = closeTo(rect.bottom, m.bottom);
    Gdiplus::SolidBrush cornerBrush(Gdiplus::Color(255, GetRValue(kMonitorCornerColor),
                                                   GetGValue(kMonitorCornerColor),
                                                   GetBValue(kMonitorCornerColor)));
    // The two corners on the taskbar's edge belong to the XAML ears inside
    // the taskbar (the acrylic frame reaches those corners too, under the
    // taskbar; a second arc there, a pixel off the XAML one, would show
    // through the transparent background as a jagged edge). An auto-hide
    // taskbar takes its ears away with it when it slides out, so with one
    // the frame paints those corners and the taskbar gets no ears (its
    // bezel lines only).
    int taskbarCorners[2] = {-1, -1};
    if (layout.edge != kEdgeNone && !IsRectEmpty(&layout.taskbarRect) && !layout.autoHide) {
        TaskbarEdgeCorners(layout.edge, taskbarCorners);
    }
    // The arc is tangent to the inner edge of the one-pixel bezel ring, not
    // to the monitor's edge: the composition window already lies a pixel in
    // (the ring is its own windows); the layered frame paints the ring
    // itself and starts the arc a pixel in.
    int inset = layout.composition ? 0 : 1;
    int size = cr + inset;
    for (int corner = 0; corner < 4; corner++) {
        bool left = corner == kCornerTL || corner == kCornerBL;
        bool top = corner == kCornerTL || corner == kCornerTR;
        if (!((left ? atLeft : atRight) && (top ? atTop : atBottom))) {
            continue;
        }
        if (corner == taskbarCorners[0] || corner == taskbarCorners[1]) {
            continue;
        }
        int x = left ? 0 : width - size;
        int y = top ? 0 : height - size;
        int cx = left ? x + size : x;  // circle center: `inset` + radius in from the corner
        int cy = top ? y + size : y;
        Gdiplus::GraphicsPath ear(Gdiplus::FillModeAlternate);
        ear.AddRectangle(Gdiplus::Rect(x - 1, y - 1, size + 2, size + 2));
        ear.AddEllipse(Gdiplus::Rect(cx - cr, cy - cr, 2 * cr, 2 * cr));
        graphics.SetClip(Gdiplus::Rect(x, y, size, size));
        graphics.FillPath(&cornerBrush, &ear);
        graphics.ResetClip();
    }
}

// The bezel ring of the layered frame: one pixel of black along each of the
// window's edges that lies on a monitor edge (the taskbar's rows get their
// lines inside the taskbar, see ConfigureBezelLine). The composition frame
// can't reach the monitor's edges; its ring is four windows of its own.
void DrawBezelRing(Gdiplus::Graphics& graphics, const RECT& rect, const Layout& layout) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    const RECT& m = layout.monitorRect;
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, GetRValue(kMonitorCornerColor),
                                             GetGValue(kMonitorCornerColor),
                                             GetBValue(kMonitorCornerColor)));
    if (rect.top == m.top) {
        graphics.FillRectangle(&brush, 0, 0, width, 1);
    }
    if (rect.bottom == m.bottom) {
        graphics.FillRectangle(&brush, 0, height - 1, width, 1);
    }
    if (rect.left == m.left) {
        graphics.FillRectangle(&brush, 0, 0, 1, height);
    }
    if (rect.right == m.right) {
        graphics.FillRectangle(&brush, width - 1, 0, 1, height);
    }
}

// Everything in the frame window that doesn't follow the frame color.
void DrawStaticLayer(Gdiplus::Graphics& graphics, const Layout& layout) {
    int width = layout.overlayRect.right - layout.overlayRect.left;
    int height = layout.overlayRect.bottom - layout.overlayRect.top;

    RECT inner = layout.innerRect;
    OffsetRect(&inner, -layout.overlayRect.left, -layout.overlayRect.top);

    // Extra work-area inset strip: visually wallpaper, but it holds the
    // resize borders of maximized windows, so give it 1/255 alpha to make
    // it hit-testable (zero-alpha pixels would let the mouse through).
    int hitInset = layout.workAreaInset;
    if (hitInset > 0) {
        RECT hole = inner;
        InflateRect(&hole, -hitInset, -hitInset);
        if (hole.right > hole.left && hole.bottom > hole.top) {
            Gdiplus::GraphicsPath strip(Gdiplus::FillModeAlternate);
            AddRoundedRectPath(strip, inner, layout.radius);
            strip.AddRectangle(Gdiplus::Rect(hole.left, hole.top, hole.right - hole.left,
                                             hole.bottom - hole.top));
            Gdiplus::SolidBrush hitBrush(Gdiplus::Color(1, 0, 0, 0));
            graphics.FillPath(&hitBrush, &strip);
        }
    }

    // Monitor corners that coincide with this window's corners (the two
    // away from the taskbar; all four without one), and the bezel ring.
    DrawMonitorEars(graphics, layout.overlayRect, layout);
    if (!layout.composition) {
        DrawBezelRing(graphics, layout.overlayRect, layout);
    }
}

inline DWORD BlendOver(DWORD top, DWORD bottom);  // defined with the painting code below

// Signed distance from `p` to a rounded rectangle (negative inside) and the
// outward normal there.
void RoundedRectDistance(double px, double py, double hx, double hy, double r, double& d,
                         double& gx, double& gy) {
    double qx = std::fabs(px) - (hx - r), qy = std::fabs(py) - (hy - r);
    if (qx > 0.0 && qy > 0.0) {
        double len = std::sqrt(qx * qx + qy * qy);
        d = len - r;
        gx = (px < 0 ? -qx : qx) / len;
        gy = (py < 0 ? -qy : qy) / len;
    } else if (qx > qy) {
        d = qx - r;
        gx = px < 0 ? -1.0 : 1.0;
        gy = 0.0;
    } else {
        d = qy - r;
        gx = 0.0;
        gy = py < 0 ? -1.0 : 1.0;
    }
}

// The lit bevel: the frame's inner edge, along the rounded screen, as a
// rounded bevel of acrylic glass tilting inwards, lit from up to two fixed
// directions (one light only reaches the edges facing it, so a second one
// from the other side lights the rest): a Blinn-Phong highlight per light,
// a Fresnel rim as white over the frame, an edge line in its own color on
// top, an edge shade as black under them, blended into the static layer
// under the ears. Geometry only, so it is rasterized once with the frame:
// the lighting doesn't move with the screen content. The bevel is a dome
// along the edge (`curve` is 1 at the edge and falls to 0 where the acrylic
// glass goes flat). Under the taskbar the frame's inner edge is the
// taskbar's inner edge, so the bevel runs along it there as well, seen
// through the taskbar's transparent background.
void DrawBevelLighting(const Layout& layout, const std::vector<BYTE>& coverage,
                       std::vector<DWORD>& staticLayer) {
    if (layout.bevelWidth <= 0) {
        return;
    }
    int width = layout.overlayRect.right - layout.overlayRect.left;
    int height = layout.overlayRect.bottom - layout.overlayRect.top;
    RECT inner = layout.innerRect;
    OffsetRect(&inner, -layout.overlayRect.left, -layout.overlayRect.top);
    double icx = (inner.left + inner.right) / 2.0, icy = (inner.top + inner.bottom) / 2.0;
    double ihx = (inner.right - inner.left) / 2.0, ihy = (inner.bottom - inner.top) / 2.0;
    double ir = layout.radius;
    double bevelIn = layout.bevelWidth;
    double z = g_settings.bevelCurvature / 10.0;
    double specular = g_settings.specular / 100.0, fresnel = g_settings.fresnel / 100.0;
    double edgeLine = g_settings.edgeLine / 100.0, edgeShade = g_settings.edgeShade / 100.0;
    double shininess = g_settings.shininess;

    // The lights' half vectors (light + viewer, normalized) and strengths.
    struct Light {
        double hx, hy, hz, strength;
    } lights[2];
    int lightCount = 0;
    auto addLight = [&](int degrees, double strength) {
        if (strength <= 0.0) {
            return;
        }
        double angle = degrees * 3.14159265358979 / 180.0;
        double lx = std::cos(angle), ly = -std::sin(angle), lz = 1.0;
        double ll = std::sqrt(lx * lx + ly * ly + lz * lz);
        double hx = lx / ll, hy = ly / ll, hz = lz / ll + 1.0;
        double hl = std::sqrt(hx * hx + hy * hy + hz * hz);
        lights[lightCount++] = {hx / hl, hy / hl, hz / hl, strength};
    };
    addLight(g_settings.lightAngle, g_settings.light / 100.0);
    addLight(g_settings.light2Angle, g_settings.light2 / 100.0);

    // The bevel's contribution at distance `d` from its edge, with `tx,ty`
    // the direction its surface tilts towards.
    auto bevel = [&](double d, double widthPx, double tx, double ty, double& light,
                     double& shade, double& line) {
        double t = std::clamp(d / widthPx, 0.0, 1.0);
        double curve = std::pow(1.0 - t, z);
        double nx = tx * curve, ny = ty * curve, nz = 1.0 - 0.7 * curve;
        double nl = std::sqrt(nx * nx + ny * ny + nz * nz);
        nx /= nl, ny /= nl, nz /= nl;
        double spec = 0.0;
        for (int i = 0; i < lightCount; i++) {
            double ndoth = std::max(0.0, nx * lights[i].hx + ny * lights[i].hy + nz * lights[i].hz);
            spec += std::pow(ndoth, shininess) * lights[i].strength;
        }
        spec *= specular;
        double fres = std::pow(1.0 - std::clamp(nz, 0.0, 1.0), 3.0) * fresnel;
        double s = std::clamp(d / 1.5, 0.0, 1.0);
        line += (1.0 - s * s * (3.0 - 2.0 * s)) * edgeLine;
        light += spec + fres;
        shade += (1.0 - t) * (1.0 - t) * edgeShade;
    };
    // The edge line's color, premultiplied per alpha below.
    unsigned lineR = GetRValue(g_settings.edgeLineColor), lineG = GetGValue(g_settings.edgeLineColor),
             lineB = GetBValue(g_settings.edgeLineColor);

    int reachIn = layout.bevelWidth + 3;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t i = static_cast<size_t>(y) * width + x;
            unsigned c = coverage[i];
            if (!c) {
                continue;
            }
            double d, gx, gy;
            RoundedRectDistance(x + 0.5 - icx, y + 0.5 - icy, ihx, ihy, ir, d, gx, gy);
            if (d > reachIn) {
                continue;
            }
            double light = 0.0, shade = 0.0, line = 0.0;
            bevel(d, bevelIn, -gx, -gy, light, shade, line);  // tilts inward, to the screen
            light = std::clamp(light, 0.0, 1.0) * c / 255.0;
            shade = std::clamp(shade, 0.0, 1.0) * c / 255.0;
            line = std::clamp(line, 0.0, 1.0) * c / 255.0;
            DWORD pixel = 0;
            if (shade > 0.0) {
                unsigned a = static_cast<unsigned>(shade * 255.0 + 0.5);
                pixel = a << 24;  // black
            }
            if (light > 0.0) {
                unsigned a = static_cast<unsigned>(light * 255.0 + 0.5);
                DWORD white = (a << 24) | (a << 16) | (a << 8) | a;
                pixel = pixel ? BlendOver(white, pixel) : white;
            }
            if (line > 0.0) {
                // The line lies on top of the lighting, in its own color.
                unsigned a = static_cast<unsigned>(line * 255.0 + 0.5);
                DWORD colored = (a << 24) | ((lineR * a / 255) << 16) | ((lineG * a / 255) << 8) |
                                (lineB * a / 255);
                pixel = pixel ? BlendOver(colored, pixel) : colored;
            }
            if (!pixel) {
                continue;
            }
            DWORD fixed = staticLayer[i];
            staticLayer[i] = fixed ? BlendOver(fixed, pixel) : pixel;
        }
    }
}

// Allocates the persistent top-down 32-bit DIB a layered window is updated
// from.
bool AllocateSurfaceBitmap(FrameSurface& surface) {
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = surface.width;
    bmi.bmiHeader.biHeight = -surface.height;  // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    surface.dib = CreateDIBSection(screenDc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (surface.dib && bits) {
        surface.memDc = CreateCompatibleDC(screenDc);
        surface.oldBitmap = SelectObject(surface.memDc, surface.dib);
        surface.pixels = static_cast<DWORD*>(bits);
    }
    ReleaseDC(nullptr, screenDc);
    if (!surface.memDc) {
        Wh_Log(L"CreateDIBSection failed: %u", GetLastError());
        return false;
    }
    return true;
}

// Rasterizes the frame's shape and static layer once and, for a layered
// frame, allocates the persistent DIB the window is updated from (the
// acrylic frame uploads the shape to composition surfaces instead).
std::unique_ptr<FrameSurface> BuildFrameSurface(const Layout& layout, bool withBitmap) {
    const RECT& rect = layout.overlayRect;
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    if (width <= 0 || height <= 0) {
        return nullptr;
    }

    auto surface = std::make_unique<FrameSurface>();
    surface->width = width;
    surface->height = height;

    std::vector<DWORD> shapePixels;
    if (!RenderToPixels(width, height, shapePixels, DrawFrameShape, layout) ||
        !RenderToPixels(width, height, surface->staticLayer, DrawStaticLayer, layout)) {
        Wh_Log(L"frame rasterization failed");
        return nullptr;
    }
    surface->coverage.resize(shapePixels.size());
    for (size_t i = 0; i < shapePixels.size(); i++) {
        surface->coverage[i] = static_cast<BYTE>(shapePixels[i] >> 24);
    }
    DrawBevelLighting(layout, surface->coverage, surface->staticLayer);

    if (withBitmap && !AllocateSurfaceBitmap(*surface)) {
        return nullptr;
    }
    return surface;
}

// ---- Inner shadow -----------------------------------------------------------
// The shadow is a Gaussian blur of everything outside the rounded screen
// (shifted by the offset), kept inside the rounded screen - a CSS inset
// box-shadow. The mask is a rounded rectangle, so its horizontal blur has a
// closed form per row (the kernel's cumulative sum over the row's inside
// interval); only the vertical pass sums taps, and only over the band along
// the edges where the shadow is non-zero.

// The band along the edges of the rounded screen where the shadow can be
// non-zero (scaled px): the kernel's reach, the offset and the corner
// radius.
int ShadowBand(const Layout& layout) {
    double sigma = layout.shadowBlur / 2.0;
    int kr = static_cast<int>(std::ceil(3.0 * sigma));
    return kr + std::max(std::abs(layout.shadowOffsetX), std::abs(layout.shadowOffsetY)) +
           layout.radius + 2;
}

// The four slices: top and bottom rows across the whole width, left and
// right columns between them; on a screen the band fills, one slice is the
// whole rounded screen. Empty rects for slices not needed.
void ShadowSliceRects(const Layout& layout, RECT out[kShadowSlices]) {
    for (int i = 0; i < kShadowSlices; i++) {
        out[i] = RECT{};
    }
    const RECT& r = layout.innerRect;
    int width = r.right - r.left, height = r.bottom - r.top;
    if (width <= 0 || height <= 0 || layout.shadowBlur <= 0) {
        return;
    }
    int band = ShadowBand(layout);
    if (2 * band >= height || 2 * band >= width) {
        out[0] = r;
        return;
    }
    out[0] = RECT{r.left, r.top, r.right, r.top + band};
    out[1] = RECT{r.left, r.bottom - band, r.right, r.bottom};
    out[2] = RECT{r.left, r.top + band, r.left + band, r.bottom - band};
    out[3] = RECT{r.right - band, r.top + band, r.right, r.bottom - band};
}

// The rounded screen's anti-aliased "inside" coverage for one slice, in the
// slice's own coordinates (the alpha the shadow is clipped to).
bool RenderInsideMask(const Layout& layout, const RECT& slice, std::vector<DWORD>& out) {
    int width = slice.right - slice.left, height = slice.bottom - slice.top;
    out.assign(static_cast<size_t>(width) * height, 0);
    Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppPARGB,
                           reinterpret_cast<BYTE*>(out.data()));
    Gdiplus::Graphics graphics(&bitmap);
    if (graphics.GetLastStatus() != Gdiplus::Ok) {
        return false;
    }
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    graphics.TranslateTransform(static_cast<Gdiplus::REAL>(layout.innerRect.left - slice.left),
                                static_cast<Gdiplus::REAL>(layout.innerRect.top - slice.top));
    RECT inner = layout.innerRect;
    OffsetRect(&inner, -layout.innerRect.left, -layout.innerRect.top);
    Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
    AddRoundedRectPath(path, inner, layout.radius);
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
    graphics.FillPath(&brush, &path);
    graphics.Flush(Gdiplus::FlushIntentionSync);
    return true;
}

// Builds the shadow's pixels for every slice of the layout (the slices'
// rects are set here too). Each slice is computed on its own: the
// horizontal blur has the closed form above, the vertical pass sums taps
// over the slice plus the kernel's reach above and below it.
bool BuildShadowSurfaces(const Layout& layout, ShadowSlice slices[kShadowSlices]) {
    RECT rects[kShadowSlices];
    ShadowSliceRects(layout, rects);
    for (int i = 0; i < kShadowSlices; i++) {
        slices[i].rect = rects[i];
        slices[i].surface.reset();
    }
    if (layout.shadowBlur <= 0 || IsRectEmpty(&rects[0])) {
        return false;
    }

    // Gaussian kernel: the blur setting is the visible extent (~2 sigma).
    double sigma = layout.shadowBlur / 2.0;
    int kr = static_cast<int>(std::ceil(3.0 * sigma));
    std::vector<float> kernel(2 * kr + 1);
    double sum = 0.0;
    for (int k = -kr; k <= kr; k++) {
        double v = std::exp(-(k * k) / (2.0 * sigma * sigma));
        kernel[k + kr] = static_cast<float>(v);
        sum += v;
    }
    std::vector<float> cumulative(kernel.size() + 1, 0.0f);  // cumulative[i] = sum of kernel[0..i)
    for (size_t i = 0; i < kernel.size(); i++) {
        kernel[i] = static_cast<float>(kernel[i] / sum);
        cumulative[i + 1] = cumulative[i] + kernel[i];
    }

    // The occluder: everything outside the rounded screen shifted by the
    // offset. Its horizontal blur at pixel x of row y (rounded-screen
    // coordinates) is 1 minus the kernel mass that falls inside the row's
    // inside interval [xl, xr).
    int width = layout.innerRect.right - layout.innerRect.left;
    int height = layout.innerRect.bottom - layout.innerRect.top;
    double left = layout.shadowOffsetX, top = layout.shadowOffsetY;
    double right = left + width, bottom = top + height;
    double r = layout.radius;
    auto blurredRow = [&](int y, int x) -> float {
        double yc = y + 0.5;
        if (yc < top || yc >= bottom) {
            return 1.0f;  // the row lies entirely outside: all occluder
        }
        double inset = 0.0;
        if (r > 0.0) {
            double t = yc < top + r ? top + r - yc : (yc > bottom - r ? yc - (bottom - r) : 0.0);
            if (t > 0.0) {
                inset = r - std::sqrt(std::max(r * r - t * t, 0.0));
            }
        }
        double xl = left + inset, xr = right - inset;
        // Taps k with x + k + 0.5 in [xl, xr): k in [ceil(xl - 0.5 - x), ceil(xr - 0.5 - x))
        int ka = std::clamp(static_cast<int>(std::ceil(xl - 0.5 - x)) + kr, 0, 2 * kr + 1);
        int kb = std::clamp(static_cast<int>(std::ceil(xr - 0.5 - x)) + kr, 0, 2 * kr + 1);
        return 1.0f - (kb > ka ? cumulative[kb] - cumulative[ka] : 0.0f);
    };

    COLORREF rgb = g_settings.shadowColor;
    double opacity = g_settings.shadowOpacity / 100.0;
    bool any = false;
    for (int i = 0; i < kShadowSlices; i++) {
        const RECT& rc = rects[i];
        if (IsRectEmpty(&rc)) {
            continue;
        }
        int sx0 = rc.left - layout.innerRect.left, sy0 = rc.top - layout.innerRect.top;
        int sw = rc.right - rc.left, sh = rc.bottom - rc.top;

        std::vector<DWORD> inside;
        if (!RenderInsideMask(layout, rc, inside)) {
            Wh_Log(L"shadow rasterization failed");
            return any;
        }
        std::vector<float> rows(static_cast<size_t>(sh + 2 * kr) * sw);
        for (int yy = 0; yy < sh + 2 * kr; yy++) {
            float* row = &rows[static_cast<size_t>(yy) * sw];
            for (int xx = 0; xx < sw; xx++) {
                row[xx] = blurredRow(sy0 - kr + yy, sx0 + xx);
            }
        }

        auto surface = std::make_unique<FrameSurface>();
        surface->width = sw;
        surface->height = sh;
        if (!AllocateSurfaceBitmap(*surface)) {
            return any;
        }
        for (int yy = 0; yy < sh; yy++) {
            DWORD* out = &surface->pixels[static_cast<size_t>(yy) * sw];
            const DWORD* mask = &inside[static_cast<size_t>(yy) * sw];
            for (int xx = 0; xx < sw; xx++) {
                float acc = 0.0f;
                for (int k = 0; k <= 2 * kr; k++) {
                    acc += kernel[k] * rows[static_cast<size_t>(yy + k) * sw + xx];
                }
                double a = std::clamp(static_cast<double>(acc), 0.0, 1.0) * opacity *
                           (mask[xx] >> 24) / 255.0;
                unsigned alpha = static_cast<unsigned>(a * 255.0 + 0.5);
                if (alpha == 0) {
                    out[xx] = 0;
                    continue;
                }
                unsigned pr = GetRValue(rgb) * alpha / 255;
                unsigned pg = GetGValue(rgb) * alpha / 255;
                unsigned pb = GetBValue(rgb) * alpha / 255;
                out[xx] = (alpha << 24) | (pr << 16) | (pg << 8) | pb;
            }
        }
        slices[i].surface = std::move(surface);
        any = true;
    }
    return any;
}

// Premultiplied "source over": `top` over `bottom`. Two channels per multiply;
// the >> 8 (instead of / 255) keeps the lanes separate and is only used on
// anti-aliased edge pixels of the static layer.
inline DWORD BlendOver(DWORD top, DWORD bottom) {
    unsigned inv = 255 - (top >> 24);
    DWORD rb = (((bottom & 0x00FF00FF) * inv) >> 8) & 0x00FF00FF;
    DWORD ag = ((((bottom >> 8) & 0x00FF00FF) * inv) >> 8) & 0x00FF00FF;
    return top + (rb | (ag << 8));
}

// ---------------------------------------------------------------------------
// Acrylic: the frame as a composition window
// ---------------------------------------------------------------------------
// With composition available the frame on each monitor is a
// Windows.UI.Composition window (WS_EX_NOREDIRECTIONBITMAP) rather than a
// layered bitmap - "the acrylic frame" throughout this code, from the fill
// it was built for, though it carries either fill: the bands show the
// blurred backdrop (the acrylic fills) or the frame color (the solid fill,
// one sprite). It lies under the taskbar as well - the taskbar's own
// background is made transparent, so the frame's surface shows through it
// and the four sides are one surface without a seam - and shows, through a
// mask of the frame's shape, a HostBackdropBrush: what is behind the window,
// blurred live by DWM, then saturated and tinted. A second sprite holds
// what doesn't follow the backdrop (the monitor corner ears, the noise
// grain, the bevel), rasterized by the same code as the layered frame, so
// both have the same geometry. The inner shadow stays a layered window,
// placed right above the frame so the backdrop never samples it. Under each band,
// an invisible window at the bottom of the z-order, repainted on window
// events, has DWM read the backdrop there again when only DWM's own
// content behind the frame changed (RefreshBackdrops).
//
// Two rules of the window manager shape the windows:
// - a topmost window whose rectangle contains a whole monitor is a
//   full-screen app to the shell, which then lowers the taskbar under it.
//   So the frame stops one pixel short of every monitor edge, and four
//   one-pixel windows paint that ring in the monitor-corner color, like
//   the bezel (the same on every monitor, with or without a taskbar).
// - an unowned window covering (almost) a monitor is refused the topmost
//   bit while it isn't in the foreground; an owned one isn't. So each frame
//   has a hidden owner window.
// The taskbar has to stay right above the frame: the frame is made topmost
// and the taskbar then raised to the top of the topmost band (the window
// manager doesn't honor inserting a window after the primary taskbar); a
// hook on the taskbar thread puts the frame back under the taskbar the
// moment the taskbar's z-order changes, and the tick checks. The taskbar is
// never *made* topmost: the shell lowers it under full-screen apps on
// purpose. Cloaking (DWM) takes the place of alpha 0 in the reveal
// handshake: a cloaked window keeps its place in the z-order, and
// uncloaking is instant.

// The composition window is layered as well (the style is part of what
// keeps a monitor-sized borderless popup from being taken for a full-screen
// app), at full alpha. It was one step short of opaque, to be a layered
// window in every respect: that looks the same, but has DWM render the
// window through an intermediate surface of the window's size, effect
// graph and all, on every frame the backdrop changes - which stuttered any
// window dragged under the frame (10.0.26100; a window a fraction of the
// size, or the backdrop without the effects, or alpha 255 each ended it).
constexpr BYTE kAcrylicWindowAlpha = 255;
// The refresher window's alpha: the least that is still a layered window
// DWM composes (1/255 of black under the frame, which no eye can see).
constexpr BYTE kRefreshAlpha = 1;

// GUIDs spelled out: uuid.lib isn't linked, and the composition interop
// header isn't used (its interfaces are declared below, ABI only).
constexpr GUID kIidIUnknown = {
    0x00000000, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
constexpr GUID kIidIInspectable = {
    0xAF86E2E0, 0xB12D, 0x4C6A, {0x9C, 0x5A, 0xD7, 0xAA, 0x65, 0x10, 0x1E, 0x90}};
constexpr GUID kIidIGraphicsEffect = {
    0xCB51C0CE, 0x8FE6, 0x4636, {0xB2, 0x02, 0x86, 0x1F, 0xAA, 0x07, 0xD8, 0xF3}};
constexpr GUID kIidIGraphicsEffectSource = {
    0x2D8F9DDC, 0x4339, 0x4EB9, {0x92, 0x16, 0xF9, 0xDE, 0xB7, 0x56, 0x58, 0xA2}};
constexpr GUID kIidIGraphicsEffectD2D1Interop = {
    0x2FC57384, 0xA068, 0x44D7, {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};
constexpr GUID kIidIPropertyValue = {
    0x4BD682DD, 0x7554, 0x40E9, {0x9A, 0x9B, 0x82, 0x65, 0x4E, 0xDE, 0x7E, 0x62}};
constexpr GUID kIidICompositorDesktopInterop = {
    0x29E691FA, 0x4567, 0x4DCA, {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};
constexpr GUID kIidICompositorInterop = {
    0x25297D5C, 0x3AD4, 0x4C9C, {0xB5, 0xCF, 0xE3, 0x6A, 0x38, 0x51, 0x23, 0x30}};
constexpr GUID kIidICompositionDrawingSurfaceInterop = {
    0xFD04E6E3, 0xFE0C, 0x4C3C, {0xAB, 0x19, 0xA0, 0x76, 0x01, 0xA5, 0x76, 0xEE}};
constexpr GUID kIidIDXGIDevice = {
    0x54EC77FA, 0x1377, 0x44E6, {0x8C, 0x32, 0x88, 0xFD, 0x5F, 0x44, 0xC8, 0x4C}};
constexpr GUID kIidID2D1DeviceContext = {
    0xE8F7FE7A, 0x191C, 0x466D, {0xAD, 0x95, 0x97, 0x56, 0x78, 0xBD, 0xA9, 0x98}};
// Direct2D effects (d2d1effects.h). Property indices and types are the
// effects' own; blend and composite modes are the D2D1_BLEND_MODE /
// D2D1_COMPOSITE_MODE values.
constexpr GUID kClsidD2D1GaussianBlur = {
    0x1FEB6D69, 0x2FE6, 0x4AC9, {0x8C, 0x58, 0x1D, 0x7F, 0x93, 0xE7, 0xA6, 0xA5}};
constexpr GUID kClsidD2D1Saturation = {
    0x5CB2D9CF, 0x327D, 0x459F, {0xA0, 0xCE, 0x40, 0xC0, 0xB2, 0x08, 0x6B, 0xF7}};
constexpr GUID kClsidD2D1Flood = {
    0x61C23C20, 0xAE69, 0x4D8E, {0x94, 0xCF, 0x50, 0x07, 0x8D, 0xF6, 0x38, 0xF2}};
constexpr GUID kClsidD2D1Blend = {
    0x81C5B77B, 0x13F8, 0x4CDD, {0xAD, 0x20, 0xC8, 0x90, 0x54, 0x7A, 0xC6, 0x5D}};
constexpr GUID kClsidD2D1Composite = {
    0x48FC9F51, 0xF6AC, 0x48F1, {0x8B, 0x58, 0x3B, 0x28, 0xAC, 0x46, 0xF7, 0x6D}};
constexpr UINT32 kBlendModeLuminosity = 23;
constexpr UINT32 kCompositeModeSourceOver = 0;
constexpr UINT32 kGaussianBlurOptimizationBalanced = 1;
constexpr UINT32 kBorderModeHard = 1;

enum GraphicsEffectPropertyMapping {
    kMappingUnknown = 0,
    kMappingDirect = 1,
    kMappingColorToVector4 = 10,
};

struct IGraphicsEffectAbi : ::IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Name(HSTRING* name) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Name(HSTRING name) = 0;
};
struct IGraphicsEffectSourceAbi : ::IInspectable {};
struct IGraphicsEffectD2D1InteropAbi : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, GraphicsEffectPropertyMapping* mapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) = 0;  // IPropertyValue*
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) = 0;  // IGraphicsEffectSource*
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) = 0;
};
struct ICompositorDesktopInteropAbi : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateDesktopWindowTarget(HWND hwnd, BOOL isTopmost,
                                                                void** result) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnsureOnThread(DWORD threadId) = 0;
};
struct ICompositorInteropAbi : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateCompositionSurfaceForHandle(HANDLE swapChain,
                                                                        void** result) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCompositionSurfaceForSwapChain(::IUnknown* swapChain,
                                                                           void** result) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateGraphicsDevice(::IUnknown* renderingDevice,
                                                           void** result) = 0;
};
struct ICompositionDrawingSurfaceInteropAbi : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE BeginDraw(const RECT* updateRect, REFIID iid,
                                                void** updateObject, POINT* updateOffset) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndDraw() = 0;
    virtual HRESULT STDMETHODCALLTYPE Resize(SIZE sizePixels) = 0;
    virtual HRESULT STDMETHODCALLTYPE Scroll(const RECT* scrollRect, const RECT* clipRect,
                                             int offsetX, int offsetY) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResumeDraw() = 0;
    virtual HRESULT STDMETHODCALLTYPE SuspendDraw() = 0;
};

struct DispatcherQueueOptionsAbi {
    DWORD dwSize;
    int threadType;     // DQTYPE_THREAD_CURRENT = 2
    int apartmentType;  // DQTAT_COM_NONE = 0
};
using CreateDispatcherQueueController_t = HRESULT(WINAPI*)(DispatcherQueueOptionsAbi,
                                                           void** controller);
// d2d1.dll's D2D1CreateDevice (not declared by every d2d1_1.h).
using D2D1CreateDevice_t = HRESULT(WINAPI*)(IDXGIDevice* dxgiDevice,
                                            const void* creationProperties,  // D2D1_CREATION_PROPERTIES, unused
                                            ID2D1Device** d2dDevice);

#ifndef DWMWA_USE_HOSTBACKDROPBRUSH
#define DWMWA_USE_HOSTBACKDROPBRUSH 17
#endif

bool SameGuid(REFIID a, REFIID b) {
    return memcmp(&a, &b, sizeof(GUID)) == 0;
}

// ---- Effect graph ------------------------------------------------------------
// One Direct2D effect in a composition effect graph, described through the
// IGraphicsEffectD2D1Interop ABI (what Win2D's effect classes do, without
// Win2D). Properties are given in the effect's own order and types; sources
// are other effects or the backdrop parameter. Nothing is animated by name,
// so the graph is rebuilt when a setting changes.
struct EffectValue {
    enum Kind { kFloat, kUInt, kFloat4 };
    Kind kind;
    float f;
    UINT32 u;
    float v[4];
};

EffectValue EffectFloat(float f) {
    return {EffectValue::kFloat, f, 0, {}};
}

EffectValue EffectUInt(UINT32 u) {
    return {EffectValue::kUInt, 0.0f, u, {}};
}

// A color as the float4 (straight alpha) the Flood effect takes.
// The frame color at the frame's opacity, as a composition color.
wu::Color FillColor(COLORREF rgb) {
    return wu::Color{static_cast<uint8_t>(std::max(1, g_settings.opacity * 255 / 100)),
                     GetRValue(rgb), GetGValue(rgb), GetBValue(rgb)};
}

EffectValue EffectColor(COLORREF rgb, float alpha) {
    return {EffectValue::kFloat4,
            0.0f,
            0,
            {GetRValue(rgb) / 255.0f, GetGValue(rgb) / 255.0f, GetBValue(rgb) / 255.0f, alpha}};
}

struct D2DEffect final : IGraphicsEffectAbi,
                         IGraphicsEffectSourceAbi,
                         IGraphicsEffectD2D1InteropAbi {
    std::atomic<ULONG> refs{1};
    GUID clsid;
    winrt::hstring name;
    std::vector<EffectValue> properties;
    std::vector<wge::IGraphicsEffectSource> sources;

    D2DEffect(const GUID& id, PCWSTR effectName, std::vector<EffectValue> props,
              std::vector<wge::IGraphicsEffectSource> srcs)
        : clsid(id), name(effectName), properties(std::move(props)), sources(std::move(srcs)) {}

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) {
            return E_POINTER;
        }
        if (SameGuid(riid, kIidIUnknown) || SameGuid(riid, kIidIInspectable) ||
            SameGuid(riid, kIidIGraphicsEffect)) {
            *ppv = static_cast<IGraphicsEffectAbi*>(this);
        } else if (SameGuid(riid, kIidIGraphicsEffectSource)) {
            *ppv = static_cast<IGraphicsEffectSourceAbi*>(this);
        } else if (SameGuid(riid, kIidIGraphicsEffectD2D1Interop)) {
            *ppv = static_cast<IGraphicsEffectD2D1InteropAbi*>(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG left = --refs;
        if (left == 0) {
            delete this;
        }
        return left;
    }

    // IInspectable
    HRESULT STDMETHODCALLTYPE GetIids(ULONG* count, IID** iids) override {
        *count = 0;
        *iids = nullptr;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetRuntimeClassName(HSTRING* className) override {
        *className =
            static_cast<HSTRING>(winrt::detach_abi(winrt::hstring(L"RoundEverything.Effect")));
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetTrustLevel(TrustLevel* level) override {
        *level = BaseTrust;
        return S_OK;
    }

    // IGraphicsEffect
    HRESULT STDMETHODCALLTYPE get_Name(HSTRING* out) override try {
        winrt::copy_to_abi(name, *reinterpret_cast<void**>(out));
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE put_Name(HSTRING in) override try {
        winrt::copy_from_abi(name, in);
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) override {
        *id = clsid;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR propertyName, UINT* index, GraphicsEffectPropertyMapping* mapping) override {
        // A flood's color (its only property) is animated by name, as a
        // Windows.UI.Color turned into the effect's RGBA vector.
        if (propertyName && index && mapping && SameGuid(clsid, kClsidD2D1Flood) &&
            _wcsicmp(propertyName, L"Color") == 0) {
            *index = 0;
            *mapping = kMappingColorToVector4;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) override {
        *count = static_cast<UINT>(properties.size());
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) override try {
        if (index >= properties.size()) {
            return E_INVALIDARG;
        }
        const EffectValue& p = properties[index];
        wf::IInspectable boxed = nullptr;
        switch (p.kind) {
            case EffectValue::kFloat:
                boxed = wf::PropertyValue::CreateSingle(p.f);
                break;
            case EffectValue::kUInt:
                boxed = wf::PropertyValue::CreateUInt32(p.u);
                break;
            default:
                boxed = wf::PropertyValue::CreateSingleArray(
                    winrt::array_view<float const>(p.v, p.v + 4));
                break;
        }
        return winrt::get_unknown(boxed)->QueryInterface(kIidIPropertyValue, value);
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** out) override try {
        if (index >= sources.size()) {
            return E_INVALIDARG;
        }
        winrt::copy_to_abi(sources[index], *out);
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) override {
        *count = static_cast<UINT>(sources.size());
        return S_OK;
    }
};

wge::IGraphicsEffectSource MakeEffect(const GUID& clsid, PCWSTR name,
                                      std::vector<EffectValue> properties,
                                      std::vector<wge::IGraphicsEffectSource> sources) {
    auto* effect = new D2DEffect(clsid, name, std::move(properties), std::move(sources));
    return wge::IGraphicsEffectSource{static_cast<IGraphicsEffectSourceAbi*>(effect),
                                      winrt::take_ownership_from_abi};
}

// ---- Composition plumbing (mod UI thread) ------------------------------------

[[clang::no_destroy]] ws::DispatcherQueueController g_dispatcherController{nullptr};
winrt::event_token g_deviceReplacedToken{};
winrt::event_token g_advancedEffectsToken{};
std::atomic<bool> g_surfacesLost{false};  // the rendering device was replaced: redraw

// A Compositor needs a DispatcherQueue on its thread.
ws::DispatcherQueueController CreateDispatcherQueueOnThisThread() {
    HMODULE coreMessaging =
        LoadLibraryExW(L"CoreMessaging.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    auto create = coreMessaging
                      ? reinterpret_cast<CreateDispatcherQueueController_t>(
                            GetProcAddress(coreMessaging, "CreateDispatcherQueueController"))
                      : nullptr;
    if (!create) {
        Wh_Log(L"CreateDispatcherQueueController not found");
        return nullptr;
    }
    DispatcherQueueOptionsAbi options{sizeof(options), 2 /* DQTYPE_THREAD_CURRENT */,
                                      0 /* DQTAT_COM_NONE */};
    void* controller = nullptr;
    HRESULT hr = create(options, &controller);
    if (FAILED(hr)) {
        Wh_Log(L"CreateDispatcherQueueController failed: %08X", hr);
        return nullptr;
    }
    return ws::DispatcherQueueController{controller, winrt::take_ownership_from_abi};
}

// Pumps this thread's messages until a dispatcher-queue operation (its
// shutdown) has completed.
void PumpUntilDone(wf::IAsyncAction const& action) {
    ULONGLONG start = GetTickCount64();
    while (action.Status() == wf::AsyncStatus::Started && GetTickCount64() - start < 2000) {
        MsgWaitForMultipleObjectsEx(0, nullptr, 50, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}

// The Direct2D-backed composition graphics device the mask and overlay
// surfaces are drawn through (the pixels come from the CPU rasterization;
// Direct2D only uploads them).
bool CreateGraphicsDevice() {
    ID3D11Device* d3d = nullptr;
    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                   D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0,
                                   D3D11_SDK_VERSION, &d3d, nullptr, nullptr);
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice (hardware) failed: %08X, trying WARP", hr);
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
                               &d3d, nullptr, nullptr);
    }
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: %08X", hr);
        return false;
    }
    void* dxgiRaw = nullptr;
    ID2D1Device* d2d = nullptr;
    void* interopRaw = nullptr;
    void* deviceRaw = nullptr;
    PCWSTR step = L"IDXGIDevice";
    hr = d3d->QueryInterface(kIidIDXGIDevice, &dxgiRaw);
    if (SUCCEEDED(hr)) {
        step = L"D2D1CreateDevice";
        HMODULE d2dModule = LoadLibraryExW(L"d2d1.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        auto createDevice = d2dModule ? reinterpret_cast<D2D1CreateDevice_t>(
                                            GetProcAddress(d2dModule, "D2D1CreateDevice"))
                                      : nullptr;
        hr = createDevice ? createDevice(static_cast<IDXGIDevice*>(dxgiRaw), nullptr, &d2d)
                          : HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }
    if (SUCCEEDED(hr)) {
        step = L"ICompositorInterop";
        hr = winrt::get_unknown(g_compositor)->QueryInterface(kIidICompositorInterop, &interopRaw);
    }
    if (SUCCEEDED(hr)) {
        step = L"CreateGraphicsDevice";
        hr = static_cast<ICompositorInteropAbi*>(interopRaw)->CreateGraphicsDevice(d2d, &deviceRaw);
    }
    if (SUCCEEDED(hr)) {
        g_graphicsDevice = wuc::CompositionGraphicsDevice{deviceRaw, winrt::take_ownership_from_abi};
    }
    if (interopRaw) {
        static_cast<::IUnknown*>(interopRaw)->Release();
    }
    if (d2d) {
        d2d->Release();
    }
    if (dxgiRaw) {
        static_cast<::IUnknown*>(dxgiRaw)->Release();
    }
    d3d->Release();
    if (FAILED(hr)) {
        Wh_Log(L"%s failed: %08X", step, hr);
        return false;
    }
    // A lost device (driver update, GPU reset) empties the surfaces; they
    // are drawn again on the next tick.
    try {
        g_deviceReplacedToken = g_graphicsDevice.RenderingDeviceReplaced(
            [](wuc::CompositionGraphicsDevice const&,
               wuc::RenderingDeviceReplacedEventArgs const&) {
                Wh_Log(L"rendering device replaced: acrylic surfaces will be redrawn");
                g_surfacesLost = true;
                if (HWND ctrl = g_ctrlWnd.load()) {
                    PostMessageW(ctrl, kMsgReload, 0, 0);
                }
            });
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"RenderingDeviceReplaced subscription threw %08X", e.code().value);
    }
    return true;
}

void InitComposition() {
    // Transparency effects: the acrylic follows the system setting, like
    // Windows' own does.
    try {
        g_uiSettings = wuvm::UISettings();
        g_advancedEffects = g_uiSettings.AdvancedEffectsEnabled();
        g_advancedEffectsToken = g_uiSettings.AdvancedEffectsEnabledChanged(
            [](wuvm::UISettings const& settings, wf::IInspectable const&) {
                bool on = settings.AdvancedEffectsEnabled();
                g_advancedEffects = on;
                Wh_Log(L"transparency effects %s", on ? L"on: acrylic fill" : L"off: solid fill");
                if (HWND ctrl = g_ctrlWnd.load()) {
                    PostMessageW(ctrl, kMsgReload, 0, 0);
                }
            });
        if (!g_advancedEffects) {
            Wh_Log(L"transparency effects are off: solid fill until they are on");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"UISettings threw %08X; transparency effects assumed on", e.code().value);
        g_uiSettings = nullptr;
    }

    g_dispatcherController = CreateDispatcherQueueOnThisThread();
    if (!g_dispatcherController) {
        return;
    }
    try {
        g_compositor = wuc::Compositor();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Compositor creation threw %08X: %s", e.code().value, e.message().c_str());
        g_compositor = nullptr;
        return;
    }
    if (!CreateGraphicsDevice()) {
        g_compositor = nullptr;
        return;
    }
    Wh_Log(L"composition ready (acrylic fill available)");
}

// Everything of the composition must be released on this thread before the
// image goes away: the effect objects' vtables live in it.
void ShutdownComposition() {
    if (g_graphicsDevice) {
        try {
            if (g_deviceReplacedToken) {
                g_graphicsDevice.RenderingDeviceReplaced(g_deviceReplacedToken);
            }
        } catch (...) {
        }
        g_deviceReplacedToken = {};
        g_graphicsDevice = nullptr;
    }
    g_compositor = nullptr;
    if (g_uiSettings) {
        try {
            if (g_advancedEffectsToken) {
                g_uiSettings.AdvancedEffectsEnabledChanged(g_advancedEffectsToken);
            }
        } catch (...) {
        }
        g_advancedEffectsToken = {};
        g_uiSettings = nullptr;
    }
    // The queue is shut down before its thread ends; its shutdown work (and
    // the compositor's) is pumped here (a tick that might come by is a
    // no-op: the mod is unloading).
    if (g_dispatcherController) {
        try {
            PumpUntilDone(g_dispatcherController.ShutdownQueueAsync());
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"ShutdownQueueAsync threw %08X", e.code().value);
        }
        g_dispatcherController = nullptr;
    }
}

// Uploads premultiplied BGRA pixels into a drawing surface of the same size.
bool UploadPixels(wuc::CompositionDrawingSurface const& surface, int width, int height,
                  const DWORD* pixels, PCWSTR what) {
    void* interopRaw = nullptr;
    HRESULT hr = winrt::get_unknown(surface)->QueryInterface(kIidICompositionDrawingSurfaceInterop,
                                                             &interopRaw);
    if (FAILED(hr)) {
        Wh_Log(L"%s surface: ICompositionDrawingSurfaceInterop %08X", what, hr);
        return false;
    }
    auto* interop = static_cast<ICompositionDrawingSurfaceInteropAbi*>(interopRaw);
    void* dcRaw = nullptr;
    POINT offset{};
    hr = interop->BeginDraw(nullptr, kIidID2D1DeviceContext, &dcRaw, &offset);
    if (FAILED(hr)) {
        Wh_Log(L"%s surface: BeginDraw %08X", what, hr);
        interop->Release();
        return false;
    }
    auto* dc = static_cast<ID2D1DeviceContext*>(dcRaw);
    D2D1_BITMAP_PROPERTIES1 props{};
    props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
    props.colorContext = nullptr;
    D2D1_SIZE_U size{static_cast<UINT32>(width), static_cast<UINT32>(height)};
    ID2D1Bitmap1* bitmap = nullptr;
    hr = dc->CreateBitmap(size, pixels, static_cast<UINT32>(width) * 4, &props, &bitmap);
    if (SUCCEEDED(hr)) {
        D2D1_COLOR_F clear{0.0f, 0.0f, 0.0f, 0.0f};
        dc->Clear(&clear);
        D2D1_POINT_2F target{static_cast<float>(offset.x), static_cast<float>(offset.y)};
        dc->DrawImage(bitmap, &target, nullptr, D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
                      D2D1_COMPOSITE_MODE_SOURCE_COPY);
        bitmap->Release();
    } else {
        Wh_Log(L"%s surface: CreateBitmap %08X", what, hr);
    }
    HRESULT hrEnd = interop->EndDraw();
    dc->Release();
    interop->Release();
    if (FAILED(hrEnd)) {
        Wh_Log(L"%s surface: EndDraw %08X", what, hrEnd);
        return false;
    }
    return SUCCEEDED(hr);
}

wuc::CompositionDrawingSurface CreateSurface(int width, int height, const DWORD* pixels,
                                             PCWSTR what) {
    try {
        auto surface = g_graphicsDevice.CreateDrawingSurface(
            {static_cast<float>(width), static_cast<float>(height)},
            wgd::DirectXPixelFormat::B8G8R8A8UIntNormalized, wgd::DirectXAlphaMode::Premultiplied);
        if (!UploadPixels(surface, width, height, pixels, what)) {
            return nullptr;
        }
        return surface;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"%s surface (%dx%d) threw %08X: %s", what, width, height, e.code().value,
               e.message().c_str());
        return nullptr;
    }
}

// ---- Z-order -----------------------------------------------------------------
// The windows of one acrylic frame, as the placement code sees them.
struct FollowEntry {
    HWND taskbar = nullptr;
    HWND frame = nullptr;
    HWND shadow[kShadowSlices] = {};
    HWND strips[4] = {};
};

bool IsTopmost(HWND hwnd) {
    return (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
}

// Whether `above` precedes `below` in the z-order.
bool WindowAbove(HWND above, HWND below) {
    for (HWND h = GetWindow(above, GW_HWNDNEXT); h; h = GetWindow(h, GW_HWNDNEXT)) {
        if (h == below) {
            return true;
        }
    }
    return false;
}

// Whether the shell has taken the taskbar off the top of the z-order: what
// it does to a monitor's taskbar while it sees a full-screen app there -
// including one the mod's own detection leaves alone, such as a screen
// recorder's layered, click-through overlay. The frame then can't stay
// topmost (it would sit over the taskbar's icons): it goes right under the
// taskbar wherever the taskbar is, and back on top with it.
bool TaskbarDemoted(HWND taskbar) {
    return taskbar && IsWindowVisible(taskbar) && !IsTopmost(taskbar);
}

// UI thread. Frame topmost, the shadow right above it, the bezel ring right
// below it, and the taskbar - when it is topmost; it is never made so here
// - on top of them all. The mod's own windows are placed synchronously; the
// taskbar, which belongs to another thread, asynchronously and last, so
// this thread never waits on the taskbar thread and the order still ends
// with the taskbar on top. With the taskbar demoted (see TaskbarDemoted)
// the same stack is built right under it instead, off the top as well: a
// window inserted after a non-topmost one loses the topmost bit.
void PlaceAcrylicOrder(const FollowEntry& e, UINT flags) {
    if (TaskbarDemoted(e.taskbar)) {
        SetWindowPos(e.frame, e.taskbar, 0, 0, 0, 0, flags);
        for (HWND shadow : e.shadow) {
            if (shadow) {
                SetWindowPos(shadow, e.taskbar, 0, 0, 0, 0, flags);  // between taskbar and frame
            }
        }
        for (HWND strip : e.strips) {
            if (strip) {
                SetWindowPos(strip, e.frame, 0, 0, 0, 0, flags);
            }
        }
        return;
    }
    SetWindowPos(e.frame, HWND_TOPMOST, 0, 0, 0, 0, flags);
    for (HWND shadow : e.shadow) {
        if (shadow) {
            SetWindowPos(shadow, HWND_TOPMOST, 0, 0, 0, 0, flags);
        }
    }
    for (HWND strip : e.strips) {
        if (strip) {
            SetWindowPos(strip, e.frame, 0, 0, 0, 0, flags);
        }
    }
    if (e.taskbar && IsTopmost(e.taskbar)) {
        SetWindowPos(e.taskbar, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING |
                         SWP_ASYNCWINDOWPOS);
    }
}

// The frame lost its topmost bit, got above its (topmost) taskbar, or got
// above its shadow. With the taskbar demoted: the frame is still topmost,
// or no longer right under the taskbar.
bool AcrylicOrderBroken(const FollowEntry& e) {
    if (TaskbarDemoted(e.taskbar)) {
        if (IsTopmost(e.frame) || !WindowAbove(e.taskbar, e.frame)) {
            return true;
        }
        for (HWND shadow : e.shadow) {
            if (shadow && IsWindowVisible(shadow) && !WindowAbove(shadow, e.frame)) {
                return true;
            }
        }
        return false;
    }
    if (!IsTopmost(e.frame)) {
        return true;
    }
    if (e.taskbar && IsTopmost(e.taskbar) && WindowAbove(e.frame, e.taskbar)) {
        return true;
    }
    for (HWND shadow : e.shadow) {
        if (shadow && IsWindowVisible(shadow) && !WindowAbove(shadow, e.frame)) {
            return true;
        }
    }
    return false;
}

FollowEntry FollowEntryFor(const Overlay& overlay) {
    FollowEntry e;
    e.taskbar = overlay.layout.taskbar;
    e.frame = overlay.hwnd;
    for (int i = 0; i < kShadowSlices; i++) {
        e.shadow[i] = overlay.shadow[i].hwnd;
    }
    if (overlay.comp) {
        for (int i = 0; i < 4; i++) {
            e.strips[i] = overlay.comp->strips[i];
        }
    }
    return e;
}

// ---- Windows -----------------------------------------------------------------

bool PresentFrame(HWND hwnd, HDC memDc, int width, int height, const RECT* rect,
                  BYTE alpha);  // defined with the painting code below
void ApplyFrameMouseStyle(HWND hwnd);  // defined with the overlay code
void SyncRefreshers(Overlay& overlay);  // defined with the backdrop refresh code

// One row or column of the bezel ring: a layered window painted opaque in
// the monitor-corner color, click-through (on the taskbar's edge that row
// is the taskbar's own outermost pixel row), cloaked until the frame shows.
HWND CreateBezelStrip(HWND owner, const RECT& rc, COLORREF color) {
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0) {
        return nullptr;
    }
    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                                    WS_EX_NOACTIVATE,
                                kFrameClassName, L"Round Everything Bezel", WS_POPUP, rc.left,
                                rc.top, width, height, owner, nullptr, g_module, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW (bezel) failed: %u", GetLastError());
        return nullptr;
    }
    BOOL excludedFromPeek = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_EXCLUDED_FROM_PEEK, &excludedFromPeek,
                          sizeof(excludedFromPeek));
    BOOL noTransitions = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noTransitions,
                          sizeof(noTransitions));
    CloakWindow(hwnd, true);
    FrameSurface pixels;
    pixels.width = width;
    pixels.height = height;
    if (AllocateSurfaceBitmap(pixels)) {
        DWORD opaque = 0xFF000000u | (static_cast<DWORD>(GetRValue(color)) << 16) |
                       (static_cast<DWORD>(GetGValue(color)) << 8) | GetBValue(color);
        for (int i = 0; i < width * height; i++) {
            pixels.pixels[i] = opaque;
        }
        PresentFrame(hwnd, pixels.memDc, width, height, &rc, 255);
    }
    return hwnd;
}

void DestroyBezelStrips(CompositionFrame& ac) {
    for (HWND& strip : ac.strips) {
        if (strip) {
            DestroyWindow(strip);
            strip = nullptr;
        }
    }
}

// The ring: top and bottom rows across the whole monitor, left and right
// columns between them. One window per side - a single ring-shaped window
// would have a monitor-sized rectangle and count as full-screen. It depends
// on the monitor's rectangle alone (its color is fixed), so a re-shaped frame
// (a settings change, the taskbar moving) keeps the ring it has: new strips
// start cloaked and only show once the frame is raised again, and in between
// the screen's outermost pixels would show whatever lies under them.
void CreateBezelStrips(Overlay& overlay) {
    CompositionFrame& ac = *overlay.comp;
    const RECT& m = overlay.layout.monitorRect;
    bool complete = true;
    for (HWND strip : ac.strips) {
        complete = complete && strip && IsWindow(strip);
    }
    if (complete && EqualRect(&ac.ringRect, &m)) {
        return;
    }
    DestroyBezelStrips(ac);
    ac.ringRect = m;
    RECT rows[4] = {
        {m.left, m.top, m.right, m.top + 1},
        {m.left, m.bottom - 1, m.right, m.bottom},
        {m.left, m.top + 1, m.left + 1, m.bottom - 1},
        {m.right - 1, m.top + 1, m.right, m.bottom - 1},
    };
    for (int i = 0; i < 4; i++) {
        ac.strips[i] = CreateBezelStrip(ac.owner, rows[i], kMonitorCornerColor);
    }
}

// Hit-test (and DWM clip) region from the frame's rasterized shape: every
// pixel the layered frame would paint - its anti-aliased coverage plus the
// static layer (ears, the hit-testable work-area inset strip) - so the mouse
// meets the acrylic frame exactly where it meets the layered one.
HRGN RegionFromSurface(const FrameSurface& surface) {
    int width = surface.width, height = surface.height;
    std::vector<RECT> rects;
    for (int y = 0; y < height; y++) {
        const BYTE* coverage = &surface.coverage[static_cast<size_t>(y) * width];
        const DWORD* fixed = &surface.staticLayer[static_cast<size_t>(y) * width];
        int runStart = -1;
        for (int x = 0; x <= width; x++) {
            bool on = x < width && (coverage[x] != 0 || (fixed[x] >> 24) != 0);
            if (on && runStart < 0) {
                runStart = x;
            } else if (!on && runStart >= 0) {
                rects.push_back({runStart, y, x, y + 1});
                runStart = -1;
            }
        }
    }
    if (rects.empty()) {
        return CreateRectRgn(0, 0, 0, 0);
    }
    std::vector<BYTE> data(sizeof(RGNDATAHEADER) + rects.size() * sizeof(RECT));
    auto* header = reinterpret_cast<RGNDATAHEADER*>(data.data());
    header->dwSize = sizeof(RGNDATAHEADER);
    header->iType = RDH_RECTANGLES;
    header->nCount = static_cast<DWORD>(rects.size());
    header->nRgnSize = 0;
    header->rcBound = RECT{0, 0, width, height};
    memcpy(data.data() + sizeof(RGNDATAHEADER), rects.data(), rects.size() * sizeof(RECT));
    HRGN region = ExtCreateRegion(nullptr, static_cast<DWORD>(data.size()),
                                  reinterpret_cast<RGNDATA*>(data.data()));
    if (!region) {
        Wh_Log(L"ExtCreateRegion (%zu rects) failed: %u", rects.size(), GetLastError());
        region = CreateRectRgn(0, 0, width, height);
    }
    return region;
}

void ApplyAcrylicRegion(Overlay& overlay) {
    HRGN region = RegionFromSurface(*overlay.surface);
    SetWindowRgn(overlay.hwnd, region, FALSE);  // the system owns the region now
}

// Creates the acrylic frame's windows (owner, frame, bezel ring) and its
// composition target. The frame is created hidden and cloaked; its content
// is built by PaintOverlay.
bool CreateAcrylicFrame(Overlay& overlay) {
    auto ac = std::make_unique<CompositionFrame>();
    ac->owner = CreateWindowExW(WS_EX_TOOLWINDOW, kFrameClassName, L"", WS_POPUP, 0, 0, 0, 0,
                                nullptr, nullptr, g_module, nullptr);
    if (!ac->owner) {
        Wh_Log(L"CreateWindowExW (owner) failed: %u", GetLastError());
        return false;
    }
    const RECT& rc = overlay.layout.overlayRect;
    DWORD exStyle = WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    HWND hwnd = CreateWindowExW(exStyle, kFrameClassName, L"Round Everything", WS_POPUP, rc.left,
                                rc.top, rc.right - rc.left, rc.bottom - rc.top, ac->owner, nullptr,
                                g_module, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW (acrylic frame) failed: %u", GetLastError());
        DestroyWindow(ac->owner);
        return false;
    }
    SetLayeredWindowAttributes(hwnd, 0, kAcrylicWindowAlpha, LWA_ALPHA);
    // DWMWA_USE_HOSTBACKDROPBRUSH is set with the content, when the fill
    // shows the backdrop.
    BOOL excludedFromPeek = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_EXCLUDED_FROM_PEEK, &excludedFromPeek,
                          sizeof(excludedFromPeek));
    BOOL noTransitions = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noTransitions,
                          sizeof(noTransitions));
    CloakWindow(hwnd, true);
    ApplyFrameMouseStyle(hwnd);

    void* interopRaw = nullptr;
    HRESULT hr = winrt::get_unknown(g_compositor)->QueryInterface(kIidICompositorDesktopInterop,
                                                          &interopRaw);
    void* targetRaw = nullptr;
    if (SUCCEEDED(hr)) {
        hr = static_cast<ICompositorDesktopInteropAbi*>(interopRaw)->CreateDesktopWindowTarget(
            hwnd, FALSE, &targetRaw);
        static_cast<::IUnknown*>(interopRaw)->Release();
    }
    if (FAILED(hr)) {
        Wh_Log(L"CreateDesktopWindowTarget: %08X", hr);
        DestroyWindow(hwnd);
        DestroyWindow(ac->owner);
        return false;
    }
    try {
        ac->target = wucd::DesktopWindowTarget{targetRaw, winrt::take_ownership_from_abi};
        ac->root = g_compositor.CreateContainerVisual();
        ac->target.Root(ac->root);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"composition target setup threw %08X: %s", e.code().value, e.message().c_str());
        ac->root = nullptr;
        ac->target = nullptr;
        DestroyWindow(hwnd);
        DestroyWindow(ac->owner);
        return false;
    }
    overlay.hwnd = hwnd;
    overlay.comp = std::move(ac);
    CreateBezelStrips(overlay);
    Wh_Log(L"monitor %p: acrylic frame %p (%ld,%ld,%ld,%ld), owner %p", overlay.layout.monitor,
           hwnd, rc.left, rc.top, rc.right, rc.bottom, overlay.comp->owner);
    return true;
}

// Moves a re-shaped acrylic frame to its new rectangle (the content is
// rebuilt by the next PaintOverlay); the bezel ring is rebuilt only if the
// monitor's rectangle changed.
void PlaceAcrylicFrame(Overlay& overlay) {
    const RECT& rc = overlay.layout.overlayRect;
    SetWindowPos(overlay.hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    overlay.comp->built = false;  // the refreshers follow the rebuilt bands
    CreateBezelStrips(overlay);
}

void DestroyAcrylic(Overlay& overlay) {
    CompositionFrame& ac = *overlay.comp;
    try {
        if (ac.target) {
            ac.target.Root(nullptr);
        }
    } catch (...) {
    }
    ac.effectBrush = nullptr;
    ac.colorBrush = nullptr;
    ac.gradientBrush = nullptr;
    ac.gradientStops.clear();
    ac.gradientFactory = nullptr;
    ac.factories.clear();
    ac.hostBackdrop = nullptr;
    ac.maskSurface = nullptr;
    ac.overlaySurface = nullptr;
    ac.root = nullptr;
    ac.target = nullptr;
    DestroyBezelStrips(ac);
    for (HWND& refresher : ac.refreshers) {
        if (refresher) {
            DestroyWindow(refresher);
            refresher = nullptr;
        }
    }
    if (overlay.hwnd) {
        DestroyWindow(overlay.hwnd);
        overlay.hwnd = nullptr;
    }
    if (ac.owner) {
        DestroyWindow(ac.owner);
        ac.owner = nullptr;
    }
    overlay.comp.reset();
}

// Cloaked = off the screen (alpha 0's counterpart for the composition
// window and its ring). Uncloaking from the taskbar thread is what reveals
// the frame in the handshake.
void SetAcrylicCloaked(const Overlay& overlay, bool cloak) {
    if (!cloak) {
        for (HWND strip : overlay.comp->strips) {
            if (strip) {
                CloakWindow(strip, false);
            }
        }
    }
    CloakWindow(overlay.hwnd, cloak);
    if (cloak) {
        for (HWND strip : overlay.comp->strips) {
            if (strip) {
                CloakWindow(strip, true);
            }
        }
    }
}

// ---- Content -----------------------------------------------------------------

// The color a gradient's stop shows: its own while the frame shows the
// frame color, the one color (a title bar's) otherwise.
COLORREF StopColor(size_t i, COLORREF rgb) {
    return rgb == g_settings.customColor && i < g_settings.fillColors.size()
               ? g_settings.fillColors[i]
               : rgb;
}

bool FrameColorIsGradient() {
    return g_settings.fillColors.size() >= 2;
}

// A linear gradient of the frame color's colors across the whole frame
// window (`width` x `height`) at the angle set, one stop per color, evenly
// spaced, the points stretched so the gradient reaches the window's corners
// at any angle - in the coordinates of a sprite at `origin` in the window,
// so the bands, each a sprite of its own, show one gradient together. The
// stops are kept on the frame (for the color animations) when `ac` is given.
wuc::CompositionLinearGradientBrush MakeWindowGradient(CompositionFrame* ac, COLORREF rgb,
                                                       uint8_t alpha, float width, float height,
                                                       wf::Numerics::float2 origin) {
    auto brush = g_compositor.CreateLinearGradientBrush();
    brush.MappingMode(wuc::CompositionMappingMode::Absolute);
    double a = g_settings.gradientAngle * 3.14159265358979 / 180.0;
    double dx = std::cos(a), dy = std::sin(a);
    double scale = 0.5 / std::max(std::abs(dx), std::abs(dy));
    dx *= scale;
    dy *= scale;
    brush.StartPoint({static_cast<float>((0.5 - dx) * width) - origin.x,
                      static_cast<float>((0.5 - dy) * height) - origin.y});
    brush.EndPoint({static_cast<float>((0.5 + dx) * width) - origin.x,
                    static_cast<float>((0.5 + dy) * height) - origin.y});
    const std::vector<COLORREF>& colors = g_settings.fillColors;
    for (size_t i = 0; i < colors.size(); i++) {
        float offset = colors.size() > 1 ? static_cast<float>(i) / (colors.size() - 1) : 0.0f;
        COLORREF c = StopColor(i, rgb);
        auto stop = g_compositor.CreateColorGradientStop(
            offset, wu::Color{alpha, GetRValue(c), GetGValue(c), GetBValue(c)});
        brush.ColorStops().Append(stop);
        if (ac) {
            ac->gradientStops.push_back({stop, i, alpha});
        }
    }
    return brush;
}

uint8_t OpacityAlpha(int percent) {
    return static_cast<uint8_t>(std::clamp(percent, 0, 100) * 255 / 100);
}

// A gradient frame color for the solid fill, the window-sized sprite's
// brush. False when there is no gradient (one color) or it couldn't be made.
bool BuildGradientBrush(CompositionFrame& ac, COLORREF rgb, float width, float height) {
    if (!FrameColorIsGradient()) {
        return false;
    }
    try {
        ac.gradientBrush =
            MakeWindowGradient(&ac, rgb, static_cast<uint8_t>(std::max(1, g_settings.opacity * 255 / 100)),
                               width, height, {0.0f, 0.0f});
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"gradient brush threw %08X: the average color instead", e.code().value);
        ac.gradientBrush = nullptr;
        ac.gradientStops.clear();
        return false;
    }
}

// One band's effect brush from the gradient graph: the backdrop, and the
// tint and luminosity gradients in the band's coordinates.
wuc::CompositionEffectBrush MakeBandGradientBrush(CompositionFrame& ac, COLORREF rgb, float width,
                                                  float height, wf::Numerics::float2 origin) {
    auto brush = ac.gradientFactory.CreateBrush();
    brush.SetSourceParameter(L"backdrop", ac.hostBackdrop);
    if (g_settings.luminosityOpacity > 0) {
        brush.SetSourceParameter(L"luminosity",
                                 MakeWindowGradient(&ac, rgb, OpacityAlpha(g_settings.luminosityOpacity),
                                                    width, height, origin));
    }
    if (g_settings.tintOpacity > 0) {
        brush.SetSourceParameter(L"tint",
                                 MakeWindowGradient(&ac, rgb, OpacityAlpha(g_settings.tintOpacity),
                                                    width, height, origin));
    }
    return brush;
}

// The effect graph behind the mask: backdrop -> blur -> saturation ->
// luminosity blend with the frame color -> the frame color laid over it.
// (The backdrop can't be moved or stretched in the graph: the composition
// engine rejects any transform over a backdrop brush.) Returns the level
// built: 0 that graph; 1 the blur alone (the graph the prototype ran on -
// tried when the composition engine rejects the full one), with the tint
// then laid over it as a color sprite; -1 nothing. Only the effects that
// do something are in the graph (no blur pass at blur 0, and so on); with
// none left, `result` stays null and the bands show the backdrop itself.
int BuildAcrylicBrush(Overlay& overlay, COLORREF rgb, wuc::CompositionEffectBrush& result) {
    CompositionFrame& ac = *overlay.comp;
    const Layout& layout = overlay.layout;
    if (!ac.hostBackdrop) {
        ac.hostBackdrop = g_compositor.CreateHostBackdropBrush();
    }
    result = nullptr;
    ac.colorProperties.clear();
    ac.gradientFactory = nullptr;
    // A gradient frame color goes into the graph as two inputs (the tint and
    // the luminosity) rather than two floods; should the compositor refuse a
    // gradient as an effect input, the graph is made again with floods of
    // its average color.
    bool gradient = FrameColorIsGradient();
    for (int level = 0; level < 2; level++) {
        try {
            wge::IGraphicsEffectSource chain = wuc::CompositionEffectSourceParameter(L"backdrop");
            bool any = false;
            std::vector<winrt::hstring> animatable;
            if (layout.backdropBlur > 0.0f) {
                chain = MakeEffect(kClsidD2D1GaussianBlur, L"Blur",
                                   {EffectFloat(layout.backdropBlur),
                                    EffectUInt(kGaussianBlurOptimizationBalanced),
                                    EffectUInt(kBorderModeHard)},
                                   {chain});
                any = true;
            }
            if (level == 0) {
                if (g_settings.saturation != 100) {
                    chain = MakeEffect(kClsidD2D1Saturation, L"Saturation",
                                       {EffectFloat(g_settings.saturation / 100.0f)}, {chain});
                    any = true;
                }
                if (g_settings.luminosityOpacity > 0) {
                    // Luminosity blend: the frame color's brightness with
                    // the blurred content's hue and saturation. Direct2D's
                    // blend effect takes the *source* (whose luminosity is
                    // used) as input 0 and the backdrop (whose hue and
                    // saturation survive) as input 1 - the other way round
                    // from what the names in Win2D/WinUI suggest, which is
                    // why WinUI's acrylic code flips them too.
                    wge::IGraphicsEffectSource color{nullptr};
                    if (gradient) {
                        color = wuc::CompositionEffectSourceParameter(L"luminosity");
                    } else {
                        color = MakeEffect(
                            kClsidD2D1Flood, L"LuminosityColor",
                            {EffectColor(rgb, g_settings.luminosityOpacity / 100.0f)}, {});
                        animatable.push_back(L"LuminosityColor.Color");
                    }
                    chain = MakeEffect(kClsidD2D1Blend, L"Luminosity",
                                       {EffectUInt(kBlendModeLuminosity)}, {color, chain});
                    any = true;
                }
                if (g_settings.tintOpacity > 0) {
                    wge::IGraphicsEffectSource color{nullptr};
                    if (gradient) {
                        color = wuc::CompositionEffectSourceParameter(L"tint");
                    } else {
                        color = MakeEffect(kClsidD2D1Flood, L"TintColor",
                                           {EffectColor(rgb, g_settings.tintOpacity / 100.0f)},
                                           {});
                        animatable.push_back(L"TintColor.Color");
                    }
                    chain = MakeEffect(kClsidD2D1Composite, L"Tint",
                                       {EffectUInt(kCompositeModeSourceOver)}, {chain, color});
                    any = true;
                }
            }
            if (!any) {
                return level;  // the backdrop as it is
            }
            auto factory =
                g_compositor.CreateEffectFactory(chain.as<wge::IGraphicsEffect>(), animatable);
            // The graph loads asynchronously: Pending is the usual answer
            // here, not a failure.
            auto status = factory.LoadStatus();
            if (status == wuc::CompositionEffectFactoryLoadStatus::EffectTooComplex ||
                status == wuc::CompositionEffectFactoryLoadStatus::Other) {
                Wh_Log(L"monitor %p: effect graph (level %d) rejected: status %d, error %08X",
                       layout.monitor, level, static_cast<int>(status),
                       factory.ExtendedError().value);
                continue;
            }
            auto brush = factory.CreateBrush();
            brush.SetSourceParameter(L"backdrop", ac.hostBackdrop);
            if (gradient && level == 0) {
                // Tried here, once, with gradients that aren't kept: the
                // bands get brushes of their own (MakeBandGradientBrush).
                if (g_settings.luminosityOpacity > 0) {
                    brush.SetSourceParameter(L"luminosity",
                                             MakeWindowGradient(nullptr, rgb, 255, 1.0f, 1.0f,
                                                                {0.0f, 0.0f}));
                }
                if (g_settings.tintOpacity > 0) {
                    brush.SetSourceParameter(L"tint", MakeWindowGradient(nullptr, rgb, 255, 1.0f,
                                                                         1.0f, {0.0f, 0.0f}));
                }
                ac.gradientFactory = factory;
            }
            result = brush;
            ac.colorProperties = std::move(animatable);
            ac.factories.push_back(factory);  // its load is checked by later ticks
            if (level > 0) {
                Wh_Log(L"monitor %p: acrylic reduced to blur and tint (full graph rejected)",
                       layout.monitor);
            }
            return level;
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"monitor %p: effect graph (level %d) threw %08X: %s", layout.monitor, level,
                   e.code().value, e.message().c_str());
            if (gradient && level == 0) {
                Wh_Log(L"monitor %p: gradient as an effect input refused - its average color "
                       L"instead",
                       layout.monitor);
                gradient = false;
                ac.gradientFactory = nullptr;
                level = -1;  // the full graph again, with floods
            }
        }
    }
    return -1;
}

// The window-sized frame cut into the bands that hold its painted pixels
// (the taskbar strip, outside the rounded screen, is inside them): top and
// bottom across the whole width, left and right between them, each
// `reach` pixels past the rounded screen's edge. One band, the whole
// window, when they would overlap.
int BackdropBandRects(int width, int height, const RECT& inner, int reach, RECT out[4]) {
    int top = std::min<int>(height, inner.top + reach);
    int bottom = std::max<int>(0, inner.bottom - reach);
    int left = std::min<int>(width, inner.left + reach);
    int right = std::max<int>(0, inner.right - reach);
    if (top >= bottom || left >= right) {
        out[0] = RECT{0, 0, width, height};
        return 1;
    }
    out[0] = RECT{0, 0, width, top};
    out[1] = RECT{0, bottom, width, height};
    out[2] = RECT{0, top, left, bottom};
    out[3] = RECT{right, top, width, bottom};
    return 4;
}

wuc::CompositionSurfaceBrush SurfaceBrushFor(wuc::CompositionDrawingSurface const& surface) {
    auto brush = g_compositor.CreateSurfaceBrush(surface);
    brush.Stretch(wuc::CompositionStretch::None);
    brush.HorizontalAlignmentRatio(0.0f);
    brush.VerticalAlignmentRatio(0.0f);
    return brush;
}

// Builds (or rebuilds) the composition content from the rasterized frame:
// the mask surface (the frame's coverage), the overlay surface (noise grain
// under the ears), the effect brush, and the sprites.
bool BuildAcrylicContent(Overlay& overlay, COLORREF rgb) try {
    CompositionFrame& ac = *overlay.comp;
    const Layout& layout = overlay.layout;
    const FrameSurface& surface = *overlay.surface;
    int width = surface.width, height = surface.height;
    size_t count = surface.coverage.size();

    if (!ac.built) {
        std::vector<DWORD> mask(count);
        for (size_t i = 0; i < count; i++) {
            DWORD c = surface.coverage[i];
            mask[i] = (c << 24) | (c << 16) | (c << 8) | c;  // premultiplied white
        }

        std::vector<DWORD> over(count, 0);
        unsigned density = static_cast<unsigned>(g_settings.noiseDensity);
        unsigned opacity = static_cast<unsigned>(g_settings.noiseOpacity);
        if (density > 0 && opacity > 0) {
            // Grey grain over the frame (the taskbar's part included), the
            // same from one build to the next so a recolor doesn't shimmer.
            uint32_t state = 0x9E3779B9u ^
                             static_cast<uint32_t>(reinterpret_cast<ULONG_PTR>(layout.monitor));
            if (!state) {
                state = 1;
            }
            for (size_t i = 0; i < count; i++) {
                state ^= state << 13;
                state ^= state >> 17;
                state ^= state << 5;
                unsigned c = surface.coverage[i];
                if (!c || (state % 100) >= density) {
                    continue;
                }
                unsigned a = c * opacity / 100;
                if (!a) {
                    continue;
                }
                unsigned grey = ((state >> 8) & 0xFF) * a / 255;
                over[i] = (a << 24) | (grey << 16) | (grey << 8) | grey;
            }
        }
        for (size_t i = 0; i < count; i++) {
            DWORD fixed = surface.staticLayer[i];
            if (fixed) {
                over[i] = BlendOver(fixed, over[i]);
            }
        }

        ac.maskSurface = CreateSurface(width, height, mask.data(), L"mask");
        ac.overlaySurface = CreateSurface(width, height, over.data(), L"overlay");
        if (!ac.maskSurface || !ac.overlaySurface) {
            return false;
        }
        ApplyAcrylicRegion(overlay);
        ac.built = true;
    }

    // The bands show the backdrop (through one effect brush) or the color.
    bool backdrop = UsesBackdrop();
    BOOL hostBackdrop = backdrop;
    DwmSetWindowAttribute(overlay.hwnd, DWMWA_USE_HOSTBACKDROPBRUSH, &hostBackdrop,
                          sizeof(hostBackdrop));
    ac.factories.clear();
    ac.graphsChecked = false;
    ac.graphTicksPending = 0;
    ac.effectBrush = nullptr;
    ac.colorProperties.clear();
    ac.colorBrush = nullptr;
    ac.gradientBrush = nullptr;
    ac.gradientStops.clear();
    ac.gradientFactory = nullptr;
    int level = 0;
    if (backdrop) {
        level = BuildAcrylicBrush(overlay, rgb, ac.effectBrush);
        if (level < 0) {
            return false;
        }
    } else {
        ac.hostBackdrop = nullptr;
    }
    wf::Numerics::float2 size{static_cast<float>(width), static_cast<float>(height)};
    if (!backdrop) {
        // The color fill: one window-sized sprite, the frame's mask over
        // the color at the frame's opacity, with the overlay on top.
        ac.root.Children().RemoveAll();
        ac.backdropRects.clear();
        ac.root.Size(size);
        auto maskBrush = g_compositor.CreateMaskBrush();
        maskBrush.Mask(SurfaceBrushFor(ac.maskSurface));
        if (!BuildGradientBrush(ac, rgb, size.x, size.y)) {
            ac.colorBrush = g_compositor.CreateColorBrush(FillColor(rgb));
        }
        if (ac.gradientBrush) {
            maskBrush.Source(ac.gradientBrush);
        } else {
            maskBrush.Source(ac.colorBrush);
        }
        auto fill = g_compositor.CreateSpriteVisual();
        fill.Size(size);
        fill.Brush(maskBrush);
        ac.root.Children().InsertAtTop(fill);
        auto top = g_compositor.CreateSpriteVisual();
        top.Size(size);
        top.Brush(SurfaceBrushFor(ac.overlaySurface));
        ac.root.Children().InsertAtTop(top);
        SyncRefreshers(overlay);  // none for a color fill
        Wh_Log(L"monitor %p: color content %dx%d (#%02X%02X%02X%s at %d%%, bevel %d px)",
               layout.monitor, width, height, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb),
               ac.gradientBrush ? L", a gradient" : L"", g_settings.opacity, layout.bevelWidth);
        return true;
    }

    // The backdrop sprites: not one over the whole window - DWM would then
    // blur the whole monitor whenever something behind it changes - but one
    // per band, each reaching far enough inwards for the blur at the band's
    // inner edge to be complete (the corner radius plus the kernel's reach).
    // Each sprite's blur input also extends past its band into the
    // neighboring ones by that reach, and the sprite is clipped back to its
    // band, so the blur on either side of a join is computed from the same
    // content and the joins don't show. The mask is the one window-sized
    // surface, offset into each sprite. The tint rides on the same sprites
    // when the graph couldn't carry it; the overlay (ears, noise: no
    // effect) is one window-sized sprite on top.
    ac.root.Children().RemoveAll();
    ac.backdropRects.clear();
    ac.root.Size(size);
    RECT inner = layout.innerRect;
    OffsetRect(&inner, -layout.overlayRect.left, -layout.overlayRect.top);
    int reach = layout.radius + static_cast<int>(std::ceil(3.0f * layout.backdropBlur)) + 1;
    RECT bands[4];
    int bandCount = BackdropBandRects(width, height, inner, reach, bands);
    for (int b = 0; b < bandCount; b++) {
        const RECT& band = bands[b];
        RECT input = band;  // the sprite: the band plus the blur's reach around it
        InflateRect(&input, reach, reach);
        RECT window{0, 0, width, height};
        IntersectRect(&input, &input, &window);
        wf::Numerics::float2 origin{static_cast<float>(input.left), static_cast<float>(input.top)};
        wf::Numerics::float2 extent{static_cast<float>(input.right - input.left),
                                    static_cast<float>(input.bottom - input.top)};
        auto clip = g_compositor.CreateInsetClip(
            static_cast<float>(band.left - input.left), static_cast<float>(band.top - input.top),
            static_cast<float>(input.right - band.right),
            static_cast<float>(input.bottom - band.bottom));
        auto maskBrush = g_compositor.CreateMaskBrush();
        auto maskSurfaceBrush = SurfaceBrushFor(ac.maskSurface);
        maskSurfaceBrush.Offset({-origin.x, -origin.y});
        maskBrush.Mask(maskSurfaceBrush);
        wuc::CompositionBrush source = ac.effectBrush ? wuc::CompositionBrush(ac.effectBrush)
                                                      : wuc::CompositionBrush(ac.hostBackdrop);
        if (ac.gradientFactory) {
            try {
                source = MakeBandGradientBrush(ac, rgb, size.x, size.y, origin);
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"monitor %p: band gradient brush threw %08X", layout.monitor,
                       e.code().value);
            }
        }
        maskBrush.Source(source);
        auto fill = g_compositor.CreateSpriteVisual();
        fill.Offset({origin.x, origin.y, 0.0f});
        fill.Size(extent);
        fill.Clip(clip);
        fill.Brush(maskBrush);
        ac.root.Children().InsertAtTop(fill);
        ac.backdropRects.push_back(input);
        if (level > 0 && g_settings.tintOpacity > 0) {
            auto tintMask = g_compositor.CreateMaskBrush();
            auto tintMaskSurface = SurfaceBrushFor(ac.maskSurface);
            tintMaskSurface.Offset({-origin.x, -origin.y});
            tintMask.Mask(tintMaskSurface);
            wuc::CompositionBrush tintSource{nullptr};
            if (FrameColorIsGradient()) {
                try {
                    tintSource = MakeWindowGradient(&ac, rgb, OpacityAlpha(g_settings.tintOpacity),
                                                    size.x, size.y, origin);
                } catch (winrt::hresult_error const&) {
                }
            }
            if (!tintSource) {
                tintSource = g_compositor.CreateColorBrush(
                    wu::Color{OpacityAlpha(g_settings.tintOpacity), GetRValue(rgb), GetGValue(rgb),
                              GetBValue(rgb)});
            }
            tintMask.Source(tintSource);
            auto tint = g_compositor.CreateSpriteVisual();
            tint.Offset({origin.x, origin.y, 0.0f});
            tint.Size(extent);
            tint.Clip(g_compositor.CreateInsetClip(
                static_cast<float>(band.left - input.left), static_cast<float>(band.top - input.top),
                static_cast<float>(input.right - band.right),
                static_cast<float>(input.bottom - band.bottom)));
            tint.Brush(tintMask);
            ac.root.Children().InsertAtTop(tint);
        }
    }
    auto top = g_compositor.CreateSpriteVisual();
    top.Size(size);
    top.Brush(SurfaceBrushFor(ac.overlaySurface));
    ac.root.Children().InsertAtTop(top);
    SyncRefreshers(overlay);
    Wh_Log(L"monitor %p: acrylic content %dx%d (%s; blur %.1f, tint #%02X%02X%02X at "
           L"%d%%, luminosity %d%%, saturation %d%%, noise %d%% at %d%%, bevel %d px)",
           layout.monitor, width, height,
           !ac.effectBrush ? L"no effects"
           : level == 0    ? (ac.gradientFactory ? L"full graph, gradient" : L"full graph")
                           : L"blur only",
           layout.backdropBlur, GetRValue(rgb),
           GetGValue(rgb), GetBValue(rgb), g_settings.tintOpacity, g_settings.luminosityOpacity,
           g_settings.saturation, g_settings.noiseOpacity, g_settings.noiseDensity,
           layout.bevelWidth);
    return true;
} catch (winrt::hresult_error const& e) {
    Wh_Log(L"monitor %p: acrylic content threw %08X: %s", overlay.layout.monitor, e.code().value,
           e.message().c_str());
    return false;
}

// Pushes a frame bitmap to its window at a constant alpha (255 to show, 0
// to take it off the screen). With `rect`, UpdateLayeredWindow also moves
// and resizes the window to it - which sends window messages to the mod's
// UI thread, so only that thread passes a rect. The taskbar thread passes
// none (position and size unchanged) and can therefore never block on the
// UI thread. The caller holds g_revealMutex.
bool PresentFrame(HWND hwnd, HDC memDc, int width, int height, const RECT* rect, BYTE alpha) {
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }
    POINT dstPos{};
    SIZE size{width, height};
    if (rect) {
        dstPos = {rect->left, rect->top};
    }
    POINT srcPos{0, 0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    BOOL ok = UpdateLayeredWindow(hwnd, screenDc, rect ? &dstPos : nullptr, rect ? &size : nullptr,
                                  memDc, &srcPos, 0, &blend, ULW_ALPHA);
    ReleaseDC(nullptr, screenDc);
    if (!ok) {
        Wh_Log(L"UpdateLayeredWindow failed: %u", GetLastError());
    }
    return ok != FALSE;
}

// Pushes the frame's bitmap to the screen at its presentation alpha (an
// acrylic frame is uncloaked or cloaked instead), and with it the shadow
// slices that have their pixels (built beforehand, see EnsureShadowSurfaces).
// The caller holds g_revealMutex.
bool PresentOverlayLocked(Overlay& overlay) {
    bool ok = true;
    if (overlay.comp) {
        SetAcrylicCloaked(overlay, overlay.presentAlpha == 0);
    } else {
        const FrameSurface& surface = *overlay.surface;
        ok = PresentFrame(overlay.hwnd, surface.memDc, surface.width, surface.height,
                          &overlay.layout.overlayRect, overlay.presentAlpha);
    }
    for (const ShadowSlice& slice : overlay.shadow) {
        if (slice.hwnd && slice.surface) {
            PresentFrame(slice.hwnd, slice.surface->memDc, slice.surface->width,
                         slice.surface->height, &slice.rect, overlay.presentAlpha);
        }
    }
    return ok;
}

// Builds the shadow slices' pixels for the current layout if they aren't
// (a CPU blur along the screen's edges), outside any lock: the taskbar
// thread's rendering tick takes g_revealMutex while a reveal is pending.
void EnsureShadowSurfaces(Overlay& overlay) {
    if (overlay.shadowBuilt || !overlay.shadow[0].hwnd) {
        return;
    }
    BuildShadowSurfaces(overlay.layout, overlay.shadow);
    overlay.shadowBuilt = true;
}

bool PresentOverlay(Overlay& overlay) {
    if (!overlay.hwnd || (!overlay.surface && !overlay.comp)) {
        return false;
    }
    if (overlay.presentAlpha != 0) {
        EnsureShadowSurfaces(overlay);  // taking it off the screen needs no pixels
    }
    std::lock_guard<std::mutex> lock(g_revealMutex);
    return PresentOverlayLocked(overlay);
}

// Colors the cached shape (one table lookup per pixel) and pushes it to the
// window.
bool PaintOverlay(Overlay& overlay, COLORREF rgb) {
    const Layout& layout = overlay.layout;
    if (!overlay.hwnd) {
        return false;
    }
    if (!overlay.surface) {
        overlay.surface = BuildFrameSurface(layout, !overlay.comp);
        if (!overlay.surface) {
            return false;
        }
        if (!overlay.comp) {
            // The layered frame's painted shape as its window region: DWM then
            // composes only the bands, not a monitor-sized layered surface
            // over every window (the acrylic frame gets its region with its
            // content).
            SetWindowRgn(overlay.hwnd, RegionFromSurface(*overlay.surface), FALSE);
        }
    }
    if (overlay.presentAlpha != 0) {
        EnsureShadowSurfaces(overlay);
    }
    if (overlay.comp) {
        // The shape goes to the composition surfaces; the color into the
        // effect graph. The taskbar thread never touches either.
        if (!BuildAcrylicContent(overlay, rgb)) {
            if (!g_compositionUnavailable.exchange(true)) {
                Wh_Log(L"acrylic frame can't be built here - falling back to the solid fill");
                if (HWND ctrl = g_ctrlWnd.load()) {
                    PostMessageW(ctrl, kMsgReload, 0, 0);
                }
            }
            return false;
        }
        overlay.painted = true;
        overlay.paintedColor = rgb;
        overlay.displayColor = rgb;
        if (!overlay.colorAnimating) {
            overlay.colorTarget = rgb;
        }
        std::lock_guard<std::mutex> lock(g_revealMutex);
        return PresentOverlayLocked(overlay);
    }
    std::lock_guard<std::mutex> lock(g_revealMutex);

    BYTE alpha = static_cast<BYTE>(g_settings.opacity * 255 / 100);
    if (alpha == 0) {
        alpha = 1;  // keep the frame hit-testable even when invisible
    }

    // Premultiplied BGRA for every coverage value.
    DWORD lut[256];
    for (unsigned c = 0; c < 256; c++) {
        unsigned a = c * alpha / 255;
        unsigned r = GetRValue(rgb) * a / 255;
        unsigned g = GetGValue(rgb) * a / 255;
        unsigned b = GetBValue(rgb) * a / 255;
        lut[c] = (a << 24) | (r << 16) | (g << 8) | b;
    }

    FrameSurface& surface = *overlay.surface;
    size_t count = surface.coverage.size();
    const BYTE* coverage = surface.coverage.data();
    const DWORD* staticLayer = surface.staticLayer.data();
    DWORD* dst = surface.pixels;
    for (size_t i = 0; i < count; i++) {
        DWORD frame = lut[coverage[i]];
        DWORD fixed = staticLayer[i];
        dst[i] = fixed ? BlendOver(fixed, frame) : frame;
    }

    overlay.painted = true;
    overlay.paintedColor = rgb;
    overlay.displayColor = rgb;
    if (!overlay.colorAnimating) {
        overlay.colorTarget = rgb;
    }
    return PresentOverlayLocked(overlay);
}

// ---------------------------------------------------------------------------
// Full-screen detection
// ---------------------------------------------------------------------------

bool IsIgnoredWindowClass(HWND hwnd) {
    WCHAR className[128];
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return true;
    }

    static const PCWSTR kIgnored[] = {
        kFrameClassName,
        kAppBarClassName,
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"Progman",
        L"WorkerW",
        L"XamlExplorerHostIslandWindow",
        L"Windows.UI.Core.CoreWindow",
        L"ForegroundStaging",
        L"MultitaskingViewFrame",
        L"TaskListThumbnailWnd",
        L"TopLevelWindowForOverflowXamlIsland",
        L"Shell_LightDismissOverlay",  // monitor-sized click catcher behind tray flyouts
        L"Shell_InputSwitchTopLevelWindow",
        L"Xaml_WindowedPopupClass",
        L"Windows.Internal.Shell.TabProxyWindow",
        L"Shell_SystemDialog",
    };
    for (PCWSTR ignored : kIgnored) {
        if (wcscmp(className, ignored) == 0) {
            return true;
        }
    }

    // Anything else explorer itself hosts (flyout islands, dismiss overlays,
    // drag images) is shell chrome, never a full-screen app. File Explorer
    // windows are the one exception worth keeping.
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && wcscmp(className, L"CabinetWClass") != 0) {
        return true;
    }

    return false;
}

// Walks the top-level windows in z-order. A visible, uncloaked window in the
// foreground that covers the whole monitor is full-screen, unless an ordinary
// window is layered above it on that monitor. `confirmed` is the window this
// monitor is already hidden for (if any): it keeps counting while it covers
// the monitor even after losing focus.
bool HasFullscreenWindow(const Layout& layout, HWND confirmed, HWND* fullscreenWnd) {
    *fullscreenWnd = nullptr;
    HWND foreground = GetForegroundWindow();

    // A maximized window fills the work area. While the work area is the
    // whole monitor (in flux: taskbar moving, appbars being re-registered)
    // a maximized window spanning the monitor means nothing; once the work
    // area is smaller than the monitor, a maximized window that still spans
    // the whole monitor is ignoring the work area - a full-screen app (a
    // browser in F11 keeps its maximized state).
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    bool workAreaReduced =
        GetMonitorInfoW(layout.monitor, &mi) && !EqualRect(&mi.rcWork, &mi.rcMonitor);

    for (HWND hwnd = GetTopWindow(nullptr); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
            continue;
        }

        RECT rect{};
        if (!GetWindowRect(hwnd, &rect)) {
            continue;
        }
        RECT intersection{};
        if (!IntersectRect(&intersection, &rect, &layout.monitorRect)) {
            continue;
        }

        if (IsIgnoredWindowClass(hwnd) || IsWindowCloaked(hwnd)) {
            continue;
        }

        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT)) {
            continue;  // overlays, click-through utilities, tool windows
        }

        if (RectContains(rect, layout.monitorRect)) {
            // A full-screen app is the window the user is in, or was in when
            // it went full-screen: a desktop-sized window that is never in
            // the foreground (wallpaper hosts, widget layers) doesn't count.
            if ((!IsZoomed(hwnd) || workAreaReduced) &&
                (hwnd == foreground || hwnd == confirmed)) {
                *fullscreenWnd = hwnd;
                return true;
            }
        }

        // Owned popups (browser "press F11 to exit" bubbles, tooltips, menus)
        // sit above their owner in z-order; they never decide the outcome.
        if (GetWindow(hwnd, GW_OWNER)) {
            continue;
        }

        // The first ordinary window on this monitor decides: if it isn't
        // full-screen, nothing beneath it matters.
        if (!(exStyle & WS_EX_TOPMOST)) {
            return false;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Overlays
// ---------------------------------------------------------------------------

// Takes a frame's pending reveal away from the taskbar thread (a no-op if
// it already presented the frame, or none was registered). After this the
// taskbar thread won't touch the frame's bitmap.
void WithdrawReveal(Overlay& overlay) {
    if (!overlay.revealPending) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_revealMutex);
    auto it = g_reveals.find(overlay.layout.taskbar);
    if (it != g_reveals.end() && it->second.frame == overlay.hwnd) {
        g_reveals.erase(it);
    }
    overlay.revealPending = false;
}

// Takes a frame off the screen on the spot: blanked (alpha 0) and hidden, so
// nothing that might animate a window's disappearance has anything visible
// to animate. Also ends a pending reveal (whose window is visible at alpha
// 0, or already revealed by the taskbar thread).
void HideFrame(Overlay& overlay) {
    bool visible = overlay.shown || overlay.revealPending;
    WithdrawReveal(overlay);
    if (overlay.hwnd && visible) {
        overlay.presentAlpha = 0;
        PresentOverlay(overlay);  // a no-op without a surface
        ShowWindow(overlay.hwnd, SW_HIDE);
        for (const ShadowSlice& slice : overlay.shadow) {
            if (slice.hwnd) {
                ShowWindow(slice.hwnd, SW_HIDE);
            }
        }
        if (overlay.comp) {
            for (HWND strip : overlay.comp->strips) {
                if (strip) {
                    ShowWindow(strip, SW_HIDE);
                }
            }
        }
    }
    overlay.shown = false;
}

// Removes a frame (and its shadow) on the spot.
void DestroyOverlay(Overlay& overlay) {
    if (overlay.hwnd) {
        HideFrame(overlay);
        if (overlay.comp) {
            DestroyAcrylic(overlay);
        } else {
            DestroyWindow(overlay.hwnd);
            overlay.hwnd = nullptr;
        }
    }
    for (ShadowSlice& slice : overlay.shadow) {
        if (slice.hwnd) {
            DestroyWindow(slice.hwnd);
            slice.hwnd = nullptr;
        }
        slice.surface.reset();
    }
    overlay.shadowBuilt = false;
}

// Takes every frame off the screen on the spot, leaving the windows to be
// destroyed later.
void HideOverlays(std::vector<Overlay>& overlays) {
    for (Overlay& overlay : overlays) {
        HideFrame(overlay);
    }
}

void DestroyOverlays(std::vector<Overlay>& overlays) {
    for (Overlay& overlay : overlays) {
        DestroyOverlay(overlay);
    }
    overlays.clear();
}

Overlay* FindOverlay(HMONITOR monitor) {
    for (Overlay& overlay : *g_overlays) {
        if (overlay.layout.monitor == monitor) {
            return &overlay;
        }
    }
    return nullptr;
}

// The frame is solid to the mouse where it is painted, like the taskbar
// (the layered window passes the mouse through wherever its pixels have
// zero alpha anyway; the acrylic window's region is its painted shape): the
// work area keeps windows out from under it, so nothing that needs clicks
// is there, and the resize borders of maximized windows stay unreachable.
void ApplyFrameMouseStyle(HWND hwnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    LONG_PTR wanted = exStyle & ~WS_EX_TRANSPARENT;
    if (wanted != exStyle) {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, wanted);
    }
}

// The layered frame (composition unavailable): a layered window updated
// from its bitmap.
HWND CreateFrameWindow(const RECT& rect) {
    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST;
    HWND hwnd = CreateWindowExW(exStyle, kFrameClassName, L"Round Everything", WS_POPUP,
                                rect.left, rect.top, rect.right - rect.left,
                                rect.bottom - rect.top, nullptr, nullptr, g_module, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        return nullptr;
    }

    // Aero Peek (taskbar thumbnail hover, show-desktop corner hover) fades
    // every other window to glass; keep the frame painted through it.
    BOOL excludedFromPeek = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_EXCLUDED_FROM_PEEK, &excludedFromPeek,
                          sizeof(excludedFromPeek));
    // No DWM show/hide animation: a frame that is hidden or destroyed must
    // vanish on the spot rather than dim out.
    BOOL noTransitions = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noTransitions,
                          sizeof(noTransitions));
    ApplyFrameMouseStyle(hwnd);
    return hwnd;
}

// A shadow slice's window: like the frame, but always click-through (its
// pixels lie over the screen content). Created hidden; shown with the frame.
HWND CreateShadowWindow(const RECT& rect) {
    DWORD exStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                    WS_EX_TOPMOST;
    HWND hwnd = CreateWindowExW(exStyle, kFrameClassName, L"Round Everything Shadow", WS_POPUP,
                                rect.left, rect.top, rect.right - rect.left,
                                rect.bottom - rect.top, nullptr, nullptr, g_module, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW (shadow) failed: %u", GetLastError());
        return nullptr;
    }
    BOOL excludedFromPeek = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_EXCLUDED_FROM_PEEK, &excludedFromPeek,
                          sizeof(excludedFromPeek));
    BOOL noTransitions = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noTransitions,
                          sizeof(noTransitions));
    return hwnd;
}

// Creates, moves or removes a frame's shadow windows to match its layout
// (one per slice the layout calls for). The pixels are built later, by the
// next paint. Returns whether a window was created (it is still hidden
// then).
bool SyncShadowWindows(Overlay& overlay) {
    RECT rects[kShadowSlices];
    ShadowSliceRects(overlay.layout, rects);
    bool created = false;
    for (int i = 0; i < kShadowSlices; i++) {
        ShadowSlice& slice = overlay.shadow[i];
        slice.surface.reset();
        bool wanted = !IsRectEmpty(&rects[i]);
        if (wanted && !slice.hwnd) {
            slice.hwnd = CreateShadowWindow(rects[i]);
            created = created || slice.hwnd != nullptr;
        } else if (wanted && !EqualRect(&slice.rect, &rects[i])) {
            SetWindowPos(slice.hwnd, nullptr, rects[i].left, rects[i].top,
                         rects[i].right - rects[i].left, rects[i].bottom - rects[i].top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        } else if (!wanted && slice.hwnd) {
            DestroyWindow(slice.hwnd);
            slice.hwnd = nullptr;
        }
        slice.rect = rects[i];
    }
    overlay.shadowBuilt = false;
    return created;
}

// Brings the frames in line with the layouts: one persistent frame window per
// monitor. A monitor whose layout changed (the taskbar moved, resized, came
// or went) keeps its window and gets a new shape; with a taskbar it is then
// shown again together with it. New monitors get a new frame, vanished
// monitors lose theirs.
void WaitForTaskbar(Overlay& overlay, ULONGLONG now, PCWSTR why);  // defined with the visibility code
void RaiseFrame(Overlay& overlay, bool show);

void HandBackTaskbar(HWND taskbar);  // defined with the taskbar-side code

void SyncOverlays(const std::vector<Layout>& layouts) {
    // Frames of monitors that are gone - their taskbars, when no layout
    // uses them any more, are handed back (ears, background, stroke, theme).
    for (Overlay& overlay : *g_overlays) {
        bool present = false;
        bool taskbarUsed = false;
        for (const Layout& layout : layouts) {
            present = present || layout.monitor == overlay.layout.monitor;
            taskbarUsed = taskbarUsed || (overlay.layout.taskbar &&
                                          layout.taskbar == overlay.layout.taskbar);
        }
        if (!present) {
            if (overlay.layout.taskbar && !taskbarUsed) {
                HandBackTaskbar(overlay.layout.taskbar);
            }
            DestroyOverlay(overlay);
        }
    }
    g_overlays->erase(std::remove_if(g_overlays->begin(), g_overlays->end(),
                                    [](const Overlay& o) { return o.hwnd == nullptr; }),
                     g_overlays->end());

    for (const Layout& layout : layouts) {
        if (Overlay* existing = FindOverlay(layout.monitor)) {
            if (existing->layout.signature == layout.signature) {
                existing->layout = layout;  // same geometry, refreshed handles
                continue;
            }
            // Another taskbar window, another edge, or the taskbar strip
            // resized: the frame goes with the taskbar and comes back once
            // its tree is whole. The strip appearing or going (auto-hide
            // turned off or on) re-shapes in place: the taskbar window is
            // the same and stays.
            bool taskbarChanged =
                existing->layout.taskbar != layout.taskbar ||
                existing->layout.edge != layout.edge ||
                (!EqualRect(&existing->layout.taskbarRect, &layout.taskbarRect) &&
                 !IsRectEmpty(&existing->layout.taskbarRect) && !IsRectEmpty(&layout.taskbarRect));
            if (existing->revealPending) {
                HideFrame(*existing);  // the old shape must not be revealed
            }
            existing->layout = layout;
            existing->surface.reset();  // rebuilt by the paint below
            bool shadowCreated = SyncShadowWindows(*existing);  // moved to the new layout
            ApplyFrameMouseStyle(existing->hwnd);
            if (existing->comp) {
                PlaceAcrylicFrame(*existing);  // moved now; content rebuilt by the paint
                EnsureTaskbarHooks(layout.taskbar);
            }
            if (layout.taskbar && taskbarChanged) {
                // Shown again together with the taskbar once it is whole
                // (a change of settings alone re-shapes on the spot).
                WaitForTaskbar(*existing, GetTickCount64(), L"taskbar changed");
            }
            if (existing->shown) {
                // Re-shape in place; UpdateLayeredWindow moves and resizes
                // the windows along with the new bitmaps.
                PaintOverlay(*existing, existing->painted ? existing->paintedColor
                                                          : FrameColorFor(*existing));
                if (shadowCreated || existing->comp) {
                    RaiseFrame(*existing, true);  // new shadow / bezel windows are still hidden
                }
            } else {
                if (!existing->comp) {
                    SetWindowPos(existing->hwnd, nullptr, layout.overlayRect.left,
                                 layout.overlayRect.top,
                                 layout.overlayRect.right - layout.overlayRect.left,
                                 layout.overlayRect.bottom - layout.overlayRect.top,
                                 SWP_NOZORDER | SWP_NOACTIVATE);
                }
                existing->painted = false;
            }
            Wh_Log(L"monitor %p: frame re-shaped (taskbar %p, edge %d)", layout.monitor,
                   layout.taskbar, layout.edge);
            continue;
        }

        Overlay overlay;
        overlay.layout = layout;
        if (UsesComposition()) {
            if (!CreateAcrylicFrame(overlay)) {
                continue;
            }
            EnsureTaskbarHooks(layout.taskbar);
        } else {
            overlay.hwnd = CreateFrameWindow(layout.overlayRect);
            if (!overlay.hwnd) {
                continue;
            }
        }
        SyncShadowWindows(overlay);
        if (layout.taskbar) {
            overlay.waiting = true;  // appears together with the taskbar
            overlay.waitSince = GetTickCount64();
        }
        PaintOverlay(overlay, FrameColorFor(overlay));
        g_overlays->push_back(std::move(overlay));
    }
}

// True when the frame is no longer above ordinary windows: it lost the
// topmost bit, or a visible non-topmost window precedes it in z-order over
// the frame's area.
bool FrameNeedsRaise(const Overlay& overlay) {
    if (overlay.comp) {
        return AcrylicOrderBroken(FollowEntryFor(overlay));
    }
    if (!(GetWindowLongPtrW(overlay.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        return true;
    }
    for (HWND hwnd = GetTopWindow(nullptr); hwnd && hwnd != overlay.hwnd;
         hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
            continue;
        }
        if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) {
            continue;  // other topmost windows above us are fine
        }
        RECT rect{};
        if (!GetWindowRect(hwnd, &rect)) {
            continue;
        }
        RECT intersection{};
        if (!IntersectRect(&intersection, &rect, &overlay.layout.overlayRect)) {
            continue;
        }
        if (IsIgnoredWindowClass(hwnd) || IsWindowCloaked(hwnd)) {
            continue;
        }
        return true;
    }
    return false;
}

// Puts the frame on top and its shadow right behind it (the shadow inherits
// the frame's place in the z-order: topmost, just below it). An acrylic
// frame goes under its taskbar instead, its shadow right above it and the
// bezel ring right below.
void RaiseFrame(Overlay& overlay, bool show) {
    UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | (show ? SWP_SHOWWINDOW : 0);
    if (overlay.comp) {
        PlaceAcrylicOrder(FollowEntryFor(overlay), flags);
        return;
    }
    SetWindowPos(overlay.hwnd, HWND_TOPMOST, 0, 0, 0, 0, flags);
    for (const ShadowSlice& slice : overlay.shadow) {
        if (slice.hwnd) {
            SetWindowPos(slice.hwnd, overlay.hwnd, 0, 0, 0, 0, flags);
        }
    }
}

// Takes a frame off the screen on the spot and starts waiting for its
// taskbar; it comes back together with the taskbar.
void WaitForTaskbar(Overlay& overlay, ULONGLONG now, PCWSTR why) {
    if (!overlay.layout.taskbar) {
        return;
    }
    if (overlay.shown || overlay.revealPending) {
        HideFrame(overlay);
        Wh_Log(L"monitor %p: frame hidden (%s)", overlay.layout.monitor, why);
    } else if (!overlay.waiting) {
        Wh_Log(L"monitor %p: frame waits for its taskbar (%s)", overlay.layout.monitor, why);
    }
    if (!overlay.waiting) {
        overlay.waitSince = now;
        overlay.recolorRequestTick = 0;
        overlay.recolorSerial = 0;
    }
    overlay.waiting = true;
}

bool AppBarSetInPlace(HMONITOR monitor, const RECT& baseWorkArea);  // appbar code, below
RECT AppBarBase(const Layout& layout);

// Defined with the taskbar-side code: asks the taskbar thread to paint the
// background again in its next rendering pass, revealing the frame registered
// in g_reveals with it; it answers with kMsgTaskbarRecolored, carrying the
// serial, from that same rendering pass.
void RequestTaskbarRecolor(HWND taskbar, UINT serial);

// Whether the taskbar has rendered the recolor with the given serial.
bool TaskbarRecolored(HWND taskbar, UINT serial) {
    auto it = g_taskbarRecolored.find(taskbar);
    return it != g_taskbarRecolored.end() && it->second >= serial;
}

// Whether the frame is revealed through the taskbar (its rendering pass
// carries the background color, so only when the mod reaches the tree).
bool UsesHandshake() {
    return g_taskbarXamlAvailable;
}

// The handshake: the frame is made ready (painted, its window visible but at
// alpha 0, so there is nothing to see yet) and handed to the taskbar thread,
// which presents it at full alpha in the rendering pass that also paints the
// taskbar's background, before that pass commits - the two reach DWM for the
// same composition. Returns the recolor request's serial; its answer says
// the frame has been presented.
UINT BeginReveal(Overlay& overlay) {
    HWND taskbar = overlay.layout.taskbar;
    overlay.presentAlpha = 0;
    EnsureShadowSurfaces(overlay);  // the taskbar thread presents them from the registration
    if (!overlay.painted) {
        PaintOverlay(overlay, FrameColorFor(overlay));
    } else {
        PresentOverlay(overlay);
    }
    RaiseFrame(overlay, true);
    UINT serial = ++g_recolorSerial;
    if (overlay.surface) {
        std::lock_guard<std::mutex> lock(g_revealMutex);
        const FrameSurface& surface = *overlay.surface;
        Reveal reveal{serial, overlay.hwnd, surface.memDc, surface.width, surface.height};
        if (overlay.comp) {
            reveal.composition = true;
            for (int i = 0; i < 4; i++) {
                reveal.strips[i] = overlay.comp->strips[i];
            }
        }
        for (int i = 0; i < kShadowSlices; i++) {
            const ShadowSlice& slice = overlay.shadow[i];
            if (slice.hwnd && slice.surface) {
                reveal.shadow[i] = {slice.hwnd, slice.surface->memDc, slice.surface->width,
                                    slice.surface->height};
            }
        }
        g_reveals[taskbar] = reveal;
        overlay.revealPending = true;
    }
    RequestTaskbarRecolor(taskbar, serial);
    return serial;
}

// Whether a waiting frame may show. Everything has to have settled first:
// the taskbar's tree reported whole-and-rendered for the frame's edge after
// the wait began, the shell isn't cloaking the taskbar, and the work area
// is reserved for the new layout. Then the frame is handed to the taskbar
// thread (BeginReveal), which paints the taskbar's background and reveals
// the frame in one rendering pass; this returns true once that has happened
// (the frame is on its way to the screen by then). A wait that runs too long
// skips to that step, so even then the two still appear together whenever
// the taskbar answers in time; a reveal that isn't answered in time is done
// by the UI thread itself.
bool TaskbarReadyFor(Overlay& overlay, ULONGLONG now) {
    HWND taskbar = overlay.layout.taskbar;
    if (!taskbar || !overlay.waiting) {
        return true;
    }
    if (overlay.recolorRequestTick) {
        // Settled; waiting for the taskbar to have painted its background.
        return TaskbarRecolored(taskbar, overlay.recolorSerial) ||
               now - overlay.recolorRequestTick >= kRecolorWaitMaxMs;
    }
    if (now - overlay.waitSince >= kTaskbarWaitMaxMs) {
        const TaskbarReady& ready = g_taskbarReady[taskbar];
        Wh_Log(L"monitor %p: wait given up (taskbar %s, tree %s, work area %s); taskbar asked "
               L"to color",
               overlay.layout.monitor, IsWindowCloaked(taskbar) ? L"still cloaked" : L"uncloaked",
               ready.tick >= overlay.waitSince ? L"whole" : L"not reported whole",
               AppBarSetInPlace(overlay.layout.monitor, AppBarBase(overlay.layout)) ? L"in place"
                                                                                     : L"pending");
        if (!UsesHandshake()) {
            return true;
        }
        overlay.recolorRequestTick = now;
        overlay.recolorSerial = BeginReveal(overlay);
        return false;
    }
    // (An auto-hide taskbar that has slid out is hidden, and stays so until
    // the mouse asks for it: not waited for.)
    if (IsWindowCloaked(taskbar) || (!IsWindowVisible(taskbar) && !overlay.layout.autoHide)) {
        return false;
    }
    const TaskbarReady& ready = g_taskbarReady[taskbar];
    if (ready.tick < overlay.waitSince ||
        (ready.edge != kEdgeNone && ready.edge != overlay.layout.edge)) {
        return false;
    }
    if (!AppBarSetInPlace(overlay.layout.monitor, AppBarBase(overlay.layout))) {
        return false;
    }
    if (!UsesHandshake()) {
        return true;
    }
    overlay.recolorRequestTick = now;
    overlay.recolorSerial = BeginReveal(overlay);
    Wh_Log(L"monitor %p: settled, taskbar asked to color and reveal the frame",
           overlay.layout.monitor);
    return false;
}

// Defined with the taskbar-side code: makes the taskbar's background
// transparent right away (the color comes back with RequestTaskbarRecolor).
void RequestTaskbarTransparent(HWND taskbar);

// Task View (Win+Tab) is a visible, monitor-sized island window that holds
// the foreground (it is driven by the keyboard). The same window class also
// hosts Alt+Tab and snap assist (not monitor-sized) and the taskbar's
// thumbnail previews (monitor-sized, but hovering a button activates
// nothing, so they are never in the foreground). Checked the moment such a
// window shows or hides (in-process WinEvent), so the reaction doesn't wait
// for the next tick.
bool IsTaskViewWindowClass(HWND hwnd) {
    WCHAR cls[64];
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           (wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
            wcscmp(cls, L"MultitaskingViewFrame") == 0);
}

bool SpansItsMonitor(HWND hwnd) {
    RECT rect{};
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    return GetWindowRect(hwnd, &rect) && monitor && GetMonitorInfoW(monitor, &mi) &&
           RectContains(rect, mi.rcMonitor);
}

// A visible, monitor-sized window of the Task View classes that carries a
// title - "Task View" (localized elsewhere), there from the moment the
// window shows, before the foreground has moved to it - and is either
// titled exactly that or in the foreground. The taskbar's thumbnail flyout
// is the same kind of window and is activated by a click on a grouped
// button, but it has no title, so it never passes.
bool IsTaskViewWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsWindowCloaked(hwnd) || !SpansItsMonitor(hwnd)) {
        return false;
    }
    WCHAR title[64];
    if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) <= 0) {
        return false;
    }
    return wcscmp(title, L"Task View") == 0 || hwnd == GetForegroundWindow();
}

// The open Task View window, or null.
HWND FindTaskViewWindow() {
    static const PCWSTR kClasses[] = {L"XamlExplorerHostIslandWindow", L"MultitaskingViewFrame"};
    for (PCWSTR cls : kClasses) {
        for (HWND hwnd = FindWindowExW(nullptr, nullptr, cls, nullptr); hwnd;
             hwnd = FindWindowExW(nullptr, hwnd, cls, nullptr)) {
            if (IsTaskViewWindow(hwnd)) {
                return hwnd;
            }
        }
    }
    return nullptr;
}

// kMsgTaskbarMoving (from the taskbar thread's hook): the window just moved
// or resized - the frame on that monitor goes away right now.
void OnTaskbarMoving(LPARAM lParam) {
    if (g_unloading || !g_overlays) {
        return;
    }
    HWND taskbar = reinterpret_cast<HWND>(lParam);
    ULONGLONG now = GetTickCount64();
    for (Overlay& overlay : *g_overlays) {
        if (overlay.layout.taskbar == taskbar) {
            WaitForTaskbar(overlay, now, L"taskbar moving");
        }
    }
}

// kMsgTaskbarZOrder (from the taskbar thread's hook): a taskbar window
// changed its place in the z-order. An acrylic frame that is no longer
// right under its taskbar is put back there now.
void OnTaskbarZOrder() {
    static ULONGLONG lastLogTick = 0;
    if (g_unloading || !g_overlays) {
        return;
    }
    for (Overlay& overlay : *g_overlays) {
        if (!overlay.comp || !overlay.hwnd || !IsWindowVisible(overlay.hwnd)) {
            continue;
        }
        FollowEntry entry = FollowEntryFor(overlay);
        if (!AcrylicOrderBroken(entry)) {
            continue;
        }
        PlaceAcrylicOrder(entry, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        ULONGLONG now = GetTickCount64();
        if (now - lastLogTick >= kRoutineLogMs) {
            lastLogTick = now;
            Wh_Log(L"monitor %p: frame put back under its taskbar", overlay.layout.monitor);
        }
    }
}

void UpdateVisibility() {
    static ULONGLONG lastRaiseLogTick = 0;
    ULONGLONG now = GetTickCount64();
    HWND taskViewWnd = FindTaskViewWindow();
    bool taskView = taskViewWnd != nullptr;

    for (Overlay& overlay : *g_overlays) {
        HWND fullscreenWnd = nullptr;
        bool detected = HasFullscreenWindow(overlay.layout, overlay.fullscreenWnd, &fullscreenWnd);
        // The foreground window itself spanning the monitor (F11, a game) is
        // acted on at once. Anything else detected must hold for two
        // consecutive ticks before the frame hides, so a transient z-order
        // state (show desktop, window switching bursts) can't blink it.
        // Coming back is immediate.
        overlay.fullscreenTicks = detected ? overlay.fullscreenTicks + 1 : 0;
        bool immediate = detected && fullscreenWnd && fullscreenWnd == GetForegroundWindow();
        bool fullscreen =
            detected && (immediate || overlay.fullscreenTicks >= 2 || overlay.fullscreen);
        if (fullscreen != overlay.fullscreen) {
            WCHAR cls[64] = L"";
            if (fullscreenWnd) {
                GetClassNameW(fullscreenWnd, cls, ARRAYSIZE(cls));
            }
            Wh_Log(L"monitor %p: full-screen %s%s%s", overlay.layout.monitor,
                   fullscreen ? L"on" : L"off", fullscreen ? L" - " : L"",
                   fullscreen ? cls : L"");
        }
        overlay.fullscreen = fullscreen;
        overlay.fullscreenWnd = fullscreen ? fullscreenWnd : nullptr;

        // A taskbar the shell has cloaked (a layout change in progress) takes
        // its frame with it; the frame comes back with the taskbar.
        HWND taskbar = overlay.layout.taskbar;
        if (taskbar && !overlay.waiting && overlay.shown && IsWindowCloaked(taskbar) &&
            !g_veils.count(taskbar)) {
            WaitForTaskbar(overlay, now, L"taskbar cloaked");
        }

        // Task View: taskbar transparent and frame gone the moment it opens;
        // when it closes, the taskbar is asked to color again and to reveal
        // the frame with it (the same handshake as after a layout change).
        if (taskView && !overlay.taskViewHidden) {
            overlay.taskViewHidden = true;
            overlay.taskViewRecolorTick = 0;
            overlay.taskViewRecolorSerial = 0;
            if (taskbar) {
                RequestTaskbarTransparent(taskbar);
            }
            WCHAR title[64] = L"";
            GetWindowTextW(taskViewWnd, title, ARRAYSIZE(title));
            Wh_Log(L"monitor %p: Task View open (%p \"%s\"%s), frame off and taskbar transparent",
                   overlay.layout.monitor, taskViewWnd, title,
                   taskViewWnd == GetForegroundWindow() ? L", foreground" : L"");
        }
        bool taskViewHolds = false;
        if (!taskView && overlay.taskViewHidden) {
            if (!taskbar) {
                overlay.taskViewHidden = false;
            } else if (overlay.waiting) {
                overlay.taskViewHidden = false;  // the wait recolors it, with the frame
            } else if (overlay.fullscreen || !UsesHandshake()) {
                // The frame stays off (or shows on its own); the taskbar just
                // gets its color and ears back.
                overlay.taskViewHidden = false;
                RequestTaskbarRecolor(taskbar, ++g_recolorSerial);
            } else if (!overlay.taskViewRecolorTick) {
                overlay.taskViewRecolorTick = now;
                overlay.taskViewRecolorSerial = BeginReveal(overlay);
                taskViewHolds = true;
            } else if (TaskbarRecolored(taskbar, overlay.taskViewRecolorSerial) ||
                       now - overlay.taskViewRecolorTick >= kRecolorWaitMaxMs) {
                overlay.taskViewHidden = false;
                Wh_Log(L"monitor %p: Task View closed, frame back with the taskbar's color",
                       overlay.layout.monitor);
            } else {
                taskViewHolds = true;
            }
        }

        bool show = !overlay.fullscreen && !taskView && !taskViewHolds &&
                    TaskbarReadyFor(overlay, now);

        if (show && !overlay.shown) {
            // Revealed by the taskbar thread already (the usual case: the
            // recolor answered), or presented here (no handshake, or one
            // that wasn't answered in time).
            bool revealed = false;
            if (overlay.revealPending) {
                std::lock_guard<std::mutex> lock(g_revealMutex);
                auto it = g_reveals.find(taskbar);
                revealed = it == g_reveals.end() || it->second.frame != overlay.hwnd;
            }
            WithdrawReveal(overlay);
            overlay.presentAlpha = 255;
            if (!revealed) {
                if (!overlay.painted) {
                    PaintOverlay(overlay, FrameColorFor(overlay));
                } else {
                    PresentOverlay(overlay);
                }
                RaiseFrame(overlay, true);
            }
            overlay.shown = true;
            if (overlay.waiting) {
                overlay.waiting = false;
                Wh_Log(L"monitor %p: taskbar ready, frame shown %s", overlay.layout.monitor,
                       revealed ? L"by the taskbar with its color" : L"by the mod");
            } else {
                Wh_Log(L"monitor %p: frame shown%s", overlay.layout.monitor,
                       revealed ? L" by the taskbar with its color" : L"");
            }
        } else if (!show && (overlay.shown || overlay.revealPending)) {
            Wh_Log(L"monitor %p: frame hidden (%s)", overlay.layout.monitor,
                   overlay.fullscreen ? L"full-screen" : taskView ? L"Task View" : L"waiting");
            HideFrame(overlay);
        } else if (show) {
            if (!IsWindowVisible(overlay.hwnd) || IsIconic(overlay.hwnd)) {
                Wh_Log(L"monitor %p: frame not visible although shown, re-showing",
                       overlay.layout.monitor);
                RaiseFrame(overlay, true);
            }
            // Watchdog: keep the frame above ordinary windows, like the taskbar.
            if (FrameNeedsRaise(overlay)) {
                RaiseFrame(overlay, false);
                if (now - lastRaiseLogTick >= kRoutineLogMs) {
                    lastRaiseLogTick = now;
                    Wh_Log(L"monitor %p: frame re-ordered", overlay.layout.monitor);
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Work area: invisible appbars, one per frame edge with a non-zero offset.
// Everything in this section runs on the appbar thread (see g_appBarThread).
// ---------------------------------------------------------------------------

LRESULT CALLBACK AppBarWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case kMsgAppBarCallback:
            if (wParam == ABN_POSCHANGED) {
                g_appBarsDirty = true;  // re-query on the next pass
                g_shellStateDirty = true;
            } else if (wParam == ABN_STATECHANGE) {
                g_shellStateDirty = true;  // auto-hide toggled
            }
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

RECT ProposedAppBarRect(const RECT& base, UINT edge, int thickness) {
    RECT rc = base;
    switch (edge) {
        case ABE_LEFT:
            rc.right = rc.left + thickness;
            break;
        case ABE_TOP:
            rc.bottom = rc.top + thickness;
            break;
        case ABE_RIGHT:
            rc.left = rc.right - thickness;
            break;
        default:
            rc.top = rc.bottom - thickness;
            break;
    }
    return rc;
}

// ABM_QUERYPOS + ABM_SETPOS for one appbar, keeping its thickness.
void SetAppBarPosition(const RECT& base, AppBar& bar) {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = bar.hwnd;
    abd.uEdge = bar.edge;
    abd.rc = ProposedAppBarRect(base, bar.edge, bar.thickness);

    SHAppBarMessage(ABM_QUERYPOS, &abd);

    // The shell may have shifted the strip (e.g. past the taskbar); keep the
    // thickness and the far edge where the query left them. The appbar on
    // the taskbar's edge is the exception: it is meant to lie over the
    // taskbar's strip, so it stays where it was proposed - were it shifted
    // past the strip the shell still reserves, the two would stack.
    if (bar.anchored) {
        abd.rc = ProposedAppBarRect(base, bar.edge, bar.thickness);
    }
    switch (bar.edge) {
        case ABE_LEFT:
            abd.rc.right = abd.rc.left + bar.thickness;
            break;
        case ABE_TOP:
            abd.rc.bottom = abd.rc.top + bar.thickness;
            break;
        case ABE_RIGHT:
            abd.rc.left = abd.rc.right - bar.thickness;
            break;
        default:
            abd.rc.top = abd.rc.bottom - bar.thickness;
            break;
    }

    SHAppBarMessage(ABM_SETPOS, &abd);

    SetWindowPos(bar.hwnd, nullptr, abd.rc.left, abd.rc.top, abd.rc.right - abd.rc.left,
                 abd.rc.bottom - abd.rc.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void RecordWorkArea(AppBarSet& set) {
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(set.monitor, &mi)) {
        set.workAreaAfterApply = mi.rcWork;
    }
}

void SetAppBarPositions(AppBarSet& set) {
    for (AppBar& bar : set.bars) {
        SetAppBarPosition(set.baseWorkArea, bar);
    }
    RecordWorkArea(set);
}

// The rectangle the appbars are measured from: the monitor. The appbar on
// the taskbar's edge covers the taskbar's own strip (anchored there, over
// the taskbar) whether the taskbar is docked or auto-hides: a docked
// taskbar reserves that strip itself, so the two overlap and the work area
// is the same; an auto-hide taskbar reserves nothing, and the strip stays
// reserved all the same. Toggling auto-hide then moves no window, and the
// shell's own handover (which reaches secondary taskbars later than the
// primary) can't stack a strip of its own next to the mod's.
RECT AppBarBase(const Layout& layout) {
    return layout.monitorRect;
}

// The frame strip and the extra inset on every edge, plus the taskbar's
// strip on its edge.
void DesiredThickness(const Layout& layout, int out[4]) {
    for (int e = 0; e < 4; e++) {
        out[e] = layout.offset[e] + layout.workAreaInset;
    }
    if (layout.edge != kEdgeNone && !IsRectEmpty(&layout.taskbarRect)) {
        const RECT& strip = layout.taskbarRect;
        out[layout.edge] += (layout.edge == kEdgeTop || layout.edge == kEdgeBottom)
                                ? strip.bottom - strip.top
                                : strip.right - strip.left;
    }
}

// Registers one appbar on one edge and positions it.
bool CreateAppBar(const RECT& base, UINT edge, int thickness, AppBar& bar) {
    bar.edge = edge;  // kEdge* values match ABE_* (bar.anchored is the caller's)
    bar.thickness = thickness;

    RECT rc = ProposedAppBarRect(base, bar.edge, thickness);
    bar.hwnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_LAYERED, kAppBarClassName,
        L"Round Everything AppBar", WS_POPUP, rc.left, rc.top, rc.right - rc.left,
        rc.bottom - rc.top, nullptr, nullptr, g_module, nullptr);
    if (!bar.hwnd) {
        Wh_Log(L"CreateWindowExW (appbar) failed: %u", GetLastError());
        return false;
    }
    SetLayeredWindowAttributes(bar.hwnd, 0, 0, LWA_ALPHA);  // invisible, but "visible"

    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = bar.hwnd;
    abd.uCallbackMessage = kMsgAppBarCallback;
    if (!SHAppBarMessage(ABM_NEW, &abd)) {
        Wh_Log(L"ABM_NEW failed for edge %u", edge);
        DestroyWindow(bar.hwnd);
        bar.hwnd = nullptr;
        return false;
    }

    ShowWindow(bar.hwnd, SW_SHOWNOACTIVATE);
    SetAppBarPosition(base, bar);
    return true;
}

void RemoveAppBar(AppBar& bar) {
    if (!bar.hwnd) {
        return;
    }
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = bar.hwnd;
    SHAppBarMessage(ABM_REMOVE, &abd);
    DestroyWindow(bar.hwnd);
    bar.hwnd = nullptr;
}

// Brings one monitor's set in line with a wish, touching only the edges that
// differ: an appbar that stays is re-positioned, not re-registered. Every
// shell call here changes the work area and makes every window re-layout,
// so the fewer the better.
void UpdateAppBarSet(AppBarSet& set, const AppBarWish& wish) {
    set.baseWorkArea = wish.baseWorkArea;
    set.taskbarEdge = wish.taskbarEdge;
    for (int e = 0; e < 4; e++) {
        int wanted = wish.thickness[e];
        AppBar* bar = nullptr;
        for (AppBar& candidate : set.bars) {
            if (candidate.edge == static_cast<UINT>(e)) {
                bar = &candidate;
                break;
            }
        }
        bool anchored = e == wish.taskbarEdge;
        if (wanted <= 0) {
            if (bar) {
                RemoveAppBar(*bar);
                set.bars.erase(set.bars.begin() + (bar - set.bars.data()));
            }
        } else if (bar) {
            bar->thickness = wanted;
            bar->anchored = anchored;
            SetAppBarPosition(set.baseWorkArea, *bar);
        } else {
            AppBar created;
            created.anchored = anchored;
            if (CreateAppBar(set.baseWorkArea, static_cast<UINT>(e), wanted, created)) {
                set.bars.push_back(created);
            }
        }
        set.thickness[e] = wanted;
    }
    RecordWorkArea(set);
    Wh_Log(L"monitor %p: %u appbar(s) in place", wish.monitor,
           static_cast<unsigned>(set.bars.size()));
}

void RemoveAppBarSet(AppBarSet& set) {
    for (AppBar& bar : set.bars) {
        RemoveAppBar(bar);
    }
    set.bars.clear();
}

void RemoveAllAppBars(std::vector<AppBarSet>& sets) {
    for (AppBarSet& set : sets) {
        RemoveAppBarSet(set);
    }
    sets.clear();
}

// Tells the UI thread what is in place.
void PublishAppBarStatus(const std::vector<AppBarSet>& sets) {
    std::vector<AppBarStatus> status;
    for (const AppBarSet& set : sets) {
        status.push_back({set.monitor, set.baseWorkArea});
    }
    std::lock_guard<std::mutex> lock(g_shellMutex);
    g_appBarStatus = std::move(status);
}

// Keeps one appbar set per monitor, re-positioning only when the wished
// strips changed or when the shell changed the
// work area since the last pass (taskbar moved/resized, another appbar came
// or went) or told us so through ABN_POSCHANGED. Never fights the shell.
void ReconcileAppBars(std::vector<AppBarSet>& sets, ULONGLONG& lastWorkAreaTick,
                      std::vector<AppBarWish>& lastWishes) {
    std::vector<AppBarWish> wishes;
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        wishes = g_appBarWishes;
    }

    // A changed wish list gets its positions right away, not after the
    // retry interval.
    bool wishesChanged = wishes.size() != lastWishes.size();
    for (size_t i = 0; !wishesChanged && i < wishes.size(); i++) {
        wishesChanged = wishes[i].monitor != lastWishes[i].monitor ||
                        !EqualRect(&wishes[i].baseWorkArea, &lastWishes[i].baseWorkArea) ||
                        !std::equal(wishes[i].thickness, wishes[i].thickness + 4,
                                    lastWishes[i].thickness) ||
                        wishes[i].taskbarEdge != lastWishes[i].taskbarEdge;
    }
    if (wishesChanged) {
        lastWorkAreaTick = 0;
    }
    lastWishes = wishes;

    bool dirty = g_appBarsDirty.exchange(false);
    ULONGLONG now = GetTickCount64();

    for (const AppBarWish& wish : wishes) {
        AppBarSet* set = nullptr;
        for (AppBarSet& candidate : sets) {
            if (candidate.monitor == wish.monitor) {
                set = &candidate;
                break;
            }
        }
        if (!set) {
            AppBarSet created;
            created.monitor = wish.monitor;
            sets.push_back(std::move(created));
            set = &sets.back();
            UpdateAppBarSet(*set, wish);
            continue;
        }
        if (!EqualRect(&set->baseWorkArea, &wish.baseWorkArea) ||
            !std::equal(wish.thickness, wish.thickness + 4, set->thickness) ||
            set->taskbarEdge != wish.taskbarEdge) {
            UpdateAppBarSet(*set, wish);
            continue;
        }
        if (set->bars.empty()) {
            continue;
        }

        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoW(wish.monitor, &mi)) {
            continue;
        }
        bool changed = !EqualRect(&mi.rcWork, &set->workAreaAfterApply);
        if ((changed || dirty) && now - lastWorkAreaTick >= kWorkAreaRetryMs) {
            if (changed) {
                Wh_Log(L"monitor %p: work area changed under the appbars to (%ld,%ld,%ld,%ld), "
                       L"re-applying",
                       wish.monitor, mi.rcWork.left, mi.rcWork.top, mi.rcWork.right,
                       mi.rcWork.bottom);
            }
            SetAppBarPositions(*set);
            lastWorkAreaTick = now;
        }
    }

    // Monitors that are gone take their appbars with them.
    for (size_t i = 0; i < sets.size();) {
        HMONITOR monitor = sets[i].monitor;
        bool present = false;
        for (const AppBarWish& wish : wishes) {
            present = present || wish.monitor == monitor;
        }
        if (present) {
            i++;
        } else {
            RemoveAppBarSet(sets[i]);
            sets.erase(sets.begin() + i);
        }
    }

    PublishAppBarStatus(sets);
}

// ABM_GETSTATE / ABM_GETTASKBARPOS, for the UI thread to read. Two
// synchronous calls into the taskbar thread, so only when something asked
// (a taskbar event, an ABN_* notification) or as a slow fallback.
void RefreshShellState() {
    ShellState state;
    APPBARDATA stateData{};
    stateData.cbSize = sizeof(stateData);
    UINT shellFlags = static_cast<UINT>(SHAppBarMessage(ABM_GETSTATE, &stateData));
    state.autoHide = (shellFlags & ABS_AUTOHIDE) != 0;
    {
        static int lastLogged = -1;
        if (lastLogged != static_cast<int>(state.autoHide)) {
            lastLogged = state.autoHide;
            Wh_Log(L"taskbar auto-hide %s", state.autoHide ? L"on" : L"off");
        }
    }

    APPBARDATA posData{};
    posData.cbSize = sizeof(posData);
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &posData) && posData.uEdge <= ABE_BOTTOM) {
        state.primaryEdge = static_cast<int>(posData.uEdge);
        state.primaryRect = posData.rc;
    }

    std::lock_guard<std::mutex> lock(g_shellMutex);
    g_shellState = state;
}

DWORD WINAPI AppBarThreadProc(LPVOID) {
    SetThreadPerMonitorDpiAware();

    std::vector<AppBarSet> sets;
    std::vector<AppBarWish> lastWishes;
    ULONGLONG lastWorkAreaTick = 0;
    ULONGLONG lastStateTick = 0;

    RefreshShellState();
    SetEvent(g_shellStateReady);

    // Not the mod's stop event: the appbars (each removal is a work-area
    // change that keeps the taskbar thread busy) are only taken down once
    // the UI thread says so - after the taskbar has been handed back.
    HANDLE handles[] = {g_appBarStop, g_appBarWake};
    for (;;) {
        DWORD wait = MsgWaitForMultipleObjectsEx(2, handles, 1000, QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) {
            break;
        }

        // The appbar windows live on this thread: their ABN callbacks are
        // pumped here.
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        ULONGLONG now = GetTickCount64();
        if (g_shellStateDirty.exchange(false) || now - lastStateTick >= kShellStateRefreshMs) {
            RefreshShellState();
            lastStateTick = now;
        }
        ReconcileAppBars(sets, lastWorkAreaTick, lastWishes);
    }

    RemoveAllAppBars(sets);
    PublishAppBarStatus(sets);
    return 0;
}

// UI thread: publishes what every monitor should reserve and wakes the
// appbar thread. Cheap, called every tick.
void PublishAppBarWishes() {
    std::vector<AppBarWish> wishes;
    for (const Overlay& overlay : *g_overlays) {
        const Layout& layout = overlay.layout;
        AppBarWish wish;
        wish.monitor = layout.monitor;
        wish.baseWorkArea = AppBarBase(layout);
        DesiredThickness(layout, wish.thickness);
        wish.taskbarEdge = IsRectEmpty(&layout.taskbarRect) ? kEdgeNone : layout.edge;
        wishes.push_back(wish);
    }
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        g_appBarWishes = std::move(wishes);
    }
    if (g_appBarWake) {
        SetEvent(g_appBarWake);
    }
}

// UI thread: whether the appbar thread has this monitor's set in place for
// the given work area.
bool AppBarSetInPlace(HMONITOR monitor, const RECT& baseWorkArea) {
    std::lock_guard<std::mutex> lock(g_shellMutex);
    for (const AppBarStatus& status : g_appBarStatus) {
        if (status.monitor == monitor) {
            return EqualRect(&status.baseWorkArea, &baseWorkArea) != FALSE;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Keeping the taskbar-side tweaks across tree rebuilds - taskbar thread only
// ---------------------------------------------------------------------------
// When the taskbar rebuilds its XAML tree (a layout change), the stroke, fill
// and ears are gone with the old tree. The mod's posted requests only run
// once the taskbar thread pumps messages again, which can be a second after
// the new tree is already on screen. So what the mod wants on each taskbar is
// kept here, and a LayoutUpdated handler on the taskbar's XAML root content
// re-applies it from inside the taskbar's own layout pass - before the new
// tree is first presented.

// Longest the XAML-level veil keeps a taskbar's content invisible.
constexpr ULONGLONG kXamlVeilMaxMs = 1500;

// Counts the taskbar thread's rendering ticks while content is hidden or a
// recolor is waiting (CompositionTarget.Rendering; subscribing to it also
// keeps frames coming).
unsigned g_renderCounter = 0;
winrt::event_token g_renderingToken{};
bool g_renderingSubscribed = false;

void OnRendering(winrt::Windows::Foundation::IInspectable const&,
                 winrt::Windows::Foundation::IInspectable const&);

void EnsureRenderingSubscribed() {
    if (g_renderingSubscribed) {
        return;
    }
    g_renderingToken = wuxm::CompositionTarget::Rendering(OnRendering);
    g_renderingSubscribed = true;
}

void UnsubscribeRendering() {
    if (g_renderingSubscribed) {
        wuxm::CompositionTarget::Rendering(g_renderingToken);
        g_renderingSubscribed = false;
    }
}

void SetContentHidden(HWND taskbar, TaskbarWants& wants, bool hide) {
    auto content = TaskbarXamlContent(taskbar);
    if (!content) {
        return;
    }
    if (hide) {
        if (content.Opacity() != 0.0) {
            content.Opacity(0.0);
        }
        if (!wants.hidden) {
            wants.hidden = true;
            wants.hiddenSince = GetTickCount64();
            Wh_Log(L"taskbar %p: content hidden until whole", taskbar);
        }
        wants.lastWhole = false;
        wants.changedAtRender = g_renderCounter;
        EnsureRenderingSubscribed();
    } else if (wants.hidden) {
        content.ClearValue(wux::UIElement::OpacityProperty());
        wants.hidden = false;
        Wh_Log(L"taskbar %p: content shown (after %llu ms)", taskbar,
               GetTickCount64() - wants.hiddenSince);
    }
}

// The UI thread learns that a taskbar is whole only once that state has
// been rendered (CompositionTarget.Rendered), so the frame it shows on that
// signal appears in the same frame as the taskbar's new look.
std::vector<std::pair<HWND, int>> g_readyPending;
winrt::event_token g_readyRenderToken{};

void ReportReadyAfterRender(HWND taskbar, int edge) try {
    for (auto& entry : g_readyPending) {
        if (entry.first == taskbar) {
            entry.second = edge;
            return;
        }
    }
    g_readyPending.push_back({taskbar, edge});
    if (g_readyRenderToken) {
        return;
    }
    g_readyRenderToken = wuxm::CompositionTarget::Rendered(
        [](winrt::Windows::Foundation::IInspectable const&, wuxm::RenderedEventArgs const&) {
            wuxm::CompositionTarget::Rendered(g_readyRenderToken);
            g_readyRenderToken = {};
            auto pending = std::move(g_readyPending);
            g_readyPending.clear();
            HWND ctrl = g_ctrlWnd.load();
            for (auto const& entry : pending) {
                if (ctrl) {
                    PostMessageW(ctrl, kMsgTaskbarReady, static_cast<WPARAM>(entry.second + 1),
                                 reinterpret_cast<LPARAM>(entry.first));
                }
            }
        });
} catch (...) {
    Wh_Log(L"ReportReadyAfterRender: exception %08X", winrt::to_hresult().value);
}

// Recolors (the UI thread asks for one once everything has settled, or when
// Task View closes). A request waits for the next CompositionTarget.Rendering
// tick: the brush change made there is part of the frame that tick builds,
// and the frame window registered for it is presented in the same tick,
// right after the change - before the tick commits its frame to DWM. Both
// then reach DWM ahead of the same vertical blank and come up in one
// composition. (Presenting on CompositionTarget.Rendered instead trails by a
// frame or two: that event fires once the taskbar's frame is already out.)
struct PendingRecolor {
    HWND taskbar;
    UINT serial;
};
std::vector<PendingRecolor> g_recolorsWaiting;  // for the next Rendering tick

// The monitor-corner ears inside the taskbar go with the background: both
// off in Task View, both back together.
void SetEarsHidden(HWND taskbar, bool hidden) {
    auto corner = g_xamlCorners->find(taskbar);
    if (corner == g_xamlCorners->end()) {
        return;
    }
    for (auto const& ear : corner->second.ear) {
        if (ear) {
            ear.Opacity(hidden ? 0.0 : 1.0);
        }
    }
    for (auto const& line : corner->second.line) {
        if (line) {
            line.Opacity(hidden ? 0.0 : 1.0);
        }
    }
}

void QueueRecolor(std::vector<PendingRecolor>& list, HWND taskbar, UINT serial) {
    for (auto& entry : list) {
        if (entry.taskbar == taskbar) {
            entry.serial = std::max(entry.serial, serial);  // the later one covers it
            return;
        }
    }
    list.push_back({taskbar, serial});
}

// Forgets recolors asked for before a taskbar went transparent again (a
// move, Task View): they must not color it behind the UI thread's back.
void DropRecolors(HWND taskbar) {
    auto drop = [taskbar](std::vector<PendingRecolor>& list) {
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [taskbar](const PendingRecolor& r) { return r.taskbar == taskbar; }),
                   list.end());
    };
    drop(g_recolorsWaiting);
}

// Makes sure this tick renders a frame (so Rendered fires) even when the
// recolor changed nothing visible (the color was already on): a change of
// opacity by one millionth on the fill brush, or failing that on an ear -
// invisible, but it dirties the tree.
void NudgeRender(HWND taskbar) {
    static bool flip = false;
    flip = !flip;
    double value = flip ? 0.999999 : 1.0;
    if (auto fill = g_fills->find(taskbar); fill != g_fills->end() && fill->second.ourBrush) {
        fill->second.ourBrush.Opacity(value);
        return;
    }
    if (auto corner = g_xamlCorners->find(taskbar);
        corner != g_xamlCorners->end() && corner->second.ear[0]) {
        corner->second.ear[0].Opacity(value);
    }
}

// Rendering tick: for every taskbar with a recolor waiting, paints the
// background and shows the ears (part of the frame this tick builds), then
// presents the frame window registered for it and tells the UI thread.
void ApplyRecolorsInRenderingTick() {
    if (g_recolorsWaiting.empty() || g_unloading || !g_wants) {
        return;
    }
    auto waiting = std::move(g_recolorsWaiting);
    g_recolorsWaiting.clear();
    HWND ctrl = g_ctrlWnd.load();
    for (const PendingRecolor& recolor : waiting) {
        auto it = g_wants->find(recolor.taskbar);
        if (it != g_wants->end()) {
            TaskbarWants& wants = it->second;
            wants.fillSuspended = false;
            if (wants.fill) {
                FillOp op{recolor.taskbar, wants.fillArgb, false, false};
                ApplyFillOnTaskbarThread(&op);
            }
            SetEarsHidden(recolor.taskbar, false);
        }
        NudgeRender(recolor.taskbar);
        {
            std::lock_guard<std::mutex> lock(g_revealMutex);
            auto reveal = g_reveals.find(recolor.taskbar);
            if (reveal != g_reveals.end() && reveal->second.serial <= recolor.serial) {
                // Left registered when the present fails (the window was
                // resized under us, say): the UI thread then presents it
                // itself instead of counting it as revealed.
                const Reveal& r = reveal->second;
                for (auto const& shadow : r.shadow) {
                    if (shadow.hwnd) {
                        PresentFrame(shadow.hwnd, shadow.memDc, shadow.width, shadow.height, nullptr,
                                     255);
                    }
                }
                bool presented;
                if (r.composition) {
                    BOOL off = FALSE;
                    for (HWND strip : r.strips) {
                        if (strip) {
                            DwmSetWindowAttribute(strip, DWMWA_CLOAK, &off, sizeof(off));
                        }
                    }
                    presented =
                        SUCCEEDED(DwmSetWindowAttribute(r.frame, DWMWA_CLOAK, &off, sizeof(off)));
                } else {
                    presented = PresentFrame(r.frame, r.memDc, r.width, r.height, nullptr, 255);
                }
                if (presented) {
                    g_reveals.erase(reveal);
                }
            }
        }
        if (ctrl) {
            PostMessageW(ctrl, kMsgTaskbarRecolored, recolor.serial,
                         reinterpret_cast<LPARAM>(recolor.taskbar));
        }
    }
}

// UI thread: posts the recolor (with the request's serial, which the
// kMsgTaskbarRecolored answer carries) to the taskbar's thread.
void RequestTaskbarRecolor(HWND taskbar, UINT serial) {
    if (!g_taskbarXamlAvailable) {
        return;  // nothing of the mod inside the taskbar, and no hooks to drain at unload
    }
    PostToWindowThread(taskbar, [taskbar, serial] {
        if (g_unloading) {
            return;
        }
        QueueRecolor(g_recolorsWaiting, taskbar, serial);
        EnsureRenderingSubscribed();
    });
}

void WINAPI TransparentOnTaskbarThread(void* param) try {
    HWND taskbar = static_cast<HWND>(param);
    if (g_unloading || !g_wants) {
        return;
    }
    auto it = g_wants->find(taskbar);
    if (it == g_wants->end()) {
        return;
    }
    TaskbarWants& wants = it->second;
    DropRecolors(taskbar);
    wants.fillSuspended = true;
    if (wants.fill) {
        FillOp op{taskbar, wants.fillArgb & 0x00FFFFFF, false, false};
        ApplyFillOnTaskbarThread(&op);
    }
    SetEarsHidden(taskbar, true);
} catch (...) {
    Wh_Log(L"TransparentOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// UI thread: "background and ears transparent now" on the taskbar's thread.
// Sent (sent messages go ahead of everything queued) with a short wait, so
// an idle taskbar thread does it within the call; if it is busy, posted.
void RequestTaskbarTransparent(HWND taskbar) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    if (RunFromWindowThreadTimeout(taskbar, TransparentOnTaskbarThread, taskbar, 30)) {
        return;
    }
    PostToWindowThread(taskbar, [taskbar] { TransparentOnTaskbarThread(taskbar); });
}

// Re-applies what is wanted on one taskbar; a no-op (a few cheap checks)
// while everything is in place. Returns whether it all is, and tells the UI
// thread (kMsgTaskbarReady, with the edge the ears are built for) whenever
// that becomes true - or again when `requested` - so a waiting frame can be
// shown the moment the taskbar is whole.
// ---------------------------------------------------------------------------
// Taskbar theme: light for a light frame color, dark for a dark one (the
// title-bar color source), so the labels stay readable. RequestedTheme on
// the island's root content re-resolves every theme-resource brush under it
// (labels, monochrome icons, hover highlights, the tray) for this monitor's
// taskbar only. The root's own value is captured when a theme is first
// forced and put back when none is wanted - the value for the current
// Windows mode if the mode changed meanwhile (the taskbar rewrites the
// root's value on a mode change, which a forced value can hide). Ported from
// Top Taskbar Bleed. Taskbar thread only.
// ---------------------------------------------------------------------------
struct ThemeState {
    winrt::weak_ref<wux::FrameworkElement> root;
    int forced = kThemeNone;
    wux::ElementTheme original = wux::ElementTheme::Default;
    bool originalSystemLight = false;
};
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, ThemeState>> g_themes{std::in_place};

wux::FrameworkElement ThemeRootOf(HWND taskbar, ThemeState& state) {
    if (auto root = state.root.get()) {
        return root;
    }
    auto xamlRoot = XamlRootOfTaskbarWindow(taskbar);
    if (!xamlRoot) {
        return nullptr;
    }
    auto root = xamlRoot.Content().try_as<wux::FrameworkElement>();
    state.root = root;
    return root;
}

wux::ElementTheme RestoreThemeValue(ThemeState& state) {
    bool light = SystemUsesLightTheme();
    if (state.original != wux::ElementTheme::Default && light != state.originalSystemLight) {
        state.original = light ? wux::ElementTheme::Light : wux::ElementTheme::Dark;
        state.originalSystemLight = light;
    }
    return state.original;
}

void ApplyThemeOnTaskbarThread(HWND taskbar, int want) try {
    if (g_unloading || !g_themes) {
        return;
    }
    ThemeState& state = (*g_themes)[taskbar];
    auto root = ThemeRootOf(taskbar, state);
    if (!root) {
        return;
    }
    if (want == kThemeNone) {
        if (state.forced != kThemeNone) {
            state.forced = kThemeNone;
            root.RequestedTheme(RestoreThemeValue(state));
            Wh_Log(L"taskbar %p: theme restored to %d", taskbar, static_cast<int>(state.original));
        }
        return;
    }
    wux::ElementTheme value =
        want == kThemeLight ? wux::ElementTheme::Light : wux::ElementTheme::Dark;
    if (state.forced == kThemeNone) {
        state.original = root.RequestedTheme();
        state.originalSystemLight = SystemUsesLightTheme();
    }
    bool change = state.forced != want;
    state.forced = want;
    if (root.RequestedTheme() != value) {
        root.RequestedTheme(value);  // set, or set again after the taskbar rewrote it
        if (change) {
            Wh_Log(L"taskbar %p: theme %s", taskbar, want == kThemeLight ? L"light" : L"dark");
        }
    }
} catch (...) {
    Wh_Log(L"ApplyThemeOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

void RestoreThemesOnTaskbarThread() try {
    if (!g_themes) {
        return;
    }
    for (auto& entry : *g_themes) {
        ThemeState& state = entry.second;
        if (state.forced == kThemeNone) {
            continue;
        }
        state.forced = kThemeNone;
        if (auto root = state.root.get()) {
            root.RequestedTheme(RestoreThemeValue(state));
            Wh_Log(L"taskbar %p: theme restored", entry.first);
        }
    }
    g_themes.reset();
} catch (...) {
    Wh_Log(L"RestoreThemesOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

bool ReapplyWants(HWND taskbar, bool requested) try {
    if (g_unloading) {
        return false;
    }
    auto it = g_wants->find(taskbar);
    if (it == g_wants->end()) {
        return false;
    }
    TaskbarWants& wants = it->second;
    bool found = true;
    bool clean = true;
    bool placed = true;
    bool earsRight = true;  // what the XAML-level veil is about: the corners
    int edge = kEdgeNone;
    if (wants.strokeHidden) {
        StrokeOp op{taskbar, false, false};
        HideStrokeOnTaskbarThread(&op);
        found = found && op.found;
        clean = clean && !op.changed;
    }
    if (wants.fill) {
        FillOp op{taskbar, wants.fillSuspended ? (wants.fillArgb & 0x00FFFFFF) : wants.fillArgb,
                  false, false};
        ApplyFillOnTaskbarThread(&op);
        found = found && op.found;
        clean = clean && !op.changed;
    }
    if (wants.theme != kThemeNone) {
        ApplyThemeOnTaskbarThread(taskbar, wants.theme);
    }
    if (wants.ears) {
        XamlCornerOp op{taskbar,      wants.edge, wants.radiusDips, wants.earInsetDips,
                        wants.earRgb, false,      false,            false};
        ApplyXamlCornersOnTaskbarThread(&op);
        found = found && op.found;
        clean = clean && !op.changed;
        placed = op.placed;
        earsRight = op.found && !op.changed && op.placed;
        auto corner = g_xamlCorners->find(taskbar);
        if (corner != g_xamlCorners->end()) {
            edge = corner->second.edge;
        }
    }
    bool whole = found && clean && placed;

    // The XAML-level veil goes up here whenever the corners aren't right
    // (ears missing, just added or reconfigured in this pass, or not yet on
    // the window's corners). It comes down in OnRendering, once the tree has
    // been whole and untouched across two rendered frames: a change made
    // during a layout pass is not in that pass's frame yet.
    if (!earsRight) {
        if (!wants.hidden || wants.changedAtRender != g_renderCounter) {
            SetContentHidden(taskbar, wants, true);
        }
    }
    wants.lastWhole = whole;

    if (whole && (!wants.lastReady || requested)) {
        // Logged when the tree becomes whole or its edge changes, not for
        // every requested pass (a color animation requests one per step).
        if (!wants.lastReady || edge != wants.loggedEdge) {
            Wh_Log(L"taskbar %p: tree whole (ears for edge %d)", taskbar, edge);
            wants.loggedEdge = edge;
        }
        ReportReadyAfterRender(taskbar, edge);
    }
    wants.lastReady = whole;
    return whole;
} catch (...) {
    Wh_Log(L"ReapplyWants: exception %08X", winrt::to_hresult().value);
    return false;
}

// Once per rendering tick while content is hidden or a recolor is waiting.
// Changes made in this handler are part of the frame about to be rendered.
void OnRendering(winrt::Windows::Foundation::IInspectable const&,
                 winrt::Windows::Foundation::IInspectable const&) try {
    g_renderCounter++;
    ApplyRecolorsInRenderingTick();
    bool anyHidden = !g_recolorsWaiting.empty();
    for (auto& entry : *g_wants) {
        HWND taskbar = entry.first;
        TaskbarWants& wants = entry.second;
        if (!wants.hidden) {
            continue;
        }
        if (g_unloading || !IsWindow(taskbar)) {
            SetContentHidden(taskbar, wants, false);
            continue;
        }
        if (!wants.lastWhole) {
            ReapplyWants(taskbar, false);  // no layout pass came by itself
        }
        bool settled = wants.lastWhole && g_renderCounter >= wants.changedAtRender + 2;
        bool overdue = GetTickCount64() - wants.hiddenSince >= kXamlVeilMaxMs;
        if (settled || overdue) {
            if (overdue && !settled) {
                Wh_Log(L"taskbar %p: shown anyway (whole %d, renders since change %u)", taskbar,
                       wants.lastWhole ? 1 : 0, g_renderCounter - wants.changedAtRender);
            }
            SetContentHidden(taskbar, wants, false);
        } else {
            anyHidden = true;
        }
    }
    if (!anyHidden) {
        UnsubscribeRendering();
    }
} catch (...) {
    Wh_Log(L"OnRendering: exception %08X", winrt::to_hresult().value);
}

// WM_WINDOWPOSCHANGED on a taskbar window (WH_CALLWNDPROC hook, taskbar
// thread): the earliest moment a layout change can be seen from inside. If
// the window is now on another edge than the ears are built for, the content
// goes invisible before anything of the new position is rendered.
void OnTaskbarWindowPosChanged(HWND hwnd, const WINDOWPOS* pos) {
    if (g_unloading || ((pos->flags & SWP_NOMOVE) && (pos->flags & SWP_NOSIZE))) {
        return;
    }
    auto it = g_wants->find(hwnd);
    if (it == g_wants->end()) {
        return;
    }
    // Edge or size changed: the frame on this monitor goes away with the
    // taskbar and comes back once the tree is whole again (a slide along the
    // same edge - auto-hide - is left alone).
    auto corner = g_xamlCorners->find(hwnd);
    int oldEdge = corner != g_xamlCorners->end() ? corner->second.edge : kEdgeNone;
    int edge = LiveTaskbarEdge(hwnd, oldEdge);
    if (edge == oldEdge && (pos->flags & SWP_NOSIZE)) {
        return;
    }
    Wh_Log(L"taskbar %p: window now on edge %d (ears for %d)", hwnd, edge, oldEdge);
    TaskbarWants& wants = it->second;
    wants.lastReady = false;  // report whole again, for the new place
    DropRecolors(hwnd);
    if (wants.fill && !wants.fillSuspended) {
        // Transparent right away; the color comes back with the frame.
        wants.fillSuspended = true;
        FillOp op{hwnd, wants.fillArgb & 0x00FFFFFF, false, false};
        ApplyFillOnTaskbarThread(&op);
        Wh_Log(L"taskbar %p: background made transparent for the move", hwnd);
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        PostMessageW(ctrl, kMsgTaskbarMoving, 0, reinterpret_cast<LPARAM>(hwnd));
    }
    if (it->second.ears && corner != g_xamlCorners->end()) {
        SetContentHidden(hwnd, it->second, true);
    }
}

// Puts the LayoutUpdated handler on the taskbar's current XAML root content
// (moving it when the content was replaced).
void WatchTaskbarLayout(HWND taskbar) try {
    if (g_unloading) {
        return;
    }
    TaskbarWants& wants = (*g_wants)[taskbar];
    auto content = TaskbarXamlContent(taskbar);
    auto element = content ? content.try_as<wux::FrameworkElement>() : nullptr;
    auto watched = wants.watched.get();
    if (watched == element) {
        return;
    }
    if (watched && wants.layoutToken) {
        watched.LayoutUpdated(wants.layoutToken);
    }
    wants.watched = nullptr;
    wants.layoutToken = {};
    if (!element) {
        return;
    }
    wants.layoutToken = element.LayoutUpdated(
        [taskbar](winrt::Windows::Foundation::IInspectable const&,
                  winrt::Windows::Foundation::IInspectable const&) { ReapplyWants(taskbar, false); });
    wants.watched = element;
    wants.lastReady = false;  // a new tree: report again once it is whole
    Wh_Log(L"taskbar %p: watching its layout (%s content)", taskbar,
           watched ? L"replaced" : L"first");
} catch (...) {
    Wh_Log(L"WatchTaskbarLayout: exception %08X", winrt::to_hresult().value);
}

// The XamlRoot outlives its content: its Changed event (size, scale,
// visibility) is where a replaced content tree is picked up and watched.
void WatchTaskbarRoot(HWND taskbar) try {
    if (g_unloading) {
        return;
    }
    TaskbarWants& wants = (*g_wants)[taskbar];
    auto root = XamlRootOfTaskbarWindow(taskbar);
    auto watched = wants.rootWatched.get();
    if (watched == root) {
        return;
    }
    if (watched && wants.rootToken) {
        watched.Changed(wants.rootToken);
    }
    wants.rootWatched = nullptr;
    wants.rootToken = {};
    if (!root) {
        return;
    }
    wants.rootToken = root.Changed([taskbar](wux::XamlRoot const&,
                                             wux::XamlRootChangedEventArgs const&) {
        Wh_Log(L"taskbar %p: XamlRoot changed", taskbar);
        WatchTaskbarLayout(taskbar);
        ReapplyWants(taskbar, false);
    });
    wants.rootWatched = root;
    Wh_Log(L"taskbar %p: watching its XamlRoot (%s)", taskbar, watched ? L"replaced" : L"first");
} catch (...) {
    Wh_Log(L"WatchTaskbarRoot: exception %08X", winrt::to_hresult().value);
}

// Takes every LayoutUpdated handler off again (unload). Must complete before
// the mod's code goes away: a handler left behind would be called into
// unloaded code.
// Every removal is guarded on its own: a handler left behind would be
// called into unloaded code, so one XAML call throwing (a tree in the middle
// of a rebuild) must not skip the others. The handlers come off first; only
// then is the content, which may be in that state, touched.
void WINAPI UnwatchTaskbarsOnTaskbarThread(void*) {
    try {
        UnsubscribeRendering();
    } catch (...) {
        Wh_Log(L"UnwatchTaskbars: Rendering unsubscribe threw %08X", winrt::to_hresult().value);
    }
    try {
        if (g_readyRenderToken) {
            wuxm::CompositionTarget::Rendered(g_readyRenderToken);
            g_readyRenderToken = {};
        }
    } catch (...) {
        Wh_Log(L"UnwatchTaskbars: Rendered unsubscribe threw %08X", winrt::to_hresult().value);
    }
    g_readyPending.clear();
    g_recolorsWaiting.clear();
    for (auto& [taskbar, wants] : *g_wants) {
        try {
            if (auto watched = wants.watched.get(); watched && wants.layoutToken) {
                watched.LayoutUpdated(wants.layoutToken);
            }
        } catch (...) {
            Wh_Log(L"taskbar %p: LayoutUpdated unsubscribe threw %08X", taskbar,
                   winrt::to_hresult().value);
        }
        try {
            if (auto root = wants.rootWatched.get(); root && wants.rootToken) {
                root.Changed(wants.rootToken);
            }
        } catch (...) {
            Wh_Log(L"taskbar %p: XamlRoot.Changed unsubscribe threw %08X", taskbar,
                   winrt::to_hresult().value);
        }
        wants.watched = nullptr;
        wants.layoutToken = {};
        wants.rootWatched = nullptr;
        wants.rootToken = {};
    }
    for (auto& [taskbar, wants] : *g_wants) {
        try {
            if (wants.hidden && IsWindow(taskbar)) {
                SetContentHidden(taskbar, wants, false);
            }
        } catch (...) {
            Wh_Log(L"taskbar %p: content restore threw %08X", taskbar, winrt::to_hresult().value);
        }
    }
    g_wants->clear();
}

// Unload: one trip to the taskbar thread hands everything back at once -
// handlers off, content shown, ears removed, background and stroke restored -
// so the taskbar changes in a single frame of its own.
winrt::event_token g_restoredRenderToken{};

// Drops the Rendered subscription if it never fired (the handler must not
// outlive the mod).
void WINAPI UnsubscribeRestoredRenderOnTaskbarThread(void*) try {
    if (g_restoredRenderToken) {
        wuxm::CompositionTarget::Rendered(g_restoredRenderToken);
        g_restoredRenderToken = {};
    }
} catch (...) {
}

void WINAPI RestoreTaskbarsOnTaskbarThread(void*) try {
    UnwatchTaskbarsOnTaskbarThread(nullptr);
    RemoveXamlCornersOnTaskbarThread(nullptr);
    RestoreFillsOnTaskbarThread(nullptr);
    RestoreStrokesOnTaskbarThread(nullptr);
    RestoreThemesOnTaskbarThread();
    // The maps go away here, on the thread their XAML references belong to.
    g_wants.reset();
    g_strokes.reset();
    g_fills.reset();
    g_xamlCorners.reset();

    // Tell the UI thread once the handed-back taskbar has actually been
    // rendered, so the frames can go in the same instant it changes.
    g_restoredRenderToken = wuxm::CompositionTarget::Rendered(
        [](winrt::Windows::Foundation::IInspectable const&, wuxm::RenderedEventArgs const&) {
            wuxm::CompositionTarget::Rendered(g_restoredRenderToken);
            g_restoredRenderToken = {};
            if (g_taskbarRestored) {
                SetEvent(g_taskbarRestored);
            }
        });
} catch (...) {
    Wh_Log(L"RestoreTaskbarsOnTaskbarThread: exception %08X", winrt::to_hresult().value);
    if (g_taskbarRestored) {
        SetEvent(g_taskbarRestored);
    }
}

// One taskbar handed back while the mod runs: its monitor dropped out of
// the layouts (Monitors set to the primary one, a monitor unplugged while
// its taskbar window lives on) - ears, background, stroke and theme
// restored, the handlers off, as at unload but for this window alone.
void WINAPI RestoreTaskbarOnTaskbarThread(void* param) try {
    HWND taskbar = static_cast<HWND>(param);
    if (auto it = g_wants->find(taskbar); it != g_wants->end()) {
        TaskbarWants& wants = it->second;
        try {
            if (auto watched = wants.watched.get(); watched && wants.layoutToken) {
                watched.LayoutUpdated(wants.layoutToken);
            }
            if (auto root = wants.rootWatched.get(); root && wants.rootToken) {
                root.Changed(wants.rootToken);
            }
            if (wants.hidden && IsWindow(taskbar)) {
                SetContentHidden(taskbar, wants, false);
            }
        } catch (...) {
            Wh_Log(L"taskbar %p: hand-back unsubscribe threw %08X", taskbar,
                   winrt::to_hresult().value);
        }
        g_wants->erase(it);
    }
    if (auto it = g_xamlCorners->find(taskbar); it != g_xamlCorners->end()) {
        RemoveEars(it->second);
        g_xamlCorners->erase(it);
    }
    if (auto it = g_fills->find(taskbar); it != g_fills->end()) {
        RestoreFillOnTaskbarThread(taskbar, it->second);
        g_fills->erase(it);
    }
    if (auto it = g_strokes->find(taskbar); it != g_strokes->end()) {
        StrokeState& state = it->second;
        if (state.hidden) {
            if (auto stroke = state.element.get()) {
                stroke.Opacity(state.originalOpacity);
            }
        }
        g_strokes->erase(it);
    }
    if (g_themes) {
        if (auto it = g_themes->find(taskbar); it != g_themes->end()) {
            ThemeState& state = it->second;
            if (state.forced != kThemeNone) {
                if (auto root = state.root.get()) {
                    root.RequestedTheme(RestoreThemeValue(state));
                }
            }
            g_themes->erase(it);
        }
    }
    g_readyPending.erase(std::remove_if(g_readyPending.begin(), g_readyPending.end(),
                                        [taskbar](const std::pair<HWND, int>& p) {
                                            return p.first == taskbar;
                                        }),
                         g_readyPending.end());
    Wh_Log(L"taskbar %p: handed back (no frame on its monitor)", taskbar);
} catch (...) {
    Wh_Log(L"RestoreTaskbarOnTaskbarThread: exception %08X", winrt::to_hresult().value);
}

// UI thread: hands one taskbar back and forgets what the UI side kept on it.
void HandBackTaskbar(HWND taskbar) {
    if (!IsWindow(taskbar)) {
        return;
    }
    if (!RunFromWindowThreadTimeout(taskbar, RestoreTaskbarOnTaskbarThread, taskbar, 1000)) {
        Wh_Log(L"taskbar %p: hand-back not run (thread busy)", taskbar);
    }
    g_taskbarReady.erase(taskbar);
}

// ---------------------------------------------------------------------------
// Taskbar XAML drivers (mod UI thread -> taskbar thread)
// ---------------------------------------------------------------------------

// A pass is requested on a taskbar (posted to its thread) unless a request is
// already out; the answer arrives through kMsgPassResult. A pass is re-sent
// when it reported "not found" (the tree is being rebuilt), every
// kTaskbarXamlRetryMs to re-assert it, or right away when `force` is set.
void RequestPass(XamlPass pass, HWND taskbar, ULONGLONG now, bool force,
                 std::function<bool()> op) {
    PassState& state = g_passState[pass][taskbar];
    if (state.outstanding && now - state.sentTick < kTaskbarCallLostMs) {
        return;
    }
    bool due = force || (!state.done && now - state.answerTick >= kPassRetryMs) ||
               (state.done && now - state.doneTick >= kTaskbarXamlRetryMs);
    if (!due) {
        return;
    }
    UINT serial = ++g_requestSerial;
    HWND ctrl = g_ctrlWnd.load();
    if (PostToWindowThread(taskbar, [op, pass, serial, taskbar, ctrl] {
            bool found = false;
            try {
                found = op();
            } catch (...) {
            }
            if (ctrl) {
                PostMessageW(ctrl, kMsgPassResult, MAKEWPARAM(pass | (found ? 0x100 : 0), serial),
                             reinterpret_cast<LPARAM>(taskbar));
            }
        })) {
        state.outstanding = true;
        state.sentTick = now;
        state.serial = serial;
    }
}

// kMsgPassResult handler (UI thread).
void OnPassResult(WPARAM wParam, LPARAM lParam) {
    int pass = LOWORD(wParam) & 0xFF;
    bool found = (LOWORD(wParam) & 0x100) != 0;
    UINT serial = HIWORD(wParam);
    HWND taskbar = reinterpret_cast<HWND>(lParam);
    if (pass < 0 || pass >= kPassCount) {
        return;
    }
    auto it = g_passState[pass].find(taskbar);
    if (it == g_passState[pass].end() || (it->second.serial & 0xFFFF) != serial) {
        return;  // a request from before a layout change
    }
    PassState& state = it->second;
    ULONGLONG now = GetTickCount64();
    state.outstanding = false;
    state.answerTick = now;
    state.done = found;
    if (found) {
        state.doneTick = now;
        if (pass == kPassCorners) {
            g_xamlCornersOwned = true;
        }
    }
}

// Forgets every pass state: after a layout change each taskbar has to be
// confirmed again (answers to older requests are dropped by their serial).
void ResetPassStates() {
    for (auto& states : g_passState) {
        states.clear();
    }
}

void HideStrokes(ULONGLONG now) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    for (const Overlay& overlay : *g_overlays) {
        HWND taskbar = overlay.layout.taskbar;
        if (!taskbar) {
            continue;
        }
        RequestPass(kPassStroke, taskbar, now, false, [taskbar] {
            (*g_wants)[taskbar].strokeHidden = true;
            WatchTaskbarRoot(taskbar);
            WatchTaskbarLayout(taskbar);
            StrokeOp op{taskbar, false, false};
            HideStrokeOnTaskbarThread(&op);
            ReapplyWants(taskbar, true);
            return op.found;
        });
    }
}

// The brush painted onto the taskbar background: the frame color at the
// frame's opacity, so the taskbar is the frame's fourth side.
DWORD CustomColorArgb(const Overlay& overlay) {
    BYTE alpha = static_cast<BYTE>(g_settings.opacity * 255 / 100);
    COLORREF rgb = overlay.painted ? overlay.displayColor : FrameColorFor(overlay);
    return (static_cast<DWORD>(alpha) << 24) | (static_cast<DWORD>(GetRValue(rgb)) << 16) |
           (static_cast<DWORD>(GetGValue(rgb)) << 8) | GetBValue(rgb);
}

// What the taskbar on a monitor is painted with: transparent, so that the
// acrylic frame under it shows through (with either fill), or the frame
// color over a layered frame, which stops at the taskbar. (Whenever the
// frame is off - Task View, a full-screen window - the taskbar is
// transparent as well, as it is in Task View anyway.)
DWORD FillArgbFor(const Overlay& overlay) {
    DWORD argb = CustomColorArgb(overlay);
    if (overlay.comp && !IsRectEmpty(&overlay.layout.taskbarRect)) {
        argb &= 0x00FFFFFF;
    }
    return argb;
}

void ApplyFills(ULONGLONG now) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    for (Overlay& overlay : *g_overlays) {
        HWND taskbar = overlay.layout.taskbar;
        if (!taskbar) {
            continue;
        }
        DWORD argb = FillArgbFor(overlay);
        bool force = !overlay.fillAsked || argb != overlay.lastFillArgb;  // a new color is applied at once
        overlay.fillAsked = true;
        overlay.lastFillArgb = argb;
        RequestPass(kPassFill, taskbar, now, force, [taskbar, argb] {
            TaskbarWants& wants = (*g_wants)[taskbar];
            wants.fill = true;
            wants.fillArgb = argb;
            WatchTaskbarRoot(taskbar);
            WatchTaskbarLayout(taskbar);
            FillOp op{taskbar, wants.fillSuspended ? (argb & 0x00FFFFFF) : argb, false, false};
            ApplyFillOnTaskbarThread(&op);
            ReapplyWants(taskbar, true);
            return op.found;
        });
    }
}

// Radius in DIPs for the XAML ears (XAML scales by the taskbar's DPI itself).
float XamlCornerRadiusDips(const Layout& layout) {
    // An auto-hide taskbar's corners are the frame's (see DrawMonitorEars).
    if (!g_settings.monitorCorners || layout.monitorCornerRadius <= 0 || layout.autoHide) {
        return 0.0f;
    }
    if (g_settings.scaleWithDpi) {
        return static_cast<float>(g_settings.monitorCornerRadius);
    }
    return static_cast<float>(layout.monitorCornerRadius) * 96.0f / static_cast<float>(layout.dpi);
}

void ApplyXamlCorners(ULONGLONG now) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    for (const Overlay& overlay : *g_overlays) {
        const Layout& layout = overlay.layout;
        HWND taskbar = layout.taskbar;
        if (!taskbar) {
            continue;  // all four ears are painted into the frame itself
        }
        int edge = layout.edge;
        float radius = XamlCornerRadiusDips(layout);
        // One device pixel: the bezel ring's thickness, where the arcs
        // start (like the frame's own ears).
        float inset = 96.0f / static_cast<float>(layout.dpi);
        DWORD rgb = kMonitorCornerColor;
        RequestPass(kPassCorners, taskbar, now, false, [taskbar, edge, radius, inset, rgb] {
            TaskbarWants& wants = (*g_wants)[taskbar];
            wants.ears = true;  // the bezel lines at least; the ears with a radius
            wants.edge = edge;
            wants.radiusDips = radius;
            wants.earInsetDips = inset;
            wants.earRgb = rgb;
            WatchTaskbarRoot(taskbar);
            WatchTaskbarLayout(taskbar);
            XamlCornerOp op{taskbar, edge, radius, inset, rgb, false, false, false};
            ApplyXamlCornersOnTaskbarThread(&op);
            ReapplyWants(taskbar, true);
            return op.found;
        });
    }
}

// ---------------------------------------------------------------------------
// Periodic update (mod UI thread)
// ---------------------------------------------------------------------------

// An effect graph is loaded asynchronously, so CreateEffectFactory can't say
// whether the composition engine accepts it; a rejected graph renders
// nothing, silently. Once a frame's factories have all finished loading,
// this logs the outcome; a rejected graph falls back to the color fill.
void CheckEffectGraphs() {
    for (Overlay& overlay : *g_overlays) {
        if (!overlay.comp || overlay.comp->graphsChecked || overlay.comp->factories.empty()) {
            continue;
        }
        bool pending = false, rejected = false;
        winrt::hresult error{};
        for (auto const& factory : overlay.comp->factories) {
            try {
                auto status = factory.LoadStatus();
                if (status == wuc::CompositionEffectFactoryLoadStatus::Pending) {
                    pending = true;
                } else if (status != wuc::CompositionEffectFactoryLoadStatus::Success) {
                    rejected = true;
                    error = factory.ExtendedError();
                }
            } catch (winrt::hresult_error const& e) {
                rejected = true;
                error = e.code();
            }
        }
        if (pending) {
            if (++overlay.comp->graphTicksPending == 20) {
                Wh_Log(L"monitor %p: %zu effect graph(s) still loading", overlay.layout.monitor,
                       overlay.comp->factories.size());
            }
            continue;
        }
        overlay.comp->graphsChecked = true;
        if (!rejected) {
            Wh_Log(L"monitor %p: %zu effect graph(s) loaded", overlay.layout.monitor,
                   overlay.comp->factories.size());
            continue;
        }
        Wh_Log(L"monitor %p: effect graph rejected (%08X) - the color fill", overlay.layout.monitor,
               error.value);
        g_backdropUnavailable = true;
        for (Overlay& o : *g_overlays) {
            o.painted = false;  // rebuilt by this tick's paint
        }
        if (HWND ctrl = g_ctrlWnd.load()) {
            PostMessageW(ctrl, kMsgReload, 0, 0);
        }
    }
}

// DWM reads a window's host backdrop again only when a window beneath it
// presents new content there. Its own changes don't count: a window gone
// in a minimize animation, a shadow handed from one window to another or
// taken away, the wallpaper uncovered - all of that stayed in the bands
// until something else drew under the frame (a click on the desktop, for
// one). Nothing done to the frame itself helps: its visuals moved, its
// opacity or layered alpha changed, its DWM attributes set again, a
// SetWindowPos with SWP_FRAMECHANGED - DWM kept the stale backdrop through
// each (tested one by one on 10.0.26100).
//
// So each band of an acrylic frame has a "refresher" under everything: a
// layered window the size of the band sprite's input rectangle at the
// bottom of the z-order, at the lowest alpha so that nothing of it shows,
// which is repainted after every window event and, as a fallback, every
// tick. The repaint is a window presenting content under the frame, and
// the band reads the backdrop again.
LRESULT CALLBACK RefreshWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            if (dc) {
                // GDI marks what it draws as changed whether or not the
                // pixels differ.
                FillRect(dc, &ps.rcPaint, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// One refresher per band, each the band's input rectangle: created, moved
// or destroyed as the bands are rebuilt.
void SyncRefreshers(Overlay& overlay) {
    CompositionFrame& ac = *overlay.comp;
    const RECT& frame = overlay.layout.overlayRect;
    size_t count = std::min<size_t>(ac.backdropRects.size(), 4);
    for (size_t i = 0; i < 4; i++) {
        HWND& hwnd = ac.refreshers[i];
        if (i >= count) {
            if (hwnd) {
                DestroyWindow(hwnd);
                hwnd = nullptr;
            }
            continue;
        }
        const RECT& band = ac.backdropRects[i];
        int x = frame.left + band.left, y = frame.top + band.top;
        int w = band.right - band.left, h = band.bottom - band.top;
        if (hwnd) {
            SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
            continue;
        }
        hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                                   WS_EX_NOACTIVATE,
                               kRefreshClassName, L"", WS_POPUP, x, y, w, h, nullptr, nullptr,
                               g_module, nullptr);
        if (!hwnd) {
            Wh_Log(L"CreateWindowExW (backdrop refresher) failed: %u", GetLastError());
            continue;
        }
        SetLayeredWindowAttributes(hwnd, 0, kRefreshAlpha, LWA_ALPHA);
        BOOL excludedFromPeek = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_EXCLUDED_FROM_PEEK, &excludedFromPeek,
                              sizeof(excludedFromPeek));
        BOOL noTransitions = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noTransitions,
                              sizeof(noTransitions));
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        UpdateWindow(hwnd);
    }
}

// One repaint of every shown frame's refreshers. On a window event they
// are also put back under everything, in case a window was sent to the
// bottom since (it would have gone below them).
void RefreshBackdrops(bool reassertBottom) {
    for (Overlay& overlay : *g_overlays) {
        CompositionFrame* ac = overlay.comp.get();
        if (!ac || !overlay.shown) {
            continue;
        }
        for (HWND hwnd : ac->refreshers) {
            if (!hwnd) {
                continue;
            }
            if (reassertBottom && GetWindow(hwnd, GW_HWNDNEXT)) {
                SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            UpdateWindow(hwnd);
        }
    }
}

// A window event: refresh now, and every frame for a moment.
ULONGLONG g_refreshBurstEnd = 0;
bool g_refreshBurstRunning = false;
void StartRefreshBurst() {
    if (!g_overlaysBackdrop) {
        return;  // nothing samples the backdrop
    }
    g_refreshBurstEnd = GetTickCount64() + kRefreshBurstMs;
    RefreshBackdrops(true);
    if (!g_refreshBurstRunning) {
        if (HWND ctrl = g_ctrlWnd.load()) {
            if (SetTimer(ctrl, kRefreshTimerId, kRefreshIntervalMs, nullptr)) {
                g_refreshBurstRunning = true;
            }
        }
    }
}

void OnRefreshTimer(HWND ctrl) {
    RefreshBackdrops(false);
    if (GetTickCount64() >= g_refreshBurstEnd) {
        KillTimer(ctrl, kRefreshTimerId);
        g_refreshBurstRunning = false;
    }
}

// ---------------------------------------------------------------------------
// Frame color source: the maximized window's title bar
// ---------------------------------------------------------------------------
// With the "Title bar" source, each monitor's frame takes the color of the
// title bar of the maximized window on it (or of the window Aero Peek shows
// from a taskbar thumbnail): the fourth row of the window inside the rounded
// screen, read off the screen the way Top Taskbar Bleed reads it, reduced to
// its dominant color - caption text and buttons are too few pixels to win -
// with the columns covered by other windows left out (the peeked window is
// copied from its surface instead, see CaptureWindowRow). A window seen
// before gets its last color the moment it takes over; a row that differs
// from the color in use is read again every frame until two readings agree
// - the color the window read as last time counts as one - then the frame
// eases to it. With no maximized window the frame has the frame color. The color goes wherever the frame color goes: the
// solid fill, the acrylic's tint and luminosity, the taskbar's background.

// Whether a window is cloaked (on another virtual desktop, mostly).
bool IsCloakedWindow(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked != 0;
}

// Dominant color of 0x00RRGGBB pixels: the mode of a 5-bit-per-channel
// histogram - or, with no clear mode (a photo), the mean of the three
// largest bins - refined by two mean-shift passes so a tight cluster that
// straddles a bin edge still gives its center. From Top Taskbar Bleed.
// The histogram (1 MB) is kept per thread and only the bins a reading
// touched are cleared after it: readings come every frame while a color
// settles.
COLORREF DominantColor(const std::vector<uint32_t>& px) {
    struct Bin {
        uint32_t count = 0;
        uint64_t r = 0, g = 0, b = 0;
    };
    thread_local std::vector<Bin> bins(32768);
    thread_local std::vector<uint32_t> touched;
    struct Clear {
        ~Clear() {
            for (uint32_t key : touched) {
                bins[key] = Bin{};
            }
            touched.clear();
        }
    } clear;
    for (uint32_t v : px) {
        uint32_t b = v & 0xFF, g = (v >> 8) & 0xFF, r = (v >> 16) & 0xFF;
        uint32_t key = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
        Bin& bin = bins[key];
        if (bin.count == 0) {
            touched.push_back(key);
        }
        bin.count++;
        bin.r += r;
        bin.g += g;
        bin.b += b;
    }
    if (touched.empty()) {
        return 0;
    }
    std::sort(touched.begin(), touched.end(),
              [&](uint32_t a, uint32_t b) { return bins[a].count > bins[b].count; });
    int r, g, b;
    const Bin& top = bins[touched[0]];
    if (top.count * 100 >= px.size() * 15 || touched.size() == 1) {
        r = static_cast<int>(top.r / top.count);
        g = static_cast<int>(top.g / top.count);
        b = static_cast<int>(top.b / top.count);
    } else {
        uint64_t cnt = 0, sr = 0, sg = 0, sb = 0;
        size_t k = std::min<size_t>(3, touched.size());
        for (size_t i = 0; i < k; i++) {
            const Bin& bin = bins[touched[i]];
            cnt += bin.count;
            sr += bin.r;
            sg += bin.g;
            sb += bin.b;
        }
        r = static_cast<int>(sr / cnt);
        g = static_cast<int>(sg / cnt);
        b = static_cast<int>(sb / cnt);
    }
    for (int pass = 0; pass < 2; pass++) {
        uint64_t cnt = 0, sr = 0, sg = 0, sb = 0;
        for (uint32_t v : px) {
            int pb = v & 0xFF, pg = (v >> 8) & 0xFF, pr = (v >> 16) & 0xFF;
            if (std::abs(pr - r) <= 12 && std::abs(pg - g) <= 12 && std::abs(pb - b) <= 12) {
                cnt++;
                sr += pr;
                sg += pg;
                sb += pb;
            }
        }
        if (cnt == 0) {
            break;
        }
        int nr = static_cast<int>((sr + cnt / 2) / cnt), ng = static_cast<int>((sg + cnt / 2) / cnt),
            nb = static_cast<int>((sb + cnt / 2) / cnt);
        if (nr == r && ng == g && nb == b) {
            break;
        }
        r = nr;
        g = ng;
        b = nb;
    }
    return RGB(r, g, b);
}

int ColorDistance(COLORREF a, COLORREF b) {
    return std::max({std::abs(GetRValue(a) - GetRValue(b)), std::abs(GetGValue(a) - GetGValue(b)),
                     std::abs(GetBValue(a) - GetBValue(b))});
}


struct AdaptiveMonitor {
    HMONITOR monitor = nullptr;
    HWND window = nullptr;        // the maximized window sampled
    bool settling = false;        // the row differs from the color in use: fast readings
    ULONGLONG sampledAt = 0;
    bool haveSample = false;
    COLORREF lastSample = 0;
    // What the frame is given.
    bool committedKnown = false;
    COLORREF committed = 0;
};
std::vector<AdaptiveMonitor> g_adaptive;
// The color each window's title bar read as the last time it was the
// maximized window, with whether it was the foreground window then (many
// apps color an inactive title bar differently). It counts as a reading: a
// window seen before commits on its first reading when that agrees with
// what it read before, at the event itself rather than one reading later.
// Entries of windows gone are dropped at the regular tick.
struct TitleColorMemory {
    COLORREF color;
    bool active;
};
std::unordered_map<HWND, TitleColorMemory> g_titleColors;
constexpr UINT_PTR kSampleTimerId = 5;  // the fast readings while a color settles
// The fast readings: every frame. The first reading after a window event
// is taken once the window has zoomed into place (at once with the window
// animations off, or for a window Aero Peek shows, read off its own
// surface); the next confirms it a frame later.
constexpr UINT kSampleFastMs = 16;
// Two consecutive readings agree when they are within this many levels: a
// title bar at rest reads the same to the level, while the tail of a
// window's zoom animation, slow as it is, still moves more than this
// between two frames.
constexpr int kAgreeDistance = 3;
// A reading this far from the color in use is a change.
constexpr int kChangeDistance = 6;
constexpr LONG kSampleRows = 1;
constexpr LONG kSampleSkipRows = 3;  // the first three rows are skipped: the fourth is read
// Rows a title bar can be counted on for (at 100% scaling): the inner shadow
// is skipped only while the row read stays within them.
constexpr int kTitleBarRows = 28;

bool AdaptiveSource() {
    return g_settings.colorSource != 0 && !g_settings.systemLook;
}

COLORREF FrameColorFor(const Overlay& overlay) {
    if (!AdaptiveSource()) {
        return g_settings.customColor;
    }
    for (const AdaptiveMonitor& a : g_adaptive) {
        if (a.monitor == overlay.layout.monitor) {
            return a.committedKnown ? a.committed : g_settings.customColor;
        }
    }
    return g_settings.customColor;
}

AdaptiveMonitor& AdaptiveFor(HMONITOR monitor) {
    for (AdaptiveMonitor& a : g_adaptive) {
        if (a.monitor == monitor) {
            return a;
        }
    }
    g_adaptive.push_back(AdaptiveMonitor{monitor});
    return g_adaptive.back();
}

// The topmost maximized window on a monitor (not minimized, not cloaked).
HWND MaximizedWindowOn(HMONITOR monitor) {
    for (HWND hwnd = GetTopWindow(nullptr); hwnd; hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || !IsZoomed(hwnd) ||
            IsCloakedWindow(hwnd)) {
            continue;
        }
        if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != monitor) {
            continue;
        }
        return hwnd;
    }
    return nullptr;
}

// The row sampled: the fourth row of the window that shows - the window
// clipped to the work area (the maximized frame extends under it by the
// invisible border width) and to the rounded screen, since the frame paints
// over whatever lies outside it and would be read as the title bar's color
// (its own, when a window reaches under it) - three rows in, past any line
// an app paints along its top, kept off the taskbar window where the two
// overlap. The inner shadow darkens the rows it falls on, so where the
// title bar is tall enough the row is taken past it.
RECT TitleBarStrip(HWND window, const RECT& work, const Layout& layout) {
    RECT frame{}, wr{}, shown{};
    if (!GetWindowRect(window, &frame) || !IntersectRect(&wr, &frame, &work) ||
        !IntersectRect(&shown, &wr, &layout.innerRect)) {
        return RECT{};
    }
    LONG top = shown.top;
    RECT tb{};
    if (layout.taskbar && GetWindowRect(layout.taskbar, &tb) && top < tb.bottom &&
        top + kSampleRows > tb.top && shown.left < tb.right && shown.right > tb.left) {
        top = tb.bottom;
    }
    LONG pastShadow = layout.innerRect.top + layout.shadowBlur;
    LONG titleBottom = wr.top + ScaleForDpi(kTitleBarRows, layout.dpi);
    if (pastShadow > top && pastShadow + kSampleSkipRows + kSampleRows <= titleBottom) {
        top = pastShadow;
    }
    top += kSampleSkipRows;
    RECT strip{shown.left, top, shown.right, top + kSampleRows};
    RECT clipped{};
    if (!IntersectRect(&clipped, &strip, &shown)) {
        return RECT{};
    }
    return clipped;
}

// The columns of the strip covered by windows above the target in the
// z-order (a dialog by the title bar, a flyout), to be left out of the
// color. Click-through and fully
// transparent windows, cloaked ones, the mod's own and the shell's chrome
// cover nothing - the shell's chrome being its monitor-sized hosts, which
// draw a small part of their rectangle (light-dismiss catchers, flyout
// hosts): a shell window the size of what it shows (a context menu, a
// tray flyout) covers what it covers. A window covering the whole strip is
// taken as a transient (the mask is dropped).
bool CoversNothing(HWND hwnd, const RECT& windowRect, const RECT& monitorRect) {
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TRANSPARENT) {
        return true;
    }
    if (ex & WS_EX_LAYERED) {
        BYTE alpha = 255;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags) && (flags & LWA_ALPHA) &&
            alpha == 0) {
            return true;
        }
    }
    if (!IsWindowVisible(hwnd) || IsCloakedWindow(hwnd)) {
        return true;
    }
    WCHAR cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return true;
    }
    if (wcscmp(cls, kFrameClassName) == 0 || wcscmp(cls, kRefreshClassName) == 0 ||
        wcscmp(cls, kAppBarClassName) == 0) {
        return true;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || wcscmp(cls, L"CabinetWClass") == 0) {
        return false;  // another app's window, or a File Explorer window
    }
    bool monitorSized = windowRect.left <= monitorRect.left && windowRect.top <= monitorRect.top &&
                        windowRect.right >= monitorRect.right &&
                        windowRect.bottom >= monitorRect.bottom;
    return monitorSized;
}

std::vector<uint8_t> OcclusionMask(HWND target, const RECT& strip, const RECT& monitorRect) {
    int w = static_cast<int>(strip.right - strip.left);
    std::vector<uint8_t> mask(w, 0);
    int covered = 0;
    for (HWND hwnd = GetTopWindow(nullptr); hwnd && hwnd != target;
         hwnd = GetWindow(hwnd, GW_HWNDNEXT)) {
        RECT r{};
        if (!GetWindowRect(hwnd, &r) || r.bottom <= strip.top || r.top >= strip.bottom ||
            r.right <= strip.left || r.left >= strip.right) {
            continue;
        }
        if (CoversNothing(hwnd, r, monitorRect)) {
            continue;
        }
        int x0 = static_cast<int>(std::max(r.left, strip.left) - strip.left);
        int x1 = static_cast<int>(std::min(r.right, strip.right) - strip.left);
        for (int x = x0; x < x1; x++) {
            if (!mask[x]) {
                covered++;
            }
            mask[x] = 1;
        }
    }
    if (covered >= w) {
        std::fill(mask.begin(), mask.end(), 0);
    }
    return mask;
}

// The strip's pixels off the screen, as 0x00RRGGBB.
bool CaptureStrip(const RECT& strip, std::vector<uint32_t>& px) {
    int w = static_cast<int>(strip.right - strip.left), h = static_cast<int>(strip.bottom - strip.top);
    if (w <= 0 || h <= 0 || static_cast<long long>(w) * h > 8192LL * 64) {
        return false;
    }
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return false;
    }
    bool ok = false;
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HDC mem = CreateCompatibleDC(screen);
    if (dib && bits && mem) {
        HGDIOBJ old = SelectObject(mem, dib);
        ok = BitBlt(mem, 0, 0, w, h, screen, strip.left, strip.top, SRCCOPY) != FALSE;
        if (ok) {
            GdiFlush();
            auto* p = static_cast<const uint32_t*>(bits);
            px.assign(p, p + static_cast<size_t>(w) * h);
        }
        SelectObject(mem, old);
    }
    if (mem) {
        DeleteDC(mem);
    }
    if (dib) {
        DeleteObject(dib);
    }
    ReleaseDC(nullptr, screen);
    return ok;
}

// Whether a taskbar thumbnail preview is showing: on these builds it is a
// visible, monitor-sized XamlExplorerHostIslandWindow without a title (Task
// View's carries one), drawing the flyout in a small part of its rectangle
// - which the mask can't tell from the rest, so no row is read while one
// shows (the same window hosts snap assist, untitled too, which also hangs
// over title bars); on older builds, a TaskListThumbnailWnd.
bool ThumbnailPreviewShowing() {
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, L"XamlExplorerHostIslandWindow", nullptr);
         hwnd; hwnd = FindWindowExW(nullptr, hwnd, L"XamlExplorerHostIslandWindow", nullptr)) {
        if (!IsWindowVisible(hwnd) || IsCloakedWindow(hwnd) || !SpansItsMonitor(hwnd)) {
            continue;
        }
        WCHAR title[64];
        if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) <= 0) {
            return true;
        }
    }
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, L"TaskListThumbnailWnd", nullptr); hwnd;
         hwnd = FindWindowExW(nullptr, hwnd, L"TaskListThumbnailWnd", nullptr)) {
        if (IsWindowVisible(hwnd) && !IsCloakedWindow(hwnd)) {
            return true;
        }
    }
    return false;
}

// One reading of a monitor's title-bar row; true when the row could be
// read. Sets `color` to its dominant color.
// The row read off the window's own surface rather than off the screen -
// only for the window Aero Peek shows from a thumbnail, whose title bar the
// preview flyout hangs over. PrintWindow with PW_RENDERFULLCONTENT has the
// app render the window into the DC (some apps' 3D views, Revit's for one,
// flicker when that happens, which is why it isn't the regular way),
// clipped to the bitmap: the window's width by a dozen rows or so.
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif
// `frame` is where the window's surface lies, in the coordinates of `strip`
// (its window rectangle unless given: a minimized window's is a placeholder).
// PrintWindow is answered by the window's own thread, so it is only asked of
// a window whose thread has just answered a message within a few
// milliseconds; a busy one is read off the screen instead (the caller's
// fallback), and the mod's thread never waits on it for more than that.
constexpr UINT kResponsiveProbeMs = 15;
bool WindowResponsive(HWND window) {
    DWORD_PTR result = 0;
    // SMTO_BLOCK: nothing else is dispatched to this thread while it waits.
    return SendMessageTimeoutW(window, WM_NULL, 0, 0,
                               SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT,
                               kResponsiveProbeMs, &result) != 0;
}
bool CaptureWindowRow(HWND window, const RECT& strip, std::vector<uint32_t>& px,
                      const RECT* frame = nullptr) {
    if (!WindowResponsive(window)) {
        return false;
    }
    RECT wr{};
    if (frame) {
        wr = *frame;
    } else if (!GetWindowRect(window, &wr)) {
        return false;
    }
    int w = static_cast<int>(wr.right - wr.left);
    int rows = static_cast<int>(strip.bottom - wr.top);  // the window's top down to the row
    int left = static_cast<int>(strip.left - wr.left), right = static_cast<int>(strip.right - wr.left);
    int top = static_cast<int>(strip.top - wr.top);
    if (w <= 0 || rows <= 0 || rows > 256 || left < 0 || right > w || left >= right || top < 0 ||
        static_cast<long long>(w) * rows > 8192LL * 256) {
        return false;
    }
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return false;
    }
    bool ok = false;
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -rows;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HDC mem = CreateCompatibleDC(screen);
    if (dib && bits && mem) {
        HGDIOBJ old = SelectObject(mem, dib);
        ok = PrintWindow(window, mem, PW_RENDERFULLCONTENT) != FALSE;
        if (ok) {
            GdiFlush();
            auto* p = static_cast<const uint32_t*>(bits);
            px.clear();
            for (int y = top; y < rows; y++) {
                px.insert(px.end(), p + static_cast<size_t>(y) * w + left,
                          p + static_cast<size_t>(y) * w + right);
            }
            ok = !px.empty();
        }
        SelectObject(mem, old);
    }
    if (mem) {
        DeleteDC(mem);
    }
    if (dib) {
        DeleteObject(dib);
    }
    ReleaseDC(nullptr, screen);
    return ok;
}

// Aero Peek from a taskbar thumbnail: the taskbar asks DWM to show the
// hovered window alone (DwmpActivateLivePreview, dwmapi.dll ordinal 113,
// called from the taskbar's own XAML in this process), and the frame takes
// that window's color while it shows, as if it had been brought to the
// front. The hook keeps the window peeked, if any; the color code picks it
// over the monitor's maximized window when it is a maximized one.
std::atomic<HWND> g_peekWindow{nullptr};
using DwmpActivateLivePreview_t = HRESULT(WINAPI*)(BOOL activate, HWND exclude, HWND insertBefore,
                                                   UINT trigger, void* finalRect);
DwmpActivateLivePreview_t DwmpActivateLivePreview_Original;
HRESULT WINAPI DwmpActivateLivePreview_Hook(BOOL activate, HWND exclude, HWND insertBefore,
                                            UINT trigger, void* finalRect) {
    g_peekWindow = activate ? exclude : nullptr;
    g_titleEventPending = true;  // no zoom to wait for: the peeked window is read from its surface
    if (HWND ctrl = g_ctrlWnd.load()) {
        PostMessageW(ctrl, kMsgTaskbarEvent, 0, 0);  // a tick, fast readings
    }
    return DwmpActivateLivePreview_Original(activate, exclude, insertBefore, trigger, finalRect);
}
void HookLivePreview() {
    // Already loaded: the mod links dwmapi, and so does Explorer.
    HMODULE dwmapi = GetModuleHandleW(L"dwmapi.dll");
    FARPROC proc = dwmapi ? GetProcAddress(dwmapi, MAKEINTRESOURCEA(113)) : nullptr;
    if (!proc) {
        Wh_Log(L"DwmpActivateLivePreview not found: no color while peeking");
        return;
    }
    WindhawkUtils::SetFunctionHook(reinterpret_cast<DwmpActivateLivePreview_t>(proc),
                                   DwmpActivateLivePreview_Hook, &DwmpActivateLivePreview_Original);
}

// Whether a window is maximized, or minimized from maximized (a thumbnail
// peeks at those too; they come back maximized).
bool MaximizedOrWasMaximized(HWND hwnd) {
    if (IsZoomed(hwnd)) {
        return true;
    }
    WINDOWPLACEMENT wp{sizeof(wp)};
    return IsIconic(hwnd) && GetWindowPlacement(hwnd, &wp) && (wp.flags & WPF_RESTORETOMAXIMIZED);
}

// The window peeked from a thumbnail, for this monitor: a maximized window
// on it, or one minimized from maximized that would come back to it.
HWND PeekedWindowOn(HMONITOR monitor) {
    HWND peek = g_peekWindow.load();
    if (!peek || !IsWindow(peek) || !MaximizedOrWasMaximized(peek)) {
        return nullptr;
    }
    HMONITOR on = nullptr;
    if (IsIconic(peek)) {
        WINDOWPLACEMENT wp{sizeof(wp)};
        if (GetWindowPlacement(peek, &wp)) {
            on = MonitorFromRect(&wp.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
        }
    } else {
        on = MonitorFromWindow(peek, MONITOR_DEFAULTTONEAREST);
    }
    return on == monitor ? peek : nullptr;
}

// How long a window takes to zoom into place (maximize, restore) or out of
// it (minimize) with the window animations on.
constexpr ULONGLONG kZoomSettleMs = 300;
bool WindowAnimationsOn() {
    BOOL on = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &on, 0);
    return on != FALSE;
}

bool ReadTitleBar(const Overlay& overlay, HWND window, COLORREF& color) {
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(overlay.layout.monitor, &mi)) {
        return false;
    }
    // A minimized window (peeked from its thumbnail) sits at -32000 with a
    // placeholder rectangle, while its surface keeps the maximized size it
    // comes back to - the work area plus the invisible border on each side.
    // The row is taken from that surface at the window's own offsets - past
    // the border, then the skipped rows - and only the pixels the surface
    // filled count.
    if (IsIconic(window)) {
        UINT dpi = GetWindowDpi(window);
        int border = GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi) +
                     GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
        LONG width = (mi.rcWork.right - mi.rcWork.left) + 2 * border;
        RECT frame{0, 0, width, border + kSampleSkipRows + kSampleRows};
        RECT strip{border, border + kSampleSkipRows, width - border,
                   border + kSampleSkipRows + kSampleRows};
        std::vector<uint32_t> px;
        if (strip.right <= strip.left || !CaptureWindowRow(window, strip, px, &frame)) {
            return false;
        }
        px.erase(std::remove_if(px.begin(), px.end(), [](uint32_t p) { return (p >> 24) == 0; }),
                 px.end());
        if (px.empty()) {
            return false;
        }
        color = DominantColor(px);
        return true;
    }
    RECT strip = TitleBarStrip(window, mi.rcWork, overlay.layout);
    if (IsRectEmpty(&strip)) {
        return false;
    }
    std::vector<uint32_t> px;
    // The window Aero Peek shows from a thumbnail: the preview flyout hangs
    // over the screen, so its surface is copied - once or twice, while the
    // peek lasts (PrintWindow has the app render, see CaptureWindowRow).
    if (window == g_peekWindow.load() && CaptureWindowRow(window, strip, px)) {
        color = DominantColor(px);
        return true;
    }
    // Otherwise the row as on screen, the way Top Taskbar Bleed reads it -
    // the app is never asked to render (some, like Revit, flicker when they
    // are) - with whatever covers it masked out; not while a thumbnail
    // preview hangs over it, nor while a window is still zooming into place
    // or out of it (the row then shows what is behind it).
    if (ThumbnailPreviewShowing()) {
        return false;
    }
    if (GetTickCount64() - g_titleEventTick.load() < kZoomSettleMs && WindowAnimationsOn()) {
        return false;
    }
    if (!CaptureStrip(strip, px) || px.empty()) {
        return false;
    }
    std::vector<uint8_t> mask = OcclusionMask(window, strip, mi.rcMonitor);
    int w = static_cast<int>(strip.right - strip.left);
    std::vector<uint32_t> visible;
    visible.reserve(px.size());
    for (size_t i = 0; i < px.size(); i++) {
        if (!mask[i % w]) {
            visible.push_back(px[i]);
        }
    }
    color = DominantColor(visible.empty() ? px : visible);
    return true;
}

// Every tick, and every frame while a monitor's color settles: the
// maximized (or peeked) window's title-bar row. A reading that differs from the color
// in use starts the fast readings; two consecutive readings that agree
// commit (the frame eases to the color, see AnimateFrameColor).
void UpdateAdaptiveColors(ULONGLONG now, bool fast) {
    if (!AdaptiveSource()) {
        g_adaptive.clear();
        g_titleColors.clear();
        return;
    }
    bool anySettling = false;
    for (Overlay& overlay : *g_overlays) {
        AdaptiveMonitor& a = AdaptiveFor(overlay.layout.monitor);
        HWND window = nullptr;
        if (overlay.shown && !overlay.fullscreen) {
            window = PeekedWindowOn(a.monitor);
            if (!window) {
                window = MaximizedWindowOn(a.monitor);
            }
        }
        if (window != a.window) {
            a.window = window;
            a.haveSample = false;
            a.settling = window != nullptr;
            // A window seen before: the color it read as the last time, at
            // once - before it has even finished zooming in - and the
            // readings that follow confirm it or correct it.
            auto remembered = window ? g_titleColors.find(window) : g_titleColors.end();
            if (remembered != g_titleColors.end()) {
                HWND foreground = GetForegroundWindow();
                bool active = foreground && (foreground == window ||
                                             GetAncestor(foreground, GA_ROOTOWNER) == window);
                if (remembered->second.active == active &&
                    (!a.committedKnown ||
                     ColorDistance(remembered->second.color, a.committed) > kChangeDistance)) {
                    a.committed = remembered->second.color;
                    a.committedKnown = true;
                    Wh_Log(L"monitor %p: frame color #%02X%02X%02X from the title bar "
                           L"(as last read)",
                           a.monitor, GetRValue(a.committed), GetGValue(a.committed),
                           GetBValue(a.committed));
                }
            }
        }
        if (!window) {
            // No maximized window: the frame color, at once.
            if (a.committedKnown) {
                a.committedKnown = false;
                Wh_Log(L"monitor %p: frame color back to the frame color (no maximized window)",
                       a.monitor);
            }
            continue;
        }
        if (fast && !a.settling) {
            continue;  // the fast readings are for monitors settling
        }
        a.sampledAt = now;
        COLORREF color = 0;
        if (!ReadTitleBar(overlay, window, color)) {
            anySettling = anySettling || a.settling;  // read again once it can be
            continue;
        }
        HWND foreground = GetForegroundWindow();
        bool active = foreground && (foreground == window ||
                                     GetAncestor(foreground, GA_ROOTOWNER) == window);
        auto remembered = g_titleColors.find(window);
        bool agrees = (a.haveSample && ColorDistance(color, a.lastSample) <= kAgreeDistance) ||
                      (remembered != g_titleColors.end() && remembered->second.active == active &&
                       ColorDistance(color, remembered->second.color) <= kAgreeDistance);
        a.lastSample = color;
        a.haveSample = true;
        bool differs = !a.committedKnown || ColorDistance(color, a.committed) > kChangeDistance;
        if (!differs) {
            a.settling = false;
            g_titleColors[window] = {color, active};
            continue;
        }
        if (agrees) {
            a.committed = color;
            a.committedKnown = true;
            a.settling = false;
            g_titleColors[window] = {color, active};
            Wh_Log(L"monitor %p: frame color #%02X%02X%02X from the title bar", a.monitor,
                   GetRValue(color), GetGValue(color), GetBValue(color));
        } else {
            a.settling = true;  // read again soon
        }
        anySettling = anySettling || a.settling;
    }
    if (!fast) {
        for (auto it = g_titleColors.begin(); it != g_titleColors.end();) {
            it = IsWindow(it->first) ? std::next(it) : g_titleColors.erase(it);
        }
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        if (anySettling) {
            SetTimer(ctrl, kSampleTimerId, kSampleFastMs, nullptr);
        } else {
            KillTimer(ctrl, kSampleTimerId);
        }
    }
}

COLORREF LerpColor(COLORREF from, COLORREF to, float t) {
    auto mix = [t](int a, int b) {
        return static_cast<BYTE>(std::clamp(static_cast<int>(std::lround(a + (b - a) * t)), 0, 255));
    };
    return RGB(mix(GetRValue(from), GetRValue(to)), mix(GetGValue(from), GetGValue(to)),
               mix(GetBValue(from), GetBValue(to)));
}

// Eases a painted frame from its shown color to `to` over kColorAnimMs.
// Acrylic frame: the color brush's color (the solid fill) or the effect
// graph's flood colors (the backdrop fills, one animation per named
// property) are animated by the compositor, so nothing is rebuilt. Layered
// frame: repainted at each step of the timer below (a recolor is a lookup
// over the cached shape, no re-rasterization). The taskbar's background
// follows the shown color at each step where it is painted.
void AnimateFrameColor(Overlay& overlay, COLORREF to, ULONGLONG now) {
    overlay.animFrom = overlay.displayColor;
    overlay.animTo = to;
    overlay.animStart = now;
    overlay.colorTarget = to;
    overlay.colorAnimating = true;
    if (overlay.comp && !overlay.comp->gradientStops.empty()) {
        // A gradient (the solid fill's, or the acrylic bands' inputs): each
        // stop eases to its own color (the frame color again) or to the one
        // color (a title bar's), by the compositor.
        try {
            auto easing = g_compositor.CreateCubicBezierEasingFunction({0.2f, 0.0f}, {0.0f, 1.0f});
            for (const auto& entry : overlay.comp->gradientStops) {
                COLORREF c = StopColor(entry.index, to);
                auto animation = g_compositor.CreateColorKeyFrameAnimation();
                animation.Duration(std::chrono::milliseconds(kColorAnimMs));
                animation.InterpolationColorSpace(wuc::CompositionColorSpace::Rgb);
                animation.InsertKeyFrame(
                    1.0f, wu::Color{entry.alpha, GetRValue(c), GetGValue(c), GetBValue(c)}, easing);
                entry.stop.StartAnimation(L"Color", animation);
            }
            overlay.paintedColor = to;
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"color animation threw %08X: painted instead", e.code().value);
            PaintOverlay(overlay, to);
        }
    } else if (overlay.comp && overlay.comp->colorBrush) {
        // The color fill: the brush's color, animated by the compositor.
        try {
            auto easing = g_compositor.CreateCubicBezierEasingFunction({0.2f, 0.0f}, {0.0f, 1.0f});
            auto animation = g_compositor.CreateColorKeyFrameAnimation();
            animation.Duration(std::chrono::milliseconds(kColorAnimMs));
            animation.InterpolationColorSpace(wuc::CompositionColorSpace::Rgb);
            animation.InsertKeyFrame(1.0f, FillColor(to), easing);
            overlay.comp->colorBrush.StartAnimation(L"Color", animation);
            overlay.paintedColor = to;
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"color animation threw %08X: painted instead", e.code().value);
            PaintOverlay(overlay, to);
        }
    } else if (overlay.comp && (!overlay.comp->effectBrush || overlay.comp->colorProperties.empty())) {
        // The color isn't in an animatable graph (blur only, with the tint
        // as color sprites; or no effects at all): painted as it is.
        PaintOverlay(overlay, to);
    } else if (overlay.comp) {
        try {
            auto easing = g_compositor.CreateCubicBezierEasingFunction({0.2f, 0.0f}, {0.0f, 1.0f});
            for (const winrt::hstring& property : overlay.comp->colorProperties) {
                bool luminosity = property == L"LuminosityColor.Color";
                int opacity = luminosity ? g_settings.luminosityOpacity : g_settings.tintOpacity;
                wu::Color color{static_cast<uint8_t>(opacity * 255 / 100), GetRValue(to),
                                GetGValue(to), GetBValue(to)};
                auto animation = g_compositor.CreateColorKeyFrameAnimation();
                animation.Duration(std::chrono::milliseconds(kColorAnimMs));
                animation.InterpolationColorSpace(wuc::CompositionColorSpace::Rgb);
                animation.InsertKeyFrame(1.0f, color, easing);
                overlay.comp->effectBrush.StartAnimation(property, animation);
            }
            // The graph now heads for `to`: a rebuild meanwhile paints it.
            overlay.paintedColor = to;
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"color animation threw %08X: painted instead", e.code().value);
            PaintOverlay(overlay, to);
        }
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        SetTimer(ctrl, kColorTimerId, kColorStepMs, nullptr);
    }
}

// A window event that can change a title bar (an activation, a maximize or
// restore, a minimize starting or ending, a move ending, Aero Peek - see
// g_titleEventPending): every monitor's row is read at the fast pace until
// it agrees with the color in use, so the window behind a minimized one is picked up as the
// minimize animation ends rather than at the next tick.
void BeginFastReadings() {
    if (!AdaptiveSource()) {
        return;
    }
    for (AdaptiveMonitor& a : g_adaptive) {
        if (a.window) {
            a.settling = true;
        }
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        SetTimer(ctrl, kSampleTimerId, kSampleFastMs, nullptr);
    }
}

// The fast readings while a color settles; a committed color is applied
// the way Tick applies one.
void FollowFrameColors(ULONGLONG now);
void OnSampleTimer() {
    if (g_inTick || g_unloading) {
        return;
    }
    ULONGLONG now = GetTickCount64();
    UpdateAdaptiveColors(now, true);
    FollowFrameColors(now);
}

void OnColorTimer(HWND ctrl) {
    ULONGLONG now = GetTickCount64();
    bool any = false;
    for (Overlay& overlay : *g_overlays) {
        if (!overlay.colorAnimating) {
            continue;
        }
        float t = std::min(1.0f, static_cast<float>(now - overlay.animStart) / kColorAnimMs);
        float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
        COLORREF shown = t >= 1.0f ? overlay.animTo : LerpColor(overlay.animFrom, overlay.animTo, eased);
        if (overlay.comp) {
            overlay.displayColor = shown;  // the compositor paints; the taskbar follows
        } else if (overlay.painted) {
            PaintOverlay(overlay, shown);
        }
        if (t >= 1.0f) {
            overlay.colorAnimating = false;
            overlay.colorTarget = overlay.animTo;
        } else {
            any = true;
        }
    }
    ApplyFills(now);
    if (!any) {
        KillTimer(ctrl, kColorTimerId);
    }
}

// Perceived brightness above which a color counts as light (black text
// starts out-contrasting white around here), with a hysteresis band so a
// color at the threshold can't flap the taskbar's theme.
constexpr int kLightThreshold = 120;
constexpr int kThemeHysteresis = 8;
int PerceivedBrightness(COLORREF c) {
    return static_cast<int>(
        std::lround(0.299 * GetRValue(c) + 0.587 * GetGValue(c) + 0.114 * GetBValue(c)));
}

// The taskbar's theme follows the title-bar color (the frame color: its own).
void ApplyThemes(ULONGLONG now) {
    if (!g_taskbarXamlAvailable) {
        return;
    }
    for (Overlay& overlay : *g_overlays) {
        HWND taskbar = overlay.layout.taskbar;
        if (!taskbar) {
            continue;
        }
        int want = kThemeNone;
        if (AdaptiveSource()) {
            for (const AdaptiveMonitor& a : g_adaptive) {
                if (a.monitor != overlay.layout.monitor || !a.committedKnown) {
                    continue;
                }
                int lum = PerceivedBrightness(a.committed);
                bool light = overlay.themeWanted == kThemeLight ? lum >= kLightThreshold - kThemeHysteresis
                             : overlay.themeWanted == kThemeDark ? lum > kLightThreshold + kThemeHysteresis
                                                                  : lum > kLightThreshold;
                want = light ? kThemeLight : kThemeDark;
            }
        }
        if (overlay.themeAsked && want == overlay.themeWanted) {
            continue;
        }
        overlay.themeAsked = true;
        overlay.themeWanted = want;
        RequestPass(kPassTheme, taskbar, now, true, [taskbar, want] {
            TaskbarWants& wants = (*g_wants)[taskbar];
            wants.theme = want;
            ApplyThemeOnTaskbarThread(taskbar, want);
            return true;
        });
    }
}

// A changed color: painted right away when it is the frame color
// (settings), eased over when it is a title bar's.
void FollowFrameColors(ULONGLONG now) {
    bool adaptive = AdaptiveSource();
    for (Overlay& overlay : *g_overlays) {
        COLORREF color = FrameColorFor(overlay);
        if (!overlay.painted) {
            PaintOverlay(overlay, color);
        } else if (color != overlay.colorTarget) {
            if (adaptive) {
                AnimateFrameColor(overlay, color, now);
            } else {
                overlay.colorAnimating = false;
                PaintOverlay(overlay, color);
            }
        }
    }
    ApplyThemes(now);
}

void Tick(bool forceRebuild) {
    if (g_inTick || g_unloading) {
        return;
    }
    g_inTick = true;
    ULONGLONG tStart = GetTickCount64();
    // Diagnostics: a tick that comes much later than scheduled means this
    // thread was held up somewhere (message loop starved or a call blocked).
    if (g_lastTickTime && tStart - g_lastTickTime > 1000) {
        Wh_Log(L"tick gap: %llu ms since the previous tick", tStart - g_lastTickTime);
    }
    g_lastTickTime = tStart;

    if ((g_colorDeps.mode || g_settings.systemLook) &&
        SystemUsesLightTheme() != g_colorLoadedLight) {
        Wh_Log(L"Windows mode changed: the frame follows");
        g_reloadRequested = true;
    } else if (g_colorDeps.accent && CurrentAccentColor() != g_colorLoadedAccent) {
        Wh_Log(L"accent color changed: the frame follows");
        g_reloadRequested = true;
    }
    if (g_reloadRequested.exchange(false)) {
        LoadSettings();
        forceRebuild = true;
    }

    SYSTEM_POWER_STATUS power{};
    if (GetSystemPowerStatus(&power)) {
        // SystemStatusFlag (1: battery saver on) - the fourth byte, by
        // offset, as older headers call it Reserved1.
        bool saver = reinterpret_cast<const BYTE*>(&power)[3] == 1;
        if (g_batterySaver.exchange(saver) != saver) {
            Wh_Log(L"battery saver %s", saver ? (g_settings.batterySaverSolid ? L"on: solid fill" : L"on")
                                              : L"off");
        }
    }

    // Composition coming or going (turning out unavailable) means different
    // windows: every frame is rebuilt from scratch. A change of fill
    // (settings, transparency effects, battery saver) is a new content for
    // the same windows: painted again below.
    bool composition = UsesComposition();
    if (composition != g_overlaysComposition) {
        Wh_Log(L"frames: %s", composition ? L"composition" : L"layered");
        DestroyOverlays(*g_overlays);
        g_overlaysComposition = composition;
        forceRebuild = true;
    }
    bool backdrop = UsesBackdrop();
    if (backdrop != g_overlaysBackdrop) {
        Wh_Log(L"fill: %s", backdrop ? L"backdrop" : L"color");
        g_overlaysBackdrop = backdrop;
        for (Overlay& overlay : *g_overlays) {
            if (overlay.comp) {
                overlay.painted = false;  // the content is built again by the paint below
            }
        }
    }
    if (g_surfacesLost.exchange(false)) {
        for (Overlay& overlay : *g_overlays) {
            if (overlay.comp) {
                overlay.comp->built = false;  // drawn again by the paint below
                overlay.painted = false;
            }
        }
    }
    CheckEffectGraphs();

    std::vector<Layout> layouts;
    std::wstring signature = ComputeLayouts(layouts);
    if (forceRebuild || signature != g_layoutSignature ||
        layouts.size() != g_overlays->size()) {
        if (forceRebuild) {
            // Settings changed: every frame is rebuilt from scratch.
            for (Overlay& overlay : *g_overlays) {
                overlay.layout.signature.clear();
            }
        }
        SyncOverlays(layouts);
        g_layoutSignature = signature;
        ResetPassStates();
    }

    ULONGLONG now = GetTickCount64();
    MaintainVeils(now);
    PublishAccentColor(now);
    UpdateAdaptiveColors(now, false);

    // Work area: the sooner the shell shrinks it after a taskbar change, the
    // sooner maximized windows stop spanning the monitor. The appbar thread
    // does the (blocking) shell calls.
    PublishAppBarWishes();

    // Taskbar-side tweaks: each is requested on the taskbar thread and
    // confirmed asynchronously, re-sent while a taskbar's tree isn't ready
    // and re-asserted every couple of seconds (the taskbar may rebuild its
    // tree or another mod may reset a value). Nothing here waits.
    HideStrokes(now);

    // The taskbar's bezel lines, and its two monitor corners when they are
    // on (the frame paints its own either way).
    ApplyXamlCorners(now);

    ApplyFills(now);

    UpdateVisibility();

    // Tick fast while a frame waits for its taskbar (it must appear within a
    // frame of the taskbar being uncloaked), at the regular rate otherwise.
    bool anyWaiting = false;
    for (const Overlay& overlay : *g_overlays) {
        // A frame hidden for full-screen or Task View can't show until that
        // ends, and the event that ends it ticks right away - no need to
        // spin at 60 Hz for as long as a game runs.
        anyWaiting = anyWaiting ||
                     (overlay.waiting && !overlay.fullscreen && !overlay.taskViewHidden);
    }
    if (anyWaiting != g_fastTick) {
        if (HWND ctrl = g_ctrlWnd.load()) {
            SetTimer(ctrl, kTimerId, anyWaiting ? kWaitTickIntervalMs : kTickIntervalMs, nullptr);
        }
        g_fastTick = anyWaiting;
    }

    FollowFrameColors(now);

    if (backdrop) {
        RefreshBackdrops(false);  // the fallback for changes no event announced
    }

    ULONGLONG tEnd = GetTickCount64();
    if (tEnd - tStart > 500) {
        Wh_Log(L"slow tick: %llu ms", tEnd - tStart);
    }
    g_inTick = false;
}

// ---------------------------------------------------------------------------
// Windows and thread
// ---------------------------------------------------------------------------

LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            // Solid: the painted pixels are "client area" - the mouse stops
            // here (arrow cursor, clicks swallowed). Zero-alpha pixels never
            // reach this handler; the system passes them through.
            return HTCLIENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_XBUTTONDBLCLK:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_CONTEXTMENU:
            return 0;  // swallowed
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool IsTaskbarWindow(HWND hwnd) {
    WCHAR cls[32];
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
           (wcscmp(cls, L"Shell_TrayWnd") == 0 || wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0);
}

// Delivered on the mod thread (out-of-context hook). A taskbar or Task View
// window that shows, hides or moves triggers a tick right away, so the change
// is picked up when it happens instead of at the next timer tick.
void CALLBACK TaskbarWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                  LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) {
        return;
    }
    if (event != EVENT_OBJECT_SHOW && event != EVENT_OBJECT_HIDE &&
        event != EVENT_OBJECT_LOCATIONCHANGE) {
        return;
    }
    bool taskbar = IsTaskbarWindow(hwnd);
    if (!taskbar && !IsTaskViewWindowClass(hwnd)) {
        return;
    }
    if (taskbar) {
        // Auto-hide may have been toggled, the taskbar may have moved: the
        // appbar thread re-reads the shell state - at most twice a second,
        // since an auto-hide slide reports a location change every frame.
        static ULONGLONG lastRefreshAsk = 0;
        ULONGLONG now = GetTickCount64();
        if (now - lastRefreshAsk >= 500) {
            lastRefreshAsk = now;
            g_shellStateDirty = true;
            if (g_appBarWake) {
                SetEvent(g_appBarWake);
            }
        }
    }
    if (g_taskbarEventPending) {
        return;
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        if (PostMessageW(ctrl, kMsgTaskbarEvent, 0, 0)) {
            g_taskbarEventPending = true;
        }
    }
}

void HookForegroundLocation(HWND foreground);

// A foreground change anywhere, or the foreground window moving or resizing
// (the moment it can go full-screen - F11, a game switching modes), triggers
// a tick right away so the frame hides in the same instant.
void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                     LONG idChild, DWORD, DWORD) {
    // The foreground window's maximized state as last seen, so a location
    // change that maximizes or restores it is told from one that moves it.
    static HWND lastWindow = nullptr;
    static bool lastZoomed = false;
    if (event == EVENT_SYSTEM_FOREGROUND) {
        HookForegroundLocation(hwnd);
        lastWindow = hwnd;
        lastZoomed = hwnd && IsZoomed(hwnd);
        g_titleEventPending = true;  // an activation alone has no zoom to wait for
    } else if (event == EVENT_SYSTEM_MINIMIZESTART || event == EVENT_SYSTEM_MINIMIZEEND ||
               event == EVENT_SYSTEM_MOVESIZEEND) {
        // A window leaving or arriving under the frames: the tick (and the
        // backdrop refresh it starts) is wanted whichever window it is.
        g_titleEventPending = true;
        if (event != EVENT_SYSTEM_MOVESIZEEND) {
            g_titleEventTick = GetTickCount64();  // the minimize / restore zoom
        }
    } else if (event != EVENT_OBJECT_LOCATIONCHANGE || idObject != OBJID_WINDOW ||
               idChild != CHILDID_SELF || !hwnd || hwnd != GetForegroundWindow()) {
        return;  // the other system events in the hooked range, or not the foreground window
    } else {
        bool zoomed = IsZoomed(hwnd) != FALSE;
        if (hwnd != lastWindow || zoomed != lastZoomed) {
            g_titleEventPending = true;  // maximized or restored
            g_titleEventTick = GetTickCount64();
        }
        lastWindow = hwnd;
        lastZoomed = zoomed;
    }
    if (g_taskbarEventPending) {
        return;
    }
    if (HWND ctrl = g_ctrlWnd.load()) {
        if (PostMessageW(ctrl, kMsgTaskbarEvent, 0, 0)) {
            g_taskbarEventPending = true;
        }
    }
}

// The location hook follows the foreground window's process: a hook for all
// processes would queue every location change in the session (cursor and
// caret moves included) to this thread. Re-registered on each foreground
// change; null unhooks.
HWINEVENTHOOK g_locationHook = nullptr;
DWORD g_locationHookPid = 0;

void HookForegroundLocation(HWND foreground) {
    DWORD pid = 0;
    if (foreground) {
        GetWindowThreadProcessId(foreground, &pid);
    }
    if (pid == g_locationHookPid && (g_locationHook || !pid)) {
        return;
    }
    if (g_locationHook) {
        UnhookWinEvent(g_locationHook);
        g_locationHook = nullptr;
    }
    g_locationHookPid = pid;
    if (!pid) {
        return;
    }
    g_locationHook =
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                        ForegroundWinEventProc, pid, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_locationHook) {
        Wh_Log(L"SetWinEventHook (location, pid %u) failed: %u", pid, GetLastError());
    }
}

LRESULT CALLBACK CtrlWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kTimerId) {
                Tick(false);
            } else if (wParam == kCoalesceTimerId) {
                KillTimer(hwnd, kCoalesceTimerId);
                Tick(false);
            } else if (wParam == kRefreshTimerId) {
                OnRefreshTimer(hwnd);
            } else if (wParam == kColorTimerId) {
                OnColorTimer(hwnd);
            } else if (wParam == kSampleTimerId) {
                OnSampleTimer();
            }
            return 0;
        case kMsgTaskbarEvent:
            g_taskbarEventPending = false;
            StartRefreshBurst();
            if (g_titleEventPending.exchange(false)) {
                BeginFastReadings();
            }
            // An auto-hide slide fires location changes every frame: an
            // event right after a tick is coalesced into one tick a moment
            // later rather than dropped.
            if (GetTickCount64() - g_lastTickTime >= kCoalesceDelayMs) {
                Tick(false);
            } else {
                SetTimer(hwnd, kCoalesceTimerId, kCoalesceDelayMs, nullptr);
            }
            return 0;
        case kMsgPassResult:
            OnPassResult(wParam, lParam);
            return 0;
        case kMsgTaskbarReady:
            OnTaskbarReady(wParam, lParam);
            return 0;
        case kMsgTaskbarMoving:
            OnTaskbarMoving(lParam);
            Tick(false);
            return 0;
        case kMsgTaskbarZOrder:
            g_zOrderEventPending = false;
            OnTaskbarZOrder();
            return 0;
        case kMsgTaskbarRecolored: {
            UINT& last = g_taskbarRecolored[reinterpret_cast<HWND>(lParam)];
            last = std::max(last, static_cast<UINT>(wParam));
            Tick(false);
            return 0;
        }
        case kMsgReload:
            Tick(false);  // the reload flag is picked up inside
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// A class that already exists would be one a previous instance left behind,
// with its window procedure in an unloaded image - never reused: every
// failure is a failure. (Every window is destroyed before its class is
// unregistered at unload, so this can't happen after a clean unload.)
bool RegisterClasses() {
    WNDCLASSEXW frameClass{};
    frameClass.cbSize = sizeof(frameClass);
    frameClass.lpfnWndProc = FrameWndProc;
    frameClass.hInstance = g_module;
    frameClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    frameClass.lpszClassName = kFrameClassName;
    if (!RegisterClassExW(&frameClass)) {
        Wh_Log(L"RegisterClassExW (frame) failed: %u", GetLastError());
        return false;
    }

    WNDCLASSEXW ctrlClass{};
    ctrlClass.cbSize = sizeof(ctrlClass);
    ctrlClass.lpfnWndProc = CtrlWndProc;
    ctrlClass.hInstance = g_module;
    ctrlClass.lpszClassName = kCtrlClassName;
    if (!RegisterClassExW(&ctrlClass)) {
        Wh_Log(L"RegisterClassExW (ctrl) failed: %u", GetLastError());
        UnregisterClassW(kFrameClassName, g_module);
        return false;
    }

    WNDCLASSEXW appBarClass{};
    appBarClass.cbSize = sizeof(appBarClass);
    appBarClass.lpfnWndProc = AppBarWndProc;
    appBarClass.hInstance = g_module;
    appBarClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    appBarClass.lpszClassName = kAppBarClassName;
    if (!RegisterClassExW(&appBarClass)) {
        Wh_Log(L"RegisterClassExW (appbar) failed: %u", GetLastError());
        UnregisterClassW(kFrameClassName, g_module);
        UnregisterClassW(kCtrlClassName, g_module);
        return false;
    }

    WNDCLASSEXW refreshClass{};
    refreshClass.cbSize = sizeof(refreshClass);
    refreshClass.lpfnWndProc = RefreshWndProc;
    refreshClass.hInstance = g_module;
    refreshClass.lpszClassName = kRefreshClassName;
    if (!RegisterClassExW(&refreshClass)) {
        Wh_Log(L"RegisterClassExW (refresher) failed: %u", GetLastError());
        UnregisterClassW(kFrameClassName, g_module);
        UnregisterClassW(kCtrlClassName, g_module);
        UnregisterClassW(kAppBarClassName, g_module);
        return false;
    }

    return true;
}

DWORD WINAPI ModThreadProc(LPVOID) {
    // Wait until the taskbar exists and make sure this process owns it.
    for (;;) {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) {
            DWORD pid = 0;
            GetWindowThreadProcessId(tray, &pid);
            if (pid != GetCurrentProcessId()) {
                Wh_Log(L"Taskbar belongs to another process, nothing to do here");
                return 0;
            }
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 1000) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    SetThreadPerMonitorDpiAware();
    g_threadStartTick = GetTickCount64();
    // The composition (acrylic fill) lives on this thread: an apartment (left
    // on every exit path) and a dispatcher queue for it.
    struct Apartment {
        bool entered = false;
        Apartment() {
            try {
                winrt::init_apartment(winrt::apartment_type::single_threaded);
                entered = true;
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"init_apartment threw %08X", e.code().value);
            }
        }
        ~Apartment() {
            if (entered) {
                winrt::uninit_apartment();
            }
        }
    } apartment;

    Gdiplus::GdiplusStartupInput gdiplusInput;
    ULONG_PTR gdiplusToken = 0;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, nullptr) != Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return 0;
    }

    if (!RegisterClasses()) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }

    HWND ctrl = CreateWindowExW(0, kCtrlClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                g_module, nullptr);
    if (!ctrl) {
        Wh_Log(L"CreateWindowExW (ctrl) failed: %u", GetLastError());
        UnregisterClassW(kFrameClassName, g_module);
        UnregisterClassW(kCtrlClassName, g_module);
        UnregisterClassW(kAppBarClassName, g_module);
        UnregisterClassW(kRefreshClassName, g_module);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return 0;
    }
    g_ctrlWnd = ctrl;
    InitComposition();

    // The appbar thread; its first shell state is waited for briefly so the
    // first layouts know about auto-hide and the primary taskbar's edge.
    g_appBarWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_appBarStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_taskbarRestored = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_shellStateReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_appBarWake && g_appBarStop && g_shellStateReady) {
        g_appBarThread = CreateThread(nullptr, 0, AppBarThreadProc, nullptr, 0, nullptr);
        if (g_appBarThread) {
            WaitForSingleObject(g_shellStateReady, 500);
        } else {
            Wh_Log(L"CreateThread (appbar) failed: %u", GetLastError());
        }
    } else {
        Wh_Log(L"CreateEventW (appbar) failed: %u", GetLastError());
    }

    // Clear before loading so a settings change racing with the load is
    // picked up by the next tick instead of lost.
    g_reloadRequested = false;
    LoadSettings();
    Tick(true);
    SetTimer(ctrl, kTimerId, kTickIntervalMs, nullptr);

    // Taskbar show/hide/move events from this process, delivered to this
    // thread's message loop.
    HWINEVENTHOOK taskbarEventHook =
        SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                        TaskbarWinEventProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
    if (!taskbarEventHook) {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
    }
    HWINEVENTHOOK foregroundEventHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                        ForegroundWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!foregroundEventHook) {
        Wh_Log(L"SetWinEventHook (foreground) failed: %u", GetLastError());
    }
    // Minimize start and end, move/size end (the handler ignores the other
    // system events in the range, all rare).
    HWINEVENTHOOK windowEventHook =
        SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MINIMIZEEND, nullptr,
                        ForegroundWinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!windowEventHook) {
        Wh_Log(L"SetWinEventHook (window events) failed: %u", GetLastError());
    }
    HookForegroundLocation(GetForegroundWindow());

    for (;;) {
        DWORD wait = MsgWaitForMultipleObjectsEx(1, &g_stopEvent, INFINITE, QS_ALLINPUT,
                                                 MWMO_INPUTAVAILABLE);
        if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED) {
            break;
        }

        bool quit = false;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                quit = true;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        if (quit) {
            break;
        }
    }

    g_ctrlWnd = nullptr;
    KillTimer(ctrl, kTimerId);
    KillTimer(ctrl, kCoalesceTimerId);
    KillTimer(ctrl, kRefreshTimerId);
    KillTimer(ctrl, kColorTimerId);
    KillTimer(ctrl, kSampleTimerId);
    if (taskbarEventHook) {
        UnhookWinEvent(taskbarEventHook);
    }
    if (foregroundEventHook) {
        UnhookWinEvent(foregroundEventHook);
    }
    if (windowEventHook) {
        UnhookWinEvent(windowEventHook);
    }
    HookForegroundLocation(nullptr);

    // No more posted work; the synchronous restores below also drain any
    // hook call still in flight on the taskbar thread.
    RemoveAsyncHooks();
    LiftAllVeils();

    // Everything vanishes together. One trip to the taskbar thread hands the
    // taskbar back in a single step - handlers off (they are the one thing on
    // that thread that calls back into this code, so this must complete),
    // ears removed, background and stroke restored - and reports when that
    // has been rendered; the frames are taken off the screen at that moment
    // (an immediate layered-window update). The slow part (appbars, work
    // area) comes after, when nothing of the mod is visible.
    //
    // Nothing here is given up on: Windhawk unmaps the image the moment
    // Wh_ModUninit returns, so a handler left subscribed or a thread still
    // running would crash Explorer. The calls into the taskbar thread are
    // plain SendMessages (this thread keeps handling sent messages while it
    // waits, so they can't deadlock); a busy taskbar delays the unload by a
    // few seconds at worst.
    if (g_taskbarXamlAvailable) {
        if (HWND taskbar = FindCurrentProcessTaskbarWnd()) {
            if (g_taskbarRestored) {
                ResetEvent(g_taskbarRestored);
            }
            ULONGLONG t0 = GetTickCount64();
            if (!RunFromWindowThreadTimeout(taskbar, RestoreTaskbarsOnTaskbarThread, nullptr, 0)) {
                Wh_Log(L"taskbar window gone before the hand-back");
            } else if (g_taskbarRestored) {
                DWORD rendered = WaitForSingleObject(g_taskbarRestored, 1000);
                Wh_Log(L"taskbar handed back in %llu ms (%s)", GetTickCount64() - t0,
                       rendered == WAIT_OBJECT_0 ? L"rendered" : L"render not seen");
            }
            // The Rendered handler must not outlive the image, whether or
            // not it fired.
            RunFromWindowThreadTimeout(taskbar, UnsubscribeRestoredRenderOnTaskbarThread, nullptr,
                                       0);
        } else {
            Wh_Log(L"no taskbar window, nothing to hand back");
        }
    }
    HideOverlays(*g_overlays);
    // The frame windows go now, so no other thread can have a reason to
    // send to this one while it waits below.
    DestroyOverlays(*g_overlays);
    g_overlaysComposition = false;
    g_overlaysBackdrop = false;
    ShutdownComposition();
    g_overlays.reset();  // after the pump above: nothing dispatched there can reach it any more
    g_xamlCornersOwned = false;
    ResetPassStates();
    // Only now the appbars: the appbar thread removes them (each removal is a
    // synchronous shell call) and exits; waited for however long that takes.
    if (g_appBarStop) {
        SetEvent(g_appBarStop);
    }
    if (g_appBarThread) {
        WaitForSingleObject(g_appBarThread, INFINITE);
        CloseHandle(g_appBarThread);
        g_appBarThread = nullptr;
    }
    if (g_appBarWake) {
        CloseHandle(g_appBarWake);
        g_appBarWake = nullptr;
    }
    if (g_appBarStop) {
        CloseHandle(g_appBarStop);
        g_appBarStop = nullptr;
    }
    if (g_taskbarRestored) {
        HANDLE restored = g_taskbarRestored;
        g_taskbarRestored = nullptr;
        CloseHandle(restored);
    }
    if (g_shellStateReady) {
        CloseHandle(g_shellStateReady);
        g_shellStateReady = nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(g_shellMutex);
        g_appBarWishes.clear();
        g_appBarStatus.clear();
        g_shellState = ShellState{};
    }
    {
        std::lock_guard<std::mutex> lock(g_revealMutex);
        g_reveals.clear();
    }
    g_seenTaskbars.clear();
    g_veils.clear();
    g_taskbarReady.clear();
    g_taskbarRecolored.clear();
    g_cloakedSince.clear();
    g_layoutSignature.clear();
    g_adaptive.clear();
    g_titleColors.clear();

    if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        RemovePropW(taskbar, kAccentProp);
        RemovePropW(taskbar, kBorderAccentProp);
    }
    DestroyWindow(ctrl);
    UnregisterClassW(kFrameClassName, g_module);
    UnregisterClassW(kCtrlClassName, g_module);
    UnregisterClassW(kAppBarClassName, g_module);
    UnregisterClassW(kRefreshClassName, g_module);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

// ---------------------------------------------------------------------------
// dwm.exe side: one corner radius for normal, snapped and maximized windows
// (the mechanism Alexey Lavrinenko contributed to Custom Window Corner Radius)
// ---------------------------------------------------------------------------

namespace dwm {

// The dwm.exe part is active with any of its options on; the corner
// radius is changed only when one is set (-1 leaves the corners to
// Windows). (The radius query is what tells the border hook a window is
// maximized, so it is followed - not answered - for the border and shadow
// options alone.)
std::atomic<bool> g_enabled{true};  // any option on
std::atomic<int> g_radius{20};      // -1 Windows' corners, 0 square, else the radius
std::atomic<bool> g_maximizedBorder{false};
std::atomic<bool> g_maximizedShadow{false};
std::atomic<bool> g_customBorderColors{false};
// Monitors set to the primary one: windows on the other monitors, where
// there is no frame, keep Windows' corners, border and shadow.
std::atomic<bool> g_primaryOnly{false};
std::atomic<uint32_t> g_activeBorderArgb{0xFFFFFFFF};    // the accent, once read
std::atomic<uint32_t> g_inactiveBorderArgb{0xFF3C3C3C};

// The primary taskbar window, which the explorer.exe part publishes the
// accent colors on: looked up again only when the one found before is gone
// (border updates ask for it, and they come often).
std::atomic<HWND> g_publishingTaskbar{nullptr};
HWND PublishingTaskbar() {
    HWND taskbar = g_publishingTaskbar.load();
    if (!taskbar || !IsWindow(taskbar)) {
        taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        g_publishingTaskbar = taskbar;
    }
    return taskbar;
}

// The Windows accent color, as the explorer.exe part publishes it on the
// taskbar window (see kAccentProp); dwmapi's colorization color only when
// that part isn't running - read inside dwm.exe it is DWM's own account's,
// not the user's, and doesn't follow the user's accent.
bool AccentColor(uint32_t& argb) {
    if (HWND taskbar = PublishingTaskbar()) {
        auto published = reinterpret_cast<ULONG_PTR>(GetPropW(taskbar, kAccentProp));
        if (published) {
            argb = static_cast<uint32_t>(published);
            return true;
        }
    }
    DWORD color = 0;
    BOOL opaque = FALSE;
    if (FAILED(DwmGetColorizationColor(&color, &opaque))) {
        return false;
    }
    argb = 0xFF000000 | (color & 0x00FFFFFF);
    return true;
}
// "#AARRGGBB", "#RRGGBB" (opaque) or "accent" (`isAccent` says which).
// False for anything else.
bool ParseBorderColor(PCWSTR text, uint32_t& argb, bool& isAccent) {
    isAccent = false;
    if (!text) {
        return false;
    }
    while (*text == L' ') {
        text++;
    }
    if (_wcsicmp(text, L"accent") == 0) {
        isAccent = true;
        return AccentColor(argb);
    }
    if (*text == L'#') {
        text++;
    }
    size_t length = wcslen(text);
    if (length != 6 && length != 8) {
        return false;
    }
    wchar_t* end = nullptr;
    unsigned long value = wcstoul(text, &end, 16);
    if (!end || *end != L'\0') {
        return false;
    }
    argb = length == 6 ? (0xFF000000 | (uint32_t)value) : (uint32_t)value;
    return true;
}
// Which of the two colors follow the accent color (re-read by the
// decoration timer, so a change of accent color is picked up).
std::atomic<bool> g_activeBorderIsAccent{false};
std::atomic<bool> g_inactiveBorderIsAccent{false};
// `fallbackAccent`: a setting that can't be read falls back to the accent
// color (the active border's default) rather than to `fallback`.
bool LoadBorderColor(PCWSTR key, std::atomic<uint32_t>& into, std::atomic<bool>& isAccent,
                     uint32_t fallback, bool fallbackAccent = false) {
    auto text = WindhawkUtils::StringSetting::make(key);
    uint32_t argb = 0;
    bool accent = false;
    bool ok = *text.get() && ParseBorderColor(text.get(), argb, accent);
    if (!ok && fallbackAccent && AccentColor(argb)) {
        into = argb;
        isAccent = true;
        return false;
    }
    into = ok ? argb : fallback;
    isAccent = ok && accent;
    return ok;
}

// Windows' full rounding radius at 100%; anything smaller is the "small"
// style (menus, tooltips) or no rounding at all, which the mod leaves alone.
constexpr float kFullRoundRadius = 8.0f;

void LoadDwmSettings() {
    int radius = Wh_GetIntSetting(L"windowCorners.radius");
    g_maximizedBorder = Wh_GetIntSetting(L"windowCorners.maximizedBorder") != 0;
    g_maximizedShadow = Wh_GetIntSetting(L"windowCorners.maximizedShadow") != 0;
    g_customBorderColors = Wh_GetIntSetting(L"windowCorners.borderColors") != 0;
    // A ready-made theme sets these outright (the border colors stay off);
    // the window corner radius can be overridden.
    auto themeSetting = WindhawkUtils::StringSetting::make(L"theme");
    if (const ThemeDef* theme = FindTheme(themeSetting.get())) {
        radius = theme->windowRadius;
        g_maximizedShadow = theme->maximizedShadow;
        g_maximizedBorder = theme->maximizedBorder;
        g_customBorderColors = false;
        int wanted = Wh_GetIntSetting(L"overrides.windowCornerRadius");
        if (wanted >= -1) {
            radius = wanted;
        }
        Wh_Log(L"[dwm] theme: %s", theme->id);
    }
    g_radius = radius < 0 ? -1 : radius;
    auto monitors = WindhawkUtils::StringSetting::make(L"general.monitors");
    g_primaryOnly = wcscmp(monitors.get(), L"primary") == 0;
    g_enabled = g_radius >= 0 || g_maximizedBorder || g_maximizedShadow || g_customBorderColors;
    if (g_customBorderColors) {
        bool active = LoadBorderColor(L"windowCorners.activeBorderColor", g_activeBorderArgb,
                                      g_activeBorderIsAccent, 0xFFFFFFFF, true);
        // Empty or invalid: the active color for inactive windows too.
        bool inactive = LoadBorderColor(L"windowCorners.inactiveBorderColor",
                                        g_inactiveBorderArgb, g_inactiveBorderIsAccent,
                                        g_activeBorderArgb);
        if (!inactive) {
            g_inactiveBorderIsAccent = g_activeBorderIsAccent.load();
        }
        Wh_Log(L"[dwm] border colors: active #%08X%s, inactive #%08X%s", g_activeBorderArgb.load(),
               active ? L"" : L" (setting not understood, default used)",
               g_inactiveBorderArgb.load(),
               inactive ? L"" : L" (setting empty or not understood, active color used)");
    }
    Wh_Log(L"[dwm] window corner radius %d (-1 = Windows' corners, 0 = square), maximized "
           L"border %s, maximized shadow %s, custom border colors %s",
           g_radius.load(), g_maximizedBorder ? L"on" : L"off",
           g_maximizedShadow ? L"on" : L"off", g_customBorderColors ? L"on" : L"off");
}

// CTopLevelWindow::GetRadiusFromCornerStyle: pure style -> radius mapping
// with no maximized check. Resolved, not hooked.
using GetRadiusFromCornerStyle_t = float(WINAPI*)(void* self);
GetRadiusFromCornerStyle_t GetRadiusFromCornerStyle;

// CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle: returns 0 for
// maximized / full-screen windows even when the style is rounded.
using GetFloatCornerRadiusForCurrentStyle_t = float(WINAPI*)(void* self);
GetFloatCornerRadiusForCurrentStyle_t GetFloatCornerRadiusForCurrentStyle_Original;
// What the last corner-radius query on this thread said about its window:
// DWM asks right before it sets the window's border parameters, which is
// where the maximized decorations are decided (see SetBorderParameters).
struct LastRadiusQuery {
    bool valid = false;
    bool squared = false;    // DWM answered 0: maximized or full-screen
    HWND hwnd = nullptr;     // the window, when DWM's data gives it away
    void* window = nullptr;  // its CTopLevelWindow
    float radius = 0.0f;     // the corner radius the window was given
    bool eligible = false;   // a fully rounded style on a framed monitor: the mod's to decorate
};
thread_local LastRadiusQuery g_lastRadiusQuery;

// The monitor a window is on - the nearest one: a window in transit (being
// moved between monitors, restored, animated) can lie on none for a moment,
// and that moment is when DWM sets its border.
HMONITOR MonitorOf(HWND hwnd) {
    return MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
}

// Whether the window is on a monitor the frame is on (every monitor, or the
// primary one only). A window without a handle counts as framed.
bool OnFramedMonitor(HWND hwnd) {
    if (!g_primaryOnly || !hwnd) {
        return true;
    }
    return MonitorOf(hwnd) == MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

// The window's HWND, read from DWM's CWindowData the way Custom Window
// Corner Radius does: CWindowData has no getter for it, but
// CWindowData::IsGhostWindow(HWND*) starts by reading the field, and the
// displacement of that first `mov reg, [rcx+disp]` is the field's offset.
// Everything here only reads: nothing is hooked.
using GetWindowData_t = void*(WINAPI*)(void* self);
GetWindowData_t GetWindowData;  // resolved, optional
void* IsGhostWindowCode;        // resolved, optional
std::atomic<int> g_hwndOffset{-1};  // -1 = unknown

void FindHwndOffset() {
    if (!IsGhostWindowCode) {
        return;
    }
    const BYTE* code = static_cast<const BYTE*>(IsGhostWindowCode);
    for (int i = 0; i + 7 < 64; i++) {
        // REX.W (0x48 / 0x4C) 8B /r with the memory operand [rcx+disp].
        if ((code[i] == 0x48 || code[i] == 0x4C) && code[i + 1] == 0x8B) {
            BYTE modrm = code[i + 2];
            if ((modrm & 0x07) == 0x01 && (modrm & 0xC0) == 0x40) {
                g_hwndOffset = static_cast<signed char>(code[i + 3]);
                break;
            }
            if ((modrm & 0x07) == 0x01 && (modrm & 0xC0) == 0x80) {
                int disp;
                memcpy(&disp, code + i + 3, sizeof(disp));
                g_hwndOffset = disp;
                break;
            }
        }
    }
    Wh_Log(L"[dwm] window handle offset in CWindowData: %d", g_hwndOffset.load());
}

HWND WindowHandleOfData(void* data) {
    int offset = g_hwndOffset;
    if (offset < 0 || !data) {
        return nullptr;
    }
    HWND hwnd = *reinterpret_cast<HWND*>(static_cast<BYTE*>(data) + offset);
    return IsWindow(hwnd) ? hwnd : nullptr;
}

HWND WindowHandleOf(void* topLevelWindow) {
    if (!GetWindowData) {
        return nullptr;
    }
    return WindowHandleOfData(GetWindowData(topLevelWindow));
}

// The windows the mod rounds, kept by the radius hook below for the
// animated clip further down, which has no say of its own: the radius of
// their corner style (what DWM's own animation radius is scaled from) and
// whether DWM last squared them (maximized, snapped or full-screen), with
// when that stopped.
struct RoundedWindow {
    float styleRadius = 0.0f;
    bool squared = false;
    ULONGLONG unsquaredAt = 0;  // GetTickCount64 when DWM stopped squaring it
};
std::mutex g_roundedWindowsMutex;
std::unordered_map<HWND, RoundedWindow> g_roundedWindows;

void NoteRounded(HWND hwnd, bool rounded, float styleRadius = 0.0f, bool squared = false) {
    if (!hwnd) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_roundedWindowsMutex);
    if (!rounded) {
        g_roundedWindows.erase(hwnd);
        return;
    }
    if (g_roundedWindows.size() > 4096) {
        g_roundedWindows.clear();  // handles of windows long gone; re-learned as asked
    }
    RoundedWindow& window = g_roundedWindows[hwnd];
    if (window.squared && !squared) {
        window.unsquaredAt = GetTickCount64();
    }
    window.styleRadius = styleRadius;
    window.squared = squared;
}

bool RoundedByMod(HWND hwnd, RoundedWindow& out) {
    std::lock_guard<std::mutex> lock(g_roundedWindowsMutex);
    auto it = g_roundedWindows.find(hwnd);
    if (it == g_roundedWindows.end()) {
        return false;
    }
    out = it->second;
    return true;
}

float WINAPI GetFloatCornerRadiusForCurrentStyle_Hook(void* self) {
    float r = GetFloatCornerRadiusForCurrentStyle_Original(self);
    if (!g_enabled) {
        return r;
    }

    float base = GetRadiusFromCornerStyle(self);
    if (base < kFullRoundRadius) {
        HWND hwnd = WindowHandleOf(self);
        NoteRounded(hwnd, false);
        g_lastRadiusQuery = {true, false, hwnd, self, r};
        return r;  // DONOTROUND or the small style: not ours to change
    }

    HWND hwnd = WindowHandleOf(self);
    if (!OnFramedMonitor(hwnd)) {
        // No frame on this monitor: Windows' corners, and the border hook
        // treats the window as one of DWM's own (not squared by us).
        NoteRounded(hwnd, false);
        g_lastRadiusQuery = {true, false, hwnd, self, r};
        return r;
    }
    bool squared = r == 0.0f;  // maximized or full-screen: Windows squares it
    NoteRounded(hwnd, true, base, squared);
    int radius = g_radius;
    // -1: the corners are Windows' (the query still serves the border and
    // shadow); otherwise the one radius, maximized or not (0: square).
    float given = radius < 0 ? r : static_cast<float>(radius);
    g_lastRadiusQuery = {true, squared, hwnd, self, given, true};
    return given;
}

// Corners while a window animates. Windows animates a window into and out of
// the maximized state with a clip of its own, without asking the radius
// query above: CTopLevelWindow3D::UpdateAnimatedResources runs every frame
// of the animation and sets the clip's rectangle and corner radius - a
// maximize (animation type 2) makes a new clip each frame through
// ResourceHelper::CreateRectangleGeometry, a restore (type 4) updates the
// clip it has through CRectangleGeometryProxy::SetRectangle. Stock Windows
// draws maximized windows square, so DWM eases the radius between the two
// states (8, 7, 6, 3, 1, then 0 for the rest of a maximize) and a window the
// mod rounds went square for the length of the animation, rounded again at
// its end. Such a window has the same radius in both states, so while
// UpdateAnimatedResources runs for it, a transition - the window maximized
// or snapped now, or DWM stopped squaring it within the last two seconds -
// keeps the mod's radius throughout, at the window's DPI; other animations
// (minimize, back from the taskbar) keep DWM's easing, scaled from the corner
// style's radius to the mod's. CTopLevelWindow3D carries the window's
// CWindowData, stored by CTopLevelWindow3D::SetWindowData; the offset of that
// field is read from the function's code, as the handle's offset is read
// above.
void* SetWindowData3DCode;               // resolved, read for the offset
std::atomic<int> g_windowData3DOffset{-1};  // -1 = unknown

void FindWindowData3DOffset() {
    if (!SetWindowData3DCode) {
        return;
    }
    // The registers holding `this` and the CWindowData argument (x64: rcx,
    // rdx; ARM64: x0, x1), following plain register moves; the first store
    // of the argument through `this` is the field. A call clobbers the
    // argument registers; any other write to a register drops it.
    std::vector<std::string> self = {"rcx", "x0"};
    std::vector<std::string> data = {"rdx", "x1"};
    auto has = [](const std::vector<std::string>& set, const std::string& r) {
        return std::find(set.begin(), set.end(), r) != set.end();
    };
    auto drop = [](std::vector<std::string>& set, const std::string& r) {
        set.erase(std::remove(set.begin(), set.end(), r), set.end());
    };
    auto trim = [](std::string t) {
        size_t a = t.find_first_not_of(' ');
        size_t b = t.find_last_not_of(' ');
        return a == std::string::npos ? std::string() : t.substr(a, b - a + 1);
    };
    const BYTE* code = static_cast<const BYTE*>(SetWindowData3DCode);
    for (int i = 0; i < 48; i++) {
        WH_DISASM_RESULT result = {};
        if (!Wh_Disasm(const_cast<BYTE*>(code), &result) || result.length == 0) {
            break;
        }
        code += result.length;
        std::string text = result.text;
        std::transform(text.begin(), text.end(), text.begin(),
                       [](char c) { return c >= 'A' && c <= 'Z' ? static_cast<char>(c + 32) : c; });
        size_t space = text.find(' ');
        std::string op = text.substr(0, space);
        std::string rest = space == std::string::npos ? "" : text.substr(space + 1);
        if (op.empty()) {
            continue;
        }
        if (op == "ret") {
            break;
        }
        if (op == "call" || op == "bl" || op == "blr") {
            for (const char* r : {"rcx", "rdx", "r8", "r9", "r10", "r11", "rax", "x0", "x1",
                                  "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x16", "x17"}) {
                drop(self, r);
                drop(data, r);
            }
            continue;
        }
        size_t comma = rest.find(',');
        if (comma == std::string::npos) {
            continue;
        }
        std::string first = trim(rest.substr(0, comma));
        std::string second = trim(rest.substr(comma + 1));
        // The store: x64 "mov qword ptr [rcx+0x58], rdx", ARM64
        // "str x1, [x0, #0x58]".
        std::string base, offset, value;
        if (op == "mov" && first.find('[') != std::string::npos) {
            size_t open = first.find('['), plus = first.find('+', open);
            size_t close = first.find(']', open);
            if (plus != std::string::npos && close != std::string::npos) {
                base = trim(first.substr(open + 1, plus - open - 1));
                offset = trim(first.substr(plus + 1, close - plus - 1));
                value = second;
            }
        } else if (op == "str" && second.size() > 2 && second[0] == '[' && second.back() == ']') {
            size_t inner = second.find(',');
            if (inner != std::string::npos) {
                base = trim(second.substr(1, inner - 1));
                offset = trim(second.substr(inner + 1, second.size() - inner - 2));
                if (!offset.empty() && offset[0] == '#') {
                    offset = offset.substr(1);
                }
                value = first;
            }
        }
        if (!base.empty()) {
            if (has(self, base) && has(data, value)) {
                unsigned long found = strtoul(offset.c_str(), nullptr, 0);
                if (found >= sizeof(void*) && found <= 0x1000 && found % sizeof(void*) == 0) {
                    g_windowData3DOffset = static_cast<int>(found);
                }
                break;
            }
            continue;  // a store writes no register
        }
        if (op == "cmp" || op == "test" || op == "push" || op == "stp" || op == "str" ||
            op[0] == 'j' || op == "cbz" || op == "cbnz" || op == "b") {
            continue;
        }
        // A register written: an alias if copied from one, else no longer one.
        bool copy = op == "mov" && second.find('[') == std::string::npos;
        bool selfCopy = copy && has(self, second);
        bool dataCopy = copy && has(data, second);
        drop(self, first);
        drop(data, first);
        if (selfCopy) {
            self.push_back(first);
        }
        if (dataCopy) {
            data.push_back(first);
        }
    }
    Wh_Log(L"[dwm] window data offset in CTopLevelWindow3D: %d", g_windowData3DOffset.load());
}

HWND WindowHandleOf3D(void* window3D) {
    int offset = g_windowData3DOffset;
    if (offset < 0 || !window3D) {
        return nullptr;
    }
    return WindowHandleOfData(*reinterpret_cast<void**>(static_cast<BYTE*>(window3D) + offset));
}

// D2D_POINTANDSIZE_L: the animated rectangle, in pixels.
struct PointAndSizeL {
    LONG x, y, width, height;
};

thread_local void* g_animatedWindow3D;  // the CTopLevelWindow3D whose clip is being set

float AnimatedClipRadius(void* window3D, const PointAndSizeL* rect, float radius,
                         HWND* transitionWindow = nullptr) {
    int wanted = g_radius;
    if (wanted <= 0) {
        return radius;  // Windows' corners, or square: DWM's clip is right
    }
    HWND hwnd = WindowHandleOf3D(window3D);
    RoundedWindow window;
    if (!hwnd || !RoundedByMod(hwnd, window) || window.styleRadius <= 0.0f) {
        return radius;
    }
    constexpr ULONGLONG kRestoreWindowMs = 2000;
    bool transition = IsZoomed(hwnd) || window.squared ||
                      (window.unsquaredAt && GetTickCount64() - window.unsquaredAt < kRestoreWindowMs);
    if (transition && transitionWindow) {
        *transitionWindow = hwnd;
    }
    float result;
    if (transition || radius <= 0.0f) {
        UINT dpi = GetDpiForWindow(hwnd);
        result = wanted * (dpi ? dpi / 96.0f : 1.0f);
        if (rect && rect->width > 0 && rect->height > 0) {
            result = std::min(result, std::min(rect->width, rect->height) / 2.0f);
        }
    } else {
        // DWM's radius, with its DPI and animation scaling, for the mod's.
        result = radius * wanted / window.styleRadius;
    }
    return result;
}

using UpdateAnimatedResources_t = long(WINAPI*)(void* self);
UpdateAnimatedResources_t UpdateAnimatedResources_Original;
long WINAPI UpdateAnimatedResources_Hook(void* self) {
    void* previous = g_animatedWindow3D;
    g_animatedWindow3D = self;
    long result = UpdateAnimatedResources_Original(self);
    g_animatedWindow3D = previous;
    return result;
}

using CreateRectangleGeometry_t = long(WINAPI*)(const PointAndSizeL* rect, float radius,
                                                void** geometry);
CreateRectangleGeometry_t CreateRectangleGeometry_Original;
thread_local bool g_inCreateRectangleGeometry;  // its SetRectangle call is already handled
long WINAPI CreateRectangleGeometry_Hook(const PointAndSizeL* rect, float radius,
                                         void** geometry) {
    if (g_animatedWindow3D && g_enabled) {
        radius = AnimatedClipRadius(g_animatedWindow3D, rect, radius);
    }
    bool previous = g_inCreateRectangleGeometry;
    g_inCreateRectangleGeometry = true;
    long result = CreateRectangleGeometry_Original(rect, radius, geometry);
    g_inCreateRectangleGeometry = previous;
    return result;
}

// CRectangleGeometryProxy::SetRectangle: the clip geometry itself, with an X
// and Y radius per corner (DWM passes one radius in all eight; corners that
// differ are someone else's and left alone).
using SetRectangle_t = long(WINAPI*)(void* self, float left, float top, float right,
                                     float bottom, float topLeftX, float topLeftY,
                                     float topRightX, float topRightY, float bottomLeftX,
                                     float bottomLeftY, float bottomRightX, float bottomRightY,
                                     bool flag);
SetRectangle_t SetRectangle_Original;

// A restore cross-fades two layers: the maximized image, shrinking inside the
// animated clip above, and the restored window, stretched from its own size
// up to that clip and shrinking with it. The restored window keeps its
// content clip - the one CWindowBorder::SetClipRectangle sets, in the
// window's own pixels, with the window's radius - and the stretch widens
// that clip's corners with it: about 1.7 times wide and 1.2 times tall half
// way, an oval far rounder than the window's corner, shrinking to it at the
// end. So the content clip of each window the mod rounds is remembered as it
// is set (its geometry, rectangle and radius), and every frame of a restore
// sets it again with each radius divided by the stretch along its axis,
// which on screen is the window's own radius; the last frames, at no
// stretch, put back the radius as it was. The geometry belongs to the
// window's border, so it is forgotten with the border (see
// ForgetBorderObject), none is kept unless the destructor hooks are in, and
// it is only used within a second of being set: DWM sets it as a restore
// begins (the window takes its restored size), so a geometry the border has
// since replaced is never called into.
struct ContentClip {
    void* border = nullptr;
    void* geometry = nullptr;  // CRectangleGeometryProxy*
    float left = 0, top = 0, right = 0, bottom = 0;
    float radius = 0;
    bool flag = false;
    ULONGLONG setAt = 0;  // GetTickCount64
};
constexpr ULONGLONG kContentClipFreshMs = 1000;
std::mutex g_contentClipsMutex;
std::unordered_map<HWND, ContentClip> g_contentClips;
thread_local void* g_clipBorder;    // the CWindowBorder whose content clip is being set
thread_local void* g_clipGeometry;  // and the geometry it is set on

void ForgetContentClips(void* border) {
    std::lock_guard<std::mutex> lock(g_contentClipsMutex);
    for (auto it = g_contentClips.begin(); it != g_contentClips.end();) {
        if (it->second.border == border) {
            it = g_contentClips.erase(it);
        } else {
            ++it;
        }
    }
}

using SetClipRectangle_t = void(WINAPI*)(void* self, void* geometry, const RECT* rect);
SetClipRectangle_t SetClipRectangle_Original;
void WINAPI SetClipRectangle_Hook(void* self, void* geometry, const RECT* rect) {
    void* previousBorder = g_clipBorder;
    void* previousGeometry = g_clipGeometry;
    g_clipBorder = self;
    g_clipGeometry = geometry;
    SetClipRectangle_Original(self, geometry, rect);
    g_clipBorder = previousBorder;
    g_clipGeometry = previousGeometry;
}

// Defined with the border records further down, whose border-to-window map
// it reads.
void RememberContentClip(void* geometry, float left, float top, float right, float bottom,
                         float radius, bool flag);

std::atomic<ULONGLONG> g_contentClipLogged{0};  // one log line per restore

// One frame of a restore: the window's content clip, its radius divided by
// how far the animated rectangle stretches the window.
void FitContentClip(HWND hwnd, float width, float height) {
    if (IsZoomed(hwnd)) {
        return;  // a maximize shrinks the window: its clip stays inside the animated one
    }
    ContentClip clip;
    {
        std::lock_guard<std::mutex> lock(g_contentClipsMutex);
        auto it = g_contentClips.find(hwnd);
        if (it == g_contentClips.end() ||
            GetTickCount64() - it->second.setAt > kContentClipFreshMs) {
            return;
        }
        clip = it->second;
    }
    float clipWidth = clip.right - clip.left, clipHeight = clip.bottom - clip.top;
    if (clip.radius <= 0.0f || clipWidth <= 0.0f || clipHeight <= 0.0f) {
        return;
    }
    float across = std::max(1.0f, width / clipWidth);
    float down = std::max(1.0f, height / clipHeight);
    if (across < 1.01f && down < 1.01f) {
        across = down = 1.0f;  // the window's own size, give or take its border
    }
    float x = clip.radius / across, y = clip.radius / down;
    SetRectangle_Original(clip.geometry, clip.left, clip.top, clip.right, clip.bottom, x, y, x, y,
                          x, y, x, y, clip.flag);
    ULONGLONG now = GetTickCount64();
    if (now - g_contentClipLogged.load() > kContentClipFreshMs) {
        g_contentClipLogged = now;
        Wh_Log(L"[dwm] restore %p: content clip %.0fx%.0f fitted to the stretch (radius %.1f -> "
               L"%.1f x %.1f on the first frame)",
               hwnd, clipWidth, clipHeight, clip.radius, x, y);
    }
}

long WINAPI SetRectangle_Hook(void* self, float left, float top, float right, float bottom,
                              float topLeftX, float topLeftY, float topRightX, float topRightY,
                              float bottomLeftX, float bottomLeftY, float bottomRightX,
                              float bottomRightY, bool flag) {
    bool oneRadius = topLeftY == topLeftX && topRightX == topLeftX && topRightY == topLeftX &&
                     bottomLeftX == topLeftX && bottomLeftY == topLeftX &&
                     bottomRightX == topLeftX && bottomRightY == topLeftX;
    HWND transition = nullptr;
    if (g_enabled && oneRadius && g_clipBorder && self == g_clipGeometry && !g_animatedWindow3D) {
        RememberContentClip(self, left, top, right, bottom, topLeftX, flag);
    } else if (g_animatedWindow3D && g_enabled && !g_inCreateRectangleGeometry && oneRadius) {
        PointAndSizeL rect{static_cast<LONG>(left), static_cast<LONG>(top),
                           static_cast<LONG>(right - left), static_cast<LONG>(bottom - top)};
        float radius = AnimatedClipRadius(g_animatedWindow3D, &rect, topLeftX, &transition);
        topLeftX = topLeftY = topRightX = topRightY = radius;
        bottomLeftX = bottomLeftY = bottomRightX = bottomRightY = radius;
    }
    long result = SetRectangle_Original(self, left, top, right, bottom, topLeftX, topLeftY,
                                        topRightX, topRightY, bottomLeftX, bottomLeftY,
                                        bottomRightX, bottomRightY, flag);
    if (transition) {
        FitContentClip(transition, right - left, bottom - top);
    }
    return result;
}

// Border and shadow on maximized windows, and the border colors. DWM
// leaves both out of a maximized window: its shadow style comes to
// CWindowBorder::SetBorderParameters as "none" and its border color with
// alpha 0 (the border style is 0 for every window; the color is what
// decides). With the options on, the hook on that call swaps them for what
// DWM gives floating windows - learned from its own calls, per activation
// state, so no enum value or color is assumed beyond a fallback - when the
// corner-radius query DWM made just before (same thread) said the window is
// squared, i.e. maximized, snapped or full-screen. Every such window gets
// the border; the shadow goes to the topmost maximized window of each
// monitor (snapped windows get none: two side by side would overlap
// theirs), in the style floating windows get in the same activation state;
// the others keep a shadow visual in a style that draws nothing, so a
// handover changes parameters rather than creating visuals. With custom
// border colors on, the same hook recolors floating windows' borders too.
//
// DWM only sets a window's border when something changed for that window,
// as far as it knows: not when another window comes to the top of it, not
// when it becomes or stops being the foreground window while maximized, and
// at load it sets the maximized windows before any floating one, when no
// style or color is known yet. So the mod keeps, per window, what it last
// decided and compares that with what it would decide now, a few times a
// second from a timer on DWM's UI thread - at once, for the monitors
// concerned, when DWM's own update moved a shadow; a window whose decision
// changed is given a visual update (CTopLevelWindow::UpdateWindowVisuals,
// the routine DWM runs for its own reasons), which comes back through the
// hooks with the current decision. Nothing is replayed by hand unless that
// routine is missing.
//
// Why the border call and not CTopLevelWindow::GetShadowStyle: that is a
// tiny internal function whose callers, in DWM's link-time-optimized build,
// rely on registers it doesn't touch; a hook clobbers them and the caller's
// next call faulted on a bad `this` (udwm.dll+0x1504a on 10.0.26100.9818).
// SetBorderParameters and UpdateWindowVisuals are functions other mods hook
// on these builds.
struct StyleTally {
    std::mutex mutex;
    int count[16]{};
    unsigned lastSeen[16]{};  // sequence number of the latest sample, for ties
    HWND windows[16][2]{};    // two distinct windows a style was seen on
    unsigned samples = 0;
    void See(int value, HWND hwnd) {
        if (value > 0 && value < 16) {
            std::lock_guard<std::mutex> lock(mutex);
            count[value]++;
            lastSeen[value] = ++samples;
            if (windows[value][0] != hwnd && windows[value][1] != hwnd) {
                windows[value][1] = windows[value][0];
                windows[value][0] = hwnd;
            }
        }
    }
    // The most seen style - among those seen on two different windows,
    // once there is one, so that a single window dragged for a while can't
    // teach a style of its own.
    int Usual() {
        std::lock_guard<std::mutex> lock(mutex);
        int best = 0;
        for (int pass = 0; pass < 2 && best == 0; pass++) {
            for (int i = 1; i < 16; i++) {
                if (count[i] == 0 || (pass == 0 && !windows[i][1])) {
                    continue;
                }
                if (count[i] > count[best] ||
                    (count[i] == count[best] && lastSeen[i] > lastSeen[best])) {
                    best = i;
                }
            }
        }
        return best;
    }
};
StyleTally g_activeShadowStyles;    // floating windows that were the foreground window
StyleTally g_inactiveShadowStyles;  // floating windows that were not

// The styles DWM gave ordinary active and inactive floating windows on
// 10.0.22000 through 10.0.26100; used only until a floating window of the
// session has shown its own.
constexpr int kUsualActiveShadowStyle = 3;
constexpr int kUsualInactiveShadowStyle = 5;
// The style DWM gives a window while it is dragged or minimized. It once
// drew nothing on a maximized window and served as the "no shadow" style
// (keeping the shadow visual alive across handovers); on current builds it
// draws a shadow, so maximized windows that cast none get DWM's own style
// 0, as DWM gives maximized windows itself.
constexpr int kTransitionalShadowStyle = 4;
constexpr int kNoShadowStyle = 0;
bool ShadowShown(int style) {
    return style != kNoShadowStyle && style != kTransitionalShadowStyle;
}

// Only ordinary app frames at rest teach the tallies: a hidden, layered or
// tool window gets a shadow style of its own, and so does a window while
// it is being dragged or minimized (style 4 on 10.0.26100).
bool IsOrdinaryFrame(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    if ((style & WS_CAPTION) != WS_CAPTION || !(style & WS_THICKFRAME)) {
        return false;
    }
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_NOACTIVATE)) {
        return false;
    }
    GUITHREADINFO info{sizeof(info)};
    DWORD thread = GetWindowThreadProcessId(hwnd, nullptr);
    return !(thread && GetGUIThreadInfo(thread, &info) && (info.flags & GUI_INMOVESIZE));
}

// The top-level window that counts as active for the foreground window:
// the foreground window's root owner (a dialog or a menu keeps its owner's
// frame active), or, for a captionless popup (a tooltip, a web view's
// popup), the maximized window of its thread, if any.
HWND ActiveTopLevelWindow() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return nullptr;
    }
    HWND root = GetAncestor(foreground, GA_ROOTOWNER);
    if (!root) {
        root = foreground;
    }
    if ((GetWindowLongPtrW(root, GWL_STYLE) & WS_CAPTION) != WS_CAPTION && !IsZoomed(root)) {
        DWORD thread = GetWindowThreadProcessId(root, nullptr);
        struct Search {
            DWORD thread;
            HWND found;
        } search{thread, nullptr};
        EnumThreadWindows(
            thread,
            [](HWND hwnd, LPARAM param) -> BOOL {
                auto* s = reinterpret_cast<Search*>(param);
                if (IsWindowVisible(hwnd) && IsZoomed(hwnd) && !IsIconic(hwnd)) {
                    s->found = hwnd;
                    return FALSE;
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&search));
        if (search.found) {
            return search.found;
        }
    }
    return root;
}
bool IsForegroundWindow(HWND hwnd) {
    return hwnd && ActiveTopLevelWindow() == hwnd;
}

// The shadow style a maximized window should get: the one DWM gives
// floating windows in the same activation state. Until one of those has
// been seen, the known style for the state - provided the other state's
// style agrees with what these builds use - else whatever style floating
// windows have shown, which at least exists on this build.
int ShadowStyleFor(bool active) {
    int own = (active ? g_activeShadowStyles : g_inactiveShadowStyles).Usual();
    if (own > 0) {
        return own;
    }
    int known = active ? kUsualActiveShadowStyle : kUsualInactiveShadowStyle;
    int otherKnown = active ? kUsualInactiveShadowStyle : kUsualActiveShadowStyle;
    int other = (active ? g_inactiveShadowStyles : g_activeShadowStyles).Usual();
    if (other > 0 && other != otherKnown) {
        return other;
    }
    return known;
}

// Whether a window is cloaked - on another virtual desktop, mostly. Asked
// of win32k rather than DWM, which this code runs inside.
bool IsCloaked(HWND hwnd) {
    struct WINDOWCOMPOSITIONATTRIBDATA {
        DWORD attrib;
        void* data;
        SIZE_T size;
    };
    using GetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
    static GetWindowCompositionAttribute_t get = [] {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? (GetWindowCompositionAttribute_t)GetProcAddress(
                            user32, "GetWindowCompositionAttribute")
                      : nullptr;
    }();
    if (!get) {
        return false;
    }
    DWORD cloaked = 0;
    WINDOWCOMPOSITIONATTRIBDATA data{18 /* WCA_CLOAKED */, &cloaked, sizeof(cloaked)};
    return get(hwnd, &data) && cloaked != 0;
}

// The border color DWM gives floating windows (D3DCOLORVALUE: r, g, b, a
// floats), by how often each was seen: the most frequent is the system's
// (a window with a border color of its own is a minority).
struct BorderColor {
    float r, g, b, a;
};
BorderColor BorderColorFromArgb(uint32_t argb) {
    return {((argb >> 16) & 0xFF) / 255.0f, ((argb >> 8) & 0xFF) / 255.0f,
            (argb & 0xFF) / 255.0f, ((argb >> 24) & 0xFF) / 255.0f};
}
bool SameColor(const BorderColor& a, const BorderColor& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
// The custom border color for a window in the given activation state.
BorderColor CustomBorderColor(bool active) {
    return BorderColorFromArgb(active ? g_activeBorderArgb.load() : g_inactiveBorderArgb.load());
}
// Windows whose border the custom colors apply to: app frames (a caption
// or a sizing frame), not menus, tooltips or other popups.
bool IsAppFrame(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    return (style & WS_CAPTION) == WS_CAPTION || (style & WS_THICKFRAME);
}
struct ColorTally {
    struct Entry {
        BorderColor color;
        int count;
        HWND windows[2];  // two distinct windows the color was seen on
        unsigned lastSeen;
    };
    std::mutex mutex;
    Entry entries[8]{};
    unsigned samples = 0;
    void See(const BorderColor& color, HWND hwnd) {
        std::lock_guard<std::mutex> lock(mutex);
        Entry* slot = nullptr;
        for (Entry& e : entries) {
            if (e.count > 0 && SameColor(e.color, color)) {
                e.count++;
                e.lastSeen = ++samples;
                if (e.windows[0] != hwnd && e.windows[1] != hwnd) {
                    e.windows[1] = e.windows[0];
                    e.windows[0] = hwnd;
                }
                return;
            }
            if (!slot && e.count == 0) {
                slot = &e;
            }
        }
        if (slot) {
            *slot = {color, 1, {hwnd, nullptr}, ++samples};
        }
    }
    // The most seen color - among those seen on two different windows,
    // once there is one, so that one window with a color of its own,
    // dragged for a while, can't teach it.
    bool Usual(BorderColor& out) {
        std::lock_guard<std::mutex> lock(mutex);
        const Entry* best = nullptr;
        for (int pass = 0; pass < 2 && !best; pass++) {
            for (const Entry& e : entries) {
                if (e.count == 0 || (pass == 0 && !e.windows[1])) {
                    continue;
                }
                if (!best || e.count > best->count ||
                    (e.count == best->count && e.lastSeen > best->lastSeen)) {
                    best = &e;
                }
            }
        }
        if (!best) {
            return false;
        }
        out = best->color;
        return true;
    }
};
// By activation, the way DWM colors them: with Windows' "accent color on
// title bars and window borders" on, the active window's border is the
// accent color and the others' stay gray.
ColorTally g_activeBorderColors;
ColorTally g_inactiveBorderColors;
// The accent DWM paints borders with, and whether Windows' "Show accent
// color on title bars and window borders" is on, as the explorer.exe part
// publishes them (see kBorderAccentProp); false when it hasn't.
bool WindowsBorderAccent(BorderColor& color, bool& on) {
    HWND taskbar = PublishingTaskbar();
    auto published =
        taskbar ? reinterpret_cast<ULONG_PTR>(GetPropW(taskbar, kBorderAccentProp)) : 0;
    if (!published) {
        return false;
    }
    on = (published >> 24) == 0xFF;
    color = BorderColorFromArgb(0xFF000000u | (static_cast<uint32_t>(published) & 0xFFFFFF));
    return true;
}
// The same color, alpha aside, within a level per channel.
bool SameRgb(const BorderColor& a, const BorderColor& b) {
    constexpr float kLevel = 1.5f / 255.0f;
    return std::abs(a.r - b.r) <= kLevel && std::abs(a.g - b.g) <= kLevel &&
           std::abs(a.b - b.b) <= kLevel;
}

// DWM's border color for a window in the given activation state. The
// active window's is Windows' accent border color whenever that option is
// on - not left to what was learned, so a maximized window always gets it;
// otherwise, and for inactive windows, the color learned from DWM's own
// calls for floating windows in that state - else the other state's.
bool UsualBorderColor(bool active, BorderColor& out) {
    BorderColor accent{};
    bool accentOn = false;
    bool accentKnown = WindowsBorderAccent(accent, accentOn);
    if (active && accentKnown && accentOn) {
        out = accent;
        return true;
    }
    // With the option off, an accent learned while it was on (earlier in
    // the session) doesn't count: the inactive gray stands in for it.
    if (active && accentKnown && g_activeBorderColors.Usual(out) && SameRgb(out, accent) &&
        g_inactiveBorderColors.Usual(out)) {
        return true;
    }
    if ((active ? g_activeBorderColors : g_inactiveBorderColors).Usual(out)) {
        return true;
    }
    return (active ? g_inactiveBorderColors : g_activeBorderColors).Usual(out);
}

using SetBorderParameters_t = HRESULT(WINAPI*)(void* self, const RECT* rect, float radius,
                                               int dpi, const void* color, int borderStyle,
                                               int shadowStyle);
SetBorderParameters_t SetBorderParameters_Original;
using UpdateWindowVisuals_t = HRESULT(WINAPI*)(void* self);
UpdateWindowVisuals_t UpdateWindowVisuals;  // resolved, optional

// What the mod last decided for each maximized window (keyed by HWND), with
// DWM's objects for it: the CTopLevelWindow (to ask DWM for a visual
// update) and, should that routine be missing, the border object and
// parameters to set again by hand.
struct BorderRecord {
    void* window;  // CTopLevelWindow*
    void* border;  // CWindowBorder*
    RECT rect;
    float radius;
    int dpi;
    BorderColor color;
    int borderStyle;
    int shadowStyle;
    bool active;   // the activation state the shadow style was chosen for
    bool colorKnown;  // a border color was available when the decision was made
    HMONITOR monitor;  // where the window was when the decision was made
    DWORD thread;  // the window's thread and class, to tell a reused handle
    wchar_t className[64];
    ULONGLONG refreshed;  // when the mod last asked for a visual update
    // A changed decision is acted on once it has held for two passes: a
    // window on its way between monitors, or an activation half done,
    // would otherwise toggle the shadow and back.
    int pendingShadow;
    bool pendingColor;
    int pendingCount;
};
std::mutex g_borderStateMutex;
std::unordered_map<HWND, BorderRecord> g_borderRecords;
// What the last radius query said about each border object (CWindowBorder*),
// for border updates that come without one.
std::unordered_map<void*, LastRadiusQuery> g_borderStates;

// The records hold DWM's own objects (CTopLevelWindow*, CWindowBorder*),
// which the timer calls into later. Their (virtual) destructors are hooked
// - the same hooks Custom Window Corner Radius and Wobbly Windows have on
// these classes - and a record whose object is being destroyed goes with
// it, so no stored pointer outlives its object.
// Without both hooks the timer doesn't call into stored objects at all:
// a changed decision then asks DWM to re-evaluate every window instead
// (RequestDwmRefresh, the way the other dwm.exe mods do), which costs a
// repaint of all frames.
std::atomic<bool> g_objectLifetimesTracked{false};
void ForgetWindowObject(void* window) {
    std::lock_guard<std::mutex> lock(g_borderStateMutex);
    for (auto it = g_borderRecords.begin(); it != g_borderRecords.end();) {
        if (it->second.window == window) {
            it = g_borderRecords.erase(it);
        } else {
            ++it;
        }
    }
}
void ForgetBorderObject(void* border) {
    ForgetContentClips(border);
    std::lock_guard<std::mutex> lock(g_borderStateMutex);
    g_borderStates.erase(border);
    for (auto it = g_borderRecords.begin(); it != g_borderRecords.end();) {
        if (it->second.border == border) {
            it = g_borderRecords.erase(it);
        } else {
            ++it;
        }
    }
}
using Destructor_t = void(WINAPI*)(void* self);
Destructor_t TopLevelWindowDestructor_Original;
void WINAPI TopLevelWindowDestructor_Hook(void* self) {
    ForgetWindowObject(self);
    TopLevelWindowDestructor_Original(self);
}
Destructor_t WindowBorderDestructor_Original;
void WINAPI WindowBorderDestructor_Hook(void* self) {
    ForgetBorderObject(self);
    WindowBorderDestructor_Original(self);
}

// A content clip as it is set: kept for the window its border belongs to
// (see ContentClip).
void RememberContentClip(void* geometry, float left, float top, float right, float bottom,
                         float radius, bool flag) {
    if (!g_objectLifetimesTracked) {
        return;
    }
    HWND hwnd = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_borderStateMutex);
        auto it = g_borderStates.find(g_clipBorder);
        if (it != g_borderStates.end()) {
            hwnd = it->second.hwnd;
        }
    }
    if (!hwnd) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_contentClipsMutex);
    if (g_contentClips.size() > 4096) {
        g_contentClips.clear();  // windows long gone
    }
    g_contentClips[hwnd] = {g_clipBorder, geometry, left, top, right, bottom,
                            radius,       flag,     GetTickCount64()};
}

// The window's thread and class, as its identity beyond the handle.
void WindowIdentity(HWND hwnd, DWORD& thread, wchar_t (&className)[64]) {
    thread = GetWindowThreadProcessId(hwnd, nullptr);
    if (!GetClassNameW(hwnd, className, 64)) {
        className[0] = L'\0';
    }
}

// A live maximized window the record still belongs to (a handle can be
// reused by a new window before DWM has set a border for it).
bool RecordIsCurrent(HWND hwnd, const BorderRecord& record) {
    if (!IsWindow(hwnd) || !IsZoomed(hwnd) || IsIconic(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }
    DWORD thread;
    wchar_t className[64];
    WindowIdentity(hwnd, thread, className);
    return thread == record.thread && wcscmp(className, record.className) == 0;
}

// Whether `hwnd`, a maximized window, is the one of its monitor that casts
// the shadow: the topmost visible maximized window there. The foreground
// window is checked first because an activation change updates the borders
// of both windows involved before the z-order settles, and the foreground
// window is already known then: if it is a maximized window on the same
// monitor, it is the one. (Windows on another virtual desktop still count
// in the z-order walk: they are cloaked, which can't be asked from inside
// DWM.)
bool CastsMaximizedShadow(HWND hwnd) {
    HMONITOR monitor = MonitorOf(hwnd);
    HWND foreground = ActiveTopLevelWindow();
    if (foreground == hwnd) {
        return true;
    }
    if (foreground && IsZoomed(foreground) && IsWindowVisible(foreground) &&
        !IsCloaked(foreground) && MonitorOf(foreground) == monitor) {
        return false;
    }
    for (HWND above = GetWindow(hwnd, GW_HWNDPREV); above; above = GetWindow(above, GW_HWNDPREV)) {
        if (!IsWindowVisible(above) || IsIconic(above) || !IsZoomed(above)) {
            continue;
        }
        if (GetWindowLongPtrW(above, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
            continue;
        }
        if (IsCloaked(above)) {
            continue;  // on another virtual desktop
        }
        if (MonitorOf(above) == monitor) {
            return false;
        }
    }
    return true;
}

// The mod's decision for a squared window, from the current state of the
// desktop: the border color and the shadow style it should have.
struct Decision {
    bool colorKnown = false;
    BorderColor color{0, 0, 0, 0};
    int shadowStyle = 0;
    bool active = false;
};
Decision Decide(HWND hwnd) {
    Decision d;
    d.active = hwnd && IsForegroundWindow(hwnd);
    if (!OnFramedMonitor(hwnd)) {
        return d;  // DWM's own border and shadow (a maximized window moved off the frame's monitor)
    }
    // A maximized window (or one without a handle, taken as maximized) gets
    // a border only with the option on; a snapped window has DWM's own
    // border, which only the custom colors have anything to say about.
    bool maximized = !hwnd || IsZoomed(hwnd);
    if (!maximized || g_maximizedBorder) {
        if (g_customBorderColors && (!hwnd || IsAppFrame(hwnd))) {
            d.color = CustomBorderColor(d.active);
            d.colorKnown = true;
        } else if (maximized) {
            d.colorKnown = UsualBorderColor(d.active, d.color);
        }
    }
    if (g_maximizedShadow) {
        // Snapped windows are squared too, but only a maximized one casts
        // the shadow: two snapped side by side would overlap theirs.
        // Without a handle (no window data), every maximized window.
        bool topmost =
            !hwnd || (IsZoomed(hwnd) && !IsCloaked(hwnd) && CastsMaximizedShadow(hwnd));
        d.shadowStyle = topmost ? ShadowStyleFor(d.active) : kNoShadowStyle;
    }
    return d;
}

void RequestDwmRefresh(const wchar_t* why);

// Brings a recorded window to the decision with DWM's visual update, which
// applies the hooks' substitution to its border and shadow - or, without
// that routine, sets the border again by hand. Only with the objects'
// lifetimes tracked (see above); otherwise every window is re-evaluated,
// at most a few times a second. The caller holds no lock.
ULONGLONG g_lastGlobalRefresh;
void RefreshWindow(const BorderRecord& record, const Decision& d) {
    if (!g_objectLifetimesTracked) {
        ULONGLONG now = GetTickCount64();
        if (now - g_lastGlobalRefresh >= 300) {
            g_lastGlobalRefresh = now;
            RequestDwmRefresh(L"decoration");
        }
        return;
    }
    if (UpdateWindowVisuals && record.window) {
        UpdateWindowVisuals(record.window);
        return;
    }
    BorderColor color = d.colorKnown && record.color.a <= 0.0f ? d.color : record.color;
    SetBorderParameters_Original(record.border, &record.rect, record.radius, record.dpi, &color,
                                 record.borderStyle, d.shadowStyle);
}

// The timer's pass: every recorded window whose decision changed since the
// mod last saw its border gets a visual update. Records of windows gone, or
// no longer maximized, are dropped.
// A monitor on which a window's shadow just changed by DWM's own update
// (an activation): the other windows there are brought up to date at the
// very next timer tick, without the two-pass hold, so the old and the new
// holder don't both cast a shadow for a tenth of a second.
std::atomic<HMONITOR> g_urgentMonitor{nullptr};
std::atomic<HWND> g_lastForeground{nullptr};  // last window set as the foreground one
std::atomic<HMONITOR> g_urgentMonitor2{nullptr};  // a window moved: both monitors
void DecorPass(HMONITOR urgent) {
    ULONGLONG now = GetTickCount64();
    struct Refresh {
        HWND hwnd;
        BorderRecord record;
        Decision decision;
    };
    std::vector<Refresh> refreshes;
    {
        std::lock_guard<std::mutex> lock(g_borderStateMutex);
        for (auto it = g_borderRecords.begin(); it != g_borderRecords.end();) {
            HWND hwnd = it->first;
            BorderRecord& record = it->second;
            if (!IsWindow(hwnd)) {
                it = g_borderRecords.erase(it);
                continue;
            }
            ++it;
            if (!RecordIsCurrent(hwnd, record)) {
                continue;
            }
            HMONITOR here = MonitorOf(hwnd);
            bool rushed = urgent && (here == urgent || here == g_urgentMonitor2.load());
            if (now - record.refreshed < (rushed ? 40u : 300u)) {
                continue;
            }
            Decision d = Decide(hwnd);
            bool shadowDiffers = d.shadowStyle != record.shadowStyle;
            // The color is missing (not known yet when DWM set the border)
            // or, with custom colors, the other activation state's.
            bool colorMissing = d.colorKnown && !record.colorKnown;
            bool colorDiffers = colorMissing || (d.colorKnown && record.colorKnown &&
                                                 !SameColor(d.color, record.color));
            if (!shadowDiffers && !colorDiffers) {
                record.pendingCount = 0;
                continue;
            }
            if (record.pendingCount > 0 && record.pendingShadow == d.shadowStyle &&
                record.pendingColor == colorDiffers) {
                record.pendingCount++;
            } else {
                record.pendingShadow = d.shadowStyle;
                record.pendingColor = colorDiffers;
                record.pendingCount = 1;
            }
            if (record.pendingCount < 2 && !rushed) {
                continue;  // seen once: wait for it to hold
            }
            record.refreshed = now;
            record.pendingCount = 0;
            // What the hooks will decide; so a second pass before DWM gets
            // to it doesn't ask again. A border set by hand doesn't pass
            // through the hook, so the record carries the new values itself.
            if (shadowDiffers) {
                // The shadow handover, the one thing worth a line here.
                Wh_Log(L"[dwm] window %p (%s): shadow style %d -> %d (%s)%s", hwnd,
                       d.active ? L"active" : L"inactive", record.shadowStyle, d.shadowStyle,
                       ShadowShown(d.shadowStyle) ? L"shown" : L"none",
                       rushed ? L", rushed" : L"");
            }
            record.shadowStyle = d.shadowStyle;
            record.colorKnown = d.colorKnown;
            if (d.colorKnown) {
                record.color = d.color;
            }
            record.active = d.active;
            refreshes.push_back({hwnd, record, d});
        }
    }
    for (const Refresh& r : refreshes) {
        // One visual update can lead DWM to destroy another window's
        // objects; the destructor hooks drop that record from the map, not
        // from this copy. So each record is looked up again right before
        // its call and skipped if it is gone or now holds other objects.
        {
            std::lock_guard<std::mutex> lock(g_borderStateMutex);
            auto it = g_borderRecords.find(r.hwnd);
            if (it == g_borderRecords.end() || it->second.window != r.record.window ||
                it->second.border != r.record.border) {
                continue;
            }
        }
        RefreshWindow(r.record, r.decision);
    }
}

// The timer on DWM's UI thread: a timer of the "Dwm" window, which that
// thread owns and dispatches messages for (a thread timer never fired -
// DWM's loop doesn't dispatch thread messages). DispatchMessage calls the
// procedure directly, on the thread the hooks run on. Started from the
// first border call, killed before the mod unloads: the timer must not
// outlive its procedure.
constexpr UINT_PTR kDecorTimerId = 0x52455645;  // 'REVE'
constexpr UINT kDecorTickMs = 100;
std::atomic<HWND> g_decorTimerWindow{nullptr};
std::atomic<bool> g_decorTimerStop{false};  // unloading: never armed again
// Every SetTimer / KillTimer of the decoration timer happens under this
// lock, with the stop flag checked under it, so the stop from the unload
// path can't be undone by a re-arm racing it.
std::mutex g_decorTimerMutex;
std::atomic<int> g_decorTicksRunning{0};
void CALLBACK DecorTimerProc(HWND hwnd, UINT, UINT_PTR id, DWORD);

// Arms (or re-arms) the timer, unless the mod is on its way out.
bool ArmDecorTimer(HWND dwmWnd, UINT ms) {
    std::lock_guard<std::mutex> lock(g_decorTimerMutex);
    return !g_decorTimerStop && SetTimer(dwmWnd, kDecorTimerId, ms, DecorTimerProc) != 0;
}
bool DecorationWanted() {
    return g_enabled && (g_maximizedShadow || g_maximizedBorder || g_customBorderColors);
}
// The accent color, re-read once a second while a border color follows it.
void FollowAccentColor() {
    static int ticks = 0;
    if (!g_customBorderColors || !(g_activeBorderIsAccent || g_inactiveBorderIsAccent) ||
        ++ticks % 10 != 0) {
        return;
    }
    uint32_t accent = 0;
    if (!AccentColor(accent)) {
        return;
    }
    bool changed = false;
    if (g_activeBorderIsAccent && g_activeBorderArgb.exchange(accent) != accent) {
        changed = true;
    }
    if (g_inactiveBorderIsAccent && g_inactiveBorderArgb.exchange(accent) != accent) {
        changed = true;
    }
    if (changed) {
        RequestDwmRefresh(L"accent color");  // floating windows too
    }
}
void CALLBACK DecorTimerProc(HWND hwnd, UINT, UINT_PTR id, DWORD) {
    struct Running {
        Running() { g_decorTicksRunning++; }
        ~Running() { g_decorTicksRunning--; }
    } running;
    if (g_decorTimerStop) {
        KillTimer(hwnd, id);  // the stop did this too; a tick already queued ends here
        return;
    }
    if (!DecorationWanted()) {
        // Every option off: the timer goes until an option is turned on
        // again (the border hook starts it).
        std::lock_guard<std::mutex> lock(g_decorTimerMutex);
        if (g_decorTimerWindow.exchange(nullptr) == hwnd) {
            KillTimer(hwnd, id);
        }
        return;
    }
    HMONITOR urgent = g_urgentMonitor.exchange(nullptr);
    if (urgent) {
        ArmDecorTimer(hwnd, kDecorTickMs);  // back to the regular pace
    }
    FollowAccentColor();
    DecorPass(urgent);
    g_urgentMonitor2 = nullptr;
}

// The next tick as soon as possible, for `monitor` (and `other`, when a
// window just moved from there).
void RushDecorPass(HMONITOR monitor, HMONITOR other) {
    HWND dwmWnd = g_decorTimerWindow.load();
    if (!dwmWnd || !monitor) {
        return;
    }
    g_urgentMonitor2 = other;
    g_urgentMonitor = monitor;
    ArmDecorTimer(dwmWnd, USER_TIMER_MINIMUM);
}
void EnsureDecorTimer() {
    if (g_decorTimerWindow.load() || g_decorTimerStop) {
        return;
    }
    HWND dwmWnd = FindWindowW(L"Dwm", nullptr);
    if (!dwmWnd) {
        return;  // tried again at the next border call
    }
    std::lock_guard<std::mutex> lock(g_decorTimerMutex);
    if (g_decorTimerStop || g_decorTimerWindow.load()) {
        return;
    }
    if (!SetTimer(dwmWnd, kDecorTimerId, kDecorTickMs, DecorTimerProc)) {
        Wh_Log(L"[dwm] decoration timer not started: %u", GetLastError());
        g_decorTimerStop = true;  // not again
        return;
    }
    g_decorTimerWindow = dwmWnd;
}
// Stops the timer for good, before the mod unloads: the stop flag goes up
// under the arming lock, so no re-arm can follow, the timer is killed
// (KillTimer takes a window timer from any thread), and a tick in progress
// - or one retrieved by DWM's loop just before the kill - is given a moment
// to finish. Nothing is run on DWM's thread for this: a thread hook or a
// sent message would be one more piece of this image for that thread to
// be calling when the image goes.
void StopDecorTimer() {
    {
        std::lock_guard<std::mutex> lock(g_decorTimerMutex);
        g_decorTimerStop = true;
        if (HWND dwmWnd = g_decorTimerWindow.exchange(nullptr)) {
            if (!KillTimer(dwmWnd, kDecorTimerId)) {
                Wh_Log(L"[dwm] KillTimer failed: %u", GetLastError());
            }
        }
    }
    Sleep(50);  // a WM_TIMER already retrieved reaches the procedure within this
    for (int i = 0; i < 200 && g_decorTicksRunning > 0; i++) {
        Sleep(10);
    }
}

HRESULT WINAPI SetBorderParameters_Hook(void* self, const RECT* rect, float radius, int dpi,
                                        const void* color, int borderStyle, int shadowStyle) {
    LastRadiusQuery query = g_lastRadiusQuery;
    g_lastRadiusQuery.valid = false;  // one query, one border update
    {
        std::lock_guard<std::mutex> lock(g_borderStateMutex);
        if (query.valid) {
            if (g_borderStates.size() > 4096) {
                g_borderStates.clear();  // borders of windows long gone
            }
            g_borderStates[self] = query;
        } else {
            auto it = g_borderStates.find(self);
            if (it != g_borderStates.end()) {
                query = it->second;
            }
        }
    }
    HWND hwnd = query.hwnd;
    // Squared: maximized, snapped or full-screen, as the radius query DWM
    // made for this update says. DWM doesn't always ask again before a
    // border update, though, and the query kept for the border object is
    // then the last one asked - the window's floating one, after a restore -
    // so a window found maximized is squared all the same (were it passed
    // through as floating, DWM's own "no border" would stick).
    bool zoomedNow = query.valid && query.eligible && !query.squared && hwnd && IsWindow(hwnd) &&
                     IsZoomed(hwnd);
    bool squared = query.valid && (query.squared || zoomedNow);
    if (DecorationWanted()) {
        EnsureDecorTimer();
    }
    if (!color) {
        return SetBorderParameters_Original(self, rect, radius, dpi, color, borderStyle,
                                            shadowStyle);
    }
    BorderColor used = *static_cast<const BorderColor*>(color);
    bool known = hwnd && IsWindow(hwnd);
    if (!squared) {
        bool ordinary = known && IsOrdinaryFrame(hwnd);
        if (ordinary && used.a > 0.0f) {
            bool active = IsForegroundWindow(hwnd);
            // Style 4 is the transitional style (a window being minimized
            // is still visible, not yet iconic, for its first update): never
            // learned, or the shadow holder would be given it until a
            // floating window at rest taught otherwise.
            if (shadowStyle > 0 && shadowStyle != kTransitionalShadowStyle) {
                (active ? g_activeShadowStyles : g_inactiveShadowStyles).See(shadowStyle, hwnd);
            }
            // The accent is never taught where Windows' own option wouldn't
            // draw it: as the inactive color (a window that just lost the
            // foreground), nor at all with the option off (an app that
            // colors its own border the accent) - so with the option off,
            // maximized windows get the gray floating windows have.
            BorderColor accentBorder{};
            bool accentOn = false;
            bool accentSeen = WindowsBorderAccent(accentBorder, accentOn) &&
                              SameRgb(used, accentBorder) && (!active || !accentOn);
            if (!accentSeen) {
                (active ? g_activeBorderColors : g_inactiveBorderColors).See(used, hwnd);
            }
        }
        // Custom colors replace DWM's for app frames that have a border (a
        // window that hid its own border, alpha 0, keeps it hidden).
        if (g_enabled && g_customBorderColors && used.a > 0.0f && known && IsAppFrame(hwnd) &&
            OnFramedMonitor(hwnd)) {
            used = CustomBorderColor(IsForegroundWindow(hwnd));
        }
        // A window that was maximized a moment ago has been restored: the
        // monitor it leaves behind may need a new shadow holder now, and the
        // record goes, so that maximizing it again counts as a change.
        if (known && g_enabled) {
            HMONITOR left = nullptr;
            {
                std::lock_guard<std::mutex> lock(g_borderStateMutex);
                auto it = g_borderRecords.find(hwnd);
                if (it != g_borderRecords.end()) {
                    left = it->second.monitor;
                    g_borderRecords.erase(it);
                }
            }
            if (left && g_maximizedShadow) {
                RushDecorPass(left, nullptr);
            }
        }
        return SetBorderParameters_Original(self, rect, radius, dpi, &used, borderStyle,
                                            shadowStyle);
    }
    if (!g_enabled) {
        return SetBorderParameters_Original(self, rect, radius, dpi, &used, borderStyle,
                                            shadowStyle);
    }
    Decision d = Decide(hwnd);
    // Maximized: DWM hands over alpha 0; snapped: DWM's own color, which the
    // custom colors replace as for floating windows.
    if (d.colorKnown && (used.a <= 0.0f || g_customBorderColors)) {
        used = d.color;
    }
    // The border follows the corners the window was given: square with the
    // rounding off (DWM hands the border its style's radius even then,
    // which never showed while the border was invisible), the one radius
    // with it on.
    // (A window found maximized under a floating query gets the radius it
    // would have been given maximized: the one radius, or square with
    // Windows' own corners.)
    radius = zoomedNow ? (g_radius.load() >= 0 ? static_cast<float>(g_radius.load()) : 0.0f)
                       : query.radius;
    if (shadowStyle == 0 && d.shadowStyle > 0) {
        shadowStyle = d.shadowStyle;
    }
    HRESULT hr =
        SetBorderParameters_Original(self, rect, radius, dpi, &used, borderStyle, shadowStyle);
    if (known && rect) {
        HMONITOR monitor = MonitorOf(hwnd);
        BorderRecord record{query.window, self,        *rect,       radius, dpi, used,
                            borderStyle,  shadowStyle, d.active, d.colorKnown, monitor, 0,
                            {},           0,           -1,       false,        0};
        WindowIdentity(hwnd, record.thread, record.className);
        // DWM's own update changed where this window casts its shadow (an
        // activation, a move to another monitor): the other windows on the
        // monitor(s) concerned are brought up to date at the next tick.
        bool rush = false;
        HMONITOR before = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_borderStateMutex);
            if (g_borderRecords.size() > 4096) {
                g_borderRecords.clear();
            }
            auto it = g_borderRecords.find(hwnd);
            if (it != g_borderRecords.end()) {
                record.refreshed = it->second.refreshed;
                rush = ShadowShown(it->second.shadowStyle) != ShadowShown(shadowStyle) ||
                       it->second.monitor != monitor;
                before = it->second.monitor != monitor ? it->second.monitor : nullptr;
            } else {
                rush = ShadowShown(shadowStyle);
            }
            g_borderRecords[hwnd] = record;
        }
        // The foreground moved here from another monitor: the shadow holder
        // there turns inactive now, not at the next regular tick.
        if (d.active) {
            HWND previous = g_lastForeground.exchange(hwnd);
            if (previous && previous != hwnd && IsWindow(previous)) {
                HMONITOR previousMonitor = MonitorOf(previous);
                if (previousMonitor != monitor) {
                    rush = true;
                    if (!before) {
                        before = previousMonitor;
                    }
                }
            }
        }
        if (rush && g_maximizedShadow) {
            RushDecorPass(monitor, before);
        }
    }
    return hr;
}

// DWM rebuilds a window's corner radius and clip only when something it
// tracks changes. A system color change makes it re-evaluate every top-level
// window, without touching any window's own corner preference (which
// DwmGetWindowAttribute can't read back, so it couldn't be restored).
void RequestDwmRefresh(const wchar_t* why) {
    HWND dwmWnd = FindWindowW(L"Dwm", nullptr);
    if (!dwmWnd) {
        Wh_Log(L"[dwm] refresh (%s): no Dwm window; corners update as windows change", why);
        return;
    }
    PostMessageW(dwmWnd, WM_SYSCOLORCHANGE, 0, 0);
    Wh_Log(L"[dwm] refresh (%s) requested", why);
}


// Two or more Dwminit warnings in the Application log within the last
// minute mean DWM has just been crashing and restarting; a mod that keeps
// hooking it could be the reason, so the dwm.exe part stays out (as Custom
// Window Corner Radius does) until the next sign-in or mod reload.
bool DwmRestartedRecently() {
    HANDLE log = OpenEventLogW(nullptr, L"Application");
    if (!log) {
        return false;
    }
    std::vector<BYTE> buffer(64 * 1024);
    DWORD read = 0, needed = 0;
    int warnings = 0;
    bool done = false;
    DWORD now = static_cast<DWORD>(std::time(nullptr));
    while (!done && ReadEventLogW(log, EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ, 0,
                                  buffer.data(), static_cast<DWORD>(buffer.size()), &read,
                                  &needed)) {
        BYTE* p = buffer.data();
        while (p + sizeof(EVENTLOGRECORD) <= buffer.data() + read) {
            auto* record = reinterpret_cast<EVENTLOGRECORD*>(p);
            if (record->Length < sizeof(EVENTLOGRECORD) || now - record->TimeGenerated > 60) {
                done = true;
                break;
            }
            if (record->EventType == EVENTLOG_WARNING_TYPE) {
                PCWSTR source = reinterpret_cast<PCWSTR>(p + sizeof(EVENTLOGRECORD));
                if (_wcsicmp(source, L"Dwminit") == 0 && ++warnings >= 2) {
                    done = true;
                    break;
                }
            }
            p += record->Length;
        }
    }
    CloseEventLog(log);
    return warnings >= 2;
}

BOOL Init() {
    LoadDwmSettings();
    if (DwmRestartedRecently()) {
        Wh_Log(L"[dwm] DWM restarted twice in the last minute - staying out of dwm.exe");
        return FALSE;
    }

    HMODULE udwm = GetModuleHandleW(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"[dwm] udwm.dll not loaded");
        return FALSE;
    }

    // Both access specifiers are listed: older Windows 11 builds export
    // these members as private, newer ones as public.
    WindhawkUtils::SYMBOL_HOOK udwmDllHooks[] = {
        {
            {LR"(private: float __cdecl CTopLevelWindow::GetRadiusFromCornerStyle(void))",
             LR"(public: float __cdecl CTopLevelWindow::GetRadiusFromCornerStyle(void))"},
            &GetRadiusFromCornerStyle,  // address only, no hook
        },
        {
            {LR"(public: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))",
             LR"(private: float __cdecl CTopLevelWindow::GetFloatCornerRadiusForCurrentStyle(void))"},
            &GetFloatCornerRadiusForCurrentStyle_Original,
            GetFloatCornerRadiusForCurrentStyle_Hook,
        },
        {
            {LR"(public: long __cdecl CWindowBorder::SetBorderParameters(struct tagRECT const &,float,int,struct _D3DCOLORVALUE const &,enum CWindowBorder::BorderStyle,enum CWindowBorder::ShadowStyle))"},
            &SetBorderParameters_Original,
            SetBorderParameters_Hook,
            true,  // optional: without it maximized windows just stay undressed
        },
        {
            {LR"(public: class CWindowData * __cdecl CTopLevelWindow::GetWindowData(void)const )"},
            &GetWindowData,  // address only
            nullptr,
            true,
        },
        {
            {LR"(private: long __cdecl CTopLevelWindow::UpdateWindowVisuals(void))",
             LR"(public: long __cdecl CTopLevelWindow::UpdateWindowVisuals(void))"},
            &UpdateWindowVisuals,  // address only
            nullptr,
            true,
        },
        {
            {LR"(public: bool __cdecl CWindowData::IsGhostWindow(struct HWND__ * *)const )"},
            &IsGhostWindowCode,  // address only, read for the handle's offset
            nullptr,
            true,
        },
        // The clip of a window while it animates (see AnimatedClipRadius).
        {
            {LR"(private: long __cdecl CTopLevelWindow3D::UpdateAnimatedResources(void))",
             LR"(public: long __cdecl CTopLevelWindow3D::UpdateAnimatedResources(void))"},
            &UpdateAnimatedResources_Original,
            UpdateAnimatedResources_Hook,
            true,  // optional: without it windows animate with Windows' corners
        },
        {
            {LR"(public: static long __cdecl ResourceHelper::CreateRectangleGeometry(struct D2D_POINTANDSIZE_L const &,float,class CRectangleGeometryProxy * *))"},
            &CreateRectangleGeometry_Original,
            CreateRectangleGeometry_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl CTopLevelWindow3D::SetWindowData(class CWindowData *))"},
            &SetWindowData3DCode,  // address only, read for the window data's offset
            nullptr,
            true,
        },
        {
            {LR"(public: long __cdecl CRectangleGeometryProxy::SetRectangle(float,float,float,float,float,float,float,float,float,float,float,float,bool))"},
            &SetRectangle_Original,
            SetRectangle_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl CWindowBorder::SetClipRectangle(class CRectangleGeometryProxy *,struct tagRECT const &))",
             LR"(public: void __cdecl CWindowBorder::SetClipRectangle(class CRectangleGeometryProxy *,struct tagRECT const &))"},
            &SetClipRectangle_Original,
            SetClipRectangle_Hook,
            true,  // optional: without it a restore shows the window's corner stretched
        },
        // The destructors: records of objects DWM frees go with them (see
        // g_objectLifetimesTracked).
        {
            {LR"(protected: virtual __cdecl CTopLevelWindow::~CTopLevelWindow(void))",
             LR"(public: virtual __cdecl CTopLevelWindow::~CTopLevelWindow(void))",
             LR"(private: virtual __cdecl CTopLevelWindow::~CTopLevelWindow(void))"},
            &TopLevelWindowDestructor_Original,
            TopLevelWindowDestructor_Hook,
            true,
        },
        {
            {LR"(public: virtual __cdecl CWindowBorder::~CWindowBorder(void))",
             LR"(protected: virtual __cdecl CWindowBorder::~CWindowBorder(void))",
             LR"(private: virtual __cdecl CWindowBorder::~CWindowBorder(void))"},
            &WindowBorderDestructor_Original,
            WindowBorderDestructor_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(udwm, udwmDllHooks, ARRAYSIZE(udwmDllHooks))) {
        Wh_Log(L"[dwm] HookSymbols failed (udwm.dll)");
        return FALSE;
    }

    if (GetWindowData) {
        FindHwndOffset();
    }
    FindWindowData3DOffset();
    g_objectLifetimesTracked =
        TopLevelWindowDestructor_Original && WindowBorderDestructor_Original;
    Wh_Log(L"[dwm] hooks installed (maximized border and shadow: %s%s%s%s%s%s)",
           SetBorderParameters_Original ? L"available" : L"SetBorderParameters not found",
           SetBorderParameters_Original && g_hwndOffset < 0 ? L", for every maximized window"
                                                             : L"",
           UpdateWindowVisuals ? L"" : L"; UpdateWindowVisuals not found: borders set by hand",
           UpdateAnimatedResources_Original && CreateRectangleGeometry_Original &&
                   SetRectangle_Original && g_windowData3DOffset >= 0
               ? L""
               : L"; animated clip not found: windows animate with Windows' corners",
           SetClipRectangle_Original && g_objectLifetimesTracked
               ? L""
               : L"; content clip not tracked: a restore stretches the window's corner",
           g_objectLifetimesTracked
               ? L""
               : L"; destructors not found: changed decisions re-evaluate every window");
    return TRUE;
}

void AfterInit() {
    // Hooks are active from here on; have existing windows re-evaluated.
    RequestDwmRefresh(L"init");
}

void Uninit() {
    // Hooks are already removed: this re-evaluation restores the stock corners.
    RequestDwmRefresh(L"uninit");
}

void SettingsChanged() {
    LoadDwmSettings();
    RequestDwmRefresh(L"settings");
}

}  // namespace dwm

}  // namespace

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_host = DetectHostProcess();
    if (g_host == HostProcess::Dwm) {
        return dwm::Init();
    }

    // If the taskbar already exists and belongs to another explorer.exe
    // instance, stay out of this process.
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (tray) {
        DWORD pid = 0;
        GetWindowThreadProcessId(tray, &pid);
        if (pid != GetCurrentProcessId()) {
            Wh_Log(L"Not the taskbar process");
            return FALSE;
        }
    }

    g_module = GetCurrentModule();
    g_unloading = false;

    // Everything done inside the taskbar (border line, background color,
    // corner ears, showing the frame together with it) needs these symbols;
    // the frame itself works without them.
    g_taskbarXamlAvailable = HookTaskbarDllSymbols();
    if (!g_taskbarXamlAvailable) {
        Wh_Log(L"taskbar.dll symbols unavailable - the taskbar is left as it is");
    }
    HookLivePreview();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, ModThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_host == HostProcess::Dwm) {
        dwm::AfterInit();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit");
    g_unloading = true;  // no new taskbar-side changes from here on
    if (g_host == HostProcess::Dwm) {
        dwm::StopDecorTimer();
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_host == HostProcess::Dwm) {
        dwm::Uninit();
        return;
    }

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    if (g_host == HostProcess::Dwm) {
        dwm::SettingsChanged();
        return;
    }

    g_reloadRequested = true;
    HWND ctrl = g_ctrlWnd.load();
    if (ctrl) {
        PostMessageW(ctrl, kMsgReload, 0, 0);
    }
}

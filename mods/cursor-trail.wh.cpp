// ==WindhawkMod==
// @id              cursor-trail
// @name            Simple Cursor Trail
// @description     A fully customizable cursor trail overlay for the Windows desktop.
// @version         1.2
// @author          Ulrizza
// @github          https://github.com/Ulrizza
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -lole32 -lgdi32 -lshell32 -lwindowscodecs -lwinmm -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Cursor Trail

A fully customizable cursor trail for the Windows desktop.  

## Intro

I always liked the original Windows cursor trail, but it was too limited, so I made this one. I personally enjoy it very much so I want to share it 😊

If you see any bug or want more settings/features, add an issue on the [Github repo](https://github.com/Ulrizza/WindHawk-CursorTrail)!  
I cannot promise you anything because I don't have much free time but I enjoyed making this plugin so I'll do my best 🤘

**Disclaimer:** I used AI to make this project, I just wanted a custom cursor trail and now I have it so I'm happy. If you are bothered by that, just don't install it.

## Installation

1. Install [Windhawk](https://windhawk.net/).
2. Install **Simple Cursor Trail** from the Windhawk mods catalog, or import the
   source (`CursorTrail.cpp`) from this repository via the Windhawk mod editor.

## Styles

This mod is intentionally minimal: it just draws a trail — no particles,
physics, or extra effects.

- **Simple line**: a polyline that follows the cursor; its width, color, and
  opacity can change from head to tail.
- **Cursor ghost**: faded copies of the cursor image, each latched at the spot
  where it spawned (they stay put and only fade out); directly inspired by the
  classic Windows cursor-trail feature. Each copy keeps the exact cursor image
  from when it was sampled, so an image change (e.g. arrow to I-beam) appears
  gradually along the trail.

By contrast, [Mouse Trail](https://windhawk.net/mods/mouse-trail) follows the
cursor with special effects from a full D3D11 particle/physics engine, while
[Cursor Motion Blur](https://windhawk.net/mods/cursor-motion-blur) only smears
the cursor at high speed.

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

How long each trail segment stays visible, in milliseconds. Minimum 20, maximum 10000.

`100`  
![simpleLineOptions.timeBased.tail_duration = 100](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.timeBased.tail_duration.100.gif)

`300`  
![simpleLineOptions.timeBased.tail_duration = 300](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.timeBased.tail_duration.300.gif)

`1000`  
![simpleLineOptions.timeBased.tail_duration = 1000](https://raw.githubusercontent.com/Ulrizza/WindHawk-CursorTrail/main/images/simpleLineOptions.timeBased.tail_duration.1000.gif)

#### Size based

Applies when Trail mode is Size based.

##### Tail length

Maximum trail length in pixels. Minimum 20, maximum 10000.

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

How long each cursor copy stays visible, in milliseconds. Minimum 20, maximum 10000.

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

## Licence

This project is licensed under the MIT License. You're free to use, modify, and
redistribute the code as long as you keep the original copyright notice and
credit the author (Ulrizza). See [LICENSE](https://github.com/Ulrizza/WindHawk-CursorTrail/blob/main/LICENSE) for the full text.

Part of this mod is adapted from
[Cursor Motion Blur](https://windhawk.net/mods/cursor-motion-blur) by
[TheatriChris](https://github.com/chrisc44890), Copyright (c) TheatriChris
(MIT): the overlay-window scaffolding, the fullscreen-game detection, and
parts of the render loop.

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
      $description: How long each trail segment stays visible, in milliseconds. Minimum 20, maximum 10000.
    $name: Time based
    $description: Applies when Trail mode is Time based.
  - sizeBased:
    - tail_size: 2000
      $name: Tail length
      $description: Maximum trail length in pixels. Minimum 20, maximum 10000.
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
      $description: How long each cursor copy stays visible, in milliseconds. Minimum 20, maximum 10000.
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
    bool  isGhost = false;                            // active style is cursor_ghost
    int   ghostSpacing = 0;                           // extra px between ghost copies
    float ghostSpawnDist = 2.0f;                      // px cursor must travel before a new ghost is stamped
    std::vector<float> ghostSizes;                    // per-copy size multipliers, head → tail
    float ghostSizeMax = 1.0f;                        // largest size multiplier (min 1), for the bbox
    bool  ghostTintActive = false;                    // ghost color list is non-empty (recolor copies)
    std::vector<Rgb> ghostTints;                      // precomputed tint per baked variant (head → tail)
    GhostReplaceMode ghostReplaceMode = GHOST_REPLACE_AUTO;  // which pixels to recolor
    Rgb   ghostReplaceColor = { 1.0f, 1.0f, 1.0f };   // Custom: original cursor color to swap for the tint

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
    POINT visualOffset = { 0, 0 };   // hotspot → visible-pixel center (scaled)
    float visCenterX = 0.0f, visCenterY = 0.0f;  // visible center in bitmap pixels
    bool  visibleValid = false;
    int   visLeft = 0, visTop = 0, visRight = 0, visBottom = 0;
};

// Cursor geometry cache. The visual offset is shared with the poll thread via
// offsetMutex; the cursor dimensions and geomCache below are render-thread-only.
struct CursorState {
    HCURSOR cachedCursor = NULL;
    POINT   visualOffset = { 0, 0 };  // hotspot → visible-pixel center (trail origin)
    std::mutex offsetMutex;                 // protects visualOffset (read by poll thread)

    int   bmWidth = 0, bmHeight = 0;        // bitmap dims + DPI scale (toggle effect + ghost bitmaps)
    float dpiScaleX = 1.0f, dpiScaleY = 1.0f;
    UINT  lastDpiX = 96, lastDpiY = 96;     // effective DPI the current geometry was built for

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
    HANDLE overlayReadyEvent = NULL;          // signalled once the overlay window exists
    std::atomic<bool> isGameRunning{false};   // set by poll thread, read by render and poll threads
    std::atomic<bool> renderScheduled{false}; // set by MMTimerCallback, cleared by overlay thread
    std::atomic<bool> trailEnabled{true};     // toggled by the enable/disable hotkey; read by both threads
    std::atomic<bool> overlayIdle{false};     // render timer stopped (set by overlay, read by poll)
    std::atomic<bool> idleWakePending{false}; // coalesces poll -> overlay kMsgIdleWake posts
    // Bumped by the poll thread whenever the trail content changes (sample
    // pushed/evicted, cursor image changed). The render thread only redraws
    // when this differs from renderedRevision, so a static trail doesn't keep
    // repainting an identical frame. Read/written under historyMutex.
    std::atomic<unsigned long long> contentRevision{0};
    MMRESULT mmTimerId = 0;
    DWORD   lastActiveTime = 0;               // overlay-thread-only; last frame with work
    bool    periodRaised = false;             // overlay-thread-only; timeBeginPeriod(1) state

    DWORD lastMovementTime = 0;               // poll-thread-owned
    POINT lastCursorPos = { 0, 0 };           // previous raw cursor position (movement tracking)
    bool  lastCursorValid = false;
    bool  isFading = false;
    POINT lastSamplePos = { 0, 0 };           // poll-thread-owned; last pushed sample (canvas coords)
    bool  lastSampleValid = false;            // poll-thread-owned; whether lastSamplePos is set

    RECT  prevDirtyRect = { 0, 0, 0, 0 };     // render-thread-only frame state
    bool  hasPrevDirty = false;
    bool  needsClear = false;
    bool  needsFullClear = false;             // backbuffer just (re)created; clear the whole thing once
    unsigned long long renderedRevision = 0;  // render-thread-only; last content revision drawn
    unsigned renderedSettingsVersion = 0;     // render-thread-only; last settings version drawn
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

// Idle handling: the render timer and the 1 ms timer resolution are only kept
// alive while there is something to draw. See EnterIdleIfInactive/ResumeRenderTimer.
static const int   kRenderIntervalMs  = 8;   // ~125 Hz render timer
static const DWORD kIdleGraceMs       = 200; // inactivity before the render timer stops
static const DWORD kIdlePollIntervalMs = 20; // poll interval while the overlay is idle
static const DWORD kSampleIntervalMs  = 1;   // cursor poll interval while active
static const DWORD kDisabledPollIntervalMs = 250; // poll interval while the trail is off
static const DWORD kGamePollIntervalMs = 1000; // poll interval while a fullscreen game suppresses the trail

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
static const UINT kMsgIdleWake = WM_APP + 2;  // poll thread -> overlay: resume rendering
static const UINT kMsgApplySettings = WM_APP + 3;  // main thread -> overlay: reload settings

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

// Bounds for the trail-length settings. The result caps TrailPointBudget and
// the time-based history, so an out-of-range value can't over-allocate (or make
// reserve throw) every frame.
static const int kMinTailDuration = 20;
static const int kMaxTailDuration = 10000;   // 10 s
static const int kMinLineTailSize = 20;
static const int kMaxLineTailSize = 10000;   // px

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
    int sizeTimeout = Wh_GetIntSetting(key(L"sizeBased.timeout").c_str());
    if (sizeTimeout < 0) sizeTimeout = 0;   // clamp before the unsigned conversion
    settings.sizeTimeout  = (DWORD)sizeTimeout;

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

    // Unknown style values fall back to simple_line (isGhost stays false).
    std::wstring style = ReadStringSetting(L"style", L"simple_line");
    settings.isGhost = (style == L"cursor_ghost");

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

    if (settings.tailDuration < kMinTailDuration) settings.tailDuration = kMinTailDuration;
    if (settings.tailDuration > kMaxTailDuration) settings.tailDuration = kMaxTailDuration;
    if (settings.isGhost) {
        // tail_size is a copy count for the ghost style.
        if (settings.tailSize < 2) settings.tailSize = 2;
        if (settings.tailSize > 512) settings.tailSize = 512;
    } else {
        if (settings.tailSize < kMinLineTailSize) settings.tailSize = kMinLineTailSize;
        if (settings.tailSize > kMaxLineTailSize) settings.tailSize = kMaxLineTailSize;
    }

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

// Returns the effective per-monitor DPI of the monitor under the pointer
// (default 96 when it can't be determined). Used both to size the cursor and to
// detect when the geometry cache must be rebuilt after a DPI change.
static void GetCursorDpi(UINT& dpiX, UINT& dpiY) {
    dpiX = 96; dpiY = 96;
    POINT pt;
    if (GetCursorPos(&pt)) {
        HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        if (hMon) {
            GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        }
    }
}

// Returns the true on-screen size (physical pixels) of the cursor on the monitor
// under the pointer. The Windows cursor-size setting is stored in the registry
// as a DPI-independent base size (HKCU\Control Panel\Cursors\CursorBaseSize,
// default 32); the system scales it by the monitor DPI. SM_CXCURSOR/SM_CYCURSOR
// only report the nominal default and ignore the setting, so they are used only
// as a fallback.
static void GetActualCursorSize(int& cx, int& cy) {
    UINT dpiX = 96, dpiY = 96;
    GetCursorDpi(dpiX, dpiY);

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

// Single shared WIC imaging factory (render-thread-only). Created lazily on
// first use and released when the overlay thread tears down; creating one per
// call (as ComputeVisibleBounds / EnsureCursorBitmap used to) is wasteful.
static IWICImagingFactory* g_pWicFactory = nullptr;

static IWICImagingFactory* GetWicFactory() {
    if (!g_pWicFactory) {
        CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(&g_pWicFactory));
    }
    return g_pWicFactory;
}

// Computes the visible (non-transparent) pixel bounds of a cursor HBITMAP by
// scanning its alpha channel (threshold 8 ignores faint anti-aliased edges).
// Bounds are in bitmap pixel coordinates (right/bottom exclusive). Uses WIC
// only, so it doesn't require a Direct2D render target.
static void ComputeVisibleBounds(HBITMAP hbm, int& left, int& top, int& right, int& bottom, bool& valid) {
    left = top = right = bottom = 0;
    valid = false;

    IWICImagingFactory* pWicFactory = GetWicFactory();
    if (!pWicFactory) return;

    IWICBitmap* pWicBitmap = nullptr;
    HRESULT hr = pWicFactory->CreateBitmapFromHBITMAP(hbm, NULL,
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
// visual offset is also published in the scalar cursor.* fields for the poll
// thread (trail origin); the full per-cursor geometry lives in cursor.geomCache
// so the ghost style can place older cursor images too.
void UpdateCursorCenterOffset() {
    CURSORINFO ci = { sizeof(CURSORINFO) };
    if (!GetCursorInfo(&ci) || !(ci.flags & CURSOR_SHOWING) || !ci.hCursor) {
        std::lock_guard<std::mutex> lock(cursor.offsetMutex);
        cursor.visualOffset = { 0, 0 };
        cursor.bmWidth = 0;
        cursor.bmHeight = 0;
        cursor.cachedCursor = NULL;
        return;
    }

    UINT dpiX = 96, dpiY = 96;
    GetCursorDpi(dpiX, dpiY);

    {
        std::lock_guard<std::mutex> lock(cursor.offsetMutex);
        if (ci.hCursor == cursor.cachedCursor &&
            dpiX == cursor.lastDpiX && dpiY == cursor.lastDpiY) {
            return;  // same cursor and DPI, reuse cached geometry
        }
    }

    // Compute the geometry WITHOUT holding offsetMutex: this does a WIC
    // factory call, a pixel scan, and registry/DPI reads, and the poll thread
    // takes the same lock briefly every sample just to read visualOffset.
    CursorGeom g;
    if (!ComputeCursorGeom(ci.hCursor, g)) {
        std::lock_guard<std::mutex> lock(cursor.offsetMutex);
        cursor.visualOffset = { 0, 0 };
        cursor.bmWidth = 0;
        cursor.bmHeight = 0;
        cursor.cachedCursor = NULL;
        return;
    }

    std::lock_guard<std::mutex> lock(cursor.offsetMutex);
    cursor.geomCache[ci.hCursor] = g;

    cursor.visualOffset = g.visualOffset;
    cursor.bmWidth = g.bmWidth;
    cursor.bmHeight = g.bmHeight;
    cursor.dpiScaleX = g.dpiScaleX;
    cursor.dpiScaleY = g.dpiScaleY;
    cursor.lastDpiX = dpiX;
    cursor.lastDpiY = dpiY;

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

    IWICImagingFactory* pWicFactory = GetWicFactory();
    if (pWicFactory) {
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
            HRESULT hr = pWicFactory->CreateBitmapFromMemory(
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

    // Look up the desktop handles fresh each call (this runs at most once per
    // 500 ms) instead of caching them, so they don't go stale when Explorer
    // restarts and recreates the Progman/WorkerW windows.
    HWND hwndProgman = FindWindowW(L"Progman", NULL);
    HWND hwndWorkerW = FindWindowW(L"WorkerW", NULL);

    if (hwnd == hwndProgman || hwnd == hwndWorkerW) {
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

// Forward declaration: defined with the idle handling below.
static void RequestOverlayWake();

// Records that the trail content changed since the last rendered frame: bumps
// the revision so the render thread redraws, and, if the overlay has gone idle,
// wakes it. Call with runtime.historyMutex held (the poll thread mutates the
// history under that lock); the render thread re-checks the revision under the
// same lock before idling, so a change can never be stranded.
static void MarkContentChanged() {
    runtime.contentRevision.fetch_add(1, std::memory_order_relaxed);
    if (runtime.overlayIdle.load()) {
        RequestOverlayWake();
    }
}

// Time-based eviction: drop samples older than settings.tailDuration, then cap the
// total count. Caller must hold runtime.historyMutex.
static void EvictByTime(DWORD now) {
    bool changed = false;
    while (!runtime.history.empty() && (now - runtime.history.back().t) > (DWORD)settings.tailDuration) {
        runtime.history.pop_back();
        changed = true;
    }
    const size_t kMaxSamples = (size_t)(settings.tailDuration);
    while (runtime.history.size() > kMaxSamples) {
        runtime.history.pop_back();
        changed = true;
    }
    if (changed) MarkContentChanged();
}

// Forward declaration: MMTimerCallback is defined later, before OverlayThreadProc.
void CALLBACK MMTimerCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

// --- Idle handling ---------------------------------------------------------
// The render timer and the 1 ms timer resolution are only needed while there is
// something to draw. When the trail has fully faded, no toggle effect is playing
// and the cursor is still, the overlay thread stops the multimedia timer,
// releases the timer resolution, and hides the overlay window; the poll thread
// then samples at a much slower rate. Cursor movement (or a hotkey / settings
// change) wakes the overlay and shows the window again.

// Re-arms the render timer, the 1 ms timer resolution, and the overlay window.
// Safe to call when already active. Overlay-thread-only.
static void ResumeRenderTimer() {
    runtime.idleWakePending.store(false);
    runtime.overlayIdle.store(false);
    if (runtime.overlayHwnd) {
        ShowWindow(runtime.overlayHwnd, SW_SHOWNA);
    }
    if (!runtime.periodRaised) {
        timeBeginPeriod(1);
        runtime.periodRaised = true;
    }
    if (!runtime.mmTimerId) {
        runtime.mmTimerId = timeSetEvent(kRenderIntervalMs, 1, MMTimerCallback, 0, TIME_PERIODIC);
    }
}

// Called by the overlay thread after a frame. idleAllowed is false when the
// frame just drew something (so there is no point idling). If the overlay has
// been inactive for kIdleGraceMs and no content change is pending, stops the
// render timer, releases the timer resolution, and hides the overlay window.
// Overlay-thread-only.
static void EnterIdleIfInactive(DWORD now, bool idleAllowed) {
    if (!idleAllowed) {
        return;
    }
    if (runtime.overlayIdle.load()) {
        return;
    }
    if (now - runtime.lastActiveTime < kIdleGraceMs) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(runtime.historyMutex);
        // A content change may have arrived since the frame was rendered (the
        // poll thread bumps the revision and wakes under this same lock); stay
        // awake so that change gets drawn instead of being stranded.
        if (runtime.contentRevision.load(std::memory_order_relaxed) !=
            runtime.renderedRevision) {
            return;
        }
        // Set the flag while holding the lock: the poll thread bumps the
        // revision under the same lock and re-checks the flag, so a change
        // arriving right now is guaranteed to wake us again.
        runtime.overlayIdle.store(true);
    }

    if (runtime.mmTimerId) {
        timeKillEvent(runtime.mmTimerId);
        runtime.mmTimerId = 0;
    }
    if (runtime.periodRaised) {
        timeEndPeriod(1);
        runtime.periodRaised = false;
    }
    // Only hide the overlay when it shows nothing; a static trail must stay
    // visible while the render timer is stopped.
    if (runtime.overlayHwnd && !runtime.hasPrevDirty) {
        ShowWindow(runtime.overlayHwnd, SW_HIDE);
    }
}

// Called by the poll thread when the cursor has moved. Coalesces wake requests
// so at most one kMsgIdleWake is queued at a time.
static void RequestOverlayWake() {
    if (runtime.overlayHwnd && !runtime.idleWakePending.exchange(true)) {
        if (!PostMessage(runtime.overlayHwnd, kMsgIdleWake, 0, 0)) {
            runtime.idleWakePending.store(false);
        }
    }
}

// High-frequency cursor polling thread.
// Runs at kSampleIntervalMs (1 ms) intervals while active, pushing sampled
// positions into runtime.history. It also runs the fullscreen-game check (the
// render timer stops while idle, so this thread must keep watching). All D2D
// operations remain on the overlay/render thread — this thread only touches
// runtime.history (under mutex), GetCursorPos, IsGameRunning, and the atomic
// flags.
DWORD WINAPI PollThreadProc(LPVOID) {
    // Last time the fullscreen-game check ran (poll-thread-owned).
    DWORD lastGameCheck = GetTickCount();

    // Wait on the stop event to drive the loop. Sampling runs at
    // kSampleIntervalMs (1 ms) while the overlay is active, and slower while the
    // overlay is idle (kIdlePollIntervalMs). When there is nothing to sample —
    // the trail is off via the hotkey, or a fullscreen game suppresses it — poll
    // much slower; the game check below then runs once per wake, so a running
    // game is re-checked at kGamePollIntervalMs. The hotkey still wakes the
    // overlay directly.
    for (;;) {
        DWORD waitMs;
        if (runtime.isGameRunning.load()) {
            waitMs = kGamePollIntervalMs;
        } else if (!runtime.trailEnabled.load()) {
            waitMs = kDisabledPollIntervalMs;
        } else if (runtime.overlayIdle.load()) {
            waitMs = kIdlePollIntervalMs;
        } else {
            waitMs = kSampleIntervalMs;
        }
        if (WaitForSingleObject(runtime.pollStopEvent, waitMs) != WAIT_TIMEOUT) {
            break;
        }

        // Detect fullscreen games here rather than on the render thread: the
        // render timer stops while the overlay is idle, but this thread keeps
        // polling, so a game starting (erase the trail) or ending (resume) is
        // still noticed. Publish the result and wake the overlay on a change.
        DWORD tick = GetTickCount();
        if (tick - lastGameCheck > 500) {
            lastGameCheck = tick;
            bool gameNow = IsGameRunning();
            if (runtime.isGameRunning.exchange(gameNow) != gameNow) {
                std::lock_guard<std::mutex> lock(runtime.historyMutex);
                MarkContentChanged();
            }
        }

        // Respect the enable/disable hotkey state and the game-running flag set
        // above. Drop the sample anchor so a fresh one is pushed when sampling
        // resumes.
        if (runtime.isGameRunning.load() || !runtime.trailEnabled.load()) {
            runtime.lastSampleValid = false;
            continue;
        }

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

            // === CURSOR HIDDEN — flush the trail at once ===
            // The OS hides the pointer (e.g. Windows' hide-while-typing). Stop
            // sampling and drop the whole trail immediately rather than letting
            // it fade: a lingering trail (and, in Cursor ghost, a copy of the
            // cursor image) at the pointer looks like the cursor never went away.
            if (!sampleCursor) {
                origin.lastCursorValid = false;
                runtime.lastSampleValid = false;
                runtime.isFading = false;
                if (!runtime.history.empty()) {
                    runtime.history.clear();
                    MarkContentChanged();
                }
                continue;
            }

            if (settings.sizeBased) {
                if (settings.sizeTimeout > 0 && runtime.lastMovementTime > 0 &&
                    now - runtime.lastMovementTime > settings.sizeTimeout) {
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
                        if (runtime.history.size() > (size_t)settings.tailSize) {
                            while (runtime.history.size() > (size_t)settings.tailSize)
                                runtime.history.pop_back();
                            MarkContentChanged();
                        }
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
                                MarkContentChanged();
                                break;
                            }
                        }
                    }
                }
            } else {
                EvictByTime(now);
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
            // Compare against the last pushed sample (not history.front()) so a
            // stationary cursor doesn't push a fresh sample every time the
            // history drains; that would keep the trail non-empty forever and
            // stop the overlay from ever going idle.
            if (runtime.lastSampleValid) {
                if (settings.isGhost) {
                    // Ghost copies are latched at their spawn position, so only
                    // stamp a new copy once the cursor has travelled far enough
                    // from the last one. This keeps existing copies fixed in
                    // place instead of sliding with the cursor.
                    float dx = (float)(newPt.x - runtime.lastSamplePos.x);
                    float dy = (float)(newPt.y - runtime.lastSamplePos.y);
                    if (dx * dx + dy * dy < settings.ghostSpawnDist * settings.ghostSpawnDist)
                        continue;
                } else if (runtime.lastSamplePos.x == newPt.x &&
                           runtime.lastSamplePos.y == newPt.y) {
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
            runtime.lastSamplePos = newPt;
            runtime.lastSampleValid = true;
            runtime.lastMovementTime = now;
            runtime.isFading = false;
            MarkContentChanged();
        }
    }
    return 0;
}

// Allocates/recreates the backbuffer bitmap when the screen size changes.
// Rebuilding the bitmap invalidates the render target, so it is released here.
static void EnsureBackbuffer(HDC hdcScreen, int vW, int vH) {
    if (!render.hBitmap || render.cachedVW != vW || render.cachedVH != vH) {
        // Delete the DC before the bitmap: the bitmap is still selected into the
        // DC and DeleteObject fails for a selected bitmap (leaking it).
        if (render.hdcMem) {
            DeleteDC(render.hdcMem);
            render.hdcMem = NULL;
        }
        if (render.hBitmap) {
            DeleteObject(render.hBitmap);
            render.hBitmap = NULL;
        }

        render.hdcMem = CreateCompatibleDC(hdcScreen);
        render.hBitmap = CreateCompatibleBitmap(hdcScreen, vW, vH);
        SelectObject(render.hdcMem, render.hBitmap);

        render.cachedVW = vW;
        render.cachedVH = vH;

        // A freshly created bitmap holds uninitialized pixels; force a full
        // clear on the next frame instead of relying on the dirty-rect clear.
        runtime.needsFullClear = true;

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

    // Reserve for the points actually available, capped by the budget (kMaxPoints
    // is bounded by the settings clamps), instead of the full budget each frame.
    size_t n = runtime.history.size();
    size_t cap = n < kMaxPoints ? n : kMaxPoints;
    smoothed.reserve(cap);
    cursors.reserve(cap);

    if (settings.isGhost) {
        // Ghost copies are latched at their spawn position by the poll thread
        // (spaced by ghostSpawnDist), so draw every history sample directly and
        // apply no render-time decimation. That keeps each copy fixed in place
        // instead of sliding along with the moving head.
        DWORD now = timeGetTime();
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
        // else if (style == L"glow") { RenderGlowStyle(smoothed); }
    }
    // A trail frame replaces the previous one and the dirty-rect blit erases
    // any leftovers, so no separate clear pass is queued. needsClear is only
    // raised by the transient toggle effect that ends without drawing, to force
    // one erasing frame.
    runtime.needsClear = false;
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

// Grows bbox/hasBBox to cover the toggle-effect circle's maximum extent. Uses
// the largest possible radius (rather than the current eased radius) so the
// clear/clip region always contains the circle.
static void GrowToggleEffectBBox(const D2D1_POINT_2F& head, RECT& bbox, bool& hasBBox) {
    float cursorSize = 32.0f;
    if (cursor.bmWidth > 0 && cursor.bmHeight > 0) {
        float w = cursor.bmWidth * cursor.dpiScaleX;
        float h = cursor.bmHeight * cursor.dpiScaleY;
        cursorSize = (w > h) ? w : h;
    }
    float maxRadius = cursorSize * kEffectDiameterFactor * 0.5f;
    float margin = maxRadius + kEffectStrokeWidth + 1.0f;
    GrowBBox(bbox, hasBBox,
             (LONG)(head.x - margin), (LONG)(head.y - margin),
             (LONG)(head.x + margin), (LONG)(head.y + margin));
}

VOID CALLBACK SmearTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(idEvent);

    POINT pt;
    GetCursorPos(&pt);

    // Read local copies of the atomic flags for consistent use within this
    // frame. The trail is suppressed (history cleared, overlay wiped) while a
    // fullscreen game runs or the enable/disable hotkey has it turned off.
    bool gameRunning = runtime.isGameRunning.load();
    bool suppress = gameRunning || !runtime.trailEnabled.load();
    bool effectActive = toggleEffect.active;

    int vX = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vY = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vH = GetSystemMetrics(SM_CYVIRTUALSCREEN) - 1;

    if (suppress) {
        // Keep the cursor geometry fresh so the toggle circle can follow the
        // cursor even while the trail itself is off.
        if (effectActive) {
            UpdateCursorCenterOffset();
        }
        // Decide whether there is anything to draw while holding the lock, but
        // release it before EnterIdleIfInactive (which takes the lock itself).
        bool nothingToDraw;
        {
            std::lock_guard<std::mutex> lock(runtime.historyMutex);
            bool wasEmpty = runtime.history.empty();
            runtime.history.clear();
            nothingToDraw = wasEmpty && !runtime.needsClear && !effectActive;
            if (nothingToDraw) {
                // Nothing is staged for drawing after the wipe; mark the current
                // content as rendered so EnterIdleIfInactive may stop the timer.
                runtime.renderedRevision =
                    runtime.contentRevision.load(std::memory_order_relaxed);
            } else {
                // Something was on screen (or an effect is pending);
                // force a frame so it gets erased instead of being skipped.
                MarkContentChanged();
            }
        }
        if (nothingToDraw) {
            // Nothing to draw; let the overlay go idle. The poll thread keeps
            // checking for games and the hotkey wakes us again.
            EnterIdleIfInactive(dwTime, true);
            return;
        }
    } else {
        // Trail active — update cursor appearance caches. The poll thread
        // owns trail-origin selection (smooth glide to the cursor's visual
        // center) and sample accumulation.
        UpdateCursorCenterOffset();
    }

    // Snapshot the content revision (and the sample count) to decide whether
    // this frame would differ from the last one. Frames that would be
    // pixel-identical are skipped so a static trail doesn't keep repainting the
    // whole backbuffer at 125 Hz.
    unsigned long long rev;
    size_t historyCount;
    {
        std::lock_guard<std::mutex> lock(runtime.historyMutex);
        rev = runtime.contentRevision.load(std::memory_order_relaxed);
        historyCount = runtime.history.size();
    }
    bool contentChanged = (rev != runtime.renderedRevision);
    unsigned settingsVersion = g_settingsVersion.load(std::memory_order_relaxed);
    bool settingsChanged = (settingsVersion != runtime.renderedSettingsVersion);
    // Cursor-ghost time-based copies fade by age, so their look changes every
    // frame even with an unchanged history. Every other combination only
    // changes when the history does (index-based ratios, eviction-driven fades).
    // A single sample draws nothing, so it doesn't count.
    bool timeDependent = settings.isGhost && !settings.sizeBased &&
                         historyCount >= 2;
    bool wantDraw = contentChanged || settingsChanged || timeDependent ||
                    runtime.needsClear || effectActive;

    if (wantDraw) {
        // Something is being drawn this frame; keep the overlay awake.
        runtime.lastActiveTime = dwTime;
        HDC hdcScreen = GetDC(NULL);

        EnsureBackbuffer(hdcScreen, vW, vH);
        EnsureRenderTarget();

        RECT curBBox = { 0, 0, 0, 0 };
        bool hasCurBBox = false;

        // Build the trail points and bounding box BEFORE drawing so the clear
        // pass can be clipped to just the affected region instead of wiping the
        // whole virtual-desktop backbuffer every frame.
        std::vector<D2D1_POINT_2F> smoothed;
        std::vector<HCURSOR> cursors;
        std::vector<float> ratios;
        if (render.pDCRenderTarget) {
            BuildTrailPoints(pt, vX, vY, smoothed, cursors, ratios);
            if (smoothed.size() >= 2) {
                if (!settings.isGhost) {
                    ChaikinSmooth(smoothed);
                }
                ComputeTrailBBox(smoothed, cursors, curBBox);
                hasCurBBox = true;
            }
            if (effectActive && !smoothed.empty()) {
                GrowToggleEffectBBox(smoothed[0], curBBox, hasCurBBox);
            }
        }

        // The region to clear + redraw is the union of this frame's bbox and the
        // previous frame's bbox (so the old trail is erased). Falls back to the
        // full screen when there is no bounded region.
        RECT clearRect = { 0, 0, vW, vH };
        bool useClearRect = false;
        if (hasCurBBox && runtime.hasPrevDirty) {
            clearRect.left   = (curBBox.left   < runtime.prevDirtyRect.left)   ? curBBox.left   : runtime.prevDirtyRect.left;
            clearRect.top    = (curBBox.top    < runtime.prevDirtyRect.top)    ? curBBox.top    : runtime.prevDirtyRect.top;
            clearRect.right  = (curBBox.right  > runtime.prevDirtyRect.right)  ? curBBox.right  : runtime.prevDirtyRect.right;
            clearRect.bottom = (curBBox.bottom > runtime.prevDirtyRect.bottom) ? curBBox.bottom : runtime.prevDirtyRect.bottom;
            useClearRect = true;
        } else if (hasCurBBox) {
            clearRect = curBBox;
            useClearRect = true;
        } else if (runtime.hasPrevDirty) {
            clearRect = runtime.prevDirtyRect;
            useClearRect = true;
        }
        if (useClearRect) {
            if (clearRect.left < 0) clearRect.left = 0;
            if (clearRect.top < 0) clearRect.top = 0;
            if (clearRect.right > vW) clearRect.right = vW;
            if (clearRect.bottom > vH) clearRect.bottom = vH;
            int clearW = clearRect.right - clearRect.left;
            int clearH = clearRect.bottom - clearRect.top;
            if (clearW > 768 || clearH > 768 || clearW <= 0 || clearH <= 0) {
                useClearRect = false;
            }
        }
        // A freshly (re)created backbuffer holds uninitialized pixels, so clear
        // the whole thing once rather than relying on the clipped clear.
        if (runtime.needsFullClear) {
            useClearRect = false;
            runtime.needsFullClear = false;
        }

        if (render.pDCRenderTarget) {
            RECT rc = { 0, 0, vW, vH };
            if (useClearRect) rc = clearRect;
            render.pDCRenderTarget->BindDC(render.hdcMem, &rc);

            render.pDCRenderTarget->BeginDraw();
            // BindDC re-origins the target at the sub-rect's top-left, so undo
            // that shift: drawing code still uses absolute backbuffer coords.
            render.pDCRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(
                useClearRect ? -(float)rc.left : 0.0f,
                useClearRect ? -(float)rc.top : 0.0f));
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

            RenderTrail(smoothed, cursors, ratios);

            RenderToggleEffect(smoothed, curBBox, hasCurBBox);

            HRESULT hr = render.pDCRenderTarget->EndDraw();
            if (hr == D2DERR_RECREATE_TARGET) {
                ReleaseRenderTargetResources();
            }
            if (settings.isGhost) {
                PruneCursorCaches(cursors);
            }
        }

        BlitOverlay(hwnd, hdcScreen, vX, vY, vW, vH, curBBox, hasCurBBox);

        // Mark the revisions we actually drew. Any change that arrived while
        // this frame was being built leaves contentRevision/settingsVersion
        // ahead, so the next tick redraws.
        runtime.renderedRevision = rev;
        runtime.renderedSettingsVersion = settingsVersion;
    }

    // If nothing changed this frame, release the render timer once the grace
    // period has elapsed.
    EnterIdleIfInactive(dwTime, !wantDraw);
}

// Custom window proc for the overlay. Handles WM_TIMER (posted by the
// multimedia timer callback) by calling SmearTimerProc directly. Also handles
// the enable/disable hotkey (WM_HOTKEY), hotkey re-registration on settings
// change (kMsgApplyHotkey), and settings reload (kMsgApplySettings). All other
// messages go to DefWindowProc. This keeps all rendering on the overlay thread
// while using the multimedia timer for non-coalesced wakeups.
LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_TIMER) {
        // Clear before rendering so a frame that arrives while this one is still
        // in flight can queue exactly one more render (bounded, no backlog).
        runtime.renderScheduled.store(false);
        SmearTimerProc(hwnd, uMsg, wParam, GetTickCount());
        return 0;
    }
    if (uMsg == kMsgIdleWake) {
        // The poll thread saw movement while the overlay was idle; resume
        // rendering (and re-arm the render timer / timer resolution).
        ResumeRenderTimer();
        runtime.lastActiveTime = GetTickCount();
        return 0;
    }
    if (uMsg == WM_HOTKEY) {
        // A hotkey must work even while the overlay is idle.
        ResumeRenderTimer();
        runtime.lastActiveTime = GetTickCount();
        if ((int)wParam == kHotkeyId) {
            bool enabled = !runtime.trailEnabled.load();
            runtime.trailEnabled.store(enabled);
            StartToggleEffect(enabled);
            Wh_Log(L"Trail %s via hotkey", enabled ? L"enabled" : L"disabled");
        }
        return 0;
    }
    if (uMsg == kMsgApplyHotkey) {
        ResumeRenderTimer();
        runtime.lastActiveTime = GetTickCount();
        ApplyHotkey(hwnd);
        return 0;
    }
    if (uMsg == kMsgApplySettings) {
        // Settings must not change while the render thread reads them mid-frame
        // or while the poll thread reads them under historyMutex. Handling the
        // reload here on the overlay thread serializes it against rendering,
        // and taking historyMutex serializes it against the poll thread.
        ResumeRenderTimer();
        runtime.lastActiveTime = GetTickCount();
        {
            std::lock_guard<std::mutex> lock(runtime.historyMutex);
            LoadSettings();
            // Samples captured under the old style were offset differently
            // (e.g. the line origin vs. the ghost hotspot), so drop them and
            // start the trail fresh rather than briefly misplacing the copies.
            runtime.history.clear();
            runtime.lastSampleValid = false;
            MarkContentChanged();
        }
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

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &render.pD2DFactory);

    HINSTANCE hInstance = GetModuleHandle(NULL);
    // Unique overlay window class name (must not collide with other mods).
    const wchar_t CLASS_NAME[] = L"CursorTrailOverlayClass";

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
        L"CursorTrailOverlay",
        WS_POPUP,
        screenX, screenY, screenW, screenH,
        NULL, NULL, hInstance, NULL
    );

    // The window (and its message queue) now exist, or creation failed and we
    // are about to return. Signal either way so WhTool_ModUninit's wait for the
    // window can't block forever on the failed path.
    if (runtime.overlayReadyEvent) {
        SetEvent(runtime.overlayReadyEvent);
    }

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
    // rendering on this thread. The timer (and the 1 ms resolution) is released
    // by EnterIdleIfInactive when there is nothing to draw and re-armed by
    // ResumeRenderTimer on movement or a hotkey.
    timeBeginPeriod(1);
    runtime.periodRaised = true;
    runtime.mmTimerId = timeSetEvent(kRenderIntervalMs, 1, MMTimerCallback, 0, TIME_PERIODIC);

    runtime.lastActiveTime = GetTickCount();

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

    if (render.hdcMem) { DeleteDC(render.hdcMem); render.hdcMem = NULL; }
    if (render.hBitmap) { DeleteObject(render.hBitmap); render.hBitmap = NULL; }

    // Kill the multimedia timer if still running (may have been killed
    // already by EnterIdleIfInactive) and release the timer resolution if we
    // still hold it.
    if (runtime.mmTimerId) {
        timeKillEvent(runtime.mmTimerId);
        runtime.mmTimerId = 0;
    }
    if (runtime.periodRaised) {
        timeEndPeriod(1);
        runtime.periodRaised = false;
    }

    UnregisterHotKey(runtime.overlayHwnd, kHotkeyId);
    DestroyWindow(runtime.overlayHwnd);
    UnregisterClass(CLASS_NAME, hInstance);

    if (g_pWicFactory) { g_pWicFactory->Release(); g_pWicFactory = nullptr; }
    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    // Created before the thread so WhTool_ModUninit can wait for the overlay
    // window to exist even if the mod is disabled immediately after starting.
    runtime.overlayReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
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

    // Wait until the overlay thread has created (or failed to create) its
    // window. Without this, disabling right after startup could find
    // runtime.overlayHwnd still NULL, post no WM_QUIT, and then block the join
    // below forever.
    if (runtime.threadHandle && runtime.overlayReadyEvent) {
        WaitForSingleObject(runtime.overlayReadyEvent, INFINITE);
    }

    // Post WM_QUIT to the overlay window so its message loop exits and runs the
    // thread's own teardown (kill the timer, free D2D, destroy the window).
    // The timer callback coalesces to a single pending WM_TIMER (renderScheduled
    // guard), so it can't starve WM_QUIT and a clean, unbounded join is reliable.
    if (runtime.overlayHwnd) {
        PostMessage(runtime.overlayHwnd, WM_QUIT, 0, 0);
    }
    if (runtime.threadHandle) {
        WaitForSingleObject(runtime.threadHandle, INFINITE);
        CloseHandle(runtime.threadHandle);
        runtime.threadHandle = NULL;
    }
    if (runtime.overlayReadyEvent) {
        CloseHandle(runtime.overlayReadyEvent);
        runtime.overlayReadyEvent = NULL;
    }
}

void WhTool_ModSettingsChanged() {
    // Reload settings on the overlay thread (kMsgApplySettings) so the write
    // doesn't race with the render/poll threads. Re-register the hotkey on the
    // overlay thread too, which owns the window and its message queue
    // (RegisterHotKey/UnregisterHotKey are thread-bound).
    if (runtime.overlayHwnd) {
        PostMessage(runtime.overlayHwnd, kMsgApplySettings, 0, 0);
        PostMessage(runtime.overlayHwnd, kMsgApplyHotkey, 0, 0);
    } else {
        LoadSettings();  // overlay thread not running yet; no race
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
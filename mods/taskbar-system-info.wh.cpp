// ==WindhawkMod==
// @id              taskbar-system-info
// @name            Taskbar System Info
// @name:uk-UA      Системний монітор панелі завдань
// @description     CPU, GPU, RAM and VRAM on the Windows 11 taskbar, with temperatures, history graphs, adaptive layouts and live dragging.
// @description:uk-UA CPU, GPU, RAM і VRAM на панелі завдань Windows 11: температури, графіки історії, адаптивні макети та перетягування живого віджета.
// @version         1.7.0
// @author          Yevhenii Starychenko
// @github          https://github.com/starychenko
// @homepage        https://github.com/starychenko/windhawk-taskbar-system-info
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lpdh -ldxgi -lcomctl32 -lgdi32 -lgdiplus -DWIN32_LEAN_AND_MEAN
// ==/WindhawkMod==

// Taskbar XAML discovery and window-thread marshaling are based on techniques
// from "Multirow taskbar for Windows 11" by Michael Maltsev (m417z):
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp
// Native GPU temperature collection via D3DKMT follows Taskbar Clock
// Customization by Michael Maltsev (m417z):
// https://github.com/m417z/my-windhawk-mods/commit/861920df6380f4c13abec5d9226362c4725e8362
// Secondary-taskbar discovery is adapted from Taskbar Fluent Media Player by
// Salyts:
// https://github.com/Salyts/Taskbar-Fluent-Media-Player
// The first two projects are GPL-3.0; Taskbar Fluent Media Player is MIT.

/*
Taskbar Fluent Media Player MIT notice:

Copyright (c) 2026 Salyts

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
*/

// ==WindhawkModReadme==
/*
# Taskbar System Info

CPU, GPU, RAM and VRAM on the Windows 11 taskbar. Usage, temperatures and
history graphs stay in one widget. It adapts to the free space and taskbar
height, and you can drag it to another position or monitor.

![Dark theme with sample CPU, GPU, RAM and VRAM readings](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-dark.png)

The images on this page use the mod's actual XAML widget and native move renderer.
The readings match a workstation snapshot: Ryzen 9 5900X, 64 GB RAM and Radeon
RX 7900 XTX with 24 GB VRAM. Graph history is illustrative. These are isolated
renders, not screenshots of a live Explorer session.

The widget shows CPU and GPU usage, their temperatures, and a history graph for
each. RAM and VRAM show the percentage, used/total capacity and a thin usage bar.
Values stay in fixed columns as the readings change. CPU/GPU fields are compact,
with six logical pixels of padding on each side of the widget.

When space is limited, the widget can hide graphs or memory details, or use one
row on a low taskbar. It restores the full view when space returns. No manual
mode switch is needed. See [Adaptive layouts](#adaptive-layouts) for examples.

Normal readings use the taskbar text color. Temperature and memory alerts add
color when a threshold is reached. Light, dark and Windows high-contrast themes
are supported. You can also set the fonts, colors, opacity and alert thresholds.

In normal use, clicks pass through to the taskbar. Press **Ctrl+Alt+M** when you
want to move the widget. It stays live, gets a hand cursor and a translucent
background only while dragging, and snaps to a usable place. A red frame means the position cannot be
saved. **Enter** saves it, **Esc** cancels it.

## Quick start

1. Install and enable the mod. CPU, GPU, RAM and VRAM usually work without
   additional software. See [Install](#install) for the current source.
2. Leave **Temperature source** on **Automatic**. If a temperature stays at
   `--°C`, check [Setting up HWiNFO temperatures](#setting-up-hwinfo-temperatures).
3. Press **Ctrl+Alt+M**, drag the widget along the taskbar, then press **Enter**.
   You can drag onto another monitor's taskbar too. **Esc** keeps the old position.
4. If you want space before Start, enable **Reserve space before the Start button**.
   The mod only shifts the button group when the measured controls still fit.
5. Set **Taskbar monitor** and **Left offset** if you prefer to use settings.
   Monitor 1 is the primary display. Changing the monitor setting overrides the
   display selected by dragging.

## Screenshots

These examples use the same readings in each layout: CPU 4% at 47°C, GPU 0%
at 37°C, RAM about 23.3/64 GiB and VRAM about 2.3/24 GiB. The larger text
example uses font size 13 and a 200% move-preview scale. Windows contrast colors
are simulated in the high-contrast example.

| Light theme | Full layout at the minimum configured width, 330 logical pixels |
| --- | --- |
| ![Light theme](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-light.png) | ![Full widget at 330 logical pixels, with graphs and memory capacities](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-compact.png) |

| Moving the live widget | No readable space at the chosen position |
| --- | --- |
| ![Live readings during dragging](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-move.png) | ![Red frame for an invalid position](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-move-invalid.png) |

| Larger text at 200% scale | Windows high-contrast colors |
| --- | --- |
| ![Large text at 200 percent scale](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-large-text.png) | ![High-contrast move frame](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-high-contrast.png) |

Unavailable readings stay visible as `--`. A graph leaves a gap when there is
no valid sample.
If collection stops, readings become unavailable after five seconds or three
configured update intervals, whichever is longer. History continues to age out;
the next fresh sample restores the readings, including in move mode.

![Unavailable readings](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-unavailable.png)

## Adaptive layouts

Adaptation is automatic on the selected taskbar. The mod measures the free gaps
between visible controls, the taskbar height and text in the configured font.
Opening more apps, changing Search or Widgets, changing the font, or moving to a
taskbar with different dimensions can change the result. Screen resolution alone
does not determine the layout.

| Layout | What stays visible | When it is used |
| --- | --- | --- |
| **Full, two rows** | CPU/GPU usage, temperatures and graphs; RAM/VRAM percentages, capacities and bars. | The full view fits, including readable shrinking or tighter row spacing. |
| **Without graphs, two rows** | All numeric readings and memory bars. | The full view cannot fit, but removing graphs leaves enough room for the numeric details. |
| **Compact, two rows** | CPU/GPU usage and temperatures; RAM/VRAM percentages. | Removing memory capacities and bars makes two rows fit. |
| **Compact, one row** | The same essential readings, ordered CPU, GPU, RAM, VRAM. | The panel is too low for readable two-row layouts and a wider single row fits. |

| Full, two rows | Without graphs, two rows |
| --- | --- |
| ![Full adaptive layout with graphs, memory capacities and bars](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-adaptive-full.png) | ![Adaptive two-row layout without graphs, retaining memory capacities and bars](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-adaptive-no-graphs.png) |

| Compact, two rows | Compact, one row |
| --- | --- |
| ![Compact two-row layout with CPU and GPU temperatures and RAM and VRAM percentages](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-adaptive-two-rows.png) | ![Compact single-row layout on a low taskbar](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-adaptive-one-row.png) |

| Compact two rows in the light theme | Compact one row in the light theme |
| --- | --- |
| ![Light-theme compact two-row layout](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-adaptive-two-rows-light.png) | ![Light-theme compact single-row layout](https://raw.githubusercontent.com/starychenko/windhawk-taskbar-system-info/main/assets/widget-adaptive-one-row-light.png) |

The compact examples above use 30- and 20-logical-pixel taskbar heights. These
are example dimensions, not fixed switching thresholds. The single row needs
more horizontal space than the compact two-row layout. A narrow gap on a normal
height taskbar can therefore use two rows, while a low panel with a wider gap
can use one.

The normal full layout is tried first. It can shrink to 85% of its size while
keeping the main font at least 9 logical pixels. If it cannot fit or its cells
are too small for the configured font, the mod tries a measured full layout,
then no graphs, compact two rows and compact one row at the configured font size.
Two-row layouts reduce vertical gaps before reducing row height. If needed,
compact layouts can also shrink, keeping the main font at least 9 logical pixels.
If no readable layout fits, the normal widget hides. In move mode, the frame
turns red and the position cannot be saved.

Hidden details still update. Graphs keep collecting history while hidden and
return with its actual age. When room becomes available again, the mod restores
the full view and preferred position. Adaptation does not overwrite **Widget
width**, **Left offset** or positions saved by dragging. **Widget width** is a
preference for the full view; it does not cap the measured width of an adaptive
layout that fits at the configured font size.

While dragging between taskbars, the preview uses the destination's free space,
height and DPI. It can change layout during the drag while keeping the grabbed
point under the cursor. **Enter** checks the current layout again before saving.

## Moving and saved positions

Press **Ctrl+Alt+M** to enter move mode. Grab the widget with the hand cursor and
drag it where you need it. The readings and graphs keep updating while you move.
The original widget is hidden during editing and returns when you cancel.
The normal widget has no glass background. Glass appears only while you hold the
mouse button and drag. After release, the background clears and a thin outline
marks the pending position until Enter or Esc. Hover alone does not add glass.

The frame snaps to a place that can fit the widget. Full size and readable text
take priority over distance. If there is no usable place, it turns red. Releasing
the mouse keeps the preview there, so you can check the position before saving it.

| Key | What it does |
| --- | --- |
| **Enter** | Checks the current taskbar layout again and saves a valid position. A red position cannot be applied. |
| **Esc** | Cancels editing and keeps the previous position. |
| **Home** | Prepares a return to **Taskbar monitor** and **Left offset**. Press Enter to confirm or Esc to cancel. |

Clicking another application, changing settings, changing the display setup or
unloading the mod also cancels editing. The widget and reserved button space
move only after confirmation. A failed move or storage write keeps the previous
saved position.

You can change **Move widget hotkey**. It accepts Ctrl, Alt, Shift or Win with
one letter, digit or F1-F24. Leave it empty to disable the shortcut. An invalid
or already registered shortcut is not registered; the Windhawk log gives the
reason.

Positions are saved separately for each display in Windhawk's local mod storage.
The display is identified by its device path, so its Windows display number can
change without replacing the saved target. The horizontal position is stored
as a fraction of the current layout's available travel width.

Before the first confirmed drag, **Taskbar monitor** and **Left offset** choose
the target. Changing **Taskbar monitor** restores the display choice from settings.
Changing **Left offset** clears the dragged position for the current display.
Other settings keep the saved positions. **Home**, then **Enter**, clears all
positions saved by dragging and returns control to the monitor/offset settings.

If the selected display disconnects, the widget temporarily uses the primary
taskbar. Its saved profile stays intact. When the display returns, the widget
returns to it too.

## Placement and spacing

Placement uses the visible hit areas of Start, Search, Task View, Widgets/weather,
app buttons, overflow, the tray and clock. An empty or stretched background
container does not count as occupied space. Other bounded XAML buttons can also
be included in the map.

The widget keeps at least six logical pixels clear of mapped controls and the
taskbar edges. It also has six logical pixels of inner side padding. These gaps
scale with the display, so the widget does not sit against the next button.

The nearest place for the full width is preferred. When shrinking is necessary,
the widest usable gap is preferred before distance. The widget chooses a readable
layout for that gap and the taskbar height; see [Adaptive layouts](#adaptive-layouts).
Compact cells reserve room for maximum readings and unavailable placeholders, so
changing values do not move the columns.

**Reserve space before the Start button** adds a placement option before the
Start/app group. Existing margins are kept. If the arranged buttons would
collide with another mapped element, the reservation is undone and the mod uses
a free gap. **Reserved space gap** defaults to 8 logical pixels, with an effective
minimum of 6. The mod does not reorder individual app buttons.

Left and centered taskbar alignment use the same placement map. Separately drawn
items or windows from another taskbar mod may need manual positioning or a
compatibility fix. A particular Taskbar Styler preset still needs a live check.

## Metrics and alerts

- CPU usage comes from Windows Processor Utility when available, with
  `GetSystemTimes` as the fallback.
- GPU usage and memory usage come from Windows performance counters. Adapter
  identity and memory capacity come from D3DKMT, with DXGI as the fallback.
- RAM usage and capacity come from Windows memory status.
- Temperatures come from HWiNFO or the Windows/driver interfaces listed below.

CPU and GPU graphs use a fixed 0-100% scale. History defaults to 60 seconds and
can be set from 15 to 180 seconds. Readings default to a one-second interval;
the supported range is 1-10 seconds. Missing samples leave gaps instead of
joining unknown readings. Memory capacities use GiB, shown as `G` in the widget.
Small or fractional totals keep one decimal place.

| Reading | Warning | Critical |
| --- | ---: | ---: |
| CPU temperature | 75°C | 85°C |
| GPU temperature | 80°C | 90°C |
| RAM and VRAM | 80% | 90% |

A small release margin stops alerts flickering around a threshold. CPU and GPU
usage stays in the normal text color, including short 100% spikes.

The GPU with the most dedicated VRAM is selected by default. Use **GPU adapter
filter** for another card. Usage, memory and native GPU temperature are matched
to the selected live adapter.

**GPU memory type** normally stays on **Automatic**. Integrated GPUs use the
Windows shared-memory limit; discrete GPUs use dedicated VRAM. Shared memory is
backed by system RAM and is not a fixed VRAM chip capacity. If a driver or an
older low-memory card is detected incorrectly, set the memory type explicitly.

The mod reads system information. It does not control clocks, fans, power limits
or GPU settings. It does not collect network/disk activity, send telemetry or
make internet requests.

## Temperature providers

**Automatic** fills CPU and GPU temperatures separately. It tries HWiNFO Shared
Memory, then HWiNFO Gadget Registry, then the Windows fallback for any temperature
that is still missing.

| Temperature source | What it reads |
| --- | --- |
| **Automatic** | HWiNFO first, then Windows/driver readings for missing temperatures. |
| **HWiNFO automatic** | Shared Memory, then Gadget Registry. |
| **HWiNFO Shared Memory** | `Global\HWiNFO_SENS_SM2` only. The shared-memory interface targets HWiNFO 7.0 or newer. |
| **HWiNFO Gadget Registry** | `HKCU\Software\HWiNFO64\VSB` only. HWiNFO and Explorer must use the same Windows user. |
| **Windows native** | GPU temperature from the selected display driver through D3DKMT; CPU fallback from Windows ACPI thermal zones through PDH. |
| **Disabled** | Skips temperature collection. Usage, memory and graphs keep working. |

Windows thermal zones can describe a motherboard, chassis, skin or
processor-related sensor. They are not always the CPU package temperature.
**Windows thermal zone filter** selects matching zone names. **Windows thermal
zone aggregation** uses their average by default, or the hottest zone if selected.
The Windows CPU fallback stays unavailable if the system exposes no thermal zones.

HWiNFO is optional and is not bundled with the mod. Automatic CPU matching
prefers `CPU (Tctl/Tdie)`, `CPU Die (average)` or `CPU Package`; GPU matching
prefers `GPU Temperature` for the selected adapter. On a multi-GPU system, use
the adapter and temperature filters if automatic matching picks the wrong sensor.
If Windows has never supplied an adapter identity and no adapter filter is set,
HWiNFO uses its generic GPU-temperature match.

An unavailable reading shows `--°C`. An old temperature is not kept as a current
reading. One missing provider does not stop the other metrics.
Shared Memory readings expire when HWiNFO's poll timestamp stops advancing or is
already too old. The limit allows three HWiNFO polling periods plus two seconds
of slack, with a five-second minimum. Automatic then tries the remaining sources.
Gadget Registry readings expire after 60 seconds without a key write. Registry
has no polling heartbeat, and HWiNFO skips writes when exported values do not
change, so an unchanged key may also expire while HWiNFO is running. Automatic
then tries the remaining sources; missing temperatures in HWiNFO-only modes
show `--°C`.
Use Shared Memory when constant temperatures must remain available. A new
Registry write restores readings, even if the selected temperature is unchanged.

## Setting up HWiNFO temperatures

Use HWiNFO when Windows cannot supply the temperature you want, or when you want
its CPU package sensor. Keep HWiNFO running. **Sensors-only** mode is enough.

### Shared Memory

1. Open HWiNFO **Settings**.
2. Under **General / User Interface**, enable **Shared Memory Support**.
3. Start or reopen the Sensors window.
4. Keep the mod on **Automatic**, or select **HWiNFO Shared Memory** to use only
   that interface.

The non-Pro HWiNFO64/ARM64 edition disables Shared Memory after 12 hours of
continuous use. Re-enable it manually, use Gadget Registry, allow the Windows
fallback, or use Pro. This limit belongs to HWiNFO; the mod does not bypass it.
See [HWiNFO's license comparison](https://www.hwinfo.com/licenses/).

### Gadget Registry

1. Open the Sensors window and **Sensor Settings**.
2. Open the **HWiNFO Gadget** tab and enable gadget reporting.
3. Mark the CPU and GPU temperature readings for **Report to Gadget** reporting
   (the sensor checkbox may be labelled **Report value in Gadget**).
4. Run HWiNFO and Explorer under the same Windows user.
5. Keep the mod on **Automatic**, or select **HWiNFO Gadget Registry**.

The [HWiNFO author's setup note](https://www.hwinfo.com/forum/threads/hwinfomonitor-version-confusion.9300/)
explains the Gadget tab and the registry location. If the wrong sensor is chosen,
set **CPU temperature sensor filter** or **GPU temperature sensor filter** to a
distinctive part of its HWiNFO name. Otherwise, leave the filters empty.

## Settings guide

### Layout and sampling

| Setting | What to change |
| --- | --- |
| **Widget width** | Preferred full-layout width, including side padding. Range: 330-800 logical pixels; default: 410. Adaptive layouts use their measured size within the available space. |
| **Left offset** | Preferred horizontal position before dragging. Nonnegative logical pixels; default: 10. |
| **Taskbar monitor** | Initial display, range 1-32. Monitor 1 is primary; the rest follow their position in the virtual desktop and may differ from Windows numbering. |
| **Move widget hotkey** | Default: `Ctrl+Alt+M`. Empty disables it. |
| **Reserve space before the Start button** | Allows the button group to shift when a safe reservation fits. Off by default. |
| **Reserved space gap** | Gap after a reservation. Range: 0-100 logical pixels; default: 8; effective minimum: 6. |
| **Update interval** | Collection interval, 1-10 seconds; default: 1. |
| **Graph history** | CPU/GPU history, 15-180 seconds; default: 60. |

### Appearance

| Setting | What to change |
| --- | --- |
| **Font size** | Range: 9-13 logical pixels; default: 11. |
| **Font family** | Default: Segoe UI Variable Text. Keep a compact font or increase the widget width. |
| **Adapt colors to the taskbar theme** | On by default. Follows light, dark and Windows high-contrast colors. |
| **Text color** | Manual text color when adaptive colors are off. Empty uses the system color. |
| **Graph and bar color** | Manual color for CPU/GPU graphs and RAM/VRAM bars. |
| **Warning color** | Manual color for warning readings. |
| **Critical color** | Manual color for critical readings. |
| **Text opacity** | Default: 96%. Labels are slightly dimmer; high contrast keeps important content fully visible. |

### Alerts and sensors

| Setting | What to change |
| --- | --- |
| **CPU temperature warning** | Default: 75°C. |
| **CPU critical temperature** | Default: 85°C. |
| **GPU temperature warning** | Default: 80°C. |
| **GPU critical temperature** | Default: 90°C. |
| **Memory usage warning** | RAM/VRAM warning; default: 80%. |
| **Critical memory usage** | RAM/VRAM critical level; default: 90%. |
| **GPU adapter filter** | Partial Windows adapter name. Empty selects the GPU with the most dedicated VRAM. |
| **GPU memory type** | Automatic, Dedicated VRAM or Shared GPU memory. |
| **Temperature source** | The provider modes listed above; default: Automatic. |
| **Windows thermal zone filter** | Partial zone name for the Windows CPU fallback. |
| **Windows thermal zone aggregation** | Average or Hottest; default: Average. |
| **CPU temperature sensor filter** | Partial HWiNFO CPU sensor name. |
| **GPU temperature sensor filter** | Partial HWiNFO GPU sensor name. |

Manual text, graph and alert colors apply when adaptive colors are off. A critical
threshold is kept above its warning threshold. Alerts only change the display.

## Troubleshooting

| What you see | What to check |
| --- | --- |
| Temperature stays at `--°C` | Windows may not expose that sensor. Configure HWiNFO and confirm it is running. Leave Automatic mode on. |
| HWiNFO stops after about 12 hours | Re-enable Shared Memory, use Gadget Registry or let Automatic use Windows readings. |
| GPU temperature belongs to another card | Set GPU adapter filter, then GPU temperature sensor filter if needed. |
| GPU or VRAM stays at `--` after a driver update | Allow up to one minute for adapter refresh or a fresh-counter probe, plus a few samples to establish a baseline. Check the Windhawk log; reload the mod if Windows still supplies no valid readings. |
| Integrated-GPU memory looks too large | Automatic shows the Windows shared-memory limit. Select Dedicated VRAM only if you want the reserved carve-out. |
| An old 512 MB discrete card is shown as shared | Set GPU memory type to Dedicated VRAM. Automatic detection can mistake an old low-memory card for an integrated GPU. |
| Graphs or memory capacities disappeared | The widget selected an adaptive layout. These details return automatically when the free gap or taskbar height allows it. See Adaptive layouts. |
| The widget changed to one row | The selected taskbar is too low for a readable two-row view. One row keeps essential readings when enough width is available. |
| The widget is hidden | The available gap must fit the readability limits and side clearance. Check width, font and the Windhawk log. Try another position or Reserve space. |
| The widget is on the wrong display | Check Taskbar monitor or drag it to the required taskbar. Home, then Enter, clears positions saved by dragging. |
| The hotkey does nothing | Check Move widget hotkey and the log. Choose another combination if it is invalid or already registered. |
| The move frame turns red | The target has no ready taskbar or no readable space. Move to a usable area before pressing Enter. |
| Another taskbar mod overlaps it | Only discoverable XAML bounds are mapped. Custom drawing or separate windows may need a different position or a compatibility fix. |

The Windhawk log records provider changes, adapter selection, counter recovery,
placement errors and sensor mismatches. It does not print every sample.

## Compatibility

The current source is **1.7.0**, built with Windhawk **1.7.3** for Windows 11.
The widget targets horizontal primary and secondary taskbars. x64 and ARM64
builds pass; ARM64 hardware has not been checked.

Placement and editing pass isolated XAML/Win32 tests. Live Explorer checks for
this version are still pending, including top/bottom taskbars, physical moves
between monitors with different DPI, unplug/reconnect, restart persistence and
Taskbar Styler presets. Test renders do not prove those configurations.

The normal widget uses native taskbar XAML. Move mode uses a temporary Win32
window. It does not use XAML Diagnostics. Display changes recheck the target even
when the number of monitors stays the same. Wider fonts need more free space and
can select a compact layout or hide the widget if no readable layout fits.

## Install

Search for **Taskbar System Info** in Windhawk and select **Install**.
To use this repository's source directly:

1. Open Windhawk and select **Create a new mod**.
2. Replace the generated source with [taskbar-system-info.wh.cpp](https://github.com/starychenko/windhawk-taskbar-system-info/blob/main/taskbar-system-info.wh.cpp).
3. Select **Compile Mod** and enable it.

## Credits and license

Taskbar discovery and thread dispatch follow
[Multirow taskbar for Windows 11](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multirow.wh.cpp)
by Michael Maltsev (`m417z`). Native GPU temperature collection follows his
[Taskbar Clock Customization implementation](https://github.com/m417z/my-windhawk-mods/commit/861920df6380f4c13abec5d9226362c4725e8362).
Secondary-taskbar discovery is adapted from
[Taskbar Fluent Media Player](https://github.com/Salyts/Taskbar-Fluent-Media-Player)
by Salyts.

Released under [GPL-3.0](https://github.com/starychenko/windhawk-taskbar-system-info/blob/main/LICENSE).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 410
  $name: Widget width
  $name:uk-UA: Ширина блока
  $description: "Preferred full-layout width: 330-800 logical pixels. Adaptive layouts use their measured size in the available space."
  $description:uk-UA: "Бажана ширина повного макету: 330-800 логічних пікселів. Адаптивні макети займають місце за виміряними розмірами."

- leftOffset: 10
  $name: Left offset
  $name:uk-UA: Відступ зліва
  $description: "Initial preferred distance from the left taskbar edge in logical pixels. Nonnegative; the nearest readable gap is selected automatically. Changing this setting clears the current display's dragged position."
  $description:uk-UA: "Початковий бажаний відступ зліва в логічних пікселях. Невідємний; автоматично вибирається найближче місце для читабельного блока. Зміна скидає позицію перетягування поточного дисплея."

- monitor: 1
  $name: Taskbar monitor
  $name:uk-UA: Монітор панелі завдань
  $description: "Initial target, range 1-32; 1 is primary. Others follow virtual desktop order, which can differ from Windows numbering. Dragging overrides this target; changing this setting restores manual selection. Missing displays temporarily use the primary taskbar."
  $description:uk-UA: "Початковий монітор, діапазон 1-32; 1 - основний. Інші йдуть за розташуванням у віртуальному робочому столі, номери можуть відрізнятися від Windows. Перетягування змінює ціль; зміна налаштування повертає ручний вибір. Недоступний дисплей тимчасово замінює основна панель."

- moveHotkey: "Ctrl+Alt+M"
  $name: Move widget hotkey
  $name:uk-UA: Клавіша переміщення віджета
  $description: "Move the live widget with a hand cursor; red indicates no space. Drag between taskbars, Enter to save, Esc to cancel, Home to reset. Ctrl/Alt/Shift/Win plus A-Z, 0-9 or F1-F24; empty disables it."
  $description:uk-UA: "Переміщення живого віджета з курсором руки; червоне підсвічування означає брак місця. Перетягуйте між панелями; Enter зберігає, Esc скасовує, Home готує скидання. Ctrl/Alt/Shift/Win та A-Z, 0-9 або F1-F24; порожнє значення вимикає."

- reserveSpace: false
  $name: Reserve space before the Start button
  $name:uk-UA: Резервувати місце перед кнопкою Пуск
  $description: "Allows placement before Start by shifting the button group only if mapped controls still fit. Existing margins are preserved. If arranged controls collide, reservation is rolled back and a free gap is used."
  $description:uk-UA: "Дозволяє місце перед Пуском зі зміщенням групи кнопок, якщо враховані елементи вміщаються. Зовнішні відступи зберігаються. За перетину після layout резервування скасовується та вибирається вільне місце."

- reserveGap: 8
  $name: Reserved space gap
  $name:uk-UA: Проміжок після блока
  $description: "Gap after the reserved widget area, from 0 to 100 logical pixels. The effective minimum is 6 logical pixels."
  $description:uk-UA: "Проміжок після зарезервованої області, від 0 до 100 логічних пікселів. Фактичний мінімум - 6 логічних пікселів."

- updateInterval: 1
  $name: Update interval
  $name:uk-UA: Інтервал оновлення
  $description: "Metric refresh interval, from 1 to 10 seconds. One second is recommended for quick monitoring; a longer interval reduces wakeups."
  $description:uk-UA: "Інтервал від 1 до 10 секунд. Для оперативного моніторингу рекомендована 1 секунда; більший інтервал зменшує кількість оновлень."

- historySeconds: 60
  $name: Graph history
  $name:uk-UA: Історія графіків
  $description: "CPU and GPU graph window, from 15 to 180 seconds. It is sampled at the configured update interval."
  $description:uk-UA: "Проміжок історії графіків CPU і GPU від 15 до 180 секунд. Дані додаються із заданим інтервалом оновлення."

- fontSize: 11
  $name: Font size
  $name:uk-UA: Розмір тексту
  $description: "From 9 to 13 pixels."
  $description:uk-UA: "Від 9 до 13 пікселів."

- fontFamily: "Segoe UI Variable Text"
  $name: Font family
  $name:uk-UA: Шрифт
  $description: "Installed Windows font-family name. The default is tuned for the compact taskbar layout."
  $description:uk-UA: "Назва встановленого у Windows шрифту. Стандартний шрифт підібраний для компактного блока на панелі."

- textColor: ""
  $name: Text color
  $name:uk-UA: Колір тексту
  $description: "#RRGGBB or #AARRGGBB. Leave empty to use the system color. Used when adaptive colors are disabled."
  $description:uk-UA: "#RRGGBB або #AARRGGBB. Порожнє значення використовує системний колір. Застосовується, коли адаптивні кольори вимкнені."

- adaptiveColors: true
  $name: Adapt colors to the taskbar theme
  $name:uk-UA: Адаптувати кольори до теми панелі
  $description: "Automatically uses contrasting light, dark or Windows high-contrast colors for text, graphs and alerts. Disable to use the manual colors below exactly."
  $description:uk-UA: "Автоматично добирає контрастні кольори тексту, графіків і попереджень для світлої, темної або висококонтрастної теми Windows. Вимкніть, щоб точно використовувати ручні кольори нижче."

- graphColor: "#78A8FF"
  $name: Graph and bar color
  $name:uk-UA: Колір графіків і смуг
  $description: "#RRGGBB or #AARRGGBB accent for CPU/GPU history and memory capacity bars. Used when adaptive colors are disabled. Invalid values use the default color."
  $description:uk-UA: "Акцент #RRGGBB або #AARRGGBB для історії CPU/GPU та смуг памяті. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

- warningColor: "#FFFFB900"
  $name: Warning color
  $name:uk-UA: Колір попередження
  $description: "#RRGGBB or #AARRGGBB. Used when adaptive colors are disabled. Invalid values use the default color."
  $description:uk-UA: "#RRGGBB або #AARRGGBB. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

- criticalColor: "#FFFF6B6B"
  $name: Critical color
  $name:uk-UA: Критичний колір
  $description: "#RRGGBB or #AARRGGBB. Used when adaptive colors are disabled. Invalid values use the default color."
  $description:uk-UA: "#RRGGBB або #AARRGGBB. Застосовується, коли адаптивні кольори вимкнені. Для некоректного значення використовується стандартний колір."

- textOpacity: 96
  $name: Text opacity
  $name:uk-UA: Прозорість тексту
  $description: "From 0 to 100 percent. Metric labels are intentionally slightly dimmer; Windows high-contrast mode keeps important content fully visible."
  $description:uk-UA: "Від 0 до 100 відсотків. Підписи показуються трохи тьмяніше; у висококонтрастному режимі важливі значення залишаються повністю видимими."

- cpuWarningTemp: 75
  $name: CPU temperature warning
  $name:uk-UA: Попередження температури CPU
  $description: "From 40 to 95 degrees Celsius."
  $description:uk-UA: "Від 40 до 95 градусів Цельсія."

- cpuCriticalTemp: 85
  $name: CPU critical temperature
  $name:uk-UA: Критична температура CPU
  $description: "From one degree above the warning threshold to 105 degrees Celsius."
  $description:uk-UA: "Від одного градуса вище порога попередження до 105 градусів Цельсія."

- gpuWarningTemp: 80
  $name: GPU temperature warning
  $name:uk-UA: Попередження температури GPU
  $description: "From 40 to 105 degrees Celsius."
  $description:uk-UA: "Від 40 до 105 градусів Цельсія."

- gpuCriticalTemp: 90
  $name: GPU critical temperature
  $name:uk-UA: Критична температура GPU
  $description: "From one degree above the warning threshold to 115 degrees Celsius."
  $description:uk-UA: "Від одного градуса вище порога попередження до 115 градусів Цельсія."

- memoryWarningPercent: 80
  $name: Memory usage warning
  $name:uk-UA: Попередження заповнення памяті
  $description: "From 50 to 98 percent."
  $description:uk-UA: "Від 50 до 98 відсотків."

- memoryCriticalPercent: 90
  $name: Critical memory usage
  $name:uk-UA: Критичне заповнення памяті
  $description: "From one percent above the warning threshold to 100 percent."
  $description:uk-UA: "Від одного відсотка вище порога попередження до 100 відсотків."

- gpuAdapter: ""
  $name: GPU adapter filter
  $name:uk-UA: Відеокарта
  $description: "Optional partial Windows adapter name for multi-GPU systems. Empty selects the adapter with the most dedicated VRAM. GPU usage, VRAM and automatic HWiNFO matching follow this adapter."
  $description:uk-UA: "Необовязкова частина назви адаптера Windows для систем із кількома GPU. Порожнє значення вибирає адаптер з найбільшим обсягом VRAM. Навантаження, VRAM і автоматичний вибір HWiNFO привязуються до цього адаптера."

- gpuMemoryMode: auto
  $name: GPU memory type
  $name:uk-UA: Тип пам'яті GPU
  $description: "Automatic uses shared GPU memory for an integrated adapter and dedicated VRAM for a discrete adapter. The explicit modes override automatic detection."
  $description:uk-UA: "Автоматичний режим використовує спільну GPU-пам'ять для інтегрованого адаптера і виділену VRAM для дискретного. Явний режим перевизначає автоматичне визначення."
  $options:
  - auto: Automatic
  - dedicated: Dedicated VRAM
  - shared: Shared GPU memory
  $options:uk-UA:
  - auto: Автоматично
  - dedicated: Виділена VRAM
  - shared: Спільна GPU-пам'ять

- temperatureSource: auto
  $name: Temperature source
  $name:uk-UA: Джерело температури
  $description: "Automatic is recommended: it tries both HWiNFO interfaces, then Windows D3DKMT for a missing GPU reading and Windows thermal zones for a missing CPU reading. HWiNFO is optional but must be running and configured as explained on the Details page."
  $description:uk-UA: "Рекомендовано Автоматично: режим перевіряє обидва інтерфейси HWiNFO, потім Windows D3DKMT для відсутньої температури GPU та термозони Windows для CPU. HWiNFO необовязковий, але має працювати й бути налаштований за інструкцією на сторінці Деталі."
  $options:
  - auto: Automatic
  - hwinfoAuto: HWiNFO automatic
  - sharedMemory: HWiNFO Shared Memory
  - gadgetRegistry: HWiNFO Gadget Registry
  - windowsNative: Windows native (ACPI CPU + D3DKMT GPU)
  - disabled: Disabled
  $options:uk-UA:
  - auto: Автоматично
  - hwinfoAuto: HWiNFO автоматично
  - sharedMemory: HWiNFO Shared Memory
  - gadgetRegistry: HWiNFO Gadget Registry
  - windowsNative: Системні датчики Windows (ACPI CPU + D3DKMT GPU)
  - disabled: Вимкнено

- windowsThermalZoneFilter: ""
  $name: Windows thermal zone filter
  $name:uk-UA: Фільтр системної термозони Windows
  $description: "Optional partial PDH instance name. Empty uses every valid ACPI thermal zone. Applies to the CPU part of Windows native temperature collection."
  $description:uk-UA: "Необов'язкова частина назви екземпляра PDH. Порожнє значення використовує всі коректні термозони ACPI. Застосовується до CPU у системному режимі Windows."

- windowsThermalZoneAggregation: average
  $name: Windows thermal zone aggregation
  $name:uk-UA: Об'єднання системних термозон Windows
  $description: "Average matches Taskbar Clock Customization. Hottest is safer for alert-oriented monitoring."
  $description:uk-UA: "Середня відповідає Taskbar Clock Customization. Найгарячіша краще підходить для моніторингу попереджень."
  $options:
  - average: Average
  - hottest: Hottest
  $options:uk-UA:
  - average: Середня
  - hottest: Найгарячіша

- cpuTempSensor: ""
  $name: CPU temperature sensor filter
  $name:uk-UA: Датчик температури CPU
  $description: "Optional partial HWiNFO sensor name. Empty automatically selects CPU (Tctl/Tdie), CPU Die, or CPU Package. Set this only when automatic selection chooses the wrong sensor."
  $description:uk-UA: "Необов'язкова частина назви датчика HWiNFO. Порожнє значення автоматично вибирає CPU (Tctl/Tdie), CPU Die або CPU Package. Заповнюйте лише коли автоматично вибрано не той датчик."

- gpuTempSensor: ""
  $name: GPU temperature sensor filter
  $name:uk-UA: Датчик температури GPU
  $description: "Optional partial HWiNFO sensor name. Empty automatically selects GPU Temperature for the selected Windows adapter. Set this only when automatic selection chooses the wrong sensor."
  $description:uk-UA: "Необов'язкова частина назви датчика HWiNFO. Порожнє значення автоматично вибирає GPU Temperature для вибраного адаптера Windows. Заповнюйте лише коли автоматично вибрано не той датчик."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dxgi.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <windows.h>
#include <commctrl.h>
#include <objidl.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <deque>
#include <iterator>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using XamlPath = winrt::Windows::UI::Xaml::Shapes::Path;
using XamlRectangle = winrt::Windows::UI::Xaml::Shapes::Rectangle;

namespace {

constexpr wchar_t kWidgetName[] = L"WindhawkTaskbarSystemInfo";
constexpr double kWidgetHeight = 38.0;
constexpr double kRowHeight = 18.0;
constexpr double kRowGap = 2.0;
constexpr double kColumnGap = 14.0;
constexpr double kWidgetSidePadding = 6.0;
constexpr double kTaskbarClearance = 6.0;
constexpr double kMetricLabelWidth = 28.0;
constexpr double kMetricUsageWidth = 34.0;
constexpr double kMetricTempWidth = 40.0;
constexpr double kGraphLeftGap = 6.0;
constexpr double kMemoryLabelWidth = 43.0;
constexpr double kMemoryPercentWidth = 38.0;
constexpr double kGraphHeight = 12.0;
constexpr double kGiB = 1024.0 * 1024.0 * 1024.0;
constexpr auto kGadgetRegistryRescanInterval = std::chrono::seconds(30);
constexpr auto kSharedMemoryRescanInterval = std::chrono::seconds(60);
constexpr auto kHwInfoUnavailableRetryInterval = std::chrono::seconds(5);
constexpr wchar_t kDefaultGraphColor[] = L"#78A8FF";
constexpr wchar_t kDefaultWarningColor[] = L"#FFFFB900";
constexpr wchar_t kDefaultCriticalColor[] = L"#FFFF6B6B";
constexpr wchar_t kLightGraphColor[] = L"#FF005FB8";
constexpr wchar_t kLightWarningColor[] = L"#FF8A4B00";
constexpr wchar_t kLightCriticalColor[] = L"#FFC42B1C";
constexpr uint32_t kHwInfoSignature = 0x53695748;  // "HWiS"
constexpr uint32_t kHwInfoTemperatureType = 1;

enum class TemperatureSource {
    Auto,
    HwInfoAuto,
    SharedMemory,
    GadgetRegistry,
    WindowsNative,
    Disabled,
};

enum class ThermalZoneAggregation {
    Average,
    Hottest,
};

enum class GpuMemoryMode {
    Auto,
    Dedicated,
    Shared,
};

enum class TemperatureProvider {
    None,
    HwInfoSharedMemory,
    HwInfoGadgetRegistry,
    WindowsD3dkmt,
    WindowsThermalZones,
};

struct ModSettings {
    std::wstring moveHotkey = L"Ctrl+Alt+M";
    std::wstring fontFamily;
    std::wstring textColor;
    std::wstring graphColor;
    std::wstring warningColor;
    std::wstring criticalColor;
    std::wstring gpuAdapter;
    std::wstring cpuTempSensor;
    std::wstring gpuTempSensor;
    std::wstring windowsThermalZoneFilter;
    TemperatureSource temperatureSource = TemperatureSource::Auto;
    ThermalZoneAggregation windowsThermalZoneAggregation =
        ThermalZoneAggregation::Average;
    GpuMemoryMode gpuMemoryMode = GpuMemoryMode::Auto;
    int width = 410;
    int leftOffset = 10;
    int monitor = 1;
    bool adaptiveColors = true;
    bool reserveSpace = false;
    int reserveGap = 8;
    int updateInterval = 1;
    int historySeconds = 60;
    int fontSize = 11;
    int textOpacity = 96;
    int cpuWarningTemp = 75;
    int cpuCriticalTemp = 85;
    int gpuWarningTemp = 80;
    int gpuCriticalTemp = 90;
    int memoryWarningPercent = 80;
    int memoryCriticalPercent = 90;
};

std::shared_ptr<const ModSettings> g_settings{
    std::make_shared<ModSettings>()};
std::mutex g_settingsMutex;
std::atomic<bool> g_unloading;
std::atomic<bool> g_uiTornDown;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_taskbarViewHookAttempted;
std::atomic<HWND> g_taskbarWindow{nullptr};
std::atomic<DWORD> g_taskbarThreadId{0};
std::atomic<bool> g_placementApplyPending{false};
std::atomic<bool> g_taskbarUiResourcesRegistered{false};
std::atomic<HWND> g_notificationWindow{nullptr};
std::atomic<UINT> g_taskbarRefreshMessage{0};
bool g_refreshInProgress = false;
std::mutex g_placementRetryWorkerMutex;
std::atomic<bool> g_stopPlacementRetryWorker{false};
std::atomic<bool> g_placementRetryWorkerRunning{false};
HANDLE g_placementRetryWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_placementRetryWorker;

[[clang::no_destroy]] Grid g_widget{nullptr};
[[clang::no_destroy]] Viewbox g_widgetHost{nullptr};
event_token g_rootSizeChangedToken{};
event_token g_rootLayoutUpdatedToken{};
[[clang::no_destroy]] Grid g_rootGrid{nullptr};
[[clang::no_destroy]] FrameworkElement g_taskItemsRepeater{nullptr};
[[clang::no_destroy]] FrameworkElement g_systemTrayFrame{nullptr};
bool g_applyingTaskbarPlacement = false;
HWND g_placementControlWindow = nullptr;
bool g_geometryQueued = false;
bool g_reservationRejected = false;
bool g_rejectionNeedsBaseline = false;
std::atomic<double> g_previousPlacementLeft{0.0};
uint64_t g_layoutRevision = 0;
uint64_t g_reservationRevision = 0;
bool g_reservationAwaitingLayout = false;
void QueueTaskbarPlacement();
void EnsurePlacementControl(const ModSettings& settings);
void RemovePlacementControl();
void CancelMoveEditor();
void InvalidateMonitorKeys();
std::wstring MonitorKey(HWND window);
std::optional<HWND> DraggedTaskbarWindow();
double PreferredTaskbarLeft(HWND window, double width, const ModSettings& settings);
std::optional<double> SavedTaskbarFraction(HWND window);
double g_reservedMargin = 0.0;
std::optional<double> g_lastAppliedRepeaterMarginLeft;
double g_graphWidth = 96.0;
double g_memoryBarWidth = 120.0;
[[clang::no_destroy]] DispatcherTimer g_timer{nullptr};
event_token g_timerToken{};
event_token g_actualThemeChangedToken{};
HWND g_lastFailedPlacementTarget = nullptr;
bool g_hasFailedPlacementTarget = false;
std::chrono::steady_clock::time_point g_nextPlacementRetry{};
uint32_t g_placementFailures = 0;
bool g_placementIsFallback = false;
bool g_placementLocationUnknown = false;
[[clang::no_destroy]]
std::optional<std::list<FrameworkElement::Loaded_revoker>> g_loadedRevokers{
    std::in_place};

[[clang::no_destroy]] TextBlock g_cpuLabel{nullptr};
[[clang::no_destroy]] TextBlock g_cpuUsageText{nullptr};
[[clang::no_destroy]] TextBlock g_cpuTempText{nullptr};
[[clang::no_destroy]] TextBlock g_gpuLabel{nullptr};
[[clang::no_destroy]] TextBlock g_gpuUsageText{nullptr};
[[clang::no_destroy]] TextBlock g_gpuTempText{nullptr};
[[clang::no_destroy]] TextBlock g_ramLabel{nullptr};
[[clang::no_destroy]] TextBlock g_ramPercentText{nullptr};
[[clang::no_destroy]] TextBlock g_ramCapacityText{nullptr};
[[clang::no_destroy]] TextBlock g_vramLabel{nullptr};
[[clang::no_destroy]] TextBlock g_vramPercentText{nullptr};
[[clang::no_destroy]] TextBlock g_vramCapacityText{nullptr};
[[clang::no_destroy]] XamlPath g_cpuGraph{nullptr};
[[clang::no_destroy]] XamlPath g_gpuGraph{nullptr};
[[clang::no_destroy]] XamlRectangle g_ramTrack{nullptr};
[[clang::no_destroy]] XamlRectangle g_ramFill{nullptr};
[[clang::no_destroy]] XamlRectangle g_vramTrack{nullptr};
[[clang::no_destroy]] XamlRectangle g_vramFill{nullptr};
// All four rows keep their controls when the layout changes.
[[clang::no_destroy]] std::array<Grid, 4> g_metricRows{nullptr, nullptr, nullptr, nullptr};
bool g_layoutGraphsVisible = true;
[[clang::no_destroy]] ColumnDefinition g_leftColumn{nullptr};
[[clang::no_destroy]] ColumnDefinition g_gapColumn{nullptr};
[[clang::no_destroy]] ColumnDefinition g_rightColumn{nullptr};
[[clang::no_destroy]] SolidColorBrush g_textBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_graphBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_warningBrush{nullptr};
[[clang::no_destroy]] SolidColorBrush g_criticalBrush{nullptr};
ElementTheme g_cachedWidgetTheme = ElementTheme::Default;
bool g_themeBrushesInitialized = false;
bool g_cachedHighContrast = false;
COLORREF g_cachedHighlightColor = CLR_INVALID;
COLORREF g_cachedHotlightColor = CLR_INVALID;
COLORREF g_cachedWindowTextColor = CLR_INVALID;

using SampleTime = std::chrono::steady_clock::time_point;

struct HistorySample {
    SampleTime time;
    std::optional<double> value;
};

std::deque<HistorySample> g_cpuHistory;
std::deque<HistorySample> g_gpuHistory;
int g_historyInterval = 0;
int g_historyWindow = 0;

PDH_HQUERY g_pdhQuery = nullptr;  // GPU-only: recovery must not reset CPU counters.
PDH_HQUERY g_cpuPdhQuery = nullptr;
std::chrono::steady_clock::time_point g_nextCpuPdhCounterRetry{};
PDH_HCOUNTER g_cpuUtilityCounter = nullptr;
PDH_HCOUNTER g_gpuCounter = nullptr;
PDH_HCOUNTER g_vramCounter = nullptr;
PDH_HCOUNTER g_sharedVramCounter = nullptr;
PDH_HCOUNTER g_thermalZoneCounter = nullptr;
std::chrono::steady_clock::time_point g_nextPdhCounterRetry{};
std::chrono::steady_clock::time_point g_nextPdhRecovery{};
uint32_t g_consecutivePdhReadFailures = 0;
uint32_t g_consecutiveInvalidGpuSamples = 0;
struct GpuEngineRecoveryProbe {
    PDH_HQUERY query = nullptr;
    PDH_HCOUNTER counter = nullptr;
    LUID adapterLuid{};
    std::chrono::steady_clock::time_point nextSample{};
};
GpuEngineRecoveryProbe g_gpuEngineRecoveryProbe;
bool g_hwInfoInvalidUnitLogged = false;
bool g_hwInfoLayoutRejectedLogged = false;
std::atomic<bool> g_hwInfoGpuAdapterMismatchLogged{false};

struct HwInfoGadgetReadingIdentity {
    std::wstring sensor;
    std::wstring label;
    bool operator==(const HwInfoGadgetReadingIdentity&) const = default;
};

struct HwInfoGadgetRegistryCache {
    std::optional<int> cpuIndex;
    std::optional<int> gpuIndex;
    std::optional<HwInfoGadgetReadingIdentity> cpuIdentity;
    std::optional<HwInfoGadgetReadingIdentity> gpuIdentity;
    unsigned fastRescans = 0;
    std::chrono::steady_clock::time_point nextFullScan{};
    std::wstring cpuFilter;
    std::wstring gpuFilter;
    std::wstring gpuAdapter;
    uint64_t lastWriteTime = 0;
    SampleTime lastWriteChange{};
};

HwInfoGadgetRegistryCache g_hwInfoGadgetRegistryCache;

struct MetricsSnapshot {
    SampleTime capturedAt{};
    double cpu = 0.0;
    bool cpuAvailable = false;
    double ram = 0.0;
    double ramUsedGb = 0.0;
    double ramTotalGb = 0.0;
    bool ramAvailable = false;
    double gpu = 0.0;
    bool gpuAvailable = false;
    double vram = 0.0;
    double vramUsedGb = 0.0;
    double vramTotalGb = 0.0;
    bool vramAvailable = false;
    std::optional<double> cpuTemp;
    std::optional<double> gpuTemp;
    TemperatureProvider cpuTempProvider = TemperatureProvider::None;
    TemperatureProvider gpuTempProvider = TemperatureProvider::None;
};

std::mutex g_metricsMutex;
struct PublishedMetricsSnapshot {
    uint64_t sequence = 0;
    MetricsSnapshot snapshot;
};
std::deque<PublishedMetricsSnapshot> g_publishedMetrics;
uint64_t g_latestMetricsSequence = 0;
uint64_t g_lastRenderedMetricsSequence = 0;
uint64_t g_widgetVisualRevision = 0;
constexpr size_t kMaximumPublishedMetrics = 256;

std::mutex g_metricsWorkerMutex;
std::atomic<bool> g_stopMetricsWorker{false};
HANDLE g_metricsWorkerWakeEvent = nullptr;
[[clang::no_destroy]] std::optional<std::thread> g_metricsWorker;

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

std::optional<Color> ParseColor(const std::wstring& value);

TemperatureSource ParseTemperatureSource(const std::wstring& value) {
    if (value == L"hwinfoAuto") {
        return TemperatureSource::HwInfoAuto;
    }
    if (value == L"sharedMemory") {
        return TemperatureSource::SharedMemory;
    }
    if (value == L"gadgetRegistry") {
        return TemperatureSource::GadgetRegistry;
    }
    if (value == L"windowsNative") {
        return TemperatureSource::WindowsNative;
    }
    if (value == L"disabled") {
        return TemperatureSource::Disabled;
    }
    return TemperatureSource::Auto;
}

ThermalZoneAggregation ParseThermalZoneAggregation(
    const std::wstring& value) {
    return value == L"hottest" ? ThermalZoneAggregation::Hottest
                                : ThermalZoneAggregation::Average;
}

GpuMemoryMode ParseGpuMemoryMode(const std::wstring& value) {
    if (value == L"dedicated") {
        return GpuMemoryMode::Dedicated;
    }
    if (value == L"shared") {
        return GpuMemoryMode::Shared;
    }
    return GpuMemoryMode::Auto;
}

void LoadSettings() {
    ModSettings settings;
    settings.moveHotkey = GetStringSetting(L"moveHotkey");
    settings.fontFamily = GetStringSetting(L"fontFamily");
    settings.textColor = GetStringSetting(L"textColor");
    settings.graphColor = GetStringSetting(L"graphColor");
    settings.warningColor = GetStringSetting(L"warningColor");
    settings.criticalColor = GetStringSetting(L"criticalColor");
    settings.gpuAdapter = GetStringSetting(L"gpuAdapter");
    settings.gpuMemoryMode =
        ParseGpuMemoryMode(GetStringSetting(L"gpuMemoryMode"));
    settings.temperatureSource =
        ParseTemperatureSource(GetStringSetting(L"temperatureSource"));
    settings.cpuTempSensor = GetStringSetting(L"cpuTempSensor");
    settings.gpuTempSensor = GetStringSetting(L"gpuTempSensor");
    settings.windowsThermalZoneFilter =
        GetStringSetting(L"windowsThermalZoneFilter");
    settings.windowsThermalZoneAggregation = ParseThermalZoneAggregation(
        GetStringSetting(L"windowsThermalZoneAggregation"));
    settings.width = std::clamp(Wh_GetIntSetting(L"width"), 330, 800);
    settings.leftOffset = std::max(Wh_GetIntSetting(L"leftOffset"), 0);
    settings.monitor = std::clamp(Wh_GetIntSetting(L"monitor"), 1, 32);
    settings.adaptiveColors = Wh_GetIntSetting(L"adaptiveColors") != 0;
    settings.reserveSpace = Wh_GetIntSetting(L"reserveSpace") != 0;
    settings.reserveGap = std::clamp(Wh_GetIntSetting(L"reserveGap"), 0, 100);
    settings.updateInterval =
        std::clamp(Wh_GetIntSetting(L"updateInterval"), 1, 10);
    settings.historySeconds =
        std::clamp(Wh_GetIntSetting(L"historySeconds"), 15, 180);
    settings.fontSize = std::clamp(Wh_GetIntSetting(L"fontSize"), 9, 13);
    settings.textOpacity =
        std::clamp(Wh_GetIntSetting(L"textOpacity"), 0, 100);
    settings.cpuWarningTemp =
        std::clamp(Wh_GetIntSetting(L"cpuWarningTemp"), 40, 95);
    settings.cpuCriticalTemp = std::clamp(
        Wh_GetIntSetting(L"cpuCriticalTemp"), settings.cpuWarningTemp + 1, 105);
    settings.gpuWarningTemp =
        std::clamp(Wh_GetIntSetting(L"gpuWarningTemp"), 40, 105);
    settings.gpuCriticalTemp = std::clamp(
        Wh_GetIntSetting(L"gpuCriticalTemp"), settings.gpuWarningTemp + 1, 115);
    settings.memoryWarningPercent =
        std::clamp(Wh_GetIntSetting(L"memoryWarningPercent"), 50, 98);
    settings.memoryCriticalPercent =
        std::clamp(Wh_GetIntSetting(L"memoryCriticalPercent"),
                   settings.memoryWarningPercent + 1, 100);

    if (settings.fontFamily.empty()) {
        settings.fontFamily = L"Segoe UI Variable Text";
    }
    if (!settings.textColor.empty() && !ParseColor(settings.textColor)) {
        Wh_Log(L"Invalid text color; using the system color");
        settings.textColor.clear();
    }
    if (!ParseColor(settings.graphColor)) {
        Wh_Log(L"Invalid graph color; using the default");
        settings.graphColor = kDefaultGraphColor;
    }
    if (!ParseColor(settings.warningColor)) {
        Wh_Log(L"Invalid warning color; using the default");
        settings.warningColor = kDefaultWarningColor;
    }
    if (!ParseColor(settings.criticalColor)) {
        Wh_Log(L"Invalid critical color; using the default");
        settings.criticalColor = kDefaultCriticalColor;
    }

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::make_shared<ModSettings>(std::move(settings));
    g_hwInfoGpuAdapterMismatchLogged = false;
}

std::shared_ptr<const ModSettings> CurrentSettings() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t ch) {
        return static_cast<wchar_t>(std::towlower(ch));
    });
    return value;
}

bool Contains(const std::wstring& text, const std::wstring& needle) {
    return needle.empty() || text.find(needle) != std::wstring::npos;
}

std::optional<std::wstring> ResolveGpuTemperatureAdapterName(
    const ModSettings& settings);

std::wstring FixedAnsiToWide(const char* value, size_t capacity) {
    size_t length = 0;
    while (length < capacity && value[length]) {
        length++;
    }
    if (!length) {
        return {};
    }

    int wideLength = MultiByteToWideChar(CP_ACP, 0, value,
                                         static_cast<int>(length), nullptr, 0);
    if (wideLength <= 0) {
        return {};
    }

    std::wstring result(wideLength, L'\0');
    MultiByteToWideChar(CP_ACP, 0, value, static_cast<int>(length),
                        result.data(), wideLength);
    return result;
}

int CpuTemperatureScore(const std::wstring& sensorName,
                        const std::wstring& label,
                        const std::wstring& preferred) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    std::wstring combined = sensor + L" " + reading;
    std::wstring preferredLower = ToLower(preferred);

    if (!preferredLower.empty()) {
        return Contains(combined, preferredLower) ? 10000 : -1;
    }

    bool gpuSensor = Contains(sensor, L"gpu") || Contains(sensor, L"nvidia") ||
                     Contains(sensor, L"radeon");
    bool platformSensor = Contains(sensor, L"pch") ||
                          Contains(sensor, L"chipset");
    bool cpuSensor =
        Contains(sensor, L"cpu") || Contains(sensor, L"processor") ||
        Contains(sensor, L"ryzen") || Contains(sensor, L"threadripper") ||
        Contains(sensor, L"epyc") || Contains(sensor, L"xeon") ||
        (Contains(sensor, L"intel") && Contains(sensor, L"core") &&
         !gpuSensor);
    if (!cpuSensor || platformSensor) {
        return -1;
    }

    if (Contains(reading, L"vrm") || Contains(reading, L"ccd") ||
        Contains(reading, L"iod") || Contains(reading, L"soc") ||
        Contains(reading, L"l3 cache")) {
        return -1;
    }

    if (Contains(reading, L"tctl/tdie")) {
        return 1000;
    }
    if (Contains(reading, L"cpu die (average)")) {
        return 950;
    }
    if (Contains(reading, L"cpu package")) {
        return 900;
    }
    if (Contains(reading, L"package temperature")) {
        return 850;
    }
    if (Contains(reading, L"cpu temperature")) {
        return 800;
    }
    if (Contains(reading, L"core temperatures")) {
        return 700;
    }
    if (Contains(reading, L"temperature")) {
        return 400;
    }

    // Gadget labels follow the selected HWiNFO UI language. A temperature
    // unit check is applied by the registry reader, so a reading that belongs
    // to a recognized CPU sensor remains a safe low-priority fallback even if
    // words such as "temperature" or "package" are localized.
    return 100;
}

std::wstring NormalizeAdapterIdentity(std::wstring value) {
    value = ToLower(std::move(value));
    for (wchar_t& character : value) {
        if (!std::iswalnum(character)) {
            character = L' ';
        }
    }
    std::wstring normalized;
    bool previousSpace = true;
    for (wchar_t character : value) {
        bool space = std::iswspace(character) != 0;
        if (space) {
            if (!previousSpace) {
                normalized.push_back(L' ');
            }
        } else {
            normalized.push_back(character);
        }
        previousSpace = space;
    }
    if (!normalized.empty() && normalized.back() == L' ') {
        normalized.pop_back();
    }

    // Windows adapter names often include trademark markers that HWiNFO omits
    // (for example "Intel(R)" or "Radeon(TM)"). Remove only these standalone
    // tokens so otherwise identical adapter names still match.
    std::wstring filtered;
    size_t start = 0;
    while (start < normalized.size()) {
        size_t end = normalized.find(L' ', start);
        if (end == std::wstring::npos) {
            end = normalized.size();
        }
        std::wstring_view token(normalized.data() + start, end - start);
        if (token != L"r" && token != L"tm") {
            if (!filtered.empty()) {
                filtered.push_back(L' ');
            }
            filtered.append(token);
        }
        start = end + 1;
    }
    return filtered;
}

std::vector<std::wstring> IdentityTokens(const std::wstring& value) {
    std::vector<std::wstring> tokens;
    size_t start = 0;
    while (start < value.size()) {
        size_t end = value.find(L' ', start);
        if (end == std::wstring::npos) {
            end = value.size();
        }
        if (end > start) {
            tokens.emplace_back(value.substr(start, end - start));
        }
        start = end + 1;
    }
    return tokens;
}

bool HasDigit(const std::wstring& value) {
    return std::any_of(value.begin(), value.end(), [](wchar_t character) {
        return std::iswdigit(character) != 0;
    });
}

int GpuAdapterIdentityScore(const std::wstring& sensorName,
                            const std::wstring& adapterName) {
    if (adapterName.empty()) {
        return 0;
    }
    std::wstring sensor = NormalizeAdapterIdentity(sensorName);
    std::wstring adapter = NormalizeAdapterIdentity(adapterName);
    if (adapter.empty()) {
        return 0;
    }
    if (Contains(sensor, adapter)) {
        return 5000;
    }

    auto sensorTokens = IdentityTokens(sensor);
    auto adapterTokens = IdentityTokens(adapter);
    int numericMatches = 0;
    int distinctiveMatches = 0;
    for (const std::wstring& token : adapterTokens) {
        bool matched = std::find(sensorTokens.begin(), sensorTokens.end(),
                                 token) != sensorTokens.end();
        if (!matched) {
            continue;
        }
        if (HasDigit(token)) {
            numericMatches++;
        } else if (token.size() >= 4 && token != L"graphics" &&
                   token != L"radeon" && token != L"geforce" &&
                   token != L"nvidia" && token != L"intel") {
            distinctiveMatches++;
        }
    }
    if (numericMatches) {
        return 4000 + numericMatches * 100 + distinctiveMatches * 10;
    }
    if (distinctiveMatches >= 2) {
        return 3000 + distinctiveMatches * 10;
    }
    return -1;
}

int GpuTemperatureScore(const std::wstring& sensorName,
                        const std::wstring& label,
                        const std::wstring& preferred,
                        const std::optional<std::wstring>& adapterName) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    std::wstring combined = sensor + L" " + reading;
    std::wstring preferredLower = ToLower(preferred);

    if (!preferredLower.empty()) {
        return Contains(combined, preferredLower) ? 10000 : -1;
    }

    if (!adapterName) {
        return -1;
    }

    int adapterScore = GpuAdapterIdentityScore(sensorName, *adapterName);
    if (adapterScore < 0) {
        return -1;
    }

    if (!Contains(sensor, L"gpu") && !Contains(sensor, L"nvidia") &&
        !Contains(sensor, L"radeon")) {
        return -1;
    }

    if (Contains(reading, L"hot spot") || Contains(reading, L"hotspot") ||
        Contains(reading, L"memory") || Contains(reading, L"vram")) {
        return -1;
    }

    if (reading == L"gpu temperature") {
        return adapterScore + 1000;
    }
    if (Contains(reading, L"gpu temperature")) {
        return adapterScore + 950;
    }
    if (Contains(reading, L"gpu core")) {
        return adapterScore + 900;
    }
    if (Contains(reading, L"temperature")) {
        return adapterScore + 500;
    }

    // Prefer the shortest localized label containing the stable GPU acronym.
    // This normally selects labels such as "GPU Temperature" over longer
    // hotspot, junction, or memory-temperature labels.
    if (Contains(reading, L"gpu")) {
        return adapterScore + 400 -
               static_cast<int>(std::min<size_t>(reading.size(), 200));
    }

    // As with CPU readings, the registry path validates the temperature unit
    // before this locale-independent fallback can be selected.
    return adapterScore + 100;
}

struct HwInfoTemperatureDiagnostics {
    bool gpuTemperatureReadingFound = false;
    bool gpuAdapterMatched = false;
};

bool IsGpuTemperatureReadingCandidate(const std::wstring& sensorName,
                                      const std::wstring& label) {
    std::wstring sensor = ToLower(sensorName);
    std::wstring reading = ToLower(label);
    if (!Contains(sensor, L"gpu") && !Contains(sensor, L"nvidia") &&
        !Contains(sensor, L"radeon")) {
        return false;
    }
    return !Contains(reading, L"hot spot") &&
           !Contains(reading, L"hotspot") &&
           !Contains(reading, L"memory") && !Contains(reading, L"vram");
}

void RecordGpuTemperatureDiagnostic(
    HwInfoTemperatureDiagnostics& diagnostics,
    const std::wstring& sensorName,
    const std::wstring& label,
    const std::optional<std::wstring>& adapterName) {
    if (!IsGpuTemperatureReadingCandidate(sensorName, label)) {
        return;
    }
    diagnostics.gpuTemperatureReadingFound = true;
    if (adapterName &&
        GpuAdapterIdentityScore(sensorName, *adapterName) >= 0) {
        diagnostics.gpuAdapterMatched = true;
    }
}

// HWiNFO's published shared-memory layout explicitly uses one-byte packing.
#pragma pack(push, 1)
struct HwInfoHeader {
    uint32_t signature;
    uint32_t version;
    uint32_t revision;
    int64_t pollTime;
    uint32_t sensorOffset;
    uint32_t sensorStride;
    uint32_t sensorCount;
    uint32_t readingOffset;
    uint32_t readingStride;
    uint32_t readingCount;
    uint32_t pollingPeriod;
};

struct HwInfoSensorPrefix {
    uint32_t sensorId;
    uint32_t sensorInstance;
    char originalName[128];
    char userName[128];
};

struct HwInfoReadingPrefix {
    uint32_t readingType;
    uint32_t sensorIndex;
    uint32_t readingId;
    char originalLabel[128];
    char userLabel[128];
    char unit[16];
    double value;
};
#pragma pack(pop)

static_assert(sizeof(HwInfoHeader) == 48);
static_assert(offsetof(HwInfoHeader, pollTime) == 12);
static_assert(offsetof(HwInfoHeader, sensorOffset) == 20);
static_assert(sizeof(HwInfoSensorPrefix) == 264);
static_assert(offsetof(HwInfoReadingPrefix, value) == 284);
static_assert(sizeof(HwInfoReadingPrefix) == 292);

struct HwInfoReadingIdentity {
    uint32_t sensorId;
    uint32_t sensorInstance;
    uint32_t readingId;
    bool operator==(const HwInfoReadingIdentity&) const = default;
};

struct HwInfoSharedMemoryCache {
    std::optional<uint32_t> cpuReadingIndex;
    std::optional<uint32_t> gpuReadingIndex;
    std::optional<HwInfoReadingIdentity> cpuIdentity;
    std::optional<HwInfoReadingIdentity> gpuIdentity;
    unsigned fastRescans = 0;
    std::chrono::steady_clock::time_point nextFullScan{};
    std::wstring cpuFilter;
    std::wstring gpuFilter;
    std::wstring gpuAdapter;
    int64_t lastPollTime = 0;
    SampleTime lastPollChange{};
};

bool HwInfoPublicationIsFresh(const HwInfoHeader& header,
                              HwInfoSharedMemoryCache& cache,
                              int64_t unixSeconds,
                              SampleTime now) {
    // pollTime is a Unix timestamp in whole seconds; pollingPeriod is in ms.
    // Older revisions lack pollingPeriod. Allow three polls plus rounding and
    // scheduling slack, without mistaking an unchanged temperature for a stall.
    auto period = header.revision >= 1 && header.pollingPeriod
                      ? header.pollingPeriod : 2000u;
    auto grace = std::chrono::milliseconds(
        std::max<int64_t>(5000, int64_t{period} * 3 + 2000));
    int64_t graceSeconds = (grace.count() + 999) / 1000;
    if (header.pollTime <= 0 || header.pollTime > unixSeconds + 2 ||
        header.pollTime < unixSeconds - graceSeconds) {
        return false;
    }
    if (cache.lastPollTime != header.pollTime) {
        cache.lastPollTime = header.pollTime;
        cache.lastPollChange = now;
    }
    // The monotonic limit still expires a frozen publication after a clock
    // adjustment makes its wall-clock timestamp appear recent again.
    return now >= cache.lastPollChange && now - cache.lastPollChange <= grace;
}

struct HwInfoRawTemperatureReading {
    uint32_t index = 0;
    HwInfoSensorPrefix sensor{};
    HwInfoReadingPrefix reading{};
};

HwInfoSharedMemoryCache g_hwInfoSharedMemoryCache;

// A short discovery window, not a permanent five-second full-table scan.
std::chrono::seconds HwInfoRescanDelay(bool complete,
                                       unsigned& fastRescans,
                                       std::chrono::seconds regularInterval) {
    if (complete) {
        fastRescans = 0;
        return regularInterval;
    }
    if (fastRescans < 4) {
        ++fastRescans;
        return kHwInfoUnavailableRetryInterval;
    }
    return regularInterval;
}

HwInfoReadingIdentity ReadingIdentity(const HwInfoSensorPrefix& sensor,
                                       const HwInfoReadingPrefix& reading) {
    return {sensor.sensorId, sensor.sensorInstance, reading.readingId};
}

bool IsRangeValid(size_t totalSize,
                  uint32_t offset,
                  uint32_t stride,
                  uint32_t count,
                  size_t minimumStride) {
    if (stride < minimumStride || offset > totalSize) {
        return false;
    }
    size_t remaining = totalSize - offset;
    return count <= remaining / stride;
}

std::optional<double> NormalizeTemperature(double value,
                                           std::wstring unitOrFormattedValue) {
    if (!std::isfinite(value)) {
        return std::nullopt;
    }

    std::wstring unit = ToLower(unitOrFormattedValue);
    unit.erase(std::remove_if(unit.begin(), unit.end(), [](wchar_t character) {
                   return std::iswspace(character) != 0;
               }),
               unit.end());

    bool celsius = Contains(unit, L"\u00B0c") || Contains(unit, L"\u2103") ||
                   unit == L"c" || unit == L"celsius";
    bool fahrenheit =
        Contains(unit, L"\u00B0f") || Contains(unit, L"\u2109") ||
        unit == L"f" || unit == L"fahrenheit";
    if (celsius == fahrenheit) {
        return std::nullopt;
    }

    double celsiusValue =
        fahrenheit ? (value - 32.0) * 5.0 / 9.0 : value;
    if (!std::isfinite(celsiusValue) || celsiusValue < -50.0 ||
        celsiusValue > 200.0) {
        return std::nullopt;
    }
    return celsiusValue;
}

constexpr char HwInfoTemperatureUnit(const char* unit, size_t capacity) {
    char result = 0;
    for (size_t i = 0; i < capacity && unit[i]; i++) {
        char candidate = 0;
        if (unit[i] == 'C' || unit[i] == 'c') {
            candidate = 'C';
        } else if (unit[i] == 'F' || unit[i] == 'f') {
            candidate = 'F';
        }
        if (candidate) {
            if (result && result != candidate) {
                return 0;
            }
            result = candidate;
        }
    }
    return result;
}

constexpr char kHwInfoRawCelsiusUnit[] = {
    static_cast<char>(0xB0), 'C', 0};
constexpr char kHwInfoRawFahrenheitUnit[] = {
    static_cast<char>(0xB0), 'F', 0};
static_assert(HwInfoTemperatureUnit(kHwInfoRawCelsiusUnit,
                                    std::size(kHwInfoRawCelsiusUnit)) == 'C');
static_assert(HwInfoTemperatureUnit(kHwInfoRawFahrenheitUnit,
                                    std::size(kHwInfoRawFahrenheitUnit)) ==
              'F');

std::optional<double> NormalizeHwInfoTemperature(double value,
                                                 const char* unit,
                                                 size_t capacity) {
    // HWiNFO stores a raw single-byte degree sign followed by an ASCII unit
    // letter. Decoding the buffer through CP_ACP corrupts that sequence on
    // DBCS locales, so classify the ASCII letter directly from the bytes.
    char unitLetter = HwInfoTemperatureUnit(unit, capacity);
    if (!unitLetter) {
        return std::nullopt;
    }
    return NormalizeTemperature(value, unitLetter == 'F' ? L"F" : L"C");
}

void ReadHwInfoSharedMemory(MetricsSnapshot& snapshot,
                            const ModSettings& settings,
                            const std::optional<std::wstring>& gpuAdapterName,
                            HwInfoTemperatureDiagnostics& diagnostics) {
    std::wstring gpuAdapter = gpuAdapterName.value_or(L"");
    if (g_hwInfoSharedMemoryCache.cpuFilter != settings.cpuTempSensor ||
        g_hwInfoSharedMemoryCache.gpuFilter != settings.gpuTempSensor ||
        g_hwInfoSharedMemoryCache.gpuAdapter != gpuAdapter) {
        g_hwInfoSharedMemoryCache = {};
        g_hwInfoSharedMemoryCache.cpuFilter = settings.cpuTempSensor;
        g_hwInfoSharedMemoryCache.gpuFilter = settings.gpuTempSensor;
        g_hwInfoSharedMemoryCache.gpuAdapter = std::move(gpuAdapter);
    }

    HANDLE mapping = OpenFileMappingW(FILE_MAP_READ, FALSE,
                                      L"Global\\HWiNFO_SENS_SM2");
    if (!mapping) {
        return;
    }

    HANDLE mutex = OpenMutexW(SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE,
                              L"Global\\HWiNFO_SM2_MUTEX");
    bool mutexOwned = false;
    if (mutex) {
        DWORD waitResult = WaitForSingleObject(mutex, 50);
        if (waitResult == WAIT_OBJECT_0 || waitResult == WAIT_ABANDONED) {
            mutexOwned = true;
        } else {
            CloseHandle(mutex);
            CloseHandle(mapping);
            return;
        }
    }

    void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    if (!view) {
        if (mutexOwned) {
            ReleaseMutex(mutex);
        }
        if (mutex) {
            CloseHandle(mutex);
        }
        CloseHandle(mapping);
        return;
    }

    std::vector<HwInfoSensorPrefix> sensors;
    std::vector<HwInfoReadingPrefix> readings;
    std::optional<HwInfoRawTemperatureReading> cachedCpuReading;
    std::optional<HwInfoRawTemperatureReading> cachedGpuReading;
    bool fullScanCopied = false;
    std::optional<HwInfoHeader> publicationHeader;
    auto now = std::chrono::steady_clock::now();
    auto unixSeconds = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    MEMORY_BASIC_INFORMATION memoryInfo{};
    if (VirtualQuery(view, &memoryInfo, sizeof(memoryInfo))) {
        size_t viewOffset = static_cast<const uint8_t*>(view) -
                            static_cast<const uint8_t*>(memoryInfo.BaseAddress);
        size_t mappedSize = viewOffset <= memoryInfo.RegionSize
                                ? memoryInfo.RegionSize - viewOffset
                                : 0;
        if (mappedSize >= sizeof(HwInfoHeader)) {
            HwInfoHeader header{};
            std::memcpy(&header, view, sizeof(header));
            bool validLayout = header.signature == kHwInfoSignature &&
                IsRangeValid(mappedSize, header.sensorOffset,
                             header.sensorStride, header.sensorCount,
                             sizeof(HwInfoSensorPrefix)) &&
                IsRangeValid(mappedSize, header.readingOffset,
                             header.readingStride, header.readingCount,
                             sizeof(HwInfoReadingPrefix));
            if (validLayout) g_hwInfoLayoutRejectedLogged = false;
            if (validLayout &&
                HwInfoPublicationIsFresh(header, g_hwInfoSharedMemoryCache,
                                          unixSeconds, now)) {
                const auto* bytes = static_cast<const uint8_t*>(view);
                bool performFullScan =
                    g_hwInfoSharedMemoryCache.nextFullScan ==
                        std::chrono::steady_clock::time_point{} ||
                    now >= g_hwInfoSharedMemoryCache.nextFullScan;

                auto copyCachedReading =
                    [&](std::optional<uint32_t>& cachedIndex,
                        const std::optional<HwInfoReadingIdentity>& identity,
                        std::optional<HwInfoRawTemperatureReading>& output) {
                        if (!cachedIndex || performFullScan) {
                            return;
                        }
                        if (*cachedIndex >= header.readingCount) {
                            cachedIndex.reset();
                            g_hwInfoSharedMemoryCache.nextFullScan = {};
                            performFullScan = true;
                            return;
                        }

                        HwInfoRawTemperatureReading raw{};
                        raw.index = *cachedIndex;
                        const uint8_t* readingAddress =
                            bytes + header.readingOffset +
                            static_cast<size_t>(raw.index) *
                                header.readingStride;
                        std::memcpy(&raw.reading, readingAddress,
                                    sizeof(raw.reading));
                        if (raw.reading.readingType !=
                                kHwInfoTemperatureType ||
                            raw.reading.sensorIndex >= header.sensorCount) {
                            cachedIndex.reset();
                            g_hwInfoSharedMemoryCache.nextFullScan = {};
                            performFullScan = true;
                            return;
                        }
                        const uint8_t* sensorAddress =
                            bytes + header.sensorOffset +
                            static_cast<size_t>(raw.reading.sensorIndex) *
                                header.sensorStride;
                        std::memcpy(&raw.sensor, sensorAddress,
                                    sizeof(raw.sensor));
                        if (!identity ||
                            ReadingIdentity(raw.sensor, raw.reading) != *identity) {
                            cachedIndex.reset();
                            performFullScan = true;
                            return;
                        }
                        output = raw;
                    };

                copyCachedReading(
                    g_hwInfoSharedMemoryCache.cpuReadingIndex,
                    g_hwInfoSharedMemoryCache.cpuIdentity, cachedCpuReading);
                copyCachedReading(
                    g_hwInfoSharedMemoryCache.gpuReadingIndex,
                    g_hwInfoSharedMemoryCache.gpuIdentity, cachedGpuReading);

                if (performFullScan) {
                    cachedCpuReading.reset();
                    cachedGpuReading.reset();
                    sensors.resize(header.sensorCount);
                    readings.resize(header.readingCount);
                    for (uint32_t i = 0; i < header.sensorCount; i++) {
                        const uint8_t* address =
                            bytes + header.sensorOffset +
                            static_cast<size_t>(i) * header.sensorStride;
                        std::memcpy(&sensors[i], address, sizeof(sensors[i]));
                    }
                    for (uint32_t i = 0; i < header.readingCount; i++) {
                        const uint8_t* address =
                            bytes + header.readingOffset +
                            static_cast<size_t>(i) * header.readingStride;
                        std::memcpy(&readings[i], address,
                                    sizeof(readings[i]));
                    }
                    fullScanCopied = true;
                }

                if (!mutexOwned) {
                    HwInfoHeader verificationHeader{};
                    std::memcpy(&verificationHeader, view,
                                sizeof(verificationHeader));
                    if (std::memcmp(&header, &verificationHeader,
                                    sizeof(header)) != 0) {
                        sensors.clear();
                        readings.clear();
                        cachedCpuReading.reset();
                        cachedGpuReading.reset();
                        fullScanCopied = false;
                    } else {
                        publicationHeader = header;
                    }
                } else {
                    publicationHeader = header;
                }
            } else if (!validLayout && !g_hwInfoLayoutRejectedLogged) {
                Wh_Log(L"HWiNFO layout rejected: version=%u revision=%u "
                       L"sensors=%u@%u/%u readings=%u@%u/%u",
                       header.version, header.revision, header.sensorCount,
                       header.sensorOffset, header.sensorStride,
                       header.readingCount, header.readingOffset,
                       header.readingStride);
                g_hwInfoLayoutRejectedLogged = true;
            }
        } else if (!g_hwInfoLayoutRejectedLogged) {
            Wh_Log(L"HWiNFO layout rejected: mapping too small (%llu bytes)",
                   static_cast<unsigned long long>(mappedSize));
            g_hwInfoLayoutRejectedLogged = true;
        }
    }

    UnmapViewOfFile(view);
    if (mutexOwned) {
        ReleaseMutex(mutex);
    }
    if (mutex) {
        CloseHandle(mutex);
    }
    CloseHandle(mapping);

    unixSeconds = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    if (!publicationHeader ||
        !HwInfoPublicationIsFresh(*publicationHeader, g_hwInfoSharedMemoryCache,
                                  unixSeconds, SampleTime::clock::now())) {
        return;
    }

    bool sawTemperatureReading = false;
    bool sawSupportedTemperatureUnit = false;
    if (fullScanCopied) {
        // The full table is copied only for discovery or periodic refresh.
        // Normal samples below validate and read the two cached records.
        int bestCpuScore = -1;
        int bestGpuScore = -1;
        std::optional<uint32_t> bestCpuIndex;
        std::optional<uint32_t> bestGpuIndex;
        for (uint32_t i = 0; i < readings.size(); i++) {
            const HwInfoReadingPrefix& reading = readings[i];
            if (reading.readingType != kHwInfoTemperatureType ||
                reading.sensorIndex >= sensors.size()) {
                continue;
            }
            sawTemperatureReading = true;

            auto value = NormalizeHwInfoTemperature(
                reading.value, reading.unit, std::size(reading.unit));
            if (!value) {
                continue;
            }
            sawSupportedTemperatureUnit = true;

            const HwInfoSensorPrefix& sensor = sensors[reading.sensorIndex];
            std::wstring sensorName = FixedAnsiToWide(
                sensor.originalName, std::size(sensor.originalName));
            std::wstring label = FixedAnsiToWide(
                reading.originalLabel, std::size(reading.originalLabel));
            RecordGpuTemperatureDiagnostic(diagnostics, sensorName, label,
                                           gpuAdapterName);

            int cpuScore = CpuTemperatureScore(
                sensorName, label, settings.cpuTempSensor);
            if (cpuScore > bestCpuScore) {
                bestCpuScore = cpuScore;
                bestCpuIndex = i;
                snapshot.cpuTemp = *value;
                snapshot.cpuTempProvider =
                    TemperatureProvider::HwInfoSharedMemory;
            }

            int gpuScore = GpuTemperatureScore(
                sensorName, label, settings.gpuTempSensor, gpuAdapterName);
            if (gpuScore > bestGpuScore) {
                bestGpuScore = gpuScore;
                bestGpuIndex = i;
                snapshot.gpuTemp = *value;
                snapshot.gpuTempProvider =
                    TemperatureProvider::HwInfoSharedMemory;
            }
        }

        g_hwInfoSharedMemoryCache.cpuReadingIndex = bestCpuIndex;
        g_hwInfoSharedMemoryCache.gpuReadingIndex = bestGpuIndex;
        auto identityAt = [&](std::optional<uint32_t> index)
            -> std::optional<HwInfoReadingIdentity> {
            if (!index) {
                return std::nullopt;
            }
            const auto& reading = readings[*index];
            return ReadingIdentity(sensors[reading.sensorIndex], reading);
        };
        g_hwInfoSharedMemoryCache.cpuIdentity = identityAt(bestCpuIndex);
        g_hwInfoSharedMemoryCache.gpuIdentity = identityAt(bestGpuIndex);
        g_hwInfoSharedMemoryCache.nextFullScan =
            now + HwInfoRescanDelay(bestCpuIndex && bestGpuIndex,
                                   g_hwInfoSharedMemoryCache.fastRescans,
                                   kSharedMemoryRescanInterval);
    } else {
        auto applyCachedReading =
            [&](const std::optional<HwInfoRawTemperatureReading>& raw,
                std::optional<uint32_t>& cachedIndex, bool cpu) {
                if (!raw) {
                    return;
                }
                sawTemperatureReading = true;

                const HwInfoReadingPrefix& reading = raw->reading;
                std::wstring sensorName = FixedAnsiToWide(
                    raw->sensor.originalName,
                    std::size(raw->sensor.originalName));
                std::wstring label = FixedAnsiToWide(
                    reading.originalLabel,
                    std::size(reading.originalLabel));
                RecordGpuTemperatureDiagnostic(diagnostics, sensorName, label,
                                               gpuAdapterName);
                int score = cpu
                                ? CpuTemperatureScore(
                                      sensorName, label,
                                      settings.cpuTempSensor)
                                : GpuTemperatureScore(
                                      sensorName, label,
                                      settings.gpuTempSensor,
                                      gpuAdapterName);
                if (score < 0) {
                    cachedIndex.reset();
                    g_hwInfoSharedMemoryCache.nextFullScan = {};
                    return;
                }

                auto value = NormalizeHwInfoTemperature(
                    reading.value, reading.unit, std::size(reading.unit));
                if (!value) {
                    cachedIndex.reset();
                    g_hwInfoSharedMemoryCache.nextFullScan = {};
                    return;
                }
                sawSupportedTemperatureUnit = true;

                if (cpu) {
                    snapshot.cpuTemp = *value;
                    snapshot.cpuTempProvider =
                        TemperatureProvider::HwInfoSharedMemory;
                } else {
                    snapshot.gpuTemp = *value;
                    snapshot.gpuTempProvider =
                        TemperatureProvider::HwInfoSharedMemory;
                }
            };

        applyCachedReading(cachedCpuReading,
                           g_hwInfoSharedMemoryCache.cpuReadingIndex,
                           true);
        applyCachedReading(cachedGpuReading,
                           g_hwInfoSharedMemoryCache.gpuReadingIndex,
                           false);
    }

    if (sawTemperatureReading && !sawSupportedTemperatureUnit &&
        !g_hwInfoInvalidUnitLogged) {
        Wh_Log(L"HWiNFO temperature readings use an unsupported unit");
        g_hwInfoInvalidUnitLogged = true;
    }
}

std::optional<std::wstring> ReadRegistryString(HKEY key,
                                                const std::wstring& name) {
    DWORD type = 0;
    DWORD bytes = 0;
    LONG status = RegQueryValueExW(key, name.c_str(), nullptr, &type, nullptr,
                                   &bytes);
    if (status != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ) || bytes < sizeof(wchar_t)) {
        return std::nullopt;
    }

    std::vector<wchar_t> buffer(bytes / sizeof(wchar_t) + 1, L'\0');
    status = RegQueryValueExW(key, name.c_str(), nullptr, &type,
                              reinterpret_cast<BYTE*>(buffer.data()), &bytes);
    if (status != ERROR_SUCCESS) {
        return std::nullopt;
    }
    return std::wstring(buffer.data());
}

std::optional<double> ParseLocalizedDouble(std::wstring value) {
    std::replace(value.begin(), value.end(), L',', L'.');
    size_t start = value.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) {
        return std::nullopt;
    }
    if (value[start] == L'+') {
        ++start;
    }
    std::string ascii;
    for (size_t i = start; i < value.size() && value[i] <= 127; ++i) {
        ascii.push_back(static_cast<char>(value[i]));
    }
    double result = 0;
    auto parsed = std::from_chars(ascii.data(), ascii.data() + ascii.size(),
                                  result);
    if (parsed.ec != std::errc{} || !std::isfinite(result)) {
        return std::nullopt;
    }
    return result;
}

std::optional<double> NormalizeRegistryTemperature(
    const std::wstring& rawValue,
    const std::wstring& formattedValue) {
    auto value = ParseLocalizedDouble(rawValue);
    if (!value) {
        return std::nullopt;
    }

    return NormalizeTemperature(*value, formattedValue);
}

struct HwInfoGadgetReading {
    std::wstring sensor;
    std::wstring label;
    double value = 0.0;
};

std::vector<int> HwInfoRegistryIndices(HKEY key) {
    std::vector<int> indices;
    // Enumerate existing values instead of stopping at an arbitrary hole in
    // SensorN numbering. This scan only runs on discovery/identity invalidation.
    for (DWORD valueIndex = 0;; ++valueIndex) {
        wchar_t name[64];
        DWORD length = static_cast<DWORD>(std::size(name));
        LSTATUS status = RegEnumValueW(key, valueIndex, name, &length, nullptr,
                                       nullptr, nullptr, nullptr);
        if (status == ERROR_NO_MORE_ITEMS) {
            break;
        }
        if (status == ERROR_MORE_DATA) {
            continue;  // A name this long can't be Sensor + an int index.
        }
        if (status != ERROR_SUCCESS) {
            break;
        }
        std::wstring_view valueName(name, length);
        if (!valueName.starts_with(L"Sensor") || length <= 6) {
            continue;
        }
        std::string digits;
        for (DWORD i = 6; i < length && name[i] >= L'0' && name[i] <= L'9'; ++i) {
            digits.push_back(static_cast<char>(name[i]));
        }
        int index = 0;
        auto parsed = std::from_chars(digits.data(), digits.data() + digits.size(), index);
        if (digits.size() == length - 6 && parsed.ec == std::errc{}) {
            indices.push_back(index);
        }
    }
    std::sort(indices.begin(), indices.end());
    indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
    return indices;
}

std::optional<HwInfoGadgetReading> ReadHwInfoGadgetReading(HKEY key,
                                                           int index) {
    std::wstring suffix = std::to_wstring(index);
    auto sensor = ReadRegistryString(key, L"Sensor" + suffix);
    auto label = ReadRegistryString(key, L"Label" + suffix);
    auto rawValue = ReadRegistryString(key, L"ValueRaw" + suffix);
    auto formattedValue = ReadRegistryString(key, L"Value" + suffix);
    if (!sensor || !label || !rawValue || !formattedValue) {
        return std::nullopt;
    }

    auto value = NormalizeRegistryTemperature(*rawValue, *formattedValue);
    if (!value) {
        return std::nullopt;
    }
    return HwInfoGadgetReading{std::move(*sensor), std::move(*label), *value};
}

uint64_t FileTimeValue(const FILETIME& value);

std::optional<uint64_t> ReadRegistryWriteTime(HKEY key) {
    FILETIME lastWrite{};
    if (RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, nullptr, nullptr,
                        nullptr, nullptr, nullptr, nullptr, nullptr,
                        &lastWrite) != ERROR_SUCCESS) {
        return std::nullopt;
    }
    return FileTimeValue(lastWrite);
}

constexpr auto kGadgetRegistryFreshnessLimit = std::chrono::seconds(60);

bool HwInfoRegistryPublicationIsFresh(uint64_t lastWrite,
                                      uint64_t wallNow,
                                      SampleTime now,
                                      HwInfoGadgetRegistryCache& cache) {
    constexpr uint64_t ticksPerSecond = 10000000;
    constexpr auto maximumAge = kGadgetRegistryFreshnessLimit.count() * ticksPerSecond;
    if (!lastWrite ||
        (lastWrite > wallNow && lastWrite - wallNow > 2 * ticksPerSecond) ||
        (lastWrite <= wallNow && wallNow - lastWrite > maximumAge)) {
        return false;
    }
    if (cache.lastWriteTime != lastWrite) {
        cache.lastWriteTime = lastWrite;
        cache.lastWriteChange = now;
    }
    return now >= cache.lastWriteChange &&
           now - cache.lastWriteChange <= kGadgetRegistryFreshnessLimit;
}

void ReadHwInfoGadgetRegistryValues(HKEY key,
                                    MetricsSnapshot& snapshot,
                                    const ModSettings& settings,
                                    const std::optional<std::wstring>& gpuAdapterName,
                                    HwInfoTemperatureDiagnostics& diagnostics,
                                    SampleTime now) {
    bool needsCpu = !snapshot.cpuTemp;
    bool needsGpu = !snapshot.gpuTemp;
    bool performFullScan =
        g_hwInfoGadgetRegistryCache.nextFullScan ==
            std::chrono::steady_clock::time_point{} ||
        now >= g_hwInfoGadgetRegistryCache.nextFullScan;

    auto readCached =
        [&](std::optional<int>& cachedIndex,
            const std::optional<HwInfoGadgetReadingIdentity>& identity,
            bool cpu, bool needed) {
        if (!needed || !cachedIndex || performFullScan) {
            return;
        }
        auto reading = ReadHwInfoGadgetReading(key, *cachedIndex);
        if (!reading || !identity ||
            HwInfoGadgetReadingIdentity{reading->sensor, reading->label} !=
                *identity) {
            cachedIndex.reset();
            performFullScan = true;
            return;
        }

        RecordGpuTemperatureDiagnostic(diagnostics, reading->sensor,
                                       reading->label, gpuAdapterName);
        int score = cpu ? CpuTemperatureScore(reading->sensor, reading->label,
                                              settings.cpuTempSensor)
                        : GpuTemperatureScore(reading->sensor, reading->label,
                                              settings.gpuTempSensor,
                                              gpuAdapterName);
        if (score < 0) {
            cachedIndex.reset();
            performFullScan = true;
            return;
        }

        if (cpu && !snapshot.cpuTemp) {
            snapshot.cpuTemp = reading->value;
            snapshot.cpuTempProvider =
                TemperatureProvider::HwInfoGadgetRegistry;
        } else if (!cpu && !snapshot.gpuTemp) {
            snapshot.gpuTemp = reading->value;
            snapshot.gpuTempProvider =
                TemperatureProvider::HwInfoGadgetRegistry;
        }
    };

    readCached(g_hwInfoGadgetRegistryCache.cpuIndex,
               g_hwInfoGadgetRegistryCache.cpuIdentity, true, needsCpu);
    readCached(g_hwInfoGadgetRegistryCache.gpuIndex,
               g_hwInfoGadgetRegistryCache.gpuIdentity, false, needsGpu);
    if (!performFullScan) {
        return;
    }

    int bestCpuScore = -1;
    int bestGpuScore = -1;
    std::optional<int> bestCpuIndex;
    std::optional<int> bestGpuIndex;
    std::optional<double> bestCpuValue;
    std::optional<double> bestGpuValue;
    std::optional<HwInfoGadgetReadingIdentity> bestCpuIdentity;
    std::optional<HwInfoGadgetReadingIdentity> bestGpuIdentity;
    for (int i : HwInfoRegistryIndices(key)) {
        std::wstring suffix = std::to_wstring(i);
        auto sensor = ReadRegistryString(key, L"Sensor" + suffix);
        if (!sensor) {
            continue;
        }
        auto label = ReadRegistryString(key, L"Label" + suffix);
        auto rawValue = ReadRegistryString(key, L"ValueRaw" + suffix);
        auto formattedValue = ReadRegistryString(key, L"Value" + suffix);
        if (!label || !rawValue || !formattedValue) {
            continue;
        }
        auto value =
            NormalizeRegistryTemperature(*rawValue, *formattedValue);
        if (!value) {
            continue;
        }
        RecordGpuTemperatureDiagnostic(diagnostics, *sensor, *label,
                                       gpuAdapterName);

        int cpuScore =
            CpuTemperatureScore(*sensor, *label, settings.cpuTempSensor);
        if (cpuScore > bestCpuScore) {
            bestCpuScore = cpuScore;
            bestCpuIndex = i;
            bestCpuValue = *value;
            bestCpuIdentity = HwInfoGadgetReadingIdentity{*sensor, *label};
        }

        int gpuScore =
            GpuTemperatureScore(*sensor, *label, settings.gpuTempSensor,
                                gpuAdapterName);
        if (gpuScore > bestGpuScore) {
            bestGpuScore = gpuScore;
            bestGpuIndex = i;
            bestGpuValue = *value;
            bestGpuIdentity = HwInfoGadgetReadingIdentity{*sensor, *label};
        }
    }

    g_hwInfoGadgetRegistryCache.cpuIndex = bestCpuIndex;
    g_hwInfoGadgetRegistryCache.gpuIndex = bestGpuIndex;
    g_hwInfoGadgetRegistryCache.cpuIdentity = std::move(bestCpuIdentity);
    g_hwInfoGadgetRegistryCache.gpuIdentity = std::move(bestGpuIdentity);
    g_hwInfoGadgetRegistryCache.nextFullScan =
        now + HwInfoRescanDelay((!needsCpu || bestCpuIndex) &&
                                   (!needsGpu || bestGpuIndex),
                               g_hwInfoGadgetRegistryCache.fastRescans,
                               kGadgetRegistryRescanInterval);

    if (!snapshot.cpuTemp && bestCpuValue) {
        snapshot.cpuTemp = *bestCpuValue;
        snapshot.cpuTempProvider = TemperatureProvider::HwInfoGadgetRegistry;
    }
    if (!snapshot.gpuTemp && bestGpuValue) {
        snapshot.gpuTemp = *bestGpuValue;
        snapshot.gpuTempProvider = TemperatureProvider::HwInfoGadgetRegistry;
    }
}

void ReadHwInfoGadgetRegistry(MetricsSnapshot& snapshot,
                              const ModSettings& settings,
                              const std::optional<std::wstring>& gpuAdapterName,
                              HwInfoTemperatureDiagnostics& diagnostics) {
    std::wstring gpuAdapter = gpuAdapterName.value_or(L"");
    if (g_hwInfoGadgetRegistryCache.cpuFilter != settings.cpuTempSensor ||
        g_hwInfoGadgetRegistryCache.gpuFilter != settings.gpuTempSensor ||
        g_hwInfoGadgetRegistryCache.gpuAdapter != gpuAdapter) {
        g_hwInfoGadgetRegistryCache = {};
        g_hwInfoGadgetRegistryCache.cpuFilter = settings.cpuTempSensor;
        g_hwInfoGadgetRegistryCache.gpuFilter = settings.gpuTempSensor;
        g_hwInfoGadgetRegistryCache.gpuAdapter = std::move(gpuAdapter);
    }
    auto now = SampleTime::clock::now();
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\HWiNFO64\\VSB", 0,
                      KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        g_hwInfoGadgetRegistryCache.cpuIndex.reset();
        g_hwInfoGadgetRegistryCache.gpuIndex.reset();
        g_hwInfoGadgetRegistryCache.nextFullScan = now + kHwInfoUnavailableRetryInterval;
        return;
    }
    // Registry has no polling heartbeat: unchanged values may not be rewritten.
    // Bound the last key write's age so retained values cannot stay live forever.
    auto written = ReadRegistryWriteTime(key);
    FILETIME wallNow{};
    GetSystemTimeAsFileTime(&wallNow);
    if (!written || !HwInfoRegistryPublicationIsFresh(
            *written, FileTimeValue(wallNow), now, g_hwInfoGadgetRegistryCache)) {
        RegCloseKey(key);
        return;
    }
    MetricsSnapshot candidate = snapshot;
    auto candidateDiagnostics = diagnostics;
    ReadHwInfoGadgetRegistryValues(key, candidate, settings, gpuAdapterName,
                                    candidateDiagnostics, now);
    auto verifiedWrite = ReadRegistryWriteTime(key);
    RegCloseKey(key);
    GetSystemTimeAsFileTime(&wallNow);
    // A concurrent writer can reorder Sensor/Label/Value entries while we read.
    // Discard that attempt and leave any previously supplied SM values intact.
    if (verifiedWrite != written || !HwInfoRegistryPublicationIsFresh(
            *written, FileTimeValue(wallNow), SampleTime::clock::now(),
            g_hwInfoGadgetRegistryCache)) {
        return;
    }
    snapshot = candidate;
    diagnostics = candidateDiagnostics;
}

void ReadWindowsThermalZones(MetricsSnapshot& snapshot,
                             const ModSettings& settings);
void ReadWindowsGpuTemperature(MetricsSnapshot& snapshot,
                               const ModSettings& settings);

void ReadHwInfoTemperatures(MetricsSnapshot& snapshot,
                            const ModSettings& settings,
                            const std::optional<std::wstring>& gpuAdapterName,
                            HwInfoTemperatureDiagnostics& diagnostics) {
    ReadHwInfoSharedMemory(snapshot, settings, gpuAdapterName, diagnostics);
    if (!snapshot.cpuTemp || !snapshot.gpuTemp) {
        ReadHwInfoGadgetRegistry(snapshot, settings, gpuAdapterName,
                                 diagnostics);
    }
}

void LogHwInfoGpuTemperatureMismatch(
    const MetricsSnapshot& snapshot,
    const ModSettings& settings,
    const std::optional<std::wstring>& gpuAdapterName,
    const HwInfoTemperatureDiagnostics& diagnostics) {
    if (snapshot.gpuTemp || !settings.gpuTempSensor.empty() ||
        !diagnostics.gpuTemperatureReadingFound ||
        diagnostics.gpuAdapterMatched ||
        g_hwInfoGpuAdapterMismatchLogged.exchange(true)) {
        return;
    }

    if (gpuAdapterName) {
        Wh_Log(L"HWiNFO GPU temperature readings found, but none matched "
               L"adapter '%s'; set the GPU temperature sensor filter to "
               L"select one explicitly",
               gpuAdapterName->c_str());
    } else {
        Wh_Log(L"HWiNFO GPU temperature readings found, but the selected "
               L"Windows GPU adapter could not be identified; set the GPU "
               L"temperature sensor filter to select one explicitly");
    }
}

void ReadTemperatures(MetricsSnapshot& snapshot,
                      const ModSettings& settings) {
    std::optional<std::wstring> gpuAdapterName;
    HwInfoTemperatureDiagnostics hwInfoDiagnostics;
    bool usedHwInfo = false;
    if (settings.temperatureSource != TemperatureSource::WindowsNative &&
        settings.temperatureSource != TemperatureSource::Disabled) {
        gpuAdapterName = ResolveGpuTemperatureAdapterName(settings);
    }
    switch (settings.temperatureSource) {
        case TemperatureSource::SharedMemory:
            usedHwInfo = true;
            ReadHwInfoSharedMemory(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            break;

        case TemperatureSource::GadgetRegistry:
            usedHwInfo = true;
            ReadHwInfoGadgetRegistry(snapshot, settings, gpuAdapterName,
                                     hwInfoDiagnostics);
            break;

        case TemperatureSource::WindowsNative:
            ReadWindowsGpuTemperature(snapshot, settings);
            ReadWindowsThermalZones(snapshot, settings);
            break;

        case TemperatureSource::Disabled:
            break;

        case TemperatureSource::HwInfoAuto:
            usedHwInfo = true;
            ReadHwInfoTemperatures(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            break;

        case TemperatureSource::Auto:
        default:
            usedHwInfo = true;
            ReadHwInfoTemperatures(snapshot, settings, gpuAdapterName,
                                   hwInfoDiagnostics);
            if (!snapshot.gpuTemp) {
                ReadWindowsGpuTemperature(snapshot, settings);
            }
            if (!snapshot.cpuTemp) {
                ReadWindowsThermalZones(snapshot, settings);
            }
            break;
    }
    if (usedHwInfo) {
        LogHwInfoGpuTemperatureMismatch(snapshot, settings, gpuAdapterName,
                                        hwInfoDiagnostics);
    }
}

uint64_t FileTimeValue(const FILETIME& value) {
    ULARGE_INTEGER result{};
    result.LowPart = value.dwLowDateTime;
    result.HighPart = value.dwHighDateTime;
    return result.QuadPart;
}

std::optional<double> ReadCpuUsage() {
    static bool initialized = false;
    static uint64_t previousIdle = 0;
    static uint64_t previousKernel = 0;
    static uint64_t previousUser = 0;

    FILETIME idleTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return std::nullopt;
    }

    uint64_t idle = FileTimeValue(idleTime);
    uint64_t kernel = FileTimeValue(kernelTime);
    uint64_t user = FileTimeValue(userTime);
    if (!initialized) {
        initialized = true;
        previousIdle = idle;
        previousKernel = kernel;
        previousUser = user;
        return std::nullopt;
    }

    uint64_t idleDelta = idle - previousIdle;
    uint64_t kernelDelta = kernel - previousKernel;
    uint64_t userDelta = user - previousUser;
    previousIdle = idle;
    previousKernel = kernel;
    previousUser = user;

    uint64_t total = kernelDelta + userDelta;
    if (!total || idleDelta > total) {
        return std::nullopt;
    }
    return std::clamp(100.0 * static_cast<double>(total - idleDelta) /
                          static_cast<double>(total),
                      0.0, 100.0);
}

void ReadMemory(MetricsSnapshot& snapshot) {
    MEMORYSTATUSEX memory{};
    memory.dwLength = sizeof(memory);
    if (!GlobalMemoryStatusEx(&memory)) {
        return;
    }

    snapshot.ram = static_cast<double>(memory.dwMemoryLoad);
    snapshot.ramTotalGb = static_cast<double>(memory.ullTotalPhys) / kGiB;
    snapshot.ramUsedGb =
        static_cast<double>(memory.ullTotalPhys - memory.ullAvailPhys) / kGiB;
    snapshot.ramAvailable = true;
}

// D3DKMT exposes display-adapter performance data, including temperature,
// without requiring a third-party monitoring application. The declarations are
// kept local so the mod can build in Windhawk environments without d3dkmthk.h.
using D3DKMT_HANDLE = UINT32;

struct D3DKMT_OPENADAPTERFROMLUID {
    LUID AdapterLuid;
    D3DKMT_HANDLE hAdapter;
};

struct D3DKMT_CLOSEADAPTER {
    D3DKMT_HANDLE hAdapter;
};

struct D3DKMT_QUERYADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    UINT Type;
    void* pPrivateDriverData;
    UINT PrivateDriverDataSize;
};

struct D3DKMT_ADAPTER_PERFDATA {
    UINT PhysicalAdapterIndex;
    ULONGLONG MemoryFrequency;
    ULONGLONG MaxMemoryFrequency;
    ULONGLONG MaxMemoryFrequencyOC;
    ULONGLONG MemoryBandwidth;
    ULONGLONG PCIEBandwidth;
    ULONG FanRPM;
    ULONG Power;
    ULONG Temperature;
    UCHAR PowerStateOverride;
};

struct D3DKMT_ADAPTERINFO {
    D3DKMT_HANDLE hAdapter;
    LUID AdapterLuid;
    ULONG NumOfSources;
    BOOL bPresentMoveRegionsPreferred;
};

struct D3DKMT_ENUMADAPTERS2 {
    ULONG NumAdapters;
    D3DKMT_ADAPTERINFO* pAdapters;
};

struct D3DKMT_ADAPTERREGISTRYINFO {
    WCHAR AdapterString[MAX_PATH];
    WCHAR BiosString[MAX_PATH];
    WCHAR DacType[MAX_PATH];
    WCHAR ChipType[MAX_PATH];
};

struct D3DKMT_SEGMENTSIZEINFO {
    ULONGLONG DedicatedVideoMemorySize;
    ULONGLONG DedicatedSystemMemorySize;
    ULONGLONG SharedSystemMemorySize;
};

struct D3DKMT_ADAPTERTYPE {
    UINT Value;
};

constexpr UINT kAdapterRegistryInfoQueryType = 8;
constexpr UINT kAdapterSegmentSizeQueryType = 3;
constexpr UINT kAdapterTypeQueryType = 15;
constexpr UINT kAdapterPerfDataQueryType = 62;  // KMTQAITYPE_ADAPTERPERFDATA
constexpr ULONG kMaxD3dkmtAdapters = 16;
constexpr UINT kHybridIntegratedAdapterFlag = 1u << 5;
constexpr LONG kStatusNotImplemented = static_cast<LONG>(0xC0000002u);
constexpr LONG kStatusNotSupported = static_cast<LONG>(0xC00000BBu);
constexpr LONG kStatusInvalidHandle = static_cast<LONG>(0xC0000008u);

using D3DKMTEnumAdapters2_t = LONG(WINAPI*)(D3DKMT_ENUMADAPTERS2*);
using D3DKMTOpenAdapterFromLuid_t =
    LONG(WINAPI*)(D3DKMT_OPENADAPTERFROMLUID*);
using D3DKMTQueryAdapterInfo_t =
    LONG(WINAPI*)(D3DKMT_QUERYADAPTERINFO*);
using D3DKMTCloseAdapter_t =
    LONG(WINAPI*)(const D3DKMT_CLOSEADAPTER*);

D3DKMTEnumAdapters2_t g_d3dkmtEnumAdapters2 = nullptr;
D3DKMTOpenAdapterFromLuid_t g_d3dkmtOpenAdapterFromLuid = nullptr;
D3DKMTQueryAdapterInfo_t g_d3dkmtQueryAdapterInfo = nullptr;
D3DKMTCloseAdapter_t g_d3dkmtCloseAdapter = nullptr;

struct GpuAdapterInfo {
    std::wstring description;
    std::wstring luid;
    LUID luidValue{};
    uint64_t dedicatedVideoMemory = 0;
    uint64_t sharedSystemMemory = 0;
    bool integrated = false;
};

std::optional<std::wstring> g_cachedGpuAdapterFilter;
std::optional<GpuAdapterInfo> g_cachedGpuAdapterInfo;
bool g_cachedGpuAdapterResolved = false;
std::chrono::steady_clock::time_point g_nextGpuAdapterResolve{};
std::optional<std::wstring> g_lastResolvedGpuAdapterFilter;
std::optional<LUID> g_lastResolvedGpuAdapterLuid;
bool g_gpuAdapterIdentityChanged = false;
bool g_hasResolvedGpuAdapterIdentity = false;
D3DKMT_HANDLE g_cachedD3dkmtAdapterHandle = 0;
LUID g_cachedD3dkmtAdapterLuid{};

struct GpuTemperatureRetryState {
    std::optional<LUID> adapterLuid;
    unsigned failures = 0;
    SampleTime nextAttempt{};
};
GpuTemperatureRetryState g_gpuTemperatureRetry;

bool SameLuid(const LUID& left, const LUID& right) {
    return left.HighPart == right.HighPart && left.LowPart == right.LowPart;
}

void CloseCachedD3dkmtAdapterHandle() {
    if (g_cachedD3dkmtAdapterHandle && g_d3dkmtCloseAdapter) {
        D3DKMT_CLOSEADAPTER closeAdapter{g_cachedD3dkmtAdapterHandle};
        g_d3dkmtCloseAdapter(&closeAdapter);
    }
    g_cachedD3dkmtAdapterHandle = 0;
    g_cachedD3dkmtAdapterLuid = {};
}

void InvalidateGpuAdapterCache() {
    CloseCachedD3dkmtAdapterHandle();
    g_cachedGpuAdapterFilter.reset();
    g_cachedGpuAdapterInfo.reset();
    g_cachedGpuAdapterResolved = false;
    g_nextGpuAdapterResolve = {};
}

std::wstring FormatAdapterLuid(const LUID& luid) {
    wchar_t buffer[32];
    swprintf(buffer, std::size(buffer), L"0x%08X_0x%08X",
             static_cast<DWORD>(luid.HighPart), luid.LowPart);
    return ToLower(buffer);
}

std::wstring FixedWideToString(const wchar_t* value, size_t capacity) {
    size_t length = 0;
    while (length < capacity && value[length]) {
        length++;
    }
    return std::wstring(value, length);
}

std::optional<GpuAdapterInfo> GetLiveD3dkmtAdapterInfo(
    const std::wstring& filterLower) {
    if (!g_d3dkmtEnumAdapters2 || !g_d3dkmtQueryAdapterInfo ||
        !g_d3dkmtCloseAdapter) {
        return std::nullopt;
    }

    D3DKMT_ADAPTERINFO adapters[kMaxD3dkmtAdapters]{};
    D3DKMT_ENUMADAPTERS2 enumeration{};
    enumeration.NumAdapters = std::size(adapters);
    enumeration.pAdapters = adapters;
    if (g_d3dkmtEnumAdapters2(&enumeration) != 0) {
        return std::nullopt;
    }

    std::optional<GpuAdapterInfo> selected;
    ULONG adapterCount = std::min<ULONG>(enumeration.NumAdapters,
                                         std::size(adapters));
    for (ULONG index = 0; index < adapterCount; index++) {
        const auto& adapter = adapters[index];

        D3DKMT_ADAPTERREGISTRYINFO registryInfo{};
        D3DKMT_QUERYADAPTERINFO registryQuery{};
        registryQuery.hAdapter = adapter.hAdapter;
        registryQuery.Type = kAdapterRegistryInfoQueryType;
        registryQuery.pPrivateDriverData = &registryInfo;
        registryQuery.PrivateDriverDataSize = sizeof(registryInfo);
        bool registryAvailable =
            g_d3dkmtQueryAdapterInfo(&registryQuery) == 0;

        D3DKMT_SEGMENTSIZEINFO segmentInfo{};
        D3DKMT_QUERYADAPTERINFO segmentQuery{};
        segmentQuery.hAdapter = adapter.hAdapter;
        segmentQuery.Type = kAdapterSegmentSizeQueryType;
        segmentQuery.pPrivateDriverData = &segmentInfo;
        segmentQuery.PrivateDriverDataSize = sizeof(segmentInfo);
        bool segmentsAvailable =
            g_d3dkmtQueryAdapterInfo(&segmentQuery) == 0;

        D3DKMT_ADAPTERTYPE adapterType{};
        D3DKMT_QUERYADAPTERINFO adapterTypeQuery{};
        adapterTypeQuery.hAdapter = adapter.hAdapter;
        adapterTypeQuery.Type = kAdapterTypeQueryType;
        adapterTypeQuery.pPrivateDriverData = &adapterType;
        adapterTypeQuery.PrivateDriverDataSize = sizeof(adapterType);
        bool adapterTypeAvailable =
            g_d3dkmtQueryAdapterInfo(&adapterTypeQuery) == 0;

        std::wstring description =
            registryAvailable
                ? FixedWideToString(registryInfo.AdapterString,
                                    std::size(registryInfo.AdapterString))
                : L"";
        GpuAdapterInfo candidate{
            description,
            FormatAdapterLuid(adapter.AdapterLuid),
            adapter.AdapterLuid,
            segmentsAvailable ? segmentInfo.DedicatedVideoMemorySize : 0,
            segmentsAvailable ? segmentInfo.SharedSystemMemorySize : 0,
            adapterTypeAvailable &&
                (adapterType.Value & kHybridIntegratedAdapterFlag) != 0,
        };

        bool matchesFilter =
            filterLower.empty() ||
            Contains(ToLower(candidate.description), filterLower);
        bool betterCandidate =
            !selected || candidate.dedicatedVideoMemory >
                             selected->dedicatedVideoMemory ||
            (candidate.dedicatedVideoMemory ==
                 selected->dedicatedVideoMemory &&
             !candidate.description.empty() &&
             selected->description.empty()) ||
            (candidate.dedicatedVideoMemory ==
                 selected->dedicatedVideoMemory &&
             candidate.description.empty() == selected->description.empty() &&
             candidate.sharedSystemMemory > selected->sharedSystemMemory);
        if (matchesFilter && betterCandidate) {
            selected = std::move(candidate);
        }
    }

    for (ULONG index = 0; index < adapterCount; index++) {
        if (adapters[index].hAdapter) {
            D3DKMT_CLOSEADAPTER closeAdapter{adapters[index].hAdapter};
            g_d3dkmtCloseAdapter(&closeAdapter);
        }
    }

    // Stale D3DKMT duplicates can retain the full memory sizes while losing
    // their registry identity. Prefer the DXGI compatibility path instead of
    // caching such an ambiguous adapter.
    return selected && !selected->description.empty() ? selected
                                                       : std::nullopt;
}

std::optional<GpuAdapterInfo> GetDxgiAdapterInfo(
    const std::wstring& filterLower) {
    com_ptr<IDXGIFactory> factory;
    if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.put())))) {
        return std::nullopt;
    }

    DXGI_ADAPTER_DESC selected{};
    bool found = false;
    for (UINT index = 0;; index++) {
        com_ptr<IDXGIAdapter> adapter;
        HRESULT result = factory->EnumAdapters(index, adapter.put());
        if (result == DXGI_ERROR_NOT_FOUND) {
            break;
        }
        if (FAILED(result)) {
            continue;
        }

        DXGI_ADAPTER_DESC description{};
        if (FAILED(adapter->GetDesc(&description))) {
            continue;
        }

        if (!filterLower.empty()) {
            if (Contains(ToLower(description.Description), filterLower)) {
                selected = description;
                found = true;
                break;
            }
        } else if (!found || description.DedicatedVideoMemory >
                                      selected.DedicatedVideoMemory) {
            selected = description;
            found = true;
        }
    }

    if (!found) {
        return std::nullopt;
    }

    return GpuAdapterInfo{
        selected.Description, FormatAdapterLuid(selected.AdapterLuid),
        selected.AdapterLuid,
        selected.DedicatedVideoMemory, selected.SharedSystemMemory, false};
}

std::optional<GpuAdapterInfo> ResolveCurrentGpuAdapterInfo(
    const std::wstring& filterLower,
    PCWSTR* provider = nullptr) {
    auto adapter = GetLiveD3dkmtAdapterInfo(filterLower);
    PCWSTR resolvedProvider = L"D3DKMT";
    if (!adapter) {
        adapter = GetDxgiAdapterInfo(filterLower);
        resolvedProvider = L"DXGI fallback";
    }
    if (provider) {
        *provider = resolvedProvider;
    }
    return adapter;
}

std::optional<GpuAdapterInfo> GetGpuAdapterInfo(
    const std::wstring& adapterFilter) {
    std::wstring filterLower = ToLower(adapterFilter);
    auto now = std::chrono::steady_clock::now();
    if (g_cachedGpuAdapterResolved &&
        g_cachedGpuAdapterFilter == filterLower &&
        now < g_nextGpuAdapterResolve) {
        return g_cachedGpuAdapterInfo;
    }

    std::optional<GpuAdapterInfo> previousAdapter;
    if (g_cachedGpuAdapterFilter &&
        *g_cachedGpuAdapterFilter != filterLower) {
        CloseCachedD3dkmtAdapterHandle();
    } else {
        previousAdapter = g_cachedGpuAdapterInfo;
    }

    g_cachedGpuAdapterFilter = filterLower;
    PCWSTR provider = nullptr;
    auto resolvedAdapter = ResolveCurrentGpuAdapterInfo(filterLower, &provider);
    g_cachedGpuAdapterResolved = true;

    if (!resolvedAdapter) {
        CloseCachedD3dkmtAdapterHandle();
        g_cachedGpuAdapterInfo.reset();
        g_nextGpuAdapterResolve = now + std::chrono::seconds(5);
        if (!previousAdapter) {
            if (filterLower.empty()) {
                Wh_Log(L"No GPU adapter found; retrying automatically");
            } else {
                Wh_Log(L"No GPU adapter matched: %s; retrying automatically",
                       filterLower.c_str());
            }
        }
        return std::nullopt;
    }

    bool adapterChanged =
        !previousAdapter || previousAdapter->luid != resolvedAdapter->luid ||
        previousAdapter->dedicatedVideoMemory !=
            resolvedAdapter->dedicatedVideoMemory ||
        previousAdapter->sharedSystemMemory !=
            resolvedAdapter->sharedSystemMemory ||
        previousAdapter->integrated != resolvedAdapter->integrated;
    if (previousAdapter &&
        !SameLuid(previousAdapter->luidValue, resolvedAdapter->luidValue)) {
        CloseCachedD3dkmtAdapterHandle();
    }
    if (g_lastResolvedGpuAdapterLuid && g_lastResolvedGpuAdapterFilter &&
        *g_lastResolvedGpuAdapterFilter == filterLower &&
        !SameLuid(*g_lastResolvedGpuAdapterLuid,
                  resolvedAdapter->luidValue)) {
        // A driver restart can preserve the adapter name while assigning a new
        // LUID. Existing PDH wildcard counters may keep the old instances, so
        // request one rebuild when the normal adapter refresh confirms it.
        g_gpuAdapterIdentityChanged = true;
    }
    g_lastResolvedGpuAdapterFilter = filterLower;
    g_lastResolvedGpuAdapterLuid = resolvedAdapter->luidValue;
    g_hasResolvedGpuAdapterIdentity = true;
    g_cachedGpuAdapterInfo = std::move(resolvedAdapter);
    g_nextGpuAdapterResolve = now + std::chrono::seconds(60);

    if (!adapterChanged) {
        return g_cachedGpuAdapterInfo;
    }
    Wh_Log(L"Selected GPU (%s): %s, LUID %s, dedicated %.1f GiB, shared %.1f "
           L"GiB, integrated=%d",
           provider, g_cachedGpuAdapterInfo->description.c_str(),
           g_cachedGpuAdapterInfo->luid.c_str(),
           static_cast<double>(g_cachedGpuAdapterInfo->dedicatedVideoMemory) /
               kGiB,
           static_cast<double>(g_cachedGpuAdapterInfo->sharedSystemMemory) /
               kGiB,
           g_cachedGpuAdapterInfo->integrated);
    return g_cachedGpuAdapterInfo;
}

std::optional<std::wstring> ResolveGpuTemperatureAdapterName(
    const ModSettings& settings) {
    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (adapter) {
        return adapter->description;
    }
    if (settings.gpuAdapter.empty() && !g_hasResolvedGpuAdapterIdentity) {
        // Preserve HWiNFO-only operation when Windows adapter enumeration has
        // never been available. Once an adapter was resolved, a later failure
        // is treated as transient to avoid selecting another GPU.
        return std::wstring{};
    }
    // Without a resolved Windows adapter identity, generic HWiNFO matching can
    // select another GPU on multi-adapter systems. Leave the temperature
    // unavailable until the short adapter retry succeeds instead.
    return std::nullopt;
}

void DeferGpuTemperatureRetry(LONG status) {
    // This provider is optional: an unsupported perf-data query must not
    // invalidate the adapter identity or the independent GPU/VRAM counters.
    // Reprobe unavailable sensors once a minute, including after a driver
    // replacement that preserves the LUID. Transient failures start at 5 s.
    unsigned exponent = std::min(g_gpuTemperatureRetry.failures, 4u);
    g_gpuTemperatureRetry.failures = exponent + 1;
    bool unsupported = status == kStatusNotImplemented ||
                       status == kStatusNotSupported;
    unsigned delaySeconds = status == 0 || unsupported
                                ? 60u
                                : std::min(5u << exponent, 60u);
    g_gpuTemperatureRetry.nextAttempt =
        std::chrono::steady_clock::now() + std::chrono::seconds(delaySeconds);
    CloseCachedD3dkmtAdapterHandle();
}

std::optional<D3DKMT_HANDLE> GetD3dkmtAdapterHandle(
    const GpuAdapterInfo& adapter) {
    if (g_cachedD3dkmtAdapterHandle &&
        SameLuid(g_cachedD3dkmtAdapterLuid, adapter.luidValue)) {
        return g_cachedD3dkmtAdapterHandle;
    }

    CloseCachedD3dkmtAdapterHandle();
    D3DKMT_OPENADAPTERFROMLUID openAdapter{};
    openAdapter.AdapterLuid = adapter.luidValue;
    LONG status = g_d3dkmtOpenAdapterFromLuid(&openAdapter);
    if (status != 0 || !openAdapter.hAdapter) {
        DeferGpuTemperatureRetry(status != 0 ? status : kStatusInvalidHandle);
        return std::nullopt;
    }
    g_cachedD3dkmtAdapterHandle = openAdapter.hAdapter;
    g_cachedD3dkmtAdapterLuid = adapter.luidValue;
    return g_cachedD3dkmtAdapterHandle;
}

void ReadWindowsGpuTemperature(MetricsSnapshot& snapshot,
                               const ModSettings& settings) {
    if (snapshot.gpuTemp || !g_d3dkmtOpenAdapterFromLuid ||
        !g_d3dkmtQueryAdapterInfo || !g_d3dkmtCloseAdapter) {
        return;
    }

    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (!adapter) {
        return;
    }

    if (!g_gpuTemperatureRetry.adapterLuid ||
        !SameLuid(*g_gpuTemperatureRetry.adapterLuid, adapter->luidValue)) {
        CloseCachedD3dkmtAdapterHandle();
        g_gpuTemperatureRetry = {};
        g_gpuTemperatureRetry.adapterLuid = adapter->luidValue;
    }
    if (std::chrono::steady_clock::now() <
        g_gpuTemperatureRetry.nextAttempt) {
        return;
    }

    auto adapterHandle = GetD3dkmtAdapterHandle(*adapter);
    if (!adapterHandle) {
        return;
    }

    D3DKMT_ADAPTER_PERFDATA perfData{};
    D3DKMT_QUERYADAPTERINFO queryInfo{};
    queryInfo.hAdapter = *adapterHandle;
    queryInfo.Type = kAdapterPerfDataQueryType;
    queryInfo.pPrivateDriverData = &perfData;
    queryInfo.PrivateDriverDataSize = sizeof(perfData);

    LONG status = g_d3dkmtQueryAdapterInfo(&queryInfo);
    // The driver reports tenths of a degree Celsius. Zero means unavailable;
    // reject values above 200 C as invalid driver data.
    if (status != 0 || perfData.Temperature == 0 ||
        perfData.Temperature > 2000) {
        DeferGpuTemperatureRetry(status);
        return;
    }

    g_gpuTemperatureRetry.failures = 0;
    g_gpuTemperatureRetry.nextAttempt = {};
    snapshot.gpuTemp = perfData.Temperature / 10.0;
    snapshot.gpuTempProvider = TemperatureProvider::WindowsD3dkmt;
}

bool MatchesGpuAdapter(const std::wstring& instance,
                       const std::optional<GpuAdapterInfo>& adapter) {
    return !adapter || Contains(ToLower(instance), adapter->luid);
}

void CloseMetricSources();

constexpr auto kPdhCounterRetryInterval = std::chrono::seconds(30);
constexpr auto kPdhRecoveryRetryDelay = std::chrono::seconds(1);
constexpr auto kPdhRecoveryCooldown = std::chrono::seconds(60);
constexpr uint32_t kPdhReadFailureThreshold = 3;

void CloseGpuEngineRecoveryProbe() {
    if (g_gpuEngineRecoveryProbe.query) {
        PdhCloseQuery(g_gpuEngineRecoveryProbe.query);
    }
    g_gpuEngineRecoveryProbe = {};
}

void ClosePdhQuery() {
    CloseGpuEngineRecoveryProbe();
    g_consecutiveInvalidGpuSamples = 0;
    if (g_pdhQuery) {
        PdhCloseQuery(g_pdhQuery);
        g_pdhQuery = nullptr;
    }
    g_gpuCounter = nullptr;
    g_vramCounter = nullptr;
    g_sharedVramCounter = nullptr;
}

void CloseCpuPdhQuery() {
    if (g_cpuPdhQuery) {
        PdhCloseQuery(g_cpuPdhQuery);
        g_cpuPdhQuery = nullptr;
    }
    g_cpuUtilityCounter = nullptr;
    g_thermalZoneCounter = nullptr;
}

void RecreatePdhSources(PCWSTR reason,
                        PDH_STATUS status,
                        std::chrono::steady_clock::time_point now) {
    if (status == ERROR_SUCCESS) {
        Wh_Log(L"Recreating GPU performance counters after %s", reason);
    } else {
        Wh_Log(L"Recreating GPU performance counters after %s: %08X",
               reason, status);
    }

    ClosePdhQuery();
    InvalidateGpuAdapterCache();
    g_consecutivePdhReadFailures = 0;
    g_nextPdhCounterRetry = now + kPdhRecoveryRetryDelay;
    g_nextPdhRecovery = now + kPdhRecoveryCooldown;
}

void RecordPdhReadFailure(PCWSTR reason,
                          PDH_STATUS status = ERROR_SUCCESS) {
    g_consecutivePdhReadFailures =
        std::min(g_consecutivePdhReadFailures + 1,
                 kPdhReadFailureThreshold);

    auto now = std::chrono::steady_clock::now();
    if (g_consecutivePdhReadFailures < kPdhReadFailureThreshold ||
        now < g_nextPdhRecovery) {
        return;
    }

    RecreatePdhSources(reason, status, now);
}

void RecordPdhReadSuccess() {
    g_consecutivePdhReadFailures = 0;
}

bool AddPdhCounter(PDH_HQUERY query,
                   PDH_HCOUNTER& counter,
                   PCWSTR path,
                   PCWSTR description) {
    if (counter) {
        return false;
    }

    PDH_HCOUNTER newCounter = nullptr;
    PDH_STATUS status =
        PdhAddEnglishCounterW(query, path, 0, &newCounter);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Adding the %s counter failed: %08X", description, status);
        return false;
    }

    counter = newCounter;
    return true;
}

bool NeedsWindowsThermalZones(const ModSettings& settings) {
    return settings.temperatureSource == TemperatureSource::Auto ||
           settings.temperatureSource == TemperatureSource::WindowsNative;
}

bool EnsureCpuPdhQuery(const ModSettings& settings) {
    auto now = std::chrono::steady_clock::now();
    bool needsThermal = NeedsWindowsThermalZones(settings);
    if (!needsThermal && g_thermalZoneCounter) {
        PdhRemoveCounter(g_thermalZoneCounter);
        g_thermalZoneCounter = nullptr;
    }
    if (g_cpuPdhQuery && g_cpuUtilityCounter &&
        (!needsThermal || g_thermalZoneCounter)) {
        return false;
    }
    if (now < g_nextCpuPdhCounterRetry) {
        return false;
    }
    g_nextCpuPdhCounterRetry = now + kPdhCounterRetryInterval;
    bool created = false;
    if (!g_cpuPdhQuery) {
        if (PdhOpenQueryW(nullptr, 0, &g_cpuPdhQuery) != ERROR_SUCCESS) {
            g_cpuPdhQuery = nullptr;
            return false;
        }
        created = true;
    }
    bool added = AddPdhCounter(
        g_cpuPdhQuery, g_cpuUtilityCounter,
        L"\\Processor Information(_Total)\\% Processor Utility", L"CPU utility");
    if (needsThermal) {
        added |= AddPdhCounter(
            g_cpuPdhQuery, g_thermalZoneCounter,
            L"\\Thermal Zone Information(*)\\Temperature", L"Windows thermal-zone");
    }
    if (!g_cpuUtilityCounter && !g_thermalZoneCounter) {
        CloseCpuPdhQuery();
        return false;
    }
    if (created || added) {
        PdhCollectQueryData(g_cpuPdhQuery);
    }
    return created || added;
}

bool EnsurePdhQuery() {
    auto now = std::chrono::steady_clock::now();
    if (g_pdhQuery && g_gpuCounter && g_vramCounter && g_sharedVramCounter) {
        return false;
    }
    if (now < g_nextPdhCounterRetry) {
        return false;
    }
    g_nextPdhCounterRetry = now + kPdhCounterRetryInterval;
    bool created = false;
    if (!g_pdhQuery) {
        if (PdhOpenQueryW(nullptr, 0, &g_pdhQuery) != ERROR_SUCCESS) {
            g_pdhQuery = nullptr;
            return false;
        }
        created = true;
    }
    bool added = AddPdhCounter(
        g_pdhQuery, g_gpuCounter,
        L"\\GPU Engine(*)\\Utilization Percentage", L"GPU usage");
    added |= AddPdhCounter(
        g_pdhQuery, g_vramCounter,
        L"\\GPU Adapter Memory(*)\\Dedicated Usage", L"VRAM usage");
    added |= AddPdhCounter(
        g_pdhQuery, g_sharedVramCounter,
        L"\\GPU Adapter Memory(*)\\Shared Usage", L"shared GPU-memory usage");
    if (!g_gpuCounter && !g_vramCounter && !g_sharedVramCounter) {
        ClosePdhQuery();
        return false;
    }
    if (created || added) {
        PdhCollectQueryData(g_pdhQuery);
    }
    return created || added;
}

PDH_STATUS ReadPdhArray(PDH_HCOUNTER counter,
                        std::vector<uint8_t>& buffer,
                        DWORD& itemCount) {
    if (!counter) {
        return PDH_CSTATUS_NO_COUNTER;
    }
    // GPU wildcard instances can appear or disappear between the sizing call
    // and the data call. PDH_MORE_DATA on a later call is a normal resize race,
    // not evidence that the provider or display driver is broken.
    constexpr int kMaxArrayReadAttempts = 4;
    DWORD bufferSize = 0;
    PDH_STATUS status = static_cast<PDH_STATUS>(PDH_MORE_DATA);
    for (int attempt = 0; attempt < kMaxArrayReadAttempts; attempt++) {
        itemCount = 0;
        auto* items = bufferSize
                          ? reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(
                                buffer.data())
                          : nullptr;
        status = PdhGetFormattedCounterArrayW(
            counter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items);
        if (status == ERROR_SUCCESS) {
            if (!bufferSize) {
                buffer.clear();
                itemCount = 0;
            }
            return ERROR_SUCCESS;
        }
        if (status != static_cast<PDH_STATUS>(PDH_MORE_DATA) || !bufferSize) {
            return status;
        }
        buffer.resize(bufferSize);
    }
    return status;
}

bool IsHardPdhArrayFailure(PDH_STATUS status) {
    // A wildcard list can keep growing through all bounded buffer retries.
    // Skip that sample; it is not evidence that the query itself is broken.
    return status != ERROR_SUCCESS &&
           status != static_cast<PDH_STATUS>(PDH_NO_DATA) &&
           status != static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE) &&
           status != static_cast<PDH_STATUS>(PDH_MORE_DATA);
}

void ReadWindowsThermalZones(MetricsSnapshot& snapshot,
                             const ModSettings& settings) {
    if (snapshot.cpuTemp || !g_thermalZoneCounter) {
        return;
    }

    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    if (ReadPdhArray(g_thermalZoneCounter, buffer, itemCount) !=
        ERROR_SUCCESS) {
        return;
    }

    std::wstring filter = ToLower(settings.windowsThermalZoneFilter);
    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    double aggregate = 0.0;
    size_t validCount = 0;

    for (DWORD i = 0; i < itemCount; i++) {
        const auto& item = items[i];
        const auto& value = item.FmtValue;
        if (value.CStatus != PDH_CSTATUS_VALID_DATA &&
            value.CStatus != PDH_CSTATUS_NEW_DATA) {
            continue;
        }

        std::wstring instance = item.szName ? ToLower(item.szName) : L"";
        if (!filter.empty() && !Contains(instance, filter)) {
            continue;
        }

        // This counter is reported in Kelvin. Match Taskbar Clock
        // Customization by rejecting dead zones below 200 K, and reject values
        // above the module's supported 200 °C ceiling as corrupt data.
        double kelvin = value.doubleValue;
        if (!std::isfinite(kelvin) || kelvin < 200.0 || kelvin > 473.15) {
            continue;
        }

        double celsius = kelvin - 273.15;
        if (settings.windowsThermalZoneAggregation ==
            ThermalZoneAggregation::Hottest) {
            aggregate = validCount ? std::max(aggregate, celsius) : celsius;
        } else {
            aggregate += celsius;
        }
        validCount++;
    }

    if (!validCount) {
        return;
    }

    if (settings.windowsThermalZoneAggregation ==
        ThermalZoneAggregation::Average) {
        aggregate /= validCount;
    }
    snapshot.cpuTemp = aggregate;
    snapshot.cpuTempProvider = TemperatureProvider::WindowsThermalZones;
}

std::optional<double> ReadGpuUsage(
    PDH_HCOUNTER counter,
    const std::optional<GpuAdapterInfo>& adapter,
    PDH_STATUS& readStatus,
    bool& adapterInstanceFound) {
    adapterInstanceFound = false;
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    readStatus = ReadPdhArray(counter, buffer, itemCount);
    if (readStatus == static_cast<PDH_STATUS>(PDH_NO_DATA) ||
        readStatus == static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE)) {
        return std::nullopt;
    }
    if (readStatus != ERROR_SUCCESS) {
        return std::nullopt;
    }

    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    std::unordered_map<std::wstring, double> engineTotals;
    bool found = false;
    for (DWORD i = 0; i < itemCount; i++) {
        std::wstring instance = items[i].szName ? items[i].szName : L"";
        if (!MatchesGpuAdapter(instance, adapter)) {
            continue;
        }
        adapterInstanceFound = true;
        const auto& value = items[i].FmtValue;
        if ((value.CStatus != PDH_CSTATUS_VALID_DATA &&
             value.CStatus != PDH_CSTATUS_NEW_DATA) ||
            !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
            continue;
        }
        size_t luidPosition = instance.find(L"luid_");
        std::wstring engineKey =
            luidPosition == std::wstring::npos ? instance
                                                : instance.substr(luidPosition);
        engineTotals[engineKey] += value.doubleValue;
        found = true;
    }

    double busiestEngine = 0.0;
    for (const auto& [engine, usage] : engineTotals) {
        busiestEngine = std::max(busiestEngine, usage);
    }
    if (found) {
        return std::clamp(busiestEngine, 0.0, 100.0);
    }
    // Absence alone cannot distinguish a parked adapter from a stale query.
    // The caller may report idle only when this adapter's memory counter works.
    return std::nullopt;
}

std::optional<double> ReadCpuUtility() {
    if (!g_cpuUtilityCounter) {
        return std::nullopt;
    }
    PDH_FMT_COUNTERVALUE value{};
    PDH_STATUS status = PdhGetFormattedCounterValue(
        g_cpuUtilityCounter, PDH_FMT_DOUBLE, nullptr, &value);
    if (status != ERROR_SUCCESS ||
        (value.CStatus != PDH_CSTATUS_VALID_DATA &&
         value.CStatus != PDH_CSTATUS_NEW_DATA) ||
        !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
        return std::nullopt;
    }
    return std::clamp(value.doubleValue, 0.0, 100.0);
}

std::optional<double> ReadVramUsedBytes(
    PDH_HCOUNTER counter,
    const std::optional<GpuAdapterInfo>& adapter,
    PDH_STATUS& readStatus) {
    std::vector<uint8_t> buffer;
    DWORD itemCount = 0;
    readStatus = ReadPdhArray(counter, buffer, itemCount);
    if (readStatus != ERROR_SUCCESS) {
        return std::nullopt;
    }

    auto* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
    double total = 0.0;
    bool found = false;
    for (DWORD i = 0; i < itemCount; i++) {
        const auto& value = items[i].FmtValue;
        if ((value.CStatus != PDH_CSTATUS_VALID_DATA &&
             value.CStatus != PDH_CSTATUS_NEW_DATA) ||
            !std::isfinite(value.doubleValue) || value.doubleValue < 0.0) {
            continue;
        }
        std::wstring instance = items[i].szName ? items[i].szName : L"";
        if (!MatchesGpuAdapter(instance, adapter)) {
            continue;
        }
        total += value.doubleValue;
        found = true;
    }
    return found ? std::optional<double>(total) : std::nullopt;
}

// Confirm a stale query with an independent, instantaneous memory counter.
// An absent/parked GPU in both queries is NOT a reason to tear down the query.
void RecoverFromMissingGpuSample(const GpuAdapterInfo& adapter,
                                  bool sharedMemory) {
    auto now = std::chrono::steady_clock::now();
    if (now < g_nextPdhRecovery) {
        return;
    }
    g_nextPdhRecovery = now + kPdhRecoveryCooldown;

    PDH_HQUERY probe = nullptr;
    if (PdhOpenQueryW(nullptr, 0, &probe) != ERROR_SUCCESS) {
        return;
    }
    struct QueryGuard {
        PDH_HQUERY query;
        ~QueryGuard() { PdhCloseQuery(query); }
    } guard{probe};
    PDH_HCOUNTER counter = nullptr;
    PCWSTR path = sharedMemory ? L"\\GPU Adapter Memory(*)\\Shared Usage"
                               : L"\\GPU Adapter Memory(*)\\Dedicated Usage";
    if (PdhAddEnglishCounterW(probe, path, 0, &counter) != ERROR_SUCCESS ||
        PdhCollectQueryData(probe) != ERROR_SUCCESS) {
        return;
    }
    PDH_STATUS status = ERROR_SUCCESS;
    if (ReadVramUsedBytes(counter, adapter, status)) {
        RecreatePdhSources(L"fresh query confirmed stale GPU counters",
                           ERROR_SUCCESS, now);
    }
}

// A successful array read can still contain only invalid CStatus values.
// Confirm that a fresh engine query works before discarding the old query.
// Rate counters need a baseline and a later worker tick, not two instant collects.
void RecoverFromInvalidGpuSample(const GpuAdapterInfo& adapter,
                                 const ModSettings& settings) {
    g_consecutiveInvalidGpuSamples = std::min(
        g_consecutiveInvalidGpuSamples + 1, kPdhReadFailureThreshold);
    auto now = std::chrono::steady_clock::now();
    auto& probe = g_gpuEngineRecoveryProbe;
    if (probe.query) {
        if (!SameLuid(probe.adapterLuid, adapter.luidValue)) {
            CloseGpuEngineRecoveryProbe();
            return;
        }
        if (now < probe.nextSample) return;
        PDH_STATUS status = PdhCollectQueryData(probe.query);
        bool instanceFound = false;
        bool fresh = status == ERROR_SUCCESS &&
                     ReadGpuUsage(probe.counter, adapter, status, instanceFound).has_value();
        CloseGpuEngineRecoveryProbe();
        if (fresh) {
            RecreatePdhSources(L"fresh query confirmed invalid GPU engine samples",
                               ERROR_SUCCESS, now);
        }
        return;
    }
    if (g_consecutiveInvalidGpuSamples < kPdhReadFailureThreshold || now < g_nextPdhRecovery) return;
    g_nextPdhRecovery = now + kPdhRecoveryCooldown;
    if (PdhOpenQueryW(nullptr, 0, &probe.query) != ERROR_SUCCESS) {
        CloseGpuEngineRecoveryProbe();
        return;
    }
    if (PdhAddEnglishCounterW(probe.query, L"\\GPU Engine(*)\\Utilization Percentage", 0,
                             &probe.counter) != ERROR_SUCCESS ||
        PdhCollectQueryData(probe.query) != ERROR_SUCCESS) {
        CloseGpuEngineRecoveryProbe();
        return;
    }
    probe.adapterLuid = adapter.luidValue;
    probe.nextSample = now + std::chrono::seconds(settings.updateInterval);
}

bool LooksLikeIntegratedGpu(const GpuAdapterInfo& adapter) {
    if (adapter.integrated || adapter.dedicatedVideoMemory == 0) {
        return true;
    }

    // Some WDDM drivers don't set HybridIntegrated. A small hardware carve-out
    // plus a larger shared pool is the stable signal; the explicit setting is
    // available for unusual adapters instead of maintaining a GPU name list.
    constexpr uint64_t kMaximumIntegratedCarveout = 512ull * 1024 * 1024;
    return adapter.dedicatedVideoMemory <= kMaximumIntegratedCarveout &&
           adapter.sharedSystemMemory > adapter.dedicatedVideoMemory;
}

bool UseSharedGpuMemory(const GpuAdapterInfo& adapter,
                        const ModSettings& settings) {
    switch (settings.gpuMemoryMode) {
        case GpuMemoryMode::Dedicated:
            return false;
        case GpuMemoryMode::Shared:
            return true;
        case GpuMemoryMode::Auto:
        default:
            return adapter.sharedSystemMemory > 0 &&
                   LooksLikeIntegratedGpu(adapter);
    }
}

bool IsSoftPdhArrayAbsence(PDH_STATUS status) {
    return status == ERROR_SUCCESS ||
           status == static_cast<PDH_STATUS>(PDH_NO_DATA) ||
           status == static_cast<PDH_STATUS>(PDH_CSTATUS_NO_INSTANCE);
}

void ReadPdhMetrics(MetricsSnapshot& snapshot, const ModSettings& settings) {
    bool cpuPrimed = EnsureCpuPdhQuery(settings);
    if (g_cpuPdhQuery && !cpuPrimed) {
        if (PdhCollectQueryData(g_cpuPdhQuery) == ERROR_SUCCESS) {
            if (auto cpuUtility = ReadCpuUtility()) {
                snapshot.cpu = *cpuUtility;
                snapshot.cpuAvailable = true;
            }
        } else {
            CloseCpuPdhQuery();
            g_nextCpuPdhCounterRetry =
                std::chrono::steady_clock::now() + kPdhCounterRetryInterval;
        }
    }
    // Never publish a rate value from two collects in the same tick. CPU/RAM
    // remain available while the independent GPU query establishes a baseline.
    if (EnsurePdhQuery()) {
        return;
    }
    if (!g_pdhQuery) {
        return;
    }

    PDH_STATUS collectStatus = PdhCollectQueryData(g_pdhQuery);
    if (collectStatus != ERROR_SUCCESS) {
        CloseGpuEngineRecoveryProbe();
        g_consecutiveInvalidGpuSamples = 0;
        RecordPdhReadFailure(L"collection", collectStatus);
        return;
    }

    auto adapter = GetGpuAdapterInfo(settings.gpuAdapter);
    if (std::exchange(g_gpuAdapterIdentityChanged, false)) {
        RecreatePdhSources(L"confirmed GPU adapter LUID change", ERROR_SUCCESS,
                           std::chrono::steady_clock::now());
        return;
    }
    PDH_STATUS gpuReadStatus = ERROR_SUCCESS;
    bool gpuAdapterInstanceFound = false;
    auto gpuUsage = adapter
                        ? ReadGpuUsage(g_gpuCounter, adapter, gpuReadStatus,
                                       gpuAdapterInstanceFound)
                        : std::optional<double>{};
    uint64_t vramTotalBytes = 0;
    PDH_HCOUNTER vramCounter = nullptr;
    if (adapter) {
        if (UseSharedGpuMemory(*adapter, settings)) {
            vramTotalBytes = adapter->sharedSystemMemory;
            vramCounter = g_sharedVramCounter;
        } else {
            vramTotalBytes = adapter->dedicatedVideoMemory;
            vramCounter = g_vramCounter;
        }
    }
    PDH_STATUS vramReadStatus = ERROR_SUCCESS;
    auto vramUsedBytes = ReadVramUsedBytes(vramCounter, adapter,
                                           vramReadStatus);
    bool vramAvailable = vramTotalBytes > 0 && vramUsedBytes.has_value();
    if (vramAvailable) {
        snapshot.vramUsedGb = *vramUsedBytes / kGiB;
        snapshot.vramTotalGb = static_cast<double>(vramTotalBytes) / kGiB;
        snapshot.vram = std::clamp(
            snapshot.vramUsedGb / snapshot.vramTotalGb * 100.0, 0.0, 100.0);
        snapshot.vramAvailable = true;
    }

    if (gpuUsage) {
        snapshot.gpu = *gpuUsage;
        snapshot.gpuAvailable = true;
    } else if (vramAvailable && g_gpuCounter && !gpuAdapterInstanceFound &&
               IsSoftPdhArrayAbsence(gpuReadStatus)) {
        // A working memory counter identifies a healthy idle/parked adapter.
        snapshot.gpu = 0.0;
        snapshot.gpuAvailable = true;
    }

    bool hardReadFailure =
        (g_gpuCounter && IsHardPdhArrayFailure(gpuReadStatus)) ||
        (vramCounter && IsHardPdhArrayFailure(vramReadStatus));
    bool adapterSampleMissing = adapter &&
                                 vramCounter && vramTotalBytes > 0 &&
                                 !vramAvailable &&
                                 IsSoftPdhArrayAbsence(vramReadStatus);
    if (hardReadFailure) {
        CloseGpuEngineRecoveryProbe();
        g_consecutiveInvalidGpuSamples = 0;
        RecordPdhReadFailure(L"counter read");
    } else {
        RecordPdhReadSuccess();
        bool invalidEngineSample = adapter && g_gpuCounter &&
                                   gpuAdapterInstanceFound && !gpuUsage &&
                                   IsSoftPdhArrayAbsence(gpuReadStatus);
        if (invalidEngineSample && vramAvailable) {
            RecoverFromInvalidGpuSample(*adapter, settings);
        } else {
            CloseGpuEngineRecoveryProbe();
            g_consecutiveInvalidGpuSamples = 0;
        }
        if (adapterSampleMissing) {
            RecoverFromMissingGpuSample(*adapter,
                                        UseSharedGpuMemory(*adapter, settings));
        }
    }
}

MetricsSnapshot CollectMetrics(const ModSettings& settings) {
    MetricsSnapshot snapshot;
    snapshot.capturedAt = std::chrono::steady_clock::now();
    if (auto cpu = ReadCpuUsage()) {
        snapshot.cpu = *cpu;
        snapshot.cpuAvailable = true;
    }
    ReadMemory(snapshot);
    ReadPdhMetrics(snapshot, settings);
    ReadTemperatures(snapshot, settings);
    return snapshot;
}

PCWSTR TemperatureProviderName(TemperatureProvider provider) {
    switch (provider) {
        case TemperatureProvider::HwInfoSharedMemory:
            return L"HWiNFO Shared Memory";
        case TemperatureProvider::HwInfoGadgetRegistry:
            return L"HWiNFO Gadget Registry";
        case TemperatureProvider::WindowsD3dkmt:
            return L"Windows D3DKMT";
        case TemperatureProvider::WindowsThermalZones:
            return L"Windows thermal zones";
        case TemperatureProvider::None:
        default:
            return L"unavailable";
    }
}

void PostTaskbarRefresh(bool refreshSystemState = false) {
    HWND window = g_notificationWindow.load();
    UINT message = g_taskbarRefreshMessage.load();
    if (window && message && !g_unloading) {
        // No pointers or module delegates outlive this numeric message.
        PostMessageW(window, message, refreshSystemState ? 1 : 0, 0);
    }
}

void PublishMetrics(MetricsSnapshot snapshot) {
    {
        std::lock_guard lock(g_metricsMutex);
        g_latestMetricsSequence++;
        g_publishedMetrics.push_back(
            {g_latestMetricsSequence, std::move(snapshot)});
        while (g_publishedMetrics.size() > kMaximumPublishedMetrics) {
            g_publishedMetrics.pop_front();
        }
    }
    PostTaskbarRefresh();
}

bool GetMetricsSince(uint64_t afterSequence,
                     MetricsSnapshot& latestSnapshot,
                     uint64_t& latestSequence,
                     std::vector<MetricsSnapshot>& newSnapshots) {
    std::lock_guard lock(g_metricsMutex);
    if (g_publishedMetrics.empty()) {
        return false;
    }
    latestSnapshot = g_publishedMetrics.back().snapshot;
    latestSequence = g_publishedMetrics.back().sequence;
    for (const PublishedMetricsSnapshot& published : g_publishedMetrics) {
        if (published.sequence > afterSequence) {
            newSnapshots.push_back(published.snapshot);
        }
    }
    return true;
}

SampleTime AdvanceSampleDeadline(SampleTime previous,
                                  SampleTime now,
                                  std::chrono::seconds interval) {
    // Skip missed deadlines instead of collecting catch-up bursts. Timestamps
    // preserve the resulting gap in the graph, without stretching its axis.
    if (previous > now) {
        return previous;
    }
    return previous + interval * ((now - previous) / interval + 1);
}

void MetricsWorkerProc() {
    ReadCpuUsage();
    EnsureCpuPdhQuery(*CurrentSettings());
    EnsurePdhQuery();

    auto nextSample = std::chrono::steady_clock::now() +
                      std::chrono::milliseconds(250);
    bool waitFailureLogged = false;
    bool providersLogged = false;
    TemperatureProvider lastCpuProvider = TemperatureProvider::None;
    TemperatureProvider lastGpuProvider = TemperatureProvider::None;
    while (!g_stopMetricsWorker) {
        auto settings = CurrentSettings();
        auto remaining = std::chrono::ceil<std::chrono::milliseconds>(
            nextSample - std::chrono::steady_clock::now());
        DWORD waitMilliseconds = static_cast<DWORD>(
            std::clamp<int64_t>(remaining.count(), 0, 10000));
        DWORD waitResult =
            WaitForSingleObject(g_metricsWorkerWakeEvent, waitMilliseconds);
        if (g_stopMetricsWorker) {
            break;
        }
        if (waitResult == WAIT_FAILED) {
            if (!waitFailureLogged) {
                Wh_Log(L"Metrics worker wait failed: %u", GetLastError());
                waitFailureLogged = true;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else if (waitResult == WAIT_OBJECT_0) {
            settings = CurrentSettings();
            ReadCpuUsage();
            InvalidateGpuAdapterCache();
            g_gpuTemperatureRetry = {};
            g_nextCpuPdhCounterRetry = {};
            EnsureCpuPdhQuery(*settings);
            EnsurePdhQuery();
            // No near-zero settings-change sample. The next measurement gets
            // a complete interval, while the UI may re-render the last sample.
            nextSample = std::chrono::steady_clock::now() +
                         std::chrono::seconds(settings->updateInterval);
            continue;
        } else {
            waitFailureLogged = false;
        }

        settings = CurrentSettings();
        auto snapshot = CollectMetrics(*settings);
        if (!providersLogged ||
            snapshot.cpuTempProvider != lastCpuProvider ||
            snapshot.gpuTempProvider != lastGpuProvider) {
            Wh_Log(L"Temperature providers: CPU=%s, GPU=%s",
                   TemperatureProviderName(snapshot.cpuTempProvider),
                   TemperatureProviderName(snapshot.gpuTempProvider));
            lastCpuProvider = snapshot.cpuTempProvider;
            lastGpuProvider = snapshot.gpuTempProvider;
            providersLogged = true;
        }
        PublishMetrics(std::move(snapshot));
        nextSample = AdvanceSampleDeadline(
            nextSample, std::chrono::steady_clock::now(),
            std::chrono::seconds(settings->updateInterval));
    }
    CloseMetricSources();
}

bool StartMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    if (g_metricsWorker) {
        return true;
    }
    if (g_unloading) {
        return false;
    }

    g_metricsWorkerWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_metricsWorkerWakeEvent) {
        Wh_Log(L"Creating metrics worker event failed: %u", GetLastError());
        return false;
    }

    {
        std::lock_guard metricsLock(g_metricsMutex);
        g_publishedMetrics.clear();
        g_latestMetricsSequence = 0;
    }
    g_stopMetricsWorker = false;
    try {
        g_metricsWorker.emplace(MetricsWorkerProc);
    } catch (...) {
        CloseHandle(g_metricsWorkerWakeEvent);
        g_metricsWorkerWakeEvent = nullptr;
        Wh_Log(L"Starting metrics worker failed");
        return false;
    }
    return true;
}

void WakeMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    if (g_metricsWorkerWakeEvent) {
        SetEvent(g_metricsWorkerWakeEvent);
    }
}

void StopMetricsWorker() {
    std::lock_guard lock(g_metricsWorkerMutex);
    g_stopMetricsWorker = true;
    if (g_metricsWorkerWakeEvent) {
        SetEvent(g_metricsWorkerWakeEvent);
    }
    if (g_metricsWorker) {
        if (g_metricsWorker->joinable()) {
            g_metricsWorker->join();
        }
        g_metricsWorker.reset();
    }
    if (g_metricsWorkerWakeEvent) {
        CloseHandle(g_metricsWorkerWakeEvent);
        g_metricsWorkerWakeEvent = nullptr;
    }

    std::lock_guard metricsLock(g_metricsMutex);
    g_publishedMetrics.clear();
    g_latestMetricsSequence = 0;
}

std::wstring FormatFixed(double value, int decimals) {
    // Fixed-point integer formatting is independent of Explorer's CRT locale.
    long long scaled = std::llround(std::abs(value) * (decimals ? 10.0 : 1.0));
    std::wstring text = value < 0 && scaled ? L"-" : L"";
    text += std::to_wstring(decimals ? scaled / 10 : scaled);
    if (decimals) {
        text += L"." + std::to_wstring(scaled % 10);
    }
    return text;
}

std::wstring FormatPercent(double value) {
    wchar_t buffer[64];
    swprintf(buffer, std::size(buffer), L"%.0f%%",
             std::clamp(value, 0.0, 100.0));
    return buffer;
}

std::wstring FormatTemperature(const std::optional<double>& value) {
    return value ? FormatFixed(*value, 0) + L"°C" : L"--°C";
}

std::wstring FormatCapacity(double usedGb, double totalGb, bool available) {
    if (!available || !std::isfinite(usedGb) || !std::isfinite(totalGb) ||
        totalGb <= 0.0) {
        return L"--/--G";
    }
    double roundedTotalGb = std::round(totalGb);
    int totalDecimals =
        totalGb < 1.0 ||
                (totalGb < 4.0 && std::abs(totalGb - roundedTotalGb) >= 0.05)
            ? 1
            : 0;
    return FormatFixed(usedGb, 1) + L"/" +
           FormatFixed(totalGb, totalDecimals) + L"G";
}

enum class AlertLevel { Normal, Warning, Critical };

AlertLevel g_cpuTemperatureAlert = AlertLevel::Normal;
AlertLevel g_gpuTemperatureAlert = AlertLevel::Normal;
AlertLevel g_ramAlert = AlertLevel::Normal;
AlertLevel g_vramAlert = AlertLevel::Normal;

AlertLevel EvaluateAlert(double value,
                         double warning,
                         double critical,
                         AlertLevel previous,
                         double releaseMargin) {
    if (!std::isfinite(value)) {
        return AlertLevel::Normal;
    }
    if (value >= critical ||
        (previous == AlertLevel::Critical &&
         value >= critical - releaseMargin)) {
        return AlertLevel::Critical;
    }
    if (value >= warning ||
        (previous != AlertLevel::Normal && value >= warning - releaseMargin)) {
        return AlertLevel::Warning;
    }
    return AlertLevel::Normal;
}

std::optional<Color> ParseColor(const std::wstring& value) {
    std::wstring hex = value;
    if (!hex.empty() && hex.front() == L'#') {
        hex.erase(hex.begin());
    }
    if (hex.size() != 6 && hex.size() != 8) {
        return std::nullopt;
    }
    if (!std::all_of(hex.begin(), hex.end(), [](wchar_t character) {
            return std::iswxdigit(character) != 0;
        })) {
        return std::nullopt;
    }

    wchar_t* end = nullptr;
    unsigned long parsed = std::wcstoul(hex.c_str(), &end, 16);
    if (!end || *end) {
        return std::nullopt;
    }

    Color color{};
    if (hex.size() == 8) {
        color.A = static_cast<uint8_t>((parsed >> 24) & 0xFF);
    } else {
        color.A = 0xFF;
    }
    color.R = static_cast<uint8_t>((parsed >> 16) & 0xFF);
    color.G = static_cast<uint8_t>((parsed >> 8) & 0xFF);
    color.B = static_cast<uint8_t>(parsed & 0xFF);
    return color;
}

Color MakeColor(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) {
    Color color{};
    color.A = alpha;
    color.R = red;
    color.G = green;
    color.B = blue;
    return color;
}

SolidColorBrush BrushFromSetting(const std::wstring& value, Color fallback) {
    return SolidColorBrush(ParseColor(value).value_or(fallback));
}

ElementTheme ResolveWidgetTheme() {
    if (g_widget) {
        ElementTheme theme = g_widget.ActualTheme();
        if (theme != ElementTheme::Default) {
            return theme;
        }
    }
    Application application = Application::Current();
    return application && application.RequestedTheme() == ApplicationTheme::Light
               ? ElementTheme::Light
               : ElementTheme::Dark;
}

bool SystemColorsChanged() {
    HIGHCONTRASTW highContrast{};
    highContrast.cbSize = sizeof(highContrast);
    bool highContrastEnabled =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast),
                              &highContrast, 0) &&
        (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    return !g_themeBrushesInitialized ||
           highContrastEnabled != g_cachedHighContrast ||
           (highContrastEnabled &&
             (GetSysColor(COLOR_HIGHLIGHT) != g_cachedHighlightColor ||
              GetSysColor(COLOR_HOTLIGHT) != g_cachedHotlightColor ||
              GetSysColor(COLOR_WINDOWTEXT) != g_cachedWindowTextColor));
}

Color ColorFromColorRef(COLORREF value) {
    return MakeColor(0xFF, GetRValue(value), GetGValue(value),
                     GetBValue(value));
}

void ApplyCachedBrushesToVisuals() {
    for (XamlPath graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Stroke(g_graphBrush);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Fill(g_graphBrush);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Fill(g_graphBrush);
        }
    }
}

struct ThemeOpacityValues {
    double label;
    double value;
    double graph;
    double track;
    double fill;
};

constexpr ThemeOpacityValues ResolveThemeOpacities(bool highContrast,
                                                    int textOpacityPercent) {
    double textOpacity =
        static_cast<double>(textOpacityPercent) / 100.0;
    return highContrast
               ? ThemeOpacityValues{1.0, 1.0, 1.0, 0.45, 1.0}
               : ThemeOpacityValues{textOpacity * 0.62, textOpacity, 0.78,
                                    0.18, 0.76};
}

void ApplyThemeOpacities(const ModSettings& settings) {
    bool highContrast = settings.adaptiveColors && g_cachedHighContrast;
    ThemeOpacityValues opacities =
        ResolveThemeOpacities(highContrast, settings.textOpacity);
    for (TextBlock label :
         {g_cpuLabel, g_gpuLabel, g_ramLabel, g_vramLabel}) {
        if (label) {
            label.Opacity(opacities.label);
        }
    }
    for (TextBlock value : {g_cpuUsageText, g_cpuTempText, g_gpuUsageText,
                            g_gpuTempText, g_ramPercentText,
                            g_ramCapacityText, g_vramPercentText,
                            g_vramCapacityText}) {
        if (value) {
            value.Opacity(opacities.value);
        }
    }
    for (XamlPath graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Opacity(opacities.graph);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Opacity(opacities.track);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Opacity(opacities.fill);
        }
    }
}

void RefreshThemeBrushes(const ModSettings& settings) {
    ElementTheme theme = ResolveWidgetTheme();
    g_cachedWidgetTheme = theme;
    g_themeBrushesInitialized = true;
    HIGHCONTRASTW highContrast{};
    highContrast.cbSize = sizeof(highContrast);
    g_cachedHighContrast =
        SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast),
                              &highContrast, 0) &&
        (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
    g_cachedHighlightColor = GetSysColor(COLOR_HIGHLIGHT);
    g_cachedHotlightColor = GetSysColor(COLOR_HOTLIGHT);
    g_cachedWindowTextColor = GetSysColor(COLOR_WINDOWTEXT);
    if (settings.adaptiveColors) {
        bool light = theme == ElementTheme::Light;
        g_textBrush = nullptr;
        if (g_cachedHighContrast) {
            g_graphBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedWindowTextColor));
            g_warningBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedHotlightColor));
            g_criticalBrush =
                SolidColorBrush(ColorFromColorRef(g_cachedHighlightColor));
        } else {
            g_graphBrush = BrushFromSetting(
                light ? kLightGraphColor : kDefaultGraphColor,
                MakeColor(0xFF, 0x78, 0xA8, 0xFF));
            g_warningBrush = BrushFromSetting(
                light ? kLightWarningColor : kDefaultWarningColor,
                MakeColor(0xFF, 0xFF, 0xB9, 0x00));
            g_criticalBrush = BrushFromSetting(
                light ? kLightCriticalColor : kDefaultCriticalColor,
                MakeColor(0xFF, 0xFF, 0x6B, 0x6B));
        }
    } else {
        auto textColor = ParseColor(settings.textColor);
        g_textBrush = textColor ? SolidColorBrush(*textColor) : nullptr;
        g_graphBrush = BrushFromSetting(settings.graphColor,
                                        MakeColor(0xFF, 0x78, 0xA8, 0xFF));
        g_warningBrush = BrushFromSetting(settings.warningColor,
                                          MakeColor(0xFF, 0xFF, 0xB9, 0x00));
        g_criticalBrush = BrushFromSetting(
            settings.criticalColor, MakeColor(0xFF, 0xFF, 0x6B, 0x6B));
    }
    ApplyCachedBrushesToVisuals();
    ApplyThemeOpacities(settings);
}

SolidColorBrush AlertBrush(AlertLevel alert) {
    if (alert == AlertLevel::Critical) {
        return g_criticalBrush;
    }
    if (alert == AlertLevel::Warning) {
        return g_warningBrush;
    }
    return g_graphBrush;
}

void SetTextForeground(TextBlock text, AlertLevel alert) {
    if (!text) {
        return;
    }

    SolidColorBrush brush = alert == AlertLevel::Normal
                                ? g_textBrush
                                : AlertBrush(alert);
    if (brush) {
        text.Foreground(brush);
    } else {
        text.ClearValue(TextBlock::ForegroundProperty());
    }
}

void SetTextIfChanged(TextBlock text, const std::wstring& value) {
    if (!text) {
        return;
    }
    hstring current = text.Text();
    if (current.size() != value.size() ||
        !std::equal(value.begin(), value.end(), current.begin())) {
        text.Text(value);
    }
}

void PruneHistory(std::deque<HistorySample>& history,
                  SampleTime end,
                  int historySeconds) {
    auto cutoff = end - std::chrono::seconds(historySeconds);
    while (!history.empty() && history.front().time < cutoff) {
        history.pop_front();
    }
}

void ApplyHistorySample(std::deque<HistorySample>& history,
                        bool available,
                        double value,
                        SampleTime time,
                        int historySeconds) {
    history.push_back({time, available && std::isfinite(value)
                                 ? std::optional(std::clamp(value, 0.0, 100.0))
                                 : std::nullopt});
    PruneHistory(history, time, historySeconds);
}

struct SparklinePoint {
    float x;
    float y;
};
using SparklineRuns = std::vector<std::vector<SparklinePoint>>;

SparklineRuns BuildSparklineRuns(const std::deque<HistorySample>& history,
                                 int historySeconds,
                                 int updateInterval,
                                 double width,
                                 double height,
                                 SampleTime end = {}) {
    SparklineRuns runs;
    if (history.empty() || historySeconds <= 0 || width <= 1 || height <= 2) {
        return runs;
    }
    if (end == SampleTime{}) end = history.back().time;
    auto cutoff = end - std::chrono::seconds(historySeconds);
    bool connected = false;
    SampleTime previous{};
    for (const auto& sample : history) {
        if (sample.time < cutoff || !sample.value) {
            connected = false;
            continue;
        }
        double secondsSincePrevious =
            std::chrono::duration<double>(sample.time - previous).count();
        if (!connected || secondsSincePrevious > updateInterval * 1.5) {
            runs.emplace_back();
        }
        double age = std::chrono::duration<double>(end - sample.time).count();
        runs.back().push_back({
            static_cast<float>(width * (1.0 - age / historySeconds)),
            static_cast<float>(1.0 + (100.0 - *sample.value) / 100.0 *
                                        (height - 2.0))});
        previous = sample.time;
        connected = true;
    }
    // A single reading has no duration; don't connect it across a missing sample.
    std::erase_if(runs, [](const auto& run) { return run.size() < 2; });
    return runs;
}

void UpdateSparkline(XamlPath graph,
                     const std::deque<HistorySample>& history,
                     const ModSettings& settings,
                     SampleTime end) {
    if (!graph) {
        return;
    }
    auto runs = BuildSparklineRuns(history, settings.historySeconds,
                                   settings.updateInterval, g_graphWidth,
                                   kGraphHeight, end);
    graph.Visibility(runs.empty() || !g_layoutGraphsVisible ? Visibility::Collapsed : Visibility::Visible);
    auto geometry = graph.Data().try_as<PathGeometry>();
    if (!geometry) {
        geometry = PathGeometry();
        graph.Data(geometry);
    }
    auto figures = geometry.Figures();
    while (figures.Size() > runs.size()) {
        figures.RemoveAtEnd();
    }
    for (size_t i = 0; i < runs.size(); ++i) {
        if (i == figures.Size()) {
            PathFigure figure;
            figure.IsClosed(false);
            figure.IsFilled(false);
            figures.Append(figure);
        }
        auto figure = figures.GetAt(static_cast<uint32_t>(i));
        const auto& run = runs[i];
        figure.StartPoint(Point{run.front().x, run.front().y});
        auto segments = figure.Segments();
        while (segments.Size() + 1 > run.size()) {
            segments.RemoveAtEnd();
        }
        for (size_t j = 1; j < run.size(); ++j) {
            if (j > segments.Size()) {
                segments.Append(LineSegment());
            }
            auto segment = segments.GetAt(static_cast<uint32_t>(j - 1))
                               .as<LineSegment>();
            Point point{run[j].x, run[j].y};
            Point previous = segment.Point();
            if (point.X != previous.X || point.Y != previous.Y) {
                segment.Point(point);
            }
        }
    }
}

void UpdateMemoryBar(XamlRectangle fill,
                     double percent,
                     bool available,
                     AlertLevel alert) {
    if (!fill) {
        return;
    }
    fill.Width(available ? g_memoryBarWidth *
                               std::clamp(percent, 0.0, 100.0) / 100.0
                         : 0.0);
    fill.Fill(AlertBrush(alert));
}

template <typename F>
FrameworkElement FindChildRecursive(FrameworkElement element,
                                    F callback,
                                    int depth = 16) {
    if (!element || depth <= 0) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (callback(child)) {
            return child;
        }
        if (auto nested = FindChildRecursive(child, callback, depth - 1)) {
            return nested;
        }
    }
    return nullptr;
}

FrameworkElement FindDirectChildByName(FrameworkElement parent, PCWSTR name) {
    if (!parent) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        auto child = VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (child && child.Name() == name) {
            return child;
        }
    }
    return nullptr;
}

void ApplyTextStyle(TextBlock text,
                    bool label,
                    const ModSettings& settings) {
    if (!text) {
        return;
    }
    text.FontFamily(Media::FontFamily(settings.fontFamily));
    text.FontSize(settings.fontSize);
    text.FontWeight(label ? Text::FontWeights::SemiBold()
                          : Text::FontWeights::Normal());
    bool highContrast = settings.adaptiveColors && g_cachedHighContrast;
    ThemeOpacityValues opacities =
        ResolveThemeOpacities(highContrast, settings.textOpacity);
    text.Opacity(label ? opacities.label : opacities.value);
    text.TextWrapping(TextWrapping::NoWrap);
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    SetTextForeground(text, AlertLevel::Normal);
}

double WidgetHeightForTaskbar(double availableHeight) {
    return std::isfinite(availableHeight) && availableHeight > 0.0
               ? std::min(kWidgetHeight, availableHeight)
               : kWidgetHeight;
}

struct WidgetColumnWidths { double left, right, graph; };
WidgetColumnWidths ResolveWidgetColumns(double width) {
    width -= 2 * kWidgetSidePadding;
    double right = std::clamp(width * 0.38, 145.0, 170.0);
    double left = width - kColumnGap - right;
    return {left, right, std::max(24.0, left - kMetricLabelWidth - kMetricUsageWidth -
                                      kMetricTempWidth - kGraphLeftGap)};
}
// Keep GDI+ alive across cached font measurements and editor reopenings.
// MinGW's GenericTypographic wrapper retains a native handle between calls.
ULONG_PTR g_fontGraphicsToken = 0;
bool EnsureFontGraphics() {
    if (g_fontGraphicsToken) return true;
    Gdiplus::GdiplusStartupInput input;
    return Gdiplus::GdiplusStartup(&g_fontGraphicsToken, &input, nullptr) == Gdiplus::Ok;
}
void StopFontGraphics() {
    if (g_fontGraphicsToken) { Gdiplus::GdiplusShutdown(g_fontGraphicsToken); g_fontGraphicsToken = 0; }
}
std::unique_ptr<Gdiplus::Font> CreateWidgetPreviewFont(std::wstring name, double size, int weight) {
    int style = weight >= 600 ? Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular;
    if (name == L"Segoe UI" && weight == 600) { name = L"Segoe UI Semibold"; style = Gdiplus::FontStyleRegular; }
    auto font = std::make_unique<Gdiplus::Font>(name.c_str(), static_cast<float>(size), style, Gdiplus::UnitPixel);
    if (font->GetLastStatus() != Gdiplus::Ok)
        font = std::make_unique<Gdiplus::Font>(L"Segoe UI", static_cast<float>(size), style, Gdiplus::UnitPixel);
    return font;
}
enum class WidgetLayoutMode { Full, NoGraphs, CompactTwoRows, CompactOneRow };
struct WidgetFontMetrics {
    std::array<double, 12> widths{28, 34, 40, 43, 38, 48, 28, 34, 40, 43, 38, 48};
    double textHeight = 15;
};
struct WidgetLayout {
    WidgetLayoutMode mode = WidgetLayoutMode::Full;
    double width = 410, height = kWidgetHeight, scale = 1;
    double rowHeight = kRowHeight, rowGap = kRowGap;
    std::array<double, 4> groups{}; // CPU, GPU, RAM, VRAM.
    std::array<Rect, 12> cells{}; // CPU, RAM, GPU, VRAM, three fields each.
    std::array<Rect, 2> graphs{}, bars{};
    bool legacy = true, showGraphs = true, showMemoryDetails = true;
};
[[clang::no_destroy]] std::optional<WidgetLayout> g_widgetLayout;
[[clang::no_destroy]] std::wstring g_measuredFontFamily;
int g_measuredFontSize = 0;
[[clang::no_destroy]] std::array<std::wstring, 2> g_capacityBudgets{L"--/--G", L"--/--G"};
uint64_t g_capacityBudgetRevision = 0, g_measuredCapacityRevision = 0;
WidgetFontMetrics g_widgetFontMetrics;
uint64_t g_layoutApplyCount = 0;

// This cache is accessed only on the owning widget UI thread. Preview receives
// a value copy; no TextBlock or other XAML object crosses a taskbar thread.
WidgetFontMetrics MeasureWidgetFont(const ModSettings& settings) {
    if (g_measuredFontFamily == settings.fontFamily && g_measuredFontSize == settings.fontSize &&
        g_measuredCapacityRevision == g_capacityBudgetRevision)
        return g_widgetFontMetrics;
    // XAML and GDI+ can have different line metrics for the same family.
    // Reserve the larger measured bounds so destination previews cannot clip.
    if (!EnsureFontGraphics())
        throw hresult_error(E_FAIL, L"Cannot measure the native preview font");
    Gdiplus::Bitmap bitmap(1, 1, PixelFormat32bppPARGB);
    Gdiplus::Graphics graphics(&bitmap);
    auto labelFont = CreateWidgetPreviewFont(settings.fontFamily, settings.fontSize, 600);
    auto valueFont = CreateWidgetPreviewFont(settings.fontFamily, settings.fontSize, 400);
    Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
    format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);
    WidgetFontMetrics result;
    result.widths.fill(0); result.textHeight = 0;
    TextBlock probe;
    probe.FontFamily(Media::FontFamily(settings.fontFamily));
    probe.FontSize(settings.fontSize);
    probe.TextWrapping(TextWrapping::NoWrap);
    auto measure = [&](PCWSTR value, bool label) {
        probe.FontWeight(label ? Text::FontWeights::SemiBold() : Text::FontWeights::Normal());
        probe.Text(value);
        probe.Measure(Size{4096, 4096});
        auto size = probe.DesiredSize();
        Gdiplus::RectF native;
        auto status = graphics.MeasureString(value, static_cast<int>(wcslen(value)),
            label ? labelFont.get() : valueFont.get(), Gdiplus::PointF(0, 0),
            &format, &native);
        if (status != Gdiplus::Ok) throw hresult_error(E_FAIL, L"Cannot measure the native preview text");
        result.textHeight = std::max({result.textHeight, std::ceil(static_cast<double>(size.Height)),
                                     std::ceil(static_cast<double>(native.Height))});
        return std::ceil(std::max(static_cast<double>(size.Width), static_cast<double>(native.Width))) + 2;
    };
    for (auto [index, label] : {std::pair{0, L"CPU"}, {6, L"GPU"}, {3, L"RAM"}, {9, L"VRAM"}})
        result.widths[index] = measure(label, true);
    double percent = measure(L"--%", false), temperature = measure(L"--°C", false);
    for (int value = 0; value <= 100; ++value) {
        auto text = FormatPercent(value);
        percent = std::max(percent, measure(text.c_str(), false));
    }
    // Covers rounded Shared Memory/Registry and native thermal-zone values.
    for (int value = -74; value <= 200; ++value) {
        auto text = FormatTemperature(static_cast<double>(value));
        temperature = std::max(temperature, measure(text.c_str(), false));
    }
    for (int index : {1, 4, 7, 10}) result.widths[index] = percent;
    for (int index : {2, 8}) result.widths[index] = temperature;
    for (size_t memory = 0; memory < g_capacityBudgets.size(); ++memory) {
        double width = measure(L"--/--G", false);
        // Use the known total's digit count, reserving every possible digit
        // width so changing usage cannot resize the capacity column.
        for (wchar_t digit = L'0'; digit <= L'9'; ++digit) {
            auto text = g_capacityBudgets[memory];
            for (auto& ch : text) if (ch >= L'0' && ch <= L'9') ch = digit;
            width = std::max(width, measure(text.c_str(), false));
        }
        result.widths[memory == 0 ? 5 : 11] = width;
    }
    if (!std::isfinite(result.textHeight) || result.textHeight <= 0)
        throw hresult_error(E_FAIL, L"Cannot measure the widget font");
    g_measuredFontFamily = settings.fontFamily; g_measuredFontSize = settings.fontSize;
    g_measuredCapacityRevision = g_capacityBudgetRevision;
    return g_widgetFontMetrics = result;
}

bool UpdateCapacityBudgets(const MetricsSnapshot& snapshot) {
    bool changed = false;
    const double used[] = {snapshot.ramUsedGb, snapshot.vramUsedGb};
    const double total[] = {snapshot.ramTotalGb, snapshot.vramTotalGb};
    const bool available[] = {snapshot.ramAvailable, snapshot.vramAvailable};
    for (size_t i = 0; i < g_capacityBudgets.size(); ++i) {
        if (!available[i] || !std::isfinite(used[i]) || !std::isfinite(total[i]) || total[i] <= 0) continue;
        auto budget = FormatCapacity(std::max(used[i], total[i]), total[i], true);
        if (budget != g_capacityBudgets[i]) { g_capacityBudgets[i] = std::move(budget); changed = true; }
    }
    if (changed) ++g_capacityBudgetRevision;
    return changed;
}

WidgetLayout BuildWidgetLayout(const ModSettings& settings, const WidgetFontMetrics& font,
                               WidgetLayoutMode mode, double availableHeight, bool legacy = false) {
    WidgetLayout layout;
    layout.mode = mode; layout.legacy = legacy;
    layout.showGraphs = mode == WidgetLayoutMode::Full;
    layout.showMemoryDetails = mode == WidgetLayoutMode::Full || mode == WidgetLayoutMode::NoGraphs;
    bool single = mode == WidgetLayoutMode::CompactOneRow;
    auto widths = font.widths;
    // Identical field widths across paired rows keep changing values aligned.
    for (auto [a, b] : {std::pair{0, 6}, {1, 7}, {2, 8}, {3, 9}, {4, 10}, {5, 11}})
        widths[a] = widths[b] = std::max(widths[a], widths[b]);
    if (legacy) widths = {28, 34, 40, 43, 38, 0, 28, 34, 40, 43, 38, 0};
    if (!layout.showMemoryDetails) widths[5] = widths[11] = 0;
    double compute = widths[0] + widths[1] + widths[2];
    double memory = widths[3] + widths[4] + widths[5];
    if (legacy) {
        auto columns = ResolveWidgetColumns(settings.width);
        layout.width = settings.width;
        compute = columns.left; memory = columns.right;
        widths[5] = widths[11] = memory - widths[3] - widths[4];
    } else {
        if (layout.showGraphs) {
            compute += kGraphLeftGap + 24;
            double minimum = 2 * kWidgetSidePadding + kColumnGap + compute + memory;
            compute += std::max(0.0, settings.width - minimum);
        }
        layout.width = 2 * kWidgetSidePadding + (single ? 2 * compute + 2 * memory + 3 * kColumnGap
                                                       : compute + memory + kColumnGap);
        double minimumRow = font.textHeight + (layout.showMemoryDetails ? 2.25 : 0);
        if (layout.showGraphs) minimumRow = std::max(minimumRow, kGraphHeight + 1.25);
        double nominalRow = std::max(kRowHeight, minimumRow);
        availableHeight = std::isfinite(availableHeight) && availableHeight > 0 ? availableHeight : WidgetHeightForTaskbar(0);
        if (single) {
            layout.rowGap = 0;
            layout.rowHeight = std::max(minimumRow, std::min(nominalRow, availableHeight));
            layout.height = layout.rowHeight;
        } else {
            layout.rowGap = std::clamp(availableHeight - 2 * nominalRow, 0.0, kRowGap);
            layout.rowHeight = std::max(minimumRow, std::min(nominalRow, (availableHeight - layout.rowGap) / 2));
            layout.height = 2 * layout.rowHeight + layout.rowGap;
        }
    }
    layout.groups = {compute, compute, memory, memory};
    const int starts[] = {0, 6, 3, 9};
    double nextX = kWidgetSidePadding;
    for (int group = 0; group < 4; ++group) {
        int first = starts[group];
        double x = single ? nextX : kWidgetSidePadding + (group >= 2 ? compute + kColumnGap : 0);
        double y = single ? 0 : (group % 2) * (layout.rowHeight + layout.rowGap);
        double textHeight = layout.rowHeight - (!legacy && layout.showMemoryDetails && group >= 2 ? 2.25 : 0);
        double cellX = x;
        for (int field = 0; field < 3; ++field) {
            layout.cells[first + field] = {static_cast<float>(cellX), static_cast<float>(y),
                static_cast<float>(widths[first + field]), static_cast<float>(textHeight)};
            cellX += widths[first + field];
        }
        if (group < 2 && layout.showGraphs)
            layout.graphs[group] = {static_cast<float>(cellX + kGraphLeftGap),
                static_cast<float>(y + (layout.rowHeight - kGraphHeight) / 2),
                static_cast<float>(compute - (cellX - x) - kGraphLeftGap), static_cast<float>(kGraphHeight)};
        if (group >= 2 && layout.showMemoryDetails)
            layout.bars[group - 2] = {static_cast<float>(x), static_cast<float>(y + layout.rowHeight - 1.25),
                static_cast<float>(memory), 1.25f};
        nextX += layout.groups[group] + kColumnGap;
    }
    return layout;
}

bool SameWidgetLayout(const WidgetLayout& a, const WidgetLayout& b) {
    if (a.mode != b.mode || a.legacy != b.legacy || a.width != b.width || a.height != b.height ||
        a.rowHeight != b.rowHeight || a.rowGap != b.rowGap) return false;
    for (size_t i = 0; i < a.cells.size(); ++i) {
        const auto& x = a.cells[i]; const auto& y = b.cells[i];
        if (x.X != y.X || x.Y != y.Y || x.Width != y.Width || x.Height != y.Height) return false;
    }
    return a.groups == b.groups;
}

void ApplyWidgetGeometry(const ModSettings& settings, const WidgetLayout& layout) {
    if (!g_widget) return;
    if (g_widgetLayout && SameWidgetLayout(*g_widgetLayout, layout)) {
        g_widgetLayout->scale = layout.scale; return;
    }
    ++g_layoutApplyCount;
    ++g_widgetVisualRevision;
    double ramRatio = g_memoryBarWidth > 0 && g_ramFill ? g_ramFill.Width() / g_memoryBarWidth : 0;
    double vramRatio = g_memoryBarWidth > 0 && g_vramFill ? g_vramFill.Width() / g_memoryBarWidth : 0;
    g_widgetLayout = layout;
    g_layoutGraphsVisible = layout.showGraphs;
    g_graphWidth = layout.showGraphs ? layout.graphs[0].Width : 0;
    g_memoryBarWidth = layout.groups[2];
    g_widget.Width(layout.width); g_widget.Height(layout.height);
    g_widget.Padding(Thickness{kWidgetSidePadding, 0, kWidgetSidePadding, 0});
    bool single = layout.mode == WidgetLayoutMode::CompactOneRow;
    auto columns = g_widget.ColumnDefinitions();
    for (uint32_t i = 0; i < columns.Size(); ++i) {
        double width = 0;
        if (single) width = i % 2 ? kColumnGap : layout.groups[i / 2];
        else if (i <= 2) width = i == 1 ? kColumnGap : layout.groups[i == 0 ? 0 : 2];
        columns.GetAt(i).Width(GridLength{width, GridUnitType::Pixel});
    }
    auto rows = g_widget.RowDefinitions();
    rows.GetAt(0).Height(GridLength{layout.rowHeight, GridUnitType::Pixel});
    rows.GetAt(1).Height(GridLength{single ? 0 : layout.rowGap, GridUnitType::Pixel});
    rows.GetAt(2).Height(GridLength{single ? 0 : layout.rowHeight, GridUnitType::Pixel});
    const int starts[] = {0, 6, 3, 9};
    for (int group = 0; group < 4; ++group) {
        auto row = g_metricRows[group];
        if (!row) continue;
        Grid::SetColumn(row, single ? 2 * group : group < 2 ? 0 : 2);
        Grid::SetRow(row, single ? 0 : 2 * (group % 2));
        row.Height(layout.rowHeight);
        auto fields = row.ColumnDefinitions();
        for (int field = 0; field < 3; ++field)
            fields.GetAt(field).Width(GridLength{layout.cells[starts[group] + field].Width, GridUnitType::Pixel});
        if (group < 2) fields.GetAt(3).Width(GridLength{layout.showGraphs ? layout.graphs[group].Width + kGraphLeftGap : 0, GridUnitType::Pixel});
    }
    for (TextBlock text : {g_ramCapacityText, g_vramCapacityText})
        if (text) text.Visibility(layout.showMemoryDetails ? Visibility::Visible : Visibility::Collapsed);
    for (TextBlock text : {g_ramLabel, g_ramPercentText, g_ramCapacityText, g_vramLabel, g_vramPercentText, g_vramCapacityText})
        if (text) text.Margin(Thickness{0, 0, 0, !layout.legacy && layout.showMemoryDetails ? 2.25 : 0});
    for (XamlPath graph : {g_cpuGraph, g_gpuGraph}) {
        if (!graph) continue;
        graph.Width(g_graphWidth); graph.Height(kGraphHeight);
    }
    for (XamlRectangle bar : {g_ramTrack, g_vramTrack, g_ramFill, g_vramFill}) {
        if (!bar) continue;
        bar.Visibility(layout.showMemoryDetails ? Visibility::Visible : Visibility::Collapsed);
    }
    g_ramTrack.Width(g_memoryBarWidth); g_vramTrack.Width(g_memoryBarWidth);
    g_ramFill.Width(g_memoryBarWidth * ramRatio); g_vramFill.Width(g_memoryBarWidth * vramRatio);
    auto now = SampleTime::clock::now();
    UpdateSparkline(g_cpuGraph, g_cpuHistory, settings, now);
    UpdateSparkline(g_gpuGraph, g_gpuHistory, settings, now);
}

// Geometry is a value snapshot: no XAML references cross a taskbar thread.
struct OccupiedInterval {
    double left = 0.0;
    double right = 0.0;
    bool movable = false;
};
struct TaskbarGeometry {
    double width = 0.0;
    double height = 0.0;
    std::vector<OccupiedInterval> occupied;
    bool ready = false;
};
struct WidgetPointerAnchor {
    double x = 0; // Logical cursor position in the destination taskbar.
    double fraction = 0.5; // Relative grab point, including the Viewbox's side margins.
};
struct PreferredPosition {
    double left = 0.0;
    double previousLeft = 0.0;
    std::optional<double> fraction = std::nullopt; // Saved anchor uses the candidate's actual travel width.
    std::optional<WidgetPointerAnchor> pointer = std::nullopt;
};
struct TaskbarPlacement {
    double left = 0.0;
    double width = 0.0;
    double reserved = 0.0;
};
constexpr double kPlacementTolerance = 0.25;

bool Intersects(double left, double right, const OccupiedInterval& item) {
    return left < item.right - kPlacementTolerance &&
           right > item.left + kPlacementTolerance;
}
std::vector<OccupiedInterval> FreeTaskbarIntervals(
    double width, std::vector<OccupiedInterval> occupied) {
    std::vector<OccupiedInterval> free;
    std::erase_if(occupied, [](const auto& item) {
        return !std::isfinite(item.left) || !std::isfinite(item.right) ||
               item.right <= item.left;
    });
    std::sort(occupied.begin(), occupied.end(), [](const auto& a, const auto& b) {
        return a.left < b.left;
    });
    double end = 0.0;
    for (const auto& item : occupied) {
        double left = std::clamp(item.left, 0.0, width);
        double right = std::clamp(item.right, 0.0, width);
        if (left > end) free.push_back({end, left});
        end = std::max(end, right);
    }
    if (end < width) free.push_back({end, width});
    return free;
}
std::vector<OccupiedInterval> FreeWidgetIntervals(
    double width, std::vector<OccupiedInterval> occupied) {
    for (auto& item : occupied) {
        item.left -= kTaskbarClearance; item.right += kTaskbarClearance;
    }
    auto free = FreeTaskbarIntervals(width, std::move(occupied));
    for (auto& slot : free) {
        slot.left = std::max(slot.left, kTaskbarClearance);
        slot.right = std::min(slot.right, width - kTaskbarClearance);
    }
    std::erase_if(free, [](const auto& slot) { return slot.right <= slot.left; });
    return free;
}
bool PlacementFits(const TaskbarGeometry& geometry,
                   const TaskbarPlacement& placement) {
    if (!std::isfinite(placement.width) || !std::isfinite(placement.left) ||
        placement.width <= 0 || placement.left < kTaskbarClearance - kPlacementTolerance ||
        placement.left + placement.width > geometry.width - kTaskbarClearance + kPlacementTolerance)
        return false;
    for (const auto& item : geometry.occupied) {
        OccupiedInterval shifted = item;
        if (item.movable) {
            shifted.left += placement.reserved;
            shifted.right += placement.reserved;
            if (placement.reserved > 0 && shifted.right > geometry.width + kPlacementTolerance) return false;
            for (const auto& fixed : geometry.occupied)
                if (placement.reserved > 0 && !fixed.movable && Intersects(shifted.left, shifted.right, fixed))
                    return false;
        }
        shifted.left -= kTaskbarClearance; shifted.right += kTaskbarClearance;
        if (Intersects(placement.left, placement.left + placement.width, shifted))
            return false;
    }
    return true;
}
bool ReservedControlsFit(const TaskbarGeometry& arranged) {
    for (const auto& button : arranged.occupied) {
        if (!button.movable) continue;
        if (button.left < -kPlacementTolerance || button.right > arranged.width + kPlacementTolerance) return false;
        for (const auto& fixed : arranged.occupied)
            if (!fixed.movable && Intersects(button.left, button.right, fixed)) return false;
    }
    return true;
}

double PreferredLeftForWidget(const PreferredPosition& preferred, const TaskbarGeometry& geometry,
                              double width, double intrinsicWidth, double intrinsicHeight) {
    if (preferred.pointer) {
        double scale = std::min({1.0, width / intrinsicWidth,
            geometry.height > 0 ? geometry.height / intrinsicHeight : 1.0});
        double contentWidth = intrinsicWidth * scale;
        // Viewbox centers content when height limits its scale. Hold the same
        // relative point in that content, including on a different-sized layout.
        double offset = (width - contentWidth) / 2 + preferred.pointer->fraction * contentWidth;
        return preferred.pointer->x - std::clamp(offset, 0.0, width);
    }
    return preferred.fraction ? *preferred.fraction * std::max(0.0, geometry.width - width)
                              : preferred.left;
}

TaskbarPlacement ResolvePlacementForSize(const ModSettings& settings,
                                        const TaskbarGeometry& geometry,
                                        PreferredPosition preferred, double desiredWidth,
                                        double minimumScale, double contentHeight,
                                        bool allowReservation = true) {
    if (!geometry.ready || !std::isfinite(geometry.width) || geometry.width <= 0)
        return {};
    if (!std::isfinite(geometry.height) || !std::isfinite(desiredWidth) || desiredWidth <= 0 ||
        (geometry.height > 0 && geometry.height / contentHeight + 1e-6 < minimumScale)) return {};
    double minimumWidth = desiredWidth * minimumScale;
    // The full layout already incorporates the configured preferred width.
    // Adaptive layouts need their measured size, bounded by actual free gaps.
    double maximumWidth = desiredWidth;
    double reservationGap = std::max<double>(kTaskbarClearance, settings.reserveGap);
    std::vector<OccupiedInterval> fixed;
    double firstButton = geometry.width;
    double maximumShift = geometry.width;
    bool hasButtons = false;
    for (const auto& item : geometry.occupied) {
        if (!item.movable) fixed.push_back(item);
        else {
            hasButtons = true;
            firstButton = std::min(firstButton, item.left);
            maximumShift = std::min(maximumShift, geometry.width - item.right);
            for (const auto& obstacle : geometry.occupied)
                if (!obstacle.movable && obstacle.left >= item.right - kPlacementTolerance)
                    maximumShift = std::min(maximumShift, obstacle.left - item.right);
        }
    }
    // Prefer the largest readable size before distance. Otherwise saving the
    // snapped anchor can select a wider slot and a different adaptive layout.
    // Equal-distance choices stay near the preceding placement.
    for (bool shrink : {false, true}) {
        TaskbarPlacement best{};
        double bestDistance = HUGE_VAL, bestPrevious = HUGE_VAL;
        auto consider = [&](OccupiedInterval slot, bool reserve) {
            double width = shrink ? std::min<double>(maximumWidth, slot.right - slot.left)
                                  : maximumWidth;
            if (width + 1e-6 < minimumWidth || slot.right - slot.left + 1e-6 < width) return;
            double wanted = PreferredLeftForWidget(preferred, geometry, width, desiredWidth, contentHeight);
            double left = std::clamp(wanted, slot.left, std::max(slot.left, slot.right - width));
            double shift = reserve ? std::max(0.0, left + width + reservationGap - firstButton)
                                   : 0.0;
            TaskbarPlacement candidate{left, width, shift};
            if (!PlacementFits(geometry, candidate)) return;
            double distance = std::abs(left - wanted);
            double previous = std::abs(left - preferred.previousLeft);
            bool larger = width > best.width + 1e-6;
            bool sameSize = std::abs(width - best.width) <= 1e-6;
            if (larger || (sameSize && (distance < bestDistance - kPlacementTolerance ||
                (std::abs(distance - bestDistance) <= kPlacementTolerance &&
                 (previous < bestPrevious - kPlacementTolerance ||
                  (std::abs(previous - bestPrevious) <= kPlacementTolerance &&
                    candidate.reserved < best.reserved)))))) {
                best = candidate; bestDistance = distance; bestPrevious = previous;
            }
        };
        for (auto slot : FreeWidgetIntervals(geometry.width, geometry.occupied))
            consider(slot, false);
        if (settings.reserveSpace && allowReservation && hasButtons && maximumShift > 0) {
            for (auto slot : FreeWidgetIntervals(geometry.width, fixed)) {
                slot.right = std::min(slot.right, firstButton + maximumShift - reservationGap);
                if (slot.right > slot.left) consider(slot, true);
            }
        }
        if (best.width > 0) return best;
    }
    return {};
}

// The legacy solver remains independently testable; adaptive callers share
// the same obstacle/reservation algorithm with different measured size limits.
TaskbarPlacement ResolveTaskbarPlacement(const ModSettings& settings, const TaskbarGeometry& geometry,
                                        PreferredPosition preferred, bool allowReservation = true) {
    return ResolvePlacementForSize(settings, geometry, preferred, settings.width,
        std::max(0.85, 9.0 / settings.fontSize), kWidgetHeight, allowReservation);
}
struct WidgetPlacement { TaskbarPlacement placement; WidgetLayout layout; };
WidgetPlacement ResolveWidgetPlacement(const ModSettings& settings, const TaskbarGeometry& geometry,
                                       PreferredPosition preferred, const WidgetFontMetrics& font,
                                       bool allowReservation = true) {
    auto legacy = BuildWidgetLayout(settings, font, WidgetLayoutMode::Full, kWidgetHeight, true);
    auto evaluate = [&](WidgetLayout layout, double minimumScale) {
        auto placement = ResolvePlacementForSize(settings, geometry, preferred, layout.width,
                                                 minimumScale, layout.height, allowReservation);
        layout.scale = placement.width > 0 ? std::min({1.0, placement.width / layout.width,
            geometry.height > 0 ? geometry.height / layout.height : 1.0}) : 0;
        return WidgetPlacement{placement, layout};
    };
    auto result = evaluate(legacy, std::max(0.85, 9.0 / settings.fontSize));
    // Preserve the existing normal-taskbar layout, including its accepted shrinking.
    bool readableCells = font.textHeight <= kRowHeight;
    for (int index = 0; index < 12; ++index)
        // Existing fixed cells need the measured glyph width; the extra two
        // DIP are an overhang allowance used when allocating adaptive cells.
        readableCells = readableCells && font.widths[index] - 2 <= legacy.cells[index].Width;
    if (result.placement.width > 0 && readableCells) return result;
    for (auto mode : {WidgetLayoutMode::Full, WidgetLayoutMode::NoGraphs,
                     WidgetLayoutMode::CompactTwoRows, WidgetLayoutMode::CompactOneRow}) {
        result = evaluate(BuildWidgetLayout(settings, font, mode, geometry.height), 1);
        if (result.placement.width > 0) return result;
    }
    WidgetPlacement best{{}, legacy};
    best.layout.scale = 0;
    for (auto mode : {WidgetLayoutMode::CompactTwoRows, WidgetLayoutMode::CompactOneRow}) {
        result = evaluate(BuildWidgetLayout(settings, font, mode, geometry.height), 9.0 / settings.fontSize);
        if (result.placement.width > 0 && result.layout.scale > best.layout.scale + 1e-6) best = result;
    }
    return best;
}

bool IsTaskbarObstacle(FrameworkElement element) {
    auto name = element.Name();
    auto type = winrt::get_class_name(element);
    return name == L"SystemTrayFrame" || type == L"SystemTray.SystemTrayFrame" ||
           type == L"Taskbar.StartButton" || type == L"Taskbar.SearchBoxButton" ||
           type == L"Taskbar.SearchBoxLaunchListButton" || type == L"Taskbar.TaskViewButton" ||
           type == L"Taskbar.TaskListButton" || type == L"Taskbar.ExperienceToggleButton" ||
           type == L"Taskbar.OverflowToggleButton" || type == L"Taskbar.AugmentedEntryPointButton" ||
           type == L"SystemTray.CopilotIcon" ||
           element.try_as<Controls::Primitives::ButtonBase>() != nullptr;
}
struct CachedTaskbarElement {
    FrameworkElement element{nullptr};
    DependencyObject parent{nullptr};
    int childCount = 0;
    bool obstacle = false;
    bool movable = false;
};
struct TaskbarGeometryCache {
    FrameworkElement root{nullptr};
    FrameworkElement searchRoot{nullptr};
    FrameworkElement repeater{nullptr};
    std::vector<CachedTaskbarElement> elements;
    bool complete = false;
    size_t rebuilds = 0;
};
[[clang::no_destroy]] TaskbarGeometryCache g_geometryCache;
void CollectTaskbarObstacles(FrameworkElement element, FrameworkElement repeater,
                             bool movable, TaskbarGeometryCache& cache, int depth = 32) {
    if (depth <= 0) { cache.complete = false; return; }
    if (!element || element.Name() == kWidgetName) return;
    movable = movable || (repeater && element == repeater);
    bool obstacle = IsTaskbarObstacle(element);
    int count = obstacle ? 0 : VisualTreeHelper::GetChildrenCount(element);
    cache.elements.push_back({element, VisualTreeHelper::GetParent(element), count, obstacle, movable});
    if (obstacle) return; // Children are part of this button's hit area.
    for (int i = 0; i < count; ++i)
        CollectTaskbarObstacles(VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>(),
                               repeater, movable, cache, depth - 1);
}
bool GeometryCacheMatches(const TaskbarGeometryCache& cache, FrameworkElement root,
                          FrameworkElement searchRoot, FrameworkElement repeater) {
    if (cache.root != root || cache.searchRoot != searchRoot || cache.repeater != repeater || !cache.complete)
        return false;
    for (const auto& item : cache.elements) {
        if (VisualTreeHelper::GetParent(item.element) != item.parent ||
            (!item.obstacle && VisualTreeHelper::GetChildrenCount(item.element) != item.childCount)) return false;
    }
    return true;
}
bool VisibleInTaskbar(FrameworkElement element, FrameworkElement searchRoot) {
    for (int depth = 0; element && depth < 32; ++depth) {
        if (element.Visibility() != Visibility::Visible || element.Opacity() <= 0) return false;
        if (element == searchRoot) return true;
        element = VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    }
    return false;
}
TaskbarGeometry MeasureTaskbarGeometry(FrameworkElement root,
                                      FrameworkElement repeater,
                                      double ownReservation = 0.0,
                                      TaskbarGeometryCache* existingCache = nullptr) {
    TaskbarGeometry result;
    if (!root) return result;
    result.width = root.ActualWidth(); result.height = root.ActualHeight();
    result.ready = std::isfinite(result.width) && result.width > 0 &&
                   std::isfinite(result.height) && result.height > 0;
    auto searchRoot = root;
    if (auto xaml = root.XamlRoot())
        if (auto content = xaml.Content().try_as<FrameworkElement>()) searchRoot = content;
    TaskbarGeometryCache temporary;
    auto& cache = existingCache ? *existingCache : temporary;
    if (!GeometryCacheMatches(cache, root, searchRoot, repeater)) {
        cache.root = root; cache.searchRoot = searchRoot; cache.repeater = repeater;
        cache.elements.clear(); cache.complete = true; ++cache.rebuilds;
        // Cache hidden controls too: visibility changes need no rediscovery.
        CollectTaskbarObstacles(searchRoot, repeater, false, cache);
    }
    result.ready = result.ready && cache.complete;
    for (const auto& item : cache.elements) {
        auto element = item.element;
        if (!item.obstacle || !VisibleInTaskbar(element, searchRoot) ||
            element.ActualWidth() <= 0 || element.ActualHeight() <= 0) continue;
        try {
            auto rect = element.TransformToVisual(root).TransformBounds(
                Rect{0, 0, static_cast<float>(element.ActualWidth()),
                     static_cast<float>(element.ActualHeight())});
            if (!std::isfinite(rect.X) || !std::isfinite(rect.Width) || !std::isfinite(rect.Y) ||
                !std::isfinite(rect.Height)) { result.ready = false; continue; }
            if (rect.Y < result.height && rect.Y + rect.Height > 0) {
                double left = rect.X - (item.movable ? ownReservation : 0.0);
                result.occupied.push_back({left, left + rect.Width, item.movable});
            }
        } catch (...) {
            result.ready = false; // Never guess that an unmeasurable button is free.
        }
    }
    return result;
}
[[maybe_unused]] double TaskbarAvailableWidth() {
    if (!g_rootGrid) return 0;
    auto geometry = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater, g_reservedMargin);
    double width = geometry.width;
    if (g_systemTrayFrame && g_systemTrayFrame.ActualWidth() > 0) {
        try { width = std::min(width, static_cast<double>(g_systemTrayFrame.TransformToVisual(
                    g_rootGrid).TransformPoint({0, 0}).X)); } catch (...) {}
    }
    return std::max(0.0, width);
}
std::vector<OccupiedInterval> g_lastBaselineObstacles;
double g_lastGeometryWidth = 0.0;
double g_lastGeometryHeight = 0.0;

void ApplyTaskbarPlacement(const ModSettings& settings) {
    if (!g_widgetHost || !g_rootGrid || g_applyingTaskbarPlacement) return;
    if (g_reservationAwaitingLayout && g_layoutRevision <= g_reservationRevision) return;
    g_reservationAwaitingLayout = false;
    g_applyingTaskbarPlacement = true;
    struct Guard { ~Guard() { g_applyingTaskbarPlacement = false; } } guard;
    Thickness margin{};
    bool ownMargin = false;
    if (g_taskItemsRepeater) {
        margin = g_taskItemsRepeater.Margin();
        ownMargin = g_lastAppliedRepeaterMarginLeft &&
            std::abs(margin.Left - *g_lastAppliedRepeaterMarginLeft) < 0.01;
        if (ownMargin) margin.Left -= g_reservedMargin;
        else if (g_reservedMargin != 0) g_reservedMargin = 0;
    }
    auto geometry = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater,
                                          ownMargin ? g_reservedMargin : 0.0, &g_geometryCache);
    bool changed = geometry.occupied.size() != g_lastBaselineObstacles.size() ||
                   std::abs(geometry.width - g_lastGeometryWidth) > 0.5 ||
                   std::abs(geometry.height - g_lastGeometryHeight) > 0.5;
    if (!changed) for (size_t i = 0; i < geometry.occupied.size(); ++i) {
        const auto& a = geometry.occupied[i]; const auto& b = g_lastBaselineObstacles[i];
        if (a.movable != b.movable || std::abs(a.left - b.left) > 0.5 ||
            std::abs(a.right - b.right) > 0.5) { changed = true; break; }
    }
    if (g_rejectionNeedsBaseline) g_rejectionNeedsBaseline = false;
    else if (changed) g_reservationRejected = false;
    g_lastBaselineObstacles = geometry.occupied;
    g_lastGeometryWidth = geometry.width;
    g_lastGeometryHeight = geometry.height;
    // Validate the arranged result against its unshifted measurement. A styler
    // or layout manager need not move every button by the requested margin.
    if (ownMargin && g_reservedMargin > 0 && g_widgetHost.Visibility() == Visibility::Visible) {
        auto arranged = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater, 0, &g_geometryCache);
        if (!PlacementFits(arranged, {g_widgetHost.Margin().Left, g_widgetHost.Width(), 0}) ||
            !ReservedControlsFit(arranged)) {
            g_reservationRejected = true;
            g_rejectionNeedsBaseline = true;
            // Wait for the restored base layout. Subtracting a requested margin
            // cannot reconstruct it when a centered/styled group moved less.
            g_taskItemsRepeater.Margin(margin);
            g_reservedMargin = 0;
            g_lastAppliedRepeaterMarginLeft.reset();
            g_widgetHost.Visibility(Visibility::Collapsed);
            g_reservationAwaitingLayout = true;
            g_reservationRevision = g_layoutRevision;
            return;
        }
    }
    PreferredPosition preferred{static_cast<double>(settings.leftOffset), g_previousPlacementLeft.load(),
                                SavedTaskbarFraction(g_taskbarWindow.load())};
    auto resolved = ResolveWidgetPlacement(settings, geometry,
        preferred, MeasureWidgetFont(settings), !g_reservationRejected);
    auto placement = resolved.placement;
    ApplyWidgetGeometry(settings, resolved.layout);
    auto visibility = placement.width > 0 ? Visibility::Visible : Visibility::Collapsed;
    if (g_widgetHost.Visibility() != visibility) g_widgetHost.Visibility(visibility);
    if (!std::isfinite(g_widgetHost.Width()) || std::abs(g_widgetHost.Width() - placement.width) > 0.01)
        g_widgetHost.Width(placement.width);
    double height = std::isfinite(geometry.height) && geometry.height > 0
        ? std::min(resolved.layout.height, geometry.height) : resolved.layout.height;
    if (!std::isfinite(g_widgetHost.Height()) || std::abs(g_widgetHost.Height() - height) > 0.01)
        g_widgetHost.Height(height);
    if (std::abs(g_widgetHost.Margin().Left - placement.left) > 0.01)
        g_widgetHost.Margin(Thickness{placement.left, 0, 0, 0});
    g_previousPlacementLeft = placement.left;
    if (g_taskItemsRepeater) {
        double base = margin.Left;
        margin.Left += placement.reserved;
        if (std::abs(g_taskItemsRepeater.Margin().Left - margin.Left) > 0.01) {
            g_reservationAwaitingLayout = true;
            g_reservationRevision = g_layoutRevision;
            g_taskItemsRepeater.Margin(margin);
        }
        g_reservedMargin = g_taskItemsRepeater.Margin().Left - base;
        g_lastAppliedRepeaterMarginLeft = g_reservedMargin != 0
            ? std::optional<double>{g_taskItemsRepeater.Margin().Left} : std::nullopt;
    }
}
void RefreshTaskbarPlacement() {
    if (g_unloading) return;
    try { ApplyTaskbarPlacement(*CurrentSettings()); }
    catch (...) { Wh_Log(L"Taskbar geometry update failed: %08X", static_cast<unsigned>(winrt::to_hresult())); }
}

void UpdateTimerInterval();

void ApplyWidgetSettings() {
    if (!g_widget) {
        return;
    }
    auto settingsSnapshot = CurrentSettings();
    const ModSettings& settings = *settingsSnapshot;
    EnsurePlacementControl(settings);
    RefreshThemeBrushes(settings);
    if (g_historyInterval != settings.updateInterval ||
        g_historyWindow != settings.historySeconds) {
        g_cpuHistory.clear();
        g_gpuHistory.clear();
        g_historyInterval = settings.updateInterval;
        g_historyWindow = settings.historySeconds;
    }
    g_widget.Margin(Thickness{});
    g_widget.HorizontalAlignment(HorizontalAlignment::Left);
    g_widget.VerticalAlignment(VerticalAlignment::Center);
    g_widget.IsHitTestVisible(false);

    for (TextBlock label :
         {g_cpuLabel, g_gpuLabel, g_ramLabel, g_vramLabel}) {
        ApplyTextStyle(label, true, settings);
    }
    for (TextBlock value : {g_cpuUsageText, g_cpuTempText, g_gpuUsageText,
                            g_gpuTempText, g_ramPercentText,
                            g_ramCapacityText, g_vramPercentText,
                            g_vramCapacityText}) {
        ApplyTextStyle(value, false, settings);
    }

    for (XamlPath graph : {g_cpuGraph, g_gpuGraph}) {
        if (graph) {
            graph.Stroke(g_graphBrush);
            graph.StrokeThickness(1.25);
            graph.StrokeStartLineCap(PenLineCap::Round);
            graph.StrokeEndLineCap(PenLineCap::Round);
            graph.StrokeLineJoin(PenLineJoin::Round);
        }
    }
    for (XamlRectangle track : {g_ramTrack, g_vramTrack}) {
        if (track) {
            track.Fill(g_graphBrush);
        }
    }
    for (XamlRectangle fill : {g_ramFill, g_vramFill}) {
        if (fill) {
            fill.Fill(g_graphBrush);
        }
    }

    ++g_widgetVisualRevision;
    ApplyTaskbarPlacement(settings);
    auto now = SampleTime::clock::now();
    UpdateSparkline(g_cpuGraph, g_cpuHistory, settings, now);
    UpdateSparkline(g_gpuGraph, g_gpuHistory, settings, now);

    UpdateTimerInterval();
}

bool MetricsSnapshotIsFresh(const MetricsSnapshot& snapshot,
                             const ModSettings& settings,
                             SampleTime now) {
    return snapshot.capturedAt != SampleTime{} && now >= snapshot.capturedAt &&
           now - snapshot.capturedAt <=
               std::chrono::seconds(std::max(5, settings.updateInterval * 3));
}

void UpdateWidgetText(bool force = false) {
    if (!g_widget || g_unloading) {
        return;
    }
    MetricsSnapshot snapshot;
    uint64_t metricsSequence = 0;
    std::vector<MetricsSnapshot> newSnapshots;
    if (!GetMetricsSince(g_lastRenderedMetricsSequence, snapshot,
                         metricsSequence, newSnapshots)) {
        return;
    }
    bool hasNewSample = !newSnapshots.empty();
    auto settingsSnapshot = CurrentSettings();
    const ModSettings& settings = *settingsSnapshot;
    auto now = SampleTime::clock::now();
    bool fresh = MetricsSnapshotIsFresh(snapshot, settings, now);
    bool capacityChanged = fresh && UpdateCapacityBudgets(snapshot);
    if (!force && !hasNewSample && fresh) {
        return;
    }
    if (!fresh) snapshot = {};

    g_cpuTemperatureAlert = snapshot.cpuTemp
                                ? EvaluateAlert(*snapshot.cpuTemp,
                                                settings.cpuWarningTemp,
                                                settings.cpuCriticalTemp,
                                                g_cpuTemperatureAlert, 3.0)
                                : AlertLevel::Normal;
    g_gpuTemperatureAlert = snapshot.gpuTemp
                                ? EvaluateAlert(*snapshot.gpuTemp,
                                                settings.gpuWarningTemp,
                                                settings.gpuCriticalTemp,
                                                g_gpuTemperatureAlert, 3.0)
                                : AlertLevel::Normal;
    g_ramAlert = snapshot.ramAvailable
                     ? EvaluateAlert(snapshot.ram,
                                     settings.memoryWarningPercent,
                                     settings.memoryCriticalPercent, g_ramAlert,
                                     3.0)
                     : AlertLevel::Normal;
    g_vramAlert =
        snapshot.vramAvailable
            ? EvaluateAlert(snapshot.vram, settings.memoryWarningPercent,
                            settings.memoryCriticalPercent, g_vramAlert, 3.0)
            : AlertLevel::Normal;

    SetTextIfChanged(g_cpuUsageText,
                     snapshot.cpuAvailable ? FormatPercent(snapshot.cpu)
                                           : L"--%");
    if (g_cpuTempText) {
        SetTextIfChanged(g_cpuTempText, FormatTemperature(snapshot.cpuTemp));
        SetTextForeground(g_cpuTempText, g_cpuTemperatureAlert);
    }
    SetTextIfChanged(g_gpuUsageText, snapshot.gpuAvailable
                                         ? FormatPercent(snapshot.gpu)
                                         : L"--%");
    if (g_gpuTempText) {
        SetTextIfChanged(g_gpuTempText, FormatTemperature(snapshot.gpuTemp));
        SetTextForeground(g_gpuTempText, g_gpuTemperatureAlert);
    }
    if (g_ramPercentText) {
        SetTextIfChanged(g_ramPercentText,
                         snapshot.ramAvailable ? FormatPercent(snapshot.ram)
                                               : L"--%");
        SetTextForeground(g_ramPercentText, g_ramAlert);
    }
    SetTextIfChanged(g_ramCapacityText,
                     FormatCapacity(snapshot.ramUsedGb, snapshot.ramTotalGb,
                                    snapshot.ramAvailable));
    if (g_vramPercentText) {
        SetTextIfChanged(g_vramPercentText, snapshot.vramAvailable
                                                ? FormatPercent(snapshot.vram)
                                                : L"--%");
        SetTextForeground(g_vramPercentText, g_vramAlert);
    }
    SetTextIfChanged(g_vramCapacityText,
                     FormatCapacity(snapshot.vramUsedGb, snapshot.vramTotalGb,
                                    snapshot.vramAvailable));

    if (hasNewSample) {
        for (const MetricsSnapshot& newSnapshot : newSnapshots) {
            ApplyHistorySample(g_cpuHistory, newSnapshot.cpuAvailable,
                               newSnapshot.cpu, newSnapshot.capturedAt,
                               settings.historySeconds);
            ApplyHistorySample(g_gpuHistory, newSnapshot.gpuAvailable,
                               newSnapshot.gpu, newSnapshot.capturedAt,
                               settings.historySeconds);
        }
        g_lastRenderedMetricsSequence = metricsSequence;
        UpdateTimerInterval();
    }
    if (!fresh) {
        PruneHistory(g_cpuHistory, now, settings.historySeconds);
        PruneHistory(g_gpuHistory, now, settings.historySeconds);
    }
    if (hasNewSample || !fresh) {
        UpdateSparkline(g_cpuGraph, g_cpuHistory, settings, now);
        UpdateSparkline(g_gpuGraph, g_gpuHistory, settings, now);
    }
    UpdateMemoryBar(g_ramFill, snapshot.ram, snapshot.ramAvailable, g_ramAlert);
    UpdateMemoryBar(g_vramFill, snapshot.vram, snapshot.vramAvailable,
                    g_vramAlert);
    if (capacityChanged) ApplyTaskbarPlacement(settings);
    ++g_widgetVisualRevision;
}

void EnsureConfiguredTaskbarPlacement();
void ResetPlacementRetryState();
HWND FindPrimaryTaskbarWindow();
bool IsCurrentProcessTaskbarWindow(HWND window, bool* secondary);

void RefreshTaskbarUi(bool refreshSystemState = true) {
    if (g_unloading || g_refreshInProgress) {
        return;
    }
    // Placement can synchronously dispatch to a taskbar window. Don't reenter
    // a move or consume the same snapshot from a nested window message.
    g_refreshInProgress = true;
    try {
        bool force = refreshSystemState && g_widget && SystemColorsChanged();
        if (force) {
            RefreshThemeBrushes(*CurrentSettings());
        }
        if (refreshSystemState || g_placementApplyPending) {
            EnsureConfiguredTaskbarPlacement();
        }
        UpdateWidgetText(force);
    } catch (...) {
        Wh_Log(L"Taskbar update failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
    g_refreshInProgress = false;
}

LRESULT CALLBACK TaskbarNotificationsProc(HWND window, UINT message,
                                         WPARAM wParam, LPARAM lParam,
                                         UINT_PTR subclassId, DWORD_PTR) {
    if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(window, TaskbarNotificationsProc, subclassId);
        HWND expected = window;
        g_notificationWindow.compare_exchange_strong(expected, nullptr);
        g_placementApplyPending = true;
        ResetPlacementRetryState();
    } else if (!g_unloading) {
        if (message == g_taskbarRefreshMessage.load()) {
            RefreshTaskbarUi(wParam != 0);
            return 0;
        }
        if (message == WM_DISPLAYCHANGE || message == WM_SETTINGCHANGE) {
            CancelMoveEditor();
            InvalidateMonitorKeys();
            g_placementApplyPending = true;
            ResetPlacementRetryState();
            PostTaskbarRefresh(true);
        } else if (message == WM_THEMECHANGED || message == WM_SYSCOLORCHANGE) {
            PostTaskbarRefresh(true);
        }
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

bool RemoveTaskbarNotifications() {
    HWND window = g_notificationWindow.load();
    if (!window) {
        return true;
    }
    if (GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId() ||
        !RemoveWindowSubclass(window, TaskbarNotificationsProc, 1)) {
        Wh_Log(L"Removing taskbar notification handler failed");
        return false;
    }
    g_notificationWindow = nullptr;
    return true;
}

void EnsureTaskbarNotifications() {
    if (g_notificationWindow || g_unloading) {
        return;
    }
    HWND window = FindPrimaryTaskbarWindow();
    if (!window ||
        GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) {
        window = nullptr;
        EnumThreadWindows(
            GetCurrentThreadId(),
            [](HWND candidate, LPARAM context) -> BOOL {
                if (IsCurrentProcessTaskbarWindow(candidate, nullptr)) {
                    *reinterpret_cast<HWND*>(context) = candidate;
                    return FALSE;
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&window));
    }
    if (!window) {
        return;
    }
    if (!g_taskbarRefreshMessage) {
        g_taskbarRefreshMessage = RegisterWindowMessageW(
            L"Windhawk_TaskbarSystemInfo_Refresh_" WH_MOD_ID);
    }
    if (g_taskbarRefreshMessage &&
        SetWindowSubclass(window, TaskbarNotificationsProc, 1, 0)) {
        g_notificationWindow = window;
        g_taskbarUiResourcesRegistered = true;
    }
}

std::chrono::milliseconds UiTimerInterval(bool hasWidget, bool hasSample,
                                          int updateInterval) {
    if (!hasWidget) {
        return std::chrono::milliseconds(1000);
    }
    return hasSample ? std::chrono::milliseconds(updateInterval * 1000)
                     : std::chrono::milliseconds(250);
}

void UpdateTimerInterval() {
    if (!g_timer) {
        return;
    }
    auto interval = UiTimerInterval(static_cast<bool>(g_widget),
                                    g_lastRenderedMetricsSequence != 0,
                                    CurrentSettings()->updateInterval);
    if (g_timer.Interval() != interval) {
        g_timer.Interval(interval);
    }
}

void EnsureTimer() {
    EnsureTaskbarNotifications();
    if (g_timer) {
        UpdateTimerInterval();
        return;
    }
    // All delegates and the subclass must be revoked on this same UI thread.
    if (!g_taskbarThreadId.load()) {
        g_taskbarThreadId = GetCurrentThreadId();
    }
    g_timer = DispatcherTimer();
    g_taskbarUiResourcesRegistered = true;
    UpdateTimerInterval();
    g_timerToken = g_timer.Tick([](IInspectable const&, IInspectable const&) {
        // Watchdog for failed posts/temporarily absent taskbar windows. New
        // samples and theme/display changes normally render via notifications.
        EnsureTaskbarNotifications();
        RefreshTaskbarUi();
    });
    g_timer.Start();
}

bool StopTimer() {
    if (g_timer) {
        try {
            g_timer.Stop();
        } catch (...) {
            Wh_Log(L"Stopping taskbar timer failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
        try {
            g_timer.Tick(g_timerToken);
        } catch (...) {
            Wh_Log(L"Removing taskbar timer handler failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
        g_timer = nullptr;
        g_timerToken = {};
    }
    return true;
}

ColumnDefinition PixelColumn(double width) {
    ColumnDefinition column;
    column.Width(GridLength{width, GridUnitType::Pixel});
    return column;
}

RowDefinition PixelRow(double height) {
    RowDefinition row;
    row.Height(GridLength{height, GridUnitType::Pixel});
    return row;
}

TextBlock CreateCellText(PCWSTR name, TextAlignment alignment) {
    TextBlock text;
    text.Name(name);
    text.HorizontalAlignment(HorizontalAlignment::Stretch);
    text.VerticalAlignment(VerticalAlignment::Center);
    text.TextAlignment(alignment);
    text.TextWrapping(TextWrapping::NoWrap);
    text.TextTrimming(TextTrimming::CharacterEllipsis);
    text.IsHitTestVisible(false);
    return text;
}

Grid CreateComputeRow(PCWSTR label,
                      PCWSTR prefix,
                      TextBlock& labelText,
                      TextBlock& usageText,
                      TextBlock& temperatureText,
                      XamlPath& graph) {
    Grid row;
    row.Height(kRowHeight);
    row.IsHitTestVisible(false);

    row.ColumnDefinitions().Append(PixelColumn(kMetricLabelWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMetricUsageWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMetricTempWidth));
    ColumnDefinition graphColumn;
    graphColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(graphColumn);

    std::wstring labelName = std::wstring(prefix) + L"Label";
    labelText = CreateCellText(labelName.c_str(), TextAlignment::Left);
    labelText.Text(label);

    std::wstring usageName = std::wstring(prefix) + L"Usage";
    usageText = CreateCellText(usageName.c_str(), TextAlignment::Right);
    usageText.Text(L"--%");

    std::wstring temperatureName = std::wstring(prefix) + L"Temperature";
    temperatureText =
        CreateCellText(temperatureName.c_str(), TextAlignment::Right);
    temperatureText.Text(L"--°C");

    graph = XamlPath();
    graph.Name((std::wstring(prefix) + L"History").c_str());
    graph.HorizontalAlignment(HorizontalAlignment::Left);
    graph.VerticalAlignment(VerticalAlignment::Center);
    graph.Margin(Thickness{kGraphLeftGap, 0, 0, 0});
    graph.Stretch(Stretch::None);
    graph.IsHitTestVisible(false);

    Grid::SetColumn(labelText, 0);
    Grid::SetColumn(usageText, 1);
    Grid::SetColumn(temperatureText, 2);
    Grid::SetColumn(graph, 3);
    row.Children().Append(labelText);
    row.Children().Append(usageText);
    row.Children().Append(temperatureText);
    row.Children().Append(graph);
    return row;
}

Grid CreateMemoryRow(PCWSTR label,
                     PCWSTR prefix,
                     TextBlock& labelText,
                     TextBlock& percentText,
                     TextBlock& capacityText,
                     XamlRectangle& track,
                     XamlRectangle& fill) {
    Grid row;
    row.Height(kRowHeight);
    row.IsHitTestVisible(false);

    row.ColumnDefinitions().Append(PixelColumn(kMemoryLabelWidth));
    row.ColumnDefinitions().Append(PixelColumn(kMemoryPercentWidth));
    ColumnDefinition capacityColumn;
    capacityColumn.Width(GridLength{1, GridUnitType::Star});
    row.ColumnDefinitions().Append(capacityColumn);

    track = XamlRectangle();
    track.Name((std::wstring(prefix) + L"Track").c_str());
    track.Height(1.25);
    track.HorizontalAlignment(HorizontalAlignment::Left);
    track.VerticalAlignment(VerticalAlignment::Bottom);
    track.RadiusX(0.625);
    track.RadiusY(0.625);
    track.IsHitTestVisible(false);

    fill = XamlRectangle();
    fill.Name((std::wstring(prefix) + L"Fill").c_str());
    fill.Height(1.25);
    fill.HorizontalAlignment(HorizontalAlignment::Left);
    fill.VerticalAlignment(VerticalAlignment::Bottom);
    fill.RadiusX(0.625);
    fill.RadiusY(0.625);
    fill.IsHitTestVisible(false);

    std::wstring labelName = std::wstring(prefix) + L"Label";
    labelText = CreateCellText(labelName.c_str(), TextAlignment::Left);
    labelText.Text(label);

    std::wstring percentName = std::wstring(prefix) + L"Percent";
    percentText = CreateCellText(percentName.c_str(), TextAlignment::Right);
    percentText.Text(L"--%");

    std::wstring capacityName = std::wstring(prefix) + L"Capacity";
    capacityText = CreateCellText(capacityName.c_str(), TextAlignment::Right);
    capacityText.Text(L"--/--G");

    Grid::SetColumnSpan(track, 3);
    Grid::SetColumnSpan(fill, 3);
    Grid::SetColumn(labelText, 0);
    Grid::SetColumn(percentText, 1);
    Grid::SetColumn(capacityText, 2);
    row.Children().Append(track);
    row.Children().Append(fill);
    row.Children().Append(labelText);
    row.Children().Append(percentText);
    row.Children().Append(capacityText);
    return row;
}

bool RemoveWidget() {
    RemovePlacementControl();
    if (g_widget && g_actualThemeChangedToken.value) {
        try {
            g_widget.ActualThemeChanged(g_actualThemeChangedToken);
        } catch (...) {
            HRESULT error = winrt::to_hresult();
            Wh_Log(L"Removing taskbar theme handler failed: %08X",
                   static_cast<unsigned>(error));
            return false;
        }
    }
    g_actualThemeChangedToken = {};
    if (g_rootGrid && g_rootSizeChangedToken.value) {
        try {
            g_rootGrid.SizeChanged(g_rootSizeChangedToken);
        } catch (...) {
            Wh_Log(L"Removing taskbar size handler failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
    }
    g_rootSizeChangedToken = {};
    if (g_rootGrid && g_rootLayoutUpdatedToken.value) {
        try {
            g_rootGrid.LayoutUpdated(g_rootLayoutUpdatedToken);
        } catch (...) {
            Wh_Log(L"Removing taskbar layout handler failed: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
            return false;
        }
    }
    g_rootLayoutUpdatedToken = {};

    if (g_taskItemsRepeater && g_reservedMargin != 0.0) {
        Thickness margin = g_taskItemsRepeater.Margin();
        if (g_lastAppliedRepeaterMarginLeft &&
            std::abs(margin.Left - *g_lastAppliedRepeaterMarginLeft) < 0.01) {
            margin.Left -= g_reservedMargin;
            g_taskItemsRepeater.Margin(margin);
        } else {
            Wh_Log(L"Taskbar repeater margin changed externally; leaving the "
                   L"external value intact");
        }
    }
    g_reservedMargin = 0.0;
    g_lastAppliedRepeaterMarginLeft.reset();
    g_reservationAwaitingLayout = false;
    g_reservationRejected = false;
    g_rejectionNeedsBaseline = false;
    g_lastBaselineObstacles.clear();
    g_geometryCache = {}; // Release cached XAML references on their owning thread.

    if (g_rootGrid && g_widgetHost) {
        uint32_t index = 0;
        if (g_rootGrid.Children().IndexOf(g_widgetHost, index)) {
            g_rootGrid.Children().RemoveAt(index);
        }
    }

    g_widget = nullptr;
    g_widgetLayout.reset();
    for (auto& row : g_metricRows) row = nullptr;
    g_layoutGraphsVisible = true;
    g_capacityBudgets = {L"--/--G", L"--/--G"}; ++g_capacityBudgetRevision;
    g_measuredFontFamily.clear(); g_measuredFontSize = 0;
    g_widgetHost = nullptr;
    g_rootGrid = nullptr;
    g_taskItemsRepeater = nullptr;
    g_systemTrayFrame = nullptr;
    g_cpuLabel = nullptr;
    g_cpuUsageText = nullptr;
    g_cpuTempText = nullptr;
    g_gpuLabel = nullptr;
    g_gpuUsageText = nullptr;
    g_gpuTempText = nullptr;
    g_ramLabel = nullptr;
    g_ramPercentText = nullptr;
    g_ramCapacityText = nullptr;
    g_vramLabel = nullptr;
    g_vramPercentText = nullptr;
    g_vramCapacityText = nullptr;
    g_cpuGraph = nullptr;
    g_gpuGraph = nullptr;
    g_ramTrack = nullptr;
    g_ramFill = nullptr;
    g_vramTrack = nullptr;
    g_vramFill = nullptr;
    g_leftColumn = nullptr;
    g_gapColumn = nullptr;
    g_rightColumn = nullptr;
    g_textBrush = nullptr;
    g_graphBrush = nullptr;
    g_warningBrush = nullptr;
    g_criticalBrush = nullptr;
    g_cachedWidgetTheme = ElementTheme::Default;
    g_themeBrushesInitialized = false;
    g_cachedHighContrast = false;
    g_cachedHighlightColor = CLR_INVALID;
    g_cachedHotlightColor = CLR_INVALID;
    g_cachedWindowTextColor = CLR_INVALID;
    g_cpuHistory.clear();
    g_gpuHistory.clear();
    g_lastRenderedMetricsSequence = 0;
    g_cpuTemperatureAlert = AlertLevel::Normal;
    g_gpuTemperatureAlert = AlertLevel::Normal;
    g_ramAlert = AlertLevel::Normal;
    g_vramAlert = AlertLevel::Normal;
    UpdateTimerInterval();
    return true;
}

bool InjectWidget(FrameworkElement taskbarFrame) {
    if (!taskbarFrame || g_unloading) {
        return false;
    }

    auto root = FindDirectChildByName(taskbarFrame, L"RootGrid").try_as<Grid>();
    if (!root) {
        Wh_Log(L"Taskbar RootGrid not found");
        return false;
    }

    auto children = root.Children();
    for (uint32_t index = 0; index < children.Size();) {
        auto element = children.GetAt(index).try_as<FrameworkElement>();
        if (!element || element.Name() != kWidgetName) {
            index++;
            continue;
        }

        uint32_t currentWidgetIndex = 0;
        if (g_widgetHost && children.IndexOf(g_widgetHost, currentWidgetIndex) &&
            currentWidgetIndex == index) {
            ApplyWidgetSettings();
            if (!StartMetricsWorker()) {
                Wh_Log(L"Metrics worker unavailable");
            }
            EnsureTimer();
            UpdateWidgetText(true);
            return true;
        }

        Wh_Log(L"Removing stale Taskbar System Info widget");
        children.RemoveAt(index);
    }

    // A retry-only timer can exist before the first successful injection. Keep
    // it alive so an injection started by its Tick callback doesn't tear down
    // and recreate the very timer that's currently dispatching the callback.
    if (g_widget || g_rootGrid || g_taskItemsRepeater) {
        if (!RemoveWidget()) {
            Wh_Log(L"Removing the previous widget before reinjection failed");
            return false;
        }
    }

    Grid widget;
    widget.IsHitTestVisible(false);
    Viewbox host;
    host.Name(kWidgetName);
    host.IsHitTestVisible(false);
    host.Stretch(Stretch::Uniform);
    host.StretchDirection(StretchDirection::DownOnly);
    host.HorizontalAlignment(HorizontalAlignment::Left);
    host.VerticalAlignment(VerticalAlignment::Center);
    host.Child(widget);
    Canvas::SetZIndex(host, 10000);
    Grid::SetColumn(host, 0);
    Grid::SetColumnSpan(host,
                        std::max(1, static_cast<int>(root.ColumnDefinitions().Size())));

    g_leftColumn = ColumnDefinition();
    g_gapColumn = ColumnDefinition();
    g_rightColumn = ColumnDefinition();
    widget.ColumnDefinitions().Append(g_leftColumn);
    widget.ColumnDefinitions().Append(g_gapColumn);
    widget.ColumnDefinitions().Append(g_rightColumn);

    // Extra columns collapse to zero in the original two-row layout.
    for (int i = 0; i < 4; ++i) widget.ColumnDefinitions().Append(PixelColumn(0));
    widget.RowDefinitions().Append(PixelRow(kRowHeight));
    widget.RowDefinitions().Append(PixelRow(kRowGap));
    widget.RowDefinitions().Append(PixelRow(kRowHeight));

    Grid cpuRow = CreateComputeRow(L"CPU", L"Cpu", g_cpuLabel,
                                   g_cpuUsageText, g_cpuTempText, g_cpuGraph);
    Grid gpuRow = CreateComputeRow(L"GPU", L"Gpu", g_gpuLabel,
                                   g_gpuUsageText, g_gpuTempText, g_gpuGraph);
    Grid::SetRow(cpuRow, 0);
    Grid::SetRow(gpuRow, 2);


    Grid ramRow = CreateMemoryRow(L"RAM", L"Ram", g_ramLabel,
                                  g_ramPercentText, g_ramCapacityText,
                                  g_ramTrack, g_ramFill);
    Grid vramRow = CreateMemoryRow(L"VRAM", L"Vram", g_vramLabel,
                                   g_vramPercentText, g_vramCapacityText,
                                   g_vramTrack, g_vramFill);
    Grid::SetRow(ramRow, 0);
    Grid::SetRow(vramRow, 2);
    g_metricRows = {cpuRow, gpuRow, ramRow, vramRow};
    Grid::SetColumn(ramRow, 2); Grid::SetColumn(vramRow, 2);
    for (auto row : g_metricRows) widget.Children().Append(row);
    root.Children().Append(host);
    g_taskbarUiResourcesRegistered = true;

    g_rootGrid = root;
    g_widget = widget;
    g_widgetHost = host;
    g_taskItemsRepeater =
        FindDirectChildByName(root, L"TaskbarFrameRepeater");
    FrameworkElement searchRoot = taskbarFrame;
    if (auto xamlRoot = taskbarFrame.XamlRoot()) {
        if (auto content = xamlRoot.Content().try_as<FrameworkElement>()) {
            searchRoot = content;
        }
    }
    g_systemTrayFrame = FindChildRecursive(searchRoot, [](FrameworkElement child) {
        return winrt::get_class_name(child) == L"SystemTray.SystemTrayFrame" ||
               child.Name() == L"SystemTrayFrame";
    });
    auto sizeChanged = [](auto const&, auto const&) { QueueTaskbarPlacement(); };
    g_rootSizeChangedToken = root.SizeChanged(sizeChanged);
    // SizeChanged can run before sibling positions have finished arranging.
    // LayoutUpdated also covers a tray moved without a size change by a styler.
    // Placement writes only changed properties, so the extra pass settles.
    g_rootLayoutUpdatedToken = root.LayoutUpdated([](auto const&, auto const&) {
        ++g_layoutRevision;
        QueueTaskbarPlacement();
    });
    EnsurePlacementControl(*CurrentSettings());
    g_actualThemeChangedToken = g_widget.ActualThemeChanged(
        [](auto const&, auto const&) {
            try {
                if (!g_widget || g_unloading) {
                    return;
                }
                RefreshThemeBrushes(*CurrentSettings());
                UpdateWidgetText(true);
            } catch (...) {
                HRESULT error = winrt::to_hresult();
                Wh_Log(L"Taskbar theme update failed: %08X",
                       static_cast<unsigned>(error));
            }
        });
    g_reservedMargin = 0.0;
    g_lastAppliedRepeaterMarginLeft.reset();

    ApplyWidgetSettings();
    if (!StartMetricsWorker()) {
        Wh_Log(L"Metrics worker unavailable");
    }
    EnsureTimer();
    UpdateWidgetText();
    Wh_Log(L"Taskbar System Info injected");
    return true;
}

using RunFromWindowThreadProc = void (*)(void*);

struct WindowThreadCallbackContext {
    RunFromWindowThreadProc callback;
    void* context;
    std::atomic<bool> invoked{false};
};

std::mutex g_windowThreadCallbackRegistryMutex;
[[clang::no_destroy]]
std::optional<
    std::unordered_map<ULONG_PTR, WindowThreadCallbackContext*>>
    g_windowThreadCallbackRegistry{std::in_place};
std::atomic<ULONG_PTR> g_nextWindowThreadCallbackToken{1};

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc callback,
                         void* context) {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        callback(context);
        return true;
    }

    WindowThreadCallbackContext callbackContext{callback, context};
    ULONG_PTR callbackToken = 0;
    try {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        if (!g_windowThreadCallbackRegistry) {
            return false;
        }
        do {
            callbackToken = g_nextWindowThreadCallbackToken.fetch_add(
                1, std::memory_order_relaxed);
        } while (!callbackToken ||
                 g_windowThreadCallbackRegistry->contains(callbackToken));
        g_windowThreadCallbackRegistry->emplace(callbackToken,
                                                 &callbackContext);
    } catch (...) {
        Wh_Log(L"Preparing taskbar thread dispatch failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
        return false;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const auto* messageData =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (messageData->message == message) {
                    WindowThreadCallbackContext* callbackContext = nullptr;
                    {
                        std::lock_guard lock(
                            g_windowThreadCallbackRegistryMutex);
                        if (g_windowThreadCallbackRegistry) {
                            auto entry = g_windowThreadCallbackRegistry->find(
                                static_cast<ULONG_PTR>(messageData->lParam));
                            if (entry !=
                                g_windowThreadCallbackRegistry->end()) {
                                callbackContext = entry->second;
                                g_windowThreadCallbackRegistry->erase(entry);
                            }
                        }
                    }
                    if (callbackContext) {
                        callbackContext->callback(callbackContext->context);
                        callbackContext->invoked.store(true,
                                                       std::memory_order_release);
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        if (g_windowThreadCallbackRegistry) {
            g_windowThreadCallbackRegistry->erase(callbackToken);
        }
        Wh_Log(L"SetWindowsHookEx failed for taskbar thread %u: %u", threadId,
               GetLastError());
        return false;
    }

    SendMessageW(window, message, 0, static_cast<LPARAM>(callbackToken));
    bool unhooked = false;
    DWORD unhookError = ERROR_SUCCESS;
    for (int attempt = 0; attempt < 3; attempt++) {
        if (UnhookWindowsHookEx(hook)) {
            unhooked = true;
            break;
        }
        unhookError = GetLastError();
        if (unhookError == ERROR_INVALID_HOOK_HANDLE) {
            unhooked = true;
            break;
        }
        if (attempt < 2) {
            Sleep(10);
        }
    }
    {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        if (g_windowThreadCallbackRegistry) {
            g_windowThreadCallbackRegistry->erase(callbackToken);
        }
    }
    if (!unhooked) {
        Wh_Log(L"UnhookWindowsHookEx failed for taskbar thread %u: %u",
               threadId, unhookError);
    }
    bool invoked = callbackContext.invoked.load(std::memory_order_acquire);
    if (!invoked) {
        Wh_Log(L"Taskbar thread dispatch failed for thread %u", threadId);
    }
    return invoked && unhooked;
}

bool IsTaskbarWindowClass(HWND window, bool* secondary = nullptr) {
    WCHAR className[64];
    if (!window ||
        !GetClassNameW(window, className, std::size(className))) {
        return false;
    }

    bool isPrimary = _wcsicmp(className, L"Shell_TrayWnd") == 0;
    bool isSecondary =
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
    if (secondary) {
        *secondary = isSecondary;
    }
    return isPrimary || isSecondary;
}

bool IsCurrentProcessTaskbarWindow(HWND window, bool* secondary = nullptr) {
    DWORD processId = 0;
    return window && GetWindowThreadProcessId(window, &processId) != 0 &&
           processId == GetCurrentProcessId() &&
           IsTaskbarWindowClass(window, secondary);
}

struct DisplayMonitor {
    HMONITOR handle = nullptr;
    RECT bounds{};
    bool primary = false;
};

std::vector<DisplayMonitor> EnumerateDisplayMonitors() {
    std::vector<DisplayMonitor> monitors;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM context) -> BOOL {
            MONITORINFO info{};
            info.cbSize = sizeof(info);
            if (GetMonitorInfoW(monitor, &info)) {
                reinterpret_cast<std::vector<DisplayMonitor>*>(context)
                    ->push_back({monitor, info.rcMonitor,
                                 (info.dwFlags & MONITORINFOF_PRIMARY) != 0});
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&monitors));

    std::stable_sort(
        monitors.begin(), monitors.end(),
        [](const DisplayMonitor& left, const DisplayMonitor& right) {
            if (left.primary != right.primary) {
                return left.primary;
            }
            if (left.bounds.left != right.bounds.left) {
                return left.bounds.left < right.bounds.left;
            }
            if (left.bounds.top != right.bounds.top) {
                return left.bounds.top < right.bounds.top;
            }
            if (left.bounds.right != right.bounds.right) {
                return left.bounds.right < right.bounds.right;
            }
            if (left.bounds.bottom != right.bounds.bottom) {
                return left.bounds.bottom < right.bounds.bottom;
            }
            return reinterpret_cast<uintptr_t>(left.handle) <
                   reinterpret_cast<uintptr_t>(right.handle);
        });
    return monitors;
}

HWND FindTaskbarWindowForMonitor(HMONITOR monitor) {
    struct SearchContext {
        HMONITOR monitor;
        HWND result;
    } context{monitor, nullptr};

    EnumWindows(
        [](HWND window, LPARAM contextValue) -> BOOL {
            auto* context = reinterpret_cast<SearchContext*>(contextValue);
            if (IsCurrentProcessTaskbarWindow(window) &&
                MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) ==
                    context->monitor) {
                context->result = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.result;
}

HWND FindPrimaryTaskbarWindow() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND window, LPARAM context) -> BOOL {
            bool secondary = false;
            if (IsCurrentProcessTaskbarWindow(window, &secondary) &&
                !secondary) {
                *reinterpret_cast<HWND*>(context) = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

HWND FindOnlyTaskbarWindow() {
    struct SearchContext {
        HWND window = nullptr;
        uint32_t count = 0;
    } context;
    EnumWindows(
        [](HWND window, LPARAM contextValue) -> BOOL {
            auto* context = reinterpret_cast<SearchContext*>(contextValue);
            if (!IsCurrentProcessTaskbarWindow(window)) {
                return TRUE;
            }
            context->window = window;
            context->count++;
            return context->count < 2;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.count == 1 ? context.window : nullptr;
}

HWND FindConfiguredTaskbarWindow(
    const std::vector<DisplayMonitor>& monitors,
    bool logFallback = true) {
    if (auto dragged = DraggedTaskbarWindow()) return *dragged ? *dragged : FindPrimaryTaskbarWindow();
    int monitorNumber = CurrentSettings()->monitor;
    if (monitorNumber <= static_cast<int>(monitors.size())) {
        if (HWND window =
                FindTaskbarWindowForMonitor(monitors[monitorNumber - 1].handle)) {
            return window;
        }
        if (logFallback) {
            Wh_Log(L"Monitor %d has no taskbar; using the primary taskbar",
                   monitorNumber);
        }
    } else {
        if (logFallback) {
            Wh_Log(L"Monitor %d is unavailable; using the primary taskbar",
                   monitorNumber);
        }
    }
    return FindPrimaryTaskbarWindow();
}

HWND FindConfiguredTaskbarWindow(bool logFallback = true) {
    return FindConfiguredTaskbarWindow(EnumerateDisplayMonitors(), logFallback);
}

void RememberTaskbarWindow(HWND window) {
    if (!IsCurrentProcessTaskbarWindow(window)) {
        return;
    }
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (threadId) {
        g_taskbarWindow = window;
        g_taskbarThreadId = threadId;
        g_placementLocationUnknown = false;
    }
}

HWND FindRememberedTaskbarWindow() {
    HWND rememberedWindow = g_taskbarWindow.load();
    if (IsCurrentProcessTaskbarWindow(rememberedWindow)) {
        return rememberedWindow;
    }

    DWORD rememberedThreadId = g_taskbarThreadId.load();
    if (rememberedThreadId) {
        HWND threadWindow = nullptr;
        EnumThreadWindows(
            rememberedThreadId,
            [](HWND window, LPARAM context) -> BOOL {
                if (IsCurrentProcessTaskbarWindow(window)) {
                    *reinterpret_cast<HWND*>(context) = window;
                    return FALSE;
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&threadWindow));
        if (IsCurrentProcessTaskbarWindow(threadWindow)) {
            RememberTaskbarWindow(threadWindow);
            return threadWindow;
        }
    }
    return nullptr;
}

HWND FindAnyWindowOnTaskbarThread(HWND excludedWindow) {
    DWORD threadId = g_taskbarThreadId.load();
    if (!threadId) {
        return nullptr;
    }

    struct SearchContext {
        HWND excludedWindow;
        HWND result;
    } context{excludedWindow, nullptr};
    EnumThreadWindows(
        threadId,
        [](HWND window, LPARAM contextValue) -> BOOL {
            auto* context = reinterpret_cast<SearchContext*>(contextValue);
            DWORD processId = 0;
            if (window != context->excludedWindow &&
                GetWindowThreadProcessId(window, &processId) != 0 &&
                processId == GetCurrentProcessId()) {
                context->result = window;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&context));
    return context.result;
}

using CTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t =
    void*(WINAPI*)(void* pThis, void* taskbarHostSharedPtr);
CSecondaryTaskBand_GetTaskbarHost_t
    CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;

using RefCountBase_Decref_t = void(WINAPI*)(void* pThis);
RefCountBase_Decref_t RefCountBase_Decref_Original = nullptr;

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

XamlRoot GetTaskbarXamlRoot(HWND taskbarWindow) {
    bool isSecondary = false;
    if (!IsCurrentProcessTaskbarWindow(taskbarWindow) ||
        !IsTaskbarWindowClass(taskbarWindow, &isSecondary) ||
        !TaskbarHost_FrameHeight_Original || !RefCountBase_Decref_Original) {
        return nullptr;
    }

    auto getTaskbarHost = isSecondary
                              ? CSecondaryTaskBand_GetTaskbarHost_Original
                              : CTaskBand_GetTaskbarHost_Original;
    void* expectedVftable =
        isSecondary ? CSecondaryTaskBand_ITaskListWndSite_vftable
                    : CTaskBand_ITaskListWndSite_vftable;
    if (!getTaskbarHost || !expectedVftable) {
        Wh_Log(L"%s taskbar symbols unavailable",
               isSecondary ? L"Secondary" : L"Primary");
        return nullptr;
    }

    HWND taskBandWindow = isSecondary
                              ? FindWindowExW(taskbarWindow, nullptr, L"WorkerW",
                                              nullptr)
                              : reinterpret_cast<HWND>(GetPropW(
                                    taskbarWindow, L"TaskbandHWND"));
    if (!taskBandWindow) {
        Wh_Log(L"%s taskband host window not found",
               isSecondary ? L"Secondary" : L"Primary");
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(
        GetWindowLongPtrW(taskBandWindow, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForSite = taskBand;
    for (int i = 0;; i++) {
        if (*reinterpret_cast<void**>(taskBandForSite) == expectedVftable) {
            break;
        }
        if (i == 20) {
            Wh_Log(L"Taskband site vftable not found");
            return nullptr;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    getTaskbarHost(taskBandForSite, taskbarHostSharedPtr);
    struct TaskbarHostReferenceGuard {
        void* reference;
        ~TaskbarHostReferenceGuard() {
            if (reference) {
                RefCountBase_Decref_Original(reference);
            }
        }
    } referenceGuard{taskbarHostSharedPtr[1]};
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t elementOffset = 0;
#if defined(_M_X64)
    const BYTE* code =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        elementOffset = code[7];
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        return nullptr;
    }
#elif defined(_M_ARM64)
    const DWORD* code =
        reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (code[0] == 0xD503237F &&
        (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD &&
        (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (code[3] >> 12) & 0xFF;
    } else {
        Wh_Log(L"Unsupported TaskbarHost::FrameHeight pattern");
        return nullptr;
    }
#else
#error "Unsupported architecture"
#endif

    auto* elementAddress =
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset;
    auto* elementUnknown =
        *reinterpret_cast<::IUnknown**>(elementAddress);
    if (!elementUnknown) {
        return nullptr;
    }

    FrameworkElement taskbarElement = nullptr;
    elementUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                   winrt::put_abi(taskbarElement));
    return taskbarElement ? taskbarElement.XamlRoot() : nullptr;
}

FrameworkElement FindTaskbarFrame(HWND taskbarWindow) {
    XamlRoot xamlRoot = GetTaskbarXamlRoot(taskbarWindow);
    if (!xamlRoot) {
        return nullptr;
    }
    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    return FindChildRecursive(content, [](FrameworkElement child) {
        return winrt::get_class_name(child) == L"Taskbar.TaskbarFrame";
    });
}

struct ApplyWidgetContext {
    HWND taskbarWindow = nullptr;
    FrameworkElement taskbarFrame{nullptr};
    bool succeeded = false;
};

void ApplyToTaskbarWindow(void* contextValue) {
    auto* context = reinterpret_cast<ApplyWidgetContext*>(contextValue);
    HWND taskbarWindow = context->taskbarWindow;
    if (!IsCurrentProcessTaskbarWindow(taskbarWindow)) {
        return;
    }

    try {
        FrameworkElement taskbarFrame = context->taskbarFrame;
        if (!taskbarFrame) {
            taskbarFrame = FindTaskbarFrame(taskbarWindow);
            if (!taskbarFrame) {
                Wh_Log(L"TaskbarFrame not found");
                return;
            }
        }
        if (taskbarFrame && InjectWidget(taskbarFrame)) {
            RememberTaskbarWindow(taskbarWindow);
            context->succeeded = true;
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Applying widget failed: %08X",
               static_cast<unsigned>(error));
    }
}

bool ApplyWidgetToTaskbarWindow(
    HWND taskbarWindow,
    FrameworkElement taskbarFrame = nullptr) {
    ApplyWidgetContext context{taskbarWindow, taskbarFrame, false};
    return RunFromWindowThread(taskbarWindow, ApplyToTaskbarWindow,
                               &context) &&
           context.succeeded;
}

struct TaskbarProbeContext {
    HWND window = nullptr;
    bool ready = false;
};
void ProbeTaskbarWindow(void* value) {
    auto& context = *static_cast<TaskbarProbeContext*>(value);
    try {
        auto frame = FindTaskbarFrame(context.window);
        context.ready = frame && FindDirectChildByName(frame, L"RootGrid") != nullptr;
    } catch (...) {
        Wh_Log(L"Probing taskbar XAML failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
    }
}
bool IsReadyTaskbarFrame(HWND window) {
    TaskbarProbeContext context{window};
    return RunFromWindowThread(window, ProbeTaskbarWindow, &context) && context.ready;
}

struct RemoveWidgetForMoveContext {
    bool succeeded = false;
};

void RemoveWidgetForMove(void* contextValue) {
    auto* context =
        reinterpret_cast<RemoveWidgetForMoveContext*>(contextValue);
    try {
        auto cpuHistory = g_cpuHistory;
        auto gpuHistory = g_gpuHistory;
        auto renderedSequence = g_lastRenderedMetricsSequence;
        context->succeeded = RemoveWidget();
        g_cpuHistory = std::move(cpuHistory);
        g_gpuHistory = std::move(gpuHistory);
        g_lastRenderedMetricsSequence = renderedSequence; // Do not replay the published queue.
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing widget before monitor switch failed: %08X",
               static_cast<unsigned>(error));
    }
}

struct RemoveTaskbarUiContext {
    bool succeeded = false;
};

void RemoveFromCurrentTaskbar(void* contextValue) {
    auto* context = reinterpret_cast<RemoveTaskbarUiContext*>(contextValue);
    bool succeeded = true;
    try {
        succeeded = RemoveWidget() && succeeded;
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing widget failed: %08X",
               static_cast<unsigned>(error));
        succeeded = false;
    }
    succeeded = StopTimer() && succeeded;
    succeeded = RemoveTaskbarNotifications() && succeeded;
    try {
        g_loadedRevokers.reset();
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Removing taskbar Loaded handlers failed: %08X",
               static_cast<unsigned>(error));
        succeeded = false;
    }
    if (succeeded) {
        g_taskbarWindow = nullptr;
        g_taskbarThreadId = 0;
        g_lastFailedPlacementTarget = nullptr;
        g_hasFailedPlacementTarget = false;
        g_nextPlacementRetry = {};
        g_placementFailures = 0;
        g_placementIsFallback = false;
        g_placementLocationUnknown = false;
        g_placementApplyPending = false;
        g_taskbarUiResourcesRegistered = false;
    }
    if (context) {
        context->succeeded = succeeded;
    }
}

void ResetPlacementRetryState() {
    g_lastFailedPlacementTarget = nullptr;
    g_hasFailedPlacementTarget = false;
    g_nextPlacementRetry = {};
    g_placementFailures = 0;
}

uint32_t SchedulePlacementRetry(HWND targetWindow) {
    if (!g_hasFailedPlacementTarget ||
        targetWindow != g_lastFailedPlacementTarget) {
        g_lastFailedPlacementTarget = targetWindow;
        g_hasFailedPlacementTarget = true;
        g_placementFailures = 0;
    }

    if (g_placementFailures < 7) {
        g_placementFailures++;
    }
    uint32_t exponent = std::min(g_placementFailures - 1, 6u);
    uint32_t retrySeconds = std::min(1u << exponent, 60u);
    g_nextPlacementRetry =
        std::chrono::steady_clock::now() + std::chrono::seconds(retrySeconds);
    return retrySeconds;
}

void RecordPlacementFailure(HWND targetWindow) {
    uint32_t retrySeconds = SchedulePlacementRetry(targetWindow);
    Wh_Log(L"Taskbar placement failed; retrying in %u seconds", retrySeconds);
}

void RecordPlacementFallback(HWND targetWindow) {
    uint32_t retrySeconds = SchedulePlacementRetry(targetWindow);
    Wh_Log(L"Taskbar placement fell back; re-checking in %u seconds",
           retrySeconds);
}

void MarkPlacementForRetry(HWND requestedWindow) {
    g_placementIsFallback = true;
    RecordPlacementFailure(requestedWindow);
}

void CompletePlacement(HWND requestedWindow, HWND actualWindow) {
    g_placementApplyPending = false;
    g_placementLocationUnknown = false;
    if (requestedWindow == actualWindow) {
        g_placementIsFallback = false;
        ResetPlacementRetryState();
    } else {
        g_placementIsFallback = true;
        RecordPlacementFallback(requestedWindow);
    }
}

bool RefreshExistingWidget() {
    if (!g_widget) {
        return false;
    }

    ApplyWidgetSettings();
    if (!StartMetricsWorker()) {
        Wh_Log(L"Metrics worker unavailable");
    }
    EnsureTimer();
    UpdateWidgetText(true);
    return true;
}

bool ApplyLoadedFrameFallback(FrameworkElement fallbackFrame,
                              HWND logicalWindow) {
    if (!fallbackFrame) {
        return false;
    }

    Wh_Log(L"Taskbar XAML lookup unavailable; using the loaded frame directly");
    if (!InjectWidget(fallbackFrame)) {
        return false;
    }

    // On a multi-taskbar system the private XAML-root lookup is the only way to
    // map this frame to an HWND. Don't claim a monitor we couldn't identify;
    // keep the widget visible, mark its location unknown and retry with backoff.
    g_taskbarWindow = nullptr;
    g_taskbarThreadId = GetCurrentThreadId();
    g_placementLocationUnknown = true;
    Wh_Log(L"Monitor selection is unavailable for the direct-frame fallback");
    MarkPlacementForRetry(logicalWindow);
    return true;
}

struct ApplyOnTaskbarThreadContext {
    FrameworkElement fallbackFrame{nullptr};
    HWND requestedWindow = nullptr;
    bool refreshExistingWidget = true;
    bool succeeded = false;
};

void ApplyOnTaskbarUiThreadImpl(void* contextValue) {
    auto* context =
        reinterpret_cast<ApplyOnTaskbarThreadContext*>(contextValue);
    if (g_unloading) {
        return;
    }
    // Keep a retry timer even before the first successful injection. All state
    // below is intentionally owned by Explorer's taskbar/XAML UI thread.
    EnsureTimer();

    HWND requestedWindow = context->requestedWindow;
    if (!IsCurrentProcessTaskbarWindow(requestedWindow)) {
        requestedWindow = FindConfiguredTaskbarWindow();
    }
    if (!requestedWindow) {
        Wh_Log(L"Taskbar window not found");
        MarkPlacementForRetry(nullptr);
        return;
    }

    HWND currentWindow = g_taskbarWindow.load();
    if (g_placementLocationUnknown ||
        !IsCurrentProcessTaskbarWindow(currentWindow)) {
        currentWindow = nullptr;
    }
    if (currentWindow == requestedWindow && g_widget) {
        if (context->refreshExistingWidget) {
            RefreshExistingWidget();
        }
        CompletePlacement(requestedWindow, currentWindow);
        context->succeeded = true;
        return;
    }

    HWND targetWindow = requestedWindow;
    bool targetReady = IsReadyTaskbarFrame(targetWindow);
    if (!targetReady) {
        HWND primaryWindow = FindPrimaryTaskbarWindow();
        if (primaryWindow && primaryWindow != targetWindow) {
            bool primaryReady = IsReadyTaskbarFrame(primaryWindow);
            if (primaryReady) {
                Wh_Log(
                    L"Selected taskbar is not ready; using the primary taskbar");
                targetWindow = primaryWindow;
                targetReady = true;
            }
        }
    }

    if (!targetReady) {
        Wh_Log(L"No taskbar exposes a ready XAML root");
        if (ApplyLoadedFrameFallback(context->fallbackFrame, targetWindow)) {
            context->succeeded = true;
            return;
        }
        if (g_widget) {
            Wh_Log(L"Keeping the existing widget until taskbar placement recovers");
        }
        MarkPlacementForRetry(requestedWindow);
        return;
    }

    if (currentWindow == targetWindow && g_widget) {
        if (context->refreshExistingWidget) {
            RefreshExistingWidget();
        }
        CompletePlacement(requestedWindow, targetWindow);
        context->succeeded = true;
        return;
    }

    HWND removalWindow = currentWindow;
    if (!removalWindow && g_widget && g_taskbarThreadId.load()) {
        removalWindow = FindAnyWindowOnTaskbarThread(nullptr);
    }
    bool widgetRemovedForMove = false;
    if (removalWindow && g_widget && currentWindow != targetWindow) {
        RemoveWidgetForMoveContext removeContext;
        if (!RunFromWindowThread(removalWindow, RemoveWidgetForMove,
                                 &removeContext) ||
            !removeContext.succeeded) {
            Wh_Log(L"Removing widget from the previous monitor failed");
            MarkPlacementForRetry(requestedWindow);
            return;
        }
        widgetRemovedForMove = true;
    }

    if (ApplyWidgetToTaskbarWindow(targetWindow)) {
        CompletePlacement(requestedWindow, targetWindow);
        context->succeeded = true;
        return;
    }

    Wh_Log(L"Applying widget on taskbar thread failed");
    if (ApplyLoadedFrameFallback(context->fallbackFrame, requestedWindow)) {
        context->succeeded = true;
        return;
    }

    if (widgetRemovedForMove &&
        IsCurrentProcessTaskbarWindow(currentWindow)) {
        Wh_Log(L"Restoring widget on the previous taskbar");
        if (!ApplyWidgetToTaskbarWindow(currentWindow)) {
            Wh_Log(L"Restoring widget on the previous taskbar failed");
        }
    }
    MarkPlacementForRetry(requestedWindow);
}

void ApplyOnTaskbarUiThread(void* contextValue) {
    auto* context =
        reinterpret_cast<ApplyOnTaskbarThreadContext*>(contextValue);
    try {
        ApplyOnTaskbarUiThreadImpl(contextValue);
        if (context->succeeded) {
            g_placementApplyPending = false;
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Taskbar placement update failed: %08X",
               static_cast<unsigned>(error));
        MarkPlacementForRetry(nullptr);
    }
}

bool ApplyOnTaskbarThread(FrameworkElement fallbackFrame = nullptr,
                          bool refreshExistingWidget = true,
                          HWND requestedWindow = nullptr) {
    if (g_unloading) {
        return false;
    }

    g_placementApplyPending = true;
    constexpr int kMaximumDispatchAttempts = 3;
    for (int attempt = 0; attempt < kMaximumDispatchAttempts; attempt++) {
        DWORD attemptedThreadIds[3]{};
        size_t attemptedThreadCount = 0;
        HWND rememberedWindow = g_taskbarWindow.load();
        if (!IsCurrentProcessTaskbarWindow(rememberedWindow)) {
            rememberedWindow = nullptr;
        }
        HWND primaryWindow = FindPrimaryTaskbarWindow();
        HWND candidates[] = {
            primaryWindow, rememberedWindow,
            FindAnyWindowOnTaskbarThread(primaryWindow ? primaryWindow
                                                       : rememberedWindow)};
        for (size_t i = 0; i < std::size(candidates); i++) {
            HWND dispatchWindow = candidates[i];
            if (!dispatchWindow ||
                std::find(candidates, candidates + i, dispatchWindow) !=
                    candidates + i) {
                continue;
            }
            DWORD dispatchThreadId =
                GetWindowThreadProcessId(dispatchWindow, nullptr);
            if (!dispatchThreadId ||
                std::find(attemptedThreadIds,
                          attemptedThreadIds + attemptedThreadCount,
                          dispatchThreadId) !=
                    attemptedThreadIds + attemptedThreadCount) {
                continue;
            }
            attemptedThreadIds[attemptedThreadCount++] = dispatchThreadId;

            ApplyOnTaskbarThreadContext context{
                dispatchThreadId == GetCurrentThreadId() ? fallbackFrame : nullptr,
                requestedWindow, refreshExistingWidget, false};
            if (RunFromWindowThread(dispatchWindow, ApplyOnTaskbarUiThread,
                                    &context)) {
                return context.succeeded;
            }
        }
        if (attempt + 1 < kMaximumDispatchAttempts) {
            Sleep(25);
        }
    }
    Wh_Log(L"Taskbar UI thread is unavailable; placement remains pending");
    return false;
}

// The editor and its native windows are owned by one UI thread. Its geometry
// probes return plain data, never FrameworkElement references from another UI.
struct MoveHotkey { UINT modifiers = 0; UINT key = 0; };
std::optional<MoveHotkey> ParseMoveHotkey(std::wstring text, std::wstring* reason = nullptr) {
    MoveHotkey result;
    if (text.empty()) return result;
    auto invalid = [&](const wchar_t* message) -> std::optional<MoveHotkey> {
        if (reason) *reason = message;
        return std::nullopt;
    };
    size_t begin = 0;
    while (begin <= text.size()) {
        size_t end = text.find(L'+', begin);
        auto part = text.substr(begin, end == std::wstring::npos ? end : end - begin);
        size_t first = part.find_first_not_of(L" \t"), last = part.find_last_not_of(L" \t");
        if (first == std::wstring::npos) return invalid(L"Empty component in the combination");
        part = ToLower(part.substr(first, last - first + 1));
        UINT modifier = part == L"ctrl" ? MOD_CONTROL : part == L"alt" ? MOD_ALT :
                        part == L"shift" ? MOD_SHIFT : part == L"win" ? MOD_WIN : 0;
        if (modifier) {
            if (result.modifiers & modifier) return invalid(L"Duplicate modifier");
            result.modifiers |= modifier;
        } else {
            if (result.key) return invalid(L"Only one letter, digit or function key is allowed");
            if (part.size() == 1 && ((part[0] >= L'a' && part[0] <= L'z') ||
                                    (part[0] >= L'0' && part[0] <= L'9')))
                result.key = static_cast<UINT>(std::towupper(part[0]));
            else if (part.size() >= 2 && part[0] == L'f') {
                if (part.size() > 3 || part[1] < L'1' || part[1] > L'9' ||
                    (part.size() == 3 && (part[2] < L'0' || part[2] > L'9')))
                    return invalid(L"Function key must be F1-F24");
                int number = part[1] - L'0';
                if (part.size() == 3) number = number * 10 + part[2] - L'0';
                if (number > 24) return invalid(L"Function key must be F1-F24");
                result.key = VK_F1 + number - 1;
            } else return invalid(L"Unsupported key or modifier; use Ctrl/Alt/Shift/Win and A-Z, 0-9 or F1-F24");
        }
        if (end == std::wstring::npos) break;
        begin = end + 1;
    }
    return result.key ? std::optional<MoveHotkey>{result} : invalid(L"The combination has no key");
}
struct PlacementProfiles {
    int monitor = 1;
    int offset = 10;
    std::wstring target;
    std::unordered_map<std::wstring, double> positions;
};
PlacementProfiles g_profiles;
std::mutex g_profilesMutex;
std::mutex g_monitorKeysMutex;
std::unordered_map<std::wstring, std::wstring> g_monitorKeys;
void InvalidateMonitorKeys() { std::lock_guard lock(g_monitorKeysMutex); g_monitorKeys.clear(); }
std::wstring SerializeProfiles(const PlacementProfiles& profiles) {
    std::wstring result = L"1\n" + std::to_wstring(profiles.monitor) + L" " +
                          std::to_wstring(profiles.offset) + L"\n" + profiles.target + L"\n";
    // Sorted output makes saves and corruption diagnostics reproducible.
    std::vector<std::pair<std::wstring, double>> ordered(profiles.positions.begin(), profiles.positions.end());
    std::sort(ordered.begin(), ordered.end());
    for (const auto& [key, fraction] : ordered) {
        char value[64];
        auto converted = std::to_chars(value, value + sizeof(value), fraction);
        if (converted.ec != std::errc{}) continue;
        result += key + L"\t" + std::wstring(value, converted.ptr) + L"\n";
    }
    return result;
}
std::optional<PlacementProfiles> ParseProfiles(const std::wstring& text) {
    if (text.size() > 32767 || !text.starts_with(L"1\n")) return std::nullopt;
    PlacementProfiles result;
    size_t second = text.find(L'\n', 2), third = text.find(L'\n', second + 1);
    if (second == std::wstring::npos || third == std::wstring::npos) return std::nullopt;
    auto wideBase = text.substr(2, second - 2);
    if (!std::all_of(wideBase.begin(), wideBase.end(), [](wchar_t c) { return c < 128; })) return std::nullopt;
    std::string base(wideBase.begin(), wideBase.end());
    auto separator = base.find(' ');
    if (separator == std::string::npos) return std::nullopt;
    auto monitor = std::from_chars(base.data(), base.data() + separator, result.monitor);
    auto offset = std::from_chars(base.data() + separator + 1, base.data() + base.size(), result.offset);
    if (monitor.ec != std::errc{} || monitor.ptr != base.data() + separator ||
        offset.ec != std::errc{} || offset.ptr != base.data() + base.size() ||
        result.monitor < 1 || result.monitor > 32 || result.offset < 0) return std::nullopt;
    result.target = text.substr(second + 1, third - second - 1);
    if (result.target.size() > 1024 || result.target.find(L'\t') != std::wstring::npos) return std::nullopt;
    for (size_t start = third + 1; start < text.size();) {
        size_t stop = text.find(L'\n', start), tab = text.find(L'\t', start);
        if (stop == std::wstring::npos || tab == std::wstring::npos || tab >= stop || tab == start)
            return std::nullopt;
        auto key = text.substr(start, tab - start);
        auto wide = text.substr(tab + 1, stop - tab - 1);
        if (key.size() > 1024 || wide.empty() || wide.size() > 64) return std::nullopt;
        if (!std::all_of(wide.begin(), wide.end(), [](wchar_t c) { return c < 128; })) return std::nullopt;
        std::string narrow(wide.begin(), wide.end()); double value = 0;
        auto parsed = std::from_chars(narrow.data(), narrow.data() + narrow.size(), value);
        if (parsed.ec != std::errc{} || parsed.ptr != narrow.data() + narrow.size() ||
            !std::isfinite(value) || value < 0 || value > 1 || result.positions.contains(key)) return std::nullopt;
        result.positions[key] = value;
        if (result.positions.size() > 32) return std::nullopt;
        start = stop + 1;
    }
    // A selected display can use Left offset after its dragged position is
    // cleared. Keep that target and the saved positions on other displays.
    return result;
}
PlacementProfiles PlacementProfilesSnapshot() {
    std::lock_guard lock(g_profilesMutex); return g_profiles;
}
void SetPlacementProfiles(PlacementProfiles profiles) {
    std::lock_guard lock(g_profilesMutex); g_profiles = std::move(profiles);
}
bool SavePlacementProfiles(const PlacementProfiles& profiles) {
    auto text = SerializeProfiles(profiles);
    if (text.size() > 32767) { Wh_Log(L"Saved widget positions exceed the local storage size limit"); return false; }
    if (!Wh_SetStringValue(L"placement.v1", text.c_str())) {
        Wh_Log(L"Saving widget position to local storage failed");
        return false;
    }
    return true;
}
void LoadPlacementProfiles() {
    auto settings = CurrentSettings();
    PlacementProfiles profiles; profiles.monitor = settings->monitor; profiles.offset = settings->leftOffset;
    std::vector<wchar_t> buffer(32768);
    size_t length = Wh_GetStringValue(L"placement.v1", buffer.data(), buffer.size());
    if (length > 0 && length < buffer.size()) {
        if (auto parsed = ParseProfiles(buffer.data())) profiles = std::move(*parsed);
        else Wh_Log(L"Ignoring invalid saved widget positions");
    }
    SetPlacementProfiles(std::move(profiles));
}
std::wstring MonitorKey(HWND window) {
    if (!window) return {};
    MONITORINFOEXW info{}; info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONULL), &info)) return {};
    {
        std::lock_guard lock(g_monitorKeysMutex);
        if (auto it = g_monitorKeys.find(info.szDevice); it != g_monitorKeys.end()) return it->second;
    }
    std::wstring key;
    // Device paths survive display renumbering; an HMONITOR or DISPLAY1 does not.
    for (int attempt = 0; attempt < 3; ++attempt) {
        UINT32 pathsCount = 0, modesCount = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathsCount, &modesCount) != ERROR_SUCCESS) break;
        std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathsCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modesCount);
        LONG status = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathsCount, paths.data(), &modesCount, modes.data(), nullptr);
        if (status == ERROR_INSUFFICIENT_BUFFER) continue;
        if (status != ERROR_SUCCESS) break;
        for (UINT32 i = 0; i < pathsCount; ++i) {
            DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
            source.header = {DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME, sizeof(source), paths[i].sourceInfo.adapterId, paths[i].sourceInfo.id};
            if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS ||
                _wcsicmp(source.viewGdiDeviceName, info.szDevice) != 0) continue;
            DISPLAYCONFIG_TARGET_DEVICE_NAME target{};
            target.header = {DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME, sizeof(target), paths[i].targetInfo.adapterId, paths[i].targetInfo.id};
            if (DisplayConfigGetDeviceInfo(&target.header) == ERROR_SUCCESS) {
                auto candidate = ToLower(target.monitorDevicePath);
                if (!candidate.empty() && (key.empty() || candidate < key)) key = std::move(candidate);
            }
        }
        break;
    }
    if (!key.empty()) { std::lock_guard lock(g_monitorKeysMutex); g_monitorKeys[info.szDevice] = key; }
    return key;
}
std::optional<HWND> DraggedTaskbarWindow() {
    auto profiles = PlacementProfilesSnapshot();
    if (profiles.target.empty()) return std::nullopt;
    for (const auto& monitor : EnumerateDisplayMonitors())
        if (HWND window = FindTaskbarWindowForMonitor(monitor.handle))
            if (MonitorKey(window) == profiles.target) return window;
    return std::optional<HWND>{nullptr};
}
std::optional<double> SavedTaskbarFraction(HWND window) {
    auto profiles = PlacementProfilesSnapshot();
    auto key = MonitorKey(window);
    if (auto it = profiles.positions.find(key); !key.empty() && it != profiles.positions.end()) return it->second;
    return std::nullopt;
}
double PreferredTaskbarLeft(HWND window, double width, const ModSettings& settings) {
    if (auto fraction = SavedTaskbarFraction(window))
        return *fraction * std::max(0.0, width - settings.width);
    return settings.leftOffset;
}
bool ReconcileProfiles(PlacementProfiles& profiles, const ModSettings& settings,
                       const std::wstring& currentKey) {
    bool changed = false;
    if (profiles.monitor != settings.monitor) {
        profiles.target.clear(); profiles.monitor = settings.monitor; changed = true;
    }
    if (profiles.offset != settings.leftOffset) {
        if (!currentKey.empty()) profiles.positions.erase(currentKey);
        profiles.offset = settings.leftOffset; changed = true;
    }
    return changed;
}
void ReconcilePlacementSettings() {
    auto settings = CurrentSettings(); auto profiles = PlacementProfilesSnapshot();
    std::wstring currentKey;
    if (profiles.offset != settings->leftOffset) {
        auto prospective = profiles;
        if (profiles.monitor != settings->monitor) prospective.target.clear();
        SetPlacementProfiles(std::move(prospective));
        currentKey = MonitorKey(FindConfiguredTaskbarWindow());
    }
    bool changed = ReconcileProfiles(profiles, *settings, currentKey);
    SetPlacementProfiles(profiles);
    if (changed) SavePlacementProfiles(profiles);
}

std::optional<PlacementProfiles> ConfirmPlacementPreference(
    PlacementProfiles next, const ModSettings& settings, const std::wstring& key,
    double left, double panelWidth, bool reset, double widgetWidth = 0) {
    if (reset) { next.target.clear(); next.positions.clear(); }
    else {
        if (key.empty() || !std::isfinite(left) || !std::isfinite(panelWidth) ||
            (!next.positions.contains(key) && next.positions.size() >= 32)) return std::nullopt;
        next.target = key;
        if (widgetWidth != 0 && (!std::isfinite(widgetWidth) || widgetWidth <= 0 || widgetWidth > panelWidth))
            return std::nullopt;
        double span = std::max(0.0, panelWidth - (widgetWidth > 0 ? widgetWidth : settings.width));
        next.positions[key] = span > 0 ? std::clamp(left / span, 0.0, 1.0) : 0;
    }
    next.monitor = settings.monitor; next.offset = settings.leftOffset;
    return next;
}

struct TaskbarProjection {
    HWND window = nullptr;
    TaskbarGeometry geometry;
    POINT origin{};
    RECT panel{};
    double scale = 1.0;
    std::wstring key;
};
struct ScopedPhysicalDpi {
    DPI_AWARENESS_CONTEXT previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    ~ScopedPhysicalDpi() { if (previous) SetThreadDpiAwarenessContext(previous); }
};
double ScreenPointToTaskbarX(const TaskbarProjection& projection, POINT point) {
    return (point.x - projection.origin.x) / projection.scale;
}
void ProbeTaskbarGeometry(void* value) {
    auto& probe = *static_cast<TaskbarProjection*>(value);
    try {
        ScopedPhysicalDpi physicalCoordinates;
        auto frame = FindTaskbarFrame(probe.window);
        if (!frame) return;
        auto root = FindDirectChildByName(frame, L"RootGrid");
        auto repeater = FindDirectChildByName(root, L"TaskbarFrameRepeater");
        bool current = probe.window == g_taskbarWindow.load();
        probe.geometry = MeasureTaskbarGeometry(root, repeater, current ? g_reservedMargin : 0,
                                               current ? &g_geometryCache : nullptr);
        auto xaml = root.XamlRoot();
        if (!xaml || !probe.geometry.ready) return;
        probe.scale = xaml.RasterizationScale();
        if (!std::isfinite(probe.scale) || probe.scale <= 0) { probe.geometry.ready = false; return; }
        probe.origin = {0, 0}; ClientToScreen(probe.window, &probe.origin);
        if (auto content = xaml.Content().try_as<UIElement>()) {
            auto point = root.TransformToVisual(content).TransformPoint({0, 0});
            probe.origin.x += static_cast<LONG>(std::lround(point.X * probe.scale));
            probe.origin.y += static_cast<LONG>(std::lround(point.Y * probe.scale));
        }
        GetWindowRect(probe.window, &probe.panel);
    } catch (...) { probe.geometry.ready = false; }
}
TaskbarProjection ProjectTaskbar(HWND window) {
    TaskbarProjection result; result.window = window;
    if (IsCurrentProcessTaskbarWindow(window) && RunFromWindowThread(window, ProbeTaskbarGeometry, &result))
        result.key = MonitorKey(window);
    return result;
}
struct PreviewText {
    std::wstring text, font;
    Rect bounds{};
    Color color{};
    double fontSize = 11;
    int weight = 400;
    bool right = false;
};
struct PreviewBar { Rect bounds{}; Color color{}; double fraction = 1; };
struct PreviewGraph { Rect bounds{}; Color color{}; SparklineRuns runs; };
struct WidgetPreviewFrame {
    uint64_t sequence = 0;
    bool ready = false, light = false, highContrast = false;
    double width = 410, height = kWidgetHeight;
    std::wstring configuredFont;
    int configuredFontSize = 0, configuredWidth = 0;
    WidgetFontMetrics fontMetrics;
    std::array<std::deque<HistorySample>, 2> histories;
    std::vector<PreviewText> texts;
    std::vector<PreviewBar> bars;
    std::vector<PreviewGraph> graphs;
};
Color PreviewBrushColor(Brush brush, double opacity, Color fallback) {
    if (auto solid = brush.try_as<SolidColorBrush>()) {
        fallback = solid.Color(); opacity *= solid.Opacity();
    }
    fallback.A = static_cast<uint8_t>(std::lround(fallback.A * std::clamp(opacity, 0.0, 1.0)));
    return fallback;
}
struct CaptureWidgetPreviewContext { WidgetPreviewFrame frame; bool changed = false; };
void CaptureWidgetPreview(void* value) {
    auto& context = *static_cast<CaptureWidgetPreviewContext*>(value);
    if (!g_widget) return;
    bool light = g_cachedWidgetTheme == ElementTheme::Light;
    auto settings = CurrentSettings();
    if (context.frame.ready && context.frame.sequence == g_widgetVisualRevision &&
        context.frame.light == light && context.frame.highContrast == g_cachedHighContrast &&
        context.frame.configuredFont == settings->fontFamily && context.frame.configuredFontSize == settings->fontSize &&
        context.frame.configuredWidth == settings->width) return;
    WidgetPreviewFrame frame;
    frame.ready = true; frame.sequence = g_widgetVisualRevision;
    frame.light = light; frame.highContrast = g_cachedHighContrast;
    frame.configuredFont = settings->fontFamily; frame.configuredFontSize = settings->fontSize;
    frame.configuredWidth = settings->width;
    frame.fontMetrics = MeasureWidgetFont(*settings);
    frame.histories = {g_cpuHistory, g_gpuHistory};
    Color fallback = light ? Color{255, 25, 25, 25} : Color{255, 238, 238, 238};
    if (frame.highContrast) fallback = ColorFromColorRef(GetSysColor(COLOR_WINDOWTEXT));
    // Snapshot every logical field, even when the source layout hides it.
    // Destination geometry, not source coordinates, determines its bounds.
    for (TextBlock text : {g_cpuLabel, g_cpuUsageText, g_cpuTempText,
        g_ramLabel, g_ramPercentText, g_ramCapacityText, g_gpuLabel, g_gpuUsageText,
        g_gpuTempText, g_vramLabel, g_vramPercentText, g_vramCapacityText}) {
        auto label = text.Text();
        frame.texts.push_back({std::wstring(label.begin(), label.end()), settings->fontFamily, {},
            PreviewBrushColor(text.Foreground(), text.Opacity(), fallback), static_cast<double>(settings->fontSize),
            text.FontWeight().Weight, text.TextAlignment() == TextAlignment::Right});
    }
    for (auto graph : {g_cpuGraph, g_gpuGraph})
        frame.graphs.push_back({{}, PreviewBrushColor(graph.Stroke(), graph.Opacity(), Color{255, 120, 168, 255}), {}});
    for (auto bar : {g_ramTrack, g_ramFill, g_vramTrack, g_vramFill})
        frame.bars.push_back({{}, PreviewBrushColor(bar.Fill(), bar.Opacity(), Color{255, 120, 168, 255}),
            g_memoryBarWidth > 0 ? std::clamp(bar.Width() / g_memoryBarWidth, 0.0, 1.0) : 0});
    context.frame = std::move(frame); context.changed = true;
}
void ArrangeWidgetPreview(WidgetPreviewFrame& frame, const WidgetLayout& layout, const ModSettings& settings) {
    frame.width = layout.width; frame.height = layout.height;
    for (size_t i = 0; i < frame.texts.size(); ++i) frame.texts[i].bounds = layout.cells[i];
    auto now = SampleTime::clock::now();
    for (size_t i = 0; i < frame.graphs.size(); ++i) {
        auto& graph = frame.graphs[i]; graph.bounds = layout.graphs[i];
        graph.runs = layout.showGraphs ? BuildSparklineRuns(frame.histories[i], settings.historySeconds,
            settings.updateInterval, graph.bounds.Width, graph.bounds.Height, now) : SparklineRuns{};
    }
    for (size_t i = 0; i < frame.bars.size(); ++i) {
        auto& bar = frame.bars[i]; bar.bounds = layout.bars[i / 2];
        bar.bounds.Width *= static_cast<float>(bar.fraction);
    }
}
bool OnPreviewSourceThread(HWND source, RunFromWindowThreadProc callback, void* context) {
    if (GetCurrentThreadId() == g_taskbarThreadId.load()) { callback(context); return true; }
    return source && RunFromWindowThread(source, callback, context);
}
struct SourceWidgetOpacityContext { HWND source; bool hide; double original = 1; bool changed = false; };
void SetSourceWidgetOpacity(void* value) {
    auto& context = *static_cast<SourceWidgetOpacityContext*>(value);
    if (!g_widgetHost || g_taskbarWindow.load() != context.source) return;
    if (context.hide) {
        context.original = g_widgetHost.Opacity(); g_widgetHost.Opacity(0); context.changed = true;
    } else if (g_widgetHost.Opacity() == 0) g_widgetHost.Opacity(context.original);
}
struct PreviewSurface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    uint32_t* pixels = nullptr;
    int width = 0, height = 0;
    ~PreviewSurface() {
        if (dc && previous) SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
    }
    bool Resize(int w, int h) {
        if (w == width && h == height && bitmap) return true;
        if (!dc) dc = CreateCompatibleDC(nullptr);
        if (!dc) return false;
        if (previous) { SelectObject(dc, previous); previous = nullptr; }
        if (bitmap) { DeleteObject(bitmap); bitmap = nullptr; }
        BITMAPINFO info{}; info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = w; info.bmiHeader.biHeight = -h;
        info.bmiHeader.biPlanes = 1; info.bmiHeader.biBitCount = 32; info.bmiHeader.biCompression = BI_RGB;
        void* data = nullptr;
        bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &data, nullptr, 0);
        if (!bitmap) return false;
        pixels = static_cast<uint32_t*>(data); previous = SelectObject(dc, bitmap);
        width = w; height = h; return true;
    }
};
ULONG_PTR g_previewGraphicsToken = 0;
bool EnsurePreviewGraphics() {
    if (g_previewGraphicsToken) return true;
    Gdiplus::GdiplusStartupInput input;
    return Gdiplus::GdiplusStartup(&g_previewGraphicsToken, &input, nullptr) == Gdiplus::Ok;
}
void StopPreviewGraphics() {
    if (g_previewGraphicsToken) { Gdiplus::GdiplusShutdown(g_previewGraphicsToken); g_previewGraphicsToken = 0; }
}
struct MoveEditorState {
    HWND window = nullptr;
    HWND previousForeground = nullptr;
    bool dragging = false;
    bool reset = false;
    bool pointerOutside = false;
    bool hovered = false, sourceHidden = false, dirty = true, closing = false;
    HWND source = nullptr;
    double sourceOpacity = 1;
    WidgetPreviewFrame visual;
    std::shared_ptr<PreviewSurface> surface;
    RECT renderedBody{}, renderedContent{};
    double renderedDpi = 0;
    std::optional<WidgetLayout> renderedLayout;
    double grabFraction = 0.5;
    double preferredLeft = 0;
    TaskbarProjection target;
    TaskbarPlacement candidate;
    std::chrono::steady_clock::time_point lastProbe{};
};
// WM_DESTROY releases GDI resources explicitly on the editor thread. Avoid a
// implicit GDI teardown under the loader lock when Explorer exits with a preview open.
[[clang::no_destroy]] std::shared_ptr<MoveEditorState> g_moveEditor;
std::atomic<HWND> g_moveEditorWindow{nullptr};
std::atomic<bool> g_moveCommitting{false};
std::atomic<uint64_t> g_moveEpoch{0};
constexpr wchar_t kMoveWindowClass[] = L"WindhawkTaskbarSystemInfoMove_" WH_MOD_ID;
constexpr wchar_t kPlacementWindowClass[] = L"WindhawkTaskbarSystemInfoPlacement_" WH_MOD_ID;
constexpr UINT kGeometryMessage = WM_APP + 190;
constexpr int kMoveHotkeyId = 190;
HMODULE PlacementModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<PCWSTR>(&PlacementModule), &module);
    return module;
}
struct MovePreviewLayout { RECT bounds{}, body{}, content{}; double contentScale = 1; WidgetLayout widget; };
MovePreviewLayout ResolveMovePreviewLayout(const MoveEditorState& editor, RECT display) {
    const auto& target = editor.target;
    double width = editor.candidate.width > 0 ? editor.candidate.width : CurrentSettings()->width;
    double left = editor.candidate.width > 0 ? editor.candidate.left : editor.preferredLeft;
    if (!editor.candidate.width) left = std::clamp(left, 0.0, std::max(0.0, target.geometry.width - width));
    LONG x = target.origin.x + static_cast<LONG>(std::lround(left * target.scale));
    LONG w = static_cast<LONG>(std::lround(width * target.scale));
    auto settings = CurrentSettings();
    double heightLimit = target.geometry.height > 0 ? target.geometry.height : kWidgetHeight;
    // Candidate placement already accounts for controls and reservation. Resolve
    // its exact available span through the shared layout policy.
    TaskbarGeometry span{width + 2 * kTaskbarClearance, heightLimit, {}, true};
    auto resolved = ResolveWidgetPlacement(*settings, span, {kTaskbarClearance, kTaskbarClearance},
                                           editor.visual.fontMetrics, false);
    auto shape = resolved.placement.width > 0 ? resolved.layout :
        BuildWidgetLayout(*settings, editor.visual.fontMetrics, WidgetLayoutMode::Full, kWidgetHeight, true);
    double sourceWidth = shape.width;
    double downScale = std::min({1.0, width / sourceWidth, heightLimit / shape.height});
    LONG h = std::max<LONG>(1, static_cast<LONG>(std::lround(shape.height * downScale * target.scale)));
    LONG y = target.origin.y + std::max<LONG>(0, static_cast<LONG>(std::lround(target.geometry.height * target.scale)) - h) / 2;
    LONG padding = std::max<LONG>(1, static_cast<LONG>(std::lround(3 * target.scale)));
    LONG bodyLeft = std::max(display.left, x), bodyRight = std::min(display.right, x + w);
    LONG bodyTop = std::max(display.top, y - padding), bodyBottom = std::min(display.bottom, y + h + padding);
    LONG contentWidth = static_cast<LONG>(std::lround(sourceWidth * downScale * target.scale));
    MovePreviewLayout layout;
    layout.bounds = {bodyLeft, bodyTop, bodyRight, bodyBottom};
    layout.body = {0, 0, bodyRight - bodyLeft, bodyBottom - bodyTop};
    layout.content = {x - bodyLeft + (w - contentWidth) / 2, y - bodyTop,
                      x - bodyLeft + (w + contentWidth) / 2, y + h - bodyTop};
    layout.contentScale = downScale * target.scale;
    shape.scale = downScale; layout.widget = shape;
    return layout;
}
MovePreviewLayout CurrentMovePreviewLayout(const MoveEditorState& editor) {
    MONITORINFO info{}; info.cbSize = sizeof(info);
    GetMonitorInfoW(MonitorFromPoint(editor.target.origin, MONITOR_DEFAULTTONEAREST), &info);
    return ResolveMovePreviewLayout(editor, info.rcMonitor);
}
Gdiplus::Color NativePreviewColor(Color color) { return {color.A, color.R, color.G, color.B}; }
std::unique_ptr<Gdiplus::Font> MovePreviewFont(const PreviewText& text) {
    return CreateWidgetPreviewFont(text.font, text.fontSize, text.weight);
}
void RoundedPreviewPath(Gdiplus::GraphicsPath& path, RECT rect, float radius) {
    float x = rect.left + .5f, y = rect.top + .5f;
    float w = rect.right - rect.left - 1.f, h = rect.bottom - rect.top - 1.f;
    float d = std::min({radius * 2, w, h});
    path.AddArc(x, y, d, d, 180, 90); path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90); path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}
bool PaintMovePreviewSurface(MoveEditorState& editor, const MovePreviewLayout& layout) {
    if (!EnsurePreviewGraphics()) return false;
    if (!editor.surface) editor.surface = std::make_shared<PreviewSurface>();
    auto& surface = *editor.surface;
    int width = layout.bounds.right - layout.bounds.left, height = layout.bounds.bottom - layout.bounds.top;
    if (surface.width != width || surface.height != height) editor.dirty = true;
    if (!EqualRect(&editor.renderedBody, &layout.body) || !EqualRect(&editor.renderedContent, &layout.content) ||
        editor.renderedDpi != editor.target.scale) editor.dirty = true;
    if (!editor.renderedLayout || !SameWidgetLayout(*editor.renderedLayout, layout.widget)) editor.dirty = true;
    if (!surface.Resize(width, height)) return false;
    if (!editor.dirty) return true;
    ArrangeWidgetPreview(editor.visual, layout.widget, *CurrentSettings());
    Gdiplus::Bitmap bitmap(width, height, width * 4, PixelFormat32bppPARGB, reinterpret_cast<BYTE*>(surface.pixels));
    Gdiplus::Graphics graphics(&bitmap);
    graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    bool light = editor.visual.light, contrast = editor.visual.highContrast;
    bool valid = editor.candidate.width > 0;
    float dpi = static_cast<float>(editor.target.scale);
    Gdiplus::GraphicsPath body; RoundedPreviewPath(body, layout.body, 7 * dpi);
    if (editor.dragging && contrast) {
        Gdiplus::SolidBrush background(NativePreviewColor(ColorFromColorRef(GetSysColor(COLOR_WINDOW))));
        graphics.FillPath(&background, &body);
    } else if (editor.dragging) {
        int emphasis = 24;
        Gdiplus::LinearGradientBrush background(
            Gdiplus::Point(layout.body.left, layout.body.top), Gdiplus::Point(layout.body.left, layout.body.bottom),
            !valid ? Gdiplus::Color(210, light ? 255 : 78, light ? 233 : 34, light ? 233 : 43) :
            light ? Gdiplus::Color(218 + emphasis, 253, 254, 255) : Gdiplus::Color(166 + emphasis, 52, 62, 82),
            !valid ? Gdiplus::Color(225, light ? 247 : 56, light ? 211 : 23, light ? 214 : 31) :
            light ? Gdiplus::Color(230 + emphasis, 230, 236, 245) : Gdiplus::Color(190 + emphasis, 25, 31, 44));
        graphics.FillPath(&background, &body);
    } else {
        // A zero-alpha layered pixel lets clicks through before WM_NCHITTEST.
        // Keep the whole edit surface grabbable without a visible glass fill.
        Gdiplus::SolidBrush hitArea(Gdiplus::Color(1, 0, 0, 0));
        graphics.FillPath(&hitArea, &body);
    }
    auto border = !valid ? Gdiplus::Color(255, 225, 83, 88) :
        contrast ? NativePreviewColor(ColorFromColorRef(GetSysColor(COLOR_WINDOWTEXT))) :
        light ? Gdiplus::Color(65, 82, 103, 139) : Gdiplus::Color(editor.dragging ? 110 : 65, 220, 232, 255);
    Gdiplus::Pen edge(border, dpi); graphics.DrawPath(&edge, &body);
    auto saved = graphics.Save();
    graphics.TranslateTransform(static_cast<float>(layout.content.left), static_cast<float>(layout.content.top));
    graphics.ScaleTransform(static_cast<float>(layout.contentScale), static_cast<float>(layout.contentScale));
    graphics.SetClip(Gdiplus::RectF(0, 0, static_cast<float>(editor.visual.width), static_cast<float>(editor.visual.height)));
    for (const auto& bar : editor.visual.bars) {
        if (bar.bounds.Width <= 0 || bar.bounds.Height <= 0) continue;
        Gdiplus::SolidBrush brush(NativePreviewColor(bar.color));
        graphics.FillRectangle(&brush, bar.bounds.X, bar.bounds.Y, bar.bounds.Width, bar.bounds.Height);
    }
    for (const auto& graph : editor.visual.graphs) {
        Gdiplus::Pen pen(NativePreviewColor(graph.color), 1.25f);
        pen.SetStartCap(Gdiplus::LineCapRound); pen.SetEndCap(Gdiplus::LineCapRound);
        for (const auto& run : graph.runs) {
            std::vector<Gdiplus::PointF> points;
            for (const auto& point : run) points.emplace_back(graph.bounds.X + point.x, graph.bounds.Y + point.y);
            if (points.size() > 1) graphics.DrawLines(&pen, points.data(), static_cast<int>(points.size()));
        }
    }
    for (const auto& text : editor.visual.texts) {
        if (text.bounds.Width <= 0 || text.bounds.Height <= 0) continue;
        auto font = MovePreviewFont(text);
        Gdiplus::SolidBrush brush(NativePreviewColor(text.color));
        Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericTypographic());
        format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);
        format.SetAlignment(text.right ? Gdiplus::StringAlignmentFar : Gdiplus::StringAlignmentNear);
        format.SetLineAlignment(Gdiplus::StringAlignmentCenter); format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        Gdiplus::RectF bounds(text.bounds.X, text.bounds.Y, text.bounds.Width, text.bounds.Height);
        graphics.DrawString(text.text.c_str(), static_cast<int>(text.text.size()), font.get(), bounds, &format, &brush);
    }
    graphics.Restore(saved);
    editor.renderedBody = layout.body; editor.renderedContent = layout.content;
    editor.renderedDpi = editor.target.scale; editor.renderedLayout = layout.widget;
    editor.dirty = false; return true;
}
void RefreshMovePreviewVisual() {
    auto session = g_moveEditor;
    if (!session || g_moveCommitting) return;
    CaptureWidgetPreviewContext capture{session->visual};
    if (OnPreviewSourceThread(session->source, CaptureWidgetPreview, &capture) &&
        session == g_moveEditor && capture.changed) {
        session->visual = std::move(capture.frame); session->dirty = true;
    }
}
bool UploadMovePreviewSurface(HWND window, const MovePreviewLayout& layout, const PreviewSurface& surface) {
    POINT destination{layout.bounds.left, layout.bounds.top}, origin{};
    SIZE size{layout.bounds.right - layout.bounds.left, layout.bounds.bottom - layout.bounds.top};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    return UpdateLayeredWindow(window, nullptr, &destination, &size, surface.dc,
                               &origin, 0, &blend, ULW_ALPHA) != FALSE;
}
void RenderMovePreview() {
    auto session = g_moveEditor;
    if (!session || session->closing) return;
    auto layout = CurrentMovePreviewLayout(*session);
    auto rect = layout.bounds;
    if (!PaintMovePreviewSurface(*session, layout)) {
        Wh_Log(L"Cannot render the live move preview"); CancelMoveEditor(); return;
    }
    if (GetWindowLongPtrW(session->window, GWL_EXSTYLE) & WS_EX_LAYERED) {
        if (!UploadMovePreviewSurface(session->window, layout, *session->surface)) {
            Wh_Log(L"Updating live move preview failed: Win32 error %u", GetLastError()); CancelMoveEditor(); return;
        }
        ShowWindow(session->window, SW_SHOWNOACTIVATE);
    } else { // Private test hosts use the same renderer through WM_PRINTCLIENT.
        SetWindowPos(session->window, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(session->window, nullptr, FALSE);
    }
}
void UpdateMoveCandidate(TaskbarProjection target, double left,
                         std::optional<WidgetPointerAnchor> pointer = std::nullopt) {
    if (!g_moveEditor) return;
    auto settings = CurrentSettings();
    PreferredPosition preferred{left, g_moveEditor->candidate.left, std::nullopt, pointer};
    auto resolved = target.key.empty() ? WidgetPlacement{} : ResolveWidgetPlacement(
        *settings, target.geometry, preferred, g_moveEditor->visual.fontMetrics);
    auto candidate = resolved.placement;
    if (pointer && candidate.width > 0)
        left = PreferredLeftForWidget(preferred, target.geometry, candidate.width,
                                      resolved.layout.width, resolved.layout.height);
    if ((candidate.width > 0) != (g_moveEditor->candidate.width > 0)) g_moveEditor->dirty = true;
    g_moveEditor->target = std::move(target);
    g_moveEditor->preferredLeft = left;
    g_moveEditor->candidate = candidate;
    RenderMovePreview();
}
void UpdateMoveCandidate(HWND window, double left) {
    auto session = g_moveEditor;
    auto target = ProjectTaskbar(window);
    if (session && g_moveEditor == session) UpdateMoveCandidate(std::move(target), left);
}
void VerifyMovedWidget(void* value) {
    auto& fits = *static_cast<bool*>(value);
    if (!g_rootGrid || !g_widgetHost) return;
    for (int i = 0; i < 3; ++i) {
        g_rootGrid.UpdateLayout();
        ++g_layoutRevision; RefreshTaskbarPlacement();
    }
    g_rootGrid.UpdateLayout();
    auto geometry = MeasureTaskbarGeometry(g_rootGrid, g_taskItemsRepeater, 0, &g_geometryCache);
    fits = geometry.ready && PlacementFits(geometry, {g_widgetHost.Margin().Left, g_widgetHost.Width(), 0}) &&
           (g_reservedMargin == 0 || ReservedControlsFit(geometry));
    if (fits) {
        auto settings = CurrentSettings();
        auto now = SampleTime::clock::now();
        UpdateSparkline(g_cpuGraph, g_cpuHistory, *settings, now);
        UpdateSparkline(g_gpuGraph, g_gpuHistory, *settings, now);
    }
}
struct PreviewPlacementVerification {
    TaskbarPlacement expected;
    WidgetLayout layout;
    bool fits = false;
};
void VerifyPreviewedWidget(void* value) {
    auto& context = *static_cast<PreviewPlacementVerification*>(value);
    VerifyMovedWidget(&context.fits);
    context.fits = context.fits && g_widgetLayout && SameWidgetLayout(*g_widgetLayout, context.layout) &&
        std::abs(g_widgetHost.Margin().Left - context.expected.left) <= kPlacementTolerance &&
        std::abs(g_widgetHost.Width() - context.expected.width) <= kPlacementTolerance &&
        std::abs(g_widgetLayout->scale - context.layout.scale) < 1e-6;
}

template <class Apply, class Verify, class Rollback>
bool CompleteMoveTransaction(const PlacementProfiles& before, const PlacementProfiles& next,
                             uint64_t epoch, Apply apply, Verify verify, Rollback rollback) {
    g_moveCommitting = true;
    struct Guard { ~Guard() { g_moveCommitting = false; } } guard;
    bool saved = false;
    try {
        SetPlacementProfiles(next);
        if (apply() && verify() && epoch == g_moveEpoch.load()) {
            saved = SavePlacementProfiles(next);
            if (saved && epoch == g_moveEpoch.load()) return true;
        }
    } catch (...) {
        Wh_Log(L"Move transaction failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
    }
    SetPlacementProfiles(before);
    if (!rollback()) Wh_Log(L"Previous widget could not be restored immediately; placement retry is pending");
    if (saved) SavePlacementProfiles(PlacementProfilesSnapshot());
    return false;
}
bool CommitMoveEditor() {
    if (!g_moveEditor || !g_moveEditor->candidate.width) return false;
    auto epoch = g_moveEpoch.load();
    auto editor = *g_moveEditor;
    UpdateMoveCandidate(editor.target.window, editor.preferredLeft);
    if (!g_moveEditor || !g_moveEditor->candidate.width) return false;
    editor = *g_moveEditor;
    auto before = PlacementProfilesSnapshot();
    auto settings = CurrentSettings();
    auto confirmed = ConfirmPlacementPreference(before, *settings, editor.target.key,
        editor.candidate.left, editor.target.geometry.width, editor.reset, editor.candidate.width);
    if (!confirmed) {
        Wh_Log(L"Cannot save this display position (missing identity or 32-profile limit); reset with Home first");
        return false;
    }
    auto next = std::move(*confirmed);
    HWND previousWindow = g_taskbarWindow.load();
    bool succeeded = CompleteMoveTransaction(before, next, epoch, [&] {
        return ApplyOnTaskbarThread(nullptr, true, editor.target.window);
    }, [&] {
        PreviewPlacementVerification verification{editor.candidate,
            ResolveMovePreviewLayout(editor, editor.target.panel).widget};
        if (g_taskbarWindow.load() == editor.target.window)
            RunFromWindowThread(editor.target.window, VerifyPreviewedWidget, &verification);
        return verification.fits;
    }, [&] {
        ReconcilePlacementSettings();
        return ApplyOnTaskbarThread(nullptr, true,
            epoch == g_moveEpoch.load() ? previousWindow : FindConfiguredTaskbarWindow());
    });
    if (!succeeded) {
        Wh_Log(L"Move could not be applied; restored the preceding position preference");
        if (epoch != g_moveEpoch.load()) CancelMoveEditor();
        else if (g_moveEditor && g_moveEditor->sourceHidden) {
            SourceWidgetOpacityContext opacity{g_moveEditor->source, true};
            OnPreviewSourceThread(opacity.source, SetSourceWidgetOpacity, &opacity);
        }
    }
    return succeeded;
}
void CancelMoveEditor() {
    ++g_moveEpoch;
    if (g_moveCommitting) return;
    if (HWND window = g_moveEditorWindow.load()) SendMessageW(window, WM_CLOSE, 0, 0);
}
void UpdateDraggedCandidate(bool finalPoint = false) {
    auto session = g_moveEditor;
    if (!session || !session->dragging) return;
    auto now = std::chrono::steady_clock::now();
    if (!finalPoint && now - session->lastProbe < std::chrono::milliseconds(16)) return;
    session->lastProbe = now;
    POINT point; if (!GetPhysicalCursorPos(&point)) return;
    HWND target = FindTaskbarWindowForMonitor(MonitorFromPoint(point, MONITOR_DEFAULTTONULL));
    RECT bounds{};
    if (!target || !GetWindowRect(target, &bounds) || !PtInRect(&bounds, point)) {
        session->pointerOutside = true; session->candidate = {}; session->dirty = true;
        RenderMovePreview(); return;
    }
    session->pointerOutside = false;
    auto projection = ProjectTaskbar(target);
    if (g_moveEditor != session) return;
    WidgetPointerAnchor pointer{ScreenPointToTaskbarX(projection, point), session->grabFraction};
    UpdateMoveCandidate(std::move(projection), session->preferredLeft, pointer);
}
LRESULT CALLBACK MoveEditorProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    try {
        ScopedPhysicalDpi physicalCoordinates;
        if (message == WM_CLOSE) { DestroyWindow(window); return 0; }
        if (message == WM_DESTROY) {
            auto closing = g_moveEditor;
            if (closing) closing->closing = true;
            HWND previous = g_moveEditor ? g_moveEditor->previousForeground : nullptr;
            bool restoreFocus = GetForegroundWindow() == window;
            if (GetCapture() == window) ReleaseCapture();
            KillTimer(window, 1); g_moveEditorWindow = nullptr;
            if (closing && closing->sourceHidden) {
                SourceWidgetOpacityContext opacity{closing->source, false, closing->sourceOpacity};
                OnPreviewSourceThread(opacity.source, SetSourceWidgetOpacity, &opacity);
            }
            g_moveEditor.reset(); StopPreviewGraphics();
            if (restoreFocus && IsWindow(previous)) SetForegroundWindow(previous);
            return 0;
        }
        if (!g_moveEditor) return DefWindowProcW(window, message, wParam, lParam);
        auto editorOwner = g_moveEditor;
        auto& editor = *editorOwner;
        if (message == WM_ERASEBKGND) return 1;
        if (message == WM_SETCURSOR && LOWORD(lParam) == HTCLIENT) {
            SetCursor(LoadCursorW(nullptr, IDC_HAND)); return TRUE;
        }
        if (message == WM_NCHITTEST) {
            POINT point{static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam))};
            ScreenToClient(window, &point);
            auto layout = CurrentMovePreviewLayout(editor);
            return PtInRect(&layout.body, point) ? HTCLIENT : HTTRANSPARENT;
        }
        if (message == WM_LBUTTONDOWN) {
            auto layout = CurrentMovePreviewLayout(editor);
            // Preserve clicks in the Viewbox's centered side margins too.
            // Placement bounds the held offset if the destination has no margin.
            editor.grabFraction = static_cast<double>(static_cast<short>(LOWORD(lParam)) - layout.content.left) /
                                  std::max<LONG>(1, layout.content.right - layout.content.left);
            editor.dragging = true; editor.reset = false; editor.dirty = true;
            SetCapture(window); RenderMovePreview(); return 0;
        }
        if (message == WM_LBUTTONUP || message == WM_CAPTURECHANGED) {
            if (message == WM_LBUTTONUP) UpdateDraggedCandidate(true);
            editor.dragging = false; editor.dirty = true;
            if (GetCapture() == window) ReleaseCapture();
            RenderMovePreview();
            return 0;
        }
        if (message == WM_MOUSEMOVE) {
            if (!editor.hovered) {
                editor.hovered = true; editor.dirty = true;
                TRACKMOUSEEVENT tracking{sizeof(tracking), TME_LEAVE, window, 0}; TrackMouseEvent(&tracking);
                RenderMovePreview();
            }
            if (editor.dragging) UpdateDraggedCandidate();
            return 0;
        }
        if (message == WM_MOUSELEAVE) { editor.hovered = false; editor.dirty = true; RenderMovePreview(); return 0; }
        if (message == WM_TIMER && wParam == 1) {
            RefreshMovePreviewVisual();
            if (g_moveEditor != editorOwner) return 0;
            if (!editor.dragging && !editor.pointerOutside) UpdateMoveCandidate(editor.target.window, editor.preferredLeft);
            else if (editor.dirty) RenderMovePreview();
            return 0;
        }
        if (message == WM_KEYDOWN) {
            if (wParam == VK_ESCAPE) { DestroyWindow(window); return 0; }
            if (wParam == VK_RETURN) {
                if (CommitMoveEditor()) DestroyWindow(window);
                return 0;
            }
            if (wParam == VK_HOME) {
                auto settings = CurrentSettings(); auto monitors = EnumerateDisplayMonitors();
                HWND target = settings->monitor <= static_cast<int>(monitors.size())
                    ? FindTaskbarWindowForMonitor(monitors[settings->monitor - 1].handle) : nullptr;
                if (!target) target = FindPrimaryTaskbarWindow();
                editor.reset = true; editor.pointerOutside = false; editor.dirty = true;
                UpdateMoveCandidate(target, settings->leftOffset); return 0;
            }
        }
        if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE && !g_moveCommitting) {
            DestroyWindow(window); return 0;
        }
        if (message == WM_PAINT || message == WM_PRINTCLIENT) {
            PAINTSTRUCT paint{};
            HDC dc = message == WM_PAINT ? BeginPaint(window, &paint) : reinterpret_cast<HDC>(wParam);
            if (PaintMovePreviewSurface(editor, CurrentMovePreviewLayout(editor))) {
                Gdiplus::Bitmap bitmap(editor.surface->width, editor.surface->height, editor.surface->width * 4,
                    PixelFormat32bppPARGB, reinterpret_cast<BYTE*>(editor.surface->pixels));
                Gdiplus::Graphics graphics(dc); graphics.DrawImage(&bitmap, 0, 0);
            }
            if (message == WM_PAINT) EndPaint(window, &paint);
            return 0;
        }
    } catch (...) {
        Wh_Log(L"Move editor failed: %08X", static_cast<unsigned>(winrt::to_hresult()));
        DestroyWindow(window); return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
void BeginMoveEditor() {
    ScopedPhysicalDpi physicalCoordinates;
    if (g_moveEditorWindow) { CancelMoveEditor(); return; }
    HWND target = g_taskbarWindow.load();
    if (!IsCurrentProcessTaskbarWindow(target)) target = FindConfiguredTaskbarWindow();
    auto projection = ProjectTaskbar(target);
    if (!projection.geometry.ready || projection.key.empty()) {
        Wh_Log(L"Move editor unavailable: taskbar geometry or stable display identity missing"); return;
    }
    WNDCLASSW cls{}; cls.hInstance = PlacementModule(); cls.lpfnWndProc = MoveEditorProc;
    cls.lpszClassName = kMoveWindowClass; cls.hCursor = LoadCursorW(nullptr, IDC_HAND);
    if (!RegisterClassW(&cls) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return;
    auto settings = CurrentSettings();
    MoveEditorState editor; editor.previousForeground = GetForegroundWindow(); editor.source = target;
    editor.target = projection;
    editor.preferredLeft = PreferredTaskbarLeft(target, projection.geometry.width, *settings);
    editor.candidate = ResolveTaskbarPlacement(*settings, projection.geometry,
                                              {editor.preferredLeft, editor.preferredLeft});
    g_moveEditor = std::make_shared<MoveEditorState>(std::move(editor));
    HWND window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        kMoveWindowClass, L"Taskbar System Info - Move", WS_POPUP, 0, 0, 1, 1,
        nullptr, nullptr, cls.hInstance, nullptr);
    if (!window) { g_moveEditor.reset(); return; }
    g_moveEditor->window = window; g_moveEditorWindow = window;
    RefreshMovePreviewVisual();
    auto initial = ResolveWidgetPlacement(*settings, projection.geometry,
        {g_moveEditor->preferredLeft, g_previousPlacementLeft.load(), SavedTaskbarFraction(target)},
        g_moveEditor->visual.fontMetrics);
    g_moveEditor->candidate = initial.placement;
    if (initial.placement.width > 0) g_moveEditor->preferredLeft = initial.placement.left;
    RenderMovePreview();
    if (!g_moveEditor) return;
    SourceWidgetOpacityContext opacity{target, true};
    if (OnPreviewSourceThread(target, SetSourceWidgetOpacity, &opacity) && opacity.changed) {
        g_moveEditor->sourceHidden = true; g_moveEditor->sourceOpacity = opacity.original;
    }
    SetForegroundWindow(window); SetFocus(window); SetTimer(window, 1, 150, nullptr);
}
std::wstring g_registeredMoveHotkey;
bool g_hotkeyRegistered = false;
std::atomic<bool> g_hotkeyRefreshPending{false};
LRESULT CALLBACK PlacementControlProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == kGeometryMessage) {
        g_geometryQueued = false;
        if (!g_unloading && window == g_placementControlWindow) RefreshTaskbarPlacement();
        return 0;
    }
    if (message == WM_HOTKEY && wParam == kMoveHotkeyId && !g_unloading) {
        BeginMoveEditor(); return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
void QueueTaskbarPlacement() {
    if (!g_unloading && !g_geometryQueued && g_placementControlWindow)
        g_geometryQueued = PostMessageW(g_placementControlWindow, kGeometryMessage, 0, 0) != FALSE;
}
void EnsurePlacementControl(const ModSettings& settings) {
    if (!g_placementControlWindow) {
        WNDCLASSW cls{}; cls.hInstance = PlacementModule(); cls.lpfnWndProc = PlacementControlProc;
        cls.lpszClassName = kPlacementWindowClass;
        if (!RegisterClassW(&cls) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return;
        g_placementControlWindow = CreateWindowExW(0, cls.lpszClassName, L"", 0, 0, 0, 0, 0,
                                                  HWND_MESSAGE, nullptr, cls.hInstance, nullptr);
        g_registeredMoveHotkey = L"\n";
    }
    if (!g_placementControlWindow) return;
    bool retryRegistration = g_hotkeyRefreshPending.exchange(false);
    if (!retryRegistration && g_registeredMoveHotkey == settings.moveHotkey) return;
    if (g_hotkeyRegistered) UnregisterHotKey(g_placementControlWindow, kMoveHotkeyId);
    g_hotkeyRegistered = false; g_registeredMoveHotkey = settings.moveHotkey;
    std::wstring reason;
    auto hotkey = ParseMoveHotkey(settings.moveHotkey, &reason);
    if (!hotkey) { Wh_Log(L"Invalid moveHotkey '%s': %s", settings.moveHotkey.c_str(), reason.c_str()); return; }
    if (!hotkey->key) return;
    g_hotkeyRegistered = RegisterHotKey(g_placementControlWindow, kMoveHotkeyId,
                                       hotkey->modifiers | MOD_NOREPEAT, hotkey->key) != FALSE;
    if (!g_hotkeyRegistered) {
        DWORD error = GetLastError();
        Wh_Log(L"Cannot register moveHotkey '%s': %s (Win32 error %u)", settings.moveHotkey.c_str(),
               error == ERROR_HOTKEY_ALREADY_REGISTERED ? L"Combination is already registered" : L"RegisterHotKey failed", error);
    }
}
void RemovePlacementControl() {
    if (!g_moveCommitting) CancelMoveEditor();
    if (g_placementControlWindow) {
        if (g_hotkeyRegistered) UnregisterHotKey(g_placementControlWindow, kMoveHotkeyId);
        DestroyWindow(g_placementControlWindow); g_placementControlWindow = nullptr;
    }
    g_hotkeyRegistered = false; g_geometryQueued = false; g_registeredMoveHotkey.clear();
    if (!g_moveEditorWindow) StopPreviewGraphics();
    UnregisterClassW(kPlacementWindowClass, PlacementModule());
    if (!g_moveEditorWindow) UnregisterClassW(kMoveWindowClass, PlacementModule());
}

void StartPlacementRetryWorker() {
    std::lock_guard lock(g_placementRetryWorkerMutex);
    if (g_unloading) {
        return;
    }
    if (g_placementRetryWorker && g_placementRetryWorker->joinable()) {
        if (g_placementRetryWorkerRunning) {
            if (g_placementRetryWakeEvent) {
                SetEvent(g_placementRetryWakeEvent);
            }
            return;
        }
        g_placementRetryWorker->join();
        g_placementRetryWorker.reset();
    }

    if (!g_placementRetryWakeEvent) {
        g_placementRetryWakeEvent =
            CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_placementRetryWakeEvent) {
            Wh_Log(L"Creating taskbar placement retry event failed: %u",
                   GetLastError());
            return;
        }
    }

    g_stopPlacementRetryWorker = false;
    g_placementRetryWorkerRunning = true;
    HANDLE retryWakeEvent = g_placementRetryWakeEvent;
    try {
        g_placementRetryWorker.emplace([retryWakeEvent] {
            constexpr int kMaximumFastStartupRetryAttempts = 30;
            int fastAttempts = 0;
            bool slowBackoffLogged = false;
            while (true) {
                if (g_stopPlacementRetryWorker || g_unloading ||
                    !g_placementApplyPending) {
                    break;
                }
                if (ApplyOnTaskbarThread()) {
                    break;
                }
                bool slowBackoff =
                    fastAttempts >= kMaximumFastStartupRetryAttempts;
                if (slowBackoff && !slowBackoffLogged) {
                    Wh_Log(L"Taskbar placement is still unavailable; retrying "
                           L"every 30 seconds");
                    slowBackoffLogged = true;
                }
                DWORD waitResult = WaitForSingleObject(
                    retryWakeEvent, slowBackoff ? 30000 : 1000);
                if (waitResult == WAIT_FAILED) {
                    Wh_Log(L"Taskbar placement retry wait failed: %u",
                           GetLastError());
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                } else if (waitResult == WAIT_OBJECT_0) {
                    fastAttempts = 0;
                    slowBackoffLogged = false;
                } else if (!slowBackoff) {
                    fastAttempts++;
                }
            }
            g_placementRetryWorkerRunning = false;
        });
    } catch (...) {
        g_placementRetryWorkerRunning = false;
        Wh_Log(L"Starting taskbar placement retry worker failed: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
}

void StopPlacementRetryWorker() {
    std::optional<std::thread> worker;
    HANDLE wakeEvent = nullptr;
    {
        std::lock_guard lock(g_placementRetryWorkerMutex);
        g_stopPlacementRetryWorker = true;
        if (g_placementRetryWakeEvent) {
            SetEvent(g_placementRetryWakeEvent);
        }
        if (g_placementRetryWorker && g_placementRetryWorker->joinable()) {
            worker.emplace(std::move(*g_placementRetryWorker));
        }
        g_placementRetryWorker.reset();
        wakeEvent = g_placementRetryWakeEvent;
        g_placementRetryWakeEvent = nullptr;
    }
    if (worker && worker->joinable()) {
        worker->join();
    }
    if (wakeEvent) {
        CloseHandle(wakeEvent);
    }
    g_placementRetryWorkerRunning = false;
}

void EnsureConfiguredTaskbarPlacement() {
    if (g_unloading) {
        return;
    }

    HWND rememberedWindow = g_taskbarWindow.load();
    bool placementApplyPending = g_placementApplyPending.load();
    if (!placementApplyPending && !g_placementIsFallback &&
        g_placementFailures == 0 && g_widget &&
        IsCurrentProcessTaskbarWindow(rememberedWindow)) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    if (g_placementFailures != 0 && now < g_nextPlacementRetry) {
        return;
    }

    auto monitors = EnumerateDisplayMonitors();
    HWND targetWindow = FindConfiguredTaskbarWindow(monitors, false);
    if (!targetWindow) {
        MarkPlacementForRetry(nullptr);
        return;
    }
    if (!placementApplyPending && g_widget &&
        IsCurrentProcessTaskbarWindow(rememberedWindow) &&
        rememberedWindow == targetWindow && !g_placementIsFallback) {
        ResetPlacementRetryState();
        return;
    }

    Wh_Log(L"Retrying taskbar placement");
    ApplyOnTaskbarThread(nullptr, false, targetWindow);
}

void ApplyLoadedTaskbarFrame(FrameworkElement taskbarFrame) {
    if (g_unloading) {
        return;
    }
    ResetPlacementRetryState();
    if (!taskbarFrame) {
        ApplyOnTaskbarThread();
        return;
    }

    // The constructor hook already gives us the stable XAML element. Use it
    // directly whenever its taskbar is unambiguous, and keep the symbol-based
    // XAML-root lookup only for choosing between multiple monitor taskbars.
    HWND directWindow = FindOnlyTaskbarWindow();
    if (!directWindow && g_widget) {
        XamlRoot loadedRoot = taskbarFrame.XamlRoot();
        XamlRoot widgetRoot = g_widget.XamlRoot();
        HWND rememberedWindow = g_taskbarWindow.load();
        if (loadedRoot && widgetRoot && loadedRoot == widgetRoot &&
            IsCurrentProcessTaskbarWindow(rememberedWindow)) {
            directWindow = rememberedWindow;
        }
    }

    if (directWindow && InjectWidget(taskbarFrame)) {
        RememberTaskbarWindow(directWindow);
        CompletePlacement(directWindow, directWindow);
        return;
    }
    ApplyOnTaskbarThread(taskbarFrame);
}

using TaskbarFrame_Constructor_t = void*(WINAPI*)(void* pThis);
TaskbarFrame_Constructor_t TaskbarFrame_Constructor_Original = nullptr;

void* WINAPI TaskbarFrame_Constructor_Hook(void* pThis) {
    void* result = TaskbarFrame_Constructor_Original(pThis);
    g_taskbarThreadId = GetCurrentThreadId();
    if (g_unloading || !g_loadedRevokers) {
        return result;
    }

    try {
        FrameworkElement taskbarFrame = nullptr;
        reinterpret_cast<::IUnknown**>(pThis)[1]->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarFrame));
        if (!taskbarFrame) {
            return result;
        }

        g_loadedRevokers->emplace_back();
        auto revoker = std::prev(g_loadedRevokers->end());
        try {
            *revoker = taskbarFrame.Loaded(
                winrt::auto_revoke_t{},
                [revoker](IInspectable const& sender,
                          RoutedEventArgs const&) {
                    if (!g_loadedRevokers) {
                        return;
                    }
                    FrameworkElement loadedFrame =
                        sender.try_as<FrameworkElement>();
                    g_loadedRevokers->erase(revoker);
                    if (g_unloading) {
                        return;
                    }
                    try {
                        ApplyLoadedTaskbarFrame(loadedFrame);
                    } catch (...) {
                        HRESULT error = winrt::to_hresult();
                        Wh_Log(L"Loaded injection failed: %08X",
                               static_cast<unsigned>(error));
                    }
                });
            g_taskbarUiResourcesRegistered = true;
        } catch (...) {
            g_loadedRevokers->erase(revoker);
            throw;
        }
    } catch (...) {
        HRESULT error = winrt::to_hresult();
        Wh_Log(L"Registering taskbar Loaded handler failed: %08X",
               static_cast<unsigned>(error));
    }
    return result;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
         &CSecondaryTaskBand_ITaskListWndSite_vftable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &CTaskBand_GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &CSecondaryTaskBand_GetTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
         &TaskbarHost_FrameHeight_Original},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
         &RefCountBase_Decref_Original},
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      std::size(taskbarDllHooks));
}

bool HookTaskbarViewSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: __cdecl winrt::Taskbar::implementation::TaskbarFrame::TaskbarFrame(void))"},
        &TaskbarFrame_Constructor_Original,
        TaskbarFrame_Constructor_Hook,
    }};
    return WindhawkUtils::HookSymbols(module, hooks, std::size(hooks));
}

HMODULE GetTaskbarViewModule() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

bool HandleLoadedModuleIfTaskbarView(HMODULE module,
                                     LPCWSTR fileName,
                                     bool applyImmediately) {
    if (!module || GetTaskbarViewModule() != module) {
        return false;
    }
    if (g_taskbarViewHookAttempted.exchange(true)) {
        return static_cast<bool>(g_taskbarViewDllLoaded);
    }

    Wh_Log(L"Taskbar view module loaded: %s",
           fileName ? fileName : L"<already loaded>");
    if (!HookTaskbarViewSymbols(module)) {
        Wh_Log(L"Taskbar.View symbol hook failed");
        return false;
    }
    g_taskbarViewDllLoaded = true;
    if (applyImmediately) {
        Wh_ApplyHookOperations();
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName,
                                   HANDLE file,
                                   DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    DWORD lastError = GetLastError();
    if (module && !g_taskbarViewHookAttempted) {
        HandleLoadedModuleIfTaskbarView(module, fileName, true);
    }
    SetLastError(lastError);
    return module;
}

void CloseMetricSources() {
    ClosePdhQuery();
    CloseCpuPdhQuery();
    g_nextCpuPdhCounterRetry = {};
    InvalidateGpuAdapterCache();
    g_gpuTemperatureRetry = {};
    g_hwInfoSharedMemoryCache = {};
    g_hwInfoGadgetRegistryCache = {};
    g_nextPdhCounterRetry = {};
    g_nextPdhRecovery = {};
    g_consecutivePdhReadFailures = 0;
    g_lastResolvedGpuAdapterFilter.reset();
    g_lastResolvedGpuAdapterLuid.reset();
    g_gpuAdapterIdentityChanged = false;
    g_hasResolvedGpuAdapterIdentity = false;
    g_hwInfoInvalidUnitLogged = false;
    g_hwInfoLayoutRejectedLogged = false;
    g_hwInfoGpuAdapterMismatchLogged = false;
}

bool TearDownTaskbarUi() {
    if (!g_taskbarUiResourcesRegistered) {
        g_loadedRevokers.reset();
        g_taskbarWindow = nullptr;
        g_taskbarThreadId = 0;
        return true;
    }
    DWORD attemptedThreadIds[3]{};
    size_t attemptedThreadCount = 0;
    HWND rememberedWindow = FindRememberedTaskbarWindow();
    HWND candidates[] = {rememberedWindow,
                         FindAnyWindowOnTaskbarThread(rememberedWindow),
                         FindPrimaryTaskbarWindow()};
    for (size_t i = 0; i < std::size(candidates); i++) {
        HWND window = candidates[i];
        if (!window || std::find(candidates, candidates + i, window) !=
                           candidates + i) {
            continue;
        }
        DWORD threadId = GetWindowThreadProcessId(window, nullptr);
        if (!threadId ||
            std::find(attemptedThreadIds,
                      attemptedThreadIds + attemptedThreadCount,
                      threadId) != attemptedThreadIds + attemptedThreadCount) {
            continue;
        }
        attemptedThreadIds[attemptedThreadCount++] = threadId;
        RemoveTaskbarUiContext context;
        if (RunFromWindowThread(window, RemoveFromCurrentTaskbar, &context)) {
            return context.succeeded;
        }
    }
    return false;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_unloading = false;
    g_taskbarUiResourcesRegistered = false;
    g_uiTornDown = false;
    g_loadedRevokers.emplace();
    {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        g_windowThreadCallbackRegistry.emplace();
    }
    if (HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll")) {
        g_d3dkmtEnumAdapters2 = reinterpret_cast<D3DKMTEnumAdapters2_t>(
            GetProcAddress(gdi32, "D3DKMTEnumAdapters2"));
        g_d3dkmtOpenAdapterFromLuid =
            reinterpret_cast<D3DKMTOpenAdapterFromLuid_t>(
                GetProcAddress(gdi32, "D3DKMTOpenAdapterFromLuid"));
        g_d3dkmtQueryAdapterInfo =
            reinterpret_cast<D3DKMTQueryAdapterInfo_t>(
                GetProcAddress(gdi32, "D3DKMTQueryAdapterInfo"));
        g_d3dkmtCloseAdapter = reinterpret_cast<D3DKMTCloseAdapter_t>(
            GetProcAddress(gdi32, "D3DKMTCloseAdapter"));
    }

    LoadSettings();
    LoadPlacementProfiles();
    ReconcilePlacementSettings();

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols unavailable");
        return FALSE;
    }

    if (HMODULE module = GetTaskbarViewModule()) {
        if (!HandleLoadedModuleIfTaskbarView(module, nullptr, false)) {
            Wh_Log(L"Taskbar.View symbols unavailable");
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelBase) {
            kernelBase = GetModuleHandleW(L"kernel32.dll");
        }
        auto loadLibraryEx = kernelBase
                                 ? reinterpret_cast<LoadLibraryExW_t>(
                                       GetProcAddress(kernelBase,
                                                      "LoadLibraryExW"))
                                 : nullptr;
        if (!loadLibraryEx ||
            !WindhawkUtils::SetFunctionHook(loadLibraryEx, LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            return FALSE;
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (!g_taskbarViewHookAttempted) {
        if (HMODULE module = GetTaskbarViewModule()) {
            HandleLoadedModuleIfTaskbarView(module, nullptr, true);
        }
    }
    if (!ApplyOnTaskbarThread()) {
        StartPlacementRetryWorker();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    CancelMoveEditor();
    LoadSettings();
    g_hotkeyRefreshPending = true;
    ReconcilePlacementSettings();
    WakeMetricsWorker();
    if (!ApplyOnTaskbarThread(nullptr, true)) {
        StartPlacementRetryWorker();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
    CancelMoveEditor();
    StopPlacementRetryWorker();
    StopMetricsWorker();

    g_uiTornDown = TearDownTaskbarUi();
    if (g_uiTornDown) StopFontGraphics();
    if (!g_uiTornDown) {
        Wh_Log(L"Initial taskbar UI teardown failed; will retry");
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    StopPlacementRetryWorker();
    if (!g_uiTornDown) {
        g_uiTornDown = TearDownTaskbarUi();
        if (!g_uiTornDown) {
            Wh_Log(L"Taskbar UI teardown retry failed");
        }
    }
    if (g_uiTornDown) StopFontGraphics();
    // Both workers were joined in BeforeUninit; the metrics worker owns and
    // closes all provider handles before returning.
    {
        std::lock_guard lock(g_windowThreadCallbackRegistryMutex);
        g_windowThreadCallbackRegistry.reset();
    }
}

// ==WindhawkMod==
// @id              siri-plus-launchpad
// @name            Siri Plus Launchpad
// @description     Replaces the Start menu with a macOS 27 Siri-style launcher: search bar with categories, app tabs, files, clipboard, actions and live liquid glass
// @version         1.43.3
// @author          Thiago Silva
// @github          https://github.com/brideinflame-rgb
// @homepage        https://github.com/brideinflame-rgb/Siri-Plus-Launchpad
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32 -lshcore -ld2d1 -ldwrite -lole32 -luuid -lwindowscodecs -lshlwapi -ldwmapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Siri Plus Launchpad

A [Windhawk](https://windhawk.net) mod that replaces the Windows 11 Start menu with a macOS-inspired launcher: a Siri-style search bar with a categories menu, app tabs, file search, clipboard history, quick actions and live liquid-glass effects.

Press the **Windows key** (or click **Start**) and type, or pick what you want to search from the menu that drops down from the bar.

---

## Highlights

- **Four modes**: Original Siri (default), Floating, Window and Fullscreen
- **Live liquid glass**: real-time blur behind the bar and panels (animated wallpapers and videos keep playing underneath)
- **One search for everything**: Apps, Files, Clipboard and Actions
- **App tabs**: All, automatic categories and up to 6 tabs of your own
- **Recent apps row**: recently opened or installed apps (or the most used ones) on top
- **Power options**: type *shut down*, *restart*, *sleep*, *lock* or *sign out*, or use the floating power button
- **Custom icons**: give any app your own icon (PNG, JPG, ICO, WebP...)
- **Themes**: Clear glass, Light, Siri (macOS 27 gradient) and a fully Custom theme
- **Your language**: the interface follows the Windows display language (English, Portuguese, Spanish, French, German and Italian)

---

## Modes

### Original Siri (default)
A search bar in the macOS 27 style.

- **Categories menu**: hover over the bar (or click it) and a menu drops down with **Applications**, **Files**, **Actions** and **Clipboard**, each with its icon and its keyboard shortcut (default **Ctrl+1 … Ctrl+4**, configurable).
- Picking a category makes its icon and name glide from the menu into the bar, and the search then works inside that category. If you type straight away, you go to Applications.
- **Going back**:
  - Hover the category icon in the bar and it turns into a back arrow. Click it, or press **Esc**, to return to "Search" with the magnifier.
  - Click the empty text next to the icon to reopen the categories menu and switch directly.
- **App tabs**:
  - **All** comes first and shows every app.
  - Then the automatic categories: Utilities, Productivity, Entertainment, Social, Creativity, Reading and Other.
  - Then up to **6 tabs of your own**, for example "Games".
- **First row**: recently opened or installed apps, or the most used ones (configurable), separated from the rest by a thin line.
- **Actions**: Lock, Sleep, Restart, Shut down, Sign out, Settings, Task Manager, File Explorer, Control Panel, Screenshot and Empty Recycle Bin.

### Floating
The bar appears out of an animated circle, the **Siri wave** or a soft glow, with zoom and an adjustable bounce. Clicking it or typing slides the results panel down. When it closes, the bar is "sucked" back into the bubble.

### Window and Fullscreen
The classic Launchpad grid with pages or vertical scrolling, folders and a power button. Fullscreen respects the work area and the space reserved by docks.

---

## Managing app tabs (Original Siri)

| What | How |
|---|---|
| Create a tab | Right-click an app → **New tab with this app**, or right-click the tab strip / an empty area → **New tab**. Type the name and press **Enter** |
| Add apps | Inside a tab, right-click → **Add app**. A picker shows every app: **long-press** to select as many as you want (selected apps jiggle), then **Done** |
| Remove an app | Inside a tab, right-click the app → **Remove from tab** |
| Rename or delete | Right-click the tab → **Rename tab** / **Delete tab** |
| Built-in tabs | They can also be edited (add or remove apps) and deleted. **Restore default tabs** brings them back |
| Move through tabs | **Tab / Shift+Tab**, or click and drag the strip sideways (the mouse wheel works too) |

---

## Files
- **Indexer**: Everything (through IPC, no DLL needed), Windows Search or a direct scan. The default, Automatic, uses the first one that is available.
- When the search is empty, your **recent searches** are shown, with a button to clear them. History can be turned off.
- Results show the **folder** of each file under its name, in a list or as icons.
- The maximum number of results is configurable.

## Clipboard
- Keeps the texts you copy while the mod runs. The number of items is configurable.
- Shown as a **list**: the first line of each text, with a preview of the rest (or its size) underneath.
- Click an item to copy it again. Turning the category off also stops monitoring and clears the history.

## Apps and icons
- **Search** apps by typing.
- **Folders** (Window/Fullscreen): drag apps together, rename, undo.
- **Multi-select** with jiggling icons, **pin to taskbar** (also by dragging an icon to the taskbar), **uninstall**.
- **Custom icons**: right-click an app → **Change icon...** and pick an image (PNG, JPG, JPEG, BMP, ICO, GIF or WebP). The new icon is remembered across restarts; **Reset icon** brings back the original. Works in every mode, including apps inside folders.
- Icon size, number of columns and the scrolling direction are configurable.

---

## Customization

All options are in the mod's **Settings** tab in Windhawk, with names in English and Portuguese.

| Block | Options |
|---|---|
| ① Mode | Original Siri (default), Floating, Window, Fullscreen |
| ② Theme | Clear glass, Light, Siri (macOS 27 gradient), Custom |
| ③ Appearance | Glass transparency, corner rounding, background blur (0 = no blur) |
| ④ Custom theme | Color, opacity, dark-to-light gradient, border width, color and opacity |
| ⑤ Window and Fullscreen | Window size, search style (top bar or centered box), power button and its position, opening and closing animations (pop, fade, rise from/sink into the taskbar, CRT), animation speed, blur behind open folders |
| ⑥ Floating and Original Siri | Bar size, results box width and rows, circle animation (Siri wave, soft glow, none), circle zoom, bounce of the circle and of the results box, opening/closing duration, animation speed, power options when typing, floating power button, first row (recent, most used, off), Original Siri category shortcuts for Applications, Files, Actions and Clipboard (defaults **Ctrl+1**, **Ctrl+2**, **Ctrl+3**, **Ctrl+4**; any combination starting with Ctrl, e.g. Ctrl+Shift+F or Ctrl+F2) |
| ⑦ Opening | Open with the Windows key and/or the Start button |
| ⑧ Apps and icons | Icon size, columns, scrolling direction, jiggle, drag to the taskbar to pin |
| ⑨ Files | On/off, search history, list or icons, indexer, maximum results |
| ⑩ Clipboard | On/off, number of items kept |

---

## Keyboard and mouse

| Key / action | What it does |
|---|---|
| **Win** / Start button | Open or close |
| Type | Search (in Original Siri, typing right away searches Applications) |
| **Ctrl+1 / 2 / 3 / 4** | Original Siri: go to Applications / Files / Actions / Clipboard (configurable) |
| **Tab / Shift+Tab** | Original Siri: next/previous app tab, or move through the results in Files, Clipboard and Actions |
| Arrows + **Enter** | Move through results / open |
| Other **Ctrl+digits** | Launch the matching result |
| **Esc** | Closes menus, then clears the search, goes back to the categories and finally closes |
| Right-click on the search bar | Cut, Copy, Paste, Select all, Delete all |
| Long-press an app | Select it (in Original Siri, inside the app picker) |

---

## Requirements
- Windows 11 (x64)
- [Windhawk](https://windhawk.net)
- Live blur needs **Transparency effects** turned on in Windows (*Settings → Personalization → Colors*). Without it, a blurred still image is used.
- Optional: [Everything](https://www.voidtools.com) for instant file search (run it normally, not as administrator)

## Installation
1. Install [Windhawk](https://windhawk.net).
2. Search for **Siri Plus Launchpad** in Windhawk's mod browser and click **Install**.
   Or click **Create a new mod**, paste the contents of `siri-plus-launchpad.wh.cpp` from this repository, then **Compile** and **Enable**.
3. Press the Windows key.

---

## Support the project
If you enjoy Siri Plus Launchpad, you can support its development with a Pix donation:

**[Donate with LivePix (Pix)](https://livepix.gg/kurokobr)**

---

## Credits
Originally based on the **Fullscreen Start (Launchpad style)** mod (`fullscreen-start`, MIT License) by **Rafael Mendes** ([github.com/RafaelMendes1407](https://github.com/RafaelMendes1407)). The project has since been largely rewritten and extended.

Parts that still come from (or are adapted from) his code:
- **App list and icons**: enumerating the "All apps" list (programs and Store apps), loading each app's icon and trimming/centering it, skipping non-app entries, loading them on a background thread
- **Standalone Dock integration**: respecting the space the Dock reserves and keeping the Dock above the launcher
- **Fallback blur**: capturing the screen behind the launcher and blurring it (used when Windows' transparency effects are off)
- **Drawing base**: the layered window drawn with Direct2D (surface, render target and icon bitmaps)
- **Start menu interception**: the hook that opens the launcher instead of the Windows Start menu
- **Small helpers**: text lowercasing, name sorting, color parsing, timing

Everything else (Original Siri and Floating modes, live liquid glass, themes, app tabs, folders, Files, Clipboard, Actions, menus, animations, power options, shortcuts and translations) was written for this project.

Developed and maintained by **Thiago Silva** ([github.com/brideinflame-rgb](https://github.com/brideinflame-rgb)).

## License
Copyright (c) 2026 Thiago Silva. Licensed under the **GNU General Public License v3.0** (see `LICENSE`).

The parts that come from Rafael Mendes' *Fullscreen Start* are also available under their original license:

```
MIT License
Copyright (c) Rafael Mendes

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

## Changelog
### 1.43.3
- The category shortcuts belong to Original Siri: they moved to block ⑥ (Floating and Original Siri) and work only in that mode; block ⑦ is now just "Opening"
### 1.43.2
- Fixed: turning the Clipboard category off now really stops monitoring and clears the history
- Fixed: Original Siri no longer creates hidden folders when an app is dropped on another (tabs replace folders there; dragging to the taskbar still pins)
- Fixed: a possible crash when an app vanished while being drawn
- Code cleanup: one power menu definition, unused variables removed, a lost press on the tab strip no longer blocks hovering
### 1.43.1
- New README (everything the mod does and customizes, credits, support link)
- License: GPL-3.0 (parts from Fullscreen Start keep their MIT notice)
### 1.43.0
- Shortcuts: only the Ctrl shortcuts for the categories remain, and they are configurable (defaults Ctrl+1 Apps, Ctrl+2 Files, Ctrl+3 Actions, Ctrl+4 Clipboard); they work in every mode and appear in the categories menu
- Removed the global Win+key shortcuts (and their keyboard hook)
### 1.42.0
- Renamed to **Siri Plus Launchpad** (id `siri-plus-launchpad`, repository github.com/brideinflame-rgb/Siri-Plus-Launchpad)
- The former Siri mode is now called **Floating** (same settings value, nothing to reconfigure)
- Original Siri is the main mode and the default on install
### 1.41.3
- Clipboard is a list: each row shows the first line of what was copied and, below, a preview of the rest (or its size)
### 1.41.2
- Right-clicking the search bar no longer offers "New tab": it shows a text menu (cut, copy, paste, select all, delete all; only paste when empty)
- Original Siri: the tab options appear only inside the apps panel
### 1.41.1
- Original Siri: clicking the bar before picking a category offers the categories instead of opening Apps with no way back; typing right away picks Apps (the back arrow and Esc work)
### 1.41.0
- Original Siri: hovering the category icon in the bar turns it into a back arrow; clicking it goes back to "Search" like Esc
- Original Siri: clicking the empty bar next to the category reopens the categories menu to switch directly
### 1.40.1
- The selected filter and the page dots are more discreet: a faint capsule with a hairline edge instead of raised glass; the dots' capsule shows fully only when the mouse is near
### 1.40.0
- Original Siri: the built-in tabs can be deleted too (right-click > "Restore default tabs" brings them back), and apps can be added to or removed from them
- Original Siri: Tab / Shift+Tab walks the app tabs, or the results in Files, Clipboard and Actions
- Original Siri: the filter strip has no outline; hold and drag it sideways (its hidden side fades out)
### 1.39.0
- Original Siri: an "All" filter comes first (every app, with the recent row)
- Original Siri: your own tabs (up to 6) instead of folders - right-click an app or the strip for "New tab", type its name, then "Add app" opens every app: long-press to pick as many as you want (picked apps jiggle), Done saves
- Original Siri: right-click a tab to rename or delete it; inside a tab, "Remove from tab"; the filter strip scrolls sideways with the wheel when it is full
### 1.38.0
- Original Siri: the first row of apps shows the recently opened or installed apps (or the most used, or nothing - a setting), with a thin line under it
### 1.37.1
- Original Siri: Esc after picking a category goes back to "Search" with the magnifier and offers the categories again (arrows + Enter or the mouse)
### 1.37.0
- Original Siri: the bar says just "Search" when it opens; picking a category makes its icon and name glide from the menu into the bar
### 1.36.1
- Original Siri: the categories menu is larger and macOS-like (same glass as the bar, big icons, keycap shortcuts, aligned rows)
### 1.36.0
- New mode: Original Siri (macOS 27 style). Hovering the bar drops down Applications, Files, Actions and Clipboard with icons and Ctrl+1..4 shortcuts; the search works inside the chosen category
- Actions category: lock, sleep, restart, shut down, sign out, settings, task manager, file explorer, control panel, screenshot, empty recycle bin
- Applications: filters (Utilities, Productivity, Entertainment, Social, Creativity, Reading, Other) and the most used apps in the first row
### 1.35.1
- Siri power button: opens the same power options pill as typing "shut down"; with that pill turned off, it opens the power menu outside the results box, on its right
### 1.35.0
- Siri mode: optional floating power button next to the bar; it slides out from behind the bar once the bar is complete and opens the power menu
### 1.34.2
- Live blur rebuilt on Windows.UI.Composition: cut exactly to the shapes with smooth corners (no more blurred film around the menus), adjustable strength again
### 1.34.1
- Live blur: fixed the blurred rectangle that showed around the menus; the blur is now cut to the shapes
### 1.34.0
- Live blur: Windows' own blur under the menus, cut to their shapes, so animated wallpapers and videos keep playing behind a blurred glass (falls back to the blurred still image if Windows refuses it)
### 1.33.0
- One Background blur level (0-100) in Appearance for every theme and mode; 0 keeps the live background (replaces the Live background switch and the Custom theme blur)
### 1.32.0
- New Live background option (on by default): animated wallpapers and videos keep playing behind the menus in every mode and theme
- Window mode: the page indicator now sits inside the glass panel
### 1.31.0
- Raised liquid-glass buttons: power button, centered search box, clear button, category strip and its selected tab, page indicator, Siri power options and context menu rows
- Power menu translated in Window and Fullscreen modes
### 1.30.0
- Settings reorganized into short, clearly named blocks; a single Theme choice now applies to every mode
- Custom theme settings grouped in one block (one color/opacity for the window, the Siri bar and the panel)
- One corner rounding setting for window, fullscreen (now rounded too), open folders and the Siri panel
- Glass transparency now also applies to the Light and Siri themes
- Light and Siri themes: smaller, discreet shadow and soft edges like the Clear glass theme
- Removed settings that no longer changed anything useful (Liquid Glass toggle, separate bar/panel colors, separate radii)
- Note: settings were renamed, so they start from their default values once
### 1.29.1
- Clear glass theme: smaller, discreet shadow; soft gray edge (light at the top and bottom, darker in the middle, LiquidGlass style)
- New setting: Clear glass transparency (100 = completely clear)
### 1.29.0
- The bright Glassmorphism theme is now called Light
- New Clear glass theme: fully translucent glass with a thick-glass edge that casts a shadow on the wallpaper
### 1.28.0
- New Theme menu in each mode block: Custom (all color, blur, glass and border settings), Glassmorphism (bright translucent glass with a soft shadow) and Siri (macOS 27 gradient)
- Removed the Blur Mode menu: in the Custom theme, blur strength 0 turns blur off
- Siri mode now also blurs what is behind the pill and the panel
- Removed the "Siri wave without the circle" animation
### 1.26.0
- Removed the activation GIF feature and its settings (the circle zoom, bounce and "sucked in" closing remain)
### 1.25.2
- Siri mode closing: the bar collapses without zoom or bounce and is sucked into the bubble, which then shrinks and fades away
### 1.25.1
- Siri mode: the opening circle is no longer cut at the top
- The circle pops in with an adjustable zoom, returns to the bar height and then the bar expands (also on closing)
### 1.25.0
- Siri mode: optional activation GIF. Set a GIF file in the settings: it plays when the bar is invoked (black background removed automatically) and the bar then emerges from it with a fade
### 1.24.2
- Siri mode: restored the continuous dark-to-light gradient (darkest on the pill, lighter and slightly cool at the bottom of the panel), now visible on dark colors, with an adjustable intensity
### 1.24.1
- Files: duplicate paths are removed from the results and each file shows its folder under its name
### 1.24.0
- Siri mode: new animated "Siri wave" circle (colorful flowing blobs on a dark disk) for opening/closing; selectable in the settings (wave / soft glow / none)
### 1.23.0
- Siri mode: adjustable bounce for the circle opening/closing and for the Apps/Files/Clipboard box (0-100 %, 0 = none)
### 1.22.0
- Fix: the Clipboard (or Files) category could still be opened with Tab after being disabled in the settings
- Fix: category strip clicks now match the drawn labels (translated labels have different widths); the highlight slides again
- Fix: Ctrl+V pasted twice, Ctrl+Backspace did nothing, and plain Left/Right/Home/End no longer navigate the grid; Delete key added
- Fix: a stale text cursor position could crash after clearing the search or picking a history entry
- Smoother animations: frames follow the monitor refresh rate (high-resolution timer instead of the 15.6 ms WM_TIMER)
- More messages translated (Everything status, "New folder with N items")
- Code cleanup and changelog tidy-up
### 1.21.1
- Fixes the undeclared Direct2D factory identifier in the Files tab folder icon code
### 1.21.0
- Small folder icon in the Siri Files search tab
### 1.20.9
- Navigation direction is a General setting shared by all modes
- Vertical scrolling uses the clickable page indicator (no scrollbar)
### 1.20.1
- Siri Apps results: long-press to select, drag onto another app to create a folder, horizontal pages
### 1.19.1
- Siri mode skips the synchronous backdrop capture so opening stays responsive
### 1.19.0
- Faster, cancellable file scans and hardened Everything IPC parsing
### 1.18.1
- Mouse selection, caret navigation and selection-aware copy/cut/paste in the search box
### 1.18.0
- Ctrl+A / Ctrl+C / Ctrl+X in the search box
### 1.12.0
- Automatic interface language (Portuguese, English, Spanish, French, German, Italian)
### 1.11.0
- Direct shortcuts (must start with Win): Win+A Applications, Win+S Files, Win+D Clipboard (configurable)
### 1.10.1
- Files tab: empty search shows only the search history, with a trash icon to clear it
- Files shown as a list (new default, switchable in settings)
### 1.10.0
- Everything via IPC (WM_COPYDATA), no Everything64.dll needed
- New: options to disable Files search and Clipboard history
- Files tab shows the indexer error/status
### 1.9.0
- New: Apps / Files / Clipboard categories in Window, Fullscreen and Siri modes
- New: global file indexer setting (Everything, Windows Search, direct scan)
- New: asynchronous file search with debounce; configurable result and clipboard limits
### 1.8.0
- Settings reorganized into Mode, mode-specific and General blocks
- Animations are now configured per mode
- New: Siri closing animation (pill -> dot -> hide)
### 1.7.0
- Siri pill-only opening, intro animation, tabs, separate pill/panel settings
### 1.6.0
- Power pill in Siri mode
### 1.5.0
- Siri display mode
### 1.4.0
- Clear button and Uninstall
### 1.3.0
- Liquid Glass and jiggle
### 1.2.0
- Search inside folders
### 1.1.0
- Power button position
### 1.0.0
- First release
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayMode: siriOriginal
  $name: "① Mode"
  $name:pt-BR: "① Modo"
  $options:
  - siriOriginal: Original Siri (macOS 27 style menu, default)
  - siri: Floating (search bar)
  - window: Window
  - fullscreen: Fullscreen
  $options:pt-BR:
  - siriOriginal: Siri original (menu estilo macOS 27, padrão)
  - siri: Flutuante (barra de pesquisa)
  - window: Janela
  - fullscreen: Tela cheia
- theme: glass
  $name: "② Theme (all modes)"
  $name:pt-BR: "② Tema (todos os modos)"
  $options:
  - glass: Clear glass
  - light: Light
  - siri: Siri (macOS 27 gradient)
  - custom: Custom (block ④)
  $options:pt-BR:
  - glass: Vidro transparente
  - light: Claro
  - siri: Siri (degradê do macOS 27)
  - custom: Personalizado (bloco ④)
- appearance:
  - transparency: 80
    $name: Glass transparency (%)
    $name:pt-BR: "Transparência do vidro (%)"
    $description: Ready-made themes. 100 = completely clear; lower = more solid
    $description:pt-BR: "Temas prontos. 100 = totalmente transparente; menos = mais sólido"
  - cornerRadius: 26
    $name: Corner rounding (px)
    $name:pt-BR: "Arredondamento dos cantos (px)"
    $description: Window, fullscreen, open folders and the Siri panel
    $description:pt-BR: "Janela, tela cheia, pastas abertas e painel do Siri"
  - blur: 60
    $name: Background blur (0-100, 0 = none)
    $name:pt-BR: "Desfoque do fundo (0–100, 0 = nenhum)"
    $description: Live blur behind the menus (animated wallpapers and videos keep playing). Needs Windows' transparency effects; without them a blurred still image is used
    $description:pt-BR: "Desfoque ao vivo atrás dos menus (papéis de parede animados e vídeos continuam rodando). Precisa dos efeitos de transparência do Windows; sem eles, usa uma imagem parada desfocada"
  $name: "③ Appearance (all themes)"
  $name:pt-BR: "③ Aparência (todos os temas)"
- custom:
  - color: "#1e1e1e"
    $name: Color
    $name:pt-BR: "Cor"
  - opacity: 60
    $name: Opacity (%)
    $name:pt-BR: "Opacidade (%)"
  - shading: 70
    $name: "Floating / Original Siri: dark-to-light gradient (%, 0 = off)"
    $name:pt-BR: "Flutuante / Siri original: degradê escuro → claro (%, 0 = desligado)"
  - borderWidth: 1
    $name: Border width (px, 0 = none)
    $name:pt-BR: "Espessura da borda (px, 0 = nenhuma)"
  - borderColor: "#ffffff"
    $name: Border color
    $name:pt-BR: "Cor da borda"
  - borderOpacity: 18
    $name: Border opacity (%)
    $name:pt-BR: "Opacidade da borda (%)"
  $name: "④ Custom theme (only used when Theme = Custom)"
  $name:pt-BR: "④ Tema personalizado (só vale com o Tema = Personalizado)"
- window:
  - width: 80
    $name: "Window width (%, Window mode only)"
    $name:pt-BR: "Largura da janela (%, só modo Janela)"
  - height: 75
    $name: "Window height (%, Window mode only)"
    $name:pt-BR: "Altura da janela (%, só modo Janela)"
  - searchStyle: bar
    $name: Search style
    $name:pt-BR: "Estilo da pesquisa"
    $options:
    - bar: Top bar
    - box: Centered box (macOS)
    $options:pt-BR:
    - bar: Barra superior
    - box: Caixa centralizada (macOS)
  - showPowerButton: true
    $name: Show power button
    $name:pt-BR: "Mostrar botão de energia"
  - powerButtonPosition: left
    $name: Power button position
    $name:pt-BR: "Posição do botão de energia"
    $options:
    - left: Left of the search bar
    - right: Right of the search bar
    $options:pt-BR:
    - left: À esquerda da pesquisa
    - right: À direita da pesquisa
  - openAnimation: zoom
    $name: Opening animation
    $name:pt-BR: "Animação de abertura"
    $options:
    - zoom: Pop in
    - fade: Fade
    - slide: Rise from taskbar
    - crt: CRT
    $options:pt-BR:
    - zoom: Surgir
    - fade: Esmaecer
    - slide: Subir da barra de tarefas
    - crt: CRT
  - closeAnimation: zoom
    $name: Closing animation
    $name:pt-BR: "Animação de fechamento"
    $options:
    - zoom: Pop out
    - fade: Fade
    - slide: Sink into taskbar
    - crt: CRT
    $options:pt-BR:
    - zoom: Desaparecer
    - fade: Esmaecer
    - slide: Descer até a barra de tarefas
    - crt: CRT
  - animationSpeed: 100
    $name: Animation speed (%, 25-400)
    $name:pt-BR: "Velocidade das animações (%, 25–400)"
  - folderBlur: 40
    $name: Blur behind an open folder (0-100)
    $name:pt-BR: "Desfoque atrás da pasta aberta (0–100)"
  $name: "⑤ Window and Fullscreen modes"
  $name:pt-BR: "⑤ Modos Janela e Tela cheia"
- siri:
  - pillWidth: 560
    $name: Bar width (px)
    $name:pt-BR: "Largura da barra (px)"
  - pillHeight: 56
    $name: Bar height (px)
    $name:pt-BR: "Altura da barra (px)"
  - panelWidth: 680
    $name: Results box width (px)
    $name:pt-BR: "Largura da caixa de resultados (px)"
  - panelRows: 3
    $name: Results box rows
    $name:pt-BR: "Linhas da caixa de resultados"
  - orbStyle: wave
    $name: Circle animation
    $name:pt-BR: "Animação do círculo"
    $options:
    - wave: Siri wave (colorful)
    - glow: Soft glow
    - none: None
    $options:pt-BR:
    - wave: Onda Siri (colorida)
    - glow: Brilho suave
    - none: Nenhuma
  - circleZoom: 25
    $name: Circle zoom when opening (%, 0 = none)
    $name:pt-BR: "Zoom do círculo ao abrir (%, 0 = nenhum)"
  - bounce: 50
    $name: Circle bounce when opening (%, 0 = none)
    $name:pt-BR: "Bounce do círculo ao abrir (%, 0 = nenhum)"
  - panelBounce: 30
    $name: Results box bounce (%, 0 = none)
    $name:pt-BR: "Bounce da caixa de resultados (%, 0 = nenhum)"
  - introMs: 520
    $name: Opening duration (ms, 0 = instant)
    $name:pt-BR: "Duração da abertura (ms, 0 = instantânea)"
  - outroMs: 420
    $name: Closing duration (ms, 0 = instant)
    $name:pt-BR: "Duração do fechamento (ms, 0 = instantâneo)"
  - animationSpeed: 100
    $name: Animation speed (%, 25-400)
    $name:pt-BR: "Velocidade das animações (%, 25–400)"
  - powerPill: true
    $name: Power options when typing shut down, restart...
    $name:pt-BR: "Opções de energia ao digitar desligar, reiniciar..."
  - powerButton: true
    $name: Floating power button next to the bar
    $name:pt-BR: "Botão de energia flutuante ao lado da barra"
  - firstRow: recent
    $name: "Original Siri: first row of apps (separated by a line)"
    $name:pt-BR: "Siri original: primeira linha de aplicativos (separada por uma linha)"
    $options:
    - recent: Recently opened or installed
    - used: Most used
    - off: "Off"
    $options:pt-BR:
    - recent: Abertos ou instalados recentemente
    - used: Mais usados
    - off: Desligada
  - shortcutApps: Ctrl+1
    $name: "Original Siri: shortcut - Applications"
    $name:pt-BR: "Siri original: atalho — Aplicativos"
    $description: "With the launcher open, jumps to this category (shown next to it in the categories menu). Must start with Ctrl; empty = none. Examples - Ctrl+1, Ctrl+Shift+F, Ctrl+F2 (Ctrl+A/C/V/X stay for the text)"
    $description:pt-BR: "Com o menu aberto, vai direto para esta categoria (aparece ao lado dela no menu de categorias). Deve começar com Ctrl; vazio = nenhum. Exemplos: Ctrl+1, Ctrl+Shift+F, Ctrl+F2 (Ctrl+A/C/V/X ficam para o texto)"
  - shortcutFiles: Ctrl+2
    $name: "Original Siri: shortcut - Files"
    $name:pt-BR: "Siri original: atalho — Arquivos"
  - shortcutActions: Ctrl+3
    $name: "Original Siri: shortcut - Actions"
    $name:pt-BR: "Siri original: atalho — Ações"
  - shortcutClipboard: Ctrl+4
    $name: "Original Siri: shortcut - Clipboard"
    $name:pt-BR: "Siri original: atalho — Área de transferência"
  $name: "⑥ Floating and Original Siri modes"
  $name:pt-BR: "⑥ Modos Flutuante e Siri original"
- open:
  - winKey: true
    $name: Open with the Windows key
    $name:pt-BR: "Abrir com a tecla Windows"
  - startButton: true
    $name: Open with the Start button
    $name:pt-BR: "Abrir com o botão Iniciar"
  $name: "⑦ Opening"
  $name:pt-BR: "⑦ Abertura"
- grid:
  - iconSize: 64
    $name: Icon size (px, Floating / Original Siri up to 56)
    $name:pt-BR: "Tamanho dos ícones (px; nos modos Flutuante e Siri original até 56)"
  - columns: 0
    $name: Columns (0 = automatic)
    $name:pt-BR: "Colunas (0 = automático)"
  - pageMode: vertical
    $name: Scrolling direction
    $name:pt-BR: "Direção da rolagem"
    $options:
    - vertical: Vertical
    - horizontal: Horizontal (pages)
    $options:pt-BR:
    - vertical: Vertical
    - horizontal: Horizontal (páginas)
  - jiggle: true
    $name: Jiggle selected icons
    $name:pt-BR: "Ícones selecionados tremem"
  - dragToPin: true
    $name: Drag an icon to the taskbar to pin it
    $name:pt-BR: "Arrastar o ícone até a barra de tarefas fixa o app"
  $name: "⑧ Apps and icons"
  $name:pt-BR: "⑧ Aplicativos e ícones"
- files:
  - enabled: true
    $name: Files category
    $name:pt-BR: "Categoria Arquivos"
  - history: true
    $name: Remember searches
    $name:pt-BR: "Lembrar pesquisas"
    $description: Shown when the Files search is empty
    $description:pt-BR: "Aparecem quando a pesquisa de Arquivos está vazia"
  - view: list
    $name: Results view
    $name:pt-BR: "Exibição dos resultados"
    $options:
    - list: List
    - icons: Icons (grid)
    $options:pt-BR:
    - list: Lista
    - icons: Ícones (grade)
  - indexer: auto
    $name: File indexer
    $name:pt-BR: "Indexador de arquivos"
    $description: Everything must be running (no DLL needed, don't run it as admin)
    $description:pt-BR: "O Everything precisa estar aberto (não requer DLL; não execute como administrador)"
    $options:
    - auto: Automatic (Everything > Windows Search > Scan)
    - everything: Everything
    - windows: Windows Search
    - scan: Direct scan (slow)
    $options:pt-BR:
    - auto: Automático (Everything > Pesquisa do Windows > Verificação)
    - everything: Everything
    - windows: Pesquisa do Windows
    - scan: Verificação direta (lenta)
  - maxResults: 50
    $name: Maximum results
    $name:pt-BR: "Máximo de resultados"
  $name: "⑨ Files"
  $name:pt-BR: "⑨ Arquivos"
- clipboard:
  - enabled: true
    $name: Clipboard category
    $name:pt-BR: "Categoria Área de transferência"
    $description: When off, the clipboard is not monitored and the history is cleared
    $description:pt-BR: "Desligada, a área de transferência não é monitorada e o histórico é apagado"
  - size: 30
    $name: Items kept
    $name:pt-BR: "Itens guardados"
  $name: "⑩ Clipboard"
  $name:pt-BR: "⑩ Área de transferência"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <propsys.h>
#include <dwmapi.h>
#include <inspectable.h>
#include <winstring.h>
#undef DrawText
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Microsoft::WRL::ComPtr;
extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace {

// Automatic UI localization based on the current Windows display/UI language.
// Supported languages: Portuguese, English, Spanish, French, German and Italian.
// Unknown languages fall back to English.
enum class UiLanguage { Portuguese, English, Spanish, French, German, Italian };

UiLanguage DetectUiLanguage() {
    LANGID lang = GetUserDefaultUILanguage();
    switch (PRIMARYLANGID(lang)) {
        case LANG_PORTUGUESE: return UiLanguage::Portuguese;
        case LANG_SPANISH: return UiLanguage::Spanish;
        case LANG_FRENCH: return UiLanguage::French;
        case LANG_GERMAN: return UiLanguage::German;
        case LANG_ITALIAN: return UiLanguage::Italian;
        default: return UiLanguage::English;
    }
}

UiLanguage g_uiLanguage = DetectUiLanguage();

struct UiTranslation {
    PCWSTR source;
    PCWSTR pt;
    PCWSTR es;
    PCWSTR fr;
    PCWSTR de;
    PCWSTR it;
};

const UiTranslation kUiTranslations[] = {
    {L"Apps", L"Aplicativos", L"Aplicaciones", L"Applications", L"Apps", L"App"},
    {L"Files", L"Arquivos", L"Archivos", L"Fichiers", L"Dateien", L"File"},
    {L"Clipboard", L"Área de transferência", L"Portapapeles", L"Presse-papiers", L"Zwischenablage", L"Appunti"},
    {L"Search Files", L"Pesquisar arquivos", L"Buscar archivos", L"Rechercher des fichiers", L"Dateien suchen", L"Cerca file"},
    {L"Search Clipboard", L"Pesquisar área de transferência", L"Buscar en portapapeles", L"Rechercher dans le presse-papiers", L"Zwischenablage durchsuchen", L"Cerca negli appunti"},
    {L"Search Apps", L"Pesquisar aplicativos", L"Buscar aplicaciones", L"Rechercher des applications", L"Apps suchen", L"Cerca app"},
    {L"Search Applications", L"Pesquisar aplicativos", L"Buscar aplicaciones", L"Rechercher des applications", L"Anwendungen suchen", L"Cerca applicazioni"},
    {L"Loading apps...", L"Carregando aplicativos...", L"Cargando aplicaciones...", L"Chargement des applications...", L"Apps werden geladen...", L"Caricamento app..."},
    {L"No apps", L"Nenhum aplicativo", L"No hay aplicaciones", L"Aucune application", L"Keine Apps", L"Nessuna app"},
    {L"No apps found", L"Nenhum aplicativo encontrado", L"No se encontraron aplicaciones", L"Aucune application trouvée", L"Keine Apps gefunden", L"Nessuna app trovata"},
    {L"No files found", L"Nenhum arquivo encontrado", L"No se encontraron archivos", L"Aucun fichier trouvé", L"Keine Dateien gefunden", L"Nessun file trovato"},
    {L"No recent searches", L"Nenhuma pesquisa recente", L"No hay búsquedas recientes", L"Aucune recherche récente", L"Keine letzten Suchanfragen", L"Nessuna ricerca recente"},
    {L"Recent searches", L"Pesquisas recentes", L"Búsquedas recientes", L"Recherches récentes", L"Letzte Suchanfragen", L"Ricerche recenti"},
    {L"Type to search files", L"Digite para pesquisar arquivos", L"Escribe para buscar archivos", L"Tapez pour rechercher des fichiers", L"Dateien suchen: Eingabe", L"Digita per cercare file"},
    {L"Type at least 2 characters", L"Digite pelo menos 2 caracteres", L"Escribe al menos 2 caracteres", L"Saisissez au moins 2 caractères", L"Mindestens 2 Zeichen eingeben", L"Inserisci almeno 2 caratteri"},
    {L"Searching...", L"Pesquisando...", L"Buscando...", L"Recherche en cours...", L"Suche läuft...", L"Ricerca..."},
    {L"Copy some text and it will show up here", L"Copie algum texto para vê-lo aqui", L"Copia texto para verlo aquí", L"Copiez du texte pour le voir ici", L"Kopieren Sie Text, damit er hier erscheint", L"Copia del testo per visualizzarlo qui"},
    {L"Nothing found", L"Nada encontrado", L"No se encontró nada", L"Aucun résultat", L"Nichts gefunden", L"Nessun risultato"},
    {L"Lock", L"Bloquear", L"Bloquear", L"Verrouiller", L"Sperren", L"Blocca"},
    {L"Sleep", L"Suspender", L"Suspender", L"Mettre en veille", L"Energie sparen", L"Sospendi"},
    {L"Restart", L"Reiniciar", L"Reiniciar", L"Redémarrer", L"Neu starten", L"Riavvia"},
    {L"Shut down", L"Desligar", L"Apagar", L"Arrêter", L"Herunterfahren", L"Arresta"},
    {L"Sign out", L"Sair", L"Cerrar sesión", L"Se déconnecter", L"Abmelden", L"Esci"},
    {L"New Folder", L"Nova pasta", L"Nueva carpeta", L"Nouveau dossier", L"Neuer Ordner", L"Nuova cartella"},
    {L"New folder", L"Nova pasta", L"Nueva carpeta", L"Nouveau dossier", L"Neuer Ordner", L"Nuova cartella"},
    {L"New folder with this app", L"Nova pasta com este aplicativo", L"Nueva carpeta con esta aplicación", L"Nouveau dossier avec cette application", L"Neuer Ordner mit dieser App", L"Nuova cartella con questa app"},
    {L"Select", L"Selecionar", L"Seleccionar", L"Sélectionner", L"Auswählen", L"Seleziona"},
    {L"Deselect", L"Desmarcar", L"Deseleccionar", L"Désélectionner", L"Abwählen", L"Deseleziona"},
    {L"Select all", L"Selecionar tudo", L"Seleccionar todo", L"Tout sélectionner", L"Alle auswählen", L"Seleziona tutto"},
    {L"Clear selection", L"Limpar seleção", L"Borrar selección", L"Effacer la sélection", L"Auswahl aufheben", L"Annulla selezione"},
    {L"Rename", L"Renomear", L"Renombrar", L"Renommer", L"Umbenennen", L"Rinomina"},
    {L"Undo folder", L"Desfazer pasta", L"Deshacer carpeta", L"Annuler le dossier", L"Ordner rückgängig", L"Annulla cartella"},
    {L"Pin to taskbar", L"Fixar na barra de tarefas", L"Anclar a la barra de tareas", L"Épingler à la barre des tâches", L"An Taskleiste anheften", L"Aggiungi alla barra delle applicazioni"},
    {L"Pin selected to taskbar", L"Fixar selecionados na barra de tarefas", L"Anclar selección a la barra de tareas", L"Épingler la sélection à la barre des tâches", L"Auswahl an Taskleiste anheften", L"Aggiungi selezionati alla barra delle applicazioni"},
    {L"Change icon...", L"Alterar ícone...", L"Cambiar icono...", L"Changer l’icône...", L"Symbol ändern...", L"Cambia icona..."},
    {L"Reset icon", L"Redefinir ícone", L"Restablecer icono", L"Réinitialiser l’icône", L"Symbol zurücksetzen", L"Reimposta icona"},
    {L"Uninstall", L"Desinstalar", L"Desinstalar", L"Désinstaller", L"Deinstallieren", L"Disinstalla"},
    {L"Remove from history", L"Remover do histórico", L"Quitar del historial", L"Retirer de l’historique", L"Aus Verlauf entfernen", L"Rimuovi dalla cronologia"},
    {L"Clear search history", L"Limpar histórico de pesquisa", L"Borrar historial de búsqueda", L"Effacer l’historique de recherche", L"Suchverlauf löschen", L"Cancella cronologia ricerche"},
    {L"Could not load this image.", L"Não foi possível carregar esta imagem.", L"No se pudo cargar esta imagen.", L"Impossible de charger cette image.", L"Dieses Bild konnte nicht geladen werden.", L"Impossibile caricare questa immagine."},
    {L"Choose icon image", L"Escolher imagem do ícone", L"Elegir imagen del icono", L"Choisir l’image de l’icône", L"Symbolbild auswählen", L"Scegli immagine icona"},
    {L"Images", L"Imagens", L"Imágenes", L"Images", L"Bilder", L"Immagini"},
    {L"All files", L"Todos os arquivos", L"Todos los archivos", L"Tous les fichiers", L"Alle Dateien", L"Tutti i file"},
    {L"Everything is not running", L"O Everything não está em execução", L"Everything no se está ejecutando", L"Everything n’est pas en cours d’exécution", L"Everything läuft nicht", L"Everything non è in esecuzione"},
    {L"Actions", L"Ações", L"Acciones", L"Actions", L"Aktionen", L"Azioni"},
    {L"Search", L"Pesquisar", L"Buscar", L"Rechercher", L"Suchen", L"Cerca"},
    {L"Search Actions", L"Pesquisar ações", L"Buscar acciones", L"Rechercher des actions", L"Aktionen suchen", L"Cerca azioni"},
    {L"No actions found", L"Nenhuma ação encontrada", L"No se encontraron acciones", L"Aucune action trouvée", L"Keine Aktionen gefunden", L"Nessuna azione trovata"},
    {L"Utilities", L"Utilitários", L"Utilidades", L"Utilitaires", L"Dienstprogramme", L"Utilità"},
    {L"Productivity", L"Produtividade", L"Productividad", L"Productivité", L"Produktivität", L"Produttività"},
    {L"Entertainment", L"Entretenimento", L"Entretenimiento", L"Divertissement", L"Unterhaltung", L"Intrattenimento"},
    {L"Social", L"Social", L"Social", L"Social", L"Sozial", L"Social"},
    {L"Creativity", L"Criatividade", L"Creatividad", L"Créativité", L"Kreativität", L"Creatività"},
    {L"Reading", L"Leitura", L"Lectura", L"Lecture", L"Lesen", L"Lettura"},
    {L"Other", L"Outros", L"Otros", L"Autres", L"Andere", L"Altro"},
    {L"Settings", L"Configurações", L"Configuración", L"Paramètres", L"Einstellungen", L"Impostazioni"},
    {L"Task Manager", L"Gerenciador de Tarefas", L"Administrador de tareas", L"Gestionnaire des tâches", L"Task-Manager", L"Gestione attività"},
    {L"File Explorer", L"Explorador de Arquivos", L"Explorador de archivos", L"Explorateur de fichiers", L"Datei-Explorer", L"Esplora file"},
    {L"Control Panel", L"Painel de Controle", L"Panel de control", L"Panneau de configuration", L"Systemsteuerung", L"Pannello di controllo"},
    {L"Screenshot", L"Capturar tela", L"Captura de pantalla", L"Capture d’écran", L"Bildschirmausschnitt", L"Cattura schermata"},
    {L"Empty Recycle Bin", L"Esvaziar Lixeira", L"Vaciar Papelera de reciclaje", L"Vider la corbeille", L"Papierkorb leeren", L"Svuota cestino"},
    {L"New folder with %d items", L"Nova pasta com %d itens", L"Nueva carpeta con %d elementos", L"Nouveau dossier avec %d éléments", L"Neuer Ordner mit %d Elementen", L"Nuova cartella con %d elementi"},
    {L"Everything is running as admin (run it without admin)", L"O Everything está em execução como administrador (execute-o sem privilégios de administrador)", L"Everything se ejecuta como administrador (ejecútalo sin privilegios de administrador)", L"Everything s’exécute en administrateur (lancez-le sans privilèges d’administrateur)", L"Everything läuft als Administrator (ohne Administratorrechte starten)", L"Everything è in esecuzione come amministratore (avvialo senza privilegi di amministratore)"},
    {L"Everything did not accept the query", L"O Everything não aceitou a consulta", L"Everything no aceptó la consulta", L"Everything n’a pas accepté la requête", L"Everything hat die Anfrage nicht akzeptiert", L"Everything non ha accettato la query"},
    {L"Everything rejected the query", L"O Everything rejeitou a consulta", L"Everything rechazó la consulta", L"Everything a rejeté la requête", L"Everything hat die Anfrage abgelehnt", L"Everything ha rifiutato la query"},
    {L"Everything is still indexing...", L"O Everything ainda está indexando...", L"Everything todavía está indexando...", L"Everything est encore en cours d’indexation...", L"Everything indiziert noch...", L"Everything sta ancora indicizzando..."},
    {L"All", L"Todos", L"Todas", L"Toutes", L"Alle", L"Tutte"},
    {L"Cancel", L"Cancelar", L"Cancelar", L"Annuler", L"Abbrechen", L"Annulla"},
    {L"Done (%d)", L"Concluir (%d)", L"Listo (%d)", L"Terminé (%d)", L"Fertig (%d)", L"Fine (%d)"},
    {L"New tab", L"Nova aba", L"Nueva pestaña", L"Nouvel onglet", L"Neuer Tab", L"Nuova scheda"},
    {L"New tab with this app", L"Nova aba com este aplicativo", L"Nueva pestaña con esta aplicación", L"Nouvel onglet avec cette application", L"Neuer Tab mit dieser App", L"Nuova scheda con questa app"},
    {L"Add app", L"Adicionar aplicativo", L"Añadir aplicación", L"Ajouter une application", L"App hinzufügen", L"Aggiungi app"},
    {L"Rename tab", L"Renomear aba", L"Renombrar pestaña", L"Renommer l’onglet", L"Tab umbenennen", L"Rinomina scheda"},
    {L"Delete tab", L"Excluir aba", L"Eliminar pestaña", L"Supprimer l’onglet", L"Tab löschen", L"Elimina scheda"},
    {L"Remove from tab", L"Remover da aba", L"Quitar de la pestaña", L"Retirer de l’onglet", L"Aus Tab entfernen", L"Rimuovi dalla scheda"},
    {L"Add to \u201C%ls\u201D", L"Adicionar a \u201C%ls\u201D", L"Añadir a \u201C%ls\u201D", L"Ajouter à \u00AB %ls \u00BB", L"Zu \u201E%ls\u201C hinzufügen", L"Aggiungi a \u201C%ls\u201D"},
    {L"%d characters", L"%d caracteres", L"%d caracteres", L"%d caractères", L"%d Zeichen", L"%d caratteri"},
    {L"Cut", L"Recortar", L"Cortar", L"Couper", L"Ausschneiden", L"Taglia"},
    {L"Copy", L"Copiar", L"Copiar", L"Copier", L"Kopieren", L"Copia"},
    {L"Paste", L"Colar", L"Pegar", L"Coller", L"Einfügen", L"Incolla"},
    {L"Delete all", L"Apagar tudo", L"Borrar todo", L"Tout effacer", L"Alles löschen", L"Cancella tutto"},
    {L"Restore default tabs", L"Restaurar abas padrão", L"Restaurar pestañas predeterminadas", L"Restaurer les onglets par défaut", L"Standard-Tabs wiederherstellen", L"Ripristina schede predefinite"},
    {L"Hold an app to select it", L"Segure um aplicativo para selecioná-lo", L"Mantén pulsada una aplicación para elegirla", L"Maintenez une application pour la choisir", L"App gedrückt halten, um sie auszuwählen", L"Tieni premuta un’app per sceglierla"},
    {L"Everything did not reply", L"O Everything não respondeu", L"Everything no respondió", L"Everything n’a pas répondu", L"Everything hat nicht geantwortet", L"Everything non ha risposto"},
};

PCWSTR Tr(PCWSTR source);
PCWSTR TabLabel(int id) { return Tr(id == 0 ? L"Apps" : id == 1 ? L"Files" : id == 3 ? L"Actions" : L"Clipboard"); }

PCWSTR Tr(PCWSTR source) {
    for (const auto& item : kUiTranslations) {
        if (wcscmp(source, item.source) == 0) {
            switch (g_uiLanguage) {
                case UiLanguage::Portuguese: return item.pt;
                case UiLanguage::Spanish: return item.es;
                case UiLanguage::French: return item.fr;
                case UiLanguage::German: return item.de;
                case UiLanguage::Italian: return item.it;
                default: return item.source;
            }
        }
    }
    return source;
}

constexpr WCHAR kWindowClass[] = L"WindhawkMacLaunchpad";
constexpr WCHAR kAppsFolder[] = L"shell:AppsFolder\\";
constexpr WCHAR kFoldersKey[] = L"folders";
constexpr WCHAR kCustomTabsKey[] = L"customTabs";
constexpr WCHAR kBuiltinTabsKey[] = L"builtinTabs";
constexpr WCHAR kIconsKey[] = L"customIcons";

constexpr UINT WM_APP_TOGGLE = WM_APP + 1;
constexpr UINT WM_APP_APPS = WM_APP + 2;
constexpr UINT WM_APP_RELOAD = WM_APP + 3;
constexpr UINT WM_APP_QUIT = WM_APP + 4;
constexpr UINT WM_APP_FILES = WM_APP + 5;

constexpr UINT_PTR kTimerFrame = 1;
constexpr UINT_PTR kTimerFirstLoad = 4;
constexpr UINT_PTR kTimerFileSearch = 5;
constexpr UINT_PTR kTimerLongPress = 6;
constexpr UINT kFileSearchDelayMs = 220;
constexpr UINT kLongPressDelayMs = 550;

constexpr float kFolderOpenSeconds = 0.34f;
constexpr float kFolderCloseSeconds = 0.24f;

// g_layoutItems encoding (main screen):
//   0..999                     -> index in g_apps
//   1000..999999               -> folder (1000 + folder index)
//   kFolderAppBase..kTabBase-1 -> app inside a folder (search result)
//   >= kTabBase                -> category item (Files / Clipboard): kTabBase + index in g_tabItems
constexpr int kFolderAppBase = 1000000;
constexpr int kTabBase = 900000000;

enum { kMenuRename = 1, kMenuUndoFolder, kMenuNewFolder, kMenuSelect, kMenuSelectAll, kMenuClearSelection, kMenuPin,
       kMenuChangeIcon, kMenuResetIcon, kMenuUninstall,
       kMenuLock, kMenuSleep, kMenuRestart, kMenuShutdown, kMenuSignOut,
       kMenuHistRemove, kMenuHistClear, kMenuTabApps, kMenuTabFiles, kMenuTabClipboard, kMenuTabActions,
       kMenuNewTab, kMenuAddApps, kMenuRenameTab, kMenuDeleteTab, kMenuRemoveFromTab, kMenuRestoreTabs,
       kMenuTextCut, kMenuTextCopy, kMenuTextPaste, kMenuTextDeleteAll, kMenuTextSelectAll };
constexpr int kMenuAddToTab = 300;   // + user tab index

constexpr GUID kBhidEnumItems = {0x94f60519, 0x2850, 0x4924, {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};
constexpr GUID kBhidSFUIObject = {0x3981e225, 0xf559, 0x11d3, {0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5}};
constexpr GUID kClsidGaussianBlur = {0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

double Now() {
    static LARGE_INTEGER frequency = [] { LARGE_INTEGER f; QueryPerformanceFrequency(&f); return f; }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / frequency.QuadPart;
}

std::wstring ToLower(std::wstring text) {
    if (!text.empty()) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, text.data(), (int)text.size(), text.data(), (int)text.size(), nullptr, nullptr, 0);
    }
    return text;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class Theme { Custom, Light, Glass, Siri };
enum class Anim { Zoom, Fade, Slide, Crt };
enum class Indexer { Auto, Everything, Windows, Scan };

Anim ParseAnim(const std::wstring& v) { return v == L"fade" ? Anim::Fade : v == L"slide" ? Anim::Slide : v == L"crt" ? Anim::Crt : Anim::Zoom; }
float AnimSeconds(Anim a, bool closing) {
    switch (a) {
        case Anim::Fade:  return closing ? 0.18f : 0.24f;
        case Anim::Slide: return closing ? 0.26f : 0.36f;
        case Anim::Crt:   return closing ? 0.40f : 0.44f;
        default:          return closing ? 0.16f : 0.30f;
    }
}

struct Settings {
    Theme theme = Theme::Custom;       // Window / Fullscreen
    Theme siriTheme = Theme::Custom;   // Siri
    int blurAmount = 60;
    int glassTransparency = 80;   // Clear glass theme
    bool liveBackground = true;   // no snapshot: animated wallpapers and videos keep playing behind
    float opacity = 0.6f;
    D2D1_COLOR_F tint{0.118f, 0.118f, 0.118f, 1.0f};
    int windowWidth = 80;
    int windowHeight = 75;
    int iconSize = 64;
    int columns = 0;
    bool fullscreen = false;
    bool searchBox = false;
    int cornerRadius = 28;
    int folderRadius = 26;
    int folderBlur = 40;
    float animSpeed = 1.0f;
    int borderWidth = 1;
    D2D1_COLOR_F borderColor{1.0f, 1.0f, 1.0f, 1.0f};
    float borderOpacity = 0.18f;
    bool horizontal = false;
    bool dragToPin = true;
    bool showPower = true;
    bool powerRight = false;
    bool liquidGlass = true;
    bool jiggle = true;
    bool darkText = false;
    bool siri = false;
    bool siriOriginal = false;   // Original Siri: hover menu, Actions, app filters, recommendations
    int siriFirstRow = 1;   // 0 off, 1 recently opened/installed, 2 most used
    int siriPillW = 560;
    int siriPillH = 56;
    D2D1_COLOR_F siriPillColor{0.118f, 0.118f, 0.118f, 1.0f};
    float siriPillOpacity = 0.72f;
    int siriPillIntroMs = 520;
    int siriPillOutroMs = 420;
    int siriBounce = 50;
    bool siriPowerButton = true;
    int siriCircleZoom = 25;
    int siriOrbStyle = 0;   // 0 wave, 1 glow, 2 none
    int siriPanelBounce = 30;
    int siriPanelW = 680;
    int siriRows = 3;
    bool siriHorizontal = true;
    D2D1_COLOR_F siriPanelColor{0.118f, 0.118f, 0.118f, 1.0f};
    float siriPanelOpacity = 0.60f;
    int siriPanelRadius = 24;
    bool siriShade = true;
    int siriShadeStrength = 70;
    bool siriPower = true;
    Anim openAnim = Anim::Zoom, closeAnim = Anim::Zoom;
    bool filesEnabled = true;
    bool clipEnabled = true;
    Indexer indexer = Indexer::Auto;
    int maxResults = 50;
    int clipMax = 30;
};

Settings g_settings;
std::atomic<bool> g_winKeyEnabled{true};
std::atomic<bool> g_startButtonEnabled{true};
[[clang::no_destroy]] std::vector<std::wstring> g_searchHistory;
bool g_historyEnabled = true;
bool g_filesList = false;

struct Shortcut { bool valid = false, shift = false; UINT vk = 0; };
Shortcut g_shortcuts[4];   // category shortcuts (Ctrl+...), indexed by tab id: Apps, Files, Clipboard, Actions
[[clang::no_destroy]] std::wstring g_shortcutLabels[4];   // as shown in the categories menu
void SaveSearchHistory();
void QueryChanged();


// Visible categories: 0 Apps (always), 1 Files, 2 Clipboard, 3 Actions (Original Siri)
int g_tabIds[4] = {0, 1, 2, 3};
int g_tabCount = 3;

void BuildTabs() {
    g_tabCount = 0;
    g_tabIds[g_tabCount++] = 0;
    if (g_settings.filesEnabled) g_tabIds[g_tabCount++] = 1;
    if (g_settings.siriOriginal) g_tabIds[g_tabCount++] = 3;
    if (g_settings.clipEnabled) g_tabIds[g_tabCount++] = 2;
}
bool TabsVisible() { return g_tabCount > 1; }
bool TabEnabled(int id) { for (int i = 0; i < g_tabCount; i++) if (g_tabIds[i] == id) return true; return false; }
int TabSlot(int id) { for (int i = 0; i < g_tabCount; i++) if (g_tabIds[i] == id) return i; return 0; }

std::wstring GetStringSetting(PCWSTR name) { return WindhawkUtils::StringSetting::make(name).get(); }

D2D1_COLOR_F ParseColor(std::wstring text, D2D1_COLOR_F fallback) {
    text.erase(std::remove_if(text.begin(), text.end(), iswspace), text.end());
    if (!text.empty() && text[0] == L'#') text.erase(0, 1);
    if (text.size() != 6 || !std::all_of(text.begin(), text.end(), iswxdigit)) return fallback;
    UINT rgb = wcstoul(text.c_str(), nullptr, 16);
    return D2D1::ColorF(((rgb >> 16) & 0xFF) / 255.0f, ((rgb >> 8) & 0xFF) / 255.0f, (rgb & 0xFF) / 255.0f);
}

float Luma(const D2D1_COLOR_F& c) { return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b; }

// "Ctrl+1", "Ctrl+Shift+F", "Ctrl+F5" ... must start with Ctrl (Ctrl+A/C/V/X are kept for editing the text)
Shortcut ParseShortcut(const std::wstring& text, std::wstring& label) {
    Shortcut sc; bool ctrl = false, key = false;
    std::wstring keyName;
    label.clear();
    size_t pos = 0;
    while (pos <= text.size()) {
        size_t end = text.find(L'+', pos);
        if (end == std::wstring::npos) end = text.size();
        std::wstring part = text.substr(pos, end - pos);
        pos = end + 1;
        part.erase(0, part.find_first_not_of(L' '));
        part.erase(part.find_last_not_of(L' ') + 1);
        std::wstring p = ToLower(part);
        if (p.empty()) continue;
        if (p == L"ctrl" || p == L"control") { ctrl = true; continue; }
        if (p == L"shift") { sc.shift = true; continue; }
        if (key) return Shortcut{};
        wchar_t* stop = nullptr;
        long n = p.size() > 1 && p[0] == L'f' ? wcstol(p.c_str() + 1, &stop, 10) : 0;
        if (p.size() == 1 && p[0] >= L'a' && p[0] <= L'z') sc.vk = (UINT)(p[0] - L'a' + L'A');
        else if (p.size() == 1 && p[0] >= L'0' && p[0] <= L'9') sc.vk = (UINT)p[0];
        else if (n >= 1 && n <= 24 && stop && *stop == 0) sc.vk = (UINT)(VK_F1 + n - 1);
        else return Shortcut{};   // Win, Alt and other keys are not accepted
        keyName = sc.vk >= VK_F1 && sc.vk <= VK_F24 ? L"F" + std::to_wstring(sc.vk - VK_F1 + 1) : std::wstring(1, (wchar_t)sc.vk);
        key = true;
    }
    if (!ctrl || !key || (!sc.shift && (sc.vk == 'A' || sc.vk == 'C' || sc.vk == 'V' || sc.vk == 'X'))) return Shortcut{};
    sc.valid = true;
    label = std::wstring(L"Ctrl+") + (sc.shift ? L"Shift+" : L"") + keyName;
    return sc;
}

void LoadSettings() {
    Settings s;
    auto I = [](PCWSTR k) { return Wh_GetIntSetting(k); };
    auto S = [](PCWSTR k) { return GetStringSetting(k); };

    // ① Mode
    std::wstring mode = S(L"displayMode");

    s.fullscreen = mode == L"fullscreen";
    // Original Siri is the main mode (also used when the setting is missing or unknown)
    s.siriOriginal = mode != L"window" && mode != L"fullscreen" && mode != L"siri";
    s.siri = mode == L"siri" || s.siriOriginal;

    // ② Theme (one for every mode)
    std::wstring theme = S(L"theme");
    s.theme = theme == L"light" ? Theme::Light : theme == L"siri" ? Theme::Siri : theme == L"custom" ? Theme::Custom : Theme::Glass;
    s.siriTheme = s.theme;

    // ③ Appearance
    s.glassTransparency = std::clamp(I(L"appearance.transparency"), 0, 100);
    s.cornerRadius = std::clamp(I(L"appearance.cornerRadius"), 0, 60);
    s.blurAmount = std::clamp(I(L"appearance.blur"), 0, 100);
    s.liveBackground = s.blurAmount == 0;
    s.folderRadius = s.siriPanelRadius = s.cornerRadius;

    // ④ Custom theme: one color/opacity for the window, the Siri bar and the Siri panel
    s.tint = ParseColor(S(L"custom.color"), s.tint);
    s.siriPillColor = s.siriPanelColor = s.tint;
    s.opacity = s.siriPanelOpacity = std::clamp(I(L"custom.opacity"), 0, 100) / 100.0f;
    s.siriPillOpacity = std::min(1.0f, s.opacity + 0.12f);
    s.siriShadeStrength = std::clamp(I(L"custom.shading"), 0, 100);
    s.siriShade = s.siriShadeStrength > 0;
    s.liquidGlass = true;
    s.borderWidth = std::clamp(I(L"custom.borderWidth"), 0, 8);
    s.borderColor = ParseColor(S(L"custom.borderColor"), s.borderColor);
    s.borderOpacity = std::clamp(I(L"custom.borderOpacity"), 0, 100) / 100.0f;

    // ⑤ Window / Fullscreen
    s.windowWidth = std::clamp(I(L"window.width"), 30, 100);
    s.windowHeight = std::clamp(I(L"window.height"), 30, 100);
    s.searchBox = S(L"window.searchStyle") == L"box";
    s.showPower = I(L"window.showPowerButton") != 0;
    s.powerRight = S(L"window.powerButtonPosition") == L"right";
    s.openAnim = ParseAnim(S(L"window.openAnimation"));
    s.closeAnim = ParseAnim(S(L"window.closeAnimation"));
    s.folderBlur = std::clamp(I(L"window.folderBlur"), 0, 100);

    // ⑥ Siri
    s.siriPillW = std::clamp(I(L"siri.pillWidth"), 200, 1400);
    s.siriPillH = std::clamp(I(L"siri.pillHeight"), 36, 96);
    s.siriPanelW = std::clamp(I(L"siri.panelWidth"), 300, 1400);
    s.siriRows = std::clamp(I(L"siri.panelRows"), 1, 8);
    std::wstring orb = S(L"siri.orbStyle");
    s.siriOrbStyle = orb == L"none" ? 2 : orb == L"glow" ? 1 : 0;
    s.siriCircleZoom = std::clamp(I(L"siri.circleZoom"), 0, 60);
    s.siriBounce = std::clamp(I(L"siri.bounce"), 0, 100);
    s.siriPanelBounce = std::clamp(I(L"siri.panelBounce"), 0, 100);
    s.siriPillIntroMs = std::clamp(I(L"siri.introMs"), 0, 2000);
    s.siriPillOutroMs = std::clamp(I(L"siri.outroMs"), 0, 2000);
    s.siriPower = I(L"siri.powerPill") != 0;
    s.siriPowerButton = I(L"siri.powerButton") != 0;
    std::wstring firstRow = S(L"siri.firstRow");
    s.siriFirstRow = firstRow == L"off" ? 0 : firstRow == L"used" ? 2 : 1;
    s.animSpeed = std::clamp(I(s.siri ? L"siri.animationSpeed" : L"window.animationSpeed"), 25, 400) / 100.0f;

    // ⑧ Apps and icons
    s.iconSize = std::clamp(I(L"grid.iconSize"), 24, 192);
    s.columns = std::clamp(I(L"grid.columns"), 0, 30);
    s.horizontal = s.siriHorizontal = S(L"grid.pageMode") == L"horizontal";
    s.jiggle = I(L"grid.jiggle") != 0;
    s.dragToPin = I(L"grid.dragToPin") != 0;

    // ⑨ Files / ⑩ Clipboard
    s.filesEnabled = I(L"files.enabled") != 0;
    std::wstring ix = S(L"files.indexer");
    s.indexer = ix == L"everything" ? Indexer::Everything
              : ix == L"windows"    ? Indexer::Windows
              : ix == L"scan"       ? Indexer::Scan : Indexer::Auto;
    s.maxResults = std::clamp(I(L"files.maxResults"), 10, 500);
    s.clipEnabled = I(L"clipboard.enabled") != 0;
    s.clipMax = std::clamp(I(L"clipboard.size"), 5, 200);

    Theme active = s.siri ? s.siriTheme : s.theme;
    s.darkText = active == Theme::Light ? true : active != Theme::Custom ? false : Luma(s.siri ? s.siriPanelColor : s.tint) > 0.6f;
    g_settings = s;
    BuildTabs();
    g_winKeyEnabled = I(L"open.winKey") != 0;
    g_startButtonEnabled = I(L"open.startButton") != 0;
    g_historyEnabled = I(L"files.history") != 0;
    g_filesList = S(L"files.view") == L"list";
    g_shortcuts[0] = ParseShortcut(S(L"siri.shortcutApps"), g_shortcutLabels[0]);
    g_shortcuts[1] = ParseShortcut(S(L"siri.shortcutFiles"), g_shortcutLabels[1]);
    g_shortcuts[2] = ParseShortcut(S(L"siri.shortcutClipboard"), g_shortcutLabels[2]);
    g_shortcuts[3] = ParseShortcut(S(L"siri.shortcutActions"), g_shortcutLabels[3]);
    if (!g_historyEnabled) { g_searchHistory.clear(); SaveSearchHistory(); }
}


////////////////////////////////////////////////////////////////////////////////
// App list & Folders

struct App {
    std::wstring name;
    std::wstring target;
    std::vector<UINT32> pixels;
    int iconSize = 0;
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
    int kind = 0; // 0 app, 1 file, 2 clipboard text, 3 search history, 4 action
};

struct Folder {
    std::wstring name = L"Folder";
    std::wstring sortKey;
    std::vector<std::shared_ptr<App>> apps;
};

using AppList = std::vector<std::shared_ptr<App>>;
[[clang::no_destroy]] AppList g_apps;
[[clang::no_destroy]] AppList g_tabItems;
[[clang::no_destroy]] std::vector<std::wstring> g_clipHistory;
constexpr WCHAR kHistoryKey[] = L"fileHistory";
constexpr int kHistoryMax = 20;
[[clang::no_destroy]] std::vector<Folder> g_folders;
[[clang::no_destroy]] std::vector<int> g_layoutItems;

bool g_appsLoaded;
double g_appsTime;
int g_appsIconPixels;
HANDLE g_worker;
std::atomic<bool> g_workerStop;
std::atomic<HWND> g_wnd;

void FinishIcon(std::vector<UINT32> pixels, int width, int height, App& app) {
    if (std::none_of(pixels.begin(), pixels.end(), [](UINT32 p) { return (p >> 24) != 0; })) {
        for (auto& p : pixels) p |= 0xFF000000;
    }
    bool premultiplied = std::none_of(pixels.begin(), pixels.end(), [](UINT32 p) {
        UINT a = p >> 24;
        return ((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a || (p & 0xFF) > a;
    });
    if (!premultiplied) {
        for (auto& p : pixels) {
            UINT a = p >> 24, r = ((p >> 16) & 0xFF) * a / 255, g = ((p >> 8) & 0xFF) * a / 255, b = (p & 0xFF) * a / 255;
            p = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    int left = width, top = height, right = -1, bottom = -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((pixels[y * width + x] >> 24) > 8) {
                left = std::min(left, x); right = std::max(right, x); top = std::min(top, y); bottom = std::max(bottom, y);
            }
        }
    }
    if (right < 0) return;
    int full = std::max(width, height), content = std::max(right - left + 1, bottom - top + 1);
    int side, cx, cy;
    if (content < full * 0.6f) {
        side = std::min(full, (int)std::ceil(content * 1.08f));
        cx = (left + right + 1) / 2; cy = (top + bottom + 1) / 2;
    } else { side = full; cx = width / 2; cy = height / 2; }
    app.iconSize = side;
    app.pixels.assign((size_t)side * side, 0);
    int x0 = cx - side / 2, y0 = cy - side / 2;
    for (int y = 0; y < side; y++) {
        int sy = y0 + y;
        if (sy < 0 || sy >= height) continue;
        for (int x = 0; x < side; x++) {
            int sx = x0 + x;
            if (sx >= 0 && sx < width) app.pixels[(size_t)y * side + x] = pixels[sy * width + sx];
        }
    }
}

void LoadAppIcon(IShellItem* item, int size, App& app) {
    ComPtr<IShellItemImageFactory> factory;
    HBITMAP bitmap = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&factory))) || FAILED(factory->GetImage(SIZE{size, size}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bitmap))) return;
    BITMAP bm{};
    if (GetObjectW(bitmap, sizeof(bm), &bm) && bm.bmWidth > 0) {
        int width = bm.bmWidth, height = std::abs(bm.bmHeight);
        std::vector<UINT32> pixels((size_t)width * height);
        BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = width; bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
        HDC dc = GetDC(nullptr);
        int lines = GetDIBits(dc, bitmap, 0, height, pixels.data(), &bi, DIB_RGB_COLORS);
        ReleaseDC(nullptr, dc);
        if (lines) FinishIcon(std::move(pixels), width, height, app);
    }
    DeleteObject(bitmap);
}

bool ShouldSkip(const std::wstring& name, const std::wstring& parsing) {
    std::wstring lowerName = ToLower(name), lowerTarget = ToLower(parsing);
    if (lowerName.empty() || lowerTarget.find(L"\\unins0") != std::wstring::npos || lowerTarget.find(L"\\uninst.exe") != std::wstring::npos || lowerTarget.find(L"\\uninstall.exe") != std::wstring::npos || lowerTarget.rfind(L"http", 0) == 0) return true;
    for (PCWSTR ext : {L".chm", L".txt", L".pdf", L".rtf", L".htm", L".html"}) {
        if (lowerTarget.size() >= wcslen(ext) && lowerTarget.compare(lowerTarget.size() - wcslen(ext), wcslen(ext), ext) == 0) return true;
    }
    return false;
}

bool NameLess(const std::wstring& a, const std::wstring& b) {
    return CompareStringEx(LOCALE_NAME_USER_DEFAULT, LINGUISTIC_IGNORECASE | SORT_DIGITSASNUMBERS, a.c_str(), -1, b.c_str(), -1, nullptr, nullptr, 0) == CSTR_LESS_THAN;
}

AppList* EnumerateApps(int iconPixels) {
    auto list = std::make_unique<AppList>();
    ComPtr<IShellItem> folder; ComPtr<IEnumShellItems> items;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&folder))) || FAILED(folder->BindToHandler(nullptr, kBhidEnumItems, IID_PPV_ARGS(&items)))) return list.release();
    ComPtr<IShellItem> item;
    while (!g_workerStop && items->Next(1, item.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
        PWSTR name = nullptr, parsing = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) && SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &parsing)) && !ShouldSkip(name, parsing)) {
            auto app = std::make_shared<App>();
            app->name = name; app->target = std::wstring(kAppsFolder) + parsing;
            LoadAppIcon(item.Get(), iconPixels, *app);
            list->push_back(std::move(app));
        }
        CoTaskMemFree(name); CoTaskMemFree(parsing);
    }
    std::stable_sort(list->begin(), list->end(), [](const auto& a, const auto& b) { return NameLess(a->name, b->name); });
    return list.release();
}

DWORD WINAPI LoadAppsThread(void* param) {
    int iconPixels = (int)(INT_PTR)param;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    AppList* list = EnumerateApps(iconPixels);
    CoUninitialize();
    HWND wnd = g_wnd;
    if (g_workerStop || !wnd || !PostMessageW(wnd, WM_APP_APPS, iconPixels, (LPARAM)list)) delete list;
    return 0;
}

int IconPixels() {
    UINT maxDpi = 96;
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
        UINT dpiX = 96, dpiY = 96;
        if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) *(UINT*)param = std::max(*(UINT*)param, dpiX);
        return TRUE;
    }, (LPARAM)&maxDpi);
    return std::min(256, (int)std::ceil(g_settings.iconSize * maxDpi / 96.0));
}

void StartLoadingApps() {
    if (g_worker) {
        if (WaitForSingleObject(g_worker, 0) == WAIT_TIMEOUT) return;
        CloseHandle(g_worker); g_worker = nullptr;
    }
    g_appsTime = Now();
    g_worker = CreateThread(nullptr, 0, LoadAppsThread, (void*)(INT_PTR)IconPixels(), 0, nullptr);
}

std::wstring g_query;
size_t g_queryCaret = 0;
size_t g_queryAnchor = 0;
bool g_queryDragging = false;
size_t QuerySelStart() { return std::min(g_queryCaret, g_queryAnchor); }
size_t QuerySelEnd() { return std::max(g_queryCaret, g_queryAnchor); }
bool QueryHasSelection() { return QuerySelStart() != QuerySelEnd(); }
void ClampQueryCaret() {
    g_queryCaret = std::min(g_queryCaret, g_query.size());
    g_queryAnchor = std::min(g_queryAnchor, g_query.size());
}
void ClearQuery() { g_query.clear(); g_queryCaret = g_queryAnchor = 0; }
void SetQuery(const std::wstring& text) { g_query = text; g_queryCaret = g_queryAnchor = g_query.size(); }
void DeleteQuerySelection() {
    ClampQueryCaret();
    size_t a = QuerySelStart(), b = QuerySelEnd();
    if (a < b && b <= g_query.size()) g_query.erase(a, b - a);
    g_queryCaret = g_queryAnchor = a;

}
int g_openFolderIndex = -1;

// Category state (all modes)
int g_siriTab;            // 0 Apps, 1 Files, 2 Clipboard
float g_tabSlide;         // animated tab indicator (visible slot)
float g_tabNatural[4];
[[clang::no_destroy]] std::unordered_map<std::wstring, float> g_chipWidths;   // measured filter labels (cleared when the scale changes)
float g_chipPad;   // padding around a filter label
bool g_searchActive;      // Window/Fullscreen: category strip visible (search bar clicked or typing)
float g_tabsAnim;         // Window/Fullscreen: 0 hidden -> 1 strip fully shown

// Siri mode state
bool g_siriExpanded;      // results panel wanted
float g_siriAnim;         // 0 = pill only, 1 = panel fully open
float g_siriBtnAnim;      // floating power button: 0 = hidden behind the bar, 1 = out on the right
float g_pillIntro = 1.0f; // 0 -> 1: circle -> pill
bool g_siriClosingPill;   // closing: panel collapses, then pill -> dot -> hide

bool IsTabItem(int it) { return it >= kTabBase; }
bool IsFolderApp(int it) { return it >= kFolderAppBase && it < kTabBase; }
bool IsFolderItem(int it) { return it >= 1000 && it < kFolderAppBase; }

App* ResolveApp(int it) {
    if (it < 0) return nullptr;
    if (IsTabItem(it)) { int i = it - kTabBase; return i < (int)g_tabItems.size() ? g_tabItems[i].get() : nullptr; }
    if (it < 1000) return it < (int)g_apps.size() ? g_apps[it].get() : nullptr;
    if (!IsFolderApp(it)) return nullptr;
    int f = (it - kFolderAppBase) / 10000, a = (it - kFolderAppBase) % 10000;
    if (f >= (int)g_folders.size() || a >= (int)g_folders[f].apps.size()) return nullptr;
    return g_folders[f].apps[a].get();
}

// ---------- Original Siri: most used apps and app categories ----------
std::vector<std::wstring> Split(const std::wstring& s, WCHAR sep);
int g_recommendedCount = 7;   // apps per row in the Siri panel (set by the layout)
constexpr WCHAR kLaunchKey[] = L"launchCounts";
[[clang::no_destroy]] std::unordered_map<std::wstring, int> g_launchCounts;   // launches through the mod, by lowercase target
[[clang::no_destroy]] std::unordered_map<std::wstring, int> g_userAssist;     // Windows' own run counts (UserAssist)
[[clang::no_destroy]] std::unordered_map<std::wstring, unsigned long long> g_userAssistTime;   // ... and when they last ran (FILETIME)
[[clang::no_destroy]] std::unordered_map<std::wstring, unsigned long long> g_launchTimes;      // last launch through the mod
[[clang::no_destroy]] std::unordered_map<std::wstring, unsigned long long> g_firstSeen;         // when an app first showed up (installed)
constexpr WCHAR kKnownAppsKey[] = L"knownApps";
int g_firstRowCount;   // items in the Original Siri first row (a line is drawn under it)

unsigned long long NowFileTime() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
}
int g_appFilter = -1;   // Original Siri: app category filter (-1 = all)

// Original Siri: user tabs, shown after the built-in filters (each keeps the apps the user picked)
struct CustomTab { std::wstring name; std::vector<std::wstring> targets; };
[[clang::no_destroy]] std::vector<CustomTab> g_customTabs;
constexpr int kMaxCustomTabs = 6;
constexpr int kChipAll = -1, kChipCustom = 100, kChipCancel = -100, kChipDone = -101, kNoChip = -1000;
[[clang::no_destroy]] std::unordered_set<App*> g_selection;   // multi-select (Original Siri: the apps picked for a tab)
int g_pickerChip = kNoChip;  // the tab whose apps are being picked
bool Picking() { return g_pickerChip != kNoChip; }
int g_pickerPrevFilter = -1;
int g_chipRename = -1;       // the user tab whose name is being typed
float g_chipScroll;          // the strip scrolls sideways when the chips don't fit
int g_menuChip = kNoChip;    // the tab a context menu acts on
bool g_chipPress, g_chipDragged;   // the strip is held / dragged sideways with the mouse
int g_chipPressed = kNoChip;
float g_chipPressX, g_chipPressScroll;
bool g_siriChosen;          // Original Siri: a category was picked (the bar shows it instead of "Search")
float g_flyAnim = 1.0f;     // the picked menu row flying into the bar (0 -> 1)
D2D1_RECT_F g_flyGlyph{}, g_flyText{};   // where the picked row's icon and label started

void LoadLaunchCounts() {
    std::vector<WCHAR> buf(1 << 15, 0);
    Wh_GetStringValue(kLaunchKey, buf.data(), buf.size());
    buf.back() = 0;
    g_launchCounts.clear();
    g_launchTimes.clear();
    for (auto& line : Split(buf.data(), L'\n')) {
        // "count<TAB>target" or "count<TAB>last launch<TAB>target"
        auto parts = Split(line, L'\t');
        if (parts.size() == 2) g_launchCounts[parts[1]] = _wtoi(parts[0].c_str());
        if (parts.size() == 3) {
            g_launchCounts[parts[2]] = _wtoi(parts[0].c_str());
            g_launchTimes[parts[2]] = _wcstoui64(parts[1].c_str(), nullptr, 10);
        }
    }
    std::vector<WCHAR> known(1 << 16, 0);
    Wh_GetStringValue(kKnownAppsKey, known.data(), known.size());
    known.back() = 0;
    g_firstSeen.clear();
    for (auto& line : Split(known.data(), L'\n')) {
        size_t tab = line.find(L'\t');
        if (tab != std::wstring::npos) g_firstSeen[line.substr(tab + 1)] = _wcstoui64(line.substr(0, tab).c_str(), nullptr, 10);
    }
}

// Apps never seen before are "recently installed" (the first time, every app is just remembered)
void UpdateKnownApps(const AppList& apps) {
    bool first = g_firstSeen.empty(), changed = false;
    unsigned long long now = NowFileTime();
    for (auto& app : apps) {
        std::wstring target = ToLower(app->target);
        if (g_firstSeen.count(target)) continue;
        g_firstSeen[target] = first ? 0 : now;
        changed = true;
    }
    if (!changed) return;
    std::wstring data;
    for (auto& [target, time] : g_firstSeen) {
        if (data.size() > 60000) break;
        data += std::to_wstring(time) + L"\t" + target + L"\n";
    }
    Wh_SetStringValue(kKnownAppsKey, data.c_str());
}

void SaveLaunchCounts() {
    std::vector<std::pair<int, std::wstring>> top;
    for (auto& [target, count] : g_launchCounts) top.push_back({count, target});
    std::sort(top.begin(), top.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    if (top.size() > 60) top.resize(60);
    std::wstring data;
    for (auto& [count, target] : top) data += std::to_wstring(count) + L"\t" + std::to_wstring(g_launchTimes[target]) + L"\t" + target + L"\n";
    Wh_SetStringValue(kLaunchKey, data.c_str());
}

// Windows keeps how often each program was run (names are ROT13-encoded; the count is at offset 4)
void LoadUserAssist() {
    g_userAssist.clear();
    g_userAssistTime.clear();
    for (PCWSTR guid : {L"{CEBFF5CD-ACE2-4F4F-9178-9926F41749EA}", L"{F4E57C4B-2036-45F0-A9AB-443BCFE33D9F}"}) {
        std::wstring path = std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\\") + guid + L"\\Count";
        HKEY key;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, KEY_READ, &key) != ERROR_SUCCESS) continue;
        for (DWORD i = 0;; i++) {
            WCHAR name[1024];
            BYTE data[128];
            DWORD nameLen = 1024, dataLen = sizeof(data), type = 0;
            LONG r = RegEnumValueW(key, i, name, &nameLen, nullptr, &type, data, &dataLen);
            if (r == ERROR_NO_MORE_ITEMS) break;
            if (r != ERROR_SUCCESS || type != REG_BINARY || dataLen < 8) continue;
            for (DWORD c = 0; c < nameLen; c++) {
                WCHAR& ch = name[c];
                if (ch >= L'a' && ch <= L'z') ch = L'a' + (ch - L'a' + 13) % 26;
                else if (ch >= L'A' && ch <= L'Z') ch = L'A' + (ch - L'A' + 13) % 26;
            }
            DWORD count = 0;
            memcpy(&count, data + 4, sizeof(count));
            if (!count) continue;
            std::wstring decoded = ToLower(std::wstring(name, nameLen));
            unsigned long long lastRun = 0;
            if (dataLen >= 68) memcpy(&lastRun, data + 60, sizeof(lastRun));   // last run time (FILETIME)
            g_userAssist[decoded] += (int)count;
            g_userAssistTime[decoded] = std::max(g_userAssistTime[decoded], lastRun);
            size_t slash = decoded.find_last_of(L'\\');
            if (slash != std::wstring::npos) {
                g_userAssist[decoded.substr(slash + 1)] += (int)count;
                g_userAssistTime[decoded.substr(slash + 1)] = std::max(g_userAssistTime[decoded.substr(slash + 1)], lastRun);
            }
        }
        RegCloseKey(key);
    }
}

int UsageScore(const App& a) {
    std::wstring target = ToLower(a.target);
    int score = 0;
    if (auto it = g_launchCounts.find(target); it != g_launchCounts.end()) score += it->second * 3;
    if (auto it = g_userAssist.find(target); it != g_userAssist.end()) return score + it->second;
    size_t slash = target.find_last_of(L'\\');
    if (slash != std::wstring::npos)
        if (auto it = g_userAssist.find(target.substr(slash + 1)); it != g_userAssist.end()) return score + it->second;
    if (auto it = g_userAssist.find(ToLower(a.name) + L".lnk"); it != g_userAssist.end()) score += it->second;
    return score;
}

// The most recent of: launched through the mod, run (Windows' record), first seen (installed)
unsigned long long RecentTime(const App& a) {
    std::wstring target = ToLower(a.target);
    unsigned long long t = 0;
    if (auto it = g_launchTimes.find(target); it != g_launchTimes.end()) t = std::max(t, it->second);
    if (auto it = g_firstSeen.find(target); it != g_firstSeen.end()) t = std::max(t, it->second);
    size_t slash = target.find_last_of(L'\\');
    for (const std::wstring& key : {target, slash == std::wstring::npos ? target : target.substr(slash + 1), ToLower(a.name) + L".lnk"})
        if (auto it = g_userAssistTime.find(key); it != g_userAssistTime.end()) t = std::max(t, it->second);
    return t;
}

void CountLaunch(const App& a) {
    if (a.kind != 0) return;
    g_launchCounts[ToLower(a.target)]++;
    g_launchTimes[ToLower(a.target)] = NowFileTime();
    SaveLaunchCounts();
}

// App categories for the Original Siri filters, guessed from the app's name (a heuristic)
constexpr int kFilterCount = 7;   // Utilities, Productivity, Entertainment, Social, Creativity, Reading, Other
PCWSTR FilterLabel(int i) {
    static const PCWSTR kLabels[kFilterCount] = {L"Utilities", L"Productivity", L"Entertainment", L"Social", L"Creativity", L"Reading", L"Other"};
    return Tr(kLabels[std::clamp(i, 0, kFilterCount - 1)]);
}

bool HasWord(const std::wstring& text, const std::wstring& word) {
    for (size_t at = text.find(word); at != std::wstring::npos; at = text.find(word, at + 1)) {
        bool before = at == 0 || !iswalnum(text[at - 1]);
        bool after = at + word.size() >= text.size() || !iswalnum(text[at + word.size()]);
        if (word.size() > 3 || (before && after)) return true;
    }
    return false;
}

int AppCategory(const App& a) {
    static const std::vector<std::pair<int, std::vector<std::wstring>>> kRules = {
        {3, {L"whatsapp", L"telegram", L"discord", L"instagram", L"facebook", L"messenger", L"twitter", L"tiktok", L"linkedin",
             L"skype", L"signal", L"reddit", L"snapchat", L"threads", L"wechat", L"teams"}},
        {2, {L"spotify", L"netflix", L"prime video", L"disney", L"hbo", L"youtube", L"twitch", L"steam", L"epic games", L"game", L"jogo",
             L"xbox", L"playstation", L"bluestacks", L"emulat", L"vlc", L"media player", L"music", L"música", L"movies", L"filmes",
             L"kodi", L"plex", L"deezer", L"roblox", L"minecraft", L"league of", L"valorant", L"ubisoft", L"ea app", L"battle.net",
             L"riot", L"gog", L"tv", L"death stranding", L"anchor"}},
        {4, {L"photoshop", L"illustrator", L"premiere", L"after effects", L"lightroom", L"indesign", L"figma", L"canva", L"blender",
             L"gimp", L"inkscape", L"krita", L"davinci", L"capcut", L"obs", L"audacity", L"fl studio", L"ableton", L"photos", L"fotos",
             L"clipchamp", L"paint 3d", L"sketch", L"autocad", L"unity", L"unreal", L"cinema 4d", L"affinity", L"audition", L"camera", L"câmera"}},
        {5, {L"edge", L"chrome", L"firefox", L"opera", L"brave", L"browser", L"navegador", L"kindle", L"reader", L"leitor", L"news",
             L"notícias", L"weather", L"clima", L"maps", L"mapas", L"wikipedia", L"books", L"livros", L"bible", L"bíblia",
             L"dictionary", L"dicionário", L"translat", L"tradutor", L"chatgpt", L"copilot", L"gemini", L"claude", L"perplexity"}},
        {1, {L"word", L"excel", L"powerpoint", L"outlook", L"onenote", L"access", L"publisher", L"office", L"acrobat", L"pdf",
             L"notion", L"to do", L"todo", L"calendar", L"calendário", L"mail", L"zoom", L"slack", L"trello", L"evernote", L"bank",
             L"banco", L"financ", L"money", L"project", L"visio", L"code", L"studio", L"intellij", L"pycharm", L"git", L"docker",
             L"sql", L"python", L"java", L"node", L"clínica", L"clinic", L"agenda"}},
        {0, {L"calc", L"notepad", L"bloco de notas", L"settings", L"configura", L"terminal", L"powershell", L"cmd", L"prompt",
             L"explorer", L"explorador", L"task manager", L"gerenciador", L"control", L"painel", L"paint", L"snipping", L"captura",
             L"recorte", L"clock", L"relógio", L"alarm", L"7-zip", L"winrar", L"everything", L"windhawk", L"defrag", L"desfragment",
             L"backup", L"cleanup", L"limpeza", L"disk", L"disco", L"device", L"dispositivo", L"printer", L"impressora", L"remote",
             L"remota", L"anydesk", L"teamviewer", L"vpn", L"bluetooth", L"store", L"loja", L"diagnós", L"diagnos", L"calibr",
             L"altsnap", L"utility", L"tools", L"ferrament", L"assistência", L"assistance", L"dolby", L"driver", L"update"}},
    };
    std::wstring text = ToLower(a.name);
    for (const auto& [category, words] : kRules)
        for (const auto& word : words)
            if (HasWord(text, word)) return category;
    return 6;
}

// Built-in tabs can be deleted (hidden) and the user can add or take apps out of them
struct BuiltinTab { bool hidden = false; std::vector<std::wstring> added, removed; };
[[clang::no_destroy]] BuiltinTab g_builtinTabs[kFilterCount];

bool ListHas(const std::vector<std::wstring>& v, const std::wstring& t) { return std::find(v.begin(), v.end(), t) != v.end(); }

bool InBuiltin(int i, const App& a) {
    std::wstring t = ToLower(a.target);
    if (ListHas(g_builtinTabs[i].removed, t)) return false;
    return ListHas(g_builtinTabs[i].added, t) || AppCategory(a) == i;
}

bool TabHas(int tab, const App& a) {
    if (tab < 0 || tab >= (int)g_customTabs.size()) return false;
    const auto& v = g_customTabs[tab].targets;
    return std::find(v.begin(), v.end(), ToLower(a.target)) != v.end();
}

bool ChipHas(int chip, const App& a) {
    if (chip >= kChipCustom) return TabHas(chip - kChipCustom, a);
    if (chip >= 0 && chip < kFilterCount) return InBuiltin(chip, a);
    return chip == kChipAll;
}

bool ChipEditable(int chip) {
    return (chip >= 0 && chip < kFilterCount) || (chip >= kChipCustom && chip - kChipCustom < (int)g_customTabs.size());
}

void BuildMainItems(std::vector<int>& out) {
    out.clear();
    std::wstring q = ToLower(g_query);
    if (g_siriTab != 0) {
        bool indexed = g_siriTab == 1 && !g_query.empty(); // already filtered by the indexer
        for (int i = 0; i < (int)g_tabItems.size(); i++) {
            const App& a = *g_tabItems[i];
            if (indexed || ToLower(a.kind == 2 ? a.target : a.name).find(q) != std::wstring::npos) out.push_back(kTabBase + i);
        }
        return;
    }
    if (!g_query.empty() || g_settings.siri) {
        bool picking = g_settings.siriOriginal && Picking();
        bool filter = g_settings.siriOriginal && g_appFilter >= 0 && !picking;
        auto keep = [&](const App& a) {
            return !filter || ChipHas(g_appFilter, a);
        };
        size_t prefix = 0;
        g_firstRowCount = 0;
        if (g_settings.siriOriginal && g_settings.siriFirstRow && g_query.empty() && !filter && !picking) {
            // First row: recently opened/installed apps, or the most used ones; a line separates it from the rest
            std::vector<std::pair<unsigned long long, int>> scored;
            for (int i = 0; i < (int)g_apps.size() && i < 1000; i++) {
                unsigned long long score = g_settings.siriFirstRow == 1 ? RecentTime(*g_apps[i]) : (unsigned long long)UsageScore(*g_apps[i]);
                if (score > 0) scored.push_back({score, i});
            }
            std::stable_sort(scored.begin(), scored.end(), [](const auto& x, const auto& y) { return x.first > y.first; });
            int row = std::max(1, g_recommendedCount);
            for (int k = 0; k < (int)scored.size() && k < row; k++) out.push_back(scored[k].second);
            if (!out.empty()) {
                while ((int)out.size() < row) out.push_back(-1);   // empty slots keep the row whole
                g_firstRowCount = row;
            }
            prefix = out.size();
        }
        for (int i = 0; i < (int)g_apps.size() && i < 1000; i++)
            if (ToLower(g_apps[i]->name).find(q) != std::wstring::npos && keep(*g_apps[i])) out.push_back(i);
        for (int f = 0; f < (int)g_folders.size(); f++)
            for (int a = 0; a < (int)g_folders[f].apps.size() && a < 10000; a++)
                if (ToLower(g_folders[f].apps[a]->name).find(q) != std::wstring::npos && keep(*g_folders[f].apps[a]))
                    out.push_back(kFolderAppBase + f * 10000 + a);
        std::stable_sort(out.begin() + prefix, out.end(), [](int x, int y) { return NameLess(ResolveApp(x)->name, ResolveApp(y)->name); });
        return;
    }
    std::vector<int> folderOrder;
    for (int j = 0; j < (int)g_folders.size(); j++) folderOrder.push_back(j);
    std::stable_sort(folderOrder.begin(), folderOrder.end(), [](int a, int b) { return NameLess(g_folders[a].sortKey, g_folders[b].sortKey); });
    size_t next = 0;
    for (int i = 0; i < (int)g_apps.size(); i++) {
        while (next < folderOrder.size() && !NameLess(g_apps[i]->name, g_folders[folderOrder[next]].sortKey)) {
            out.push_back(1000 + folderOrder[next++]);
        }
        out.push_back(i);
    }
    while (next < folderOrder.size()) out.push_back(1000 + folderOrder[next++]);
}

void UndoFolder(int index) {
    if (index < 0 || index >= (int)g_folders.size()) return;
    for (const auto& app : g_folders[index].apps) g_apps.push_back(app);
    g_folders.erase(g_folders.begin() + index);
    std::stable_sort(g_apps.begin(), g_apps.end(), [](const auto& a, const auto& b) { return NameLess(a->name, b->name); });
}

////////////////////////////////////////////////////////////////////////////////
// Clipboard helpers

std::wstring ReadClipboardText(size_t maxLen) {
    std::wstring text;
    bool open = false;
    for (int i = 0; i < 5 && !(open = OpenClipboard(g_wnd) != 0); i++) Sleep(10);
    if (!open) return text;
    if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
        if (auto* p = (PCWSTR)GlobalLock(data)) { text = p; GlobalUnlock(data); }
    }
    CloseClipboard();
    if (text.size() > maxLen) text.resize(maxLen);
    return text;
}

void CopyText(const std::wstring& text) {
    if (!OpenClipboard(g_wnd)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(WCHAR);
    if (HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes)) {
        if (void* p = GlobalLock(mem)) {
            memcpy(p, text.c_str(), bytes);
            GlobalUnlock(mem);
            if (!SetClipboardData(CF_UNICODETEXT, mem)) GlobalFree(mem);
        } else GlobalFree(mem);
    }
    CloseClipboard();
}

std::wstring ClipSnippet(const std::wstring& text);
// Clipboard list rows: the first line of what was copied, then a preview of the rest (or its size)
void ClipRowText(const std::wstring& text, std::wstring& first, std::wstring& rest) {
    size_t start = text.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) { first.clear(); rest.clear(); return; }
    size_t end = text.find_first_of(L"\r\n", start);
    first = ClipSnippet(text.substr(start, end == std::wstring::npos ? std::wstring::npos : end - start));
    rest = end == std::wstring::npos ? std::wstring() : ClipSnippet(text.substr(end, 400));
    if (rest.empty()) {
        wchar_t size[64];
        swprintf(size, 64, Tr(L"%d characters"), (int)text.size());
        rest = size;
    }
}

std::wstring ClipSnippet(const std::wstring& text) {
    std::wstring out;
    for (WCHAR c : text) {
        if (c == L'\r' || c == L'\n' || c == L'\t') c = L' ';
        if (c == L' ' && (out.empty() || out.back() == L' ')) continue;
        out += c;
        if (out.size() >= 80) break;
    }
    while (!out.empty() && out.back() == L' ') out.pop_back();
    return out;
}

////////////////////////////////////////////////////////////////////////////////
// Folder persistence

std::vector<std::wstring> Split(const std::wstring& s, WCHAR sep) {
    std::vector<std::wstring> parts;
    size_t start = 0;
    for (;;) {
        size_t pos = s.find(sep, start);
        if (pos == std::wstring::npos) { parts.push_back(s.substr(start)); break; }
        parts.push_back(s.substr(start, pos - start));
        start = pos + 1;
    }
    return parts;
}

std::wstring Clean(std::wstring s) {
    for (auto& c : s) if (c == L'\t' || c == L'\n' || c == L'\r') c = L' ';
    return s;
}

void SaveFolders() {
    std::wstring data;
    for (const auto& f : g_folders) {
        data += Clean(f.name); data += L'\t'; data += Clean(f.sortKey);
        for (const auto& a : f.apps) { data += L'\t'; data += Clean(a->target); }
        data += L'\n';
    }
    Wh_SetStringValue(kFoldersKey, data.c_str());
}

void LoadFolders() {
    std::vector<WCHAR> buf(1 << 18, 0);
    Wh_GetStringValue(kFoldersKey, buf.data(), buf.size());
    buf.back() = 0;
    g_folders.clear();
    for (const auto& line : Split(buf.data(), L'\n')) {
        auto fields = Split(line, L'\t');
        if (fields.size() < 2) continue;
        Folder folder;
        folder.name = fields[0]; folder.sortKey = fields[1];
        for (size_t i = 2; i < fields.size(); i++) {
            if (fields[i].empty()) continue;
            auto app = std::make_shared<App>();
            app->name = app->target = fields[i];
            folder.apps.push_back(app);
        }
        g_folders.push_back(std::move(folder));
    }
}

void SaveCustomTabs() {
    std::wstring data;
    for (const auto& t : g_customTabs) {
        data += Clean(t.name);
        for (const auto& target : t.targets) { data += L'\t'; data += Clean(target); }
        data += L'\n';
    }
    Wh_SetStringValue(kCustomTabsKey, data.c_str());
}

void LoadCustomTabs() {
    std::vector<WCHAR> buf(1 << 17, 0);
    Wh_GetStringValue(kCustomTabsKey, buf.data(), buf.size());
    buf.back() = 0;
    g_customTabs.clear();
    for (const auto& line : Split(buf.data(), L'\n')) {
        auto fields = Split(line, L'\t');
        if (fields.empty() || fields[0].empty() || (int)g_customTabs.size() >= kMaxCustomTabs) continue;
        CustomTab tab;
        tab.name = fields[0];
        for (size_t i = 1; i < fields.size(); i++)
            if (!fields[i].empty()) tab.targets.push_back(ToLower(fields[i]));
        g_customTabs.push_back(std::move(tab));
    }
}

void SaveBuiltinTabs() {
    std::wstring data;
    for (int i = 0; i < kFilterCount; i++) {
        const auto& b = g_builtinTabs[i];
        data += std::to_wstring(i) + L"\t" + (b.hidden ? L"1" : L"0");
        for (const auto& t : b.added) { data += L"\t+"; data += Clean(t); }
        for (const auto& t : b.removed) { data += L"\t-"; data += Clean(t); }
        data += L'\n';
    }
    Wh_SetStringValue(kBuiltinTabsKey, data.c_str());
}

void LoadBuiltinTabs() {
    std::vector<WCHAR> buf(1 << 17, 0);
    Wh_GetStringValue(kBuiltinTabsKey, buf.data(), buf.size());
    buf.back() = 0;
    for (auto& b : g_builtinTabs) b = BuiltinTab{};
    for (const auto& line : Split(buf.data(), L'\n')) {
        auto fields = Split(line, L'\t');
        if (fields.size() < 2) continue;
        int i = _wtoi(fields[0].c_str());
        if (i < 0 || i >= kFilterCount) continue;
        auto& b = g_builtinTabs[i];
        b.hidden = fields[1] == L"1";
        for (size_t k = 2; k < fields.size(); k++) {
            if (fields[k].size() < 2) continue;
            (fields[k][0] == L'+' ? b.added : b.removed).push_back(ToLower(fields[k].substr(1)));
        }
    }
}

void SaveSearchHistory() {
    std::wstring data;
    for (const auto& q : g_searchHistory) { data += Clean(q); data += L'\n'; }
    Wh_SetStringValue(kHistoryKey, data.c_str());
}

void LoadSearchHistory() {
    std::vector<WCHAR> buf(1 << 14, 0);
    Wh_GetStringValue(kHistoryKey, buf.data(), buf.size());
    buf.back() = 0;
    g_searchHistory.clear();
    for (auto& q : Split(buf.data(), L'\n'))
        if (!q.empty() && (int)g_searchHistory.size() < kHistoryMax) g_searchHistory.push_back(q);
}

void AddSearchHistory(std::wstring q) {
    if (!g_historyEnabled) return;
    q.erase(0, q.find_first_not_of(L' '));
    q.erase(q.find_last_not_of(L' ') + 1);
    if (q.size() < 2) return;
    auto it = std::find_if(g_searchHistory.begin(), g_searchHistory.end(),
        [&](const std::wstring& h) { return ToLower(h) == ToLower(q); });
    if (it != g_searchHistory.end()) g_searchHistory.erase(it);
    g_searchHistory.insert(g_searchHistory.begin(), q);
    if ((int)g_searchHistory.size() > kHistoryMax) g_searchHistory.resize(kHistoryMax);
    SaveSearchHistory();
}


void ReconcileFolders(AppList& fresh) {
    std::unordered_map<std::wstring, std::shared_ptr<App>> byTarget;
    for (const auto& app : fresh) byTarget[app->target] = app;
    std::unordered_set<std::wstring> inFolders;
    for (auto& folder : g_folders) {
        std::vector<std::shared_ptr<App>> updated;
        for (const auto& old : folder.apps) {
            auto it = byTarget.find(old->target);
            if (it != byTarget.end() && inFolders.insert(old->target).second) updated.push_back(it->second);
        }
        folder.apps = std::move(updated);
    }
    if (inFolders.empty()) return;
    fresh.erase(std::remove_if(fresh.begin(), fresh.end(), [&](const std::shared_ptr<App>& app) { return inFolders.count(app->target) != 0; }), fresh.end());
}

////////////////////////////////////////////////////////////////////////////////
// Custom icons

[[clang::no_destroy]] std::unordered_map<std::wstring, std::wstring> g_customIcons;
bool g_dialogOpen;

void SaveCustomIcons() {
    std::wstring data;
    for (auto& [target, path] : g_customIcons) { data += Clean(target); data += L'\t'; data += Clean(path); data += L'\n'; }
    Wh_SetStringValue(kIconsKey, data.c_str());
}

void LoadCustomIcons() {
    std::vector<WCHAR> buf(1 << 17, 0);
    Wh_GetStringValue(kIconsKey, buf.data(), buf.size());
    buf.back() = 0;
    g_customIcons.clear();
    for (const auto& line : Split(buf.data(), L'\n')) {
        auto f = Split(line, L'\t');
        if (f.size() == 2 && !f[0].empty() && !f[1].empty()) g_customIcons[f[0]] = f[1];
    }
}

bool LoadImageFile(const std::wstring& path, int size, App& app) {
    ComPtr<IWICImagingFactory> wic; ComPtr<IWICBitmapDecoder> dec; ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICBitmapScaler> scaler; ComPtr<IWICFormatConverter> conv;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic))) ||
        FAILED(wic->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &dec))) return false;
    UINT frames = 0, best = 0, bestArea = 0; dec->GetFrameCount(&frames);
    for (UINT i = 0; i < frames; i++) {
        ComPtr<IWICBitmapFrameDecode> f; UINT w = 0, h = 0;
        if (SUCCEEDED(dec->GetFrame(i, &f)) && SUCCEEDED(f->GetSize(&w, &h)) && w * h > bestArea) { bestArea = w * h; best = i; }
    }
    if (FAILED(dec->GetFrame(best, &frame))) return false;
    UINT w = 0, h = 0; frame->GetSize(&w, &h);
    if (!w || !h) return false;
    float k = (float)size / std::max(w, h);
    UINT sw = std::clamp((UINT)std::lround(w * k), 1u, (UINT)size), sh = std::clamp((UINT)std::lround(h * k), 1u, (UINT)size);
    if (FAILED(wic->CreateBitmapScaler(&scaler)) ||
        FAILED(scaler->Initialize(frame.Get(), sw, sh, (WICBitmapInterpolationMode)4)) ||
        FAILED(wic->CreateFormatConverter(&conv)) ||
        FAILED(conv->Initialize(scaler.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom))) return false;
    std::vector<UINT32> tmp((size_t)sw * sh);
    if (FAILED(conv->CopyPixels(nullptr, sw * 4, (UINT)tmp.size() * 4, (BYTE*)tmp.data()))) return false;
    app.pixels.assign((size_t)size * size, 0);
    UINT ox = (size - sw) / 2, oy = (size - sh) / 2;
    for (UINT y = 0; y < sh; y++) std::copy_n(&tmp[(size_t)y * sw], sw, &app.pixels[(size_t)(y + oy) * size + ox]);
    app.iconSize = size; app.bitmap.Reset(); app.generation = 0;
    return true;
}

void ApplyCustomIcons(AppList& list, int size) {
    for (auto& app : list) {
        auto it = g_customIcons.find(app->target);
        if (it != g_customIcons.end()) LoadImageFile(it->second, size, *app);
    }
}

int CurrentIconPixels() { return g_appsIconPixels ? g_appsIconPixels : IconPixels(); }

void ResetAppIcon(App& app) {
    app.pixels.clear(); app.bitmap.Reset(); app.generation = 0;
    ComPtr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(app.target.c_str(), nullptr, IID_PPV_ARGS(&item))))
        LoadAppIcon(item.Get(), CurrentIconPixels(), app);
}

bool PickImage(HWND owner, std::wstring& out) {
    ComPtr<IFileOpenDialog> dlg;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg)))) return false;
    COMDLG_FILTERSPEC types[] = { {Tr(L"Images"), L"*.png;*.jpg;*.jpeg;*.bmp;*.ico;*.gif;*.webp"}, {Tr(L"All files"), L"*.*"} };
    dlg->SetFileTypes(ARRAYSIZE(types), types);
    dlg->SetTitle(Tr(L"Choose icon image"));
    g_dialogOpen = true;
    SetWindowPos(owner, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    HRESULT hr = dlg->Show(owner);
    SetWindowPos(owner, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    g_dialogOpen = false;
    ComPtr<IShellItem> res; PWSTR path = nullptr;
    if (FAILED(hr) || FAILED(dlg->GetResult(&res)) || FAILED(res->GetDisplayName(SIGDN_FILESYSPATH, &path))) return false;
    out = path; CoTaskMemFree(path);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// File search: Everything (IPC) / Windows Search / direct scan (worker threads)

std::atomic<UINT> g_fileSearchGen{0};
std::atomic<int> g_fileThreads{0};
[[clang::no_destroy]] AppList g_fileResults;
[[clang::no_destroy]] std::wstring g_fileStatus;
bool g_fileSearching;

struct FileJob { std::wstring query; Indexer indexer; int max; int iconPx; UINT gen; HWND wnd; };
struct FileResult { UINT gen; AppList list; std::wstring status; };

bool Stale(UINT gen) { return gen != g_fileSearchGen || g_workerStop; }

// ---------- Everything IPC (WM_COPYDATA, no DLL) ----------
// Layouts from everything_ipc.h (QUERY2 / LIST2 / ITEM2)
constexpr WCHAR kEvIpcClass[] = L"WindhawkLaunchpadEverythingIPC";
constexpr DWORD kEvCopyQuery2W = 18;                 // EVERYTHING_IPC_COPYDATA_QUERY2W
constexpr DWORD kEvReqName = 0x1, kEvReqPath = 0x2, kEvReqFullPath = 0x4;
constexpr DWORD kEvSortNameAsc = 1;                  // EVERYTHING_IPC_SORT_NAME_ASCENDING
constexpr DWORD kEvReplyId = 0x4C503031;             // our dwData tag for replies
constexpr UINT  kEvWmIpc = WM_USER;                  // EVERYTHING_WM_IPC
constexpr WPARAM kEvIsDbLoaded = 401;                // EVERYTHING_IPC_IS_DB_LOADED

#pragma pack(push, 1)
struct EvQuery2 { DWORD reply_hwnd, reply_copydata_message, search_flags, offset, max_results, request_flags, sort_type; };
struct EvList2  { DWORD totitems, numitems, offset, request_flags, sort_type; };
struct EvItem2  { DWORD flags, data_offset; };
#pragma pack(pop)

struct EvReply { std::vector<std::wstring>* out; int max; bool done; };

// Each item's data (at data_offset, relative to the list start) holds the requested
// fields in flag order; strings are DWORD length + WCHAR[length] + NUL.
void ParseEvList(const COPYDATASTRUCT* cds, EvReply& r) {
    const BYTE* base = (const BYTE*)cds->lpData; size_t size = cds->cbData;
    if (!base || size < sizeof(EvList2)) return;
    const auto* list = (const EvList2*)base;
    const auto* items = (const EvItem2*)(base + sizeof(EvList2));
    for (DWORD i = 0; i < list->numitems && (int)r.out->size() < r.max; i++) {
        if (sizeof(EvList2) + (size_t)(i + 1) * sizeof(EvItem2) > size) break;
        size_t off = items[i].data_offset;
        std::wstring name, path, full;
        for (DWORD flag : {kEvReqName, kEvReqPath, kEvReqFullPath}) {
            if (!(list->request_flags & flag)) continue;
            if (off > size || size - off < sizeof(DWORD)) return;
            DWORD len = 0;
            memcpy(&len, base + off, sizeof(len));
            off += sizeof(DWORD);
            // Validate without overflowing size_t when processing external IPC data.
            if ((size_t)len > (size - off) / sizeof(WCHAR) ||
                (size - off) / sizeof(WCHAR) < (size_t)len + 1) return;
            std::wstring s((const WCHAR*)(base + off), (size_t)len);
            off += ((size_t)len + 1) * sizeof(WCHAR);
            (flag == kEvReqName ? name : flag == kEvReqPath ? path : full) = std::move(s);
        }
        if (full.empty() && !name.empty()) full = path.empty() ? name : path + L"\\" + name;
        if (!full.empty()) r.out->push_back(std::move(full));
    }
}

LRESULT CALLBACK EvIpcProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    if (msg == WM_COPYDATA) {
        auto* r = (EvReply*)GetWindowLongPtrW(h, GWLP_USERDATA);
        auto* cds = (const COPYDATASTRUCT*)l;
        if (r && cds && cds->dwData == kEvReplyId && !r->done) { ParseEvList(cds, *r); r->done = true; return TRUE; }
    }
    return DefWindowProcW(h, msg, w, l);
}

// FIX: the class was never registered -> CreateWindowExW failed ("IPC window failed")
std::atomic<bool> g_evClassRegistered{false};
SRWLOCK g_evClassLock = SRWLOCK_INIT;

bool EnsureEvIpcClass() {
    if (g_evClassRegistered) return true;
    AcquireSRWLockExclusive(&g_evClassLock);
    if (!g_evClassRegistered) {
        WNDCLASSEXW wc{sizeof(wc)};
        wc.lpfnWndProc = EvIpcProc;
        wc.hInstance = (HINSTANCE)&__ImageBase;
        wc.lpszClassName = kEvIpcClass;
        if (RegisterClassExW(&wc) || GetLastError() == ERROR_CLASS_ALREADY_EXISTS) g_evClassRegistered = true;
        else Wh_Log(L"Everything: RegisterClassEx failed (%u)", GetLastError());
    }
    ReleaseSRWLockExclusive(&g_evClassLock);
    return g_evClassRegistered;
}

void UnregisterEvIpcClass() { // call from Wh_ModUninit
    if (g_evClassRegistered.exchange(false)) UnregisterClassW(kEvIpcClass, (HINSTANCE)&__ImageBase);
}

HWND FindEverything() {
    for (PCWSTR cls : {L"EVERYTHING_TASKBAR_NOTIFICATION", L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)"})
        if (HWND h = FindWindowW(cls, nullptr)) return h;
    return nullptr;
}

HWND CreateEvReplyWindow() {
    HINSTANCE inst = (HINSTANCE)&__ImageBase;
    HWND w = CreateWindowExW(0, kEvIpcClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, inst, nullptr);
    if (!w) // fallback: regular hidden top-level window
        w = CreateWindowExW(WS_EX_TOOLWINDOW, kEvIpcClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    return w;
}

// Runs on the file worker thread: the reply window lives and dies on this thread.
bool SearchEverything(const FileJob& job, std::vector<std::wstring>& out, std::wstring& status) {
    HWND ev = FindEverything();
        if (!ev) { status = Tr(L"Everything is not running"); return false; }
    if (!EnsureEvIpcClass()) { status = L"Everything: IPC class failed"; return false; }

    EvReply reply{&out, job.max, false};
    HWND wnd = CreateEvReplyWindow();
    if (!wnd) { status = L"Everything: IPC window failed"; Wh_Log(L"CreateWindowEx: %u", GetLastError()); return false; }
    SetWindowLongPtrW(wnd, GWLP_USERDATA, (LONG_PTR)&reply);
    ChangeWindowMessageFilterEx(wnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);

    size_t textBytes = (job.query.size() + 1) * sizeof(WCHAR);
    std::vector<BYTE> buf(sizeof(EvQuery2) + textBytes, 0);
    auto* q = (EvQuery2*)buf.data();
    q->reply_hwnd = (DWORD)(DWORD_PTR)wnd;
    q->reply_copydata_message = kEvReplyId;
    q->search_flags = 0;
    q->offset = 0;
    q->max_results = (DWORD)job.max;
    q->request_flags = kEvReqFullPath;
    q->sort_type = kEvSortNameAsc;
    memcpy(buf.data() + sizeof(EvQuery2), job.query.c_str(), textBytes);

    COPYDATASTRUCT cds{kEvCopyQuery2W, (DWORD)buf.size(), buf.data()};
    DWORD_PTR accepted = 0;
    SetLastError(0);
    if (!SendMessageTimeoutW(ev, WM_COPYDATA, (WPARAM)wnd, (LPARAM)&cds, SMTO_ABORTIFHUNG, 2000, &accepted)) {
        status = GetLastError() == ERROR_ACCESS_DENIED
            ? Tr(L"Everything is running as admin (run it without admin)")
            : Tr(L"Everything did not accept the query");
        } else if (!accepted && !reply.done) {
            DWORD_PTR loaded = 1;
            SendMessageTimeoutW(ev, kEvWmIpc, kEvIsDbLoaded, 0, SMTO_ABORTIFHUNG, 500, &loaded);
            status = loaded ? Tr(L"Everything rejected the query") : Tr(L"Everything is still indexing...");

    } else {
        double deadline = Now() + 3.0;
        while (!reply.done && !Stale(job.gen) && Now() < deadline) {
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 30, QS_ALLINPUT | QS_SENDMESSAGE);
            MSG m; while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) { TranslateMessage(&m); DispatchMessageW(&m); }
        }
        if (!reply.done && !Stale(job.gen)) status = Tr(L"Everything did not reply");
    }
    SetWindowLongPtrW(wnd, GWLP_USERDATA, 0);
    DestroyWindow(wnd);
    Wh_Log(L"Everything: %s (%d results)", reply.done ? L"ok" : status.c_str(), (int)out.size());
    return reply.done;
}

// ---------- Windows Search (ISearchFolderItemFactory + condition) ----------
constexpr CLSID kClsidSearchFolderFactory = {0x14010e02, 0xbbbd, 0x41f0, {0x88, 0xe3, 0xed, 0xa3, 0x71, 0x21, 0x65, 0x84}};
constexpr CLSID kClsidConditionFactory    = {0xe03e85b0, 0x7be3, 0x4000, {0xba, 0x98, 0x6c, 0x13, 0xde, 0x9f, 0xa4, 0x86}};
constexpr PROPERTYKEY kPkeyItemNameDisplay = {{0xb725f130, 0x47ef, 0x101a, {0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac}}, 10};
// IConditionFactory2 (structuredquery.h is missing in MinGW). Only the vtable order matters.
constexpr IID kIidConditionFactory2 = {0x71d222e1, 0x432f, 0x429e, {0x8c, 0x13, 0xb6, 0xda, 0xfd, 0xe5, 0x07, 0x7a}};
constexpr IID kIidCondition         = {0x0fc988d4, 0xc935, 0x4b97, {0xa9, 0x73, 0x46, 0x28, 0x2e, 0xa1, 0x75, 0xc8}};
constexpr int kCopValueContains = 9; // COP_VALUE_CONTAINS

struct IConditionFactory2Min : IUnknown {
    // IConditionFactory
    virtual HRESULT STDMETHODCALLTYPE MakeNot() = 0;
    virtual HRESULT STDMETHODCALLTYPE MakeAndOr() = 0;
    virtual HRESULT STDMETHODCALLTYPE MakeLeaf() = 0;
    virtual HRESULT STDMETHODCALLTYPE Resolve() = 0;
    // IConditionFactory2
    virtual HRESULT STDMETHODCALLTYPE CreateTrueFalse() = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateNegation() = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCompoundFromObjectArray() = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateCompoundFromArray() = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateStringLeaf(const PROPERTYKEY& key, int cop, LPCWSTR value,
                                                       LPCWSTR locale, int options, REFIID riid, void** ppv) = 0;
};


bool SearchWindowsIndex(const FileJob& job, std::vector<std::wstring>& out) {
    ComPtr<ISearchFolderItemFactory> factory;
    ComPtr<IConditionFactory2Min> conditions;
    ComPtr<ICondition> condition;
    if (FAILED(CoCreateInstance(kClsidSearchFolderFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))) ||
        FAILED(CoCreateInstance(kClsidConditionFactory, nullptr, CLSCTX_INPROC_SERVER, kIidConditionFactory2, (void**)conditions.GetAddressOf())) ||
        FAILED(conditions->CreateStringLeaf(kPkeyItemNameDisplay, kCopValueContains, job.query.c_str(), nullptr,
                                            0, kIidCondition, (void**)condition.GetAddressOf()))) {
        Wh_Log(L"Windows Search: factory failed");
        return false;
    }

    // Scope = user profile (indexed by default; non-indexed scopes would fall back to slow grep)
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHGetKnownFolderIDList(FOLDERID_Profile, 0, nullptr, &pidl))) return false;
    ComPtr<IShellItemArray> scope;
    PCIDLIST_ABSOLUTE list[] = {pidl};
    HRESULT hr = SHCreateShellItemArrayFromIDLists(1, list, &scope);
    CoTaskMemFree(pidl);
    if (FAILED(hr)) return false;

    factory->SetScope(scope.Get());
    factory->SetCondition(condition.Get());
    factory->SetDisplayName(L"Launchpad");

    ComPtr<IShellItem> folder; ComPtr<IEnumShellItems> en;
    if (FAILED(factory->GetShellItem(IID_PPV_ARGS(&folder))) ||
        FAILED(folder->BindToHandler(nullptr, kBhidEnumItems, IID_PPV_ARGS(&en)))) {
        Wh_Log(L"Windows Search: enumeration failed");
        return false;
    }
    ComPtr<IShellItem> item;
    while ((int)out.size() < job.max && !Stale(job.gen) && en->Next(1, item.ReleaseAndGetAddressOf(), nullptr) == S_OK) {
        PWSTR path = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) { out.push_back(path); CoTaskMemFree(path); }
    }
    Wh_Log(L"Windows Search: %d results", (int)out.size());
    return true;
}
// ---------- Direct scan (fallback, time-limited) ----------
void ScanDir(const std::wstring& dir, const FileJob& job, std::vector<std::wstring>& out, double deadline, int depth) {
    if (depth > 12 || (int)out.size() >= job.max || Stale(job.gen) || Now() > deadline) return;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW((dir + L"\\*").c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
    if (h == INVALID_HANDLE_VALUE) return;
    std::vector<std::wstring> subdirs;
    do {
        // Abort promptly when a newer query replaces this job or its time budget expires.
        if (Stale(job.gen) || Now() >= deadline || (int)out.size() >= job.max) break;
        if (fd.cFileName[0] == L'.' || fd.cFileName[0] == L'$') continue;
        std::wstring full = dir + L"\\" + fd.cFileName;
        if (StrStrIW(fd.cFileName, job.query.c_str())) out.push_back(full);
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(fd.dwFileAttributes & (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_SYSTEM)))
            subdirs.push_back(full);
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    for (auto& s : subdirs) ScanDir(s, job, out, deadline, depth + 1);
}

void ScanFallback(const FileJob& job, std::vector<std::wstring>& out) {
    double deadline = Now() + 4.0;
    PWSTR profile = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &profile))) {
        std::wstring home = profile; CoTaskMemFree(profile);
        ScanDir(home, job, out, deadline, 0); // user profile first
    }
    DWORD drives = GetLogicalDrives();
    for (WCHAR d = L'A'; d <= L'Z'; d++) {
        if (Stale(job.gen) || Now() >= deadline || (int)out.size() >= job.max) break;
        if (!(drives & (1u << (d - L'A')))) continue;
        WCHAR rootSlash[] = {d, L':', L'\\', 0};
        if (GetDriveTypeW(rootSlash) != DRIVE_FIXED) continue;
        WCHAR root[] = {d, L':', 0};
        ScanDir(root, job, out, deadline, 0);
    }
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
}
// Icon cache by extension (shared by file threads)
struct CachedIcon { std::vector<UINT32> pixels; int size; };
[[clang::no_destroy]] std::unordered_map<std::wstring, CachedIcon> g_extIcons;
SRWLOCK g_extIconsLock = SRWLOCK_INIT;
int g_extIconsPx;

void LoadFileIcon(const std::wstring& path, int px, App& app) {
    std::wstring ext = ToLower(PathFindExtensionW(path.c_str()));
    DWORD attr = GetFileAttributesW(path.c_str());
    bool unique = ext.empty() || (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
    for (PCWSTR e : {L".exe", L".lnk", L".ico", L".url", L".scr", L".cpl", L".msc", L".appref-ms"})
        if (ext == e) unique = true;

    if (!unique) {
        AcquireSRWLockShared(&g_extIconsLock);
        bool hit = false;
        if (g_extIconsPx == px) {
            auto it = g_extIcons.find(ext);
            if (it != g_extIcons.end()) { app.pixels = it->second.pixels; app.iconSize = it->second.size; hit = true; }
        }
        ReleaseSRWLockShared(&g_extIconsLock);
        if (hit) return;
    }
    ComPtr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item)))) LoadAppIcon(item.Get(), px, app);
    if (!unique && !app.pixels.empty()) {
        AcquireSRWLockExclusive(&g_extIconsLock);
        if (g_extIconsPx != px) { g_extIcons.clear(); g_extIconsPx = px; }
        g_extIcons[ext] = {app.pixels, app.iconSize};
        ReleaseSRWLockExclusive(&g_extIconsLock);
    }
}


// The same item can come back more than once (overlapping scopes, links, several indexers)
void RemoveDuplicatePaths(std::vector<std::wstring>& paths) {
    std::unordered_set<std::wstring> seen;
    std::vector<std::wstring> unique;
    for (auto& path : paths) {
        std::wstring key = ToLower(path);
        while (key.size() > 3 && key.back() == L'\\') key.pop_back();
        if (seen.insert(key).second) unique.push_back(std::move(path));
    }
    paths = std::move(unique);
}

DWORD WINAPI FileSearchThread(void* param) {
    std::unique_ptr<FileJob> job((FileJob*)param);
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    const bool comInitialized = SUCCEEDED(comHr);
    std::vector<std::wstring> paths;
    std::wstring status;
    Indexer m = job->indexer;
    bool ok = false;
    if (m == Indexer::Auto || m == Indexer::Everything) ok = SearchEverything(*job, paths, status);
    if (!ok && (m == Indexer::Auto || m == Indexer::Windows)) {
        ok = SearchWindowsIndex(*job, paths);
        if (ok) status.clear();
    }
    if (!ok && (m == Indexer::Auto || m == Indexer::Scan)) { ScanFallback(*job, paths); status.clear(); }
    RemoveDuplicatePaths(paths);
    if ((int)paths.size() > job->max) paths.resize(job->max);

    auto res = std::make_unique<FileResult>();
    res->gen = job->gen;
    if (!paths.empty()) status.clear();
    else if (status.empty()) status = Tr(L"No files found");
    res->status = status;
    for (auto& path : paths) {
        if (Stale(job->gen)) break;
        auto app = std::make_shared<App>();
        app->kind = 1; app->name = PathFindFileNameW(path.c_str()); app->target = path;
        LoadFileIcon(path, job->iconPx, *app);
        if (Stale(job->gen)) break;
        res->list.push_back(std::move(app));
    }
    if (comInitialized) CoUninitialize();
    if (!Stale(job->gen) && IsWindow(job->wnd) && PostMessageW(job->wnd, WM_APP_FILES, 0, (LPARAM)res.get())) res.release();
    g_fileThreads--;
    return 0;
}

// Called on the UI thread (debounced by kTimerFileSearch)
void StartFileSearch(const std::wstring& q) {
    UINT gen = ++g_fileSearchGen; // invalidates every previous search
    g_fileResults.clear(); g_fileStatus.clear();
    g_fileSearching = g_settings.filesEnabled && q.size() >= 2;
    if (!g_fileSearching) return;
    auto* job = new FileJob{q, g_settings.indexer, g_settings.maxResults, CurrentIconPixels(), gen, g_wnd};
    g_fileThreads++;
    if (HANDLE t = CreateThread(nullptr, 0, FileSearchThread, job, 0, nullptr)) CloseHandle(t);
    else { g_fileThreads--; delete job; g_fileSearching = false; }
}

void CancelFileSearch() {
    ++g_fileSearchGen;
    g_fileSearching = false;
    g_fileResults.clear(); g_fileStatus.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Categories: search history, indexed files & clipboard

void LoadTabSource() {
    g_tabItems.clear();
    if (g_siriTab == 1 && g_settings.filesEnabled) {
        if (g_query.empty()) {          // busca vazia: apenas o histórico de buscas
            if (g_historyEnabled) {
                for (const auto& q : g_searchHistory) {
                    auto app = std::make_shared<App>();
                    app->kind = 3; app->name = q; app->target = q;
                    g_tabItems.push_back(std::move(app));
                }
            }
        } else {                        // digitando: resultados do indexador
            g_tabItems = g_fileResults;
        }
    } else if (g_siriTab == 3) {
        static const std::pair<PCWSTR, PCWSTR> kActions[] = {
            {L"lock", L"Lock"}, {L"sleep", L"Sleep"}, {L"restart", L"Restart"}, {L"shutdown", L"Shut down"}, {L"signout", L"Sign out"},
            {L"settings", L"Settings"}, {L"taskmgr", L"Task Manager"}, {L"explorer", L"File Explorer"}, {L"control", L"Control Panel"},
            {L"screenshot", L"Screenshot"}, {L"recycle", L"Empty Recycle Bin"},
        };
        for (const auto& [id, label] : kActions) {
            auto app = std::make_shared<App>();
            app->kind = 4; app->name = Tr(label); app->target = id;
            g_tabItems.push_back(std::move(app));
        }
    } else if (g_siriTab == 2 && g_settings.clipEnabled) {
        for (const auto& text : g_clipHistory) {
            auto app = std::make_shared<App>();
            app->kind = 2; app->name = ClipSnippet(text); app->target = text;
            g_tabItems.push_back(std::move(app));
        }
    }
}


void RebuildLayout() {
    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        g_layoutItems.clear();
        for (int i = 0; i < (int)g_folders[g_openFolderIndex].apps.size(); i++) g_layoutItems.push_back(i);
        return;
    }
    BuildMainItems(g_layoutItems);
}
////////////////////////////////////////////////////////////////////////////////
// Drawing

[[clang::no_destroy]] ComPtr<ID2D1Factory> g_d2d;
[[clang::no_destroy]] ComPtr<IDWriteFactory> g_dwrite;
[[clang::no_destroy]] ComPtr<ID2D1DCRenderTarget> g_target;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_brush;
UINT g_targetGeneration = 1;
[[clang::no_destroy]] ComPtr<ID2D1Bitmap1> g_layerBitmap;
UINT g_layerGeneration;

struct Surface { HDC dc = nullptr; HBITMAP bitmap = nullptr; HGDIOBJ old = nullptr; int width = 0, height = 0; };
Surface g_surface;

void FreeSurface(Surface& s) {
    if (s.dc) { SelectObject(s.dc, s.old); DeleteObject(s.bitmap); DeleteDC(s.dc); }
    s = {};
}

bool EnsureSurface(Surface& s, int width, int height) {
    if (s.dc && s.width == width && s.height == height) return true;
    FreeSurface(s);
    BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = width; bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    s.dc = CreateCompatibleDC(nullptr);
    s.bitmap = CreateDIBSection(s.dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!s.bitmap) { DeleteDC(s.dc); s = {}; return false; }
    s.old = SelectObject(s.dc, s.bitmap); s.width = width; s.height = height;
    return true;
}

bool EnsureTarget() {
    if (g_target) return true;
    auto props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &g_target))) return false;
    g_target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    g_target->CreateSolidColorBrush(D2D1::ColorF(0, 0), &g_brush);
    g_targetGeneration++;
    return true;
}

constexpr int kOrbBlobs = 5;
[[clang::no_destroy]] ComPtr<ID2D1RadialGradientBrush> g_orbBrush[kOrbBlobs];

void ResetTarget() {
    g_layerBitmap.Reset(); g_brush.Reset();
    for (auto& b : g_orbBrush) b.Reset();
    g_target.Reset();
}

ID2D1Bitmap1* EnsureLayerBitmap(ID2D1DeviceContext* dc, int w, int h) {
    if (g_layerBitmap && g_layerGeneration == g_targetGeneration) {
        D2D1_SIZE_U sz = g_layerBitmap->GetPixelSize();
        if ((int)sz.width == w && (int)sz.height == h) return g_layerBitmap.Get();
    }
    g_layerBitmap.Reset(); g_layerGeneration = g_targetGeneration;
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96);
    dc->CreateBitmap(D2D1::SizeU(w, h), nullptr, 0, &props, &g_layerBitmap);
    return g_layerBitmap.Get();
}

ID2D1Bitmap* GetBitmap(App& app) {
    if (app.pixels.empty()) return nullptr;
    if (app.generation != g_targetGeneration) {
        app.generation = g_targetGeneration; app.bitmap.Reset();
        g_target->CreateBitmap(D2D1::SizeU(app.iconSize, app.iconSize), app.pixels.data(), app.iconSize * 4,
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96), &app.bitmap);
    }
    return app.bitmap.Get();
}

D2D1_COLOR_F Alpha(D2D1_COLOR_F c, float a) { c.a = a; return c; }

Theme ActiveTheme() { return g_settings.siri ? g_settings.siriTheme : g_settings.theme; }

// Blur behind the shapes: the Custom theme uses the General setting, the other themes have their own
// One blur level for every theme; 0 keeps the background live (nothing is captured)
extern HWND g_glassWnd;
extern bool g_glassOk;

// Blur by snapshot (a still image): only when the live blur isn't available
bool TransparencyEffectsOn();
bool g_transparencyOn = true;   // Windows' transparency effects (its blur needs them), read when opening
int BlurStrength() { return g_settings.liveBackground || (g_glassOk && g_transparencyOn) ? 0 : g_settings.blurAmount; }
bool LiveBlurOn() { return g_glassOk && g_transparencyOn && !g_settings.liveBackground; }

// Margin the window keeps around themed shapes so their shadow is not cut
float ThemeShadowMargin(float s) {
    switch (ActiveTheme()) {
        case Theme::Glass: case Theme::Light: case Theme::Siri: return std::round(18 * s);
        default: return 0.0f;
    }
}

// Base color for popups (context menu) that matches the theme
D2D1_COLOR_F ThemeMenuColor() {
    switch (ActiveTheme()) {
        case Theme::Light: return D2D1::ColorF(0.96f, 0.96f, 0.98f, 0.82f);
        case Theme::Glass: return D2D1::ColorF(0.10f, 0.10f, 0.12f, 0.55f);
        case Theme::Siri: return D2D1::ColorF(0.08f, 0.08f, 0.09f, 0.82f);
        default: return Alpha(g_settings.siri ? g_settings.siriPanelColor : g_settings.tint, 0.70f);
    }
}
void Fill(const D2D1_RECT_F& r, D2D1_COLOR_F color, float radius = 0) {
    g_brush->SetColor(color);
    if (radius > 0) g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), g_brush.Get());
    else g_target->FillRectangle(r, g_brush.Get());
}

D2D1_COLOR_F Fg(float a) {
    if (g_settings.darkText) return D2D1::ColorF(0.07f, 0.07f, 0.09f, a);
    return D2D1::ColorF(1.0f, 1.0f, 1.0f, a);
}


D2D1_COLOR_F DangerInk() {
    if (g_settings.darkText) return D2D1::ColorF(0.78f, 0.10f, 0.10f, 0.95f);
    return D2D1::ColorF(1.0f, 0.55f, 0.55f, 0.95f);
}

////////////////////////////////////////////////////////////////////////////////
// Background (Blur Mode: Adjustable)

struct Backdrop {
    std::vector<UINT32> pixels;
    int width = 0, height = 0;
    float sourceWidth = 0, sourceHeight = 0;
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
};
[[clang::no_destroy]] Backdrop g_backdropImage;

void BoxBlurPass(const UINT32* src, UINT32* dst, int lines, int length, int lineStep, int step, int radius) {
    int window = 2 * radius + 1;
    for (int line = 0; line < lines; line++) {
        const UINT32* s = src + (size_t)line * lineStep; UINT32* d = dst + (size_t)line * lineStep;
        auto at = [&](int i) { return s[(size_t)std::clamp(i, 0, length - 1) * step]; };
        int sum[3] = {};
        for (int i = -radius; i <= radius; i++) { UINT32 p = at(i); sum[0] += p & 0xFF; sum[1] += (p >> 8) & 0xFF; sum[2] += (p >> 16) & 0xFF; }
        for (int i = 0; i < length; i++) {
            d[(size_t)i * step] = 0xFF000000 | ((sum[2] / window) << 16) | ((sum[1] / window) << 8) | (sum[0] / window);
            UINT32 in = at(i + radius + 1), out = at(i - radius);
            sum[0] += (int)(in & 0xFF) - (int)(out & 0xFF);
            sum[1] += (int)((in >> 8) & 0xFF) - (int)((out >> 8) & 0xFF);
            sum[2] += (int)((in >> 16) & 0xFF) - (int)((out >> 16) & 0xFF);
        }
    }
}

void CaptureBackdrop(const RECT& rc, float sigma) {
    g_backdropImage = {};
    int width = rc.right - rc.left, height = rc.bottom - rc.top;
    if (width <= 0 || height <= 0) return;
    BITMAPINFO bi{}; bi.bmiHeader.biSize = sizeof(bi.bmiHeader); bi.bmiHeader.biWidth = width; bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr; HDC screen = GetDC(nullptr); HDC dc = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) { DeleteDC(dc); ReleaseDC(nullptr, screen); return; }
    HGDIOBJ old = SelectObject(dc, bitmap);
    BitBlt(dc, 0, 0, width, height, screen, rc.left, rc.top, SRCCOPY); GdiFlush(); ReleaseDC(nullptr, screen);
    int f = sigma >= 3 ? std::clamp((int)(sigma / 3), 1, 16) : 1;
    int sw = (width + f - 1) / f, sh = (height + f - 1) / f;
    std::vector<UINT32> small((size_t)sw * sh); const UINT32* full = (const UINT32*)bits;
    for (int y = 0; y < sh; y++) {
        for (int x = 0; x < sw; x++) {
            int sum[3] = {}, count = 0;
            for (int yy = y * f; yy < std::min(height, (y + 1) * f); yy++) {
                for (int xx = x * f; xx < std::min(width, (x + 1) * f); xx++) {
                    UINT32 p = full[(size_t)yy * width + xx]; sum[0] += p & 0xFF; sum[1] += (p >> 8) & 0xFF; sum[2] += (p >> 16) & 0xFF; count++;
                }
            }
            small[(size_t)y * sw + x] = 0xFF000000 | ((sum[2] / count) << 16) | ((sum[1] / count) << 8) | (sum[0] / count);
        }
    }
    SelectObject(dc, old); DeleteObject(bitmap); DeleteDC(dc);
    float s = sigma / f; int radius = (int)std::lround((std::sqrt(4 * s * s + 1) - 1) / 2);
    if (radius > 0) {
        std::vector<UINT32> temp(small.size());
        for (int pass = 0; pass < 3; pass++) {
            BoxBlurPass(small.data(), temp.data(), sh, sw, sw, 1, radius); BoxBlurPass(temp.data(), small.data(), sw, sh, 1, sw, radius);
        }
    }
    g_backdropImage.pixels = std::move(small); g_backdropImage.width = sw; g_backdropImage.height = sh;
    g_backdropImage.sourceWidth = (float)width / f; g_backdropImage.sourceHeight = (float)height / f;
}

ID2D1Bitmap* GetBackdropBitmap() {
    Backdrop& b = g_backdropImage;
    if (b.pixels.empty()) return nullptr;
    if (b.generation != g_targetGeneration) {
        b.generation = g_targetGeneration; b.bitmap.Reset();
        g_target->CreateBitmap(D2D1::SizeU(b.width, b.height), b.pixels.data(), b.width * 4,
            D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 96, 96), &b.bitmap);
    }
    return b.bitmap.Get();
}

////////////////////////////////////////////////////////////////////////////////
// Fullscreen: available area minus the Dock

constexpr WCHAR kDockClass[] = L"WindhawkMacDock";
constexpr WCHAR kDockPlateClass[] = L"WindhawkMacDockPlate";
constexpr WCHAR kDockEdgeProp[] = L"WindhawkMacDock.Edge";
constexpr WCHAR kDockReservedProp[] = L"WindhawkMacDock.Reserved";

RECT AvailableArea(HMONITOR monitor) {
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    struct Search { HMONITOR monitor; RECT monitorRect; RECT area; } search{monitor, mi.rcMonitor, mi.rcWork};
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        auto* s = (Search*)param;
        WCHAR className[64];
        if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) || wcscmp(className, kDockClass) ||
            MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL) != s->monitor) return TRUE;
        int edge = (int)(INT_PTR)GetPropW(hwnd, kDockEdgeProp);
        int reserved = (int)(INT_PTR)GetPropW(hwnd, kDockReservedProp);
        const RECT& m = s->monitorRect; RECT& a = s->area;
        switch (edge) {
            case 1: a.bottom = std::min<LONG>(a.bottom, m.bottom - reserved); break;
            case 2: a.left = std::max<LONG>(a.left, m.left + reserved); break;
            case 3: a.right = std::min<LONG>(a.right, m.right - reserved); break;
        }
        return TRUE;
    }, (LPARAM)&search);
    RECT& a = search.area;
    if (a.right - a.left < 200 || a.bottom - a.top < 200) a = mi.rcWork;
    return a;
}

void RaiseDockWindows() {
    std::vector<HWND> windows;
    for (PCWSTR className : {kDockPlateClass, kDockClass}) {
        HWND hwnd = nullptr;
        while ((hwnd = FindWindowExW(nullptr, hwnd, className, nullptr))) {
            if (IsWindowVisible(hwnd)) windows.push_back(hwnd);
        }
    }
    for (HWND hwnd : windows) {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Layout

struct Layout {
    RECT area{}; int width = 0, height = 0; float scale = 1;
    float icon = 72, labelHeight = 20, cellWidth = 140, cellHeight = 140; float cellPad = 6;
    int columns = 1; float gridLeft = 0; float viewTop = 0, viewBottom = 0; int visibleRows = 1;
    float contentHeight = 0; D2D1_RECT_F search{}, btnBack{}, panel{}, btnPower{};
    bool horizontal = false; float pageWidth = 1; int perPage = 1, pages = 1;
    D2D1_RECT_F dots{}; float dotsH = 20, dotGap = 16;
    int extraBottom = 0;
    D2D1_RECT_F siriPanel{};
    D2D1_RECT_F siriPower{};
    D2D1_RECT_F siriTabs{};   // category strip (all modes)
    bool list = false;        // Files tab shown as a one-column list
    D2D1_RECT_F listHeader{}; // 'Recent searches' strip above the list
};

Layout g_layout;
float g_scroll, g_scrollTarget;
int g_wheelDelta;
float g_dotsHoverAnim; bool g_dotsHover; int g_dotHover = -1;
bool g_swiping; float g_swipeStartScroll; double g_lastFlip;

Layout g_mainLayout;
float g_mainScroll;
[[clang::no_destroy]] std::vector<int> g_mainItems;

D2D1_RECT_F g_folderOrigin{};
float g_folderAnim;
bool g_folderClosing;

bool g_renaming, g_renameSelectAll;
[[clang::no_destroy]] std::wstring g_renameText;

[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_labelFormat, g_searchFormat, g_siriFormat, g_messageFormat, g_titleFormat, g_menuFormat, g_tabFormat, g_listFormat, g_listSubFormat, g_glyphFormat, g_hintFormat, g_glyphLargeFormat, g_categoryFormat, g_keyFormat;
float g_formatScale;

constexpr float kTabWidth = 110; // per visible category

void EnsureFormats(float scale) {
    if (g_labelFormat && g_formatScale == scale) return;
    g_formatScale = scale;
    auto make = [&](float size, DWRITE_FONT_WEIGHT weight, DWRITE_TEXT_ALIGNMENT align, ComPtr<IDWriteTextFormat>& format) {
        format.Reset();
        g_dwrite->CreateTextFormat(L"Segoe UI Variable Text", nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size * scale, L"", &format);
        if (!format) return;
        format->SetTextAlignment(align); format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
        ComPtr<IDWriteInlineObject> ellipsis; g_dwrite->CreateEllipsisTrimmingSign(format.Get(), &ellipsis);
        format->SetTrimming(&trimming, ellipsis.Get());
    };
    make(12.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, g_labelFormat);
    make(14.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, g_searchFormat);
    make(18.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, g_siriFormat);
    make(16.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, g_titleFormat);
    make(16, DWRITE_FONT_WEIGHT_SEMI_LIGHT, DWRITE_TEXT_ALIGNMENT_CENTER, g_messageFormat);
    make(13.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, g_menuFormat);
    make(13.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_CENTER, g_tabFormat);
    make(14.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, g_listFormat);
    make(11.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, g_listSubFormat);
    make(12.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_TRAILING, g_hintFormat);
    make(15.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_TEXT_ALIGNMENT_LEADING, g_categoryFormat);
    make(11.5f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_TEXT_ALIGNMENT_CENTER, g_keyFormat);
    {
        // Icon font: Segoe Fluent Icons (Windows 11), Segoe MDL2 Assets otherwise
        PCWSTR family = L"Segoe Fluent Icons";
        ComPtr<IDWriteFontCollection> fonts;
        UINT32 index = 0;
        BOOL exists = FALSE;
        if (SUCCEEDED(g_dwrite->GetSystemFontCollection(&fonts)) && fonts) fonts->FindFamilyName(family, &index, &exists);
        if (!exists) family = L"Segoe MDL2 Assets";
        for (auto [size, format] : {std::pair{16.0f, std::addressof(g_glyphFormat)}, std::pair{19.0f, std::addressof(g_glyphLargeFormat)}}) {
            format->Reset();
            g_dwrite->CreateTextFormat(family, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                       size * scale, L"", format->GetAddressOf());
            if (*format) {
                (*format)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                (*format)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
        }
    }
    g_chipWidths.clear();
    g_chipPad = 26 * scale;
    for (int id = 0; id < 4; id++) {
        PCWSTR label = TabLabel(id);
        float textWidth = 0;
        ComPtr<IDWriteTextLayout> layout;
        if (g_tabFormat && SUCCEEDED(g_dwrite->CreateTextLayout(label, (UINT32)wcslen(label), g_tabFormat.Get(), 1000, 40 * scale, &layout))) {
            DWRITE_TEXT_METRICS tm{};
            if (SUCCEEDED(layout->GetMetrics(&tm))) textWidth = tm.widthIncludingTrailingWhitespace;
        }
        g_tabNatural[id] = std::max(std::round(kTabWidth * scale), textWidth + 30 * scale);
    }
}

// Original Siri shows the app filters instead of the category strip (categories come from the hover menu)
bool SiriStripVisible() { return g_settings.siriOriginal ? g_siriTab == 0 : TabsVisible(); }
// Chips: "All", the built-in categories and the user tabs (while picking apps: Cancel and Done)
std::vector<int> Chips() {
    if (Picking()) return {kChipCancel, kChipDone};
    std::vector<int> chips = {kChipAll};
    for (int i = 0; i < kFilterCount; i++) if (!g_builtinTabs[i].hidden) chips.push_back(i);
    for (int k = 0; k < (int)g_customTabs.size(); k++) chips.push_back(kChipCustom + k);
    return chips;
}

std::wstring ChipLabel(int chip) {
    if (chip == kChipAll) return Tr(L"All");
    if (chip == kChipCancel) return Tr(L"Cancel");
    if (chip == kChipDone) {
        wchar_t text[64];
        swprintf(text, 64, Tr(L"Done (%d)"), (int)g_selection.size());
        return text;
    }
    if (chip >= kChipCustom) {
        int k = chip - kChipCustom;
        if (k == g_chipRename) return g_renameText;
        return k < (int)g_customTabs.size() ? g_customTabs[k].name : std::wstring();
    }
    return FilterLabel(chip);
}

float ChipTextWidth(const std::wstring& text) {
    if (text.empty() || !g_tabFormat) return 0;
    if (auto it = g_chipWidths.find(text); it != g_chipWidths.end()) return it->second;
    float width = 0;
    ComPtr<IDWriteTextLayout> layout;
    if (SUCCEEDED(g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(), g_tabFormat.Get(), 2000, 100, &layout))) {
        DWRITE_TEXT_METRICS tm{};
        if (SUCCEEDED(layout->GetMetrics(&tm))) width = tm.widthIncludingTrailingWhitespace;
    }
    if (g_chipWidths.size() > 300) g_chipWidths.clear();
    g_chipWidths[text] = width;
    return width;
}

float ChipNatural(int chip) {
    float width = ChipTextWidth(ChipLabel(chip)) + g_chipPad;
    if (chip >= kChipCustom && chip - kChipCustom == g_chipRename) width = std::max(width, g_chipPad * 3);   // room to type
    return width;
}

float FiltersNaturalWidth() {
    float total = 0;
    for (int chip : Chips()) total += ChipNatural(chip);
    return total;
}

void FilterSlotRect(const D2D1_RECT_F& r, const std::vector<int>& chips, int i, float& left, float& width) {
    float total = 0, avail = r.right - r.left;
    for (int chip : chips) total += ChipNatural(chip);
    bool overflow = total > avail + 0.5f;
    float ratio = overflow || total <= 0 ? 1.0f : avail / total;
    left = r.left - (overflow ? std::clamp(g_chipScroll, 0.0f, total - avail) : 0.0f);
    for (int k = 0; k < i; k++) left += ChipNatural(chips[k]) * ratio;
    width = ChipNatural(chips[i]) * ratio;
}

void ClampChipScroll() {
    const D2D1_RECT_F& r = g_layout.siriTabs;
    g_chipScroll = std::clamp(g_chipScroll, 0.0f, std::max(0.0f, FiltersNaturalWidth() - (r.right - r.left)));
}

bool ChipsOverflow() {
    const D2D1_RECT_F& r = g_layout.siriTabs;
    return r.right > r.left && FiltersNaturalWidth() > r.right - r.left + 0.5f;
}

void EnsureChipVisible(int chip) {
    const D2D1_RECT_F& r = g_layout.siriTabs;
    if (r.right <= r.left) return;
    ClampChipScroll();
    auto chips = Chips();
    for (int i = 0; i < (int)chips.size(); i++) {
        if (chips[i] != chip) continue;
        float left, width;
        FilterSlotRect(r, chips, i, left, width);
        if (left < r.left) g_chipScroll -= r.left - left;
        else if (left + width > r.right) g_chipScroll += left + width - r.right;
    }
    ClampChipScroll();
}

float TabsNaturalWidth() {
    float total = 0;
    for (int i = 0; i < g_tabCount; i++) total += g_tabNatural[g_tabIds[i]];
    return total;
}

// Left edge and width of a category slot inside the (possibly narrowed) tab strip
void TabSlotRect(const D2D1_RECT_F& r, int slot, float& left, float& width) {
    float total = TabsNaturalWidth(), ratio = total > 0 ? (r.right - r.left) / total : 1.0f;
    left = r.left;
    for (int i = 0; i < slot; i++) left += g_tabNatural[g_tabIds[i]] * ratio;
    width = g_tabNatural[g_tabIds[slot]] * ratio;
}

float MaxScroll() {
    const Layout& L = g_layout;
    if (L.horizontal) return std::max(0, L.pages - 1) * L.pageWidth;
    return std::max(0.0f, L.contentHeight - (L.viewBottom - L.viewTop));
}

// ---------- Math helpers ----------
float Ease(float t) { t = std::clamp(t, 0.0f, 1.0f); return 1 - (1 - t) * (1 - t) * (1 - t); }
float EaseInOut(float t) { t = std::clamp(t, 0.0f, 1.0f); return t < 0.5f ? 4 * t * t * t : 1 - std::pow(-2 * t + 2, 3.0f) / 2; }
// Ease-out that overshoots the target by an amount controlled by c1 (0 = no overshoot)
float BackOutK(float t, float c1) { t = std::clamp(t, 0.0f, 1.0f); float c3 = c1 + 1, u = t - 1; return 1 + c3 * u * u * u + c1 * u * u; }
float BackOut(float t) { return BackOutK(t, 1.4f); }
float Lerp(float a, float b, float t) { return a + (b - a) * t; }
D2D1_RECT_F LerpRect(const D2D1_RECT_F& a, const D2D1_RECT_F& b, float t) {
    return D2D1::RectF(Lerp(a.left, b.left, t), Lerp(a.top, b.top, t), Lerp(a.right, b.right, t), Lerp(a.bottom, b.bottom, t));
}

// ---------- Power pill ----------
constexpr float kSiriPowerPill = 48;

struct PowerChoice { PCWSTR label; int id; };
constexpr PowerChoice kPowerChoices[5] = {
    {L"Lock", kMenuLock},
    {L"Sleep", kMenuSleep},
    {L"Restart", kMenuRestart},
    {L"Shut down", kMenuShutdown},
    {L"Sign out", kMenuSignOut},
};

struct PowerKeyword { PCWSTR word; int choice; };
constexpr PowerKeyword kPowerKeywords[] = {
    {L"desligar", 3}, {L"shutdown", 3}, {L"shut down", 3}, {L"power", 3}, {L"energia", 3},
    {L"reiniciar", 2}, {L"restart", 2}, {L"reboot", 2},
    {L"suspender", 1}, {L"dormir", 1}, {L"sleep", 1},
    {L"bloquear", 0}, {L"lock", 0},
    {L"sair", 4}, {L"logoff", 4}, {L"logout", 4}, {L"sign out", 4},
};

bool g_powerPillWanted;
bool g_powerPillByButton;   // opened by the floating power button (not by typing)
float g_powerPillAnim;
int g_powerPillPick = 3, g_powerPillHover = -1, g_powerPillFocus = -1;

float PowerPillEase() {
    float t = std::clamp(g_powerPillAnim, 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

void ResetPowerPill() {
    g_powerPillWanted = false;
    g_powerPillByButton = false;
    g_powerPillAnim = 0;
    g_powerPillHover = g_powerPillFocus = -1;
}

int PowerChoiceForQuery(const std::wstring& query) {
    std::wstring q = ToLower(query);
    while (!q.empty() && q.back() == L' ') q.pop_back();
    for (const PowerKeyword& k : kPowerKeywords) {
        size_t len = wcslen(k.word);
        size_t need = std::min<size_t>(5, len);
        if (q.size() >= need && q.size() <= len && wcsncmp(k.word, q.c_str(), q.size()) == 0) return k.choice;
    }
    return -1;
}

// ---------- Siri pill intro & panel expansion ----------
constexpr float kIntroCircle = 0.45f;   // share of the intro spent as a circle (zoom in, settle)

// Bounce settings (0-100 %) scale the overshoot of the circle/pill and of the results panel
float PillBounce() { return g_settings.siriBounce / 100.0f * 2.8f; }
float PanelBounce() { return g_settings.siriPanelBounce / 100.0f * 2.0f; }

// The circle pops in a bit larger than the bar (zoom), settles back to the bar height, then the bar stretches
float CircleZoom() { return 1.0f + g_settings.siriCircleZoom / 100.0f; }
float PillIntroScale() {
    float t = std::clamp(g_pillIntro / kIntroCircle, 0.0f, 1.0f), zoom = CircleZoom();
    if (g_siriClosingPill) return 0.12f + 0.88f * t * t;   // closing: no zoom, sucked in (accelerating) and gone
    if (t < 0.6f) return 0.5f + (zoom - 0.5f) * BackOutK(t / 0.6f, PillBounce());
    return Lerp(zoom, 1.0f, Ease((t - 0.6f) / 0.4f));
}
float PillIntroWidth() {
    float t = (g_pillIntro - kIntroCircle) / (1 - kIntroCircle);
    return g_siriClosingPill ? Ease(t) : BackOutK(t, PillBounce());   // closing collapses without bouncing
}
float PillIntroAlpha() { return Ease(g_pillIntro / kIntroCircle); }
float PillContentAlpha() { return std::clamp((g_pillIntro - 0.75f) / 0.25f, 0.0f, 1.0f); }
float SiriExpandEase() { return BackOutK(g_siriAnim, PanelBounce()); }
float SiriContentAlpha() { return std::clamp((g_siriAnim - 0.55f) / 0.45f, 0.0f, 1.0f); }

// ---------- Files tab list mode ----------
bool g_histClearHover;
bool ListMode() {
    if (g_openFolderIndex >= 0) return false;
    if (g_siriTab == 3 || g_siriTab == 2) return true;   // Actions and the clipboard are always a list
    return g_siriTab == 1 && g_settings.filesEnabled && (g_query.empty() || g_filesList);
}
bool HistoryHeader() { return ListMode() && g_siriTab == 1 && g_query.empty() && g_historyEnabled && !g_searchHistory.empty(); }

// Segoe Fluent Icons / Segoe MDL2 Assets glyphs
PCWSTR ActionGlyph(const std::wstring& id) {
    if (id == L"lock") return L"\uE72E";
    if (id == L"sleep") return L"\uE708";
    if (id == L"restart") return L"\uE777";
    if (id == L"shutdown") return L"\uE7E8";
    if (id == L"signout") return L"\uF3B1";
    if (id == L"settings") return L"\uE713";
    if (id == L"taskmgr") return L"\uE9D9";
    if (id == L"explorer") return L"\uEC50";
    if (id == L"control") return L"\uE80F";
    if (id == L"screenshot") return L"\uE722";
    return L"\uE74D";   // recycle bin
}
PCWSTR TabGlyph(int tab) { return tab == 0 ? L"\uE71D" : tab == 1 ? L"\uE8B7" : tab == 3 ? L"\uE945" : L"\uE77F"; }

constexpr float kSiriGap = 10, kSiriPad = 14, kSiriTabs = 32;

float SiriIconPx(float s) { return std::round(std::min(g_settings.iconSize, 56) * s); }
float SiriCellHeight(float s) { return std::round(SiriIconPx(s) + std::round(18 * s) + 30 * s); }

// Room above the pill so the zoomed circle (bigger than the bar) is never cut at the window top
float SiriTopPad(float s) {
    float overshoot = 1.0f + 0.2f * g_settings.siriBounce / 100.0f;
    float biggest = CircleZoom() * overshoot;
    return std::ceil(g_settings.siriPillH * s * std::max(0.0f, biggest - 1.0f) / 2 + 2 * s);
}

float SiriShadowMargin(float s) { return ThemeShadowMargin(s); }

// Window mode with a ready-made theme draws the glass slightly inside the window, leaving room for its shadow
float WindowInset(float s) {
    if (g_settings.siri || g_settings.fullscreen || ActiveTheme() == Theme::Custom) return 0.0f;
    return std::max(std::round(16 * s), ThemeShadowMargin(s) * 0.6f);
}

float SiriButtonSize(float s) { return std::round(g_settings.siriPillH * s * 0.86f); }
float SiriButtonGap(float s) { return std::round(10 * s); }

int SiriWindowHeight(float s) {
    float pillH = std::round(g_settings.siriPillH * s), gap = std::round(kSiriGap * s), pad = std::round(kSiriPad * s);
    float tabsBlock = TabsVisible() || g_settings.siriOriginal ? std::round(kSiriTabs * s) + std::round(pad * 0.6f) : 0;
    float h = SiriTopPad(s) + pillH + gap + pad + tabsBlock + g_settings.siriRows * SiriCellHeight(s) + pad;
    if (g_settings.siriPower) h += std::round(kSiriPowerPill * s) + gap;
    h += SiriShadowMargin(s);
    h += g_settings.siriPanelBounce / 100.0f * 0.14f * (h - pillH);   // room for the panel overshoot
    return (int)std::ceil(h) + 2;
}

void ComputeSiriLayout() {
    Layout& L = g_layout;
    float s = L.scale, W = (float)L.width, H = (float)L.height;
    float pillW = std::min(W, std::round(g_settings.siriPillW * s));
    float pillH = std::round(g_settings.siriPillH * s);
    float panelW = std::min(W, std::round(g_settings.siriPanelW * s));
    float panelLeft = std::round((W - panelW) / 2), panelRight = panelLeft + panelW;
    float gap = std::round(kSiriGap * s), pad = std::round(kSiriPad * s), tabsH = std::round(kSiriTabs * s);

    {
        float d = pillH * PillIntroScale();
        float curW = std::clamp(d + (pillW - pillH) * PillIntroWidth(), d, W);
        float cx = W / 2, cy = SiriTopPad(s) + pillH / 2;
        L.search = D2D1::RectF(cx - curW / 2, cy - d / 2, cx + curW / 2, cy + d / 2);
    }
    L.btnPower = L.btnBack = L.panel = L.dots = D2D1::RectF(0, 0, 0, 0);
    if (g_settings.siriPowerButton && g_siriBtnAnim > 0.001f) {
        // Slides out from behind the right end of the bar and floats next to it
        float btn = SiriButtonSize(s), cy = SiriTopPad(s) + pillH / 2;
        float start = W / 2 + pillW / 2 - btn, end = W / 2 + pillW / 2 + SiriButtonGap(s);
        float left = Lerp(start, end, BackOutK(g_siriBtnAnim, 1.2f));
        L.btnPower = D2D1::RectF(left, cy - btn / 2, left + btn, cy + btn / 2);
    }
    L.icon = SiriIconPx(s);
    L.labelHeight = std::round(18 * s);
    L.cellPad = std::round(6 * s);
    L.cellHeight = SiriCellHeight(s);

    float available = std::max(L.icon * 2, panelW - 2 * pad);
    float minCell = L.icon + 34 * s;
    L.columns = g_settings.columns > 0 ? g_settings.columns : std::clamp((int)(available / minCell), 3, 10);
    g_recommendedCount = L.columns;
    L.cellWidth = std::floor(available / L.columns);
    L.gridLeft = std::round(panelLeft + (panelW - L.cellWidth * L.columns) / 2);
    // Keep Siri Apps navigation consistent with the configured Launchpad page mode.
    // List-based results (Files/History) remain vertically scrolling.
    L.horizontal = !L.list && g_siriTab == 0 && (g_settings.siriHorizontal || g_settings.horizontal);
    L.dotsH = std::round(std::clamp(std::min(W, H) * 0.028f, 16 * s, 28 * s));

    float e = PowerPillEase();
    float powerH = std::round(kSiriPowerPill * s);
    float push = e * (powerH + gap);
    L.siriPower = D2D1::RectF(0, 0, 0, 0);
    float pillBottom = SiriTopPad(s) + pillH;
    if (e > 0.001f) L.siriPower = D2D1::RectF(panelLeft, pillBottom + gap * e, panelRight, pillBottom + gap * e + powerH * e);

    float panelTop = pillBottom + gap + push;
    if (SiriStripVisible()) {
        float tabsW = std::min(panelW - 2 * pad, g_settings.siriOriginal ? FiltersNaturalWidth() : TabsNaturalWidth());
        float tabsLeft = std::round((W - tabsW) / 2);
        L.siriTabs = D2D1::RectF(tabsLeft, panelTop + pad, tabsLeft + tabsW, panelTop + pad + tabsH);
        L.viewTop = L.siriTabs.bottom + std::round(pad * 0.6f);
    } else {
        L.siriTabs = D2D1::RectF(0, 0, 0, 0);
        L.viewTop = panelTop + pad;
    }
    L.visibleRows = std::max(1, g_settings.siriRows);
    L.list = ListMode(); L.listHeader = D2D1::RectF(0, 0, 0, 0);
    if (L.list) {
        float viewportH = g_settings.siriRows * SiriCellHeight(s), rowH = std::round(44 * s);
        float hdr = HistoryHeader() ? std::round(30 * s) : 0;
        if (hdr > 0) { L.listHeader = D2D1::RectF(panelLeft + pad, L.viewTop, panelRight - pad, L.viewTop + hdr); L.viewTop += hdr; }
        L.icon = std::round(30 * s); L.cellPad = std::round(3 * s); L.columns = 1;
        L.cellWidth = std::floor(panelW - 2 * pad); L.gridLeft = std::round(panelLeft + pad); L.cellHeight = rowH;
        L.visibleRows = std::max(1, (int)((viewportH - hdr) / rowH));
    }
    // Reserve a bottom strip for the same clickable pill used by Window mode.
    L.viewBottom = L.viewTop + L.visibleRows * L.cellHeight;
    L.viewBottom = std::min(L.viewBottom, H - L.dotsH - 18 * s);

    int count = (int)g_layoutItems.size();
    int rows = (count + L.columns - 1) / L.columns;
    L.pageWidth = L.horizontal ? W : std::max(1.0f, L.cellHeight * L.visibleRows);
    L.perPage = std::max(1, L.visibleRows * L.columns);
    L.pages = std::max(1, (count + L.perPage - 1) / L.perPage);
    L.contentHeight = L.horizontal ? 0.0f : rows * L.cellHeight;
    L.dots = D2D1::RectF(0, 0, 0, 0);
    if (L.pages > 1 && (L.horizontal || L.contentHeight > (L.viewBottom - L.viewTop))) {
        float h = L.dotsH, padDots = h * 0.45f, gapDots = h * 0.8f;
        L.dotGap = gapDots;
        if (L.horizontal) {
            float maxW = W * 0.6f;
            if (L.pages * gapDots + 2 * padDots > maxW)
                gapDots = std::max(h * 0.35f, (maxW - 2 * padDots) / L.pages);
            L.dotGap = gapDots;
            float w = L.pages * gapDots + 2 * padDots;
            float cx = (panelLeft + panelRight) / 2;
            float top = std::round(L.viewBottom + 4 * s);
            L.dots = D2D1::RectF(std::round(cx - w / 2), top, std::round(cx + w / 2), top + h);
        } else {
            float height = L.pages * gapDots + 2 * padDots;
            float x = std::round(panelRight - h - 8 * s);
            float y = std::round(L.viewTop + ((L.viewBottom - L.viewTop) - height) / 2);
            L.dots = D2D1::RectF(x, y, x + h, y + height);
        }
    }

    int shownRows = std::clamp(rows, 1, L.visibleRows);
    float contentBottom = L.viewTop + shownRows * L.cellHeight + pad;
    if (L.horizontal && L.dots.bottom > L.dots.top) contentBottom = L.dots.bottom + pad * 0.5f;
    D2D1_RECT_F full = D2D1::RectF(panelLeft, panelTop, panelRight, contentBottom);
    float ex = SiriExpandEase();
    L.siriPanel = ex > 0.001f ? LerpRect(L.search, full, ex) : D2D1::RectF(0, 0, 0, 0);
    L.siriPanel.left = std::max(L.siriPanel.left, 0.0f);
    L.siriPanel.right = std::min(L.siriPanel.right, W);

    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
    g_scroll = std::clamp(g_scroll, 0.0f, MaxScroll());
}

void ComputeLayout() {
    if (g_settings.siri) { ComputeSiriLayout(); return; }
    g_layout.siriPanel = g_layout.siriPower = g_layout.siriTabs = D2D1::RectF(0, 0, 0, 0);
    Layout& L = g_layout; float s = L.scale; float W = (float)L.width, H = (float)L.height;
    float padding = 40 * s;
    bool inFolder = g_openFolderIndex >= 0;
    float bottomLimit;

    if (inFolder) {
        float margin = 24 * s;
        L.panel = D2D1::RectF(margin, 72 * s, W - margin, H - margin);
        L.btnBack = D2D1::RectF(padding, 24 * s, padding + 32 * s, 24 * s + 32 * s);
        L.search = D2D1::RectF(0, 0, 0, 0);
        L.btnPower = D2D1::RectF(0, 0, 0, 0);
        L.viewTop = L.panel.top + 16 * s;
        bottomLimit = L.panel.bottom - 16 * s;
    } else {
        L.panel = D2D1::RectF(0, 0, 0, 0);
        L.btnBack = D2D1::RectF(0, 0, 0, 0);
        bool power = g_settings.showPower, right = g_settings.powerRight;
        float pw = std::round(36 * s);
        float powerLeft = 0;
        if (g_settings.searchBox) {
            float gap = std::round(8 * s);
            float reserve = power ? 2 * (pw + gap) : 0;
            float sw = std::max(120 * s, std::min(320 * s, W - 2 * padding - reserve));
            float top = std::round(std::max(28 * s, H * 0.045f));
            L.search = D2D1::RectF(std::round((W - sw) / 2), top, std::round((W + sw) / 2), top + std::round(36 * s));
            powerLeft = right ? L.search.right + gap : L.search.left - gap - pw;
        } else {
            float reserve = power ? pw + 12 * s : 0;
            float left = power && !right ? padding + reserve : padding;
            float rightEdge = power && right ? W - padding - reserve : W - padding;
            L.search = D2D1::RectF(left, 24 * s, rightEdge, 24 * s + 40 * s);
            powerLeft = right ? W - padding - pw : padding;
        }
        float cy = (L.search.top + L.search.bottom) / 2;
        powerLeft = std::round(powerLeft);
        L.btnPower = power
            ? D2D1::RectF(powerLeft, std::round(cy - pw / 2), powerLeft + pw, std::round(cy - pw / 2) + pw)
            : D2D1::RectF(0, 0, 0, 0);

        // Category strip slides in under the search bar (only when more than Apps is enabled)
        float te = TabsVisible() ? Ease(g_tabsAnim) : 0.0f, extra = 0;
        if (te > 0.001f) {
            float tabsH = std::round(kSiriTabs * s), gap = std::round(14 * s);
            float tw = std::min(W - 2 * padding, TabsNaturalWidth());
            float tl = std::round((W - tw) / 2), tt = L.search.bottom + std::round(gap * te);
            L.siriTabs = D2D1::RectF(tl, tt, tl + tw, tt + tabsH);
            extra = (tabsH + gap) * te;
        }
        L.viewTop = L.search.bottom + std::round(30 * s) + std::round(extra);
        bottomLimit = H - padding;
    }

    L.icon = std::round(g_settings.iconSize * s);
    L.labelHeight = std::round(18 * s);
    L.cellPad = std::round(6 * s);
    L.cellHeight = std::round(L.icon + L.labelHeight + 45 * s);

    float availableWidth = inFolder ? (L.panel.right - L.panel.left) - 40 * s : W - 2 * padding;
    float available = std::max(L.icon * 2, availableWidth);
    float minCell = std::max(L.icon * 1.8f, L.icon + 64 * s);

    L.columns = g_settings.columns > 0 ? g_settings.columns : std::clamp((int)(available / minCell), 2, 14);
    L.cellWidth = std::floor(std::min(available / L.columns, std::max(minCell, L.icon * 2.6f)));
    L.gridLeft = inFolder ? std::round(L.panel.left + 20 * s) : std::round((W - L.cellWidth * L.columns) / 2);

    L.list = !inFolder && ListMode(); L.listHeader = D2D1::RectF(0, 0, 0, 0);
    if (L.list) {
        float listW = std::min(availableWidth, std::round(720 * s)), hdr = HistoryHeader() ? std::round(30 * s) : 0;
        L.gridLeft = std::round((W - listW) / 2);
        if (hdr > 0) { L.listHeader = D2D1::RectF(L.gridLeft, L.viewTop, L.gridLeft + listW, L.viewTop + hdr); L.viewTop += hdr; }
        L.icon = std::round(32 * s); L.cellPad = std::round(3 * s); L.columns = 1;
        L.cellWidth = std::floor(listW); L.cellHeight = std::round(46 * s);
    }
    L.horizontal = (g_settings.siri ? g_settings.siriHorizontal : g_settings.horizontal) && !L.list;
    L.dotsH = std::round(std::clamp(std::min(W, H) * 0.028f, 16 * s, 28 * s));
    // Reserve room for the unified bottom pill indicator in both scroll directions.
    bottomLimit -= L.dotsH + 18 * s;
    L.visibleRows = std::max(1, (int)((bottomLimit - L.viewTop) / L.cellHeight));
    L.viewBottom = L.viewTop + L.visibleRows * L.cellHeight;

    int count = (int)g_layoutItems.size();
    L.pageWidth = L.horizontal ? (inFolder ? (L.panel.right - L.panel.left) : W) : (L.cellHeight * L.visibleRows);
    L.perPage = std::max(1, L.visibleRows * L.columns);
    L.pages = std::max(1, (count + L.perPage - 1) / L.perPage);
    L.contentHeight = L.horizontal ? 0 : ((count + L.columns - 1) / L.columns) * L.cellHeight;

    L.dots = D2D1::RectF(0, 0, 0, 0);
    if (L.pages > 1 && (L.horizontal || L.contentHeight > (L.viewBottom - L.viewTop))) {
        float h = L.dotsH, pad = h * 0.45f, gap = h * 0.8f, maxW = L.pageWidth * 0.6f;
        if (L.horizontal && L.pages * gap + 2 * pad > maxW)
            gap = std::max(h * 0.35f, (maxW - 2 * pad) / L.pages);
        L.dotGap = gap;
        float extent = L.pages * gap + 2 * pad;
        if (L.horizontal) {
            float cx = inFolder ? (L.panel.left + L.panel.right) / 2 : W / 2;
            float top = std::round(L.viewBottom + 9 * s);
            L.dots = D2D1::RectF(std::round(cx - extent / 2), top, std::round(cx + extent / 2), top + h);
        } else {
            float right = inFolder ? L.panel.right : W - WindowInset(s);
            float x = std::round(right - h - 10 * s);
            float y = std::round(L.viewTop + ((L.viewBottom - L.viewTop) - extent) / 2);
            L.dots = D2D1::RectF(x, y, x + h, y + extent);
        }
    }
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll()); g_scroll = std::clamp(g_scroll, 0.0f, MaxScroll());
}

struct Cell { D2D1_RECT_F highlight, icon, label; };
Cell CellAtIn(const Layout& L, float scroll, int index) {
    if (L.list) {
        float lx = L.gridLeft, ly = L.viewTop + index * L.cellHeight - std::round(scroll);
        Cell c;
        c.highlight = D2D1::RectF(lx, ly, lx + L.cellWidth, ly + L.cellHeight - L.cellPad * 2);
        float hh = c.highlight.bottom - c.highlight.top;
        float il = std::round(lx + 10 * L.scale), it = std::round(ly + (hh - L.icon) / 2);
        c.icon = D2D1::RectF(il, it, il + L.icon, it + L.icon);
        c.label = D2D1::RectF(c.icon.right + 12 * L.scale, ly, lx + L.cellWidth - 12 * L.scale, ly + hh);
        return c;
    }
    float x, y;
    if (L.horizontal) {
        int page = index / L.perPage, local = index % L.perPage;
        x = L.gridLeft + page * L.pageWidth + (local % L.columns) * L.cellWidth - std::round(scroll);
        y = L.viewTop + (local / L.columns) * L.cellHeight;
    } else {
        x = L.gridLeft + (index % L.columns) * L.cellWidth;
        y = L.viewTop + (index / L.columns) * L.cellHeight - std::round(scroll);
    }
    Cell c;
    c.highlight = D2D1::RectF(x + L.cellPad, y, x + L.cellWidth - L.cellPad, y + L.cellHeight - L.cellPad * 2);
    float iconLeft = std::round(x + (L.cellWidth - L.icon) / 2), iconTop = std::round(y + 8 * L.scale);
    c.icon = D2D1::RectF(iconLeft, iconTop, iconLeft + L.icon, iconTop + L.icon);
    float labelTop = c.icon.bottom + std::round(8 * L.scale);
    c.label = D2D1::RectF(x + L.cellPad + 4 * L.scale, labelTop, x + L.cellWidth - L.cellPad - 4 * L.scale, labelTop + L.labelHeight);
    return c;
}
Cell CellAt(int index) { return CellAtIn(g_layout, g_scroll, index); }

bool InRect(const D2D1_RECT_F& r, float x, float y) { return x >= r.left && x < r.right && y >= r.top && y < r.bottom; }

D2D1_RECT_F ClearButtonRect(const Layout& L) {
    float size = 18 * L.scale;
    float inset = g_settings.siri ? 30 : (g_settings.searchBox ? 20 : 16);
    float cx = L.search.right - inset * L.scale;
    float cy = (L.search.top + L.search.bottom) / 2;
    return D2D1::RectF(cx - size / 2, cy - size / 2, cx + size / 2, cy + size / 2);
}

D2D1_RECT_F HistClearRect(const Layout& L) {
    float size = 28 * L.scale, cy = (L.listHeader.top + L.listHeader.bottom) / 2;
    return D2D1::RectF(L.listHeader.right - size - 4 * L.scale, cy - size / 2, L.listHeader.right - 4 * L.scale, cy + size / 2);
}

bool HitHistClear(float x, float y) {
    const Layout& L = g_layout;
    if (!L.list || L.listHeader.right <= L.listHeader.left) return false;
    if (g_settings.siri && g_siriAnim < 0.99f) return false;
    return InRect(HistClearRect(L), x, y);
}

// Original Siri: the picked category's icon in the bar (a click goes back to "Search", like Esc)
D2D1_RECT_F BackButtonRect(const Layout& L) {
    float s = L.scale, cx = L.search.left + 30 * s, cy = (L.search.top + L.search.bottom) / 2, r = 17 * s;
    return D2D1::RectF(cx - r, cy - r, cx + r, cy + r);
}
bool HitBack(float x, float y) {
    if (!g_settings.siriOriginal || !g_siriChosen || g_flyAnim < 1.0f || g_openFolderIndex >= 0) return false;
    return InRect(BackButtonRect(g_layout), x, y);
}
// ...and clicking the empty text next to it reopens the categories to switch
bool HitChosenText(float x, float y) {
    if (!g_settings.siriOriginal || !g_siriChosen || !g_query.empty() || g_flyAnim < 1.0f || g_openFolderIndex >= 0) return false;
    return InRect(g_layout.search, x, y) && !HitBack(x, y);
}

bool HitClear(float x, float y) {
    if (g_openFolderIndex >= 0 || g_query.empty()) return false;
    D2D1_RECT_F r = ClearButtonRect(g_layout);
    float m = 5 * g_layout.scale;
    return InRect(D2D1::RectF(r.left - m, r.top - m, r.right + m, r.bottom + m), x, y);
}

int PowerChoiceAt(float x, float y) {
    if (!g_settings.siri || !g_powerPillWanted || g_powerPillAnim < 0.95f) return -1;
    const D2D1_RECT_F& r = g_layout.siriPower;
    if (!InRect(r, x, y)) return -1;
    int index = (int)((x - r.left) / ((r.right - r.left) / 5));
    return std::clamp(index, 0, 4);
}

// Category strip hit test (all modes) -> returns the tab id (0 Apps, 1 Files, 2 Clipboard)
int TabAt(float x, float y) {
    if (g_openFolderIndex >= 0 || !TabsVisible() || g_settings.siriOriginal) return -1;
    if (g_settings.siri ? g_siriAnim < 0.95f : g_tabsAnim < 0.95f) return -1;
    const D2D1_RECT_F& r = g_layout.siriTabs;
    if (r.right <= r.left || !InRect(r, x, y)) return -1;
    for (int i = 0; i < g_tabCount; i++) {
        float left, width;
        TabSlotRect(r, i, left, width);
        if (x >= left && x < left + width) return g_tabIds[i];
    }
    return -1;
}

D2D1_RECT_F TitleRect(const Layout& L) {
    float w = 360 * L.scale, cx = L.width / 2.0f;
    return D2D1::RectF(cx - w / 2, L.btnBack.top, cx + w / 2, L.btnBack.bottom);
}

int ItemAt(int px, int py) {
    if (g_settings.siri && g_siriAnim < 0.99f) return -1;
    const Layout& L = g_layout; float x = (float)px, y = (float)py;
    if (y < L.viewTop || y >= L.viewBottom) return -1;
    int index;
    if (L.horizontal) {
        float vx = x + std::round(g_scroll) - L.gridLeft;
        int page = (int)std::floor(vx / L.pageWidth);
        float lx = vx - page * L.pageWidth;
        if (page < 0 || lx < 0 || lx >= L.cellWidth * L.columns) return -1;
        index = page * L.perPage + (int)((y - L.viewTop) / L.cellHeight) * L.columns + (int)(lx / L.cellWidth);
    } else {
        if (x < L.gridLeft || x >= L.gridLeft + L.cellWidth * L.columns) return -1;
        int row = (int)((y - L.viewTop + std::round(g_scroll)) / L.cellHeight);
        if (row < 0) return -1;
        index = row * L.columns + (int)((x - L.gridLeft) / L.cellWidth);
    }
    if (index < 0 || index >= (int)g_layoutItems.size() || !InRect(CellAt(index).highlight, x, y)) return -1;
    return index;
}

////////////////////////////////////////////////////////////////////////////////
// States and Drawing

HANDLE g_frameWaitTimer;
bool g_shown, g_closing; float g_appear; double g_lastFrame, g_lastClosed; bool g_frameTimer;
int g_hover = -1, g_selected = -1, g_pressed = -1; bool g_pressedEmpty, g_trackingMouse;
bool g_longPressTriggered = false;
float g_dragX = -1, g_dragY = -1; int g_dragItem = -1; bool g_isDragging = false;
bool g_powerHover;
bool g_clearHover;
bool g_backHover;    // Original Siri: the category icon in the bar turns into a back arrow
bool g_ignoreUp;     // the button-down already acted (it opened the categories menu)

// The power button can be used (Window/Fullscreen: shown; Siri: out next to the bar)
bool PowerButtonOn() { return g_settings.siri ? g_settings.siriPowerButton && g_siriBtnAnim > 0.9f : g_settings.showPower; }

bool FolderSettled() { return g_openFolderIndex < 0 || (!g_folderClosing && g_folderAnim >= 1.0f); }

float g_slideDistance;
struct AnimFx { D2D1::Matrix3x2F m = D2D1::Matrix3x2F::Identity(); float alpha = 1, flash = 0; };

AnimFx ComputeAnim() {
    AnimFx fx;
    if (g_settings.siri) return fx;
    const Layout& L = g_layout; float W = (float)L.width, H = (float)L.height, s = L.scale;
    D2D1_POINT_2F c = D2D1::Point2F(W / 2, H / 2);
    float p = std::clamp(g_appear, 0.0f, 1.0f);
    switch (g_closing ? g_settings.closeAnim : g_settings.openAnim) {
        case Anim::Fade: fx.alpha = p * p * (3 - 2 * p); break;
        case Anim::Zoom: {
            float e = g_closing ? Ease(p) : BackOut(p), k = 0.86f + 0.14f * e;
            fx.m = D2D1::Matrix3x2F::Scale(k, k, c); fx.alpha = std::min(1.0f, p * 2.2f); break;
        }
        case Anim::Slide: {
            float e = Ease(p), k = 0.92f + 0.08f * e;
            fx.m = D2D1::Matrix3x2F::Scale(k, k, D2D1::Point2F(W / 2, H)) * D2D1::Matrix3x2F::Translation(0, (1 - e) * g_slideDistance);
            fx.alpha = std::min(1.0f, p * 2.5f); break;
        }
        case Anim::Crt: {
            float minY = std::max(2 * s / H, 0.003f), sx, sy;
            if (p < 0.45f) { sx = Lerp(0.02f, 1.0f, Ease(p / 0.45f)); sy = minY; }
            else { sx = 1; sy = Lerp(minY, 1.0f, Ease((p - 0.45f) / 0.55f)); }
            fx.m = D2D1::Matrix3x2F::Scale(sx, sy, c);
            fx.flash = 0.85f * (1 - Ease(p));
            fx.alpha = std::min(1.0f, p * 12); break;
        }
    }
    return fx;
}

// ---------- Context menu ----------
struct MenuItem { std::wstring text; int id = 0; bool separator = false; bool danger = false; PCWSTR glyph = nullptr; PCWSTR hint = nullptr; };
struct PopupMenu {
    bool open = false, closing = false; float anim = 0; D2D1_RECT_F rect{}; D2D1_POINT_2F origin{};
    std::vector<MenuItem> items; int hover = -1, folder = -1, appIdx = -1, folderApp = -1; float itemH = 32, sepH = 9;
    bool category = false;   // Original Siri: the categories menu shown when hovering the bar
    bool keyboard = false;   // opened by Esc: stays until the mouse has been over it once
};
[[clang::no_destroy]] PopupMenu g_menu;

float MenuItemTop(int i) {
    float y = g_menu.rect.top + (g_menu.category ? 8 : 6) * g_layout.scale;
    for (int k = 0; k < i; k++) y += g_menu.items[k].separator ? g_menu.sepH : g_menu.itemH;
    return y;
}

int MenuHit(float x, float y) {
    if (!g_menu.open || g_menu.closing || !InRect(g_menu.rect, x, y)) return -1;
    for (int i = 0; i < (int)g_menu.items.size(); i++) {
        if (g_menu.items[i].separator) continue;
        float t = MenuItemTop(i);
        if (y >= t && y < t + g_menu.itemH) return i;
    }
    return -1;
}

void DrawThemeShadow(const D2D1_RECT_F& r, float radius, float strength, float spread, float dy);
ComPtr<ID2D1LinearGradientBrush> MakeGradient(const D2D1_GRADIENT_STOP* stops, UINT32 count, D2D1_POINT_2F from, D2D1_POINT_2F to);

// Raised liquid-glass button: soft shadow, translucent body, light caught on the top edge and a bright beveled rim.
// level > 1 makes it stand out more (hover / selected).
void DrawGlassButton(const D2D1_RECT_F& r, float radius, float a, float level = 1.0f, bool shadow = true) {
    if (a <= 0.001f || r.right - r.left < 2 || r.bottom - r.top < 2) return;
    float s = g_layout.scale, h = r.bottom - r.top;
    radius = std::min(radius, std::min(r.right - r.left, h) / 2);
    if (shadow) DrawThemeShadow(r, radius, 0.20f * a, 5 * s, 2.5f * s);
    float base = g_settings.darkText ? 0.30f : 0.10f;   // light themes need a whiter body
    D2D1_GRADIENT_STOP body[3] = {
        {0.0f, D2D1::ColorF(1, 1, 1, std::min(1.0f, (base + 0.12f) * level))},
        {0.55f, D2D1::ColorF(1, 1, 1, std::min(1.0f, base * 0.5f * level))},
        {1.0f, D2D1::ColorF(1, 1, 1, std::min(1.0f, (base + 0.04f) * level))},
    };
    if (auto fill = MakeGradient(body, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
        fill->SetOpacity(a);
        g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), fill.Get());
    }
    // Light caught on the upper half
    float inset = 1.5f * s;
    D2D1_RECT_F top = D2D1::RectF(r.left + inset, r.top + inset, r.right - inset, r.top + h * 0.55f);
    D2D1_GRADIENT_STOP glow[2] = {{0.0f, D2D1::ColorF(1, 1, 1, std::min(1.0f, 0.22f * level))}, {1.0f, D2D1::ColorF(1, 1, 1, 0.0f)}};
    if (auto shine = MakeGradient(glow, 2, D2D1::Point2F(0, top.top), D2D1::Point2F(0, top.bottom))) {
        shine->SetOpacity(a);
        float tr = std::max(0.0f, radius - inset);
        g_target->FillRoundedRectangle(D2D1::RoundedRect(top, tr, tr), shine.Get());
    }
    // Beveled rim: bright on the top-left and bottom-right corners
    D2D1_GRADIENT_STOP rimStops[4] = {
        {0.0f, D2D1::ColorF(1, 1, 1, 0.85f)}, {0.35f, D2D1::ColorF(1, 1, 1, 0.18f)},
        {0.65f, D2D1::ColorF(1, 1, 1, 0.12f)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.55f)},
    };
    if (auto rim = MakeGradient(rimStops, 4, D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.right, r.bottom))) {
        float bw = std::max(1.0f, 1.1f * s), half = bw / 2, rr = std::max(0.0f, radius - half);
        rim->SetOpacity(a);
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), rim.Get(), bw);
        float in2 = bw + 1.2f * s, ri = std::max(0.0f, radius - in2);
        rim->SetOpacity(0.25f * a);
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + in2, r.top + in2, r.right - in2, r.bottom - in2), ri, ri), rim.Get(), bw * 0.8f);
    }
}

// Discreet capsule: a faint tint and a hairline edge (selected filter, page dots)
void DrawSoftPill(const D2D1_RECT_F& r, float radius, float a, float tint) {
    if (a <= 0.001f || r.right - r.left < 2 || r.bottom - r.top < 2) return;
    float s = g_layout.scale, bw = std::max(1.0f, 0.8f * s), half = bw / 2;
    radius = std::min(radius, std::min(r.right - r.left, r.bottom - r.top) / 2);
    Fill(r, Fg(tint * a), radius);
    D2D1_GRADIENT_STOP edge[2] = {{0.0f, D2D1::ColorF(1, 1, 1, 0.22f)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.06f)}};
    if (auto rim = MakeGradient(edge, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
        rim->SetOpacity(a);
        float rr = std::max(0.0f, radius - half);
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), rim.Get(), bw);
    }
}

// Original Siri categories: the same glass as the bar, big icons, labels and keycap shortcuts (macOS style)
void DrawThemedShape(const D2D1_RECT_F& r, float radius, ID2D1Bitmap* backdrop, bool shadow);
void PushOpacity(float a);
D2D1_COLOR_F Alpha(D2D1_COLOR_F c, float a);
void DrawCategoryMenu(const D2D1_RECT_F& r, float rad, float e) {
    const Layout& L = g_layout;
    float s = L.scale;
    PushOpacity(e);
    if (ActiveTheme() != Theme::Custom) {
        DrawThemedShape(r, rad, BlurStrength() > 0 ? GetBackdropBitmap() : nullptr, true);
    } else {
        DrawThemeShadow(r, rad, 0.20f, 10 * s, 4 * s);
        Fill(r, Alpha(g_settings.siriPanelColor, std::max(0.55f, g_settings.siriPanelOpacity)), rad);
    }
    for (int i = 0; i < (int)g_menu.items.size(); i++) {
        const MenuItem& it = g_menu.items[i];
        float top = MenuItemTop(i);
        D2D1_RECT_F row = D2D1::RectF(r.left + 8 * s, top, r.right - 8 * s, top + g_menu.itemH);
        float cy = (row.top + row.bottom) / 2;
        if (i == g_menu.hover) Fill(row, Fg(0.11f), 14 * s);
        if (it.glyph && g_glyphLargeFormat) {
            g_brush->SetColor(Fg(0.88f));
            g_target->DrawText(it.glyph, (UINT32)wcslen(it.glyph), g_glyphLargeFormat.Get(),
                               D2D1::RectF(row.left + 8 * s, row.top, row.left + 44 * s, row.bottom), g_brush.Get());
        }
        float keyW = 0;
        if (it.hint && g_keyFormat) {
            keyW = 58 * s;
            D2D1_RECT_F key = D2D1::RectF(row.right - 12 * s - keyW, cy - 12 * s, row.right - 12 * s, cy + 12 * s);
            Fill(key, Fg(0.08f), 7 * s);
            g_brush->SetColor(Fg(0.55f));
            g_target->DrawText(it.hint, (UINT32)wcslen(it.hint), g_keyFormat.Get(), key, g_brush.Get());
        }
        if (g_categoryFormat) {
            g_brush->SetColor(Fg(0.95f));
            g_target->DrawText(it.text.c_str(), (UINT32)it.text.size(), g_categoryFormat.Get(),
                               D2D1::RectF(row.left + 54 * s, row.top, row.right - keyW - 24 * s, row.bottom), g_brush.Get());
        }
    }
    g_target->PopLayer();
}

void DrawPopupMenu(const D2D1::Matrix3x2F& base) {
    if (!g_menu.open) return;
    const Layout& L = g_layout; float s = L.scale, e = Ease(g_menu.anim), k = 0.92f + 0.08f * e;
    if (e <= 0.001f) return;
    g_target->SetTransform(D2D1::Matrix3x2F::Scale(k, k, g_menu.origin) * base);
    D2D1_RECT_F r = g_menu.rect; float rad = (g_menu.category ? 24 : 14) * s;
    if (g_menu.category) { DrawCategoryMenu(r, rad, e); return; }
    for (int i = 1; i <= 3; i++) {
        float g = i * 3 * s;
        Fill(D2D1::RectF(r.left - g, r.top - g + 5 * s, r.right + g, r.bottom + g + 5 * s), D2D1::ColorF(0, 0, 0, 0.06f * e), rad + g);
    }
    ComPtr<ID2D1RoundedRectangleGeometry> geo;
    g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, rad, rad), &geo);
    if (!geo) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geo.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), e), nullptr);
    if (BlurStrength() > 0) {
        if (ID2D1Bitmap* bmp = GetBackdropBitmap())
            g_target->DrawBitmap(bmp, D2D1::RectF(0, 0, (float)L.width, (float)L.height), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
    }
    Fill(r, ThemeMenuColor());
    Fill(r, D2D1::ColorF(1, 1, 1, 0.07f));
    for (int i = 0; i < (int)g_menu.items.size(); i++) {
        const MenuItem& it = g_menu.items[i]; float top = MenuItemTop(i);
        if (it.separator) {
            float y = std::round(top + g_menu.sepH / 2) + 0.5f;
            g_brush->SetColor(Fg(0.12f));
            g_target->DrawLine(D2D1::Point2F(r.left + 12 * s, y), D2D1::Point2F(r.right - 12 * s, y), g_brush.Get(), 1.0f);
            continue;
        }
        D2D1_RECT_F row = D2D1::RectF(r.left + 6 * s, top, r.right - 6 * s, top + g_menu.itemH);
        if (i == g_menu.hover) {
            if (it.danger) Fill(row, D2D1::ColorF(1, 0.3f, 0.3f, 0.20f), 8 * s);
            DrawGlassButton(row, 8 * s, 1.0f, 1.2f, false);
        }
        float textLeft = row.left + 10 * s;
        if (it.glyph && g_glyphFormat) {
            g_brush->SetColor(Fg(0.85f));
            g_target->DrawText(it.glyph, (UINT32)wcslen(it.glyph), g_glyphFormat.Get(), D2D1::RectF(row.left + 6 * s, row.top, row.left + 32 * s, row.bottom), g_brush.Get());
            textLeft = row.left + 38 * s;
        }
        if (g_menuFormat) {
            g_brush->SetColor(it.danger ? DangerInk() : Fg(0.92f));
            g_target->DrawText(it.text.c_str(), (UINT32)it.text.size(), g_menuFormat.Get(),
                D2D1::RectF(textLeft, row.top, row.right - 10 * s, row.bottom), g_brush.Get());
        }
        if (it.hint && g_hintFormat) {
            g_brush->SetColor(Fg(0.45f));
            g_target->DrawText(it.hint, (UINT32)wcslen(it.hint), g_hintFormat.Get(), D2D1::RectF(row.left, row.top, row.right - 12 * s, row.bottom), g_brush.Get());
        }
    }
    g_target->PopLayer();
    D2D1_GRADIENT_STOP rimStops[3] = {{0.0f, D2D1::ColorF(1, 1, 1, 0.55f)}, {0.5f, D2D1::ColorF(1, 1, 1, 0.12f)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.35f)}};
    if (auto rim = MakeGradient(rimStops, 3, D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.right, r.bottom))) {
        rim->SetOpacity(e);
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.6f, r.top + 0.6f, r.right - 0.6f, r.bottom - 0.6f), rad, rad), rim.Get(), 1.2f * s);
    }
}

// ---------- Multi-select ----------

App* AppOfMain(int idx) {
    if (g_openFolderIndex >= 0 || idx < 0 || idx >= (int)g_layoutItems.size()) return nullptr;
    int it = g_layoutItems[idx];
    return IsFolderItem(it) ? nullptr : ResolveApp(it);
}
App* SelectableApp(int idx) {
    App* a = AppOfMain(idx);
    int it = g_layoutItems[idx];
    return a && (it < 1000 || (Picking() && it >= kFolderAppBase && it < kTabBase)) ? a : nullptr;
}
bool IsSelected(int idx) { App* a = SelectableApp(idx); return a && g_selection.count(a); }

bool JiggleActive() {
    return (g_settings.jiggle || Picking()) && g_shown && !g_closing && g_openFolderIndex < 0 && !g_selection.empty();
}

D2D1::Matrix3x2F JiggleMatrix(const D2D1_RECT_F& icon, int index, float scale) {
    const double period = 0.26;
    double phase = std::fmod(Now(), period) / period * 6.2831853;
    float w = (float)phase + (index % 7) * 0.9f;
    float angle = 2.5f * std::sin(w);
    float lift = 0.8f * scale * std::sin(2 * w);
    D2D1_POINT_2F center = D2D1::Point2F((icon.left + icon.right) / 2, (icon.top + icon.bottom) / 2);
    return D2D1::Matrix3x2F::Rotation(angle, center) * D2D1::Matrix3x2F::Translation(0, lift);
}
bool GroupDrag() { return g_isDragging && g_dragItem >= 0 && g_selection.size() > 1 && IsSelected(g_dragItem); }

void DrawCheckBadge(const D2D1_RECT_F& icon, float a) {
    float s = g_layout.scale, r = 9 * s; D2D1_POINT_2F c = D2D1::Point2F(icon.right - 4 * s, icon.top + 4 * s);
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f * a)); g_target->FillEllipse(D2D1::Ellipse(c, r + 1.5f * s, r + 1.5f * s), g_brush.Get());
    g_brush->SetColor(D2D1::ColorF(0.04f, 0.52f, 1.0f, a)); g_target->FillEllipse(D2D1::Ellipse(c, r, r), g_brush.Get());
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, a));
    g_target->DrawLine(D2D1::Point2F(c.x - 4 * s, c.y), D2D1::Point2F(c.x - 1 * s, c.y + 3 * s), g_brush.Get(), 1.8f * s);
    g_target->DrawLine(D2D1::Point2F(c.x - 1 * s, c.y + 3 * s), D2D1::Point2F(c.x + 4.5f * s, c.y - 3.5f * s), g_brush.Get(), 1.8f * s);
}

void DrawGroupDrag(const D2D1_RECT_F& r, int item) {
    float s = g_layout.scale; App* lead = g_apps[item].get(); std::vector<App*> back;
    for (auto& a : g_apps) if (a.get() != lead && g_selection.count(a.get())) { back.push_back(a.get()); if (back.size() == 2) break; }
    for (int k = (int)back.size(); k >= 1; k--) {
        float o = k * 7 * s;
        if (ID2D1Bitmap* b = GetBitmap(*back[k - 1]))
            g_target->DrawBitmap(b, D2D1::RectF(r.left + o, r.top - o, r.right + o, r.bottom - o), 0.5f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }
    if (ID2D1Bitmap* b = GetBitmap(*lead)) g_target->DrawBitmap(b, r, 0.9f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    float br = 11 * s; D2D1_POINT_2F c = D2D1::Point2F(r.right - 2 * s, r.top + 2 * s);
    g_brush->SetColor(D2D1::ColorF(1.0f, 0.27f, 0.23f, 0.95f)); g_target->FillEllipse(D2D1::Ellipse(c, br, br), g_brush.Get());
    std::wstring t = std::to_wstring(g_selection.size());
    if (g_labelFormat) {
        g_brush->SetColor(D2D1::ColorF(1, 1, 1));
        g_target->DrawText(t.c_str(), (UINT32)t.size(), g_labelFormat.Get(), D2D1::RectF(c.x - br, c.y - br, c.x + br, c.y + br), g_brush.Get());
    }
}

// Pagination

void VisibleRange(const Layout& L, float scroll, int count, int& first, int& last) {
    if (L.horizontal) {
        int p = std::max(0, (int)std::floor(scroll / L.pageWidth));
        first = p * L.perPage; last = std::min(count, (p + 2) * L.perPage);
    } else {
        int firstRow = std::max(0, (int)(scroll / L.cellHeight));
        int lastRow = (int)((scroll + L.viewBottom - L.viewTop) / L.cellHeight) + 1;
        first = firstRow * L.columns; last = std::min(count, (lastRow + 1) * L.columns);
    }
}

float PageFade(const Layout& L, float scroll, int index) {
    if (!L.horizontal) return 1.0f;
    float off = std::abs(index / L.perPage - scroll / L.pageWidth);
    return 1.0f - 0.55f * std::min(1.0f, off);
}

int DotAt(float x, float y) {
    const Layout& L = g_layout;
    if (L.pages < 2 || (!L.horizontal && L.contentHeight <= (L.viewBottom - L.viewTop))) return -1;
    float m = 6 * L.scale;
    if (x < L.dots.left - m || x >= L.dots.right + m || y < L.dots.top - m || y >= L.dots.bottom + m) return -1;
    if (L.horizontal)
        return std::clamp((int)((x - L.dots.left - L.dotsH * 0.45f) / L.dotGap), 0, L.pages - 1);
    return std::clamp((int)((y - L.dots.top - L.dotsH * 0.45f) / L.dotGap), 0, L.pages - 1);
}

void DrawPageDots(const Layout& L, float scroll, float a) {
    if (L.pages < 2 || a <= 0.001f) return;
    float s = L.scale, h = L.dotsH, gap = L.dotGap, hv = EaseInOut(g_dotsHoverAnim);
    bool vertical = !L.horizontal;
    float grow = h * 0.10f * hv;
    D2D1_RECT_F r = D2D1::RectF(L.dots.left - grow, L.dots.top - grow, L.dots.right + grow, L.dots.bottom + grow);
    float rad = std::min(r.right - r.left, r.bottom - r.top) / 2;
    float cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    DrawSoftPill(r, rad, a * (0.35f + 0.65f * hv), 0.07f);   // barely there until the mouse comes near
    float dr = std::max(1.6f * s, std::min(h * 0.10f, gap * 0.24f));
    float pos = std::clamp(scroll / std::max(1.0f, L.pageWidth), 0.0f, (float)(L.pages - 1));
    float stretch = std::sin((pos - std::floor(pos)) * 3.14159265f);
    for (int i = 0; i < L.pages; i++) {
        float boost = i == g_dotHover ? hv : 0.0f, rr = dr * (1.0f + 0.25f * boost);
        g_brush->SetColor(Fg((0.22f + 0.30f * boost) * a));
        float px = vertical ? cx : L.dots.left + h * 0.45f + gap * 0.5f + i * gap;
        float py = vertical ? L.dots.top + h * 0.45f + gap * 0.5f + i * gap : cy;
        g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), rr, rr), g_brush.Get());
    }
    float active = dr * 2.8f + gap * 0.4f * stretch;
    if (vertical) {
        float ay = L.dots.top + h * 0.45f + gap * 0.5f + pos * gap;
        Fill(D2D1::RectF(cx - dr, ay - active / 2, cx + dr, ay + active / 2), Fg(0.82f * a), dr);
    } else {
        float ax = L.dots.left + h * 0.45f + gap * 0.5f + pos * gap;
        Fill(D2D1::RectF(ax - active / 2, cy - dr, ax + active / 2, cy + dr), Fg(0.82f * a), dr);
    }
}

D2D1_RECT_F QueryTextRect(const Layout& L) {
    float s = L.scale; bool siri = g_settings.siri;
    bool box = g_settings.searchBox && !siri;
    float reserve = g_query.empty() ? 20.0f : 38.0f;
    if (siri) reserve = g_query.empty() ? 30.0f : 62.0f;
    float textLeft = (siri ? 62.0f : (box ? 34.0f : 30.0f)) * s;
    return D2D1::RectF(L.search.left + textLeft, L.search.top,
                       L.search.right - reserve * s, L.search.bottom);
}

size_t QueryIndexAtPoint(const Layout& L, float x, float y) {
    if (g_query.empty() || !g_dwrite) return 0;
    IDWriteTextFormat* fmt = g_settings.siri && g_siriFormat ? g_siriFormat.Get() : g_searchFormat.Get();
    if (!fmt) return g_query.size();
    D2D1_RECT_F text = QueryTextRect(L);
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(g_dwrite->CreateTextLayout(g_query.c_str(), (UINT32)g_query.size(), fmt,
            10000.0f, text.bottom - text.top, &layout)) || !layout) return g_query.size();
    DWRITE_HIT_TEST_METRICS hit{}; BOOL trailing = FALSE, inside = FALSE;
    float shift = 0.0f; DWRITE_TEXT_METRICS metrics{};
    if (SUCCEEDED(layout->GetMetrics(&metrics))) {
        float room = text.right - text.left - 3.0f * L.scale;
        shift = std::max(0.0f, metrics.widthIncludingTrailingWhitespace - room);
    }
    layout->HitTestPoint(x - (text.left - shift), y - text.top, &trailing, &inside, &hit);
    size_t idx = (size_t)hit.textPosition + (trailing ? hit.length : 0);
    return std::min(idx, g_query.size());
}

void DrawSearchBar(const Layout& L, float a) {
    if (a <= 0.001f) return;
    float s = L.scale;
    const D2D1_RECT_F& r = L.search;
    bool siri = g_settings.siri;
    bool box = g_settings.searchBox && !siri;
    if (box) {
        float rad = (r.bottom - r.top) / 2;
        DrawGlassButton(r, rad, a, g_searchActive ? 1.35f : 1.0f);
    }
    float m = siri ? 1.3f : 1.0f;
    float cx = r.left + (siri ? 30 : (box ? 18 : 12)) * s, cy = (r.top + r.bottom) / 2;
    g_brush->SetColor(Fg(0.72f * a));

    bool original = g_settings.siriOriginal;
    bool flying = original && g_flyAnim < 1.0f;
    if (original) {
        // Original Siri: a magnifier until a category is picked, then that category's icon (hidden while it flies in)
        if (!g_siriChosen) {
            g_target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx - 1.5f * s, cy - 1.5f * s), 6.5f * s, 6.5f * s), g_brush.Get(), 1.6f * s);
            g_target->DrawLine(D2D1::Point2F(cx + 3.3f * s, cy + 3.3f * s), D2D1::Point2F(cx + 8 * s, cy + 8 * s), g_brush.Get(), 1.6f * s);
        } else if (!flying && g_backHover) {
            // Hovered: a soft round button with a back arrow
            D2D1_RECT_F b = BackButtonRect(L);
            float br = (b.right - b.left) / 2;
            DrawSoftPill(D2D1::RectF(b.left + 2 * s, b.top + 2 * s, b.right - 2 * s, b.bottom - 2 * s), br, a, 0.12f);
            g_brush->SetColor(Fg(0.9f * a));
            D2D1_POINT_2F p1 = D2D1::Point2F(cx + 2.5f * s, cy - 6 * s), p2 = D2D1::Point2F(cx - 3.5f * s, cy), p3 = D2D1::Point2F(cx + 2.5f * s, cy + 6 * s);
            g_target->DrawLine(p1, p2, g_brush.Get(), 1.9f * s);
            g_target->DrawLine(p2, p3, g_brush.Get(), 1.9f * s);
            g_target->FillEllipse(D2D1::Ellipse(p2, 0.95f * s, 0.95f * s), g_brush.Get());
        } else if (!flying && g_glyphLargeFormat) {
            PCWSTR glyph = TabGlyph(g_siriTab);
            g_target->DrawText(glyph, 1, g_glyphLargeFormat.Get(), D2D1::RectF(cx - 14 * s, cy - 14 * s, cx + 14 * s, cy + 14 * s), g_brush.Get());
        }
    } else if (siri) {
        // Mode-specific monochrome icons, placed immediately before the placeholder/query.
        // Apps = four rounded tiles; Files = document; Clipboard = clipboard sheet.
        float u = s * 1.0f;
        if (g_siriTab == 0) {
            const float d = 5.0f * u, gap = 2.2f * u;
            for (int row = 0; row < 2; ++row) {
                for (int col = 0; col < 2; ++col) {
                    float x = cx - d - gap * 0.5f + col * (d + gap);
                    float y = cy - d - gap * 0.5f + row * (d + gap);
                    g_target->FillRoundedRectangle(
                        D2D1::RoundedRect(D2D1::RectF(x, y, x + d, y + d), 1.2f * u, 1.2f * u),
                        g_brush.Get());
                }
            }
        } else if (g_siriTab == 3) {
            if (g_glyphFormat)
                g_target->DrawText(TabGlyph(3), 1, g_glyphFormat.Get(), D2D1::RectF(cx - 12 * u, cy - 12 * u, cx + 12 * u, cy + 12 * u), g_brush.Get());
        } else if (g_siriTab == 1) {
            // Files tab: small folder icon (tab), not a document/page.
            D2D1_POINT_2F folderPts[] = {
                D2D1::Point2F(cx - 7.0f*u, cy - 4.0f*u),
                D2D1::Point2F(cx - 2.0f*u, cy - 4.0f*u),
                D2D1::Point2F(cx + 0.0f*u, cy - 1.5f*u),
                D2D1::Point2F(cx + 7.0f*u, cy - 1.5f*u),
                D2D1::Point2F(cx + 6.0f*u, cy + 6.5f*u),
                D2D1::Point2F(cx - 7.0f*u, cy + 6.5f*u),
                D2D1::Point2F(cx - 7.0f*u, cy - 4.0f*u)
            };
            ComPtr<ID2D1PathGeometry> folderGeometry;
            ComPtr<ID2D1GeometrySink> folderSink;
            if (SUCCEEDED(g_d2d->CreatePathGeometry(&folderGeometry)) &&
                SUCCEEDED(folderGeometry->Open(&folderSink))) {
                folderSink->BeginFigure(folderPts[0], D2D1_FIGURE_BEGIN_HOLLOW);
                for (int p = 1; p < (int)(sizeof(folderPts) / sizeof(folderPts[0])); ++p)
                    folderSink->AddLine(folderPts[p]);
                folderSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                folderSink->Close();
                g_target->DrawGeometry(folderGeometry.Get(), g_brush.Get(), 1.5f*u);
            }
        } else {
            D2D1_RECT_F sheet = D2D1::RectF(cx - 6.0f*u, cy - 5.5f*u, cx + 6.0f*u, cy + 8.0f*u);
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(sheet, 1.5f*u, 1.5f*u), g_brush.Get(), 1.5f*u);
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - 3.5f*u, cy - 8.0f*u, cx + 3.5f*u, cy - 3.0f*u), 1.5f*u, 1.5f*u), g_brush.Get(), 1.5f*u);
            g_target->DrawLine(D2D1::Point2F(cx - 3.0f*u, cy - 0.5f*u), D2D1::Point2F(cx + 3.0f*u, cy - 0.5f*u), g_brush.Get(), 1.2f*u);
            g_target->DrawLine(D2D1::Point2F(cx - 3.0f*u, cy + 3.0f*u), D2D1::Point2F(cx + 3.0f*u, cy + 3.0f*u), g_brush.Get(), 1.2f*u);
        }
    } else {
        g_target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 5.5f * s * m, 5.5f * s * m), g_brush.Get(), 1.2f * s * m);
        g_target->DrawLine(D2D1::Point2F(cx + 4.0f * s * m, cy + 4.0f * s * m), D2D1::Point2F(cx + 8.0f * s * m, cy + 8.0f * s * m), g_brush.Get(), 1.2f * s * m);
    }

    if (!g_query.empty()) {
        D2D1_RECT_F cr = ClearButtonRect(L);
        D2D1_POINT_2F cc = D2D1::Point2F((cr.left + cr.right) / 2, (cr.top + cr.bottom) / 2);
        float crad = (cr.right - cr.left) / 2, arm = crad * 0.38f;
        float grow = 2 * s;
        DrawGlassButton(D2D1::RectF(cr.left - grow, cr.top - grow, cr.right + grow, cr.bottom + grow), crad + grow, a, g_clearHover ? 1.7f : 1.15f, false);
        g_brush->SetColor(Fg((g_clearHover ? 0.95f : 0.75f) * a));
        g_target->DrawLine(D2D1::Point2F(cc.x - arm, cc.y - arm), D2D1::Point2F(cc.x + arm, cc.y + arm), g_brush.Get(), 1.5f * s);
        g_target->DrawLine(D2D1::Point2F(cc.x - arm, cc.y + arm), D2D1::Point2F(cc.x + arm, cc.y - arm), g_brush.Get(), 1.5f * s);
    }

    float reserve = g_query.empty() ? 20 : 38;
    if (siri) reserve = g_query.empty() ? 30 : 62;
    float textLeft = (siri ? 62 : (box ? 34 : 30)) * s;
    D2D1_RECT_F text = D2D1::RectF(r.left + textLeft, r.top, r.right - reserve * s, r.bottom);
    IDWriteTextFormat* fmt = siri && g_siriFormat ? g_siriFormat.Get() : g_searchFormat.Get();
    if (!fmt) return;

    if (g_query.empty() && original) {
        if (!flying) {
            PCWSTR ph = Picking() ? Tr(L"Hold an app to select it") : g_siriChosen ? TabLabel(g_siriTab) : Tr(L"Search");
            g_brush->SetColor(Fg((g_siriChosen ? 0.62f : 0.4f) * a));
            g_target->DrawText(ph, (UINT32)wcslen(ph), fmt, text, g_brush.Get());
        }
    } else if (g_query.empty()) {
        PCWSTR ph = g_siriTab == 1 ? Tr(L"Search Files") : g_siriTab == 2 ? Tr(L"Search Clipboard") : g_siriTab == 3 ? Tr(L"Search Actions")
                  : siri ? Tr(L"Search Apps") : Tr(L"Search Applications");
        g_brush->SetColor(Fg(0.4f * a));
        g_target->DrawText(ph, (UINT32)wcslen(ph), fmt, text, g_brush.Get());
    }

    ComPtr<IDWriteTextLayout> layout;
    g_dwrite->CreateTextLayout(g_query.c_str(), (UINT32)g_query.size(), fmt, 10000, text.bottom - text.top, &layout);
    if (!layout) return;
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); DWRITE_TEXT_METRICS metrics{}; layout->GetMetrics(&metrics);
    float textWidth = metrics.widthIncludingTrailingWhitespace, room = text.right - text.left - 3 * s, shift = std::max(0.0f, textWidth - room);
    g_target->PushAxisAlignedClip(text, D2D1_ANTIALIAS_MODE_ALIASED);
    size_t selStart = QuerySelStart(), selEnd = QuerySelEnd();
    if (selEnd > selStart && selEnd <= g_query.size()) {
        DWRITE_HIT_TEST_METRICS hitMetrics[256]{}; UINT32 actual = 0;
        if (SUCCEEDED(layout->HitTestTextRange((UINT32)selStart, (UINT32)(selEnd - selStart),
                text.left - shift, text.top, hitMetrics, 256, &actual))) {
            for (UINT32 i = 0; i < std::min<UINT32>(actual, 256); ++i) {
                const auto& hm = hitMetrics[i];
                Fill(D2D1::RectF(hm.left, hm.top + 2.0f * s, hm.left + hm.width,
                                 hm.top + hm.height - 2.0f * s),
                     D2D1::ColorF(0.20f, 0.48f, 0.92f, 0.48f));
            }
        }
    }
    g_brush->SetColor(Fg(0.95f * a)); g_target->DrawTextLayout(D2D1::Point2F(text.left - shift, text.top), layout.Get(), g_brush.Get());

    // Caret follows the insertion point, not always the end of the query.
    if ((siri || g_searchActive) && !QueryHasSelection() && (!g_frameTimer || (int)(Now() * 2) % 2 == 0)) {
        DWRITE_HIT_TEST_METRICS hm{}; FLOAT caretX = 0, caretY = 0;
        layout->HitTestTextPosition((UINT32)std::min(g_queryCaret, g_query.size()), FALSE, &caretX, &caretY, &hm);
        caretX += text.left - shift;
        float caretH = (siri ? 26 : 20) * s, mid = (text.top + text.bottom) / 2;
        Fill(D2D1::RectF(std::round(caretX), mid - caretH / 2, std::round(caretX) + std::max(1.0f, s), mid + caretH / 2), Fg(0.6f * a));
    }
    g_target->PopAxisAlignedClip();
}

// ---------- Power button ----------
void DrawPowerIcon(D2D1_POINT_2F c, float s, float a);
void DrawPowerButton(const Layout& L, float a) {
    if (!g_settings.showPower || a <= 0.001f || L.btnPower.right <= L.btnPower.left) return;
    float s = L.scale; const D2D1_RECT_F& r = L.btnPower;
    float rad = (r.right - r.left) / 2;
    D2D1_POINT_2F c = D2D1::Point2F((r.left + r.right) / 2, (r.top + r.bottom) / 2);

    DrawGlassButton(r, rad, a, g_powerHover ? 1.6f : 1.0f);
    DrawPowerIcon(c, s, a);
}

void DrawPowerIcon(D2D1_POINT_2F c, float s, float a) {
    float ir = 8 * s, gap = 0.70f;
    D2D1_POINT_2F p0 = D2D1::Point2F(c.x - ir * std::sin(gap), c.y - ir * std::cos(gap));
    D2D1_POINT_2F p1 = D2D1::Point2F(c.x + ir * std::sin(gap), c.y - ir * std::cos(gap));
    g_brush->SetColor(Fg(0.9f * a));
    ComPtr<ID2D1PathGeometry> path; ComPtr<ID2D1GeometrySink> sink;
    if (SUCCEEDED(g_d2d->CreatePathGeometry(&path)) && SUCCEEDED(path->Open(&sink))) {
        sink->BeginFigure(p0, D2D1_FIGURE_BEGIN_HOLLOW);
        sink->AddArc(D2D1::ArcSegment(p1, D2D1::SizeF(ir, ir), 0, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_LARGE));
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();
        g_target->DrawGeometry(path.Get(), g_brush.Get(), 1.8f * s);
    }
    g_target->DrawLine(D2D1::Point2F(c.x, c.y - ir * 1.2f), D2D1::Point2F(c.x, c.y - ir * 0.15f), g_brush.Get(), 1.8f * s);
}

void EnableShutdownPrivilege() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) return;
    TOKEN_PRIVILEGES tp{}; tp.PrivilegeCount = 1; tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid))
        AdjustTokenPrivileges(token, FALSE, &tp, 0, nullptr, nullptr);
    CloseHandle(token);
}

void DoPowerAction(int id) {
    switch (id) {
        case kMenuLock: LockWorkStation(); break;
        case kMenuSleep: {
            using SetSuspendState_t = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
            if (HMODULE h = LoadLibraryExW(L"powrprof.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32)) {
                if (auto fn = (SetSuspendState_t)GetProcAddress(h, "SetSuspendState")) fn(FALSE, FALSE, FALSE);
                FreeLibrary(h);
            }
            break;
        }
        case kMenuRestart:  EnableShutdownPrivilege(); ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_FLAG_PLANNED); break;
        case kMenuShutdown: EnableShutdownPrivilege(); ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF, SHTDN_REASON_FLAG_PLANNED); break;
        case kMenuSignOut:  ExitWindowsEx(EWX_LOGOFF, 0); break;
    }
}

void DrawFolderHeader(const Layout& L, float a) {
    if (a <= 0.001f) return;
    float s = L.scale;
    float cx = L.btnBack.left + 16 * s, cy = (L.btnBack.top + L.btnBack.bottom) / 2;
    g_brush->SetColor(Fg(0.6f * a));
    g_target->DrawLine(D2D1::Point2F(cx - 4 * s, cy), D2D1::Point2F(cx + 6 * s, cy), g_brush.Get(), 2.0f * s);
    g_target->DrawLine(D2D1::Point2F(cx - 4 * s, cy), D2D1::Point2F(cx + 2 * s, cy - 6 * s), g_brush.Get(), 2.0f * s);
    g_target->DrawLine(D2D1::Point2F(cx - 4 * s, cy), D2D1::Point2F(cx + 2 * s, cy + 6 * s), g_brush.Get(), 2.0f * s);

    if (g_titleFormat && g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        D2D1_RECT_F row = D2D1::RectF(0, L.btnBack.top, (float)L.width, L.btnBack.bottom);
        if (g_renaming) {
            float cx2 = L.width / 2.0f, textWidth = 0;
            ComPtr<IDWriteTextLayout> layout;
            g_dwrite->CreateTextLayout(g_renameText.c_str(), (UINT32)g_renameText.size(), g_titleFormat.Get(), 10000, row.bottom - row.top, &layout);
            if (layout) { DWRITE_TEXT_METRICS metrics{}; layout->GetMetrics(&metrics); textWidth = metrics.widthIncludingTrailingWhitespace; }
            float half = std::max(textWidth / 2 + 18 * s, 90 * s);
            D2D1_RECT_F box = D2D1::RectF(cx2 - half, row.top, cx2 + half, row.bottom);
            Fill(box, Fg(0.14f * a), 8 * s);
            g_brush->SetColor(Fg(0.30f * a));
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(box, 8 * s, 8 * s), g_brush.Get(), 1.0f);
            if (g_renameSelectAll && textWidth > 0) {
                Fill(D2D1::RectF(cx2 - textWidth / 2 - 2 * s, row.top + 6 * s, cx2 + textWidth / 2 + 2 * s, row.bottom - 6 * s), D2D1::ColorF(0.33f, 0.55f, 1.0f, 0.55f * a), 3 * s);
            }
            g_brush->SetColor(Fg(0.95f * a));
            g_target->DrawText(g_renameText.c_str(), (UINT32)g_renameText.size(), g_titleFormat.Get(), row, g_brush.Get());
            if (!g_renameSelectAll) {
                float caretX = std::round(cx2 + textWidth / 2) + 1, caretH = 18 * s, mid = (row.top + row.bottom) / 2;
                Fill(D2D1::RectF(caretX, mid - caretH / 2, caretX + std::max(1.0f, s), mid + caretH / 2), Fg(0.8f * a));
            }
        } else {
            const std::wstring& name = g_folders[g_openFolderIndex].name;
            g_brush->SetColor(Fg(0.95f * a));
            g_target->DrawText(name.c_str(), (UINT32)name.size(), g_titleFormat.Get(), row, g_brush.Get());
        }
    }
}

void DrawMessage(PCWSTR text) {
    const Layout& L = g_layout;
    if (!g_messageFormat) return;
    g_brush->SetColor(Fg(0.6f));
    float h = g_settings.siri ? L.cellHeight : 120 * L.scale;
    g_target->DrawText(text, (UINT32)wcslen(text), g_messageFormat.Get(), D2D1::RectF(0, L.viewTop, (float)L.width, L.viewTop + h), g_brush.Get());
}

D2D1_RECT_F FolderMiniSlot(const D2D1_RECT_F& r, int index, float pad) {
    int k = std::min(index, 3);
    float cellW = (r.right - r.left - pad * 3) / 2;
    float cellH = (r.bottom - r.top - pad * 3) / 2;
    float left = r.left + pad + (k % 2) * (cellW + pad);
    float top = r.top + pad + (k / 2) * (cellH + pad);
    return D2D1::RectF(left, top, left + cellW, top + cellH);
}

void DrawFolderIcon(const Folder& folder, const D2D1_RECT_F& r, float opacity = 1.0f) {
    float s = g_layout.scale;
    float pad = 10 * s;
    float tile = g_settings.darkText ? 0.55f : 0.2f;
    Fill(r, D2D1::ColorF(1.0f, 1.0f, 1.0f, tile * opacity), 16 * s);
    for (int i = 0; i < 4 && i < (int)folder.apps.size(); i++) {
        D2D1_RECT_F slot = FolderMiniSlot(r, i, pad);
        App& app = *folder.apps[i];
        if (ID2D1Bitmap* bitmap = GetBitmap(app)) {
            g_target->DrawBitmap(bitmap, slot, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }
}

void DrawHistoryHeader(const Layout& L) {
    float s = L.scale;
    if (g_listFormat) {
        D2D1_RECT_F t = D2D1::RectF(L.listHeader.left + 10 * s, L.listHeader.top, L.listHeader.right - 40 * s, L.listHeader.bottom);
        g_brush->SetColor(Fg(0.55f));
        g_target->DrawText(Tr(L"Recent searches"), 15, g_listFormat.Get(), t, g_brush.Get());
    }
    D2D1_RECT_F r = HistClearRect(L);
    if (g_histClearHover) Fill(r, Fg(0.12f), 8 * s);
    float cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2, u = 6 * s, w = 1.5f * s, lid = cy - u * 0.6f;
    g_brush->SetColor(Fg(g_histClearHover ? 0.9f : 0.55f));
    g_target->DrawLine(D2D1::Point2F(cx - u - s, lid), D2D1::Point2F(cx + u + s, lid), g_brush.Get(), w);
    g_target->DrawLine(D2D1::Point2F(cx - 2.5f * s, lid - 2 * s), D2D1::Point2F(cx + 2.5f * s, lid - 2 * s), g_brush.Get(), w);
    g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(cx - u * 0.8f, lid + s, cx + u * 0.8f, cy + u), 1.5f * s, 1.5f * s), g_brush.Get(), w);
}

void DrawMainGrid(const Layout& L, const std::vector<int>& items, float scroll, float alpha, bool interactive, int hiddenFolderItem, float hiddenFade) {
    float s = L.scale;
    if (interactive) {
        bool tab = g_siriTab != 0;
        if (!tab && !g_appsLoaded && g_apps.empty()) { DrawMessage(Tr(L"Loading apps...")); return; }
        if (items.empty()) {
            PCWSTR msg = g_query.empty() ? Tr(L"No apps") : Tr(L"No apps found");
            if (g_siriTab == 1) {
                if (g_query.empty()) msg = g_historyEnabled ? Tr(L"No recent searches") : Tr(L"Type to search files");
                else if (g_query.size() < 2) msg = Tr(L"Type at least 2 characters");
                else if (g_fileSearching) msg = Tr(L"Searching...");
                else msg = g_fileStatus.empty() ? Tr(L"No files found") : g_fileStatus.c_str();
            }
            if (g_siriTab == 2) msg = g_query.empty() ? Tr(L"Copy some text and it will show up here") : Tr(L"Nothing found");
            if (g_siriTab == 3) msg = Tr(L"No actions found");
            DrawMessage(msg);
            return;
        }
    } else if (items.empty()) {
        return;
    }
    g_target->PushAxisAlignedClip(D2D1::RectF(0, L.viewTop, (float)L.width, L.viewBottom), D2D1_ANTIALIAS_MODE_ALIASED);
    int first, last; VisibleRange(L, scroll, (int)items.size(), first, last);

    for (int i = first; i < last; i++) {
        int item = items[i];
        if (item < 0) continue;
        if (IsFolderItem(item) ? item - 1000 >= (int)g_folders.size() : !ResolveApp(item)) continue;
        Cell c = CellAtIn(L, scroll, i);

        bool sel = interactive && IsSelected(i);
        if (sel) Fill(c.highlight, D2D1::ColorF(0.04f, 0.52f, 1.0f, 0.22f), 12 * s);
        else if (interactive && (i == g_selected || i == g_hover))
            Fill(c.highlight, Fg(i == g_selected ? 0.18f : 0.10f), 12 * s);
        float dragDim = (interactive && ((i == g_pressed && g_isDragging) || i == g_dragItem || (sel && GroupDrag()))) ? 0.4f : 1.0f;
        float opacity = dragDim * alpha * PageFade(L, scroll, i);

        if (IsFolderItem(item)) {
            const Folder& f = g_folders[item - 1000];
            float labelOpacity = opacity;
            if (item == hiddenFolderItem) labelOpacity *= 1.0f - hiddenFade;
            else DrawFolderIcon(f, c.icon, opacity);
            if (g_labelFormat) {
                g_brush->SetColor(Fg(0.95f * labelOpacity));
                g_target->DrawText(f.name.c_str(), (UINT32)f.name.size(), g_labelFormat.Get(), c.label, g_brush.Get());
            }
        } else {
            App* resolved = ResolveApp(item);
            if (!resolved) continue;
            App& app = *resolved;
            D2D1::Matrix3x2F baseTransform;
            g_target->GetTransform(&baseTransform);
            bool jiggle = sel && (g_settings.jiggle || Picking());
            if (jiggle) g_target->SetTransform(JiggleMatrix(c.icon, i, s) * baseTransform);
            if (ID2D1Bitmap* bitmap = GetBitmap(app)) {
                g_target->DrawBitmap(bitmap, c.icon, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            } else {
                Fill(c.icon, Fg(0.12f * opacity), 14 * s);
                if (app.kind == 2 && L.list && g_glyphFormat) {
                    g_brush->SetColor(Fg(0.8f * opacity));
                    g_target->DrawText(L"\uE77F", 1, g_glyphFormat.Get(), c.icon, g_brush.Get());   // clipboard
                } else if ((app.kind == 2 || app.kind == 3) && g_titleFormat) {
                    PCWSTR glyph = app.kind == 2 ? L"Aa" : L"\u21BA"; // ↺
                    g_brush->SetColor(Fg(0.8f * opacity));
                    g_target->DrawText(glyph, (UINT32)wcslen(glyph), g_titleFormat.Get(), c.icon, g_brush.Get());
                }
                if (app.kind == 4 && g_glyphFormat) {
                    PCWSTR glyph = ActionGlyph(app.target);
                    g_brush->SetColor(Fg(0.85f * opacity));
                    g_target->DrawText(glyph, (UINT32)wcslen(glyph), g_glyphFormat.Get(), c.icon, g_brush.Get());
                }

            }
            if (sel) DrawCheckBadge(c.icon, opacity);
            if (jiggle) g_target->SetTransform(baseTransform);
            if (L.list) {
                if (g_listFormat) {
                    // Files show their folder under the name so same-named items can be told apart
                    bool clip = app.kind == 2 && g_listSubFormat;
                    bool twoLines = clip || (app.kind == 1 && g_listSubFormat && app.target.find(L'\\') != std::wstring::npos);
                    D2D1_RECT_F nameRect = c.label;
                    if (twoLines) nameRect.bottom = c.label.top + (c.label.bottom - c.label.top) * 0.58f;
                    std::wstring name = app.name, folder;
                    if (clip) ClipRowText(app.target, name, folder);
                    else if (twoLines) folder = app.target.substr(0, app.target.find_last_of(L'\\'));
                    g_brush->SetColor(Fg(0.95f * opacity));
                    g_target->DrawText(name.c_str(), (UINT32)name.size(), g_listFormat.Get(), nameRect, g_brush.Get());
                    if (twoLines) {
                        D2D1_RECT_F subRect = D2D1::RectF(c.label.left, nameRect.bottom - 3 * s, c.label.right, c.label.bottom - 2 * s);
                        g_brush->SetColor(Fg(0.5f * opacity));
                        g_target->DrawText(folder.c_str(), (UINT32)folder.size(), g_listSubFormat.Get(), subRect, g_brush.Get());
                    }
                }
            } else if (g_labelFormat) {
                D2D1_RECT_F shadow = c.label; shadow.top += 1 * s; shadow.bottom += 1 * s;
                g_brush->SetColor(D2D1::ColorF(0, 0, 0, (g_settings.darkText ? 0.0f : 0.45f) * opacity)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), shadow, g_brush.Get());
                g_brush->SetColor(Fg(0.95f * opacity)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), c.label, g_brush.Get());
            }
        }
    }

    if (interactive && g_isDragging && g_dragItem >= 0 && g_dragItem < (int)items.size() && g_dragX >= 0) {
        int item = items[g_dragItem];
        D2D1_RECT_F dragRect = D2D1::RectF(g_dragX - L.icon / 2, g_dragY - L.icon / 2, g_dragX + L.icon / 2, g_dragY + L.icon / 2);
        if (IsFolderItem(item)) {
            if (item - 1000 < (int)g_folders.size()) DrawFolderIcon(g_folders[item - 1000], dragRect, 0.8f);
        } else if (App* da = ResolveApp(item)) {
            if (GroupDrag() && item < 1000) DrawGroupDrag(dragRect, item);
            else if (ID2D1Bitmap* bitmap = GetBitmap(*da))
                g_target->DrawBitmap(bitmap, dragRect, 0.8f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }

    // Original Siri: a thin line under the first row (recent / most used apps)
    if (interactive && g_settings.siriOriginal && g_siriTab == 0 && !L.list && g_firstRowCount > 0 &&
        (int)items.size() > g_firstRowCount && L.columns > 0) {
        Cell rowStart = CellAtIn(L, scroll, 0), rowEnd = CellAtIn(L, scroll, std::min(L.columns, g_firstRowCount) - 1);
        float y = std::round(rowStart.highlight.top + L.cellHeight - L.cellPad) + 0.5f;
        D2D1_GRADIENT_STOP stops[3] = {{0.0f, Fg(0.0f)}, {0.5f, Fg(0.22f * alpha)}, {1.0f, Fg(0.0f)}};
        if (auto line = MakeGradient(stops, 3, D2D1::Point2F(rowStart.highlight.left, y), D2D1::Point2F(rowEnd.highlight.right, y)))
            g_target->DrawLine(D2D1::Point2F(rowStart.highlight.left + 6 * s, y), D2D1::Point2F(rowEnd.highlight.right - 6 * s, y), line.Get(), std::max(1.0f, s));
    }

    g_target->PopAxisAlignedClip();

    if (interactive) {
        if (L.list && L.listHeader.right > L.listHeader.left) DrawHistoryHeader(L);
        if (L.pages > 1 && (L.horizontal || MaxScroll() > 0)) DrawPageDots(L, scroll, 1.0f);
    }
}

void DrawFolderPanel(float t, const D2D1_RECT_F& from, float fromPad, bool settled) {
    const Layout& L = g_layout; float s = L.scale;
    if (g_openFolderIndex < 0 || g_openFolderIndex >= (int)g_folders.size()) return;
    const Folder& folder = g_folders[g_openFolderIndex];

    D2D1_RECT_F rect = LerpRect(from, L.panel, t);
    float radius = Lerp(16 * s, g_settings.folderRadius * s, t);

    Fill(rect, D2D1::ColorF(1, 1, 1, Lerp(0.2f, 0.04f, t)), radius);
    if (g_settings.borderWidth > 0) {
        g_brush->SetColor(Alpha(g_settings.borderColor, g_settings.borderOpacity * t));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), g_brush.Get(), g_settings.borderWidth * s);
    }

    D2D1_RECT_F clip = D2D1::RectF(rect.left, Lerp(rect.top, L.viewTop, t), rect.right, Lerp(rect.bottom, L.viewBottom, t));
    g_target->PushAxisAlignedClip(clip, D2D1_ANTIALIAS_MODE_ALIASED);

    int count = (int)g_layoutItems.size();
    int first = 0, last = count;
    if (settled) VisibleRange(L, g_scroll, count, first, last);

    float labelFade = std::clamp((t - 0.5f) / 0.5f, 0.0f, 1.0f);
    for (int i = first; i < last; i++) {
        int appIdx = g_layoutItems[i];
        if (appIdx < 0 || appIdx >= (int)folder.apps.size()) continue;
        App& app = *folder.apps[appIdx];
        Cell c = CellAtIn(L, g_scroll, i);

        if (settled && (i == g_selected || i == g_hover)) {
            Fill(c.highlight, Fg(i == g_selected ? 0.18f : 0.10f), 12 * s);
        }

        D2D1_RECT_F slot = FolderMiniSlot(from, appIdx, fromPad);
        float delay = std::min(appIdx, 8) * 0.035f;
        float localT = std::clamp((t - delay) / std::max(0.01f, 1.0f - delay), 0.0f, 1.0f);
        localT = EaseInOut(localT);

        D2D1_RECT_F icon = LerpRect(slot, c.icon, localT);
        float iconScale = 0.72f + 0.28f * localT;
        float centerX = (icon.left + icon.right) * 0.5f, centerY = (icon.top + icon.bottom) * 0.5f;
        float halfWidth = (icon.right - icon.left) * 0.5f * iconScale, halfHeight = (icon.bottom - icon.top) * 0.5f * iconScale;
        icon = D2D1::RectF(centerX - halfWidth, centerY - halfHeight, centerX + halfWidth, centerY + halfHeight);

        float dragDim = (settled && i == g_pressed && g_isDragging) ? 0.4f : 1.0f;
        float opacity = dragDim * localT * PageFade(L, g_scroll, i);

        if (ID2D1Bitmap* bitmap = GetBitmap(app)) {
            g_target->DrawBitmap(bitmap, icon, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }

        if (g_labelFormat && labelFade > 0.0f) {
            D2D1_RECT_F label = c.label;
            if (!settled) {
                float cx = (icon.left + icon.right) / 2, w = c.label.right - c.label.left;
                float top = icon.bottom + std::round(8 * s * localT);
                label = D2D1::RectF(cx - w / 2, top, cx + w / 2, top + L.labelHeight);
            }
            float lo = labelFade * opacity;
            D2D1_RECT_F shadow = label; shadow.top += 1 * s; shadow.bottom += 1 * s;
            g_brush->SetColor(D2D1::ColorF(0, 0, 0, (g_settings.darkText ? 0.0f : 0.45f) * lo)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), shadow, g_brush.Get());
            g_brush->SetColor(Fg(0.95f * lo)); g_target->DrawText(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), label, g_brush.Get());
        }
    }

    g_target->PopAxisAlignedClip();

    if (settled && g_isDragging && g_dragItem >= 0 && g_dragItem < (int)g_layoutItems.size() && g_dragX >= 0) {
        int idx = g_layoutItems[g_dragItem];
        if (idx >= 0 && idx < (int)folder.apps.size()) {
            if (ID2D1Bitmap* bmp = GetBitmap(*folder.apps[idx])) {
                D2D1_RECT_F r = D2D1::RectF(g_dragX - L.icon / 2, g_dragY - L.icon / 2, g_dragX + L.icon / 2, g_dragY + L.icon / 2);
                g_target->DrawBitmap(bmp, r, 0.8f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            }
        }
    }

    if (settled) {
        if (L.pages > 1 && (L.horizontal || MaxScroll() > 0)) DrawPageDots(L, g_scroll, 1.0f);
    }
}

void StartFrames();

void DrawGlass(const D2D1_RECT_F& r, float radius, float sheenScale = 1.0f) {
    if (!g_settings.liquidGlass) return;
    float s = g_layout.scale;
    float w = r.right - r.left;
    ComPtr<ID2D1GradientStopCollection> sheenStops;
    ComPtr<ID2D1LinearGradientBrush> sheen;
    D2D1_GRADIENT_STOP sheenData[3] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, (g_settings.darkText ? 0.30f : 0.16f) * sheenScale)},
        {0.45f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.04f * sheenScale)},
        {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
    };
    if (SUCCEEDED(g_target->CreateGradientStopCollection(sheenData, 3, &sheenStops)) &&
        SUCCEEDED(g_target->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.left + w * 0.4f, r.bottom)),
            sheenStops.Get(), &sheen))) {
        g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), sheen.Get());
    }
    if (g_settings.fullscreen) return;

    ComPtr<ID2D1GradientStopCollection> rimStops;
    ComPtr<ID2D1LinearGradientBrush> rim;
    D2D1_GRADIENT_STOP rimData[4] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.60f)},
        {0.30f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.12f)},
        {0.70f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.10f)},
        {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.45f)},
    };
    if (FAILED(g_target->CreateGradientStopCollection(rimData, 4, &rimStops)) ||
        FAILED(g_target->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.right, r.bottom)),
            rimStops.Get(), &rim))) return;
    float bw = std::max(1.0f, 1.5f * s), half = bw / 2;
    float outer = std::max(0.0f, radius - half);
    g_target->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), outer, outer),
        rim.Get(), bw);
    float inset = bw + 1.5f * s, inner = std::max(0.0f, radius - inset);
    rim->SetOpacity(0.35f);
    g_target->DrawRoundedRectangle(
        D2D1::RoundedRect(D2D1::RectF(r.left + inset, r.top + inset, r.right - inset, r.bottom - inset), inner, inner),
        rim.Get(), bw * 0.8f);
}

void PushOpacity(float a) {
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), a), nullptr);
}


// Native Direct2D Siri-style animated orb used during the initial dot phase.
// This avoids requiring a separate Lottie runtime or external animation file.
// Colorful flowing wave inside a dark disk (recreates the Siri-style Lottie look natively:
// a few blurred, stretched blobs drift, rotate and breathe over a near-black circle).
void DrawSiriWaveOrb(const D2D1_RECT_F& r, float envelope) {
    const float size = std::min(r.right - r.left, r.bottom - r.top);
    const float cx = (r.left + r.right) * 0.5f, cy = (r.top + r.bottom) * 0.5f, R = size * 0.5f;
    if (R < 2.0f || envelope <= 0.001f) return;

    struct Blob { float r, g, b, cx, cy, rx, ry, speed, phase, alpha; };
    static const Blob blobs[kOrbBlobs] = {
        {0.05f, 0.38f, 0.95f, 0.00f,  0.30f, 1.00f, 0.42f, 0.9f, 0.5f, 0.85f},   // blue base
        {1.00f, 0.55f, 0.22f, -0.42f, 0.04f, 0.80f, 0.34f, 1.2f, 0.0f, 0.95f},   // orange
        {0.38f, 0.86f, 1.00f,  0.40f, -0.06f, 0.85f, 0.38f, 1.0f, 2.0f, 0.95f},  // cyan
        {1.00f, 0.52f, 0.91f,  0.10f, 0.14f, 0.60f, 0.28f, 1.6f, 4.0f, 0.80f},   // pink
        {1.00f, 0.96f, 0.99f,  0.00f, 0.18f, 0.70f, 0.26f, 1.9f, 1.0f, 1.00f},   // white lens
    };

    ComPtr<ID2D1EllipseGeometry> mask;
    if (FAILED(g_d2d->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(cx, cy), R, R), &mask))) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                              D2D1::IdentityMatrix(), envelope), nullptr);
    g_brush->SetColor(D2D1::ColorF(0.02f, 0.02f, 0.035f, 1.0f));
    g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), R, R), g_brush.Get());

    const float t = (float)Now();
    for (int i = 0; i < kOrbBlobs; i++) {
        const Blob& b = blobs[i];
        if (!g_orbBrush[i]) {
            D2D1_GRADIENT_STOP stops[3] = {
                {0.0f, D2D1::ColorF(b.r, b.g, b.b, 1.0f)},
                {0.5f, D2D1::ColorF(b.r, b.g, b.b, 0.55f)},
                {1.0f, D2D1::ColorF(b.r, b.g, b.b, 0.0f)},
            };
            ComPtr<ID2D1GradientStopCollection> collection;
            if (FAILED(g_target->CreateGradientStopCollection(stops, 3, &collection))) continue;
            g_target->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, 0), 1.0f, 1.0f),
                                                collection.Get(), &g_orbBrush[i]);
            if (!g_orbBrush[i]) continue;
        }
        float w = t * b.speed + b.phase;
        float px = cx + R * (b.cx + 0.14f * std::sin(w * 1.3f)), py = cy + R * (b.cy + 0.10f * std::sin(w * 1.7f + 1.0f));
        float breathe = 1.0f + 0.12f * std::sin(w * 2.1f);
        float rx = R * b.rx * breathe, ry = R * b.ry * (2.0f - breathe), angle = 0.30f * std::sin(w * 1.1f);
        g_orbBrush[i]->SetTransform(D2D1::Matrix3x2F::Scale(rx, ry) * D2D1::Matrix3x2F::Rotation(angle * 57.29578f) *
                                    D2D1::Matrix3x2F::Translation(px, py));
        g_orbBrush[i]->SetOpacity(b.alpha);
        g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), rx * 1.05f + ry, rx * 1.05f + ry), g_orbBrush[i].Get());
    }
    g_target->PopLayer();
}

void DrawSiriIntroOrb(const Layout& L) {
    if (!g_settings.siri || g_settings.siriOrbStyle == 2 || g_pillIntro >= kIntroCircle || g_pillIntro <= 0.001f) return;
    if (g_settings.siriOrbStyle == 0) {
        DrawSiriWaveOrb(L.search, 1.0f - Ease(std::clamp(g_pillIntro / kIntroCircle, 0.0f, 1.0f)));
        return;
    }

    const D2D1_RECT_F r = L.search;
    const float size = std::min(r.right - r.left, r.bottom - r.top);
    if (size < 4.0f) return;

    const float cx = (r.left + r.right) * 0.5f;
    const float cy = (r.top + r.bottom) * 0.5f;
    const float t = (float)Now() * 2.8f;
    const float intro = std::clamp(g_pillIntro / kIntroCircle, 0.0f, 1.0f);
    const float alpha = (1.0f - Ease(intro)) * g_settings.siriPillOpacity;
    const float radius = size * 0.29f;

    struct OrbColor { float r, g, b, phase; };
    const OrbColor colors[] = {
        {0.10f, 0.55f, 1.00f, 0.0f},   // blue
        {0.48f, 0.20f, 1.00f, 1.57f},   // violet
        {1.00f, 0.20f, 0.62f, 3.14f},   // pink
        {0.10f, 0.88f, 0.78f, 4.71f}    // cyan/teal
    };

    for (const auto& c : colors) {
        float a = t + c.phase;
        float orbit = size * 0.13f;
        D2D1_POINT_2F center = D2D1::Point2F(
            cx + std::cos(a) * orbit,
            cy + std::sin(a * 1.17f) * orbit);
        ComPtr<ID2D1RadialGradientBrush> glow;
        D2D1_RADIAL_GRADIENT_BRUSH_PROPERTIES props =
            D2D1::RadialGradientBrushProperties(center, D2D1::Point2F(0, 0), radius, radius);
        D2D1_GRADIENT_STOP colorStops[3] = {
            {0.0f, D2D1::ColorF(c.r, c.g, c.b, 0.92f * alpha)},
            {0.55f, D2D1::ColorF(c.r, c.g, c.b, 0.52f * alpha)},
            {1.0f, D2D1::ColorF(c.r, c.g, c.b, 0.0f)}
        };
        ComPtr<ID2D1GradientStopCollection> coloredStops;
        if (FAILED(g_target->CreateGradientStopCollection(colorStops, 3, &coloredStops))) continue;
        if (FAILED(g_target->CreateRadialGradientBrush(props, coloredStops.Get(), &glow))) continue;

        g_target->FillEllipse(D2D1::Ellipse(center, radius, radius), glow.Get());
    }

    // A soft luminous core makes the moving colors read as a single Siri-like orb.
    ComPtr<ID2D1SolidColorBrush> core;
    if (SUCCEEDED(g_target->CreateSolidColorBrush(D2D1::ColorF(0.88f, 0.94f, 1.0f, 0.72f * alpha), &core))) {
        g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), size * 0.10f, size * 0.10f), core.Get());
    }
}

// ---------- Themes ----------
// Soft drop shadow drawn only outside the shape, so translucent glass stays clean
void DrawThemeShadow(const D2D1_RECT_F& r, float radius, float strength, float spread, float dy) {
    ComPtr<ID2D1RectangleGeometry> area;
    ComPtr<ID2D1RoundedRectangleGeometry> hole;
    ComPtr<ID2D1PathGeometry> outside;
    ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(g_d2d->CreateRectangleGeometry(D2D1::RectF(-10000, -10000, 10000, 10000), &area)) ||
        FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &hole)) ||
        FAILED(g_d2d->CreatePathGeometry(&outside)) || FAILED(outside->Open(&sink)))
        return;
    area->CombineWithGeometry(hole.Get(), D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink.Get());
    sink->Close();
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), outside.Get()), nullptr);
    const int steps = 10;
    for (int i = steps; i >= 1; i--) {
        float g = spread * i / steps;
        Fill(D2D1::RectF(r.left - g, r.top - g + dy, r.right + g, r.bottom + g + dy), D2D1::ColorF(0, 0, 0, strength / steps), radius + g);
    }
    g_target->PopLayer();
}

ComPtr<ID2D1LinearGradientBrush> MakeGradient(const D2D1_GRADIENT_STOP* stops, UINT32 count, D2D1_POINT_2F from, D2D1_POINT_2F to) {
    ComPtr<ID2D1GradientStopCollection> collection;
    ComPtr<ID2D1LinearGradientBrush> brush;
    if (SUCCEEDED(g_target->CreateGradientStopCollection(stops, count, &collection)))
        g_target->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(from, to), collection.Get(), &brush);
    return brush;
}

// Light: bright frosted glass, white top glow, luminous rim and a soft shadow.
// Clear glass: almost no fill, the wallpaper shows through; a thick-glass edge and a shadow projected on the wallpaper.
// Siri (macOS 27): smoky glass, dark at the top fading to clear at the bottom, thin light rim.
void DrawThemedShape(const D2D1_RECT_F& r, float radius, ID2D1Bitmap* backdrop, bool shadow) {
    const Layout& L = g_layout;
    float s = L.scale, h = r.bottom - r.top;
    Theme theme = ActiveTheme();
    if (h < 1 || r.right - r.left < 1) return;
    if (shadow) {
        if (theme == Theme::Glass) {
            DrawThemeShadow(r, radius, 0.16f, 10 * s, 4 * s);   // small, discreet: just a sense of depth
        } else {
            DrawThemeShadow(r, radius, theme == Theme::Light ? 0.14f : 0.18f, 10 * s, 4 * s);
        }
    }

    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &clip))) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
    if (backdrop)
        g_target->DrawBitmap(backdrop, D2D1::RectF(0, 0, (float)L.width, (float)L.height), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                             D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
    D2D1_GRADIENT_STOP lightBody[3] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.52f)},
        {0.5f, D2D1::ColorF(0.97f, 0.97f, 0.99f, 0.40f)},
        {1.0f, D2D1::ColorF(0.93f, 0.94f, 0.97f, 0.34f)},
    };
    float tint = (100 - g_settings.glassTransparency) / 100.0f * 0.6f;   // 0 = clear, 0.6 = smoky
    D2D1_GRADIENT_STOP glassBody[3] = {
        {0.0f, D2D1::ColorF(0.07f, 0.07f, 0.07f, tint * 0.85f)},
        {0.5f, D2D1::ColorF(0.04f, 0.04f, 0.04f, tint)},
        {1.0f, D2D1::ColorF(0.06f, 0.06f, 0.06f, tint * 0.9f)},
    };
    D2D1_GRADIENT_STOP siriBody[3] = {
        {0.0f, D2D1::ColorF(0.05f, 0.05f, 0.06f, 0.90f)},
        {0.55f, D2D1::ColorF(0.11f, 0.11f, 0.13f, 0.64f)},
        {1.0f, D2D1::ColorF(0.30f, 0.29f, 0.34f, 0.32f)},
    };
    // Glass transparency also thins out the Light and Siri glass (80 % = the theme as designed, 100 % = clear)
    float density = std::clamp((100 - g_settings.glassTransparency) / 20.0f, 0.0f, 2.5f);
    for (auto* stops : {lightBody, siriBody})
        for (int i = 0; i < 3; i++) stops[i].color.a = std::min(1.0f, stops[i].color.a * density);
    const D2D1_GRADIENT_STOP* body = theme == Theme::Light ? lightBody : theme == Theme::Glass ? glassBody : siriBody;
    if (auto brush = MakeGradient(body, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)))
        g_target->FillRectangle(r, brush.Get());
    if (theme == Theme::Light || theme == Theme::Glass) {
        // Specular glow along the top edge
        float glowH = std::min(h * (theme == Theme::Glass ? 0.22f : 0.35f), (theme == Theme::Glass ? 14 : 26) * s);
        float glowA = theme == Theme::Glass ? 0.07f : 0.55f;
        D2D1_GRADIENT_STOP glow[2] = {{0.0f, D2D1::ColorF(1, 1, 1, glowA)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.0f)}};
        if (auto top = MakeGradient(glow, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + glowH)))
            g_target->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + glowH), top.Get());
    }
    g_target->PopLayer();

    D2D1_GRADIENT_STOP softRim[7] = {
        {0.0f, D2D1::ColorF(0.83f, 0.83f, 0.83f, 0.44f)}, {0.1f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.31f)},
        {0.25f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.38f)}, {0.5f, D2D1::ColorF(0.13f, 0.13f, 0.13f, 0.44f)},
        {0.75f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.56f)}, {0.9f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.56f)},
        {1.0f, D2D1::ColorF(0.76f, 0.76f, 0.76f, 0.44f)},
    };
    // Light glass gets the same soft edge in brighter tones
    D2D1_GRADIENT_STOP softLightRim[5] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.75f)}, {0.15f, D2D1::ColorF(0.75f, 0.75f, 0.78f, 0.35f)},
        {0.5f, D2D1::ColorF(0.62f, 0.62f, 0.66f, 0.30f)}, {0.85f, D2D1::ColorF(0.75f, 0.75f, 0.78f, 0.40f)},
        {1.0f, D2D1::ColorF(0.96f, 0.96f, 0.98f, 0.60f)},
    };
    ComPtr<ID2D1LinearGradientBrush> rim = theme == Theme::Light ? MakeGradient(softLightRim, 5, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))
                                                                 : MakeGradient(softRim, 7, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom));
    if (!rim) return;
    float bw = std::max(1.0f, 1.2f * s), half = bw / 2, rr = std::max(0.0f, radius - half);
    g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), rim.Get(), bw);
}

void DrawSiriShapes(const Layout& L) {
    float s = L.scale, W = (float)L.width, H = (float)L.height;
    ID2D1Bitmap* backdrop = BlurStrength() > 0 ? GetBackdropBitmap() : nullptr;
    const bool themed = ActiveTheme() != Theme::Custom;

    // One continuous vertical gradient across pill, power pill and panel (macOS style):
    // darkest at the top of the pill, getting lighter (slightly cool) towards the bottom of the panel
    const bool shading = !themed && g_settings.siriShade && !g_settings.darkText && g_settings.siriShadeStrength > 0;
    const float k = g_settings.siriShadeStrength / 100.0f;
    ComPtr<ID2D1LinearGradientBrush> shade;
    if (shading) {
        D2D1_GRADIENT_STOP data[3] = {
            {0.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.60f * k)},
            {0.45f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f * k)},
            {1.0f, D2D1::ColorF(0.85f, 0.88f, 1.0f, 0.30f * k)},
        };
        float top = L.search.top, bottom = std::max({L.search.bottom, L.siriPanel.bottom, L.siriPower.bottom});
        ComPtr<ID2D1GradientStopCollection> stops;
        if (SUCCEEDED(g_target->CreateGradientStopCollection(data, 3, &stops)))
            g_target->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, top), D2D1::Point2F(0, std::max(top + 1, bottom))),
                                                stops.Get(), &shade);
    }
    auto shape = [&](const D2D1_RECT_F& r, float radius, D2D1_COLOR_F color, float opacity) {
        if (themed) { DrawThemedShape(r, radius, backdrop, true); return; }
        if (backdrop) {
            ComPtr<ID2D1RoundedRectangleGeometry> clip;
            g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &clip);
            if (clip) {
                g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
                g_target->DrawBitmap(backdrop, D2D1::RectF(0, 0, W, H), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                                     D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
                g_target->PopLayer();
            }
        }
        g_brush->SetColor(Alpha(color, opacity));
        g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), g_brush.Get());
        if (shade) g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), shade.Get());
        DrawGlass(r, radius, shade ? 0.35f : 1.0f);
        if (g_settings.borderWidth > 0) {
            float bw = g_settings.borderWidth * s, half = bw / 2, rr = std::max(0.0f, radius - half);
            g_brush->SetColor(Alpha(g_settings.borderColor, g_settings.borderOpacity));
            D2D1_RECT_F edge = D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half);
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(edge, rr, rr), g_brush.Get(), bw);
        }
    };

    float ex = SiriExpandEase();
    float panelW = L.siriPanel.right - L.siriPanel.left, panelH = L.siriPanel.bottom - L.siriPanel.top;
    if (ex > 0.001f && panelH > 1 && panelW > 1) {
        float pillR = (L.search.bottom - L.search.top) / 2;
        float rad = std::min({Lerp(pillR, g_settings.siriPanelRadius * s, ex), panelH / 2, panelW / 2});
        float pa = std::min(1.0f, g_siriAnim * 4);
        PushOpacity(pa);
        shape(L.siriPanel, rad, g_settings.siriPanelColor, g_settings.siriPanelOpacity);
        g_target->PopLayer();
    }

    float powerHeight = L.siriPower.bottom - L.siriPower.top;
    if (powerHeight > 2) shape(L.siriPower, powerHeight / 2, g_settings.siriPillColor, g_settings.siriPillOpacity);

    // Floating power button: drawn before the bar so it comes out from behind it
    if (L.btnPower.right > L.btnPower.left) {
        const D2D1_RECT_F& b = L.btnPower;
        float r = (b.right - b.left) / 2;
        PushOpacity(Ease(g_siriBtnAnim));
        shape(b, r, g_settings.siriPillColor, g_settings.siriPillOpacity);
        if (g_powerHover || g_powerPillByButton) Fill(b, Fg(g_powerPillByButton ? 0.18f : 0.12f), r);
        DrawPowerIcon(D2D1::Point2F((b.left + b.right) / 2, (b.top + b.bottom) / 2), s, 1.0f);
        g_target->PopLayer();
    }
    float ia = PillIntroAlpha();
    if (ia > 0.001f) {
        bool layer = ia < 0.999f;
        if (layer) PushOpacity(ia);
        shape(L.search, (L.search.bottom - L.search.top) / 2, g_settings.siriPillColor, g_settings.siriPillOpacity);
        if (layer) g_target->PopLayer();
    }
    DrawSiriIntroOrb(L);
}

// Original Siri: the picked row (icon + label) glides from the menu into the bar
void DrawCategoryFlight(const Layout& L) {
    if (!g_settings.siriOriginal || g_flyAnim >= 1.0f || !g_query.empty()) return;
    float s = L.scale, t = EaseInOut(g_flyAnim);
    const D2D1_RECT_F& r = L.search;
    float cy = (r.top + r.bottom) / 2, iconX = r.left + 30 * s;
    D2D1_RECT_F glyphTo = D2D1::RectF(iconX - 14 * s, cy - 14 * s, iconX + 14 * s, cy + 14 * s);
    D2D1_RECT_F textTo = D2D1::RectF(r.left + 62 * s, r.top, r.right - 30 * s, r.bottom);
    D2D1_RECT_F glyph = LerpRect(g_flyGlyph, glyphTo, t);
    D2D1_RECT_F text = LerpRect(g_flyText, textTo, t);
    float lift = std::sin(t * 3.14159265f) * 10 * s;   // a small arc on the way up
    glyph.top -= lift; glyph.bottom -= lift; text.top -= lift; text.bottom -= lift;
    g_brush->SetColor(Fg(Lerp(0.9f, 0.72f, t)));
    if (g_glyphLargeFormat) {
        PCWSTR icon = TabGlyph(g_siriTab);
        g_target->DrawText(icon, 1, g_glyphLargeFormat.Get(), glyph, g_brush.Get());
    }
    if (g_categoryFormat) {
        // The label grows from the menu's size to the bar's
        PCWSTR label = TabLabel(g_siriTab);
        float k = Lerp(1.0f, 18.0f / 15.0f, t), midY = (text.top + text.bottom) / 2;
        D2D1::Matrix3x2F base;
        g_target->GetTransform(&base);
        g_target->SetTransform(D2D1::Matrix3x2F::Scale(k, k, D2D1::Point2F(text.left, midY)) * base);
        g_brush->SetColor(Fg(Lerp(0.95f, 0.62f, t)));
        g_target->DrawText(label, (UINT32)wcslen(label), g_categoryFormat.Get(),
                           D2D1::RectF(text.left, midY - 20 * s, text.left + (text.right - text.left) / k, midY + 20 * s), g_brush.Get());
        g_target->SetTransform(base);
    }
}

void DrawSiriPillContent(const Layout& L) {
    float ca = PillContentAlpha();
    if (ca <= 0.001f) return;
    float r = (L.search.bottom - L.search.top) / 2;
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(L.search, r, r), &clip);
    if (!clip) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), ca), nullptr);
    DrawSearchBar(L, 1.0f);
    g_target->PopLayer();
    DrawCategoryFlight(L);
}

// Category strip — only the enabled categories, shared by every mode
int FilterAt(float x, float y) {
    if (!g_settings.siriOriginal || g_siriTab != 0 || g_siriAnim < 0.95f) return kNoChip;
    const D2D1_RECT_F& r = g_layout.siriTabs;
    if (r.right <= r.left || !InRect(r, x, y)) return kNoChip;
    auto chips = Chips();
    for (int i = 0; i < (int)chips.size(); i++) {
        float left, width;
        FilterSlotRect(r, chips, i, left, width);
        if (x >= left && x < left + width) return chips[i];
    }
    return kNoChip;
}

void DrawFilters(const Layout& L) {
    const D2D1_RECT_F& r = L.siriTabs;
    if (g_siriTab != 0 || r.right <= r.left || !g_tabFormat) return;
    float s = L.scale, h = r.bottom - r.top, rad = h / 2, inset = 3 * s;
    auto chips = Chips();
    // No outline around the strip; when it doesn't fit, its hidden side fades out
    ComPtr<ID2D1LinearGradientBrush> fade;
    float width = r.right - r.left, total = FiltersNaturalWidth();
    if (total > width + 0.5f && width > 0) {
        float scroll = std::clamp(g_chipScroll, 0.0f, total - width), f = std::min(36 * s, width / 4) / width;
        float leftA = scroll > 0.5f ? 0.0f : 1.0f, rightA = scroll < total - width - 0.5f ? 0.0f : 1.0f;
        D2D1_GRADIENT_STOP stops[] = {{0.0f, D2D1::ColorF(0, 0, 0, leftA)}, {f, D2D1::ColorF(0, 0, 0, 1)},
                                      {1.0f - f, D2D1::ColorF(0, 0, 0, 1)}, {1.0f, D2D1::ColorF(0, 0, 0, rightA)}};
        ComPtr<ID2D1GradientStopCollection> collection;
        if (SUCCEEDED(g_target->CreateGradientStopCollection(stops, 4, &collection)))
            g_target->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(r.left, 0), D2D1::Point2F(r.right, 0)),
                                                collection.Get(), &fade);
    }
    D2D1_RECT_F bounds = D2D1::RectF(r.left, r.top - 10 * s, r.right, r.bottom + 10 * s);
    g_target->PushLayer(D2D1::LayerParameters(bounds, nullptr, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), 1.0f, fade.Get()), nullptr);
    for (int i = 0; i < (int)chips.size(); i++) {
        int chip = chips[i];
        float x, w;
        FilterSlotRect(r, chips, i, x, w);
        if (x + w < r.left || x > r.right) continue;
        bool renaming = chip >= kChipCustom && chip - kChipCustom == g_chipRename;
        bool on = chip == g_appFilter || chip == kChipDone || renaming;
        if (on) {
            float vin = std::max(inset, h * 0.16f);
            DrawSoftPill(D2D1::RectF(x + inset, r.top + vin, x + w - inset, r.bottom - vin), rad, 1.0f, 0.13f);
        }
        std::wstring label = ChipLabel(chip);
        if (renaming) {
            // Typing the tab's name: the old name starts selected, then a caret follows the text
            float tw = ChipTextWidth(label), cx = x + w / 2;
            if (g_renameSelectAll && tw > 0)
                Fill(D2D1::RectF(cx - tw / 2 - 3 * s, r.top + 7 * s, cx + tw / 2 + 3 * s, r.bottom - 7 * s), D2D1::ColorF(0.04f, 0.52f, 1.0f, 0.45f), 4 * s);
            else {
                g_brush->SetColor(Fg(0.95f));
                g_target->DrawLine(D2D1::Point2F(cx + tw / 2 + 1.5f * s, r.top + 9 * s), D2D1::Point2F(cx + tw / 2 + 1.5f * s, r.bottom - 9 * s), g_brush.Get(), 1.4f * s);
            }
        }
        g_brush->SetColor(Fg(on ? 0.96f : 0.55f));
        g_target->DrawText(label.c_str(), (UINT32)label.size(), g_tabFormat.Get(), D2D1::RectF(x, r.top, x + w, r.bottom), g_brush.Get());
    }
    g_target->PopLayer();
}

void DrawTabs(const Layout& L) {
    if (g_settings.siriOriginal) { DrawFilters(L); return; }
    const D2D1_RECT_F& r = L.siriTabs;
    if (!TabsVisible() || r.right <= r.left || !g_tabFormat) return;
    float s = L.scale, h = r.bottom - r.top, rad = h / 2, inset = 3 * s;
    DrawGlassButton(r, rad, 1.0f, 0.75f);
    int a = std::clamp((int)std::floor(g_tabSlide), 0, g_tabCount - 1), b = std::min(a + 1, g_tabCount - 1);
    float la, wa, lb, wb;
    TabSlotRect(r, a, la, wa);
    TabSlotRect(r, b, lb, wb);
    float t = std::clamp(g_tabSlide - a, 0.0f, 1.0f), left = Lerp(la, lb, t), width = Lerp(wa, wb, t);
    DrawGlassButton(D2D1::RectF(left + inset, r.top + inset, left + width - inset, r.bottom - inset), std::max(0.0f, rad - inset), 1.0f, 1.7f);
    for (int i = 0; i < g_tabCount; i++) {
        int id = g_tabIds[i];
        float x, seg;
        TabSlotRect(r, i, x, seg);
        PCWSTR label = TabLabel(id);
        g_brush->SetColor(Fg(id == g_siriTab ? 0.98f : 0.62f));
        g_target->DrawText(label, (UINT32)wcslen(label), g_tabFormat.Get(), D2D1::RectF(x, r.top, x + seg, r.bottom), g_brush.Get());
    }
}

void DrawPowerPill(const Layout& L) {
    const D2D1_RECT_F& r = L.siriPower;
    float e = PowerPillEase();
    if (!g_settings.siri || e < 0.02f || r.bottom - r.top < 4 || !g_titleFormat) return;
    float s = L.scale, segment = (r.right - r.left) / 5;
    float labelAlpha = e * e, inset = 5 * s;
    float rad = std::max(0.0f, (r.bottom - r.top) / 2 - inset);
    g_target->PushAxisAlignedClip(r, D2D1_ANTIALIAS_MODE_ALIASED);
    for (int i = 0; i < 5; i++) {
        D2D1_RECT_F seg = D2D1::RectF(r.left + i * segment, r.top, r.left + (i + 1) * segment, r.bottom);
        D2D1_RECT_F cell = D2D1::RectF(seg.left + inset, seg.top + inset, seg.right - inset, seg.bottom - inset);
        bool strong = i == g_powerPillPick || i == g_powerPillHover;
        float level = i == g_powerPillHover ? 1.8f : (i == g_powerPillPick ? 1.25f : 0.0f);
        if (level > 0) DrawGlassButton(cell, rad, labelAlpha, level);
        if (i == g_powerPillFocus) {
            g_brush->SetColor(Fg(0.7f * labelAlpha));
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(cell, rad, rad), g_brush.Get(), 1.5f * s);
        }
        g_brush->SetColor(Fg((strong ? 0.98f : 0.78f) * labelAlpha));
        PCWSTR label = Tr(kPowerChoices[i].label);
        g_target->DrawText(label, (UINT32)wcslen(label), g_titleFormat.Get(), seg, g_brush.Get());
    }
    g_target->PopAxisAlignedClip();
}

// ---------- Live blur window ----------
// A click-through window right under ours shows Windows' live blur of what is behind it (the host backdrop
// brush of Windows.UI.Composition), cut to our shapes with antialiased rounded clips. The interfaces are
// declared here from the Windows metadata (method order = vtable order), so no C++/WinRT is needed.
namespace comp {
struct Vec2 { float X, Y; };
struct Vec3 { float X, Y, Z; };
#define COMP_SLOT(n) virtual HRESULT STDMETHODCALLTYPE Slot##n() = 0;
struct IVisualCollection : IInspectable {
    COMP_SLOT(0) COMP_SLOT(1) COMP_SLOT(2)
    virtual HRESULT STDMETHODCALLTYPE InsertAtTop(IUnknown* visual) = 0;
};
struct IContainerVisual : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Children(IVisualCollection** value) = 0;
};
struct IVisual : IInspectable {
    COMP_SLOT(0) COMP_SLOT(1) COMP_SLOT(2) COMP_SLOT(3) COMP_SLOT(4) COMP_SLOT(5) COMP_SLOT(6) COMP_SLOT(7) COMP_SLOT(8)
    virtual HRESULT STDMETHODCALLTYPE put_Clip(IUnknown* value) = 0;                     // 9
    COMP_SLOT(10) COMP_SLOT(11) COMP_SLOT(12)
    virtual HRESULT STDMETHODCALLTYPE put_IsVisible(unsigned char value) = 0;            // 13
    COMP_SLOT(14)
    virtual HRESULT STDMETHODCALLTYPE put_Offset(Vec3 value) = 0;                        // 15
    COMP_SLOT(16)
    virtual HRESULT STDMETHODCALLTYPE put_Opacity(float value) = 0;                      // 17
    COMP_SLOT(18) COMP_SLOT(19) COMP_SLOT(20) COMP_SLOT(21) COMP_SLOT(22) COMP_SLOT(23) COMP_SLOT(24)
    COMP_SLOT(25) COMP_SLOT(26) COMP_SLOT(27) COMP_SLOT(28) COMP_SLOT(29)
    virtual HRESULT STDMETHODCALLTYPE put_Size(Vec2 value) = 0;                          // 30
};
struct ISpriteVisual : IInspectable {
    COMP_SLOT(0)
    virtual HRESULT STDMETHODCALLTYPE put_Brush(IUnknown* value) = 0;                    // 1
};
struct ICompositionTarget : IInspectable {
    COMP_SLOT(0)
    virtual HRESULT STDMETHODCALLTYPE put_Root(IUnknown* value) = 0;                     // 1
};
struct IRoundedRectangleGeometry : IInspectable {
    COMP_SLOT(0)
    virtual HRESULT STDMETHODCALLTYPE put_CornerRadius(Vec2 value) = 0;                  // 1
    COMP_SLOT(2)
    virtual HRESULT STDMETHODCALLTYPE put_Offset(Vec2 value) = 0;                        // 3
    COMP_SLOT(4)
    virtual HRESULT STDMETHODCALLTYPE put_Size(Vec2 value) = 0;                          // 5
};
struct ICompositor : IInspectable {
    COMP_SLOT(0) COMP_SLOT(1) COMP_SLOT(2)
    virtual HRESULT STDMETHODCALLTYPE CreateContainerVisual(IContainerVisual** result) = 0;   // 3
    COMP_SLOT(4) COMP_SLOT(5) COMP_SLOT(6) COMP_SLOT(7) COMP_SLOT(8) COMP_SLOT(9) COMP_SLOT(10) COMP_SLOT(11)
    COMP_SLOT(12) COMP_SLOT(13) COMP_SLOT(14) COMP_SLOT(15)
    virtual HRESULT STDMETHODCALLTYPE CreateSpriteVisual(ISpriteVisual** result) = 0;          // 16
};
struct ICompositor3 : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE CreateHostBackdropBrush(IInspectable** result) = 0;      // 0
};
struct ICompositor5 : IInspectable {
    COMP_SLOT(0) COMP_SLOT(1) COMP_SLOT(2) COMP_SLOT(3) COMP_SLOT(4) COMP_SLOT(5) COMP_SLOT(6) COMP_SLOT(7)
    COMP_SLOT(8) COMP_SLOT(9) COMP_SLOT(10) COMP_SLOT(11) COMP_SLOT(12) COMP_SLOT(13)
    virtual HRESULT STDMETHODCALLTYPE CreateRoundedRectangleGeometry(IRoundedRectangleGeometry** result) = 0;   // 14
};
struct ICompositor6 : IInspectable {
    COMP_SLOT(0)
    virtual HRESULT STDMETHODCALLTYPE CreateGeometricClipWithGeometry(IUnknown* geometry, IInspectable** result) = 0;   // 1
};
struct IDispatcherQueueController : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE Slot0() = 0;
    virtual HRESULT STDMETHODCALLTYPE ShutdownQueueAsync(IInspectable** operation) = 0;   // 1
};
constexpr GUID IID_IDispatcherQueueController{0x22F34E66, 0x50DB, 0x4E36, {0xA9, 0x8D, 0x61, 0xC0, 0x1B, 0x38, 0x4D, 0x20}};
struct ICompositorDesktopInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateDesktopWindowTarget(HWND window, BOOL isTopmost, IInspectable** result) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnsureOnThread(DWORD threadId) = 0;
};
#undef COMP_SLOT
constexpr GUID IID_ICompositor{0xB403CA50, 0x7F8C, 0x4E83, {0x98, 0x5F, 0xCC, 0x45, 0x06, 0x00, 0x36, 0xD8}};
constexpr GUID IID_ICompositor3{0xC9DD8EF0, 0x6EB1, 0x4E3C, {0xA6, 0x58, 0x67, 0x5D, 0x9C, 0x64, 0xD4, 0xAB}};
constexpr GUID IID_ICompositor5{0x48EA31AD, 0x7FCD, 0x4076, {0xA7, 0x9C, 0x90, 0xCC, 0x4B, 0x85, 0x2C, 0x9B}};
constexpr GUID IID_ICompositor6{0x7A38B2BD, 0xCEC8, 0x4EEB, {0x83, 0x0F, 0xD8, 0xD0, 0x7A, 0xED, 0xEB, 0xC3}};
constexpr GUID IID_IVisual{0x117E202D, 0xA859, 0x4C89, {0x87, 0x3B, 0xC2, 0xAA, 0x56, 0x67, 0x88, 0xE3}};
constexpr GUID IID_ICompositionTarget{0xA1BEA8BA, 0xD726, 0x4663, {0x81, 0x29, 0x6B, 0x5E, 0x79, 0x27, 0xFF, 0xA6}};
constexpr GUID IID_ICompositionBrush{0xAB0D7608, 0x30C0, 0x40E9, {0xB5, 0x68, 0xB6, 0x0A, 0x6B, 0xD1, 0xFB, 0x46}};
constexpr GUID IID_ICompositionClip{0x1CCD2A52, 0xCFC7, 0x4ACE, {0x99, 0x83, 0x14, 0x6B, 0xB8, 0xEB, 0x6A, 0x3C}};
constexpr GUID IID_ICompositionGeometry{0xE985217C, 0x6A17, 0x4207, {0xAB, 0xD8, 0x5F, 0xD3, 0xDD, 0x61, 0x2A, 0x9D}};
constexpr GUID IID_ICompositorDesktopInterop{0x29E691FA, 0x4567, 0x4DCA, {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};
}  // namespace comp

constexpr WCHAR kGlassClass[] = L"WindhawkMacLaunchpadGlass";
constexpr int kGlassPieces = 5;   // Siri: bar, power button, power options, panel; + the context menu
HWND g_glassWnd;
bool g_glassOk;   // the live blur is ready (otherwise the blurred snapshot is used)

struct GlassParts {
    ComPtr<IUnknown> queue;   // the dispatcher queue Windows.UI.Composition needs on this thread
    ComPtr<comp::ICompositor> compositor;
    ComPtr<IInspectable> target;
    ComPtr<IUnknown> backdrop;   // ICompositionBrush
    ComPtr<comp::IVisual> visual[kGlassPieces];
    ComPtr<comp::IRoundedRectangleGeometry> shape[kGlassPieces];
};
[[clang::no_destroy]] GlassParts g_glass;

LRESULT CALLBACK GlassWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool TransparencyEffectsOn() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"EnableTransparency",
                 RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value != 0;
}

bool BuildGlassComposition() {
    struct QueueOptions { DWORD size; int threadType; int apartmentType; };
    using CreateQueue_t = HRESULT(WINAPI*)(QueueOptions, IUnknown**);
    using Activate_t = HRESULT(WINAPI*)(HSTRING, IInspectable**);
    using MakeString_t = HRESULT(WINAPI*)(PCWSTR, UINT32, HSTRING_HEADER*, HSTRING*);
    HMODULE messaging = LoadLibraryExW(L"CoreMessaging.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE combase = GetModuleHandleW(L"combase.dll");
    auto createQueue = messaging ? (CreateQueue_t)GetProcAddress(messaging, "CreateDispatcherQueueController") : nullptr;
    auto activate = combase ? (Activate_t)GetProcAddress(combase, "RoActivateInstance") : nullptr;
    auto makeString = combase ? (MakeString_t)GetProcAddress(combase, "WindowsCreateStringReference") : nullptr;
    if (!createQueue || !activate || !makeString) return false;
    if (FAILED(createQueue(QueueOptions{sizeof(QueueOptions), 2, 2}, g_glass.queue.GetAddressOf()))) return false;   // current thread, STA

    static constexpr WCHAR kClass[] = L"Windows.UI.Composition.Compositor";
    HSTRING_HEADER header;
    HSTRING name = nullptr;
    ComPtr<IInspectable> instance;
    if (FAILED(makeString(kClass, (UINT32)wcslen(kClass), &header, &name)) || FAILED(activate(name, &instance)) ||
        FAILED(instance->QueryInterface(comp::IID_ICompositor, (void**)g_glass.compositor.GetAddressOf())))
        return false;
    ComPtr<comp::ICompositor3> compositor3;
    ComPtr<comp::ICompositor5> compositor5;
    ComPtr<comp::ICompositor6> compositor6;
    ComPtr<comp::ICompositorDesktopInterop> interop;
    ComPtr<IInspectable> backdrop;
    if (FAILED(g_glass.compositor->QueryInterface(comp::IID_ICompositor3, (void**)compositor3.GetAddressOf())) ||
        FAILED(g_glass.compositor->QueryInterface(comp::IID_ICompositor5, (void**)compositor5.GetAddressOf())) ||
        FAILED(g_glass.compositor->QueryInterface(comp::IID_ICompositor6, (void**)compositor6.GetAddressOf())) ||
        FAILED(g_glass.compositor->QueryInterface(comp::IID_ICompositorDesktopInterop, (void**)interop.GetAddressOf())) ||
        FAILED(compositor3->CreateHostBackdropBrush(&backdrop)) ||
        FAILED(backdrop->QueryInterface(comp::IID_ICompositionBrush, (void**)g_glass.backdrop.GetAddressOf())) ||
        FAILED(interop->CreateDesktopWindowTarget(g_glassWnd, TRUE, &g_glass.target)))
        return false;

    ComPtr<comp::IContainerVisual> root;
    ComPtr<comp::IVisual> rootVisual;
    ComPtr<comp::ICompositionTarget> target;
    ComPtr<comp::IVisualCollection> children;
    if (FAILED(g_glass.compositor->CreateContainerVisual(&root)) ||
        FAILED(root->QueryInterface(comp::IID_IVisual, (void**)rootVisual.GetAddressOf())) ||
        FAILED(g_glass.target->QueryInterface(comp::IID_ICompositionTarget, (void**)target.GetAddressOf())) ||
        FAILED(target->put_Root(rootVisual.Get())) || FAILED(root->get_Children(&children)))
        return false;
    for (int i = 0; i < kGlassPieces; i++) {
        ComPtr<comp::ISpriteVisual> sprite;
        ComPtr<IUnknown> geometry, clip;
        ComPtr<IInspectable> geometricClip;
        if (FAILED(g_glass.compositor->CreateSpriteVisual(&sprite)) || FAILED(sprite->put_Brush(g_glass.backdrop.Get())) ||
            FAILED(sprite->QueryInterface(comp::IID_IVisual, (void**)g_glass.visual[i].GetAddressOf())) ||
            FAILED(compositor5->CreateRoundedRectangleGeometry(&g_glass.shape[i])) ||
            FAILED(g_glass.shape[i]->QueryInterface(comp::IID_ICompositionGeometry, (void**)geometry.GetAddressOf())) ||
            FAILED(compositor6->CreateGeometricClipWithGeometry(geometry.Get(), &geometricClip)) ||
            FAILED(geometricClip->QueryInterface(comp::IID_ICompositionClip, (void**)clip.GetAddressOf())) ||
            FAILED(g_glass.visual[i]->put_Clip(clip.Get())) || FAILED(g_glass.visual[i]->put_IsVisible(0)) ||
            FAILED(children->InsertAtTop(g_glass.visual[i].Get())))
            return false;
    }
    return true;
}

void ReleaseGlassComposition() {
    for (int i = 0; i < kGlassPieces; i++) { g_glass.visual[i].Reset(); g_glass.shape[i].Reset(); }
    g_glass.backdrop.Reset(); g_glass.target.Reset(); g_glass.compositor.Reset();
    // The queue is shut down before it goes: its last messages are pumped here (briefly)
    ComPtr<comp::IDispatcherQueueController> controller;
    if (g_glass.queue && SUCCEEDED(g_glass.queue->QueryInterface(comp::IID_IDispatcherQueueController, (void**)controller.GetAddressOf()))) {
        ComPtr<IInspectable> operation;
        if (SUCCEEDED(controller->ShutdownQueueAsync(&operation))) {
            for (int i = 0; i < 20; i++) {
                MSG msg;
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) DispatchMessageW(&msg);
                Sleep(5);
            }
        }
    }
    g_glass.queue.Reset();
}

void CreateGlassWindow(HINSTANCE instance) {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = GlassWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = kGlassClass;
    RegisterClassExW(&wc);
    g_glassWnd = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
                                 WS_EX_NOACTIVATE, kGlassClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    if (!g_glassWnd) return;
    SetLayeredWindowAttributes(g_glassWnd, 0, 255, LWA_ALPHA);
    BOOL hostBackdrop = TRUE;   // DWMWA_USE_HOSTBACKDROPBRUSH: lets a desktop app show Windows' blur of what is behind
    g_glassOk = SUCCEEDED(DwmSetWindowAttribute(g_glassWnd, 17, &hostBackdrop, sizeof(hostBackdrop))) && BuildGlassComposition();
    if (!g_glassOk) {
        Wh_Log(L"Live blur unavailable, using a blurred snapshot instead");
        ReleaseGlassComposition();
    }
}

void DestroyGlassWindow() {
    ReleaseGlassComposition();
    if (g_glassWnd) { DestroyWindow(g_glassWnd); g_glassWnd = nullptr; }
    g_glassOk = false;
}

void HideGlass() {
    if (g_glassWnd && IsWindowVisible(g_glassWnd)) ShowWindow(g_glassWnd, SW_HIDE);
}

struct GlassPiece { D2D1_RECT_F r; float radius; };

// The shapes the live blur goes under (window coordinates, after the opening animation transform)
void CollectGlassPieces(const Layout& L, const D2D1::Matrix3x2F& m, std::vector<GlassPiece>& out, float& alpha) {
    float s = L.scale;
    alpha = 1.0f;
    if (g_settings.siri) {
        float ia = PillIntroAlpha(), pa = std::min(1.0f, g_siriAnim * 4);
        if (ia > 0.02f) out.push_back({L.search, (L.search.bottom - L.search.top) / 2});
        if (L.btnPower.right > L.btnPower.left && g_siriBtnAnim > 0.3f) out.push_back({L.btnPower, (L.btnPower.right - L.btnPower.left) / 2});
        float powerH = L.siriPower.bottom - L.siriPower.top;
        if (powerH > 2) out.push_back({L.siriPower, powerH / 2});
        float panelW = L.siriPanel.right - L.siriPanel.left, panelH = L.siriPanel.bottom - L.siriPanel.top;
        float ex = SiriExpandEase();
        if (ex > 0.001f && panelH > 1 && panelW > 1) {
            float pillR = (L.search.bottom - L.search.top) / 2;
            out.push_back({L.siriPanel, std::min({Lerp(pillR, g_settings.siriPanelRadius * s, ex), panelH / 2, panelW / 2})});
        }
        alpha = std::max(ia, out.size() > 1 ? pa : 0.0f);
    } else {
        float inset = WindowInset(s), radius = g_settings.cornerRadius * s;
        if (inset > 0) radius = std::max(0.0f, radius - inset * 0.25f);
        D2D1_POINT_2F a = m.TransformPoint(D2D1::Point2F(inset, inset));
        D2D1_POINT_2F b = m.TransformPoint(D2D1::Point2F((float)L.width - inset, (float)L.height - inset));
        out.push_back({D2D1::RectF(std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x), std::max(a.y, b.y)), radius * std::fabs(m._11)});
    }
    if (g_menu.open) out.push_back({g_menu.rect, (g_menu.category ? 24 : 14) * s});
}

// Cuts the live blur to the shapes and keeps the glass window right under ours
void UpdateGlass(const Layout& L, const D2D1::Matrix3x2F& m, float windowAlpha) {
    if (!g_glassWnd) return;
    if (!LiveBlurOn() || !g_shown) { HideGlass(); return; }
    std::vector<GlassPiece> pieces;
    float alpha = 1.0f;
    CollectGlassPieces(L, m, pieces, alpha);
    alpha *= windowAlpha * (0.35f + 0.65f * g_settings.blurAmount / 100.0f);   // weaker blur = more of the sharp background
    comp::Vec2 whole{(float)L.width, (float)(L.height + L.extraBottom)};
    for (int i = 0; i < kGlassPieces; i++) {
        comp::IVisual* visual = g_glass.visual[i].Get();
        if (i >= (int)pieces.size() || alpha <= 0.01f) { visual->put_IsVisible(0); continue; }
        const GlassPiece& p = pieces[i];
        float w = std::max(0.0f, p.r.right - p.r.left), h = std::max(0.0f, p.r.bottom - p.r.top);
        float radius = std::min(p.radius, std::min(w, h) / 2);
        g_glass.shape[i]->put_Offset({p.r.left, p.r.top});
        g_glass.shape[i]->put_Size({w, h});
        g_glass.shape[i]->put_CornerRadius({radius, radius});
        visual->put_Size(whole);
        visual->put_Opacity(std::clamp(alpha, 0.0f, 1.0f));
        visual->put_IsVisible(1);
    }
    SetWindowPos(g_glassWnd, g_wnd, L.area.left, L.area.top, L.width, L.height + L.extraBottom,
                 SWP_NOACTIVATE | (IsWindowVisible(g_wnd) ? SWP_SHOWWINDOW : 0));
}

void Render() {
    if (!g_shown || !EnsureTarget()) return;
    const Layout& L = g_layout;
    if (!EnsureSurface(g_surface, L.width, L.height + L.extraBottom)) return;
    RECT bind{0, 0, L.width, L.height + L.extraBottom};
    if (FAILED(g_target->BindDC(g_surface.dc, &bind))) return;
    float W = (float)L.width, H = (float)L.height;
    float radius = g_settings.cornerRadius * L.scale;

    g_target->BeginDraw(); g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    g_target->Clear(D2D1::ColorF(0, 0, 0, 0));
    if (g_settings.fullscreen) Fill(D2D1::RectF(0, 0, W, H), D2D1::ColorF(0, 0, 0, 1 / 255.0f));
    AnimFx fx = ComputeAnim();
    g_target->SetTransform(fx.m);

    if (g_settings.siri) DrawSiriShapes(L);
    const bool themed = !g_settings.siri && ActiveTheme() != Theme::Custom;
    if (themed) {
        // Window mode keeps a margin so the theme shadow fits inside the window
        float m = WindowInset(L.scale);
        DrawThemedShape(D2D1::RectF(m, m, W - m, H - m), std::max(0.0f, radius - m * 0.25f), GetBackdropBitmap(), !g_settings.fullscreen);
    }
    if (!themed && BlurStrength() > 0 && !g_settings.siri) {
        if (ID2D1Bitmap* bitmap = GetBackdropBitmap()) {
            ComPtr<ID2D1RoundedRectangleGeometry> clipGeo;
            g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), radius, radius), &clipGeo);
            g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeo.Get()), nullptr);
            g_target->DrawBitmap(bitmap, D2D1::RectF(0, 0, W, H), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
            g_target->PopLayer();
        }
    }

    if (!g_settings.siri && !themed) {
        g_brush->SetColor(Alpha(g_settings.tint, g_settings.opacity));
        g_target->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), radius, radius), g_brush.Get());
        DrawGlass(D2D1::RectF(0, 0, W, H), radius);
    }

    if (!g_settings.fullscreen && !g_settings.siri && !themed && g_settings.borderWidth > 0) {
        float bw = g_settings.borderWidth * L.scale, h = bw / 2, rr = std::max(0.0f, radius - h);
        g_brush->SetColor(Alpha(g_settings.borderColor, g_settings.borderOpacity));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(h, h, W - h, H - h), rr, rr), g_brush.Get(), bw);
    }

    D2D1::Matrix3x2F zoomMatrix = fx.m;

    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        float t = EaseInOut(g_folderAnim);
        bool settled = FolderSettled();
        float s = L.scale;
        float bgScale = 1.0f - 0.05f * EaseInOut(t);
        D2D1_POINT_2F center = D2D1::Point2F(W / 2, H / 2);
        D2D1::Matrix3x2F bgMatrix = D2D1::Matrix3x2F::Scale(bgScale, bgScale, center) * zoomMatrix;
        float bgFade = 1.0f - 0.80f * EaseInOut(t);
        auto drawBackground = [&] {
            g_target->SetTransform(bgMatrix);
            DrawSearchBar(g_mainLayout, 1.0f - 0.85f * t);
            DrawPowerButton(g_mainLayout, 1.0f - t);
            DrawMainGrid(g_mainLayout, g_mainItems, g_mainScroll, bgFade, false, 1000 + g_openFolderIndex, t);
        };
        D2D1_RECT_F from = D2D1::RectF(
            center.x + (g_folderOrigin.left - center.x) * bgScale, center.y + (g_folderOrigin.top - center.y) * bgScale,
            center.x + (g_folderOrigin.right - center.x) * bgScale, center.y + (g_folderOrigin.bottom - center.y) * bgScale);

        float sigma = g_settings.folderBlur * 0.3f * s * t;
        bool blurred = false;
        ComPtr<ID2D1DeviceContext> dc;
        if (sigma > 0.5f && SUCCEEDED(g_target.As(&dc))) {
            if (ID2D1Bitmap1* layer = EnsureLayerBitmap(dc.Get(), L.width, L.height)) {
                ComPtr<ID2D1Image> old; dc->GetTarget(&old);
                dc->SetTarget(layer); dc->Clear(D2D1::ColorF(0, 0, 0, 0));
                drawBackground();
                dc->SetTarget(old.Get());
                g_target->SetTransform(D2D1::Matrix3x2F::Identity());

                D2D1_RECT_F panel = LerpRect(from, L.panel, t);
                float pr = Lerp(16 * s, g_settings.folderRadius * s, t);
                ComPtr<ID2D1RoundedRectangleGeometry> panelGeo;
                ComPtr<ID2D1RectangleGeometry> fullGeo;
                ComPtr<ID2D1PathGeometry> outside; ComPtr<ID2D1GeometrySink> sink;
                ComPtr<ID2D1Effect> blur;
                g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(panel, pr, pr), &panelGeo);
                g_d2d->CreateRectangleGeometry(D2D1::RectF(-W, -H, 2 * W, 2 * H), &fullGeo);
                if (panelGeo && fullGeo && SUCCEEDED(g_d2d->CreatePathGeometry(&outside)) && SUCCEEDED(outside->Open(&sink)) &&
                    SUCCEEDED(fullGeo->CombineWithGeometry(panelGeo.Get(), D2D1_COMBINE_MODE_EXCLUDE, nullptr, sink.Get())) &&
                    SUCCEEDED(sink->Close()) && SUCCEEDED(dc->CreateEffect(kClsidGaussianBlur, &blur))) {
                    blur->SetInput(0, layer);
                    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma);
                    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
                    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), outside.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, zoomMatrix), nullptr);
                    g_target->DrawBitmap(layer, D2D1::RectF(0, 0, W, H));
                    g_target->PopLayer();
                    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), panelGeo.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, zoomMatrix), nullptr);
                    dc->DrawImage(blur.Get());
                    g_target->PopLayer();
                    blurred = true;
                }
            }
        }
        if (!blurred) drawBackground();

        g_target->SetTransform(zoomMatrix);
        DrawFolderPanel(t, from, 10 * s * bgScale, settled);
        DrawFolderHeader(L, std::clamp((t - 0.35f) / 0.65f, 0.0f, 1.0f));
    } else if (g_settings.siri) {
        g_target->SetTransform(zoomMatrix);
        DrawSiriPillContent(L);
        DrawPowerPill(L);
        float ca = SiriContentAlpha();
        if (ca > 0.001f) {
            PushOpacity(ca);
            DrawTabs(L);
            DrawMainGrid(L, g_layoutItems, g_scroll, 1.0f, true, -1, 0.0f);
            g_target->PopLayer();
        }
    } else {
        g_target->SetTransform(zoomMatrix);
        DrawSearchBar(L, 1.0f);
        DrawPowerButton(L, 1.0f);
        float te = TabsVisible() ? Ease(g_tabsAnim) : 0.0f;
        if (te > 0.001f) { PushOpacity(te); DrawTabs(L); g_target->PopLayer(); }
        DrawMainGrid(L, g_layoutItems, g_scroll, 1.0f, true, -1, 0.0f);
    }
    DrawPopupMenu(fx.m);
    if (fx.flash > 0.001f && !g_settings.siri) { g_target->SetTransform(fx.m); Fill(D2D1::RectF(0, 0, W, H), D2D1::ColorF(1, 1, 1, fx.flash), radius); }
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());

    if (g_target->EndDraw() == D2DERR_RECREATE_TARGET) { ResetTarget(); return; }
    POINT dst{L.area.left, L.area.top}, src{0, 0}; SIZE size{L.width, L.height + L.extraBottom};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)std::lround(255 * std::clamp(fx.alpha, 0.0f, 1.0f)), AC_SRC_ALPHA};
    UpdateLayeredWindow(g_wnd, nullptr, &dst, &size, g_surface.dc, &src, 0, &blend, ULW_ALPHA);
    UpdateGlass(L, fx.m, std::clamp(fx.alpha, 0.0f, 1.0f));
    if (JiggleActive() && !g_frameTimer) StartFrames();
}

// Frames are paced by a high-resolution waitable timer at the monitor refresh rate
// (the classic WM_TIMER only ticks every ~15.6 ms, which caps animations near 64 fps)
int FrameIntervalMs() {
    DEVMODEW mode{};
    mode.dmSize = sizeof(mode);
    int hz = EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &mode) ? (int)mode.dmDisplayFrequency : 60;
    return std::clamp(hz > 1 ? 1000 / hz : 16, 4, 16);
}

void StartFrames() {
    if (g_frameTimer) return;
    g_frameTimer = true;
    g_lastFrame = Now();
    int interval = FrameIntervalMs();
    if (g_frameWaitTimer) {
        LARGE_INTEGER due;
        due.QuadPart = -10000LL * interval;
        SetWaitableTimer(g_frameWaitTimer, &due, interval, nullptr, nullptr, FALSE);
    } else {
        SetTimer(g_wnd, kTimerFrame, interval, nullptr);
    }
}

void StopFrames() {
    if (g_frameWaitTimer) CancelWaitableTimer(g_frameWaitTimer);
    else KillTimer(g_wnd, kTimerFrame);
    g_frameTimer = false;
}

void ExpandSiri() {
    if (!g_settings.siri || g_siriClosingPill) return;
    if (g_pillIntro < 1.0f) g_pillIntro = std::max(g_pillIntro, kIntroCircle);
    // Original Siri: opening the panel always means a category (typing straight away = Apps), so the back arrow / Esc work
    if (g_settings.siriOriginal && !g_siriChosen) { g_siriChosen = true; g_flyAnim = 1.0f; }
    if (g_siriExpanded) return;
    g_siriExpanded = true;
    StartFrames();
}

void CollapseSiri() {
    if (!g_siriExpanded) return;
    g_siriExpanded = false;
    g_hover = g_selected = -1;
    StartFrames();
}

void BeginRename() {
    if (g_openFolderIndex < 0 || g_openFolderIndex >= (int)g_folders.size()) return;
    g_renaming = true; g_renameSelectAll = true;
    g_renameText = g_folders[g_openFolderIndex].name;
    Render();
}

void CommitRename() {
    if (!g_renaming) return;
    g_renaming = false;
    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size()) {
        std::wstring name = g_renameText;
        name.erase(0, name.find_first_not_of(L' '));
        name.erase(name.find_last_not_of(L' ') + 1);
        if (!name.empty()) { g_folders[g_openFolderIndex].name = name; SaveFolders(); }
    }
    Render();
}

void CancelRename() {
    if (!g_renaming) return;
    g_renaming = false; Render();
}

void OpenFolder(int folderIndex) {
    if (folderIndex < 0 || folderIndex >= (int)g_folders.size() || g_openFolderIndex >= 0) return;

    g_mainLayout = g_layout; g_mainScroll = g_scroll; g_mainItems = g_layoutItems;

    int layoutIdx = -1;
    for (int i = 0; i < (int)g_mainItems.size(); i++) {
        if (g_mainItems[i] == 1000 + folderIndex) { layoutIdx = i; break; }
    }
    if (layoutIdx >= 0) {
        g_folderOrigin = CellAtIn(g_mainLayout, g_mainScroll, layoutIdx).icon;
    } else {
        float cx = g_mainLayout.width / 2.0f, cy = g_mainLayout.height / 2.0f, h = g_mainLayout.icon / 2;
        g_folderOrigin = D2D1::RectF(cx - h, cy - h, cx + h, cy + h);
    }

    g_openFolderIndex = folderIndex;
    g_folderAnim = 0.0f; g_folderClosing = false;
    ClearQuery(); g_hover = g_selected = g_pressed = -1; g_scroll = g_scrollTarget = 0;
    g_dotsHover = false; g_dotHover = -1; g_powerHover = false;
    RebuildLayout(); ComputeLayout(); Render(); StartFrames();
}

void FinishFolderClose() {
    g_openFolderIndex = -1; g_folderClosing = false; g_folderAnim = 0.0f; g_renaming = false;
    g_hover = g_selected = g_pressed = -1; g_dotsHover = false; g_dotHover = -1;
    g_scroll = g_scrollTarget = g_mainScroll;
    RebuildLayout(); ComputeLayout();
}

void CloseFolder(bool animate = true) {
    if (g_openFolderIndex < 0) return;
    CommitRename();
    if (!animate) { FinishFolderClose(); return; }
    if (g_folderClosing) return;
    g_folderClosing = true; g_hover = g_selected = g_pressed = -1;
    StartFrames();
}

void HideNow() {
    ShowWindow(g_wnd, SW_HIDE);
    HideGlass();
    g_openFolderIndex = -1; g_folderAnim = 0.0f; g_folderClosing = false; g_renaming = false; g_mainItems.clear();
    g_shown = false; g_closing = false; g_appear = 0; g_backdropImage = {}; FreeSurface(g_surface); g_trackingMouse = false;
    g_menu.open = false; g_selection.clear(); g_powerHover = false;
    g_clearHover = false; g_histClearHover = false; g_backHover = false;
    g_pickerChip = kNoChip; g_chipRename = -1; g_chipPress = false;
    ResetPowerPill();
    g_siriExpanded = false; g_siriAnim = 0; g_siriBtnAnim = 0; g_pillIntro = 1.0f; g_siriClosingPill = false;
    g_searchActive = false; g_tabsAnim = 0; g_siriTab = 0; g_tabSlide = 0;
    KillTimer(g_wnd, kTimerFileSearch); CancelFileSearch();
    g_tabItems.clear();
}

void OnFrame() {
    double now = Now(); float dt = (float)std::clamp(now - g_lastFrame, 0.0, 0.05); g_lastFrame = now;
    bool animating = false;
    float target = g_closing ? 0.0f : 1.0f;
    if (g_settings.siri) {
        g_appear = 1.0f;
    } else if (g_appear != target) {
        float speed = g_settings.animSpeed / AnimSeconds(g_closing ? g_settings.closeAnim : g_settings.openAnim, g_closing);
        g_appear = g_closing ? std::max(0.0f, g_appear - dt * speed) : std::min(1.0f, g_appear + dt * speed);
        animating = g_appear != target;
    }
    if (g_scroll != g_scrollTarget && !g_swiping) {
        float k = (g_layout.horizontal ? 10.0f : 16.0f) * g_settings.animSpeed;
        g_scroll += (g_scrollTarget - g_scroll) * std::min(1.0f, dt * k);
        if (std::abs(g_scrollTarget - g_scroll) < 0.5f) g_scroll = g_scrollTarget;
        animating |= g_scroll != g_scrollTarget;
    }
    {
        float hvTarget = g_dotsHover ? 1.0f : 0.0f, st = dt * 7 * g_settings.animSpeed;
        if (g_dotsHoverAnim != hvTarget) {
            g_dotsHoverAnim = hvTarget > g_dotsHoverAnim ? std::min(hvTarget, g_dotsHoverAnim + st) : std::max(hvTarget, g_dotsHoverAnim - st);
            animating |= g_dotsHoverAnim != hvTarget;
        }
    }
    if (g_menu.open) {
        float tgt = g_menu.closing ? 0.0f : 1.0f, st = dt * g_settings.animSpeed / (g_menu.closing ? 0.10f : 0.15f);
        g_menu.anim = g_menu.closing ? std::max(0.0f, g_menu.anim - st) : std::min(1.0f, g_menu.anim + st);
        if (g_menu.closing && g_menu.anim <= 0) g_menu.open = false;
        animating |= g_menu.open && g_menu.anim != tgt;
    }
    if (g_openFolderIndex >= 0) {
        float folderTarget = g_folderClosing ? 0.0f : 1.0f;
        if (g_folderAnim != folderTarget) {
            float step = dt * g_settings.animSpeed / (g_folderClosing ? kFolderCloseSeconds : kFolderOpenSeconds);
            g_folderAnim = g_folderClosing ? std::max(0.0f, g_folderAnim - step) : std::min(1.0f, g_folderAnim + step);
            animating |= g_folderAnim != folderTarget;
        }
        if (g_folderClosing && g_folderAnim <= 0.0f) FinishFolderClose();
    }

    // Tab indicator slides to the slot of the current tab among the visible ones
    {
        float slot = (float)TabSlot(g_siriTab);
        if (g_tabSlide != slot) {
            g_tabSlide += (slot - g_tabSlide) * std::min(1.0f, dt * 14 * g_settings.animSpeed);
            if (std::abs(slot - g_tabSlide) < 0.01f) g_tabSlide = slot;
            animating |= g_tabSlide != slot;
        }
    }

    // Window / Fullscreen: category strip slides in/out
    if (!g_settings.siri) {
        float tt = g_searchActive && g_openFolderIndex < 0 && TabsVisible() ? 1.0f : 0.0f;
        if (g_tabsAnim != tt) {
            float step = dt * g_settings.animSpeed / 0.22f;
            g_tabsAnim = tt > g_tabsAnim ? std::min(tt, g_tabsAnim + step) : std::max(tt, g_tabsAnim - step);
            ComputeLayout();
            animating |= g_tabsAnim != tt;
        }
    }

    if (g_settings.siri) {
        bool relayout = false;
        if (g_siriClosingPill) {
            if (g_siriAnim <= 0.0f && g_powerPillAnim <= 0.0f && g_siriBtnAnim <= 0.0f && g_pillIntro > 0.0f) {
                if (g_settings.siriPillOutroMs == 0) g_pillIntro = 0.0f;
                else {
                    float dur = std::max(1, g_settings.siriPillOutroMs) / 1000.0f;
                    g_pillIntro = std::max(0.0f, g_pillIntro - dt * g_settings.animSpeed / dur);
                }
                relayout = true;
            }
            animating = true;
        } else if (g_pillIntro < 1.0f) {
            float dur = std::max(1, g_settings.siriPillIntroMs) / 1000.0f;
            g_pillIntro = std::min(1.0f, g_pillIntro + dt * g_settings.animSpeed / dur);
            relayout = true;
            animating |= g_pillIntro < 1.0f;
        }
        if (g_flyAnim < 1.0f) {
            g_flyAnim = std::min(1.0f, g_flyAnim + dt * g_settings.animSpeed / 0.42f);
            animating = true;
        }
        float btnTarget = g_settings.siriPowerButton && !g_siriClosingPill && g_pillIntro >= 1.0f ? 1.0f : 0.0f;
        if (g_siriBtnAnim != btnTarget) {
            float step = dt * g_settings.animSpeed / 0.34f;
            g_siriBtnAnim = btnTarget > g_siriBtnAnim ? std::min(btnTarget, g_siriBtnAnim + step) : std::max(btnTarget, g_siriBtnAnim - step);
            relayout = true;
            animating = true;
        }
        float expandTarget = g_siriExpanded ? 1.0f : 0.0f;
        if (g_siriAnim != expandTarget) {
            float step = dt * g_settings.animSpeed / (g_siriExpanded ? 0.34f : 0.24f);
            g_siriAnim = g_siriExpanded ? std::min(1.0f, g_siriAnim + step) : std::max(0.0f, g_siriAnim - step);
            relayout = true;
            animating |= g_siriAnim != expandTarget;
        }
        float powerTarget = g_powerPillWanted ? 1.0f : 0.0f, powerStep = dt * 5 * g_settings.animSpeed;
        if (g_powerPillAnim != powerTarget) {
            if (powerTarget > g_powerPillAnim) g_powerPillAnim = std::min(powerTarget, g_powerPillAnim + powerStep);
            else g_powerPillAnim = std::max(powerTarget, g_powerPillAnim - powerStep);
            relayout = true;
            animating |= g_powerPillAnim != powerTarget;
        }
        if (relayout) ComputeLayout();
    }
    if (JiggleActive()) animating = true;
    bool done = g_settings.siri ? (g_closing && g_siriClosingPill && g_pillIntro <= 0.0f)
                                : (g_closing && g_appear <= 0);
    if (done) { HideNow(); animating = false; }
    else Render();
    if (!animating) StopFrames();
}
////////////////////////////////////////////////////////////////////////////////
// Actions

void ForceForeground(HWND hwnd) { INPUT input{}; SendInput(1, &input, sizeof(INPUT)); SetForegroundWindow(hwnd); SetFocus(hwnd); }

void PlaceWindows() {
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (g_settings.fullscreen) RaiseDockWindows();
}

void Open() {
    if (g_shown) {
        if (g_closing) {
            g_closing = false;
            g_siriClosingPill = false;
            ForceForeground(g_wnd); PlaceWindows(); StartFrames();
        }
        return;
    }
    POINT pt; GetCursorPos(&pt); HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96; GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    Layout& L = g_layout;
    if (g_settings.siri) {
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(monitor, &mi);
        float sc = dpiX / 96.0f;
        int workWidth = mi.rcWork.right - mi.rcWork.left, workHeight = mi.rcWork.bottom - mi.rcWork.top;
        float barW = g_settings.siriPillW * sc + (g_settings.siriPowerButton ? 2 * (SiriButtonGap(sc) + SiriButtonSize(sc)) : 0.0f);
        float boxW = g_settings.siriPanelW * sc;
        if (g_settings.siriPowerButton && !g_settings.siriPower) boxW += 2 * (12 + 230) * sc;   // the power menu opens beside the box
        int wantW = (int)std::lround(std::max(barW, boxW) + 2 * SiriShadowMargin(sc));
        int winWidth = std::min(workWidth, wantW);
        int winHeight = std::min(workHeight, SiriWindowHeight(sc));
        L.area.left = mi.rcWork.left + (workWidth - winWidth) / 2;
        int topPad = (int)SiriTopPad(sc);
        L.area.top = std::max(mi.rcWork.top, std::min(mi.rcWork.top + workHeight * 16 / 100 - topPad, mi.rcWork.bottom - winHeight));
        L.area.right = L.area.left + winWidth;
        L.area.bottom = L.area.top + winHeight;
    } else if (g_settings.fullscreen) {
        L.area = AvailableArea(monitor);
    } else {
        MONITORINFO mi{sizeof(mi)}; GetMonitorInfoW(monitor, &mi);
        int workWidth = mi.rcWork.right - mi.rcWork.left, workHeight = mi.rcWork.bottom - mi.rcWork.top;
        int winWidth = (workWidth * g_settings.windowWidth) / 100, winHeight = (workHeight * g_settings.windowHeight) / 100;
        L.area.left = mi.rcWork.left + (workWidth - winWidth) / 2; L.area.top = mi.rcWork.top + (workHeight - winHeight) / 2;
        L.area.right = L.area.left + winWidth; L.area.bottom = L.area.top + winHeight;
    }
    L.width = L.area.right - L.area.left; L.height = L.area.bottom - L.area.top;
    L.scale = dpiX / 96.0f; EnsureFormats(L.scale);
    {
        MONITORINFO mi{sizeof(mi)}; GetMonitorInfoW(monitor, &mi);
        g_slideDistance = (float)(mi.rcMonitor.bottom - L.area.top);
        bool slide = !g_settings.siri && (g_settings.openAnim == Anim::Slide || g_settings.closeAnim == Anim::Slide);
        L.extraBottom = slide ? (int)std::max<LONG>(0, mi.rcMonitor.bottom - L.area.bottom) : 0;
    }
    g_menu.open = false; g_selection.clear();
    ClearQuery(); g_openFolderIndex = -1; g_folderAnim = 0.0f; g_folderClosing = false; g_renaming = false;
    g_hover = g_selected = g_pressed = -1; g_scroll = g_scrollTarget = 0; g_wheelDelta = 0; g_dragItem = -1; g_isDragging = false;
    g_dotsHover = false; g_dotHover = -1; g_dotsHoverAnim = 0; g_swiping = false; g_powerHover = false;
    g_clearHover = false; g_histClearHover = false; g_backHover = false;
    ResetPowerPill();
    g_siriExpanded = false; g_siriAnim = 0; g_siriBtnAnim = 0; g_siriTab = 0; g_tabSlide = 0; g_siriClosingPill = false;
    g_appFilter = -1;
    g_pickerChip = kNoChip; g_chipRename = -1; g_chipScroll = 0; g_chipPress = false;
    g_siriChosen = false;
    g_flyAnim = 1.0f;
    g_searchActive = false; g_tabsAnim = 0;
    KillTimer(g_wnd, kTimerFileSearch); CancelFileSearch(); g_fileStatus.clear();
    g_pillIntro = g_settings.siri && g_settings.siriPillIntroMs > 0 ? 0.0f : 1.0f;
    LoadTabSource();
    RebuildLayout(); ComputeLayout();
    // Custom backdrop capture performs a full screen copy and CPU blur synchronously.
    // In Siri mode this can stall the pill animation and delay the first keystroke.
    // Keep the custom backdrop path for Window/Fullscreen, but skip it for Siri
    // so opening the search pill remains responsive.
    g_transparencyOn = TransparencyEffectsOn();
    if (BlurStrength() > 0)
        CaptureBackdrop(L.area, BlurStrength() / 100.0f * 60 * L.scale);
    g_shown = true; g_closing = false; g_appear = g_settings.siri ? 1.0f : 0.0f; Render(); ShowWindow(g_wnd, SW_SHOW);
    ForceForeground(g_wnd); PlaceWindows(); Render(); StartFrames();
    if (!g_appsLoaded || Now() - g_appsTime > 60 || g_appsIconPixels != IconPixels()) StartLoadingApps();
}

void Close() {
    if (!g_shown || g_closing) return;
    CommitRename();
    KillTimer(g_wnd, kTimerFileSearch); CancelFileSearch();
    g_closing = true; g_pressed = -1; g_swiping = false;
    if (GetCapture() == g_wnd) ReleaseCapture();
    if (g_settings.siri) {
        g_siriClosingPill = true;
        g_siriExpanded = false;
        g_powerPillWanted = false;
        g_powerPillHover = g_powerPillFocus = -1;
        g_hover = g_selected = -1;
        if (g_menu.open) { g_menu.closing = true; g_menu.hover = -1; }
        if (g_pillIntro > 1.0f) g_pillIntro = 1.0f;
    }
    StartFrames();
}

void Toggle() { if (g_shown && !g_closing) Close(); else Open(); }

void RunPowerChoice(int id);

void RunAction(const std::wstring& id) {
    if (id == L"lock") { RunPowerChoice(kMenuLock); return; }
    if (id == L"sleep") { RunPowerChoice(kMenuSleep); return; }
    if (id == L"restart") { RunPowerChoice(kMenuRestart); return; }
    if (id == L"shutdown") { RunPowerChoice(kMenuShutdown); return; }
    if (id == L"signout") { RunPowerChoice(kMenuSignOut); return; }
    Close();
    AllowSetForegroundWindow(ASFW_ANY);
    if (id == L"recycle") { SHEmptyRecycleBinW(nullptr, nullptr, 0); return; }
    PCWSTR file = id == L"settings" ? L"ms-settings:" : id == L"taskmgr" ? L"taskmgr.exe" : id == L"explorer" ? L"explorer.exe"
                : id == L"control" ? L"control.exe" : L"ms-screenclip:";
    ShellExecuteW(nullptr, L"open", file, nullptr, nullptr, SW_SHOWNORMAL);
}

void LaunchApp(int item) {
    if (item < 0) return;
    const App* ap = nullptr;
    if (g_openFolderIndex >= 0) {
        if (g_openFolderIndex >= (int)g_folders.size() || item >= (int)g_folders[g_openFolderIndex].apps.size()) return;
        ap = g_folders[g_openFolderIndex].apps[item].get();
    } else {
        if (IsFolderItem(item)) { OpenFolder(item - 1000); return; }
        ap = ResolveApp(item);
    }
    if (!ap) return;
    if (ap->kind == 2) { CopyText(ap->target); Close(); return; }
    if (ap->kind == 4) { RunAction(ap->target); return; }
    CountLaunch(*ap);
    if (ap->kind == 3) { SetQuery(ap->target); QueryChanged(); return; }
    if (ap->kind == 1 && g_siriTab == 1 && !g_query.empty()) AddSearchHistory(g_query);

    SHELLEXECUTEINFOW sei{sizeof(sei)}; sei.fMask = SEE_MASK_ASYNCOK; sei.lpFile = ap->target.c_str(); sei.nShow = SW_SHOWNORMAL;
    AllowSetForegroundWindow(ASFW_ANY); ShellExecuteExW(&sei); Close();
}

int CurrentPage() { return (int)std::lround(g_scrollTarget / std::max(1.0f, g_layout.pageWidth)); }

void GoToPage(int page) {
    g_scrollTarget = std::clamp(page, 0, g_layout.pages - 1) * g_layout.pageWidth;
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll()); StartFrames();
}

void EnsureVisible(int index) {
    if (index < 0) return;
    const Layout& L = g_layout;
    if (L.horizontal) { g_scrollTarget = std::clamp((index / L.perPage) * L.pageWidth, 0.0f, MaxScroll()); return; }
    int row = index / L.columns, firstRow = (int)std::lround(g_scrollTarget / L.cellHeight);
    if (row < firstRow) g_scrollTarget = row * L.cellHeight;
    else if (row >= firstRow + L.visibleRows) g_scrollTarget = (row - L.visibleRows + 1) * L.cellHeight;
    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
}

void ScrollRows(int rows) {
    if (g_layout.horizontal) { GoToPage(CurrentPage() + (rows > 0) - (rows < 0)); return; }
    float cell = g_layout.cellHeight, target = (std::lround(g_scrollTarget / cell) + rows) * cell;
    g_scrollTarget = std::clamp(target, 0.0f, MaxScroll()); StartFrames();
}

void UpdatePowerPill() {
    int pick = g_settings.siri && g_settings.siriPower && g_siriTab == 0 ? PowerChoiceForQuery(g_query) : -1;
    if (!g_query.empty()) g_powerPillByButton = false;   // typing takes over
    g_powerPillWanted = pick >= 0 || (g_powerPillByButton && g_settings.siri && g_settings.siriPower);
    if (pick >= 0) g_powerPillPick = pick;
    g_powerPillFocus = -1;
    StartFrames();
}

// Debounced file search (Files tab only)
void ScheduleFileSearch() {
    KillTimer(g_wnd, kTimerFileSearch);
    CancelFileSearch();
    g_fileStatus.clear();
    if (g_siriTab != 1 || g_query.size() < 2) return;
    g_fileSearching = true; // shows "Searching..." while waiting
    UINT delay = g_settings.indexer == Indexer::Everything || g_settings.indexer == Indexer::Auto ? 60 : kFileSearchDelayMs;
    SetTimer(g_wnd, kTimerFileSearch, delay, nullptr);

}

void RefreshTabItems(bool resetScroll) {
    LoadTabSource();
    RebuildLayout();
    g_hover = -1; g_selected = g_query.empty() || g_layoutItems.empty() ? -1 : 0;
    if (resetScroll) g_scroll = g_scrollTarget = 0;
    ComputeLayout(); Render();
}

void QueryChanged() {
    UpdatePowerPill();
    if (g_openFolderIndex >= 0) FinishFolderClose();
    if (!g_settings.siri && !g_query.empty() && !g_searchActive) { g_searchActive = true; StartFrames(); }
    ScheduleFileSearch();
    RefreshTabItems(true);
}

void SetSiriTab(int tab) {
    if (tab < 0 || tab > 3 || !TabEnabled(tab) || tab == g_siriTab) return;
    g_siriTab = tab;
    g_selection.clear();
    if (g_settings.siri) ExpandSiri(); else g_searchActive = true;
    UpdatePowerPill();
    ScheduleFileSearch();
    RefreshTabItems(true);
    StartFrames();
}

void ActivateSearch() {
    if (g_settings.siri || g_openFolderIndex >= 0 || g_searchActive) return;
    g_searchActive = true;
    if (g_wnd && GetForegroundWindow() == g_wnd) SetFocus(g_wnd);
    StartFrames(); Render();
}

void DeactivateSearch() {
    if (g_settings.siri || !g_searchActive) return;
    g_searchActive = false;
    if (g_siriTab != 0) { g_siriTab = 0; ScheduleFileSearch(); RefreshTabItems(true); }
    StartFrames(); Render();
}

void OnFilesFound(FileResult* res) {
    std::unique_ptr<FileResult> owner(res);
    if (res->gen != g_fileSearchGen || !g_shown || g_siriTab != 1 || g_query.empty()) return;
    g_fileResults = std::move(res->list);
    g_fileStatus = std::move(res->status); // e.g. "Everything is not running"
    g_fileSearching = false;
    RefreshTabItems(false);
}

void OnClipboardUpdate() {
    if (!g_settings.clipEnabled) return;   // off: nothing is monitored
    std::wstring text = ReadClipboardText(20000);
    if (text.find_first_not_of(L" \t\r\n") == std::wstring::npos) return;
    auto it = std::find(g_clipHistory.begin(), g_clipHistory.end(), text);
    if (it != g_clipHistory.end()) g_clipHistory.erase(it);
    g_clipHistory.insert(g_clipHistory.begin(), text);
    if ((int)g_clipHistory.size() > g_settings.clipMax) g_clipHistory.resize(g_settings.clipMax);
    if (g_shown && g_siriTab == 2) RefreshTabItems(false);
}

std::wstring ReadClipboardLine() {
    std::wstring line = ReadClipboardText(100000);
    return line.substr(0, line.find_first_of(L"\r\n"));
}

void PasteText() {
    ClampQueryCaret();
    DeleteQuerySelection();
    std::wstring paste = ReadClipboardLine().substr(0, 200);
    if (g_query.size() + paste.size() > 200) paste.resize(200 - g_query.size());
    g_query.insert(g_queryCaret, paste);
    g_queryCaret += paste.size(); g_queryAnchor = g_queryCaret;
    QueryChanged();
}

void CopyQueryToClipboard() {
    size_t a = QueryHasSelection() ? QuerySelStart() : 0;
    size_t b = QueryHasSelection() ? QuerySelEnd() : g_query.size();
    if (a >= b || !OpenClipboard(nullptr)) return;
    std::wstring selected = g_query.substr(a, b - a);
    EmptyClipboard();
    const SIZE_T bytes = (selected.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (memory) {
        void* dst = GlobalLock(memory);
        if (dst) {
            memcpy(dst, selected.c_str(), bytes);
            GlobalUnlock(memory);
            if (!SetClipboardData(CF_UNICODETEXT, memory)) GlobalFree(memory);
        } else GlobalFree(memory);
    }
    CloseClipboard();
}

void OnRenameChar(WCHAR ch) {
    auto dropSelection = [] { if (g_renameSelectAll) { g_renameText.clear(); g_renameSelectAll = false; } };
    switch (ch) {
        case 8:
            if (g_renameSelectAll) dropSelection();
            else if (!g_renameText.empty()) {
                g_renameText.pop_back();
                if (!g_renameText.empty() && IS_HIGH_SURROGATE(g_renameText.back())) g_renameText.pop_back();
            }
            break;
        case 0x7F: g_renameText.clear(); g_renameSelectAll = false; break;
        case 0x16: { std::wstring line = ReadClipboardLine(); if (!line.empty()) { dropSelection(); g_renameText += line; } break; }
        default: if (ch >= 32) { dropSelection(); g_renameText += ch; } break;
    }
    if (g_renameText.size() > 60) g_renameText.resize(60);
    Render();
}

void OnChar(WCHAR ch) {
    if (g_closing) return;
    if (g_menu.open && g_menu.category && !g_menu.closing) { g_menu.closing = true; g_menu.hover = -1; StartFrames(); }
    if (g_menu.open && !g_menu.category) return;
    if (g_renaming) { OnRenameChar(ch); return; }
    if (g_chipRename >= 0) {
        OnRenameChar(ch);
        if (g_renameText.size() > 24) g_renameText.resize(24);
        ComputeLayout(); EnsureChipVisible(kChipCustom + g_chipRename); Render();
        return;
    }
    bool edits = ch == 8 || ch == 0x7F || ch == 0x16 || ch >= 32;
    ClampQueryCaret();
    if (edits && g_openFolderIndex >= 0) { FinishFolderClose(); Render(); }
    if (edits && g_settings.siri) ExpandSiri();
    if (ch == 8) {
        if (QueryHasSelection()) DeleteQuerySelection();
        else if (g_queryCaret > 0) {
            size_t pos = g_queryCaret - 1;
            if (pos > 0 && IS_LOW_SURROGATE(g_query[pos]) && IS_HIGH_SURROGATE(g_query[pos - 1])) --pos;
            g_query.erase(pos, g_queryCaret - pos); g_queryCaret = g_queryAnchor = pos;
        } else return;
        QueryChanged(); return;
    }
    if (ch == 0x7F) {   // Ctrl+Backspace: delete the previous word
        if (QueryHasSelection()) DeleteQuerySelection();
        else if (g_queryCaret > 0) {
            size_t end = g_queryCaret, start = end;
            while (start > 0 && g_query[start - 1] == L' ') --start;
            while (start > 0 && g_query[start - 1] != L' ') --start;
            g_query.erase(start, end - start);
            g_queryCaret = g_queryAnchor = start;
        } else return;
        QueryChanged(); return;
    }
    if (ch == 0x16) { PasteText(); return; }
    if (ch >= 32 && g_query.size() < 200) {
        DeleteQuerySelection();
        g_query.insert(g_queryCaret, 1, ch); ++g_queryCaret; g_queryAnchor = g_queryCaret;
        QueryChanged();
    }
}

void MoveSelection(int delta) {
    if (g_layoutItems.empty()) return;
    int count = (int)g_layoutItems.size();
    if (g_selected < 0) g_selected = 0;
    else {
        int next = g_selected + delta;
        if (next >= 0 && next < count) g_selected = next;
        else if (delta > 0 && delta == g_layout.columns) g_selected = count - 1;
    }
    EnsureVisible(g_selected); Render(); StartFrames();
}

void CloseMenu();
void MenuStep(int d);
void ExecuteMenu(int id);
void RunPowerChoice(int id);

void BackToCategoryMenu();
void CommitChipRename();
void CancelChipRename();
void EndPicker(bool save);
void OnChipClick(int chip);
void OpenCategoryMenu();

void OnKeyDown(WPARAM key) {
    if (g_closing) return;
    bool categoryKey = g_menu.category && (GetKeyState(VK_CONTROL) & 0x8000) != 0;   // a category shortcut while the menu is open
    if (g_menu.open && !g_menu.closing && !categoryKey) {
        if (key == VK_ESCAPE) CloseMenu();
        else if (key == VK_DOWN) MenuStep(1);
        else if (key == VK_UP) MenuStep(-1);
        else if (key == VK_RETURN && g_menu.hover >= 0) ExecuteMenu(g_menu.items[g_menu.hover].id);
        return;
    }
    if (g_chipRename >= 0) {
        if (key == VK_ESCAPE) CancelChipRename();
        else if (key == VK_RETURN) CommitChipRename();
        return;
    }
    if (Picking()) {
        if (key == VK_RETURN) { EndPicker(true); return; }
        if (key == VK_ESCAPE && g_query.empty()) { EndPicker(false); return; }
    }
    if (key == VK_ESCAPE && !g_selection.empty() && !g_renaming && !Picking()) { g_selection.clear(); Render(); return; }
    if (g_renaming) {
        if (key == VK_ESCAPE) CancelRename();
        else if (key == VK_RETURN) CommitRename();
        return;
    }
    if (key != VK_ESCAPE && !FolderSettled()) return;
    const bool ctrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    ClampQueryCaret();
    if (ctrlDown && g_openFolderIndex < 0 && g_settings.siriOriginal) {
        // Original Siri category shortcuts (Ctrl+..., set in the mod's settings)
        for (int tab = 0; tab < 4; tab++) {
            const Shortcut& sc = g_shortcuts[tab];
            if (!sc.valid || sc.vk != (UINT)key || sc.shift != shiftDown) continue;
            if (!TabEnabled(tab)) return;
            if (g_menu.open) CloseMenu();
            if (g_settings.siriOriginal) g_siriChosen = true;
            if (g_siriTab != tab) SetSiriTab(tab);
            if (g_settings.siri) ExpandSiri(); else g_searchActive = true;
            Render();
            return;
        }
    }
    if (ctrlDown && (key == 'A' || key == 'a')) {
        g_queryAnchor = 0; g_queryCaret = g_query.size();
        Render(); return;
    }
    if (ctrlDown && (key == 'C' || key == 'c')) { CopyQueryToClipboard(); return; }
    if (ctrlDown && (key == 'X' || key == 'x')) {
        if (QueryHasSelection()) { CopyQueryToClipboard(); DeleteQuerySelection(); QueryChanged(); }
        return;
    }
    // Ctrl+V is pasted from WM_CHAR (0x16); handling it here too would paste twice.
    // Plain Left/Right keep navigating the grid; they edit the text only with Shift or an active selection.
    bool editKey = key == VK_HOME || key == VK_END || ((key == VK_LEFT || key == VK_RIGHT) && (shiftDown || QueryHasSelection()));
    if (key == VK_DELETE && !g_query.empty()) {
        if (QueryHasSelection()) DeleteQuerySelection();
        else if (g_queryCaret < g_query.size()) {
            bool pair = IS_HIGH_SURROGATE(g_query[g_queryCaret]) && g_queryCaret + 1 < g_query.size() && IS_LOW_SURROGATE(g_query[g_queryCaret + 1]);
            g_query.erase(g_queryCaret, pair ? 2 : 1);
            g_queryAnchor = g_queryCaret;
        } else return;
        QueryChanged();
        return;
    }
    if (editKey && !g_query.empty()) {
        size_t old = g_queryCaret;
        if (!shiftDown && QueryHasSelection() && (key == VK_LEFT || key == VK_RIGHT)) {
            g_queryCaret = key == VK_LEFT ? QuerySelStart() : QuerySelEnd();
        } else if (key == VK_HOME) g_queryCaret = 0;
        else if (key == VK_END) g_queryCaret = g_query.size();
        else if (key == VK_LEFT && g_queryCaret > 0) {
            --g_queryCaret; if (g_queryCaret > 0 && IS_LOW_SURROGATE(g_query[g_queryCaret]) && IS_HIGH_SURROGATE(g_query[g_queryCaret - 1])) --g_queryCaret;
        } else if (key == VK_RIGHT && g_queryCaret < g_query.size()) {
            if (IS_HIGH_SURROGATE(g_query[g_queryCaret]) && g_queryCaret + 1 < g_query.size() && IS_LOW_SURROGATE(g_query[g_queryCaret + 1])) g_queryCaret += 2; else ++g_queryCaret;
        }
        if (!shiftDown) g_queryAnchor = g_queryCaret;
        if (old != g_queryCaret || QueryHasSelection()) Render();
        return;
    }
    if (key >= '1' && key <= '9' && ctrlDown && g_openFolderIndex < 0) {
        int n = (int)(key - '1');
        if (n < (int)g_layoutItems.size()) LaunchApp(g_layoutItems[n]);
        return;
    }
    if (g_settings.siri && g_powerPillWanted && g_powerPillAnim > 0.9f) {
        if (key == VK_ESCAPE && g_powerPillByButton && g_powerPillFocus < 0) { g_powerPillByButton = false; UpdatePowerPill(); return; }
        if (key == VK_TAB) { g_powerPillFocus = g_powerPillFocus < 0 ? g_powerPillPick : -1; Render(); return; }
        if (g_powerPillFocus >= 0) {
            if (key == VK_LEFT || key == VK_RIGHT) {
                g_powerPillFocus = std::clamp(g_powerPillFocus + (key == VK_RIGHT ? 1 : -1), 0, 4);
                Render(); return;
            }
            if (key == VK_RETURN) { RunPowerChoice(kPowerChoices[g_powerPillFocus].id); return; }
            g_powerPillFocus = -1;
            if (key == VK_ESCAPE) { Render(); return; }
        }
    }
    if (key == VK_TAB && g_openFolderIndex < 0 && g_settings.siriOriginal) {
        // Original Siri: Tab walks the app tabs, or the results in the other categories
        bool back = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        if (!g_siriExpanded) {
            if (g_siriChosen) ExpandSiri();
            else { OpenCategoryMenu(); g_menu.keyboard = true; g_menu.hover = 0; Render(); }
            return;
        }
        if (g_siriTab == 0 && !Picking()) {
            auto chips = Chips();
            int at = (int)(std::find(chips.begin(), chips.end(), g_appFilter) - chips.begin());
            if (at >= (int)chips.size()) at = 0;
            int n = (int)chips.size();
            OnChipClick(chips[((at + (back ? -1 : 1)) % n + n) % n]);
        } else if (!g_layoutItems.empty()) {
            int count = (int)g_layoutItems.size();
            g_selected = g_selected < 0 ? (back ? count - 1 : 0) : ((g_selected + (back ? -1 : 1)) % count + count) % count;
            EnsureVisible(g_selected); Render(); StartFrames();
        }
        return;
    }
    if (key == VK_TAB && g_openFolderIndex < 0) {
        bool back = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        if (g_settings.siri) ExpandSiri();
        else if (!g_searchActive) { ActivateSearch(); return; } // first Tab just shows the categories
        int slot = TabSlot(g_siriTab);
        SetSiriTab(g_tabIds[(slot + (back ? g_tabCount - 1 : 1)) % g_tabCount]);
        return;
    }
    if (g_settings.siriOriginal && !g_siriChosen && !g_siriExpanded && key == VK_DOWN) {
        OpenCategoryMenu(); g_menu.keyboard = true; g_menu.hover = 0; Render();
        return;
    }
    if (g_settings.siri && !g_siriExpanded && key == VK_DOWN) { ExpandSiri(); return; }
    switch (key) {
        case VK_ESCAPE:
            if (!g_query.empty()) { ClearQuery(); QueryChanged(); }
            else if (g_openFolderIndex >= 0) CloseFolder();
            else if (g_settings.siriOriginal && (g_siriChosen || g_siriExpanded)) BackToCategoryMenu();
            else if (!g_settings.siri && g_searchActive) DeactivateSearch();
            else if (g_settings.siri && g_siriTab != 0) SetSiriTab(0);
            else if (g_settings.siri && g_siriExpanded) CollapseSiri();
            else Close();
            break;
        case VK_RETURN: if (g_selected >= 0 && g_selected < (int)g_layoutItems.size()) LaunchApp(g_layoutItems[g_selected]); break;
        case VK_LEFT: MoveSelection(-1); break;
        case VK_RIGHT: MoveSelection(1); break;
        case VK_UP: MoveSelection(-g_layout.columns); break;
        case VK_DOWN: MoveSelection(g_layout.columns); break;
        case VK_PRIOR: ScrollRows(-g_layout.visibleRows); break;
        case VK_NEXT: ScrollRows(g_layout.visibleRows); break;
    }
}

void MoveAppOutOfFolder(int appIdx) {
    int fi = g_openFolderIndex;
    if (fi < 0 || fi >= (int)g_folders.size()) return;
    auto& apps = g_folders[fi].apps;
    if (appIdx < 0 || appIdx >= (int)apps.size()) return;
    g_apps.push_back(apps[appIdx]);
    apps.erase(apps.begin() + appIdx);
    std::stable_sort(g_apps.begin(), g_apps.end(), [](const auto& a, const auto& b) { return NameLess(a->name, b->name); });
    if (apps.empty()) {
        FinishFolderClose();
        g_folders.erase(g_folders.begin() + fi);
    } else {
        g_hover = g_selected = -1;
        BuildMainItems(g_mainItems);
    }
    SaveFolders();
    RebuildLayout(); ComputeLayout(); Render();
}

// Pin to taskbar

bool IsOverTaskbar(float cx, float cy) {
    POINT pt{g_layout.area.left + (LONG)cx, g_layout.area.top + (LONG)cy};
    for (PCWSTR cls : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd", kDockClass}) {
        HWND h = nullptr;
        while ((h = FindWindowExW(nullptr, h, cls, nullptr))) {
            RECT rc;
            if (IsWindowVisible(h) && GetWindowRect(h, &rc) && PtInRect(&rc, pt)) return true;
        }
    }
    return false;
}

bool InvokeShellVerb(const std::wstring& target, HWND owner, std::initializer_list<PCWSTR> verbs, PCWSTR fallbackVerb) {
    ComPtr<IShellItem> item; ComPtr<IContextMenu> cm;
    if (FAILED(SHCreateItemFromParsingName(target.c_str(), nullptr, IID_PPV_ARGS(&item))) ||
        FAILED(item->BindToHandler(nullptr, kBhidSFUIObject, IID_PPV_ARGS(&cm)))) return false;
    HMENU menu = CreatePopupMenu(); if (!menu) return false;
    constexpr UINT kFirst = 1; bool ok = false;
    auto invoke = [&](LPCSTR verbA, LPCWSTR verbW) {
        CMINVOKECOMMANDINFOEX ici{sizeof(ici)};
        ici.fMask = CMIC_MASK_UNICODE; ici.hwnd = owner; ici.nShow = SW_SHOWNORMAL;
        ici.lpVerb = verbA; ici.lpVerbW = verbW;
        return SUCCEEDED(cm->InvokeCommand((CMINVOKECOMMANDINFO*)&ici));
    };
    if (SUCCEEDED(cm->QueryContextMenu(menu, 0, kFirst, 0x7FFF, CMF_NORMAL))) {
        for (int i = 0, n = GetMenuItemCount(menu); i < n && !ok; i++) {
            UINT id = GetMenuItemID(menu, i);
            if (id == (UINT)-1 || id < kFirst) continue;
            WCHAR verb[128] = {};
            if (FAILED(cm->GetCommandString(id - kFirst, GCS_VERBW, nullptr, (LPSTR)verb, ARRAYSIZE(verb)))) continue;
            std::wstring v = ToLower(verb);
            for (PCWSTR wanted : verbs)
                if (!ok && v == wanted) ok = invoke(MAKEINTRESOURCEA(id - kFirst), MAKEINTRESOURCEW(id - kFirst));
        }
    }
    if (!ok) {
        std::wstring wide = fallbackVerb;
        std::string narrow(wide.begin(), wide.end());
        ok = invoke(narrow.c_str(), fallbackVerb);
    }
    DestroyMenu(menu);
    return ok;
}

bool PinToTaskbar(const std::wstring& target) {
    return InvokeShellVerb(target, g_wnd, {L"taskbarpin", L"pintotaskbar"}, L"taskbarpin");
}

void UninstallApp(const std::wstring& target) {
    if (InvokeShellVerb(target, nullptr, {L"uninstall"}, L"uninstall")) return;
    ShellExecuteW(nullptr, L"open", L"ms-settings:appsfeatures", nullptr, nullptr, SW_SHOWNORMAL);
}

std::vector<std::shared_ptr<App>> TakeApps(const std::unordered_set<App*>& set) {
    std::vector<std::shared_ptr<App>> out;
    for (auto& a : g_apps) if (set.count(a.get())) out.push_back(a);
    g_apps.erase(std::remove_if(g_apps.begin(), g_apps.end(), [&](const std::shared_ptr<App>& a) { return set.count(a.get()) != 0; }), g_apps.end());
    return out;
}

void CreateFolderFromSelection() {
    Folder f; f.name = Tr(L"New Folder");
    auto taken = TakeApps(g_selection); g_selection.clear();
    f.sortKey = taken.empty() ? f.name : taken[0]->name;
    f.apps = std::move(taken);
    g_folders.push_back(std::move(f)); SaveFolders();
    g_hover = g_selected = -1; RebuildLayout(); ComputeLayout();
    OpenFolder((int)g_folders.size() - 1); BeginRename();
}

void EndDrag() {
    if (g_dragItem < 0) return;
    int fromIdx = g_dragItem;
    if (fromIdx >= (int)g_layoutItems.size()) { g_dragItem = -1; return; }

    if (g_settings.dragToPin && IsOverTaskbar(g_dragX, g_dragY)) {
        int item = g_layoutItems[fromIdx]; g_dragItem = -1;
        App* app = nullptr;
        if (g_openFolderIndex >= 0) {
            if (g_openFolderIndex < (int)g_folders.size() && item >= 0 && item < (int)g_folders[g_openFolderIndex].apps.size())
                app = g_folders[g_openFolderIndex].apps[item].get();
        } else if (!IsFolderItem(item)) app = ResolveApp(item);
        if (app && app->kind != 0) app = nullptr;
        if (app && g_openFolderIndex < 0 && item < 1000 && g_selection.size() > 1 && g_selection.count(app)) {
            for (auto& a : g_apps) if (g_selection.count(a.get())) PinToTaskbar(a->target);
        } else if (app) PinToTaskbar(app->target);
        Render(); return;
    }

    // Folders are supported in the Apps category, including Floating Apps results (Original Siri uses tabs instead)
    if (g_siriTab != 0 || g_settings.siriOriginal) { g_dragItem = -1; return; }

    if (g_openFolderIndex >= 0) {
        g_dragItem = -1;
        if (FolderSettled() && !InRect(g_layout.panel, g_dragX, g_dragY))
            MoveAppOutOfFolder(g_layoutItems[fromIdx]);
        return;
    }

    int toIdx = ItemAt((int)g_dragX, (int)g_dragY);
    int fromItem = g_layoutItems[fromIdx];
    g_dragItem = -1;
    if (toIdx < 0 || toIdx == fromIdx || fromItem >= 1000) return;
    int toItem = g_layoutItems[toIdx];
    if (IsFolderApp(toItem) || IsTabItem(toItem)) return;
    App* dragged = ResolveApp(fromItem);
    if (!dragged) return;
    std::unordered_set<App*> moving;
    if (g_selection.count(dragged)) moving = g_selection; else moving.insert(dragged);

    if (toItem < 1000) {
        App* targetRaw = ResolveApp(toItem);
        if (!targetRaw) return;
        auto targetIt = std::find_if(g_apps.begin(), g_apps.end(), [&](const std::shared_ptr<App>& a) { return a.get() == targetRaw; });
        if (targetIt == g_apps.end()) return;
        std::shared_ptr<App> target = *targetIt;
        moving.erase(target.get());
        if (moving.empty()) return;
        auto taken = TakeApps(moving);
        // TakeApps() mutates g_apps and invalidates iterators; find the target again.
        targetIt = std::find_if(g_apps.begin(), g_apps.end(), [&](const std::shared_ptr<App>& a) { return a.get() == targetRaw; });
        if (targetIt == g_apps.end()) return;
        g_apps.erase(targetIt);
        Folder f; f.name = f.sortKey = target->name;
        f.apps.push_back(target);
        f.apps.insert(f.apps.end(), taken.begin(), taken.end());
        g_folders.push_back(std::move(f));
    } else {
        auto taken = TakeApps(moving);
        auto& dst = g_folders[toItem - 1000].apps;
        dst.insert(dst.end(), taken.begin(), taken.end());
    }
    g_selection.clear();
    SaveFolders();
    RebuildLayout(); ComputeLayout(); Render();
}

////////////////////////////////////////////////////////////////////////////////
// System Hooks & Window Proc

bool OwnsTaskbar() {
    DWORD pid = 0; HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    return tray && GetWindowThreadProcessId(tray, &pid) && pid == GetCurrentProcessId();
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void*, int, int);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;

HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis, int showMethod, int showFlags) {
    bool enabled = (showMethod == 1 && (showFlags & 0x8)) ? g_winKeyEnabled : g_startButtonEnabled;
    HWND wnd = g_wnd; if (!enabled || !wnd) return XamlLauncher_ShowStartView_Original(pThis, showMethod, showFlags);
    PostMessageW(wnd, WM_APP_TOGGLE, 0, 0); return S_OK;
}

void OnAppsLoaded(AppList* list, int iconPixels) {
    LoadUserAssist();
    if (!list->empty()) UpdateKnownApps(*list);
    if (Picking()) { g_appFilter = g_pickerPrevFilter; g_pickerChip = kNoChip; }   // the picked apps were the old list
    g_menu.open = false; g_selection.clear();
    if (!list->empty()) { ApplyCustomIcons(*list, iconPixels); ReconcileFolders(*list); }
    g_apps = std::move(*list); delete list;
    g_appsLoaded = true; g_appsIconPixels = iconPixels;
    if (g_shown) {
        if (g_openFolderIndex >= 0) BuildMainItems(g_mainItems);
        RebuildLayout(); ComputeLayout(); Render();
    }
}

void UndoFolderAt(int index) {
    if (index < 0 || index >= (int)g_folders.size()) return;
    if (g_openFolderIndex >= 0) FinishFolderClose();
    UndoFolder(index);
    SaveFolders();
    g_hover = g_selected = -1;
    RebuildLayout(); ComputeLayout(); Render();
}

void CloseMenu() { if (g_menu.open && !g_menu.closing) { g_menu.closing = true; g_menu.hover = -1; StartFrames(); } }

void OpenMenu(float x, float y, std::vector<MenuItem> items, bool alignRight, float minWidth);
void OpenCategoryMenu();

// Original Siri, Esc after picking: back to "Search" with the magnifier, and the categories are offered again
void BackToCategoryMenu() {
    g_siriChosen = false;
    g_flyAnim = 1.0f;
    g_appFilter = -1;
    if (Picking()) g_selection.clear();
    g_pickerChip = kNoChip; g_chipRename = -1; g_chipScroll = 0;
    if (g_siriTab != 0) {
        g_siriTab = 0;
        g_tabSlide = 0;
        ScheduleFileSearch();
        RefreshTabItems(true);
    }
    CollapseSiri();
    OpenCategoryMenu();
    g_menu.keyboard = true;
    g_menu.hover = 0;   // ready for the arrow keys and Enter
    Render();
}

// Original Siri: hovering the bar drops down the categories, each with its icon and shortcut
void OpenCategoryMenu() {
    const Layout& L = g_layout;
    float s = L.scale;
    std::vector<MenuItem> items;
    for (int i = 0; i < g_tabCount && i < 4; i++) {
        int id = g_tabIds[i];
        int menuId = id == 0 ? kMenuTabApps : id == 1 ? kMenuTabFiles : id == 3 ? kMenuTabActions : kMenuTabClipboard;
        items.push_back({TabLabel(id), menuId, false, false, TabGlyph(id), g_shortcuts[id].valid ? g_shortcutLabels[id].c_str() : nullptr});
    }
    g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
    OpenMenu(L.search.left, L.search.bottom + 10 * s, std::move(items), false, L.search.right - L.search.left);
    g_menu.category = true;
    g_menu.keyboard = false;
    // macOS-like: taller rows, larger icons and text
    g_menu.itemH = std::round(48 * s);
    g_menu.rect.bottom = g_menu.rect.top + 16 * s + g_menu.items.size() * g_menu.itemH;
}

void OpenMenu(float x, float y, std::vector<MenuItem> items, bool alignRight = false, float minWidth = 0) {
    if (items.empty()) return;
    const Layout& L = g_layout; float s = L.scale;
    g_menu.items = std::move(items); g_menu.itemH = std::round(32 * s); g_menu.sepH = std::round(9 * s);
    float w = 0, h = 12 * s;
    for (auto& it : g_menu.items) {
        h += it.separator ? g_menu.sepH : g_menu.itemH;
        if (it.separator || !g_menuFormat) continue;
        ComPtr<IDWriteTextLayout> tl;
        g_dwrite->CreateTextLayout(it.text.c_str(), (UINT32)it.text.size(), g_menuFormat.Get(), 1000, 40, &tl);
        if (tl) { DWRITE_TEXT_METRICS m{}; tl->GetMetrics(&m); w = std::max(w, m.widthIncludingTrailingWhitespace); }
    }
    bool glyphs = false, hints = false;
    for (auto& it : g_menu.items) { glyphs |= it.glyph != nullptr; hints |= it.hint != nullptr; }
    w = std::round(std::max({190 * s, w + 44 * s + (glyphs ? 28 * s : 0.0f) + (hints ? 56 * s : 0.0f), minWidth}));
    float m = 8 * s, left = alignRight ? x - w : x, top = y;
    if (!alignRight && left + w > L.width - m) left = x - w;
    if (alignRight && left < m) left = x;
    if (top + h > L.height - m) top = y - h;
    left = std::clamp(left, m, std::max(m, L.width - w - m)); top = std::clamp(top, m, std::max(m, L.height - h - m));
    g_menu.rect = D2D1::RectF(left, top, left + w, top + h); g_menu.origin = D2D1::Point2F(x, y);
    g_menu.open = true; g_menu.closing = false; g_menu.anim = 0; g_menu.hover = -1; g_menu.category = false;
    StartFrames(); Render();
}

void MenuStep(int d) {
    int n = (int)g_menu.items.size(); if (!n) return;
    int i = g_menu.hover < 0 ? (d > 0 ? -1 : 0) : g_menu.hover;
    for (int k = 0; k < n; k++) { i = ((i + d) % n + n) % n; if (!g_menu.items[i].separator) { g_menu.hover = i; break; } }
    Render();
}

App* MenuApp() {
    if (g_menu.appIdx >= 0) return AppOfMain(g_menu.appIdx);
    if (g_openFolderIndex >= 0 && g_openFolderIndex < (int)g_folders.size() &&
        g_menu.folderApp >= 0 && g_menu.folderApp < (int)g_layoutItems.size()) {
        int i = g_layoutItems[g_menu.folderApp]; auto& v = g_folders[g_openFolderIndex].apps;
        if (i >= 0 && i < (int)v.size()) return v[i].get();
    }
    return nullptr;
}

int g_flyIndex = -1;   // the category row that was clicked (for the flight into the bar)

// ---------- Original Siri user tabs ----------
void RefreshAppsView() {
    g_hover = g_selected = -1;
    g_scroll = g_scrollTarget = 0;
    RebuildLayout(); ComputeLayout(); Render();
}

void BeginChipRename(int tab) {
    if (tab < 0 || tab >= (int)g_customTabs.size()) return;
    g_chipRename = tab; g_renameSelectAll = true;
    g_renameText = g_customTabs[tab].name;
    ComputeLayout(); EnsureChipVisible(kChipCustom + tab); Render();
}

void CommitChipRename() {
    if (g_chipRename < 0) return;
    int tab = g_chipRename;
    g_chipRename = -1;
    std::wstring name = Clean(g_renameText);
    name.erase(0, name.find_first_not_of(L' '));
    name.erase(name.find_last_not_of(L' ') + 1);
    if (tab < (int)g_customTabs.size() && !name.empty()) { g_customTabs[tab].name = name; SaveCustomTabs(); }
    ComputeLayout(); Render();
}

void CancelChipRename() {
    if (g_chipRename < 0) return;
    g_chipRename = -1;
    ComputeLayout(); Render();
}

void NewCustomTab(App* app) {
    if ((int)g_customTabs.size() >= kMaxCustomTabs) return;
    std::wstring base = Tr(L"New tab"), name = base;
    auto taken = [&](const std::wstring& n) {
        return std::any_of(g_customTabs.begin(), g_customTabs.end(), [&](const CustomTab& t) { return t.name == n; });
    };
    for (int n = 2; taken(name); n++) name = base + L" " + std::to_wstring(n);
    CustomTab tab;
    tab.name = name;
    if (app) tab.targets.push_back(ToLower(app->target));
    g_customTabs.push_back(std::move(tab));
    SaveCustomTabs();
    g_appFilter = kChipCustom + (int)g_customTabs.size() - 1;
    RefreshAppsView();
    BeginChipRename((int)g_customTabs.size() - 1);   // type the name right away (Enter keeps it)
}

void DeleteCustomTab(int tab) {
    if (tab < 0 || tab >= (int)g_customTabs.size()) return;
    g_customTabs.erase(g_customTabs.begin() + tab);
    SaveCustomTabs();
    if (g_appFilter == kChipCustom + tab) g_appFilter = kChipAll;
    else if (g_appFilter > kChipCustom + tab) g_appFilter--;
    ClampChipScroll();
    RefreshAppsView();
}

void DeleteChip(int chip) {
    if (chip >= kChipCustom) { DeleteCustomTab(chip - kChipCustom); return; }
    if (chip < 0 || chip >= kFilterCount) return;
    g_builtinTabs[chip].hidden = true;
    SaveBuiltinTabs();
    if (g_appFilter == chip) g_appFilter = kChipAll;
    ClampChipScroll();
    RefreshAppsView();
}

void RestoreBuiltinTabs() {
    for (auto& b : g_builtinTabs) b.hidden = false;
    SaveBuiltinTabs();
    ComputeLayout(); Render();
}

void AddToChip(int chip, const App& app) {
    std::wstring t = ToLower(app.target);
    if (chip >= kChipCustom) {
        int tab = chip - kChipCustom;
        if (tab >= (int)g_customTabs.size() || TabHas(tab, app)) return;
        g_customTabs[tab].targets.push_back(t);
        SaveCustomTabs();
    } else if (chip >= 0 && chip < kFilterCount) {
        auto& b = g_builtinTabs[chip];
        b.removed.erase(std::remove(b.removed.begin(), b.removed.end(), t), b.removed.end());
        if (AppCategory(app) != chip && !ListHas(b.added, t)) b.added.push_back(t);
        SaveBuiltinTabs();
    }
}

void RemoveFromChip(int chip, const App& app) {
    std::wstring t = ToLower(app.target);
    if (chip >= kChipCustom) {
        int tab = chip - kChipCustom;
        if (tab >= (int)g_customTabs.size()) return;
        auto& v = g_customTabs[tab].targets;
        v.erase(std::remove(v.begin(), v.end(), t), v.end());
        SaveCustomTabs();
    } else if (chip >= 0 && chip < kFilterCount) {
        auto& b = g_builtinTabs[chip];
        b.added.erase(std::remove(b.added.begin(), b.added.end(), t), b.added.end());
        if (AppCategory(app) == chip && !ListHas(b.removed, t)) b.removed.push_back(t);
        SaveBuiltinTabs();
    } else return;
    RefreshAppsView();
}

std::vector<App*> AllApps() {
    std::vector<App*> all;
    for (auto& a : g_apps) all.push_back(a.get());
    for (auto& f : g_folders) for (auto& a : f.apps) all.push_back(a.get());
    return all;
}

// Picking apps for a tab: every app is shown, a long press selects (selected apps jiggle)
void StartPicker(int chip) {
    if (!ChipEditable(chip)) return;
    CommitChipRename();
    g_pickerPrevFilter = g_appFilter;
    g_pickerChip = chip;
    g_selection.clear();
    for (App* a : AllApps()) if (ChipHas(chip, *a)) g_selection.insert(a);
    g_chipScroll = 0;
    ClearQuery();
    RefreshAppsView();
    StartFrames();
}

void EndPicker(bool save) {
    if (!Picking()) return;
    int chip = g_pickerChip, tab = chip - kChipCustom;
    g_pickerChip = kNoChip;
    if (save && chip >= 0 && chip < kFilterCount) {
        // Built-in tab: remember what differs from the automatic category (apps not installed now are kept)
        auto& b = g_builtinTabs[chip];
        std::unordered_set<std::wstring> present;
        for (App* a : AllApps()) present.insert(ToLower(a->target));
        std::vector<std::wstring> added, removed;
        for (const auto& t : b.added) if (!present.count(t)) added.push_back(t);
        for (const auto& t : b.removed) if (!present.count(t)) removed.push_back(t);
        for (App* a : AllApps()) {
            bool picked = g_selection.count(a) != 0, automatic = AppCategory(*a) == chip;
            std::wstring t = ToLower(a->target);
            if (picked && !automatic && !ListHas(added, t)) added.push_back(t);
            if (!picked && automatic && !ListHas(removed, t)) removed.push_back(t);
        }
        b.added = std::move(added);
        b.removed = std::move(removed);
        SaveBuiltinTabs();
        g_appFilter = chip;
    } else if (save && chip >= kChipCustom && tab < (int)g_customTabs.size()) {
        // Apps that aren't installed right now stay in the tab
        std::unordered_set<std::wstring> present;
        for (auto& a : g_apps) present.insert(ToLower(a->target));
        for (auto& f : g_folders) for (auto& a : f.apps) present.insert(ToLower(a->target));
        std::vector<std::wstring> targets;
        for (const auto& t : g_customTabs[tab].targets) if (!present.count(t)) targets.push_back(t);
        for (App* a : g_selection) targets.push_back(ToLower(a->target));
        std::sort(targets.begin(), targets.end());
        targets.erase(std::unique(targets.begin(), targets.end()), targets.end());
        g_customTabs[tab].targets = std::move(targets);
        SaveCustomTabs();
        g_appFilter = kChipCustom + tab;
    } else {
        g_appFilter = g_pickerPrevFilter;
    }
    g_selection.clear();
    ClearQuery();
    RefreshAppsView();
    EnsureChipVisible(g_appFilter);
    Render();
}

void OnChipClick(int chip) {
    if (g_chipRename >= 0) {
        if (chip == kChipCustom + g_chipRename) return;
        CommitChipRename();
    }
    if (chip == kChipCancel || chip == kChipDone) { EndPicker(chip == kChipDone); return; }
    g_appFilter = chip;
    EnsureChipVisible(chip);
    RefreshAppsView();
}

void ExecuteMenu(int id) {
    g_flyIndex = -1;
    if (g_menu.category)
        for (int i = 0; i < (int)g_menu.items.size(); i++)
            if (g_menu.items[i].id == id) g_flyIndex = i;
    int folder = g_menu.folder; App* app = MenuApp();
    int histIdx = g_menu.appIdx;
    CloseMenu();
    if (id >= kMenuAddToTab && id < kMenuAddToTab + kMaxCustomTabs) {
        if (app) AddToChip(kChipCustom + id - kMenuAddToTab, *app);
        Render();
        return;
    }
    switch (id) {
        case kMenuNewTab: NewCustomTab(app); break;
        case kMenuAddApps: StartPicker(g_menuChip); break;
        case kMenuRenameTab: if (g_menuChip >= kChipCustom) BeginChipRename(g_menuChip - kChipCustom); break;
        case kMenuDeleteTab: DeleteChip(g_menuChip); break;
        case kMenuRemoveFromTab: if (app) RemoveFromChip(g_menuChip, *app); break;
        case kMenuRestoreTabs: RestoreBuiltinTabs(); break;
        case kMenuTextCopy: CopyQueryToClipboard(); break;
        case kMenuTextCut:
            if (!QueryHasSelection()) { g_queryAnchor = 0; g_queryCaret = g_query.size(); }
            CopyQueryToClipboard(); DeleteQuerySelection(); QueryChanged();
            break;
        case kMenuTextPaste:
            PasteText();
            if (g_settings.siri && !g_query.empty()) ExpandSiri();
            break;
        case kMenuTextDeleteAll: ClearQuery(); QueryChanged(); break;
        case kMenuTextSelectAll: g_queryAnchor = 0; g_queryCaret = g_query.size(); Render(); break;
        case kMenuTabApps: case kMenuTabFiles: case kMenuTabClipboard: case kMenuTabActions: {
            int tab = id == kMenuTabApps ? 0 : id == kMenuTabFiles ? 1 : id == kMenuTabActions ? 3 : 2;
            if (g_flyIndex >= 0) {
                // The row's icon and label fly into the bar
                float s = g_layout.scale, top = MenuItemTop(g_flyIndex);
                D2D1_RECT_F row = D2D1::RectF(g_menu.rect.left + 8 * s, top, g_menu.rect.right - 8 * s, top + g_menu.itemH);
                g_flyGlyph = D2D1::RectF(row.left + 8 * s, row.top, row.left + 44 * s, row.bottom);
                g_flyText = D2D1::RectF(row.left + 54 * s, row.top, row.right - 90 * s, row.bottom);
                g_flyAnim = 0.0f;
            }
            g_siriChosen = true;
            SetSiriTab(tab);
            ExpandSiri();
            StartFrames();
            break;
        }
        case kMenuRename:
            if (folder >= 0 && folder < (int)g_folders.size()) { if (g_openFolderIndex < 0) OpenFolder(folder); BeginRename(); }
            break;
        case kMenuUndoFolder: UndoFolderAt(folder); break;
        case kMenuSelect: if (app && !g_selection.erase(app)) g_selection.insert(app); Render(); break;
        case kMenuSelectAll:
            for (int it : g_layoutItems) if (it >= 0 && it < 1000 && it < (int)g_apps.size()) g_selection.insert(g_apps[it].get());
            Render(); break;
        case kMenuClearSelection: g_selection.clear(); Render(); break;
        case kMenuNewFolder:
            if (app && !g_selection.count(app)) { g_selection.clear(); g_selection.insert(app); }
            CreateFolderFromSelection(); break;
        case kMenuPin:
            if (app && g_selection.count(app) && g_selection.size() > 1) { for (auto& a : g_apps) if (g_selection.count(a.get())) PinToTaskbar(a->target); }
            else if (app) PinToTaskbar(app->target);
            break;
        case kMenuHistRemove:
            if (histIdx >= 0 && histIdx < (int)g_layoutItems.size())
                if (App* h = ResolveApp(g_layoutItems[histIdx]); h && h->kind == 3) {
                    std::wstring q = h->target;
                    g_searchHistory.erase(std::remove(g_searchHistory.begin(), g_searchHistory.end(), q), g_searchHistory.end());
                    SaveSearchHistory(); RefreshTabItems(false);
                }
            break;
        case kMenuHistClear:
            g_searchHistory.clear(); SaveSearchHistory(); RefreshTabItems(true);
            break;
        case kMenuChangeIcon: {
            std::wstring path;
            if (app && PickImage(g_wnd, path)) {
                if (LoadImageFile(path, CurrentIconPixels(), *app)) { g_customIcons[app->target] = path; SaveCustomIcons(); }
                else MessageBoxW(g_wnd, Tr(L"Could not load this image."), L"Launchpad", MB_ICONWARNING);
            }
            if (g_shown) { ForceForeground(g_wnd); Render(); }
            break;
        }
        case kMenuResetIcon:
            if (app) { g_customIcons.erase(app->target); SaveCustomIcons(); ResetAppIcon(*app); Render(); }
            break;
        case kMenuUninstall:
            if (app) {
                std::wstring target = app->target;
                HideNow();
                g_appsTime = 0;
                UninstallApp(target);
            }
            break;
        case kMenuLock: case kMenuSleep: case kMenuRestart: case kMenuShutdown: case kMenuSignOut:
            HideNow();
            DoPowerAction(id);
            break;
    }
}

void RunPowerChoice(int id) {
    HideNow();
    DoPowerAction(id);
}

std::vector<MenuItem> PowerMenuItems() {
    return {
        {Tr(L"Lock"), kMenuLock},
        {Tr(L"Sleep"), kMenuSleep},
        {L"", 0, true},
        {Tr(L"Restart"), kMenuRestart},
        {Tr(L"Shut down"), kMenuShutdown, false, true},
        {L"", 0, true},
        {Tr(L"Sign out"), kMenuSignOut},
    };
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_TOGGLE:
            if ((!g_shown || g_closing) && Now() - g_lastClosed < 0.5) return 0;
            Toggle(); return 0;
        case WM_APP_APPS: OnAppsLoaded((AppList*)lParam, (int)wParam); return 0;
        case WM_APP_FILES: OnFilesFound((FileResult*)lParam); return 0;
        case WM_APP_RELOAD:
            LoadSettings();
            if (g_shown) HideNow();
            if (!g_settings.clipEnabled) g_clipHistory.clear();   // off: the history is cleared
            else if ((int)g_clipHistory.size() > g_settings.clipMax) g_clipHistory.resize(g_settings.clipMax);
            g_fileStatus.clear(); // Everything IPC needs no reload (no DLL)
            return 0;
        case WM_APP_QUIT: DestroyWindow(hwnd); return 0;
        case WM_CLIPBOARDUPDATE: OnClipboardUpdate(); return 0;
        case WM_TIMER:
            if (wParam == kTimerFrame) OnFrame();
            else if (wParam == kTimerFileSearch) {
                KillTimer(hwnd, kTimerFileSearch);
                if (g_shown && g_siriTab == 1) { StartFileSearch(g_query); Render(); }
            }
            else if (wParam == kTimerLongPress) {
                KillTimer(hwnd, kTimerLongPress);
                // A long press selects the app; it must not act like a normal
                // left click or synthesize a right-click/context-menu action.
                // (Original Siri selects only while picking apps for a tab)
                if (g_shown && g_settings.siri && (!g_settings.siriOriginal || Picking()) && g_siriTab == 0 && g_pressed >= 0 &&
                    !g_isDragging && g_openFolderIndex < 0 && g_pressed < (int)g_layoutItems.size()) {
                    int item = g_layoutItems[g_pressed];
                    if (SelectableApp(g_pressed)) {
                        if (App* app = ResolveApp(item)) {
                            if (g_selection.count(app)) g_selection.erase(app);
                            else g_selection.insert(app);
                            g_longPressTriggered = true;
                            g_pressedEmpty = false;
                            Render();
                        }
                    }
                }
            }
            else if (wParam == kTimerFirstLoad && OwnsTaskbar()) { KillTimer(hwnd, kTimerFirstLoad); if (!g_appsLoaded) StartLoadingApps(); }
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_shown && !g_closing && !g_dialogOpen) { g_lastClosed = Now(); Close(); }
            return 0;
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT p; GetCursorPos(&p); ScreenToClient(hwnd, &p);
                bool hand = TabAt((float)p.x, (float)p.y) >= 0 || FilterAt((float)p.x, (float)p.y) != kNoChip ||
                            HitBack((float)p.x, (float)p.y) || HitChosenText((float)p.x, (float)p.y);
                SetCursor(LoadCursorW(nullptr, hand ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_MOUSEMOVE: {
            if (g_closing) return 0;
            if (!g_trackingMouse) { TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0}; g_trackingMouse = TrackMouseEvent(&tme); }
            if (g_chipPress) {
                float dx = (float)GET_X_LPARAM(lParam) - g_chipPressX;
                if (!g_chipDragged && std::abs(dx) > 4 * g_layout.scale && ChipsOverflow()) g_chipDragged = true;
                if (g_chipDragged) { g_chipScroll = g_chipPressScroll - dx; ClampChipScroll(); Render(); }
                return 0;
            }
            if (g_menu.open && !g_menu.closing) {
                float mx = (float)GET_X_LPARAM(lParam), my = (float)GET_Y_LPARAM(lParam);
                if (g_menu.category) {
                    D2D1_RECT_F zone = D2D1::RectF(std::min(g_layout.search.left, g_menu.rect.left), g_layout.search.top,
                                                   std::max(g_layout.search.right, g_menu.rect.right), g_menu.rect.bottom);
                    bool inside = InRect(zone, mx, my);
                    if (inside) g_menu.keyboard = false;
                    else if (!g_menu.keyboard) { CloseMenu(); return 0; }
                    else return 0;   // opened by Esc: waits for the mouse or the keyboard
                }
                int h = MenuHit(mx, my);
                if (h != g_menu.hover) { g_menu.hover = h; Render(); }
                return 0;
            }
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (g_queryDragging && GetCapture() == hwnd) {
                const size_t nextCaret = QueryIndexAtPoint(g_layout, (float)x, (float)y);
                if (nextCaret != g_queryCaret) {
                    g_queryCaret = nextCaret;

                    Render();
                }
                return 0;
            }
            int dot = (FolderSettled() && !g_swiping && g_pressed < 0) ? DotAt((float)x, (float)y) : -1;
            if ((dot >= 0) != g_dotsHover || dot != g_dotHover) { g_dotsHover = dot >= 0; g_dotHover = dot; StartFrames(); }
            if (g_pressed >= 0) {
                if (!g_isDragging && !Picking() && (std::abs(x - (int)g_dragX) > 5 || std::abs(y - (int)g_dragY) > 5)) {
                    KillTimer(hwnd, kTimerLongPress);
                    // If the long-press already selected this app, continue into drag mode.
                    g_longPressTriggered = false;
                    g_isDragging = true; g_dragItem = g_pressed;
                }
                if (g_isDragging) {
                    g_dragX = (float)x; g_dragY = (float)y; Render();
                    float edge = 24 * g_layout.scale;
                    if (g_layout.horizontal && Now() - g_lastFlip > 0.7 && y >= g_layout.viewTop && y < g_layout.viewBottom &&
                        (x < edge || x > g_layout.width - edge)) { g_lastFlip = Now(); GoToPage(CurrentPage() + (x < edge ? -1 : 1)); }
                }
            } else if (g_pressedEmpty && g_layout.horizontal && GetCapture() == hwnd) {
                float dx = x - g_dragX;
                if (!g_swiping && std::abs(dx) > 6 * g_layout.scale) g_swiping = true;
                if (g_swiping) {
                    float v = g_swipeStartScroll - dx, m = MaxScroll();
                    if (v < 0) v *= 0.35f; else if (v > m) v = m + (v - m) * 0.35f;
                    g_scroll = g_scrollTarget = v; Render();
                }
            } else if (FolderSettled()) {
                if (g_settings.siriOriginal && !g_menu.open && !g_siriExpanded && !g_siriClosingPill && g_pillIntro >= 1.0f &&
                    InRect(g_layout.search, (float)x, (float)y)) {
                    OpenCategoryMenu();
                    return 0;
                }
                bool ph = g_openFolderIndex < 0 && PowerButtonOn() && InRect(g_layout.btnPower, (float)x, (float)y);
                if (ph != g_powerHover) { g_powerHover = ph; Render(); }
                bool ch = HitClear((float)x, (float)y);
                if (ch != g_clearHover) { g_clearHover = ch; Render(); }
                bool bh = HitBack((float)x, (float)y);
                if (bh != g_backHover) { g_backHover = bh; Render(); }
                bool hc = HitHistClear((float)x, (float)y);
                if (hc != g_histClearHover) { g_histClearHover = hc; Render(); }
                int pc = PowerChoiceAt((float)x, (float)y);
                if (pc != g_powerPillHover) { g_powerPillHover = pc; Render(); }
                int hover = ItemAt(x, y);
                if (hover != g_hover) { g_hover = hover; Render(); }
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_trackingMouse = false; g_dotsHover = false; g_dotHover = -1; StartFrames();
            if (g_powerHover) { g_powerHover = false; Render(); }
            if (g_clearHover) { g_clearHover = false; Render(); }
            if (g_backHover) { g_backHover = false; Render(); }
            if (g_histClearHover) { g_histClearHover = false; Render(); }
            if (g_powerPillHover >= 0) { g_powerPillHover = -1; Render(); }
            if (g_menu.open && g_menu.hover >= 0) { g_menu.hover = -1; Render(); }
            if (g_hover >= 0) { g_hover = -1; Render(); } return 0;
        case WM_LBUTTONDOWN: {
            if (g_closing) return 0;
            g_ignoreUp = false;
            g_chipPress = false;   // a press whose release was lost (outside the window)
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (g_menu.open && !g_menu.closing) {
                if (!InRect(g_menu.rect, (float)x, (float)y)) {
                    bool onBar = g_menu.category && InRect(g_layout.search, (float)x, (float)y);
                    if (onBar && g_settings.siriOriginal && !g_siriChosen) { g_ignoreUp = true; return 0; }   // the categories stay: pick one
                    CloseMenu();
                    if (onBar) ExpandSiri();   // clicking the bar opens the current category
                }
                return 0;
            }
            if (g_settings.siri && g_settings.siriPower && PowerButtonOn() && InRect(g_layout.btnPower, (float)x, (float)y)) {
                // Siri: the button calls the same power options pill as typing "shut down"
                g_powerPillByButton = !g_powerPillByButton;
                if (g_powerPillByButton) g_powerPillPick = 3;
                UpdatePowerPill();
                return 0;
            }
            if (g_settings.siri && PowerButtonOn() && InRect(g_layout.btnPower, (float)x, (float)y)) {
                // Siri without the power pill: the menu opens outside the results box, on its right
                g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                float s = g_layout.scale, boxRight = g_layout.width / 2.0f + g_settings.siriPanelW * s / 2;
                OpenMenu(std::max(boxRight, g_layout.btnPower.right) + 12 * s, g_layout.btnPower.top, PowerMenuItems());
                return 0;
            }
            if (g_openFolderIndex < 0 && PowerButtonOn() && InRect(g_layout.btnPower, (float)x, (float)y)) {
                g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                bool right = g_settings.powerRight;
                OpenMenu(right ? g_layout.btnPower.right : g_layout.btnPower.left, g_layout.btnPower.bottom + 6 * g_layout.scale, PowerMenuItems(), right);
                return 0;
            }
            int powerChoice = PowerChoiceAt((float)x, (float)y);
            if (powerChoice >= 0) { RunPowerChoice(kPowerChoices[powerChoice].id); return 0; }
            if (HitHistClear((float)x, (float)y)) {
                g_histClearHover = false;
                g_searchHistory.clear(); SaveSearchHistory(); RefreshTabItems(true);
                return 0;
            }
            if (HitClear((float)x, (float)y)) {
                g_clearHover = false;
                ClearQuery();
                QueryChanged();
                return 0;
            }
            if (g_openFolderIndex < 0) {
                int tab = TabAt((float)x, (float)y);
                if (tab >= 0) { SetSiriTab(tab); return 0; }
                int chip = FilterAt((float)x, (float)y);
                if (chip != kNoChip) {
                    // A click picks the chip on release; holding and moving drags the strip sideways
                    g_chipPress = true; g_chipDragged = false; g_chipPressed = chip;
                    ClampChipScroll();
                    g_chipPressX = (float)x; g_chipPressScroll = g_chipScroll;
                    SetCapture(hwnd);
                    return 0;
                }
                CommitChipRename();
                if (HitBack((float)x, (float)y)) {
                    g_backHover = false; g_ignoreUp = true;
                    BackToCategoryMenu();
                    return 0;
                }
                if (HitChosenText((float)x, (float)y)) {
                    g_ignoreUp = true;
                    OpenCategoryMenu();
                    Render();
                    return 0;
                }
                if (g_settings.siriOriginal && !g_siriChosen && InRect(g_layout.search, (float)x, (float)y)) {
                    // Nothing picked yet: clicking the bar offers the categories (typing goes straight to Apps)
                    g_ignoreUp = true;
                    if (!g_menu.open) OpenCategoryMenu();
                    Render();
                    return 0;
                }
                if (InRect(g_layout.search, (float)x, (float)y)) {
                    if (g_settings.siri) ExpandSiri(); else ActivateSearch();
                    g_queryCaret = QueryIndexAtPoint(g_layout, (float)x, (float)y);
                    g_queryAnchor = g_queryCaret;
                    g_queryDragging = true; SetCapture(hwnd); Render();
                    return 0;
                }
            }
            if (g_openFolderIndex < 0 && (wParam & MK_CONTROL) && (!g_settings.siriOriginal || Picking())) {
                if (App* a = SelectableApp(ItemAt(x, y))) { if (!g_selection.erase(a)) g_selection.insert(a); Render(); return 0; }
            }
            if (g_renaming) {
                if (InRect(TitleRect(g_layout), (float)x, (float)y)) return 0;
                CommitRename();
            }
            if (g_openFolderIndex >= 0) {
                if (InRect(g_layout.btnBack, (float)x, (float)y)) { CloseFolder(); return 0; }
                if (!FolderSettled()) return 0;
                if (InRect(TitleRect(g_layout), (float)x, (float)y)) { BeginRename(); return 0; }
            }
            if (FolderSettled()) { int dot = DotAt((float)x, (float)y); if (dot >= 0) { GoToPage(dot); return 0; } }
            SetCapture(hwnd); g_pressed = ItemAt(x, y); g_dragX = (float)x; g_dragY = (float)y;
            g_longPressTriggered = false;
            g_isDragging = false; g_swiping = false; g_swipeStartScroll = g_scrollTarget;
            g_pressedEmpty = g_pressed < 0 && !InRect(g_layout.search, (float)x, (float)y);
            if (g_settings.siri && g_pressed >= 0 && g_openFolderIndex < 0)
                SetTimer(hwnd, kTimerLongPress, kLongPressDelayMs, nullptr);
            Render(); return 0;
        }
        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (g_closing) { if (GetCapture() == hwnd) ReleaseCapture(); return 0; }
            if (g_ignoreUp) { g_ignoreUp = false; return 0; }
            if (g_chipPress) {
                g_chipPress = false;
                if (GetCapture() == hwnd) ReleaseCapture();
                if (!g_chipDragged) OnChipClick(g_chipPressed);
                else Render();
                return 0;
            }
            if (g_longPressTriggered) {
                g_longPressTriggered = false;
                g_pressed = -1; g_dragItem = -1; g_isDragging = false; g_pressedEmpty = false;
                if (GetCapture() == hwnd) ReleaseCapture();
                Render(); return 0;
            }
            if (g_menu.open && !g_menu.closing) {
                int h = MenuHit((float)x, (float)y);
                if (h >= 0) ExecuteMenu(g_menu.items[h].id);
                else if (g_menu.category && InRect(g_layout.search, (float)x, (float)y) && (!g_settings.siriOriginal || g_siriChosen)) { CloseMenu(); ExpandSiri(); }
                return 0;
            }
            if (g_queryDragging) {
                g_queryCaret = QueryIndexAtPoint(g_layout, (float)x, (float)y);
                g_queryDragging = false;
                if (GetCapture() == hwnd) ReleaseCapture();
                Render(); return 0;
            }
            if (GetCapture() == hwnd) ReleaseCapture();
            if (g_swiping) {
                float dx = x - g_dragX, pw = g_layout.pageWidth;
                int base = (int)std::lround(g_swipeStartScroll / pw);
                g_swiping = false;
                GoToPage(std::abs(dx) > pw * 0.12f ? base + (dx < 0 ? 1 : -1) : base);
            } else if (g_pressed >= 0) {
                if (g_isDragging) { g_dragX = (float)x; g_dragY = (float)y; EndDrag(); }
                else if (ItemAt(x, y) == g_pressed && !Picking()) LaunchApp(g_layoutItems[g_pressed]);
            } else if (g_pressedEmpty && ItemAt(x, y) < 0 && !Picking()) {
                if (g_openFolderIndex >= 0) {
                    if (!InRect(g_layout.panel, (float)x, (float)y)) CloseFolder();
                } else if (!InRect(g_layout.search, (float)x, (float)y) && !InRect(g_layout.siriPanel, (float)x, (float)y) &&
                           !InRect(g_layout.siriPower, (float)x, (float)y) && !InRect(g_layout.siriTabs, (float)x, (float)y)) {
                    if (!g_selection.empty()) g_selection.clear();
                    else Close();
                }
            }
            g_pressed = -1; g_dragItem = -1; g_isDragging = false; g_pressedEmpty = false; Render(); return 0;
        }
        case WM_RBUTTONUP: {
            KillTimer(hwnd, kTimerLongPress);
            if (g_closing) return 0;
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (!FolderSettled()) return 0;
            if (g_openFolderIndex < 0 && InRect(g_layout.search, (float)x, (float)y)) {
                // The search bar has only a text menu: with text, cut/copy/paste/delete all/select all; empty, just paste
                if (g_menu.open) CloseMenu();
                CommitChipRename();
                ClampQueryCaret();
                g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                bool canPaste = IsClipboardFormatAvailable(CF_UNICODETEXT) != 0;
                std::vector<MenuItem> text;
                if (!g_query.empty()) {
                    text.push_back({Tr(L"Cut"), kMenuTextCut});
                    text.push_back({Tr(L"Copy"), kMenuTextCopy});
                }
                if (canPaste) text.push_back({Tr(L"Paste"), kMenuTextPaste});
                if (!g_query.empty()) {
                    text.push_back({L"", 0, true});
                    text.push_back({Tr(L"Select all"), kMenuTextSelectAll});
                    text.push_back({Tr(L"Delete all"), kMenuTextDeleteAll, false, true});
                }
                if (!text.empty()) OpenMenu((float)x, (float)y, std::move(text));
                return 0;
            }
            if (g_siriTab != 0) {
                int idx = ItemAt(x, y);
                App* a = idx >= 0 ? ResolveApp(g_layoutItems[idx]) : nullptr;
                if (g_siriTab != 1 || !g_query.empty() || !g_historyEnabled || g_searchHistory.empty()) return 0;
                g_menu.open = false; g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                std::vector<MenuItem> hist;
                if (a && a->kind == 3) { g_menu.appIdx = idx; hist.push_back({Tr(L"Remove from history"), kMenuHistRemove}); }
                hist.push_back({Tr(L"Clear search history"), kMenuHistClear, false, true});
                OpenMenu((float)x, (float)y, std::move(hist));
                return 0;
            }

            if (g_settings.siriOriginal && g_openFolderIndex < 0) {
                // Original Siri: user tabs instead of folders (only inside the apps panel)
                if (Picking() || !InRect(g_layout.siriPanel, (float)x, (float)y)) return 0;
                CommitChipRename();
                g_menu.open = false; g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                std::vector<MenuItem> items;
                auto separate = [&] { if (!items.empty() && !items.back().separator) items.push_back({L"", 0, true}); };
                int current = g_appFilter;
                bool editable = ChipEditable(current), room = (int)g_customTabs.size() < kMaxCustomTabs;
                bool hidden = std::any_of(std::begin(g_builtinTabs), std::end(g_builtinTabs), [](const BuiltinTab& b) { return b.hidden; });
                int chip = FilterAt((float)x, (float)y);
                if (chip != kNoChip) {
                    g_menuChip = chip;
                    if (ChipEditable(chip)) {
                        items.push_back({Tr(L"Add app"), kMenuAddApps});
                        if (chip >= kChipCustom) items.push_back({Tr(L"Rename tab"), kMenuRenameTab});
                    }
                    if (room) items.push_back({Tr(L"New tab"), kMenuNewTab});
                    if (hidden) items.push_back({Tr(L"Restore default tabs"), kMenuRestoreTabs});
                    if (ChipEditable(chip)) {
                        separate();
                        items.push_back({Tr(L"Delete tab"), kMenuDeleteTab, false, true});
                    }
                } else if (App* a = AppOfMain(ItemAt(x, y))) {
                    g_menu.appIdx = ItemAt(x, y);
                    g_menuChip = current;
                    if (editable) {
                        items.push_back({Tr(L"Add app"), kMenuAddApps});
                        items.push_back({Tr(L"Remove from tab"), kMenuRemoveFromTab});
                    } else {
                        if (room) items.push_back({Tr(L"New tab with this app"), kMenuNewTab});
                        for (int k = 0; k < (int)g_customTabs.size(); k++) {
                            if (TabHas(k, *a)) continue;
                            wchar_t text[160];
                            swprintf(text, 160, Tr(L"Add to \u201C%ls\u201D"), g_customTabs[k].name.c_str());
                            items.push_back({text, kMenuAddToTab + k});
                        }
                    }
                    separate();
                    items.push_back({Tr(L"Pin to taskbar"), kMenuPin});
                    separate();
                    items.push_back({Tr(L"Change icon..."), kMenuChangeIcon});
                    if (g_customIcons.count(a->target)) items.push_back({Tr(L"Reset icon"), kMenuResetIcon});
                    separate();
                    items.push_back({Tr(L"Uninstall"), kMenuUninstall, false, true});
                } else {
                    g_menuChip = current;
                    if (editable) items.push_back({Tr(L"Add app"), kMenuAddApps});
                    if (room) items.push_back({Tr(L"New tab"), kMenuNewTab});
                    if (hidden) items.push_back({Tr(L"Restore default tabs"), kMenuRestoreTabs});
                }
                if (items.empty()) return 0;
                OpenMenu((float)x, (float)y, std::move(items));
                return 0;
            }

            CommitRename();
            g_menu.open = false; g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
            std::vector<MenuItem> items;
            int n = (int)g_selection.size();
            wchar_t withNText[96];
            swprintf(withNText, 96, Tr(L"New folder with %d items"), n);
            std::wstring withN = withNText;
            if (g_openFolderIndex >= 0) {
                if (!InRect(g_layout.panel, (float)x, (float)y) && !InRect(TitleRect(g_layout), (float)x, (float)y)) return 0;
                g_menu.folder = g_openFolderIndex;
            } else {
                int index = ItemAt(x, y);
                if (index >= 0 && IsFolderItem(g_layoutItems[index])) g_menu.folder = g_layoutItems[index] - 1000;
                else if (App* a = AppOfMain(index)) {
                    g_menu.appIdx = index;
                    if (SelectableApp(index)) {
                        bool sel = g_selection.count(a) != 0;
                        items.push_back({sel ? Tr(L"Deselect") : Tr(L"Select"), kMenuSelect});
                        items.push_back({sel && n > 1 ? withN : std::wstring(Tr(L"New folder with this app")), kMenuNewFolder});
                        items.push_back({L"", 0, true});
                        items.push_back({sel && n > 1 ? Tr(L"Pin selected to taskbar") : Tr(L"Pin to taskbar"), kMenuPin});
                    } else {
                        items.push_back({Tr(L"Pin to taskbar"), kMenuPin});
                    }
                    items.push_back({L"", 0, true});
                    items.push_back({Tr(L"Change icon..."), kMenuChangeIcon});
                    if (g_customIcons.count(a->target)) items.push_back({Tr(L"Reset icon"), kMenuResetIcon});
                    items.push_back({L"", 0, true});
                    items.push_back({Tr(L"Uninstall"), kMenuUninstall, false, true});
                } else {
                    items.push_back({n ? withN : std::wstring(Tr(L"New folder")), kMenuNewFolder});
                    items.push_back({Tr(L"Select all"), kMenuSelectAll});
                    if (n) items.push_back({Tr(L"Clear selection"), kMenuClearSelection});
                }
            }
            if (g_menu.folder >= 0) {
                items.clear();
                if (g_openFolderIndex >= 0) {
                    int idx = ItemAt(x, y);
                    if (idx >= 0) {
                        g_menu.folderApp = idx;
                        if (App* fa = MenuApp()) {
                            items.push_back({Tr(L"Change icon..."), kMenuChangeIcon});
                            if (g_customIcons.count(fa->target)) items.push_back({Tr(L"Reset icon"), kMenuResetIcon});
                            items.push_back({L"", 0, true});
                            items.push_back({Tr(L"Uninstall"), kMenuUninstall, false, true});
                            items.push_back({L"", 0, true});
                        }
                    }
                }
                items.push_back({Tr(L"Rename"), kMenuRename});
                items.push_back({L"", 0, true});
                items.push_back({Tr(L"Undo folder"), kMenuUndoFolder, false, true});
            }
            OpenMenu((float)x, (float)y, std::move(items));
            return 0;
        }
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            if (g_menu.open || g_closing) return 0;
            if (g_settings.siriOriginal && g_siriTab == 0 && ChipsOverflow()) {
                POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ScreenToClient(hwnd, &p);
                if (InRect(g_layout.siriTabs, (float)p.x, (float)p.y)) {
                    // The filter strip scrolls sideways
                    float d = (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA * 60 * g_layout.scale;
                    g_chipScroll += msg == WM_MOUSEHWHEEL ? d : -d;
                    ClampChipScroll(); Render();
                    return 0;
                }
            }
            if (!FolderSettled()) return 0;
            bool h = msg == WM_MOUSEHWHEEL;
            if (h && !g_layout.horizontal) return 0;
            if (g_layout.horizontal && Now() - g_lastFlip < 0.28) { g_wheelDelta = 0; return 0; }
            g_wheelDelta += GET_WHEEL_DELTA_WPARAM(wParam);
            if (std::abs(g_wheelDelta) >= WHEEL_DELTA) {
                int steps = g_wheelDelta / WHEEL_DELTA; g_wheelDelta %= WHEEL_DELTA;
                ScrollRows(h ? steps : -steps);
                if (g_layout.horizontal) g_lastFlip = Now();
            }
            return 0;
        }
        case WM_KEYDOWN: OnKeyDown(wParam); return 0;
        case WM_CHAR: OnChar((WCHAR)wParam); return 0;
        case WM_CLOSE: Close(); return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kTimerLongPress);
            KillTimer(hwnd, kTimerFileSearch);
            RemoveClipboardFormatListener(hwnd); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HANDLE g_thread;
HANDLE g_readyEvent;


DWORD WINAPI StartThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); OleInitialize(nullptr);
    LoadSettings(); LoadFolders(); LoadCustomTabs(); LoadBuiltinTabs(); LoadCustomIcons();
    LoadSearchHistory(); LoadLaunchCounts();
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)g_dwrite.GetAddressOf());
    HINSTANCE instance = (HINSTANCE)&__ImageBase;

    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kWindowClass;
    RegisterClassExW(&wc);

    HWND wnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED, kWindowClass, L"Start", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    bool ok = g_d2d && g_dwrite && wnd;
    if (ok) { g_wnd = wnd; SetTimer(wnd, kTimerFirstLoad, 3000, nullptr); AddClipboardFormatListener(wnd); CreateGlassWindow(instance); }
    SetEvent(g_readyEvent);
    if (ok) g_frameWaitTimer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (ok) {
        MSG msg;
        bool running = true;
        while (running) {
            DWORD handles = g_frameWaitTimer ? 1 : 0;
            DWORD wait = MsgWaitForMultipleObjectsEx(handles, handles ? &g_frameWaitTimer : nullptr, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (handles && wait == WAIT_OBJECT_0 && g_frameTimer) OnFrame();
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) { running = false; break; }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    } else { if (wnd) DestroyWindow(wnd); }
    if (g_frameWaitTimer) { CloseHandle(g_frameWaitTimer); g_frameWaitTimer = nullptr; }
    g_frameTimer = false;
    g_wnd = nullptr; g_workerStop = true;
    ++g_fileSearchGen;                                   // every file thread becomes stale
    if (g_worker) { WaitForSingleObject(g_worker, INFINITE); CloseHandle(g_worker); g_worker = nullptr; }
    while (g_fileThreads > 0) Sleep(10);                 // file threads run code from this module
    UnregisterEvIpcClass();                              // Everything IPC reply windows are gone
    AcquireSRWLockExclusive(&g_extIconsLock);
    g_extIcons.clear(); g_extIconsPx = 0;
    ReleaseSRWLockExclusive(&g_extIconsLock);

    MSG msg;
    while (PeekMessageW(&msg, nullptr, WM_APP_APPS, WM_APP_APPS, PM_REMOVE)) delete (AppList*)msg.lParam;
    while (PeekMessageW(&msg, nullptr, WM_APP_FILES, WM_APP_FILES, PM_REMOVE)) delete (FileResult*)msg.lParam;
    g_selection.clear(); g_menu = PopupMenu{};
    AppList().swap(g_apps); AppList().swap(g_tabItems); AppList().swap(g_fileResults);
    g_clipHistory.clear();
    g_searchHistory.clear();
    g_fileStatus.clear();
    g_folders.clear(); g_customIcons.clear(); g_mainItems.clear(); g_backdropImage = {}; FreeSurface(g_surface);
    g_labelFormat.Reset(); g_searchFormat.Reset(); g_siriFormat.Reset(); g_messageFormat.Reset();
    g_titleFormat.Reset(); g_menuFormat.Reset(); g_tabFormat.Reset(); g_listFormat.Reset();
    g_listSubFormat.Reset(); g_glyphFormat.Reset(); g_hintFormat.Reset();
    g_glyphLargeFormat.Reset(); g_categoryFormat.Reset(); g_keyFormat.Reset();
    ResetTarget(); g_dwrite.Reset(); g_d2d.Reset();
    DestroyGlassWindow();
    UnregisterClassW(kGlassClass, instance);
    UnregisterClassW(kWindowClass, instance); OleUninitialize(); return 0;
}

bool IsFolderWindowsProcess() {
    std::wstring cmd = ToLower(GetCommandLineW());
    return cmd.find(L"/factory") != std::wstring::npos || cmd.find(L"-embedding") != std::wstring::npos || cmd.find(L"/separate") != std::wstring::npos;
}

bool HookShowStartView() {
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinui_pcshell_hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };
    return WindhawkUtils::HookSymbols(module, twinui_pcshell_hooks, ARRAYSIZE(twinui_pcshell_hooks));
}

} // namespace

BOOL Wh_ModInit() {
    if (IsFolderWindowsProcess()) return FALSE;
    LoadSettings(); if (!HookShowStartView()) return FALSE;
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, StartThread, nullptr, 0, nullptr);
    if (!g_thread) { CloseHandle(g_readyEvent); g_readyEvent = nullptr; return FALSE; }
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_thread) return;
    WaitForSingleObject(g_readyEvent, INFINITE);
    if (HWND wnd = g_wnd) PostMessageW(wnd, WM_APP_QUIT, 0, 0);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread); g_thread = nullptr; CloseHandle(g_readyEvent); g_readyEvent = nullptr;

}

void Wh_ModSettingsChanged() { if (HWND wnd = g_wnd) PostMessageW(wnd, WM_APP_RELOAD, 0, 0); }

// ==WindhawkMod==
// @id              siri-plus-launchpad
// @name            Siri Plus Launchpad
// @description     Replaces the Start menu with a macOS 27 Siri-style launcher: search bar with categories, app tabs, files, clipboard, actions and live glass
// @version         1.0.0
// @author          Thiago Silva
// @github          https://github.com/brideinflame-rgb
// @homepage        https://github.com/brideinflame-rgb/Siri-Plus-Launchpad
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomdlg32 -lwinhttp -lgdi32 -lshell32 -lshcore -ld2d1 -ldwrite -lole32 -luuid -lwindowscodecs -lshlwapi -ldwmapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Siri Plus Launchpad

A [Windhawk](https://windhawk.net) mod that replaces the Windows 11 Start menu with a macOS-inspired launcher: a Siri-style search bar with a categories menu, app tabs, file search, clipboard history, quick actions and live glass effects.

Press the **Windows key** (or click **Start**) and type, or pick what you want to search from the menu that drops down from the bar.

![Original Siri mode](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/original-siri.gif)

---

## Highlights

- **Four modes**: Original Siri (default), Floating, Window and Fullscreen
- **Live glass**: real-time blur behind the bar and panels (animated wallpapers and videos keep playing underneath), plus an experimental liquid glass with refraction in the Siri modes
- **One search for everything**: Apps, Files, Clipboard and Actions
- **App tabs**: All, automatic categories and up to 6 tabs of your own
- **Recent apps row**: recently opened or installed apps (or the most used ones) on top
- **Power options**: type *shut down*, *restart*, *sleep*, *lock* or *sign out*, or use the floating power button
- **Keyboard shortcuts**: Ctrl+1 … Ctrl+4 jump to Applications, Files, Actions and Clipboard, also with the launcher closed (configurable)
- **Custom icons**: give any app your own icon (PNG, JPG, ICO, WebP...), or pick one from a **GitHub repository** of icons, with search and a color filter
- **Themes**: Clear glass, Light, Dark, OLED (deep black), Siri (macOS 27 gradient) and a fully Custom theme, with an optional glass effect on Light, Dark and OLED
- **Settings panel**: the **⋯** button at the end of the bar opens every setting in a macOS-style panel, applied live
- **Move the bar**: hold the Siri bar and drag it anywhere, on any monitor
- **Backup**: save settings, folders, tabs and custom icons to one file and restore them, even after reinstalling
- **Your language**: follows the Windows display language, or pick one (English, Portuguese, Spanish, French, German and Italian)

---

## Screenshots

### Original Siri (default)
| Clear glass | Dark | OLED |
|---|---|---|
| ![Original Siri, Clear glass theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/original-glass.png) | ![Original Siri, Dark theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/original-dark.png) | ![Original Siri, OLED theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/original-oled.png) |

### Floating
| Clear glass | Light | Siri gradient |
|---|---|---|
| ![Floating, Clear glass theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/floating-glass.png) | ![Floating, Light theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/floating-light.png) | ![Floating, Siri theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/floating-siri.png) |

| Dark | OLED |
|---|---|
| ![Floating, Dark theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/floating-dark.png) | ![Floating, OLED theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/floating-oled.png) |

### Window
| Clear glass | Light | Siri gradient |
|---|---|---|
| ![Window, Clear glass theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/window-glass.png) | ![Window, Light theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/window-light.png) | ![Window, Siri theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/window-siri.png) |

| Dark | OLED |
|---|---|
| ![Window, Dark theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/window-dark.png) | ![Window, OLED theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/window-oled.png) |

### Fullscreen
| Clear glass | Light | Siri gradient |
|---|---|---|
| ![Fullscreen, Clear glass theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/fullscreen-glass.png) | ![Fullscreen, Light theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/fullscreen-light.png) | ![Fullscreen, Siri theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/fullscreen-siri.png) |

| Dark | OLED |
|---|---|
| ![Fullscreen, Dark theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/fullscreen-dark.png) | ![Fullscreen, OLED theme](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/fullscreen-oled.png) |

### In action
| Settings panel | Moving the bar |
|---|---|
| ![The settings panel](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/settings-panel.gif) | ![Dragging the Siri bar](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/drag-the-bar.gif) |

| Right-click menus and tabs | Power options |
|---|---|
| ![Right-click menus and app tabs](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/menus-and-tabs.gif) | ![Typing to shut down](https://raw.githubusercontent.com/brideinflame-rgb/Siri-Plus-Launchpad/main/screenshots/power-options.gif) |

---

## Modes

### Original Siri (default)
A search bar in the macOS 27 style.

- **Categories menu**: hover over the bar (or click it) and a menu drops down with **Applications**, **Files**, **Actions** and **Clipboard**, each with its icon and its keyboard shortcut (default **Ctrl+1 … Ctrl+4**, configurable).
- Picking a category makes its icon and name glide from the menu into the bar, and the search then works inside that category. If you type straight away, you go to Applications.
- **Going back**: hover the category icon in the bar and it turns into a back arrow. Click it, or press **Esc**, to return to "Search" with the magnifier and pick another category.
- **App tabs**:
  - **All** comes first and shows every app.
  - Then the automatic categories: Utilities, Productivity, Entertainment, Social, Creativity, Reading and Other.
  - Then up to **6 tabs of your own**, for example "Games".
- **First row**: recently opened or installed apps, or the most used ones (configurable), separated from the rest by a thin line.
- **Actions**: Lock, Sleep, Restart, Shut down, Sign out, Settings, Task Manager, File Explorer, Control Panel, Screenshot and Empty Recycle Bin. Shut down works like Windows' own (fast startup when it is on) and warns before signing off other users who are still signed in.

### Floating
The bar appears out of an animated circle, the **Siri wave** or a soft glow, with zoom and an adjustable bounce. Clicking it or typing slides the results panel down. When it closes, the bar is "sucked" back into the bubble.

### Window and Fullscreen
The classic Launchpad grid with pages or vertical scrolling, folders and a power button. Pages always slide inside the glass. Fullscreen respects the work area and the space reserved by docks.

---

## Managing app tabs (Original Siri)

| What | How |
|---|---|
| Create a tab | Right-click an app → **New tab with this app**, or right-click the tab strip / an empty area → **New tab**. Type the name and press **Enter** |
| Add apps | Inside a tab, right-click → **Add app**. A picker shows every app: **long-press** the first one, then a simple click selects or deselects the others (selected apps jiggle); **Done** saves |
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
- Keeps the texts you copy while the mod runs (in memory only, never saved). The number of items and how long a copy stays (15 minutes to 7 days, or until Windows restarts) are configurable.
- The category only shows up when there is something in it. Each item has an **×** to remove it, and right-clicking gives **Clear clipboard**, which empties the history and the Windows clipboard.
- **Passwords stay out**: items that password managers and browsers mark as secret are skipped, just like Windows' own clipboard history (Win+V).
- Shown as a **list**: the first line of each text, with a preview of the rest (or its size) underneath.
- Click an item to copy it again. Turning the category off also stops monitoring and clears the history.

## Apps and icons
- **Search** apps by typing.
- **Folders** (Window and Fullscreen): drag apps together, rename, undo. The Siri modes list the apps of every folder together.
- **Multi-select** with jiggling icons, **pin to taskbar** (also by dragging an icon to the taskbar), **uninstall**.
- **Custom icons**: right-click an app → **Change icon...** and pick an image (PNG, JPG, JPEG, BMP, ICO, GIF or WebP). The new icon is remembered across restarts; **Reset icon** brings back the original. Works in every mode, including apps inside folders.
- **Icons from GitHub**: set a repository in the settings (`owner/repository`, or a link to it or to a folder in it), then right-click an app → **Icon from GitHub...** The repository's images show in the grid; type to search them by name, or use the color dots to see only one color or put them in rainbow order. Pictures added or replaced in the repository show up the next time the gallery opens. After picking, the launcher goes back to where it was, with that app selected.
- Icon size, number of columns and the scrolling direction are configurable.

## Settings panel
- Click the **⋯** button at the end of the bar (every mode). The bar moves to the left and becomes a list of sections, and a panel with the settings comes out of the button. Every change applies at once.
- Changes made there are kept on top of Windhawk's settings, and a change made later in Windhawk wins again.
- **Theme defaults** (Appearance) brings back the chosen theme's own look. **Restore all defaults** (General) puts every setting back to how the mod comes and the bar back in the center; folders, tabs and custom icons stay.
- **Back up** saves everything to a `.siribackup` file: every setting (the GitHub repository too), folders, tabs, search history, the bar's place and the custom icons, with their pictures inside the file. **Restore a backup** brings it all back, also after uninstalling the mod (which erases its stored data) or on another PC.
- In Window and Fullscreen the window folds into its search field while the settings are open. With the bar low on the screen, the settings open upward.

## Moving the bar (Floating and Original Siri)
- Hold the bar for a moment and drag it anywhere, on any monitor. Its place is remembered. Low on the screen, the results and menus open above it.
- Right-click the bar → **Put the bar back in the center** (shown once it has been moved).

---

## Customization

All options are in the **⋯** settings panel (in six languages) and in the mod's **Settings** tab in Windhawk (English and Portuguese).

| Block | Options |
|---|---|
| ① Mode | Original Siri (default), Floating, Window, Fullscreen |
| ② Theme | Clear glass, Light, Dark, OLED, Siri (macOS 27 gradient), Custom |
| Language | Automatic (Windows) or English, Português, Español, Français, Deutsch, Italiano |
| ③ Appearance | Theme defaults button, glass transparency (each theme keeps its own; Light and OLED start solid), corner rounding, background blur (0 = no blur), blur behind right-click menus, text color (automatic: on clear glass, black over light backgrounds; or white or black), clear glass style (frosted, or experimental liquid for Floating and Original Siri, with its refraction strength and an optional live mode), glass effect on Light/Dark/OLED |
| ④ Custom theme | Color, opacity, dark-to-light gradient, border width, color and opacity |
| ⑤ Window and Fullscreen | Window size, search style (top bar or centered box), power button and its position, opening and closing animations (pop, fade, rise from/sink into the taskbar, CRT), animation speed, blur behind open folders |
| ⑥ Floating and Original Siri | Bar size, results box width and rows for each category (Files and Clipboard as wide as the bar by default), circle animation (Siri wave, soft glow, none), circle zoom, bounce of the circle and of the results box, opening/closing duration, animation speed, power options when typing, floating power button, first row (recent, most used, off), Original Siri category shortcuts for Applications, Files, Actions and Clipboard (defaults **Ctrl+1**, **Ctrl+2**, **Ctrl+3**, **Ctrl+4**; any combination starting with Ctrl, e.g. Ctrl+Shift+F or Ctrl+F2), and whether they also work with the launcher closed |
| ⑦ Opening | Open with the Windows key and/or the Start button |
| ⑧ Apps and icons | Icon size, columns, scrolling direction, jiggle, drag to the taskbar to pin, GitHub icon repository |
| ⑨ Files | On/off, search history, list or icons, indexer, maximum results |
| ⑩ Clipboard | On/off, number of items kept, how long a copy stays |

---

## Keyboard and mouse

| Key / action | What it does |
|---|---|
| **Win** / Start button | Open or close |
| Type | Search (in Original Siri, typing right away searches Applications) |
| **Ctrl+1 / 2 / 3 / 4** | Original Siri: go to Applications / Files / Actions / Clipboard (configurable). By default they also open the launcher from any program; turn that off if you need these keys elsewhere (e.g. browser tabs) |
| **Tab / Shift+Tab** | Original Siri: next/previous app tab, or move through the results in Files, Clipboard and Actions |
| Arrows + **Enter** | Move through results / open |
| Other **Ctrl+digits** | Launch the matching result (never an action such as Shut down) |
| **Esc** | Closes menus, then clears the search, goes back to the categories and finally closes |
| Right-click on the search bar | Cut, Copy, Paste, Select all, Delete all |
| Long-press an app | Select it; then a simple click selects or deselects more apps (Window/Fullscreen: right-click → **New folder with N items** puts them in a folder; Original Siri: inside the app picker) |

---

## Requirements
- Windows 11 (x64)
- [Windhawk](https://windhawk.net)
- Live blur needs **Transparency effects** turned on in Windows (*Settings → Personalization → Colors*). Without it, a blurred still image is used.
- Optional: [Everything](https://www.voidtools.com) for instant file search (run it normally, not as administrator)

## Privacy, network and files
- **Network**: the mod only goes online for **Icons from GitHub**, and only after you set a repository: it reads the repository's file list from `api.github.com` and downloads images from `raw.githubusercontent.com`. Nothing about you or your PC is sent.
- **Files it writes**: downloaded GitHub images (and the colors found for them) are cached in `%LOCALAPPDATA%\SiriPlusLaunchpad\GitHubIcons`; icons restored from a backup go to `%LOCALAPPDATA%\SiriPlusLaunchpad\Icons`. These folders stay after uninstalling and can be deleted by hand. Backups are written only where you choose.
- The clipboard history lives in memory only and is never written to disk.

## Compatibility
- **Ctrl+1 … Ctrl+4 everywhere** (on by default): while this is on, other programs no longer get these keys (for example, the browser's Ctrl+1 for the first tab). Turn it off in *Shortcuts*, or pick other combinations.
- **Fullscreen Start** (`fullscreen-start`): don't enable both. Each one replaces the Start menu through the same hook without calling the original, so only one of them would get the Windows key.
- **Spotlight Launcher**: it adds a separate search bar on Alt+Space and leaves the Start menu alone. This mod replaces the Start menu itself (Windows key and Start button) and adds app tabs, folders, files, clipboard history and actions. Both can be used together.

---

## Support the project
If you enjoy Siri Plus Launchpad, you can support its development with a Pix donation:

**[Donate with LivePix (Pix)](https://livepix.gg/kurokobr)**

---

## Author
Developed and maintained by **Thiago Silva** ([github.com/brideinflame-rgb](https://github.com/brideinflame-rgb)).

## License
Copyright (c) 2026 Thiago Silva. Licensed under the **GNU General Public License v3.0**.
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
  - dark: Dark
  - oled: OLED (deep black)
  - custom: Custom (block ④)
  $options:pt-BR:
  - glass: Vidro transparente
  - light: Claro
  - siri: Siri (degradê do macOS 27)
  - dark: Escuro
  - oled: OLED (preto profundo)
  - custom: Personalizado (bloco ④)
- language: auto
  $name: Language
  $name:pt-BR: Idioma
  $description: Automatic follows the language of Windows
  $description:pt-BR: Automático segue o idioma do Windows
  $options:
  - auto: Automatic (Windows)
  - pt: Português
  - en: English
  - es: Español
  - fr: Français
  - de: Deutsch
  - it: Italiano
  $options:pt-BR:
  - auto: Automático (Windows)
  - pt: Português
  - en: English
  - es: Español
  - fr: Français
  - de: Deutsch
  - it: Italiano
- appearance:
  - transparency: 80
    $name: Glass transparency (%)
    $name:pt-BR: "Transparência do vidro (%)"
    $description: Ready-made themes. 100 = completely clear; lower = more solid. Light and OLED start solid (0) while this stays at 80
    $description:pt-BR: "Temas prontos. 100 = totalmente transparente; menos = mais sólido. Claro e OLED começam sólidos (0) enquanto isto fica em 80"
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
  - menuTextColor: auto
    $name: Text color
    $name:pt-BR: "Cor do texto"
    $description: "Automatic: on the clear glass theme the text turns black over a light background (the bar, the menus and the results)"
    $description:pt-BR: "Automática: no tema Vidro transparente o texto fica preto sobre um fundo claro (barra, menus e resultados)"
    $options:
    - auto: Automatic (by the background)
    - white: White
    - black: Black
    $options:pt-BR:
    - auto: Automática (pelo fundo)
    - white: Branco
    - black: Preto
  - menuBlur: 100
    $name: Blur behind context menus (%, 0 = none)
    $name:pt-BR: "Desfoque atrás dos menus de contexto (%, 0 = nenhum)"
    $description: The icons and the window under a right-click menu are blurred. 100 = default, up to 200
    $description:pt-BR: "Os ícones e a janela sob um menu de clique direito ficam desfocados. 100 = padrão, até 200"
  - glassStyle: frosted
    $name: "Clear glass and Siri themes: glass style"
    $name:pt-BR: "Temas Vidro transparente e Siri: estilo do vidro"
    $options:
    - frosted: Frosted (live blur)
    - liquid: Liquid (refraction at the edges, experimental; Floating and Original Siri only)
    $options:pt-BR:
    - frosted: Fosco (desfoque ao vivo)
    - liquid: Líquido (refração nas bordas, experimental; só Flutuante e Siri original)
  - refraction: 60
    $name: "Liquid glass: refraction at the edges (%)"
    $name:pt-BR: "Vidro líquido: refração nas bordas (%)"
  - glassEffect: true
    $name: "Light, Dark and OLED themes: glass effect"
    $name:pt-BR: "Temas Claro, Escuro e OLED: efeito vidro"
    $description: "Light on the top, the thickness of the glass at the edges and a beveled rim"
    $description:pt-BR: "Brilho no topo, a espessura do vidro nas bordas e uma borda chanfrada"
  - liveGlass: false
    $name: "Liquid glass: follow the background live"
    $name:pt-BR: "Vidro líquido: acompanhar o fundo ao vivo"
    $description: "Videos and animated wallpapers move inside the glass, but the launcher no longer shows in screenshots and screen recordings"
    $description:pt-BR: "Vídeos e papéis de parede animados se mexem dentro do vidro, mas o menu deixa de aparecer em prints e gravações de tela"
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
    $name: "Applications: results box width (px)"
    $name:pt-BR: "Aplicativos: largura da caixa de resultados (px)"
  - panelRows: 3
    $name: "Applications: results box rows"
    $name:pt-BR: "Aplicativos: linhas da caixa de resultados"
  - filesWidth: 0
    $name: "Files: results box width (px)"
    $name:pt-BR: "Arquivos: largura da caixa de resultados (px)"
    $description: 0 = same width as the bar
    $description:pt-BR: "0 = mesma largura da barra"
  - filesRows: 3
    $name: "Files: results box rows"
    $name:pt-BR: "Arquivos: linhas da caixa de resultados"
  - actionsWidth: 680
    $name: "Actions: results box width (px)"
    $name:pt-BR: "Ações: largura da caixa de resultados (px)"
    $description: 0 = same width as the bar
    $description:pt-BR: "0 = mesma largura da barra"
  - actionsRows: 3
    $name: "Actions: results box rows"
    $name:pt-BR: "Ações: linhas da caixa de resultados"
  - clipboardWidth: 0
    $name: "Clipboard: results box width (px)"
    $name:pt-BR: "Área de transferência: largura da caixa de resultados (px)"
    $description: 0 = same width as the bar
    $description:pt-BR: "0 = mesma largura da barra"
  - clipboardRows: 3
    $name: "Clipboard: results box rows"
    $name:pt-BR: "Área de transferência: linhas da caixa de resultados"
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
  - shortcutsAnywhere: true
    $name: "Original Siri: the shortcuts above also work with the launcher closed"
    $name:pt-BR: "Siri original: os atalhos acima funcionam também com o menu fechado"
    $description: "They then open the launcher on that category from any program, and that program no longer gets those keys (e.g. Ctrl+1 for browser tabs)"
    $description:pt-BR: "Assim eles abrem o menu na categoria de qualquer programa, e esse programa deixa de receber essas teclas (ex.: Ctrl+1 das abas do navegador)"
  $name: "⑥ Floating and Original Siri modes"
  $name:pt-BR: "⑥ Modos Flutuante e Siri original"
- open:
  - winKey: true
    $name: The Windows key opens the launcher
    $name:pt-BR: "Abrir com a tecla Windows"
  - startButton: true
    $name: The Start button opens the launcher
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
  - githubIcons: ""
    $name: Icons from GitHub (repository)
    $name:pt-BR: "Ícones do GitHub (repositório)"
    $description: "owner/repository, or a link to it or to a folder in it. Then right-click an app > Icon from GitHub..."
    $description:pt-BR: "dono/repositório, ou o link dele ou de uma pasta dele. Depois, clique direito em um app > Ícone do GitHub..."
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
  - keep: always
    $name: How long a copy stays
    $name:pt-BR: "Por quanto tempo uma cópia fica"
    $options:
    - always: Until Windows restarts
    - 15m: 15 minutes
    - 1h: 1 hour
    - 8h: 8 hours
    - 24h: 1 day
    - 7d: 7 days
    $options:pt-BR:
    - always: Até o Windows reiniciar
    - 15m: 15 minutos
    - 1h: 1 hora
    - 8h: 8 horas
    - 24h: 1 dia
    - 7d: 7 dias
  $name: "⑩ Clipboard"
  $name:pt-BR: "⑩ Área de transferência"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <winhttp.h>
// Shell: apps folder, icons, launching
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <propsys.h>
// Windows.UI.Composition without C++/WinRT
#include <inspectable.h>
#include <winstring.h>
#undef DrawText
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <windhawk_utils.h>

#include <commdlg.h>

#include <algorithm>
#include <chrono>
#include <map>
#include <atomic>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <deque>
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
    {L"Remove this item", L"Remover este item", L"Quitar este elemento", L"Retirer cet élément", L"Dieses Element entfernen", L"Rimuovi questo elemento"},
    {L"Clear clipboard", L"Limpar área de transferência", L"Borrar portapapeles", L"Vider le presse-papiers", L"Zwischenablage leeren", L"Svuota appunti"},
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
    {L"Someone else is still using this PC. If you continue, they could lose unsaved work.", L"Outra pessoa ainda está usando este computador. Se você continuar, ela pode perder trabalhos não salvos.", L"Otra persona sigue usando este equipo. Si continúas, podría perder el trabajo no guardado.", L"Quelqu’un d’autre utilise encore ce PC. Si vous continuez, cette personne risque de perdre son travail non enregistré.", L"Jemand anderes verwendet diesen PC noch. Wenn Sie fortfahren, gehen möglicherweise ungespeicherte Arbeiten verloren.", L"Qualcun altro sta ancora usando questo PC. Se continui, potrebbe perdere il lavoro non salvato."},
    {L"Restore default tabs", L"Restaurar abas padrão", L"Restaurar pestañas predeterminadas", L"Restaurer les onglets par défaut", L"Standard-Tabs wiederherstellen", L"Ripristina schede predefinite"},
    {L"Click apps to select or deselect them", L"Clique nos aplicativos para selecionar ou desmarcar", L"Haz clic en las aplicaciones para elegirlas o quitarlas", L"Cliquez sur les applications pour les choisir ou les retirer", L"Apps anklicken zum Auswählen oder Abwählen", L"Fai clic sulle app per sceglierle o toglierle"},
    {L"Hold an app to select it", L"Segure um aplicativo para selecioná-lo", L"Mantén pulsada una aplicación para elegirla", L"Maintenez une application pour la choisir", L"App gedrückt halten, um sie auszuwählen", L"Tieni premuta un’app per sceglierla"},
    {L"Everything did not reply", L"O Everything não respondeu", L"Everything no respondió", L"Everything n’a pas répondu", L"Everything hat nicht geantwortet", L"Everything non ha risposto"},
};

PCWSTR Tr(PCWSTR source);
PCWSTR CfgT(PCWSTR en, PCWSTR pt);
PCWSTR TabLabel(int id) {
    if (id == 4) return CfgT(L"Icons from GitHub", L"Ícones do GitHub");
    return Tr(id == 0 ? L"Apps" : id == 1 ? L"Files" : id == 3 ? L"Actions" : L"Clipboard");
}

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
constexpr UINT WM_APP_SHORTCUT = WM_APP + 6;   // a category shortcut pressed while the launcher is closed (wParam = tab)
constexpr UINT WM_APP_RELOAD = WM_APP + 3;
constexpr UINT WM_APP_QUIT = WM_APP + 4;
constexpr UINT WM_APP_FILES = WM_APP + 5;

constexpr UINT_PTR kTimerFrame = 1;
constexpr UINT_PTR kTimerFirstLoad = 4;
constexpr UINT_PTR kTimerFileSearch = 5;
constexpr UINT_PTR kTimerLongPress = 6;
constexpr UINT_PTR kTimerMenuLeave = 7;   // checks where the mouse went after leaving the launcher with the categories open
constexpr UINT kFileSearchDelayMs = 220;
constexpr UINT kLongPressDelayMs = 550;

constexpr float kFolderOpenSeconds = 0.34f;
constexpr float kFolderCloseSeconds = 0.24f;

// g_layoutItems encoding (main screen):
//   0..kFolderBase-1           -> index in g_apps
//   kFolderBase..999999        -> folder (kFolderBase + folder index)
//   kFolderAppBase..kTabBase-1 -> app inside a folder (search result)
//   >= kTabBase                -> category item (Files / Clipboard): kTabBase + index in g_tabItems
constexpr int kFolderBase = 100000;
constexpr int kFolderAppBase = 1000000;
constexpr int kTabBase = 900000000;

enum { kMenuRename = 1, kMenuUndoFolder, kMenuNewFolder, kMenuSelect, kMenuSelectAll, kMenuClearSelection, kMenuPin,
       kMenuChangeIcon, kMenuResetIcon, kMenuUninstall,
       kMenuLock, kMenuSleep, kMenuRestart, kMenuShutdown, kMenuSignOut,
       kMenuHistRemove, kMenuHistClear, kMenuTabApps, kMenuTabFiles, kMenuTabClipboard, kMenuTabActions,
       kMenuNewTab, kMenuAddApps, kMenuRenameTab, kMenuDeleteTab, kMenuRemoveFromTab, kMenuRestoreTabs,
       kMenuTextCut, kMenuTextCopy, kMenuTextPaste, kMenuTextDeleteAll, kMenuTextSelectAll, kMenuClipRemove, kMenuClipClear, kMenuGhIcon, kMenuBarCenter };
constexpr int kMenuAddToTab = 300;   // + user tab index

constexpr GUID kBhidEnumItems = {0x94f60519, 0x2850, 0x4924, {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}};
constexpr GUID kBhidSFUIObject = {0x3981e225, 0xf559, 0x11d3, {0x8e, 0x3a, 0x00, 0xc0, 0x4f, 0x68, 0x37, 0xd5}};
constexpr GUID kClsidGaussianBlur = {0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

double Now() {
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}

std::wstring ToLower(std::wstring text) {
    if (!text.empty()) CharLowerBuffW(text.data(), (DWORD)text.size());
    return text;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class Theme { Custom, Light, Glass, Siri, Dark, Oled };
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
    int blurAmount = 60;
    int menuBlur = 100;            // blur behind context menus, % of the default
    int menuText = -1;             // text color: -1 automatic (clear glass: by the background), 0 white, 1 black
    int glassTransparency = 80;   // Clear glass theme
    bool liveBackground = true;   // no snapshot: animated wallpapers and videos keep playing behind
    bool glassLiquid = false;      // Clear glass theme: refraction (liquid glass) instead of the live blur
    int refraction = 60;          // liquid glass: how far the edges bend the background (%)
    bool liveGlass = false;       // liquid glass: picture retaken while open (hides the launcher from captures)
    bool glassEffect = true;      // Light, Dark and OLED themes: glass optics (sheen, edge thickness, beveled rim)
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
    // Results box per category (Applications use the two above; width 0 = as wide as the bar)
    int filesPanelW = 0, filesRows = 3, actionsPanelW = 680, actionsRows = 3, clipPanelW = 0, clipRows = 3;
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
    int clipKeepSec = 0;   // how long a copy stays in the history (0 = until Windows restarts)
};

Settings g_settings;
// Read by the Start menu hook (another thread): what is allowed to open the launcher
std::atomic<bool> g_startButtonEnabled{true}, g_winKeyEnabled{true};
std::vector<std::wstring> g_searchHistory;
bool g_historyEnabled = true;
bool g_filesList = false;

struct Shortcut { bool valid = false, shift = false; UINT vk = 0; };
Shortcut g_shortcuts[4];   // category shortcuts (Ctrl+...), indexed by tab id: Apps, Files, Clipboard, Actions
std::wstring g_shortcutLabels[4];   // as shown in the categories menu
bool g_shortcutsAnywhere = true;     // Original Siri: the category shortcuts also work with the launcher closed
UINT g_swallowVk;                    // key whose release must also be swallowed
HHOOK g_keyHook;
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

// "#rrggbb" (or "rrggbb", spaces allowed) -> color; anything else keeps the fallback
D2D1_COLOR_F ParseColor(std::wstring text, D2D1_COLOR_F fallback) {
    std::wstring hex;
    for (wchar_t ch : text) if (!iswspace(ch)) hex += ch;
    if (!hex.empty() && hex.front() == L'#') hex.erase(hex.begin());
    if (hex.size() != 6) return fallback;
    int value[6];
    for (int i = 0; i < 6; i++) {
        wchar_t ch = (wchar_t)towlower(hex[i]);
        if (ch >= L'0' && ch <= L'9') value[i] = ch - L'0';
        else if (ch >= L'a' && ch <= L'f') value[i] = 10 + ch - L'a';
        else return fallback;
    }
    auto channel = [&](int i) { return (value[i] * 16 + value[i + 1]) / 255.0f; };
    return D2D1::ColorF(channel(0), channel(2), channel(4));
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

////////////////////////////////////////////////////////////////////////////////
// Settings panel (opened from the gear in the bar): every setting of the mod, changed live.
// Changes are kept in the mod's own storage, on top of the Windhawk settings. Each one remembers the
// Windhawk value it replaced: when that value is changed in Windhawk later, Windhawk wins again.

enum CfgType { kCfgToggle, kCfgSlider, kCfgChoice, kCfgColor, kCfgShortcut, kCfgButton, kCfgText };
struct CfgOpt { PCWSTR value, en, pt; };
struct CfgItem {
    int section; CfgType type; PCWSTR key, en, pt, descEn, descPt;
    int min, max, step; PCWSTR unit; const CfgOpt* opts; int optCount;
};
struct CfgSection { PCWSTR en, pt, descEn, descPt, glyph; UINT32 color; };


// Texts written with English and Portuguese side by side; Spanish, French, German and Italian come from this table
struct CfgTranslation { PCWSTR en, es, fr, de, it; };
const CfgTranslation kCfgTranslations[] = {
    {L"Text color",
     L"Color del texto",
     L"Couleur du texte",
     L"Textfarbe",
     L"Colore del testo"},
    {L"Automatic: on clear glass, black over a light background",
     L"Automático: en el cristal transparente, negro sobre un fondo claro",
     L"Automatique : sur le verre transparent, noir sur un fond clair",
     L"Automatisch: auf klarem Glas schwarz über hellem Hintergrund",
     L"Automatico: sul vetro trasparente, nero su uno sfondo chiaro"},
    {L"White",
     L"Blanco",
     L"Blanc",
     L"Weiß",
     L"Bianco"},
    {L"Black",
     L"Negro",
     L"Noir",
     L"Schwarz",
     L"Nero"},
    {L"Blur behind menus",
     L"Desenfoque tras los menús",
     L"Flou derrière les menus",
     L"Unschärfe hinter Menüs",
     L"Sfocatura dietro i menu"},
    {L"Right-click menus. 100 = default, 0 = none",
     L"Menús del clic derecho. 100 = predeterminado, 0 = ninguno",
     L"Menus du clic droit. 100 = par défaut, 0 = aucun",
     L"Rechtsklickmenüs. 100 = Standard, 0 = keine",
     L"Menu del clic destro. 100 = predefinito, 0 = nessuna"},
    {L"Applications: results width",
     L"Aplicaciones: ancho de los resultados",
     L"Applications : largeur des résultats",
     L"Anwendungen: Breite der Ergebnisse",
     L"Applicazioni: larghezza dei risultati"},
    {L"Applications: result rows",
     L"Aplicaciones: filas de resultados",
     L"Applications : lignes de résultats",
     L"Anwendungen: Ergebniszeilen",
     L"Applicazioni: righe dei risultati"},
    {L"Files: results width",
     L"Archivos: ancho de los resultados",
     L"Fichiers : largeur des résultats",
     L"Dateien: Breite der Ergebnisse",
     L"File: larghezza dei risultati"},
    {L"Files: result rows",
     L"Archivos: filas de resultados",
     L"Fichiers : lignes de résultats",
     L"Dateien: Ergebniszeilen",
     L"File: righe dei risultati"},
    {L"Actions: results width",
     L"Acciones: ancho de los resultados",
     L"Actions : largeur des résultats",
     L"Aktionen: Breite der Ergebnisse",
     L"Azioni: larghezza dei risultati"},
    {L"Actions: result rows",
     L"Acciones: filas de resultados",
     L"Actions : lignes de résultats",
     L"Aktionen: Ergebniszeilen",
     L"Azioni: righe dei risultati"},
    {L"Clipboard: results width",
     L"Portapapeles: ancho de los resultados",
     L"Presse-papiers : largeur des résultats",
     L"Zwischenablage: Breite der Ergebnisse",
     L"Appunti: larghezza dei risultati"},
    {L"Clipboard: result rows",
     L"Portapapeles: filas de resultados",
     L"Presse-papiers : lignes de résultats",
     L"Zwischenablage: Ergebniszeilen",
     L"Appunti: righe dei risultati"},
    {L"0 = same width as the bar",
     L"0 = mismo ancho que la barra",
     L"0 = même largeur que la barre",
     L"0 = gleiche Breite wie die Leiste",
     L"0 = stessa larghezza della barra"},
    {L"As the bar",
     L"Como la barra",
     L"Comme la barre",
     L"Wie die Leiste",
     L"Come la barra"},
    {L"Theme defaults",
     L"Valores del tema",
     L"Réglages du thème",
     L"Design-Standard",
     L"Predefiniti del tema"},
    {L"Brings back the chosen theme's own look: its transparency, corners, blur and glass",
     L"Recupera el aspecto propio del tema elegido: transparencia, esquinas, desenfoque y cristal",
     L"Rétablit l’aspect propre au thème choisi : transparence, coins, flou et verre",
     L"Stellt das eigene Aussehen des gewählten Designs wieder her: Transparenz, Ecken, Unschärfe und Glas",
     L"Ripristina l’aspetto proprio del tema scelto: trasparenza, angoli, sfocatura e vetro"},
    {L"Each theme keeps its own; Light and OLED start solid. 100 = completely clear",
     L"Cada tema guarda la suya; Claro y OLED empiezan sólidos. 100 = totalmente transparente",
     L"Chaque thème garde la sienne ; Clair et OLED commencent opaques. 100 = totalement transparent",
     L"Jedes Design behält seine eigene; Hell und OLED beginnen deckend. 100 = völlig klar",
     L"Ogni tema tiene la sua; Chiaro e OLED partono opachi. 100 = completamente trasparente"},
    {L"Restore all defaults",
     L"Restaurar todos los valores predeterminados",
     L"Rétablir tous les réglages par défaut",
     L"Alle Standardwerte wiederherstellen",
     L"Ripristina tutti i predefiniti"},
    {L"Every setting goes back to how the mod comes, and the bar to the center. Folders, tabs and custom icons stay",
     L"Todos los ajustes vuelven a como viene el mod, y la barra al centro. Carpetas, pestañas e iconos personalizados se quedan",
     L"Tous les réglages reviennent à ceux d’origine du mod, et la barre au centre. Dossiers, onglets et icônes personnalisées restent",
     L"Alle Einstellungen gehen auf den Auslieferungszustand des Mods zurück, die Leiste in die Mitte. Ordner, Tabs und eigene Symbole bleiben",
     L"Tutte le impostazioni tornano come arriva la mod, e la barra al centro. Cartelle, schede e icone personalizzate restano"},
    {L"Restore",
     L"Restaurar",
     L"Rétablir",
     L"Wiederherstellen",
     L"Ripristina"},
    {L"Restore…",
     L"Restaurar…",
     L"Rétablir…",
     L"Wiederherstellen…",
     L"Ripristina…"},
    {L"Restore all the defaults?\n\nEvery setting goes back to how the mod comes, and the bar to the center. Folders, tabs and custom icons stay.",
     L"¿Restaurar todos los valores predeterminados?\n\nTodos los ajustes vuelven a como viene el mod, y la barra al centro. Carpetas, pestañas e iconos personalizados se quedan.",
     L"Rétablir tous les réglages par défaut ?\n\nTous les réglages reviennent à ceux d’origine du mod, et la barre au centre. Dossiers, onglets et icônes personnalisées restent.",
     L"Alle Standardwerte wiederherstellen?\n\nAlle Einstellungen gehen auf den Auslieferungszustand des Mods zurück, die Leiste in die Mitte. Ordner, Tabs und eigene Symbole bleiben.",
     L"Ripristinare tutti i predefiniti?\n\nTutte le impostazioni tornano come arriva la mod, e la barra al centro. Cartelle, schede e icone personalizzate restano."},
    {L"Icons from GitHub",
     L"Iconos de GitHub",
     L"Icônes de GitHub",
     L"Symbole von GitHub",
     L"Icone da GitHub"},
    {L"Auto",
     L"Auto",
     L"Auto",
     L"Auto",
     L"Auto"},
    {L"Press the keys…",
     L"Pulsa las teclas…",
     L"Appuyez sur les touches…",
     L"Tasten drücken…",
     L"Premi i tasti…"},
    {L"None",
     L"Ninguno",
     L"Aucun",
     L"Keiner",
     L"Nessuno"},
    {L"Save…",
     L"Guardar…",
     L"Enregistrer…",
     L"Speichern…",
     L"Salva…"},
    {L"Open…",
     L"Abrir…",
     L"Ouvrir…",
     L"Öffnen…",
     L"Apri…"},
    {L"owner/repository",
     L"propietario/repositorio",
     L"propriétaire/dépôt",
     L"besitzer/repository",
     L"proprietario/repository"},
    {L"Results",
     L"Resultados",
     L"Résultats",
     L"Ergebnisse",
     L"Risultati"},
    {L"Done",
     L"Listo",
     L"Terminé",
     L"Fertig",
     L"Fine"},
    {L"No setting found",
     L"No se encontró ningún ajuste",
     L"Aucun réglage trouvé",
     L"Keine Einstellung gefunden",
     L"Nessuna impostazione trovata"},
    {L"Search settings",
     L"Buscar ajustes",
     L"Rechercher des réglages",
     L"Einstellungen durchsuchen",
     L"Cerca nelle impostazioni"},
    {L"Siri Plus Launchpad backup",
     L"Copia de seguridad de Siri Plus Launchpad",
     L"Sauvegarde de Siri Plus Launchpad",
     L"Siri Plus Launchpad-Sicherung",
     L"Backup di Siri Plus Launchpad"},
    {L"All files",
     L"Todos los archivos",
     L"Tous les fichiers",
     L"Alle Dateien",
     L"Tutti i file"},
    {L"Save backup",
     L"Guardar copia de seguridad",
     L"Enregistrer la sauvegarde",
     L"Sicherung speichern",
     L"Salva backup"},
    {L"Restore backup",
     L"Restaurar copia de seguridad",
     L"Restaurer la sauvegarde",
     L"Sicherung wiederherstellen",
     L"Ripristina backup"},
    {L"Could not save the backup in this place.",
     L"No se pudo guardar la copia de seguridad en este lugar.",
     L"Impossible d’enregistrer la sauvegarde à cet emplacement.",
     L"Die Sicherung konnte hier nicht gespeichert werden.",
     L"Impossibile salvare il backup in questa posizione."},
    {L"Backup saved.\n\nSettings, GitHub repository, folders, tabs and %d custom icon(s) are in the file.",
     L"Copia de seguridad guardada.\n\nAjustes, repositorio de GitHub, carpetas, pestañas y %d icono(s) personalizado(s) están en el archivo.",
     L"Sauvegarde enregistrée.\n\nRéglages, dépôt GitHub, dossiers, onglets et %d icône(s) personnalisée(s) sont dans le fichier.",
     L"Sicherung gespeichert.\n\nEinstellungen, GitHub-Repository, Ordner, Tabs und %d eigene(s) Symbol(e) sind in der Datei.",
     L"Backup salvato.\n\nImpostazioni, repository GitHub, cartelle, schede e %d icona/e personalizzata/e sono nel file."},
    {L"\n\n%d icon(s) were left out: the picture is no longer on disk.",
     L"\n\n%d icono(s) quedaron fuera: la imagen ya no está en el disco.",
     L"\n\n%d icône(s) laissée(s) de côté : l’image n’est plus sur le disque.",
     L"\n\n%d Symbol(e) ausgelassen: das Bild ist nicht mehr auf dem Datenträger.",
     L"\n\n%d icona/e esclusa/e: l’immagine non è più sul disco."},
    {L"This file is not a Siri Plus Launchpad backup.",
     L"Este archivo no es una copia de seguridad de Siri Plus Launchpad.",
     L"Ce fichier n’est pas une sauvegarde de Siri Plus Launchpad.",
     L"Diese Datei ist keine Siri Plus Launchpad-Sicherung.",
     L"Questo file non è un backup di Siri Plus Launchpad."},
    {L"Restore this backup?\n\nThe current settings, folders, tabs and custom icons are replaced by the ones in the file.",
     L"¿Restaurar esta copia de seguridad?\n\nLos ajustes, carpetas, pestañas e iconos personalizados actuales se reemplazan por los del archivo.",
     L"Restaurer cette sauvegarde ?\n\nLes réglages, dossiers, onglets et icônes personnalisées actuels sont remplacés par ceux du fichier.",
     L"Diese Sicherung wiederherstellen?\n\nDie aktuellen Einstellungen, Ordner, Tabs und eigenen Symbole werden durch die aus der Datei ersetzt.",
     L"Ripristinare questo backup?\n\nLe impostazioni, cartelle, schede e icone personalizzate attuali vengono sostituite da quelle del file."},
    {L"Backup restored.\n\n%d custom icon(s) back.",
     L"Copia de seguridad restaurada.\n\n%d icono(s) personalizado(s) recuperado(s).",
     L"Sauvegarde restaurée.\n\n%d icône(s) personnalisée(s) récupérée(s).",
     L"Sicherung wiederhergestellt.\n\n%d eigene(s) Symbol(e) zurück.",
     L"Backup ripristinato.\n\n%d icona/e personalizzata/e recuperata/e."},
    {L"\n%d icon(s) could not be written.",
     L"\nNo se pudieron guardar %d icono(s).",
     L"\n%d icône(s) n’ont pas pu être écrites.",
     L"\n%d Symbol(e) konnten nicht geschrieben werden.",
     L"\nImpossibile scrivere %d icona/e."},
    {L"\n\nThe restored mode is used the next time the launcher opens.",
     L"\n\nEl modo restaurado se usa la próxima vez que se abra el menú.",
     L"\n\nLe mode restauré s’applique à la prochaine ouverture du menu.",
     L"\n\nDer wiederhergestellte Modus gilt beim nächsten Öffnen des Menüs.",
     L"\n\nLa modalità ripristinata vale alla prossima apertura del menu."},
    {L"No connection to GitHub",
     L"Sin conexión con GitHub",
     L"Pas de connexion à GitHub",
     L"Keine Verbindung zu GitHub",
     L"Nessuna connessione a GitHub"},
    {L"Repository or folder not found",
     L"Repositorio o carpeta no encontrados",
     L"Dépôt ou dossier introuvable",
     L"Repository oder Ordner nicht gefunden",
     L"Repository o cartella non trovati"},
    {L"GitHub's limit for now was reached; try again in a while",
     L"Se alcanzó el límite de GitHub por ahora; inténtalo dentro de un rato",
     L"La limite de GitHub est atteinte pour l’instant ; réessayez plus tard",
     L"Das GitHub-Limit ist vorerst erreicht; später erneut versuchen",
     L"Limite di GitHub raggiunto per ora; riprova tra poco"},
    {L"GitHub didn't answer as expected",
     L"GitHub no respondió como se esperaba",
     L"GitHub n’a pas répondu comme prévu",
     L"GitHub hat nicht wie erwartet geantwortet",
     L"GitHub non ha risposto come previsto"},
    {L"Loading the icons from GitHub...",
     L"Cargando los iconos de GitHub...",
     L"Chargement des icônes de GitHub...",
     L"Symbole von GitHub werden geladen...",
     L"Caricamento delle icone da GitHub..."},
    {L"No images (PNG, ICO, JPG...) in this repository",
     L"No hay imágenes (PNG, ICO, JPG...) en este repositorio",
     L"Aucune image (PNG, ICO, JPG...) dans ce dépôt",
     L"Keine Bilder (PNG, ICO, JPG...) in diesem Repository",
     L"Nessuna immagine (PNG, ICO, JPG...) in questo repository"},
    {L"Finding the colors of the icons...",
     L"Buscando los colores de los iconos...",
     L"Recherche des couleurs des icônes...",
     L"Farben der Symbole werden ermittelt...",
     L"Ricerca dei colori delle icone..."},
    {L"No icon found",
     L"No se encontró ningún icono",
     L"Aucune icône trouvée",
     L"Kein Symbol gefunden",
     L"Nessuna icona trovata"},
    {L"Set the repository first: settings (⋯) > Apps and icons > Icons from GitHub, for example\nowner/repository  or  https://github.com/owner/repository/tree/main/icons",
     L"Indica primero el repositorio: ajustes (⋯) > Aplicaciones e iconos > Iconos de GitHub, por ejemplo\npropietario/repositorio  o  https://github.com/propietario/repositorio/tree/main/iconos",
     L"Indiquez d’abord le dépôt : réglages (⋯) > Applications et icônes > Icônes de GitHub, par exemple\npropriétaire/dépôt  ou  https://github.com/propriétaire/dépôt/tree/main/icones",
     L"Zuerst das Repository angeben: Einstellungen (⋯) > Apps und Symbole > Symbole von GitHub, zum Beispiel\nbesitzer/repository  oder  https://github.com/besitzer/repository/tree/main/symbole",
     L"Indica prima il repository: impostazioni (⋯) > App e icone > Icone da GitHub, per esempio\nproprietario/repository  oppure  https://github.com/proprietario/repository/tree/main/icone"},
    {L"Could not start the download",
     L"No se pudo iniciar la descarga",
     L"Impossible de lancer le téléchargement",
     L"Download konnte nicht gestartet werden",
     L"Impossibile avviare il download"},
    {L"Could not download this image.",
     L"No se pudo descargar esta imagen.",
     L"Impossible de télécharger cette image.",
     L"Dieses Bild konnte nicht heruntergeladen werden.",
     L"Impossibile scaricare questa immagine."},
    {L"Could not load this image.",
     L"No se pudo cargar esta imagen.",
     L"Impossible de charger cette image.",
     L"Dieses Bild konnte nicht geladen werden.",
     L"Impossibile caricare questa immagine."},
    {L"%d icons · colors %d%%",
     L"%d iconos · colores %d%%",
     L"%d icônes · couleurs %d%%",
     L"%d Symbole · Farben %d%%",
     L"%d icone · colori %d%%"},
    {L"%d icons",
     L"%d iconos",
     L"%d icônes",
     L"%d Symbole",
     L"%d icone"},
    {L"Type the icon's name...",
     L"Escribe el nombre del icono...",
     L"Tapez le nom de l’icône...",
     L"Namen des Symbols eingeben...",
     L"Scrivi il nome dell’icona..."},
    {L"Put the bar back in the center",
     L"Volver a centrar la barra",
     L"Remettre la barre au centre",
     L"Leiste wieder zentrieren",
     L"Rimetti la barra al centro"},
    {L"Icon from GitHub...",
     L"Icono de GitHub...",
     L"Icône de GitHub...",
     L"Symbol von GitHub...",
     L"Icona da GitHub..."},
    {L"General",
     L"General",
     L"Général",
     L"Allgemein",
     L"Generale"},
    {L"Launcher mode and theme",
     L"Modo del menú y tema",
     L"Mode du menu et thème",
     L"Menümodus und Design",
     L"Modalità del menu e tema"},
    {L"Appearance",
     L"Apariencia",
     L"Apparence",
     L"Darstellung",
     L"Aspetto"},
    {L"Glass, corners and background blur",
     L"Cristal, esquinas y desenfoque del fondo",
     L"Verre, coins et flou de l’arrière-plan",
     L"Glas, Ecken und Hintergrundunschärfe",
     L"Vetro, angoli e sfocatura dello sfondo"},
    {L"Custom theme",
     L"Tema personalizado",
     L"Thème personnalisé",
     L"Eigenes Design",
     L"Tema personalizzato"},
    {L"Used when the theme is Custom",
     L"Se usa cuando el tema es Personalizado",
     L"Utilisé quand le thème est Personnalisé",
     L"Gilt, wenn das Design Eigenes ist",
     L"Usato quando il tema è Personalizzato"},
    {L"Siri bar",
     L"Barra Siri",
     L"Barre Siri",
     L"Siri-Leiste",
     L"Barra Siri"},
    {L"Size of the bar and of the results box",
     L"Tamaño de la barra y del cuadro de resultados",
     L"Taille de la barre et de la zone de résultats",
     L"Größe der Leiste und des Ergebnisfelds",
     L"Dimensioni della barra e del riquadro dei risultati"},
    {L"Animations",
     L"Animaciones",
     L"Animations",
     L"Animationen",
     L"Animazioni"},
    {L"How the bar opens and closes",
     L"Cómo se abre y se cierra la barra",
     L"Comment la barre s’ouvre et se ferme",
     L"Wie die Leiste öffnet und schließt",
     L"Come la barra si apre e si chiude"},
    {L"Shortcuts",
     L"Atajos",
     L"Raccourcis",
     L"Tastenkürzel",
     L"Scorciatoie"},
    {L"Ctrl shortcuts for the categories",
     L"Atajos Ctrl para las categorías",
     L"Raccourcis Ctrl pour les catégories",
     L"Strg-Kürzel für die Kategorien",
     L"Scorciatoie Ctrl per le categorie"},
    {L"Window and Fullscreen",
     L"Ventana y Pantalla completa",
     L"Fenêtre et Plein écran",
     L"Fenster und Vollbild",
     L"Finestra e Schermo intero"},
    {L"Only for the Window and Fullscreen modes",
     L"Solo para los modos Ventana y Pantalla completa",
     L"Seulement pour les modes Fenêtre et Plein écran",
     L"Nur für die Modi Fenster und Vollbild",
     L"Solo per le modalità Finestra e Schermo intero"},
    {L"Opening",
     L"Apertura",
     L"Ouverture",
     L"Öffnen",
     L"Apertura"},
    {L"What opens the launcher",
     L"Qué abre el menú",
     L"Ce qui ouvre le menu",
     L"Was das Menü öffnet",
     L"Cosa apre il menu"},
    {L"Apps and icons",
     L"Aplicaciones e iconos",
     L"Applications et icônes",
     L"Apps und Symbole",
     L"App e icone"},
    {L"Icons, columns and scrolling",
     L"Iconos, columnas y desplazamiento",
     L"Icônes, colonnes et défilement",
     L"Symbole, Spalten und Bildlauf",
     L"Icone, colonne e scorrimento"},
    {L"Files",
     L"Archivos",
     L"Fichiers",
     L"Dateien",
     L"File"},
    {L"File search and its history",
     L"Búsqueda de archivos y su historial",
     L"Recherche de fichiers et son historique",
     L"Dateisuche und ihr Verlauf",
     L"Ricerca file e cronologia"},
    {L"Clipboard",
     L"Portapapeles",
     L"Presse-papiers",
     L"Zwischenablage",
     L"Appunti"},
    {L"Clipboard history",
     L"Historial del portapapeles",
     L"Historique du presse-papiers",
     L"Verlauf der Zwischenablage",
     L"Cronologia degli appunti"},
    {L"Original Siri",
     L"Siri original",
     L"Siri d’origine",
     L"Original-Siri",
     L"Siri originale"},
    {L"Floating",
     L"Flotante",
     L"Flottant",
     L"Schwebend",
     L"Fluttuante"},
    {L"Window",
     L"Ventana",
     L"Fenêtre",
     L"Fenster",
     L"Finestra"},
    {L"Fullscreen",
     L"Pantalla completa",
     L"Plein écran",
     L"Vollbild",
     L"Schermo intero"},
    {L"Clear glass",
     L"Cristal transparente",
     L"Verre transparent",
     L"Klares Glas",
     L"Vetro trasparente"},
    {L"Light",
     L"Claro",
     L"Clair",
     L"Hell",
     L"Chiaro"},
    {L"Siri gradient",
     L"Degradado Siri",
     L"Dégradé Siri",
     L"Siri-Verlauf",
     L"Sfumatura Siri"},
    {L"Dark",
     L"Oscuro",
     L"Sombre",
     L"Dunkel",
     L"Scuro"},
    {L"OLED (deep black)",
     L"OLED (negro profundo)",
     L"OLED (noir profond)",
     L"OLED (tiefes Schwarz)",
     L"OLED (nero profondo)"},
    {L"Custom",
     L"Personalizado",
     L"Personnalisé",
     L"Eigenes",
     L"Personalizzato"},
    {L"Frosted (live blur)",
     L"Esmerilado (desenfoque en vivo)",
     L"Givré (flou en direct)",
     L"Milchglas (Live-Unschärfe)",
     L"Smerigliato (sfocatura dal vivo)"},
    {L"Liquid (experimental)",
     L"Líquido (experimental)",
     L"Liquide (expérimental)",
     L"Flüssig (experimentell)",
     L"Liquido (sperimentale)"},
    {L"Until Windows restarts",
     L"Hasta reiniciar Windows",
     L"Jusqu’au redémarrage de Windows",
     L"Bis Windows neu startet",
     L"Fino al riavvio di Windows"},
    {L"15 minutes",
     L"15 minutos",
     L"15 minutes",
     L"15 Minuten",
     L"15 minuti"},
    {L"1 hour",
     L"1 hora",
     L"1 heure",
     L"1 Stunde",
     L"1 ora"},
    {L"8 hours",
     L"8 horas",
     L"8 heures",
     L"8 Stunden",
     L"8 ore"},
    {L"1 day",
     L"1 día",
     L"1 jour",
     L"1 Tag",
     L"1 giorno"},
    {L"7 days",
     L"7 días",
     L"7 jours",
     L"7 Tage",
     L"7 giorni"},
    {L"Top bar",
     L"Barra superior",
     L"Barre en haut",
     L"Leiste oben",
     L"Barra in alto"},
    {L"Centered box",
     L"Cuadro centrado",
     L"Zone centrée",
     L"Zentriertes Feld",
     L"Riquadro centrato"},
    {L"Left",
     L"Izquierda",
     L"À gauche",
     L"Links",
     L"A sinistra"},
    {L"Right",
     L"Derecha",
     L"À droite",
     L"Rechts",
     L"A destra"},
    {L"Pop in",
     L"Aparecer",
     L"Apparaître",
     L"Einblenden",
     L"Comparsa"},
    {L"Fade",
     L"Desvanecer",
     L"Fondu",
     L"Überblenden",
     L"Dissolvenza"},
    {L"Rise from taskbar",
     L"Subir desde la barra de tareas",
     L"Monter depuis la barre des tâches",
     L"Aus der Taskleiste aufsteigen",
     L"Sali dalla barra delle applicazioni"},
    {L"CRT",
     L"CRT",
     L"CRT",
     L"CRT",
     L"CRT"},
    {L"Pop out",
     L"Desaparecer",
     L"Disparaître",
     L"Ausblenden",
     L"Scomparsa"},
    {L"Sink into taskbar",
     L"Bajar a la barra de tareas",
     L"Descendre dans la barre des tâches",
     L"In die Taskleiste absinken",
     L"Scendi nella barra delle applicazioni"},
    {L"Siri wave",
     L"Onda Siri",
     L"Onde Siri",
     L"Siri-Welle",
     L"Onda Siri"},
    {L"Soft glow",
     L"Brillo suave",
     L"Lueur douce",
     L"Sanftes Leuchten",
     L"Bagliore morbido"},
    {L"Recently opened or installed",
     L"Abiertas o instaladas recientemente",
     L"Ouvertes ou installées récemment",
     L"Zuletzt geöffnet oder installiert",
     L"Aperte o installate di recente"},
    {L"Most used",
     L"Más usadas",
     L"Les plus utilisées",
     L"Meistverwendet",
     L"Più usate"},
    {L"Off",
     L"Desactivada",
     L"Désactivée",
     L"Aus",
     L"Disattivata"},
    {L"Vertical",
     L"Vertical",
     L"Vertical",
     L"Vertikal",
     L"Verticale"},
    {L"Horizontal (pages)",
     L"Horizontal (páginas)",
     L"Horizontal (pages)",
     L"Horizontal (Seiten)",
     L"Orizzontale (pagine)"},
    {L"List",
     L"Lista",
     L"Liste",
     L"Liste",
     L"Elenco"},
    {L"Icons (grid)",
     L"Iconos (cuadrícula)",
     L"Icônes (grille)",
     L"Symbole (Raster)",
     L"Icone (griglia)"},
    {L"Automatic",
     L"Automático",
     L"Automatique",
     L"Automatisch",
     L"Automatico"},
    {L"Everything",
     L"Everything",
     L"Everything",
     L"Everything",
     L"Everything"},
    {L"Windows Search",
     L"Windows Search",
     L"Windows Search",
     L"Windows-Suche",
     L"Ricerca di Windows"},
    {L"Direct scan (slow)",
     L"Exploración directa (lenta)",
     L"Analyse directe (lente)",
     L"Direkte Suche (langsam)",
     L"Scansione diretta (lenta)"},
    {L"Mode",
     L"Modo",
     L"Mode",
     L"Modus",
     L"Modalità"},
    {L"The new mode is used the next time the launcher opens",
     L"El nuevo modo se usa la próxima vez que se abra el menú",
     L"Le nouveau mode s’applique à la prochaine ouverture du menu",
     L"Der neue Modus gilt beim nächsten Öffnen des Menüs",
     L"La nuova modalità vale alla prossima apertura del menu"},
    {L"Theme",
     L"Tema",
     L"Thème",
     L"Design",
     L"Tema"},
    {L"Back up",
     L"Hacer copia de seguridad",
     L"Sauvegarder",
     L"Sichern",
     L"Fai un backup"},
    {L"Saves every setting, the GitHub repository, folders, tabs and the custom icons (their pictures go inside the file)",
     L"Guarda todos los ajustes, el repositorio de GitHub, carpetas, pestañas y los iconos personalizados (sus imágenes van dentro del archivo)",
     L"Enregistre tous les réglages, le dépôt GitHub, les dossiers, les onglets et les icônes personnalisées (leurs images sont incluses dans le fichier)",
     L"Speichert alle Einstellungen, das GitHub-Repository, Ordner, Tabs und die eigenen Symbole (ihre Bilder kommen mit in die Datei)",
     L"Salva tutte le impostazioni, il repository GitHub, cartelle, schede e le icone personalizzate (le immagini vanno dentro il file)"},
    {L"Restore a backup",
     L"Restaurar una copia de seguridad",
     L"Restaurer une sauvegarde",
     L"Sicherung wiederherstellen",
     L"Ripristina un backup"},
    {L"Brings everything back from a backup file, even after reinstalling the mod or on another PC",
     L"Recupera todo desde un archivo de copia de seguridad, incluso tras reinstalar el mod o en otro PC",
     L"Récupère tout depuis un fichier de sauvegarde, même après avoir réinstallé le mod ou sur un autre PC",
     L"Stellt alles aus einer Sicherungsdatei wieder her, auch nach einer Neuinstallation des Mods oder auf einem anderen PC",
     L"Ripristina tutto da un file di backup, anche dopo aver reinstallato la mod o su un altro PC"},
    {L"Glass transparency",
     L"Transparencia del cristal",
     L"Transparence du verre",
     L"Glastransparenz",
     L"Trasparenza del vetro"},
    {L"Corner rounding",
     L"Redondeo de las esquinas",
     L"Arrondi des coins",
     L"Eckenrundung",
     L"Arrotondamento degli angoli"},
    {L"Window, open folders and the Siri panel",
     L"Ventana, carpetas abiertas y panel de Siri",
     L"Fenêtre, dossiers ouverts et panneau Siri",
     L"Fenster, geöffnete Ordner und Siri-Feld",
     L"Finestra, cartelle aperte e pannello Siri"},
    {L"Background blur",
     L"Desenfoque del fondo",
     L"Flou de l’arrière-plan",
     L"Hintergrundunschärfe",
     L"Sfocatura dello sfondo"},
    {L"0 = none. The live blur needs Windows' transparency effects",
     L"0 = ninguno. El desenfoque en vivo necesita los efectos de transparencia de Windows",
     L"0 = aucun. Le flou en direct nécessite les effets de transparence de Windows",
     L"0 = keine. Die Live-Unschärfe braucht die Transparenzeffekte von Windows",
     L"0 = nessuna. La sfocatura dal vivo richiede gli effetti di trasparenza di Windows"},
    {L"Clear glass style",
     L"Estilo del cristal transparente",
     L"Style du verre transparent",
     L"Stil des klaren Glases",
     L"Stile del vetro trasparente"},
    {L"Clear glass and Siri themes, in Floating and Original Siri. Liquid bends the background at the edges, like macOS 26. Shows the next time the launcher opens",
     L"Temas Cristal transparente y Siri, en Flotante y Siri original. Líquido curva el fondo en los bordes, como macOS 26. Se ve la próxima vez que se abra el menú",
     L"Thèmes Verre transparent et Siri, en Flottant et Siri d’origine. Liquide courbe l’arrière-plan sur les bords, comme macOS 26. Visible à la prochaine ouverture du menu",
     L"Designs Klares Glas und Siri, in Schwebend und Original-Siri. Flüssig krümmt den Hintergrund an den Rändern, wie macOS 26. Sichtbar beim nächsten Öffnen des Menüs",
     L"Temi Vetro trasparente e Siri, in Fluttuante e Siri originale. Liquido curva lo sfondo ai bordi, come macOS 26. Si vede alla prossima apertura del menu"},
    {L"Refraction at the edges",
     L"Refracción en los bordes",
     L"Réfraction sur les bords",
     L"Lichtbrechung an den Rändern",
     L"Rifrazione ai bordi"},
    {L"Liquid glass",
     L"Cristal líquido",
     L"Verre liquide",
     L"Flüssiges Glas",
     L"Vetro liquido"},
    {L"Glass effect on Light, Dark and OLED",
     L"Efecto cristal en Claro, Oscuro y OLED",
     L"Effet verre sur Clair, Sombre et OLED",
     L"Glaseffekt bei Hell, Dunkel und OLED",
     L"Effetto vetro su Chiaro, Scuro e OLED"},
    {L"Light on the top, the thickness of the glass at the edges and a beveled rim",
     L"Luz en la parte superior, el grosor del cristal en los bordes y un borde biselado",
     L"Lumière en haut, épaisseur du verre sur les bords et un rebord biseauté",
     L"Licht oben, Glasdicke an den Rändern und ein abgeschrägter Rand",
     L"Luce in alto, spessore del vetro ai bordi e un bordo smussato"},
    {L"Liquid glass follows the background live",
     L"El cristal líquido sigue el fondo en vivo",
     L"Le verre liquide suit l’arrière-plan en direct",
     L"Flüssiges Glas folgt dem Hintergrund live",
     L"Il vetro liquido segue lo sfondo dal vivo"},
    {L"Videos move inside the glass, but the launcher no longer shows in screenshots and recordings. Applies the next time it opens",
     L"Los vídeos se mueven dentro del cristal, pero el menú deja de aparecer en capturas y grabaciones. Se aplica la próxima vez que se abra",
     L"Les vidéos bougent dans le verre, mais le menu n’apparaît plus dans les captures et enregistrements. S’applique à la prochaine ouverture",
     L"Videos bewegen sich im Glas, aber das Menü erscheint nicht mehr in Screenshots und Aufnahmen. Gilt beim nächsten Öffnen",
     L"I video si muovono nel vetro, ma il menu non compare più in screenshot e registrazioni. Vale alla prossima apertura"},
    {L"Color",
     L"Color",
     L"Couleur",
     L"Farbe",
     L"Colore"},
    {L"Opacity",
     L"Opacidad",
     L"Opacité",
     L"Deckkraft",
     L"Opacità"},
    {L"Dark-to-light gradient",
     L"Degradado de oscuro a claro",
     L"Dégradé du sombre au clair",
     L"Verlauf von dunkel zu hell",
     L"Sfumatura da scuro a chiaro"},
    {L"Floating and Original Siri. 0 = off",
     L"Flotante y Siri original. 0 = desactivado",
     L"Flottant et Siri d’origine. 0 = désactivé",
     L"Schwebend und Original-Siri. 0 = aus",
     L"Fluttuante e Siri originale. 0 = disattivato"},
    {L"Border width",
     L"Grosor del borde",
     L"Épaisseur de la bordure",
     L"Rahmenbreite",
     L"Spessore del bordo"},
    {L"0 = no border",
     L"0 = sin borde",
     L"0 = sans bordure",
     L"0 = kein Rahmen",
     L"0 = nessun bordo"},
    {L"Border color",
     L"Color del borde",
     L"Couleur de la bordure",
     L"Rahmenfarbe",
     L"Colore del bordo"},
    {L"Border opacity",
     L"Opacidad del borde",
     L"Opacité de la bordure",
     L"Rahmendeckkraft",
     L"Opacità del bordo"},
    {L"Bar width",
     L"Ancho de la barra",
     L"Largeur de la barre",
     L"Leistenbreite",
     L"Larghezza della barra"},
    {L"Bar height",
     L"Alto de la barra",
     L"Hauteur de la barre",
     L"Leistenhöhe",
     L"Altezza della barra"},
    {L"First row of apps",
     L"Primera fila de aplicaciones",
     L"Première ligne d’applications",
     L"Erste App-Zeile",
     L"Prima riga di app"},
    {L"Original Siri, separated by a line",
     L"Siri original, separada por una línea",
     L"Siri d’origine, séparée par une ligne",
     L"Original-Siri, durch eine Linie getrennt",
     L"Siri originale, separata da una linea"},
    {L"Power options when typing shut down, restart…",
     L"Opciones de energía al escribir apagar, reiniciar…",
     L"Options d’alimentation en tapant arrêter, redémarrer…",
     L"Energieoptionen beim Tippen von Herunterfahren, Neustart…",
     L"Opzioni di alimentazione scrivendo arresta, riavvia…"},
    {L"Floating power button",
     L"Botón de encendido flotante",
     L"Bouton d’alimentation flottant",
     L"Schwebende Ein/Aus-Taste",
     L"Pulsante di alimentazione fluttuante"},
    {L"Next to the bar",
     L"Junto a la barra",
     L"À côté de la barre",
     L"Neben der Leiste",
     L"Accanto alla barra"},
    {L"Circle animation",
     L"Animación del círculo",
     L"Animation du cercle",
     L"Kreisanimation",
     L"Animazione del cerchio"},
    {L"Circle zoom when opening",
     L"Zoom del círculo al abrir",
     L"Zoom du cercle à l’ouverture",
     L"Kreiszoom beim Öffnen",
     L"Zoom del cerchio all’apertura"},
    {L"0 = none",
     L"0 = ninguno",
     L"0 = aucun",
     L"0 = keiner",
     L"0 = nessuno"},
    {L"Circle bounce",
     L"Rebote del círculo",
     L"Rebond du cercle",
     L"Kreis-Federn",
     L"Rimbalzo del cerchio"},
    {L"Results box bounce",
     L"Rebote del cuadro de resultados",
     L"Rebond de la zone de résultats",
     L"Federn des Ergebnisfelds",
     L"Rimbalzo del riquadro dei risultati"},
    {L"Opening duration",
     L"Duración de la apertura",
     L"Durée d’ouverture",
     L"Dauer beim Öffnen",
     L"Durata dell’apertura"},
    {L"0 = instant",
     L"0 = instantáneo",
     L"0 = instantané",
     L"0 = sofort",
     L"0 = istantaneo"},
    {L"Closing duration",
     L"Duración del cierre",
     L"Durée de fermeture",
     L"Dauer beim Schließen",
     L"Durata della chiusura"},
    {L"Animation speed",
     L"Velocidad de la animación",
     L"Vitesse de l’animation",
     L"Animationsgeschwindigkeit",
     L"Velocità dell’animazione"},
    {L"Applications",
     L"Aplicaciones",
     L"Applications",
     L"Anwendungen",
     L"Applicazioni"},
    {L"Click and press the keys (Ctrl + …). Backspace clears it",
     L"Haz clic y pulsa las teclas (Ctrl + …). Retroceso lo borra",
     L"Cliquez et appuyez sur les touches (Ctrl + …). Retour arrière l’efface",
     L"Klicken und die Tasten drücken (Strg + …). Rücktaste löscht es",
     L"Fai clic e premi i tasti (Ctrl + …). Backspace lo cancella"},
    {L"Actions",
     L"Acciones",
     L"Actions",
     L"Aktionen",
     L"Azioni"},
    {L"Also with the launcher closed",
     L"También con el menú cerrado",
     L"Aussi avec le menu fermé",
     L"Auch bei geschlossenem Menü",
     L"Anche con il menu chiuso"},
    {L"They open the launcher on that category from any program, and that program no longer gets those keys",
     L"Abren el menú en esa categoría desde cualquier programa, y ese programa deja de recibir esas teclas",
     L"Ils ouvrent le menu sur cette catégorie depuis n’importe quel programme, qui ne reçoit plus ces touches",
     L"Sie öffnen das Menü in dieser Kategorie aus jedem Programm, und dieses Programm erhält diese Tasten nicht mehr",
     L"Aprono il menu su quella categoria da qualsiasi programma, che non riceve più quei tasti"},
    {L"Window width",
     L"Ancho de la ventana",
     L"Largeur de la fenêtre",
     L"Fensterbreite",
     L"Larghezza della finestra"},
    {L"Window mode only",
     L"Solo modo Ventana",
     L"Mode Fenêtre uniquement",
     L"Nur Fenstermodus",
     L"Solo modalità Finestra"},
    {L"Window height",
     L"Alto de la ventana",
     L"Hauteur de la fenêtre",
     L"Fensterhöhe",
     L"Altezza della finestra"},
    {L"Search style",
     L"Estilo de búsqueda",
     L"Style de recherche",
     L"Suchstil",
     L"Stile di ricerca"},
    {L"Show power button",
     L"Mostrar botón de encendido",
     L"Afficher le bouton d’alimentation",
     L"Ein/Aus-Taste anzeigen",
     L"Mostra il pulsante di alimentazione"},
    {L"Power button position",
     L"Posición del botón de encendido",
     L"Position du bouton d’alimentation",
     L"Position der Ein/Aus-Taste",
     L"Posizione del pulsante di alimentazione"},
    {L"Opening animation",
     L"Animación de apertura",
     L"Animation d’ouverture",
     L"Öffnungsanimation",
     L"Animazione di apertura"},
    {L"Closing animation",
     L"Animación de cierre",
     L"Animation de fermeture",
     L"Schließanimation",
     L"Animazione di chiusura"},
    {L"Blur behind an open folder",
     L"Desenfoque detrás de una carpeta abierta",
     L"Flou derrière un dossier ouvert",
     L"Unschärfe hinter einem geöffneten Ordner",
     L"Sfocatura dietro una cartella aperta"},
    {L"Open with the Windows key",
     L"Abrir con la tecla Windows",
     L"Ouvrir avec la touche Windows",
     L"Mit der Windows-Taste öffnen",
     L"Apri con il tasto Windows"},
    {L"Open with the Start button",
     L"Abrir con el botón Inicio",
     L"Ouvrir avec le bouton Démarrer",
     L"Mit der Start-Schaltfläche öffnen",
     L"Apri con il pulsante Start"},
    {L"Icon size",
     L"Tamaño de los iconos",
     L"Taille des icônes",
     L"Symbolgröße",
     L"Dimensione delle icone"},
    {L"Floating and Original Siri use up to 56 px",
     L"Flotante y Siri original usan hasta 56 px",
     L"Flottant et Siri d’origine utilisent jusqu’à 56 px",
     L"Schwebend und Original-Siri nutzen bis zu 56 px",
     L"Fluttuante e Siri originale usano fino a 56 px"},
    {L"Columns",
     L"Columnas",
     L"Colonnes",
     L"Spalten",
     L"Colonne"},
    {L"0 = automatic",
     L"0 = automático",
     L"0 = automatique",
     L"0 = automatisch",
     L"0 = automatico"},
    {L"Scrolling direction",
     L"Dirección del desplazamiento",
     L"Sens du défilement",
     L"Bildlaufrichtung",
     L"Direzione di scorrimento"},
    {L"Jiggle selected icons",
     L"Hacer temblar los iconos seleccionados",
     L"Faire trembler les icônes sélectionnées",
     L"Ausgewählte Symbole wackeln lassen",
     L"Fai tremolare le icone selezionate"},
    {L"owner/repository, or a link to it or to a folder in it. Then right-click an app > Icon from GitHub...",
     L"propietario/repositorio, o un enlace a él o a una carpeta suya. Luego clic derecho en una aplicación > Icono de GitHub...",
     L"propriétaire/dépôt, ou un lien vers celui-ci ou vers un de ses dossiers. Puis clic droit sur une application > Icône de GitHub...",
     L"besitzer/repository oder ein Link dorthin oder zu einem Ordner darin. Dann Rechtsklick auf eine App > Symbol von GitHub...",
     L"proprietario/repository, o un link a esso o a una sua cartella. Poi clic destro su un’app > Icona da GitHub..."},
    {L"Drag an icon to the taskbar to pin it",
     L"Arrastra un icono a la barra de tareas para anclarlo",
     L"Faites glisser une icône vers la barre des tâches pour l’épingler",
     L"Symbol auf die Taskleiste ziehen, um es anzuheften",
     L"Trascina un’icona sulla barra delle applicazioni per aggiungerla"},
    {L"Files category",
     L"Categoría Archivos",
     L"Catégorie Fichiers",
     L"Kategorie Dateien",
     L"Categoria File"},
    {L"Remember searches",
     L"Recordar búsquedas",
     L"Mémoriser les recherches",
     L"Suchen merken",
     L"Ricorda le ricerche"},
    {L"Shown when the Files search is empty",
     L"Se muestran cuando la búsqueda de Archivos está vacía",
     L"Affichées quand la recherche Fichiers est vide",
     L"Angezeigt, wenn die Dateisuche leer ist",
     L"Mostrate quando la ricerca File è vuota"},
    {L"Results view",
     L"Vista de resultados",
     L"Affichage des résultats",
     L"Ergebnisansicht",
     L"Vista dei risultati"},
    {L"File indexer",
     L"Indexador de archivos",
     L"Indexeur de fichiers",
     L"Dateiindex",
     L"Indicizzatore di file"},
    {L"Everything must be running (don't run it as admin)",
     L"Everything debe estar en ejecución (no como administrador)",
     L"Everything doit être lancé (pas en tant qu’administrateur)",
     L"Everything muss laufen (nicht als Administrator)",
     L"Everything deve essere in esecuzione (non come amministratore)"},
    {L"Maximum results",
     L"Máximo de resultados",
     L"Nombre maximal de résultats",
     L"Maximale Ergebnisse",
     L"Risultati massimi"},
    {L"Clipboard category",
     L"Categoría Portapapeles",
     L"Catégorie Presse-papiers",
     L"Kategorie Zwischenablage",
     L"Categoria Appunti"},
    {L"When off, the clipboard is not monitored and the history is cleared",
     L"Si está desactivada, no se vigila el portapapeles y se borra el historial",
     L"Désactivée, le presse-papiers n’est pas surveillé et l’historique est effacé",
     L"Wenn aus, wird die Zwischenablage nicht überwacht und der Verlauf gelöscht",
     L"Se disattivata, gli appunti non vengono monitorati e la cronologia viene cancellata"},
    {L"Items kept",
     L"Elementos guardados",
     L"Éléments conservés",
     L"Gespeicherte Einträge",
     L"Elementi conservati"},
    {L"How long a copy stays",
     L"Cuánto tiempo se queda una copia",
     L"Durée de conservation d’une copie",
     L"Wie lange eine Kopie bleibt",
     L"Quanto resta una copia"},
    {L"Older copies leave the history by themselves",
     L"Las copias más antiguas salen solas del historial",
     L"Les copies plus anciennes quittent l’historique d’elles-mêmes",
     L"Ältere Kopien verlassen den Verlauf von selbst",
     L"Le copie più vecchie escono da sole dalla cronologia"},
    {L"Language",
     L"Idioma",
     L"Langue",
     L"Sprache",
     L"Lingua"},
    {L"Automatic follows the language of Windows",
     L"Automático sigue el idioma de Windows",
     L"Automatique suit la langue de Windows",
     L"Automatisch folgt der Sprache von Windows",
     L"Automatico segue la lingua di Windows"},
    {L"Automatic (Windows)",
     L"Automático (Windows)",
     L"Automatique (Windows)",
     L"Automatisch (Windows)",
     L"Automatico (Windows)"},
};

PCWSTR CfgT(PCWSTR en, PCWSTR pt) {
    if (!en) return pt;
    switch (g_uiLanguage) {
        case UiLanguage::Portuguese: return pt ? pt : en;
        case UiLanguage::English: return en;
        default: break;
    }
    // Built once, thread-safely (the GitHub download thread also asks for texts)
    [[clang::no_destroy]] static const std::unordered_map<std::wstring_view, const CfgTranslation*> byEnglish = [] {
        std::unordered_map<std::wstring_view, const CfgTranslation*> map;
        for (const CfgTranslation& t : kCfgTranslations) map.emplace(t.en, &t);
        return map;
    }();
    auto found = byEnglish.find(en);
    if (found == byEnglish.end()) return en;
    const CfgTranslation& t = *found->second;
    switch (g_uiLanguage) {
        case UiLanguage::Spanish: return t.es;
        case UiLanguage::French: return t.fr;
        case UiLanguage::German: return t.de;
        case UiLanguage::Italian: return t.it;
        default: return en;
    }
}

// A translated text with one number in it (%d)
std::wstring CfgFormat(PCWSTR format, int n) {
    wchar_t text[512];
    swprintf(text, 512, format, n);
    return text;
}

const CfgSection kCfgSections[] = {
    {L"General", L"Geral", L"Launcher mode and theme", L"Modo do menu e tema", L"", 0x8E8E93},
    {L"Appearance", L"Aparência", L"Glass, corners and background blur", L"Vidro, cantos e desfoque do fundo", L"", 0x0A84FF},
    {L"Custom theme", L"Tema personalizado", L"Used when the theme is Custom", L"Usado quando o tema é Personalizado", L"", 0xBF5AF2},
    {L"Siri bar", L"Barra Siri", L"Size of the bar and of the results box", L"Tamanho da barra e da caixa de resultados", L"", 0xFF375F},
    {L"Animations", L"Animações", L"How the bar opens and closes", L"Como a barra abre e fecha", L"", 0x30D158},
    {L"Shortcuts", L"Atalhos", L"Ctrl shortcuts for the categories", L"Atalhos Ctrl para as categorias", L"", 0x64D2FF},
    {L"Window and Fullscreen", L"Janela e Tela cheia", L"Only for the Window and Fullscreen modes", L"Só para os modos Janela e Tela cheia", L"", 0x5E5CE6},
    {L"Opening", L"Abertura", L"What opens the launcher", L"O que abre o menu", L"", 0xFF9F0A},
    {L"Apps and icons", L"Apps e ícones", L"Icons, columns and scrolling", L"Ícones, colunas e rolagem", L"", 0xFF453A},
    {L"Files", L"Arquivos", L"File search and its history", L"Pesquisa de arquivos e histórico", L"", 0x0A84FF},
    {L"Clipboard", L"Área de transferência", L"Clipboard history", L"Histórico da área de transferência", L"", 0xFFB300},
};
constexpr int kCfgSectionCount = (int)ARRAYSIZE(kCfgSections);

const CfgOpt kOptMode[] = {{L"siriOriginal", L"Original Siri", L"Siri original"}, {L"siri", L"Floating", L"Flutuante"},
                           {L"window", L"Window", L"Janela"}, {L"fullscreen", L"Fullscreen", L"Tela cheia"}};
const CfgOpt kOptTheme[] = {{L"glass", L"Clear glass", L"Vidro transparente"}, {L"light", L"Light", L"Claro"},
                            {L"siri", L"Siri gradient", L"Degradê Siri"}, {L"dark", L"Dark", L"Escuro"},
                            {L"oled", L"OLED (deep black)", L"OLED (preto profundo)"}, {L"custom", L"Custom", L"Personalizado"}};
const CfgOpt kOptGlass[] = {{L"frosted", L"Frosted (live blur)", L"Fosco (desfoque ao vivo)"}, {L"liquid", L"Liquid (experimental)", L"Líquido (experimental)"}};
const CfgOpt kOptClipKeep[] = {{L"always", L"Until Windows restarts", L"Até o Windows reiniciar"}, {L"15m", L"15 minutes", L"15 minutos"},
                               {L"1h", L"1 hour", L"1 hora"}, {L"8h", L"8 hours", L"8 horas"}, {L"24h", L"1 day", L"1 dia"}, {L"7d", L"7 days", L"7 dias"}};
const CfgOpt kOptSearch[] = {{L"bar", L"Top bar", L"Barra superior"}, {L"box", L"Centered box", L"Caixa centralizada"}};
const CfgOpt kOptSide[] = {{L"left", L"Left", L"À esquerda"}, {L"right", L"Right", L"À direita"}};
const CfgOpt kOptOpenAnim[] = {{L"zoom", L"Pop in", L"Surgir"}, {L"fade", L"Fade", L"Esmaecer"},
                               {L"slide", L"Rise from taskbar", L"Subir da barra de tarefas"}, {L"crt", L"CRT", L"CRT"}};
const CfgOpt kOptCloseAnim[] = {{L"zoom", L"Pop out", L"Desaparecer"}, {L"fade", L"Fade", L"Esmaecer"},
                                {L"slide", L"Sink into taskbar", L"Descer até a barra de tarefas"}, {L"crt", L"CRT", L"CRT"}};
const CfgOpt kOptOrb[] = {{L"wave", L"Siri wave", L"Onda Siri"}, {L"glow", L"Soft glow", L"Brilho suave"}, {L"none", L"None", L"Nenhuma"}};
const CfgOpt kOptFirstRow[] = {{L"recent", L"Recently opened or installed", L"Abertos ou instalados recentemente"},
                               {L"used", L"Most used", L"Mais usados"}, {L"off", L"Off", L"Desligada"}};
const CfgOpt kOptPage[] = {{L"vertical", L"Vertical", L"Vertical"}, {L"horizontal", L"Horizontal (pages)", L"Horizontal (páginas)"}};
const CfgOpt kOptView[] = {{L"list", L"List", L"Lista"}, {L"icons", L"Icons (grid)", L"Ícones (grade)"}};
const CfgOpt kOptIndexer[] = {{L"auto", L"Automatic", L"Automático"}, {L"everything", L"Everything", L"Everything"},
                              {L"windows", L"Windows Search", L"Pesquisa do Windows"}, {L"scan", L"Direct scan (slow)", L"Verificação direta (lenta)"}};

const CfgOpt kOptMenuText[] = {{L"auto", L"Automatic", L"Automática"}, {L"white", L"White", L"Branco"}, {L"black", L"Black", L"Preto"}};
const CfgOpt kOptLanguage[] = {{L"auto", L"Automatic (Windows)", L"Automático (Windows)"}, {L"pt", L"Português", L"Português"},
                                {L"en", L"English", L"English"}, {L"es", L"Español", L"Español"}, {L"fr", L"Français", L"Français"},
                                {L"de", L"Deutsch", L"Deutsch"}, {L"it", L"Italiano", L"Italiano"}};
#define CFG_OPTS(a) a, (int)ARRAYSIZE(a)
const CfgItem kCfgItems[] = {
    // 0 General
    {0, kCfgChoice, L"displayMode", L"Mode", L"Modo", L"The new mode is used the next time the launcher opens",
     L"O novo modo vale na próxima vez que o menu abrir", 0, 0, 0, nullptr, CFG_OPTS(kOptMode)},
    {0, kCfgChoice, L"theme", L"Theme", L"Tema", nullptr, nullptr, 0, 0, 0, nullptr, CFG_OPTS(kOptTheme)},
    {0, kCfgChoice, L"language", L"Language", L"Idioma", L"Automatic follows the language of Windows", L"Automático segue o idioma do Windows",
     0, 0, 0, nullptr, CFG_OPTS(kOptLanguage)},
    {0, kCfgButton, L"#reset", L"Restore all defaults", L"Restaurar todos os padrões",
     L"Every setting goes back to how the mod comes, and the bar to the center. Folders, tabs and custom icons stay",
     L"Todos os ajustes voltam a como o mod vem, e a barra volta ao centro. Pastas, abas e ícones personalizados ficam", 0, 0, 0, nullptr, nullptr, 0},
    {0, kCfgButton, L"#export", L"Back up", L"Fazer backup",
     L"Saves every setting, the GitHub repository, folders, tabs and the custom icons (their pictures go inside the file)",
     L"Salva todas as configurações, o repositório do GitHub, pastas, abas e os ícones personalizados (as imagens vão dentro do arquivo)",
     0, 0, 0, nullptr, nullptr, 0},
    {0, kCfgButton, L"#import", L"Restore a backup", L"Restaurar backup",
     L"Brings everything back from a backup file, even after reinstalling the mod or on another PC",
     L"Traz tudo de volta de um arquivo de backup, mesmo depois de reinstalar o mod ou em outro PC",
     0, 0, 0, nullptr, nullptr, 0},
    // 1 Appearance
    {1, kCfgButton, L"#themeReset", L"Theme defaults", L"Padrão do tema",
     L"Brings back the chosen theme's own look: its transparency, corners, blur and glass",
     L"Volta ao visual próprio do tema escolhido: transparência, cantos, desfoque e vidro", 0, 0, 0, nullptr, nullptr, 0},
    {1, kCfgSlider, L"appearance.transparency", L"Glass transparency", L"Transparência do vidro",
     L"Each theme keeps its own; Light and OLED start solid. 100 = completely clear",
     L"Cada tema guarda a sua; Claro e OLED começam sólidos. 100 = totalmente transparente", 0, 100, 1, L"%", nullptr, 0},
    {1, kCfgSlider, L"appearance.cornerRadius", L"Corner rounding", L"Arredondamento dos cantos", L"Window, open folders and the Siri panel",
     L"Janela, pastas abertas e painel do Siri", 0, 60, 1, L"px", nullptr, 0},
    {1, kCfgSlider, L"appearance.blur", L"Background blur", L"Desfoque do fundo", L"0 = none. The live blur needs Windows' transparency effects",
     L"0 = nenhum. O desfoque ao vivo precisa dos efeitos de transparência do Windows", 0, 100, 1, nullptr, nullptr, 0},
    {1, kCfgChoice, L"appearance.menuTextColor", L"Text color", L"Cor do texto",
     L"Automatic: on clear glass, black over a light background", L"Automática: no vidro transparente, preto sobre um fundo claro",
     0, 0, 0, nullptr, CFG_OPTS(kOptMenuText)},
    {1, kCfgSlider, L"appearance.menuBlur", L"Blur behind menus", L"Desfoque atrás dos menus", L"Right-click menus. 100 = default, 0 = none",
     L"Menus de clique direito. 100 = padrão, 0 = nenhum", 0, 200, 5, L"%", nullptr, 0},
    {1, kCfgChoice, L"appearance.glassStyle", L"Clear glass style", L"Estilo do vidro transparente",
     L"Clear glass and Siri themes, in Floating and Original Siri. Liquid bends the background at the edges, like macOS 26. Shows the next time the launcher opens",
     L"Temas Vidro transparente e Siri, nos modos Flutuante e Siri original. Líquido curva o fundo nas bordas, como no macOS 26. Vale na próxima vez que o menu abrir", 0, 0, 0, nullptr, CFG_OPTS(kOptGlass)},
    {1, kCfgSlider, L"appearance.refraction", L"Refraction at the edges", L"Refração nas bordas", L"Liquid glass", L"Vidro líquido",
     0, 100, 1, L"%", nullptr, 0},
    {1, kCfgToggle, L"appearance.glassEffect", L"Glass effect on Light, Dark and OLED", L"Efeito vidro nos temas Claro, Escuro e OLED",
     L"Light on the top, the thickness of the glass at the edges and a beveled rim",
     L"Brilho no topo, a espessura do vidro nas bordas e uma borda chanfrada", 0, 0, 0, nullptr, nullptr, 0},
    {1, kCfgToggle, L"appearance.liveGlass", L"Liquid glass follows the background live", L"Vidro líquido acompanha o fundo ao vivo",
     L"Videos move inside the glass, but the launcher no longer shows in screenshots and recordings. Applies the next time it opens",
     L"Vídeos se mexem dentro do vidro, mas o menu deixa de aparecer em prints e gravações. Vale na próxima vez que abrir",
     0, 0, 0, nullptr, nullptr, 0},
    // 2 Custom theme
    {2, kCfgColor, L"custom.color", L"Color", L"Cor", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {2, kCfgSlider, L"custom.opacity", L"Opacity", L"Opacidade", nullptr, nullptr, 0, 100, 1, L"%", nullptr, 0},
    {2, kCfgSlider, L"custom.shading", L"Dark-to-light gradient", L"Degradê escuro → claro", L"Floating and Original Siri. 0 = off",
     L"Flutuante e Siri original. 0 = desligado", 0, 100, 1, L"%", nullptr, 0},
    {2, kCfgSlider, L"custom.borderWidth", L"Border width", L"Espessura da borda", L"0 = no border", L"0 = sem borda", 0, 8, 1, L"px", nullptr, 0},
    {2, kCfgColor, L"custom.borderColor", L"Border color", L"Cor da borda", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {2, kCfgSlider, L"custom.borderOpacity", L"Border opacity", L"Opacidade da borda", nullptr, nullptr, 0, 100, 1, L"%", nullptr, 0},
    // 3 Siri bar
    {3, kCfgSlider, L"siri.pillWidth", L"Bar width", L"Largura da barra", nullptr, nullptr, 200, 1400, 10, L"px", nullptr, 0},
    {3, kCfgSlider, L"siri.pillHeight", L"Bar height", L"Altura da barra", nullptr, nullptr, 36, 96, 1, L"px", nullptr, 0},
    {3, kCfgSlider, L"siri.panelWidth", L"Applications: results width", L"Aplicativos: largura dos resultados", nullptr, nullptr, 300, 1400, 10, L"px", nullptr, 0},
    {3, kCfgSlider, L"siri.panelRows", L"Applications: result rows", L"Aplicativos: linhas dos resultados", nullptr, nullptr, 1, 8, 1, nullptr, nullptr, 0},
    {3, kCfgSlider, L"siri.filesWidth", L"Files: results width", L"Arquivos: largura dos resultados", L"0 = same width as the bar",
     L"0 = mesma largura da barra", 0, 1400, 10, L"px", nullptr, 0},
    {3, kCfgSlider, L"siri.filesRows", L"Files: result rows", L"Arquivos: linhas dos resultados", nullptr, nullptr, 1, 8, 1, nullptr, nullptr, 0},
    {3, kCfgSlider, L"siri.actionsWidth", L"Actions: results width", L"Ações: largura dos resultados", L"0 = same width as the bar",
     L"0 = mesma largura da barra", 0, 1400, 10, L"px", nullptr, 0},
    {3, kCfgSlider, L"siri.actionsRows", L"Actions: result rows", L"Ações: linhas dos resultados", nullptr, nullptr, 1, 8, 1, nullptr, nullptr, 0},
    {3, kCfgSlider, L"siri.clipboardWidth", L"Clipboard: results width", L"Área de transferência: largura dos resultados", L"0 = same width as the bar",
     L"0 = mesma largura da barra", 0, 1400, 10, L"px", nullptr, 0},
    {3, kCfgSlider, L"siri.clipboardRows", L"Clipboard: result rows", L"Área de transferência: linhas dos resultados", nullptr, nullptr, 1, 8, 1, nullptr, nullptr, 0},
    {3, kCfgChoice, L"siri.firstRow", L"First row of apps", L"Primeira linha de apps", L"Original Siri, separated by a line",
     L"Siri original, separada por uma linha", 0, 0, 0, nullptr, CFG_OPTS(kOptFirstRow)},
    {3, kCfgToggle, L"siri.powerPill", L"Power options when typing shut down, restart…", L"Opções de energia ao digitar desligar, reiniciar…",
     nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {3, kCfgToggle, L"siri.powerButton", L"Floating power button", L"Botão de energia flutuante", L"Next to the bar", L"Ao lado da barra",
     0, 0, 0, nullptr, nullptr, 0},
    // 4 Animations
    {4, kCfgChoice, L"siri.orbStyle", L"Circle animation", L"Animação do círculo", nullptr, nullptr, 0, 0, 0, nullptr, CFG_OPTS(kOptOrb)},
    {4, kCfgSlider, L"siri.circleZoom", L"Circle zoom when opening", L"Zoom do círculo ao abrir", L"0 = none", L"0 = nenhum", 0, 60, 1, L"%", nullptr, 0},
    {4, kCfgSlider, L"siri.bounce", L"Circle bounce", L"Bounce do círculo", L"0 = none", L"0 = nenhum", 0, 100, 1, L"%", nullptr, 0},
    {4, kCfgSlider, L"siri.panelBounce", L"Results box bounce", L"Bounce da caixa de resultados", L"0 = none", L"0 = nenhum", 0, 100, 1, L"%", nullptr, 0},
    {4, kCfgSlider, L"siri.introMs", L"Opening duration", L"Duração da abertura", L"0 = instant", L"0 = instantânea", 0, 2000, 20, L"ms", nullptr, 0},
    {4, kCfgSlider, L"siri.outroMs", L"Closing duration", L"Duração do fechamento", L"0 = instant", L"0 = instantâneo", 0, 2000, 20, L"ms", nullptr, 0},
    {4, kCfgSlider, L"siri.animationSpeed", L"Animation speed", L"Velocidade das animações", nullptr, nullptr, 25, 400, 5, L"%", nullptr, 0},
    // 5 Shortcuts
    {5, kCfgShortcut, L"siri.shortcutApps", L"Applications", L"Aplicativos", L"Click and press the keys (Ctrl + …). Backspace clears it",
     L"Clique e pressione as teclas (Ctrl + …). Backspace apaga", 0, 0, 0, nullptr, nullptr, 0},
    {5, kCfgShortcut, L"siri.shortcutFiles", L"Files", L"Arquivos", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {5, kCfgShortcut, L"siri.shortcutActions", L"Actions", L"Ações", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {5, kCfgShortcut, L"siri.shortcutClipboard", L"Clipboard", L"Área de transferência", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {5, kCfgToggle, L"siri.shortcutsAnywhere", L"Also with the launcher closed", L"Também com o menu fechado",
     L"They open the launcher on that category from any program, and that program no longer gets those keys",
     L"Abrem o menu na categoria de qualquer programa, e esse programa deixa de receber essas teclas", 0, 0, 0, nullptr, nullptr, 0},
    // 6 Window and Fullscreen
    {6, kCfgSlider, L"window.width", L"Window width", L"Largura da janela", L"Window mode only", L"Só modo Janela", 30, 100, 1, L"%", nullptr, 0},
    {6, kCfgSlider, L"window.height", L"Window height", L"Altura da janela", L"Window mode only", L"Só modo Janela", 30, 100, 1, L"%", nullptr, 0},
    {6, kCfgChoice, L"window.searchStyle", L"Search style", L"Estilo da pesquisa", nullptr, nullptr, 0, 0, 0, nullptr, CFG_OPTS(kOptSearch)},
    {6, kCfgToggle, L"window.showPowerButton", L"Show power button", L"Mostrar botão de energia", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {6, kCfgChoice, L"window.powerButtonPosition", L"Power button position", L"Posição do botão de energia", nullptr, nullptr,
     0, 0, 0, nullptr, CFG_OPTS(kOptSide)},
    {6, kCfgChoice, L"window.openAnimation", L"Opening animation", L"Animação de abertura", nullptr, nullptr, 0, 0, 0, nullptr, CFG_OPTS(kOptOpenAnim)},
    {6, kCfgChoice, L"window.closeAnimation", L"Closing animation", L"Animação de fechamento", nullptr, nullptr, 0, 0, 0, nullptr, CFG_OPTS(kOptCloseAnim)},
    {6, kCfgSlider, L"window.animationSpeed", L"Animation speed", L"Velocidade das animações", nullptr, nullptr, 25, 400, 5, L"%", nullptr, 0},
    {6, kCfgSlider, L"window.folderBlur", L"Blur behind an open folder", L"Desfoque atrás da pasta aberta", nullptr, nullptr, 0, 100, 1, nullptr, nullptr, 0},
    // 7 Opening
    {7, kCfgToggle, L"open.winKey", L"Open with the Windows key", L"Abrir com a tecla Windows", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {7, kCfgToggle, L"open.startButton", L"Open with the Start button", L"Abrir com o botão Iniciar", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    // 8 Apps and icons
    {8, kCfgSlider, L"grid.iconSize", L"Icon size", L"Tamanho dos ícones", L"Floating and Original Siri use up to 56 px",
     L"Os modos Flutuante e Siri original usam até 56 px", 24, 192, 1, L"px", nullptr, 0},
    {8, kCfgSlider, L"grid.columns", L"Columns", L"Colunas", L"0 = automatic", L"0 = automático", 0, 30, 1, nullptr, nullptr, 0},
    {8, kCfgChoice, L"grid.pageMode", L"Scrolling direction", L"Direção da rolagem", nullptr, nullptr, 0, 0, 0, nullptr, CFG_OPTS(kOptPage)},
    {8, kCfgToggle, L"grid.jiggle", L"Jiggle selected icons", L"Ícones selecionados tremem", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {8, kCfgText, L"grid.githubIcons", L"Icons from GitHub", L"Ícones do GitHub",
     L"owner/repository, or a link to it or to a folder in it. Then right-click an app > Icon from GitHub...",
     L"dono/repositório, ou o link dele ou de uma pasta dele. Depois, clique direito em um app > Ícone do GitHub...",
     0, 0, 0, nullptr, nullptr, 0},
    {8, kCfgToggle, L"grid.dragToPin", L"Drag an icon to the taskbar to pin it", L"Arrastar o ícone até a barra de tarefas fixa o app",
     nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    // 9 Files
    {9, kCfgToggle, L"files.enabled", L"Files category", L"Categoria Arquivos", nullptr, nullptr, 0, 0, 0, nullptr, nullptr, 0},
    {9, kCfgToggle, L"files.history", L"Remember searches", L"Lembrar pesquisas", L"Shown when the Files search is empty",
     L"Aparecem quando a pesquisa de Arquivos está vazia", 0, 0, 0, nullptr, nullptr, 0},
    {9, kCfgChoice, L"files.view", L"Results view", L"Exibição dos resultados", nullptr, nullptr, 0, 0, 0, nullptr, CFG_OPTS(kOptView)},
    {9, kCfgChoice, L"files.indexer", L"File indexer", L"Indexador de arquivos", L"Everything must be running (don't run it as admin)",
     L"O Everything precisa estar aberto (não execute como administrador)", 0, 0, 0, nullptr, CFG_OPTS(kOptIndexer)},
    {9, kCfgSlider, L"files.maxResults", L"Maximum results", L"Máximo de resultados", nullptr, nullptr, 10, 500, 10, nullptr, nullptr, 0},
    // 10 Clipboard
    {10, kCfgToggle, L"clipboard.enabled", L"Clipboard category", L"Categoria Área de transferência",
     L"When off, the clipboard is not monitored and the history is cleared",
     L"Desligada, a área de transferência não é monitorada e o histórico é apagado", 0, 0, 0, nullptr, nullptr, 0},
    {10, kCfgSlider, L"clipboard.size", L"Items kept", L"Itens guardados", nullptr, nullptr, 5, 200, 1, nullptr, nullptr, 0},
    {10, kCfgChoice, L"clipboard.keep", L"How long a copy stays", L"Por quanto tempo uma cópia fica",
     L"Older copies leave the history by themselves", L"Cópias mais antigas saem do histórico sozinhas", 0, 0, 0, nullptr, CFG_OPTS(kOptClipKeep)},
};
#undef CFG_OPTS
constexpr int kCfgItemCount = (int)ARRAYSIZE(kCfgItems);

const CfgItem* CfgFind(PCWSTR key) {
    for (const CfgItem& it : kCfgItems) if (wcscmp(it.key, key) == 0) return &it;
    return nullptr;
}
bool CfgIsInt(const CfgItem& it) { return it.type == kCfgToggle || it.type == kCfgSlider; }

struct CfgOverride { std::wstring value, base; };
[[clang::no_destroy]] std::map<std::wstring, CfgOverride> g_cfgOverrides;
bool g_cfgOverridesLoaded, g_cfgOverridesDirty;
constexpr WCHAR kCfgOverridesKey[] = L"settingsPanel";

void LoadCfgOverrides() {
    g_cfgOverridesLoaded = true;
    g_cfgOverrides.clear();
    std::vector<WCHAR> buf(32768);
    Wh_GetStringValue(kCfgOverridesKey, buf.data(), buf.size());
    std::wstring text = buf.data();
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find(L'\n', pos);
        if (end == std::wstring::npos) end = text.size();
        std::wstring line = text.substr(pos, end - pos);
        pos = end + 1;
        size_t a = line.find(L'\t'), b = a == std::wstring::npos ? a : line.find(L'\t', a + 1);
        if (b == std::wstring::npos) continue;
        std::wstring key = line.substr(0, a);
        if (CfgFind(key.c_str())) g_cfgOverrides[key] = {line.substr(a + 1, b - a - 1), line.substr(b + 1)};
    }
}

void SaveThemeTransparency();
void SaveCfgOverrides() {
    SaveThemeTransparency();
    std::wstring text;
    for (auto& [key, o] : g_cfgOverrides) text += key + L"\t" + o.value + L"\t" + o.base + L"\n";
    Wh_SetStringValue(kCfgOverridesKey, text.c_str());
    g_cfgOverridesDirty = false;
}

std::wstring GetStringSetting(PCWSTR name);
std::wstring CfgWindhawkValue(const CfgItem& it) {
    return CfgIsInt(it) ? std::to_wstring(Wh_GetIntSetting(it.key)) : GetStringSetting(it.key);
}

// The value changed in the panel, if Windhawk still has the value it replaced
bool CfgOverrideFor(PCWSTR key, std::wstring& out) {
    if (!g_cfgOverridesLoaded) LoadCfgOverrides();
    auto found = g_cfgOverrides.find(key);
    if (found == g_cfgOverrides.end()) return false;
    const CfgItem* it = CfgFind(key);
    if (!it || CfgWindhawkValue(*it) != found->second.base) { g_cfgOverrides.erase(found); g_cfgOverridesDirty = true; return false; }
    out = found->second.value;
    return true;
}
int CfgInt(PCWSTR key) { std::wstring v; return CfgOverrideFor(key, v) ? _wtoi(v.c_str()) : Wh_GetIntSetting(key); }
std::wstring CfgStr(PCWSTR key) { std::wstring v; return CfgOverrideFor(key, v) ? v : GetStringSetting(key); }

// Panel state
struct CfgState {
    bool active = false, closing = false, pendingReload = false;
    bool powerButton = false;        // the floating power button as it was when the settings opened (kept until the next opening)
    float endT = 0;                          // Siri modes: after the bar is home, 0 -> 1 while it shrinks into its gear and is sucked in
    D2D1_RECT_F closeHome{};                 // Window / Fullscreen: where the bar is while the settings close (it stays put)
    float anim = 0;                  // 0 = the bar at home ... 1 = settings fully open
    int section = 0, hover = -1, sideHover = -1;
    bool doneHover = false, gearHover = false;
    float scroll = 0, contentH = 0;
    int drag = -1;                   // slider being dragged (item index)
    int edit = -1;                   // color code being typed
    std::wstring editText;
    int capture = -1;                // shortcut waiting for its keys
    std::wstring search;
    D2D1_RECT_F home{}, bar{}, side{}, panel{}, done{};
    RECT homeArea{};
    double lastApply = 0;
    bool siriMode = true, siriOriginal = true, fullscreen = false;   // the launcher's mode when the settings opened
    D2D1_RECT_F window{};   // Window / Fullscreen: the window's shape (it folds into the search field)
    int homeExtra = 0;
};
CfgState g_cfg;
int g_cfgLastSection = 0;
struct GearState { bool shown = false, hover = false; float anim = 0; };
GearState g_gear;
[[clang::no_destroy]] std::map<std::wstring, std::wstring> g_cfgValues;   // what the panel shows

void CfgRefreshValues() {
    g_cfgValues.clear();
    for (const CfgItem& it : kCfgItems) {
        if (it.type == kCfgButton) continue;
        g_cfgValues[it.key] = CfgIsInt(it) ? std::to_wstring(CfgInt(it.key)) : CfgStr(it.key);
    }
    g_cfgValues[L"appearance.transparency"] = std::to_wstring(g_settings.glassTransparency);   // the active theme's own
    if (g_cfgOverridesDirty) SaveCfgOverrides();
}

constexpr int kMenuCfgBase = 20000;   // + item * 32 + option
void CfgChoose(int code);
void CfgApplyLayout();
struct GlassPiece;
void CfgCollectGlass(std::vector<GlassPiece>& out);
void DrawCfgBehind();
void CfgEnter();
bool CfgMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& result);
void DrawCfg();
void DrawGear();
bool CfgStep(float dt);
void CfgAbort();
D2D1_RECT_F GearRect();
bool HitMore(float x, float y);
extern bool g_moreHover;
void DrawMoreButton();
void TrackLeave(HWND hwnd);
// Icons from GitHub (defined further down)
struct App;
std::wstring g_ghSetting;   // the repository from the settings
void GhItems(std::vector<std::shared_ptr<App>>& out);
void GhFilterItems(std::vector<int>& out, const std::wstring& q);
PCWSTR GhMessage();
void GhQueueVisible();
void GhClose(bool render);
void GhOpen(App* app);
void GhApply(const std::wstring& path);
struct GhListResult; struct GhIconResult;
void OnGhList(GhListResult* result);
void OnGhIcon(GhIconResult* result);
void GhStopThreads();
// The Siri bar can be held and dragged anywhere; its place is kept (per mille of the work area, -1 = centered)
int g_barPosX = -1, g_barPosY = -1;
bool g_siriFlip;        // the bar sits low on the screen: the results box opens above it
float g_siriShiftX;     // the bar near a side of the screen: how far it is from the middle of the window
struct BarDrag { bool pending = false, active = false; POINT start{}, grab{}; RECT startArea{}; };
BarDrag g_barDrag;
constexpr UINT_PTR kTimerBarHold = 11;
float CfgWindowT();
D2D1_RECT_F CfgWindowRect();
float CfgWindowRadius();
void DrawCfgWindow();
void DrawCfgSearchField();
float CfgCloseU();
void DrawCfgClosing();

std::vector<std::wstring> Split(const std::wstring& s, WCHAR sep);
int g_autoDarkText = -1;   // automatic text color on clear glass: 1 black, 0 white (read from the background when opening)
// What every setting is when the mod is installed (the defaults of the Windhawk settings above)
struct CfgDefault { PCWSTR key, value; };
const CfgDefault kCfgDefaults[] = {
    {L"displayMode", L"siriOriginal"},
    {L"theme", L"glass"},
    {L"language", L"auto"},
    {L"appearance.transparency", L"80"},
    {L"appearance.cornerRadius", L"26"},
    {L"appearance.blur", L"60"},
    {L"appearance.menuBlur", L"100"},
    {L"appearance.menuTextColor", L"auto"},
    {L"appearance.glassStyle", L"frosted"},
    {L"appearance.refraction", L"60"},
    {L"appearance.glassEffect", L"1"},
    {L"appearance.liveGlass", L"0"},
    {L"custom.color", L"#1e1e1e"},
    {L"custom.opacity", L"60"},
    {L"custom.shading", L"70"},
    {L"custom.borderWidth", L"1"},
    {L"custom.borderColor", L"#ffffff"},
    {L"custom.borderOpacity", L"18"},
    {L"siri.pillWidth", L"560"},
    {L"siri.pillHeight", L"56"},
    {L"siri.panelWidth", L"680"},
    {L"siri.panelRows", L"3"},
    {L"siri.filesWidth", L"0"},
    {L"siri.filesRows", L"3"},
    {L"siri.actionsWidth", L"680"},
    {L"siri.actionsRows", L"3"},
    {L"siri.clipboardWidth", L"0"},
    {L"siri.clipboardRows", L"3"},
    {L"siri.firstRow", L"recent"},
    {L"siri.powerPill", L"1"},
    {L"siri.powerButton", L"1"},
    {L"siri.orbStyle", L"wave"},
    {L"siri.circleZoom", L"25"},
    {L"siri.bounce", L"50"},
    {L"siri.panelBounce", L"30"},
    {L"siri.introMs", L"520"},
    {L"siri.outroMs", L"420"},
    {L"siri.animationSpeed", L"100"},
    {L"siri.shortcutApps", L"Ctrl+1"},
    {L"siri.shortcutFiles", L"Ctrl+2"},
    {L"siri.shortcutActions", L"Ctrl+3"},
    {L"siri.shortcutClipboard", L"Ctrl+4"},
    {L"siri.shortcutsAnywhere", L"1"},
    {L"window.width", L"80"},
    {L"window.height", L"75"},
    {L"window.searchStyle", L"bar"},
    {L"window.showPowerButton", L"1"},
    {L"window.powerButtonPosition", L"left"},
    {L"window.openAnimation", L"zoom"},
    {L"window.closeAnimation", L"zoom"},
    {L"window.animationSpeed", L"100"},
    {L"window.folderBlur", L"40"},
    {L"open.winKey", L"1"},
    {L"open.startButton", L"1"},
    {L"grid.iconSize", L"64"},
    {L"grid.columns", L"0"},
    {L"grid.pageMode", L"vertical"},
    {L"grid.jiggle", L"1"},
    {L"grid.githubIcons", L""},
    {L"grid.dragToPin", L"1"},
    {L"files.enabled", L"1"},
    {L"files.history", L"1"},
    {L"files.view", L"list"},
    {L"files.indexer", L"auto"},
    {L"files.maxResults", L"50"},
    {L"clipboard.enabled", L"1"},
    {L"clipboard.size", L"30"},
    {L"clipboard.keep", L"always"},
};

// Glass transparency is kept per theme: Light and OLED start solid (0), the others from the Windhawk setting
constexpr WCHAR kThemeTransparencyKey[] = L"themeTransparency";
[[clang::no_destroy]] std::map<std::wstring, int> g_themeTransparency;
bool g_themeTransparencyLoaded;
PCWSTR ThemeName(Theme t) {
    switch (t) {
        case Theme::Light: return L"light";
        case Theme::Siri: return L"siri";
        case Theme::Dark: return L"dark";
        case Theme::Oled: return L"oled";
        case Theme::Custom: return L"custom";
        default: return L"glass";
    }
}
void LoadThemeTransparency() {
    g_themeTransparencyLoaded = true;
    g_themeTransparency.clear();
    WCHAR buf[512] = {};
    Wh_GetStringValue(kThemeTransparencyKey, buf, ARRAYSIZE(buf));
    for (const auto& line : Split(buf, L'\n')) {
        auto f = Split(line, L'\t');
        if (f.size() == 2 && !f[0].empty()) g_themeTransparency[f[0]] = std::clamp(_wtoi(f[1].c_str()), 0, 100);
    }
}
void SaveThemeTransparency() {
    std::wstring text;
    for (auto& [theme, v] : g_themeTransparency) text += theme + L"\t" + std::to_wstring(v) + L"\n";
    Wh_SetStringValue(kThemeTransparencyKey, text.c_str());
}
int ThemeTransparency(Theme t, int fromWindhawk) {
    if (!g_themeTransparencyLoaded) LoadThemeTransparency();
    auto found = g_themeTransparency.find(ThemeName(t));
    if (found != g_themeTransparency.end()) return found->second;
    // Light and OLED start solid, unless the Windhawk setting was moved from its default (80): then it counts
    return (t == Theme::Light || t == Theme::Oled) && fromWindhawk == 80 ? 0 : fromWindhawk;
}

void LoadSettings() {
    Settings s;
    auto I = [](PCWSTR k) { return CfgInt(k); };   // the settings panel can override Windhawk
    auto S = [](PCWSTR k) { return CfgStr(k); };

    // Language: the one chosen, or the language of Windows
    {
        std::wstring lang = S(L"language");
        g_uiLanguage = lang == L"pt" ? UiLanguage::Portuguese : lang == L"en" ? UiLanguage::English : lang == L"es" ? UiLanguage::Spanish
                     : lang == L"fr" ? UiLanguage::French : lang == L"de" ? UiLanguage::German : lang == L"it" ? UiLanguage::Italian
                     : DetectUiLanguage();
    }

    // ① Mode
    std::wstring mode = S(L"displayMode");

    s.fullscreen = mode == L"fullscreen";
    // Original Siri is the main mode (also used when the setting is missing or unknown)
    s.siriOriginal = mode != L"window" && mode != L"fullscreen" && mode != L"siri";
    s.siri = mode == L"siri" || s.siriOriginal;

    // ② Theme (one for every mode)
    std::wstring theme = S(L"theme");
    s.theme = theme == L"light" ? Theme::Light : theme == L"siri" ? Theme::Siri : theme == L"custom" ? Theme::Custom
            : theme == L"dark" ? Theme::Dark : theme == L"oled" ? Theme::Oled : Theme::Glass;

    // ③ Appearance
    s.glassTransparency = ThemeTransparency(s.theme, std::clamp(I(L"appearance.transparency"), 0, 100));
    s.cornerRadius = std::clamp(I(L"appearance.cornerRadius"), 0, 60);
    s.blurAmount = std::clamp(I(L"appearance.blur"), 0, 100);
    s.menuBlur = std::clamp(I(L"appearance.menuBlur"), 0, 200);
    {
        std::wstring c = S(L"appearance.menuTextColor");
        s.menuText = c == L"white" ? 0 : c == L"black" ? 1 : -1;
    }
    s.liveBackground = s.blurAmount == 0;
    s.glassLiquid = S(L"appearance.glassStyle") == L"liquid";
    s.refraction = std::clamp(I(L"appearance.refraction"), 0, 100);
    s.liveGlass = I(L"appearance.liveGlass") != 0;
    s.glassEffect = I(L"appearance.glassEffect") != 0;
    s.folderRadius = s.siriPanelRadius = s.cornerRadius;

    // ④ Custom theme: one color/opacity for the window, the Siri bar and the Siri panel
    s.tint = ParseColor(S(L"custom.color"), s.tint);
    s.siriPillColor = s.siriPanelColor = s.tint;
    s.opacity = s.siriPanelOpacity = std::clamp(I(L"custom.opacity"), 0, 100) / 100.0f;
    s.siriPillOpacity = std::min(1.0f, s.opacity + 0.12f);
    s.siriShadeStrength = std::clamp(I(L"custom.shading"), 0, 100);
    s.siriShade = s.siriShadeStrength > 0;
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
    auto width = [&](PCWSTR k) { int v = I(k); return v <= 0 ? 0 : std::clamp(v, 300, 1400); };
    s.filesPanelW = width(L"siri.filesWidth"); s.filesRows = std::clamp(I(L"siri.filesRows"), 1, 8);
    s.actionsPanelW = width(L"siri.actionsWidth"); s.actionsRows = std::clamp(I(L"siri.actionsRows"), 1, 8);
    s.clipPanelW = width(L"siri.clipboardWidth"); s.clipRows = std::clamp(I(L"siri.clipboardRows"), 1, 8);
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
    {
        std::wstring keep = S(L"clipboard.keep");
        s.clipKeepSec = keep == L"15m" ? 15 * 60 : keep == L"1h" ? 3600 : keep == L"8h" ? 8 * 3600 : keep == L"24h" ? 24 * 3600
                      : keep == L"7d" ? 7 * 24 * 3600 : 0;
    }

    Theme active = s.theme;
    s.darkText = active == Theme::Light ? true : active != Theme::Custom ? false : Luma(s.siri ? s.siriPanelColor : s.tint) > 0.6f;
    // Text color chosen in the settings (every theme but Custom); automatic: clear glass follows the background
    if (active != Theme::Custom) {
        if (s.menuText >= 0) s.darkText = s.menuText == 1;
        else if (active == Theme::Glass && g_autoDarkText >= 0) s.darkText = g_autoDarkText == 1;
    }
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
    g_shortcutsAnywhere = I(L"siri.shortcutsAnywhere") != 0;
    g_ghSetting = S(L"grid.githubIcons");
    g_barPosX = Wh_GetIntValue(L"barX", -1);
    g_barPosY = Wh_GetIntValue(L"barY", -1);
    if (!g_historyEnabled) { g_searchHistory.clear(); SaveSearchHistory(); }
    if (g_cfgOverridesDirty) SaveCfgOverrides();
}


////////////////////////////////////////////////////////////////////////////////
// App list & Folders

struct App {
    int kind = 0;               // 0 app, 1 file, 2 clipboard text, 3 search history, 4 action
    std::wstring name, target;  // shown name; what opens it (or the text / the query / the action)
    // Icon: square premultiplied BGRA pixels, and the D2D bitmap made from them for the current render target
    int iconSize = 0;
    std::vector<UINT32> pixels;
    UINT generation = 0;
    ComPtr<ID2D1Bitmap> bitmap;
    // The label under the icon, laid out once (laying text out again every frame slowed the scrolling)
    ComPtr<IDWriteTextLayout> labelLayout;
    IDWriteTextFormat* labelFormat = nullptr;
    float labelW = 0, labelH = 0;
    std::wstring labelText;
};

struct Folder {
    std::wstring name = L"Folder";
    std::wstring sortKey;
    std::vector<std::shared_ptr<App>> apps;
};

using AppList = std::vector<std::shared_ptr<App>>;
[[clang::no_destroy]] AppList g_apps;
[[clang::no_destroy]] AppList g_tabItems;
std::vector<std::wstring> g_clipHistory;
constexpr WCHAR kHistoryKey[] = L"fileHistory";
constexpr int kHistoryMax = 20;
[[clang::no_destroy]] std::vector<Folder> g_folders;
std::vector<int> g_layoutItems;

std::atomic<HWND> g_wnd;               // the launcher window (also read by the hook and the worker threads)
// The app list is read on a worker thread
HANDLE g_worker;
std::atomic<bool> g_workerStop;
bool g_appsLoaded;
int g_appsIconPixels;                   // icon size the current list was made for
double g_appsTime;                      // when it was last read

// Icons come as 32-bit pixels, some with no alpha at all and some with straight alpha: they end up premultiplied.
// An icon drawn small in the middle of a big transparent canvas is cut around what is visible, so all icons
// look about the same size in the grid.
void FinishIcon(std::vector<UINT32> pixels, int width, int height, App& app) {
    bool hasAlpha = false, straight = false;
    for (UINT32 px : pixels) {
        UINT a = px >> 24, brightest = std::max({(px >> 16) & 0xFFu, (px >> 8) & 0xFFu, px & 0xFFu});
        hasAlpha = hasAlpha || a != 0;
        straight = straight || brightest > a;
    }
    if (!hasAlpha) {
        for (UINT32& px : pixels) px |= 0xFF000000u;
        straight = false;
    }
    if (straight) {
        for (UINT32& px : pixels) {
            UINT a = px >> 24;
            auto scale = [a](UINT v) { return (v * a + 127) / 255; };
            px = (a << 24) | (scale((px >> 16) & 0xFF) << 16) | (scale((px >> 8) & 0xFF) << 8) | scale(px & 0xFF);
        }
    }
    // Box around the visible pixels, one row at a time
    int minX = width, maxX = -1, minY = height, maxY = -1;
    for (int y = 0; y < height; y++) {
        const UINT32* row = pixels.data() + (size_t)y * width;
        int first = -1, last = -1;
        for (int x = 0; x < width; x++)
            if ((row[x] >> 24) > 8) { if (first < 0) first = x; last = x; }
        if (first < 0) continue;
        minX = std::min(minX, first); maxX = std::max(maxX, last);
        if (minY > y) minY = y;
        maxY = y;
    }
    if (maxX < 0) return;   // fully transparent
    int canvas = std::max(width, height), visible = std::max(maxX - minX + 1, maxY - minY + 1);
    bool drawnSmall = visible * 10 < canvas * 6;   // uses less than 60 % of its canvas
    int side = drawnSmall ? std::min(canvas, (int)std::ceil(visible * 1.08f)) : canvas;
    int centerX = drawnSmall ? (minX + maxX + 1) / 2 : width / 2, centerY = drawnSmall ? (minY + maxY + 1) / 2 : height / 2;
    int fromX = centerX - side / 2, fromY = centerY - side / 2;
    int colStart = std::max(0, -fromX), colEnd = std::min(side, width - fromX);
    std::vector<UINT32> square((size_t)side * side, 0);
    for (int y = 0; y < side && colEnd > colStart; y++) {
        int sy = fromY + y;
        if (sy < 0 || sy >= height) continue;
        std::copy_n(pixels.data() + (size_t)sy * width + fromX + colStart, colEnd - colStart, square.data() + (size_t)y * side + colStart);
    }
    app.iconSize = side;
    app.pixels = std::move(square);
}

// 32-bit top-down copy of a GDI bitmap
bool BitmapToPixels(HBITMAP bitmap, std::vector<UINT32>& pixels, int& width, int& height) {
    BITMAP info{};
    if (!GetObjectW(bitmap, sizeof(info), &info) || info.bmWidth <= 0 || info.bmHeight == 0) return false;
    width = info.bmWidth;
    height = std::abs(info.bmHeight);
    BITMAPINFO request{};
    request.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
    pixels.resize((size_t)width * height);
    HDC screen = GetDC(nullptr);
    int rows = GetDIBits(screen, bitmap, 0, (UINT)height, pixels.data(), &request, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    return rows > 0;
}

void LoadAppIcon(IShellItem* item, int size, App& app) {
    ComPtr<IShellItemImageFactory> images;
    HBITMAP bitmap = nullptr;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(&images))) ||
        FAILED(images->GetImage(SIZE{size, size}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bitmap)) || !bitmap)
        return;
    std::vector<UINT32> pixels;
    int width = 0, height = 0;
    if (BitmapToPixels(bitmap, pixels, width, height)) FinishIcon(std::move(pixels), width, height, app);
    DeleteObject(bitmap);
}

// Uninstallers, web links and documents also live in the Start menu folders, but they are not apps
bool ShouldSkip(const std::wstring& name, const std::wstring& parsing) {
    if (name.empty()) return true;
    static const PCWSTR kNotApps[] = {L"\\unins0", L"\\uninst.exe", L"\\uninstall.exe"};
    static const PCWSTR kDocuments[] = {L".chm", L".txt", L".pdf", L".rtf", L".htm", L".html"};
    const std::wstring target = ToLower(parsing);
    if (target.compare(0, 4, L"http") == 0) return true;
    for (PCWSTR part : kNotApps)
        if (target.find(part) != std::wstring::npos) return true;
    for (PCWSTR ext : kDocuments) {
        size_t n = wcslen(ext);
        if (target.size() >= n && std::equal(target.end() - (ptrdiff_t)n, target.end(), ext)) return true;
    }
    return false;
}

bool NameLess(const std::wstring& a, const std::wstring& b) { return StrCmpLogicalW(a.c_str(), b.c_str()) < 0; }   // "App 2" before "App 10"
bool AppNameLess(const std::shared_ptr<App>& a, const std::shared_ptr<App>& b) { return NameLess(a->name, b->name); }

// Every entry of the Apps folder (the list the Start menu shows), with its icon, in name order
AppList* EnumerateApps(int iconPixels) {
    struct TaskString { PWSTR text = nullptr; ~TaskString() { CoTaskMemFree(text); } };
    auto* found = new AppList();
    ComPtr<IShellItem> appsFolder;
    ComPtr<IEnumShellItems> entries;
    if (FAILED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&appsFolder))) ||
        FAILED(appsFolder->BindToHandler(nullptr, kBhidEnumItems, IID_PPV_ARGS(&entries))))
        return found;
    IShellItem* batch[16];
    ULONG count = 0;
    while (!g_workerStop && SUCCEEDED(entries->Next(ARRAYSIZE(batch), batch, &count)) && count > 0) {
        for (ULONG i = 0; i < count; i++) {
            ComPtr<IShellItem> entry;
            entry.Attach(batch[i]);
            TaskString display, id;
            if (g_workerStop || FAILED(entry->GetDisplayName(SIGDN_NORMALDISPLAY, &display.text)) ||
                FAILED(entry->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &id.text)) || ShouldSkip(display.text, id.text))
                continue;
            auto app = std::make_shared<App>();
            app->name = display.text;
            app->target = kAppsFolder + std::wstring(id.text);
            LoadAppIcon(entry.Get(), iconPixels, *app);
            found->push_back(std::move(app));
        }
    }
    std::stable_sort(found->begin(), found->end(), AppNameLess);
    return found;
}

struct AppsJob { int iconPixels; std::unordered_map<std::wstring, std::wstring> customIcons; };
bool LoadImageFile(const std::wstring& path, int size, App& app);
extern std::unordered_map<std::wstring, std::wstring> g_customIcons;

DWORD WINAPI LoadAppsThread(void* param) {
    std::unique_ptr<AppsJob> job(static_cast<AppsJob*>(param));
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    std::unique_ptr<AppList> apps(EnumerateApps(job->iconPixels));
    // Custom icons are decoded here too, so the launcher never waits for image files
    for (const auto& app : *apps) {
        if (g_workerStop) break;
        auto custom = job->customIcons.find(app->target);
        if (custom != job->customIcons.end()) LoadImageFile(custom->second, job->iconPixels, *app);
    }
    if (SUCCEEDED(com)) CoUninitialize();
    HWND launcher = g_wnd;
    if (!g_workerStop && launcher && PostMessageW(launcher, WM_APP_APPS, job->iconPixels, (LPARAM)apps.get())) {
        auto* owned = apps.release();   // now owned by the window
        (void)owned;
    }
    return 0;
}

BOOL CALLBACK HighestDpi(HMONITOR monitor, HDC, LPRECT, LPARAM data) {
    UINT x = 0, y = 0;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &x, &y))) {
        UINT& best = *reinterpret_cast<UINT*>(data);
        best = std::max(best, x);
    }
    return TRUE;
}
// Icons are made for the sharpest monitor, so they stay crisp on all of them
int IconPixels() {
    UINT dpi = 96;
    EnumDisplayMonitors(nullptr, nullptr, HighestDpi, (LPARAM)&dpi);
    return std::min(256, (int)std::ceil(g_settings.iconSize * (dpi / 96.0)));
}

bool WorkerBusy() { return g_worker && WaitForSingleObject(g_worker, 0) == WAIT_TIMEOUT; }
void StartLoadingApps() {
    if (WorkerBusy()) return;   // a list is already on its way
    if (g_worker) CloseHandle(std::exchange(g_worker, nullptr));
    auto job = std::make_unique<AppsJob>(AppsJob{IconPixels(), g_customIcons});
    g_appsTime = Now();
    g_worker = CreateThread(nullptr, 0, LoadAppsThread, job.get(), 0, nullptr);
    if (g_worker) {
        auto* owned = job.release();   // the thread owns it now
        (void)owned;
    }
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
std::unordered_map<std::wstring, float> g_chipWidths;   // measured filter labels (cleared when the scale changes)
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
bool IsFolderItem(int it) { return it >= kFolderBase && it < kFolderAppBase; }

App* ResolveApp(int it) {
    if (it < 0) return nullptr;
    if (IsTabItem(it)) { int i = it - kTabBase; return i < (int)g_tabItems.size() ? g_tabItems[i].get() : nullptr; }
    if (it < kFolderBase) return it < (int)g_apps.size() ? g_apps[it].get() : nullptr;
    if (!IsFolderApp(it)) return nullptr;
    int f = (it - kFolderAppBase) / 10000, a = (it - kFolderAppBase) % 10000;
    if (f >= (int)g_folders.size() || a >= (int)g_folders[f].apps.size()) return nullptr;
    return g_folders[f].apps[a].get();
}

// ---------- Original Siri: most used apps and app categories ----------
std::vector<std::wstring> Split(const std::wstring& s, WCHAR sep);
int g_recommendedCount = 7;   // apps per row in the Siri panel (set by the layout)
constexpr WCHAR kLaunchKey[] = L"launchCounts";
std::unordered_map<std::wstring, int> g_launchCounts;   // launches through the mod, by lowercase target
std::unordered_map<std::wstring, int> g_userAssist;     // Windows' own run counts (UserAssist)
std::unordered_map<std::wstring, unsigned long long> g_userAssistTime;   // ... and when they last ran (FILETIME)
std::unordered_map<std::wstring, unsigned long long> g_launchTimes;      // last launch through the mod
std::unordered_map<std::wstring, unsigned long long> g_firstSeen;         // when an app first showed up (installed)
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
std::vector<CustomTab> g_customTabs;
constexpr int kMaxCustomTabs = 6;
constexpr int kChipAll = -1, kChipCustom = 100, kChipCancel = -100, kChipDone = -101, kNoChip = -1000;
std::unordered_set<App*> g_selection;   // multi-select (Original Siri: the apps picked for a tab)
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

// Whole words only (a plural "s" is fine); a keyword ending in '*' is a stem ("translat*" matches "Translator")
bool HasWord(const std::wstring& text, const std::wstring& keyword) {
    bool stem = !keyword.empty() && keyword.back() == L'*';
    std::wstring word = stem ? keyword.substr(0, keyword.size() - 1) : keyword;
    for (size_t at = text.find(word); at != std::wstring::npos; at = text.find(word, at + 1)) {
        if (at > 0 && iswalnum(text[at - 1])) continue;
        size_t end = at + word.size();
        if (end < text.size() && text[end] == L's' && !stem) end++;
        if (stem || end >= text.size() || !iswalnum(text[end])) return true;
    }
    return false;
}

int AppCategory(const App& a) {
    static const std::vector<std::pair<int, std::vector<std::wstring>>> kRules = {
        {3, {L"whatsapp", L"telegram", L"discord", L"instagram", L"facebook", L"messenger", L"twitter", L"tiktok", L"linkedin",
             L"skype", L"signal", L"reddit", L"snapchat", L"threads", L"wechat", L"teams"}},
        {2, {L"spotify", L"netflix", L"prime video", L"disney", L"hbo", L"youtube", L"twitch", L"steam", L"epic games", L"game*", L"jogo*",
             L"xbox", L"playstation", L"bluestacks", L"emulat*", L"vlc", L"media player", L"music*", L"música", L"movies", L"filmes",
             L"kodi", L"plex", L"deezer", L"roblox", L"minecraft", L"league of", L"valorant", L"ubisoft", L"ea app", L"battle.net",
             L"riot", L"gog", L"tv", L"death stranding", L"anchor"}},
        {4, {L"photoshop", L"illustrator", L"premiere", L"after effects", L"lightroom", L"indesign", L"figma", L"canva", L"blender",
             L"gimp", L"inkscape", L"krita", L"davinci", L"capcut", L"obs", L"audacity", L"fl studio", L"ableton", L"photos", L"fotos",
             L"clipchamp", L"paint 3d", L"sketch", L"autocad", L"unity", L"unreal", L"cinema 4d", L"affinity", L"audition", L"camera", L"câmera"}},
        {5, {L"edge", L"chrome", L"firefox", L"opera", L"brave", L"browser", L"navegador", L"kindle", L"reader", L"leitor", L"news",
             L"notícias", L"weather", L"clima", L"maps", L"mapas", L"wikipedia", L"books", L"livros", L"bible", L"bíblia",
             L"dictionary", L"dicionário", L"translat*", L"tradutor", L"chatgpt", L"copilot", L"gemini", L"claude", L"perplexity"}},
        {1, {L"word", L"excel", L"powerpoint", L"outlook", L"onenote", L"access", L"publisher", L"office", L"acrobat", L"pdf",
             L"notion", L"to do", L"todo", L"calendar", L"calendário", L"mail", L"zoom", L"slack", L"trello", L"evernote", L"bank*",
             L"banco", L"financ*", L"money", L"project", L"visio", L"code", L"studio", L"intellij", L"pycharm", L"git*", L"docker",
             L"sql", L"python", L"java", L"node", L"clínica", L"clinic", L"agenda"}},
        {0, {L"calc*", L"notepad", L"bloco de notas", L"settings", L"configura*", L"terminal", L"powershell", L"cmd", L"prompt",
             L"explorer", L"explorador", L"task manager", L"gerenciador", L"control", L"painel", L"paint", L"snipping", L"captura",
             L"recorte", L"clock", L"relógio", L"alarm", L"7-zip", L"winrar", L"everything", L"windhawk", L"defrag*", L"desfragment*",
             L"backup", L"cleanup", L"limpeza", L"disk", L"disco", L"device", L"dispositivo", L"printer", L"impressora", L"remote",
             L"remota", L"anydesk", L"teamviewer", L"vpn", L"bluetooth", L"store", L"loja", L"diagnós*", L"diagnos*", L"calibr*",
             L"altsnap", L"utilit*", L"tools", L"ferrament*", L"assistência", L"assistance", L"dolby", L"driver", L"update"}},
    };
    std::wstring text = ToLower(a.name);
    for (const auto& [category, words] : kRules)
        for (const auto& word : words)
            if (HasWord(text, word)) return category;
    return 6;
}

// Built-in tabs can be deleted (hidden) and the user can add or take apps out of them
struct BuiltinTab { bool hidden = false; std::vector<std::wstring> added, removed; };
BuiltinTab g_builtinTabs[kFilterCount];

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
    if (g_siriTab == 4) { GhFilterItems(out, q); return; }   // GitHub gallery: name, color and order
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
            for (int i = 0; i < (int)g_apps.size() && i < kFolderBase; i++) {
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
        for (int i = 0; i < (int)g_apps.size() && i < kFolderBase; i++)
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
            out.push_back(kFolderBase + folderOrder[next++]);
        }
        out.push_back(i);
    }
    while (next < folderOrder.size()) out.push_back(kFolderBase + folderOrder[next++]);
}

void UndoFolder(int index) {
    if (index < 0 || index >= (int)g_folders.size()) return;
    for (const auto& app : g_folders[index].apps) g_apps.push_back(app);
    g_folders.erase(g_folders.begin() + index);
    std::stable_sort(g_apps.begin(), g_apps.end(), AppNameLess);
}

////////////////////////////////////////////////////////////////////////////////
// Clipboard helpers

bool ExcludedFromHistory();

// forHistory: items marked as secret (passwords...) are not read for the clipboard history
int g_clipReadResult;   // last history read: 0 ok, 1 clipboard busy, 2 marked as secret, 3 no text
std::wstring ReadClipboardText(size_t maxLen, bool forHistory = false) {
    std::wstring text;
    bool open = false;
    for (int i = 0; i < 5 && !(open = OpenClipboard(g_wnd) != 0); i++) Sleep(10);
    g_clipReadResult = open ? 3 : 1;
    if (!open) return text;
    if (forHistory && ExcludedFromHistory()) { CloseClipboard(); g_clipReadResult = 2; return text; }
    if (HANDLE data = GetClipboardData(CF_UNICODETEXT)) {
        if (auto* p = (PCWSTR)GlobalLock(data)) { text = p; GlobalUnlock(data); g_clipReadResult = 0; }
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

std::unordered_map<std::wstring, std::wstring> g_customIcons;
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
std::vector<HANDLE> g_fileThreadHandles;   // file search threads (UI thread only), joined at teardown
[[clang::no_destroy]] AppList g_fileResults;
std::wstring g_fileStatus;
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
        if (RegisterClassExW(&wc)) g_evClassRegistered = true;
        else Wh_Log(L"Everything: RegisterClassEx failed (%u)", GetLastError());
    }
    ReleaseSRWLockExclusive(&g_evClassLock);
    return g_evClassRegistered;
}

void UnregisterEvIpcClass() {   // called from StartThread's teardown, after the file threads are joined
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
std::unordered_map<std::wstring, CachedIcon> g_extIcons;
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
    if (!Stale(job->gen) && IsWindow(job->wnd) && PostMessageW(job->wnd, WM_APP_FILES, 0, (LPARAM)res.get())) {
        auto* owned = res.release();   // now owned by the window
        (void)owned;
    }
    return 0;
}

// Called on the UI thread (debounced by kTimerFileSearch)
void StartFileSearch(const std::wstring& q) {
    UINT gen = ++g_fileSearchGen; // invalidates every previous search
    g_fileResults.clear(); g_fileStatus.clear();
    g_fileSearching = g_settings.filesEnabled && q.size() >= 2;
    if (!g_fileSearching) return;
    auto* job = new FileJob{q, g_settings.indexer, g_settings.maxResults, CurrentIconPixels(), gen, g_wnd};
    if (HANDLE t = CreateThread(nullptr, 0, FileSearchThread, job, 0, nullptr)) {
        // Forget the searches that already finished; the running ones are waited for at teardown
        std::erase_if(g_fileThreadHandles, [](HANDLE h) {
            if (WaitForSingleObject(h, 0) != WAIT_OBJECT_0) return false;
            CloseHandle(h);
            return true;
        });
        g_fileThreadHandles.push_back(t);
    } else { delete job; g_fileSearching = false; }
}

void CancelFileSearch() {
    ++g_fileSearchGen;
    g_fileSearching = false;
    g_fileResults.clear(); g_fileStatus.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Categories: search history, indexed files & clipboard

bool PruneClipHistory();
void LoadTabSource() {
    g_tabItems.clear();
    if (g_siriTab == 1 && g_settings.filesEnabled) {
        if (g_query.empty()) {          // empty search: only the search history
            if (g_historyEnabled) {
                for (const auto& q : g_searchHistory) {
                    auto app = std::make_shared<App>();
                    app->kind = 3; app->name = q; app->target = q;
                    g_tabItems.push_back(std::move(app));
                }
            }
        } else {                        // typing: the indexer results
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
    } else if (g_siriTab == 4) {
        GhItems(g_tabItems);   // the GitHub gallery
    } else if (g_siriTab == 2 && g_settings.clipEnabled) {
        PruneClipHistory();
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

[[clang::no_destroy]] ComPtr<IDWriteFactory> g_dwrite;
[[clang::no_destroy]] ComPtr<ID2D1Factory> g_d2d;
// Everything is drawn by one DC render target into a DIB, then shown with UpdateLayeredWindow
[[clang::no_destroy]] ComPtr<ID2D1DCRenderTarget> g_target;
[[clang::no_destroy]] ComPtr<ID2D1SolidColorBrush> g_brush;   // recolored for each fill
UINT g_targetGeneration = 1;   // bumped when the target is recreated (bitmaps must be remade)
[[clang::no_destroy]] ComPtr<ID2D1Bitmap1> g_layerBitmap;
UINT g_layerGeneration;

struct Surface { int width = 0, height = 0; HDC dc = nullptr; HGDIOBJ old = nullptr; HBITMAP bitmap = nullptr; };

// A top-down 32-bit DIB section selected into `dc`
HBITMAP NewDib(HDC dc, int width, int height, void** bits) {
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB, 0, 0, 0, 0, 0};
    return dc ? CreateDIBSection(dc, &info, DIB_RGB_COLORS, bits, nullptr, 0) : nullptr;
}
Surface g_surface;

void FreeSurface(Surface& s) {
    if (!s.dc) { s = Surface{}; return; }
    SelectObject(s.dc, s.old);
    if (s.bitmap) DeleteObject(s.bitmap);
    DeleteDC(s.dc);
    s = Surface{};
}

// The 32-bit DIB the frame is drawn into, kept while the size stays the same
// The frame buffer, kept while the window keeps its size
bool EnsureSurface(Surface& s, int width, int height) {
    if (s.dc != nullptr && width == s.width && height == s.height) return true;
    FreeSurface(s);
    HDC dc = CreateCompatibleDC(nullptr);
    void* bits = nullptr;
    HBITMAP bitmap = NewDib(dc, width, height, &bits);
    if (!bitmap) {
        if (dc) DeleteDC(dc);
        return false;
    }
    s.width = width; s.height = height; s.dc = dc; s.bitmap = bitmap; s.old = SelectObject(dc, bitmap);
    return true;
}

bool EnsureTarget() {
    if (g_target) return true;
    D2D1_RENDER_TARGET_PROPERTIES props{};
    props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
    props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
    props.dpiX = props.dpiY = 96;   // 1 DIP = 1 pixel: the layout does its own scaling
    ComPtr<ID2D1DCRenderTarget> target;
    if (FAILED(g_d2d->CreateDCRenderTarget(&props, &target))) return false;
    target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);   // layered window: no ClearType
    if (FAILED(target->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &g_brush))) return false;
    g_target = target;
    ++g_targetGeneration;   // bitmaps made for the previous target are rebuilt
    return true;
}

constexpr int kOrbBlobs = 5;
[[clang::no_destroy]] ComPtr<ID2D1RadialGradientBrush> g_orbBrush[kOrbBlobs];

void ResetTarget() {
    for (auto& brush : g_orbBrush) brush.Reset();
    g_layerBitmap.Reset();
    g_brush.Reset();
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

// D2D bitmap from premultiplied BGRA pixels, rebuilt when the render target was recreated
ID2D1Bitmap* CachedBitmap(ComPtr<ID2D1Bitmap>& cache, UINT& generation, const std::vector<UINT32>& pixels, int width, int height) {
    if (pixels.empty() || width <= 0 || height <= 0) return nullptr;
    if (cache && generation == g_targetGeneration) return cache.Get();
    cache.Reset();
    generation = g_targetGeneration;
    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    g_target->CreateBitmap(D2D1::SizeU(width, height), pixels.data(), width * 4, props, &cache);
    return cache.Get();
}
ID2D1Bitmap* GetBitmap(App& app) { return CachedBitmap(app.bitmap, app.generation, app.pixels, app.iconSize, app.iconSize); }

D2D1_COLOR_F Alpha(D2D1_COLOR_F c, float a) { c.a = a; return c; }

Theme ActiveTheme() { return g_settings.theme; }

// Blur behind the shapes: the Custom theme uses the General setting, the other themes have their own
// One blur level for every theme; 0 keeps the background live (nothing is captured)
extern HWND g_glassWnd;
extern bool g_glassOk;

// Blur by snapshot (a still image): only when the live blur isn't available
bool TransparencyEffectsOn();
bool g_transparencyOn = true;   // Windows' transparency effects (its blur needs them), read when opening
bool LiquidWanted();
bool LiquidActive();
int BlurStrength() { return g_settings.liveBackground || (g_glassOk && g_transparencyOn) || LiquidWanted() ? 0 : g_settings.blurAmount; }
bool LiveBlurOn() { return g_glassOk && g_transparencyOn && !g_settings.liveBackground && !LiquidActive() &&
                          (ActiveTheme() != Theme::Oled || g_settings.glassEffect); }

// Margin the window keeps around themed shapes so their shadow is not cut
float ThemeShadowMargin(float s) {
    switch (ActiveTheme()) {
        case Theme::Glass: case Theme::Light: case Theme::Siri: case Theme::Dark: case Theme::Oled: return std::round(18 * s);
        default: return 0.0f;
    }
}

// Base color for popups (context menu) that matches the theme
D2D1_COLOR_F ThemeMenuColor() {
    switch (ActiveTheme()) {
        case Theme::Light: return D2D1::ColorF(0.96f, 0.96f, 0.98f, 0.82f);
        case Theme::Glass: return D2D1::ColorF(0.10f, 0.10f, 0.12f, 0.55f);
        case Theme::Siri: return D2D1::ColorF(0.08f, 0.08f, 0.09f, 0.82f);
        case Theme::Dark: return D2D1::ColorF(0.13f, 0.13f, 0.14f, 0.86f);
        case Theme::Oled: return D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.97f);
        default: return Alpha(g_settings.siri ? g_settings.siriPanelColor : g_settings.tint, 0.70f);
    }
}
void Fill(const D2D1_RECT_F& r, D2D1_COLOR_F color, float radius = 0) {
    g_brush->SetColor(color);
    if (radius <= 0) { g_target->FillRectangle(r, g_brush.Get()); return; }
    g_target->FillRoundedRectangle(D2D1::RoundedRect(r, radius, radius), g_brush.Get());
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
    int width = 0, height = 0;               // blurred picture (smaller than the screen area)
    std::vector<UINT32> pixels;
    float sourceWidth = 0, sourceHeight = 0; // part of the picture that matches the window
    UINT generation = 0;
    ComPtr<ID2D1Bitmap> bitmap;
};
[[clang::no_destroy]] Backdrop g_backdropImage;

// One box blur pass over every row (or column) of an opaque image, using running totals per channel
void BoxBlurPass(std::vector<UINT32>& image, int width, int height, int radius, bool rows) {
    int length = rows ? width : height, lines = rows ? height : width;
    std::vector<int> total[3];
    for (auto& t : total) t.assign(length + 1, 0);
    std::vector<UINT32> line(length);
    for (int l = 0; l < lines; l++) {
        auto at = [&](int i) -> UINT32& { return rows ? image[(size_t)l * width + i] : image[(size_t)i * width + l]; };
        for (int i = 0; i < length; i++) {
            UINT32 p = at(i);
            for (int ch = 0; ch < 3; ch++) total[ch][i + 1] = total[ch][i] + (int)((p >> (8 * ch)) & 0xFF);
        }
        for (int i = 0; i < length; i++) {
            int from = std::max(0, i - radius), to = std::min(length, i + radius + 1), n = to - from;
            UINT32 out = 0xFF000000u;
            for (int ch = 0; ch < 3; ch++) out |= (UINT32)((total[ch][to] - total[ch][from]) / n) << (8 * ch);
            line[i] = out;
        }
        for (int i = 0; i < length; i++) at(i) = line[i];
    }
}

// A blurred still picture of the screen behind the launcher, used only when Windows' live blur isn't available.
// A strong blur doesn't need every pixel: GDI first shrinks the screen (averaging), then three box passes
// each way approximate a Gaussian blur.
// A blurred still picture of the screen behind the launcher, used only when Windows' live blur isn't available.
// A strong blur doesn't need every pixel: GDI first shrinks the screen (averaging), then three box passes
// each way approximate a Gaussian blur.
void CaptureBackdrop(const RECT& rc, float sigma) {
    Backdrop picture;
    const int areaW = rc.right - rc.left, areaH = rc.bottom - rc.top;
    if (areaW > 0 && areaH > 0) {
        int shrink = 1;
        if (sigma >= 3) shrink = std::min(16, std::max(1, (int)(sigma / 3)));
        int sw = std::max(1, areaW / shrink), sh = std::max(1, areaH / shrink);
        HDC screen = GetDC(nullptr);
        HDC dc = CreateCompatibleDC(screen);
        void* bits = nullptr;
        if (HBITMAP bitmap = NewDib(dc, sw, sh, &bits)) {
            HGDIOBJ old = SelectObject(dc, bitmap);
            SetStretchBltMode(dc, HALFTONE);
            SetBrushOrgEx(dc, 0, 0, nullptr);
            StretchBlt(dc, 0, 0, sw, sh, screen, rc.left, rc.top, areaW, areaH, SRCCOPY);
            GdiFlush();
            picture.pixels.assign((const UINT32*)bits, (const UINT32*)bits + (size_t)sw * sh);
            SelectObject(dc, old);
            DeleteObject(bitmap);
        }
        if (dc) DeleteDC(dc);
        ReleaseDC(nullptr, screen);
        if (!picture.pixels.empty()) {
            for (UINT32& px : picture.pixels) px |= 0xFF000000u;
            // Box width for 3 passes ~ Gaussian: w = sqrt(12 s^2 / 3 + 1)
            float s = sigma / shrink;
            int radius = (int)std::lround((std::sqrt(12.0f * s * s / 3 + 1) - 1) / 2);
            for (int pass = 0; radius > 0 && pass < 3; pass++) {
                BoxBlurPass(picture.pixels, sw, sh, radius, true);
                BoxBlurPass(picture.pixels, sw, sh, radius, false);
            }
            picture.width = sw; picture.height = sh;
            picture.sourceWidth = (float)sw; picture.sourceHeight = (float)sh;
        }
    }
    g_backdropImage = std::move(picture);
}

// The still blurred picture, stretched over the window (0, 0, width, height)
void DrawBackdropPicture(ID2D1Bitmap* picture, float width, float height) {
    D2D1_RECT_F source = D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight);
    g_target->DrawBitmap(picture, D2D1::RectF(0, 0, width, height), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, source);
}

ID2D1Bitmap* GetBackdropBitmap() {
    Backdrop& b = g_backdropImage;
    return CachedBitmap(b.bitmap, b.generation, b.pixels, b.width, b.height);
}

////////////////////////////////////////////////////////////////////////////////
// Fullscreen: available area minus the Dock

// What the Dock mod publishes: its window classes, and on its window the edge it sits on and the room it keeps
constexpr WCHAR kDockClass[] = L"WindhawkMacDock", kDockPlateClass[] = L"WindhawkMacDockPlate";
constexpr WCHAR kDockEdgeProp[] = L"WindhawkMacDock.Edge", kDockReservedProp[] = L"WindhawkMacDock.Reserved";

// The work area minus the room a Dock (Dock mod) keeps on an edge of this monitor.
// The Dock window tells which edge (1 bottom, 2 left, 3 right) and how many pixels, in two window properties.
RECT AvailableArea(HMONITOR monitor) {
    MONITORINFO info{sizeof(info)};
    GetMonitorInfoW(monitor, &info);
    RECT area = info.rcWork;
    for (HWND dock = FindWindowExW(nullptr, nullptr, kDockClass, nullptr); dock; dock = FindWindowExW(nullptr, dock, kDockClass, nullptr)) {
        if (MonitorFromWindow(dock, MONITOR_DEFAULTTONULL) != monitor) continue;
        LONG keep = (LONG)(INT_PTR)GetPropW(dock, kDockReservedProp);
        int edge = (int)(INT_PTR)GetPropW(dock, kDockEdgeProp);
        if (edge == 1) area.bottom = std::min(area.bottom, info.rcMonitor.bottom - keep);
        else if (edge == 2) area.left = std::max(area.left, info.rcMonitor.left + keep);
        else if (edge == 3) area.right = std::min(area.right, info.rcMonitor.right - keep);
    }
    bool usable = area.right - area.left >= 200 && area.bottom - area.top >= 200;
    return usable ? area : info.rcWork;
}

// Fullscreen: the Dock stays above the launcher (raised after the windows are found, so the search isn't disturbed)
void RaiseDockWindows() {
    HWND found[16];
    int count = 0;
    for (PCWSTR cls : {kDockPlateClass, kDockClass})
        for (HWND w = FindWindowExW(nullptr, nullptr, cls, nullptr); w && count < 16; w = FindWindowExW(nullptr, w, cls, nullptr))
            if (IsWindowVisible(w)) found[count++] = w;
    while (count > 0) SetWindowPos(found[--count], HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
}

////////////////////////////////////////////////////////////////////////////////
// Layout

struct Layout {
    // Window: screen rect, size in pixels, DPI scale (and extra room below for the "rise from taskbar" animation)
    RECT area{};
    int width = 0, height = 0, extraBottom = 0;
    float scale = 1;
    // Pieces placed by ComputeLayout
    D2D1_RECT_F search{}, btnPower{}, btnBack{}, panel{};
    // Grid of cells
    int columns = 1, visibleRows = 1;
    float gridLeft = 0, cellWidth = 140, cellHeight = 140, cellPad = 6;
    float icon = 72, labelHeight = 20;
    float viewTop = 0, viewBottom = 0, contentHeight = 0;   // visible band and the whole list height (vertical)
    float clipLeft = 0, clipRight = 0;   // horizontal limits of the grid (0, 0 = the whole window)
    // Pages (horizontal scrolling) and their dots
    bool horizontal = false;
    int perPage = 1, pages = 1;
    float pageWidth = 1;
    D2D1_RECT_F dots{};
    float dotsH = 20, dotGap = 16;
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
std::vector<int> g_mainItems;

D2D1_RECT_F g_folderOrigin{};
float g_folderAnim;
bool g_folderClosing;

bool g_renaming, g_renameSelectAll;
std::wstring g_renameText;

[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_labelFormat, g_searchFormat, g_siriFormat, g_messageFormat, g_titleFormat, g_menuFormat, g_tabFormat, g_listFormat, g_listSubFormat, g_glyphFormat, g_hintFormat, g_glyphLargeFormat, g_categoryFormat, g_keyFormat;
float g_formatScale;

constexpr float kTabWidth = 110; // per visible category

// A text format for one line, centered vertically; text that doesn't fit ends in "..."
ComPtr<IDWriteTextFormat> OneLineFormat(PCWSTR family, float px, DWRITE_FONT_WEIGHT weight, DWRITE_TEXT_ALIGNMENT align) {
    ComPtr<IDWriteTextFormat> format;
    if (FAILED(g_dwrite->CreateTextFormat(family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, px, L"", &format)))
        return nullptr;
    format->SetTextAlignment(align);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteInlineObject> dots;
    g_dwrite->CreateEllipsisTrimmingSign(format.Get(), &dots);
    DWRITE_TRIMMING cut{};
    cut.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
    format->SetTrimming(&cut, dots.Get());
    return format;
}

void EnsureFormats(float scale) {
    if (g_labelFormat && g_formatScale == scale) return;
    g_formatScale = scale;
    auto make = [&](float size, DWRITE_FONT_WEIGHT weight, DWRITE_TEXT_ALIGNMENT align, ComPtr<IDWriteTextFormat>& format) {
        format = OneLineFormat(L"Segoe UI Variable Text", size * scale, weight, align);
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
bool SiriStripVisible() { return g_siriTab == 4 || (g_settings.siriOriginal ? g_siriTab == 0 : TabsVisible()); }   // 4: the GitHub icon search
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
    float extent = L.horizontal ? (float)std::max(0, L.pages - 1) * L.pageWidth : L.contentHeight - (L.viewBottom - L.viewTop);
    return std::max(0.0f, extent);
}

// ---------- Math helpers ----------
float Ease(float t) { float u = 1 - std::clamp(t, 0.0f, 1.0f); return 1 - u * u * u; }   // cubic ease-out
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
PCWSTR TabGlyph(int tab) { return tab == 0 ? L"\uE71D" : tab == 1 ? L"\uE8B7" : tab == 3 ? L"\uE945" : tab == 4 ? L"\uE753" : L"\uE77F"; }

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

// The results box of each category (the GitHub gallery uses the Applications one)
int SiriPanelSettingW(int tab) {
    int w = tab == 1 ? g_settings.filesPanelW : tab == 2 ? g_settings.clipPanelW : tab == 3 ? g_settings.actionsPanelW : g_settings.siriPanelW;
    return w > 0 ? w : g_settings.siriPillW;   // 0 = as wide as the bar
}
int SiriRowsFor(int tab) { return tab == 1 ? g_settings.filesRows : tab == 2 ? g_settings.clipRows : tab == 3 ? g_settings.actionsRows : g_settings.siriRows; }
int SiriMaxPanelW() { int w = 0; for (int t = 0; t < 4; t++) w = std::max(w, SiriPanelSettingW(t)); return w; }
int SiriMaxRows() { int r = 0; for (int t = 0; t < 4; t++) r = std::max(r, SiriRowsFor(t)); return r; }
float g_siriPanelWShown = 0;   // the box width on screen: glides to the new category's width
float SiriPanelWidthNow(float s) {
    float target = std::round(SiriPanelSettingW(g_siriTab) * s);
    return g_siriPanelWShown > 0 ? g_siriPanelWShown : target;
}

int SiriWindowHeight(float s) {
    float pillH = std::round(g_settings.siriPillH * s), gap = std::round(kSiriGap * s), pad = std::round(kSiriPad * s);
    float tabsBlock = TabsVisible() || g_settings.siriOriginal ? std::round(kSiriTabs * s) + std::round(pad * 0.6f) : 0;
    float h = SiriTopPad(s) + pillH + gap + pad + tabsBlock + SiriMaxRows() * SiriCellHeight(s) + pad;   // room for the tallest box
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
    float panelW = std::min(W, SiriPanelWidthNow(s));
    float panelLeft = std::round((W - panelW) / 2), panelRight = panelLeft + panelW;
    const int rowsNow = SiriRowsFor(g_siriTab);
    float gap = std::round(kSiriGap * s), pad = std::round(kSiriPad * s), tabsH = std::round(kSiriTabs * s);

    {
        float d = pillH * PillIntroScale();
        float curW = std::clamp(d + (pillW - pillH) * PillIntroWidth(), d, W);
        float cx = W / 2 + g_siriShiftX, cy = SiriTopPad(s) + pillH / 2;
        L.search = D2D1::RectF(cx - curW / 2, cy - d / 2, cx + curW / 2, cy + d / 2);
    }
    L.btnPower = L.btnBack = L.panel = L.dots = D2D1::RectF(0, 0, 0, 0);
    if (g_settings.siriPowerButton && g_siriBtnAnim > 0.001f) {
        // Slides out from behind the right end of the bar and floats next to it
        float btn = SiriButtonSize(s), cy = SiriTopPad(s) + pillH / 2;
        float start = W / 2 + g_siriShiftX + pillW / 2 - btn, end = W / 2 + g_siriShiftX + pillW / 2 + SiriButtonGap(s);
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
        float tabsW = std::min(panelW - 2 * pad, g_siriTab == 4 ? panelW : g_settings.siriOriginal ? FiltersNaturalWidth() : TabsNaturalWidth());
        float tabsLeft = std::round((W - tabsW) / 2);
        L.siriTabs = D2D1::RectF(tabsLeft, panelTop + pad, tabsLeft + tabsW, panelTop + pad + tabsH);
        L.viewTop = L.siriTabs.bottom + std::round(pad * 0.6f);
    } else {
        L.siriTabs = D2D1::RectF(0, 0, 0, 0);
        L.viewTop = panelTop + pad;
    }
    L.visibleRows = std::max(1, rowsNow);
    L.list = ListMode(); L.listHeader = D2D1::RectF(0, 0, 0, 0);
    if (L.list) {
        float viewportH = rowsNow * SiriCellHeight(s), rowH = std::round(44 * s);
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
    // Siri panels: pages are as wide as the panel and slide inside it, never over the wallpaper
    L.pageWidth = L.horizontal ? panelW : std::max(1.0f, L.cellHeight * L.visibleRows);
    L.clipLeft = panelLeft + std::round(2 * s);
    L.clipRight = panelRight - std::round(2 * s);
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

    if (g_siriFlip) {
        // Bar low on the screen: everything is laid out upside down around it. The bar, its power button and the power
        // options are mirrored (the bar at the bottom of the window); the results box keeps its inside order and is
        // moved so it ends just above them (it grows upwards from the bar).
        auto mirror = [H](D2D1_RECT_F& r) { if (r.bottom > r.top) { float top = H - r.bottom; r.bottom = H - r.top; r.top = top; } };
        auto move = [](D2D1_RECT_F& r, float dy) { if (r.bottom > r.top) { r.top += dy; r.bottom += dy; } };
        mirror(L.search); mirror(L.btnPower); mirror(L.siriPower);
        float dy = (H - contentBottom) - panelTop;
        move(L.siriTabs, dy); move(L.listHeader, dy); move(L.dots, dy); move(full, dy);
        L.viewTop += dy; L.viewBottom += dy;
        L.siriPanel = ex > 0.001f ? LerpRect(L.search, full, ex) : D2D1::RectF(0, 0, 0, 0);
        L.siriPanel.left = std::max(L.siriPanel.left, 0.0f);
        L.siriPanel.right = std::min(L.siriPanel.right, W);
    }

    g_scrollTarget = std::clamp(g_scrollTarget, 0.0f, MaxScroll());
    g_scroll = std::clamp(g_scroll, 0.0f, MaxScroll());
}

void ComputeLayout() {
    if (g_settings.siri) { ComputeSiriLayout(); if (g_cfg.active) CfgApplyLayout(); return; }
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
            float top = std::max(std::round(H * 0.045f), std::round(28 * s));   // a little below the top edge
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
            float tw = std::min(W - 2 * padding, g_siriTab == 4 ? std::round(560 * s) : TabsNaturalWidth());
            float tl = std::round((W - tw) / 2), tt = L.search.bottom + std::round(gap * te);
            L.siriTabs = D2D1::RectF(tl, tt, tl + tw, tt + tabsH);
            extra = (tabsH + gap) * te;
        }
        L.viewTop = L.search.bottom + std::round(30 * s) + std::round(extra);
        bottomLimit = H - padding;
    }

    // A cell: the icon, its name under it and some air
    L.icon = std::round(g_settings.iconSize * s);
    L.labelHeight = std::round(18 * s);
    L.cellPad = std::round(6 * s);
    L.cellHeight = L.icon + std::round(L.labelHeight + 45 * s);

    float availableWidth = inFolder ? (L.panel.right - L.panel.left) - 40 * s : W - 2 * padding;
    float available = std::max(L.icon * 2, availableWidth);
    float minCell = L.icon + std::max(L.icon * 0.8f, 64 * s);   // icon plus room for its name

    int fit = (int)(available / minCell);
    L.columns = g_settings.columns > 0 ? g_settings.columns : std::max(2, std::min(fit, 14));
    // Few columns on a wide window: the cells don't spread out past 2.6 icons
    L.cellWidth = std::floor(std::min(available / L.columns, std::max(minCell, 2.6f * L.icon)));
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
    // Pages slide only inside the glass: the open folder's panel, or the window minus the room left for the theme shadow
    if (inFolder) { L.clipLeft = L.panel.left + std::round(2 * s); L.clipRight = L.panel.right - std::round(2 * s); }
    else if (WindowInset(s) > 0) { L.clipLeft = WindowInset(s) + std::round(2 * s); L.clipRight = W - WindowInset(s) - std::round(2 * s); }
    else L.clipLeft = L.clipRight = 0;
    L.perPage = std::max(1, L.visibleRows * L.columns);
    L.pages = std::max(1, (count + L.perPage - 1) / L.perPage);
    const int contentRows = (count + L.columns - 1) / L.columns;   // whole rows
    L.contentHeight = L.horizontal ? 0 : contentRows * L.cellHeight;

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
    float limit = MaxScroll();
    g_scroll = std::min(std::max(g_scroll, 0.0f), limit);
    g_scrollTarget = std::min(std::max(g_scrollTarget, 0.0f), limit);
    if (g_cfg.active) CfgApplyLayout();   // the settings use the window as it is
}

struct Cell { D2D1_RECT_F icon, label, highlight; };   // where the icon, its name and the hover highlight go
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
        const int row = local / L.columns;
        y = L.viewTop + row * L.cellHeight;
    } else {
        x = L.gridLeft + (index % L.columns) * L.cellWidth;
        const int row = index / L.columns;
        y = L.viewTop + row * L.cellHeight - std::round(scroll);
    }
    // Grid cell: rounded highlight, the icon centered at the top, its name under it
    const float s = L.scale, gap = std::round(8 * s);
    D2D1_POINT_2F iconAt = D2D1::Point2F(std::round(x + (L.cellWidth - L.icon) / 2), std::round(y + gap));
    Cell c;
    c.highlight = D2D1::RectF(x, y, x + L.cellWidth, y + L.cellHeight - L.cellPad);
    c.highlight.left += L.cellPad; c.highlight.right -= L.cellPad; c.highlight.bottom -= L.cellPad;
    c.icon = D2D1::RectF(iconAt.x, iconAt.y, iconAt.x + L.icon, iconAt.y + L.icon);
    float nameTop = iconAt.y + L.icon + gap, side = L.cellPad + 4 * s;
    c.label = D2D1::RectF(x + side, nameTop, x + L.cellWidth - side, nameTop + L.labelHeight);
    return c;
}
Cell CellAt(int index) { return CellAtIn(g_layout, g_scroll, index); }

bool InRect(const D2D1_RECT_F& r, float x, float y) { return !(x < r.left || x >= r.right || y < r.top || y >= r.bottom); }

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
    if (g_openFolderIndex >= 0 || !TabsVisible() || g_settings.siriOriginal || g_siriTab == 4) return -1;
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
    const Layout& L = g_layout;
    if ((g_settings.siri && g_siriAnim < 0.99f) || L.columns <= 0) return -1;
    float x = (float)px, y = (float)py;
    bool outsidePanel = L.clipRight > L.clipLeft && (x < L.clipLeft || x >= L.clipRight);
    if (outsidePanel || y < L.viewTop || y >= L.viewBottom) return -1;
    float gx = x - L.gridLeft, gy = y - L.viewTop;   // position inside the grid
    int page = 0;
    if (L.horizontal) {
        gx += std::round(g_scroll);
        page = (int)std::floor(gx / L.pageWidth);
        gx -= page * L.pageWidth;
    } else {
        gy += std::round(g_scroll);
    }
    if (page < 0 || gx < 0 || gy < 0 || gx >= L.cellWidth * L.columns) return -1;
    int col = (int)(gx / L.cellWidth), row = (int)(gy / L.cellHeight);
    int index = (L.horizontal ? page * L.perPage : 0) + row * L.columns + col;
    bool hit = index >= 0 && index < (int)g_layoutItems.size() && InRect(CellAt(index).highlight, x, y);
    return hit ? index : -1;
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
PopupMenu g_menu;

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
    if (g_settings.darkText) {
        // Light themes: a white rim vanishes on white glass, so the button also gets a fine gray outline
        // (darker at the bottom, like a shadow under the edge)
        D2D1_GRADIENT_STOP edge[2] = {{0.0f, D2D1::ColorF(0.55f, 0.57f, 0.62f, 0.30f)}, {1.0f, D2D1::ColorF(0.45f, 0.47f, 0.52f, 0.50f)}};
        if (auto line = MakeGradient(edge, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
            float bw = std::max(1.0f, 1.0f * s), half = bw / 2, rr = std::max(0.0f, radius - half);
            line->SetOpacity(a);
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), line.Get(), bw);
        }
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
// Context menus pass a cap here: they keep a fairly solid glass whatever the window's transparency, so they never
// melt into what is under them
int g_surfaceMaxTransparency = 100;
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
        if (i == g_menu.hover) Fill(row, Fg(0.11f), std::max(4 * s, rad - 8 * s));   // concentric with the menu
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

// What the launcher already drew under a context menu, blurred inside the menu's shape (the live blur only reaches
// the desktop behind the window, not the launcher's own icons)
void DrawMenuBackdropBlur(const D2D1_RECT_F& r, float rad, float alpha) {
    ComPtr<ID2D1DeviceContext> dc;
    if (alpha <= 0.01f || FAILED(g_target.As(&dc))) return;
    const Layout& L = g_layout;
    float s = L.scale, sigma = 9 * s * g_settings.menuBlur / 100.0f, margin = std::ceil(sigma * 3);   // 100 % = the default blur
    if (sigma < 0.3f) return;   // 0 = no blur
    D2D1::Matrix3x2F m;
    g_target->GetTransform(&m);
    D2D1_POINT_2F a = m.TransformPoint(D2D1::Point2F(r.left, r.top)), b = m.TransformPoint(D2D1::Point2F(r.right, r.bottom));
    int left = std::max(0, (int)std::floor(std::min(a.x, b.x) - margin)), top = std::max(0, (int)std::floor(std::min(a.y, b.y) - margin));
    int right = std::min(L.width, (int)std::ceil(std::max(a.x, b.x) + margin));
    int bottom = std::min(L.height + L.extraBottom, (int)std::ceil(std::max(a.y, b.y) + margin));
    if (right - left < 4 || bottom - top < 4) return;
    ComPtr<ID2D1Bitmap> copy;
    D2D1_SIZE_U size = D2D1::SizeU(right - left, bottom - top);
    if (FAILED(g_target->CreateBitmap(size, D2D1::BitmapProperties(g_target->GetPixelFormat()), &copy))) return;
    D2D1_POINT_2U at = D2D1::Point2U(0, 0);
    D2D1_RECT_U from = D2D1::RectU(left, top, right, bottom);
    if (FAILED(copy->CopyFromRenderTarget(&at, g_target.Get(), &from))) return;
    ComPtr<ID2D1Effect> blur;
    if (FAILED(dc->CreateEffect(kClsidGaussianBlur, &blur))) return;
    blur->SetInput(0, copy.Get());
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma);
    blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, rad, rad), &clip))) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), alpha), nullptr);
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    D2D1_POINT_2F offset = D2D1::Point2F((float)left, (float)top);
    dc->DrawImage(blur.Get(), &offset);
    g_target->SetTransform(m);
    g_target->PopLayer();
}

void DrawPopupMenu(const D2D1::Matrix3x2F& base) {
    if (!g_menu.open) return;
    const Layout& L = g_layout; float s = L.scale, e = Ease(g_menu.anim), k = 0.92f + 0.08f * e;
    if (e <= 0.001f) return;
    g_target->SetTransform(D2D1::Matrix3x2F::Scale(k, k, g_menu.origin) * base);
    D2D1_RECT_F r = g_menu.rect; float rad = (g_menu.category ? 24 : 14) * s;
    if (g_menu.category) { DrawCategoryMenu(r, rad, e); return; }
    // Same glass, shadow and rim as the selected theme (power menu and context menus, in every mode)
    bool themed = ActiveTheme() != Theme::Custom;
    DrawMenuBackdropBlur(r, rad, e);   // the icons and window under the menu, blurred (every theme)
    PushOpacity(e);
    if (themed) {
        // Context menus: the theme's glass, but solid enough to read over the window at any transparency,
        // with a fine outline that sets it apart from what is behind
        g_surfaceMaxTransparency = 35;
        DrawThemedShape(r, rad, BlurStrength() > 0 ? GetBackdropBitmap() : nullptr, true);
        g_surfaceMaxTransparency = 100;
        // A text color against the theme gets a veil on the glass so it reads: light under black text, dark under white
        bool lightGlass = ActiveTheme() == Theme::Light;
        if (g_settings.darkText && !lightGlass) Fill(r, D2D1::ColorF(1, 1, 1, 0.55f), rad);
        if (!g_settings.darkText && lightGlass) Fill(r, D2D1::ColorF(0, 0, 0, 0.55f), rad);
        float bw = std::max(1.0f, 1.0f * s), half = bw / 2, rr = std::max(0.0f, rad - half);
        g_brush->SetColor(g_settings.darkText ? D2D1::ColorF(0.45f, 0.47f, 0.52f, 0.35f) : D2D1::ColorF(1, 1, 1, 0.16f));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), g_brush.Get(), bw);
    } else {
        DrawThemeShadow(r, rad, 0.20f, 10 * s, 4 * s);
        ComPtr<ID2D1RoundedRectangleGeometry> geo;
        g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, rad, rad), &geo);
        if (geo) {
            g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), geo.Get()), nullptr);
            if (BlurStrength() > 0) {
                if (ID2D1Bitmap* bmp = GetBackdropBitmap())
                    g_target->DrawBitmap(bmp, D2D1::RectF(0, 0, (float)L.width, (float)L.height), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                        D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
            }
            Fill(r, ThemeMenuColor());
            Fill(r, D2D1::ColorF(1, 1, 1, 0.07f));
            g_target->PopLayer();
        }
    }
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
    if (themed) return;   // the themed shape already has its rim
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
    return a && (it < kFolderBase || (Picking() && it >= kFolderAppBase && it < kTabBase)) ? a : nullptr;
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
    const int page = index / L.perPage;
    float off = std::abs(page - scroll / L.pageWidth);
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
    float reserve = g_query.empty() ? 46.0f : 38.0f;          // empty: room for the "..." button
    if (siri) reserve = g_query.empty() ? 52.0f : 62.0f;
    float textLeft = (siri ? 62.0f : (box ? 34.0f : 30.0f)) * s;
    return D2D1::RectF(L.search.left + textLeft, L.search.top,
                       L.search.right - reserve * s, L.search.bottom);
}

// The typed text laid out on one line. When it is wider than the bar, it slides left so its end stays
// visible: `origin` is where the layout starts (left of the text box by the hidden part).
ComPtr<IDWriteTextLayout> QueryLayout(const Layout& L, IDWriteTextFormat* format, const D2D1_RECT_F& box, D2D1_POINT_2F& origin) {
    ComPtr<IDWriteTextLayout> layout;
    origin = D2D1::Point2F(box.left, box.top);
    if (!format || FAILED(g_dwrite->CreateTextLayout(g_query.c_str(), (UINT32)g_query.size(), format, 10000.0f, box.bottom - box.top, &layout)))
        return nullptr;
    layout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    DWRITE_TEXT_METRICS m{};
    if (SUCCEEDED(layout->GetMetrics(&m))) {
        float hidden = m.widthIncludingTrailingWhitespace - (box.right - box.left - 3 * L.scale);
        if (hidden > 0) origin.x -= hidden;
    }
    return layout;
}

size_t QueryIndexAtPoint(const Layout& L, float x, float y) {
    if (g_query.empty() || !g_dwrite) return 0;
    IDWriteTextFormat* fmt = g_settings.siri && g_siriFormat ? g_siriFormat.Get() : g_searchFormat.Get();
    D2D1_POINT_2F origin;
    ComPtr<IDWriteTextLayout> layout = QueryLayout(L, fmt, QueryTextRect(L), origin);
    if (!layout) return g_query.size();
    DWRITE_HIT_TEST_METRICS hit{};
    BOOL trailing = FALSE, inside = FALSE;
    layout->HitTestPoint(x - origin.x, y - origin.y, &trailing, &inside, &hit);
    return std::min<size_t>(hit.textPosition + (trailing ? hit.length : 0), g_query.size());
}

void DrawSearchBar(const Layout& L, float a) {
    if (a <= 0.001f) return;
    float s = L.scale;
    const D2D1_RECT_F& r = L.search;
    bool siri = g_settings.siri;
    bool box = g_settings.searchBox && !siri;
    if (box) {
        float rad = (r.bottom - r.top) / 2;
        if (ActiveTheme() == Theme::Custom) DrawGlassButton(r, rad, a, g_searchActive ? 1.35f : 1.0f);
        else {
            // The same glass as the chosen theme (a field of that glass inside the window), a little brighter while typing
            PushOpacity(a);
            DrawThemedShape(r, rad, nullptr, false);
            if (g_searchActive) Fill(r, Fg(0.06f), rad);
            g_target->PopLayer();
        }
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

    float reserve = g_query.empty() ? 46 : 38;
    if (siri) reserve = g_query.empty() ? 52 : 62;
    float textLeft = (siri ? 62 : (box ? 34 : 30)) * s;
    D2D1_RECT_F text = D2D1::RectF(r.left + textLeft, r.top, r.right - reserve * s, r.bottom);
    IDWriteTextFormat* fmt = siri && g_siriFormat ? g_siriFormat.Get() : g_searchFormat.Get();
    if (!fmt) return;

    if (g_query.empty() && original) {
        if (!flying) {
            PCWSTR ph = Picking() ? (g_selection.empty() ? Tr(L"Hold an app to select it") : Tr(L"Click apps to select or deselect them")) : g_siriChosen ? TabLabel(g_siriTab) : Tr(L"Search");
            g_brush->SetColor(Fg((g_siriChosen ? 0.62f : 0.4f) * a));
            g_target->DrawText(ph, (UINT32)wcslen(ph), fmt, text, g_brush.Get());
        }
    } else if (g_query.empty()) {
        PCWSTR ph = g_siriTab == 1 ? Tr(L"Search Files") : g_siriTab == 2 ? Tr(L"Search Clipboard") : g_siriTab == 3 ? Tr(L"Search Actions")
                  : siri ? Tr(L"Search Apps") : Tr(L"Search Applications");
        g_brush->SetColor(Fg(0.4f * a));
        g_target->DrawText(ph, (UINT32)wcslen(ph), fmt, text, g_brush.Get());
    }

    D2D1_POINT_2F origin;
    ComPtr<IDWriteTextLayout> layout = QueryLayout(L, fmt, text, origin);
    if (!layout) return;
    g_target->PushAxisAlignedClip(text, D2D1_ANTIALIAS_MODE_ALIASED);
    size_t selStart = QuerySelStart(), selEnd = QuerySelEnd();
    if (selEnd > selStart && selEnd <= g_query.size()) {
        DWRITE_HIT_TEST_METRICS hitMetrics[256]{}; UINT32 actual = 0;
        if (SUCCEEDED(layout->HitTestTextRange((UINT32)selStart, (UINT32)(selEnd - selStart),
                origin.x, origin.y, hitMetrics, 256, &actual))) {
            for (UINT32 i = 0; i < std::min<UINT32>(actual, 256); ++i) {
                const auto& hm = hitMetrics[i];
                Fill(D2D1::RectF(hm.left, hm.top + 2.0f * s, hm.left + hm.width,
                                 hm.top + hm.height - 2.0f * s),
                     D2D1::ColorF(0.20f, 0.48f, 0.92f, 0.48f));
            }
        }
    }
    g_brush->SetColor(Fg(0.95f * a));
    g_target->DrawTextLayout(origin, layout.Get(), g_brush.Get());

    // Caret follows the insertion point, not always the end of the query.
    if ((siri || g_searchActive) && !QueryHasSelection() && (!g_frameTimer || (int)(Now() * 2) % 2 == 0)) {
        DWRITE_HIT_TEST_METRICS hm{}; FLOAT caretX = 0, caretY = 0;
        layout->HitTestTextPosition((UINT32)std::min(g_queryCaret, g_query.size()), FALSE, &caretX, &caretY, &hm);
        caretX += origin.x;
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

#ifndef ERROR_SHUTDOWN_USERS_LOGGED_ON
#define ERROR_SHUTDOWN_USERS_LOGGED_ON 1191L
#endif

// Like Windows: other signed-in users are only signed off after a warning (Cancel keeps them)
DWORD ShutdownWithWarning(DWORD flags) {
    DWORD r = InitiateShutdownW(nullptr, nullptr, 0, flags, SHTDN_REASON_FLAG_PLANNED);
    if (r == ERROR_SHUTDOWN_USERS_LOGGED_ON &&
        MessageBoxW(nullptr, Tr(L"Someone else is still using this PC. If you continue, they could lose unsaved work."), L"Siri Plus Launchpad",
                    MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND) == IDOK)
        r = InitiateShutdownW(nullptr, nullptr, 0, flags | SHUTDOWN_FORCE_OTHERS, SHTDN_REASON_FLAG_PLANNED);
    return r;
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
        case kMenuRestart: {
            EnableShutdownPrivilege();
            DWORD r = ShutdownWithWarning(SHUTDOWN_RESTART);
            if (r != ERROR_SUCCESS && r != ERROR_SHUTDOWN_USERS_LOGGED_ON) ExitWindowsEx(EWX_REBOOT, SHTDN_REASON_FLAG_PLANNED);
            break;
        }
        case kMenuShutdown: {
            // Like Windows' own Shut down: a hybrid shutdown when Fast Startup is on
            EnableShutdownPrivilege();
            DWORD hiberboot = 0, size = sizeof(hiberboot);
            RegGetValueW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Power", L"HiberbootEnabled",
                         RRF_RT_REG_DWORD, nullptr, &hiberboot, &size);
            DWORD r = ShutdownWithWarning(SHUTDOWN_POWEROFF | (hiberboot ? SHUTDOWN_HYBRID : 0));
            // Hibernation turned off leaves Fast Startup unavailable: try a plain shutdown
            if (r != ERROR_SUCCESS && r != ERROR_SHUTDOWN_USERS_LOGGED_ON && hiberboot) r = ShutdownWithWarning(SHUTDOWN_POWEROFF);
            if (r != ERROR_SUCCESS && r != ERROR_SHUTDOWN_USERS_LOGGED_ON) ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF, SHTDN_REASON_FLAG_PLANNED);
            break;
        }
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
                float x = std::round(cx2 + textWidth / 2) + 1, mid = (row.top + row.bottom) / 2;
                g_brush->SetColor(Fg(0.8f * a));
                g_target->DrawLine(D2D1::Point2F(x, mid - 9 * s), D2D1::Point2F(x, mid + 9 * s), g_brush.Get(), std::max(1.0f, s));   // caret
            }
        } else {
            const std::wstring& name = g_folders[g_openFolderIndex].name;
            g_brush->SetColor(Fg(0.95f * a));
            g_target->DrawText(name.c_str(), (UINT32)name.size(), g_titleFormat.Get(), row, g_brush.Get());
        }
    }
}

void DrawMessage(PCWSTR text) {
    if (!g_messageFormat || !text) return;
    const Layout& L = g_layout;
    float height = g_settings.siri ? L.cellHeight : 120 * L.scale;
    g_brush->SetColor(Fg(0.6f));
    g_target->DrawText(text, (UINT32)wcslen(text), g_messageFormat.Get(), D2D1::RectF(0, L.viewTop, (float)L.width, L.viewTop + height), g_brush.Get());
}

D2D1_RECT_F FolderMiniSlot(const D2D1_RECT_F& r, int index, float pad) {
    int k = std::min(index, 3);
    float cellW = (r.right - r.left - pad * 3) / 2;
    float cellH = (r.bottom - r.top - pad * 3) / 2;
    float left = r.left + pad + (k % 2) * (cellW + pad);
    const int pairRow = k / 2;
    float top = r.top + pad + pairRow * (cellH + pad);
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

// Clipboard rows: the small X at the right end that removes that copy
int g_clipXHover = -1;
D2D1_RECT_F ClipDeleteRect(const D2D1_RECT_F& row) {
    float s = g_layout.scale, cy = (row.top + row.bottom) / 2, d = std::round(24 * s);
    return D2D1::RectF(row.right - 10 * s - d, cy - d / 2, row.right - 10 * s, cy + d / 2);
}
bool ClipRows() { return g_siriTab == 2 && g_openFolderIndex < 0 && g_layout.list; }

IDWriteTextLayout* LabelLayout(App& app, const D2D1_RECT_F& r) {
    float w = r.right - r.left, h = r.bottom - r.top;
    if (!g_labelFormat || w <= 0 || h <= 0) return nullptr;
    if (!app.labelLayout || app.labelFormat != g_labelFormat.Get() || std::fabs(app.labelW - w) > 0.5f || std::fabs(app.labelH - h) > 0.5f ||
        app.labelText != app.name) {
        app.labelLayout.Reset();
        g_dwrite->CreateTextLayout(app.name.c_str(), (UINT32)app.name.size(), g_labelFormat.Get(), w, h, &app.labelLayout);
        app.labelFormat = g_labelFormat.Get(); app.labelW = w; app.labelH = h; app.labelText = app.name;
    }
    return app.labelLayout.Get();
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
            if (g_siriTab == 4) msg = GhMessage();
            DrawMessage(msg);
            return;
        }
    } else if (items.empty()) {
        return;
    }
    bool panelClip = L.clipRight > L.clipLeft;
    g_target->PushAxisAlignedClip(D2D1::RectF(panelClip ? L.clipLeft : 0, L.viewTop, panelClip ? L.clipRight : (float)L.width, L.viewBottom),
                                  D2D1_ANTIALIAS_MODE_ALIASED);
    int first, last; VisibleRange(L, scroll, (int)items.size(), first, last);

    for (int i = first; i < last; i++) {
        int item = items[i];
        if (item < 0) continue;
        if (IsFolderItem(item) ? item - kFolderBase >= (int)g_folders.size() : !ResolveApp(item)) continue;
        Cell c = CellAtIn(L, scroll, i);

        bool sel = interactive && IsSelected(i);
        if (sel) Fill(c.highlight, D2D1::ColorF(0.04f, 0.52f, 1.0f, 0.22f), 12 * s);
        else if (interactive && (i == g_selected || i == g_hover))
            Fill(c.highlight, Fg(i == g_selected ? 0.18f : 0.10f), 12 * s);
        float dragDim = (interactive && ((i == g_pressed && g_isDragging) || i == g_dragItem || (sel && GroupDrag()))) ? 0.4f : 1.0f;
        float opacity = dragDim * alpha * PageFade(L, scroll, i);

        if (IsFolderItem(item)) {
            const Folder& f = g_folders[item - kFolderBase];
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
                    if (app.kind == 2 && interactive && ClipRows()) {
                        // X to remove this copy: faint, clearer on the hovered row, a glass circle under the mouse
                        D2D1_RECT_F x = ClipDeleteRect(c.highlight);
                        nameRect.right = std::min(nameRect.right, x.left - 8 * s);
                        if (i == g_clipXHover) DrawGlassButton(x, (x.right - x.left) / 2, opacity, 1.4f, false);
                        D2D1_POINT_2F xc = D2D1::Point2F((x.left + x.right) / 2, (x.top + x.bottom) / 2);
                        float arm = 4 * s;
                        g_brush->SetColor(Fg((i == g_clipXHover ? 0.95f : i == g_hover ? 0.60f : 0.28f) * opacity));
                        g_target->DrawLine(D2D1::Point2F(xc.x - arm, xc.y - arm), D2D1::Point2F(xc.x + arm, xc.y + arm), g_brush.Get(), 1.4f * s);
                        g_target->DrawLine(D2D1::Point2F(xc.x - arm, xc.y + arm), D2D1::Point2F(xc.x + arm, xc.y - arm), g_brush.Get(), 1.4f * s);
                    }
                    if (twoLines) nameRect.bottom = c.label.top + (c.label.bottom - c.label.top) * 0.58f;
                    std::wstring name = app.name, folder;
                    if (clip) ClipRowText(app.target, name, folder);
                    else if (twoLines) folder = app.target.substr(0, app.target.find_last_of(L'\\'));
                    g_brush->SetColor(Fg(0.95f * opacity));
                    g_target->DrawText(name.c_str(), (UINT32)name.size(), g_listFormat.Get(), nameRect, g_brush.Get());
                    if (twoLines) {
                        D2D1_RECT_F subRect = D2D1::RectF(c.label.left, nameRect.bottom - 3 * s, nameRect.right, c.label.bottom - 2 * s);
                        g_brush->SetColor(Fg(0.5f * opacity));
                        g_target->DrawText(folder.c_str(), (UINT32)folder.size(), g_listSubFormat.Get(), subRect, g_brush.Get());
                    }
                }
            } else if (g_labelFormat) {
                if (IDWriteTextLayout* tl = LabelLayout(app, c.label)) {
                    if (!g_settings.darkText) {
                        g_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.45f * opacity));
                        g_target->DrawTextLayout(D2D1::Point2F(c.label.left, c.label.top + 1 * s), tl, g_brush.Get());
                    }
                    g_brush->SetColor(Fg(0.95f * opacity));
                    g_target->DrawTextLayout(D2D1::Point2F(c.label.left, c.label.top), tl, g_brush.Get());
                }
            }
        }
    }

    if (interactive && g_isDragging && g_dragItem >= 0 && g_dragItem < (int)items.size() && g_dragX >= 0) {
        int item = items[g_dragItem];
        D2D1_RECT_F dragRect = D2D1::RectF(g_dragX - L.icon / 2, g_dragY - L.icon / 2, g_dragX + L.icon / 2, g_dragY + L.icon / 2);
        if (IsFolderItem(item)) {
            if (item - kFolderBase < (int)g_folders.size()) DrawFolderIcon(g_folders[item - kFolderBase], dragRect, 0.8f);
        } else if (App* da = ResolveApp(item)) {
            if (GroupDrag() && item < kFolderBase) DrawGroupDrag(dragRect, item);
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
            if (IDWriteTextLayout* tl = LabelLayout(app, label)) {
                if (!g_settings.darkText) {
                    g_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.45f * lo));
                    g_target->DrawTextLayout(D2D1::Point2F(label.left, label.top + s), tl, g_brush.Get());
                }
                g_brush->SetColor(Fg(0.95f * lo));
                g_target->DrawTextLayout(D2D1::Point2F(label.left, label.top), tl, g_brush.Get());
            }
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

// ---------- Liquid glass (Glass theme, like macOS 26): the screen behind, refracted at the edges ----------
// A snapshot of the screen is taken as the launcher opens (before it shows). Each glass shape draws that picture
// lightly blurred and bent near its edges by a displacement map (the background slides in from beyond the edge,
// as through thick curved glass), then the light on its rim. With "live" on, the launcher is hidden from screen
// captures so the picture can be retaken a few times per second.
constexpr GUID kClsidAffine = {0x6aa97485, 0x6354, 0x4cfc, {0x90, 0x8c, 0xe4, 0xa7, 0x4f, 0x62, 0xc9, 0x6c}};
constexpr GUID kClsidDisplacement = {0xedc48364, 0x0417, 0x4111, {0x94, 0x50, 0x43, 0x84, 0x5f, 0xa9, 0xf8, 0x90}};
constexpr GUID kClsidColorMatrix = {0x921f03d6, 0x641c, 0x47df, {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};
constexpr UINT_PTR kTimerLiquid = 9;
constexpr int kLiquidMargin = 64;   // extra picture around the window, for the edges to bend in from

struct LiquidSnap {
    std::vector<UINT32> pixels;
    int width = 0, height = 0;
    RECT rc{};                       // screen rect of the picture
    ComPtr<ID2D1Bitmap> bitmap;
    UINT generation = 0;
    bool fresh = false;              // new pixels not yet in the bitmap
};
[[clang::no_destroy]] LiquidSnap g_liquid;

struct LiquidMap { int w = 0, h = 0, radius = 0, band = 0, depth = 0; ComPtr<ID2D1Bitmap> bitmap; UINT generation = 0; double used = 0; };
[[clang::no_destroy]] std::vector<LiquidMap> g_liquidMaps;
[[clang::no_destroy]] ComPtr<ID2D1Effect> g_lqSource, g_lqBlur, g_lqMapXf, g_lqDisp, g_lqColor;
UINT g_lqEffectsGeneration;

bool LiquidSetting();
// The settings panel always uses the theme's frosted glass (live blur): the liquid picture is taken for the launcher's
// own area, and the sections and the panel reach far outside it, where it would show as dark blocks
bool LiquidWanted() { return LiquidSetting() && !g_cfg.active; }
bool LiquidSetting() {
    // Only in Floating and Original Siri: there the glass is a bar and a box. In Window and Fullscreen the glass is the
    // whole window, and bending all of it (and retaking it live) is far too heavy for every frame
    if (!g_settings.siri) return false;
    Theme t = ActiveTheme();
    bool glassTheme = t == Theme::Glass || t == Theme::Siri ||
                      ((t == Theme::Light || t == Theme::Dark || t == Theme::Oled) && g_settings.glassEffect);
    return glassTheme && g_settings.glassLiquid;
}
bool LiquidActive() { return LiquidWanted() && g_liquid.width > 0; }

void FreeLiquid() {
    g_liquid.pixels.clear(); g_liquid.pixels.shrink_to_fit();
    g_liquid.width = g_liquid.height = 0; g_liquid.bitmap.Reset(); g_liquid.fresh = false;
    g_liquidMaps.clear();
}

bool CaptureLiquid(const RECT& rc) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return false;
    void* bits = nullptr;
    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    HBITMAP bitmap = NewDib(dc, w, h, &bits);
    if (!bitmap) { if (dc) DeleteDC(dc); ReleaseDC(nullptr, screen); return false; }
    HGDIOBJ old = SelectObject(dc, bitmap);
    BitBlt(dc, 0, 0, w, h, screen, rc.left, rc.top, SRCCOPY);
    GdiFlush();
    ReleaseDC(nullptr, screen);
    if (g_liquid.width != w || g_liquid.height != h) { g_liquid.bitmap.Reset(); g_liquid.pixels.assign((size_t)w * h, 0); }
    const UINT32* src = (const UINT32*)bits;
    for (size_t i = 0; i < (size_t)w * h; i++) g_liquid.pixels[i] = src[i] | 0xFF000000;   // opaque
    SelectObject(dc, old); DeleteObject(bitmap); DeleteDC(dc);
    g_liquid.width = w; g_liquid.height = h; g_liquid.rc = rc; g_liquid.fresh = true;
    return true;
}

ID2D1Bitmap* LiquidSnapBitmap() {
    if (g_liquid.width <= 0) return nullptr;
    if (g_liquid.bitmap && g_liquid.generation != g_targetGeneration) g_liquid.bitmap.Reset();
    if (!g_liquid.bitmap) {
        g_target->CreateBitmap(D2D1::SizeU(g_liquid.width, g_liquid.height), g_liquid.pixels.data(), g_liquid.width * 4,
                               D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &g_liquid.bitmap);
        g_liquid.generation = g_targetGeneration;
        g_liquid.fresh = false;
    } else if (g_liquid.fresh) {
        g_liquid.bitmap->CopyFromMemory(nullptr, g_liquid.pixels.data(), g_liquid.width * 4);
        g_liquid.fresh = false;
    }
    return g_liquid.bitmap.Get();
}

// Displacement map of a rounded rectangle (half resolution): R/G = where to look, around 0.5 = right here.
// Within `band` of the edge the picture is taken from up to `depth` pixels further out, more right at the edge.
ID2D1Bitmap* LiquidMapFor(float w, float h, float radius, float band, float depth) {
    int mw = std::max(1, (int)std::ceil(w / 2)), mh = std::max(1, (int)std::ceil(h / 2));
    int ir = (int)std::lround(radius), ib = (int)std::lround(band), id = (int)std::lround(depth);
    for (LiquidMap& m : g_liquidMaps)
        if (m.w == mw && m.h == mh && m.radius == ir && m.band == ib && m.depth == id && m.generation == g_targetGeneration) {
            m.used = Now();
            return m.bitmap.Get();
        }
    std::vector<UINT32> px((size_t)mw * mh);
    float hw = mw, hh = mh;   // half of the full size = the map size
    float r = std::min({radius / 2, hw, hh}), b = std::max(1.0f, band / 2), dep = std::max(0.5f, depth / 2);
    for (int y = 0; y < mh; y++) {
        for (int x = 0; x < mw; x++) {
            float px_ = x + 0.5f - hw, py = y + 0.5f - hh;
            float qx = std::fabs(px_) - (hw - r), qy = std::fabs(py) - (hh - r);
            float ox = std::max(qx, 0.0f), oy = std::max(qy, 0.0f), len = std::sqrt(ox * ox + oy * oy);
            float dist = len + std::min(std::max(qx, qy), 0.0f) - r;   // < 0 inside
            float nx, ny;
            if (qx > 0 && qy > 0 && len > 0) { nx = ox / len * (px_ < 0 ? -1.0f : 1.0f); ny = oy / len * (py < 0 ? -1.0f : 1.0f); }
            else if (qx > qy) { nx = px_ < 0 ? -1.0f : 1.0f; ny = 0; }
            else { nx = 0; ny = py < 0 ? -1.0f : 1.0f; }
            float t = std::clamp(1.0f + dist / b, 0.0f, 1.0f);   // 1 at the edge, 0 deeper than the band
            float k = dep * std::pow(t, 2.2f);
            int R = std::clamp((int)std::lround(255 * (0.5f + nx * k / (2 * dep))), 0, 255);
            int G = std::clamp((int)std::lround(255 * (0.5f + ny * k / (2 * dep))), 0, 255);
            px[(size_t)y * mw + x] = 0xFF000000u | ((UINT32)R << 16) | ((UINT32)G << 8);
        }
    }
    ComPtr<ID2D1Bitmap> bitmap;
    if (FAILED(g_target->CreateBitmap(D2D1::SizeU(mw, mh), px.data(), mw * 4,
                                      D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)), &bitmap)))
        return nullptr;
    if (g_liquidMaps.size() >= 8) {
        auto oldest = std::min_element(g_liquidMaps.begin(), g_liquidMaps.end(), [](const LiquidMap& a, const LiquidMap& c) { return a.used < c.used; });
        g_liquidMaps.erase(oldest);
    }
    g_liquidMaps.push_back({mw, mh, ir, ib, id, bitmap, g_targetGeneration, Now()});
    return g_liquidMaps.back().bitmap.Get();
}

bool EnsureLiquidEffects(ID2D1DeviceContext* dc) {
    if (g_lqColor && g_lqEffectsGeneration == g_targetGeneration) return true;
    g_lqSource.Reset(); g_lqBlur.Reset(); g_lqMapXf.Reset(); g_lqDisp.Reset(); g_lqColor.Reset();
    if (FAILED(dc->CreateEffect(kClsidAffine, &g_lqSource)) || FAILED(dc->CreateEffect(kClsidGaussianBlur, &g_lqBlur)) ||
        FAILED(dc->CreateEffect(kClsidAffine, &g_lqMapXf)) || FAILED(dc->CreateEffect(kClsidDisplacement, &g_lqDisp)) ||
        FAILED(dc->CreateEffect(kClsidColorMatrix, &g_lqColor))) {
        g_lqColor.Reset();
        return false;
    }
    g_lqEffectsGeneration = g_targetGeneration;
    g_lqBlur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    g_lqMapXf->SetValue(D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE, D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR);
    g_lqDisp->SetValue(D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT, D2D1_CHANNEL_SELECTOR_R);
    g_lqDisp->SetValue(D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT, D2D1_CHANNEL_SELECTOR_G);
    // A touch more saturated and brighter, like light passing through glass
    float sat = 1.12f, gain = 1.03f, lw[3] = {0.2126f, 0.7152f, 0.0722f};
    D2D1_MATRIX_5X4_F cm{};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) cm.m[i][j] = ((1 - sat) * lw[i] + (i == j ? sat : 0.0f)) * gain;
    cm.m[3][3] = 1;
    for (int j = 0; j < 3; j++) cm.m[4][j] = 0.015f;
    g_lqColor->SetValue(D2D1_COLORMATRIX_PROP_COLOR_MATRIX, cm);
    return true;
}

// The refracted picture inside a shape (the caller has clipped to the shape)
bool DrawLiquidImage(const D2D1_RECT_F& r, float radius) {
    ComPtr<ID2D1DeviceContext> dc;
    if (FAILED(g_target.As(&dc))) return false;
    ID2D1Bitmap* snap = LiquidSnapBitmap();
    if (!snap || !EnsureLiquidEffects(dc.Get())) return false;
    // In device pixels (the launcher only scales and moves its shapes)
    D2D1::Matrix3x2F m;
    g_target->GetTransform(&m);
    D2D1_POINT_2F a = m.TransformPoint(D2D1::Point2F(r.left, r.top)), b = m.TransformPoint(D2D1::Point2F(r.right, r.bottom));
    D2D1_RECT_F dr = D2D1::RectF(std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x), std::max(a.y, b.y));
    float w = dr.right - dr.left, h = dr.bottom - dr.top, k = std::fabs(m._11);
    if (w < 2 || h < 2) return false;
    float s = g_layout.scale;
    float band = std::min(std::min(w, h) * 0.42f, 30 * s);
    float depth = band * g_settings.refraction / 100.0f * 1.6f;          // 60 % = about one band deep
    // While the settings open or close every shape changes size each frame: no bending then (a new map
    // per frame is slow, and the moving edges look odd); it comes back as soon as they settle
    if (g_cfg.active && (g_cfg.closing || g_cfg.anim < 1.0f)) depth = 0;
    float sigma = std::pow(g_settings.blurAmount / 100.0f, 2.0f) * 4 * s;   // clear glass: just a light softening
    const Layout& L = g_layout;
    g_lqSource->SetInput(0, snap);
    g_lqSource->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,
                         D2D1::Matrix3x2F::Translation((float)(g_liquid.rc.left - L.area.left), (float)(g_liquid.rc.top - L.area.top)));
    ComPtr<ID2D1Image> picture;
    g_lqSource->GetOutput(&picture);
    if (sigma > 0.3f) {
        g_lqBlur->SetInputEffect(0, g_lqSource.Get());
        g_lqBlur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, sigma);
        picture.Reset();
        g_lqBlur->GetOutput(&picture);
    }
    if (depth >= 0.5f) {
        if (ID2D1Bitmap* map = LiquidMapFor(w, h, radius * k, band, depth)) {
            D2D1_SIZE_U ms = map->GetPixelSize();
            g_lqMapXf->SetInput(0, map);
            g_lqMapXf->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX,
                                D2D1::Matrix3x2F::Scale(w / ms.width, h / ms.height) * D2D1::Matrix3x2F::Translation(dr.left, dr.top));
            g_lqDisp->SetInput(0, picture.Get());
            g_lqDisp->SetInputEffect(1, g_lqMapXf.Get());
            g_lqDisp->SetValue(D2D1_DISPLACEMENTMAP_PROP_SCALE, 2 * std::max(0.5f, depth / 2) * (w / ms.width));
            picture.Reset();
            g_lqDisp->GetOutput(&picture);
        }
    }
    g_lqColor->SetInput(0, picture.Get());
    ComPtr<ID2D1Image> out;
    g_lqColor->GetOutput(&out);
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());
    dc->DrawImage(out.Get(), D2D1::Point2F(dr.left, dr.top), dr, D2D1_INTERPOLATION_MODE_LINEAR);
    g_target->SetTransform(m);
    g_target->Flush();   // the effects are set up again for the next shape
    return true;
}

// A liquid glass shape: the refracted background with the same body and edge as the frosted glass
void DrawLiquidShape(const D2D1_RECT_F& r, float radius, bool shadow) {
    float s = g_layout.scale, h = r.bottom - r.top, w = r.right - r.left;
    radius = std::min(radius, std::min(w, h) / 2);
    if (shadow) DrawThemeShadow(r, radius, 0.16f, 10 * s, 4 * s);
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &clip))) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
    DrawLiquidImage(r, radius);
    float tint = (100 - g_settings.glassTransparency) / 100.0f * 0.6f;
    D2D1_GRADIENT_STOP body[3] = {
        {0.0f, D2D1::ColorF(0.07f, 0.07f, 0.07f, tint * 0.85f)},
        {0.5f, D2D1::ColorF(0.04f, 0.04f, 0.04f, tint)},
        {1.0f, D2D1::ColorF(0.06f, 0.06f, 0.06f, tint * 0.9f)},
    };
    if (auto brush = MakeGradient(body, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) g_target->FillRectangle(r, brush.Get());
    float glowH = std::min(h * 0.22f, 14 * s);
    D2D1_GRADIENT_STOP glow[2] = {{0.0f, D2D1::ColorF(1, 1, 1, 0.07f)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.0f)}};
    if (auto top = MakeGradient(glow, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + glowH)))
        g_target->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + glowH), top.Get());
    g_target->PopLayer();
    D2D1_GRADIENT_STOP softRim[7] = {
        {0.0f, D2D1::ColorF(0.83f, 0.83f, 0.83f, 0.44f)}, {0.1f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.31f)},
        {0.25f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.38f)}, {0.5f, D2D1::ColorF(0.13f, 0.13f, 0.13f, 0.44f)},
        {0.75f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.56f)}, {0.9f, D2D1::ColorF(0.25f, 0.25f, 0.25f, 0.56f)},
        {1.0f, D2D1::ColorF(0.76f, 0.76f, 0.76f, 0.44f)},
    };
    if (auto rim = MakeGradient(softRim, 7, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))) {
        float bw = std::max(1.0f, 1.2f * s), half = bw / 2, rr = std::max(0.0f, radius - half);
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), rim.Get(), bw);
    }
}

// Light: bright frosted glass, white top glow, luminous rim and a soft shadow.
// Clear glass: almost no fill, the wallpaper shows through; a thick-glass edge and a shadow projected on the wallpaper.
// Siri (macOS 27): smoky glass, dark at the top fading to clear at the bottom, thin light rim.
void DrawThemedShape(const D2D1_RECT_F& r, float radius, ID2D1Bitmap* backdrop, bool shadow) {
    const Layout& L = g_layout;
    const int transparency = std::min(g_settings.glassTransparency, g_surfaceMaxTransparency);
    float s = L.scale, h = r.bottom - r.top;
    Theme theme = ActiveTheme();
    if (h < 1 || r.right - r.left < 1) return;
    if (theme == Theme::Glass && LiquidActive() && g_surfaceMaxTransparency >= 100) { DrawLiquidShape(r, radius, shadow); return; }
    if (shadow) {
        if (theme == Theme::Glass) {
            DrawThemeShadow(r, radius, 0.16f, 10 * s, 4 * s);   // small, discreet: just a sense of depth
        } else {
            DrawThemeShadow(r, radius, theme == Theme::Light ? 0.14f : theme == Theme::Oled ? 0.30f : 0.18f, 10 * s, 4 * s);
        }
    }

    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    if (FAILED(g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(r, radius, radius), &clip))) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get()), nullptr);
    if (backdrop)
        g_target->DrawBitmap(backdrop, D2D1::RectF(0, 0, (float)L.width, (float)L.height), 1, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                             D2D1::RectF(0, 0, g_backdropImage.sourceWidth, g_backdropImage.sourceHeight));
    if (theme != Theme::Glass && LiquidActive()) DrawLiquidImage(r, radius);   // liquid glass: the bent background under the tint
    D2D1_GRADIENT_STOP lightBody[3] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.52f)},
        {0.5f, D2D1::ColorF(0.97f, 0.97f, 0.99f, 0.40f)},
        {1.0f, D2D1::ColorF(0.93f, 0.94f, 0.97f, 0.34f)},
    };
    float tint = (100 - transparency) / 100.0f * 0.6f;   // 0 = clear, 0.6 = smoky
    D2D1_GRADIENT_STOP glassBody[3] = {
        {0.0f, D2D1::ColorF(0.07f, 0.07f, 0.07f, tint * 0.85f)},
        {0.5f, D2D1::ColorF(0.04f, 0.04f, 0.04f, tint)},
        {1.0f, D2D1::ColorF(0.06f, 0.06f, 0.06f, tint * 0.9f)},
    };
    D2D1_GRADIENT_STOP siriBody[3] = {
        {0.0f, D2D1::ColorF(0.05f, 0.05f, 0.06f, 0.80f)},
        {0.55f, D2D1::ColorF(0.11f, 0.11f, 0.13f, 0.54f)},
        {1.0f, D2D1::ColorF(0.30f, 0.29f, 0.34f, 0.26f)},
    };
    // Dark (macOS dark material): neutral graphite over the live blur
    D2D1_GRADIENT_STOP darkBody[3] = {
        {0.0f, D2D1::ColorF(0.16f, 0.16f, 0.17f, 0.74f)},
        {0.5f, D2D1::ColorF(0.12f, 0.12f, 0.13f, 0.72f)},
        {1.0f, D2D1::ColorF(0.11f, 0.11f, 0.12f, 0.74f)},
    };
    // OLED: deep black (pixels off on OLED screens), nothing behind shows through
    float oledA = 1.0f - transparency / 100.0f * 0.6f;   // 0 % = solid black; more = black glass
    D2D1_GRADIENT_STOP oledBody[3] = {
        {0.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, oledA)}, {0.5f, D2D1::ColorF(0.0f, 0.0f, 0.0f, oledA)}, {1.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, oledA)},
    };
    // Glass transparency also thins out the Light, Siri and Dark glass (80 % = the theme as designed, 100 % = clear)
    float density = std::clamp((100 - transparency) / 20.0f, 0.0f, 2.5f);
    for (auto* stops : {lightBody, siriBody, darkBody})
        for (int i = 0; i < 3; i++) stops[i].color.a = std::min(1.0f, stops[i].color.a * density);
    const D2D1_GRADIENT_STOP* body = theme == Theme::Light ? lightBody : theme == Theme::Glass ? glassBody : theme == Theme::Dark ? darkBody
                                   : theme == Theme::Oled ? oledBody : siriBody;
    if (auto brush = MakeGradient(body, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)))
        g_target->FillRectangle(r, brush.Get());
    const bool optics = theme == Theme::Siri || (g_settings.glassEffect && (theme == Theme::Light || theme == Theme::Dark || theme == Theme::Oled));
    // How strong the glass light is: softer on the light theme (white on white) and on OLED (keeps the black deep)
    const float ok = theme == Theme::Light ? 0.7f : theme == Theme::Oled ? 0.6f : 1.0f;
    if (optics) {
        // Glass optics over the tint: a sheen on the upper part, light bouncing back along the bottom,
        // and the thickness of the glass as a faint bright band just inside the edge
        float sheenH = std::min(h * 0.42f, 34 * s);
        D2D1_GRADIENT_STOP sheen[3] = {{0.0f, D2D1::ColorF(1, 1, 1, 0.18f * ok)}, {0.45f, D2D1::ColorF(1, 1, 1, 0.05f * ok)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.0f)}};
        if (auto top = MakeGradient(sheen, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + sheenH)))
            g_target->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + sheenH), top.Get());
        float bounceH = std::min(h * 0.30f, 22 * s);
        D2D1_GRADIENT_STOP bounce[2] = {{0.0f, D2D1::ColorF(0.85f, 0.88f, 1.0f, 0.0f)}, {1.0f, D2D1::ColorF(0.85f, 0.88f, 1.0f, 0.09f * ok)}};
        if (auto bottom = MakeGradient(bounce, 2, D2D1::Point2F(0, r.bottom - bounceH), D2D1::Point2F(0, r.bottom)))
            g_target->FillRectangle(D2D1::RectF(r.left, r.bottom - bounceH, r.right, r.bottom), bottom.Get());
        float tw = 3.5f * s, ti = tw / 2 + 1.2f * s;
        g_brush->SetColor(D2D1::ColorF(1, 1, 1, 0.09f * ok));
        g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + ti, r.top + ti, r.right - ti, r.bottom - ti),
                                                         std::max(0.0f, radius - ti), std::max(0.0f, radius - ti)), g_brush.Get(), tw);
    }
    if (theme == Theme::Dark) {
        float glowH = std::min(h * 0.30f, 22 * s);
        D2D1_GRADIENT_STOP glow[2] = {{0.0f, D2D1::ColorF(1, 1, 1, 0.06f)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.0f)}};
        if (auto top = MakeGradient(glow, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.top + glowH)))
            g_target->FillRectangle(D2D1::RectF(r.left, r.top, r.right, r.top + glowH), top.Get());
    }
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
    if ((theme == Theme::Dark || theme == Theme::Oled) && !optics) {
        // A fine edge: light on top for Dark; a faint gray outline for OLED, so it stands apart from a black wallpaper
        D2D1_GRADIENT_STOP darkRim[3] = {{0.0f, D2D1::ColorF(1, 1, 1, 0.22f)}, {0.35f, D2D1::ColorF(1, 1, 1, 0.08f)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.12f)}};
        D2D1_GRADIENT_STOP oledRim[2] = {{0.0f, D2D1::ColorF(1, 1, 1, 0.16f)}, {1.0f, D2D1::ColorF(1, 1, 1, 0.10f)}};
        auto edge = theme == Theme::Dark ? MakeGradient(darkRim, 3, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom))
                                         : MakeGradient(oledRim, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom));
        if (edge) {
            float bw = std::max(1.0f, 1.0f * s), half = bw / 2, rr = std::max(0.0f, radius - half);
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), edge.Get(), bw);
        }
        return;
    }
    if (optics) {
        // Light caught by a beveled glass edge: strongest at the top-left and bottom-right corners
        // (on the light theme the edge also needs some gray to show against the white)
        float rk = theme == Theme::Light ? 1.0f : ok;
        D2D1_COLOR_F ink = theme == Theme::Light ? D2D1::ColorF(0.55f, 0.57f, 0.62f) : D2D1::ColorF(1, 1, 1);
        D2D1_GRADIENT_STOP bevel[5] = {
            {0.0f, theme == Theme::Light ? D2D1::ColorF(1, 1, 1, 0.95f) : Alpha(ink, 0.80f * rk)}, {0.28f, Alpha(ink, 0.20f * rk)},
            {0.55f, Alpha(ink, 0.10f * rk)}, {0.82f, Alpha(ink, 0.22f * rk)}, {1.0f, Alpha(ink, 0.50f * rk)},
        };
        float w = r.right - r.left, side = std::min(w, h);
        if (auto edge = MakeGradient(bevel, 5, D2D1::Point2F(r.left, r.top), D2D1::Point2F(r.left + side * 1.5f, r.top + side))) {
            float bw = std::max(1.0f, 1.1f * s), half = bw / 2, rr = std::max(0.0f, radius - half);
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half), rr, rr), edge.Get(), bw);
        }
        return;
    }
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
                DrawBackdropPicture(backdrop, W, H);
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

void DrawGhSearchBox(const Layout& L);
void DrawTabs(const Layout& L) {
    if (g_siriTab == 4) { DrawGhSearchBox(L); return; }   // GitHub gallery: a search field for the icon names
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

bool g_glassClassOk;

void CreateGlassWindow(HINSTANCE instance) {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = GlassWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = kGlassClass;
    if (!RegisterClassExW(&wc)) { Wh_Log(L"Glass window class failed (%u)", GetLastError()); return; }
    g_glassClassOk = true;
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

RECT g_glassPlaced{};   // where the glass window was last put (moved again only when this changes)
void HideGlass() {
    if (g_glassWnd && IsWindowVisible(g_glassWnd)) ShowWindow(g_glassWnd, SW_HIDE);
    g_glassPlaced = RECT{};
}

struct GlassPiece { D2D1_RECT_F r; float radius; };

// The shapes the live blur goes under (window coordinates, after the opening animation transform)
void CollectGlassPieces(const Layout& L, const D2D1::Matrix3x2F& m, std::vector<GlassPiece>& out, float& alpha) {
    float s = L.scale;
    alpha = 1.0f;
    if (g_settings.siri) {
        float ia = PillIntroAlpha(), pa = std::min(1.0f, g_siriAnim * 4);
        if (ia > 0.02f) out.push_back({L.search, (L.search.bottom - L.search.top) / 2});
        if (g_cfg.active) CfgCollectGlass(out);
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
    } else if (g_cfg.active) {
        if (CfgWindowT() < 1.0f) out.push_back({CfgWindowRect(), CfgWindowRadius()});
        out.push_back({L.search, (L.search.bottom - L.search.top) / 2});
        CfgCollectGlass(out);
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
    RECT place{L.area.left, L.area.top, L.area.left + L.width, L.area.top + L.height + L.extraBottom};
    if (!EqualRect(&place, &g_glassPlaced) || !IsWindowVisible(g_glassWnd)) {
        SetWindowPos(g_glassWnd, g_wnd, L.area.left, L.area.top, L.width, L.height + L.extraBottom,
                     SWP_NOACTIVATE | (IsWindowVisible(g_wnd) ? SWP_SHOWWINDOW : 0));
        if (IsWindowVisible(g_glassWnd)) g_glassPlaced = place;
    }
}

void GhScrolled();
void Render() {
    if (!g_shown || !EnsureTarget()) return;
    GhScrolled();
    const Layout& L = g_layout;
    if (!EnsureSurface(g_surface, L.width, L.height + L.extraBottom)) return;
    RECT bind{0, 0, L.width, L.height + L.extraBottom};
    if (FAILED(g_target->BindDC(g_surface.dc, &bind))) return;
    float W = (float)L.width, H = (float)L.height;
    float radius = g_settings.cornerRadius * L.scale;

    g_target->BeginDraw();
    g_target->SetTransform(D2D1::IdentityMatrix());
    g_target->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));   // fully transparent outside the shapes
    if (g_settings.fullscreen && !g_cfg.active) Fill(D2D1::RectF(0, 0, W, H), D2D1::ColorF(0, 0, 0, 1 / 255.0f));
    AnimFx fx = ComputeAnim();
    g_target->SetTransform(fx.m);

    if (g_settings.siri) {
        if (g_cfg.active) DrawCfgBehind();   // the sections come out from under the bar
        DrawSiriShapes(L);
    }
    const bool windowBg = !g_settings.siri && !g_cfg.active;   // with the settings open the window is folded away
    const bool themed = windowBg && ActiveTheme() != Theme::Custom;
    if (themed) {
        // Window mode keeps a margin so the theme shadow fits inside the window
        float m = WindowInset(L.scale);
        DrawThemedShape(D2D1::RectF(m, m, W - m, H - m), std::max(0.0f, radius - m * 0.25f), GetBackdropBitmap(), !g_settings.fullscreen);
    }
    if (!themed && BlurStrength() > 0 && windowBg) {
        if (ID2D1Bitmap* bitmap = GetBackdropBitmap()) {
            ComPtr<ID2D1RoundedRectangleGeometry> clipGeo;
            g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), radius, radius), &clipGeo);
            g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clipGeo.Get()), nullptr);
            DrawBackdropPicture(bitmap, W, H);
            g_target->PopLayer();
        }
    }

    if (windowBg && !themed) {
        g_brush->SetColor(Alpha(g_settings.tint, g_settings.opacity));
        g_target->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(0, 0, W, H), radius, radius), g_brush.Get());
        DrawGlass(D2D1::RectF(0, 0, W, H), radius);
    }

    if (!g_settings.fullscreen && windowBg && !themed && g_settings.borderWidth > 0) {
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
            DrawMainGrid(g_mainLayout, g_mainItems, g_mainScroll, bgFade, false, kFolderBase + g_openFolderIndex, t);
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
    } else if (g_settings.siri && g_cfg.active) {
        g_target->SetTransform(zoomMatrix);
        if (CfgCloseU() > 0) DrawCfgClosing(); else DrawCfg();
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
        DrawMoreButton();
        DrawGear();
        if (g_barDrag.active) {
            float r = (L.search.bottom - L.search.top) / 2;
            Fill(L.search, Fg(0.07f), r);
            g_brush->SetColor(Fg(0.35f));
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(L.search, r, r), g_brush.Get(), 1.2f * L.scale);
        }
    } else {
        g_target->SetTransform(zoomMatrix);
        if (g_cfg.active) {
            // Settings inside the window: the search field becomes the settings search on the left
            DrawCfgWindow();
            DrawCfgBehind();
            DrawCfgSearchField();
            if (CfgCloseU() > 0) DrawCfgClosing(); else DrawCfg();
            DrawPopupMenu(fx.m);
            g_target->SetTransform(D2D1::IdentityMatrix());
            goto present;
        }
        DrawSearchBar(L, 1.0f);
        DrawPowerButton(L, 1.0f);
        DrawMoreButton();
        float te = TabsVisible() ? Ease(g_tabsAnim) : 0.0f;
        if (te > 0.001f) { PushOpacity(te); DrawTabs(L); g_target->PopLayer(); }
        DrawMainGrid(L, g_layoutItems, g_scroll, 1.0f, true, -1, 0.0f);
    }
    DrawPopupMenu(fx.m);
    if (fx.flash > 0.001f && !g_settings.siri) { g_target->SetTransform(fx.m); Fill(D2D1::RectF(0, 0, W, H), D2D1::ColorF(1, 1, 1, fx.flash), radius); }
    g_target->SetTransform(D2D1::Matrix3x2F::Identity());

present:
    HRESULT drawn = g_target->EndDraw();
    if (drawn == D2DERR_RECREATE_TARGET) { ResetTarget(); return; }   // the next frame makes a new target
    const float windowAlpha = std::clamp(fx.alpha, 0.0f, 1.0f);
    POINT origin{L.area.left, L.area.top}, fromCorner{};
    SIZE extent{L.width, L.height + L.extraBottom};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, (BYTE)std::lround(windowAlpha * 255), AC_SRC_ALPHA};
    UpdateLayeredWindow(g_wnd, nullptr, &origin, &extent, g_surface.dc, &fromCorner, 0, &blend, ULW_ALPHA);
    UpdateGlass(L, fx.m, windowAlpha);
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
        if (g_mainItems[i] == kFolderBase + folderIndex) { layoutIdx = i; break; }
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
    g_cfg.active = false; g_cfg.closing = false; g_gear = GearState{};
    GhClose(false);
    KillTimer(g_wnd, kTimerLiquid); FreeLiquid();
    if (g_cfg.pendingReload) { g_cfg.pendingReload = false; LoadSettings(); }   // a mode picked in the settings panel
}

void OpenCategoryMenu();
void FitCategoryMenu();

void OnFrame() {
    const double now = Now();
    const float dt = (float)std::min(0.05, std::max(0.0, now - g_lastFrame));   // after a stall, animations don't jump
    g_lastFrame = now;
    bool animating = false;
    // Window / Fullscreen: opening (0 -> 1) and closing (1 -> 0) animation; Siri has its own
    const float appearGoal = g_closing ? 0.0f : 1.0f;
    if (g_settings.siri) g_appear = 1.0f;
    else if (g_appear != appearGoal) {
        float step = dt * g_settings.animSpeed / AnimSeconds(g_closing ? g_settings.closeAnim : g_settings.openAnim, g_closing);
        g_appear = std::clamp(g_appear + (g_closing ? -step : step), 0.0f, 1.0f);
        animating = g_appear != appearGoal;
    }
    if (g_scroll != g_scrollTarget && !g_swiping) {
        if (g_layout.horizontal) {
            // Page turns: a fixed, quick glide that settles crisply (macOS-like), instead of a long slow tail
            static float from, to, last;
            static double start;
            // a new target, or the position was moved meanwhile (a swipe): the glide starts again from here
            if (to != g_scrollTarget || g_scroll != last || now - start > 1.0) { from = g_scroll; to = g_scrollTarget; start = now; }
            float dur = std::max(0.08f, 0.32f / g_settings.animSpeed), t = std::clamp((float)(now - start) / dur, 0.0f, 1.0f);
            float e = 1 - std::pow(1 - t, 3.0f);
            g_scroll = t >= 1 ? g_scrollTarget : Lerp(from, to, e);
            last = g_scroll;
        } else {
            float k = 18.0f * g_settings.animSpeed;
            g_scroll += (g_scrollTarget - g_scroll) * (1.0f - std::exp(-dt * k));   // same feel at any refresh rate
            if (std::abs(g_scrollTarget - g_scroll) < 0.5f) g_scroll = g_scrollTarget;
        }
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

    const bool cfgMoved = CfgStep(dt);   // settings opening / closing (every mode)
    if (cfgMoved) { animating = true; if (!g_settings.siri) ComputeLayout(); }
    {
        float gearTarget = (g_gear.shown || g_cfg.active) && !g_siriClosingPill && !g_closing ? 1.0f : 0.0f;
        if (g_gear.anim != gearTarget) {
            float step = dt * g_settings.animSpeed / 0.24f;
            g_gear.anim = gearTarget > g_gear.anim ? std::min(gearTarget, g_gear.anim + step) : std::max(gearTarget, g_gear.anim - step);
            animating = true;
        }
    }
    if (g_settings.siri) {
        bool relayout = cfgMoved;
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
            // Original Siri: the mouse already resting on the bar opens the categories as soon as the bar is complete
            if (g_pillIntro >= kIntroCircle && g_settings.siriOriginal && !g_siriExpanded && !g_menu.open && !g_closing && !g_gear.shown) {
                POINT p;
                GetCursorPos(&p);
                ScreenToClient(g_wnd, &p);
                if (InRect(g_layout.search, (float)p.x, (float)p.y)) OpenCategoryMenu();
            }
        }
        if (g_flyAnim < 1.0f) {
            g_flyAnim = std::min(1.0f, g_flyAnim + dt * g_settings.animSpeed / 0.42f);
            animating = true;
        }
        {   // the results box glides to the width of the category picked (snaps while it is closed)
            float target = std::round(SiriPanelSettingW(g_siriTab) * g_layout.scale);
            if (g_siriAnim <= 0.001f || g_siriPanelWShown <= 0) g_siriPanelWShown = target;
            else if (g_siriPanelWShown != target) {
                float k = 1.0f - std::exp(-dt * g_settings.animSpeed * 14.0f);
                g_siriPanelWShown += (target - g_siriPanelWShown) * k;
                if (std::fabs(target - g_siriPanelWShown) < 0.5f) g_siriPanelWShown = target;
                relayout = true;
                animating = true;
            }
        }
        float btnTarget = g_settings.siriPowerButton && !g_siriClosingPill && g_pillIntro >= 1.0f ? 1.0f : 0.0f;
        if (g_siriBtnAnim != btnTarget) {
            float step = dt * g_settings.animSpeed / 0.34f;
            g_siriBtnAnim = btnTarget > g_siriBtnAnim ? std::min(btnTarget, g_siriBtnAnim + step) : std::max(btnTarget, g_siriBtnAnim - step);
            relayout = true;
            animating = true;
        }
        // Files: the results box only comes out once something is typed (empty = just the bar);
        // Clipboard: only when something has been copied
        bool panelOpen = g_siriExpanded && !(g_siriTab == 1 && g_query.empty()) && !(g_siriTab == 2 && g_clipHistory.empty());
        float expandTarget = panelOpen ? 1.0f : 0.0f;
        if (g_siriAnim != expandTarget) {
            float step = dt * g_settings.animSpeed / (panelOpen ? 0.34f : 0.24f);
            g_siriAnim = panelOpen ? std::min(1.0f, g_siriAnim + step) : std::max(0.0f, g_siriAnim - step);
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
        if (relayout) { ComputeLayout(); FitCategoryMenu(); }
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

// Windows only gives the foreground to a process that just had input: an empty input event first
void ForceForeground(HWND hwnd) {
    INPUT nothing{};
    SendInput(1, &nothing, sizeof(nothing));
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

void PlaceWindows() {
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (g_settings.fullscreen) RaiseDockWindows();
}

// The monitor the bar was dragged to (by its device name), if it is still connected
HMONITOR BarMonitor() {
    WCHAR wanted[CCHDEVICENAME] = {};
    Wh_GetStringValue(L"barMonitor", wanted, ARRAYSIZE(wanted));
    if (!wanted[0]) return nullptr;
    struct Find { const WCHAR* name; HMONITOR found; } find{wanted, nullptr};
    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR m, HDC, LPRECT, LPARAM data) -> BOOL {
        auto* f = reinterpret_cast<Find*>(data);
        MONITORINFOEXW info{};
        info.cbSize = sizeof(info);
        if (GetMonitorInfoW(m, &info) && wcscmp(info.szDevice, f->name) == 0) { f->found = m; return FALSE; }
        return TRUE;
    }, (LPARAM)&find);
    return find.found;
}

void SetArea(Layout& L, const RECT& area) {
    L.area = area;
    L.width = area.right - area.left;
    L.height = area.bottom - area.top;
}

// Where the Siri window sits on a monitor
RECT SiriAreaFor(HMONITOR monitor, float sc) {
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    int workWidth = mi.rcWork.right - mi.rcWork.left, workHeight = mi.rcWork.bottom - mi.rcWork.top;
    float barW = g_settings.siriPillW * sc + (g_settings.siriPowerButton ? 2 * (SiriButtonGap(sc) + SiriButtonSize(sc)) : 0.0f);
    float boxW = SiriMaxPanelW() * sc;   // the widest category box fits
    if (g_settings.siriPowerButton && !g_settings.siriPower) boxW += 2 * (12 + 230) * sc;   // the power menu opens beside the box
    int wantW = (int)std::lround(std::max(barW, boxW) + 2 * SiriShadowMargin(sc));
    int winWidth = std::min(workWidth, wantW);
    int winHeight = std::min(workHeight, SiriWindowHeight(sc));
    RECT area;
    area.left = mi.rcWork.left + (workWidth - winWidth) / 2;
    int topPad = (int)SiriTopPad(sc);
    area.top = std::max(mi.rcWork.top, std::min(mi.rcWork.top + workHeight * 16 / 100 - topPad, mi.rcWork.bottom - winHeight));
    g_siriFlip = false;
    g_siriShiftX = 0;
    if (g_barPosX >= 0 && g_barPosY >= 0) {
        // Where the bar was left (its center and top). The bar itself stays on the screen; the window around it stays
        // on the screen too, so near a side the bar sits off the window's middle, and low on the screen the results
        // box opens above it.
        int pillW = (int)std::lround(std::min((float)winWidth, g_settings.siriPillW * sc)), pillH = (int)std::lround(g_settings.siriPillH * sc);
        int barCenter = mi.rcWork.left + workWidth * g_barPosX / 1000, barTop = mi.rcWork.top + workHeight * g_barPosY / 1000;
        barCenter = std::clamp(barCenter, (int)mi.rcWork.left + pillW / 2, std::max((int)mi.rcWork.left + pillW / 2, (int)mi.rcWork.right - pillW / 2));
        barTop = std::clamp(barTop, (int)mi.rcWork.top, std::max((int)mi.rcWork.top, (int)mi.rcWork.bottom - pillH));
        int minX = mi.rcWork.left, maxX = std::max(minX, (int)mi.rcWork.right - winWidth);
        area.left = std::clamp(barCenter - winWidth / 2, minX, maxX);
        g_siriShiftX = std::clamp((float)(barCenter - area.left) - winWidth / 2.0f, -(winWidth - pillW) / 2.0f, (winWidth - pillW) / 2.0f);
        int below = barTop - topPad, above = barTop + pillH + topPad - winHeight;
        if (below + winHeight <= mi.rcWork.bottom || above < mi.rcWork.top) area.top = below;
        else { area.top = above; g_siriFlip = true; }
    }
    area.right = area.left + winWidth;
    area.bottom = area.top + winHeight;
    return area;
}

// How light the screen is in a rectangle (0 black ... 1 white): an averaged copy of what is shown there
float ScreenLuminance(const RECT& rc) {
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    if (w < 2 || h < 2) return -1;
    constexpr int n = 12;
    HDC screen = GetDC(nullptr);
    HDC mem = screen ? CreateCompatibleDC(screen) : nullptr;
    void* bits = nullptr;
    HBITMAP bmp = mem ? NewDib(mem, n, n, &bits) : nullptr;
    float lum = -1;
    if (bmp && bits) {
        HGDIOBJ old = SelectObject(mem, bmp);
        SetStretchBltMode(mem, HALFTONE);
        if (StretchBlt(mem, 0, 0, n, n, screen, rc.left, rc.top, w, h, SRCCOPY)) {
            GdiFlush();
            const UINT32* p = static_cast<const UINT32*>(bits);
            double sum = 0;
            for (int i = 0; i < n * n; i++) {
                UINT32 c = p[i];
                sum += (0.2126 * ((c >> 16) & 0xFF) + 0.7152 * ((c >> 8) & 0xFF) + 0.0722 * (c & 0xFF)) / 255.0;
            }
            lum = (float)(sum / (n * n));
        }
        SelectObject(mem, old);
    }
    if (bmp) DeleteObject(bmp);
    if (mem) DeleteDC(mem);
    if (screen) ReleaseDC(nullptr, screen);
    return lum;
}

// Automatic text color on clear glass: the background where the launcher is about to show (it is still hidden)
void UpdateAutoTextColor() {
    g_autoDarkText = -1;
    if (g_settings.menuText < 0 && ActiveTheme() == Theme::Glass) {
        float lum = ScreenLuminance(g_layout.area);
        if (lum >= 0) g_autoDarkText = lum > 0.55f ? 1 : 0;
    }
    if (g_autoDarkText >= 0) g_settings.darkText = g_autoDarkText == 1;   // LoadSettings keeps it when settings change
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
    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY);   // it opens where the mouse is
    if (g_settings.siri && g_barPosX >= 0)
        if (HMONITOR placed = BarMonitor()) monitor = placed;   // ...or on the monitor the bar was dragged to
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    Layout& L = g_layout;
    if (g_settings.siri) {
        L.area = SiriAreaFor(monitor, dpiX / 96.0f);
    } else if (g_settings.fullscreen) {
        L.area = AvailableArea(monitor);
    } else {
        MONITORINFO mi{sizeof(mi)}; GetMonitorInfoW(monitor, &mi);
        int workWidth = mi.rcWork.right - mi.rcWork.left, workHeight = mi.rcWork.bottom - mi.rcWork.top;
        int winWidth = (workWidth * g_settings.windowWidth) / 100, winHeight = (workHeight * g_settings.windowHeight) / 100;
        L.area.left = mi.rcWork.left + (workWidth - winWidth) / 2; L.area.top = mi.rcWork.top + (workHeight - winHeight) / 2;
        L.area.right = L.area.left + winWidth; L.area.bottom = L.area.top + winHeight;
    }
    SetArea(L, L.area);
    L.scale = dpiX / 96.0f;
    EnsureFormats(L.scale);
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
    // Liquid glass: the picture behind is taken now, before the launcher shows (or retaken live, see kTimerLiquid)
    SetWindowDisplayAffinity(g_wnd, LiquidWanted() && g_settings.liveGlass ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    if (LiquidWanted()) {
        MONITORINFO mi{sizeof(mi)};
        GetMonitorInfoW(monitor, &mi);
        RECT cap = mi.rcMonitor;
        if (g_settings.liveGlass) InflateRect(&(cap = L.area), kLiquidMargin, kLiquidMargin);
        if (!CaptureLiquid(cap)) FreeLiquid();
        if (g_settings.liveGlass) SetTimer(g_wnd, kTimerLiquid, 33, nullptr);
    } else FreeLiquid();
    if (BlurStrength() > 0)
        CaptureBackdrop(L.area, BlurStrength() / 100.0f * 60 * L.scale);
    UpdateAutoTextColor();   // before the launcher shows: what is behind it now
    g_shown = true; g_closing = false; g_appear = g_settings.siri ? 1.0f : 0.0f; Render(); ShowWindow(g_wnd, SW_SHOW);
    ForceForeground(g_wnd); PlaceWindows(); Render(); StartFrames();
    // The app list is refreshed in the background at most every 5 minutes
    if (!g_appsLoaded || Now() - g_appsTime > 300 || g_appsIconPixels != IconPixels()) StartLoadingApps();
}

void Close() {
    if (!g_shown || g_closing) return;
    if (g_cfg.active) CfgAbort();
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
        if (IsFolderItem(item)) { OpenFolder(item - kFolderBase); return; }
        ap = ResolveApp(item);
    }
    if (!ap) return;
    if (ap->kind == 2) { CopyText(ap->target); Close(); return; }
    if (ap->kind == 5) { GhApply(ap->target); return; }   // GitHub gallery: this picture becomes the app's icon
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
    const Layout& L = g_layout;
    if (index < 0 || L.columns <= 0) return;
    float target = g_scrollTarget;
    const int page = index / std::max(1, L.perPage), row = index / L.columns;
    if (L.horizontal) target = page * L.pageWidth;
    else {
        float top = row * L.cellHeight, view = L.visibleRows * L.cellHeight;
        if (top < target) target = top;
        else if (top + L.cellHeight > target + view) target = top + L.cellHeight - view;
    }
    g_scrollTarget = std::clamp(target, 0.0f, MaxScroll());
}

void ScrollRows(int rows) {
    const Layout& L = g_layout;
    if (L.horizontal) { GoToPage(CurrentPage() + (rows > 0 ? 1 : rows < 0 ? -1 : 0)); return; }
    float row = std::round(g_scrollTarget / L.cellHeight);   // always lands on a whole row
    g_scrollTarget = std::clamp((row + rows) * L.cellHeight, 0.0f, MaxScroll());
    StartFrames();
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
    if (g_siriTab == 4) GhQueueVisible();
}

void QueryChanged() {
    UpdatePowerPill();
    if (g_settings.siri && g_siriTab == 1) StartFrames();   // Files: the box opens / closes with the text
    if (g_openFolderIndex >= 0) FinishFolderClose();
    if (!g_settings.siri && !g_query.empty() && !g_searchActive) { g_searchActive = true; StartFrames(); }
    ScheduleFileSearch();
    RefreshTabItems(true);
}

void SetSiriTab(int tab) {
    if (g_siriTab == 4) {   // leaving the GitHub gallery for a category
        GhClose(false);
        if (tab == 0) { RefreshTabItems(true); StartFrames(); return; }
    }
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

// Formats that password managers and other apps set to keep an item out of clipboard history (clipboard must be open)
bool ExcludedFromHistory() {
    static const UINT exclude = RegisterClipboardFormatW(L"ExcludeClipboardContentFromMonitorProcessing");
    static const UINT viewerIgnore = RegisterClipboardFormatW(L"Clipboard Viewer Ignore");
    static const UINT canInclude = RegisterClipboardFormatW(L"CanIncludeInClipboardHistory");
    if (IsClipboardFormatAvailable(exclude) || IsClipboardFormatAvailable(viewerIgnore)) return true;
    bool excluded = false;
    if (HANDLE h = GetClipboardData(canInclude); h && GlobalSize(h) >= sizeof(DWORD)) {
        if (auto* v = (const DWORD*)GlobalLock(h)) { excluded = *v == 0; GlobalUnlock(h); }
    }
    return excluded;
}

// When each copy entered the history (for "how long a copy stays")
[[clang::no_destroy]] std::unordered_map<std::wstring, double> g_clipTimes;
// Copies older than the chosen time leave the history; returns true if something left
bool PruneClipHistory() {
    if (g_settings.clipKeepSec <= 0 || g_clipHistory.empty()) return false;
    double now = Now();
    size_t before = g_clipHistory.size();
    g_clipHistory.erase(std::remove_if(g_clipHistory.begin(), g_clipHistory.end(), [&](const std::wstring& t) {
        auto f = g_clipTimes.find(t);
        return f != g_clipTimes.end() && now - f->second > g_settings.clipKeepSec;
    }), g_clipHistory.end());
    for (auto it = g_clipTimes.begin(); it != g_clipTimes.end();)
        it = std::find(g_clipHistory.begin(), g_clipHistory.end(), it->first) == g_clipHistory.end() ? g_clipTimes.erase(it) : std::next(it);
    return g_clipHistory.size() != before;
}
void RemoveClipItem(const std::wstring& text) {
    g_clipHistory.erase(std::remove(g_clipHistory.begin(), g_clipHistory.end(), text), g_clipHistory.end());
    g_clipTimes.erase(text);
    if (g_shown && g_siriTab == 2) { RefreshTabItems(false); StartFrames(); }
}
// Empties the history and what is on the clipboard right now
void ClearClipboardAll() {
    g_clipHistory.clear(); g_clipTimes.clear();
    if (OpenClipboard(g_wnd)) { EmptyClipboard(); CloseClipboard(); }
    if (g_shown && g_siriTab == 2) { RefreshTabItems(true); StartFrames(); }
}

constexpr UINT_PTR kTimerClipRetry = 10;
int g_clipRetries;
void OnClipboardUpdate() {
    if (g_shown && g_settings.siri && g_siriTab == 2) StartFrames();   // the box can come out now
    if (!g_settings.clipEnabled) return;
    std::wstring text = ReadClipboardText(20000, true);
    if (g_clipReadResult == 1) {
        // The app that copied still holds the clipboard: try again a little later (a few times)
        if (g_clipRetries++ < 8) SetTimer(g_wnd, kTimerClipRetry, 120, nullptr);
        else { g_clipRetries = 0; Wh_Log(L"Clipboard: still busy, skipped"); }   // the next copy gets its own tries
        return;
    }
    g_clipRetries = 0;
    if (g_clipReadResult == 2) return;   // marked as secret by the app that copied (passwords): not kept
    if (text.find_first_not_of(L" \t\r\n") == std::wstring::npos) return;   // no text (an image, files...)
    auto it = std::find(g_clipHistory.begin(), g_clipHistory.end(), text);
    if (it != g_clipHistory.end()) g_clipHistory.erase(it);
    g_clipHistory.insert(g_clipHistory.begin(), text);
    g_clipTimes[text] = Now();
    if ((int)g_clipHistory.size() > g_settings.clipMax) g_clipHistory.resize(g_settings.clipMax);
    PruneClipHistory();
    if (g_shown && g_siriTab == 2) RefreshTabItems(false);
}

std::wstring ReadClipboardLine() {
    std::wstring line = ReadClipboardText(100000);
    return line.substr(0, line.find_first_of(L"\r\n"));
}

void PasteText() {
    constexpr size_t kMaxQuery = 200;
    ClampQueryCaret();
    DeleteQuerySelection();
    std::wstring text = ReadClipboardLine();
    size_t room = g_query.size() < kMaxQuery ? kMaxQuery - g_query.size() : 0;
    if (text.size() > room) text.resize(room);
    g_query.insert(g_queryCaret, text);
    g_queryCaret += text.size();
    g_queryAnchor = g_queryCaret;
    QueryChanged();
}

void CopyQueryToClipboard() {
    size_t a = QueryHasSelection() ? QuerySelStart() : 0;
    size_t b = QueryHasSelection() ? QuerySelEnd() : g_query.size();
    if (a >= b || !OpenClipboard(g_wnd)) return;
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
    if (g_gear.shown) { g_gear.shown = false; g_gear.hover = false; StartFrames(); }
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
    int count = (int)g_layoutItems.size();
    if (count == 0) return;
    int next = g_selected < 0 ? 0 : g_selected + delta;
    if (g_selected >= 0 && next >= count && delta == g_layout.columns) next = count - 1;   // down from the last, shorter row
    if (next >= 0 && next < count) g_selected = next;
    EnsureVisible(g_selected);
    Render();
    StartFrames();
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
    if (key == VK_ESCAPE && g_gear.shown) { g_gear.shown = false; g_gear.hover = false; StartFrames(); return; }
    if (key == VK_ESCAPE && g_siriTab == 4 && g_query.empty()) { GhClose(true); return; }   // leaves the GitHub gallery
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
        // Actions (sign out, shut down...) only run from a click or Enter, never from Ctrl+digit
        if (n < (int)g_layoutItems.size()) {
            App* target = ResolveApp(g_layoutItems[n]);
            if (!target || target->kind != 4) LaunchApp(g_layoutItems[n]);
        }
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
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN: {
            int columns = g_layout.columns;
            MoveSelection(key == VK_LEFT ? -1 : key == VK_RIGHT ? 1 : key == VK_UP ? -columns : columns);
            break;
        }
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
    std::stable_sort(g_apps.begin(), g_apps.end(), AppNameLess);
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
        if (app && g_openFolderIndex < 0 && item < kFolderBase && g_selection.size() > 1 && g_selection.count(app)) {
            for (auto& a : g_apps) if (g_selection.count(a.get())) PinToTaskbar(a->target);
        } else if (app) PinToTaskbar(app->target);
        Render(); return;
    }

    // Folders are made only in Window and Fullscreen: the Siri modes show every app in one list (Original Siri uses tabs)
    if (g_siriTab != 0 || g_settings.siri) { g_dragItem = -1; return; }

    if (g_openFolderIndex >= 0) {
        g_dragItem = -1;
        if (FolderSettled() && !InRect(g_layout.panel, g_dragX, g_dragY))
            MoveAppOutOfFolder(g_layoutItems[fromIdx]);
        return;
    }

    int toIdx = ItemAt((int)g_dragX, (int)g_dragY);
    int fromItem = g_layoutItems[fromIdx];
    g_dragItem = -1;
    if (toIdx < 0 || toIdx == fromIdx || fromItem >= kFolderBase) return;
    int toItem = g_layoutItems[toIdx];
    if (IsFolderApp(toItem) || IsTabItem(toItem)) return;
    App* dragged = ResolveApp(fromItem);
    if (!dragged) return;
    std::unordered_set<App*> moving;
    if (g_selection.count(dragged)) moving = g_selection; else moving.insert(dragged);

    if (toItem < kFolderBase) {
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
        auto& dst = g_folders[toItem - kFolderBase].apps;
        dst.insert(dst.end(), taken.begin(), taken.end());
    }
    g_selection.clear();
    SaveFolders();
    RebuildLayout(); ComputeLayout(); Render();
}

////////////////////////////////////////////////////////////////////////////////
// System Hooks & Window Proc

bool OwnsTaskbar() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD owner = 0;
    if (taskbar) GetWindowThreadProcessId(taskbar, &owner);
    return owner != 0 && owner == GetCurrentProcessId();
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void*, int, int);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;

// Windows calls ShowStartView with method 1 and flag 0x8 for the Windows key; anything else is the Start button
bool OpenedByWinKey(int method, int flags) { return method == 1 && (flags & 0x8) != 0; }
HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis, int showMethod, int showFlags) {
    HWND launcher = g_wnd;
    bool ours = launcher && (OpenedByWinKey(showMethod, showFlags) ? g_winKeyEnabled.load() : g_startButtonEnabled.load());
    if (!ours) return XamlLauncher_ShowStartView_Original(pThis, showMethod, showFlags);   // Windows' own Start menu
    PostMessageW(launcher, WM_APP_TOGGLE, 0, 0);
    return S_OK;
}

void OnAppsLoaded(AppList* list, int iconPixels) {
    LoadUserAssist();
    if (!list->empty()) UpdateKnownApps(*list);
    if (Picking()) { g_appFilter = g_pickerPrevFilter; g_pickerChip = kNoChip; }   // the picked apps were the old list
    if (!g_menu.category) g_menu.open = false;   // a context menu points to the old list; the categories menu can stay
    g_selection.clear();
    if (!list->empty()) ReconcileFolders(*list);
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
float g_categoryMenuW;
void FitCategoryMenu();

void OpenCategoryMenu() {
    // While the power options are showing (button or typed "shut down"), the bar does not offer the categories
    if (g_powerPillWanted || g_powerPillAnim > 0.01f) return;
    const Layout& L = g_layout;
    float s = L.scale;
    std::vector<MenuItem> items;
    for (int i = 0; i < g_tabCount && i < 4; i++) {
        int id = g_tabIds[i];
        int menuId = id == 0 ? kMenuTabApps : id == 1 ? kMenuTabFiles : id == 3 ? kMenuTabActions : kMenuTabClipboard;
        items.push_back({TabLabel(id), menuId, false, false, TabGlyph(id), g_shortcuts[id].valid ? g_shortcutLabels[id].c_str() : nullptr});
    }
    g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
    OpenMenu(L.search.left, L.search.bottom + 10 * s, std::move(items), false, 0);
    g_categoryMenuW = g_menu.rect.right - g_menu.rect.left;   // natural width; the menu is never narrower than the bar
    g_menu.category = true;
    g_menu.keyboard = false;
    if (!g_trackingMouse) {   // also when it opens without a mouse move (bar completed under the cursor)
        TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, g_wnd, 0};
        g_trackingMouse = TrackMouseEvent(&tme);
    }
    // macOS-like: taller rows, larger icons and text
    g_menu.itemH = std::round(48 * s);
    g_menu.rect.bottom = g_menu.rect.top + 16 * s + g_menu.items.size() * g_menu.itemH;
    FitCategoryMenu();
}

// The categories menu hangs under the bar; while the bar is still growing (opening animation) it follows it
void FitCategoryMenu() {
    if (!g_menu.open || !g_menu.category) return;
    const Layout& L = g_layout;
    float s = L.scale, barW = L.search.right - L.search.left, cx = (L.search.left + L.search.right) / 2;
    float w = std::max(barW, g_categoryMenuW), h = g_menu.rect.bottom - g_menu.rect.top;
    float left = barW >= g_categoryMenuW ? L.search.left : cx - w / 2;
    float top = g_siriFlip ? L.search.top - 10 * s - h : L.search.bottom + 10 * s;   // above the bar when it sits low
    g_menu.rect = D2D1::RectF(left, top, left + w, top + h);
    g_menu.origin = D2D1::Point2F(cx, g_siriFlip ? L.search.top : L.search.bottom);
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
    if (id >= kMenuCfgBase && id < kMenuCfgBase + kCfgItemCount * 32) { CfgChoose(id - kMenuCfgBase); return; }
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
            for (int it : g_layoutItems) if (it >= 0 && it < kFolderBase && it < (int)g_apps.size()) g_selection.insert(g_apps[it].get());
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
        case kMenuClipRemove:
            if (histIdx >= 0 && histIdx < (int)g_layoutItems.size())
                if (App* h = ResolveApp(g_layoutItems[histIdx]); h && h->kind == 2) RemoveClipItem(h->target);
            break;
        case kMenuClipClear:
            ClearClipboardAll();
            break;
        case kMenuHistClear:
            g_searchHistory.clear(); SaveSearchHistory(); RefreshTabItems(true);
            break;
        case kMenuChangeIcon: {
            std::wstring path;
            if (app && PickImage(g_wnd, path)) {
                if (LoadImageFile(path, CurrentIconPixels(), *app)) { g_customIcons[app->target] = path; SaveCustomIcons(); }
                else MessageBoxW(g_wnd, Tr(L"Could not load this image."), L"Siri Plus Launchpad", MB_ICONWARNING);
            }
            if (g_shown) { ForceForeground(g_wnd); Render(); }
            break;
        }
        case kMenuBarCenter: {
            g_barPosX = g_barPosY = -1;
            Wh_SetStringValue(L"barMonitor", L"");
            Wh_SetIntValue(L"barX", -1);
            Wh_SetIntValue(L"barY", -1);
            SetArea(g_layout, SiriAreaFor(MonitorFromWindow(g_wnd, MONITOR_DEFAULTTONEAREST), g_layout.scale));
            ComputeLayout();
            Render();
            break;
        }
        case kMenuGhIcon:
            GhOpen(app);
            break;
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

// ---------- Original Siri: the category shortcuts (Ctrl+...) also work with the launcher closed ----------
int MatchCategoryShortcut(UINT vk) {
    auto down = [](int k) { return (GetAsyncKeyState(k) & 0x8000) != 0; };
    if (!down(VK_CONTROL) || down(VK_LWIN) || down(VK_RWIN) || down(VK_MENU)) return -1;
    bool shift = down(VK_SHIFT);
    for (int tab = 0; tab < 4; tab++) {
        const Shortcut& sc = g_shortcuts[tab];
        if (sc.valid && sc.vk == vk && sc.shift == shift) return tab;
    }
    return -1;
}

LRESULT CALLBACK KeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
    const KBDLLHOOKSTRUCT* k = (const KBDLLHOOKSTRUCT*)lParam;
    // With the launcher in front, its own keyboard handling takes the shortcuts
    if (code != HC_ACTION || !k || (k->flags & LLKHF_INJECTED) || !g_wnd || !g_settings.siriOriginal || !g_shortcutsAnywhere ||
        GetForegroundWindow() == g_wnd)
        return CallNextHookEx(nullptr, code, wParam, lParam);
    bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    bool up = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
    if (g_swallowVk && k->vkCode == g_swallowVk && (down || up)) {
        if (up) g_swallowVk = 0;
        return 1;
    }
    if (down) {
        int tab = MatchCategoryShortcut(k->vkCode);
        if (tab >= 0) {
            g_swallowVk = k->vkCode;
            PostMessageW(g_wnd, WM_APP_SHORTCUT, (WPARAM)tab, 0);
            return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Opens the launcher right on that category (pressed again: closes it)
void OnCategoryShortcut(int tab) {
    if (!g_settings.siriOriginal || tab < 0 || tab > 3 || !TabEnabled(tab)) return;
    if (g_shown && !g_closing && g_siriExpanded && g_siriTab == tab) { Close(); return; }
    if (!g_shown || g_closing) Open();
    if (g_menu.open) CloseMenu();
    g_siriChosen = true;
    if (g_siriTab != tab) SetSiriTab(tab);
    ExpandSiri();
    Render();
    StartFrames();
}

////////////////////////////////////////////////////////////////////////////////
// Settings panel: layout, drawing and input


D2D1_COLOR_F CfgRgb(UINT32 rgb, float a = 1.0f) { return D2D1::ColorF(rgb, a); }   // 0xRRGGBB
D2D1_COLOR_F CfgAccent() { return CfgRgb(g_settings.darkText ? 0x007AFF : 0x0A84FF); }

[[clang::no_destroy]] ComPtr<IDWriteTextFormat> g_cfgTitle, g_cfgHead, g_cfgBody, g_cfgDesc, g_cfgHeadDesc, g_cfgValue, g_cfgCaption,
    g_cfgBar, g_cfgButton, g_cfgGlyphS, g_cfgGlyphM, g_cfgGlyphL, g_cfgGlyphXS;
float g_cfgFormatScale;

void CfgFormats(float scale) {
    if (g_cfgBody && g_cfgFormatScale == scale) return;
    g_cfgFormatScale = scale;
    auto make = [&](PCWSTR family, float size, DWRITE_FONT_WEIGHT weight, DWRITE_TEXT_ALIGNMENT align, bool wrap, ComPtr<IDWriteTextFormat>& format) {
        format = OneLineFormat(family, size * scale, weight, align);
        if (format && wrap) {   // descriptions: several lines from the top
            format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            format->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
            DWRITE_TRIMMING none{};
            none.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;
            format->SetTrimming(&none, nullptr);
        }
    };
    PCWSTR text = L"Segoe UI Variable Text", display = L"Segoe UI Variable Display";
    make(display, 21.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, false, g_cfgTitle);
    make(display, 15.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, false, g_cfgHead);
    make(text, 13.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, false, g_cfgBody);
    make(text, 11.5f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, true, g_cfgDesc);
    make(text, 12.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, true, g_cfgHeadDesc);
    make(text, 13.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_TRAILING, false, g_cfgValue);
    make(text, 12.0f, DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_TEXT_ALIGNMENT_LEADING, false, g_cfgCaption);
    make(text, 15.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_LEADING, false, g_cfgBar);
    make(text, 13.0f, DWRITE_FONT_WEIGHT_MEDIUM, DWRITE_TEXT_ALIGNMENT_CENTER, false, g_cfgButton);
    PCWSTR icons = L"Segoe Fluent Icons";
    ComPtr<IDWriteFontCollection> fonts;
    UINT32 index = 0;
    BOOL exists = FALSE;
    if (SUCCEEDED(g_dwrite->GetSystemFontCollection(&fonts)) && fonts) fonts->FindFamilyName(icons, &index, &exists);
    if (!exists) icons = L"Segoe MDL2 Assets";
    make(icons, 13.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, false, g_cfgGlyphS);
    make(icons, 16.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, false, g_cfgGlyphM);
    make(icons, 22.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, false, g_cfgGlyphL);
    make(icons, 9.0f, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_TEXT_ALIGNMENT_CENTER, false, g_cfgGlyphXS);
}

void CfgText(const std::wstring& text, IDWriteTextFormat* format, const D2D1_RECT_F& r, D2D1_COLOR_F color) {
    if (!format || text.empty()) return;
    g_brush->SetColor(color);
    g_target->DrawText(text.c_str(), (UINT32)text.size(), format, r, g_brush.Get());
}
float CfgTextWidth(const std::wstring& text, IDWriteTextFormat* format) {
    ComPtr<IDWriteTextLayout> layout;
    if (!format || FAILED(g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(), format, 4000, 200, &layout))) return 0;
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    return m.widthIncludingTrailingWhitespace;
}
float CfgTextHeight(const std::wstring& text, IDWriteTextFormat* format, float width) {
    ComPtr<IDWriteTextLayout> layout;
    if (!format || FAILED(g_dwrite->CreateTextLayout(text.c_str(), (UINT32)text.size(), format, std::max(10.0f, width), 2000, &layout))) return 0;
    DWRITE_TEXT_METRICS m{};
    layout->GetMetrics(&m);
    return std::ceil(m.height);
}

// Animation phases: the bar moves aside, the sections come out of the bar, the panel comes out of the gear
float CfgBarT() { return EaseInOut(std::clamp(g_cfg.anim / 0.30f, 0.0f, 1.0f)); }
float CfgSideT() { return std::clamp((g_cfg.anim - 0.30f) / 0.30f, 0.0f, 1.0f); }
float CfgPanelT() { return std::clamp((g_cfg.anim - 0.55f) / 0.45f, 0.0f, 1.0f); }
bool CfgSettled() { return g_cfg.active && !g_cfg.closing && g_cfg.anim >= 1.0f; }
float CfgRadius() { return std::clamp(g_settings.siriPanelRadius * g_layout.scale, 6 * g_layout.scale, 30 * g_layout.scale); }

// Gear: round glass button at the right end of the bar
D2D1_RECT_F GearRect() {
    const D2D1_RECT_F& b = g_layout.search;
    float h = b.bottom - b.top, d = std::round(h * 0.66f), m = (h - d) / 2;
    return D2D1::RectF(b.right - m - d, b.top + m, b.right - m, b.bottom - m);
}

// Where the Siri window sits on a monitor (also used when the settings close)
RECT SiriAreaFor(HMONITOR monitor, float sc);

// The bar's home (window coordinates of the work area) for a Siri window area
D2D1_RECT_F CfgHomeBar(const RECT& area, const RECT& work, float s) {
    float W = (float)(area.right - area.left);
    float pillW = std::min(W, std::round(g_settings.siriPillW * s)), pillH = std::round(g_settings.siriPillH * s);
    float cx = (area.left - work.left) + W / 2 + g_siriShiftX, top = (area.top - work.top) + SiriTopPad(s);
    if (g_siriFlip) top = (area.bottom - work.top) - SiriTopPad(s) - pillH;   // bar at the bottom of the window
    return D2D1::RectF(cx - pillW / 2, top, cx + pillW / 2, top + pillH);
}

// ---------- Rows ----------
struct CfgRow { int item; D2D1_RECT_F r, ctl; float labelH, descH; };
struct CfgCaption { int section; D2D1_RECT_F r; };
struct CfgSideRow { int section; D2D1_RECT_F r; };
[[clang::no_destroy]] std::vector<CfgRow> g_cfgRows;
[[clang::no_destroy]] std::vector<D2D1_RECT_F> g_cfgCards;
[[clang::no_destroy]] std::vector<CfgCaption> g_cfgCaptions;
[[clang::no_destroy]] std::vector<CfgSideRow> g_cfgSideRows;
D2D1_RECT_F g_cfgHeader{}, g_cfgView{};
bool g_cfgNothing;

std::wstring CfgFold(std::wstring s) {
    s = ToLower(s);
    static const wchar_t from[] = L"áàâãäéèêëíìîïóòôõöúùûüçñß", to[] = L"aaaaaeeeeiiiiooooouuuucns";
    for (auto& c : s)
        for (int i = 0; from[i]; i++) if (c == from[i]) { c = to[i]; break; }
    return s;
}
bool CfgMatches(const CfgItem& it) {
    if (g_cfg.search.empty()) return it.section == g_cfg.section;
    std::wstring q = CfgFold(g_cfg.search);
    auto has = [&](PCWSTR t) { return t && CfgFold(t).find(q) != std::wstring::npos; };
    const CfgSection& sec = kCfgSections[it.section];
    return has(it.en) || has(it.pt) || has(it.descEn) || has(it.descPt) || has(sec.en) || has(sec.pt) ||
           has(CfgT(it.en, it.pt)) || has(CfgT(it.descEn, it.descPt)) || has(CfgT(sec.en, sec.pt));
}
bool CfgSectionHasMatch(int section) {
    for (const CfgItem& it : kCfgItems) if (it.section == section && CfgMatches(it)) return true;
    return false;
}

const std::wstring& CfgValue(const CfgItem& it) {
    static const std::wstring empty;
    auto found = g_cfgValues.find(it.key);
    return found == g_cfgValues.end() ? empty : found->second;
}
std::wstring CfgOptionLabel(const CfgItem& it, const std::wstring& value) {
    for (int k = 0; k < it.optCount; k++)
        if (value == it.opts[k].value) return CfgT(it.opts[k].en, it.opts[k].pt);
    return it.optCount ? CfgT(it.opts[0].en, it.opts[0].pt) : value;
}
std::wstring CfgSliderText(const CfgItem& it, int v) {
    if (wcscmp(it.key, L"grid.columns") == 0 && v == 0) return CfgT(L"Auto", L"Auto");
    if (v == 0 && wcsstr(it.key, L"siri.") == it.key && wcsstr(it.key, L"Width")) return CfgT(L"As the bar", L"Igual à barra");
    std::wstring t = std::to_wstring(v);
    if (it.unit) t += (it.unit[0] == L'%' ? L"" : L" ") + std::wstring(it.unit);
    return t;
}
std::wstring CfgShortcutText(const CfgItem& it) {
    if (g_cfg.capture >= 0 && &kCfgItems[g_cfg.capture] == &it) return CfgT(L"Press the keys…", L"Pressione…");
    const std::wstring& v = CfgValue(it);
    return v.empty() ? std::wstring(CfgT(L"None", L"Nenhum")) : v;
}
PCWSTR CfgButtonText(const CfgItem& it) {
    if (!wcscmp(it.key, L"#export")) return CfgT(L"Save…", L"Salvar…");
    if (!wcscmp(it.key, L"#import")) return CfgT(L"Open…", L"Abrir…");
    if (!wcscmp(it.key, L"#themeReset")) return CfgT(L"Restore", L"Restaurar");
    return CfgT(L"Restore…", L"Restaurar…");
}
bool CfgThemeChanged();
bool CfgButtonEnabled(const CfgItem& it) { return wcscmp(it.key, L"#themeReset") || CfgThemeChanged(); }

float CfgControlWidth(const CfgItem& it, float s) {
    switch (it.type) {
        case kCfgToggle: return std::round(40 * s);
        case kCfgSlider: return std::round(214 * s);
        case kCfgChoice: return std::clamp(CfgTextWidth(CfgOptionLabel(it, CfgValue(it)), g_cfgBody.Get()) + 40 * s, 96 * s, 250 * s);
        case kCfgColor: return std::round(128 * s);
        case kCfgText: return std::round(236 * s);
        case kCfgShortcut: return std::round(124 * s);
        case kCfgButton: return CfgTextWidth(CfgButtonText(it), g_cfgButton.Get()) + 32 * s;
    }
    return 0;
}

// Positions of everything in the panel (final panel rect, scrolled)
void CfgBuild() {
    g_cfgRows.clear(); g_cfgCards.clear(); g_cfgCaptions.clear(); g_cfgSideRows.clear();
    float s = g_layout.scale;
    CfgFormats(s);
    const D2D1_RECT_F& P = g_cfg.panel;
    float pad = std::round(22 * s), titleH = std::round(34 * s);
    g_cfg.done = D2D1::RectF(P.right - pad - std::round(96 * s), P.top + pad, P.right - pad, P.top + pad + std::round(30 * s));
    g_cfgView = D2D1::RectF(P.left, P.top + pad + titleH + std::round(10 * s), P.right, P.bottom - std::round(10 * s));
    float cardL = P.left + pad, cardR = P.right - pad, in = std::round(14 * s);
    float y = g_cfgView.top + std::round(4 * s) - std::round(g_cfg.scroll);
    float startY = y;
    g_cfgHeader = D2D1::RectF(0, 0, 0, 0);
    if (g_cfg.search.empty()) {
        const CfgSection& sec = kCfgSections[g_cfg.section];
        float textW = cardR - cardL - in - std::round(56 * s) - in;
        float dh = CfgTextHeight(CfgT(sec.descEn, sec.descPt), g_cfgHeadDesc.Get(), textW);
        float h = std::max(std::round(68 * s), std::round(16 * s) + std::round(20 * s) + dh + std::round(16 * s));
        g_cfgHeader = D2D1::RectF(cardL, y, cardR, y + h);
        y += h + std::round(14 * s);
    }
    auto addCard = [&](int section) {
        float top = y;
        for (int i = 0; i < kCfgItemCount; i++) {
            const CfgItem& it = kCfgItems[i];
            if (it.section != section || !CfgMatches(it)) continue;
            float cw = CfgControlWidth(it, s);
            float labelW = cardR - cardL - 2 * in - cw - std::round(16 * s);
            PCWSTR desc = CfgT(it.descEn, it.descPt);
            float labelH = std::round(19 * s), descH = desc ? CfgTextHeight(desc, g_cfgDesc.Get(), labelW) : 0;
            float h = std::max(std::round(46 * s), std::round(12 * s) + labelH + (desc ? std::round(2 * s) + descH : 0) + std::round(12 * s));
            D2D1_RECT_F r = D2D1::RectF(cardL, y, cardR, y + h);
            float ch = it.type == kCfgToggle ? std::round(22 * s) : it.type == kCfgSlider ? std::round(24 * s)
                     : it.type == kCfgButton ? std::round(28 * s) : std::round(26 * s);
            float cy = (r.top + r.bottom) / 2;
            D2D1_RECT_F ctl = D2D1::RectF(cardR - in - cw, std::round(cy - ch / 2), cardR - in, std::round(cy - ch / 2) + ch);
            g_cfgRows.push_back({i, r, ctl, labelH, descH});
            y += h;
        }
        if (y > top) g_cfgCards.push_back(D2D1::RectF(cardL, top, cardR, y));
    };
    g_cfgNothing = false;
    if (g_cfg.search.empty()) addCard(g_cfg.section);
    else {
        for (int sec = 0; sec < kCfgSectionCount; sec++) {
            if (!CfgSectionHasMatch(sec)) continue;
            g_cfgCaptions.push_back({sec, D2D1::RectF(cardL + std::round(4 * s), y, cardR, y + std::round(26 * s))});
            y += std::round(26 * s);
            addCard(sec);
            y += std::round(14 * s);
        }
        g_cfgNothing = g_cfgRows.empty();
    }
    g_cfg.contentH = y - startY + std::round(16 * s);
    // Sections (sidebar)
    const D2D1_RECT_F& S = g_cfg.side;
    float rowH = std::round(36 * s), sy = S.top + std::round(10 * s);
    for (int sec = 0; sec < kCfgSectionCount; sec++) {
        if (!g_cfg.search.empty() && !CfgSectionHasMatch(sec)) continue;
        g_cfgSideRows.push_back({sec, D2D1::RectF(S.left + std::round(8 * s), sy, S.right - std::round(8 * s), sy + rowH - std::round(2 * s))});
        sy += rowH;
    }
}

float CfgMaxScroll() { return std::max(0.0f, g_cfg.contentH - (g_cfgView.bottom - g_cfgView.top)); }

// The settings take the whole work area: bar + sections on the left, the floating panel on the right
// Window / Fullscreen closing, once the sections are back in the bar: 0 -> 1 while the bar shrinks into its gear
// (turning) and the gear is sucked into its own center. 0 at any other time
// (Siri modes: the same, once the bar is back home)
float CfgCloseU() {
    if (!g_cfg.active || !g_cfg.closing) return 0.0f;
    return g_cfg.siriMode ? g_cfg.endT : 1.0f - std::clamp(g_cfg.anim / 0.30f, 0.0f, 1.0f);
}
constexpr float kCfgShrinkEnd = 0.62f;   // share of it spent shrinking into the button; the rest is the suck
D2D1_RECT_F CfgClosingBar(const D2D1_RECT_F& bar) {
    float u = CfgCloseU(), h = bar.bottom - bar.top;
    if (u <= 0) return bar;
    // The left end slides to the button: only the round button is left, at the right end
    float a = EaseInOut(std::clamp(u / kCfgShrinkEnd, 0.0f, 1.0f));
    D2D1_RECT_F r = D2D1::RectF(Lerp(bar.left, bar.right - h, a), bar.top, bar.right, bar.bottom);
    if (u <= kCfgShrinkEnd) return r;
    // Then the button is sucked into its own center, faster and faster
    float v = (u - kCfgShrinkEnd) / (1 - kCfgShrinkEnd), k = std::max(0.0f, 1.0f - v * v);
    float cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2, d = h * k / 2;
    return D2D1::RectF(cx - d, cy - d, cx + d, cy + d);
}

void CfgApplyLayout() {
    Layout& L = g_layout;
    float s = L.scale, W = (float)L.width, H = (float)L.height;
    float pillH = std::round(g_settings.siriPillH * s), m = std::round(24 * s), gap = std::round(18 * s);
    float sideW = std::round(272 * s);
    float panelW = std::max(std::round(320 * s), std::min(std::round(700 * s), W - sideW - gap - 2 * m));
    float total = sideW + gap + panelW, left0 = std::round((W - total) / 2);
    float need = std::round(380 * s);
    float below = H - m - g_cfg.home.top, above = g_cfg.home.bottom - m;
    if (g_cfg.siriMode && below < need && above > below) {
        // The bar sits low on the screen: it stays there and the settings open upward, the sections above it and the
        // panel ending at its bottom edge
        float barTop = std::clamp(g_cfg.home.top, m, std::max(m, H - m - pillH));
        float bottom = barTop + pillH;
        float panelH = std::max(std::round(260 * s), std::min(std::round(640 * s), bottom - m));
        g_cfg.bar = D2D1::RectF(left0, barTop, left0 + sideW, bottom);
        g_cfg.side = D2D1::RectF(left0, bottom - panelH, left0 + sideW, barTop - std::round(10 * s));
        g_cfg.panel = D2D1::RectF(left0 + sideW + gap, bottom - panelH, left0 + total, bottom);
    } else {
        float top0 = std::clamp(g_cfg.home.top, m, std::max(m, H - need));
        float panelH = std::max(std::round(260 * s), std::min(std::round(640 * s), H - top0 - m));
        g_cfg.bar = D2D1::RectF(left0, top0, left0 + sideW, top0 + pillH);
        g_cfg.side = D2D1::RectF(left0, g_cfg.bar.bottom + std::round(10 * s), left0 + sideW, top0 + panelH);
        g_cfg.panel = D2D1::RectF(left0 + sideW + gap, top0, left0 + total, top0 + panelH);
    }
    // Window / Fullscreen closing: the bar stays in place (closeHome is where it is), then shrinks into its gear
    L.search = g_cfg.closing && !g_cfg.siriMode ? CfgClosingBar(g_cfg.bar) : LerpRect(g_cfg.home, g_cfg.bar, CfgBarT());
    if (g_cfg.closing && g_cfg.siriMode && g_cfg.endT > 0) L.search = CfgClosingBar(g_cfg.home);   // Siri: home, then into the gear
    L.btnPower = L.btnBack = L.panel = L.dots = L.siriPanel = L.siriPower = L.siriTabs = D2D1::RectF(0, 0, 0, 0);
    g_cfg.scroll = std::clamp(g_cfg.scroll, 0.0f, std::max(0.0f, g_cfg.contentH));
    CfgBuild();
    g_cfg.scroll = std::clamp(g_cfg.scroll, 0.0f, CfgMaxScroll());
}

// The sections list, coming out of the bar
D2D1_RECT_F CfgSideRect() {
    float s = g_layout.scale, e = Ease(CfgSideT());
    const D2D1_RECT_F& b = g_layout.search;
    D2D1_RECT_F from = D2D1::RectF(b.left + 18 * s, b.top + 4 * s, b.right - 18 * s, b.bottom - 4 * s);
    return LerpRect(from, g_cfg.side, e);
}
// The panel, coming out of the gear: a thin stream first, then it widens (a reversed "suck")
D2D1_RECT_F CfgPanelRect(float& radius) {
    float t = CfgPanelT();
    D2D1_RECT_F P = g_cfg.panel, G = GearRect();
    float gx = (G.left + G.right) / 2, gy = (G.top + G.bottom) / 2, gr = (G.right - G.left) / 2;
    float ey = Ease(std::clamp(t / 0.62f, 0.0f, 1.0f)), ex = EaseInOut(std::clamp((t - 0.12f) / 0.88f, 0.0f, 1.0f));
    radius = Lerp(gr, CfgRadius(), std::clamp(t * 1.6f, 0.0f, 1.0f));
    return D2D1::RectF(Lerp(gx - gr, P.left, ex), Lerp(gy - gr, P.top, ey), Lerp(gx + gr, P.right, ex), Lerp(gy + gr, P.bottom, ey));
}

struct GlassPiece;
void CfgCollectGlass(std::vector<GlassPiece>& out) {
    if (CfgSideT() > 0.02f) out.push_back({CfgSideRect(), CfgRadius()});
    if (CfgPanelT() > 0.02f) { float r = 0; D2D1_RECT_F p = CfgPanelRect(r); out.push_back({p, r}); }
}

// ---------- Drawing ----------
void CfgSurface(const D2D1_RECT_F& r, float rad, float alpha, bool tint) {
    if (alpha <= 0.001f || r.right - r.left < 2 || r.bottom - r.top < 2) return;
    float s = g_layout.scale;
    rad = std::min(rad, std::min(r.right - r.left, r.bottom - r.top) / 2);
    PushOpacity(alpha);
    if (ActiveTheme() != Theme::Custom) DrawThemedShape(r, rad, nullptr, true);
    else {
        DrawThemeShadow(r, rad, 0.22f, 12 * s, 5 * s);
        Fill(r, Alpha(g_settings.siriPanelColor, std::max(0.6f, g_settings.siriPanelOpacity)), rad);
        if (g_settings.borderWidth > 0) {
            float bw = g_settings.borderWidth * s, half = bw / 2;
            g_brush->SetColor(Alpha(g_settings.borderColor, g_settings.borderOpacity));
            g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + half, r.top + half, r.right - half, r.bottom - half),
                                                             std::max(0.0f, rad - half), std::max(0.0f, rad - half)), g_brush.Get(), bw);
        }
    }
    if (tint) Fill(r, g_settings.darkText ? D2D1::ColorF(1, 1, 1, 0.38f) : D2D1::ColorF(0.05f, 0.05f, 0.07f, 0.34f), rad);
    g_target->PopLayer();
}

void CfgIcon(int section, const D2D1_RECT_F& r, IDWriteTextFormat* glyph) {
    const CfgSection& sec = kCfgSections[section];
    float rad = (r.right - r.left) * 0.24f;
    D2D1_COLOR_F c = CfgRgb(sec.color);
    D2D1_GRADIENT_STOP stops[2] = {{0.0f, D2D1::ColorF(std::min(1.0f, c.r + 0.10f), std::min(1.0f, c.g + 0.10f), std::min(1.0f, c.b + 0.10f))}, {1.0f, c}};
    if (auto fill = MakeGradient(stops, 2, D2D1::Point2F(0, r.top), D2D1::Point2F(0, r.bottom)))
        g_target->FillRoundedRectangle(D2D1::RoundedRect(r, rad, rad), fill.Get());
    CfgText(sec.glyph, glyph, r, D2D1::ColorF(1, 1, 1, 1));
}

void DrawCfgSide() {
    float t = CfgSideT();
    if (t <= 0.0f) return;
    float s = g_layout.scale, rad = CfgRadius();
    D2D1_RECT_F cur = CfgSideRect();
    CfgSurface(cur, rad, std::min(1.0f, t * 3), false);
    float ca = std::clamp((t - 0.25f) / 0.75f, 0.0f, 1.0f);
    if (ca <= 0.001f) return;
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(cur, rad, rad), &clip);
    if (!clip) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), ca), nullptr);
    for (const CfgSideRow& row : g_cfgSideRows) {
        bool selected = g_cfg.search.empty() && row.section == g_cfg.section;
        // Concentric with the sidebar's corners (its radius minus the 8 px margin)
        float rowRad = std::max(4 * s, rad - 8 * s);
        if (selected) Fill(row.r, CfgAccent(), rowRad);
        else if (row.section == g_cfg.sideHover) Fill(row.r, Fg(0.09f), rowRad);
        float cy = (row.r.top + row.r.bottom) / 2, d = std::round(24 * s);
        D2D1_RECT_F icon = D2D1::RectF(row.r.left + 8 * s, cy - d / 2, row.r.left + 8 * s + d, cy + d / 2);
        CfgIcon(row.section, icon, g_cfgGlyphS.Get());
        const CfgSection& sec = kCfgSections[row.section];
        CfgText(CfgT(sec.en, sec.pt), g_cfgBody.Get(), D2D1::RectF(icon.right + 10 * s, row.r.top, row.r.right - 8 * s, row.r.bottom),
                selected ? D2D1::ColorF(1, 1, 1, 1) : Fg(0.92f));
    }
    g_target->PopLayer();
}

void DrawCfgToggle(const D2D1_RECT_F& c, bool on) {
    float h = c.bottom - c.top, r = h / 2;
    Fill(c, on ? CfgAccent() : Fg(0.20f), r);
    float d = h - 4 * g_layout.scale, cx = on ? c.right - 2 * g_layout.scale - d / 2 : c.left + 2 * g_layout.scale + d / 2, cy = (c.top + c.bottom) / 2;
    g_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.18f));
    g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy + 0.8f * g_layout.scale), d / 2, d / 2), g_brush.Get());
    g_brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
    g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), d / 2, d / 2), g_brush.Get());
}

D2D1_RECT_F CfgSliderTrack(const D2D1_RECT_F& c) {
    float s = g_layout.scale, cy = (c.top + c.bottom) / 2;
    return D2D1::RectF(c.left + std::round(62 * s), cy - 2 * s, c.right - 9 * s, cy + 2 * s);
}

void DrawCfgControl(const CfgRow& row, bool hover) {
    const CfgItem& it = kCfgItems[row.item];
    const D2D1_RECT_F& c = row.ctl;
    float s = g_layout.scale;
    const std::wstring& v = CfgValue(it);
    switch (it.type) {
        case kCfgToggle: DrawCfgToggle(c, _wtoi(v.c_str()) != 0); break;
        case kCfgSlider: {
            int value = std::clamp(_wtoi(v.c_str()), it.min, it.max);
            D2D1_RECT_F tr = CfgSliderTrack(c);
            float k = it.max > it.min ? (float)(value - it.min) / (it.max - it.min) : 0, x = Lerp(tr.left, tr.right, k);
            CfgText(CfgSliderText(it, value), g_cfgValue.Get(), D2D1::RectF(c.left, c.top, tr.left - 12 * s, c.bottom), Fg(0.60f));
            Fill(tr, Fg(0.18f), 2 * s);
            Fill(D2D1::RectF(tr.left, tr.top, x, tr.bottom), CfgAccent(), 2 * s);
            float kr = (row.item == g_cfg.drag ? 10.0f : 9.0f) * s, cy = (tr.top + tr.bottom) / 2;
            g_brush->SetColor(D2D1::ColorF(0, 0, 0, 0.22f));
            g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cy + 1 * s), kr, kr), g_brush.Get());
            g_brush->SetColor(D2D1::ColorF(1, 1, 1, 1));
            g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, cy), kr, kr), g_brush.Get());
            break;
        }
        case kCfgChoice: {
            Fill(c, Fg(hover ? 0.16f : 0.10f), 7 * s);
            CfgText(CfgOptionLabel(it, v), g_cfgBody.Get(), D2D1::RectF(c.left + 10 * s, c.top, c.right - 26 * s, c.bottom), Fg(0.92f));
            CfgText(L"", g_cfgGlyphXS.Get(), D2D1::RectF(c.right - 24 * s, c.top, c.right - 6 * s, c.bottom), Fg(0.70f));
            break;
        }
        case kCfgColor: {
            float d = c.bottom - c.top, cy = (c.top + c.bottom) / 2;
            D2D1_COLOR_F color = ParseColor(v, D2D1::ColorF(0.5f, 0.5f, 0.5f));
            g_brush->SetColor(color);
            g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c.left + d / 2, cy), d / 2 - 1 * s, d / 2 - 1 * s), g_brush.Get());
            g_brush->SetColor(Fg(0.35f));
            g_target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(c.left + d / 2, cy), d / 2 - 1 * s, d / 2 - 1 * s), g_brush.Get(), 1 * s);
            D2D1_RECT_F box = D2D1::RectF(c.left + d + 8 * s, c.top, c.right, c.bottom);
            bool editing = g_cfg.edit == row.item;
            Fill(box, Fg(editing ? 0.14f : hover ? 0.12f : 0.08f), 7 * s);
            if (editing) {
                g_brush->SetColor(CfgAccent());
                g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(box.left + 0.75f * s, box.top + 0.75f * s, box.right - 0.75f * s, box.bottom - 0.75f * s),
                                                                 7 * s, 7 * s), g_brush.Get(), 1.5f * s);
            }
            std::wstring text = editing ? g_cfg.editText : v;
            D2D1_RECT_F tr = D2D1::RectF(box.left + 10 * s, box.top, box.right - 8 * s, box.bottom);
            CfgText(text, g_cfgBody.Get(), tr, Fg(0.92f));
            if (editing) {
                float x = tr.left + CfgTextWidth(text, g_cfgBody.Get()) + 1 * s;
                g_brush->SetColor(Fg(0.9f));
                g_target->DrawLine(D2D1::Point2F(x, box.top + 6 * s), D2D1::Point2F(x, box.bottom - 6 * s), g_brush.Get(), 1.2f * s);
            }
            break;
        }
        case kCfgText: {
            bool editing = g_cfg.edit == row.item;
            Fill(c, Fg(editing ? 0.14f : hover ? 0.12f : 0.08f), 7 * s);
            if (editing) {
                g_brush->SetColor(CfgAccent());
                g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(c.left + 0.75f * s, c.top + 0.75f * s, c.right - 0.75f * s, c.bottom - 0.75f * s),
                                                                 7 * s, 7 * s), g_brush.Get(), 1.5f * s);
            }
            std::wstring text = editing ? g_cfg.editText : v;
            D2D1_RECT_F tr = D2D1::RectF(c.left + 10 * s, c.top, c.right - 8 * s, c.bottom);
            if (text.empty() && !editing) CfgText(CfgT(L"owner/repository", L"dono/repositório"), g_cfgBody.Get(), tr, Fg(0.40f));
            else {
                // While typing, the end of the text stays visible
                float w = CfgTextWidth(text, g_cfgBody.Get()), room = tr.right - tr.left;
                g_target->PushAxisAlignedClip(tr, D2D1_ANTIALIAS_MODE_ALIASED);
                float x = editing && w > room ? tr.right - w - 2 * s : tr.left;
                CfgText(text, g_cfgBody.Get(), D2D1::RectF(x, tr.top, x + w + 4 * s, tr.bottom), Fg(0.92f));
                if (editing) {
                    g_brush->SetColor(Fg(0.9f));
                    g_target->DrawLine(D2D1::Point2F(x + w + 1 * s, c.top + 6 * s), D2D1::Point2F(x + w + 1 * s, c.bottom - 6 * s), g_brush.Get(), 1.2f * s);
                }
                g_target->PopAxisAlignedClip();
            }
            break;
        }
        case kCfgShortcut: {
            bool capturing = g_cfg.capture == row.item;
            Fill(c, Fg(capturing ? 0.14f : hover ? 0.12f : 0.08f), 7 * s);
            if (capturing) {
                g_brush->SetColor(CfgAccent());
                g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(c.left + 0.75f * s, c.top + 0.75f * s, c.right - 0.75f * s, c.bottom - 0.75f * s),
                                                                 7 * s, 7 * s), g_brush.Get(), 1.5f * s);
            }
            CfgText(CfgShortcutText(it), g_cfgButton.Get(), c, Fg(capturing ? 0.65f : 0.92f));
            break;
        }
        case kCfgButton: {
            bool enabled = CfgButtonEnabled(it), danger = !wcscmp(it.key, L"#reset");
            Fill(c, Fg(enabled && hover ? 0.16f : 0.10f), 7 * s);
            CfgText(CfgButtonText(it), g_cfgButton.Get(), c, !enabled ? Fg(0.35f) : danger ? DangerInk() : Fg(0.92f));
            break;
        }
    }
}

void DrawCfgPanelContent() {
    float s = g_layout.scale;
    const D2D1_RECT_F& P = g_cfg.panel;
    float pad = std::round(22 * s);
    // Title and Done
    std::wstring title = g_cfg.search.empty() ? CfgT(kCfgSections[g_cfg.section].en, kCfgSections[g_cfg.section].pt)
                                              : std::wstring(CfgT(L"Results", L"Resultados"));
    CfgText(title, g_cfgTitle.Get(), D2D1::RectF(P.left + pad, P.top + pad, g_cfg.done.left - 12 * s, P.top + pad + std::round(30 * s)), Fg(0.95f));
    DrawGlassButton(g_cfg.done, (g_cfg.done.bottom - g_cfg.done.top) / 2, 1.0f, g_cfg.doneHover ? 1.7f : 1.15f, false);
    CfgText(CfgT(L"Done", L"Concluído"), g_cfgButton.Get(), g_cfg.done, Fg(0.95f));

    g_target->PushAxisAlignedClip(g_cfgView, D2D1_ANTIALIAS_MODE_ALIASED);
    if (g_cfgHeader.right > g_cfgHeader.left) {
        const CfgSection& sec = kCfgSections[g_cfg.section];
        Fill(g_cfgHeader, Fg(0.07f), 12 * s);
        float d = std::round(42 * s), cy = (g_cfgHeader.top + g_cfgHeader.bottom) / 2;
        D2D1_RECT_F icon = D2D1::RectF(g_cfgHeader.left + 14 * s, cy - d / 2, g_cfgHeader.left + 14 * s + d, cy + d / 2);
        CfgIcon(g_cfg.section, icon, g_cfgGlyphL.Get());
        float tx = icon.right + 14 * s, textW = g_cfgHeader.right - 14 * s - tx;
        PCWSTR desc = CfgT(sec.descEn, sec.descPt);
        float dh = CfgTextHeight(desc, g_cfgHeadDesc.Get(), textW), block = std::round(20 * s) + dh, top = cy - block / 2;
        CfgText(CfgT(sec.en, sec.pt), g_cfgHead.Get(), D2D1::RectF(tx, top, g_cfgHeader.right - 14 * s, top + std::round(20 * s)), Fg(0.95f));
        CfgText(desc, g_cfgHeadDesc.Get(), D2D1::RectF(tx, top + std::round(20 * s), tx + textW, top + block + 4 * s), Fg(0.58f));
    }
    for (const CfgCaption& c : g_cfgCaptions) {
        const CfgSection& sec = kCfgSections[c.section];
        CfgText(CfgT(sec.en, sec.pt), g_cfgCaption.Get(), c.r, Fg(0.55f));
    }
    for (const D2D1_RECT_F& card : g_cfgCards) Fill(card, Fg(0.07f), 12 * s);
    for (size_t i = 0; i < g_cfgRows.size(); i++) {
        const CfgRow& row = g_cfgRows[i];
        if (row.r.bottom < g_cfgView.top || row.r.top > g_cfgView.bottom) continue;
        const CfgItem& it = kCfgItems[row.item];
        float in = std::round(14 * s);
        if (i > 0 && g_cfgRows[i - 1].r.bottom == row.r.top) {
            g_brush->SetColor(Fg(0.10f));
            g_target->DrawLine(D2D1::Point2F(row.r.left + in, row.r.top + 0.5f), D2D1::Point2F(row.r.right - in, row.r.top + 0.5f), g_brush.Get(), 1.0f);
        }
        PCWSTR desc = CfgT(it.descEn, it.descPt);
        float block = row.labelH + (desc ? std::round(2 * s) + row.descH : 0);
        float top = (row.r.top + row.r.bottom) / 2 - block / 2;
        float labelRight = row.ctl.left - std::round(16 * s);
        CfgText(CfgT(it.en, it.pt), g_cfgBody.Get(), D2D1::RectF(row.r.left + in, top, labelRight, top + row.labelH), Fg(0.94f));
        if (desc)
            CfgText(desc, g_cfgDesc.Get(), D2D1::RectF(row.r.left + in, top + row.labelH + std::round(2 * s), labelRight, top + block + 4 * s), Fg(0.55f));
        DrawCfgControl(row, (int)i == g_cfg.hover);
    }
    if (g_cfgNothing)
        CfgText(CfgT(L"No setting found", L"Nenhum ajuste encontrado"), g_cfgBody.Get(),
                D2D1::RectF(P.left + pad, g_cfgView.top + 10 * s, P.right - pad, g_cfgView.top + 40 * s), Fg(0.55f));
    g_target->PopAxisAlignedClip();
    // Scroll indicator
    float maxScroll = CfgMaxScroll();
    if (maxScroll > 1) {
        float viewH = g_cfgView.bottom - g_cfgView.top, barH = std::max(30 * s, viewH * viewH / (viewH + maxScroll));
        float y = g_cfgView.top + (viewH - barH) * (g_cfg.scroll / maxScroll);
        Fill(D2D1::RectF(P.right - 8 * s, y, P.right - 4.5f * s, y + barH), Fg(0.22f), 2 * s);
    }
}

void DrawCfgPanel() {
    float t = CfgPanelT();
    if (t <= 0.0f) return;
    float rad = 0;
    D2D1_RECT_F cur = CfgPanelRect(rad);
    CfgSurface(cur, rad, std::min(1.0f, t * 4), true);
    float ca = std::clamp((t - 0.68f) / 0.32f, 0.0f, 1.0f);
    if (ca <= 0.001f) return;
    const D2D1_RECT_F& P = g_cfg.panel;
    float sx = (cur.right - cur.left) / std::max(1.0f, P.right - P.left), sy = (cur.bottom - cur.top) / std::max(1.0f, P.bottom - P.top);
    D2D1::Matrix3x2F base;
    g_target->GetTransform(&base);
    D2D1::Matrix3x2F m = D2D1::Matrix3x2F::Scale(sx, sy, D2D1::Point2F(P.left, P.top)) *
                         D2D1::Matrix3x2F::Translation(cur.left - P.left, cur.top - P.top) * base;
    ComPtr<ID2D1RoundedRectangleGeometry> clip;
    g_d2d->CreateRoundedRectangleGeometry(D2D1::RoundedRect(cur, rad, rad), &clip);
    if (!clip) return;
    g_target->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), clip.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), ca), nullptr);
    g_target->SetTransform(m);
    DrawCfgPanelContent();
    g_target->SetTransform(base);
    g_target->PopLayer();
}

// "..." at the right end of the bar (as on macOS 27): it opens the settings
bool g_moreHover;
bool L_SearchReady() { const D2D1_RECT_F& r = g_layout.search; return r.right - r.left > 60 && r.bottom - r.top > 10; }
bool MoreShown() {
    if (!g_shown || g_closing || g_cfg.active || g_openFolderIndex >= 0 || !g_query.empty() || Picking()) return false;
    if (g_settings.siri) return g_pillIntro >= 1.0f && !g_siriClosingPill;
    return g_appear >= 1.0f && L_SearchReady();   // Window / Fullscreen: once the window is open
}
bool HitMore(float x, float y) { return MoreShown() && InRect(GearRect(), x, y); }
void DrawMoreButton() {
    if (!MoreShown()) return;
    D2D1_RECT_F g = GearRect();
    float s = g_layout.scale, r = (g.right - g.left) / 2;
    D2D1_POINT_2F c = D2D1::Point2F((g.left + g.right) / 2, (g.top + g.bottom) / 2);
    if (g_moreHover) DrawGlassButton(g, r, 1.0f, 1.3f, false);
    g_brush->SetColor(Fg(g_moreHover ? 0.92f : 0.45f));
    float dr = std::max(1.2f, 1.7f * s), gap = 5.5f * s;
    for (int i = -1; i <= 1; i++) g_target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(c.x + i * gap, c.y), dr, dr), g_brush.Get());
}

void DrawGear() {
    float a = g_cfg.active ? 1.0f : g_gear.anim;
    if (a <= 0.001f || !g_glyphFormat) return;
    D2D1_RECT_F g = GearRect();
    float s = g_layout.scale, k = g_cfg.active ? 1.0f : BackOut(a);
    D2D1_POINT_2F c = D2D1::Point2F((g.left + g.right) / 2, (g.top + g.bottom) / 2);
    float r = (g.right - g.left) / 2 * k;
    if (r < 1) return;
    D2D1_RECT_F sr = D2D1::RectF(c.x - r, c.y - r, c.x + r, c.y + r);
    bool hover = g_cfg.active ? g_cfg.gearHover : g_gear.hover;
    DrawGlassButton(sr, r, std::min(1.0f, a * 1.5f), hover ? 1.8f : 1.25f, true);
    D2D1::Matrix3x2F base;
    g_target->GetTransform(&base);
    float angle = (g_cfg.active ? g_cfg.anim : 0.0f) * 180.0f + (1 - k) * -90.0f - CfgCloseU() * 360.0f;   // turns while the bar shrinks into it
    float gs = k * (r * 2 / (30 * s));   // the glyph follows the button size
    g_target->SetTransform(D2D1::Matrix3x2F::Scale(gs, gs, c) * D2D1::Matrix3x2F::Rotation(angle, c) * base);
    CfgText(L"", g_glyphFormat.Get(), D2D1::RectF(c.x - 15 * s, c.y - 15 * s, c.x + 15 * s, c.y + 15 * s), Fg(hover ? 1.0f : 0.88f));
    g_target->SetTransform(base);
}

// The bar while the settings are open: it becomes the settings search
void DrawCfgBar() {
    const D2D1_RECT_F& b = g_layout.search;
    float s = g_layout.scale, h = b.bottom - b.top, cy = (b.top + b.bottom) / 2;
    D2D1_RECT_F glyph = D2D1::RectF(b.left + h * 0.22f, cy - 14 * s, b.left + h * 0.22f + 28 * s, cy + 14 * s);
    CfgText(L"", g_cfgGlyphM.Get(), glyph, Fg(0.70f));
    D2D1_RECT_F text = D2D1::RectF(glyph.right + 8 * s, b.top, GearRect().left - 8 * s, b.bottom);
    if (g_cfg.search.empty())
        CfgText(CfgT(L"Search settings", L"Pesquisar ajustes"), g_cfgBar.Get(), text, Fg(0.50f));
    else {
        CfgText(g_cfg.search, g_cfgBar.Get(), text, Fg(0.95f));
        float x = std::min(text.right, text.left + CfgTextWidth(g_cfg.search, g_cfgBar.Get()) + 1 * s);
        g_brush->SetColor(Fg(0.9f));
        g_target->DrawLine(D2D1::Point2F(x, cy - 10 * s), D2D1::Point2F(x, cy + 10 * s), g_brush.Get(), 1.3f * s);
    }
}

// Window / Fullscreen: the window folds into the search field while the bar moves aside (and unfolds when closing)
// (it stays folded while the settings close: the launcher then fades away from the search field, never reopening the window)
float CfgWindowT() { return g_cfg.siriMode || g_cfg.closing ? 1.0f : Ease(std::clamp(g_cfg.anim / 0.30f, 0.0f, 1.0f)); }
D2D1_RECT_F CfgWindowRect() { return LerpRect(g_cfg.window, g_layout.search, CfgWindowT()); }
float CfgWindowRadius() {
    const D2D1_RECT_F& b = g_layout.search;
    return Lerp(g_cfg.fullscreen ? 0.0f : g_settings.cornerRadius * g_layout.scale, (b.bottom - b.top) / 2, CfgWindowT());
}
void DrawCfgWindow() {
    float t = CfgWindowT();
    if (t >= 1.0f) return;
    D2D1_RECT_F r = CfgWindowRect();
    float rad = CfgWindowRadius();
    PushOpacity(1 - t * t);
    if (ActiveTheme() != Theme::Custom) DrawThemedShape(r, rad, nullptr, !g_cfg.fullscreen);
    else {
        Fill(r, Alpha(g_settings.tint, g_settings.opacity), rad);
        DrawGlass(r, rad);
    }
    g_target->PopLayer();
}

// Window / Fullscreen: the search field the window folded into, in the chosen theme (as the sections and the panel)
void DrawCfgSearchField() {
    const D2D1_RECT_F& b = g_layout.search;
    float rad = std::min(b.bottom - b.top, b.right - b.left) / 2, t = CfgWindowT();
    if (t <= 0.001f) return;
    PushOpacity(t);
    if (ActiveTheme() != Theme::Custom) DrawThemedShape(b, rad, nullptr, true);
    else {
        Fill(b, Alpha(g_settings.tint, g_settings.opacity), rad);
        DrawGlass(b, rad);
    }
    g_target->PopLayer();
}

// Window / Fullscreen closing: the text leaves first; the gear stays (turning) until it is sucked in
void DrawCfgClosing() {
    CfgFormats(g_layout.scale);
    DrawCfgPanel();
    float ta = 1.0f - std::clamp(CfgCloseU() / 0.25f, 0.0f, 1.0f);
    if (ta > 0.001f) { PushOpacity(ta); DrawCfgBar(); g_target->PopLayer(); }
    DrawGear();
}

void DrawCfgBehind() { DrawCfgSide(); }
void DrawCfg() {
    CfgFormats(g_layout.scale);
    DrawCfgPanel();
    DrawCfgBar();
    DrawGear();
}

// ---------- Changing values ----------
void CfgApplySettings() {
    LoadSettings();
    // A new mode is used the next time the launcher opens: until the settings close, they keep the current one
    bool modeChanged = g_settings.siri != g_cfg.siriMode || g_settings.siriOriginal != g_cfg.siriOriginal || g_settings.fullscreen != g_cfg.fullscreen;
    // What would add or remove pieces around the bar (the floating power button) also waits for the next opening:
    // the launcher closes the way it was opened
    bool buttonChanged = g_settings.siriPowerButton != g_cfg.powerButton;
    if (buttonChanged) g_settings.siriPowerButton = g_cfg.powerButton;
    g_cfg.pendingReload = modeChanged || buttonChanged;
    if (modeChanged) {
        g_settings.siri = g_cfg.siriMode; g_settings.siriOriginal = g_cfg.siriOriginal; g_settings.fullscreen = g_cfg.fullscreen;
        BuildTabs();
    }
    if (!g_settings.clipEnabled) g_clipHistory.clear();
    else if ((int)g_clipHistory.size() > g_settings.clipMax) g_clipHistory.resize(g_settings.clipMax);
    ComputeLayout();
    Render();
}

void CfgSet(const CfgItem& it, const std::wstring& value, bool save = true) {
    if (!g_cfgOverridesLoaded) LoadCfgOverrides();
    if (!wcscmp(it.key, L"appearance.transparency")) {   // kept for the active theme only
        if (!g_themeTransparencyLoaded) LoadThemeTransparency();
        g_themeTransparency[ThemeName(g_settings.theme)] = std::clamp(_wtoi(value.c_str()), 0, 100);
        if (save) SaveThemeTransparency();
        g_cfgValues[it.key] = value;
        CfgApplySettings();
        return;
    }
    std::wstring base = CfgWindhawkValue(it);
    if (value == base) g_cfgOverrides.erase(it.key);
    else g_cfgOverrides[it.key] = {value, base};
    if (save) SaveCfgOverrides(); else g_cfgOverridesDirty = true;
    g_cfgValues[it.key] = value;
    CfgApplySettings();
    if (!wcscmp(it.key, L"theme")) g_cfgValues[L"appearance.transparency"] = std::to_wstring(g_settings.glassTransparency);
}

// One setting back to its default (as an override when Windhawk has another value)
void CfgSetDefault(const CfgDefault& d) {
    const CfgItem* it = CfgFind(d.key);
    if (!it) return;
    std::wstring base = CfgWindhawkValue(*it);
    if (ToLower(base) == ToLower(d.value)) g_cfgOverrides.erase(it->key);
    else g_cfgOverrides[it->key] = {d.value, base};
}

// The look of the active theme differs from its defaults (Appearance; for Custom also its own section)
bool CfgThemeChanged() {
    if (!g_cfgOverridesLoaded) LoadCfgOverrides();
    if (!g_themeTransparencyLoaded) LoadThemeTransparency();
    if (g_themeTransparency.count(ThemeName(g_settings.theme))) return true;
    for (const CfgDefault& d : kCfgDefaults) {
        const CfgItem* it = CfgFind(d.key);
        if (!it || (it->section != 1 && !(it->section == 2 && g_settings.theme == Theme::Custom))) continue;
        if (!wcscmp(d.key, L"appearance.transparency")) continue;
        std::wstring v = CfgIsInt(*it) ? std::to_wstring(CfgInt(it->key)) : CfgStr(it->key);
        if (ToLower(v) != ToLower(d.value)) return true;
    }
    return false;
}

void CfgResetTheme() {
    if (!g_cfgOverridesLoaded) LoadCfgOverrides();
    if (!g_themeTransparencyLoaded) LoadThemeTransparency();
    g_themeTransparency.erase(ThemeName(g_settings.theme));
    for (const CfgDefault& d : kCfgDefaults) {
        const CfgItem* it = CfgFind(d.key);
        if (!it || (it->section != 1 && !(it->section == 2 && g_settings.theme == Theme::Custom))) continue;
        if (!wcscmp(d.key, L"appearance.transparency")) continue;   // per theme, just cleared above
        CfgSetDefault(d);
    }
    SaveCfgOverrides();
    CfgApplySettings();
    CfgRefreshValues();
}

int BackupMessage(PCWSTR text, UINT flags);
// Everything back to how the mod comes (folders, tabs, icons and history stay)
void CfgResetAll() {
    if (BackupMessage(CfgT(L"Restore all the defaults?\n\nEvery setting goes back to how the mod comes, and the bar to the center. "
                           L"Folders, tabs and custom icons stay.",
                           L"Restaurar todos os padrões?\n\nTodos os ajustes voltam a como o mod vem, e a barra volta ao centro. "
                           L"Pastas, abas e ícones personalizados ficam."), MB_ICONQUESTION | MB_YESNO) != IDYES) return;
    if (!g_cfgOverridesLoaded) LoadCfgOverrides();
    g_cfgOverrides.clear();
    for (const CfgDefault& d : kCfgDefaults) CfgSetDefault(d);
    g_themeTransparency.clear();
    g_themeTransparencyLoaded = true;
    g_barPosX = g_barPosY = -1;
    Wh_SetIntValue(L"barX", -1);
    Wh_SetIntValue(L"barY", -1);
    Wh_SetStringValue(L"barMonitor", L"");
    SaveCfgOverrides();
    CfgApplySettings();
    CfgRefreshValues();
}

// ---------- Backup ----------
// One file with everything the user chose: the settings changed here (the GitHub repository too), folders, tabs,
// history, the bar's place and the custom icons. The icon pictures go inside the file, so they come back even after
// the mod is uninstalled (which erases its storage) or on another PC.
constexpr char kBackupHeader[] = "SiriPlusLaunchpadBackup 1";
constexpr PCWSTR kBackupStrings[] = {kFoldersKey, kCustomTabsKey, kBuiltinTabsKey, kHistoryKey, kLaunchKey, kKnownAppsKey, L"barMonitor",
                                     kThemeTransparencyKey};
constexpr PCWSTR kBackupInts[] = {L"barX", L"barY"};

std::string B64Encode(const std::string& in) {
    static const char* t = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve((in.size() + 2) / 3 * 4);
    size_t i = 0;
    for (; i + 2 < in.size(); i += 3) {
        unsigned v = (unsigned char)in[i] << 16 | (unsigned char)in[i + 1] << 8 | (unsigned char)in[i + 2];
        out += t[v >> 18]; out += t[(v >> 12) & 63]; out += t[(v >> 6) & 63]; out += t[v & 63];
    }
    if (i < in.size()) {
        unsigned v = (unsigned char)in[i] << 16 | (i + 1 < in.size() ? (unsigned char)in[i + 1] << 8 : 0);
        out += t[v >> 18]; out += t[(v >> 12) & 63];
        out += i + 1 < in.size() ? t[(v >> 6) & 63] : '=';
        out += '=';
    }
    return out;
}

bool B64Decode(const std::string& in, std::string& out) {
    out.clear();
    unsigned v = 0; int bits = 0;
    for (char ch : in) {
        int d;
        if (ch >= 'A' && ch <= 'Z') d = ch - 'A';
        else if (ch >= 'a' && ch <= 'z') d = ch - 'a' + 26;
        else if (ch >= '0' && ch <= '9') d = ch - '0' + 52;
        else if (ch == '+') d = 62;
        else if (ch == '/') d = 63;
        else if (ch == '=') break;
        else return false;
        v = v << 6 | d; bits += 6;
        if (bits >= 8) { bits -= 8; out += (char)((v >> bits) & 0xFF); }
    }
    return true;
}

std::string ToUtf8(const std::wstring& w);     // defined with the GitHub icons
std::wstring FromUtf8(const std::string& s);
std::string B64W(const std::wstring& s) { return B64Encode(ToUtf8(s)); }
bool B64ToW(const std::string& s, std::wstring& out) { std::string raw; if (!B64Decode(s, raw)) return false; out = FromUtf8(raw); return true; }

bool ReadWholeFile(const std::wstring& path, std::string& out, DWORD maxBytes) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    bool ok = GetFileSizeEx(f, &size) && size.QuadPart <= maxBytes;
    if (ok) {
        out.resize((size_t)size.QuadPart);
        DWORD got = 0;
        ok = out.empty() || (ReadFile(f, out.data(), (DWORD)out.size(), &got, nullptr) && got == out.size());
    }
    CloseHandle(f);
    return ok;
}

bool WriteWholeFile(const std::wstring& path, const std::string& data) {
    HANDLE f = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    DWORD put = 0;
    bool ok = data.empty() || (WriteFile(f, data.data(), (DWORD)data.size(), &put, nullptr) && put == data.size());
    CloseHandle(f);
    if (!ok) DeleteFileW(path.c_str());
    return ok;
}

std::wstring StorageString(PCWSTR key) {
    std::vector<WCHAR> buf(1 << 20, 0);
    Wh_GetStringValue(key, buf.data(), buf.size());
    buf.back() = 0;
    return buf.data();
}

// A message over the launcher (it stays open while the box is up)
int BackupMessage(PCWSTR text, UINT flags) {
    g_dialogOpen = true;
    SetWindowPos(g_wnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    int r = MessageBoxW(g_wnd, text, L"Siri Plus Launchpad", flags);
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    g_dialogOpen = false;
    if (g_shown) ForceForeground(g_wnd);
    return r;
}

bool PickBackupFile(bool save, std::wstring& out) {
    ComPtr<IFileDialog> dlg;
    if (FAILED(CoCreateInstance(save ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg)))) return false;
    COMDLG_FILTERSPEC types[] = { {CfgT(L"Siri Plus Launchpad backup", L"Backup do Siri Plus Launchpad"), L"*.siribackup"},
                                  {CfgT(L"All files", L"Todos os arquivos"), L"*.*"} };
    dlg->SetFileTypes(ARRAYSIZE(types), types);
    dlg->SetDefaultExtension(L"siribackup");
    dlg->SetTitle(save ? CfgT(L"Save backup", L"Salvar backup") : CfgT(L"Restore backup", L"Restaurar backup"));
    if (save) {
        SYSTEMTIME st; GetLocalTime(&st);
        wchar_t name[64];
        swprintf_s(name, L"SiriPlusLaunchpad %04d-%02d-%02d.siribackup", st.wYear, st.wMonth, st.wDay);
        dlg->SetFileName(name);
    }
    g_dialogOpen = true;
    SetWindowPos(g_wnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    HRESULT hr = dlg->Show(g_wnd);
    SetWindowPos(g_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    g_dialogOpen = false;
    if (g_shown) ForceForeground(g_wnd);
    ComPtr<IShellItem> res; PWSTR path = nullptr;
    if (FAILED(hr) || FAILED(dlg->GetResult(&res)) || FAILED(res->GetDisplayName(SIGDN_FILESYSPATH, &path))) return false;
    out = path; CoTaskMemFree(path);
    return true;
}

void BackupExport() {
    std::wstring path;
    if (!PickBackupFile(true, path)) return;
    if (g_cfgOverridesDirty) SaveCfgOverrides();
    std::string text = std::string(kBackupHeader) + "\n";
    // Every setting as it is now (the GitHub repository is one of them)
    for (const CfgItem& it : kCfgItems) {
        if (it.type == kCfgButton) continue;
        text += "S\t" + B64W(it.key) + "\t" + B64W(CfgIsInt(it) ? std::to_wstring(CfgInt(it.key)) : CfgStr(it.key)) + "\n";
    }
    for (PCWSTR key : kBackupStrings) text += "V\t" + B64W(key) + "\t" + B64W(StorageString(key)) + "\n";
    for (PCWSTR key : kBackupInts) text += "N\t" + B64W(key) + "\t" + std::to_string(Wh_GetIntValue(key, -1)) + "\n";
    // The icons, with their pictures
    int icons = 0, missing = 0;
    for (auto& [target, file] : g_customIcons) {
        std::string bytes;
        if (!ReadWholeFile(file, bytes, 16 << 20)) { missing++; continue; }
        PCWSTR ext = PathFindExtensionW(file.c_str());
        text += "I\t" + B64W(target) + "\t" + B64W(ext && *ext ? ext : L".png") + "\t" + B64Encode(bytes) + "\n";
        icons++;
    }
    if (!WriteWholeFile(path, text)) {
        BackupMessage(CfgT(L"Could not save the backup in this place.", L"Não foi possível salvar o backup neste local."), MB_ICONWARNING);
        return;
    }
    std::wstring msg = CfgFormat(CfgT(L"Backup saved.\n\nSettings, GitHub repository, folders, tabs and %d custom icon(s) are in the file.",
                                      L"Backup salvo.\n\nConfigurações, repositório do GitHub, pastas, abas e %d ícone(s) personalizado(s) estão no arquivo."), icons);
    if (missing)
        msg += CfgFormat(CfgT(L"\n\n%d icon(s) were left out: the picture is no longer on disk.",
                              L"\n\n%d ícone(s) ficaram de fora: a imagem não existe mais no disco."), missing);
    BackupMessage(msg.c_str(), MB_ICONINFORMATION);
}

// Restored icon pictures live here (the original files may be gone)
std::wstring RestoredIconFile(const std::wstring& target, std::wstring ext) {
    wchar_t base[MAX_PATH] = {};
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", base, MAX_PATH)) return {};
    std::wstring dir = std::wstring(base) + L"\\SiriPlusLaunchpad";
    CreateDirectoryW(dir.c_str(), nullptr);
    dir += L"\\Icons";
    CreateDirectoryW(dir.c_str(), nullptr);
    for (auto& ch : ext) if (!iswalnum(ch) && ch != L'.') ch = L'_';
    if (ext.size() > 8 || ext.empty() || ext[0] != L'.') ext = L".png";
    unsigned long long h = 1469598103934665603ULL;   // FNV-1a of the app's target
    for (wchar_t ch : ToLower(target)) { h ^= ch; h *= 1099511628211ULL; }
    wchar_t name[32];
    swprintf_s(name, L"\\%016llx", h);
    return dir + name + ext;
}

void BackupImport() {
    std::wstring path;
    if (!PickBackupFile(false, path)) return;
    std::string text;
    if (!ReadWholeFile(path, text, 512u << 20) || text.compare(0, sizeof(kBackupHeader) - 1, kBackupHeader) != 0) {
        BackupMessage(CfgT(L"This file is not a Siri Plus Launchpad backup.", L"Este arquivo não é um backup do Siri Plus Launchpad."), MB_ICONWARNING);
        return;
    }
    if (BackupMessage(CfgT(L"Restore this backup?\n\nThe current settings, folders, tabs and custom icons are replaced by the ones in the file.",
                           L"Restaurar este backup?\n\nAs configurações, pastas, abas e ícones personalizados atuais serão trocados pelos do arquivo."),
                      MB_ICONQUESTION | MB_YESNO) != IDYES) return;

    if (!g_cfgOverridesLoaded) LoadCfgOverrides();
    g_cfgOverrides.clear();
    std::unordered_map<std::wstring, std::wstring> icons;
    int iconsBad = 0;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find('\n', pos);
        if (end == std::string::npos) end = text.size();
        std::string line = text.substr(pos, end - pos);
        pos = end + 1;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::vector<std::string> f;
        for (size_t a = 0;;) {
            size_t b = line.find('\t', a);
            f.push_back(line.substr(a, b == std::string::npos ? b : b - a));
            if (b == std::string::npos) break;
            a = b + 1;
        }
        std::wstring key, value;
        if (f.size() == 3 && f[0] == "S" && B64ToW(f[1], key) && B64ToW(f[2], value)) {
            const CfgItem* it = CfgFind(key.c_str());
            if (!it || it->type == kCfgButton) continue;
            std::wstring base = CfgWindhawkValue(*it);
            if (value != base) g_cfgOverrides[it->key] = {value, base};
        } else if (f.size() == 3 && f[0] == "V" && B64ToW(f[1], key) && B64ToW(f[2], value)) {
            for (PCWSTR k : kBackupStrings) if (key == k) Wh_SetStringValue(k, value.c_str());
        } else if (f.size() == 3 && f[0] == "N" && B64ToW(f[1], key)) {
            for (PCWSTR k : kBackupInts) if (key == k) Wh_SetIntValue(k, atoi(f[2].c_str()));
        } else if (f.size() == 4 && f[0] == "I") {
            std::wstring target, ext; std::string bytes;
            std::wstring file;
            if (!B64ToW(f[1], target) || target.empty() || !B64ToW(f[2], ext) || !B64Decode(f[3], bytes) ||
                (file = RestoredIconFile(target, ext)).empty() || !WriteWholeFile(file, bytes)) { iconsBad++; continue; }
            icons[target] = file;
        }
    }
    LoadThemeTransparency();   // the per-theme transparency just written from the backup (saved again as is below)
    SaveCfgOverrides();

    // Load everything again, as when the mod starts
    LoadFolders(); LoadCustomTabs(); LoadBuiltinTabs(); LoadSearchHistory(); LoadLaunchCounts();
    for (auto& [target, file] : g_customIcons) {   // icons the backup does not have go back to the app's own
        if (icons.count(target)) continue;
        for (auto& a : g_apps) if (a->target == target) ResetAppIcon(*a);
    }
    g_customIcons = std::move(icons);
    SaveCustomIcons();
    CfgRefreshValues();
    CfgApplySettings();
    g_appsTime = 0;
    StartLoadingApps();   // the apps come back with the restored icons
    Render();

    std::wstring msg = CfgFormat(CfgT(L"Backup restored.\n\n%d custom icon(s) back.", L"Backup restaurado.\n\n%d ícone(s) personalizado(s) de volta."),
                                 (int)g_customIcons.size());
    if (iconsBad)
        msg += CfgFormat(CfgT(L"\n%d icon(s) could not be written.", L"\n%d ícone(s) não puderam ser gravados."), iconsBad);
    if (g_cfg.pendingReload)
        msg += CfgT(L"\n\nThe restored mode is used the next time the launcher opens.", L"\n\nO modo restaurado vale na próxima vez que o menu abrir.");
    BackupMessage(msg.c_str(), MB_ICONINFORMATION);
}

void CfgChoose(int code) {
    int item = code / 32, opt = code % 32;
    if (item < 0 || item >= kCfgItemCount) return;
    const CfgItem& it = kCfgItems[item];
    if (it.type != kCfgChoice || opt >= it.optCount) return;
    CfgSet(it, it.opts[opt].value);
}

void CfgSliderAt(int item, float x, bool save) {
    const CfgItem& it = kCfgItems[item];
    for (const CfgRow& row : g_cfgRows) {
        if (row.item != item) continue;
        D2D1_RECT_F tr = CfgSliderTrack(row.ctl);
        float k = std::clamp((x - tr.left) / std::max(1.0f, tr.right - tr.left), 0.0f, 1.0f);
        int step = std::max(1, it.step), v = it.min + (int)std::lround(k * (it.max - it.min) / step) * step;
        v = std::clamp(v, it.min, it.max);
        std::wstring text = std::to_wstring(v);
        if (text == CfgValue(it) && !save) return;
        // While dragging, the value shows at once; the launcher follows a few times per second
        g_cfgValues[it.key] = text;
        if (save || Now() - g_cfg.lastApply > 0.045) { g_cfg.lastApply = Now(); CfgSet(it, text, save); }
        else Render();
        return;
    }
}

void CfgCommitEdit() {
    if (g_cfg.edit < 0) return;
    int item = g_cfg.edit;
    g_cfg.edit = -1;
    std::wstring t = g_cfg.editText;
    if (kCfgItems[item].type == kCfgText) {
        t.erase(0, t.find_first_not_of(L" \t"));
        t.erase(t.find_last_not_of(L" \t") + 1);
        CfgSet(kCfgItems[item], t);
        return;
    }
    if (!t.empty() && t[0] != L'#') t = L"#" + t;
    D2D1_COLOR_F a = ParseColor(t, D2D1::ColorF(0, 0, 0, 0)), b = ParseColor(t, D2D1::ColorF(1, 1, 1, 1));
    if (a.a == b.a && t.size() == 7) CfgSet(kCfgItems[item], ToLower(t));   // valid #rrggbb
    else Render();
}

bool CfgPickColor(int item) {
    const CfgItem& it = kCfgItems[item];
    static COLORREF custom[16];
    D2D1_COLOR_F c = ParseColor(CfgValue(it), D2D1::ColorF(1, 1, 1));
    CHOOSECOLORW cc{};
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = g_wnd;
    cc.rgbResult = RGB((BYTE)std::lround(c.r * 255), (BYTE)std::lround(c.g * 255), (BYTE)std::lround(c.b * 255));
    cc.lpCustColors = custom;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;
    g_dialogOpen = true;
    BOOL ok = ChooseColorW(&cc);
    g_dialogOpen = false;
    ForceForeground(g_wnd);
    if (!ok) return false;
    wchar_t text[8];
    swprintf(text, 8, L"#%02x%02x%02x", GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));
    CfgSet(it, text);
    return true;
}

// ---------- Opening and closing ----------
// Live liquid glass: one picture covering everything the settings animation passes over (bar home, sections, panel)
void CfgCaptureLiquid() {
    if (!LiquidWanted() || !g_settings.liveGlass) return;
    D2D1_RECT_F u = g_cfg.home;
    for (const D2D1_RECT_F& r : {g_cfg.bar, g_cfg.side, g_cfg.panel}) {
        u.left = std::min(u.left, r.left); u.top = std::min(u.top, r.top);
        u.right = std::max(u.right, r.right); u.bottom = std::max(u.bottom, r.bottom);
    }
    const Layout& L = g_layout;
    RECT cap{L.area.left + (LONG)std::floor(u.left), L.area.top + (LONG)std::floor(u.top),
             L.area.left + (LONG)std::ceil(u.right), L.area.top + (LONG)std::ceil(u.bottom)};
    InflateRect(&cap, kLiquidMargin, kLiquidMargin);
    CaptureLiquid(cap);
}

void CfgEnter() {
    if (g_cfg.active || g_closing || !g_shown || g_openFolderIndex >= 0) return;
    g_menu.open = false; g_menu.closing = false;
    g_siriExpanded = false; g_siriAnim = 0; ResetPowerPill();
    g_powerHover = g_clearHover = g_backHover = false; g_hover = -1;
    g_gear.shown = true; g_gear.anim = 1; g_gear.hover = false;
    HMONITOR monitor = MonitorFromWindow(g_wnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    Layout& L = g_layout;
    g_cfg = CfgState{};
    g_cfg.active = true;
    g_cfg.section = g_cfgLastSection;
    g_cfg.siriMode = g_settings.siri; g_cfg.siriOriginal = g_settings.siriOriginal; g_cfg.fullscreen = g_settings.fullscreen;
    g_cfg.powerButton = g_settings.siriPowerButton;
    // The window grows to the whole work area; everything keeps its place on the screen
    float dx = (float)(L.area.left - mi.rcWork.left), dy = (float)(L.area.top - mi.rcWork.top);
    g_cfg.home = D2D1::RectF(L.search.left + dx, L.search.top + dy, L.search.right + dx, L.search.bottom + dy);
    float inset = WindowInset(L.scale);
    g_cfg.window = D2D1::RectF(dx + inset, dy + inset, dx + L.width - inset, dy + L.height - inset);
    g_cfg.homeArea = L.area;
    g_cfg.homeExtra = L.extraBottom;
    SetArea(L, mi.rcWork);
    L.extraBottom = 0;
    g_backdropImage = {};   // a still blur would no longer line up (the live blur is not affected)
    CfgRefreshValues();
    ComputeLayout();
    CfgCaptureLiquid();
    Render();
    StartFrames();
}

void CfgExit() {
    if (!g_cfg.active || g_cfg.closing) return;
    CfgCommitEdit();
    g_cfg.capture = g_cfg.drag = -1;
    if (GetCapture() == g_wnd) ReleaseCapture();
    g_menu.open = false;
    if (g_cfgOverridesDirty) SaveCfgOverrides();
    g_cfgLastSection = g_cfg.section;
    // Where the bar goes back to (its size may have changed)
    HMONITOR monitor = MonitorFromWindow(g_wnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(monitor, &mi);
    if (g_cfg.siriMode) {
        g_cfg.homeArea = SiriAreaFor(monitor, g_layout.scale);
        g_cfg.home = CfgHomeBar(g_cfg.homeArea, mi.rcWork, g_layout.scale);
    } else {
        g_cfg.closeHome = g_cfg.bar;   // Window / Fullscreen: the bar stays where it is and fades away as the settings fold into it
    }
    g_cfg.closing = true;
    CfgCaptureLiquid();
    g_cfg.search.clear();
    g_cfg.hover = g_cfg.sideHover = -1; g_cfg.doneHover = g_cfg.gearHover = false;
    StartFrames();
}

// Leaves at once (the launcher is closing for another reason)
void CfgAbort() {
    if (!g_cfg.active) return;
    if (g_cfgOverridesDirty) SaveCfgOverrides();
    g_cfgLastSection = g_cfg.section;
    if (GetCapture() == g_wnd) ReleaseCapture();
    Layout& L = g_layout;
    if (g_cfg.siriMode) SetArea(L, SiriAreaFor(MonitorFromWindow(g_wnd, MONITOR_DEFAULTTOPRIMARY), L.scale));
    else { SetArea(L, g_cfg.homeArea); L.extraBottom = g_cfg.homeExtra; }
    g_cfg.active = false; g_cfg.closing = false;
    g_menu.open = false;
    g_gear.shown = false;
    ComputeLayout();
}

bool CfgStep(float dt) {
    if (!g_cfg.active) return false;
    float target = g_cfg.closing ? 0.0f : 1.0f;
    if (g_cfg.anim != target) {
        float step = dt * g_settings.animSpeed / (g_cfg.closing ? 0.90f : 1.00f);
        if (g_cfg.closing && !g_cfg.siriMode && g_cfg.anim <= 0.30f) step *= 0.30f / 0.55f;   // the shrink and suck take about 0.55 s
        g_cfg.anim = g_cfg.closing ? std::max(0.0f, g_cfg.anim - step) : std::min(1.0f, g_cfg.anim + step);
        return true;
    }
    if (g_cfg.closing && !g_cfg.siriMode) {
        // Window / Fullscreen: everything has folded into the bar and the bar has faded: the launcher just hides
        // (no search field, no window, no close animation; what changed shows the next time it opens)
        if (g_cfgOverridesDirty) SaveCfgOverrides();
        g_lastClosed = Now();
        HideNow();
        return true;
    }
    if (g_cfg.closing) {
        // Siri modes: the bar is home; it shrinks into its gear (turning) and the gear is sucked in, then the launcher hides
        g_cfg.endT = std::min(1.0f, g_cfg.endT + dt * g_settings.animSpeed / 0.55f);
        if (g_cfg.endT >= 1.0f) {
            if (g_cfgOverridesDirty) SaveCfgOverrides();
            g_lastClosed = Now();
            HideNow();   // what changed shows the next time it opens
        }
        return true;
    }
    return false;
}

// ---------- Input ----------
int CfgSideAt(float x, float y) {
    if (CfgSideT() < 1.0f) return -1;
    for (const CfgSideRow& r : g_cfgSideRows) if (InRect(r.r, x, y)) return r.section;
    return -1;
}
int CfgRowAt(float x, float y) {
    if (!InRect(g_cfgView, x, y)) return -1;
    for (int i = 0; i < (int)g_cfgRows.size(); i++) if (InRect(g_cfgRows[i].r, x, y)) return i;
    return -1;
}
bool CfgOnControl(int row, float x, float y) {
    if (row < 0) return false;
    const CfgRow& r = g_cfgRows[row];
    if (kCfgItems[r.item].type == kCfgToggle) return true;   // the whole row switches it
    float m = 4 * g_layout.scale;
    return InRect(D2D1::RectF(r.ctl.left - m, r.ctl.top - m, r.ctl.right + m, r.ctl.bottom + m), x, y);
}

void CfgSelectSection(int section) {
    if (section < 0 || section >= kCfgSectionCount) return;
    CfgCommitEdit();
    g_cfg.capture = -1;
    g_cfg.section = section; g_cfg.search.clear(); g_cfg.scroll = 0;
    ComputeLayout(); Render();
}

void CfgClickRow(int row, float x, float y) {
    const CfgRow& r = g_cfgRows[row];
    int item = r.item;
    const CfgItem& it = kCfgItems[item];
    float s = g_layout.scale;
    switch (it.type) {
        case kCfgToggle: CfgSet(it, _wtoi(CfgValue(it).c_str()) ? L"0" : L"1"); break;
        case kCfgSlider:
            g_cfg.drag = item;
            SetCapture(g_wnd);
            CfgSliderAt(item, x, false);
            break;
        case kCfgChoice: {
            std::vector<MenuItem> items;
            const std::wstring& v = CfgValue(it);
            for (int k = 0; k < it.optCount; k++)
                items.push_back({CfgT(it.opts[k].en, it.opts[k].pt), kMenuCfgBase + item * 32 + k, false, false, v == it.opts[k].value ? L"" : nullptr});
            g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
            OpenMenu(r.ctl.left, r.ctl.bottom + 4 * s, std::move(items), false, r.ctl.right - r.ctl.left);
            break;
        }
        case kCfgColor: {
            float d = r.ctl.bottom - r.ctl.top;
            if (x < r.ctl.left + d + 4 * s) { CfgPickColor(item); break; }
            g_cfg.edit = item; g_cfg.editText = CfgValue(it);
            Render();
            break;
        }
        case kCfgShortcut: g_cfg.capture = item; Render(); break;
        case kCfgText: g_cfg.edit = item; g_cfg.editText = CfgValue(it); Render(); break;
        case kCfgButton:
            if (!wcscmp(it.key, L"#export")) BackupExport();
            else if (!wcscmp(it.key, L"#import")) BackupImport();
            else if (!wcscmp(it.key, L"#themeReset")) { if (CfgThemeChanged()) CfgResetTheme(); }
            else CfgResetAll();
            break;
    }
    (void)y;
}

void CfgShortcutKey(WPARAM key) {
    const CfgItem& it = kCfgItems[g_cfg.capture];
    if (key == VK_ESCAPE) { g_cfg.capture = -1; Render(); return; }
    if (key == VK_BACK || key == VK_DELETE) { g_cfg.capture = -1; CfgSet(it, L""); return; }
    if (key == VK_CONTROL || key == VK_SHIFT || key == VK_MENU || key == VK_LWIN || key == VK_RWIN) return;
    if (!(GetKeyState(VK_CONTROL) & 0x8000)) return;   // must start with Ctrl
    std::wstring name;
    if ((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9')) name = std::wstring(1, (wchar_t)key);
    else if (key >= VK_F1 && key <= VK_F24) name = L"F" + std::to_wstring(key - VK_F1 + 1);
    else return;
    std::wstring text = std::wstring(L"Ctrl+") + ((GetKeyState(VK_SHIFT) & 0x8000) ? L"Shift+" : L"") + name;
    std::wstring label;
    if (!ParseShortcut(text, label).valid) return;   // Ctrl+A/C/V/X stay for the text
    g_cfg.capture = -1;
    CfgSet(it, text);
}

void CfgHover(float x, float y) {
    int side = CfgSideAt(x, y);
    int row = CfgSettled() ? CfgRowAt(x, y) : -1;
    if (row >= 0 && !CfgOnControl(row, x, y)) row = -1;
    bool done = CfgSettled() && InRect(g_cfg.done, x, y);
    bool gear = InRect(GearRect(), x, y);
    if (side != g_cfg.sideHover || row != g_cfg.hover || done != g_cfg.doneHover || gear != g_cfg.gearHover) {
        g_cfg.sideHover = side; g_cfg.hover = row; g_cfg.doneHover = done; g_cfg.gearHover = gear;
        Render();
    }
}

bool CfgMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT& result) {
    result = 0;
    if (g_menu.open && !g_menu.closing && msg != WM_RBUTTONUP && msg != WM_RBUTTONDOWN) return false;   // the options menu works as usual
    float x = (float)GET_X_LPARAM(lParam), y = (float)GET_Y_LPARAM(lParam);
    switch (msg) {
        case WM_SETCURSOR: {
            if (LOWORD(lParam) != HTCLIENT) return false;
            POINT p; GetCursorPos(&p); ScreenToClient(hwnd, &p);
            float px = (float)p.x, py = (float)p.y;
            int row = CfgSettled() ? CfgRowAt(px, py) : -1;
            bool hand = CfgSideAt(px, py) >= 0 || (row >= 0 && CfgOnControl(row, px, py)) || (CfgSettled() && InRect(g_cfg.done, px, py)) ||
                        InRect(GearRect(), px, py);
            SetCursor(LoadCursorW(nullptr, hand ? IDC_HAND : IDC_ARROW));
            result = TRUE;
            return true;
        }
        case WM_MOUSEMOVE:
            TrackLeave(hwnd);
            if (g_cfg.drag >= 0) { CfgSliderAt(g_cfg.drag, x, false); return true; }
            CfgHover(x, y);
            return true;
        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            if (g_cfg.drag < 0) { g_cfg.hover = g_cfg.sideHover = -1; g_cfg.doneHover = g_cfg.gearHover = false; Render(); }
            return true;
        case WM_LBUTTONDOWN: {
            if (g_cfg.closing) return true;
            g_ignoreUp = true;
            if (InRect(GearRect(), x, y) || (CfgSettled() && InRect(g_cfg.done, x, y))) { CfgExit(); return true; }
            int side = CfgSideAt(x, y);
            int row = CfgSettled() ? CfgRowAt(x, y) : -1;
            bool onControl = row >= 0 && CfgOnControl(row, x, y);
            // Clicking anywhere else finishes the color being typed / the shortcut being waited for
            if (g_cfg.edit >= 0 && !(onControl && g_cfgRows[row].item == g_cfg.edit)) CfgCommitEdit();
            if (g_cfg.capture >= 0 && !(onControl && g_cfgRows[row].item == g_cfg.capture)) { g_cfg.capture = -1; Render(); }
            if (side >= 0) { CfgSelectSection(side); return true; }
            if (onControl) CfgClickRow(row, x, y);
            return true;
        }
        case WM_LBUTTONUP:
            g_ignoreUp = false;
            if (g_cfg.drag >= 0) {
                int item = g_cfg.drag;
                g_cfg.drag = -1;
                if (GetCapture() == hwnd) ReleaseCapture();
                CfgSliderAt(item, x, true);
            }
            return true;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return true;
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL: {
            POINT p{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &p);
            if (msg == WM_MOUSEWHEEL && CfgSettled() && InRect(g_cfg.panel, (float)p.x, (float)p.y)) {
                g_cfg.scroll = std::clamp(g_cfg.scroll - GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA * 56 * g_layout.scale, 0.0f, CfgMaxScroll());
                CfgBuild();
                CfgHover((float)p.x, (float)p.y);
                Render();
            }
            return true;
        }
        case WM_KEYDOWN: {
            if (g_cfg.closing) return true;
            if (g_cfg.capture >= 0) { CfgShortcutKey(wParam); return true; }
            if (g_cfg.edit >= 0) {
                if (wParam == VK_RETURN) CfgCommitEdit();
                else if (wParam == VK_ESCAPE) { g_cfg.edit = -1; Render(); }
                return true;
            }
            if (wParam == VK_ESCAPE) {
                if (!g_cfg.search.empty()) { g_cfg.search.clear(); g_cfg.scroll = 0; ComputeLayout(); Render(); }
                else CfgExit();
            } else if ((wParam == VK_DOWN || wParam == VK_UP) && g_cfg.search.empty()) {
                CfgSelectSection(std::clamp(g_cfg.section + (wParam == VK_DOWN ? 1 : -1), 0, kCfgSectionCount - 1));
            }
            return true;
        }
        case WM_CHAR: {
            if (g_cfg.closing || g_cfg.capture >= 0) return true;
            WCHAR ch = (WCHAR)wParam;
            if (g_cfg.edit >= 0) {
                bool freeText = kCfgItems[g_cfg.edit].type == kCfgText;
                if (ch == 8) { if (!g_cfg.editText.empty()) g_cfg.editText.pop_back(); }
                else if (ch == 0x16 && freeText) {   // Ctrl+V
                    std::wstring paste = ReadClipboardLine();
                    g_cfg.editText += paste.substr(0, 300 - std::min<size_t>(300, g_cfg.editText.size()));
                }
                else if (freeText) { if (ch >= 32 && g_cfg.editText.size() < 300) g_cfg.editText += ch; }
                else if ((iswxdigit(ch) || (ch == L'#' && g_cfg.editText.empty())) && g_cfg.editText.size() < 7) g_cfg.editText += ch;
                Render();
                return true;
            }
            if (ch == 8) { if (g_cfg.search.empty()) return true; g_cfg.search.pop_back(); }
            else if (ch >= 32 && ch != 0x7F) { if (g_cfg.search.size() < 40) g_cfg.search += ch; }
            else return true;
            g_cfg.scroll = 0; g_cfg.hover = -1;
            ComputeLayout(); Render();
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Icons from a GitHub repository: right-click an app > "Icon from GitHub..." shows the repository's images in the
// grid, with the bar searching them. Images are downloaded once into %LOCALAPPDATA%\SiriPlusLaunchpad\GitHubIcons
// and the chosen one becomes the app's custom icon (like "Change icon...").

struct GhRepo { std::wstring owner, repo, branch, folder, commit; };   // commit: the branch's latest, for fresh downloads

// "owner/repo", "https://github.com/owner/repo", ".../tree/branch/some/folder"
bool ParseGhRepo(std::wstring text, GhRepo& out) {
    out = GhRepo{};
    text.erase(0, text.find_first_not_of(L" \t"));
    text.erase(text.find_last_not_of(L" \t/") + 1);
    for (PCWSTR prefix : {L"https://", L"http://"})
        if (ToLower(text).compare(0, wcslen(prefix), prefix) == 0) text.erase(0, wcslen(prefix));
    if (ToLower(text).compare(0, 11, L"github.com/") == 0) text.erase(0, 11);
    if (text.size() > 4 && ToLower(text.substr(text.size() - 4)) == L".git") text.resize(text.size() - 4);
    std::vector<std::wstring> parts;
    for (size_t pos = 0; pos <= text.size();) {
        size_t end = text.find(L'/', pos);
        if (end == std::wstring::npos) end = text.size();
        if (end > pos) parts.push_back(text.substr(pos, end - pos));
        pos = end + 1;
    }
    if (parts.size() < 2) return false;
    out.owner = parts[0]; out.repo = parts[1];
    if (parts.size() >= 4 && (parts[2] == L"tree" || parts[2] == L"blob")) {
        out.branch = parts[3];
        for (size_t i = 4; i < parts.size(); i++) out.folder += (out.folder.empty() ? L"" : L"/") + parts[i];
    }
    return true;
}

std::string ToUtf8(const std::wstring& w) {
    if (w.empty()) return {};
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), s.data(), n, nullptr, nullptr);
    return s;
}
std::wstring FromUtf8(const std::string& s) {
    if (s.empty()) return {};
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), w.data(), n);
    return w;
}
// Path for a URL: every byte that is not a plain letter, digit or -._~/ becomes %XX
std::wstring UrlPath(const std::wstring& path) {
    std::wstring out;
    for (unsigned char b : ToUtf8(path)) {
        if (isalnum(b) || b == '-' || b == '.' || b == '_' || b == '~' || b == '/') out += (wchar_t)b;
        else { wchar_t hex[4]; swprintf(hex, 4, L"%%%02X", b); out += hex; }
    }
    return out;
}

// HTTPS GET with WinHTTP; body into memory. Returns the HTTP status (0 = no connection)
DWORD HttpGet(const std::wstring& url, std::string& body) {
    body.clear();
    URL_COMPONENTS parts{};
    parts.dwStructSize = sizeof(parts);
    wchar_t host[256] = {}, path[2048] = {};
    parts.lpszHostName = host; parts.dwHostNameLength = ARRAYSIZE(host);
    parts.lpszUrlPath = path; parts.dwUrlPathLength = ARRAYSIZE(path);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &parts)) return 0;
    DWORD status = 0;
    HINTERNET session = WinHttpOpen(L"SiriPlusLaunchpad", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!session) session = WinHttpOpen(L"SiriPlusLaunchpad", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET connect = session ? WinHttpConnect(session, host, parts.nPort, 0) : nullptr;
    HINTERNET request = connect ? WinHttpOpenRequest(connect, L"GET", path, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                                     parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0) : nullptr;
    if (request) {
        WinHttpSetTimeouts(request, 8000, 8000, 10000, 20000);
        PCWSTR headers = L"Accept: application/vnd.github+json\r\n";
        if (WinHttpSendRequest(request, headers, (DWORD)-1L, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(request, nullptr)) {
            DWORD size = sizeof(status);
            WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX);
            char buffer[16384];
            DWORD read = 0;
            while (WinHttpReadData(request, buffer, sizeof(buffer), &read) && read > 0 && body.size() < 64u * 1024 * 1024) body.append(buffer, read);
        }
        WinHttpCloseHandle(request);
    }
    if (connect) WinHttpCloseHandle(connect);
    if (session) WinHttpCloseHandle(session);
    return status;
}

// The JSON string that starts at `pos` (just after its opening quote); `pos` ends after the closing quote
std::string JsonString(const std::string& json, size_t& pos) {
    std::string out;
    auto utf8 = [&](unsigned cp) {
        // UTF-8: 1 to 4 bytes; the first says how many follow, the others carry 6 bits each
        int extra = cp < 0x80 ? 0 : cp < 0x800 ? 1 : cp < 0x10000 ? 2 : 3;
        static const unsigned char lead[4] = {0x00, 0xC0, 0xE0, 0xF0};
        out += (char)(lead[extra] | (cp >> (6 * extra)));
        for (int i = extra - 1; i >= 0; i--) out += (char)(0x80 | ((cp >> (6 * i)) & 0x3F));
    };
    while (pos < json.size() && json[pos] != '"') {
        char ch = json[pos++];
        if (ch != '\\' || pos >= json.size()) { out += ch; continue; }
        char e = json[pos++];
        if (e == 'n') out += '\n'; else if (e == 't') out += '\t'; else if (e == 'r') out += '\r';
        else if (e == 'b') out += '\b'; else if (e == 'f') out += '\f';
        else if (e == 'u' && pos + 4 <= json.size()) {
            unsigned cp = (unsigned)strtoul(json.substr(pos, 4).c_str(), nullptr, 16);
            pos += 4;
            if (cp >= 0xD800 && cp < 0xDC00 && pos + 6 <= json.size() && json[pos] == '\\' && json[pos + 1] == 'u') {
                unsigned low = (unsigned)strtoul(json.substr(pos + 2, 4).c_str(), nullptr, 16);
                if (low >= 0xDC00 && low < 0xE000) { cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00); pos += 6; }
            }
            utf8(cp);
        } else out += e;   // \" \\ \/
    }
    pos++;
    return out;
}

bool GhImagePath(const std::wstring& path) {
    std::wstring lower = ToLower(path);
    for (PCWSTR ext : {L".png", L".ico", L".jpg", L".jpeg", L".bmp", L".gif", L".webp"}) {
        size_t n = wcslen(ext);
        if (lower.size() > n && lower.compare(lower.size() - n, n, ext) == 0) return true;
    }
    return false;
}

constexpr UINT WM_APP_GHLIST = WM_APP + 7;
constexpr UINT WM_APP_GHICON = WM_APP + 8;
struct GhListResult { std::wstring key, branch, commit, error; std::vector<std::wstring> paths, shas; };
// The main color of an icon: a class for the filter (0 red ... 6 pink, 7 white, 8 gray, 9 black, -1 unreadable)
// and a key that puts the icons in rainbow order
struct GhColor { int cls = -1; float key = 1000; };
struct GhIconResult { std::wstring path; std::vector<UINT32> pixels; int size = 0; GhColor color; bool replaced = false; };

struct GhState {
    bool active = false;                  // the gallery is showing
    std::wstring key;                     // the repository text the list belongs to
    GhRepo repo;
    std::vector<std::wstring> paths;      // images in the repository (inside the folder)
    bool loading = false;
    std::wstring error;
    std::wstring target;                  // the app getting the icon
    int colorFilter = -1;                 // only icons of this color (-1 = all)
    bool byColor = false;                 // the icons in rainbow order
    bool refreshing = false;              // the list is being read again (new pictures in the repository)
    double listTime = -1000;              // when the list was last read
    // Where the launcher was when the gallery opened, to go back there
    struct Return { int tab = 0, folder = -1; std::wstring query; float scroll = 0; bool searchActive = false; } back;
};
GhState g_gh;
[[clang::no_destroy]] std::unordered_map<std::wstring, GhColor> g_ghColors;   // path -> its color (known once the picture was read)
int g_ghColorsNew;                        // found since the color list was last saved
[[clang::no_destroy]] std::unordered_set<std::wstring> g_ghOnScreen;   // pictures the grid shows now (only these keep their pixels)
constexpr int kGhColorCount = 10;
constexpr UINT32 kGhColorRgb[kGhColorCount] = {0xFF3B30, 0xFF9500, 0xFFCC00, 0x34C759, 0x0A84FF, 0xAF52DE, 0xFF2D92, 0xFFFFFF, 0x8E8E93, 0x000000};
constexpr UINT_PTR kTimerGhColor = 12;
bool GhColorMode() { return g_gh.colorFilter >= 0 || g_gh.byColor; }
[[clang::no_destroy]] std::unordered_map<std::wstring, std::shared_ptr<App>> g_ghItems;   // path -> grid item (keeps its picture)
HANDLE g_ghListThread, g_ghWake;
constexpr int kGhIconThreads = 4;
HANDLE g_ghIconThreads[kGhIconThreads];
std::atomic<bool> g_ghStop;
SRWLOCK g_ghLock = SRWLOCK_INIT;
[[clang::no_destroy]] std::deque<std::wstring> g_ghQueue;   // pictures to fetch, most wanted first
[[clang::no_destroy]] GhRepo g_ghQueueRepo;
// Each picture's version (git blob sha): what the repository has now, and what the copy on disk is. When they differ the
// picture was replaced in the repository and is downloaded again. Both under g_ghLock (the download threads use them)
[[clang::no_destroy]] std::unordered_map<std::wstring, std::wstring> g_ghWantSha, g_ghFileSha;
bool g_ghFileShaLoaded, g_ghFileShaDirty;
constexpr double kGhListRefresh = 15;     // seconds: opening the gallery again after this reads the list again

std::wstring GhCacheFile(const GhRepo& r, const std::wstring& path) {
    wchar_t base[MAX_PATH] = {};
    if (!GetEnvironmentVariableW(L"LOCALAPPDATA", base, MAX_PATH)) return {};
    auto safe = [](std::wstring s) { for (auto& ch : s) if (wcschr(L"<>:\"|?*/\\", ch)) ch = L'_'; return s; };
    std::wstring file = std::wstring(base) + L"\\SiriPlusLaunchpad\\GitHubIcons\\" + safe(r.owner) + L"\\" + safe(r.repo) + L"\\" + safe(r.branch) + L"\\";
    for (wchar_t ch : path) file += ch == L'/' ? L'\\' : (wcschr(L"<>:\"|?*", ch) ? L'_' : ch);
    return file;
}

// The picture on disk (downloaded the first time)
bool GhEnsureFile(const GhRepo& r, const std::wstring& path, std::wstring& file, bool* replaced = nullptr) {
    file = GhCacheFile(r, path);
    if (file.empty()) return false;
    std::wstring want, have;
    AcquireSRWLockShared(&g_ghLock);
    if (auto w = g_ghWantSha.find(path); w != g_ghWantSha.end()) want = w->second;
    if (auto h = g_ghFileSha.find(path); h != g_ghFileSha.end()) have = h->second;
    ReleaseSRWLockShared(&g_ghLock);
    bool exists = GetFileAttributesW(file.c_str()) != INVALID_FILE_ATTRIBUTES;
    if (exists && (want.empty() || want == have)) return true;
    std::string body;
    // By the commit, not the branch: the branch's address can serve an old copy for a few minutes after an upload
    std::wstring ref = r.commit.empty() ? r.branch : r.commit;
    std::wstring url = L"https://raw.githubusercontent.com/" + r.owner + L"/" + r.repo + L"/" + UrlPath(ref) + L"/" + UrlPath(path);
    if (HttpGet(url, body) != 200 || body.empty()) return exists;   // the old copy still serves
    std::wstring dir = file.substr(0, file.find_last_of(L'\\'));
    SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    std::wstring part = file + L".part";
    HANDLE h = CreateFileW(part.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    bool ok = WriteFile(h, body.data(), (DWORD)body.size(), &written, nullptr) && written == body.size();
    CloseHandle(h);
    if (!ok || !MoveFileExW(part.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING)) { DeleteFileW(part.c_str()); return exists; }
    if (!want.empty()) {
        AcquireSRWLockExclusive(&g_ghLock);
        g_ghFileSha[path] = want;
        g_ghFileShaDirty = true;
        ReleaseSRWLockExclusive(&g_ghLock);
    }
    if (replaced) *replaced = exists;
    return true;
}

// The versions of the copies on disk, kept next to them
std::wstring GhShaFile(const GhRepo& r) { return r.branch.empty() ? std::wstring() : GhCacheFile(r, L"~versions.tsv"); }

void GhLoadFileShas() {
    g_ghFileShaLoaded = true;
    std::wstring file = GhShaFile(g_gh.repo);
    HANDLE h = file.empty() ? INVALID_HANDLE_VALUE : CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    std::string data;
    char buffer[65536];
    DWORD got = 0;
    while (ReadFile(h, buffer, sizeof(buffer), &got, nullptr) && got > 0 && data.size() < (32u << 20)) data.append(buffer, got);
    CloseHandle(h);
    AcquireSRWLockExclusive(&g_ghLock);
    for (const std::wstring& line : Split(FromUtf8(data), L'\n')) {
        auto f = Split(line, L'\t');
        if (f.size() == 2 && !f[0].empty() && !f[1].empty()) g_ghFileSha.emplace(f[0], f[1]);
    }
    ReleaseSRWLockExclusive(&g_ghLock);
}

void GhSaveFileShas() {
    std::wstring file = GhShaFile(g_gh.repo), text;
    AcquireSRWLockExclusive(&g_ghLock);
    if (!g_ghFileShaDirty || file.empty()) { ReleaseSRWLockExclusive(&g_ghLock); return; }
    g_ghFileShaDirty = false;
    for (auto& [path, sha] : g_ghFileSha) text += Clean(path) + L"\t" + sha + L"\n";
    ReleaseSRWLockExclusive(&g_ghLock);
    SHCreateDirectoryExW(nullptr, file.substr(0, file.find_last_of(L'\\')).c_str(), nullptr);
    std::string data = ToUtf8(text);
    HANDLE h = CreateFileW(file.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD put = 0;
    WriteFile(h, data.data(), (DWORD)data.size(), &put, nullptr);
    CloseHandle(h);
}

// The color most of the icon has. Colored pixels vote for their hue (stronger and brighter ones count more); an icon with
// almost no color is white, gray or black by its brightness
GhColor GhClassify(const std::vector<UINT32>& pixels) {
    GhColor out;
    float hist[36] = {};
    double opaque = 0, colored = 0, bright = 0;
    for (UINT32 p : pixels) {
        UINT32 a = p >> 24;
        if (a < 96) continue;
        float r = ((p >> 16) & 0xFF) * 255.0f / a, g = ((p >> 8) & 0xFF) * 255.0f / a, b = (p & 0xFF) * 255.0f / a;
        float mx = std::max({r, g, b}), mn = std::min({r, g, b});
        float v = std::min(mx / 255.0f, 1.0f), sat = mx > 0 ? (mx - mn) / mx : 0;
        opaque++; bright += v;
        if (sat < 0.28f || v < 0.22f) continue;
        float d = mx - mn, hue;
        if (mx == r) hue = 60 * std::fmod((g - b) / d + 6, 6.0f);
        else if (mx == g) hue = 60 * ((b - r) / d + 2);
        else hue = 60 * ((r - g) / d + 4);
        hist[std::clamp((int)(hue / 10), 0, 35)] += sat * v;
        colored++;
    }
    if (opaque < 1) return out;
    if (colored / opaque < 0.12) {   // no real color
        float avg = (float)(bright / opaque);
        out.cls = avg > 0.72f ? 7 : avg < 0.32f ? 9 : 8;
        out.key = 400 + (1 - avg) * 100;   // after the colors: white, gray, black
        return out;
    }
    int best = 0;
    float bestW = -1;
    for (int i = 0; i < 36; i++) {
        float w = hist[i] + 0.5f * (hist[(i + 35) % 36] + hist[(i + 1) % 36]);
        if (w > bestW) { bestW = w; best = i; }
    }
    // The hue inside the winning band and its neighbors (on the circle)
    float x = 0, y = 0;
    for (int k = -1; k <= 1; k++) {
        int i = (best + k + 36) % 36;
        float ang = (i * 10 + 5) * 3.14159265f / 180;
        x += hist[i] * std::cos(ang); y += hist[i] * std::sin(ang);
    }
    float hue = std::atan2(y, x) * 180 / 3.14159265f;
    if (hue < 0) hue += 360;
    out.cls = hue < 14 || hue >= 340 ? 0 : hue < 42 ? 1 : hue < 70 ? 2 : hue < 165 ? 3 : hue < 255 ? 4 : hue < 290 ? 5 : 6;
    out.key = hue >= 340 ? hue - 360 : hue;   // red first
    return out;
}

// Colors already found for a repository, kept next to its pictures so they are not read again
std::wstring GhColorsFile(const GhRepo& r) { return r.branch.empty() ? std::wstring() : GhCacheFile(r, L"~colors.tsv"); }

void GhLoadColors() {
    std::wstring file = GhColorsFile(g_gh.repo);
    HANDLE h = file.empty() ? INVALID_HANDLE_VALUE : CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    std::string data;
    char buffer[65536];
    DWORD got = 0;
    while (ReadFile(h, buffer, sizeof(buffer), &got, nullptr) && got > 0 && data.size() < (32u << 20)) data.append(buffer, got);
    CloseHandle(h);
    for (const std::wstring& line : Split(FromUtf8(data), L'\n')) {
        auto f = Split(line, L'\t');
        if (f.size() != 3 || f[0].empty()) continue;
        GhColor c{_wtoi(f[1].c_str()), (float)_wtof(f[2].c_str())};
        if (c.cls >= -1 && c.cls < kGhColorCount) g_ghColors.emplace(f[0], c);
    }
}

void GhSaveColors() {
    g_ghColorsNew = 0;
    std::wstring file = GhColorsFile(g_gh.repo);
    if (file.empty() || g_ghColors.empty()) return;
    SHCreateDirectoryExW(nullptr, file.substr(0, file.find_last_of(L'\\')).c_str(), nullptr);
    std::wstring text;
    for (auto& [path, c] : g_ghColors) {
        wchar_t num[48];
        swprintf(num, 48, L"\t%d\t%.2f\n", c.cls, c.key);
        text += Clean(path) + num;
    }
    std::string data = ToUtf8(text);
    HANDLE h = CreateFileW(file.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD put = 0;
    WriteFile(h, data.data(), (DWORD)data.size(), &put, nullptr);
    CloseHandle(h);
}

struct GhListJob { std::wstring key; GhRepo repo; };
DWORD WINAPI GhListThread(void* param) {
    std::unique_ptr<GhListJob> job(static_cast<GhListJob*>(param));
    auto* result = new GhListResult{job->key, job->repo.branch, {}, {}, {}, {}};
    const GhRepo& r = job->repo;
    std::string body;
    std::wstring api = L"https://api.github.com/repos/" + r.owner + L"/" + r.repo;
    auto fail = [&](DWORD status) {
        result->error = status == 0 ? CfgT(L"No connection to GitHub", L"Sem conexão com o GitHub")
                      : status == 404 ? CfgT(L"Repository or folder not found", L"Repositório ou pasta não encontrado")
                      : status == 403 || status == 429 ? CfgT(L"GitHub's limit for now was reached; try again in a while", L"Limite do GitHub atingido por agora; tente daqui a pouco")
                      : CfgT(L"GitHub didn't answer as expected", L"O GitHub não respondeu como esperado");
    };
    if (result->branch.empty()) {   // the repository's main branch
        DWORD status = HttpGet(api, body);
        size_t at = body.find("\"default_branch\"");
        if (status != 200 || at == std::string::npos) fail(status);
        else {
            size_t q = body.find('"', body.find(':', at) + 1);
            size_t pos = q + 1;
            if (q != std::string::npos) result->branch = FromUtf8(JsonString(body, pos));
        }
    }
    if (result->error.empty() && !result->branch.empty()) {   // the branch's latest commit (falls back to the branch name)
        DWORD status = HttpGet(api + L"/branches/" + UrlPath(result->branch), body);
        size_t at = status == 200 ? body.find("\"commit\"") : std::string::npos;
        size_t shaAt = at == std::string::npos ? at : body.find("\"sha\"", at);
        if (shaAt != std::string::npos) {
            size_t pos = body.find('"', body.find(':', shaAt) + 1) + 1;
            result->commit = FromUtf8(JsonString(body, pos));
        }
    }
    if (result->error.empty() && !result->branch.empty()) {
        std::wstring tree = result->commit.empty() ? result->branch : result->commit;
        DWORD status = HttpGet(api + L"/git/trees/" + UrlPath(tree) + L"?recursive=1", body);
        if (status != 200) fail(status);
        else {
            std::wstring prefix = r.folder.empty() ? L"" : r.folder + L"/";
            std::vector<std::pair<std::wstring, std::wstring>> found;   // path, version
            for (size_t at = body.find("\"path\""); at != std::string::npos && !g_ghStop; at = body.find("\"path\"", at + 6)) {
                size_t q = body.find('"', body.find(':', at) + 1);
                if (q == std::string::npos) break;
                size_t pos = q + 1;
                std::wstring path = FromUtf8(JsonString(body, pos));
                size_t next = body.find("\"path\"", pos), type = body.find("\"type\"", pos);
                bool blob = type != std::string::npos && (next == std::string::npos || type < next) && body.compare(body.find('"', body.find(':', type) + 1) + 1, 4, "blob") == 0;
                if (!blob || !GhImagePath(path) || path.compare(0, prefix.size(), prefix) != 0) continue;
                std::wstring sha;
                size_t shaAt = body.find("\"sha\"", pos);
                if (shaAt != std::string::npos && (next == std::string::npos || shaAt < next)) {
                    size_t p2 = body.find('"', body.find(':', shaAt) + 1) + 1;
                    sha = FromUtf8(JsonString(body, p2));
                }
                found.push_back({path, sha});
            }
            std::stable_sort(found.begin(), found.end(), [](const auto& a, const auto& b) { return NameLess(a.first, b.first); });
            for (auto& [path, sha] : found) { result->paths.push_back(std::move(path)); result->shas.push_back(std::move(sha)); }
        }
    }
    HWND launcher = g_wnd;
    if (g_ghStop || !launcher || !PostMessageW(launcher, WM_APP_GHLIST, 0, (LPARAM)result)) delete result;
    return 0;
}

// Fetches the pictures in the queue, one after the other, and sends them decoded to the window
DWORD WINAPI GhIconThread(void*) {
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    while (!g_ghStop) {
        std::wstring path;
        GhRepo repo;
        AcquireSRWLockExclusive(&g_ghLock);
        if (!g_ghQueue.empty()) { path = std::move(g_ghQueue.front()); g_ghQueue.pop_front(); repo = g_ghQueueRepo; }
        ReleaseSRWLockExclusive(&g_ghLock);
        if (path.empty()) { WaitForSingleObject(g_ghWake, 1000); continue; }
        auto* result = new GhIconResult{path, {}, 0, {}};
        std::wstring file;
        App temp;
        if (GhEnsureFile(repo, path, file, &result->replaced) && LoadImageFile(file, CurrentIconPixels(), temp)) {
            result->color = GhClassify(temp.pixels);
            result->pixels = std::move(temp.pixels);
            result->size = temp.iconSize;
        }
        HWND launcher = g_wnd;
        if (g_ghStop || !launcher || !PostMessageW(launcher, WM_APP_GHICON, 0, (LPARAM)result)) delete result;
    }
    if (SUCCEEDED(com)) CoUninitialize();
    return 0;
}

void GhStopThreads() {
    g_ghStop = true;
    if (g_ghWake) SetEvent(g_ghWake);
    std::vector<HANDLE*> threads = {&g_ghListThread};
    for (HANDLE& h : g_ghIconThreads) threads.push_back(&h);
    for (HANDLE* h : threads)
        if (*h) { WaitForSingleObject(*h, INFINITE); CloseHandle(*h); *h = nullptr; }
    if (g_ghColorsNew) GhSaveColors();
    GhSaveFileShas();
    if (g_ghWake) { CloseHandle(g_ghWake); g_ghWake = nullptr; }
    g_ghStop = false;
    MSG msg;   // results that were already on their way
    while (PeekMessageW(&msg, nullptr, WM_APP_GHLIST, WM_APP_GHLIST, PM_REMOVE)) delete (GhListResult*)msg.lParam;
    while (PeekMessageW(&msg, nullptr, WM_APP_GHICON, WM_APP_GHICON, PM_REMOVE)) delete (GhIconResult*)msg.lParam;
}

// The first item the grid shows at its current scroll
int GhFirstVisible() {
    const Layout& L = g_layout;
    if (L.columns <= 0) return 0;
    if (L.horizontal) return (int)(g_scrollTarget / std::max(1.0f, L.pageWidth)) * std::max(1, L.perPage);
    return (int)(g_scrollTarget / std::max(1.0f, L.cellHeight)) * L.columns;
}
int g_ghQueuedFrom = -1;   // the first visible item when the queue was last made

// The pictures still missing for what the grid shows now (in its order), for the download thread
void GhQueueVisible() {
    if (!g_gh.active) return;
    std::deque<std::wstring> wanted;
    std::unordered_set<std::wstring> queued;
    g_ghOnScreen.clear();
    // From a little before the first row on screen (the grid may be scrolled far down a big repository)
    int first = std::max(0, GhFirstVisible() - 60);
    g_ghQueuedFrom = GhFirstVisible();
    for (int k = first; k < (int)g_layoutItems.size(); k++) {
        App* a = ResolveApp(g_layoutItems[k]);
        if (!a || a->kind != 5 || g_ghOnScreen.size() >= 400) continue;
        g_ghOnScreen.insert(a->target);
        if (a->pixels.empty() && a->iconSize >= 0) { wanted.push_back(a->target); queued.insert(a->target); }
    }
    // Pictures far from the screen let go of their pixels (a big repository would fill the memory); they are read
    // again from the disk when the grid comes back to them
    for (auto& [path, item] : g_ghItems)
        if (!item->pixels.empty() && !g_ghOnScreen.count(path)) {
            item->pixels.clear(); item->pixels.shrink_to_fit(); item->bitmap.Reset(); item->iconSize = 0; item->generation = 0;
        }
    // Filtering or ordering by color needs the color of every picture: the ones not read yet come after
    if (GhColorMode())
        for (const std::wstring& path : g_gh.paths)
            if (!g_ghColors.count(path) && !queued.count(path)) wanted.push_back(path);
    AcquireSRWLockExclusive(&g_ghLock);
    g_ghQueue = std::move(wanted);
    g_ghQueueRepo = g_gh.repo;
    ReleaseSRWLockExclusive(&g_ghLock);
    if (!g_ghWake) g_ghWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    // One thread for the pictures on screen; more while the colors of the whole repository are being found
    int threads = g_gh.paths.empty() ? 0 : GhColorMode() ? kGhIconThreads : 1;
    for (int i = 0; i < threads; i++)
        if (!g_ghIconThreads[i]) g_ghIconThreads[i] = CreateThread(nullptr, 0, GhIconThread, nullptr, 0, nullptr);
    if (g_ghWake) for (int i = 0; i < threads; i++) SetEvent(g_ghWake);
}

// Called on every frame: when the gallery was scrolled to other rows, the pictures there are asked for
void GhScrolled() {
    if (!g_gh.active || g_siriTab != 4 || g_gh.paths.empty()) return;
    int first = GhFirstVisible();
    if (std::abs(first - g_ghQueuedFrom) >= std::max(1, g_layout.columns)) GhQueueVisible();
}

void GhItems(AppList& out) {
    for (const std::wstring& path : g_gh.paths) {
        auto& item = g_ghItems[path];
        if (!item) {
            item = std::make_shared<App>();
            item->kind = 5;
            item->target = path;
            std::wstring name = path.substr(path.find_last_of(L'/') + 1);
            item->name = name.substr(0, name.find_last_of(L'.'));
        }
        out.push_back(item);
    }
}

// The gallery's grid: names with the typed text, of the chosen color, alphabetical or in rainbow order
void GhFilterItems(std::vector<int>& out, const std::wstring& q) {
    out.clear();
    for (int i = 0; i < (int)g_tabItems.size(); i++) {
        const App& a = *g_tabItems[i];
        if (ToLower(a.name).find(q) == std::wstring::npos) continue;
        if (g_gh.colorFilter >= 0) {
            auto c = g_ghColors.find(a.target);
            if (c == g_ghColors.end() || c->second.cls != g_gh.colorFilter) continue;
        }
        out.push_back(kTabBase + i);
    }
    if (!g_gh.byColor) return;
    auto key = [](int item) {
        auto c = g_ghColors.find(g_tabItems[item - kTabBase]->target);
        return c == g_ghColors.end() || c->second.cls < 0 ? 100000.0f : c->second.key;   // unknown ones last
    };
    std::stable_sort(out.begin(), out.end(), [&](int x, int y) { return key(x) < key(y); });
}

// How many pictures still need their color read
int GhColorsMissing() {
    int n = 0;
    for (const std::wstring& path : g_gh.paths) if (!g_ghColors.count(path)) n++;
    return n;
}

PCWSTR GhMessage() {
    static std::wstring text;
    if (g_gh.loading) text = CfgT(L"Loading the icons from GitHub...", L"Carregando os ícones do GitHub...");
    else if (!g_gh.error.empty()) text = g_gh.error;
    else if (g_gh.paths.empty()) text = CfgT(L"No images (PNG, ICO, JPG...) in this repository", L"Nenhuma imagem (PNG, ICO, JPG...) neste repositório");
    else if (GhColorMode() && GhColorsMissing() > 0) text = CfgT(L"Finding the colors of the icons...", L"Descobrindo as cores dos ícones...");
    else text = CfgT(L"No icon found", L"Nenhum ícone encontrado");
    return text.c_str();
}

void GhClose(bool render = true) {
    if (!g_gh.active) return;
    g_gh.active = false;
    AcquireSRWLockExclusive(&g_ghLock);
    g_ghQueue.clear();
    ReleaseSRWLockExclusive(&g_ghLock);
    if (g_wnd) KillTimer(g_wnd, kTimerGhColor);
    if (g_ghColorsNew) GhSaveColors();
    GhSaveFileShas();
    if (g_siriTab != 4) return;
    g_siriTab = 0;
    ClearQuery();
    if (!render) return;   // another category was chosen instead
    // Back to where the launcher was: same category, text, folder and scroll, with the changed app selected
    const auto& back = g_gh.back;
    if (back.tab != 4 && TabEnabled(back.tab)) g_siriTab = back.tab;
    if (!back.query.empty()) SetQuery(back.query);
    if (!g_settings.siri) g_searchActive = back.searchActive;
    RefreshTabItems(false);
    g_scroll = g_scrollTarget = std::clamp(back.scroll, 0.0f, MaxScroll());
    if (back.folder >= 0 && back.folder < (int)g_folders.size()) {
        OpenFolder(back.folder);
        const auto& apps = g_folders[back.folder].apps;
        for (int i = 0; i < (int)apps.size(); i++)
            if (apps[i]->target == g_gh.target) { g_selected = i; EnsureVisible(i); g_scroll = g_scrollTarget; break; }
    } else {
        for (int i = 0; i < (int)g_layoutItems.size(); i++) {
            App* a = ResolveApp(g_layoutItems[i]);
            if (a && a->target == g_gh.target) { g_selected = i; EnsureVisible(i); g_scroll = g_scrollTarget; break; }
        }
    }
    ComputeLayout();
    Render();
    StartFrames();
}

// Right-click an app > "Icon from GitHub..."
void GhOpen(App* app) {
    if (!app) return;
    GhRepo repo;
    if (!ParseGhRepo(g_ghSetting, repo)) {
        MessageBoxW(g_wnd, CfgT(L"Set the repository first: settings (⋯) > Apps and icons > Icons from GitHub, for example\n"
                                L"owner/repository  or  https://github.com/owner/repository/tree/main/icons",
                                L"Informe o repositório primeiro: ajustes (⋯) > Apps e ícones > Ícones do GitHub, por exemplo\n"
                                L"dono/repositório  ou  https://github.com/dono/repositório/tree/main/icones"),
                    L"Siri Plus Launchpad", MB_ICONINFORMATION);
        if (g_shown) ForceForeground(g_wnd);
        return;
    }
    if (g_ghSetting != g_gh.key) {   // another repository: start over
        if (g_ghColorsNew) GhSaveColors();
        GhSaveFileShas();
        g_gh.paths.clear(); g_gh.error.clear(); g_ghItems.clear(); g_ghColors.clear();
        g_gh.colorFilter = -1; g_gh.byColor = false;
        g_gh.key = g_ghSetting;
        g_gh.repo = repo;
        g_gh.loading = g_gh.refreshing = false;
        g_gh.listTime = -1000;
        AcquireSRWLockExclusive(&g_ghLock);
        g_ghWantSha.clear(); g_ghFileSha.clear();
        ReleaseSRWLockExclusive(&g_ghLock);
        g_ghFileShaLoaded = g_ghFileShaDirty = false;
    }
    g_gh.target = app->target;
    g_gh.active = true;
    // Where the launcher is now, to come back to it
    g_gh.back.tab = g_siriTab;
    g_gh.back.query = g_query;
    g_gh.back.searchActive = g_searchActive;
    g_gh.back.folder = g_openFolderIndex;
    g_gh.back.scroll = g_openFolderIndex >= 0 ? g_mainScroll : g_scrollTarget;
    if (g_openFolderIndex >= 0) CloseFolder(false);
    // The list is read again when it is old: pictures added or replaced in the repository show up without restarting
    if (!g_gh.refreshing && (g_gh.paths.empty() || Now() - g_gh.listTime > kGhListRefresh)) {
        if (g_ghListThread) { WaitForSingleObject(g_ghListThread, INFINITE); CloseHandle(g_ghListThread); g_ghListThread = nullptr; }
        g_gh.loading = g_gh.paths.empty(); g_gh.refreshing = true; g_gh.error.clear();
        g_ghListThread = CreateThread(nullptr, 0, GhListThread, new GhListJob{g_gh.key, g_gh.repo}, 0, nullptr);
        if (!g_ghListThread) {
            g_gh.loading = g_gh.refreshing = false;
            if (g_gh.paths.empty()) g_gh.error = CfgT(L"Could not start the download", L"Não foi possível iniciar o download");
        }
    }
    // The grid now shows the repository; the bar searches it
    if (g_menu.open) g_menu.open = false;
    g_selection.clear();
    ClearQuery();
    g_siriTab = 4;
    if (g_settings.siri) { g_siriChosen = true; ExpandSiri(); } else g_searchActive = true;
    RefreshTabItems(true);
    GhQueueVisible();
    StartFrames();
}

void OnGhList(GhListResult* result) {
    std::unique_ptr<GhListResult> r(result);
    if (r->key != g_gh.key) return;
    bool first = g_gh.paths.empty();
    g_gh.loading = g_gh.refreshing = false;
    g_gh.listTime = Now();
    if (!r->error.empty() && !first) return;   // reading it again failed: the list already shown stays
    g_gh.error = r->error;
    g_gh.paths = std::move(r->paths);
    g_gh.repo.branch = r->branch;
    g_gh.repo.commit = r->commit;
    if (!g_ghFileShaLoaded) GhLoadFileShas();
    if (first) { g_ghColors.clear(); GhLoadColors(); }
    // Pictures replaced in the repository: their old picture and color are dropped, the new ones come in
    AcquireSRWLockExclusive(&g_ghLock);
    g_ghWantSha.clear();
    std::vector<std::wstring> changed;
    for (size_t i = 0; i < g_gh.paths.size() && i < r->shas.size(); i++) {
        if (r->shas[i].empty()) continue;
        g_ghWantSha[g_gh.paths[i]] = r->shas[i];
        auto have = g_ghFileSha.find(g_gh.paths[i]);
        if (have == g_ghFileSha.end() || have->second != r->shas[i]) changed.push_back(g_gh.paths[i]);
    }
    ReleaseSRWLockExclusive(&g_ghLock);
    for (const std::wstring& path : changed) {
        if (g_ghColors.erase(path)) g_ghColorsNew++;
        auto item = g_ghItems.find(path);
        if (item != g_ghItems.end()) { App& a = *item->second; a.pixels.clear(); a.iconSize = 0; a.bitmap.Reset(); a.generation = 0; }
    }
    if (g_gh.active && g_siriTab == 4) { RefreshTabItems(first); GhQueueVisible(); StartFrames(); }
}

void OnGhIcon(GhIconResult* result) {
    std::unique_ptr<GhIconResult> r(result);
    auto found = g_ghItems.find(r->path);
    if (found == g_ghItems.end()) return;
    App& a = *found->second;
    // Only pictures near the screen keep their pixels (memory); the others are read again from the disk later
    bool keep = g_ghOnScreen.count(r->path) != 0;
    if (r->size > 0 && !keep) {}
    else if (r->size > 0) { a.pixels = std::move(r->pixels); a.iconSize = r->size; a.bitmap.Reset(); a.generation = 0; }
    else a.iconSize = -1;   // could not be read: not asked again
    if (!g_ghColors.count(r->path)) { g_ghColors[r->path] = r->color; g_ghColorsNew++; }
    if (r->replaced) {   // a newer version of a picture: apps that use it as their icon show the new one
        std::wstring file = GhCacheFile(g_gh.repo, r->path);
        auto refresh = [&](App& app) {
            auto c = g_customIcons.find(app.target);
            if (c != g_customIcons.end() && c->second == file && LoadImageFile(file, CurrentIconPixels(), app)) { app.bitmap.Reset(); app.generation = 0; }
        };
        for (auto& app : g_apps) refresh(*app);
        for (auto& f : g_folders) for (auto& app : f.apps) refresh(*app);
    }
    if (g_gh.active && g_shown && GhColorMode()) SetTimer(g_wnd, kTimerGhColor, 250, nullptr);
    if (g_gh.active && g_shown) Render();
}

void OnGhColorTimer() {
    KillTimer(g_wnd, kTimerGhColor);
    if (!g_gh.active || g_siriTab != 4) return;
    if (g_ghColorsNew >= 200) GhSaveColors();
    // Keep the grid where it is: only the items change
    LoadTabSource();
    RebuildLayout();
    g_hover = -1;
    if (g_selected >= (int)g_layoutItems.size()) g_selected = g_layoutItems.empty() ? -1 : 0;
    ComputeLayout();
    Render();
    GhQueueVisible();
}

// A picture was clicked: it becomes the app's icon
void GhApply(const std::wstring& path) {
    std::wstring file;
    if (!GhEnsureFile(g_gh.repo, path, file)) {
        MessageBoxW(g_wnd, CfgT(L"Could not download this image.", L"Não foi possível baixar esta imagem."), L"Siri Plus Launchpad", MB_ICONWARNING);
        if (g_shown) ForceForeground(g_wnd);
        return;
    }
    std::wstring target = g_gh.target;
    bool applied = false;
    auto apply = [&](App& a) {
        if (a.target == target && LoadImageFile(file, CurrentIconPixels(), a)) { a.bitmap.Reset(); a.generation = 0; applied = true; }
    };
    for (auto& a : g_apps) apply(*a);
    for (auto& f : g_folders) for (auto& a : f.apps) apply(*a);
    if (!applied) {
        MessageBoxW(g_wnd, CfgT(L"Could not load this image.", L"Não foi possível carregar esta imagem."), L"Siri Plus Launchpad", MB_ICONWARNING);
        if (g_shown) ForceForeground(g_wnd);
        return;
    }
    g_customIcons[target] = file;
    SaveCustomIcons();
    GhClose();
}

// The color dots at the right of the gallery's search field: [rainbow order] red ... black
constexpr int kGhDotCount = kGhColorCount + 1;
bool GhDots(const Layout& L, D2D1_RECT_F dots[kGhDotCount], float& left) {
    const D2D1_RECT_F& r = L.siriTabs;
    float s = L.scale, h = r.bottom - r.top;
    if (r.right <= r.left || g_gh.loading || g_gh.paths.empty()) { left = r.right; return false; }
    float step = std::min(std::round(21 * s), (r.right - r.left) * 0.42f / kGhDotCount), d = std::min(step - 4 * s, std::round(14 * s));
    float x = r.right - std::round(10 * s) - step * kGhDotCount, cy = (r.top + r.bottom) / 2;
    left = x - std::round(6 * s);
    for (int i = 0; i < kGhDotCount; i++) {
        float cx = x + step * i + step / 2;
        dots[i] = D2D1::RectF(cx - d / 2, cy - d / 2, cx + d / 2, cy + d / 2);
    }
    (void)h;
    return true;
}

// A dot was clicked: 0 = rainbow order (on / off), 1.. = only that color (again = all colors)
bool GhDotClick(float x, float y) {
    D2D1_RECT_F dots[kGhDotCount];
    float left;
    if (!g_gh.active || !GhDots(g_layout, dots, left)) return false;
    float s = g_layout.scale;
    for (int i = 0; i < kGhDotCount; i++) {
        D2D1_RECT_F hit = D2D1::RectF(dots[i].left - 3 * s, g_layout.siriTabs.top, dots[i].right + 3 * s, g_layout.siriTabs.bottom);
        if (!InRect(hit, x, y)) continue;
        if (i == 0) g_gh.byColor = !g_gh.byColor;
        else g_gh.colorFilter = g_gh.colorFilter == i - 1 ? -1 : i - 1;
        RefreshTabItems(true);
        StartFrames();
        return true;
    }
    return false;
}

void DrawGhDots(const Layout& L, const D2D1_RECT_F dots[kGhDotCount]) {
    float s = L.scale;
    for (int i = 0; i < kGhDotCount; i++) {
        const D2D1_RECT_F& d = dots[i];
        float rad = (d.right - d.left) / 2, cx = (d.left + d.right) / 2, cy = (d.top + d.bottom) / 2;
        D2D1_ELLIPSE e = D2D1::Ellipse(D2D1::Point2F(cx, cy), rad, rad);
        bool on = i == 0 ? g_gh.byColor : g_gh.colorFilter == i - 1;
        bool dim = g_gh.colorFilter >= 0 && i > 0 && !on;
        if (i == 0) {   // rainbow: the icons in color order
            ComPtr<ID2D1GradientStopCollection> stops;
            ComPtr<ID2D1LinearGradientBrush> rainbow;
            D2D1_GRADIENT_STOP st[] = {{0.0f, D2D1::ColorF(0xFF3B30)}, {0.25f, D2D1::ColorF(0xFFCC00)}, {0.5f, D2D1::ColorF(0x34C759)},
                                       {0.75f, D2D1::ColorF(0x0A84FF)}, {1.0f, D2D1::ColorF(0xAF52DE)}};
            if (SUCCEEDED(g_target->CreateGradientStopCollection(st, ARRAYSIZE(st), stops.GetAddressOf())) &&
                SUCCEEDED(g_target->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(d.left, d.top), D2D1::Point2F(d.right, d.bottom)),
                                                              stops.Get(), rainbow.GetAddressOf())))
                g_target->FillEllipse(e, rainbow.Get());
        } else {
            D2D1_COLOR_F c = D2D1::ColorF(kGhColorRgb[i - 1]);
            if (dim) c.a = 0.35f;
            g_brush->SetColor(c);
            g_target->FillEllipse(e, g_brush.Get());
        }
        g_brush->SetColor(Fg(dim ? 0.15f : 0.35f));   // keeps white and black visible on any theme
        g_target->DrawEllipse(e, g_brush.Get(), 1.0f);
        if (on) {
            g_brush->SetColor(Fg(0.9f));
            g_target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), rad + 2.5f * s, rad + 2.5f * s), g_brush.Get(), 1.5f * s);
        }
    }
}

// GitHub gallery: the field where the icon's name is typed (it shows the same text as the bar), with the count
void DrawGhSearchBox(const Layout& L) {
    const D2D1_RECT_F& r = L.siriTabs;
    if (r.right <= r.left) return;
    float s = L.scale, h = r.bottom - r.top, rad = h / 2;
    CfgFormats(s);
    Fill(r, Fg(0.09f), rad);
    g_brush->SetColor(Fg(0.22f));
    g_target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(r.left + 0.5f, r.top + 0.5f, r.right - 0.5f, r.bottom - 0.5f), rad, rad), g_brush.Get(), 1.0f);
    D2D1_RECT_F glyph = D2D1::RectF(r.left + 8 * s, r.top, r.left + 8 * s + h, r.bottom);
    CfgText(L"\uE721", g_cfgGlyphS.Get(), glyph, Fg(0.65f));
    // The color dots on the right, and before them how many icons match (or how the color reading goes)
    D2D1_RECT_F dots[kGhDotCount];
    float dotsLeft = r.right;
    if (GhDots(L, dots, dotsLeft)) DrawGhDots(L, dots);
    std::wstring count;
    if (!g_gh.loading && !g_gh.paths.empty()) {
        wchar_t text[96];
        int missing = GhColorMode() ? GhColorsMissing() : 0;
        if (missing > 0) {
            int total = (int)g_gh.paths.size();
            swprintf(text, 96, CfgT(L"%d icons · colors %d%%", L"%d ícones · cores %d%%"), (int)g_layoutItems.size(), (total - missing) * 100 / std::max(1, total));
        } else swprintf(text, 96, CfgT(L"%d icons", L"%d ícones"), (int)g_layoutItems.size());
        count = text;
    }
    float countRight = dotsLeft < r.right ? dotsLeft : r.right - 12 * s;
    float countW = count.empty() ? 0 : CfgTextWidth(count, g_cfgBody.Get()) + 14 * s;
    if (!count.empty()) CfgText(count, g_cfgBody.Get(), D2D1::RectF(countRight - countW + 12 * s, r.top, countRight, r.bottom), Fg(0.45f));
    countW += r.right - countRight - 12 * s;
    D2D1_RECT_F text = D2D1::RectF(glyph.right + 2 * s, r.top, r.right - countW - 8 * s, r.bottom);
    if (g_query.empty()) {
        CfgText(CfgT(L"Type the icon's name...", L"Digite o nome do ícone..."), g_cfgBody.Get(), text, Fg(0.45f));
    } else {
        g_target->PushAxisAlignedClip(text, D2D1_ANTIALIAS_MODE_ALIASED);
        float w = CfgTextWidth(g_query, g_cfgBody.Get()), x = std::min(text.left, text.right - w - 3 * s);
        CfgText(g_query, g_cfgBody.Get(), D2D1::RectF(x, text.top, x + w + 4 * s, text.bottom), Fg(0.95f));
        g_brush->SetColor(Fg(0.85f));
        float cx = x + w + 1 * s, cy = (r.top + r.bottom) / 2;
        g_target->DrawLine(D2D1::Point2F(cx, cy - 8 * s), D2D1::Point2F(cx, cy + 8 * s), g_brush.Get(), 1.2f * s);
        g_target->PopAxisAlignedClip();
    }
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

// ---------- Dragging the Siri bar ----------
// Held still on the bar for a moment: it comes loose and follows the mouse (the whole launcher moves with it)
void BarDragGrab(HWND hwnd) {
    KillTimer(hwnd, kTimerBarHold);
    if (!g_barDrag.pending) return;
    g_barDrag.pending = false;
    POINT p;
    GetCursorPos(&p);
    bool held = (GetAsyncKeyState(GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON) & 0x8000) != 0;
    if (!held || std::abs(p.x - g_barDrag.start.x) > 6 || std::abs(p.y - g_barDrag.start.y) > 6 || !g_shown || g_closing) return;
    g_queryDragging = false;
    g_menu.open = false;   // the categories step aside while it moves
    g_barDrag.active = true;
    g_barDrag.grab = p;
    g_barDrag.startArea = g_layout.area;
    SetCapture(hwnd);
    SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
    Render();
}

void BarDragMove() {
    POINT p;
    GetCursorPos(&p);
    const RECT& from = g_barDrag.startArea;
    int w = from.right - from.left, h = from.bottom - from.top;
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST), &mi);
    const RECT& work = mi.rcWork;
    int left = from.left + p.x - g_barDrag.grab.x, top = from.top + p.y - g_barDrag.grab.y;
    // Only the bar has to stay on the screen (any monitor); the window around it may go past the edges meanwhile
    const D2D1_RECT_F& bar = g_layout.search;
    int bl = (int)bar.left, bt = (int)bar.top, bw = (int)(bar.right - bar.left), bh = (int)(bar.bottom - bar.top);
    left = std::clamp(left, (int)work.left - bl, std::max((int)work.left - bl, (int)work.right - bl - bw));
    top = std::clamp(top, (int)work.top - bt, std::max((int)work.top - bt, (int)work.bottom - bt - bh));
    if (left == g_layout.area.left && top == g_layout.area.top) return;
    SetArea(g_layout, RECT{left, top, left + w, top + h});
    Render();
}

// Let go: the place is kept for the next time (relative to the monitor's work area)
void BarDragDrop(HWND hwnd) {
    g_barDrag.active = false;
    if (GetCapture() == hwnd) ReleaseCapture();
    const Layout& L = g_layout;
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &mi);
    int workW = std::max(1, (int)(mi.rcWork.right - mi.rcWork.left)), workH = std::max(1, (int)(mi.rcWork.bottom - mi.rcWork.top));
    int center = L.area.left + (int)std::lround((L.search.left + L.search.right) / 2), barTop = L.area.top + (int)std::lround(L.search.top);
    g_barPosX = std::clamp((int)(center - mi.rcWork.left) * 1000 / workW, 0, 1000);
    g_barPosY = std::clamp((int)(barTop - mi.rcWork.top) * 1000 / workH, 0, 1000);
    Wh_SetIntValue(L"barX", g_barPosX);
    Wh_SetIntValue(L"barY", g_barPosY);
    MONITORINFOEXW named{};
    named.cbSize = sizeof(named);
    if (GetMonitorInfoW(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &named)) Wh_SetStringValue(L"barMonitor", named.szDevice);
    // The window settles around the bar (results box above it when it is low, off-center near a side)
    SetArea(g_layout, SiriAreaFor(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), g_layout.scale));
    ComputeLayout();
    FitCategoryMenu();
    Render();
}

// The bar and the categories menu together (the menu hangs below the bar, or above it when the bar sits low):
// the menu stays open while the mouse is anywhere in here
D2D1_RECT_F CategoryZone() {
    const D2D1_RECT_F& b = g_layout.search;
    const D2D1_RECT_F& m = g_menu.rect;
    return D2D1::RectF(std::min(b.left, m.left), std::min(b.top, m.top), std::max(b.right, m.right), std::max(b.bottom, m.bottom));
}

// Asks for WM_MOUSELEAVE once per stay of the mouse over the window
void TrackLeave(HWND hwnd) {
    if (g_trackingMouse) return;
    TRACKMOUSEEVENT request{};
    request.cbSize = sizeof(request);
    request.dwFlags = TME_LEAVE;
    request.hwndTrack = hwnd;
    g_trackingMouse = TrackMouseEvent(&request) != FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (g_cfg.active && !g_closing) {   // the settings panel takes the mouse and the keyboard
        LRESULT result = 0;
        if (CfgMessage(hwnd, msg, wParam, lParam, result)) return result;
    }
    switch (msg) {
        case WM_APP_TOGGLE:
            if ((!g_shown || g_closing) && Now() - g_lastClosed < 0.5) return 0;
            Toggle(); return 0;
        case WM_APP_APPS: OnAppsLoaded((AppList*)lParam, (int)wParam); return 0;
        case WM_APP_SHORTCUT: OnCategoryShortcut((int)wParam); return 0;
        case WM_APP_FILES: OnFilesFound((FileResult*)lParam); return 0;
        case WM_APP_GHLIST: OnGhList((GhListResult*)lParam); return 0;
        case WM_APP_GHICON: OnGhIcon((GhIconResult*)lParam); return 0;
        case WM_APP_RELOAD:
            LoadSettings();
            if (g_shown) HideNow();
            if (!g_settings.clipEnabled) g_clipHistory.clear();   // off: the history is cleared
            else if ((int)g_clipHistory.size() > g_settings.clipMax) g_clipHistory.resize(g_settings.clipMax);
            g_fileStatus.clear(); // Everything IPC needs no reload (no DLL)
            return 0;
        case WM_APP_QUIT: DestroyWindow(hwnd); return 0;
        case WM_CLIPBOARDUPDATE: g_clipRetries = 0; OnClipboardUpdate(); return 0;
        case WM_TIMER:
            if (wParam == kTimerFrame) OnFrame();
            else if (wParam == kTimerFileSearch) {
                KillTimer(hwnd, kTimerFileSearch);
                if (g_shown && g_siriTab == 1) { StartFileSearch(g_query); Render(); }
            }
            else if (wParam == kTimerMenuLeave) {
                if (!g_menu.open || !g_menu.category || g_menu.closing || g_menu.keyboard || g_trackingMouse) { KillTimer(hwnd, kTimerMenuLeave); return 0; }
                POINT p;
                GetCursorPos(&p);
                ScreenToClient(hwnd, &p);
                D2D1_RECT_F zone = CategoryZone();
                if (!InRect(zone, (float)p.x, (float)p.y)) { KillTimer(hwnd, kTimerMenuLeave); CloseMenu(); }
            }
            else if (wParam == kTimerLongPress) {
                KillTimer(hwnd, kTimerLongPress);
                // A long press selects the app; it must not act like a normal
                // left click or synthesize a right-click/context-menu action.
                // (Original Siri selects only while picking apps for a tab)
                if (g_shown && (!g_settings.siriOriginal || Picking()) && g_siriTab == 0 && g_pressed >= 0 &&
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
            else if (wParam == kTimerClipRetry) { KillTimer(hwnd, kTimerClipRetry); OnClipboardUpdate(); }
            else if (wParam == kTimerBarHold) BarDragGrab(hwnd);
            else if (wParam == kTimerGhColor) OnGhColorTimer();
            else if (wParam == kTimerLiquid) {
                // Live liquid glass: the background is retaken a few times per second (the launcher is hidden from captures)
                if (!g_shown || !LiquidSetting() || !g_settings.liveGlass) { KillTimer(hwnd, kTimerLiquid); return 0; }
                // Only the part under the glass is retaken (small = fast); not while the settings are open (frosted there)
                if (g_cfg.active) return 0;
                std::vector<GlassPiece> pieces;
                float pa = 1;
                CollectGlassPieces(g_layout, D2D1::Matrix3x2F::Identity(), pieces, pa);
                if (pieces.empty()) return 0;
                D2D1_RECT_F u = pieces[0].r;
                for (const GlassPiece& gp : pieces) {
                    u.left = std::min(u.left, gp.r.left); u.top = std::min(u.top, gp.r.top);
                    u.right = std::max(u.right, gp.r.right); u.bottom = std::max(u.bottom, gp.r.bottom);
                }
                RECT cap{g_layout.area.left + (LONG)std::floor(u.left), g_layout.area.top + (LONG)std::floor(u.top),
                         g_layout.area.left + (LONG)std::ceil(u.right), g_layout.area.top + (LONG)std::ceil(u.bottom)};
                InflateRect(&cap, kLiquidMargin, kLiquidMargin);
                if (CaptureLiquid(cap)) Render();
            }
            else if (wParam == kTimerFirstLoad && OwnsTaskbar()) { KillTimer(hwnd, kTimerFirstLoad); if (!g_appsLoaded) StartLoadingApps(); }
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_shown && !g_closing && !g_dialogOpen) {
                g_lastClosed = Now();
                if (g_cfg.active) CfgExit();   // clicking outside the settings: they close with their animation
                else Close();
            }
            return 0;
        case WM_SETCURSOR:
            if (g_barDrag.active) { SetCursor(LoadCursorW(nullptr, IDC_SIZEALL)); return TRUE; }
            if (LOWORD(lParam) == HTCLIENT) {
                POINT p; GetCursorPos(&p); ScreenToClient(hwnd, &p);
                bool hand = TabAt((float)p.x, (float)p.y) >= 0 || FilterAt((float)p.x, (float)p.y) != kNoChip ||
                            HitBack((float)p.x, (float)p.y) || HitMore((float)p.x, (float)p.y);
                SetCursor(LoadCursorW(nullptr, hand ? IDC_HAND : IDC_ARROW));
                return TRUE;
            }
            break;
        case WM_MOUSEMOVE: {
            if (g_closing) return 0;
            if (g_barDrag.active) { BarDragMove(); return 0; }
            if (g_barDrag.pending) {
                POINT p;
                GetCursorPos(&p);
                if (std::abs(p.x - g_barDrag.start.x) > 6 || std::abs(p.y - g_barDrag.start.y) > 6) { g_barDrag.pending = false; KillTimer(hwnd, kTimerBarHold); }
            }
            TrackLeave(hwnd);
            if (g_chipPress) {
                float dx = (float)GET_X_LPARAM(lParam) - g_chipPressX;
                if (!g_chipDragged && std::abs(dx) > 4 * g_layout.scale && ChipsOverflow()) g_chipDragged = true;
                if (g_chipDragged) { g_chipScroll = g_chipPressScroll - dx; ClampChipScroll(); Render(); }
                return 0;
            }
            if (g_menu.open && !g_menu.closing) {
                float mx = (float)GET_X_LPARAM(lParam), my = (float)GET_Y_LPARAM(lParam);
                if (g_menu.category) {
                    D2D1_RECT_F zone = CategoryZone();
                    bool inside = InRect(zone, mx, my);
                    if (inside) g_menu.keyboard = false;
                    else if (!g_menu.keyboard) { CloseMenu(); return 0; }
                    else return 0;   // opened by Esc: waits for the mouse or the keyboard
                }
                bool mh = HitMore(mx, my);
                if (mh != g_moreHover) { g_moreHover = mh; Render(); }
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
                // (also while the bar is still forming: the options appear as the animation ends)
                if (g_gear.shown) {
                    bool gh = InRect(GearRect(), (float)x, (float)y);
                    if (gh != g_gear.hover) { g_gear.hover = gh; Render(); }
                }
                bool mh = HitMore((float)x, (float)y);
                if (mh != g_moreHover) { g_moreHover = mh; Render(); }
                if (g_settings.siriOriginal && !g_menu.open && !g_siriExpanded && !g_siriClosingPill && g_pillIntro >= kIntroCircle &&
                    !g_gear.shown && !g_moreHover && InRect(g_layout.search, (float)x, (float)y)) {
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
                int xh = ClipRows() && hover >= 0 && InRect(ClipDeleteRect(CellAtIn(g_layout, g_scroll, hover).highlight), (float)x, (float)y) ? hover : -1;
                if (xh != g_clipXHover) { g_clipXHover = xh; Render(); }
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_trackingMouse = false; g_dotsHover = false; g_dotHover = -1; StartFrames();
            if (g_gear.hover || g_moreHover) { g_gear.hover = g_moreHover = false; Render(); }
            if (g_powerHover) { g_powerHover = false; Render(); }
            if (g_clearHover) { g_clearHover = false; Render(); }
            if (g_backHover) { g_backHover = false; Render(); }
            if (g_histClearHover) { g_histClearHover = false; Render(); }
            if (g_powerPillHover >= 0) { g_powerPillHover = -1; Render(); }
            if (g_menu.open && g_menu.hover >= 0) { g_menu.hover = -1; Render(); }
            // The mouse left the launcher (or crossed a see-through gap): the categories menu closes if it really went away
            if (g_menu.open && g_menu.category && !g_menu.keyboard) SetTimer(hwnd, kTimerMenuLeave, 120, nullptr);
            g_clipXHover = -1;
            if (g_hover >= 0) { g_hover = -1; Render(); } return 0;
        case WM_LBUTTONDOWN: {
            if (g_closing) return 0;
            g_ignoreUp = false;
            g_chipPress = false;   // a press whose release was lost (outside the window)
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (HitMore((float)x, (float)y)) { g_ignoreUp = true; g_moreHover = false; CfgEnter(); return 0; }   // "..." opens the settings
            if (g_settings.siri && !g_cfg.active && g_pillIntro >= 1.0f && g_openFolderIndex < 0 && InRect(g_layout.search, (float)x, (float)y) &&
                !HitClear((float)x, (float)y) && !HitBack((float)x, (float)y)) {
                // Holding still on the bar picks it up (see BarDragGrab); a quick click works as always
                g_barDrag.pending = true;
                GetCursorPos(&g_barDrag.start);
                SetTimer(hwnd, kTimerBarHold, 380, nullptr);
            }
            if (g_gear.shown) {
                if (g_gear.anim > 0.5f && InRect(GearRect(), (float)x, (float)y)) { g_ignoreUp = true; CfgEnter(); return 0; }
                g_gear.shown = false; g_gear.hover = false; StartFrames();   // a click anywhere else puts it away
            }
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
                float s = g_layout.scale, boxRight = g_layout.width / 2.0f + SiriPanelWidthNow(s) / 2;
                OpenMenu(std::max(boxRight, g_layout.btnPower.right) + 12 * s, g_layout.btnPower.top, PowerMenuItems());
                return 0;
            }
            if (g_openFolderIndex < 0 && PowerButtonOn() && InRect(g_layout.btnPower, (float)x, (float)y)) {
                g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                // The options open beside the button, on its side of the bar (left button: to the left; right button: to the right)
                const D2D1_RECT_F& b = g_layout.btnPower;
                float gap = 10 * g_layout.scale;
                bool right = g_settings.powerRight;
                OpenMenu(right ? b.right + gap : b.left - gap, b.top, PowerMenuItems(), !right);
                bool beside = right ? g_menu.rect.left >= b.right : g_menu.rect.right <= b.left;
                // No room on that side (the button is at the window's edge): below the button instead
                if (!beside) OpenMenu(right ? b.right : b.left, b.bottom + 6 * g_layout.scale, PowerMenuItems(), right);
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
                if (g_siriTab == 4 && GhDotClick((float)x, (float)y)) return 0;   // a color dot of the GitHub gallery
                if (g_siriTab == 4 && InRect(g_layout.siriTabs, (float)x, (float)y)) {   // the icon search field: typing goes there
                    if (!g_settings.siri) g_searchActive = true;
                    return 0;
                }
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
            if (ClipRows()) {
                int idx = ItemAt(x, y);
                if (idx >= 0 && InRect(ClipDeleteRect(CellAtIn(g_layout, g_scroll, idx).highlight), (float)x, (float)y)) {
                    if (App* a = ResolveApp(g_layoutItems[idx]); a && a->kind == 2) { g_clipXHover = -1; RemoveClipItem(a->target); }
                    g_ignoreUp = true;
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
            if (g_pressed >= 0 && g_openFolderIndex < 0)
                SetTimer(hwnd, kTimerLongPress, kLongPressDelayMs, nullptr);
            Render(); return 0;
        }
        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (g_barDrag.pending) { g_barDrag.pending = false; KillTimer(hwnd, kTimerBarHold); }
            if (g_barDrag.active) { BarDragDrop(hwnd); g_ignoreUp = false; return 0; }
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
                else if (ItemAt(x, y) == g_pressed) {
                    // Selection mode: after a long press selected an app, a simple click selects (or deselects) the others
                    App* app = SelectableApp(g_pressed);
                    bool selecting = !g_selection.empty() && (Picking() || !g_settings.siriOriginal);
                    if (selecting && app) {
                        if (!g_selection.erase(app)) g_selection.insert(app);
                        StartFrames();
                    } else if (!Picking()) LaunchApp(g_layoutItems[g_pressed]);
                }
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
                if (g_settings.siri && g_barPosX >= 0) {
                    if (!text.empty()) text.push_back({L"", 0, true});
                    text.push_back({CfgT(L"Put the bar back in the center", L"Voltar a barra para o centro"), kMenuBarCenter});
                }
                if (!text.empty()) OpenMenu((float)x, (float)y, std::move(text));
                return 0;
            }
            if (g_siriTab == 2 && g_openFolderIndex < 0 && !g_clipHistory.empty()) {
                int idx = ItemAt(x, y);
                App* a = idx >= 0 ? ResolveApp(g_layoutItems[idx]) : nullptr;
                g_menu.open = false; g_menu.folder = g_menu.appIdx = g_menu.folderApp = -1;
                std::vector<MenuItem> clip;
                if (a && a->kind == 2) { g_menu.appIdx = idx; clip.push_back({Tr(L"Remove this item"), kMenuClipRemove}); clip.push_back({L"", 0, true}); }
                clip.push_back({Tr(L"Clear clipboard"), kMenuClipClear, false, true});
                OpenMenu((float)x, (float)y, std::move(clip));
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
                    items.push_back({CfgT(L"Icon from GitHub...", L"Ícone do GitHub..."), kMenuGhIcon});
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
                if (index >= 0 && IsFolderItem(g_layoutItems[index])) g_menu.folder = g_layoutItems[index] - kFolderBase;
                else if (App* a = AppOfMain(index)) {
                    g_menu.appIdx = index;
                    if (SelectableApp(index)) {
                        bool sel = g_selection.count(a) != 0;
                        items.push_back({sel ? Tr(L"Deselect") : Tr(L"Select"), kMenuSelect});
                        // Floating mode shows the apps of every folder in one list, so no folders are made there
                        if (!g_settings.siri) items.push_back({sel && n > 1 ? withN : std::wstring(Tr(L"New folder with this app")), kMenuNewFolder});
                        items.push_back({L"", 0, true});
                        items.push_back({sel && n > 1 ? Tr(L"Pin selected to taskbar") : Tr(L"Pin to taskbar"), kMenuPin});
                    } else {
                        items.push_back({Tr(L"Pin to taskbar"), kMenuPin});
                    }
                    items.push_back({L"", 0, true});
                    items.push_back({Tr(L"Change icon..."), kMenuChangeIcon});
                    items.push_back({CfgT(L"Icon from GitHub...", L"Ícone do GitHub..."), kMenuGhIcon});
                    if (g_customIcons.count(a->target)) items.push_back({Tr(L"Reset icon"), kMenuResetIcon});
                    items.push_back({L"", 0, true});
                    items.push_back({Tr(L"Uninstall"), kMenuUninstall, false, true});
                } else {
                    if (!g_settings.siri) {
                        items.push_back({n ? withN : std::wstring(Tr(L"New folder")), kMenuNewFolder});
                        items.push_back({Tr(L"Select all"), kMenuSelectAll});
                    }
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
                    items.push_back({CfgT(L"Icon from GitHub...", L"Ícone do GitHub..."), kMenuGhIcon});
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
            if (items.empty()) return 0;
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
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(g_dwrite.GetAddressOf()));
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, g_d2d.GetAddressOf());
    HINSTANCE instance = reinterpret_cast<HINSTANCE>(&__ImageBase);   // this mod's DLL

    // A class left from an earlier load would point to code that is gone: it is registered fresh, never reused
    WNDCLASSEXW cls{};
    cls.cbSize = sizeof(cls);
    cls.hInstance = instance;
    cls.lpszClassName = kWindowClass;
    cls.lpfnWndProc = WndProc;
    cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    bool classOk = RegisterClassExW(&cls) != 0;
    if (!classOk) Wh_Log(L"Window class failed (%u)", GetLastError());

    // Layered (drawn with per-pixel alpha), always on top, no taskbar button
    constexpr DWORD kExStyle = WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
    HWND wnd = classOk ? CreateWindowExW(kExStyle, kWindowClass, L"Siri Plus Launchpad", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, nullptr)
                       : nullptr;
    bool ok = g_d2d && g_dwrite && wnd;
    if (ok) { g_wnd = wnd; SetTimer(wnd, kTimerFirstLoad, 3000, nullptr); AddClipboardFormatListener(wnd); CreateGlassWindow(instance); }
    SetEvent(g_readyEvent);
    if (ok) g_keyHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook, instance, 0);   // category shortcuts with the launcher closed
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
    if (g_keyHook) { UnhookWindowsHookEx(g_keyHook); g_keyHook = nullptr; }
    if (g_frameWaitTimer) { CloseHandle(g_frameWaitTimer); g_frameWaitTimer = nullptr; }
    g_frameTimer = false;
    GhStopThreads();   // GitHub downloads run code from this module
    g_wnd = nullptr; g_workerStop = true;
    ++g_fileSearchGen;                                   // every file thread becomes stale
    if (g_worker) { WaitForSingleObject(g_worker, INFINITE); CloseHandle(g_worker); g_worker = nullptr; }
    for (HANDLE h : g_fileThreadHandles) { WaitForSingleObject(h, INFINITE); CloseHandle(h); }   // they run code from this module
    std::vector<HANDLE>().swap(g_fileThreadHandles);
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
    std::vector<Folder>().swap(g_folders); g_customIcons.clear(); g_mainItems.clear(); g_backdropImage = {}; FreeSurface(g_surface);
    g_labelFormat.Reset(); g_searchFormat.Reset(); g_siriFormat.Reset(); g_messageFormat.Reset();
    g_titleFormat.Reset(); g_menuFormat.Reset(); g_tabFormat.Reset(); g_listFormat.Reset();
    g_listSubFormat.Reset(); g_glyphFormat.Reset(); g_hintFormat.Reset();
    g_glyphLargeFormat.Reset(); g_categoryFormat.Reset(); g_keyFormat.Reset();
    ResetTarget(); g_dwrite.Reset(); g_d2d.Reset();
    DestroyGlassWindow();
    if (g_glassClassOk) { UnregisterClassW(kGlassClass, instance); g_glassClassOk = false; }
    if (classOk) UnregisterClassW(kWindowClass, instance);
    OleUninitialize();
    return 0;
}

// explorer.exe also runs as separate processes for folder windows: the launcher only lives in the shell
bool IsFolderWindowsProcess() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    bool folders = false;
    for (int i = 1; argv && i < argc && !folders; i++) {
        std::wstring arg = ToLower(argv[i]);
        for (PCWSTR start : {L"/factory", L"-embedding", L"/separate"})
            if (arg.compare(0, wcslen(start), start) == 0) folders = true;
    }
    if (argv) LocalFree(argv);
    return folders;
}

bool HookShowStartView() {
    HMODULE twinui = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!twinui) { Wh_Log(L"twinui.pcshell.dll could not be loaded"); return false; }
    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
        &XamlLauncher_ShowStartView_Original,
        XamlLauncher_ShowStartView_Hook,
    }};
    return WindhawkUtils::HookSymbols(twinui, hooks, ARRAYSIZE(hooks));
}

} // namespace

BOOL Wh_ModInit() {
    if (IsFolderWindowsProcess()) return FALSE;   // only the shell's explorer.exe
    LoadSettings();
    if (!HookShowStartView()) return FALSE;
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) return FALSE;
    g_thread = CreateThread(nullptr, 0, StartThread, nullptr, 0, nullptr);
    if (g_thread) return TRUE;
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    return FALSE;
}

void Wh_ModUninit() {
    if (!g_thread) return;
    WaitForSingleObject(g_readyEvent, INFINITE);   // the window exists (or failed) before it is told to quit
    if (HWND launcher = g_wnd) PostMessageW(launcher, WM_APP_QUIT, 0, 0);
    WaitForSingleObject(g_thread, INFINITE);
    for (HANDLE* handle : {&g_thread, &g_readyEvent}) { CloseHandle(*handle); *handle = nullptr; }
}

void Wh_ModSettingsChanged() { if (HWND wnd = g_wnd) PostMessageW(wnd, WM_APP_RELOAD, 0, 0); }

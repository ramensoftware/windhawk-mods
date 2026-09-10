// ==WindhawkMod==
// @id              recycle-bin-tray
// @name            Recycle Bin Tray Icon
// @description     Adds an interactive Recycle Bin icon to the Windows 11 system tray with live state updates, drag-and-drop support, theme-aware rendering, multiple icon styles, and configurable mouse actions.
// @description:fr-FR Ajoute une icône interactive de la Corbeille à la zone de notification de Windows 11, avec mise à jour en temps réel, glisser-déposer, adaptation au thème, plusieurs styles d'icône et actions de souris configurables.
// @version         1.0.0
// @author          Wildstyle23
// @github          https://github.com/wildstyle23
// @license         GPL-3.0
// @include         windhawk.exe
// @compilerOptions -lshell32 -ladvapi32 -luser32 -lole32 -lgdi32 -lgdiplus -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Recycle Bin Tray Icon

![Drag and drop support](https://raw.githubusercontent.com/wildstyle23/Recycle-Bin-Tray-Icon/refs/heads/main/Images/Recycle%20Bin%20Tray%20Icon.png)

A lightweight [Windhawk](https://windhawk.net/) Windows 11 mod that places an interactive Recycle Bin directly into your system tray.

The icon follows the current Recycle Bin state, can optionally disappear when the bin is empty, reacts to Windows Shell notifications, and supports multiple rendering styles, themes, and configurable mouse actions.

The mod is designed to remain lightweight and self-contained while integrating with the native Windows Shell wherever possible.

## Features

* **Live Recycle Bin state** — updates when items are added to or removed from the Recycle Bin.
* **Optional auto-hide** — automatically hides the tray icon while the Recycle Bin is empty.
* **Automatic recovery** — a configurable fallback timer periodically checks the Recycle Bin state in case a Shell notification is missed.
* **Taskbar restart recovery** — the tray icon is recreated after Explorer or the Windows taskbar is restarted.
* **Light / dark theme support** — vector and font rendering adapt to the current Windows system theme; custom icons use Light-theme source files, optional Dark-theme alternatives, and can automatically adapt transparent monochrome image colors to the active theme.
* **DPI-aware rendering** — the render target follows the tray's DPI, while Shell-reported tray geometry is tracked separately for layout changes. Vector icons are rendered at high internal resolution for smoother results.
* **Four icon styles**:
  * `system` — use the native Windows Recycle Bin icon.
  * `vector` — render a lightweight custom vector Windows 11-style icon.
  * `font` — render a glyph from an installed font family.
  * `custom` — load an `.ico`, `.png`, `.bmp`, or `.jpg` file.
* **Three vector variants** — choose between a minimal geometric style with a lifted-lid full state, a Fluent-inspired trapezoidal style, and a compact symbolic style.
* **Separate icons for states and themes** — custom mode uses Empty/Full Light-theme files with optional Empty/Full Dark-theme alternatives.
* **Configurable mouse actions** — independently configure left click, double-click, middle click, and right click.
* **Recycle Bin context menu** — open, empty, or open Properties using Windows Shell-provided localized labels when available.
* **Optional empty confirmation** — keep or disable the confirmation dialog before emptying the Recycle Bin.
* **Drag & Drop to Recycle Bin** — drag files and folders directly from the Desktop, File Explorer, or other applications that expose dropped files through the standard Windows `CF_HDROP` format.

## Installation

### From Windhawk

1. Open **Windhawk**.
2. Go to the **Explore** tab and search for **Recycle Bin Tray Icon**.
3. Click **Details**, then click **Install**.
4. Open the mod's settings to customize the icon and mouse actions.

### Manual Installation from Source

1. Install and open [Windhawk](https://windhawk.net/).
2. Click **Create Mod**.
3. Replace the template code with the contents of `mod.wh.cpp`.
4. Click **Compile Mod** and enable the mod.

The mod runs in a dedicated `windhawk.exe` process rather than being injected into `explorer.exe`, isolating it from the Windows Shell (see [Mods as tools](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process)).

No separate installation, executable, or Windows service is required. Windhawk manages the process automatically.

## Using the Tray Icon

The tray icon provides configurable actions for each supported mouse input.

### Mouse actions

| Input        | Available actions                                 |
| ------------ | ------------------------------------------------- |
| Left click   | Open, context menu, empty, Properties, do nothing |
| Double-click | Open, empty, Properties, do nothing               |
| Middle click | Empty, open, context menu, Properties, do nothing |
| Right click  | Context menu, open, empty, Properties, do nothing |

Each action can be configured independently from the Windhawk settings.

### Click / double-click behavior

Windows sends a single-click notification before it can determine whether a second click will arrive.

When a double-click action is enabled, the mod therefore waits for the Windows double-click interval before executing the single-click action.

This small delay is intentional. Removing it while keeping independent single- and double-click actions could cause a physical double-click to execute the configured single-click action as well.

When **Double click action = Do nothing**, the single-click action is executed immediately because no double-click needs to be distinguished.

## Drag & Drop

The Recycle Bin tray icon can be used as a normal drag & drop destination for files and folders.

Simply drag one or more items from:

* the Windows Desktop;
* File Explorer;
* other applications that expose file lists through the standard Windows `CF_HDROP` format;

and drop them directly onto the Recycle Bin tray icon.

The mod receives the dropped items through the standard Windows OLE drag & drop mechanism and moves them to the Windows Recycle Bin.

### Requirements

For reliable drag & drop operation:

* The Recycle Bin tray icon must be **visible directly in the system tray**.
* The icon should not be located inside the `^` notification-area overflow menu.
* Windows must be able to identify the tray icon as the active drop target.

To ensure the icon is always visible and available as a drop target:

* **Manual placement:** Drag the icon out of the overflow menu and place it directly in the main system tray area. Windows will remember this placement.
* **Windows Settings (if available):** Go to **Settings → Personalization → Taskbar → Other system tray icons** and toggle the icon to **On**.
* **Windhawk alternative:** On modern Windows 11 builds where native toggles may be missing or restricted, you can use the [**Always show all taskbar tray icons**](https://windhawk.net/mods/taskbar-notification-icons-show-all) mod to keep all icons permanently visible.

### Desktop focus behavior

Windows Explorer may occasionally fail to initiate a drag from a Desktop file when another application currently has focus.

For example, clicking a Desktop file and immediately moving the mouse may be interpreted primarily as a focus or selection operation instead of starting the drag.

This happens before the drag operation reaches the mod and is related to normal Explorer Desktop behavior.

### Workarounds

Two simple workarounds are available:

**Option 1 — focus the Desktop first**

Click the file once without dragging, then start the drag operation again.

**Option 2 — hold the mouse button briefly**

Press and hold the left mouse button for approximately 500 ms before moving the file.

Once Explorer has successfully initiated the drag operation, dropping the item onto the Recycle Bin works normally.

The mod does not synthesize or force a Desktop drag when Explorer has not started an OLE drag operation. A low-level mouse hook is used only to arm the tray drop overlay; the actual data transfer still requires the source application to start normal OLE drag & drop.

## Context Menu

The optional Recycle Bin context menu contains:

* **Open**
* **Empty Recycle Bin** — disabled when the bin is already empty
* **Properties**

Menu labels are loaded from localized `shell32.dll` resources when available, so they follow the current Windows display language. The popup itself uses native Win32 menu rendering without custom menu icons.

English fallback strings are used when a corresponding Shell resource cannot be loaded.

## Emptying the Recycle Bin

The **Confirm before emptying** option controls whether the standard Windows confirmation dialog is displayed.

When enabled, the normal Windows confirmation dialog is shown before the Recycle Bin is emptied.

When disabled, the mod requests the silent empty operation directly.

This setting applies to actions that empty the Recycle Bin through the mod.

## Icon Styles

> **Choose one icon style:** `System` · `Vector` · `Font` · `Custom Icon`

### System

Uses the Windows Shell's own Recycle Bin icon.

This is the simplest option and generally provides the closest match to the native Windows appearance.

### Vector

The icon is generated at runtime with GDI+ instead of loading an image file.

It is designed for a clean Windows 11-style appearance and is rendered at a **4× internal resolution** before being reduced to the final tray size for smoother edges.

Three variants are available:

* **Style 1** — minimal geometric 24×24 silhouette. The empty state uses a closed lid; the full state lifts the lid and exposes two compact content shapes while keeping the body unchanged.
* **Style 2** — Fluent-inspired trapezoidal body optimized for tray-size rendering. The empty state uses a clean outline, while the full state uses the same silhouette with a filled body.
* **Style 3** — compact symbolic silhouette with a simple lid, handle, and body. The empty state uses an outlined body, while the full state uses the same silhouette with a filled body.

### Font

The icon is drawn from glyphs provided by installed font families.

The empty and full states have separate **font family / typeface**, **glyph**, and **font weight** settings. Font and glyph fields are intentionally blank by default: if Font mode is selected without a usable configuration, the mod falls back to the native Windows Recycle Bin icon.

Font names are matched against GDI font/typeface names exposed by Windows. Leading/trailing whitespace and surrounding quotation marks are ignored when settings are loaded. GDI limits the requested typeface name to **31 characters** (`LF_FACESIZE - 1`); longer names are rejected instead of being silently truncated.

The **Font weight** option is available independently for the Empty and Full states and supports the standard Windows range from **Thin (100)** to **Black (900)**. Use the base family/typeface name with the desired weight when Windows exposes the family through normal weight matching. If Windows exposes a variant as a separate typeface (for example **Font Awesome 7 Free Solid** or **Arial Narrow**), enter that exact Windows typeface name instead. When an exact weight is unavailable, Windows can select the nearest available weight in the requested family.

#### Fallback behavior

Font mode validates both the requested font family and glyph before rendering:

* **Empty state:** if the empty-state font or glyph is unavailable, the full-bin font/glyph configuration is used automatically.
* **Full state:** if the full-bin font or glyph is unavailable, the native Windows Recycle Bin icon is used.
* If the empty state falls back to the full configuration and that configuration is also unavailable, the native Windows Recycle Bin icon is used.

This lets a font with only one suitable trash/delete glyph remain usable without showing a missing-glyph square.

#### Entering a glyph code

Glyph values can be entered as hexadecimal Unicode code points with or without the `0x` prefix.

Examples:

```text
0xF014
F014
```

To find a code point, you can use:

* **Windows Character Map** (`charmap.exe`) to inspect installed fonts.
* The documentation/reference of the font you are using.
* Microsoft's references for **Segoe Fluent Icons** or **Segoe MDL2 Assets** when using those font families.

### Recommended fonts & glyphs

| Font Family                       | State / role | Glyph Code | Notes |
| --------------------------------- | ------------ | ---------- | ----- |
| **Font Awesome 7 Free**           | Empty        | `0xF014`   | Recommended example for the empty state. |
| **Font Awesome 7 Free Solid**     | Full         | `0xF014`   | Recommended example for the full state. |
| **Segoe Fluent Icons** (Built-in, default Windows 11 icon font) | Single state | `0xE74D` | `Delete`; no distinct empty/full Recycle Bin pair. Best used with **Hide when empty** enabled. |
| **Segoe MDL2 Assets** (Built-in, legacy Windows icon font) | Single state | `0xE74D` | `Delete`; no distinct empty/full Recycle Bin pair. Best used with **Hide when empty** enabled. |
| **Material-Design-Iconic-Font**   | Full         | `0xF154`   | Optional third-party example. |

The two built-in Segoe icon fonts are useful when a single `Delete` symbol is sufficient. Because they don't provide distinct empty and full Recycle Bin states, they work best with **Hide when empty** enabled and the `Delete` glyph configured for the full state: the tray icon is then hidden while the Recycle Bin is empty and shown when it contains items. If you want an always-visible icon with visually distinct empty and full states, another font is preferable.

### Custom Icon

Custom mode accepts:

* `.ico`
* `.png`
* `.bmp`
* `.jpg`

Images are automatically resized to match the current system tray icon target size.

For the cleanest raster result, use artwork designed to remain readable at small tray sizes. Ideally, use a source close to the active tray target size or a clean integer multiple of it (for example 16×16, 32×32, 48×48, or 64×64 when appropriate). Avoid oversized raster artwork with hairline details: a stroke that looks clean in the source can become sub-pixel when Windows renders the tray at 16 px (100% scaling), where no resampling method can fully reconstruct the original visual weight. The actual tray target depends on the Windows display scale factor; refer to the **Theme and DPI Handling** table below.

Custom Icon color handling can either preserve the colors stored in the selected source image or **Automatically adapt monochrome image colors to the active theme**. Theme adaptation reads the selected image's alpha channel, downsamples it by exact pixel-area coverage, and recolors the resulting mask in memory for the active Light or Dark theme. At the smallest 16 px Dark-theme target, a small stroke-preservation and coverage adjustment helps thin monochrome artwork retain more visual weight. For best results, still use simple transparent monochrome artwork designed to remain readable at small sizes; very fine raster details can remain softer at 100% display scaling. No source file is modified. Native `.ico` files and images without an alpha channel keep their original colors.

For transparent monochrome images, the **Light theme** Empty and Full files are normally enough. If a Dark-theme field is blank, the corresponding Light-theme file is reused automatically. The optional **Dark theme** section is only needed when you want different source artwork in Dark theme.

| If you want... | Use |
| --- | --- |
| Keep the icon's original colors | **Keep original image colors** |
| Automatically match a transparent monochrome icon to the Light/Dark theme | **Automatically adapt monochrome image colors to theme** |
| Use different artwork in Dark theme | **Configure the optional Dark theme image** |

Looking for ready-made icons? [Icons8](https://icons8.com/) is a useful resource for finding icons suitable for custom `.ico` / `.png` files.

For `.ico` files, a multi-size icon containing a **16×16 px** layer is recommended for a crisp tray result.

You can paste a Windows path copied with **Copy as path** directly into the setting, including the quotation marks added by Windows. The mod automatically removes surrounding quotes and unnecessary whitespace.

Examples:

```text
[Light theme → Empty bin] "C:\Users\YourUsername\Pictures\recycle_empty.png"
[Light theme → Full bin]  D:\CustomIcons\recycle_full.png
[Dark theme → Full bin]   D:\CustomIcons\recycle_full_dark.ico
```

Local paths are strongly recommended for optimal performance and to avoid delays if a network share is unavailable or goes to sleep.

Custom mode uses two Light-theme source files plus optional Dark-theme alternatives:

| Recycle Bin state | Light-theme file          | Optional Dark-theme file |
| ----------------- | ------------------------- | ------------------------ |
| Empty             | `customIcon.light.empty`  | `customIcon.dark.empty`  |
| Full              | `customIcon.light.full`   | `customIcon.dark.full`   |

The Light-theme file is always selected in Light theme. In Dark theme, the matching Dark-theme file is selected when configured; otherwise the Light-theme file is reused automatically. When **Automatically adapt monochrome image colors to the active theme** is selected, the selected transparent monochrome image is recolored in memory for the current theme.

When a custom file cannot be loaded, the mod automatically falls back to the Windows system Recycle Bin icon.

## Recycle Bin State Detection

The mod uses Windows Shell APIs to query the current Recycle Bin state and register for Shell notifications.

Shell notifications allow the icon to react quickly when items are added to or removed from the Recycle Bin.

Because Shell notifications can occasionally be missed, a configurable fallback timer periodically checks the actual Recycle Bin state.

The default fallback interval is **60 seconds** and can be changed in the Windhawk settings.

Initialization checks the Shell and tray immediately. A one-second startup retry is armed only if Shell notification registration or tray creation is not ready yet.

## Theme and DPI Handling

The tray icon is regenerated when Windows reports a relevant theme or display configuration change.

The mod derives its icon render target from the tray/display DPI.

| Scale Factor | Icon Size |
| ------------ | --------- |
| 100%         | 16×16 px  |
| 125%         | 20×20 px  |
| 150%         | 24×24 px  |
| 200%         | 32×32 px  |
| 250%         | 40×40 px  |
| 300%         | 48×48 px  |
| 400%         | 64×64 px  |

The target size is obtained from the current tray/display DPI. The rectangle returned by `Shell_NotifyIconGetRect` is tracked separately as a geometry-change signal; its slot bounds are not treated as a documented HICON target size.

Vector icons are rendered internally at **4× the target resolution** before being downsampled with high-quality interpolation to fit the active tray size. This reduces jagged edges while keeping rendering and memory usage low.

## Taskbar / Explorer Restart Recovery

The mod listens for the standard `TaskbarCreated` message.

When Windows Explorer or the taskbar is restarted, the mod recreates the tray icon automatically.

This allows the icon to recover without requiring the Windhawk mod to be manually restarted.

## Technical Notes

* The tray icon is hosted by a hidden top-level tool window owned by the mod's dedicated tray thread.
* Drag & drop uses the standard Windows OLE `IDropTarget` mechanism and accepts dropped file lists exposed through `CF_HDROP`.
* The mod does not create a separate executable or Windows service.
* GDI+ is initialized lazily only when vector rendering or custom raster-image loading requires it; native `.ico` loading can bypass GDI+. It is released opportunistically when no rendered icon needs it and again during final teardown.
* Tray timers are explicitly cancelled during window destruction before icon and Shell resources are released.
* Explorer/taskbar restarts are detected through the standard `TaskbarCreated` broadcast received by the hidden tray host.
* The current icon handle is explicitly destroyed whenever the icon is replaced or the mod unloads.
* The tray thread uses Per-Monitor V2 DPI awareness, follows the rectangle reported by `Shell_NotifyIconGetRect`, and tracks slot geometry changes independently from the DPI-derived render size.
* A transparent top-level helper window provides the physical per-monitor DPI reference and is also used as the OLE drop overlay when drag & drop is enabled.
* Explorer owns notification-area placement. If Windows exposes the tray on multiple taskbars, drag & drop follows the single icon rectangle returned by the Shell API.
* A low-level mouse hook is used only while drag & drop support is enabled to detect the beginning and end of a potential drag; OLE `IDropTarget` remains responsible for the actual drop.

## Settings Reference

The Windhawk settings are grouped by function:

* **General** — visibility and icon style.
* **Vector** — vector variant selection.
* **Font** — font family and glyph values for empty/full states.
* **Custom Icon** — color handling, Light-theme Empty/Full source files, and optional Dark-theme files.
* **Actions** — mouse-button behavior and empty confirmation.
* **System** — fallback refresh interval and tray DPI check interval.

Renderer-specific settings only apply to the corresponding selected icon style.

For example, the **Vector** section is used only when **Icon style** is set to **Vector**.

## Troubleshooting

### The icon does not appear

Check that the Recycle Bin is not empty if auto-hide is enabled.

If the icon is still missing:

1. Disable and re-enable the mod in Windhawk.
2. Try restarting Windhawk.
3. Check whether Windows has moved the icon into the notification-area overflow menu (`^`).

If necessary, drag the icon from the overflow menu into the main system tray area. Windows will remember the placement.

### Drag & Drop does not work

1. Check the mod's settings to ensure that Drag & Drop is enabled.
2. Make sure the tray icon is visible directly on the taskbar and is not inside the `^` overflow menu.
3. Make sure the source application provides dropped files through the standard Windows `CF_HDROP` format.

If dragging from the Desktop does not work, check whether another application currently has the input focus.

In that situation, either:

* click the Desktop/file once before starting the drag; or
* hold the left mouse button for approximately 500 ms before moving the file.

### The dark theme does not apply

Check your Windows personalization settings to ensure that global dark mode is active.

For custom icons:

* with **Original colors**, add an optional Dark-theme file if you want different artwork or colors in Dark theme;
* with **Automatically adapt monochrome image colors to the active theme**, use a transparent monochrome image with an alpha channel. Native `.ico` files and images without an alpha channel keep their original colors.

### Custom icons do not display

Verify that:

* the configured path is correct;
* the file exists and is accessible;
* the image format is supported;
* the selected Color handling mode matches the type of image you are using.

For raster images, choose a source size appropriate for the current display scale using the **Theme and DPI Handling** table; thin diagonal strokes can retain some aliasing at very small tray sizes. For `.ico` files, a multi-size icon containing a **16×16 px** image is recommended.

If a custom icon cannot be loaded, the mod automatically falls back to the Windows system Recycle Bin icon.

## License

This project is licensed under the GNU General Public License Version 3.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- general:
  - hideWhenEmpty: false
    $name: "Hide when empty"
    $name:fr-FR: "Masquer si vide"
    $description: "Automatically hide the tray icon when the Recycle Bin contains no items. If the bin is empty when enabled, the icon disappears immediately."
    $description:fr-FR: "Masquer automatiquement l'icône lorsque la corbeille est vide. Si elle est déjà vide au moment de l'activation, l'icône disparaît immédiatement."
  - enableDragDrop: true
    $name: "Enable drag & drop"
    $name:fr-FR: "Autoriser le glisser-déposer"
    $description: "Allow dropping files onto the tray icon to move them to the Recycle Bin. ⚠ Requires \"Hide when empty\" to be turned off."
    $description:fr-FR: "Permet de glisser des fichiers et dossiers directement sur l'icône de la corbeille. ⚠ Nécessite que \"Masquer si vide\" soit désactivé."
  - iconStyle: system
    $name: "Icon style"
    $name:fr-FR: "Style d'icône"
    $description: "Choose how the tray icon is rendered. The Vector, Font, and Custom Icon sections below apply only to their corresponding style."
    $description:fr-FR: "Choisissez le mode de rendu de l'icône. Les sections Vectoriel, Police et Icône personnalisée ci-dessous s'appliquent uniquement au style correspondant."
    $options:
      - system: System default icon
      - vector: Custom vector icon (Windows 11 style)
      - font: Font glyph (installed font)
      - custom: Custom files (.ico, .png, .bmp, .jpg)
    $options:fr-FR:
      - system: Icône système par défaut
      - vector: Icône vectorielle personnalisée (style Windows 11)
      - font: Glyphe d'une police installée
      - custom: Fichiers personnalisés (.ico, .png, .bmp, .jpg)
  $name: "General"
  $name:fr-FR: "Général"
  $description: "Core tray behavior and icon renderer selection."
  $description:fr-FR: "Comportement principal de l'icône et choix du mode de rendu."

- vector:
  - style: style1
    $name: "Style variant"
    $name:fr-FR: "Variante du style"
    $description: "Style 1 uses a minimal geometric 24x24 silhouette with a lifted lid and visible contents for the full state. Style 2 uses a Fluent-inspired trapezoidal silhouette. Style 3 uses a compact symbolic silhouette with a simple lid, handle, and body."
    $description:fr-FR: "Le Style 1 utilise une silhouette géométrique minimale sur une grille 24x24, avec couvercle soulevé et contenu visible lorsque la corbeille est pleine. Le Style 2 utilise une silhouette trapézoïdale inspirée de Fluent. Le Style 3 utilise une silhouette symbolique compacte avec un couvercle, une poignée et un corps simples."
    $options:
      - style1: Style 1 (Minimal / Geometric)
      - style2: Style 2 (Fluent / Trapezoidal)
      - style3: Style 3 (Symbolic / Simple)
    $options:fr-FR:
      - style1: Style 1 (Minimal / Géométrique)
      - style2: Style 2 (Fluent / Trapézoïdal)
      - style3: Style 3 (Symbolique / Simple)
  $name: "Vector"
  $name:fr-FR: "Vectoriel"
  $description: "Used only when Icon style is set to Vector."
  $description:fr-FR: "Utilisé uniquement lorsque le style d'icône est Vectoriel."

- font:
  - empty:
    - name: ""
      $name: "Font family / typeface"
      $name:fr-FR: "Famille / nom de police"
      $description: "Exact Windows GDI font/typeface name, maximum 31 characters. Use Font weight for standard Thin-to-Black variants; if Windows exposes a variant as a separate typeface (e.g. Font Awesome 7 Free Solid), enter that exact name."
      $description:fr-FR: "Nom exact de la police/typeface GDI exposée par Windows, 31 caractères maximum. Utilisez Épaisseur de la police pour les variantes standard de Thin à Black ; si Windows expose une variante comme un type distinct (ex. Font Awesome 7 Free Solid), saisissez ce nom exact."
    - code: ""
      $name: "Glyph (hex)"
      $name:fr-FR: "Glyphe (hex)"
      $description: "Unicode code point in hexadecimal. e.g. 0xF014."
      $description:fr-FR: "Code Unicode hexadécimal. ex. : 0xF014."
    - weight: regular
      $name: "Font weight"
      $name:fr-FR: "Épaisseur de la police"
      $description: "Select the Windows font weight used for this glyph. If the exact weight is unavailable, Windows may select the nearest available weight."
      $description:fr-FR: "Sélectionnez l'épaisseur de la police Windows utilisée pour ce glyphe. Si cette épaisseur exacte n'est pas disponible, Windows peut sélectionner l'épaisseur disponible la plus proche."
      $options:
        - thin: Thin (100)
        - extraLight: Extra Light (200)
        - light: Light (300)
        - regular: Regular (400)
        - medium: Medium (500)
        - semiBold: Semi Bold (600)
        - bold: Bold (700)
        - extraBold: Extra Bold (800)
        - black: Black (900)
      $options:fr-FR:
        - thin: Thin (100)
        - extraLight: Extra Light (200)
        - light: Light (300)
        - regular: Regular (400)
        - medium: Medium (500)
        - semiBold: Semi Bold (600)
        - bold: Bold (700)
        - extraBold: Extra Bold (800)
        - black: Black (900)
    $name: "Empty bin"
    $name:fr-FR: "Corbeille vide"
    $description: "⚠ Fallback: If the Empty font or glyph is invalid or blank, the Full-bin configuration is used automatically."
    $description:fr-FR: "⚠ Repli : si la police ou le glyphe de la corbeille vide est invalide ou non renseigné, la configuration de la corbeille pleine est utilisée automatiquement."
  - full:
    - name: ""
      $name: "Font family / typeface"
      $name:fr-FR: "Famille / nom de police"
      $description: "Exact Windows GDI font/typeface name, maximum 31 characters. Use Font weight for standard Thin-to-Black variants; if Windows exposes a variant as a separate typeface (e.g. Font Awesome 7 Free Solid), enter that exact name."
      $description:fr-FR: "Nom exact de la police/typeface GDI exposée par Windows, 31 caractères maximum. Utilisez Épaisseur de la police pour les variantes standard de Thin à Black ; si Windows expose une variante comme un type distinct (ex. Font Awesome 7 Free Solid), saisissez ce nom exact."
    - code: ""
      $name: "Glyph (hex)"
      $name:fr-FR: "Glyphe (hex)"
      $description: "Unicode code point in hexadecimal. e.g. 0xF014."
      $description:fr-FR: "Code Unicode hexadécimal. ex. : 0xF014."
    - weight: regular
      $name: "Font weight"
      $name:fr-FR: "Épaisseur de la police"
      $description: "Select the Windows font weight used for this glyph. If the exact weight is unavailable, Windows may select the nearest available weight."
      $description:fr-FR: "Sélectionnez l'épaisseur de la police Windows utilisée pour ce glyphe. Si cette épaisseur exacte n'est pas disponible, Windows peut sélectionner l'épaisseur disponible la plus proche."
      $options:
        - thin: Thin (100)
        - extraLight: Extra Light (200)
        - light: Light (300)
        - regular: Regular (400)
        - medium: Medium (500)
        - semiBold: Semi Bold (600)
        - bold: Bold (700)
        - extraBold: Extra Bold (800)
        - black: Black (900)
      $options:fr-FR:
        - thin: Thin (100)
        - extraLight: Extra Light (200)
        - light: Light (300)
        - regular: Regular (400)
        - medium: Medium (500)
        - semiBold: Semi Bold (600)
        - bold: Bold (700)
        - extraBold: Extra Bold (800)
        - black: Black (900)
    $name: "Full bin"
    $name:fr-FR: "Corbeille pleine"
    $description: "⚠ Fallback: If the Full font or glyph is invalid or blank, the native Windows Recycle Bin icon is used."
    $description:fr-FR: "⚠ Repli : si la police ou le glyphe de la corbeille pleine est invalide ou non renseigné, l'icône système Windows de la Corbeille est utilisée."
  $name: "Font"
  $name:fr-FR: "Police"
  $description: "Used only when Icon style is set to Font. Glyphs keep the normal tray size and are reduced only if their rendered ink would otherwise be clipped."
  $description:fr-FR: "Utilisé uniquement lorsque le style d'icône est Police. Les glyphes conservent la taille normale de la zone de notification et ne sont réduits que si leur tracé serait autrement rogné."

- customIcon:
  - colorMode: original
    $name: "Color handling"
    $name:fr-FR: "Gestion des couleurs"
    $description: "Choose whether to preserve the source image colors or automatically adapt transparent monochrome images to the current Light or Dark theme. For theme adaptation, use simple artwork designed for small icon sizes; very thin raster details may look softer at 16 px / 100% scaling. Native .ico files and images without transparency keep their original colors."
    $description:fr-FR: "Choisissez de conserver les couleurs de l'image source ou d'adapter automatiquement les images monochromes transparentes au thème clair ou sombre actif. Pour l'adaptation au thème, privilégiez un dessin simple conçu pour les petites tailles d'icône ; les détails raster très fins peuvent paraître plus doux à 16 px / 100 %. Les fichiers .ico natifs et les images sans transparence conservent leurs couleurs d'origine."
    $options:
      - original: Keep original image colors
      - themeTint: Automatically adapt monochrome image colors to theme
    $options:fr-FR:
      - original: Conserver les couleurs d'origine
      - themeTint: Adapter automatiquement les couleurs des images monochromes au thème
  - light:
    - empty: ""
      $name: "Empty bin"
      $name:fr-FR: "Corbeille vide"
      $description: "Light-theme Empty-bin image (.ico, .png, .bmp, .jpg). Also used as the Dark-theme source when the corresponding Dark field is blank. Quotes from 'Copy as path' are supported."
      $description:fr-FR: "Image de la corbeille vide pour le thème clair (.ico, .png, .bmp, .jpg). Sert également de source pour le thème sombre lorsque le champ sombre correspondant est vide. Les guillemets de 'Copier en tant que chemin d'accès' sont acceptés."
    - full: ""
      $name: "Full bin"
      $name:fr-FR: "Corbeille pleine"
      $description: "Light-theme Full-bin image (.ico, .png, .bmp, .jpg). Also used as the Dark-theme source when the corresponding Dark field is blank. Quotes from 'Copy as path' are supported."
      $description:fr-FR: "Image de la corbeille pleine pour le thème clair (.ico, .png, .bmp, .jpg). Sert également de source pour le thème sombre lorsque le champ sombre correspondant est vide. Les guillemets de 'Copier en tant que chemin d'accès' sont acceptés."
    $name: "Light theme"
    $name:fr-FR: "Thème clair"
    $description: "Images used in Light theme. They are also reused in Dark theme when no corresponding Dark image is provided."
    $description:fr-FR: "Images utilisées en thème clair. Elles sont également réutilisées en thème sombre lorsqu'aucune image sombre correspondante n'est renseignée."
  - dark:
    - empty: ""
      $name: "Empty bin"
      $name:fr-FR: "Corbeille vide"
      $description: "Optional image used only for the Empty state in Dark theme. Leave blank to use Light theme → Empty bin instead."
      $description:fr-FR: "Image facultative utilisée uniquement pour l'état vide en thème sombre. Laisser vide pour utiliser Thème clair → Corbeille vide."
    - full: ""
      $name: "Full bin"
      $name:fr-FR: "Corbeille pleine"
      $description: "Optional image used only for the Full state in Dark theme. Leave blank to use Light theme → Full bin instead."
      $description:fr-FR: "Image facultative utilisée uniquement pour l'état plein en thème sombre. Laisser vide pour utiliser Thème clair → Corbeille pleine."
    $name: "Dark theme (optional)"
    $name:fr-FR: "Thème sombre (facultatif)"
    $description: "Optional images used only in Dark theme. Leave these fields blank to reuse the corresponding Light-theme images."
    $description:fr-FR: "Images facultatives utilisées uniquement en thème sombre. Laisser ces champs vides pour réutiliser les images correspondantes du thème clair."
  $name: "Custom Icon"
  $name:fr-FR: "Icône personnalisée"
  $description: "Used only when Icon style is set to Custom files."
  $description:fr-FR: "Utilisé uniquement lorsque le style d'icône est Fichiers personnalisés."

- actions:
  - leftClick: none
    $name: "Left click"
    $name:fr-FR: "Clic gauche"
    $options:
      - open: Open Recycle Bin
      - contextMenu: Open context menu
      - empty: Empty Recycle Bin
      - properties: Open Properties
      - none: Do nothing
    $options:fr-FR:
      - open: Ouvrir la corbeille
      - contextMenu: Ouvrir le menu contextuel
      - empty: Vider la corbeille
      - properties: Propriétés
      - none: Ne rien faire
  - doubleClick: open
    $name: "Double click"
    $name:fr-FR: "Double-clic"
    $options:
      - open: Open Recycle Bin
      - empty: Empty Recycle Bin
      - properties: Open Properties
      - none: Do nothing
    $options:fr-FR:
      - open: Ouvrir la corbeille
      - empty: Vider la corbeille
      - properties: Propriétés
      - none: Ne rien faire
  - middleClick: empty
    $name: "Middle click"
    $name:fr-FR: "Clic milieu"
    $options:
      - empty: Empty Recycle Bin
      - open: Open Recycle Bin
      - contextMenu: Open context menu
      - properties: Open Properties
      - none: Do nothing
    $options:fr-FR:
      - empty: Vider la corbeille
      - open: Ouvrir la corbeille
      - contextMenu: Ouvrir le menu contextuel
      - properties: Propriétés
      - none: Ne rien faire
  - rightClick: contextMenu
    $name: "Right click"
    $name:fr-FR: "Clic droit"
    $options:
      - contextMenu: Open context menu
      - open: Open Recycle Bin
      - empty: Empty Recycle Bin
      - properties: Open Properties
      - none: Do nothing
    $options:fr-FR:
      - contextMenu: Ouvrir le menu contextuel
      - open: Ouvrir la corbeille
      - empty: Vider la corbeille
      - properties: Propriétés
      - none: Ne rien faire
  - confirmEmpty: true
    $name: "Confirm before emptying"
    $name:fr-FR: "Confirmer avant de vider la corbeille"
    $description: "Ask for confirmation whenever an action triggers emptying the Recycle Bin."
    $description:fr-FR: "Demander confirmation lorsqu'une action déclenche le vidage de la corbeille."
  $name: "Actions"
  $name:fr-FR: "Actions"
  $description: "Mouse-button behavior and emptying confirmation."
  $description:fr-FR: "Comportement des boutons de la souris et confirmation du vidage."

- system:
  - fallbackTimerInterval: 60
    $name: "Fallback timer interval (seconds)"
    $name:fr-FR: "Intervalle de vérification de secours (secondes)"
    $description: "Safety polling interval in seconds to refresh state if Shell events are missed. Set to 0 to disable."
    $description:fr-FR: "Intervalle utilisé pour vérifier l'état de la Corbeille si une notification Windows est manquée. Régler sur 0 pour désactiver."
  - dpiCheckInterval: 30
    $name: "Tray position / DPI check interval (seconds)"
    $name:fr-FR: "Intervalle de vérification de la position / du DPI de l'icône (secondes)"
    $description: "How often the mod checks the real tray icon position and DPI. Set to 0 to disable."
    $description:fr-FR: "Fréquence de vérification de la position réelle et du DPI de l'icône. Régler sur 0 pour désactiver."
  $name: "System"
  $name:fr-FR: "Système"
  $description: "Fallback polling and tray DPI monitoring."
  $description:fr-FR: "Vérification de secours et suivi du DPI de l'icône."
*/
// ==/WindhawkModSettings==
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#define INITGUID
#include <algorithm>
#include <atomic>
#include <cmath>
#include <memory>
#include <new>
#include <string>
#include <utility>
#include <string_view>
#include <type_traits>
#include <vector>
#include <initguid.h>
#include <windows.h>
#include <cguid.h>
#include <ole2.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <knownfolders.h>
#include <gdiplus.h>
#include <windhawk_api.h>
#include <strsafe.h>

// Constants and input state
constexpr UINT WM_USER_START_DRAG_POLL = WM_USER + 101;
constexpr UINT WM_USER_STOP_DRAG_POLL = WM_USER + 102;
constexpr UINT WM_USER_END_OLE_DRAG = WM_USER + 103;
constexpr UINT WM_USER_TRAY_DPI_CHANGED = WM_USER + 104;
constexpr UINT WM_USER_SHUTDOWN = WM_USER + 105;
constexpr UINT WM_USER_REFRESH_AFTER_TRAY_ADD = WM_USER + 106;
constexpr UINT WM_USER_RECYCLE_WORK_COMPLETE = WM_USER + 107;
constexpr int VECTOR_SUPERSAMPLE = 4;
constexpr wchar_t TRAY_WINDOW_CLASS[] = L"WindhawkRecycleTrayClass";
constexpr wchar_t DROP_OVERLAY_CLASS[] = L"WindhawkBinDropOverlay";

HHOOK g_hMouseHook = NULL;
POINT g_dragStartPt = { 0, 0 };
HMODULE g_hThisModule = NULL;

// Resolve the module that actually contains this mod's code, not the host windhawk.exe.
static bool InitializeThisModuleHandle() {
    if (g_hThisModule) return true;

    return GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&g_hThisModule),
        &g_hThisModule) != FALSE;
}

// Small RAII wrappers for GDI, icon and window-resource handles.
struct GdiObjectDeleter {
    void operator()(HGDIOBJ h) const {
        if (h) DeleteObject(h);
    }
};
using UniqueBitmap = std::unique_ptr<std::remove_pointer_t<HBITMAP>, GdiObjectDeleter>;
using UniqueFont = std::unique_ptr<std::remove_pointer_t<HFONT>, GdiObjectDeleter>;

struct IconDeleter {
    void operator()(HICON h) const {
        if (h) DestroyIcon(h);
    }
};
using UniqueIcon = std::unique_ptr<std::remove_pointer_t<HICON>, IconDeleter>;

struct MemDCDeleter {
    void operator()(HDC h) const {
        if (h) DeleteDC(h);
    }
};
using UniqueMemDC = std::unique_ptr<std::remove_pointer_t<HDC>, MemDCDeleter>;

struct OleInitGuard {
    HRESULT hr;

    OleInitGuard() : hr(OleInitialize(nullptr)) {}
    ~OleInitGuard() {
        if (SUCCEEDED(hr)) {
            OleUninitialize();
        }
    }

    OleInitGuard(const OleInitGuard&) = delete;
    OleInitGuard& operator=(const OleInitGuard&) = delete;

    explicit operator bool() const { return SUCCEEDED(hr); }
    HRESULT GetResult() const { return hr; }
};

// Applies Per-Monitor V2 awareness to windows created by the tray thread.
class ThreadDpiAwarenessGuard {
public:
    ThreadDpiAwarenessGuard() {
        if (IsValidDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
            m_previous = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }

    ~ThreadDpiAwarenessGuard() {
        if (m_previous) {
            SetThreadDpiAwarenessContext(m_previous);
        }
    }

    ThreadDpiAwarenessGuard(const ThreadDpiAwarenessGuard&) = delete;
    ThreadDpiAwarenessGuard& operator=(const ThreadDpiAwarenessGuard&) = delete;

    explicit operator bool() const { return m_previous != NULL; }

private:
    DPI_AWARENESS_CONTEXT m_previous = NULL;
};

// Owns a screen DC and releases it with the matching HWND.
class ScreenDC {
   public:
    ScreenDC() : m_hdc(GetDC(NULL)) {}
    ~ScreenDC() {
        if (m_hdc) ReleaseDC(NULL, m_hdc);
    }
    ScreenDC(const ScreenDC&) = delete;
    ScreenDC& operator=(const ScreenDC&) = delete;

    explicit operator bool() const { return m_hdc != NULL; }
    HDC get() const { return m_hdc; }

   private:
    HDC m_hdc;
};

// Restores the previously selected GDI object when leaving scope.
class SelectObjectScope {
   public:
    SelectObjectScope(HDC hdc, HGDIOBJ hObj)
        : m_hdc(hdc), m_hOld(SelectObject(hdc, hObj)) {}
    ~SelectObjectScope() {
        if (m_hdc && m_hOld) SelectObject(m_hdc, m_hOld);
    }
    SelectObjectScope(const SelectObjectScope&) = delete;
    SelectObjectScope& operator=(const SelectObjectScope&) = delete;

   private:
    HDC m_hdc;
    HGDIOBJ m_hOld;
};

constexpr UINT TRAY_ICON_ID = 1001;
constexpr UINT WM_TRAYICON = WM_USER + 1;
constexpr UINT WM_SHELLNOTIFY = WM_USER + 2;
constexpr UINT WM_APPLY_SETTINGS = WM_USER + 3;

constexpr UINT TIMER_CLICK_ID = 101;
constexpr UINT TIMER_REFRESH_ID = 102;
constexpr UINT TIMER_STARTUP_ID = 103;
constexpr UINT TIMER_DRAG_POLL_ID = 104;
constexpr UINT TIMER_DPI_CHECK_ID = 105;
constexpr UINT TIMER_DISPLAY_SETTLE_ID = 106;
constexpr UINT TIMER_SHELL_COALESCE_ID = 107;
constexpr UINT TIMER_DPI_REINSTALL_ID = 108;

// Active drag polling runs only while a physical left-button gesture is in progress.
constexpr UINT DRAG_POLL_INTERVAL_ACTIVE_MS = 20;

// Display changes are debounced, then sampled until the tray rectangle is stable.
constexpr UINT DISPLAY_SETTLE_INTERVAL_MS = 200;
constexpr UINT DISPLAY_SETTLE_MAX_ATTEMPTS = 10;

// Coalesce bursts of per-item Shell notifications into one Recycle Bin query.
constexpr UINT SHELL_COALESCE_INTERVAL_MS = 300;

// Give the Shell a short one-shot settle window after the tray owner changes DPI.
constexpr UINT DPI_REINSTALL_DELAY_MS = 250;

// Tool-mod shutdown must not wait forever on nested Shell UI.
constexpr DWORD TOOL_THREAD_SHUTDOWN_TIMEOUT_MS = 5000;

constexpr UINT IDM_OPEN = 201;
constexpr UINT IDM_EMPTY = 202;
constexpr UINT IDM_PROPERTIES = 203;

// Enumeration of the possible click actions
enum class TrayAction {
    None,
    Open,
    ContextMenu,
    Empty,
    Properties
};

inline TrayAction ParseTrayAction(std::wstring_view str) {
    if (str == L"open")        return TrayAction::Open;
    if (str == L"contextMenu") return TrayAction::ContextMenu;
    if (str == L"empty")       return TrayAction::Empty;
    if (str == L"properties")  return TrayAction::Properties;
    return TrayAction::None;
}

// Supported icon renderers
enum class IconStyle {
    System,
    Vector,
    Font,
    Custom
};

// Cache key for rendered icon content
struct IconCacheKey {
    IconStyle style;
    bool empty = false;
    bool dark = false;
    int iconSize = 0; // Final icon size in physical pixels.
    std::wstring vectorStyle;
    std::wstring fontName;
    std::wstring glyph;
    std::wstring customPath;
    int fontWeight = FW_NORMAL;
    bool customThemeTint = false;

    bool operator==(const IconCacheKey& other) const {
        return style == other.style &&
               empty == other.empty &&
               dark == other.dark &&
               iconSize == other.iconSize &&
               vectorStyle == other.vectorStyle &&
               fontName == other.fontName &&
               glyph == other.glyph &&
               customPath == other.customPath &&
               fontWeight == other.fontWeight &&
               customThemeTint == other.customThemeTint;
    }

};

// Runtime tray state
struct TrayState {
    RECT cachedIconRect = { 0 };
    bool isIconRectValid = false;
    bool dragPollTimerActive = false;
    bool dragRectRefreshed = false;
    bool displaySettleTimerActive = false;
    bool shellCoalesceTimerActive = false;
    RECT displaySettleLastRect = { 0 };
    bool displaySettleHasRect = false;
    bool displaySettleProbedStableRect = false;
    UINT displaySettleAttempts = 0;
    IconCacheKey lastKey;
};

inline TrayState g_trayState;

// True while OLE owns an active drag session over our IDropTarget.
// The mouse hook must never hide the overlay while OLE is about to call Drop().
inline std::atomic_bool g_oleDragActive{false};
inline std::atomic<ULONGLONG> g_oleDragGeneration{0};
inline bool g_dragGestureSawOle = false;
// Keep an armed OLE target visible until the physical gesture ends; recreating it breaks reliable re-entry.
inline std::atomic_bool g_dropOverlayArmed{false};

// Suppresses the nested DPI notification while a display-settle probe handles it synchronously.
inline bool g_displayDpiProbeActive = false;

struct TrayGeometryCache {
    int renderSize = 0;
    int slotWidth = 0;
    int slotHeight = 0;
    bool renderSizeValid = false;
    bool hasShellGeometry = false;
};

static TrayGeometryCache g_trayGeometryCache;

struct SystemThemeCache {
    bool dark = false;
    bool valid = false;
};

static SystemThemeCache g_systemThemeCache;

// Cache invalidation

// Invalidates only the cached tray icon rectangle.
void InvalidateIconRectCache() {
    g_trayState.isIconRectValid = false;
}

// Re-sample the render size while preserving the last Shell slot dimensions as
// a baseline for detecting real layout changes.
void InvalidateTrayRenderSize() {
    g_trayGeometryCache.renderSizeValid = false;
}

// Invalidates transient tray position and render size while preserving the last Shell slot baseline.
void InvalidateTrayPositionAndRenderSize() {
    InvalidateIconRectCache();
    InvalidateTrayRenderSize();
}

// User settings
struct ModSettings {
    bool hideWhenEmpty;
    bool enableDragDrop;
    IconStyle iconStyle;
    WCHAR vectorStyle[32];
    WCHAR fontNameEmpty[LF_FACESIZE];
    WCHAR fontCodeEmpty[16];
    int fontWeightEmpty;
    WCHAR fontNameFull[LF_FACESIZE];
    WCHAR fontCodeFull[16];
    int fontWeightFull;
    bool customIconThemeTint;
    std::wstring customIconEmpty;
    std::wstring customIconFull;
    std::wstring customIconEmptyDark;
    std::wstring customIconFullDark;
    TrayAction leftClickAction;
    TrayAction doubleClickAction;
    TrayAction middleClickAction;
    TrayAction rightClickAction;
    bool confirmEmpty;
    UINT fallbackTimerInterval;
    UINT dpiCheckInterval;
} g_settings;

// Settings callbacks can run outside the tray thread. Keep only the newest
// snapshot here; the tray thread remains the sole writer of g_settings.
ModSettings* g_pendingSettings = nullptr;

static ModSettings* TakePendingSettings() {
    return static_cast<ModSettings*>(InterlockedExchangePointer(
        reinterpret_cast<PVOID volatile*>(&g_pendingSettings), nullptr));
}

static void QueuePendingSettings(std::unique_ptr<ModSettings> settings) {
    ModSettings* raw = settings.release();
    ModSettings* replaced = static_cast<ModSettings*>(InterlockedExchangePointer(
        reinterpret_cast<PVOID volatile*>(&g_pendingSettings), raw));
    delete replaced;
}

static bool ConsumePendingSettings() {
    std::unique_ptr<ModSettings> pending(TakePendingSettings());
    if (!pending) return false;

    g_settings = std::move(*pending);
    return true;
}

static bool ReadSystemDarkTheme() {
    HKEY hKey;
    DWORD data = 1;
    DWORD dataSize = sizeof(data);
    LONG result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        0,
        KEY_READ,
        &hKey
    );

    if (result == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"SystemUsesLightTheme", NULL, NULL, (LPBYTE)&data, &dataSize);
        RegCloseKey(hKey);
    }

    return data == 0;
}

static bool GetSystemDarkTheme() {
    if (!g_systemThemeCache.valid) {
        g_systemThemeCache.dark = ReadSystemDarkTheme();
        g_systemThemeCache.valid = true;
    }
    return g_systemThemeCache.dark;
}

static bool RefreshSystemThemeCache() {
    const bool dark = ReadSystemDarkTheme();
    const bool changed =
        !g_systemThemeCache.valid || g_systemThemeCache.dark != dark;
    g_systemThemeCache.dark = dark;
    g_systemThemeCache.valid = true;
    return changed;
}

static void InvalidateSystemThemeCache() {
    g_systemThemeCache.valid = false;
}


// Shared tray resources. The drop-target type is forward-declared for its global pointer.
class RecycleBinDropTarget;

struct RecycleDroppedItem {
    std::wstring path;
    std::wstring parentPath;
    bool isDirectory = false;
};

struct RecycleWorkerJob {
    HWND owner = NULL;
    std::vector<std::wstring> paths;
};

HANDLE g_hThread = NULL;
HWND g_hWnd = NULL;
HWND g_hOverlayWnd = NULL;
RecycleBinDropTarget* g_pDropTarget = NULL;
NOTIFYICONDATAW g_nid = {0};
ULONG g_shellNotifyLock = 0;
bool g_iconVisible = false;
bool g_ignoreNextLeftUp = false;
bool g_trayVersion4 = false;
bool g_loggedInitialState = false;
std::atomic_bool g_shutdownRequested{false};
UINT g_shellModalDepth = 0; // Tray-thread only; protects the hidden Shell UI owner.
bool g_hostDpiProbeActive = false;
bool g_pendingDpiShellReinstall = false;
bool QueryTrayIconRect(HWND hWnd, RECT& rect, bool forceRefresh);
bool RefreshTrayGeometry(HWND hWnd, bool forceRegeneration);
static HWND GetSafeHwnd();
const WCHAR* TimerName(UINT timerId);
bool SetLoggedTimer(HWND hWnd, UINT timerId, UINT intervalMs);
bool KillLoggedTimer(HWND hWnd, UINT timerId);
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
UniqueIcon g_hCurrentIcon;
bool g_forceIconRegen = true;
ULONG_PTR g_gdiplusToken = 0;
bool g_gdiplusInitialized = false;
UINT g_wmTaskbarCreated = 0;

// Recycle work runs outside IDropTarget::Drop so the source's DoDragDrop can
// return immediately even if Shell needs slow I/O or user confirmation.
HANDLE g_hRecycleWorkerThread = NULL;
HANDLE g_hRecycleWorkerEvent = NULL;
RecycleWorkerJob* g_pendingRecycleWorkerJob = nullptr;
std::atomic_bool g_recycleWorkerStop{false};
std::atomic_bool g_recycleWorkerBusy{false};

static RecycleWorkerJob* TakePendingRecycleWorkerJob() {
    return static_cast<RecycleWorkerJob*>(InterlockedExchangePointer(
        reinterpret_cast<PVOID volatile*>(&g_pendingRecycleWorkerJob), nullptr));
}

static bool CanCloseTrayHost() {
    // The hidden host can own Shell UI on either the tray thread or the recycle
    // worker. Keep it alive until both paths have unwound.
    return g_shellModalDepth == 0 && !g_recycleWorkerBusy.load();
}

// Shell APIs can enter nested modal message loops. Keep the hidden owner alive
// until the outermost tray-thread call returns, then finish a pending shutdown.
class ShellModalScope {
public:
    explicit ShellModalScope(HWND owner) : m_owner(owner) {
        ++g_shellModalDepth;
    }

    ~ShellModalScope() {
        if (g_shellModalDepth > 0) {
            --g_shellModalDepth;
        }

        if (CanCloseTrayHost() && g_shutdownRequested.load() &&
            m_owner && IsWindow(m_owner)) {
            (void)PostMessageW(m_owner, WM_CLOSE, 0, 0);
        }
    }

    ShellModalScope(const ShellModalScope&) = delete;
    ShellModalScope& operator=(const ShellModalScope&) = delete;

private:
    HWND m_owner;
};

static std::wstring GetParentPath(const std::wstring& path) {
    if (path.empty()) return {};

    std::wstring parent = path;
    if (!PathRemoveFileSpecW(parent.data())) {
        return {};
    }

    parent.resize(wcslen(parent.c_str()));
    return parent;
}

static void NotifyShellSourceChanged(const std::vector<RecycleDroppedItem>& items) {
    std::vector<std::wstring> notifiedParents;
    notifiedParents.reserve(items.size());

    for (const auto& item : items) {
        SHChangeNotify(
            item.isDirectory ? SHCNE_RMDIR : SHCNE_DELETE,
            SHCNF_PATHW | SHCNF_FLUSH,
            item.path.c_str(),
            NULL);

        if (!item.parentPath.empty() &&
            std::find(notifiedParents.begin(), notifiedParents.end(),
                      item.parentPath) == notifiedParents.end()) {
            SHChangeNotify(
                SHCNE_UPDATEDIR,
                SHCNF_PATHW | SHCNF_FLUSH,
                item.parentPath.c_str(),
                NULL);
            notifiedParents.push_back(item.parentPath);
        }
    }

    Wh_Log(L"D&D: Shell source refresh notifications sent (%u item(s), %u parent(s))",
           static_cast<UINT>(items.size()),
           static_cast<UINT>(notifiedParents.size()));
}

static bool PerformRecycleWorkerJob(RecycleWorkerJob& job) {
    if (job.paths.empty()) {
        return false;
    }

    std::vector<RecycleDroppedItem> items;
    items.reserve(job.paths.size());

    // Resolve attributes on the worker as well; network/removable paths must
    // not turn IDropTarget::Drop into a blocking filesystem operation.
    for (auto& path : job.paths) {
        const DWORD attributes = GetFileAttributesW(path.c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            Wh_Log(L"D&D worker: GetFileAttributes failed for '%s': %lu",
                   path.c_str(), GetLastError());
            return false;
        }

        RecycleDroppedItem item;
        item.parentPath = GetParentPath(path);
        item.isDirectory = (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        item.path = std::move(path);
        items.push_back(std::move(item));
    }

    // Only the enriched item list is needed from here; release the moved-from
    // path-vector storage before a potentially long Shell operation.
    std::vector<std::wstring>().swap(job.paths);

    IFileOperation* pfo = NULL;
    HRESULT hr = CoCreateInstance(
        CLSID_FileOperation, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfo));
    if (FAILED(hr)) {
        Wh_Log(L"D&D worker: CoCreateInstance(CLSID_FileOperation) failed: 0x%08X", hr);
        return false;
    }

    if (job.owner && IsWindow(job.owner)) {
        hr = pfo->SetOwnerWindow(job.owner);
        if (FAILED(hr)) {
            Wh_Log(L"D&D worker: SetOwnerWindow failed: 0x%08X", hr);
        }
    }

    hr = pfo->SetOperationFlags(
        FOFX_RECYCLEONDELETE |
        FOFX_ADDUNDORECORD |
        FOF_NOCONFIRMATION |
        FOF_SILENT |
        FOF_WANTNUKEWARNING);
    if (FAILED(hr)) {
        Wh_Log(L"D&D worker: SetOperationFlags failed: 0x%08X", hr);
        pfo->Release();
        return false;
    }

    bool allQueued = true;
    for (const auto& item : items) {
        IShellItem* psi = NULL;
        hr = SHCreateItemFromParsingName(
            item.path.c_str(), NULL, IID_PPV_ARGS(&psi));
        if (FAILED(hr)) {
            Wh_Log(L"D&D worker: SHCreateItemFromParsingName failed for '%s': 0x%08X",
                   item.path.c_str(), hr);
            allQueued = false;
            break;
        }

        hr = pfo->DeleteItem(psi, NULL);
        psi->Release();
        if (FAILED(hr)) {
            Wh_Log(L"D&D worker: DeleteItem failed for '%s': 0x%08X",
                   item.path.c_str(), hr);
            allQueued = false;
            break;
        }
    }

    bool completed = false;
    if (allQueued) {
        // This may show Shell UI (for example a permanent-delete warning), but
        // it no longer runs inside the source application's DoDragDrop call.
        hr = pfo->PerformOperations();

        if (FAILED(hr)) {
            Wh_Log(L"D&D worker: PerformOperations failed: 0x%08X", hr);
        } else {
            BOOL aborted = FALSE;
            const HRESULT hrAborted = pfo->GetAnyOperationsAborted(&aborted);
            if (FAILED(hrAborted)) {
                Wh_Log(L"D&D worker: GetAnyOperationsAborted failed: 0x%08X", hrAborted);
            } else if (aborted) {
                Wh_Log(L"D&D worker: one or more file operations were aborted");
            } else {
                completed = true;
                Wh_Log(L"D&D worker: %u item(s) sent to Recycle Bin asynchronously",
                       static_cast<UINT>(items.size()));
                NotifyShellSourceChanged(items);
            }
        }
    }

    pfo->Release();
    return completed;
}

static DWORD WINAPI RecycleWorkerProc(LPVOID) {
    ThreadDpiAwarenessGuard dpiAwareness;
    if (!dpiAwareness) {
        Wh_Log(L"D&D worker: failed to enable Per-Monitor V2 awareness: %lu",
               GetLastError());
    }

    OleInitGuard oleInit;
    if (!oleInit) {
        Wh_Log(L"D&D worker: OleInitialize failed: 0x%08X", oleInit.GetResult());
        return 0;
    }

    Wh_Log(L"D&D worker: started");

    for (;;) {
        const DWORD waitResult = WaitForSingleObject(g_hRecycleWorkerEvent, INFINITE);
        if (waitResult != WAIT_OBJECT_0) {
            if (waitResult == WAIT_FAILED) {
                Wh_Log(L"D&D worker: wait failed: %lu", GetLastError());
            } else {
                Wh_Log(L"D&D worker: unexpected wait result: %lu", waitResult);
            }
            break;
        }

        std::unique_ptr<RecycleWorkerJob> job(TakePendingRecycleWorkerJob());
        if (job) {
            const bool completed = PerformRecycleWorkerJob(*job);
            g_recycleWorkerBusy.store(false);

            HWND hWnd = GetSafeHwnd();
            if (hWnd && IsWindow(hWnd)) {
                // Refresh state regardless of success/abort; Shell may have
                // completed only part of a multi-item operation.
                (void)PostMessageW(hWnd, WM_SHELLNOTIFY, 0, 0);
                (void)PostMessageW(
                    hWnd, WM_USER_RECYCLE_WORK_COMPLETE,
                    completed ? TRUE : FALSE, 0);
            }
        }

        if (g_recycleWorkerStop.load()) {
            break;
        }
    }

    delete TakePendingRecycleWorkerJob();
    g_recycleWorkerBusy.store(false);
    Wh_Log(L"D&D worker: stopped");
    return 0;
}

static bool InitializeRecycleWorker() {
    g_recycleWorkerStop.store(false);
    g_recycleWorkerBusy.store(false);
    delete TakePendingRecycleWorkerJob();

    g_hRecycleWorkerEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!g_hRecycleWorkerEvent) {
        Wh_Log(L"D&D worker: CreateEvent failed: %lu", GetLastError());
        return false;
    }

    g_hRecycleWorkerThread = CreateThread(
        NULL, 0, RecycleWorkerProc, NULL, 0, NULL);
    if (!g_hRecycleWorkerThread) {
        Wh_Log(L"D&D worker: CreateThread failed: %lu", GetLastError());
        CloseHandle(g_hRecycleWorkerEvent);
        g_hRecycleWorkerEvent = NULL;
        return false;
    }

    return true;
}

static void RequestRecycleWorkerStop() {
    g_recycleWorkerStop.store(true);
    if (g_hRecycleWorkerEvent) {
        (void)SetEvent(g_hRecycleWorkerEvent);
    }
}

static bool QueueRecycleWorkerJob(HWND owner, std::vector<std::wstring> paths) {
    if (paths.empty() || g_shutdownRequested.load() ||
        !g_hRecycleWorkerThread || !g_hRecycleWorkerEvent) {
        return false;
    }

    const DWORD workerState = WaitForSingleObject(g_hRecycleWorkerThread, 0);
    if (workerState != WAIT_TIMEOUT) {
        if (workerState == WAIT_FAILED) {
            Wh_Log(L"D&D worker: state check failed: %lu", GetLastError());
        } else {
            Wh_Log(L"D&D worker: thread is not running");
        }
        return false;
    }

    if (g_recycleWorkerBusy.exchange(true)) {
        Wh_Log(L"D&D: recycle worker is busy; rejecting concurrent drop");
        return false;
    }

    auto job = std::make_unique<RecycleWorkerJob>();
    job->owner = owner;
    job->paths = std::move(paths);

    // Transfer ownership to the atomic pending slot. If the slot is already
    // occupied, immediately reclaim the object locally before returning.
    RecycleWorkerJob* raw = job.release();
    PVOID previous = InterlockedCompareExchangePointer(
        reinterpret_cast<PVOID volatile*>(&g_pendingRecycleWorkerJob),
        raw,
        nullptr);
    if (previous != nullptr) {
        std::unique_ptr<RecycleWorkerJob> rejectedJob(raw);
        g_recycleWorkerBusy.store(false);
        Wh_Log(L"D&D worker: pending job slot unexpectedly occupied");
        return false;
    }

    if (!SetEvent(g_hRecycleWorkerEvent)) {
        std::unique_ptr<RecycleWorkerJob> failedJob(TakePendingRecycleWorkerJob());
        g_recycleWorkerBusy.store(false);
        Wh_Log(L"D&D worker: SetEvent failed: %lu", GetLastError());
        return false;
    }

    return true;
}

static bool ExtractDroppedPaths(HDROP hDrop, std::vector<std::wstring>& paths) {
    paths.clear();

    const UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
    if (fileCount == 0) {
        Wh_Log(L"D&D: Drop received but CF_HDROP contains no files");
        return false;
    }

    paths.reserve(fileCount);
    for (UINT i = 0; i < fileCount; ++i) {
        const UINT cchNeeded = DragQueryFileW(hDrop, i, NULL, 0);
        if (cchNeeded == 0) {
            Wh_Log(L"D&D: DragQueryFileW size query failed for item %u", i);
            return false;
        }

        std::wstring path(cchNeeded + 1, L'\0');
        const UINT cchCopied = DragQueryFileW(
            hDrop, i, path.data(), static_cast<UINT>(path.size()));
        if (cchCopied == 0) {
            Wh_Log(L"D&D: DragQueryFileW failed for item %u", i);
            return false;
        }

        path.resize(cchCopied);
        paths.push_back(std::move(path));
    }

    return true;
}

// OLE drop target
class RecycleBinDropTarget : public IDropTarget {
private:
    LONG m_cRef;
    HWND m_hMainWnd;
    bool m_hasValidFormat = false;

    static bool ReportPerformedDropEffect(IDataObject* dataObject, DWORD effect) {
        if (!dataObject) return false;

        const CLIPFORMAT format = static_cast<CLIPFORMAT>(
            RegisterClipboardFormatW(CFSTR_PERFORMEDDROPEFFECT));
        if (!format) {
            Wh_Log(L"D&D: RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT) failed: %lu",
                   GetLastError());
            return false;
        }

        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (!hGlobal) {
            Wh_Log(L"D&D: GlobalAlloc for performed drop effect failed: %lu", GetLastError());
            return false;
        }

        DWORD* value = static_cast<DWORD*>(GlobalLock(hGlobal));
        if (!value) {
            Wh_Log(L"D&D: GlobalLock for performed drop effect failed: %lu", GetLastError());
            GlobalFree(hGlobal);
            return false;
        }
        *value = effect;
        GlobalUnlock(hGlobal);

        FORMATETC formatEtc = {
            format, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL
        };
        STGMEDIUM medium = {};
        medium.tymed = TYMED_HGLOBAL;
        medium.hGlobal = hGlobal;

        const HRESULT hr = dataObject->SetData(&formatEtc, &medium, TRUE);
        if (FAILED(hr)) {
            // Ownership remains with us when SetData doesn't accept the medium.
            ReleaseStgMedium(&medium);
            Wh_Log(L"D&D: SetData(CFSTR_PERFORMEDDROPEFFECT) failed: 0x%08X", hr);
            return false;
        }

        return true;
    }


public:
    RecycleBinDropTarget(HWND hMainWnd) : m_cRef(1), m_hMainWnd(hMainWnd) {}

    virtual ~RecycleBinDropTarget() = default;

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;

        if (riid == __uuidof(IUnknown) || riid == __uuidof(IDropTarget)) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return InterlockedIncrement(&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() override {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) delete this;
        return cRef;
    }

    STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override {
        if (!pdwEffect) return E_POINTER;
        if (!pDataObj) {
            *pdwEffect = DROPEFFECT_NONE;
            g_oleDragActive.store(false);
            return E_POINTER;
        }

        FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        const bool hasDropData = (pDataObj->QueryGetData(&fmt) == S_OK);
        m_hasValidFormat = hasDropData && !g_recycleWorkerBusy.load();
        if (hasDropData && !m_hasValidFormat) {
            Wh_Log(L"D&D: DragEnter rejected while recycle worker is busy");
        }

        // Once entered, OLE owns the session; the mouse hook must not hide the target before Drop().
        const ULONGLONG generation = g_oleDragGeneration.fetch_add(1) + 1;
        g_dragGestureSawOle = true;
        g_oleDragActive.store(m_hasValidFormat);
        if (m_hasValidFormat) {
            g_dropOverlayArmed.store(true);
        }

        if (m_hasValidFormat && (*pdwEffect & DROPEFFECT_MOVE)) {
            *pdwEffect = DROPEFFECT_MOVE;
            Wh_Log(L"D&D: DragEnter (CF_HDROP accepted), session=%llu", generation);
        } else {
            *pdwEffect = DROPEFFECT_NONE;
            Wh_Log(L"D&D: DragEnter rejected, session=%llu", generation);
        }
        return S_OK;
    }

    STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override {
        if (!pdwEffect) return E_POINTER;

        if (m_hasValidFormat && (*pdwEffect & DROPEFFECT_MOVE)) {
            *pdwEffect = DROPEFFECT_MOVE;
        } else {
            *pdwEffect = DROPEFFECT_NONE;
        }
        return S_OK;
    }

    STDMETHODIMP DragLeave() override {
        m_hasValidFormat = false;
        const ULONGLONG generation = g_oleDragGeneration.load();
        g_oleDragActive.store(false);

        // DragLeave is temporary; keep the target armed so OLE can DragEnter again on re-entry.
        Wh_Log(L"D&D: DragLeave, session=%llu (target kept armed for re-entry)", generation);

        (void)SetCursor(LoadCursor(NULL, IDC_ARROW));
        return S_OK;
    }

    STDMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override {
        if (!pdwEffect) return E_POINTER;
        if (!pDataObj) {
            *pdwEffect = DROPEFFECT_NONE;
            m_hasValidFormat = false;
            const ULONGLONG generation = g_oleDragGeneration.load();
            g_oleDragActive.store(false);
            if (g_hWnd) {
                (void)PostMessageW(g_hWnd, WM_USER_END_OLE_DRAG, static_cast<WPARAM>(generation), 0);
            }
            return E_POINTER;
        }

        // Revalidate the format at the exact Drop point rather than relying on
        // the state captured by DragEnter/DragOver.
        FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM medium = { 0 };

        HRESULT hr = pDataObj->GetData(&fmt, &medium);
        if (SUCCEEDED(hr)) {
            HDROP hDrop = static_cast<HDROP>(GlobalLock(medium.hGlobal));
            if (hDrop) {
                std::vector<std::wstring> paths;
                const bool extracted = ExtractDroppedPaths(hDrop, paths);
                (void)GlobalUnlock(medium.hGlobal);

                const bool queued = extracted &&
                    QueueRecycleWorkerJob(m_hMainWnd, std::move(paths));
                if (queued) {
                    // Ownership of the operation is now ours. Report NONE so the
                    // source returns from DoDragDrop without deleting the items itself.
                    const bool effectReported =
                        ReportPerformedDropEffect(pDataObj, DROPEFFECT_NONE);
                    *pdwEffect = DROPEFFECT_NONE;
                    Wh_Log(
                        L"D&D: Drop queued for asynchronous recycle (CF_HDROP); "
                        L"performed effect%s",
                        effectReported ? L" reported" : L" fallback via pdwEffect");
                } else {
                    *pdwEffect = DROPEFFECT_NONE;
                    Wh_Log(L"D&D: Drop rejected (CF_HDROP)");
                }
            } else {
                *pdwEffect = DROPEFFECT_NONE;
                Wh_Log(L"D&D: GlobalLock failed for CF_HDROP");
            }
            ReleaseStgMedium(&medium);
        } else {
            *pdwEffect = DROPEFFECT_NONE;
            Wh_Log(L"D&D: IDataObject::GetData(CF_HDROP) failed: 0x%08X", hr);
        }

        m_hasValidFormat = false;
        const ULONGLONG generation = g_oleDragGeneration.load();
        g_oleDragActive.store(false);

        // Post cleanup after OLE returns; the generation rejects stale completion messages.
        if (g_hWnd) {
            (void)PostMessageW(g_hWnd, WM_USER_END_OLE_DRAG, static_cast<WPARAM>(generation), 0);
        }

        (void)SetCursor(LoadCursor(NULL, IDC_ARROW));
        return S_OK;
    }
};

// Loads GDI+ only for vector or raster-image rendering.
static bool EnsureGdiplusInitialized() {
    if (g_gdiplusInitialized) return true;

    Gdiplus::GdiplusStartupInput startupInput;
    const Gdiplus::Status status = GdiplusStartup(&g_gdiplusToken, &startupInput, NULL);
    g_gdiplusInitialized = (status == Gdiplus::Ok);
    if (!g_gdiplusInitialized) {
        Wh_Log(L"GDI+: startup failed with status %d", status);
    }
    return g_gdiplusInitialized;
}

static void ShutdownGdiplusIfInitialized() {
    if (!g_gdiplusInitialized) return;

    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    g_gdiplusToken = 0;
    g_gdiplusInitialized = false;
}

// Window procedure for the transparent DPI and drag-and-drop overlay.
LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_NCHITTEST:
        // Keep hit-testing deterministic; CheckDragStatus owns overlay visibility.
        return HTCLIENT;

    case WM_DPICHANGED: {
        // Apply Windows' suggested bounds so the hidden reference window
        // completes its per-monitor DPI transition before the tray is refreshed.
        const RECT* suggestedRect = reinterpret_cast<const RECT*>(lParam);
        const UINT dpiX = LOWORD(wParam);
        const UINT dpiY = HIWORD(wParam);

        if (suggestedRect) {
            (void)SetWindowPos(
                hWnd, NULL,
                suggestedRect->left, suggestedRect->top,
                suggestedRect->right - suggestedRect->left,
                suggestedRect->bottom - suggestedRect->top,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }

        Wh_Log(L"DPI: overlay WM_DPICHANGED dpi=%u x %u", dpiX, dpiY);

        if (!g_displayDpiProbeActive) {
            HWND hMainWnd = GetSafeHwnd();
            if (hMainWnd) {
                (void)PostMessageW(hMainWnd, WM_USER_TRAY_DPI_CHANGED, 0, 0);
            }
        }
        return 0;
    }

    case WM_LBUTTONUP:
        if (g_hWnd) PostMessageW(g_hWnd, WM_TRAYICON, 0, WM_LBUTTONUP);
        return 0;

    case WM_LBUTTONDBLCLK:
        if (g_hWnd) PostMessageW(g_hWnd, WM_TRAYICON, 0, WM_LBUTTONDBLCLK);
        return 0;

    case WM_MBUTTONUP:
        if (g_hWnd) PostMessageW(g_hWnd, WM_TRAYICON, 0, WM_MBUTTONUP);
        return 0;

    case WM_RBUTTONUP:
        if (g_hWnd) PostMessageW(g_hWnd, WM_TRAYICON, 0, WM_RBUTTONUP);
        return 0;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}

static void HideDropOverlay() {
    if (g_hOverlayWnd && IsWindowVisible(g_hOverlayWnd)) {
        (void)ShowWindow(g_hOverlayWnd, SW_HIDE);
    }
}

static bool PositionDropOverlay(const RECT& rect, bool show) {
    if (!g_hOverlayWnd) return false;

    UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER;
    HWND insertAfter = HWND_TOPMOST;
    if (show) {
        flags |= SWP_SHOWWINDOW;
    } else {
        flags |= SWP_NOZORDER;
        insertAfter = NULL;
    }

    return SetWindowPos(
        g_hOverlayWnd, insertAfter, rect.left, rect.top,
        rect.right - rect.left, rect.bottom - rect.top, flags) != FALSE;
}

// A hidden Per-Monitor V2 window may keep its previous DPI after a display change.
// Briefly showing the nearly transparent overlay makes Windows complete the transition.
static bool ProbeDpiReferenceAtTray(const RECT& rect) {
    if (!g_hOverlayWnd) return false;

    // Never disturb an active physical/OLE drag; the normal D&D path will move
    // the overlay itself and therefore complete the DPI transition.
    if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
        g_oleDragActive.load() || g_dropOverlayArmed.load()) {
        return false;
    }

    const bool wasVisible = IsWindowVisible(g_hOverlayWnd) != FALSE;

    g_displayDpiProbeActive = true;
    const bool positioned = PositionDropOverlay(rect, true);
    g_displayDpiProbeActive = false;

    if (!wasVisible) {
        HideDropOverlay();
    }

    return positioned;
}

// Keep the notification-icon owner on the same physical DPI as the tray.
static bool ProbeTrayHostDpiAtRect(HWND hWnd, const RECT& rect) {
    if (!hWnd) return false;

    const bool wasVisible = IsWindowVisible(hWnd) != FALSE;

    g_hostDpiProbeActive = true;
    const BOOL positioned = SetWindowPos(
        hWnd, NULL, rect.left, rect.top, 1, 1,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
    g_hostDpiProbeActive = false;

    if (!wasVisible) {
        (void)ShowWindow(hWnd, SW_HIDE);
    }

    return positioned != FALSE;
}

// Overlay visibility is derived from the current tray and D&D state.
void UpdateOverlayState() {
    if (!g_settings.enableDragDrop || !g_iconVisible || !g_hOverlayWnd) {
        g_dropOverlayArmed.store(false);
        HideDropOverlay();
        return;
    }

    // Never reveal the overlay from routine tray refreshes; only real drag polling may show it.
    if (!IsWindowVisible(g_hOverlayWnd)) {
        return;
    }

    RECT iconRect = {};
    if (!QueryTrayIconRect(g_hWnd, iconRect, false)) {
        HideDropOverlay();
        return;
    }

    (void)PositionDropOverlay(iconRect, true);
}

static HWND GetSafeHwnd() {
    return (HWND)InterlockedCompareExchangePointer((PVOID volatile*)&g_hWnd, NULL, NULL);
}

static std::wstring TrimAndUnquote(std::wstring_view value) {
    auto trim = [](std::wstring_view input) {
        const size_t first = input.find_first_not_of(L" \t\r\n");
        if (first == std::wstring_view::npos) return std::wstring_view{};
        const size_t last = input.find_last_not_of(L" \t\r\n");
        return input.substr(first, last - first + 1);
    };

    value = trim(value);
    if (value.size() >= 2 && value.front() == L'"' && value.back() == L'"') {
        value = trim(value.substr(1, value.size() - 2));
    }

    return std::wstring(value);
}

static void ReadStringSetting(PCWSTR key, WCHAR* target, size_t maxCount, PCWSTR defaultValue) {
    PCWSTR val = Wh_GetStringSetting(key);
    StringCchCopyW(target, maxCount, *val ? val : defaultValue);
    Wh_FreeStringSetting(val);
}

static void ReadTrimmedStringSetting(PCWSTR key, WCHAR* target, size_t maxCount) {
    PCWSTR value = Wh_GetStringSetting(key);
    const std::wstring normalized = TrimAndUnquote(value);
    StringCchCopyW(target, maxCount, normalized.c_str());
    Wh_FreeStringSetting(value);
}

static void ReadFontNameSetting(PCWSTR key, WCHAR* target, size_t maxCount) {
    PCWSTR value = Wh_GetStringSetting(key);
    const std::wstring normalized = TrimAndUnquote(value);
    Wh_FreeStringSetting(value);

    target[0] = L'\0';
    if (normalized.empty()) {
        return;
    }

    if (normalized.size() >= maxCount) {
        Wh_Log(L"Font: setting '%s' exceeds the GDI typeface-name limit of %u characters",
               key, static_cast<UINT>(maxCount - 1));
        return;
    }

    (void)StringCchCopyW(target, maxCount, normalized.c_str());
}

static int ParseFontWeightSetting(PCWSTR value) {
    if (_wcsicmp(value, L"thin") == 0) return 100;
    if (_wcsicmp(value, L"extraLight") == 0) return 200;
    if (_wcsicmp(value, L"light") == 0) return 300;
    if (_wcsicmp(value, L"medium") == 0) return 500;
    if (_wcsicmp(value, L"semiBold") == 0) return 600;
    if (_wcsicmp(value, L"bold") == 0) return 700;
    if (_wcsicmp(value, L"extraBold") == 0) return 800;
    if (_wcsicmp(value, L"black") == 0) return 900;
    return FW_NORMAL;
}

static std::wstring SanitizePath(std::wstring_view path) {
    std::wstring rawPath = TrimAndUnquote(path);
    if (rawPath.empty()) return L"";

    DWORD needed = ExpandEnvironmentStringsW(rawPath.c_str(), NULL, 0);
    if (needed > 0) {
        std::wstring expanded(needed, L'\0');
        DWORD result = ExpandEnvironmentStringsW(rawPath.c_str(), &expanded[0], needed);
        if (result > 0 && result <= needed) {
            expanded.resize(result - 1);
            return expanded;
        }
    }

    return rawPath;
}

static bool GetShellStringIndirect(PCWSTR dllName, UINT resourceId, WCHAR* buffer, size_t maxCount) {
    WCHAR source[128];
    StringCchPrintfW(source, ARRAYSIZE(source), L"@%s,-%u", dllName, resourceId);
    return SUCCEEDED(SHLoadIndirectString(source, buffer, static_cast<UINT>(maxCount), NULL)) &&
           buffer[0] != L'\0';
}

static void GetShell32String(UINT resourceId, WCHAR* buffer,
                             size_t maxCount, PCWSTR fallback) {
    if (!GetShellStringIndirect(L"shell32.dll", resourceId, buffer, maxCount)) {
        StringCchCopyW(buffer, maxCount, fallback);
    }
}

struct RecycleBinTooltipLabels {
    WCHAR name[64] = L"";
    WCHAR empty[64] = L"";
};

// Cache the two tray tooltip labels after their first use.
static const RecycleBinTooltipLabels& GetRecycleBinTooltipLabels() {
    static RecycleBinTooltipLabels labels;
    static bool initialized = false;
    if (initialized) return labels;
    initialized = true;

    GetShell32String(8964, labels.name, ARRAYSIZE(labels.name), L"Recycle Bin");
    GetShell32String(30389, labels.empty, ARRAYSIZE(labels.empty), L"Recycle Bin (empty)");
    return labels;
}

void LoadPathSetting(PCWSTR settingName, std::wstring& outPath) {
    PCWSTR value = Wh_GetStringSetting(settingName);
    const std::wstring raw(value);
    Wh_FreeStringSetting(value);
    outPath = SanitizePath(raw);
}

static TrayAction ReadActionSetting(PCWSTR valueName, PCWSTR defaultValue) {
    WCHAR action[32];
    ReadStringSetting(valueName, action, ARRAYSIZE(action), defaultValue);
    return ParseTrayAction(action);
}

void LoadSettingsInto(ModSettings& s) {
    s.hideWhenEmpty = Wh_GetIntSetting(L"general.hideWhenEmpty") != 0;
    s.enableDragDrop = Wh_GetIntSetting(L"general.enableDragDrop") != 0;

    // Validate the icon renderer.
    WCHAR szStyle[32];
    ReadStringSetting(L"general.iconStyle", szStyle, ARRAYSIZE(szStyle), L"system");
    if (_wcsicmp(szStyle, L"vector") == 0) s.iconStyle = IconStyle::Vector;
    else if (_wcsicmp(szStyle, L"font") == 0) s.iconStyle = IconStyle::Font;
    else if (_wcsicmp(szStyle, L"custom") == 0) s.iconStyle = IconStyle::Custom;
    else s.iconStyle = IconStyle::System;

    // Accept only known vector variants.
    ReadStringSetting(L"vector.style", s.vectorStyle, ARRAYSIZE(s.vectorStyle), L"style1");
    if (_wcsicmp(s.vectorStyle, L"style1") != 0 &&
        _wcsicmp(s.vectorStyle, L"style2") != 0 &&
        _wcsicmp(s.vectorStyle, L"style3") != 0) {
        StringCchCopyW(s.vectorStyle, ARRAYSIZE(s.vectorStyle), L"style1");
    }

    // Font mode starts unconfigured; examples live in the settings descriptions.
    ReadFontNameSetting(L"font.empty.name", s.fontNameEmpty, ARRAYSIZE(s.fontNameEmpty));
    ReadTrimmedStringSetting(L"font.empty.code", s.fontCodeEmpty, ARRAYSIZE(s.fontCodeEmpty));
    WCHAR emptyWeight[16];
    ReadStringSetting(L"font.empty.weight", emptyWeight, ARRAYSIZE(emptyWeight), L"regular");
    s.fontWeightEmpty = ParseFontWeightSetting(emptyWeight);

    ReadFontNameSetting(L"font.full.name", s.fontNameFull, ARRAYSIZE(s.fontNameFull));
    ReadTrimmedStringSetting(L"font.full.code", s.fontCodeFull, ARRAYSIZE(s.fontCodeFull));
    WCHAR fullWeight[16];
    ReadStringSetting(L"font.full.weight", fullWeight, ARRAYSIZE(fullWeight), L"regular");
    s.fontWeightFull = ParseFontWeightSetting(fullWeight);

    WCHAR customColorMode[32];
    ReadStringSetting(
        L"customIcon.colorMode", customColorMode, ARRAYSIZE(customColorMode), L"original");
    s.customIconThemeTint = _wcsicmp(customColorMode, L"themeTint") == 0;

    // Normalize Light-theme sources and optional Dark-theme paths once.
    LoadPathSetting(L"customIcon.light.empty", s.customIconEmpty);
    LoadPathSetting(L"customIcon.light.full",  s.customIconFull);
    LoadPathSetting(L"customIcon.dark.empty", s.customIconEmptyDark);
    LoadPathSetting(L"customIcon.dark.full",  s.customIconFullDark);

    // Parse all mouse actions through one helper.
    s.leftClickAction   = ReadActionSetting(L"actions.leftClick",   L"none");
    s.doubleClickAction = ReadActionSetting(L"actions.doubleClick", L"open");
    s.middleClickAction = ReadActionSetting(L"actions.middleClick", L"empty");
    s.rightClickAction  = ReadActionSetting(L"actions.rightClick",  L"contextMenu");

    s.confirmEmpty = Wh_GetIntSetting(L"actions.confirmEmpty") != 0;

    // Clamp fallback polling to 0..24 hours.
    int interval = Wh_GetIntSetting(L"system.fallbackTimerInterval");
    if (interval < 0) {
        s.fallbackTimerInterval = 60;
    } else {
        s.fallbackTimerInterval = static_cast<UINT>(std::min(interval, 86400));
    }

    int dpiInterval = Wh_GetIntSetting(L"system.dpiCheckInterval");
    if (dpiInterval < 0) {
        s.dpiCheckInterval = 30;
    } else {
        s.dpiCheckInterval = static_cast<UINT>(std::min(dpiInterval, 86400));
    }
}

// Direct writes are safe only before the tray thread starts; later changes are marshalled through WM_APPLY_SETTINGS.
void LoadSettings() {
    LoadSettingsInto(g_settings);
}

static UINT GetSystemFallbackDpi() {
    UINT dpi = GetDpiForSystem();

    if (dpi == 0) {
        ScreenDC screenDC;
        if (screenDC) {
            dpi = GetDeviceCaps(screenDC.get(), LOGPIXELSX);
        }
    }

    return dpi ? dpi : 96;
}

static UINT GetDpiForReferenceWindow(HWND hWnd) {
    if (hWnd) {
        const UINT dpi = GetDpiForWindow(hWnd);
        if (dpi) {
            return dpi;
        }
    }

    return GetSystemFallbackDpi();
}

static int GetSmallIconMetricForDpi(UINT dpi) {
    const int size = GetSystemMetricsForDpi(SM_CXSMICON, dpi);
    return size > 0 ? size : MulDiv(16, static_cast<int>(dpi), 96);
}

int GetTrayRenderSize(HWND hWnd) {
    if (g_trayGeometryCache.renderSizeValid) {
        return g_trayGeometryCache.renderSize;
    }

    // The hidden host isn't positioned at the tray; use the physical overlay once available.
    HWND dpiReferenceWnd = g_hOverlayWnd ? g_hOverlayWnd : hWnd;
    const UINT dpi = GetDpiForReferenceWindow(dpiReferenceWnd);

    // Cache only the provisional render size here. Shell slot geometry becomes
    // authoritative after Shell_NotifyIconGetRect succeeds.
    g_trayGeometryCache.renderSize = GetSmallIconMetricForDpi(dpi);
    g_trayGeometryCache.renderSizeValid = true;
    return g_trayGeometryCache.renderSize;
}

static bool TryParseGlyph(std::wstring_view str, std::wstring& glyph) {
    glyph.clear();
    if (str.empty()) return false;

    const std::wstring temp(str);
    wchar_t* endPtr = nullptr;
    unsigned long value = wcstoul(temp.c_str(), &endPtr, 16);

    if (!endPtr || endPtr == temp.c_str() || *endPtr != L'\0' ||
        value == 0 || value > 0x10FFFF ||
        (value >= 0xD800 && value <= 0xDFFF)) {
        return false;
    }

    if (value <= 0xFFFF) {
        glyph.assign(1, static_cast<wchar_t>(value));
        return true;
    }

    value -= 0x10000;
    const wchar_t buf[3] = {
        static_cast<wchar_t>((value >> 10) + 0xD800),
        static_cast<wchar_t>((value & 0x3FF) + 0xDC00),
        L'\0'
    };
    glyph.assign(buf);
    return true;
}

static bool FontHasGlyph(std::wstring_view fontName, const std::wstring& glyph, int weight) {
    if (fontName.empty() || glyph.empty()) return false;

    ScreenDC screenDC;
    if (!screenDC) return false;

    const std::wstring requestedFont(fontName);

    LOGFONTW lf = {};
    lf.lfHeight = -16;
    lf.lfWeight = weight;
    lf.lfCharSet = DEFAULT_CHARSET;
    StringCchCopyW(lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), requestedFont.c_str());

    UniqueFont hFont(CreateFontIndirectW(&lf));
    if (!hFont) return false;

    SelectObjectScope fontSelect(screenDC.get(), hFont.get());

    WCHAR resolvedFace[LF_FACESIZE] = {};
    if (GetTextFaceW(screenDC.get(), ARRAYSIZE(resolvedFace), resolvedFace) <= 0 ||
        _wcsicmp(resolvedFace, requestedFont.c_str()) != 0) {
        return false;
    }

    std::vector<WORD> indices(glyph.size());
    const DWORD result = GetGlyphIndicesW(
        screenDC.get(), glyph.c_str(), static_cast<int>(glyph.size()),
        indices.data(), GGI_MARK_NONEXISTING_GLYPHS);

    if (result == GDI_ERROR) return false;

    return std::all_of(indices.begin(), indices.end(), [](WORD index) {
        return index != 0xFFFF;
    });
}

struct FontConfigCacheEntry {
    WCHAR glyph[3] = {};
    bool initialized = false;
    bool valid = false;
};

static FontConfigCacheEntry g_fontConfigCache[2];

static void InvalidateFontConfigCache() {
    g_fontConfigCache[0] = {};
    g_fontConfigCache[1] = {};
}

static bool ResolveFontConfig(bool emptyState, std::wstring& fontName,
                              std::wstring& glyph, int& weight) {
    const size_t index = emptyState ? 0 : 1;
    FontConfigCacheEntry& cache = g_fontConfigCache[index];

    const WCHAR* configuredFont =
        emptyState ? g_settings.fontNameEmpty : g_settings.fontNameFull;
    const WCHAR* configuredCode =
        emptyState ? g_settings.fontCodeEmpty : g_settings.fontCodeFull;

    fontName = configuredFont;
    weight = emptyState ? g_settings.fontWeightEmpty : g_settings.fontWeightFull;

    if (!cache.initialized) {
        cache.initialized = true;
        std::wstring parsedGlyph;
        cache.valid =
            !fontName.empty() &&
            TryParseGlyph(configuredCode, parsedGlyph) &&
            FontHasGlyph(fontName, parsedGlyph, weight);

        if (cache.valid) {
            (void)StringCchCopyW(
                cache.glyph, ARRAYSIZE(cache.glyph), parsedGlyph.c_str());
        }
    }

    if (!cache.valid) {
        glyph.clear();
        return false;
    }

    glyph.assign(cache.glyph);
    return true;
}

static BYTE GetThemeIconIntensity(bool isDarkTheme, BYTE darkIntensity = 240) {
    return isDarkTheme ? darkIntensity : 30;
}

// Shared monochrome output for Font and Custom Tint.
static HICON CreateIconFromAlphaMask(
    int iconSize, const std::vector<BYTE>& alphaMask, BYTE target) {
    if (iconSize <= 0 ||
        alphaMask.size() != static_cast<size_t>(iconSize) * iconSize) {
        return NULL;
    }

    ScreenDC screenDC;
    if (!screenDC) return NULL;

    UniqueMemDC hdcMem(CreateCompatibleDC(screenDC.get()));
    if (!hdcMem) return NULL;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iconSize;
    bmi.bmiHeader.biHeight = -iconSize;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* bits = NULL;
    UniqueBitmap colorBitmap(CreateDIBSection(
        hdcMem.get(), &bmi, DIB_RGB_COLORS,
        reinterpret_cast<void**>(&bits), NULL, 0));
    if (!colorBitmap || !bits) return NULL;

    for (size_t i = 0; i < alphaMask.size(); ++i) {
        const BYTE alpha = alphaMask[i];
        if (alpha == 0) {
            bits[i] = 0;
            continue;
        }

        const BYTE value =
            static_cast<BYTE>((static_cast<UINT>(target) * alpha) / 255);
        bits[i] =
            (static_cast<DWORD>(alpha) << 24) |
            (static_cast<DWORD>(value) << 16) |
            (static_cast<DWORD>(value) << 8) |
            value;
    }

    const size_t maskStrideBytes =
        ((static_cast<size_t>(iconSize) + 15) / 16) * 2;
    std::vector<BYTE> maskBits(maskStrideBytes * iconSize, 0);

    UniqueBitmap maskBitmap(
        CreateBitmap(iconSize, iconSize, 1, 1, maskBits.data()));
    if (!maskBitmap) return NULL;

    ICONINFO iconInfo = {
        TRUE, 0, 0, maskBitmap.get(), colorBitmap.get()
    };
    return CreateIconIndirect(&iconInfo);
}

// Find the largest GDI font height whose actual rasterized ink fits the icon.
// The normal size is returned unchanged unless the glyph would be clipped.
static int GetFontHeightToFit(
    HDC screenDC, int iconSize, const std::wstring& glyphStr,
    std::wstring_view fontName, int weight) {
    if (!screenDC || iconSize <= 1 || glyphStr.empty() || fontName.empty()) {
        return iconSize;
    }

    // Keep the normal icon-sized layout rectangle, but render it without clipping
    // into a padded temporary surface so overhanging ink remains measurable.
    const int padding = iconSize * 2;
    const int probeSize = iconSize + padding * 2;

    UniqueMemDC probeDC(CreateCompatibleDC(screenDC));
    if (!probeDC) {
        return iconSize;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = probeSize;
    bmi.bmiHeader.biHeight = -probeSize;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* probeBits = NULL;
    UniqueBitmap probeBitmap(CreateDIBSection(
        probeDC.get(), &bmi, DIB_RGB_COLORS,
        reinterpret_cast<void**>(&probeBits), NULL, 0));
    if (!probeBitmap || !probeBits) {
        return iconSize;
    }

    SelectObjectScope bitmapSelect(probeDC.get(), probeBitmap.get());
    SetBkMode(probeDC.get(), TRANSPARENT);
    SetTextColor(probeDC.get(), RGB(255, 255, 255));

    const RECT targetRect = {
        padding, padding, padding + iconSize, padding + iconSize
    };
    const std::wstring requestedFont(fontName);

    for (int fontHeight = iconSize; fontHeight >= 1; --fontHeight) {
        ZeroMemory(
            probeBits,
            static_cast<size_t>(probeSize) * probeSize * sizeof(DWORD));

        LOGFONTW lf = {};
        lf.lfHeight = -fontHeight;
        lf.lfWeight = weight;
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfQuality = ANTIALIASED_QUALITY;
        StringCchCopyW(
            lf.lfFaceName, ARRAYSIZE(lf.lfFaceName), requestedFont.c_str());

        UniqueFont font(CreateFontIndirectW(&lf));
        if (!font) {
            return iconSize;
        }

        {
            SelectObjectScope fontSelect(probeDC.get(), font.get());
            RECT drawRect = targetRect;
            DrawTextW(
                probeDC.get(), glyphStr.c_str(), -1, &drawRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

            // GDI may batch DrawTextW; flush before reading CreateDIBSection bits.
            GdiFlush();
        }

        bool inkOutsideTarget = false;
        for (int y = 0; y < probeSize && !inkOutsideTarget; ++y) {
            const bool outsideY = y < targetRect.top || y >= targetRect.bottom;
            for (int x = 0; x < probeSize; ++x) {
                if ((probeBits[static_cast<size_t>(y) * probeSize + x] &
                     0x00FFFFFF) == 0) {
                    continue;
                }

                if (outsideY || x < targetRect.left || x >= targetRect.right) {
                    inkOutsideTarget = true;
                    break;
                }
            }
        }

        if (!inkOutsideTarget) {
            return fontHeight;
        }
    }

    return 1;
}

// Font icon renderer
HICON CreateFontIcon(int iconSize, const std::wstring& glyphStr, std::wstring_view fontName, bool isDarkTheme, int weight) {
    const int cx = iconSize;
    const int cy = cx;

    // RAII keeps every GDI resource balanced across early returns.
    ScreenDC screenDC;
    if (!screenDC) return NULL;

    UniqueMemDC hdcMem(CreateCompatibleDC(screenDC.get()));
    if (!hdcMem) return NULL;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = -cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    DWORD* pBits = NULL;
    UniqueBitmap hbmColor(CreateDIBSection(hdcMem.get(), &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0));
    if (!hbmColor) return NULL;

    SelectObjectScope bmpSelect(hdcMem.get(), hbmColor.get());
    ZeroMemory(pBits, cx * cy * sizeof(DWORD));

    const int fontHeight =
        GetFontHeightToFit(screenDC.get(), cy, glyphStr, fontName, weight);
    if (fontHeight < cy) {
        Wh_Log(L"Font: glyph reduced from %d to %d px to avoid clipping",
               cy, fontHeight);
    }

    LOGFONTW lf = {};
    lf.lfHeight = -fontHeight;
    lf.lfWeight = weight;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfQuality = ANTIALIASED_QUALITY;

    if (fontName.empty() || glyphStr.empty()) return NULL;
    StringCchCopyW(lf.lfFaceName, LF_FACESIZE, std::wstring(fontName).c_str());

    UniqueFont hFont(CreateFontIndirectW(&lf));
    if (!hFont) return NULL;

    SelectObjectScope fontSelect(hdcMem.get(), hFont.get());

    SetBkMode(hdcMem.get(), TRANSPARENT);
    SetTextColor(hdcMem.get(), RGB(255, 255, 255));

    RECT rc = {0, 0, cx, cy};
    DrawTextW(hdcMem.get(), glyphStr.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // GDI may batch DrawTextW; flush before reading CreateDIBSection bits.
    GdiFlush();

    std::vector<BYTE> alphaMask(static_cast<size_t>(cx) * cy);
    for (int i = 0; i < cx * cy; ++i) {
        const DWORD pixel = pBits[i];
        alphaMask[i] = static_cast<BYTE>(
            (((pixel >> 16) & 0xFF) +
             ((pixel >> 8) & 0xFF) +
             (pixel & 0xFF)) / 3);
    }

    return CreateIconFromAlphaMask(
        cx, alphaMask, GetThemeIconIntensity(isDarkTheme));
}

// Vector icon renderer
static void AddRoundedRect(Gdiplus::GraphicsPath& path, float x, float y, float w, float h, float r) {
    float d = r * 2.0f;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

static void BuildHandlePath_Style3(Gdiplus::GraphicsPath& path, float s) {
    path.Reset();
    AddRoundedRect(path, 6.3f * s, 1.0f * s, 3.4f * s, 2.2f * s, 0.5f * s);
}

static void BuildLidPath_Style3(Gdiplus::GraphicsPath& path, float s) {
    path.Reset();
    AddRoundedRect(path, 2.0f * s, 2.6f * s, 12.0f * s, 1.8f * s, 0.7f * s);
}

static void BuildBodyPath_Style3(Gdiplus::GraphicsPath& path, float s, float inset = 0.0f) {
    float x = (3.4f + inset) * s;
    float y = (5.6f + inset) * s;
    float w = (12.6f - 3.4f - 2.0f * inset) * s;
    float h = (15.2f - 5.6f - 2.0f * inset) * s;
    float r = ((0.9f - inset) > 0.25f ? (0.9f - inset) : 0.25f) * s;
    path.Reset();
    AddRoundedRect(path, x, y, w, h, r);
}

// Style 2 follows a Fluent-like tray silhouette: compact, rounded and simple.
// Each side wall is one uninterrupted diagonal down to the rounded base.
static void BuildHandlePath_Style2(Gdiplus::GraphicsPath& path, float s) {
    path.Reset();
    path.AddArc(5.62f * s, 1.10f * s, 4.76f * s, 4.06f * s, 180.0f, 180.0f);
}

static void BuildLidPath_Style2(Gdiplus::GraphicsPath& path, float s) {
    path.Reset();
    AddRoundedRect(path, 1.85f * s, 3.05f * s, 12.30f * s, 1.55f * s, 0.68f * s);
}

static void BuildBodyOutline_Style2(Gdiplus::GraphicsPath& path, float s) {
    const float xTopL = 3.50f * s;
    const float xTopR = 12.50f * s;
    const float xBotL = 4.20f * s;
    const float xBotR = 11.80f * s;
    const float yTop = 4.55f * s;
    const float yBot = 14.45f * s;
    const float r = 0.85f * s;

    path.Reset();

    // Left wall: one continuous diagonal, then a smooth lower corner.
    path.AddLine(xTopL, yTop, xBotL, yBot - r);
    path.AddBezier(
        xBotL, yBot - r,
        xBotL, yBot - 0.28f * s,
        xBotL + 0.28f * s, yBot,
        xBotL + r, yBot);

    // Bottom edge and mirrored lower-right corner.
    path.AddLine(xBotL + r, yBot, xBotR - r, yBot);
    path.AddBezier(
        xBotR - r, yBot,
        xBotR - 0.28f * s, yBot,
        xBotR, yBot - 0.28f * s,
        xBotR, yBot - r);

    // Right wall: one continuous diagonal back to the lid.
    path.AddLine(xBotR, yBot - r, xTopR, yTop);
}

static void BuildBodyFill_Style2(Gdiplus::GraphicsPath& path, float s) {
    const float xTopL = 3.50f * s;
    const float xTopR = 12.50f * s;
    const float xBotL = 4.20f * s;
    const float xBotR = 11.80f * s;
    const float yTop = 4.45f * s;
    const float yBot = 14.45f * s;
    const float r = 0.85f * s;

    path.Reset();
    path.AddLine(xTopL, yTop, xTopR, yTop);
    path.AddLine(xTopR, yTop, xBotR, yBot - r);
    path.AddBezier(
        xBotR, yBot - r,
        xBotR, yBot - 0.28f * s,
        xBotR - 0.28f * s, yBot,
        xBotR - r, yBot);
    path.AddLine(xBotR - r, yBot, xBotL + r, yBot);
    path.AddBezier(
        xBotL + r, yBot,
        xBotL + 0.28f * s, yBot,
        xBotL, yBot - 0.28f * s,
        xBotL, yBot - r);
    path.AddLine(xBotL, yBot - r, xTopL, yTop);
    path.CloseFigure();
}

// Style 1 uses a 24x24 logical coordinate system. The empty silhouette follows
// the compact geometric trash glyph, while the full state keeps the exact same
// body and lifts the lid to expose two simple pieces of contents.
//
// Keeping all geometry in logical coordinates and applying one scale factor makes
// the result independent of the final tray size. The existing 4x supersampling
// then handles 16/20/24/32/40/48/64 px targets consistently across DPI changes.
static void BuildBodyPath_Style1(Gdiplus::GraphicsPath& path, float s) {
    path.Reset();
    const Gdiplus::PointF points[] = {
        { 4.66675f * s,  9.33325f * s },
        {19.33340f * s,  9.33325f * s },
        {18.00010f * s, 21.33330f * s },
        { 6.00008f * s, 21.33330f * s },
    };
    path.AddPolygon(points, ARRAYSIZE(points));
}

static void BuildLidPath_Style1(Gdiplus::GraphicsPath& path, float s) {
    path.Reset();
    const Gdiplus::PointF points[] = {
        {16.00010f * s, 3.99992f * s },
        {14.66670f * s, 1.33325f * s },
        { 9.33342f * s, 1.33325f * s },
        { 8.00008f * s, 3.99992f * s },
        { 2.66675f * s, 3.99992f * s },
        { 2.66675f * s, 6.66659f * s },
        {21.33340f * s, 6.66659f * s },
        {21.33340f * s, 3.99992f * s },
    };
    path.AddPolygon(points, ARRAYSIZE(points));
}

static void BuildContentsPath_Style1(Gdiplus::GraphicsPath& path, float s) {
    path.Reset();

    // Small left piece. Its bottom remains slightly above the body opening so it
    // doesn't visually merge with the bin after downsampling at 16 px.
    const Gdiplus::PointF leftPiece[] = {
        { 8.20f * s, 8.88f * s },
        { 8.55f * s, 7.72f * s },
        {10.72f * s, 8.02f * s },
        {11.05f * s, 8.88f * s },
    };
    path.AddPolygon(leftPiece, ARRAYSIZE(leftPiece));

    // Taller right piece. The top is deliberately close to the lifted lid while
    // preserving a small optical gap, matching the compact taskbar rendition.
    const Gdiplus::PointF rightPiece[] = {
        {13.20f * s, 8.88f * s },
        {13.45f * s, 7.35f * s },
        {15.20f * s, 6.68f * s },
        {16.65f * s, 7.58f * s },
        {16.35f * s, 8.88f * s },
    };
    path.AddPolygon(rightPiece, ARRAYSIZE(rightPiece));
}

static void TiltLidPath_Style1(Gdiplus::GraphicsPath& path, float s) {
    // Negative angle raises the right side in GDI+'s screen coordinate system.
    // The pivot is near the visual center of the closed lid, keeping the full
    // state inside the original 24x24 logical bounds.
    Gdiplus::Matrix matrix;
    matrix.RotateAt(-8.0f, Gdiplus::PointF(12.0f * s, 5.0f * s));
    path.Transform(&matrix);
}

HICON CreateVectorTrashIcon(int iconSize, bool isEmpty, bool isDarkTheme, std::wstring_view vectorStyle) {
    if (!EnsureGdiplusInitialized()) return NULL;

    const int cx = iconSize;
    const int cy = cx;

    const int bigW = cx * VECTOR_SUPERSAMPLE;
    const int bigH = cy * VECTOR_SUPERSAMPLE;
    const float s = static_cast<float>(bigW) / 16.0f;
    const float s24 = static_cast<float>(bigW) / 24.0f;

    Gdiplus::Bitmap bigBmp(bigW, bigH, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics g(&bigBmp);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

        const bool useStyle1 = (vectorStyle.compare(L"style1") == 0);
        const bool useStyle2 = (vectorStyle.compare(L"style2") == 0);

        // Styles 1 and 2 target the same bright foreground as Windows tray glyphs.
        const bool useBrightTrayColor = useStyle1 || useStyle2;
        const Gdiplus::Color color =
            (isDarkTheme && useBrightTrayColor)
                ? Gdiplus::Color(255, 255, 255, 255)
                : (isDarkTheme ? Gdiplus::Color(255, 240, 240, 240)
                               : Gdiplus::Color(255, 30, 30, 30));
        const Gdiplus::SolidBrush brush(color);

        if (useStyle1) {
            // Body is intentionally identical in both states. The state change is
            // communicated only by the lid and contents, preserving a stable tray
            // silhouette and avoiding size/weight jumps between empty and full.
            Gdiplus::GraphicsPath bodyPath;
            BuildBodyPath_Style1(bodyPath, s24);
            g.FillPath(&brush, &bodyPath);

            if (isEmpty) {
                Gdiplus::GraphicsPath lidPath;
                BuildLidPath_Style1(lidPath, s24);
                g.FillPath(&brush, &lidPath);
            } else {
                // Contents are painted before the lid so they appear behind it.
                Gdiplus::GraphicsPath contentsPath;
                BuildContentsPath_Style1(contentsPath, s24);
                g.FillPath(&brush, &contentsPath);

                Gdiplus::GraphicsPath lidPath;
                BuildLidPath_Style1(lidPath, s24);
                TiltLidPath_Style1(lidPath, s24);
                g.FillPath(&brush, &lidPath);
            }
        } else if (useStyle2) {
            // A one-unit stroke is about 2 px at the common 32 px tray size.
            Gdiplus::Pen pen(color, 1.35f * s);
            pen.SetLineJoin(Gdiplus::LineJoinRound);
            pen.SetStartCap(Gdiplus::LineCapRound);
            pen.SetEndCap(Gdiplus::LineCapRound);

            Gdiplus::Pen handlePen(color, 1.35f * s);
            handlePen.SetLineJoin(Gdiplus::LineJoinRound);
            handlePen.SetStartCap(Gdiplus::LineCapRound);
            handlePen.SetEndCap(Gdiplus::LineCapRound);

            // Draw the body first so the lid naturally covers the top endpoints.
            if (isEmpty) {
                Gdiplus::GraphicsPath bodyOutline;
                BuildBodyOutline_Style2(bodyOutline, s);
                g.DrawPath(&pen, &bodyOutline);
            } else {
                Gdiplus::GraphicsPath bodyFill;
                BuildBodyFill_Style2(bodyFill, s);
                g.FillPath(&brush, &bodyFill);

                // Match the empty-state outer footprint: its centered stroke
                // otherwise makes the hollow body look wider near the lid.
                g.DrawPath(&pen, &bodyFill);
            }

            // Hollow arch handle; its lower ends are hidden by the lid.
            Gdiplus::GraphicsPath handlePath;
            BuildHandlePath_Style2(handlePath, s);
            g.DrawPath(&handlePen, &handlePath);

            Gdiplus::GraphicsPath lidPath;
            BuildLidPath_Style2(lidPath, s);
            g.FillPath(&brush, &lidPath);
        } else {
            // Style 3 is the compact symbolic renderer.
            Gdiplus::GraphicsPath handlePath;
            BuildHandlePath_Style3(handlePath, s);
            g.FillPath(&brush, &handlePath);

            Gdiplus::GraphicsPath lidPath;
            BuildLidPath_Style3(lidPath, s);
            g.FillPath(&brush, &lidPath);

            if (isEmpty) {
                const float wall = 1.4f;
                Gdiplus::GraphicsPath outerPath;
                BuildBodyPath_Style3(outerPath, s, 0.0f);
                Gdiplus::GraphicsPath innerPath;
                BuildBodyPath_Style3(innerPath, s, wall);

                Gdiplus::Region region(&outerPath);
                region.Exclude(&innerPath);

                const float gapX = (3.4f + wall) * s;
                const float gapW = (12.6f - 3.4f - 2.0f * wall) * s;
                const float gapY = 5.0f * s;
                const float gapH = 2.0f * s;
                region.Exclude(Gdiplus::RectF(gapX, gapY, gapW, gapH));

                g.FillRegion(&brush, &region);
            } else {
                Gdiplus::GraphicsPath bodyPath;
                BuildBodyPath_Style3(bodyPath, s, 0.0f);
                g.FillPath(&brush, &bodyPath);
            }
        }
    }

    Gdiplus::Bitmap finalBmp(cx, cy, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics g(&finalBmp);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        
        g.DrawImage(&bigBmp, 0, 0, cx, cy);
    }

    HICON hIcon = NULL;
    finalBmp.GetHICON(&hIcon);
    return hIcon;
}

// System and custom icon loaders
static HICON LoadShellStockIcon(SHSTOCKICONID iconId) {
    SHSTOCKICONINFO info = { sizeof(info) };
    return SUCCEEDED(SHGetStockIconInfo(iconId, SHGSI_ICON | SHGSI_SMALLICON, &info))
        ? info.hIcon
        : NULL;
}

// Request the stock empty/full icon directly to avoid stale PIDL icon-cache state.
HICON GetSystemRecycleBinIcon(bool isEmpty) {
    return LoadShellStockIcon(isEmpty ? SIID_RECYCLER : SIID_RECYCLERFULL);
}

const std::wstring& GetCustomIconPath(bool isEmpty, bool isDarkTheme) {
    const std::wstring& primary =
        isEmpty ? g_settings.customIconEmpty : g_settings.customIconFull;

    if (!isDarkTheme) {
        return primary;
    }

    const std::wstring& darkOverride =
        isEmpty ? g_settings.customIconEmptyDark : g_settings.customIconFullDark;
    return darkOverride.empty() ? primary : darkOverride;
}

bool IsIcoExtension(std::wstring_view path) {
    const size_t dotIndex = path.rfind(L'.');
    if (dotIndex == std::wstring_view::npos) {
        return false;
    }

    const std::wstring_view ext = path.substr(dotIndex);
    if (ext.length() != 4) {
        return false;
    }

    return _wcsnicmp(ext.data(), L".ico", 4) == 0;
}

// Downsample source alpha by exact pixel-area coverage instead of interpolation.
static bool BuildAreaResampledAlphaMask(
    Gdiplus::Bitmap& source,
    int iconSize,
    bool preserveThinStrokes,
    std::vector<BYTE>& alphaMask) {
    const int srcWidth = static_cast<int>(source.GetWidth());
    const int srcHeight = static_cast<int>(source.GetHeight());
    if (srcWidth <= 0 || srcHeight <= 0 || iconSize <= 0) {
        return false;
    }

    Gdiplus::Rect sourceRect(0, 0, srcWidth, srcHeight);
    Gdiplus::BitmapData sourceData = {};
    if (source.LockBits(
            &sourceRect,
            Gdiplus::ImageLockModeRead,
            PixelFormat32bppARGB,
            &sourceData) != Gdiplus::Ok) {
        return false;
    }

    std::vector<BYTE> sourceAlpha(
        static_cast<size_t>(srcWidth) * srcHeight);

    for (int y = 0; y < srcHeight; ++y) {
        const BYTE* row =
            static_cast<const BYTE*>(sourceData.Scan0) +
            y * sourceData.Stride;

        for (int x = 0; x < srcWidth; ++x) {
            sourceAlpha[static_cast<size_t>(y) * srcWidth + x] =
                row[x * 4 + 3];
        }
    }

    source.UnlockBits(&sourceData);

    if (preserveThinStrokes) {
        std::vector<BYTE> expandedAlpha(sourceAlpha);

        for (int y = 0; y < srcHeight; ++y) {
            for (int x = 0; x < srcWidth; ++x) {
                BYTE neighborMax = 0;

                for (int dy = -1; dy <= 1; ++dy) {
                    const int ny = y + dy;
                    if (ny < 0 || ny >= srcHeight) continue;

                    for (int dx = -1; dx <= 1; ++dx) {
                        const int nx = x + dx;
                        if (nx < 0 || nx >= srcWidth ||
                            (dx == 0 && dy == 0)) {
                            continue;
                        }

                        neighborMax = std::max(
                            neighborMax,
                            sourceAlpha[
                                static_cast<size_t>(ny) * srcWidth + nx]);
                    }
                }

                // Preserve some subpixel stroke weight before 16 px reduction
                // without adding a full destination pixel of dilation.
                const BYTE spread = static_cast<BYTE>(
                    (static_cast<UINT>(neighborMax) * 45u + 50u) / 100u);

                BYTE& dstAlpha =
                    expandedAlpha[static_cast<size_t>(y) * srcWidth + x];
                dstAlpha = std::max(dstAlpha, spread);
            }
        }

        sourceAlpha.swap(expandedAlpha);
    }

    const double scale = std::min(
        static_cast<double>(iconSize) / srcWidth,
        static_cast<double>(iconSize) / srcHeight);
    if (scale <= 0.0) return false;

    const double destWidth = srcWidth * scale;
    const double destHeight = srcHeight * scale;
    const double destX = (iconSize - destWidth) / 2.0;
    const double destY = (iconSize - destHeight) / 2.0;
    const double inverseScale = 1.0 / scale;
    const double sourceAreaToDestArea = scale * scale;

    alphaMask.assign(static_cast<size_t>(iconSize) * iconSize, 0);

    for (int destYIndex = 0; destYIndex < iconSize; ++destYIndex) {
        const double srcY0 =
            (destYIndex - destY) * inverseScale;
        const double srcY1 =
            (destYIndex + 1.0 - destY) * inverseScale;
        const int srcYBegin = std::max(
            0, static_cast<int>(std::floor(srcY0)));
        const int srcYEnd = std::min(
            srcHeight, static_cast<int>(std::ceil(srcY1)));

        if (srcYBegin >= srcYEnd) continue;

        for (int destXIndex = 0; destXIndex < iconSize; ++destXIndex) {
            const double srcX0 =
                (destXIndex - destX) * inverseScale;
            const double srcX1 =
                (destXIndex + 1.0 - destX) * inverseScale;
            const int srcXBegin = std::max(
                0, static_cast<int>(std::floor(srcX0)));
            const int srcXEnd = std::min(
                srcWidth, static_cast<int>(std::ceil(srcX1)));

            if (srcXBegin >= srcXEnd) continue;

            double weightedAlpha = 0.0;

            for (int srcYIndex = srcYBegin;
                 srcYIndex < srcYEnd;
                 ++srcYIndex) {
                const double overlapY = std::max(
                    0.0,
                    std::min(srcY1, srcYIndex + 1.0) -
                        std::max(srcY0, static_cast<double>(srcYIndex)));
                if (overlapY <= 0.0) continue;

                for (int srcXIndex = srcXBegin;
                     srcXIndex < srcXEnd;
                     ++srcXIndex) {
                    const double overlapX = std::max(
                        0.0,
                        std::min(srcX1, srcXIndex + 1.0) -
                            std::max(srcX0, static_cast<double>(srcXIndex)));
                    if (overlapX <= 0.0) continue;

                    weightedAlpha +=
                        sourceAlpha[
                            static_cast<size_t>(srcYIndex) * srcWidth +
                            srcXIndex] *
                        overlapX * overlapY;
                }
            }

            const double coverage =
                std::clamp(
                    weightedAlpha * sourceAreaToDestArea,
                    0.0,
                    255.0);

            alphaMask[
                static_cast<size_t>(destYIndex) * iconSize +
                destXIndex] =
                static_cast<BYTE>(coverage + 0.5);
        }
    }

    return true;
}

// Thin monochrome raster strokes can become too faint at the 16 px tray
// target. Dark theme tint gets a small coverage remap after resampling.
static BYTE RemapSmallDarkTintAlpha(BYTE alpha) {
    const UINT a = alpha;

    if (a <= 28u) return 0;
    if (a >= 160u) return 255;

    UINT out;
    if (a < 96u) {
        // Keep a narrow antialiasing fringe below the main stroke.
        out = static_cast<UINT>((a - 28u) * 145u / 100u);
    } else {
        // Strong coverage becomes opaque sooner for a cleaner small-icon core.
        out = 100u +
              static_cast<UINT>((a - 96u) * 240u / 100u);
    }

    return static_cast<BYTE>(std::min<UINT>(255u, out));
}

static void StrengthenSmallDarkTintCoverage(
    int iconSize, bool isDarkTheme, std::vector<BYTE>& alphaMask) {
    if (iconSize != 16 || !isDarkTheme) {
        return;
    }

    for (BYTE& alpha : alphaMask) {
        alpha = RemapSmallDarkTintAlpha(alpha);
    }
}

HICON LoadCustomIcon(
    int iconSize, std::wstring_view path, bool isDarkTheme, bool themeTint) {
    if (path.empty()) return NULL;

    const std::wstring pathStr(path);

    Wh_Log(L"LoadCustomIcon attempting to load path: %s", pathStr.c_str());

    const int size = iconSize;

    // Native .ico loading avoids GDI+.
    if (IsIcoExtension(path)) {
        HICON hIcon = (HICON)LoadImageW(NULL, pathStr.c_str(), IMAGE_ICON, size, size, LR_LOADFROMFILE);
        if (hIcon) return hIcon;
    }

    // Raster formats and malformed .ico fall back to GDI+.
    if (!EnsureGdiplusInitialized()) return NULL;
    Gdiplus::Bitmap srcBmp(pathStr.c_str());
    const Gdiplus::Status status = srcBmp.GetLastStatus();
    if (status != Gdiplus::Ok) {
        Wh_Log(L"WARNING: Failed to load custom image from '%s' (GDI+ error status: %d)", pathStr.c_str(), status);
        return NULL;
    }

    const UINT imageFlags = srcBmp.GetFlags();
    const bool applyTint =
        themeTint && (imageFlags & Gdiplus::ImageFlagsHasAlpha) != 0;

    if (applyTint) {
        std::vector<BYTE> alphaMask;
        const bool preserveThinStrokes = isDarkTheme && size == 16;
        if (BuildAreaResampledAlphaMask(
                srcBmp, size, preserveThinStrokes, alphaMask)) {
            StrengthenSmallDarkTintCoverage(size, isDarkTheme, alphaMask);

            HICON tintedIcon = CreateIconFromAlphaMask(
                size, alphaMask, GetThemeIconIntensity(isDarkTheme, 255));
            if (tintedIcon) {
                return tintedIcon;
            }

            Wh_Log(L"WARNING: Failed to create area-resampled tinted HICON");
        } else {
            Wh_Log(L"WARNING: Failed to area-resample custom image alpha");
        }
    }

    // Original colors and tint failures use the existing GDI+ image resize.
    Gdiplus::Bitmap scaledBmp(size, size, PixelFormat32bppARGB);
    {
        Gdiplus::Graphics g(&scaledBmp);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);

        int srcWidth = srcBmp.GetWidth();
        int srcHeight = srcBmp.GetHeight();

        if (srcWidth > 0 && srcHeight > 0) {
            float scaleW = (float)size / srcWidth;
            float scaleH = (float)size / srcHeight;
            float scale = (scaleW < scaleH) ? scaleW : scaleH;
            float destWidth = srcWidth * scale;
            float destHeight = srcHeight * scale;
            float destX = (size - destWidth) / 2.0f;
            float destY = (size - destHeight) / 2.0f;

            g.DrawImage(&srcBmp, destX, destY, destWidth, destHeight);
        } else {
            // Defensive fallback for invalid metadata.
            g.DrawImage(&srcBmp, 0, 0, size, size);
        }
    }

    HICON hIcon = NULL;
    scaledBmp.GetHICON(&hIcon);
    return hIcon;
}

// Shared icon cache key and renderer dispatch
IconCacheKey BuildIconCacheKey(bool isEmpty, bool isDark, int iconSize) {
    IconCacheKey key;
    key.style = g_settings.iconStyle;
    key.empty = isEmpty;
    key.dark = (g_settings.iconStyle == IconStyle::System) ? false : isDark;
    key.iconSize = iconSize;

    switch (g_settings.iconStyle) {
        case IconStyle::Vector:
            key.vectorStyle = g_settings.vectorStyle;
            break;
        case IconStyle::Font: {
            bool valid = ResolveFontConfig(isEmpty, key.fontName, key.glyph, key.fontWeight);

            // Empty-state fonts may reuse the full configuration when no
            // dedicated empty glyph is available.
            if (!valid && isEmpty) {
                valid = ResolveFontConfig(false, key.fontName, key.glyph, key.fontWeight);
            }

            if (!valid) {
                key.style = IconStyle::System;
                key.dark = false;
                key.fontName.clear();
                key.glyph.clear();
                key.fontWeight = FW_NORMAL;
            }
            break;
        }
        case IconStyle::Custom:
            key.customPath = GetCustomIconPath(isEmpty, isDark);
            key.customThemeTint = g_settings.customIconThemeTint;
            break;
        case IconStyle::System:
        default:
            break;
    }

    return key;
}

HICON LoadDesiredIcon(const IconCacheKey& key) {
    switch (key.style) {
        case IconStyle::Vector:
            return CreateVectorTrashIcon(key.iconSize, key.empty, key.dark, key.vectorStyle);
        case IconStyle::Font:
            return CreateFontIcon(key.iconSize, key.glyph, key.fontName, key.dark, key.fontWeight);
        case IconStyle::Custom:
            return LoadCustomIcon(
                key.iconSize, key.customPath, key.dark, key.customThemeTint);
        case IconStyle::System:
        default:
            return GetSystemRecycleBinIcon(key.empty);
    }
}

// Shell actions and Recycle Bin commands
bool SafeShellExecute(HWND hWnd, PCWSTR verb, PCWSTR file, PCWSTR params = NULL, INT nShowCmd = SW_SHOWNORMAL) {
    ShellModalScope modalScope(hWnd);
    HINSTANCE hInst = ShellExecuteW(hWnd, verb, file, params, NULL, nShowCmd);
    const INT_PTR result = reinterpret_cast<INT_PTR>(hInst);

    if (result <= 32) {
        Wh_Log(L"ShellExecuteW failed (%d) for target: %s", static_cast<int>(result), file ? file : L"");
        return false;
    }
    return true;
}

static bool QueryRecycleBinInfo(SHQUERYRBINFO& info) {
    info = {};
    info.cbSize = sizeof(info);

    const HRESULT hr = SHQueryRecycleBinW(NULL, &info);
    if (FAILED(hr)) {
        Wh_Log(L"SHQueryRecycleBinW failed: 0x%08X", hr);
        return false;
    }
    return true;
}

void EmptyRecycleBinAction(HWND hWnd) {
    SHQUERYRBINFO info;
    if (!QueryRecycleBinInfo(info) || info.i64NumItems == 0) {
        return;
    }

    const DWORD flags = g_settings.confirmEmpty ? 0 : SHERB_NOCONFIRMATION;
    ShellModalScope modalScope(hWnd);
    const HRESULT hr = SHEmptyRecycleBinW(hWnd, NULL, flags);
    if (FAILED(hr) && hr != E_ABORT && hr != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        Wh_Log(L"SHEmptyRecycleBinW failed: 0x%08X", hr);
    }
}

void OpenPropertiesAction(HWND hWnd) {
    SafeShellExecute(hWnd, L"properties", L"shell:RecycleBinFolder");
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN) {
            const MSLLHOOKSTRUCT* pMouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
            g_dragStartPt = pMouse->pt;
            if (g_hWnd) {
                (void)PostMessageW(g_hWnd, WM_USER_START_DRAG_POLL, 0, 0);
            }
        } else if (wParam == WM_LBUTTONUP) {
            if (g_hWnd) {
                (void)PostMessageW(g_hWnd, WM_USER_STOP_DRAG_POLL, 0, 0);
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

void UpdateDragDropHookState(bool enable) {
    if (enable) {
        if (!g_hMouseHook) {
            g_hMouseHook = SetWindowsHookExW(
                WH_MOUSE_LL,
                LowLevelMouseProc,
                g_hThisModule,
                0
            );
            if (!g_hMouseHook) {
                Wh_Log(L"Failed to install WH_MOUSE_LL hook: %lu", GetLastError());
            }
        }
    } else {
        if (g_hMouseHook) {
            (void)UnhookWindowsHookEx(g_hMouseHook);
            g_hMouseHook = NULL;
        }
        if (g_hWnd) {
            (void)PostMessageW(g_hWnd, WM_USER_STOP_DRAG_POLL, 0, 0);
        }
    }
}

// The overlay window stays alive as the physical per-monitor DPI reference.
// Only the OLE drop target and global mouse hook are enabled on demand.
static bool SetDragDropEnabled(HWND hWnd, bool enable) {
    if (!enable) {
        UpdateDragDropHookState(false);
        g_dropOverlayArmed.store(false);
        g_trayState.dragRectRefreshed = false;
        HideDropOverlay();

        if (g_pDropTarget) {
            if (g_hOverlayWnd) {
                const HRESULT hr = RevokeDragDrop(g_hOverlayWnd);
                if (FAILED(hr) && hr != DRAGDROP_E_NOTREGISTERED) {
                    Wh_Log(L"D&D: RevokeDragDrop failed: 0x%08X", hr);
                }
            }
            g_pDropTarget->Release();
            g_pDropTarget = nullptr;
        }

        g_oleDragActive.store(false);
        return true;
    }

    if (!g_hOverlayWnd) {
        Wh_Log(L"D&D: cannot enable without overlay window");
        UpdateDragDropHookState(false);
        return false;
    }

    if (!g_pDropTarget) {
        RecycleBinDropTarget* target = new (std::nothrow) RecycleBinDropTarget(hWnd);
        if (!target) {
            Wh_Log(L"D&D: failed to allocate drop target");
            UpdateDragDropHookState(false);
            return false;
        }

        const HRESULT hr = RegisterDragDrop(g_hOverlayWnd, target);
        if (FAILED(hr)) {
            Wh_Log(L"D&D: RegisterDragDrop failed: 0x%08X", hr);
            target->Release();
            UpdateDragDropHookState(false);
            return false;
        }

        g_pDropTarget = target;
        Wh_Log(L"D&D: RegisterDragDrop succeeded.");
    }

    UpdateDragDropHookState(true);
    if (!g_hMouseHook) {
        const HRESULT hr = RevokeDragDrop(g_hOverlayWnd);
        if (FAILED(hr) && hr != DRAGDROP_E_NOTREGISTERED) {
            Wh_Log(L"D&D: rollback RevokeDragDrop failed: 0x%08X", hr);
        }
        g_pDropTarget->Release();
        g_pDropTarget = nullptr;
        return false;
    }

    return true;
}

static bool AddTrayIconAndNegotiateVersion(HWND hWnd, bool reInstantiation) {
    if (!Shell_NotifyIconW(NIM_ADD, &g_nid)) {
        return false;
    }

    if (reInstantiation) {
        Wh_Log(L"Tray: NIM_ADD succeeded (re-instantiation).");
    } else {
        Wh_Log(L"Tray: NIM_ADD succeeded.");
    }

    g_nid.uVersion = NOTIFYICON_VERSION_4;

    if (Shell_NotifyIconW(NIM_SETVERSION, &g_nid)) {
        g_trayVersion4 = true;
        Wh_Log(L"Tray: NIM_SETVERSION(4) succeeded.");
    } else {
        g_trayVersion4 = false;
        Wh_Log(L"Tray: NIM_SETVERSION(4) failed: %lu (fallback to legacy)",
               GetLastError());
    }

    g_iconVisible = true;
    InvalidateIconRectCache();

    if (RefreshTrayGeometry(hWnd, true)) {
        // Refresh once after the Shell has assigned the icon its real tray geometry.
        (void)PostMessageW(hWnd, WM_USER_REFRESH_AFTER_TRAY_ADD, 0, 0);
    }

    return true;
}

bool UpdateTrayState() {
    const HWND hWnd = GetSafeHwnd();
    if (!hWnd) return false;

    SHQUERYRBINFO rbInfo;
    if (!QueryRecycleBinInfo(rbInfo)) {
        return false;
    }

    const bool isEmpty = (rbInfo.i64NumItems == 0);

    if (!g_loggedInitialState) {
        g_loggedInitialState = true;
        const WCHAR* styleName = L"system";
        switch (g_settings.iconStyle) {
            case IconStyle::Vector: styleName = L"vector"; break;
            case IconStyle::Font: styleName = L"font"; break;
            case IconStyle::Custom: styleName = L"custom"; break;
            default: break;
        }
        Wh_Log(
            L"Startup: Recycle Bin empty=%d items=%lld size=%lld bytes; hideWhenEmpty=%d dragDrop=%d iconStyle=%s fallbackTimer=%u s dpiCheck=%u s",
            isEmpty ? 1 : 0, rbInfo.i64NumItems, rbInfo.i64Size,
            g_settings.hideWhenEmpty ? 1 : 0, g_settings.enableDragDrop ? 1 : 0,
            styleName, g_settings.fallbackTimerInterval, g_settings.dpiCheckInterval);
    }

    // Hide the notification icon when requested and the bin is empty.
    if (isEmpty && g_settings.hideWhenEmpty) {
        if (g_iconVisible) {
            Shell_NotifyIconW(NIM_DELETE, &g_nid);
            g_iconVisible = false;
            InvalidateIconRectCache();
        }
        g_hCurrentIcon.reset();
        g_trayState.lastKey = {};
        ShutdownGdiplusIfInitialized();

        UpdateOverlayState();
        return true;
    }

    // System stock icons do not depend on the app light/dark theme.
    const bool isDark = g_settings.iconStyle != IconStyle::System && GetSystemDarkTheme();

    // Recycle Bin polling reuses the last validated render size; geometry probes refresh it.
    const int iconSize = GetTrayRenderSize(hWnd);

    const IconCacheKey currentKey = BuildIconCacheKey(isEmpty, isDark, iconSize);

    // Re-render only when content, theme, size or settings changed.
    const bool iconUnchanged = !g_forceIconRegen && 
                               g_hCurrentIcon && 
                               (currentKey == g_trayState.lastKey);


    // Load or reuse the rendered icon.
    if (!iconUnchanged) {
        HICON rawIcon = LoadDesiredIcon(currentKey);

        // Safety fallback
        if (!rawIcon) {
            rawIcon = GetSystemRecycleBinIcon(isEmpty);
            if (!rawIcon) {
                Wh_Log(L"CRITICAL: Failed to load any icon");
                return false;
            }
        }

        // Replace the previous icon atomically through RAII.
        g_hCurrentIcon.reset(rawIcon);
        g_trayState.lastKey = currentKey;
    }

    // Tooltip
    const RecycleBinTooltipLabels& tooltipLabels = GetRecycleBinTooltipLabels();

    WCHAR newTip[128];
    if (isEmpty) {
        StringCchCopyW(newTip, ARRAYSIZE(newTip), tooltipLabels.empty);
    } else {
        WCHAR szFormattedSize[64] = {};
        if (StrFormatByteSizeW(rbInfo.i64Size, szFormattedSize,
                               ARRAYSIZE(szFormattedSize))) {
            StringCchPrintfW(newTip, ARRAYSIZE(newTip), L"%s : %lld (%s)",
                             tooltipLabels.name, rbInfo.i64NumItems, szFormattedSize);
        } else {
            StringCchPrintfW(newTip, ARRAYSIZE(newTip), L"%s : %lld",
                             tooltipLabels.name, rbInfo.i64NumItems);
        }
    }

    g_nid.hIcon = g_hCurrentIcon.get();
    StringCchCopyW(g_nid.szTip, ARRAYSIZE(g_nid.szTip), newTip);

    g_forceIconRegen = false;

    // Add or update the notification-area icon.
    bool success = false;
    if (!g_iconVisible) {
        success = AddTrayIconAndNegotiateVersion(hWnd, false);
    } else if (Shell_NotifyIconW(NIM_MODIFY, &g_nid)) {
        success = true;
    } else {
        // Re-add the icon if Explorer lost it.
        g_iconVisible = false;
        success = AddTrayIconAndNegotiateVersion(hWnd, true);
    }

    if (!success) {
        Wh_Log(L"Shell_NotifyIcon failed, last error: %lu", GetLastError());
    }

    // Keep a visible drag overlay aligned with the current icon.
    UpdateOverlayState();

    return success;
}

// Keep the native popup menu outside the taskbar area.
static RECT GetTaskbarExclusionRect(const RECT& iconRect) {
    RECT result = iconRect;
    const HMONITOR monitor = MonitorFromRect(&iconRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = { sizeof(info) };
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return result;
    }

    if (info.rcWork.bottom < info.rcMonitor.bottom && iconRect.top >= info.rcWork.bottom) {
        result = { info.rcMonitor.left, info.rcWork.bottom, info.rcMonitor.right, info.rcMonitor.bottom };
    } else if (info.rcWork.top > info.rcMonitor.top && iconRect.bottom <= info.rcWork.top) {
        result = { info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right, info.rcWork.top };
    } else if (info.rcWork.left > info.rcMonitor.left && iconRect.right <= info.rcWork.left) {
        result = { info.rcMonitor.left, info.rcMonitor.top, info.rcWork.left, info.rcMonitor.bottom };
    } else if (info.rcWork.right < info.rcMonitor.right && iconRect.left >= info.rcWork.right) {
        result = { info.rcWork.right, info.rcMonitor.top, info.rcMonitor.right, info.rcMonitor.bottom };
    }
    return result;
}

struct RecycleBinMenuLabels {
    WCHAR open[128] = L"";
    WCHAR empty[128] = L"";
    WCHAR properties[128] = L"";
};

// Resolve the three native Shell menu labels once without constructing a
// Shell context menu or loading context-menu extensions into this process.
static const RecycleBinMenuLabels& GetRecycleBinMenuLabels() {
    static RecycleBinMenuLabels labels;
    static bool initialized = false;
    if (initialized) return labels;
    initialized = true;

    GetShell32String(12850, labels.open, ARRAYSIZE(labels.open), L"Open");
    GetShell32String(10564, labels.empty, ARRAYSIZE(labels.empty), L"Empty Recycle Bin");
    GetShell32String(16534, labels.properties, ARRAYSIZE(labels.properties), L"Properties");
    return labels;
}

void ShowContextMenu(HWND hWnd) {
    SHQUERYRBINFO info;
    const bool stateKnown = QueryRecycleBinInfo(info);
    const bool isEmpty = !stateKnown || info.i64NumItems == 0;
    const RecycleBinMenuLabels& labels = GetRecycleBinMenuLabels();

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        Wh_Log(L"CreatePopupMenu failed: %lu", GetLastError());
        return;
    }

    const UINT emptyFlags = MF_STRING | (isEmpty ? MF_GRAYED : MF_ENABLED);
    if (!AppendMenuW(menu, MF_STRING, IDM_OPEN, labels.open) ||
        !AppendMenuW(menu, emptyFlags, IDM_EMPTY, labels.empty) ||
        !AppendMenuW(menu, MF_SEPARATOR, 0, NULL) ||
        !AppendMenuW(menu, MF_STRING, IDM_PROPERTIES, labels.properties)) {
        Wh_Log(L"AppendMenuW failed: %lu", GetLastError());
        DestroyMenu(menu);
        return;
    }

    RECT iconRect = {};
    POINT anchor = {};
    TPMPARAMS popupParams = { sizeof(popupParams) };
    TPMPARAMS* popupParamsPtr = nullptr;
    if (QueryTrayIconRect(hWnd, iconRect, true)) {
        anchor.x = (iconRect.left + iconRect.right) / 2;
        anchor.y = (iconRect.top + iconRect.bottom) / 2;
        popupParams.rcExclude = GetTaskbarExclusionRect(iconRect);
        popupParamsPtr = &popupParams;
    } else if (!GetCursorPos(&anchor)) {
        Wh_Log(L"GetCursorPos failed: %lu", GetLastError());
        DestroyMenu(menu);
        return;
    }

    (void)SetForegroundWindow(hWnd);
    const UINT cmd = TrackPopupMenuEx(
        menu,
        TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON | TPM_VERTICAL,
        anchor.x, anchor.y, hWnd, popupParamsPtr);

    (void)PostMessageW(hWnd, WM_NULL, 0, 0);
    (void)Shell_NotifyIconW(NIM_SETFOCUS, &g_nid);

    if (cmd == IDM_OPEN) {
        SafeShellExecute(hWnd, L"open", L"shell:RecycleBinFolder");
    } else if (cmd == IDM_EMPTY) {
        EmptyRecycleBinAction(hWnd);
    } else if (cmd == IDM_PROPERTIES) {
        OpenPropertiesAction(hWnd);
    }

    DestroyMenu(menu);
}

void ExecuteAction(TrayAction action, HWND hWnd) {
    switch (action) {
        case TrayAction::Open:
            SafeShellExecute(hWnd, L"open", L"shell:RecycleBinFolder");
            break;
        case TrayAction::ContextMenu:
            ShowContextMenu(hWnd);
            break;
        case TrayAction::Empty:
            EmptyRecycleBinAction(hWnd);
            break;
        case TrayAction::Properties:
            OpenPropertiesAction(hWnd);
            break;
        case TrayAction::None:
        default:
            break;
    }
}

bool QueryTrayIconRect(HWND hWnd, RECT& rect, bool forceRefresh) {
    if (!forceRefresh && g_trayState.isIconRectValid) {
        rect = g_trayState.cachedIconRect;
        return true;
    }

    NOTIFYICONIDENTIFIER nid = { sizeof(nid) };
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICON_ID;

    RECT fresh = {};
    const HRESULT hr = Shell_NotifyIconGetRect(&nid, &fresh);
    if (FAILED(hr) || fresh.right <= fresh.left || fresh.bottom <= fresh.top) {
        return false;
    }

    g_trayState.cachedIconRect = fresh;
    g_trayState.isIconRectValid = true;
    rect = fresh;
    return true;
}

void CheckDragStatus(HWND hWnd) {
    if (!g_settings.enableDragDrop || !g_iconVisible || !g_hOverlayWnd ||
        g_recycleWorkerBusy.load()) {
        g_dropOverlayArmed.store(false);
        HideDropOverlay();
        return;
    }

    const bool isDragging = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

    // WM_LBUTTONUP can precede Drop(); keep the target alive while OLE still owns it.
    if (!isDragging) {
        if (!g_oleDragActive.load()) {
            g_dropOverlayArmed.store(false);
            HideDropOverlay();
        }
        return;
    }

    POINT pt = {};
    if (!GetCursorPos(&pt)) {
        return;
    }

    // A simple click must never activate the overlay. Arm it only after the
    // pointer moved beyond the system drag threshold.
    const int dragThresholdX = GetSystemMetrics(SM_CXDRAG);
    const int dragThresholdY = GetSystemMetrics(SM_CYDRAG);
    const bool movedEnoughToBeDrag =
        (abs(pt.x - g_dragStartPt.x) > dragThresholdX) ||
        (abs(pt.y - g_dragStartPt.y) > dragThresholdY);
    if (!movedEnoughToBeDrag) {
        return;
    }

    RECT iconRect = { 0 };
    // A real drag gets one fresh Shell rectangle so manual tray rearrangement
    // is observed without making ordinary clicks query Explorer.
    const bool forceRectRefresh = !g_trayState.dragRectRefreshed;
    if (!QueryTrayIconRect(hWnd, iconRect, forceRectRefresh)) {
        return;
    }
    g_trayState.dragRectRefreshed = true;

    const bool isOverIcon = PtInRect(&iconRect, pt) != FALSE;

    if (isOverIcon) {
        RECT currentRect = { 0 };
        GetWindowRect(g_hOverlayWnd, &currentRect);

        if (!EqualRect(&currentRect, &iconRect) || !IsWindowVisible(g_hOverlayWnd)) {
            (void)PositionDropOverlay(iconRect, true);
        }

        // Keep the armed target alive through DragLeave until Drop() or button-up.
        g_dropOverlayArmed.store(true);
        return;
    }

    // Before first contact keep it hidden; once armed, preserve the same HWND for reliable re-entry.
    if (!g_dropOverlayArmed.load() && !g_oleDragActive.load()) {
        HideDropOverlay();
    }
}

static bool RefreshTrayGeometryFromRect(
    HWND hWnd, const RECT& iconRect, bool forceRegeneration) {
    if (!hWnd || !g_iconVisible ||
        iconRect.right <= iconRect.left || iconRect.bottom <= iconRect.top) {
        return false;
    }

    // Position the overlay on the real tray rectangle so GetDpiForWindow() uses that monitor.
    (void)PositionDropOverlay(iconRect, false);

    const UINT dpi = GetDpiForReferenceWindow(g_hOverlayWnd);

    const UINT oldHostDpi = GetDpiForReferenceWindow(hWnd);
    if (oldHostDpi != dpi && ProbeTrayHostDpiAtRect(hWnd, iconRect)) {
        const UINT newHostDpi = GetDpiForReferenceWindow(hWnd);
        Wh_Log(L"DPI: tray host synchronized %u -> %u (tray=%u)",
               oldHostDpi, newHostDpi, dpi);

        if (newHostDpi == dpi) {
            g_pendingDpiShellReinstall = true;
            (void)SetLoggedTimer(
                hWnd, TIMER_DPI_REINSTALL_ID, DPI_REINSTALL_DELAY_MS);
        }
    }

    const int expectedSize = GetSmallIconMetricForDpi(dpi);
    const int slotWidth = iconRect.right - iconRect.left;
    const int slotHeight = iconRect.bottom - iconRect.top;

    // The Shell rectangle is a geometry signal, not a documented HICON target size.
    // Initial Shell geometry establishes a baseline; it isn't itself a layout change.
    const bool renderSizeChanged =
        !g_trayGeometryCache.renderSizeValid ||
        g_trayGeometryCache.renderSize != expectedSize;
    const bool shellGeometryChanged =
        g_trayGeometryCache.hasShellGeometry &&
        (g_trayGeometryCache.slotWidth != slotWidth ||
         g_trayGeometryCache.slotHeight != slotHeight);
    const bool changed = renderSizeChanged || shellGeometryChanged;

    g_trayGeometryCache.renderSize = expectedSize;
    g_trayGeometryCache.slotWidth = slotWidth;
    g_trayGeometryCache.slotHeight = slotHeight;
    g_trayGeometryCache.renderSizeValid = true;
    g_trayGeometryCache.hasShellGeometry = true;

    if (changed) {
        Wh_Log(L"DPI: tray dpi=%u size=%d slot=%dx%d",
               dpi, expectedSize, slotWidth, slotHeight);
    }

    if (changed && forceRegeneration) {
        g_forceIconRegen = true;
    }

    return changed;
}

bool RefreshTrayGeometry(HWND hWnd, bool forceRegeneration) {
    if (!hWnd || !g_iconVisible) {
        return false;
    }

    RECT iconRect = {};
    if (!QueryTrayIconRect(hWnd, iconRect, true)) {
        return false;
    }

    return RefreshTrayGeometryFromRect(hWnd, iconRect, forceRegeneration);
}

const WCHAR* TimerName(UINT timerId) {
    switch (timerId) {
        case TIMER_CLICK_ID: return L"CLICK";
        case TIMER_REFRESH_ID: return L"REFRESH";
        case TIMER_STARTUP_ID: return L"STARTUP";
        case TIMER_DRAG_POLL_ID: return L"DRAG_POLL";
        case TIMER_DPI_CHECK_ID: return L"DPI_CHECK";
        case TIMER_DISPLAY_SETTLE_ID: return L"DISPLAY_SETTLE";
        case TIMER_SHELL_COALESCE_ID: return L"SHELL_COALESCE";
        case TIMER_DPI_REINSTALL_ID: return L"DPI_REINSTALL";
        default: return L"UNKNOWN";
    }
}

bool SetLoggedTimer(HWND hWnd, UINT timerId, UINT intervalMs) {
    const UINT_PTR result = SetTimer(hWnd, timerId, intervalMs, NULL);
    if (result != 0) {
        Wh_Log(L"Timer START %s: %u ms", TimerName(timerId), intervalMs);
        return true;
    }
    Wh_Log(L"Timer START %s FAILED: interval=%u ms error=%lu", TimerName(timerId), intervalMs, GetLastError());
    return false;
}

bool KillLoggedTimer(HWND hWnd, UINT timerId) {
    const BOOL result = KillTimer(hWnd, timerId);
    if (result) {
        Wh_Log(L"Timer STOP %s", TimerName(timerId));
    }
    return result != FALSE;
}

void UpdateRefreshTimer(HWND hWnd) {
    (void)KillLoggedTimer(hWnd, TIMER_REFRESH_ID);

    if (g_settings.fallbackTimerInterval == 0) {
        Wh_Log(L"Timer REFRESH disabled by settings");
        return;
    }

    (void)SetLoggedTimer(hWnd, TIMER_REFRESH_ID, g_settings.fallbackTimerInterval * 1000);
}

void UpdateDpiCheckTimer(HWND hWnd) {
    (void)KillLoggedTimer(hWnd, TIMER_DPI_CHECK_ID);

    if (g_settings.dpiCheckInterval == 0) {
        Wh_Log(L"Timer DPI_CHECK disabled by settings");
        return;
    }

    (void)SetLoggedTimer(hWnd, TIMER_DPI_CHECK_ID, g_settings.dpiCheckInterval * 1000);
}

static void StopShellCoalesceTimer(HWND hWnd) {
    if (!g_trayState.shellCoalesceTimerActive) {
        return;
    }

    (void)KillLoggedTimer(hWnd, TIMER_SHELL_COALESCE_ID);
    g_trayState.shellCoalesceTimerActive = false;
}

static void ArmShellCoalesceTimer(HWND hWnd) {
    if (!g_trayState.shellCoalesceTimerActive) {
        g_trayState.shellCoalesceTimerActive =
            SetLoggedTimer(
                hWnd,
                TIMER_SHELL_COALESCE_ID,
                SHELL_COALESCE_INTERVAL_MS);
        return;
    }

    // SetTimer with the same HWND/ID resets the countdown. Keep re-arms quiet
    // so a bulk Shell event burst doesn't become a log burst.
    if (!SetTimer(
            hWnd,
            TIMER_SHELL_COALESCE_ID,
            SHELL_COALESCE_INTERVAL_MS,
            NULL)) {
        Wh_Log(L"Timer REARM SHELL_COALESCE FAILED: error=%lu",
               GetLastError());
        g_trayState.shellCoalesceTimerActive = false;
    }
}

static void StopDisplaySettleTimer(HWND hWnd) {
    if (g_trayState.displaySettleTimerActive) {
        (void)KillLoggedTimer(hWnd, TIMER_DISPLAY_SETTLE_ID);
        g_trayState.displaySettleTimerActive = false;
    }
    g_trayState.displaySettleHasRect = false;
    g_trayState.displaySettleProbedStableRect = false;
    g_trayState.displaySettleAttempts = 0;
}

static void StartDisplaySettleTimer(HWND hWnd) {
    // Re-arming the timer is the debounce: a burst of display broadcasts must
    // become quiet before we start judging the tray rectangle as stable.
    if (g_trayState.displaySettleTimerActive) {
        (void)KillLoggedTimer(hWnd, TIMER_DISPLAY_SETTLE_ID);
    }

    g_trayState.displaySettleHasRect = false;
    g_trayState.displaySettleProbedStableRect = false;
    g_trayState.displaySettleAttempts = 0;
    InvalidateIconRectCache();

    g_trayState.displaySettleTimerActive =
        SetLoggedTimer(hWnd, TIMER_DISPLAY_SETTLE_ID, DISPLAY_SETTLE_INTERVAL_MS);
}

static void UnregisterShellNotifications() {
    if (!g_shellNotifyLock) {
        return;
    }

    if (!SHChangeNotifyDeregister(g_shellNotifyLock)) {
        Wh_Log(L"Shell notify: deregistration failed for id=%lu",
               g_shellNotifyLock);
    }

    g_shellNotifyLock = 0;
}

static bool RegisterShellNotifications(HWND hWnd) {
    UnregisterShellNotifications();

    PIDLIST_ABSOLUTE pidlBin = NULL;
    const HRESULT hr =
        SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0, NULL, &pidlBin);
    if (FAILED(hr) || !pidlBin) {
        Wh_Log(L"Shell notify: failed to resolve Recycle Bin PIDL: 0x%08lX",
               static_cast<unsigned long>(hr));
        return false;
    }

    // Include child changes so restores trigger promptly.
    SHChangeNotifyEntry entry = { pidlBin, TRUE };
    const ULONG registration = SHChangeNotifyRegister(
        hWnd,
        SHCNRF_InterruptLevel | SHCNRF_ShellLevel,
        SHCNE_ALLEVENTS,
        WM_SHELLNOTIFY,
        1,
        &entry);

    CoTaskMemFree(pidlBin);

    if (!registration) {
        Wh_Log(L"Shell notify: registration failed");
        return false;
    }

    g_shellNotifyLock = registration;
    Wh_Log(L"Shell notify: registered id=%lu", g_shellNotifyLock);
    return true;
}

static void ApplyPendingSettings(HWND hWnd) {
    if (!ConsumePendingSettings()) return;

    InvalidateSystemThemeCache();
    InvalidateFontConfigCache();
    g_forceIconRegen = true;
    UpdateRefreshTimer(hWnd);
    UpdateDpiCheckTimer(hWnd);
    (void)SetDragDropEnabled(hWnd, g_settings.enableDragDrop);
    UpdateTrayState();

    // System and Font rendering do not need GDI+. Release its process-wide
    // state after the new icon has been fully rendered and installed.
    if (g_settings.iconStyle == IconStyle::System ||
        g_settings.iconStyle == IconStyle::Font) {
        ShutdownGdiplusIfInitialized();
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DPICHANGED && g_hostDpiProbeActive) {
        Wh_Log(L"DPI: tray host WM_DPICHANGED during sync dpi=%u x %u",
               LOWORD(wParam), HIWORD(wParam));
        return 0;
    }

    if (g_wmTaskbarCreated && msg == g_wmTaskbarCreated) {
        g_iconVisible = false;
        g_pendingDpiShellReinstall = false;
        (void)KillLoggedTimer(hWnd, TIMER_DPI_REINSTALL_ID);
        InvalidateTrayPositionAndRenderSize();

        // Shell-level change-notification registrations can become stale when
        // Explorer is recreated. Re-establish ours after the Shell settles.
        StopShellCoalesceTimer(hWnd);
        UnregisterShellNotifications();

        // Hide stale drag feedback after an Explorer/taskbar restart.
        HideDropOverlay();

        UpdateTrayState();

        // Retry once Explorer has rebuilt the notification area.
        (void)SetLoggedTimer(hWnd, TIMER_STARTUP_ID, 250);
        return 0;
    }

    switch (msg) {
        case WM_USER_START_DRAG_POLL:
            g_dragGestureSawOle = false;
            g_trayState.dragRectRefreshed = false;
            if (g_settings.enableDragDrop && !g_trayState.dragPollTimerActive) {
                (void)SetLoggedTimer(hWnd, TIMER_DRAG_POLL_ID, DRAG_POLL_INTERVAL_ACTIVE_MS);
                g_trayState.dragPollTimerActive = true;
            }
            return 0;

        case WM_USER_STOP_DRAG_POLL:
            if (g_dropOverlayArmed.load() && !g_dragGestureSawOle) {
                Wh_Log(L"D&D: physical drag ended without OLE DragEnter; source did not start/reach an OLE drag");
            }
            if (g_trayState.dragPollTimerActive) {
                (void)KillLoggedTimer(hWnd, TIMER_DRAG_POLL_ID);
                g_trayState.dragPollTimerActive = false;
            }
            g_trayState.dragRectRefreshed = false;
            // Button-up is final only when OLE is no longer inside the target.
            if (!g_oleDragActive.load()) {
                g_dropOverlayArmed.store(false);
                HideDropOverlay();
            }
            return 0;

        case WM_USER_END_OLE_DRAG: {
            // Drop() posts final cleanup; generation checking rejects stale completions.
            const ULONGLONG generation = static_cast<ULONGLONG>(wParam);
            if (generation != g_oleDragGeneration.load() || g_oleDragActive.load()) {
                return 0;
            }

            g_oleDragActive.store(false);
            g_dropOverlayArmed.store(false);
            g_trayState.dragRectRefreshed = false;
            HideDropOverlay();
            if (g_trayState.dragPollTimerActive) {
                (void)KillLoggedTimer(hWnd, TIMER_DRAG_POLL_ID);
                g_trayState.dragPollTimerActive = false;
            }
            return 0;
        }

        case WM_USER_REFRESH_AFTER_TRAY_ADD:
            // The add path may discover the final tray DPI only after NIM_ADD.
            UpdateTrayState();
            return 0;

        case WM_TRAYICON: {
            // Decode callbacks according to the negotiated notification-icon version.
            const UINT trayMessage = g_trayVersion4 ? LOWORD(lParam) : static_cast<UINT>(lParam);

            if (trayMessage == WM_LBUTTONUP || trayMessage == NIN_SELECT ||
                trayMessage == NIN_KEYSELECT) {
                if (g_ignoreNextLeftUp) {
                    g_ignoreNextLeftUp = false;
                    return 0;
                }
                // If no double-click action is configured, run the single-click action immediately.
                if (g_settings.doubleClickAction == TrayAction::None) {
                    ExecuteAction(g_settings.leftClickAction, hWnd);
                } else {
                    (void)SetLoggedTimer(hWnd, TIMER_CLICK_ID, GetDoubleClickTime());
                }
            } else if (trayMessage == WM_LBUTTONDBLCLK) {
                (void)KillLoggedTimer(hWnd, TIMER_CLICK_ID);
                g_ignoreNextLeftUp = true;
                ExecuteAction(g_settings.doubleClickAction, hWnd);
            } else if (trayMessage == WM_MBUTTONUP) {
                ExecuteAction(g_settings.middleClickAction, hWnd);
            } else if (trayMessage == WM_RBUTTONUP || trayMessage == WM_CONTEXTMENU) {
                ExecuteAction(g_settings.rightClickAction, hWnd);
            }
            return 0;
        }

        case WM_DISPLAYCHANGE:
            StartDisplaySettleTimer(hWnd);
            return 0;

        case WM_USER_TRAY_DPI_CHANGED:
        case WM_DPICHANGED:
            // A real DPI transition means display settling has completed.
            StopDisplaySettleTimer(hWnd);
            InvalidateTrayPositionAndRenderSize();
            (void)RefreshTrayGeometry(hWnd, true);
            g_forceIconRegen = true;
            UpdateTrayState();
            return 0;

        case WM_THEMECHANGED:
            if (g_settings.iconStyle == IconStyle::System) {
                InvalidateSystemThemeCache();
            } else if (RefreshSystemThemeCache()) {
                g_forceIconRegen = true;
                UpdateTrayState();
            }
            return 0;

        case WM_SETTINGCHANGE:
            // Avoid turning every unrelated system-setting broadcast into a
            // theme read. ImmersiveColorSet is the relevant notification.
            if (lParam &&
                wcscmp(reinterpret_cast<LPCWSTR>(lParam), L"ImmersiveColorSet") == 0) {
                if (g_settings.iconStyle == IconStyle::System) {
                    InvalidateSystemThemeCache();
                } else if (RefreshSystemThemeCache()) {
                    g_forceIconRegen = true;
                    UpdateTrayState();
                }
            }
            return 0;

        case WM_TIMER:
            if (wParam == TIMER_CLICK_ID) {
                (void)KillLoggedTimer(hWnd, TIMER_CLICK_ID);
                ExecuteAction(g_settings.leftClickAction, hWnd);
            } else if (wParam == TIMER_REFRESH_ID) {
                // Safety polling also rechecks the theme in case a broadcast was missed.
                if (g_settings.iconStyle != IconStyle::System &&
                    RefreshSystemThemeCache()) {
                    g_forceIconRegen = true;
                }
                UpdateTrayState();
            } else if (wParam == TIMER_STARTUP_ID) {
                bool shellReady = g_shellNotifyLock != 0;
                if (!shellReady) {
                    shellReady = RegisterShellNotifications(hWnd);
                }

                const bool trayReady = UpdateTrayState();
                if (shellReady && trayReady) {
                    (void)KillLoggedTimer(hWnd, TIMER_STARTUP_ID);
                }
            } else if (wParam == TIMER_DRAG_POLL_ID) {
                CheckDragStatus(hWnd);
            } else if (wParam == TIMER_SHELL_COALESCE_ID) {
                StopShellCoalesceTimer(hWnd);
                UpdateTrayState();
            } else if (wParam == TIMER_DPI_REINSTALL_ID) {
                (void)KillLoggedTimer(hWnd, TIMER_DPI_REINSTALL_ID);

                if (!g_pendingDpiShellReinstall) {
                    return 0;
                }

                // Do not disturb an active drag/drop gesture. Retry the one-shot
                // commit once input ownership has returned to the tray thread.
                if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
                    g_oleDragActive.load() || g_dropOverlayArmed.load()) {
                    (void)SetLoggedTimer(
                        hWnd, TIMER_DPI_REINSTALL_ID, DPI_REINSTALL_DELAY_MS);
                    return 0;
                }

                g_pendingDpiShellReinstall = false;

                if (g_iconVisible) {
                    HideDropOverlay();

                    if (Shell_NotifyIconW(NIM_DELETE, &g_nid)) {
                        g_iconVisible = false;
                        g_trayVersion4 = false;
                        InvalidateIconRectCache();
                        g_forceIconRegen = true;

                        Wh_Log(L"DPI: reinstalling tray icon after host DPI settle");
                        if (!UpdateTrayState()) {
                            Wh_Log(L"DPI: delayed tray icon reinstall failed");
                        }
                    } else {
                        Wh_Log(L"DPI: delayed NIM_DELETE failed: %lu",
                               GetLastError());
                    }
                }
            } else if (wParam == TIMER_DPI_CHECK_ID) {
                const bool changed = RefreshTrayGeometry(hWnd, false);
                if (changed) {
                    Wh_Log(L"DPI_CHECK: tray geometry/render size changed; regenerating icon");
                    g_forceIconRegen = true;
                    UpdateTrayState();
                }
            } else if (wParam == TIMER_DISPLAY_SETTLE_ID) {
                ++g_trayState.displaySettleAttempts;

                RECT iconRect = {};
                const bool haveRect = QueryTrayIconRect(hWnd, iconRect, true);

                if (haveRect) {
                    const bool stable =
                        g_trayState.displaySettleHasRect &&
                        EqualRect(&g_trayState.displaySettleLastRect, &iconRect);

                    if (!stable) {
                        g_trayState.displaySettleLastRect = iconRect;
                        g_trayState.displaySettleHasRect = true;
                        g_trayState.displaySettleProbedStableRect = false;
                    } else if (!g_trayState.displaySettleProbedStableRect) {
                        // Probe each stable tray position only once. If the Shell moves it
                        // later, the changed rectangle re-arms the probe automatically.
                        if (ProbeDpiReferenceAtTray(iconRect)) {
                            g_trayState.displaySettleProbedStableRect = true;

                            if (RefreshTrayGeometryFromRect(hWnd, iconRect, false)) {
                                Wh_Log(L"DPI: display settle detected a tray geometry/render-size change; regenerating icon");
                                g_forceIconRegen = true;
                                UpdateTrayState();
                                StopDisplaySettleTimer(hWnd);
                            }
                        }
                    }
                }

                if (g_trayState.displaySettleTimerActive &&
                    g_trayState.displaySettleAttempts >= DISPLAY_SETTLE_MAX_ATTEMPTS) {
                    // One final probe catches a late DPI transition even when the
                    // tray rectangle itself did not visibly move.
                    bool changed = false;
                    if (haveRect && ProbeDpiReferenceAtTray(iconRect)) {
                        changed = RefreshTrayGeometryFromRect(hWnd, iconRect, false);
                    }

                    if (changed) {
                        Wh_Log(L"DPI: display settle final probe detected a tray geometry/render-size change; regenerating icon");
                        g_forceIconRegen = true;
                        UpdateTrayState();
                    } else {
                        Wh_Log(L"DPI: display settle complete; no tray geometry/DPI/icon-size change detected");
                    }

                    StopDisplaySettleTimer(hWnd);
                }
            }
            return 0;

        case WM_SHELLNOTIFY:
            ArmShellCoalesceTimer(hWnd);
            return 0;

        case WM_APPLY_SETTINGS:
            ApplyPendingSettings(hWnd);
            return 0;

        case WM_USER_RECYCLE_WORK_COMPLETE:
            // The worker has released the hidden owner. Complete a shutdown that
            // was deferred while IFileOperation was active.
            if (g_shutdownRequested.load() && CanCloseTrayHost()) {
                (void)PostMessageW(hWnd, WM_CLOSE, 0, 0);
            }
            return 0;

        case WM_USER_SHUTDOWN:
            // EndMenu affects the calling thread's active menu, so it must run
            // here on the tray thread rather than in WhTool_ModUninit().
            if (EndMenu()) {
                Wh_Log(L"Shutdown: active tray menu cancelled");
            }

            // Shell APIs can pump this message from nested modal loops. Do not
            // destroy their owner while the call is still on the stack.
            if (!CanCloseTrayHost()) {
                Wh_Log(L"Shutdown: deferred while Shell UI or recycle worker is active");
                return 0;
            }

            // Queue normal teardown after a nested TrackPopupMenuEx loop has
            // had a chance to unwind.
            (void)PostMessageW(hWnd, WM_CLOSE, 0, 0);
            return 0;

        case WM_CLOSE:
            if (!CanCloseTrayHost()) {
                Wh_Log(L"Shutdown: WM_CLOSE deferred while Shell UI or recycle worker is active");
                return 0;
            }
            (void)DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            InterlockedExchangePointer((PVOID volatile*)&g_hWnd, NULL);

            // Stop input tracking and release the OLE target before tray teardown.
            (void)SetDragDropEnabled(hWnd, false);

            (void)KillLoggedTimer(hWnd, TIMER_CLICK_ID);
            (void)KillLoggedTimer(hWnd, TIMER_REFRESH_ID);
            (void)KillLoggedTimer(hWnd, TIMER_STARTUP_ID);
            (void)KillLoggedTimer(hWnd, TIMER_DRAG_POLL_ID);
            (void)KillLoggedTimer(hWnd, TIMER_DPI_CHECK_ID);
            StopShellCoalesceTimer(hWnd);
            (void)KillLoggedTimer(hWnd, TIMER_DISPLAY_SETTLE_ID);
            (void)KillLoggedTimer(hWnd, TIMER_DPI_REINSTALL_ID);
            g_pendingDpiShellReinstall = false;
            g_trayState.displaySettleTimerActive = false;
            g_ignoreNextLeftUp = false;
            g_oleDragActive.store(false);
            g_dropOverlayArmed.store(false);

            UnregisterShellNotifications();
            if (g_iconVisible) {
                (void)Shell_NotifyIconW(NIM_DELETE, &g_nid);
                g_iconVisible = false;
            }
            g_hCurrentIcon.reset();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI TrayThreadProc(LPVOID lpParam) {
    const HINSTANCE hInstance = g_hThisModule;
    ThreadDpiAwarenessGuard dpiAwareness;
    if (!dpiAwareness) {
        Wh_Log(L"DPI: failed to enable Per-Monitor V2 awareness: %lu", GetLastError());
    }

    OleInitGuard oleInit;
    if (!oleInit) {
        Wh_Log(L"D&D: OleInitialize failed: 0x%08X", oleInit.GetResult());
        return 0;
    }
    Wh_Log(L"D&D: OLE initialized.");

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TRAY_WINDOW_CLASS;
    RegisterClassW(&wc);

    g_wmTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");

    // Use a real hidden top-level owner for broadcasts and tray UI.
    // Layered alpha keeps DPI synchronization visually hidden.
    HWND hWndNew = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        TRAY_WINDOW_CLASS,
        L"WindhawkRecycleTrayWindow",
        WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, hInstance, NULL);
    if (!hWndNew) {
        Wh_Log(L"Tray: Failed to create host window: %lu", GetLastError());
        return 0; // OleInitGuard handles OleUninitialize().
    }

    (void)SetLayeredWindowAttributes(hWndNew, 0, 1, LWA_ALPHA);

    // SHChangeNotify can originate in the medium-integrity Shell while this
    // tool process is elevated. Allow only our Shell callback through UIPI.
    if (!ChangeWindowMessageFilterEx(
            hWndNew, WM_SHELLNOTIFY, MSGFLT_ALLOW, nullptr)) {
        Wh_Log(L"Shell notify: ChangeWindowMessageFilterEx failed: %lu",
               GetLastError());
    }

    // Explorer can broadcast TaskbarCreated from a lower-integrity process.
    // Allow only this registered restart notification through UIPI.
    if (g_wmTaskbarCreated &&
        !ChangeWindowMessageFilterEx(
            hWndNew, g_wmTaskbarCreated, MSGFLT_ALLOW, nullptr)) {
        Wh_Log(L"Tray: TaskbarCreated message filter failed: %lu",
               GetLastError());
    }

    InterlockedExchangePointer((PVOID volatile*)&g_hWnd, (PVOID)hWndNew);

    if (g_shutdownRequested.load()) {
        (void)DestroyWindow(hWndNew);
        (void)UnregisterClassW(TRAY_WINDOW_CLASS, hInstance);
        return 0;
    }

    // Apply the newest settings snapshot if it arrived while the window was starting.
    (void)ConsumePendingSettings();

    // Keep a transparent physical window as the per-monitor DPI reference.
    WNDCLASSW wcOverlay = {0};
    wcOverlay.style = CS_DBLCLKS;
    wcOverlay.lpfnWndProc = OverlayWndProc;
    wcOverlay.hInstance = hInstance;
    wcOverlay.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcOverlay.lpszClassName = DROP_OVERLAY_CLASS;
    RegisterClassW(&wcOverlay);

    g_hOverlayWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        DROP_OVERLAY_CLASS, L"", WS_POPUP,
        0, 0, 0, 0, NULL, NULL, hInstance, NULL
    );

    if (g_hOverlayWnd) {
        Wh_Log(L"DPI: reference overlay created (HWND: 0x%p)", g_hOverlayWnd);
        SetLayeredWindowAttributes(g_hOverlayWnd, 0, 1, LWA_ALPHA);

        // Allow OLE drag/drop data messages from a lower-integrity Shell.
        constexpr UINT kWmCopyGlobalData = 0x0049;
        const UINT dragDropMessages[] = { WM_DROPFILES, WM_COPYDATA, kWmCopyGlobalData };
        for (UINT message : dragDropMessages) {
            if (!ChangeWindowMessageFilterEx(
                    g_hOverlayWnd, message, MSGFLT_ALLOW, nullptr)) {
                Wh_Log(L"D&D: message filter failed for 0x%04X: %lu",
                       message, GetLastError());
            }
        }
    } else {
        Wh_Log(L"DPI: reference overlay creation failed: %lu", GetLastError());
    }

    g_nid.cbSize = sizeof(NOTIFYICONDATAW);
    g_nid.hWnd = hWndNew;
    g_nid.uID = TRAY_ICON_ID;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.uVersion = NOTIFYICON_VERSION_4;

    const bool shellReady = RegisterShellNotifications(hWndNew);

    UpdateRefreshTimer(hWndNew);
    UpdateDpiCheckTimer(hWndNew);
    (void)SetDragDropEnabled(hWndNew, g_settings.enableDragDrop);
    const bool trayReady = UpdateTrayState();

    // A healthy startup needs no retry timer. Keep it only for Shell/tray recovery.
    if (!shellReady || !trayReady) {
        (void)SetLoggedTimer(hWndNew, TIMER_STARTUP_ID, 1000);
    }

    MSG msg;
    for (;;) {
        const BOOL result = GetMessageW(&msg, NULL, 0, 0);
        if (result <= 0) {
            if (result == -1) {
                Wh_Log(L"GetMessageW failed: %lu", GetLastError());
            }
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_iconVisible) {
        (void)Shell_NotifyIconW(NIM_DELETE, &g_nid);
        g_iconVisible = false;
    }

    // Fallback cleanup if the message loop exited unexpectedly.
    UnregisterShellNotifications();

    // Fallback cleanup if WM_DESTROY wasn't reached.
    (void)SetDragDropEnabled(hWndNew, false);
    g_hCurrentIcon.reset();

    if (g_hOverlayWnd) {
        DestroyWindow(g_hOverlayWnd);
        g_hOverlayWnd = NULL;
    }

    UnregisterClassW(DROP_OVERLAY_CLASS, hInstance);
    UnregisterClassW(TRAY_WINDOW_CLASS, hInstance);
    
    return 0; // OleInitGuard handles OleUninitialize().
}

// Windhawk tool callbacks

BOOL WhTool_ModInit() {
    g_shutdownRequested.store(false);

    if (!InitializeThisModuleHandle()) {
        Wh_Log(L"Failed to resolve the mod module handle: %lu", GetLastError());
        return FALSE;
    }

    LoadSettings();

    if (!InitializeRecycleWorker()) {
        return FALSE;
    }

    // Dedicated UI/tray thread.
    g_hThread = CreateThread(NULL, 0, TrayThreadProc, NULL, 0, NULL);
    if (!g_hThread) {
        Wh_Log(L"CreateThread failed");
        RequestRecycleWorkerStop();
        (void)WaitForSingleObject(g_hRecycleWorkerThread, INFINITE);
        CloseHandle(g_hRecycleWorkerThread);
        g_hRecycleWorkerThread = NULL;
        CloseHandle(g_hRecycleWorkerEvent);
        g_hRecycleWorkerEvent = NULL;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_shutdownRequested.load()) return;

    auto newSettings = std::make_unique<ModSettings>();
    LoadSettingsInto(*newSettings);
    QueuePendingSettings(std::move(newSettings));

    HWND hWnd = GetSafeHwnd();
    if (hWnd && IsWindow(hWnd) &&
        !PostMessageW(hWnd, WM_APPLY_SETTINGS, 0, 0)) {
        Wh_Log(L"Failed to post settings update: %lu", GetLastError());
    }
}

void WhTool_ModUninit() {
    g_shutdownRequested.store(true);
    RequestRecycleWorkerStop();

    HWND hWnd = GetSafeHwnd();
    if (hWnd) {
        (void)PostMessageW(hWnd, WM_USER_SHUTDOWN, 0, 0);
    }

    HANDLE waitHandles[2] = {};
    DWORD waitCount = 0;
    if (g_hThread) {
        waitHandles[waitCount++] = g_hThread;
    }
    if (g_hRecycleWorkerThread) {
        waitHandles[waitCount++] = g_hRecycleWorkerThread;
    }

    if (waitCount != 0) {
        const DWORD waitResult = WaitForMultipleObjects(
            waitCount, waitHandles, TRUE, TOOL_THREAD_SHUTDOWN_TIMEOUT_MS);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Shutdown: tool threads did not exit within %lu ms; process exit will complete teardown",
                   TOOL_THREAD_SHUTDOWN_TIMEOUT_MS);

            if (g_hThread) {
                CloseHandle(g_hThread);
                g_hThread = NULL;
            }
            if (g_hRecycleWorkerThread) {
                CloseHandle(g_hRecycleWorkerThread);
                g_hRecycleWorkerThread = NULL;
            }
            return;
        }
        if (waitResult == WAIT_FAILED) {
            Wh_Log(L"Shutdown: tool thread wait failed: %lu; process exit will complete teardown",
                   GetLastError());

            if (g_hThread) {
                CloseHandle(g_hThread);
                g_hThread = NULL;
            }
            if (g_hRecycleWorkerThread) {
                CloseHandle(g_hRecycleWorkerThread);
                g_hRecycleWorkerThread = NULL;
            }
            return;
        }
    }

    if (g_hThread) {
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
    if (g_hRecycleWorkerThread) {
        CloseHandle(g_hRecycleWorkerThread);
        g_hRecycleWorkerThread = NULL;
    }
    if (g_hRecycleWorkerEvent) {
        CloseHandle(g_hRecycleWorkerEvent);
        g_hRecycleWorkerEvent = NULL;
    }

    delete TakePendingRecycleWorkerJob();
    delete TakePendingSettings();

    ShutdownGdiplusIfInitialized();
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

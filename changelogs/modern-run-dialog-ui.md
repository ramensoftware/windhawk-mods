## 4.0.101 ([Nov 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fee6d20e88618018c299d1d4bf18008a916798c8/mods/modern-run-dialog-ui.wh.cpp))

* Added manual Run dialog size and font customization: adjust dialog width (`360`–`1200 px`), command bar/total height (`36`–`96 px` / `150`–`600 px`), and font size (`9`–`24 pt`) with live preview via inline `-` / `[value]` / `+` controls in the in-app `⚙️` quick settings menu or Windhawk settings.
* Enhanced **Liquid Glass** optical rendering (3D Snell's-law edge refraction, chromatic dispersion, and adjustable tint from `-100` to `100`) and inline numeric controls in the `⚙️` quick settings menu.
* Added instant web search prefixes (`ggsearch:` for Google in Chrome/default browser and `msesearch:` for Microsoft Edge) alongside real-time app suggestions and full 21-language localization across both the Run dialog and `⚙️` quick settings menu.

## 4.0.2 ([Nov 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a22894f34de93ae33481c9ca3f29cf294832a9d9/mods/modern-run-dialog-ui.wh.cpp))

- Fixed the Run dialog not being intercepted on Windows 11 Build 26300.9818+ (#6712) by matching the Run dialog resource ID (`1003`) with a `shell32.dll` dialog procedure in `DialogBoxParamW` (supporting localized `.dll.mui` module handles) and a thread-safe `ShowWindow` fallback (no `shell32.pdb` download required).
- Added an in-app quick settings gear button (`⚙️`) on the command bar with a custom Direct2D/DirectWrite Fluent Acrylic popup menu.
- Added a **Minimal Mode** option (toggleable directly from the main `⚙️` menu or mod settings) that hides the **Browse** and **Exit** buttons while keeping the standard dialog width and expanding the command input box.
- Added support for custom static images (`.png`, `.jpg`, `.webp`, `.bmp`), animated `.gif`, and `.mp4`/`.webm` video backgrounds (up to 50 MB) with adjustable media opacity.
- Added `ggsearch:<query>` (Google Chrome / default browser) and `msesearch:<query>` (Microsoft Edge) quick web search commands.
- Expanded command history storage up to 50 items with a 5-row scrollable viewport, mouse wheel support, and a draggable Direct2D scrollbar.
- Added UI scale options (`85%` – `120%`), single-click Taskbar / `Win + R` foreground restore, and fixed `GUID_NULL` linker compatibility with Clang 20 (`-luuid`).

## 3.0.6 ([Oct 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1b66e28eee1f1299f896cfa1bd3d8c738210e99b/mods/modern-run-dialog-ui.wh.cpp))

Fixed an issue on Windows 11 build 26300.9807+ where the classic Run dialog appeared when shell32.pdb symbols were unavailable on Microsoft's symbol server (HTTP 404), by adding an automatic symbol-less hooking fallback (DialogBoxParamW, DialogBoxIndirectParamW, CreateDialogParamW, CreateDialogIndirectParamW, and ShowWindow) (#6544).

Added experimental PowerToys Run-style app and command suggestions while typing (enableAppSuggestions), indexing Start Menu shortcuts, Registry App Paths, built-in system tools, and recent history with lazy icon loading and keyboard/mouse navigation (Up/Down, Tab to autocomplete, Enter or click to launch).

Restored the distinct high-contrast colors and Direct2D vector icons for the 3 bottom action buttons (Browse with folder icon, Run in Fluent Blue with return arrow icon, and Exit in dark crimson with close icon) across all surface themes including Liquid Glass, with dynamic button width adaptation for localized labels.

Added 10 new display languages (totaling 21 languages): Thai (th), Indonesian (id), Malay (ms), Portuguese (pt), Ukrainian (uk), Polish (pl), Dutch (nl), Turkish (tr), Hindi (hi), and Arabic (ar).

## 3.0.4 ([Oct 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/75402edd191fb26b9987dc4b0d60bf49288224ee/mods/modern-run-dialog-ui.wh.cpp))

- Added full compatibility support for both **Windows 10** and **Windows 11** (`x86-64`), anchoring the Run dialog flush above the Taskbar regardless of history item count.
- Added **Liquid Glass** surface theme featuring dual refractive rim strokes, pointer-following specular highlights, glass pill buttons, and automatic 16×16 background luminance contrast adaptation (alongside **Windows Blur** and **Opaque** themes).
- Added **Auto Light / Dark Mode** support (`Auto`, `Dark`, `Light`) and **Italian (`it`)** localization.
- Upgraded the rendering engine to **Direct2D & DirectWrite** (`IDWriteTextLayout`) for crisp `Segoe UI` typography and uniform multilingual diacritics scaling.
- Updated history list interaction: clicking a history item now executes the command immediately instead of only filling the input box.
- Fixed **"Max History Items"** (expanded range to `0–10`) and **"Show Last Executed Time"** not working when Windows `RunMRU` tracking is disabled, isolated stored history per Windows user (`HistoryTimestamps_<SID>`), added non-destructive history clearing (`HiddenRunMru_<SID>` & `Delete` key shortcut) without deleting Windows' native `RunMRU` entries, and hid the clock icon when timestamps are turned off (#6467).

## 2.0.15 ([Oct 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d66466c25b0eeda4428a0294b2726ec94b9c4276/mods/modern-run-dialog-ui.wh.cpp))

Initial release.

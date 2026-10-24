## 2.0 ([Oct 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/45217a613b9826883069ffed9129215b7851c0ce/mods/start-everything.wh.cpp))

* Move search into its own Windows 11 window. Win+S, the taskbar search icon, Start’s search bar, and typing in Start open it while Start remains available normally.
* Copy Start’s size, alignment, background, rounded corners, and supported Start Menu Styler appearance. Copy supported static composition effects with their actual tint, blur, and color adjustments; retain their hook dependencies safely. Apply context-menu and submenu styling before they appear, with fixed menu corner radii.
* Preserve captured glass across updates and off/on cycles using validated effect definitions in this mod's Windhawk storage, scoped to the current Start process and matching brush identities. Keep live caches bounded and non-owning, invalidate reused identities and unsupported replacements, and leave Start's brush lifecycle and properties unchanged.
* Add 50 bundled left-to-right interface language options, automatic Windows display-language detection, and a manual language override. Translate search, context menus, preview labels, keyboard hints, and utility messages; keep Windows-provided app and Settings names in their native language.
* Add suggested apps and custom commands for empty searches, with both using the same matching and favorite-learning system.
* Add custom commands with CMD, Windows PowerShell, PowerShell 7, or a custom shell; optional hidden terminals; and emoji, Windows glyph, image, icon, or executable icons.
* Add an optional global search shortcut with modifier and key dropdowns, custom menu width and height, and a Files-panel toggle that also pauses Everything queries.
* Add automatic text and monochrome-icon contrast using one cached background map, plus a fixed text-color override. Pause background sampling during animations, scrolling, and dragging.
* Switch immediately when typing in Start and buffer opening input. Replay IME opening keys through Windows native input after the editable text context is ready, and improve focus recovery. Typing or Backspace closes context menus and continues the query; Escape closes the menu before clearing search. Preview clicks dismiss context menus so preview details remain copyable.
* Fetch previews on a dedicated background thread, with built-in Windows image decoding, PDF rendering, SVG rasterization, and bounded animation caches. Use native muted, looping video playback without a separate helper process. Add more copyable preview details and Copy name for files and folders.
* Animate preview height changes smoothly. With a bottom taskbar, short previews align at the top and tall previews at the bottom; top and side taskbars keep previews top-aligned and growing downward.
* Improve native shell drag input and drag-image tracking, and resolve optional pointer conversion by name. Keep drag and drop copy-only and suppress previews throughout the drag.
* Discover Windows Settings names and search keywords from Windows’ local database and navigation metadata. Cache results in memory, refresh changed catalogs in the background, retain additional Windows tools, and fall back to the built-in list when native data is unavailable. Prefer specific Settings names over broad keywords and avoid repeated Settings subtitles.
* Add postfix percentages such as `10% × 7`, mathematical symbols (`×`, `·`, `÷`, `−`, `π`, `√`), and decimal-comma calculator input. Keep binary remainder and existing English expressions compatible, support symbol-based unit conversions, reject malformed numeric tokens, and bound input length and recursion.
* Support opening and closing animations for top, left, and right taskbars. Keep these animations within the selected monitor and place the preview to the left for a right-side taskbar.
* Defer search UI creation until first use to avoid startup crashes, and remove obsolete code from the earlier in-Start design.
* Complete asynchronous XAML cleanup before the search UI thread exits, so Taskbar Styler can continue applying themes after mod updates or reloads.
* Organize settings into collapsible sections and remove Search Panel Margin. Existing 1.x preferences must be reapplied in the new sections; custom-command and unit-conversion keys are unchanged.
* Use typed material hooks, query protected brush interfaces from their live base objects, and remove development diagnostics from the submitted mod.
* Update the README and screenshot, including compatibility notes for the tested Windows 11 25H2 and 26H2 builds, language selection, symbol-based utility input, and theme setup after installation or updates.

* Fix Ctrl+V in Start so search opens immediately and receives the clipboard text.

## 1.1 ([Oct 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/32987307cbb3a539e5936bd080e412b2e4d000e5/mods/start-everything.wh.cpp))

* File preview: the selected file gets a card beside the Start menu with a large thumbnail (pictures, a frame of a video, documents) and its details: type, size, dates, and per kind dimensions, length, artist and album, camera, a program's version, a document's title and author. Videos and animated GIF and WebP images play in it, muted; a PDF turns through its first pages; SVG images are drawn by Windows' own renderer; text and code files show their first lines, and Word, Excel, PowerPoint and RTF documents their first paragraphs, read by Windows' own document readers (no Office needed). A folder shows how many items it holds and, when Everything keeps folder sizes, its size. A click on the path copies it. Can be turned off in the settings.
* Drag and drop: drag a file or folder from the results into File Explorer, the desktop or another app, as you would from File Explorer. It is always a copy, never a move.
* Uninstall in an app's right-click menu, after a confirmation: a Store app is removed for the current user, and a program runs its own uninstaller, the one Settings > Installed apps runs. Not offered for apps that are part of Windows. Store apps are removed in the background, without a progress bar.
* Calculator and unit converter without a command: type a sum (`2^10`), a number with a unit (`100 km`), a conversion (`100 km to mi`) or another base (`255 hex`, `0xFF`) and the result shows above the apps. `/c` is gone. Built-in units for length, mass, temperature, speed, area, volume, data, time, pressure, energy, power, angle and frequency, converting both ways, with `.` or `,` as the decimal point; several conversions on one line; scientific notation for large results; binary digits grouped; a color per kind; long results wrap. Custom conversions can convert back (Reciprocal). (#5988, #5989, #5992, #5993, #5994, #5995, #5996, #5998, #5999, thanks @Telezhka-the-First)
* New Text Size setting for the search panel (#6000).
* New Search Panel Margin setting. The panel is now edge to edge by default (fixes #5970, thanks @Josh65-2201).
* Everything 1.5 alpha ("1.5a") and other named Everything instances are now found; they showed "Everything (not running)".
* A Start menu shortcut to a website is no longer shown as a web search.

## 1.0 ([Oct 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d2f3636c817d15b6a8c61f16b67ad7d9652adeb1/mods/start-everything.wh.cpp))

Initial release.

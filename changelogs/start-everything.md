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

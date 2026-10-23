## 0.7.0 ([Oct 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6fcd3d85b84e3caf6c8ff1add265132f272a0735/mods/spotlight-launcher.wh.cpp))

* Added a **Files** mode: browse drives and folders in the launcher with location chips, a path bar, folder history, an actions menu (rename, copy, new folder, terminal, properties, Recycle Bin), filters and image thumbnails.
* Added **deep search** in the current folder and all its subfolders, with results appearing while the background scan runs.
* Added **web search** results with a configurable engine (Google, Bing, DuckDuckGo, YouTube, Wikipedia) and `?`, `g:`, `b:`, `d:`, `y:`, `w:` prefixes.
* Added **Windows Settings** results for common settings pages.
* Added an **F1 help sheet** with all shortcuts, prefixes and file filters, plus a second page documenting the calculator syntax.
* Added the **Open on monitor** setting (monitor with the mouse, active window or primary).
* Added the **File browser** setting to turn Files mode off; the folder button is hidden and Tab goes back to the app list.
* Grouped the settings into **Appearance**, **Behavior**, **Shortcuts**, **Apps**, **Features** and **Sounds**.
* Improved Files performance: metadata is built lazily, very large folders are capped at 5,000 folders and 5,000 files, and image previews decode in the background (network and cloud placeholders are skipped).
* **Live acrylic** now also drives Glass in real time; **Blur strength** applies to Mica and, while Live acrylic is off, to the other effects.
* Removed **Open with** from the Files actions menu.
* Fixed the bar closing after deleting a file or folder, and the acrylic backdrop lingering briefly after the bar is hidden.

## 0.6.0 ([Oct 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c3497a2ea65f5c7e110ce53dcc52de7012342217/mods/spotlight-launcher.wh.cpp))

* Added a built-in calculator: type an expression in the search box and the result appears as a highlighted card and/or faintly inline. Enter copies it, and recent results are kept in a reusable history.
* Added soft interface sounds for opening the launcher, opening/closing the details panel, and typing. Everything is generated inside the mod, so it works offline.
* Added a "Live acrylic (real time)" setting: Acrylic, Frosted and Aurora now follow the background in real time, and Glass keeps refreshing its blur. It can be turned off to use a single snapshot taken on open, which is lighter on the CPU and battery.
* The details panel background now animates together with the panel as it opens and closes.
* Softened the tint of the Acrylic, Frosted, Aurora and Mica effects.
* The Glass effect now follows the "Live acrylic" setting (single static capture when it is disabled).
* Rewrote every setting description and the README in clearer, plain language.

## 0.5.11 ([Oct 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/002d01efc7998c0c2c45cd7f0a44650aef7f3955/mods/spotlight-launcher.wh.cpp))

Initial release.

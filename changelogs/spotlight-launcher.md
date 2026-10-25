## 0.7.1 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/29d1380647b00ebd4049d838db77d2773779aad1/mods/spotlight-launcher.wh.cpp))

* **A chip for every drive.** File mode now shows a chip (with a disk icon) for each of your drives on a **second row**, kept apart from the location chips so the two groups do not read as one list. A chip is added for every fixed and removable drive that is ready; empty card readers, optical drives with no disc and network drives are skipped, so nothing ever raises a "there is no disk" dialog. The row appears only when you have **two or more** drives (with a single drive there is nothing to switch to). The chips are detected automatically, no setup.
* **A "This PC" drives view.** Going up from a drive root — **←** or **Backspace** on an empty box, or **Alt + ↑** — now opens a list of all your drives instead of doing nothing, so you can move between drives with the arrows alone. **Enter** or **→** opens the highlighted drive; each row shows its label and free space.
* **Location chips follow moved folders.** The Home, Downloads, Documents, Pictures, Music and Videos chips now resolve to each folder's real location, so they still work when a folder has been moved to another drive or redirected (for example a `OneDrive`-backed Documents).
* **Switch drives with Ctrl + Tab.** `Ctrl + Tab` cycles forward through the drive chips and `Ctrl + Shift + Tab` backward (the six location chips are left out, since they are a separate idea). `Ctrl + 1…9` now jumps to any chip — locations first, then drives — and typing a path such as `D:\` in the path bar still works.
* **Start drive setting.** A new **Start drive** option (a single drive letter) chooses which drive the file browser opens on the first time you use it in a session. Empty — or a drive that is not available, such as an unplugged USB stick — opens on Home, as before.
* The F1 cheat sheet and the README shortcut tables list the new keys.
* **The F1 help sheet no longer resets the bar.** It now overlays the current view and closing it returns exactly where you were — the same folder and selection in the file browser, or the calculation you were working on — instead of starting over as if the launcher had just opened.

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

## 1.0.0 ([Oct 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/baa44b7b054e315d6b522775ecd1bb5a13594546/mods/spotlight-launcher.wh.cpp))

* **Clipboard history.** An opt-in feature (off by default) that keeps the last 30 things you copied
  and adds a `clip:` prefix to search them. **Enter** pastes the selected entry into the window you
  came from, **Ctrl + Enter** copies it, **Ctrl + D** pins it, **Ctrl + Shift + R** removes it, and
  the **Clear Clipboard History** command erases everything. Stored locally per user.
* **Images in the clipboard history.** Copied pictures are kept too: each one is saved as a
  compressed PNG inside the mod's own folder (scaled down to 2560 px on its long side when bigger),
  shows its own thumbnail in the list, and pastes back as an image. The file name is a hash of the
  pixels, so copying the same picture twice reuses one file and one entry. `clip: image` narrows the
  list to pictures.
* **Clipboard retention.** A **Clipboard retention (days)** setting (3 by default) deletes entries
  older than that, deleting their image files with them; pinned entries never expire, and `0` keeps
  entries until the 30-entry limit drops them. Files with no entry are cleaned up on load.
* **Pinned clipboard entries.** Pinned entries stay at the top of `clip:` and survive both the
  30-entry trim and the retention.
* **Recent files.** The empty bar gains a **Recent files** section with the files and folders from
  Windows' Recent list, and `recent:` searches them. Enter opens, Ctrl+Enter opens the folder,
  Ctrl+Shift+C copies the path. Read locally from the Recent folder; can be turned off in the
  settings. Windows must be recording recent items (Settings → Personalization → Start), or the list
  stays empty.
* **Global file search.** `find:` searches file and folder names under Home (Documents, Downloads,
  Desktop, Pictures and the rest) and any extra folders configured in the settings. It runs in a
  worker thread, shows partial results while scanning, stops when the bar closes, and builds no
  index. Enter opens, Ctrl+Enter opens the folder, Ctrl+Shift+C copies the path.
* **Window switcher.** `win:` lists the open windows with their title and process; **Enter** activates
  the selected one (restoring it if minimized). Launching a classic desktop app that already has a
  visible window now focuses it instead of starting a second instance; launches with arguments and
  packaged apps keep their normal behaviour.
* **Coloured feedback.** Anything the launcher has to tell you now carries a colour and a small dot
  in the footer: green when something is done (**Copied**), amber when it had to work around a problem
  (**Everything not running · built-in scan**) and red for what needs care (a destructive command
  asking for a second Enter, a path that does not exist, a preview that could not be opened). The
  empty list uses the same language: the accent while it is still searching, amber when a problem is
  the reason it is empty.
* **Everything as an optional index.** With **File search engine** set to **Everything** (voidtools),
  both `find:` and the deep search inside Files go to Everything's IPC instead of scanning: every
  drive (or just the folder you are browsing), answers in milliseconds, and Everything's own syntax
  (`*.pdf`, `"exact phrase"`, `!excluded`). No DLL is shipped or loaded, the built-in scan stays the
  default, and it takes over automatically when Everything is not running or does not answer within a
  second, with the footer saying *Everything not running · built-in scan*.
* **Sort the Files list.** **Ctrl + S** (or **Sort…** in the actions menu) opens a sort list with the
  order in use marked: name (A-Z and Z-A), type, modified date (newest and oldest) and size (largest
  and smallest). Folders stay on top, the order also applies while filtering, and deep-search results
  keep their relevance order. **Remember file sort** (off by default) keeps the chosen order across
  closes and restarts; while it is off the order only lives while the bar is open and Files starts
  again at Name (A-Z).
* **Learning from your choices.** Picking an app after typing a few words puts that app first the next
  time you type those same words — even when it was a weak match (`ex` + **Firefox** puts Firefox
  above Excel). One pick is enough, only those words are affected, the pairs live in the mod storage
  (capped at 200, forgotten after 90 days without a pick), and **Learn from your choices** in the
  settings turns it off.
* **Modes as results.** Typing the name of a mode (for example **Open Windows** — also found by
  *windows manager* or *switcher* — **Bookmarks**, **Clipboard History**, **Recent Files**, **All
  Files** or **Commands**) shows it as one more result, like an app; **Enter** switches into that mode
  and fills in its prefix, so the prefixes are optional. A mode row only appears while its feature is
  enabled, and it never filters: `win: chrome` is still how a list is narrowed.
* **The Settings tab is grouped by what each option configures**: Appearance, Behavior, Shortcuts,
  Apps, Search (web search, bookmarks, recent files, learning and the file search engine), Files
  (browser, start drive, remembered sort), Clipboard, Calculator and Sounds. Options that moved
  between sections keep their saved values; the tab shows the defaults until you save it once, so it
  is worth a look the first time.
* **The F1 sheet** lists every shortcut, prefix and filter (including the newer ones: sort, preview,
  pin, the clipboard keys and the mode names) and scrolls with the wheel or the arrow keys when a
  page is taller than the bar.
* **Feedback has a colour**: green when something is done, amber when a problem had to be worked
  around and red for what needs care, with a small dot in the footer and the same tones in the empty
  list and the error bands.
* Disabled features no longer claim their prefixes: `bm:`, `clip:` and the web prefixes (`?`, `g:`,
  `b:`, `d:`, `y:`, `w:`) are only recognized while the matching feature is enabled; otherwise the
  text is searched as a normal query.

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

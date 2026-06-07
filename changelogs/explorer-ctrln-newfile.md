## 1.1 ([Jun 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8b1683a4ec3660d5dbc7340248764ee68d40c45b/mods/explorer-ctrln-newfile.wh.cpp))

* Support the active tab in tabbed Explorer windows.
* Run as a Windhawk tool mod outside of explorer.exe.
* Resolve the active folder through Explorer's Shell view instead of scraping address bar text with UI Automation.
* Keep the low-level keyboard hook thread responsive by running file creation and rename work on a separate STA worker thread.
* Use Shell file operation fallback for protected folders, allowing Windows to show the standard elevation prompt when needed.
* Improve rename reliability by using the matched active Shell view first.

## 1.0 ([Nov 23, 2025](https://github.com/ramensoftware/windhawk-mods/blob/b39425a15084dc10756c51355b79623d8d59b596/mods/explorer-ctrln-newfile.wh.cpp))

Initial release.

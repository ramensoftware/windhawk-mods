## 2.0.1 ([Oct 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1b77d44584c94c1be861fc3745528cca2e13133b/mods/reopen-programs-at-sign-in.wh.cpp))

* Fixed: nothing was reopened in Windhawk portable mode, because the mod storage drops line breaks. The saved list now uses a separator that survives (the old one is still read).
* Fixed: programs with a background process but no window were treated as already open and skipped. The check now compares windows, also right before each launch.
* Windhawk's own programs (tray app, 2.0 tool-mod hosts, editor) are no longer saved or reopened.
* README: documents the already-open check and the helper-process limitation.

## 2.0.0 ([Oct 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9da2ffcc4366a0e801ca4b4ec0474cf668771490/mods/reopen-programs-at-sign-in.wh.cpp))

Initial release.

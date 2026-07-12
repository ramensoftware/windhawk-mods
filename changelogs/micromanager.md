## 1.1.0 ([Jul 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1588c9290e6c51a476e1ce0e1b399a0055d5e5ce/mods/micromanager.wh.cpp))

- **Fixed:** Tooltip now displays correctly when hovering the tray icon.
- **Fixed:** Ghost window prevention — popup no longer flickers on rapid open/close.
- **Fixed:** Data delay recovery — processes that take time to report usage are retried with exponential backoff.
- **Fixed:** Safe mod reload — icon and window clean up properly without crashing.
- **Improved:** Tray tooltip updates only when values change, reducing unnecessary CPU work.
- **Fixed:** Popup window reference cleaned up on destroy.

## 1.0.0 ([Jun 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7f6a9d164aae1027702ac4603ff218928a53661b/mods/micromanager.wh.cpp))

Initial release.

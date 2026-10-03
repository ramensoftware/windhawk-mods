## 1.2.0 ([Oct 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/503d101f1c5e6abf8d57e0058bb2d49617237f33/mods/micromanager.wh.cpp))

* **Live Activity Graphs:** Added a 16-sample history graph to the popup. Click any row or the graph canvas to cycle between live CPU, GPU, and RAM trends.
* **Process Tree Termination:** "End Task" now terminates the entire process tree, cleanly closing multi-process apps (Firefox, Chrome, Electron) without leaving orphaned crashed tabs.
* **Open File Location:** Added a quick action to reveal and select the top consumer's executable in File Explorer.
* **Safety & Identity Protection:** Confirms termination targets, validates process creation times against PID reuse, and blocks critical Windows system processes.
* **High-Contrast Readability:** Pure white text across popup labels, graph titles, and context menus for excellent readability with dark and custom themes.
* **Top Taskbar Placement:** Automatically detects top-aligned taskbars and flips the popup below the tray icon so it never renders off-screen.
* **System Process Display:** Correctly identifies and labels "System Idle Process" and the NT Kernel (PID 4) when they consume resources.
* **Tray & Navigation Fixes:** Fixed popup flicker on rapid tray clicks, added keyboard navigation (Up / Down / Esc), and improved reload stability.

## 1.1.0 ([Jul 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1588c9290e6c51a476e1ce0e1b399a0055d5e5ce/mods/micromanager.wh.cpp))

- **Fixed:** Tooltip now displays correctly when hovering the tray icon.
- **Fixed:** Ghost window prevention — popup no longer flickers on rapid open/close.
- **Fixed:** Data delay recovery — processes that take time to report usage are retried with exponential backoff.
- **Fixed:** Safe mod reload — icon and window clean up properly without crashing.
- **Improved:** Tray tooltip updates only when values change, reducing unnecessary CPU work.
- **Fixed:** Popup window reference cleaned up on destroy.

## 1.0.0 ([Jun 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7f6a9d164aae1027702ac4603ff218928a53661b/mods/micromanager.wh.cpp))

Initial release.

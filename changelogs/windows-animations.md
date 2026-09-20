## 1.3.5 ([Sep 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9b0af5f09f130af8d7f59d90f4133bc0e019588e/mods/windows-animations.wh.cpp))

* **Added smart hybrid GPU acceleration:** Restores and compatible launches can use DirectComposition GPU rendering, while qualifying large 1 px Thanos/Perlin closes can use GPU rendering. Minimize, ordinary close, Square Shatter, and unsupported cases use the faster or safer optimized CPU path.
* **Improved performance:** Optimized Perlin, Shatter, and Thanos CPU effects, reduced temporary memory usage, right-sized rendering surfaces, added refresh-aware frame pacing, and safely pre-warmed only the GPU effects likely to be used next.
* **Improved minimize, restore, and launch animations:** Fixed content flicker, blank or invisible windows, cursor stalls, incorrect taskbar targets, and rapid-click/reversal problems. Non-maximized animations now preserve the Windows 11 rounded-window silhouette.
* **Added taskbar support on every edge:** Genie and Windows 10 animations now target bottom, top, left, and right taskbars on primary or secondary monitors, including auto-hidden taskbars. Windows Terminal targeting was also improved.
* **Added the optional Optimize Show Desktop mode:** Win+D custom-animates only the foreground window while background windows minimize immediately, avoiding long sequential waits. Matching background restores stay native, while the foreground restore keeps its custom animation without duplicate windows or pre-animation flashes.
* **Reworked random styles into true shuffle:** Close and minimize/restore animations use separate session-wide shuffle cycles shared by all apps. Every style plays once before reshuffling, consecutive cycles do not repeat the same boundary style, and launch/minimize plus rapid reversal pairings stay visually consistent.
* **Improved Alt+Tab:** The switch effect now runs only for genuine Alt+Tab switches and no longer leaves File Explorer headers or backdrop areas black, opaque, or incorrectly styled.
* **Improved close and tray-hide behavior:** Long animations can finish fully, close-time flicker was reduced, console windows animate correctly, and GPU resources no longer keep closed Chrome or Task Manager processes alive.
* **Improved compatibility and stability:** Added safer automatic CPU fallback, better handling for File Explorer/backdrop windows and Chromium/Electron apps, support for windows already open when the mod is re-enabled, and protection against whole-screen animations after sleep or resume. Background-only helper processes are skipped.
* **Added optional performance telemetry:** When enabled, one compact summary per animation reports renderer choice, adapter, timing, frame coverage, memory, pacing, and CPU rendering breakdown. Normal logging remains quiet when telemetry is disabled.
* **Updated the README and settings:** Added a clear explanation of the hybrid renderer, performance guidance, GPU routing and fallback behavior, Show Desktop behavior, taskbar placement, rounded corners, and shuffle semantics.

## 1.2.0 ([Aug 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3d753806f59713ff1824c20cb24b6414e006ce03/mods/windows-animations.wh.cpp))

* **Windows 10 Style:** Added the `Windows 10` (Collapse-to-taskbar) minimize/restore animation style. Perfect for those who love the clean, classic look of Windows 10 but with the flawless execution of our new rendering engine.
* **Random Effects Mode:** Can't decide on just one style? You can now enable the **Random** option to let the mod surprise you with a different effect every time you Close, Minimize, or Restore a window.
* **Flawless "Spam Click" Handling:** The biggest upgrade of this release! By introducing the `NativeMinimizeBarrier` and `AsyncRestoreAnimData` reservation system, the mod can now gracefully queue and synchronize overlapping requests. Whether you double-click or rapidly spam clicks on the taskbar icon, the minimize/restore animations will flow seamlessly without stuttering or skipping frames.
* **Pinpoint Taskbar Positioning:** The taskbar icon tracking algorithm has been completely overhauled. Using window set signature hashing combined with Explorer UI events, windows will now accurately suck right into their exact icon position on the taskbar with perfect cache invalidation.
* **Alt+Tab Optimization:** The mod no longer continuously polls global keys in the background. Alt+Tab session tracking has been refactored to use an `ExplorerFgHookThread`, saving CPU resources and providing smarter, more reliable responsiveness.
* Improved window uncloaking reliability, ensuring the correct Z-order is maintained and complex visual backdrops (like DWM Mica/Acrylic -- Translucent Windows mod) are properly restored.
* Fixed a focus management bug that occurred when temporarily borrowing taskbar focus to reveal an auto-hidden taskbar during the **Genie** effect.
* Migrated all API hooking and string setting retrievals to use standard `WindhawkUtils`, resulting in a cleaner and more maintainable codebase.

## 1.1.7 ([Aug 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9c9fb82e9c7935004d0325e723d0f14dae3f631e/mods/windows-animations.wh.cpp))

* Enhanced minimize and restore animations with five new fluid effects: Scorch, Splinter, Mirage, Stipple, and Swell.
* Updated Cyber Glitch animation for a more visually appealing and realistic effect.
* Added two new close animations: Retro TV Off and Pixel Melt, expanding the cinematic effects available.
* Updated compiler options for improved compatibility.
* Updated Readme and Settings
* Modified handling of deferred window show commands in animation logic for improved behavior.

## 1.1.5 ([Aug 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fcf4752975ee79f29179fa66057e216ba4f52ff5/mods/windows-animations.wh.cpp))

Initial release.

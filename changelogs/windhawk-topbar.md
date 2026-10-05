## 1.3.0 ([Oct 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3a25447184fb292f91b386490c355dd55658d2fb/mods/windhawk-topbar.wh.cpp))

### Added
* Added a Media Player widget in TopBar with song name, thumbnail, visualizer, and media controls.
* Added a Control Panel button & flyout to quickly access Wi-Fi, Bluetooth, Battery, Sound, Brightness, and other basic settings.
* Added a Start menu flyout featuring All Apps grid, account settings, and power controls.
* Added a Spotlight style Search flyout supporting Apps, Files, Settings, and Control Panel results.
* Default Start and Search key remapping: Win key and Taskbar Start button map to TopBar Start Menu; Win + S and Taskbar Search icon map to TopBar Spotlight Search (BETA, may have some bugs).
* Added Per-Monitor brightness control in display flyout.
* Added Night Light button in the Display flyout.
* Added a "Restart Explorer" button in the Start and Taskbar context menus to restart windows `explorer.exe` easily.
* Added Glyphs to all context menu entries.
* Added Font color setting that changes to set custom font color.
* Added Wi-Fi signal-strength indication on the its icon.
* Added Option to customize button Resource Button labels(e.g., "CPU:", "RAM:").
* Added more font family options in the font dropdown *(Note: Certain fonts may affect button scaling)*.
* Added Midnight Neon theme

### Updates & Improvements
* Added native background blur, removed dependency on the wallpaper layer, and added live wallpaper support.
* Icons now feature transparent backgrounds (previously black) and shrink dynamically when it is out of space.
* Added support for switching between °C and °F units.
* Updated Recycle Bin icon, and reduced Battery icon glyph size.
* Updated the OS27 GoldenGate, NoIslands, and GreenBar themes.

### Fixes
* Fixed issue where right-clicking task list buttons showed the incorrect menu.
* Fixed button flashing on hover in the OS27 Golden Gate theme.(made the background be the same on all the CommonStates by if the value is set without any state.

## 1.2.0 ([Sep 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d0ec196399e1da4244bdf8c73dc36fe04e291ecd/mods/windhawk-topbar.wh.cpp))

Additions:
* Added Weather Widget showing Temperature, Hourly weather, Rain probability, and a location selector.
* Added Support for WindhawkBlur for better custom Blurred backgrounds, applied to the top bar, flyouts, and context menus. Blur amount can be adjusted from settings.
* Added option to move TopBar widgets Left, Right, or Center of the TopBar (Accessible through the UI Alignment tab in the mod's settings app).
* Added Alternative for Tasklist: Application Name box showing the name of the active running application.
* Added Glyphs for Resource Monitor Items.
* Styling support was improved.
* Added Theme: OS27 GoldenGate.

Updates:
* Settings are now shifted to a separate mod settings app (Accessible through the settings icon in the top bar OR through context menus).
* Updated Themes: GreenBar, and NoIslands.

## 1.1.0 ([Sep 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/50b1da24769a21b15ee5293e97d03a04e98f4499/mods/windhawk-topbar.wh.cpp))

* Add Resource Monitor to show CPU, GPU, and RAM Usage on the topbar.
* Fixed Rough Corners for the flyouts, and context menus.(Note: uses DWM to round corners which is Windows 11 exclusive)
* Added option for moving or rearranging the TrayPanel(right panel) items.
* Made Battery Icon look better
* Fixed Win+D(Show Desktop) hides the TopBar.
* Fixed Bluetooth panel not finding new devices.
* Updated themes: NoIslands, and GreenBar.

## 1.0.0 ([Sep 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8dcad63f573216788738142dc36cb982536d970a/mods/windhawk-topbar.wh.cpp))

Initial release.

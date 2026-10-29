## 1.4.2 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fb4b682804d2a506db5efda8d58bf5159580a812/mods/windhawk-topbar.wh.cpp))

**Addition:**

* Added Start Menu & Search settings for hiding pinned apps, adding folders, and support for different start menu & search menu's display alignment(top, center, bottom, etc).
* Added support for setting the TopBar height directly through control styles.
* Added Neon Blue and Neon Blue (Island) themes.

**Updates:**
* Updated the Start menu with pinned apps, several other ui changes, optimized its initial loading time, and improved the styling support.
* Improved the Top Bar Search by adding more searches, Quick controls directly through search results(dark mode, night light, power plan, etc) without opening any app.
* Fixed the TopBar appearing in fullscreen apps.
* Fixed Island mode (Width=Auto) causing the Start Menu and Search flyouts to misalign and block mouse inputs.
* Improved the Navigation Panel UI in Settings app.
* Updated the Media Player flyout to remove unnecessary top padding.
* Updated theme: Midnight Neon - remove the Width=Auto causing it to be a island.
* Improved the Styling support
* Other minor updates.

## 1.4.1 ([Oct 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cc988a903bbf7aad82025e31a485561824478fa2/mods/windhawk-topbar.wh.cpp))

**Addition:**
* Added a Shortcuts panel in the Settings app to assign custom keyboard shortcuts for flyouts, Start, Search, and Restart Explorer, including support for overriding Windows default keys.
* **Added Titlebar Bleed:** When a window is maximized, the TopBar now samples the maximized window's titlebar gradient and displays it behind the bar's content.

**Updates:**
* Fixed the Top Bar not hiding in full screen window.
* Fixed the Auto-Hide animation causing the Top Bar to misalign to the top-left.
* Fixed TopBar popup is misaligned at top left, and is rendering small when secondary is set to Vertical Alignment.
* Fixed the DWM backdrop shadow appearing when Top Bar hides in auto hide mode.(made it less noticeable)
* Updated the Reload action to temporarily switch to Default mode before reloading and switching back to Auto-Hide, fixing a mouse input bug.
* Topbar now force-restarts on resolution, and display scale changes to avoid misalignment of topbar.
* Added popup misalignment detection. if the bar gets misaligned, the topbar restarts the thread fixing it.
* Updated the Notification flyout to hide the "Show full notification" button for short messages.
* Fixed activation pill not re-centering after resolution or scale changes.
* Now, setting Width=Auto for `Grid#TopBarRoot` makes combine removes space between Left, Right, and Center panel and topbar looks like a dynamic island instead of a Bar.
* Fixed GreenBar theme not applying custom colors to WinUI3 icons.

## 1.4.0 ([Oct 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fbb8e30010d661ed001161637cd38e8365b7f7c3/mods/windhawk-topbar.wh.cpp))

**Addition:**
* Added a New Auto Hide mode that hides the Top bar when moving cursor out of the grid. It can be shown by either Activation pill, OR by hovering on edge of the screen.
* Added a Tray Button & Flyout to show tray icons.
* Added New Flyouts Animations Setting to customize Flyouts opening and closing animation.
* Added a Notification Button & Flyout to show Notifications.
* Added a Calendar flyout date and time button.
* Added a Thumbnail Preview for TaskList buttons, featuring a set of app control buttons (Pin, Minimize, Maximize, Close).
* Added more controls in Control Centre flyout.
* Added new Win UI Icon Pack to Top bar.

**Updates:**
* Updated Weather Flyout to show weekly weather.
* Made the background in Settings app, have hand-built Mica effect.
* Progress bar in Media player flyout can now rewind the playing music.
* Made graph animations in Resource flyout smoother.
* Updated the thumbnail icon in Media player button to show squared rounded album art instead of small thumbnail maintaining ascpect ratio of the image.
* Improved Styling support for Top Bar.
* Other Minor UI updates.
* Updated All the themes.

**Fixes:**
* Fix the UWP apps such as Calculator, and some other apps like Windows Settings were not showing icons in tasklist.
* Fixed Top Bar Flickering in secondary monitor.
* Fixed Top Bar wrong display scalings when switching between monitors.

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

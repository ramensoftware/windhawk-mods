## 2.1 ([Aug 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/524f3dd8faa26727a0e788c82ed797a7eb2daa44/mods/simple-window-switcher.wh.cpp))

### Fixes
- Fixed an issue where the alt-tab interface flashed at the top-left of the screen on launch for single-monitor setups by utilizing absolute negative off-screen coordinates
- Fixed an issue where child popup windows or dialogs overshadowed their root parent windows in the switcher by correctly prioritizing `GW_OWNER`
- Fixed the "Scroll Pages" layout engine wrapping issue to ensure strictly computed page boundaries and alignments without incorrectly pulling from the start of the list
- Fixed the layout and spacing of overflow indicators so they no longer stick flush against the absolute screen edges
- Fixed corner scaling to properly support custom, rounded, or default corner preferences in the switcher background (Credits to @bropines)
- Fixed mirror switcher cleanup to prevent resource leaks and overlap bugs on multi-monitor setups (Credits to @bropines)
- Fixed an issue where only File Explorer tabs would close instead of the entire window by safely utilizing native `SC_CLOSE` commands (Credits to @bropines)

### Updates
- Updated the title fetching logic to prioritize native `GetWindowTextW` fallback for better title extraction and consistency
- Updated the static `...` overflow indicator into dynamic directional chevrons (`▲`, `▼`, `◀`, `▶`) depending on the layout orientation
- Updated keyboard shortcut handling to natively support `Ctrl+W` and `Del` for closing the selected window
- Updated default value for `Virutal Desktop Behavior` to show windows from all virtual displays

### Additions
- Added a new setting `sortMinimizedWindowsToEnd` to prevent game windows that minimize on focus loss from being incorrectly sorted to the end of the list (Credits to @bropines)
- Added `Alt+Backtick` shortcut support with a configurable setting to cycle backwards or filter exclusively to windows of the same active application
- Added a "Show close button" (`showCloseButton`) setting toggle under Appearance -> Thumbnails to easily hide the X button
- Added advanced scroll wheel customization options (Scroll Wheel Action & Secondary Modifiers) to allow seamless toggle between selection scrolling and full-page scrolling

## 2.0 ([Jun 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e5909b71ad7f7fa9b7abdafe7c1008f801efed41/mods/simple-window-switcher.wh.cpp))

### Fixes
- Fixed centering for elements. Now elements like Title and Label are correctly centered when DWM Thumbnails is disabled
- Fixed UWP Icons display for Windows 10 and older builds (Credits to @Asteski)
- Fixed Background Opacity fallback issue where setting it to 0 or above 100 falls back to 90. Now they follow the range correctly
- Fixed layout issues for icon-only setup
- Fixed scroll navigation issue where it doesn't navigate when the cursor is out of the alt tab menu
- Fixed an issue where special apps that uses overlays like Raycast, WindowSill etc were caught in the alt tab menu. 
- Fixed an issue with vertical alignment configuration where wide thumbnails "cut-through" the space. 
- Fixed an edge case with `Display Windows Only From the Monitor Containing the Cursor` option (Credits to @zhilemann)
- Fixed minor thumbnail issues. Now it correctly follows the DWM state for the windows
- Fixed row width to correctly follow the layout (Credits to @Asteski)
- Fixed Mica theme multi-monitor bug

### Updates
- Updated Explorer Restart Prompt logic using a new mutex check
- Updated Settings Layout to be more structured
- Updated Switcher Display options into a drop-down menu
- Updated Icons Size configuration, has four options: (Credits to @Asteski)
  - Small
  - Medium
  - Large
  - Extra Large
- Update Theme Mode Settings to allow a modular configuration separate for both light and dark modes
- Updated close button glyph to stay in static color (Credits to @Asteski)

### Additions
- Added new options: 
  - Show Title 
  - Show Label 
  - Reverse Scroll Direction
  - Switcher Background Color
- Added Virtual Desktop Support
- Added Font Customizations
- Added four new features (Credits to @Asteski) 
  - Thumbnail Position
  - Thumbnail Alignment
  - Task Highlight Style
  - Thumbnails to Task Width
  - Grouping Applications
- Added a new theme: Mica (Only Supported for Windows 11)
- Added Exclude Windows support
- Added Custom Border Radius support
- Added Switcher Positioning support
- Added Automatic Icon Size depending on the switcher dimensions (Credits to @Asteski)
- Added middle-mouse button task close (Credits to @Asteski)
- Added a new layout: Bagde, inspired from macOS with a set of customization options
- Added Grouper Window Indicator to show the number of windows of the related process
- Added Thumbnail Shadow Support (Credits to @Asteski)

## 1.1 ([May 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/666743da4e145c838ffbbf89db8f15aa8c7915da/mods/simple-window-switcher.wh.cpp))

- Refactor Explorer Restart Prompt logic
- Fixed an issue where the Show Delay setting is broken, [as it just freezes the switcher thread and blocks the message loop](https://github.com/Louis047/windhawk-mods/blob/c0b6582ad2161f8406e294b2a74639851923cfac/mods/simple-window-switcher.wh.cpp#L1510) Fix by [@zhilemann](https://github.com/zhilemann)

## 1.0 ([May 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2da6ee87b56eb42c1802b43f0c201e5e5d3149aa/mods/simple-window-switcher.wh.cpp))

Initial release.

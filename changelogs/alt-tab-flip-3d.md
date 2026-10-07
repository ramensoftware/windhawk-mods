## 1.6.0 ([Oct 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/843d28cb7ca4d6d1b96b32b117820a12131dd087/mods/alt-tab-flip-3d.wh.cpp))

* Alt+Tab is taken over reliably next to tools that inject keys, like AutoHotkey scripts: keys injected by other programs no longer end or block a session (#5945).
* New "Shortcut" setting: Alt+Tab, Win+Tab, or both (#5946).
* Ctrl+Alt+Tab opens the switcher and keeps it open after the keys are released (can be turned off) (#5947).
* New "Title position" setting: bottom of the screen, or right below the selected window (#5948).
* The blurred wallpaper background no longer turns black when the wallpaper can't be read (Windows Spotlight, HEIC/AVIF files, a deleted file): Windows' own copy of the wallpaper is used, and the desktop shows through if there's none.
* Middle-click a window to close it, or click the X shown on the window under the mouse (both can be turned off). The closed window shrinks away and the others slide into place.
* New "Window size" and "Depth" settings, to bring the windows closer or push them farther back.
* Bars and docks such as YASB and WindowSill are left out of the switcher, and an "Excluded apps" list leaves out any other app.

## 1.5.3 ([Oct 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0d68d720b3eecf432b1b80445a6efb0afcb50de8/mods/alt-tab-flip-3d.wh.cpp))

Initial release.

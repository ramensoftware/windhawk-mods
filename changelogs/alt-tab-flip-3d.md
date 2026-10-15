## 1.7.1 ([Oct 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b2813a1c76395983542ac7cbbb8561f99e7c6912/mods/alt-tab-flip-3d.wh.cpp))

* New "Extra shortcuts" list, each item with a style of its own: Win+Ctrl+Tab, which keeps the switcher open like Ctrl+Alt+Tab (the original Flip 3D shortcut; off by default), and Ctrl+Alt+Tab, to give it a style of its own (#5946, #5947). Win+Ctrl+Tab also works when a program running as administrator is in front.
* New "Show windows hidden by window managers" setting: windows that tiling window managers such as komorebi hide on their other workspaces can be shown, like in the native Alt+Tab, and picking one brings its workspace back (#6040).
* New "Virtual desktops" setting: follows the Windows setting for Alt+Tab by default, or shows only the current desktop or all of them. Picking a window on another desktop switches to it. Store apps (Settings, Calculator) on other desktops or minimized are listed once.
* Four new "iTab" styles: Cover Flow on a glossy floor, with the windows reflected in it, the title and the app's name centered under them, and a scroll bar. On a black floor of their own or on the Background setting, with the app's name or with the title only.
* New "Live desktop" background: the real desktop behind the stack, with animated wallpapers (Wallpaper Engine, Lively Wallpaper) still playing.
* New "Screen refresh rate + 20 FPS" frame rate option, per screen.
* Frames are evenly paced on every screen. Before, some PCs drew 2-3 times more frames than the screen shows, or unevenly with screens of different refresh rates.
* An open switcher at rest (Ctrl+Alt+Tab or Win+Ctrl+Tab left open) no longer draws anything, and a quick Alt+Tab does less work.
* README: a new picture with every style, and a Contact section.

## 1.7.0 ([Oct 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8a2982e08b1096b64c08a9623a90b5c94bd9555e/mods/alt-tab-flip-3d.wh.cpp))

* Minimized windows that sometimes stayed minimized when picked, like console windows (cmd), are now restored reliably (#5955).
* After a graphics driver update or reset, the switcher starts over by itself. Before, Alt+Tab could stop responding until the mod was restarted.
* On laptops with two GPUs, the switcher draws on the GPU the screen is connected to, so frames aren't copied between GPUs (#5956, #5982).
* New "Frame rate limit" and "Live window content" settings, to make the switcher lighter on slower PCs and laptops (#5956).
* No more double image while the switcher closes: the chosen window comes up right away, behind the switcher, and the desktop is uncovered only at the end of the animation (#5982).
* With both Alt+Tab and Win+Tab on, Win+Tab can use its own animation style.
* Four new styles: Cube, Sphere, Shuffle and Domino.
* The wallpaper background works with live wallpaper apps like Wallpaper Engine, which mark the background as a solid black color.
* Setting descriptions show their default values, and the middle-click setting explains what it does.

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

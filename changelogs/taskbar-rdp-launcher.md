## 1.3.0 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/72291cdf01bb95ba90b95e01c9a46317ddd63903/mods/taskbar-rdp-launcher.wh.cpp))

* The widget is part of the taskbar's own layout now, which makes room for it, rather than a window placed over it. The new **Position** setting puts it left of the notification area, of the Wi-Fi, volume and battery icons, or of the clock, or at the left end of the taskbar. It's no longer dragged.
* With Taskbar Snipper or Taskbar Audio Flipper in the same position, the widgets sit side by side: **Move left** and **Move right** in the right-click menu change their order. They replace **Link widgets**.
* The display map is a XAML flyout, opened by hovering a number and closed once the mouse moves off it and the number, and the menu is Windows' own XAML menu.
* The computer's name and address are as wide as their text, without a divider.
* The mod needs Windows 11 now. The first time it starts, Windhawk downloads the taskbar's symbols.

## 1.2.0 ([Oct 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ec7de6631f7bbf765e62e8643055e93324817e4b/mods/taskbar-rdp-launcher.wh.cpp))

* The widget is inside the taskbar now: the mod is loaded into Explorer instead of running in a windhawk.exe process of its own. It moves and auto-hides with the taskbar, and full-screen apps and the Start menu cover it as they do the rest of the taskbar.
* Update linked widgets together (Taskbar Audio Flipper and Taskbar RDP Launcher 1.2, Taskbar Snipper 1.1): older versions don't look for widgets inside the taskbar, so they overlap them and don't follow them.

## 1.1.0 ([Oct 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f6432c021584bf08d3ee6629ab47de33ecd7452a/mods/taskbar-rdp-launcher.wh.cpp))

* The widget links with up to two others now: Taskbar Audio Flipper and the new Taskbar Snipper. **Link widgets** in the right-click menu has **Link with** for each of them, and **Move this widget left** and **right** to put them in order. A link made with Taskbar Audio Flipper in 1.0 carries over.
* Fluent icons from the Windows icon font (Segoe Fluent Icons, or Segoe MDL2 Assets on Windows 10), like the taskbar's own: This PC for the computer, a monitor with the number on its screen for each display button, and a check mark in the display picker.
* The readme shows the three widgets linked, with links to each mod.

## 1.0.0 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3c8dad353a8a45fa414a8a27e7ac46a0f403d48c/mods/taskbar-rdp-launcher.wh.cpp))

Initial release.

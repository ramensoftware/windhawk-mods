## 1.4.0 ([Nov 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/66b700c6645c8bcbc1b793d74a78d5d206ce2782/mods/taskbar-snipper.wh.cpp))

* **Position** in the right-click menu moves the buttons, like the **Position** setting; whichever you changed last is used.
* At the left end of the taskbar, the buttons go right of the Widgets button instead of over it.

## 1.3.0 ([Nov 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/38463a071b68590c96a5416dcb0ce84eb0ef3e2e/mods/taskbar-snipper.wh.cpp))

* The buttons' right-click menu has **Screenshot button** and **Record button**, to show or hide each one (one of them stays), and **Swap the buttons**, to put the record button first.
* **Which buttons** in the mod's settings has a new **Record and screenshot** option. A choice made in the menu is used until the setting is changed, as with the save folders.
* **Change icons…** in the menu gives the buttons other glyphs from the Windows icon font (screenshot, record and stop recording). Choose an icon, then a glyph, or type its code; **Default** puts its own back. The same picker shows the icons of Taskbar Audio Flipper and Taskbar RDP Launcher when they're running.

## 1.2.0 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3e2422875c4232c20bb802f54806a8c81b71f58e/mods/taskbar-snipper.wh.cpp))

* The screenshot and record buttons are part of the taskbar's own layout now, which makes room for them, rather than a window placed over it. The new **Position** setting puts them left of the notification area, of the Wi-Fi, volume and battery icons, or of the clock, or at the left end of the taskbar. They're no longer dragged.
* With Taskbar Audio Flipper or Taskbar RDP Launcher in the same position, the widgets sit side by side: **Move left** and **Move right** in the right-click menu change their order. They replace **Link widgets**.
* The menu is Windows' own XAML menu.
* The mod needs Windows 11 now. The first time it starts, Windhawk downloads the taskbar's symbols.

## 1.1.0 ([Oct 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/063bc430d57a0fe909326b5f36f5b7f562b2314f/mods/taskbar-snipper.wh.cpp))

* The widget is inside the taskbar now: the mod is loaded into Explorer instead of running in a windhawk.exe process of its own. It moves and auto-hides with the taskbar, and full-screen apps and the Start menu cover it as they do the rest of the taskbar.
* Update linked widgets together (Taskbar Audio Flipper and Taskbar RDP Launcher 1.2, Taskbar Snipper 1.1): older versions don't look for widgets inside the taskbar, so they overlap them and don't follow them.

## 1.0.0 ([Oct 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/737ac0bd8a791ba491e35dadcda50eda00c1d3e2/mods/taskbar-snipper.wh.cpp))

Initial release.

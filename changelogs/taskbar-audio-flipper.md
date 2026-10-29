## 1.3.0 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/dd4a1eb36146f43188ef7f099ec9ef0cc7387d03/mods/taskbar-audio-flipper.wh.cpp))

* The widget is part of the taskbar's own layout now, which makes room for it, rather than a window placed over it. The new **Position** setting puts it left of the notification area, of the Wi-Fi, volume and battery icons, or of the clock, or at the left end of the taskbar. It's no longer dragged.
* With Taskbar Snipper or Taskbar RDP Launcher in the same position, the widgets sit side by side: **Move left** and **Move right** in the right-click menu change their order. They replace **Link widgets**.
* The output's volume flyout has a volume mixer, like Windows': each app using the output, with its icon (click it to mute the app) and its volume, a gear for Windows' volume mixer settings, and **More volume settings**. The input's has **More sound settings**.
* The flyouts and menus are Windows' own XAML ones, and the flyout closes once the mouse moves off it and its section.
* Each section is as wide as its text, without dividers.
* The mod needs Windows 11 now. The first time it starts, Windhawk downloads the taskbar's symbols.

## 1.2.0 ([Oct 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/fa94df932028eee4def35698ec8a469ac68a030b/mods/taskbar-audio-flipper.wh.cpp))

* The widget is inside the taskbar now: the mod is loaded into Explorer instead of running in a windhawk.exe process of its own. It moves and auto-hides with the taskbar, and full-screen apps and the Start menu cover it as they do the rest of the taskbar.
* Update linked widgets together (Taskbar Audio Flipper and Taskbar RDP Launcher 1.2, Taskbar Snipper 1.1): older versions don't look for widgets inside the taskbar, so they overlap them and don't follow them.

## 1.1.0 ([Oct 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0559a0e550149979f1896abf7be76c006b5594b7/mods/taskbar-audio-flipper.wh.cpp))

* Devices are shown with a Fluent icon for their kind (speakers, headphones, headset, monitor or microphone) in the taskbar's color, like the rest of Windows 11, struck through while muted. The webcam's icon is a Fluent glyph too. The icons chosen in the Sound control panel are no longer shown, and **Rename or change device icons…** is now **Rename devices…**.
* The widget links with up to two others now: Taskbar RDP Launcher and the new Taskbar Snipper. **Link widgets** in the right-click menu has **Link with** for each of them, and **Move this widget left** and **right** to put them in order. A link made with Taskbar RDP Launcher in 1.0 carries over.
* The readme shows the three widgets linked, with links to each mod.

## 1.0.0 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/50db4c4d6c2dd9c6822ad676c7787c78cd5aba4f/mods/taskbar-audio-flipper.wh.cpp))

Initial release.

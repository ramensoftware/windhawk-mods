## 1.1.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2e774e506b47ba68e529a535f0ed50ac9b4d7152/mods/background-dimmer.wh.cpp))

* Taskbar, Start, Alt+Tab and notification center now count as focused: while one of them has focus, every regular window is dimmed. Previously they left the layer where it was, so the last active window stayed lit behind them.
* A dialog now can dim the window that owns it (enabled by default). The new option "Dim a window while one of its dialogs has focus" can be turned off to keep them lit together, as in 1.0.0.
* New option "Dim everything behind an open menu" (off by default). It works with standard Windows menus; apps that draw their own menus aren't detected. Without it, a menu keeps the window that opened it lit.
* "Hide the layer from screenshots and screen sharing" is now on by default. Existing users keep their saved choice.
* Optional tray icon (new "Tray icon" setting, off by default): click to open the configuration window, right-click to toggle dimming on and off.
* Patterns simplified to Solid, Checkerboard, Stripes and Image, with a new Rotation slider for stripes and checkerboard (any angle, antialiased). Saved horizontal, vertical and diagonal stripes are converted to Stripes at 0°, 90° and 45°.

## 1.0.0 ([Nov 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a7de0d897193602ab4c4fa4a8c35bf549f1e9999/mods/background-dimmer.wh.cpp))

Initial release.

## 1.2.0 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c8f4d8fc48cf117a61fc76d342b32288b99579db/mods/background-dimmer.wh.cpp))

* **Per-monitor dimming:** configuration window lists the connected monitors under Active; unchecked ones stay always lit. 
* **Improved menu vs dialog detection:** new logic based of title bars. Windows with a title bar are treated as dialogs; small windows without one, like Office menus or Chrome extension panels, are treated as menus.
* **Fade:** the window that loses focus dims gradually, including when a dialog opens or Alt+Tab, Start, the taskbar or the desktop takes focus.
* **Taskbar dimming** (off by default): a click-through layer over each taskbar, hidden while the taskbar, Start or Alt+Tab has focus.
* **Image / Custom pattern** replaces Image: a Scale setting (1–400%) takes the place of Size, and brightness, contrast and base color apply to the image like they do to the other patterns. 
* **Tray icon click-action swap:** click toggles dimming, right-click opens the configuration window.
* **New default configuration hotkey:** Win+Ctrl+Shift+D, to match with possible Win+Shift+D on/off toggle shortcut.

## 1.1.1 ([Nov 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4448165feff7f0a5beb2b4865643312557e0431d/mods/background-dimmer.wh.cpp))

* Hotkeys now need Win plus at least one of Ctrl, Alt or Shift, so a hotkey can't take over typing keys (Shift+letter) or the Win+letter keys Windows uses. Saved hotkeys that don't follow this stop working and must be set again.

## 1.1.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2e774e506b47ba68e529a535f0ed50ac9b4d7152/mods/background-dimmer.wh.cpp))

* Taskbar, Start, Alt+Tab and notification center now count as focused: while one of them has focus, every regular window is dimmed. Previously they left the layer where it was, so the last active window stayed lit behind them.
* A dialog now can dim the window that owns it (enabled by default). The new option "Dim a window while one of its dialogs has focus" can be turned off to keep them lit together, as in 1.0.0.
* New option "Dim everything behind an open menu" (off by default). It works with standard Windows menus; apps that draw their own menus aren't detected. Without it, a menu keeps the window that opened it lit.
* "Hide the layer from screenshots and screen sharing" is now on by default. Existing users keep their saved choice.
* Optional tray icon (new "Tray icon" setting, off by default): click to open the configuration window, right-click to toggle dimming on and off.
* Patterns simplified to Solid, Checkerboard, Stripes and Image, with a new Rotation slider for stripes and checkerboard (any angle, antialiased). Saved horizontal, vertical and diagonal stripes are converted to Stripes at 0°, 90° and 45°.

## 1.0.0 ([Nov 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a7de0d897193602ab4c4fa4a8c35bf549f1e9999/mods/background-dimmer.wh.cpp))

Initial release.

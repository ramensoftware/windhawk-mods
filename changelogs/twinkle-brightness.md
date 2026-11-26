## 1.2.0 ([Nov 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e54cd77948e634febaae4ccb6647c9080b546712/mods/twinkle-brightness.wh.cpp))

* Type an exact level: double-click a display's level in the panel, or press F2 or Enter on the focused row.
* The tray icon shows the brightness level: the sun's rays light up like a gauge. It can be turned off in the settings.
* New setting for the position of the on-screen indicator: the top or the bottom of the screen, centered or in a corner.
* Tooltips for the buttons at the bottom of the panel.
* Turkish translation of the panel, the menus and the settings.
* Brightness changes made outside the mod are picked up. Laptop screens report them right away, and monitors are read again before a hotkey step.
* Fixed: some monitors showed "Brightness control is not available" until the displays were refreshed by hand. DDC/CI reads are retried, and monitors that don't answer are detected again, up to three times in the first minute.
* Fixed: scrolling with a touchpad in the panel changed the brightness much too fast.

## 1.1.0 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6ec92a920503db975426e6489bb3537a44b534fd/mods/twinkle-brightness.wh.cpp))

Initial release.

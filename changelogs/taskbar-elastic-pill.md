## 2.0.0 ([Jul 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9f9f096a896bdd73f8af47bb284efb01b29e2e50/mods/taskbar-elastic-pill.wh.cpp))

* Added elastic intensity setting
* Added squish multiplier setting
* Added fade duration multiplier setting
* Added hover/pressed interactions
* Added dedicated opacity setting

## 1.2.0 ([Jul 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1531df78bb6b21bdc2951ee4f54adc8d7625f46b/mods/taskbar-elastic-pill.wh.cpp))

* Added requested 'None' animation mode
* Fixed color app icon color mode crossfade sometimes permanently inheriting an app's icon color as the color-to-fade-from until mod is restarted.
* Fixed rare flickering issue with color cross-fade with app icon color mode

## 1.1.0 ([Jul 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e845e641c7d0d1590296685d367841b91087f93a/mods/taskbar-elastic-pill.wh.cpp))

* Added 'Elastic' animation mode, which is a hybrid of stretch and bounce, inspired by Apple's Dynamic Island animations.
* Added spring physics toggle for smoother animations, which is especially noticeable paired with bounce or elastic animation modes.
* Added a speed multiplier setting.
* Added a simple color-crossfade animation when using app icon color mode.
* Fixed initialization bug where the pill renders at the far left of the taskbar.
* Fixed an issue where the pill retains a closed app's position before animating to the currently focused app's.

## 1.0.0 ([Jul 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/539e1e531d5bdf820354c713e6975552c18cad9d/mods/taskbar-elastic-pill.wh.cpp))

Initial release.

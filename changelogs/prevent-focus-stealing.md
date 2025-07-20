## 1.0.1 ([Jul 20, 2025](https://github.com/ramensoftware/windhawk-mods/blob/07ca47be112f26803db61e862625f660a9f4a952/mods/prevent-focus-stealing.wh.cpp))

Reimplemented support for "Never Focus" setting and made changes to settings be applied to existing windows

* Reimplemented support for "Never Focus" setting which was forgotten during overhaul
* "Never Focus" only prevents the app from focusing itself, while allowing manual activation by the user
* Changes to settings are now applied to existing window and not only new windows
* Titles are rechecked on late load in case reload happened between window creation and title being set
* Fixed timestamp being truncated after 49 days uptime on 32 bit systems

## 1.0.0 ([Jul 19, 2025](https://github.com/ramensoftware/windhawk-mods/blob/bb5bb030304e6a0be4276b1a97237e05d99e2f12/mods/prevent-focus-stealing.wh.cpp))

Initial release.

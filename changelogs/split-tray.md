## 1.4.8 ([Nov 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4f5fa67cfa26842d4c144e4dcda0873310657043/mods/split-tray.wh.cpp))

* Drag application icons directly between Windows' primary notification area and Split Tray's embedded or floating trays.
* Show the dragged icon, allowed cursor feedback, insertion lines and dimmed source ghosts. Reordering commits on release and cancels cleanly.
* Coalesce icon refresh bursts and reuse maintained display geometry to avoid redundant taskbar work.
* Improve Arrange with search, explicit order controls, per-icon rule reset, resizing, scrolling and DPI updates.
* Share saved order and hidden choices across embedded and floating trays; refresh open overflow cells in place and protect taskbar space.
* Improve screen-reader controls and touch/pen hold menus; make global shortcuts opt-in and off by default with Ctrl+Shift+F11/F12.
* Integrate secondary overflow dragging, Show promotion for capacity overflow, and missed-release refresh recovery.
* Respect swapped mouse buttons and preserve captured floating clicks through release.
* Make private drag hooks optional so unavailable symbols do not disable the existing IconView constructor hook.

* Preserve remembered order across tray transfers; promote only explicit Show and overflow-to-bar drags.
* Improve overflow popup hit testing and unload cleanup; reuse the owned Arrange window handle.

## 1.3.3 ([Oct 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d73e9f74f445a07dd765cf6479498d20c421d2ff/mods/split-tray.wh.cpp))

Initial release.

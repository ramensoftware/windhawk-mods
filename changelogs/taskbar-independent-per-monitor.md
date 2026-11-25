## 1.7.2 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f11433e958a8f8a44449a19aefc096eedec76c22/mods/taskbar-independent-per-monitor.wh.cpp))

**1.7.2**

* Pins made from a window button no longer break after the app restarts. With *Disable grouping on
  the taskbar*, the mod pins the app's Start menu shortcut instead. This is a temporary workaround
  for #7900, existing pins are not changed.
* The jump list of a window button shows *Pin to taskbar* again if the app is only pinned on another
  taskbar.
* Pinning from a window button puts the pin on that taskbar (before, it could end up on the main
  taskbar).
* Fixed a rare case where pinning an app on another taskbar too moved it to the end of the pin list.
* Code cleanup: shared helpers and automatic cleanup of system objects.

**1.7.1** (not released on its own, included here)

* Works together with mods that change taskbar grouping (e.g. *Disable grouping on the taskbar*,
  including *Pinned items remain in place*): pinning and unpinning from a window button now apply to the
  clicked taskbar. Before, *Unpin* could remove the pin from another taskbar.
* Readme in English, German, Russian, French and Spanish, with animated before/after pictures and a link
  for reporting bugs.
* The note about other mods is now a general hint instead of "not compatible".

## 1.7.0 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cc308a58f9ddbaf76a9d2425e710abe68ea3b70f/mods/taskbar-independent-per-monitor.wh.cpp))

* Fixed: window buttons of windows parked off-screen could appear on the taskbar of the leftmost monitor.
* Fixed: clicking a pin could freeze the taskbar while the app's minimized window was busy.
* Setting titles start with their section (Windows, Pins, Click, Animation).
* Settings are available in English, German, Russian, French and Spanish.

## 1.6.0 ([Oct 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b0f5bd82a63baa016bb54b8604848c0d287ed9ae/mods/taskbar-independent-per-monitor.wh.cpp))

* Windows 10 support (tested on build 19044 with 4 monitors).
* Each taskbar keeps its own order of buttons, also after a restart. New window buttons are added at the end; buttons moved by the user keep their place.
* Drag a pin to another taskbar to move it there, to the place where it's dropped. A shortcut dropped on a taskbar is pinned where it's dropped.
* New option: open a clicked pin's app on the monitor of that taskbar.
* Fixed: the window-move thread could still run while the mod was unloaded.

## 1.5.0 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/004342fdf226b83a905abffd4a880249e2c47679/mods/taskbar-independent-per-monitor.wh.cpp))

Initial release.

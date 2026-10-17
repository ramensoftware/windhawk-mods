## 1.1 ([Oct 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c1f6d0d3554f55260011548f812901bcce4ac5b5/mods/cursor-auto-hide.wh.cpp))

* Added the `Toggle hotkey` setting: a global shortcut that turns the auto-hide on and off at runtime.
* Added `Record the shortcut by pressing it`: press the shortcut within 60 seconds and it is copied to the clipboard in the format the field expects, with an on-screen note while the window is open.
* A shortcut needs Ctrl, Alt or Win, because a global shortcut takes its key away from every application; F1 to F24, Pause and ScrollLock may be used alone or with Shift.
* Recording accepts every key the field can hold, punctuation, lock keys and numeric keypad included, and the note is placed on the monitor under the pointer at that monitor's scale.
* Documented the recording order - shortcut first, then paste - in the readme and in both settings descriptions.

## 1.0 ([Oct 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/38431c1d52c473768fa80bae5ea0ba720aa74324/mods/cursor-auto-hide.wh.cpp))

Initial release.

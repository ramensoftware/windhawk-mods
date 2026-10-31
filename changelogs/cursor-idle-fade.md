## 1.1.0 ([Oct 31, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3a6b53839a652b42503400a26faad86bad771d88/mods/cursor-idle-fade.wh.cpp))

Added

* Excluded programs setting: the cursor is never faded while one of the listed programs has focus. You enter executable file names, such as rustdesk.exe. rustdesk.exe is on the list by default.
* README section: a new “Excluded programs” section explains why the setting exists and when to use it.

Fixed

* Remote desktop clients: the cursor could stay permanently invisible or never fade inside a remote desktop client such as RustDesk. These apps take their own copy of the system cursor. If the copy is made while the cursor is faded, the app keeps an invisible cursor. If it’s made while the cursor is visible, the app’s cursor never fades.

Behaviour details

* Focus change: if an excluded program gets focus while the cursor is faded, the cursor is restored immediately.
Settings changes: changing the list takes effect immediately. If an excluded program already has focus, the cursor is restored.
* Cost: the foreground check reuses the existing foreground-change hook, so there’s no extra polling and no change in idle CPU use.

Known limitation

* The mod can’t update cursor copies that other apps have already made. Exclusion prevents the problem for the listed programs only. If another app shows the same behaviour, add its executable name to the list.

## 1.0.0 ([Oct 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8d60b4c4113a7f0a18b4b48a5443a199acf54805/mods/cursor-idle-fade.wh.cpp))

Initial release.

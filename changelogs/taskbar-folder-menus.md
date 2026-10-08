## 2.1 ([Oct 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/349f08bd4bd7132006ca8b2a9ce5c928b6d4f91c/mods/taskbar-folder-menus.wh.cpp))

* Added native side-taskbar layouts with literal manual arrangements
* Made taskbar construction wake the icon worker without joining it
* Added cancellation checks between targets and cached every failed icon target until settings reload
* Published settings on the taskbar thread and filtered length-only edge events
* Gave the icon worker its own snapshot of the folder list and button size, so a settings save cannot race its Shell icon extraction
* Cleaned up unused helpers, initialization handles and comments

## 0.7 ([Jul 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/70800f8cdf50ac6f5a85ec0655f03db98eaa2bf5/mods/taskbar-folder-menus.wh.cpp))

Initial release.

## 1.1 ([Jan 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/632fd7eca32052974e780235c2ba000a8e459430/mods/taskbar-disappearing-tray-icons-fix.wh.cpp))

Patch GDI handle leak and disable passive mode by default.

There's a GDI handle leak that *appears* to be coming from LoadImageW in a suspiciously identical amount of icons that would be in my "quick settings tray". This adds hooks to LoadImageW, CreateIconIndirect, CopyIcon, and CreateIconFromResourceEx as possible culprits for handle leaks.

These hooks only run during passive mode. I'm making a tradeoff for reliability since a one-time rebroadcasting is unlikely to cause 10000 GDI handles to be created. Passive mode is also now off by default.

## 1.0 ([Jan 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cbb50a1cb10d76cd7c499e02ddaa63a147f2f148/mods/taskbar-disappearing-tray-icons-fix.wh.cpp))

Initial release.

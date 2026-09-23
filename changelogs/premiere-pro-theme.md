## 1.0.1 ([Sep 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/da37018fd20a8b772aa5c5cc9e533aed6f2925f3/mods/premiere-pro-theme.wh.cpp))

* Premiere Pro 26.3 and later are themed again. Those versions no longer ship the module the mod used to hook, so panels, the timeline and the rest of the interface kept Premiere's own gray while only the window frame, the menu bar and the native dialogs followed the palette.
* The band around the picture in the Source and Program monitors takes the theme's **Monitor background** on those versions too. As before, **Monitor band** is its own switch and ships off.
* Custom theme colors copied from a color picker work now. A picker writes them as `#RRGGBBAA`, and a color in that form was refused — so a theme picked rather than typed fell back to Onyx one color at a time, and looked like it had changed nothing.
* Sharing a theme no longer needs the palette set to **Custom** first. The mod's log writes the ready-to-paste line for whichever palette is in force, so a built-in palette comes out as the custom theme that reproduces it — which is where a theme of your own starts.
* The **Menu bar and menus** description no longer promises palette-colored dropdown menus on Windows 11. They are dark there, from the app mode, rather than colored by the palette, and a menu taller than the screen keeps light scroll buttons. Both were measured on build 26200.
* A Premiere the mod cannot theme now says so in the log, instead of looking exactly like one it can.

## 1.0.0 ([Sep 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c2b85a85429190b8aa1e544da5a845d9abf8ed01/mods/premiere-pro-theme.wh.cpp))

Initial release.

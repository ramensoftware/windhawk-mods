## 1.1.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/180c307cae12a0c45412f9394ff035f6a67a7ab9/mods/siri-plus-launchpad.wh.cpp))

New

* Search everything (Original Siri): typing in "Search" lists apps, actions, folders and files together in one list, like Spotlight. "Show all" opens the category with what you typed. Can be turned off.
* Windows Settings in the search: about 60 Settings pages (Display, Startup apps, Personalization, Windows Update, Wi-Fi...) are found by name or by what you would type to look for them, and open with Enter. They follow the Actions tab being on.
* Right-click a file or folder found by the search: "Open file location" shows it in File Explorer.
* Calculator: type a calculation (2+2, 15% * 80, (3+4)^2) and its result shows first; Enter copies it. Start with = or calc to always calculate (=5-3). Can be turned off.
* New settings window in the style of macOS System Settings: it grows out of the bar, its content comes in step by step, and it switches off in place like an old TV (CRT) when closed. A search field sits on top of the sections.
* Settings reorganized into 12 sections (General, Appearance, Custom theme, Search, Siri bar, Results size, Window and Fullscreen, Animations, Keyboard shortcuts, Apps and icons, Performance, Backup and defaults), with group titles and a short description for every setting.
* Settings that have no effect with your current choices are dimmed in gray, say why, and turn blue again when they can be used; whole sections (such as Custom theme) dim the same way.
* Glass style: Translucent glass (flat glass with a soft live blur, light on the PC) or Liquid glass (the background bends at the edges, Floating and Original Siri modes); the description follows the choice.
* Performance: Light mode works with any theme (no liquid glass, no live blur, no blur behind menus, no shadows, at most 60 fps, calmer animations), a frame rate limit, and an experimental option to draw with the processor instead of the graphics card.
* Clear glass theme: the selection in menus can be Glass, Colored (like macOS) or Frosted color, with a color of your choice.
* Floating power button: left or right of the bar.
* Create desktop shortcut in an app's right-click menu (also for several selected apps).
* Floating mode: the category icon in the bar turns into a back arrow, as in Original Siri.
* The "..." button turns into a soft gear under the mouse.
* Blinking text cursor in the bar and in the settings search.
* Simplified Chinese (follows Windows, or pick it in Language).

Changed

* A single "Change icon..." option: it opens your icon folder (asked for the first time). The single-file picker was removed.
* New defaults: Siri bar height 96 px; floating power button off. Users who changed these keep their values.
* Settings that only apply to some modes say so in their description, in Windhawk's settings too.

Fixed

* Clear areas of the glass let the mouse through, which made the settings button and the result rows slow to respond or skip; they now answer right away.
* Folder, file and color pickers are no longer covered by the launcher or its glass.
* Backspace with an empty search no longer switches to Applications.
* Faster drawing: shadows, gradients, settings reads and the blur behind menus are cached, and the settings rows are only rebuilt when they change.

## 1.0.0 ([Nov 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/91b08e11456122862911513f39e4ca8f79542c49/mods/siri-plus-launchpad.wh.cpp))

Initial release.

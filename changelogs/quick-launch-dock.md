## 1.1.0 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5483048714c0596d3ecfc91670898c77b60d0ead/mods/quick-launch-dock.wh.cpp))

* Up to four dock panels (right, left, top, bottom) in any combination, each with its own independent list of programs.
* Icons can be reordered by pressing and holding an icon and dragging it along the dock, or onto another panel. All panels slide out while you drag.
* Panel size settings: panel thickness, space around each icon and panel inner padding, set separately from the icon size.
* Websites can be added as slots from the "Add to the dock" item of the right-click menu, or by pasting a link from the clipboard. A click opens the address in the default browser.
* Websites get a gray globe icon. The optional "Download site icons" setting (off by default) lets the mod request only the site's own /favicon.ico in the background: a spinner is shown while it downloads and the globe stays if it cannot be downloaded. No third-party servers are contacted.
* Downloaded site icons that blend in with the panel color get a light or dark backing so they stay visible.
* Seven animation styles: Slide and pop (default), Jelly, Ripple, Fade, Genie, Oval and Droplet.
* Added an animation speed setting (default 25%) that applies to every style.
* Icon magnification under the cursor, like in macOS, works with any animation style.
* Panel opacity setting (10-100%, the icons stay opaque) and a glass style.
* Change icon / Reset icon in the right-click menu (PNG, JPG, BMP, GIF, ICO). The choice is remembered.
* Shortcut icons are taken from the shortcut target, so the shortcut arrow overlay is no longer shown.
* The "+" slot can be hidden in the settings. Items can still be added by drag and drop or from the "Add to the dock" item of the right-click menu of any icon.
* The dock lists are now stored separately for every Windows account. The first account that starts this version takes over the list saved by earlier versions.
* Fixed Chrome not starting and programs launching slowly: programs are now launched through Explorer.
* Fixed black squares behind icons.
* The old "Screen side" setting was removed. The list saved by previous versions stays on the right panel, so users who kept the dock on the left will see their list on the right panel.
* Added an animation refresh rate setting (Auto, 120, 144, 180, 240 or 360 Hz), useful for laptops whose refresh rate changes.
* Corner radius setting for the panel.
* "Follow the Windows theme" option: takes light/dark colors and the accent color from Windows and updates when the theme changes.
* Optional dot on icons of running programs (on by default).
* Small press effect when you click an icon.

## 1.0.0 ([Oct 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/35684ca5377a3971a573a524841c6922e20eab27/mods/quick-launch-dock.wh.cpp))

Initial release.

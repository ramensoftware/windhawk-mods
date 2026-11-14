## 1.1.0 ([Nov 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4f5de75424636c368c4578baee30b02f9487e6c4/mods/chrome-context-menu-items.wh.cpp))

* Add {url}, {page}, {title}, {selection}, {src} and {frame} placeholders for custom commands. Values are URL-encoded inside URLs and escaped for the command line in arguments.
* Show entries that use {url}, {selection} or {src} only when the right-clicked item provides a value.
* Allow placeholders in the program only inside a URL, or as the whole program for http and https links, and don't run entries that pass placeholders to batch files or shells.
* Add a per-item "Show only on pages" setting with wildcard address patterns.
* Add a shortcut, Ctrl+Shift by default and configurable, that shows Chrome's original page menu while held (on by default).
* Add a toggle to hide Back, Forward and Reload, and opt-in toggles for Search Google for, Copy link to highlight, Open link in new / incognito window, Open link in another profile, Picture in picture, Loop / Show controls, Emoji, Spell check / Writing direction, Password manager / Autofill and Inspect.
* Anti-alias rounded menu item backgrounds.
* Check the layout of Chrome's context menu data on every menu, and turn off the features that depend on it if the layout doesn't match.

## 1.0.0 ([Sep 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b0a115af85f476fd7e61a7ccfc9b06770f8d9b9f/mods/chrome-context-menu-items.wh.cpp))

Initial release.

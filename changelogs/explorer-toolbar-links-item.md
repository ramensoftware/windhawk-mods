## 1.1.1 ([Apr 11, 2025](https://github.com/ramensoftware/windhawk-mods/blob/5e2643ce30dbd3e7f47f4b9f68693bc8b125cd93/mods/explorer-toolbar-links-item.wh.cpp))

Injection broke in a patch update to 26100. 26100 RTM, as well as all prior Windows 11 builds, are just fine.

The method `CShellBrowser::_OnViewMenuPopup`, as well as its sole caller `_OnInitMenuPopup`, were inlined into the sole caller `WndProcBS`. As an easy workaround, I simply hooked the window procedure and checked the arguments in the same way as would result in calling `_OnInitMenuPopup`, which produces the same effect.

No other changes were made for this update.

## 1.1 ([Apr 7, 2025](https://github.com/ramensoftware/windhawk-mods/blob/94bda638328d60ba4b4d259215bf791a9b69108d/mods/explorer-toolbar-links-item.wh.cpp))

This update adds support for WoW64 (unfortunately, 32-bit Windows 10 seems
to ignore the symbol hooks), fixes a crash in Windows 11, and improves the
user experience.

The Links toolbar can now be enabled through "Options" dropdown in the View
tab when using the Ribbon UI, like other toolbars can:

https://raw.githubusercontent.com/kawapure/images/refs/heads/main/linksbar_ribbon.png

Additionally, the toolbars menu now always appears in the views menu of the
menu bar when the classic Explorer interface is enabled:

https://raw.githubusercontent.com/kawapure/images/refs/heads/main/linksbar_views_menu.png

All in all, these changes make the necessarily-hacky menu item show up in
all the right places and behave just the same as any other toolbar option
does.

## 1.0.1 ([Apr 6, 2025](https://github.com/ramensoftware/windhawk-mods/blob/a432147b52de3b822a673d6750654548de8d2f92/mods/explorer-toolbar-links-item.wh.cpp))

Fixed a minor injection issue that some users may experience.

## 1.0 ([Apr 5, 2025](https://github.com/ramensoftware/windhawk-mods/blob/d4f6575790261ab1b1d13b62ca3fc879a42a4776/mods/explorer-toolbar-links-item.wh.cpp))

Initial release.

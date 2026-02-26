## 1.2 ([Feb 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cfb61a49f07450f896d6598a1a2a18e2af344a5e/mods/win7-login-fade.wh.cpp))

* Add support for fading on sleep, hibernation, and monitor off
* Add support for setting different duration values on logon/unlock and logoff/shutdown, and optionally keeping the default DWM fade on logon/unlock
* Add support for using the kernel-mode fade implementation instead of the custom logic in this mod (on supported versions)

## 1.1 ([Feb 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/407aa7889e7c75d5eca0310b16dfc82055846d1a/mods/win7-login-fade.wh.cpp))

* Fixed the logon fade not respecting and resetting the previous gamma value, including those set by ICC profile, color calibration, some display drivers' color settings, f.lux, etc.
* Removed the 'fade on lock' feature, as it turns out, even Windows 7 did not fade when locking. That feature was also overly complicated and prone to bugs.

## 1.0 ([Feb 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bf283bac8816ef3cd5ce8dd68e8d12c3058a8340/mods/win7-login-fade.wh.cpp))

Initial release.

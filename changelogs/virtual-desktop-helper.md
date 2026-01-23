## 2.1.2 ([Jan 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/917fb6cede7d9079054a1e20be7c49eaff4cb569/mods/virtual-desktop-helper.wh.cpp))

This fix makes the mod work properly after explorer.exe restarts. fix [#2855](https://github.com/ramensoftware/windhawk-mods/issues/2855)

## 2.1.1 ([Jan 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/980412439698cdb23bf0ce440781638a2e4a7cca/mods/virtual-desktop-helper.wh.cpp))

There is no need to call InitializeVirtualDesktopAPI at module startup. it is initialized on demand anyway. fix https://github.com/ramensoftware/windhawk-mods/issues/3052

## 2.1.0 ([Dec 24, 2025](https://github.com/ramensoftware/windhawk-mods/blob/bf674013e9b1ad958972eb0c5cc632aa1674f17e/mods/virtual-desktop-helper.wh.cpp))

Add X (alongside E, N, Z) key option for "Switch to Next Desktop" hotkey, which cycles through virtual desktops in a wrap-around manner (1→2→3→...→1)

## 2.0.0 ([Dec 19, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c504a465dcdc607b3caff5c1852a1ef752f200ee/mods/virtual-desktop-helper.wh.cpp))

- Add window tiling (manually triggered by hotkey)
- Add window pinning to all desktops
- Add previous desktop toggle
- Add per-desktop focus/layout memory

## 1.0.0 ([Dec 9, 2025](https://github.com/ramensoftware/windhawk-mods/blob/835cd298a06d12ab8373c6e07f94db05b2e1c536/mods/virtual-desktop-helper.wh.cpp))

Initial release.

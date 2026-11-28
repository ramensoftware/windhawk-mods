## 2.6.0 ([Nov 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/83e88597e526b16fe12f18d0b553726550070ad1/mods/virtual-desktop-helper.wh.cpp))

* **Move window to previous/next desktop:** new hotkeys with wrap-around, sharing the Move Window modifier and the Maximum Desktops limit. If the current desktop is beyond that limit, Next goes to desktop 1 and Previous goes to the last desktop in range. Follow Moved Window is respected; the keys are unassigned by default.
* **Separate pin/unpin:** new optional Unpin key. When set, Pin only pins and Unpin only unpins; when blank, Pin keeps the existing toggle behavior. If the unpin hotkey cannot be registered, Pin keeps toggling.
* **Explorer restart recovery:** pin and move operations reconnect to the shell after Explorer restarts.
* **Windows 11 23H2 .3085+ support:** added the new interface entry; desktop switching stays instant on this build.
* **Desktop switch animation:** new "Animate Desktop Switch" setting (on by default) for Windows 11 24H2/25H2, ignored when Windows animation effects are off. Focus and tracking are restored after the slide finishes.
* **Windows version check:** the mod logs a warning when the selected version entry does not match the detected build.

## 2.5.0 ([Sep 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/dc9cd28419f63d2d86396bcb845cf8e307186d5e/mods/virtual-desktop-helper.wh.cpp))

* Per-key disable: Leaving PrevDesktopKey, NextDesktopKey, LastDesktopKey, or PinKey blank now disables only that hotkey instead of falling back to the default key. Invalid key values are logged and disabled.
* Named key handling: Tab, Space, and Enter are parsed explicitly, and unsupported multi-character key names are rejected instead of being interpreted as their first character.
* Last-used desktop: Renamed from "last visited desktop"; SwitchToLastDesktop now repairs stale tracking against the actual virtual desktop manager state and ignores targets that are already the current desktop.
* Desktop tracking: Foreground-window tracking now treats the virtual desktop manager as authoritative, avoiding incorrect state when a foreground event briefly refers to a window from the previous desktop during a transition.
* Z-order fix: After moving a window to another desktop, it is raised to the front of the normal non-topmost Z-order band without temporarily making it always-on-top.
* Hotkey lifecycle: Settings changes are applied on the existing hotkey thread through its message window instead of restarting the thread.
* No key-repeat switching: Hotkeys are registered with MOD_NOREPEAT, preventing a held key from queueing repeated desktop operations.
* UIAccess support: Adds the normal Windhawk 2.0 host and UIAccess host in addition to windhawk.exe, allowing the appropriate Windhawk 2.0 host to be used while retaining compatibility with the current stable tool-mod implementation.

## 2.4.2 ([May 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3940166c632f8dd5293d1ef4bf3138376d8cc904/mods/virtual-desktop-helper.wh.cpp))

* Align tool-mod launcher with wiki recommendations

## 2.4.1 ([Mar 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d31cc9caa1d942878e751a16924b51ab1a8a4a90/mods/virtual-desktop-helper.wh.cpp))

Update last desktop tracking when the foreground window changes, so desktop switches triggered by taskbar window activation are recorded correctly.

## 2.4.0 ([Mar 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0e5eb31430c7c36664389f737cc3b68fcd50e6d5/mods/virtual-desktop-helper.wh.cpp))

Rename confusingly named cycling functions for clarity:
- SwitchToPreviousDesktop (toggle last visited) → SwitchToLastDesktop
- SwitchToPreviousIndexDesktop (idx-1) → SwitchToPreviousDesktop
- Rename HK_PREV → HK_LAST, HK_PREV_INDEX → HK_PREV
- Rename settings keys: PrevDesktopKey → LastDesktopKey, PrevIndexKey → PrevDesktopKey
- Reorder and clarify settings group as [Previous/Next/Last Desktop]
- Update README with clearer descriptions and add See Also link
- Customize modifier keys with string 
- Bump version to 2.4.0

## 2.3.0 ([Feb 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8df200d486cea8f11c582b906f87689e371a7bc8/mods/virtual-desktop-helper.wh.cpp))

- Removed tiling functionality and all related settings/hotkeys; the mod now focuses only on virtual desktop switching, moving, and pinning.

- Added “Previous Desktop (Index)” behavior and hotkey handling, plus documentation updates for previous/next/last terminology.

## 2.2.0 ([Jan 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c6f6f935554a24307402fbe91fd45489fb059a1b/mods/virtual-desktop-helper.wh.cpp))

Add hotkey toggles and fix COM cleanup timeout ([#3149](https://github.com/ramensoftware/windhawk-mods/issues/3149))

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

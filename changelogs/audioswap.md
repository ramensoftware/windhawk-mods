## 2.1.0 ([May 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7f641c526280f213a15d2e9098f5c4e847729990/mods/audioswap.wh.cpp))

- New: Device Priority panel — rank your preferred devices; the highest-priority connected device is assigned to a swap slot automatically.
- New: Advanced Mode toggle in Windhawk settings — shows the Device Priority panel in Mod Settings.
- Fixed: GUI sizing on high-DPI monitors.
- Fixed: Context menu now follows your Windows dark/light theme.
- Fixed: AudioSwap icon is now independent — no longer linked to the Windhawk tray icon.

## 2.0.0 ([May 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/95bddae96da8f6e404291b5e7d9842674ac104d4/mods/audioswap.wh.cpp))

improved performance
Windows Native dashboard = right-click → **Mod Settings**.
All configuration (device slots, icons, mode, count) lives in the dashboard; Windhawk's settings panel is no longer used.

## 1.4.0 ([May 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e0943fee9ae46fd36207fa540b7047695ab1543c/mods/audioswap.wh.cpp))

* added Custom icon selection from `.ico` files.
* opens the file explorer after saving the custom settings

## 1.3.0 ([May 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0430b67bb5347bbe8b774d3cf99e332b04c9ecb2/mods/audioswap.wh.cpp))

- Select a mode in the Windhawk settings: Click / Scroll
- Up to 6 devices; scroll-to-swap mode; dynamic right-click menu.
- Left-click mutes in scroll mode; black dot overlay on tray icon.
- Cycling auto-unmutes previous device.
- Inactive/disconnected devices skipped when cycling.
- Mute state persisted across mod restarts.

## 1.1.1 ([May 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/795240a56bf0287788c83bd16e5f76245a8e37f5/mods/audioswap.wh.cpp))

- New icons in the settings
- Can now open Windhawk directly from the mod icon

## 1.1.0 ([Apr 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b021b7eb4f7d83858aac69a9f1eac00aeafdc618/mods/audioswap.wh.cpp))

- Right-click the icon to select devices:
  Windhawk settings page reduced to icon selection only.
  auto-detects all active audio outputs and lets you assign Device 1 and Device 2 directly from a live list.
  No more typing device names manually.

- Device selections persist across restarts.

- Toggle now matches devices by their unique system ID instead of a name substring search,
works correctly regardless of how Windows names the device.

## 1.0 ([Apr 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/de26d434714384dc9cab67fa36a08a5399276e64/mods/audioswap.wh.cpp))

Initial release.

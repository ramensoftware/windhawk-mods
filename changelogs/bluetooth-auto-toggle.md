## 1.4.0 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1d512f6e35e6a0d48b334dd1dfee5856f149d9f3/mods/bluetooth-auto-toggle.wh.cpp))

1, Mod Metadata & Settings
2. Device Name Matching Engine
3. Filter-Aware Startup & Event Watcher

## 1.3.0 ([Nov 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/be32b2a231e9fe3a4047845117ac1dba414903dc/mods/bluetooth-auto-toggle.wh.cpp))

- Changed Default Value
- Set Placement for toggleOnEveryDeviceConnection
- Infinite Loop Prevention & Smart Cooldown
- Zero-Overhead Performance

## 1.2.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5fc3ea8d67b66a7e5825abed0ae317d6c7a0971d/mods/bluetooth-auto-toggle.wh.cpp))

- Added startupBluetoothState setting to enforce Bluetooth power state (ON / OFF / Unchanged) at startup.
- Added background listener for manual turn-on events when Bluetooth starts in an OFF state.
- Reordered initialization flow so radio state is verified before device connection checks.

## 1.1.0 ([Nov 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/825ac6bf19f87740412dc3afba8254259886932a/mods/bluetooth-auto-toggle.wh.cpp))

Initial release.

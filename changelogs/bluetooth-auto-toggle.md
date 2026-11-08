## 1.2.0 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5fc3ea8d67b66a7e5825abed0ae317d6c7a0971d/mods/bluetooth-auto-toggle.wh.cpp))

- Added startupBluetoothState setting to enforce Bluetooth power state (ON / OFF / Unchanged) at startup.
- Added background listener for manual turn-on events when Bluetooth starts in an OFF state.
- Reordered initialization flow so radio state is verified before device connection checks.

## 1.1.0 ([Nov 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/825ac6bf19f87740412dc3afba8254259886932a/mods/bluetooth-auto-toggle.wh.cpp))

Initial release.

## 2.1.0 ([Jan 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ad6403b26bec3a55590affeea02bde5431709f49/mods/passkey-popup-blocker.wh.cpp))

* Fixed: Critical bug where the mod blocked all passkeys (even manual clicks) after closing and reopening the browser.
* Improved: Fixed compatibility conflicts with other mods (e.g., Edge Tab Manager blockers).
* Tech: Switched from `DispatchMessage` to `PeekMessage` to correctly handle browser process restarts and background threads.

## 2.0.0 ([Jan 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/00fadbde61a8961e0566e701552f23c43c370227/mods/passkey-popup-blocker.wh.cpp))

Initial release.

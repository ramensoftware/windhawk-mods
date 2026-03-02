## 3.1.0 ([Mar 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d555d48688bd3e832f9ffef9b012cc236dffbff2/mods/passkey-popup-blocker.wh.cpp))

* **Changed**: Increased default **Interaction Timeout** from **500ms** to **800ms**.
* **Improved**: **WebAuthn hook is now initialized first** (before message loop hooks) — mod fails cleanly early if the critical hook can't be established, avoiding partial hook state.
* **Improved**: `GetLastTick()` now uses `InterlockedOr(&val, 0)` instead of `InterlockedCompareExchange` for a more correct and idiomatic atomic read on LONG-aligned memory.
* **Improved**: `SetLastTick()` local fallback now also uses `InterlockedExchange` (was a plain assignment before), making it thread-safe in all cases.
* **Fixed**: Removed `m->time` as a tick source in message hooks — it was unreliable (can be 0 or stale in some browser message loops). `GetTickCount()` is now used consistently.
* **Code**: Replaced C-style casts with `static_cast` / `reinterpret_cast` throughout for cleaner, safer code.

## 3.0.0 ([Feb 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b1f2493c90d1f67de2767b2d669694d3b7ead97d/mods/passkey-popup-blocker.wh.cpp))

* **Major**: Added **multi-process browser support** using **shared memory** to sync user intent across Chrome/Edge tabs and sandboxed processes (fixes cases where clicks were not seen by the WebAuthn call).
* **Added**: New **Block Behavior** setting: return **NTE_USER_CANCELLED** (default/recommended) or **NTE_NOT_FOUND** (compatibility).
* **Improved**: Expanded intent detection to include mouse/touch **down/up** events and key **down/up** for **Enter/Space** (including system key variants).
* **Improved**: Added **Null DACL** security attributes so sandboxed Chromium processes can access the shared memory mapping.
* **Improved**: More robust initialization — the mod fails cleanly if the WebAuthn hook can’t be established.
* **Fixed**: Proper resource cleanup on unload (`Wh_ModUninit`) and safer timing logic (wraparound-safe tick arithmetic).

## 2.1.0 ([Jan 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ad6403b26bec3a55590affeea02bde5431709f49/mods/passkey-popup-blocker.wh.cpp))

* Fixed: Critical bug where the mod blocked all passkeys (even manual clicks) after closing and reopening the browser.
* Improved: Fixed compatibility conflicts with other mods (e.g., Edge Tab Manager blockers).
* Tech: Switched from `DispatchMessage` to `PeekMessage` to correctly handle browser process restarts and background threads.

## 2.0.0 ([Jan 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/00fadbde61a8961e0566e701552f23c43c370227/mods/passkey-popup-blocker.wh.cpp))

Initial release.

## 3.4.0 ([Jun 3, 2026](https://github.com/ramensoftware/windhawk-mods/blob/59c9161f0f38faf7e4f88414e0f35d428d4dd61b/mods/zen-desktop-toggle-icons.wh.cpp))

* **Asynchronous Execution Pipeline (`PostMessageW`)**: Replaced all synchronous `ShowWindow` calls in subclass procedures with asynchronous custom messages to avoid message queue deadlocks and Explorer crashes.
* **Full-Screen Application Guard**: Added automatic active window monitoring. Prevents auto-hiding icons when a full-screen application (video playback, PPT slide presentation, or game) is active.
* **Zero-Latency Physical Coordinate Guard**: Replaced the laggy 100ms timer guard with a precise, zero-latency mouse coordinate comparison to filter out synthetic window-layout movements immediately while enabling absolute 0ms physical mouse-move restore.
* **Dynamic `CS_DBLCLKS` Support**: Queries GCL_STYLE dynamically to support both native double-click inputs and manual interval-distance tracking fallback for windows lacking double-click style (like `SHELLDLL_DefView`).
* **Type Safety & Process Check**: Standardized custom properties using `UlongToHandle` / `HandleToUlong` to prevent 64-bit truncation, and added PID checks before modifying window states on subclass cleanup.

## 1.2.0 ([May 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/130894a9b78c9429b22e05420e865d51b9f448b7/mods/zen-desktop-toggle-icons.wh.cpp))

Initial release.

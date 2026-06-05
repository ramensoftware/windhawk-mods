## 1.0.1 ([Jun 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7b8cbb03327c369d4eb77030b8dd7c0a9c7ee242/mods/transparent-desktop-icons-spotlight.wh.cpp))

### 🚀 Architecture Updates & Optimizations

*   **Robust Thread-Safe Initialization:** Fixed early-boot race conditions and cold-start bugs. Implemented a throttled desktop discovery loop with an exponential backoff system that safely triggers native overlay initialization on the correct desktop thread.
*   **0% CPU Deep Sleep:** Replaced active rendering poll loops with an event-driven `WaitForSingleObject` wait. The background thread drops into an `INFINITE` sleep state (0.00% CPU/GPU) when the desktop is fully idle.
*   **Zero-Latency Mouse Wake:** Bypassed Windows timer throttling by intercepting hardware-level mouse events (`WM_MOUSEFIRST/LAST` and pointer updates) in the message dispatch hook to wake the render thread instantly.
*   **Refactored Wallpaper Capture:** Refined the background capture routine to target the correct `WorkerW` sibling and cleanly hide transient layers, resolving theme-change ghosting and recursive rendering bugs.
*   **UX & Stability Fixes:** 
    *   Keeps the spotlight fully illuminated while renaming desktop icons (F2 edit controls).
    *   Added robust DXGI device loss recovery to rebuild the DirectX pipeline after GPU resets or system sleep.
	* Minor settings description changes for better readability

## 1.0 ([Jun 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7de4fff6bbaa2bbc92512039e8bc39c61480c7bf/mods/transparent-desktop-icons-spotlight.wh.cpp))

Initial release.

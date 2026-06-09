## 1.1.0 ([Jun 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/20aea8bd8a1224edf15d725d671a55cf158f2577/mods/transparent-desktop-icons-spotlight.wh.cpp))

**NEW FEATURE**
*   Double-click on empty desktop background to hide or show the overlay

**Added:**
*   Native WIC image decoding (`-lwindowscodecs`).
*   COM integration (`IDesktopWallpaper`) for accurate Fit/Position enums.
*   Registry Watcher thread for instant theme/wallpaper detection.
*   Native Window Subclassing for desktop icon interaction.
*   Support for Solid Colour backgrounds and Centre/Fit letterboxing.

**Changed:**
*   Refactored thread safety: Split `g_stateMutex` (UI) and `g_renderMutex` (Render).
*   Replaced GDI `CaptureWallpaperBitmap` with WIC `LoadWallpaperBitmap`.
*   Optimized render loop sleep states for high-refresh-rate (144Hz+) monitors.
*   Replaced global DispatchMessageW hook with a thread-local WH_GETMESSAGE bootstrap
*   Replaced hardcoded WM_APP macros with dynamic RegisterWindowMessageW routing
*   Replaced per-frame WindowFromPoint Z-order traversal with native TrackMouseEvent (TME_LEAVE) on desktop subclasses.
*   Replaced per-frame WindowFromPoint Z-order traversal with native TrackMouseEvent (TME_LEAVE) on desktop subclasses. This eliminates hundreds of Win32 global lock acquisitions per second, dropping idle CPU usage to absolute zero and preventing micro-stutters on high-refresh-rate monitors.
*   Added bounds checking and path validation to TranscodedImageCache parsing to prevent std::bad_alloc crashes from corrupted registry data, and added ERROR_SUCCESS validation to the Registry Watcher thread to prevent undefined behaviour from null handles.

**Fixed:**
*   Wallpaper "Fill" mode misalignment across all DPI scales.
*   Spotlight/Selection masks bleeding through the wallpaper overlay.
*   Overlay turning solid black/colour after DPI scaling changes.
*   Micro-stutters caused by UI thread mutex contention during D2D rendering.
*   Icon selection not moving from keyboard input and clean-up code

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

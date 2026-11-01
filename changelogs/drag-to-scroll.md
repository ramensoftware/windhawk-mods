## 1.1.0 ([Nov 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d13d325a5443ddd931928f05762566bec3c0ac99/mods/drag-to-scroll.wh.cpp))

- ⚠️ **Important for v1.0.0 Users:** 
- Due to an unloading bug in v1.0.0, clicking "Update" to install this version may cause active applications to freeze or close as the old version unloads. To avoid losing work, please save your open apps before updating, or restart your computer after the update completes. This issue is fully resolved in v1.1.0 and will not happen on future updates.

* **Horizontal & 2D Scrolling:** Added full sideways drag-to-scroll with automatic directional snapping to keep vertical and horizontal movement straight and prevent diagonal drift.
* **Custom Dual-Contour Cursors:** Added built-in, high-contrast dynamic drag cursors (vertical, horizontal, and cruising) with centered hotspots that automatically scale to match system display metrics without external assets.
* **Accidental Browser Zoom Prevention:** Implemented a 250ms compositor drain shield that prevents browsers (Chrome, Edge, Firefox) from zooming in or out when letting go of the scroll gesture.
* **Motion Smoothing & Velocity Curve:** Added a low-pass motion filter to eliminate sensor jitter and micro-stutter, paired with dynamic velocity scaling for responsive crawls and rapid flicks.
* **Initialization & Stability Hardening:** Moved cursor loading to on-demand initialization and excluded non-interactive desktop sessions and sandboxed processes to eliminate mod load hangs.
* **Refined Discrete & Sub-Notch Scrolling:** Optimized legacy Win32 line-stepping message dispatch and fine-tuned fractional wheel deltas for fluid modern app scrolling.
* **Clearer Documentation:** Redesigned the README with everyday quick-start instructions, feature highlights, and clarified capability notes.

## 1.0.0 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/365a77bb8705c9f36705cf265a395a7c499780a8/mods/drag-to-scroll.wh.cpp))

Initial release.

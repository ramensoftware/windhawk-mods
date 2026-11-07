## 1.2.0 ([Nov 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d1647770bf67559be8d1eb209b6a9876a1c56f32/mods/drag-to-scroll.wh.cpp))

### ✨ New Features
- **Right-Click Only Trigger Mode:** Added a `trigger_mode` setting to switch between `Ctrl + Right-Click` and `Right-Click Only` (cruising is automatically disabled in Right-Click Only mode).
- **5-Tier Dynamic Velocity Mode:** Replaced the old `enable_velocity` toggle with `velocity_mode` (`Off`, `Gentle`, `Standard`, `Fast`, `UltraFast`). *Note for upgrading users: the new `Standard` default is gentler and more controlled than v1.1.0; select `Ultra Fast` if you prefer the stronger v1.1.0 flick boost.*
- **Configurable Cruising Anchor:** Added `cruise_anchor` to start hands-free cruising by holding `Right-Click` (default), `Ctrl`, or `Either`. *Note for upgrading users: If you used cruising in v1.1.0, select Hold Ctrl to keep the old gesture.*
- **Out-of-Bounds Dragging:** Added `allow_out_of_bounds` to keep scrolling even when the cursor leaves the target window.

### 🛠️ Bug Fixes & Stability
- **Cross-Process Cruise Cancellation:** Added per-session shared-memory coordination so clicking on other hooked desktop apps during `Ctrl`-anchored cruising stops the glide cleanly and swallows the stop-click (with instant fallback for unhooked, UWP, or elevated surfaces).
- **Swapped Mouse Button Support:** Added `SM_SWAPBUTTON` handling so right-click tap replay, RMB-anchored cruising, and cancel checks work properly for left-handed mouse setups.
- **Scroll Target & Capture Reliability:** Locked wheel scroll delivery to the initial target window via `PostMessageW`, deferred window activation until past the deadzone, and added `GetCapture()` loss recovery (e.g., on `Alt+Tab` or modal dialogs) without rogue context menus.
- **Cursor & Modifier Polish:** Suppressed deadzone cursor changes in Right-Click Only mode, improved arrow cursor restoration when scrolling ends, and refined `Ctrl` masking during scroll drain to prevent accidental browser zooming.
- **Thread & Unload Safety:** Refactored state synchronization with `CRITICAL_SECTION` and atomics, hardened Base64 cursor bounds and alignment checks, joined the cruise worker in `Wh_ModBeforeUninit`, and updated `@exclude` entries (`conhost.exe`, `ShellExperienceHost.exe`, `Widgets.exe`).

### 📝 Documentation
- Added a **Recommended Settings** section to the README (highlighting why the `Hold Right-Click` cruising anchor prevents click leaks across elevated, UWP, and system windows) and updated settings descriptions to cover trigger trade-offs and compatibility notes.

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

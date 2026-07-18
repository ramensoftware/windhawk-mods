## 0.9.211 ([Jul 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/427a0adad1078cb37b0f397ce82f77b39ed9bd3b/mods/island-media-controls.wh.cpp))

- Add a compact expanded layout that places the album cover beside the title and artist, reducing pointer travel to the playback controls.
- Add taskbar-position awareness, including support for Taskbar on top for Windows 11: the popup direction, component layout, and album wash adapt to the taskbar position.
- Refine expand and collapse animations so the compact island, artwork, progress bar, playback controls, text, corner radius, and live backdrop morph more cohesively.
- Restore display-refresh-rate-synchronized live blur without the temporary 60 FPS animation cap or 15 FPS settled-state cap.
- Optimize Acrylic and Liquid Glass with WGC dirty-region rendering, Direct2D effect-graph composition, cached GPU resources, and fewer full-size intermediate render passes.
- Serialize WGC frame rendering, DirectComposition updates, and live-blur render snapshots under the overlay mutex to avoid free-threaded capture races with UI-thread animation and settings updates.
- Improve Acrylic and Liquid Glass interaction stability, reducing flicker, self-sampling, invisible overlay hit-test issues, and repeated-toggle stalls.
- Refine the Liquid Glass edge refraction and strengthen the light-mode backdrop and playback-control tint for clearer surface shape.
- Increase the playback-control album-wash layer opacity so the control card reads as more strongly blurred.
- Improve light-mode visibility by keeping the album-art wash enabled for Liquid Glass.
- Remove obsolete CPU WGC presentation, native-blur handoff, diagnostic, and unused animation helper paths while preserving the recording-mode static blur fallback.
- Update the default settings to the currently tuned setup while keeping browser thumbnails as the default artwork mode.
- Refresh the README and What's new sections to focus on user-facing features.
- Use SetWindowSubclass/RemoveWindowSubclass for safe popup XAML child cleanup.

## 0.9.146 ([Jul 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a9b7eaed62566ac6fbe2fe5ea8371af1f475cced/mods/island-media-controls.wh.cpp))

### Materials and defaults

- Changed the default material for new installs from Mica-like to Acrylic.
- Added a one-time migration for users who still had the old Mica-like default.
- Restored Mica-like as a normal explicit choice after migration.
- Fixed the light-mode Mica-like expanded background becoming almost pure white from a duplicated tint layer.
- Kept compact and expanded material colors consistent across light and dark themes.

### Liquid Glass and Acrylic rendering

- Added the Liquid Glass material option.
- Added GPU displacement-map refraction with a rounded-rectangle SDF so edge and corner refraction follows the component shape.
- Added a narrow high-index rim and a wider, smooth interior falloff into the center.
- Increased sampling quality and removed the earlier tiled/nine-slice appearance.
- Added theme-aware edge highlights, reduced dark-mode tint, and independently tuned blur for the main surface and playback card.
- Applied matching material treatment to the expanded playback controls.
- Retained the live WGC/D3D/D2D Acrylic backdrop and fallback blur handoff.

### Recording support

- Added an optional screen-capture setting for the expanded popup.
- Added a capturable static blurred backdrop for Acrylic and Liquid Glass recording.
- Avoided recursive self-capture while keeping the blur layer visible to recorders.
- Added dark/light Mica-like, Acrylic, and Liquid Glass screenshots to the mod details page.

### Media transitions and presentation

- Added smoother directional title and artist transitions.
- Added edge-aware fading that appears as text moves beneath the artwork or popup boundary.
- Reversed expanded transition direction when navigating to the previous track.
- Removed transition-end flicker, persistent fade overlays, black blocks, and abrupt clipping.
- Improved compact and expanded text clipping consistency.

### Media and browser session handling

- Improved browser/live-media artwork identity tracking.
- Prevented stale thumbnails from leaking between browser sessions.
- Added bounded waits around media properties, thumbnail reads, transport commands, and manager requests.
- Kept event-driven GSMTC refreshes with a low-frequency provider compatibility fallback.
- Preserved Apple Music seeking and transport-control compatibility.

### Lifecycle and unload safety

- Released GSMTC manager references before uninitializing the WinRT apartment.
- Explicitly released WGC D3D/DXGI/D2D and WinRT capture resources before DLL unload.
- Added lifecycle guards around injection, callbacks, settings changes, and teardown.
- Avoided indefinite unload joins and lost media-thread wakeups.
- Improved recompilation and reload behavior so Explorer no longer needs to be restarted after each build.

### Threading, performance, and DPI review fixes

- Removed the dead WGC availability macro and unreachable stub implementation.
- Moved overlay window presentation to its owning UI thread through an asynchronous latest-frame queue.
- Kept cross-thread WGC rendering under its resource mutex without holding that lock across `UpdateLayeredWindow`, `ShowWindow`, or capture-affinity calls.
- Replaced the permanent per-frame taskbar layout monitor with a 600 ms Win32 timer.
- Created the popup on the taskbar monitor before initializing XAML and added `WM_DPICHANGED` handling for mixed-DPI multi-monitor setups.
- Switched the D2D factory to multi-threaded mode.
- Allocated the readback buffer before mapping the staging texture and explicitly closed consumed WGC frames.
- Removed the duplicate Windows.Foundation include and the dead popup controls scale state.

### Details page

- Added a top-level What's new section.
- Added a three-material dark/light comparison table.
- Preserved the original overview, animated previews, and feature list.

## 0.9.27 ([Jul 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1f7614c178f23c0662bc8f677207cb9998bb9340/mods/island-media-controls.wh.cpp))

Initial release.

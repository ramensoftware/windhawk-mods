## 1.1.0 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d8d332cac51b2112390617ea31229fd76fb7df20/mods/neiz-supersmile-audio-visualizer.wh.cpp))

* Replaced standalone EQ popup with a full Media & EQ overlay featuring playback controls, seek bar, and scrollable interactive lyrics.
* Added support for up to 12 custom EQ presets with responsive layout and active-preset auto-saving.
* Popup positioning with aspect-ratio placement storage and bounds clamping.
* Reworked "Selected applications" mode to capture and mix audio from multiple target processes simultaneously.
* Added automatic process tracking, device-invalidation recovery, and state resets when switching audio modes.
* Upgraded album color extraction to a perceptual OKLab k-means pipeline with better hue diversity and deterministic results.
* Optimized blur rendering with a sliding-sum algorithm and added per-overlay blur/artwork caching.
* Added multi-monitor selection support and dynamic bar width based on cursor distance.
* Added Windows 11 24H2+ desktop placement modes (behind/above icons).
* Added native Windows 10 support with tray icon integration and automatic tray-recreation recovery.
* Added local `.lrc` lyrics file matching (`Artist - Title.lrc`).
* Added smart per-monitor throttling (drops to 1 FPS during fullscreen applications without changing global settings).

## 1.0.1 ([Sep 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e01d0d0dbd6204804235831fd7f68821e4614028/mods/neiz-supersmile-audio-visualizer.wh.cpp))

Initial release.

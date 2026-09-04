## 1.2.0 ([Sep 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2d25f22950c8994476d69ab5728ab2105d476537/mods/explorer-info-bar.wh.cpp))

- Adds configurable font family, font size, left padding, and spacing between info sections.
- Adds solid and directional gradient fills for Flat panes and Soft cards.
- Expands single-file details for photos and RAW files with dimensions, camera and lens information, plus optional ISO, aperture, shutter speed, and focal length.
- Expands video and audio details with duration, video resolution, and optional frame rate, sample rate, and channel information.
- Adds separate Off / Standard / Extended detail controls for photos, media files, and other file types.
- Adds optional Windows file type and modified date/time details for other files.
- Enables single-file details by default.
- Improves metadata caching and refresh behavior.
- Reduces unnecessary filesystem access when resolving file type descriptions.
- Improves metadata shutdown/cancellation and settings refresh behavior.
- Fixes custom left-padding and section-spacing values not always applying correctly.
- Fixes stale single-file metadata remaining visible after deselecting a file.
- Fixes leftover file-detail content when switching from a file with longer metadata to one with shorter metadata.

## 1.1.0 ([Sep 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/08025d3448b960177420ffba3cc4e6dcb68fbfe8/mods/explorer-info-bar.wh.cpp))

- Adds an optional setting to hide Explorer's native bottom-right view buttons.
- Covers the native button area when hidden so the controls don't peek through on hover or press.
- Suppresses pointer interaction in the hidden button area while preserving Explorer's keyboard view shortcuts.
- Keeps the existing behavior by default (`Show Explorer view buttons` is enabled).

## 1.0.0 ([Sep 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e012cc18f46b9136c58fdf570368d7de19b183a7/mods/explorer-info-bar.wh.cpp))

Initial release.

## 0.9.2 ([Nov 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/aa6d2e02e6c2f1bdd3448937e1147caf8ab9cad4/mods/parallax-wallpaper.wh.cpp))

- Fixed perspective having no visible effect. Effect setup failed with E_INVALIDARG because the 3D transform was given an unsupported interpolation mode, so perspective disabled itself silently. It now uses cubic sampling, and falls back to linear sampling if a driver rejects it.
- Made perspective clearly visible: at 100% intensity, the virtual viewing distance now equals the screen's larger dimension (previously twice that), so the tilt no longer reads as extra movement.
- The default perspective tilt is now 4° instead of 2°. Existing settings are kept; if perspective still looks subtle, raise the tilt to 4° or more.

## 0.9.0 ([Sep 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0f940ba13bb4cfbc285f2b5acdf4e2cacac112e7/mods/parallax-wallpaper.wh.cpp))

Initial release.

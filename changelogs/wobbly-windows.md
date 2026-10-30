## 0.115.4 ([Oct 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2742e6b601ca2a1d7295ddeaf115e4555351eb0f/mods/wobbly-windows.wh.cpp))

Fixed complete-window targeting and initialization failures on affected Windows 11 builds.

- Fix content clipping, stationary borders, and related visual artifacts by targeting the complete window root instead of falling back to an inner Canvas visual.
- Resolve the visual-proxy accessor independently of the Canvas getter’s layout.
- Support both verified null-check branch layouts in native getters, fixing “Failed to initialize DWM hooks” when the direct accessor symbol is unavailable.
- Validate decoded offsets against independent transition getters when available, while preserving module and proxy-type checks.
- Improve startup diagnostics to identify the selected access path and compatibility failures.

## 0.115 ([Sep 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/dfb8f35a56860377efb2ee4c719ca66997f6eb0a/mods/wobbly-windows.wh.cpp))

Initial release.

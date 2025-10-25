## 0.2 ([Oct 25, 2025](https://github.com/ramensoftware/windhawk-mods/blob/962cbdb6b5d7944f668aa12a44af0d45f68f4d38/mods/explorer-font-changer.wh.cpp))

- Live font changes:
Through hooking into `DrawText[Ex]W`, we can now change fonts at the time of rendering, instead of depending on intercepting font creation!

- Glow text:
This toggles on the "Glow text" functionality, proxying calls to `user32!DrawTextW` and `user32!DrawTextExW` to `uxtheme.dll`'s "DrawGlowText" function. Thanks to the mod "Translucent Windows" for bringing this function to my attention!

- Glow: Red (0-255):
The red value of the glow text color (0-255).

- Glow: Green (0-255):
The green value of the glow text color (0-255).

- Glow: Blue (0-255):
The blue value of the glow text color (0-255).

- Glow: Alpha (0-255):
The alpha value of the glow text color (0-255).

- Glow: Intensity (1-..):
The intensity value of the glow that surrounds the text.

## 0.1 ([Oct 14, 2025](https://github.com/ramensoftware/windhawk-mods/blob/1a36bc8c17174f3930c1999973759fb639e03320/mods/explorer-font-changer.wh.cpp))

Initial release.

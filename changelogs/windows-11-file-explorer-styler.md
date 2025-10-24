## 1.2.2 ([Oct 24, 2025](https://github.com/ramensoftware/windhawk-mods/blob/d4f524e3e969acf4f18a77650a5afdb093961008/mods/windows-11-file-explorer-styler.wh.cpp))

* Added themes: Matter, WindowGlass.
* Images from online sources are now fetched once internet becomes available. Previously, if images didn't load successfully, disabling and re-enabling the mod was required.
* WindhawkBlur brushes that use ThemeResource colors are now updated automatically on theme change. Previously, disabling and re-enabling the mod was required to apply the new colors.

## 1.2.1 ([Aug 4, 2025](https://github.com/ramensoftware/windhawk-mods/blob/9bf555a229d798630345d558c6f1c30ffbc500ce/mods/windows-11-file-explorer-styler.wh.cpp))

* Added the Translucent Explorer11 theme.
* Added ThemeResource support to WindhawkBlur.

## 1.2 ([May 31, 2025](https://github.com/ramensoftware/windhawk-mods/blob/c08d8c4047fd99859a2d1449a2126ec56f20ec2d/mods/windows-11-file-explorer-styler.wh.cpp))

* Added a built-in blur brush object, `WindhawkBlur`, which supports the `BlurAmount` and `TintColor` properties. For example: `Fill:=<WindhawkBlur BlurAmount="10" TintColor="#80FF00FF"/>`.
* Added themes: Tabless, NoCommandBar, MicaBar.

## 1.1 ([May 9, 2025](https://github.com/ramensoftware/windhawk-mods/blob/d01044de4128b7fafc768e340b116b2c129b660a/mods/windows-11-file-explorer-styler.wh.cpp))

* Added style constants which can be defined once and used in multiple styles.
* Improved handling of commented targets and styles (starting with `//`), and added details about this in the mod's description.
* Added the first theme: Minimal Explorer11!

## 1.0 ([May 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/8e961324620af4d91f28d77ed0daf1ce57485f0b/mods/windows-11-file-explorer-styler.wh.cpp))

Initial release.

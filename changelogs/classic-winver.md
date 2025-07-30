## 1.1.0 ([Jul 30, 2025](https://github.com/ramensoftware/windhawk-mods/blob/08e6aef365b0d283169ceff6c0c030ad2a02a3ef/mods/classic-winver.wh.cpp))

- Improve detection for 95/98/NT4 to account for IE4 desktop update shell32.dll
  - This unfortunately means that users must enter the version text for 95 and 98, as distinction between the two is impossible

## 1.0.1 ([Jul 30, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7c1f5a38df8f51277a6b4bb1d7bd0b0e408063c2/mods/classic-winver.wh.cpp))

- Hide divider when no about bitmap in 7+ layout
  - This behavior is accurate to the real implementation, and is
    especially needed since `BrandingLoadImage` is seemingly broken
    outside of `winver.exe` in recent Windows 11 builds

## 1.0.0 ([Jul 30, 2025](https://github.com/ramensoftware/windhawk-mods/blob/7968fb15fba2d062946d129a5d2a1e489e676c75/mods/classic-winver.wh.cpp))

Initial release.

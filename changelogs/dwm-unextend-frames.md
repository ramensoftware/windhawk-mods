## 1.3.0 ([Jul 5, 2024](https://github.com/ramensoftware/windhawk-mods/blob/b5539c2babd5da816922a73d3bfcfd9258e68546/mods/dwm-unextend-frames.wh.cpp))

Add option to only apply to secure desktop

## 1.2.0 ([Dec 26, 2023](https://github.com/ramensoftware/windhawk-mods/blob/b47219234aced7b977db2e1833aa5bc0006f79e0/mods/dwm-unextend-frames.wh.cpp))

Remove `IsThreadDesktopComposited` hook, add comctl32 hook for Aero wizards. As it turns out, `IsThreadDesktopComposited` is a REALLY low-level function and will break a lot of applications if hooked.

## 1.1.0 ([Dec 23, 2023](https://github.com/ramensoftware/windhawk-mods/blob/6f9ba7f4499d1e16ede82d0a2c44cdd8ad6295a5/mods/dwm-unextend-frames.wh.cpp))

Hook IsThreadDesktopComposited. This mainly fixes Aero wizards.

## 1.0.0 ([Nov 21, 2023](https://github.com/ramensoftware/windhawk-mods/blob/73631efe39f232421878841a4fe1b543e674bbe0/mods/dwm-unextend-frames.wh.cpp))

Initial release.

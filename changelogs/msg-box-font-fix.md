## 2.2.0 ([Nov 1, 2025](https://github.com/ramensoftware/windhawk-mods/blob/97fcfb04c1e490b16df1bef76c33bc440d777fe3/mods/msg-box-font-fix.wh.cpp))

- Clean up README and mod description
- Split "Windows Vista-10 1703" style into "Windows Vista" and "Windows 7-10 1703"
  - "Windows Vista" style is the same as "Windows 10-1703", but will play the "Default Beep" sound for message boxes with no icon
  - If you were using "Windows Vista-10 1703" before this update and want to continue using it, you will need to change the style back from "Windows Vista"

## 2.1.1 ([Feb 27, 2025](https://github.com/ramensoftware/windhawk-mods/blob/090064e365f4d31312c1b2fac07ec3116825699a/mods/msg-box-font-fix.wh.cpp))

Message Box Fix 2.1.1: Fix crashes on Windows 11 24H2

## 2.1.0 ([Feb 25, 2025](https://github.com/ramensoftware/windhawk-mods/blob/911869c1f7d3038b58ada05b11f0552f11d49e61/mods/msg-box-font-fix.wh.cpp))

- Refactor "Classic style" option into "Message box style" dropdown
- Add Windows 95/NT 4.0 message box style

## 2.0.1 ([Feb 25, 2025](https://github.com/ramensoftware/windhawk-mods/blob/f3d7230ad5b9dd832306c0a49fdc033c1167299f/mods/msg-box-font-fix.wh.cpp))

- Fix classic message box style horizontal sizing (fix discovered by @kawapure)

## 2.0.0 ([Sep 16, 2024](https://github.com/ramensoftware/windhawk-mods/blob/e7a03d627c3bc66756e9748a987d484863153397/mods/msg-box-font-fix.wh.cpp))

- Restructure code to match modern conventions
- Remove option to fix the font and make it always fix it; there's
  not much use to having it disabled.
- Revamp "Remove background" option
  - Renamed to "Classic style"
  - Now shows message boxes with Windows XP positioning and scaling
  - May break with per-monitor DPI, have not tested. Regardless, I don't
    think anyone using a Windows XP/2000 theme would want per-monitor DPI.

## 1.5.0 ([Dec 22, 2023](https://github.com/ramensoftware/windhawk-mods/blob/dd8ab1339fb2b5208222b38f325f8bfc378da558/mods/msg-box-font-fix.wh.cpp))

* DPI support improvements

## 1.4.6 ([Nov 5, 2023](https://github.com/ramensoftware/windhawk-mods/blob/a08c60fdb07f810002787e4b6bf12dd0828973ce/mods/msg-box-font-fix.wh.cpp))

* Add option to fix background color for pre-Windows Vista classic theme look

## 1.2 ([Sep 2, 2023](https://github.com/ramensoftware/windhawk-mods/blob/cc1a729e476a6b5044cd09d1068f6ad195292cbe/mods/msg-box-font-fix.wh.cpp))

* Temporarily x64 only

## 1.1 ([Sep 2, 2023](https://github.com/ramensoftware/windhawk-mods/blob/f4942e9c7cba28ea5fa515307e4dd843969a7902/mods/msg-box-font-fix.wh.cpp))

* Use built-in WindhawkUtils

## 1.0 ([Sep 2, 2023](https://github.com/ramensoftware/windhawk-mods/blob/dd74a66cba0597c67f530df2d84259bcbc94432d/mods/msg-box-font-fix.wh.cpp))

Initial release.

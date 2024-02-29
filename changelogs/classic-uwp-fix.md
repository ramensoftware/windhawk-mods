## 0.3 ([Feb 29, 2024](https://github.com/ramensoftware/windhawk-mods/blob/596131805828a3dccd291548eadf7cd1f2b1a13c/mods/classic-uwp-fix.wh.cpp))

Added support for the UWP MSA sign-in window, hosted by explorer.exe (or ShellAppRuntime or CustomShellHost).
To get the Explorer hooking working without crashing, I had to add checks to ensure that the program requested the UWP splash fade animation. Explorer uses the same function on startup to fetch a different animation.

## 0.2 ([Feb 2, 2024](https://github.com/ramensoftware/windhawk-mods/blob/d29135d8365b71afaeb4fce0ca06d793bf133e2a/mods/classic-uwp-fix.wh.cpp))

Initial release.

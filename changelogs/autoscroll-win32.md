## 1.0.2 ([Jun 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/10df87270e61881eaa75f976de21d0a3d9e58464/mods/autoscroll-win32.wh.cpp))

1. Fixed possible explorer.exe crash on startup
2. Fixed inverted horizontal scrolling

## 1.0.1 ([May 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d9e0a6536452c29607e43e8030a4dd03c7d7ee84/mods/autoscroll-win32.wh.cpp))

1. Fixed middle-click "open in new tab" so it now works in hold mode
2. Fixed overscroll without Smooth Scroll Win32 (capped to 1 wheel notch per frame)
3. Added Toggle Mode -- click to start scrolling, click again to stop
4. Added: Block Middle Button Release -- suppresses mouse3 release for bound apps
5. Added per-monitor DPI support for the visual indicator
6. Improved: scroll thread idles at zero CPU (WaitForSingleObject on event)

## 1.0.0 ([May 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/126a2a9093f065ec58610d11c98e8f75a0cdb014/mods/autoscroll-win32.wh.cpp))

Initial release.

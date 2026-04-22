## 2.2 ([Apr 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/8346ae1c33be7e1d792c7e8351455d26862a2aa0/mods/taskbar-autohide-instant-show.wh.cpp))

- Add No animation and Overshoot options
- Upgrade Slide + Fade to quintic ease for silkier motion
- Edge detection: replace one-shot trigger with throttled retry so the taskbar reliably appears over fullscreen apps and foreground windows that swallow the edge event

## 2.1 ([Feb 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9fddc7f33e424cabfaca200627b002cff2cd24b4/mods/taskbar-autohide-instant-show.wh.cpp))

- Fix fade cleanup flash on hide (alpha set before removing WS_EX_LAYERED)
- Add animation cancellation to prevent state corruption on rapid show/hide
- Suppress pre-animation flash by updating TrayUI state while window is hidden
- Add mouse edge detection thread for monitors with no visible windows

## 2.0 ([Feb 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0278b8e63627c0577c2709c2ae9a0dd54a768811/mods/taskbar-autohide-instant-show.wh.cpp))

Initial release.

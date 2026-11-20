## 1.6.0 ([Nov 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/11efbe449849c5cb5773643a522bf4af22b28b90/mods/macos-gestures-mission-control.wh.cpp))

### New
- Four-finger option: the gestures can use three or four fingers. The other finger count keeps Windows' own gestures.
- Win+Tab opens Mission Control in place of Windows' Task View, on the monitor of the window in front, also on PCs without a touchpad. Press Win+Tab again, Esc or pick a window to close it. Can be turned off in the settings.
- App Exposé: swiping down while Mission Control is closed shows the windows of the app in front, without the desktops bar. Swipe up to leave. With the desktop or the taskbar in front, or an app that has no windows on this desktop, all the windows are shown.
- "Swipe down (Mission Control closed)" setting: App Exposé (default), all windows of the current desktop, or minimize all windows.
  - Minimizing fires once, after the fingers travel 35% of "Swipe distance".
  - It affects the windows of the current virtual desktop, on every monitor, with no limit on their number.
  - It uses the same filters as Mission Control (for example, always-on-top).
  - Windows without a minimize button are skipped.
  - Windows are minimized from the bottom of the z-order up, so Windows doesn't activate and animate the next window after each one.
  - A swipe up brings them back, in the same order, as long as nothing else was opened or restored by hand in the meantime and you are on the same desktop. Otherwise the swipe up opens Mission Control as usual.
- Alt+Tab inside Mission Control and App Exposé works as an internal switcher instead of opening Windows' switcher on top of the overlay, which used to break it. It moves the selection between the windows (Shift goes back, holding Tab keeps cycling) and picks the selected one when Alt is released. The first press goes to the window after the one you were in, and small touchpad nudges while cycling don't drop the selection.
- "Alt+Tab while Mission Control is open" setting: cycle (default), ignore, or let Windows handle it.
- Reorder desktops: drag a card of the desktops bar sideways to move that desktop. Unnamed desktops are renumbered by position, like in Task View. Clicking a card now switches on release, so a click can be told apart from a drag. Requires Windows 11 24H2 (26100.2605) or later; on other builds the cards can't be dragged and everything else keeps working.
- Titles are drawn with DirectWrite.
- README: added a "Support the project" section.

## 1.3.1 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a7e0fd466e314ed06abf79ae5cd877daa644a62d/mods/macos-gestures-mission-control.wh.cpp))

Initial release.

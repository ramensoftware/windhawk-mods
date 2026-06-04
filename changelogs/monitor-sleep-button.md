## 1.1.1 ([Jun 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7911f2ed2e772be13cb019231beb8a2d397dc15b/mods/monitor-sleep-button.wh.cpp))

Monitor Sleep Button now uses the actual Shell_NotifyIconGetRect rectangle for mouse wheel hit testing instead of expanding the tray icon rectangle. The rectangle is refreshed before handling mouse wheel events.

Tested with an adjacent tray icon. Monitor Sleep Button now only changes the countdown when scrolling over its own tray icon.

## 1.1.0 ([May 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6da1e2166b9f1d3ea9c073170de375ff2d8e8d91/mods/monitor-sleep-button.wh.cpp))

- Configurable global hotkey
- Ctrl / Alt / Shift / Win modifier support
- Right-click tray menu
- Esc cancels the countdown
- Mouse wheel over the tray icon changes countdown duration
- Tray icon displays the selected countdown duration
- Dynamic tray tooltip and menu text
- Open Windhawk shortcut
- New action mode:
  - Turn monitor off
  - Show black screen
- Black screen mode can be closed instantly with Esc, a mouse click, or by pressing the tray hotkey / tray icon again

## 1.0.0 ([May 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bba2b04e3327bb83eb2c37a9725e0782be23fb4d/mods/monitor-sleep-button.wh.cpp))

Initial release.

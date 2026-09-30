## 2.1.0 ([Sep 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a9fbca8f625ac2e8d3150da96b212090c49b432e/mods/microswap.wh.cpp))

* **Live input meter:** Added a live microphone level meter inside the volume slider popup.
* **Mute sound cues:** Optional Windows sound cues when toggling mute state.
* **Actual endpoint mute state:** Accurately reflects endpoint mute state directly from WASAPI notifications.
* **Settings focus restoration:** Re-opening Mod Settings while already running brings the existing window to the foreground.
* **Hook-free architecture:** Completely removed low-level keyboard/mouse hooks, eliminating anti-cheat interference and hook chain overhead.
* **Bluetooth auto-reconnect:** Automatically reconnects Bluetooth microphones and headsets when powered back on without requiring manual re-selection in settings.
* **Persistent disconnected devices:** Disconnected microphones are remembered in settings instead of being removed or unselected on save.
* **Explorer restart recovery:** Tray icon restores cleanly after Windows Explorer updates or restarts.
* **Multi-monitor volume slider:** Volume slider popup is properly clamped to the bottom edge of secondary monitors.
* **Security:** Hardened DLL search paths to prevent library hijacking.

## 2.0.1 ([Jul 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/99c8afb62c5af3264f20deea6408b8475a079c9d/mods/microswap.wh.cpp))

- **New:** Persistent Mute toggle in dashboard settings. When checked, mute state survives device cycles (USB replug, sleep/wake, driver resets).
- **Fixed:** Persistent mute now survives device cycling — mute is re-applied to the newly defaulted device after switching.
- **Fixed:** Tray icon red-dot overlay updates after persistent re-muting.
- **Fixed:** "Persistent Mute" checkbox text now renders correctly on the dark dashboard.

## 2.0.0 ([May 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9825ae7dfb1d78b8be7d98d2e8b2cc14547f3589/mods/microswap.wh.cpp))

- **Complete rebuild.** Full feature sync with AudioSwap:
6 device slots, configurable cycle order, native dark-themed settings dashboard, scroll-to-swap mode, push-to-mute, instant device-change updates via the MMDevice notification API, and Advanced Mode with priority routing.
- New: Right-click → **Sound Settings...** opens Windows Sound dialog directly on the Recording tab.
- New: Middle-click tray icon → compact volume slider popup with a custom dark design. Drag to adjust microphone volume live; click outside or press Escape to close.
- New: Scroll wheel over the tray icon (Click to Swap mode) adjusts microphone volume, matching native Windows sound icon behaviour.

## 1.1.0 ([May 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4a08293dac7c0135a0a059dbda268f29117f8da8/mods/microswap.wh.cpp))

- **Renamed from MicroSwap to MicSwitch.**
- Custom icon support — pick your own image for each device via Settings or right-click.
- Added donate button on the mod page.
- Context menu now follows your Windows dark/light theme.
- Active device shown at the top of the right-click menu.
- Fixed: rare crash when switching devices rapidly.
- Fixed: occasional hang when Windhawk unloads the mod.
- Fixed: tray window appeared in Alt+Tab.
- Fixed: tray icon failed to load on some Windows configurations.
- Tray / Taskbar icon is now independent, no longer linked to the Windhawk icon.

## 1.0.0 ([May 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d464024830ea0879a2e1f89c6d5be2769be2ba4c/mods/microswap.wh.cpp))

Initial release.

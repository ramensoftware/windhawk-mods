## 1.7.0 ([Sep 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/691c1898c7f30d05c5f44e919692a0268fb815f1/mods/tray-hover-expand.wh.cpp))

- Identify the chevron by its class name plus AutomationId, independent of the  display language, and accept it only when exactly one element matches
- Restrict detection to tray elements, so no taskbar button can be matched by a keyword and invoked
- Do nothing instead of guessing the chevron by position when it cannot be identified; the guess is now an opt-in setting, off by default
- Cover the most common Windows display languages in the name fallback
- Add a "Hover delay" setting, so the flyout does not open when the cursor only brushes past the chevron
- Keep tray icon context menus usable: detect clicks that fall between polling ticks, and never collapse while a context menu is open
- Suppress the chevron tooltip whenever the cursor is on it, covering both "Show hidden icons" and "Hide", and with the taskbar on either edge
- Hide only tooltips owned by the taskbar process
- Do not treat an unrendered chevron as a hover target, which could turn the corner of the screen into a hotspot and make the flyout cycle
- Drive re-finding from the cursor instead of a fixed timer, with backoff on repeated failures, so an auto-hiding taskbar works immediately on reveal and a taskbar with no hidden icons costs almost nothing
- Scope the fullscreen check to the monitor the chevron is on
- Set per-monitor-v2 DPI awareness on the worker thread
- Document that the mod is Windows 11 only, and how to restore the keyword defaults

## 1.6.0 ([Jun 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4b6102a9d024834632f3f28143a8b7172287f0ef/mods/tray-hover-expand.wh.cpp))

- **Hide the chevron tooltip** (new setting, opt-in, default off): while the flyout is open and the cursor is on the chevron, Windows shows a "Hide" tooltip that covers the bottom row of icons (#4487). When enabled, the mod hides that tooltip window. It is matched both by class and by position (only a popup sitting over the chevron is hidden), so the flyout's own per-icon tooltips and unrelated popups elsewhere are left untouched. The tooltip window class is also exposed as an advanced setting in case it differs across builds.
- **Do not activate over fullscreen apps** (new setting, default on): when a fullscreen app is in the foreground (a fullscreen video, a game, etc.), the taskbar is hidden but the cached chevron rectangle still matches that screen area, so moving the cursor there (e.g. onto a video scrubber) would pop the flyout up over the content. The mod now skips activation in that case. The check runs only on the cursor-enter edge, so it costs nothing on idle ticks.

## 1.4.2 ([Jun 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ed888d43cc5bd73a43fd1f2a1c7acc67801f725b/mods/tray-hover-expand.wh.cpp))

Initial release.

## 1.6.0 ([Jun 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4b6102a9d024834632f3f28143a8b7172287f0ef/mods/tray-hover-expand.wh.cpp))

- **Hide the chevron tooltip** (new setting, opt-in, default off): while the flyout is open and the cursor is on the chevron, Windows shows a "Hide" tooltip that covers the bottom row of icons (#4487). When enabled, the mod hides that tooltip window. It is matched both by class and by position (only a popup sitting over the chevron is hidden), so the flyout's own per-icon tooltips and unrelated popups elsewhere are left untouched. The tooltip window class is also exposed as an advanced setting in case it differs across builds.
- **Do not activate over fullscreen apps** (new setting, default on): when a fullscreen app is in the foreground (a fullscreen video, a game, etc.), the taskbar is hidden but the cached chevron rectangle still matches that screen area, so moving the cursor there (e.g. onto a video scrubber) would pop the flyout up over the content. The mod now skips activation in that case. The check runs only on the cursor-enter edge, so it costs nothing on idle ticks.

## 1.4.2 ([Jun 15, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ed888d43cc5bd73a43fd1f2a1c7acc67801f725b/mods/tray-hover-expand.wh.cpp))

Initial release.

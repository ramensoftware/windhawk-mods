## 8.1.8 ([Oct 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9690c990f8bcb9dc98bf5f19dee944138d20d9a5/mods/hide-taskbar-only-on-desktop.wh.cpp))

### Changelog 4 — Version 8.1.8

- Added shell cloak/uncloak event handling for more reliable taskbar updates when Start/Search or other shell surfaces change visibility through DWM cloaking.
- Improved recent-close automatic taskbar-focus handling while preserving the existing same-monitor restriction.
- Reduced false automatic-focus classification by clearing stale close-handoff state when explicit Windows-key taskbar navigation is detected.
- Corrected minimize/restore lifecycle handling so `EVENT_SYSTEM_MINIMIZEEND` is treated as a restore transition.
- Improved completed-minimize detection while preserving transition protection during minimize/restore operations.
- Removed ineffective destroy-time recent-departure bookkeeping.
- Removed duplicate mouse-activation and other redundant state checks.
- Renamed the cloak-related application-window parameter to `allowCloaked` for clarity.
- Removed a redundant transition-validation bounds check and other small pieces of dead or duplicated code.
- Restored focused explanatory comments for the non-obvious state-machine mechanisms.
- Restored the canonical Windhawk tool-mod launcher boilerplate, including Session 0 and service-process exclusions.
- Preserved the Windows Animations, Start menu, cross-monitor, hover, keyboard navigation, fullscreen, and taskbar transition behavior from previous 8.1.x updates.

### Changelog 3 — Version 8.1.7

- Improved Windows Animations close-probe triggering by limiting it to windows associated with an actual close session or animation ghost.
- Removed unnecessary close-probe arming from unrelated foreground, minimize, move/size, show/hide, and destroy events.
- Increased the close-probe interval from 8 ms to 16 ms while preserving the existing close-session burst and grace-period behavior.
- Reduced unnecessary transition-validation and taskbar-integrity polling by limiting rechecks to active or recently active transitions.
- Increased the taskbar-integrity recheck interval from 8 ms to 16 ms.
- Fixed Start menu visibility detection for DWM-cloaked Start menu windows.
- Preserved the v8.1.6 cross-monitor taskbar protection and the v8.1.5 hover, Start menu, keyboard, minimize/restore, fullscreen, and multi-monitor behavior.

### Changelog 2 — Version 8.1.6

- Improved compatibility with the `windows-animations` mod during application close and minimize transitions.
- Prevented automatic taskbar focus after closing an application from being treated as intentional taskbar activation.
- Improved cross-monitor transition handling to prevent inactive secondary taskbars from being shown unnecessarily.
- Removed redundant `MonitorList` plumbing and unused monitor snapshots from the Windows Animations close-session scanner.
- Simplified the monitor intersection helper by removing an unused output parameter and redundant local variables.
- Preserved existing close, restore/maximize, minimize-transition, fullscreen, hover, Start menu, and multi-monitor behaviour.

### Changelog 1 — Version 8.1.5

- Fixed hover reveal being blocked by the transition-time taskbar integrity guard after minimize/close transitions.
- Gave hover reveal priority over transition reassertion while keeping anti-flash protection for unrelated displays.
- Prevented the integrity timer from re-hiding a taskbar after normal state evaluation determined it should remain visible.
- Fixed the empty work-area strip and restored taskbar hover/interaction after application open, restore, and maximize transitions.
- Added explicit Start menu session handling so repeated keyboard Start open/close cycles do not leave the taskbar permanently revealed.

## 8.0.0 ([Sep 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/222fe7db8466b63b488ee78695a9cee03145e3ed/mods/hide-taskbar-only-on-desktop.wh.cpp))

Initial release.

## 0.9.13 ([Sep 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/2bf015383d0f7ea4d5365120ae08c9e6359ca882/mods/mutealert.wh.cpp))

- make an observable SteelSeries mute state authoritative for the Windows input in full synchronization mode
- apply explicit standard-HID unmute actions without requiring MuteAlert to own the Windows mute
- keep call-app unmute transition-only to avoid changing a meeting during startup
- expire queued unmute requests after five seconds

Previously, a Windows mute that MuteAlert did not create prevented an observed physical headset unmute from taking effect. Users had to unmute Windows manually before headset synchronization began working. Full synchronization now follows the observable physical headset state as its source of truth.

## 0.9.10 ([Sep 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/cfa9b1247d303811aded2063004f07525dc1345b/mods/mutealert.wh.cpp))

- recover an unmuted Windows input at startup only when MuteAlert recorded that it applied the preceding headset-driven mute
- keep manual Windows privacy mutes intact and keep call-app unmute synchronization transition-driven
- preserve fail-safe physical-mute enforcement for Windows input and active calls
- organize settings into collapsible General, Headset, Slack, Teams, Zoom, and Google Meet groups

## 0.9.8 ([Sep 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/96af7a4c23f66842ccc6aa627b8cb0d611d81df4/mods/mutealert.wh.cpp))

- Add Google Meet call and mute-state detection through Windows UI Automation.
- Add speaking-while-muted warnings, optional audio cue, taskbar mute/unmute, call-window focus, and headset mute synchronization.
- Add a distinct Google Meet call-state logo and configurable localized control labels.
- Recognize Meet windows in Chrome, Edge, Firefox, Brave, Vivaldi, Opera, and Arc.
- Document that the meeting must be the active, visible browser tab because inactive tabs don't expose their controls to UI Automation.

## 0.9.7 ([Aug 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/accf687cec4460a68914869c4d7fcd998a626265/mods/mutealert.wh.cpp))

Initial release.

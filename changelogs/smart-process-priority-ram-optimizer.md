## 1.0.3 ([Nov 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/568f435119377c2e880191bb82cffe0d5498f2ce/mods/smart-process-priority-ram-optimizer.wh.cpp))

- add WASAPI capture-endpoint audio scanning (microphones/line-in) with 5-second silence hysteresis
- implement network transfer shielding via extended TCP and UDP owner tables
- route early memory-cap sweeps through SystemActionArbiter with fast working-set pre-filtering
- replace hardcoded browser process names with command-line role detection (`--type=renderer` and `-contentproc`)
- preserve full process families in immunity sets to prevent active browser and Electron UI renderers from being trimmed
- add minimize-aware and multitasking-adaptive memory trim grace periods
- throttle runaway background tasks when saturating a single CPU core (>= 80%)
- add borderless full-screen window geometry detection for gaming sanctuary
- preserve original application EcoQoS power-throttling state across throttling cycles
- reduce foreground focus debounce to 200 ms with bounded focus history
- include touchpad driver helpers in essential system security process immunity
- use native Windows SDK `<iphlpapi.h>` definitions and link `-liphlpapi` directly

## 1.0.2 ([Oct 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/25462332c30b60131ce2d02809ed23eecb17fe46/mods/smart-process-priority-ram-optimizer.wh.cpp))

- Decouple immune family processes from handled PID set in memory trim arbiter
- Protect descendants of visible non-minimized windows during working set trim
- Report working set reduction as trimmed bytes rather than ambient memory noise
- Separate hard memory reclamation from soft priority demotion accounting
- Remove residual standby accounting and sort top entries by trimmed bytes
- Prevent side-effecting map mutation in const EvaluatePreOpen arbiter method
- Guard power scheme state persistence against failed PowerSetActiveScheme invocations
- Eliminate hardcoded kNonGames blacklist in favor of native GameConfigStore
- Evaluate D3D exclusive fullscreen and game folders before the packaged app filter
- Avoid game launcher false positives by dropping ambiguous launcher folders
- Add configurable game Alt-Tab grace period setting (default 30 seconds)
- Remove generic -m CLI marker and add jittered TTL to AI inference cache
- Calibrate automatic memory thresholds for systems with 32 GB or more RAM
- Count only actually boosted processes in ProBalance logs
- Fix access-denied count in the unload session report
- Streamline ProBalance process tree logging and normalize log statements

## 1.0.1 ([Oct 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f6287775fe5cae1cda1d96364d744d008dd3db07/mods/smart-process-priority-ram-optimizer.wh.cpp))

Initial release.

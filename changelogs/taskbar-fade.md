## 2.0 ([Feb 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b5a803add78c879b47659306b82418629083ae53/mods/taskbar-fade.wh.cpp))

Performance / Idle Logic: I refactored the main loop to address the CPU usage concern. The mod now separates its logic: heavy window geometry updates are throttled to 1Hz (1 check/s), while lightweight mouse detection remains at 15Hz to ensure the taskbar wakes up instantly without lag.

Documentation: Added a full WindhawkModReadme block with a description, features, and configuration guide.

Improvements: Switched to a Cubic Ease-Out curve for a smoother feel. The animation is now time-based (deterministic), ensuring consistent speed regardless of system frame rate. Also Implemented a new pipeline to handle state transitions, added monitor hotplug detection to handle multi-monitor setups correctly and made some other adjustments.

## 1.5 ([Feb 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a758bce656de5cff159573823319da3fe0e45c38/mods/taskbar-fade.wh.cpp))

Initial release.

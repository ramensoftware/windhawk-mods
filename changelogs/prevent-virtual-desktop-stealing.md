## 0.4.0 ([Sep 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a582b4e5a3122b2ecdd6a12150d45206ae93cb0c/mods/prevent-virtual-desktop-stealing.wh.cpp))

- Renamed the mod.
- Changed the classifier to be rooted narrowly in CVirtualDesktopForegroundPolicy::ForegroundViewChanged, which offers better classification accuracy against normal user actions. 
- Hardening against apps that briefly activate a pre-existing window before creating a new one.

## 0.3.14 ([Aug 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bbaa6505ffc05ac4f3add9b22f0787565ec7ebfd/mods/prevent-virtual-desktop-stealing.wh.cpp))

Initial release.

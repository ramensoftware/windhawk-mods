## 2.0.1 ([Nov 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7a68f9db4ab7ce1f00951688c7381b9b3cc99de3/mods/remove-shortcut-arrows.wh.cpp))

* **Fix Desktop Startup Crash:** Defer icon refresh until desktop view initialization completes to prevent startup crashes on certain Windows 11 builds (thanks to @DavidHiFi).
* **Non-blocking Shell Notification:** Switch to asynchronous shell flush notifications to eliminate UI thread stalls.

## 2.0.0 ([Nov 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/b158ee856a1be9d1cbfa930b45068ab3463242ba/mods/remove-shortcut-arrows.wh.cpp))

* **Improved Desktop Stability:** Shortcut arrows now stay hidden reliably when Explorer recreates the desktop at startup and after system setting or theme changes.
* **Process & Subclass Isolation:** Restricted window hooks strictly to the primary desktop list view, preventing conflicts with other File Explorer windows or separate Explorer processes.
* **Visual Preview:** Added a side-by-side before-and-after preview to the mod description.
* **SEO & Title Polish:** Renamed to "Remove Shortcut Arrow Icons" for better searchability.

## 1.0.0 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/99b2c90b8dcdc892d7bcbe4d5978e47dced3a0bb/mods/remove-shortcut-arrows.wh.cpp))

Initial release.

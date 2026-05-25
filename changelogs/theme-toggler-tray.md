## 1.2.2 ([May 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4859d479f73c86feed28cd9e3b9f50d98c8b5a8b/mods/theme-toggler-tray.wh.cpp))

* Fixed an architectural issue where the tray icon permanently disappears when explorer.exe restarts or the taskbar is recreated (e.g., due to DPI changes).
* Changed the hidden message-only window (HWND_MESSAGE) to a hidden top-level tool window (WS_EX_TOOLWINDOW and WS_POPUP) to ensure it correctly receives the TaskbarCreated system broadcast.
* Refactored the TaskbarCreated handler in WindowProc to safely and robustly reconstruct the icon.

## 1.2.1 ([May 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/acfafad42bba74504b8d8556fc7fe148aa512344/mods/theme-toggler-tray.wh.cpp))

Initial release.

## 1.45 ([Oct 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/069667fd60723342d8decaaf94058f21ef32ff32/mods/app-owned-frame.wh.cpp))

* Fixed the extra system frame returning in Photoshop and other apps while a file open/save dialog or another modal dialog is open, and staying after it closes.
* `WM_NCACTIVATE` now always returns the default window procedure's result, so the optional DWM caption delegation can never block deactivation.
* Hooks use the type-checked `WindhawkUtils::SetFunctionHook`, and string settings use `WindhawkUtils::StringSetting`.
* Setting descriptions name the visible "Use the application's own title bar" option, and Photoshop's default profile no longer enables the inert DWM caption delegation.
* DWM attributes use the named `DWMWA_*` constants.

## 1.44 ([Oct 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6f0a3b091c1e1f00b0fed647227a52a9f9af5313/mods/app-owned-frame.wh.cpp))

Initial release.

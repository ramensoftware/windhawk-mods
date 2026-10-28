## 0.5.0 ([Oct 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a2e932eb009c3476b3ae0de439ee3777fe513af0/mods/macos-enter-rename.wh.cpp))

Extends Enter-to-rename support to selected desktop files and folders in version 0.5.0.

Desktop support is enabled by default and can be disabled in the mod settings. It applies only to the focused desktop icon list with a nonempty selection. Existing File Explorer behavior is preserved, and Open/Save dialogs remain unaffected.

Validation:
- Passed compilation checks with the bundled Windhawk compiler.
- Passed automated checks for desktop detection, modifiers, held Enter, key-up handling, and rename confirmation.
- Manually tested on Windows 11: desktop rename, confirmation and cancellation, held Enter, settings changes, and existing Explorer behavior.

* Added Enter-to-rename support for selected desktop files and folders.
* Added a setting to disable desktop support, which is enabled by default.

## 0.4.0 ([Oct 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9f270d11043b5f657484cc66bcf077aa80b2036d/mods/macos-enter-rename.wh.cpp))

Initial release.

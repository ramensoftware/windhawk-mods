## 1.5.7 ([Oct 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/75cac23324554c3245b5e197a2daa204ebcbb939/mods/explorer-picker.wh.cpp))

* Grant foreground permission to the process that owns the completed Explorer picker, and back to the application before confirmation, control changes or cancellation and after overwrite/warning prompts; assign initial focus before delivering application callbacks.
* Retain IFileDialogEvents and Unicode Win32 initialization/completion hooks with the risky option enabled, so applications which open/save inside OnFileOk or CDN_FILEOK still perform their operation.
* Keep fully supported controls and native panels. Clarify that incompatible optional features can omit the whole extra application UI, while completion events, acceptance vetoes and retained values remain available.
* Retain native fallback for ANSI hooks/templates and Unicode callback contracts which cannot be represented. Remove unreachable plain-dialog hook/template allowances. Update the English/Russian option description and readme.

## 1.5.4 ([Oct 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e107837f61706174ee2d11039349466bb78d2fb6/mods/explorer-picker.wh.cpp))

Initial release.

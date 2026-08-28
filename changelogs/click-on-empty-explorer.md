## 2.7.0 ([Aug 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ffe3c0fad6d8af42c49b64003af32fae5687dc80/mods/click-on-empty-explorer.wh.cpp))

### Bugfixes
- **Verb truncation:** `GCS_VERB` → `GCS_VERBA` — Unicode build was writing wide chars into `CHAR*` buffer, truncating all verbs to 1 character
- **& markers:** `StrContainsNorm` now strips `&` so "Git Bash" matches "Open Git Ba&sh here"
- **SendInput:** merged split press/release calls into single atomic call

### Improvements
- **Perf:** subclass procs skip `CopySettings()` for unhandled messages
- **Debug:** dump shows normalized match text per entry (`→ match:`)
- **Docs:** expanded Context Menu Match section with rules table
- **Open in Terminal:** now launches Windows Terminal directly via `wt.exe -d <folder>` instead of matching the context-menu entry. Works on non-English Windows and survives shell updates that rename the menu verb/text. Requires Windows Terminal to be installed.

## 2.3.0 ([Jun 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ccde902c8764bffba2413670fe38bd4c923200c1/mods/click-on-empty-explorer.wh.cpp))

- **Paste action** (Ctrl+V) — pastes into the current folder view
- **Custom Hotkey action** — each trigger (double/middle/double-middle click) gets an independent key combo field supporting multiple modifiers (Ctrl, Shift, Alt, Win) and all common keys
- Detailed format instructions per custom hotkey field

## 2.1.1 ([Jun 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/17537dd9bf6b29f13a26dfe0cadf8eeecd76a86d/mods/click-on-empty-explorer.wh.cpp))

Initial release.

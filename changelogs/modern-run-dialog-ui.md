## 3.0.4 ([Oct 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/75402edd191fb26b9987dc4b0d60bf49288224ee/mods/modern-run-dialog-ui.wh.cpp))

- Added full compatibility support for both **Windows 10** and **Windows 11** (`x86-64`), anchoring the Run dialog flush above the Taskbar regardless of history item count.
- Added **Liquid Glass** surface theme featuring dual refractive rim strokes, pointer-following specular highlights, glass pill buttons, and automatic 16×16 background luminance contrast adaptation (alongside **Windows Blur** and **Opaque** themes).
- Added **Auto Light / Dark Mode** support (`Auto`, `Dark`, `Light`) and **Italian (`it`)** localization.
- Upgraded the rendering engine to **Direct2D & DirectWrite** (`IDWriteTextLayout`) for crisp `Segoe UI` typography and uniform multilingual diacritics scaling.
- Updated history list interaction: clicking a history item now executes the command immediately instead of only filling the input box.
- Fixed **"Max History Items"** (expanded range to `0–10`) and **"Show Last Executed Time"** not working when Windows `RunMRU` tracking is disabled, isolated stored history per Windows user (`HistoryTimestamps_<SID>`), added non-destructive history clearing (`HiddenRunMru_<SID>` & `Delete` key shortcut) without deleting Windows' native `RunMRU` entries, and hid the clock icon when timestamps are turned off (#6467).

## 2.0.15 ([Oct 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d66466c25b0eeda4428a0294b2726ec94b9c4276/mods/modern-run-dialog-ui.wh.cpp))

Initial release.

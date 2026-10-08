## 2.1 ([Oct 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a2316b7fb9a4d8fe2d9c952113454b20b05079b9/mods/tray-utility-customizer.wh.cpp))

* Added support for Windows 11 26200.9457 / KB5129195, where `SystemTrayFrameGrid` became a `StackPanel`; both panel shapes are classified at runtime and never cached
* Fixed restore-on-disable on an ordered tray panel, where host markers recorded a column that no longer meant anything; hosts now return to their marker's live child index
* Moved the whole settings surface to grouped keys; 1.x settings do not migrate
* Replaced the layout expression, primary axis, group alignment and twelve per-icon nudge settings with one `Arrangement` field; `|` is always horizontal and `,` always vertical
* Added `auto`, which fits the enabled utilities to the taskbar height and logs its generated expression
* Added per-utility `Content` switches and a `Layout.NewItems` policy
* Added `Adjust.PadX` / `PadY` / `OffsetX` / `OffsetY`
* Computed row capacity in DIPs from the real DPI, fixing the automatic shape at 125% and 150% scaling
* Added support for Windows 11's native left and right taskbars: the arrangement follows the edge Windows reports, re-arranges on a move between edges, and keeps Windows' own side-taskbar cells intact; a taskbar another mod rotates is still left untouched
* A written arrangement that names no utility Windows is showing now waits for one to appear
* With Emoji hidden, the lone-icon Emoji fallback no longer claims the touch keyboard's host
* The microphone, camera and location in-use indicators no longer trigger a re-layout of a tray host the mod is not arranging
* A re-arrangement that finds the tray mid-change, such as during a move between edges, now retries instead of leaving the native layout in place
* Restored borrowed elements to their exact previous local value, clearing the property when it had none

## 1.1 ([Jul 23, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c0d2cdf90c41060ac6f8f932491d0fba012ed2cb/mods/tray-utility-customizer.wh.cpp))

Initial release.

## 1.3.0 ([Nov 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1eba3bf6eab6dde86968cfc1cb8ab382f5728655/mods/coin-stock-monitor.wh.cpp))

* Preserves the existing 1.2.1 setting keys and saved values while adding Color preset and Background controls under Appearance.
* Adds coordinated color presets with matching Solid backgrounds; new Custom defaults use Slate colors.
* Combines background mode and sizing into Transparent, Solid - full area, and Solid - fit text. Horizontal padding scales at every DPI; Fit text follows content size and left/right alignment.
* Uses antialiased text in Solid mode and retains color-key-safe text in Transparent mode.

## 1.2.1 ([Oct 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/0e077339c6b1504dc9943f5ed14b7b311baac2de/mods/coin-stock-monitor.wh.cpp))

* `Symbols` and `EntryPrices` are documented with `|` as the separator. Lists saved with commas by older versions still work.
* Entry prices use `.` as the decimal separator. A comma is accepted only as a thousands separator, followed by exactly three digits, so `12,005` means `12005` and `0,45` is rejected.
* Numbers are parsed with the classic locale, so the result does not depend on the regional settings of the system.
* Fixes a wrong percentage change when an entry price had a comma.

## 1.1.0 ([Oct 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/466bc6c1a6e9a7ca517547b61ecaf891ef0eedc6/mods/coin-stock-monitor.wh.cpp))

Initial release.

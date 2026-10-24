## 5.1 ([Oct 24, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f8cc42031b51bfcdb218a3a9a8b1a153168a32f2/mods/mond-desktop-clock.wh.cpp))

* Seconds: new “Show seconds” toggle. Digits use fixed-width cells so the time doesn’t jitter as it ticks.
* Monitors: show the clock on all monitors, the primary only, or a specific monitor number. You can also turn off “Same position on every monitor” to drag and save each monitor’s clock independently.
* Alignment: a global Left / Centre / Right setting, plus a per-row override. The aligned edge stays fixed when weekday or month names change length.
* Settings tidy-up: grouped as Layout → Placement → Weekday → Date → Time → Per-letter, with labelled prefixes and the same order for every row.
* Upgrade note: shadow offsets reset to 2 DIPs, and old weekday letter colours should be re-entered in the new per-letter group. Everything else carries over.

## 3.5 ([Oct 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d546070d720a18714a4dba2eb05c936b2227ea51/mods/mond-desktop-clock.wh.cpp))

Initial release.

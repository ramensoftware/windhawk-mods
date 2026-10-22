## 1.0 ([Oct 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/52517936a7ca67146a155b080dbca63004efed0b/mods/chinese-holiday-calendar.wh.cpp))

- ⚠️ BREAKING CHANGE: Holiday markers are now user-defined rules instead of built-in matching. Existing holiday text and colour settings are not migrated; existing feed URLs still work, but after upgrading please review “Highlight rules” and add any unwanted feed markers to “Text to remove”.
- ⚠️ BREAKING CHANGE: The global “Replace the lunar text” switch is gone. To change colour only, leave the rule’s “Text” empty.
- Added date rules: mark a lunar or solar date directly, without a feed.
- Added an optional Vietnamese lunar calendar, contributed by [@vantruong-dev](https://github.com/vantruong-dev) and [@phatMT97](https://github.com/phatMT97):
  - Day cells show Vietnamese lunar dates, and the header shows the Can Chi year; everything is calculated offline.
- Added a hover tooltip (on by default) for full holiday names that do not fit in a cell.
- Added support for more ICS feeds: recurring events, extra dates, excluded dates, durations, and compressed responses.
- Improved text matching and restoration: feeds that use different Unicode forms now match correctly, and holiday text no longer stays behind after the mod or the system lunar calendar is turned off.
- Renamed the mod to Lunar & Holiday Calendar (ID unchanged); the README and settings are available in Chinese, English, and Vietnamese.

## 0.21 ([Oct 11, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c3f657ac9b83f60e2cb7561b9ab66c5c517ae2bb/mods/chinese-holiday-calendar.wh.cpp))

Initial release.

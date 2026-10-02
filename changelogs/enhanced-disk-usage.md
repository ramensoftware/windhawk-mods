## 1.2.0 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e6b8b838e544d8cd7520efd7ab45581422170e19/mods/enhanced-disk-usage.wh.cpp))

- separate disk bar and text customization toggles
- updated text formatting to support any subset of stats in any order
- independent bold toggles for free, used, total, and both percentages
- added used percentage (%p) and free percentage (%fp) stats
- added unit normalization (ex show 1.5TB as 1536GB, or 512GB as 0.5TB)
- optional unit precision (0-10 decimals when converting units) and separate percentage precision (0-10)

## 1.1.0 ([May 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/87147b198c32a43877d51a7803c3afc9d4a0712e/mods/enhanced-disk-usage.wh.cpp))

middle click to open new file explorer windows wasn't working when "launch folder windows in a separate process" was not enabled. I usually have it enabled so I hadn't caught it until now. This update changes the way windows are refreshed, which avoids a deadlock in that situation.

I've also reordered the settings to flow better/grouped together which I think makes it a bit better overall.

## 1.0 ([May 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/9ffbf99e36e4479e9dcd8a14026aee1840c5abcf/mods/enhanced-disk-usage.wh.cpp))

Initial release.

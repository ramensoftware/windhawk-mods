## 1.5.0 ([Oct 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a9748e5fc7e051d9899daff9d1664b18ea152a83/mods/on-screen-indicator-position.wh.cpp))

* Each indicator can now use its own horizontal and vertical offset, including zero, or keep the main offset.
* Each indicator can be left at the Windows default position on its own; the offsets still apply to it.
* New display profiles change placement by monitor name, number of monitors in use, and indicator.
* New Seconds on screen setting changes how long the indicator stays up, from 1 to 60 seconds. When it is off, the mod leaves Explorer's timers alone.
* Offsets are scaled using the actual scale of the monitor the indicator is on.

## 1.4.4 ([Oct 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/bb8f7a130795448ce79edf3fa5d3e199796b95ec/mods/on-screen-indicator-position.wh.cpp))

* The readme's migration note now names the right setting, Other indicators.
* After a settings change, the log now repeats the warning that the Desktop N popup can't use its own position.
* Tightened the wording of that warning.

## 1.4.3 ([Sep 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6300d15608cf4ff49c749bced382b1ab27cc9340/mods/on-screen-indicator-position.wh.cpp))

* Bug reports and feature requests now go to the mod's own issues page on GitHub.

## 1.4.2 ([Sep 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/6a06dbbff13cacd1cfd4bb530d53ca643b42a019/mods/on-screen-indicator-position.wh.cpp))

* Give the "Desktop N" popup that appears on virtual desktop change its own position, separate from other text indicators.
* New setting to make the indicator disappear straight away instead of sliding off screen.
* Keeps working on future Windows builds where the parts it hooks into get moved around.

## 1.2.7 ([Sep 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c27674120c8e769a26898d151ce4b3a48d0497c0/mods/on-screen-indicator-position.wh.cpp))

* Fixed the per indicator positions, which had not worked at all since 1.2.6. Whatever you set for volume or brightness on its own was ignored and everything used the main position instead.

## 1.2.6 ([Aug 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3a64981cd805fadceec72d0b4db1aecadadc3408/mods/on-screen-indicator-position.wh.cpp))

* Each kind of indicator can now sit in its own spot, so volume and brightness don't have to share a position. Anything you leave alone follows the main position.

## 1.1.4 ([Aug 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/54b0e49c4e5899cc7b3f38f3f0ccbf428857b3c5/mods/on-screen-indicator-position.wh.cpp))

Initial release.

## 1.5.1 ([Nov 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e935dd1c5404ed3916207e33b4a4b4fcbafe45c7/mods/wintips-osd.wh.cpp))

* `Ctrl+Insert`, `Shift+Insert` and `Shift+Delete` now raise the copy, paste and cut tips, not just `Ctrl+C` / `Ctrl+V` / `Ctrl+X`, since terminal windows still lean on the legacy set. `Ctrl+Shift+Delete` stays silent because browsers and editors use it for something else.
* Fixed a card-shaped ghost of the previous tip bleeding into the new card's blurred backdrop when two tips appear in quick succession; the old card is now flushed out of the composed frame before the screen is captured.
* Readme and settings text reworked, including a note that the card also works on Windows 10 and both shortcut sets in the copy / paste / cut descriptions.

## 1.5.0 ([Nov 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/1492292686dabfa975d4165614a4eb3707ce300d/mods/wintips-osd.wh.cpp))

* Eight card languages: Simplified and Traditional Chinese, English, German, Japanese, Russian, Spanish, Portuguese, plus automatic detection from the Windows display language.
* New "Tint opacity" setting for the acrylic material (20-90%, default 55%). The default is more transparent than before - raise it to 85 to get the 1.4.x look.
* The acrylic card border now uses a top-lit gradient stroke instead of a flat color.
* "Language" option labels are now localized; existing language settings keep working.

## 1.4.1 ([Nov 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/85f2f9f7fe7d0488db6034e8d1a8cbb3e6fe4465/mods/wintips-osd.wh.cpp))

* New "Language" setting: follow Windows (simplified Chinese on a Chinese Windows, English on every other language), or pin the card to English or Chinese.
* Setting names, descriptions and the drop-down option labels are now fully bilingual, so a Chinese Windhawk no longer shows English options next to Chinese labels.
* Follow an auto-hidden taskbar: the card lifts as the bar slides out and settles back to the screen edge as it retracts, instead of being pinned to the work area.
* New "Card size" setting (60–200%) that scales the text, icon, accent bar and the ×N badge together.
* "Show on" now defaults to the display with the active window; "Cursor" and "Primary" are still available.
* The follow loop caches the taskbar window and monitor handle instead of walking top-level windows 17 times a second.
* Per-monitor DPI is now read with GetDpiForMonitor, and the taskbar cache is dropped on display or DPI changes.

## 1.2.1 ([Nov 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/468eb61fae183f46914dba7a37528cded99d146b/mods/wintips-osd.wh.cpp))

Initial release.

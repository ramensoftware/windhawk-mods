## 1.5.2 ([Nov 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3a33b42eeb7bf2faacab922f5e9aeeef8a13bb28/mods/wintips-osd.wh.cpp))

* Added an input language tip: Win+Space, Ctrl+Shift, Alt+Shift and Shift inside an IME now show the input language, or the Chinese/English mode inside a Chinese IME. Off by default.
* The backdrop now keeps refreshing while the card is up (Windows 10 2004 and later), so a video or a scrolling page moves through the blur instead of freezing on the frame it had when the tip appeared. The card excludes itself from its own screen grabs, so it is not picked up by screenshots and recordings while it is visible.
* Keys pressed while an administrator window has focus now come from Raw Input, which covers Insert and copy/paste/cut there too, not just the lock keys. The key-state poll stays behind it for remote sessions and virtual keyboards. The "Detect toggles under elevated apps" setting is gone: the integrity level of the foreground process is read instead, and the fallback only runs when it is actually needed.
* Added a topmost guard: a fullscreen game, a recorder or a launcher overlay covering the card is noticed within 100 ms and the card is raised again. When nothing covers it, no other window is touched.
* The settings page no longer has "Detect toggles under elevated apps"; nothing else was renamed, so existing settings carry over.
* Rewrote the mod description.

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

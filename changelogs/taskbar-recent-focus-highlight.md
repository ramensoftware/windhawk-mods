## 0.12.3 ([Nov 27, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a7ff207d21bf1f46785fbd1b30c14bd3fdce1dbb/mods/taskbar-recent-focus-highlight.wh.cpp))

* Highlights now last from when you last used an app (keyboard or mouse input while it was in front), not from when you switched to it, so the app you are working in keeps its highlight.
* taskbar.dll is no longer hooked (12 symbols instead of 26), and preview cards are matched to their windows through Explorer's own card data, improving reliability.
* New icon styles: Plate (the button's own plate, tinted) and Plate with accent (half tint plus an accent border, the look of Taskbar Active Window Highlight) replace Full and Frame. The side bar lost its halo. On multi-window buttons the plates also tint the stacked edge.
* Previews: separate styles for rank 1 and ranks 2+, new Title (a plate over the card's title row) and "with accent" variants (after Taskbar Active Window Highlight), and ranks 2+ can be left unmarked. A previously chosen preview style is kept until you save the settings.
* New option to also mark single-window previews (off by default).
* Ranks 4 and up fade further instead of repeating rank 3.
* Icons are matched by Explorer's own app ID, so two buttons of one program (e.g. Total Commander and its Lister) are told apart; the exclude list accepts app IDs too.
* No native taskbar element is modified any more (the icon size boost is gone), and highlights take no layout space: they follow Taskbar Styler themes that resize the buttons.
* Less work on the taskbar thread, nothing done in Explorer processes without a taskbar, and a quieter log.
* The settings page is reordered: the four Taskbar settings first, the exclude list last. Stored settings are kept.
* Removed settings: icon roundness, glow layers, size boost and "Only apps on the taskbar" (always on now).

## 0.11.1 ([Oct 6, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5380aa37205a3760e2a49d49c62f23e88561abdd/mods/taskbar-recent-focus-highlight.wh.cpp))

Initial release.

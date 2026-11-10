## 0.6.2 ([Nov 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/87d003102cd41f3f7d71e51b44ab84ce8a0f1d7f/mods/outlook-refined.wh.cpp))

* Credit the author in the readme, with a link to their site.
* Align the agenda and its divider with the favorites list above them. The agenda was positioned against the navigation pane while the list sits inside Outlook's own scrolling container, so the two halves of the pane never shared an edge.
* Give the folder names back 18px. The element holding the folder tree reserves a scrollbar gutter while its own overflow is hidden, so nothing is ever drawn in it; releasing the reservation takes the pane from 163px of usable width to 181px on a 197px pane and leaves 8px on both sides.

## 0.6.1 ([Nov 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e4a1aeceacc4da99b4b2f1848ef84b6655ee5250/mods/outlook-refined.wh.cpp))

* Favorites collected by the Outgoing and Junk groups are labelled with their own account when no other account has a favorite of the same name. They were labelled with the primary account.

## 0.6 ([Nov 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/990c27e145efb3be3be4a934663578ea209ed651/mods/outlook-refined.wh.cpp))

Initial release.

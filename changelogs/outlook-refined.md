## 0.6.5 ([Nov 13, 2026](https://github.com/ramensoftware/windhawk-mods/blob/63b4770b5de06f02844e2d0e9cb283d281dd8819/mods/outlook-refined.wh.cpp))

* A changed setting now reaches open Outlook windows the next time you switch to them, with no restart.
* With "Calendar, times on event tiles" on, a meeting that is about to start keeps its title on its tile. Outlook adds a row holding only the Join button before the title, and that row was taken for the title, so the tile showed only "Join".
* Stop the calendar's month and year label clipping when the navigation pane is narrowed. The button carries a 20px left indent on top of its own padding, which at a 180px pane leaves it 91px while the longest labels need 104px, so the text was cut off mid-word under the header's hidden overflow. Dropping the indent gives the button 111px and the label fits at every pane width the mod allows.
* Double-clicking an event in a merged tile opens it, and the merged tile shows how long each event runs.
* Right-click and double-click on a row of a merged tile act the way they do on an Outlook event tile.
* The readme says where to turn on the agenda, the month grid and the Other inbox card.

## 0.6.2 ([Nov 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/87d003102cd41f3f7d71e51b44ab84ce8a0f1d7f/mods/outlook-refined.wh.cpp))

* Credit the author in the readme, with a link to their site.
* Align the agenda and its divider with the favorites list above them. The agenda was positioned against the navigation pane while the list sits inside Outlook's own scrolling container, so the two halves of the pane never shared an edge.
* Give the folder names back 18px. The element holding the folder tree reserves a scrollbar gutter while its own overflow is hidden, so nothing is ever drawn in it; releasing the reservation takes the pane from 163px of usable width to 181px on a 197px pane and leaves 8px on both sides.

## 0.6.1 ([Nov 7, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e4a1aeceacc4da99b4b2f1848ef84b6655ee5250/mods/outlook-refined.wh.cpp))

* Favorites collected by the Outgoing and Junk groups are labelled with their own account when no other account has a favorite of the same name. They were labelled with the primary account.

## 0.6 ([Nov 5, 2026](https://github.com/ramensoftware/windhawk-mods/blob/990c27e145efb3be3be4a934663578ea209ed651/mods/outlook-refined.wh.cpp))

Initial release.

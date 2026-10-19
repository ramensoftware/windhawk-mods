## 1.1.0 ([Oct 19, 2026](https://github.com/ramensoftware/windhawk-mods/blob/3275a4ab62bd8c754a1820acf3cdff814fe08575/mods/custom-cached-context-menu.wh.cpp))

### Added
- Top action bar (opt-in): Cut, Copy, Paste, Rename and Delete as an icon bar on the side of the menu nearest the cursor, like the Windows 11 menu. Unavailable actions are hidden from the bar.
- Paste into a single right-clicked folder, like the Windows 11 menu.

### Fixed
- Middle-clicking a folder no longer fails to open it in a new tab (invocation-only menu builds are no longer deferred).
- The icon cache no longer runs its destructor at process exit.
- Items that report SFGAO_FOLDER but are not directories (for example .zip files) no longer get the folder menu and its Paste.
- Actions no longer wait for the shell menu: Open, Cut, Copy, Delete, Properties and Set as desktop background are dispatched directly, and registered extension verbs (Share, Print, Open in Terminal and others) run through the shell's association lookup, so clicking an action starts it immediately instead of after a short pause with a busy cursor. The shell menu is only populated for items that exist only inside it.
- The menu disappears immediately when an action is chosen instead of playing the closing animation, so nothing lingers on screen while a modal dialog (Properties, the delete confirmation) is up.
- Cut and Copy run the file view's own command when the view owns the keyboard (guarded so the keys can never land in another window), keeping the cut dimming, the shell clipboard data object and undo exactly like the native menu; otherwise they write the clipboard directly. Paste drops the clipboard object on the target folder's drop target — the shell's own paste path — so collision naming, undo and virtual clipboard items match, including for a right-clicked folder and the navigation tree.
- Hover highlights track the mouse immediately during fast sweeps: interactive repaints no longer wait for vsync, and a burst of mouse moves paints only the final hover position.
- Windows items that are not moved into the submenu no longer jump to the top of background menus; they stay with the other extras above the "More options" entry.

### Changed
- Theme names in the settings now match the bundled theme previews.
- Default padding tightened to 4; existing configs and theme files are migrated when they still carry the old default (schema v9). Right-aligned content now mirrors the icon's left inset.
- The "Windows items to move" default list uses language-independent verbs where they exist instead of English labels (labels still work as a fallback); commands whose verb is a GUID accept friendly aliases (openterminal, editwithphotos, editwithpaint, editwithnotepad), and Windows' modern commands are no longer treated as third-party handlers.

## 1.0.0 ([Oct 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d150b4e7044c23221be5dbc2bfb581789fe36dce/mods/custom-cached-context-menu.wh.cpp))

Initial release.

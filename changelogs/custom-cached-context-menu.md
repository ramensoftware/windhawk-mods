## 1.2.0 ([Nov 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/62ce51acd0fe918b84e73bddcfa47f4f609891dc/mods/custom-cached-context-menu.wh.cpp))

### Added
- Localization for 36 languages: Arabic, Azerbaijani, Bengali, Czech, Danish,
  German, Greek, English, Spanish, Persian, French, Hindi, Croatian, Haitian
  Creole, Hungarian, Indonesian, Italian, Japanese, Korean, Norwegian, Dutch,
  Odia, Polish, Portuguese (Brazil and Portugal), Romanian, Russian, Swedish,
  Tamil, Thai, Turkish, Ukrainian, Uzbek, Vietnamese, and Chinese (Simplified
  and Traditional).
- The menu and the in-menu settings editor follow the Windows display language
  by default. A new Language setting can override it without changing the
  Windows language. Shell-extension labels stay in the language the extension
  supplies.
- The Windhawk settings page follows Windhawk's own language: setting names,
  descriptions and option labels are localized through Windhawk's metadata
  annotations.
- Right-to-left layout for Arabic and Farsi: mirrored menu geometry, submenu
  side, hit-testing, slider direction and keyboard Left/Right, with text drawn
  in the correct reading direction.

### Fixed
- On non-English Windows, New, Display settings and Personalize no longer
  appear twice (once in the mod's English labels and once in the system
  language). Native items are matched by verb, known shell labels, or
  OS-language New child names instead of translated text.
- Existing menu.ini configuration keeps working on a localized menu: core
  items retain canonical English labels for label predicates, per-item
  overrides, advanced-submenu matching and move destinations.
- The New submenu uses stable template identities and a locked snapshot, so a
  language change cannot reorder the list into creating the wrong file type.
- The cache records the Windows UI language and rebuilds after a system
  language change instead of showing stale labels.
- Multi-selection now uses the stock "Open" label instead of "Open N items",
  avoiding per-language plural rules.

### Changed
- The menu and settings editor are no longer English-only. Native translations
  can be corrected in the mod's source repository and regenerated; the
  submission still contains only the single mod file.

## 1.1.1 ([Oct 26, 2026](https://github.com/ramensoftware/windhawk-mods/blob/4d0e9c036ca788eae9817ed8d7b28714cac6e97f/mods/custom-cached-context-menu.wh.cpp))

### Added
- A "Background pre-build" setting (on by default). Turning it off skips the
  startup warm-up, which is an extra moment where every registered context-menu
  handler is loaded; handlers then load only for menus that open, as with the
  stock menu. This avoids startup crashes from faulty third-party extensions,
  while a handler that would crash the stock menu still does when its menu is
  built. The first open of each context builds on demand; later opens stay
  instant. The mod's own New submenu and Send to entries are unaffected: their
  lists are built by the invalidation thread, which always runs.

### Fixed
- Extension items on a folder background now invoke their verb on the current
  folder directly when the folder registration runs the same command, instead
  of going through the deferred menu population.
- Labels that embed the selected file's name (for example NanaZip's
  "Extract to "<name>"") are cached as templates and re-rendered with the
  current name, so they no longer show a previous file's name and their
  invocation matches again. Only the start of a quoted label segment is
  templated, and folders keep their full name. Existing caches rebuild once
  after the update.
- The per-thread bitmap and owner-subclass buffers are released on unload
  instead of staying on Explorer's long-lived UI threads.

### Changed
- Debug logging now reports when a live-menu match was found but its invocation
  failed.

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

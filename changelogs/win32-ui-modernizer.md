## 1.0.2 ([Jul 30, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ba5ce914896f8bec55bb51db2e9d0822e6a4afa7/mods/win32-ui-modernizer.wh.cpp))

- Fixed some WinUI apps failing to open like Photos and OneDrive
- Fixed headers missing it's dividers on dark mode
- Fixed the placeholder text on the rebar search bar not appearing on dark mode
- Fixed more DPI unaware controls
- Fixed the File Explorer navigation pane glyphs not rendering correctly when the translucent compatibility option is on
- Fixed the Recycle Bin glyph not displaying the full and empty variants accordingly
- Excluded the .msi installers from the mod as requested
- NEW: Added an option for hover/hot fade animation on the navigation pane
- NEW: Added a hover/hot state to the Registry Editor tree view
- NEW: All tree views now get a hover/hot state
- NEW: The selected state for tree views and list views now display a fade animation
- NEW: Added an option to switch the Registry Editor tree view icons with glyphs
- NEW: Revamped the Property Sheets windows design to match the new WinUI look that's currently being tested by Microsoft

## 1.0.1 ([Jul 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c3138536027c3d55ba5efa387085c42d856f7394/mods/win32-ui-modernizer.wh.cpp))

- Fixed the mod getting stuck on unloading
- Fixed the glyphs blocking the File Explorer thread while animating
- Made the glyphs use direct composition when inside File Explorer
- Fixed black background behind glyphs
- Styled the Registry Editor separator
- Styled the old address bar's overflow chevron
- Enhanced text rendering
- Added a new Fade animation option for the navigation panel pill
- The buttons and combo boxes styles are now closer to the winui ones
- Changed the focus border option into a drop down and fixed the style not being applied
- Added styles for missing controls
- Corrected non dpi aware styles
- Fixed the "Optimize Drives" window not being painted dark
- Fixed leaks
- Removed dead code
- Hardened glyphs detection

## 1.0.0 ([Jul 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d9bb22b73df373844b864a87299693353035e34e/mods/win32-ui-modernizer.wh.cpp))

Initial release.

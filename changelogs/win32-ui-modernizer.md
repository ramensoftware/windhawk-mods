## 1.0.3 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/50f4b88cfb44c00344966688db3b0a275614857b/mods/win32-ui-modernizer.wh.cpp))

### Added
- Mica surface for File Explorer, with content-pane and opaque variants
- Open/Save dialogs follow that surface, with matching shades, dividers and scrollbars
- Collapsible navigation pane, with an animated hamburger button and an icon rail
- Optional automatic collapse the way WinUI's NavigationView does it
- Expand and collapse animation for navigation pane folders
- Multi-color Fluent icons for the navigation pane
- Modern shortcut overlay arrow
- DirectWrite text in Explorer, file pickers and desktop icon labels
- Single-line property-sheet tabs
- Insertion mark extended to list views and the desktop
- About dialog also covered in the Registry Editor and MMC consoles, with an animated logo
- Accent logo in the Alt+F4 "Shut Down Windows" banner
- Acrylic menus in light mode
- Experimental MMC support
- Mica background for the Registry Editor, with its value list as a card
- Task dialogs laid out like WinUI's ContentDialog
- Buttons, check boxes, radios, ComboBox and list selection restyled from WinUI's own templates
- Control borders antialiased with WinUI's corner radius, matching the modern GroupBox
- Legacy address and search boxes styled like the other text fields
- Progress bar colors taken from WinUI's ProgressBar
- Control Panel home page text in WinUI colors
- Dark surfaces on WinUI's tokens: #202020 base outside File Explorer, #2B2B2B for property sheets and tab panes
- Group headers without the separator line, like current File Explorer
- Drag and drop redrawn with rounded backgrounds, Fluent action badges and DirectWrite text
- Disk usage chart redrawn as an animated WinUI ring
- Hover pill on hyperlinks, modern rename editors
- Pill, selection and ReBar glyph motion retimed from WinUI's own keyframes
- Scrollbars continue the background of the control they scroll
- Trackbars fill the track up to the thumb, in accent, like WinUI's Slider
- Trackbar track and thumb take their colors and elevation border from WinUI's Slider
- Simplified Chinese translation, by SS3-4001
- Related settings merged into fewer options

### Fixed
- Dark mode now changes 7 system-wide colors instead of 30, the rest resolved in-process
- System colors restored on unload, including entries left behind by earlier versions
- Threads, timers and window changes left behind after the mod unloads
- Crash when a file picker's breadcrumb drop-down closed
- Crash when reloading the mod inside Explorer
- Registry Editor freezing on locales that parse decimal numbers differently
- Registry Editor tree scrollbars opening white or keeping stale pixels after a resize
- Dark mode following the system theme instead of the app theme
- Dark mode forcing the modern scrollbar on, with no way to turn it off
- Dark mode leaking into excluded processes, including accent colors resolved from the system instead of the window
- Disabled and read-only fields painted dark in excluded processes
- Edit focus line using the dark accent in light apps under a dark system
- Primary buttons enabled after a dialog opens missing their accent fill until hovered
- Progress bar fill using the system accent variant instead of the window's
- Progress bar track not scaling with the DPI
- Navigation pane chevron and pill animations interfering between windows
- Desktop icons covering the wallpaper with an opaque dark fill
- White backgrounds in the permissions dialog and on the second AeroWizard page
- Seam around a focused edit in ComboBoxEx32, in both light and dark
- Menu borders never reaching light mode
- Tooltip border color never being applied
- "Previous Versions" DirectUI list staying light
- Legacy address bar breadcrumb repaint desync and drop-down recursion
- Modern borders drawn around scrollbars
- Tooltip text rendering over acrylic
- Black hover text on tree items in dark mode
- Gradient showing through the property-sheet tabs of the permissions editor
- Dark system colors not restored after an external theme change
- A dialog's own background brush being overridden in dark mode
- The mod reverting Explorer's app-mode preference on unload
- Shared color data opened to any process instead of the current user and SYSTEM

### Performance
- Hot paint paths moved off Direct2D to GDI+ and direct pixel writes, idle animation threads blocked instead of polling, accent/theme lookups cached, and private symbols resolved once per module

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

## 1.3.2 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/465432f8048ba7d081947718b405b51f479e98f3/mods/overhaulded-alt-tab.wh.cpp))

## Added

* Arrange open windows in an adaptive grid instead of the traditional horizontal carousel.
* Automatically organizes cards into balanced rows, using up to four columns when space allows.
* Grid: Navigate between cards using the arrow keys, with smooth selection animations.
* Smooth scrolling with indicators when additional rows are available.
* Added System, Dark and Light themes. System automatically follows the Windows app theme.

## Improved

* Window management:
* Added smooth card repositioning when the window list or layout changes.
* Improved selected-card scaling and visual transitions.
* Refined the shared transparent background surface.
* Improved the restoration of minimized windows when activating them.
* Improved screen-coordinate handling for mouse interactions.

## Other changes

- Added additional activation diagnostics.
- Improved timer error handling and logging.
- Updated settings and documentation.
- Added the required Windows system library dependency.

## 1.3.1 ([Nov 8, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5b1d0cdb276170cb6da30d20e314813571cd9b5a/mods/overhaulded-alt-tab.wh.cpp))

### Added
* Added configurable card motion speed (100% by default).
* Added independent animation controls for navigation and selector transitions.
* Added a toggle for the background mouse spotlight.
* Added Dots and Squares shapes to Pattern Waves.
* Added Fullscreen background

### Changed
* Refined carousel motion styles: Smooth, Linear, and Bouncy.
* Improved card transitions, animation timing, and synchronization with background effects.
* Adjusted Pattern Waves animation speed and reorganized animation settings.
* Made card visibility fully automatic, replacing the Visible Cards and Single Window Card settings.
* Optimized card layouts based on window count, displaying up to five cards in the carousel.

## 1.3.0 ([Oct 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/190eebae832796ad8fc34dbe0930dfb398ccf5b8/mods/overhaulded-alt-tab.wh.cpp))

* Added a new DWM transparent background system with configurable color and opacity.
* Added experimental **Pattern Waves** background with cursor interaction.
* Added experimental **Window Expand* animation on selected window.
* Added UI scaling from 50% to 125%.
* Added customizable card colors.
* Improved Alt+Tab and AltGr input handling.
* Improved animation smoothness using hardware-accelerated Direct2D rendering.

### Changed
* Replaced the old CPU-based desktop blur with the new DWM background system.
* Improved DWM thumbnail management and rendering efficiency.
* Refined opening, closing, and carousel animations.
* Reorganized settings into clearer categories.
* Improved background rendering performance on older hardware.

### Fixed
* Fixed unnecessary DWM thumbnail updates.
* Fixed animation and clipping inconsistencies.
* Improved hook initialization and input reliability.

### Note
* Pattern Waves is experimental and may require additional system resources.

## 1.2.1 ([Oct 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/44918b3fcd5111318f1ebf7d663b788826688f95/mods/overhaulded-alt-tab.wh.cpp))

Initial release.

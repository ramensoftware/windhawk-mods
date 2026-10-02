## 1.7 ([Oct 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/128938b0170236340ac149a6dddb1a6860e78eee/mods/desktop-window-cards.wh.cpp))

## Desktop Window Cards 1.7

### New
- **Drag & drop onto cards** – hold a dragged file, text or image over a card and its window opens, ready for the drop, just like taskbar buttons. The app handles the drop natively (copy/move into an Explorer folder, attach to an e-mail, open in an editor...). The delay is adjustable, or 0 to disable it.
- **Multi-monitor** – dropping a card on another monitor moves its window there too, centered, while it's still minimized. It opens there however it's restored (card, taskbar, Alt+Tab). In grid layout the card joins that monitor's grid where it's dropped.
- **Grid layout (optional)** – a Task View-style grid per monitor, with cards as large as the space allows. Drag cards to reorder them; with "Remember the position of moved cards" they return to their place the next time their window is minimized.
- **Hover zoom (optional)** – resting cards grow slightly under the mouse with a springy motion and rise above their neighbors.

### Improved
- **Smoother animations** – on Windows 11, frames are synchronized with the compositor clock (`DCompositionWaitForCompositorClock`), with the high-resolution timer as fallback. Grid rearrangements use a critically damped spring, so cards that get a new destination mid-motion keep their speed instead of restarting, and falling cards are redirected mid-air without jumps.
- **Virtual desktops** – cards follow their window's virtual desktop (windows pinned to all desktops keep their card everywhere). On Ctrl+Win+Arrow switches, cards of the desktop being left are hidden at the key press instead of after the shell's slide animation, so they no longer linger on the new desktop.

### Fixed
- **Content shift when a card hands over to the restored window** – cards used the system frame metrics to crop the invisible resize borders, which don't match apps that draw their own frame (browsers, WinUI/XAML, Electron...), and assumed the wrong layout for the thumbnail source on recent Windows builds. The mod now measures each window's real borders (window rect vs. `DWMWA_EXTENDED_FRAME_BOUNDS`) when it's activated, moved or restored, and crops the thumbnail 1:1, so the card lines up with the real window to the pixel.
- If a restored window still doesn't land exactly where the card ended, the smooth-reveal snapshot fades out quickly instead of showing both images side by side, and the mismatch is logged.
- Cards no longer slide underneath other cards while being dragged.

## 1.5 ([Oct 1, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e720a2611fdcaad46cf05cbe608b24526a74493e/mods/desktop-window-cards.wh.cpp))

- grid layout 
- virtual desktop support

## 1.3 ([Sep 28, 2026](https://github.com/ramensoftware/windhawk-mods/blob/651e01908512da9fa4935a9ef2859741c7e69240/mods/desktop-window-cards.wh.cpp))

Initial release.

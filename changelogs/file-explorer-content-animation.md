## 1.0.2 ([Oct 29, 2026](https://github.com/ramensoftware/windhawk-mods/blob/a0fb63a19b61b5169b38433d5b72357cd84d9f3e/mods/file-explorer-content-animation.wh.cpp))

- Rewrote most of the mod: DirectUI pages now animate through DirectUI's standard layer mechanism and run on the compositor, staying smooth while thumbnails and large folders load.
- New WinUI defaults: 450 ms timeline and 140 DIP entrance; entering a folder uses the entrance transition.
- Optional staggered item reveal with WinUI AddDelete curves, plus a short exit for deleted items.
- Pages wait for the first files to appear instead of animating an empty view.
- Clicking or pressing a key finishes the animation quickly; scrolling hands the content back at once, including with smooth scrolling mods.
- The mod no longer loads in the Control Panel process.
- The DirectComposition fade now also plays when opening Explorer, new tabs and the navigation pane.

## 1.0.1 ([Jun 2, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ff269b026fa0e4628a625cbfb59da891642bb872/mods/file-explorer-content-animation.wh.cpp))

- GPU-accelerated fade - The fade effect now uses a DirectComposition overlay capturing the previous folder's content (including Mica/Acrylic). The overlay fades out on the GPU compositor thread, fully independent of CPU load from thumbnail rendering.
- Page transitions - Detects the navigation type and applies a different animation for each context:
Entrance (slide up) - Opening Explorer, new tabs, and general navigation.
Enter folder (short fast slide from right) - Navigating into subfolders.
Back slide (slide from left) - Going back via button, breadcrumb, or Up arrow.
Forward slide (slide from right) - Going forward after going back.
Reverse direction - Content can slide down instead of up.
- Navigation pane animation - The nav pane tree slides in from the left when opening Explorer or a new tab.
- Property page animation - Animates tab switching in folder and file Properties windows.
- Requires symbol resolution on explorerframe.dll (handled automatically by Windhawk).

## 1.0 ([Mar 4, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d474c7f1b77392ec9794d915f1b707a54e81164d/mods/file-explorer-content-animation.wh.cpp))

Initial release.

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

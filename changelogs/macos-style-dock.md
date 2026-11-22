## 1.2.0 ([Nov 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/7454669d26ffd162e4a663d4e420069b70227117/mods/macos-style-dock.wh.cpp))

* New: Liquid Glass (off by default). The dock's rim bends what's behind it like real glass, instead of only a frosted blur. Turn it on by picking Lens, Reflection or Stretch in "Edge Style", and adjust "Edge Depth", "Edge Strength" and its own "Liquid Glass Blur".
* Liquid Glass has two downsides: it trails the screen behind the dock by a frame or two, and while it's on, the taskbar is hidden from screenshots, screen recordings and screen sharing. If either is a problem, set "Edge Style" to Off for the plain frosted glass.
* New shortcuts while Liquid Glass is on: Win+Ctrl+Shift+G freezes the glass so the taskbar shows up in screen captures, and Win+Ctrl+Shift+K turns Liquid Glass off at once in an emergency, until the mod's settings are saved again.
* New: the system tray gets a background. By default it matches the dock (its glass, Liquid Glass, border and glow) as a smaller pill. Turn off "Match Dock Style" to give it its own color, opacity, blur and border. "Tray Padding" sets the room around its icons and clock.
* New: per-pixel brightness limits behind the dock and the tray, on by default at 80, so bright windows behind them are dimmed and the tray's clock and icons stay readable. Under dark text it becomes a minimum brightness instead.
* The tray's text and icon shadow can now be adjusted: color, opacity, blur and offset.
* The tray now slides in and out of its hot corner without fading.
* New dock options: "No Background", a Custom border (color, width and opacity), and None for "App Activity Indicators".
* New dividers: one before the Recycle Bin (on by default), and up to 5 more at positions of your choice with "Custom Index Dividers".
* New "Keep Icons Still" option, which turns off the bounce Windows gives icons as apps open, close, minimize or are restored.
* Start can now stay in the dock: turn off "Hide Windows Start Button" to keep it at the front, with Windows' icon or your own ("Start Menu Icon").
* "Squircle Color" has new options: Always Light, Prefer Light, Prefer Dark and Always Dark.
* Custom icons now accept Mac .icns files as well as PNG and other images, at any size. Each icon now has a single "Margins" setting (-50 to 50) instead of a margin and a size.
* Squircle icons: fixed icons showing another app's icon and icons that never got their tile. As a last resort, a tile is now made from the app's icon as Start shows it.
* Fixed the dock and hot corners staying blocked after an exclusive full screen game was minimized or switched away from.
* Settings are reorganized into new groups (Dock Size, Dock Color, Liquid Glass, Tray Color, Dock Items, Custom Icons, Hot Corners, Additional Settings). Settings saved in 1.1.2 don't carry over, so the mod starts from its defaults once. This is intended.
* New defaults: "Magnification" 60 (was 70), "Dock Glow Spread" 8 (was 6), "Dock Glow Opacity" 63 (was 60).
* The readme has a new Liquid Glass section near the top, links to macOS icon resources, and a new section on the system tray.

## 1.1.2 ([Nov 12, 2026](https://github.com/ramensoftware/windhawk-mods/blob/eb317fa97233903f7cf5e1cb5d703debe7ecb248/mods/macos-style-dock.wh.cpp))

* New: macOS-style app name labels on hover
* New: hot corners for Task View or Start (bottom left) and the system tray (bottom right)
* New: Auto-hide, either Off, On, or Hybrid (hides while a window is maximized or snapped)
* New: in Hybrid, a window dropped with its title bar under the dock moves back up
* New: Task View hides the dock and shows its row of desktops at the bottom
* New: squircle icons, drawing app icons as macOS-style tiles, with a Squircle color setting
* New: your own File Explorer and Recycle Bin icons from image files, with margin and size settings; they're named Finder and Trash
* New: running indicator styles: round dots, Hybrid dots, or Windows 11's own
* New: dock edge styles (macOS, Standard, None) with an adjustable inner glow
* New: custom dock color, spacing settings and a high resolution icons setting
* New: soft shadow on the system tray's icons and clock
* New: "Keep File Explorer first" setting, on by default; turn it off to leave your pinned order alone
* New: ARM64 builds (not yet tested on an ARM64 PC)
* Improved: Apple's squircle corners on the dock
* Improved: round running dots, the same size at any dock size
* Improved: File Explorer is now truly moved first and stays there
* Improved: smoother dragging; apps stay between File Explorer and the Recycle Bin, and nothing jumps on release
* Improved: the app name label uses less CPU while icons animate
* Improved: the log names any feature whose hook failed to install
* Improved: rewritten readme, with a "Will it work on my PC?" compatibility guide
* Changed: Start is no longer in the dock; use the bottom-left corner or the Windows key
* Changed: window previews open on click; turn on "Window previews" to also show them on hover
* Changed: magnifying over maximized windows is always on; Auto-hide replaces "Magnify over windows"
* Changed: "Tint" is now "Opacity", with new defaults for size, magnification, blur, color and the divider after File Explorer
* Fixed: clicks and file drops on the Recycle Bin now work wherever its label shows
* Fixed: at Size 40 and below, clicking near an icon's edge could open the app next to it
* Fixed: the tray keeps the taskbar's height on SystemTray.dll 2609.4001 and later
* Fixed: the system tray kept the dock's height after turning the mod off
* Fixed: the taskbar could end up behind windows at startup

## 1.0 ([Oct 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/c89ca2c9981682990720394ed651bcc0d5d717d5/mods/macos-style-dock.wh.cpp))

Initial release.

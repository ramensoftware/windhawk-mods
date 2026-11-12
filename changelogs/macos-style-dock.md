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

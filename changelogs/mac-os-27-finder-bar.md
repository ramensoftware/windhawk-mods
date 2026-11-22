## 1.1.1 ([Nov 22, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f25af3d2731fb9672e90d7538cc8c71b12c0ddb6/mods/mac-os-27-finder-bar.wh.cpp))

* Stacked notifications: in the floating layout, notifications from three apps or more are stacked, with the newest in front. The stack follows the theme, and the front card hides the cards behind it, which show only at the bottom edge.
* The mouse wheel turns the stack to the next notification.
* Dragging the front card aside dismisses it and shows the next one.
* A click spreads the cards into the list with a spring animation; "Show less" stacks them again.
* Scrolling the notification list, with the wheel or by dragging, is smooth. Cards pile up at the top and bottom edges instead of being cut off, as on Android.
* The bell turns red when a notification arrives, like the microphone and camera icons. This replaces the red dot.
* Notifications and notice pills are now separate switches. "Receive app notifications" (formerly "Windows notifications") is in the Notifications button group, and with the pills off, notifications still go to the bell. "Only the bar shows notifications" needs the pills, so it shows only when both are on.
* Selection gradient: an optional second selection color blends the bar's selection pill and the menus' selection from one color to the other.
* New "Test notifications" button in Menu Bar Settings. Each click sends a test notification from a different app.
* Fix: the notifications panel now catches the mouse wheel and clicks over very clear glass. Before, they went to the window below.
* README: demo of the stacked notifications.
* "Clear" in the notifications panel slides the cards out one after another before removing them.
* The bar slides up and down when it hides and shows (with the handle and with "Hide when a window is maximized").
* With "Hide when a window is maximized", pointing at the top edge shows the bar over the window until the pointer leaves, as on macOS.
* Settings backup: export the Menu Bar Settings to a file and import them again.
* Num Lock and Scroll Lock pills, like Caps Lock.
* Restart, Shut Down and Sign Out ask in a glass dialog.
* Fix: the notice pill set to follow the bar had no glass with the Glass background and "live".
* Fix: with the handle on, leaving full screen no longer shows the bar.
* Fix: after the active app is minimized or closed, the bar goes back to "Finder".
* Fix: moving between the bar's menus no longer flickers the app's window.
* Fix: a full-screen video on another display now hides that display's bar.
* Fix: optional updates (drivers, previews) are no longer counted as pending.
* Fix: Windows' own volume, network and power icons no longer show in the bar's tray.
* Fix: video thumbnails (YouTube) are cropped instead of stretched.
* Fix: connected Bluetooth LE devices (mice, keyboards) now show in the Bluetooth menu and pill.
* Fix: clicking the bell or Now Playing on another display while the panel is open.
* Fix: "System Settings" is now "Ajustes do Sistema" in Portuguese.
* Now Playing: the cover is enlarged once in high quality, and video thumbnails show whole over a blurred copy of themselves instead of a cropped, stretched middle.
* The bar slides in more smoothly with the handle; windows move down once it's in place.
* Fix: resting the pointer on a bar button could hide the bar or make it reload over and over on setups with displays at different scales.
* Settings backup: the import replaces all the bar's settings, and the copy now includes "This window"'s look and the weather widget's glass.

## 1.1.0 ([Nov 16, 2026](https://github.com/ramensoftware/windhawk-mods/blob/ecdb02582e9b7ac40ba466ebce21381d4249e2dc/mods/mac-os-27-finder-bar.wh.cpp))

* New **Custom liquid glass** theme (from Siri Plus Launchpad): your color at the top, fading into clear glass, with a fine line of light along the bottom edge. It works for the panels, the bar's background or both. The color (presets or any color), the strength and how far down the color goes are set in Menu Bar Settings.
* New **Light**, **Dark** and **OLED** solid themes.
* Themes everywhere: the bar's theme also styles the menus, the Control Center, the panels and the pills. New **Panel theme** option gives the menus a theme of their own (with the Clear bar, this choice is also in Customize theme).
* New **Customize theme** section (replaces "Glass"): it adjusts the bar's theme, and each glass theme keeps its own settings (transparent ↔ frosted, edge refraction, background blur, live), with a button to restore that theme's defaults. Without "live", Glass and "Window color" use a still, blurred picture of the background.
* Bar background per display: **Customize per display** gives each connected display its own background and text color.
* Notice pills: style follows the bar, or glass, liquid glass, custom liquid glass, light, dark or OLED; text color (theme, white or black); "Live background" for the glass styles.
* Control Center: the cascade opening now slides each row out from under the row above.
* Control Center: new "When the Control Center closes" option: cascade, shrink, fade, slide up or none.
* Notifications: new **Notifications** choice: **Floating** (default: the top and each notification in its own glass card, in the Control Center's style, following the theme) or **Inside the box**. Text color: theme, white or black. The bell is now one of the bar items.
* Notifications: a round picture on each card. It's the sender's photo when the app saves it on the PC (with the app's icon in the corner), otherwise the app's icon. Icon shape: circle or the macOS icon shape.
* Notifications: a Reply button on the card for apps that support replies. Clicking a card opens it in the app.
* Notifications: your replies stay in the card until you clear it, and new messages from the same conversation go into the same card, in order.
* The volume and brightness pill shows the percentage.
* Bluetooth: the Bluetooth pill can search for new devices and pair them, without opening Settings. The pairing code shows in the pill (confirm it, or type it on the device), and a failed pairing shows the reason code. Devices that ask you to type a PIN on the PC still need Settings.
* Now Playing: it stays in the bar and the Control Center while the music is paused, with the cover, and Play resumes it.
* While the bar's volume indicator is on, Windows' own volume pill is hidden, including the newer pill that the keyboard volume keys show on recent Windows 11 versions. It comes back when the indicator or the mod is turned off.
* New **Disks** item (optional): its menu lists the local and removable disks with a usage bar each, and opens them in Explorer.
* Bar text and icons: automatic (Windows theme, or the solid background), white or black, in Menu Bar Settings.
* "Show the bar on": all displays, the main one, or a chosen display (listed by name). If that display is disconnected, the bar moves to the main one.
* Now Playing: clicking the album cover (Control Center or Now Playing) brings the playing app's window to the front.
* Now Playing: new "Full cover" layout. The cover fills the panel, the text and progress bar sit on a dark gradient at the bottom, and the controls are round glass buttons like the Control Center's, with the cover magnified under each one.
* Bar icons now share one size: each one is scaled to the same height, and icons and text grow together with one size setting. The bar height is set on its own, with a button to restore the default size.
* One selection color for the bar, the menus and their switches.
* The font is picked in Menu Bar Settings (it left Windhawk's settings page).
* Menu Bar Settings reorganized (bar language, font, themes, size and behavior); its drop-down lists use the window's own glass, and a row reacts only to clicks on its control.
* Now Playing: the bar icon is a thin round play or pause sign. In the "Full cover" layout, the buttons are white in light glass themes.
* Menu Bar Settings: the window can be moved from any empty spot and opens where it was left.
* Notifications: with no notifications, a small "No notifications" pill, like "Edit Controls".
* Status pills (Caps Lock, Focus, audio output...) show the latest state: a new one of the same kind replaces the text of the one on screen instead of queuing behind it.
* New option: hide the bar while a window is maximized on its display. The window gets the whole height, and the bar comes back by itself when the window is restored or minimized, or on the desktop.
* New "Hide with the handle" option: a thin pill at the top center; a click shows the bar and windows move down for it (with the "Window color" background, the bar also takes the color of the active window's title bar); another click hides it. It's invisible while the bar is shown, until you point at it, and grows with a spring animation. While it's on, the other ways of hiding are off; "Hide while maximized" turns it off, since the bar comes back on its own. Hiding when the pointer touches the top was removed.
* New **Window color** bar background: transparent over the desktop, and the color of the title bar of the maximized window on that display; the color stays while a menu, Start, search or another window is in front. It replaces "Solid background with a maximized window".
* Weather: a plain glass look for the widget, and a switch to turn off its animations.
* Fix: with "Follow the background live", Now Playing and the Notifications panel follow the background too.
* Fix: after an update or recompile, the bar comes back on its own (before, the mod had to be turned off and on).
* Fix: with liquid glass, a panel that had just closed (Control Center, calendar, weather, tray) no longer shows up inside the next panel's background.
* Fix: opening Notifications no longer highlights the camera icon; the bar's highlight stays on the right item.
* Fix: with liquid glass, panels at the edge of the screen no longer show a clear strip along that edge.
* Fix: with the liquid bar, the bar no longer reflects the title bar of a maximized window below it.
* Fix: floating notifications with Glass or "Window color" and live on no longer show glass slivers at the ends of the top pill.
* Cleanup: removed unused code and translations.
* README: new demo with zoom on each feature (animated WebP, plus the full-quality MP4).

## 1.0.0 ([Nov 10, 2026](https://github.com/ramensoftware/windhawk-mods/blob/490977e5b6cf20cdba95d86f1203e8f8be44e012/mods/mac-os-27-finder-bar.wh.cpp))

Initial release.

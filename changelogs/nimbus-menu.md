## 2.0.0 ([Nov 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/d24319c9fa752c9a0649de112ec7f8f2668cdfb1/mods/nimbus-menu.wh.cpp))

* Tray: the icons of the apps running in the background, next to the status area. Inline with arrows, behind one button (pin the ones you want outside), or in a grid or list panel. Clicks go to the app itself; the middle button pins; drag an icon onto another to reorder.
* Control center: one dock button opens do not disturb, Bluetooth, night light, shortcuts (project, cast, captions, accessibility), each monitor's brightness (DDC/CI), sound, power and any other item placed there. Each item opens its full panel inside it, and it scrolls on small screens.
* Every status item can be in the dock, the control center and Start at once, or off.
* Bluetooth: turn it on and off, paired devices with their battery, connect and disconnect headsets and speakers.
* Notifications: Windows' notifications also show as Nimbus cards (corner of your choice, paused under the mouse, never over games), with their buttons; a bell opens the list, newest first; Win+N can open it. Optional: hide Windows' own banners (asks first, restored when turned off). Its own sounds (15 to pick, or Windows' own), one for notifications and one for reminders.
* Player: what any app plays in Windows' media controls: cover, title, time, controls, shuffle and repeat, that app's volume, switch between apps. In the dock as the cover and title, the cover, the title, bars moving with the sound, or an icon.
* Devices: wireless batteries (Bluetooth, Xbox controllers, and USB receivers through BatteryHub when installed), with warnings at 20% and 10%.
* Notes and reminders: notes stay until deleted; reminders come as Nimbus notifications, late too if the PC was off.
* Shelf: drop files on the dock to keep them for a moment, then drag them anywhere; Ctrl+V turns a copied picture or text into a file.
* Start: smarter search (no accents needed, initials like "vsc", one typo, common nicknames, what you open most first), keyboard navigation in every tab, power buttons that ask before restart or shut down, and a right-click menu with sign out, hibernate and restart to advanced options. All apps refreshes when an app is installed or removed.
* Power modes follow Windows' power plans (High performance, Balanced, Power saver).
* Its own settings window with a live preview; Windhawk's settings tab is empty on purpose (1.x settings carry over). A guided setup asks a few questions on the first run.
* Follows Windows' high contrast and "Animation effects" off.
* Start's Status tab in the control center's look (time, player, round switches, sliders, tray, power, rows), its parts in any order ("Organize": drag them, or Alt + arrows), saved per account.
* Sound panel, simple (the default, as before) or complete (Settings › Status area › Sound): the apps playing on the left and every output on the right. Drag an app onto an output to play it there too, from one output to another to move it, × to take it off one; choose the default output and the one for calls. Complete is off by default because it changes Windows' sound output: the default and communications outputs (IPolicyConfig) and each app's output (the AudioPolicyConfig factory Windows' own Sound settings use), both undocumented by Microsoft; an app on a second output is a copy Nimbus plays through process loopback (documented, Windows 10 2004+) while it runs. An app that picks its own outputs (TeamSpeak, Discord) is muted on the one it's taken off. Going back to Simple puts everything back (every app on the default, copies off, those mutes undone). Listed in the readme's "What it changes in Windows".
* Notification sounds stay audible during calls (Nimbus' audio session opts out of Windows' communications ducking).
* Many fixes: dock clicks on admin windows, panels anchored to their item, faster panels (slow reads moved to worker threads).

## 1.1.0 ([Nov 20, 2026](https://github.com/ramensoftware/windhawk-mods/blob/e8a0e11d5ea622bf5b9be550aaae98502fec3a35/mods/nimbus-menu.wh.cpp))

* New "Downloads" tab in Start: the newest files in the Downloads folder, plus files downloaded from the internet into any other folder (found with Everything).
* Search box: select text with the mouse or Shift + arrows, and use Ctrl+A, Ctrl+C, Ctrl+X, Ctrl+V, Home, End and Delete.
* Right-click on a file (search results and Downloads): open, open file location, copy path. "Open file location" for desktop apps in All apps too.
* "End task" in the dock's app menu, following the Windows option (Settings > System > For developers), with a new setting to always or never show it.
* Windows key: typing right after it now always goes to Nimbus' Start, also when the keyboard used to go back to the previous window. Pressing it again with Start open only closes it.
* Apps whose window belongs to a helper process, like Steam, are now pinned with the right program and icon.
* With Windows' transparency effects off, or battery saver on, the glass becomes a solid color instead of black.
* Smoother rounded glass corners, and no more stutter when scrolling through all apps.
* New homepage for help and suggestions: https://github.com/igoredm/nimbus-menu

## 1.0.0 ([Nov 18, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f97981a8e580c12cc3eaa170a499de7e5b087d4b/mods/nimbus-menu.wh.cpp))

Initial release.

## 2.0.0 ([Oct 14, 2026](https://github.com/ramensoftware/windhawk-mods/blob/083d7d594931ed64ed8214c4aff06f8daf3ea6f8/mods/mac-island-dock.wh.cpp))

**Styles and look**
* "Fixed" style: the pill keeps its room at its edge, so maximized windows stop beside it (also right after Windows starts).
* Notches for the Fixed style: one in the middle, one at each top corner, or three, with curves from flat to wide and sweeping (0-200%); items are dragged between notches in the settings, each notch takes at most its share of the screen, notifications come out of the notch with the bell, and the notches hide each on its own or together as one line.
* Colors: "Auto" and inverted colors following Windows' light/dark mode, your own colors (fill, text, icons; a warning when they're hard to read), colors following the app in front, and the island's opacity (#6048).
* Frosted glass on the bar and on the Fixed style's strip (Windows' blur; it turns Windows' transparency effects on, and back off when it's turned off) (#6048, #6063).
* Islands on every monitor (optional): each other monitor follows the main island's settings or uses a profile of its own; editing a profile from any island updates every island using it.

**Place**
* Minimizing to a line at the bottom and on the sides too, and hiding as its line until the mouse reaches it, after a chosen delay (#6048).
* Staying clear of the taskbar on any edge, and out of a screenshot selection's way.
* Place and icons per app: also what's playing's parts, the clock, the distance from the edges, the size and the items' order per app; the list scrolls; only for the Island style (the Fixed style and the bar keep one place for every app).
* A pill longer than the screen stops short of its edges and the mouse wheel slides along it.

**Settings, profiles and backup**
* Settings in tabs: General (profiles, backup, place), Look (style and notches, colors, size and transparency, animations), Pill (the items' order, system, desktop, apps and tools, the control center's buttons), Tray, media and clock, Notifications, and Monitors with two or more screens. Wider columns, each option right under the one it depends on, names shown whole on their lines with the selectors and sliders under them; a search through every tab; in the middle of the screen with the bar style; a gear at the bottom of the control center opens them; a dimmed option says why under the mouse.
* Monitors tab: a map following the real display layout; one screen, all screens, or following the mouse; each other monitor picks the main island's settings or a profile with a click, and opens its own settings from its gear; only the main island shows the notifications.
* Text boxes work like Windows' (mouse and Shift selection, double click on a word, Ctrl+A); scroll bars easier to see.
* Profiles: save the settings as profiles (up to 10), rename, delete; the one in use saves every change by itself; an optional pill button switches them (Shift + wheel picks one). The pinned tray icons and their places are kept in them too.
* Backup: export and import a file with notes, tasks, keyboard shortcuts, calendar events and agendas (their links too, so it's kept private), pinned folders, the tray icons' order and places, the wallpapers' folder, profiles and every island setting.

**Pill and items**
* The clock kept in the middle (the other items move around it, no blank part beside it) (#6102), 24 or 12 hours with a small AM/PM, the date order and the month as a number (#6064).
* Middle click mutes the volume or the microphone, or turns Do not disturb on or off; Shift + wheel on the volume switches the output device; the microphone's mute also turns its level to 0 (and back).
* Volume and battery percentages, and the screen's brightness on the wheel with its own small panel (#6064).
* The control center's item has its own icon; the control center's buttons can be on the pill, each on its own (Wi-Fi, Bluetooth, night light, hotspot, airplane mode, project, cast, captions, accessibility, screenshot, lock, emoji, clipboard, Task Manager, Calculator, File Explorer), each with its own place in the order; the order's hint says items can be dragged (#6146).
* New items: the keyboard's language and input mode (中/英), with a bubble when it changes while minimized (#6068); the apps using the microphone, camera or location (#6071); USB drives (open, eject, and a notice when one is plugged in) (#6071); virtual desktops (the wheel switches them); folders and places, browsed in Finder-like columns (#6065); wallpapers from a folder, with transitions.
* An optional bubble with what's copied (#6064).

**Calendar**
* Agendas (#6144): .ics links or files on a drive (Google Calendar's secret iCal address, Outlook and others; a Google Calendar page link is read as its iCal address), added from the calendar's gear or in the mod's settings, read only, repeating events too, each agenda in its own color; the next events under the month, with the year when it's another one.
* Today's color: red, Windows' accent color or any color (#6144).

**What's playing**
* The wheel on it can change the song instead of the volume (option); clicking the cover brings its app to the front; long titles and artists scroll in its panel under the mouse (#6146).
* On the pill, the mouse over it shows the whole title and the artist, and a long title can keep scrolling while it plays (option).
* The app's own volume and mute in its panel.

**Notifications**
* A count and the app's name on unread apps' icons (WhatsApp's counts its unread messages); one icon per app for apps that send notifications under several IDs, like Phone Link's phone apps (option, on by default).
* Clicking a notification also opens apps hidden in the tray, like QQ and WeChat (#6063).
* Notifications that came while the island wasn't running (the PC off, the island turned off) and are still in Windows' list show on the island when it starts.
* With the bar style, banners drop out of its middle; banners arriving while a panel is open wait for it to close.
* The bell's right click opens the notifications with their switches; optional Win+N and Win+A open the island's notifications and control center instead of Windows' (#6064).
* Snipping Tool's notifications can open the picture just saved in File Explorer instead of the app (option) (#6107).
* Selecting text with the mouse in the pill's reply box.

**Control center, power and weather**
* Wi-Fi: a switch to turn it on or off, connect, disconnect or forget a network (two clicks), the whole reason when connecting fails, and the cable's state and speed.
* komorebi (if installed): start, stop and reload buttons with its version, as an optional button on the pill and as a control center section; each runs komorebi's own `komorebic` only on a click, without a window; when it fails, a visible terminal shows that attempt's output. Options for whkd and masir. Nothing is installed for the user, and the mod never stops komorebi by itself.
* Power: the row says it's the Windows power plan (#6101).
* Weather: cities found by their Chinese names when the island's texts are in Chinese (#6101).

**Tray icons**
* Names in a bubble drawn like the island, with the program's name for icons that send none (#6065).
* The icons not pinned can open in a list under the pill, as a grid or with their names, which stays open while an app's menu shows (#6065); the grid and the list say how to pin one.
* Dragged with the mouse: reordered in the list, the grid and the list with names; pinned ones put anywhere on the pill, beside any item (they go with it, in every style and notch); from the list to the pill (pinned there) and back (unpinned). Each island keeps its own: the ones following the main settings share them, and a monitor with a profile of its own keeps them in it.
* A click in the room between two icons goes to the nearer one (#6065); right clicks reach every app's menu, including Qt apps like VLC and qBittorrent.

**Taskbar**
* New installs keep the Windows taskbar as it is (the dock is a choice in the settings).
* "Hide the Windows taskbar's clock and tray" for the unchanged taskbar, and hiding the taskbar with every taskbar choice (#6048).
* The transparent taskbar on the top and side edges, and a fix for the dock and the taskbar taking turns while the hidden taskbar slid.

**Other**
* Chinese (Simplified) texts and settings.
* Smoother and lighter: no stalls when opening panels, high-resolution frame pacing, scrolling texts moving smoothly, panels opening under the clicked item in every style, and options that are off run nothing (#6099); panels left open draw nothing while nothing happens: a text box's caret stops blinking after a few seconds, long names scroll only under the mouse, the settings aren't laid out again every 2 seconds, and an open panel isn't redrawn by the island's own animations, like the usage counts (#6151).

## 1.0.0 ([Oct 9, 2026](https://github.com/ramensoftware/windhawk-mods/blob/503f3543e6b770871324d79b725fe59b6ddc5555/mods/mac-island-dock.wh.cpp))

Initial release.

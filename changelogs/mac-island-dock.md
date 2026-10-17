## 2.0.1 ([Oct 17, 2026](https://github.com/ramensoftware/windhawk-mods/blob/5a3e20ab7652e5c542efc0af02a6bb1a83b8234a/mods/mac-island-dock.wh.cpp))

**Panels and look**
* Panels always open under the island (over it on the lower half of the screen), under the item clicked, the same for every panel (#6211). A panel already open for another item (the volume's page, then Wi-Fi) moves under the new one, and a closing panel no longer takes clicks meant for what's under it.
* Every icon on the pill is the same size (the media and control center buttons and the arrows were smaller, the apps' icons bigger), and a new "Icon size" option (70% to 150%, per profile and monitor, in every style) changes them apart from the island's size, so a big bar can keep small icons.
* Notches grow with their items until they almost touch each other (they scroll only then), instead of each stopping at a third of the screen; the "Icons before scrolling" option now says it's for the other apps' tray icons.
* The power menu's confirmation shows its whole text, and buttons and menu rows no longer end with "…".
* The control center's pages opened straight from their pill icon (Wi-Fi, Bluetooth...) have no back arrow (nothing to go back to).
* A panel opened and closed again at once (a quick double click) no longer leaves its outline behind the island.

**Profiles and monitors**
* The profiles' button is per profile: turning it off on one monitor's island no longer turns it off on the others.
* A monitor with its own profile no longer follows the main island for options added after its profile was saved (they're kept in its profile as it first shows them).
* A monitor with its own profile shows the virtual desktops, the keyboard language, USB drives and the microphone/camera/location in use when its profile turns them on, even with the main island's off.

**Notifications**
* The Widgets button's news show on the island as "Widgets" with its icon, and a click opens Widgets (it was an unnamed grey square that did nothing) (#6206).
* Windows' own notifications (like Windows Security's "Update antivirus protection", or a sender whose icon is only in the registry) show their name and icon instead of "SecurityCenter" and a grey square (#6206, #6187); apps whose windows carry no app ID (like Discord) show their program's icon.
* A switch for the icons of the apps with notifications on the pill (Notifications section, on by default).
* Clearing notifications on the island (the X, a swipe, a card's button, "Clear all") also removes them from Windows' notification center; "Clear all" removes only the ones the island's list shows (compared by ID).
* With the island's own notifications, a priority app's Windows banner no longer flashes before the pill takes it, and a notification right after a screenshot shows in the pill (#6159).
* With the island's own notifications and its "Do not disturb" on, Windows' "Do not disturb" is "alarms only": an app on Windows' priority list (like the Snipping Tool) no longer shows Windows' banner or plays its sound (calls, alarms and reminders still show and ring in the pill). Windows' "Do not disturb" is also turned back off when the mod is turned off or removed while the island isn't running.
* The keyboard language bubble only shows when the mode changes in the same app (not when opening Start or minimizing an app), once, over the island's line, without repeating "中" (#6210, #6199, #6233).

**Control center**
* Brightness with more monitors: a slider for each monitor (with its name) instead of one for all, and the wheel on an island's brightness item changes its own monitor only (the control center follows it). The list is read again when a monitor is plugged in or out, a monitor plugged back in shows once it answers, and one turned off (still connected) leaves the list the next time it opens.
* Bluetooth: clicking headphones or speakers connects or disconnects them, showing "Connecting...", then "Connected" or "Couldn't connect" on their row, and connected ones have a "Disconnect" button; other devices open Windows' Bluetooth settings (#6160). A control that's on shows in the island's own colors on the pill (no colored icon).
* The volume mixer shows each app once, like Windows' mixer: an app playing from more than one of its processes (Spotify during an ad) was listed twice.
* Updates: an app updated in the terminal that only finishes when it closes (like VS Code while open) says "Close it to finish" instead of offering the update again.

**Folders**
* Pinned folders can be dragged into another order; each folder's items can be listed by name or newest first (its own order) and searched (a name or a type like *.pdf); the folders shown update by themselves while the panel is open (a download, a file renamed or deleted), their rows come in with a short animation, and going back slides the columns.
* Files and folders can be dragged out to Explorer or the desktop, copied or moved as chosen (asked the first time, with "Remember my choice"; also in the island's settings), and files can be dropped on a folder in the panel, from the panel itself or from Explorer and the desktop (held a moment on the folders item, the panel opens; held on a folder, it opens), copied or moved by Windows as in Explorer (its questions for names already there, its progress, Ctrl+Z). After a drop the panel stays open, the folder opens and the files dropped light up.

**New pill items**
* komorebi's layout (an option, off by default): the current layout of the island's monitor as an icon and a name ("Grid", "Paused", "Monocle"), after a dot for each of its workspaces (the one shown lit, ones with windows faint, empty ones a ring or hidden by an option, like yasb's; a click shows another one), only while komorebi runs; its click opens a menu with the layouts' icons to change the layout, tiling, monocle and pause for that monitor (#6163). It follows komorebi's own events (`komorebic subscribe-pipe`), with nothing polled, and finds a komorebi that starts slowly (at sign-in). komorebi's start and stop button has its own icon, and the settings say when "with whkd" is on but whkd isn't installed.
* The app in focus on each monitor's island (an option, off by default): its icon and name (as the taskbar shows them, Steam included), or "Desktop", sliding to the next one; a click brings it to the front.

**Other fixes**
* Fog shows as a cloud in the weather (Windows 11's fog emoji is a plain square) (#6226).
* Changing the wallpaper: the transition stays until Windows really shows the new picture (the old one no longer flashes back, and Windows' own zoom no longer shows through), the picture is placed exactly where Windows puts it ("Fill" keeps a third of what's cut above and two thirds below, measured on Windows 11; "Span" set for one monitor is drawn like "Fill"), so it no longer slides down at the end, and the transition no longer covers the taskbar of a monitor showing the desktop.
* Clicking what's playing's cover brings back an app hidden in the tray that ignores its tray icon's click (Spotify from the Microsoft Store): it's started again by its ID, which shows its window.
* Dock: one shown for an app's notification on one monitor no longer moves another monitor's dock (with monitors at different scales, the second dock was left almost hidden), and its background tint applies when Windows can't blur (transparency effects off, energy saver) instead of turning solid (#6251).
* A transparent taskbar hides again while an app's button was already asking for attention when the taskbar's style changed.

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

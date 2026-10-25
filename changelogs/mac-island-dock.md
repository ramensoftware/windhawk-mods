## 2.1.0 ([Oct 25, 2026](https://github.com/ramensoftware/windhawk-mods/blob/41fdb9158f257c7d4bbc3358369facf21ab625a7/mods/mac-island-dock.wh.cpp))

**Folders**
* Sorting by date now finds the newest files in folders with more than 200 items; wider columns.
* Sorted as in Explorer (name, date modified, type or size, either way, with the date, type or size shown), only one kind of file shown or folders mixed with the files, and each pin with its own icon and color (with an "Apply" button to close the choice); pinned folders show a folder icon again.
* A file's row has two buttons: copy the file (pasted in a folder, it goes there) and copy its path (#6161).
* Windows' places open with a click; their pin, at the row's end, pins them (a click used to pin them, #6161).
* Favorite files: a star on a file's row keeps it with the pinned folders, where a click opens it and its buttons copy the file or its path; one deleted or moved leaves the list, one on a drive taken out comes back with the drive (up to 40).
* The pins, their icons and colors and the folders' order are each Windows account's own (another account on the same PC no longer shares them).

**Calendar**
* Clicking an upcoming event or holiday shows its day in the calendar (#6156).

**Notifications**
* With islands on several monitors, the Monitors section chooses which islands show the notifications (the main one by default). The main island keeps the sounds, Windows' banners, Win+N, and calls, alarms and reminders; opening, deleting or minimizing a notification on one island closes it on the others (#6159).
* The apps with notifications can fold into an arrow, like the tray: they come out a moment when a notification comes, and under the mouse (an option, #6129, #6204, #6208).
* Chat apps that only blink their tray icon instead of sending notifications to Windows (the Chinese WeChat, WeCom, DingTalk) show on the pill with their dot when it starts blinking (#6074, #6215).

**The pill**
* The time can be turned off (on by default, per profile): the date alone shows then, with its own options as before, or nothing when they're off too.
* A "Show desktop" button for the control center and the pill (off by default).
* The language bubble only shows when the app in front changes its own mode (Shift, the indicator), not when another app comes in front, and it's no longer cut with a low pill (#6422).
* An item drawn in two parts (the battery or the volume and its percentage) has one hover highlight (#6460).
* Win+J (an option, off by default, #6519) minimizes the islands to their line and brings them back; its keys can be changed by recording them in the settings (a click, then the new keys; Win+J back with one click), with a warning when Windows or another app uses them too.
* With notches, the keyboard language bubble shows once, from the middle notch (#6199).
* What's playing's note, its cover and title, and its buttons are three items of their own in the pill's order (each placed and turned on or off on its own; current layouts keep them together).
* A control center button that's on (like night light) shows its icon in the accent color with a dot under it, instead of a white disc behind it.
* A notification's title too long scrolls under the mouse (#6200).

**Media**
* A mixer in the media panel: every app with audio sessions on the default output, one entry per program with its icon, name, volume and mute by clicking the icon; the playing app keeps its own volume and mute in the player, with the device's volume and a button for Windows' mixer below it (Win+Ctrl+V on Windows 11 23H2+, sndvol on older versions or if input fails). The mixer opens collapsed and expands with an animated arrow. The session list and icon conversion are shared with the control center.
* Source chips show the apps' icons, dimmed when paused, and Automatic has its own glyph. They wrap into more rows when needed.
* A larger cover, the whole modal tinted from the cover's average color with a smooth color transition, a small app icon beside its name, and two pill switches with rounded highlights in a compact footer; the panel scrolls for long lists and stays within its monitor, with press and spring animations, crossfading/sliding media source transitions and animated mute icons/volume bars.
* Volume sliders follow the pointer while dragging; audio writes are coalesced off the drawing thread, the mixer reveal keeps its existing row geometry, and footer labels have padding on both sides. Accent colors, media text and icons are cached outside drawing; idle monitor/notch profiles are skipped by the animation loop.
* Lyrics default to Follows the line (each line's own width, up to 360 DIPs); Short, Medium and Long hold the widest line until a pause or gap.

**Credits**
* The README's credits (English and Portuguese) link m417z's mod and ask for credit, with a link, from anyone reusing this mod's code or inspired by it (a request, not a license condition).

**New items on the pill** (each off by default, #6157)
* The Recycle Bin: how many items; open it, or empty it with a second click; always shown, or only once what's in it weighs 100 MB, 500 MB, 1 GB or 5 GB.
* A timer, a stopwatch (with hundredths) and Pomodoro, each on its own (all three can run at once), in a panel like the macOS Clock app: lengths in hours, minutes and seconds on wheels (the mouse wheel, a drag or typed), a ring while it runs, Pomodoro's focus and break chosen; the pill shows the one ending first, in orange, and a click on it lists every one going as Live Activities like the iPhone's (each with its time, Pause and Cancel, the next alarm with its switch, Show all); a notice when each ends, with Repeat and Stop (Pomodoro: Stop and OK); and alarms in the same panel: a time each day on wheels, each on or off, with or without a sound, edited with a pencil, its notice with Stop (its sound repeats until then, a minute at most); alarms ring and timers end on time also behind a full-screen app or while the island restarts, and each Windows account has its own; Control Center buttons for the stopwatch, Pomodoro and alarms (#6156).
* The network's speed, up and down.
* An audio visualizer: six rounded bars jumping with the beat of the playing app's own sound (or any sound, like VLC's) while something plays, or swaying smoothly like the iPhone's (an option), with a sensitivity slider and a color (the icons', Windows' accent, a rainbow, your own or the cover's).
* World clocks: Windows' own additional clocks (Date & time settings), shown as soon as they're added there, with a day ahead or behind marked; a click opens Windows' list of them.
* The time until the next event, alarm, timer or Pomodoro part (each can be left out; in the next 12 hours); a click opens it.
* The pill filling like a loading bar (each an option, off by default, in the Animations tab): with the progress apps show on their taskbar button (downloads, copies, installs; #6071, read from the taskbar's own progress bar), and with a laptop's battery charge while it charges (not while it's held at a charge limit, like 80%); one bar across the Fixed style's notches (the whole bar in the Bar style), a solid line under it like the taskbar's; a color for each kind (downloads and other apps, Windows' file copies, the battery: green, blue, Windows' accent or your own; red for an error, yellow paused); it moves simply, as a liquid wave or as a glow with particles; paused it breathes, an error shakes, done flashes with a check, canceled drains back.
* Song lyrics (an option, off by default, goes online: each song's title and artist are sent to lrclib.net): the line sung now on the pill, in time with the song (its size, color and longest line chosen; or in the song title's place while a line is sung, an option), and all of them in a panel where a line clicked goes there in the song. Only for songs (with an artist, 45 s to 20 min, not videos or voice messages), and "Apps that look for lyrics" lists the apps seen playing as cards with their icons, each one's lyrics turned off with its switch; a search result is taken only when its title, artist and length match.

**Status notices** (from the pill, an option, off by default)
* The charger plugged in, the battery under 20% and 10%, a Bluetooth audio device connected, and "Do not disturb" turned on or off (#6158, #6204); on the main island, also when it doesn't show the apps' notifications.

**Tray icons**
* The grid of the icons not pinned has its own number of columns (6 as before, #6162).

**Languages**
* The island and Windhawk's settings in German, Spanish, Russian and Japanese too (#6472), chosen in the island's settings or like Windows' display language; the dates follow them.

**Settings**
* The island's settings open in a window of their own, like Windows' Settings (the gear under the island and the control center's): a side bar with the search and a tab per subject (also Dock, About and Logs), each option on a card with a line saying what it does, the island's preview at the top (its items in their order, dragged to move them), resizable, in the island's colors over Mica. A dimmed option says why under its name, and a click there goes to what it needs. Logs copies or saves a summary, the settings and the island's last 300 log lines without turning on Windhawk's logs.
* Clearer texts for the two options that hide the clock and the tray (the Windows taskbar's, with the island; the dock's, without it).
* The Monitors page: where the island appears as three buttons that say what they do, the screens as a bigger map (the islands marked), and with an island on every monitor a card for each (its notifications, its look).
* One settings window a monitor (a notch opens its monitor's), opened at its own monitor's size, and back where it was after the island starts again.
* A What's new tab (between About and Logs) with the notes of every published version (2.0.0, 2.0.1, 2.0.6 and 2.1.0) for users, in the island's 7 languages, switched by flags the mod draws itself (Direct2D shapes, nothing downloaded); it opens in the island's language, and the search finds the notes. The language buttons in General show the flags too.
* Once per version (also on a first install), 10 seconds after the main island starts, a notice from the pill says what's new and stays until one of its buttons opens that tab or closes it (an option, on by default, per profile). The language buttons' flags sit on whole pixels, sharp at 150% and 175% too.

**Control center**
* Every pill item that opens something can also be a control center button (hidden until added with Edit): notifications, calendar, what's playing, power, CPU/RAM use, profiles, keyboard language, privacy, USB drives, virtual desktops, folders, wallpaper, komorebi's layout, world clocks, next event, Recycle Bin, timer and the tray icons.
* The magnifier, the narrator and the new on-screen keyboard are buttons that start them (their state wasn't always read right; the magnifier falls back to Win+Plus when it doesn't start); the on-screen keyboard can also go on the control center and the pill (#6160).

**Brightness**
* A laptop's own screen has its slider and wheel too (through WMI, like Windows' own slider), beside monitors with DDC/CI; a monitor without DDC/CI shows none, as before (#6436, #6457, #6501, #6350).
* The wheel on the brightness adds up small turns (touchpads, smooth wheels) instead of losing them (#6420).

**Fixes**
* The island's window no longer reaches 3 pixels into a monitor above its own (it kept that monitor's dock from coming up at the middle of its edge).
* Panels stay inside their monitor and never as tall as it (a long list went past the screen's bottom, into the next monitor, and Wallpaper Engine took it for a full-screen app and stopped).
* Win+N and Win+A open on the main island wherever the mouse is; minimizing a notification sets its app aside on every island.
* Every item on the pill grows in and shrinks away (the tray, the media's parts, the uses, the app in focus, the folders, the minimize button and the percentages used to jump); a group whose contents change size follows them smoothly.
* The Bar style scrolls through its items when they don't fit; the control center has no limit of eight and grows wider to fit the screen.
* A notification's app icon missing right after a restart is looked for again within seconds.
* Closing a virtual desktop from the desktops' list closes that one, also when it's a few desktops away, and goes back to where you were (#6164).
* The network item no longer shows a cable on Wi-Fi when Windows doesn't give the Wi-Fi's details (#6361).
* The Task Manager button opens it also with an administrator's window in front (it pressed Ctrl+Shift+Esc, which Windows dropped then).
* The dock comes up only with the mouse at the screen's bottom edge again; the strip above the edge counts only with another monitor below (#6472).
* A Windhawk setting chooses the height over the bottom edge that brings the dock (or the transparent taskbar) up (in pixels at 100% scale), the same on every monitor; 0, the default, keeps it automatic (#6472).
* The minimized line's invisible strip is half as thick: the mouse passing near the edge no longer brings the island back (#6468).
* The control center's Emoji and Clipboard buttons open for the app in front, instead of closing at once.
* While a panel opened from the pill is open, the pill keeps its width (a longer or shorter media title scrolls in it) and resizes once the panel closes, so the panel's corners stay put; one wider than the pill closes up before narrowing, so its contents aren't cut at the sides.
* Right after Windows starts, the progress fill waits 30 seconds (apps restoring themselves reported a progress they didn't have and filled the whole bar).
* A taskbar button without an app ID for a moment no longer makes its badge blink or loses the taskbar's own picture.
* The dock no longer shows the plate the taskbar stacks behind an app with more windows, and an overlay picture (like a browser profile's photo) stays sharp at its own size as the icon grows.
* With the timer's item on the pill, the next event item no longer shows the timer and Pomodoro too.
* The progress fill reaches into the notches' curves.
* The progress fill follows only what the taskbar's buttons show, read again every second while it shows (it no longer stays after an app has finished), and a new progress fills the pill after 5 seconds (Windows' file copies at once).
* The visualizer follows the playing app's loudest audio session (a browser has several), and the output's sound while that app is paused or quiet, instead of staying still.
* Turning the mod off puts the taskbar's auto-hide back only once Windows confirms it (and tries again later if it didn't), each taskbar button keeps its own app's icon and badge, and one taskbar element failing no longer stops the rest of the cleanup.

## 2.0.6 ([Oct 21, 2026](https://github.com/ramensoftware/windhawk-mods/blob/f0be7478ca936e97c740aa3ffa3407b421e4a7cf/mods/mac-island-dock.wh.cpp))

**The dock**
* Redone to look and move like macOS's: the icons spread on a rounded shelf that grows in width and height with them as they magnify in a wave, pushing their neighbors aside, and a click always goes to the icon under the mouse (on the transparent taskbar too). The taskbar's window makes room above itself for the magnified icons (the work area doesn't change), and the windows' previews open right above the dock.
* Magnified icons are sharp (#6202): drawn from the apps' own bigger pictures (also for apps with several profiles, like a second Chrome profile, and for apps like Steam whose windows give the taskbar no icon: their program's), and icons shrunk at rest are smoothed.
* New settings: icon size (24-64 px, replacing the dock's height: its height and margins follow the icons), space between icons, gap at the bottom, and the background behind the icons (off: only the icons). New defaults: icons 32 px, magnification 170%, reach 150 px, corners 22 px, gap 2 px.
* The background is a plain tint in the theme's color (50% by default), as see-through as chosen (Windows' acrylic only drew its fallback color behind the taskbar, and turned black when the taskbar had the focus). The "Taskbar" choice now says "Dock: compact and floating, see-through, like on macOS".
* Showing and hiding: the mouse within the icons' height of the bottom edge brings it up (with a monitor below, the mouse easily goes past the last row, and those first rows of the monitor below count too when coming from above); the strip beside the shelf keeps it up instead of showing and hiding it in a loop; hidden, only the part under the icons takes the mouse when the tray isn't shown; it no longer stays up after clicking an app in it, and stays up while an app's jump list is open.
* Apps asking for attention no longer make the whole dock jump up for a moment (also with the mouse on another monitor), with "When an app needs attention" set to only the icon or nothing.
* Badges and apps' overlay icons (like Discord's counter) fit in the dock on every monitor, and Discord's counter shows as a sharp red bubble with the right number, also when it was set before the mod started.
* Dots under open apps also show for a pinned app opened from the dock.

**The island's look**
* Glass: the island (in every style, the notches too) and its panels see-through and blurred, with a bright edge, using Windows' blur (it needs Windows' transparency effects; a plain color without them or with the energy saver on).
* "Icon size" is now "Icon and text size" (the pill's texts follow it too).
* New options: the line's opacity (0% hides it, it still brings the island back), how long the area along the edge that brings it back is, the panels' size (100% to 150%), the media title's width on the pill, the minimize arrow at the end of the pill, the gray highlight behind the item under the mouse (#6324), and the tips under the icons and buttons. All of them per profile and monitor.
* Language: the island's texts in English, Portuguese or Chinese, or "Automatic" (Windows' display language); the dates follow it.
* Notches hiding together: each notch keeps its own line, and the mouse on any of them opens them all.
* Icons drawn smaller than their picture are smoothed (no jagged edges).
* On Windows 10, the island's icons use Segoe MDL2 Assets (they were empty boxes).
* The pill's height, how round its corners are and the room at its ends (its width) are options (#6200).
* The thin edge around the island, its panels, banners and bubbles (the glass's bright edge too) has a slider: 0% for none (the minimized line keeps its own).
* Glass: a translucent option (a lighter tint and livelier colors, like the taskbar styler's LiquidGlass themes) and a slider to blur it more than Windows' own (#6368, #6309); it's still Windows' blur, so screenshots and recordings show it.
* The bar style can put its items at the screen's corners, with the clock in the middle (#6275).
* The battery icon turns green while charging (#6271), the volume's and battery's percentages sit closer to their icons (#6199), and a notification's title and text are centered by its icon (#6200).
* The battery icon is drawn a bit bigger, as big as the other icons at every icon size.
* The island's settings, Look tab: grouped in sections (Style, Colors, Glass, The pill's shape, Sizes, Details, Animations), each kept whole in one column; a dimmed slider says why on its name, like the options; the frosted glass's tip lists everything it needs.

**Calendar and updates**
* The calendar's gear (main island) hides your own past events after a day, a week or thirty days (their day keeps its dot and lists them behind "Show N hidden"), shows how many events are kept (out of 200, with a warning when almost full) and deletes the past ones (two clicks).
* Updates are checked every 1, 3, 6, 12 or 24 hours, chosen under their list; an item shown only by another monitor's profile (updates, weather, usage) now gets its data too.

**Other fixes**
* The island starts even when Windows isn't ready for it at sign-in (it tries again for a minute) (#6309).
* Updating or recompiling the mod no longer leaves the island off (the new version's island used to leave while the old one was still closing), and Windows' "Do not disturb" stays on through it with the island's own notifications.
* Discord's notifications stay on the island while its taskbar counter does, and clicking one opens Discord.
* An app's icon that came out as a white square (read while Explorer was restarting) is read again.
* Bluetooth: a device no longer stays at "Disconnecting..." when Windows doesn't answer.
* Changing the power mode or plan from the control center no longer can crash the island (Windows' power library is kept loaded).
* The notification counts on the pill sit in the middle of their bubbles in the Fixed style too.
* Windows' own notifications with buttons the island can't press (like Nearby sharing's "Save" and "Decline") keep Windows' banner, and a click on the island opens Windows' notification center, where the buttons work.
* The keyboard language bubble shows again when switching 中/英 with the island minimized: an input method's own window coming to the front (like Microsoft Pinyin's) no longer counts as another app (#6313).
* Clicking a notification of an app hidden in the tray, like QQ, brings up the running app through its tray icon instead of starting it again (the login screen) (#6076).
* Wi-Fi: the network shows as Wi-Fi even when Windows doesn't give its signal (it showed as a cable); when Windows refuses the networks in reach (no location access on newer Windows 11 builds), the list says so and opens the page that allows it, instead of looking for networks forever; failures are logged.
* Windows' "Do not disturb" stays on when the mod is updated while a folder window runs in its own Explorer process.
* The dock (and the transparent taskbar) no longer shows behind the window in front when Windows left the taskbar under other windows with its "always on top" style still set: it's put on top every time it shows.
* README redone: what the mod does at a glance, with new pictures and GIFs (the styles, glass, the dock), and the whole list of features below.

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

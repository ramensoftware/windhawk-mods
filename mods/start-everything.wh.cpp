// ==WindhawkMod==
// @id              start-everything
// @name            Everything & Power Tools in the Start Menu
// @description     Replace Windows 11 search with Everything-powered files, apps and settings in a separate search window.
// @version         2.0
// @author          bardelyne
// @github          https://github.com/bardelyne
// @include         StartMenuExperienceHost.exe
// @include         SearchHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @license         GPL-3.0
// @compilerOptions -ld2d1 -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lshlwapi -lcomctl32 -ldwmapi -luser32 -liphlpapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Everything & Power Tools in the Start Menu

A native replacement for Windows 11 search, powered by voidtools Everything. Press Win+S, click the taskbar search icon, or start typing in Start to search files, apps, and settings in a separate window styled after your Start menu. Windows Search stays out of the way: its window is never shown, and it cannot start the Edge WebView2 process behind its Bing-backed search panel.

Version **2.0** introduces a separate Windows 11 search window, with Start appearance copying, app suggestions, and custom commands.

![Everything & Power Tools in the Start Menu](https://raw.githubusercontent.com/bardelyne/start-everything/main/img.png)

*Version 2.0: the separate search window with a folder preview.*

## Version 2.0 Changes

- Search runs in its own Windows 11 window, opened directly with Win+S, the taskbar search icon, an optional custom shortcut, or by typing in Start. Start itself still opens normally.
- Copies Start's measured size, alignment, background, rounded corners, and supported Start Menu Styler appearance. Root context menus and submenus receive their copied style before appearing.
- Adds app and custom-command suggestions for an empty query, shared favorite learning, configurable menu width and height, and settings organized into sections.
- Adds named custom commands with CMD, Windows PowerShell, PowerShell 7, or a custom shell; optional hidden terminals; and emoji, Windows glyph, image, icon, or executable icons.
- Adds automatic text and monochrome-icon contrast across the search window, with a fixed-color override. Background sampling pauses during motion and scrolling; unchanged samples reuse the cached color map.
- Improves image previews with a built-in Windows decoding fallback. With a bottom taskbar, preview cards grow down while they fit search, then align at its bottom and grow upward when taller. Top and side taskbars keep previews top-aligned and growing downward. Crossing that height and changing selections mid-animation remain smooth.
- Keeps search input and selection when a context menu closes. Typing or Backspace closes the menu and continues the query; Escape closes the menu before clearing search.
- Defers search UI creation until first use, buffers opening text, and restores native and text-box focus after input handoffs.
- Supports top and side taskbars, with animations confined to the selected monitor. Right-side taskbars place the preview on the left.

## Key Features

- **Instant voidtools Everything IPC**: Queries Everything directly through its Win32 IPC interface for fast results across millions of files. The mod keeps no index of its own.
- **Smart Apps and Windows Settings Search**: Instant fuzzy matching across Desktop applications, Microsoft Store / UWP packages, Control Panel applets, and Windows Settings URIs (`ms-settings:`), with sharp shell icons, made at the exact pixel size of your display.
- **Learns Your Favorites**: Apps and custom commands you open from here more often move up among results that match equally well. A clearly better match always stays on top. Can be turned off in the settings, which also forgets what was learned.
- **Separate Animated Search Window**: Search follows Start's size, taskbar alignment, colors, and rounded corners, with opening and closing animations that follow the taskbar edge (bottom, top, left, or right) and respect Windows' animation setting. It stays out of the taskbar and Alt+Tab. Typing in Start closes Start and carries the opening text into search. The search bar uses Windows' own search icon.
- **Windows Search Out of the Way**: `SearchHost.exe` keeps running for the shell, but its window is never shown and it cannot launch Edge WebView2, the web view behind its Bing-backed search panel.
- **Search Entry Points**: Open search directly with Win+S, the taskbar search icon, Start’s search bar, or an optional custom shortcut. Pressing Win or clicking Start opens Start normally; begin typing to switch into search.
- **Suggested Apps**: An empty search shows apps and custom commands together, ordered by learned usage when enabled and alphabetically when learning is off. Escape clears the query to suggestions; press it again to close search.
- **Custom Commands**: Add named commands to Apps, choose CMD, Windows PowerShell, PowerShell 7, or a custom shell, and optionally hide the terminal. Icons accept emoji, pasted Windows icon glyphs, Unicode codes, and local image, icon, or app paths. Commands also appear in empty-search suggestions.
- **Optional Files Panel**: Hide the Files panel and pause Everything file queries. With automatic sizing, search becomes half as wide. Apps, settings, custom commands, and utilities remain available.
- **Custom Menu Size**: Set your own menu width and height, or turn the toggle off to follow Start's size. Dimensions follow Windows display scaling and stay within the screen's available area.
- **Inline Calculator**: Type a sum (e.g. `100 * 5`, `sqrt(144)`, `15% of 200`, `2^10`) and the result shows above the apps; large and small results show in scientific notation too (`= 99000`, `= 9.9e4`). Press Enter to copy it.
- **Unit Converter**: Type `100 km` to see it in the usual units of its kind, all on one line first, or `100 km to mi` (also `in`, `->`, `=`) for just one; `255 hex` or `0xFF` for other bases. It knows length, mass, temperature, speed, area, volume, data, data rate, time, pressure, energy, power, angle and frequency, with `.` or `,` as the decimal point, and each kind has its color. A number alone, or one in a name (`2024 report`, `3d`), is left to the search. Add units it does not know in the settings, and have them convert back too (Reciprocal).
- **Network Interface Inspector**: Type `/ip` to display all active Wi-Fi, Ethernet, and VPN network interfaces with their IP addresses, subnet masks, gateways, and hardware descriptions. Press Enter to copy the IP.
- **Full Right-Click Context Menu**: Right-click any file, folder, or application to Open, Run as Administrator, Open in terminal, Properties, Create desktop shortcut, Cut/Copy (files), Copy name (files/folders), Copy path, Open file location, or Uninstall (apps). Escape closes the menu; typing closes it and continues the query.
- **Native Properties Dialogs**: Properties opens through `explorer.exe`, the same dialog as in File Explorer.
- **File Preview**: Images, muted media, PDF pages, SVGs, text/document excerpts, and file properties beside search. The card adapts its alignment and resize direction to the search window’s height. See [File Preview](#file-preview) for formats, copying, and folder sizes.
- **Drag and Drop**: Drag a file or folder into File Explorer, the desktop, or another app. It is always a copy, never a move. Previews stay hidden during the drag. A successful drop closes search; Escape cancels.
- **Uninstall**: Right-click an app and choose Uninstall. After a confirmation that names what goes, a Store app is removed as Start removes it, and a program runs its own uninstaller, the one Installed apps in Settings runs. Protected Windows components offer no uninstall action. A Store app is removed in the background, with no progress bar: it is gone from the list after a few seconds.
- **Opens in Front**: Programs are started by Explorer, the way the stock Start menu starts them, so they come to the front even when they take a while to start or you move the mouse meanwhile.
- **Explicit Web Search**: Trigger web searches on demand using the `?` prefix (e.g. `?query`). Includes customizable keyword shortcuts such as `?yt` (YouTube), `?gh` (GitHub), `?w` (Wikipedia), and `?r` (Reddit).
- **Start Menu Styler Compatibility**: Copies supported Start backgrounds, colors, rounded corners, and menu styling before menus appear. Automatic text/icon contrast or a fixed text color keeps labels readable; see [Automatic Text and Icon Contrast](#automatic-text-and-icon-contrast). Arbitrary custom Styler effects may not be reproduced exactly.
- **No Simulated Keystrokes**: Focus is handled with standard foreground APIs only; the mod never attaches thread input or synthesizes keystrokes.
- **Accidental Open Prevention**: Enter only triggers actions when an item is selected in the active panel, preventing unintended opening of files.

## Settings Sections

The mod's Settings page groups related options together:

| Section | Options |
| :--- | :--- |
| **Appearance** | Text size and color, keyboard shortcut hints, and custom menu dimensions |
| **Apps & Search** | App result limit, favorite learning, search delay, and an optional keyboard shortcut |
| **Files & Preview** | Files panel, file result limit, previews, animation, and excluded paths |
| **Web Search** | Default search engine and keyword shortcuts |
| **Custom Commands** | Named commands, icons, shells, and terminal visibility |
| **Custom Unit Conversions** | Units, formulas, categories, and reciprocal conversion |

## Custom Commands

In Windhawk, open the mod's **Settings → Custom Commands** and add an entry:

- **Name**: Display name and search term.
- **Icon**: Paste an emoji or a Windows icon glyph, enter a Unicode code such as `E713` (Settings) or `E7E8` (Power), or provide a full path to an image, `.ico`, or executable. `U+E713`, `0xE713`, `\uE713`, and `&#xE713;` also work. Environment variables in paths are expanded. Leave blank for the terminal icon.
- **Shell**: Choose **Command Prompt (CMD)**, **Windows PowerShell**, **PowerShell 7**, or **Custom shell**. Existing commands default to CMD. PowerShell 7 must be installed separately; it is found in its standard installation folder or through PATH.
- **Shell Executable**: For Custom shell, enter its full executable path or a name available in PATH, such as `nu.exe` or `wsl.exe`. Environment variables such as `%ProgramFiles%` are supported.
- **Shell Arguments**: For Custom shell, put `{command}` once as a separate argument where the shell expects the command. The mod quotes it as one Windows argument, preserving quotes, spaces, Unicode and operators inside your code. Do not add quotes around `{command}`. Environment variables in this arguments template are expanded before inserting the command.
- **Hide Terminal**: Run without showing the shell console (default off). Apps opened by the command can still show their own windows, and Run as administrator can still display Windows' permission prompt.
- **Command**: Enter the command for the selected shell. The mod does not guess the shell from the command text. Blank names or commands are ignored. If the selected shell is missing or the Custom shell arguments are invalid, search stays open and shows the reason.

Commands appear alongside installed apps in search results and empty-search suggestions, within **Max App Results**. They share the favorite system: frequently launched entries move up, and equal-use suggestions sort alphabetically. Turning off **Learn Favorite Apps** stops learning and clears history for both apps and commands. Right-click for **Run**, **Run as administrator**, or **Copy command**. Settings changes apply without reopening search. Up to 100 entries are supported.

For example, name a CMD command `Downloads`, use `📂` as its icon, and set its command to `explorer.exe "%USERPROFILE%\Downloads"`. Browse Microsoft's [Segoe Fluent Icons list](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font) for Windows glyphs and their codes.

Custom shell examples (the shell must already be installed):

| Shell | Shell Executable | Shell Arguments |
| :--- | :--- | :--- |
| Git Bash | `%ProgramFiles%\Git\bin\bash.exe` | `-c {command}` |
| Nushell | `nu.exe` | `-c {command}` |
| Bash in WSL | `wsl.exe` | `--exec bash -lc {command}` |

Use a full executable path if it is installed somewhere else. WSL requires an installed Linux distribution. PowerShell 7 installation options are described in [Microsoft’s documentation](https://learn.microsoft.com/en-us/powershell/scripting/install/install-powershell-on-windows).

## File Preview

Enable **Settings → Files & Preview → File Preview** to show a card beside the selected file or folder. The card includes its name, type, size, dates, and relevant media or document properties.

With a **bottom taskbar**, the preview uses the available screen space independently of the search window's height:

- **Preview fits inside search's height:** align at the top and resize downward.
- **Preview is taller than search:** align its bottom with search and resize upward, including above the search window.
- **Preview crosses search's height:** switch alignment continuously during the same resize animation, without growing down and jumping up afterward. Rapid selection changes continue from the current size.

The card stays within the monitor's usable area where its height permits. If upward growth reaches the screen's top edge, it uses the available space below.

With a **top, left, or right taskbar**, the preview always aligns with the top of search and resizes downward. A **right taskbar** places the preview on the **left** of search.

| Content | Preview behavior |
| :--- | :--- |
| **PNG, JPEG, BMP, GIF, TIFF** | Shell thumbnails, with Windows' built-in image decoder as a fallback. IrfanView or another image viewer is not required. |
| **SVG** | Rendered using Windows' SVG support. |
| **Video, animated GIF and WebP** | Muted looping playback when **Play Videos and Animations** is enabled. Video format support depends on the codecs available in Windows. |
| **PDF** | Rendered with Windows' PDF engine, without a separate PDF app. With animation enabled, cycle through up to the first ten pages, one per second. |
| **Text and code** | The beginning of the file, with selectable and copyable text. |
| **Word, Excel, PowerPoint, RTF and OpenDocument** | A text excerpt when a supported Windows or Office document filter is available. Text is extracted without document page layout. |
| **Folders** | A folder icon, item counts, and the indexed folder size when enabled in Everything. |
| **PSD and other specialist formats** | A shell thumbnail when a suitable thumbnail provider is installed; otherwise a file icon and available details. Installing an app does not necessarily install a thumbnail provider. |

Click the name, an individual detail value, or the full path to copy it. Copy targets follow the text area, and copying keeps the displayed path unchanged. Select and copy text directly from text/document excerpts. Previews stay hidden throughout drag and drop.

Turn off **Play Videos and Animations** to keep previews still, or turn off **File Preview** to hide the card. Hiding the Files panel also hides its previews and stops the mod's Everything file queries; the Everything application itself keeps running.

## Automatic Text and Icon Contrast

Leave **Settings → Appearance → Text Color** set to **auto** to adapt text and monochrome interface icons to their backgrounds. App and file icons keep their original colors. Enter a color such as **#202020** or **#FFFFFF** to use one fixed text color instead. Windows high contrast takes precedence.

One shared background map covers the search bar, both result panels, headings, and footer. Labels use strong local light or dark tones, and subtitles and hints stay opaque for readability. The map stays aligned while either panel scrolls and is reused as results change. Separate preview and context-menu popups adapt to their own backgrounds.

Background samples refresh up to four times per second while stationary and pause during opening, closing, scrolling, and drag and drop. Unchanged samples reuse the existing map. The menu continues to follow supported Start Menu Styler backgrounds; arbitrary custom effects may not be reproduced exactly.

## Folder Sizes in Preview

To show folder sizes, open **Everything → Tools → Options → Indexes**, enable **Index folder size**, and click **OK**. Let Everything finish updating its index, then select a folder with File Preview enabled. Folder sizes are read from Everything's index; the mod does not calculate them by scanning folders. Excluded files are not included. Without this option, previews can show item counts but no indexed folder size. See the [Everything documentation](https://www.voidtools.com/en-us/support/everything/indexes/).

## Requirements

1. **Windows 11 (x86-64)**: Tested on **25H2 and 26H2**. Other Windows versions and builds, including Insider builds, have not been verified.
2. **voidtools Everything**: **Everything 1.4** or **Everything 1.5**, including alpha/beta and named instances. Keep the application running in the tray; the service alone is not enough. Download from [voidtools](https://www.voidtools.com/).
3. **Windhawk**: Install [Windhawk](https://windhawk.net/) and load the mod.

## Taskbar Search

**Win+S**, the **taskbar search icon**, and **Start’s search bar** open search directly. Pressing **Win** or clicking the **Start button** opens Start normally; begin typing there to switch into this search. The full taskbar search box is not supported, so set Search to **Search icon only** or **Hide**:

1. Right-click an empty area on the Taskbar and select **Taskbar settings** (or navigate to **Settings** > **Personalization** > **Taskbar**).
2. Under **Taskbar items**, set **Search** to **Search icon only** or **Hide**.

## Keyboard Shortcuts and Navigation

To add another way to open search, go to **Settings → Apps & Search**, turn on **Enable Alternative Shortcut**, then choose **Shortcut Modifiers** and **Shortcut Key**. The default is **Alt+Space**, with the feature off until enabled. You can combine Ctrl, Alt, Shift, and Windows, or choose None; a key without modifiers is reserved even while typing in another app. The key list includes letters, digits, function, navigation, numpad, punctuation/OEM, browser, and media keys. F12 is reserved by Windows, and Fn/hardware-only keys cannot be registered. Punctuation labels use a US keyboard layout. If the combination belongs to Windows or another app, choose another; registration failures are recorded in the mod log. Changes apply immediately, and Win+S remains available.

| Shortcut / Input | Action |
| :--- | :--- |
| **Win+S / Taskbar search icon** | Open the search window directly |
| **Alternative shortcut (optional)** | Open and focus search using your chosen modifier and key combination |
| **Type in Start** | Close Start and carry the opening text into search |
| **Click Start's search bar** | Open and focus the search window |
| **Up / Down Arrow Keys** | Navigate selection through applications, utility cards, and files |
| **Tab / Shift + Tab** | Move to the next / previous result, through the apps and on into the files |
| **Left / Right Arrow Keys** | Switch between the Apps and Files columns (Right only with the cursor at the end of the query, so the arrows still move the cursor while you edit) |
| **Enter** | Launch selected application, copy calculation/conversion/IP result, or open item. Pressed before the results for what you typed are in, it opens the first one as soon as they arrive |
| **Ctrl + Enter** | Run selected application or file as Administrator (triggers UAC) |
| **Shift + Enter** | Open the selected result's context menu (same as right-click); navigate it with the arrow keys and Enter |
| **Escape** | Close an open context menu first; otherwise clear the query to suggestions, or close search when already empty |
| **Type while a context menu is open** | Close the menu and continue editing the query |
| **Right-Click** | Open the result's context menu |

Note on Pinning: Windows 11 blocks programmatic pinning to the Taskbar or Start Menu. Use Create desktop shortcut first, then right-click the shortcut on your desktop and select Pin to Taskbar or Pin to Start.

## Built-In Commands and Utilities

| Command | Description | Example | Enter Action |
| :--- | :--- | :--- | :--- |
| `<sum>` | Calculation | `100 * 5`<br>`sqrt(144)`<br>`15% of 200` | Copies the result |
| `<number> hex`, `0xFF`, `0b1010` | The number in other bases (digits grouped: `0b111 1111 1111`) | `2047 hex`<br>`0x7FF` | Copies the hex value |
| `<number> <unit>` | The number in the usual units of its kind, all on one line first | `100 km`<br>`32 f`<br>`1,5 kg` | Copies the selected one |
| `<number> <unit> to <unit>` | Just that conversion (also `in`, `->`, `=`) | `100 km to mi`<br>`100 mbps to MB/s` | Copies the result |
| `/ip` | Lists all active Wi-Fi, Ethernet, and VPN adapters with IP, subnet, gateway, and adapter name | `/ip` | Copies selected IP address to clipboard |
| `?<query>` | Searches the web using the default search engine URL | `?weather today` | Opens search in default web browser |
| `?<shortcut> <query>` | Searches specific service using configured shortcut keyword | `?yt lofi hip hop`<br>`?gh windhawk` | Opens query on target service |

## Troubleshooting and Compatibility

- **Everything is reported as not running:** start the Everything application and leave it running in the tray. Its service alone does not provide the search IPC window. Everything 1.4 and 1.5 alpha/beta, including named instances, are supported.
- **A preview shows only an icon:** enable File Preview. Common image formats have a built-in decoding fallback; specialist formats may require a thumbnail provider, and videos may require a compatible Windows codec. Unavailable or unreadable content falls back to its icon and available properties.
- **Folder sizes are missing:** enable Everything's **Index folder size** option; see [Folder Sizes in Preview](#folder-sizes-in-preview). The mod does not recursively scan folders to calculate sizes.
- **Start Menu Styler appearance is not copied yet:** open Start so its layout and styling can be measured. Supported solid, acrylic, gradient, and recognized glass definitions are copied; arbitrary Styler effects are not guaranteed to match.
- **A custom command does not start:** choose the shell that understands the command. PowerShell 7 and custom shells must already be installed. Hide Terminal hides the shell console; it does not shorten the command's lifetime or hide windows opened by the command.

Tested on **Windows 11 25H2 and 26H2, x86-64**. Other Windows versions and builds, including Insider builds, have not been verified. For a reproducible problem, include the full Windows build, mod version, Everything version and instance name, other Start/search mods, and steps to reproduce it.
## Upgrading from 1.x

Version 2.0 organizes settings into collapsible sections. Windhawk does not transfer preferences from the old flat setting names to these sections automatically. Before upgrading from 1.x, save your settings or note your choices, then reapply them in the new sections after updating. This includes web search shortcuts and excluded path patterns. Custom commands and custom unit conversions keep their existing setting names.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearance:
  - textScale: 100
    $name: Text Size (%)
    $description: Size of the search panel's text, as a percentage of the usual (80 to 150, default 100).
  - textColor: auto
    $name: Text Color
    $description: >-
      Leave auto to adapt text and interface icons using one background color map across the main search window. Text uses strong local light or dark tones without a fading gray crossover. Subtitles, hints and monochrome symbols stay opaque in automatic mode. The map stays aligned while scrolling and is reused when typing. Background captures refresh up to four times per second while stationary. Optionally enter #202020 (dark) or #FFFFFF (light) to force one color. Applies to search, preview details and context menus. Windows high contrast takes precedence.
  - showKeyHints: true
    $name: Show Keyboard Shortcuts Bar
    $description: Display the keyboard shortcut hints ([Up/Down] Select, [Tab] Next, [Enter] Open, [Ctrl+Enter] Admin, [Shift+Enter] Menu, [Esc] Close) in the bottom bar.
  - useCustomMenuSize: false
    $name: Use Custom Menu Size
    $description: Set the menu width and height below. Turn off to follow Start's size. The menu is always kept within the screen's available area.
  - menuWidth: 660
    $name: Menu Width
    $description: Width of the main menu at 100% display scaling (320 to 3840, default 660). Requires Use Custom Menu Size. This is the actual width even when the Files panel is hidden; the preview has its own space.
  - menuHeight: 660
    $name: Menu Height
    $description: Height of the main menu at 100% display scaling (240 to 2160, default 660). Requires Use Custom Menu Size.
  $name: Appearance

- search:
  - maxAppResults: 6
    $name: Max App Results
    $description: Number of application matches to display in the Apps column (default 6).
  - learnFavorites: true
    $name: Learn Favorite Apps
    $description: Apps and custom commands you open from here more often move up among results that match equally well. Turning this off stops counting and forgets learned usage for both apps and commands; turn it on again to start over.
  - searchDebounceMs: 0
    $name: Search Debounce Delay (ms)
    $description: Extra delay in milliseconds before searching, to let typing settle (default 0, instant). Rarely needed - while a search runs, new keystrokes already wait and only the latest text is searched.
  - enableAlternativeShortcut: false
    $name: Enable Alternative Shortcut
    $description: Open search with an additional keyboard shortcut. Win+S and the existing entry points remain available. Changes apply immediately. If another app or Windows reserves the combination, choose another.
  - shortcutModifiers: alt
    $name: Shortcut Modifiers
    $description: Modifier keys held with the key below. Requires Enable Alternative Shortcut. None reserves the selected key even while typing in other apps.
    $options:
    - alt: Alt
    - ctrl: Ctrl
    - shift: Shift
    - win: Windows
    - ctrl_alt: Ctrl + Alt
    - ctrl_shift: Ctrl + Shift
    - alt_shift: Alt + Shift
    - win_ctrl: Windows + Ctrl
    - win_alt: Windows + Alt
    - win_shift: Windows + Shift
    - ctrl_alt_shift: Ctrl + Alt + Shift
    - win_ctrl_alt: Windows + Ctrl + Alt
    - win_ctrl_shift: Windows + Ctrl + Shift
    - win_alt_shift: Windows + Alt + Shift
    - win_ctrl_alt_shift: Windows + Ctrl + Alt + Shift
    - none: None
  - shortcutKey: "0x20"
    $name: Shortcut Key
    $description: Key pressed with the selected modifiers. Includes letters, numbers, function keys, navigation, numpad, media, browser and OEM keys. F12 is reserved by Windows; Fn and hardware-only keys cannot be registered. Punctuation labels use a US keyboard layout.
    $options:
    - "0x20": "Space"
    - "0x41": "A"
    - "0x42": "B"
    - "0x43": "C"
    - "0x44": "D"
    - "0x45": "E"
    - "0x46": "F"
    - "0x47": "G"
    - "0x48": "H"
    - "0x49": "I"
    - "0x4A": "J"
    - "0x4B": "K"
    - "0x4C": "L"
    - "0x4D": "M"
    - "0x4E": "N"
    - "0x4F": "O"
    - "0x50": "P"
    - "0x51": "Q"
    - "0x52": "R"
    - "0x53": "S"
    - "0x54": "T"
    - "0x55": "U"
    - "0x56": "V"
    - "0x57": "W"
    - "0x58": "X"
    - "0x59": "Y"
    - "0x5A": "Z"
    - "0x30": "0"
    - "0x31": "1"
    - "0x32": "2"
    - "0x33": "3"
    - "0x34": "4"
    - "0x35": "5"
    - "0x36": "6"
    - "0x37": "7"
    - "0x38": "8"
    - "0x39": "9"
    - "0x70": "F1"
    - "0x71": "F2"
    - "0x72": "F3"
    - "0x73": "F4"
    - "0x74": "F5"
    - "0x75": "F6"
    - "0x76": "F7"
    - "0x77": "F8"
    - "0x78": "F9"
    - "0x79": "F10"
    - "0x7A": "F11"
    - "0x7C": "F13"
    - "0x7D": "F14"
    - "0x7E": "F15"
    - "0x7F": "F16"
    - "0x80": "F17"
    - "0x81": "F18"
    - "0x82": "F19"
    - "0x83": "F20"
    - "0x84": "F21"
    - "0x85": "F22"
    - "0x86": "F23"
    - "0x87": "F24"
    - "0x08": "Backspace"
    - "0x09": "Tab"
    - "0x0C": "Clear"
    - "0x0D": "Enter"
    - "0x13": "Pause / Break"
    - "0x14": "Caps Lock"
    - "0x1B": "Escape"
    - "0x21": "Page Up"
    - "0x22": "Page Down"
    - "0x23": "End"
    - "0x24": "Home"
    - "0x25": "Left Arrow"
    - "0x26": "Up Arrow"
    - "0x27": "Right Arrow"
    - "0x28": "Down Arrow"
    - "0x29": "Select"
    - "0x2A": "Print"
    - "0x2B": "Execute"
    - "0x2C": "Print Screen"
    - "0x2D": "Insert"
    - "0x2E": "Delete"
    - "0x2F": "Help"
    - "0x5D": "Menu / Apps"
    - "0x5F": "Sleep"
    - "0x60": "Numpad 0"
    - "0x61": "Numpad 1"
    - "0x62": "Numpad 2"
    - "0x63": "Numpad 3"
    - "0x64": "Numpad 4"
    - "0x65": "Numpad 5"
    - "0x66": "Numpad 6"
    - "0x67": "Numpad 7"
    - "0x68": "Numpad 8"
    - "0x69": "Numpad 9"
    - "0x6A": "Numpad Multiply"
    - "0x6B": "Numpad Add"
    - "0x6C": "Numpad Separator"
    - "0x6D": "Numpad Subtract"
    - "0x6E": "Numpad Decimal"
    - "0x6F": "Numpad Divide"
    - "0x90": "Num Lock"
    - "0x91": "Scroll Lock"
    - "0xA6": "Browser Back"
    - "0xA7": "Browser Forward"
    - "0xA8": "Browser Refresh"
    - "0xA9": "Browser Stop"
    - "0xAA": "Browser Search"
    - "0xAB": "Browser Favorites"
    - "0xAC": "Browser Home"
    - "0xAD": "Volume Mute"
    - "0xAE": "Volume Down"
    - "0xAF": "Volume Up"
    - "0xB0": "Media Next Track"
    - "0xB1": "Media Previous Track"
    - "0xB2": "Media Stop"
    - "0xB3": "Media Play / Pause"
    - "0xB4": "Launch Mail"
    - "0xB5": "Launch Media Player"
    - "0xB6": "Launch App 1"
    - "0xB7": "Launch App 2"
    - "0xBA": "OEM 1 (; :)"
    - "0xBB": "OEM Plus (= +)"
    - "0xBC": "OEM Comma (, <)"
    - "0xBD": "OEM Minus (- _)"
    - "0xBE": "OEM Period (. >)"
    - "0xBF": "OEM 2 (/ ?)"
    - "0xC0": "OEM 3 (` ~)"
    - "0xDB": "OEM 4 ([ {)"
    - "0xDC": "OEM 5 (\\ |)"
    - "0xDD": "OEM 6 (] })"
    - "0xDE": "OEM 7 (single / double quote)"
    - "0xDF": "OEM 8"
    - "0xE2": "OEM 102 (extra angle / backslash key)"
    - "0x15": "IME Kana / Hangul"
    - "0x17": "IME Junja"
    - "0x18": "IME Final"
    - "0x19": "IME Hanja / Kanji"
    - "0x1C": "IME Convert"
    - "0x1D": "IME Nonconvert"
    - "0x1E": "IME Accept"
    - "0x1F": "IME Mode Change"
    - "0xF6": "Attention"
    - "0xF7": "CrSel"
    - "0xF8": "ExSel"
    - "0xF9": "Erase EOF"
    - "0xFA": "Play"
    - "0xFB": "Zoom"
    - "0xFD": "PA1"
    - "0xFE": "OEM Clear"
  $name: Apps & Search

- files:
  - hideFilesPanel: false
    $name: Hide Files Panel
    $description: Hide the Files column and pause file searches. In automatic sizing, the search window becomes half as wide.
  - maxFileResults: 12
    $name: Max File Results
    $description: Number of file matches to display in the Files column (default 12).
  - filePreview: true
    $name: File Preview
    $description: Show a preview of the selected file beside search - its thumbnail (pictures, video frames, PDF pages, SVG), or the start of a text file or document - with its details (type, size, dates, and per kind dimensions, length, artist, version or author) and its path. Click the name, a detail value, or the path to copy it; text previews also allow selecting and copying text. To show folder sizes, enable Index folder size in Everything > Tools > Options > Indexes.
  - animatePreview: true
    $name: Play Videos and Animations
    $description: In the file preview, play videos and animated GIF and WebP images - muted, on a loop - and turn through a PDF's first pages.
  - filterNoisyPaths: true
    $name: Filter Noisy Paths
    $description: Filter out deep build caches, version control internals, and temporary directories from file search results unless no other matches exist.
  - excludedPaths:
      - "\\node_modules\\"
      - "\\.git\\"
      - "\\.gradle\\"
      - "\\appdata\\local\\temp\\"
      - "\\appdata\\local\\packages\\"
      - "\\__pycache__\\"
      - "\\.venv\\"
      - "\\site-packages\\"
      - "\\.cache\\"
      - "\\build\\intermediates\\"
      - "\\obj\\debug\\"
      - "\\obj\\release\\"
      - "\\windows\\winsxs\\"
      - "\\windows\\servicing\\"
    $name: Excluded Path Patterns
    $description: >-
      Paths matching any of these substrings will be filtered out from file search results so build caches, dependencies, and internal system folders don't clutter matches.
  $name: Files & Preview

- web:
  - defaultSearchUrl: "https://duckduckgo.com/?q={q}"
    $name: Default Search Engine URL
    $description: >-
      The URL template for standard web searches. Use {q} for the query placeholder.
      Defaults to DuckDuckGo.
  - webShortcuts:
      - - prefix: "yt"
          $name: Shortcut Keyword
          $description: Keyword to trigger this search (e.g. "?yt music")
        - name: "YouTube"
          $name: Service Name
        - url: "https://www.youtube.com/results?search_query={q}"
          $name: Search URL
          $description: URL template with {q} placeholder
      - - prefix: "gh"
          $name: Shortcut Keyword
        - name: "GitHub"
          $name: Service Name
        - url: "https://github.com/search?q={q}"
          $name: Search URL
      - - prefix: "w"
          $name: Shortcut Keyword
        - name: "Wikipedia"
          $name: Service Name
        - url: "https://en.wikipedia.org/wiki/Special:Search?search={q}"
          $name: Search URL
      - - prefix: "r"
          $name: Shortcut Keyword
        - name: "Reddit"
          $name: Service Name
        - url: "https://www.reddit.com/search/?q={q}"
          $name: Search URL
    $name: Web Search Shortcuts
    $description: >-
      Keywords that send a query to a specific site instead of the default search engine, typed as "?keyword query" (for example "?yt music").
  $name: Web Search

- customCommands:
    - - name: ""
        $name: Name
        $description: Name shown in the Apps panel and used to find this command.
      - icon: ""
        $name: Icon
        $description: Paste an emoji or Windows icon glyph, enter a Unicode value such as E713, or use a full path to an image, icon, or app file. Leave empty for the terminal icon. Environment variables in paths are supported.
      - shell: cmd
        $name: Shell
        $description: Choose the shell that understands your command. PowerShell 7 must be installed separately. Custom shell uses the executable and arguments below.
        $options:
        - cmd: Command Prompt (CMD)
        - powershell: Windows PowerShell
        - pwsh: PowerShell 7
        - custom: Custom shell
      - shellExecutable: ""
        $name: Shell Executable
        $description: For Custom shell only. Enter the full path to its executable, or a name available in PATH, such as nu.exe or wsl.exe. Environment variables such as %ProgramFiles% are supported.
      - shellArguments: "-c {command}"
        $name: Shell Arguments
        $description: >-
          For Custom shell only. Put {command} once where the shell expects the command, without adding quotes around the placeholder. It is passed as one argument automatically. Examples include -c {command} for Bash or Nushell and --exec bash -lc {command} for WSL. Environment variables in these arguments are expanded before inserting the command.
      - hideTerminal: false
        $name: Hide Terminal
        $description: Run without showing the terminal. Programs opened by the command can still show their own windows.
      - command: ""
        $name: Command
        $description: Command to run in the selected shell, exactly as entered. Blank names or commands are ignored.
  $name: Custom Commands
  $description: Add named commands to the Apps panel. They share favorite ranking with installed apps, including when search is empty. Right-click to run as administrator or copy the command. Up to 100 commands are supported.

- unitConversions:
    - - fromUnit: "px"
        $name: Source Unit
        $description: The unit typed after the number (e.g. px)
      - toUnit: "em"
        $name: Target Unit
      - formula: "x / 16"
        $name: Formula
        $description: Formula using 'x' as input number
      - category: "CSS"
        $name: Category
      - reciprocal: true
        $name: Reciprocal
        $description: Also convert back, from the target unit to the source unit. For a linear formula (a * x + b), as a unit conversion is.
  $name: Custom Unit Conversions
  $description: >-
    Units the converter does not know already - it knows length, mass, temperature, speed, area, volume, data, data rate, time, pressure, energy, power, angle and frequency.
    Formulas use 'x' as the input number. Add, edit or remove items at any time.
*/
// ==/WindhawkModSettings==

// Defines the GUIDs the headers below declare, so only the used ones are
// linked instead of whole objects from libuuid.
#include <initguid.h>

#include <inspectable.h>

// winbase.h defines GetCurrentTime as a macro, which collides with
// Windows.UI.Xaml.Media.Animation's method of the same name.
#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
// Not just the .0.h forward declarations: Append/Size have deduced return
// types and must be defined before use.
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <shlguid.h>
#include <exdisp.h>
#include <shldisp.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <appmodel.h>
#include <filter.h>
#include <filterr.h>
#include <string_view>
#include <limits>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <thread>

inline HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle), &module);
    return module;
}

// ===========================================================================
// Component: Everything IPC Client
// ===========================================================================
// Everything IPC client.
//
// Everything exposes a hidden window that answers WM_COPYDATA queries. This
// is the only transport worth shipping: the HTTP server is off by default and
// the SDK DLL would have to be redistributed, whereas the IPC window is there
// whenever Everything is running.
//
// Two query messages exist. QUERY2 is preferred: it carries a sort order and
// returns size, dates, attributes and run count, which the ranker needs.
// QUERYW is the original and is kept as a fallback for older builds -- it
// returns names and paths only, in whatever order the Everything window
// happens to be sorted by.
//
// The struct layouts were taken from the official everything_ipc.h shipped
// with the "es" client, then confirmed against a live reply, because that
// header documents the QUERY2 field order in a sequence that does not match
// the flag bit order. They are ABI: a wrong offset reads arbitrary bytes out
// of a buffer another process filled in, so every read here is bounds-checked
// rather than trusted.


#include <windows.h>

#include <string>
#include <vector>

namespace everything {

// Window class of Everything's IPC listener.
inline constexpr wchar_t kIpcWindowClass[] = L"EVERYTHING_TASKBAR_NOTIFICATION";

// WM_COPYDATA dwData values for the two query messages.
inline constexpr DWORD kCopyDataQueryW = 2;
inline constexpr DWORD kCopyDataQuery2W = 18;

// Search flags. Worth spelling out rather than trusting memory: match-path is
// 0x4, while 0x2 is match-whole-word. Confusing the two does not fail, it
// silently changes what every query means.
inline constexpr DWORD kMatchCase = 0x00000001;
inline constexpr DWORD kMatchWholeWord = 0x00000002;
inline constexpr DWORD kMatchPath = 0x00000004;
inline constexpr DWORD kRegex = 0x00000008;

// max_results sentinel.
inline constexpr DWORD kAllResults = 0xFFFFFFFF;

// Item flags in a reply.
inline constexpr DWORD kItemFolder = 0x00000001;
inline constexpr DWORD kItemDrive = 0x00000002;

// QUERY2 request flags. Only the subset below is supported, and that is
// deliberate: the official header lists the fields inside the data blob in an
// order that disagrees with their bit order for EXTENSION, TYPE_NAME and the
// highlighted variants. For every flag named here the two orders agree, so
// the blob can be walked in ascending bit order without guessing. Adding one
// of the others means re-establishing the order against a live reply first.
inline constexpr DWORD kReqName = 0x00000001;
inline constexpr DWORD kReqPath = 0x00000002;
inline constexpr DWORD kReqFullPath = 0x00000004;
inline constexpr DWORD kReqSize = 0x00000010;
inline constexpr DWORD kReqDateCreated = 0x00000020;
inline constexpr DWORD kReqDateModified = 0x00000040;
inline constexpr DWORD kReqDateAccessed = 0x00000080;
inline constexpr DWORD kReqAttributes = 0x00000100;
inline constexpr DWORD kReqRunCount = 0x00000400;
inline constexpr DWORD kReqDateRun = 0x00000800;

inline constexpr DWORD kReqSupported =
    kReqName | kReqPath | kReqFullPath | kReqSize | kReqDateCreated |
    kReqDateModified | kReqDateAccessed | kReqAttributes | kReqRunCount |
    kReqDateRun;

// What the panel shows, plus what the ranker sorts on.
inline constexpr DWORD kDefaultRequestFlags = kReqName | kReqPath | kReqSize |
                                              kReqDateModified |
                                              kReqAttributes | kReqRunCount;

// Sort orders. Name-ascending is the only one guaranteed instant; the others
// are fast only when the matching fast-sort is enabled in Everything's
// options, which cannot be assumed on a user machine. So the client asks for
// name-ascending and the ranking happens here.
inline constexpr DWORD kSortNameAscending = 1;
inline constexpr DWORD kSortDateModifiedDescending = 14;
inline constexpr DWORD kSortRunCountDescending = 20;

#pragma pack(push, 1)
struct QueryHeaderW {
    DWORD reply_hwnd;              // truncated HWND; 32 bits suffice on x64
    DWORD reply_copydata_message;  // dwData Everything uses on the reply
    DWORD search_flags;
    DWORD offset;
    DWORD max_results;
    // followed by the null-terminated search string
};

struct ListHeaderW {
    DWORD totfolders;
    DWORD totfiles;
    DWORD totitems;
    DWORD numfolders;
    DWORD numfiles;
    DWORD numitems;
    DWORD offset;
    // followed by numitems ItemW, then the string pool
};

struct ItemW {
    DWORD flags;
    DWORD filename_offset;  // byte offset from the start of ListHeaderW
    DWORD path_offset;
};

struct Query2HeaderW {
    DWORD reply_hwnd;
    DWORD reply_copydata_message;
    DWORD search_flags;
    DWORD offset;
    DWORD max_results;
    DWORD request_flags;
    DWORD sort_type;
    // followed by the null-terminated search string
};

struct List2HeaderW {
    DWORD totitems;
    DWORD numitems;
    DWORD offset;
    DWORD request_flags;  // what Everything actually honoured
    DWORD sort_type;      // may differ from what was asked for
    // followed by numitems Item2, then the per-item data blobs
};

struct Item2 {
    DWORD flags;
    DWORD data_offset;  // byte offset from the start of List2HeaderW
};
#pragma pack(pop)

struct Result {
    std::wstring name;
    std::wstring path;
    bool isFolder = false;
    ULONGLONG size = 0;
    FILETIME dateModified{};
    DWORD attributes = 0;
    DWORD runCount = 0;
};

// Is Everything running and listening? A named instance listens under the
// class name with its name appended in brackets, as voidtools' es.exe looks
// for it with -instance -- and Everything 1.5 alpha runs as the instance
// "1.5a" unless told otherwise, so for most 1.5a users the plain name finds
// nothing. The plain name first, then 1.5a, then any other instance.
inline HWND FindIpcWindow() {
    if (HWND hwnd = FindWindowW(kIpcWindowClass, nullptr)) {
        return hwnd;
    }
    if (HWND hwnd = FindWindowW(L"EVERYTHING_TASKBAR_NOTIFICATION_(1.5a)", nullptr)) {
        return hwnd;
    }
    static const wchar_t kInstancePrefix[] = L"EVERYTHING_TASKBAR_NOTIFICATION_(";
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, nullptr, nullptr); hwnd;
         hwnd = FindWindowExW(nullptr, hwnd, nullptr, nullptr)) {
        wchar_t name[128] = {};
        if (GetClassNameW(hwnd, name, ARRAYSIZE(name)) &&
            _wcsnicmp(name, kInstancePrefix, ARRAYSIZE(kInstancePrefix) - 1) == 0) {
            return hwnd;
        }
    }
    return nullptr;
}

namespace detail {

// A bounds-checked walk over a reply buffer. Once a read runs past the end
// the cursor latches failed, so a truncated or mismatched reply yields no
// results instead of reading whatever happens to follow it in memory.
class Cursor {
   public:
    Cursor(const BYTE* base, size_t size, size_t pos)
        : base_(base), size_(size), pos_(pos), ok_(pos <= size) {}

    bool ok() const { return ok_; }

    template <typename T>
    bool Read(T* out) {
        if (!ok_ || pos_ + sizeof(T) > size_) {
            ok_ = false;
            return false;
        }
        memcpy(out, base_ + pos_, sizeof(T));
        pos_ += sizeof(T);
        return true;
    }

    // A length-prefixed, null-terminated wide string: a DWORD character count
    // excluding the terminator, then count+1 characters.
    bool ReadString(std::wstring* out) {
        DWORD chars = 0;
        if (!Read(&chars)) {
            return false;
        }
        // Cap the count before it is multiplied: a corrupt length could
        // otherwise wrap the size computation and slip past the bounds check.
        if (chars > (1u << 20)) {
            ok_ = false;
            return false;
        }
        size_t bytes = (static_cast<size_t>(chars) + 1) * sizeof(wchar_t);
        if (pos_ + bytes > size_) {
            ok_ = false;
            return false;
        }
        out->assign(reinterpret_cast<const wchar_t*>(base_ + pos_), chars);
        pos_ += bytes;
        return true;
    }

    bool Skip(size_t bytes) {
        if (!ok_ || pos_ + bytes > size_) {
            ok_ = false;
            return false;
        }
        pos_ += bytes;
        return true;
    }

   private:
    const BYTE* base_;
    size_t size_;
    size_t pos_;
    bool ok_;
};

}  // namespace detail

// Parses a QUERY2 reply.
inline bool ParseReply2(const void* data, DWORD size, std::vector<Result>* out,
                        DWORD* totalMatches) {
    if (!data || size < sizeof(List2HeaderW)) {
        return false;
    }
    const auto* base = static_cast<const BYTE*>(data);
    List2HeaderW list{};
    memcpy(&list, base, sizeof(list));

    if (totalMatches) {
        *totalMatches = list.totitems;
    }
    // Anything Everything honoured that this parser does not know the
    // position of means the blob cannot be walked safely.
    if (list.request_flags & ~kReqSupported) {
        return false;
    }
    size_t itemsEnd = sizeof(List2HeaderW) +
                      static_cast<size_t>(list.numitems) * sizeof(Item2);
    if (list.numitems > 100000 || itemsEnd > size) {
        return false;
    }

    for (DWORD i = 0; i < list.numitems; i++) {
        Item2 item{};
        memcpy(&item, base + sizeof(List2HeaderW) + i * sizeof(Item2),
               sizeof(item));

        detail::Cursor c(base, size, item.data_offset);
        Result r;
        r.isFolder = (item.flags & kItemFolder) != 0;

        // Ascending bit order, which for this flag subset is also the order
        // the fields appear in.
        std::wstring scratch;
        if ((list.request_flags & kReqName) && !c.ReadString(&r.name)) break;
        if ((list.request_flags & kReqPath) && !c.ReadString(&r.path)) break;
        if ((list.request_flags & kReqFullPath) && !c.ReadString(&scratch)) break;
        if ((list.request_flags & kReqSize) && !c.Read(&r.size)) break;
        if ((list.request_flags & kReqDateCreated) && !c.Skip(sizeof(FILETIME))) break;
        if ((list.request_flags & kReqDateModified) && !c.Read(&r.dateModified)) break;
        if ((list.request_flags & kReqDateAccessed) && !c.Skip(sizeof(FILETIME))) break;
        if ((list.request_flags & kReqAttributes) && !c.Read(&r.attributes)) break;
        if ((list.request_flags & kReqRunCount) && !c.Read(&r.runCount)) break;
        if ((list.request_flags & kReqDateRun) && !c.Skip(sizeof(FILETIME))) break;

        if (!c.ok()) {
            break;
        }
        out->push_back(std::move(r));
    }
    return true;
}

// Parses a legacy QUERYW reply.
inline bool ParseReply(const void* data, DWORD size, std::vector<Result>* out,
                       DWORD* totalMatches) {
    if (!data || size < sizeof(ListHeaderW)) {
        return false;
    }
    const auto* base = static_cast<const BYTE*>(data);
    ListHeaderW list{};
    memcpy(&list, base, sizeof(list));

    size_t itemsEnd = sizeof(ListHeaderW) +
                      static_cast<size_t>(list.numitems) * sizeof(ItemW);
    if (list.numitems > 100000 || itemsEnd > size) {
        return false;
    }
    if (totalMatches) {
        *totalMatches = list.totitems;
    }

    for (DWORD i = 0; i < list.numitems; i++) {
        ItemW it{};
        memcpy(&it, base + sizeof(ListHeaderW) + i * sizeof(ItemW), sizeof(it));
        if (it.filename_offset >= size || it.path_offset >= size) {
            return false;  // layout mismatch; refuse the whole reply
        }
        auto readString = [&](DWORD offset) -> std::wstring {
            const auto* p = reinterpret_cast<const wchar_t*>(base + offset);
            DWORD maxChars = (size - offset) / sizeof(wchar_t);
            DWORD n = 0;
            while (n < maxChars && p[n]) {
                n++;
            }
            return std::wstring(p, n);
        };
        Result r;
        r.name = readString(it.filename_offset);
        r.path = readString(it.path_offset);
        r.isFolder = (it.flags & kItemFolder) != 0;
        out->push_back(std::move(r));
    }
    return true;
}

// Synchronous query. Creates a hidden window, sends the request, and pumps
// until the reply arrives or the timeout expires.
class Client {
   public:
    // Each thread that queries has a client of its own, under a class name of
    // its own: the search thread's and the preview thread's (FolderFacts).
    explicit Client(const wchar_t* className = kReplyClass) : className_(className) {}
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    ~Client() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
        }
        if (atom_) {
            UnregisterClassW(className_, GetCurrentModuleHandle());
        }
    }

    bool Init() {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = &Client::WndProc;
        wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = className_;
        atom_ = RegisterClassExW(&wc);
        if (!atom_) {
            return false;
        }
        hwnd_ = CreateWindowExW(0, className_, L"", WS_POPUP, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, wc.hInstance, this);
        if (!hwnd_) {
            return false;
        }

        // No message filter: Everything replies from the same integrity level
        // as Start or a higher one, which UIPI lets through, and allowing
        // lower ones would only let a sandboxed process hand us results.
        return true;
    }

    bool Query(const std::wstring& text, DWORD maxResults,
               std::vector<Result>* out, DWORD* totalMatches,
               DWORD timeoutMs = 500,
               DWORD requestFlags = kDefaultRequestFlags,
               DWORD sortType = kSortNameAscending) {
        HWND everything = FindIpcWindow();
        if (!everything || !hwnd_) {
            return false;
        }

        if (SendQuery2(everything, text, maxResults, requestFlags, sortType) &&
            Await(timeoutMs)) {
            *out = std::move(results_);
            if (totalMatches) {
                *totalMatches = total_;
            }
            return true;
        }

        // Older Everything builds answer FALSE to QUERY2. Names and paths
        // still come back through the original message; the ranker then has
        // only the name to score on.
        if (!SendQueryLegacy(everything, text, maxResults) ||
            !Await(timeoutMs)) {
            return false;
        }
        *out = std::move(results_);
        if (totalMatches) {
            *totalMatches = total_;
        }
        return true;
    }

   private:
    static constexpr wchar_t kReplyClass[] = L"WindhawkEverythingBrokerReply";
    const wchar_t* className_ = kReplyClass;
    static constexpr DWORD kReplyIdBase = 0x45560000;  // EV, then a serial

    // Everything echoes the id as the reply's dwData. A new one per query means
    // a late reply to a query that timed out can't pass for the next one's.
    void Reset(bool query2) {
        results_.clear();
        total_ = 0;
        replied_ = false;
        expecting_ = kReplyIdBase | (++serial_ & 0xFFFF);
        expectingQuery2_ = query2;
    }

    bool SendQuery2(HWND everything, const std::wstring& text,
                    DWORD maxResults, DWORD requestFlags, DWORD sortType) {
        Reset(true);
        std::vector<BYTE> buffer(sizeof(Query2HeaderW) +
                                 (text.size() + 1) * sizeof(wchar_t));
        auto* q = reinterpret_cast<Query2HeaderW*>(buffer.data());
        q->reply_hwnd = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hwnd_));
        q->reply_copydata_message = expecting_;
        q->search_flags = 0;
        q->offset = 0;
        q->max_results = maxResults;
        q->request_flags = requestFlags & kReqSupported;
        q->sort_type = sortType;
        memcpy(buffer.data() + sizeof(Query2HeaderW), text.c_str(),
               (text.size() + 1) * sizeof(wchar_t));
        return Send(everything, kCopyDataQuery2W, buffer);
    }

    bool SendQueryLegacy(HWND everything, const std::wstring& text,
                         DWORD maxResults) {
        Reset(false);
        std::vector<BYTE> buffer(sizeof(QueryHeaderW) +
                                 (text.size() + 1) * sizeof(wchar_t));
        auto* q = reinterpret_cast<QueryHeaderW*>(buffer.data());
        q->reply_hwnd = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hwnd_));
        q->reply_copydata_message = expecting_;
        q->search_flags = 0;
        q->offset = 0;
        q->max_results = maxResults;
        memcpy(buffer.data() + sizeof(QueryHeaderW), text.c_str(),
               (text.size() + 1) * sizeof(wchar_t));
        return Send(everything, kCopyDataQueryW, buffer);
    }

    bool Send(HWND everything, DWORD message, std::vector<BYTE>& buffer) {
        COPYDATASTRUCT cds{};
        cds.dwData = message;
        cds.cbData = static_cast<DWORD>(buffer.size());
        cds.lpData = buffer.data();
        DWORD_PTR result = 0;
        return SendMessageTimeoutW(everything, WM_COPYDATA,
                                   reinterpret_cast<WPARAM>(hwnd_),
                                   reinterpret_cast<LPARAM>(&cds),
                                   SMTO_ABORTIFHUNG, 500,
                                   &result) != 0;
    }

    // Everything answers with a sent (not posted) message, so this thread has
    // to be inside a message-retrieval call for the reply to be delivered.
    //
    // This used to spin on Sleep(1). That looked harmless and was not: the
    // default system timer tick is about 15.6 ms, so Sleep(1) sleeps for a
    // tick, and every query appeared to cost 15, 30 or 45 ms. A search with
    // zero matches measured 30 ms -- all of it this loop. Blocking on the
    // message queue instead removes that floor entirely.
    bool Await(DWORD timeoutMs) {
        const DWORD start = GetTickCount();
        MSG msg;
        for (;;) {
            // Drain first. The reply may already be waiting, and blocking
            // before draining would wait for the message after it.
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (replied_) {
                return true;
            }
            const DWORD elapsed = GetTickCount() - start;
            if (elapsed >= timeoutMs) {
                return false;
            }
            MsgWaitForMultipleObjectsEx(0, nullptr, timeoutMs - elapsed,
                                        QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        }
    }

    static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
        if (m == WM_CREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(l);
            SetWindowLongPtrW(h, GWLP_USERDATA,
                              reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return 0;
        }
        if (m == WM_COPYDATA) {
            auto* self =
                reinterpret_cast<Client*>(GetWindowLongPtrW(h, GWLP_USERDATA));
            auto* cds = reinterpret_cast<COPYDATASTRUCT*>(l);
            if (self && cds && cds->dwData == self->expecting_) {
                if (self->expectingQuery2_) {
                    ParseReply2(cds->lpData, cds->cbData, &self->results_,
                                &self->total_);
                } else {
                    ParseReply(cds->lpData, cds->cbData, &self->results_,
                               &self->total_);
                }
                self->replied_ = true;
                return TRUE;
            }
        }
        return DefWindowProcW(h, m, w, l);
    }

    ATOM atom_ = 0;
    HWND hwnd_ = nullptr;
    std::vector<Result> results_;
    DWORD total_ = 0;
    DWORD expecting_ = 0;
    DWORD serial_ = 0;
    bool expectingQuery2_ = false;
    bool replied_ = false;
};

}  // namespace everything
// ===========================================================================
// Component: Everything Relevance Ranker
// ===========================================================================
// Relevance ordering for Everything results.
//
// Everything is asked for results sorted by name ascending, because that is
// the only sort it guarantees is instant -- every other sort is fast only if
// the user happens to have the matching fast-sort enabled in Tools ->
// Options -> Indexes, which a mod cannot assume. Name-ascending is useless as
// a presentation order, though: a search for "code" leads with
// "-abstract-code-quality-task" out of a Gradle docs folder.
//
// So the client over-fetches a pool and this reorders it.
//
// The ordering is a lexicographic key rather than a sum of weights. A sum
// reads naturally but behaves badly here: the first attempt added small
// bonuses for recency and run count on top of a match score, and because
// thousands of results tie on the match score and most have a run count of
// zero, the top eight for "code" came back as eight different folders all
// literally named "Code", ordered by nothing meaningful. A key makes the
// tie-breaks explicit and total: how well the name matched, then how often
// the user has opened it, then how recently it changed.
//
// Known limitation: the pool is the alphabetically first N matches, so
// ranking can only reorder what that window happened to catch. A very
// recently edited file whose name sorts late loses to worse matches that sort
// early, and for a query with thousands of hits the window is a small
// fraction of them. Fixing it properly needs a sort Everything can do
// server-side, which means date-modified-descending -- fast only when the
// user has that fast-sort enabled, and silently slow when they have not. The
// honest options are to keep this, or to probe the cost of the date sort once
// at startup and use it when it turns out to be cheap. Not decided yet.


#include <windows.h>

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>


namespace ranker {

// How many results to ask Everything for before ranking. The pool has to be
// much larger than what is displayed or ranking has nothing to work with: at
// a pool of 10, a name-ascending query for "code" only ever sees names
// starting with punctuation.
//
// Measured on this machine (best of 3, ms, blocking wait):
//
//   query          total     8    50   200   300   500  1000
//   c             589945  11.3  11.7  16.6  48.6  65.7 145.6
//   code            8365  30.1  28.4  32.8  39.3  44.3  51.4
//   readme          2434  29.6  28.0  32.8  37.6  39.2  50.3
//   (no matches)       0  22.6  23.0  23.5  27.1  24.6  25.2
//
// So about 23 ms is a fixed cost inside Everything that no pool size avoids,
// and 200 is the largest pool that stays close to it for every query
// including a single letter. 300 is already 48 ms on "c" and 1000 is far too
// slow to run per keystroke.
inline constexpr DWORD kDefaultPool = 200;

inline std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return s;
}

namespace detail {

inline std::wstring TrimSlashes(std::wstring_view s) {
    while (!s.empty() && (s.front() == L'\\' || s.front() == L'/')) s.remove_prefix(1);
    while (!s.empty() && (s.back() == L'\\' || s.back() == L'/')) s.remove_suffix(1);
    return std::wstring(s);
}

// Match quality, coarse buckets, lower is better. This is the primary key and
// the only signal available when Everything is too old for QUERY2.
enum MatchClass : int {
    kExactName = 0,
    kNamePrefix = 1,
    kNameWordStart = 2,
    kNameSubstring = 3,
    kPathOnly = 4,
};

inline int Classify(const std::wstring& nameLower, const std::wstring& q) {
    size_t pos = nameLower.find(q);
    if (pos == std::wstring::npos) {
        return kPathOnly;
    }
    if (pos == 0) {
        return nameLower.size() == q.size() ? kExactName : kNamePrefix;
    }
    wchar_t prev = nameLower[pos - 1];
    bool wordStart = prev == L' ' || prev == L'-' || prev == L'_' ||
                     prev == L'.' || prev == L'(' || prev == L'[';
    return wordStart ? kNameWordStart : kNameSubstring;
}

// Checks if an item belongs to any excluded noisy path pattern.
// Handles directory paths with or without trailing slashes, full item paths,
// and skips penalization if the user explicitly typed the keyword in their query.
inline bool IsNoise(const std::wstring& pathLower, const std::wstring& nameLower, bool isFolder,
                    const std::wstring& qLower,
                    const std::vector<std::wstring>* customNoise = nullptr) {
    if (!customNoise || customNoise->empty()) return false;

    // Normalised directory path with trailing backslash
    std::wstring dir = pathLower;
    if (dir.empty() || dir.back() != L'\\') {
        dir.push_back(L'\\');
    }

    // Normalised full item path with trailing backslash for directories
    std::wstring full = dir + nameLower;
    if (isFolder) {
        full.push_back(L'\\');
    }

    for (const auto& n : *customNoise) {
        if (n.empty()) continue;

        // If the query explicitly targets this noise pattern, don't penalize it
        std::wstring raw = TrimSlashes(n);
        if (!raw.empty() && qLower.find(raw) != std::wstring::npos) {
            continue;
        }

        // Direct substring check on dir or full item path
        if (dir.find(n) != std::wstring::npos || full.find(n) != std::wstring::npos) {
            return true;
        }
        // Match with path delimiters around raw pattern
        if (!raw.empty()) {
            std::wstring bounded = L"\\" + raw + L"\\";
            if (dir.find(bounded) != std::wstring::npos || full.find(bounded) != std::wstring::npos) {
                return true;
            }
        }
    }
    return false;
}

inline ULONGLONG AsTicks(const FILETIME& ft) {
    ULARGE_INTEGER v{};
    v.LowPart = ft.dwLowDateTime;
    v.HighPart = ft.dwHighDateTime;
    return v.QuadPart;
}

// Sort key. Descending fields are stored pre-negated so the comparison is a
// plain ascending tuple compare -- writing the descending fields by swapping
// this and other inside std::tie does work, but it is the kind of expression
// nobody can check at a glance.
struct Key {
    int matchClass;         // ascending: better match first
    ULONGLONG runsDesc;     // negated run count: more opens first
    ULONGLONG modifiedDesc; // negated timestamp: more recent first
    size_t index;           // keeps the order total and reproducible

    bool operator<(const Key& o) const {
        return std::tie(matchClass, runsDesc, modifiedDesc, index) <
               std::tie(o.matchClass, o.runsDesc, o.modifiedDesc, o.index);
    }
};

// Turns a "higher is better" value into a sort key.
inline ULONGLONG Descending(ULONGLONG v) {
    return ~0ull - v;
}

}  // namespace detail

// Reorders a pool in place and truncates it to limit. Returns how many of the
// results kept are clean (not under an excluded path); when that is zero, all
// that was left was noise and the pool holds it.
inline size_t Rank(std::vector<everything::Result>* pool,
                   const std::wstring& query, size_t limit,
                   const std::vector<std::wstring>* excludedPaths = nullptr) {
    if (!pool || pool->empty()) {
        return 0;
    }
    const std::wstring q = ToLower(query);

    std::vector<detail::Key> keys;
    keys.reserve(pool->size());
    for (size_t i = 0; i < pool->size(); i++) {
        const everything::Result& r = (*pool)[i];
        int cls = detail::Classify(ToLower(r.name), q);
        // Demote noisy locations by 100 so all non-noise matches rank above them.
        if (detail::IsNoise(ToLower(r.path), ToLower(r.name), r.isFolder, q, excludedPaths)) {
            cls += 100;
        }
        // Everything only counts opens that went through Everything itself,
        // so this is sparse -- but where it is set it is the strongest
        // evidence available that the user wants this particular file. Capped
        // so one heavily used file cannot dominate a whole class.
        ULONGLONG runs = r.runCount > 50 ? 50 : r.runCount;
        keys.push_back({cls, detail::Descending(runs),
                        detail::Descending(detail::AsTicks(r.dateModified)), i});
    }

    std::sort(keys.begin(), keys.end());

    // If we have clean (non-noisy) matches, do not pollute remaining slots with noisy items!
    bool hasCleanMatches = !keys.empty() && keys.front().matchClass < 100;

    std::vector<everything::Result> ranked;
    ranked.reserve(keys.size() < limit ? keys.size() : limit);
    for (size_t i = 0; i < keys.size() && ranked.size() < limit; i++) {
        if (hasCleanMatches && keys[i].matchClass >= 100) {
            break;
        }
        ranked.push_back(std::move((*pool)[keys[i].index]));
    }
    *pool = std::move(ranked);
    return hasCleanMatches ? pool->size() : 0;
}

// The excluded paths as Everything search terms, appended to the query, for
// when Rank alone cannot help: it can only demote what the query returned,
// the first kDefaultPool matches by name, and for a broad query those can all
// be noise (a one-letter search starts with "-x-..." folders under .gradle).
//
// `!path:"<pattern>"` rules out the pattern anywhere in the full path, as
// IsNoise does -- but only single-folder patterns are sent. Measured on this
// machine for "c" (700k matches, 19 ms as typed), one like \.gradle\ adds
// about 10 ms, while one spanning folders like \appdata\local\temp\ adds 60-80
// ms, all six of the defaults together about 380 ms. Those stay with IsNoise,
// which applies every pattern exactly to whatever comes back. A pattern the
// query itself names is left out, as IsNoise leaves it undemoted.
inline std::wstring WithExclusions(const std::wstring& query,
                                   const std::vector<std::wstring>* excludedPaths) {
    if (!excludedPaths || excludedPaths->empty()) {
        return query;
    }
    const std::wstring q = ToLower(query);
    std::wstring out = query;
    for (const auto& n : *excludedPaths) {
        std::wstring raw = detail::TrimSlashes(n);
        if (raw.empty() || raw.find(L'\\') != std::wstring::npos ||
            q.find(raw) != std::wstring::npos) {
            continue;
        }
        std::wstring term;
        for (wchar_t c : n) {
            if (c != L'"') term.push_back(c);
        }
        out += L" !path:\"" + term + L"\"";
    }
    return out;
}

}  // namespace ranker
// ===========================================================================
// Component: Shell Icon Utilities
// ===========================================================================
// Turning shell icons into the pixel format the panel can draw.
//
// The panel lives in an AppContainer with no shell namespace and no file
// access, so it cannot fetch an icon for anything. Icons have to arrive as
// raw pixels over the same message that carries the rows: BGRA, top-down,
// premultiplied, which is what a XAML WriteableBitmap expects.

#include <commoncontrols.h>
#include <shellapi.h>
#include <shlobj.h>

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace icons {

// Icons are made at exactly the size they are drawn, in physical pixels, so
// XAML puts them on screen pixel for pixel. Left to XAML, a 48px icon shown at
// 24 was resampled on the GPU, and the shell's own resizing is a plain GDI
// stretch: both left jagged edges. Instead the shell is asked for its nearest
// frame at or above the size, and Resample shrinks that by averaging, which
// keeps the edges smooth.

// The sources of one output pixel along one axis, and how much of it each covers.
struct Tap {
    int index;
    float weight;
};

inline std::vector<std::vector<Tap>> AxisTaps(int from, int to) {
    std::vector<std::vector<Tap>> taps(to);
    const double scale = static_cast<double>(from) / to;
    for (int o = 0; o < to; ++o) {
        if (to <= from) {
            // Shrinking: the average of the source pixels the output pixel
            // covers, the ones at its edges weighted by how much of them.
            const double begin = o * scale, end = begin + scale;
            for (int i = static_cast<int>(begin); i < from && i < end; ++i) {
                const double cover = std::min<double>(i + 1, end) - std::max<double>(i, begin);
                if (cover > 1e-6) {
                    taps[o].push_back({i, static_cast<float>(cover / scale)});
                }
            }
        } else {
            // Growing (an icon with no frame that large): linear between the
            // two nearest source pixels.
            const double x = (o + 0.5) * scale - 0.5;
            const int left = static_cast<int>(std::floor(x));
            const float t = static_cast<float>(x - left);
            taps[o].push_back({std::clamp(left, 0, from - 1), 1.0f - t});
            taps[o].push_back({std::clamp(left + 1, 0, from - 1), t});
        }
    }
    return taps;
}

// Resizes a premultiplied BGRA image, one axis at a time.
inline std::vector<BYTE> Resample(const std::vector<BYTE>& src, int fromW, int fromH, int toW, int toH) {
    if (fromW == toW && fromH == toH) {
        return src;
    }
    const auto tapsX = AxisTaps(fromW, toW);
    const auto tapsY = AxisTaps(fromH, toH);
    std::vector<float> rows(static_cast<size_t>(fromH) * toW * 4);
    for (int y = 0; y < fromH; ++y) {
        for (int x = 0; x < toW; ++x) {
            float* d = &rows[(static_cast<size_t>(y) * toW + x) * 4];
            for (const Tap& t : tapsX[x]) {
                const BYTE* s = &src[(static_cast<size_t>(y) * fromW + t.index) * 4];
                for (int c = 0; c < 4; ++c) {
                    d[c] += s[c] * t.weight;
                }
            }
        }
    }
    std::vector<BYTE> out(static_cast<size_t>(toW) * toH * 4);
    for (int y = 0; y < toH; ++y) {
        for (int x = 0; x < toW; ++x) {
            float acc[4] = {};
            for (const Tap& t : tapsY[y]) {
                const float* s = &rows[(static_cast<size_t>(t.index) * toW + x) * 4];
                for (int c = 0; c < 4; ++c) {
                    acc[c] += s[c] * t.weight;
                }
            }
            BYTE* d = &out[(static_cast<size_t>(y) * toW + x) * 4];
            const BYTE alpha = static_cast<BYTE>(std::clamp(std::lround(acc[3]), 0L, 255L));
            for (int c = 0; c < 3; ++c) {
                // A colour above its alpha is not a premultiplied pixel.
                d[c] = static_cast<BYTE>(std::clamp(std::lround(acc[c]), 0L, static_cast<long>(alpha)));
            }
            d[3] = alpha;
        }
    }
    return out;
}

// The same for a square image.
inline std::vector<BYTE> Resample(const std::vector<BYTE>& src, int from, int to) {
    return Resample(src, from, from, to, to);
}

// Premultiplies an image whose colours are not, which XAML would otherwise
// draw with bright fringes wherever an edge is partly transparent. The shell
// hands icons over either way; a colour above its pixel's alpha is the tell.
// A bitmap with no alpha at all is opaque when the caller says so: the shell's
// are, while DrawIconEx leaves an old mask-only icon at zero alpha throughout.
inline void Premultiply(std::vector<BYTE>* pixels, bool opaqueIfNoAlpha) {
    bool straight = false, anyAlpha = false;
    for (size_t i = 0; i < pixels->size(); i += 4) {
        const BYTE* p = &(*pixels)[i];
        anyAlpha |= p[3] != 0;
        straight |= p[0] > p[3] || p[1] > p[3] || p[2] > p[3];
    }
    for (size_t i = 0; i < pixels->size(); i += 4) {
        BYTE* p = &(*pixels)[i];
        if (!anyAlpha) {
            if (opaqueIfNoAlpha) {
                p[3] = 255;
            }
        } else if (straight) {
            for (int c = 0; c < 3; ++c) {
                p[c] = static_cast<BYTE>((p[c] * p[3] + 127) / 255);
            }
        }
    }
}

// Copies a square bitmap into a top-down 32bpp premultiplied BGRA buffer of
// size x size pixels.
inline bool BitmapToBgra(HBITMAP bitmap, int size, std::vector<BYTE>* out) {
    if (!bitmap) {
        return false;
    }
    BITMAP info{};
    if (!GetObjectW(bitmap, sizeof(info), &info)) {
        return false;
    }
    const int side = info.bmWidth;
    if (side <= 0 || side > 1024 || std::abs(info.bmHeight) != side) {
        return false;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = side;
    bi.bmiHeader.biHeight = -side;  // negative: top-down, matching XAML
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> pixels(static_cast<size_t>(side) * side * 4, 0);
    HDC screen = GetDC(nullptr);
    int scanned = GetDIBits(screen, bitmap, 0, side, pixels.data(), &bi,
                            DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    if (scanned != side) {
        return false;
    }
    Premultiply(&pixels, true);
    *out = Resample(pixels, side, size);
    return true;
}

// Copies a bitmap of any shape into a top-down 32bpp premultiplied BGRA
// buffer, at its own size: thumbnails are seldom square.
inline bool BitmapToPixels(HBITMAP bitmap, std::vector<BYTE>* out, int* width, int* height) {
    BITMAP info{};
    if (!bitmap || !GetObjectW(bitmap, sizeof(info), &info) || info.bmWidth <= 0 || info.bmWidth > 4096 ||
        info.bmHeight == 0 || std::abs(info.bmHeight) > 4096) {
        return false;
    }
    const int w = info.bmWidth, h = std::abs(info.bmHeight);
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    out->assign(static_cast<size_t>(w) * h * 4, 0);
    HDC screen = GetDC(nullptr);
    const int scanned = GetDIBits(screen, bitmap, 0, h, out->data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, screen);
    if (scanned != h) {
        return false;
    }
    Premultiply(out, true);
    *width = w;
    *height = h;
    return true;
}

// The size an icon was made at, which DrawIconEx draws it at when given no
// size of its own.
inline int IconSideOf(HICON icon) {
    ICONINFO info{};
    if (!GetIconInfo(icon, &info)) {
        return 0;
    }
    BITMAP bitmap{};
    int side = 0;
    if (info.hbmColor && GetObjectW(info.hbmColor, sizeof(bitmap), &bitmap)) {
        side = bitmap.bmWidth;
    } else if (info.hbmMask && GetObjectW(info.hbmMask, sizeof(bitmap), &bitmap)) {
        side = bitmap.bmWidth;  // a monochrome icon: the mask is twice as high
    }
    if (info.hbmColor) {
        DeleteObject(info.hbmColor);
    }
    if (info.hbmMask) {
        DeleteObject(info.hbmMask);
    }
    return side;
}

// Draws an icon at its own size into a 32bpp surface, copies it out and
// resizes it to size x size. DrawIconEx is used rather than reading the
// icon's own bitmaps because it handles both modern 32bpp icons and the old
// mask-plus-colour pairs.
inline bool IconToBgra(HICON icon, int size, std::vector<BYTE>* out) {
    if (!icon) {
        return false;
    }
    int side = IconSideOf(icon);
    if (side <= 0 || side > 1024) {
        side = size;
    }
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = side;
    bi.bmiHeader.biHeight = -side;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!dc) {
        return false;
    }
    void* bits = nullptr;
    HBITMAP dib = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!dib || !bits) {
        DeleteDC(dc);
        return false;
    }
    HGDIOBJ previous = SelectObject(dc, dib);
    memset(bits, 0, static_cast<size_t>(side) * side * 4);
    BOOL drawn = DrawIconEx(dc, 0, 0, icon, side, side, 0, nullptr, DI_NORMAL);
    if (drawn) {
        std::vector<BYTE> pixels(static_cast<BYTE*>(bits),
                                 static_cast<BYTE*>(bits) + static_cast<size_t>(side) * side * 4);
        Premultiply(&pixels, false);
        *out = Resample(pixels, side, size);
    }
    SelectObject(dc, previous);
    DeleteObject(dib);
    DeleteDC(dc);
    return drawn != FALSE;
}

// The side of a square BGRA icon buffer, or 0 if it is not one.
inline int IconSide(const std::vector<BYTE>& pixels) {
    const int side = static_cast<int>(std::lround(std::sqrt(pixels.size() / 4.0)));
    return side > 0 && static_cast<size_t>(side) * side * 4 == pixels.size() ? side : 0;
}

// Icons for files.
//
// Fetching a real icon per result is far too slow to do per keystroke -- a
// single IShellItemImageFactory::GetImage measured 38-220 ms. Almost every
// file of the same type has the same icon, though, so the shell is asked once
// per extension using SHGFI_USEFILEATTRIBUTES, which answers from the
// registered file type without touching the disk at all.
//
// The exception is a type whose files carry their own icon: programs,
// shortcuts, icon files. Those are fetched per file and kept per path. A new
// one measured 3-40 ms and about 0.6 ms once the shell has seen it, so each
// set of results gets a time budget for them, top rows first; the rest show
// their type's icon until they are fetched while idle (TakeLate).
class FileIconCache {
   public:
    explicit FileIconCache(int size) : size_(size) {}

    // A new icon size -- Start is on a display with another scale. Every icon
    // kept was made for the old one.
    void SetSize(int size) {
        if (size == size_) {
            return;
        }
        size_ = size;
        types_.clear();
        files_.clear();
        pending_.clear();
        late_.clear();
    }

    // Starts a new set of results: files still queued from the last one are
    // no longer on screen.
    void NewResults(std::chrono::milliseconds budget) {
        pending_.clear();
        late_.clear();
        deadline_ = std::chrono::steady_clock::now() + budget;
    }

    // Returns BGRA pixels, or nullptr when the shell had nothing.
    const std::vector<BYTE>* Get(const std::wstring& path, bool isFolder) {
        if (!isFolder && HasOwnIcon(ExtensionOf(path))) {
            auto it = files_.find(path);
            if (it == files_.end() && std::chrono::steady_clock::now() < deadline_) {
                it = StoreFile(path);
            }
            if (it == files_.end()) {
                pending_.push_back(path);
            } else if (!it->second.empty()) {
                return &it->second;
            }
        }
        return ForType(path, isFolder);
    }

    // Fetches one file left over from the budget. Returns whether there was one.
    bool FetchPending() {
        while (!pending_.empty()) {
            std::wstring path = std::move(pending_.front());  // top rows first
            pending_.erase(pending_.begin());
            if (!files_.count(path)) {
                auto it = StoreFile(path);
                if (!it->second.empty()) {
                    late_.emplace_back(path, it->second);
                }
                return true;
            }
        }
        return false;
    }

    bool HasPending() const { return !pending_.empty(); }

    // The icons FetchPending found since the last call, for rows already drawn.
    std::vector<std::pair<std::wstring, std::vector<BYTE>>> TakeLate() {
        return std::exchange(late_, {});
    }

   private:
    using Map = std::unordered_map<std::wstring, std::vector<BYTE>>;

    const std::vector<BYTE>* ForType(const std::wstring& nameOrPath, bool isFolder) {
        std::wstring key = isFolder ? L"<dir>" : ExtensionOf(nameOrPath);
        auto it = types_.find(key);
        if (it == types_.end()) {
            // A name that does not exist is fine and is the point: with
            // SHGFI_USEFILEATTRIBUTES the shell answers from the extension alone.
            std::wstring probe = isFolder ? L"folder" : (L"file" + key);
            it = types_.emplace(key, Load(probe.c_str(),
                                          isFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
                                          SHGFI_USEFILEATTRIBUTES)).first;
        }
        return it->second.empty() ? nullptr : &it->second;
    }

    // Empty when the icon would come from somewhere slow; the type's icon
    // stands in for it then.
    Map::iterator StoreFile(const std::wstring& path) {
        if (files_.size() >= 4096) {
            files_.clear();
        }
        return files_.emplace(path, IconIsLocal(path) ? Load(path.c_str(), 0, 0) : std::vector<BYTE>{}).first;
    }

    static bool HasOwnIcon(const std::wstring& ext) {
        static const wchar_t* const kTypes[] = {L".exe", L".lnk", L".ico", L".url",
                                                L".scr", L".cpl", L".cur", L".ani"};
        for (const wchar_t* type : kTypes) {
            if (ext == type) {
                return true;
            }
        }
        return false;
    }

    // Whether the shell would read the icon from a local fixed drive: a
    // network or removable path could stall the search thread until it times
    // out. For a shortcut that is its icon location, or else its target.
    static bool IconIsLocal(const std::wstring& path) {
        if (!OnFixedDrive(path)) {
            return false;
        }
        std::wstring ext = ExtensionOf(path);
        wchar_t where[MAX_PATH] = {};
        if (ext == L".url") {
            GetPrivateProfileStringW(L"InternetShortcut", L"IconFile", L"", where, MAX_PATH, path.c_str());
        } else if (ext == L".lnk") {
            IShellLinkW* link = nullptr;
            if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)))) {
                return false;
            }
            IPersistFile* file = nullptr;
            bool loaded = SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(&file))) &&
                          SUCCEEDED(file->Load(path.c_str(), STGM_READ));
            int index = 0;
            if (loaded && (FAILED(link->GetIconLocation(where, MAX_PATH, &index)) || !where[0]) &&
                FAILED(link->GetPath(where, MAX_PATH, nullptr, SLGP_RAWPATH))) {
                where[0] = 0;
            }
            if (file) {
                file->Release();
            }
            link->Release();
            if (!loaded) {
                return false;
            }
        }
        // Nothing named: the file's own icon, a URL's browser, or the shell
        // item a link points to.
        if (!where[0]) {
            return true;
        }
        wchar_t expanded[MAX_PATH] = {};
        return ExpandEnvironmentStringsW(where, expanded, MAX_PATH) && OnFixedDrive(expanded);
    }

    static bool OnFixedDrive(const std::wstring& path) {
        if (path.size() < 3 || path[1] != L':' || path[2] != L'\\') {
            return false;
        }
        const wchar_t root[] = {path[0], L':', L'\\', 0};
        return GetDriveTypeW(root) == DRIVE_FIXED;
    }

    std::vector<BYTE> Load(const wchar_t* probe, DWORD attributes, UINT flags) {
        std::vector<BYTE> pixels;
        SHFILEINFOW info{};

        // Through the system image lists: the smaller of SHIL_LARGE (normally
        // 32px) and SHIL_EXTRALARGE (48) that is at least the size wanted,
        // resized to it by IconToBgra; past 48, the 48 enlarged. Not
        // SHIL_JUMBO: a type with no 256px image has its 48 in a corner of
        // it. The shell already has these lists, so the right one costs no
        // more than any other.
        bool got = false;
        if (SHGetFileInfoW(probe, attributes, &info, sizeof(info),
                           flags | SHGFI_SYSICONINDEX)) {
            for (int which : {SHIL_LARGE, SHIL_EXTRALARGE}) {
                IImageList* list = nullptr;
                if (FAILED(SHGetImageList(which, IID_PPV_ARGS(&list))) || !list) {
                    continue;
                }
                int cx = 0, cy = 0;
                list->GetIconSize(&cx, &cy);
                const bool fits = cx >= size_ || which == SHIL_EXTRALARGE;
                if (fits) {
                    HICON icon = nullptr;
                    if (SUCCEEDED(list->GetIcon(info.iIcon, ILD_TRANSPARENT, &icon)) && icon) {
                        got = IconToBgra(icon, size_, &pixels);
                        DestroyIcon(icon);
                    }
                }
                list->Release();
                if (fits) {
                    break;
                }
            }
        }

        if (!got &&
            SHGetFileInfoW(probe, attributes, &info, sizeof(info),
                           flags | SHGFI_ICON | SHGFI_LARGEICON)) {
            IconToBgra(info.hIcon, size_, &pixels);
            DestroyIcon(info.hIcon);
        }
        return pixels;
    }

    static std::wstring ExtensionOf(const std::wstring& name) {
        size_t dot = name.rfind(L'.');
        if (dot == std::wstring::npos || dot + 1 >= name.size() ||
            name.find_first_of(L"\\/", dot) != std::wstring::npos) {
            return L"";
        }
        std::wstring ext = name.substr(dot);
        for (wchar_t& c : ext) {
            c = static_cast<wchar_t>(towlower(c));
        }
        return ext;
    }

    int size_;
    Map types_;
    Map files_;
    std::vector<std::wstring> pending_;
    std::vector<std::pair<std::wstring, std::vector<BYTE>>> late_;
    std::chrono::steady_clock::time_point deadline_{};
};

}  // namespace icons
// ===========================================================================
// Component: Windows Settings Database
// ===========================================================================

namespace settings {

struct SettingItem {
    const wchar_t* name;
    const wchar_t* command;
    const wchar_t* area;
    const wchar_t* altNames;
};

inline const std::vector<SettingItem>& GetSettingsList() {
    static const std::vector<SettingItem> kSettings = {
        { L"Settings (Application homepage)", L"ms-settings:", L"Settings", L"Settings app;System settings" },
        { L"Control Panel (Application homepage)", L"control.exe", L"Control Panel", L"" },
        { L"Access work or school", L"ms-settings:workplace", L"Accounts", L"Workplace" },
        { L"Email and app accounts", L"ms-settings:emailandaccounts", L"Accounts", L"" },
        { L"Family and other people", L"ms-settings:otherusers", L"Accounts", L"Other users" },
        { L"Set up a kiosk", L"ms-settings:assignedaccess", L"Accounts", L"Assigned access" },
        { L"Sign-in options", L"ms-settings:signinoptions", L"Accounts", L"sign in;sign-in;password;pin;windows hello;fingerprint;face" },
        { L"Sign-in options - Dynamic lock", L"ms-settings:signinoptions-dynamiclock", L"Accounts", L"" },
        { L"Sync your settings", L"ms-settings:sync", L"Accounts", L"" },
        { L"Windows Hello setup - Face", L"ms-settings:signinoptions-launchfaceenrollment", L"Accounts", L"" },
        { L"Windows Hello setup - Fingerprint", L"ms-settings:signinoptions-launchfingerprintenrollment", L"Accounts", L"" },
        { L"Your info", L"ms-settings:yourinfo", L"Accounts", L"" },
        { L"Apps & Features", L"ms-settings:appsfeatures", L"Apps", L"Uninstall programs;Remove programs;Change programs;Repair programs;apps;apps & features;installed apps;uninstall;remove programs;applications" },
        { L"App features", L"ms-settings:appsfeatures-app", L"Apps", L"" },
        { L"Apps for websites", L"ms-settings:appsforwebsites", L"Apps", L"" },
        { L"Default apps", L"ms-settings:defaultapps", L"Apps", L"default apps;default browser;default programs;defaults" },
        { L"Manage optional features", L"ms-settings:optionalfeatures", L"Apps", L"" },
        { L"Offline Maps", L"ms-settings:maps", L"Apps", L"" },
        { L"Offline Maps - Download maps", L"ms-settings:maps-downloadmaps", L"Apps", L"" },
        { L"Startup apps", L"ms-settings:startupapps", L"Apps", L"startup;startup apps;autostart;run on startup" },
        { L"Video playback", L"ms-settings:videoplayback", L"Apps", L"" },
        { L"Notifications", L"ms-settings:cortana-notifications", L"Cortana", L"" },
        { L"More details", L"ms-settings:cortana-moredetails", L"Cortana", L"" },
        { L"Permissions and history", L"ms-settings:cortana-permissions", L"Cortana", L"" },
        { L"Windows search", L"ms-settings:cortana-windowssearch", L"Cortana", L"Windows search settings" },
        { L"Cortana - Language", L"ms-settings:cortana-language", L"Cortana", L"Talk" },
        { L"Cortana", L"ms-settings:cortana", L"Cortana", L"" },
        { L"Talk to Cortana", L"ms-settings:cortana-talktocortana", L"Cortana", L"" },
        { L"AutoPlay", L"ms-settings:autoplay", L"Devices", L"" },
        { L"Bluetooth and other devices", L"ms-settings:bluetooth", L"Devices", L"bluetooth;bt;wireless;pair" },
        { L"Devices", L"ms-settings:bluetooth", L"Bluetooth & devices", L"Manage devices;Add devices;Bluetooth devices;bluetooth;bt;wireless;pair" },
        { L"Bluetooth & devices", L"ms-settings:devices", L"Settings", L"Devices;Bluetooth devices;Printers and scanners;Phone Link;Camera;Mouse;Touchpad;Pen and Windows Ink;AutoPlay;USB" },
        { L"Connected Devices", L"ms-settings:connecteddevices", L"Devices", L"" },
        { L"Default camera", L"ms-settings:camera", L"Devices", L"camera;webcam" },
        { L"Mouse and touchpad", L"ms-settings:mousetouchpad", L"Devices", L"mouse;touchpad;cursor;pointer;scroll;touch" },
        { L"Pen and Windows Ink", L"ms-settings:pen", L"Devices", L"" },
        { L"Printers and scanners", L"ms-settings:printers", L"Devices", L"printers;printer;scanners;scanner;print;fax" },
        { L"Touchpad", L"ms-settings:devices-touchpad", L"Devices", L"" },
        { L"Typing", L"ms-settings:typing", L"Devices", L"typing;keyboard;autocorrect;touch keyboard" },
        { L"USB", L"ms-settings:usb", L"Devices", L"" },
        { L"Wheel", L"ms-settings:wheel", L"Devices", L"" },
        { L"Phone", L"ms-settings:mobile-devices", L"Phone", L"Mobile devices" },
        { L"Audio", L"ms-settings:easeofaccess-audio", L"Ease of access", L"Mono;Volume;Audio alerts" },
        { L"Closed captions", L"ms-settings:easeofaccess-closedcaptioning", L"Ease of access", L"" },
        { L"Color filters", L"ms-settings:easeofaccess-colorfilter", L"Ease of access", L"Inverted colors;Grayscale;Red-green;Blue-yellow;Green week;Red week;deuteranopia;protanopia;tritanopia" },
        { L"Mouse pointer", L"ms-settings:easeofaccess-mousepointer", L"Ease of access", L"Touch feedback" },
        { L"Display", L"ms-settings:easeofaccess-display", L"Ease of access", L"Transparency;Animations;Scroll bars;Size" },
        { L"Eye control", L"ms-settings:easeofaccess-eyecontrol", L"Ease of access", L"" },
        { L"Fonts", L"ms-settings:fonts", L"Ease of access", L"" },
        { L"High contrast", L"ms-settings:easeofaccess-highcontrast", L"Ease of access", L"" },
        { L"Keyboard", L"ms-settings:easeofaccess-keyboard", L"Ease of access", L"Print screen;Shortcuts;On-Screen;Keys;Scroll Lock;Caps Lock;Num Lock" },
        { L"Magnifier", L"ms-settings:easeofaccess-magnifier", L"Ease of access", L"Zoom" },
        { L"Mouse", L"ms-settings:easeofaccess-mouse", L"Ease of access", L"Keypad;Touch" },
        { L"Narrator", L"ms-settings:easeofaccess-narrator", L"Ease of access", L"" },
        { L"Other options", L"ms-settings:easeofaccess-otheroptions", L"Ease of access", L"" },
        { L"Speech", L"ms-settings:easeofaccess-speechrecognition", L"Ease of access", L"Recognition;Talk" },
        { L"Extras", L"ms-settings:extras", L"Extras", L"" },
        { L"Broadcasting", L"ms-settings:gaming-broadcasting", L"Gaming", L"" },
        { L"Game bar", L"ms-settings:gaming-gamebar", L"Gaming", L"" },
        { L"Game DVR", L"ms-settings:gaming-gamedvr", L"Gaming", L"" },
        { L"Game Mode", L"ms-settings:gaming-gamemode", L"Gaming", L"" },
        { L"Playing a game full screen", L"ms-settings:quietmomentsgame", L"Gaming", L"Quiet moments game" },
        { L"TruePlay", L"ms-settings:gaming-trueplay", L"Gaming", L"" },
        { L"Xbox Networking", L"ms-settings:gaming-xboxnetworking", L"Gaming", L"" },
        { L"Audio and speech", L"ms-settings:holographic-audio", L"Mixed reality", L"Holographic audio" },
        { L"Environment", L"ms-settings:privacy-holographic-environment", L"Mixed reality", L"Holographic Environment" },
        { L"Headset display", L"ms-settings:holographic-headset", L"Mixed reality", L"Holographic Headset" },
        { L"Uninstall", L"ms-settings:holographic-management", L"Mixed reality", L"Holographic Management" },
        { L"Airplane mode", L"ms-settings:network-airplanemode", L"Network and Internet", L"" },
        { L"Proximity", L"ms-settings:proximity", L"Network and Internet", L"" },
        { L"Cellular and SIM", L"ms-settings:network-cellular", L"Network and Internet", L"" },
        { L"Data usage", L"ms-settings:datausage", L"Network and Internet", L"" },
        { L"Dial-up", L"ms-settings:network-dialup", L"Network and Internet", L"" },
        { L"Direct access", L"ms-settings:network-directaccess", L"Network and Internet", L"" },
        { L"Ethernet", L"ms-settings:network-ethernet", L"Network and Internet", L"DNS;SDNS;SecureDNS;Gateway;DHCP;IP" },
        { L"Manage known networks", L"ms-settings:network-wifisettings", L"Network and Internet", L"Wi-Fi settings;wifi" },
        { L"Mobile hotspot", L"ms-settings:network-mobilehotspot", L"Network and Internet", L"hotspot;mobile hotspot;share internet" },
        { L"NFC", L"ms-settings:nfctransactions", L"Network and Internet", L"NFC Transactions" },
        { L"Proxy", L"ms-settings:network-proxy", L"Network and Internet", L"proxy;manual proxy;pac" },
        { L"Network status", L"ms-settings:network-status", L"Network and Internet", L"" },
        { L"Network", L"ms-settings:network", L"Network and Internet", L"DNS;SDNS;SecureDNS;Gateway;DHCP;IP;network;internet;ethernet;connection" },
        { L"VPN", L"ms-settings:network-vpn", L"Network and Internet", L"vpn;virtual private network" },
        { L"Wi-Fi", L"ms-settings:network-wifi", L"Network and Internet", L"Wireless;Metered connection;wifi;wi-fi;wlan;wireless;internet" },
        { L"Wi-Fi Calling", L"ms-settings:network-wificalling", L"Network and Internet", L"wifi" },
        { L"Background", L"ms-settings:personalization-background", L"Personalization", L"Wallpaper;Picture;Image;wallpaper;background;desktop image;picture" },
        { L"Choose which folders appear on Start", L"ms-settings:personalization-start-places", L"Personalization", L"Start places" },
        { L"Colors", L"ms-settings:colors", L"Personalization", L"Dark mode;Light mode;Dark color;Light color;App color;Taskbar color;Window border" },
        { L"Glance", L"ms-settings:personalization-glance", L"Personalization", L"" },
        { L"Lock screen", L"ms-settings:lockscreen", L"Personalization", L"Image;Picture;Screen saver;lock screen;lockscreen" },
        { L"Navigation bar", L"ms-settings:personalization-navbar", L"Personalization", L"" },
        { L"Personalization (category)", L"ms-settings:personalization", L"Settings", L"personalization;personalize;wallpaper;theme;colors" },
        { L"Start", L"ms-settings:personalization-start", L"Personalization", L"" },
        { L"Taskbar", L"ms-settings:taskbar", L"Personalization", L"taskbar;task bar;taskbar behaviors;unhide taskbar" },
        { L"Themes", L"ms-settings:themes", L"Personalization", L"themes;theme;desktop theme" },
        { L"Add your phone", L"ms-settings:mobile-devices-addphone", L"Phone", L"" },
        { L"Direct open your phone", L"ms-settings:mobile-devices-addphone-direct", L"Phone", L"" },
        { L"Accessory apps", L"ms-settings:privacy-accessoryapps", L"Privacy", L"" },
        { L"Account info", L"ms-settings:privacy-accountinfo", L"Privacy", L"" },
        { L"Activity history", L"ms-settings:privacy-activityhistory", L"Privacy", L"" },
        { L"Advertising ID", L"ms-settings:privacy-advertisingid", L"Privacy", L"" },
        { L"App diagnostics", L"ms-settings:privacy-appdiagnostics", L"Privacy", L"" },
        { L"Automatic file downloads", L"ms-settings:privacy-automaticfiledownloads", L"Privacy", L"" },
        { L"Background Apps", L"ms-settings:privacy-backgroundapps", L"Privacy", L"" },
        { L"Calendar", L"ms-settings:privacy-calendar", L"Privacy", L"" },
        { L"Call history", L"ms-settings:privacy-callhistory", L"Privacy", L"" },
        { L"Camera", L"ms-settings:privacy-webcam", L"Privacy", L"" },
        { L"Contacts", L"ms-settings:privacy-contacts", L"Privacy", L"" },
        { L"Documents", L"ms-settings:privacy-documents", L"Privacy", L"" },
        { L"Email", L"ms-settings:privacy-email", L"Privacy", L"" },
        { L"Eye tracker", L"ms-settings:privacy-eyetracker", L"Privacy", L"" },
        { L"Feedback and diagnostics", L"ms-settings:privacy-feedback", L"Privacy", L"" },
        { L"File system", L"ms-settings:privacy-broadfilesystemaccess", L"Privacy", L"" },
        { L"General", L"ms-settings:privacy-general", L"Privacy", L"" },
        { L"Inking and typing", L"ms-settings:privacy-speechtyping", L"Privacy", L"Speech typing" },
        { L"Location", L"ms-settings:privacy-location", L"Privacy", L"" },
        { L"Messaging", L"ms-settings:privacy-messaging", L"Privacy", L"" },
        { L"Microphone", L"ms-settings:privacy-microphone", L"Privacy", L"microphone;mic;record audio" },
        { L"Motion", L"ms-settings:privacy-motion", L"Privacy", L"" },
        { L"Notifications", L"ms-settings:privacy-notifications", L"Privacy", L"" },
        { L"Other devices", L"ms-settings:privacy-customdevices", L"Privacy", L"Custom devices" },
        { L"Phone calls", L"ms-settings:privacy-phonecalls", L"Privacy", L"" },
        { L"Pictures", L"ms-settings:privacy-pictures", L"Privacy", L"" },
        { L"Radios", L"ms-settings:privacy-radios", L"Privacy", L"" },
        { L"Speech", L"ms-settings:privacy-speech", L"Privacy", L"" },
        { L"Tasks", L"ms-settings:privacy-tasks", L"Privacy", L"" },
        { L"Videos", L"ms-settings:privacy-videos", L"Privacy", L"" },
        { L"Voice activation", L"ms-settings:privacy-voiceactivation", L"Privacy", L"" },
        { L"Accounts", L"ms-settings:surfacehub-accounts", L"SurfaceHub", L"" },
        { L"Session cleanup", L"ms-settings:surfacehub-sessioncleanup", L"SurfaceHub", L"" },
        { L"Team Conferencing", L"ms-settings:surfacehub-calling", L"SurfaceHub", L"calling" },
        { L"Team device management", L"ms-settings:surfacehub-devicemanagenent", L"SurfaceHub", L"" },
        { L"Welcome screen", L"ms-settings:surfacehub-welcome", L"SurfaceHub", L"" },
        { L"About", L"ms-settings:about", L"System", L"RAM;Processor;OS;ID;Edition;Version;about;system info;specs;pc specs;ram;processor;device name" },
        { L"Advanced display settings", L"ms-settings:display-advanced", L"System", L"refresh rate;hz;144hz;165hz;240hz;60hz;display refresh rate;display information;bit depth;color format;dynamic refresh rate" },
        { L"App volume and device preferences", L"ms-settings:apps-volume", L"System", L"volume mixer;app volume;individual volume;application volume;audio output per app;spatial audio;device preferences" },
        { L"Battery Saver", L"ms-settings:batterysaver", L"System", L"battery;battery saver;power saving" },
        { L"Battery Saver settings", L"ms-settings:batterysaver-settings", L"System", L"" },
        { L"Battery use", L"ms-settings:batterysaver-usagedetails", L"System", L"Battery saver usage details" },
        { L"Clipboard", L"ms-settings:clipboard", L"System", L"clipboard;clipboard history;win+v;sync clipboard;clear clipboard data" },
        { L"Display", L"ms-settings:display", L"System", L"Night light;Blue light;Warmer color;Red eye;display;screen;resolution;monitor;monitors;scale;hdr;refresh rate" },
        { L"Default Save Locations", L"ms-settings:savelocations", L"System", L"" },
        { L"Screen rotation", L"ms-settings:screenrotation", L"System", L"" },
        { L"Duplicating my display", L"ms-settings:quietmomentspresentation", L"System", L"Presentation" },
        { L"During these hours", L"ms-settings:quietmomentsscheduled", L"System", L"Scheduled" },
        { L"Encryption", L"ms-settings:deviceencryption", L"System", L"" },
        { L"Focus assist - Quiet hours", L"ms-settings:quiethours", L"System", L"" },
        { L"Focus assist - Quiet moments", L"ms-settings:quietmomentshome", L"System", L"" },
        { L"Graphics settings", L"ms-settings:display-advancedgraphics", L"System", L"graphics;graphics settings;hags;hardware-accelerated gpu scheduling;gpu preference;variable refresh rate;vrr;optimizations for windowed games;auto hdr;high performance gpu" },
        { L"Messaging", L"ms-settings:messaging", L"System", L"" },
        { L"Multitasking", L"ms-settings:multitasking", L"System", L"multitasking;snap windows;snap layouts;snap assist;title bar window shake;virtual desktops;alt tab;timeline" },
        { L"Night light settings", L"ms-settings:nightlight", L"System", L"night light;blue light filter;warm colors;color temperature;schedule night light" },
        { L"Phone - Default apps", L"ms-settings:phone-defaultapps", L"System", L"" },
        { L"Projecting to this PC", L"ms-settings:project", L"System", L"" },
        { L"Shared experience settings", L"ms-settings:crossdevice", L"System", L"Crossdevice;Nearby sharing settings;Share across devices" },
        { L"Nearby sharing settings", L"ms-settings:crossdevice", L"System", L"Crossdevice;Share across devices;Shared experience settings" },
        { L"Share across devices", L"ms-settings:crossdevice", L"System", L"Crossdevice;Nearby sharing settings;Shared experience settings" },
        { L"Tablet mode", L"ms-settings:tabletmode", L"System", L"" },
        { L"Notifications and actions", L"ms-settings:notifications", L"System", L"notifications;alerts;do not disturb;focus;focus assist;dnd;priority notifications" },
        { L"Remote Desktop", L"ms-settings:remotedesktop", L"System", L"remote desktop;enable rdp;allow remote desktop;remote desktop port" },
        { L"Phone", L"ms-settings:phone", L"System", L"" },
        { L"Power and sleep", L"ms-settings:powersleep", L"System", L"power;sleep;battery;power & sleep;energy;screen off;standby;power mode;power plan" },
        { L"Sound", L"ms-settings:sound", L"System", L"sound;audio;volume;speaker;speakers;headphones;mic;microphone;sound settings" },
        { L"Storage Sense", L"ms-settings:storagesense", L"System", L"storage;disk space;free up space;clean drive;storage sense;cleanup recommendations;temporary files" },
        { L"Storage policies", L"ms-settings:storagepolicies", L"System", L"" },
        { L"Date and time", L"ms-settings:dateandtime", L"Time and language", L"date;time;clock;timezone;time zone;set time" },
        { L"Japan IME settings", L"ms-settings:regionlanguage-jpnime", L"Time and language", L"jpnime" },
        { L"Region", L"ms-settings:regionformatting", L"Time and language", L"Region formatting" },
        { L"Keyboard", L"ms-settings:keyboard", L"Time and language", L"" },
        { L"Regional language", L"ms-settings:regionlanguage", L"Time and language", L"language;region;locale;keyboard language;input language" },
        { L"Bopomofo IME", L"ms-settings:regionlanguage-bpmfime", L"Time and language", L"bpmf" },
        { L"Cangjie IME", L"ms-settings:regionlanguage-cangjieime", L"Time and language", L"" },
        { L"Pinyin IME settings - domain lexicon", L"ms-settings:regionlanguage-chsime-pinyin-domainlexicon", L"Time and language", L"" },
        { L"Pinyin IME settings - Key configuration", L"ms-settings:regionlanguage-chsime-pinyin-keyconfig", L"Time and language", L"" },
        { L"Pinyin IME settings - UDP", L"ms-settings:regionlanguage-chsime-pinyin-udp", L"Time and language", L"" },
        { L"Wubi IME settings - UDP", L"ms-settings:regionlanguage-chsime-wubi-udp", L"Time and language", L"" },
        { L"Quickime", L"ms-settings:regionlanguage-quickime", L"Time and language", L"" },
        { L"Pinyin IME settings", L"ms-settings:regionlanguage-chsime-pinyin", L"Time and language", L"" },
        { L"Speech", L"ms-settings:speech", L"Time and language", L"" },
        { L"Wubi IME settings", L"ms-settings:regionlanguage-chsime-wubi", L"Time and language", L"" },
        { L"Activation", L"ms-settings:activation", L"Update and security", L"activation;product key;change product key;windows license;digital license;upgrade windows" },
        { L"Backup", L"ms-settings:backup", L"Update and security", L"" },
        { L"Delivery Optimization", L"ms-settings:delivery-optimization", L"Update and security", L"delivery optimization;allow downloads from other pcs;bandwidth limits;peer to peer update" },
        { L"Find My Device", L"ms-settings:findmydevice", L"Update and security", L"" },
        { L"For developers", L"ms-settings:developers", L"Update and security", L"developer mode;end task;taskbar end task;sideload apps;remote tools;developer options" },
        { L"Recovery", L"ms-settings:recovery", L"Update and security", L"recovery;reset this pc;advanced startup;uefi firmware settings;boot to bios;go back;reinstall windows;troubleshoot" },
        { L"Troubleshoot", L"ms-settings:troubleshoot", L"Update and security", L"" },
        { L"Windows Security", L"ms-settings:windowsdefender", L"Update and security", L"Windows Defender;Firewall;Virus;Core Isolation;Security Processor;Isolated Browsing;Exploit Protection;windows security;antivirus;defender;virus;threat" },
        { L"Windows Insider Program", L"ms-settings:windowsinsider", L"Update and security", L"" },
        { L"Windows Update", L"ms-settings:windowsupdate", L"Update and security", L"windows update;update;updates;patch;check for updates" },
        { L"Windows Update - Check for updates", L"ms-settings:windowsupdate-action", L"Update and security", L"" },
        { L"Windows Update - Check for updates", L"ms-settings:windowsupdate", L"Update and security", L"windows update;update;updates;patch;check for updates" },
        { L"Windows Update - Advanced options", L"ms-settings:windowsupdate-options", L"Update and security", L"windows update options;optional updates;delivery optimization;active hours;metered network update;pause updates" },
        { L"Windows Update - Restart options", L"ms-settings:windowsupdate-restartoptions", L"Update and security", L"" },
        { L"Windows Update - View update history", L"ms-settings:windowsupdate-history", L"Update and security", L"update history;view installed updates;uninstall updates;quality updates;driver updates" },
        { L"Windows Update - View optional updates", L"ms-settings:windowsupdate-optionalupdates", L"Update and security", L"" },
        { L"Workplace provisioning", L"ms-settings:workplace-provisioning", L"User accounts", L"" },
        { L"Provisioning", L"ms-settings:provisioning", L"User accounts", L"" },
        { L"Windows Anywhere", L"ms-settings:windowsanywhere", L"User accounts", L"" },
        { L"Accessibility Options", L"control access.cpl", L"Ease of access", L"access.cpl" },
        { L"Action Center", L"control /name Microsoft.ActionCenter", L"System and Security", L"wscui.cpl" },
        { L"Add Hardware", L"control /name Microsoft.AddHardware", L"Hardware and Sound", L"" },
        { L"Add or remove programs", L"control appwiz.cpl", L"Hardware and Sound", L"appwiz.cpl;Uninstall programs;Change programs;Repair programs" },
        { L"Administrative Tools", L"control /name Microsoft.AdministrativeTools", L"System and Security", L"" },
        { L"AutoPlay", L"control /name Microsoft.AutoPlay", L"Programs", L"" },
        { L"Backup and Restore", L"control /name Microsoft.BackupAndRestore", L"System and Security", L"" },
        { L"Biometric Devices", L"control /name Microsoft.BiometricDevices", L"Hardware and Sound", L"" },
        { L"BitLocker Drive Encryption", L"control /name Microsoft.BitLockerDriveEncryption", L"System and Security", L"" },
        { L"Bluetooth devices", L"control /bthprops.cpl", L"Hardware and Sound", L"" },
        { L"Color management", L"control /name Microsoft.ColorManagement", L"Appearance and Personalization", L"colorcpl.exe;color management;icc profile;icm;monitor color;color profiles;display profile" },
        { L"Credential manager", L"control /name Microsoft.CredentialManager", L"User accounts", L"Password" },
        { L"Client service for NetWare", L"control nwc.cpl", L"Programs", L"nwc.cpl" },
        { L"Date and time", L"control /name Microsoft.DateAndTime", L"Clock and Region", L"timedate.cpl;date and time;internet time;clock" },
        { L"Default location", L"control /name Microsoft.DefaultLocation", L"Clock and Region", L"" },
        { L"Default programs", L"control /name Microsoft.DefaultPrograms", L"Programs", L"" },
        { L"Device manager", L"control /name Microsoft.DeviceManager", L"Hardware and Sound", L"hdwwiz.cpl;devmgmt.msc;device manager;devices;hardware" },
        { L"Devices and printers", L"control /name Microsoft.DevicesAndPrinters", L"Hardware and Sound", L"printers;devices and printers;control printers" },
        { L"Devices and printers", L"explorer.exe shell:::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", L"Hardware and Sound", L"printers;devices and printers" },
        { L"Ease of access center", L"control /name Microsoft.EaseOfAccessCenter", L"Ease of access", L"" },
        { L"Folder options", L"control /name Microsoft.FolderOptions", L"Appearance and Personalization", L"folder options;file explorer options;show hidden files" },
        { L"Fonts", L"control /name Microsoft.Fonts", L"Appearance and Personalization", L"" },
        { L"Game controllers", L"control /name Microsoft.GameControllers", L"Hardware and Sound", L"joy.cpl;game controllers;gamepad;joystick;calibrate controller" },
        { L"Get programs", L"control /name Microsoft.GetPrograms", L"Programs", L"" },
        { L"Getting started", L"control /name Microsoft.GettingStarted", L"Control Panel", L"" },
        { L"Home group", L"control /name Microsoft.HomeGroup", L"Network and Internet", L"" },
        { L"Indexing options", L"control /name Microsoft.IndexingOptions", L"System and Security", L"indexing options;search index;rebuild index" },
        { L"Infrared", L"control /name Microsoft.Infrared", L"Hardware and Sound", L"irprops.cpl" },
        { L"Internet options", L"control /name Microsoft.InternetOptions", L"Network and Internet", L"inetcpl.cpl;internet properties;proxy settings;trusted sites;certificates;tls;ssl;connections;lan settings" },
        { L"Mail - Microsoft Exchange or Windows Messaging", L"control mlcfg32.cpl", L"Network and Internet", L"mlcfg32.cpl" },
        { L"Mouse", L"control /name Microsoft.Mouse", L"Hardware and Sound", L"mouse;mouse properties;pointer speed;cursor" },
        { L"Network and sharing center", L"control /name Microsoft.NetworkAndSharingCenter", L"Network and Internet", L"network and sharing;adapter settings;network center" },
        { L"Network Connections (ncpa.cpl)", L"ncpa.cpl", L"Network and Internet", L"ncpa.cpl;network connections;change adapter settings;network adapters;network adapter properties;ethernet properties;wifi properties;ip configuration;dns properties;lan settings;network cards;network control panel" },
        { L"Network Setup Wizard", L"control netsetup.cpl", L"Network and Internet", L"netsetup.cpl" },
        { L"ODBC Data Source Administrator (32-bit)", L"%windir%/syswow64/odbcad32.exe", L"System and Security", L"odbccp32.cpl" },
        { L"ODBC Data Source Administrator (64-bit)", L"%windir%/system32/odbcad32.exe", L"System and Security", L"" },
        { L"Offline files", L"control /name Microsoft.OfflineFiles", L"Network and Internet", L"" },
        { L"Parental controls", L"control /name Microsoft.ParentalControls", L"User accounts", L"" },
        { L"Pen and input devices", L"control /name Microsoft.PenAndInputDevices", L"Hardware and Sound", L"" },
        { L"Pen and touch", L"control /name Microsoft.PenAndTouch", L"Hardware and Sound", L"" },
        { L"People Near Me", L"control /name Microsoft.PeopleNearMe", L"User accounts", L"" },
        { L"Performance information and tools", L"control /name Microsoft.PerformanceInformationAndTools", L"System and Security", L"" },
        { L"Phone and modem - Options", L"control /name Microsoft.PhoneAndModemOptions", L"Network and Internet", L"modem.cpl;telephon.cpl" },
        { L"Phone and modem", L"control /name Microsoft.PhoneAndModem", L"Network and Internet", L"modem.cpl;telephon.cpl" },
        { L"Power options", L"control /name Microsoft.PowerOptions", L"System and Security", L"powercfg.cpl" },
        { L"Printers", L"control /name Microsoft.Printers", L"Hardware and Sound", L"" },
        { L"Problem reports and solutions", L"control /name Microsoft.ProblemReportsAndSolutions", L"System and Security", L"" },
        { L"Programs and features", L"control /name Microsoft.ProgramsAndFeatures", L"Programs", L"" },
        { L"Recovery", L"control /name Microsoft.Recovery", L"System and Security", L"" },
        { L"Region and language", L"control /name Microsoft.RegionAndLanguage", L"Clock and Region", L"" },
        { L"RemoteApp and desktop connections", L"control /name Microsoft.RemoteAppAndDesktopConnections", L"Network and Internet", L"" },
        { L"Scanners and cameras", L"control /name Microsoft.ScannersAndCameras", L"Hardware and Sound", L"sticpl.cpl" },
        { L"Scheduled tasks", L"control schedtasks", L"System and Security", L"schedtasks" },
        { L"Security Center", L"control /name Microsoft.SecurityCenter", L"System and Security", L"" },
        { L"Sound", L"control /name Microsoft.Sound", L"Hardware and Sound", L"" },
        { L"Speech recognition", L"control /name Microsoft.SpeechRecognition", L"Ease of access", L"" },
        { L"Sync center", L"control /name Microsoft.SyncCenter", L"System and Security", L"" },
        { L"System", L"control sysdm.cpl", L"System and Security", L"sysdm.cpl" },
        { L"Tablet PC settings", L"control /name Microsoft.TabletPCSettings", L"System and Security", L"TabletPC.cpl" },
        { L"Text to speech", L"control /name Microsoft.TextToSpeech", L"Ease of access", L"" },
        { L"User accounts", L"control /name Microsoft.UserAccounts", L"User accounts", L"" },
        { L"Welcome center", L"control /name Microsoft.WelcomeCenter", L"System and Security", L"" },
        { L"Windows Anytime Upgrade", L"control /name Microsoft.WindowsAnytimeUpgrade", L"System and Security", L"" },
        { L"Windows CardSpace", L"control /name Microsoft.CardSpace", L"Hardware and Sound", L"" },
        { L"Windows Defender", L"control /name Microsoft.WindowsDefender", L"System and Security", L"" },
        { L"Windows Firewall", L"control /name Microsoft.WindowsFirewall", L"System and Security", L"" },
        { L"Windows Mobility Center", L"control /name Microsoft.MobilityCenter", L"Network and Internet", L"" },
        { L"Display properties", L"control Desk.cpl", L"Hardware and Sound", L"desk.cpl" },
        { L"FindFast", L"control FindFast.cpl", L"System and Security", L"findfast.cpl" },
        { L"Regional settings properties", L"control Intl.cpl", L"Ease of access", L"intl.cpl" },
        { L"Joystick properties", L"control Joy.cpl", L"Hardware and Sound", L"joy.cpl" },
        { L"Mouse, Fonts, Keyboard, and Printers properties", L"control Main.cpl", L"Hardware and Sound", L"main.cpl" },
        { L"Multimedia properties", L"control Mmsys.cpl", L"Hardware and Sound", L"mmsys.cpl" },
        { L"Network properties", L"control Netcpl.cpl", L"Network and Internet", L"netcpl.cpl" },
        { L"Password properties", L"control Password.cpl", L"User accounts", L"password.cpl" },
        { L"System properties and Add New Hardware wizard", L"control Sysdm.cpl", L"Hardware and Sound", L"sysdm.cpl" },
        { L"Desktop themes", L"control Themes.cpl", L"Appearance and Personalization", L"themes.cpl" },
        { L"Microsoft Mail Post Office", L"control Wgpocpl.cpl", L"Programs", L"wgpocpl.cpl" },
        { L"Change User Account Control settings", L"UserAccountControlSettings.exe", L"System and Security", L"UserAccountControlSettings.exe;User accounts;UAC;uac;user account control;slider;admin prompt" },
        { L"Edit the system environment variables", L"SystemPropertiesAdvanced.exe", L"System and Security", L"Edit environment variables;System variables;Env vars;System env vars;sysdm.cpl;environment variables;env;path;system variables;advanced system settings" },
        { L"Edit environment variables for your account", L"rundll32.exe sysdm.cpl,EditEnvironmentVariables", L"System and Security", L"User environment variables;User variables;Env vars;User env vars;sysdm.cpl" },
        { L"Change screen saver", L"control desk.cpl,,@screensaver", L"Control Panel", L"Turn screen saver on or off;Timeout;desk.cpl" },
        { L"Connect to a wireless display", L"ms-settings-connectabledevices:devicediscovery", L"System", L"Connect panel;Connectable devices;Connect to a wireless audio device;Device discovery" },
        { L"Microsoft Management Console", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"Authorization Manager", L"azman.msc", L"Administrative Tools", L"mmc.exe;azman.msc" },
        { L"Certificates - Current User", L"certmgr.msc", L"Administrative Tools", L"mmc.exe;certmgr.msc" },
        { L"Certificates - Local Computer", L"certlm.msc", L"Administrative Tools", L"mmc.exe;certlm.msc" },
        { L"Component Services", L"comexp.msc", L"Administrative Tools", L"mmc.exe;comexp.msc;COM-Objects" },
        { L"Computer Management", L"compmgmt.msc", L"Administrative Tools", L"mmc.exe;compmgmt.msc;System Tools;Task Scheduler;Event Viewer;Shared Folders;Network sessions;SMB;Local Users and Groups;Performance Monitor;Device manager;PNP Device;Storage;Disk Management;Create and format hard disk partitions;GPT;MBR;Services;WMI Control;Windows Management Instrumentation" },
        { L"Device manager", L"devmgmt.msc", L"Administrative Tools", L"mmc.exe;devmgmt.msc;PNP Device" },
        { L"Disk Management", L"diskmgmt.msc", L"Administrative Tools", L"mmc.exe;diskmgmt.msc;Storage;Create and format hard disk partitions;GPT;MBR" },
        { L"Event Viewer", L"eventvwr.msc", L"Administrative Tools", L"mmc.exe;eventvwr.msc" },
        { L"Local Computer Policy", L"gpedit.msc", L"Administrative Tools", L"mmc.exe;gpedit.msc;Group Policy" },
        { L"IP Security Monitor", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"IP Security Policies on Local Computer", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"Local Users and Groups", L"lusrmgr.msc", L"Administrative Tools", L"mmc.exe;lusrmgr.msc" },
        { L"Performance Monitor", L"perfmon.msc", L"Administrative Tools", L"mmc.exe;perfmon.msc" },
        { L"Print Management", L"printmanagement.msc", L"Administrative Tools", L"mmc.exe;printmanagement.msc;Printer Spooler" },
        { L"Resultant Set of Policy", L"rsop.msc", L"Administrative Tools", L"mmc.exe;rsop.msc" },
        { L"Security Configuration and Analysis", L"secpol.msc", L"Administrative Tools", L"mmc.exe;secpol.msc" },
        { L"Security Templates", L"mmc.exe", L"Administrative Tools", L"mmc.exe" },
        { L"Services", L"services.msc", L"Administrative Tools", L"mmc.exe;services.msc" },
        { L"Shared Folders", L"fsmgmt.msc", L"Administrative Tools", L"mmc.exe;fsmgmt.msc;Network sessions" },
        { L"Task Scheduler", L"taskschd.msc", L"Administrative Tools", L"mmc.exe;taskschd.msc" },
        { L"TPM Management", L"tpm.msc", L"Administrative Tools", L"mmc.exe;tpm.msc" },
        { L"Windows Defender Firewall with Advanced Security", L"wf.msc", L"Administrative Tools", L"mmc.exe;WF.msc" },
        { L"WMI Control", L"wmimgmt.msc", L"Administrative Tools", L"mmc.exe;WmiMgmt.msc;Windows Management Instrumentation" },

        // Deep Administrative Tools & Control Applets
        { L"Turn Windows features on or off", L"OptionalFeatures.exe", L"System and Security", L"OptionalFeatures.exe;windows features;optional features;hyper-v;wsl;windows subsystem for linux;iis;sandbox;windows sandbox;telnet;smb1;.net framework;tftp;virtual machine platform;features" },
        { L"System Configuration", L"msconfig.exe", L"Administrative Tools", L"msconfig.exe;msconfig;system configuration;boot options;safe mode;clean boot;services configuration;diagnostic startup;bootloader" },
        { L"Disk Cleanup", L"cleanmgr.exe", L"Administrative Tools", L"cleanmgr.exe;cleanmgr;disk cleanup;free up disk space;clean c drive;delete temp files;recycle bin cleanup;windows update cleanup" },
        { L"DirectX Diagnostic Tool", L"dxdiag.exe", L"Administrative Tools", L"dxdiag.exe;dxdiag;directx;directx diagnostic;gpu info;vram;system specifications;sound diagnostics;display diagnostics;graphics card info" },
        { L"User Accounts (Advanced)", L"netplwiz.exe", L"Administrative Tools", L"netplwiz.exe;netplwiz;control userpasswords2;userpasswords2;auto login;autologin;manage user accounts;local accounts;passwords;require sign in" },
        { L"Resource Monitor", L"resmon.exe", L"Administrative Tools", L"resmon.exe;resmon;resource monitor;cpu monitor;memory monitor;disk activity;network activity;handles;modules;processes" },
        { L"Registry Editor", L"regedit.exe", L"Administrative Tools", L"regedit.exe;regedit;registry;reg;hkey_local_machine;hkey_current_user;regedit32" },
        { L"Display Color Calibration", L"dccw.exe", L"Appearance and Personalization", L"dccw.exe;dccw;color calibration;calibrate display;gamma;color balance;brightness;contrast;calibrate monitor" },
        { L"Color Management", L"colorcpl.exe", L"Appearance and Personalization", L"colorcpl.exe;color management;icc profile;icm;monitor color;color profiles;display profile" },
        { L"Windows Memory Diagnostic", L"mdsched.exe", L"Administrative Tools", L"mdsched.exe;mdsched;memory test;ram test;check ram;memory diagnostic;ram errors" },
        { L"Sound Control Panel (Legacy)", L"mmsys.cpl", L"Hardware and Sound", L"mmsys.cpl;sound control panel;playback devices;recording devices;stereo mix;audio settings legacy;communications;sounds;exclusive mode;bitrate;sample rate" },
        { L"Internet Properties", L"inetcpl.cpl", L"Network and Internet", L"inetcpl.cpl;internet options;proxy settings;trusted sites;certificates;tls;ssl;connections;lan settings" },
        { L"Game Controllers", L"joy.cpl", L"Hardware and Sound", L"joy.cpl;game controllers;gamepad;joystick;calibrate controller;controller test;usb gamepad" },
        { L"Date and Time (Legacy)", L"timedate.cpl", L"Clock and Region", L"timedate.cpl;date and time;internet time;ntp;sync time;additional clocks;time zone" },
        { L"Region (Legacy)", L"intl.cpl", L"Clock and Region", L"intl.cpl;region;formats;short date;long date;administrative region;system locale;non-unicode" },
        { L"Power Options (Legacy)", L"powercfg.cpl", L"System and Security", L"powercfg.cpl;power options;power plan;high performance;ultimate performance;balanced;sleep timer;turn off display;choose what the power button does;fast startup" },
        { L"Programs and Features (Legacy)", L"appwiz.cpl", L"Programs", L"appwiz.cpl;uninstall a program;programs and features;classic uninstall;installed updates;view installed updates" },
        { L"System Properties (Legacy)", L"sysdm.cpl", L"System and Security", L"sysdm.cpl;system properties;computer name;hardware;device manager;advanced system settings;system protection;system restore;pagefile;virtual memory;remote" },
        { L"Windows Defender Firewall (Legacy)", L"firewall.cpl", L"System and Security", L"firewall.cpl;firewall;allow an app through firewall;inbound rules;outbound rules;turn firewall on or off;network firewall" },
        { L"Devices and Printers (Classic)", L"control printers", L"Hardware and Sound", L"control printers;printers;classic printers;devices and printers;add printer;printer properties;print queue" },
        { L"Remote Desktop Connection", L"mstsc.exe", L"System and Security", L"mstsc.exe;mstsc;remote desktop;rdp;remote connect;terminal services" },
        { L"About Windows (Winver)", L"winver.exe", L"System", L"winver.exe;winver;windows version;os build;build number;windows 11 edition" },
        { L"Performance Options", L"SystemPropertiesPerformance.exe", L"System and Security", L"SystemPropertiesPerformance.exe;performance options;visual effects;adjust for best performance;smooth edges of screen fonts;virtual memory;page file;paging file size" },
        { L"System Protection (System Restore)", L"SystemPropertiesProtection.exe", L"System and Security", L"SystemPropertiesProtection.exe;system protection;system restore;create restore point;configure restore;restore points" },
        { L"System Properties - Computer Name", L"SystemPropertiesComputerName.exe", L"System and Security", L"SystemPropertiesComputerName.exe;rename pc;workgroup;domain;full computer name" },
        { L"System Properties - Hardware", L"SystemPropertiesHardware.exe", L"Hardware and Sound", L"SystemPropertiesHardware.exe;device installation settings;hardware wizard" },
        { L"System Properties - Remote", L"SystemPropertiesRemote.exe", L"System and Security", L"SystemPropertiesRemote.exe;remote desktop;remote assistance;allow remote connections" },
        { L"Data Execution Prevention (DEP)", L"SystemPropertiesDataExecutionPrevention.exe", L"System and Security", L"SystemPropertiesDataExecutionPrevention.exe;dep;data execution prevention;turn on dep" },
        { L"God Mode (All Tasks)", L"explorer.exe shell:::{ED7BA470-8E54-465E-825C-99712043E01C}", L"Administrative Tools", L"god mode;godmode;all tasks;all settings;master control panel;every setting" },

        // Modern Windows 11 Deep Settings URIs
        { L"Core Isolation (Memory Integrity)", L"ms-settings:coreisolation", L"Update and security", L"ms-settings:coreisolation;core isolation;memory integrity;hvci;hypervisor;vbs;virtualization-based security;driver blacklist;vulnerable driver blocklist;security processor" },
        { L"Exploit Protection", L"ms-settings:exploitprotection", L"Update and security", L"ms-settings:exploitprotection;exploit protection;dep;cfg;aslr;system exploit protection;mitigations" },
        { L"Sound Devices & Properties", L"ms-settings:sound-devices", L"System", L"ms-settings:sound-devices;sound devices;output devices;input devices;default playback device;sample rate;bitrate;audio enhancement;audio properties" },
        { L"Encrypted DNS & IP Settings", L"ms-settings:network-ethernet", L"Network and Internet", L"ms-settings:network-ethernet;dns;encrypted dns;dns over https;doh;static ip;dhcp;ipv4;ipv6;gateway;subnet mask;ip address" },
        { L"Installed Apps (Windows 11)", L"ms-settings:installed-apps", L"Apps", L"ms-settings:installed-apps;installed apps;uninstall apps;remove apps;installed programs;manage apps" },
    };
    return kSettings;
}

} // namespace settings
// ===========================================================================
// Apps & Settings Indexer
// ===========================================================================
// An index of installed applications, from the shell's AppsFolder.
//
// AppsFolder is the virtual folder that unions Win32 shortcuts and Store
// apps, so one enumeration covers both. Measured on a real machine: 187 apps,
// ~200 ms to enumerate names but ~9.7 ms per icon -- which is why names are
// enumerated once up front and icons are fetched lazily, for visible rows
// only.
//
// Enumeration runs in-process on the background search thread in
// StartMenuExperienceHost at Medium integrity.

#include <cwctype>
#include <memory>
#include <mutex>
#include <sstream>

namespace apps {

// PKEY_Link_TargetParsingPath. Not from propkey.h: after initguid.h, that
// header would define every key it declares.
inline constexpr PROPERTYKEY kLinkTargetParsingPath = {
    {0xb9b4b3fc, 0x2b51, 0x4a42, {0xb5, 0xd8, 0x32, 0x41, 0x46, 0xaf, 0xcf, 0x25}}, 2};

namespace detail {

struct PidlDeleter {
    void operator()(ITEMIDLIST* p) const noexcept {
        if (p) {
            CoTaskMemFree(p);
        }
    }
};

}  // namespace detail

using UniquePidl = std::unique_ptr<ITEMIDLIST, detail::PidlDeleter>;

struct App {
    std::wstring name;
    std::wstring nameLower;        // precomputed, so filtering never allocates
    std::wstring targetPath;       // file path, AUMID, or ms-settings: URI
    std::wstring targetPathLower;  // precomputed lowercase target path
    std::wstring linkTarget;       // the program an app-ID entry's shortcut runs, if the shell knows it
    std::wstring exeNameLower;     // executable / command name (e.g. "cmd", "wt", "calc")
    std::wstring acronym;          // acronym from name words (e.g. "cp" for Command Prompt)
    std::vector<std::wstring> words;   // individual words in name
    std::vector<std::wstring> aliases; // smart aliases / keywords
    UniquePidl pidl;
    bool isSetting = false;        // true if Windows Setting or Control Panel page
    std::wstring area;             // Category/Area (e.g. "System", "Devices", "Personalization")
};

struct Match {
    const App* app;
    int score;  // lower is better
    int rank;   // the score less the head start for use (UsageBonus): the order shown
};

// How often apps and custom commands were opened. App keys are lowercase
// names; commands have their own namespace. Owned by the search thread and
// kept together in the mod's storage so disabling learning forgets both.
using UsageCounts = std::unordered_map<std::wstring, int>;

constexpr wchar_t kUsageValueName[] = L"appLaunchCounts";
constexpr size_t kUsageKept = 300;   // most used first; keeps the value small
constexpr int kUsageCap = 1000;

inline int UsageCount(const UsageCounts* usage, const std::wstring& key) {
    if (!usage) return 0;
    auto found = usage->find(key);
    return found == usage->end() ? 0 : std::clamp(found->second, 0, kUsageCap);
}

inline void RecordUsage(UsageCounts* usage, const std::wstring& key) {
    if (usage) (*usage)[key] = std::min(UsageCount(usage, key) + 1, kUsageCap);
}

// A head start, in score points, for apps opened often, applied only among
// matches of the same kind (MatchTier): an often opened name prefix moves up
// past other prefixes, never past an exact match. 3 points after one launch,
// 9 at most, from about seven.
inline int UsageBonus(const UsageCounts* usage, const std::wstring& nameLower) {
    const int count = UsageCount(usage, nameLower);
    return count ? std::min(9, static_cast<int>(std::lround(3.0 * std::log2(1.0 + count)))) : 0;
}

// The kind of match a ScoreApp score stands for: exact (name, program, alias),
// prefix, acronym or substring, fuzzy, typo.
inline int MatchTier(int score) {
    return score < 10 ? 0 : score < 30 ? 1 : score < 80 ? 2 : score < 120 ? 3 : 4;
}

// One line per entry: count, a tab, the app name or namespaced command key.
inline UsageCounts LoadUsage() {
    UsageCounts usage;
    std::vector<wchar_t> buffer(64 * 1024);
    size_t length = Wh_GetStringValue(kUsageValueName, buffer.data(), buffer.size());
    std::wstring text(buffer.data(), length);
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = text.find(L'\n', pos);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        size_t tab = text.find(L'\t', pos);
        if (tab != std::wstring::npos && tab < end) {
            int count = _wtoi(text.substr(pos, tab - pos).c_str());
            std::wstring name = text.substr(tab + 1, end - tab - 1);
            if (count > 0 && !name.empty()) {
                usage[name] = std::min(count, kUsageCap);
            }
        }
        pos = end + 1;
    }
    return usage;
}

inline void SaveUsage(const UsageCounts& usage) {
    std::vector<std::pair<int, std::wstring>> sorted;
    sorted.reserve(usage.size());
    for (const auto& [name, count] : usage) {
        sorted.emplace_back(count, name);
    }
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    if (sorted.size() > kUsageKept) {
        sorted.resize(kUsageKept);
    }
    std::wstring text;
    for (const auto& [count, name] : sorted) {
        text += std::to_wstring(count) + L'\t' + name + L'\n';
    }
    Wh_SetStringValue(kUsageValueName, text.c_str());
}

inline std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return s;
}

inline std::wstring CommandUsageKey(const std::wstring& name) {
    // An app with the same display name must not share the command's count.
    return L"\x1F" L"command:" + ToLower(name);
}

inline std::wstring ResolveAppParsingPath(const std::wstring& parse) {
    if (parse.empty()) return L"";
    if (parse.front() == L'{') {
        size_t closing = parse.find(L'}');
        if (closing != std::wstring::npos) {
            std::wstring guidStr = parse.substr(0, closing + 1);
            GUID guid{};
            if (SUCCEEDED(IIDFromString(guidStr.c_str(), &guid))) {
                PWSTR kfPath = nullptr;
                if (SUCCEEDED(SHGetKnownFolderPath(guid, 0, NULL, &kfPath)) && kfPath) {
                    std::wstring resolved = kfPath;
                    CoTaskMemFree(kfPath);
                    if (closing + 1 < parse.size()) {
                        resolved += parse.substr(closing + 1);
                    }
                    return resolved;
                }
            }
        }
    }
    return parse;
}

inline std::wstring ResolveLnkTarget(const std::wstring& lnkPath) {
    if (lnkPath.size() < 4) return L"";
    std::wstring ext = lnkPath.substr(lnkPath.size() - 4);
    for (auto& c : ext) c = static_cast<wchar_t>(towlower(c));
    if (ext != L".lnk") return L"";

    IShellLinkW* psl = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&psl)))) {
        return L"";
    }
    IPersistFile* ppf = nullptr;
    std::wstring result;
    if (SUCCEEDED(psl->QueryInterface(IID_PPV_ARGS(&ppf)))) {
        if (SUCCEEDED(ppf->Load(lnkPath.c_str(), STGM_READ))) {
            wchar_t szTarget[MAX_PATH] = {};
            if (SUCCEEDED(psl->GetPath(szTarget, MAX_PATH, nullptr, 0)) && szTarget[0]) {
                result = szTarget;
            }
        }
        ppf->Release();
    }
    psl->Release();
    return result;
}

inline std::wstring ExtractExeName(const std::wstring& path) {
    if (path.empty()) return L"";
    if (path.starts_with(L"ms-settings:")) {
        std::wstring sub = path.substr(12);
        size_t q = sub.find_first_of(L"?#");
        if (q != std::wstring::npos) sub = sub.substr(0, q);
        return sub;
    }
    if (path.starts_with(L"control ")) {
        return L"control";
    }
    std::wstring lower = ToLower(path);
    size_t exePos = lower.rfind(L".exe");
    if (exePos != std::wstring::npos && exePos > 0) {
        size_t start = lower.find_last_of(L"\\/._", exePos - 1);
        if (start == std::wstring::npos) {
            return lower.substr(0, exePos);
        } else {
            return lower.substr(start + 1, exePos - (start + 1));
        }
    }
    size_t lastSlash = path.find_last_of(L"\\/");
    std::wstring filename = (lastSlash != std::wstring::npos) ? path.substr(lastSlash + 1) : path;
    // A packaged app's ID (Microsoft.WindowsNotepad_8wekyb3d8bbwe!App): the
    // name is the package's last dotted part, not what precedes the first dot.
    size_t bang = filename.find_last_of(L'!');
    if (bang != std::wstring::npos) {
        std::wstring pkg = filename.substr(0, std::min(bang, filename.find_first_of(L'_')));
        size_t dotInPkg = pkg.find_last_of(L'.');
        return dotInPkg != std::wstring::npos ? pkg.substr(dotInPkg + 1) : pkg;
    }
    size_t dot = filename.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        return filename.substr(0, dot);
    }
    return filename;
}

inline void PopulateAppAliases(App& a) {
    if (a.exeNameLower.empty()) {
        if (a.nameLower == L"command prompt") a.exeNameLower = L"cmd";
        else if (a.nameLower.find(L"powershell") != std::wstring::npos) a.exeNameLower = L"powershell";
        else if (a.nameLower == L"terminal" || a.nameLower == L"windows terminal") a.exeNameLower = L"wt";
        else if (a.nameLower == L"task manager") a.exeNameLower = L"taskmgr";
        else if (a.nameLower == L"registry editor") a.exeNameLower = L"regedit";
        else if (a.nameLower == L"calculator") a.exeNameLower = L"calc";
        else if (a.nameLower == L"control panel") a.exeNameLower = L"control";
        else if (a.nameLower == L"file explorer") a.exeNameLower = L"explorer";
        else if (a.nameLower == L"notepad") a.exeNameLower = L"notepad";
        else if (a.nameLower == L"paint") a.exeNameLower = L"mspaint";
        else if (a.nameLower.find(L"snipping") != std::wstring::npos) a.exeNameLower = L"snippingtool";
        else if (a.nameLower.find(L"remote desktop") != std::wstring::npos) a.exeNameLower = L"mstsc";
        else if (a.nameLower.find(L"visual studio code") != std::wstring::npos) a.exeNameLower = L"code";
    }

    if (a.targetPath.empty()) {
        if (a.exeNameLower == L"cmd") a.targetPath = L"C:\\Windows\\System32\\cmd.exe";
        else if (a.exeNameLower == L"powershell") a.targetPath = L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
        else if (a.exeNameLower == L"taskmgr") a.targetPath = L"C:\\Windows\\System32\\Taskmgr.exe";
        else if (a.exeNameLower == L"regedit") a.targetPath = L"C:\\Windows\\regedit.exe";
        else if (a.exeNameLower == L"control") a.targetPath = L"C:\\Windows\\System32\\control.exe";
        else if (a.exeNameLower == L"explorer") a.targetPath = L"C:\\Windows\\explorer.exe";
        else if (a.exeNameLower == L"notepad") a.targetPath = L"C:\\Windows\\System32\\notepad.exe";
        else if (a.exeNameLower == L"mspaint") a.targetPath = L"C:\\Windows\\System32\\mspaint.exe";
    }

    if (!a.exeNameLower.empty() && a.exeNameLower != a.nameLower) {
        a.aliases.push_back(a.exeNameLower);
    }

    if (a.exeNameLower == L"cmd" || a.nameLower == L"command prompt") {
        a.aliases.insert(a.aliases.end(), {L"cmd", L"cmd.exe", L"command", L"prompt", L"terminal", L"console", L"cli", L"shell", L"dos"});
    } else if (a.exeNameLower == L"powershell" || a.nameLower.find(L"powershell") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"powershell", L"pwsh", L"ps", L"posh", L"shell", L"terminal", L"console"});
    } else if (a.nameLower == L"terminal" || a.targetPathLower.find(L"windowsterminal") != std::wstring::npos || a.exeNameLower == L"wt") {
        a.aliases.insert(a.aliases.end(), {L"wt", L"wt.exe", L"terminal", L"windows terminal", L"bash", L"cmd", L"powershell", L"console"});
    } else if (a.exeNameLower == L"taskmgr" || a.nameLower == L"task manager") {
        a.aliases.insert(a.aliases.end(), {L"taskmgr", L"taskmgr.exe", L"task", L"tasks", L"process", L"processes", L"kill", L"performance"});
    } else if (a.exeNameLower == L"regedit" || a.nameLower == L"registry editor") {
        a.aliases.insert(a.aliases.end(), {L"regedit", L"regedit.exe", L"reg", L"registry"});
    } else if (a.exeNameLower == L"calc" || a.nameLower == L"calculator") {
        a.aliases.insert(a.aliases.end(), {L"calc", L"calc.exe", L"calculator", L"math"});
    } else if (a.exeNameLower == L"control" || a.nameLower == L"control panel") {
        a.aliases.insert(a.aliases.end(), {L"control", L"control.exe", L"control panel", L"cpl", L"settings"});
    } else if (a.exeNameLower == L"explorer" || a.nameLower == L"file explorer") {
        a.aliases.insert(a.aliases.end(), {L"explorer", L"explorer.exe", L"files", L"my pc", L"this pc"});
    } else if (a.exeNameLower == L"notepad" || a.nameLower == L"notepad") {
        a.aliases.insert(a.aliases.end(), {L"notepad", L"notepad.exe", L"text", L"editor", L"notes", L"txt"});
    } else if (a.exeNameLower == L"mspaint" || a.nameLower == L"paint") {
        a.aliases.insert(a.aliases.end(), {L"mspaint", L"paint", L"pbrush", L"draw", L"sketch"});
    } else if (a.nameLower.find(L"snipping") != std::wstring::npos || a.targetPathLower.find(L"snippingtool") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"snippingtool", L"snip", L"screenshot", L"capture", L"screen"});
    } else if (a.exeNameLower == L"mstsc" || a.nameLower.find(L"remote desktop") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"mstsc", L"mstsc.exe", L"rdp", L"remote"});
    } else if (a.exeNameLower == L"code" || a.nameLower.find(L"visual studio code") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"code", L"code.exe", L"vscode", L"vs code", L"ide", L"editor"});
    } else if (a.nameLower.find(L"device manager") != std::wstring::npos || a.targetPathLower.find(L"devmgmt") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"devmgmt", L"devmgmt.msc", L"device manager", L"devices"});
    } else if (a.nameLower.find(L"disk management") != std::wstring::npos || a.targetPathLower.find(L"diskmgmt") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"diskmgmt", L"diskmgmt.msc", L"partition", L"disk"});
    } else if (a.nameLower == L"services") {
        a.aliases.insert(a.aliases.end(), {L"services", L"services.msc"});
    } else if (a.nameLower.find(L"event viewer") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"eventvwr", L"eventvwr.msc", L"event viewer", L"logs"});
    } else if (a.nameLower.find(L"system configuration") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"msconfig", L"msconfig.exe"});
    } else if (a.nameLower.find(L"system information") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"msinfo32", L"msinfo"});
    } else if (a.nameLower == L"settings") {
        a.aliases.insert(a.aliases.end(), {L"settings", L"control", L"preferences", L"config"});
    } else if (a.nameLower == L"word" || a.exeNameLower == L"winword" || a.targetPathLower.find(L"winword") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"winword", L"winword.exe", L"word", L"doc", L"docx", L"document", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"winword";
    } else if (a.nameLower == L"excel" || a.exeNameLower == L"excel" || a.targetPathLower.find(L"excel") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"excel", L"excel.exe", L"xls", L"xlsx", L"sheet", L"spreadsheet", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"excel";
    } else if (a.nameLower == L"powerpoint" || a.exeNameLower == L"powerpnt" || a.targetPathLower.find(L"powerpnt") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"powerpnt", L"powerpnt.exe", L"powerpoint", L"ppt", L"pptx", L"slides", L"presentation", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"powerpnt";
    } else if (a.nameLower == L"access" || a.exeNameLower == L"msaccess" || a.targetPathLower.find(L"msaccess") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"msaccess", L"msaccess.exe", L"access", L"database", L"accdb", L"mdb", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"msaccess";
    } else if (a.nameLower == L"outlook" || a.exeNameLower == L"outlook" || a.targetPathLower.find(L"outlook") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"outlook", L"outlook.exe", L"mail", L"email", L"calendar", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"outlook";
    } else if (a.nameLower == L"onenote" || a.exeNameLower == L"onenote" || a.targetPathLower.find(L"onenote") != std::wstring::npos) {
        a.aliases.insert(a.aliases.end(), {L"onenote", L"onenote.exe", L"notes", L"notebook", L"office"});
        if (a.exeNameLower.empty()) a.exeNameLower = L"onenote";
    }
}

inline int DamerauLevenshteinDistance(const std::wstring& s1, const std::wstring& s2, int maxDist = 2) {
    int len1 = static_cast<int>(s1.size());
    int len2 = static_cast<int>(s2.size());
    if (std::abs(len1 - len2) > maxDist) return maxDist + 1;
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;

    constexpr int kMaxDim = 40;
    if (len1 >= kMaxDim || len2 >= kMaxDim) return maxDist + 1;

    int d[kMaxDim + 1][kMaxDim + 1];
    for (int i = 0; i <= len1; ++i) d[i][0] = i;
    for (int j = 0; j <= len2; ++j) d[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        int rowMin = 999;
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({
                d[i - 1][j] + 1,       // deletion
                d[i][j - 1] + 1,       // insertion
                d[i - 1][j - 1] + cost // substitution
            });
            // Transposition
            if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1]) {
                d[i][j] = std::min(d[i][j], d[i - 2][j - 2] + 1);
            }
            if (d[i][j] < rowMin) rowMin = d[i][j];
        }
        if (rowMin > maxDist) return maxDist + 1;
    }
    return d[len1][len2];
}

inline int ScoreApp(const App& app, const std::wstring& q) {
    if (q.empty()) return -1;
    int s = -1;

    // 1. Exact Name match
    if (app.nameLower == q) {
        s = 0;
    }
    // 2. Exact Exe Name match (e.g. q == "cmd")
    else if (!app.exeNameLower.empty() && app.exeNameLower == q) {
        s = 2;
    }
    // 3. Exact Alias match (e.g. q == "cmd", "wifi", "calc")
    else {
        for (const auto& al : app.aliases) {
            if (al == q) {
                s = 4;
                break;
            }
        }
    }

    // 4. Name starts with query
    if (s < 0) {
        if (app.nameLower.starts_with(q)) {
            s = 10;
        }
        // 5. Exe Name starts with query
        else if (!app.exeNameLower.empty() && app.exeNameLower.starts_with(q)) {
            s = 15;
        }
        // 6. Word in Name starts with query
        else {
            for (const auto& w : app.words) {
                if (w.starts_with(q)) {
                    s = 20;
                    break;
                }
            }
            // 7. Alias starts with query
            if (s < 0) {
                for (const auto& al : app.aliases) {
                    if (al.starts_with(q)) {
                        s = 25;
                        break;
                    }
                }
            }
        }
    }

    // 8. Exact Acronym match (e.g. "cp" -> "Command Prompt")
    if (s < 0) {
        if (!app.acronym.empty() && app.acronym == q) {
            s = 30;
        }
        // 9. Substring in Name
        else {
            size_t namePos = app.nameLower.find(q);
            if (namePos != std::wstring::npos) {
                s = 40 + static_cast<int>(std::min<size_t>(namePos, 20));
            }
            // 10. Substring in Exe / Target Path
            else if (!app.targetPathLower.empty() && app.targetPathLower.find(q) != std::wstring::npos) {
                s = 60;
            }
            // 11. Substring in any alias
            else {
                for (const auto& al : app.aliases) {
                    if (al.find(q) != std::wstring::npos) {
                        s = 70;
                        break;
                    }
                }
            }
        }
    }

    // 12. Fuzzy subsequence match on Name or Exe Name
    if (s < 0) {
        auto isSubsequence = [](const std::wstring& pattern, const std::wstring& text, int& gaps) -> bool {
            if (pattern.size() > text.size()) return false;
            size_t p = 0;
            size_t lastMatch = 0;
            gaps = 0;
            for (size_t t = 0; t < text.size() && p < pattern.size(); ++t) {
                if (pattern[p] == text[t]) {
                    if (p > 0) gaps += static_cast<int>(t - lastMatch - 1);
                    lastMatch = t;
                    ++p;
                }
            }
            return p == pattern.size();
        };

        // Letters the query leaves unmatched count against it too, so a typo
        // of a short name beats the same letters strewn through a long one:
        // "setings" is Settings, not File Converter Settings.
        auto spare = [&q](const std::wstring& text) {
            return static_cast<int>((text.size() - q.size()) / 4);
        };
        int gaps = 0;
        if (q.size() >= 2 && isSubsequence(q, app.nameLower, gaps)) {
            s = 80 + std::min(gaps + spare(app.nameLower), 30);
        } else if (q.size() >= 2 && !app.exeNameLower.empty() && isSubsequence(q, app.exeNameLower, gaps)) {
            s = 85 + std::min(gaps + spare(app.exeNameLower), 30);
        }
    }

    // 13. Typo-tolerant edit distance matching
    if (s < 0 && q.size() >= 3) {
        int maxDist = (q.size() <= 4) ? 1 : 2;
        int bestDist = 999;

        auto checkCandidate = [&](const std::wstring& cand) {
            if (cand.empty()) return;
            // Whole string distance
            int d = DamerauLevenshteinDistance(q, cand, maxDist);
            if (d <= maxDist && d < bestDist) {
                bestDist = d;
            }
            // Prefix distance if candidate is longer
            if (cand.size() > q.size()) {
                std::wstring candPfx = cand.substr(0, q.size());
                int pfxD = DamerauLevenshteinDistance(q, candPfx, maxDist);
                if (pfxD <= maxDist && pfxD < bestDist) {
                    bestDist = pfxD;
                }
                if (cand.size() > q.size() + 1) {
                    std::wstring candPfx1 = cand.substr(0, q.size() + 1);
                    int pfxD1 = DamerauLevenshteinDistance(q, candPfx1, maxDist);
                    if (pfxD1 <= maxDist && pfxD1 < bestDist) {
                        bestDist = pfxD1;
                    }
                }
            }
        };

        checkCandidate(app.nameLower);
        if (!app.exeNameLower.empty()) {
            checkCandidate(app.exeNameLower);
        }
        for (const auto& w : app.words) {
            checkCandidate(w);
        }
        for (const auto& al : app.aliases) {
            checkCandidate(al);
        }

        if (bestDist <= maxDist) {
            s = 120 + bestDist * 15;
        }
    }

    if (s >= 0 && app.isSetting) {
        s += 1; // tie-breaker: slightly prefer real apps on exact ties
    }
    return s;
}

class Index {
   public:
    Index() = default;
    Index(const Index&) = delete;
    Index& operator=(const Index&) = delete;

    bool Rebuild() {
        std::vector<App> fresh;

        IShellItem* folder = nullptr;
        HRESULT hr = SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr,
                                                 IID_PPV_ARGS(&folder));
        if (FAILED(hr) || !folder) {
            return false;
        }

        IEnumShellItems* e = nullptr;
        hr = folder->BindToHandler(nullptr, BHID_EnumItems, IID_PPV_ARGS(&e));
        folder->Release();
        if (FAILED(hr) || !e) {
            return false;
        }

        IShellItem* item = nullptr;
        while (e->Next(1, &item, nullptr) == S_OK && item) {
            App a;
            LPWSTR display = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &display)) &&
                display) {
                a.name = display;
                CoTaskMemFree(display);
            }
            LPWSTR fsPath = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &fsPath)) && fsPath) {
                std::wstring resolved = ResolveLnkTarget(fsPath);
                if (!resolved.empty()) {
                    a.targetPath = resolved;
                } else {
                    a.targetPath = fsPath;
                }
                CoTaskMemFree(fsPath);
            }
            
            LPWSTR parse = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &parse)) && parse) {
                std::wstring resolvedParse = ResolveAppParsingPath(parse);
                if (a.targetPath.empty()) {
                    a.targetPath = resolvedParse;
                } else if (a.targetPath.size() >= 4 && 
                           ToLower(a.targetPath.substr(a.targetPath.size() - 4)) == L".lnk" &&
                           !resolvedParse.empty() && resolvedParse.front() != L'{') {
                    a.targetPath = resolvedParse;
                }
                CoTaskMemFree(parse);
            }

            // An app registered under an app ID (Windhawk's is
            // RamenSoftware.Windhawk) has the ID as its parsing name. For a
            // desktop app the shell still knows the program its shortcut
            // runs; a packaged app has none.
            if (a.targetPath.find(L":\\") == std::wstring::npos) {
                IShellItem2* item2 = nullptr;
                if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&item2))) && item2) {
                    LPWSTR target = nullptr;
                    if (SUCCEEDED(item2->GetString(kLinkTargetParsingPath, &target)) && target) {
                        a.linkTarget = target;
                        CoTaskMemFree(target);
                    }
                    item2->Release();
                }
            }

            ITEMIDLIST* pidl = nullptr;
            if (SUCCEEDED(SHGetIDListFromObject(item, &pidl)) && pidl) {
                a.pidl.reset(pidl);
            }
            if (!a.name.empty() && a.pidl) {
                a.nameLower = ToLower(a.name);
                a.targetPathLower = ToLower(a.targetPath);
                a.exeNameLower = ToLower(ExtractExeName(a.linkTarget.empty() ? a.targetPath : a.linkTarget));

                std::wistringstream ss(a.nameLower);
                std::wstring word;
                while (ss >> word) {
                    while (!word.empty() && iswpunct(word.front())) word.erase(word.begin());
                    while (!word.empty() && iswpunct(word.back())) word.pop_back();
                    if (!word.empty()) {
                        a.words.push_back(word);
                        a.acronym.push_back(word[0]);
                    }
                }

                PopulateAppAliases(a);
                if (a.nameLower == L"settings" || a.nameLower == L"windows settings" ||
                    a.targetPathLower.find(L"immersivecontrolpanel") != std::wstring::npos ||
                    a.exeNameLower == L"systemsettings") {
                    a.isSetting = true;
                }
                fresh.push_back(std::move(a));
            }
            item->Release();
            item = nullptr;
        }
        e->Release();

        // Append Windows Settings
        for (const auto& s : settings::GetSettingsList()) {
            App a;
            a.name = s.name;
            a.targetPath = s.command;
            a.area = s.area;
            a.isSetting = true;
            a.nameLower = ToLower(a.name);
            a.targetPathLower = ToLower(a.targetPath);
            a.exeNameLower = ToLower(ExtractExeName(a.targetPath));

            std::wistringstream ss(a.nameLower);
            std::wstring word;
            while (ss >> word) {
                while (!word.empty() && iswpunct(word.front())) word.erase(word.begin());
                while (!word.empty() && iswpunct(word.back())) word.pop_back();
                if (!word.empty()) {
                    a.words.push_back(word);
                    a.acronym.push_back(word[0]);
                }
            }

            if (s.altNames && s.altNames[0]) {
                std::wistringstream alts(s.altNames);
                std::wstring alt;
                while (std::getline(alts, alt, L';')) {
                    if (!alt.empty()) {
                        a.aliases.push_back(ToLower(alt));
                    }
                }
            }
            if (!a.area.empty()) {
                a.aliases.push_back(ToLower(a.area));
            }

            fresh.push_back(std::move(a));
        }

        std::lock_guard<std::mutex> lock(mutex_);
        apps_ = std::move(fresh);
        return true;
    }

    size_t Count() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return apps_.size();
    }

    std::vector<Match> Search(const std::wstring& query, size_t limit, const UsageCounts* usage) const {
        std::vector<Match> hits;
        if (query.empty()) {
            return hits;
        }
        const std::wstring q = ToLower(query);

        std::lock_guard<std::mutex> lock(mutex_);
        for (const App& a : apps_) {
            int score = ScoreApp(a, q);
            if (score >= 0) {
                hits.push_back({&a, score, score - UsageBonus(usage, a.nameLower)});
            }
        }

        std::stable_sort(hits.begin(), hits.end(),
                         [](const Match& x, const Match& y) {
                             if (MatchTier(x.score) != MatchTier(y.score)) {
                                 return x.score < y.score;
                             }
                             if (x.rank != y.rank) {
                                 return x.rank < y.rank;
                             }
                             if (x.score != y.score) {
                                 return x.score < y.score;
                             }
                             return x.app->name.size() < y.app->name.size();
                         });

        // What is shown:
        //  - one entry per name: many things are indexed twice, typically a
        //    Settings page and the Control Panel applet or app behind it
        //    (Display, Sound, Device Manager), and a second row with the same
        //    name only takes a slot from something else;
        //  - with a real match (below the fuzzy tiers at 80), no fuzzy ones:
        //    "calc" is Calculator, not Local Computer Policy;
        //  - with only fuzzy or typo matches, just those close to the best.
        // By the score itself: use reorders what is shown, never what is.
        const int kFuzzy = 80;
        int best = hits.empty() ? 0 : hits.front().score;
        for (const Match& m : hits) {
            best = std::min(best, m.score);
        }
        const int cutoff = best < kFuzzy ? kFuzzy : best + 12;
        std::vector<Match> shown;
        shown.reserve(std::min(limit, hits.size()));
        for (const Match& m : hits) {
            if (shown.size() >= limit) {
                break;
            }
            if (m.score >= cutoff) {
                continue;
            }
            bool seen = false;
            for (const Match& kept : shown) {
                if (kept.app->nameLower == m.app->nameLower) {
                    seen = true;
                    break;
                }
            }
            if (!seen) {
                shown.push_back(m);
            }
        }
        return shown;
    }

    // Empty search shows installed apps, with learned favorites first. Turning
    // learning off uses an alphabetical list and never reads the saved counts.
    std::vector<Match> Suggest(size_t limit,const UsageCounts* usage) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Match> hits;
        for(const App& app:apps_) {
            if(app.isSetting || app.name.empty() || (!app.pidl && app.targetPath.empty())) continue;
            const int uses=UsageCount(usage,app.nameLower);
            hits.push_back({&app,0,-uses});
        }
        std::stable_sort(hits.begin(),hits.end(),[](const Match& a,const Match& b) {
            if(a.rank!=b.rank) return a.rank<b.rank;
            return a.app->nameLower<b.app->nameLower;
        });
        std::vector<Match> shown;
        shown.reserve(std::min(limit,hits.size()));
        for(const Match& hit:hits) {
            if(shown.size()>=limit) break;
            if(std::none_of(shown.begin(),shown.end(),[&](const Match& kept) {
                return kept.app->nameLower==hit.app->nameLower;
            })) shown.push_back(hit);
        }
        return shown;
    }

    // The app at position i, for work that walks the whole index (the icon
    // prefetch). Valid until the next Rebuild, which only the search thread
    // calls -- the same thread that walks it.
    const App* At(size_t i) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return i < apps_.size() ? &apps_[i] : nullptr;
    }

   private:
    mutable std::mutex mutex_;
    std::vector<App> apps_;
};

}  // namespace apps
// ===========================================================================
// Calculator & Power Tools
// ===========================================================================

#include <iphlpapi.h>
#include <cmath>
#include <iomanip>
#include <cctype>

#ifndef DROPEFFECT_COPY
#define DROPEFFECT_COPY 1
#endif
#ifndef DROPEFFECT_MOVE
#define DROPEFFECT_MOVE 2
#endif

namespace tools {

// ---------------------------------------------------------------------------
// 1. Clipboard Copy & Cut
// ---------------------------------------------------------------------------
inline bool CopyTextToClipboard(const std::wstring& text) {
    if (!OpenClipboard(nullptr)) {
        return false;
    }
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem) {
        void* pMem = GlobalLock(hMem);
        if (pMem) {
            memcpy(pMem, text.c_str(), bytes);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
    }
    CloseClipboard();
    return true;
}

inline bool CopyOrCutFileToClipboard(const std::wstring& filePath, bool isCut) {
    if (filePath.empty()) return false;
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();

    // 1. CF_HDROP (Shell file list)
    size_t pathLen = filePath.size();
    size_t dropFilesSize = sizeof(DROPFILES);
    size_t totalBytes = dropFilesSize + (pathLen + 2) * sizeof(wchar_t);

    HGLOBAL hDrop = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, totalBytes);
    if (hDrop) {
        char* pData = static_cast<char*>(GlobalLock(hDrop));
        if (pData) {
            DROPFILES* pDrop = reinterpret_cast<DROPFILES*>(pData);
            pDrop->pFiles = static_cast<DWORD>(dropFilesSize);
            pDrop->fWide = TRUE;
            wchar_t* pDest = reinterpret_cast<wchar_t*>(pData + dropFilesSize);
            memcpy(pDest, filePath.c_str(), pathLen * sizeof(wchar_t));
            pDest[pathLen] = L'\0';
            pDest[pathLen + 1] = L'\0';
            GlobalUnlock(hDrop);
            SetClipboardData(CF_HDROP, hDrop);
        } else {
            GlobalFree(hDrop);
        }
    }

    // 2. Preferred DropEffect (DROPEFFECT_COPY or DROPEFFECT_MOVE)
    UINT uDropEffect = RegisterClipboardFormatW(L"Preferred DropEffect");
    if (uDropEffect) {
        HGLOBAL hEffect = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
        if (hEffect) {
            DWORD* pEffect = static_cast<DWORD*>(GlobalLock(hEffect));
            if (pEffect) {
                *pEffect = isCut ? DROPEFFECT_MOVE : DROPEFFECT_COPY;
                GlobalUnlock(hEffect);
                SetClipboardData(uDropEffect, hEffect);
            } else {
                GlobalFree(hEffect);
            }
        }
    }

    // 3. CF_UNICODETEXT (fallback for text fields & text editors)
    size_t textBytes = (pathLen + 1) * sizeof(wchar_t);
    HGLOBAL hText = GlobalAlloc(GMEM_MOVEABLE, textBytes);
    if (hText) {
        wchar_t* pText = static_cast<wchar_t*>(GlobalLock(hText));
        if (pText) {
            memcpy(pText, filePath.c_str(), textBytes);
            GlobalUnlock(hText);
            SetClipboardData(CF_UNICODETEXT, hText);
        } else {
            GlobalFree(hText);
        }
    }

    CloseClipboard();
    return true;
}

inline bool CreateDesktopShortcut(const std::wstring& targetPath, const std::wstring& preferredName = L"") {
    if (targetPath.empty()) return false;
    if (targetPath.starts_with(L"ms-settings:") || targetPath.find(L"immersivecontrolpanel") != std::wstring::npos) {
        return false;
    }

    PWSTR desktopFolder = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopFolder);
    if (FAILED(hr) || !desktopFolder) return false;

    std::wstring desktop = desktopFolder;
    CoTaskMemFree(desktopFolder);

    std::wstring baseName = preferredName;
    if (baseName.empty()) {
        size_t slash = targetPath.find_last_of(L"\\/");
        baseName = (slash != std::wstring::npos) ? targetPath.substr(slash + 1) : targetPath;
        if (baseName.size() > 4 && _wcsicmp(baseName.c_str() + baseName.size() - 4, L".lnk") == 0) {
            baseName.resize(baseName.size() - 4);
        } else if (baseName.size() > 4 && _wcsicmp(baseName.c_str() + baseName.size() - 4, L".exe") == 0) {
            baseName.resize(baseName.size() - 4);
        }
    }

    for (wchar_t& ch : baseName) {
        if (wcschr(L"\\/:*?\"<>|", ch)) {
            ch = L'_';
        }
    }
    while (!baseName.empty() && (baseName.back() == L' ' || baseName.back() == L'.')) {
        baseName.pop_back();
    }
    size_t first = baseName.find_first_not_of(L' ');
    if (first != std::wstring::npos && first > 0) {
        baseName = baseName.substr(first);
    }
    if (baseName.empty()) baseName = L"Shortcut";

    // A unique name, because neither write path below asks before replacing
    // what is already on the desktop: CopyFileW's third argument is
    // bFailIfExists, and IPersistFile::Save always overwrites. An installer's
    // own "Google Chrome.lnk" may carry a profile argument, a custom icon or a
    // hotkey, and replacing it with a bare link to the exe loses all of that
    // with no prompt and no way back.
    //
    // PathYetAnotherMakeUniqueName yields "Name.lnk", then "Name (2).lnk", and
    // so on -- the same convention Explorer's own "Create shortcut" uses.
    // The trailing backslash is load-bearing. Without one this treats the last
    // component as a file name and resolves against the *parent*, so passing
    // "C:\Users\me\Desktop" quietly writes to "C:\Users\me". SHGetKnownFolderPath
    // returns the path without it, so it has to be added here.
    std::wstring spec = baseName + L".lnk";
    std::wstring desktopDir = desktop + L"\\";
    WCHAR unique[MAX_PATH];
    if (!PathYetAnotherMakeUniqueName(unique, desktopDir.c_str(), nullptr,
                                      spec.c_str())) {
        Wh_Log(L"No unique shortcut name for %s", spec.c_str());
        return false;
    }
    std::wstring shortcutFile = unique;

    if (targetPath.size() > 4 && _wcsicmp(targetPath.c_str() + targetPath.size() - 4, L".lnk") == 0) {
        if (GetFileAttributesW(targetPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            // Fail rather than overwrite. The name was unique a moment ago, so
            // this only fires if something else got there in between.
            if (CopyFileW(targetPath.c_str(), shortcutFile.c_str(), TRUE)) {
                return true;
            }
        }
    }

    IShellLinkW* psl = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, reinterpret_cast<void**>(&psl));
    if (FAILED(hr) || !psl) return false;

    // Logged rather than fatal. SetPath is documented for file-system paths,
    // and UWP entries come through here as "shell:AppsFolder\<AUMID>", so a
    // failure here is exactly the case worth seeing in a log -- but refusing
    // outright would stop creating links that may be working today, which is
    // not something to change without testing it.
    hr = psl->SetPath(targetPath.c_str());
    if (FAILED(hr)) {
        Wh_Log(L"SetPath(%s) failed: %08X", targetPath.c_str(),
               static_cast<unsigned>(hr));
    }

    size_t lastSlash = targetPath.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos && !targetPath.starts_with(L"shell:")) {
        std::wstring dir = targetPath.substr(0, lastSlash);
        psl->SetWorkingDirectory(dir.c_str());
    }

    IPersistFile* ppf = nullptr;
    hr = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&ppf));
    bool success = false;
    if (SUCCEEDED(hr) && ppf) {
        hr = ppf->Save(shortcutFile.c_str(), TRUE);
        success = SUCCEEDED(hr);
        ppf->Release();
    }
    psl->Release();
    return success;
}

// ---------------------------------------------------------------------------
// 2. Local IPv4 Resolver & Network Interfaces
// ---------------------------------------------------------------------------
struct LocalIPv4Info {
    std::wstring ip;
    std::wstring adapterType;
};

inline std::vector<LocalIPv4Info> GetLocalIPv4Addresses() {
    std::vector<LocalIPv4Info> list;
    ULONG size = 0;
    if (GetIpAddrTable(nullptr, &size, FALSE) == ERROR_INSUFFICIENT_BUFFER && size > 0) {
        std::vector<BYTE> buf(size);
        PMIB_IPADDRTABLE pTable = reinterpret_cast<PMIB_IPADDRTABLE>(buf.data());
        if (GetIpAddrTable(pTable, &size, FALSE) == NO_ERROR) {
            for (DWORD i = 0; i < pTable->dwNumEntries; ++i) {
                DWORD addr = pTable->table[i].dwAddr;
                BYTE b1 = static_cast<BYTE>((addr >> 0) & 0xFF);
                BYTE b2 = static_cast<BYTE>((addr >> 8) & 0xFF);
                BYTE b3 = static_cast<BYTE>((addr >> 16) & 0xFF);
                BYTE b4 = static_cast<BYTE>((addr >> 24) & 0xFF);

                // Filter loopback (127.x), zero (0.0.0.0), APIPA (169.254.x)
                if (b1 == 127 || b1 == 0 || (b1 == 169 && b2 == 254)) {
                    continue;
                }

                wchar_t ipBuf[64];
                swprintf_s(ipBuf, L"%u.%u.%u.%u", b1, b2, b3, b4);

                std::wstring type = L"Local IPv4";
                if (b1 == 192 && b2 == 168) {
                    type = L"Wi-Fi / LAN (192.168.x.x)";
                } else if (b1 == 10) {
                    type = L"Private Network (10.x.x.x)";
                } else if (b1 == 172 && (b2 >= 16 && b2 <= 31)) {
                    type = L"Private Network (172.16.x.x)";
                } else if (b1 == 26) {
                    type = L"Virtual Network / VPN";
                }

                list.push_back({ipBuf, type});
            }
        }
    }
    return list;
}

struct NetworkInterfaceInfo {
    std::wstring ip;
    std::wstring mask;
    std::wstring gateway;
    std::wstring adapterName;
    std::wstring typeLabel;
    std::wstring glyph;
};

inline std::vector<NetworkInterfaceInfo> GetNetworkInterfaces() {
    std::vector<NetworkInterfaceInfo> list;
    ULONG size = 0;
    DWORD ret = GetAdaptersInfo(nullptr, &size);
    if (ret == ERROR_BUFFER_OVERFLOW && size > 0) {
        std::vector<BYTE> buf(size);
        PIP_ADAPTER_INFO pInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buf.data());
        if (GetAdaptersInfo(pInfo, &size) == NO_ERROR) {
            for (PIP_ADAPTER_INFO pCurr = pInfo; pCurr != nullptr; pCurr = pCurr->Next) {
                std::string desc = pCurr->Description;
                std::string descLower = desc;
                for (char& c : descLower) c = static_cast<char>(tolower(c));

                std::wstring typeLabel = L"Network";
                std::wstring glyph = L"\uE701";

                if (pCurr->Type == 71 || descLower.find("wi-fi") != std::string::npos || descLower.find("wireless") != std::string::npos) {
                    typeLabel = L"Wi-Fi";
                    glyph = L"\uE704";
                } else if (descLower.find("vpn") != std::string::npos || descLower.find("radmin") != std::string::npos || descLower.find("wireguard") != std::string::npos || descLower.find("tailscale") != std::string::npos || descLower.find("tap") != std::string::npos) {
                    typeLabel = L"VPN";
                    glyph = L"\uE701";
                } else if (pCurr->Type == 6) {
                    typeLabel = L"Ethernet";
                    glyph = L"\uE839";
                }

                int wlen = MultiByteToWideChar(CP_ACP, 0, desc.c_str(), -1, nullptr, 0);
                std::wstring wDesc(wlen > 1 ? wlen - 1 : 0, L'\0');
                if (wlen > 1) {
                    MultiByteToWideChar(CP_ACP, 0, desc.c_str(), -1, &wDesc[0], wlen);
                }

                std::string gw = (pCurr->GatewayList.IpAddress.String[0] != '\0' && strcmp(pCurr->GatewayList.IpAddress.String, "0.0.0.0") != 0) ? pCurr->GatewayList.IpAddress.String : "";
                std::wstring wGw(gw.begin(), gw.end());

                for (IP_ADDR_STRING* pIp = &(pCurr->IpAddressList); pIp != nullptr; pIp = pIp->Next) {
                    std::string ipStr = pIp->IpAddress.String;
                    if (ipStr.empty() || ipStr == "0.0.0.0" || ipStr.starts_with("127.")) {
                        continue;
                    }
                    std::string maskStr = pIp->IpMask.String;
                    std::wstring wIp(ipStr.begin(), ipStr.end());
                    std::wstring wMask(maskStr.begin(), maskStr.end());

                    list.push_back({
                        wIp,
                        wMask,
                        wGw,
                        wDesc,
                        typeLabel,
                        glyph
                    });
                }
            }
        }
    }

    if (list.empty()) {
        auto fallback = GetLocalIPv4Addresses();
        for (const auto& fb : fallback) {
            list.push_back({
                fb.ip,
                L"",
                L"",
                fb.adapterType,
                L"Local IPv4",
                L"\uE701"
            });
        }
    }
    return list;
}

// ---------------------------------------------------------------------------
// 3. String & Number Formatting Helpers
// ---------------------------------------------------------------------------
inline std::wstring FormatCleanNumber(double val) {
    if (std::isnan(val) || std::isinf(val)) {
        return L"Error";
    }
    // Check if effectively integer
    double rounded = std::round(val);
    if (std::abs(val - rounded) < 1e-9 && std::abs(val) < 1e15) {
        return std::to_wstring(static_cast<long long>(rounded));
    }
    // Decimal formatting
    wchar_t buf[64];
    swprintf_s(buf, L"%.6f", val);
    std::wstring s = buf;
    while (!s.empty() && s.back() == L'0') s.pop_back();
    if (!s.empty() && s.back() == L'.') s.pop_back();
    return s;
}

inline std::wstring ToLower(const std::wstring& str) {
    std::wstring res = str;
    for (auto& c : res) c = static_cast<wchar_t>(towlower(c));
    return res;
}

inline std::wstring Trim(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// ---------------------------------------------------------------------------
// 4. Math Expression Evaluator
// ---------------------------------------------------------------------------
namespace detail {

struct Token {
    enum Type { Number, Plus, Minus, Mul, Div, Mod, Pow, LParen, RParen, End, Ident } type;
    double value = 0.0;
    std::wstring text;
};

inline bool IsIdentChar(wchar_t c) {
    return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || (c == L'_');
}

inline std::vector<Token> Tokenize(const std::wstring& expr) {
    std::vector<Token> tokens;
    size_t i = 0;
    size_t n = expr.size();

    while (i < n) {
        wchar_t c = expr[i];
        if (c == L' ' || c == L'\t' || c == L'\r' || c == L'\n') {
            i++;
            continue;
        }

        // Check for Hex prefix: 0x or 0X
        if (c == L'0' && i + 1 < n && (expr[i + 1] == L'x' || expr[i + 1] == L'X')) {
            size_t start = i + 2;
            size_t end = start;
            while (end < n && iswxdigit(expr[end])) end++;
            if (end > start) {
                std::wstring hexStr = expr.substr(start, end - start);
                wchar_t* pEnd = nullptr;
                unsigned long long val = wcstoull(hexStr.c_str(), &pEnd, 16);
                tokens.push_back({Token::Number, static_cast<double>(val), L""});
                i = end;
                continue;
            }
        }

        // Numbers: digits or '.' followed by digit
        if (iswdigit(c) || (c == L'.' && i + 1 < n && iswdigit(expr[i + 1]))) {
            size_t start = i;
            while (i < n && (iswdigit(expr[i]) || expr[i] == L'.')) i++;
            // Scientific notation: e+10, e-5
            if (i < n && (expr[i] == L'e' || expr[i] == L'E')) {
                i++;
                if (i < n && (expr[i] == L'+' || expr[i] == L'-')) i++;
                while (i < n && iswdigit(expr[i])) i++;
            }
            std::wstring numStr = expr.substr(start, i - start);
            wchar_t* pEnd = nullptr;
            double val = wcstod(numStr.c_str(), &pEnd);
            tokens.push_back({Token::Number, val, L""});
            continue;
        }

        // Operators & Parens
        if (c == L'+') { tokens.push_back({Token::Plus}); i++; continue; }
        if (c == L'-') { tokens.push_back({Token::Minus}); i++; continue; }
        if (c == L'*') { tokens.push_back({Token::Mul}); i++; continue; }
        if (c == L'x' || c == L'X') {
            // Can be multiply if preceded by a number or closing paren, and followed by whitespace or number
            if (!tokens.empty() && (tokens.back().type == Token::Number || tokens.back().type == Token::RParen)) {
                tokens.push_back({Token::Mul});
                i++;
                continue;
            }
        }
        if (c == L'/') { tokens.push_back({Token::Div}); i++; continue; }
        if (c == L'%') { tokens.push_back({Token::Mod}); i++; continue; }
        if (c == L'^') { tokens.push_back({Token::Pow}); i++; continue; }
        if (c == L'(') { tokens.push_back({Token::LParen}); i++; continue; }
        if (c == L')') { tokens.push_back({Token::RParen}); i++; continue; }

        // Identifiers / function names / constants
        if (IsIdentChar(c)) {
            size_t start = i;
            while (i < n && IsIdentChar(expr[i])) i++;
            std::wstring word = ToLower(expr.substr(start, i - start));
            if (word == L"pi") {
                tokens.push_back({Token::Number, 3.14159265358979323846, L""});
            } else if (word == L"e") {
                tokens.push_back({Token::Number, 2.71828182845904523536, L""});
            } else {
                tokens.push_back({Token::Ident, 0.0, word});
            }
            continue;
        }

        // Unrecognized character -> cannot parse
        return {};
    }

    tokens.push_back({Token::End});
    return tokens;
}

class Parser {
   public:
    explicit Parser(std::vector<Token> t) : tokens(std::move(t)), pos(0) {}

    bool Parse(double& result) {
        if (tokens.empty() || tokens.front().type == Token::End) return false;
        try {
            result = ParseExpression();
            return Current().type == Token::End && !std::isnan(result) && !std::isinf(result);
        } catch (...) {
            return false;
        }
    }

   private:
    const std::vector<Token> tokens;
    size_t pos;

    const Token& Current() const {
        return (pos < tokens.size()) ? tokens[pos] : tokens.back();
    }

    void Consume() {
        if (pos < tokens.size()) pos++;
    }

    // Expression = Term (('+' | '-') Term)*
    double ParseExpression() {
        double left = ParseTerm();
        while (true) {
            if (Current().type == Token::Plus) {
                Consume();
                left += ParseTerm();
            } else if (Current().type == Token::Minus) {
                Consume();
                left -= ParseTerm();
            } else {
                break;
            }
        }
        return left;
    }

    // Term = Power (('*' | '/' | '%') Power)*
    double ParseTerm() {
        double left = ParsePower();
        while (true) {
            if (Current().type == Token::Mul) {
                Consume();
                left *= ParsePower();
            } else if (Current().type == Token::Div) {
                Consume();
                double right = ParsePower();
                if (std::abs(right) < 1e-15) throw false;
                left /= right;
            } else if (Current().type == Token::Mod) {
                Consume();
                double right = ParsePower();
                if (std::abs(right) < 1e-15) throw false;
                left = std::fmod(left, right);
            } else {
                break;
            }
        }
        return left;
    }

    // Power = Factor ('^' Factor)*
    double ParsePower() {
        double left = ParseFactor();
        if (Current().type == Token::Pow) {
            Consume();
            double right = ParsePower(); // right associative
            left = std::pow(left, right);
        }
        return left;
    }

    // Factor = ('+' | '-')? Primary
    double ParseFactor() {
        if (Current().type == Token::Plus) {
            Consume();
            return ParseFactor();
        }
        if (Current().type == Token::Minus) {
            Consume();
            return -ParseFactor();
        }
        return ParsePrimary();
    }

    // Primary = Number | '(' Expression ')' | Ident '(' Expression ')'
    double ParsePrimary() {
        const Token& cur = Current();
        if (cur.type == Token::Number) {
            double v = cur.value;
            Consume();
            return v;
        }
        if (cur.type == Token::LParen) {
            Consume();
            double v = ParseExpression();
            if (Current().type != Token::RParen) throw false;
            Consume();
            return v;
        }
        if (cur.type == Token::Ident) {
            std::wstring fn = cur.text;
            Consume();
            if (Current().type != Token::LParen) throw false;
            Consume();
            double arg = ParseExpression();
            if (Current().type != Token::RParen) throw false;
            Consume();

            if (fn == L"sqrt") {
                if (arg < 0.0) throw false;
                return std::sqrt(arg);
            } else if (fn == L"cbrt") {
                return std::cbrt(arg);
            } else if (fn == L"abs") {
                return std::abs(arg);
            } else if (fn == L"sin") {
                return std::sin(arg);
            } else if (fn == L"cos") {
                return std::cos(arg);
            } else if (fn == L"tan") {
                return std::tan(arg);
            } else if (fn == L"log" || fn == L"log10") {
                if (arg <= 0.0) throw false;
                return std::log10(arg);
            } else if (fn == L"ln") {
                if (arg <= 0.0) throw false;
                return std::log(arg);
            } else if (fn == L"round") {
                return std::round(arg);
            } else if (fn == L"floor") {
                return std::floor(arg);
            } else if (fn == L"ceil") {
                return std::ceil(arg);
            }
            throw false;
        }
        throw false;
    }
};

} // namespace detail

inline bool IsStandaloneNumber(const std::wstring& input, double& outNum) {
    std::wstring t = Trim(input);
    if (t.empty()) return false;

    wchar_t* pEnd = nullptr;
    double val = wcstod(t.c_str(), &pEnd);
    if (pEnd && *pEnd == L'\0' && pEnd != t.c_str()) {
        outNum = val;
        return true;
    }
    return false;
}

inline bool EvaluateMath(const std::wstring& input, double& outResult) {
    std::wstring trimmed = Trim(input);
    if (trimmed.empty()) return false;

    // Handle "X% of Y" pattern
    std::wstring lower = ToLower(trimmed);
    size_t ofPos = lower.find(L"% of ");
    if (ofPos != std::wstring::npos) {
        std::wstring part1 = Trim(trimmed.substr(0, ofPos));
        std::wstring part2 = Trim(trimmed.substr(ofPos + 5));
        double pct = 0, base = 0;
        auto evalVal = [](const std::wstring& s, double& val) -> bool {
            if (IsStandaloneNumber(s, val)) return true;
            return EvaluateMath(s, val);
        };
        if (evalVal(part1, pct) && evalVal(part2, base)) {
            outResult = (pct / 100.0) * base;
            return true;
        }
    }

    auto tokens = detail::Tokenize(trimmed);
    if (tokens.size() <= 1) return false;

    // Ensure it contains at least one operator or function or paren, not just a standalone number
    bool hasOperator = false;
    for (const auto& t : tokens) {
        if (t.type != detail::Token::Number && t.type != detail::Token::End) {
            hasOperator = true;
            break;
        }
    }
    if (!hasOperator) return false;

    detail::Parser parser(std::move(tokens));
    return parser.Parse(outResult);
}

inline bool EvaluateConversionFormula(const std::wstring& formula, double n, double& outResult) {
    std::wstring expr = Trim(formula);
    if (expr.empty()) return false;

    // Check if formula is just a numeric multiplier (e.g. "0.621371")
    wchar_t* pEnd = nullptr;
    double mult = wcstod(expr.c_str(), &pEnd);
    if (pEnd && *pEnd == L'\0' && pEnd != expr.c_str()) {
        outResult = n * mult;
        return true;
    }

    std::wstring nStr = FormatCleanNumber(n);
    if (n < 0) nStr = L"(" + nStr + L")";

    auto replaceAll = [](std::wstring& s, const std::wstring& from, const std::wstring& to) {
        size_t pos = 0;
        while ((pos = s.find(from, pos)) != std::wstring::npos) {
            s.replace(pos, from.length(), to);
            pos += to.length();
        }
    };

    replaceAll(expr, L"{n}", nStr);
    replaceAll(expr, L"{N}", nStr);
    replaceAll(expr, L"{x}", nStr);
    replaceAll(expr, L"{X}", nStr);

    for (size_t i = 0; i < expr.size(); ++i) {
        if (expr[i] == L'x' || expr[i] == L'X') {
            bool leftOk = (i == 0 || (!iswalnum(expr[i - 1]) && expr[i - 1] != L'_'));
            bool rightOk = (i + 1 == expr.size() || (!iswalnum(expr[i + 1]) && expr[i + 1] != L'_'));
            if (leftOk && rightOk) {
                expr.replace(i, 1, nStr);
                i += nStr.size() - 1;
            }
        }
    }

    return EvaluateMath(expr, outResult);
}

// ---------------------------------------------------------------------------
// 5. Standalone Number & Multi-Unit Conversions (/c)
// ---------------------------------------------------------------------------
inline std::wstring NormalizeUnit(const std::wstring& unitRaw) {
    std::wstring u = ToLower(Trim(unitRaw));
    if (u == L"c" || u == L"\u00B0c" || u == L"celsius") return L"c";
    if (u == L"f" || u == L"\u00B0f" || u == L"fahrenheit") return L"f";
    if (u == L"k" || u == L"kelvin") return L"k";
    if (u == L"km" || u == L"kilometer" || u == L"kilometers") return L"km";
    if (u == L"mi" || u == L"mile" || u == L"miles") return L"miles";
    if (u == L"m" || u == L"meter" || u == L"meters") return L"m";
    if (u == L"cm" || u == L"centimeter" || u == L"centimeters") return L"cm";
    if (u == L"mm" || u == L"millimeter" || u == L"millimeters") return L"mm";
    if (u == L"in" || u == L"inch" || u == L"inches" || u == L"\"") return L"in";
    if (u == L"ft" || u == L"foot" || u == L"feet" || u == L"'") return L"feet";
    if (u == L"yd" || u == L"yard" || u == L"yards") return L"yd";
    if (u == L"kg" || u == L"kilo" || u == L"kilogram" || u == L"kilograms") return L"kg";
    if (u == L"lb" || u == L"lbs" || u == L"pound" || u == L"pounds") return L"lbs";
    if (u == L"g" || u == L"gram" || u == L"grams") return L"g";
    if (u == L"oz" || u == L"ounce" || u == L"ounces") return L"oz";
    if (u == L"kmh" || u == L"km/h" || u == L"kph") return L"km/h";
    if (u == L"mph") return L"mph";
    if (u == L"b" || u == L"bytes" || u == L"byte") return L"bytes";
    if (u == L"kb" || u == L"kilobyte" || u == L"kilobytes") return L"kb";
    if (u == L"mb" || u == L"megabyte" || u == L"megabytes") return L"mb";
    if (u == L"gb" || u == L"gigabyte" || u == L"gigabytes") return L"gb";
    if (u == L"tb" || u == L"terabyte" || u == L"terabytes") return L"tb";
    if (u == L"s" || u == L"sec" || u == L"second" || u == L"seconds") return L"s";
    if (u == L"min" || u == L"minute" || u == L"minutes") return L"min";
    if (u == L"h" || u == L"hr" || u == L"hrs" || u == L"hour" || u == L"hours") return L"hours";
    if (u == L"d" || u == L"day" || u == L"days") return L"days";
    if (u == L"l" || u == L"liter" || u == L"liters") return L"l";
    if (u == L"gal" || u == L"gallon" || u == L"gallons") return L"gal";
    if (u == L"bar") return L"bar";
    if (u == L"psi") return L"psi";
    if (u == L"kw") return L"kw";
    if (u == L"hp") return L"hp";
    return u;
}

} // namespace tools

#include <array>
#include <d2d1.h>
#include <d2d1effects.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.Data.Pdf.h>
#include <winrt/Windows.Media.Core.h>
#include <winrt/Windows.Media.Playback.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Management.Deployment.h>

#pragma pop_macro("GetCurrentTime")

#include <robuffer.h>
#include <windhawk_utils.h>

namespace wf = winrt::Windows::Foundation;
namespace wut = winrt::Windows::UI::Text;
namespace wuc = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wui = winrt::Windows::UI::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;
namespace wuxma = winrt::Windows::UI::Xaml::Media::Animation;

static std::atomic<bool> g_quit{false};

// ===========================================================================
// Domain: Process Identification
// ===========================================================================

enum class TargetProcess {
    Unknown,
    StartMenu,
    SearchHost,
    Explorer
};

static TargetProcess g_targetProcess = TargetProcess::Unknown;

// Local Windows 11 prototype: search has its own desktop XAML window.
static constexpr wchar_t kSearchWindowClass[] = L"StartEverything_SearchWindowPrototype";
static std::atomic<HWND> g_searchWindowWindow{nullptr};
static bool g_searchWindowWaitingForStart = false; // search UI thread only
static bool g_searchWindowSliding = false; // search UI thread only
static bool g_renderAfterSlide = false; // latest results wait for the short slide
static UINT SearchWindowForegroundMessage() {
    static UINT id = RegisterWindowMessageW(L"StartEverything_SearchWindowForeground");
    return id;
}
static UINT SearchWindowAppearanceMessage() {
    static UINT id = RegisterWindowMessageW(L"StartEverything_SearchWindowAppearance");
    return id;
}
static UINT_PTR SearchAppearanceTimerId() {
    static const int identity = 0;
    return reinterpret_cast<UINT_PTR>(&identity);
}
constexpr ULONG_PTR kSearchWindowAppearanceData = 0x53455354; // 'SEST'
struct SearchBrushData {
    uint32_t kind = 0, color = 0, fallback = 0, count = 0;
    double opacity = 1, tint = 1, luminosity = -1;
    double blur = 3, saturation = 1, points[4]{0,0,1,1};
    uint32_t colors[8]{};
    double offsets[8]{};
    bool operator==(SearchBrushData const&) const = default;
};
struct SearchSurfaceData {
    uint32_t flags = 0;
    SearchBrushData background, outline;
    uint32_t foreground = 0;
    double corners[4]{}, thickness[4]{}, padding[4]{}, margin[4]{};
    bool operator==(SearchSurfaceData const&) const = default;
};
enum class SearchTaskbarEdge : uint32_t { Left, Top, Right, Bottom };
static SearchTaskbarEdge g_searchTaskbarEdge = SearchTaskbarEdge::Bottom;

SearchTaskbarEdge TaskbarEdgeFromBounds(RECT const& bar, RECT const& screen) {
    if (bar.right - bar.left > bar.bottom - bar.top)
        return bar.top + (bar.bottom - bar.top) / 2 < screen.top + (screen.bottom - screen.top) / 2
            ? SearchTaskbarEdge::Top : SearchTaskbarEdge::Bottom;
    return bar.left + (bar.right - bar.left) / 2 < screen.left + (screen.right - screen.left) / 2
        ? SearchTaskbarEdge::Left : SearchTaskbarEdge::Right;
}

SearchTaskbarEdge TaskbarEdgeForMonitor(HMONITOR screen) {
    struct Probe { HMONITOR screen; SearchTaskbarEdge edge; bool found = false; } probe{screen, SearchTaskbarEdge::Bottom};
    EnumWindows([](HWND window, LPARAM value) -> BOOL {
        auto& probe = *reinterpret_cast<Probe*>(value);
        wchar_t name[64]{};
        if (!GetClassNameW(window, name, ARRAYSIZE(name)) ||
            (wcscmp(name, L"Shell_TrayWnd") && wcscmp(name, L"Shell_SecondaryTrayWnd")) ||
            MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) != probe.screen) return TRUE;
        RECT bar{};
        MONITORINFO monitor{sizeof(monitor)};
        if (!GetWindowRect(window, &bar) || IsRectEmpty(&bar) || !GetMonitorInfoW(probe.screen, &monitor)) return TRUE;
        probe.edge = TaskbarEdgeFromBounds(bar, monitor.rcMonitor);
        probe.found = true;
        return FALSE;
    }, reinterpret_cast<LPARAM>(&probe));
    if (probe.found) return probe.edge;
    // A display without its own taskbar follows the primary taskbar's edge.
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        RECT bar{};
        MONITORINFO monitor{sizeof(monitor)};
        if (GetWindowRect(tray, &bar) && !IsRectEmpty(&bar) &&
            GetMonitorInfoW(MonitorFromWindow(tray, MONITOR_DEFAULTTONEAREST), &monitor))
            return TaskbarEdgeFromBounds(bar, monitor.rcMonitor);
    }
    return SearchTaskbarEdge::Bottom;
}

RECT SearchBoundsForEdge(RECT const& work, int width, int height, int gap,
                         double alignment, SearchTaskbarEdge edge) {
    const bool vertical = edge == SearchTaskbarEdge::Left || edge == SearchTaskbarEdge::Right;
    const int availableX = std::max(0L, work.right - work.left - width);
    const int availableY = std::max(0L, work.bottom - work.top - height);
    alignment = std::clamp(alignment, 0.0, 1.0);
    gap = std::clamp(gap, 0, vertical ? availableX : availableY);
    int left = work.left + static_cast<int>(availableX * alignment);
    int top = work.top + static_cast<int>(availableY * alignment);
    switch (edge) {
    case SearchTaskbarEdge::Left: left = work.left + gap; break;
    case SearchTaskbarEdge::Right: left = work.right - gap - width; break;
    case SearchTaskbarEdge::Top: top = work.top + gap; break;
    case SearchTaskbarEdge::Bottom: top = work.bottom - gap - height; break;
    }
    return {left, top, left + width, top + height};
}

POINT SearchSlideTranslation(SearchTaskbarEdge edge, int pixels) {
    switch (edge) {
    case SearchTaskbarEdge::Left: return {-pixels, 0};
    case SearchTaskbarEdge::Top: return {0, -pixels};
    case SearchTaskbarEdge::Right: return {pixels, 0};
    case SearchTaskbarEdge::Bottom: return {0, pixels};
    }
    return {};
}

double SearchHiddenDistance(RECT const& bounds, RECT const& work, double scale, SearchTaskbarEdge edge) {
    switch (edge) {
    case SearchTaskbarEdge::Left: return (bounds.right - work.left) / scale + 16;
    case SearchTaskbarEdge::Top: return (bounds.bottom - work.top) / scale + 16;
    case SearchTaskbarEdge::Right: return (work.right - bounds.left) / scale + 16;
    case SearchTaskbarEdge::Bottom: return (bounds.bottom - bounds.top) / scale + 16;
    }
    return 0;
}

// Keep side/top slides on their destination display. The XAML child retains
// its full layout size; the native host reveals only the part inside work.
RECT SearchSlideViewport(RECT const& frame, RECT const& work, SearchTaskbarEdge edge) {
    if (edge == SearchTaskbarEdge::Bottom || IsRectEmpty(&work)) return frame;
    RECT visible{};
    if (IntersectRect(&visible, &frame, &work)) return visible;
    const LONG x = std::clamp(frame.left, work.left, work.right - 1);
    const LONG y = std::clamp(frame.top, work.top, work.bottom - 1);
    return {x, y, x + 1, y + 1};  // completely hidden by the native region
}

struct SearchAppearanceData {
    uint32_t version = 4, dark = 1;
    uint32_t taskbarEdge = static_cast<uint32_t>(SearchTaskbarEdge::Bottom), reserved = 0;
    uint32_t foreground = 0, searchForeground = 0;
    double width = 0, height = 0, edgeGap = 12, alignment = 0.5;
    double corners[4]{8,8,8,8}, thickness[4]{1,1,1,1};
    double searchCorners[4]{4,4,4,4}, searchThickness[4]{1,1,1,1};
    SearchBrushData background, outline, searchBackground, searchOutline;
    SearchSurfaceData menu, menuBorder, menuItem;
};
static SearchAppearanceData g_searchAppearance;
static bool g_searchAppearanceValid = false;
static bool g_searchAppearanceDirty = false;
namespace {
void LoadSettings();
bool SearchWindowInitialize();
void StartSearchWindowInput();
void StopSearchWindowInput();
void ApplySearchWindowInput();
bool HasBufferedSearchWindowInput();
constexpr UINT kSearchWindowInputMessage = WM_APP + 0x73;
void SearchWindowTrace(const wchar_t* message, HRESULT hr);
void SearchWindowTeardown();
bool SearchWindowPreTranslate(MSG* message);
void SearchWindowShow(std::wstring const& text = L"", bool append = false);
void SearchWindowHide();
void SearchWindowLayout(bool animate = true);
void SearchWindowStyle();
void QueueSearchBoxRefocus(bool bodyClick = false);
bool DismissSearchContextMenu();
void CaptureStartAppearance(HWND target);
void RequestStartAppearance();
void ApplyStartAppearance();
void ApplySearchTextColors();
uint32_t SearchTextOverride();
void InvalidateSearchContrastContainer(wux::FrameworkElement const& root);
void PrepareSearchResultContrastColors();
void SearchContrastScrolled(wuxc::ScrollViewer const& scroll);
void SearchContainerContrastTick();
void ApplySearchContainerContrast();
void PauseSearchContainerContrast();
void ResetSearchContainerContrast();
void StopSearchContainerContrast();
void UpdateSearchWindowShape();
void InitializeCopiedMenu(wuxc::MenuFlyout const& menu);
void RefreshCopiedMenuStyles();

}


TargetProcess IdentifyCurrentProcess() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(NULL, path, MAX_PATH);
    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? (name + 1) : path;
    if (_wcsicmp(name, L"StartMenuExperienceHost.exe") == 0) return TargetProcess::StartMenu;
    if (_wcsicmp(name, L"SearchHost.exe") == 0) return TargetProcess::SearchHost;
    if (_wcsicmp(name, L"explorer.exe") == 0) return TargetProcess::Explorer;
    return TargetProcess::Unknown;
}

// ===========================================================================
// Domain: explorer.exe (Start and Search window lookup)
// ===========================================================================
//
// Explorer's foreground calls are not hooked. Explorer only holds the right to
// change the foreground because it received the click or keypress, and
// AllowSetForegroundWindow hands that right over: a grant made before
// Explorer's own SetForegroundWindow left Explorer unable to make the switch,
// so with another app in front, clicking Start did nothing. Explorer switches
// to SearchHost exactly as it does without this mod, and SearchHost -- then
// the foreground process -- is what lets Start take over.

// Start publishes its CoreWindow on the taskbar when it attaches. Reading it
// back is a property lookup, and the PID comes from GetWindowThreadProcessId,
// which opens no process handle -- cheap enough to call on any activation.
static HWND CachedStartWindow() {
    static std::atomic<HWND> s_start{nullptr};
    HWND h = s_start.load(std::memory_order_relaxed);
    if (h && IsWindow(h)) {
        return h;
    }
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    h = tray ? static_cast<HWND>(GetPropW(tray, L"WindhawkStartMenuHwnd")) : nullptr;
    if (h && !IsWindow(h)) {
        h = nullptr;
    }
    if (!h) {
        HWND hStart = FindWindowW(L"Windows.UI.Core.CoreWindow", L"Start");
        if (hStart && IsWindow(hStart)) {
            h = hStart;
            if (tray) {
                SetPropW(tray, L"WindhawkStartMenuHwnd", hStart);
            }
        }
    }
    if (!h) {
        EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
            if (GetPropW(hwnd, L"WindhawkStartMenuWindow")) {
                *reinterpret_cast<HWND*>(lParam) = hwnd;
                return FALSE;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&h));
        if (h && IsWindow(h) && tray) {
            SetPropW(tray, L"WindhawkStartMenuHwnd", h);
        }
    }
    s_start.store(h, std::memory_order_relaxed);
    return h;
}

static bool IsSearchHostWindow(HWND hwnd) {
    static std::atomic<DWORD> s_searchPid{0};
    static std::atomic<DWORD> s_otherPid{0};
    if (!hwnd) {
        return false;
    }
    wchar_t cls[32];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) ||
        wcscmp(cls, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == s_otherPid.load(std::memory_order_relaxed)) {
        return false;
    }
    if (pid == s_searchPid.load(std::memory_order_relaxed)) {
        return true;
    }
    bool match = false;
    if (HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid)) {
        wchar_t path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            const wchar_t* name = wcsrchr(path, L'\\');
            match = _wcsicmp(name ? name + 1 : path, L"SearchHost.exe") == 0;
        }
        CloseHandle(process);
    }
    (match ? s_searchPid : s_otherPid).store(pid, std::memory_order_relaxed);
    return match;
}

// Explorer's helper sends this to Start's CoreWindow to ask whether a request
// to remove a Store app is really Start's (AcceptPackageRemoval): lParam is
// the family name's hash; Start answers 1 while it is asking for just that
// (RequestPackageRemoval).
static UINT RemovalCheckMessage() {
    static const UINT message = RegisterWindowMessageW(L"StartEverything_RemovalCheck");
    return message;
}

// Explorer's helper posts this to Start's CoreWindow when a drag of a result
// it ran (StartFileDrag) is over: wParam 1 if it was dropped.
static UINT DragDoneMessage() {
    static const UINT message = RegisterWindowMessageW(L"StartEverything_DragDone");
    return message;
}

// Tracked launch threads for clean unload synchronization across processes
static std::mutex g_launchHandlesMutex;
static std::vector<HANDLE> g_launchHandles;

template <typename F>
static void SpawnTrackedLaunch(F&& f) {
    auto fnCopy = new std::decay_t<F>(std::forward<F>(f));
    HANDLE h = CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
        auto pFn = reinterpret_cast<std::decay_t<F>*>(param);
        try {
            (*pFn)();
        } catch (...) {}
        delete pFn;
        return 0;
    }, fnCopy, 0, nullptr);

    if (h) {
        std::lock_guard<std::mutex> lock(g_launchHandlesMutex);
        g_launchHandles.erase(
            std::remove_if(g_launchHandles.begin(), g_launchHandles.end(),
                [](HANDLE handle) {
                    if (WaitForSingleObject(handle, 0) == WAIT_OBJECT_0) {
                        CloseHandle(handle);
                        return true;
                    }
                    return false;
                }),
            g_launchHandles.end()
        );
        g_launchHandles.push_back(h);
    } else {
        delete fnCopy;
    }
}

static void WaitForTrackedLaunches() {
    std::vector<HANDLE> handlesToJoin;
    {
        std::lock_guard<std::mutex> lock(g_launchHandlesMutex);
        handlesToJoin = std::move(g_launchHandles);
    }
    for (HANDLE h : handlesToJoin) {
        if (h) {
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
        }
    }
}

// ===========================================================================
// Domain: Explorer Shell Property Relay
// ===========================================================================

static const wchar_t kExplorerHelperClassName[] = L"StartEverything_ExplorerHostClass";
static const wchar_t kExplorerHelperWindowName[] = L"StartEverything_ExplorerHost";
static const ULONG_PTR kExplorerCopyDataMagic = 0x53455052; // 'SEPR'
static const ULONG_PTR kExplorerDragMagic = 0x53454447;      // 'SEDG': drag a result (StartFileDrag)
static const ULONG_PTR kExplorerUninstallMagic = 0x5345554E; // 'SEUN': remove a packaged app (RequestPackageRemoval)
static const UINT kExplorerStartDrag = WM_APP + 0x31;
static const UINT kExplorerSearchShown = WM_APP + 0x32;
static constexpr UINT_PTR kExplorerSearchStartupTimer = 2;
static unsigned g_explorerSearchStartupAttempts = 0;  // helper thread only
static bool g_explorerSearchRequested = false;  // helper thread only
static constexpr UINT kExplorerOpenSearch = WM_APP + 0x33;
static constexpr UINT kExplorerUpdateShortcut = WM_APP + 0x34;
static constexpr int kAlternativeShortcutId = 0x5345;
static std::atomic<UINT> g_alternativeShortcutDesired{0};
static std::atomic<UINT> g_alternativeShortcutActive{0};
static std::wstring g_explorerDragPath;  // helper thread only

static HANDLE g_hExplorerHelperThread = nullptr;
static DWORD g_explorerHelperThreadId = 0;
static std::atomic<HWND> g_hExplorerHelperWnd{nullptr};
static HANDLE g_hExplorerHelperReadyEvent = nullptr;

// ---------------------------------------------------------------------------
// Native shell search activations must open the independent search window.
// Watch SearchHost activation and Start closing as a fallback for the shell's
// internal hotkeys/search buttons. If Start is opening normally, leave it alone.
// Requests go to the helper thread and coalesce during an existing activation.
// ---------------------------------------------------------------------------
static const UINT_PTR kSearchAloneTimerId = 1;
static const UINT kSearchAloneTickMs = 40;
static const int kSearchAloneTicks = 2;
static const int kSearchAloneMaxTicks = 15;
static HWINEVENTHOOK g_searchForegroundHook = nullptr;
static HWINEVENTHOOK g_startCloakedHook = nullptr;
static bool g_searchAloneArmed = false;
static int g_searchAloneTicksNeeded = 0;
static int g_searchAloneClosedTicks = 0;  // consecutive checks with Start closed
static int g_searchAloneTotalTicks = 0;

static bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return hwnd && SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

static bool IsStartMenuClosed() {
    HWND start = CachedStartWindow();
    return !start || !IsWindowVisible(start) || IsCloaked(start);
}

// SearchHost's CoreWindow, found by process: its title is localized.
static HWND FindSearchHostCoreWindow() {
    static HWND s_search = nullptr;  // helper thread only
    if (s_search && IsSearchHostWindow(s_search)) {
        return s_search;
    }
    s_search = nullptr;
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, L"Windows.UI.Core.CoreWindow", nullptr); hwnd;
         hwnd = FindWindowExW(nullptr, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) {
        if (IsSearchHostWindow(hwnd)) {
            s_search = hwnd;
            break;
        }
    }
    return s_search;
}

// Repeated shell notifications during one search activation must not reopen
// search, clear its query, or restart the opening animation.
static const ULONGLONG kSearchActivationSettleMs = 600;
static ULONGLONG g_searchActivationTick = 0;  // helper thread only

static bool SearchActivationSettling() {
    return g_searchActivationTick && GetTickCount64() - g_searchActivationTick < kSearchActivationSettleMs;
}

static void OpenSearchFromShell() {
    PostMessageW(g_hExplorerHelperWnd.load(), kExplorerOpenSearch, 0, 0);
}

// Only the Explorer process that owns the taskbar may open search.
static bool IsTaskbarProcess() {
    DWORD pid = 0;
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &pid);
    }
    return pid == GetCurrentProcessId();
}

// A packed shortcut is zero when disabled; the key occupies the high byte.
static UINT PackAlternativeShortcut(UINT modifiers, UINT key) {
    return (key << 8) | modifiers;
}

static std::optional<UINT> ParseAlternativeShortcutModifiers(std::wstring_view text) {
    if (text.empty()) return MOD_ALT;
    static constexpr struct { PCWSTR name; UINT value; } values[] = {
        {L"alt", 1},
        {L"ctrl", 2},
        {L"shift", 4},
        {L"win", 8},
        {L"ctrl_alt", 3},
        {L"ctrl_shift", 6},
        {L"alt_shift", 5},
        {L"win_ctrl", 10},
        {L"win_alt", 9},
        {L"win_shift", 12},
        {L"ctrl_alt_shift", 7},
        {L"win_ctrl_alt", 11},
        {L"win_ctrl_shift", 14},
        {L"win_alt_shift", 13},
        {L"win_ctrl_alt_shift", 15},
        {L"none", 0}
    };
    const std::wstring value(text);
    for (auto const& item : values) if (_wcsicmp(value.c_str(),item.name)==0) return item.value;
    return std::nullopt;
}

static UINT ParseAlternativeShortcutKey(std::wstring_view text) {
    if (text.empty()) return VK_SPACE;
    if (text.size()!=4 || text[0]!=L'0' || (text[1]!=L'x' && text[1]!=L'X')) return 0;
    UINT key=0;
    for (size_t i=2;i<4;++i) {
        wchar_t c=text[i];
        int digit=c>=L'0' && c<=L'9' ? c-L'0' : c>=L'a' && c<=L'f' ? c-L'a'+10 :
                  c>=L'A' && c<=L'F' ? c-L'A'+10 : -1;
        if (digit<0) return 0;
        key=(key<<4)|digit;
    }
    static constexpr UINT keys[] = {
        0x20, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x8, 0x9, 0xC, 0xD, 0x13, 0x14, 0x1B, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x5D, 0x5F, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x90, 0x91, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE2, 0x15, 0x17, 0x18, 0x19, 0x1C, 0x1D, 0x1E, 0x1F, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFD, 0xFE
    };
    return std::find(std::begin(keys),std::end(keys),key)!=std::end(keys) ? key : 0;
}

static bool AlternativeShortcutMatches(UINT shortcut, UINT key, const BYTE* state) {
    if (!shortcut || key != (shortcut>>8)) return false;
    const UINT modifiers=((state[VK_MENU]&0x80) ? MOD_ALT : 0) |
        ((state[VK_CONTROL]&0x80) ? MOD_CONTROL : 0) |
        ((state[VK_SHIFT]&0x80) ? MOD_SHIFT : 0) |
        (((state[VK_LWIN]|state[VK_RWIN])&0x80) ? MOD_WIN : 0);
    return modifiers == (shortcut&0xFF);
}

// Registration belongs to the helper's message thread, in the taskbar process.
static void ReleaseAlternativeShortcut(HWND helper) {
    if (g_alternativeShortcutActive.exchange(0)) UnregisterHotKey(helper,kAlternativeShortcutId);
}

static void UpdateAlternativeShortcut(HWND helper) {
    const UINT desired=IsTaskbarProcess() ? g_alternativeShortcutDesired.load() : 0;
    if (desired==g_alternativeShortcutActive.load()) return;
    ReleaseAlternativeShortcut(helper);
    if (!desired) return;
    if (RegisterHotKey(helper,kAlternativeShortcutId,(desired&0xFF)|MOD_NOREPEAT,desired>>8)) {
        g_alternativeShortcutActive=desired;
        SearchWindowTrace(L"alternative search shortcut registered",S_OK);
    } else {
        SearchWindowTrace(L"alternative search shortcut unavailable; choose another combination",
                          HRESULT_FROM_WIN32(GetLastError()));
    }
}

static void ArmSearchAloneCheck(int ticksNeeded) {
    g_searchAloneArmed = true;
    g_searchAloneTicksNeeded = ticksNeeded;
    g_searchAloneClosedTicks = 0;
    g_searchAloneTotalTicks = 0;
    SetTimer(g_hExplorerHelperWnd, kSearchAloneTimerId, kSearchAloneTickMs, nullptr);
}

static void CALLBACK ShellWindowEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                                          LONG idChild, DWORD, DWORD) {
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !g_hExplorerHelperWnd ||
        SearchActivationSettling()) {
        return;
    }
    if (event == EVENT_OBJECT_CLOAKED) {
        // Start's cloak is the last step of a close, never a transition to
        // wait out: one confirming check is enough.
        if (hwnd == CachedStartWindow() && IsTaskbarProcess()) {
            ArmSearchAloneCheck(1);
        }
        return;
    }
    if (event != EVENT_SYSTEM_FOREGROUND || !IsSearchHostWindow(hwnd) || !IsTaskbarProcess()) {
        return;
    }
    bool winKeyDown = ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) & 0x8000) != 0;
    if (winKeyDown && IsStartMenuClosed()) {
        KillTimer(g_hExplorerHelperWnd, kSearchAloneTimerId);
        g_searchAloneArmed = false;
        OpenSearchFromShell();
        return;
    }
    ArmSearchAloneCheck(winKeyDown ? 1 : kSearchAloneTicks);
}

// Returns true once there is nothing left to wait for.
static bool CheckSearchShownWithoutStart() {
    HWND search = FindSearchHostCoreWindow();
    HWND start = CachedStartWindow();
    HWND fg = GetForegroundWindow();
    if (!g_searchAloneArmed || !search || (fg != search && fg != start) || IsCloaked(search) ||
        SearchActivationSettling()) {
        return true;  // the user moved on, Search is not showing, or Start is on its way
    }
    if (fg == start && !IsStartMenuClosed()) {
        return true;  // Start is open and has the keyboard
    }
    if (++g_searchAloneTotalTicks > kSearchAloneMaxTicks) {
        // What is left is an invisible Search holding the keyboard. Worth a
        // trace: the checks above assume timings that may not hold everywhere.
        Wh_Log(L"[Explorer] Search still in front after %d checks, Start %ls; giving up",
               kSearchAloneMaxTicks, IsCloaked(start) ? L"closed" : L"open");
        return true;
    }
    if (!IsStartMenuClosed()) {
        g_searchAloneClosedTicks = 0;  // Start is open, opening, or still closing
        return false;
    }
    if (++g_searchAloneClosedTicks < g_searchAloneTicksNeeded) {
        return false;
    }
    OpenSearchFromShell();
    return true;
}

static int PrimaryButton() {
    return GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
}

// Feeds a drag of a result (RunExplorerDrag) the mouse. The button was
// pressed in Start, and until it is released Windows sends the mouse to the
// window it was pressed on -- Start's -- whichever window holds the capture:
// the drag loop here would never hear of the moves or the release. So a
// thread watches the pointer and tells the loop: a move whenever it has
// moved, and the release once the button is up, each with where the pointer
// is -- the loop takes it from the message, as from a real one -- and the
// loop asks ExplorerDragSource whether to go on.
struct ExplorerDragFeed {
    DWORD loopThread = 0;
    std::atomic<bool> released{false};
    std::atomic<bool> done{false};
};

static DWORD WINAPI ExplorerDragFeedThread(LPVOID param) {
    auto* feed = static_cast<ExplorerDragFeed*>(param);
    POINT last{LONG_MIN, LONG_MIN};
    while (!feed->done.load()) {
        if (!(GetAsyncKeyState(PrimaryButton()) & 0x8000)) {
            feed->released.store(true);
        }
        GUITHREADINFO info{sizeof(info)};
        HWND loop = GetGUIThreadInfo(feed->loopThread, &info) ? info.hwndCapture : nullptr;
        POINT pt;
        if (loop && GetCursorPos(&pt)) {
            POINT client = pt;
            ScreenToClient(loop, &client);
            const LPARAM at = MAKELPARAM(client.x, client.y);
            if (feed->released.load()) {
                PostMessageW(loop, WM_LBUTTONUP, 0, at);
            } else if (pt.x != last.x || pt.y != last.y) {
                PostMessageW(loop, WM_MOUSEMOVE, MK_LBUTTON, at);
                last = pt;
            }
        }
        Sleep(feed->released.load() ? 50 : 10);
    }
    return 0;
}

// Ends a drag of a result when the button is up or Escape is pressed. Lives
// on RunExplorerDrag's stack.
struct ExplorerDragSource : IDropSource {
    ExplorerDragFeed* feed;
    explicit ExplorerDragSource(ExplorerDragFeed* f) : feed(f) {}
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == IID_IUnknown || riid == IID_IDropSource) {
            *ppv = static_cast<IDropSource*>(this);
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escape, DWORD) override {
        if (escape) {
            return DRAGDROP_S_CANCEL;
        }
        if (feed->released.load() || !(GetAsyncKeyState(PrimaryButton()) & 0x8000)) {
            return DRAGDROP_S_DROP;
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD) override { return DRAGDROP_S_USEDEFAULTCURSORS; }
};

// Runs a drag of a result for Start (StartFileDrag) with the shell's own data
// object and drag loop -- the same as dragging the file in File Explorer: the
// same formats for whatever it is dropped on, the same picture under the
// pointer -- but only ever as a copy, so a drag from search can never move a
// file. Then tells Start how it ended -- dropped, or cancelled with Escape
// or by letting go over Start itself -- and on a cancel hands it the
// keyboard back.
static void RunExplorerDrag(HWND hWnd) {
    std::wstring path = std::exchange(g_explorerDragPath, std::wstring());
    if (path.empty()) {
        return;
    }
    HRESULT ole = OleInitialize(nullptr);
    IShellItem* item = nullptr;
    IDataObject* data = nullptr;
    DWORD effect = DROPEFFECT_NONE;
    HRESULT hr = E_FAIL;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) && item &&
        SUCCEEDED(item->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&data))) && data) {
        ExplorerDragFeed feed;
        feed.loopThread = GetCurrentThreadId();
        ExplorerDragSource source(&feed);
        if (HANDLE feeder = CreateThread(nullptr, 0, ExplorerDragFeedThread, &feed, 0, nullptr)) {
            hr = SHDoDragDrop(hWnd, data, &source, DROPEFFECT_COPY, &effect);
            feed.done.store(true);
            WaitForSingleObject(feeder, INFINITE);
            CloseHandle(feeder);
        }
        Wh_Log(L"[Explorer] drag: %ls -> %08X, effect %lu", path.c_str(), static_cast<unsigned>(hr), effect);
    } else {
        Wh_Log(L"[Explorer] drag: could not open %ls", path.c_str());
    }
    if (data) {
        data->Release();
    }
    if (item) {
        item->Release();
    }
    if (SUCCEEDED(ole)) {
        OleUninitialize();
    }
    // Not the effect: a drop into a folder copies in the background and
    // reports none.
    HWND start = g_searchWindowWindow.load();
    if (!start) start = CachedStartWindow();
    POINT pt{};
    GetCursorPos(&pt);
    HWND under = WindowFromPoint(pt);
    const bool overStart = start && under && GetAncestor(under, GA_ROOT) == start;
    const bool dropped = hr == DRAGDROP_S_DROP && !overStart;
    if (start) {
        if (!dropped && !IsCloaked(start) && GetForegroundWindow() == hWnd) {
            SetForegroundWindow(start);
        }
        PostMessageW(start, DragDoneMessage(), dropped ? 1 : 0, 0);
    }
}

// The thread showing an uninstall error, while it does (AcceptPackageRemoval):
// StopExplorerHelperHost closes the box rather than wait for a click.
static std::atomic<DWORD> g_removalBoxThread{0};

// Removes a packaged app for this user, for Uninstall in an app's menu in
// Start (RequestPackageRemoval): here, in a plain desktop process, as package
// management is not open to Start's. Nothing the request says is taken on
// trust -- any process can send it: it is acted on only while Start is open
// and in front, where the user has just confirmed it, when Start itself says
// it is asking for this package (RemovalCheckMessage), and only for a
// package that is not part of Windows, checked again here. Says so when it
// fails, under the package's own name.
static bool AcceptPackageRemoval(std::wstring family) {
    while (!family.empty() && family.back() == L'\0') {
        family.pop_back();
    }
    HWND start = g_searchWindowWindow.load();
    if (!start) start = CachedStartWindow();
    DWORD startPid = 0, foregroundPid = 0;
    if (start) {
        GetWindowThreadProcessId(start, &startPid);
    }
    GetWindowThreadProcessId(GetForegroundWindow(), &foregroundPid);
    const size_t underscore = family.rfind(L'_');
    if (!start || IsCloaked(start) || !startPid || foregroundPid != startPid || family.size() > 128 ||
        underscore == std::wstring::npos || family.size() - underscore - 1 != 13 ||
        family.find_first_not_of(L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-_") !=
            std::wstring::npos) {
        Wh_Log(L"[Explorer] uninstall: refused %ls", family.c_str());
        return false;
    }
    // Start sent this from inside a SendMessage, so it answers this one meanwhile.
    DWORD_PTR confirmed = 0;
    if (!SendMessageTimeoutW(start, RemovalCheckMessage(), 0, static_cast<LPARAM>(std::hash<std::wstring>{}(family)),
                             SMTO_ABORTIFHUNG, 1000, &confirmed) ||
        !confirmed) {
        Wh_Log(L"[Explorer] uninstall: refused %ls, not asked for by Start", family.c_str());
        return false;
    }
    SpawnTrackedLaunch([family] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        std::wstring name = family;
        std::wstring error;
        try {
            namespace appmodel = winrt::Windows::ApplicationModel;
            winrt::Windows::Management::Deployment::PackageManager manager;
            bool found = false;
            for (auto const& package : manager.FindPackagesForUser(L"", family)) {
                if (package.IsFramework() || package.SignatureKind() == appmodel::PackageSignatureKind::System) {
                    continue;  // part of Windows: never removed from here
                }
                found = true;
                try {
                    name = package.DisplayName().c_str();
                } catch (...) {
                }
                auto result = manager.RemovePackageAsync(package.Id().FullName()).get();
                const int32_t code = result.ExtendedErrorCode();
                if (code < 0) {
                    error = result.ErrorText().c_str();
                    if (error.empty()) {
                        error = winrt::hresult_error(code).message().c_str();
                    }
                }
            }
            if (!found) {
                error = L"It is not installed for this account, or it is part of Windows.";
            }
        } catch (winrt::hresult_error const& e) {
            error = e.message().c_str();
        }
        Wh_Log(L"[Explorer] uninstall %ls: %ls", family.c_str(), error.empty() ? L"removed" : error.c_str());
        if (!error.empty() && !g_quit.load()) {
            g_removalBoxThread.store(GetCurrentThreadId());
            MessageBoxW(nullptr, (L"Couldn't uninstall " + name + L".\n\n" + error).c_str(), name.c_str(),
                        MB_OK | MB_ICONWARNING | MB_TOPMOST | MB_SETFOREGROUND);
            g_removalBoxThread.store(0);
        }
        winrt::uninit_apartment();
    });
    return true;
}

// Shell_TrayWnd is created before the taskbar's XAML island. Do not become
// Explorer's first XAML application while its own UI is still being built.
static bool TaskbarXamlReady(HWND tray) {
    HWND bridge = FindWindowExW(tray, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    if (!bridge) return false;
    RECT client{};
    if (!GetClientRect(bridge, &client) || client.right <= client.left || client.bottom <= client.top) return false;
    return FindWindowExW(bridge, nullptr, L"Windows.UI.Input.InputSite.WindowClass", nullptr) ||
           FindWindowExW(tray, nullptr, L"Windows.UI.Core.CoreWindow", nullptr);
}

static void TryInitializeExplorerSearch(HWND helper) {
    if (g_quit.load()) return;
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD taskbarPid = 0;
    if (tray) GetWindowThreadProcessId(tray, &taskbarPid);
    if (!taskbarPid) return;
    if (taskbarPid != GetCurrentProcessId()) {
        KillTimer(helper, kExplorerSearchStartupTimer);
        return;
    }

    // Input capture is native Win32 and can buffer the first query without
    // constructing XAML. The actual UI is created only on a search request.
    UpdateAlternativeShortcut(helper);
    StartSearchWindowInput();
    if (!g_explorerSearchRequested) {
        KillTimer(helper, kExplorerSearchStartupTimer);
        return;
    }
    if (!TaskbarXamlReady(tray)) return;
    KillTimer(helper, kExplorerSearchStartupTimer);
    if (!g_searchWindowWindow.load()) {
        ++g_explorerSearchStartupAttempts;
        if (!SearchWindowInitialize()) {
            if (g_explorerSearchStartupAttempts < 5)
                SetTimer(helper, kExplorerSearchStartupTimer, 1000, nullptr);
            else g_explorerSearchRequested = false;
            return;
        }
    }
    g_explorerSearchRequested = false;
    if (HasBufferedSearchWindowInput()) ApplySearchWindowInput();
    else SearchWindowShow();
}

static void RequestExplorerSearch(HWND helper) {
    g_explorerSearchRequested = true;
    SetTimer(helper, kExplorerSearchStartupTimer, 250, nullptr);
    TryInitializeExplorerSearch(helper);
}

static LRESULT CALLBACK ExplorerHelperWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case kExplorerUpdateShortcut:
        UpdateAlternativeShortcut(hWnd);
        return 0;
    case WM_HOTKEY:
        if (wParam == kAlternativeShortcutId &&
            g_alternativeShortcutActive.load() == PackAlternativeShortcut(LOWORD(lParam)&0xF,HIWORD(lParam)) &&
            g_alternativeShortcutActive.load()) {
            DismissSearchContextMenu();
            // Keep the hotkey's foreground permission during lazy UI creation.
            SetForegroundWindow(hWnd);
            RequestExplorerSearch(hWnd);
        }
        return 0;
    case kExplorerOpenSearch:
        RequestExplorerSearch(hWnd);
        return 0;
    case kSearchWindowInputMessage:
        if (HasBufferedSearchWindowInput()) RequestExplorerSearch(hWnd);
        return 0;
    case kExplorerSearchShown:
        if (IsTaskbarProcess() && !SearchActivationSettling()) ArmSearchAloneCheck(kSearchAloneTicks);
        return 0;
    case kExplorerStartDrag:
        RunExplorerDrag(hWnd);
        return 0;
    case WM_COPYDATA: {
        auto pcds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
        if (pcds && pcds->dwData == kExplorerUninstallMagic && pcds->lpData && pcds->cbData >= sizeof(wchar_t)) {
            return AcceptPackageRemoval(
                       std::wstring(reinterpret_cast<const wchar_t*>(pcds->lpData), pcds->cbData / sizeof(wchar_t)))
                       ? 1
                       : 0;
        }
        if (pcds && pcds->dwData == kExplorerDragMagic && pcds->lpData && pcds->cbData >= sizeof(wchar_t)) {
            std::wstring path(reinterpret_cast<const wchar_t*>(pcds->lpData), pcds->cbData / sizeof(wchar_t));
            while (!path.empty() && path.back() == L'\0') {
                path.pop_back();
            }
            // Only a drag the user is making: this window was given the
            // foreground, which only the foreground process can do, and the
            // primary button is down. And only a local file or folder that
            // exists, as for Properties.
            const int button = PrimaryButton();
            bool local = path.size() >= 3 && path[1] == L':' && !PathIsUNCW(path.c_str());
            if (local) {
                const wchar_t root[] = {path[0], L':', L'\\', 0};
                local = GetDriveTypeW(root) != DRIVE_REMOTE;
            }
            if (GetForegroundWindow() != hWnd || !(GetAsyncKeyState(button) & 0x8000) || !local ||
                GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
                Wh_Log(L"[Explorer] drag: refused for %ls", path.c_str());
                if (HWND start = CachedStartWindow(); start && !IsCloaked(start) && GetForegroundWindow() == hWnd) {
                    SetForegroundWindow(start);  // it gave this window the foreground for the drag
                }
                return 0;
            }
            // Started from the message loop: the drag loop must not run
            // inside Start's SendMessage, which would wait for it.
            g_explorerDragPath = std::move(path);
            PostMessageW(hWnd, kExplorerStartDrag, 0, 0);
            return 1;
        }
        if (pcds && pcds->dwData == kExplorerCopyDataMagic && pcds->lpData && pcds->cbData >= sizeof(wchar_t)) {
            size_t charCount = pcds->cbData / sizeof(wchar_t);
            const wchar_t* pStr = reinterpret_cast<const wchar_t*>(pcds->lpData);
            std::wstring targetPath(pStr, charCount);
            while (!targetPath.empty() && targetPath.back() == L'\0') {
                targetPath.pop_back();
            }
            while (!targetPath.empty() && (targetPath.front() == L' ' || targetPath.front() == L'\t' || targetPath.front() == L'"')) {
                targetPath.erase(targetPath.begin());
            }
            while (!targetPath.empty() && (targetPath.back() == L' ' || targetPath.back() == L'\t' || targetPath.back() == L'"')) {
                targetPath.pop_back();
            }

            if (targetPath.empty()) return 0;

            // Security validation: reject UNC paths and remote network drives
            if (PathIsUNCW(targetPath.c_str()) || targetPath.starts_with(L"\\\\")) {
                Wh_Log(L"[Explorer] Rejected UNC path: %ls", targetPath.c_str());
                return 0;
            }
            if (targetPath.size() >= 2 && targetPath[1] == L':') {
                wchar_t root[4] = { targetPath[0], L':', L'\\', 0 };
                if (GetDriveTypeW(root) == DRIVE_REMOTE) {
                    Wh_Log(L"[Explorer] Rejected remote drive path: %ls", targetPath.c_str());
                    return 0;
                }
            }

            // Security validation: path must exist
            DWORD attr = GetFileAttributesW(targetPath.c_str());
            if (attr == INVALID_FILE_ATTRIBUTES) {
                Wh_Log(L"[Explorer] Rejected non-existent path: %ls", targetPath.c_str());
                return 0;
            }

            Wh_Log(L"[Explorer] Received valid SEPR WM_COPYDATA for: %ls", targetPath.c_str());

            SpawnTrackedLaunch([path = std::move(targetPath)]() {
                HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

                // Attempt 1: SHObjectProperties (Dedicated Win32 Shell Properties API)
                BOOL ok = SHObjectProperties(nullptr, 0x00000002 /* SHOP_FILEPATH */, path.c_str(), nullptr);
                Wh_Log(L"[Explorer] SHObjectProperties returned %d, err=%lu for %ls", ok, GetLastError(), path.c_str());

                // Attempt 2: ShellExecuteExW with PIDL
                if (!ok) {
                    PIDLIST_ABSOLUTE pidl = nullptr;
                    SFGAOF sfgao = 0;
                    if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, &sfgao)) && pidl) {
                        SHELLEXECUTEINFOW sei{};
                        sei.cbSize = sizeof(sei);
                        sei.fMask = SEE_MASK_INVOKEIDLIST;
                        sei.hwnd = nullptr;
                        sei.lpIDList = pidl;
                        sei.lpVerb = L"properties";
                        sei.nShow = SW_SHOWNORMAL;
                        ok = ShellExecuteExW(&sei);
                        Wh_Log(L"[Explorer] ShellExecuteExW PIDL returned %d, err=%lu", ok, GetLastError());
                        CoTaskMemFree(pidl);
                    }
                }

                // Attempt 3: ShellExecuteExW with file path
                if (!ok) {
                    SHELLEXECUTEINFOW sei{};
                    sei.cbSize = sizeof(sei);
                    sei.fMask = SEE_MASK_INVOKEIDLIST;
                    sei.hwnd = nullptr;
                    sei.lpFile = path.c_str();
                    sei.lpVerb = L"properties";
                    sei.nShow = SW_SHOWNORMAL;
                    ok = ShellExecuteExW(&sei);
                    Wh_Log(L"[Explorer] ShellExecuteExW string returned %d, err=%lu", ok, GetLastError());
                }

                if (SUCCEEDED(hr)) {
                    CoUninitialize();
                }
            });

            return 1;
        }
        break;
    }
    case WM_TIMER:
        if (wParam == kExplorerSearchStartupTimer) {
            TryInitializeExplorerSearch(hWnd);
            return 0;
        }
        if (wParam == kSearchAloneTimerId) {
            if (CheckSearchShownWithoutStart()) {
                KillTimer(hWnd, kSearchAloneTimerId);
                g_searchAloneArmed = false;
            }
            return 0;
        }
        break;
    case WM_SYSCOMMAND:
        // The helper can hold activation during a drag or lazy search opening.
        // Alt+F4 must not destroy this relay window.
        if ((wParam & 0xFFF0) == SC_CLOSE) {
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        ReleaseAlternativeShortcut(hWnd);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

static DWORD WINAPI ExplorerHelperThreadProc(LPVOID) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = ExplorerHelperWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = kExplorerHelperClassName;
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW,
        kExplorerHelperClassName,
        kExplorerHelperWindowName,
        WS_POPUP,
        0, 0, 0, 0,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    if (hWnd) {
        ChangeWindowMessageFilterEx(hWnd, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
        g_hExplorerHelperWnd = hWnd;
        Wh_Log(L"[Explorer] Helper host window created: %p", hWnd);
        // Out of context: delivered to this thread's message loop, never run
        // inside the thread that changed the foreground.
        g_searchForegroundHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            ShellWindowEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        g_startCloakedHook = SetWinEventHook(
            EVENT_OBJECT_CLOAKED, EVENT_OBJECT_CLOAKED, nullptr,
            ShellWindowEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    } else {
        Wh_Log(L"[Explorer] Failed to create helper host window, err=%lu", GetLastError());
    }

    if (hWnd) {
        g_explorerSearchStartupAttempts = 0;
        g_explorerSearchRequested = false;
        SearchWindowTrace(L"search startup armed; XAML deferred until first search", S_OK);
        SetTimer(hWnd, kExplorerSearchStartupTimer, 250, nullptr);
    }

    if (g_hExplorerHelperReadyEvent) {
        SetEvent(g_hExplorerHelperReadyEvent);
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (SearchWindowPreTranslate(&msg)) continue;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    ReleaseAlternativeShortcut(hWnd);
    SearchWindowTeardown();
    StopSearchWindowInput();  // also when search was never opened

    if (g_searchForegroundHook) {
        UnhookWinEvent(g_searchForegroundHook);
        g_searchForegroundHook = nullptr;
    }
    if (g_startCloakedHook) {
        UnhookWinEvent(g_startCloakedHook);
        g_startCloakedHook = nullptr;
    }
    g_searchAloneArmed = false;

    if (hWnd && IsWindow(hWnd)) {
        DestroyWindow(hWnd);
    }
    g_hExplorerHelperWnd = nullptr;
    UnregisterClassW(kExplorerHelperClassName, wc.hInstance);

    if (SUCCEEDED(hr)) {
        CoUninitialize();
    }
    return 0;
}

// Every explorer.exe hosts one: File Explorer windows can run in processes of
// their own, and which process loads this mod first is not fixed, so "only the
// first" could leave the taskbar's process without one. Extra hosts are
// harmless: the properties relay sends to whichever FindWindow returns, and
// only the taskbar's process acts on Start and Search (IsTaskbarProcess).
static void StartExplorerHelperHost() {
    if (g_hExplorerHelperThread) return;

    g_hExplorerHelperReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hExplorerHelperThread = CreateThread(nullptr, 0, ExplorerHelperThreadProc, nullptr, 0, &g_explorerHelperThreadId);
    if (g_hExplorerHelperThread && g_hExplorerHelperReadyEvent) {
        WaitForSingleObject(g_hExplorerHelperReadyEvent, 3000);
    }
    if (g_hExplorerHelperReadyEvent) {
        CloseHandle(g_hExplorerHelperReadyEvent);
        g_hExplorerHelperReadyEvent = nullptr;
    }
}

static void StopExplorerHelperHost() {
    if (g_hExplorerHelperWnd && IsWindow(g_hExplorerHelperWnd)) {
        PostMessageW(g_hExplorerHelperWnd, WM_CLOSE, 0, 0);
    }
    if (g_explorerHelperThreadId) {
        PostThreadMessageW(g_explorerHelperThreadId, WM_QUIT, 0, 0);
    }
    if (g_hExplorerHelperThread) {
        WaitForSingleObject(g_hExplorerHelperThread, INFINITE);
        CloseHandle(g_hExplorerHelperThread);
        g_hExplorerHelperThread = nullptr;
        g_explorerHelperThreadId = 0;
    }
    // An uninstall error still up would hold the unload until clicked.
    for (int i = 0; i < 50 && g_removalBoxThread.load(); ++i) {
        EnumThreadWindows(g_removalBoxThread.load(), [](HWND hwnd, LPARAM) -> BOOL {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            return TRUE;
        }, 0);
        Sleep(100);
    }
    WaitForTrackedLaunches();
}

void InitExplorer() {
    LoadSettings();
    Wh_Log(L"=== start-everything: explorer.exe settings ready ===");
}

// ===========================================================================
// Domain: SearchHost.exe (kept invisible, no WebView2)
// ===========================================================================
//
// Windows hands the foreground to SearchHost whenever Start or Search opens:
// Explorer activates SearchHost's CoreWindow first (~16ms after the Win key or
// a Start-button click) and Start after that. Earlier versions fought this --
// every SearchHost window subclassed, its activation and focus swallowed,
// every show forced to a hide, every foreground call redirected -- and the
// fight is what made the focus workarounds elsewhere necessary.
//
// Now SearchHost does what the shell asks of it, with three differences:
//
//   1. Its CoreWindow is drawn at zero alpha (SetNeutralized). The shell
//      still shows, hides and activates it exactly as before; it just never
//      appears, and clicks pass through it to Start.
//   2. Native search activation is relayed to the independent search window,
//      with foreground permission granted to the taskbar's Explorer process.
//   3. WebView2 and the indexer are not launched: that is the disconnect.

struct HookReentryGuard {
    bool& flag;
    explicit HookReentryGuard(bool& f) : flag(f) { flag = true; }
    ~HookReentryGuard() { flag = false; }
};

using CreateProcessW_t = decltype(&CreateProcessW);
static CreateProcessW_t pOriginalCreateProcessW = nullptr;

static bool IsWebViewProcess(LPCWSTR text) {
    if (!text) return false;
    std::wstring lower(text);
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(towlower(c)); });
    return (lower.find(L"msedgewebview2.exe") != std::wstring::npos) ||
           (lower.find(L"embeddedbrowserwebview") != std::wstring::npos) ||
           (lower.find(L"searchindexer.exe") != std::wstring::npos);
}

static BOOL WINAPI Hook_SearchHost_CreateProcessW(
    LPCWSTR applicationName, LPWSTR commandLine,
    LPSECURITY_ATTRIBUTES processAttributes,
    LPSECURITY_ATTRIBUTES threadAttributes,
    BOOL inheritHandles, DWORD creationFlags,
    LPVOID environment, LPCWSTR currentDirectory,
    LPSTARTUPINFOW startupInfo,
    LPPROCESS_INFORMATION processInformation) {
    thread_local bool inHook = false;
    if (inHook) {
        return pOriginalCreateProcessW(applicationName, commandLine,
                                      processAttributes, threadAttributes,
                                      inheritHandles, creationFlags, environment,
                                      currentDirectory, startupInfo, processInformation);
    }
    HookReentryGuard guard(inHook);

    if (IsWebViewProcess(applicationName) || IsWebViewProcess(commandLine)) {
        Wh_Log(L"[SearchHost] Blocked WebView2 launch: app=%ls, cmd=%ls",
            applicationName ? applicationName : L"(null)",
            commandLine ? commandLine : L"(null)");
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    return pOriginalCreateProcessW(applicationName, commandLine,
                                  processAttributes, threadAttributes,
                                  inheritHandles, creationFlags, environment,
                                  currentDirectory, startupInfo, processInformation);
}

// Neutralizes SearchHost's CoreWindow (wParam TRUE) or restores it (FALSE).
// Sent by the watchdog and by uninit; applied on the window's own thread.
static UINT SearchNeutralizeMessage() {
    static const UINT message = RegisterWindowMessageW(L"StartEverything_SearchNeutralize");
    return message;
}

// What SetNeutralized changed, kept on each window so every one is restored
// to exactly what it had: whether it was layered already and, if so, its
// layered attributes (color key, alpha, LWA_ flags), packed into the value.
constexpr wchar_t kOriginalLayeringProp[] = L"StartEverything_OriginalLayering";
constexpr ULONG_PTR kLayeringRecorded = ULONG_PTR{1} << 41;  // never 0, so "no prop" is unambiguous
constexpr ULONG_PTR kWasLayered = ULONG_PTR{1} << 40;

// Takes a SearchHost CoreWindow out of the picture (on) or puts it back (off).
// Runs on the window's own thread.
static void SetNeutralized(HWND hWnd, bool on) {
    // DWM refuses an app cloak (DWMWA_CLOAK) on a CoreWindow, and a window
    // region does not clip its composition content. Layered alpha 0 does:
    // nothing is drawn, and clicks pass through. The shell still shows, hides
    // and activates the window through its own cloak exactly as before.
    LONG ex = GetWindowLongW(hWnd, GWL_EXSTYLE);
    auto saved = reinterpret_cast<ULONG_PTR>(GetPropW(hWnd, kOriginalLayeringProp));
    if (on) {
        if (saved) {
            return;  // already done
        }
        ULONG_PTR record = kLayeringRecorded;
        if (ex & WS_EX_LAYERED) {
            COLORREF key = 0;
            BYTE alpha = 0;
            DWORD flags = 0;
            if (!GetLayeredWindowAttributes(hWnd, &key, &alpha, &flags)) {
                // Layered through UpdateLayeredWindow: there would be nothing
                // to restore it to, so it is left alone.
                Wh_Log(L"[SearchHost] CoreWindow %p uses UpdateLayeredWindow; left visible", hWnd);
                return;
            }
            record |= kWasLayered | (static_cast<ULONG_PTR>(key & 0xFFFFFF) << 16) |
                      (static_cast<ULONG_PTR>(flags & 0xFF) << 8) | alpha;
        } else {
            SetWindowLongW(hWnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        }
        SetPropW(hWnd, kOriginalLayeringProp, reinterpret_cast<HANDLE>(record));
        SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    } else {
        if (!saved) {
            return;  // never changed
        }
        RemovePropW(hWnd, kOriginalLayeringProp);
        if (saved & kWasLayered) {
            SetLayeredWindowAttributes(hWnd, static_cast<COLORREF>((saved >> 16) & 0xFFFFFF),
                                       static_cast<BYTE>(saved & 0xFF), static_cast<DWORD>((saved >> 8) & 0xFF));
        } else {
            SetWindowLongW(hWnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        }
    }
    Wh_Log(L"[SearchHost] %ls CoreWindow %p", on ? L"hid" : L"restored", hWnd);
}

static LRESULT CALLBACK SearchHostSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg == SearchWindowForegroundMessage()) {
        HWND host = FindWindowW(kSearchWindowClass, nullptr);
        DWORD pid = 0, shellPid = 0;
        GetWindowThreadProcessId(host, &pid);
        GetWindowThreadProcessId(FindWindowW(L"Shell_TrayWnd", nullptr), &shellPid);
        return pid && pid == shellPid && AllowSetForegroundWindow(pid);
    }
    if ((uMsg == WM_ACTIVATE && LOWORD(wParam) != WA_INACTIVE) ||
        (uMsg == WM_WINDOWPOSCHANGED && GetForegroundWindow() == hWnd && !IsCloaked(hWnd))) {
        if (HWND helper = FindWindowW(kExplorerHelperClassName, kExplorerHelperWindowName))
            PostMessageW(helper, kExplorerSearchShown, 0, 0);
    }
    UINT neutralizeMessage = SearchNeutralizeMessage();
    if (uMsg == neutralizeMessage && neutralizeMessage) {
        SetNeutralized(hWnd, wParam != 0);
        return TRUE;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static void SetSearchWindowNeutralized(HWND hwnd, BOOL on) {
    DWORD_PTR done = 0;
    SendMessageTimeoutW(hwnd, SearchNeutralizeMessage(), on, 0, SMTO_ABORTIFHUNG, 1000, &done);
}

void InitSearchHost() {
    Wh_Log(L"=== start-everything: initializing SearchHost disconnect ===");
    // kernel32's CreateProcessW only forwards to kernelbase's, and a caller
    // importing through an API set goes to kernelbase directly.
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto target = kernelBase
        ? reinterpret_cast<CreateProcessW_t>(GetProcAddress(kernelBase, "CreateProcessW"))
        : nullptr;
    WindhawkUtils::SetFunctionHook(target ? target : CreateProcessW, Hook_SearchHost_CreateProcessW,
                                   &pOriginalCreateProcessW);
}

// SearchHost's CoreWindow can appear after this mod loads, and the shell can
// recreate it, so it is looked for for as long as the mod runs: every 500ms
// for the first minute, then every 5s. Only CoreWindows are touched; the
// process's other top-level windows (IME windows, a zero-size IE host) never
// show.
[[clang::no_destroy]] static std::optional<std::thread> g_searchHostWatchdog;
static std::vector<HWND> g_searchWindows;  // watchdog thread, then uninit after the join

static void StartSearchHostWatchdog() {
    g_searchHostWatchdog.emplace([] {
        for (int i = 0; !g_quit.load(); ++i) {
            // A destroyed window's handle can be reused by a new one.
            std::erase_if(g_searchWindows, [](HWND hwnd) { return !IsWindow(hwnd); });
            for (HWND hwnd = FindWindowExW(nullptr, nullptr, L"Windows.UI.Core.CoreWindow", nullptr); hwnd;
                 hwnd = FindWindowExW(nullptr, hwnd, L"Windows.UI.Core.CoreWindow", nullptr)) {
                DWORD pid = 0;
                GetWindowThreadProcessId(hwnd, &pid);
                if (pid != GetCurrentProcessId() ||
                    std::find(g_searchWindows.begin(), g_searchWindows.end(), hwnd) != g_searchWindows.end()) {
                    continue;
                }
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, SearchHostSubclassProc, 0)) {
                    SetSearchWindowNeutralized(hwnd, TRUE);
                    g_searchWindows.push_back(hwnd);
                }
            }
            int slices = i < 120 ? 10 : 100;  // 50ms each, so unload never waits long
            for (int s = 0; s < slices && !g_quit.load(); ++s) {
                Sleep(50);
            }
        }
    });
}

// ===========================================================================
// Domain: StartMenuExperienceHost.exe
// ===========================================================================

namespace {

struct WebShortcut {
    std::wstring prefix;
    std::wstring name;
    std::wstring url;
};

enum class CustomCommandShell { Cmd, PowerShell, PowerShell7, Custom };

CustomCommandShell ParseCustomCommandShell(PCWSTR value) {
    if(value) {
        if(_wcsicmp(value,L"powershell")==0) return CustomCommandShell::PowerShell;
        if(_wcsicmp(value,L"pwsh")==0) return CustomCommandShell::PowerShell7;
        if(_wcsicmp(value,L"custom")==0) return CustomCommandShell::Custom;
    }
    return CustomCommandShell::Cmd;
}

struct CustomCommand {
    std::wstring name;
    std::wstring icon;
    std::wstring command;
    std::wstring shellExecutable;
    std::wstring shellArguments = L"-c {command}";
    CustomCommandShell shell = CustomCommandShell::Cmd;
    bool hideTerminal = false;
};

struct CustomConversion {
    std::wstring fromUnit;
    std::wstring toUnit;
    std::wstring formula;
    std::wstring category;
    bool reciprocal = false;  // also back, from toUnit (calc::Invert)
};

// The text size setting (textScale), for Fs.
std::atomic<int> g_textScale{100};

struct Settings {
    std::wstring defaultSearchUrl = L"https://duckduckgo.com/?q={q}";
    std::vector<WebShortcut> webShortcuts;
    std::vector<CustomCommand> customCommands;
    std::vector<CustomConversion> unitConversions;
    std::vector<std::wstring> excludedPaths;
    int maxAppResults = 6;
    int maxFileResults = 12;
    int searchDebounceMs = 0;
    int menuWidth = 660;
    int menuHeight = 660;
    std::wstring textColor = L"auto";
    int textScale = 100;  // percent
    bool showKeyHints = true;
    bool filterNoisyPaths = true;
    bool learnFavorites = true;
    bool hideFilesPanel = false;
    bool useCustomMenuSize = false;
    bool filePreview = true;
    bool animatePreview = true;
};

Settings g_settings;
std::mutex g_settingsMutex;
std::atomic<uint64_t> g_settingsRevision{0};

bool FilesPanelVisible() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return !g_settings.hideFilesPanel;
}
// Set when Learn Favorite Apps is turned off: the search thread drops its counts.
std::atomic<bool> g_forgetUsage{false};

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    auto shortcutModifiers=WindhawkUtils::StringSetting::make(L"search.shortcutModifiers");
    auto shortcutKey=WindhawkUtils::StringSetting::make(L"search.shortcutKey");
    auto modifiers=ParseAlternativeShortcutModifiers(shortcutModifiers.get() ? shortcutModifiers.get() : L"");
    const UINT key=ParseAlternativeShortcutKey(shortcutKey.get() ? shortcutKey.get() : L"");
    const bool shortcutEnabled=Wh_GetIntSetting(L"search.enableAlternativeShortcut")!=0;
    g_alternativeShortcutDesired=shortcutEnabled && modifiers && key ? PackAlternativeShortcut(*modifiers,key) : 0;
    if (shortcutEnabled && (!modifiers || !key))
        Wh_Log(L"Alternative search shortcut has an invalid modifier or key setting");

    auto defUrl = WindhawkUtils::StringSetting::make(L"web.defaultSearchUrl");
    if (defUrl.get() && *defUrl.get()) {
        g_settings.defaultSearchUrl = defUrl.get();
    } else {
        g_settings.defaultSearchUrl = L"https://duckduckgo.com/?q={q}";
    }

    g_settings.customCommands.clear();
    for (int i = 0; i < 100; ++i) {
        auto name = WindhawkUtils::StringSetting::make(L"customCommands[%d].name", i);
        auto icon = WindhawkUtils::StringSetting::make(L"customCommands[%d].icon", i);
        auto command = WindhawkUtils::StringSetting::make(L"customCommands[%d].command", i);
        auto shell = WindhawkUtils::StringSetting::make(L"customCommands[%d].shell", i);
        auto shellExecutable = WindhawkUtils::StringSetting::make(L"customCommands[%d].shellExecutable", i);
        auto shellArguments = WindhawkUtils::StringSetting::make(L"customCommands[%d].shellArguments", i);
        CustomCommand entry;
        if (name.get()) entry.name = tools::Trim(name.get());
        if (icon.get()) entry.icon = tools::Trim(icon.get());
        if (command.get()) entry.command = tools::Trim(command.get());
        entry.shell = ParseCustomCommandShell(shell.get());
        if(shellExecutable.get()) entry.shellExecutable=tools::Trim(shellExecutable.get());
        if(shellArguments.get() && *shellArguments.get()) entry.shellArguments=shellArguments.get();
        entry.hideTerminal = Wh_GetIntSetting(L"customCommands[%d].hideTerminal", i) != 0;
        if (!entry.name.empty() && !entry.command.empty()) {
            g_settings.customCommands.push_back(std::move(entry));
        }
    }

    g_settings.webShortcuts.clear();
    for (int i = 0;; ++i) {
        auto pfx = WindhawkUtils::StringSetting::make(L"web.webShortcuts[%d].prefix", i);
        if (!pfx.get() || !*pfx.get()) break;
        auto name = WindhawkUtils::StringSetting::make(L"web.webShortcuts[%d].name", i);
        auto url = WindhawkUtils::StringSetting::make(L"web.webShortcuts[%d].url", i);

        WebShortcut s;
        s.prefix = pfx.get();
        for (auto& c : s.prefix) c = static_cast<wchar_t>(towlower(c));
        s.name = (name.get() && *name.get()) ? name.get() : L"Web";
        s.url = (url.get() && *url.get()) ? url.get() : L"";
        if (!s.prefix.empty() && !s.url.empty()) {
            g_settings.webShortcuts.push_back(std::move(s));
        }
    }
    if (g_settings.webShortcuts.empty()) {
        g_settings.webShortcuts.push_back({L"yt", L"YouTube", L"https://www.youtube.com/results?search_query={q}"});
        g_settings.webShortcuts.push_back({L"gh", L"GitHub", L"https://github.com/search?q={q}"});
        g_settings.webShortcuts.push_back({L"w", L"Wikipedia", L"https://en.wikipedia.org/wiki/Special:Search?search={q}"});
        g_settings.webShortcuts.push_back({L"r", L"Reddit", L"https://www.reddit.com/search/?q={q}"});
    }

    g_settings.unitConversions.clear();
    for (int i = 0;; ++i) {
        auto fromU = WindhawkUtils::StringSetting::make(L"unitConversions[%d].fromUnit", i);
        if (!fromU.get() || !*fromU.get()) break;
        auto toU = WindhawkUtils::StringSetting::make(L"unitConversions[%d].toUnit", i);
        auto formula = WindhawkUtils::StringSetting::make(L"unitConversions[%d].formula", i);
        auto cat = WindhawkUtils::StringSetting::make(L"unitConversions[%d].category", i);

        CustomConversion c;
        c.fromUnit = tools::Trim(fromU.get());  // shown as written; matched by NormalizeUnit
        c.toUnit = (toU.get() && *toU.get()) ? tools::Trim(toU.get()) : L"";
        c.formula = (formula.get() && *formula.get()) ? tools::Trim(formula.get()) : L"";
        c.category = (cat.get() && *cat.get()) ? tools::Trim(cat.get()) : L"Conversion";
        c.reciprocal = Wh_GetIntSetting(L"unitConversions[%d].reciprocal", i) != 0;

        if (!c.fromUnit.empty() && !c.toUnit.empty() && !c.formula.empty()) {
            g_settings.unitConversions.push_back(std::move(c));
        }
    }

    g_settings.filterNoisyPaths = Wh_GetIntSetting(L"files.filterNoisyPaths") != 0;

    g_settings.excludedPaths.clear();
    for (int i = 0;; ++i) {
        auto val = WindhawkUtils::StringSetting::make(L"files.excludedPaths[%d]", i);
        if (!val.get() || !*val.get()) break;
        std::wstring s = tools::ToLower(tools::Trim(val.get()));
        for (auto& ch : s) {
            if (ch == L'/') ch = L'\\';
        }
        if (!s.empty()) {
            g_settings.excludedPaths.push_back(std::move(s));
        }
    }
    if (g_settings.excludedPaths.empty() && g_settings.filterNoisyPaths) {
        static const wchar_t* kDefaults[] = {
            L"\\node_modules\\",
            L"\\.git\\",
            L"\\.gradle\\",
            L"\\appdata\\local\\temp\\",
            L"\\appdata\\local\\packages\\",
            L"\\__pycache__\\",
            L"\\.venv\\",
            L"\\site-packages\\",
            L"\\.cache\\",
            L"\\build\\intermediates\\",
            L"\\obj\\debug\\",
            L"\\obj\\release\\",
            L"\\windows\\winsxs\\",
            L"\\windows\\servicing\\",
        };
        for (const wchar_t* d : kDefaults) {
            g_settings.excludedPaths.push_back(d);
        }
    }

    int maxApps = Wh_GetIntSetting(L"search.maxAppResults");
    g_settings.maxAppResults = (maxApps > 0) ? std::clamp(maxApps, 1, 30) : 6;

    int maxFiles = Wh_GetIntSetting(L"files.maxFileResults");
    g_settings.maxFileResults = (maxFiles > 0) ? std::clamp(maxFiles, 1, 50) : 12;

    g_settings.searchDebounceMs = std::clamp(Wh_GetIntSetting(L"search.searchDebounceMs"), 0, 1000);

    g_settings.useCustomMenuSize = Wh_GetIntSetting(L"appearance.useCustomMenuSize") != 0;
    const int menuWidth = Wh_GetIntSetting(L"appearance.menuWidth");
    const int menuHeight = Wh_GetIntSetting(L"appearance.menuHeight");
    g_settings.menuWidth = menuWidth > 0 ? std::clamp(menuWidth, 320, 3840) : 660;
    g_settings.menuHeight = menuHeight > 0 ? std::clamp(menuHeight, 240, 2160) : 660;
    const int textScale = Wh_GetIntSetting(L"appearance.textScale");
    g_settings.textScale = textScale ? std::clamp(textScale, 80, 150) : 100;  // 0: not set yet
    g_textScale.store(g_settings.textScale);
    auto textColor = WindhawkUtils::StringSetting::make(L"appearance.textColor");
    g_settings.textColor = textColor.get() ? tools::Trim(textColor.get()) : L"auto";

    g_settings.showKeyHints = Wh_GetIntSetting(L"appearance.showKeyHints") != 0;

    // Off forgets: the stored counts now, the search thread's copy on its next
    // turn (g_forgetUsage).
    g_settings.learnFavorites = Wh_GetIntSetting(L"search.learnFavorites") != 0;
    g_settings.hideFilesPanel = Wh_GetIntSetting(L"files.hideFilesPanel") != 0;
    g_settings.filePreview = Wh_GetIntSetting(L"files.filePreview") != 0;
    g_settings.animatePreview = Wh_GetIntSetting(L"files.animatePreview") != 0;
    if (!g_settings.learnFavorites) {
        Wh_DeleteValue(apps::kUsageValueName);
        g_forgetUsage.store(true);
    }

    g_settingsRevision.fetch_add(1);
    Wh_Log(L"=== settings: defSearch=%ls shortcuts=%zu maxApps=%d maxFiles=%d debounce=%d hints=%d filterNoise=%d excluded=%zu ===",
        g_settings.defaultSearchUrl.c_str(), g_settings.webShortcuts.size(),
        g_settings.maxAppResults, g_settings.maxFileResults,
        g_settings.searchDebounceMs,
        g_settings.showKeyHints ? 1 : 0,
        g_settings.filterNoisyPaths ? 1 : 0,
        g_settings.excludedPaths.size());
}

wux::DependencyObject FindDescendantByName(wux::DependencyObject const& root,
                                           std::wstring_view name,
                                           int maxDepth) {
    if (maxDepth < 0) {
        return nullptr;
    }
    if (auto fe = root.try_as<wux::FrameworkElement>()) {
        if (std::wstring_view{fe.Name()} == name) {
            return root;
        }
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = wuxm::VisualTreeHelper::GetChild(root, i);
        if (auto found = FindDescendantByName(child, name, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

// Search controls are created only in Explorer's independent XAML window.
[[clang::no_destroy]] wuxc::TextBox g_ourBox{nullptr};
[[clang::no_destroy]] wuxc::Grid g_resultsHost{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_resultsList{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_appsList{nullptr};
[[clang::no_destroy]] wuxc::ScrollViewer g_appsScroll{nullptr}, g_filesScroll{nullptr};
struct AppCardUI {
    int appIndex = -1;
    std::wstring title;
    std::wstring openPath;
    wuxc::Button button{nullptr};
    bool canRunAsAdmin = true;
    bool isSetting = false;
    bool isFile = false;
    std::wstring commandLine;
    uint64_t commandRevision = 0;
};

[[clang::no_destroy]] static std::optional<std::vector<wuxc::Button>> g_appButtonsOpt;
[[clang::no_destroy]] static std::optional<std::vector<AppCardUI>> g_activeAppsOpt;
// The Files column's rows, in order, for keyboard selection there.
[[clang::no_destroy]] static std::optional<std::vector<wuxc::Button>> g_fileButtonsOpt;

// Every handler the mod puts on Start's XAML objects, so the teardown can take
// them all off while the mod is still loaded. Taking an element out of the tree
// doesn't free it: XAML can hold a dropped row until its next frame, which a
// closed Start doesn't draw until it opens again, and freeing a handler whose
// code went away with the mod crashes Start.
struct XamlHandler {
    winrt::weak_ref<wf::IInspectable> source;
    std::shared_ptr<void> revoker;  // a C++/WinRT revoker; revokes when destroyed
};
[[clang::no_destroy]] std::optional<std::vector<XamlHandler>> g_xamlHandlers{std::in_place};
// Rows are rebuilt on every keystroke; handlers on objects XAML has since
// freed are forgotten whenever the list doubles.
size_t g_xamlHandlersPruneAt = 512;

template <typename Revoker>
void KeepHandler(wf::IInspectable const& source, Revoker revoker) {
    if (!g_xamlHandlers) {
        return;  // torn down: the revoker revokes as it goes out of scope
    }
    if (g_xamlHandlers->size() >= g_xamlHandlersPruneAt) {
        std::erase_if(*g_xamlHandlers, [](XamlHandler const& h) { return !h.source.get(); });
        g_xamlHandlersPruneAt = std::max<size_t>(512, g_xamlHandlers->size() * 2);
    }
    g_xamlHandlers->push_back({winrt::make_weak(source), std::make_shared<Revoker>(std::move(revoker))});
}

[[clang::no_destroy]] wuxc::Border g_appsHeaderHolder{nullptr};
[[clang::no_destroy]] wuxc::Border g_filesHeaderHolder{nullptr};
[[clang::no_destroy]] wuxc::Border g_searchBarBorder{nullptr};
[[clang::no_destroy]] wuxc::Border g_divider{nullptr};
[[clang::no_destroy]] wuxc::Border g_footerBorder{nullptr};
std::atomic<bool> g_isOverlayVisible{false};
std::atomic<bool> g_isHiding{false};

void UpdateSearchResultsLayout(bool clearPreview = false);
void HidePreview();

// A font size of the mod's own UI at the text size setting (textScale).
double Fs(double size) {
    return size * g_textScale.load(std::memory_order_relaxed) / 100.0;
}

// Sets a font size of the mod's UI, keeping the size it is at 100% in the
// element's Tag, for RescaleFonts.
template <typename Element>
void ScaleFont(Element const& element, double size) {
    element.FontSize(Fs(size));
    element.Tag(winrt::box_value(size));
}

// Puts a new text size on what is already built (ScaleFont).
void RescaleFonts(wux::DependencyObject const& root) {
    if (!root) {
        return;
    }
    if (auto element = root.try_as<wux::FrameworkElement>()) {
        if (auto base = element.Tag().try_as<wf::IReference<double>>()) {
            const double size = Fs(base.Value());
            if (auto text = root.try_as<wuxc::TextBlock>()) {
                text.FontSize(size);
            } else if (auto icon = root.try_as<wuxc::FontIcon>()) {
                icon.FontSize(size);
            } else if (auto control = root.try_as<wuxc::Control>()) {
                control.FontSize(size);
            }
        }
    }
    const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int32_t i = 0; i < count; ++i) {
        RescaleFonts(wuxm::VisualTreeHelper::GetChild(root, i));
    }
}

void RequestRender();
void TeardownSearchUi();
inline UINT GetTeardownMessage() {
    static UINT s_msg = RegisterWindowMessageW(L"Windhawk_StartMenuTeardown_start-everything");
    return s_msg;
}
// The subclass's answer to the teardown message. A window that isn't
// subclassed answers 0 through DefWindowProc, which must not count as done.
constexpr LRESULT kTeardownDone = 0x5445;

[[clang::no_destroy]] wuxc::TextBox::TextChanged_revoker g_ourBoxChanged;
[[clang::no_destroy]] wux::UIElement::LostFocus_revoker g_ourBoxLost;

inline wuxm::SolidColorBrush MakeBrush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return wuxm::SolidColorBrush{winrt::Windows::UI::ColorHelper::FromArgb(a, r, g, b)};
}

// Created with the search UI, never during Explorer startup. Existing labels
// share these brushes so an appearance change also updates an open preview.
[[clang::no_destroy]] wuxm::SolidColorBrush g_searchTextBrush{nullptr};
[[clang::no_destroy]] wuxm::SolidColorBrush g_searchBoxTextBrush{nullptr};
[[clang::no_destroy]] wuxm::SolidColorBrush g_searchPlaceholderBrush{nullptr};
[[clang::no_destroy]] wuxm::Brush g_resolvedSearchTextBrush{nullptr};

wuxm::Brush SearchTextBrush() {
    if (g_resolvedSearchTextBrush) return g_resolvedSearchTextBrush;
    if (!g_searchTextBrush) g_searchTextBrush = MakeBrush(255,255,255,255);
    g_resolvedSearchTextBrush = g_searchTextBrush;
    return g_resolvedSearchTextBrush;
}
wuxc::TextBlock SearchTextBlock() {
    wuxc::TextBlock text;
    text.Foreground(SearchTextBrush());
    return text;
}

struct SearchIconAppearance {
    winrt::weak_ref<wuxc::FontIcon> icon;
    wuxm::Brush applied{nullptr};
};
[[clang::no_destroy]] std::optional<std::vector<SearchIconAppearance>> g_searchIcons;
size_t g_searchIconSweepAt = 256;
void PruneSearchIcons() {
    if (!g_searchIcons) return;
    std::erase_if(*g_searchIcons,[](auto const& item) { return !item.icon.get(); });
    // Large live result lists can exceed 256 icons. Leave room for another
    // batch instead of scanning every live weak reference for each new icon.
    g_searchIconSweepAt = g_searchIcons->size() + 256;
}
wuxc::FontIcon SearchFontIcon() {
    wuxc::FontIcon icon;
    auto brush=SearchTextBrush();
    icon.Foreground(brush);
    if(!g_searchIcons) g_searchIcons.emplace();
    if(g_searchIcons->size()>=g_searchIconSweepAt) PruneSearchIcons();
    g_searchIcons->push_back({winrt::make_weak(icon),brush});
    return icon;
}
void RefreshSearchIcons(wuxm::Brush const& brush) {
    if(!g_searchIcons) return;
    for(auto& item:*g_searchIcons) {
        auto icon=item.icon.get();
        if(!icon) continue;
        // Utility glyphs can replace the default with their own accent brush.
        // Update only icons still using the foreground assigned by this helper.
        if(icon.Foreground()==item.applied) {
            icon.Foreground(brush);
            item.applied=brush;
        }
    }
    PruneSearchIcons();
}

static std::atomic<int> g_cachedLightTheme{-1};

inline void RefreshThemeCache() {
    DWORD val = 0;
    DWORD sz = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &val, &sz) == ERROR_SUCCESS) {
        g_cachedLightTheme.store(val != 0 ? 1 : 0);
        return;
    }
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &val, &sz) == ERROR_SUCCESS) {
        g_cachedLightTheme.store(val != 0 ? 1 : 0);
        return;
    }
    try {
        wux::FrameworkElement target = g_resultsHost;
        if (target) {
            auto theme = target.ActualTheme();
            if (theme == wux::ElementTheme::Light) {
                g_cachedLightTheme.store(1);
                return;
            }
            if (theme == wux::ElementTheme::Dark) {
                g_cachedLightTheme.store(0);
                return;
            }
        }
    } catch (...) {}
    g_cachedLightTheme.store(0);
}

inline bool IsLightTheme() {
    int cached = g_cachedLightTheme.load();
    if (cached >= 0) {
        return cached != 0;
    }
    RefreshThemeCache();
    return g_cachedLightTheme.load() != 0;
}

// Refresh the independent window without replaying the old in-Start overlay.
void UpdateSearchResultsLayout(bool clearPreview) {
    if (clearPreview) HidePreview();
    if (!g_resultsHost) return;
    HWND host = g_searchWindowWindow.load();
    if (host && (clearPreview || (IsWindowVisible(host) && !g_isHiding.load())))
        SearchWindowLayout();
}

// Read and written by the XAML, search and launch threads.
static std::atomic<HWND> g_hCoreWindow{nullptr};

HWND GetSearchWindow() {
    return g_searchWindowWindow.load();
}

std::atomic<bool> g_appIndexNeedsRefresh{false};
std::atomic<ULONGLONG> g_lastAppIndexRebuildTick{0};

void RequestAppIndexRefresh();

// The Store app Start is asking Explorer to remove, while it asks
// (RequestPackageRemoval): what it answers RemovalCheckMessage by. XAML thread.
static std::wstring g_removalPending;

// A result is being dragged, by Explorer's helper (StartFileDrag), which holds
// the foreground until the drop: Start losing it then is not Start closing.
static std::atomic<bool> g_dragging{false};
// An Enter pressed before the results for the text were in, run when they
// arrive (RenderResults); dropped once Start closes, or if they take longer
// than this.
static bool g_enterWaiting = false;
static bool g_enterWaitingCtrl = false;
static ULONGLONG g_enterWaitingTick = 0;
constexpr ULONGLONG kEnterWaitMs = 3000;

void DismissSearch() {
    SearchWindowHide();
}

// Use Explorer's shell activation for ordinary launches, preserving app entries,
// shortcuts and their arguments. Elevated launches keep their runas path.

// The helper window (ExplorerHelperThreadProc) of the explorer.exe running the
// taskbar and desktop: the process ShellExecuteInExplorer reaches.
HWND FindExplorerLaunchHolder() {
    DWORD shellPid = 0;
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &shellPid);
    }
    for (HWND w = FindWindowExW(nullptr, nullptr, kExplorerHelperClassName, kExplorerHelperWindowName); w;
         w = FindWindowExW(nullptr, w, kExplorerHelperClassName, kExplorerHelperWindowName)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (shellPid && pid == shellPid) {
            return w;
        }
    }
    return nullptr;
}

// Runs ShellExecute in Explorer, with the default verb. Returns false when
// Explorer could not be asked; the caller then opens it here instead. Needs
// COM on the calling thread.
bool ShellExecuteInExplorer(const std::wstring& file, const std::wstring& params, const std::wstring& dir) {
    IShellWindows* windows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&windows))) ||
        !windows) {
        return false;
    }
    VARIANT empty{};
    long hwnd = 0;
    IDispatch* desktop = nullptr;
    HRESULT hr = windows->FindWindowSW(&empty, &empty, SWC_DESKTOP, &hwnd, SWFO_NEEDDISPATCH, &desktop);
    windows->Release();
    if (hr != S_OK || !desktop) {
        return false;
    }
    IServiceProvider* provider = nullptr;
    IShellBrowser* browser = nullptr;
    IShellView* view = nullptr;
    IDispatch* background = nullptr;
    IShellFolderViewDual* folderView = nullptr;
    IDispatch* application = nullptr;
    IShellDispatch2* shell = nullptr;
    hr = desktop->QueryInterface(IID_PPV_ARGS(&provider));
    if (SUCCEEDED(hr)) hr = provider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&browser));
    if (SUCCEEDED(hr)) hr = browser->QueryActiveShellView(&view);
    if (SUCCEEDED(hr)) hr = view->GetItemObject(SVGIO_BACKGROUND, IID_PPV_ARGS(&background));
    if (SUCCEEDED(hr)) hr = background->QueryInterface(IID_PPV_ARGS(&folderView));
    if (SUCCEEDED(hr)) hr = folderView->get_Application(&application);
    if (SUCCEEDED(hr)) hr = application->QueryInterface(IID_PPV_ARGS(&shell));
    if (SUCCEEDED(hr)) {
        BSTR fileArg = SysAllocString(file.c_str());
        VARIANT paramsArg{}, dirArg{}, verbArg{}, showArg{};
        if (!params.empty()) {
            paramsArg.vt = VT_BSTR;
            paramsArg.bstrVal = SysAllocString(params.c_str());
        }
        if (!dir.empty()) {
            dirArg.vt = VT_BSTR;
            dirArg.bstrVal = SysAllocString(dir.c_str());
        }
        showArg.vt = VT_I4;
        showArg.lVal = SW_SHOWNORMAL;
        hr = shell->ShellExecute(fileArg, paramsArg, dirArg, verbArg, showArg);
        SysFreeString(fileArg);
        VariantClear(&paramsArg);
        VariantClear(&dirArg);
    }
    for (IUnknown* object : std::initializer_list<IUnknown*>{shell, application, folderView, background, view, browser, provider, desktop}) {
        if (object) {
            object->Release();
        }
    }
    Wh_Log(L"launch: Explorer opened %ls (%08X)", file.c_str(), static_cast<unsigned>(hr));
    return SUCCEEDED(hr);
}

// An app entry as Explorer can open it: shell:AppsFolder and its name there,
// which keeps what its shortcut passes. Empty if that does not lead back to
// the same entry.
std::wstring AppsFolderPath(PCIDLIST_ABSOLUTE pidl) {
    std::wstring path;
    IShellItem* item = nullptr;
    if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) && item) {
        LPWSTR name = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &name)) && name) {
            path = std::wstring(L"shell:AppsFolder\\") + name;
            CoTaskMemFree(name);
        }
        item->Release();
    }
    PIDLIST_ABSOLUTE parsed = nullptr;
    bool same = !path.empty() && SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &parsed, 0, nullptr)) &&
                parsed && ILIsEqual(parsed, pidl);
    if (parsed) {
        ILFree(parsed);
    }
    return same ? path : std::wstring();
}

// Request the launch before hiding search. Explorer already owns this window,
// so no Start-to-Explorer foreground handoff or launch gate is needed.
void DismissSearchForLaunch(std::function<void()> launch) {
    launch();
    SearchWindowHide();
}

bool IsNavigationKey(winrt::Windows::System::VirtualKey key);
bool HandleNavigationKey(winrt::Windows::System::VirtualKey key, bool ctrl);
bool ContextMenuHasFocus();
bool EscapeBelongsToContextMenu();
void NoteContextMenuOpening(wf::IInspectable const& sender);
void NoteContextMenuClosed(wf::IInspectable const& sender);

static bool g_subclassed = false;
void TeardownStartMenuBridge(HWND core);
static LRESULT CALLBACK StartMenuSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR dwRefData) {
    if (uMsg && uMsg == GetTeardownMessage()) {
        TeardownStartMenuBridge(hWnd);
        return kTeardownDone;
    }
    if (uMsg == WM_TIMER && wParam == SearchAppearanceTimerId()) {
        HWND host = FindWindowW(kSearchWindowClass, nullptr);
        if (!g_quit.load() && host &&
            (IsWindowVisible(host) || (IsWindowVisible(hWnd) && !IsCloaked(hWnd))))
            CaptureStartAppearance(host);
        return 0;
    }
    if (uMsg == SearchWindowAppearanceMessage()) {
        CaptureStartAppearance(reinterpret_cast<HWND>(wParam));
        return 0;
    }
    if (uMsg == SearchWindowForegroundMessage()) {
        HWND host = FindWindowW(kSearchWindowClass, nullptr);
        DWORD pid = 0, shellPid = 0;
        GetWindowThreadProcessId(host, &pid);
        GetWindowThreadProcessId(FindWindowW(L"Shell_TrayWnd", nullptr), &shellPid);
        return pid && pid == shellPid && AllowSetForegroundWindow(pid);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Start owns only this appearance/activation bridge, never the search UI.
void TeardownStartMenuBridge(HWND core) {
    if (!core) return;
    KillTimer(core, SearchAppearanceTimerId());
    ChangeWindowMessageFilterEx(core, SearchWindowAppearanceMessage(), MSGFLT_RESET, nullptr);
    if (g_subclassed) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(core, StartMenuSubclassProc);
        g_subclassed = false;
    }
    RemovePropW(core, L"WindhawkStartMenuWindow");
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        tray && GetPropW(tray, L"WindhawkStartMenuHwnd") == core)
        RemovePropW(tray, L"WindhawkStartMenuHwnd");
}

// Find Start's native search control only to copy its appearance.
wux::FrameworkElement FindStockSearchToggle(wux::DependencyObject const& root,
                                            int depth = 24) {
    if (!root || depth < 0) {
        return nullptr;
    }
    try {
        std::wstring_view type{winrt::get_class_name(root)};
        if (type == L"StartMenu.SearchBoxToggleButton") {
            if (auto fe = root.try_as<wux::FrameworkElement>()) {
                std::wstring name{fe.Name()};
                if (name.find(L"Windhawk") == std::wstring::npos) {
                    return fe;
                }
            }
        }
    } catch (...) {
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto found = FindStockSearchToggle(
                wuxm::VisualTreeHelper::GetChild(root, i), depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

void SubclassStartMenuWindow();

// The hook lives on a thread of its own: UnhookWinEvent only works on the
// thread that called SetWinEventHook, and Wh_ModAfterInit and Wh_ModUninit
// don't always run on the same thread. A hook left behind keeps the DLL loaded
// and its callback running in Start after the mod is unloaded.
HANDLE g_attachWatchThread = nullptr;
HANDLE g_attachWatchStop = nullptr;
std::atomic<int> g_attachWatchCalls{0};

void CALLBACK AttachWatchProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                              LONG idObject, LONG idChild, DWORD, DWORD) {
    g_attachWatchCalls.fetch_add(1);
    struct Leave {
        ~Leave() { g_attachWatchCalls.fetch_sub(1); }
    } leave;
    if (g_quit.load() || g_targetProcess != TargetProcess::StartMenu) return;
    if ((event != EVENT_OBJECT_CREATE && event != EVENT_OBJECT_SHOW && event != EVENT_OBJECT_UNCLOAKED) ||
        !hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if (event == EVENT_OBJECT_CREATE) {
        wchar_t cls[128]{};
        GetClassNameW(hwnd,cls,ARRAYSIZE(cls));
        if (wcscmp(cls,L"Windows.UI.Core.CoreWindow") == 0) SubclassStartMenuWindow();
        return;
    }
    // In-context, so this is the thread that raised the event. The filter is
    // Window::Current() returning something rather than a class name, because
    // a name that changes across builds breaks quietly.
    SubclassStartMenuWindow();
}

DWORD WINAPI AttachWatchThreadProc(LPVOID) {
    // In-context, so the callback runs on whichever thread shows the window;
    // this thread only owns the hooks, and needs no message loop for them.
    // One hook per event: the range between them covers focus, location and
    // name changes too, each of which would be a call into the mod.
    const DWORD events[] = {EVENT_OBJECT_CREATE, EVENT_OBJECT_SHOW, EVENT_OBJECT_UNCLOAKED};
    HWINEVENTHOOK hooks[ARRAYSIZE(events)] = {};
    for (size_t i = 0; i < ARRAYSIZE(events); ++i) {
        hooks[i] = SetWinEventHook(events[i], events[i], GetCurrentModuleHandle(),
                                   AttachWatchProc, GetCurrentProcessId(), 0,
                                   WINEVENT_INCONTEXT);
        if (hooks[i]) {
            Wh_Log(L"attach watch: event 0x%X hooked on thread %lu", events[i], GetCurrentThreadId());
        } else {
            Wh_Log(L"attach watch: event 0x%X FAILED, err=%lu", events[i], GetLastError());
        }
    }
    WaitForSingleObject(g_attachWatchStop, INFINITE);
    for (HWINEVENTHOOK hook : hooks) {
        if (hook && !UnhookWinEvent(hook)) {
            Wh_Log(L"attach watch: unhook failed, err=%lu", GetLastError());
        }
    }
    return 0;
}

void StartAttachWatch() {
    if (g_attachWatchThread) {
        return;
    }
    g_attachWatchStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_attachWatchStop) {
        Wh_Log(L"attach watch: CreateEvent failed, err=%lu", GetLastError());
        return;
    }
    g_attachWatchThread = CreateThread(nullptr, 0, AttachWatchThreadProc, nullptr, 0, nullptr);
    if (!g_attachWatchThread) {
        Wh_Log(L"attach watch: CreateThread failed, err=%lu", GetLastError());
        CloseHandle(g_attachWatchStop);
        g_attachWatchStop = nullptr;
    }
}

// Must not run on Start's XAML thread: it waits for callbacks, which run there.
void StopAttachWatch() {
    if (!g_attachWatchThread) {
        return;
    }
    SetEvent(g_attachWatchStop);
    WaitForSingleObject(g_attachWatchThread, INFINITE);
    CloseHandle(g_attachWatchThread);
    g_attachWatchThread = nullptr;
    CloseHandle(g_attachWatchStop);
    g_attachWatchStop = nullptr;
    // A call that got in before the unhook may still be running elsewhere.
    for (int i = 0; g_attachWatchCalls.load() > 0 && i < 200; ++i) {
        Sleep(10);
    }
}

void SubclassStartMenuWindow() {
    if (g_targetProcess != TargetProcess::StartMenu) return;
    HWND ours = nullptr;
    EnumWindows(
        [](HWND hwnd, LPARAM param) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid != GetCurrentProcessId()) return TRUE;
            wchar_t cls[128] = {};
            GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
            if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                *reinterpret_cast<HWND*>(param) = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ours));
    if (ours) {
        ChangeWindowMessageFilterEx(ours,SearchWindowAppearanceMessage(),MSGFLT_ALLOW,nullptr);
        SetTimer(ours,SearchAppearanceTimerId(),500,nullptr);
        if (HWND host = FindWindowW(kSearchWindowClass,nullptr))
            PostMessageW(ours,SearchWindowAppearanceMessage(),reinterpret_cast<WPARAM>(host),0);
        g_hCoreWindow = ours;
        SetPropW(ours, L"WindhawkStartMenuWindow", (HANDLE)1);
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) {
            SetPropW(tray, L"WindhawkStartMenuHwnd", ours);
        }
        if (!g_subclassed) {
            if (WindhawkUtils::SetWindowSubclassFromAnyThread(ours, StartMenuSubclassProc, 0)) {
                g_subclassed = true;
                Wh_Log(L"subclass: hooked CoreWindow HWND %p", ours);
            }
        }
    }
}

// Results, handed from the search thread to the XAML thread.
std::mutex g_resultsMutex;
std::atomic<DWORD> g_totalMatches{0};

[[clang::no_destroy]] std::optional<std::thread> g_searchThread;
std::mutex g_queryMutex;
std::condition_variable g_queryWake;
std::wstring g_pendingQuery;
std::atomic<bool> g_searchQuit{false};
std::atomic<bool> g_queryDirty{false};

void RequestAppIndexRefresh() {
    ULONGLONG now = GetTickCount64();
    if (now - g_lastAppIndexRebuildTick.load() >= 5000) {
        {
            std::lock_guard<std::mutex> lock(g_queryMutex);
            g_appIndexNeedsRefresh.store(true);
        }
        g_queryWake.notify_all();
    }
}

// A row as the XAML thread needs it: text, an optional icon as raw BGRA, and
// what to do when it is clicked.
//
// Icons travel as pixels rather than as a path to fetch later, because the
// fetch is the slow part and it has already happened on the search thread.
struct Row {
    std::wstring title;
    std::wstring subtitle;
    std::wstring openPath;   // files: what ShellExecute opens
    int appIndex = -1;       // apps: which entry of the index to launch
    std::vector<BYTE> icon;  // premultiplied BGRA, IconPixels() square, or empty
    bool canRunAsAdmin = true;
    bool isSetting = false;
    bool isFolder = false;
    bool isFile = false;
    std::wstring programPath; // apps launched by app ID: their program on disk, for its file actions
    std::wstring copyText;   // text to copy to clipboard on activation
    std::wstring customGlyph; // Segoe Fluent glyph override (e.g. \uE1D0, \uE701, \uE88E)
    uint32_t glyphColor = 0;  // ARGB of the glyph -- a conversion's kind (calc::KindColor) -- or 0
    std::wstring commandLine;
    CustomCommandShell commandShell = CustomCommandShell::Cmd;
    std::wstring commandShellExecutable;
    std::wstring commandShellArguments;
    bool commandHideTerminal = false;
    std::wstring commandIcon;
    std::wstring customIconText;
    bool customIconIsGlyph = false;
    uint64_t commandRevision = 0;
    int matchScore = -1;
    int matchRank = 0;
};

// Icons take 24 logical pixels in a row, which on a display at 150% is 36 real
// ones. They are made at that real size (IconPixels), so XAML has nothing to
// scale.
inline constexpr int kIconDisplay = 24;  // what an icon occupies in the row, in DIPs

// The same in physical pixels on the display Start is on: the size icons are
// made at (see icons::Resample), so XAML draws them without scaling.
int IconPixels() {
    HWND start = GetSearchWindow();
    UINT dpi = start ? GetDpiForWindow(start) : 0;
    if (!dpi) {
        dpi = GetDpiForSystem();
    }
    return std::clamp(MulDiv(kIconDisplay, static_cast<int>(dpi), 96), 16, 256);
}

// A row's icon as a bitmap, or null when it has none.
wuxmi::WriteableBitmap IconBitmap(const std::vector<BYTE>& pixels) {
    const int side = icons::IconSide(pixels);
    if (!side) {
        return nullptr;
    }
    wuxmi::WriteableBitmap bitmap{side, side};
    auto access = bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
    BYTE* dest = nullptr;
    if (FAILED(access->Buffer(&dest)) || !dest) {
        return nullptr;
    }
    memcpy(dest, pixels.data(), pixels.size());
    bitmap.Invalidate();
    return bitmap;
}

std::vector<Row> g_appRows;
std::vector<Row> g_fileRows;
std::wstring g_rowsQuery;  // the text g_appRows and g_fileRows are the results for

// Launch requests, posted from the XAML thread back to the search thread,
// which owns the app index and therefore the PIDLs.
//
// A PIDL cannot simply be captured in a click handler: the index is rebuilt
// and the pointer would dangle. Sending an index back to the owning thread
// keeps the lifetime where the data lives.
std::atomic<int> g_launchRequest{-1};
std::atomic<bool> g_launchAsAdmin{false};
std::vector<std::wstring> g_commandUsageRequests; // g_queryMutex; drained by the search thread


// Opens what was clicked.
//
// ShellExecute rather than CreateProcess: these are paths of any kind, and
// the shell decides what opening one means. This process runs at the user's
// own integrity, so what opens is what the user would have opened -- which is
// exactly what the broker could not promise when it ran elevated.
void OpenResult(std::wstring path, bool asAdmin = false) {
    SpawnTrackedLaunch([path = std::move(path), asAdmin] {
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        wchar_t userProfile[MAX_PATH] = {};
        if (!GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH) || !userProfile[0]) {
            PWSTR kf = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &kf)) && kf) {
                wcsncpy_s(userProfile, kf, MAX_PATH - 1);
                CoTaskMemFree(kf);
            }
        }

        std::wstring lowerPath = path;
        for (auto& c : lowerPath) c = static_cast<wchar_t>(towlower(c));
        bool isTerminalOrShell = (lowerPath.find(L"cmd.exe") != std::wstring::npos ||
                                  lowerPath.find(L"powershell") != std::wstring::npos ||
                                  lowerPath.find(L"pwsh") != std::wstring::npos ||
                                  lowerPath.find(L"windowsterminal") != std::wstring::npos ||
                                  lowerPath.ends_with(L"wt.exe"));

        size_t lastSlash = path.find_last_of(L"\\/");
        std::wstring parentDir = (lastSlash != std::wstring::npos) ? path.substr(0, lastSlash) : L"";
        LPCWSTR workDir = isTerminalOrShell ? userProfile : (!parentDir.empty() ? parentDir.c_str() : userProfile);

        std::wstring params;
        if (lowerPath.ends_with(L"cmd.exe")) {
            params = L"/k cd /d \"" + std::wstring(userProfile) + L"\"";
        } else if (lowerPath.find(L"powershell.exe") != std::wstring::npos || lowerPath.ends_with(L"pwsh.exe")) {
            params = L"-NoExit -Command \"Set-Location '" + std::wstring(userProfile) + L"'\"";
        }

        SHELLEXECUTEINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOASYNC | (asAdmin ? 0 : SEE_MASK_FLAG_NO_UI);
        info.lpVerb = asAdmin ? L"runas" : L"open";
        info.lpFile = path.c_str();
        if (!params.empty()) {
            info.lpParameters = params.c_str();
        }
        info.lpDirectory = workDir;
        info.nShow = SW_SHOWNORMAL;
        if ((asAdmin || !ShellExecuteInExplorer(path, params, workDir)) && !ShellExecuteExW(&info)) {
            Wh_Log(L"open failed (%lu): %ls (admin=%d)", GetLastError(), path.c_str(), asAdmin ? 1 : 0);
        }
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

// Lets the taskbar's explorer.exe, where the Properties relay runs, bring the
// sheet it opens for us to the front. Only a process that just got user input
// may hand that right on, and Start stops being one once it closes, so this
// is called from the click handler, before DismissSearch.
void AllowExplorerForeground() {
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        DWORD explorerPid = 0;
        GetWindowThreadProcessId(tray, &explorerPid);
        if (explorerPid) {
            AllowSetForegroundWindow(explorerPid);
        }
    }
}

// Opens a folder window with the file selected, through the shell rather than
// by starting explorer.exe /select. Which explorer.exe gets the window is not
// ours to know -- a started one hands the request on and exits, and the window
// lands in another it never met -- but the shell's own call reaches that
// process over COM and hands it our right to the foreground on the way. Runs
// on a worker (OpenFileLocationThenDismiss), while Start is still in front.
void OpenFileLocation(const std::wstring& path) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr) && pidl) {
        hr = SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        CoTaskMemFree(pidl);
    }
    if (FAILED(hr)) {
        Wh_Log(L"open location failed (%08X): %ls", static_cast<unsigned>(hr), path.c_str());
    }
}

// "Open file location" from a menu: the folder opens on a worker, and Start is
// dismissed after, back on its thread. The shell call waits on Explorer and
// pumps messages meanwhile; in the click handler, Start losing the foreground
// then closed the menu and rebuilt the results under the running handler, and
// a slow or disconnected share froze Start. The foreground right belongs to
// the process, so the folder still opens in front.
void OpenFileLocationThenDismiss(std::wstring path) {
    wuc::CoreDispatcher dispatcher{nullptr};
    try {
        if (g_ourBox) {
            dispatcher = g_ourBox.Dispatcher();
        }
    } catch (...) {}
    SpawnTrackedLaunch([path = std::move(path), dispatcher] {
        HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        OpenFileLocation(path);
        if (SUCCEEDED(co)) {
            CoUninitialize();
        }
        if (dispatcher && !g_quit.load()) {
            dispatcher.RunAsync(wuc::CoreDispatcherPriority::Normal,
                                wuc::DispatchedHandler{[] { DismissSearch(); }});
        }
    });
}

// Every explorer.exe hosts a relay window (StartExplorerHelperHost). The one in
// the taskbar's process is used when it exists: that is the process the
// Properties click lets take the foreground, so the dialog opens in front.
HWND FindExplorerHelperWindow() {
    DWORD trayPid = 0;
    if (HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        GetWindowThreadProcessId(tray, &trayPid);
    }
    HWND any = nullptr;
    for (HWND hwnd = FindWindowExW(nullptr, nullptr, kExplorerHelperClassName, kExplorerHelperWindowName); hwnd;
         hwnd = FindWindowExW(nullptr, hwnd, kExplorerHelperClassName, kExplorerHelperWindowName)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == trayPid) {
            return hwnd;
        }
        if (!any) {
            any = hwnd;
        }
    }
    return any;
}

// What Uninstall in an app's menu does for an app. A packaged app is removed
// for this user, unless it is part of Windows. A program runs the uninstaller
// it registered for Settings' Installed apps -- found by its program file,
// install folder or name -- and one that registered none, or that is part of
// Windows, gets no Uninstall.
namespace uninstall {

struct Command {
    std::wstring name;    // as Installed apps shows it
    std::wstring file;    // the uninstaller
    std::wstring params;
};

inline bool EqualsI(const std::wstring& a, const std::wstring& b) {
    return a.size() == b.size() && CompareStringOrdinal(a.c_str(), -1, b.c_str(), -1, TRUE) == CSTR_EQUAL;
}

// Whether path is dir or inside it.
inline bool IsUnder(const std::wstring& path, const std::wstring& dir) {
    if (dir.empty() || path.size() < dir.size() ||
        CompareStringOrdinal(path.c_str(), static_cast<int>(dir.size()), dir.c_str(), static_cast<int>(dir.size()),
                             TRUE) != CSTR_EQUAL) {
        return false;
    }
    return path.size() == dir.size() || path[dir.size()] == L'\\' || dir.back() == L'\\';
}

inline std::wstring Folder(const std::wstring& path) {
    size_t slash = path.find_last_of(L'\\');
    return slash == std::wstring::npos ? std::wstring() : path.substr(0, slash);
}

// A registry path value as a plain path: environment variables expanded,
// without quotes, an icon index (",0") or a trailing backslash.
inline std::wstring CleanPath(std::wstring s, bool iconIndex) {
    if (s.find(L'%') != std::wstring::npos) {
        wchar_t expanded[MAX_PATH * 2];
        DWORD n = ExpandEnvironmentStringsW(s.c_str(), expanded, ARRAYSIZE(expanded));
        if (n && n <= ARRAYSIZE(expanded)) {
            s = expanded;
        }
    }
    auto trim = [&s] {
        while (!s.empty() && (s.front() == L' ' || s.front() == L'"')) s.erase(s.begin());
        while (!s.empty() && (s.back() == L' ' || s.back() == L'"')) s.pop_back();
    };
    trim();
    if (iconIndex) {
        size_t comma = s.find_last_of(L',');
        if (comma != std::wstring::npos && comma + 1 < s.size() &&
            s.find_first_not_of(L"-0123456789 ", comma + 1) == std::wstring::npos) {
            s.resize(comma);
            trim();
        }
    }
    while (s.size() > 3 && s.back() == L'\\') s.pop_back();
    return s;
}

// Splits a command line into the program and its arguments: the quoted part,
// else up to the first ".exe" (uninstall strings are often unquoted paths
// with spaces), else up to the first space.
inline void SplitCommand(const std::wstring& line, std::wstring& file, std::wstring& params) {
    std::wstring s = line;
    while (!s.empty() && s.front() == L' ') s.erase(s.begin());
    size_t end;
    if (!s.empty() && s.front() == L'"') {
        end = s.find(L'"', 1);
        file = s.substr(1, end == std::wstring::npos ? std::wstring::npos : end - 1);
        end = end == std::wstring::npos ? s.size() : end + 1;
    } else {
        std::wstring lower = s;
        for (auto& c : lower) c = static_cast<wchar_t>(towlower(c));
        size_t exe = lower.find(L".exe");
        end = exe != std::wstring::npos ? exe + 4 : s.find(L' ');
        if (end == std::wstring::npos) end = s.size();
        file = s.substr(0, end);
    }
    params = end < s.size() ? s.substr(end) : std::wstring();
    while (!params.empty() && params.front() == L' ') params.erase(params.begin());
    if (file.find(L'%') != std::wstring::npos) {
        file = CleanPath(file, false);
    }
}

inline std::wstring ReadString(HKEY key, const wchar_t* name) {
    wchar_t buf[2048];
    DWORD size = sizeof(buf) - sizeof(wchar_t);
    DWORD type = 0;
    if (RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<BYTE*>(buf), &size) != ERROR_SUCCESS ||
        (type != REG_SZ && type != REG_EXPAND_SZ)) {
        return {};
    }
    buf[size / sizeof(wchar_t)] = 0;
    return buf;
}

inline DWORD ReadDword(HKEY key, const wchar_t* name) {
    DWORD value = 0, size = sizeof(value), type = 0;
    if (RegQueryValueExW(key, name, nullptr, &type, reinterpret_cast<BYTE*>(&value), &size) != ERROR_SUCCESS ||
        type != REG_DWORD) {
        return 0;
    }
    return value;
}

// Folders that hold many programs, which an install folder must not be taken
// for (some installers register one as theirs).
inline std::vector<std::wstring> SharedFolders() {
    static const KNOWNFOLDERID* const kShared[] = {
        &FOLDERID_ProgramFiles,       &FOLDERID_ProgramFilesX86, &FOLDERID_ProgramFilesCommon,
        &FOLDERID_ProgramFilesCommonX86, &FOLDERID_LocalAppData,  &FOLDERID_RoamingAppData,
        &FOLDERID_UserProgramFiles,   &FOLDERID_Windows,         &FOLDERID_Profile,
        &FOLDERID_ProgramData,
    };
    std::vector<std::wstring> folders;
    for (const KNOWNFOLDERID* id : kShared) {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(*id, KF_FLAG_DONT_VERIFY, nullptr, &path)) && path) {
            folders.push_back(path);
        }
        CoTaskMemFree(path);
    }
    return folders;
}

inline bool IsSharedFolder(const std::wstring& dir, const std::vector<std::wstring>& shared) {
    if (dir.size() <= 3) {
        return true;  // a drive
    }
    for (const std::wstring& folder : shared) {
        if (EqualsI(dir, folder)) {
            return true;
        }
    }
    return false;
}

inline bool IsWindowsPath(const std::wstring& path) {
    wchar_t windir[MAX_PATH];
    UINT n = GetWindowsDirectoryW(windir, MAX_PATH);
    return n && n < MAX_PATH && IsUnder(path, windir);
}

// How well an entry's name fits the app's name in Start: 2 the same, 1 the
// same but for a version ("NVIDIA App 11.0.9" for "NVIDIA App"), or the
// start of it, at least two words long ("File Converter" for "File
// Converter Settings"), else 0.
inline int NameMatch(const std::wstring& name, const std::wstring& title) {
    if (name.empty() || title.empty()) {
        return 0;
    }
    if (EqualsI(name, title)) {
        return 2;
    }
    auto startsWithWord = [](const std::wstring& s, const std::wstring& prefix) {
        return s.size() > prefix.size() + 1 && s[prefix.size()] == L' ' &&
               CompareStringOrdinal(s.c_str(), static_cast<int>(prefix.size()), prefix.c_str(),
                                    static_cast<int>(prefix.size()), TRUE) == CSTR_EQUAL;
    };
    if (startsWithWord(name, title)) {
        const wchar_t next = name[title.size() + 1];
        const wchar_t after = title.size() + 2 < name.size() ? name[title.size() + 2] : 0;
        if (iswdigit(next) || next == L'(' || ((next == L'v' || next == L'V') && iswdigit(after))) {
            return 1;
        }
    }
    if (startsWithWord(title, name) && name.find(L' ') != std::wstring::npos) {
        return 1;
    }
    return 0;
}

// The uninstaller registered for a program (its file, or empty when not
// known) and its name in Start.
inline std::optional<Command> FindProgramUninstaller(const std::wstring& program, const std::wstring& title) {
    if (!program.empty() && IsWindowsPath(program)) {
        return std::nullopt;
    }
    const std::wstring programDir = Folder(program);
    const std::vector<std::wstring> shared = SharedFolders();
    struct Root {
        HKEY hive;
        REGSAM view;
    };
    static const Root kRoots[] = {
        {HKEY_LOCAL_MACHINE, KEY_WOW64_64KEY},
        {HKEY_LOCAL_MACHINE, KEY_WOW64_32KEY},
        {HKEY_CURRENT_USER, 0},
    };
    std::optional<Command> best;
    int bestScore = 0;
    size_t bestDepth = 0;
    for (const Root& root : kRoots) {
        HKEY list = nullptr;
        if (RegOpenKeyExW(root.hive, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0,
                          KEY_READ | root.view, &list) != ERROR_SUCCESS) {
            continue;
        }
        wchar_t sub[256];
        for (DWORD i = 0;; ++i) {
            DWORD subLen = ARRAYSIZE(sub);
            if (RegEnumKeyExW(list, i, sub, &subLen, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) {
                break;
            }
            HKEY entry = nullptr;
            if (RegOpenKeyExW(list, sub, 0, KEY_READ | root.view, &entry) != ERROR_SUCCESS) {
                continue;
            }
            // As Installed apps lists them: named, removable, not an update.
            const std::wstring name = ReadString(entry, L"DisplayName");
            const std::wstring uninstallString = ReadString(entry, L"UninstallString");
            const bool listed = !name.empty() && !uninstallString.empty() && !ReadDword(entry, L"SystemComponent") &&
                                !ReadDword(entry, L"NoRemove") && ReadString(entry, L"ParentKeyName").empty();

            int score = 0;
            size_t depth = 0;
            if (listed) {
                std::wstring file, params;
                SplitCommand(uninstallString, file, params);
                const std::wstring icon = CleanPath(ReadString(entry, L"DisplayIcon"), true);
                const std::wstring location = CleanPath(ReadString(entry, L"InstallLocation"), false);
                const std::wstring fileName = file.substr(file.find_last_of(L'\\') + 1);
                const bool sharedUninstaller = EqualsI(fileName, L"msiexec.exe") || EqualsI(fileName, L"rundll32.exe");
                // A launcher's uninstaller removes something else -- a game:
                // steam.exe steam://uninstall/730.
                const bool forOther = EqualsI(file, program) || params.find(L"://") != std::wstring::npos;
                int rule = 0;
                if (!program.empty() && EqualsI(icon, program)) {
                    rule = 4;
                } else if (!program.empty() && !location.empty() && IsUnder(program, location) &&
                           !IsSharedFolder(location, shared)) {
                    rule = 3;
                    depth = location.size();
                } else if (!program.empty() && !sharedUninstaller && !forOther && !programDir.empty() &&
                           (IsUnder(file, programDir) || IsUnder(icon, programDir)) && !IsSharedFolder(programDir, shared)) {
                    rule = 2;
                }
                const int named = NameMatch(name, title);
                if (!rule && named) {
                    rule = 1;
                }
                score = rule ? rule * 4 + named : 0;
                if (score > bestScore || (score == bestScore && score && depth > bestDepth)) {
                    // MSI's /I opens its repair-or-remove dialog; /X removes.
                    if (EqualsI(fileName, L"msiexec.exe") || EqualsI(file, L"msiexec")) {
                        for (size_t at = 0; at + 1 < params.size(); ++at) {
                            if (params[at] == L'/' && (params[at + 1] == L'I' || params[at + 1] == L'i')) {
                                params[at + 1] = L'X';
                                break;
                            }
                        }
                    }
                    best = Command{name, file, params};
                    bestScore = score;
                    bestDepth = depth;
                }
            }
            RegCloseKey(entry);
        }
        RegCloseKey(list);
    }
    return best;
}

// The package family of a packaged app's app ID ("family!app") when it can be
// uninstalled: installed for this user and not part of Windows (those live
// under the Windows folder, in SystemApps).
inline std::wstring RemovablePackageFamily(const std::wstring& appId) {
    const size_t bang = appId.find(L'!');
    if (bang == std::wstring::npos || bang == 0 || appId.find(L'\\') != std::wstring::npos) {
        return {};
    }
    const std::wstring family = appId.substr(0, bang);
    UINT32 count = 0, length = 0;
    if (GetPackagesByPackageFamily(family.c_str(), &count, nullptr, &length, nullptr) != ERROR_INSUFFICIENT_BUFFER ||
        !count) {
        return {};
    }
    std::vector<PWSTR> names(count);
    std::vector<wchar_t> buffer(length);
    if (GetPackagesByPackageFamily(family.c_str(), &count, names.data(), &length, buffer.data()) != ERROR_SUCCESS ||
        !count) {
        return {};
    }
    wchar_t path[MAX_PATH];
    UINT32 pathLength = MAX_PATH;
    if (GetPackagePathByFullName(names[0], &pathLength, path) != ERROR_SUCCESS || IsWindowsPath(path)) {
        return {};
    }
    return family;
}

}  // namespace uninstall

// Has Explorer remove a packaged app (AcceptPackageRemoval).
bool RequestPackageRemoval(const std::wstring& family) {
    HWND helper = FindExplorerHelperWindow();
    if (!helper) {
        return false;
    }
    std::wstring request = family;
    COPYDATASTRUCT cds{};
    cds.dwData = kExplorerUninstallMagic;
    cds.cbData = static_cast<DWORD>((request.size() + 1) * sizeof(wchar_t));
    cds.lpData = request.data();
    DWORD_PTR accepted = 0;
    g_removalPending = family;  // Explorer asks back (RemovalCheckMessage)
    const bool ok = SendMessageTimeoutW(helper, WM_COPYDATA, reinterpret_cast<WPARAM>(GetSearchWindow()),
                                        reinterpret_cast<LPARAM>(&cds), SMTO_ABORTIFHUNG, 2000, &accepted) &&
                    accepted;
    g_removalPending.clear();
    return ok;
}

// Starts a program's uninstaller, through Explorer like any program opened
// from search (DismissSearchForLaunch), so it comes to the front -- and
// asks for elevation itself when it needs it.
void RunUninstaller(uninstall::Command command) {
    SpawnTrackedLaunch([command = std::move(command)] {
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        const std::wstring dir = uninstall::Folder(command.file);
        if (!ShellExecuteInExplorer(command.file, command.params, dir)) {
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpFile = command.file.c_str();
            sei.lpParameters = command.params.c_str();
            sei.lpDirectory = dir.empty() ? nullptr : dir.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        }
        Wh_Log(L"uninstall: ran %ls %ls", command.file.c_str(), command.params.c_str());
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}


// Asks before uninstalling, as Start's own Uninstall does: a flyout on the
// app's card naming what goes.
void ConfirmUninstall(wux::FrameworkElement const& anchor, std::wstring const& name, std::function<void()> uninstallNow) {
    wuxc::Flyout flyout;
    wuxc::StackPanel panel;
    panel.MaxWidth(300);
    auto text = SearchTextBlock();
    text.Text(winrt::hstring{name + L" and its related info will be uninstalled."});
    text.TextWrapping(wux::TextWrapping::Wrap);
    panel.Children().Append(text);
    wuxc::Button confirm;
    confirm.Content(winrt::box_value(L"Uninstall"));
    confirm.HorizontalAlignment(wux::HorizontalAlignment::Right);
    confirm.Margin(wux::ThicknessHelper::FromLengths(0, 12, 0, 0));
    try {
        auto resources = wux::Application::Current().Resources();
        auto key = winrt::box_value(L"AccentButtonStyle");
        if (resources.HasKey(key)) {
            confirm.Style(resources.Lookup(key).as<wux::Style>());
        }
    } catch (...) {
    }
    KeepHandler(confirm, confirm.Click(winrt::auto_revoke, [weakFlyout = winrt::make_weak(flyout), uninstallNow](
                                                               wf::IInspectable const&, wux::RoutedEventArgs const&) {
        if (auto f = weakFlyout.get()) {
            f.Hide();
        }
        uninstallNow();
    }));
    panel.Children().Append(confirm);
    flyout.Content(panel);
    // Use the space below the card when there is not enough room above it
    // inside the independent search window, including the first result.
    try {
        panel.Measure(wf::Size{300, std::numeric_limits<float>::infinity()});
        constexpr double kChrome = 40;  // the flyout's padding, border and gap to the card
        const double cardTop = anchor.TransformToVisual(nullptr).TransformPoint(wf::Point{0, 0}).Y;
        if (cardTop - panel.DesiredSize().Height - kChrome < 0) {
            flyout.Placement(wuxcp::FlyoutPlacementMode::Bottom);
        }
    } catch (...) {
    }
    KeepHandler(flyout, flyout.Opening(winrt::auto_revoke, [](wf::IInspectable const& sender, wf::IInspectable const&) {
        NoteContextMenuOpening(sender);
    }));
    KeepHandler(flyout, flyout.Closed(winrt::auto_revoke, [](wf::IInspectable const& sender, wf::IInspectable const&) {
        NoteContextMenuClosed(sender);
    }));
    flyout.ShowAt(anchor);
}

void ShowPropertiesDialog(std::wstring path) {
    SpawnTrackedLaunch([path = std::move(path)] {
        std::wstring cleanPath = path;
        while (!cleanPath.empty() && (cleanPath.front() == L' ' || cleanPath.front() == L'\t' || cleanPath.front() == L'"')) {
            cleanPath.erase(cleanPath.begin());
        }
        while (!cleanPath.empty() && (cleanPath.back() == L' ' || cleanPath.back() == L'\t' || cleanPath.back() == L'"')) {
            cleanPath.pop_back();
        }

        if (cleanPath.empty()) return;

        // 1. Relay to Explorer host window (runs at Medium integrity desktop shell)
        HWND hHost = nullptr;
        for (int retry = 0; retry < 3 && !hHost; ++retry) {
            hHost = FindExplorerHelperWindow();
            if (!hHost) Sleep(50);
        }

        if (hHost && IsWindow(hHost)) {
            COPYDATASTRUCT cds{};
            cds.dwData = kExplorerCopyDataMagic;
            cds.cbData = static_cast<DWORD>((cleanPath.size() + 1) * sizeof(wchar_t));
            cds.lpData = const_cast<wchar_t*>(cleanPath.c_str());

            DWORD_PTR dwResult = 0;
            LRESULT lres = SendMessageTimeoutW(
                hHost,
                WM_COPYDATA,
                0,
                reinterpret_cast<LPARAM>(&cds),
                SMTO_ABORTIFHUNG | SMTO_NORMAL,
                3000,
                &dwResult
            );
            Wh_Log(L"ShowPropertiesDialog: SendMessageTimeoutW to explorer host returned %ld, res=%llu for %ls",
                   lres, (unsigned long long)dwResult, cleanPath.c_str());
            if (lres && dwResult == 1) {
                return; // Successfully handed off to Explorer
            }
        }

        Wh_Log(L"ShowPropertiesDialog: explorer host not reachable, trying direct fallback for %ls", cleanPath.c_str());

        // 2. Direct in-process fallback
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        BOOL ok = SHObjectProperties(nullptr, 0x00000002 /* SHOP_FILEPATH */, cleanPath.c_str(), nullptr);
        if (!ok) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            SFGAOF sfgao = 0;
            if (SUCCEEDED(SHParseDisplayName(cleanPath.c_str(), nullptr, &pidl, 0, &sfgao)) && pidl) {
                SHELLEXECUTEINFOW sei{};
                sei.cbSize = sizeof(sei);
                sei.fMask = SEE_MASK_INVOKEIDLIST;
                sei.hwnd = nullptr;
                sei.lpIDList = pidl;
                sei.lpVerb = L"properties";
                sei.nShow = SW_SHOWNORMAL;
                ok = ShellExecuteExW(&sei);
                CoTaskMemFree(pidl);
            }
        }

        Sleep(1000);

        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

// PowerShell receives UTF-16LE code directly, without another shell changing
// its quotes, variables, or operators.
std::wstring EncodePowerShellCommand(std::wstring const& command) {
    static constexpr wchar_t alphabet[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static_assert(sizeof(wchar_t) == 2);
    auto bytes = reinterpret_cast<const unsigned char*>(command.data());
    const size_t length = command.size() * sizeof(wchar_t);
    std::wstring encoded;
    encoded.reserve((length + 2) / 3 * 4);
    for (size_t i = 0; i < length; i += 3) {
        const uint32_t value = (uint32_t(bytes[i]) << 16) |
            (i + 1 < length ? uint32_t(bytes[i + 1]) << 8 : 0) |
            (i + 2 < length ? uint32_t(bytes[i + 2]) : 0);
        encoded.push_back(alphabet[(value >> 18) & 63]);
        encoded.push_back(alphabet[(value >> 12) & 63]);
        encoded.push_back(i + 1 < length ? alphabet[(value >> 6) & 63] : L'=');
        encoded.push_back(i + 2 < length ? alphabet[value & 63] : L'=');
    }
    return encoded;
}

std::wstring ExpandCustomShellEnvironment(std::wstring const& value) {
    const DWORD needed=ExpandEnvironmentStringsW(value.c_str(),nullptr,0);
    if(!needed || needed>32767) return {};
    std::wstring expanded(needed,L'\0');
    const DWORD written=ExpandEnvironmentStringsW(value.c_str(),expanded.data(),needed);
    if(!written || written>needed) return {};
    expanded.resize(written-1);
    return expanded;
}

std::wstring ResolveCustomShellExecutable(std::wstring const& value) {
    auto name=tools::Trim(ExpandCustomShellEnvironment(value));
    if(name.size()>1 && name.front()==L'"' && name.back()==L'"') name=name.substr(1,name.size()-2);
    if(name.empty() || name.find(L'"')!=std::wstring::npos) return {};
    const auto extension=name.find_last_of(L'.');
    if(extension!=std::wstring::npos &&
       _wcsicmp(name.c_str()+extension,L".exe")!=0 && _wcsicmp(name.c_str()+extension,L".com")!=0) return {};
    const bool hasPath=name.find_first_of(L"\\/:")!=std::wstring::npos;
    const DWORD needed=hasPath ? GetFullPathNameW(name.c_str(),0,nullptr,nullptr)
                              : SearchPathW(nullptr,name.c_str(),L".exe",0,nullptr,nullptr);
    if(!needed || needed>32767) return {};
    std::wstring resolved(needed+1,L'\0');
    const DWORD written=hasPath ? GetFullPathNameW(name.c_str(),static_cast<DWORD>(resolved.size()),resolved.data(),nullptr)
                               : SearchPathW(nullptr,name.c_str(),L".exe",static_cast<DWORD>(resolved.size()),resolved.data(),nullptr);
    if(!written || written>=resolved.size()) return {};
    resolved.resize(written);
    const DWORD attributes=GetFileAttributesW(resolved.c_str());
    return attributes!=INVALID_FILE_ATTRIBUTES && !(attributes&FILE_ATTRIBUTE_DIRECTORY) ? resolved : std::wstring();
}

std::wstring QuoteCustomShellArgument(std::wstring const& value) {
    std::wstring quoted=L"\"";
    size_t slashes=0;
    for(wchar_t c:value) {
        if(c==L'\\') { ++slashes; continue; }
        quoted.append(c==L'"' ? slashes*2+1 : slashes,L'\\');
        quoted.push_back(c);
        slashes=0;
    }
    quoted.append(slashes*2,L'\\');
    quoted.push_back(L'"');
    return quoted;
}

std::wstring CustomCommandExecutable(std::wstring const& system, CustomCommandShell shell,
                                     std::wstring const& customExecutable=L"") {
    if(shell==CustomCommandShell::Custom) return ResolveCustomShellExecutable(customExecutable);
    if(shell==CustomCommandShell::PowerShell7) {
        if(auto installed=ResolveCustomShellExecutable(L"%ProgramFiles%\\PowerShell\\7\\pwsh.exe"); !installed.empty())
            return installed;
        return ResolveCustomShellExecutable(L"pwsh.exe");
    }
    return system + (shell == CustomCommandShell::PowerShell
        ? L"\\WindowsPowerShell\\v1.0\\powershell.exe" : L"\\cmd.exe");
}

std::wstring CustomCommandParameters(std::wstring const& command, CustomCommandShell shell, bool hideTerminal,
                                     std::wstring const& customArguments=L"-c {command}") {
    if(shell==CustomCommandShell::Custom) {
        constexpr std::wstring_view marker=L"{command}";
        const size_t at=customArguments.find(marker);
        const auto space=[](wchar_t c) { return c==L' ' || c==L'\t'; };
        if(at==std::wstring::npos || customArguments.find(marker,at+marker.size())!=std::wstring::npos ||
           (at && !space(customArguments[at-1])) ||
           (at+marker.size()<customArguments.size() && !space(customArguments[at+marker.size()]))) return {};
        return customArguments.substr(0,at)+QuoteCustomShellArgument(command)+customArguments.substr(at+marker.size());
    }
    if (shell == CustomCommandShell::PowerShell || shell==CustomCommandShell::PowerShell7) {
        return std::wstring(L"-NoLogo -NoProfile ") + (hideTerminal ? L"-WindowStyle Hidden " : L"") +
               L"-EncodedCommand " + EncodePowerShellCommand(command);
    }
    // /s strips only our outer quote pair; /d disables CMD AutoRun hooks.
    return L"/d /s /c \"" + command + L"\"";
}

struct CustomCommandLaunch {
    std::wstring executable,parameters,directory;
};

bool PrepareCustomCommand(std::wstring const& command,CustomCommandShell shell,bool hideTerminal,
                           std::wstring const& customExecutable,std::wstring const& customArguments,
                           CustomCommandLaunch* launch,std::wstring* error) {
    *launch={}; error->clear();
    wchar_t system[MAX_PATH]{};
    if(!GetSystemDirectoryW(system,ARRAYSIZE(system))) {
        *error=L"Couldn't find the Windows system directory.";
        return false;
    }
    launch->executable=CustomCommandExecutable(system,shell,customExecutable);
    const DWORD attributes=launch->executable.empty() ? INVALID_FILE_ATTRIBUTES : GetFileAttributesW(launch->executable.c_str());
    if(attributes==INVALID_FILE_ATTRIBUTES || (attributes&FILE_ATTRIBUTE_DIRECTORY)) {
        *error=shell==CustomCommandShell::PowerShell7 ? L"PowerShell 7 wasn't found. Install it, or set its executable using Custom shell."
            : shell==CustomCommandShell::Custom ? L"Custom shell wasn't found. Check Shell Executable in this command's settings."
            : L"The selected shell wasn't found. Choose another shell.";
        return false;
    }
    const auto arguments=shell==CustomCommandShell::Custom ? ExpandCustomShellEnvironment(customArguments) : std::wstring();
    launch->parameters=CustomCommandParameters(command,shell,hideTerminal,arguments);
    if(launch->parameters.empty()) {
        *error=L"Shell Arguments must contain {command} once as a separate argument, without quotes around it.";
        return false;
    }
    if(launch->executable.size()+launch->parameters.size()+4>=32767) {
        *error=L"This command exceeds the Windows command-line length limit.";
        return false;
    }
    launch->directory=ExpandCustomShellEnvironment(L"%USERPROFILE%");
    if(launch->directory.empty() || launch->directory==L"%USERPROFILE%") launch->directory=system;
    return true;
}

bool CreateHiddenCommandProcess(std::wstring const& executable, std::wstring const& parameters,
                                std::wstring const& directory) {
    std::wstring commandLine = L"\"" + executable + L"\" " + parameters;
    STARTUPINFOW startup{sizeof(startup)};
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(executable.c_str(), commandLine.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, directory.c_str(), &startup, &process)) return false;
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    return true;
}

void RunCustomCommand(CustomCommandLaunch launch, bool hideTerminal, bool asAdmin) {
    SpawnTrackedLaunch([launch = std::move(launch), hideTerminal, asAdmin] {
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        try {
            const auto& executable=launch.executable;
            const auto& parameters=launch.parameters;
            const auto& directory=launch.directory;
            if (hideTerminal && !asAdmin) {
                // No console is allocated, so there is no terminal to flash
                // or to be delegated to Windows Terminal during startup.
                if (!CreateHiddenCommandProcess(executable, parameters, directory))
                    Wh_Log(L"custom command: hidden launch failed (%lu)", GetLastError());
            } else if (asAdmin || !ShellExecuteInExplorer(executable, parameters, directory)) {
                SHELLEXECUTEINFOW execute{sizeof(execute)};
                execute.fMask = SEE_MASK_NOASYNC;
                execute.lpVerb = asAdmin ? L"runas" : L"open";
                execute.lpFile = executable.c_str();
                execute.lpParameters = parameters.c_str();
                execute.lpDirectory = directory.c_str();
                execute.nShow = hideTerminal ? SW_HIDE : SW_SHOWNORMAL;
                if (!ShellExecuteExW(&execute))
                    Wh_Log(L"custom command: launch failed (%lu)", GetLastError());
            }
        } catch (...) {
            Wh_Log(L"custom command: launch failed (%08X)", static_cast<unsigned>(winrt::to_hresult()));
        }
        if (SUCCEEDED(comHr)) CoUninitialize();
    });
}


void LaunchTerminal(const std::wstring& dir, bool isPowerShell, bool asAdmin) {
    SpawnTrackedLaunch([dir, isPowerShell, asAdmin] {
        HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (isPowerShell) {
            std::wstring psTarget = dir;
            size_t pos = 0;
            while ((pos = psTarget.find(L'\'', pos)) != std::wstring::npos) {
                psTarget.insert(pos, L"'");
                pos += 2;
            }
            std::wstring params = L"-NoExit -Command Set-Location -LiteralPath '" + psTarget + L"'";
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpVerb = asAdmin ? L"runas" : L"open";
            sei.lpFile = L"powershell.exe";
            sei.lpParameters = params.c_str();
            sei.lpDirectory = dir.c_str();
            sei.nShow = SW_SHOWNORMAL;
            if (asAdmin || !ShellExecuteInExplorer(sei.lpFile, params, dir)) {
                ShellExecuteExW(&sei);
            }
        } else {
            std::wstring cmdTarget = dir;
            if (cmdTarget.size() > 3 && cmdTarget.back() == L'\\') {
                cmdTarget.pop_back();
            }
            std::wstring params = L"/K cd /d \"" + cmdTarget + (cmdTarget.back() == L'\\' ? L"\\\"" : L"\"");
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpVerb = asAdmin ? L"runas" : L"open";
            sei.lpFile = L"cmd.exe";
            sei.lpParameters = params.c_str();
            sei.lpDirectory = dir.c_str();
            sei.nShow = SW_SHOWNORMAL;
            if (asAdmin || !ShellExecuteInExplorer(sei.lpFile, params, dir)) {
                ShellExecuteExW(&sei);
            }
        }
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
    });
}

inline std::wstring UrlEncode(const std::wstring& str) {
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Len <= 0) return L"";
    std::string utf8(utf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &utf8[0], utf8Len, nullptr, nullptr);

    std::string encoded;
    const char hex[] = "0123456789ABCDEF";
    for (size_t i = 0; i < utf8.size() - 1; ++i) {
        unsigned char c = static_cast<unsigned char>(utf8[i]);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += static_cast<char>(c);
        } else if (c == ' ') {
            encoded += '+';
        } else {
            encoded += '%';
            encoded += hex[(c >> 4) & 0x0F];
            encoded += hex[c & 0x0F];
        }
    }
    return std::wstring(encoded.begin(), encoded.end());
}

struct ResolvedWebQuery {
    std::wstring serviceName;
    std::wstring queryTerm;
    std::wstring searchUrl;
    std::wstring homeUrl;
    bool isShortcut = false;
};

inline std::wstring DeriveHomeUrl(const std::wstring& urlTemplate) {
    size_t qMark = urlTemplate.find(L"?");
    if (qMark != std::wstring::npos) {
        size_t slash = urlTemplate.find(L"/", 8); // after https://
        if (slash != std::wstring::npos && slash < qMark) {
            return urlTemplate.substr(0, slash + 1);
        }
        return urlTemplate.substr(0, qMark);
    }
    return urlTemplate;
}

inline std::wstring SubstituteQuery(const std::wstring& urlTemplate, const std::wstring& query) {
    if (query.empty()) return DeriveHomeUrl(urlTemplate);
    std::wstring encoded = UrlEncode(query);
    std::wstring res = urlTemplate;
    size_t pos = res.find(L"{q}");
    if (pos != std::wstring::npos) {
        res.replace(pos, 3, encoded);
        return res;
    }
    pos = res.find(L"{searchTerms}");
    if (pos != std::wstring::npos) {
        res.replace(pos, 13, encoded);
        return res;
    }
    if (res.find(L"?") == std::wstring::npos) {
        res += L"?q=" + encoded;
    } else {
        res += L"&q=" + encoded;
    }
    return res;
}

inline std::wstring DeriveEngineName(const std::wstring& url) {
    std::wstring lower = url;
    for (auto& c : lower) c = static_cast<wchar_t>(towlower(c));
    if (lower.find(L"duckduckgo") != std::wstring::npos) return L"DuckDuckGo";
    if (lower.find(L"google") != std::wstring::npos) return L"Google";
    if (lower.find(L"bing") != std::wstring::npos) return L"Bing";
    if (lower.find(L"brave") != std::wstring::npos) return L"Brave";
    if (lower.find(L"yahoo") != std::wstring::npos) return L"Yahoo";
    if (lower.find(L"ecosia") != std::wstring::npos) return L"Ecosia";
    if (lower.find(L"kagi") != std::wstring::npos) return L"Kagi";
    if (lower.find(L"startpage") != std::wstring::npos) return L"Startpage";
    return L"Web";
}

inline ResolvedWebQuery ResolveWebSearch(const std::wstring& input) {
    ResolvedWebQuery res;
    std::wstring text = input;
    while (!text.empty() && text.front() == L' ') text.erase(0, 1);

    std::vector<WebShortcut> shortcuts;
    std::wstring defUrl;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        shortcuts = g_settings.webShortcuts;
        defUrl = g_settings.defaultSearchUrl;
    }
    if (defUrl.empty()) defUrl = L"https://duckduckgo.com/?q={q}";

    // Check if first token matches any shortcut
    size_t delim = text.find_first_of(L" :");
    std::wstring firstWord = (delim != std::wstring::npos) ? text.substr(0, delim) : text;
    std::wstring firstWordLower = firstWord;
    for (auto& c : firstWordLower) c = static_cast<wchar_t>(towlower(c));

    for (const auto& sc : shortcuts) {
        if (!firstWordLower.empty() && firstWordLower == sc.prefix) {
            res.isShortcut = true;
            res.serviceName = sc.name;
            std::wstring remainder = (delim != std::wstring::npos) ? text.substr(delim + 1) : L"";
            while (!remainder.empty() && remainder.front() == L' ') remainder.erase(0, 1);
            res.queryTerm = remainder;
            res.homeUrl = DeriveHomeUrl(sc.url);
            res.searchUrl = SubstituteQuery(sc.url, res.queryTerm);
            return res;
        }
    }

    // Default search engine (DuckDuckGo or user configured in settings)
    res.isShortcut = false;
    res.serviceName = DeriveEngineName(defUrl);
    res.queryTerm = text;
    res.homeUrl = DeriveHomeUrl(defUrl);
    res.searchUrl = SubstituteQuery(defUrl, text);
    return res;
}

[[clang::no_destroy]] wuxc::TextBlock g_footerStatus{nullptr};
[[clang::no_destroy]] wuxc::StackPanel g_footerHints{nullptr};

void QueueQuery(std::wstring text);

std::vector<Row> g_currentAppRows;
std::vector<Row> g_currentFileRows;
static int g_selectedApp = -1;
static int g_selectedFile = -1;
// Which column Up, Down and Enter act on.
static bool g_filesColumnActive = false;
static uint64_t g_lastNavTick = 0;

// Highlights the selected button of a column and clears the rest; -1 clears
// them all.
void PaintColumnSelection(const std::optional<std::vector<wuxc::Button>>& buttons, int selected) {
    if (!buttons) {
        return;
    }
    bool isLight = IsLightTheme();
    for (size_t i = 0; i < buttons->size(); ++i) {
        const auto& btn = (*buttons)[i];
        if (!btn) continue;

        if (static_cast<int>(i) == selected) {
            if (isLight) {
                btn.Background(MakeBrush(0x14, 0x00, 0x5F, 0xB8));
                btn.BorderBrush(MakeBrush(0x80, 0x00, 0x5F, 0xB8));
            } else {
                btn.Background(MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
                btn.BorderBrush(MakeBrush(0x55, 0x60, 0xCD, 0xFF));
            }
            try {
                btn.StartBringIntoView();
            } catch (...) {}
        } else {
            btn.Background(MakeBrush(0, 0, 0, 0));
            btn.BorderBrush(MakeBrush(0, 0, 0, 0));
        }
    }
}

void SchedulePreview();

// Selects an app card, which makes Apps the active column.
void SetAppSelection(int index) {
    if (!g_appButtonsOpt || g_appButtonsOpt->empty()) {
        g_selectedApp = -1;
        return;
    }
    if (index < 0) index = 0;
    if (index >= static_cast<int>(g_appButtonsOpt->size())) {
        index = static_cast<int>(g_appButtonsOpt->size()) - 1;
    }
    g_selectedApp = index;
    g_filesColumnActive = false;
    PaintColumnSelection(g_appButtonsOpt, index);
    PaintColumnSelection(g_fileButtonsOpt, -1);
    SchedulePreview();
}

// Selects a file row, which makes Files the active column.
void SetFileSelection(int index) {
    if(!FilesPanelVisible()) return;
    if (!g_fileButtonsOpt || g_fileButtonsOpt->empty()) {
        g_selectedFile = -1;
        return;
    }
    if (index < 0) index = 0;
    if (index >= static_cast<int>(g_fileButtonsOpt->size())) {
        index = static_cast<int>(g_fileButtonsOpt->size()) - 1;
    }
    g_selectedFile = index;
    g_filesColumnActive = true;
    PaintColumnSelection(g_fileButtonsOpt, index);
    PaintColumnSelection(g_appButtonsOpt, -1);
    SchedulePreview();
}

// ---------------------------------------------------------------------------
// File preview: a card beside Start for the selected file
//
// A large thumbnail and a few details -- size, date, and for pictures and
// videos their dimensions and length -- for telling similar names apart. The
// thumbnail is the one File Explorer shows: whatever the shell has for the
// type (pictures, a frame of a video, documents, anything an installed app
// provides one for), else the type's icon. So no type needs code here -- but
// PDFs, which Windows gives no thumbnail of its own: their first page is
// drawn by Windows' own PDF renderer (RenderPdfPage), so the preview does not
// depend on a PDF app being installed.
//
// An SVG with no thumbnail is drawn by XAML's own renderer (OpenSvg).
// A file with no picture shows the start of its text instead, when it has
// some: a text file -- any, by what is in it -- line for line
// (ReadTextStart), or a Word, Excel, PowerPoint, RTF or OpenDocument file
// through Windows' or Office's own document filters (ReadDocumentText),
// which run here, in Start.
//
// Motion plays over the still once it is ready (animatePreview): a video,
// muted and looping, through XAML's own player; a GIF, which XAML animates
// itself; an animated WebP, whose frames Windows' WebP decoder hands over
// whole but without their timing, which is read from the file
// (WebpFrameDurations); a PDF's first pages, a second each. Never sound. All
// of it stops and is let go the
// moment the selection moves or Start closes.
//
// The card is a Popup beside the search window, using the monitor's work area
// independently of the window height. It can extend above a short search
// window without changing any native window region. It follows the selection
// after a short pause, so arrowing through results does not flash a card per
// row. Thumbnails are fetched on a thread of their own: one can take a few
// hundred milliseconds, more the first time a video is seen.
// ---------------------------------------------------------------------------

namespace preview {

inline constexpr double kWidth = 300;       // the card, in DIPs
inline constexpr double kThumbWidth = 276;  // the thumbnail's box inside it
inline constexpr double kThumbHeight = 400;  // a portrait photo at the full width

// A file with no thumbnail shows its icon as large as a folder's thumbnail:
// up to 256 pixels, the largest size icons are made at.
inline int TypeIconPixels(double scale) {
    return std::min(256, static_cast<int>(kThumbWidth * scale));
}

// What the thread found for a path.
struct Result {
    std::wstring path;
    std::vector<BYTE> pixels;  // premultiplied BGRA, width x height; empty if none
    int width = 0;
    int height = 0;
    double scale = 1.0;        // physical pixels per DIP when it was made
    int pages = 0;             // a PDF drawn by RenderPdfPage: how many pages
    bool morePages = false;    // and its next pages are to follow (RenderPdfFrames)
    std::wstring name;
    std::vector<std::pair<std::wstring, std::wstring>> facts;  // label and value, in the order shown
    std::wstring text;       // the start of a text file or a document, shown where a picture would be
    bool monospace = false;  // the text is a text file's, line for line
    // Motion over the still (see the module comment). At most one is set.
    winrt::Windows::Storage::Streams::IRandomAccessStream gif{nullptr};
    winrt::Windows::Storage::Streams::IRandomAccessStream svg{nullptr};  // drawn by XAML (OpenSvg)
    double svgAspect = 1.0;                                               // its height over its width
    winrt::Windows::Storage::StorageFile video{nullptr};
    std::vector<std::vector<BYTE>> frames;  // an animated WebP, each like pixels
    std::vector<int> delays;                // milliseconds per frame
};

std::mutex g_mutex;
std::condition_variable g_wake;
std::wstring g_request;  // the path to fetch next, or empty
bool g_quit = false;
[[clang::no_destroy]] std::optional<Result> g_result;  // the last fetched, for the XAML thread
[[clang::no_destroy]] std::optional<Result> g_frames;  // a PDF's pages after its card (ApplyPreviewFrames)
[[clang::no_destroy]] std::optional<std::thread> g_thread;

// Properties, defined here: propkey.h would add every key there is to the
// DLL (see kLinkTargetParsingPath). System.Image.HorizontalSize and
// VerticalSize, System.Video.FrameWidth and FrameHeight, System.Media.Duration,
// then those of the details (Fetch): System.ItemTypeText,
// System.Video.FrameRate, System.Audio.EncodingBitrate, System.Music.Artist
// and AlbumTitle, System.Photo.DateTaken and CameraModel,
// System.FileDescription, FileVersion and Company, System.Title and Author.
inline constexpr PROPERTYKEY kImageWidth{{0x6444048F, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 3};
inline constexpr PROPERTYKEY kImageHeight{{0x6444048F, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 4};
inline constexpr PROPERTYKEY kVideoWidth{{0x64440491, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 3};
inline constexpr PROPERTYKEY kVideoHeight{{0x64440491, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 4};
inline constexpr PROPERTYKEY kDuration{{0x64440490, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 3};
inline constexpr PROPERTYKEY kItemTypeText{{0xB725F130, 0x47EF, 0x101A, {0xA5, 0xF1, 0x02, 0x60, 0x8C, 0x9E, 0xEB, 0xAC}}, 4};
inline constexpr PROPERTYKEY kFrameRate{{0x64440491, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 6};
inline constexpr PROPERTYKEY kBitrate{{0x64440490, 0x4C8B, 0x11D1, {0x8B, 0x70, 0x08, 0x00, 0x36, 0xB1, 0x1A, 0x03}}, 4};
inline constexpr PROPERTYKEY kArtist{{0x56A3372E, 0xCE9C, 0x11D2, {0x9F, 0x0E, 0x00, 0x60, 0x97, 0xC6, 0x86, 0xF6}}, 2};
inline constexpr PROPERTYKEY kAlbum{{0x56A3372E, 0xCE9C, 0x11D2, {0x9F, 0x0E, 0x00, 0x60, 0x97, 0xC6, 0x86, 0xF6}}, 4};
inline constexpr PROPERTYKEY kDateTaken{{0x14B81DA1, 0x0135, 0x4D31, {0x96, 0xD9, 0x6C, 0xBF, 0xC9, 0x67, 0x1A, 0x99}}, 36867};
inline constexpr PROPERTYKEY kCameraModel{{0x14B81DA1, 0x0135, 0x4D31, {0x96, 0xD9, 0x6C, 0xBF, 0xC9, 0x67, 0x1A, 0x99}}, 272};
inline constexpr PROPERTYKEY kFileDescription{{0x0CEF7D53, 0xFA64, 0x11D1, {0xA2, 0x03, 0x00, 0x00, 0xF8, 0x1F, 0xED, 0xEE}}, 3};
inline constexpr PROPERTYKEY kFileVersion{{0x0CEF7D53, 0xFA64, 0x11D1, {0xA2, 0x03, 0x00, 0x00, 0xF8, 0x1F, 0xED, 0xEE}}, 4};
inline constexpr PROPERTYKEY kCompany{{0xD5CDD502, 0x2E9C, 0x101B, {0x93, 0x97, 0x08, 0x00, 0x2B, 0x2C, 0xF9, 0xAE}}, 15};
inline constexpr PROPERTYKEY kTitle{{0xF29F85E0, 0x4FF9, 0x1068, {0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9}}, 2};
inline constexpr PROPERTYKEY kAuthor{{0xF29F85E0, 0x4FF9, 0x1068, {0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9}}, 4};

// A property as text: a string, or a list of them joined (artists, authors).
inline std::wstring PropertyText(IShellItem2* item, REFPROPERTYKEY key) {
    PROPVARIANT value;
    PropVariantInit(&value);
    std::wstring text;
    if (SUCCEEDED(item->GetProperty(key, &value))) {
        if (value.vt == VT_LPWSTR && value.pwszVal) {
            text = value.pwszVal;
        } else if (value.vt == (VT_VECTOR | VT_LPWSTR)) {
            for (ULONG i = 0; i < value.calpwstr.cElems; ++i) {
                if (value.calpwstr.pElems[i] && *value.calpwstr.pElems[i]) {
                    text += (text.empty() ? L"" : L"; ");
                    text += value.calpwstr.pElems[i];
                }
            }
        }
        PropVariantClear(&value);
    }
    while (!text.empty() && iswspace(text.back())) {
        text.pop_back();
    }
    return text;
}

inline std::wstring ByteSize(ULONGLONG bytes) {
    wchar_t text[32] = {};
    StrFormatByteSizeW(static_cast<LONGLONG>(bytes), text, ARRAYSIZE(text));
    return text;
}

inline std::wstring ItemCount(DWORD count, bool more) {
    return std::to_wstring(count) + (more ? L"+" : L"") + (count == 1 && !more ? L" item" : L" items");
}

// A folder's size and how many items it holds directly, from Everything's
// index: one query each, answered in a few milliseconds. The size only when
// Everything keeps folder sizes (its "Index folder size" option); false when
// Everything does not know the folder.
inline bool FolderFacts(everything::Client* client, const std::wstring& path, ULONGLONG* size, bool* sized,
                        DWORD* items) {
    const size_t slash = path.find_last_of(L'\\');
    if (!client || slash == std::wstring::npos || slash + 1 >= path.size()) {
        return false;  // a drive: not a folder to Everything
    }
    std::wstring parent = path.substr(0, slash);
    if (parent.size() == 2) {
        parent += L'\\';  // "C:" is "C:\"
    }
    std::vector<everything::Result> found;
    DWORD total = 0;
    if (!client->Query(L"folder: parent:\"" + parent + L"\" wfn:\"" + path.substr(slash + 1) + L"\"", 1, &found, &total,
                       300, everything::kReqName | everything::kReqPath | everything::kReqSize) ||
        found.empty()) {
        return false;
    }
    *sized = found[0].size != ~0ULL;  // what Everything answers when it keeps no folder sizes
    *size = found[0].size;
    std::vector<everything::Result> children;
    return client->Query(L"parent:\"" + path + L"\"", 1, &children, items, 300, everything::kReqName);
}

// How many items a folder holds directly, counted here: when Everything
// does not know it. Up to 10000.
inline DWORD CountItems(const std::wstring& path, bool* more) {
    DWORD count = 0;
    *more = false;
    WIN32_FIND_DATAW data;
    HANDLE find = FindFirstFileExW((path + L"\\*").c_str(), FindExInfoBasic, &data, FindExSearchNameMatch, nullptr,
                                   FIND_FIRST_EX_LARGE_FETCH);
    if (find == INVALID_HANDLE_VALUE) {
        return 0;
    }
    do {
        if (wcscmp(data.cFileName, L".") && wcscmp(data.cFileName, L"..")) {
            if (++count >= 10000) {
                *more = true;
                break;
            }
        }
    } while (FindNextFileW(find, &data));
    FindClose(find);
    return count;
}

// A thumbnail or a property can make the shell read the file, which on a
// network or removable drive can stall for seconds. Those get the name and
// folder only.
inline bool OnFixedDrive(const std::wstring& path) {
    if (path.size() < 3 || path[1] != L':') {
        return false;
    }
    const wchar_t root[] = {path[0], L':', L'\\', 0};
    return GetDriveTypeW(root) == DRIVE_FIXED;
}

inline std::wstring FormatDuration(ULONGLONG hundredNs) {
    const ULONGLONG total = hundredNs / 10000000ULL;
    wchar_t text[32];
    if (total >= 3600) {
        swprintf_s(text, L"%llu:%02llu:%02llu", total / 3600, (total / 60) % 60, total % 60);
    } else {
        swprintf_s(text, L"%llu:%02llu", total / 60, total % 60);
    }
    return text;
}

inline std::wstring FormatDate(const FILETIME& ft) {
    SYSTEMTIME utc{}, local{};
    if (!FileTimeToSystemTime(&ft, &utc) || !SystemTimeToTzSpecificLocalTime(nullptr, &utc, &local)) {
        return L"";
    }
    wchar_t date[64] = {}, time[64] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &local, nullptr, date, 64, nullptr);
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &local, nullptr, time, 64);
    return std::wstring(date) + L" " + time;
}

// Runs f on a thread of its own in the multithreaded apartment and waits for
// it: f waits for WinRT operations, which must not be done in the preview
// thread's single-threaded one.
template <typename F>
inline void RunInMta(F&& f) {
    std::thread worker([&] {
        HRESULT co = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        try {
            f();
        } catch (...) {
        }
        if (SUCCEEDED(co)) {
            CoUninitialize();
        }
    });
    worker.join();
}

// Use Windows' built-in codecs when Explorer has no usable thumbnail. This
// path does not depend on file associations, shell extensions or image apps.
inline bool DecodeNativeImage(const std::wstring& path, int width, int height, Result* r) {
    if(width<=0 || height<=0) return false;
    namespace imaging = winrt::Windows::Graphics::Imaging;
    const auto lower=tools::ToLower(path);
    const auto dot=lower.find_last_of(L'.');
    if(dot==std::wstring::npos) return false;
    const auto ext=std::wstring_view(lower).substr(dot);
    winrt::guid codec{};
    if(ext==L".png") codec=imaging::BitmapDecoder::PngDecoderId();
    else if(ext==L".jpg" || ext==L".jpeg" || ext==L".jpe" || ext==L".jfif") codec=imaging::BitmapDecoder::JpegDecoderId();
    else if(ext==L".bmp" || ext==L".dib") codec=imaging::BitmapDecoder::BmpDecoderId();
    else if(ext==L".gif") codec=imaging::BitmapDecoder::GifDecoderId();
    else if(ext==L".tif" || ext==L".tiff") codec=imaging::BitmapDecoder::TiffDecoderId();
    else return false;
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if(!GetFileAttributesExW(path.c_str(),GetFileExInfoStandard,&attributes) ||
       (attributes.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) || attributes.nFileSizeHigh ||
       attributes.nFileSizeLow>256u*1024*1024) return false;
    bool ok=false;
    RunInMta([&] {
        auto file=winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
        auto decoder=imaging::BitmapDecoder::CreateAsync(codec,file.OpenReadAsync().get()).get();
        const uint32_t w=decoder.PixelWidth(),h=decoder.PixelHeight();
        const uint32_t orientedW=decoder.OrientedPixelWidth(),orientedH=decoder.OrientedPixelHeight();
        if(!w || !h || !orientedW || !orientedH || w>100000 || h>100000 || uint64_t(w)*h>200000000) return;
        const double fit=std::min({1.0,std::clamp(width,1,2048)/double(orientedW),
                                       std::clamp(height,1,2048)/double(orientedH)});
        // Scaling precedes EXIF rotation; output dimensions follow orientation.
        const uint32_t scaledW=std::max(1u,static_cast<uint32_t>(w*fit));
        const uint32_t scaledH=std::max(1u,static_cast<uint32_t>(h*fit));
        const bool swapped=orientedW!=w;
        const int outW=static_cast<int>(swapped ? scaledH : scaledW);
        const int outH=static_cast<int>(swapped ? scaledW : scaledH);
        imaging::BitmapTransform transform;
        transform.ScaledWidth(scaledW); transform.ScaledHeight(scaledH);
        transform.InterpolationMode(imaging::BitmapInterpolationMode::Fant);
        auto data=decoder.GetPixelDataAsync(imaging::BitmapPixelFormat::Bgra8,imaging::BitmapAlphaMode::Premultiplied,
            transform,imaging::ExifOrientationMode::RespectExifOrientation,imaging::ColorManagementMode::ColorManageToSRgb).get();
        auto bytes=data.DetachPixelData();
        if(bytes.size()!=static_cast<size_t>(outW)*outH*4) return;
        std::vector<BYTE> pixels(bytes.begin(),bytes.end());
        r->pixels=std::move(pixels); r->width=outW; r->height=outH;
        ok=true;
    });
    return ok;
}

// How many of a PDF's pages the preview turns through, and for how long each.
inline constexpr int kPdfPages = 10;
inline constexpr int kPdfPageMs = 1000;

// A page of a PDF, fitted into width x height pixels, by Windows' own
// renderer (Windows.Data.Pdf). In the multithreaded apartment (RunInMta).
inline bool RenderPdfPageOf(winrt::Windows::Data::Pdf::PdfDocument const& document, uint32_t index, int width, int height,
                            std::vector<BYTE>* pixels, int* w, int* h) {
    namespace pdf = winrt::Windows::Data::Pdf;
    namespace imaging = winrt::Windows::Graphics::Imaging;
    auto page = document.GetPage(index);
    const auto size = page.Size();
    const double fit = std::min(width / std::max(1.0, static_cast<double>(size.Width)),
                                height / std::max(1.0, static_cast<double>(size.Height)));
    pdf::PdfPageRenderOptions options;
    options.DestinationWidth(static_cast<uint32_t>(std::max(1.0, size.Width * fit)));
    options.DestinationHeight(static_cast<uint32_t>(std::max(1.0, size.Height * fit)));
    winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
    page.RenderToStreamAsync(stream, options).get();
    auto decoder = imaging::BitmapDecoder::CreateAsync(stream).get();
    auto data = decoder.GetPixelDataAsync(imaging::BitmapPixelFormat::Bgra8, imaging::BitmapAlphaMode::Premultiplied,
                                          imaging::BitmapTransform(), imaging::ExifOrientationMode::IgnoreExifOrientation,
                                          imaging::ColorManagementMode::DoNotColorManage)
                    .get();
    auto bytes = data.DetachPixelData();
    *w = static_cast<int>(decoder.PixelWidth());
    *h = static_cast<int>(decoder.PixelHeight());
    if (*w <= 0 || *h <= 0 || bytes.size() != static_cast<size_t>(*w) * *h * 4) {
        return false;
    }
    pixels->assign(bytes.begin(), bytes.end());
    return true;
}

// A PDF's first page, fitted into width x height pixels, and how many pages it
// has. The card shows it at once; the next pages follow (RenderPdfFrames).
// Fails on an encrypted PDF, for one.
inline bool RenderPdfPage(const std::wstring& path, int width, int height, Result* r) {
    bool ok = false;
    RunInMta([&] {
        auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
        auto document = winrt::Windows::Data::Pdf::PdfDocument::LoadFromFileAsync(file).get();
        if (document.PageCount() > 0 && RenderPdfPageOf(document, 0, width, height, &r->pixels, &r->width, &r->height)) {
            r->pages = static_cast<int>(document.PageCount());
            ok = true;
        }
    });
    return ok;
}

// A PDF's first pages, up to kPdfPages, as frames the size of its first page
// (still), a page of another shape centered: shown a second each once they
// are ready. Gives up when stop() says the selection has moved.
template <typename Stop>
inline void RenderPdfFrames(const std::wstring& path, const Result& still, Stop&& stop, std::vector<std::vector<BYTE>>* frames,
                            std::vector<int>* delays) {
    RunInMta([&] {
        auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
        auto document = winrt::Windows::Data::Pdf::PdfDocument::LoadFromFileAsync(file).get();
        const int count = std::min(static_cast<int>(document.PageCount()), kPdfPages);
        frames->push_back(still.pixels);
        delays->push_back(kPdfPageMs);
        for (int i = 1; i < count && !stop(); ++i) {
            std::vector<BYTE> page;
            int w = 0, h = 0;
            if (!RenderPdfPageOf(document, static_cast<uint32_t>(i), still.width, still.height, &page, &w, &h)) {
                break;
            }
            std::vector<BYTE> frame(still.pixels.size(), 0);
            const int cw = std::min(w, still.width), ch = std::min(h, still.height);
            const int left = (still.width - cw) / 2, top = (still.height - ch) / 2;
            for (int y = 0; y < ch; ++y) {
                memcpy(&frame[(static_cast<size_t>(top + y) * still.width + left) * 4], &page[static_cast<size_t>(y) * w * 4],
                       static_cast<size_t>(cw) * 4);
            }
            frames->push_back(std::move(frame));
            delays->push_back(kPdfPageMs);
        }
    });
    if (frames->size() < 2 || stop()) {
        frames->clear();
        delays->clear();
    }
}

// How long each frame of an animated WebP shows, in milliseconds, from the
// file's ANMF chunks: Windows' decoder hands over the frames without it.
inline std::vector<int> WebpFrameDurations(const std::wstring& path) {
    std::vector<int> durations;
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, 0, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return durations;
    }
    BYTE header[12] = {};
    DWORD read = 0;
    if (ReadFile(file, header, sizeof(header), &read, nullptr) && read == sizeof(header) &&
        memcmp(header, "RIFF", 4) == 0 && memcmp(header + 8, "WEBP", 4) == 0) {
        LARGE_INTEGER pos{};
        pos.QuadPart = 12;
        for (int chunks = 0; chunks < 100000; ++chunks) {
            BYTE chunk[8 + 16] = {};  // the chunk header, and an ANMF's fields
            if (!SetFilePointerEx(file, pos, nullptr, FILE_BEGIN) || !ReadFile(file, chunk, sizeof(chunk), &read, nullptr) ||
                read < 8) {
                break;
            }
            const uint32_t size = chunk[4] | (chunk[5] << 8) | (chunk[6] << 16) | (static_cast<uint32_t>(chunk[7]) << 24);
            if (memcmp(chunk, "ANMF", 4) == 0 && read >= 8 + 15) {
                durations.push_back(chunk[8 + 12] | (chunk[8 + 13] << 8) | (chunk[8 + 14] << 16));
            }
            pos.QuadPart += 8 + static_cast<LONGLONG>(size) + (size & 1);
        }
    }
    CloseHandle(file);
    return durations;
}

// An animated WebP's frames, fitted into width x height pixels -- the first
// becomes the still. Leaves r alone for a still WebP, or one too large to keep
// in memory.
inline void DecodeAnimatedWebp(const std::wstring& path, int width, int height, Result* r) {
    constexpr size_t kMaxFrames = 300;
    constexpr size_t kMaxBytes = 64 << 20;
    RunInMta([&] {
        namespace imaging = winrt::Windows::Graphics::Imaging;
        auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
        auto decoder = imaging::BitmapDecoder::CreateAsync(file.OpenReadAsync().get()).get();
        const uint32_t count = decoder.FrameCount();
        const double fit = std::min({1.0, width / std::max(1.0, static_cast<double>(decoder.PixelWidth())),
                                     height / std::max(1.0, static_cast<double>(decoder.PixelHeight()))});
        const uint32_t w = std::max(1u, static_cast<uint32_t>(decoder.PixelWidth() * fit));
        const uint32_t h = std::max(1u, static_cast<uint32_t>(decoder.PixelHeight() * fit));
        if (count < 2 || count > kMaxFrames || static_cast<size_t>(count) * w * h * 4 > kMaxBytes) {
            return;
        }
        imaging::BitmapTransform transform;
        transform.ScaledWidth(w);
        transform.ScaledHeight(h);
        transform.InterpolationMode(imaging::BitmapInterpolationMode::Fant);
        std::vector<std::vector<BYTE>> frames;
        for (uint32_t i = 0; i < count; ++i) {
            auto frame = decoder.GetFrameAsync(i).get();
            auto data = frame.GetPixelDataAsync(imaging::BitmapPixelFormat::Bgra8, imaging::BitmapAlphaMode::Premultiplied,
                                                transform, imaging::ExifOrientationMode::IgnoreExifOrientation,
                                                imaging::ColorManagementMode::DoNotColorManage)
                            .get();
            auto bytes = data.DetachPixelData();
            if (bytes.size() != static_cast<size_t>(w) * h * 4) {
                return;
            }
            frames.emplace_back(bytes.begin(), bytes.end());
        }
        std::vector<int> delays = WebpFrameDurations(path);
        delays.resize(frames.size(), 100);
        for (int& delay : delays) {
            delay = delay < 20 ? 100 : delay;  // as browsers treat 0 and near-0
        }
        r->pixels = frames.front();
        r->width = static_cast<int>(w);
        r->height = static_cast<int>(h);
        r->frames = std::move(frames);
        r->delays = std::move(delays);
    });
}

// Opens what the card plays over the still, if anything: a GIF for XAML to
// animate, a video for its player, an animated WebP's frames.
inline void FetchMotion(const std::wstring& path, int width, int height, Result* r) {
    const size_t dot = r->name.rfind(L'.');
    if (dot == std::wstring::npos) {
        return;
    }
    const std::wstring ext = tools::ToLower(r->name.substr(dot));
    PERCEIVED type = PERCEIVED_TYPE_UNSPECIFIED;
    PERCEIVEDFLAG flags = 0;
    if (ext == L".gif") {
        RunInMta([&] {
            auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
            r->gif = file.OpenReadAsync().get();
        });
    } else if (ext == L".webp") {
        DecodeAnimatedWebp(path, width, height, r);
    } else if (SUCCEEDED(AssocGetPerceivedType(ext.c_str(), &type, &flags, nullptr)) && type == PERCEIVED_TYPE_VIDEO) {
        RunInMta([&] { r->video = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get(); });
    }
}

// An SVG, for XAML's own renderer (SvgImageSource) to draw on the card, and
// its shape: from the viewBox of its <svg> element, else its width and
// height, else square.
inline bool OpenSvg(const std::wstring& path, Result* r) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    char bytes[4096];
    DWORD read = 0;
    ReadFile(file, bytes, sizeof(bytes) - 1, &read, nullptr);
    CloseHandle(file);
    bytes[read] = 0;
    const char* tag = strstr(bytes, "<svg");
    if (!tag) {
        return false;
    }
    const char* end = strchr(tag, '>');
    const std::string svgTag(tag, end ? end : bytes + read);
    auto attribute = [&](const char* name) -> std::string {
        const std::string key = std::string(" ") + name + "=";
        size_t at = svgTag.find(key);
        if (at == std::string::npos) {
            at = svgTag.find(std::string("\n") + name + "=");
        }
        if (at == std::string::npos || at + key.size() >= svgTag.size()) {
            return {};
        }
        const char quote = svgTag[at + key.size()];
        const size_t start = at + key.size() + 1;
        const size_t close = svgTag.find(quote, start);
        return close == std::string::npos ? std::string() : svgTag.substr(start, close - start);
    };
    double aspect = 0;
    const std::string viewBox = attribute("viewBox");
    if (!viewBox.empty()) {
        double box[4] = {};
        const char* cursor = viewBox.c_str();
        for (double& value : box) {
            char* next = nullptr;
            value = strtod(cursor, &next);
            cursor = next;
            while (*cursor == ',' || *cursor == ' ') {
                ++cursor;
            }
        }
        if (box[2] > 0 && box[3] > 0) {
            aspect = box[3] / box[2];
        }
    }
    if (!aspect) {
        const double width = strtod(attribute("width").c_str(), nullptr);
        const double height = strtod(attribute("height").c_str(), nullptr);
        aspect = width > 0 && height > 0 ? height / width : 1;
    }
    r->svgAspect = std::clamp(aspect, 0.2, 5.0);
    RunInMta([&] {
        r->svg = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get().OpenReadAsync().get();
    });
    return r->svg != nullptr;
}

// The start of a text file -- any: by what is in it, not its name -- as up to
// 40 lines: UTF-16 or UTF-8 by its byte order mark, else UTF-8 when it is,
// else the system's code page. Nothing when it is not text: a NUL, or too
// many other control characters, in the first 8 KB.
inline bool ReadTextStart(const std::wstring& path, std::wstring* text) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                              OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    char bytes[8192];
    DWORD read = 0;
    const bool ok = ReadFile(file, bytes, sizeof(bytes), &read, nullptr) && read > 0;
    CloseHandle(file);
    if (!ok) {
        return false;
    }
    std::wstring decoded;
    auto decode = [&](UINT codePage, const char* from, int length, DWORD flags) {
        const int count = MultiByteToWideChar(codePage, flags, from, length, nullptr, 0);
        if (count <= 0) {
            return false;
        }
        decoded.resize(count);
        MultiByteToWideChar(codePage, flags, from, length, decoded.data(), count);
        return true;
    };
    if (read >= 2 && static_cast<BYTE>(bytes[0]) == 0xFF && static_cast<BYTE>(bytes[1]) == 0xFE) {
        decoded.assign(reinterpret_cast<const wchar_t*>(bytes + 2), (read - 2) / 2);
    } else {
        const int skip = read >= 3 && static_cast<BYTE>(bytes[0]) == 0xEF && static_cast<BYTE>(bytes[1]) == 0xBB &&
                                 static_cast<BYTE>(bytes[2]) == 0xBF
                             ? 3
                             : 0;
        if (memchr(bytes, 0, read)) {
            return false;
        }
        // A character cut off at the end of what was read is not an error.
        int length = static_cast<int>(read) - skip;
        bool utf8 = false;
        for (int cut = 0; cut < 4 && length - cut > 0 && !utf8; ++cut) {
            utf8 = decode(CP_UTF8, bytes + skip, length - cut, MB_ERR_INVALID_CHARS);
        }
        if (!utf8 && !decode(CP_ACP, bytes + skip, length, 0)) {
            return false;
        }
    }
    size_t control = 0;
    for (wchar_t ch : decoded) {
        if (ch == 0) {
            return false;
        }
        control += ch < 0x20 && ch != L'\t' && ch != L'\r' && ch != L'\n' && ch != L'\f';
    }
    if (decoded.empty() || control * 20 > decoded.size()) {
        return false;
    }
    // Line for line: tabs as four spaces, long lines cut.
    std::wstring out;
    int lines = 0;
    size_t column = 0;
    for (size_t i = 0; i < decoded.size() && lines < 40; ++i) {
        const wchar_t ch = decoded[i];
        if (ch == L'\r') {
            continue;
        }
        if (ch == L'\n') {
            out += L'\n';
            ++lines;
            column = 0;
        } else if (column < 200) {
            if (ch == L'\t') {
                out.append(4, L' ');
                column += 4;
            } else if (ch >= 0x20) {
                out += ch;
                ++column;
            }
        }
    }
    while (!out.empty() && (out.back() == L'\n' || out.back() == L' ')) {
        out.pop_back();
    }
    if (out.empty()) {
        return false;
    }
    *text = std::move(out);
    return true;
}

// The document filter (IFilter) registered for a file name's extension, as
// LoadIFilter finds it: its class, if its DLL is Windows' own (System32) or
// Microsoft Office's. Those are what the types in ReadDocumentText have; no
// other is loaded into Start.
inline bool TrustedDocumentFilter(const std::wstring& extension, CLSID* filter) {
    auto read = [](const std::wstring& key) {
        wchar_t value[MAX_PATH * 2];
        DWORD size = sizeof(value);
        if (RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), nullptr, RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, nullptr, value,
                         &size) != ERROR_SUCCESS) {
            return std::wstring();
        }
        wchar_t expanded[MAX_PATH * 2];
        const DWORD n = ExpandEnvironmentStringsW(value, expanded, ARRAYSIZE(expanded));
        return std::wstring(n && n <= ARRAYSIZE(expanded) ? expanded : value);
    };
    std::wstring handler = read(extension + L"\\PersistentHandler");
    if (handler.empty()) {
        const std::wstring progId = read(extension);
        const std::wstring clsid = progId.empty() ? std::wstring() : read(progId + L"\\CLSID");
        handler = clsid.empty() ? std::wstring() : read(L"CLSID\\" + clsid + L"\\PersistentHandler");
    }
    const std::wstring filterClsid =
        handler.empty()
            ? std::wstring()
            : read(L"CLSID\\" + handler + L"\\PersistentAddinsRegistered\\{89BCB740-6119-101A-BCB7-00DD010655AF}");
    const std::wstring dll = filterClsid.empty() ? std::wstring() : read(L"CLSID\\" + filterClsid + L"\\InprocServer32");
    if (dll.empty() || FAILED(CLSIDFromString(filterClsid.c_str(), filter))) {
        return false;
    }
    wchar_t system[MAX_PATH];
    const UINT n = GetSystemDirectoryW(system, MAX_PATH);
    if (n && n < MAX_PATH && uninstall::IsUnder(dll, system)) {
        return true;
    }
    static const std::pair<const KNOWNFOLDERID*, const wchar_t*> kOffice[] = {
        {&FOLDERID_ProgramFiles, L"\\Microsoft Office"},
        {&FOLDERID_ProgramFilesX86, L"\\Microsoft Office"},
        {&FOLDERID_ProgramFilesCommon, L"\\Microsoft Shared\\Filters"},
        {&FOLDERID_ProgramFilesCommonX86, L"\\Microsoft Shared\\Filters"},
    };
    for (const auto& [id, under] : kOffice) {
        PWSTR folder = nullptr;
        bool inside = false;
        if (SUCCEEDED(SHGetKnownFolderPath(*id, KF_FLAG_DONT_VERIFY, nullptr, &folder)) && folder) {
            inside = uninstall::IsUnder(dll, std::wstring(folder) + under);
        }
        CoTaskMemFree(folder);
        if (inside) {
            return true;
        }
    }
    return false;
}

// The start of a document's text -- Word, Excel, PowerPoint, RTF,
// OpenDocument -- through the document filter Windows' own search reads it
// with (TrustedDocumentFilter): Windows has them for these, Office or not.
// They run here, in Start, so: only Windows' or Office's, up to 30 MB, the
// first 1500 characters.
inline bool ReadDocumentText(const std::wstring& path, const std::wstring& lowerName, std::wstring* text) {
    static const wchar_t* const kDocuments[] = {L".doc", L".docx", L".docm", L".dot", L".dotx", L".rtf",
                                                L".odt", L".xls", L".xlsx", L".xlsm", L".ppt", L".pptx",
                                                L".pptm", L".ods", L".odp"};
    const size_t dot = lowerName.rfind(L'.');
    if (dot == std::wstring::npos ||
        std::none_of(std::begin(kDocuments), std::end(kDocuments),
                     [&](const wchar_t* ext) { return lowerName.compare(dot, std::wstring::npos, ext) == 0; })) {
        return false;
    }
    WIN32_FILE_ATTRIBUTE_DATA attributes{};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes) || attributes.nFileSizeHigh ||
        attributes.nFileSizeLow > 30u * 1024 * 1024) {
        return false;
    }
    CLSID filterClass{};
    if (!TrustedDocumentFilter(lowerName.substr(dot), &filterClass)) {
        return false;
    }
    IFilter* filter = nullptr;
    IPersistFile* file = nullptr;
    if (FAILED(CoCreateInstance(filterClass, nullptr, CLSCTX_INPROC_SERVER, IID_IFilter, reinterpret_cast<void**>(&filter))) ||
        !filter) {
        return false;
    }
    bool loaded = SUCCEEDED(filter->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&file))) && file &&
                  SUCCEEDED(file->Load(path.c_str(), STGM_READ | STGM_SHARE_DENY_NONE));
    if (file) {
        file->Release();
    }
    ULONG flags = 0;
    std::wstring out;
    if (loaded &&
        SUCCEEDED(filter->Init(IFILTER_INIT_CANON_PARAGRAPHS | IFILTER_INIT_HARD_LINE_BREAKS, 0, nullptr, &flags))) {
        STAT_CHUNK chunk{};
        for (int chunks = 0; out.size() < 1500 && chunks < 2000 && filter->GetChunk(&chunk) == S_OK; ++chunks) {
            if (!(chunk.flags & CHUNK_TEXT)) {
                continue;
            }
            if (!out.empty()) {
                out += chunk.breakType >= CHUNK_EOP ? L'\n' : L' ';
            }
            wchar_t buffer[512];
            ULONG size = ARRAYSIZE(buffer);
            HRESULT got;
            while (out.size() < 1500 && ((got = filter->GetText(&size, buffer)) == S_OK || got == FILTER_S_LAST_TEXT)) {
                for (ULONG i = 0; i < size; ++i) {
                    const wchar_t ch = buffer[i] == L'\r' || buffer[i] == 0x0B ? L'\n' : buffer[i];
                    // No empty lines or runs of spaces: the text, close.
                    if ((ch == L'\n' && (out.empty() || out.back() == L'\n')) ||
                        (ch == L' ' && (out.empty() || out.back() == L' ' || out.back() == L'\n')) ||
                        (ch < 0x20 && ch != L'\n')) {
                        continue;
                    }
                    out += ch;
                }
                if (got == FILTER_S_LAST_TEXT) {
                    break;
                }
                size = ARRAYSIZE(buffer);
            }
        }
    }
    filter->Release();
    while (!out.empty() && iswspace(out.back())) {
        out.pop_back();
    }
    if (out.empty()) {
        return false;
    }
    *text = std::move(out);
    return true;
}

// The icon of the file's type, from its name alone -- for a file the shell
// could not open (no access, or a drive not read from here). SHIL_JUMBO's,
// unless the type has no 256px image -- then that list has its 48 in a corner
// of the 256 -- and then the 48 itself, at its own size.
inline void TypeIcon(const std::wstring& path, int side, Result* r) {
    SHFILEINFOW info{};
    const DWORD attributes = GetFileAttributesW(path.c_str());
    const bool folder = attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    if (!SHGetFileInfoW(path.c_str(), folder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL, &info, sizeof(info),
                        SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX)) {
        return;
    }
    for (int which : {SHIL_JUMBO, SHIL_EXTRALARGE}) {
        IImageList* list = nullptr;
        if (FAILED(SHGetImageList(which, IID_PPV_ARGS(&list))) || !list) {
            continue;
        }
        int cx = 0, cy = 0;
        list->GetIconSize(&cx, &cy);
        const int size = which == SHIL_JUMBO ? side : std::min(side, cx);
        HICON icon = nullptr;
        std::vector<BYTE> pixels;
        bool ok = SUCCEEDED(list->GetIcon(info.iIcon, ILD_TRANSPARENT, &icon)) && icon &&
                  icons::IconToBgra(icon, size, &pixels);
        if (icon) {
            DestroyIcon(icon);
        }
        list->Release();
        if (ok && which == SHIL_JUMBO) {
            // Nothing drawn past the corner a 48 takes in a 256: that is all there is.
            const int corner = size * 48 / 256 + 1;
            bool beyond = false;
            for (int y = 0; y < size && !beyond; ++y) {
                for (int x = y < corner ? corner : 0; x < size; ++x) {
                    if (pixels[(static_cast<size_t>(y) * size + x) * 4 + 3]) {
                        beyond = true;
                        break;
                    }
                }
            }
            ok = beyond;
        }
        if (ok) {
            r->pixels = std::move(pixels);
            r->width = r->height = size;
            return;
        }
    }
}

// On the preview thread.
inline Result Fetch(const std::wstring& path, double scale, bool animate, everything::Client* everythingClient) {
    Result r;
    r.path = path;
    r.scale = scale;
    const size_t slash = path.find_last_of(L"\\/");
    r.name = slash == std::wstring::npos ? path : path.substr(slash + 1);

    // The details: what File Explorer's details pane shows of it, and what
    // its kind has of its own.
    std::wstring type, size, items, dimensions, length, frameRate, bitrate, artist, album, title, author, description,
        version, company, camera, taken, modified, created;
    if (OnFixedDrive(path)) {
        winrt::com_ptr<IShellItemImageFactory> factory;
        IShellItem2* item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) && item) {
            type = PropertyText(item, kItemTypeText);
            ULONG w = 0, h = 0;
            if ((SUCCEEDED(item->GetUInt32(kImageWidth, &w)) && SUCCEEDED(item->GetUInt32(kImageHeight, &h)) && w && h) ||
                (SUCCEEDED(item->GetUInt32(kVideoWidth, &w)) && SUCCEEDED(item->GetUInt32(kVideoHeight, &h)) && w && h)) {
                dimensions = std::to_wstring(w) + L" \u00D7 " + std::to_wstring(h);
            }
            ULONGLONG duration = 0;
            if (SUCCEEDED(item->GetUInt64(kDuration, &duration)) && duration) {
                length = FormatDuration(duration);
            }
            ULONG number = 0;
            if (SUCCEEDED(item->GetUInt32(kFrameRate, &number)) && number) {  // frames per 1000 seconds
                wchar_t text[32];
                swprintf_s(text, number % 1000 ? L"%.2f fps" : L"%.0f fps", number / 1000.0);
                frameRate = text;
            }
            if (SUCCEEDED(item->GetUInt32(kBitrate, &number)) && number) {  // bits per second
                bitrate = std::to_wstring((number + 500) / 1000) + L" kbps";
            }
            artist = PropertyText(item, kArtist);
            album = PropertyText(item, kAlbum);
            title = PropertyText(item, kTitle);
            author = PropertyText(item, kAuthor);
            description = PropertyText(item, kFileDescription);
            version = PropertyText(item, kFileVersion);
            company = PropertyText(item, kCompany);
            camera = PropertyText(item, kCameraModel);
            FILETIME shot{};
            if (SUCCEEDED(item->GetFileTime(kDateTaken, &shot)) && (shot.dwLowDateTime || shot.dwHighDateTime)) {
                taken = FormatDate(shot);
            }

            // Try the shell thumbnail first, including custom format handlers.
            if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(factory.put()))) && factory) {
                SIZE want{static_cast<LONG>(kThumbWidth * scale), static_cast<LONG>(kThumbHeight * scale)};
                HBITMAP bitmap = nullptr;
                if (SUCCEEDED(factory->GetImage(want, SIIGBF_THUMBNAILONLY, &bitmap)) && bitmap) {
                    if (!icons::BitmapToPixels(bitmap, &r.pixels, &r.width, &r.height)) {
                        r.pixels.clear();
                    } else if (r.width > want.cx || r.height > want.cy) {
                        const double fit = std::min(static_cast<double>(want.cx) / r.width,
                                                    static_cast<double>(want.cy) / r.height);
                        const int w = std::max(1, static_cast<int>(r.width * fit));
                        const int h = std::max(1, static_cast<int>(r.height * fit));
                        r.pixels = icons::Resample(r.pixels, r.width, r.height, w, h);
                        r.width = w;
                        r.height = h;
                    }
                }
                if (bitmap) DeleteObject(bitmap);
            }
            item->Release();
        }
        // Content fallbacks also work when shell item/factory creation fails,
        // or a provider returns an unreadable bitmap instead of a thumbnail.
        if (r.pixels.empty()) {
            const int width=static_cast<int>(kThumbWidth*scale),height=static_cast<int>(kThumbHeight*scale);
            const auto lower=tools::ToLower(r.name);
            if (DecodeNativeImage(path,width,height,&r)) {
                // decoded by Windows, independent of thumbnail providers
            } else if (lower.ends_with(L".pdf") && RenderPdfPage(path,width,height,&r)) {
                r.morePages=animate && r.pages>1;
            } else if (lower.ends_with(L".svg") && OpenSvg(path,&r)) {
                // drawn on the card
            } else if (ReadDocumentText(path,lower,&r.text)) {
                // extracted document text
            } else if (ReadTextStart(path,&r.text)) {
                r.monospace=true;
            }
        }
        // Keep custom folder/file icons when no content preview is available.
        if (r.pixels.empty() && r.text.empty() && !r.svg && factory) {
            const int side=TypeIconPixels(scale);
            HBITMAP bitmap=nullptr;
            if (SUCCEEDED(factory->GetImage(SIZE{side,side},SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,&bitmap)) && bitmap) {
                if (!icons::BitmapToPixels(bitmap,&r.pixels,&r.width,&r.height)) r.pixels.clear();
                else if (r.width==r.height && r.width!=side) {
                    r.pixels=icons::Resample(r.pixels,r.width,side);
                    r.width=r.height=side;
                }
            }
            if (bitmap) DeleteObject(bitmap);
        }
        if (animate) {
            FetchMotion(path, static_cast<int>(kThumbWidth * scale), static_cast<int>(kThumbHeight * scale), &r);
        }

        WIN32_FILE_ATTRIBUTE_DATA attributes{};
        if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attributes)) {
            if (attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ULONGLONG bytes = 0;
                bool sized = false, more = false;
                DWORD count = 0;
                if (!FolderFacts(everythingClient, path, &bytes, &sized, &count)) {
                    count = CountItems(path, &more);
                }
                if (sized) {
                    size = ByteSize(bytes);
                }
                items = ItemCount(count, more);
            } else {
                size = ByteSize((static_cast<ULONGLONG>(attributes.nFileSizeHigh) << 32) | attributes.nFileSizeLow);
            }
            modified = FormatDate(attributes.ftLastWriteTime);
            created = FormatDate(attributes.ftCreationTime);
        }
    }
    if (r.pixels.empty() && r.text.empty() && !r.svg) {
        TypeIcon(path, TypeIconPixels(scale), &r);
    }
    auto add = [&r](const wchar_t* label, const std::wstring& value) {
        if (!value.empty()) {
            r.facts.emplace_back(label, value);
        }
    };
    add(L"Type", type);
    add(L"Size", size);
    add(L"Contains", items);
    add(L"Dimensions", dimensions);
    add(L"Length", length);
    add(L"Pages", r.pages ? std::to_wstring(r.pages) : std::wstring());
    add(L"Frame rate", frameRate);
    add(L"Bitrate", bitrate);
    add(L"Artist", artist);
    add(L"Album", album);
    add(L"Title", title);
    add(L"Author", author);
    add(L"Description", description);
    add(L"Version", version);
    add(L"Company", company);
    add(L"Camera", camera);
    add(L"Taken", taken);
    add(L"Modified", modified);
    add(L"Created", created);
    return r;
}

}  // namespace preview

// The card, built on first use. XAML thread only.
[[clang::no_destroy]] wuxcp::Popup g_previewPopup{nullptr};
[[clang::no_destroy]] wuxc::Border g_previewCard{nullptr};
[[clang::no_destroy]] wuxc::Border g_previewThumbBox{nullptr};
[[clang::no_destroy]] wuxc::Border g_previewDetails{nullptr};
[[clang::no_destroy]] wuxc::Image g_previewImage{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_previewName{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_previewText{nullptr};
[[clang::no_destroy]] wuxc::Grid g_previewFacts{nullptr};
[[clang::no_destroy]] wuxc::TextBlock g_previewPath{nullptr};
std::wstring g_previewShownPath;
[[clang::no_destroy]] wux::DispatcherTimer g_previewTimer{nullptr};
[[clang::no_destroy]] wuxma::Storyboard g_previewResize{nullptr};  // PreviewResize
[[clang::no_destroy]] wuxma::DoubleAnimation g_previewResizeAnimation{nullptr};
std::wstring g_previewWanted;  // the selected file's path, or empty

// What plays over the still. XAML thread only.
[[clang::no_destroy]] wuxc::MediaPlayerElement g_previewVideo{nullptr};
[[clang::no_destroy]] winrt::Windows::Media::Playback::MediaPlayer g_previewPlayer{nullptr};
[[clang::no_destroy]] wuxmi::BitmapImage g_previewGif{nullptr};
[[clang::no_destroy]] std::optional<wuxmi::BitmapImage::ImageOpened_revoker> g_previewGifOpened;
[[clang::no_destroy]] wux::DispatcherTimer g_previewFrameTimer{nullptr};
[[clang::no_destroy]] wuxmi::WriteableBitmap g_previewFrameBitmap{nullptr};
std::vector<std::vector<BYTE>> g_previewFrames;
std::vector<int> g_previewDelays;
size_t g_previewFrame = 0;
std::atomic<unsigned> g_previewPlayGeneration{0};

void ApplyPreview();
void ApplyPreviewFrames();

void PreviewThreadMain() {
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    std::optional<everything::Client> everythingClient;
    everythingClient.emplace(L"WindhawkEverythingPreviewReply");
    if (!everythingClient->Init()) {
        everythingClient.reset();
    }
    for (;;) {
        std::wstring path;
        {
            std::unique_lock<std::mutex> lock(preview::g_mutex);
            preview::g_wake.wait(lock, [] { return preview::g_quit || !preview::g_request.empty(); });
            if (preview::g_quit) {
                break;
            }
            path = std::exchange(preview::g_request, std::wstring());
        }
        HWND start = GetSearchWindow();
        UINT dpi = start ? GetDpiForWindow(start) : 0;
        bool animate = true;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            animate = g_settings.animatePreview;
        }
        preview::Result result =
            preview::Fetch(path, (dpi ? dpi : 96) / 96.0, animate, everythingClient ? &*everythingClient : nullptr);
        // The card shows the first page; a PDF's next pages follow. What they
        // need is taken here, before the result is the XAML thread's.
        std::optional<preview::Result> still;
        {
            std::lock_guard<std::mutex> lock(preview::g_mutex);
            if (preview::g_quit || !preview::g_request.empty()) {
                continue;  // stopping, or already outdated
            }
            if (result.morePages) {
                still.emplace();
                still->path = result.path;
                still->pixels = result.pixels;
                still->width = result.width;
                still->height = result.height;
            }
            preview::g_result = std::move(result);
        }
        try {
            if (g_ourBox) {
                g_ourBox.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal,
                                               wuc::DispatchedHandler{[] { ApplyPreview(); }});
            }
        } catch (...) {
        }
        if (still) {
            auto moved = [] {
                std::lock_guard<std::mutex> lock(preview::g_mutex);
                return preview::g_quit || !preview::g_request.empty();
            };
            preview::RenderPdfFrames(still->path, *still, moved, &still->frames, &still->delays);
            if (!still->frames.empty()) {
                {
                    std::lock_guard<std::mutex> lock(preview::g_mutex);
                    if (preview::g_quit || !preview::g_request.empty()) {
                        continue;
                    }
                    preview::g_frames = std::move(still);
                }
                try {
                    if (g_ourBox) {
                        g_ourBox.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal,
                                                       wuc::DispatchedHandler{[] { ApplyPreviewFrames(); }});
                    }
                } catch (...) {
                }
            }
        }
    }
    everythingClient.reset();  // its window, on this thread
    if (SUCCEEDED(comHr)) {
        CoUninitialize();
    }
}

void StopPreviewThread() {
    std::optional<std::thread> thread;
    {
        std::lock_guard<std::mutex> lock(preview::g_mutex);
        preview::g_quit = true;  // for good: nothing starts the thread again
        thread.swap(preview::g_thread);
    }
    preview::g_wake.notify_all();
    if (thread && thread->joinable()) {
        thread->join();
    }
    std::lock_guard<std::mutex> lock(preview::g_mutex);
    preview::g_result.reset();
    preview::g_frames.reset();
}

bool PreviewEnabled() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings.filePreview;
}

// Stops whatever plays over the still and lets it go.
void StopPreviewMotion() {
    g_previewPlayGeneration.fetch_add(1);
    try {
        g_previewGifOpened.reset();
        g_previewGif = nullptr;
        if (g_previewFrameTimer) {
            g_previewFrameTimer.Stop();
        }
        g_previewFrames.clear();
        g_previewDelays.clear();
        g_previewFrameBitmap = nullptr;
        if (g_previewVideo) {
            g_previewVideo.Visibility(wux::Visibility::Collapsed);
            g_previewVideo.SetMediaPlayer(nullptr);
        }
        if (g_previewPlayer) {
            g_previewPlayer.Pause();
            g_previewPlayer.Source(nullptr);
            g_previewPlayer.Close();
            g_previewPlayer = nullptr;
        }
    } catch (...) {
    }
}

void HidePreview() {
    StopPreviewMotion();
    g_previewWanted.clear();
    try {
        if (g_previewTimer) {
            g_previewTimer.Stop();
        }
        if (g_previewPopup && g_previewPopup.IsOpen()) {
            g_previewPopup.IsOpen(false);
        }
    } catch (...) {
    }
}

void CopyPreviewText(std::wstring const& text) {
    if(!text.empty()) tools::CopyTextToClipboard(text);
}

// Keep the search box focused while copying a preview value, including its
// full text when the displayed value is trimmed.
wuxc::Button PreviewCopyButton(wuxc::TextBlock const& text,std::wstring const& action) {
    wuxc::Button button;
    button.Margin(text.Margin());
    text.Margin(wux::ThicknessHelper::FromUniformLength(0));
    button.Padding(wux::ThicknessHelper::FromLengths(2,1,2,1));
    button.MinWidth(0); button.MinHeight(0);
    button.HorizontalAlignment(wux::HorizontalAlignment::Left);
    button.HorizontalContentAlignment(wux::HorizontalAlignment::Left);
    button.Background(MakeBrush(0,0,0,0));
    button.BorderThickness(wux::ThicknessHelper::FromUniformLength(0));
    button.IsTabStop(false); button.AllowFocusOnInteraction(false);
    button.Content(text);
    wuxc::ToolTipService::SetToolTip(button,winrt::box_value(winrt::hstring{action}));
    wux::Automation::AutomationProperties::SetName(button,action);
    KeepHandler(button,button.Click(winrt::auto_revoke,[](wf::IInspectable const& sender,wux::RoutedEventArgs const&) {
        auto value=sender.as<wuxc::Button>().Content().as<wuxc::TextBlock>();
        CopyPreviewText(std::wstring(value.Text()));
    }));
    return button;
}

void PositionPreview(double height = 0);

void BuildPreviewCard() {
    if (g_previewPopup) {
        return;
    }
    wuxc::Border card;
    card.Width(preview::kWidth);
    card.Padding(wux::ThicknessHelper::FromLengths(12,12,12,6));
    card.CornerRadius(wux::CornerRadius{8, 8, 8, 8});
    card.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
    KeepHandler(card, card.SizeChanged(winrt::auto_revoke, [](wf::IInspectable const&, wux::SizeChangedEventArgs const& args) {
        if (g_previewPopup && g_previewPopup.IsOpen()) PositionPreview(args.NewSize().Height);
    }));

    wuxc::StackPanel stack;
    wuxc::Border thumbBox;
    thumbBox.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
    wuxc::Grid thumbGrid;
    wuxc::Image image;
    image.Stretch(wuxm::Stretch::Uniform);
    image.HorizontalAlignment(wux::HorizontalAlignment::Center);
    image.VerticalAlignment(wux::VerticalAlignment::Center);
    thumbGrid.Children().Append(image);
    wuxc::MediaPlayerElement video;
    video.AreTransportControlsEnabled(false);
    video.Stretch(wuxm::Stretch::Uniform);
    video.HorizontalAlignment(wux::HorizontalAlignment::Center);
    video.VerticalAlignment(wux::VerticalAlignment::Center);
    video.Visibility(wux::Visibility::Collapsed);
    thumbGrid.Children().Append(video);
    auto thumbText = SearchTextBlock();  // a text file's or a document's start (Result::text)
    thumbText.Margin(wux::ThicknessHelper::FromLengths(10, 8, 10, 8));
    ScaleFont(thumbText, 11);
    thumbText.Opacity(0.85);
    thumbText.MaxLines(24);
    thumbText.IsTextSelectionEnabled(true);
    thumbText.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    thumbText.Visibility(wux::Visibility::Collapsed);
    thumbGrid.Children().Append(thumbText);
    thumbBox.Child(thumbGrid);
    stack.Children().Append(thumbBox);

    // Sample details below the thumbnail, with empty padding at both ends.
    // A bright image must not choose dark text for a darker details backdrop.
    wuxc::Border details;
    details.Padding(wux::ThicknessHelper::FromLengths(0,0,0,6));
    wuxc::StackPanel detailsStack;
    details.Child(detailsStack);
    stack.Children().Append(details);
    auto name = SearchTextBlock();
    name.Margin(wux::ThicknessHelper::FromLengths(2, 10, 2, 0));
    ScaleFont(name, 14);
    name.FontWeight(wut::FontWeights::SemiBold());
    name.TextWrapping(wux::TextWrapping::Wrap);
    name.MaxLines(2);
    name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    detailsStack.Children().Append(PreviewCopyButton(name,L"Copy name"));

    // The details, label and value (ApplyPreview fills them in).
    wuxc::Grid facts;
    facts.Margin(wux::ThicknessHelper::FromLengths(2, 8, 2, 0));
    facts.ColumnSpacing(12);
    facts.RowSpacing(3);
    wuxc::ColumnDefinition labelColumn;
    labelColumn.Width(wux::GridLengthHelper::Auto());
    facts.ColumnDefinitions().Append(labelColumn);
    wuxc::ColumnDefinition valueColumn;
    valueColumn.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    facts.ColumnDefinitions().Append(valueColumn);
    detailsStack.Children().Append(facts);

    // The full path uses the same text-sized copy target as the other values.
    auto pathText = SearchTextBlock();
    ScaleFont(pathText,12);
    pathText.Opacity(0.7);
    pathText.Margin(wux::ThicknessHelper::FromLengths(2,6,2,0));
    pathText.TextWrapping(wux::TextWrapping::Wrap);
    pathText.MaxLines(3);
    pathText.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    detailsStack.Children().Append(PreviewCopyButton(pathText,L"Copy path"));
    card.Child(stack);

    wuxcp::Popup popup;
    popup.Child(card);
    popup.IsLightDismissEnabled(false);
    popup.ShouldConstrainToRootBounds(false);
    try {
        if (g_resultsHost && g_resultsHost.XamlRoot()) {
            popup.XamlRoot(g_resultsHost.XamlRoot());
        }
    } catch (...) {
    }

    g_previewPopup = popup;
    g_previewCard = card;
    g_previewThumbBox = thumbBox;
    g_previewDetails = details;
    g_previewImage = image;
    g_previewVideo = video;
    g_previewText = thumbText;
    g_previewName = name;
    g_previewFacts = facts;
    g_previewPath = pathText;
}

// Bottom taskbars share the bottom edge for tall previews. Other taskbar
// edges keep the preview aligned to the search top and always grow downward.
// During resizing, SizeChanged supplies its actual laid-out height, so the
// anchor changes continuously at equal heights without restarting the animation.
void PositionPreview(double height) {
    if (!g_previewPopup || !g_resultsHost) {
        return;
    }
    const auto origin = g_resultsHost.TransformToVisual(nullptr).TransformPoint(wf::Point{0, 0});
    const double panelWidth = g_resultsHost.ActualWidth();
    double windowWidth = 0, windowHeight = 0;
    try {
        if (auto root = g_resultsHost.XamlRoot()) {
            windowWidth = root.Size().Width;
            windowHeight = root.Size().Height;
        }
    } catch (...) {
    }
    if (windowWidth <= 0) {
        try {
            windowWidth = wux::Window::Current().Bounds().Width;
            windowHeight = wux::Window::Current().Bounds().Height;
        } catch (...) {
        }
    }
    constexpr double kGap = 12;
    double x = g_searchTaskbarEdge==SearchTaskbarEdge::Right
        ? origin.X-kGap-preview::kWidth : origin.X+panelWidth+kGap;
    if (g_searchTaskbarEdge!=SearchTaskbarEdge::Right && windowWidth > 0 && x + preview::kWidth > windowWidth && origin.X - kGap - preview::kWidth >= 0) {
        x = origin.X - kGap - preview::kWidth;
    }
    HWND start = GetSearchWindow();
    const double scale = (start ? GetDpiForWindow(start) : 96) / 96.0;
    double top = 0, bottom = windowHeight;
    POINT client{};
    MONITORINFO monitor{sizeof(monitor)};
    if (start && ClientToScreen(start, &client) &&
        GetMonitorInfoW(MonitorFromWindow(start, MONITOR_DEFAULTTONEAREST), &monitor)) {
        // Popup offsets are relative to the search window, but its available
        // height is the monitor's work area. Negative offsets let a tall
        // preview extend above a short search window without covering the taskbar.
        top = (monitor.rcWork.top - client.y) / scale;
        bottom = (monitor.rcWork.bottom - client.y) / scale;
    }
    if (height <= 0) {
        g_previewCard.Measure(wf::Size{static_cast<float>(preview::kWidth), std::numeric_limits<float>::infinity()});
        height = g_previewCard.DesiredSize().Height;
    }
    double y = origin.Y;
    if (g_searchTaskbarEdge==SearchTaskbarEdge::Bottom) {
        y += std::min(0.0, g_resultsHost.ActualHeight() - height);
        if (bottom > top) y = std::max(top, std::min(y, bottom - height));
    }
    g_previewPopup.HorizontalOffset(x);
    g_previewPopup.VerticalOffset(y);

}

// Plays a result's motion over the still, already shown at its size.
void StartPreviewMotion(preview::Result& result) {
    const unsigned generation = g_previewPlayGeneration.load();
    if (result.gif) {
        // Swapped in once loaded, so the still shows until then.
        wuxmi::BitmapImage gif;
        gif.DecodePixelWidth(result.width);
        g_previewGifOpened = gif.ImageOpened(winrt::auto_revoke, [generation](auto&&, auto&&) {
            if (generation == g_previewPlayGeneration.load() && g_previewGif && g_previewImage) {
                g_previewImage.Source(g_previewGif);
            }
        });
        g_previewGif = gif;
        gif.SetSourceAsync(result.gif);
    } else if (!result.frames.empty()) {
        g_previewFrames = std::move(result.frames);
        g_previewDelays = std::move(result.delays);
        g_previewFrame = 0;
        g_previewFrameBitmap = wuxmi::WriteableBitmap{result.width, result.height};
        g_previewImage.Source(g_previewFrameBitmap);
        if (!g_previewFrameTimer) {
            g_previewFrameTimer = wux::DispatcherTimer();
            KeepHandler(g_previewFrameTimer,
                        g_previewFrameTimer.Tick(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
                            if (g_previewFrames.empty() || !g_previewFrameBitmap) {
                                g_previewFrameTimer.Stop();
                                return;
                            }
                            g_previewFrame = (g_previewFrame + 1) % g_previewFrames.size();
                            const auto& frame = g_previewFrames[g_previewFrame];
                            auto access = g_previewFrameBitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
                            BYTE* dest = nullptr;
                            if (SUCCEEDED(access->Buffer(&dest)) && dest &&
                                g_previewFrameBitmap.PixelBuffer().Capacity() >= frame.size()) {
                                memcpy(dest, frame.data(), frame.size());
                                g_previewFrameBitmap.Invalidate();
                            }
                            g_previewFrameTimer.Interval(std::chrono::milliseconds(g_previewDelays[g_previewFrame]));
                        }));
        }
        auto access = g_previewFrameBitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
        BYTE* dest = nullptr;
        if (SUCCEEDED(access->Buffer(&dest)) && dest) {
            memcpy(dest, g_previewFrames[0].data(), g_previewFrames[0].size());
            g_previewFrameBitmap.Invalidate();
        }
        g_previewFrameTimer.Interval(std::chrono::milliseconds(g_previewDelays[0]));
        g_previewFrameTimer.Start();
    } else if (result.video) {
        // Muted and looping, over the still, which it shows as its poster
        // until the first frame is ready.
        namespace playback = winrt::Windows::Media::Playback;
        playback::MediaPlayer player;
        player.IsMuted(true);
        player.IsLoopingEnabled(true);
        player.AutoPlay(true);
        g_previewVideo.PosterSource(g_previewImage.Source());
        g_previewVideo.Visibility(wux::Visibility::Visible);
        g_previewVideo.Width(g_previewImage.Width());
        g_previewVideo.Height(g_previewImage.Height());
        g_previewVideo.SetMediaPlayer(player);
        g_previewPlayer = player;
        player.Source(winrt::Windows::Media::Core::MediaSource::CreateFromStorageFile(result.video));
    }
}

// Drags a result out of Start, as File Explorer drags a file, by handing the
// drag to this mod's helper window in Explorer (RunExplorerDrag).
//
// Nowhere in this process works. XAML's own drag (CanDrag, StartDragAsync)
// fails inside XAML in Start's window, which then ends the process. A shell
// drag loop on Start's thread gets only the first move of the mouse -- Start's
// input does not come through ordinary window messages -- and then waits for
// good, the whole menu with it. One on another thread gets no mouse at all,
// and Windows will not hand the foreground to it while the button is down.
// It does hand it to Explorer's helper, a plain window in a plain process,
// where the drag works as anywhere else. Start's results stay up meanwhile
// (g_dragging); Explorer tells Start how it ended (DragDoneMessage).
void StartFileDrag(std::wstring const& path) {
    // Only what Explorer's helper takes (a local file or folder that exists),
    // or Start would give it the foreground for nothing.
    bool local = path.size() >= 3 && path[1] == L':' && !PathIsUNCW(path.c_str());
    if (local) {
        const wchar_t root[] = {path[0], L':', L'\\', 0};
        local = GetDriveTypeW(root) != DRIVE_REMOTE && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
    }
    HWND helper = local ? FindExplorerLaunchHolder() : nullptr;
    if (!helper) {
        Wh_Log(L"drag: not for %ls", path.c_str());
        return;
    }
    ReleaseCapture();  // the row's button holds the mouse
    g_dragging = true;
    if (!SetForegroundWindow(helper)) {
        g_dragging = false;
        Wh_Log(L"drag: Explorer's helper did not get the foreground");
        return;
    }
    COPYDATASTRUCT cds{};
    cds.dwData = kExplorerDragMagic;
    cds.cbData = static_cast<DWORD>((path.size() + 1) * sizeof(wchar_t));
    cds.lpData = const_cast<wchar_t*>(path.c_str());
    DWORD_PTR accepted = 0;
    if (!SendMessageTimeoutW(helper, WM_COPYDATA, reinterpret_cast<WPARAM>(GetSearchWindow()),
                             reinterpret_cast<LPARAM>(&cds), SMTO_ABORTIFHUNG, 500, &accepted) ||
        !accepted) {
        g_dragging = false;
        Wh_Log(L"drag: Explorer's helper refused %ls", path.c_str());
    }
}

// Removes, when destroyed, a handler added with AddHandler -- which C++/WinRT
// gives no revoker for -- so KeepHandler can hold it like the others.
struct RoutedHandlerRevoker {
    winrt::weak_ref<wux::UIElement> element;
    wux::RoutedEvent event{nullptr};
    wf::IInspectable handler{nullptr};

    RoutedHandlerRevoker(wux::UIElement const& e, wux::RoutedEvent const& ev, wf::IInspectable const& h)
        : element(winrt::make_weak(e)), event(ev), handler(h) {}
    RoutedHandlerRevoker(RoutedHandlerRevoker&&) = default;
    RoutedHandlerRevoker& operator=(RoutedHandlerRevoker&&) = default;
    ~RoutedHandlerRevoker() {
        if (!handler) {
            return;  // moved from
        }
        try {
            if (auto e = element.get()) {
                e.RemoveHandler(event, handler);
            }
        } catch (...) {
        }
    }
};

// A pointer handler that also hears events a control has already handled --
// a Button handles its own presses.
void AddPointerHandler(wux::UIElement const& element, wux::RoutedEvent const& event,
                       wux::Input::PointerEventHandler const& handler) {
    auto boxed = winrt::box_value(handler);
    element.AddHandler(event, boxed, true);
    KeepHandler(element, RoutedHandlerRevoker{element, event, boxed});
}

// Shows what the preview thread found, if it is still what is selected.
// Eases the card's height from what it was to what its new content needs,
// over a sixth of a second. The content is laid out at once at its own size:
// while the card is shorter, it is cut off at the bottom; while taller, the
// card shows its background below it. SizeChanged keeps the current edge
// aligned during every frame, including when a new selection interrupts it.
void PreviewResize(double fromHeight, double toHeight) {
    if (!g_previewResize) {
        g_previewResizeAnimation = wuxma::DoubleAnimation();
        g_previewResizeAnimation.EnableDependentAnimation(true);  // Height is laid out, so on this thread
        g_previewResizeAnimation.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(160)));
        wuxma::CubicEase ease;
        ease.EasingMode(wuxma::EasingMode::EaseOut);
        g_previewResizeAnimation.EasingFunction(ease);
        wuxma::Storyboard::SetTargetProperty(g_previewResizeAnimation, L"Height");
        g_previewResize = wuxma::Storyboard();
        g_previewResize.Children().Append(g_previewResizeAnimation);
        KeepHandler(g_previewResize, g_previewResize.Completed(winrt::auto_revoke, [](wf::IInspectable const&,
                                                                                      wf::IInspectable const&) {
            if (g_previewCard) {
                g_previewCard.ClearValue(wux::FrameworkElement::HeightProperty());
            }
            if (g_previewResize) {
                g_previewResize.Stop();  // let go of the animated value: back to its own height
            }
            PositionPreview();
        }));
    }
    wuxma::Storyboard::SetTarget(g_previewResizeAnimation, g_previewCard);
    g_previewResizeAnimation.From(fromHeight);
    g_previewResizeAnimation.To(toHeight);
    g_previewCard.Height(fromHeight);
    PositionPreview(fromHeight);
    g_previewResize.Begin();
}

void ApplyPreview() try {
    std::optional<preview::Result> result;
    {
        std::lock_guard<std::mutex> lock(preview::g_mutex);
        result = std::move(preview::g_result);
        preview::g_result.reset();
    }
    if (!result || result->path != g_previewWanted || !g_isOverlayVisible.load() ||
        g_searchWindowWaitingForStart || g_searchWindowSliding || !g_resultsHost) {
        Wh_Log(L"preview: dropped (%ls)", result ? L"no longer selected" : L"nothing fetched");
        return;
    }
    BuildPreviewCard();

    const bool isLight = IsLightTheme();
    g_previewCard.RequestedTheme(isLight ? wux::ElementTheme::Light : wux::ElementTheme::Dark);
    auto background = g_resultsHost.Background();
    g_previewCard.Background(background ? background
                                        : (isLight ? MakeBrush(0xF2, 0xF3, 0xF3, 0xF3) : MakeBrush(0xF2, 0x20, 0x20, 0x20)));
    g_previewCard.BorderBrush(isLight ? MakeBrush(0x24, 0x00, 0x00, 0x00) : MakeBrush(0x30, 0xFF, 0xFF, 0xFF));
    g_previewThumbBox.Background(isLight ? MakeBrush(0x0C, 0x00, 0x00, 0x00) : MakeBrush(0x10, 0xFF, 0xFF, 0xFF));

    // Already up: it eases from the old content's height to the new one's
    // (PreviewResize) rather than jump.
    const double fromHeight = g_previewPopup.IsOpen() ? g_previewCard.ActualHeight() : 0;
    if (g_previewResize) {
        g_previewResize.Stop();
    }
    g_previewCard.ClearValue(wux::FrameworkElement::HeightProperty());

    StopPreviewMotion();
    wuxmi::WriteableBitmap bitmap{nullptr};
    if (result->width > 0 && result->height > 0 &&
        result->pixels.size() == static_cast<size_t>(result->width) * result->height * 4) {
        bitmap = wuxmi::WriteableBitmap{result->width, result->height};
        auto access = bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
        BYTE* dest = nullptr;
        if (SUCCEEDED(access->Buffer(&dest)) && dest) {
            memcpy(dest, result->pixels.data(), result->pixels.size());
            bitmap.Invalidate();
        } else {
            bitmap = nullptr;
        }
    }
    g_previewImage.Source(bitmap);
    g_previewImage.Margin(wux::ThicknessHelper::FromUniformLength(0));
    if (bitmap) {
        // Pixel for pixel: made at the display's scale. The box is as high as
        // the picture, so a wide one leaves no bands above and below.
        g_previewImage.Width(result->width / result->scale);
        g_previewImage.Height(result->height / result->scale);
        g_previewThumbBox.Height(std::max(result->height / result->scale, 48.0));
    }
    // An SVG, drawn by XAML at the display's scale, in a box of its shape, on
    // a light background: most are dark shapes on nothing.
    const bool showSvg = !bitmap && result->svg;
    if (showSvg) {
        double width = preview::kThumbWidth, height = width * result->svgAspect;
        if (height > preview::kThumbHeight) {
            height = preview::kThumbHeight;
            width = height / result->svgAspect;
        }
        height = std::max(height, 48.0);
        constexpr double kInset = 8;
        wuxmi::SvgImageSource svg;
        svg.RasterizePixelWidth((width - 2 * kInset) * result->scale);
        svg.RasterizePixelHeight((height - 2 * kInset) * result->scale);
        svg.SetSourceAsync(result->svg);
        g_previewImage.Source(svg);
        g_previewImage.Width(width - 2 * kInset);
        g_previewImage.Height(height - 2 * kInset);
        g_previewImage.Margin(wux::ThicknessHelper::FromUniformLength(kInset));
        g_previewThumbBox.Height(height);
        g_previewThumbBox.Background(MakeBrush(0xF2, 0xFF, 0xFF, 0xFF));
    }
    const bool picture = bitmap || showSvg;
    g_previewImage.Visibility(picture ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    // Text in place of a picture: a text file line for line, in a monospaced
    // font; a document's paragraphs, wrapped. The box is as high as the text.
    const bool showText = !picture && !result->text.empty();
    g_previewText.Visibility(showText ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (showText) {
        g_previewText.Text(winrt::hstring{result->text});
        g_previewText.FontFamily(wuxm::FontFamily(result->monospace ? L"Cascadia Mono, Consolas" : L"Segoe UI"));
        g_previewText.TextWrapping(result->monospace ? wux::TextWrapping::NoWrap : wux::TextWrapping::Wrap);
        g_previewThumbBox.ClearValue(wux::FrameworkElement::HeightProperty());
    }
    g_previewThumbBox.Visibility(picture || showText ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if (bitmap) {
        StartPreviewMotion(*result);
    }
    g_previewName.Text(winrt::hstring{result->name});
    g_previewFacts.Children().Clear();
    g_previewFacts.RowDefinitions().Clear();
    for (size_t i = 0; i < result->facts.size(); ++i) {
        wuxc::RowDefinition row;
        row.Height(wux::GridLengthHelper::Auto());
        g_previewFacts.RowDefinitions().Append(row);
        auto label = SearchTextBlock();
        label.Text(winrt::hstring{result->facts[i].first});
        ScaleFont(label, 12);
        label.Opacity(0.55);
        wuxc::Grid::SetRow(label, static_cast<int32_t>(i));
        g_previewFacts.Children().Append(label);
        auto value = SearchTextBlock();
        value.Text(winrt::hstring{result->facts[i].second});
        ScaleFont(value, 12);
        value.TextWrapping(wux::TextWrapping::Wrap);
        value.MaxLines(2);
        value.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        auto copyValue=PreviewCopyButton(value,L"Copy " + result->facts[i].first);
        wuxc::Grid::SetRow(copyValue,static_cast<int32_t>(i));
        wuxc::Grid::SetColumn(copyValue,1);
        g_previewFacts.Children().Append(copyValue);
    }
    InvalidateSearchContrastContainer(g_previewDetails);
    g_previewShownPath = result->path;
    g_previewPath.Text(winrt::hstring{result->path});

    g_previewCard.Measure(wf::Size{static_cast<float>(preview::kWidth), std::numeric_limits<float>::infinity()});
    const double toHeight = g_previewCard.DesiredSize().Height;
    if (fromHeight > 0 && std::abs(toHeight - fromHeight) > 1) {
        PreviewResize(fromHeight, toHeight);
    } else {
        PositionPreview();
    }
    if (!g_previewPopup.IsOpen()) {
        g_previewCard.Opacity(0.0);
        g_previewPopup.IsOpen(true);
        wuxma::Storyboard fade;
        wuxma::DoubleAnimation opacity;
        opacity.From(0.0);
        opacity.To(1.0);
        opacity.Duration(wux::DurationHelper::FromTimeSpan(std::chrono::milliseconds(120)));
        wuxma::Storyboard::SetTarget(opacity, g_previewCard);
        wuxma::Storyboard::SetTargetProperty(opacity, L"Opacity");
        fade.Children().Append(opacity);
        fade.Begin();
    }
    Wh_Log(L"preview: showing %ls (%dx%d) at %.0f,%.0f", result->name.c_str(), result->width, result->height,
           g_previewPopup.HorizontalOffset(), g_previewPopup.VerticalOffset());
} catch (...) {
    Wh_Log(L"preview: failed %08X", static_cast<unsigned>(winrt::to_hresult()));
}

// A PDF's next pages, once ready, for the card still showing its first.
void ApplyPreviewFrames() try {
    std::optional<preview::Result> frames;
    {
        std::lock_guard<std::mutex> lock(preview::g_mutex);
        frames = std::move(preview::g_frames);
        preview::g_frames.reset();
    }
    if (!frames || !g_previewPopup || !g_previewPopup.IsOpen() || frames->path != g_previewShownPath ||
        !g_previewFrames.empty()) {
        return;
    }
    StartPreviewMotion(*frames);
} catch (...) {
}

// Follows the selection: the selected file's preview, after a short pause.
void SchedulePreview() try {
    if (g_dragging || g_searchWindowWaitingForStart || g_searchWindowSliding) {
        return;
    }
    std::wstring path;
    if (g_filesColumnActive && g_selectedFile >= 0 && g_selectedFile < static_cast<int>(g_currentFileRows.size())) {
        path = g_currentFileRows[g_selectedFile].openPath;
    }
    if (path.empty() || !PreviewEnabled()) {
        HidePreview();
        return;
    }
    if (path == g_previewWanted) {
        return;
    }
    g_previewWanted = path;
    if (!g_previewTimer) {
        g_previewTimer = wux::DispatcherTimer();
        KeepHandler(g_previewTimer, g_previewTimer.Tick(winrt::auto_revoke, [](wf::IInspectable const&, wf::IInspectable const&) {
            g_previewTimer.Stop();
            if (g_previewWanted.empty() || !g_isOverlayVisible.load()) {
                return;
            }
            Wh_Log(L"preview: fetching %ls", g_previewWanted.c_str());
            {
                std::lock_guard<std::mutex> lock(preview::g_mutex);
                preview::g_request = g_previewWanted;
                if (!preview::g_thread && !preview::g_quit) {
                    preview::g_thread.emplace(PreviewThreadMain);
                }
            }
            preview::g_wake.notify_all();
        }));
    }
    // Quicker once a card is up: moving from one file to the next.
    const bool open = g_previewPopup && g_previewPopup.IsOpen();
    g_previewTimer.Interval(std::chrono::milliseconds(open ? 80 : 250));
    g_previewTimer.Stop();
    g_previewTimer.Start();
} catch (...) {
}

// File types "Run as administrator" is offered for.
bool CanElevatePath(const std::wstring& path) {
    std::wstring lower = path;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
    return lower.ends_with(L".exe") || lower.ends_with(L".bat") ||
           lower.ends_with(L".cmd") || lower.ends_with(L".ps1") ||
           lower.ends_with(L".msc") || lower.ends_with(L".lnk");
}

void OpenSelectedFile(int index, bool asAdmin) {
    if (index < 0 || index >= static_cast<int>(g_currentFileRows.size())) {
        return;
    }
    const std::wstring& path = g_currentFileRows[index].openPath;
    if (path.empty()) {
        return;
    }
    if (asAdmin && !CanElevatePath(path)) {
        asAdmin = false;
    }
    DismissSearchForLaunch([path = path, asAdmin] { OpenResult(path, asAdmin); });
    Wh_Log(L"open file: index %d ('%ls'), asAdmin=%d", index, path.c_str(), asAdmin ? 1 : 0);
}

// Opens the selected result's context menu, as a right-click on it would.
// Shown in standard mode it takes the keyboard, so the arrows and Enter work
// in it (see ContextMenuHasFocus) and Escape hands focus back to the search
// box. Queued rather than shown here: the key usually arrives inside the
// message hook, which is no place to open a popup.
void OpenSelectedContextMenu() {
    wuxc::Button target{nullptr};
    if (g_filesColumnActive && g_fileButtonsOpt && g_selectedFile >= 0 &&
        g_selectedFile < static_cast<int>(g_fileButtonsOpt->size())) {
        target = (*g_fileButtonsOpt)[g_selectedFile];
    } else if (!g_filesColumnActive && g_appButtonsOpt && g_selectedApp >= 0 &&
               g_selectedApp < static_cast<int>(g_appButtonsOpt->size())) {
        target = (*g_appButtonsOpt)[g_selectedApp];
    }
    if (!target || !target.ContextFlyout()) {
        return;
    }
    try {
        target.Dispatcher().RunAsync(
            wuc::CoreDispatcherPriority::Normal,
            [weak = winrt::make_weak(target)] {
                auto button = weak.get();
                if (!button) return;
                auto flyout = button.ContextFlyout();
                if (!flyout) return;
                try {
                    wuxc::Primitives::FlyoutShowOptions options;
                    options.ShowMode(wuxc::Primitives::FlyoutShowMode::Standard);
                    options.Placement(wuxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
                    flyout.ShowAt(button, options);
                } catch (...) {}
            });
    } catch (...) {}
}

// Whether the caret sits at the end of the query, with nothing selected, so
// Right has no text left to move over.
bool CaretAtEndOfQuery() {
    if (!g_ourBox) {
        return true;
    }
    try {
        return g_ourBox.SelectionLength() == 0 &&
               g_ourBox.SelectionStart() >= static_cast<int32_t>(g_ourBox.Text().size());
    } catch (...) {
        return true;
    }
}

void LaunchSelectedApp(int index, bool asAdmin) {
    if (index < 0 || index >= static_cast<int>(g_currentAppRows.size())) {
        return;
    }
    const auto& row = g_currentAppRows[index];
    if (!row.commandLine.empty()) {
        CustomCommandLaunch launch;
        std::wstring error;
        if(!PrepareCustomCommand(row.commandLine,row.commandShell,row.commandHideTerminal,
                                 row.commandShellExecutable,row.commandShellArguments,&launch,&error)) {
            Wh_Log(L"custom command: %ls",error.c_str());
            if(g_footerStatus) {
                g_footerStatus.Text(winrt::hstring{error});
                g_footerStatus.Opacity(1);
                g_footerStatus.MaxWidth(std::numeric_limits<double>::infinity());
                g_footerStatus.TextWrapping(wux::TextWrapping::Wrap);
                wuxc::ToolTipService::SetToolTip(g_footerStatus,winrt::box_value(error));
            }
            if(g_footerHints) g_footerHints.Visibility(wux::Visibility::Collapsed);
            return;
        }
        {
            std::lock_guard<std::mutex> lock(g_queryMutex);
            g_commandUsageRequests.push_back(apps::CommandUsageKey(row.title));
        }
        g_queryWake.notify_all();
        DismissSearchForLaunch([launch = std::move(launch),
                                   hideTerminal = row.commandHideTerminal, asAdmin]() mutable {
            RunCustomCommand(std::move(launch), hideTerminal, asAdmin);
        });
        return;
    }
    if (!row.copyText.empty()) {
        DismissSearch();
        tools::CopyTextToClipboard(row.copyText);
        Wh_Log(L"copied to clipboard: '%ls'", row.copyText.c_str());
        return;
    }
    if (row.openPath.starts_with(L"http:") || row.openPath.starts_with(L"https:")) {
        DismissSearchForLaunch([url = row.openPath] { OpenResult(url, false); });
        Wh_Log(L"launch web: '%ls'", row.openPath.c_str());
        return;
    }
    int which = row.appIndex;
    if (which >= 0) {
        if (asAdmin && !row.canRunAsAdmin) {
            asAdmin = false;
        }
        DismissSearchForLaunch([which, asAdmin] {
            {
                std::lock_guard<std::mutex> lock(g_queryMutex);
                g_launchAsAdmin.store(asAdmin);
                g_launchRequest.store(which);
            }
            g_queryWake.notify_all();
        });
        Wh_Log(L"launch app: index %d ('%ls'), asAdmin=%d", index, row.title.c_str(), asAdmin ? 1 : 0);
    }
}

// The text the rows on screen are the results for (RenderResults).
std::wstring g_shownQuery;

// Whether the rows on screen are not yet the results for what is typed.
bool ResultsPending() {
    std::lock_guard<std::mutex> lock(g_queryMutex);
    return g_pendingQuery != g_shownQuery;
}

// What Enter does: opens the selected result.
void ActivateSelection(bool asAdmin) {
    const bool haveFiles = FilesPanelVisible() && g_fileButtonsOpt && !g_fileButtonsOpt->empty();
    if (g_filesColumnActive && haveFiles) {
        OpenSelectedFile(g_selectedFile < 0 ? 0 : g_selectedFile, asAdmin);
    } else if (!g_currentAppRows.empty()) {
        LaunchSelectedApp(g_selectedApp < 0 ? 0 : g_selectedApp, asAdmin);
    }
}

bool IsNavigationKey(winrt::Windows::System::VirtualKey key) {
    using VK = winrt::Windows::System::VirtualKey;
    return key == VK::Down || key == VK::Up || key == VK::Enter || key == VK::Tab ||
           key == VK::Left || key == VK::Right;
}

// A result's context menu is open and has the keyboard: the arrows, Tab and
// Enter are its own (Right opens its submenus).
bool ContextMenuHasFocus() {
    try {
        auto focused = g_resultsHost && g_resultsHost.XamlRoot()
                           ? wux::Input::FocusManager::GetFocusedElement(g_resultsHost.XamlRoot())
                           : wux::Input::FocusManager::GetFocusedElement();
        return focused && (focused.try_as<wuxc::MenuFlyoutItemBase>() ||
                           focused.try_as<wuxc::MenuFlyoutPresenter>());
    } catch (...) {
        return false;
    }
}

// Escape reaches the CoreWindow's KeyDown only after XAML has used it to
// close an open menu, by which time the menu no longer has focus. So a
// menu still closing, or closed a moment ago, counts as open, and the
// query is kept.
static std::atomic<bool> g_contextMenuOpen{false};
[[clang::no_destroy]] winrt::weak_ref<wuxcp::FlyoutBase> g_activeContextFlyout;
static uint64_t g_contextMenuClosedTick = 0;

void NoteContextMenuOpening(wf::IInspectable const& sender) {
    if(auto flyout=sender.try_as<wuxcp::FlyoutBase>()) g_activeContextFlyout=winrt::make_weak(flyout);
    g_contextMenuOpen=true;
}

void NoteContextMenuClosed(wf::IInspectable const& sender) {
    auto active=g_activeContextFlyout.get();
    if(active && active!=sender.try_as<wuxcp::FlyoutBase>()) return;
    g_activeContextFlyout={}; g_contextMenuOpen=false;
    g_contextMenuClosedTick=GetTickCount64();
    if(g_searchWindowWindow.load()) QueueSearchBoxRefocus(true);
}

bool EscapeBelongsToContextMenu() {
    return g_contextMenuOpen || ContextMenuHasFocus() ||
           GetTickCount64() - g_contextMenuClosedTick < 250;
}

// Keyboard selection. Up and Down move within the active column and Enter
// opens what is selected there; Shift+Enter opens its context menu. Tab and Shift+Tab step through every result
// as one list, the apps and then the files, so Tab past the last app lands on
// the first file. Left and Right switch between the columns, but still edit
// the query while they have text to move over: Right switches only with the
// caret at the end, and Left only from the Files column. Returns whether the
// key was used; a key that was not goes on to the search box.
bool HandleNavigationKey(winrt::Windows::System::VirtualKey key, bool ctrl) {
    using VK = winrt::Windows::System::VirtualKey;

    if (ContextMenuHasFocus()) {
        return false;
    }

    // One keypress can arrive by several routes (the message hook, the box,
    // the CoreWindow). Act on it once, and give the other routes the same
    // answer, so that none of them lets a used Left through to the caret.
    static VK s_lastKey = VK::None;
    static bool s_lastUsed = false;
    uint64_t now = GetTickCount64();
    if (key == s_lastKey && now - g_lastNavTick < 60) {
        return s_lastUsed;
    }

    const bool haveApps = g_appButtonsOpt && !g_appButtonsOpt->empty();
    const bool haveFiles = FilesPanelVisible() && g_fileButtonsOpt && !g_fileButtonsOpt->empty();
    const bool inFiles = g_filesColumnActive && haveFiles;
    auto toApps = [&] { SetAppSelection(g_selectedApp < 0 ? 0 : g_selectedApp); };
    auto toFiles = [&] { SetFileSelection(g_selectedFile < 0 ? 0 : g_selectedFile); };

    bool used = true;
    switch (key) {
        case VK::Down:
            if (inFiles) {
                SetFileSelection(g_selectedFile < 0 ? 0 : g_selectedFile + 1);
            } else if (haveApps) {
                SetAppSelection(g_selectedApp < 0 ? 0 : g_selectedApp + 1);
            }
            break;
        case VK::Up:
            if (inFiles) {
                SetFileSelection(g_selectedFile - 1);
            } else if (haveApps) {
                SetAppSelection(g_selectedApp - 1);
            }
            break;
        case VK::Enter:
            if (GetKeyState(VK_SHIFT) < 0) {
                OpenSelectedContextMenu();
            } else if (ResultsPending()) {
                // Typed faster than the search: what is on screen is for
                // an earlier text, or nothing yet. RenderResults opens the
                // first result once the ones for this text are in.
                g_enterWaiting = true;
                g_enterWaitingCtrl = ctrl;
                g_enterWaitingTick = now;
                Wh_Log(L"nav: Enter before the results; waiting for them");
            } else {
                ActivateSelection(ctrl);
            }
            break;
        case VK::Tab: {
            // Used even at either end of the list, so focus never tabs out of
            // the search box.
            const int appCount = haveApps ? static_cast<int>(g_appButtonsOpt->size()) : 0;
            if (GetKeyState(VK_SHIFT) >= 0) {
                if (inFiles) {
                    SetFileSelection(g_selectedFile + 1);
                } else if (haveApps && g_selectedApp < appCount - 1) {
                    SetAppSelection(g_selectedApp < 0 ? 0 : g_selectedApp + 1);
                } else if (haveFiles) {
                    SetFileSelection(0);
                }
            } else {
                if (inFiles && g_selectedFile > 0) {
                    SetFileSelection(g_selectedFile - 1);
                } else if (inFiles && haveApps) {
                    SetAppSelection(appCount - 1);
                } else if (!inFiles && haveApps) {
                    SetAppSelection(g_selectedApp - 1);
                }
            }
            break;
        }
        case VK::Right:
            used = !inFiles && haveFiles && CaretAtEndOfQuery();
            if (used) {
                toFiles();
            }
            break;
        case VK::Left:
            used = inFiles && haveApps;
            if (used) {
                toApps();
            }
            break;
        default:
            used = false;
            break;
    }

    s_lastKey = key;
    s_lastUsed = used;
    g_lastNavTick = now;
    return used;
}

// Results palette:
// Search box at top (Row 0),
// Dual-column apps and files in middle (Row 1),
// and keyboard navigation footer at bottom (Row 2).
void ApplyFilesPanelLayout() {
    if(!g_resultsHost || g_resultsHost.Children().Size()<2) return;
    const bool visible=FilesPanelVisible();
    auto grid=g_resultsHost.Children().GetAt(1).as<wuxc::Grid>();
    if(grid.ColumnDefinitions().Size()<3 || grid.Children().Size()<3) return;
    grid.ColumnDefinitions().GetAt(0).Width(wux::GridLengthHelper::FromValueAndType(visible ? 40 : 1,wux::GridUnitType::Star));
    grid.ColumnDefinitions().GetAt(2).Width(visible ? wux::GridLengthHelper::FromValueAndType(60,wux::GridUnitType::Star)
                                                  : wux::GridLengthHelper::FromValueAndType(0,wux::GridUnitType::Pixel));
    grid.Children().GetAt(2).Visibility(visible ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    if(g_divider) g_divider.Visibility(visible ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    static std::optional<bool> previous;
    if(previous!=visible) { HidePreview(); previous=visible; }
    if(!visible) { g_filesColumnActive=false; g_selectedFile=-1; }
    if(g_footerStatus) {
        g_footerStatus.MaxWidth(visible ? std::numeric_limits<double>::infinity() : 80);
        g_footerStatus.TextTrimming(wux::TextTrimming::CharacterEllipsis);
    }
    if(g_footerHints) {
        auto hints=g_footerHints.Children();
        for(uint32_t i=0;i<hints.Size();++i)
            hints.GetAt(i).Visibility(visible || i==2 || i==5 ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    }
}

// Use the installed Start menu's artwork; no asset is bundled with the mod.
// Raw pixels are cached per theme. WinRT decoding starts only with the UI.
const std::vector<BYTE>& NativeSearchIconPixels(bool light) {
    static std::array<std::vector<BYTE>,2> cached;
    static std::array<bool,2> attempted{};
    const size_t variant=light ? 1 : 0;
    if(attempted[variant]) return cached[variant];
    attempted[variant]=true;
    wchar_t windows[MAX_PATH]{};
    const UINT length=GetWindowsDirectoryW(windows,ARRAYSIZE(windows));
    if(!length || length>=ARRAYSIZE(windows)) return cached[variant];
    static constexpr PCWSTR folders[] = {
        L"\\SystemApps\\MicrosoftWindows.Client.Core_cw5n1h2txyewy\\StartMenu\\Assets\\SearchIcons\\",
        L"\\SystemApps\\Microsoft.Windows.StartMenuExperienceHost_cw5n1h2txyewy\\Assets\\SearchIcons\\",
        L"\\SystemApps\\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\\StartMenu\\Assets\\SearchIcons\\",
        L"\\SystemApps\\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\\Assets\\SearchIcons\\",
    };
    for(PCWSTR folder:folders) for(int scale:{400,200,100}) {
        const std::wstring path=std::wstring(windows)+folder+L"SearchIconOn"+
            (light ? L"Light" : L"Dark")+L".scale-"+std::to_wstring(scale)+L".png";
        const DWORD attributes=GetFileAttributesW(path.c_str());
        if(attributes==INVALID_FILE_ATTRIBUTES || (attributes&FILE_ATTRIBUTE_DIRECTORY)) continue;
        preview::RunInMta([&] {
            namespace imaging=winrt::Windows::Graphics::Imaging;
            auto file=winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path).get();
            auto decoder=imaging::BitmapDecoder::CreateAsync(file.OpenReadAsync().get()).get();
            const uint32_t width=decoder.PixelWidth(),height=decoder.PixelHeight();
            if(!width || width!=height || width>256) return;
            auto data=decoder.GetPixelDataAsync(imaging::BitmapPixelFormat::Bgra8,
                imaging::BitmapAlphaMode::Premultiplied,imaging::BitmapTransform(),
                imaging::ExifOrientationMode::IgnoreExifOrientation,imaging::ColorManagementMode::DoNotColorManage).get();
            auto bytes=data.DetachPixelData();
            if(bytes.size()==static_cast<size_t>(width)*height*4)
                cached[variant].assign(bytes.begin(),bytes.end());
        });
        if(!cached[variant].empty()) {
            SearchWindowTrace(light ? L"native Windows search icon loaded (light)" :
                                      L"native Windows search icon loaded (dark)",S_OK);
            return cached[variant];
        }
    }
    SearchWindowTrace(L"native search icon unavailable; using Windows search glyph",S_FALSE);
    return cached[variant];
}

void UpdateSearchIcon(bool light) {
    if(!g_searchBarBorder || !g_searchBarBorder.Child()) return;
    auto grid=g_searchBarBorder.Child().as<wuxc::Grid>();
    auto icon=grid.Children().GetAt(0).as<wuxc::Border>();
    HWND host=GetSearchWindow();
    const UINT dpi=host ? GetDpiForWindow(host) : GetDpiForSystem();
    const double scale=(dpi ? dpi : 96)/96.0;
    const int pixels=std::clamp(static_cast<int>(std::lround(Fs(20)*scale)),1,256);
    const double dip=pixels/scale;
    icon.Width(dip); icon.Height(dip);
    HIGHCONTRASTW contrast{sizeof(contrast)};
    const bool highContrast=SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(contrast),&contrast,0) &&
                            (contrast.dwFlags&HCF_HIGHCONTRASTON);
    const int variant=highContrast ? 2 : (light ? 1 : 0);
    const int cacheKey=variant*512+pixels;
    if(winrt::unbox_value_or<int>(icon.Tag(),-1)==cacheKey) return;
    if(!highContrast) {
        const auto& source=NativeSearchIconPixels(light);
        if(const int side=icons::IconSide(source); side>0) {
            // Average the original's premultiplied pixels at the final display
            // size so the circular edge stays smooth. A Viewbox would leave
            // this downsampling to the render transform, including text scale.
            if(auto bitmap=IconBitmap(icons::Resample(source,side,pixels))) {
                wuxc::Image image;
                image.Width(dip); image.Height(dip); image.Stretch(wuxm::Stretch::Uniform);
                image.UseLayoutRounding(true);
                image.Source(bitmap); icon.Child(image); icon.Tag(winrt::box_value(cacheKey));
                return;
            }
        }
    }
    auto glyph = SearchFontIcon();
    glyph.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
    glyph.Glyph(L"\uE721"); glyph.FontSize(dip);
    glyph.HorizontalAlignment(wux::HorizontalAlignment::Center);
    glyph.VerticalAlignment(wux::VerticalAlignment::Center);
    icon.Child(glyph); icon.Tag(winrt::box_value(cacheKey));
}

void BuildResultsList(wuxc::Panel const& ownerPanel) try {
    if (g_resultsHost) {
        return;
    }

    if (!g_activeAppsOpt) g_activeAppsOpt.emplace();
    if (!g_appButtonsOpt) g_appButtonsOpt.emplace();
    if (!g_fileButtonsOpt) g_fileButtonsOpt.emplace();

    wuxc::Grid root;
    root.Name(L"WindhawkEverythingResults");
    root.Margin(wux::ThicknessHelper::FromUniformLength(0));
    root.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    root.VerticalAlignment(wux::VerticalAlignment::Stretch);
    root.Visibility(wux::Visibility::Visible);
    root.Opacity(0.0);
    root.IsHitTestVisible(false);
    root.Padding(wux::ThicknessHelper::FromLengths(16, 12, 16, 8));
    root.CornerRadius(wux::CornerRadius{8, 8, 8, 8});

    // Root layout:
    // Row 0 (Auto): Search bar inside the overlay
    // Row 1 (1*): Dual-column Results (Apps 40* / hairline divider / Files 60*)
    // Row 2 (Auto): Status Footer
    wuxc::RowDefinition searchRowDef, resultsRowDef, footerRowDef;
    searchRowDef.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    resultsRowDef.Height(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    footerRowDef.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    root.RowDefinitions().Append(searchRowDef);
    root.RowDefinitions().Append(resultsRowDef);
    root.RowDefinitions().Append(footerRowDef);

    bool isLight = IsLightTheme();

    // Search bar container at Row 0
    wuxc::Border searchBarBorder;
    searchBarBorder.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 8));
    searchBarBorder.Padding(wux::ThicknessHelper::FromLengths(12, 0, 12, 0));
    searchBarBorder.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
    searchBarBorder.Background(isLight ? MakeBrush(0xD0, 0xFF, 0xFF, 0xFF) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
    searchBarBorder.BorderBrush(isLight ? MakeBrush(0x30, 0x00, 0x00, 0x00) : MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
    searchBarBorder.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
    searchBarBorder.Height(Fs(40));  // the text size's (Wh_ModSettingsChanged)
    wuxc::Grid::SetRow(searchBarBorder, 0);
    g_searchBarBorder = searchBarBorder;

    wuxc::Grid searchBarGrid;
    wuxc::ColumnDefinition sbIconCol, sbBoxCol;
    sbIconCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    sbBoxCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    searchBarGrid.ColumnDefinitions().Append(sbIconCol);
    searchBarGrid.ColumnDefinitions().Append(sbBoxCol);

    wuxc::Border searchIcon;
    searchIcon.Width(Fs(20)); searchIcon.Height(Fs(20));
    searchIcon.Margin(wux::ThicknessHelper::FromLengths(0,0,8,0));
    searchIcon.VerticalAlignment(wux::VerticalAlignment::Center);
    searchIcon.UseLayoutRounding(true);
    searchIcon.IsHitTestVisible(false);
    wuxc::Grid::SetColumn(searchIcon,0);
    searchBarGrid.Children().Append(searchIcon);

    wuxc::TextBox box;
    box.Name(L"WindhawkStartSearchBox");
    box.PlaceholderText(L"Search Anything");
    box.VerticalAlignment(wux::VerticalAlignment::Center);
    box.VerticalContentAlignment(wux::VerticalAlignment::Center);
    ScaleFont(box, 14);
    // As tall as its text and centered in the bar, at any text size: no
    // minimum height, the same space above the text as below.
    box.MinHeight(0);
    box.Padding(wux::ThicknessHelper::FromLengths(0, 5, 0, 5));
    box.Background(MakeBrush(0, 0, 0, 0));
    box.BorderThickness(wux::ThicknessHelper::FromUniformLength(0));
    box.IsTabStop(true);
    box.TabIndex(0);
    box.IsSpellCheckEnabled(false);
    box.IsTextPredictionEnabled(false);

    static const wchar_t* kClearKeys[] = {
        L"TextControlBackground",
        L"TextControlBackgroundPointerOver",
        L"TextControlBackgroundFocused",
        L"TextControlBackgroundDisabled",
        L"TextControlBorderBrush",
        L"TextControlBorderBrushPointerOver",
        L"TextControlBorderBrushFocused",
        L"TextControlBorderBrushDisabled",
        L"TextControlButtonBackground",
        L"TextControlButtonBackgroundPointerOver",
        L"TextControlButtonBackgroundPressed",
    };
    for (const wchar_t* key : kClearKeys) {
        box.Resources().Insert(winrt::box_value(winrt::hstring{key}),
                               wuxm::SolidColorBrush{winrt::Windows::UI::Colors::Transparent()});
    }
    // Start's TextBox style still draws a background of its own when focused,
    // a lighter pill inside the bar: its template's BorderElement, hidden so
    // the bar is one surface. The bar has the border.
    KeepHandler(box, box.Loaded(winrt::auto_revoke, [](wf::IInspectable const& sender, wux::RoutedEventArgs const&) {
        std::function<bool(wux::DependencyObject const&)> hide = [&](wux::DependencyObject const& node) {
            const int32_t count = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int32_t i = 0; i < count; ++i) {
                auto child = wuxm::VisualTreeHelper::GetChild(node, i);
                if (auto element = child.try_as<wux::FrameworkElement>(); element && element.Name() == L"BorderElement") {
                    element.Opacity(0);
                    return true;
                }
                if (hide(child)) {
                    return true;
                }
            }
            return false;
        };
        try {
            hide(sender.as<wux::DependencyObject>());
        } catch (...) {
        }
    }));

    g_ourBoxChanged = box.TextChanged(
        winrt::auto_revoke,
        [](wf::IInspectable const& sender, wuxc::TextChangedEventArgs const&) {
            try {
                auto b = sender.as<wuxc::TextBox>();
                std::wstring text{b.Text()};
                Wh_Log(L"own box: '%ls'", text.c_str());
                if (text.empty()) {
                    UpdateSearchResultsLayout(true);
                } else {
                    UpdateSearchResultsLayout();
                }
                QueueQuery(std::move(text));
            } catch (...) {}
        });

    KeepHandler(box, box.PreviewKeyDown(winrt::auto_revoke, [](wf::IInspectable const&, wux::Input::KeyRoutedEventArgs const& args) {
        try {
            auto key = args.Key();
            if (key == winrt::Windows::System::VirtualKey::Escape && !EscapeBelongsToContextMenu()) {
                if (g_ourBox && !g_ourBox.Text().empty()) g_ourBox.Text(L"");
                else SearchWindowHide();
                args.Handled(true);
                return;
            }
            if (IsNavigationKey(key) && g_isOverlayVisible.load()) {
                bool ctrl = (GetKeyState(VK_CONTROL) < 0) || ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0);
                if (HandleNavigationKey(key, ctrl)) {
                    args.Handled(true);
                }
            }
        } catch (...) {}
    }));

    wuxc::Grid::SetColumn(box, 1);
    searchBarGrid.Children().Append(box);
    g_ourBox = box;

    searchBarBorder.Child(searchBarGrid);
    UpdateSearchIcon(isLight);
    root.Children().Append(searchBarBorder);

    // Results container grid: Apps column (40*), 1px hairline divider, Files column (60*)
    wuxc::Grid resultsGrid;
    wuxc::Grid::SetRow(resultsGrid, 1);

    wuxc::ColumnDefinition appsCol, divCol, filesCol;
    appsCol.Width(wux::GridLengthHelper::FromValueAndType(40, wux::GridUnitType::Star));
    divCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    filesCol.Width(wux::GridLengthHelper::FromValueAndType(60, wux::GridUnitType::Star));
    resultsGrid.ColumnDefinitions().Append(appsCol);
    resultsGrid.ColumnDefinitions().Append(divCol);
    resultsGrid.ColumnDefinitions().Append(filesCol);

    wuxc::StackPanel apps;

    wuxc::Grid appsColGrid;
    wuxc::RowDefinition aHeadRow, aListRow;
    aHeadRow.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    aListRow.Height(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    appsColGrid.RowDefinitions().Append(aHeadRow);
    appsColGrid.RowDefinitions().Append(aListRow);
    wuxc::Grid::SetColumn(appsColGrid, 0);

    wuxc::Border appsHeaderHolder;
    wuxc::Grid::SetRow(appsHeaderHolder, 0);
    appsColGrid.Children().Append(appsHeaderHolder);
    g_appsHeaderHolder = appsHeaderHolder;

    wuxc::ScrollViewer appsScroll;
    appsScroll.Content(apps);
    g_appsScroll = appsScroll;
    KeepHandler(appsScroll,appsScroll.ViewChanged(winrt::auto_revoke,[](wf::IInspectable const& sender,auto const&) {
        SearchContrastScrolled(sender.as<wuxc::ScrollViewer>());
    }));
    appsScroll.IsTabStop(false);
    appsScroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    appsScroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
    wuxc::Grid::SetRow(appsScroll, 1);
    appsColGrid.Children().Append(appsScroll);

    resultsGrid.Children().Append(appsColGrid);

    wuxc::Border divider;
    divider.Width(1);
    divider.HorizontalAlignment(wux::HorizontalAlignment::Center);
    divider.VerticalAlignment(wux::VerticalAlignment::Stretch);
    divider.Background(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x14, 0xFF, 0xFF, 0xFF));
    divider.Margin(wux::ThicknessHelper::FromLengths(6, 4, 6, 4));
    wuxc::Grid::SetColumn(divider, 1);
    resultsGrid.Children().Append(divider);
    g_divider = divider;

    wuxc::StackPanel files;
    wuxc::Grid filesColGrid;
    wuxc::RowDefinition fHeadRow, fListRow;
    fHeadRow.Height(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    fListRow.Height(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    filesColGrid.RowDefinitions().Append(fHeadRow);
    filesColGrid.RowDefinitions().Append(fListRow);
    wuxc::Grid::SetColumn(filesColGrid, 2);

    wuxc::Border filesHeaderHolder;
    wuxc::Grid::SetRow(filesHeaderHolder, 0);
    filesColGrid.Children().Append(filesHeaderHolder);
    g_filesHeaderHolder = filesHeaderHolder;

    wuxc::ScrollViewer filesScroll;
    filesScroll.Content(files);
    g_filesScroll = filesScroll;
    KeepHandler(filesScroll,filesScroll.ViewChanged(winrt::auto_revoke,[](wf::IInspectable const& sender,auto const&) {
        SearchContrastScrolled(sender.as<wuxc::ScrollViewer>());
    }));
    filesScroll.IsTabStop(false);
    filesScroll.VerticalScrollBarVisibility(wuxc::ScrollBarVisibility::Auto);
    filesScroll.HorizontalScrollBarVisibility(wuxc::ScrollBarVisibility::Disabled);
    wuxc::Grid::SetRow(filesScroll, 1);
    filesColGrid.Children().Append(filesScroll);

    resultsGrid.Children().Append(filesColGrid);

    root.Children().Append(resultsGrid);

    // Modern Raycast-style bottom status and shortcut bar
    wuxc::Border footerBorder;
    footerBorder.Margin(wux::ThicknessHelper::FromLengths(0, 6, 0, 0));
    footerBorder.Padding(wux::ThicknessHelper::FromLengths(8, 6, 8, 4));
    footerBorder.BorderBrush(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x12, 0xFF, 0xFF, 0xFF));
    footerBorder.BorderThickness(wux::ThicknessHelper::FromLengths(0, 1, 0, 0));
    wuxc::Grid::SetRow(footerBorder, 2);
    g_footerBorder = footerBorder;

    wuxc::Grid footerGrid;
    wuxc::ColumnDefinition fColLeft, fColRight;
    fColLeft.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
    fColRight.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
    footerGrid.ColumnDefinitions().Append(fColLeft);
    footerGrid.ColumnDefinitions().Append(fColRight);

    wuxc::StackPanel leftStatus;
    leftStatus.Orientation(wuxc::Orientation::Horizontal);
    leftStatus.VerticalAlignment(wux::VerticalAlignment::Center);

    auto boltIcon = SearchFontIcon();
    boltIcon.Glyph(L"\uE946");
    ScaleFont(boltIcon, 11);
    boltIcon.Opacity(0.5);
    boltIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 6, 0));
    boltIcon.VerticalAlignment(wux::VerticalAlignment::Center);
    leftStatus.Children().Append(boltIcon);

    auto statusText = SearchTextBlock();
    statusText.Text(L"Everything Search");
    ScaleFont(statusText, 11);
    statusText.Opacity(0.5);
    statusText.VerticalAlignment(wux::VerticalAlignment::Center);
    leftStatus.Children().Append(statusText);
    g_footerStatus = statusText;

    wuxc::Grid::SetColumn(leftStatus, 0);
    footerGrid.Children().Append(leftStatus);

    wuxc::StackPanel rightHints;
    rightHints.Orientation(wuxc::Orientation::Horizontal);
    rightHints.VerticalAlignment(wux::VerticalAlignment::Center);

    auto makeKeyCap = [isLight](const wchar_t* key, const wchar_t* action) {
        wuxc::StackPanel pair;
        pair.Orientation(wuxc::Orientation::Horizontal);
        pair.VerticalAlignment(wux::VerticalAlignment::Center);
        pair.Margin(wux::ThicknessHelper::FromLengths(8, 0, 0, 0));

        wuxc::Border keyBadge;
        keyBadge.CornerRadius(wux::CornerRadius{3, 3, 3, 3});
        keyBadge.Background(isLight ? MakeBrush(0x0C, 0x00, 0x00, 0x00) : MakeBrush(0x18, 0xFF, 0xFF, 0xFF));
        keyBadge.BorderBrush(isLight ? MakeBrush(0x20, 0x00, 0x00, 0x00) : MakeBrush(0x24, 0xFF, 0xFF, 0xFF));
        keyBadge.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        keyBadge.Padding(wux::ThicknessHelper::FromLengths(4, 1, 4, 1));
        keyBadge.VerticalAlignment(wux::VerticalAlignment::Center);

        auto keyBlock = SearchTextBlock();
        keyBlock.Text(winrt::hstring{key});
        ScaleFont(keyBlock, 9.5);
        keyBlock.FontWeight(wut::FontWeights::SemiBold());
        keyBlock.Opacity(0.75);
        keyBadge.Child(keyBlock);
        pair.Children().Append(keyBadge);

        auto actionBlock = SearchTextBlock();
        actionBlock.Text(winrt::hstring{action});
        ScaleFont(actionBlock, 10.5);
        actionBlock.Opacity(0.45);
        actionBlock.Margin(wux::ThicknessHelper::FromLengths(4, 0, 0, 0));
        actionBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        pair.Children().Append(actionBlock);

        return pair;
    };

    rightHints.Children().Append(makeKeyCap(L"\u2191\u2193", L"Select"));
    rightHints.Children().Append(makeKeyCap(L"Tab", L"Next"));
    rightHints.Children().Append(makeKeyCap(L"\u21B5", L"Open"));
    rightHints.Children().Append(makeKeyCap(L"Ctrl+\u21B5", L"Admin"));
    rightHints.Children().Append(makeKeyCap(L"\u21E7\u21B5", L"Menu"));
    rightHints.Children().Append(makeKeyCap(L"Esc", L"Close"));

    wuxc::Grid::SetColumn(rightHints, 1);
    g_footerHints = rightHints;
    bool showHints = true;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        showHints = g_settings.showKeyHints;
    }
    rightHints.Visibility(showHints ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    footerGrid.Children().Append(rightHints);

    footerBorder.Child(footerGrid);
    root.Children().Append(footerBorder);

    // Spanning every row and column of the menu's grid.
    wuxc::Grid::SetRow(root, 0);
    wuxc::Grid::SetRowSpan(root, 12);
    wuxc::Grid::SetColumn(root, 0);
    wuxc::Grid::SetColumnSpan(root, 12);

    ownerPanel.Children().Append(root);
    g_appsList = apps;
    g_resultsList = files;
    g_resultsHost = root;


    if (!root.Background()) {
        root.Background(isLight ? MakeBrush(0xF2, 0xF3, 0xF3, 0xF3) : MakeBrush(0xF2, 0x20, 0x20, 0x20));
        Wh_Log(L"results: no AcrylicBorder found; flat background instead");
    }

    ApplyFilesPanelLayout();
    Wh_Log(L"results: two columns and footer built");
} catch (...) {
    Wh_Log(L"results list failed %08X", static_cast<unsigned>(winrt::to_hresult()));
}

// Runs on the XAML thread.
void RenderResults() try {
    if (g_searchWindowSliding) { g_renderAfterSlide = true; return; }
    g_renderAfterSlide = false;
    if (!g_resultsList || !g_resultsHost || !g_appsList || !g_activeAppsOpt || !g_appButtonsOpt) {
        return;
    }

    bool isLight = IsLightTheme();
    const bool filesVisible=FilesPanelVisible();
    ApplyFilesPanelLayout();

    std::vector<Row> files;
    std::vector<Row> appNames;
    DWORD total = 0;
    {
        std::lock_guard<std::mutex> lock(g_resultsMutex);
        files = g_fileRows;
        appNames = g_appRows;
        g_shownQuery = g_rowsQuery;
        total = g_totalMatches.load();
    }

    if(!filesVisible) { files.clear(); total=0; }
    if (files.empty() && appNames.empty() && g_ourBox && g_ourBox.Text().empty()) {
        g_enterWaiting = false;
        g_currentAppRows.clear();
        g_currentFileRows.clear();
        g_selectedApp = -1;
        g_selectedFile = -1;
        g_filesColumnActive = false;
        UpdateSearchResultsLayout(true);
        Wh_Log(L"render: nothing to show; menu restored");
        return;
    }

    g_resultsList.Children().Clear();
    g_currentAppRows = appNames;

    const bool suggestions=g_searchWindowWindow.load() && g_shownQuery.empty();
    // Update bottom status bar
    if (g_footerStatus) {
        g_footerStatus.Opacity(0.5);
        g_footerStatus.TextWrapping(wux::TextWrapping::NoWrap);
        wuxc::ToolTipService::SetToolTip(g_footerStatus,nullptr);
        if (suggestions) {
            g_footerStatus.Text(filesVisible ? L"Suggested apps" : L"Apps");
        } else if(!filesVisible) {
            g_footerStatus.Text(L"Apps");
        } else if (!appNames.empty() && (appNames[0].customGlyph == L"\uE701" || appNames[0].customGlyph == L"\uE704" || appNames[0].customGlyph == L"\uE839")) {
            std::wstring statusStr = L"Network Interfaces \u2022 " + std::to_wstring(appNames.size()) + L" active \u2022 Press Enter to copy IP";
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE88E") {
            std::wstring statusStr = L"Unit Converter \u2022 " + std::to_wstring(appNames.size()) + L" conversions \u2022 Press Enter to copy";
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE1D0") {
            std::wstring statusStr = L"Calculator \u2022 Press Enter to copy result";
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else if (!appNames.empty() && appNames[0].appIndex < 0 &&
                   (appNames[0].openPath.starts_with(L"http:") || appNames[0].openPath.starts_with(L"https:"))) {
            std::wstring statusStr = L"Web Search \u2022 Press Enter to search in default browser";
            if (!files.empty()) {
                statusStr += L" (" + std::to_wstring(files.size()) + L" files matched)";
            }
            g_footerStatus.Text(winrt::hstring{statusStr});
        } else {
            std::wstring statusStr;
            if (everything::FindIpcWindow()) {
                statusStr = L"Everything \u2022 " + std::to_wstring(total) + L" matches";
                if (!appNames.empty() || !files.empty()) {
                    statusStr += L" (" + std::to_wstring(appNames.size()) + L" apps, " + std::to_wstring(files.size()) + L" files shown)";
                }
            } else {
                statusStr = L"Everything (not running)";
                if (!appNames.empty()) {
                    statusStr += L" \u2022 " + std::to_wstring(appNames.size()) + L" apps shown";
                }
            }
            g_footerStatus.Text(winrt::hstring{statusStr});
        }
    }

    if (g_footerHints) {
        bool showHints = true;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            showHints = g_settings.showKeyHints;
        }
        g_footerHints.Visibility(showHints ? wux::Visibility::Visible : wux::Visibility::Collapsed);
    }

    auto makeHeader = [isLight](const std::wstring& title, const wchar_t* iconGlyph, int count, const std::wstring& badgeText = L"") {
        wuxc::Grid headerGrid;
        headerGrid.Margin(wux::ThicknessHelper::FromLengths(8, 4, 8, 6));

        wuxc::ColumnDefinition leftCol, rightCol;
        leftCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        rightCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
        headerGrid.ColumnDefinitions().Append(leftCol);
        headerGrid.ColumnDefinitions().Append(rightCol);

        wuxc::StackPanel leftStack;
        leftStack.Orientation(wuxc::Orientation::Horizontal);
        leftStack.VerticalAlignment(wux::VerticalAlignment::Center);

        auto icon = SearchFontIcon();
        icon.Glyph(winrt::hstring{iconGlyph});
        ScaleFont(icon, 11.5);
        icon.Opacity(0.6);
        icon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 6, 0));
        icon.VerticalAlignment(wux::VerticalAlignment::Center);
        leftStack.Children().Append(icon);

        auto titleBlock = SearchTextBlock();
        titleBlock.Text(winrt::hstring{title});
        ScaleFont(titleBlock, 11);
        titleBlock.FontWeight(wut::FontWeights::SemiBold());
        titleBlock.Opacity(0.65);
        titleBlock.CharacterSpacing(40);
        titleBlock.VerticalAlignment(wux::VerticalAlignment::Center);
        leftStack.Children().Append(titleBlock);

        wuxc::Grid::SetColumn(leftStack, 0);
        headerGrid.Children().Append(leftStack);

        std::wstring badgeStr = badgeText.empty() ? std::to_wstring(count) : badgeText;
        if (!badgeStr.empty() && count >= 0) {
            wuxc::Border badge;
            badge.CornerRadius(wux::CornerRadius{4, 4, 4, 4});
            badge.Background(isLight ? MakeBrush(0x0E, 0x00, 0x00, 0x00) : MakeBrush(0x15, 0xFF, 0xFF, 0xFF));
            badge.BorderBrush(isLight ? MakeBrush(0x18, 0x00, 0x00, 0x00) : MakeBrush(0x20, 0xFF, 0xFF, 0xFF));
            badge.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
            badge.Padding(wux::ThicknessHelper::FromLengths(7, 1, 7, 1));
            badge.VerticalAlignment(wux::VerticalAlignment::Center);

            auto badgeBlock = SearchTextBlock();
            badgeBlock.Text(winrt::hstring{badgeStr});
            ScaleFont(badgeBlock, 9.5);
            badgeBlock.FontWeight(wut::FontWeights::SemiBold());
            badgeBlock.Opacity(0.7);
            badge.Child(badgeBlock);

            wuxc::Grid::SetColumn(badge, 1);
            headerGrid.Children().Append(badge);
        }

        return headerGrid;
    };

    // The web-search rows have no app index; a Start menu internet shortcut is an app.
    bool isWebMode = (!appNames.empty() && appNames[0].appIndex < 0 &&
                      (appNames[0].openPath.starts_with(L"http:") || appNames[0].openPath.starts_with(L"https:")));
    if (g_appsHeaderHolder) {
        if (!appNames.empty() && (appNames[0].customGlyph == L"\uE701" || appNames[0].customGlyph == L"\uE704" || appNames[0].customGlyph == L"\uE839")) {
            g_appsHeaderHolder.Child(makeHeader(L"NETWORK INTERFACES", L"\uE701", static_cast<int>(appNames.size())));
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE88E") {
            g_appsHeaderHolder.Child(makeHeader(L"UNIT CONVERTER", L"\uE88E", static_cast<int>(appNames.size())));
        } else if (!appNames.empty() && appNames[0].customGlyph == L"\uE1D0") {
            g_appsHeaderHolder.Child(makeHeader(L"CALCULATOR", L"\uE1D0", static_cast<int>(appNames.size())));
        } else if (isWebMode) {
            g_appsHeaderHolder.Child(makeHeader(L"WEB SEARCH", L"\uE774", static_cast<int>(appNames.size())));
        } else {
            g_appsHeaderHolder.Child(makeHeader(suggestions ? L"SUGGESTED APPS" : L"APPS", L"\uE71D", static_cast<int>(appNames.size())));
        }
    }
    std::wstring filesBadge = std::to_wstring(files.size()) + L" of " + std::to_wstring(total);
    if (g_filesHeaderHolder) {
        g_filesHeaderHolder.Child(makeHeader(L"FILES", L"\uE8B7", static_cast<int>(files.size()), filesBadge));
    }

    auto makeAppCard = [isLight](const Row& item) -> AppCardUI {
        wuxc::Grid layout;
        wuxc::ColumnDefinition iconCol, textCol;
        iconCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
        textCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        layout.ColumnDefinitions().Append(iconCol);
        layout.ColumnDefinitions().Append(textCol);

        bool hasBitmap = false;
        if (auto bmp = IconBitmap(item.icon)) {
            wuxc::Image image;
            image.Source(bmp);
            image.Width(kIconDisplay);
            image.Height(kIconDisplay);
            image.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            image.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetColumn(image, 0);
            layout.Children().Append(image);
            hasBitmap = true;
        }

        if (!hasBitmap) {
            wuxc::Border iconBox;
            iconBox.Width(kIconDisplay);
            iconBox.Height(kIconDisplay);
            iconBox.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            iconBox.VerticalAlignment(wux::VerticalAlignment::Center);

            auto fallbackIcon = SearchFontIcon();
            ScaleFont(fallbackIcon, 15);
            fallbackIcon.Opacity(0.5);
            fallbackIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
            fallbackIcon.VerticalAlignment(wux::VerticalAlignment::Center);

            if (item.openPath.starts_with(L"ms-settings:") || item.subtitle.starts_with(L"Settings")) {
                fallbackIcon.Glyph(L"\uE713");
                fallbackIcon.Opacity(0.7);
            } else if (item.openPath.starts_with(L"http:") || item.openPath.starts_with(L"https:")) {
                fallbackIcon.Glyph(L"\uE774");
                fallbackIcon.Opacity(0.85);
            } else if (!item.customGlyph.empty()) {
                fallbackIcon.Glyph(winrt::hstring{item.customGlyph});
                fallbackIcon.Opacity(0.85);
                if (const uint32_t c = item.glyphColor) {
                    fallbackIcon.Foreground(MakeBrush(static_cast<uint8_t>(c >> 24), static_cast<uint8_t>(c >> 16),
                                                      static_cast<uint8_t>(c >> 8), static_cast<uint8_t>(c)));
                    fallbackIcon.Opacity(1.0);
                }
            } else {
                fallbackIcon.Glyph(L"\uE71D");
            }
            if (item.customIconIsGlyph) {
                fallbackIcon.FontFamily(wuxm::FontFamily(L"Segoe Fluent Icons"));
                fallbackIcon.Glyph(winrt::hstring{item.customIconText});
                fallbackIcon.Opacity(1.0);
                ScaleFont(fallbackIcon, 20);
                iconBox.Child(fallbackIcon);
            } else if (!item.customIconText.empty()) {
                auto emoji = SearchTextBlock();
                emoji.Text(winrt::hstring{item.customIconText});
                emoji.FontFamily(wuxm::FontFamily(L"Segoe UI Emoji"));
                emoji.IsColorFontEnabled(true);
                ScaleFont(emoji, 20);
                emoji.HorizontalAlignment(wux::HorizontalAlignment::Center);
                emoji.VerticalAlignment(wux::VerticalAlignment::Center);
                iconBox.Child(emoji);
            } else {
                iconBox.Child(fallbackIcon);
            }
            wuxc::Grid::SetColumn(iconBox, 0);
            layout.Children().Append(iconBox);
        }

        wuxc::StackPanel text;
        wuxc::Grid::SetColumn(text, 1);
        text.VerticalAlignment(wux::VerticalAlignment::Center);

        auto name = SearchTextBlock();
        name.Text(winrt::hstring{item.title});
        ScaleFont(name, 12.5);
        name.FontWeight(wut::FontWeights::SemiBold());
        name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        name.TextWrapping(wux::TextWrapping::NoWrap);
        // A result to copy -- a sum, a conversion -- is shown whole: a long
        // one wraps instead of being cut off.
        const bool whole = !item.copyText.empty();
        if (whole) {
            name.TextWrapping(wux::TextWrapping::Wrap);
            name.MaxLines(4);
        }
        text.Children().Append(name);

        if (!item.subtitle.empty()) {
            auto sub = SearchTextBlock();
            sub.Text(winrt::hstring{item.subtitle});
            sub.Opacity(0.45);
            ScaleFont(sub, 10.5);
            sub.Margin(wux::ThicknessHelper::FromLengths(0, 1, 0, 0));
            sub.TextTrimming(wux::TextTrimming::CharacterEllipsis);
            sub.TextWrapping(whole ? wux::TextWrapping::Wrap : wux::TextWrapping::NoWrap);
            if (whole) {
                sub.MaxLines(2);
            }
            text.Children().Append(sub);
        }
        layout.Children().Append(text);

        wuxc::Button button;
        button.Content(layout);
        button.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        button.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
        button.Background(MakeBrush(0, 0, 0, 0));
        button.BorderBrush(MakeBrush(0, 0, 0, 0));
        button.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        button.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
        button.Padding(wux::ThicknessHelper::FromLengths(10, 6, 10, 6));
        button.Margin(wux::ThicknessHelper::FromLengths(2, 1, 2, 1));
        button.IsTabStop(false);
        button.AllowFocusOnInteraction(false);
        if (whole) {
            wuxc::ToolTipService::SetToolTip(button, winrt::box_value(winrt::hstring{item.title}));
        }

        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPointerOver"),
            isLight ? MakeBrush(0x14, 0x00, 0x5F, 0xB8) : MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPointerOver"),
            isLight ? MakeBrush(0x80, 0x00, 0x5F, 0xB8) : MakeBrush(0x55, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPressed"),
            isLight ? MakeBrush(0x24, 0x00, 0x5F, 0xB8) : MakeBrush(0x35, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPressed"),
            isLight ? MakeBrush(0xA0, 0x00, 0x5F, 0xB8) : MakeBrush(0x70, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundFocused"),
            MakeBrush(0, 0, 0, 0));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushFocused"),
            MakeBrush(0, 0, 0, 0));

        // Weak: the button holds these handlers, and a strong self-reference
        // would keep every card ever built alive.
        KeepHandler(button, button.PointerEntered(winrt::auto_revoke, [weakBtn = winrt::make_weak(button)](wf::IInspectable const&,
                                                                   wux::Input::PointerRoutedEventArgs const&) {
            auto btn = weakBtn.get();
            if (!btn || !g_activeAppsOpt || g_dragging) return;  // a drag passing over
            for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                if ((*g_activeAppsOpt)[i].button == btn) {
                    SetAppSelection(static_cast<int>(i));
                    break;
                }
            }
        }));

        KeepHandler(button, button.Click(winrt::auto_revoke, [weakBtn = winrt::make_weak(button)](wf::IInspectable const&, wux::RoutedEventArgs const&) {
            auto btn = weakBtn.get();
            if (!btn || !g_activeAppsOpt) return;
            for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                if ((*g_activeAppsOpt)[i].button == btn) {
                    LaunchSelectedApp(static_cast<int>(i), false);
                    return;
                }
            }
        }));

        wuxc::MenuFlyout menu;
        KeepHandler(menu, menu.Opening(winrt::auto_revoke, [](wf::IInspectable const& sender, wf::IInspectable const&) { NoteContextMenuOpening(sender); }));
        KeepHandler(menu, menu.Closed(winrt::auto_revoke, [](wf::IInspectable const& sender, wf::IInspectable const&) { NoteContextMenuClosed(sender); }));
        // Filled in when first opened, not with the card (see the file rows).
        // What it needs of the row is copied, less the icon pixels; the card
        // is held weakly, since it owns this menu.
        Row menuItem = item;
        menuItem.icon.clear();
        KeepHandler(menu, menu.Opening(winrt::auto_revoke, [item = std::move(menuItem), weakBtn = winrt::make_weak(button)](
                         wf::IInspectable const& sender, wf::IInspectable const&) {
            auto flyout = sender.as<wuxc::MenuFlyout>();
            if (flyout.Items().Size() > 0) return;
            if (!item.copyText.empty()) {
                wuxc::MenuFlyoutItem copyItem;
                copyItem.Text(L"Copy to clipboard");
                auto copyIcon = SearchFontIcon();
                copyIcon.Glyph(L"\uE8C8");
                copyItem.Icon(copyIcon);
                KeepHandler(copyItem, copyItem.Click(winrt::auto_revoke, [txt = item.copyText](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyTextToClipboard(txt);
                }));
                flyout.Items().Append(copyItem);
            } else {
                bool isWebItem = item.appIndex < 0 && (item.openPath.starts_with(L"http:") || item.openPath.starts_with(L"https:"));
                bool isSettingItem = item.isSetting;
                const bool isCommandItem = !item.commandLine.empty();

                wuxc::MenuFlyoutItem openItem;
                openItem.Text(isCommandItem ? L"Run" : isWebItem ? L"Search in browser" : L"Open");
                auto openIcon = SearchFontIcon();
                openIcon.Glyph(isWebItem ? L"\uE774" : L"\uE8A7");
                openItem.Icon(openIcon);
                KeepHandler(openItem, openItem.Click(winrt::auto_revoke, [weakBtn](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    auto btn = weakBtn.get();
                    if (!btn || !g_activeAppsOpt) return;
                    for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                        if ((*g_activeAppsOpt)[i].button == btn) {
                            LaunchSelectedApp(static_cast<int>(i), false);
                            return;
                        }
                    }
                }));
                flyout.Items().Append(openItem);

                if (item.canRunAsAdmin && !isWebItem && !isSettingItem) {
                    wuxc::MenuFlyoutItem adminItem;
                    adminItem.Text(L"Run as administrator");
                    auto adminIcon = SearchFontIcon();
                    adminIcon.Glyph(L"\uE7EF");
                    adminItem.Icon(adminIcon);
                    KeepHandler(adminItem, adminItem.Click(winrt::auto_revoke, [weakBtn](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        auto btn = weakBtn.get();
                        if (!btn || !g_activeAppsOpt) return;
                        for (size_t i = 0; i < g_activeAppsOpt->size(); ++i) {
                            if ((*g_activeAppsOpt)[i].button == btn) {
                                LaunchSelectedApp(static_cast<int>(i), true);
                                return;
                            }
                        }
                    }));
                    flyout.Items().Append(adminItem);
                }

                if (isCommandItem) {
                    wuxc::MenuFlyoutItem copyCommand;
                    copyCommand.Text(L"Copy command");
                    auto copyIcon = SearchFontIcon();
                    copyIcon.Glyph(L"\uE8C8");
                    copyCommand.Icon(copyIcon);
                    KeepHandler(copyCommand, copyCommand.Click(winrt::auto_revoke, [command = item.commandLine](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        tools::CopyTextToClipboard(command);
                    }));
                    flyout.Items().Append(copyCommand);
                } else if (isWebItem) {
                    std::wstring webUrl = item.openPath;
                    wuxc::MenuFlyoutItem copyUrlItem;
                    copyUrlItem.Text(L"Copy search link");
                    auto copyIcon = SearchFontIcon();
                    copyIcon.Glyph(L"\uE8C8");
                    copyUrlItem.Icon(copyIcon);
                    KeepHandler(copyUrlItem, copyUrlItem.Click(winrt::auto_revoke, [webUrl](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        tools::CopyTextToClipboard(webUrl);
                    }));
                    flyout.Items().Append(copyUrlItem);
                } else if (!isSettingItem && !item.openPath.empty()) {
                    std::wstring locTarget = item.openPath;
                    std::wstring appTitle = item.title;
                    bool isFile = item.isFile;
                    bool viaAppId = !isFile && !locTarget.starts_with(L"ms-settings:") &&
                                    !locTarget.starts_with(L"http:") && !locTarget.starts_with(L"https:");
                    // An app launched by its app ID can still have a program
                    // on disk, which the card shows; the file actions use it.
                    std::wstring filePath = isFile ? locTarget : viaAppId ? item.programPath : L"";

                    if (isFile || viaAppId) {
                        wuxc::MenuFlyoutSeparator sep1;
                        flyout.Items().Append(sep1);

                        if (!filePath.empty()) {
                            wuxc::MenuFlyoutItem locItem;
                            locItem.Text(L"Open file location");
                            auto locIcon = SearchFontIcon();
                            locIcon.Glyph(L"\uE838");
                            locItem.Icon(locIcon);
                            KeepHandler(locItem, locItem.Click(winrt::auto_revoke, [filePath](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                OpenFileLocationThenDismiss(filePath);
                            }));
                            flyout.Items().Append(locItem);

                            wuxc::MenuFlyoutItem copyPathItem;
                            copyPathItem.Text(L"Copy path");
                            auto copyPathIcon = SearchFontIcon();
                            copyPathIcon.Glyph(L"\uE71B");
                            copyPathItem.Icon(copyPathIcon);
                            KeepHandler(copyPathItem, copyPathItem.Click(winrt::auto_revoke, [filePath](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                tools::CopyTextToClipboard(filePath);
                            }));
                            flyout.Items().Append(copyPathItem);
                        }

                        // Through the app ID where there is one, so the
                        // shortcut keeps the original's arguments.
                        std::wstring shortcutTarget = isFile ? locTarget : L"shell:AppsFolder\\" + locTarget;
                        wuxc::MenuFlyoutItem shortcutItem;
                        shortcutItem.Text(L"Create desktop shortcut");
                        auto shortcutIcon = SearchFontIcon();
                        shortcutIcon.Glyph(L"\uE7C5");
                        shortcutItem.Icon(shortcutIcon);
                        KeepHandler(shortcutItem, shortcutItem.Click(winrt::auto_revoke, [shortcutTarget, appTitle](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                            tools::CreateDesktopShortcut(shortcutTarget, appTitle);
                        }));
                        flyout.Items().Append(shortcutItem);

                        if (!filePath.empty()) {
                            wuxc::MenuFlyoutSeparator sep2;
                            flyout.Items().Append(sep2);

                            wuxc::MenuFlyoutItem propItem;
                            propItem.Text(L"Properties");
                            auto propIcon = SearchFontIcon();
                            propIcon.Glyph(L"\uE946");
                            propItem.Icon(propIcon);
                            KeepHandler(propItem, propItem.Click(winrt::auto_revoke, [filePath](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                AllowExplorerForeground();
                                DismissSearch();
                                ShowPropertiesDialog(filePath);
                            }));
                            flyout.Items().Append(propItem);
                        }

                        // Uninstall, as in Start's own menu: a packaged app is
                        // removed, a program runs its uninstaller (uninstall).
                        // Offered only where one of them applies.
                        std::wstring family = viaAppId ? uninstall::RemovablePackageFamily(locTarget) : std::wstring();
                        std::optional<uninstall::Command> uninstaller;
                        if (family.empty()) {
                            std::wstring program = filePath;
                            if (std::wstring target = apps::ResolveLnkTarget(program); !target.empty()) {
                                program = target;
                            }
                            uninstaller = uninstall::FindProgramUninstaller(program, appTitle);
                        }
                        if (!family.empty() || uninstaller) {
                            wuxc::MenuFlyoutSeparator sep3;
                            flyout.Items().Append(sep3);

                            wuxc::MenuFlyoutItem uninstallItem;
                            uninstallItem.Text(L"Uninstall");
                            auto uninstallIcon = SearchFontIcon();
                            uninstallIcon.Glyph(L"\uE74D");
                            uninstallItem.Icon(uninstallIcon);
                            KeepHandler(uninstallItem, uninstallItem.Click(winrt::auto_revoke, [weakBtn, family, uninstaller, appTitle](
                                                                               wf::IInspectable const&, wux::RoutedEventArgs const&) {
                                auto btn = weakBtn.get();
                                if (!btn) return;
                                // After the menu has closed: one flyout at a time.
                                btn.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Low, wuc::DispatchedHandler{[weakBtn, family, uninstaller, appTitle] {
                                    auto anchor = weakBtn.get();
                                    if (!anchor) return;
                                    ConfirmUninstall(anchor, uninstaller ? uninstaller->name : appTitle, [family, uninstaller, appTitle] {
                                        if (!family.empty()) {
                                            if (RequestPackageRemoval(family)) {
                                                DismissSearch();
                                            }
                                        } else if (uninstaller) {
                                            DismissSearchForLaunch([command = *uninstaller] { RunUninstaller(command); });
                                        }
                                    });
                                }});
                            }));
                            flyout.Items().Append(uninstallItem);
                        }
                    }
                }
            }

        }));
        InitializeCopiedMenu(menu);
        button.ContextFlyout(menu);

        return AppCardUI{item.appIndex, item.title, item.openPath, button, item.canRunAsAdmin, item.isSetting, item.isFile, item.commandLine, item.commandRevision};
    };

    auto makeAppEmptyCard = [isLight]() -> wuxc::Border {
        wuxc::Border emptyCard;
        emptyCard.CornerRadius(wux::CornerRadius{8, 8, 8, 8});
        emptyCard.Background(isLight ? MakeBrush(0x08, 0x00, 0x00, 0x00) : MakeBrush(0x0A, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderBrush(isLight ? MakeBrush(0x15, 0x00, 0x00, 0x00) : MakeBrush(0x10, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        emptyCard.Padding(wux::ThicknessHelper::FromLengths(16, 22, 16, 22));
        emptyCard.Margin(wux::ThicknessHelper::FromLengths(6, 10, 6, 8));
        emptyCard.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

        wuxc::StackPanel emptyStack;
        emptyStack.HorizontalAlignment(wux::HorizontalAlignment::Center);

        auto emptyIcon = SearchFontIcon();
        emptyIcon.Glyph(L"\uE71D");
        ScaleFont(emptyIcon, 24);
        emptyIcon.Opacity(0.2);
        emptyIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 6));
        emptyStack.Children().Append(emptyIcon);

        auto emptyTitle = SearchTextBlock();
        emptyTitle.Text(L"No matching applications");
        ScaleFont(emptyTitle, 11.5);
        emptyTitle.FontWeight(wut::FontWeights::SemiBold());
        emptyTitle.Opacity(0.45);
        emptyTitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyStack.Children().Append(emptyTitle);

        auto emptySubtitle = SearchTextBlock();
        emptySubtitle.Text(L"Check files or refine your query");
        ScaleFont(emptySubtitle, 10.5);
        emptySubtitle.Opacity(0.3);
        emptySubtitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptySubtitle.Margin(wux::ThicknessHelper::FromLengths(0, 2, 0, 0));
        emptyStack.Children().Append(emptySubtitle);

        emptyCard.Child(emptyStack);
        return emptyCard;
    };

    auto isMatch = [](const AppCardUI& card, const Row& row) {
        if (!card.commandLine.empty() || !row.commandLine.empty()) {
            return card.title == row.title && card.commandLine == row.commandLine &&
                   card.commandRevision == row.commandRevision;
        }
        if (!card.openPath.empty() && !row.openPath.empty()) {
            return card.openPath == row.openPath && card.title == row.title;
        }
        return card.title == row.title;
    };

    if (appNames.empty()) {
        for (int i = static_cast<int>(g_activeAppsOpt->size()) - 1; i >= 0; --i) {
            g_appsList.Children().RemoveAt(i);
        }
        g_activeAppsOpt->clear();
        g_appButtonsOpt->clear();

        if (g_appsList.Children().Size() == 0) {
            g_appsList.Children().Append(makeAppEmptyCard());
        }
        SetAppSelection(-1);
    } else {
        if (g_activeAppsOpt->empty() && g_appsList.Children().Size() > 0) {
            g_appsList.Children().Clear();
        }

        // 1. Remove cards that are no longer in appNames
        for (int i = static_cast<int>(g_activeAppsOpt->size()) - 1; i >= 0; --i) {
            bool found = false;
            for (const auto& newApp : appNames) {
                if (isMatch((*g_activeAppsOpt)[i], newApp)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                g_appsList.Children().RemoveAt(i);
                g_activeAppsOpt->erase(g_activeAppsOpt->begin() + i);
            }
        }

        // 2. Insert new cards or reorder existing ones
        for (size_t targetIdx = 0; targetIdx < appNames.size(); ++targetIdx) {
            const Row& want = appNames[targetIdx];
            int existingIdx = -1;
            for (size_t i = targetIdx; i < g_activeAppsOpt->size(); ++i) {
                if (isMatch((*g_activeAppsOpt)[i], want)) {
                    existingIdx = static_cast<int>(i);
                    break;
                }
            }

            if (existingIdx >= 0) {
                (*g_activeAppsOpt)[existingIdx].appIndex = want.appIndex;
                (*g_activeAppsOpt)[existingIdx].canRunAsAdmin = want.canRunAsAdmin;
                (*g_activeAppsOpt)[existingIdx].isSetting = want.isSetting;
                (*g_activeAppsOpt)[existingIdx].isFile = want.isFile;

                if (static_cast<size_t>(existingIdx) != targetIdx) {
                    auto card = (*g_activeAppsOpt)[existingIdx];
                    g_activeAppsOpt->erase(g_activeAppsOpt->begin() + existingIdx);
                    g_activeAppsOpt->insert(g_activeAppsOpt->begin() + targetIdx, card);

                    g_appsList.Children().RemoveAt(existingIdx);
                    g_appsList.Children().InsertAt(static_cast<uint32_t>(targetIdx), card.button);
                }
            } else {
                AppCardUI newCard = makeAppCard(want);
                g_activeAppsOpt->insert(g_activeAppsOpt->begin() + targetIdx, newCard);
                g_appsList.Children().InsertAt(static_cast<uint32_t>(targetIdx), newCard.button);
            }
        }

        g_appButtonsOpt->clear();
        for (const auto& card : *g_activeAppsOpt) {
            g_appButtonsOpt->push_back(card.button);
        }

        SetAppSelection(0);
    }

    // Files renderer
    auto makeFileRow = [isLight](const Row& item) {
        wuxc::Grid layout;
        wuxc::ColumnDefinition iconCol, textCol;
        iconCol.Width(wux::GridLengthHelper::FromValueAndType(0, wux::GridUnitType::Auto));
        textCol.Width(wux::GridLengthHelper::FromValueAndType(1, wux::GridUnitType::Star));
        layout.ColumnDefinitions().Append(iconCol);
        layout.ColumnDefinitions().Append(textCol);

        bool hasBitmap = false;
        if (auto bmp = IconBitmap(item.icon)) {
            wuxc::Image image;
            image.Source(bmp);
            image.Width(kIconDisplay);
            image.Height(kIconDisplay);
            image.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            image.VerticalAlignment(wux::VerticalAlignment::Center);
            wuxc::Grid::SetColumn(image, 0);
            layout.Children().Append(image);
            hasBitmap = true;
        }

        if (!hasBitmap) {
            wuxc::Border iconBox;
            iconBox.Width(kIconDisplay);
            iconBox.Height(kIconDisplay);
            iconBox.Margin(wux::ThicknessHelper::FromLengths(0, 0, 10, 0));
            iconBox.VerticalAlignment(wux::VerticalAlignment::Center);

            auto fallbackIcon = SearchFontIcon();
            ScaleFont(fallbackIcon, 15);
            fallbackIcon.Opacity(0.5);
            fallbackIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
            fallbackIcon.VerticalAlignment(wux::VerticalAlignment::Center);

            if (item.isFolder) {
                fallbackIcon.Glyph(L"\uE8B7");
            } else {
                fallbackIcon.Glyph(L"\uE8A5");
            }
            iconBox.Child(fallbackIcon);
            wuxc::Grid::SetColumn(iconBox, 0);
            layout.Children().Append(iconBox);
        }

        wuxc::StackPanel text;
        wuxc::Grid::SetColumn(text, 1);
        text.VerticalAlignment(wux::VerticalAlignment::Center);

        auto name = SearchTextBlock();
        name.Text(winrt::hstring{item.title});
        ScaleFont(name, 12.5);
        name.FontWeight(wut::FontWeights::SemiBold());
        name.TextTrimming(wux::TextTrimming::CharacterEllipsis);
        name.TextWrapping(wux::TextWrapping::NoWrap);
        text.Children().Append(name);

        if (!item.subtitle.empty()) {
            auto sub = SearchTextBlock();
            sub.Text(winrt::hstring{item.subtitle});
            sub.Opacity(0.45);
            ScaleFont(sub, 10.5);
            sub.Margin(wux::ThicknessHelper::FromLengths(0, 1, 0, 0));
            sub.TextTrimming(wux::TextTrimming::CharacterEllipsis);
            sub.TextWrapping(wux::TextWrapping::NoWrap);
            text.Children().Append(sub);
        }
        layout.Children().Append(text);

        wuxc::Button button;
        button.Content(layout);
        button.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
        button.HorizontalContentAlignment(wux::HorizontalAlignment::Stretch);
        button.Background(MakeBrush(0, 0, 0, 0));
        button.BorderBrush(MakeBrush(0, 0, 0, 0));
        button.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        button.CornerRadius(wux::CornerRadius{6, 6, 6, 6});
        button.Padding(wux::ThicknessHelper::FromLengths(10, 6, 10, 6));
        button.Margin(wux::ThicknessHelper::FromLengths(2, 1, 2, 1));
        button.IsTabStop(false);
        button.AllowFocusOnInteraction(false);

        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPointerOver"),
            isLight ? MakeBrush(0x14, 0x00, 0x5F, 0xB8) : MakeBrush(0x28, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPointerOver"),
            isLight ? MakeBrush(0x80, 0x00, 0x5F, 0xB8) : MakeBrush(0x55, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundPressed"),
            isLight ? MakeBrush(0x24, 0x00, 0x5F, 0xB8) : MakeBrush(0x35, 0xFF, 0xFF, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushPressed"),
            isLight ? MakeBrush(0xA0, 0x00, 0x5F, 0xB8) : MakeBrush(0x70, 0x60, 0xCD, 0xFF));
        button.Resources().Insert(winrt::box_value(L"ButtonBackgroundFocused"),
            MakeBrush(0, 0, 0, 0));
        button.Resources().Insert(winrt::box_value(L"ButtonBorderBrushFocused"),
            MakeBrush(0, 0, 0, 0));

        // Hovering a row selects it, as it does an app card, so the pointer's
        // highlight and the keyboard's are the same one. Weak: the button
        // holds this handler.
        KeepHandler(button, button.PointerEntered(winrt::auto_revoke, [weak = winrt::make_weak(button)](wf::IInspectable const&,
                                                                wux::Input::PointerRoutedEventArgs const&) {
            auto btn = weak.get();
            if (!btn || !g_fileButtonsOpt || g_dragging) return;  // a drag passing over
            for (size_t i = 0; i < g_fileButtonsOpt->size(); ++i) {
                if ((*g_fileButtonsOpt)[i] == btn) {
                    SetFileSelection(static_cast<int>(i));
                    break;
                }
            }
        }));

        if (!item.openPath.empty()) {
            std::wstring target = item.openPath;
            KeepHandler(button, button.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                if (g_dragging) {
                    return;  // the release that ends a drag (StartFileDrag)
                }
                DismissSearchForLaunch([target] { OpenResult(target); });
            }));

            // Dragging the row drags the file (StartFileDrag). A Button takes the
            // mouse the moment it is pressed and handles the events, so the
            // press and the moves are watched here, handled or not, and the
            // drag starts once the pointer has moved a few pixels with the
            // button down. The Button lets the mouse go then, so no click
            // follows.
            auto pressedAt = std::make_shared<std::optional<wf::Point>>();
            const auto weakButton = winrt::make_weak(button);
            AddPointerHandler(button, wux::UIElement::PointerPressedEvent(),
                              [pressedAt, weakButton](wf::IInspectable const&, wux::Input::PointerRoutedEventArgs const& e) {
                                  auto btn = weakButton.get();
                                  pressedAt->reset();
                                  if (btn) {
                                      auto point = e.GetCurrentPoint(btn);
                                      if (point.Properties().IsLeftButtonPressed()) {
                                          *pressedAt = point.Position();
                                      }
                                  }
                              });
            AddPointerHandler(button, wux::UIElement::PointerMovedEvent(),
                              [pressedAt, weakButton, target](wf::IInspectable const&,
                                                              wux::Input::PointerRoutedEventArgs const& e) {
                                  auto btn = weakButton.get();
                                  if (!*pressedAt || !btn) {
                                      return;
                                  }
                                  auto point = e.GetCurrentPoint(btn);
                                  if (!point.Properties().IsLeftButtonPressed()) {
                                      pressedAt->reset();
                                      return;
                                  }
                                  const float dx = point.Position().X - (*pressedAt)->X;
                                  const float dy = point.Position().Y - (*pressedAt)->Y;
                                  if (dx * dx + dy * dy < 36) {
                                      return;
                                  }
                                  pressedAt->reset();
                                  btn.ReleasePointerCaptures();
                                  HidePreview();
                                  StartFileDrag(target);
                              });
            AddPointerHandler(button, wux::UIElement::PointerReleasedEvent(),
                              [pressedAt](wf::IInspectable const&, wux::Input::PointerRoutedEventArgs const&) {
                                  pressedAt->reset();
                              });

            wuxc::MenuFlyout menu;
            KeepHandler(menu, menu.Opening(winrt::auto_revoke, [](wf::IInspectable const& sender, wf::IInspectable const&) { NoteContextMenuOpening(sender); }));
            KeepHandler(menu, menu.Closed(winrt::auto_revoke, [](wf::IInspectable const& sender, wf::IInspectable const&) { NoteContextMenuClosed(sender); }));
            // Filled in when first opened, not with the row: every keystroke
            // builds every row, and hardly any of their menus are ever opened.
            KeepHandler(menu, menu.Opening(winrt::auto_revoke, [target, isFolder = item.isFolder, title = item.title](
                             wf::IInspectable const& sender, wf::IInspectable const&) {
                auto flyout = sender.as<wuxc::MenuFlyout>();
                if (flyout.Items().Size() > 0) return;
                wuxc::MenuFlyoutItem openItem;
                openItem.Text(L"Open");
                auto openIcon = SearchFontIcon();
                openIcon.Glyph(L"\uE8A7");
                openItem.Icon(openIcon);
                KeepHandler(openItem, openItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    DismissSearchForLaunch([target] { OpenResult(target); });
                }));
                flyout.Items().Append(openItem);

                if (CanElevatePath(target)) {
                    wuxc::MenuFlyoutItem adminItem;
                    adminItem.Text(L"Run as administrator");
                    auto adminIcon = SearchFontIcon();
                    adminIcon.Glyph(L"\uE7EF");
                    adminItem.Icon(adminIcon);
                    KeepHandler(adminItem, adminItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                        DismissSearchForLaunch([target] { OpenResult(target, true /* asAdmin */); });
                    }));
                    flyout.Items().Append(adminItem);
                }

                if (isFolder) {
                    wuxc::MenuFlyoutSubItem termSub;
                    termSub.Text(L"Open in terminal");
                    auto termIcon = SearchFontIcon();
                    termIcon.Glyph(L"\uE756");
                    termSub.Icon(termIcon);

                    auto addTermItem = [&](const wchar_t* label, bool isPowerShell, bool asAdmin) {
                        wuxc::MenuFlyoutItem termItem;
                        termItem.Text(winrt::hstring{label});
                        auto icon = SearchFontIcon();
                        icon.Glyph(asAdmin ? L"\uE7EF" : L"\uE756");
                        termItem.Icon(icon);
                        KeepHandler(termItem, termItem.Click(winrt::auto_revoke, [target, isPowerShell, asAdmin](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                            DismissSearchForLaunch(
                                [target, isPowerShell, asAdmin] { LaunchTerminal(target, isPowerShell, asAdmin); });
                        }));
                        termSub.Items().Append(termItem);
                    };

                    addTermItem(L"Command Prompt", false, false);
                    addTermItem(L"Command Prompt (Administrator)", false, true);
                    addTermItem(L"PowerShell", true, false);
                    addTermItem(L"PowerShell (Administrator)", true, true);

                    flyout.Items().Append(termSub);
                }

                wuxc::MenuFlyoutSeparator sep1;
                flyout.Items().Append(sep1);

                wuxc::MenuFlyoutItem cutItem;
                cutItem.Text(L"Cut");
                auto cutIcon = SearchFontIcon();
                cutIcon.Glyph(L"\uE8C6");
                cutItem.Icon(cutIcon);
                KeepHandler(cutItem, cutItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyOrCutFileToClipboard(target, true /* isCut */);
                }));
                flyout.Items().Append(cutItem);

                wuxc::MenuFlyoutItem copyItem;
                copyItem.Text(L"Copy");
                auto copyIcon = SearchFontIcon();
                copyIcon.Glyph(L"\uE8C8");
                copyItem.Icon(copyIcon);
                KeepHandler(copyItem, copyItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyOrCutFileToClipboard(target, false /* isCut */);
                }));
                flyout.Items().Append(copyItem);

                wuxc::MenuFlyoutItem copyNameItem;
                copyNameItem.Text(L"Copy name");
                auto copyNameIcon = SearchFontIcon(); copyNameIcon.Glyph(L"\uE8D2");
                copyNameItem.Icon(copyNameIcon);
                KeepHandler(copyNameItem,copyNameItem.Click(winrt::auto_revoke,[title](wf::IInspectable const&,wux::RoutedEventArgs const&) {
                    tools::CopyTextToClipboard(title);
                }));
                flyout.Items().Append(copyNameItem);

                wuxc::MenuFlyoutItem copyPathItem;
                copyPathItem.Text(L"Copy path");
                auto copyPathIcon = SearchFontIcon();
                copyPathIcon.Glyph(L"\uE71B");
                copyPathItem.Icon(copyPathIcon);
                KeepHandler(copyPathItem, copyPathItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CopyTextToClipboard(target);
                }));
                flyout.Items().Append(copyPathItem);

                wuxc::MenuFlyoutSeparator sep2;
                flyout.Items().Append(sep2);

                wuxc::MenuFlyoutItem locItem;
                locItem.Text(L"Open file location");
                auto locIcon = SearchFontIcon();
                locIcon.Glyph(L"\uE838");
                locItem.Icon(locIcon);
                KeepHandler(locItem, locItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    OpenFileLocationThenDismiss(target);
                }));
                flyout.Items().Append(locItem);

                wuxc::MenuFlyoutItem shortcutItem;
                shortcutItem.Text(L"Create desktop shortcut");
                auto shortcutIcon = SearchFontIcon();
                shortcutIcon.Glyph(L"\uE7C5");
                shortcutItem.Icon(shortcutIcon);
                KeepHandler(shortcutItem, shortcutItem.Click(winrt::auto_revoke, [target, title](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    tools::CreateDesktopShortcut(target, title);
                }));
                flyout.Items().Append(shortcutItem);

                wuxc::MenuFlyoutSeparator sep3;
                flyout.Items().Append(sep3);

                wuxc::MenuFlyoutItem propItem;
                propItem.Text(L"Properties");
                auto propIcon = SearchFontIcon();
                propIcon.Glyph(L"\uE946");
                propItem.Icon(propIcon);
                KeepHandler(propItem, propItem.Click(winrt::auto_revoke, [target](wf::IInspectable const&, wux::RoutedEventArgs const&) {
                    AllowExplorerForeground();
                    DismissSearch();
                    ShowPropertiesDialog(target);
                }));
                flyout.Items().Append(propItem);

            }));
            InitializeCopiedMenu(menu);
        button.ContextFlyout(menu);
        }
        return button;
    };

    // Files on the right.
    g_currentFileRows = files;
    g_selectedFile = -1;
    if (!g_fileButtonsOpt) g_fileButtonsOpt.emplace();
    g_fileButtonsOpt->clear();
    if (files.empty()) {
        wuxc::Border emptyCard;
        emptyCard.CornerRadius(wux::CornerRadius{8, 8, 8, 8});
        emptyCard.Background(isLight ? MakeBrush(0x08, 0x00, 0x00, 0x00) : MakeBrush(0x0A, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderBrush(isLight ? MakeBrush(0x15, 0x00, 0x00, 0x00) : MakeBrush(0x10, 0xFF, 0xFF, 0xFF));
        emptyCard.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
        emptyCard.Padding(wux::ThicknessHelper::FromLengths(16, 22, 16, 22));
        emptyCard.Margin(wux::ThicknessHelper::FromLengths(6, 10, 6, 8));
        emptyCard.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

        wuxc::StackPanel emptyStack;
        emptyStack.HorizontalAlignment(wux::HorizontalAlignment::Center);

        auto emptyIcon = SearchFontIcon();
        emptyIcon.Glyph(L"\uE8B7");
        ScaleFont(emptyIcon, 24);
        emptyIcon.Opacity(0.2);
        emptyIcon.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyIcon.Margin(wux::ThicknessHelper::FromLengths(0, 0, 0, 6));
        emptyStack.Children().Append(emptyIcon);

        auto emptyTitle = SearchTextBlock();
        emptyTitle.Text(suggestions ? L"Type to search files" : L"No matching files");
        ScaleFont(emptyTitle, 11.5);
        emptyTitle.FontWeight(wut::FontWeights::SemiBold());
        emptyTitle.Opacity(0.45);
        emptyTitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptyStack.Children().Append(emptyTitle);

        auto emptySubtitle = SearchTextBlock();
        emptySubtitle.Text(suggestions ? L"Files and folders will appear here" : L"Everything index returned 0 items");
        ScaleFont(emptySubtitle, 10.5);
        emptySubtitle.Opacity(0.3);
        emptySubtitle.HorizontalAlignment(wux::HorizontalAlignment::Center);
        emptySubtitle.Margin(wux::ThicknessHelper::FromLengths(0, 2, 0, 0));
        emptyStack.Children().Append(emptySubtitle);

        emptyCard.Child(emptyStack);
        g_resultsList.Children().Append(emptyCard);
    } else {
        for (const Row& file : files) {
            wuxc::Button row = makeFileRow(file);
            g_resultsList.Children().Append(row);
            g_fileButtonsOpt->push_back(row);
        }
    }

    InvalidateSearchContrastContainer(g_appsHeaderHolder);
    InvalidateSearchContrastContainer(g_filesHeaderHolder);
    PrepareSearchResultContrastColors();

    // New results start the keyboard selection over: on the first app, or on
    // the first file when there are no apps.
    if (g_appButtonsOpt->empty() && !g_fileButtonsOpt->empty()) {
        SetFileSelection(0);
    } else {
        g_filesColumnActive = false;
    }

    if (g_ourBox && g_ourBox.Text().empty()) {
        UpdateSearchResultsLayout(true);
    } else {
        UpdateSearchResultsLayout();
    }
    Wh_Log(L"render: %zu apps, %zu files, host %.0fx%.0f", appNames.size(),
        files.size(), g_resultsHost.ActualWidth(),
        g_resultsHost.ActualHeight());

    if (g_enterWaiting && !ResultsPending()) {
        g_enterWaiting = false;
        if (GetTickCount64() - g_enterWaitingTick < kEnterWaitMs) {
            Wh_Log(L"render: running the Enter that waited for these results");
            ActivateSelection(g_enterWaitingCtrl);
        }
    }
} catch (...) {
    Wh_Log(L"render failed %08X", static_cast<unsigned>(winrt::to_hresult()));
}

// File icons fetched after their rows were drawn (see FileIconCache), under
// g_resultsMutex. They are swapped into the rows in place: drawing the rows
// again would start the keyboard selection over.
std::vector<std::pair<std::wstring, std::vector<BYTE>>> g_lateFileIcons;

void ApplyLateFileIcons() try {
    std::vector<std::pair<std::wstring, std::vector<BYTE>>> late;
    {
        std::lock_guard<std::mutex> lock(g_resultsMutex);
        late.swap(g_lateFileIcons);
    }
    if (!g_fileButtonsOpt) {
        return;
    }
    const size_t rows = std::min(g_currentFileRows.size(), g_fileButtonsOpt->size());
    for (const auto& [path, pixels] : late) {
        auto bmp = IconBitmap(pixels);
        if (!bmp) {
            continue;
        }
        for (size_t i = 0; i < rows; ++i) {
            if (g_currentFileRows[i].openPath != path) {
                continue;
            }
            auto layout = (*g_fileButtonsOpt)[i].Content().try_as<wuxc::Grid>();
            auto image = layout && layout.Children().Size() > 0
                             ? layout.Children().GetAt(0).try_as<wuxc::Image>()
                             : nullptr;
            if (!image) {
                continue;
            }
            image.Source(bmp);
        }
    }
} catch (...) {
}

// Called from the search thread.
void PublishLateFileIcons(std::vector<std::pair<std::wstring, std::vector<BYTE>>> late) {
    if (late.empty()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_resultsMutex);
        for (auto& icon : late) {
            g_lateFileIcons.push_back(std::move(icon));
        }
    }
    try {
        if (g_ourBox) {
            g_ourBox.Dispatcher().RunAsync(wuc::CoreDispatcherPriority::Normal,
                                           wuc::DispatchedHandler{[] { ApplyLateFileIcons(); }});
        }
    } catch (...) {
    }
}

std::atomic<bool> g_renderPending{false};

// Called from the search thread once results are in. Each dispatch reads the
// latest result snapshot, so multiple queued requests would redraw it twice.
void RequestRender() {
    bool scheduled = false;
    try {
        if (!g_ourBox) {
            return;
        }
        auto dispatcher = g_ourBox.Dispatcher();
        if (!dispatcher) {
            return;
        }
        if (g_renderPending.exchange(true)) return;
        scheduled = true;
        dispatcher.RunAsync(wuc::CoreDispatcherPriority::High,
                            wuc::DispatchedHandler{[] {
                                g_renderPending.store(false);
                                if (!g_quit.load()) RenderResults();
                            }});
    } catch (...) {
        if (scheduled) g_renderPending.store(false);
    }
}

// ---------------------------------------------------------------------------
// Asking Everything, from inside the Start menu
//
// On its own thread, with its own message pump: the IPC is a WM_COPYDATA
// round trip and the reply lands on a window, so it cannot run on the XAML
// thread without blocking the menu while the user types.
//
// Debounced, because a keystroke every ~60ms would otherwise be a query every
// ~60ms. 120ms was measured as comfortable in the broker.
// ---------------------------------------------------------------------------


void QueueQuery(std::wstring text) {
    {
        std::lock_guard<std::mutex> lock(g_queryMutex);
        g_pendingQuery = std::move(text);
        g_queryDirty.store(true);
    }
    g_queryWake.notify_all();
}

// An app's icon, from its shell identity.
//
// Not the extension cache: that answers from a file type, and an app is not a
// file type -- every app has its own icon, so there is nothing to share. It
// goes through the item itself for the same reason the index stores PIDLs
// rather than paths: AUMIDs and known-folder GUIDs cannot be re-parsed back
// into something SHGetFileInfo understands.
//
// Measured at roughly 9.7ms per app in the broker, so this runs on the search
// thread and is cached by name. Six visible rows make it about 60ms once, and
// nothing after that.
bool FetchAppIcon(const apps::App* app, int size, std::vector<BYTE>* out) {
    if (!app || !app->pidl || !out) {
        return false;
    }
    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromIDList(app->pidl.get(), IID_PPV_ARGS(&item))) ||
        !item) {
        return false;
    }
    bool ok = false;
    IShellItemImageFactory* factory = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
        SIZE want{size, size};
        HBITMAP bitmap = nullptr;
        if (SUCCEEDED(factory->GetImage(
                want, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &bitmap)) &&
            bitmap) {
            ok = icons::BitmapToBgra(bitmap, size, out);
            DeleteObject(bitmap);
        }
        factory->Release();
    }
    item->Release();
    return ok;
}

// Opens an app.
//
// One with no file of its own goes by its shell identity: the parsing names
// come in several shapes, including AUMIDs and known-folder GUIDs, and
// rebuilding a path from them fails outright for some, while the PIDL, or the
// shell:AppsFolder name made from it (AppsFolderPath), works for all of them.
//
// Runs on a launch thread. The index stays with the search thread, so this
// gets copies of what it needs, the PIDL included.
// Accept a pasted Windows icon character or its commonly copied Unicode value.
// Emoji stay on the color-font path; filenames must not be parsed as glyphs.
std::wstring CustomCommandWindowsGlyph(std::wstring_view icon) {
    const auto isIcon = [](wchar_t c) { return c >= 0xE000 && c <= 0xF8FF; };
    if ((icon.size() == 1 || (icon.size() == 2 && (icon[1] == 0xFE0E || icon[1] == 0xFE0F))) &&
        isIcon(icon[0])) return std::wstring(1, icon[0]);
    if (icon.starts_with(L"U+") || icon.starts_with(L"u+") ||
        icon.starts_with(L"0x") || icon.starts_with(L"0X") ||
        icon.starts_with(L"\\u") || icon.starts_with(L"\\U")) {
        icon.remove_prefix(2);
    } else if ((icon.starts_with(L"&#x") || icon.starts_with(L"&#X")) && icon.ends_with(L";")) {
        icon.remove_prefix(3);
        icon.remove_suffix(1);
    }
    if (icon.size() != 4) return {};
    unsigned value = 0;
    for (wchar_t c : icon) {
        unsigned digit;
        if (c >= L'0' && c <= L'9') digit = c - L'0';
        else if (c >= L'a' && c <= L'f') digit = c - L'a' + 10;
        else if (c >= L'A' && c <= L'F') digit = c - L'A' + 10;
        else return {};
        value = (value << 4) | digit;
    }
    return isIcon(static_cast<wchar_t>(value)) ? std::wstring(1, static_cast<wchar_t>(value)) : std::wstring();
}

std::wstring CustomCommandIconPath(std::wstring icon) {
    icon=tools::Trim(icon);
    if(icon.size()>1 && icon.front()==L'"' && icon.back()==L'"') icon=icon.substr(1,icon.size()-2);
    const DWORD needed=ExpandEnvironmentStringsW(icon.c_str(),nullptr,0);
    if(needed>0 && needed<=32768) {
        std::wstring expanded(needed,L'\0');
        if(ExpandEnvironmentStringsW(icon.c_str(),expanded.data(),needed)) {
            expanded.resize(needed-1); icon=std::move(expanded);
        }
    }
    return icon;
}

std::vector<BYTE> FetchCustomCommandIcon(std::wstring const& path,int size) {
    std::vector<BYTE> result;
    winrt::com_ptr<IShellItemImageFactory> image;
    if(FAILED(SHCreateItemFromParsingName(path.c_str(),nullptr,IID_PPV_ARGS(image.put())))) return result;
    HBITMAP bitmap=nullptr;
    if(FAILED(image->GetImage(SIZE{size,size},SIIGBF_THUMBNAILONLY | SIIGBF_BIGGERSIZEOK,&bitmap)) &&
       FAILED(image->GetImage(SIZE{size,size},SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,&bitmap))) return result;
    std::vector<BYTE> pixels; int width=0,height=0;
    const bool copied=icons::BitmapToPixels(bitmap,&pixels,&width,&height);
    DeleteObject(bitmap);
    if(!copied) return result;
    const double scale=std::min(double(size)/width,double(size)/height);
    const int w=std::clamp(static_cast<int>(std::lround(width*scale)),1,size);
    const int h=std::clamp(static_cast<int>(std::lround(height*scale)),1,size);
    auto fitted=icons::Resample(pixels,width,height,w,h);
    result.resize(static_cast<size_t>(size)*size*4);
    const int x=(size-w)/2,y=(size-h)/2;
    for(int row=0;row<h;++row)
        memcpy(result.data()+(static_cast<size_t>(y+row)*size+x)*4,fitted.data()+static_cast<size_t>(row)*w*4,w*4);
    return result;
}

void AddCustomCommandRows(std::vector<Row>* rows,std::vector<CustomCommand> const& commands,
                          std::wstring const& query,size_t limit,uint64_t revision,int iconPixels,
                          std::map<std::wstring,std::vector<BYTE>>* iconCache,
                          const apps::UsageCounts* usage) {
    const auto lower=apps::ToLower(query);
    for(auto const& command:commands) {
        apps::App match;
        match.name=command.name; match.nameLower=apps::ToLower(command.name);
        std::wstring word;
        for(wchar_t c:match.nameLower) {
            if(iswalnum(c)) word+=c;
            else if(!word.empty()) { match.acronym+=word.front(); match.words.push_back(std::move(word)); word.clear(); }
        }
        if(!word.empty()) { match.acronym+=word.front(); match.words.push_back(std::move(word)); }
        const int score=query.empty() ? 0 : apps::ScoreApp(match,lower);
        if(score<0) continue;
        Row row;
        row.title=command.name; row.subtitle=L"Custom command";
        row.commandLine=command.command; row.commandIcon=command.icon; row.commandRevision=revision;
        row.commandShell = command.shell; row.commandHideTerminal = command.hideTerminal;
        row.commandShellExecutable=command.shellExecutable;
        row.commandShellArguments=command.shellArguments;
        const auto usageKey=apps::CommandUsageKey(command.name);
        row.matchScore=score;
        row.matchRank=query.empty() ? -apps::UsageCount(usage,usageKey) : score-apps::UsageBonus(usage,usageKey);
        row.customGlyph=L"\uE756";
        rows->push_back(std::move(row));
    }
    int best=std::numeric_limits<int>::max();
    for(auto const& row:*rows) if(row.matchScore>=0) best=std::min(best,row.matchScore);
    if(best!=std::numeric_limits<int>::max()) {
        const int cutoff=best<80 ? 80 : best+12;
        std::erase_if(*rows,[&](Row const& row) { return row.matchScore>=cutoff; });
    }
    std::stable_sort(rows->begin(),rows->end(),[&](Row const& a,Row const& b) {
        if(a.matchScore<0 || b.matchScore<0) return a.matchScore<b.matchScore;
        if(apps::MatchTier(a.matchScore)!=apps::MatchTier(b.matchScore)) return a.matchScore<b.matchScore;
        if(a.matchRank!=b.matchRank) return a.matchRank<b.matchRank;
        if(query.empty()) return apps::ToLower(a.title)<apps::ToLower(b.title);
        if(a.matchScore!=b.matchScore) return a.matchScore<b.matchScore;
        return a.title.size()<b.title.size();
    });
    const size_t tools=std::count_if(rows->begin(),rows->end(),[](Row const& row) { return row.matchScore<0; });
    if(rows->size()>tools+limit) rows->resize(tools+limit);
    // Load only visible icons and keep them between searches.
    for(auto& row:*rows) {
        if(row.commandLine.empty() || row.commandIcon.empty()) continue;
        auto icon=CustomCommandIconPath(row.commandIcon);
        if (auto glyph = CustomCommandWindowsGlyph(icon); !glyph.empty()) {
            row.customIconText = std::move(glyph);
            row.customIconIsGlyph = true;
        } else if(icon.find_first_of(L"\\/")!=std::wstring::npos || icon.find(L':')!=std::wstring::npos) {
            auto found=iconCache->find(icon);
            if(found==iconCache->end()) found=iconCache->emplace(icon,FetchCustomCommandIcon(icon,iconPixels)).first;
            row.icon=found->second;
        } else row.customIconText=icon;
    }
}


void LaunchAppAsync(std::wstring name, std::wstring path, ITEMIDLIST* pidl, bool asAdmin = false) {
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    bool launched = false;

    wchar_t userProfile[MAX_PATH] = {};
    if (!GetEnvironmentVariableW(L"USERPROFILE", userProfile, MAX_PATH) || !userProfile[0]) {
        PWSTR kf = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &kf)) && kf) {
            wcsncpy_s(userProfile, kf, MAX_PATH - 1);
            CoTaskMemFree(kf);
        }
    }

    std::wstring lowerPath = path;
    for (auto& c : lowerPath) c = static_cast<wchar_t>(towlower(c));
    std::wstring lowerName = name;
    for (auto& c : lowerName) c = static_cast<wchar_t>(towlower(c));

    bool isTerminalOrShell = (lowerPath.find(L"cmd.exe") != std::wstring::npos ||
                              lowerPath.find(L"powershell") != std::wstring::npos ||
                              lowerPath.find(L"pwsh") != std::wstring::npos ||
                              lowerPath.find(L"windowsterminal") != std::wstring::npos ||
                              lowerPath.ends_with(L"wt.exe") ||
                              lowerName == L"command prompt" ||
                              lowerName.find(L"powershell") != std::wstring::npos ||
                              lowerName == L"terminal" ||
                              lowerName == L"windows terminal");

    size_t lastSlash = path.find_last_of(L"\\/");
    std::wstring parentDir = (lastSlash != std::wstring::npos) ? path.substr(0, lastSlash) : L"";
    LPCWSTR workDir = isTerminalOrShell ? userProfile : (!parentDir.empty() ? parentDir.c_str() : userProfile);

    // Expand environment strings if any (e.g. %windir%\system32\...)
    if (path.find(L'%') != std::wstring::npos) {
        wchar_t expanded[MAX_PATH] = {};
        if (ExpandEnvironmentStringsW(path.c_str(), expanded, MAX_PATH) > 0) {
            path = expanded;
        }
    }

    // 0. If path is a URI (e.g. ms-settings:, http:, https:) or command:
    if (!path.empty()) {
        if (path.starts_with(L"ms-settings:") || path.starts_with(L"http:") || path.starts_with(L"https:")) {
            if (ShellExecuteInExplorer(path, L"", L"")) {
                launched = true;
            } else if (HINSTANCE hInst = ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
                       reinterpret_cast<INT_PTR>(hInst) > 32) {
                Wh_Log(L"launched URI setting: %ls", path.c_str());
                launched = true;
            } else {
                Wh_Log(L"URI launch failed (%ld): %ls", reinterpret_cast<INT_PTR>(hInst), path.c_str());
            }
        } else if (path.starts_with(L"control ")) {
            std::wstring params = path.substr(8);
            if (!asAdmin && ShellExecuteInExplorer(L"control.exe", params, L"")) {
                launched = true;
            } else if (HINSTANCE hInst = ShellExecuteW(nullptr, asAdmin ? L"runas" : L"open", L"control.exe", params.c_str(), nullptr, SW_SHOWNORMAL);
                       reinterpret_cast<INT_PTR>(hInst) > 32) {
                Wh_Log(L"launched control applet: %ls", path.c_str());
                launched = true;
            }
        }
    }

    // 1. If path is a real file on disk or executable in PATH (e.g. C:\Windows\System32\cmd.exe, ncpa.cpl, services.msc, cleanmgr.exe):
    bool isFileOnDisk = false;
    std::wstring extraParams;
    if (!launched && !path.empty() && path.find(L"http") != 0 && !path.starts_with(L"ms-settings:")) {
        DWORD attr = GetFileAttributesW(path.c_str());
        if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
            isFileOnDisk = true;
        } else {
            // Try resolving via PATH / System32
            wchar_t resolved[MAX_PATH] = {};
            if (SearchPathW(nullptr, path.c_str(), nullptr, MAX_PATH, resolved, nullptr) > 0) {
                path = resolved;
                isFileOnDisk = true;
            } else {
                // Check if path has arguments (e.g. "rundll32.exe sysdm.cpl,EditEnvironmentVariables" or "explorer.exe shell:::{...}")
                size_t spacePos = path.find(L' ');
                if (spacePos != std::wstring::npos) {
                    std::wstring exePart = path.substr(0, spacePos);
                    std::wstring paramPart = path.substr(spacePos + 1);
                    if (SearchPathW(nullptr, exePart.c_str(), nullptr, MAX_PATH, resolved, nullptr) > 0) {
                        path = resolved;
                        extraParams = paramPart;
                        isFileOnDisk = true;
                    }
                }
            }
        }
    }

    if (isFileOnDisk) {
        std::wstring params;
        if (lowerPath.ends_with(L"cmd.exe")) {
            params = L"/k cd /d \"" + std::wstring(userProfile) + L"\"";
        } else if (lowerPath.find(L"powershell.exe") != std::wstring::npos || lowerPath.ends_with(L"pwsh.exe")) {
            params = L"-NoExit -Command \"Set-Location '" + std::wstring(userProfile) + L"'\"";
        }
        if (!extraParams.empty()) {
            if (!params.empty()) params += L" ";
            params += extraParams;
        }

        SHELLEXECUTEINFOW info{};
        info.cbSize = sizeof(info);
        // If asAdmin, do NOT suppress UI (UAC elevation prompt must show).
        info.fMask = SEE_MASK_NOASYNC | (asAdmin ? 0 : SEE_MASK_FLAG_NO_UI);
        info.lpVerb = asAdmin ? L"runas" : L"open";
        info.lpFile = path.c_str();
        if (!params.empty()) {
            info.lpParameters = params.c_str();
        }
        info.lpDirectory = workDir;
        info.nShow = SW_SHOWNORMAL;
        if ((!asAdmin && ShellExecuteInExplorer(path, params, workDir ? workDir : L"")) || ShellExecuteExW(&info)) {
            Wh_Log(L"launched app (by file path): %ls (admin=%d, dir=%ls)", name.c_str(), asAdmin ? 1 : 0, workDir ? workDir : L"(null)");
            launched = true;
        } else {
            Wh_Log(L"app file launch failed (%lu): %ls, trying fallback", GetLastError(), path.c_str());
        }
    }

    // 2. If not launched yet and we have a PIDL:
    if (!launched && pidl) {
        if (asAdmin) {
            // Try IContextMenu verb "runas"
            IShellItem* item = nullptr;
            if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item))) && item) {
                IContextMenu* menu = nullptr;
                if (SUCCEEDED(item->BindToHandler(nullptr, BHID_SFUIObject, IID_PPV_ARGS(&menu))) && menu) {
                    CMINVOKECOMMANDINFOEX ici{};
                    ici.cbSize = sizeof(ici);
                    ici.fMask = CMIC_MASK_UNICODE;
                    ici.lpVerb = "runas";
                    ici.lpVerbW = L"runas";
                    ici.lpDirectoryW = workDir;
                    ici.nShow = SW_SHOWNORMAL;
                    HRESULT hr = menu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&ici));
                    menu->Release();
                    item->Release();
                    if (SUCCEEDED(hr)) {
                        Wh_Log(L"launched app (by IContextMenu runas): %ls", name.c_str());
                        launched = true;
                    } else {
                        Wh_Log(L"IContextMenu runas failed (%08X) for %ls", static_cast<unsigned>(hr), name.c_str());
                    }
                } else if (item) {
                    item->Release();
                }
            }

            if (!launched) {
                // Fallback: ShellExecuteExW with SEE_MASK_IDLIST and "runas"
                SHELLEXECUTEINFOW info{};
                info.cbSize = sizeof(info);
                info.fMask = SEE_MASK_IDLIST | SEE_MASK_NOASYNC;
                info.lpIDList = pidl;
                info.lpVerb = L"runas";
                info.lpDirectory = workDir;
                info.nShow = SW_SHOWNORMAL;
                if (ShellExecuteExW(&info)) {
                    Wh_Log(L"launched app (by PIDL runas): %ls", name.c_str());
                    launched = true;
                } else {
                    Wh_Log(L"app PIDL runas failed (%lu): %ls", GetLastError(), name.c_str());
                }
            }
        }
        
        // Fallback to normal launch if runas failed or was not requested:
        if (!launched && !asAdmin) {
            std::wstring appPath = AppsFolderPath(pidl);
            if (!appPath.empty() && ShellExecuteInExplorer(appPath, L"", L"")) {
                Wh_Log(L"launched app (by app entry): %ls", name.c_str());
                launched = true;
            }
        }
        if (!launched) {
            SHELLEXECUTEINFOW info{};
            info.cbSize = sizeof(info);
            info.fMask = SEE_MASK_IDLIST | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
            info.lpIDList = pidl;
            info.lpVerb = L"open";
            info.lpDirectory = workDir;
            info.nShow = SW_SHOWNORMAL;
            if (ShellExecuteExW(&info)) {
                Wh_Log(L"launched app (by PIDL open): %ls", name.c_str());
                launched = true;
            } else {
                Wh_Log(L"app PIDL open failed (%lu): %ls", GetLastError(), name.c_str());
            }
        }
    }

    // 3. Fallback direct ShellExecuteExW for any remaining commands without PIDL:
    if (!launched && !path.empty()) {
        SHELLEXECUTEINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOASYNC | (asAdmin ? 0 : SEE_MASK_FLAG_NO_UI);
        info.lpVerb = asAdmin ? L"runas" : L"open";
        info.lpFile = path.c_str();
        info.lpDirectory = workDir;
        info.nShow = SW_SHOWNORMAL;
        if (ShellExecuteExW(&info)) {
            Wh_Log(L"launched app (by direct fallback): %ls (admin=%d)", path.c_str(), asAdmin ? 1 : 0);
            launched = true;
        } else {
            Wh_Log(L"app direct fallback launch failed (%lu): %ls", GetLastError(), path.c_str());
        }
    }

    if (SUCCEEDED(comHr)) {
        CoUninitialize();
    }
}

// ---------------------------------------------------------------------------
// Calculator and unit converter, with no command: whatever is typed that is a
// sum (2^10), a number with a unit (100 km), a conversion (100 km to mi) or a
// number in another base (0xFF, 255 hex) is worked out above the apps -- and
// only that: a number alone, or one in a file name ("2024 report", "3d"), is
// left to the search.
//
// It knows units itself (kUnits), so nothing has to be set up: any unit
// converts to any other of its kind, through the kind's base unit (base =
// value * factor + offset), both ways. "100 km" shows the kind's usual units,
// and first all of them on one line; "100 km to mi" (in, ->, =) just that
// one. The settings add units it does not know (unitConversions): one formula
// each, and back again when asked (reciprocal) and the formula is linear.
// Numbers are read with "." or "," as the decimal point and shown with the
// user's.
// ---------------------------------------------------------------------------
namespace calc {

struct Unit {
    const wchar_t* symbol;  // as shown
    const wchar_t* kind;
    double factor;  // to the kind's base unit
    double offset;
    bool usual;  // among those shown when no target is asked for
    const wchar_t* names;  // lowercase, '|'-separated; the symbol is one too
};

// KB, MB, GB, TB count in 1024s, as File Explorer does.
inline constexpr Unit kUnits[] = {
    {L"mm", L"Length", 0.001, 0, true, L"millimeter|millimeters|millimetre|millimetres"},
    {L"cm", L"Length", 0.01, 0, true, L"centimeter|centimeters|centimetre|centimetres"},
    {L"m", L"Length", 1, 0, true, L"meter|meters|metre|metres"},
    {L"km", L"Length", 1000, 0, true, L"kilometer|kilometers|kilometre|kilometres"},
    {L"in", L"Length", 0.0254, 0, true, L"inch|inches|\""},
    {L"ft", L"Length", 0.3048, 0, true, L"foot|feet|'"},
    {L"yd", L"Length", 0.9144, 0, false, L"yard|yards"},
    {L"mi", L"Length", 1609.344, 0, true, L"mile|miles"},
    {L"nmi", L"Length", 1852, 0, false, L"nautical mile|nautical miles"},

    {L"mg", L"Mass", 1e-6, 0, false, L"milligram|milligrams"},
    {L"g", L"Mass", 0.001, 0, true, L"gram|grams|gramme|grammes"},
    {L"kg", L"Mass", 1, 0, true, L"kilo|kilos|kilogram|kilograms"},
    {L"t", L"Mass", 1000, 0, false, L"tonne|tonnes|ton|tons"},
    {L"oz", L"Mass", 0.028349523125, 0, true, L"ounce|ounces"},
    {L"lb", L"Mass", 0.45359237, 0, true, L"lbs|pound|pounds"},
    {L"st", L"Mass", 6.35029318, 0, false, L"stone|stones"},

    {L"\u00B0C", L"Temperature", 1, 273.15, true, L"c|\u2103|celsius"},
    {L"\u00B0F", L"Temperature", 5.0 / 9, 459.67 * 5.0 / 9, true, L"f|\u2109|fahrenheit"},
    {L"K", L"Temperature", 1, 0, true, L"kelvin|kelvins"},

    {L"m/s", L"Speed", 1, 0, true, L"mps"},
    {L"km/h", L"Speed", 1 / 3.6, 0, true, L"kmh|kph|kmph"},
    {L"mph", L"Speed", 0.44704, 0, true, L"mi/h"},
    {L"kn", L"Speed", 1852.0 / 3600, 0, true, L"kt|knot|knots"},
    {L"ft/s", L"Speed", 0.3048, 0, false, L""},

    {L"mm\u00B2", L"Area", 1e-6, 0, false, L"mm2|sq mm"},
    {L"cm\u00B2", L"Area", 1e-4, 0, true, L"cm2|sq cm"},
    {L"m\u00B2", L"Area", 1, 0, true, L"m2|sq m|sqm"},
    {L"km\u00B2", L"Area", 1e6, 0, true, L"km2|sq km"},
    {L"a", L"Area", 100, 0, false, L"are|ares"},
    {L"ha", L"Area", 1e4, 0, true, L"hectare|hectares"},
    {L"ac", L"Area", 4046.8564224, 0, true, L"acre|acres"},
    {L"in\u00B2", L"Area", 0.00064516, 0, false, L"in2|sq in"},
    {L"ft\u00B2", L"Area", 0.09290304, 0, true, L"ft2|sq ft|sqft"},
    {L"yd\u00B2", L"Area", 0.83612736, 0, false, L"yd2|sq yd"},
    {L"mi\u00B2", L"Area", 2589988.110336, 0, false, L"mi2|sq mi"},

    {L"ml", L"Volume", 0.001, 0, true, L"milliliter|milliliters|millilitre|millilitres|cc|cm\u00B3|cm3"},
    {L"l", L"Volume", 1, 0, true, L"liter|liters|litre|litres"},
    {L"m\u00B3", L"Volume", 1000, 0, true, L"m3|cubic meter|cubic meters|cubic metre|cubic metres"},
    {L"gal", L"Volume", 3.785411784, 0, true, L"gallon|gallons"},
    {L"qt", L"Volume", 0.946352946, 0, false, L"quart|quarts"},
    {L"pt", L"Volume", 0.473176473, 0, false, L"pint|pints"},
    {L"cup", L"Volume", 0.2365882365, 0, true, L"cups"},
    {L"fl oz", L"Volume", 0.0295735295625, 0, true, L"floz|fluid ounce|fluid ounces"},
    {L"tbsp", L"Volume", 0.01478676478125, 0, false, L"tablespoon|tablespoons"},
    {L"tsp", L"Volume", 0.00492892159375, 0, false, L"teaspoon|teaspoons"},

    {L"bit", L"Data", 0.125, 0, false, L"bits"},
    {L"B", L"Data", 1, 0, true, L"byte|bytes"},
    {L"KB", L"Data", 1024, 0, true, L"kib|kilobyte|kilobytes"},
    {L"MB", L"Data", 1048576, 0, true, L"mib|megabyte|megabytes"},
    {L"GB", L"Data", 1073741824, 0, true, L"gib|gigabyte|gigabytes"},
    {L"TB", L"Data", 1099511627776, 0, true, L"tib|terabyte|terabytes"},
    {L"PB", L"Data", 1125899906842624, 0, false, L"pib|petabyte|petabytes"},
    {L"Kbit", L"Data", 125, 0, false, L"kbits|kilobit|kilobits"},
    {L"Mbit", L"Data", 125000, 0, false, L"mbits|megabit|megabits"},
    {L"Gbit", L"Data", 125000000, 0, false, L"gbits|gigabit|gigabits"},

    {L"KB/s", L"Data rate", 1024, 0, false, L""},
    {L"MB/s", L"Data rate", 1048576, 0, true, L""},
    {L"Mbps", L"Data rate", 125000, 0, true, L"mbit/s"},
    {L"Gbps", L"Data rate", 125000000, 0, true, L"gbit/s"},

    {L"ms", L"Time", 0.001, 0, false, L"millisecond|milliseconds"},
    {L"s", L"Time", 1, 0, true, L"sec|secs|second|seconds"},
    {L"min", L"Time", 60, 0, true, L"mins|minute|minutes"},
    {L"h", L"Time", 3600, 0, true, L"hr|hrs|hour|hours"},
    {L"d", L"Time", 86400, 0, true, L"day|days"},
    {L"wk", L"Time", 604800, 0, true, L"week|weeks"},
    {L"yr", L"Time", 31557600, 0, false, L"year|years"},

    {L"Pa", L"Pressure", 1, 0, false, L"pascal|pascals"},
    {L"kPa", L"Pressure", 1000, 0, true, L""},
    {L"MPa", L"Pressure", 1e6, 0, false, L""},
    {L"bar", L"Pressure", 1e5, 0, true, L"bars"},
    {L"atm", L"Pressure", 101325, 0, true, L"atmosphere|atmospheres"},
    {L"psi", L"Pressure", 6894.757293168, 0, true, L""},
    {L"mmHg", L"Pressure", 133.322387415, 0, true, L"torr"},

    {L"J", L"Energy", 1, 0, true, L"joule|joules"},
    {L"kJ", L"Energy", 1000, 0, true, L""},
    {L"cal", L"Energy", 4.184, 0, false, L"calorie|calories"},
    {L"kcal", L"Energy", 4184, 0, true, L""},
    {L"Wh", L"Energy", 3600, 0, false, L""},
    {L"kWh", L"Energy", 3.6e6, 0, true, L""},

    {L"W", L"Power", 1, 0, true, L"watt|watts"},
    {L"kW", L"Power", 1000, 0, true, L"kilowatt|kilowatts"},
    {L"hp", L"Power", 745.69987158227022, 0, true, L"horsepower"},

    {L"\u00B0", L"Angle", 1, 0, true, L"deg|degree|degrees"},
    {L"rad", L"Angle", 57.29577951308232, 0, true, L"radian|radians"},
    {L"grad", L"Angle", 0.9, 0, false, L"gon|gradian|gradians"},
    {L"turn", L"Angle", 360, 0, false, L"turns|rev"},

    {L"Hz", L"Frequency", 1, 0, true, L"hertz"},
    {L"kHz", L"Frequency", 1e3, 0, true, L""},
    {L"MHz", L"Frequency", 1e6, 0, true, L""},
    {L"GHz", L"Frequency", 1e9, 0, true, L""},
};

// One color per kind, on the row's icon; a kind from the settings gets one
// of the same by its name.
inline uint32_t KindColor(const std::wstring& kind) {
    static const std::pair<const wchar_t*, uint32_t> kColors[] = {
        {L"Length", 0xFF4FA3F7},    {L"Mass", 0xFFF2994A},      {L"Temperature", 0xFFEB5757},
        {L"Speed", 0xFFBB6BD9},     {L"Area", 0xFF27AE60},      {L"Volume", 0xFF2DB7B7},
        {L"Data", 0xFF6C7BF2},      {L"Data rate", 0xFF9B8CF2}, {L"Time", 0xFFE2B93B},
        {L"Pressure", 0xFFB08968},  {L"Energy", 0xFFF2C94C},    {L"Power", 0xFFEF7AB0},
        {L"Angle", 0xFF9AA5B1},     {L"Frequency", 0xFF56CCF2}, {L"Base Radix", 0xFF8FA0B5},
    };
    for (const auto& [name, color] : kColors) {
        if (kind == name) {
            return color;
        }
    }
    uint32_t hash = 2166136261u;
    for (wchar_t ch : kind) {
        hash = (hash ^ static_cast<uint32_t>(towlower(ch))) * 16777619u;
    }
    return kColors[hash % std::size(kColors)].second;
}

// Lowercase, any script.
inline std::wstring Lower(std::wstring text) {
    if (!text.empty()) {
        CharLowerBuffW(text.data(), static_cast<DWORD>(text.size()));
    }
    return text;
}

// A unit by its symbol or any of its names, whatever the case, the spaces or
// a closing period ("min.").
inline const Unit* FindUnit(const std::wstring& name) {
    std::wstring key;
    for (wchar_t ch : Lower(tools::Trim(name))) {
        if (ch == L' ' && (key.empty() || key.back() == L' ')) {
            continue;
        }
        key += ch;
    }
    for (int pass = 0; pass < 2 && !key.empty(); ++pass) {
        for (const Unit& unit : kUnits) {
            if (key == Lower(unit.symbol)) {
                return &unit;
            }
            for (const wchar_t* name = unit.names; *name;) {
                const wchar_t* end = wcschr(name, L'|');
                const size_t length = end ? static_cast<size_t>(end - name) : wcslen(name);
                if (key.size() == length && key.compare(0, length, name, length) == 0) {
                    return &unit;
                }
                if (!end) {
                    break;
                }
                name = end + 1;
            }
        }
        if (key.back() != L'.') {
            break;
        }
        key.pop_back();
    }
    return nullptr;
}

// The user's decimal point.
inline wchar_t DecimalPoint() {
    wchar_t point[4] = {};
    return GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, point, 4) > 0 && point[0] ? point[0] : L'.';
}

// A number at the start of text: "." or "," as the decimal point (in
// "1,000.5" the last one is), thousands grouped by spaces ("1 000"), the
// other mark ("1,000" in English) or several of one ("1.000.000"), an
// exponent ("1e3"). How many characters it took, or 0.
inline size_t ParseNumber(const std::wstring& text, double* value) {
    size_t i = 0;
    std::wstring span;  // digits and marks, spaces left out
    if (i < text.size() && (text[i] == L'-' || text[i] == L'+')) {
        span += text[i++];
    }
    auto threeDigits = [&](size_t at) {
        return at + 3 <= text.size() && iswdigit(text[at]) && iswdigit(text[at + 1]) && iswdigit(text[at + 2]) &&
               (at + 3 == text.size() || !iswdigit(text[at + 3]));
    };
    bool digits = false;
    while (i < text.size()) {
        const wchar_t ch = text[i];
        if (ch >= L'0' && ch <= L'9') {
            span += ch;
            digits = true;
            ++i;
        } else if ((ch == L'.' || ch == L',') && i + 1 < text.size() && iswdigit(text[i + 1])) {
            span += ch;
            ++i;
        } else if ((ch == L' ' || ch == 0xA0 || ch == 0x202F || ch == L'\'') && digits && threeDigits(i + 1)) {
            ++i;  // a thousands group
        } else {
            break;
        }
    }
    if (!digits) {
        return 0;
    }
    // Which mark is the decimal point.
    const size_t lastDot = span.rfind(L'.'), lastComma = span.rfind(L',');
    wchar_t point = 0;
    if (lastDot != std::wstring::npos && lastComma != std::wstring::npos) {
        point = lastDot > lastComma ? L'.' : L',';
    } else if (lastDot != std::wstring::npos || lastComma != std::wstring::npos) {
        const wchar_t mark = lastDot != std::wstring::npos ? L'.' : L',';
        const size_t at = span.rfind(mark);
        const bool several = span.find(mark) != at;
        const bool groupLike = span.size() - at - 1 == 3 && mark != DecimalPoint();
        point = several || groupLike ? 0 : mark;
    }
    std::string ascii;
    for (wchar_t ch : span) {
        if (ch == point) {
            ascii += '.';
        } else if (ch != L'.' && ch != L',') {
            ascii += static_cast<char>(ch);
        }
    }
    // An exponent: e, a sign, digits.
    if (i < text.size() && (text[i] == L'e' || text[i] == L'E')) {
        size_t j = i + 1;
        std::string exponent = "e";
        if (j < text.size() && (text[j] == L'-' || text[j] == L'+')) {
            exponent += static_cast<char>(text[j++]);
        }
        const size_t digitsAt = j;
        while (j < text.size() && iswdigit(text[j])) {
            exponent += static_cast<char>(text[j++]);
        }
        if (j > digitsAt) {
            ascii += exponent;
            i = j;
        }
    }
    *value = strtod(ascii.c_str(), nullptr);
    return i;
}

// Scientific notation, as 9.9e4: up to 6 significant digits.
inline std::wstring Scientific(double value) {
    if (value == 0 || std::isnan(value) || std::isinf(value)) {
        return value == 0 ? L"0" : L"Error";
    }
    int exponent = static_cast<int>(std::floor(std::log10(std::fabs(value))));
    double mantissa = value / std::pow(10.0, exponent);
    wchar_t text[64];
    swprintf_s(text, L"%.6g", mantissa);
    if (std::fabs(wcstod(text, nullptr)) >= 10) {  // rounded up to 10
        mantissa /= 10;
        ++exponent;
        swprintf_s(text, L"%.6g", mantissa);
    }
    std::wstring result = text;
    std::replace(result.begin(), result.end(), L'.', DecimalPoint());
    return result + L"e" + std::to_wstring(exponent);
}

// A result as people write it: whole numbers whole, others to 7 significant
// digits, the user's decimal point; scientific when too large or small for
// that.
inline std::wstring Format(double value) {
    if (std::isnan(value) || std::isinf(value)) {
        return L"Error";
    }
    const double magnitude = std::fabs(value);
    if (magnitude != 0 && (magnitude >= 1e15 || magnitude < 1e-4)) {
        return Scientific(value);
    }
    wchar_t text[64];
    if (value == std::round(value)) {
        swprintf_s(text, L"%.0f", value);
    } else {
        swprintf_s(text, L"%.7g", value);
    }
    std::wstring result = text;
    std::replace(result.begin(), result.end(), L'.', DecimalPoint());
    return result;
}

// Worth showing in scientific notation too: large or small, and not
// already shown that way (Format).
inline bool Large(double value) {
    const double magnitude = std::fabs(value);
    return (magnitude >= 1e4 || (magnitude != 0 && magnitude < 1e-3)) && Format(value) != Scientific(value);
}

inline Row MakeRow(std::wstring title, std::wstring subtitle, std::wstring copy, const wchar_t* glyph,
                   uint32_t color = 0) {
    Row r;
    r.title = std::move(title);
    r.subtitle = std::move(subtitle);
    r.copyText = std::move(copy);
    r.customGlyph = glyph;
    r.glyphColor = color;
    r.canRunAsAdmin = false;
    return r;
}

inline constexpr wchar_t kCalcGlyph[] = L"\uE1D0";
inline constexpr wchar_t kConvertGlyph[] = L"\uE88E";
inline constexpr wchar_t kDot[] = L" \u2022 ";

// Whether a unit typed is a unit from the settings: by its name, by what it
// stands for (NormalizeUnit), or as the same unit of kUnits ("kilometre",
// "km").
inline bool SameUnit(const std::wstring& typed, const std::wstring& configured) {
    if (Lower(tools::Trim(typed)) == Lower(tools::Trim(configured)) ||
        tools::NormalizeUnit(typed) == tools::NormalizeUnit(configured)) {
        return true;
    }
    const Unit* unit = FindUnit(typed);
    return unit && unit == FindUnit(configured);
}

// A conversion from the settings that kUnits does already: one of those the
// earlier default settings had, left as it was in someone's settings. One
// changed -- a decimal MB to GB, say -- is the user's, and stays.
inline bool Known(const CustomConversion& c) {
    static const wchar_t* const kOldDefaults[][3] = {
        {L"km", L"miles", L"x*0.621371"}, {L"c", L"\u00B0f", L"x*9/5+32"}, {L"kg", L"lbs", L"x*2.20462"},
        {L"m", L"feet", L"x*3.28084"},    {L"cm", L"in", L"x/2.54"},      {L"mb", L"gb", L"x/1024"},
    };
    std::wstring formula;
    for (wchar_t ch : Lower(c.formula)) {
        if (ch != L' ') {
            formula += ch;
        }
    }
    const std::wstring from = Lower(tools::Trim(c.fromUnit)), to = Lower(tools::Trim(c.toUnit));
    for (const auto& old : kOldDefaults) {
        if (from == old[0] && to == old[1] && formula == old[2]) {
            return true;
        }
    }
    return false;
}

// Runs a formula from the settings backwards: possible when it is linear,
// as a * x + b, which three points tell.
inline bool Invert(const std::wstring& formula, double y, double* x) {
    double f0 = 0, f1 = 0, f2 = 0;
    if (!tools::EvaluateConversionFormula(formula, 0, f0) || !tools::EvaluateConversionFormula(formula, 1, f1) ||
        !tools::EvaluateConversionFormula(formula, 2, f2)) {
        return false;
    }
    const double a = f1 - f0;
    if (a == 0 || std::fabs(f2 - (f0 + 2 * a)) > 1e-9 * std::max(1.0, std::fabs(f2))) {
        return false;
    }
    *x = (y - f0) / a;
    return true;
}

// The settings' conversions of a number with the unit typed: from it, and
// back to it where reciprocal.
inline void CustomRows(double number, const std::wstring& unit, const std::vector<CustomConversion>& customs,
                       std::vector<Row>* rows) {
    for (const CustomConversion& c : customs) {
        if (Known(c)) {
            continue;
        }
        double out = 0;
        if (SameUnit(unit, c.fromUnit) && tools::EvaluateConversionFormula(c.formula, number, out)) {
            const std::wstring result = Format(out) + L" " + c.toUnit;
            rows->push_back(MakeRow(Format(number) + L" " + c.fromUnit + L" = " + result,
                                    c.category + kDot + L"Press Enter to copy " + result, result, kConvertGlyph,
                                    KindColor(c.category)));
        }
        if (c.reciprocal && SameUnit(unit, c.toUnit) && Invert(c.formula, number, &out)) {
            const std::wstring result = Format(out) + L" " + c.fromUnit;
            rows->push_back(MakeRow(Format(number) + L" " + c.toUnit + L" = " + result,
                                    c.category + kDot + L"Press Enter to copy " + result, result, kConvertGlyph,
                                    KindColor(c.category)));
        }
    }
}

// Groups of `size` digits from the right, as 111 1111 1111.
inline std::wstring Grouped(const std::wstring& digits, size_t size) {
    std::wstring out;
    for (size_t i = 0; i < digits.size(); ++i) {
        if (i && (digits.size() - i) % size == 0) {
            out += L' ';
        }
        out += digits[i];
    }
    return out;
}

// A whole number in other bases: 2047 = 0x7FF = 0b111 1111 1111 = 0o3777.
inline void RadixRows(double number, std::vector<Row>* rows) {
    if (number < 0 || number > 9007199254740992.0 || number != std::floor(number)) {
        return;
    }
    const unsigned long long n = static_cast<unsigned long long>(number);
    wchar_t hex[32], oct[32];
    swprintf_s(hex, L"%llX", n);
    swprintf_s(oct, L"%llo", n);
    std::wstring bin;
    for (unsigned long long v = n; v || bin.empty(); v >>= 1) {
        bin.insert(bin.begin(), (v & 1) ? L'1' : L'0');
    }
    const std::wstring hexText = std::wstring(L"0x") + (wcslen(hex) > 8 ? Grouped(hex, 4) : hex);
    rows->push_back(MakeRow(Format(number) + L" = " + hexText + L" (Hex) = 0b" + Grouped(bin, 4) + L" (Bin) = 0o" + oct +
                                L" (Oct)",
                            std::wstring(L"Base Radix") + kDot + L"Press Enter to copy 0x" + hex,
                            std::wstring(L"0x") + hex, kConvertGlyph, KindColor(L"Base Radix")));
}

// A number written in another base: 0xFF, 0b1010, 0o17.
inline bool ParseBased(const std::wstring& text, double* value) {
    if (text.size() < 3 || text[0] != L'0') {
        return false;
    }
    const wchar_t mark = static_cast<wchar_t>(towlower(text[1]));
    const int radix = mark == L'x' ? 16 : mark == L'b' ? 2 : mark == L'o' ? 8 : 0;
    if (!radix) {
        return false;
    }
    unsigned long long n = 0;
    for (size_t i = 2; i < text.size(); ++i) {
        const wchar_t ch = static_cast<wchar_t>(towlower(text[i]));
        const int digit = iswdigit(ch) ? ch - L'0' : (ch >= L'a' && ch <= L'f') ? ch - L'a' + 10 : 99;
        if (digit >= radix || n > (1ULL << 53)) {
            return false;
        }
        n = n * radix + digit;
    }
    *value = static_cast<double>(n);
    return true;
}

// Asks for other bases: "hex", "to binary", "in oct".
inline bool BaseWords(std::wstring text) {
    text = Lower(tools::Trim(text));
    for (const wchar_t* lead : {L"to ", L"in ", L"as "}) {
        if (text.starts_with(lead)) {
            text = tools::Trim(text.substr(wcslen(lead)));
        }
    }
    for (const wchar_t* word : {L"hex", L"hexadecimal", L"bin", L"binary", L"oct", L"octal", L"base", L"bases", L"radix"}) {
        if (text == word) {
            return true;
        }
    }
    return false;
}

// A number with a unit, and maybe a target: "100 km", "100 km to mi".
inline bool ConversionRows(double number, const std::wstring& text, const std::vector<CustomConversion>& customs,
                           std::vector<Row>* rows) {
    const Unit* from = nullptr;
    const Unit* to = nullptr;
    std::wstring unitText = text;
    const std::wstring lower = Lower(text);
    for (const wchar_t* separator : {L" to ", L" in ", L" into ", L" as ", L"->", L"\u2192", L"="}) {
        for (size_t at = lower.find(separator); at != std::wstring::npos && !to;
             at = lower.find(separator, at + 1)) {
            const Unit* left = FindUnit(text.substr(0, at));
            const Unit* right = FindUnit(text.substr(at + wcslen(separator)));
            if (left && right && wcscmp(left->kind, right->kind) == 0) {
                from = left;
                to = right;
                unitText = tools::Trim(text.substr(0, at));
            }
        }
    }
    if (!from) {
        from = FindUnit(text);
    }
    const size_t before = rows->size();
    if (from) {
        const double base = number * from->factor + from->offset;
        const std::wstring source = Format(number) + L" " + from->symbol;
        const uint32_t color = KindColor(from->kind);
        std::vector<std::wstring> results;
        for (const Unit& unit : kUnits) {
            if (to ? &unit != to : (!unit.usual || &unit == from || wcscmp(unit.kind, from->kind) != 0)) {
                continue;
            }
            results.push_back(Format((base - unit.offset) / unit.factor) + L" " + unit.symbol);
        }
        if (results.size() > 1) {
            // All of them on one line first.
            std::wstring all = source;
            for (const std::wstring& result : results) {
                all += L" = " + result;
            }
            rows->push_back(MakeRow(all, from->kind + std::wstring(kDot) + L"Press Enter to copy this line", all,
                                    kConvertGlyph, color));
        }
        for (const std::wstring& result : results) {
            rows->push_back(MakeRow(source + L" = " + result,
                                    from->kind + std::wstring(kDot) + L"Press Enter to copy " + result, result,
                                    kConvertGlyph, color));
        }
    }
    if (!to) {
        CustomRows(number, unitText, customs, rows);
    }
    return rows->size() > before;
}

// Not a sum, though it would work out as one: a name, as of a file -- a
// resolution (1920x1080), a date (2024-01-01, 01.02.2024), a span of years
// (2024-2025).
inline bool LooksLikeName(const std::wstring& text) {
    for (size_t i = 1; i + 1 < text.size(); ++i) {
        if ((text[i] == L'x' || text[i] == L'X') && iswdigit(text[i - 1]) && iswdigit(text[i + 1])) {
            return true;
        }
    }
    if (text.find_first_not_of(L"0123456789-./") != std::wstring::npos) {
        return false;
    }
    const size_t marks = std::count_if(text.begin(), text.end(), [](wchar_t ch) { return ch == L'-' || ch == L'.' || ch == L'/'; });
    if (marks >= 2) {
        return true;
    }
    const size_t dash = text.find(L'-');
    return marks == 1 && dash == 4 && (text.size() - dash - 1 == 2 || text.size() - dash - 1 == 4);
}

// The rows for what was typed, when it is something to work out; none when
// it is not.
inline std::vector<Row> Rows(const std::wstring& input) {
    std::vector<Row> rows;
    const std::wstring arg = tools::Trim(input);
    if (arg.find_first_of(L"0123456789") == std::wstring::npos) {
        return rows;  // "e", "pi": words, not sums
    }
    double number = 0;
    if (ParseBased(arg, &number)) {
        RadixRows(number, &rows);
        return rows;
    }
    std::vector<CustomConversion> customs;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        customs = g_settings.unitConversions;
    }
    const size_t used = ParseNumber(arg, &number);
    if (used && used < arg.size()) {
        const std::wstring rest = tools::Trim(arg.substr(used));
        if (BaseWords(rest)) {
            RadixRows(number, &rows);
            return rows;
        }
        // A one-letter unit right after the number is a name: 3d, 4k, 5g.
        const bool spaced = arg[used] == L' ';
        if ((spaced || rest.size() > 1) && ConversionRows(number, rest, customs, &rows)) {
            return rows;
        }
    }
    double value = 0;
    if (used < arg.size() && !LooksLikeName(arg) && tools::EvaluateMath(arg, value)) {  // a sum, not a number alone
        rows.push_back(MakeRow(L"= " + Format(value), arg + kDot + L"Press Enter to copy result", Format(value),
                               kCalcGlyph));
        if (Large(value)) {
            rows.push_back(MakeRow(L"= " + Scientific(value),
                                   std::wstring(L"Scientific notation") + kDot + L"Press Enter to copy " +
                                       Scientific(value),
                                   Scientific(value), kCalcGlyph));
        }
    }
    return rows;
}

}  // namespace calc

void SearchThreadMain() {
    // COM for the apps index: it enumerates shell:AppsFolder.
    HRESULT comHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    apps::Index appIndex;
    apps::UsageCounts appUsage = apps::LoadUsage();
    // The counts to rank by, or null while Learn Favorite Apps is off.
    auto learnedUsage = [&appUsage]() -> apps::UsageCounts* {
        if (g_forgetUsage.exchange(false)) {
            appUsage.clear();
        }
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        return g_settings.learnFavorites ? &appUsage : nullptr;
    };
    if (appIndex.Rebuild()) {
        g_lastAppIndexRebuildTick.store(GetTickCount64());
        Wh_Log(L"apps: indexed (%zu apps)", appIndex.Count());
    } else {
        Wh_Log(L"apps: index failed");
    }

    int iconPixels = IconPixels();
    icons::FileIconCache iconCache(iconPixels);

    // The apps behind the rows currently on screen, in the same order.
    std::vector<const apps::App*> lastHits;

    // One fetch per app, ever. Keyed by name because that is what identifies
    // an entry across index rebuilds.
    std::map<std::wstring, std::vector<BYTE>> appIconCache;
    std::map<std::wstring, std::vector<BYTE>> commandIconCache;

    // An app's icon is the slow part of showing it the first time -- about
    // 10 ms each through the shell, 20-50 ms for a keystroke that brings up a
    // few new ones -- so they are fetched ahead, while there is nothing to
    // search, one at a time so a keystroke never waits for more than one.
    // Walks the index once, and again after a rebuild.
    size_t prefetchNext = 0;
    auto prefetchIcon = [&] {
        const apps::App* app = appIndex.At(prefetchNext++);
        if (!app || appIconCache.count(app->name)) {
            return;
        }
        std::vector<BYTE> pixels;
        if (!app->isSetting || app->pidl) {
            FetchAppIcon(app, iconPixels, &pixels);
        }
        appIconCache.emplace(app->name, std::move(pixels));
    };

    everything::Client client;
    if (!client.Init()) {
        Wh_Log(L"search: could not create the reply window");
        if (SUCCEEDED(comHr)) {
            CoUninitialize();
        }
        return;
    }
    Wh_Log(L"search: ready (Everything %ls)",
        everything::FindIpcWindow() ? L"found" : L"NOT running");

    std::wstring last;
    bool haveLast=false,lastFilesVisible=true;
    uint64_t lastSettingsRevision = 0;
    while (!g_searchQuit.load()) {
        std::wstring query;
        {
            std::unique_lock<std::mutex> lock(g_queryMutex);
            auto hasWork = [] {
                return g_queryDirty.load() || g_searchQuit.load() || (g_launchRequest.load() >= 0) ||
                       !g_commandUsageRequests.empty() || g_appIndexNeedsRefresh.load();
            };
            if (!hasWork() && (iconCache.HasPending() || prefetchNext < appIndex.Count())) {
                lock.unlock();
                // Files first: they are on screen now.
                if (iconCache.FetchPending()) {
                    if (!iconCache.HasPending()) {
                        PublishLateFileIcons(iconCache.TakeLate());
                    }
                } else {
                    prefetchIcon();
                }
                continue;
            }
            g_queryWake.wait(lock, hasWork);
            if (g_searchQuit.load()) {
                if (SUCCEEDED(comHr)) {
                    CoUninitialize();
                }
                return;
            }
            if (!g_commandUsageRequests.empty()) {
                auto requests=std::move(g_commandUsageRequests);
                g_commandUsageRequests.clear();
                auto usage=learnedUsage();
                haveLast=false;
                lock.unlock();
                if(usage) {
                    for(const auto& key:requests) apps::RecordUsage(usage,key);
                    apps::SaveUsage(*usage);
                }
                continue;
            }
            int wanted = g_launchRequest.exchange(-1);
            if (wanted >= 0) {
                haveLast=false; // refresh suggestions after a launch changes favorite counts
                bool asAdmin = g_launchAsAdmin.exchange(false);
                if (static_cast<size_t>(wanted) < lastHits.size() && lastHits[wanted]) {
                    const auto* app = lastHits[wanted];
                    apps::UsageCounts* usage = learnedUsage();
                    apps::RecordUsage(usage,app->nameLower);
                    std::wstring name = app->name;
                    std::wstring targetPath = app->targetPath;
                    ITEMIDLIST* pidlClone = app->pidl ? ILClone(app->pidl.get()) : nullptr;
                    lock.unlock();
                    if (usage) {
                        apps::SaveUsage(*usage);  // this thread's own: no lock needed
                    }

                    SpawnTrackedLaunch([name = std::move(name), targetPath = std::move(targetPath), pidlClone, asAdmin] {
                        LaunchAppAsync(name, targetPath, pidlClone, asAdmin);
                        if (pidlClone) {
                            ILFree(pidlClone);
                        }
                    });
                } else {
                    lock.unlock();
                }
                continue;
            }

            // Serve a pending query from the existing index first. Refreshing
            // shell:AppsFolder must not precede the first results on reopening.
            bool shouldRebuild = !g_queryDirty.load() && g_appIndexNeedsRefresh.exchange(false);
            if (shouldRebuild) {
                lock.unlock();
                g_lastAppIndexRebuildTick.store(GetTickCount64());
                bool rebuilt = appIndex.Rebuild();
                lock.lock();
                if (rebuilt) {
                    lastHits.clear();
                    prefetchNext = 0;
                    g_launchRequest.store(-1);
                    last.clear();
                    haveLast=false;
                    g_queryDirty.store(true);
                    Wh_Log(L"apps: dynamically refreshed (%zu apps)", appIndex.Count());
                }
            }

            if (!g_queryDirty.exchange(false)) {
                continue;
            }
            query = g_pendingQuery;
        }

        // Optional settle (searchDebounceMs, off by default). Not needed to keep
        // up with typing: this thread takes the latest text each time round,
        // so keystrokes that land during a search are folded into the next
        // one. A delay here would only be added to every keystroke -- 25 ms of
        // the ~110 it took, when that was the default. A fresh query from
        // empty never waits, so results are ready before the overlay reveals.
        int debounceMs = 0;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            debounceMs = g_settings.searchDebounceMs;
        }
        if (haveLast && !last.empty() && !query.empty() && debounceMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(debounceMs));
            std::lock_guard<std::mutex> lock(g_queryMutex);
            if (g_pendingQuery != query) {
                g_queryDirty.store(true);
                continue;
            }
        }

        const bool filesVisible=FilesPanelVisible();
        const uint64_t settingsRevision = g_settingsRevision.load();
        if (haveLast && query == last && filesVisible==lastFilesVisible && settingsRevision==lastSettingsRevision) {
            continue;
        }
        last = query;
        haveLast=true; lastFilesVisible=filesVisible;
        if (settingsRevision != lastSettingsRevision) {
            commandIconCache.clear();
            lastSettingsRevision = settingsRevision;
        }

        // Start may be on a display with another scale by now.
        if (int pixels = IconPixels(); pixels != iconPixels) {
            iconPixels = pixels;
            iconCache.SetSize(pixels);
            appIconCache.clear();
            commandIconCache.clear();
            prefetchNext = 0;
        }

        std::wstring qTrim = tools::Trim(query);
        std::wstring qLower = tools::ToLower(qTrim);

        bool isIpCommand = (qLower == L"/ip" || qLower.starts_with(L"/ip "));
        bool isExplicitWeb = query.starts_with(L"?");
        std::wstring webQuery;
        ResolvedWebQuery explicitWeb;
        if (isExplicitWeb) {
            webQuery = query.substr(1);
            while (!webQuery.empty() && webQuery.front() == L' ') {
                webQuery.erase(0, 1);
            }
            explicitWeb = ResolveWebSearch(webQuery);
        }

        std::vector<everything::Result> pool;
        DWORD total = 0;
        long long ms = 0;

        int maxFiles = 12;
        int maxApps = 6;
        bool filterNoise = true;
        std::vector<std::wstring> excludedPaths;
        std::vector<CustomCommand> customCommands;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            maxFiles = g_settings.maxFileResults;
            maxApps = g_settings.maxAppResults;
            filterNoise = g_settings.filterNoisyPaths;
            excludedPaths = g_settings.excludedPaths;
            customCommands = g_settings.customCommands;
        }
        static const std::vector<std::wstring> s_disabledNoise;
        const std::vector<std::wstring>* noisePtr = filterNoise ? &excludedPaths : &s_disabledNoise;

        // Queries and ranks the files. The query as typed is the fast one and
        // is usually enough: Rank takes the excluded paths out of what it
        // returns. Only when that leaves too few clean results while
        // Everything has more matches than it sent is the query run again
        // with the excluded paths left out at the source (WithExclusions) --
        // it costs more, and most queries never need it. Noise still shows
        // when it is all there is.
        auto queryFiles = [&](const std::wstring& text) {
            const size_t limit = static_cast<size_t>(maxFiles);
            if (!client.Query(text, ranker::kDefaultPool, &pool, &total)) {
                return false;
            }
            size_t clean = ranker::Rank(&pool, text, limit, noisePtr);
            if (clean >= limit || total <= ranker::kDefaultPool) {
                return true;
            }
            const std::wstring filtered = ranker::WithExclusions(text, noisePtr);
            if (filtered == text) {
                return true;
            }
            std::vector<everything::Result> more;
            DWORD moreTotal = 0;
            if (client.Query(filtered, ranker::kDefaultPool, &more, &moreTotal) &&
                ranker::Rank(&more, text, limit, noisePtr) > clean) {
                pool = std::move(more);
                total = moreTotal;
            }
            return true;
        };

        if (!filesVisible || query.empty() || isIpCommand) {
            pool.clear();
            total = 0;
            ms = 0;
        } else if (isExplicitWeb) {
            if (!explicitWeb.queryTerm.empty()) {
                auto start = std::chrono::steady_clock::now();
                queryFiles(explicitWeb.queryTerm);
                ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::steady_clock::now() - start)
                         .count();
            }
        } else {
            auto start = std::chrono::steady_clock::now();
            bool ok = queryFiles(query);
            ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::steady_clock::now() - start)
                          .count();
            if (!ok) {
                Wh_Log(L"search: '%ls' failed (Everything running?)", query.c_str());
                pool.clear();
                total = 0;
            }
        }

        // Apps are a name match over an index built once at startup, so this
        // costs nothing next to the file query.
        auto usage=learnedUsage();
        std::vector<Row> appRows;
        lastHits.clear();

        if (isIpCommand) {
            auto ifaces = tools::GetNetworkInterfaces();
            if (ifaces.empty()) {
                Row r;
                r.title = L"No Active Network Interfaces";
                r.subtitle = L"Check your Wi-Fi, Ethernet, or VPN connection";
                r.customGlyph = L"\uE701";
                r.canRunAsAdmin = false;
                r.appIndex = static_cast<int>(lastHits.size());
                lastHits.push_back(nullptr);
                appRows.push_back(std::move(r));
            } else {
                for (const auto& iface : ifaces) {
                    Row r;
                    r.title = iface.ip;
                    std::wstring sub = iface.typeLabel + L": " + iface.adapterName;
                    if (!iface.mask.empty()) sub += L" \u2022 Subnet: " + iface.mask;
                    if (!iface.gateway.empty()) sub += L" \u2022 GW: " + iface.gateway;
                    r.subtitle = sub + L" \u2022 Press Enter to copy";
                    r.copyText = iface.ip;
                    r.customGlyph = iface.glyph;
                    r.canRunAsAdmin = false;
                    r.appIndex = static_cast<int>(lastHits.size());
                    lastHits.push_back(nullptr);
                    appRows.push_back(std::move(r));
                }
            }
        } else if (isExplicitWeb) {
            Row webRow;
            if (explicitWeb.queryTerm.empty()) {
                if (explicitWeb.isShortcut) {
                    webRow.title = L"Open " + explicitWeb.serviceName;
                    webRow.subtitle = explicitWeb.serviceName + L" \u2022 " + explicitWeb.homeUrl;
                } else {
                    webRow.title = L"Search the web";
                    webRow.subtitle = explicitWeb.serviceName + L" Search";
                }
                webRow.openPath = explicitWeb.homeUrl;
            } else {
                webRow.title = L"Search " + explicitWeb.serviceName + L" for \"" + explicitWeb.queryTerm + L"\"";
                webRow.subtitle = explicitWeb.serviceName + L" Search \u2022 " + explicitWeb.queryTerm;
                webRow.openPath = explicitWeb.searchUrl;
            }
            webRow.canRunAsAdmin = false;
            webRow.appIndex = -1;
            appRows.push_back(std::move(webRow));
        } else {
            // A sum, a conversion or another base typed: worked out above the
            // apps (calc).
            for (Row& r : calc::Rows(qTrim)) {
                r.appIndex = static_cast<int>(lastHits.size());
                lastHits.push_back(nullptr);
                appRows.push_back(std::move(r));
            }
            auto matches=query.empty() ? appIndex.Suggest(static_cast<size_t>(maxApps),usage)
                                       : appIndex.Search(query,static_cast<size_t>(maxApps),usage);
            for (const apps::Match& m : matches) {
                if (!m.app) {
                    continue;
                }
                Row row;
                row.title = m.app->name;
                row.matchScore = m.score;
                row.matchRank = m.rank;
                bool isSettingItem = m.app->isSetting;
                row.isSetting = isSettingItem;

                bool canAdmin = true;
                bool isFileTarget = false;
                if (isSettingItem) {
                    if (!m.app->area.empty()) {
                        row.subtitle = L"Settings \u2022 " + m.app->area;
                    } else {
                        row.subtitle = L"Settings";
                    }
                    canAdmin = false;
                } else if (!m.app->targetPath.empty() && GetFileAttributesW(m.app->targetPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    row.subtitle = m.app->targetPath;
                    canAdmin = true;
                    isFileTarget = true;
                } else if (!m.app->linkTarget.empty() &&
                           GetFileAttributesW(m.app->linkTarget.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    // Shown, and used by the file actions, but not launched:
                    // the app ID keeps the shortcut's arguments and taskbar
                    // identity.
                    row.subtitle = m.app->linkTarget;
                    row.programPath = m.app->linkTarget;
                } else {
                    row.subtitle = L"Application";
                }
                row.canRunAsAdmin = canAdmin;
                row.isFile = isFileTarget;
                row.openPath = m.app->targetPath;
                row.appIndex = static_cast<int>(lastHits.size());

                auto cached = appIconCache.find(m.app->name);
                if (cached == appIconCache.end()) {
                    std::vector<BYTE> pixels;
                    if (!m.app->isSetting || m.app->pidl) {
                        FetchAppIcon(m.app, iconPixels, &pixels);
                    }
                    cached = appIconCache.emplace(m.app->name, std::move(pixels))
                                 .first;
                }
                row.icon = cached->second;

                appRows.push_back(std::move(row));
                // The App outlives this loop -- the index owns it and lives as
                // long as this thread -- so a pointer is safe here in a way it
                // would not be inside a XAML click handler.
                lastHits.push_back(m.app);
            }

        }

        if (!isIpCommand && !isExplicitWeb && !customCommands.empty()) {
            AddCustomCommandRows(&appRows, customCommands, query,
                                 static_cast<size_t>(maxApps), settingsRevision,
                                 iconPixels, &commandIconCache, usage);
        }

        std::vector<Row> fileRows;
        iconCache.NewResults(std::chrono::milliseconds(25));
        for (const everything::Result& r : pool) {
            Row row;
            row.title = r.name;
            row.subtitle = r.path;
            row.isFolder = r.isFolder;
            row.openPath = r.path;
            if (!row.openPath.empty() && row.openPath.back() != L'\\') {
                row.openPath += L'\\';
            }
            row.openPath += r.name;
            // One shell call per distinct extension, not per row, except for
            // files with an icon of their own (see FileIconCache).
            if (const std::vector<BYTE>* pixels =
                    iconCache.Get(row.openPath, r.isFolder)) {
                row.icon = *pixels;
            }
            fileRows.push_back(std::move(row));
        }

        {
            std::lock_guard<std::mutex> lock(g_resultsMutex);
            g_appRows = appRows;
            g_fileRows = fileRows;
            g_rowsQuery = query;
            g_totalMatches.store(total);
        }
        RequestRender();
        Wh_Log(L"search: '%ls' -> %u files (kept %zu), %zu apps (%lld ms)", query.c_str(),
            total, pool.size(), appRows.size(), static_cast<long long>(ms));
        for (size_t i = 0; i < appRows.size(); ++i) {
            Wh_Log(L"    [app %zu] %ls -> %ls", i, appRows[i].title.c_str(), appRows[i].subtitle.c_str());
        }
        for (size_t i = 0; i < pool.size() && i < 5; ++i) {
            Wh_Log(L"    [file %zu] %ls  %ls", i, pool[i].name.c_str(), pool[i].path.c_str());
        }
    }
}


void TeardownSearchUi() {
    Wh_Log(L"teardown: tearing down search UI on XAML thread");
    g_contextMenuOpen=false; g_activeContextFlyout={}; g_contextMenuClosedTick=0;
    // Before anything is dropped: see g_xamlHandlers.
    try {
        g_xamlHandlers.reset();
    } catch (...) {}

    try {
        HidePreview();
        if (g_previewResize) {
            g_previewResize.Stop();
        }
        g_previewResize = nullptr;
        g_previewResizeAnimation = nullptr;
        g_previewTimer = nullptr;
        g_previewFrameTimer = nullptr;
        g_previewVideo = nullptr;
        g_previewText = nullptr;
        g_previewPopup = nullptr;
        g_previewCard = nullptr;
        g_previewDetails = nullptr;
        g_previewThumbBox = nullptr;
        g_previewImage = nullptr;
        g_previewName = nullptr;
        g_previewFacts = nullptr;
        g_previewPath = nullptr;
    } catch (...) {}

    try {
        g_ourBoxChanged.revoke();
        g_ourBoxLost.revoke();
    } catch (...) {}

    try {
        if (g_resultsList) {
            g_resultsList.Children().Clear();
        }
        if (g_appsList) {
            g_appsList.Children().Clear();
        }
        if (g_resultsHost) {
            g_resultsHost.Visibility(wux::Visibility::Collapsed);
            g_resultsHost.Opacity(0.0);
            g_resultsHost.IsHitTestVisible(false);
            auto parent = wuxm::VisualTreeHelper::GetParent(g_resultsHost);
            if (parent) {
                if (auto parentPanel = parent.try_as<wuxc::Panel>()) {
                    uint32_t idx = 0;
                    if (parentPanel.Children().IndexOf(g_resultsHost, idx)) {
                        parentPanel.Children().RemoveAt(idx);
                    }
                }
            }
        }
    } catch (...) {}

    g_activeAppsOpt.reset();
    g_appButtonsOpt.reset();
    g_fileButtonsOpt.reset();
    g_resultsHost = nullptr;
    g_searchTextBrush = nullptr;
    g_resolvedSearchTextBrush = nullptr;
    g_searchBoxTextBrush = nullptr;
    g_searchPlaceholderBrush = nullptr;
    g_searchIcons.reset();
    g_searchIconSweepAt = 256;
    g_renderPending.store(false);
    g_renderAfterSlide = false;
    ResetSearchContainerContrast();
    g_ourBox = nullptr;
    g_appsList = nullptr;
    g_appsScroll = nullptr;
    g_filesScroll = nullptr;
    g_resultsList = nullptr;
    g_appsHeaderHolder = nullptr;
    g_filesHeaderHolder = nullptr;
    g_searchBarBorder = nullptr;
    g_divider = nullptr;
    g_footerBorder = nullptr;
    g_footerStatus = nullptr;
    g_footerHints = nullptr;
    g_currentAppRows.clear();
    g_currentFileRows.clear();
    g_isOverlayVisible.store(false);
    g_isHiding.store(false);
    Wh_Log(L"teardown: search UI teardown complete");
}


// ===========================================================================
// Independent Windows 11 search. Search providers, cards and actions above are
// reused unchanged; only the window ownership and Start entry point differ.
// ===========================================================================
[[clang::no_destroy]] winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource g_searchWindowSource{nullptr};
[[clang::no_destroy]] winrt::Windows::UI::Xaml::Hosting::WindowsXamlManager g_searchWindowManager{nullptr};
[[clang::no_destroy]] winrt::com_ptr<IDesktopWindowXamlSourceNative2> g_searchWindowNative;
[[clang::no_destroy]] wuxc::Grid g_searchWindowRoot{nullptr};
HWND g_searchWindowChild = nullptr;
RECT g_searchWindowWork{};
bool g_searchWindowChanging = false;
constexpr UINT_PTR kSearchWindowDismissTimer = 0x701;
constexpr UINT kSearchWindowLayoutMessage = WM_APP + 0x71;
constexpr UINT kSearchWindowShowMessage = WM_APP + 0x72;

void SearchWindowTrace(const wchar_t* message, HRESULT hr = S_OK) {
    Wh_Log(L"[SearchWindow] %ls: %08X", message, static_cast<unsigned>(hr));
}

// Capture the opening text before Start's native type-to-search consumes it.
// The hook runs on its own message thread: no XAML, I/O, or cross-process
// SendMessage runs inside it. Only Start/Search and the brief handoff qualify.
constexpr UINT kSearchWindowDismissMenuMessage = WM_APP + 0x75;
struct SearchWindowInput {
    enum Kind { Text, Backspace, Enter, Escape, Show } kind = Text;
    std::wstring text;
};
std::mutex g_searchWindowInputMutex;
std::vector<SearchWindowInput> g_searchWindowInputs;
std::atomic<unsigned> g_searchWindowBuffered{0};
std::atomic<bool> g_searchWindowInputReady{false};
std::atomic<bool> g_searchWindowMenuTyping{false};
int g_searchWindowMenuCaret=-1,g_searchWindowMenuSelection=0; // UI thread only
std::atomic<bool> g_searchWindowInputStop{false};
HANDLE g_searchWindowInputThread = nullptr;
HANDLE g_searchWindowInputStarted = nullptr;
DWORD g_searchWindowInputThreadId = 0;
BYTE g_searchWindowKeyState[256]{}; // input thread only
bool g_searchWindowSwallowed[256]{};
bool g_searchWindowDeadKey = false;
HKL g_searchWindowDeadLayout = nullptr;

void ClearSearchWindowDeadKey() {
    if (!g_searchWindowDeadKey) return;
    BYTE state[256]{};
    wchar_t ignored[8]{};
    ToUnicodeEx(VK_SPACE, MapVirtualKeyExW(VK_SPACE, MAPVK_VK_TO_VSC, g_searchWindowDeadLayout),
                state, ignored, ARRAYSIZE(ignored), 0, g_searchWindowDeadLayout);
    g_searchWindowDeadKey = false;
}

// The foreground host can retain logical XAML focus after its native input
// window loses keyboard focus. This check is also safe on the input thread.
bool SearchWindowHasNativeFocus(HWND host) {
    if (!host) return false;
    GUITHREADINFO info{sizeof(info)};
    const DWORD thread = GetWindowThreadProcessId(host, nullptr);
    return thread && GetGUIThreadInfo(thread, &info) && info.hwndFocus &&
           IsChild(host, info.hwndFocus);
}

bool QueueSearchWindowInput(HWND host, SearchWindowInput input) {
    std::lock_guard lock(g_searchWindowInputMutex);
    if (g_searchWindowInputStop.load() || g_searchWindowBuffered.load() >= 256) return false;
    g_searchWindowInputs.push_back(std::move(input));
    g_searchWindowBuffered.fetch_add(1);
    if (PostMessageW(host, kSearchWindowInputMessage, 0, 0)) return true;
    g_searchWindowInputs.pop_back();
    g_searchWindowBuffered.fetch_sub(1);
    return false;
}

LRESULT CALLBACK SearchWindowKeyboardProc(int code, WPARAM message, LPARAM param) try {
    if (code != HC_ACTION || g_searchWindowInputStop.load())
        return CallNextHookEx(nullptr, code, message, param);
    const auto& key = *reinterpret_cast<const KBDLLHOOKSTRUCT*>(param);
    const UINT vk = key.vkCode;
    if (vk >= 256) return CallNextHookEx(nullptr, code, message, param);
    const bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
    const bool wasDown = (g_searchWindowKeyState[vk] & 0x80) != 0;
    BYTE toggle = g_searchWindowKeyState[vk] & 1;
    if (vk == VK_CAPITAL && down && !wasDown) toggle ^= 1;
    g_searchWindowKeyState[vk] = (down ? 0x80 : 0) | toggle;
    g_searchWindowKeyState[VK_SHIFT] = (g_searchWindowKeyState[VK_LSHIFT] | g_searchWindowKeyState[VK_RSHIFT]) & 0x80;
    g_searchWindowKeyState[VK_CONTROL] = (g_searchWindowKeyState[VK_LCONTROL] | g_searchWindowKeyState[VK_RCONTROL]) & 0x80;
    g_searchWindowKeyState[VK_MENU] = (g_searchWindowKeyState[VK_LMENU] | g_searchWindowKeyState[VK_RMENU]) & 0x80;
    if (!down) {
        if (std::exchange(g_searchWindowSwallowed[vk], false)) return 1;
        return CallNextHookEx(nullptr, code, message, param);
    }
    // Let Windows deliver a registered hotkey. In particular, a bare key or
    // Shift-only binding must not be consumed by Start's typing buffer first.
    if (AlternativeShortcutMatches(g_alternativeShortcutActive.load(),vk,g_searchWindowKeyState)) {
        g_searchWindowSwallowed[vk]=false;
        return CallNextHookEx(nullptr,code,message,param);
    }
    HWND host = g_searchWindowWindow.load();
    HWND inputTarget = host ? host : g_hExplorerHelperWnd.load();
    HWND foreground = GetForegroundWindow();
    const bool inHost = host && (foreground == host || GetAncestor(foreground,GA_ROOTOWNER)==host);
    const bool menuActive=g_contextMenuOpen.load() && host && foreground &&
        (inHost || GetWindowThreadProcessId(foreground,nullptr)==GetWindowThreadProcessId(host,nullptr));
    if(menuActive) {
        const bool modified=(g_searchWindowKeyState[VK_CONTROL] | g_searchWindowKeyState[VK_MENU] |
                             g_searchWindowKeyState[VK_LWIN] | g_searchWindowKeyState[VK_RWIN]) & 0x80;
        if(vk==VK_ESCAPE && !modified) {
            if(g_searchWindowSwallowed[vk] || PostMessageW(host,kSearchWindowDismissMenuMessage,0,0)) {
                g_searchWindowSwallowed[vk]=true; return 1;
            }
        }
        // Arrows, Enter and Tab navigate the menu. Printable text and
        // Backspace continue below, closing the menu and editing search.
        if(vk==VK_RETURN || vk==VK_TAB || vk==VK_UP || vk==VK_DOWN ||
           vk==VK_LEFT || vk==VK_RIGHT || vk==VK_HOME || vk==VK_END ||
           vk==VK_PRIOR || vk==VK_NEXT) return CallNextHookEx(nullptr,code,message,param);
    }
    if(inHost && vk==VK_ESCAPE && g_searchWindowSwallowed[vk]) return 1;
    const bool inShell = foreground && (foreground == CachedStartWindow() || IsSearchHostWindow(foreground));
    const bool missingNativeFocus = host && foreground == host && g_isOverlayVisible.load() &&
        !g_dragging.load() && !menuActive && !SearchWindowHasNativeFocus(host);
    const bool handoff = g_searchWindowBuffered.load() || !g_searchWindowInputReady.load() ||
                         g_searchWindowDeadKey || missingNativeFocus;
    const bool menuTyping=g_searchWindowMenuTyping.load() && host && foreground &&
        GetWindowThreadProcessId(foreground,nullptr)==GetWindowThreadProcessId(host,nullptr);
    if (!inputTarget || (!inShell && !menuActive && !menuTyping && !(host && foreground == host && handoff))) {
        if (!inHost) ClearSearchWindowDeadKey();
        g_searchWindowSwallowed[vk] = false;
        return CallNextHookEx(nullptr, code, message, param);
    }
    const bool ctrl = (g_searchWindowKeyState[VK_CONTROL] & 0x80) != 0;
    const bool alt = (g_searchWindowKeyState[VK_MENU] & 0x80) != 0;
    const bool altGr = (g_searchWindowKeyState[VK_RMENU] & 0x80) != 0;
    if (((ctrl || alt) && !altGr) ||
        ((g_searchWindowKeyState[VK_LWIN] | g_searchWindowKeyState[VK_RWIN]) & 0x80))
        return CallNextHookEx(nullptr, code, message, param);

    SearchWindowInput input;
    if ((vk == VK_BACK || vk == VK_RETURN || vk == VK_ESCAPE) &&
        (g_searchWindowBuffered.load() || g_searchWindowDeadKey || inHost || menuActive || menuTyping)) {
        ClearSearchWindowDeadKey();
        input.kind = vk == VK_BACK ? SearchWindowInput::Backspace :
                     vk == VK_RETURN ? SearchWindowInput::Enter : SearchWindowInput::Escape;
    } else {
        HKL layout = g_searchWindowDeadKey ? g_searchWindowDeadLayout :
                     GetKeyboardLayout(GetWindowThreadProcessId(foreground, nullptr));
        wchar_t chars[8]{};
        int count;
        if (vk == VK_PACKET) {
            chars[0] = static_cast<wchar_t>(key.scanCode);
            count = 1;
        } else {
            // A stateless translation preserves Shift, Caps Lock, AltGr and
            // the active layout without changing native text input. Only a
            // dead-key sequence uses this dedicated thread's compose state.
            count = ToUnicodeEx(vk, key.scanCode, g_searchWindowKeyState, chars,
                                ARRAYSIZE(chars), g_searchWindowDeadKey ? 0 : 4, layout);
        }
        if (count < 0) {
            if (!g_searchWindowDeadKey)
                ToUnicodeEx(vk, key.scanCode, g_searchWindowKeyState, chars, ARRAYSIZE(chars), 0, layout);
            g_searchWindowDeadKey = true;
            g_searchWindowDeadLayout = layout;
            input.kind = SearchWindowInput::Show;
        } else if (count > 0 && chars[0] >= 0x20 && chars[0] != 0x7f) {
            g_searchWindowDeadKey = false;
            input.text.assign(chars, std::min(count, static_cast<int>(ARRAYSIZE(chars))));
        } else {
            return CallNextHookEx(nullptr, code, message, param);
        }
    }
    const bool wasMenuTyping=g_searchWindowMenuTyping.load();
    if(menuActive) g_searchWindowMenuTyping=true;
    if (!QueueSearchWindowInput(inputTarget, std::move(input))) {
        if(menuActive) g_searchWindowMenuTyping=wasMenuTyping;
        return CallNextHookEx(nullptr, code, message, param);
    }
    g_searchWindowSwallowed[vk] = true;
    return 1;
} catch (...) {
    return CallNextHookEx(nullptr, code, message, param);
}

DWORD WINAPI SearchWindowInputThreadProc(void*) {
    MSG message;
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    for (int i = 0; i < 256; ++i)
        g_searchWindowKeyState[i] = (GetAsyncKeyState(i) & 0x8000) ? 0x80 : 0;
    g_searchWindowKeyState[VK_CAPITAL] |= GetKeyState(VK_CAPITAL) & 1;
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, SearchWindowKeyboardProc, GetCurrentModuleHandle(), 0);
    SearchWindowTrace(hook ? L"opening input buffer ready" : L"opening input hook failed",
                   hook ? S_OK : HRESULT_FROM_WIN32(GetLastError()));
    SetEvent(g_searchWindowInputStarted);
    if (!hook) return 0;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    UnhookWindowsHookEx(hook);
    ClearSearchWindowDeadKey();
    return 0;
}

void StartSearchWindowInput() {
    if (g_searchWindowInputThread) return;
    g_searchWindowInputStop = false;
    g_searchWindowInputStarted = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_searchWindowInputStarted) return;
    g_searchWindowInputThread = CreateThread(nullptr, 0, SearchWindowInputThreadProc, nullptr, 0, &g_searchWindowInputThreadId);
    if (g_searchWindowInputThread) WaitForSingleObject(g_searchWindowInputStarted, 1000);
}

void StopSearchWindowInput() {
    g_searchWindowInputStop = true;
    if (g_searchWindowInputThread) {
        PostThreadMessageW(g_searchWindowInputThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_searchWindowInputThread, INFINITE);
        CloseHandle(g_searchWindowInputThread);
        g_searchWindowInputThread = nullptr;
    }
    if (g_searchWindowInputStarted) CloseHandle(g_searchWindowInputStarted);
    g_searchWindowInputStarted = nullptr;
    std::lock_guard lock(g_searchWindowInputMutex);
    g_searchWindowInputs.clear();
    g_searchWindowBuffered = 0;
}

bool HasBufferedSearchWindowInput() {
    return g_searchWindowBuffered.load() != 0;
}

void ApplySearchWindowInput() try {
    // Keep the count nonzero until the whole batch has reached the textbox,
    // so later physical keys cannot overtake the opening letters.
    std::vector<SearchWindowInput> inputs;
    {
        std::lock_guard lock(g_searchWindowInputMutex);
        inputs.swap(g_searchWindowInputs);
    }
    if (inputs.empty()) return;
    struct Drained {
        unsigned count;
        ~Drained() { g_searchWindowBuffered.fetch_sub(count); }
    } drained{static_cast<unsigned>(inputs.size())};
    HWND host = g_searchWindowWindow.load();
    HWND foreground = GetForegroundWindow();
    const bool inHost=foreground && host && (foreground==host || GetAncestor(foreground,GA_ROOTOWNER)==host ||
        (g_searchWindowMenuTyping.load() && GetWindowThreadProcessId(foreground,nullptr)==GetWindowThreadProcessId(host,nullptr)));
    if (foreground && (inHost || foreground==CachedStartWindow() || IsSearchHostWindow(foreground))) {
        if(inHost && IsWindowVisible(host) && !g_searchWindowWaitingForStart) {
            // Keep the query and caret. The first printable character closes
            // the whole menu, and every buffered character reaches the box.
            const bool menuTyping=g_searchWindowMenuTyping.load();
            if(menuTyping && g_searchWindowMenuCaret<0) {
                g_searchWindowMenuCaret=g_ourBox.SelectionStart(); g_searchWindowMenuSelection=g_ourBox.SelectionLength();
            }
            DismissSearchContextMenu();
            QueueSearchBoxRefocus(true);
            if(menuTyping) { g_ourBox.SelectionStart(g_searchWindowMenuCaret); g_ourBox.SelectionLength(g_searchWindowMenuSelection); }
        } else SearchWindowShow();
        for (auto const& input : inputs) {
            if (input.kind == SearchWindowInput::Text) {
                const int caret=g_ourBox.SelectionStart();
                g_ourBox.SelectedText(winrt::hstring{input.text});
                g_ourBox.SelectionStart(caret+static_cast<int32_t>(input.text.size()));
                g_ourBox.SelectionLength(0);
            } else if (input.kind == SearchWindowInput::Backspace) {
                std::wstring text{g_ourBox.Text()};
                size_t caret=std::min(static_cast<size_t>(g_ourBox.SelectionStart()),text.size());
                size_t length=std::min(static_cast<size_t>(g_ourBox.SelectionLength()),text.size()-caret);
                if(length) text.erase(caret,length);
                else if(caret) {
                    size_t previous=caret-1;
                    if(text[previous]>=0xDC00 && text[previous]<=0xDFFF && previous &&
                       text[previous-1]>=0xD800 && text[previous-1]<=0xDBFF) --previous;
                    text.erase(previous,caret-previous); caret=previous;
                }
                g_ourBox.Text(text); g_ourBox.SelectionStart(static_cast<int32_t>(caret));
                g_ourBox.SelectionLength(0);
            } else if (input.kind == SearchWindowInput::Enter) {
                HandleNavigationKey(winrt::Windows::System::VirtualKey::Enter, false);
            } else if (input.kind == SearchWindowInput::Escape) {
                if (g_ourBox.Text().empty()) SearchWindowHide();
                else g_ourBox.Text(L"");
            }
        }
        if(g_searchWindowMenuTyping.load()) {
            g_searchWindowMenuCaret=g_ourBox.SelectionStart(); g_searchWindowMenuSelection=g_ourBox.SelectionLength();
        }
        wchar_t detail[96]{};
        swprintf_s(detail, L"opening input delivered, events=%zu length=%u", inputs.size(), g_ourBox.Text().size());
        SearchWindowTrace(detail);
    }
} catch (...) {
    SearchWindowTrace(L"opening input delivery failed", winrt::to_hresult());
}


// Move the complete host so acrylic and the input region follow its contents.
// The ordinary UI timer is only for the hidden Start handoff. Visible motion
// uses a high-resolution clock at the destination display's refresh rate.
constexpr UINT_PTR kSearchWindowMotionTimer = 0x702;
constexpr UINT_PTR kSearchWindowStyleTimer = 0x703;
constexpr UINT kSearchWindowSlideFrameMessage = WM_APP + 0x76;

double SearchMotionTime() {
    return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

struct SearchWindowSlide {
    double from = 0, to = 0, started = 0, duration = 0;
    bool closing = false;
    double Value(double now) const {
        if (duration <= 0) return to;
        const double t = std::clamp((now-started)/duration,0.0,1.0);
        const double ease = closing ? t*t*t : 1-(1-t)*(1-t)*(1-t);
        return from+(to-from)*ease;
    }
    bool Active(double now) const { return duration > 0 && now-started < duration; }
    void Snap(double offset) { from = to = offset; duration = 0; }
};

// No XAML or window-position operations run on this thread. It posts at most
// one pending frame, sleeps completely between slides, and is joined before
// the host is destroyed. No global timer-resolution change is needed.
class SearchSlideClock {
    HANDLE stop_ = nullptr, wake_ = nullptr, timer_ = nullptr;
    std::thread thread_;
    std::atomic<bool> active_{false};
    std::atomic<HWND> host_{nullptr};
    std::atomic<double> interval_{1000.0/60};
    void Run() {
        const HANDLE waits[]{stop_,wake_,timer_};
        double next = 0;
        for (;;) {
            const DWORD result = WaitForMultipleObjects(3,waits,FALSE,INFINITE);
            if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) return;
            if (result == WAIT_OBJECT_0+1) {
                CancelWaitableTimer(timer_);
                next = SearchMotionTime();
            }
            if (!active_.load()) continue;
            if (result == WAIT_OBJECT_0+2) {
                if (!pending.exchange(true) && !PostMessageW(host_.load(),kSearchWindowSlideFrameMessage,0,0))
                    pending.store(false);
                next += interval_.load();
            }
            const double now = SearchMotionTime();
            if (next < now) next = now;
            LARGE_INTEGER due{};
            due.QuadPart = -std::max<LONGLONG>(1,static_cast<LONGLONG>((next-now)*10000));
            if (!SetWaitableTimer(timer_,&due,0,nullptr,nullptr,FALSE)) {
                active_.store(false);
                PostMessageW(host_.load(),kSearchWindowSlideFrameMessage,1,0);
            }
        }
    }
public:
    std::atomic<bool> pending{false};
    SearchSlideClock() {
        stop_ = CreateEventW(nullptr,TRUE,FALSE,nullptr);
        wake_ = CreateEventW(nullptr,FALSE,FALSE,nullptr);
        // CREATE_WAITABLE_TIMER_HIGH_RESOLUTION (Windows 10 version 1803+).
        timer_ = CreateWaitableTimerExW(nullptr,nullptr,0x2,TIMER_ALL_ACCESS);
        if (!stop_ || !wake_ || !timer_) {
            if (stop_) CloseHandle(stop_);
            if (wake_) CloseHandle(wake_);
            if (timer_) CloseHandle(timer_);
            throw std::runtime_error("search animation clock unavailable");
        }
        try { thread_ = std::thread([this] { Run(); }); }
        catch (...) { CloseHandle(stop_); CloseHandle(wake_); CloseHandle(timer_); throw; }
    }
    ~SearchSlideClock() {
        SetEvent(stop_);
        if (thread_.joinable()) thread_.join();
        CloseHandle(timer_); CloseHandle(wake_); CloseHandle(stop_);
    }
    void Start(HWND host,double interval) {
        host_.store(host); interval_.store(interval); active_.store(true); SetEvent(wake_);
    }
    void Pause() { active_.store(false); SetEvent(wake_); }
};
[[clang::no_destroy]] std::optional<SearchSlideClock> g_searchSlideClock;
SearchWindowSlide g_searchWindowSlide;
double g_searchWindowSlideTarget = 0;
RECT g_searchWindowBounds{};
RECT g_searchWindowLastFrame{};
bool g_searchWindowClosing = false;
bool g_searchWindowCloaked = false;
bool g_searchWindowStartWasOpen = false;
bool g_searchWindowShellCloseAsked = false;
ULONGLONG g_searchWindowHandoffStarted = 0;
ULONGLONG g_searchWindowStartClosedAt = 0;

bool SearchWindowAnimationsEnabled() {
    BOOL enabled = TRUE;
    return !SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0) || enabled;
}

// Documented WinRT interface, omitted from the projected SDK headers.
struct SearchXamlTransparency : IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_IsBackgroundTransparent(boolean* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsBackgroundTransparent(boolean value) = 0;
};

bool SearchWindowHasActivePopup() try {
    if(g_contextMenuOpen) return true;
    if(g_resultsHost && g_resultsHost.XamlRoot()) {
        for(auto popup:wuxm::VisualTreeHelper::GetOpenPopupsForXamlRoot(g_resultsHost.XamlRoot()))
            if(popup!=g_previewPopup && popup.IsLightDismissEnabled()) return true;
    }
    return false;
} catch(...) { return true; }

constexpr UINT_PTR kSearchWindowRefocusTimer=0x704;
ULONGLONG g_searchWindowRefocusUntil=0;
bool g_searchWindowRefocusBodyClick=false;

bool SearchWindowTextboxHasFocus() {
    return g_ourBox && g_searchWindowSource &&
        SearchWindowHasNativeFocus(g_searchWindowWindow.load()) && g_searchWindowSource.HasFocus() &&
        wux::Input::FocusManager::GetFocusedElement(g_ourBox.XamlRoot()) == g_ourBox;
}

void TraceSearchWindowFocus(const wchar_t* reason) {
    wchar_t state[256]{};
    auto focused = g_ourBox ? wux::Input::FocusManager::GetFocusedElement(g_ourBox.XamlRoot()) : nullptr;
    swprintf_s(state, L"%ls; foreground=%p native=%p island=%d textbox=%d preview=%d", reason,
               GetForegroundWindow(), GetFocus(), g_searchWindowSource && g_searchWindowSource.HasFocus(),
               g_ourBox && focused == g_ourBox, g_previewText && focused == g_previewText);
    SearchWindowTrace(state);
}

void QueueSearchBoxRefocus(bool bodyClick) {
    HWND host=g_searchWindowWindow.load();
    if(!host || !g_ourBox || g_dragging || g_searchWindowClosing || !g_isOverlayVisible.load()) return;
    g_searchWindowRefocusBodyClick |= bodyClick;
    g_searchWindowRefocusUntil=GetTickCount64()+400;
    // The popup can still be finishing its close when Closed fires. Retry on
    // the UI thread instead of abandoning focus after one dispatcher callback.
    if(!g_contextMenuOpen) g_searchWindowInputReady=false;
    SetTimer(host,kSearchWindowRefocusTimer,16,nullptr);
}

void SearchWindowRefocusFrame() try {
    HWND host=g_searchWindowWindow.load();
    auto stop=[&] {
        if(host) KillTimer(host,kSearchWindowRefocusTimer);
        g_searchWindowRefocusUntil=0; g_searchWindowRefocusBodyClick=false;
        g_searchWindowMenuTyping=false; g_searchWindowMenuCaret=-1; g_searchWindowMenuSelection=0;
    };
    HWND foreground=GetForegroundWindow();
    // Modal dialogs and other windows on this thread keep their own focus.
    const bool ours=foreground == host && IsWindowEnabled(host);
    if(!host || !g_ourBox || !IsWindowVisible(host) || !ours || g_dragging ||
       g_searchWindowClosing || g_quit.load()) {
        stop(); return;
    }
    if(GetTickCount64()>g_searchWindowRefocusUntil) {
        TraceSearchWindowFocus(L"search focus retry expired");
        stop(); return;
    }
    if(SearchWindowHasActivePopup()) return;
    auto focused=wux::Input::FocusManager::GetFocusedElement(g_ourBox.XamlRoot());
    if(!g_searchWindowRefocusBodyClick && focused && focused==g_previewText) {
        g_searchWindowInputReady=SearchWindowHasNativeFocus(host); stop(); return;
    }
    // Logical XAML focus alone is insufficient after a popup: restore the
    // native island input window first, then its editable search control.
    const bool menuTyping=g_searchWindowMenuTyping.load() && g_searchWindowMenuCaret>=0;
    const int caret=menuTyping ? g_searchWindowMenuCaret : g_ourBox.SelectionStart();
    const int selection=menuTyping ? g_searchWindowMenuSelection : g_ourBox.SelectionLength();
    HWND nativeFocus=GetFocus();
    if((nativeFocus!=g_searchWindowChild && !IsChild(g_searchWindowChild,nativeFocus)) ||
       !g_searchWindowSource.HasFocus()) {
        SetFocus(g_searchWindowChild);
        g_searchWindowSource.NavigateFocus(winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationRequest{
            winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::First});
    }
    const bool requested=g_ourBox.Focus(wux::FocusState::Programmatic);
    // Focus() can succeed logically while the host still owns native input.
    // Keep buffering and retrying until both layers agree.
    if(requested) {
        g_ourBox.SelectionStart(caret); g_ourBox.SelectionLength(selection);
    }
    if(requested && SearchWindowTextboxHasFocus()) {
        g_searchWindowInputReady=true;
        SearchWindowTrace(L"search native and XAML focus restored");
        stop();
    }
} catch(...) {
    SearchWindowTrace(L"search focus restore failed",winrt::to_hresult());
    if(HWND host=g_searchWindowWindow.load()) KillTimer(host,kSearchWindowRefocusTimer);
    g_searchWindowRefocusUntil=0;
}

void CheckSearchWindowFocus() try {
    HWND host=g_searchWindowWindow.load();
    if(!host || !g_ourBox || !IsWindowVisible(host) || GetForegroundWindow()!=host ||
       !IsWindowEnabled(host) || !g_isOverlayVisible.load() || g_dragging ||
       g_searchWindowClosing || g_quit.load() || SearchWindowHasActivePopup()) return;
    auto focused=wux::Input::FocusManager::GetFocusedElement(g_ourBox.XamlRoot());
    // Selecting/copying a preview excerpt is deliberate keyboard focus.
    if(focused && focused==g_previewText) return;
    if(SearchWindowTextboxHasFocus()) return;
    if(!g_searchWindowRefocusUntil) {
        TraceSearchWindowFocus(L"search focus loss detected");
        QueueSearchBoxRefocus();
    }
} catch(...) {
    SearchWindowTrace(L"search focus check failed",winrt::to_hresult());
}


bool DismissSearchContextMenu() try {
    auto flyout=g_activeContextFlyout.get();
    if(!g_contextMenuOpen && !flyout) return false;
    if(flyout) flyout.Hide();
    else { g_contextMenuOpen=false; g_contextMenuClosedTick=GetTickCount64(); }
    QueueSearchBoxRefocus(true);
    return true;
} catch(...) {
    SearchWindowTrace(L"context menu dismiss failed",winrt::to_hresult());
    return true;
}

void SetSearchWindowCloaked(bool cloak) {
    HWND host = g_searchWindowWindow.load();
    if (!host || cloak == g_searchWindowCloaked) return;
    BOOL value = cloak;
    winrt::check_hresult(DwmSetWindowAttribute(host, DWMWA_CLOAK, &value, sizeof(value)));
    g_searchWindowCloaked = cloak;
}

void SearchWindowStyle() {
    const bool light = IsLightTheme();
    HWND host = g_searchWindowWindow.load();
    BOOL dark = !light;
    DwmSetWindowAttribute(host, 20, &dark, sizeof(dark));
    if (!g_resultsHost) return;
    g_resultsHost.RequestedTheme(light ? wux::ElementTheme::Light : wux::ElementTheme::Dark);
    g_resultsHost.Margin(wux::ThicknessHelper::FromUniformLength(0));
    g_resultsHost.Padding(wux::ThicknessHelper::FromLengths(16,16,16,12));
    g_resultsHost.CornerRadius(wux::CornerRadius{8,8,8,8});
    g_resultsHost.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
    g_resultsHost.BorderBrush(light ? MakeBrush(0x25,0,0,0) : MakeBrush(0x20,255,255,255));
    wuxm::AcrylicBrush surface;
    surface.BackgroundSource(wuxm::AcrylicBackgroundSource::HostBackdrop);
    surface.TintColor(light ? winrt::Windows::UI::Color{255,243,243,243} : winrt::Windows::UI::Color{255,32,32,32});
    surface.TintOpacity(light ? 0.80 : 0.78);
    surface.TintLuminosityOpacity(light ? 0.90 : 0.82);
    surface.FallbackColor(light ? winrt::Windows::UI::Color{255,243,243,243} : winrt::Windows::UI::Color{255,32,32,32});
    g_resultsHost.Background(surface);
    g_searchBarBorder.Height(Fs(40));
    g_searchBarBorder.Margin(wux::ThicknessHelper::FromLengths(0,0,0,16));
    g_searchBarBorder.Padding(wux::ThicknessHelper::FromLengths(12,0,12,0));
    g_searchBarBorder.CornerRadius(wux::CornerRadius{4,4,4,4});
    g_searchBarBorder.BorderThickness(wux::ThicknessHelper::FromUniformLength(1));
    g_searchBarBorder.BorderBrush(light ? MakeBrush(0x18,0,0,0) : MakeBrush(0x22,255,255,255));
    g_searchBarBorder.Background(light ? MakeBrush(0xCE,255,255,255) : MakeBrush(0x50,0,0,0));
    ScaleFont(g_ourBox,14);
    g_ourBox.PlaceholderText(L"Search Anything");
    g_ourBox.Padding(wux::ThicknessHelper::FromUniformLength(0));
    auto searchGrid = g_searchBarBorder.Child().as<wuxc::Grid>();
    auto searchIcon=searchGrid.Children().GetAt(0).as<wux::FrameworkElement>();
    searchIcon.Width(Fs(20)); searchIcon.Height(Fs(20));
    UpdateSearchIcon(light);
    if (g_divider) g_divider.Background(light ? MakeBrush(0x18,0,0,0) : MakeBrush(0x14,255,255,255));
    if (g_footerBorder) g_footerBorder.BorderBrush(light ? MakeBrush(0x18,0,0,0) : MakeBrush(0x14,255,255,255));
    g_searchAppearanceDirty = true;
    ApplyStartAppearance();
    ApplySearchTextColors();
}

uint32_t SearchPackColor(winrt::Windows::UI::Color c) {
    return (uint32_t(c.A)<<24)|(uint32_t(c.R)<<16)|(uint32_t(c.G)<<8)|c.B;
}
winrt::Windows::UI::Color SearchUnpackColor(uint32_t c) {
    return {static_cast<BYTE>(c>>24),static_cast<BYTE>(c>>16),static_cast<BYTE>(c>>8),static_cast<BYTE>(c)};
}

uint32_t SearchParseTextColor(std::wstring_view text) {
    if(text.size()!=7 || text[0]!=L'#') return 0;
    uint32_t color=0;
    for(size_t i=1;i<text.size();++i) {
        const wchar_t c=text[i];
        const int digit=c>=L'0' && c<=L'9' ? c-L'0' :
                        c>=L'a' && c<=L'f' ? c-L'a'+10 :
                        c>=L'A' && c<=L'F' ? c-L'A'+10 : -1;
        if(digit<0) return 0;
        color=(color<<4)|digit;
    }
    return color|0xFF000000;
}
double SearchLuminance(uint32_t color) {
    auto linear=[](uint32_t c) { double v=c/255.0; return v<=0.04045 ? v/12.92 : std::pow((v+0.055)/1.055,2.4); };
    return 0.2126*linear((color>>16)&255)+0.7152*linear((color>>8)&255)+0.0722*linear(color&255);
}
std::optional<uint32_t> SearchBackgroundColor(SearchBrushData const& brush,std::optional<uint32_t> backdrop=std::nullopt) {
    // Transparent glass depends on whatever is behind the window. Keep the
    // copied text there; a user override covers deliberately unusual themes.
    if(brush.kind==2 && (brush.fallback>>24)) return brush.fallback;
    if(brush.kind==1 && backdrop) {
        const double alpha=std::clamp((brush.color>>24)/255.0*brush.opacity,0.0,1.0);
        uint32_t color=0xFF000000;
        for(int shift:{0,8,16}) color|=static_cast<uint32_t>(std::lround(
            ((brush.color>>shift)&255)*alpha+((*backdrop>>shift)&255)*(1-alpha)))<<shift;
        return color;
    }
    if((brush.kind==1 || brush.kind==4) && (brush.color>>24)*brush.opacity>=128) return brush.color;
    return std::nullopt;
}
uint32_t SearchChooseTextColor(SearchBrushData const& background,uint32_t copied,bool dark,uint32_t overrideColor,
                             std::optional<uint32_t> backdrop=std::nullopt) {
    if(overrideColor) return overrideColor;
    uint32_t color=(copied>>24) ? copied|0xFF000000 : dark ? 0xFFFFFFFF : 0xFF202020;
    if(auto surface=SearchBackgroundColor(background,backdrop)) {
        const double bg=SearchLuminance(*surface),fg=SearchLuminance(color);
        if((std::max(bg,fg)+0.05)/(std::min(bg,fg)+0.05)<4.5)
            color=(bg+0.05)/0.05 >= 1.05/(bg+0.05) ? 0xFF000000 : 0xFFFFFFFF;
    }
    return color;
}
uint32_t SearchTextOverride() {
    HIGHCONTRASTW contrast{sizeof(contrast)};
    if(SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(contrast),&contrast,0) && (contrast.dwFlags&HCF_HIGHCONTRASTON)) {
        const COLORREF color=GetSysColor(COLOR_WINDOWTEXT);
        return 0xFF000000|(GetRValue(color)<<16)|(GetGValue(color)<<8)|GetBValue(color);
    }
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return SearchParseTextColor(g_settings.textColor);
}
uint32_t SearchMainTextColor() {
    return SearchChooseTextColor(g_searchAppearance.background,g_searchAppearance.foreground,
                                g_searchAppearanceValid ? g_searchAppearance.dark!=0 : !IsLightTheme(),SearchTextOverride());
}
void ApplySearchTextColors() try {
    if(!g_resultsHost || !g_ourBox) return;
    // Theme/settings notifications and copied-appearance changes invalidate
    // these UI-thread caches. Ordinary result rows reuse the resolved brush.
    g_resolvedSearchTextBrush = nullptr;
    ResetSearchContainerContrast();
    const uint32_t main=SearchMainTextColor();
    const uint32_t search=SearchChooseTextColor(g_searchAppearance.searchBackground,
        g_searchAppearance.searchForeground ? g_searchAppearance.searchForeground : main,
        SearchLuminance(main)>0.5,SearchTextOverride(),SearchBackgroundColor(g_searchAppearance.background));
    if(!g_searchTextBrush) g_searchTextBrush=MakeBrush(255,255,255,255);
    g_searchTextBrush.Color(SearchUnpackColor(main));
    if(!g_searchBoxTextBrush) g_searchBoxTextBrush=MakeBrush(255,255,255,255);
    if(!g_searchPlaceholderBrush) g_searchPlaceholderBrush=MakeBrush(255,255,255,255);
    g_searchBoxTextBrush.Color(SearchUnpackColor(search));
    g_searchPlaceholderBrush.Color(SearchUnpackColor(search));
    g_searchPlaceholderBrush.Opacity(0.65);
    wuxm::Brush foreground=g_searchBoxTextBrush;
    g_ourBox.Foreground(foreground);
    g_ourBox.PlaceholderForeground(g_searchPlaceholderBrush);
    // Focus/hover visual states use these resources instead of Foreground.
    for(auto key:{L"TextControlForeground",L"TextControlForegroundFocused",L"TextControlForegroundPointerOver",
                  L"TextControlButtonForeground",L"TextControlButtonForegroundPointerOver",L"TextControlButtonForegroundPressed"})
        g_ourBox.Resources().Insert(winrt::box_value(key),foreground);
    auto textBrush=SearchTextBrush();
    RefreshSearchIcons(textBrush);
    auto apply=[&](auto&& self,wux::DependencyObject const& node,int depth)->void {
        if(!node || depth>32) return;
        if(node==g_ourBox) return; // retain the field's own placeholder/focus brushes
        if(auto text=node.try_as<wuxc::TextBlock>()) text.Foreground(textBrush);
        const int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
        for(int i=0;i<count;++i) self(self,wuxm::VisualTreeHelper::GetChild(node,i),depth+1);
    };
    apply(apply,g_resultsHost,0);
    if(g_previewCard) apply(apply,g_previewCard,0);
    const bool light=SearchLuminance(main)<0.5;
    auto theme=light ? wux::ElementTheme::Light : wux::ElementTheme::Dark;
    g_resultsHost.RequestedTheme(theme);
    g_searchBarBorder.RequestedTheme(SearchLuminance(search)<0.5 ? wux::ElementTheme::Light : wux::ElementTheme::Dark);
    g_cachedLightTheme.store(light);
    UpdateSearchIcon(SearchLuminance(search)<0.5);
    if(g_previewCard) {
        g_previewCard.RequestedTheme(theme);
        g_previewCard.Background(g_resultsHost.Background());
        g_previewCard.BorderBrush(light ? MakeBrush(0x24,0,0,0) : MakeBrush(0x30,255,255,255));
        g_previewThumbBox.Background(light ? MakeBrush(0x0C,0,0,0) : MakeBrush(0x10,255,255,255));
    }
    SearchContainerContrastTick();
} catch(...) { SearchWindowTrace(L"text appearance failed",winrt::to_hresult()); }

// Surface rules from the Styler bundled themes; user overrides take precedence.
struct SearchThemeRule { PCWSTR theme; int role; PCWSTR style; };
static const SearchThemeRule kSearchThemeRules[] = {
    {L"TranslucentStartMenu",-1,L"CommonBgBrush=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#25323232\"/>"},
    {L"TranslucentStartMenu",0,L"Background:=$CommonBgBrush"},
    {L"TranslucentStartMenu",1,L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>"},
    {L"TranslucentStartMenu",3,L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#22000000\"/>"},
    {L"TranslucentStartMenu",3,L"BorderThickness=1"},
    {L"TranslucentStartMenu_variant_ClassicStartMenu",-1,L"CommonBgBrush=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#25323232\"/>"},
    {L"TranslucentStartMenu_variant_ClassicStartMenu",0,L"Background:=$CommonBgBrush"},
    {L"TranslucentStartMenu_variant_ClassicStartMenu",1,L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#15000000\"/>"},
    {L"TranslucentStartMenu_variant_ClassicStartMenu",3,L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#00000000\"/>"},
    {L"TranslucentStartMenu_variant_ClassicStartMenu",3,L"BorderThickness=0"},
    {L"LiquidGlass2",-1,L"Glass=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.1\" />"},
    {L"LiquidGlass2",-1,L"Frosted=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.3\" />"},
    {L"LiquidGlass2",-1,L"Acrylic=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"},
    {L"LiquidGlass2",-1,L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>"},
    {L"LiquidGlass2",-1,L"BorderBrush2=<WindhawkBlur BlurAmount=\"10\" TintColor=\"#909090\" TintOpacity=\"0.3\"/>"},
    {L"LiquidGlass2",-1,L"ClockBG=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAccentColorLight2}\" TintOpacity=\"0.3\" />"},
    {L"LiquidGlass2",-1,L"BorderThickness=0.3,1,0.3,1"},
    {L"LiquidGlass2",-1,L"CornerRadius=25"},
    {L"LiquidGlass2",-1,L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.30\" />"},
    {L"LiquidGlass2",-1,L"ElementBorderThickness=0.3,0.5,0.3,1"},
    {L"LiquidGlass2",-1,L"ElementCornerRadius=20"},
    {L"LiquidGlass2",-1,L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>"},
    {L"LiquidGlass2",-1,L"ElementBorderBrush2=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#909090\" TintOpacity=\"0.3\"/>"},
    {L"LiquidGlass2",-1,L"GlassDark=<WindhawkBlur BlurAmount=\"5\" TintColor=\"#1A000000\" />"},
    {L"LiquidGlass2",-1,L"GlassDark2=<WindhawkBlur BlurAmount=\"5\" TintColor=\"#0D000000\" />"},
    {L"LiquidGlass2",-1,L"GlassDarkTitles=<WindhawkBlur BlurAmount=\"7\" TintColor=\"#10151515\"/>"},
    {L"LiquidGlass2",-1,L"GlassDarkBottomTray=<WindhawkBlur BlurAmount=\"14\" TintColor=\"#3E4A4A4A\"/>"},
    {L"LiquidGlass2",-1,L"fluidBorder=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Offset=\"0\" Color=\"#2FFFFFFF\" /><GradientStop Offset=\"0.2\" Color=\"#1AFFFFFF\" /><GradientStop Offset=\"0.65\" Color=\"#1AF0F0F0\" /><GradientStop Offset=\"1\" Color=\"#2F707070\" /></LinearGradientBrush>"},
    {L"LiquidGlass2",-1,L"fluidNormal=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />"},
    {L"LiquidGlass2",-1,L"fluidHover=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />"},
    {L"LiquidGlass2",-1,L"fluidPressed=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />"},
    {L"LiquidGlass2",-1,L"GlassDarkSearchPage=<WindhawkBlur BlurAmount=\"4\" TintColor=\"#1F000000\"/>"},
    {L"LiquidGlass2",0,L"Background:=$GlassDark"},
    {L"LiquidGlass2",0,L"Background:=$GlassDark"},
    {L"LiquidGlass2",1,L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#6F000000\" Offset=\"0.0\" /><GradientStop Color=\"#40000000\" Offset=\"0.3\" /><GradientStop Color=\"#30000000\" Offset=\"0.41\" /><GradientStop Color=\"#10000000\" Offset=\"0.6\" /><GradientStop Color=\"#00000000\" Offset=\"0.8\" /><GradientStop Color=\"#00000000\" Offset=\"1\" /></LinearGradientBrush>"},
    {L"LiquidGlass2",2,L"Background:=Transparent"},
    {L"LiquidGlass2",2,L"CornerRadius=40"},
    {L"LiquidGlass2",2,L"BorderThickness=0"},
    {L"LiquidGlass2",3,L"BorderBrush:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#70D3D3D3\" Offset=\"0.0\" /><GradientStop Color=\"#60CDCDCD\" Offset=\"0.25\" /><GradientStop Color=\"#40CDCDCD\" Offset=\"0.4\" /><GradientStop Color=\"#40696969\" Offset=\"0.65\" /><GradientStop Color=\"#553C3C3C\" Offset=\"1\" /></LinearGradientBrush>"},
    {L"LiquidGlass2",3,L"Background:=$GlassDark"},
    {L"LiquidGlass2",3,L"CornerRadius=40"},
    {L"LiquidGlass2",3,L"BorderThickness=1,1,1,1"},
    {L"LiquidGlass2",3,L"Padding=8,6,8,7"},
    {L"LiquidGlass2",4,L"CornerRadius=11"},
    {L"LiquidGlass2",4,L"Margin=2,3,2,3"},
    {L"LiquidGlass",-1,L"BorderThickness=0.3,1,0.3,0.3"},
    {L"LiquidGlass",-1,L"ElementBorderThickness=0.3,0.3,0.3,1"},
    {L"LiquidGlass",-1,L"CornerRadius=12"},
    {L"LiquidGlass",-1,L"ElementCornerRadius=8"},
    {L"LiquidGlass",-1,L"Background=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />"},
    {L"LiquidGlass",-1,L"ElementBackground=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.4\" />"},
    {L"LiquidGlass",-1,L"ElementBackground2 = <WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\"  />"},
    {L"LiquidGlass",-1,L"AccentBackground=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAccentColorLight1}\" TintOpacity=\"0.2\"  />"},
    {L"LiquidGlass",-1,L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>\""},
    {L"LiquidGlass",-1,L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>"},
    {L"LiquidGlass",0,L"Background:=$Background"},
    {L"LiquidGlass",1,L"Background@Checked:=$ElementBackground"},
    {L"LiquidGlass",1,L"Background@CheckedPointerOver:=$AccentBackground"},
    {L"LiquidGlass",1,L"Background@CheckedPressed:=$ElementBackground2"},
    {L"LiquidGlass",1,L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"},
    {L"LiquidGlass",3,L"BorderThickness=$BorderThickness"},
    {L"LiquidGlass",3,L"BorderBrush:=$BorderBrush"},
    {L"LiquidGlass",3,L"CornerRadius=$CornerRadius"},
    {L"TintedGlass",-1,L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>"},
    {L"TintedGlass",0,L"Background:=$CommonBgBrush"},
    {L"TintedGlass",1,L"Background:=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#1AFFFFFF\"/>"},
    {L"TintedGlass",3,L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#22000000\"/>"},
    {L"TintedGlass",3,L"BorderThickness=1"},
    {L"TintedGlass_variant_ClassicStartMenu",-1,L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>"},
    {L"TintedGlass_variant_ClassicStartMenu",0,L"Background:=$CommonBgBrush"},
    {L"TintedGlass_variant_ClassicStartMenu",1,L"Background:=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#1AFFFFFF\"/>"},
    {L"TintedGlass_variant_ClassicStartMenu",3,L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#00000000\"/>"},
    {L"TintedGlass_variant_ClassicStartMenu",3,L"BorderThickness=0"},
};

std::wstring SearchTrim(std::wstring value) {
    const auto first=value.find_first_not_of(L" \r\n\t");
    return first==std::wstring::npos ? std::wstring() : value.substr(first,value.find_last_not_of(L" \r\n\t")-first+1);
}
std::wstring SearchStylerSetting(HKEY key,std::wstring const& name) {
    wchar_t text[4096]{}; DWORD type=0,size=sizeof(text);
    if (RegQueryValueExW(key,name.c_str(),nullptr,&type,reinterpret_cast<BYTE*>(text),&size)!=ERROR_SUCCESS ||
        type!=REG_SZ || size>sizeof(text) || size<sizeof(wchar_t)) return {};
    text[ARRAYSIZE(text)-1]=0; return text;
}
std::wstring SearchXmlAttribute(std::wstring const& xml,std::wstring_view name) {
    std::wstring needle=std::wstring(name)+L"=\"";
    size_t begin=xml.find(needle); if(begin==std::wstring::npos) return {};
    begin+=needle.size(); size_t end=xml.find(L'\"',begin);
    return end==std::wstring::npos ? std::wstring() : xml.substr(begin,end-begin);
}
using SearchStyleProperties=std::map<std::wstring,std::wstring>;
struct SearchStyleConfig {
    std::map<std::wstring,std::wstring> constants;
    SearchStyleProperties roles[5]; // main, search bar, menu, menu border, item
    void Add(int role,std::wstring text) {
        size_t equal=text.find(L'='); if(equal==std::wstring::npos) return;
        auto name=SearchTrim(text.substr(0,equal)); auto value=SearchTrim(text.substr(equal+1));
        if(role<0) { constants[name]=value; return; }
        if(name.ends_with(L":")) name.pop_back();
        // Stateful rules belong to the Styler; retain native item interaction.
        if(name.find(L'@')==std::wstring::npos) roles[role][name]=value;
    }
    std::wstring Value(int role,PCWSTR property) const {
        auto it=roles[role].find(property); if(it==roles[role].end()) return {};
        auto value=it->second;
        for(int i=0;i<8 && value.starts_with(L"$");++i) {
            auto c=constants.find(value.substr(1));
            if(c==constants.end()) return {}; value=SearchTrim(c->second);
        }
        if(value.starts_with(L"$") || value.find(L"{{")!=std::wstring::npos) return {};
        return value;
    }
};
int SearchStyleRole(std::wstring const& target) {
    if(target.ends_with(L"#AcrylicBorder")) return 0;
    if(target==L"Border#BorderElement" ||
       (target.find(L"SearchBoxToggleButton")!=std::wstring::npos && target.ends_with(L"#BorderElement"))) return 1;
    if(target==L"MenuFlyoutPresenter" || target==L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter") return 2;
    if(target==L"MenuFlyoutPresenter > Border" || target==L"MenuFlyoutPresenter > Border#BackgroundElement" ||
       target==L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border") return 3;
    if(target==L"MenuFlyoutItem" || target==L"Windows.UI.Xaml.Controls.MenuFlyoutItem") return 4;
    return -1;
}
SearchStyleConfig ReadSearchStyleConfig() {
    SearchStyleConfig config;
    HKEY mod=nullptr;
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Windhawk\\Engine\\Mods\\windows-11-start-menu-styler",0,KEY_READ,&mod)!=ERROR_SUCCESS) return config;
    struct Close {HKEY key; ~Close(){if(key)RegCloseKey(key);}} closeMod{mod};
    DWORD disabled=0,size=sizeof(disabled);
    if(RegQueryValueExW(mod,L"Disabled",nullptr,nullptr,reinterpret_cast<BYTE*>(&disabled),&size)==ERROR_SUCCESS && disabled) return config;
    HKEY key=nullptr; if(RegOpenKeyExW(mod,L"Settings",0,KEY_READ,&key)!=ERROR_SUCCESS) return config;
    Close close{key};
    auto theme=SearchStylerSetting(key,L"theme");
    for(auto const& rule:kSearchThemeRules) if(theme==rule.theme) config.Add(rule.role,rule.style);
    for(int i=0;i<256;++i) {
        auto value=SearchStylerSetting(key,L"styleConstants["+std::to_wstring(i)+L"]");
        if(value.empty()) break; config.Add(-1,value);
    }
    for(int i=0;i<256;++i) {
        const auto prefix=L"controlStyles["+std::to_wstring(i)+L"]";
        auto target=SearchStylerSetting(key,prefix+L".target"); if(target.empty()) break;
        const int role=SearchStyleRole(SearchTrim(target)); if(role<0) continue;
        for(int j=0;j<128;++j) {
            auto value=SearchStylerSetting(key,prefix+L".styles["+std::to_wstring(j)+L"]");
            if(value.empty()) break; config.Add(role,value);
        }
    }
    return config;
}

SearchBrushData CaptureSearchBrush(wuxm::Brush const& brush) {
    SearchBrushData data; if(!brush) return data; data.opacity=brush.Opacity();
    if(auto solid=brush.try_as<wuxm::SolidColorBrush>()) {
        data.kind=1; data.color=SearchPackColor(solid.Color());
    } else if(auto acrylic=brush.try_as<wuxm::AcrylicBrush>()) {
        data.kind=2; data.color=SearchPackColor(acrylic.TintColor());
        data.fallback=SearchPackColor(acrylic.FallbackColor()); data.tint=acrylic.TintOpacity();
        if(auto luminosity=acrylic.TintLuminosityOpacity()) data.luminosity=luminosity.Value();
    } else if(auto gradient=brush.try_as<wuxm::LinearGradientBrush>()) {
        data.kind=3; data.count=std::min<uint32_t>(8,gradient.GradientStops().Size());
        auto start=gradient.StartPoint(),end=gradient.EndPoint();
        data.points[0]=start.X; data.points[1]=start.Y; data.points[2]=end.X; data.points[3]=end.Y;
        for(uint32_t i=0;i<data.count;++i) {
            auto stop=gradient.GradientStops().GetAt(i); data.colors[i]=SearchPackColor(stop.Color()); data.offsets[i]=stop.Offset();
        }
    }
    return data;
}

// Attach a short-lived brush resource so ThemeResource resolves in Start's theme.
wuxm::Brush SearchReadBrush(std::wstring value,wux::FrameworkElement const& context) {
    value=SearchTrim(value);
    if(value.ends_with(L">\"")) value.pop_back(); // tolerate trailing quote in legacy theme
    if(value.empty()) return nullptr;
    if(value[0]!=L'<') value=L"<SolidColorBrush Color=\""+value+L"\"/>";
    if(!value.starts_with(L"<SolidColorBrush ") && !value.starts_with(L"<AcrylicBrush ") &&
       !value.starts_with(L"<LinearGradientBrush ")) return nullptr;
    value.insert(value.find(L' '),L" xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\"");
    auto brush=wux::Markup::XamlReader::Load(value).try_as<wuxm::Brush>();
    if(brush && context) {
        auto key=winrt::box_value(L"__StartEverything_ThemeProbe");
        auto resources=context.Resources();
        auto previous=resources.HasKey(key) ? resources.Lookup(key) : nullptr;
        resources.Insert(key,brush);
        if(previous) resources.Insert(key,previous); else resources.Remove(key);
    }
    return brush;
}
SearchBrushData SearchConfiguredBrush(std::wstring const& value,wux::FrameworkElement const& context) noexcept {
    try {
        if(!value.starts_with(L"<WindhawkBlur ")) return CaptureSearchBrush(SearchReadBrush(value,context));
        SearchBrushData data;
        auto color=SearchReadBrush(SearchXmlAttribute(value,L"TintColor"),context).try_as<wuxm::SolidColorBrush>();
        if(!color) return data;
        data.kind=4; data.color=SearchPackColor(color.Color()); data.fallback=data.color|0xFF000000;
        data.blur=3; data.luminosity=0;
        auto number=[&](PCWSTR name,double& out,double lo,double hi) {
            auto text=SearchXmlAttribute(value,name); if(text.empty()) return;
            size_t read=0; double parsed=std::stod(text,&read);
            if(read!=text.size() || !std::isfinite(parsed)) throw std::invalid_argument("glass parameter");
            out=std::clamp(parsed,lo,hi);
        };
        double alpha=(data.color>>24)/255.0;
        number(L"TintOpacity",alpha,0,1); data.color=(data.color&0xFFFFFF)|(static_cast<uint32_t>(std::lround(alpha*255))<<24);
        number(L"BlurAmount",data.blur,0,100); number(L"TintLuminosityOpacity",data.luminosity,0,1);
        number(L"TintSaturation",data.saturation,0,10); number(L"Opacity",data.opacity,0,1);
        auto fallback=SearchXmlAttribute(value,L"FallbackColor");
        if(!fallback.empty()) if(auto brush=SearchReadBrush(fallback,context).try_as<wuxm::SolidColorBrush>()) data.fallback=SearchPackColor(brush.Color());
        return data;
    } catch(...) { return {}; }
}
bool SearchParseFour(std::wstring const& text,double (&values)[4]) {
    if(text.empty()) return false;
    std::vector<double> parts; const wchar_t* p=text.c_str();
    while(*p && parts.size()<4) {
        wchar_t* end=nullptr; double value=wcstod(p,&end);
        if(end==p || !std::isfinite(value) || value<0 || value>200) return false;
        parts.push_back(value); p=end; while(iswspace(*p))++p;
        if(*p!=L',') break; ++p;
    }
    if(*p || (parts.size()!=1 && parts.size()!=4)) return false;
    for(size_t i=0;i<4;++i) values[i]=parts[parts.size()==1 ? 0 : i];
    return true;
}
SearchSurfaceData SearchConfiguredSurface(SearchStyleConfig const& config,int role,wux::FrameworkElement const& context) {
    SearchSurfaceData data;
    auto foreground=SearchConfiguredBrush(config.Value(role,L"Foreground"),context);
    if(foreground.kind==1 && (foreground.color>>24)) { data.foreground=foreground.color; data.flags|=64; }
    data.background=SearchConfiguredBrush(config.Value(role,L"Background"),context);
    data.outline=SearchConfiguredBrush(config.Value(role,L"BorderBrush"),context);
    if(data.background.kind) data.flags|=1;
    if(data.outline.kind) data.flags|=2;
    if(SearchParseFour(config.Value(role,L"CornerRadius"),data.corners)) data.flags|=4;
    if(SearchParseFour(config.Value(role,L"BorderThickness"),data.thickness)) data.flags|=8;
    if(SearchParseFour(config.Value(role,L"Padding"),data.padding)) data.flags|=16;
    if(SearchParseFour(config.Value(role,L"Margin"),data.margin)) data.flags|=32;
    return data;
}

void CaptureStartAppearance(HWND target) try {
    DWORD pid=0,shellPid=0;
    GetWindowThreadProcessId(target,&pid);
    GetWindowThreadProcessId(FindWindowW(L"Shell_TrayWnd",nullptr),&shellPid);
    wchar_t cls[128]{};
    GetClassNameW(target,cls,ARRAYSIZE(cls));
    if (!pid || pid != shellPid || wcscmp(cls,kSearchWindowClass) != 0) return;
    auto window = wux::Window::Current();
    if (!window || !window.Content()) return;
    auto content = window.Content();
    auto found = FindDescendantByName(content,L"AcrylicBorder",16);
    auto border = found ? found.try_as<wuxc::Border>() : nullptr;
    if (!border) return;
    if (border.ActualWidth() < 400 || border.ActualHeight() < 350) {
        if (auto element = content.try_as<wux::FrameworkElement>()) element.UpdateLayout();
    }
    SearchAppearanceData appearance;
    appearance.dark = border.ActualTheme() == wux::ElementTheme::Dark;
    appearance.background = CaptureSearchBrush(border.Background());
    appearance.outline = CaptureSearchBrush(border.BorderBrush());
    // Read rendered labels so custom TextBlock Foreground rules are included.
    auto textColor=[](wux::DependencyObject const& root,std::initializer_list<PCWSTR> names) {
        for(auto name:names) if(auto node=FindDescendantByName(root,name,20)) {
            if(auto text=node.try_as<wuxc::TextBlock>()) {
                if(auto brush=text.Foreground().try_as<wuxm::SolidColorBrush>())
                    return SearchPackColor(brush.Color());
            }
        }
        return uint32_t{0};
    };
    appearance.foreground=textColor(content,{L"DisplayName",L"AppDisplayName",L"PinnedListHeaderText",L"AllAppsHeading"});
    auto corners = border.CornerRadius();
    appearance.corners[0]=corners.TopLeft; appearance.corners[1]=corners.TopRight;
    appearance.corners[2]=corners.BottomRight; appearance.corners[3]=corners.BottomLeft;
    auto thickness = border.BorderThickness();
    appearance.thickness[0]=thickness.Left; appearance.thickness[1]=thickness.Top;
    appearance.thickness[2]=thickness.Right; appearance.thickness[3]=thickness.Bottom;
    if (auto toggle = FindStockSearchToggle(content)) {
        appearance.searchForeground=textColor(toggle,{L"PlaceholderText",L"Text",L"PlaceholderTextContentPresenter"});
        if (auto searchFound = FindDescendantByName(toggle,L"BorderElement",8)) {
            if (auto searchBorder = searchFound.try_as<wuxc::Border>()) {
                appearance.searchBackground = CaptureSearchBrush(searchBorder.Background());
                appearance.searchOutline = CaptureSearchBrush(searchBorder.BorderBrush());
                auto c = searchBorder.CornerRadius(); auto t = searchBorder.BorderThickness();
                appearance.searchCorners[0]=c.TopLeft; appearance.searchCorners[1]=c.TopRight;
                appearance.searchCorners[2]=c.BottomRight; appearance.searchCorners[3]=c.BottomLeft;
                appearance.searchThickness[0]=t.Left; appearance.searchThickness[1]=t.Top;
                appearance.searchThickness[2]=t.Right; appearance.searchThickness[3]=t.Bottom;
            }
        }
    }
    // Read settings once per request, but parse brush markup only when a value
    // or resolved theme changes. No XAML objects survive the native capture.
    auto config=ReadSearchStyleConfig();
    std::wstring signature=appearance.dark ? L"dark" : L"light";
    winrt::Windows::UI::ViewManagement::UISettings uiSettings;
    signature+=std::to_wstring(SearchPackColor(uiSettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent)));
    static const PCWSTR properties[]{L"Background",L"BorderBrush",L"CornerRadius",L"BorderThickness",L"Padding",L"Margin",L"Foreground"};
    for(int role=0;role<5;++role) for(auto property:properties) {
        auto value=config.Value(role,property);
        signature+=L"|"+std::to_wstring(value.size())+L":"+value;
    }
    static thread_local std::wstring previousSignature;
    static thread_local std::array<SearchSurfaceData,5> surfaces;
    if(signature!=previousSignature) {
        std::array<SearchSurfaceData,5> updated;
        for(int role=0;role<5;++role) updated[role]=SearchConfiguredSurface(config,role,border);
        surfaces=updated; previousSignature=std::move(signature);
    }
    // Some stock AcrylicBorder getters return the original acrylic even after
    // Styler replaces the rendered brush. Explicit glass rules take precedence.
    if(surfaces[0].background.kind==4) appearance.background=surfaces[0].background;
    if(surfaces[1].background.kind==4) appearance.searchBackground=surfaces[1].background;
    appearance.menu=surfaces[2]; appearance.menuBorder=surfaces[3]; appearance.menuItem=surfaces[4];
    HWND start=CachedStartWindow(); POINT client{}; MONITORINFO monitor{sizeof(monitor)};
    const double width=border.ActualWidth(),height=border.ActualHeight();
    if (width >= 400 && height >= 350 && ClientToScreen(start,&client) &&
        GetMonitorInfoW(MonitorFromWindow(start,MONITOR_DEFAULTTONEAREST),&monitor)) {
        // Arrange offsets describe the settled layout. TransformToVisual also
        // includes Start's opening/closing translation, which would erase the
        // bottom gap when search is opened while native Start is hidden.
        wf::Point origin{};
        wux::DependencyObject node=border;
        for(int depth=0;node && node!=content && depth<32;++depth) {
            if(auto element=node.try_as<wux::UIElement>()) {
                auto offset=element.ActualOffset(); origin.X+=offset.x; origin.Y+=offset.y;
            }
            node=wuxm::VisualTreeHelper::GetParent(node);
        }
        const double scale=GetDpiForWindow(start)/96.0;
        const double workWidth=(monitor.rcWork.right-monitor.rcWork.left)/scale;
        if (node==content && width <= workWidth && height*scale <= monitor.rcWork.bottom-monitor.rcWork.top) {
            appearance.width=width; appearance.height=height;
            const auto edge = TaskbarEdgeForMonitor(MonitorFromWindow(start,MONITOR_DEFAULTTONEAREST));
            appearance.taskbarEdge = static_cast<uint32_t>(edge);
            const double left = (client.x-monitor.rcWork.left)/scale+origin.X;
            const double top = (client.y-monitor.rcWork.top)/scale+origin.Y;
            const double workHeight = (monitor.rcWork.bottom-monitor.rcWork.top)/scale;
            const bool vertical = edge==SearchTaskbarEdge::Left || edge==SearchTaskbarEdge::Right;
            const double space = vertical ? workHeight-height : workWidth-width;
            appearance.alignment = space>0 ? std::clamp((vertical ? top : left)/space,0.0,1.0) : 0.5;
            const double gap = edge==SearchTaskbarEdge::Left ? left : edge==SearchTaskbarEdge::Top ? top :
                edge==SearchTaskbarEdge::Right ? workWidth-left-width : workHeight-top-height;
            appearance.edgeGap = std::max(0.0,gap);
        }
    }
    COPYDATASTRUCT data{kSearchWindowAppearanceData,sizeof(appearance),&appearance};
    DWORD_PTR received=0;
    SendMessageTimeoutW(target,WM_COPYDATA,reinterpret_cast<WPARAM>(start),reinterpret_cast<LPARAM>(&data),SMTO_ABORTIFHUNG,150,&received);
} catch (...) { Wh_Log(L"search appearance capture failed: %08X",static_cast<unsigned>(winrt::to_hresult())); }

void RequestStartAppearance() {
    HWND start=CachedStartWindow(),host=g_searchWindowWindow.load();
    if (!start || !host) return;
    if (g_searchAppearanceValid) {
        // Reuse measured placement immediately. A busy Start process must not
        // hold up our UI thread on every opening or focus-check tick.
        PostMessageW(start,SearchWindowAppearanceMessage(),reinterpret_cast<WPARAM>(host),0);
        return;
    }
    // The first opening still obtains measured bounds before positioning, so
    // different entry points don't initially open at different sizes.
    DWORD_PTR done=0;
    SendMessageTimeoutW(start,SearchWindowAppearanceMessage(),reinterpret_cast<WPARAM>(host),0,SMTO_ABORTIFHUNG,180,&done);
}

// Small Direct2D effect graph for Styler glass: blur, optional color adjustment,
// then a source-over tint. Unlike AcrylicBrush, black retains its specified alpha.
// Uses the public IGraphicsEffectD2D1Interop ABI (SDK header absent in MinGW).
struct SearchEffectInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR, UINT*, UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT, winrt::impl::abi_t<wf::IPropertyValue>**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT, IUnknown**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT*) = 0;
};

} // namespace
template <> inline constexpr winrt::guid winrt::impl::guid_v<SearchEffectInterop>{
    0x2FC57384,0xA068,0x44D7,{0xA3,0x31,0x30,0x98,0x2F,0xCF,0x71,0x77}};
namespace {
namespace searchcomp = winrt::Windows::UI::Composition;
namespace searcheffect = winrt::Windows::Graphics::Effects;

struct SearchEffect : winrt::implements<SearchEffect,searcheffect::IGraphicsEffect,
                                       searcheffect::IGraphicsEffectSource,SearchEffectInterop> {
    GUID id{};
    winrt::hstring name;
    std::vector<wf::IInspectable> properties;
    std::vector<searcheffect::IGraphicsEffectSource> sources;
    SearchEffect(GUID value) : id(value) {}
    winrt::hstring Name() const { return name; }
    void Name(winrt::hstring value) { name=std::move(value); }
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* value) noexcept override {
        if (!value) return E_POINTER; *value=id; return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR,UINT*,UINT*) noexcept override { return E_INVALIDARG; }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* value) noexcept override {
        if (!value) return E_POINTER; *value=static_cast<UINT>(properties.size()); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index,winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) return E_POINTER; *value=nullptr;
        if (index>=properties.size()) return E_BOUNDS;
        winrt::copy_to_abi(properties[index].as<wf::IPropertyValue>(),*reinterpret_cast<void**>(value)); return S_OK;
    } catch(...) { return winrt::to_hresult(); }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* value) noexcept override {
        if (!value) return E_POINTER; *value=static_cast<UINT>(sources.size()); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index,IUnknown** value) noexcept override {
        if (!value) return E_POINTER; *value=nullptr;
        if (index>=sources.size()) return E_BOUNDS;
        winrt::copy_to_abi(sources[index],*reinterpret_cast<void**>(value)); return S_OK;
    }
};

// Container contrast sampling. One small in-memory snapshot per update, with
// decisions made only from empty container padding. No XAML or disk I/O runs on
// the worker, and it sleeps completely while search is hidden or animating.
constexpr UINT_PTR kSearchContainerContrastTimer = 0x706;
constexpr UINT kSearchContainerContrastReady = WM_APP + 0x77;
constexpr size_t kSearchContrastMaxContainers = 64;
constexpr int kSearchContrastStripWidth = 3;

bool SearchContainerLightText(double luminance, std::optional<bool> previous = std::nullopt) {
    if (!std::isfinite(luminance)) return previous.value_or(true);
    // Retain polarity only inside the range where either side can still reach
    // 4.5:1. Outside it, even pure white or black would be too faint.
    if (previous) return *previous ? luminance <= 1.05/4.5-.05 : luminance < .05*4.5-.05;
    return luminance < 0.17912878;
}

std::array<double,256> const& SearchContrastLinearColors() {
    static const auto values = [] {
        std::array<double,256> result{};
        for (int i = 0; i < 256; ++i) {
            const double value = i/255.0;
            result[i] = value <= .04045 ? value/12.92 : std::pow((value+.055)/1.055,2.4);
        }
        return result;
    }();
    return values;
}

constexpr double SearchReadableTextOpacity(double requested) {
    return requested > 0 ? 1.0 : requested;
}

constexpr uint32_t SearchContrastPreferredColor(bool light) {
    return light ? 0xFFE0E0E0 : 0xFF282828;
}
uint32_t SearchContrastGrayColor(double background, bool light) {
    if (!std::isfinite(background)) return SearchContrastPreferredColor(light);
    background = std::clamp(background,0.0,1.0);
    auto const& linear = SearchContrastLinearColors();
    int gray = light ? 224 : 40;
    const double target = light ? 4.5*(background+.05)-.05 : (background+.05)/4.5-.05;
    // Quantized tones keep tiny sampling changes from shimmering. Stronger
    // black/white is used only near the middle-brightness crossover.
    while (light ? linear[gray] < target && gray < 255 : linear[gray] > target && gray > 0)
        gray = light ? std::min(gray+4,255) : std::max(gray-4,0);
    return 0xFF000000u | static_cast<uint32_t>(gray)*0x010101u;
}

constexpr size_t kSearchContrastProfilePoints = 33;
struct SearchContrastGradientStop {
    double offset;
    bool light;
    uint32_t color;
    bool operator==(SearchContrastGradientStop const&) const = default;
};
struct SearchContrastGradient {
    std::array<std::optional<bool>,kSearchContrastProfilePoints> light{};
    std::array<uint32_t,kSearchContrastProfilePoints> colors{};
    std::vector<SearchContrastGradientStop> stops;
    double widthDip = 0;
    bool Mixed() const {
        return !stops.empty() && std::any_of(stops.begin(),stops.end(),[&](auto const& stop) { return stop.color != stops.front().color; });
    }
    bool Sample(std::array<double,kSearchContrastProfilePoints> const& values, double inset, double width) {
        auto old = stops;
        widthDip = std::max(width,1.0);
        inset = std::clamp(inset,0.0,.2);
        auto known = values;
        // Keep both the tone and polarity through a covered/unsampled part of
        // a card. The first observation borrows the nearest exposed sample.
        for (size_t i = 0; i < known.size(); ++i) {
            if (!std::isfinite(known[i])) {
                if (light[i]) continue;
                size_t nearest = known.size(), distance = known.size();
                for (size_t j = 0; j < values.size(); ++j) if (std::isfinite(values[j])) {
                    const size_t d = i > j ? i-j : j-i;
                    if (d < distance) { nearest = j; distance = d; }
                }
                if (nearest < known.size()) known[i] = values[nearest];
            }
            if (std::isfinite(known[i])) {
                light[i] = SearchContainerLightText(known[i],light[i]);
                colors[i] = SearchContrastGrayColor(known[i],*light[i]);
            }
        }
        if (std::any_of(light.begin(),light.end(),[](auto v) { return !v.has_value(); })) return false;
        stops.clear();
        auto append = [&](double offset, size_t point) {
            SearchContrastGradientStop stop{offset,*light[point],colors[point]};
            if (!stops.empty() && stops.back().offset == offset) stops.back() = stop;
            else stops.push_back(stop);
        };
        append(0,0);
        const double spacing = (1-2*inset)/(light.size()-1);
        const double feather = std::min(1.0/widthDip,spacing*.4);
        for (size_t i = 1; i < light.size(); ++i) {
            if (colors[i] == colors[i-1]) continue;
            const double a = inset+(i-1)*spacing, b = inset+i*spacing;
            append(a,i-1);
            if (light[i] != light[i-1]) {
                double t = .5;
                if (std::isfinite(known[i-1]) && std::isfinite(known[i]) && std::abs(known[i]-known[i-1]) > .000001)
                    t = std::clamp((.17912878-known[i-1])/(known[i]-known[i-1]),0.0,1.0);
                // Quantized positions and a two-DIP crossover avoid a broad
                // gray band cutting through letters on mixed backgrounds.
                const double cross = std::clamp(std::round((a+t*spacing)*widthDip)/widthDip,a,b);
                append(std::max(a,cross-feather),i-1);
                append(std::min(b,cross+feather),i);
            }
            append(b,i);
        }
        append(1,light.size()-1);
        return old != stops;
    }
};

// One color field covers the main search window. Sparse empty strips across
// controls seed the background; scroll position only changes brush coordinates.
constexpr int kSearchContrastMapWidth = 512, kSearchContrastMapHeight = 512;
constexpr size_t kSearchContrastMaxMapBands = 128;
struct SearchContrastMapBand {
    double y;
    std::array<double,kSearchContrastProfilePoints> values;
    double left = 0, right = 1;
};
struct SearchContrastMapProbe {
    uint64_t id;
    RECT bounds;
    std::vector<RECT> strips;
    uint8_t exposed = 3;
};
struct SearchContrastMapReading {
    uint64_t id;
    RECT bounds;
    std::shared_ptr<std::vector<uint32_t> const> pixels;
    uint64_t revision = 0;
};
class SearchContrastMap {
    std::vector<uint32_t> pixels_;
    std::vector<SearchContrastMapBand> previousBands_;
    uint64_t revision_ = 0;
public:
    std::vector<uint32_t> const& Pixels() const { return pixels_; }
    uint64_t Revision() const { return revision_; }
    bool Sample(std::vector<SearchContrastMapBand> const& bands) {
        // Compare the small sampled profile before touching the full image.
        // Covered samples are semantically equal regardless of NaN payload.
        if (!pixels_.empty() && bands.size() == previousBands_.size() &&
            std::equal(bands.begin(),bands.end(),previousBands_.begin(),[](auto const& a,auto const& b) {
                return a.y == b.y && a.left == b.left && a.right == b.right &&
                    std::equal(a.values.begin(),a.values.end(),b.values.begin(),[](double x,double y) {
                        return x == y || (!std::isfinite(x) && !std::isfinite(y));
                    });
            })) return false;
        if (bands.empty() || std::none_of(bands.begin(),bands.end(),[](auto const& band) {
            return std::any_of(band.values.begin(),band.values.end(),[](double value){return std::isfinite(value);});
        })) return false;
        constexpr size_t count = kSearchContrastMapWidth*kSearchContrastMapHeight;
        const bool first = pixels_.empty();
        if (first) pixels_.resize(count);
        bool changed = first;
        std::vector<std::pair<double,double>> column;
        column.reserve(bands.size());
        for (int x = 0; x < kSearchContrastMapWidth; ++x) {
            const double position = x/double(kSearchContrastMapWidth-1);
            column.clear();
            for (auto const& band : bands) {
                if (position < band.left || position > band.right || band.right <= band.left) continue;
                const double point = std::clamp((position-band.left)/(band.right-band.left),0.0,1.0)*(kSearchContrastProfilePoints-1);
                const size_t left = static_cast<size_t>(point), right = std::min(left+1,kSearchContrastProfilePoints-1);
                double value = band.values[left];
                if (!std::isfinite(value)) value = band.values[right];
                else if (std::isfinite(band.values[right])) value += (band.values[right]-value)*(point-left);
                if (std::isfinite(value)) column.push_back({band.y,value});
            }
            if (column.empty()) continue;
            std::sort(column.begin(),column.end());
            size_t below = 0;
            for (int y = 0; y < kSearchContrastMapHeight; ++y) {
                const double row = y/double(kSearchContrastMapHeight-1);
                while (below+1 < column.size() && column[below+1].first < row) ++below;
                const auto& a = column[below]; const auto& b = column[std::min(below+1,column.size()-1)];
                const double blend = b.first > a.first ? std::clamp((row-a.first)/(b.first-a.first),0.0,1.0) : 0;
                const double luminance = a.second+(b.second-a.second)*blend;
                auto& old = pixels_[y*kSearchContrastMapWidth+x];
                const auto prior = old ? std::optional<bool>((old&255)>127) : std::nullopt;
                const auto color = SearchContrastGrayColor(luminance,SearchContainerLightText(luminance,prior));
                changed |= old != color; old = color;
            }
        }
        if (first) for (int y = 0; y < kSearchContrastMapHeight; ++y) {
            auto row = pixels_.begin()+y*kSearchContrastMapWidth;
            auto valid = std::find_if(row,row+kSearchContrastMapWidth,[](auto color){return color != 0;});
            if (valid == row+kSearchContrastMapWidth) std::fill(row,row+kSearchContrastMapWidth,SearchContrastPreferredColor(true));
            else {
                std::fill(row,valid,*valid);
                for (auto it=valid+1;it!=row+kSearchContrastMapWidth;++it) if(!*it)*it=*(it-1);
            }
        }
        // Keep contrast at every texel. Averaging light and dark text colors
        // creates a gray band that disappears against the mixed background.
        previousBands_ = bands;
        if (changed) ++revision_;
        return changed;
    }
};

struct SearchContrastProbe { uint64_t id; RECT bounds; bool outside = false; uint8_t exposed = 3; RECT surface{}; };
struct SearchContrastReading {
    uint64_t id; RECT bounds; double luminance;
    std::array<double,kSearchContrastProfilePoints> profile{};
    double inset = 0;
};
struct SearchContrastBatch {
    uint64_t generation = 0;
    HWND host = nullptr;
    DWORD thread = 0;
    std::vector<SearchContrastProbe> probes;
    std::vector<SearchContrastMapProbe> maps;
};
struct SearchContrastResult {
    uint64_t generation = 0;
    std::vector<SearchContrastReading> readings;
    std::vector<SearchContrastMapReading> maps;
};

// WindowFromPoint may ask a window to hit-test itself. Keep that operation on
// the UI thread, so joining the capture worker can never wait on this thread.
uint8_t SearchContrastExposure(SearchContrastProbe const& probe, HWND host, DWORD thread) {
    const int inset = std::max(6,MulDiv(6,GetDpiForWindow(host),96));
    uint8_t exposed = 0;
    for (int side = 0; side < 2; ++side) {
        const int x = side ? probe.bounds.right-inset-kSearchContrastStripWidth : probe.bounds.left+inset;
        HWND window = WindowFromPoint(POINT{x+1,(probe.bounds.top+probe.bounds.bottom)/2});
        if (!window || GetWindowThreadProcessId(window,nullptr) != thread) continue;
        HWND root = GetAncestor(window,GA_ROOT);
        if (!probe.outside && root != host) continue;
        if (probe.outside) {
            RECT bounds{}, intersection{};
            if (!GetWindowRect(root,&bounds) || !IntersectRect(&intersection,&bounds,&probe.bounds) ||
                !EqualRect(&intersection,&probe.bounds)) continue;
        }
        exposed |= 1u<<side;
    }
    return exposed;
}

class SearchContrastCapture {
    HDC memory_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ previous_ = nullptr;
    uint32_t* pixels_ = nullptr;
    static constexpr int kSize = 512;
    std::array<std::pair<uint64_t,SearchContrastMap>,1> mapCache_{};
    std::array<uint64_t,1> mapRevisions_{};
    uint64_t mapRevision_ = 0;
    std::shared_ptr<std::vector<uint32_t> const> mapPixels_;
public:
    SearchContrastCapture() {
        memory_ = CreateCompatibleDC(nullptr);
        if (!memory_) return;
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = kSize;
        info.bmiHeader.biHeight = -kSize;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        bitmap_ = CreateDIBSection(memory_,&info,DIB_RGB_COLORS,reinterpret_cast<void**>(&pixels_),nullptr,0);
        if (bitmap_) previous_ = SelectObject(memory_,bitmap_);
        SetStretchBltMode(memory_,COLORONCOLOR);
    }
    ~SearchContrastCapture() {
        if (previous_ && previous_ != HGDI_ERROR) SelectObject(memory_,previous_);
        if (bitmap_) DeleteObject(bitmap_);
        if (memory_) DeleteDC(memory_);
    }
    SearchContrastCapture(SearchContrastCapture const&) = delete;
    SearchContrastCapture& operator=(SearchContrastCapture const&) = delete;

    SearchContrastResult Read(SearchContrastBatch const& batch, std::atomic<uint64_t> const& generation) {
        SearchContrastResult result{batch.generation,{}};
        if (!pixels_ || !previous_ || previous_ == HGDI_ERROR || generation.load() != batch.generation ||
            !IsWindowVisible(batch.host)) return result;
        const size_t count = std::min(batch.probes.size(),kSearchContrastMaxContainers);
        RECT area{};
        for (size_t i = 0; i < count; ++i) UnionRect(&area,&area,&batch.probes[i].bounds);
        for (size_t i = 0; i < std::min(batch.maps.size(),mapCache_.size()); ++i) UnionRect(&area,&area,&batch.maps[i].bounds);
        RECT desktop{GetSystemMetrics(SM_XVIRTUALSCREEN),GetSystemMetrics(SM_YVIRTUALSCREEN),0,0};
        desktop.right = desktop.left+GetSystemMetrics(SM_CXVIRTUALSCREEN);
        desktop.bottom = desktop.top+GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if (!IntersectRect(&area,&area,&desktop)) return result;
        const int width = area.right-area.left, height = area.bottom-area.top;
        const int step = std::max(1,(std::max(width,height)+kSize-1)/kSize);
        const int sampleWidth = (width+step-1)/step, sampleHeight = (height+step-1)/step;
        HDC screen = GetDC(nullptr);
        if (!screen) return result;
        // One readback for every container combined. Separate tiny BitBlts can
        // each synchronize with the compositor, despite moving fewer pixels.
        const BOOL copied = StretchBlt(memory_,0,0,sampleWidth,sampleHeight,screen,
            area.left,area.top,width,height,SRCCOPY | CAPTUREBLT);
        GdiFlush();
        ReleaseDC(nullptr,screen);
        if (!copied || generation.load() != batch.generation) return result;
        auto const& linear = SearchContrastLinearColors();
        const UINT dpi = GetDpiForWindow(batch.host);
        const int inset = std::max(6,MulDiv(6,dpi,96)), verticalInset = std::max(2,MulDiv(3,dpi,96));
        for (size_t i = 0; i < count && generation.load() == batch.generation; ++i) {
            auto const& probe = batch.probes[i]; auto const& rect = probe.bounds;
            if (rect.right-rect.left < 24 || rect.bottom-rect.top < 18 || !probe.exposed) continue;
            const RECT surface = IsRectEmpty(&probe.surface) ? rect : probe.surface;
            SearchContrastReading reading{probe.id,rect,0};
            reading.profile.fill(std::numeric_limits<double>::quiet_NaN());
            reading.inset = std::min(.2,inset/double(rect.right-rect.left));
            std::array<double,kSearchContrastProfilePoints> medians{};
            size_t valid = 0;
            for (size_t p = 0; p < kSearchContrastProfilePoints; ++p) {
                if (!(probe.exposed & (p < kSearchContrastProfilePoints/2 ? 1u : 2u))) continue;
                const int x = rect.left+inset+static_cast<int>(std::lround(
                    (rect.right-rect.left-2*inset-1)*p/double(kSearchContrastProfilePoints-1)));
                std::array<double,12> samples{}; size_t used = 0;
                // Sample empty top/bottom padding, rather than glyphs, icons,
                // or the preview image. A clipped row uses only visible padding.
                for (int y : {surface.top+verticalInset,surface.bottom-verticalInset-2}) {
                    if (y < rect.top || y+1 >= rect.bottom) continue;
                    for (int dy = 0; dy < 2; ++dy) for (int dx = -1; dx <= 1; ++dx) {
                        const int sx = std::clamp(static_cast<int>(std::lround((x+dx-area.left)*sampleWidth/double(width))),0,sampleWidth-1);
                        const int sy = std::clamp(static_cast<int>(std::lround((y+dy-area.top)*sampleHeight/double(height))),0,sampleHeight-1);
                        const uint32_t color = pixels_[sy*kSize+sx];
                        samples[used++] = .2126*linear[(color>>16)&255]+.7152*linear[(color>>8)&255]+.0722*linear[color&255];
                    }
                }
                if (!used) continue;
                auto middle = samples.begin()+used/2;
                std::nth_element(samples.begin(),middle,samples.begin()+used);
                reading.profile[p] = medians[valid++] = *middle;
            }
            if (!valid) continue;
            auto middle = medians.begin()+valid/2;
            std::nth_element(medians.begin(),middle,medians.begin()+valid);
            reading.luminance = *middle;
            result.readings.push_back(std::move(reading));
        }
        for (size_t m = 0; m < std::min(batch.maps.size(),mapCache_.size()) && generation.load() == batch.generation; ++m) {
            auto const& probe = batch.maps[m]; auto const& rect = probe.bounds;
            if (!probe.exposed || rect.right-rect.left < 24 || rect.bottom-rect.top < 18) continue;
            std::vector<SearchContrastMapBand> bands;
            const size_t stripCount = std::min(probe.strips.size(),kSearchContrastMaxMapBands);
            for (size_t row = 0; row < stripCount; ++row) {
                RECT strip{};
                if (!IntersectRect(&strip,&probe.strips[row],&rect) || strip.right-strip.left < 24 || strip.bottom-strip.top < 2) continue;
                const int y = strip.top, edge = std::min<int>(inset,(strip.right-strip.left)/5);
                SearchContrastMapBand band{(y+.5-rect.top)/double(rect.bottom-rect.top),{},
                    (strip.left+edge-rect.left)/double(rect.right-rect.left),
                    (strip.right-edge-1-rect.left)/double(rect.right-rect.left)};
                band.values.fill(std::numeric_limits<double>::quiet_NaN());
                for (size_t p = 0; p < kSearchContrastProfilePoints; ++p) {
                    const int x = strip.left+edge+static_cast<int>(std::lround(
                        (strip.right-strip.left-2*edge-1)*p/double(kSearchContrastProfilePoints-1)));
                    if (!(probe.exposed & (x < (rect.left+rect.right)/2 ? 1u : 2u))) continue;
                    std::array<double,6> samples{}; size_t used = 0;
                    for (int dy = 0; dy < 2; ++dy) for (int dx = -1; dx <= 1; ++dx) {
                        const int sx = std::clamp(static_cast<int>(std::lround((x+dx-area.left)*sampleWidth/double(width))),0,sampleWidth-1);
                        const int sy = std::clamp(static_cast<int>(std::lround((y+dy-area.top)*sampleHeight/double(height))),0,sampleHeight-1);
                        const uint32_t color = pixels_[sy*kSize+sx];
                        samples[used++] = .2126*linear[(color>>16)&255]+.7152*linear[(color>>8)&255]+.0722*linear[color&255];
                    }
                    auto middle = samples.begin()+used/2;
                    std::nth_element(samples.begin(),middle,samples.begin()+used);
                    band.values[p] = *middle;
                }
                bands.push_back(std::move(band));
            }
            auto& cached = mapCache_[m];
            if (cached.first != probe.id) cached = {probe.id,{}};
            if (cached.second.Sample(bands)) {
                mapRevisions_[m] = ++mapRevision_;
                // Results share an immutable image until its actual colors
                // change; a stationary timer tick allocates/copies no bitmap.
                mapPixels_ = std::make_shared<std::vector<uint32_t> const>(cached.second.Pixels());
            }
            if (!cached.second.Pixels().empty())
                result.maps.push_back({probe.id,rect,mapPixels_,mapRevisions_[m]});
        }
        return result;
    }
};

class SearchContrastSampler {
    std::mutex mutex_;
    std::condition_variable wake_;
    std::optional<SearchContrastBatch> pending_;
    std::optional<SearchContrastResult> completed_;
    bool stopping_ = false, working_ = false;
    std::atomic<uint64_t> generation_{0};
    std::thread worker_;
    void Run() noexcept {
        auto dpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        try {
            SearchContrastCapture capture;
            std::unique_lock lock(mutex_);
            for (;;) {
                wake_.wait(lock,[&] { return stopping_ || pending_.has_value(); });
                if (stopping_) break;
                auto batch = std::move(*pending_); pending_.reset(); working_ = true;
                lock.unlock();
                auto result = capture.Read(batch,generation_);
                lock.lock(); working_ = false;
                if (!stopping_ && result.generation == generation_.load()) {
                    completed_ = std::move(result);
                    PostMessageW(batch.host,kSearchContainerContrastReady,0,0);
                }
            }
        } catch (...) {
            std::lock_guard lock(mutex_); working_ = false; stopping_ = true; pending_.reset(); completed_.reset();
        }
        if (dpi) SetThreadDpiAwarenessContext(dpi);
    }
public:
    SearchContrastSampler() : worker_([this] { Run(); }) {}
    ~SearchContrastSampler() {
        { std::lock_guard lock(mutex_); stopping_ = true; pending_.reset(); ++generation_; }
        wake_.notify_one();
        if (worker_.joinable()) worker_.join();
    }
    void Cancel() {
        std::lock_guard lock(mutex_); ++generation_; pending_.reset(); completed_.reset();
    }
    bool Current(uint64_t generation) const { return generation == generation_.load(); }
    void Submit(HWND host, DWORD thread, std::vector<SearchContrastProbe> probes, std::vector<SearchContrastMapProbe> maps = {}) {
        if (probes.empty() && maps.empty()) return;
        std::lock_guard lock(mutex_);
        if (stopping_ || working_ || pending_ || completed_) return;
        pending_ = SearchContrastBatch{generation_.load(),host,thread,std::move(probes),std::move(maps)};
        wake_.notify_one();
    }
    std::optional<SearchContrastResult> Take() {
        std::lock_guard lock(mutex_);
        auto result = std::move(completed_); completed_.reset(); return result;
    }
};

// Container contrast bindings. Uniform backgrounds use shared solid colors.
// Mixed backgrounds use ordinary gradients aligned to the container, without
// backdrop effects on individual text elements.
struct SearchGradientBinding {
    winrt::weak_ref<wux::FrameworkElement> element;
    wuxm::LinearGradientBrush brush{nullptr};
    double opacity = 1;
};
struct SearchContrastPaint {
    SearchContrastGradient gradient;
    winrt::weak_ref<wux::FrameworkElement> root;
    std::vector<SearchGradientBinding> bindings;
};

struct SearchContrastContainer {
    uint64_t id;
    winrt::weak_ref<wux::FrameworkElement> root;
    wuxm::SolidColorBrush foreground{nullptr};
    bool dirty = true, active = false, outside = false;
    std::optional<bool> lightText;
    std::shared_ptr<SearchContrastPaint> paint;
};
[[clang::no_destroy]] std::optional<SearchContrastSampler> g_searchContrastSampler;
[[clang::no_destroy]] std::optional<std::vector<SearchContrastContainer>> g_searchContrastContainers;
uint64_t g_searchContrastNextId = 0;
bool g_searchContrastReported = false;

bool g_searchGradientLayoutDirty = false, g_searchGradientLayoutHooked = false;

struct SearchOriginalTextOpacity {
    winrt::weak_ref<wux::FrameworkElement> element;
    double opacity;
};
[[clang::no_destroy]] std::optional<std::vector<SearchOriginalTextOpacity>> g_searchOriginalTextOpacities;
size_t g_searchOpacityPruneAt = 256;

void MakeSearchTextReadable(wux::FrameworkElement const& element) {
    if (!element.try_as<wuxc::TextBlock>() && !element.try_as<wuxc::FontIcon>()) return;
    const double original = element.Opacity(), readable = SearchReadableTextOpacity(original);
    if (original == readable || SearchTextOverride()) return;
    if (!g_searchOriginalTextOpacities) g_searchOriginalTextOpacities.emplace();
    auto& saved = *g_searchOriginalTextOpacities;
    if (saved.size() >= g_searchOpacityPruneAt) {
        std::erase_if(saved,[](auto const& entry){return !entry.element.get();});
        g_searchOpacityPruneAt = std::max<size_t>(256,saved.size()*2);
    }
    auto found = std::find_if(saved.begin(),saved.end(),[&](auto const& entry){return entry.element.get()==element;});
    if (found == saved.end()) saved.push_back({winrt::make_weak(element),original});
    else found->opacity = original;
    element.Opacity(readable);
}

void RestoreSearchTextOpacity() {
    if (g_searchOriginalTextOpacities) for (auto const& entry : *g_searchOriginalTextOpacities)
        if (auto element=entry.element.get();element && element.Opacity()==SearchReadableTextOpacity(entry.opacity))
            element.Opacity(entry.opacity);
    g_searchOriginalTextOpacities.reset();
    g_searchOpacityPruneAt = 256;
}

struct SearchColorMapBinding {
    winrt::weak_ref<wux::FrameworkElement> element;
    wuxm::ImageBrush brush{nullptr};
    wuxm::MatrixTransform transform{nullptr};
    mutable winrt::weak_ref<wuxc::ScrollViewer> scroll;
    double opacity = 1;
};
struct SearchColorMapPaint {
    uint64_t id = 0, revision = 0;
    winrt::weak_ref<wux::FrameworkElement> root;
    wuxmi::WriteableBitmap bitmap{nullptr};
    std::vector<SearchColorMapBinding> bindings;
};
[[clang::no_destroy]] std::shared_ptr<SearchColorMapPaint> g_searchColorMap;
ULONGLONG g_searchColorMapScrollUntil = 0;
bool g_searchColorMapReported = false;

void PositionSearchColorMap(SearchColorMapPaint const& paint, SearchColorMapBinding const& binding) try {
    auto root = paint.root.get(), element = binding.element.get();
    if (!root || !element || root.ActualWidth() < 1 || root.ActualHeight() < 1) return;
    auto scroll = binding.scroll.get();
    if (!scroll) {
        auto parent = wuxm::VisualTreeHelper::GetParent(element);
        for (int depth=0;parent && depth<32;++depth) {
            if ((scroll=parent.try_as<wuxc::ScrollViewer>())) { binding.scroll=winrt::make_weak(scroll); break; }
            parent=wuxm::VisualTreeHelper::GetParent(parent);
        }
    }
    wf::Point offset{};
    auto content = scroll ? scroll.Content().try_as<wux::FrameworkElement>() : nullptr;
    if (content) {
        // Keep one root for both lists; their independent offsets affect only
        // the coordinates where each glyph reads the shared image.
        const auto origin = scroll.TransformToVisual(root).TransformPoint(wf::Point{0,0});
        offset = element.TransformToVisual(content).TransformPoint(wf::Point{0,0});
        offset.X += static_cast<float>(origin.X+scroll.Padding().Left-scroll.HorizontalOffset());
        offset.Y += static_cast<float>(origin.Y+scroll.Padding().Top-scroll.VerticalOffset());
        if (offset.Y+element.ActualHeight() < origin.Y-1 || offset.Y > origin.Y+scroll.ActualHeight()+1) return;
    }
    else offset = element.TransformToVisual(root).TransformPoint(wf::Point{0,0});
    const auto old = binding.transform.Matrix();
    wuxm::Matrix matrix{root.ActualWidth()/kSearchContrastMapWidth,0,0,
                       root.ActualHeight()/kSearchContrastMapHeight,-offset.X,-offset.Y};
    if (old.M11 != matrix.M11 || old.M22 != matrix.M22 || old.OffsetX != matrix.OffsetX || old.OffsetY != matrix.OffsetY)
        binding.transform.Matrix(matrix);
} catch (...) {}

void PositionSearchColorMaps(wux::FrameworkElement const& only = nullptr) {
    auto const& paint=g_searchColorMap;
    if (!paint) return; // created only for auto, reset on appearance/settings changes
    std::erase_if(paint->bindings,[](auto const& item){return !item.element.get();});
    for (auto const& binding : paint->bindings)
        if (!only || !binding.scroll.get() || binding.scroll.get() == only) PositionSearchColorMap(*paint,binding);
}

void SearchContrastScrolled(wuxc::ScrollViewer const& scroll) {
    if (!g_searchWindowWindow.load() || !g_searchColorMap) return;
    // Only reuse the cached image while scrolling. Moving labels cannot spoil
    // the next background capture, and there is no readback in this callback.
    g_searchColorMapScrollUntil = GetTickCount64()+160;
    if (g_searchContrastSampler) g_searchContrastSampler->Cancel();
    PositionSearchColorMaps(scroll);
}

wuxm::Brush SearchColorMapForeground(wux::FrameworkElement const& element, SearchColorMapPaint& paint, double opacity = 1) {
    MakeSearchTextReadable(element);
    for (auto const& binding : paint.bindings) if (binding.element.get() == element && binding.opacity == opacity) return binding.brush;
    wuxm::ImageBrush brush;
    brush.ImageSource(paint.bitmap);
    brush.Opacity(opacity);
    brush.Stretch(wuxm::Stretch::None);
    brush.AlignmentX(wuxm::AlignmentX::Left); brush.AlignmentY(wuxm::AlignmentY::Top);
    wuxm::MatrixTransform transform;
    brush.Transform(transform);
    paint.bindings.push_back({winrt::make_weak(element),brush,transform,{},opacity});
    PositionSearchColorMap(paint,paint.bindings.back());
    return brush;
}


void PositionSearchGradient(SearchContrastPaint const& paint, SearchGradientBinding const& binding) try {
    auto root = paint.root.get(), element = binding.element.get();
    if (!root || !element) return;
    double width = root.ActualWidth();
    if (width < 1) width = paint.gradient.widthDip;
    if (width < 1) return;
    double offset = 0;
    try { offset = element.TransformToVisual(root).TransformPoint(wf::Point{0,0}).X; } catch (...) {}
    const wf::Point start{static_cast<float>(-offset),0}, end{static_cast<float>(width-offset),0};
    if (binding.brush.StartPoint() != start) binding.brush.StartPoint(start);
    if (binding.brush.EndPoint() != end) binding.brush.EndPoint(end);
} catch (...) {}

void UpdateSearchGradientStops(wuxm::LinearGradientBrush const& brush, SearchContrastGradient const& gradient) {
    auto stops = brush.GradientStops();
    if (stops.Size() != gradient.stops.size()) {
        stops.Clear();
        for (auto const& value : gradient.stops) {
            wuxm::GradientStop stop;
            stop.Offset(value.offset); stop.Color(SearchUnpackColor(value.color));
            stops.Append(stop);
        }
    } else for (uint32_t i = 0; i < stops.Size(); ++i) {
        auto stop = stops.GetAt(i); auto const& value = gradient.stops[i];
        const auto color = SearchUnpackColor(value.color);
        if (stop.Color() != color) stop.Color(color);
        if (stop.Offset() != value.offset) stop.Offset(value.offset);
    }
}

void RefreshSearchGradientPaint(SearchContrastPaint& paint) {
    std::erase_if(paint.bindings,[](auto const& item) { return !item.element.get(); });
    for (auto const& binding : paint.bindings) {
        UpdateSearchGradientStops(binding.brush,paint.gradient);
        PositionSearchGradient(paint,binding);
    }
}

void EnsureSearchGradientLayoutHandler() {
    g_searchGradientLayoutDirty = true;
    if (g_searchGradientLayoutHooked || !g_resultsHost) return;
    KeepHandler(g_resultsHost,g_resultsHost.LayoutUpdated(winrt::auto_revoke,[](auto const&,auto const&) {
        if (!g_searchGradientLayoutDirty) return;
        g_searchGradientLayoutDirty = false;
        PositionSearchColorMaps();
        if (!g_searchContrastContainers) return;
        for (auto& container : *g_searchContrastContainers) if (container.paint)
            for (auto const& binding : container.paint->bindings) PositionSearchGradient(*container.paint,binding);
    }));
    g_searchGradientLayoutHooked = true;
}

wuxm::Brush SearchGradientForeground(wux::FrameworkElement const& element, wuxm::Brush const& fallback,
                                    SearchContrastPaint* paint, double opacity = 1) {
    MakeSearchTextReadable(element);
    if (!paint || !paint->gradient.Mixed()) return fallback;
    for (auto const& binding : paint->bindings)
        if (binding.element.get() == element && binding.opacity == opacity) return binding.brush;
    wuxm::LinearGradientBrush brush;
    brush.MappingMode(wuxm::BrushMappingMode::Absolute);
    brush.StartPoint(wf::Point{0,0}); brush.EndPoint(wf::Point{static_cast<float>(std::max(1.0,paint->gradient.widthDip)),0});
    brush.Opacity(opacity);
    UpdateSearchGradientStops(brush,paint->gradient);
    paint->bindings.push_back({winrt::make_weak(element),brush,opacity});
    PositionSearchGradient(*paint,paint->bindings.back());
    // New rows keep the cached gradient immediately. LayoutUpdated fixes their
    // final text offset before drawing; it does not capture or force layout.
    EnsureSearchGradientLayoutHandler();
    return brush;
}

void SetSearchContainerForeground(wux::DependencyObject const& node, wuxm::Brush const& brush, int depth = 0, SearchContrastPaint* paint = nullptr, SearchColorMapPaint* map = nullptr) {
    if (!node || depth > 32 || node == g_ourBox) return;
    auto foregroundFor = [&](wux::FrameworkElement const& element) -> wuxm::Brush {
        return map ? SearchColorMapForeground(element,*map) : SearchGradientForeground(element,brush,paint);
    };
    if (auto text = node.try_as<wuxc::TextBlock>()) { text.Foreground(foregroundFor(text)); return; }
    if (auto icon = node.try_as<wuxc::FontIcon>()) {
        if (g_searchIcons) for (auto& entry : *g_searchIcons) if (entry.icon.get() == icon) {
            if (icon.Foreground() == entry.applied) { auto foreground = foregroundFor(icon); icon.Foreground(foreground); entry.applied = foreground; }
            return; // preserve deliberately colored converter/utility icons
        }
        icon.Foreground(foregroundFor(icon)); return;
    }
    if (auto control = node.try_as<wuxc::Control>(); control &&
        (node.try_as<wuxc::MenuFlyoutPresenter>() || node.try_as<wuxc::MenuFlyoutItemBase>())) {
        auto foreground = foregroundFor(control);
        control.Foreground(foreground);
        for (auto key : {L"MenuFlyoutItemForeground",L"MenuFlyoutItemForegroundPointerOver",L"MenuFlyoutItemForegroundPressed",
                        L"MenuFlyoutSubItemForeground",L"MenuFlyoutSubItemForegroundPointerOver",L"MenuFlyoutSubItemForegroundPressed"})
            control.Resources().Insert(winrt::box_value(key),foreground);
        if (auto item = node.try_as<wuxc::MenuFlyoutItem>()) SetSearchContainerForeground(item.Icon(),brush,depth+1,paint,map);
        if (auto sub = node.try_as<wuxc::MenuFlyoutSubItem>()) SetSearchContainerForeground(sub.Icon(),brush,depth+1,paint,map);
    }
    // Logical content is available before Control templates are realized. Avoid
    // following both trees and recoloring every label twice.
    if (auto border = node.try_as<wuxc::Border>()) { SetSearchContainerForeground(border.Child(),brush,depth+1,paint,map); return; }
    if (auto panel = node.try_as<wuxc::Panel>()) {
        for (auto child : panel.Children()) SetSearchContainerForeground(child,brush,depth+1,paint,map);
        return;
    }
    if (auto content = node.try_as<wuxc::ContentControl>()) {
        if (auto child = content.Content().try_as<wux::DependencyObject>()) {
            SetSearchContainerForeground(child,brush,depth+1,paint,map); return;
        }
    }
    const int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; ++i) SetSearchContainerForeground(wuxm::VisualTreeHelper::GetChild(node,i),brush,depth+1,paint,map);
}

void BindSearchContrastContainer(SearchContrastContainer& container) {
    auto root = container.root.get();
    if (!root) return;
    if (!container.paint) container.paint = std::make_shared<SearchContrastPaint>();
    auto& paint = *container.paint;
    paint.root = winrt::make_weak(root); paint.bindings.clear();
    SetSearchContainerForeground(root,container.foreground,0,&paint);
    container.dirty = false;
}

void InvalidateSearchContrastContainer(wux::FrameworkElement const& root) {
    if (g_searchContrastContainers) for (auto& container : *g_searchContrastContainers) {
        if (container.root.get() != root) continue;
        // New heading/preview labels are already attached on this UI thread.
        // Rebind before returning to layout, rather than waiting for the timer.
        BindSearchContrastContainer(container);
        break;
    }
}

void PrepareSearchResultContrastColors() {
    if (!g_searchWindowWindow.load() || SearchTextOverride() || !g_resultsHost) return;
    auto& paint = g_searchColorMap;
    if (!paint || paint->root.get() != g_resultsHost) {
        paint = std::make_shared<SearchColorMapPaint>();
        paint->id = ++g_searchContrastNextId;
        paint->root = winrt::make_weak(g_resultsHost.as<wux::FrameworkElement>());
        paint->bitmap = wuxmi::WriteableBitmap{kSearchContrastMapWidth,kSearchContrastMapHeight};
        BYTE* bytes = nullptr;
        winrt::check_hresult(paint->bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(&bytes));
        if (!bytes) winrt::throw_hresult(E_POINTER);
        std::fill_n(reinterpret_cast<uint32_t*>(bytes),kSearchContrastMapWidth*kSearchContrastMapHeight,SearchMainTextColor());
        paint->bitmap.Invalidate();
    }
    paint->bindings.clear();
    SetSearchContainerForeground(g_resultsHost,g_searchTextBrush,0,nullptr,paint.get());
    auto foreground = SearchColorMapForeground(g_ourBox,*paint);
    g_ourBox.Foreground(foreground);
    g_ourBox.PlaceholderForeground(foreground);
    for (auto key : {L"TextControlForeground",L"TextControlForegroundFocused",L"TextControlForegroundPointerOver",
                    L"TextControlButtonForeground",L"TextControlButtonForegroundPointerOver",L"TextControlButtonForegroundPressed"})
        g_ourBox.Resources().Insert(winrt::box_value(key),foreground);
    EnsureSearchGradientLayoutHandler();
}

bool SearchContrastBounds(wux::FrameworkElement const& element, bool outside, RECT* rect, RECT* surface = nullptr) try {
    if (!element || element.Visibility() != wux::Visibility::Visible || element.Opacity() < .99 || !element.XamlRoot()) return false;
    HWND host = g_searchWindowWindow.load();
    POINT origin{};
    if (!host || !ClientToScreen(g_searchWindowChild,&origin)) return false;
    const double scale = element.XamlRoot().RasterizationScale();
    auto bounds = [&](wux::FrameworkElement const& item) {
        auto r = item.TransformToVisual(g_searchWindowRoot).TransformBounds(
            wf::Rect{0,0,static_cast<float>(item.ActualWidth()),static_cast<float>(item.ActualHeight())});
        return RECT{origin.x+static_cast<LONG>(std::ceil(r.X*scale)),origin.y+static_cast<LONG>(std::ceil(r.Y*scale)),
                    origin.x+static_cast<LONG>(std::floor((r.X+r.Width)*scale)),origin.y+static_cast<LONG>(std::floor((r.Y+r.Height)*scale))};
    };
    *rect = bounds(element);
    if (surface) *surface = *rect;
    if (!outside) {
        RECT visible{}; GetClientRect(g_searchWindowChild,&visible); OffsetRect(&visible,origin.x,origin.y);
        if (!IntersectRect(rect,rect,&visible)) return false;
        // A scrolled card must be sampled only inside its visible viewport.
        auto parent = wuxm::VisualTreeHelper::GetParent(element);
        for (int depth = 0; parent && depth < 32; ++depth) {
            if (auto scroll = parent.try_as<wuxc::ScrollViewer>()) {
                RECT viewport = bounds(scroll);
                if (!IntersectRect(rect,rect,&viewport)) return false;
                break;
            }
            parent = wuxm::VisualTreeHelper::GetParent(parent);
        }
    }
    RECT desktop{GetSystemMetrics(SM_XVIRTUALSCREEN),GetSystemMetrics(SM_YVIRTUALSCREEN),0,0};
    desktop.right = desktop.left+GetSystemMetrics(SM_CXVIRTUALSCREEN);
    desktop.bottom = desktop.top+GetSystemMetrics(SM_CYVIRTUALSCREEN);
    return IntersectRect(rect,rect,&desktop) && rect->right-rect->left >= 24 && rect->bottom-rect->top >= 18;
} catch (...) { return false; }

std::optional<SearchContrastMapProbe> BuildSearchColorMapProbe(SearchColorMapPaint const& paint) {
    auto root = paint.root.get();
    RECT rect{}, surface{};
    if (!root || !SearchContrastBounds(root,false,&rect,&surface) || !EqualRect(&rect,&surface)) return std::nullopt;
    SearchContrastMapProbe probe{paint.id,rect,{}};
    probe.exposed = SearchContrastExposure({paint.id,rect,false},g_searchWindowWindow.load(),GetCurrentThreadId());
    if (!probe.exposed) return std::nullopt;
    const int padding = std::max(2,MulDiv(3,GetDpiForWindow(g_searchWindowWindow.load()),96));
    auto add = [&](wux::FrameworkElement const& element) {
        RECT visible{}, bounds{};
        if (!SearchContrastBounds(element,false,&visible,&bounds)) return;
        for (const int y : {bounds.top+padding,bounds.bottom-padding-2})
            if (y >= visible.top && y+1 < visible.bottom)
                probe.strips.push_back(RECT{visible.left,y,visible.right,y+2});
    };
    add(g_searchBarBorder); add(g_appsHeaderHolder); add(g_filesHeaderHolder); add(g_footerBorder);
    for (auto panel : {g_appsList,g_resultsList}) if (panel)
        for (auto child : panel.Children()) add(child.try_as<wux::FrameworkElement>());
    if (probe.strips.empty()) return std::nullopt;
    std::sort(probe.strips.begin(),probe.strips.end(),[](RECT const& a,RECT const& b) {
        return a.top != b.top ? a.top < b.top : a.left < b.left;
    });
    if (probe.strips.size() > kSearchContrastMaxMapBands) {
        std::vector<RECT> selected; selected.reserve(kSearchContrastMaxMapBands);
        for(size_t i=0;i<kSearchContrastMaxMapBands;++i)
            selected.push_back(probe.strips[i*(probe.strips.size()-1)/(kSearchContrastMaxMapBands-1)]);
        probe.strips=std::move(selected);
    }
    return probe;
}

void ApplySearchColorMaps(SearchContrastResult const& result) {
    auto const& paint = g_searchColorMap;
    if (!paint || GetTickCount64() < g_searchColorMapScrollUntil) return;
    for (auto const& reading : result.maps) {
        if (paint->id != reading.id || !reading.pixels || reading.pixels->size() != kSearchContrastMapWidth*kSearchContrastMapHeight) continue;
        RECT bounds{};
        if (!SearchContrastBounds(paint->root.get(),false,&bounds) || !EqualRect(&bounds,&reading.bounds) ||
            !SearchContrastExposure({reading.id,bounds,false},g_searchWindowWindow.load(),GetCurrentThreadId())) break;
        if (paint->revision != reading.revision) {
            BYTE* bytes = nullptr;
            winrt::check_hresult(paint->bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(&bytes));
            if (!bytes || paint->bitmap.PixelBuffer().Capacity() < reading.pixels->size()*sizeof(uint32_t)) break;
            memcpy(bytes,reading.pixels->data(),reading.pixels->size()*sizeof(uint32_t));
            paint->bitmap.Invalidate();
            paint->revision = reading.revision;
            if (!g_searchColorMapReported) {
                SearchWindowTrace(L"full-window contrast map active; scrolling reuses one shared image");
                g_searchColorMapReported = true;
            }
        }
        PositionSearchColorMaps();
        break;
    }
}

bool SearchContainerContrastActive() {
    HWND host = g_searchWindowWindow.load();
    return host && IsWindowVisible(host) && !g_searchWindowClosing && !g_searchWindowSliding &&
        !g_searchWindowWaitingForStart && !g_searchWindowCloaked && !g_dragging &&
        GetWindowThreadProcessId(GetForegroundWindow(),nullptr) == GetCurrentThreadId() && !SearchTextOverride();
}

void PauseSearchContainerContrast() {
    if (HWND host = g_searchWindowWindow.load()) KillTimer(host,kSearchContainerContrastTimer);
    if (g_searchContrastSampler) g_searchContrastSampler->Cancel();
}
void ResetSearchContainerContrast() {
    PauseSearchContainerContrast();
    RestoreSearchTextOpacity();
    g_searchContrastContainers.reset();
    g_searchColorMap.reset();
    g_searchColorMapScrollUntil = 0;
    g_searchColorMapReported = false;
    g_searchContrastReported = false;
    g_searchGradientLayoutDirty = false;
}
void StopSearchContainerContrast() {
    ResetSearchContainerContrast();
    g_searchContrastSampler.reset(); // join before XAML or the host can disappear
    g_searchGradientLayoutHooked = false;
}

void ApplySearchContainerContrast() try {
    if (!g_searchContrastSampler) return;
    auto result = g_searchContrastSampler->Take();
    if (!result || !g_searchContrastSampler->Current(result->generation) || !SearchContainerContrastActive() ||
        !g_searchContrastContainers) return;
    ApplySearchColorMaps(*result);
    size_t applied = 0, lightCount = 0, mixedCount = 0;
    for (auto const& reading : result->readings) for (auto& container : *g_searchContrastContainers) {
        if (!container.active || container.id != reading.id) continue;
        RECT now{};
        if (!SearchContrastBounds(container.root.get(),container.outside,&now) || !EqualRect(&now,&reading.bounds) ||
            !SearchContrastExposure({container.id,now,container.outside},g_searchWindowWindow.load(),GetCurrentThreadId())) break;
        if (!container.paint) container.paint = std::make_shared<SearchContrastPaint>();
        const bool wasMixed = container.paint->gradient.Mixed();
        const double scale = GetDpiForWindow(g_searchWindowWindow.load())/96.0;
        const bool changed = container.paint->gradient.Sample(reading.profile,reading.inset,(now.right-now.left)/scale);
        const bool mixed = container.paint->gradient.Mixed();
        const bool light = SearchContainerLightText(reading.luminance,container.lightText);
        const uint32_t foreground = SearchContrastGrayColor(reading.luminance,light);
        const auto color = SearchUnpackColor(foreground);
        if (container.foreground.Color() != color) container.foreground.Color(color);
        container.lightText = light;
        if (wasMixed != mixed) BindSearchContrastContainer(container);
        else if (changed && mixed) RefreshSearchGradientPaint(*container.paint);
        ++applied; lightCount += light; mixedCount += mixed;
        break;
    }
    if (applied && !g_searchContrastReported) {
        wchar_t detail[128]{};
        swprintf_s(detail,L"container contrast active: %zu sampled, %zu light, %zu dark, %zu gradients",applied,lightCount,applied-lightCount,mixedCount);
        SearchWindowTrace(detail);
        g_searchContrastReported = true;
    }
} catch (...) { SearchWindowTrace(L"container color update failed",winrt::to_hresult()); }

void SearchContainerContrastTick() try {
    if (!SearchContainerContrastActive()) { PauseSearchContainerContrast(); return; }
    ApplySearchContainerContrast();
    if (!g_searchContrastContainers) g_searchContrastContainers.emplace();
    auto& containers = *g_searchContrastContainers;
    std::erase_if(containers,[](auto const& item) { return !item.root.get(); });
    for (auto& item : containers) item.active = false;
    std::vector<SearchContrastProbe> probes;
    auto add = [&](wux::FrameworkElement const& element, bool outside = false) {
        RECT rect{}, surface{};
        if (probes.size() >= kSearchContrastMaxContainers || !SearchContrastBounds(element,outside,&rect,&surface)) return;
        auto found = std::find_if(containers.begin(),containers.end(),[&](auto const& item) { return item.root.get() == element; });
        if (found == containers.end()) {
            const uint32_t fallback = SearchMainTextColor();
            containers.push_back({++g_searchContrastNextId,winrt::make_weak(element),wuxm::SolidColorBrush{SearchUnpackColor(fallback)}});
            found = containers.end()-1;
        }
        auto& item = *found;
        item.active = true; item.outside = outside;
        if (item.dirty) BindSearchContrastContainer(item);
        SearchContrastProbe probe{item.id,rect,outside};
        probe.surface = surface;
        probe.exposed = SearchContrastExposure(probe,g_searchWindowWindow.load(),GetCurrentThreadId());
        if (probe.exposed) probes.push_back(probe);
    };
    if (g_previewPopup && g_previewPopup.IsOpen() && g_previewCard.Opacity() >= .99) {
        add(g_previewDetails,true);
        if (g_previewText && g_previewText.Visibility() == wux::Visibility::Visible) add(g_previewThumbBox,true);
    }
    bool menuOpen = false;
    // Give menus priority over long result lists. Each open submenu is a
    // separate container, rather than inheriting its parent's sampled color.
    if (g_resultsHost && g_resultsHost.XamlRoot()) for (auto popup : wuxm::VisualTreeHelper::GetOpenPopupsForXamlRoot(g_resultsHost.XamlRoot())) {
        if (popup == g_previewPopup) continue;
        auto visit = [&](auto&& self, wux::DependencyObject const& node, int depth) -> void {
            if (!node || depth > 16) return;
            if (auto menu = node.try_as<wuxc::MenuFlyoutPresenter>()) { menuOpen=true; add(menu,true); return; }
            const int count = wuxm::VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < count; ++i) self(self,wuxm::VisualTreeHelper::GetChild(node,i),depth+1);
        };
        visit(visit,popup.Child(),0);
    }
    if (!g_searchColorMap) PrepareSearchResultContrastColors();
    std::vector<SearchContrastMapProbe> maps;
    if (!menuOpen && GetTickCount64() >= g_searchColorMapScrollUntil && g_searchColorMap)
        if (auto probe=BuildSearchColorMapProbe(*g_searchColorMap)) maps.push_back(std::move(*probe));
    if (!probes.empty() || !maps.empty()) {
        if (!g_searchContrastSampler) g_searchContrastSampler.emplace();
        g_searchContrastSampler->Submit(g_searchWindowWindow.load(),GetCurrentThreadId(),std::move(probes),std::move(maps));
    }
    SetTimer(g_searchWindowWindow.load(),kSearchContainerContrastTimer,250,nullptr);
} catch (...) { PauseSearchContainerContrast(); SearchWindowTrace(L"container color sampling unavailable",winrt::to_hresult()); }


struct SearchGlassBrush : wuxm::XamlCompositionBrushBaseT<SearchGlassBrush> {
    SearchBrushData data;
    searchcomp::Compositor compositor{nullptr};
    searchcomp::CompositionEffectFactory factory{nullptr};
    bool hostBackdrop;
    SearchGlassBrush(SearchBrushData value,bool host) : data(value),hostBackdrop(host) {
        compositor=wux::Hosting::ElementCompositionPreview::GetElementVisual(g_resultsHost).Compositor();
    }
    void OnConnected() noexcept {
        if (CompositionBrush()) return;
        try {
            winrt::Windows::UI::ViewManagement::UISettings settings;
            HIGHCONTRASTW contrast{sizeof(contrast)};
            const bool highContrast=SystemParametersInfoW(SPI_GETHIGHCONTRAST,sizeof(contrast),&contrast,0) &&
                                    (contrast.dwFlags&HCF_HIGHCONTRASTON);
            if (!settings.AdvancedEffectsEnabled() || highContrast) {
                CompositionBrush(compositor.CreateColorBrush(SearchUnpackColor(data.fallback))); return;
            }
            // This brush's definition is immutable. Reconnecting it (for
            // example in a preview) needs a new brush, not a new effect graph.
            if (!factory) {
                auto blur=winrt::make_self<SearchEffect>(CLSID_D2D1GaussianBlur);
                blur->properties={wf::PropertyValue::CreateSingle(static_cast<float>(data.blur)),
                                  wf::PropertyValue::CreateUInt32(1),wf::PropertyValue::CreateUInt32(1)};
                blur->sources={searchcomp::CompositionEffectSourceParameter(L"Backdrop")};
                searcheffect::IGraphicsEffectSource source=*blur;
                // Same saturation/luminance operations as WindhawkBlur.
                auto matrix=[&](std::array<float,20> const& values) {
                    auto effect=winrt::make_self<SearchEffect>(CLSID_D2D1ColorMatrix);
                    effect->properties={wf::PropertyValue::CreateSingleArray(values),
                                        wf::PropertyValue::CreateUInt32(1),wf::PropertyValue::CreateBoolean(false)};
                    effect->sources={source}; source=*effect;
                };
                constexpr float r=.2126f,g=.7152f,b=.0722f;
                if (data.saturation!=1) {
                    float sat=static_cast<float>(data.saturation),inv=1-sat;
                    matrix({inv*r+sat,inv*r,inv*r,0,inv*g,inv*g+sat,inv*g,0,
                            inv*b,inv*b,inv*b+sat,0,0,0,0,1,0,0,0,0});
                }
                const auto tint=SearchUnpackColor(data.color);
                if (data.luminosity>0) {
                    float op=static_cast<float>(data.luminosity);
                    float lum=(tint.R*r+tint.G*g+tint.B*b)/255.0f*op;
                    matrix({1-r*op,-r*op,-r*op,0,-g*op,1-g*op,-g*op,0,
                            -b*op,-b*op,1-b*op,0,0,0,0,1,lum,lum,lum,0});
                }
                auto flood=winrt::make_self<SearchEffect>(CLSID_D2D1Flood);
                flood->properties={wf::PropertyValue::CreateSingleArray(
                    {tint.R/255.0f,tint.G/255.0f,tint.B/255.0f,tint.A/255.0f})};
                auto composite=winrt::make_self<SearchEffect>(CLSID_D2D1Composite);
                composite->properties={wf::PropertyValue::CreateUInt32(0)}; // SOURCE_OVER
                composite->sources={source,*flood};
                factory=compositor.CreateEffectFactory(*composite);
            }
            auto effect=factory.CreateBrush();
            effect.SetSourceParameter(L"Backdrop",hostBackdrop ? compositor.CreateHostBackdropBrush() : compositor.CreateBackdropBrush());
            CompositionBrush(effect);
            if(hostBackdrop) SearchWindowTrace(L"glass composition ready");
        } catch (...) {
            SearchWindowTrace(L"glass effect fallback",winrt::to_hresult());
            try { CompositionBrush(compositor.CreateColorBrush(SearchUnpackColor(data.fallback))); } catch (...) {}
        }
    }
    void OnDisconnected() noexcept {
        try { if(auto brush=CompositionBrush()) { CompositionBrush(nullptr); brush.Close(); } } catch (...) {}
    }
};

wuxm::Brush RecreateSearchBrush(SearchBrushData const& data,bool hostBackdrop = false) {
    if (!std::isfinite(data.opacity) || !std::isfinite(data.tint) || !std::isfinite(data.luminosity)) return nullptr;
    wuxm::Brush brush{nullptr};
    if (data.kind == 1) brush=wuxm::SolidColorBrush{SearchUnpackColor(data.color)};
    else if (data.kind == 2) {
        wuxm::AcrylicBrush acrylic;
        acrylic.BackgroundSource(wuxm::AcrylicBackgroundSource::HostBackdrop);
        acrylic.TintColor(SearchUnpackColor(data.color)); acrylic.FallbackColor(SearchUnpackColor(data.fallback));
        acrylic.TintOpacity(std::clamp(data.tint,0.0,1.0));
        if (data.luminosity >= 0) acrylic.TintLuminosityOpacity(std::clamp(data.luminosity,0.0,1.0));
        brush=acrylic;
    } else if (data.kind == 4) {
        if (!std::isfinite(data.blur) || data.blur<0 || data.blur>100 || !std::isfinite(data.saturation) || data.saturation<0 || data.saturation>10) return nullptr;
        brush=winrt::make<SearchGlassBrush>(data,hostBackdrop);
    } else if (data.kind == 3 && data.count <= 8) {
        wuxm::LinearGradientBrush gradient;
        for(double coordinate:data.points) if(!std::isfinite(coordinate) || std::abs(coordinate)>1000) return nullptr;
        gradient.StartPoint(wf::Point{static_cast<float>(data.points[0]),static_cast<float>(data.points[1])});
        gradient.EndPoint(wf::Point{static_cast<float>(data.points[2]),static_cast<float>(data.points[3])});
        for (uint32_t i=0;i<data.count;++i) {
            if (!std::isfinite(data.offsets[i])) return nullptr;
            wuxm::GradientStop stop; stop.Color(SearchUnpackColor(data.colors[i])); stop.Offset(std::clamp(data.offsets[i],0.0,1.0));
            gradient.GradientStops().Append(stop);
        }
        brush=gradient;
    }
    if (brush) brush.Opacity(std::clamp(data.opacity,0.0,1.0));
    return brush;
}

// Public Windows.Graphics interop ABI, not supplied by the MinGW SDK.
struct SearchGeometryInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetGeometry(ID2D1Geometry**) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetGeometryUsingFactory(ID2D1Factory*, ID2D1Geometry**) = 0;
};

} // namespace
template <> inline constexpr winrt::guid winrt::impl::guid_v<SearchGeometryInterop>{
    0x0657AF73,0x53FD,0x47CF,{0x84,0xFF,0xC8,0x49,0x2D,0x2A,0x80,0xA3}};
namespace {

struct SearchGeometrySource : winrt::implements<SearchGeometrySource,
        winrt::Windows::Graphics::IGeometrySource2D,SearchGeometryInterop> {
    winrt::com_ptr<ID2D1Geometry> geometry;
    explicit SearchGeometrySource(winrt::com_ptr<ID2D1Geometry> value) : geometry(std::move(value)) {}
    HRESULT STDMETHODCALLTYPE GetGeometry(ID2D1Geometry** value) noexcept override {
        if(!value) return E_POINTER;
        geometry.copy_to(value); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE TryGetGeometryUsingFactory(ID2D1Factory*,ID2D1Geometry** value) noexcept override {
        if(!value) return E_POINTER;
        *value=nullptr; return E_NOTIMPL;
    }
};

RECT g_searchWindowSlideFrame{};  // full, unclipped position of the XAML content
HRGN g_searchWindowFullShape = nullptr;  // owned here; SetWindowRgn receives a copy

void PositionSearchWindowChild() {
    HWND host = g_searchWindowWindow.load();
    if (!host || !g_searchWindowChild) return;
    RECT visible{}, frame = g_searchWindowSlideFrame;
    if (!GetWindowRect(host, &visible)) return;
    if (IsRectEmpty(&frame)) frame = visible;
    const RECT child{frame.left-visible.left,frame.top-visible.top,
                     frame.right-visible.left,frame.bottom-visible.top};
    static HWND lastChild = nullptr;
    static RECT last{};
    if (lastChild == g_searchWindowChild && EqualRect(&last,&child)) return;
    if (SetWindowPos(g_searchWindowChild,nullptr,child.left,child.top,
                     child.right-child.left,child.bottom-child.top,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW)) {
        lastChild = g_searchWindowChild;
        last = child;
    }
}

void ApplySearchWindowShapeRegion() {
    HWND host = g_searchWindowWindow.load();
    if (!host || !g_searchWindowFullShape) return;
    RECT visible{}, frame = g_searchWindowSlideFrame;
    if (!GetWindowRect(host,&visible)) return;
    if (IsRectEmpty(&frame)) frame = visible;
    const std::array<LONG,4> clip{frame.left-visible.left,frame.top-visible.top,
                                visible.right-visible.left,visible.bottom-visible.top};
    static HWND lastHost = nullptr;
    static HRGN lastShape = nullptr;
    static std::array<LONG,4> lastClip{};
    if (lastHost==host && lastShape==g_searchWindowFullShape && lastClip==clip) return;
    HRGN region = CreateRectRgn(0,0,0,0);
    HRGN viewport = CreateRectRgn(0,0,visible.right-visible.left,visible.bottom-visible.top);
    if (!region || !viewport) {
        if (region) DeleteObject(region);
        if (viewport) DeleteObject(viewport);
        return;
    }
    const bool valid = CombineRgn(region,g_searchWindowFullShape,nullptr,RGN_COPY)!=ERROR &&
        OffsetRgn(region,frame.left-visible.left,frame.top-visible.top)!=ERROR &&
        CombineRgn(region,region,viewport,RGN_AND)!=ERROR;
    DeleteObject(viewport);
    if (valid && SetWindowRgn(host,region,FALSE)) {
        lastHost=host; lastShape=g_searchWindowFullShape; lastClip=clip;
    } else DeleteObject(region);
}

void UpdateSearchWindowShape() {
    HWND host=g_searchWindowWindow.load(); if(!host || !g_resultsHost || !g_searchWindowRoot) return;
    RECT client{}; if(!GetClientRect(host,&client)) return;
    if (!IsRectEmpty(&g_searchWindowSlideFrame)) {
        client.right=g_searchWindowSlideFrame.right-g_searchWindowSlideFrame.left;
        client.bottom=g_searchWindowSlideFrame.bottom-g_searchWindowSlideFrame.top;
    }
    if(client.right<=0 || client.bottom<=0) return;
    const double scale=GetDpiForWindow(host)/96.0;
    const float width=static_cast<float>(client.right/scale),height=static_cast<float>(client.bottom/scale);
    auto corner=g_resultsHost.CornerRadius();
    const double dips[]{corner.TopLeft,corner.TopRight,corner.BottomRight,corner.BottomLeft};
    std::array<float,4> radii{};
    for(size_t i=0;i<4;++i) radii[i]=static_cast<float>(std::clamp(dips[i],0.0,std::min(width,height)/2.0));
    static HWND lastHost=nullptr; static RECT lastSize{}; static double lastScale=0;
    static std::array<float,4> lastRadii{};
    if(g_searchWindowFullShape && host==lastHost && EqualRect(&client,&lastSize) && scale==lastScale && radii==lastRadii) return;

    bool smooth=false;
    try {
        winrt::com_ptr<ID2D1Factory> factory;
        winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,factory.put()));
        winrt::com_ptr<ID2D1PathGeometry> path;
        winrt::check_hresult(factory->CreatePathGeometry(path.put()));
        winrt::com_ptr<ID2D1GeometrySink> sink;
        winrt::check_hresult(path->Open(sink.put()));
        sink->BeginFigure({radii[0],0},D2D1_FIGURE_BEGIN_FILLED);
        auto arc=[&](D2D1_POINT_2F point,float radius) {
            if(radius>0) sink->AddArc(D2D1_ARC_SEGMENT{point,{radius,radius},0,
                D2D1_SWEEP_DIRECTION_CLOCKWISE,D2D1_ARC_SIZE_SMALL});
            else sink->AddLine(point);
        };
        sink->AddLine({width-radii[1],0}); arc({width,radii[1]},radii[1]);
        sink->AddLine({width,height-radii[2]}); arc({width-radii[2],height},radii[2]);
        sink->AddLine({radii[3],height}); arc({0,height-radii[3]},radii[3]);
        sink->AddLine({0,radii[0]}); arc({radii[0],0},radii[0]);
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        winrt::check_hresult(sink->Close());
        auto visual=wux::Hosting::ElementCompositionPreview::GetElementVisual(g_searchWindowRoot);
        auto compositor=visual.Compositor();
        searchcomp::CompositionPath outline{winrt::make<SearchGeometrySource>(path.as<ID2D1Geometry>())};
        visual.BorderMode(searchcomp::CompositionBorderMode::Soft);
        visual.Clip(compositor.CreateGeometricClip(compositor.CreatePathGeometry(outline)));
        smooth=true;
        SearchWindowTrace(L"smooth window corners ready");
    } catch(...) { SearchWindowTrace(L"smooth window corners failed",winrt::to_hresult()); }

    // Retain a native region for pointer input, but place its hard edge two
    // physical pixels outside the antialiased curve. Only the composition clip
    // draws the visible edge; the transparent host has no backdrop to spill out.
    HRGN region=CreateRectRgn(0,0,client.right,client.bottom); if(!region) return;
    bool valid=true;
    for(int i=0;i<4;++i) {
        const int r=static_cast<int>(std::lround(radii[i]*scale));
        const int outside=smooth ? 2 : 0;
        if(!r) continue;
        const bool right=i==1 || i==2,bottom=i>=2;
        const int x=right ? client.right-r : 0,y=bottom ? client.bottom-r : 0;
        const int cx=right ? client.right-2*r : 0,cy=bottom ? client.bottom-2*r : 0;
        HRGN square=CreateRectRgn(x,y,x+r,y+r),circle=CreateEllipticRgn(cx-outside,cy-outside,cx+2*r+outside,cy+2*r+outside);
        if(!square || !circle || CombineRgn(square,square,circle,RGN_DIFF)==ERROR ||
           CombineRgn(region,region,square,RGN_DIFF)==ERROR) valid=false;
        if(square) DeleteObject(square); if(circle) DeleteObject(circle);
    }
    if(valid) {
        if(g_searchWindowFullShape) DeleteObject(g_searchWindowFullShape);
        g_searchWindowFullShape=region;
        lastHost=host; lastSize=client; lastScale=scale; lastRadii=radii;
        ApplySearchWindowShapeRegion();
    } else DeleteObject(region);
}


wux::CornerRadius SearchCorners(double const (&v)[4]) {
    return wux::CornerRadius{v[0],v[1],v[2],v[3]};
}
wux::Thickness SearchThickness(double const (&v)[4]) {
    return wux::ThicknessHelper::FromLengths(v[0],v[1],v[2],v[3]);
}
template<class Element> void ApplySearchSurface(Element const& element,SearchSurfaceData const& style) {
    if(style.flags&1) element.Background(RecreateSearchBrush(style.background));
    if(style.flags&2) element.BorderBrush(RecreateSearchBrush(style.outline));
    if(style.flags&4) element.CornerRadius(SearchCorners(style.corners));
    if(style.flags&8) element.BorderThickness(SearchThickness(style.thickness));
    if(style.flags&16) element.Padding(SearchThickness(style.padding));
    if(style.flags&32) element.Margin(SearchThickness(style.margin));
}

bool ValidSearchSurface(SearchSurfaceData const& surface) {
    if(surface.flags & ~127u) return false;
    for(auto values:{surface.corners,surface.thickness,surface.padding,surface.margin})
        for(int i=0;i<4;++i) if(!std::isfinite(values[i]) || values[i]<0 || values[i]>200) return false;
    return true;
}

struct SearchMenuAppearance {
    uint32_t dark,foreground,overrideColor;
    SearchSurfaceData presenter,border,item;
    bool operator==(SearchMenuAppearance const&) const = default;
};
[[clang::no_destroy]] wux::Style g_searchMenuPresenterStyle{nullptr};
std::optional<SearchMenuAppearance> g_searchMenuAppearance;
uint64_t g_searchMenuStyleGeneration=0,g_searchMenuRefreshedGeneration=0;

// The default MenuFlyoutPresenter template binds its border's material,
// corners, thickness and padding to the corresponding presenter properties.
// Put the final values in its Style before layout, instead of finding and
// repainting an already visible Border.
wux::Style CopiedMenuPresenterStyle() {
    if(!g_searchWindowWindow.load() || !g_searchAppearanceValid) return nullptr;
    auto const& appearance=g_searchAppearance;
    SearchMenuAppearance snapshot{appearance.dark,SearchMainTextColor(),SearchTextOverride(),appearance.menu,appearance.menuBorder,appearance.menuItem};
    if(g_searchMenuPresenterStyle && g_searchMenuAppearance && *g_searchMenuAppearance==snapshot)
        return g_searchMenuPresenterStyle;
    SearchSurfaceData surface=appearance.menu;
    auto const& border=appearance.menuBorder;
    if(border.flags&1) surface.background=border.background;
    if(border.flags&2) surface.outline=border.outline;
    if(border.flags&4) std::copy(std::begin(border.corners),std::end(border.corners),surface.corners);
    if(border.flags&8) std::copy(std::begin(border.thickness),std::end(border.thickness),surface.thickness);
    if(border.flags&16) std::copy(std::begin(border.padding),std::end(border.padding),surface.padding);
    // A child Border's margin is an internal inset, not the popup's position.
    // Do not transfer it to the presenter margin.
    surface.flags |= border.flags&31u;
    wux::Style style{winrt::xaml_typename<wuxc::MenuFlyoutPresenter>()};
    auto set=[&](wux::DependencyProperty const& property,wf::IInspectable const& value) {
        style.Setters().Append(wux::Setter{property,value});
    };
    const uint32_t foreground=SearchChooseTextColor(surface.background,
        surface.foreground ? surface.foreground : snapshot.foreground,appearance.dark!=0,snapshot.overrideColor);
    set(wux::FrameworkElement::RequestedThemeProperty(),winrt::box_value(
        SearchLuminance(foreground)>0.5 ? wux::ElementTheme::Dark : wux::ElementTheme::Light));
    set(wuxc::Control::ForegroundProperty(),wuxm::SolidColorBrush{SearchUnpackColor(foreground)});
    if(surface.flags&1) set(wuxc::Control::BackgroundProperty(),RecreateSearchBrush(surface.background));
    if(surface.flags&2) set(wuxc::Control::BorderBrushProperty(),RecreateSearchBrush(surface.outline));
    if(surface.flags&4) set(wuxc::Control::CornerRadiusProperty(),winrt::box_value(SearchCorners(surface.corners)));
    if(surface.flags&8) set(wuxc::Control::BorderThicknessProperty(),winrt::box_value(SearchThickness(surface.thickness)));
    if(surface.flags&16) set(wuxc::Control::PaddingProperty(),winrt::box_value(SearchThickness(surface.padding)));
    if(surface.flags&32) set(wux::FrameworkElement::MarginProperty(),winrt::box_value(SearchThickness(surface.margin)));
    g_searchMenuPresenterStyle=style;
    g_searchMenuAppearance=snapshot;
    ++g_searchMenuStyleGeneration;
    SearchWindowTrace(L"copied menu style prepared");
    return style;
}

void PrepareCopiedMenuItems(winrt::Windows::Foundation::Collections::IVector<wuxc::MenuFlyoutItemBase> const& items) {
    auto const& surface=g_searchAppearance.menuItem;
    auto const& menu=g_searchAppearance.menu;
    auto const& border=g_searchAppearance.menuBorder;
    auto background=(border.flags&1) ? border.background : menu.background;
    const uint32_t menuColor=SearchChooseTextColor(background,menu.foreground ? menu.foreground : SearchMainTextColor(),
                                                 g_searchAppearance.dark!=0,SearchTextOverride());
    const uint32_t itemColor=SearchChooseTextColor((surface.flags&1) ? surface.background : background,
        surface.foreground ? surface.foreground : menuColor,g_searchAppearance.dark!=0,SearchTextOverride());
    for(auto item:items) {
        if(item.try_as<wuxc::MenuFlyoutSeparator>()) continue;
        auto control=item.as<wuxc::Control>();
        wuxm::Brush foreground=wuxm::SolidColorBrush{SearchUnpackColor(itemColor)};
        control.Foreground(foreground);
        SetSearchContainerForeground(item,foreground);
        for(auto key:{L"MenuFlyoutItemForeground",L"MenuFlyoutItemForegroundPointerOver",L"MenuFlyoutItemForegroundPressed",
                      L"MenuFlyoutSubItemForeground",L"MenuFlyoutSubItemForegroundPointerOver",L"MenuFlyoutSubItemForegroundPressed"})
            control.Resources().Insert(winrt::box_value(key),foreground);
        // Remove our previous overrides if a new theme leaves them at default.
        if(!(surface.flags&1)) control.ClearValue(wuxc::Control::BackgroundProperty());
        if(!(surface.flags&2)) control.ClearValue(wuxc::Control::BorderBrushProperty());
        if(!(surface.flags&4)) control.ClearValue(wuxc::Control::CornerRadiusProperty());
        if(!(surface.flags&8)) control.ClearValue(wuxc::Control::BorderThicknessProperty());
        if(!(surface.flags&16)) control.ClearValue(wuxc::Control::PaddingProperty());
        if(!(surface.flags&32)) control.ClearValue(wux::FrameworkElement::MarginProperty());
        ApplySearchSurface(control,surface);
        if(auto sub=item.try_as<wuxc::MenuFlyoutSubItem>()) PrepareCopiedMenuItems(sub.Items());
    }
}

void PrepareCopiedMenu(wuxc::MenuFlyout const& menu) try {
    auto style=CopiedMenuPresenterStyle(); if(!style) return;
    if(menu.MenuFlyoutPresenterStyle()!=style) menu.MenuFlyoutPresenterStyle(style);
    PrepareCopiedMenuItems(menu.Items());
    // Opening reapplies the copied style. Restore any cached gradients before
    // that presenter draws, including reused nested-menu presenters.
    if (g_searchContrastContainers) for (auto& container : *g_searchContrastContainers)
        if (auto root = container.root.get(); root && root.try_as<wuxc::MenuFlyoutPresenter>())
            BindSearchContrastContainer(container);
} catch(...) { SearchWindowTrace(L"menu preparation failed",winrt::to_hresult()); }

void InitializeCopiedMenu(wuxc::MenuFlyout const& menu) {
    if(!g_searchWindowWindow.load()) return;
    PrepareCopiedMenu(menu);
    // Registered after the population handler, so every item and nested branch
    // is ready before the flyout can draw. Windows forwards this same presenter
    // style to each submenu before opening it (MenuFlyoutSubItem).
    KeepHandler(menu,menu.Opening(winrt::auto_revoke,[](wf::IInspectable const& sender,wf::IInspectable const&) {
        PrepareCopiedMenu(sender.as<wuxc::MenuFlyout>());
    }));
}

void RefreshCopiedMenuStyles() try {
    auto style=CopiedMenuPresenterStyle();
    if(!style || g_searchMenuRefreshedGeneration==g_searchMenuStyleGeneration) return;
    // A real appearance change may happen while a menu is already open. This
    // refresh handles that case only; opening new menus never waits for a timer.
    for(auto const* buttons:{&g_appButtonsOpt,&g_fileButtonsOpt}) {
        if(!*buttons) continue;
        for(auto const& button:**buttons) {
            if(auto menu=button.ContextFlyout().try_as<wuxc::MenuFlyout>()) PrepareCopiedMenu(menu);
        }
    }
    if(g_resultsHost && g_resultsHost.XamlRoot()) {
        for(auto popup:wuxm::VisualTreeHelper::GetOpenPopupsForXamlRoot(g_resultsHost.XamlRoot())) {
            std::function<void(wux::DependencyObject const&,int)> update;
            update=[&](wux::DependencyObject const& node,int depth) {
                if(!node || depth>16) return;
                if(auto presenter=node.try_as<wuxc::MenuFlyoutPresenter>()) {
                    presenter.Style(style);
                    return;
                }
                const int count=wuxm::VisualTreeHelper::GetChildrenCount(node);
                for(int i=0;i<count;++i) update(wuxm::VisualTreeHelper::GetChild(node,i),depth+1);
            };
            update(popup.Child(),0);
        }
    }
    g_searchMenuRefreshedGeneration=g_searchMenuStyleGeneration;
} catch(...) { SearchWindowTrace(L"menu appearance refresh failed",winrt::to_hresult()); }


void ApplyStartAppearance() try {
    if (!g_searchAppearanceValid || !g_resultsHost || !g_searchAppearanceDirty) return;
    auto const& appearance=g_searchAppearance;
    auto theme=appearance.dark ? wux::ElementTheme::Dark : wux::ElementTheme::Light;
    g_resultsHost.RequestedTheme(theme); g_cachedLightTheme.store(!appearance.dark);
    UpdateSearchIcon(!appearance.dark);
    if (auto brush=RecreateSearchBrush(appearance.background,true)) g_resultsHost.Background(brush);
    g_resultsHost.BorderBrush(RecreateSearchBrush(appearance.outline));
    auto bounded=[](double value) { return std::isfinite(value) ? std::clamp(value,0.0,200.0) : 0.0; };
    g_resultsHost.CornerRadius(wux::CornerRadius{bounded(appearance.corners[0]),bounded(appearance.corners[1]),bounded(appearance.corners[2]),bounded(appearance.corners[3])});
    g_resultsHost.BorderThickness(wux::ThicknessHelper::FromLengths(bounded(appearance.thickness[0]),bounded(appearance.thickness[1]),bounded(appearance.thickness[2]),bounded(appearance.thickness[3])));
    if (auto brush=RecreateSearchBrush(appearance.searchBackground)) g_searchBarBorder.Background(brush);
    g_searchBarBorder.BorderBrush(RecreateSearchBrush(appearance.searchOutline));
    g_searchBarBorder.CornerRadius(wux::CornerRadius{bounded(appearance.searchCorners[0]),bounded(appearance.searchCorners[1]),bounded(appearance.searchCorners[2]),bounded(appearance.searchCorners[3])});
    g_searchBarBorder.BorderThickness(wux::ThicknessHelper::FromLengths(bounded(appearance.searchThickness[0]),bounded(appearance.searchThickness[1]),bounded(appearance.searchThickness[2]),bounded(appearance.searchThickness[3])));
    g_searchAppearanceDirty = false;
    ApplySearchTextColors();
    DWORD backdrop=1; // XAML owns the material and alpha edge; keep the native host transparent
    DwmSetWindowAttribute(g_searchWindowWindow.load(),38,&backdrop,sizeof(backdrop));
    UpdateSearchWindowShape();
    RefreshCopiedMenuStyles();
} catch (...) { SearchWindowTrace(L"style copy failed",winrt::to_hresult()); }

void SearchWindowChooseBounds() {
    HWND host = g_searchWindowWindow.load();
    HWND start = CachedStartWindow();
    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR screen = start && !IsStartMenuClosed()
        ? MonitorFromWindow(start, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitor{sizeof(monitor)};
    if (!GetMonitorInfoW(screen,&monitor)) winrt::throw_last_error();
    g_searchWindowWork = monitor.rcWork;
    g_searchTaskbarEdge = TaskbarEdgeForMonitor(screen);
    const double scale = GetDpiForWindow(host) / 96.0;
    const RECT work = monitor.rcWork;
    bool customSize,filesVisible;
    int customWidth,customHeight;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        customSize=g_settings.useCustomMenuSize;
        customWidth=g_settings.menuWidth; customHeight=g_settings.menuHeight;
        filesVisible=!g_settings.hideFilesPanel;
    }
    const double fullWidth=g_searchAppearance.width >= 400 ? g_searchAppearance.width : 660;
    const double measuredWidth=customSize ? customWidth : (filesVisible ? fullWidth : fullWidth/2);
    const double measuredHeight=customSize ? customHeight :
        (g_searchAppearance.height >= 350 ? g_searchAppearance.height : 660);
    const int width=std::max(1,std::min(static_cast<int>(measuredWidth*scale),static_cast<int>(work.right-work.left-32)));
    const int height=std::max(1,std::min(static_cast<int>(measuredHeight*scale),static_cast<int>(work.bottom-work.top-32)));
    const bool sameEdge = g_searchAppearance.taskbarEdge == static_cast<uint32_t>(g_searchTaskbarEdge);
    const int gap = static_cast<int>((sameEdge ? g_searchAppearance.edgeGap : 12)*scale);
    g_searchWindowBounds = SearchBoundsForEdge(work,width,height,gap,
        sameEdge ? g_searchAppearance.alignment : 0.5,g_searchTaskbarEdge);
}

void FinishSearchWindowHide();

double SearchWindowFrameInterval() {
    MONITORINFOEXW monitor{}; monitor.cbSize = sizeof(monitor);
    DEVMODEW mode{}; mode.dmSize = sizeof(mode);
    double hz = 60;
    if (GetMonitorInfoW(MonitorFromRect(&g_searchWindowBounds,MONITOR_DEFAULTTONEAREST),&monitor) &&
        EnumDisplaySettingsW(monitor.szDevice,ENUM_CURRENT_SETTINGS,&mode) && mode.dmDisplayFrequency > 1)
        hz = mode.dmDisplayFrequency;
    DWM_TIMING_INFO timing{sizeof(timing)};
    if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr,&timing)) && timing.rateRefresh.uiDenominator) {
        const double dwmHz = static_cast<double>(timing.rateRefresh.uiNumerator)/timing.rateRefresh.uiDenominator;
        if (std::abs(dwmHz-hz) < 1) hz = dwmHz;
    }
    return 1000.0/std::clamp(hz,30.0,500.0);
}

void PositionSearchWindowSlide() {
    HWND host = g_searchWindowWindow.load();
    if (!host) return;
    RECT frame = g_searchWindowBounds;
    const int pixels = static_cast<int>(std::lround(g_searchWindowSlide.Value(SearchMotionTime())*GetDpiForWindow(host)/96.0));
    const POINT move = SearchSlideTranslation(g_searchTaskbarEdge,pixels);
    OffsetRect(&frame,move.x,move.y);
    if (EqualRect(&frame,&g_searchWindowLastFrame)) return;
    const RECT viewport = SearchSlideViewport(frame,g_searchWindowWork,g_searchTaskbarEdge);
    g_searchWindowSlideFrame = frame;
    if (SetWindowPos(host,nullptr,viewport.left,viewport.top,viewport.right-viewport.left,viewport.bottom-viewport.top,
                     SWP_NOZORDER | SWP_NOACTIVATE)) {
        PositionSearchWindowChild();
        ApplySearchWindowShapeRegion();
        g_searchWindowLastFrame = frame;
    }
}

double StopSearchWindowSlide() {
    const double current = g_searchWindowSlide.Value(SearchMotionTime());
    if (g_searchSlideClock) g_searchSlideClock->Pause();
    g_searchWindowSlide.Snap(current);
    g_searchWindowSliding = false;
    return current;
}

void SnapSearchWindowSlide(double offset) {
    StopSearchWindowSlide();
    g_searchWindowSlideTarget = offset;
    g_searchWindowSlide.Snap(offset);
    PositionSearchWindowSlide();
}

void CompleteSearchWindowSlide() {
    SnapSearchWindowSlide(g_searchWindowSlideTarget);
    if (g_searchWindowClosing) FinishSearchWindowHide();
    else if (!g_searchWindowWaitingForStart && !g_searchWindowCloaked) {
        if (g_renderAfterSlide) RequestRender();
        SchedulePreview();
        SearchContainerContrastTick();
    }
}

void StartSearchWindowSlide(double offset, unsigned duration, bool closing = false) {
    PauseSearchContainerContrast();
    const double from = StopSearchWindowSlide();
    g_searchWindowSlideTarget = offset;
    if (!duration || std::abs(from-offset) < 0.1) {
        CompleteSearchWindowSlide();
        return;
    }
    try {
        if (!g_searchSlideClock) g_searchSlideClock.emplace();
        const double interval = SearchWindowFrameInterval();
        g_searchWindowSlide = {from,offset,SearchMotionTime(),static_cast<double>(duration),closing};
        g_searchWindowSliding = true;
        g_searchSlideClock->Start(g_searchWindowWindow.load(),interval);
    } catch (...) {
        SearchWindowTrace(L"slide unavailable; finishing immediately", winrt::to_hresult());
        CompleteSearchWindowSlide();
    }
}

void FinishSearchWindowHide() {
    HWND host = g_searchWindowWindow.load();
    if (!host) return;
    KillTimer(host,kSearchWindowMotionTimer);
    KillTimer(host,kSearchWindowStyleTimer);
    PauseSearchContainerContrast();
    ShowWindow(host,SW_HIDE);
    SnapSearchWindowSlide(0);
    SetSearchWindowCloaked(false);
    g_searchWindowClosing = false;
    g_searchWindowWaitingForStart = false;
    g_isHiding = false;
    g_isOverlayVisible = false;
    if (g_ourBox) g_ourBox.Text(L"");
    SearchWindowTrace(L"hidden");
}

void SearchWindowMotionFrame() {
    HWND host = g_searchWindowWindow.load();
    if (!host) return;
    PositionSearchWindowSlide();
    const ULONGLONG now = GetTickCount64();
    if (g_searchWindowWaitingForStart) {
        const bool closed = IsStartMenuClosed();
        if (!closed) g_searchWindowStartClosedAt = 0;
        else if (!g_searchWindowStartClosedAt) g_searchWindowStartClosedAt = now;
        const ULONGLONG minimum = g_searchWindowStartWasOpen && SearchWindowAnimationsEnabled() ? 200 : 32;
        if (closed && now-g_searchWindowHandoffStarted >= minimum && now-g_searchWindowStartClosedAt >= 32) {
            g_searchWindowWaitingForStart = false;
            KillTimer(host,kSearchWindowMotionTimer);
            StartSearchWindowSlide(0,SearchWindowAnimationsEnabled() ? 240 : 0);
        } else if (!closed && !g_searchWindowShellCloseAsked && now-g_searchWindowHandoffStarted >= 450 &&
                   GetForegroundWindow() == host) {
            // A cloaked foreground window can leave Start open. Ask the
            // shell to close it once, after its normal close would have ended.
            g_searchWindowShellCloseAsked = true;
            if (HWND tray = FindWindowW(L"Shell_TrayWnd",nullptr))
                PostMessageW(tray,WM_SYSCOMMAND,SC_TASKLIST,0);
        } else if (now-g_searchWindowHandoffStarted >= 1500) {
            SearchWindowTrace(L"Start handoff timed out");
            FinishSearchWindowHide();
            return;
        }
    }
    if (!g_searchWindowWaitingForStart && g_searchWindowCloaked) {
        SetSearchWindowCloaked(false);
        if (!g_searchWindowSliding && g_renderAfterSlide) RequestRender();
        if (!g_searchWindowSliding) SchedulePreview();
        QueueSearchBoxRefocus();
        SearchWindowTrace(L"revealed after handoff");
    }
    if (!g_searchWindowWaitingForStart) KillTimer(host,kSearchWindowMotionTimer);
}

void RunSearchWindowMotion() {
    HWND host = g_searchWindowWindow.load();
    if (!host) return;
    SearchWindowMotionFrame();
    if (g_searchWindowWaitingForStart) {
        if (!SetTimer(host,kSearchWindowMotionTimer,15,nullptr)) {
            FinishSearchWindowHide();
        }
    }
}

void SearchWindowLayout(bool animate) {
    (void)animate;
    HWND host = g_searchWindowWindow.load();
    if (!host || !g_ourBox || g_searchWindowChanging || g_searchWindowClosing) return;
    g_searchWindowChanging = true;
    try {
        auto body = g_resultsHost.Children().GetAt(1).as<wux::FrameworkElement>();
        body.Visibility(wux::Visibility::Visible);
        if (g_ourBox.Text().empty()) {
            HidePreview();
            if(g_currentAppRows.empty() || !g_shownQuery.empty()) {
                g_enterWaiting=false; g_selectedApp=g_selectedFile=-1; g_filesColumnActive=false;
            }
            g_footerStatus.Text(FilesPanelVisible() ? L"Suggested apps" : L"Apps");
        }
        RunSearchWindowMotion();
    } catch (...) { SearchWindowTrace(L"layout failed",winrt::to_hresult()); }
    g_searchWindowChanging = false;
}

void SearchWindowHide() {
    HWND host = g_searchWindowWindow.load();
    if (!host || g_searchWindowClosing || !IsWindowVisible(host)) return;
    KillTimer(host,kSearchWindowDismissTimer);
    g_searchWindowInputReady = false; g_enterWaiting = false;
    g_isOverlayVisible = false; g_isHiding = true; g_searchWindowClosing = true;
    g_resultsHost.IsHitTestVisible(false);
    HidePreview();
    if (g_searchWindowWaitingForStart) { FinishSearchWindowHide(); return; }
    const double scale = GetDpiForWindow(host)/96.0;
    StartSearchWindowSlide(SearchHiddenDistance(g_searchWindowBounds,g_searchWindowWork,scale,g_searchTaskbarEdge),
                           SearchWindowAnimationsEnabled() ? 180 : 0,true);
}

void SearchWindowShow(std::wstring const& text, bool append) {
    HWND host = g_searchWindowWindow.load();
    if (!host || !g_ourBox || g_quit.load()) return;
    try {
        // Initialize defaults once, then rebuild brushes only when the copied
        // appearance (or a real theme/settings notification) changes.
        RequestStartAppearance(); ApplyStartAppearance();
        const bool wasVisible = IsWindowVisible(host);
        const bool wasClosing = g_searchWindowClosing;
        const bool fresh = !wasVisible || wasClosing;
        g_searchWindowClosing = false; g_isHiding = false;
        if (fresh) {
            StopSearchWindowSlide();
            SearchWindowChooseBounds();
            g_ourBox.Text(L""); g_dragging = false;
            RequestAppIndexRefresh();
            if (!wasVisible) {
                const double scale = GetDpiForWindow(host)/96.0;
                SnapSearchWindowSlide(SearchHiddenDistance(g_searchWindowBounds,g_searchWindowWork,scale,g_searchTaskbarEdge));
            }
            g_searchWindowStartWasOpen = !IsStartMenuClosed();
            g_searchWindowWaitingForStart = true;
            g_searchWindowHandoffStarted = GetTickCount64();
            g_searchWindowStartClosedAt = 0;
            g_searchWindowShellCloseAsked = false;
            SetSearchWindowCloaked(true);
            HidePreview();
        }
        if (!text.empty()) {
            std::wstring next = append && wasVisible && !wasClosing ? std::wstring(g_ourBox.Text()) : std::wstring();
            next += text; g_ourBox.Text(next);
        }
        QueueQuery(std::wstring(g_ourBox.Text())); // empty text also needs an initial suggestion query
        if (g_renderAfterSlide) RequestRender();
        g_searchActivationTick = GetTickCount64();
        KillTimer(host,kSearchWindowDismissTimer);
        g_isOverlayVisible = true;
        g_resultsHost.Opacity(1); g_resultsHost.IsHitTestVisible(true);
        SearchWindowLayout(false);
        HWND foreground = GetForegroundWindow();
        if (foreground && (foreground == CachedStartWindow() || IsSearchHostWindow(foreground))) {
            DWORD_PTR granted = 0;
            SendMessageTimeoutW(foreground,SearchWindowForegroundMessage(),0,0,SMTO_ABORTIFHUNG,150,&granted);
        }
        ShowWindow(host,SW_SHOWNORMAL);
        BOOL activated = SetForegroundWindow(host);
        g_searchWindowInputReady = false;
        SetFocus(g_searchWindowChild);
        g_searchWindowSource.NavigateFocus(winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationRequest{
            winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::First});
        g_ourBox.Focus(wux::FocusState::Programmatic);
        g_ourBox.SelectionStart(static_cast<int32_t>(g_ourBox.Text().size())); g_ourBox.SelectionLength(0);
        g_searchWindowInputReady = GetForegroundWindow() == host && SearchWindowTextboxHasFocus();
        QueueSearchBoxRefocus();
        SetTimer(host,kSearchWindowStyleTimer,500,nullptr);
        RunSearchWindowMotion();
        if (!g_searchWindowSliding) SearchContainerContrastTick();
        SearchWindowTrace(!activated ? L"shown, foreground denied" :
                          SearchWindowTextboxHasFocus() ? L"shown and focused" : L"shown; input focus pending");
    } catch (...) { SearchWindowTrace(L"show failed",winrt::to_hresult()); }
}

LRESULT CALLBACK SearchWindowWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) try {
    if (message == kSearchContainerContrastReady) { ApplySearchContainerContrast(); return 0; }
    if (message == kSearchWindowSlideFrameMessage) {
        if (g_searchSlideClock) g_searchSlideClock->pending.store(false);
        if (!g_searchWindowSliding) return 0;
        if (wParam || !g_searchWindowSlide.Active(SearchMotionTime())) CompleteSearchWindowSlide();
        else PositionSearchWindowSlide();
        return 0;
    }
    if (message == WM_COPYDATA && reinterpret_cast<HWND>(wParam) == CachedStartWindow()) {
        auto data = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
        if (data && data->dwData == kSearchWindowAppearanceData && data->cbData == sizeof(SearchAppearanceData) && data->lpData) {
            SearchAppearanceData appearance;
            memcpy(&appearance,data->lpData,sizeof(appearance));
            if (appearance.version == 4 && appearance.taskbarEdge <= static_cast<uint32_t>(SearchTaskbarEdge::Bottom) && std::isfinite(appearance.width) && std::isfinite(appearance.height) &&
                appearance.width >= 0 && appearance.width <= 4000 && appearance.height >= 0 && appearance.height <= 4000 &&
                std::isfinite(appearance.edgeGap) && std::isfinite(appearance.alignment) &&
                ValidSearchSurface(appearance.menu) && ValidSearchSurface(appearance.menuBorder) && ValidSearchSurface(appearance.menuItem)) {
                if ((appearance.width < 400 || appearance.height < 350) && g_searchAppearance.width >= 400) {
                    appearance.width=g_searchAppearance.width; appearance.height=g_searchAppearance.height;
                    appearance.edgeGap=g_searchAppearance.edgeGap; appearance.alignment=g_searchAppearance.alignment;
                    appearance.taskbarEdge=g_searchAppearance.taskbarEdge;
                }
                const bool changed=!g_searchAppearanceValid || memcmp(&appearance,&g_searchAppearance,sizeof(appearance)) != 0;
                g_searchAppearance = appearance; g_searchAppearanceValid = true;
                g_searchAppearanceDirty = g_searchAppearanceDirty || changed;
                if (changed) {
                    wchar_t description[128]{};
                    swprintf_s(description,L"Start appearance: %.0fx%.0f gap=%.1f align=%.2f background=%u color=%08X blur=%.0f menu=%u",appearance.width,appearance.height,appearance.edgeGap,appearance.alignment,appearance.background.kind,appearance.background.color,appearance.background.blur,appearance.menuBorder.flags);
                    SearchWindowTrace(description);
                }
                return 1;
            }
        }
        return 0;
    }
    if (message == kSearchWindowDismissMenuMessage) { DismissSearchContextMenu(); return 0; }
    if (message == kSearchWindowInputMessage) { ApplySearchWindowInput(); return 0; }
    if (message == kSearchWindowShowMessage) { SearchWindowShow(); return 0; }
    if (message == kSearchWindowLayoutMessage) { SearchWindowLayout(); return 0; }
    if (message == RemovalCheckMessage())
        return !g_removalPending.empty() && std::hash<std::wstring>{}(g_removalPending) == static_cast<size_t>(lParam);
    if (message == DragDoneMessage()) {
        g_dragging = false;
        if (wParam) SearchWindowHide(); else if (IsWindowVisible(hwnd)) SearchWindowShow();
        return 0;
    }
    switch (message) {
    case WM_NCCALCSIZE: if (wParam) return 0; break;
    case WM_ERASEBKGND: return 1;
    case WM_SIZE:
        PositionSearchWindowChild();
        UpdateSearchWindowShape();
        return 0;
    case WM_DPICHANGED:
        SearchWindowChooseBounds(); SearchWindowLayout(false);
        UpdateSearchIcon(IsLightTheme()); return 0;
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && !g_dragging) SetTimer(hwnd,kSearchWindowDismissTimer,70,nullptr);
        else {
            KillTimer(hwnd,kSearchWindowDismissTimer);
            if(LOWORD(wParam) != WA_INACTIVE) QueueSearchBoxRefocus();
        }
        break;
    case WM_SETFOCUS:
        QueueSearchBoxRefocus();
        break;
    case WM_TIMER:
        if (wParam == kSearchContainerContrastTimer) { SearchContainerContrastTick(); return 0; }
        if (wParam == kSearchWindowRefocusTimer) { SearchWindowRefocusFrame(); return 0; }
        if (wParam == kSearchWindowStyleTimer) {
            // Start already publishes appearance on its own timer. Avoid a
            // second cross-process capture every time we check input focus.
            if (!g_searchWindowSliding) ApplyStartAppearance();
            CheckSearchWindowFocus(); return 0;
        }
        if (wParam == kSearchWindowMotionTimer) { SearchWindowMotionFrame(); return 0; }
        if (wParam == kSearchWindowDismissTimer) {
            KillTimer(hwnd,kSearchWindowDismissTimer);
            HWND foreground = GetForegroundWindow();
            DWORD thread = GetWindowThreadProcessId(foreground,nullptr);
            if (!g_dragging && thread != GetCurrentThreadId() && IsWindowVisible(hwnd)) SearchWindowHide();
            return 0;
        }
        break;
    case WM_DISPLAYCHANGE:
        SearchWindowChooseBounds(); SearchWindowLayout(false);
        if (g_previewPopup && g_previewPopup.IsOpen()) PositionPreview();
        return 0;
    case WM_SETTINGCHANGE:
        if (wParam==SPI_SETWORKAREA) {
            SearchWindowChooseBounds(); SearchWindowLayout(false);
            if (g_previewPopup && g_previewPopup.IsOpen()) PositionPreview();
        }
        [[fallthrough]];
    case WM_THEMECHANGED:
        RefreshThemeCache(); SearchWindowStyle();
        if (!SearchWindowAnimationsEnabled() && g_searchWindowSliding) CompleteSearchWindowSlide();
        break;
    case WM_CLOSE: SearchWindowHide(); return 0;
    }
    return DefWindowProcW(hwnd,message,wParam,lParam);
} catch (...) {
    SearchWindowTrace(L"window message failed",winrt::to_hresult());
    return DefWindowProcW(hwnd,message,wParam,lParam);
}

// Menu text must also work when the low-level opening-input hook misses a key.
// Run before XAML's menu pretranslation, which can consume printable keydowns
// before TranslateMessage gets a chance to produce their characters.
bool SearchWindowMenuInput(MSG* message) {
    const UINT kind=message->message;
    if(kind!=WM_KEYDOWN && kind!=WM_SYSKEYDOWN && kind!=WM_CHAR && kind!=WM_SYSCHAR) return false;
    HWND host=g_searchWindowWindow.load();
    if(!host || !g_ourBox || !IsWindowVisible(host) || !IsWindowEnabled(host) ||
       g_dragging || g_searchWindowClosing ||
       GetWindowThreadProcessId(message->hwnd,nullptr)!=GetWindowThreadProcessId(host,nullptr) ||
       GetWindowThreadProcessId(GetForegroundWindow(),nullptr)!=GetWindowThreadProcessId(host,nullptr)) return false;
    if(!g_contextMenuOpen && !g_searchWindowMenuTyping && !ContextMenuHasFocus()) return false;

    BYTE state[256]{};
    if(!GetKeyboardState(state)) return false;
    const bool altGr=(state[VK_RMENU]&0x80)!=0;
    if((((state[VK_CONTROL]|state[VK_MENU])&0x80) && !altGr) ||
       ((state[VK_LWIN]|state[VK_RWIN])&0x80)) return false;
    if(kind==WM_KEYDOWN || kind==WM_SYSKEYDOWN) {
        if(message->wParam==VK_ESCAPE) { DismissSearchContextMenu(); return true; }
        if(message->wParam!=VK_BACK) {
            const auto key=static_cast<winrt::Windows::System::VirtualKey>(message->wParam);
            if(IsNavigationKey(key) || message->wParam==VK_HOME || message->wParam==VK_END ||
               message->wParam==VK_PRIOR || message->wParam==VK_NEXT) return false;
            wchar_t chars[8]{};
            const UINT scan=(static_cast<ULONG_PTR>(message->lParam)>>16)&0xff;
            if(message->wParam!=VK_PACKET && !ToUnicodeEx(static_cast<UINT>(message->wParam),scan,state,
               chars,ARRAYSIZE(chars),4,GetKeyboardLayout(0))) return false;
            // Let Windows compose dead keys and the active keyboard layout on
            // this UI thread; WM_CHAR below delivers the actual text once.
            TranslateMessage(message);
            return true;
        }
    } else if(message->wParam<0x20 || message->wParam==0x7f) return false;

    SearchWindowInput input;
    if(kind==WM_KEYDOWN || kind==WM_SYSKEYDOWN) input.kind=SearchWindowInput::Backspace;
    else input.text.assign(1,static_cast<wchar_t>(message->wParam));
    const bool wasTyping=g_searchWindowMenuTyping.exchange(true);
    if(!QueueSearchWindowInput(host,std::move(input))) {
        g_searchWindowMenuTyping=wasTyping; return false;
    }
    if(!wasTyping) SearchWindowTrace(L"context menu typing received by search window");
    return true;
}

bool SearchWindowPreTranslate(MSG* message) {
    if (!g_searchWindowNative) return false;
    if (SearchWindowMenuInput(message)) return true;
    HWND host=g_searchWindowWindow.load();
    if(message->message==WM_KEYDOWN && message->wParam==VK_ESCAPE && host &&
       GetAncestor(message->hwnd,GA_ROOT)==host && GetForegroundWindow()==host &&
       !g_dragging && !g_searchWindowClosing && !SearchWindowHasActivePopup() &&
       !EscapeBelongsToContextMenu() && GetKeyState(VK_CONTROL)>=0 && GetKeyState(VK_MENU)>=0) {
        if(g_ourBox && !g_ourBox.Text().empty()) { g_ourBox.Text(L""); QueueSearchBoxRefocus(true); }
        else SearchWindowHide();
        return true;
    }
    BOOL handled = FALSE;
    g_searchWindowNative->PreTranslateMessage(message, &handled);
    return handled;
}

bool SearchWindowInitialize() {
    try {
        SearchWindowTrace(L"initializing Windows 11 search window");
        WNDCLASSEXW wc{sizeof(wc)};
        wc.lpfnWndProc = SearchWindowWndProc; wc.hInstance = GetCurrentModuleHandle();
        wc.lpszClassName = kSearchWindowClass; wc.hCursor = LoadCursorW(nullptr,IDC_ARROW);
        RegisterClassExW(&wc);
        HWND host = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOREDIRECTIONBITMAP,kSearchWindowClass,L"Everything Search",
            WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_CLIPCHILDREN,
            0,0,660,660,nullptr,nullptr,wc.hInstance,nullptr);
        if (!host) winrt::throw_last_error();
        g_searchWindowWindow = host;
        ChangeWindowMessageFilterEx(host,WM_COPYDATA,MSGFLT_ALLOW,nullptr);
        BOOL enabled = TRUE;
        DwmSetWindowAttribute(host,DWMWA_TRANSITIONS_FORCEDISABLED,&enabled,sizeof(enabled));
        DwmSetWindowAttribute(host,17,&enabled,sizeof(enabled)); // DWMWA_USE_HOSTBACKDROPBRUSH
        DWORD borderColor=0xFFFFFFFE; // DWMWA_COLOR_NONE: the XAML panel draws its border
        DwmSetWindowAttribute(host,34,&borderColor,sizeof(borderColor));
        DWORD corner = 1;
        DwmSetWindowAttribute(host,33,&corner,sizeof(corner));
        DWORD backdrop = 1; // DWMSBT_NONE: XAML draws the material into an alpha-composed host
        DwmSetWindowAttribute(host,38,&backdrop,sizeof(backdrop));
        g_searchWindowManager = winrt::Windows::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();
        g_searchWindowSource = winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource{};
        g_searchWindowNative = g_searchWindowSource.as<IDesktopWindowXamlSourceNative2>();
        winrt::check_hresult(g_searchWindowNative->AttachToWindow(host));
        winrt::check_hresult(g_searchWindowNative->get_WindowHandle(&g_searchWindowChild));
        HRESULT transparencyResult = E_NOINTERFACE;
        if (auto current = wux::Window::Current()) {
            const IID transparencyId{0x06636c29,0x5a17,0x458d,{0x8e,0xa2,0x24,0x22,0xd9,0x97,0xa9,0x22}};
            winrt::com_ptr<SearchXamlTransparency> transparency;
            auto unknown = current.as<IUnknown>();
            transparencyResult = unknown->QueryInterface(transparencyId,transparency.put_void());
            if (SUCCEEDED(transparencyResult))
                transparencyResult = transparency->put_IsBackgroundTransparent(true);
        }
        SearchWindowTrace(L"XAML island transparency",transparencyResult);
        g_searchWindowRoot = wuxc::Grid{};
        g_searchWindowRoot.Background(MakeBrush(0,0,0,0));
        g_searchWindowRoot.RequestedTheme(IsLightTheme() ? wux::ElementTheme::Light : wux::ElementTheme::Dark);
        g_searchWindowSource.Content(g_searchWindowRoot);
        if (!g_xamlHandlers) g_xamlHandlers.emplace();
        BuildResultsList(g_searchWindowRoot);
        if (!g_resultsHost || !g_ourBox) winrt::throw_hresult(E_FAIL);
        KeepHandler(g_searchWindowSource,g_searchWindowSource.GotFocus(winrt::auto_revoke,[](auto const&,auto const&) {
            QueueSearchBoxRefocus();
        }));
        KeepHandler(g_searchWindowSource,g_searchWindowSource.TakeFocusRequested(winrt::auto_revoke,[](auto const&,auto const&) {
            QueueSearchBoxRefocus();
        }));
        SearchWindowStyle();
        g_ourBoxLost=g_ourBox.LostFocus(winrt::auto_revoke,[](wf::IInspectable const&,wux::RoutedEventArgs const&) {
            QueueSearchBoxRefocus();
        });
        AddPointerHandler(g_resultsHost,wux::UIElement::PointerReleasedEvent(),[](wf::IInspectable const&,wux::Input::PointerRoutedEventArgs const&) {
            QueueSearchBoxRefocus(true);
        });
        g_resultsHost.Opacity(1); g_resultsHost.IsHitTestVisible(true);
        SearchWindowChooseBounds(); SearchWindowLayout(false);
        g_searchQuit = false; g_searchThread.emplace(SearchThreadMain);
        StartSearchWindowInput();
        SearchWindowTrace(L"ready");
        return true;
    } catch (...) {
        SearchWindowTrace(L"initialization failed",winrt::to_hresult());
        SearchWindowTeardown(); return false;
    }
}

void SearchWindowTeardown() {
    if (!g_searchWindowWindow.load()) return;
    SearchWindowTrace(L"tearing down");
    StopSearchWindowSlide();
    StopSearchContainerContrast();
    g_searchSlideClock.reset();
    g_searchMenuPresenterStyle=nullptr;
    g_searchMenuAppearance.reset();
    KillTimer(g_searchWindowWindow.load(),kSearchWindowStyleTimer);
    KillTimer(g_searchWindowWindow.load(), kSearchWindowMotionTimer);
    KillTimer(g_searchWindowWindow.load(), kSearchWindowDismissTimer);
    StopSearchWindowInput();
    KillTimer(g_searchWindowWindow.load(),kSearchWindowRefocusTimer);
    g_searchWindowRefocusUntil=0; g_searchWindowRefocusBodyClick=false;
    g_searchWindowMenuTyping=false; g_searchWindowMenuCaret=-1; g_searchWindowMenuSelection=0;
    g_searchQuit = true;
    g_queryWake.notify_all();
    if (g_searchThread && g_searchThread->joinable()) g_searchThread->join();
    g_searchThread.reset();
    StopPreviewThread();
    WaitForTrackedLaunches();
    TeardownSearchUi();
    g_searchWindowRoot = nullptr;
    if (g_searchWindowSource) {
        g_searchWindowSource.Content(nullptr);
        g_searchWindowSource.Close();
    }
    g_searchWindowNative = nullptr;
    g_searchWindowSource = nullptr;
    g_searchWindowChild = nullptr;
    if (g_searchWindowManager) g_searchWindowManager.Close();
    g_searchWindowManager = nullptr;
    if (HWND host = g_searchWindowWindow.exchange(nullptr)) DestroyWindow(host);
    if (g_searchWindowFullShape) { DeleteObject(g_searchWindowFullShape); g_searchWindowFullShape=nullptr; }
    g_searchWindowSlideFrame={};
    g_searchWindowLastFrame={};
    UnregisterClassW(kSearchWindowClass, GetCurrentModuleHandle());
}

}  // namespace

// ===========================================================================
// Mod entry points
// ===========================================================================


BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_targetProcess = IdentifyCurrentProcess();

    if (g_targetProcess == TargetProcess::Explorer) {
        InitExplorer();
        return TRUE;
    }
    if (g_targetProcess == TargetProcess::SearchHost) {
        InitSearchHost();
        return TRUE;
    }
    if (g_targetProcess == TargetProcess::StartMenu) {
        LoadSettings();
        return TRUE;
    }
    return FALSE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"=== attached to pid %lu (%ls) ===", GetCurrentProcessId(),
        g_targetProcess == TargetProcess::StartMenu ? L"StartMenu" :
        g_targetProcess == TargetProcess::SearchHost ? L"SearchHost" :
        g_targetProcess == TargetProcess::Explorer ? L"Explorer" : L"Unknown");

    if (g_targetProcess == TargetProcess::Explorer) {
        StartExplorerHelperHost();
        return;
    }

    if (g_targetProcess == TargetProcess::SearchHost) {
        StartSearchHostWatchdog();
        return;
    }

    if (g_targetProcess != TargetProcess::StartMenu) {
        return;
    }

    StartAttachWatch();
    SubclassStartMenuWindow();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (g_targetProcess != TargetProcess::StartMenu && g_targetProcess != TargetProcess::Explorer) {
        return;
    }
    LoadSettings();
    if (g_targetProcess == TargetProcess::Explorer) {
        if (HWND helper=g_hExplorerHelperWnd.load()) PostMessageW(helper,kExplorerUpdateShortcut,0,0);
    }
    RequestAppIndexRefresh();
    if (g_resultsHost) {
        try {
            g_resultsHost.Dispatcher().RunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
                []() {
                    if (g_footerHints) {
                        bool show = true;
                        {
                            std::lock_guard<std::mutex> lock(g_settingsMutex);
                            show = g_settings.showKeyHints;
                        }
                        g_footerHints.Visibility(show ? wux::Visibility::Visible : wux::Visibility::Collapsed);
                    }
                    // The text size, on all that is built.
                    if (g_ourBox) {
                        ScaleFont(g_ourBox, 14);
                    }
                    if (g_searchBarBorder) {
                        g_searchBarBorder.Height(Fs(40));
                    }
                    RescaleFonts(g_resultsHost);
                    RescaleFonts(g_previewCard);
                    ApplyFilesPanelLayout();
                    QueueQuery(std::wstring(g_ourBox.Text()));
                    RenderResults();
                    if (g_searchWindowWindow.load()) { SearchWindowStyle(); SearchWindowChooseBounds(); SearchWindowLayout(); }
                });
        } catch (...) {}
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_quit.store(true);

    if (g_targetProcess == TargetProcess::SearchHost) {
        Wh_Log(L"uninit: SearchHost cleaning up");
        if (g_searchHostWatchdog && g_searchHostWatchdog->joinable()) {
            g_searchHostWatchdog->join();
            g_searchHostWatchdog.reset();
        }
        // Put SearchHost back as the shell left it: no subclass, and no zero
        // alpha -- left behind, it would keep native search invisible after
        // the mod is disabled.
        for (HWND hwnd : g_searchWindows) {
            if (IsWindow(hwnd)) {
                SetSearchWindowNeutralized(hwnd, FALSE);  // through the subclass: first
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, SearchHostSubclassProc);
            }
        }
        g_searchWindows.clear();
        Wh_Log(L"uninit: SearchHost cleanup complete");
        return;
    }

    if (g_targetProcess != TargetProcess::StartMenu) {
        if (g_targetProcess == TargetProcess::Explorer) {
            StopExplorerHelperHost();
        }
        Wh_Log(L"uninit: explorer.exe unhook complete");
        return;
    }

    StopAttachWatch();
    if (HWND core = g_hCoreWindow.exchange(nullptr); core && IsWindow(core)) {
        DWORD_PTR result = 0;
        const LRESULT sent = SendMessageTimeoutW(core, GetTeardownMessage(), 0, 0,
            SMTO_BLOCK | SMTO_ABORTIFHUNG, 5000, &result);
        if (!sent || static_cast<LRESULT>(result) != kTeardownDone)
            TeardownStartMenuBridge(core);
    }
    Wh_Log(L"uninit: StartMenuExperienceHost teardown complete");
}

// ==WindhawkMod==
// @id              explorer-command-bar
// @name            Explorer Command Bar
// @description     Customize the Windows 11 File Explorer command bar with commands, menus, New+, and a shell context-menu button
// @version         1.3.0
// @author          DanRotaru
// @github          https://github.com/DanRotaru
// @donateUrl       https://buymeacoffee.com/widget/page/danrotaru
// @homepage        https://dan13.me/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lcomctl32 -ldwmapi -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid
// @license         MIT
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# Explorer Command Bar

Add your own buttons and dropdown menus to the **Windows 11 File Explorer
command bar** (the toolbar with New / Sort / View), and hide the built-in
buttons, separators and spacing you don't want.

![Explorer Command Bar demo](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/explorer-command-bar/screenshots/main.gif)

Designed for Windows 11 24H2 / 25H2 / 26H2 with the WinAppSDK (WinUI 3) File
Explorer.

---

## Features

- **Custom buttons** - add as many toolbar buttons as you like, each running a
  command of your choice.
- **Dropdown menus & submenus** - a button can open a menu, and menu entries
  can themselves be submenus (three levels deep).
- **Shell context menu item** - optionally add an item which opens the real
  context menu of the active selection or folder, including shell extensions
  and optional Nilesoft Shell support.
- **Path & selection placeholders** - `%path%` (active tab folder), `%sel%`
  (all selected paths quoted and space-separated), and `%sel_each%` (launches
  one process instance per selected file).
- **Internal commands** - Execute native explorer actions like `internal:TogglePreview`,
  `internal:CopyPath`, `internal:NewFolder` and `internal:Refresh`.
- **Secondary Command Bar docking** - Optionally dock custom buttons onto the
  right-side command bar to preserve primary toolbar space.
- **Flexible icons** - light and dark theme pairs (`Light | Dark`), a Segoe Fluent
  Icons glyph, an `.exe` / `.dll` / `.ico` file, a Store-app icon
  (`shell:AppsFolder\…`), the command executable's own icon, or no icon at all.
- **Hide built-in elements** - individually hide New, Cut, Copy, Paste, Rename,
  Share, Delete, Sort, View, the group separators, the "See more" (…) overflow
  menu, the contextual commands (Set as background, Rotate left, Rotate right,
  Extract all) and the Details pane toggle. The labels of New, Sort, View and
  the Details pane toggle can be hidden too, leaving only their icons.
- **Reorder built-in buttons** - put New, Cut, Copy, Paste, Rename, Share,
  Delete, Sort and View in any order, with separators where you want them.
- **Run as administrator & window style** - start any item elevated, or
  minimized, maximized or hidden.
- **Replace New with New+** - turn Explorer's New button into a *New+* button
  which lists the templates of the
  [PowerToys **New+**](https://learn.microsoft.com/en-us/windows/powertoys/newplus)
  utility, with your own label and icon.
- **Custom item spacing** - set the exact spacing between the command bar
  buttons.
- **Open menus on hover** - optionally open dropdowns and the context menu on
  hover, with a configurable delay.
- **Rock solid** - buttons are re-applied automatically across tab switches,
  navigation, new tabs and new windows, and everything is cleanly restored when
  the mod is disabled.
- **Settings window** - a cog button in the command bar opens a Windows 11
  style settings window, where toolbar items and menus are edited in a tree
  instead of nested lists.

---

## Settings window

With **Show Explorer Command Bar Settings Item** enabled (the default), a cog
button is added to the command bar. It opens a settings window which follows
the WinUI 3 design of Windows 11. The toolbar items page shows a live preview
of the File Explorer command bar with your settings: click one of your items
(or an entry of one of its menus) to edit it below, with pickers for programs
and icons, the internal commands and the placeholders. Drag the items in the
preview, or the entries in a menu's list, to reorder them. The other settings are
grouped into pages with toggles and number boxes.

The window edits the same settings as Windhawk, so both always show the same
configuration. Windhawk stores mod settings in a protected registry key, so
**Save** shows an administrator (UAC) prompt, and File Explorer applies the new
settings right after. The portable version of Windhawk isn't supported by the
window.

**Explorer Command Bar Settings Item Order** sets the position of the cog
button among your toolbar items: `0` places it before the first item, `1`
after the first one, and so on. `-1` places it after the last one.

---

## Screenshots

Hiding built‑in buttons and separators:

![Hide buttons](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/explorer-command-bar/screenshots/hide-buttons.gif)

You may hide even all options, and use only your custom ones:

![Hide All buttons](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/explorer-command-bar/screenshots/hide-all-buttons.jpg)

---

## Command parameters

The following placeholders can be used in an item's parameters:

* `%path%` / `%dir%` / `%d%` - the folder path of the currently active tab.
* `%sel%` / `%f%` - all selected file or folder paths in the active tab, each quoted and separated by a space.
* `%sel_each%` - executes the target command once per selected file.
* `%files%` - paths of selected regular files only (quoted and space-separated).
* `%folders%` - paths of selected directories only (quoted and space-separated).
* `%base%` / `%basename%` - selected item base names without extension.
* `%file_name%` - base name of the first selected item.
* `%n%` / `%name%` - selected file/folder names only (with extension, no path).
* `%1%` - full path of the first selected item.
* `%ext%` - file extension of the first selected item.
* `%c%` / `%count%` - total number of selected items.
* `%d_name%` - native display name of the active folder.
* `%d_smart%` - selected folder if one is chosen, otherwise falls back to active folder.
* `%p%` / `%parent%` - parent folder path of the active tab.
* `%%` - literal percent sign escape.

Wrap placeholders in quotes if desired (e.g. `-d "%path%"` or `"%sel%"`).
If a used placeholder has no value (nothing selected, or a non-filesystem location like *This PC*), the command is launched without any parameters.
Commands run with the active tab's folder as their working directory.

---

## Dynamic UI Labels & Clamping

Embed live tokens directly into your button and menu labels! The text will automatically update in real-time as you click different files and folders in File Explorer.

* **Supported Label Tokens:** `%base%`, `%name%`, `%ext%`, `%count%` (or `%c%`), `%d_name%`, and `%path%`.
* **Usage:** Use the **Custom Label Text** field for top-level toolbar buttons, or the **Name** field for dropdown menu entries.
* **Example:** A button labeled `Extract %name%` will dynamically display `Extract archive.zip` when you highlight a zip file.
* **Smart Truncation:** To prevent your command bar from overflowing, labels automatically truncate with an ellipsis (`...`) if they exceed 24 characters on the main toolbar (or 48 characters inside menus). The full resolved text is always preserved in the button's hover tooltip.

---

## Internal commands

Instead of an executable, the **Command** field can run built-in File Explorer actions (supports `internal:` or `int:`, case-insensitive):

* `internal:TogglePreview` - toggles the Preview pane.
* `internal:ToggleDetails` - toggles the Details pane.
* `internal:OpenWith` - opens the native Windows "How do you want to open this file?" application chooser dialog for the selected file.
* `internal:FolderOptions` - opens the classic Folder Options control panel dialog.
* `internal:CopyPath` - copies the full paths of the selected items to the
  clipboard, one per line, or the current folder's path if nothing is
  selected.
* `internal:CopyName` - copies the names of the selected items, one per line,
  or the current folder's name if nothing is selected.
* `internal:CopyDir` - copies the path of the current folder.
* `internal:NewFolder` - creates a new folder in the current folder, selected
  and ready to be renamed.
* `internal:NewFile` - creates a new empty file the same way.
* `internal:Refresh` - refreshes the current folder.

For `NewFolder` and `NewFile`, the **Parameters** field sets the name, e.g.
`Notes $YYYY-$MM-$DD.md` (the [filename variables](#filename-variables) of
New+ can be used). If it's empty, the name is `New folder` or `file` (without
an extension), and
` (2)`, ` (3)`, … is appended if the name is taken. Both can be undone with
Ctrl+Z.

---

## Launch options

Each button and menu entry which runs a program has two more options:

* **Run as administrator** - starts the command elevated, after a UAC prompt.
  Holding **Ctrl+Shift** while clicking an item does the same for any item,
  like in the Start menu and the taskbar.
* **Window style** - `Normal`, `Minimized`, `Maximized` or `Hidden`. Hidden is
  meant for console commands and scripts which shouldn't leave a window
  behind, e.g. `cmd.exe` with `/c npm run build`. Some programs ignore the
  window style.

---

## Dynamic File Extension Filtering

The **File extension filter** field lets you automatically show or hide toolbar actions based on what is currently selected in File Explorer:

* Multiple comma-separated rules evaluate using **OR** logic (matches if *any* rule succeeds).
* Use **`+`** within a rule for **AND** logic (e.g., `files + single`).
* Prefix tokens with **`-`** (e.g., `-zip`, `-exe`, `-folder`) to act as an **absolute veto** — if any highlighted item matches, the button is immediately hidden.
* Adding `hide` permanently hides the button under all conditions.

**Available Filter Keywords:**
* Leave blank or `*` to always show the button.
* `none` (shows only when 0 items are selected).
* `selected` / `any` (shows when at least 1 item is highlighted).
* `files` or `folders` (shows if regular files or directories are selected).
* `mixed` (shows exclusively when both files and folders are highlighted).
* `single` or `multiple` (evaluates based on total item count).
* `single_file`, `multi_file`, `single_folder`, `multi_folder` (count + type).
* `only <ext>` (e.g., `only txt`, `only(txt, cpp)`) — Shows exclusively when **every** highlighted item matches the specified extensions.

---

## Keyboard shortcuts

Any button or menu entry can have a **Keyboard shortcut**, e.g.
`Ctrl+Shift+T`, `Alt+N` or `F9`. Pressing it while a File Explorer window is
active runs the item exactly like a click, with its parameters and
placeholders resolved for that window. Write the keys joined with `+`,
case-insensitive.

* Modifiers: `Ctrl`, `Alt`, `Shift` and `Win`.
* Keys: `A`-`Z`, `0`-`9`, `F1`-`F24`, `Num0`-`Num9`, `Space`, `Enter`, `Tab`,
  `Esc`, `Backspace`, `Delete`, `Insert`, `Home`, `End`, `PageUp`,
  `PageDown`, `Up`, `Down`, `Left`, `Right`, `Plus`, and the US layout
  punctuation keys `-` `,` `.` `;` `/` `` ` `` `[` `\` `]` `'`.
* A shortcut needs `Ctrl`, `Alt` or `Win`, unless the key is `F1`-`F24`, so
  that typing in the address bar or a file name isn't affected.

Shortcuts take precedence over File Explorer's own shortcuts. Dropdown menus
and submenus don't run a command, so their shortcuts are ignored.

---

## Icons

The **Icon glyph or icon path** field accepts several forms:

* **Theme-aware icons** - provide both light and dark icons separated by `|`
  (e.g., `E706 | E708`, `light.ico | dark.ico`, or `| dark.ico` to use the dark
  icon as fallback for both themes). Icons update dynamically on system theme
  changes.
* **A glyph** - a hex code point of a
  [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font)
  glyph, e.g. `E756`.
* **A file path** - an `.exe`, `.dll` or `.ico` file to take the icon from,
  optionally with an icon index, e.g. `C:\Windows\System32\shell32.dll,3`.
* **A Store app** - `shell:AppsFolder\<AppUserModelID>` to use a modern Store
  app's icon (useful for apps whose `.exe` stub carries a legacy icon, such as
  Notepad and Calculator).
* **Empty** - the icon is extracted from the command's executable (app
  execution aliases such as `wt.exe` are resolved to their real target).
* **Hide icon** - enable the toggle to show no icon at all.

---

## Built-in button order

In the settings window, drag File Explorer's own buttons, and the separators
between them, in the command bar preview to reorder them, the same way as your
own items. Selecting one of them shows a **Reset order** button.

In Windhawk's settings, the **Built-in button order** setting holds the same
order as text: the buttons separated by commas, with `|` where a separator
goes:

```
sort, view | new | cut, copy, paste, rename, share, delete
```

The names are `new`, `cut`, `copy`, `paste`, `rename`, `share`, `delete`,
`sort` and `view`. Buttons which aren't listed come last, in their usual order,
and an empty setting keeps File Explorer's own order. The New+ button moves
along with New.

File Explorer has two separators among these buttons, so the first two `|` are
used. The *Vertical separator after New / Delete* toggles still apply to a
separator placed right after New or Delete, and any other separator is hidden
along with the buttons after it once none of them is shown. The contextual
commands (Rotate, Extract all, …) aren't reordered, since they come and go with
the selection.

---

## Context menu item

Enable **Add the context menu item** in the **Context menu item** settings
group to append a button which expands File Explorer's real shell context menu.
It shows the selected items' menu, or the current folder's background menu when
nothing is selected. Shell extensions and nested entries such as *Open with*
and *Send to* are supported, and Shift-click includes extended verbs.

The **Let File Explorer show the menu** option is intended for Nilesoft Shell.
Because Nilesoft replaces the menu from inside Explorer rather than exposing an
`IContextMenu` handler, this option asks the active file list to display its own
menu. In that mode Explorer chooses its position.

---

## Replace New to New+

The **Replace New to New+** group turns Explorer's own **New** button into a
*New+* button: instead of Explorer's fixed list of file types, the dropdown
lists the templates of the
[PowerToys **New+**](https://learn.microsoft.com/en-us/windows/powertoys/newplus)
utility, and clicking one creates a copy of it in the current folder, selected
and ready to be renamed.

PowerToys is **not** required: the mod copies the templates itself and never
talks to the New+ shell extension. It only reads PowerToys' New+ settings file
(if there is one) to find the templates folder and the *Hide file extension* /
*Hide starting digits* / *Replace variables* options. Without PowerToys the New+
defaults are used: templates are read from
`%LOCALAPPDATA%\Microsoft\PowerToys\NewPlus\Templates`, extensions and starting
digits are hidden, and variables are not replaced. Any folder can be used
instead via the **Templates folder** setting.

Every file and folder directly inside the templates folder becomes a menu entry
(hidden and system files, and `desktop.ini`, are skipped). Folder templates are
listed first, then files, in the order File Explorer itself would list them, and
if the name is already taken ` (2)`, ` (3)`, … is appended. The menu is built
when it's opened, so templates added or removed in the meantime show up without
reloading anything.

The button takes the place of Explorer's New button, which is collapsed (and can
be kept visible). Its **label** and **icon** are configurable: with a label the
familiar chevron (˅) is drawn after the text, and with an empty label - or with
the label turned off - only the icon is shown, without a chevron. An empty icon
setting reuses the icon of Explorer's own New button.

---

### Filename variables

When *Replace variables* is enabled in PowerToys, these are substituted in the
name of the created copy:

| Variable | Meaning |
| --- | --- |
| `$YYYY` | Year, four digits |
| `$YY` | Year, two digits |
| `$MM` | Month, two digits |
| `$DD` | Day, two digits |
| `$hh` | Hour, two digits (24h) |
| `$mm` | Minute, two digits |
| `$ss` | Second, two digits |
| `$PARENT_FOLDER_NAME` | Name of the folder the item is created in |

Unlike New+, variables are replaced in the file *name* only, never inside the
file contents.

---

## Default configuration

Out of the box the mod adds:

* **Open in Terminal** - `wt.exe -d "%path%"`
* **Open in Notepad** - `notepad.exe "%sel%"`
* **Additional** ▾ (dropdown)
  * Open in VS Code - `code.exe "%path%"`
  * Open Paint - `mspaint.exe`
  * Open Calculator - `calc.exe`
  * **Commands** ▸ - `vite`, `npm init`, `npm install`, `npm run dev`,
    `npm run build`, `npm run start`
  * **AI** ▸ - `Claude`, `Codex`

Items whose command isn't installed simply do nothing when clicked (the failure
is written to the mod log), so remove the ones you don't need and add your own.
All of the built-in buttons stay visible by default, and the New+ button is
turned off. Everything is configurable in the mod settings.

---

## How it works

The mod hooks a couple of functions of File Explorer's own WinUI 3 code
(`FileExplorerExtensions.dll`) which run when the command bar is built, and
finds the command bars from there by walking the XAML tree. The configured
buttons are then inserted and the visibility / spacing settings are applied. The
mod also listens for the command bar being rebuilt so the buttons stay in place
across navigation, new tabs and new windows, and it restores the original state
of any element it touches when disabled.

The active tab's folder and selection are resolved through `IShellWindows` /
`IShellBrowser`, off the UI thread, so a slow or unresponsive shell can't block
the command bar.

Keyboard shortcuts are handled by a keyboard hook on the thread of each File
Explorer window, so only key presses in File Explorer windows are seen. No
system-wide hook is used.

Notably, the mod does **not** use XAML Diagnostics
(`InitializeXamlDiagnosticsEx`), since only one XAML diagnostics consumer can be
active in a process at a time. That makes it compatible with mods and tools
which do use it, such as **Windows 11 File Explorer Styler**, ExplorerBlurMica
and TranslucentTB.

File Explorer windows which are already open when the mod is enabled get the
buttons right away. Since there's no public way to get from a window to its XAML
tree, the mod briefly resizes the window's XAML island by one pixel, which makes
File Explorer report the island's XAML root to a hooked handler.

---

### Author & Attribution

* **Created and maintained by DanRotaru ([GitHub](https://github.com/DanRotaru)).**
* Dynamic extension filtering, live parameter tokens (`%path%`, `%sel_each%`, etc.), dynamic UI labels, and the real-time UI selection polling engine contributed by **ArvindSaini978 ([GitHub](https://github.com/ArvindSaini978))**.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showSettingsItem: true
  $name: Show Explorer Command Bar Settings Item
  $description: >-
    Add a cog button to the command bar which opens the Explorer Command Bar
    settings window, an easier way to edit these settings - especially the
    toolbar items and menus.
- settingsItemOrder: -1
  $name: Explorer Command Bar Settings Item Order
  $description: >-
    The position of the settings item among the toolbar items. 0 places it
    before the first item, 1 after the first item, and so on. -1 places it
    after the last item.
- items:
  - - type: button
      $name: Main Item Type
      $options:
      - button: Single button
      - menu: Dropdown menu
    - name: Open in Terminal
      $name: Name
      $description: Shown as the button tooltip.
    - command: wt.exe
      $name: Command
      $description: >-
        The executable to run. For dropdown menus it's only used as an icon
        source. Alternatively, an internal command with the internal: or int:
        prefix (case-insensitive): internal:TogglePreview,
        internal:ToggleDetails, internal:FolderOptions, internal:OpenWith,
        internal:CopyPath, internal:CopyName, internal:CopyDir,
        internal:NewFolder, internal:NewFile or internal:Refresh. For
        NewFolder and NewFile, the parameters set the name.
    - parameters: -d "%path%"
      $name: Parameters
      $description: >-
        Command line parameters. %path% is replaced with the current folder
        path, %sel% with the paths of all selected files/folders (each quoted
        and separated by a space), and %sel_each% runs the command once per
        selected item. If a used placeholder has no value (e.g. no selection,
        or a location like This PC), the command is launched without
        parameters.
    - shortcut: ""
      $name: Keyboard shortcut
      $description: >-
        Runs the item, like a click, when the keys are pressed in a File
        Explorer window, e.g. Ctrl+Shift+T, Alt+N or F9. Needs Ctrl, Alt or
        Win unless the key is F1-F24. Not used for dropdown menus.
    - runAsAdmin: false
      $name: Run as administrator
      $description: >-
        Start the command elevated, after a UAC prompt. Ctrl+Shift+click does
        the same for any item. Not used by internal commands.
    - windowStyle: normal
      $name: Window style
      $description: >-
        How the started program's window is shown. Hidden is meant for console
        commands and scripts; some programs ignore it.
      $options:
      - normal: Normal
      - minimized: Minimized
      - maximized: Maximized
      - hidden: Hidden
    - matchExtensions: ""
      $name: "File extension filter"
      $description: >-
        Comma-separated rules evaluate using OR logic. Use '+' for AND (e.g. files + single).
        Prefix with '-' to exclude. Use 'files', 'folders', 'single', 'multi', 'only txt', or 'hide'.
    - iconGlyph: ""
      $name: Icon glyph or icon path
      $description: >-
        Hex code point of a Segoe Fluent Icons glyph, e.g. E756
        (CommandPrompt) or EC7A. The list of glyphs:
        https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font
        Alternatively, a path to an .exe, .dll or
        .ico file to use its icon, optionally with an icon index, e.g.
        C:\Windows\System32\shell32.dll,3. Leave empty to use the icon of the
        command's executable. Separate two values with | for theme-aware
        icons (Light | Dark, or | Dark).
    - hideIcon: false
      $name: Hide icon
      $description: Don't show an icon for this item.
    - showLabel: false
      $name: Show label
      $description: Show text label next to the icon.
    - labelText: ""
      $name: Custom Label Text
      $description: "Text displayed next to the icon. Supports live tokens: %base%, %name%, %ext%, %count% / %c%, %d_name%, %path%. Use %% for literal %. If empty, the Name field is used."
    - separatorAfter: false
      $name: Vertical separator after
      $description: Show a vertical separator line after this button.
    - subItems:
      - - type: button
          $name: Sub-item Type
          $options:
          - button: Single button
          - menu: Submenu
        - name: ""
          $name: Name
          $description: The menu item text.
        - command: ""
          $name: Command
          $description: >-
            The executable to run. For submenus it's only used as an icon
            source.
        - parameters: ""
          $name: Parameters
          $description: >-
            Command line parameters. %path% is replaced with the current
            folder path, %sel% with the paths of all selected files/folders,
            %sel_each% runs the command once per selected item.
        - shortcut: ""
          $name: Keyboard shortcut
          $description: >-
            Runs the menu item when the keys are pressed in a File Explorer
            window, e.g. Ctrl+Shift+T. Not used for submenus.
        - runAsAdmin: false
          $name: Run as administrator
          $description: >-
            Start the command elevated, after a UAC prompt.
        - windowStyle: normal
          $name: Window style
          $options:
          - normal: Normal
          - minimized: Minimized
          - maximized: Maximized
          - hidden: Hidden
        - iconGlyph: ""
          $name: Icon glyph or icon path
          $description: >-
            Hex code point of a Segoe Fluent Icons glyph
            (https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font),
            or a path to an .exe, .dll or .ico file, optionally with an icon
            index. Leave empty to use the icon of the command's executable.
        - hideIcon: false
          $name: Hide icon
          $description: Don't show an icon for this menu item.
        - separatorAfter: false
          $name: Separator after
          $description: Show a separator line below this menu item.
        - subItems:
          - - name: ""
              $name: Name
              $description: The menu item text.
            - command: ""
              $name: Command
            - parameters: ""
              $name: Parameters
              $description: >-
                Command line parameters. %path% is replaced with the current
                folder path, %sel% with the paths of all selected
                files/folders, %sel_each% runs the command once per selected
                item.
            - shortcut: ""
              $name: Keyboard shortcut
              $description: >-
                Runs the menu item when the keys are pressed in a File
                Explorer window, e.g. Ctrl+Shift+T.
            - runAsAdmin: false
              $name: Run as administrator
              $description: >-
                Start the command elevated, after a UAC prompt.
            - windowStyle: normal
              $name: Window style
              $options:
              - normal: Normal
              - minimized: Minimized
              - maximized: Maximized
              - hidden: Hidden
            - iconGlyph: ""
              $name: Icon glyph or icon path
              $description: >-
                Hex code point of a Segoe Fluent Icons glyph
                (https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font),
                or a path to an .exe, .dll or .ico file, optionally with an
                icon index. Leave empty to use the icon of the command's
                executable.
            - hideIcon: false
              $name: Hide icon
              $description: Don't show an icon for this menu item.
            - separatorAfter: false
              $name: Separator after
              $description: Show a separator line below this menu item.
          $name: Submenu items
          $description: The entries of the submenu, for the submenu type.
      $name: Menu items
      $description: The entries of the dropdown menu, for the dropdown type.
  - - type: button
    - name: Open in Notepad
    - command: notepad.exe
    - parameters: '"%sel%"'
    - iconGlyph: 'shell:AppsFolder\Microsoft.WindowsNotepad_8wekyb3d8bbwe!App'
    - separatorAfter: false
  - - type: menu
    - name: Additional
    - command: ""
    - parameters: ""
    - iconGlyph: E81E
    - separatorAfter: false
    - subItems:
      - - type: button
        - name: Open in VS Code
        - command: code.exe
        - parameters: '"%path%"'
        - iconGlyph: ""
        - separatorAfter: false
        - subItems:
          - - name: ""
            - command: ""
            - parameters: ""
            - iconGlyph: ""
            - hideIcon: false
            - separatorAfter: false
      - - type: button
        - name: Open Paint
        - command: mspaint.exe
        - parameters: ""
        - iconGlyph: ""
        - separatorAfter: false
      - - type: button
        - name: Open Calculator
        - command: calc.exe
        - parameters: ""
        - iconGlyph: 'shell:AppsFolder\Microsoft.WindowsCalculator_8wekyb3d8bbwe!App'
        - separatorAfter: true
      - - type: menu
        - name: Commands
        - command: ""
        - parameters: ""
        - iconGlyph: EC7A
        - separatorAfter: false
        - subItems:
          - - name: vite
            - command: cmd.exe
            - parameters: /k vite
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: true
          - - name: npm init
            - command: cmd.exe
            - parameters: /k npm init
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm install
            - command: cmd.exe
            - parameters: /k npm install
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm run dev
            - command: cmd.exe
            - parameters: /k npm run dev
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm run build
            - command: cmd.exe
            - parameters: /k npm run build
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: npm run start
            - command: cmd.exe
            - parameters: /k npm run start
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
      - - type: menu
        - name: AI
        - command: ""
        - parameters: ""
        - iconGlyph: E794
        - separatorAfter: false
        - subItems:
          - - name: Claude
            - command: cmd.exe
            - parameters: /k claude
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
          - - name: Codex
            - command: cmd.exe
            - parameters: /k codex
            - iconGlyph: ""
            - hideIcon: true
            - separatorAfter: false
  $name: Toolbar items
  $description: >-
    Each toolbar item is either a single button or a dropdown menu. Use the
    plus button to add another one.
- placeOnSecondaryBar: false
  $name: Place buttons on the right side
  $description: >-
    Show custom buttons on the right side of File Explorer (next to Details
    and Preview pane toggles) instead of the main left toolbar.
- disabledInsteadOfHidden: false
  $name: Disable buttons instead of hiding them
  $description: Gray out inactive buttons instead of removing them from the toolbar.
- showMenuChevrons: false
  $name: Show chevrons on dropdowns
  $description: >-
    Show a small chevron next to custom toolbar buttons which open a dropdown
    menu.
- hideDefaultButtonLabels:
  - new: false
    $name: New
  - sort: false
    $name: Sort
  - view: false
    $name: View
  - details: false
    $name: Details pane toggle
  $name: Hide built-in button labels
  $description: >-
    Show only the icon of these built-in buttons, without their text label.
- hideDefaultButtons:
  - new: false
    $name: New
  - separatorAfterNew: false
    $name: Vertical separator after New
  - cut: false
    $name: Cut
  - copy: false
    $name: Copy
  - paste: false
    $name: Paste
  - rename: false
    $name: Rename
  - share: false
    $name: Share
  - delete: false
    $name: Delete
  - separatorAfterDelete: false
    $name: Vertical separator after Delete
  - sort: false
    $name: Sort
  - view: false
    $name: View
  - separatorAfterView: false
    $name: Vertical separator after View
  - moreOptions: false
    $name: See more (the three dots menu)
  - setAsBackground: false
    $name: Set as background
    $description: Shown when an image file is selected.
  - rotateLeft: false
    $name: Rotate left
    $description: Shown when an image file is selected.
  - rotateRight: false
    $name: Rotate right
    $description: Shown when an image file is selected.
  - extractAll: false
    $name: Extract all
    $description: Shown when a zip or other archive file is selected.
  - details: false
    $name: Details pane toggle
  $name: Hide default toolbar buttons
  $description: >-
    Hide the built-in buttons of the File Explorer command bar. The vertical
    separator of the contextual commands (Set as background, Rotate left,
    Rotate right, Extract all) is hidden along with them, once none of them is
    left to show.
- defaultButtonOrder: ""
  $name: Built-in button order
  $description: >-
    The order of the built-in buttons, separated by commas, with | for a
    separator, e.g. "sort, view | new | cut, copy, paste, rename, share,
    delete". The names are new, cut, copy, paste, rename, share, delete, sort
    and view. Buttons which aren't listed come last. Leave empty for File
    Explorer's own order.
- newPlus:
  - enabled: false
    $name: Replace New with New+
    $description: >-
      Put a New+ button in the place of Explorer's New button. Its dropdown
      lists the templates of the PowerToys New+ utility instead of Explorer's
      fixed list of file types, and clicking one creates a copy of it in the
      current folder. PowerToys isn't required - the mod copies the templates
      itself and only reads PowerToys' New+ settings, if there are any.
  - showLabel: true
    $name: Show the button label
    $description: >-
      Show the label next to the icon, the way Explorer's own New button does,
      with a chevron after the text. When disabled, only the icon is shown, no
      chevron is drawn, and the label becomes the button's tooltip.
  - buttonLabel: New
    $name: Button label
    $description: >-
      The text of the button, also used as its tooltip when the label is
      hidden. Leave empty for an icon-only button without a chevron.
  - buttonIcon: ""
    $name: Button icon glyph or icon path
    $description: >-
      Leave empty to reuse the icon of Explorer's own New button.
      Alternatively, a hex code point of a Segoe Fluent Icons glyph, e.g. E710
      (https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font),
      or a path to an .exe, .dll or .ico file, optionally with an icon index,
      e.g. C:\Windows\System32\shell32.dll,3.
  - templateFolder: ""
    $name: Templates folder
    $description: >-
      The folder to read the templates from. Leave empty to use the folder
      configured in PowerToys, or the New+ default location
      (%LOCALAPPDATA%\Microsoft\PowerToys\NewPlus\Templates) if PowerToys isn't
      installed.
  - showIcons: true
    $name: Show template icons
    $description: >-
      Show the shell icon of each template in the menu. Disable it if opening
      the menu feels slow with many templates.
  - showTemplatesFolderItem: true
    $name: Add an "Open templates folder" entry
    $description: >-
      Append an entry which opens the templates folder, creating it if it
      doesn't exist yet.
  - keepOriginalNewButton: false
    $name: Keep Explorer's New button
    $description: >-
      Show the built-in New button next to the New+ button instead of hiding
      it.
  $name: Replace New to New+
  $description: >-
    Replace Explorer's New button with a New+ button which creates files and
    folders from your own templates, the way the PowerToys New+ utility does.
    The label and the icon of the button are up to you.
- contextMenuItem:
  - enabled: false
    $name: Add the context menu item
    $description: >-
      Add another command bar item which opens the shell context menu of the
      current selection or folder, like right-clicking in File Explorer.
  - useNilesoftShell: false
    $name: Let File Explorer show the menu (for Nilesoft Shell)
    $description: >-
      Ask the file list to show its own context menu instead of building the
      classic shell menu. Enable this for Nilesoft Shell, which replaces the
      menu from inside Explorer. The menu is positioned by Explorer and the
      active selection or folder background automatically.
  - showLabel: false
    $name: Show the item label
    $description: >-
      Show the label next to the icon. When disabled, only the icon is shown
      and the label becomes the tooltip.
  - buttonLabel: Context menu
    $name: Item label
    $description: The label and tooltip of the context menu item.
  - buttonIcon: E8FD
    $name: Item icon glyph or icon path
    $description: >-
      A Segoe Fluent Icons hex code point, or an .exe, .dll or .ico path with
      an optional icon index. Leave empty for the default glyph.
  $name: Context menu item
  $description: >-
    Add a configurable item which expands the real File Explorer shell context
    menu for the active tab.
- openMenuOnHover: false
  $name: Open menus on hover
  $description: >-
    Open dropdown menus by hovering over the button instead of clicking it.
    Applies to the New+ button as well.
- menuHoverDelay: 400
  $name: Hover delay (milliseconds)
  $description: >-
    How long the cursor has to stay over the button before the menu opens.
    Only used when "Open menus on hover" is enabled.
- itemSpacing: -1
  $name: Item spacing (pixels at 100% scaling)
  $description: >-
    Horizontal spacing between the command bar buttons. -1 leaves the default
    spacing unchanged; 0 places the buttons right next to each other; 5 means 5
    pixels between buttons, and so on. The value is in effective pixels, so at
    150% display scaling it results in 1.5 times as many physical pixels. Note
    that any value other than -1 also removes the built-in buttons' minimum
    width, which shrinks their click area down to the icon.
*/
// ==/WindhawkModSettings==

// clang-format on

#include <windhawk_utils.h>
#include <windows.h>

#include <commctrl.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <servprov.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

std::atomic<bool> g_unloading;

static constexpr CLSID kCLSID_ShellWindows = {
    0x9ba05972,
    0xf6a8,
    0x11cf,
    {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};

static constexpr GUID kSID_STopLevelBrowser = {
    0x4c96be40,
    0x915c,
    0x11cf,
    {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

struct ActionItem {
    std::wstring name;
    std::wstring labelText;
    std::wstring command;
    std::wstring parameters;
    std::wstring shortcut;
    std::wstring matchExtensions;
    std::wstring icon;
    bool hideIcon = false;
    bool showLabel = false;
    bool isMenu = false;
    bool separatorAfter = false;
    bool runAsAdmin = false;
    // One of kWindowStyles, e.g. L"normal".
    std::wstring windowStyle;
    std::vector<ActionItem> subItems;

    // Helper: Returns labelText if specified, otherwise falls back to name
    std::wstring GetDisplayLabel() const {
        return labelText.empty() ? name : labelText;
    }
};

// A key combination: MOD_* flags and a virtual-key code.
struct KeyboardShortcut {
    UINT modifiers = 0;
    UINT vk = 0;
};

struct WindowStyleDef {
    PCWSTR settingValue;
    PCWSTR title;
    int showCommand;
};

constexpr WindowStyleDef kWindowStyles[] = {
    {L"normal", L"Normal", SW_SHOWNORMAL},
    {L"minimized", L"Minimized", SW_SHOWMINNOACTIVE},
    {L"maximized", L"Maximized", SW_SHOWMAXIMIZED},
    {L"hidden", L"Hidden", SW_HIDE},
};

int WindowStyleIndex(std::wstring_view value) {
    for (int i = 0; i < (int)ARRAYSIZE(kWindowStyles); i++) {
        if (value.size() == wcslen(kWindowStyles[i].settingValue) &&
            _wcsnicmp(value.data(), kWindowStyles[i].settingValue,
                      value.size()) == 0)
            return i;
    }
    return 0;
}

struct ShortcutBinding {
    KeyboardShortcut shortcut;
    ActionItem item;
};

struct DefaultButtonDef {
    PCWSTR settingKey;
    PCWSTR svgFileName;
};

constexpr DefaultButtonDef kDefaultButtons[] = {
    {L"new", L"windows.newitem.svg"},
    {L"cut", L"windows.cut.svg"},
    {L"copy", L"windows.copy.svg"},
    {L"paste", L"windows.paste.svg"},
    {L"rename", L"windows.rename.svg"},
    {L"share", L"windows.modernshare.svg"},
    {L"delete", L"windows.ribbondelete.svg"},
    {L"sort", L"sortby.svg"},
    {L"view", L"view.svg"},
    {L"setAsBackground", L"windows.setdesktopwallpaper.svg"},
    {L"rotateLeft", L"windows.rotate270.svg"},
    {L"rotateRight", L"windows.rotate90.svg"},
    {L"extractAll", L"windows.compressedfile.extract.svg"},
};

constexpr int kDefaultButtonCount = ARRAYSIZE(kDefaultButtons);
constexpr int kNewButtonIndex = 0;
constexpr int kDeleteButtonIndex = 6;
constexpr int kSortButtonIndex = 7;
constexpr int kViewButtonIndex = 8;

// The built-in buttons shown with a text label, which can be hidden.
constexpr int kLabeledDefaultButtons[] = {kNewButtonIndex, kSortButtonIndex,
                                          kViewButtonIndex};

// The built-in buttons which can be reordered: New to View, the ones which
// are always in the command bar. The contextual commands come and go with
// the selection, so they stay where File Explorer puts them.
constexpr int kReorderableButtonCount = kViewButtonIndex + 1;

// A token of the built-in button order: a button index, or a separator.
constexpr int kOrderSeparator = -1;

// File Explorer's own order: New | Cut ... Delete | Sort View.
std::vector<int> DefaultButtonOrderTokens() {
    return {kNewButtonIndex, kOrderSeparator, 1, 2, 3, 4, 5,
            kDeleteButtonIndex, kOrderSeparator, kSortButtonIndex,
            kViewButtonIndex};
}

// Parses e.g. "sort, view | new | cut, copy, paste, rename, share, delete".
// Buttons can be separated by commas or spaces, and | places a separator.
// Unknown names and repeated buttons are ignored, and buttons which aren't
// listed are put at the end in their usual order. Returns an empty list,
// meaning File Explorer's own order, if no button is listed.
std::vector<int> ParseDefaultButtonOrder(std::wstring_view text) {
    std::vector<int> tokens;
    bool listed[kReorderableButtonCount] = {};
    bool anyListed = false;
    std::wstring word;
    auto flushWord = [&]() {
        if (word.empty())
            return;
        for (int i = 0; i < kReorderableButtonCount; i++) {
            if (_wcsicmp(word.c_str(), kDefaultButtons[i].settingKey) == 0) {
                if (!listed[i]) {
                    listed[i] = true;
                    anyListed = true;
                    tokens.push_back(i);
                }
                break;
            }
        }
        word.clear();
    };

    for (WCHAR c : text) {
        if (iswalnum(c)) {
            word += c;
            continue;
        }
        flushWord();
        if (c == L'|')
            tokens.push_back(kOrderSeparator);
    }
    flushWord();

    if (!anyListed)
        return {};
    for (int i = 0; i < kReorderableButtonCount; i++) {
        if (!listed[i])
            tokens.push_back(i);
    }
    return tokens;
}

// The reverse of ParseDefaultButtonOrder, e.g. "new | cut, copy | sort".
// Returns an empty string for File Explorer's own order.
std::wstring FormatDefaultButtonOrder(std::vector<int> const& tokens) {
    if (tokens.empty() || tokens == DefaultButtonOrderTokens())
        return std::wstring();

    std::wstring text;
    bool afterButton = false;
    for (int token : tokens) {
        if (token == kOrderSeparator) {
            text += text.empty() ? L"|" : L" |";
            afterButton = false;
            continue;
        }
        if (token < 0 || token >= kReorderableButtonCount)
            continue;
        if (!text.empty())
            text += afterButton ? L", " : L" ";
        text += kDefaultButtons[token].settingKey;
        afterButton = true;
    }
    return text;
}

struct NewPlusSettings {
    bool enabled = false;
    bool showLabel = true;
    std::wstring buttonLabel = L"New";
    std::wstring buttonIcon;
    std::wstring templateFolder;
    bool showIcons = true;
    bool showTemplatesFolderItem = true;
    bool keepOriginalNewButton = false;
};

struct ContextMenuItemSettings {
    bool enabled = false;
    bool useNilesoftShell = false;
    bool showLabel = false;
    std::wstring buttonLabel = L"Context menu";
    std::wstring buttonIcon;
};

struct {
    std::mutex mutex;
    bool placeOnSecondaryBar = false;
    bool disabledInsteadOfHidden = false;
    bool openMenuOnHover = false;
    int menuHoverDelay = 400;
    bool showMenuChevrons = false;
    bool hideDefaultButtons[kDefaultButtonCount] = {};
    bool hideDefaultButtonLabels[kDefaultButtonCount] = {};
    bool hideSeparatorAfterButton[kDefaultButtonCount] = {};
    bool hideMoreButton = false;
    bool hideDetailsButton = false;
    bool hideDetailsButtonLabel = false;
    std::wstring defaultButtonOrderText;
    // Parsed defaultButtonOrderText, empty for File Explorer's own order.
    std::vector<int> defaultButtonOrder;
    int itemSpacing = -1;
    bool showSettingsItem = true;
    int settingsItemOrder = -1;
    std::vector<ActionItem> items;
    std::vector<ShortcutBinding> shortcuts;
    NewPlusSettings newPlus;
    ContextMenuItemSettings contextMenuItem;
} g_settings;

// clang-format off

#pragma region winrt_hpp
#include <Unknwn.h>
#undef GetCurrentTime
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

namespace wdj = winrt::Windows::Data::Json;
namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace mud = winrt::Microsoft::UI::Dispatching;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxd = winrt::Microsoft::UI::Xaml::Documents;
namespace muxh = winrt::Microsoft::UI::Xaml::Hosting;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
#pragma endregion

// clang-format on

constexpr PCWSTR kButtonNamePrefix = L"WindhawkActionButton";
constexpr PCWSTR kNewPlusButtonName = L"WindhawkNewPlusButton";
constexpr PCWSTR kContextMenuButtonName = L"WindhawkContextMenuButton";
constexpr PCWSTR kSettingsButtonName = L"WindhawkSettingsButton";

struct CommandBarEntry {
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::event_token vectorChangedToken{};
    winrt::event_token themeChangedToken{};

    // Selection Polling State
    mux::DispatcherTimer selectionTimer{nullptr};
    winrt::event_token selectionTickToken{};
    HWND cachedTabWnd = nullptr;
    winrt::com_ptr<IShellBrowser> cachedBrowser{nullptr};
    winrt::com_ptr<IShellView> cachedShellView{nullptr};
    int lastSelectionCount = -1;
    int lastFocusedItem = -1;
};

void StopSelectionTimer(CommandBarEntry& entry) {
    if (entry.selectionTimer) {
        try {
            entry.selectionTimer.Stop();
            entry.selectionTimer.Tick(entry.selectionTickToken);
        } catch (...) {
        }
        entry.selectionTimer = nullptr;
        entry.selectionTickToken = {};
    }
    entry.cachedBrowser = nullptr;
    entry.cachedShellView = nullptr;
    entry.cachedTabWnd = nullptr;
    entry.lastSelectionCount = -1;
    entry.lastFocusedItem = -1;
}

thread_local std::vector<CommandBarEntry> g_entries;
thread_local bool g_threadScanned;

struct TrackedRevoker {
    winrt::weak_ref<wf::IInspectable> source;
    std::function<void()> revoke;
};

thread_local std::vector<TrackedRevoker> g_revokers;
constexpr size_t kRevokersPruneMin = 64;
thread_local size_t g_revokersPruneAt = kRevokersPruneMin;

template <typename T, typename Revoker>
void TrackRevoker(T const& source, Revoker&& revoker) {
    if (g_revokers.size() >= g_revokersPruneAt) {
        std::erase_if(g_revokers, [](TrackedRevoker const& tracked) {
            return !tracked.source.get();
        });
        g_revokersPruneAt = std::max(kRevokersPruneMin, g_revokers.size() * 2);
    }

    auto held =
        std::make_shared<std::decay_t<Revoker>>(std::forward<Revoker>(revoker));
    g_revokers.push_back({winrt::weak_ref<wf::IInspectable>{source},
                          [held]() { held->revoke(); }});
}

void RevokeHandlersForCurrentThread() {
    std::vector<TrackedRevoker> taken;
    taken.swap(g_revokers);
    g_revokersPruneAt = kRevokersPruneMin;

    for (auto const& tracked : taken) {
        try {
            tracked.revoke();
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

struct ManagedElement {
    winrt::weak_ref<mux::UIElement> element;
    bool hasOriginalVisibility = false;
    mux::Visibility originalVisibility = mux::Visibility::Visible;
    bool hasOriginalSpacing = false;
    mux::Thickness originalMargin{};
    double originalMinWidth = 0;
    bool hasOriginalLabelPosition = false;
    muxc::CommandBarLabelPosition originalLabelPosition =
        muxc::CommandBarLabelPosition::Default;
    bool addedToolTip = false;
    // Dropdown chevrons of a built-in button, collapsed along with its label.
    bool wantChevronsHidden = false;
    bool chevronLoadedHooked = false;
    struct HiddenChevron {
        winrt::weak_ref<mux::UIElement> element;
        mux::Visibility originalVisibility = mux::Visibility::Visible;
        double originalOpacity = 1;
        int64_t visibilityToken = 0;
    } hiddenChevron;
    bool hasOriginalOverflow = false;
    muxc::CommandBarOverflowButtonVisibility originalOverflow =
        muxc::CommandBarOverflowButtonVisibility::Auto;
    bool watched = false;
    int64_t visibilityToken = 0;
    // For a command bar whose built-in buttons were reordered: their
    // original order, with the separators between them.
    std::vector<winrt::weak_ref<mux::UIElement>> originalButtonOrder;
    // A built-in separator left over by the custom button order, hidden.
    bool unusedSeparator = false;
};

thread_local std::vector<ManagedElement> g_managedElements;

ManagedElement* FindManagedElement(mux::UIElement const& element) {
    for (auto& entry : g_managedElements) {
        if (entry.element.get() == element) {
            return &entry;
        }
    }
    return nullptr;
}

ManagedElement& GetManagedElement(mux::UIElement const& element) {
    for (auto it = g_managedElements.begin(); it != g_managedElements.end();) {
        if (!it->element.get()) {
            it = g_managedElements.erase(it);
        } else {
            ++it;
        }
    }

    if (auto* entry = FindManagedElement(element)) {
        return *entry;
    }

    g_managedElements.push_back({winrt::make_weak(element)});
    return g_managedElements.back();
}

std::wstring ExpandEnvVars(std::wstring const& str) {
    if (str.empty())
        return str;
    WCHAR buffer[MAX_PATH * 2];
    DWORD length =
        ExpandEnvironmentStringsW(str.c_str(), buffer, ARRAYSIZE(buffer));
    if (length == 0 || length > ARRAYSIZE(buffer))
        return str;
    return buffer;
}

std::wstring ToLower(std::wstring str) {
    for (auto& c : str)
        c = towlower(c);
    return str;
}

std::wstring TrimQuotesAndSpaces(std::wstring str) {
    size_t first = str.find_first_not_of(L" \t");
    if (first == std::wstring::npos)
        return std::wstring();
    size_t last = str.find_last_not_of(L" \t");
    str = str.substr(first, last - first + 1);
    if (str.size() >= 2 && str.front() == L'"' && str.back() == L'"') {
        str = str.substr(1, str.size() - 2);
    }
    return str;
}

std::wstring JoinPath(std::wstring const& folder, std::wstring const& name) {
    std::wstring result = folder;
    if (!result.empty() && result.back() != L'\\' && result.back() != L'/') {
        result += L'\\';
    }
    return result + name;
}

bool DirectoryExists(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring ResolveCommandPath(std::wstring const& command) {
    std::wstring expanded = ExpandEnvVars(command);
    if (expanded.find(L'\\') != std::wstring::npos)
        return expanded;

    WCHAR resolved[MAX_PATH];
    if (SearchPathW(nullptr, expanded.c_str(), L".exe", ARRAYSIZE(resolved),
                    resolved, nullptr)) {
        return resolved;
    }

    std::wstring keyPath =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + expanded;
    if (!ToLower(expanded).ends_with(L".exe"))
        keyPath += L".exe";

    for (HKEY rootKey : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        WCHAR buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (RegGetValueW(rootKey, keyPath.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, buffer, &size) == ERROR_SUCCESS &&
            buffer[0]) {
            std::wstring appPath = ExpandEnvVars(buffer);
            if (appPath.size() >= 2 && appPath.front() == L'"' &&
                appPath.back() == L'"') {
                appPath = appPath.substr(1, appPath.size() - 2);
            }
            return appPath;
        }
    }

    return expanded;
}

class ComApartmentScope {
   public:
    explicit ComApartmentScope(DWORD coInit)
        : m_hr(CoInitializeEx(nullptr, coInit)) {}
    ~ComApartmentScope() {
        if (SUCCEEDED(m_hr))
            CoUninitialize();
    }
    ComApartmentScope(const ComApartmentScope&) = delete;
    ComApartmentScope& operator=(const ComApartmentScope&) = delete;

   private:
    HRESULT m_hr;
};

struct ExplorerContext {
    std::wstring folderPath;
    std::wstring selectedPath;
    std::wstring folderDisplayName;
    std::vector<std::wstring> allSelectedPaths;
    winrt::com_ptr<IShellView> shellView;
    size_t selectionCount = 0;
};

winrt::com_ptr<IShellBrowser> GetActiveShellBrowser(HWND hExplorerWnd) {
    winrt::com_ptr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put()))) ||
        !shellWindows)
        return nullptr;

    long count = 0;
    shellWindows->get_Count(&count);

    HWND hActiveTabWnd = hExplorerWnd
                             ? FindWindowExW(hExplorerWnd, nullptr,
                                             L"ShellTabWindowClass", nullptr)
                             : nullptr;

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(index, dispatch.put())) || !dispatch)
            continue;

        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        if (!webBrowser)
            continue;

        SHANDLE_PTR hWndRaw = 0;
        if (FAILED(webBrowser->get_HWND(&hWndRaw)) ||
            (hExplorerWnd && (HWND)hWndRaw != hExplorerWnd))
            continue;

        auto serviceProvider = dispatch.try_as<IServiceProvider>();
        if (!serviceProvider)
            continue;

        winrt::com_ptr<IShellBrowser> shellBrowser;
        if (FAILED(serviceProvider->QueryService(
                kSID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))) ||
            !shellBrowser)
            continue;

        HWND hTabWnd = nullptr;
        if (SUCCEEDED(shellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
            if (hActiveTabWnd) {
                if (hTabWnd != hActiveTabWnd)
                    continue;
            } else if (!IsWindowVisible(hTabWnd)) {
                continue;
            }
        }
        return shellBrowser;
    }
    return nullptr;
}

winrt::com_ptr<IShellView> GetActiveShellView(HWND hExplorerWnd) {
    winrt::com_ptr<IShellWindows> shellWindows;
    HRESULT hr = CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(shellWindows.put()));
    if (FAILED(hr) || !shellWindows) {
        Wh_Log(L"CoCreateInstance(ShellWindows) failed: %08X", hr);
        return nullptr;
    }

    long count = 0;
    shellWindows->get_Count(&count);

    HWND hActiveTabWnd = hExplorerWnd
                             ? FindWindowExW(hExplorerWnd, nullptr,
                                             L"ShellTabWindowClass", nullptr)
                             : nullptr;

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(index, dispatch.put())) || !dispatch)
            continue;

        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        if (!webBrowser)
            continue;

        SHANDLE_PTR hWndRaw = 0;
        if (FAILED(webBrowser->get_HWND(&hWndRaw)) ||
            (hExplorerWnd && (HWND)hWndRaw != hExplorerWnd))
            continue;

        auto serviceProvider = dispatch.try_as<IServiceProvider>();
        if (!serviceProvider)
            continue;

        winrt::com_ptr<IShellBrowser> shellBrowser;
        if (FAILED(serviceProvider->QueryService(
                kSID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))) ||
            !shellBrowser)
            continue;

        HWND hTabWnd = nullptr;
        if (SUCCEEDED(shellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
            if (hActiveTabWnd) {
                if (hTabWnd != hActiveTabWnd)
                    continue;
            } else if (!IsWindowVisible(hTabWnd)) {
                continue;
            }
        }

        winrt::com_ptr<IShellView> shellView;
        if (SUCCEEDED(shellBrowser->QueryActiveShellView(shellView.put())) &&
            shellView) {
            return shellView;
        }
    }

    return nullptr;
}

bool ShellViewHasSelection(winrt::com_ptr<IShellView> const& shellView) {
    auto folderView = shellView.try_as<IFolderView>();
    if (!folderView)
        return false;
    int count = 0;
    return SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &count)) &&
           count > 0;
}

struct FindWindowByClassParam {
    PCWSTR className;
    HWND result;
};

HWND FindDescendantWindow(HWND hParentWnd, PCWSTR className) {
    FindWindowByClassParam param{className, nullptr};
    EnumChildWindows(
        hParentWnd,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& p = *(FindWindowByClassParam*)lParam;
            WCHAR buffer[64];
            if (GetClassNameW(hWnd, buffer, ARRAYSIZE(buffer)) &&
                _wcsicmp(buffer, p.className) == 0) {
                p.result = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&param);
    return param.result;
}

HWND FindShellViewWindow(HWND hExplorerWnd) {
    HWND hTabWnd =
        FindWindowExW(hExplorerWnd, nullptr, L"ShellTabWindowClass", nullptr);
    if (hTabWnd) {
        if (HWND hViewWnd = FindDescendantWindow(hTabWnd, L"SHELLDLL_DefView"))
            return hViewWnd;
    }
    return FindDescendantWindow(hExplorerWnd, L"SHELLDLL_DefView");
}

bool RequestShellViewContextMenu(HWND hExplorerWnd) {
    HWND hViewWnd = FindShellViewWindow(hExplorerWnd);
    if (!hViewWnd) {
        Wh_Log(L"No shell view window for %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return false;
    }
    return PostMessageW(hViewWnd, WM_CONTEXTMENU, (WPARAM)hViewWnd,
                        (LPARAM)-1) != FALSE;
}

winrt::com_ptr<IContextMenu> GetShellContextMenu(HWND hExplorerWnd,
                                                 bool* isItemMenu) {
    *isItemMenu = false;
    auto shellView = GetActiveShellView(hExplorerWnd);
    if (!shellView) {
        Wh_Log(L"No shell view for window %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return nullptr;
    }

    *isItemMenu = ShellViewHasSelection(shellView);
    UINT viewObject = *isItemMenu ? SVGIO_SELECTION : SVGIO_BACKGROUND;

    winrt::com_ptr<IContextMenu> contextMenu;
    HRESULT hr = shellView->GetItemObject(viewObject, __uuidof(IContextMenu),
                                          contextMenu.put_void());
    if (FAILED(hr) || !contextMenu) {
        Wh_Log(L"GetItemObject(%u) failed: %08X", viewObject, hr);
        return nullptr;
    }
    return contextMenu;
}

constexpr UINT kContextMenuFirstCmdId = 1;
constexpr UINT kContextMenuLastCmdId = 0x7FFF;
thread_local IContextMenu2* g_trackedContextMenu2;
thread_local IContextMenu3* g_trackedContextMenu3;
thread_local bool g_contextMenuIsOpen;
std::atomic<int> g_openContextMenuCount;

struct OpenContextMenuScope {
    OpenContextMenuScope() { g_openContextMenuCount++; }
    ~OpenContextMenuScope() { g_openContextMenuCount--; }
    OpenContextMenuScope(const OpenContextMenuScope&) = delete;
    OpenContextMenuScope& operator=(const OpenContextMenuScope&) = delete;
};

LRESULT CALLBACK ContextMenuOwnerWndProc(HWND hWnd,
                                         UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam) {
    switch (uMsg) {
        case WM_INITMENUPOPUP:
        case WM_DRAWITEM:
        case WM_MEASUREITEM:
            if (g_trackedContextMenu3) {
                LRESULT result = 0;
                if (SUCCEEDED(g_trackedContextMenu3->HandleMenuMsg2(
                        uMsg, wParam, lParam, &result)))
                    return result;
            } else if (g_trackedContextMenu2 &&
                       SUCCEEDED(g_trackedContextMenu2->HandleMenuMsg(
                           uMsg, wParam, lParam))) {
                return uMsg == WM_INITMENUPOPUP ? 0 : TRUE;
            }
            break;
        case WM_MENUCHAR:
            if (g_trackedContextMenu3) {
                LRESULT result = 0;
                if (SUCCEEDED(g_trackedContextMenu3->HandleMenuMsg2(
                        uMsg, wParam, lParam, &result)) &&
                    result)
                    return result;
            }
            break;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

std::wstring ContextMenuOwnerClassName() {
    return std::wstring(L"WindhawkContextMenuOwner_") + WH_MOD_ID;
}

std::atomic<bool> g_contextMenuOwnerClassRegistered;
std::mutex g_contextMenuOwnersMutex;
std::unordered_map<DWORD, HWND> g_contextMenuOwners;

bool IsContextMenuOwnerWindow(DWORD threadId, HWND hWnd) {
    if (!IsWindow(hWnd) || GetWindowThreadProcessId(hWnd, nullptr) != threadId)
        return false;
    WCHAR className[128];
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, ContextMenuOwnerClassName().c_str()) == 0;
}

void RegisterContextMenuOwnerClass() {
    std::wstring className = ContextMenuOwnerClassName();
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ContextMenuOwnerWndProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.lpszClassName = className.c_str();
    if (!RegisterClassExW(&wc)) {
        DWORD error = GetLastError();
        Wh_Log(L"RegisterClassEx failed: %u%s", error,
               error == ERROR_CLASS_ALREADY_EXISTS
                   ? L" - context menu class already exists"
                   : L"");
        return;
    }
    g_contextMenuOwnerClassRegistered = true;
}

HWND EnsureContextMenuOwnerWindow() {
    DWORD threadId = GetCurrentThreadId();
    {
        std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
        auto it = g_contextMenuOwners.find(threadId);
        if (it != g_contextMenuOwners.end() &&
            IsContextMenuOwnerWindow(threadId, it->second)) {
            return it->second;
        }
        if (it != g_contextMenuOwners.end())
            g_contextMenuOwners.erase(it);
    }

    if (!g_contextMenuOwnerClassRegistered)
        return nullptr;

    HWND hWnd = CreateWindowExW(0, ContextMenuOwnerClassName().c_str(), nullptr,
                                0, 0, 0, 0, 0, nullptr, nullptr,
                                GetCurrentModuleHandle(), nullptr);
    if (!hWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
    g_contextMenuOwners[threadId] = hWnd;
    return hWnd;
}

void DismissOpenContextMenus() {
    for (int i = 0; g_openContextMenuCount > 0; i++) {
        {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            for (auto const& [threadId, hWnd] : g_contextMenuOwners) {
                if (IsContextMenuOwnerWindow(threadId, hWnd))
                    PostMessageW(hWnd, WM_CANCELMODE, 0, 0);
            }
        }
        if (i > 0 && i % 100 == 0) {
            Wh_Log(L"Still waiting for %d shell context menu(s)",
                   g_openContextMenuCount.load());
        }
        Sleep(10);
    }
}

void DestroyContextMenuOwnerWindowForCurrentThread() {
    if (g_contextMenuIsOpen)
        return;
    HWND hWnd = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
        auto it = g_contextMenuOwners.find(GetCurrentThreadId());
        if (it == g_contextMenuOwners.end())
            return;
        hWnd = it->second;
        g_contextMenuOwners.erase(it);
    }
    DestroyWindow(hWnd);
}

void InvokeShellContextMenuCommand(
    winrt::com_ptr<IContextMenu> const& contextMenu,
    UINT cmdId,
    HWND hExplorerWnd,
    POINT point) {
    CMINVOKECOMMANDINFOEX info{};
    info.cbSize = sizeof(info);
    info.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE;
    info.hwnd = hExplorerWnd;
    info.lpVerb = (LPCSTR)(UINT_PTR)(cmdId - kContextMenuFirstCmdId);
    info.lpVerbW = (LPCWSTR)(UINT_PTR)(cmdId - kContextMenuFirstCmdId);
    info.nShow = SW_SHOWNORMAL;
    info.ptInvoke = point;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        info.fMask |= CMIC_MASK_CONTROL_DOWN;
    if (GetKeyState(VK_SHIFT) & 0x8000)
        info.fMask |= CMIC_MASK_SHIFT_DOWN;
    HRESULT hr = contextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&info);
    if (FAILED(hr))
        Wh_Log(L"InvokeCommand failed: %08X", hr);
}

void ShowShellContextMenu(HWND hExplorerWnd, POINT point) {
    if (g_contextMenuIsOpen || g_unloading)
        return;
    OpenContextMenuScope openScope;

    bool useNilesoftShell;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        useNilesoftShell = g_settings.contextMenuItem.useNilesoftShell;
    }

    if (useNilesoftShell && RequestShellViewContextMenu(hExplorerWnd))
        return;

    HWND hOwnerWnd = EnsureContextMenuOwnerWindow();
    if (!hOwnerWnd)
        return;

    ComApartmentScope comScope(COINIT_APARTMENTTHREADED |
                               COINIT_DISABLE_OLE1DDE);

    bool isItemMenu = false;
    auto contextMenu = GetShellContextMenu(hExplorerWnd, &isItemMenu);
    if (!contextMenu)
        return;

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu)
        return;

    UINT flags = isItemMenu ? CMF_CANRENAME : CMF_NORMAL;
    if (GetKeyState(VK_SHIFT) & 0x8000)
        flags |= CMF_EXTENDEDVERBS;

    HRESULT hr = contextMenu->QueryContextMenu(hMenu, 0, kContextMenuFirstCmdId,
                                               kContextMenuLastCmdId, flags);
    if (FAILED(hr)) {
        Wh_Log(L"QueryContextMenu failed: %08X", hr);
        DestroyMenu(hMenu);
        return;
    }

    if (g_unloading) {
        DestroyMenu(hMenu);
        return;
    }

    auto contextMenu2 = contextMenu.try_as<IContextMenu2>();
    auto contextMenu3 = contextMenu.try_as<IContextMenu3>();
    g_trackedContextMenu2 = contextMenu2.get();
    g_trackedContextMenu3 = contextMenu3.get();
    g_contextMenuIsOpen = true;

    UINT cmdId = (UINT)TrackPopupMenuEx(
        hMenu, TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_LEFTALIGN,
        point.x, point.y, hOwnerWnd, nullptr);

    g_contextMenuIsOpen = false;
    g_trackedContextMenu2 = nullptr;
    g_trackedContextMenu3 = nullptr;

    if (cmdId >= kContextMenuFirstCmdId && cmdId <= kContextMenuLastCmdId &&
        !g_unloading) {
        InvokeShellContextMenuCommand(contextMenu, cmdId, hExplorerWnd, point);
    }

    DestroyMenu(hMenu);
}

// Reads the folder and the selection of a File Explorer window. It walks
// IShellWindows, which can be slow or live in another process, so it's called
// from a worker thread. All selected paths are only collected when
// allSelected is set, since a large selection takes a while.
ExplorerContext GetExplorerContext(HWND hExplorerWnd,
                                   bool allSelected = false) {
    ExplorerContext result;
    result.shellView = GetActiveShellView(hExplorerWnd);
    if (!result.shellView)
        return result;

    if (auto folderView = result.shellView.try_as<IFolderView>()) {
        winrt::com_ptr<IPersistFolder2> persistFolder;
        LPITEMIDLIST pidl = nullptr;
        if (SUCCEEDED(
                folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) &&
            persistFolder && SUCCEEDED(persistFolder->GetCurFolder(&pidl)) &&
            pidl) {
            WCHAR path[MAX_PATH];
            if (SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path),
                                      GPFIDL_DEFAULT)) {
                result.folderPath = path;
            }

            winrt::com_ptr<IShellItem> folderItem;
            if (SUCCEEDED(SHCreateItemFromIDList(
                    pidl, IID_PPV_ARGS(folderItem.put()))) &&
                folderItem) {
                PWSTR name = nullptr;
                if (SUCCEEDED(folderItem->GetDisplayName(SIGDN_NORMALDISPLAY,
                                                         &name)) &&
                    name) {
                    result.folderDisplayName = name;
                    CoTaskMemFree(name);
                }
            }
            CoTaskMemFree(pidl);
        }
    }

    winrt::com_ptr<IShellItemArray> selection;
    if (SUCCEEDED(result.shellView->GetItemObject(
            SVGIO_SELECTION, IID_PPV_ARGS(selection.put()))) &&
        selection) {
        DWORD count = 0;
        selection->GetCount(&count);
        for (DWORD i = 0;
             i < count && (allSelected || result.selectedPath.empty()); i++) {
            winrt::com_ptr<IShellItem> shellItem;
            if (SUCCEEDED(selection->GetItemAt(i, shellItem.put())) &&
                shellItem) {
                PWSTR itemPath = nullptr;
                if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_FILESYSPATH,
                                                        &itemPath)) &&
                    itemPath) {
                    if (result.selectedPath.empty())
                        result.selectedPath = itemPath;
                    result.allSelectedPaths.push_back(itemPath);
                    CoTaskMemFree(itemPath);
                }
            }
        }
    }

    return result;
}

bool ReplacePlaceholder(std::wstring& parameters,
                        std::wstring_view placeholder,
                        std::wstring const& value) {
    size_t pos = parameters.find(placeholder);
    if (pos == std::wstring::npos)
        return true;

    while (pos != std::wstring::npos) {
        std::wstring replacement = value;
        if (!replacement.empty() && replacement.back() == L'\\' &&
            pos + placeholder.size() < parameters.size() &&
            parameters[pos + placeholder.size()] == L'"') {
            replacement += L'\\';
        }
        parameters.replace(pos, placeholder.size(), replacement);
        pos = parameters.find(placeholder, pos + replacement.size());
    }
    return true;
}

std::wstring FormatSelectedItemsQuoted(std::vector<std::wstring> const& paths) {
    std::wstring formatted;
    for (auto const& p : paths) {
        std::wstring safe = p;
        if (!safe.empty() && safe.back() == L'\\' && safe.size() > 3)
            safe += L'\\';
        formatted += L"\"" + safe + L"\" ";
    }
    if (!formatted.empty())
        formatted.pop_back();
    return formatted;
}

std::wstring ExtractBaseName(std::wstring_view itemPath) {
    if (itemPath.empty())
        return L"";
    PCWSTR fileName = PathFindFileNameW(itemPath.data());
    if (!fileName || !*fileName)
        return L"";
    WCHAR buffer[MAX_PATH];
    wcsncpy_s(buffer, fileName, _TRUNCATE);
    PathRemoveExtensionW(buffer);
    return buffer;
}

std::wstring QuoteArg(std::wstring value) {
    if (!value.empty() && value.back() == L'\\')
        value += L'\\';
    return L"\"" + value + L"\"";
}

std::wstring GetEffectiveFolderPath(const std::wstring& folderPath) {
    if (!folderPath.empty() && PathFileExistsW(folderPath.c_str()))
        return folderPath;
    WCHAR userProfile[MAX_PATH];
    if (GetEnvironmentVariableW(L"USERPROFILE", userProfile,
                                ARRAYSIZE(userProfile)))
        return userProfile;
    WCHAR sysDir[MAX_PATH];
    if (GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir)))
        return sysDir;
    return L"C:\\";
}

constexpr size_t kMaxToolbarLabelLength = 24;
constexpr size_t kMaxMenuItemLabelLength = 48;

std::wstring TruncateWithEllipsis(std::wstring text, size_t maxLen) {
    if (text.length() > maxLen && maxLen > 3) {
        return text.substr(0, maxLen - 3) + L"...";
    }
    return text;
}

std::wstring ResolveLabelTokens(std::wstring label,
                                const ExplorerContext& context) {
    if (label.find(L'%') == std::wstring::npos)
        return label;

    bool hasEscapedPercent = (label.find(L"%%") != std::wstring::npos);
    if (hasEscapedPercent)
        ReplacePlaceholder(label, L"%%", L"\x01");

    size_t countVal = context.selectionCount > 0
                          ? context.selectionCount
                          : context.allSelectedPaths.size();
    std::wstring countStr = std::to_wstring(countVal);
    ReplacePlaceholder(label, L"%count%", countStr);
    ReplacePlaceholder(label, L"%count", countStr);
    ReplacePlaceholder(label, L"%c%", countStr);
    ReplacePlaceholder(label, L"%c", countStr);

    std::wstring baseName = L"";
    if (!context.allSelectedPaths.empty()) {
        baseName = ExtractBaseName(context.allSelectedPaths[0]);
    } else if (context.shellView) {
        winrt::com_ptr<IShellItemArray> items;
        if (SUCCEEDED(context.shellView->GetItemObject(
                SVGIO_SELECTION, IID_PPV_ARGS(items.put()))) &&
            items) {
            winrt::com_ptr<IShellItem> item;
            if (SUCCEEDED(items->GetItemAt(0, item.put())) && item) {
                PWSTR name = nullptr;
                if (SUCCEEDED(
                        item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) &&
                    name) {
                    baseName = ExtractBaseName(name);
                    CoTaskMemFree(name);
                }
            }
        }
    }
    ReplacePlaceholder(label, L"%basename%", baseName);
    ReplacePlaceholder(label, L"%basename", baseName);
    ReplacePlaceholder(label, L"%file_name%", baseName);
    ReplacePlaceholder(label, L"%file_name", baseName);
    ReplacePlaceholder(label, L"%base%", baseName);
    ReplacePlaceholder(label, L"%base", baseName);

    std::wstring fileName = L"";
    if (!context.allSelectedPaths.empty()) {
        PCWSTR n = PathFindFileNameW(context.allSelectedPaths[0].c_str());
        if (n)
            fileName = n;
    }
    ReplacePlaceholder(label, L"%name%", fileName);
    ReplacePlaceholder(label, L"%name", fileName);
    ReplacePlaceholder(label, L"%n%", fileName);
    ReplacePlaceholder(label, L"%n", fileName);
    ReplacePlaceholder(label, L"%1%", fileName);
    ReplacePlaceholder(label, L"%1", fileName);

    std::wstring ext = L"";
    if (!context.allSelectedPaths.empty()) {
        PCWSTR e = PathFindExtensionW(context.allSelectedPaths[0].c_str());
        if (e)
            ext = e;
    }
    ReplacePlaceholder(label, L"%ext%", ext);
    ReplacePlaceholder(label, L"%ext", ext);

    std::wstring dName = context.folderDisplayName;
    if (dName.empty() && !context.folderPath.empty()) {
        std::wstring dir = context.folderPath;
        while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/'))
            dir.pop_back();
        PCWSTR dirName = PathFindFileNameW(dir.c_str());
        if (dirName)
            dName = dirName;
    }
    ReplacePlaceholder(label, L"%d_name%", dName);
    ReplacePlaceholder(label, L"%d_name", dName);

    ReplacePlaceholder(label, L"%path%", context.folderPath);
    ReplacePlaceholder(label, L"%path", context.folderPath);
    ReplacePlaceholder(label, L"%dir%", context.folderPath);
    ReplacePlaceholder(label, L"%dir", context.folderPath);

    if (hasEscapedPercent)
        ReplacePlaceholder(label, L"\x01", L"%");

    return label;
}

ExplorerContext GetContextFromView(IShellView* pView, int selectionCount = -1) {
    ExplorerContext result;
    if (!pView)
        return result;

    result.shellView.copy_from(pView);
    result.selectionCount = (selectionCount >= 0) ? (size_t)selectionCount : 0;

    winrt::com_ptr<IShellItemArray> items;
    if (SUCCEEDED(
            pView->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(items.put()))) &&
        items) {
        DWORD count = 0;
        if (SUCCEEDED(items->GetCount(&count)) && count > 0) {
            winrt::com_ptr<IShellItem> item;
            if (SUCCEEDED(items->GetItemAt(0, item.put())) && item) {
                PWSTR path = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) &&
                    path) {
                    result.selectedPath = path;
                    result.allSelectedPaths.push_back(path);
                    CoTaskMemFree(path);
                }
            }
        }
    }

    if (auto folderView = result.shellView.try_as<IFolderView>()) {
        winrt::com_ptr<IPersistFolder2> persistFolder;
        if (SUCCEEDED(
                folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) &&
            persistFolder) {
            LPITEMIDLIST pidl = nullptr;
            if (SUCCEEDED(persistFolder->GetCurFolder(&pidl)) && pidl) {
                WCHAR path[MAX_PATH];
                if (SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path),
                                          GPFIDL_DEFAULT)) {
                    result.folderPath = path;
                }
                winrt::com_ptr<IShellItem> folderItem;
                if (SUCCEEDED(SHCreateItemFromIDList(
                        pidl, IID_PPV_ARGS(folderItem.put()))) &&
                    folderItem) {
                    PWSTR name = nullptr;
                    if (SUCCEEDED(folderItem->GetDisplayName(
                            SIGDN_NORMALDISPLAY, &name)) &&
                        name) {
                        result.folderDisplayName = name;
                        CoTaskMemFree(name);
                    }
                }
                CoTaskMemFree(pidl);
            }
        }
    }
    return result;
}

std::wstring BuildParameters(std::wstring parameters,
                             ExplorerContext const& context) {
    if (parameters.empty() || parameters.find(L'%') == std::wstring::npos)
        return parameters;

    std::wstring effectiveDir = GetEffectiveFolderPath(context.folderPath);

    bool hasEscapedPercent = (parameters.find(L"%%") != std::wstring::npos);
    if (hasEscapedPercent) {
        ReplacePlaceholder(parameters, L"%%", L"\x01");
    }

    std::vector<std::wstring> filesOnly;
    std::vector<std::wstring> foldersOnly;
    bool needsItemClassification =
        (parameters.find(L"%files") != std::wstring::npos ||
         parameters.find(L"%folders") != std::wstring::npos ||
         parameters.find(L"%d_smart") != std::wstring::npos);

    if (needsItemClassification) {
        for (const auto& item : context.allSelectedPaths) {
            DWORD attr = GetFileAttributesW(item.c_str());
            if (attr != INVALID_FILE_ATTRIBUTES &&
                (attr & FILE_ATTRIBUTE_DIRECTORY)) {
                foldersOnly.push_back(item);
            } else {
                filesOnly.push_back(item);
            }
        }
    }

    ReplacePlaceholder(parameters, L"\"%sel_each%\"", L"");
    ReplacePlaceholder(parameters, L"%sel_each%", L"");
    ReplacePlaceholder(parameters, L"\"%sel_each\"", L"");
    ReplacePlaceholder(parameters, L"%sel_each", L"");

    std::wstring formattedFiles = FormatSelectedItemsQuoted(filesOnly);
    ReplacePlaceholder(parameters, L"\"%files%\"", formattedFiles);
    ReplacePlaceholder(parameters, L"%files%", formattedFiles);
    ReplacePlaceholder(parameters, L"\"%files\"", formattedFiles);
    ReplacePlaceholder(parameters, L"%files", formattedFiles);

    std::wstring formattedFolders = FormatSelectedItemsQuoted(foldersOnly);
    ReplacePlaceholder(parameters, L"\"%folders%\"", formattedFolders);
    ReplacePlaceholder(parameters, L"%folders%", formattedFolders);
    ReplacePlaceholder(parameters, L"\"%folders\"", formattedFolders);
    ReplacePlaceholder(parameters, L"%folders", formattedFolders);

    if (parameters.find(L"%base") != std::wstring::npos ||
        parameters.find(L"%file_name") != std::wstring::npos) {
        std::vector<std::wstring> baseNames;
        for (const auto& p : context.allSelectedPaths) {
            baseNames.push_back(ExtractBaseName(p));
        }
        std::wstring quotedBases = FormatSelectedItemsQuoted(baseNames);
        std::wstring firstBase =
            context.allSelectedPaths.empty()
                ? L""
                : ExtractBaseName(context.allSelectedPaths[0]);

        ReplacePlaceholder(parameters, L"\"%basename%\"", quotedBases);
        ReplacePlaceholder(parameters, L"%basename%", quotedBases);
        ReplacePlaceholder(parameters, L"\"%basename\"", quotedBases);
        ReplacePlaceholder(parameters, L"%basename", quotedBases);

        ReplacePlaceholder(parameters, L"\"%base%\"", quotedBases);
        ReplacePlaceholder(parameters, L"%base%", quotedBases);
        ReplacePlaceholder(parameters, L"\"%base\"", quotedBases);
        ReplacePlaceholder(parameters, L"%base", quotedBases);

        std::wstring quotedFirstBase =
            firstBase.empty() ? L"" : (L"\"" + firstBase + L"\"");
        ReplacePlaceholder(parameters, L"\"%file_name%\"", quotedFirstBase);
        ReplacePlaceholder(parameters, L"%file_name%", firstBase);
        ReplacePlaceholder(parameters, L"\"%file_name\"", quotedFirstBase);
        ReplacePlaceholder(parameters, L"%file_name", firstBase);
    }

    if (parameters.find(L"%n") != std::wstring::npos ||
        parameters.find(L"%name") != std::wstring::npos) {
        std::vector<std::wstring> names;
        for (const auto& p : context.allSelectedPaths) {
            PCWSTR name = PathFindFileNameW(p.c_str());
            names.push_back(name ? name : L"");
        }
        std::wstring quotedNames = FormatSelectedItemsQuoted(names);
        ReplacePlaceholder(parameters, L"\"%name%\"", quotedNames);
        ReplacePlaceholder(parameters, L"%name%", quotedNames);
        ReplacePlaceholder(parameters, L"\"%name\"", quotedNames);
        ReplacePlaceholder(parameters, L"%name", quotedNames);
        ReplacePlaceholder(parameters, L"\"%n%\"", quotedNames);
        ReplacePlaceholder(parameters, L"%n%", quotedNames);
        ReplacePlaceholder(parameters, L"\"%n\"", quotedNames);
        ReplacePlaceholder(parameters, L"%n", quotedNames);
    }

    if (parameters.find(L"%1") != std::wstring::npos) {
        std::wstring firstItem = context.allSelectedPaths.empty()
                                     ? L""
                                     : QuoteArg(context.allSelectedPaths[0]);
        ReplacePlaceholder(parameters, L"\"%1%\"", firstItem);
        ReplacePlaceholder(parameters, L"%1%", firstItem);
        ReplacePlaceholder(parameters, L"\"%1\"", firstItem);
        ReplacePlaceholder(parameters, L"%1", firstItem);
    }

    if (parameters.find(L"%ext") != std::wstring::npos) {
        std::wstring ext = L"";
        if (!context.allSelectedPaths.empty()) {
            PCWSTR extPtr =
                PathFindExtensionW(context.allSelectedPaths[0].c_str());
            if (extPtr)
                ext = extPtr;
        }
        ReplacePlaceholder(parameters, L"%ext%", ext);
        ReplacePlaceholder(parameters, L"%ext", ext);
    }

    std::wstring countStr = std::to_wstring(context.allSelectedPaths.size());
    ReplacePlaceholder(parameters, L"%count%", countStr);
    ReplacePlaceholder(parameters, L"%count", countStr);
    ReplacePlaceholder(parameters, L"%c%", countStr);
    ReplacePlaceholder(parameters, L"%c", countStr);

    if (parameters.find(L"%d_name") != std::wstring::npos) {
        std::wstring dName = context.folderDisplayName;
        if (dName.empty()) {
            std::wstring dir = effectiveDir;
            while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/'))
                dir.pop_back();
            PCWSTR namePtr = PathFindFileNameW(dir.c_str());
            if (namePtr)
                dName = namePtr;
        }
        ReplacePlaceholder(parameters, L"\"%d_name%\"", L"\"" + dName + L"\"");
        ReplacePlaceholder(parameters, L"%d_name%", dName);
        ReplacePlaceholder(parameters, L"\"%d_name\"", L"\"" + dName + L"\"");
        ReplacePlaceholder(parameters, L"%d_name", dName);
    }

    if (parameters.find(L"%sel") != std::wstring::npos ||
        parameters.find(L"%f") != std::wstring::npos) {
        std::wstring formatted =
            context.allSelectedPaths.empty()
                ? L""
                : FormatSelectedItemsQuoted(context.allSelectedPaths);
        ReplacePlaceholder(parameters, L"\"%sel%\"", formatted);
        ReplacePlaceholder(parameters, L"%sel%", formatted);
        ReplacePlaceholder(parameters, L"\"%sel\"", formatted);
        ReplacePlaceholder(parameters, L"%sel", formatted);
        ReplacePlaceholder(parameters, L"\"%f%\"", formatted);
        ReplacePlaceholder(parameters, L"%f%", formatted);
        ReplacePlaceholder(parameters, L"\"%f\"", formatted);
        ReplacePlaceholder(parameters, L"%f", formatted);
    }

    std::wstring physicalDir = (!context.folderPath.empty() &&
                                PathFileExistsW(context.folderPath.c_str()))
                                   ? context.folderPath
                                   : L"";
    std::wstring quotedPhysicalPath =
        physicalDir.empty() ? L"" : QuoteArg(physicalDir);

    if (parameters.find(L"%d_smart") != std::wstring::npos) {
        std::wstring smartDir =
            !foldersOnly.empty() ? foldersOnly[0] : physicalDir;
        std::wstring quotedSmart = smartDir.empty() ? L"" : QuoteArg(smartDir);
        ReplacePlaceholder(parameters, L"\"%d_smart%\"", quotedSmart);
        ReplacePlaceholder(parameters, L"%d_smart%", smartDir);
        ReplacePlaceholder(parameters, L"\"%d_smart\"", quotedSmart);
        ReplacePlaceholder(parameters, L"%d_smart", smartDir);
    }

    ReplacePlaceholder(parameters, L"\"%path%\"", quotedPhysicalPath);
    ReplacePlaceholder(parameters, L"%path%", physicalDir);
    ReplacePlaceholder(parameters, L"\"%path\"", quotedPhysicalPath);
    ReplacePlaceholder(parameters, L"%path", physicalDir);

    ReplacePlaceholder(parameters, L"\"%dir%\"", quotedPhysicalPath);
    ReplacePlaceholder(parameters, L"%dir%", physicalDir);
    ReplacePlaceholder(parameters, L"\"%dir\"", quotedPhysicalPath);
    ReplacePlaceholder(parameters, L"%dir", physicalDir);

    ReplacePlaceholder(parameters, L"\"%d%\"", quotedPhysicalPath);
    ReplacePlaceholder(parameters, L"%d%", physicalDir);
    ReplacePlaceholder(parameters, L"\"%d\"", quotedPhysicalPath);
    ReplacePlaceholder(parameters, L"%d", physicalDir);

    if (parameters.find(L"%p") != std::wstring::npos ||
        parameters.find(L"%parent") != std::wstring::npos) {
        std::wstring p = L"";
        if (!physicalDir.empty()) {
            std::wstring parent = physicalDir;
            while (!parent.empty() &&
                   (parent.back() == L'\\' || parent.back() == L'/'))
                parent.pop_back();
            if (parent.length() == 2 && parent[1] == L':') {
                p = parent + L'\\';
            } else {
                size_t slash = parent.find_last_of(L"\\/");
                if (slash != std::wstring::npos) {
                    p = parent.substr(0, slash);
                    if (p.length() == 2 && p[1] == L':')
                        p += L'\\';
                }
            }
        }

        std::wstring quotedParent = p.empty() ? L"" : QuoteArg(p);
        ReplacePlaceholder(parameters, L"\"%parent%\"", quotedParent);
        ReplacePlaceholder(parameters, L"%parent%", p);
        ReplacePlaceholder(parameters, L"\"%parent\"", quotedParent);
        ReplacePlaceholder(parameters, L"%parent", p);

        ReplacePlaceholder(parameters, L"\"%p%\"", quotedParent);
        ReplacePlaceholder(parameters, L"%p%", p);
        ReplacePlaceholder(parameters, L"\"%p\"", quotedParent);
        ReplacePlaceholder(parameters, L"%p", p);
    }

    if (hasEscapedPercent) {
        ReplacePlaceholder(parameters, L"\x01", L"%");
    }

    return parameters;
}

struct LaunchOptions {
    bool runAsAdmin = false;
    int showCommand = SW_SHOWNORMAL;
    // The owner of the UAC prompt, so that it comes up in the foreground.
    HWND hOwnerWnd = nullptr;
};

void ExecuteProcess(std::wstring const& command,
                    std::wstring const& parameters,
                    std::wstring const& workingDir,
                    LaunchOptions const& options) {
    bool isPath = command.find(L'\\') != std::wstring::npos;
    SHELLEXECUTEINFOW execInfo{};
    execInfo.cbSize = sizeof(execInfo);
    execInfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    execInfo.hwnd = options.runAsAdmin ? options.hOwnerWnd : nullptr;
    execInfo.lpVerb = options.runAsAdmin ? L"runas" : nullptr;
    execInfo.lpFile = command.c_str();
    execInfo.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
    execInfo.lpDirectory =
        (isPath && !workingDir.empty()) ? workingDir.c_str() : nullptr;
    execInfo.nShow = options.showCommand;

    if (!ShellExecuteExW(&execInfo)) {
        // ERROR_CANCELLED (1223) means that the UAC prompt was declined.
        Wh_Log(L"ShellExecuteExW failed for %s: %u", command.c_str(),
               GetLastError());
    }
}

// Runs on a worker thread, so that a slow or unresponsive shell can't block
// the window. With forceRunAsAdmin, the command runs elevated even if the
// item isn't set to (Ctrl+Shift+click).
void LaunchItemForWindow(HWND hExplorerWnd,
                         ActionItem const& item,
                         bool forceRunAsAdmin) {
    LaunchOptions options;
    options.runAsAdmin = item.runAsAdmin || forceRunAsAdmin;
    options.showCommand =
        kWindowStyles[WindowStyleIndex(item.windowStyle)].showCommand;
    options.hOwnerWnd = hExplorerWnd;

    bool allSelected =
        item.parameters.find(L"%sel%") != std::wstring::npos ||
        item.parameters.find(L"%sel_each") != std::wstring::npos ||
        item.parameters.find(L"%files") != std::wstring::npos ||
        item.parameters.find(L"%folders") != std::wstring::npos ||
        item.parameters.find(L"%base") != std::wstring::npos ||
        item.parameters.find(L"%name") != std::wstring::npos ||
        item.parameters.find(L"%n") != std::wstring::npos ||
        item.parameters.find(L"%1") != std::wstring::npos ||
        item.parameters.find(L"%ext") != std::wstring::npos ||
        item.parameters.find(L"%c") != std::wstring::npos;

    ExplorerContext capturedContext =
        GetExplorerContext(hExplorerWnd, allSelected);

    std::wstring command = ResolveCommandPath(item.command);
    Wh_Log(L"Launching %s for window %08X, path: %s", item.command.c_str(),
           (DWORD)(ULONG_PTR)hExplorerWnd, capturedContext.folderPath.c_str());

    std::wstring workingDir =
        GetEffectiveFolderPath(capturedContext.folderPath);

    // %sel_each%: run an individual process instance per selected item
    if (item.parameters.find(L"%sel_each%") != std::wstring::npos ||
        item.parameters.find(L"%sel_each") != std::wstring::npos) {
        if (capturedContext.allSelectedPaths.empty()) {
            std::wstring param = item.parameters;
            param = BuildParameters(param, capturedContext);
            ExecuteProcess(command, param, workingDir, options);
            return;
        }

        // Safety throttle: prevent spawning hundreds of processes on large
        // selections
        constexpr size_t kMaxBatch = 20;
        size_t count =
            std::min(capturedContext.allSelectedPaths.size(), kMaxBatch);

        for (size_t i = 0; i < count; i++) {
            const auto& path = capturedContext.allSelectedPaths[i];
            std::wstring param = item.parameters;
            std::wstring safePath = path;
            if (!safePath.empty() && safePath.back() == L'\\') {
                safePath += L'\\';
            }

            std::wstring quotedPath = L"\"" + safePath + L"\"";
            ReplacePlaceholder(param, L"\"%sel_each%\"", quotedPath);
            ReplacePlaceholder(param, L"\"%sel_each\"", quotedPath);
            ReplacePlaceholder(param, L"%sel_each%", quotedPath);
            ReplacePlaceholder(param, L"%sel_each", quotedPath);

            // Re-evaluate parameter tokens isolated to just this specific file
            ExplorerContext singleCtx = capturedContext;
            singleCtx.allSelectedPaths = {path};
            param = BuildParameters(param, singleCtx);

            ExecuteProcess(command, param, workingDir, options);
        }
        return;
    }

    std::wstring parameters = BuildParameters(item.parameters, capturedContext);
    ExecuteProcess(command, parameters, workingDir, options);
}

std::mutex g_launchThreadsMutex;
std::vector<HANDLE> g_launchThreads;

void TrackLaunchThread(HANDLE thread) {
    std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
    for (auto it = g_launchThreads.begin(); it != g_launchThreads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_launchThreads.erase(it);
        } else {
            ++it;
        }
    }
    g_launchThreads.push_back(thread);
}

void WaitForLaunchThreads() {
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        threads.swap(g_launchThreads);
    }
    for (HANDLE thread : threads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

void RunShellWorkOnWorkerThread(std::function<void()> work) {
    auto* pWork = new std::function<void()>(std::move(work));
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            std::unique_ptr<std::function<void()>> work(
                reinterpret_cast<std::function<void()>*>(lpParam));
            ComApartmentScope comScope(COINIT_APARTMENTTHREADED |
                                       COINIT_DISABLE_OLE1DDE);
            try {
                (*work)();
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
            return 0;
        },
        pWork, 0, nullptr);

    if (thread) {
        TrackLaunchThread(thread);
    } else {
        delete pWork;
    }
}

HWND GetExplorerWindowForElement(mux::FrameworkElement const& element) {
    HWND hWnd = nullptr;
    try {
        if (auto xamlRoot = element.XamlRoot()) {
            if (auto environment = xamlRoot.ContentIslandEnvironment()) {
                hWnd = (HWND)(uintptr_t)environment.AppWindowId().Value;
            }
        }
    } catch (...) {
        Wh_Log(L"Failed to get window from XamlRoot: %08X",
               winrt::to_hresult().value);
    }

    if (!hWnd)
        hWnd = GetActiveWindow();
    if (!hWnd) {
        HWND hForegroundWnd = GetForegroundWindow();
        DWORD processId = 0;
        if (hForegroundWnd &&
            GetWindowThreadProcessId(hForegroundWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            hWnd = hForegroundWnd;
        }
    }
    if (hWnd)
        hWnd = GetAncestor(hWnd, GA_ROOT);
    return hWnd;
}

bool IsCommandMatch(std::wstring_view cmd, std::wstring_view actionName) {
    constexpr std::wstring_view kInternal = L"internal:";
    constexpr std::wstring_view kInt = L"int:";

    if (cmd.size() >= kInternal.size() &&
        _wcsnicmp(cmd.data(), kInternal.data(), kInternal.size()) == 0) {
        return _wcsicmp(cmd.data() + kInternal.size(), actionName.data()) == 0;
    }
    if (cmd.size() >= kInt.size() &&
        _wcsnicmp(cmd.data(), kInt.data(), kInt.size()) == 0) {
        return _wcsicmp(cmd.data() + kInt.size(), actionName.data()) == 0;
    }
    return false;
}

// Presses the keys in order and releases them in reverse order. Modifiers
// still held from a keyboard shortcut are released first, so that they don't
// change the combination.
void SendKeyCombo(std::initializer_list<WORD> keys) {
    std::vector<INPUT> inputs;
    auto addKey = [&inputs](WORD vk, bool up) {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        input.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        inputs.push_back(input);
    };

    for (WORD vk :
         {VK_LCONTROL, VK_RCONTROL, VK_LSHIFT, VK_RSHIFT, VK_LWIN, VK_RWIN}) {
        if (GetAsyncKeyState(vk) < 0)
            addKey(vk, true);
    }
    for (WORD vk : keys)
        addKey(vk, false);
    for (auto it = std::rbegin(keys); it != std::rend(keys); ++it)
        addKey(*it, true);

    SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
}

bool SetClipboardText(std::wstring const& text) {
    // Another program may have the clipboard open for a moment.
    bool opened = false;
    for (int attempt = 0; attempt < 10 && !opened; attempt++) {
        if (attempt > 0)
            Sleep(20);
        opened = OpenClipboard(nullptr);
    }
    if (!opened) {
        Wh_Log(L"OpenClipboard failed: %u", GetLastError());
        return false;
    }

    bool succeeded = false;
    size_t size = (text.size() + 1) * sizeof(WCHAR);
    if (HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size)) {
        if (void* data = GlobalLock(hMem)) {
            memcpy(data, text.c_str(), size);
            GlobalUnlock(hMem);
            if (EmptyClipboard() && SetClipboardData(CF_UNICODETEXT, hMem))
                succeeded = true;
        }
        if (!succeeded)
            GlobalFree(hMem);
    }
    CloseClipboard();
    return succeeded;
}

enum class CopyKind {
    Path,
    Name,
    Dir,
};

// Copies the selected items' paths or names, one per line, or the current
// folder's if nothing is selected.
void CopyToClipboardForWindow(HWND hExplorerWnd, CopyKind kind) {
    ExplorerContext context = GetExplorerContext(hExplorerWnd, true);

    auto folderName = [&context]() {
        std::wstring dir = context.folderPath;
        while (dir.size() > 3 && (dir.back() == L'\\' || dir.back() == L'/'))
            dir.pop_back();
        PCWSTR name = dir.empty() ? nullptr : PathFindFileNameW(dir.c_str());
        return name && *name ? std::wstring(name) : context.folderDisplayName;
    };

    std::wstring text;
    if (kind == CopyKind::Dir || context.allSelectedPaths.empty()) {
        text = kind == CopyKind::Name ? folderName() : context.folderPath;
    } else {
        for (auto const& path : context.allSelectedPaths) {
            if (!text.empty())
                text += L"\r\n";
            if (kind == CopyKind::Name) {
                PCWSTR name = PathFindFileNameW(path.c_str());
                text += name && *name ? name : path.c_str();
            } else {
                text += path;
            }
        }
    }

    if (text.empty()) {
        Wh_Log(L"Nothing to copy for window %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return;
    }
    SetClipboardText(text);
}

void RefreshViewForWindow(HWND hExplorerWnd) {
    if (auto shellView = GetActiveShellView(hExplorerWnd)) {
        HRESULT hr = shellView->Refresh();
        if (FAILED(hr))
            Wh_Log(L"IShellView::Refresh failed: %08X", hr);
    }
}

void CreateNewItemForWindow(HWND hExplorerWnd,
                            bool isDirectory,
                            std::wstring name);

// Runs an item for the given File Explorer window. Called from the thread of
// the window, on a click or a keyboard shortcut. With forceRunAsAdmin, a
// command runs elevated even if the item isn't set to.
void InvokeActionItem(HWND hWnd,
                      ActionItem const& item,
                      bool forceRunAsAdmin = false) {
    if (item.command.empty() || g_unloading)
        return;

    std::wstring_view cmd = item.command;

    if (IsCommandMatch(cmd, L"CopyPath") || IsCommandMatch(cmd, L"CopyName") ||
        IsCommandMatch(cmd, L"CopyDir")) {
        CopyKind kind = IsCommandMatch(cmd, L"CopyPath")   ? CopyKind::Path
                        : IsCommandMatch(cmd, L"CopyName") ? CopyKind::Name
                                                           : CopyKind::Dir;
        RunShellWorkOnWorkerThread(
            [hWnd, kind]() { CopyToClipboardForWindow(hWnd, kind); });
        return;
    }

    if (IsCommandMatch(cmd, L"NewFolder") || IsCommandMatch(cmd, L"NewFile")) {
        bool isDirectory = IsCommandMatch(cmd, L"NewFolder");
        RunShellWorkOnWorkerThread(
            [hWnd, isDirectory, name = TrimQuotesAndSpaces(item.parameters)]() {
                CreateNewItemForWindow(hWnd, isDirectory, name);
            });
        return;
    }

    if (IsCommandMatch(cmd, L"Refresh")) {
        RunShellWorkOnWorkerThread([hWnd]() { RefreshViewForWindow(hWnd); });
        return;
    }

    if (IsCommandMatch(cmd, L"TogglePreview")) {
        SendKeyCombo({VK_MENU, 'P'});
        return;
    }

    if (IsCommandMatch(cmd, L"ToggleDetails")) {
        SendKeyCombo({VK_MENU, VK_SHIFT, 'P'});
        return;
    }

    if (IsCommandMatch(cmd, L"FolderOptions")) {
        RunShellWorkOnWorkerThread([]() {
            ShellExecuteW(nullptr, L"open", L"control.exe", L"folders", nullptr,
                          SW_SHOWNORMAL);
        });
        return;
    }

    if (IsCommandMatch(cmd, L"OpenWith")) {
        RunShellWorkOnWorkerThread([hWnd]() {
            ExplorerContext context = GetExplorerContext(hWnd);
            if (context.selectedPath.empty() ||
                DirectoryExists(context.selectedPath))
                return;
            OPENASINFO oai = {};
            oai.pcszFile = context.selectedPath.c_str();
            oai.oaifInFlags = OAIF_EXEC | OAIF_ALLOW_REGISTRATION;
            SHOpenWithDialog(hWnd, &oai);
        });
        return;
    }

    RunShellWorkOnWorkerThread([hWnd, item, forceRunAsAdmin]() {
        LaunchItemForWindow(hWnd, item, forceRunAsAdmin);
    });
}

void OnActionInvoked(mux::FrameworkElement const& elementForWindow,
                     ActionItem const& item) {
    if (item.command.empty() || g_unloading)
        return;

    // Ctrl+Shift+click runs the command as administrator, like in the Start
    // menu and the taskbar.
    bool forceRunAsAdmin =
        GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0;
    InvokeActionItem(GetExplorerWindowForElement(elementForWindow), item,
                     forceRunAsAdmin);
}

std::wstring GetPowerToysNewPlusFolder() {
    return ExpandEnvVars(L"%LOCALAPPDATA%\\Microsoft\\PowerToys\\NewPlus");
}

std::wstring DefaultTemplatesFolder() {
    return JoinPath(GetPowerToysNewPlusFolder(), L"Templates");
}

std::wstring ReadFileAsWideString(std::wstring const& path) {
    HANDLE hFile =
        CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return std::wstring();

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(hFile, &size) || size.QuadPart <= 0 ||
        size.QuadPart > 1024 * 1024) {
        CloseHandle(hFile);
        return std::wstring();
    }

    std::string bytes((size_t)size.QuadPart, '\0');
    DWORD bytesRead = 0;
    BOOL succeeded =
        ReadFile(hFile, bytes.data(), (DWORD)bytes.size(), &bytesRead, nullptr);
    CloseHandle(hFile);
    if (!succeeded)
        return std::wstring();

    bytes.resize(bytesRead);
    if (bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF &&
        (unsigned char)bytes[1] == 0xBB && (unsigned char)bytes[2] == 0xBF) {
        bytes.erase(0, 3);
    }

    int length = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                     (int)bytes.size(), nullptr, 0);
    if (length <= 0)
        return std::wstring();

    std::wstring result((size_t)length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        result.data(), length);
    return result;
}

wdj::IJsonValue FindNewPlusSetting(wdj::JsonObject const& root, PCWSTR name) {
    wdj::JsonObject properties = root;
    if (auto nested = root.TryLookup(L"properties");
        nested && nested.ValueType() == wdj::JsonValueType::Object) {
        properties = nested.GetObject();
    }
    auto value = properties.TryLookup(name);
    if (value && value.ValueType() == wdj::JsonValueType::Object) {
        return value.GetObject().TryLookup(L"value");
    }
    return value;
}

struct PowerToysConfig {
    std::wstring templateFolder;
    bool hideFileExtension = true;
    bool hideStartingDigits = true;
    bool replaceVariables = false;
};

PowerToysConfig ReadPowerToysConfig() {
    PowerToysConfig config;
    std::wstring json = ReadFileAsWideString(
        JoinPath(GetPowerToysNewPlusFolder(), L"settings.json"));
    if (json.empty())
        return config;

    wdj::JsonObject root{nullptr};
    if (!wdj::JsonObject::TryParse(json, root)) {
        Wh_Log(L"Couldn't parse the New+ settings file");
        return config;
    }

    auto readBool = [&root](PCWSTR name, bool fallback) {
        auto value = FindNewPlusSetting(root, name);
        if (!value)
            return fallback;
        switch (value.ValueType()) {
            case wdj::JsonValueType::Boolean:
                return value.GetBoolean();
            case wdj::JsonValueType::Number:
                return value.GetNumber() != 0;
            case wdj::JsonValueType::String: {
                std::wstring text = ToLower(std::wstring(value.GetString()));
                if (text == L"true" || text == L"1")
                    return true;
                if (text == L"false" || text == L"0")
                    return false;
                return fallback;
            }
            default:
                return fallback;
        }
    };

    if (auto templateLocation = FindNewPlusSetting(root, L"TemplateLocation");
        templateLocation &&
        templateLocation.ValueType() == wdj::JsonValueType::String) {
        config.templateFolder =
            TrimQuotesAndSpaces(std::wstring(templateLocation.GetString()));
    }

    config.hideFileExtension =
        readBool(L"HideFileExtension", config.hideFileExtension);
    config.hideStartingDigits =
        readBool(L"HideStartingDigits", config.hideStartingDigits);
    config.replaceVariables =
        readBool(L"ReplaceVariables", config.replaceVariables);
    return config;
}

struct EffectiveConfig {
    std::wstring templateFolder;
    bool hideFileExtension;
    bool hideStartingDigits;
    bool replaceVariables;
    bool showIcons;
    bool showTemplatesFolderItem;
};

EffectiveConfig GetEffectiveConfig() {
    std::wstring templateFolderSetting;
    EffectiveConfig config{};
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        templateFolderSetting = g_settings.newPlus.templateFolder;
        config.showIcons = g_settings.newPlus.showIcons;
        config.showTemplatesFolderItem =
            g_settings.newPlus.showTemplatesFolderItem;
    }

    PowerToysConfig powerToys = ReadPowerToysConfig();
    config.templateFolder = ExpandEnvVars(
        !templateFolderSetting.empty()
            ? templateFolderSetting
            : (!powerToys.templateFolder.empty() ? powerToys.templateFolder
                                                 : DefaultTemplatesFolder()));
    config.hideFileExtension = powerToys.hideFileExtension;
    config.hideStartingDigits = powerToys.hideStartingDigits;
    config.replaceVariables = powerToys.replaceVariables;
    return config;
}

struct TemplateEntry {
    std::wstring path;
    std::wstring fileName;
    std::wstring displayName;
    bool isDirectory = false;
};

std::wstring StripStartingDigits(std::wstring const& name) {
    size_t pos = 0;
    while (pos < name.size() && iswdigit(name[pos]))
        pos++;
    if (pos == 0)
        return name;
    while (pos < name.size() && wcschr(L" .-_", name[pos]))
        pos++;
    if (pos >= name.size())
        return name;
    return name.substr(pos);
}

std::wstring MakeDisplayName(std::wstring const& fileName,
                             bool isDirectory,
                             EffectiveConfig const& config) {
    std::wstring name = fileName;
    if (!isDirectory && config.hideFileExtension) {
        size_t dot = name.find_last_of(L'.');
        if (dot != std::wstring::npos && dot > 0)
            name.resize(dot);
    }
    if (config.hideStartingDigits)
        name = StripStartingDigits(name);
    return name;
}

std::vector<TemplateEntry> EnumerateTemplates(EffectiveConfig const& config) {
    std::vector<TemplateEntry> entries;
    std::wstring pattern = JoinPath(config.templateFolder, L"*");

    WIN32_FIND_DATAW findData{};
    HANDLE hFind = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &findData,
                                    FindExSearchNameMatch, nullptr,
                                    FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Couldn't enumerate %s: %u", config.templateFolder.c_str(),
               GetLastError());
        return entries;
    }

    do {
        std::wstring fileName = findData.cFileName;
        if (fileName == L"." || fileName == L".." ||
            _wcsicmp(fileName.c_str(), L"desktop.ini") == 0)
            continue;
        if (findData.dwFileAttributes &
            (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
            continue;

        TemplateEntry entry;
        entry.isDirectory =
            (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        entry.fileName = fileName;
        entry.path = JoinPath(config.templateFolder, fileName);
        entry.displayName =
            MakeDisplayName(fileName, entry.isDirectory, config);
        entries.push_back(std::move(entry));
    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);

    std::sort(entries.begin(), entries.end(),
              [](TemplateEntry const& a, TemplateEntry const& b) {
                  if (a.isDirectory != b.isDirectory)
                      return a.isDirectory;
                  return StrCmpLogicalW(a.fileName.c_str(),
                                        b.fileName.c_str()) < 0;
              });

    return entries;
}

std::wstring ReplaceAll(std::wstring str,
                        std::wstring_view from,
                        std::wstring const& to) {
    if (from.empty())
        return str;
    size_t pos = str.find(from);
    while (pos != std::wstring::npos) {
        str.replace(pos, from.size(), to);
        pos = str.find(from, pos + to.size());
    }
    return str;
}

std::wstring TwoDigits(int value) {
    WCHAR buffer[8];
    swprintf(buffer, ARRAYSIZE(buffer), L"%02d", value);
    return buffer;
}

std::wstring ReplaceNameVariables(std::wstring const& fileName,
                                  std::wstring const& targetFolder) {
    SYSTEMTIME time{};
    GetLocalTime(&time);

    std::wstring parentFolderName;
    {
        std::wstring folder = targetFolder;
        while (!folder.empty() &&
               (folder.back() == L'\\' || folder.back() == L'/'))
            folder.pop_back();
        size_t slash = folder.find_last_of(L"\\/");
        parentFolderName =
            slash == std::wstring::npos ? folder : folder.substr(slash + 1);
    }

    std::wstring result = fileName;
    result = ReplaceAll(result, L"$PARENT_FOLDER_NAME", parentFolderName);
    result = ReplaceAll(result, L"$YYYY", std::to_wstring(time.wYear));
    result = ReplaceAll(result, L"$YY", TwoDigits(time.wYear % 100));
    result = ReplaceAll(result, L"$MM", TwoDigits(time.wMonth));
    result = ReplaceAll(result, L"$DD", TwoDigits(time.wDay));
    result = ReplaceAll(result, L"$hh", TwoDigits(time.wHour));
    result = ReplaceAll(result, L"$mm", TwoDigits(time.wMinute));
    result = ReplaceAll(result, L"$ss", TwoDigits(time.wSecond));
    return result;
}

void SplitFileName(std::wstring const& fileName,
                   bool isDirectory,
                   std::wstring* baseName,
                   std::wstring* extension) {
    size_t dot = isDirectory ? std::wstring::npos : fileName.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) {
        *baseName = fileName;
        extension->clear();
        return;
    }
    *baseName = fileName.substr(0, dot);
    *extension = fileName.substr(dot);
}

std::wstring MakeUniquePath(std::wstring const& targetFolder,
                            std::wstring const& fileName,
                            bool isDirectory) {
    std::wstring path = JoinPath(targetFolder, fileName);
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
        return path;

    std::wstring baseName, extension;
    SplitFileName(fileName, isDirectory, &baseName, &extension);

    for (int i = 2; i < 10000; i++) {
        std::wstring candidate =
            baseName + L" (" + std::to_wstring(i) + L")" + extension;
        path = JoinPath(targetFolder, candidate);
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
            return path;
    }
    return std::wstring();
}

bool CopyTemplateItem(std::wstring const& source,
                      std::wstring const& targetFolder,
                      std::wstring const& targetName) {
    winrt::com_ptr<IFileOperation> operation;
    HRESULT hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(operation.put()));
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance(FileOperation) failed: %08X", hr);
        return false;
    }

    hr = operation->SetOperationFlags(FOF_NO_UI);
    if (FAILED(hr)) {
        Wh_Log(L"SetOperationFlags failed: %08X", hr);
        return false;
    }

    winrt::com_ptr<IShellItem> sourceItem, targetFolderItem;
    if (FAILED(SHCreateItemFromParsingName(source.c_str(), nullptr,
                                           IID_PPV_ARGS(sourceItem.put()))) ||
        FAILED(SHCreateItemFromParsingName(
            targetFolder.c_str(), nullptr,
            IID_PPV_ARGS(targetFolderItem.put()))) ||
        FAILED(operation->CopyItem(sourceItem.get(), targetFolderItem.get(),
                                   targetName.c_str(), nullptr)) ||
        FAILED(operation->PerformOperations()))
        return false;

    BOOL aborted = FALSE;
    return SUCCEEDED(operation->GetAnyOperationsAborted(&aborted)) && !aborted;
}

void SelectAndRename(winrt::com_ptr<IShellView> const& shellView,
                     std::wstring const& path) {
    if (!shellView)
        return;
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) ||
        !pidl)
        return;

    PCUITEMID_CHILD child = ILFindLastID(pidl);
    auto folderView = shellView.try_as<IFolderView>();

    for (int attempt = 0; attempt < 20 && !g_unloading; attempt++) {
        Sleep(50);
        HRESULT hr = shellView->SelectItem(
            child, SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE |
                       SVSI_FOCUSED | SVSI_EDIT);
        if (FAILED(hr))
            continue;
        if (!folderView)
            break;
        int selectedCount = 0;
        if (SUCCEEDED(folderView->ItemCount(SVGIO_SELECTION, &selectedCount)) &&
            selectedCount > 0)
            break;
    }

    CoTaskMemFree(pidl);
}

void CreateFromTemplateForWindow(HWND hExplorerWnd,
                                 TemplateEntry const& entry,
                                 bool replaceVariables) {
    ExplorerContext context = GetExplorerContext(hExplorerWnd);
    if (context.folderPath.empty()) {
        Wh_Log(L"No filesystem folder for window %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return;
    }

    std::wstring fileName =
        replaceVariables
            ? ReplaceNameVariables(entry.fileName, context.folderPath)
            : entry.fileName;
    std::wstring targetPath =
        MakeUniquePath(context.folderPath, fileName, entry.isDirectory);
    if (targetPath.empty()) {
        Wh_Log(L"Couldn't find a free name for %s", fileName.c_str());
        return;
    }

    Wh_Log(L"Creating %s from %s", targetPath.c_str(), entry.path.c_str());

    bool succeeded = CopyTemplateItem(entry.path, context.folderPath,
                                      PathFindFileNameW(targetPath.c_str()));
    if (!succeeded &&
        GetFileAttributesW(targetPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        return;
    SelectAndRename(context.shellView, targetPath);
}

// internal:NewFolder and internal:NewFile. The name comes from the item's
// parameters, with the New+ filename variables ($YYYY, $MM, ...) replaced.
// Created through IFileOperation, so that Ctrl+Z undoes it.
void CreateNewItemForWindow(HWND hExplorerWnd,
                            bool isDirectory,
                            std::wstring name) {
    ExplorerContext context = GetExplorerContext(hExplorerWnd);
    if (context.folderPath.empty()) {
        Wh_Log(L"No filesystem folder for window %08X",
               (DWORD)(ULONG_PTR)hExplorerWnd);
        return;
    }

    if (name.empty())
        name = isDirectory ? L"New folder" : L"file";
    name = ReplaceNameVariables(name, context.folderPath);
    if (name.find_first_of(L"\\/:*?\"<>|") != std::wstring::npos) {
        Wh_Log(L"Invalid name: %s", name.c_str());
        return;
    }

    std::wstring targetPath =
        MakeUniquePath(context.folderPath, name, isDirectory);
    if (targetPath.empty()) {
        Wh_Log(L"Couldn't find a free name for %s", name.c_str());
        return;
    }

    winrt::com_ptr<IFileOperation> operation;
    HRESULT hr = CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(operation.put()));
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance(FileOperation) failed: %08X", hr);
        return;
    }

    if (FAILED(operation->SetOperationFlags(FOF_NO_UI | FOF_ALLOWUNDO |
                                            FOFX_ADDUNDORECORD))) {
        operation->SetOperationFlags(FOF_NO_UI);
    }

    winrt::com_ptr<IShellItem> folderItem;
    hr = SHCreateItemFromParsingName(context.folderPath.c_str(), nullptr,
                                     IID_PPV_ARGS(folderItem.put()));
    if (SUCCEEDED(hr)) {
        hr = operation->NewItem(
            folderItem.get(),
            isDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
            PathFindFileNameW(targetPath.c_str()), nullptr, nullptr);
    }
    if (SUCCEEDED(hr))
        hr = operation->PerformOperations();
    if (FAILED(hr) ||
        GetFileAttributesW(targetPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Creating %s failed: %08X", targetPath.c_str(), hr);
        return;
    }

    Wh_Log(L"Created %s", targetPath.c_str());
    SelectAndRename(context.shellView, targetPath);
}

#ifndef IO_REPARSE_TAG_APPEXECLINK
#define IO_REPARSE_TAG_APPEXECLINK (0x8000001BL)
#endif

std::wstring ResolveAppExecutionAlias(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        !(attributes & FILE_ATTRIBUTE_REPARSE_POINT))
        return path;

    HANDLE hFile = CreateFileW(
        path.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return path;

    struct AppExecLinkReparseBuffer {
        ULONG reparseTag;
        USHORT reparseDataLength;
        USHORT reserved;
        ULONG version;
        WCHAR stringList[1];
    };

    alignas(8) BYTE buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    DWORD bytesReturned = 0;
    BOOL succeeded =
        DeviceIoControl(hFile, FSCTL_GET_REPARSE_POINT, nullptr, 0, buffer,
                        sizeof(buffer), &bytesReturned, nullptr);
    CloseHandle(hFile);

    auto* reparse = reinterpret_cast<AppExecLinkReparseBuffer*>(buffer);
    if (!succeeded || bytesReturned < sizeof(AppExecLinkReparseBuffer) ||
        reparse->reparseTag != IO_REPARSE_TAG_APPEXECLINK) {
        return path;
    }

    const WCHAR* p = reparse->stringList;
    const WCHAR* end = reinterpret_cast<const WCHAR*>(buffer + bytesReturned);
    for (int i = 0; i < 3 && p < end; i++) {
        size_t length = wcsnlen(p, end - p);
        if (p + length >= end)
            break;
        if (i == 2)
            return std::wstring(p, length);
        p += length + 1;
    }

    return path;
}

HICON ExtractCommandIcon(std::wstring const& command) {
    std::wstring path = ResolveAppExecutionAlias(ResolveCommandPath(command));
    HICON hIcon = nullptr;
    if (ExtractIconExW(path.c_str(), 0, &hIcon, nullptr, 1) && hIcon)
        return hIcon;
    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(path.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }
    Wh_Log(L"Couldn't get an icon for %s (%s)", command.c_str(), path.c_str());
    return nullptr;
}

struct DecodedIcon {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;
    bool empty() const { return pixels.empty(); }
};

bool ReadBitmapPixels(HBITMAP hBitmap, DecodedIcon* decoded) {
    BITMAP bm{};
    if (!GetObject(hBitmap, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0)
        return false;

    int width = bm.bmWidth;
    int height = bm.bmHeight;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<uint8_t> pixels((size_t)width * height * 4);
    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc)
        return false;

    bool succeeded = GetDIBits(hdc, hBitmap, 0, height, pixels.data(), &bmi,
                               DIB_RGB_COLORS) != 0;
    DeleteDC(hdc);

    if (!succeeded)
        return false;
    decoded->width = width;
    decoded->height = height;
    decoded->pixels = std::move(pixels);
    return true;
}

bool HasNoAlphaChannel(std::vector<uint8_t> const& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        if (pixels[p])
            return false;
    }
    return true;
}

void MakeOpaque(std::vector<uint8_t>& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4)
        pixels[p] = 255;
}

bool DecodeMonochromeIcon(HBITMAP hbmMask, DecodedIcon* decoded) {
    DecodedIcon mask;
    if (!ReadBitmapPixels(hbmMask, &mask) || mask.height % 2 != 0)
        return false;

    int width = mask.width;
    int height = mask.height / 2;

    DecodedIcon result;
    result.width = width;
    result.height = height;
    result.pixels.assign((size_t)width * height * 4, 0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t andIndex = ((size_t)y * width + x) * 4;
            size_t xorIndex = ((size_t)(y + height) * width + x) * 4;
            if (mask.pixels[andIndex])
                continue;
            uint8_t value = mask.pixels[xorIndex] ? 255 : 0;
            result.pixels[andIndex + 0] = value;
            result.pixels[andIndex + 1] = value;
            result.pixels[andIndex + 2] = value;
            result.pixels[andIndex + 3] = 255;
        }
    }

    *decoded = std::move(result);
    return true;
}

bool DecodeIcon(HICON hIcon, DecodedIcon* decoded) {
    ICONINFO iconInfo{};
    if (!GetIconInfo(hIcon, &iconInfo))
        return false;

    bool succeeded = false;
    if (iconInfo.hbmColor) {
        if (ReadBitmapPixels(iconInfo.hbmColor, decoded)) {
            if (HasNoAlphaChannel(decoded->pixels)) {
                MakeOpaque(decoded->pixels);
            } else {
                for (size_t p = 0; p < decoded->pixels.size(); p += 4) {
                    uint8_t alpha = decoded->pixels[p + 3];
                    if (alpha != 255) {
                        decoded->pixels[p] = decoded->pixels[p] * alpha / 255;
                        decoded->pixels[p + 1] =
                            decoded->pixels[p + 1] * alpha / 255;
                        decoded->pixels[p + 2] =
                            decoded->pixels[p + 2] * alpha / 255;
                    }
                }
            }
            succeeded = true;
        }
    } else if (iconInfo.hbmMask) {
        succeeded = DecodeMonochromeIcon(iconInfo.hbmMask, decoded);
    }

    if (iconInfo.hbmColor)
        DeleteObject(iconInfo.hbmColor);
    if (iconInfo.hbmMask)
        DeleteObject(iconInfo.hbmMask);
    return succeeded;
}

muxm::ImageSource CreateImageSource(DecodedIcon const& decoded) try {
    if (decoded.empty())
        return nullptr;
    muxm::Imaging::WriteableBitmap bitmap(decoded.width, decoded.height);
    memcpy(bitmap.PixelBuffer().data(), decoded.pixels.data(),
           decoded.pixels.size());
    bitmap.Invalidate();
    return bitmap;
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
    return nullptr;
}

std::wstring ParseGlyphSetting(PCWSTR glyphSetting) {
    if (!glyphSetting[0])
        return std::wstring();
    if (!glyphSetting[1])
        return std::wstring(1, glyphSetting[0]);
    for (PCWSTR p = glyphSetting; *p; p++) {
        if (!iswxdigit(*p)) {
            Wh_Log(L"%s is not a glyph code point", glyphSetting);
            return std::wstring();
        }
    }
    unsigned long parsed = wcstoul(glyphSetting, nullptr, 16);
    if (parsed > 0 && parsed <= 0xFFFF)
        return std::wstring(1, (WCHAR)parsed);
    return std::wstring();
}

bool LooksLikeIconPath(std::wstring const& iconSetting) {
    if (iconSetting.find(L'\\') != std::wstring::npos ||
        iconSetting.find(L'/') != std::wstring::npos ||
        iconSetting.find(L':') != std::wstring::npos)
        return true;

    std::wstring lower = ToLower(iconSetting);
    size_t comma = lower.rfind(L',');
    if (comma != std::wstring::npos)
        lower.resize(comma);

    return lower.ends_with(L".exe") || lower.ends_with(L".dll") ||
           lower.ends_with(L".ico");
}

HICON LoadIconFromPath(std::wstring const& iconPath) {
    std::wstring expanded = ExpandEnvVars(iconPath);
    int iconIndex = 0;
    size_t comma = expanded.rfind(L',');
    if (comma != std::wstring::npos && comma + 1 < expanded.size()) {
        bool isNumber = true;
        for (size_t i = comma + 1; i < expanded.size(); i++) {
            WCHAR c = expanded[i];
            if ((c < L'0' || c > L'9') && !(i == comma + 1 && c == L'-')) {
                isNumber = false;
                break;
            }
        }
        if (isNumber) {
            iconIndex = _wtoi(expanded.c_str() + comma + 1);
            expanded.resize(comma);
        }
    }

    expanded = ResolveAppExecutionAlias(expanded);
    HICON hIcon = nullptr;
    if (ExtractIconExW(expanded.c_str(), iconIndex, &hIcon, nullptr, 1) &&
        hIcon)
        return hIcon;

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(expanded.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }
    Wh_Log(L"Couldn't load an icon from %s", iconPath.c_str());
    return nullptr;
}

bool IsShellPath(std::wstring const& s) {
    return s.size() >= 6 && _wcsnicmp(s.c_str(), L"shell:", 6) == 0;
}

bool DecodeShellPathIcon(std::wstring const& path, DecodedIcon* decoded) {
    std::wstring expanded = ExpandEnvVars(path);
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(
            SHParseDisplayName(expanded.c_str(), nullptr, &pidl, 0, nullptr)) ||
        !pidl) {
        Wh_Log(L"Couldn't parse shell path %s", path.c_str());
        return false;
    }

    winrt::com_ptr<IShellItemImageFactory> factory;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(factory.put()));
    CoTaskMemFree(pidl);
    if (FAILED(hr) || !factory)
        return false;

    SIZE size = {32, 32};
    HBITMAP hBitmap = nullptr;
    hr = factory->GetImage(size, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                           &hBitmap);
    if (FAILED(hr) || !hBitmap)
        return false;

    bool succeeded = ReadBitmapPixels(hBitmap, decoded);
    DeleteObject(hBitmap);
    if (succeeded && HasNoAlphaChannel(decoded->pixels))
        MakeOpaque(decoded->pixels);
    return succeeded;
}

std::shared_ptr<DecodedIcon> ResolveIcon(std::wstring const& iconSetting,
                                         std::wstring const& command) {
    auto decoded = std::make_shared<DecodedIcon>();
    bool isPath = !iconSetting.empty() && LooksLikeIconPath(iconSetting);
    if (isPath && IsShellPath(iconSetting)) {
        DecodeShellPathIcon(iconSetting, decoded.get());
        return decoded;
    }

    HICON hIcon = nullptr;
    if (isPath) {
        hIcon = LoadIconFromPath(iconSetting);
    } else if (iconSetting.empty() && !command.empty()) {
        hIcon = ExtractCommandIcon(command);
    }

    if (hIcon) {
        DecodeIcon(hIcon, decoded.get());
        DestroyIcon(hIcon);
    }
    return decoded;
}

std::mutex g_iconCacheMutex;
std::unordered_map<std::wstring, std::shared_ptr<DecodedIcon>> g_iconCache;

std::shared_ptr<DecodedIcon> GetIcon(std::wstring const& iconSetting,
                                     std::wstring const& command) {
    std::wstring key = iconSetting + L'\n' + command;
    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        auto it = g_iconCache.find(key);
        if (it != g_iconCache.end())
            return it->second;
    }

    auto decoded = ResolveIcon(iconSetting, command);
    std::lock_guard<std::mutex> lock(g_iconCacheMutex);
    return g_iconCache.insert_or_assign(std::move(key), std::move(decoded))
        .first->second;
}

muxc::IconElement CreateGlyphIcon(PCWSTR glyph) {
    muxc::FontIcon fontIcon;
    fontIcon.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    fontIcon.Glyph(glyph);
    return fontIcon;
}

bool IsSystemDarkModeActive() {
    DWORD useLightTheme = 1;
    DWORD size = sizeof(useLightTheme);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &useLightTheme, &size);
    return useLightTheme == 0;
}

std::wstring TrimWhitespaceAndQuotes(std::wstring str) {
    size_t first = str.find_first_not_of(L" \t");
    if (first == std::wstring::npos)
        return L"";
    size_t last = str.find_last_not_of(L" \t");
    str = str.substr(first, last - first + 1);
    if (str.size() >= 2 && str.front() == L'"' && str.back() == L'"') {
        str = str.substr(1, str.size() - 2);
        first = str.find_first_not_of(L" \t");
        if (first == std::wstring::npos)
            return L"";
        last = str.find_last_not_of(L" \t");
        str = str.substr(first, last - first + 1);
    }
    return str;
}

std::wstring ResolveThemedIconString(const std::wstring& rawSetting) {
    size_t pipePos = rawSetting.find(L'|');

    // Single icon provided (no pipe): applied to both themes
    if (pipePos == std::wstring::npos) {
        return TrimWhitespaceAndQuotes(rawSetting);
    }

    // Split at the pipe: [Light Theme | Dark Theme]
    std::wstring lightTheme =
        TrimWhitespaceAndQuotes(rawSetting.substr(0, pipePos));
    std::wstring darkTheme =
        TrimWhitespaceAndQuotes(rawSetting.substr(pipePos + 1));

    // Fallbacks if one side is left empty
    if (lightTheme.empty())
        lightTheme = darkTheme;
    if (darkTheme.empty())
        darkTheme = lightTheme;

    return IsSystemDarkModeActive() ? darkTheme : lightTheme;
}

muxc::IconElement TryCreateIconElement(std::wstring const& iconSetting,
                                       std::wstring const& command) {
    std::wstring effectiveSetting = ResolveThemedIconString(iconSetting);

    bool isPath =
        !effectiveSetting.empty() && LooksLikeIconPath(effectiveSetting);
    if (isPath || effectiveSetting.empty()) {
        if (auto source =
                CreateImageSource(*GetIcon(effectiveSetting, command))) {
            muxc::ImageIcon imageIcon;
            imageIcon.Source(source);
            return imageIcon;
        }
    }

    std::wstring glyph;
    if (!isPath)
        glyph = ParseGlyphSetting(effectiveSetting.c_str());
    if (glyph.empty())
        return nullptr;
    return CreateGlyphIcon(glyph.c_str());
}

muxc::IconElement CreateIconElement(std::wstring const& iconSetting,
                                    std::wstring const& command,
                                    PCWSTR defaultGlyph) {
    if (auto icon = TryCreateIconElement(iconSetting, command))
        return icon;
    return CreateGlyphIcon(defaultGlyph);
}

PCWSTR InternalCommandGlyph(std::wstring_view cmd) {
    if (IsCommandMatch(cmd, L"OpenWith"))
        return L"\uE7AC";
    if (IsCommandMatch(cmd, L"TogglePreview"))
        return L"\uE8A1";
    if (IsCommandMatch(cmd, L"ToggleDetails"))
        return L"\uE9F9";
    if (IsCommandMatch(cmd, L"FolderOptions"))
        return L"\uE713";
    if (IsCommandMatch(cmd, L"CopyPath"))
        return L"\uE8C8";
    if (IsCommandMatch(cmd, L"CopyName"))
        return L"\uE8AC";
    if (IsCommandMatch(cmd, L"CopyDir"))
        return L"\uE8B7";
    if (IsCommandMatch(cmd, L"NewFolder"))
        return L"\uE8F4";
    if (IsCommandMatch(cmd, L"NewFile"))
        return L"\uE8A5";
    if (IsCommandMatch(cmd, L"Refresh"))
        return L"\uE72C";
    return nullptr;
}

muxc::IconElement MakeCommandButtonIcon(ActionItem const& item) {
    if (item.hideIcon)
        return nullptr;

    PCWSTR defaultGlyph = InternalCommandGlyph(item.command);
    return CreateIconElement(item.icon, item.command,
                             defaultGlyph ? defaultGlyph : L"\uE783");
}

muxc::IconElement MakeNewPlusButtonIcon(std::wstring const& iconSetting,
                                        std::wstring const& originalIconUri) {
    if (iconSetting.empty() && !originalIconUri.empty()) {
        try {
            muxm::Imaging::SvgImageSource svgSource(
                wf::Uri(winrt::hstring{originalIconUri}));
            muxc::ImageIcon imageIcon;
            imageIcon.Source(svgSource);
            return imageIcon;
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
    return CreateIconElement(iconSetting, std::wstring(), L"");
}

muxm::ImageSource LoadShellItemIcon(std::wstring const& path) {
    DecodedIcon decoded;
    if (!DecodeShellPathIcon(path, &decoded))
        return nullptr;
    return CreateImageSource(decoded);
}

bool IsOurNewPlusButton(muxc::ICommandBarElement const& command) {
    auto element = command.try_as<mux::FrameworkElement>();
    return element && element.Name() == kNewPlusButtonName;
}

bool IsOurElement(muxc::ICommandBarElement const& command) {
    auto element = command.try_as<mux::FrameworkElement>();
    if (!element)
        return false;
    std::wstring_view name{element.Name()};
    return name.starts_with(kButtonNamePrefix) || name == kNewPlusButtonName ||
           name == kContextMenuButtonName || name == kSettingsButtonName;
}

bool HasElement(muxc::CommandBar const& commandBar,
                bool (*predicate)(muxc::ICommandBarElement const&)) {
    for (auto const& command : commandBar.PrimaryCommands()) {
        if (predicate(command))
            return true;
    }
    return false;
}

std::wstring GetButtonIconUri(muxc::AppBarButton const& button) try {
    auto icon = button.Icon();
    if (!icon)
        return std::wstring();

    wf::Uri uri{nullptr};
    if (auto imageIcon = icon.try_as<muxc::ImageIcon>()) {
        if (auto source = imageIcon.Source()) {
            if (auto svgSource =
                    source.try_as<muxm::Imaging::SvgImageSource>()) {
                uri = svgSource.UriSource();
            }
        }
    } else if (auto iconSourceElement =
                   icon.try_as<muxc::IconSourceElement>()) {
        if (auto iconSource = iconSourceElement.IconSource()) {
            if (auto imageIconSource =
                    iconSource.try_as<muxc::ImageIconSource>()) {
                if (auto source = imageIconSource.ImageSource()) {
                    if (auto svgSource =
                            source.try_as<muxm::Imaging::SvgImageSource>()) {
                        uri = svgSource.UriSource();
                    }
                }
            }
        }
    } else if (auto bitmapIcon = icon.try_as<muxc::BitmapIcon>()) {
        uri = bitmapIcon.UriSource();
    }

    if (!uri)
        return std::wstring();
    return std::wstring{uri.AbsoluteUri()};
} catch (...) {
    return std::wstring();
}

int IdentifyDefaultButton(muxc::AppBarButton const& button) {
    std::wstring uri = ToLower(GetButtonIconUri(button));
    if (!uri.empty()) {
        size_t slash = uri.find_last_of(L'/');
        std::wstring_view fileName =
            slash == std::wstring::npos
                ? std::wstring_view(uri)
                : std::wstring_view(uri).substr(slash + 1);
        if (fileName.starts_with(L"windows.iconsize."))
            return kViewButtonIndex;

        for (int i = 0; i < kDefaultButtonCount; i++) {
            if (fileName == kDefaultButtons[i].svgFileName)
                return i;
        }
    }

    try {
        if (mux::Automation::AutomationProperties::GetAutomationId(button) ==
            L"SortAndGroupButton") {
            return kSortButtonIndex;
        }
    } catch (...) {
    }

    return -1;
}

struct DefaultButtonLook {
    std::wstring iconUri;
    std::wstring label;
};

// Remembered for the command bar preview of the settings window, which shows
// the built-in buttons with File Explorer's own icons and labels.
std::mutex g_defaultButtonLooksMutex;
DefaultButtonLook g_defaultButtonLooks[kDefaultButtonCount];
DefaultButtonLook g_detailsButtonLook;

void RememberDefaultButtonLook(DefaultButtonLook* look,
                               muxc::AppBarButton const& button) {
    std::wstring iconUri = GetButtonIconUri(button);
    std::wstring label{button.Label()};
    std::lock_guard<std::mutex> lock(g_defaultButtonLooksMutex);
    if (!iconUri.empty())
        look->iconUri = std::move(iconUri);
    if (!label.empty())
        look->label = std::move(label);
}

std::wstring GetAutomationId(mux::FrameworkElement const& element) {
    try {
        return std::wstring{
            mux::Automation::AutomationProperties::GetAutomationId(element)};
    } catch (...) {
        return std::wstring();
    }
}

enum class ManagedKind {
    Button,
    SeparatorAfter,
    GroupSeparator,
    DetailsToggle,
    OverflowElement,
    UnusedSeparator,
};

struct GroupMember {
    winrt::weak_ref<mux::UIElement> element;
    int defaultButtonIndex;
};

struct ManagedTarget {
    ManagedKind kind;
    int index = 0;
    bool newPlusPresent = false;
    std::shared_ptr<std::vector<GroupMember>> group;
};

bool ShouldHide(ManagedTarget const& target);

mux::Visibility EffectiveVisibility(mux::UIElement const& element) {
    auto* entry = FindManagedElement(element);
    if (entry && entry->hasOriginalVisibility)
        return entry->originalVisibility;
    return element.Visibility();
}

bool IsGroupHidden(std::shared_ptr<std::vector<GroupMember>> const& group) {
    if (!group || group->empty())
        return false;
    for (auto const& member : *group) {
        auto element = member.element.get();
        if (!element)
            continue;
        if (member.defaultButtonIndex >= 0 &&
            ShouldHide({ManagedKind::Button, member.defaultButtonIndex}))
            continue;
        if (EffectiveVisibility(element) == mux::Visibility::Collapsed)
            continue;
        return false;
    }
    return true;
}

bool ShouldHide(ManagedTarget const& target) {
    if (target.kind == ManagedKind::GroupSeparator)
        return IsGroupHidden(target.group);

    std::lock_guard<std::mutex> lock(g_settings.mutex);
    switch (target.kind) {
        case ManagedKind::Button:
            if (target.index == kNewButtonIndex && target.newPlusPresent &&
                g_settings.newPlus.enabled &&
                !g_settings.newPlus.keepOriginalNewButton) {
                return true;
            }
            return g_settings.hideDefaultButtons[target.index];
        case ManagedKind::SeparatorAfter:
            return g_settings.hideSeparatorAfterButton[target.index];
        case ManagedKind::DetailsToggle:
            return g_settings.hideDetailsButton;
        case ManagedKind::OverflowElement:
            return g_settings.hideMoreButton;
        case ManagedKind::UnusedSeparator:
            return true;
        case ManagedKind::GroupSeparator:
            break;
    }
    return false;
}

thread_local int g_settingVisibilityDepth;

void SetVisibilityInternal(mux::UIElement const& element,
                           mux::Visibility visibility) {
    g_settingVisibilityDepth++;
    try {
        element.Visibility(visibility);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
    g_settingVisibilityDepth--;
}

void ApplyDefaultButtonVisibility(muxc::CommandBar const& commandBar,
                                  bool forceShow = false);
void UpdateCommandBar(muxc::CommandBar const& commandBar);

thread_local std::unordered_map<void*, bool> g_pendingUpdates;

void QueueCommandBarUpdate(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar,
    bool fullUpdate) {
    auto commandBar = weakCommandBar.get();
    if (!commandBar)
        return;

    void* key = winrt::get_abi(commandBar);
    auto [it, inserted] = g_pendingUpdates.insert({key, fullUpdate});
    if (!inserted) {
        it->second = it->second || fullUpdate;
        return;
    }

    auto takePending = [key]() {
        bool full = false;
        auto it = g_pendingUpdates.find(key);
        if (it != g_pendingUpdates.end()) {
            full = it->second;
            g_pendingUpdates.erase(it);
        }
        return full;
    };

    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        takePending();
        return;
    }

    // If the update can't be queued, drop the pending entry, or every later
    // update of this command bar would be merged into it and never run.
    if (!dispatcherQueue.TryEnqueue([weakCommandBar, takePending]() {
            bool full = takePending();
            if (g_unloading)
                return;
            if (auto commandBar = weakCommandBar.get()) {
                try {
                    if (full)
                        UpdateCommandBar(commandBar);
                    else
                        ApplyDefaultButtonVisibility(commandBar);
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        })) {
        takePending();
    }
}

void QueueVisibilityRecompute(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar) {
    QueueCommandBarUpdate(weakCommandBar, false);
}

void WatchVisibility(mux::UIElement const& element,
                     ManagedTarget const& target,
                     winrt::weak_ref<muxc::CommandBar> const& owner) {
    int64_t token = element.RegisterPropertyChangedCallback(
        mux::UIElement::VisibilityProperty(),
        [target, owner](mux::DependencyObject const& sender,
                        mux::DependencyProperty const&) {
            if (g_unloading || g_settingVisibilityDepth > 0)
                return;
            auto element = sender.try_as<mux::UIElement>();
            if (!element)
                return;

            mux::Visibility visibility = element.Visibility();
            bool unusedSeparator;
            {
                auto& entry = GetManagedElement(element);
                entry.originalVisibility = visibility;
                entry.hasOriginalVisibility = true;
                unusedSeparator = entry.unusedSeparator;
            }

            // The role of a separator changes with the button order, so the
            // recompute below decides for those.
            if (target.kind != ManagedKind::GroupSeparator &&
                target.kind != ManagedKind::UnusedSeparator &&
                visibility != mux::Visibility::Collapsed &&
                (unusedSeparator || ShouldHide(target))) {
                SetVisibilityInternal(element, mux::Visibility::Collapsed);
            }
            QueueVisibilityRecompute(owner);
        });

    auto& entry = GetManagedElement(element);
    entry.watched = true;
    entry.visibilityToken = token;
}

void UnwatchVisibilityForCurrentThread() {
    for (auto& entry : g_managedElements) {
        if (!entry.watched)
            continue;
        auto element = entry.element.get();
        int64_t token = entry.visibilityToken;
        entry.watched = false;
        entry.visibilityToken = 0;
        if (!element)
            continue;
        try {
            element.UnregisterPropertyChangedCallback(
                mux::UIElement::VisibilityProperty(), token);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

void ForgetManagedElementsForCurrentThread() {
    g_managedElements.clear();
}

void SetManagedVisibility(mux::UIElement const& element,
                          ManagedTarget const& target,
                          bool forceShow,
                          winrt::weak_ref<muxc::CommandBar> const& owner) {
    mux::Visibility original;
    bool watch;
    {
        auto& entry = GetManagedElement(element);
        if (!entry.hasOriginalVisibility) {
            entry.originalVisibility = element.Visibility();
            entry.hasOriginalVisibility = true;
        }
        original = entry.originalVisibility;
        watch = !forceShow && !entry.watched;
    }

    if (watch)
        WatchVisibility(element, target, owner);
    SetVisibilityInternal(element, !forceShow && ShouldHide(target)
                                       ? mux::Visibility::Collapsed
                                       : original);
}

void ApplyItemSpacing(muxc::AppBarButton const& button,
                      int spacing,
                      bool reset) {
    mux::Thickness originalMargin;
    double originalMinWidth;
    {
        auto& entry = GetManagedElement(button);
        if (!entry.hasOriginalSpacing) {
            entry.originalMargin = button.Margin();
            entry.originalMinWidth = button.MinWidth();
            entry.hasOriginalSpacing = true;
        }
        originalMargin = entry.originalMargin;
        originalMinWidth = entry.originalMinWidth;
    }

    if (reset || spacing < 0) {
        button.Margin(originalMargin);
        button.MinWidth(originalMinWidth);
        return;
    }

    double half = spacing / 2.0;
    mux::Thickness margin = originalMargin;
    margin.Left = half;
    margin.Right = half;
    button.Margin(margin);
    button.MinWidth(0);
}

// The dropdown chevron which File Explorer draws after the label of its New,
// Sort and View buttons. The overflow menu uses OverflowSubItemChevron.
mux::FrameworkElement FindSubItemChevron(mux::DependencyObject const& root) {
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto element = child.try_as<mux::FrameworkElement>();
            element && element.Name() == L"SubItemChevron") {
            return element;
        }
        if (auto found = FindSubItemChevron(child))
            return found;
    }
    return nullptr;
}

void RestoreChevron(ManagedElement::HiddenChevron const& hidden) {
    auto element = hidden.element.get();
    if (!element)
        return;
    try {
        element.UnregisterPropertyChangedCallback(
            mux::UIElement::VisibilityProperty(), hidden.visibilityToken);
        element.Visibility(hidden.originalVisibility);
        element.Opacity(hidden.originalOpacity);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void RestoreChevronsForCurrentThread() {
    for (auto& entry : g_managedElements) {
        entry.wantChevronsHidden = false;
        if (entry.hiddenChevron.element)
            RestoreChevron(entry.hiddenChevron);
        entry.hiddenChevron = {};
    }
}

// With the label of a built-in dropdown button hidden, its chevron would be
// drawn next to the icon, so it's hidden as well. The template's visual states
// can make it visible again, so its visibility is watched while hidden.
void ApplyChevronHiding(muxc::AppBarButton const& button, bool hide) try {
    ManagedElement::HiddenChevron hidden;
    bool hookLoaded = false;
    {
        auto* entry = FindManagedElement(button);
        if (!entry) {
            if (!hide)
                return;
            entry = &GetManagedElement(button);
        }
        entry->wantChevronsHidden = hide;
        hidden = entry->hiddenChevron;
        if (hide && !entry->chevronLoadedHooked) {
            entry->chevronLoadedHooked = true;
            hookLoaded = true;
        }
    }

    if (hookLoaded) {
        // The template may not be applied yet, and is re-applied when the
        // button is reloaded.
        TrackRevoker(
            button,
            button.Loaded(winrt::auto_revoke, [](wf::IInspectable const& sender,
                                                 mux::RoutedEventArgs const&) {
                auto button = sender.try_as<muxc::AppBarButton>();
                if (!button || g_unloading)
                    return;
                auto* entry = FindManagedElement(button);
                if (entry && entry->wantChevronsHidden)
                    ApplyChevronHiding(button, true);
            }));
    }

    mux::FrameworkElement chevron{nullptr};
    if (hide)
        chevron = FindSubItemChevron(button);

    // Restore a chevron which is no longer wanted hidden, or which was
    // replaced by a re-applied template.
    if (hidden.element && (!hide || hidden.element.get() != chevron)) {
        RestoreChevron(hidden);
        hidden = {};
    }

    if (chevron && !hidden.element) {
        hidden.element = winrt::make_weak<mux::UIElement>(chevron);
        hidden.originalVisibility = chevron.Visibility();
        hidden.originalOpacity = chevron.Opacity();
        auto weakButton = winrt::make_weak(button);
        hidden.visibilityToken = chevron.RegisterPropertyChangedCallback(
            mux::UIElement::VisibilityProperty(),
            [weakButton](mux::DependencyObject const& sender,
                         mux::DependencyProperty const&) {
                if (g_unloading)
                    return;
                auto button = weakButton.get();
                auto element = sender.try_as<mux::UIElement>();
                if (!button || !element)
                    return;
                auto* entry = FindManagedElement(button);
                if (entry && entry->wantChevronsHidden &&
                    element.Visibility() != mux::Visibility::Collapsed) {
                    element.Visibility(mux::Visibility::Collapsed);
                }
            });
    }

    if (chevron) {
        if (chevron.Visibility() != mux::Visibility::Collapsed)
            chevron.Visibility(mux::Visibility::Collapsed);
        if (chevron.Opacity() != 0)
            chevron.Opacity(0);
    }

    if (auto* entry = FindManagedElement(button))
        entry->hiddenChevron = hidden;
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Collapses the label of a built-in button, leaving only its icon.
void ApplyLabelHiding(muxc::AppBarButton const& button, bool hide) {
    muxc::CommandBarLabelPosition original;
    bool addedToolTip;
    {
        auto& entry = GetManagedElement(button);
        if (!entry.hasOriginalLabelPosition) {
            if (!hide)
                return;
            entry.originalLabelPosition = button.LabelPosition();
            entry.hasOriginalLabelPosition = true;
        }
        original = entry.originalLabelPosition;
        addedToolTip = entry.addedToolTip;
    }

    auto position = hide ? muxc::CommandBarLabelPosition::Collapsed : original;
    if (button.LabelPosition() != position)
        button.LabelPosition(position);
    ApplyChevronHiding(button, hide);

    // Without its label, the button gets it as a tooltip, which is removed
    // again along with the label hiding.
    bool toolTipChanged = false;
    if (hide && !addedToolTip && !button.Label().empty() &&
        !muxc::ToolTipService::GetToolTip(button)) {
        muxc::ToolTipService::SetToolTip(button,
                                         winrt::box_value(button.Label()));
        addedToolTip = true;
        toolTipChanged = true;
    } else if (!hide && addedToolTip) {
        muxc::ToolTipService::SetToolTip(button, nullptr);
        addedToolTip = false;
        toolTipChanged = true;
    }
    if (toolTipChanged)
        GetManagedElement(button).addedToolTip = addedToolTip;
}

mux::FrameworkElement FindDescendantByName(mux::DependencyObject const& root,
                                           std::wstring_view name) {
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto element = child.try_as<mux::FrameworkElement>();
            element && element.Name() == name) {
            return element;
        }
        if (auto found = FindDescendantByName(child, name))
            return found;
    }
    return nullptr;
}

void ApplyDefaultButtonVisibility(muxc::CommandBar const& commandBar,
                                  bool forceShow) {
    bool hideMore;
    int itemSpacing;
    bool hideLabels[kDefaultButtonCount];
    bool hideDetailsLabel;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        hideMore = g_settings.hideMoreButton;
        itemSpacing = g_settings.itemSpacing;
        for (int i = 0; i < kDefaultButtonCount; i++)
            hideLabels[i] = g_settings.hideDefaultButtonLabels[i];
        hideDetailsLabel = g_settings.hideDetailsButtonLabel;
    }

    bool isPrimary = commandBar.Name() == L"FileExplorerCommandBar";
    auto weakCommandBar = winrt::make_weak(commandBar);
    auto setVisibility = [&weakCommandBar, forceShow](
                             mux::UIElement const& element,
                             ManagedTarget const& target) {
        SetManagedVisibility(element, target, forceShow, weakCommandBar);
    };

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    struct Entry {
        muxc::ICommandBarElement command;
        bool isOurs = false;
        bool isNewPlus = false;
        bool isSeparator = false;
        bool isDetailsToggle = false;
        int defaultIndex = -1;
    };

    std::vector<Entry> entries;
    entries.reserve(count);

    for (uint32_t i = 0; i < count; i++) {
        Entry entry;
        entry.command = commands.GetAt(i);
        entry.isOurs = IsOurElement(entry.command);
        entry.isNewPlus = entry.isOurs && IsOurNewPlusButton(entry.command);
        entry.isSeparator =
            !entry.isOurs &&
            static_cast<bool>(entry.command.try_as<muxc::AppBarSeparator>());

        if (!entry.isOurs && !entry.isSeparator) {
            if (auto button = entry.command.try_as<muxc::AppBarButton>()) {
                if (GetAutomationId(button) == L"DetailsPaneToggleButton") {
                    entry.isDetailsToggle = true;
                    RememberDefaultButtonLook(&g_detailsButtonLook, button);
                } else {
                    entry.defaultIndex = IdentifyDefaultButton(button);
                    if (entry.defaultIndex >= 0) {
                        RememberDefaultButtonLook(
                            &g_defaultButtonLooks[entry.defaultIndex], button);
                    } else {
                        Wh_Log(
                            L"Unrecognized command %u: icon %s, automation id "
                            L"%s",
                            i, GetButtonIconUri(button).c_str(),
                            GetAutomationId(button).c_str());
                    }
                }
            }
        }
        entries.push_back(std::move(entry));
    }

    uint32_t firstCustomIndex = count;
    bool hasNewPlusButton = false;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].isNewPlus)
            hasNewPlusButton = true;
        else if (entries[i].isOurs && firstCustomIndex == count)
            firstCustomIndex = i;
    }

    uint32_t viewSeparatorIndex = count;
    if (firstCustomIndex > 0 && firstCustomIndex < count &&
        entries[firstCustomIndex - 1].isSeparator) {
        viewSeparatorIndex = firstCustomIndex - 1;
    } else {
        for (uint32_t i = count; i > 0; i--) {
            if (entries[i - 1].isSeparator) {
                viewSeparatorIndex = i - 1;
                break;
            }
        }
    }

    auto collectGroup = [&entries, count](uint32_t separatorIndex) {
        auto group = std::make_shared<std::vector<GroupMember>>();
        for (uint32_t i = separatorIndex + 1; i < count; i++) {
            auto const& entry = entries[i];
            if (entry.isNewPlus)
                continue;
            if (entry.isSeparator || entry.isOurs)
                break;
            if (auto element = entry.command.try_as<mux::UIElement>()) {
                group->push_back(
                    {winrt::make_weak(element), entry.defaultIndex});
            }
        }
        return group;
    };

    int prevIndex = -1;
    for (uint32_t i = 0; i < count; i++) {
        auto const& entry = entries[i];
        if (entry.isOurs) {
            if (auto button = entry.command.try_as<muxc::AppBarButton>()) {
                ApplyItemSpacing(button, itemSpacing, forceShow);
            }
            prevIndex = -1;
            continue;
        }

        if (entry.isSeparator) {
            auto separator = entry.command.as<muxc::AppBarSeparator>();
            if (auto* managed = FindManagedElement(separator);
                managed && managed->unusedSeparator) {
                setVisibility(separator, {ManagedKind::UnusedSeparator});
                prevIndex = -1;
                continue;
            }

            int target = -1;
            if (prevIndex == kNewButtonIndex)
                target = kNewButtonIndex;
            else if (prevIndex == kDeleteButtonIndex)
                target = kDeleteButtonIndex;
            else if (i == viewSeparatorIndex)
                target = kViewButtonIndex;

            if (target >= 0) {
                setVisibility(separator, {ManagedKind::SeparatorAfter, target});
            } else {
                setVisibility(separator, {ManagedKind::GroupSeparator, -1,
                                          false, collectGroup(i)});
            }
            prevIndex = -1;
            continue;
        }

        if (entry.isDetailsToggle) {
            setVisibility(entry.command.as<mux::UIElement>(),
                          {ManagedKind::DetailsToggle});
            if (auto button = entry.command.try_as<muxc::AppBarButton>())
                ApplyLabelHiding(button, !forceShow && hideDetailsLabel);
            prevIndex = -1;
            continue;
        }

        if (entry.defaultIndex >= 0) {
            auto button = entry.command.as<muxc::AppBarButton>();
            setVisibility(button, {ManagedKind::Button, entry.defaultIndex,
                                   hasNewPlusButton});
            ApplyItemSpacing(button, itemSpacing, forceShow);
            ApplyLabelHiding(button,
                             !forceShow && hideLabels[entry.defaultIndex]);
        }

        prevIndex = entry.defaultIndex;
    }

    if (isPrimary) {
        muxc::CommandBarOverflowButtonVisibility originalOverflow;
        {
            auto& entry = GetManagedElement(commandBar);
            if (!entry.hasOriginalOverflow) {
                entry.originalOverflow = commandBar.OverflowButtonVisibility();
                entry.hasOriginalOverflow = true;
            }
            originalOverflow = entry.originalOverflow;
        }

        commandBar.OverflowButtonVisibility(
            !forceShow && hideMore
                ? muxc::CommandBarOverflowButtonVisibility::Collapsed
                : originalOverflow);
        if (auto overflowSeparator =
                FindDescendantByName(commandBar, L"OverflowSeparator")) {
            setVisibility(overflowSeparator, {ManagedKind::OverflowElement});
        }
    }
}

muxc::AppBarButton CreateBareButton(int index,
                                    std::wstring const& tooltip,
                                    std::wstring const& labelText,
                                    muxc::IconElement const& icon,
                                    bool showLabel = false) {
    std::wstring name = kButtonNamePrefix;
    name += L'_';
    name += std::to_wstring(index);

    muxc::AppBarButton button;
    button.Name(name.c_str());

    std::wstring displayLabel = labelText.empty() ? tooltip : labelText;
    button.Label(displayLabel.c_str());
    button.LabelPosition(showLabel ? muxc::CommandBarLabelPosition::Default
                                   : muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(icon);

    if (!tooltip.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{tooltip}));
    }

    return button;
}

muxc::IconElement TryCreateIconElement(std::wstring const& iconSetting,
                                       std::wstring const& command);
muxc::IconElement MakeCommandButtonIcon(ActionItem const& item);

muxc::AppBarButton CreateActionButton(ActionItem const& item, int index) {
    std::wstring displayLabel = item.GetDisplayLabel();
    ExplorerContext emptyCtx;
    std::wstring fullResolved = (displayLabel.find(L'%') != std::wstring::npos)
                                    ? ResolveLabelTokens(displayLabel, emptyCtx)
                                    : displayLabel;
    std::wstring displayResolved =
        TruncateWithEllipsis(fullResolved, kMaxToolbarLabelLength);

    muxc::AppBarButton button =
        CreateBareButton(index, fullResolved, displayResolved,
                         MakeCommandButtonIcon(item), item.showLabel);

    TrackRevoker(
        button,
        button.Click(winrt::auto_revoke, [item](wf::IInspectable const& sender,
                                                mux::RoutedEventArgs const&) {
            if (auto element = sender.try_as<mux::FrameworkElement>()) {
                OnActionInvoked(element, item);
            }
        }));

    return button;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton);

muxc::MenuFlyoutItemBase CreateMenuEntry(
    ActionItem const& item,
    winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    std::wstring displayLabel = item.GetDisplayLabel();

    if (item.isMenu && !item.subItems.empty()) {
        muxc::MenuFlyoutSubItem subMenu;
        subMenu.Text(displayLabel.c_str());
        if (!item.hideIcon) {
            if (auto icon = TryCreateIconElement(item.icon, item.command))
                subMenu.Icon(icon);
        }
        AppendMenuEntries(item.subItems, subMenu.Items(), weakButton);
        return subMenu;
    }

    muxc::MenuFlyoutItem menuItem;
    menuItem.Text(displayLabel.c_str());
    if (!item.hideIcon) {
        if (auto icon = TryCreateIconElement(item.icon, item.command))
            menuItem.Icon(icon);
    }

    TrackRevoker(menuItem, menuItem.Click(
                               winrt::auto_revoke,
                               [item, weakButton](wf::IInspectable const&,
                                                  mux::RoutedEventArgs const&) {
                                   if (auto button = weakButton.get())
                                       OnActionInvoked(button, item);
                               }));

    return menuItem;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    for (auto const& item : items) {
        target.Append(CreateMenuEntry(item, weakButton));
        if (item.separatorAfter)
            target.Append(muxc::MenuFlyoutSeparator());
    }
}

struct HoverTimerEntry {
    winrt::weak_ref<muxc::AppBarButton> button;
    mux::DispatcherTimer timer;
    winrt::event_token tickToken;
};

thread_local std::vector<HoverTimerEntry> g_hoverTimers;

void ReleaseHoverTimer(HoverTimerEntry const& entry) {
    try {
        entry.timer.Stop();
        entry.timer.Tick(entry.tickToken);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void TrackHoverTimer(muxc::AppBarButton const& button,
                     mux::DispatcherTimer const& timer,
                     winrt::event_token tickToken) {
    for (auto it = g_hoverTimers.begin(); it != g_hoverTimers.end();) {
        if (!it->button.get()) {
            ReleaseHoverTimer(*it);
            it = g_hoverTimers.erase(it);
        } else {
            ++it;
        }
    }
    g_hoverTimers.push_back({winrt::make_weak(button), timer, tickToken});
}

void StopHoverTimersForCurrentThread() {
    std::vector<HoverTimerEntry> taken;
    taken.swap(g_hoverTimers);
    for (auto const& entry : taken)
        ReleaseHoverTimer(entry);
}

void SetUpOpenOnHover(muxc::AppBarButton const& button,
                      int hoverDelayMs,
                      std::function<void()> const& open) {
    if (hoverDelayMs <= 0) {
        TrackRevoker(
            button,
            button.PointerEntered(
                winrt::auto_revoke,
                [open](wf::IInspectable const&,
                       mux::Input::PointerRoutedEventArgs const&) { open(); }));
        return;
    }

    mux::DispatcherTimer timer;
    timer.Interval(std::chrono::milliseconds(hoverDelayMs));

    auto tickToken = timer.Tick(
        [open](wf::IInspectable const& sender, wf::IInspectable const&) {
            if (auto timer = sender.try_as<mux::DispatcherTimer>())
                timer.Stop();
            open();
        });

    TrackHoverTimer(button, timer, tickToken);

    TrackRevoker(button,
                 button.PointerEntered(
                     winrt::auto_revoke,
                     [timer](wf::IInspectable const&,
                             mux::Input::PointerRoutedEventArgs const&) {
                         timer.Stop();
                         timer.Start();
                     }));

    auto stopTimer = [timer](wf::IInspectable const&,
                             mux::Input::PointerRoutedEventArgs const&) {
        timer.Stop();
    };
    TrackRevoker(button, button.PointerExited(winrt::auto_revoke, stopTimer));
    TrackRevoker(button, button.PointerCanceled(winrt::auto_revoke, stopTimer));
}

std::function<void()> MakeShowFlyoutAction(muxc::AppBarButton const& button) {
    return [weakButton = winrt::make_weak(button)]() {
        auto button = weakButton.get();
        if (!button || g_unloading)
            return;
        if (auto flyout = button.Flyout(); flyout && !flyout.IsOpen())
            flyout.ShowAt(button);
    };
}

constexpr WCHAR kChevronGlyph = L'\uE70D';

// Draws the label of a command bar button followed by a small chevron, like
// File Explorer's own dropdown buttons. With an empty label, only the chevron
// is drawn. Returns false if the label element wasn't found.
bool AddChevronToLabel(mux::FrameworkElement const& button,
                       std::wstring label) try {
    auto textLabel =
        FindDescendantByName(button, L"TextLabel").try_as<muxc::TextBlock>();
    if (!textLabel)
        return false;

    std::wstring currentText = textLabel.Text().c_str();
    auto inlines = textLabel.Inlines();
    if (inlines.Size() > 1 && !currentText.empty() &&
        currentText.back() == kChevronGlyph) {
        return true;
    }

    while (!label.empty() &&
           (label.back() == kChevronGlyph || label.back() == L' ')) {
        label.pop_back();
    }

    inlines.Clear();
    if (!label.empty()) {
        muxd::Run text;
        text.Text(label + L"  ");
        inlines.Append(text);
    }

    muxd::Run chevron;
    chevron.Text(winrt::hstring(&kChevronGlyph, 1));
    chevron.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    chevron.FontSize(8);
    inlines.Append(chevron);
    return true;
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
    return false;
}

void AddNewPlusChevron(muxc::AppBarButton const& button) {
    if (!AddChevronToLabel(button, button.Label().c_str()))
        Wh_Log(L"The New+ label wasn't found");
}

// A custom dropdown button with "Show chevrons on dropdowns" enabled. An
// icon-only button gets the chevron glyph as its label, so that the label
// element exists to draw the chevron in.
void AddMenuButtonChevron(muxc::AppBarButton const& button) {
    std::wstring label = button.Label().c_str();
    if (label.size() == 1 && label[0] == kChevronGlyph)
        label.clear();
    AddChevronToLabel(button, label);
}

// The label of a custom dropdown button. With a chevron but no label, it's
// the chevron glyph, replaced by AddMenuButtonChevron once the button loads.
std::wstring MenuButtonLabel(std::wstring const& label,
                             bool hasLabel,
                             bool showChevron) {
    if (hasLabel)
        return label;
    return showChevron ? std::wstring(1, kChevronGlyph) : label;
}

muxc::AppBarButton CreateMenuButton(ActionItem const& item,
                                    int index,
                                    bool openOnHover,
                                    int hoverDelayMs,
                                    bool showChevron) {
    std::wstring displayLabel = item.GetDisplayLabel();
    ExplorerContext emptyCtx;
    std::wstring fullResolved = (displayLabel.find(L'%') != std::wstring::npos)
                                    ? ResolveLabelTokens(displayLabel, emptyCtx)
                                    : displayLabel;
    std::wstring truncatedDisplay =
        TruncateWithEllipsis(fullResolved, kMaxToolbarLabelLength);

    bool hasLabel = item.showLabel && !truncatedDisplay.empty();
    std::wstring finalLabel = MenuButtonLabel(truncatedDisplay, hasLabel,
                                              showChevron);

    muxc::AppBarButton button =
        CreateBareButton(index, fullResolved, finalLabel,
                         MakeCommandButtonIcon(item), hasLabel || showChevron);

    if (showChevron) {
        TrackRevoker(
            button,
            button.Loaded(winrt::auto_revoke, [](wf::IInspectable const& sender,
                                                 mux::RoutedEventArgs const&) {
                if (auto button = sender.try_as<muxc::AppBarButton>())
                    AddMenuButtonChevron(button);
            }));
    }

    auto weakButton = winrt::make_weak(button);

    muxc::MenuFlyout menu;
    menu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    // Recursively check if ANY deep child has a token
    bool hasChildTokens = false;
    std::function<bool(const std::vector<ActionItem>&)> checkTokens =
        [&](const std::vector<ActionItem>& list) {
            for (const auto& sub : list) {
                if (sub.GetDisplayLabel().find(L'%') != std::wstring::npos)
                    return true;
                if (checkTokens(sub.subItems))
                    return true;
            }
            return false;
        };
    hasChildTokens = checkTokens(item.subItems);

    auto ensureMenuEntries = [subItems = item.subItems, weakButton,
                              hasChildTokens](muxc::MenuFlyout const& menu) {
        if (!menu)
            return;
        try {
            if (menu.Items().Size() > 0 && !hasChildTokens)
                return;

            HWND hWnd = nullptr;
            if (auto btn = weakButton.get()) {
                hWnd = GetExplorerWindowForElement(btn);
            }

            ExplorerContext ctx;
            if (hasChildTokens) {
                ctx = GetExplorerContext(hWnd);
            }

            menu.Items().Clear();
            std::vector<ActionItem> resolvedList = subItems;

            if (hasChildTokens) {
                // Recursively resolve tokens at all menu depths
                std::function<void(std::vector<ActionItem>&)> resolveDeep =
                    [&](std::vector<ActionItem>& list) {
                        for (auto& entry : list) {
                            std::wstring label = entry.GetDisplayLabel();
                            if (label.find(L'%') != std::wstring::npos) {
                                std::wstring resolved =
                                    ResolveLabelTokens(label, ctx);
                                entry.name = TruncateWithEllipsis(
                                    resolved, kMaxMenuItemLabelLength);
                                entry.labelText = entry.name;
                            }
                            if (!entry.subItems.empty())
                                resolveDeep(entry.subItems);
                        }
                    };
                resolveDeep(resolvedList);
            }

            AppendMenuEntries(resolvedList, menu.Items(), weakButton);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    };

    TrackRevoker(
        menu,
        menu.Opening(winrt::auto_revoke,
                     [ensureMenuEntries](wf::IInspectable const& sender,
                                         wf::IInspectable const&) {
                         ensureMenuEntries(sender.try_as<muxc::MenuFlyout>());
                     }));

    TrackRevoker(
        button,
        button.PointerEntered(
            winrt::auto_revoke, [ensureMenuEntries, weakButton](
                                    wf::IInspectable const&,
                                    mux::Input::PointerRoutedEventArgs const&) {
                if (auto btn = weakButton.get())
                    ensureMenuEntries(btn.Flyout().try_as<muxc::MenuFlyout>());
            }));

    button.Flyout(menu);
    if (openOnHover)
        SetUpOpenOnHover(button, hoverDelayMs, MakeShowFlyoutAction(button));
    return button;
}

void OpenContextMenuForElement(mux::FrameworkElement const& element) {
    if (g_unloading || !element)
        return;
    HWND hExplorerWnd = GetExplorerWindowForElement(element);
    if (!hExplorerWnd) {
        Wh_Log(L"No File Explorer window for the context menu item");
        return;
    }
    POINT point{};
    GetCursorPos(&point);
    ShowShellContextMenu(hExplorerWnd, point);
}

muxc::AppBarButton CreateContextMenuButton(bool openOnHover, int hoverDelayMs) {
    ContextMenuItemSettings settings;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        settings = g_settings.contextMenuItem;
    }

    muxc::AppBarButton button;
    button.Name(kContextMenuButtonName);
    button.Label(settings.buttonLabel.c_str());
    button.LabelPosition(settings.showLabel
                             ? muxc::CommandBarLabelPosition::Default
                             : muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(
        CreateIconElement(settings.buttonIcon, std::wstring(), L"\uE8FD"));

    if (!settings.showLabel && !settings.buttonLabel.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{settings.buttonLabel}));
    }

    TrackRevoker(
        button,
        button.Click(winrt::auto_revoke, [](wf::IInspectable const& sender,
                                            mux::RoutedEventArgs const&) {
            OpenContextMenuForElement(sender.try_as<mux::FrameworkElement>());
        }));

    if (openOnHover) {
        auto open = [weakButton = winrt::make_weak(button)]() {
            if (auto button = weakButton.get())
                OpenContextMenuForElement(button);
        };
        SetUpOpenOnHover(button, hoverDelayMs, open);
    }

    return button;
}

void PopulateNewPlusMenu(
    muxc::MenuFlyout const& menu,
    winrt::weak_ref<muxc::AppBarButton> const& weakButton) try {
    auto items = menu.Items();
    items.Clear();

    EffectiveConfig config = GetEffectiveConfig();
    std::vector<TemplateEntry> templates = EnumerateTemplates(config);

    if (templates.empty()) {
        muxc::MenuFlyoutItem placeholder;
        placeholder.Text(DirectoryExists(config.templateFolder)
                             ? L"No templates"
                             : L"Templates folder not found");
        placeholder.IsEnabled(false);
        items.Append(placeholder);
    }

    bool replaceVariables = config.replaceVariables;
    for (auto const& entry : templates) {
        muxc::MenuFlyoutItem menuItem;
        menuItem.Text(entry.displayName.c_str());
        if (config.showIcons) {
            if (auto source = LoadShellItemIcon(entry.path)) {
                muxc::ImageIcon imageIcon;
                imageIcon.Source(source);
                menuItem.Icon(imageIcon);
            }
        }
        if (entry.displayName != entry.fileName) {
            muxc::ToolTipService::SetToolTip(
                menuItem, winrt::box_value(winrt::hstring{entry.fileName}));
        }

        TrackRevoker(
            menuItem,
            menuItem.Click(
                winrt::auto_revoke,
                [entry, replaceVariables, weakButton](
                    wf::IInspectable const&, mux::RoutedEventArgs const&) {
                    if (g_unloading)
                        return;
                    auto button = weakButton.get();
                    if (!button)
                        return;
                    HWND hWnd = GetExplorerWindowForElement(button);
                    RunShellWorkOnWorkerThread(
                        [hWnd, entry, replaceVariables]() {
                            CreateFromTemplateForWindow(hWnd, entry,
                                                        replaceVariables);
                        });
                }));

        items.Append(menuItem);
    }

    if (config.showTemplatesFolderItem) {
        if (!templates.empty())
            items.Append(muxc::MenuFlyoutSeparator());
        muxc::MenuFlyoutItem openFolderItem;
        openFolderItem.Text(L"Open templates folder");
        openFolderItem.Icon(CreateGlyphIcon(L""));

        TrackRevoker(openFolderItem,
                     openFolderItem.Click(
                         winrt::auto_revoke, [folder = config.templateFolder](
                                                 wf::IInspectable const&,
                                                 mux::RoutedEventArgs const&) {
                             if (g_unloading)
                                 return;
                             RunShellWorkOnWorkerThread([folder]() {
                                 if (!DirectoryExists(folder))
                                     SHCreateDirectoryExW(
                                         nullptr, folder.c_str(), nullptr);
                                 ShellExecuteW(nullptr, L"open", folder.c_str(),
                                               nullptr, nullptr, SW_SHOWNORMAL);
                             });
                         }));

        items.Append(openFolderItem);
    }
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

muxc::AppBarButton CreateNewPlusButton(std::wstring const& originalIconUri,
                                       bool openOnHover,
                                       int hoverDelayMs) {
    NewPlusSettings settings;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        settings = g_settings.newPlus;
    }

    bool showLabel = settings.showLabel && !settings.buttonLabel.empty();
    muxc::AppBarButton button;
    button.Name(kNewPlusButtonName);
    button.Label(settings.buttonLabel.c_str());
    button.LabelPosition(showLabel ? muxc::CommandBarLabelPosition::Default
                                   : muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(MakeNewPlusButtonIcon(settings.buttonIcon, originalIconUri));

    if (showLabel) {
        TrackRevoker(
            button,
            button.Loaded(winrt::auto_revoke, [](wf::IInspectable const& sender,
                                                 mux::RoutedEventArgs const&) {
                if (auto button = sender.try_as<muxc::AppBarButton>())
                    AddNewPlusChevron(button);
            }));
    }

    if (!showLabel && !settings.buttonLabel.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{settings.buttonLabel}));
    }

    auto weakButton = winrt::make_weak(button);
    muxc::MenuFlyout menu;
    menu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    TrackRevoker(
        menu,
        menu.Opening(winrt::auto_revoke,
                     [weakButton](wf::IInspectable const& sender,
                                  wf::IInspectable const&) {
                         if (g_unloading)
                             return;
                         if (auto menu = sender.try_as<muxc::MenuFlyout>())
                             PopulateNewPlusMenu(menu, weakButton);
                     }));

    button.Flyout(menu);
    if (openOnHover)
        SetUpOpenOnHover(button, hoverDelayMs, MakeShowFlyoutAction(button));
    return button;
}

struct SelectionSummary {
    size_t count = 0;
    size_t fileCount = 0;
    size_t folderCount = 0;
    std::vector<std::wstring> extensions;
};

SelectionSummary SummarizeSelection(IShellView* psv) {
    SelectionSummary summary;
    if (!psv)
        return summary;

    winrt::com_ptr<IShellItemArray> items;
    if (FAILED(
            psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(items.put()))) ||
        !items)
        return summary;

    DWORD count = 0;
    if (FAILED(items->GetCount(&count)) || count == 0)
        return summary;

    summary.count = count;
    summary.extensions.reserve(count);

    for (DWORD i = 0; i < count; i++) {
        winrt::com_ptr<IShellItem> item;
        if (FAILED(items->GetItemAt(i, item.put())) || !item)
            continue;

        SFGAOF attrs = 0;
        if (SUCCEEDED(
                item->GetAttributes(SFGAO_FOLDER | SFGAO_STREAM, &attrs))) {
            bool isFolder = (attrs & SFGAO_FOLDER) && !(attrs & SFGAO_STREAM);
            if (isFolder) {
                summary.folderCount++;
                summary.extensions.push_back(L"folder");
            } else {
                summary.fileCount++;
                PWSTR displayName = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,
                                                   &displayName)) &&
                    displayName) {
                    PCWSTR ext = PathFindExtensionW(displayName);
                    if (ext && *ext == L'.') {
                        summary.extensions.push_back(ToLower(ext + 1));
                    } else {
                        summary.extensions.push_back(L"file");
                    }
                    CoTaskMemFree(displayName);
                }
            }
        }
    }
    return summary;
}

bool ItemMatchesExtensionFilter(const std::wstring& filterPattern,
                                const SelectionSummary& summary) {
    std::wstring filter = TrimWhitespaceAndQuotes(filterPattern);
    if (filter.empty() || filter == L"*")
        return true;

    std::wstring lowerFilter = ToLower(filter);
    size_t hPos = 0;
    while ((hPos = lowerFilter.find_first_of(L"hH", hPos)) !=
           std::wstring::npos) {
        std::wstring_view sv(lowerFilter.c_str() + hPos);
        size_t matchLen = 0;
        if (sv.starts_with(L"hidden"))
            matchLen = 6;
        else if (sv.starts_with(L"hide"))
            matchLen = 4;

        if (matchLen > 0) {
            bool leftBoundary =
                (hPos == 0) ||
                (wcschr(L",;+|/ \t()", lowerFilter[hPos - 1]) != nullptr);
            bool rightBoundary =
                (hPos + matchLen == lowerFilter.size()) ||
                (wcschr(L",;+|/ \t()", lowerFilter[hPos + matchLen]) !=
                 nullptr);
            if (leftBoundary && rightBoundary)
                return false;
        }
        hPos++;
    }

    std::vector<std::wstring> orBranches;
    std::vector<std::wstring> negativeTokens;

    size_t start = 0;
    int parenDepth = 0;
    for (size_t i = 0; i <= filter.size(); i++) {
        if (i < filter.size()) {
            if (filter[i] == L'(')
                parenDepth++;
            else if (filter[i] == L')' && parenDepth > 0)
                parenDepth--;
        }

        if (i == filter.size() ||
            (parenDepth == 0 && (filter[i] == L',' || filter[i] == L';'))) {
            std::wstring rawToken =
                TrimWhitespaceAndQuotes(filter.substr(start, i - start));
            start = i + 1;
            if (rawToken.empty())
                continue;

            std::wstring lower = ToLower(rawToken);
            if (lower == L"hide" || lower == L"hidden")
                return false;

            if (rawToken.front() == L'-' || rawToken.front() == L'~') {
                rawToken.erase(rawToken.begin());
                rawToken = TrimWhitespaceAndQuotes(rawToken);
                if (!rawToken.empty() && rawToken.front() == L'.')
                    rawToken.erase(rawToken.begin());
                rawToken = ToLower(rawToken);
                if (rawToken == L"dir" || rawToken == L"directory" ||
                    rawToken == L"folders")
                    rawToken = L"folder";
                if (rawToken == L"files")
                    rawToken = L"file";
                negativeTokens.push_back(rawToken);
            } else {
                orBranches.push_back(rawToken);
            }
        }
    }

    if (summary.count > 0) {
        for (const auto& ext : summary.extensions) {
            bool isDir = (ext == L"folder");
            for (const auto& neg : negativeTokens) {
                if (isDir && neg == L"folder")
                    return false;
                if (!isDir && (neg == L"file" || ext == neg))
                    return false;
            }
        }
    }

    if (orBranches.empty())
        return summary.count > 0;

    auto evalAtomicToken = [&](std::wstring token) -> bool {
        token = ToLower(TrimWhitespaceAndQuotes(token));
        if (token == L"hide" || token == L"hidden")
            return false;

        if (token.starts_with(L"only ") || token.starts_with(L"only:") ||
            token.starts_with(L"only=") || token.starts_with(L"only(")) {
            if (summary.count == 0 || summary.folderCount > 0)
                return false;

            std::wstring content;
            if (token.starts_with(L"only(")) {
                size_t closeParen = token.find(L')', 5);
                if (closeParen == std::wstring::npos)
                    closeParen = token.size();
                content = token.substr(5, closeParen - 5);
            } else {
                content = token.substr(4);
                if (!content.empty() &&
                    (content.front() == L':' || content.front() == L'='))
                    content = content.substr(1);
            }

            std::vector<std::wstring> allowedExts;
            size_t cStart = 0;
            while (cStart < content.size()) {
                size_t cEnd = content.find_first_of(L",;|/ \t", cStart);
                if (cEnd == std::wstring::npos)
                    cEnd = content.size();
                std::wstring ext = TrimWhitespaceAndQuotes(
                    content.substr(cStart, cEnd - cStart));
                cStart = cEnd + 1;
                if (!ext.empty() && ext.front() == L'.')
                    ext.erase(ext.begin());
                if (!ext.empty())
                    allowedExts.push_back(ext);
            }
            if (allowedExts.empty())
                return false;

            for (const auto& ext : summary.extensions) {
                bool matchedAny = false;
                for (const auto& allowed : allowedExts) {
                    if (ext == allowed) {
                        matchedAny = true;
                        break;
                    }
                }
                if (!matchedAny)
                    return false;
            }
            return true;
        }

        if (!token.empty() && token.front() == L'.')
            token.erase(token.begin());

        if (token == L"none" || token == L"empty" || token == L"nothing" ||
            token == L"unselected")
            return summary.count == 0;
        if (summary.count == 0)
            return false;
        if (token == L"*" || token == L"any" || token == L"selected")
            return true;

        if (token == L"files" || token == L"file")
            return summary.fileCount > 0 && summary.folderCount == 0;
        if (token == L"folders" || token == L"folder" || token == L"dir" ||
            token == L"directory")
            return summary.folderCount > 0 && summary.fileCount == 0;
        if (token == L"mixed" || token == L"mixed_selection")
            return summary.fileCount > 0 && summary.folderCount > 0;
        if (token == L"single")
            return summary.count == 1;
        if (token == L"multi" || token == L"multiple")
            return summary.count >= 2;
        if (token == L"single_file" || token == L"single_files")
            return summary.fileCount == 1 && summary.folderCount == 0;
        if (token == L"multi_file" || token == L"multi_files")
            return summary.fileCount >= 2 && summary.folderCount == 0;
        if (token == L"single_folder" || token == L"single_folders")
            return summary.folderCount == 1 && summary.fileCount == 0;
        if (token == L"multi_folder" || token == L"multi_folders")
            return summary.folderCount >= 2 && summary.fileCount == 0;

        for (const auto& ext : summary.extensions) {
            if (ext != L"folder" && ext == token)
                return true;
        }
        return false;
    };

    for (const auto& branch : orBranches) {
        bool branchPassed = true;
        size_t bStart = 0;
        while (bStart < branch.size()) {
            size_t bEnd = branch.find(L'+', bStart);
            if (bEnd == std::wstring::npos)
                bEnd = branch.size();
            std::wstring subToken = branch.substr(bStart, bEnd - bStart);
            bStart = bEnd + 1;

            subToken = TrimWhitespaceAndQuotes(subToken);
            if (subToken.empty())
                continue;

            std::wstring lowerSub = ToLower(subToken);
            if (lowerSub == L"hide" || lowerSub == L"hidden")
                return false;

            if (!evalAtomicToken(subToken)) {
                branchPassed = false;
                break;
            }
        }
        if (branchPassed)
            return true;
    }
    return false;
}

void UpdateDynamicButtonStates(muxc::CommandBar const& commandBar,
                               HWND hExplorerWnd,
                               const ExplorerContext* optContext = nullptr) {
    if (g_unloading || !commandBar)
        return;
    if (!hExplorerWnd)
        hExplorerWnd = GetExplorerWindowForElement(commandBar);
    if (!hExplorerWnd)
        return;

    ExplorerContext localContext;
    const ExplorerContext& context =
        optContext ? *optContext
                   : (localContext = GetExplorerContext(hExplorerWnd));

    SelectionSummary summary = SummarizeSelection(context.shellView.get());

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    std::vector<ActionItem> itemSnapshots;
    bool disabledInsteadOfHidden;
    bool showMenuChevrons;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        itemSnapshots = g_settings.items;
        disabledInsteadOfHidden = g_settings.disabledInsteadOfHidden;
        showMenuChevrons = g_settings.showMenuChevrons;
    }

    for (uint32_t i = 0; i < count; i++) {
        auto cmd = commands.GetAt(i);
        auto element = cmd.try_as<mux::FrameworkElement>();
        if (!element)
            continue;

        std::wstring name{element.Name()};
        if (!name.starts_with(kButtonNamePrefix))
            continue;

        auto button = cmd.try_as<muxc::AppBarButton>();
        if (!button)
            continue;

        size_t underscore = name.rfind(L'_');
        if (underscore == std::wstring::npos)
            continue;

        int itemIndex = _wtoi(name.c_str() + underscore + 1);
        if (itemIndex < 0 || (size_t)itemIndex >= itemSnapshots.size())
            continue;

        const auto& item = itemSnapshots[(size_t)itemIndex];
        std::wstring rawLabel = item.GetDisplayLabel();
        bool isVisible =
            ItemMatchesExtensionFilter(item.matchExtensions, summary);

        // UI Text Updating
        if (rawLabel.find(L'%') != std::wstring::npos) {
            std::wstring fullResolved = ResolveLabelTokens(rawLabel, context);
            std::wstring displayResolved =
                TruncateWithEllipsis(fullResolved, kMaxToolbarLabelLength);

            bool isMenuButton = item.isMenu && !item.subItems.empty();
            if (isMenuButton) {
                displayResolved = MenuButtonLabel(
                    displayResolved,
                    item.showLabel && !displayResolved.empty(),
                    showMenuChevrons);
            }
            button.Label(displayResolved.c_str());
            // Setting the label replaces the text drawn with the chevron.
            if (isMenuButton && showMenuChevrons)
                AddMenuButtonChevron(button);
            muxc::ToolTipService::SetToolTip(
                button, winrt::box_value(winrt::hstring{fullResolved}));
        }

        // Visibility & Separator Logic
        if (disabledInsteadOfHidden) {
            button.Visibility(mux::Visibility::Visible);
            button.IsEnabled(isVisible);
        } else {
            auto targetVis = isVisible ? mux::Visibility::Visible
                                       : mux::Visibility::Collapsed;
            button.Visibility(targetVis);
            button.IsEnabled(isVisible);

            // Dan's schema dynamically names separators `WH_Btn_Sep_0` etc.
            std::wstring sepName = std::wstring(kButtonNamePrefix) + L"_Sep_" +
                                   std::to_wstring(itemIndex);
            for (uint32_t j = 0; j < count; j++) {
                if (auto sepEl =
                        commands.GetAt(j).try_as<mux::FrameworkElement>()) {
                    if (sepEl.Name() == sepName)
                        sepEl.Visibility(targetVis);
                }
            }
        }
    }
}

void StartSelectionTimerIfNeeded(CommandBarEntry& entry,
                                 muxc::CommandBar const& commandBar) {
    if (entry.selectionTimer || g_unloading)
        return;

    bool needsTimer = false;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        for (const auto& itm : g_settings.items) {
            if (itm.GetDisplayLabel().find(L'%') != std::wstring::npos ||
                !TrimWhitespaceAndQuotes(itm.matchExtensions).empty()) {
                needsTimer = true;
                break;
            }
        }
    }

    if (!needsTimer)
        return;

    mux::DispatcherTimer selectionTimer;
    selectionTimer.Interval(std::chrono::milliseconds(250));

    auto weakBar = winrt::make_weak(commandBar);
    auto timerToken = selectionTimer.Tick(
        [weakBar](wf::IInspectable const&, wf::IInspectable const&) {
            if (g_unloading)
                return;
            auto cb = weakBar.get();
            if (!cb)
                return;

            HWND hWnd = GetExplorerWindowForElement(cb);
            if (!hWnd || GetForegroundWindow() != hWnd)
                return;

            CommandBarEntry* currentEntry = nullptr;
            for (auto& ent : g_entries) {
                if (ent.commandBar.get() == cb) {
                    currentEntry = &ent;
                    break;
                }
            }
            if (!currentEntry)
                return;

            HWND hActiveTab =
                FindWindowExW(hWnd, nullptr, L"ShellTabWindowClass", nullptr);
            if (hActiveTab != currentEntry->cachedTabWnd ||
                !currentEntry->cachedBrowser) {
                currentEntry->cachedTabWnd = hActiveTab;
                currentEntry->cachedBrowser = GetActiveShellBrowser(hWnd);
                currentEntry->cachedShellView = nullptr;
            }

            if (!currentEntry->cachedBrowser)
                return;

            winrt::com_ptr<IShellView> view;
            if (FAILED(currentEntry->cachedBrowser->QueryActiveShellView(
                    view.put())) ||
                !view) {
                currentEntry->cachedBrowser = nullptr;
                currentEntry->cachedShellView = nullptr;
                return;
            }

            auto fv = view.try_as<IFolderView>();
            int count = 0, focused = -1;
            if (!fv || FAILED(fv->ItemCount(SVGIO_SELECTION, &count)))
                return;
            fv->GetFocusedItem(&focused);

            if (view == currentEntry->cachedShellView &&
                count == currentEntry->lastSelectionCount &&
                focused == currentEntry->lastFocusedItem) {
                return;
            }

            currentEntry->cachedShellView = view;
            currentEntry->lastSelectionCount = count;
            currentEntry->lastFocusedItem = focused;

            ExplorerContext ctx = GetContextFromView(view.get(), count);
            UpdateDynamicButtonStates(cb, hWnd, &ctx);
        });

    selectionTimer.Start();
    entry.selectionTimer = selectionTimer;
    entry.selectionTickToken = timerToken;
}

void EnsureActionButtons(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;
    if (HasElement(commandBar, [](muxc::ICommandBarElement const& command) {
            auto element = command.try_as<mux::FrameworkElement>();
            return element && std::wstring_view(element.Name())
                                  .starts_with(kButtonNamePrefix);
        }))
        return;

    bool openMenuOnHover;
    int menuHoverDelay;
    bool showMenuChevrons;
    std::vector<ActionItem> items;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
        showMenuChevrons = g_settings.showMenuChevrons;
        items = g_settings.items;
    }

    if (items.empty())
        return;

    Wh_Log(L"Adding %zu items to command bar", items.size());
    auto commands = commandBar.PrimaryCommands();

    if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
        uint32_t insertIndex = 0;
        for (size_t i = 0; i < items.size(); i++) {
            auto const& item = items[i];
            if (item.isMenu && !item.subItems.empty()) {
                commands.InsertAt(
                    insertIndex++,
                    CreateMenuButton(item, (int)i, openMenuOnHover,
                                     menuHoverDelay, showMenuChevrons));
            } else {
                commands.InsertAt(insertIndex++,
                                  CreateActionButton(item, (int)i));
            }

            if (item.separatorAfter) {
                muxc::AppBarSeparator separator;
                std::wstring name = kButtonNamePrefix;
                name += L"_Sep_";
                name += std::to_wstring(i);
                separator.Name(name.c_str());
                commands.InsertAt(insertIndex++, separator);
            }
        }
        return;
    }

    for (size_t i = 0; i < items.size(); i++) {
        auto const& item = items[i];
        if (item.isMenu && !item.subItems.empty()) {
            commands.Append(CreateMenuButton(item, (int)i, openMenuOnHover,
                                             menuHoverDelay, showMenuChevrons));
        } else {
            commands.Append(CreateActionButton(item, (int)i));
        }

        if (item.separatorAfter) {
            muxc::AppBarSeparator separator;
            std::wstring name = kButtonNamePrefix;
            name += L"_Sep_";
            name += std::to_wstring(i);
            separator.Name(name.c_str());
            commands.Append(separator);
        }
    }
}

void EnsureContextMenuButton(muxc::CommandBar const& commandBar) {
    bool enabled, openMenuOnHover;
    int menuHoverDelay;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        enabled = g_settings.contextMenuItem.enabled;
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
    }

    if (!enabled ||
        HasElement(commandBar, [](muxc::ICommandBarElement const& command) {
            auto element = command.try_as<mux::FrameworkElement>();
            return element && element.Name() == kContextMenuButtonName;
        }))
        return;

    Wh_Log(L"Adding the context menu item");
    commandBar.PrimaryCommands().Append(
        CreateContextMenuButton(openMenuOnHover, menuHoverDelay));
}

void OpenSettingsWindow(HWND hHintWnd);

muxc::AppBarButton CreateSettingsButton() {
    muxc::AppBarButton button;
    button.Name(kSettingsButtonName);
    button.Label(L"Command bar settings");
    button.LabelPosition(muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(CreateGlyphIcon(L""));
    muxc::ToolTipService::SetToolTip(
        button,
        winrt::box_value(winrt::hstring{L"Explorer Command Bar settings"}));

    TrackRevoker(
        button,
        button.Click(winrt::auto_revoke, [](wf::IInspectable const& sender,
                                            mux::RoutedEventArgs const&) {
            if (g_unloading)
                return;
            HWND hWnd = nullptr;
            if (auto element = sender.try_as<mux::FrameworkElement>())
                hWnd = GetExplorerWindowForElement(element);
            OpenSettingsWindow(hWnd);
        }));

    return button;
}

void EnsureSettingsButton(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;
    bool enabled;
    int order;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        enabled = g_settings.showSettingsItem;
        order = g_settings.settingsItemOrder;
    }

    if (!enabled ||
        HasElement(commandBar, [](muxc::ICommandBarElement const& command) {
            auto element = command.try_as<mux::FrameworkElement>();
            return element && element.Name() == kSettingsButtonName;
        }))
        return;

    // The position is counted among the custom toolbar items, separators
    // excluded. Past the last item, the button goes after the last custom
    // element (or its separator).
    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();
    bool isSecondary = commandBar.Name() == L"FileExplorerSecondaryCommandBar";
    uint32_t afterLast = isSecondary ? 0 : count;
    bool hasCustomElement = false;
    std::vector<uint32_t> itemIndexes;
    for (uint32_t i = 0; i < count; i++) {
        auto element = commands.GetAt(i).try_as<mux::FrameworkElement>();
        if (!element)
            continue;
        winrt::hstring name = element.Name();
        std::wstring_view view{name};
        if (!isSecondary && !hasCustomElement && view == kContextMenuButtonName)
            afterLast = i;
        if (!view.starts_with(kButtonNamePrefix))
            continue;
        hasCustomElement = true;
        afterLast = i + 1;
        if (view.find(L"_Sep_") == std::wstring_view::npos)
            itemIndexes.push_back(i);
    }

    uint32_t insertIndex = order >= 0 && (size_t)order < itemIndexes.size()
                               ? itemIndexes[order]
                               : afterLast;

    Wh_Log(L"Adding the settings item at %u", insertIndex);
    commands.InsertAt(insertIndex, CreateSettingsButton());
}

void EnsureNewPlusButton(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;
    bool openMenuOnHover, enabled;
    int menuHoverDelay;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        enabled = g_settings.newPlus.enabled;
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
    }

    if (!enabled || HasElement(commandBar, IsOurNewPlusButton))
        return;

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();
    uint32_t newButtonIndex = count;
    std::wstring originalIconUri;

    for (uint32_t i = 0; i < count; i++) {
        auto command = commands.GetAt(i);
        if (IsOurElement(command))
            continue;
        if (auto button = command.try_as<muxc::AppBarButton>();
            button && IdentifyDefaultButton(button) == kNewButtonIndex) {
            newButtonIndex = i;
            originalIconUri = GetButtonIconUri(button);
            break;
        }
    }

    if (newButtonIndex == count) {
        Wh_Log(L"The New button isn't in the command bar (yet)");
        return;
    }

    Wh_Log(L"Adding the New+ button");
    commands.InsertAt(
        newButtonIndex,
        CreateNewPlusButton(originalIconUri, openMenuOnHover, menuHoverDelay));
}

bool IsSameObject(wf::IInspectable const& a, wf::IInspectable const& b) {
    if (!a || !b)
        return !a && !b;
    return winrt::get_abi(a.as<wf::IUnknown>()) ==
           winrt::get_abi(b.as<wf::IUnknown>());
}

// Puts the built-in buttons New to View in the order of the
// defaultButtonOrder setting, or back in their original order if restore is
// set or the setting is empty. Only the elements from the first to the last
// of these buttons are moved: the buttons, File Explorer's separators between
// them, and the New+ button, which stays in front of New. The order is
// applied by moving the existing elements, which keep working as before.
void ApplyDefaultButtonOrder(muxc::CommandBar const& commandBar, bool restore) {
    if (commandBar.Name() != L"FileExplorerCommandBar")
        return;

    std::vector<int> order;
    if (!restore) {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        order = g_settings.defaultButtonOrder;
    }

    std::vector<winrt::weak_ref<mux::UIElement>> originalOrder;
    if (auto* barEntry = FindManagedElement(commandBar))
        originalOrder = barEntry->originalButtonOrder;
    if (order.empty() && originalOrder.empty())
        return;

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    std::vector<muxc::ICommandBarElement> buttons(kReorderableButtonCount,
                                                  nullptr);
    muxc::ICommandBarElement newPlus{nullptr};
    uint32_t first = count;
    uint32_t last = 0;
    for (uint32_t i = 0; i < count; i++) {
        auto command = commands.GetAt(i);
        if (IsOurElement(command)) {
            if (!IsOurNewPlusButton(command))
                continue;
            newPlus = command;
        } else if (auto button = command.try_as<muxc::AppBarButton>()) {
            if (GetAutomationId(button) == L"DetailsPaneToggleButton")
                continue;
            int index = IdentifyDefaultButton(button);
            if (index < 0 || index >= kReorderableButtonCount ||
                buttons[index])
                continue;
            buttons[index] = command;
        } else {
            continue;
        }
        first = std::min(first, i);
        last = std::max(last, i);
    }
    if (first >= count)
        return;

    auto isButton = [&buttons](muxc::ICommandBarElement const& command) {
        return std::any_of(buttons.begin(), buttons.end(),
                           [&command](muxc::ICommandBarElement const& button) {
                               return button && IsSameObject(button, command);
                           });
    };
    auto contains = [](std::vector<muxc::ICommandBarElement> const& list,
                       muxc::ICommandBarElement const& command) {
        return std::any_of(list.begin(), list.end(),
                           [&command](muxc::ICommandBarElement const& entry) {
                               return IsSameObject(entry, command);
                           });
    };

    // Anything else between the buttons means an unknown layout, which is
    // left alone.
    std::vector<muxc::ICommandBarElement> region;
    for (uint32_t i = first; i <= last; i++) {
        auto command = commands.GetAt(i);
        if (!IsSameObject(command, newPlus) && !isButton(command) &&
            (IsOurElement(command) ||
             !command.try_as<muxc::AppBarSeparator>())) {
            Wh_Log(L"Unexpected command %u among the built-in buttons, not "
                   L"reordering them",
                   i);
            return;
        }
        region.push_back(command);
    }

    // The original order, without the New+ button. Recorded on the first
    // reorder, and again if File Explorer recreated the buttons since.
    std::vector<muxc::ICommandBarElement> original;
    for (auto const& weakElement : originalOrder) {
        auto element = weakElement.get();
        if (!element)
            continue;
        for (auto const& command : region) {
            if (IsSameObject(command, element)) {
                original.push_back(command);
                break;
            }
        }
    }
    bool recordOriginal = original.empty();
    if (recordOriginal) {
        for (auto const& command : region) {
            if (!IsSameObject(command, newPlus))
                original.push_back(command);
        }
    }

    // The separators, in their original order.
    std::vector<muxc::ICommandBarElement> separators;
    for (auto const& command : original) {
        if (!isButton(command))
            separators.push_back(command);
    }
    for (auto const& command : region) {
        if (!IsSameObject(command, newPlus) && !isButton(command) &&
            !contains(separators, command))
            separators.push_back(command);
    }

    bool restoring = order.empty();
    std::vector<muxc::ICommandBarElement> desired;
    auto append = [&](muxc::ICommandBarElement const& command) {
        if (newPlus && buttons[kNewButtonIndex] &&
            IsSameObject(command, buttons[kNewButtonIndex]))
            desired.push_back(newPlus);
        desired.push_back(command);
    };

    size_t usedSeparators = 0;
    if (restoring) {
        for (auto const& command : original)
            append(command);
        usedSeparators = separators.size();
    } else {
        for (int token : order) {
            if (token == kOrderSeparator) {
                if (usedSeparators < separators.size())
                    append(separators[usedSeparators++]);
            } else if (buttons[token]) {
                append(buttons[token]);
            }
        }
    }

    // Whatever wasn't placed, e.g. separators left over by an order with
    // fewer separators, goes at the end.
    for (auto const& command : region) {
        if (!contains(desired, command))
            desired.push_back(command);
    }

    if (recordOriginal && !restoring) {
        auto& barEntry = GetManagedElement(commandBar);
        barEntry.originalButtonOrder.clear();
        for (auto const& command : original) {
            barEntry.originalButtonOrder.push_back(
                winrt::make_weak(command.as<mux::UIElement>()));
        }
    }

    for (size_t i = 0; i < separators.size(); i++) {
        auto element = separators[i].as<mux::UIElement>();
        if (i >= usedSeparators)
            GetManagedElement(element).unusedSeparator = true;
        else if (auto* managed = FindManagedElement(element))
            managed->unusedSeparator = false;
    }

    // Move the elements into place, one at a time. Each element keeps its
    // identity, so File Explorer's handlers and bindings stay intact.
    int moves = 0;
    try {
        for (uint32_t i = 0; i < desired.size(); i++) {
            uint32_t position = first + i;
            if (IsSameObject(commands.GetAt(position), desired[i]))
                continue;

            uint32_t from = 0;
            for (uint32_t j = position + 1; j <= last; j++) {
                if (IsSameObject(commands.GetAt(j), desired[i])) {
                    from = j;
                    break;
                }
            }
            if (!from) {
                Wh_Log(L"Couldn't find command %u to reorder", i);
                break;
            }

            commands.RemoveAt(from);
            commands.InsertAt(position, desired[i]);
            moves++;
        }
    } catch (...) {
        Wh_Log(L"Reordering failed: %08X", winrt::to_hresult().value);
    }

    if (moves > 0) {
        Wh_Log(L"%s the built-in buttons (%d moves)",
               restoring ? L"Restored the order of" : L"Reordered", moves);
    }

    if (restoring) {
        if (auto* barEntry = FindManagedElement(commandBar))
            barEntry->originalButtonOrder.clear();
    }
}

void UpdateCommandBar(muxc::CommandBar const& commandBar) {
    if (g_unloading)
        return;

    bool placeOnSecondary;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        placeOnSecondary = g_settings.placeOnSecondaryBar;
    }

    if (commandBar.Name() == L"FileExplorerCommandBar") {
        EnsureNewPlusButton(commandBar);
        ApplyDefaultButtonOrder(commandBar, false);
        if (!placeOnSecondary) {
            EnsureActionButtons(commandBar);
            EnsureSettingsButton(commandBar);
        }
        EnsureContextMenuButton(commandBar);
    } else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
        if (placeOnSecondary) {
            commandBar.DefaultLabelPosition(
                muxc::CommandBarDefaultLabelPosition::Right);
            EnsureActionButtons(commandBar);
            EnsureSettingsButton(commandBar);
        }
    }

    ApplyDefaultButtonVisibility(commandBar);
    UpdateDynamicButtonStates(commandBar, nullptr);
}

void RemoveOurButtons(muxc::CommandBar const& commandBar) {
    auto commands = commandBar.PrimaryCommands();
    for (uint32_t i = commands.Size(); i > 0; i--) {
        auto command = commands.GetAt(i - 1);
        if (!IsOurElement(command))
            continue;

        if (auto button = command.try_as<muxc::AppBarButton>()) {
            if (auto flyout = button.Flyout()) {
                try {
                    flyout.Hide();
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        }
        commands.RemoveAt(i - 1);
    }
}

void InstallShortcutHookForCurrentThread();

void OnCommandBarAdded(muxc::CommandBar const& commandBar) {
    for (auto it = g_entries.begin(); it != g_entries.end();) {
        if (!it->commandBar.get())
            it = g_entries.erase(it);
        else
            ++it;
    }

    for (auto const& entry : g_entries) {
        if (entry.commandBar.get() == commandBar)
            return;
    }

    CommandBarEntry entry;
    entry.commandBar = winrt::make_weak(commandBar);

    // These handlers live as long as the command bar is tracked. They aren't
    // registered with TrackRevoker, since RefreshButtonsForCurrentThread
    // revokes those on every settings change and the command bar is never
    // added again. RemoveButtonsForCurrentThread revokes them on unload.

    // Re-add the buttons whenever the command bar reloads.
    entry.loadedToken = commandBar.Loaded(
        [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
            if (g_unloading)
                return;
            if (auto commandBar = sender.try_as<muxc::CommandBar>())
                UpdateCommandBar(commandBar);
        });

    // Re-add the buttons and reapply visibility if Explorer ever rebuilds
    // the command list.
    entry.vectorChangedToken = commandBar.PrimaryCommands().VectorChanged(
        [weakCommandBar = winrt::make_weak(commandBar)](
            wfc::IObservableVector<muxc::ICommandBarElement> const&,
            wfc::IVectorChangedEventArgs const&) {
            if (g_unloading)
                return;

            // Defer the update; mutating the vector from within its own change
            // notification isn't allowed. Coalesced, since adding our own
            // buttons raises one notification per button.
            QueueCommandBarUpdate(weakCommandBar, true);
        });

    // Recreate the buttons with the icons of the new theme.
    entry.themeChangedToken = commandBar.ActualThemeChanged(
        [](mux::FrameworkElement const& sender, wf::IInspectable const&) {
            if (g_unloading)
                return;

            {
                std::lock_guard<std::mutex> lock(g_iconCacheMutex);
                g_iconCache.clear();
            }

            if (auto cb = sender.try_as<muxc::CommandBar>()) {
                try {
                    RemoveOurButtons(cb);
                    UpdateCommandBar(cb);
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        });

    g_entries.push_back(std::move(entry));
    StartSelectionTimerIfNeeded(entry, commandBar);
    UpdateCommandBar(commandBar);
    InstallShortcutHookForCurrentThread();
}

void RestoreNudgedIslandsForCurrentThread();
void RemoveShortcutHookForCurrentThread();

void RemoveButtonsForCurrentThread() {
    RemoveShortcutHookForCurrentThread();
    RestoreNudgedIslandsForCurrentThread();
    UnwatchVisibilityForCurrentThread();
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<CommandBarEntry> taken;
    taken.swap(g_entries);

    for (auto& entry : taken) {
        StopSelectionTimer(entry);
        auto commandBar = entry.commandBar.get();
        if (!commandBar)
            continue;
        try {
            commandBar.Loaded(entry.loadedToken);
            commandBar.PrimaryCommands().VectorChanged(
                entry.vectorChangedToken);
            commandBar.ActualThemeChanged(entry.themeChangedToken);
            RemoveOurButtons(commandBar);
            ApplyDefaultButtonOrder(commandBar, true);
            ApplyDefaultButtonVisibility(commandBar, true);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }

    RestoreChevronsForCurrentThread();
    ForgetManagedElementsForCurrentThread();
    g_pendingUpdates.clear();
    g_threadScanned = false;
    DestroyContextMenuOwnerWindowForCurrentThread();
}

void RefreshButtonsForCurrentThread() {
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<winrt::weak_ref<muxc::CommandBar>> commandBars;
    for (auto const& entry : g_entries)
        commandBars.push_back(entry.commandBar);

    for (auto const& weakCommandBar : commandBars) {
        auto commandBar = weakCommandBar.get();
        if (!commandBar)
            continue;
        try {
            RemoveOurButtons(commandBar);
            UpdateCommandBar(commandBar);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

bool IsTargetCommandBarName(std::wstring_view name) {
    return name == L"FileExplorerCommandBar" ||
           name == L"FileExplorerSecondaryCommandBar";
}

bool FoundAllCommandBars(std::vector<muxc::CommandBar> const& commandBars) {
    bool primary = false, secondary = false;
    for (auto const& commandBar : commandBars) {
        if (commandBar.Name() == L"FileExplorerCommandBar")
            primary = true;
        else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar")
            secondary = true;
    }
    return primary && secondary;
}

void CollectCommandBars(mux::DependencyObject const& root,
                        int depth,
                        std::vector<muxc::CommandBar>* commandBars) {
    if (depth > 64)
        return;
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        if (FoundAllCommandBars(*commandBars))
            return;
        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto commandBar = child.try_as<muxc::CommandBar>();
            commandBar && IsTargetCommandBarName(commandBar.Name())) {
            commandBars->push_back(std::move(commandBar));
            continue;
        }
        CollectCommandBars(child, depth + 1, commandBars);
    }
}

void ScanXamlRoot(mux::XamlRoot const& xamlRoot) try {
    if (g_unloading || !xamlRoot)
        return;
    auto content = xamlRoot.Content();
    if (!content)
        return;

    std::vector<muxc::CommandBar> commandBars;
    CollectCommandBars(content, 0, &commandBars);
    for (auto const& commandBar : commandBars)
        OnCommandBarAdded(commandBar);
    if (!commandBars.empty())
        g_threadScanned = true;
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

void ScanXamlRootForCommandBars(mux::UIElement const& element) try {
    if (g_unloading || !element)
        return;
    ScanXamlRoot(element.XamlRoot());
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

bool IsXamlRootKnownForCurrentThread(mux::XamlRoot const& xamlRoot) {
    for (auto const& entry : g_entries) {
        auto commandBar = entry.commandBar.get();
        if (commandBar && commandBar.XamlRoot() == xamlRoot)
            return true;
    }
    return false;
}

bool IsWindowKnownForCurrentThread(HWND hWnd) {
    for (auto const& entry : g_entries) {
        auto commandBar = entry.commandBar.get();
        if (!commandBar)
            continue;
        try {
            auto xamlRoot = commandBar.XamlRoot();
            auto environment =
                xamlRoot ? xamlRoot.ContentIslandEnvironment() : nullptr;
            if (!environment)
                continue;
            HWND hRootWnd = (HWND)(uintptr_t)environment.AppWindowId().Value;
            if (hRootWnd && GetAncestor(hRootWnd, GA_ROOT) == hWnd)
                return true;
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
    return false;
}

// There's no public API to get from a window to its XAML tree without XAML
// Diagnostics, and an already open window doesn't rebuild its command bar, so
// the hooks above might not run for it until the user clicks the command bar.
// Resizing the window's XAML islands by a pixel raises XamlRoot.Changed, which
// File Explorer handles in CommandBarControl::OnDpiChanged with the XamlRoot as
// a typed parameter. The original size is restored shortly after.
struct IslandNudge {
    HWND hWnd;
    int width;
    int height;
};

thread_local std::vector<IslandNudge> g_islandNudges;
thread_local mux::DispatcherTimer g_islandNudgeTimer{nullptr};
thread_local winrt::event_token g_islandNudgeTickToken;

void RestoreNudgedIslandsForCurrentThread() {
    if (g_islandNudgeTimer) {
        try {
            g_islandNudgeTimer.Stop();
            g_islandNudgeTimer.Tick(g_islandNudgeTickToken);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
        g_islandNudgeTimer = nullptr;
    }

    std::vector<IslandNudge> taken;
    taken.swap(g_islandNudges);
    for (auto const& nudge : taken) {
        if (!IsWindow(nudge.hWnd))
            continue;
        SetWindowPos(
            nudge.hWnd, nullptr, 0, 0, nudge.width, nudge.height,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
}

void NudgeXamlIslandsOfWindow(HWND hWnd) {
    std::vector<HWND> islands;
    EnumChildWindows(
        hWnd,
        [](HWND hChildWnd, LPARAM lParam) -> BOOL {
            WCHAR className[64];
            if (GetWindowThreadProcessId(hChildWnd, nullptr) ==
                    GetCurrentThreadId() &&
                GetClassName(hChildWnd, className, ARRAYSIZE(className)) &&
                wcscmp(className,
                       L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0) {
                ((std::vector<HWND>*)lParam)->push_back(hChildWnd);
            }
            return TRUE;
        },
        (LPARAM)&islands);

    for (HWND hIslandWnd : islands) {
        RECT rect;
        if (!IsWindowVisible(hIslandWnd) || !GetWindowRect(hIslandWnd, &rect))
            continue;
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        if (width <= 0 || height <= 0)
            continue;
        Wh_Log(L"Nudging XAML island %08X of window %08X",
               (DWORD)(ULONG_PTR)hIslandWnd, (DWORD)(ULONG_PTR)hWnd);
        g_islandNudges.push_back({hIslandWnd, width, height});
        SetWindowPos(
            hIslandWnd, nullptr, 0, 0, width + 1, height,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    if (g_islandNudges.empty() || g_islandNudgeTimer)
        return;

    try {
        g_islandNudgeTimer = mux::DispatcherTimer();
        g_islandNudgeTimer.Interval(std::chrono::milliseconds(100));
        g_islandNudgeTickToken = g_islandNudgeTimer.Tick(
            [](wf::IInspectable const&, wf::IInspectable const&) {
                RestoreNudgedIslandsForCurrentThread();
            });
        g_islandNudgeTimer.Start();
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
        RestoreNudgedIslandsForCurrentThread();
    }
}

void ScheduleXamlRootScan(mux::UIElement const& element) try {
    if (g_unloading || !element)
        return;
    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanXamlRootForCommandBars(element);
        return;
    }
    dispatcherQueue.TryEnqueue([weakElement = winrt::make_weak(element)]() {
        if (auto element = weakElement.get())
            ScanXamlRootForCommandBars(element);
    });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

muxc::CommandBar GetKnownCommandBarForCurrentThread() {
    for (auto const& entry : g_entries) {
        if (auto commandBar = entry.commandBar.get())
            return commandBar;
    }
    return nullptr;
}

void ScanCurrentThreadForCommandBars() try {
    if (g_unloading)
        return;
    if (auto knownCommandBar = GetKnownCommandBarForCurrentThread()) {
        ScanXamlRootForCommandBars(knownCommandBar);
        return;
    }
    auto focused = mux::Input::FocusManager::GetFocusedElement();
    auto element = focused ? focused.try_as<mux::UIElement>() : nullptr;
    if (!element) {
        Wh_Log(L"No XAML element to start from on thread %u",
               GetCurrentThreadId());
        return;
    }
    ScanXamlRootForCommandBars(element);
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

void DiscoverCommandBarsForWindow(HWND hWnd) {
    ScanCurrentThreadForCommandBars();
    if (!g_unloading && !IsWindowKnownForCurrentThread(hWnd))
        NudgeXamlIslandsOfWindow(hWnd);
}

void ScheduleCurrentThreadScan() try {
    if (g_unloading)
        return;
    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanCurrentThreadForCommandBars();
        return;
    }
    dispatcherQueue.TryEnqueue([]() { ScanCurrentThreadForCommandBars(); });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

using CommandBarManager_CommandBar_t = void(WINAPI*)(void* pThis,
                                                     void* commandBar);
CommandBarManager_CommandBar_t CommandBarManager_CommandBar_Original;

void WINAPI CommandBarManager_CommandBar_Hook(void* pThis, void* commandBar) {
    Wh_Log(L">");
    CommandBarManager_CommandBar_Original(pThis, commandBar);
    if (g_unloading || !commandBar)
        return;
    try {
        EnsureContextMenuOwnerWindow();
        auto const& element =
            *reinterpret_cast<muxc::CommandBar const*>(commandBar);
        if (!element)
            return;
        Wh_Log(L"Command bar %s, thread %u", element.Name().c_str(),
               GetCurrentThreadId());
        OnCommandBarAdded(element);
        ScheduleXamlRootScan(element);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

using CommandBarControl_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
CommandBarControl_OnApplyTemplate_t CommandBarControl_OnApplyTemplate_Original;
CommandBarControl_OnApplyTemplate_t
    CommandBarControl_Wave1_OnApplyTemplate_Original;

void WINAPI CommandBarControl_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");
    CommandBarControl_OnApplyTemplate_Original(pThis);
    ScheduleCurrentThreadScan();
}

void WINAPI CommandBarControl_Wave1_OnApplyTemplate_Hook(void* pThis) {
    Wh_Log(L">");
    CommandBarControl_Wave1_OnApplyTemplate_Original(pThis);
    ScheduleCurrentThreadScan();
}

using CommandBarControl_GotFocusHandler_t = void(WINAPI*)(void* pThis,
                                                          void* sender,
                                                          void* args);
CommandBarControl_GotFocusHandler_t CommandBarControl_GotFocusHandler_Original;
CommandBarControl_GotFocusHandler_t
    CommandBarControl_Wave1_GotFocusHandler_Original;

void HandleCommandBarControlGotFocus(void* sender) {
    if (g_unloading || !sender)
        return;
    if (g_threadScanned && GetKnownCommandBarForCurrentThread())
        return;
    try {
        auto const& inspectable =
            *reinterpret_cast<wf::IInspectable const*>(sender);
        if (auto element =
                inspectable ? inspectable.try_as<mux::UIElement>() : nullptr) {
            ScanXamlRootForCommandBars(element);
        }
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void WINAPI CommandBarControl_GotFocusHandler_Hook(void* pThis,
                                                   void* sender,
                                                   void* args) {
    CommandBarControl_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

void WINAPI CommandBarControl_Wave1_GotFocusHandler_Hook(void* pThis,
                                                         void* sender,
                                                         void* args) {
    CommandBarControl_Wave1_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

// A member function returning a class type gets a hidden return pointer after
// `this`, and a by-value WinRT parameter is passed as a pointer to a temporary.
// The XamlRoot is read before the original runs, since the coroutine moves it
// into its frame. It's queried rather than assumed, so a different layout only
// makes the hook a no-op.
using CommandBarControl_OnDpiChanged_t = void*(WINAPI*)(void* pThis,
                                                        void* pRet,
                                                        void* sender,
                                                        void* args);
CommandBarControl_OnDpiChanged_t CommandBarControl_OnDpiChanged_Original;
CommandBarControl_OnDpiChanged_t CommandBarControl_Wave1_OnDpiChanged_Original;

mux::XamlRoot GetXamlRootFromByValueParam(void* param) {
    if (g_unloading || !param)
        return nullptr;
    try {
        wf::IInspectable inspectable;
        winrt::copy_from_abi(inspectable, *(void**)param);
        return inspectable ? inspectable.try_as<mux::XamlRoot>() : nullptr;
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
        return nullptr;
    }
}

void HandleCommandBarControlXamlRootChanged(mux::XamlRoot const& xamlRoot) {
    if (g_unloading || !xamlRoot)
        return;
    try {
        if (!IsXamlRootKnownForCurrentThread(xamlRoot))
            ScanXamlRoot(xamlRoot);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

void* WINAPI CommandBarControl_OnDpiChanged_Hook(void* pThis,
                                                 void* pRet,
                                                 void* sender,
                                                 void* args) {
    auto xamlRoot = GetXamlRootFromByValueParam(sender);
    void* result =
        CommandBarControl_OnDpiChanged_Original(pThis, pRet, sender, args);
    HandleCommandBarControlXamlRootChanged(xamlRoot);
    return result;
}

void* WINAPI CommandBarControl_Wave1_OnDpiChanged_Hook(void* pThis,
                                                       void* pRet,
                                                       void* sender,
                                                       void* args) {
    auto xamlRoot = GetXamlRootFromByValueParam(sender);
    void* result = CommandBarControl_Wave1_OnDpiChanged_Original(pThis, pRet,
                                                                 sender, args);
    HandleCommandBarControlXamlRootChanged(xamlRoot);
    return result;
}

std::atomic<bool> g_symbolsHooked;

enum class SymbolHookResult {
    Success,
    ResolutionFailed,
    NoSymbolFound,
};

SymbolHookResult HookFileExplorerExtensionsSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
            },
            &CommandBarManager_CommandBar_Original,
            CommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))",
            },
            &CommandBarControl_OnApplyTemplate_Original,
            CommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))",
            },
            &CommandBarControl_Wave1_OnApplyTemplate_Original,
            CommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
            },
            &CommandBarControl_GotFocusHandler_Original,
            CommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
            },
            &CommandBarControl_Wave1_GotFocusHandler_Original,
            CommandBarControl_Wave1_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: struct winrt::fire_and_forget __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnDpiChanged(struct winrt::Microsoft::UI::Xaml::XamlRoot,struct winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs))",
            },
            &CommandBarControl_OnDpiChanged_Original,
            CommandBarControl_OnDpiChanged_Hook,
            true,
        },
        {
            {
                LR"(public: struct winrt::fire_and_forget __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnDpiChanged(struct winrt::Microsoft::UI::Xaml::XamlRoot,struct winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs))",
            },
            &CommandBarControl_Wave1_OnDpiChanged_Original,
            CommandBarControl_Wave1_OnDpiChanged_Hook,
            true,
        },
    };

    if (!HookSymbols(module, fileExplorerExtensionsDllHooks,
                     ARRAYSIZE(fileExplorerExtensionsDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return SymbolHookResult::ResolutionFailed;
    }

    if (!CommandBarManager_CommandBar_Original &&
        !CommandBarControl_OnApplyTemplate_Original &&
        !CommandBarControl_Wave1_OnApplyTemplate_Original) {
        Wh_Log(L"No command bar symbol was found");
        return SymbolHookResult::NoSymbolFound;
    }

    return SymbolHookResult::Success;
}

HMODULE GetFileExplorerExtensionsModuleHandle() {
    return GetModuleHandle(L"FileExplorerExtensions.dll");
}

bool HookFileExplorerExtensionsIfLoaded(bool applyHooks) {
    if (g_symbolsHooked)
        return true;
    HMODULE module = GetFileExplorerExtensionsModuleHandle();
    if (!module)
        return true;
    if (g_symbolsHooked.exchange(true))
        return true;

    Wh_Log(L"Hooking FileExplorerExtensions.dll");
    switch (HookFileExplorerExtensionsSymbols(module)) {
        case SymbolHookResult::Success:
            break;
        case SymbolHookResult::ResolutionFailed:
            g_symbolsHooked = false;
            return false;
        case SymbolHookResult::NoSymbolFound:
            return false;
    }

    if (applyHooks)
        Wh_ApplyHookOperations();
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading || !lpLibFileName)
        return module;

    PCWSTR fileName = lpLibFileName;
    for (PCWSTR p = lpLibFileName; *p; p++) {
        if (*p == L'\\' || *p == L'/')
            fileName = p + 1;
    }

    if (_wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0 ||
        _wcsicmp(fileName, L"FileExplorerExtensions") == 0) {
        HookFileExplorerExtensionsIfLoaded(true);
    }
    return module;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0)
        return false;
    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);

    if (!hook)
        return false;

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

constexpr int kMaxMenuDepth = 2;

struct KeyName {
    UINT vk;
    PCWSTR name;
};

constexpr KeyName kKeyNames[] = {
    {VK_SPACE, L"Space"},    {VK_RETURN, L"Enter"},  {VK_RETURN, L"Return"},
    {VK_TAB, L"Tab"},        {VK_ESCAPE, L"Esc"},    {VK_ESCAPE, L"Escape"},
    {VK_BACK, L"Backspace"}, {VK_DELETE, L"Delete"}, {VK_DELETE, L"Del"},
    {VK_INSERT, L"Insert"},  {VK_INSERT, L"Ins"},    {VK_HOME, L"Home"},
    {VK_END, L"End"},        {VK_PRIOR, L"PageUp"},  {VK_PRIOR, L"PgUp"},
    {VK_NEXT, L"PageDown"},  {VK_NEXT, L"PgDn"},     {VK_UP, L"Up"},
    {VK_DOWN, L"Down"},      {VK_LEFT, L"Left"},     {VK_RIGHT, L"Right"},
    {VK_OEM_PLUS, L"Plus"},  {VK_OEM_PLUS, L"="},    {VK_OEM_MINUS, L"-"},
    {VK_OEM_COMMA, L","},    {VK_OEM_PERIOD, L"."},  {VK_OEM_1, L";"},
    {VK_OEM_2, L"/"},        {VK_OEM_3, L"`"},       {VK_OEM_4, L"["},
    {VK_OEM_5, L"\\"},       {VK_OEM_6, L"]"},       {VK_OEM_7, L"'"},
};

bool IsModifierKey(UINT vk) {
    switch (vk) {
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        case VK_LWIN:
        case VK_RWIN:
            return true;
    }
    return false;
}

// Plain keys would get in the way of typing, so only function keys may be
// used without Ctrl, Alt or Win.
bool IsValidKeyboardShortcut(KeyboardShortcut const& shortcut) {
    if (!shortcut.vk || IsModifierKey(shortcut.vk))
        return false;
    return (shortcut.modifiers & (MOD_CONTROL | MOD_ALT | MOD_WIN)) ||
           (shortcut.vk >= VK_F1 && shortcut.vk <= VK_F24);
}

UINT VkFromKeyName(std::wstring_view name) {
    if (name.size() == 1) {
        WCHAR c = towupper(name[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
            return c;
    }
    auto numberAfter = [&name](std::wstring_view prefix, int max) {
        if (name.size() <= prefix.size() || name.size() > prefix.size() + 2 ||
            _wcsnicmp(name.data(), prefix.data(), prefix.size()) != 0)
            return -1;
        int number = 0;
        for (WCHAR c : name.substr(prefix.size())) {
            if (c < L'0' || c > L'9')
                return -1;
            number = number * 10 + (c - L'0');
        }
        return number <= max ? number : -1;
    };
    if (int number = numberAfter(L"F", 24); number >= 1)
        return VK_F1 + number - 1;
    if (int number = numberAfter(L"Num", 9); number >= 0)
        return VK_NUMPAD0 + number;
    for (auto const& key : kKeyNames) {
        if (name.size() == wcslen(key.name) &&
            _wcsnicmp(name.data(), key.name, name.size()) == 0)
            return key.vk;
    }
    return 0;
}

// Parses text like "Ctrl+Shift+T". Returns false for invalid shortcuts.
bool ParseKeyboardShortcut(std::wstring_view text, KeyboardShortcut* result) {
    KeyboardShortcut shortcut;
    bool any = false;
    while (!text.empty()) {
        size_t plus = text.find(L'+');
        std::wstring_view token = text.substr(0, plus);
        text = plus == std::wstring_view::npos ? std::wstring_view()
                                               : text.substr(plus + 1);
        while (!token.empty() && iswspace(token.front()))
            token.remove_prefix(1);
        while (!token.empty() && iswspace(token.back()))
            token.remove_suffix(1);
        if (token.empty())
            return false;
        any = true;

        auto is = [&token](PCWSTR name) {
            return token.size() == wcslen(name) &&
                   _wcsnicmp(token.data(), name, token.size()) == 0;
        };
        if (is(L"Ctrl") || is(L"Control")) {
            shortcut.modifiers |= MOD_CONTROL;
        } else if (is(L"Alt")) {
            shortcut.modifiers |= MOD_ALT;
        } else if (is(L"Shift")) {
            shortcut.modifiers |= MOD_SHIFT;
        } else if (is(L"Win") || is(L"Windows")) {
            shortcut.modifiers |= MOD_WIN;
        } else {
            if (shortcut.vk)
                return false;
            shortcut.vk = VkFromKeyName(token);
            if (!shortcut.vk)
                return false;
        }
    }
    if (!any || !IsValidKeyboardShortcut(shortcut))
        return false;
    *result = shortcut;
    return true;
}

// The modifiers which are currently held down, as MOD_* flags.
UINT GetHeldModifiers() {
    UINT modifiers = 0;
    if (GetAsyncKeyState(VK_CONTROL) < 0)
        modifiers |= MOD_CONTROL;
    if (GetAsyncKeyState(VK_MENU) < 0)
        modifiers |= MOD_ALT;
    if (GetAsyncKeyState(VK_SHIFT) < 0)
        modifiers |= MOD_SHIFT;
    if (GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0)
        modifiers |= MOD_WIN;
    return modifiers;
}

void CollectShortcutBindings(std::vector<ActionItem> const& items,
                             int depth,
                             std::vector<ShortcutBinding>* bindings) {
    for (auto const& item : items) {
        bool isMenu = item.isMenu && depth < kMaxMenuDepth;
        if (isMenu) {
            CollectShortcutBindings(item.subItems, depth + 1, bindings);
            continue;
        }
        if (item.shortcut.empty() || item.command.empty())
            continue;
        KeyboardShortcut shortcut;
        if (!ParseKeyboardShortcut(item.shortcut, &shortcut)) {
            Wh_Log(L"Ignoring invalid keyboard shortcut \"%s\" of %s",
                   item.shortcut.c_str(), item.name.c_str());
            continue;
        }
        bindings->push_back({shortcut, item});
    }
}

// The hook is installed per File Explorer window thread, so it only sees the
// key messages of this process's File Explorer windows. It runs when a key
// message is retrieved, before Explorer's own accelerators, and on the
// window's own thread, so an item runs exactly like a click.
thread_local HHOOK g_shortcutHook;
// The key of the last handled shortcut, its repeats and release are
// swallowed too.
thread_local UINT g_shortcutSwallowedVk;

// All installed hooks, so that hooks of threads which couldn't be reached
// on unload can still be removed.
std::mutex g_shortcutHooksMutex;
std::unordered_map<DWORD, HHOOK> g_shortcutHooks;

bool IsFileExplorerWindow(HWND hWnd) {
    WCHAR className[64];
    return hWnd && GetClassName(hWnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, L"CabinetWClass") == 0;
}

LRESULT CALLBACK ShortcutKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // HC_NOREMOVE is a peek, the same message is seen again when it's removed.
    if (nCode != HC_ACTION || g_unloading)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    UINT vk = (UINT)wParam;
    UINT scanCode = (lParam >> 16) & 0xFF;
    bool keyUp = (lParam & 0x80000000) != 0;
    bool repeat = (lParam & 0x40000000) != 0;

    // Keys sent with SendInput by virtual key only, such as the ones of
    // SendKeyCombo, have no scan code. They're passed on, or an internal
    // command which sends the same key as its shortcut would be swallowed.
    if (scanCode == 0)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    if (keyUp) {
        if (vk == g_shortcutSwallowedVk) {
            g_shortcutSwallowedVk = 0;
            return 1;
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (vk == g_shortcutSwallowedVk)
        return 1;
    if (repeat || IsModifierKey(vk))
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    // Keys of other windows of the thread, such as the settings window, are
    // left alone.
    HWND hFocusWnd = GetFocus();
    HWND hWnd = GetAncestor(hFocusWnd ? hFocusWnd : GetActiveWindow(), GA_ROOT);
    if (!IsFileExplorerWindow(hWnd))
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    UINT modifiers = GetHeldModifiers();
    std::optional<ActionItem> item;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        for (auto const& binding : g_settings.shortcuts) {
            if (binding.shortcut.vk == vk &&
                binding.shortcut.modifiers == modifiers) {
                item = binding.item;
                break;
            }
        }
    }
    if (!item)
        return CallNextHookEx(nullptr, nCode, wParam, lParam);

    g_shortcutSwallowedVk = vk;

    // Releasing Alt or Win alone would open the menu bar or the Start menu,
    // since the key in between is swallowed. An unassigned key in between
    // prevents that. It has no scan code, so the hook passes it on.
    if (modifiers & (MOD_ALT | MOD_WIN)) {
        INPUT inputs[2] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = 0xE8;
        inputs[1] = inputs[0];
        inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, inputs, sizeof(INPUT));
    }

    Wh_Log(L"Keyboard shortcut %s for window %08X", item->shortcut.c_str(),
           (DWORD)(ULONG_PTR)hWnd);
    InvokeActionItem(hWnd, *item);
    return 1;
}

// Called from the thread of a File Explorer window when its command bar is
// found. The hook checks the shortcuts on each key press, so it doesn't need
// to change with the settings.
void InstallShortcutHookForCurrentThread() {
    if (g_shortcutHook || g_unloading)
        return;

    DWORD threadId = GetCurrentThreadId();
    g_shortcutHook =
        SetWindowsHookEx(WH_KEYBOARD, ShortcutKeyboardProc, nullptr, threadId);
    if (!g_shortcutHook) {
        Wh_Log(L"SetWindowsHookEx(WH_KEYBOARD) failed: %u", GetLastError());
        return;
    }

    std::lock_guard<std::mutex> lock(g_shortcutHooksMutex);
    g_shortcutHooks[threadId] = g_shortcutHook;
}

void RemoveShortcutHookForCurrentThread() {
    if (!g_shortcutHook)
        return;

    {
        std::lock_guard<std::mutex> lock(g_shortcutHooksMutex);
        g_shortcutHooks.erase(GetCurrentThreadId());
    }
    UnhookWindowsHookEx(g_shortcutHook);
    g_shortcutHook = nullptr;
    g_shortcutSwallowedVk = 0;
}

// Removes the hooks of threads which couldn't be reached on unload.
void RemoveLeftoverShortcutHooks() {
    std::unordered_map<DWORD, HHOOK> leftovers;
    {
        std::lock_guard<std::mutex> lock(g_shortcutHooksMutex);
        leftovers.swap(g_shortcutHooks);
    }
    for (auto const& [threadId, hook] : leftovers) {
        Wh_Log(L"Removing leftover keyboard hook of thread %u", threadId);
        UnhookWindowsHookEx(hook);
    }
}

std::vector<HWND> GetFileExplorerWnds() {
    std::vector<HWND> hWnds;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& list = *(std::vector<HWND>*)lParam;
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
                return TRUE;
            WCHAR className[64];
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                list.push_back(hWnd);
            }
            return TRUE;
        },
        (LPARAM)&hWnds);
    return hWnds;
}

ActionItem LoadActionItem(PCWSTR prefix, int depth, bool* isEmpty) {
    auto name = WindhawkUtils::StringSetting::make(L"%s.name", prefix);
    auto command = WindhawkUtils::StringSetting::make(L"%s.command", prefix);
    auto parameters =
        WindhawkUtils::StringSetting::make(L"%s.parameters", prefix);
    auto shortcut = WindhawkUtils::StringSetting::make(L"%s.shortcut", prefix);
    auto iconGlyph =
        WindhawkUtils::StringSetting::make(L"%s.iconGlyph", prefix);

    ActionItem item;
    item.name = name.get();
    item.command = command.get();
    item.parameters = parameters.get();
    item.shortcut = shortcut.get();
    item.matchExtensions =
        WindhawkUtils::StringSetting::make(L"%s.matchExtensions", prefix).get();
    item.icon = iconGlyph.get();
    item.hideIcon = Wh_GetIntSetting(L"%s.hideIcon", prefix) != 0;
    item.showLabel = Wh_GetIntSetting(L"%s.showLabel", prefix) != 0;
    item.labelText = Wh_GetStringSetting(L"%s.labelText", prefix);
    item.separatorAfter = Wh_GetIntSetting(L"%s.separatorAfter", prefix) != 0;
    item.runAsAdmin = Wh_GetIntSetting(L"%s.runAsAdmin", prefix) != 0;
    item.windowStyle =
        WindhawkUtils::StringSetting::make(L"%s.windowStyle", prefix).get();

    if (depth < kMaxMenuDepth) {
        auto type = WindhawkUtils::StringSetting::make(L"%s.type", prefix);
        item.isMenu = wcscmp(type.get(), L"menu") == 0;
        for (int i = 0; i < 100; i++) {
            WCHAR subPrefix[256];
            swprintf(subPrefix, ARRAYSIZE(subPrefix), L"%s.subItems[%d]",
                     prefix, i);
            bool subEmpty = false;
            ActionItem subItem =
                LoadActionItem(subPrefix, depth + 1, &subEmpty);
            if (subEmpty)
                break;
            item.subItems.push_back(std::move(subItem));
        }
    }

    *isEmpty =
        item.name.empty() && item.command.empty() && item.subItems.empty();
    return item;
}

void LoadSettings() {
    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    std::lock_guard<std::mutex> lock(g_settings.mutex);

    g_settings.placeOnSecondaryBar =
        Wh_GetIntSetting(L"placeOnSecondaryBar") != 0;
    g_settings.disabledInsteadOfHidden =
        Wh_GetIntSetting(L"disabledInsteadOfHidden") != 0;
    g_settings.openMenuOnHover = Wh_GetIntSetting(L"openMenuOnHover") != 0;
    int menuHoverDelay = Wh_GetIntSetting(L"menuHoverDelay");
    g_settings.menuHoverDelay = menuHoverDelay >= 0 ? menuHoverDelay : 0;

    for (int i = 0; i < kDefaultButtonCount; i++) {
        g_settings.hideDefaultButtons[i] =
            Wh_GetIntSetting(L"hideDefaultButtons.%s",
                             kDefaultButtons[i].settingKey) != 0;
    }

    g_settings.hideMoreButton =
        Wh_GetIntSetting(L"hideDefaultButtons.moreOptions") != 0;
    g_settings.hideDetailsButton =
        Wh_GetIntSetting(L"hideDefaultButtons.details") != 0;

    g_settings.showMenuChevrons = Wh_GetIntSetting(L"showMenuChevrons") != 0;
    memset(g_settings.hideDefaultButtonLabels, 0,
           sizeof(g_settings.hideDefaultButtonLabels));
    for (int index : kLabeledDefaultButtons) {
        g_settings.hideDefaultButtonLabels[index] =
            Wh_GetIntSetting(L"hideDefaultButtonLabels.%s",
                             kDefaultButtons[index].settingKey) != 0;
    }
    g_settings.hideDetailsButtonLabel =
        Wh_GetIntSetting(L"hideDefaultButtonLabels.details") != 0;

    g_settings.defaultButtonOrderText =
        WindhawkUtils::StringSetting::make(L"defaultButtonOrder").get();
    g_settings.defaultButtonOrder =
        ParseDefaultButtonOrder(g_settings.defaultButtonOrderText);

    memset(g_settings.hideSeparatorAfterButton, 0,
           sizeof(g_settings.hideSeparatorAfterButton));
    g_settings.hideSeparatorAfterButton[kNewButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterNew") != 0;
    g_settings.hideSeparatorAfterButton[kDeleteButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterDelete") != 0;
    g_settings.hideSeparatorAfterButton[kViewButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterView") != 0;

    int itemSpacing = Wh_GetIntSetting(L"itemSpacing");
    g_settings.itemSpacing = itemSpacing < 0 ? -1 : itemSpacing;

    g_settings.showSettingsItem = Wh_GetIntSetting(L"showSettingsItem") != 0;
    int settingsItemOrder = Wh_GetIntSetting(L"settingsItemOrder");
    g_settings.settingsItemOrder =
        settingsItemOrder < 0 ? -1 : settingsItemOrder;

    g_settings.newPlus = NewPlusSettings{};
    g_settings.newPlus.enabled = Wh_GetIntSetting(L"newPlus.enabled") != 0;
    g_settings.newPlus.showLabel = Wh_GetIntSetting(L"newPlus.showLabel") != 0;
    g_settings.newPlus.buttonLabel =
        WindhawkUtils::StringSetting::make(L"newPlus.buttonLabel").get();
    g_settings.newPlus.buttonIcon =
        WindhawkUtils::StringSetting::make(L"newPlus.buttonIcon").get();
    g_settings.newPlus.templateFolder = TrimQuotesAndSpaces(
        WindhawkUtils::StringSetting::make(L"newPlus.templateFolder").get());
    g_settings.newPlus.showIcons = Wh_GetIntSetting(L"newPlus.showIcons") != 0;
    g_settings.newPlus.showTemplatesFolderItem =
        Wh_GetIntSetting(L"newPlus.showTemplatesFolderItem") != 0;
    g_settings.newPlus.keepOriginalNewButton =
        Wh_GetIntSetting(L"newPlus.keepOriginalNewButton") != 0;

    g_settings.contextMenuItem = ContextMenuItemSettings{};
    g_settings.contextMenuItem.enabled =
        Wh_GetIntSetting(L"contextMenuItem.enabled") != 0;
    g_settings.contextMenuItem.useNilesoftShell =
        Wh_GetIntSetting(L"contextMenuItem.useNilesoftShell") != 0;
    g_settings.contextMenuItem.showLabel =
        Wh_GetIntSetting(L"contextMenuItem.showLabel") != 0;
    g_settings.contextMenuItem.buttonLabel =
        WindhawkUtils::StringSetting::make(L"contextMenuItem.buttonLabel")
            .get();
    g_settings.contextMenuItem.buttonIcon = TrimQuotesAndSpaces(
        WindhawkUtils::StringSetting::make(L"contextMenuItem.buttonIcon")
            .get());

    g_settings.items.clear();
    for (int i = 0; i < 100; i++) {
        WCHAR prefix[64];
        swprintf(prefix, ARRAYSIZE(prefix), L"items[%d]", i);
        bool isEmpty = false;
        ActionItem item = LoadActionItem(prefix, 0, &isEmpty);
        if (isEmpty)
            break;
        g_settings.items.push_back(std::move(item));
    }

    g_settings.shortcuts.clear();
    CollectShortcutBindings(g_settings.items, 0, &g_settings.shortcuts);
}

struct EditableSettings {
    std::vector<ActionItem> items;
    bool placeOnSecondaryBar = false;
    bool disabledInsteadOfHidden = false;
    bool openMenuOnHover = false;
    int menuHoverDelay = 400;
    int itemSpacing = -1;
    bool showSettingsItem = true;
    int settingsItemOrder = -1;
    bool hideDefaultButtons[kDefaultButtonCount] = {};
    bool hideSeparatorAfterNew = false;
    bool hideSeparatorAfterDelete = false;
    bool hideSeparatorAfterView = false;
    bool hideMoreButton = false;
    bool hideDetailsButton = false;
    bool showMenuChevrons = false;
    bool hideDefaultButtonLabels[kDefaultButtonCount] = {};
    bool hideDetailsButtonLabel = false;
    std::wstring defaultButtonOrder;
    NewPlusSettings newPlus;
    ContextMenuItemSettings contextMenuItem;
};

EditableSettings SnapshotSettings() {
    std::lock_guard<std::mutex> lock(g_settings.mutex);
    EditableSettings settings;
    settings.items = g_settings.items;
    settings.placeOnSecondaryBar = g_settings.placeOnSecondaryBar;
    settings.disabledInsteadOfHidden = g_settings.disabledInsteadOfHidden;
    settings.openMenuOnHover = g_settings.openMenuOnHover;
    settings.menuHoverDelay = g_settings.menuHoverDelay;
    settings.itemSpacing = g_settings.itemSpacing;
    settings.showSettingsItem = g_settings.showSettingsItem;
    settings.settingsItemOrder = g_settings.settingsItemOrder;
    for (int i = 0; i < kDefaultButtonCount; i++)
        settings.hideDefaultButtons[i] = g_settings.hideDefaultButtons[i];
    settings.hideSeparatorAfterNew =
        g_settings.hideSeparatorAfterButton[kNewButtonIndex];
    settings.hideSeparatorAfterDelete =
        g_settings.hideSeparatorAfterButton[kDeleteButtonIndex];
    settings.hideSeparatorAfterView =
        g_settings.hideSeparatorAfterButton[kViewButtonIndex];
    settings.hideMoreButton = g_settings.hideMoreButton;
    settings.hideDetailsButton = g_settings.hideDetailsButton;
    settings.showMenuChevrons = g_settings.showMenuChevrons;
    for (int i = 0; i < kDefaultButtonCount; i++) {
        settings.hideDefaultButtonLabels[i] =
            g_settings.hideDefaultButtonLabels[i];
    }
    settings.hideDetailsButtonLabel = g_settings.hideDetailsButtonLabel;
    settings.defaultButtonOrder = g_settings.defaultButtonOrderText;
    settings.newPlus = g_settings.newPlus;
    settings.contextMenuItem = g_settings.contextMenuItem;
    return settings;
}

enum class SettingKind {
    Header,
    Bool,
    Int,
    String,
};

struct SettingField {
    SettingKind kind = SettingKind::Header;
    std::wstring key;
    std::wstring title;
    std::wstring description;
    PCWSTR glyph = nullptr;
    void* value = nullptr;
    int minimum = 0;
    bool inverted = false;
    bool wide = false;
};

struct SettingsPageDef {
    std::wstring tag;
    std::wstring title;
    std::wstring description;
    PCWSTR glyph = nullptr;
    std::vector<SettingField> fields;
};

SettingField HeaderField(PCWSTR title) {
    SettingField field;
    field.title = title;
    return field;
}

SettingField BoolField(std::wstring key,
                       PCWSTR title,
                       PCWSTR description,
                       bool* value,
                       PCWSTR glyph = nullptr,
                       bool inverted = false) {
    SettingField field;
    field.kind = SettingKind::Bool;
    field.key = std::move(key);
    field.title = title;
    field.description = description;
    field.value = value;
    field.glyph = glyph;
    field.inverted = inverted;
    return field;
}

SettingField IntField(PCWSTR key,
                      PCWSTR title,
                      PCWSTR description,
                      int* value,
                      int minimum,
                      PCWSTR glyph = nullptr) {
    SettingField field;
    field.kind = SettingKind::Int;
    field.key = key;
    field.title = title;
    field.description = description;
    field.value = value;
    field.minimum = minimum;
    field.glyph = glyph;
    return field;
}

SettingField StringField(PCWSTR key,
                         PCWSTR title,
                         PCWSTR description,
                         std::wstring* value,
                         bool wide,
                         PCWSTR glyph = nullptr) {
    SettingField field;
    field.kind = SettingKind::String;
    field.key = key;
    field.title = title;
    field.description = description;
    field.value = value;
    field.wide = wide;
    field.glyph = glyph;
    return field;
}

constexpr PCWSTR kDefaultButtonTitles[kDefaultButtonCount] = {
    L"New",         L"Cut",
    L"Copy",        L"Paste",
    L"Rename",      L"Share",
    L"Delete",      L"Sort",
    L"View",        L"Set as background",
    L"Rotate left", L"Rotate right",
    L"Extract all",
};

constexpr PCWSTR kDefaultButtonGlyphs[kDefaultButtonCount] = {
    L"\uE710", L"\uE8C6", L"\uE8C8", L"\uE77F", L"\uE8AC", L"\uE72D", L"\uE74D",
    L"\uE8CB", L"\uE890", L"\uE91B", L"\uE7AD", L"\uE7AD", L"\uE8B7",
};

std::vector<SettingsPageDef> DescribeSettingsPages(EditableSettings& s) {
    auto buttonField = [&s](int index, PCWSTR description = L"") {
        return BoolField(std::wstring(L"hideDefaultButtons.") +
                             kDefaultButtons[index].settingKey,
                         kDefaultButtonTitles[index], description,
                         &s.hideDefaultButtons[index],
                         kDefaultButtonGlyphs[index], true);
    };

    auto labelField = [&s](int index) {
        std::wstring title = std::wstring(L"Hide the ") +
                             kDefaultButtonTitles[index] + L" label";
        return BoolField(std::wstring(L"hideDefaultButtonLabels.") +
                             kDefaultButtons[index].settingKey,
                         title.c_str(), L"Show only its icon.",
                         &s.hideDefaultButtonLabels[index],
                         kDefaultButtonGlyphs[index]);
    };

    std::vector<SettingsPageDef> pages;

    SettingsPageDef builtIn;
    builtIn.tag = L"builtIn";
    builtIn.title = L"Built-in buttons";
    builtIn.description =
        L"Turn off the built-in File Explorer command bar elements you don't "
        L"need. The separator of the contextual commands is hidden along with "
        L"them, once none of them is left to show.";
    builtIn.glyph = L"\uE7B3";
    builtIn.fields = {
        HeaderField(L"Buttons"),
        buttonField(0),
        buttonField(1),
        buttonField(2),
        buttonField(3),
        buttonField(4),
        buttonField(5),
        buttonField(6),
        buttonField(7),
        buttonField(8),
        BoolField(L"hideDefaultButtons.moreOptions",
                  L"See more (the three dots menu)", L"", &s.hideMoreButton,
                  L"\uE712", true),
        BoolField(L"hideDefaultButtons.details", L"Details pane toggle", L"",
                  &s.hideDetailsButton, L"\uE946", true),
        HeaderField(L"Labels"),
        labelField(kNewButtonIndex),
        labelField(kSortButtonIndex),
        labelField(kViewButtonIndex),
        BoolField(L"hideDefaultButtonLabels.details",
                  L"Hide the Details pane toggle label", L"Show only its icon.",
                  &s.hideDetailsButtonLabel, L"\uE946"),
        HeaderField(L"Contextual commands"),
        buttonField(9, L"Shown when an image file is selected."),
        buttonField(10, L"Shown when an image file is selected."),
        buttonField(11, L"Shown when an image file is selected."),
        buttonField(12, L"Shown when a zip or other archive file is selected."),
        HeaderField(L"Separators"),
        BoolField(L"hideDefaultButtons.separatorAfterNew",
                  L"Vertical separator after New", L"",
                  &s.hideSeparatorAfterNew, L"\uE76F", true),
        BoolField(L"hideDefaultButtons.separatorAfterDelete",
                  L"Vertical separator after Delete", L"",
                  &s.hideSeparatorAfterDelete, L"\uE76F", true),
        BoolField(L"hideDefaultButtons.separatorAfterView",
                  L"Vertical separator after View", L"",
                  &s.hideSeparatorAfterView, L"\uE76F", true),
    };
    pages.push_back(std::move(builtIn));

    SettingsPageDef newPlus;
    newPlus.tag = L"newPlus";
    newPlus.title = L"New+";
    newPlus.description =
        L"Replace Explorer's New button with a New+ button which creates files "
        L"and folders from your own templates, the way the PowerToys New+ "
        L"utility does. PowerToys isn't required.";
    newPlus.glyph = L"\uE8F4";
    newPlus.fields = {
        BoolField(L"newPlus.enabled", L"Replace New with New+",
                  L"Put a New+ button in the place of Explorer's New button.",
                  &s.newPlus.enabled, L"\uE8F4"),
        HeaderField(L"Button"),
        BoolField(L"newPlus.showLabel", L"Show the button label",
                  L"Show the label next to the icon, with a chevron after the "
                  L"text. When disabled, the label becomes the tooltip.",
                  &s.newPlus.showLabel, L"\uE8E9"),
        StringField(L"newPlus.buttonLabel", L"Button label",
                    L"Leave empty for an icon-only button without a chevron.",
                    &s.newPlus.buttonLabel, false, L"\uE8D2"),
        StringField(L"newPlus.buttonIcon", L"Button icon glyph or icon path",
                    L"Leave empty to reuse the icon of Explorer's own New "
                    L"button. Alternatively, a Segoe Fluent Icons code point "
                    L"such as E710, or an .exe, .dll or .ico path with an "
                    L"optional icon index.",
                    &s.newPlus.buttonIcon, true, L"\uE8B9"),
        BoolField(L"newPlus.keepOriginalNewButton",
                  L"Keep Explorer's New button",
                  L"Show the built-in New button next to the New+ button "
                  L"instead of hiding it.",
                  &s.newPlus.keepOriginalNewButton, L"\uE710"),
        HeaderField(L"Templates"),
        StringField(L"newPlus.templateFolder", L"Templates folder",
                    L"Leave empty to use the folder configured in PowerToys, "
                    L"or %LOCALAPPDATA%\\Microsoft\\PowerToys\\NewPlus\\"
                    L"Templates if PowerToys isn't installed.",
                    &s.newPlus.templateFolder, true, L"\uE8B7"),
        BoolField(L"newPlus.showIcons", L"Show template icons",
                  L"Disable it if opening the menu feels slow with many "
                  L"templates.",
                  &s.newPlus.showIcons, L"\uE8B9"),
        BoolField(L"newPlus.showTemplatesFolderItem",
                  L"Add an \"Open templates folder\" entry",
                  L"Opens the templates folder, creating it if it doesn't "
                  L"exist yet.",
                  &s.newPlus.showTemplatesFolderItem, L"\uE838"),
    };
    pages.push_back(std::move(newPlus));

    SettingsPageDef contextMenu;
    contextMenu.tag = L"contextMenu";
    contextMenu.title = L"Context menu item";
    contextMenu.description =
        L"Add an item which expands the real File Explorer shell context menu "
        L"of the current selection or folder.";
    contextMenu.glyph = L"\uE8FD";
    contextMenu.fields = {
        BoolField(L"contextMenuItem.enabled", L"Add the context menu item",
                  L"Like right-clicking in File Explorer.",
                  &s.contextMenuItem.enabled, L"\uE8FD"),
        BoolField(L"contextMenuItem.useNilesoftShell",
                  L"Let File Explorer show the menu (for Nilesoft Shell)",
                  L"Ask the file list to show its own context menu instead of "
                  L"building the classic shell menu. The menu is positioned by "
                  L"Explorer.",
                  &s.contextMenuItem.useNilesoftShell, L"\uE7C4"),
        HeaderField(L"Appearance"),
        BoolField(L"contextMenuItem.showLabel", L"Show the item label",
                  L"When disabled, only the icon is shown and the label "
                  L"becomes the tooltip.",
                  &s.contextMenuItem.showLabel, L"\uE8E9"),
        StringField(L"contextMenuItem.buttonLabel", L"Item label",
                    L"The label and tooltip of the context menu item.",
                    &s.contextMenuItem.buttonLabel, false, L"\uE8D2"),
        StringField(L"contextMenuItem.buttonIcon",
                    L"Item icon glyph or icon path",
                    L"A Segoe Fluent Icons code point, or an .exe, .dll or "
                    L".ico path with an optional icon index. Leave empty for "
                    L"the default glyph.",
                    &s.contextMenuItem.buttonIcon, true, L"\uE8B9"),
    };
    pages.push_back(std::move(contextMenu));

    SettingsPageDef general;
    general.tag = L"general";
    general.title = L"General";
    general.glyph = L"\uE713";
    general.fields = {
        HeaderField(L"Settings item"),
        BoolField(L"showSettingsItem",
                  L"Show Explorer Command Bar Settings Item",
                  L"Show a cog button in the command bar which opens this "
                  L"window.",
                  &s.showSettingsItem, L"\uE713"),
        IntField(L"settingsItemOrder",
                 L"Explorer Command Bar Settings Item Order",
                 L"The position of the cog button among the toolbar items. 0 "
                 L"places it before the first item, 1 after the first item, "
                 L"and so on. -1 places it after the last item.",
                 &s.settingsItemOrder, -1, L"\uE8CB"),
        HeaderField(L"Layout"),
        BoolField(L"disabledInsteadOfHidden",
                  L"Disable buttons instead of hiding them",
                  L"Gray out inactive buttons instead of removing them from "
                  L"the toolbar.",
                  &s.disabledInsteadOfHidden, L"\uE7A6"),
        BoolField(L"placeOnSecondaryBar", L"Place buttons on the right side",
                  L"Show custom buttons on the right side of File Explorer "
                  L"(next to the Details and Preview pane toggles) instead of "
                  L"the main toolbar.",
                  &s.placeOnSecondaryBar, L"\uE90D"),
        BoolField(L"showMenuChevrons", L"Show chevrons on dropdowns",
                  L"Show a small chevron next to custom toolbar buttons which "
                  L"open a dropdown menu.",
                  &s.showMenuChevrons, L"\uE70D"),
        IntField(L"itemSpacing", L"Item spacing (pixels at 100% scaling)",
                 L"Horizontal spacing between the command bar buttons. -1 "
                 L"leaves the default spacing unchanged. Any other value also "
                 L"removes the built-in buttons' minimum width.",
                 &s.itemSpacing, -1, L"\uE799"),
        HeaderField(L"Menus"),
        BoolField(L"openMenuOnHover", L"Open menus on hover",
                  L"Open dropdown menus by hovering over the button instead of "
                  L"clicking it. Applies to the New+ button as well.",
                  &s.openMenuOnHover, L"\uE8B0"),
        IntField(L"menuHoverDelay", L"Hover delay (milliseconds)",
                 L"How long the cursor has to stay over the button before the "
                 L"menu opens.",
                 &s.menuHoverDelay, 0, L"\uE916"),
    };
    pages.push_back(std::move(general));

    return pages;
}

struct RegEntry {
    std::wstring name;
    bool isString = false;
    bool isBool = false;
    std::wstring text;
    DWORD number = 0;
    bool operator==(RegEntry const&) const = default;
};

RegEntry StringEntry(std::wstring name, std::wstring const& value) {
    RegEntry entry;
    entry.name = std::move(name);
    entry.isString = true;
    for (WCHAR c : value) {
        if (c != L'\r' && c != L'\n')
            entry.text += c;
    }
    return entry;
}

RegEntry NumberEntry(std::wstring name, int value) {
    RegEntry entry;
    entry.name = std::move(name);
    entry.number = (DWORD)value;
    return entry;
}

// Stored as a number like any other, but exported as true or false.
RegEntry BoolEntry(std::wstring name, bool value) {
    RegEntry entry = NumberEntry(std::move(name), value);
    entry.isBool = true;
    return entry;
}

// Mirrors LoadActionItem, which stops at the first empty item.
bool IsEmptyActionItem(ActionItem const& item) {
    if (!item.name.empty() || !item.command.empty())
        return false;
    return std::all_of(item.subItems.begin(), item.subItems.end(),
                       IsEmptyActionItem);
}

void AppendItemEntries(std::vector<RegEntry>& entries,
                       std::wstring const& prefix,
                       ActionItem const& item,
                       int depth) {
    if (depth < kMaxMenuDepth)
        entries.push_back(
            StringEntry(prefix + L".type", item.isMenu ? L"menu" : L"button"));
    entries.push_back(StringEntry(prefix + L".name", item.name));
    entries.push_back(StringEntry(prefix + L".command", item.command));
    entries.push_back(StringEntry(prefix + L".parameters", item.parameters));
    entries.push_back(StringEntry(prefix + L".shortcut", item.shortcut));
    entries.push_back(
        StringEntry(prefix + L".matchExtensions", item.matchExtensions));
    entries.push_back(StringEntry(prefix + L".iconGlyph", item.icon));
    entries.push_back(BoolEntry(prefix + L".hideIcon", item.hideIcon));
    if (depth == 0) {
        entries.push_back(BoolEntry(prefix + L".showLabel", item.showLabel));
        entries.push_back(StringEntry(prefix + L".labelText", item.labelText));
    }
    entries.push_back(
        BoolEntry(prefix + L".separatorAfter", item.separatorAfter));
    entries.push_back(BoolEntry(prefix + L".runAsAdmin", item.runAsAdmin));
    entries.push_back(StringEntry(
        prefix + L".windowStyle",
        kWindowStyles[WindowStyleIndex(item.windowStyle)].settingValue));

    if (depth < kMaxMenuDepth) {
        int index = 0;
        for (auto const& subItem : item.subItems) {
            if (IsEmptyActionItem(subItem))
                continue;
            AppendItemEntries(
                entries,
                prefix + L".subItems[" + std::to_wstring(index++) + L"]",
                subItem, depth + 1);
        }
    }
}

std::vector<RegEntry> BuildRegEntries(EditableSettings& settings) {
    std::vector<RegEntry> entries;
    // Edited by dragging the built-in buttons in the preview, so it isn't
    // one of the fields of the settings pages.
    entries.push_back(
        StringEntry(L"defaultButtonOrder", settings.defaultButtonOrder));
    int index = 0;
    for (auto const& item : settings.items) {
        if (IsEmptyActionItem(item))
            continue;
        AppendItemEntries(entries, L"items[" + std::to_wstring(index++) + L"]",
                          item, 0);
    }

    for (auto const& page : DescribeSettingsPages(settings)) {
        for (auto const& field : page.fields) {
            switch (field.kind) {
                case SettingKind::Bool:
                    entries.push_back(
                        BoolEntry(field.key, *(bool*)field.value));
                    break;
                case SettingKind::Int:
                    entries.push_back(
                        NumberEntry(field.key, *(int*)field.value));
                    break;
                case SettingKind::String:
                    entries.push_back(
                        StringEntry(field.key, *(std::wstring*)field.value));
                    break;
                case SettingKind::Header:
                    break;
            }
        }
    }
    return entries;
}

constexpr WCHAR kModRegistryPath[] =
    L"SOFTWARE\\Windhawk\\Engine\\Mods\\" WH_MOD_ID;

struct SaveResult {
    bool succeeded = false;
    std::wstring error;
};

std::vector<std::wstring> ReadRegistryValueNames(HKEY key) {
    std::vector<std::wstring> names;
    for (DWORD i = 0;; i++) {
        WCHAR name[1024];
        DWORD length = ARRAYSIZE(name);
        LSTATUS status = RegEnumValueW(key, i, name, &length, nullptr, nullptr,
                                       nullptr, nullptr);
        if (status == ERROR_SUCCESS)
            names.push_back(name);
        else if (status != ERROR_MORE_DATA)
            break;
    }
    return names;
}

LSTATUS WriteSettingsToRegistry(std::vector<RegEntry> const& entries,
                                std::vector<std::wstring> const& staleNames,
                                DWORD changeTime) {
    HKEY modKey;
    LSTATUS status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, kModRegistryPath, 0,
                                   KEY_SET_VALUE | KEY_CREATE_SUB_KEY, &modKey);
    if (status != ERROR_SUCCESS)
        return status;

    HKEY settingsKey;
    status = RegCreateKeyExW(modKey, L"Settings", 0, nullptr, 0, KEY_SET_VALUE,
                             nullptr, &settingsKey, nullptr);
    if (status == ERROR_SUCCESS) {
        for (auto const& entry : entries) {
            if (entry.isString) {
                status = RegSetValueExW(
                    settingsKey, entry.name.c_str(), 0, REG_SZ,
                    (BYTE const*)entry.text.c_str(),
                    (DWORD)((entry.text.size() + 1) * sizeof(WCHAR)));
            } else {
                status = RegSetValueExW(settingsKey, entry.name.c_str(), 0,
                                        REG_DWORD, (BYTE const*)&entry.number,
                                        sizeof(entry.number));
            }
            if (status != ERROR_SUCCESS)
                break;
        }
        if (status == ERROR_SUCCESS) {
            for (auto const& name : staleNames)
                RegDeleteValueW(settingsKey, name.c_str());
        }
        RegCloseKey(settingsKey);
    }

    if (status == ERROR_SUCCESS) {
        status = RegSetValueExW(modKey, L"SettingsChangeTime", 0, REG_DWORD,
                                (BYTE const*)&changeTime, sizeof(changeTime));
    }
    RegCloseKey(modKey);
    return status;
}

std::wstring EscapeRegFileString(std::wstring const& value) {
    std::wstring result;
    for (WCHAR c : value) {
        if (c == L'\\' || c == L'"')
            result += L'\\';
        result += c;
    }
    return result;
}

std::wstring BuildRegFile(std::vector<RegEntry> const& entries,
                          std::vector<std::wstring> const& staleNames,
                          DWORD changeTime) {
    std::wstring modKey =
        std::wstring(L"HKEY_LOCAL_MACHINE\\") + kModRegistryPath;
    std::wstring text = L"Windows Registry Editor Version 5.00\r\n\r\n";
    text += L"[" + modKey + L"\\Settings]\r\n";
    for (auto const& entry : entries) {
        text += L"\"" + EscapeRegFileString(entry.name) + L"\"=";
        if (entry.isString) {
            text += L"\"" + EscapeRegFileString(entry.text) + L"\"";
        } else {
            WCHAR number[16];
            swprintf(number, ARRAYSIZE(number), L"dword:%08x", entry.number);
            text += number;
        }
        text += L"\r\n";
    }
    for (auto const& name : staleNames)
        text += L"\"" + EscapeRegFileString(name) + L"\"=-\r\n";

    WCHAR time[32];
    swprintf(time, ARRAYSIZE(time), L"dword:%08x", changeTime);
    text +=
        L"\r\n[" + modKey + L"]\r\n\"SettingsChangeTime\"=" + time + L"\r\n";
    return text;
}

bool WriteUtf16File(std::wstring const& path, std::wstring const& text) {
    HANDLE hFile =
        CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                    FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    WCHAR bom = 0xFEFF;
    DWORD written = 0;
    bool succeeded =
        WriteFile(hFile, &bom, sizeof(bom), &written, nullptr) &&
        WriteFile(hFile, text.data(), (DWORD)(text.size() * sizeof(WCHAR)),
                  &written, nullptr);
    CloseHandle(hFile);
    return succeeded;
}

DWORD RunElevatedRegImport(std::wstring const& regFilePath, HWND hOwnerWnd) {
    WCHAR systemDir[MAX_PATH];
    if (!GetSystemDirectoryW(systemDir, ARRAYSIZE(systemDir)))
        return GetLastError();
    std::wstring regExe = JoinPath(systemDir, L"reg.exe");
    std::wstring parameters = L"import \"" + regFilePath + L"\"";

    SHELLEXECUTEINFOW execInfo{};
    execInfo.cbSize = sizeof(execInfo);
    execInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    execInfo.hwnd = hOwnerWnd;
    execInfo.lpVerb = L"runas";
    execInfo.lpFile = regExe.c_str();
    execInfo.lpParameters = parameters.c_str();
    execInfo.nShow = SW_HIDE;
    if (!ShellExecuteExW(&execInfo))
        return GetLastError();
    if (!execInfo.hProcess)
        return ERROR_GEN_FAILURE;

    WaitForSingleObject(execInfo.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(execInfo.hProcess, &exitCode);
    CloseHandle(execInfo.hProcess);
    if (exitCode != 0) {
        Wh_Log(L"reg.exe import exited with %u", exitCode);
        return ERROR_GEN_FAILURE;
    }
    return ERROR_SUCCESS;
}

// Windhawk keeps mod settings in a registry key which File Explorer can only
// read, so unless the process happens to be elevated, the settings are
// imported with an elevated reg.exe. Bumping SettingsChangeTime makes the
// engine call Wh_ModSettingsChanged, just like a change made in Windhawk.
SaveResult WriteModSettings(std::vector<RegEntry> const& entries,
                            HWND hOwnerWnd) {
    SaveResult result;
    HKEY modKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, kModRegistryPath, 0, KEY_READ,
                      &modKey) != ERROR_SUCCESS) {
        result.error =
            L"The settings of this mod weren't found in the registry. Saving "
            L"from this window requires the installed (not portable) version "
            L"of Windhawk.";
        return result;
    }

    DWORD changeTime = 0;
    DWORD size = sizeof(changeTime);
    RegGetValueW(modKey, nullptr, L"SettingsChangeTime", RRF_RT_REG_DWORD,
                 nullptr, &changeTime, &size);

    std::vector<std::wstring> existingNames;
    HKEY settingsKey;
    if (RegOpenKeyExW(modKey, L"Settings", 0, KEY_READ, &settingsKey) ==
        ERROR_SUCCESS) {
        existingNames = ReadRegistryValueNames(settingsKey);
        RegCloseKey(settingsKey);
    }
    RegCloseKey(modKey);

    std::vector<std::wstring> staleNames;
    for (auto const& name : existingNames) {
        bool used = std::any_of(
            entries.begin(), entries.end(), [&name](RegEntry const& entry) {
                return _wcsicmp(entry.name.c_str(), name.c_str()) == 0;
            });
        if (!used)
            staleNames.push_back(name);
    }

    DWORD newChangeTime = std::max((DWORD)time(nullptr), changeTime + 1);

    LSTATUS status =
        WriteSettingsToRegistry(entries, staleNames, newChangeTime);
    if (status == ERROR_SUCCESS) {
        result.succeeded = true;
        return result;
    }
    if (status != ERROR_ACCESS_DENIED) {
        result.error = L"Writing the settings failed (error " +
                       std::to_wstring(status) + L").";
        return result;
    }

    WCHAR tempDir[MAX_PATH];
    if (!GetTempPathW(ARRAYSIZE(tempDir), tempDir)) {
        result.error = L"The temporary folder wasn't found.";
        return result;
    }
    std::wstring regFilePath =
        JoinPath(tempDir, L"windhawk-explorer-command-bar-" +
                              std::to_wstring(GetTickCount64()) + L".reg");
    if (!WriteUtf16File(regFilePath,
                        BuildRegFile(entries, staleNames, newChangeTime))) {
        result.error = L"Couldn't write " + regFilePath + L".";
        return result;
    }

    DWORD error = RunElevatedRegImport(regFilePath, hOwnerWnd);
    DeleteFileW(regFilePath.c_str());
    if (error == ERROR_SUCCESS) {
        result.succeeded = true;
    } else if (error == ERROR_CANCELLED) {
        result.error =
            L"The administrator prompt was canceled. Windhawk keeps mod "
            L"settings in a protected registry key, so saving them needs "
            L"administrator approval.";
    } else {
        result.error = L"Importing the settings failed (error " +
                       std::to_wstring(error) + L").";
    }
    return result;
}

struct SaveJob {
    std::vector<RegEntry> entries;
    EditableSettings settings;
    SaveResult result;
};

ActionItem MakeNewActionItem(bool isMenu, int depth) {
    ActionItem item;
    item.isMenu = isMenu && depth < kMaxMenuDepth;
    if (depth == 0) {
        item.name = item.isMenu ? L"New menu" : L"New button";
        item.icon = item.isMenu ? L"E81E" : L"E8A7";
    } else {
        item.name = item.isMenu ? L"New submenu" : L"New entry";
    }
    if (item.isMenu)
        item.subItems.push_back(MakeNewActionItem(false, depth + 1));
    return item;
}

PCWSTR ItemKindName(ActionItem const& item, int depth) {
    bool isMenu = item.isMenu && depth < kMaxMenuDepth;
    if (depth == 0)
        return isMenu ? L"Dropdown menu" : L"Toolbar button";
    if (depth == 1)
        return isMenu ? L"Submenu" : L"Menu entry";
    return L"Submenu entry";
}

std::wstring DescribeActionItem(ActionItem const& item, int depth) {
    std::wstring text;
    if (item.isMenu && depth < kMaxMenuDepth) {
        size_t count = item.subItems.size();
        text = depth == 0 ? L"Dropdown menu" : L"Submenu";
        text += L" \u00B7 " + std::to_wstring(count) +
                (count == 1 ? L" entry" : L" entries");
    } else if (!item.command.empty()) {
        text = item.command;
        if (!item.parameters.empty())
            text += L" " + item.parameters;
        if (!item.shortcut.empty())
            text += L" · " + item.shortcut;
    } else {
        text = L"No command";
    }
    if (item.separatorAfter)
        text += L" \u00B7 separator after";
    return text;
}

winrt::Windows::Foundation::IInspectable BoxString(std::wstring_view text) {
    return winrt::box_value(winrt::hstring{text});
}

mux::GridLength StarLength(double value = 1) {
    return {value, mux::GridUnitType::Star};
}

mux::GridLength AutoLength() {
    return {1, mux::GridUnitType::Auto};
}

mux::GridLength PixelLength(double value) {
    return {value, mux::GridUnitType::Pixel};
}

void AddGridColumn(muxc::Grid const& grid, mux::GridLength length) {
    muxc::ColumnDefinition column;
    column.Width(length);
    grid.ColumnDefinitions().Append(column);
}

void AddGridRow(muxc::Grid const& grid, mux::GridLength length) {
    muxc::RowDefinition row;
    row.Height(length);
    grid.RowDefinitions().Append(row);
}

muxc::FontIcon MakeGlyph(PCWSTR glyph, double size) {
    muxc::FontIcon icon;
    icon.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(glyph);
    icon.FontSize(size);
    return icon;
}

muxc::TextBox MakeTextBox(std::wstring const& text, PCWSTR placeholder) {
    muxc::TextBox textBox;
    textBox.Text(text);
    if (placeholder)
        textBox.PlaceholderText(placeholder);
    return textBox;
}

muxc::Button MakeTextButton(PCWSTR text) {
    muxc::Button button;
    button.Content(BoxString(text));
    return button;
}

muxc::IconElement MakeItemIcon(ActionItem const& item, int depth, double size) {
    muxc::IconElement icon{nullptr};
    if (!item.hideIcon) {
        try {
            icon = TryCreateIconElement(item.icon, item.command);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
    if (!icon) {
        PCWSTR glyph = InternalCommandGlyph(item.command);
        if (item.isMenu && depth < kMaxMenuDepth)
            glyph = L"\uE8FD";
        else if (!glyph)
            glyph = L"\uE7C3";
        auto fontIcon = MakeGlyph(glyph, size);
        if (item.hideIcon)
            fontIcon.Opacity(0.4);
        icon = fontIcon;
    }
    icon.Width(size);
    icon.Height(size);
    return icon;
}

std::wstring BrowseForFile(HWND hOwnerWnd,
                           PCWSTR title,
                           COMDLG_FILTERSPEC const* filters,
                           UINT filterCount) {
    winrt::com_ptr<IFileOpenDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dialog.put())))) {
        return std::wstring();
    }
    dialog->SetTitle(title);
    dialog->SetFileTypes(filterCount, filters);
    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST |
                       FOS_NOCHANGEDIR);
    if (FAILED(dialog->Show(hOwnerWnd)))
        return std::wstring();

    winrt::com_ptr<IShellItem> result;
    PWSTR path = nullptr;
    if (FAILED(dialog->GetResult(result.put())) ||
        FAILED(result->GetDisplayName(SIGDN_FILESYSPATH, &path)) || !path) {
        return std::wstring();
    }
    std::wstring resultPath = path;
    CoTaskMemFree(path);
    return resultPath;
}

std::wstring BrowseForSaveFile(HWND hOwnerWnd,
                               PCWSTR title,
                               COMDLG_FILTERSPEC const* filters,
                               UINT filterCount,
                               PCWSTR defaultExtension,
                               PCWSTR defaultFileName) {
    winrt::com_ptr<IFileSaveDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileSaveDialog, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dialog.put())))) {
        return std::wstring();
    }
    dialog->SetTitle(title);
    dialog->SetFileTypes(filterCount, filters);
    dialog->SetDefaultExtension(defaultExtension);
    dialog->SetFileName(defaultFileName);
    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_OVERWRITEPROMPT |
                       FOS_NOCHANGEDIR);
    if (FAILED(dialog->Show(hOwnerWnd)))
        return std::wstring();

    winrt::com_ptr<IShellItem> result;
    PWSTR path = nullptr;
    if (FAILED(dialog->GetResult(result.put())) ||
        FAILED(result->GetDisplayName(SIGDN_FILESYSPATH, &path)) || !path) {
        return std::wstring();
    }
    std::wstring resultPath = path;
    CoTaskMemFree(path);
    return resultPath;
}

bool WriteUtf8File(std::wstring const& path, std::wstring const& text) {
    int length = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(),
                                     nullptr, 0, nullptr, nullptr);
    std::string bytes((size_t)std::max(length, 0), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), bytes.data(),
                        length, nullptr, nullptr);

    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;
    DWORD written = 0;
    bool succeeded = WriteFile(hFile, bytes.data(), (DWORD)bytes.size(),
                               &written, nullptr) &&
                     written == bytes.size();
    CloseHandle(hFile);
    return succeeded;
}

// Exported settings are YAML laid out like the mod settings in Windhawk. The
// reader handles the subset the writer produces: nested maps, "- " lists,
// plain, 'single' and "double" quoted scalars, and comments. Both convert
// from and to the flat keys Windhawk stores settings under, e.g.
// items[0].subItems[1].name.
using FlatSettings = std::unordered_map<std::wstring, std::wstring>;

struct YamlNode {
    enum class Kind {
        Scalar,
        Map,
        List,
    };
    Kind kind = Kind::Map;
    std::wstring text;
    std::vector<std::wstring> keys;
    std::vector<YamlNode> values;
};

bool YamlStringNeedsQuotes(std::wstring const& value) {
    if (value.empty() || value.front() == L' ' || value.back() == L' ' ||
        value.back() == L':')
        return true;
    if (wcschr(L"-?:,[]{}#&*!|>'\"%@`+.", value.front()) ||
        iswdigit(value.front()))
        return true;
    if (value.find(L": ") != std::wstring::npos ||
        value.find(L" #") != std::wstring::npos)
        return true;
    for (WCHAR c : value) {
        if (c < 0x20)
            return true;
    }
    std::wstring lower = ToLower(value);
    for (PCWSTR word : {L"true", L"false", L"yes", L"no", L"on", L"off", L"y",
                        L"n", L"null", L"~"}) {
        if (lower == word)
            return true;
    }
    return false;
}

std::wstring FormatYamlString(std::wstring const& value) {
    if (!YamlStringNeedsQuotes(value))
        return value;
    // Single quotes keep backslashes as they are, handy for paths.
    if (value.find(L'\'') == std::wstring::npos)
        return L"'" + value + L"'";
    std::wstring result = L"\"";
    for (WCHAR c : value) {
        if (c == L'\\' || c == L'"')
            result += L'\\';
        result += c;
    }
    return result + L"\"";
}

YamlNode* YamlMapChild(YamlNode* map,
                       std::wstring const& key,
                       YamlNode::Kind kind) {
    for (size_t i = 0; i < map->keys.size(); i++) {
        if (map->keys[i] == key)
            return &map->values[i];
    }
    map->keys.push_back(key);
    YamlNode child;
    child.kind = kind;
    map->values.push_back(std::move(child));
    return &map->values.back();
}

void EmitYamlMap(YamlNode const& map,
                 size_t indent,
                 bool listItem,
                 std::wstring* out) {
    for (size_t i = 0; i < map.keys.size(); i++) {
        YamlNode const& value = map.values[i];
        if (indent == 0 && i > 0 && value.kind != YamlNode::Kind::Scalar)
            *out += L"\n";
        if (listItem && i == 0)
            *out += std::wstring(indent - 2, L' ') + L"- ";
        else
            *out += std::wstring(indent, L' ');
        *out += map.keys[i] + L":";
        switch (value.kind) {
            case YamlNode::Kind::Scalar:
                *out += L" " + value.text + L"\n";
                break;
            case YamlNode::Kind::Map:
                *out += L"\n";
                EmitYamlMap(value, indent + 2, false, out);
                break;
            case YamlNode::Kind::List:
                if (value.values.empty()) {
                    *out += L" []\n";
                    break;
                }
                *out += L"\n";
                for (auto const& element : value.values)
                    EmitYamlMap(element, indent + 4, true, out);
                break;
        }
    }
}

std::wstring BuildSettingsYaml(std::vector<RegEntry> const& entries) {
    YamlNode root;
    for (auto const& entry : entries) {
        YamlNode* node = &root;
        size_t start = 0;
        while (true) {
            size_t dot = entry.name.find(L'.', start);
            bool last = dot == std::wstring::npos;
            std::wstring segment = entry.name.substr(
                start, last ? std::wstring::npos : dot - start);
            size_t bracket = segment.find(L'[');
            if (bracket != std::wstring::npos) {
                size_t index =
                    wcstoul(segment.c_str() + bracket + 1, nullptr, 10);
                YamlNode* list = YamlMapChild(node, segment.substr(0, bracket),
                                              YamlNode::Kind::List);
                while (list->values.size() <= index)
                    list->values.push_back(YamlNode{});
                node = &list->values[index];
            } else if (last) {
                YamlNode* leaf =
                    YamlMapChild(node, segment, YamlNode::Kind::Scalar);
                if (entry.isString)
                    leaf->text = FormatYamlString(entry.text);
                else if (entry.isBool)
                    leaf->text = entry.number ? L"true" : L"false";
                else
                    leaf->text = std::to_wstring((int)entry.number);
            } else {
                node = YamlMapChild(node, segment, YamlNode::Kind::Map);
            }
            if (last)
                break;
            start = dot + 1;
        }
    }

    std::wstring yaml = L"# Explorer Command Bar settings";
#ifdef WH_MOD_VERSION
    yaml += L" (v" WH_MOD_VERSION L")";
#endif
    yaml +=
        L"\n# Import them with the Import button of the mod's settings "
        L"window.\n\n";
    EmitYamlMap(root, 0, false, &yaml);
    return yaml;
}

std::wstring TrimSpaces(std::wstring const& text) {
    size_t first = text.find_first_not_of(L" \t");
    if (first == std::wstring::npos)
        return std::wstring();
    size_t last = text.find_last_not_of(L" \t");
    return text.substr(first, last - first + 1);
}

// Finds the first `target` character outside of quotes. A '#' only
// counts at the start or after a space, a ':' only before a space or at the
// end, as in YAML.
size_t FindYamlUnquoted(std::wstring const& text, WCHAR target) {
    bool inSingle = false;
    bool inDouble = false;
    for (size_t i = 0; i < text.size(); i++) {
        WCHAR c = text[i];
        if (inDouble) {
            if (c == L'\\')
                i++;
            else if (c == L'"')
                inDouble = false;
            continue;
        }
        if (inSingle) {
            if (c == L'\'')
                inSingle = false;
            continue;
        }
        if (c == L'"' && (i == 0 || text[i - 1] == L' ')) {
            inDouble = true;
        } else if (c == L'\'' && (i == 0 || text[i - 1] == L' ')) {
            inSingle = true;
        } else if (c == target) {
            if (target == L'#' && (i == 0 || text[i - 1] == L' '))
                return i;
            if (target == L':' && (i + 1 == text.size() || text[i + 1] == L' '))
                return i;
        }
    }
    return std::wstring::npos;
}

bool ParseYamlScalar(std::wstring const& text, std::wstring* value) {
    value->clear();
    if (text.empty())
        return true;

    if (text[0] == L'\'') {
        size_t i = 1;
        for (; i < text.size(); i++) {
            if (text[i] == L'\'') {
                if (i + 1 < text.size() && text[i + 1] == L'\'') {
                    *value += L'\'';
                    i++;
                    continue;
                }
                break;
            }
            *value += text[i];
        }
        return i < text.size() && TrimSpaces(text.substr(i + 1)).empty();
    }

    if (text[0] == L'"') {
        size_t i = 1;
        for (; i < text.size() && text[i] != L'"'; i++) {
            if (text[i] != L'\\') {
                *value += text[i];
                continue;
            }
            if (++i >= text.size())
                return false;
            switch (text[i]) {
                case L'n':
                    *value += L'\n';
                    break;
                case L't':
                    *value += L'\t';
                    break;
                case L'u':
                    if (i + 4 >= text.size())
                        return false;
                    *value += (WCHAR)wcstoul(text.substr(i + 1, 4).c_str(),
                                             nullptr, 16);
                    i += 4;
                    break;
                default:
                    *value += text[i];
                    break;
            }
        }
        return i < text.size() && TrimSpaces(text.substr(i + 1)).empty();
    }

    *value = text;
    return true;
}

struct YamlLine {
    size_t indent;
    std::wstring text;
    int number;
};

class YamlSettingsReader {
   public:
    bool Read(std::wstring const& yaml,
              FlatSettings* settings,
              std::wstring* error) {
        m_settings = settings;
        m_error = error;

        int number = 0;
        size_t start = 0;
        while (start <= yaml.size()) {
            size_t end = yaml.find(L'\n', start);
            if (end == std::wstring::npos)
                end = yaml.size();
            std::wstring line = yaml.substr(start, end - start);
            start = end + 1;
            number++;
            if (!line.empty() && line.back() == L'\r')
                line.pop_back();

            size_t indent = line.find_first_not_of(L' ');
            if (indent == std::wstring::npos)
                continue;
            if (line[indent] == L'\t') {
                *m_error = L"Line " + std::to_wstring(number) +
                           L": tabs can't be used for indentation.";
                return false;
            }
            size_t comment = FindYamlUnquoted(line, L'#');
            if (comment != std::wstring::npos)
                line.resize(comment);
            std::wstring text = TrimSpaces(line);
            if (text.empty() || text == L"---" || text == L"...")
                continue;
            m_lines.push_back({indent, text, number});
        }

        if (m_lines.empty())
            return true;
        if (!ParseBlock(std::wstring()))
            return false;
        if (m_pos < m_lines.size())
            return Fail(L"unexpected indentation.");
        return true;
    }

   private:
    static bool IsListLine(YamlLine const& line) {
        return line.text == L"-" || line.text.starts_with(L"- ");
    }

    bool Fail(std::wstring const& message) {
        int number = m_lines[std::min(m_pos, m_lines.size() - 1)].number;
        *m_error = L"Line " + std::to_wstring(number) + L": " + message;
        return false;
    }

    bool SetScalar(std::wstring const& path, std::wstring const& text) {
        std::wstring value;
        if (!ParseYamlScalar(text, &value))
            return Fail(L"the quoted value isn't closed.");
        (*m_settings)[path] = value;
        return true;
    }

    bool ParseBlock(std::wstring const& path) {
        YamlLine const& line = m_lines[m_pos];
        return IsListLine(line) ? ParseList(line.indent, path)
                                : ParseMap(line.indent, path);
    }

    bool ParseMap(size_t indent, std::wstring const& path) {
        while (m_pos < m_lines.size()) {
            YamlLine const& line = m_lines[m_pos];
            if (line.indent < indent || IsListLine(line))
                return true;
            if (line.indent > indent)
                return Fail(L"unexpected indentation.");

            size_t colon = FindYamlUnquoted(line.text, L':');
            if (colon == std::wstring::npos)
                return Fail(L"expected \"name: value\".");
            std::wstring key = TrimSpaces(line.text.substr(0, colon));
            std::wstring rest = TrimSpaces(line.text.substr(colon + 1));
            std::wstring childPath = path.empty() ? key : path + L"." + key;
            m_pos++;

            if (!rest.empty()) {
                if (rest != L"[]" && rest != L"{}" &&
                    !SetScalar(childPath, rest))
                    return false;
                continue;
            }

            if (m_pos < m_lines.size()) {
                YamlLine const& next = m_lines[m_pos];
                if (next.indent > indent ||
                    (next.indent == indent && IsListLine(next))) {
                    if (!ParseBlock(childPath))
                        return false;
                }
            }
        }
        return true;
    }

    bool ParseList(size_t indent, std::wstring const& path) {
        int index = 0;
        while (m_pos < m_lines.size()) {
            YamlLine& line = m_lines[m_pos];
            if (line.indent > indent)
                return Fail(L"unexpected indentation.");
            if (line.indent < indent || !IsListLine(line))
                return true;

            std::wstring itemPath =
                path + L"[" + std::to_wstring(index++) + L"]";
            std::wstring rest = line.text.substr(1);
            size_t spaces = rest.find_first_not_of(L' ');
            if (spaces == std::wstring::npos) {
                m_pos++;
                if (m_pos < m_lines.size() && m_lines[m_pos].indent > indent &&
                    !ParseBlock(itemPath))
                    return false;
                continue;
            }

            // "- name: value" continues as a map at the column of "name".
            line.indent = indent + 1 + spaces;
            line.text = rest.substr(spaces);
            if (!IsListLine(line) &&
                FindYamlUnquoted(line.text, L':') == std::wstring::npos) {
                if (!SetScalar(itemPath, line.text))
                    return false;
                m_pos++;
                continue;
            }
            if (!ParseBlock(itemPath))
                return false;
        }
        return true;
    }

    std::vector<YamlLine> m_lines;
    size_t m_pos = 0;
    FlatSettings* m_settings = nullptr;
    std::wstring* m_error = nullptr;
};

// Exported JSON is flat, one Windhawk key per line. The reader also takes
// nested objects and arrays, laid out like the YAML.
std::wstring BuildSettingsJson(std::vector<RegEntry> const& entries) {
    std::wstring json = L"{\n";
    for (size_t i = 0; i < entries.size(); i++) {
        auto const& entry = entries[i];
        json += L"  ";
        json += wdj::JsonValue::CreateStringValue(entry.name).Stringify();
        json += L": ";
        if (entry.isString)
            json += wdj::JsonValue::CreateStringValue(entry.text).Stringify();
        else if (entry.isBool)
            json += entry.number ? L"true" : L"false";
        else
            json += std::to_wstring((int)entry.number);
        json += i + 1 < entries.size() ? L",\n" : L"\n";
    }
    return json + L"}\n";
}

void FlattenJsonSettings(wdj::IJsonValue const& value,
                         std::wstring const& path,
                         FlatSettings* settings) {
    switch (value.ValueType()) {
        case wdj::JsonValueType::Object:
            for (auto const& pair : value.GetObject()) {
                std::wstring key{pair.Key()};
                FlattenJsonSettings(pair.Value(),
                                    path.empty() ? key : path + L"." + key,
                                    settings);
            }
            break;
        case wdj::JsonValueType::Array: {
            auto array = value.GetArray();
            for (uint32_t i = 0; i < array.Size(); i++) {
                FlattenJsonSettings(array.GetAt(i),
                                    path + L"[" + std::to_wstring(i) + L"]",
                                    settings);
            }
            break;
        }
        case wdj::JsonValueType::String:
            (*settings)[path] = std::wstring(value.GetString());
            break;
        case wdj::JsonValueType::Number:
            (*settings)[path] =
                std::to_wstring((long long)std::llround(value.GetNumber()));
            break;
        case wdj::JsonValueType::Boolean:
            (*settings)[path] = value.GetBoolean() ? L"true" : L"false";
            break;
        case wdj::JsonValueType::Null:
            (*settings)[path] = std::wstring();
            break;
    }
}

// JSON by the .json extension, YAML by .yaml or .yml, and for other names
// JSON if the content starts with an object.
bool IsJsonSettingsFile(std::wstring const& path, std::wstring const& text) {
    std::wstring extension = ToLower(PathFindExtensionW(path.c_str()));
    if (extension == L".json")
        return true;
    if (extension == L".yaml" || extension == L".yml")
        return false;
    size_t first = text.find_first_not_of(L" \t\r\n");
    return first != std::wstring::npos && text[first] == L'{';
}

std::wstring FlatSettingString(FlatSettings const& settings,
                               std::wstring const& key) {
    auto it = settings.find(key);
    return it != settings.end() ? it->second : std::wstring();
}

int FlatSettingInt(FlatSettings const& settings,
                   std::wstring const& key,
                   int fallback) {
    auto it = settings.find(key);
    if (it == settings.end())
        return fallback;
    std::wstring text = ToLower(TrimSpaces(it->second));
    if (text == L"true" || text == L"yes" || text == L"on")
        return 1;
    if (text == L"false" || text == L"no" || text == L"off")
        return 0;
    WCHAR* end = nullptr;
    long value = wcstol(text.c_str(), &end, 10);
    if (text.empty() || *end)
        return fallback;
    return (int)value;
}

// Mirrors LoadActionItem.
ActionItem ImportActionItem(FlatSettings const& root,
                            std::wstring const& prefix,
                            int depth,
                            bool* isEmpty) {
    ActionItem item;
    item.name = FlatSettingString(root, prefix + L".name");
    item.command = FlatSettingString(root, prefix + L".command");
    item.parameters = FlatSettingString(root, prefix + L".parameters");
    item.shortcut = FlatSettingString(root, prefix + L".shortcut");
    item.matchExtensions =
        FlatSettingString(root, prefix + L".matchExtensions");
    item.icon = FlatSettingString(root, prefix + L".iconGlyph");
    item.hideIcon = FlatSettingInt(root, prefix + L".hideIcon", 0) != 0;
    item.showLabel = FlatSettingInt(root, prefix + L".showLabel", 0) != 0;
    item.labelText = FlatSettingString(root, prefix + L".labelText");
    item.separatorAfter =
        FlatSettingInt(root, prefix + L".separatorAfter", 0) != 0;
    item.runAsAdmin = FlatSettingInt(root, prefix + L".runAsAdmin", 0) != 0;
    item.windowStyle = FlatSettingString(root, prefix + L".windowStyle");

    if (depth < kMaxMenuDepth) {
        item.isMenu = FlatSettingString(root, prefix + L".type") == L"menu";
        for (int i = 0; i < 100; i++) {
            bool subEmpty = false;
            ActionItem subItem = ImportActionItem(
                root, prefix + L".subItems[" + std::to_wstring(i) + L"]",
                depth + 1, &subEmpty);
            if (subEmpty)
                break;
            item.subItems.push_back(std::move(subItem));
        }
    }

    *isEmpty =
        item.name.empty() && item.command.empty() && item.subItems.empty();
    return item;
}

// Reads exported settings over the given ones. The toolbar items are
// replaced, other settings missing from the file keep their values.
bool ImportSettingsText(std::wstring const& text,
                        bool isJson,
                        EditableSettings* settings,
                        std::wstring* error) {
    FlatSettings root;
    if (isJson) {
        wdj::JsonValue value{nullptr};
        if (!wdj::JsonValue::TryParse(text, value) ||
            value.ValueType() != wdj::JsonValueType::Object) {
            *error = L"The file isn't a valid JSON object.";
            return false;
        }
        FlattenJsonSettings(value, std::wstring(), &root);
    } else {
        std::wstring parseError;
        if (!YamlSettingsReader().Read(text, &root, &parseError)) {
            *error = L"The file isn't valid YAML. " + parseError;
            return false;
        }
    }

    int recognized = 0;
    for (auto const& page : DescribeSettingsPages(*settings)) {
        for (auto const& field : page.fields) {
            if (field.kind == SettingKind::Header || !root.count(field.key))
                continue;
            recognized++;
            switch (field.kind) {
                case SettingKind::Bool: {
                    auto* value = (bool*)field.value;
                    *value = FlatSettingInt(root, field.key, *value) != 0;
                    break;
                }
                case SettingKind::Int: {
                    auto* value = (int*)field.value;
                    *value = std::max(field.minimum,
                                      FlatSettingInt(root, field.key, *value));
                    break;
                }
                case SettingKind::String:
                    *(std::wstring*)field.value =
                        FlatSettingString(root, field.key);
                    break;
                case SettingKind::Header:
                    break;
            }
        }
    }

    if (root.count(L"defaultButtonOrder")) {
        recognized++;
        settings->defaultButtonOrder =
            FlatSettingString(root, L"defaultButtonOrder");
    }

    std::vector<ActionItem> items;
    for (int i = 0; i < 100; i++) {
        bool isEmpty = false;
        ActionItem item = ImportActionItem(
            root, L"items[" + std::to_wstring(i) + L"]", 0, &isEmpty);
        if (isEmpty)
            break;
        items.push_back(std::move(item));
    }

    if (recognized == 0 && items.empty()) {
        *error = L"The file doesn't contain Explorer Command Bar settings.";
        return false;
    }
    settings->items = std::move(items);
    return true;
}

constexpr UINT kSettingsWindowCloseMsg = WM_APP + 1;
constexpr PCWSTR kDonateUrl = L"https://buymeacoffee.com/widget/page/danrotaru";
constexpr PCWSTR kGlyphListUrl =
    L"https://learn.microsoft.com/en-us/windows/apps/design/iconography/"
    L"segoe-fluent-icons-font";

void OpenUrl(std::wstring url) {
    RunShellWorkOnWorkerThread([url = std::move(url)]() {
        ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr,
                      SW_SHOWNORMAL);
    });
}

class SettingsWindow;

// Only accessed on the thread of the settings window.
SettingsWindow* g_settingsWindow;

std::mutex g_settingsWindowMutex;
HWND g_settingsWindowHwnd;
std::atomic<bool> g_settingsWindowCreating;

class SettingsWindow {
   public:
    ~SettingsWindow() { Shutdown(); }

    bool Create(HWND hHintWnd);
    HWND Hwnd() const { return m_hwnd; }
    void Shutdown();
    LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSaveCompleted(SaveJob const& job);

   private:
    void LoadStyles();
    mux::Style FindStyle(PCWSTR key);
    mux::UIElement BuildRoot();
    void ApplyTheme();
    void ShowPage(std::wstring const& tag);
    void NavigateTo(std::wstring const& tag);

    muxc::TextBlock MakeText(std::wstring const& text, mux::Style const& style);
    muxc::StackPanel MakePageHeader(std::wstring const& title,
                                    std::wstring const& description);
    muxc::Border MakeCard(PCWSTR glyph,
                          std::wstring const& title,
                          std::wstring const& description,
                          mux::FrameworkElement const& control,
                          bool wide);
    mux::FrameworkElement MakeFieldControl(SettingField const& field);
    mux::FrameworkElement BuildSettingsPage(SettingsPageDef const& page);

    mux::FrameworkElement BuildItemsPage();
    muxc::CommandBar BuildItemToolbar();
    void RefreshItemsPage();
    void RebuildPreview();
    void UpdatePreviewSelection();
    muxc::AppBarButton MakePreviewButton(muxc::IconElement const& icon,
                                         std::wstring const& label,
                                         bool showLabel,
                                         bool chevron,
                                         std::wstring const& pageTag);
    muxc::AppBarToggleButton MakePreviewItem(ActionItem const& item, int index);
    void AppendPreviewMenuEntries(
        std::vector<ActionItem> const& items,
        std::vector<int> const& parentPath,
        wfc::IVector<muxc::MenuFlyoutItemBase> const& target);
    void SelectItem(std::vector<int> const& path);
    muxc::AppBarToggleButton MakePreviewBuiltIn(int id,
                                                muxc::IconElement const& icon,
                                                std::wstring const& label,
                                                bool showLabel,
                                                bool chevron);
    void SelectBuiltIn(int id);
    bool* BuiltInHiddenField(int id);
    void BuildBuiltInEditor();
    void BuildSettingsItemEditor();
    void BuildItemEditor();
    void UpdateItemToolbar();
    void DeferItemsPageRefresh();
    void OnItemEdited();

    std::vector<ActionItem>* SiblingsOf(std::vector<int> const& path);
    std::vector<ActionItem>* ChildrenOf(std::vector<int> const& path);
    ActionItem* ItemAt(std::vector<int> const& path);
    ActionItem* EditedItem(std::vector<int> const& path, int generation);

    // Reordering by dragging, for the toolbar items of the preview and for
    // the entries of a menu. A group lists the elements which can be dragged
    // among each other, in their order.
    enum DragGroup {
        kDragGroupPreview,
        kDragGroupEntries,
        kDragGroupBuiltIns,
        kDragGroupCount
    };
    void SetDragGroup(int group,
                      mux::UIElement const& reference,
                      std::vector<mux::UIElement> elements,
                      std::function<void(int from, int to)> commit);
    void ClearDragGroup(int group);
    void EnableDragReorder(mux::UIElement const& element, int group, int slot);
    void OnDragPointerPressed(mux::UIElement const& element,
                              mux::Input::PointerRoutedEventArgs const& args,
                              int group,
                              int slot);
    void OnDragPointerMoved(mux::UIElement const& element,
                            mux::Input::PointerRoutedEventArgs const& args,
                            int group,
                            int slot);
    bool BeginDrag(mux::UIElement const& element,
                   mux::Input::PointerRoutedEventArgs const& args);
    void UpdateDrag(double delta);
    void EndDrag(bool commit);
    void ResetDragVisuals(int group);
    bool IsDragClick() const { return m_drag.active || m_suppressDragClick; }
    void ReorderToolbarItems(std::vector<int> order, int from, int to);
    void ReorderBuiltInButtons(std::vector<size_t> const& slots,
                               int from,
                               int to);
    void ReorderMenuEntries(std::vector<int> const& parentPath,
                            int from,
                            int to);

    void AddTopLevelItem(bool isMenu);
    void AddMenuEntry(bool isMenu);
    void MoveSelectedItem(int delta);
    void DuplicateSelectedItem();
    void DeleteSelectedItem();

    void OnModelChanged();
    void UpdateFooter();
    void ShowMessage(muxc::InfoBarSeverity severity,
                     std::wstring const& title,
                     std::wstring const& message);
    void Save(bool closeAfter);
    void Discard();
    void RequestClose();
    void ShowUnsavedChangesDialog();
    void ForceClose();
    void CloseXaml();
    void ExportSettings();
    void ImportSettings();
    std::wstring Browse(PCWSTR title,
                        COMDLG_FILTERSPEC const* filters,
                        UINT filterCount);

    HWND m_hwnd = nullptr;
    HWND m_hostWnd = nullptr;
    HICON m_hIconLarge = nullptr;
    HICON m_hIconSmall = nullptr;
    muxh::DesktopWindowXamlSource m_source{nullptr};
    mux::ResourceDictionary m_resources{nullptr};
    mux::Style m_cardStyle{nullptr};
    mux::Style m_secondaryTextStyle{nullptr};
    mux::Style m_sectionHeaderStyle{nullptr};
    mux::Style m_pageTitleStyle{nullptr};
    muxc::Grid m_root{nullptr};
    muxc::Grid m_pageHost{nullptr};
    muxc::InfoBar m_infoBar{nullptr};
    muxc::TextBlock m_statusText{nullptr};
    muxc::ProgressRing m_savingRing{nullptr};
    muxc::Button m_saveButton{nullptr};
    muxc::Button m_discardButton{nullptr};
    muxc::ContentDialog m_dialog{nullptr};

    std::wstring m_currentPage;
    EditableSettings m_model;
    EditableSettings m_saved;
    std::vector<RegEntry> m_savedEntries;
    bool m_dirty = false;
    bool m_saving = false;
    bool m_closeAfterSave = false;
    int m_modalDepth = 0;

    std::vector<std::pair<std::wstring, muxc::RadioButton>> m_navButtons;

    std::vector<int> m_selectedPath;
    muxc::Border m_previewHost{nullptr};
    std::vector<muxc::AppBarToggleButton> m_previewItems;
    std::vector<std::pair<int, muxc::AppBarToggleButton>> m_previewBuiltIns;
    int m_selectedBuiltIn = -1;
    muxc::StackPanel m_editorPanel{nullptr};
    muxc::Border m_editorIconPreview{nullptr};
    muxc::AppBarButton m_moveUpButton{nullptr};
    muxc::AppBarButton m_moveDownButton{nullptr};
    muxc::AppBarButton m_duplicateButton{nullptr};
    muxc::AppBarButton m_deleteButton{nullptr};
    muxc::MenuFlyoutItem m_addEntryItem{nullptr};
    muxc::MenuFlyoutItem m_addSubmenuItem{nullptr};
    int m_editorGeneration = 0;

    struct DragGroupState {
        mux::UIElement reference{nullptr};
        std::vector<mux::UIElement> elements;
        std::function<void(int, int)> commit;
    };
    struct DragState {
        bool pressed = false;
        bool active = false;
        int group = -1;
        int slot = -1;
        int target = -1;
        uint32_t pointerId = 0;
        wf::Point pressPoint{};
        std::vector<double> starts;
        std::vector<double> sizes;
    };
    DragGroupState m_dragGroups[kDragGroupCount];
    DragState m_drag;
    bool m_suppressDragClick = false;
};

std::wstring SettingsWindowClassName() {
    return std::wstring(L"WindhawkSettingsWindow_") + WH_MOD_ID;
}

LRESULT CALLBACK SettingsWindowProc(HWND hWnd,
                                    UINT uMsg,
                                    WPARAM wParam,
                                    LPARAM lParam) {
    if (uMsg == WM_NCCREATE) {
        auto* createStruct = (CREATESTRUCTW*)lParam;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA,
                          (LONG_PTR)createStruct->lpCreateParams);
    }
    auto* window = (SettingsWindow*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    if (!window)
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);

    LRESULT result;
    try {
        result = window->HandleMessage(hWnd, uMsg, wParam, lParam);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
        result = DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    // The window object lives as long as its window.
    if (uMsg == WM_NCDESTROY) {
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
        delete window;
    }
    return result;
}

// Closes the settings window before the File Explorer window whose thread it
// runs on goes away.
LRESULT CALLBACK SettingsHostSubclassProc(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          UINT_PTR subclassId,
                                          DWORD_PTR refData) {
    if (uMsg == WM_DESTROY) {
        RemoveWindowSubclass(hWnd, SettingsHostSubclassProc, subclassId);
        HWND hSettingsWnd = (HWND)refData;
        if (IsWindow(hSettingsWnd))
            DestroyWindow(hSettingsWnd);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// The step the settings window thread is at, for error reports.
PCWSTR g_settingsWindowStep = L"";

void SetSettingsWindowStep(PCWSTR step) {
    g_settingsWindowStep = step;
    Wh_Log(L"Settings window: %s", step);
}

// Must be called from a catch block.
std::wstring DescribeCurrentException() {
    WCHAR buffer[64];
    try {
        throw;
    } catch (winrt::hresult_error const& e) {
        swprintf(buffer, ARRAYSIZE(buffer), L"%08X", (DWORD)e.code().value);
        return std::wstring(buffer) + L": " + std::wstring(e.message());
    } catch (std::exception const& e) {
        std::string what = e.what();
        return std::wstring(what.begin(), what.end());
    } catch (...) {
        swprintf(buffer, ARRAYSIZE(buffer), L"%08X",
                 (DWORD)winrt::to_hresult().value);
        return buffer;
    }
}

void ReportSettingsWindowError(std::wstring const& error) {
    std::wstring text = L"The settings window couldn't be opened.\n\nStep: ";
    text += g_settingsWindowStep;
    text += L"\nError: " + error;
    Wh_Log(L"%s", text.c_str());
    MessageBoxW(nullptr, text.c_str(), L"Explorer Command Bar Settings",
                MB_ICONERROR | MB_SETFOREGROUND);
}

bool SettingsWindow::Create(HWND hHintWnd) {
    SetSettingsWindowStep(L"reading the settings");
    m_model = SnapshotSettings();
    m_saved = m_model;
    m_savedEntries = BuildRegEntries(m_model);

    HMONITOR monitor = MonitorFromWindow(hHintWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    GetMonitorInfoW(monitor, &monitorInfo);
    RECT work = monitorInfo.rcWork;

    SetSettingsWindowStep(L"creating the window");
    m_hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP, SettingsWindowClassName().c_str(),
        L"Explorer Command Bar Settings", WS_OVERLAPPEDWINDOW, work.left,
        work.top, 800, 600, nullptr, nullptr, GetCurrentModuleHandle(), this);
    if (!m_hwnd) {
        ReportSettingsWindowError(L"CreateWindowEx failed with error " +
                                  std::to_wstring(GetLastError()));
        return false;
    }

    UINT dpi = GetDpiForWindow(m_hwnd);
    if (!dpi)
        dpi = 96;
    int workWidth = work.right - work.left;
    int workHeight = work.bottom - work.top;
    int width = std::min(MulDiv(1100, dpi, 96), workWidth);
    int height = std::min(MulDiv(760, dpi, 96), workHeight);
    SetWindowPos(m_hwnd, nullptr, work.left + (workWidth - width) / 2,
                 work.top + (workHeight - height) / 2, width, height,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    WCHAR explorerPath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, explorerPath, ARRAYSIZE(explorerPath)) &&
        ExtractIconExW(explorerPath, 0, &m_hIconLarge, &m_hIconSmall, 1)) {
        SendMessageW(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIconLarge);
        SendMessageW(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIconSmall);
    }

    // No XAML system backdrop: setting one on the island crashes the WinUI
    // version File Explorer ships with. The content has a solid background,
    // and the caption is colored to match it.
    try {
        SetSettingsWindowStep(L"creating the XAML island");
        m_source = muxh::DesktopWindowXamlSource();
        SetSettingsWindowStep(L"attaching the XAML island to the window");
        m_source.Initialize(
            winrt::Microsoft::UI::WindowId{reinterpret_cast<uint64_t>(m_hwnd)});
        SetSettingsWindowStep(L"building the content");
        auto root = BuildRoot();
        SetSettingsWindowStep(L"setting the content");
        m_source.Content(root);

        SetSettingsWindowStep(L"sizing the XAML island");
        RECT client;
        GetClientRect(m_hwnd, &client);
        m_source.SiteBridge().MoveAndResize(
            {0, 0, client.right - client.left, client.bottom - client.top});
        m_source.SiteBridge().Show();
    } catch (...) {
        ReportSettingsWindowError(DescribeCurrentException());
        return false;
    }

    if (hHintWnd &&
        GetWindowThreadProcessId(hHintWnd, nullptr) == GetCurrentThreadId() &&
        SetWindowSubclass(hHintWnd, SettingsHostSubclassProc, 0,
                          (DWORD_PTR)m_hwnd)) {
        m_hostWnd = hHintWnd;
    }

    SetSettingsWindowStep(L"showing the window");
    ShowWindow(m_hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(m_hwnd);
    return true;
}

void SettingsWindow::CloseXaml() {
    if (m_dialog) {
        try {
            m_dialog.Hide();
        } catch (...) {
        }
        m_dialog = nullptr;
    }
    if (m_source) {
        try {
            m_source.Close();
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
        m_source = nullptr;
    }
}

void SettingsWindow::Shutdown() {
    CloseXaml();
    if (m_hwnd && IsWindow(m_hwnd))
        DestroyWindow(m_hwnd);
    m_hwnd = nullptr;
    if (m_hIconLarge) {
        DestroyIcon(m_hIconLarge);
        m_hIconLarge = nullptr;
    }
    if (m_hIconSmall) {
        DestroyIcon(m_hIconSmall);
        m_hIconSmall = nullptr;
    }
}

LRESULT SettingsWindow::HandleMessage(HWND hWnd,
                                      UINT uMsg,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE:
            if (m_source) {
                m_source.SiteBridge().MoveAndResize(
                    {0, 0, LOWORD(lParam), HIWORD(lParam)});
            }
            break;

        case WM_GETMINMAXINFO: {
            UINT dpi = GetDpiForWindow(hWnd);
            if (!dpi)
                dpi = 96;
            auto* info = (MINMAXINFO*)lParam;
            info->ptMinTrackSize.x = MulDiv(860, dpi, 96);
            info->ptMinTrackSize.y = MulDiv(560, dpi, 96);
            return 0;
        }

        case WM_DPICHANGED: {
            auto* rect = (RECT*)lParam;
            SetWindowPos(hWnd, nullptr, rect->left, rect->top,
                         rect->right - rect->left, rect->bottom - rect->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        }

        case WM_SETFOCUS:
            if (HWND hChildWnd = GetWindow(hWnd, GW_CHILD))
                SetFocus(hChildWnd);
            return 0;

        case WM_SETTINGCHANGE:
            if (lParam && wcscmp((PCWSTR)lParam, L"ImmersiveColorSet") == 0)
                ApplyTheme();
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_CLOSE:
            RequestClose();
            return 0;

        case kSettingsWindowCloseMsg:
            ForceClose();
            return 0;

        case WM_DESTROY:
            CloseXaml();
            if (m_hostWnd && IsWindow(m_hostWnd)) {
                RemoveWindowSubclass(m_hostWnd, SettingsHostSubclassProc, 0);
                m_hostWnd = nullptr;
            }
            return 0;

        case WM_NCDESTROY: {
            m_hwnd = nullptr;
            if (g_settingsWindow == this)
                g_settingsWindow = nullptr;
            std::lock_guard<std::mutex> lock(g_settingsWindowMutex);
            if (g_settingsWindowHwnd == hWnd)
                g_settingsWindowHwnd = nullptr;
            break;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void SettingsWindow::LoadStyles() {
    bool needsControlsResources = true;
    try {
        if (auto app = mux::Application::Current()) {
            needsControlsResources = !app.Resources().TryLookup(
                BoxString(L"CardBackgroundFillColorDefaultBrush"));
        }
    } catch (...) {
    }

    std::wstring xaml =
        LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml">)";
    if (needsControlsResources) {
        xaml +=
            LR"(
    <ResourceDictionary.MergedDictionaries>
        <XamlControlsResources xmlns="using:Microsoft.UI.Xaml.Controls"/>
    </ResourceDictionary.MergedDictionaries>)";
    }
    xaml +=
        LR"(
    <Style x:Key="CardStyle" TargetType="Border">
        <Setter Property="Background" Value="{ThemeResource CardBackgroundFillColorDefaultBrush}"/>
        <Setter Property="BorderBrush" Value="{ThemeResource CardStrokeColorDefaultBrush}"/>
        <Setter Property="BorderThickness" Value="1"/>
        <Setter Property="CornerRadius" Value="{ThemeResource ControlCornerRadius}"/>
        <Setter Property="Padding" Value="16,12"/>
        <Setter Property="MinHeight" Value="66"/>
        <Setter Property="Margin" Value="0,0,0,4"/>
    </Style>
    <Style x:Key="SecondaryTextStyle" TargetType="TextBlock">
        <Setter Property="FontSize" Value="12"/>
        <Setter Property="Foreground" Value="{ThemeResource TextFillColorSecondaryBrush}"/>
        <Setter Property="TextWrapping" Value="Wrap"/>
    </Style>
    <Style x:Key="SectionHeaderStyle" TargetType="TextBlock">
        <Setter Property="FontSize" Value="14"/>
        <Setter Property="FontWeight" Value="SemiBold"/>
        <Setter Property="Margin" Value="1,28,0,8"/>
    </Style>
    <Style x:Key="PageTitleStyle" TargetType="TextBlock">
        <Setter Property="FontSize" Value="28"/>
        <Setter Property="FontWeight" Value="SemiBold"/>
    </Style>
    <Style x:Key="ContentAreaStyle" TargetType="Border">
        <Setter Property="Background" Value="{ThemeResource LayerFillColorDefaultBrush}"/>
        <Setter Property="BorderBrush" Value="{ThemeResource CardStrokeColorDefaultBrush}"/>
        <Setter Property="BorderThickness" Value="1,1,0,0"/>
        <Setter Property="CornerRadius" Value="8,0,0,0"/>
    </Style>
    <Style x:Key="NavItemStyle" TargetType="RadioButton">
        <Setter Property="Height" Value="36"/>
        <Setter Property="Margin" Value="4,2,4,2"/>
        <Setter Property="HorizontalAlignment" Value="Stretch"/>
        <Setter Property="Foreground" Value="{ThemeResource TextFillColorPrimaryBrush}"/>
        <Setter Property="UseSystemFocusVisuals" Value="True"/>
        <Setter Property="FocusVisualMargin" Value="0"/>
        <Setter Property="Template">
            <Setter.Value>
                <ControlTemplate TargetType="RadioButton">
                    <Grid x:Name="RootGrid" Background="Transparent" CornerRadius="{ThemeResource ControlCornerRadius}">
                        <VisualStateManager.VisualStateGroups>
                            <VisualStateGroup x:Name="CommonStates">
                                <VisualState x:Name="Normal"/>
                                <VisualState x:Name="PointerOver">
                                    <VisualState.Setters>
                                        <Setter Target="HoverBackground.Background" Value="{ThemeResource SubtleFillColorSecondaryBrush}"/>
                                    </VisualState.Setters>
                                </VisualState>
                                <VisualState x:Name="Pressed">
                                    <VisualState.Setters>
                                        <Setter Target="HoverBackground.Background" Value="{ThemeResource SubtleFillColorTertiaryBrush}"/>
                                        <Setter Target="ContentPresenter.Foreground" Value="{ThemeResource TextFillColorSecondaryBrush}"/>
                                    </VisualState.Setters>
                                </VisualState>
                                <VisualState x:Name="Disabled">
                                    <VisualState.Setters>
                                        <Setter Target="ContentPresenter.Foreground" Value="{ThemeResource TextFillColorDisabledBrush}"/>
                                    </VisualState.Setters>
                                </VisualState>
                            </VisualStateGroup>
                            <VisualStateGroup x:Name="CheckStates">
                                <VisualState x:Name="Checked">
                                    <VisualState.Setters>
                                        <Setter Target="SelectedBackground.Opacity" Value="1"/>
                                        <Setter Target="SelectionPill.Opacity" Value="1"/>
                                    </VisualState.Setters>
                                </VisualState>
                                <VisualState x:Name="Unchecked"/>
                                <VisualState x:Name="Indeterminate"/>
                            </VisualStateGroup>
                        </VisualStateManager.VisualStateGroups>
                        <Border x:Name="SelectedBackground" Background="{ThemeResource SubtleFillColorSecondaryBrush}" Opacity="0"/>
                        <Border x:Name="HoverBackground" Background="Transparent"/>
                        <Border x:Name="SelectionPill" Width="3" Height="16" CornerRadius="1.5" HorizontalAlignment="Left" VerticalAlignment="Center" Background="{ThemeResource AccentFillColorDefaultBrush}" Opacity="0"/>
                        <ContentPresenter x:Name="ContentPresenter" Content="{TemplateBinding Content}" Foreground="{TemplateBinding Foreground}" Margin="16,0,12,0" HorizontalAlignment="Left" VerticalAlignment="Center"/>
                    </Grid>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>
    <Style x:Key="FooterStyle" TargetType="Border">
        <Setter Property="BorderBrush" Value="{ThemeResource CardStrokeColorDefaultBrush}"/>
        <Setter Property="BorderThickness" Value="0,1,0,0"/>
        <Setter Property="Padding" Value="24,12"/>
    </Style>
    <Style x:Key="IconPreviewStyle" TargetType="Border">
        <Setter Property="Background" Value="{ThemeResource SubtleFillColorSecondaryBrush}"/>
        <Setter Property="CornerRadius" Value="{ThemeResource ControlCornerRadius}"/>
    </Style>
    <Style x:Key="RootStyle" TargetType="Grid">
        <Setter Property="Background" Value="{ThemeResource SolidBackgroundFillColorBaseBrush}"/>
    </Style>
</ResourceDictionary>)";

    try {
        m_resources =
            mux::Markup::XamlReader::Load(xaml).as<mux::ResourceDictionary>();
    } catch (...) {
        Wh_Log(L"Loading the styles failed: %08X", winrt::to_hresult().value);
        m_resources = nullptr;
    }

    m_cardStyle = FindStyle(L"CardStyle");
    m_secondaryTextStyle = FindStyle(L"SecondaryTextStyle");
    m_sectionHeaderStyle = FindStyle(L"SectionHeaderStyle");
    m_pageTitleStyle = FindStyle(L"PageTitleStyle");
}

mux::Style SettingsWindow::FindStyle(PCWSTR key) {
    auto boxedKey = BoxString(key);
    try {
        if (m_resources) {
            if (auto value = m_resources.TryLookup(boxedKey))
                return value.try_as<mux::Style>();
        }
        if (auto app = mux::Application::Current()) {
            if (auto value = app.Resources().TryLookup(boxedKey))
                return value.try_as<mux::Style>();
        }
    } catch (...) {
    }
    return nullptr;
}

void SettingsWindow::ApplyTheme() {
    bool dark = IsSystemDarkModeActive();
    BOOL useDarkMode = dark;
    DwmSetWindowAttribute(m_hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */,
                          &useDarkMode, sizeof(useDarkMode));
    // SolidBackgroundFillColorBase of the dark and light themes.
    COLORREF captionColor =
        dark ? RGB(0x20, 0x20, 0x20) : RGB(0xF3, 0xF3, 0xF3);
    DwmSetWindowAttribute(m_hwnd, 35 /* DWMWA_CAPTION_COLOR */, &captionColor,
                          sizeof(captionColor));
    auto theme = dark ? mux::ElementTheme::Dark : mux::ElementTheme::Light;
    if (m_root)
        m_root.RequestedTheme(theme);
    if (m_dialog)
        m_dialog.RequestedTheme(theme);
    RebuildPreview();
}

mux::UIElement SettingsWindow::BuildRoot() {
    LoadStyles();

    m_root = muxc::Grid();
    if (m_resources)
        m_root.Resources(m_resources);
    if (auto style = FindStyle(L"RootStyle"))
        m_root.Style(style);

    // A navigation pane drawn the way the WinUI 3 NavigationView draws its
    // left pane: 36px items with a subtle fill on hover and selection, and an
    // accent pill for the selected one.
    AddGridColumn(m_root, PixelLength(280));
    AddGridColumn(m_root, StarLength());

    muxc::Grid pane;
    AddGridRow(pane, AutoLength());
    AddGridRow(pane, StarLength());
    AddGridRow(pane, AutoLength());
    m_root.Children().Append(pane);

    muxc::TextBlock appTitle;
    appTitle.Text(L"Explorer Command Bar");
    appTitle.FontSize(20);
    appTitle.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
    appTitle.TextWrapping(mux::TextWrapping::Wrap);
    appTitle.Margin({20, 20, 16, 16});
    pane.Children().Append(appTitle);

    // An inline Hyperlink crashes File Explorer's XAML when clicked, so the
    // link is a HyperlinkButton next to the text.
    muxc::StackPanel credit;
    credit.Orientation(muxc::Orientation::Horizontal);
    credit.Margin({20, 12, 16, 16});
    auto creditText = MakeText(L"", m_secondaryTextStyle);
    creditText.VerticalAlignment(mux::VerticalAlignment::Center);
    auto creditInlines = creditText.Inlines();
    muxd::Run createdWith;
#ifdef WH_MOD_VERSION
    createdWith.Text(L"v" WH_MOD_VERSION L" | Created with ");
#else
    createdWith.Text(L"Created with ");
#endif
    creditInlines.Append(createdWith);
    muxd::Run heart;
    heart.Text(L"\u2665");
    heart.Foreground(muxm::SolidColorBrush(
        winrt::Windows::UI::ColorHelper::FromArgb(255, 0xF0, 0x4A, 0x5E)));
    creditInlines.Append(heart);
    muxd::Run by;
    by.Text(L" by\u00a0");
    creditInlines.Append(by);
    credit.Children().Append(creditText);

    muxc::HyperlinkButton authorLink;
    muxc::TextBlock authorName;
    authorName.Text(L"DanRotaru");
    authorName.FontSize(12);
    authorName.TextDecorations(
        winrt::Windows::UI::Text::TextDecorations::Underline);
    authorLink.Content(authorName);
    authorLink.Padding({0, 0, 0, 0});
    authorLink.MinHeight(0);
    authorLink.VerticalAlignment(mux::VerticalAlignment::Center);
    muxc::ToolTipService::SetToolTip(authorLink,
                                     BoxString(L"Developer homepage"));
    authorLink.Click([](wf::IInspectable const&, mux::RoutedEventArgs const&) {
        OpenUrl(L"https://dan13.me/?utm_source=explorer-command-bar");
    });
    credit.Children().Append(authorLink);

    auto donateButton = MakeTextButton(L"Donate");
    donateButton.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
    donateButton.Margin({16, 8, 16, 0});
    muxc::ToolTipService::SetToolTip(
        donateButton, BoxString(L"Support the development of this mod"));
    donateButton.Click(
        [](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            OpenUrl(kDonateUrl);
        });

    muxc::Grid transferButtons;
    transferButtons.ColumnSpacing(8);
    transferButtons.Margin({16, 8, 16, 0});
    AddGridColumn(transferButtons, StarLength());
    AddGridColumn(transferButtons, StarLength());
    auto importButton = MakeTextButton(L"Import");
    importButton.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
    muxc::ToolTipService::SetToolTip(importButton,
                                     BoxString(L"Import settings from a file"));
    importButton.Click(
        [this](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            ImportSettings();
        });
    transferButtons.Children().Append(importButton);
    auto exportButton = MakeTextButton(L"Export");
    exportButton.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
    muxc::ToolTipService::SetToolTip(
        exportButton, BoxString(L"Export all settings to a file"));
    exportButton.Click(
        [this](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            ExportSettings();
        });
    muxc::Grid::SetColumn(exportButton, 1);
    transferButtons.Children().Append(exportButton);

    muxc::StackPanel paneFooter;
    paneFooter.Children().Append(transferButtons);
    paneFooter.Children().Append(donateButton);
    credit.Margin({16, 10, 16, 16});
    credit.HorizontalAlignment(mux::HorizontalAlignment::Center);
    paneFooter.Children().Append(credit);
    muxc::Grid::SetRow(paneFooter, 2);
    pane.Children().Append(paneFooter);

    muxc::StackPanel navItems;
    navItems.Margin({0, 0, 0, 8});
    muxc::ScrollViewer navScroller;
    navScroller.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Auto);
    navScroller.Content(navItems);
    muxc::Grid::SetRow(navScroller, 1);
    pane.Children().Append(navScroller);

    auto navItemStyle = FindStyle(L"NavItemStyle");
    muxc::RadioButton firstItem{nullptr};
    auto addNavItem = [this, &navItems, &navItemStyle, &firstItem](
                          std::wstring const& tag, std::wstring const& title,
                          PCWSTR glyph) {
        muxc::StackPanel itemContent;
        itemContent.Orientation(muxc::Orientation::Horizontal);
        itemContent.Spacing(16);
        auto icon = MakeGlyph(glyph, 16);
        icon.VerticalAlignment(mux::VerticalAlignment::Center);
        itemContent.Children().Append(icon);
        muxc::TextBlock text;
        text.Text(title);
        text.FontSize(14);
        text.VerticalAlignment(mux::VerticalAlignment::Center);
        itemContent.Children().Append(text);

        muxc::RadioButton navItem;
        if (navItemStyle)
            navItem.Style(navItemStyle);
        navItem.GroupName(L"SettingsNavigation");
        navItem.Content(itemContent);
        mux::Automation::AutomationProperties::SetName(navItem, title);
        navItem.Checked(
            [this, tag](wf::IInspectable const&, mux::RoutedEventArgs const&) {
                if (tag != m_currentPage)
                    ShowPage(tag);
            });
        navItems.Children().Append(navItem);
        m_navButtons.push_back({tag, navItem});
        if (!firstItem)
            firstItem = navItem;
    };
    addNavItem(L"items", L"Toolbar items", L"\uE71D");
    for (auto const& page : DescribeSettingsPages(m_model))
        addNavItem(page.tag, page.title, page.glyph);

    muxc::Grid content;
    AddGridRow(content, StarLength());
    AddGridRow(content, AutoLength());
    AddGridRow(content, AutoLength());

    m_pageHost = muxc::Grid();
    content.Children().Append(m_pageHost);

    m_infoBar = muxc::InfoBar();
    m_infoBar.IsOpen(false);
    m_infoBar.Margin({24, 0, 24, 12});
    muxc::Grid::SetRow(m_infoBar, 1);
    content.Children().Append(m_infoBar);

    muxc::Border footer;
    if (auto style = FindStyle(L"FooterStyle"))
        footer.Style(style);
    else
        footer.Padding({24, 12, 24, 12});
    muxc::Grid::SetRow(footer, 2);

    muxc::Grid footerGrid;
    footerGrid.ColumnSpacing(12);
    AddGridColumn(footerGrid, StarLength());
    AddGridColumn(footerGrid, AutoLength());
    AddGridColumn(footerGrid, AutoLength());
    AddGridColumn(footerGrid, AutoLength());

    m_statusText = MakeText(L"", m_secondaryTextStyle);
    m_statusText.VerticalAlignment(mux::VerticalAlignment::Center);
    footerGrid.Children().Append(m_statusText);

    m_savingRing = muxc::ProgressRing();
    m_savingRing.Width(20);
    m_savingRing.Height(20);
    m_savingRing.IsActive(false);
    muxc::Grid::SetColumn(m_savingRing, 1);
    footerGrid.Children().Append(m_savingRing);

    m_discardButton = MakeTextButton(L"Discard changes");
    m_discardButton.Click([this](wf::IInspectable const&,
                                 mux::RoutedEventArgs const&) { Discard(); });
    muxc::Grid::SetColumn(m_discardButton, 2);
    footerGrid.Children().Append(m_discardButton);

    m_saveButton = MakeTextButton(L"Save");
    m_saveButton.MinWidth(120);
    if (auto style = FindStyle(L"AccentButtonStyle"))
        m_saveButton.Style(style);
    m_saveButton.Click([this](wf::IInspectable const&,
                              mux::RoutedEventArgs const&) { Save(false); });
    muxc::ToolTipService::SetToolTip(m_saveButton, BoxString(L"Ctrl+S"));
    muxc::Grid::SetColumn(m_saveButton, 3);
    footerGrid.Children().Append(m_saveButton);

    footer.Child(footerGrid);
    content.Children().Append(footer);

    muxc::Border contentArea;
    if (auto style = FindStyle(L"ContentAreaStyle"))
        contentArea.Style(style);
    contentArea.Child(content);
    muxc::Grid::SetColumn(contentArea, 1);
    m_root.Children().Append(contentArea);

    mux::Input::KeyboardAccelerator saveAccelerator;
    saveAccelerator.Key(winrt::Windows::System::VirtualKey::S);
    saveAccelerator.Modifiers(
        winrt::Windows::System::VirtualKeyModifiers::Control);
    saveAccelerator.Invoked(
        [this](mux::Input::KeyboardAccelerator const&,
               mux::Input::KeyboardAcceleratorInvokedEventArgs const& args) {
            args.Handled(true);
            if (m_dirty)
                Save(false);
        });
    m_root.KeyboardAccelerators().Append(saveAccelerator);
    // Otherwise the root shows a "Ctrl+S" tooltip wherever the pointer
    // rests. The Save button has the tooltip instead.
    m_root.KeyboardAcceleratorPlacementMode(
        mux::Input::KeyboardAcceleratorPlacementMode::Hidden);

    ApplyTheme();
    firstItem.IsChecked(true);
    if (m_currentPage.empty())
        ShowPage(L"items");
    UpdateFooter();
    return m_root;
}

void SettingsWindow::ShowPage(std::wstring const& tag) {
    m_currentPage = tag;
    m_previewHost = nullptr;
    m_previewItems.clear();
    m_previewBuiltIns.clear();
    m_selectedBuiltIn = -1;
    m_editorPanel = nullptr;
    m_editorIconPreview = nullptr;
    m_moveUpButton = nullptr;
    m_moveDownButton = nullptr;
    m_duplicateButton = nullptr;
    m_deleteButton = nullptr;
    m_addEntryItem = nullptr;
    m_addSubmenuItem = nullptr;
    m_editorGeneration++;

    auto children = m_pageHost.Children();
    children.Clear();

    if (tag == L"items") {
        children.Append(BuildItemsPage());
        RefreshItemsPage();
        return;
    }

    for (auto const& page : DescribeSettingsPages(m_model)) {
        if (page.tag == tag) {
            children.Append(BuildSettingsPage(page));
            return;
        }
    }
}

muxc::TextBlock SettingsWindow::MakeText(std::wstring const& text,
                                         mux::Style const& style) {
    muxc::TextBlock textBlock;
    if (style)
        textBlock.Style(style);
    textBlock.Text(text);
    return textBlock;
}

muxc::StackPanel SettingsWindow::MakePageHeader(
    std::wstring const& title,
    std::wstring const& description) {
    muxc::StackPanel header;
    header.Spacing(4);
    auto titleText = MakeText(title, m_pageTitleStyle);
    if (!m_pageTitleStyle) {
        titleText.FontSize(28);
        titleText.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
    }
    header.Children().Append(titleText);
    if (!description.empty()) {
        auto descriptionText = MakeText(description, m_secondaryTextStyle);
        descriptionText.FontSize(14);
        descriptionText.TextWrapping(mux::TextWrapping::Wrap);
        header.Children().Append(descriptionText);
    }
    return header;
}

muxc::Border SettingsWindow::MakeCard(PCWSTR glyph,
                                      std::wstring const& title,
                                      std::wstring const& description,
                                      mux::FrameworkElement const& control,
                                      bool wide) {
    muxc::Border card;
    if (m_cardStyle) {
        card.Style(m_cardStyle);
    } else {
        card.Padding({16, 12, 16, 12});
        card.Margin({0, 0, 0, 4});
    }

    muxc::Grid grid;
    grid.ColumnSpacing(16);
    int textColumn = 0;
    if (glyph) {
        AddGridColumn(grid, AutoLength());
        auto icon = MakeGlyph(glyph, 20);
        icon.VerticalAlignment(mux::VerticalAlignment::Center);
        grid.Children().Append(icon);
        textColumn = 1;
    }
    AddGridColumn(grid, StarLength());
    AddGridColumn(grid, AutoLength());

    muxc::StackPanel text;
    text.VerticalAlignment(mux::VerticalAlignment::Center);
    muxc::TextBlock titleText;
    titleText.Text(title);
    titleText.TextWrapping(mux::TextWrapping::Wrap);
    text.Children().Append(titleText);
    if (!description.empty())
        text.Children().Append(MakeText(description, m_secondaryTextStyle));
    muxc::Grid::SetColumn(text, textColumn);
    grid.Children().Append(text);

    if (control) {
        if (wide) {
            AddGridRow(grid, AutoLength());
            AddGridRow(grid, AutoLength());
            control.Margin({0, 10, 0, 2});
            control.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
            muxc::Grid::SetRow(control, 1);
            muxc::Grid::SetColumn(control, textColumn);
            muxc::Grid::SetColumnSpan(control, 2);
        } else {
            control.VerticalAlignment(mux::VerticalAlignment::Center);
            muxc::Grid::SetColumn(control, textColumn + 1);
        }
        grid.Children().Append(control);
    }

    card.Child(grid);
    return card;
}

mux::FrameworkElement SettingsWindow::MakeFieldControl(
    SettingField const& field) {
    switch (field.kind) {
        case SettingKind::Bool: {
            auto* value = (bool*)field.value;
            bool inverted = field.inverted;
            muxc::ToggleSwitch toggle;
            toggle.IsOn(inverted ? !*value : *value);
            toggle.MinWidth(0);
            toggle.Toggled(
                [this, value, inverted](wf::IInspectable const& sender,
                                        mux::RoutedEventArgs const&) {
                    bool isOn = sender.as<muxc::ToggleSwitch>().IsOn();
                    *value = inverted ? !isOn : isOn;
                    OnModelChanged();
                });
            return toggle;
        }

        case SettingKind::Int: {
            auto* value = (int*)field.value;
            int minimum = field.minimum;
            muxc::NumberBox numberBox;
            numberBox.Minimum(minimum);
            numberBox.Value(*value);
            numberBox.SmallChange(1);
            numberBox.LargeChange(10);
            numberBox.Width(170);
            numberBox.SpinButtonPlacementMode(
                muxc::NumberBoxSpinButtonPlacementMode::Compact);
            numberBox.ValueChanged(
                [this, value, minimum](
                    muxc::NumberBox const& sender,
                    muxc::NumberBoxValueChangedEventArgs const& args) {
                    double newValue = args.NewValue();
                    if (std::isnan(newValue)) {
                        sender.Value(*value);
                        return;
                    }
                    *value = std::max(minimum, (int)std::lround(newValue));
                    OnModelChanged();
                });
            return numberBox;
        }

        case SettingKind::String: {
            auto* value = (std::wstring*)field.value;
            auto textBox = MakeTextBox(*value, nullptr);
            if (!field.wide)
                textBox.Width(280);
            textBox.TextChanged(
                [this, value](wf::IInspectable const& sender,
                              muxc::TextChangedEventArgs const&) {
                    *value = std::wstring(sender.as<muxc::TextBox>().Text());
                    OnModelChanged();
                });
            return textBox;
        }

        case SettingKind::Header:
            break;
    }
    return nullptr;
}

mux::FrameworkElement SettingsWindow::BuildSettingsPage(
    SettingsPageDef const& page) {
    muxc::StackPanel panel;
    panel.Margin({36, 24, 36, 36});
    panel.MaxWidth(1000);
    panel.Children().Append(MakePageHeader(page.title, page.description));

    bool first = true;
    for (auto const& field : page.fields) {
        if (field.kind == SettingKind::Header) {
            auto header = MakeText(field.title, m_sectionHeaderStyle);
            if (!m_sectionHeaderStyle) {
                header.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
                header.Margin({1, 28, 0, 8});
            }
            panel.Children().Append(header);
        } else {
            auto card = MakeCard(field.glyph, field.title, field.description,
                                 MakeFieldControl(field), field.wide);
            if (first)
                card.Margin({0, 24, 0, 4});
            panel.Children().Append(card);
        }
        first = false;
    }

    muxc::ScrollViewer scroller;
    scroller.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Auto);
    scroller.Content(panel);
    return scroller;
}

// Ids of the built-in elements in the command bar preview, besides the
// kDefaultButtons indexes.
constexpr int kPreviewSeeMoreId = kDefaultButtonCount;
constexpr int kPreviewDetailsId = kDefaultButtonCount + 1;
// The settings item (cog) isn't built-in, but is selected the same way.
constexpr int kPreviewSettingsItemId = kDefaultButtonCount + 2;

std::wstring BuiltInButtonName(int id) {
    std::lock_guard<std::mutex> lock(g_defaultButtonLooksMutex);
    if (id == kPreviewSeeMoreId)
        return L"See more";
    if (id == kPreviewSettingsItemId)
        return L"Command bar settings";
    if (id == kPreviewDetailsId) {
        return g_detailsButtonLook.label.empty() ? L"Details"
                                                 : g_detailsButtonLook.label;
    }
    return g_defaultButtonLooks[id].label.empty()
               ? std::wstring(kDefaultButtonTitles[id])
               : g_defaultButtonLooks[id].label;
}

// The default checked look is a solid accent fill meant for glyphs which turn
// white, which leaves File Explorer's colored icons hard to see. Selected
// buttons get the subtle fill WinUI uses for selected list and navigation
// items instead, outlined with the accent color.
void ApplyPreviewSelectionStyle(muxc::AppBarToggleButton const& button) {
    using winrt::Windows::UI::ColorHelper;
    namespace wuvm = winrt::Windows::UI::ViewManagement;
    bool dark = IsSystemDarkModeActive();
    auto subtle = [dark](uint8_t darkAlpha, uint8_t lightAlpha) {
        return muxm::SolidColorBrush(
            dark ? ColorHelper::FromArgb(darkAlpha, 0xFF, 0xFF, 0xFF)
                 : ColorHelper::FromArgb(lightAlpha, 0x00, 0x00, 0x00));
    };
    winrt::Windows::UI::Color accent =
        dark ? ColorHelper::FromArgb(0xFF, 0x99, 0xEB, 0xFF)
             : ColorHelper::FromArgb(0xFF, 0x00, 0x5F, 0xB8);
    try {
        accent = wuvm::UISettings().GetColorValue(
            dark ? wuvm::UIColorType::AccentLight2
                 : wuvm::UIColorType::AccentDark1);
    } catch (...) {
    }
    muxm::SolidColorBrush accentBrush(accent);
    muxm::SolidColorBrush text(
        dark ? ColorHelper::FromArgb(0xFF, 0xFF, 0xFF, 0xFF)
             : ColorHelper::FromArgb(0xE4, 0, 0, 0));

    auto resources = button.Resources();
    auto set = [&resources](PCWSTR key, wf::IInspectable const& value) {
        resources.Insert(BoxString(key), value);
    };
    set(L"AppBarToggleButtonBackgroundChecked", subtle(0x15, 0x0F));
    set(L"AppBarToggleButtonBackgroundCheckedPointerOver", subtle(0x1F, 0x18));
    set(L"AppBarToggleButtonBackgroundCheckedPressed", subtle(0x0B, 0x09));
    set(L"AppBarToggleButtonBorderBrushChecked", accentBrush);
    set(L"AppBarToggleButtonBorderBrushCheckedPointerOver", accentBrush);
    set(L"AppBarToggleButtonBorderBrushCheckedPressed", accentBrush);
    set(L"AppBarToggleButtonForegroundChecked", text);
    set(L"AppBarToggleButtonForegroundCheckedPointerOver", text);
    set(L"AppBarToggleButtonForegroundCheckedPressed", text);
    button.BorderThickness({1, 1, 1, 1});
}

// Icon-only buttons are as narrow as File Explorer's own, instead of the
// 68px a command bar button takes by default.
void MakeCompactPreviewButton(mux::FrameworkElement const& button) {
    button.MinWidth(0);
    button.Width(44);
}

// The command bars sit on a card, so they don't draw a background of their
// own, and their buttons get rounded corners like File Explorer's.
void StyleCardCommandBar(muxc::CommandBar const& bar) {
    bar.Background(muxm::SolidColorBrush(
        winrt::Windows::UI::ColorHelper::FromArgb(0, 0, 0, 0)));
    bar.BorderThickness({0, 0, 0, 0});
    for (auto const& command : bar.PrimaryCommands()) {
        if (auto control = command.try_as<muxc::Control>();
            control && !command.try_as<muxc::AppBarSeparator>()) {
            control.CornerRadius({4, 4, 4, 4});
        }
    }
}

mux::FrameworkElement SettingsWindow::BuildItemsPage() {
    muxc::Grid page;
    page.Margin({36, 24, 36, 0});
    AddGridRow(page, AutoLength());
    AddGridRow(page, AutoLength());
    AddGridRow(page, AutoLength());
    AddGridRow(page, StarLength());
    page.Children().Append(MakePageHeader(
        L"Toolbar items",
        L"This is the File Explorer command bar with your settings. Click an "
        L"item to edit it, or open a menu to pick one of its entries. Drag "
        L"your items to reorder them. Hidden built-in buttons can be shown "
        L"again on the Built-in buttons page."));

    m_previewHost = muxc::Border();
    if (m_cardStyle)
        m_previewHost.Style(m_cardStyle);
    m_previewHost.Padding({4, 4, 4, 4});
    m_previewHost.MinHeight(0);
    m_previewHost.Margin({0, 20, 0, 0});
    muxc::Grid::SetRow(m_previewHost, 1);
    page.Children().Append(m_previewHost);

    auto toolbar = BuildItemToolbar();
    StyleCardCommandBar(toolbar);
    muxc::Border toolbarCard;
    if (m_cardStyle)
        toolbarCard.Style(m_cardStyle);
    toolbarCard.Padding({4, 4, 4, 4});
    toolbarCard.MinHeight(0);
    toolbarCard.Margin({0, 8, 0, 4});
    toolbarCard.HorizontalAlignment(mux::HorizontalAlignment::Left);
    toolbarCard.Child(toolbar);
    muxc::Grid::SetRow(toolbarCard, 2);
    page.Children().Append(toolbarCard);

    muxc::ScrollViewer editorScroller;
    editorScroller.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Auto);
    m_editorPanel = muxc::StackPanel();
    m_editorPanel.Padding({0, 8, 16, 24});
    editorScroller.Content(m_editorPanel);
    muxc::Grid::SetRow(editorScroller, 3);
    page.Children().Append(editorScroller);

    return page;
}

muxc::CommandBar SettingsWindow::BuildItemToolbar() {
    muxc::CommandBar bar;
    bar.DefaultLabelPosition(muxc::CommandBarDefaultLabelPosition::Right);
    bar.OverflowButtonVisibility(
        muxc::CommandBarOverflowButtonVisibility::Collapsed);
    bar.HorizontalAlignment(mux::HorizontalAlignment::Left);

    muxc::MenuFlyout addMenu;
    addMenu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    auto addMenuItem = [&addMenu](PCWSTR text, PCWSTR glyph,
                                  std::function<void()> action) {
        muxc::MenuFlyoutItem item;
        item.Text(text);
        item.Icon(CreateGlyphIcon(glyph));
        item.Click([action](wf::IInspectable const&,
                            mux::RoutedEventArgs const&) { action(); });
        addMenu.Items().Append(item);
        return item;
    };
    addMenuItem(L"Toolbar button", L"",
                [this]() { AddTopLevelItem(false); });
    addMenuItem(L"Dropdown menu", L"", [this]() { AddTopLevelItem(true); });
    addMenu.Items().Append(muxc::MenuFlyoutSeparator());
    m_addEntryItem =
        addMenuItem(L"Menu entry", L"", [this]() { AddMenuEntry(false); });
    m_addSubmenuItem =
        addMenuItem(L"Submenu", L"", [this]() { AddMenuEntry(true); });

    muxc::AppBarButton addButton;
    addButton.Label(L"Add");
    addButton.Icon(CreateGlyphIcon(L""));
    addButton.Flyout(addMenu);
    bar.PrimaryCommands().Append(addButton);
    bar.PrimaryCommands().Append(muxc::AppBarSeparator());

    auto makeButton = [&bar](PCWSTR label, PCWSTR glyph,
                             std::function<void()> action) {
        muxc::AppBarButton button;
        button.Label(label);
        button.Icon(CreateGlyphIcon(glyph));
        button.Click([action](wf::IInspectable const&,
                              mux::RoutedEventArgs const&) { action(); });
        bar.PrimaryCommands().Append(button);
        return button;
    };
    m_moveUpButton =
        makeButton(L"Move left", L"", [this]() { MoveSelectedItem(-1); });
    m_moveDownButton =
        makeButton(L"Move right", L"", [this]() { MoveSelectedItem(1); });
    m_duplicateButton =
        makeButton(L"Duplicate", L"", [this]() { DuplicateSelectedItem(); });
    m_deleteButton =
        makeButton(L"Delete", L"", [this]() { DeleteSelectedItem(); });

    return bar;
}

std::vector<ActionItem>* SettingsWindow::SiblingsOf(
    std::vector<int> const& path) {
    std::vector<ActionItem>* list = &m_model.items;
    for (size_t i = 0; i + 1 < path.size(); i++) {
        if (path[i] < 0 || path[i] >= (int)list->size())
            return nullptr;
        list = &(*list)[path[i]].subItems;
    }
    return list;
}

std::vector<ActionItem>* SettingsWindow::ChildrenOf(
    std::vector<int> const& path) {
    if (path.empty())
        return &m_model.items;
    ActionItem* item = ItemAt(path);
    return item ? &item->subItems : nullptr;
}

ActionItem* SettingsWindow::ItemAt(std::vector<int> const& path) {
    if (path.empty())
        return nullptr;
    auto* siblings = SiblingsOf(path);
    if (!siblings || path.back() < 0 || path.back() >= (int)siblings->size())
        return nullptr;
    return &(*siblings)[path.back()];
}

ActionItem* SettingsWindow::EditedItem(std::vector<int> const& path,
                                       int generation) {
    return generation == m_editorGeneration ? ItemAt(path) : nullptr;
}

void SettingsWindow::RefreshItemsPage() {
    if (!m_previewHost)
        return;
    if (!ItemAt(m_selectedPath))
        m_selectedPath.clear();
    if (!m_selectedPath.empty())
        m_selectedBuiltIn = -1;
    RebuildPreview();
    BuildItemEditor();
    UpdateItemToolbar();
}

muxc::AppBarButton SettingsWindow::MakePreviewButton(
    muxc::IconElement const& icon,
    std::wstring const& label,
    bool showLabel,
    bool chevron,
    std::wstring const& pageTag) {
    muxc::AppBarButton button;
    button.Label(label);
    button.Icon(icon);
    button.LabelPosition(showLabel ? muxc::CommandBarLabelPosition::Default
                                   : muxc::CommandBarLabelPosition::Collapsed);
    if (!showLabel)
        MakeCompactPreviewButton(button);
    muxc::ToolTipService::SetToolTip(button, BoxString(label));
    if (showLabel && chevron) {
        button.Loaded(
            [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
                if (auto button = sender.try_as<muxc::AppBarButton>())
                    AddNewPlusChevron(button);
            });
    }
    button.Click(
        [this, pageTag](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            if (IsDragClick())
                return;
            NavigateTo(pageTag);
        });
    return button;
}

void SettingsWindow::AppendPreviewMenuEntries(
    std::vector<ActionItem> const& items,
    std::vector<int> const& parentPath,
    wfc::IVector<muxc::MenuFlyoutItemBase> const& target) {
    for (int i = 0; i < (int)items.size(); i++) {
        auto const& entry = items[i];
        if (IsEmptyActionItem(entry))
            continue;
        std::vector<int> path = parentPath;
        path.push_back(i);
        int depth = (int)path.size() - 1;
        std::wstring name = entry.name.empty() ? L"Untitled" : entry.name;

        muxc::IconElement icon{nullptr};
        if (!entry.hideIcon)
            icon = TryCreateIconElement(entry.icon, entry.command);

        if (entry.isMenu && depth < kMaxMenuDepth && !entry.subItems.empty()) {
            muxc::MenuFlyoutSubItem subMenu;
            subMenu.Text(name);
            if (icon)
                subMenu.Icon(icon);
            muxc::MenuFlyoutItem editItem;
            editItem.Text(L"Edit “" + name + L"”");
            editItem.Icon(CreateGlyphIcon(L""));
            editItem.Click([this, path](wf::IInspectable const&,
                                        mux::RoutedEventArgs const&) {
                SelectItem(path);
            });
            subMenu.Items().Append(editItem);
            subMenu.Items().Append(muxc::MenuFlyoutSeparator());
            AppendPreviewMenuEntries(entry.subItems, path, subMenu.Items());
            target.Append(subMenu);
        } else {
            muxc::MenuFlyoutItem menuItem;
            menuItem.Text(name);
            if (icon)
                menuItem.Icon(icon);
            menuItem.Click([this, path](wf::IInspectable const&,
                                        mux::RoutedEventArgs const&) {
                SelectItem(path);
            });
            target.Append(menuItem);
        }

        if (entry.separatorAfter)
            target.Append(muxc::MenuFlyoutSeparator());
    }
}

muxc::AppBarToggleButton SettingsWindow::MakePreviewItem(ActionItem const& item,
                                                         int index) {
    muxc::AppBarToggleButton button;
    ApplyPreviewSelectionStyle(button);
    bool isMenu = item.isMenu && !item.subItems.empty();
    bool chevron = isMenu && m_model.showMenuChevrons;
    std::wstring label = item.labelText.empty() ? item.name : item.labelText;
    bool hasLabel = item.showLabel && !label.empty();
    button.Label(chevron ? MenuButtonLabel(label, hasLabel, true) : label);
    if (auto icon = MakeCommandButtonIcon(item))
        button.Icon(icon);
    button.LabelPosition(hasLabel || chevron
                             ? muxc::CommandBarLabelPosition::Default
                             : muxc::CommandBarLabelPosition::Collapsed);
    if (!item.showLabel && !chevron)
        MakeCompactPreviewButton(button);
    muxc::ToolTipService::SetToolTip(
        button, BoxString(item.name.empty() ? L"Untitled" : item.name));
    if (chevron) {
        button.Loaded([hasLabel, label](wf::IInspectable const& sender,
                                        mux::RoutedEventArgs const&) {
            if (auto element = sender.try_as<mux::FrameworkElement>())
                AddChevronToLabel(element, hasLabel ? label : L"");
        });
    }

    if (isMenu) {
        muxc::MenuFlyout menu;
        menu.Placement(
            muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
        AppendPreviewMenuEntries(item.subItems, {index}, menu.Items());
        muxc::Primitives::FlyoutBase::SetAttachedFlyout(button, menu);
    }

    button.Click([this, index, isMenu](wf::IInspectable const& sender,
                                       mux::RoutedEventArgs const&) {
        if (IsDragClick())
            return;
        SelectItem({index});
        if (isMenu) {
            if (auto element = sender.try_as<mux::FrameworkElement>())
                muxc::Primitives::FlyoutBase::ShowAttachedFlyout(element);
        }
    });
    return button;
}

void SettingsWindow::RebuildPreview() {
    if (!m_previewHost)
        return;

    DefaultButtonLook looks[kDefaultButtonCount];
    DefaultButtonLook detailsLook;
    {
        std::lock_guard<std::mutex> lock(g_defaultButtonLooksMutex);
        for (int i = 0; i < kDefaultButtonCount; i++)
            looks[i] = g_defaultButtonLooks[i];
        detailsLook = g_detailsButtonLook;
    }

    auto svgOrGlyphIcon = [](std::wstring const& uri,
                             PCWSTR glyph) -> muxc::IconElement {
        if (!uri.empty()) {
            try {
                muxm::Imaging::SvgImageSource source(
                    wf::Uri(winrt::hstring{uri}));
                muxc::ImageIcon icon;
                icon.Source(source);
                return icon;
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
        }
        return CreateGlyphIcon(glyph);
    };
    auto builtInIcon = [&](int index) {
        return svgOrGlyphIcon(looks[index].iconUri,
                              kDefaultButtonGlyphs[index]);
    };
    auto builtInLabel = [&](int index) {
        return looks[index].label.empty()
                   ? std::wstring(kDefaultButtonTitles[index])
                   : looks[index].label;
    };
    auto configureBar = [](muxc::CommandBar const& bar) {
        bar.DefaultLabelPosition(muxc::CommandBarDefaultLabelPosition::Right);
        bar.IsDynamicOverflowEnabled(false);
        bar.OverflowButtonVisibility(
            muxc::CommandBarOverflowButtonVisibility::Collapsed);
        bar.VerticalAlignment(mux::VerticalAlignment::Center);
    };

    auto const& s = m_model;
    muxc::CommandBar primary;
    configureBar(primary);
    primary.HorizontalAlignment(mux::HorizontalAlignment::Left);
    muxc::CommandBar secondary;
    configureBar(secondary);
    secondary.HorizontalAlignment(mux::HorizontalAlignment::Right);
    auto primaryCommands = primary.PrimaryCommands();
    auto secondaryCommands = secondary.PrimaryCommands();

    auto appendNewPlus = [&]() -> mux::UIElement {
        if (!s.newPlus.enabled)
            return nullptr;
        bool showLabel = s.newPlus.showLabel && !s.newPlus.buttonLabel.empty();
        auto icon = s.newPlus.buttonIcon.empty()
                        ? builtInIcon(kNewButtonIndex)
                        : CreateIconElement(s.newPlus.buttonIcon,
                                            std::wstring(), L"");
        auto button = MakePreviewButton(
            icon,
            s.newPlus.buttonLabel.empty() ? L"New+" : s.newPlus.buttonLabel,
            showLabel, true, L"newPlus");
        primaryCommands.Append(button);
        return button;
    };

    m_previewBuiltIns.clear();
    auto isBuiltInShown = [&](int id) {
        if (id == kNewButtonIndex && s.newPlus.enabled &&
            !s.newPlus.keepOriginalNewButton)
            return false;
        return !*BuiltInHiddenField(id);
    };
    auto appendBuiltIn = [&](int id) -> mux::UIElement {
        if (!isBuiltInShown(id))
            return nullptr;
        bool labeled = id == kNewButtonIndex || id == kSortButtonIndex ||
                       id == kViewButtonIndex;
        bool showLabel = labeled && !s.hideDefaultButtonLabels[id];
        auto button = MakePreviewBuiltIn(id, builtInIcon(id), builtInLabel(id),
                                         showLabel, labeled);
        primaryCommands.Append(button);
        return button;
    };

    // The built-in buttons in their order, with separators placed the way
    // ApplyDefaultButtonOrder and ApplyDefaultButtonVisibility do: File
    // Explorer has two separators among these buttons, a separator after
    // New or Delete follows its setting, and any other one is shown unless
    // all buttons up to the next separator are hidden. The shown buttons
    // and separators can be dragged to reorder them; builtInSlots holds the
    // position in the order of each of them.
    std::vector<int> order = ParseDefaultButtonOrder(s.defaultButtonOrder);
    if (order.empty())
        order = DefaultButtonOrderTokens();
    std::vector<mux::UIElement> builtInElements;
    std::vector<size_t> builtInSlots;
    int separatorsLeft = 2;
    int prevButton = -1;
    for (size_t i = 0; i < order.size(); i++) {
        int token = order[i];
        if (token != kOrderSeparator) {
            // With New+, its button stands for New when dragged.
            mux::UIElement element{nullptr};
            if (token == kNewButtonIndex)
                element = appendNewPlus();
            mux::UIElement builtIn = appendBuiltIn(token);
            if (!element)
                element = builtIn;
            if (element) {
                builtInElements.push_back(element);
                builtInSlots.push_back(i);
            }
            prevButton = token;
            continue;
        }

        if (separatorsLeft == 0)
            continue;
        separatorsLeft--;
        bool show;
        if (prevButton == kNewButtonIndex) {
            show = !s.hideSeparatorAfterNew;
        } else if (prevButton == kDeleteButtonIndex) {
            show = !s.hideSeparatorAfterDelete;
        } else {
            show = false;
            for (size_t j = i + 1;
                 j < order.size() && order[j] != kOrderSeparator; j++) {
                if (isBuiltInShown(order[j]) ||
                    (order[j] == kNewButtonIndex && s.newPlus.enabled)) {
                    show = true;
                    break;
                }
            }
        }
        if (show) {
            muxc::AppBarSeparator separator;
            primaryCommands.Append(separator);
            builtInElements.push_back(separator);
            builtInSlots.push_back(i);
        }
        prevButton = -1;
    }
    SetDragGroup(kDragGroupBuiltIns, primary, std::move(builtInElements),
                 [this, builtInSlots](int from, int to) {
                     ReorderBuiltInButtons(builtInSlots, from, to);
                 });
    if (!s.hideSeparatorAfterView)
        primaryCommands.Append(muxc::AppBarSeparator());

    // The custom items, with the settings item at its position among them.
    auto customCommands =
        s.placeOnSecondaryBar ? secondaryCommands : primaryCommands;
    bool settingsItemAdded = !s.showSettingsItem;
    std::vector<mux::UIElement> dragElements;
    std::vector<int> dragOrder;
    auto appendSettingsItem = [&]() {
        auto button = MakePreviewBuiltIn(
            kPreviewSettingsItemId, CreateGlyphIcon(L""),
            BuiltInButtonName(kPreviewSettingsItemId), false, false);
        customCommands.Append(button);
        dragElements.push_back(button);
        dragOrder.push_back(-1);
        settingsItemAdded = true;
    };
    m_previewItems.assign(s.items.size(), muxc::AppBarToggleButton{nullptr});
    int itemPosition = 0;
    for (int i = 0; i < (int)s.items.size(); i++) {
        auto const& item = s.items[i];
        if (IsEmptyActionItem(item))
            continue;
        if (!settingsItemAdded && s.settingsItemOrder == itemPosition)
            appendSettingsItem();
        m_previewItems[i] = MakePreviewItem(item, i);
        customCommands.Append(m_previewItems[i]);
        dragElements.push_back(m_previewItems[i]);
        dragOrder.push_back(i);
        if (item.separatorAfter)
            customCommands.Append(muxc::AppBarSeparator());
        itemPosition++;
    }
    if (!settingsItemAdded)
        appendSettingsItem();
    SetDragGroup(kDragGroupPreview,
                 s.placeOnSecondaryBar ? mux::UIElement(secondary)
                                       : mux::UIElement(primary),
                 std::move(dragElements),
                 [this, dragOrder](int from, int to) {
                     ReorderToolbarItems(dragOrder, from, to);
                 });

    if (s.contextMenuItem.enabled) {
        primaryCommands.Append(MakePreviewButton(
            CreateIconElement(s.contextMenuItem.buttonIcon, std::wstring(),
                              L""),
            s.contextMenuItem.buttonLabel, s.contextMenuItem.showLabel, false,
            L"contextMenu"));
    }
    if (!s.hideMoreButton) {
        primaryCommands.Append(MakePreviewBuiltIn(
            kPreviewSeeMoreId, CreateGlyphIcon(L""),
            BuiltInButtonName(kPreviewSeeMoreId), false, false));
    }
    if (!s.hideDetailsButton) {
        secondaryCommands.Append(MakePreviewBuiltIn(
            kPreviewDetailsId, svgOrGlyphIcon(detailsLook.iconUri, L""),
            BuiltInButtonName(kPreviewDetailsId), !s.hideDetailsButtonLabel,
            false));
    }

    StyleCardCommandBar(primary);
    StyleCardCommandBar(secondary);

    muxc::Grid layout;
    layout.ColumnSpacing(8);
    AddGridColumn(layout, StarLength());
    AddGridColumn(layout, AutoLength());

    muxc::ScrollViewer scroller;
    scroller.HorizontalScrollBarVisibility(muxc::ScrollBarVisibility::Auto);
    scroller.HorizontalScrollMode(muxc::ScrollMode::Enabled);
    scroller.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Disabled);
    scroller.VerticalScrollMode(muxc::ScrollMode::Disabled);
    scroller.Content(primary);
    layout.Children().Append(scroller);

    // The scroll bar is drawn over the content, so while the command bar
    // doesn't fit, room is made for it below the buttons.
    auto updateScrollBarRoom = [weakScroller = winrt::make_weak(scroller),
                                weakPrimary = winrt::make_weak(primary)](
                                   wf::IInspectable const&,
                                   mux::SizeChangedEventArgs const&) {
        auto scroller = weakScroller.get();
        auto primary = weakPrimary.get();
        if (!scroller || !primary)
            return;
        bool overflows = primary.ActualWidth() > scroller.ActualWidth() + 0.5;
        double bottom = overflows ? 14 : 0;
        if (scroller.Padding().Bottom != bottom)
            scroller.Padding({0, 0, 0, bottom});
    };
    scroller.SizeChanged(updateScrollBarRoom);
    primary.SizeChanged(updateScrollBarRoom);

    muxc::Grid::SetColumn(secondary, 1);
    layout.Children().Append(secondary);

    m_previewHost.Child(layout);
    UpdatePreviewSelection();
}

void SettingsWindow::UpdatePreviewSelection() {
    int selected = m_selectedPath.empty() ? -1 : m_selectedPath[0];
    for (int i = 0; i < (int)m_previewItems.size(); i++) {
        if (auto button = m_previewItems[i])
            button.IsChecked(i == selected);
    }
    for (auto const& [id, button] : m_previewBuiltIns)
        button.IsChecked(id == m_selectedBuiltIn);
}

void SettingsWindow::SelectItem(std::vector<int> const& path) {
    m_selectedBuiltIn = -1;
    m_selectedPath = path;
    if (!ItemAt(m_selectedPath))
        m_selectedPath.clear();
    UpdatePreviewSelection();
    BuildItemEditor();
    UpdateItemToolbar();
}

bool* SettingsWindow::BuiltInHiddenField(int id) {
    if (id == kPreviewSeeMoreId)
        return &m_model.hideMoreButton;
    if (id == kPreviewDetailsId)
        return &m_model.hideDetailsButton;
    if (id >= 0 && id < kDefaultButtonCount)
        return &m_model.hideDefaultButtons[id];
    return nullptr;
}

muxc::AppBarToggleButton SettingsWindow::MakePreviewBuiltIn(
    int id,
    muxc::IconElement const& icon,
    std::wstring const& label,
    bool showLabel,
    bool chevron) {
    muxc::AppBarToggleButton button;
    ApplyPreviewSelectionStyle(button);
    button.Label(label);
    button.Icon(icon);
    button.LabelPosition(showLabel ? muxc::CommandBarLabelPosition::Default
                                   : muxc::CommandBarLabelPosition::Collapsed);
    if (!showLabel)
        MakeCompactPreviewButton(button);
    muxc::ToolTipService::SetToolTip(button, BoxString(label));
    if (showLabel && chevron) {
        button.Loaded([label](wf::IInspectable const& sender,
                              mux::RoutedEventArgs const&) {
            if (auto element = sender.try_as<mux::FrameworkElement>())
                AddChevronToLabel(element, label);
        });
    }
    button.Click(
        [this, id](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            if (IsDragClick())
                return;
            SelectBuiltIn(id);
        });

    m_previewBuiltIns.push_back({id, button});
    return button;
}

void SettingsWindow::SelectBuiltIn(int id) {
    m_selectedBuiltIn = id;
    m_selectedPath.clear();
    UpdatePreviewSelection();
    BuildItemEditor();
    UpdateItemToolbar();
}

// The same cards as on the General page, which also refresh the preview, as
// they show, hide and move the cog.
void SettingsWindow::BuildSettingsItemEditor() {
    auto children = m_editorPanel.Children();
    children.Append(MakeText(L"Settings item", m_secondaryTextStyle));
    muxc::TextBlock title;
    title.Text(BuiltInButtonName(kPreviewSettingsItemId));
    title.FontSize(20);
    title.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
    title.Margin({0, 2, 0, 16});
    children.Append(title);

    for (auto const& page : DescribeSettingsPages(m_model)) {
        for (auto const& field : page.fields) {
            if (field.key != L"showSettingsItem" &&
                field.key != L"settingsItemOrder")
                continue;
            auto control = MakeFieldControl(field);
            if (auto toggle = control.try_as<muxc::ToggleSwitch>()) {
                toggle.Toggled(
                    [this](wf::IInspectable const&,
                           mux::RoutedEventArgs const&) { RebuildPreview(); });
            } else if (auto numberBox = control.try_as<muxc::NumberBox>()) {
                numberBox.ValueChanged(
                    [this](muxc::NumberBox const&,
                           muxc::NumberBoxValueChangedEventArgs const&) {
                        RebuildPreview();
                    });
            }
            children.Append(MakeCard(field.glyph, field.title,
                                     field.description, control, field.wide));
        }
    }
}

void SettingsWindow::BuildBuiltInEditor() {
    auto children = m_editorPanel.Children();
    int id = m_selectedBuiltIn;
    int generation = m_editorGeneration;
    bool* hidden = BuiltInHiddenField(id);
    if (!hidden)
        return;

    children.Append(MakeText(L"Built-in button", m_secondaryTextStyle));
    muxc::TextBlock title;
    title.Text(BuiltInButtonName(id));
    title.FontSize(20);
    title.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
    title.Margin({0, 2, 0, 16});
    children.Append(title);

    children.Append(MakeCard(
        L"", L"This is a built-in File Explorer button",
        L"Its command and look come from File Explorer, so they can't be "
        L"changed here, but the button can be hidden.",
        nullptr, false));

    muxc::ToggleSwitch toggle;
    toggle.IsOn(!*hidden);
    toggle.MinWidth(0);
    toggle.Toggled([this, hidden, generation](wf::IInspectable const& sender,
                                              mux::RoutedEventArgs const&) {
        if (generation != m_editorGeneration)
            return;
        *hidden = !sender.as<muxc::ToggleSwitch>().IsOn();
        RebuildPreview();
        OnModelChanged();
    });
    children.Append(MakeCard(L"", L"Show this button",
                             L"When it's off, the button is hidden from File "
                             L"Explorer and from the preview above. It can be "
                             L"shown again on the Built-in buttons page.",
                             toggle, false));

    bool* labelHidden = nullptr;
    if (id == kPreviewDetailsId) {
        labelHidden = &m_model.hideDetailsButtonLabel;
    } else if (std::find(std::begin(kLabeledDefaultButtons),
                         std::end(kLabeledDefaultButtons),
                         id) != std::end(kLabeledDefaultButtons)) {
        labelHidden = &m_model.hideDefaultButtonLabels[id];
    }
    if (labelHidden) {
        muxc::ToggleSwitch labelToggle;
        labelToggle.IsOn(!*labelHidden);
        labelToggle.MinWidth(0);
        labelToggle.Toggled([this, labelHidden, generation](
                                wf::IInspectable const& sender,
                                mux::RoutedEventArgs const&) {
            if (generation != m_editorGeneration)
                return;
            *labelHidden = !sender.as<muxc::ToggleSwitch>().IsOn();
            RebuildPreview();
            OnModelChanged();
        });
        children.Append(MakeCard(L"", L"Show label",
                                 L"When it's off, only the icon of the button "
                                 L"is shown, and the label becomes its "
                                 L"tooltip.",
                                 labelToggle, false));
    }

    if (id >= 0 && id < kReorderableButtonCount) {
        auto resetButton = MakeTextButton(L"Reset order");
        resetButton.IsEnabled(!m_model.defaultButtonOrder.empty());
        resetButton.Click([this, generation](wf::IInspectable const&,
                                             mux::RoutedEventArgs const&) {
            if (generation != m_editorGeneration)
                return;
            m_model.defaultButtonOrder.clear();
            RefreshItemsPage();
            OnModelChanged();
        });
        children.Append(MakeCard(
            L"", L"Order",
            L"Drag the built-in buttons and their separators in the preview "
            L"above to reorder them. Reset puts them back in File Explorer's "
            L"own order.",
            resetButton, false));
    }

    muxc::HyperlinkButton allButtonsLink;
    allButtonsLink.Content(
        BoxString(L"All built-in buttons, separators and contextual commands"));
    allButtonsLink.Margin({0, 8, 0, 0});
    allButtonsLink.Click(
        [this](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            NavigateTo(L"builtIn");
        });
    children.Append(allButtonsLink);
}

void SettingsWindow::NavigateTo(std::wstring const& tag) {
    // Deferred, since the page being left may host the clicked element.
    auto dispatcherQueue = mud::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue)
        return;
    dispatcherQueue.TryEnqueue([this, tag]() {
        if (g_settingsWindow != this)
            return;
        for (auto const& [navTag, navButton] : m_navButtons) {
            if (navTag == tag)
                navButton.IsChecked(true);
        }
    });
}

int CountToolbarItems(EditableSettings const& settings) {
    return (int)std::count_if(
        settings.items.begin(), settings.items.end(),
        [](ActionItem const& item) { return !IsEmptyActionItem(item); });
}

// The position of the settings item among the toolbar items, from 0 (before
// the first one) to the item count (after the last one).
int SettingsItemPosition(EditableSettings const& settings) {
    int count = CountToolbarItems(settings);
    int order = settings.settingsItemOrder;
    return order < 0 || order >= count ? count : order;
}

void SettingsWindow::UpdateItemToolbar() {
    if (!m_moveUpButton)
        return;
    bool hasSelection = ItemAt(m_selectedPath) != nullptr;
    int index = hasSelection ? m_selectedPath.back() : -1;
    auto* siblings = hasSelection ? SiblingsOf(m_selectedPath) : nullptr;
    bool isToolbarItem = m_selectedPath.size() <= 1;
    m_moveUpButton.Label(isToolbarItem ? L"Move left" : L"Move up");
    m_moveUpButton.Icon(CreateGlyphIcon(isToolbarItem ? L"" : L""));
    m_moveDownButton.Label(isToolbarItem ? L"Move right" : L"Move down");
    m_moveDownButton.Icon(CreateGlyphIcon(isToolbarItem ? L"" : L""));
    m_moveUpButton.IsEnabled(hasSelection && index > 0);
    m_moveDownButton.IsEnabled(hasSelection && siblings &&
                               index + 1 < (int)siblings->size());
    m_duplicateButton.IsEnabled(hasSelection);
    m_deleteButton.IsEnabled(hasSelection);
    m_addEntryItem.IsEnabled(hasSelection);
    m_addSubmenuItem.IsEnabled(hasSelection);

    if (m_selectedBuiltIn == kPreviewSettingsItemId &&
        m_model.showSettingsItem) {
        int position = SettingsItemPosition(m_model);
        m_moveUpButton.IsEnabled(position > 0);
        m_moveDownButton.IsEnabled(position < CountToolbarItems(m_model));
    }
}

void SettingsWindow::DeferItemsPageRefresh() {
    auto dispatcherQueue = mud::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue)
        return;
    dispatcherQueue.TryEnqueue([this]() {
        if (g_settingsWindow == this)
            RefreshItemsPage();
    });
}

void SettingsWindow::OnItemEdited() {
    if (ActionItem* item = ItemAt(m_selectedPath)) {
        int depth = (int)m_selectedPath.size() - 1;
        if (m_editorIconPreview)
            m_editorIconPreview.Child(MakeItemIcon(*item, depth, 20));
    }
    RebuildPreview();
    OnModelChanged();
}

void SettingsWindow::BuildItemEditor() {
    if (!m_editorPanel)
        return;
    m_editorGeneration++;
    m_editorIconPreview = nullptr;
    ClearDragGroup(kDragGroupEntries);
    auto children = m_editorPanel.Children();
    children.Clear();

    if (m_selectedBuiltIn == kPreviewSettingsItemId) {
        BuildSettingsItemEditor();
        return;
    }
    if (m_selectedBuiltIn >= 0) {
        BuildBuiltInEditor();
        return;
    }

    ActionItem* item = ItemAt(m_selectedPath);
    if (!item) {
        muxc::StackPanel placeholder;
        placeholder.Spacing(12);
        placeholder.Margin({0, 72, 0, 0});
        placeholder.HorizontalAlignment(mux::HorizontalAlignment::Center);
        auto icon = MakeGlyph(L"\uE70F", 40);
        icon.Opacity(0.6);
        placeholder.Children().Append(icon);
        auto text =
            MakeText(m_model.items.empty()
                         ? L"There are no toolbar items yet. Use Add to "
                           L"create one."
                         : L"Click one of your items in the command "
                           L"bar above to edit it.",
                     m_secondaryTextStyle);
        text.FontSize(14);
        text.TextAlignment(mux::TextAlignment::Center);
        placeholder.Children().Append(text);
        children.Append(placeholder);
        return;
    }

    int depth = (int)m_selectedPath.size() - 1;
    std::vector<int> path = m_selectedPath;
    int generation = m_editorGeneration;

    // Selecting rebuilds this editor, so clicks inside it select later.
    auto deferSelect = [this](std::vector<int> target) {
        auto dispatcherQueue = mud::DispatcherQueue::GetForCurrentThread();
        if (!dispatcherQueue)
            return;
        dispatcherQueue.TryEnqueue([this, target]() {
            if (g_settingsWindow == this && m_editorPanel)
                SelectItem(target);
        });
    };

    // The parents link back to themselves.
    muxc::StackPanel breadcrumb;
    breadcrumb.Orientation(muxc::Orientation::Horizontal);
    breadcrumb.Spacing(6);
    std::vector<int> partialPath;
    for (size_t i = 0; i < path.size(); i++) {
        partialPath.push_back(path[i]);
        ActionItem* pathItem = ItemAt(partialPath);
        if (!pathItem)
            break;
        std::wstring name =
            pathItem->name.empty() ? L"Untitled" : pathItem->name;
        if (i + 1 < path.size()) {
            muxc::HyperlinkButton link;
            link.Content(BoxString(name));
            link.Padding({0, 0, 0, 0});
            link.MinHeight(0);
            link.MinWidth(0);
            link.FontSize(12);
            link.VerticalAlignment(mux::VerticalAlignment::Center);
            link.Click([deferSelect, partialPath](wf::IInspectable const&,
                                                  mux::RoutedEventArgs const&) {
                deferSelect(partialPath);
            });
            breadcrumb.Children().Append(link);
            auto separator = MakeText(L"\u203A", m_secondaryTextStyle);
            separator.VerticalAlignment(mux::VerticalAlignment::Center);
            breadcrumb.Children().Append(separator);
        } else {
            auto current = MakeText(name, m_secondaryTextStyle);
            current.VerticalAlignment(mux::VerticalAlignment::Center);
            breadcrumb.Children().Append(current);
        }
    }
    children.Append(breadcrumb);
    muxc::TextBlock kindText;
    kindText.Text(ItemKindName(*item, depth));
    kindText.FontSize(20);
    kindText.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
    kindText.Margin({0, 2, 0, 16});
    children.Append(kindText);

    auto onText = [this, path, generation](
                      std::function<void(ActionItem&, std::wstring)> apply) {
        return [this, path, generation, apply](
                   wf::IInspectable const& sender,
                   muxc::TextChangedEventArgs const&) {
            if (ActionItem* item = EditedItem(path, generation)) {
                apply(*item, std::wstring(sender.as<muxc::TextBox>().Text()));
                OnItemEdited();
            }
        };
    };
    auto onToggle = [this, path,
                     generation](std::function<void(ActionItem&, bool)> apply) {
        return [this, path, generation, apply](wf::IInspectable const& sender,
                                               mux::RoutedEventArgs const&) {
            if (ActionItem* item = EditedItem(path, generation)) {
                apply(*item, sender.as<muxc::ToggleSwitch>().IsOn());
                OnItemEdited();
            }
        };
    };
    auto makeToggle = [](bool isOn) {
        muxc::ToggleSwitch toggle;
        toggle.IsOn(isOn);
        toggle.MinWidth(0);
        return toggle;
    };
    auto makeFlyoutButton =
        [](PCWSTR text, winrt::weak_ref<muxc::TextBox> weakTextBox,
           std::vector<std::pair<std::wstring, std::wstring>> const& entries,
           bool insertAtCaret) {
            muxc::DropDownButton button;
            button.Content(BoxString(text));
            muxc::MenuFlyout menu;
            for (auto const& [value, label] : entries) {
                muxc::MenuFlyoutItem menuItem;
                menuItem.Text(label);
                menuItem.Click(
                    [weakTextBox, value, insertAtCaret](
                        wf::IInspectable const&, mux::RoutedEventArgs const&) {
                        auto textBox = weakTextBox.get();
                        if (!textBox)
                            return;
                        if (insertAtCaret)
                            textBox.SelectedText(value);
                        else
                            textBox.Text(value);
                        textBox.Focus(mux::FocusState::Programmatic);
                    });
                menu.Items().Append(menuItem);
            }
            button.Flyout(menu);
            return button;
        };

    if (depth < kMaxMenuDepth) {
        muxc::ComboBox typeBox;
        typeBox.MinWidth(200);
        typeBox.Items().Append(BoxString(L"Single button"));
        typeBox.Items().Append(
            BoxString(depth == 0 ? L"Dropdown menu" : L"Submenu"));
        typeBox.SelectedIndex(item->isMenu ? 1 : 0);
        typeBox.SelectionChanged(
            [this, path, generation](wf::IInspectable const& sender,
                                     muxc::SelectionChangedEventArgs const&) {
                ActionItem* item = EditedItem(path, generation);
                if (!item)
                    return;
                bool isMenu = sender.as<muxc::ComboBox>().SelectedIndex() == 1;
                if (item->isMenu == isMenu)
                    return;
                item->isMenu = isMenu;
                if (isMenu && item->subItems.empty()) {
                    item->subItems.push_back(
                        MakeNewActionItem(false, (int)path.size()));
                }
                OnModelChanged();
                DeferItemsPageRefresh();
            });
        children.Append(MakeCard(
            L"\uE8FD", L"Type",
            depth == 0 ? L"A single button runs a command, a dropdown menu "
                         L"opens a list of entries."
                       : L"A menu entry runs a command, a submenu opens a "
                         L"nested list of entries.",
            typeBox, false));
    }

    auto nameBox = MakeTextBox(item->name, L"Name");
    nameBox.TextChanged(
        onText([](ActionItem& item, std::wstring text) { item.name = text; }));
    children.Append(MakeCard(
        L"\uE8AC", L"Name",
        depth == 0 ? L"Shown as the button tooltip." : L"The menu item text.",
        nameBox, true));

    {
        auto commandBox = MakeTextBox(item->command,
                                      L"e.g. wt.exe or internal:TogglePreview");
        commandBox.TextChanged(onText(
            [](ActionItem& item, std::wstring text) { item.command = text; }));
        auto weakCommandBox = winrt::make_weak(commandBox);

        auto browseButton = MakeTextButton(L"Browse\u2026");
        browseButton.Click([this, weakCommandBox](wf::IInspectable const&,
                                                  mux::RoutedEventArgs const&) {
            COMDLG_FILTERSPEC filters[] = {
                {L"Programs", L"*.exe;*.cmd;*.bat;*.com;*.ps1"},
                {L"All files", L"*.*"},
            };
            std::wstring path =
                Browse(L"Choose a program", filters, ARRAYSIZE(filters));
            if (path.empty())
                return;
            if (auto textBox = weakCommandBox.get())
                textBox.Text(path);
        });

        auto internalButton = makeFlyoutButton(
            L"Internal", weakCommandBox,
            {
                {L"internal:TogglePreview", L"Toggle the Preview pane"},
                {L"internal:ToggleDetails", L"Toggle the Details pane"},
                {L"internal:OpenWith", L"Open with\u2026"},
                {L"internal:FolderOptions", L"Folder Options"},
                {L"internal:CopyPath", L"Copy the selected paths"},
                {L"internal:CopyName", L"Copy the selected names"},
                {L"internal:CopyDir", L"Copy the current folder path"},
                {L"internal:NewFolder", L"New folder"},
                {L"internal:NewFile", L"New file"},
                {L"internal:Refresh", L"Refresh"},
            },
            false);

        muxc::Grid commandRow;
        commandRow.ColumnSpacing(8);
        AddGridColumn(commandRow, StarLength());
        AddGridColumn(commandRow, AutoLength());
        AddGridColumn(commandRow, AutoLength());
        commandRow.Children().Append(commandBox);
        muxc::Grid::SetColumn(browseButton, 1);
        commandRow.Children().Append(browseButton);
        muxc::Grid::SetColumn(internalButton, 2);
        commandRow.Children().Append(internalButton);

        children.Append(MakeCard(
            L"\uE756", L"Command",
            item->isMenu && depth < kMaxMenuDepth
                ? L"For menus the command is only used as an icon source."
                : L"The executable to run, or an internal File Explorer "
                  L"command.",
            commandRow, true));
    }

    {
        auto parametersBox =
            MakeTextBox(item->parameters, L"e.g. -d \"%path%\"");
        parametersBox.TextChanged(
            onText([](ActionItem& item, std::wstring text) {
                item.parameters = text;
            }));
        auto insertButton = makeFlyoutButton(
            L"Insert", winrt::make_weak(parametersBox),
            {
                {L"\"%path%\"", L"%path% \u2013 the current folder"},
                {L"\"%sel%\"", L"%sel% \u2013 all selected items, quoted"},
                {L"\"%sel_each%\"",
                 L"%sel_each% \u2013 run once per selected item"},
            },
            true);

        muxc::Grid parametersRow;
        parametersRow.ColumnSpacing(8);
        AddGridColumn(parametersRow, StarLength());
        AddGridColumn(parametersRow, AutoLength());
        parametersRow.Children().Append(parametersBox);
        muxc::Grid::SetColumn(insertButton, 1);
        parametersRow.Children().Append(insertButton);

        children.Append(MakeCard(
            L"\uE943", L"Parameters",
            L"If a used placeholder has no value (e.g. nothing is selected, or "
            L"a location like This PC), the command is launched without "
            L"parameters.",
            parametersRow, true));
    }

    if (!item->isMenu || depth >= kMaxMenuDepth) {
        auto shortcutBox = MakeTextBox(item->shortcut, L"e.g. Ctrl+Shift+T");
        shortcutBox.TextChanged(onText(
            [](ActionItem& item, std::wstring text) { item.shortcut = text; }));
        children.Append(MakeCard(
            L"", L"Keyboard shortcut",
            L"Runs the item, like a click, when the keys are pressed in a "
            L"File Explorer window, e.g. Ctrl+Shift+T, Alt+X or F9. Needs "
            L"Ctrl, Alt or Win unless the key is F1-F24.",
            shortcutBox, true));

        auto runAsAdminToggle = makeToggle(item->runAsAdmin);
        runAsAdminToggle.Toggled(onToggle(
            [](ActionItem& item, bool isOn) { item.runAsAdmin = isOn; }));
        children.Append(MakeCard(
            L"", L"Run as administrator",
            L"Start the command elevated, after a UAC prompt. Ctrl+Shift+click "
            L"does the same for any item. Not used by internal commands.",
            runAsAdminToggle, false));

        muxc::ComboBox windowStyleBox;
        windowStyleBox.MinWidth(200);
        for (auto const& style : kWindowStyles)
            windowStyleBox.Items().Append(BoxString(style.title));
        windowStyleBox.SelectedIndex(WindowStyleIndex(item->windowStyle));
        windowStyleBox.SelectionChanged(
            [this, path, generation](wf::IInspectable const& sender,
                                     muxc::SelectionChangedEventArgs const&) {
                ActionItem* item = EditedItem(path, generation);
                int index = sender.as<muxc::ComboBox>().SelectedIndex();
                if (!item || index < 0 ||
                    index >= (int)ARRAYSIZE(kWindowStyles))
                    return;
                item->windowStyle = kWindowStyles[index].settingValue;
                OnItemEdited();
            });
        children.Append(MakeCard(
            L"", L"Window style",
            L"How the started program's window is shown. Hidden is meant for "
            L"console commands and scripts; some programs ignore it.",
            windowStyleBox, false));
    }

    {
        m_editorIconPreview = muxc::Border();
        if (auto style = FindStyle(L"IconPreviewStyle"))
            m_editorIconPreview.Style(style);
        m_editorIconPreview.Width(32);
        m_editorIconPreview.Height(32);
        m_editorIconPreview.Child(MakeItemIcon(*item, depth, 20));

        auto iconBox = MakeTextBox(item->icon, L"e.g. E756");
        iconBox.TextChanged(onText(
            [](ActionItem& item, std::wstring text) { item.icon = text; }));
        auto weakIconBox = winrt::make_weak(iconBox);

        auto browseButton = MakeTextButton(L"Browse\u2026");
        browseButton.Click([this, weakIconBox](wf::IInspectable const&,
                                               mux::RoutedEventArgs const&) {
            COMDLG_FILTERSPEC filters[] = {
                {L"Icons and programs", L"*.ico;*.exe;*.dll"},
                {L"All files", L"*.*"},
            };
            std::wstring path =
                Browse(L"Choose an icon", filters, ARRAYSIZE(filters));
            if (path.empty())
                return;
            if (auto textBox = weakIconBox.get())
                textBox.Text(path);
        });

        auto glyphsButton = MakeTextButton(L"Glyph list");
        glyphsButton.Click(
            [](wf::IInspectable const&, mux::RoutedEventArgs const&) {
                OpenUrl(kGlyphListUrl);
            });

        muxc::Grid iconRow;
        iconRow.ColumnSpacing(8);
        AddGridColumn(iconRow, AutoLength());
        AddGridColumn(iconRow, StarLength());
        AddGridColumn(iconRow, AutoLength());
        AddGridColumn(iconRow, AutoLength());
        iconRow.Children().Append(m_editorIconPreview);
        muxc::Grid::SetColumn(iconBox, 1);
        iconRow.Children().Append(iconBox);
        muxc::Grid::SetColumn(browseButton, 2);
        iconRow.Children().Append(browseButton);
        muxc::Grid::SetColumn(glyphsButton, 3);
        iconRow.Children().Append(glyphsButton);

        children.Append(MakeCard(
            L"\uE8B9", L"Icon glyph or icon path",
            L"A Segoe Fluent Icons code point (e.g. E756), an .exe, .dll or "
            L".ico path with an optional icon index (e.g. "
            L"C:\\Windows\\System32\\shell32.dll,3), or "
            L"shell:AppsFolder\\<AppUserModelID>. Separate two values with | "
            L"for Light | Dark theme icons. Leave empty to use the icon of "
            L"the command's executable.",
            iconRow, true));
    }

    auto hideIconToggle = makeToggle(item->hideIcon);
    hideIconToggle.Toggled(
        onToggle([](ActionItem& item, bool isOn) { item.hideIcon = isOn; }));
    children.Append(MakeCard(L"\uED1A", L"Hide icon",
                             L"Don't show an icon for this item.",
                             hideIconToggle, false));

    if (depth == 0) {
        auto filterBox = MakeTextBox(item->matchExtensions,
                                     L"e.g. single_file, folders, -zip");
        filterBox.TextChanged(onText([](ActionItem& item, std::wstring text) {
            item.matchExtensions = text;
        }));
        children.Append(
            MakeCard(L"", L"File extension filter",
                     L"Rules evaluate using OR logic. Use '+' for AND (e.g. "
                     L"files + single). Prefix with '-' to exclude (e.g. "
                     L"-exe). Use 'files', 'folders', 'single', 'multi', 'only "
                     L"txt', or 'hide'. Leave blank to always show.",
                     filterBox, true));

        auto showLabelToggle = makeToggle(item->showLabel);
        showLabelToggle.Toggled(onToggle(
            [](ActionItem& item, bool isOn) { item.showLabel = isOn; }));
        children.Append(MakeCard(L"\uE8E9", L"Show label",
                                 L"Show a text label next to the icon.",
                                 showLabelToggle, false));

        auto labelBox =
            MakeTextBox(item->labelText, L"Uses the name when empty");
        labelBox.Width(280);
        labelBox.TextChanged(onText([](ActionItem& item, std::wstring text) {
            item.labelText = text;
        }));
        children.Append(MakeCard(L"\uE8D2", L"Custom label text",
                                 L"Text displayed next to the icon. If empty, "
                                 L"the name is used.",
                                 labelBox, false));
    }

    auto separatorToggle = makeToggle(item->separatorAfter);
    separatorToggle.Toggled(onToggle(
        [](ActionItem& item, bool isOn) { item.separatorAfter = isOn; }));
    children.Append(MakeCard(
        L"\uE76F",
        depth == 0 ? L"Vertical separator after" : L"Separator after",
        depth == 0 ? L"Show a vertical separator line after this button."
                   : L"Show a separator line below this menu item.",
        separatorToggle, false));

    if (item->isMenu && depth < kMaxMenuDepth) {
        auto header = MakeText(L"Menu entries", m_sectionHeaderStyle);
        if (!m_sectionHeaderStyle) {
            header.FontWeight(winrt::Windows::UI::Text::FontWeight{600});
            header.Margin({1, 28, 0, 8});
        }
        children.Append(header);
        if (item->subItems.size() > 1) {
            auto hint = MakeText(L"Drag the entries to reorder them.",
                                 m_secondaryTextStyle);
            hint.Margin({1, -4, 0, 8});
            children.Append(hint);
        }

        std::vector<mux::UIElement> entryButtons;
        for (int i = 0; i < (int)item->subItems.size(); i++) {
            auto const& entry = item->subItems[i];
            std::vector<int> entryPath = path;
            entryPath.push_back(i);

            muxc::Grid row;
            row.ColumnSpacing(16);
            AddGridColumn(row, AutoLength());
            AddGridColumn(row, StarLength());
            AddGridColumn(row, AutoLength());
            auto icon = MakeItemIcon(entry, depth + 1, 20);
            icon.VerticalAlignment(mux::VerticalAlignment::Center);
            row.Children().Append(icon);

            muxc::StackPanel text;
            text.VerticalAlignment(mux::VerticalAlignment::Center);
            muxc::TextBlock title;
            title.Text(entry.name.empty() ? L"Untitled" : entry.name);
            title.TextTrimming(mux::TextTrimming::CharacterEllipsis);
            text.Children().Append(title);
            auto subtitle = MakeText(DescribeActionItem(entry, depth + 1),
                                     m_secondaryTextStyle);
            subtitle.TextWrapping(mux::TextWrapping::NoWrap);
            subtitle.TextTrimming(mux::TextTrimming::CharacterEllipsis);
            text.Children().Append(subtitle);
            muxc::Grid::SetColumn(text, 1);
            row.Children().Append(text);

            auto chevron = MakeGlyph(L"\uE76C", 12);
            chevron.VerticalAlignment(mux::VerticalAlignment::Center);
            muxc::Grid::SetColumn(chevron, 2);
            row.Children().Append(chevron);

            muxc::Button entryButton;
            entryButton.Content(row);
            entryButton.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
            entryButton.HorizontalContentAlignment(
                mux::HorizontalAlignment::Stretch);
            entryButton.Padding({16, 10, 16, 10});
            entryButton.Margin({0, 0, 0, 4});
            entryButton.Click(
                [this, deferSelect, entryPath](wf::IInspectable const&,
                                               mux::RoutedEventArgs const&) {
                    if (IsDragClick())
                        return;
                    deferSelect(entryPath);
                });
            children.Append(entryButton);
            entryButtons.push_back(entryButton);
        }
        SetDragGroup(kDragGroupEntries, m_editorPanel, std::move(entryButtons),
                     [this, path](int from, int to) {
                         ReorderMenuEntries(path, from, to);
                     });

        auto addEntryButton = MakeTextButton(L"Add entry");
        addEntryButton.Margin({0, 4, 0, 0});
        addEntryButton.Click([this, generation](wf::IInspectable const&,
                                                mux::RoutedEventArgs const&) {
            if (generation == m_editorGeneration)
                AddMenuEntry(false);
        });
        children.Append(addEntryButton);
    }
}

// How far the pointer has to move before a press becomes a drag.
constexpr double kDragThreshold = 6;

void SettingsWindow::SetDragGroup(int group,
                                  mux::UIElement const& reference,
                                  std::vector<mux::UIElement> elements,
                                  std::function<void(int, int)> commit) {
    ClearDragGroup(group);
    auto& state = m_dragGroups[group];
    state.reference = reference;
    state.elements = std::move(elements);
    state.commit = std::move(commit);
    for (int i = 0; i < (int)state.elements.size(); i++)
        EnableDragReorder(state.elements[i], group, i);
}

void SettingsWindow::ClearDragGroup(int group) {
    if (m_drag.group == group)
        m_drag = DragState{};
    m_dragGroups[group] = DragGroupState{};
}

void SettingsWindow::EnableDragReorder(mux::UIElement const& element,
                                       int group,
                                       int slot) {
    using mux::Input::PointerEventHandler;
    using mux::Input::PointerRoutedEventArgs;

    // Buttons handle the pointer events themselves, so the handlers are
    // added for handled events too.
    auto add = [&element](mux::RoutedEvent const& routedEvent,
                          auto&& callback) {
        PointerEventHandler handler(callback);
        element.AddHandler(routedEvent, winrt::box_value(handler), true);
    };
    add(mux::UIElement::PointerPressedEvent(),
        [this, group, slot](wf::IInspectable const& sender,
                            PointerRoutedEventArgs const& args) {
            if (auto element = sender.try_as<mux::UIElement>())
                OnDragPointerPressed(element, args, group, slot);
        });
    add(mux::UIElement::PointerMovedEvent(),
        [this, group, slot](wf::IInspectable const& sender,
                            PointerRoutedEventArgs const& args) {
            if (auto element = sender.try_as<mux::UIElement>())
                OnDragPointerMoved(element, args, group, slot);
        });
    add(mux::UIElement::PointerReleasedEvent(),
        [this](wf::IInspectable const&, PointerRoutedEventArgs const&) {
            EndDrag(true);
        });
    // Buttons release the capture when the button is released, which can
    // come before the released event.
    add(mux::UIElement::PointerCaptureLostEvent(),
        [this](wf::IInspectable const&, PointerRoutedEventArgs const&) {
            EndDrag(true);
        });
    add(mux::UIElement::PointerCanceledEvent(),
        [this](wf::IInspectable const&, PointerRoutedEventArgs const&) {
            EndDrag(false);
        });
}

void SettingsWindow::OnDragPointerPressed(
    mux::UIElement const& element,
    mux::Input::PointerRoutedEventArgs const& args,
    int group,
    int slot) {
    auto const& state = m_dragGroups[group];
    if (!state.reference || slot >= (int)state.elements.size() ||
        state.elements[slot] != element) {
        return;
    }
    auto point = args.GetCurrentPoint(state.reference);
    if (!point.Properties().IsLeftButtonPressed())
        return;

    if (m_drag.active)
        EndDrag(false);
    m_drag = DragState{};
    m_drag.pressed = true;
    m_drag.group = group;
    m_drag.slot = slot;
    m_drag.target = slot;
    m_drag.pointerId = args.Pointer().PointerId();
    m_drag.pressPoint = point.Position();
}

void SettingsWindow::OnDragPointerMoved(
    mux::UIElement const& element,
    mux::Input::PointerRoutedEventArgs const& args,
    int group,
    int slot) {
    if (!m_drag.pressed || m_drag.group != group || m_drag.slot != slot ||
        m_drag.pointerId != args.Pointer().PointerId()) {
        return;
    }
    auto const& state = m_dragGroups[group];
    if (!state.reference)
        return;

    auto point = args.GetCurrentPoint(state.reference);
    if (!point.Properties().IsLeftButtonPressed()) {
        EndDrag(true);
        return;
    }

    auto position = point.Position();
    bool vertical = group == kDragGroupEntries;
    double delta = vertical ? position.Y - m_drag.pressPoint.Y
                            : position.X - m_drag.pressPoint.X;
    if (!m_drag.active) {
        if (std::abs(delta) < kDragThreshold)
            return;
        if (!BeginDrag(element, args)) {
            m_drag = DragState{};
            return;
        }
    }
    UpdateDrag(delta);
}

bool SettingsWindow::BeginDrag(mux::UIElement const& element,
                               mux::Input::PointerRoutedEventArgs const& args) {
    auto const& state = m_dragGroups[m_drag.group];
    if (state.elements.size() < 2)
        return false;

    bool vertical = m_drag.group == kDragGroupEntries;
    m_drag.starts.clear();
    m_drag.sizes.clear();
    try {
        for (auto const& item : state.elements) {
            auto frameworkElement = item.try_as<mux::FrameworkElement>();
            if (!frameworkElement)
                return false;
            auto origin = item.TransformToVisual(state.reference)
                              .TransformPoint(wf::Point{0, 0});
            m_drag.starts.push_back(vertical ? origin.Y : origin.X);
            m_drag.sizes.push_back(vertical ? frameworkElement.ActualHeight()
                                            : frameworkElement.ActualWidth());
        }
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
        return false;
    }

    m_drag.active = true;
    element.CapturePointer(args.Pointer());
    muxc::Canvas::SetZIndex(element, 1);
    element.Opacity(0.85);
    return true;
}

void SettingsWindow::UpdateDrag(double delta) {
    auto const& state = m_dragGroups[m_drag.group];
    auto const& starts = m_drag.starts;
    auto const& sizes = m_drag.sizes;
    int count = (int)state.elements.size();
    int from = m_drag.slot;
    if (count != (int)starts.size() || from < 0 || from >= count)
        return;
    bool vertical = m_drag.group == kDragGroupEntries;

    // The dragged element stays between the first and the last one.
    double minDelta = starts[0] - starts[from];
    double maxDelta =
        starts[count - 1] + sizes[count - 1] - (starts[from] + sizes[from]);
    delta = std::clamp(delta, std::min(minDelta, 0.0), std::max(maxDelta, 0.0));

    double center = starts[from] + sizes[from] / 2 + delta;
    int target = 0;
    for (int i = 0; i < count; i++) {
        if (i != from && starts[i] + sizes[i] / 2 < center)
            target++;
    }

    // The others make room for it, by its size and the gap between items.
    double gap = 0;
    if (from + 1 < count)
        gap = starts[from + 1] - (starts[from] + sizes[from]);
    else if (from > 0)
        gap = starts[from] - (starts[from - 1] + sizes[from - 1]);
    double extent = sizes[from] + std::clamp(gap, 0.0, 8.0);

    for (int i = 0; i < count; i++) {
        double offset = 0;
        if (i == from)
            offset = delta;
        else if (from < i && i <= target)
            offset = -extent;
        else if (target <= i && i < from)
            offset = extent;

        auto const& element = state.elements[i];
        auto transform =
            element.RenderTransform().try_as<muxm::TranslateTransform>();
        if (!transform) {
            if (offset == 0)
                continue;
            transform = muxm::TranslateTransform();
            element.RenderTransform(transform);
        }
        if (vertical)
            transform.Y(offset);
        else
            transform.X(offset);
    }
    m_drag.target = target;
}

void SettingsWindow::ResetDragVisuals(int group) {
    for (auto const& element : m_dragGroups[group].elements) {
        try {
            element.RenderTransform(nullptr);
            element.Opacity(1);
            muxc::Canvas::SetZIndex(element, 0);
        } catch (...) {
        }
    }
}

void SettingsWindow::EndDrag(bool commit) {
    if (!m_drag.pressed)
        return;
    DragState drag = std::move(m_drag);
    m_drag = DragState{};
    if (!drag.active)
        return;

    ResetDragVisuals(drag.group);

    // The button may still raise its click when released, which is ignored
    // until the queued work below runs.
    m_suppressDragClick = true;
    std::function<void(int, int)> commitFn;
    if (commit && drag.target != drag.slot)
        commitFn = m_dragGroups[drag.group].commit;

    auto dispatcherQueue = mud::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue) {
        m_suppressDragClick = false;
        return;
    }
    dispatcherQueue.TryEnqueue(
        [this, commitFn, from = drag.slot, to = drag.target]() {
            if (g_settingsWindow != this)
                return;
            m_suppressDragClick = false;
            if (commitFn)
                commitFn(from, to);
            else
                UpdatePreviewSelection();
        });
}

// The order lists the item index of each dragged element, or -1 for the
// settings item, whose position is kept in its order setting.
void SettingsWindow::ReorderToolbarItems(std::vector<int> order,
                                         int from,
                                         int to) {
    int count = (int)order.size();
    if (from < 0 || from >= count || to < 0 || to >= count)
        return;
    int moved = order[from];
    order.erase(order.begin() + from);
    order.insert(order.begin() + to, moved);

    auto& items = m_model.items;
    std::vector<ActionItem> reordered;
    std::vector<int> newIndexOf(items.size(), -1);
    int settingsPosition = -1;
    for (int index : order) {
        if (index < 0) {
            settingsPosition = (int)reordered.size();
            continue;
        }
        if (index >= (int)items.size() || newIndexOf[index] >= 0)
            continue;
        newIndexOf[index] = (int)reordered.size();
        reordered.push_back(items[index]);
    }
    int toolbarItemCount = (int)reordered.size();
    // Empty items aren't shown, they stay last.
    for (int i = 0; i < (int)items.size(); i++) {
        if (newIndexOf[i] < 0) {
            newIndexOf[i] = (int)reordered.size();
            reordered.push_back(items[i]);
        }
    }
    items = std::move(reordered);

    if (settingsPosition >= 0) {
        m_model.settingsItemOrder =
            settingsPosition >= toolbarItemCount ? -1 : settingsPosition;
    }

    if (moved >= 0) {
        m_selectedBuiltIn = -1;
        m_selectedPath = {newIndexOf[moved]};
    } else {
        m_selectedBuiltIn = kPreviewSettingsItemId;
        m_selectedPath.clear();
    }
    RefreshItemsPage();
    OnModelChanged();
}

// The slots list the position in the built-in button order of each dragged
// element. The buttons and separators which aren't shown, so can't be
// dragged, keep their positions.
void SettingsWindow::ReorderBuiltInButtons(std::vector<size_t> const& slots,
                                           int from,
                                           int to) {
    int count = (int)slots.size();
    if (from < 0 || from >= count || to < 0 || to >= count)
        return;

    std::vector<int> order = ParseDefaultButtonOrder(m_model.defaultButtonOrder);
    if (order.empty())
        order = DefaultButtonOrderTokens();

    std::vector<int> shown;
    for (size_t slot : slots) {
        if (slot >= order.size())
            return;
        shown.push_back(order[slot]);
    }
    int moved = shown[from];
    shown.erase(shown.begin() + from);
    shown.insert(shown.begin() + to, moved);
    for (int i = 0; i < count; i++)
        order[slots[i]] = shown[i];

    m_model.defaultButtonOrder = FormatDefaultButtonOrder(order);

    bool newReplaced =
        m_model.newPlus.enabled && !m_model.newPlus.keepOriginalNewButton;
    if (moved != kOrderSeparator &&
        !(moved == kNewButtonIndex && newReplaced)) {
        m_selectedBuiltIn = moved;
        m_selectedPath.clear();
    }
    RefreshItemsPage();
    OnModelChanged();
}

void SettingsWindow::ReorderMenuEntries(std::vector<int> const& parentPath,
                                        int from,
                                        int to) {
    ActionItem* parent = ItemAt(parentPath);
    if (!parent)
        return;
    auto& entries = parent->subItems;
    int count = (int)entries.size();
    if (from < 0 || from >= count || to < 0 || to >= count || from == to)
        return;
    ActionItem moved = std::move(entries[from]);
    entries.erase(entries.begin() + from);
    entries.insert(entries.begin() + to, std::move(moved));

    m_selectedBuiltIn = -1;
    m_selectedPath = parentPath;
    RefreshItemsPage();
    OnModelChanged();
}

void SettingsWindow::AddTopLevelItem(bool isMenu) {
    auto& items = m_model.items;
    size_t index = m_selectedPath.empty()
                       ? items.size()
                       : std::min<size_t>(m_selectedPath[0] + 1, items.size());
    items.insert(items.begin() + index, MakeNewActionItem(isMenu, 0));
    m_selectedPath = {(int)index};
    RefreshItemsPage();
    OnModelChanged();
}

void SettingsWindow::AddMenuEntry(bool isMenu) {
    ActionItem* selected = ItemAt(m_selectedPath);
    if (!selected)
        return;
    int depth = (int)m_selectedPath.size() - 1;

    std::vector<int> parentPath;
    size_t insertIndex;
    if (depth == 0 || (depth == 1 && selected->isMenu && !isMenu)) {
        selected->isMenu = true;
        parentPath = m_selectedPath;
        insertIndex = selected->subItems.size();
    } else {
        parentPath.assign(m_selectedPath.begin(), m_selectedPath.end() - 1);
        insertIndex = m_selectedPath.back() + 1;
    }

    // Submenus only exist on the second level, so with a third level entry
    // selected, the new submenu goes after its parent.
    if (isMenu && parentPath.size() == 2) {
        insertIndex = parentPath.back() + 1;
        parentPath.pop_back();
    }

    auto* siblings = ChildrenOf(parentPath);
    if (!siblings)
        return;
    insertIndex = std::min(insertIndex, siblings->size());
    siblings->insert(siblings->begin() + insertIndex,
                     MakeNewActionItem(isMenu, (int)parentPath.size()));
    m_selectedPath = parentPath;
    m_selectedPath.push_back((int)insertIndex);
    RefreshItemsPage();
    OnModelChanged();
}

void SettingsWindow::MoveSelectedItem(int delta) {
    // Moving the settings item only changes its order setting. Past the last
    // item it's -1, the default, so it stays last when items are added.
    if (m_selectedBuiltIn == kPreviewSettingsItemId) {
        int count = CountToolbarItems(m_model);
        int position = SettingsItemPosition(m_model) + delta;
        if (!m_model.showSettingsItem || position < 0 || position > count)
            return;
        m_model.settingsItemOrder = position >= count ? -1 : position;
        RebuildPreview();
        BuildItemEditor();
        UpdateItemToolbar();
        OnModelChanged();
        return;
    }

    if (!ItemAt(m_selectedPath))
        return;
    auto* siblings = SiblingsOf(m_selectedPath);
    int index = m_selectedPath.back();
    int newIndex = index + delta;
    if (newIndex < 0 || newIndex >= (int)siblings->size())
        return;
    std::swap((*siblings)[index], (*siblings)[newIndex]);
    m_selectedPath.back() = newIndex;
    RefreshItemsPage();
    OnModelChanged();
}

void SettingsWindow::DuplicateSelectedItem() {
    ActionItem* selected = ItemAt(m_selectedPath);
    if (!selected)
        return;
    ActionItem copy = *selected;
    auto* siblings = SiblingsOf(m_selectedPath);
    int index = m_selectedPath.back() + 1;
    siblings->insert(siblings->begin() + index, std::move(copy));
    m_selectedPath.back() = index;
    RefreshItemsPage();
    OnModelChanged();
}

void SettingsWindow::DeleteSelectedItem() {
    if (!ItemAt(m_selectedPath))
        return;
    auto* siblings = SiblingsOf(m_selectedPath);
    int index = m_selectedPath.back();
    siblings->erase(siblings->begin() + index);
    if (!siblings->empty()) {
        m_selectedPath.back() = std::min(index, (int)siblings->size() - 1);
    } else {
        m_selectedPath.pop_back();
    }
    RefreshItemsPage();
    OnModelChanged();
}

void SettingsWindow::OnModelChanged() {
    m_dirty = BuildRegEntries(m_model) != m_savedEntries;
    UpdateFooter();
}

void SettingsWindow::UpdateFooter() {
    if (!m_statusText)
        return;
    m_statusText.Text(m_saving  ? L"Saving\u2026"
                      : m_dirty ? L"You have unsaved changes."
                                : L"No unsaved changes.");
    m_savingRing.IsActive(m_saving);
    m_savingRing.Visibility(m_saving ? mux::Visibility::Visible
                                     : mux::Visibility::Collapsed);
    m_saveButton.IsEnabled(m_dirty && !m_saving);
    m_discardButton.IsEnabled(m_dirty && !m_saving);
}

void SettingsWindow::ShowMessage(muxc::InfoBarSeverity severity,
                                 std::wstring const& title,
                                 std::wstring const& message) {
    m_infoBar.Severity(severity);
    m_infoBar.Title(title);
    m_infoBar.Message(message);
    m_infoBar.IsOpen(true);
}

void SettingsWindow::Save(bool closeAfter) {
    if (m_saving) {
        m_closeAfterSave = m_closeAfterSave || closeAfter;
        return;
    }

    auto dispatcherQueue = mud::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue)
        return;

    m_saving = true;
    m_closeAfterSave = closeAfter;
    m_infoBar.IsOpen(false);
    UpdateFooter();

    auto job = std::make_shared<SaveJob>();
    job->entries = BuildRegEntries(m_model);
    job->settings = m_model;
    HWND hOwnerWnd = m_hwnd;

    RunShellWorkOnWorkerThread([job, dispatcherQueue, hOwnerWnd]() {
        job->result = WriteModSettings(job->entries, hOwnerWnd);
        try {
            dispatcherQueue.TryEnqueue([job]() {
                if (g_settingsWindow)
                    g_settingsWindow->OnSaveCompleted(*job);
            });
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    });
}

void SettingsWindow::OnSaveCompleted(SaveJob const& job) {
    m_saving = false;
    if (job.result.succeeded) {
        m_saved = job.settings;
        m_savedEntries = job.entries;
        m_dirty = BuildRegEntries(m_model) != m_savedEntries;
        if (m_closeAfterSave) {
            DestroyWindow(m_hwnd);
            return;
        }
        ShowMessage(muxc::InfoBarSeverity::Success, L"Settings saved",
                    L"File Explorer applies them in a moment.");
    } else {
        Wh_Log(L"Saving failed: %s", job.result.error.c_str());
        ShowMessage(muxc::InfoBarSeverity::Error, L"Couldn't save the settings",
                    job.result.error);
    }
    m_closeAfterSave = false;
    UpdateFooter();
}

void SettingsWindow::Discard() {
    m_model = m_saved;
    m_infoBar.IsOpen(false);
    ShowPage(m_currentPage);
    OnModelChanged();
}

void SettingsWindow::RequestClose() {
    if (g_unloading) {
        ForceClose();
        return;
    }
    if (m_saving) {
        m_closeAfterSave = true;
        return;
    }
    if (!m_dirty) {
        DestroyWindow(m_hwnd);
        return;
    }
    if (!m_dialog)
        ShowUnsavedChangesDialog();
}

void SettingsWindow::ShowUnsavedChangesDialog() {
    try {
        muxc::ContentDialog dialog;
        if (auto style = FindStyle(L"DefaultContentDialogStyle"))
            dialog.Style(style);
        dialog.XamlRoot(m_root.XamlRoot());
        dialog.RequestedTheme(m_root.RequestedTheme());
        dialog.Title(BoxString(L"Save your changes?"));
        dialog.Content(BoxString(
            L"Your changes to the Explorer Command Bar settings haven't been "
            L"saved yet."));
        dialog.PrimaryButtonText(L"Save");
        dialog.SecondaryButtonText(L"Don't save");
        dialog.CloseButtonText(L"Cancel");
        dialog.DefaultButton(muxc::ContentDialogButton::Primary);
        m_dialog = dialog;

        dialog.ShowAsync().Completed(
            [this](
                wf::IAsyncOperation<muxc::ContentDialogResult> const& operation,
                wf::AsyncStatus status) {
                if (g_settingsWindow != this)
                    return;
                m_dialog = nullptr;
                if (status != wf::AsyncStatus::Completed)
                    return;
                switch (operation.GetResults()) {
                    case muxc::ContentDialogResult::Primary:
                        Save(true);
                        break;
                    case muxc::ContentDialogResult::Secondary:
                        DestroyWindow(m_hwnd);
                        break;
                    default:
                        break;
                }
            });
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
        m_dialog = nullptr;
    }
}

void SettingsWindow::ForceClose() {
    // A file dialog runs a nested message loop with this window as its owner,
    // cancel it first. The close message is posted again until the window is
    // gone.
    if (m_modalDepth > 0) {
        if (HWND hPopupWnd = GetWindow(m_hwnd, GW_ENABLEDPOPUP))
            PostMessageW(hPopupWnd, WM_CLOSE, 0, 0);
        return;
    }
    DestroyWindow(m_hwnd);
}

std::wstring SettingsWindow::Browse(PCWSTR title,
                                    COMDLG_FILTERSPEC const* filters,
                                    UINT filterCount) {
    m_modalDepth++;
    std::wstring path = BrowseForFile(m_hwnd, title, filters, filterCount);
    m_modalDepth--;
    return g_unloading ? std::wstring() : path;
}

// The save dialog picks the extension of the selected file type, and the
// extension picks the format.
constexpr COMDLG_FILTERSPEC kExportFileFilters[] = {
    {L"YAML", L"*.yaml;*.yml"},
    {L"JSON", L"*.json"},
};

constexpr COMDLG_FILTERSPEC kImportFileFilters[] = {
    {L"Explorer Command Bar settings (YAML, JSON)", L"*.yaml;*.yml;*.json"},
    {L"YAML", L"*.yaml;*.yml"},
    {L"JSON", L"*.json"},
    {L"All files", L"*.*"},
};

void SettingsWindow::ExportSettings() {
    m_modalDepth++;
    std::wstring path =
        BrowseForSaveFile(m_hwnd, L"Export settings", kExportFileFilters,
                          ARRAYSIZE(kExportFileFilters), L"yaml",
                          L"explorer-command-bar-settings.yaml");
    m_modalDepth--;
    if (path.empty() || g_unloading)
        return;

    auto entries = BuildRegEntries(m_model);
    bool isJson = ToLower(PathFindExtensionW(path.c_str())) == L".json";
    if (WriteUtf8File(path, isJson ? BuildSettingsJson(entries)
                                   : BuildSettingsYaml(entries))) {
        ShowMessage(muxc::InfoBarSeverity::Success, L"Settings exported", path);
    } else {
        ShowMessage(muxc::InfoBarSeverity::Error,
                    L"Couldn't export the settings",
                    L"Writing " + path + L" failed (error " +
                        std::to_wstring(GetLastError()) + L").");
    }
}

void SettingsWindow::ImportSettings() {
    std::wstring path = Browse(L"Import settings", kImportFileFilters,
                               ARRAYSIZE(kImportFileFilters));
    if (path.empty())
        return;

    std::wstring text = ReadFileAsWideString(path);
    EditableSettings imported = m_model;
    std::wstring error;
    if (text.empty()) {
        error =
            L"The file couldn't be read, or it's empty or larger than "
            L"1 MB.";
    } else if (ImportSettingsText(text, IsJsonSettingsFile(path, text),
                                  &imported, &error)) {
        // Assigned, not replaced, as the controls point into the model.
        m_model = imported;
        m_selectedPath.clear();
        ShowPage(m_currentPage);
        OnModelChanged();
        ShowMessage(muxc::InfoBarSeverity::Success, L"Settings imported",
                    L"Review them, then click Save to apply them.");
        return;
    }
    ShowMessage(muxc::InfoBarSeverity::Error, L"Couldn't import the settings",
                error);
}

std::atomic<bool> g_settingsWindowClassRegistered;

// The window runs on the thread of the File Explorer window it was opened
// from, which already has WinUI set up. WinUI can't be initialized on a
// thread of the mod's own in File Explorer.
void OpenSettingsWindow(HWND hHintWnd) {
    Wh_Log(L"Opening the settings window");
    if (g_unloading)
        return;
    if (!g_settingsWindowClassRegistered) {
        g_settingsWindowStep = L"registering the window class";
        ReportSettingsWindowError(L"The window class isn't registered.");
        return;
    }

    HWND hExistingWnd;
    {
        std::lock_guard<std::mutex> lock(g_settingsWindowMutex);
        hExistingWnd = g_settingsWindowHwnd;
    }
    if (hExistingWnd) {
        if (IsIconic(hExistingWnd))
            ShowWindowAsync(hExistingWnd, SW_RESTORE);
        SetForegroundWindow(hExistingWnd);
        return;
    }

    if (g_settingsWindowCreating.exchange(true))
        return;

    auto* window = new SettingsWindow();
    if (window->Create(hHintWnd)) {
        g_settingsWindow = window;
        std::lock_guard<std::mutex> lock(g_settingsWindowMutex);
        g_settingsWindowHwnd = window->Hwnd();
    } else if (HWND hWnd = window->Hwnd()) {
        // Deletes the window object.
        DestroyWindow(hWnd);
    } else {
        delete window;
    }
    g_settingsWindowCreating = false;

    if (g_unloading) {
        std::lock_guard<std::mutex> lock(g_settingsWindowMutex);
        if (g_settingsWindowHwnd)
            PostMessageW(g_settingsWindowHwnd, kSettingsWindowCloseMsg, 0, 0);
    }
}

void CloseSettingsWindow() {
    for (int i = 0;; i++) {
        HWND hWnd;
        {
            std::lock_guard<std::mutex> lock(g_settingsWindowMutex);
            hWnd = g_settingsWindowHwnd;
        }
        if (!hWnd && !g_settingsWindowCreating)
            break;
        if (hWnd)
            PostMessageW(hWnd, kSettingsWindowCloseMsg, 0, 0);
        Sleep(100);
        if (i > 0 && i % 50 == 0)
            Wh_Log(L"Still waiting for the settings window to close");
    }
}

void RegisterSettingsWindowClass() {
    std::wstring className = SettingsWindowClassName();
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = SettingsWindowProc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = className.c_str();
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassEx failed for the settings window: %u",
               GetLastError());
        return;
    }
    g_settingsWindowClassRegistered = true;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    if (GetFileExplorerExtensionsModuleHandle()) {
        if (!HookFileExplorerExtensionsIfLoaded(false))
            return FALSE;
    } else {
        Wh_Log(L"FileExplorerExtensions.dll isn't loaded yet");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW)
            return FALSE;
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    RegisterContextMenuOwnerClass();
    RegisterSettingsWindowClass();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    HookFileExplorerExtensionsIfLoaded(true);
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd,
            [](PVOID param) { DiscoverCommandBarsForWindow((HWND)param); },
            hWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;
    DismissOpenContextMenus();
    CloseSettingsWindow();
    if (g_settingsWindowClassRegistered) {
        UnregisterClassW(SettingsWindowClassName().c_str(),
                         GetCurrentModuleHandle());
        g_settingsWindowClassRegistered = false;
    }

    for (HWND hWnd : GetFileExplorerWnds()) {
        Wh_Log(L"Removing buttons for window %08X", (DWORD)(ULONG_PTR)hWnd);
        if (!RunFromWindowThread(
                hWnd, [](PVOID) { RemoveButtonsForCurrentThread(); },
                nullptr)) {
            Wh_Log(L"Couldn't reach the thread of window %08X",
                   (DWORD)(ULONG_PTR)hWnd);
        }
    }

    RemoveLeftoverShortcutHooks();

    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    {
        std::vector<std::pair<DWORD, HWND>> leftovers;
        {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            leftovers.assign(g_contextMenuOwners.begin(),
                             g_contextMenuOwners.end());
        }

        for (auto const& [threadId, hWnd] : leftovers) {
            if (!IsContextMenuOwnerWindow(threadId, hWnd)) {
                std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
                g_contextMenuOwners.erase(threadId);
                continue;
            }
            Wh_Log(
                L"Destroying leftover context menu owner window of thread %u",
                threadId);
            if (!RunFromWindowThread(
                    hWnd,
                    [](PVOID) {
                        DestroyContextMenuOwnerWindowForCurrentThread();
                    },
                    nullptr)) {
                Wh_Log(L"Couldn't reach thread %u", threadId);
            }
        }
    }

    if (g_contextMenuOwnerClassRegistered) {
        if (!UnregisterClassW(ContextMenuOwnerClassName().c_str(),
                              GetCurrentModuleHandle())) {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            Wh_Log(
                L"UnregisterClass failed: %u, %zu owner window(s) left behind",
                GetLastError(), g_contextMenuOwners.size());
        }
        g_contextMenuOwnerClassRegistered = false;
    }

    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        if (!g_launchThreads.empty()) {
            Wh_Log(L"Waiting for %zu launch thread(s)", g_launchThreads.size());
        }
    }

    WaitForLaunchThreads();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { RefreshButtonsForCurrentThread(); }, nullptr);
    }
}

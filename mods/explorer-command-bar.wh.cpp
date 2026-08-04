// ==WindhawkMod==
// @id              explorer-command-bar
// @name            Explorer Command Bar
// @description     Customize the Windows 11 File Explorer command bar with commands, menus, New+, and a shell context-menu button
// @version         1.1.0
// @author          DanRotaru
// @github          https://github.com/DanRotaru
// @homepage        https://dan13.me/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Command Bar

Add your own buttons and dropdown menus to the **Windows 11 File Explorer
command bar** (the toolbar with New / Sort / View), and hide the built-in
buttons, separators and spacing you don't want.

![Explorer Command Bar demo](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/explorer-command-bar/screenshots/main.gif)

Designed for Windows 11 24H2 / 25H2 with the WinAppSDK (WinUI 3) File
Explorer.

## Features

- **Custom buttons** - add as many toolbar buttons as you like, each running a
  command of your choice.
- **Dropdown menus & submenus** - a button can open a menu, and menu entries
  can themselves be submenus (three levels deep).
- **Shell context menu item** - optionally add an item which opens the real
  context menu of the active selection or folder, including shell extensions
  and optional Nilesoft Shell support.
- **Path & selection placeholders** - `%path%` (active tab folder) and `%sel%`
  (selected file/folder) are substituted into the command parameters.
- **Flexible icons** - a Segoe Fluent Icons glyph, an `.exe` / `.dll` / `.ico`
  file, a Store-app icon (`shell:AppsFolder\…`), the command executable's own
  icon, or no icon at all.
- **Hide built-in elements** - individually hide New, Cut, Copy, Paste, Rename,
  Share, Delete, Sort, View, the group separators, the "See more" (…) overflow
  menu, the contextual commands (Set as background, Rotate left, Rotate right,
  Extract all) and the Details pane toggle.
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

## Screenshots

Hiding built‑in buttons and separators:

![Hide buttons](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/explorer-command-bar/screenshots/hide-buttons.gif)

You may hide even all options, and use only your custom ones:

![Hide All buttons](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/explorer-command-bar/screenshots/hide-all-buttons.jpg)

## Command parameters

The following placeholders can be used in an item's parameters:

* `%path%` - the folder path of the currently active tab.
* `%sel%` - the full path of the selected file or folder in the active tab (the
  first one, if several are selected).

Wrap placeholders in quotes so paths with spaces work, e.g. `-d "%path%"`. If a
used placeholder has no value (nothing selected, or a non-filesystem location
like *This PC*), the command is launched without any parameters. Commands run
with the active tab's folder as their working directory.

## Icons

The **Icon glyph or icon path** field accepts several forms:

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

Notably, the mod does **not** use XAML Diagnostics
(`InitializeXamlDiagnosticsEx`), since only one XAML diagnostics consumer can be
active in a process at a time. That makes it compatible with mods and tools
which do use it, such as **Windows 11 File Explorer Styler**, ExplorerBlurMica
and TranslucentTB.

File Explorer windows which are already open when the mod is enabled are
handled too, but if the buttons don't show up in one of them right away, opening
a new tab or navigating to another folder makes them appear.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
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
        source.
    - parameters: -d "%path%"
      $name: Parameters
      $description: >-
        Command line parameters. %path% is replaced with the current folder
        path, %sel% with the path of the selected file/folder. If a used
        placeholder has no value (e.g. no selection, or a location like This
        PC), the command is launched without parameters.
    - iconGlyph: ""
      $name: Icon glyph or icon path
      $description: >-
        Hex code point of a Segoe Fluent Icons glyph, e.g. E756
        (CommandPrompt) or EC7A. The list of glyphs:
        https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font
        Alternatively, a path to an .exe, .dll or
        .ico file to use its icon, optionally with an icon index, e.g.
        C:\Windows\System32\shell32.dll,3. Leave empty to use the icon of the
        command's executable.
    - hideIcon: false
      $name: Hide icon
      $description: Don't show an icon for this item.
    - separatorAfter: false
      $name: Vertical separator after
      $description: Show a vertical separator line after this button.
    - subItems:
      - - type: button
          $name: Menu Items Item Type
          $options:
          - button: Menu item
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
            folder path, %sel% with the path of the selected file/folder.
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
                folder path, %sel% with the path of the selected file/folder.
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

#include <windows.h>

#include <windhawk_utils.h>

#include <exdisp.h>
#include <servprov.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <functional>
#include <memory>
#include <type_traits>
#include <mutex>
#include <string>
#include <string_view>
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
    std::wstring command;
    std::wstring parameters;
    std::wstring icon;  // Raw icon setting: a glyph code point, a path to an
                        // icon file, or empty to use the command's
                        // executable icon.
    bool hideIcon = false;
    bool isMenu = false;
    bool separatorAfter = false;
    std::vector<ActionItem> subItems;
};

// The built-in command bar buttons. They're identified at runtime by the SVG
// file name of their icon, which is stable across languages.
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
    // Contextual buttons, shown only for the matching selection: an image
    // file for the first three, an archive for Extract all.
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

// The New+ button which takes the place of Explorer's New button.
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
    bool openMenuOnHover = false;
    int menuHoverDelay = 400;
    bool hideDefaultButtons[kDefaultButtonCount] = {};
    bool hideSeparatorAfterButton[kDefaultButtonCount] = {};
    bool hideMoreButton = false;
    bool hideDetailsButton = false;
    int itemSpacing = -1;
    std::vector<ActionItem> items;
    NewPlusSettings newPlus;
    ContextMenuItemSettings contextMenuItem;
} g_settings;

////////////////////////////////////////////////////////////////////////////////
// clang-format off

#pragma region winrt_hpp

#include <Unknwn.h>

// Conflicts with a winrt method of the same name.
#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Documents.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxd = winrt::Microsoft::UI::Xaml::Documents;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;

#pragma endregion  // winrt_hpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

// Our own elements are recognized by their name. The action buttons and their
// separators share a prefix (the index of the item follows), the New+ button has
// a name of its own.
constexpr PCWSTR kButtonNamePrefix = L"WindhawkActionButton";
constexpr PCWSTR kNewPlusButtonName = L"WindhawkNewPlusButton";
constexpr PCWSTR kContextMenuButtonName = L"WindhawkContextMenuButton";

// Everything below is per-UI-thread state, and every XAML object in it belongs
// to the thread that created it: a weak reference to a live non-agile XAML
// element can only be resolved from its own thread, and its event handlers can
// only be unregistered there. The state is therefore thread_local rather than
// a global keyed by thread id - the cross-thread access becomes impossible by
// construction, the storage goes away with the window's thread, and the
// lookups don't scan other windows' entries. Both Wh_ModUninit and
// Wh_ModSettingsChanged already reach every thread through
// RunFromWindowThread.

struct CommandBarEntry {
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::event_token vectorChangedToken{};
};

thread_local std::vector<CommandBarEntry> g_entries;

// Set once a scan of this thread's XAML island found its command bars, so the
// focus hook can skip the tree walk while they're still around.
thread_local bool g_threadScanned;

// Every event handler the mod registers on the elements it creates has to be
// revoked before the mod is unloaded. The delegate object itself lives in this
// DLL, so XAML releasing it afterwards - even without ever invoking it again -
// would run code from an unmapped module. Handlers are registered with
// winrt::auto_revoke and their revokers are kept here until the buttons are
// taken down, or until the mod is disabled.
struct TrackedRevoker {
    winrt::weak_ref<wf::IInspectable> source;
    std::function<void()> revoke;
};

thread_local std::vector<TrackedRevoker> g_revokers;

constexpr size_t kRevokersPruneMin = 64;
thread_local size_t g_revokersPruneAt = kRevokersPruneMin;

template <typename T, typename Revoker>
void TrackRevoker(T const& source, Revoker&& revoker) {
    // Entries whose element is gone can go: XAML released their delegates
    // together with the element itself. Only when the vector has grown past
    // the threshold, though - resolving every weak_ref is a COM call each, and
    // populating a menu with many items would otherwise be quadratic in them.
    if (g_revokers.size() >= g_revokersPruneAt) {
        std::erase_if(g_revokers,
                      [](TrackedRevoker const& tracked) {
                          return !tracked.source.get();
                      });

        // Prune again once the survivors have doubled, so the work stays
        // proportional to what's actually being added.
        g_revokersPruneAt = std::max(kRevokersPruneMin, g_revokers.size() * 2);
    }

    // The revoker types differ per event, so they're type-erased behind the
    // std::function. Calling revoke() twice is harmless: the second call is a
    // no-op, which is also what the revoker's own destructor does.
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

// The Explorer elements we touch, each with the original state captured the
// first time we touched it, so it can be restored exactly instead of forcing a
// guessed default (which would reveal elements Explorer keeps collapsed), and
// with the visibility watcher registered on it (see below).
//
// The element is held as a weak reference and entries are matched by resolving
// it, never by its address: XAML objects die with their window or tab, and the
// heap happily hands the same address to an unrelated element of the next one.
struct ManagedElement {
    winrt::weak_ref<mux::UIElement> element;

    bool hasOriginalVisibility = false;
    mux::Visibility originalVisibility = mux::Visibility::Visible;

    bool hasOriginalSpacing = false;
    mux::Thickness originalMargin{};
    double originalMinWidth = 0;

    // Command bars only.
    bool hasOriginalOverflow = false;
    muxc::CommandBarOverflowButtonVisibility originalOverflow =
        muxc::CommandBarOverflowButtonVisibility::Auto;

    bool watched = false;
    int64_t visibilityToken = 0;
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
    // Drop the entries whose element is gone, so the list doesn't grow for the
    // lifetime of the window.
    for (auto it = g_managedElements.begin();
         it != g_managedElements.end();) {
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

////////////////////////////////////////////////////////////////////////////////
// Resolving a configured command. Used both for launching it and for taking
// its icon.

std::wstring ExpandEnvVars(std::wstring const& str) {
    if (str.empty()) {
        return str;
    }

    WCHAR buffer[MAX_PATH * 2];
    DWORD length =
        ExpandEnvironmentStringsW(str.c_str(), buffer, ARRAYSIZE(buffer));
    if (length == 0 || length > ARRAYSIZE(buffer)) {
        return str;
    }

    return buffer;
}

std::wstring ToLower(std::wstring str) {
    for (auto& c : str) {
        c = towlower(c);
    }

    return str;
}

std::wstring TrimQuotesAndSpaces(std::wstring str) {
    size_t first = str.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        return std::wstring();
    }

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

// Resolves a bare executable name to a full path the way ShellExecute does:
// through the search path, then through the App Paths registry keys.
std::wstring ResolveCommandPath(std::wstring const& command) {
    std::wstring expanded = ExpandEnvVars(command);

    if (expanded.find(L'\\') != std::wstring::npos) {
        return expanded;
    }

    WCHAR resolved[MAX_PATH];
    if (SearchPathW(nullptr, expanded.c_str(), L".exe", ARRAYSIZE(resolved),
                    resolved, nullptr)) {
        return resolved;
    }

    std::wstring keyPath =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" +
        expanded;
    if (!ToLower(expanded).ends_with(L".exe")) {
        keyPath += L".exe";
    }

    for (HKEY rootKey : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        WCHAR buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (RegGetValueW(rootKey, keyPath.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, buffer, &size) == ERROR_SUCCESS &&
            buffer[0]) {
            std::wstring appPath = ExpandEnvVars(buffer);

            // The value is sometimes quoted.
            if (appPath.size() >= 2 && appPath.front() == L'"' &&
                appPath.back() == L'"') {
                appPath = appPath.substr(1, appPath.size() - 2);
            }

            return appPath;
        }
    }

    return expanded;
}

////////////////////////////////////////////////////////////////////////////////
// Getting the current folder path and launching commands.

// What the active tab of a File Explorer window is showing. The shell view is
// only used by the New+ button, to select and rename the item it creates.
struct ExplorerContext {
    std::wstring folderPath;
    std::wstring selectedPath;
    winrt::com_ptr<IShellView> shellView;
};

// The shell view of the given File Explorer window's active tab.
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

    // The active tab's ShellTabWindowClass window is the first one in the
    // Z-order of the CabinetWClass window's children.
    HWND hActiveTabWnd =
        hExplorerWnd ? FindWindowExW(hExplorerWnd, nullptr,
                                     L"ShellTabWindowClass", nullptr)
                     : nullptr;

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        winrt::com_ptr<IDispatch> dispatch;
        if (FAILED(shellWindows->Item(index, dispatch.put())) || !dispatch) {
            continue;
        }

        auto webBrowser = dispatch.try_as<IWebBrowser2>();
        if (!webBrowser) {
            continue;
        }

        SHANDLE_PTR hWndRaw = 0;
        if (FAILED(webBrowser->get_HWND(&hWndRaw))) {
            continue;
        }

        if (hExplorerWnd && (HWND)hWndRaw != hExplorerWnd) {
            continue;
        }

        auto serviceProvider = dispatch.try_as<IServiceProvider>();
        if (!serviceProvider) {
            continue;
        }

        winrt::com_ptr<IShellBrowser> shellBrowser;
        if (FAILED(serviceProvider->QueryService(
                kSID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))) ||
            !shellBrowser) {
            continue;
        }

        // With tabs, every tab is a separate ShellWindows entry with the same
        // top-level window. Skip the entries of inactive tabs.
        HWND hTabWnd = nullptr;
        if (SUCCEEDED(shellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
            if (hActiveTabWnd) {
                if (hTabWnd != hActiveTabWnd) {
                    continue;
                }
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

////////////////////////////////////////////////////////////////////////////////
// The shell context menu item.

bool ShellViewHasSelection(winrt::com_ptr<IShellView> const& shellView) {
    auto folderView = shellView.try_as<IFolderView>();
    if (!folderView) {
        return false;
    }

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
            auto& param = *(FindWindowByClassParam*)lParam;
            WCHAR buffer[64];
            if (GetClassNameW(hWnd, buffer, ARRAYSIZE(buffer)) &&
                _wcsicmp(buffer, param.className) == 0) {
                param.result = hWnd;
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
        if (HWND hViewWnd = FindDescendantWindow(hTabWnd, L"SHELLDLL_DefView")) {
            return hViewWnd;
        }
    }

    return FindDescendantWindow(hExplorerWnd, L"SHELLDLL_DefView");
}

// Nilesoft Shell replaces the file list's context menu from inside Explorer,
// so asking the view to show its own menu is the only way to invoke it.
bool RequestShellViewContextMenu(HWND hExplorerWnd) {
    HWND hViewWnd = FindShellViewWindow(hExplorerWnd);
    if (!hViewWnd) {
        Wh_Log(L"No shell view window for %08X", (DWORD)(ULONG_PTR)hExplorerWnd);
        return false;
    }

    return PostMessageW(hViewWnd, WM_CONTEXTMENU, (WPARAM)hViewWnd,
                        (LPARAM)-1) != FALSE;
}

winrt::com_ptr<IContextMenu> GetShellContextMenu(
    HWND hExplorerWnd,
    bool* isItemMenu) {
    *isItemMenu = false;

    auto shellView = GetActiveShellView(hExplorerWnd);
    if (!shellView) {
        Wh_Log(L"No shell view for window %08X", (DWORD)(ULONG_PTR)hExplorerWnd);
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

// How many threads are anywhere inside ShowShellContextMenu. Not just the modal
// loop: GetShellContextMenu walks the shell windows and QueryContextMenu loads
// and runs every registered shell extension, which is the part that can take a
// long time, and all of it runs this DLL's code. Wh_ModUninit waits for this to
// drop to zero before it lets the module be unmapped.
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
                        uMsg, wParam, lParam, &result))) {
                    return result;
                }
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
                    result) {
                    return result;
                }
            }
            break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

std::wstring ContextMenuOwnerClassName() {
    return std::wstring(L"WindhawkContextMenuOwner_") + WH_MOD_ID;
}

// This mod's DLL HINSTANCE. GetModuleHandle(nullptr) would return explorer.exe,
// which is wrong for RegisterClass/CreateWindowEx/UnregisterClass: the class
// belongs to the module its window procedure lives in.
HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

std::atomic<bool> g_contextMenuOwnerClassRegistered;
std::mutex g_contextMenuOwnersMutex;
std::unordered_map<DWORD, HWND> g_contextMenuOwners;

bool IsContextMenuOwnerWindow(DWORD threadId, HWND hWnd) {
    if (!IsWindow(hWnd) || GetWindowThreadProcessId(hWnd, nullptr) != threadId) {
        return false;
    }

    WCHAR className[128];
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, ContextMenuOwnerClassName().c_str()) == 0;
}

// Registered once, from Wh_ModInit, so that the Explorer UI threads which reach
// EnsureContextMenuOwnerWindow never race over the registration - and so
// ERROR_CLASS_ALREADY_EXISTS can be treated as the hard failure it is. For a
// freshly loaded instance, that error means a previous load left the class
// behind, and its lpfnWndProc still points at the previous, now unmapped image:
// a window created on it would dispatch WM_INITMENUPOPUP into freed memory.
// Doing without the shell context menu item until Explorer restarts is the
// lesser evil.
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
                   ? L" - a previous load of the mod left the class behind, "
                     L"the shell context menu item will be unavailable until "
                     L"Explorer is restarted"
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
        if (it != g_contextMenuOwners.end()) {
            g_contextMenuOwners.erase(it);
        }
    }

    if (!g_contextMenuOwnerClassRegistered) {
        return nullptr;
    }

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

// Waits until no thread is inside ShowShellContextMenu anymore. There's no
// timeout on purpose: giving up would mean unmapping the DLL out from under a
// thread which is still running its code, and the wait can't deadlock, since
// those threads never send anything to the thread running Wh_ModUninit.
// WM_CANCELMODE is re-posted on every iteration because a single post can land
// before the target thread has entered its modal loop, which would drop it.
void DismissOpenContextMenus() {
    for (int i = 0; g_openContextMenuCount > 0; i++) {
        {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            for (auto const& [threadId, hWnd] : g_contextMenuOwners) {
                if (IsContextMenuOwnerWindow(threadId, hWnd)) {
                    PostMessageW(hWnd, WM_CANCELMODE, 0, 0);
                }
            }
        }

        // Once a second, so that an unload which is stuck here - a shell
        // extension taking its time in QueryContextMenu, say - is diagnosable.
        if (i > 0 && i % 100 == 0) {
            Wh_Log(L"Still waiting for %d shell context menu(s)",
                   g_openContextMenuCount.load());
        }

        Sleep(10);
    }
}

void DestroyContextMenuOwnerWindowForCurrentThread() {
    if (g_contextMenuIsOpen) {
        return;
    }

    HWND hWnd = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
        auto it = g_contextMenuOwners.find(GetCurrentThreadId());
        if (it == g_contextMenuOwners.end()) {
            return;
        }
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
    if (GetKeyState(VK_CONTROL) & 0x8000) {
        info.fMask |= CMIC_MASK_CONTROL_DOWN;
    }
    if (GetKeyState(VK_SHIFT) & 0x8000) {
        info.fMask |= CMIC_MASK_SHIFT_DOWN;
    }

    HRESULT hr = contextMenu->InvokeCommand((CMINVOKECOMMANDINFO*)&info);
    if (FAILED(hr)) {
        Wh_Log(L"InvokeCommand failed: %08X", hr);
    }
}

void ShowShellContextMenu(HWND hExplorerWnd, POINT point) {
    if (g_contextMenuIsOpen || g_unloading) {
        return;
    }

    // Held for the whole call, not just the modal loop: everything below runs
    // this DLL's code, including the shell extensions QueryContextMenu brings
    // in, and Wh_ModUninit has to wait for all of it.
    OpenContextMenuScope openScope;

    bool useNilesoftShell;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        useNilesoftShell = g_settings.contextMenuItem.useNilesoftShell;
    }

    if (useNilesoftShell && RequestShellViewContextMenu(hExplorerWnd)) {
        return;
    }

    HWND hOwnerWnd = EnsureContextMenuOwnerWindow();
    if (!hOwnerWnd) {
        return;
    }

    HRESULT hrInit =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool isItemMenu = false;
    auto contextMenu = GetShellContextMenu(hExplorerWnd, &isItemMenu);
    if (!contextMenu) {
        if (hrInit == S_OK) {
            CoUninitialize();
        }
        return;
    }

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) {
        if (hrInit == S_OK) {
            CoUninitialize();
        }
        return;
    }

    UINT flags = isItemMenu ? CMF_CANRENAME : CMF_NORMAL;
    if (GetKeyState(VK_SHIFT) & 0x8000) {
        flags |= CMF_EXTENDEDVERBS;
    }

    HRESULT hr = contextMenu->QueryContextMenu(
        hMenu, 0, kContextMenuFirstCmdId, kContextMenuLastCmdId, flags);
    if (FAILED(hr)) {
        Wh_Log(L"QueryContextMenu failed: %08X", hr);
        DestroyMenu(hMenu);
        if (hrInit == S_OK) {
            CoUninitialize();
        }
        return;
    }

    // Building the menu can take a while, so check again before showing it -
    // an unload which started in the meantime is waiting for this call.
    if (g_unloading) {
        DestroyMenu(hMenu);
        if (hrInit == S_OK) {
            CoUninitialize();
        }
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

    if (cmdId >= kContextMenuFirstCmdId &&
        cmdId <= kContextMenuLastCmdId && !g_unloading) {
        InvokeShellContextMenuCommand(contextMenu, cmdId, hExplorerWnd, point);
    }

    DestroyMenu(hMenu);
    if (hrInit == S_OK) {
        CoUninitialize();
    }
}

// The folder and the selection of the given window's active tab. Always used
// from a worker thread, so the shell calls it makes can take their time.
ExplorerContext GetExplorerContext(HWND hExplorerWnd) {
    ExplorerContext result;

    result.shellView = GetActiveShellView(hExplorerWnd);
    if (!result.shellView) {
        return result;
    }

    if (auto folderView = result.shellView.try_as<IFolderView>()) {
        winrt::com_ptr<IPersistFolder2> persistFolder;
        LPITEMIDLIST pidl = nullptr;
        if (SUCCEEDED(
                folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) &&
            persistFolder &&
            SUCCEEDED(persistFolder->GetCurFolder(&pidl)) && pidl) {
            WCHAR path[MAX_PATH];
            if (SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path),
                                      GPFIDL_DEFAULT)) {
                result.folderPath = path;
            }

            CoTaskMemFree(pidl);
        }
    }

    // The first selected file or folder, if any.
    winrt::com_ptr<IShellItemArray> selection;
    if (SUCCEEDED(result.shellView->GetItemObject(
            SVGIO_SELECTION, IID_PPV_ARGS(selection.put()))) &&
        selection) {
        winrt::com_ptr<IShellItem> shellItem;
        if (SUCCEEDED(selection->GetItemAt(0, shellItem.put())) && shellItem) {
            PWSTR selectedPath = nullptr;
            if (SUCCEEDED(shellItem->GetDisplayName(SIGDN_FILESYSPATH,
                                                    &selectedPath)) &&
                selectedPath) {
                result.selectedPath = selectedPath;
                CoTaskMemFree(selectedPath);
            }
        }
    }

    return result;
}

// Replaces all occurrences of the placeholder. Returns false if the
// placeholder is used but has no value.
bool ReplacePlaceholder(std::wstring& parameters,
                        std::wstring_view placeholder,
                        std::wstring const& value) {
    size_t pos = parameters.find(placeholder);
    if (pos == std::wstring::npos) {
        return true;
    }

    if (value.empty()) {
        return false;
    }

    while (pos != std::wstring::npos) {
        std::wstring replacement = value;
        // A trailing backslash would escape a closing quote right after the
        // placeholder (e.g. for C:\).
        if (replacement.back() == L'\\' &&
            pos + placeholder.size() < parameters.size() &&
            parameters[pos + placeholder.size()] == L'"') {
            replacement += L'\\';
        }

        parameters.replace(pos, placeholder.size(), replacement);
        pos = parameters.find(placeholder, pos + replacement.size());
    }

    return true;
}

std::wstring BuildParameters(std::wstring parameters,
                             ExplorerContext const& context) {
    // If a used placeholder has no value (no filesystem path, no selection),
    // launch without parameters.
    if (!ReplacePlaceholder(parameters, L"%path%", context.folderPath) ||
        !ReplacePlaceholder(parameters, L"%sel%", context.selectedPath)) {
        return std::wstring();
    }

    return parameters;
}

// The threads which are busy resolving the Explorer context and launching a
// command. Their code lives in this DLL, so the mod can't be unloaded while
// one of them is still running - see
// https://github.com/ramensoftware/windhawk/wiki/Global-objects-and-process-shutdown
std::mutex g_launchThreadsMutex;
std::vector<HANDLE> g_launchThreads;

void TrackLaunchThread(HANDLE thread) {
    std::lock_guard<std::mutex> lock(g_launchThreadsMutex);

    // Reap the ones which already finished.
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

// Runs shell work on a tracked worker thread with an apartment of its own. Used
// for everything which must not happen on the Explorer UI thread: launching a
// command, copying a template, opening a folder. Note that the shell objects
// involved are owned by the Explorer UI thread, so the calls marshal back to it;
// only the waiting happens elsewhere.
void RunShellWorkOnWorkerThread(std::function<void()> work) {
    auto params = std::make_unique<std::function<void()>>(std::move(work));

    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            std::unique_ptr<std::function<void()>> work(
                reinterpret_cast<std::function<void()>*>(lpParam));

            HRESULT hrInit = CoInitializeEx(
                nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            try {
                (*work)();
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
            if (SUCCEEDED(hrInit)) {
                CoUninitialize();
            }

            return 0;
        },
        params.get(), 0, nullptr);
    if (thread) {
        params.release();  // Owned by the thread now.
        // The handle is closed once the thread finished, either here on a later
        // call or in Wh_ModUninit, which waits for it.
        TrackLaunchThread(thread);
    }
}

void LaunchItemForWindow(HWND hExplorerWnd, ActionItem const& item) {
    ExplorerContext context = GetExplorerContext(hExplorerWnd);
    Wh_Log(L"Launching %s for window %08X, path: %s, selection: %s",
           item.command.c_str(), (DWORD)(ULONG_PTR)hExplorerWnd,
           context.folderPath.c_str(), context.selectedPath.c_str());

    std::wstring parameters = BuildParameters(item.parameters, context);

    // A bare command name has to be resolved before it's launched: the working
    // directory is the browsed folder, and the shell resolves a relative file
    // against it, so a same-named executable sitting in the folder the user
    // happens to be looking at would win over the real one.
    std::wstring command = ResolveCommandPath(item.command);
    bool isPath = command.find(L'\\') != std::wstring::npos;
    if (!isPath) {
        Wh_Log(L"Couldn't resolve %s to a path, launching without a working "
               L"directory",
               item.command.c_str());
    }

    SHELLEXECUTEINFOW execInfo{};
    execInfo.cbSize = sizeof(execInfo);
    // No UI: a command which doesn't exist would otherwise put up a modal
    // error box on this thread, which the mod would then have to wait for
    // while unloading. The failure is logged below instead. NOASYNC because
    // this thread exits right after.
    execInfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    execInfo.lpFile = command.c_str();
    execInfo.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
    execInfo.lpDirectory = (isPath && !context.folderPath.empty())
                               ? context.folderPath.c_str()
                               : nullptr;
    execInfo.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&execInfo)) {
        Wh_Log(L"ShellExecuteExW failed for %s: %u", command.c_str(),
               GetLastError());
    }
}

// The File Explorer window an element of ours belongs to. Note that a menu
// flyout lives in its own popup window, so the element to ask has to be the
// anchor button, not the clicked menu item.
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

    if (!hWnd) {
        hWnd = GetActiveWindow();
    }

    if (!hWnd) {
        // Last resort, and the only candidate which isn't ours by
        // construction: the foreground window can belong to another process,
        // and callers post messages into what they get back from here.
        HWND hForegroundWnd = GetForegroundWindow();
        DWORD processId = 0;
        if (hForegroundWnd &&
            GetWindowThreadProcessId(hForegroundWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            hWnd = hForegroundWnd;
        }
    }

    if (hWnd) {
        hWnd = GetAncestor(hWnd, GA_ROOT);
    }

    return hWnd;
}

void OnActionInvoked(mux::FrameworkElement const& elementForWindow,
                     ActionItem const& item) {
    if (item.command.empty() || g_unloading) {
        return;
    }

    HWND hWnd = GetExplorerWindowForElement(elementForWindow);

    // Off the UI thread, so a slow or unresponsive shell can't block the click
    // handler.
    RunShellWorkOnWorkerThread(
        [hWnd, item]() { LaunchItemForWindow(hWnd, item); });
}

////////////////////////////////////////////////////////////////////////////////
// PowerToys' New+ configuration, for the New+ button.
//
// The settings file is small and its shape is stable, so instead of pulling in a
// JSON parser, the values are picked out of it directly: find the property name,
// then the "value" which follows it.

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
    if (hFile == INVALID_HANDLE_VALUE) {
        return std::wstring();
    }

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(hFile, &size) || size.QuadPart <= 0 ||
        size.QuadPart > 1024 * 1024) {
        CloseHandle(hFile);
        return std::wstring();
    }

    std::string bytes((size_t)size.QuadPart, '\0');
    DWORD bytesRead = 0;
    BOOL succeeded = ReadFile(hFile, bytes.data(), (DWORD)bytes.size(),
                              &bytesRead, nullptr);
    CloseHandle(hFile);
    if (!succeeded) {
        return std::wstring();
    }

    bytes.resize(bytesRead);

    // The settings file is UTF-8, with or without a BOM.
    if (bytes.size() >= 3 && (unsigned char)bytes[0] == 0xEF &&
        (unsigned char)bytes[1] == 0xBB && (unsigned char)bytes[2] == 0xBF) {
        bytes.erase(0, 3);
    }

    int length = MultiByteToWideChar(CP_UTF8, 0, bytes.data(),
                                     (int)bytes.size(), nullptr, 0);
    if (length <= 0) {
        return std::wstring();
    }

    std::wstring result((size_t)length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, bytes.data(), (int)bytes.size(),
                        result.data(), length);
    return result;
}

// The raw text of the value which follows the given property name, e.g.
// `"C:\\Templates"` or `true`. Empty if the property isn't there.
std::wstring FindJsonValueText(std::wstring const& json, PCWSTR propertyName) {
    std::wstring needle = L"\"";
    needle += propertyName;
    needle += L"\"";

    size_t pos = json.find(needle);
    if (pos == std::wstring::npos) {
        return std::wstring();
    }

    // The properties are wrapped in an object with a single "value" member:
    // "TemplateLocation": { "value": "..." }. A property whose value is a plain
    // literal is supported as well.
    size_t colon = json.find(L':', pos + needle.size());
    if (colon == std::wstring::npos) {
        return std::wstring();
    }

    size_t valuePos = json.find(L"\"value\"", pos + needle.size());
    size_t nextProperty = json.find(L'}', colon);
    if (valuePos != std::wstring::npos &&
        (nextProperty == std::wstring::npos || valuePos < nextProperty)) {
        colon = json.find(L':', valuePos);
        if (colon == std::wstring::npos) {
            return std::wstring();
        }
    }

    size_t start = json.find_first_not_of(L" \t\r\n", colon + 1);
    if (start == std::wstring::npos) {
        return std::wstring();
    }

    if (json[start] == L'"') {
        // Copy the string, unescaping as we go.
        std::wstring result;
        for (size_t i = start + 1; i < json.size(); i++) {
            WCHAR c = json[i];
            if (c == L'"') {
                break;
            }

            if (c == L'\\' && i + 1 < json.size()) {
                WCHAR escaped = json[++i];
                switch (escaped) {
                    case L'n':
                        result += L'\n';
                        break;
                    case L'r':
                        result += L'\r';
                        break;
                    case L't':
                        result += L'\t';
                        break;
                    default:
                        result += escaped;  // Covers \\ and \".
                        break;
                }

                continue;
            }

            result += c;
        }

        return result;
    }

    size_t end = json.find_first_of(L",}\r\n", start);
    if (end == std::wstring::npos) {
        end = json.size();
    }

    std::wstring result = json.substr(start, end - start);
    while (!result.empty() &&
           (result.back() == L' ' || result.back() == L'\t')) {
        result.pop_back();
    }

    return result;
}

struct PowerToysConfig {
    std::wstring templateFolder;
    bool hideFileExtension = true;
    bool hideStartingDigits = true;
    bool replaceVariables = false;
};

// Read on demand rather than cached, so changing a PowerToys option takes effect
// the next time the menu is opened.
PowerToysConfig ReadPowerToysConfig() {
    PowerToysConfig config;

    std::wstring json = ReadFileAsWideString(
        JoinPath(GetPowerToysNewPlusFolder(), L"settings.json"));
    if (json.empty()) {
        return config;
    }

    auto readBool = [&json](PCWSTR name, bool fallback) {
        std::wstring text = ToLower(FindJsonValueText(json, name));
        if (text == L"true" || text == L"1") {
            return true;
        }

        if (text == L"false" || text == L"0") {
            return false;
        }

        return fallback;
    };

    config.templateFolder =
        TrimQuotesAndSpaces(FindJsonValueText(json, L"TemplateLocation"));
    config.hideFileExtension =
        readBool(L"HideFileExtension", config.hideFileExtension);
    config.hideStartingDigits =
        readBool(L"HideStartingDigits", config.hideStartingDigits);
    config.replaceVariables =
        readBool(L"ReplaceVariables", config.replaceVariables);

    return config;
}

// How the templates are presented: PowerToys' own New+ configuration, plus the
// display options of the mod.
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

////////////////////////////////////////////////////////////////////////////////
// The New+ templates.

struct TemplateEntry {
    std::wstring path;      // Full path of the template.
    std::wstring fileName;  // Name of the template, as it is on disk.
    std::wstring displayName;
    bool isDirectory = false;
};

// Digits at the start of a template's name are only there to order the menu
// entries; drop them along with the separator which follows.
std::wstring StripStartingDigits(std::wstring const& name) {
    size_t pos = 0;
    while (pos < name.size() && iswdigit(name[pos])) {
        pos++;
    }

    if (pos == 0) {
        return name;
    }

    while (pos < name.size() && wcschr(L" .-_", name[pos])) {
        pos++;
    }

    // Never end up with an empty label, e.g. for a template named "2024".
    if (pos >= name.size()) {
        return name;
    }

    return name.substr(pos);
}

std::wstring MakeDisplayName(std::wstring const& fileName,
                             bool isDirectory,
                             EffectiveConfig const& config) {
    std::wstring name = fileName;

    if (!isDirectory && config.hideFileExtension) {
        size_t dot = name.find_last_of(L'.');
        if (dot != std::wstring::npos && dot > 0) {
            name.resize(dot);
        }
    }

    if (config.hideStartingDigits) {
        name = StripStartingDigits(name);
    }

    return name;
}

std::vector<TemplateEntry> EnumerateTemplates(EffectiveConfig const& config) {
    std::vector<TemplateEntry> entries;

    std::wstring pattern = JoinPath(config.templateFolder, L"*");

    WIN32_FIND_DATAW findData{};
    HANDLE hFind =
        FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &findData,
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
            _wcsicmp(fileName.c_str(), L"desktop.ini") == 0) {
            continue;
        }

        if (findData.dwFileAttributes &
            (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) {
            continue;
        }

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

    // Folder templates first, then files, each in the natural, case-insensitive
    // order File Explorer itself would list them in - the order New+ uses.
    std::sort(entries.begin(), entries.end(),
              [](TemplateEntry const& a, TemplateEntry const& b) {
                  if (a.isDirectory != b.isDirectory) {
                      return a.isDirectory;
                  }

                  return StrCmpLogicalW(a.fileName.c_str(),
                                        b.fileName.c_str()) < 0;
              });

    return entries;
}

////////////////////////////////////////////////////////////////////////////////
// Creating an item from a New+ template.

std::wstring ReplaceAll(std::wstring str,
                        std::wstring_view from,
                        std::wstring const& to) {
    if (from.empty()) {
        return str;
    }

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
               (folder.back() == L'\\' || folder.back() == L'/')) {
            folder.pop_back();
        }

        size_t slash = folder.find_last_of(L"\\/");
        parentFolderName =
            slash == std::wstring::npos ? folder : folder.substr(slash + 1);
    }

    std::wstring result = fileName;
    // Longest first, so $YYYY isn't eaten by $YY.
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

// Splits a file name into its base name and extension (including the dot).
void SplitFileName(std::wstring const& fileName,
                   bool isDirectory,
                   std::wstring* baseName,
                   std::wstring* extension) {
    size_t dot =
        isDirectory ? std::wstring::npos : fileName.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) {
        *baseName = fileName;
        extension->clear();
        return;
    }

    *baseName = fileName.substr(0, dot);
    *extension = fileName.substr(dot);
}

// A path in the target folder which isn't taken yet, in Explorer's style:
// "Report.docx", "Report (2).docx", ...
std::wstring MakeUniquePath(std::wstring const& targetFolder,
                            std::wstring const& fileName,
                            bool isDirectory) {
    std::wstring path = JoinPath(targetFolder, fileName);
    if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return path;
    }

    std::wstring baseName;
    std::wstring extension;
    SplitFileName(fileName, isDirectory, &baseName, &extension);

    for (int i = 2; i < 10000; i++) {
        std::wstring candidate = baseName;
        candidate += L" (";
        candidate += std::to_wstring(i);
        candidate += L")";
        candidate += extension;

        path = JoinPath(targetFolder, candidate);
        if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
            return path;
        }
    }

    return std::wstring();
}

bool CopyDirectoryRecursively(std::wstring const& source,
                              std::wstring const& target) {
    if (!CreateDirectoryW(target.c_str(), nullptr) &&
        GetLastError() != ERROR_ALREADY_EXISTS) {
        Wh_Log(L"CreateDirectory(%s) failed: %u", target.c_str(),
               GetLastError());
        return false;
    }

    std::wstring pattern = JoinPath(source, L"*");

    WIN32_FIND_DATAW findData{};
    HANDLE hFind =
        FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &findData,
                         FindExSearchNameMatch, nullptr,
                         FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) {
        return true;  // An empty (or unreadable) template folder.
    }

    bool succeeded = true;
    do {
        std::wstring fileName = findData.cFileName;
        if (fileName == L"." || fileName == L"..") {
            continue;
        }

        std::wstring sourceChild = JoinPath(source, fileName);
        std::wstring targetChild = JoinPath(target, fileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            succeeded &= CopyDirectoryRecursively(sourceChild, targetChild);
        } else if (!CopyFileW(sourceChild.c_str(), targetChild.c_str(),
                              /*bFailIfExists=*/TRUE)) {
            Wh_Log(L"CopyFile(%s) failed: %u", targetChild.c_str(),
                   GetLastError());
            succeeded = false;
        }
    } while (FindNextFileW(hFind, &findData));

    FindClose(hFind);
    return succeeded;
}

// Selects the created item and starts editing its name, the way Explorer's own
// New command does. The view learns about the new item asynchronously, so give
// it a few chances.
void SelectAndRename(winrt::com_ptr<IShellView> const& shellView,
                     std::wstring const& path) {
    if (!shellView) {
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) ||
        !pidl) {
        return;
    }

    PCUITEMID_CHILD child = ILFindLastID(pidl);

    auto folderView = shellView.try_as<IFolderView>();

    // The view learns about the new item from a change notification, so
    // selecting it can come too early, in which case nothing happens (and
    // nothing fails either). Retry until the item is actually selected.
    for (int attempt = 0; attempt < 20 && !g_unloading; attempt++) {
        Sleep(50);

        HRESULT hr = shellView->SelectItem(
            child, SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE |
                       SVSI_FOCUSED | SVSI_EDIT);
        if (FAILED(hr)) {
            continue;
        }

        if (!folderView) {
            break;
        }

        int selectedCount = 0;
        if (SUCCEEDED(
                folderView->ItemCount(SVGIO_SELECTION, &selectedCount)) &&
            selectedCount > 0) {
            break;
        }
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

    bool succeeded;
    if (entry.isDirectory) {
        succeeded = CopyDirectoryRecursively(entry.path, targetPath);
    } else {
        succeeded = CopyFileW(entry.path.c_str(), targetPath.c_str(),
                              /*bFailIfExists=*/TRUE) != FALSE;
        if (!succeeded) {
            Wh_Log(L"CopyFile failed: %u", GetLastError());
        }
    }

    // A folder template can be copied only partially, in which case the folder
    // itself is there and worth selecting.
    if (!succeeded &&
        GetFileAttributesW(targetPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return;
    }

    SHChangeNotify(entry.isDirectory ? SHCNE_MKDIR : SHCNE_CREATE, SHCNF_PATHW,
                   targetPath.c_str(), nullptr);

    SelectAndRename(context.shellView, targetPath);
}

////////////////////////////////////////////////////////////////////////////////
// Icons.

#ifndef IO_REPARSE_TAG_APPEXECLINK
#define IO_REPARSE_TAG_APPEXECLINK (0x8000001BL)
#endif

// App execution aliases (e.g. wt.exe) are zero-byte reparse points without
// icons. Resolve them to the target executable.
std::wstring ResolveAppExecutionAlias(std::wstring const& path) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        !(attributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
        return path;
    }

    HANDLE hFile = CreateFileW(
        path.c_str(), FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return path;
    }

    struct AppExecLinkReparseBuffer {
        ULONG reparseTag;
        USHORT reparseDataLength;
        USHORT reserved;
        ULONG version;
        // Four NUL-separated strings follow: package id, entry point,
        // executable path, application type.
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
    const WCHAR* end =
        reinterpret_cast<const WCHAR*>(buffer + bytesReturned);
    for (int i = 0; i < 3 && p < end; i++) {
        size_t length = wcsnlen(p, end - p);
        if (p + length >= end) {
            break;
        }

        if (i == 2) {
            // The executable path.
            return std::wstring(p, length);
        }

        p += length + 1;
    }

    return path;
}

HICON ExtractCommandIcon(std::wstring const& command) {
    std::wstring path = ResolveAppExecutionAlias(ResolveCommandPath(command));

    HICON hIcon = nullptr;
    if (ExtractIconExW(path.c_str(), 0, &hIcon, nullptr, 1) && hIcon) {
        return hIcon;
    }

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(path.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }

    Wh_Log(L"Couldn't get an icon for %s (%s)", command.c_str(),
           path.c_str());
    return nullptr;
}

// A decoded icon: premultiplied BGRA pixels, top-down. Plain pixel data with
// no thread affinity, which is what lets it be cached globally and turned into
// a WriteableBitmap per XamlRoot.
struct DecodedIcon {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;

    bool empty() const { return pixels.empty(); }
};

// Reads a bitmap as 32-bit top-down pixels, whatever its own format is.
bool ReadBitmapPixels(HBITMAP hBitmap, DecodedIcon* decoded) {
    BITMAP bm{};
    if (!GetObject(hBitmap, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0) {
        return false;
    }

    int width = bm.bmWidth;
    int height = bm.bmHeight;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // Top-down.
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<uint8_t> pixels((size_t)width * height * 4);

    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) {
        return false;
    }

    bool succeeded = GetDIBits(hdc, hBitmap, 0, height, pixels.data(), &bmi,
                               DIB_RGB_COLORS) != 0;
    DeleteDC(hdc);

    if (!succeeded) {
        return false;
    }

    decoded->width = width;
    decoded->height = height;
    decoded->pixels = std::move(pixels);
    return true;
}

// True if every pixel is fully transparent, which is how a bitmap without an
// alpha channel comes back. Note that this can't be told apart from an icon
// which is genuinely fully transparent; such an icon ends up opaque (and thus
// visible) rather than invisible, which is the better of the two failure modes
// for a toolbar button.
bool HasNoAlphaChannel(std::vector<uint8_t> const& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        if (pixels[p]) {
            return false;
        }
    }

    return true;
}

void MakeOpaque(std::vector<uint8_t>& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        pixels[p] = 255;
    }
}

// 1-bpp icons have no color bitmap at all: their mask holds an AND mask
// stacked on top of an XOR (monochrome color) mask.
bool DecodeMonochromeIcon(HBITMAP hbmMask, DecodedIcon* decoded) {
    DecodedIcon mask;
    if (!ReadBitmapPixels(hbmMask, &mask) || mask.height % 2 != 0) {
        return false;
    }

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

            // A set AND mask bit means "leave the background alone", i.e.
            // transparent - the pixel stays zeroed.
            if (mask.pixels[andIndex]) {
                continue;
            }

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
    if (!GetIconInfo(hIcon, &iconInfo)) {
        return false;
    }

    bool succeeded = false;

    if (iconInfo.hbmColor) {
        if (ReadBitmapPixels(iconInfo.hbmColor, decoded)) {
            if (HasNoAlphaChannel(decoded->pixels)) {
                MakeOpaque(decoded->pixels);
            } else {
                // The XAML bitmap expects premultiplied alpha.
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

    if (iconInfo.hbmColor) {
        DeleteObject(iconInfo.hbmColor);
    }
    if (iconInfo.hbmMask) {
        DeleteObject(iconInfo.hbmMask);
    }

    return succeeded;
}

muxm::ImageSource CreateImageSource(DecodedIcon const& decoded) try {
    if (decoded.empty()) {
        return nullptr;
    }

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
    if (!glyphSetting[0]) {
        return std::wstring();
    }

    if (!glyphSetting[1]) {
        // A literal glyph character.
        return std::wstring(1, glyphSetting[0]);
    }

    // Every character has to be a hex digit, so that a typo ends up as no icon
    // instead of an unrelated glyph (`abc` would otherwise parse as 0xABC, and
    // `E756x` as 0xE756).
    for (PCWSTR p = glyphSetting; *p; p++) {
        if (!iswxdigit(*p)) {
            Wh_Log(L"%s is not a glyph code point", glyphSetting);
            return std::wstring();
        }
    }

    unsigned long parsed = wcstoul(glyphSetting, nullptr, 16);
    if (parsed > 0 && parsed <= 0xFFFF) {
        return std::wstring(1, (WCHAR)parsed);
    }

    return std::wstring();
}

bool LooksLikeIconPath(std::wstring const& iconSetting) {
    if (iconSetting.find(L'\\') != std::wstring::npos ||
        iconSetting.find(L'/') != std::wstring::npos ||
        iconSetting.find(L':') != std::wstring::npos) {
        return true;
    }

    std::wstring lower = ToLower(iconSetting);

    // Also allow a bare file name, possibly with an icon index.
    size_t comma = lower.rfind(L',');
    if (comma != std::wstring::npos) {
        lower.resize(comma);
    }

    return lower.ends_with(L".exe") || lower.ends_with(L".dll") ||
           lower.ends_with(L".ico");
}

HICON LoadIconFromPath(std::wstring const& iconPath) {
    std::wstring expanded = ExpandEnvVars(iconPath);

    // Support an icon index suffix, e.g. shell32.dll,3.
    int iconIndex = 0;
    size_t comma = expanded.rfind(L',');
    if (comma != std::wstring::npos && comma + 1 < expanded.size()) {
        bool isNumber = true;
        for (size_t i = comma + 1; i < expanded.size(); i++) {
            WCHAR c = expanded[i];
            bool isDigit = c >= L'0' && c <= L'9';
            if (!isDigit && !(i == comma + 1 && c == L'-')) {
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
        hIcon) {
        return hIcon;
    }

    SHFILEINFOW fileInfo{};
    if (SHGetFileInfoW(expanded.c_str(), 0, &fileInfo, sizeof(fileInfo),
                       SHGFI_ICON | SHGFI_LARGEICON)) {
        return fileInfo.hIcon;
    }

    Wh_Log(L"Couldn't load an icon from %s", iconPath.c_str());
    return nullptr;
}

// A "shell:" path, e.g. shell:AppsFolder\<AUMID> for a Store app whose icon
// (Notepad, Calculator, ...) can't be extracted from its legacy .exe stub.
bool IsShellPath(std::wstring const& s) {
    return s.size() >= 6 && _wcsnicmp(s.c_str(), L"shell:", 6) == 0;
}

bool DecodeShellPathIcon(std::wstring const& path, DecodedIcon* decoded) {
    std::wstring expanded = ExpandEnvVars(path);

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(expanded.c_str(), nullptr, &pidl, 0,
                                  nullptr)) ||
        !pidl) {
        Wh_Log(L"Couldn't parse shell path %s", path.c_str());
        return false;
    }

    winrt::com_ptr<IShellItemImageFactory> factory;
    HRESULT hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(factory.put()));
    CoTaskMemFree(pidl);
    if (FAILED(hr) || !factory) {
        return false;
    }

    SIZE size = {32, 32};
    HBITMAP hBitmap = nullptr;
    hr = factory->GetImage(size, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                           &hBitmap);
    if (FAILED(hr) || !hBitmap) {
        return false;
    }

    bool succeeded = ReadBitmapPixels(hBitmap, decoded);
    DeleteObject(hBitmap);

    // The shell returns premultiplied alpha already; just handle the
    // fully-opaque (no alpha channel) case.
    if (succeeded && HasNoAlphaChannel(decoded->pixels)) {
        MakeOpaque(decoded->pixels);
    }

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

// Resolving an icon is expensive - the search path, the App Paths registry
// keys, a reparse point, and for a shell path the package metadata of a Store
// app - and it happens on the Explorer UI thread while a window or a tab is
// being built. The decoded pixels don't depend on the thread or the window, so
// they're resolved once and reused; the cache is dropped when the settings
// change. An entry with no pixels is a remembered failure.
std::mutex g_iconCacheMutex;
std::unordered_map<std::wstring, std::shared_ptr<DecodedIcon>> g_iconCache;

std::shared_ptr<DecodedIcon> GetIcon(std::wstring const& iconSetting,
                                     std::wstring const& command) {
    // '\n' can't appear in either part, so it's an unambiguous separator.
    std::wstring key = iconSetting + L'\n' + command;

    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        auto it = g_iconCache.find(key);
        if (it != g_iconCache.end()) {
            return it->second;
        }
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

// Returns nullptr if no icon could be resolved from the settings.
muxc::IconElement TryCreateIconElement(std::wstring const& iconSetting,
                                       std::wstring const& command) {
    bool isPath = !iconSetting.empty() && LooksLikeIconPath(iconSetting);

    if (isPath || iconSetting.empty()) {
        if (auto source = CreateImageSource(*GetIcon(iconSetting, command))) {
            muxc::ImageIcon imageIcon;
            imageIcon.Source(source);
            return imageIcon;
        }
    }

    std::wstring glyph;
    if (!isPath) {
        glyph = ParseGlyphSetting(iconSetting.c_str());
    }

    if (glyph.empty()) {
        return nullptr;
    }

    return CreateGlyphIcon(glyph.c_str());
}

muxc::IconElement CreateIconElement(std::wstring const& iconSetting,
                                    std::wstring const& command,
                                    PCWSTR defaultGlyph) {
    if (auto icon = TryCreateIconElement(iconSetting, command)) {
        return icon;
    }

    return CreateGlyphIcon(defaultGlyph);
}

// Icon for a command bar button, honoring the item's "Hide icon" option.
// Since the label is collapsed, a button without any icon would render as an
// empty box, so an icon which couldn't be resolved (a mistyped command, a
// missing executable) falls back to a visible placeholder glyph.
muxc::IconElement MakeCommandButtonIcon(ActionItem const& item) {
    if (item.hideIcon) {
        return nullptr;
    }
    return CreateIconElement(item.icon, item.command, L"");
}

// The icon of the New+ button: the icon of Explorer's own New button when the
// setting is empty, so the command bar keeps its familiar look.
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

    // Add, the glyph of Explorer's New button.
    return CreateIconElement(iconSetting, std::wstring(), L"");
}

// The shell icon of a file or folder, the same one File Explorer shows for it.
// Used for the New+ template entries, and deliberately not cached: the menu is
// rebuilt from the templates folder every time it's opened, so a template whose
// icon changed shows the new one.
muxm::ImageSource LoadShellItemIcon(std::wstring const& path) {
    DecodedIcon decoded;
    if (!DecodeShellPathIcon(path, &decoded)) {
        return nullptr;
    }

    return CreateImageSource(decoded);
}

////////////////////////////////////////////////////////////////////////////////
// Managing the buttons in the command bar.

bool IsOurNewPlusButton(muxc::ICommandBarElement const& command) {
    auto element = command.try_as<mux::FrameworkElement>();
    return element && element.Name() == kNewPlusButtonName;
}

bool IsOurElement(muxc::ICommandBarElement const& command) {
    // Matches our action buttons and their separators, plus the special items.
    auto element = command.try_as<mux::FrameworkElement>();
    if (!element) {
        return false;
    }

    std::wstring_view name{element.Name()};
    return name.starts_with(kButtonNamePrefix) || name == kNewPlusButtonName ||
           name == kContextMenuButtonName;
}

// True if the command bar holds any of the elements the given predicate matches.
bool HasElement(muxc::CommandBar const& commandBar,
                bool (*predicate)(muxc::ICommandBarElement const&)) {
    for (auto const& command : commandBar.PrimaryCommands()) {
        if (predicate(command)) {
            return true;
        }
    }

    return false;
}

std::wstring GetButtonIconUri(muxc::AppBarButton const& button) try {
    auto icon = button.Icon();
    if (!icon) {
        return std::wstring();
    }

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

    if (!uri) {
        return std::wstring();
    }

    return std::wstring{uri.AbsoluteUri()};
} catch (...) {
    return std::wstring();
}

// Returns an index into kDefaultButtons, or -1.
int IdentifyDefaultButton(muxc::AppBarButton const& button) {
    std::wstring uri = ToLower(GetButtonIconUri(button));
    if (!uri.empty()) {
        size_t slash = uri.find_last_of(L'/');
        std::wstring_view fileName =
            slash == std::wstring::npos
                ? std::wstring_view(uri)
                : std::wstring_view(uri).substr(slash + 1);

        // The View button's icon reflects the current view mode
        // (windows.iconsize.list.svg, windows.iconsize.details.svg, etc.).
        if (fileName.starts_with(L"windows.iconsize.")) {
            return kViewButtonIndex;
        }

        for (int i = 0; i < kDefaultButtonCount; i++) {
            if (fileName == kDefaultButtons[i].svgFileName) {
                return i;
            }
        }
    }

    // The sort button has an automation id.
    try {
        if (mux::Automation::AutomationProperties::GetAutomationId(button) ==
            L"SortAndGroupButton") {
            return kSortButtonIndex;
        }
    } catch (...) {
    }

    return -1;
}

std::wstring GetAutomationId(mux::FrameworkElement const& element) {
    try {
        return std::wstring{
            mux::Automation::AutomationProperties::GetAutomationId(element)};
    } catch (...) {
        return std::wstring();
    }
}

// Which "hide" setting an element we touch is governed by. Looked up live
// rather than snapshotted, because the visibility watcher below re-applies the
// setting long after the command bar was processed.
enum class ManagedKind {
    Button,           // A default button, index into kDefaultButtons.
    SeparatorAfter,   // A separator, index of the button it follows.
    GroupSeparator,   // A separator that's redundant once its group is empty.
    DetailsToggle,    // The Details pane toggle.
    OverflowElement,  // The "See more" button's separator.
};

// A command a group separator delimits, remembered so the separator can be
// hidden once nothing in its group is left to show.
struct GroupMember {
    winrt::weak_ref<mux::UIElement> element;
    int defaultButtonIndex;  // -1 for commands we don't manage.
};

struct ManagedTarget {
    ManagedKind kind;
    int index = 0;
    // The New button only: whether our New+ button is in this command bar right
    // now. Explorer's New button is hidden for the New+ button's sake, so it
    // must not be hidden when that button isn't actually there - otherwise a
    // command bar which our button hasn't reached yet would end up with no New
    // button at all.
    bool newPlusPresent = false;
    // GroupSeparator only. Shared so the target stays cheap to copy into the
    // visibility watcher below.
    std::shared_ptr<std::vector<GroupMember>> group;
};

bool ShouldHide(ManagedTarget const& target);

// The visibility an element would have if we weren't hiding it: the value
// Explorer last set, which we track, or its current value if we've never
// touched it.
mux::Visibility EffectiveVisibility(mux::UIElement const& element) {
    auto* entry = FindManagedElement(element);
    if (entry && entry->hasOriginalVisibility) {
        return entry->originalVisibility;
    }

    return element.Visibility();
}

// True when every command in the group is hidden, by us or by Explorer, so its
// separator would leave a double line behind.
bool IsGroupHidden(std::shared_ptr<std::vector<GroupMember>> const& group) {
    if (!group || group->empty()) {
        return false;
    }

    for (auto const& member : *group) {
        auto element = member.element.get();
        if (!element) {
            continue;
        }

        if (member.defaultButtonIndex >= 0 &&
            ShouldHide({ManagedKind::Button, member.defaultButtonIndex})) {
            continue;
        }

        if (EffectiveVisibility(element) == mux::Visibility::Collapsed) {
            continue;
        }

        return false;
    }

    return true;
}

bool ShouldHide(ManagedTarget const& target) {
    // Evaluated before taking the settings lock: it consults the setting of
    // each group member itself.
    if (target.kind == ManagedKind::GroupSeparator) {
        return IsGroupHidden(target.group);
    }

    std::lock_guard<std::mutex> lock(g_settings.mutex);
    switch (target.kind) {
        case ManagedKind::Button:
            // The New+ button takes the place of Explorer's New button, so the
            // latter is collapsed unless it's explicitly asked for.
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
        case ManagedKind::GroupSeparator:
            break;
    }

    return false;
}

// Set while we're the ones assigning Visibility, so the watcher can tell our
// own writes apart from Explorer's. The callback runs synchronously on the
// thread doing the write, hence thread_local.
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

// Both a visibility recompute (hiding a group separator depends on the whole
// group, so a single element changing isn't enough to decide) and a full
// update after Explorer touched the command list are deferred and coalesced
// per command bar: Explorer updates a whole run of commands on every selection
// change, and appending our own buttons raises one change notification each.
//
// The pending entries are keyed by address, which is safe here because the
// queued work always removes its own entry: an entry can't outlive the command
// bar it belongs to and be matched against a later one at the same address.
thread_local std::unordered_map<void*, bool>
    g_pendingUpdates;  // Value: full update.

void QueueCommandBarUpdate(
    winrt::weak_ref<muxc::CommandBar> const& weakCommandBar,
    bool fullUpdate) {
    auto commandBar = weakCommandBar.get();
    if (!commandBar) {
        return;
    }

    void* key = winrt::get_abi(commandBar);
    auto [it, inserted] = g_pendingUpdates.insert({key, fullUpdate});
    if (!inserted) {
        // Already queued; the queued work reads the flag when it runs, so a
        // full update can still be requested on top of a recompute.
        it->second = it->second || fullUpdate;
        return;
    }

    // Runs on this same thread, either from the dispatcher queue or right
    // below if queueing failed.
    auto takePending = [key]() {
        bool full = false;
        auto it = g_pendingUpdates.find(key);
        if (it != g_pendingUpdates.end()) {
            full = it->second;
            g_pendingUpdates.erase(it);
        }

        return full;
    };

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::
            GetForCurrentThread();
    if (!dispatcherQueue) {
        takePending();
        return;
    }

    if (!dispatcherQueue.TryEnqueue([weakCommandBar, takePending]() {
            bool full = takePending();

            if (g_unloading) {
                return;
            }

            if (auto commandBar = weakCommandBar.get()) {
                try {
                    if (full) {
                        UpdateCommandBar(commandBar);
                    } else {
                        ApplyDefaultButtonVisibility(commandBar);
                    }
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
    QueueCommandBarUpdate(weakCommandBar, /*fullUpdate=*/false);
}

// Explorer shows and hides its contextual commands (Set as background, Rotate
// left/right, Extract all, ...) as the selection changes, which overwrites our
// collapse. Watching the property lets us re-apply immediately, and keeps the
// remembered original in sync so disabling the mod restores what Explorer
// wanted rather than what it happened to be when we first saw the element.
// The caller has established, under the lock, that the element isn't watched
// yet.
void WatchVisibility(mux::UIElement const& element,
                     ManagedTarget const& target,
                     winrt::weak_ref<muxc::CommandBar> const& owner) {
    int64_t token = element.RegisterPropertyChangedCallback(
        mux::UIElement::VisibilityProperty(),
        [target, owner](mux::DependencyObject const& sender,
                        mux::DependencyProperty const&) {
            if (g_unloading || g_settingVisibilityDepth > 0) {
                return;
            }

            auto element = sender.try_as<mux::UIElement>();
            if (!element) {
                return;
            }

            // Explorer changed it, so this is the new baseline to restore.
            mux::Visibility visibility = element.Visibility();
            {
                auto& entry = GetManagedElement(element);
                entry.originalVisibility = visibility;
                entry.hasOriginalVisibility = true;
            }

            // A group separator isn't decided here: the group snapshot this
            // watcher was registered with can be stale, and the recompute
            // below re-collects it anyway.
            if (target.kind != ManagedKind::GroupSeparator &&
                visibility != mux::Visibility::Collapsed &&
                ShouldHide(target)) {
                SetVisibilityInternal(element, mux::Visibility::Collapsed);
            }

            // This element may belong to a group whose separator now has to
            // appear or disappear with it.
            QueueVisibilityRecompute(owner);
        });

    auto& entry = GetManagedElement(element);
    entry.watched = true;
    entry.visibilityToken = token;
}

// Unregisters the watchers of this thread's elements, keeping their remembered
// original state for the restore which follows.
void UnwatchVisibilityForCurrentThread() {
    for (auto& entry : g_managedElements) {
        if (!entry.watched) {
            continue;
        }

        auto element = entry.element.get();
        int64_t token = entry.visibilityToken;

        entry.watched = false;
        entry.visibilityToken = 0;

        if (!element) {
            continue;
        }

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

// Sets an element's visibility to Collapsed when hiding, or restores its
// original (pre-modification) visibility otherwise. The original is captured
// the first time the element is seen, so we never force-show something
// Explorer intentionally kept collapsed.
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

    if (watch) {
        WatchVisibility(element, target, owner);
    }

    SetVisibilityInternal(element, !forceShow && ShouldHide(target)
                                       ? mux::Visibility::Collapsed
                                       : original);
}

// Controls the gap between command bar buttons. Command bar buttons have a
// fixed MinWidth that leaves padding around the icon, so we drop it to let the
// button hug its content and use the horizontal margin (half of `spacing` on
// each side) as the actual gap. A negative spacing (or reset) restores the
// button's original margin and MinWidth, leaving the default untouched.
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

mux::FrameworkElement FindDescendantByName(mux::DependencyObject const& root,
                                           std::wstring_view name) {
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto element = child.try_as<mux::FrameworkElement>();
            element && element.Name() == name) {
            return element;
        }

        if (auto found = FindDescendantByName(child, name)) {
            return found;
        }
    }

    return nullptr;
}

void ApplyDefaultButtonVisibility(muxc::CommandBar const& commandBar,
                                  bool forceShow) {
    bool hideMore;
    int itemSpacing;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        hideMore = g_settings.hideMoreButton;
        itemSpacing = g_settings.itemSpacing;
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

    // Classify everything up front: identifying a button is relatively
    // expensive, and the separator handling below needs to look ahead.
    struct Entry {
        muxc::ICommandBarElement command;
        bool isOurs = false;
        // The New+ button sits among Explorer's own buttons rather than after
        // them, so it doesn't mark the start of our own elements.
        bool isNewPlus = false;
        bool isSeparator = false;
        bool isDetailsToggle = false;
        int defaultIndex = -1;  // Index into kDefaultButtons, or -1.
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
                // The Details pane toggle lives in the secondary command bar.
                if (GetAutomationId(button) == L"DetailsPaneToggleButton") {
                    entry.isDetailsToggle = true;
                } else {
                    entry.defaultIndex = IdentifyDefaultButton(button);
                    if (entry.defaultIndex < 0) {
                        // Expected for the contextual commands Explorer keeps
                        // in the list, but it's also what a built-in button
                        // whose icon file was renamed in a new Windows build
                        // looks like, so log enough to tell the two apart.
                        Wh_Log(L"Unrecognized command %u: icon %s, "
                               L"automation id %s",
                               i, GetButtonIconUri(button).c_str(),
                               GetAutomationId(button).c_str());
                    }
                }
            }
        }

        entries.push_back(std::move(entry));
    }

    // Explorer keeps ~20 contextual, zero-width commands (Extract, Eject, ...)
    // in the list between the View group and the overflow region. The vertical
    // line the user sees "after View" is really the separator right before our
    // first custom button, so locate that one.
    uint32_t firstCustomIndex = count;
    bool hasNewPlusButton = false;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].isNewPlus) {
            hasNewPlusButton = true;
        } else if (entries[i].isOurs && firstCustomIndex == count) {
            firstCustomIndex = i;
        }
    }

    uint32_t viewSeparatorIndex = count;
    if (firstCustomIndex > 0 && firstCustomIndex < count &&
        entries[firstCustomIndex - 1].isSeparator) {
        viewSeparatorIndex = firstCustomIndex - 1;
    } else {
        // The mod can also be used just to hide the built-in buttons, without
        // any custom ones. Then it's the last of Explorer's own separators -
        // the same element our buttons would have followed, so the option keeps
        // targeting the same line either way.
        for (uint32_t i = count; i > 0; i--) {
            if (entries[i - 1].isSeparator) {
                viewSeparatorIndex = i - 1;
                break;
            }
        }
    }

    // The commands a separator introduces, up to the next separator (or our
    // own buttons, or the end of the list). Explorer puts a separator in front
    // of its contextual commands; once all of them are hidden, that separator
    // would sit right next to the next one as a double line, so it's hidden
    // together with the group it belongs to.
    auto collectGroup = [&entries, count](uint32_t separatorIndex) {
        auto group = std::make_shared<std::vector<GroupMember>>();

        for (uint32_t i = separatorIndex + 1; i < count; i++) {
            auto const& entry = entries[i];
            if (entry.isNewPlus) {
                // Neither a member of the group nor its end: it's one of ours,
                // but it stands between Explorer's own buttons.
                continue;
            }

            if (entry.isSeparator || entry.isOurs) {
                break;
            }

            if (auto element = entry.command.try_as<mux::UIElement>()) {
                group->push_back(
                    {winrt::make_weak(element), entry.defaultIndex});
            }
        }

        return group;
    };

    // The default-button index of the immediately preceding element, so a
    // separator can be attributed to the button right before it.
    int prevIndex = -1;
    for (uint32_t i = 0; i < count; i++) {
        auto const& entry = entries[i];

        if (entry.isOurs) {
            // Our custom buttons get the spacing too, for a uniform look.
            if (auto button = entry.command.try_as<muxc::AppBarButton>()) {
                ApplyItemSpacing(button, itemSpacing, forceShow);
            }
            prevIndex = -1;
            continue;
        }

        if (entry.isSeparator) {
            auto separator = entry.command.as<muxc::AppBarSeparator>();

            int target = -1;
            if (prevIndex == kNewButtonIndex) {
                target = kNewButtonIndex;
            } else if (prevIndex == kDeleteButtonIndex) {
                target = kDeleteButtonIndex;
            } else if (i == viewSeparatorIndex) {
                target = kViewButtonIndex;
            }

            if (target >= 0) {
                setVisibility(separator,
                              {ManagedKind::SeparatorAfter, target});
            } else {
                // One of Explorer's contextual separators. Hidden only when
                // everything it introduces is hidden, so we never reveal a
                // separator Explorer keeps collapsed.
                setVisibility(separator,
                              {ManagedKind::GroupSeparator, -1,
                               /*newPlusPresent=*/false, collectGroup(i)});
            }

            prevIndex = -1;
            continue;
        }

        if (entry.isDetailsToggle) {
            setVisibility(entry.command.as<mux::UIElement>(),
                          {ManagedKind::DetailsToggle});
            prevIndex = -1;
            continue;
        }

        if (entry.defaultIndex >= 0) {
            auto button = entry.command.as<muxc::AppBarButton>();
            setVisibility(button, {ManagedKind::Button, entry.defaultIndex,
                                   hasNewPlusButton});
            // Spacing is applied only to real buttons, never the zero-width
            // contextual commands (which would otherwise occupy space).
            ApplyItemSpacing(button, itemSpacing, forceShow);
        }

        prevIndex = entry.defaultIndex;
    }

    // The "See more" (...) overflow button and its separator (a template
    // part). OverflowButtonVisibility alone doesn't hide the separator in
    // Explorer's template, so collapse it explicitly too.
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
                                    muxc::IconElement const& icon) {
    std::wstring name = kButtonNamePrefix;
    name += L'_';
    name += std::to_wstring(index);

    muxc::AppBarButton button;
    button.Name(name.c_str());
    button.Label(tooltip.c_str());
    button.LabelPosition(muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(icon);

    if (!tooltip.empty()) {
        muxc::ToolTipService::SetToolTip(button, winrt::box_value(
                                                     winrt::hstring{tooltip}));
    }

    return button;
}

muxc::AppBarButton CreateActionButton(ActionItem const& item, int index) {
    muxc::AppBarButton button =
        CreateBareButton(index, item.name, MakeCommandButtonIcon(item));

    TrackRevoker(button,
                 button.Click(winrt::auto_revoke,
                              [item](wf::IInspectable const& sender,
                                     mux::RoutedEventArgs const&) {
                                  if (auto element =
                                          sender.try_as<mux::FrameworkElement>()) {
                                      OnActionInvoked(element, item);
                                  }
                              }));

    return button;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton);

// A regular menu item, or a submenu with a further level of entries.
muxc::MenuFlyoutItemBase CreateMenuEntry(
    ActionItem const& item,
    winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    if (item.isMenu && !item.subItems.empty()) {
        muxc::MenuFlyoutSubItem subMenu;
        subMenu.Text(item.name.c_str());
        if (!item.hideIcon) {
            if (auto icon = TryCreateIconElement(item.icon, item.command)) {
                subMenu.Icon(icon);
            }
        }

        AppendMenuEntries(item.subItems, subMenu.Items(), weakButton);
        return subMenu;
    }

    muxc::MenuFlyoutItem menuItem;
    menuItem.Text(item.name.c_str());
    if (!item.hideIcon) {
        if (auto icon = TryCreateIconElement(item.icon, item.command)) {
            menuItem.Icon(icon);
        }
    }

    TrackRevoker(menuItem,
                 menuItem.Click(winrt::auto_revoke,
                                [item, weakButton](wf::IInspectable const&,
                                                   mux::RoutedEventArgs const&) {
                                    if (auto button = weakButton.get()) {
                                        OnActionInvoked(button, item);
                                    }
                                }));

    return menuItem;
}

void AppendMenuEntries(std::vector<ActionItem> const& items,
                       wfc::IVector<muxc::MenuFlyoutItemBase> const& target,
                       winrt::weak_ref<muxc::AppBarButton> const& weakButton) {
    for (auto const& item : items) {
        target.Append(CreateMenuEntry(item, weakButton));

        if (item.separatorAfter) {
            target.Append(muxc::MenuFlyoutSeparator());
        }
    }
}

// The hover timers of our menu buttons, with their Tick registration. A started
// timer is rooted by the dispatcher queue, and its Tick handler lives in this
// DLL, so a timer has to be stopped and its handler revoked before the mod is
// unloaded. The timer is held strongly - unlike the elements, DispatcherTimer
// isn't a DependencyObject, so it isn't necessarily weak-referenceable - and
// released once the button it belongs to is gone.
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
    // Drop the timers whose button is gone.
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

    for (auto const& entry : taken) {
        ReleaseHoverTimer(entry);
    }
}

// Makes hovering over the button do what clicking it does, after the configured
// delay. Used by the buttons of ours which open a menu: the dropdowns and the
// New+ button.
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

    // The handler gets the timer as its sender, so it doesn't have to capture
    // it - which would be either a reference cycle or a weak reference on a type
    // that isn't a DependencyObject.
    auto tickToken =
        timer.Tick([open](wf::IInspectable const& sender,
                          wf::IInspectable const&) {
            if (auto timer = sender.try_as<mux::DispatcherTimer>()) {
                timer.Stop();
            }
            open();
        });

    TrackHoverTimer(button, timer, tickToken);

    TrackRevoker(button,
                 button.PointerEntered(
                     winrt::auto_revoke,
                     [timer](wf::IInspectable const&,
                             mux::Input::PointerRoutedEventArgs const&) {
                         timer.Stop();  // Restart the delay.
                         timer.Start();
                     }));

    auto stopTimer = [timer](wf::IInspectable const&,
                             mux::Input::PointerRoutedEventArgs const&) {
        timer.Stop();
    };
    TrackRevoker(button, button.PointerExited(winrt::auto_revoke, stopTimer));
    TrackRevoker(button, button.PointerCanceled(winrt::auto_revoke, stopTimer));
}

// Shows the button's own flyout, for the buttons which have one.
std::function<void()> MakeShowFlyoutAction(muxc::AppBarButton const& button) {
    return [weakButton = winrt::make_weak(button)]() {
        auto button = weakButton.get();
        if (!button || g_unloading) {
            return;
        }

        if (auto flyout = button.Flyout(); flyout && !flyout.IsOpen()) {
            flyout.ShowAt(button);
        }
    };
}

muxc::AppBarButton CreateMenuButton(ActionItem const& item,
                                    int index,
                                    bool openOnHover,
                                    int hoverDelayMs) {
    muxc::AppBarButton button =
        CreateBareButton(index, item.name, MakeCommandButtonIcon(item));

    // The menu flyout is shown in its own popup window, so resolve the
    // Explorer window from the anchor button, not from the clicked menu item.
    auto weakButton = winrt::make_weak(button);

    muxc::MenuFlyout menu;
    menu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    // Every menu entry resolves an icon, which is the expensive part of
    // building the menu, so the entries are created the first time the menu is
    // needed instead of while the window or the tab is being built.
    auto ensureMenuEntries = [subItems = item.subItems, weakButton](
                                 muxc::MenuFlyout const& menu) {
        if (!menu || menu.Items().Size() > 0) {
            return;
        }

        try {
            AppendMenuEntries(subItems, menu.Items(), weakButton);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    };

    TrackRevoker(menu,
                 menu.Opening(winrt::auto_revoke,
                              [ensureMenuEntries](
                                  wf::IInspectable const& sender,
                                  wf::IInspectable const&) {
                                  ensureMenuEntries(
                                      sender.try_as<muxc::MenuFlyout>());
                              }));

    // Also while the pointer is on its way to the button, which is both a
    // little earlier than the click and a safety net for the case above.
    TrackRevoker(
        button,
        button.PointerEntered(
            winrt::auto_revoke,
            [ensureMenuEntries, weakButton](
                wf::IInspectable const&,
                mux::Input::PointerRoutedEventArgs const&) {
                if (auto button = weakButton.get()) {
                    ensureMenuEntries(
                        button.Flyout().try_as<muxc::MenuFlyout>());
                }
            }));

    button.Flyout(menu);

    if (openOnHover) {
        SetUpOpenOnHover(button, hoverDelayMs, MakeShowFlyoutAction(button));
    }

    return button;
}

void OpenContextMenuForElement(mux::FrameworkElement const& element) {
    if (g_unloading || !element) {
        return;
    }

    HWND hExplorerWnd = GetExplorerWindowForElement(element);
    if (!hExplorerWnd) {
        Wh_Log(L"No File Explorer window for the context menu item");
        return;
    }

    POINT point{};
    GetCursorPos(&point);
    ShowShellContextMenu(hExplorerWnd, point);
}

muxc::AppBarButton CreateContextMenuButton(bool openOnHover,
                                            int hoverDelayMs) {
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
    button.Icon(CreateIconElement(settings.buttonIcon, std::wstring(),
                                  L"\uE8FD"));

    if (!settings.showLabel && !settings.buttonLabel.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{settings.buttonLabel}));
    }

    TrackRevoker(
        button,
        button.Click(
            winrt::auto_revoke,
            [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
                OpenContextMenuForElement(
                    sender.try_as<mux::FrameworkElement>());
            }));

    if (openOnHover) {
        auto open = [weakButton = winrt::make_weak(button)]() {
            if (auto button = weakButton.get()) {
                OpenContextMenuForElement(button);
            }
        };
        SetUpOpenOnHover(button, hoverDelayMs, open);
    }

    return button;
}

////////////////////////////////////////////////////////////////////////////////
// The New+ button, which takes the place of Explorer's New button.

// Rebuilds the flyout contents from the templates folder. Done every time the
// menu opens, so newly added templates show up without reloading the mod.
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

        // The item's tooltip shows the name as it is on disk, which the display
        // name may hide parts of.
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
                    if (g_unloading) {
                        return;
                    }

                    // The flyout lives in its own popup window, so the Explorer
                    // window is resolved from the anchor button.
                    auto button = weakButton.get();
                    if (!button) {
                        return;
                    }

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
        if (!templates.empty()) {
            items.Append(muxc::MenuFlyoutSeparator());
        }

        muxc::MenuFlyoutItem openFolderItem;
        openFolderItem.Text(L"Open templates folder");
        openFolderItem.Icon(CreateGlyphIcon(L""));  // OpenFolderHorizontal.

        TrackRevoker(
            openFolderItem,
            openFolderItem.Click(
                winrt::auto_revoke,
                [folder = config.templateFolder](wf::IInspectable const&,
                                                 mux::RoutedEventArgs const&) {
                    if (g_unloading) {
                        return;
                    }

                    RunShellWorkOnWorkerThread([folder]() {
                        if (!DirectoryExists(folder)) {
                            SHCreateDirectoryExW(nullptr, folder.c_str(),
                                                 nullptr);
                        }

                        ShellExecuteW(nullptr, L"open", folder.c_str(), nullptr,
                                      nullptr, SW_SHOWNORMAL);
                    });
                }));

        items.Append(openFolderItem);
    }
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

void AddNewPlusChevron(muxc::AppBarButton const& button) try {
    auto label =
        FindDescendantByName(button, L"TextLabel").try_as<muxc::TextBlock>();
    if (!label) {
        Wh_Log(L"The New+ label wasn't found");
        return;
    }

    std::wstring currentText = label.Text().c_str();
    if (!currentText.empty() && currentText.back() == L'\uE70D') {
        return;
    }

    // Use a separate text run so the label keeps Explorer's normal typeface
    // while the chevron is rendered as the Segoe Fluent Icons ChevronDown
    // symbol, just like the built-in New button.
    auto inlines = label.Inlines();
    inlines.Clear();

    muxd::Run text;
    std::wstring labelText = button.Label().c_str();
    labelText += L"  ";
    text.Text(labelText);
    inlines.Append(text);

    muxd::Run chevron;
    chevron.Text(L"\uE70D");
    chevron.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons"));
    chevron.FontSize(8);
    inlines.Append(chevron);
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

    // Explorer's command-bar template doesn't add a flyout indicator to an
    // AppBarButton automatically. Without a label, only the icon is shown and
    // the text becomes the tooltip.
    bool showLabel = settings.showLabel && !settings.buttonLabel.empty();

    muxc::AppBarButton button;
    button.Name(kNewPlusButtonName);
    button.Label(settings.buttonLabel.c_str());
    button.LabelPosition(showLabel
                             ? muxc::CommandBarLabelPosition::Default
                             : muxc::CommandBarLabelPosition::Collapsed);
    button.Icon(
        MakeNewPlusButtonIcon(settings.buttonIcon, originalIconUri));

    if (showLabel) {
        TrackRevoker(
            button,
            button.Loaded(
                winrt::auto_revoke,
                [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
                    if (auto button = sender.try_as<muxc::AppBarButton>()) {
                        AddNewPlusChevron(button);
                    }
                }));
    }

    // With the label visible there's nothing a tooltip could add.
    if (!showLabel && !settings.buttonLabel.empty()) {
        muxc::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{settings.buttonLabel}));
    }

    auto weakButton = winrt::make_weak(button);

    muxc::MenuFlyout menu;
    menu.Placement(
        muxc::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    TrackRevoker(menu,
                 menu.Opening(winrt::auto_revoke,
                              [weakButton](wf::IInspectable const& sender,
                                           wf::IInspectable const&) {
                                  if (g_unloading) {
                                      return;
                                  }

                                  if (auto menu =
                                          sender.try_as<muxc::MenuFlyout>()) {
                                      PopulateNewPlusMenu(menu, weakButton);
                                  }
                              }));

    button.Flyout(menu);

    if (openOnHover) {
        SetUpOpenOnHover(button, hoverDelayMs, MakeShowFlyoutAction(button));
    }

    return button;
}

////////////////////////////////////////////////////////////////////////////////
// Inserting our elements into the command bar.

void EnsureActionButtons(muxc::CommandBar const& commandBar) {
    if (g_unloading) {
        return;
    }

    if (HasElement(commandBar, [](muxc::ICommandBarElement const& command) {
            auto element = command.try_as<mux::FrameworkElement>();
            return element && std::wstring_view(element.Name())
                                  .starts_with(kButtonNamePrefix);
        })) {
        return;
    }

    bool openMenuOnHover;
    int menuHoverDelay;
    std::vector<ActionItem> items;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
        items = g_settings.items;
    }

    if (items.empty()) {
        return;
    }

    Wh_Log(L"Adding %zu items to command bar", items.size());

    auto commands = commandBar.PrimaryCommands();
    for (size_t i = 0; i < items.size(); i++) {
        auto const& item = items[i];
        if (item.isMenu && !item.subItems.empty()) {
            commands.Append(CreateMenuButton(item, (int)i, openMenuOnHover,
                                             menuHoverDelay));
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
    bool enabled;
    bool openMenuOnHover;
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
        })) {
        return;
    }

    Wh_Log(L"Adding the context menu item");
    commandBar.PrimaryCommands().Append(
        CreateContextMenuButton(openMenuOnHover, menuHoverDelay));
}

// Puts the New+ button where Explorer's New button is. The latter is collapsed
// by ApplyDefaultButtonVisibility, which asks ShouldHide - and that returns true
// for the New button while the New+ button is enabled.
void EnsureNewPlusButton(muxc::CommandBar const& commandBar) {
    if (g_unloading) {
        return;
    }

    bool openMenuOnHover;
    int menuHoverDelay;
    bool enabled;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        enabled = g_settings.newPlus.enabled;
        openMenuOnHover = g_settings.openMenuOnHover;
        menuHoverDelay = g_settings.menuHoverDelay;
    }

    if (!enabled || HasElement(commandBar, IsOurNewPlusButton)) {
        return;
    }

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    uint32_t newButtonIndex = count;
    std::wstring originalIconUri;
    for (uint32_t i = 0; i < count; i++) {
        auto command = commands.GetAt(i);
        if (IsOurElement(command)) {
            continue;
        }

        if (auto button = command.try_as<muxc::AppBarButton>();
            button && IdentifyDefaultButton(button) == kNewButtonIndex) {
            newButtonIndex = i;
            originalIconUri = GetButtonIconUri(button);
            break;
        }
    }

    if (newButtonIndex == count) {
        // Explorer hasn't filled the command bar in yet, or this build has no
        // New button at all. Appending would put our button somewhere it doesn't
        // belong, and since it's only ever inserted once it would stay there, so
        // leave it to the update which follows Explorer's own commands being
        // added. Explorer's New button stays visible until ours is really there.
        Wh_Log(L"The New button isn't in the command bar (yet)");
        return;
    }

    Wh_Log(L"Adding the New+ button");
    commands.InsertAt(
        newButtonIndex,
        CreateNewPlusButton(originalIconUri, openMenuOnHover, menuHoverDelay));
}

void UpdateCommandBar(muxc::CommandBar const& commandBar) {
    if (g_unloading) {
        return;
    }

    // Our buttons only go in the primary command bar; the secondary bar just
    // holds the Details pane toggle.
    if (commandBar.Name() == L"FileExplorerCommandBar") {
        EnsureNewPlusButton(commandBar);
        EnsureActionButtons(commandBar);
        EnsureContextMenuButton(commandBar);
    }

    ApplyDefaultButtonVisibility(commandBar);
}

void RemoveOurButtons(muxc::CommandBar const& commandBar) {
    auto commands = commandBar.PrimaryCommands();
    for (uint32_t i = commands.Size(); i > 0; i--) {
        auto command = commands.GetAt(i - 1);
        if (!IsOurElement(command)) {
            continue;
        }

        // An open menu flyout lives in a popup rooted by the XamlRoot, not by
        // the button, so it would stay on screen after the button is gone.
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

void OnCommandBarAdded(muxc::CommandBar const& commandBar) {
    // Prune entries whose command bar is gone. Only this thread's, since
    // g_entries is thread_local - a command bar can only ever be tracked by
    // the thread which owns it.
    for (auto it = g_entries.begin(); it != g_entries.end();) {
        if (!it->commandBar.get()) {
            it = g_entries.erase(it);
        } else {
            ++it;
        }
    }

    for (auto const& entry : g_entries) {
        if (entry.commandBar.get() == commandBar) {
            // Already tracked (e.g. re-added to the tree).
            return;
        }
    }

    CommandBarEntry entry;
    entry.commandBar = winrt::make_weak(commandBar);

    // Re-add the buttons whenever the command bar reloads.
    entry.loadedToken = commandBar.Loaded(
        [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
            if (auto commandBar = sender.try_as<muxc::CommandBar>()) {
                UpdateCommandBar(commandBar);
            }
        });

    // Re-add the buttons and reapply visibility if Explorer ever rebuilds
    // the command list.
    entry.vectorChangedToken = commandBar.PrimaryCommands().VectorChanged(
        [weakCommandBar = winrt::make_weak(commandBar)](
            wfc::IObservableVector<muxc::ICommandBarElement> const&,
            wfc::IVectorChangedEventArgs const&) {
            if (g_unloading) {
                return;
            }

            // Defer the update; mutating the vector from within its own change
            // notification isn't allowed. Coalesced, since adding our own
            // buttons raises one notification per button.
            QueueCommandBarUpdate(weakCommandBar, /*fullUpdate=*/true);
        });

    g_entries.push_back(std::move(entry));

    UpdateCommandBar(commandBar);
}

void RemoveButtonsForCurrentThread() {
    // Before restoring anything, so the watcher can't fight the restore, and
    // before the buttons go away, while the elements the handlers are
    // registered on are still around.
    UnwatchVisibilityForCurrentThread();
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<CommandBarEntry> taken;
    taken.swap(g_entries);

    for (auto& entry : taken) {
        auto commandBar = entry.commandBar.get();
        if (!commandBar) {
            continue;
        }

        try {
            commandBar.Loaded(entry.loadedToken);
            commandBar.PrimaryCommands().VectorChanged(
                entry.vectorChangedToken);
            RemoveOurButtons(commandBar);
            ApplyDefaultButtonVisibility(commandBar, /*forceShow=*/true);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }

    // Everything has been restored, so the remembered original state of this
    // thread's elements isn't needed anymore. Any update still queued on the
    // dispatcher gives up on its own, since g_unloading is set.
    ForgetManagedElementsForCurrentThread();
    g_pendingUpdates.clear();
    g_threadScanned = false;
    DestroyContextMenuOwnerWindowForCurrentThread();
}

void RefreshButtonsForCurrentThread() {
    // The buttons are recreated below, so the hover timers and event handlers
    // of the old ones are released first.
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    // A copy, since UpdateCommandBar below can add entries.
    std::vector<winrt::weak_ref<muxc::CommandBar>> commandBars;
    for (auto const& entry : g_entries) {
        commandBars.push_back(entry.commandBar);
    }

    for (auto const& weakCommandBar : commandBars) {
        auto commandBar = weakCommandBar.get();
        if (!commandBar) {
            continue;
        }

        try {
            // Recreate the buttons with the new settings.
            RemoveOurButtons(commandBar);
            UpdateCommandBar(commandBar);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Command bar discovery.
//
// The command bars are found by hooking a couple of functions of File
// Explorer's own WinUI 3 code (FileExplorerExtensions.dll) and by walking the
// XAML tree from there with the public VisualTreeHelper API.
//
// Note: XAML Diagnostics (InitializeXamlDiagnosticsEx) would be an easier way
// to watch for the command bar, but only one XAML diagnostics consumer can be
// active per process, which makes it conflict with other tools and mods, such
// as Windows 11 File Explorer Styler. That's why it's not used here.

// Note: the `this` pointer our hooks get is a C++/WinRT implementation object,
// not a XAML object, and there's no supported way to turn one into the other.
// Guessing at its layout is a good way to crash Explorer, so the hooks which
// only have a `this` pointer are used as a signal to rescan the thread, and the
// XAML objects themselves only ever come from typed parameters, from the
// command bars we already know about, or from the focused element.

bool IsTargetCommandBarName(std::wstring_view name) {
    return name == L"FileExplorerCommandBar" ||
           name == L"FileExplorerSecondaryCommandBar";
}

// Both command bars are found, so there's nothing left to look for.
bool FoundAllCommandBars(std::vector<muxc::CommandBar> const& commandBars) {
    bool primary = false;
    bool secondary = false;

    for (auto const& commandBar : commandBars) {
        if (commandBar.Name() == L"FileExplorerCommandBar") {
            primary = true;
        } else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
            secondary = true;
        }
    }

    return primary && secondary;
}

void CollectCommandBars(mux::DependencyObject const& root,
                        int depth,
                        std::vector<muxc::CommandBar>* commandBars) {
    if (depth > 64) {
        return;
    }

    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        // The scan runs on every focus event, so stop walking the tree as soon
        // as there's nothing left to find.
        if (FoundAllCommandBars(*commandBars)) {
            return;
        }

        auto child = muxm::VisualTreeHelper::GetChild(root, i);
        if (auto commandBar = child.try_as<muxc::CommandBar>();
            commandBar && IsTargetCommandBarName(commandBar.Name())) {
            commandBars->push_back(std::move(commandBar));
            // No need to descend into a command bar we already found.
            continue;
        }

        CollectCommandBars(child, depth + 1, commandBars);
    }
}

// Finds all of the File Explorer command bars of the XAML island the given
// element belongs to. Must run on the element's UI thread.
void ScanXamlRootForCommandBars(mux::UIElement const& element) try {
    if (g_unloading || !element) {
        return;
    }

    auto xamlRoot = element.XamlRoot();
    if (!xamlRoot) {
        return;
    }

    auto content = xamlRoot.Content();
    if (!content) {
        return;
    }

    std::vector<muxc::CommandBar> commandBars;
    CollectCommandBars(content, 0, &commandBars);
    for (auto const& commandBar : commandBars) {
        OnCommandBarAdded(commandBar);
    }

    if (!commandBars.empty()) {
        // The island is built and its command bars are tracked; the focus hook
        // doesn't have to keep looking. Not conditioned on both bars being
        // found, since a layout without a secondary bar would otherwise be
        // rescanned on every focus change.
        g_threadScanned = true;
    }
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Same as above, but deferred, for the cases where the command bar isn't in
// the tree yet by the time our hook runs.
void ScheduleXamlRootScan(mux::UIElement const& element) try {
    if (g_unloading || !element) {
        return;
    }

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanXamlRootForCommandBars(element);
        return;
    }

    dispatcherQueue.TryEnqueue([weakElement = winrt::make_weak(element)]() {
        if (auto element = weakElement.get()) {
            ScanXamlRootForCommandBars(element);
        }
    });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

muxc::CommandBar GetKnownCommandBarForCurrentThread() {
    for (auto const& entry : g_entries) {
        if (auto commandBar = entry.commandBar.get()) {
            return commandBar;
        }
    }

    return nullptr;
}

// Looks for the command bars of the current thread's XAML island without an
// element to start from: either from a command bar which is already known for
// this thread, or from the focused element. Must run on the UI thread.
void ScanCurrentThreadForCommandBars() try {
    if (g_unloading) {
        return;
    }

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

// Same as above, deferred to after the current layout pass.
void ScheduleCurrentThreadScan() try {
    if (g_unloading) {
        return;
    }

    auto dispatcherQueue =
        winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanCurrentThreadForCommandBars();
        return;
    }

    dispatcherQueue.TryEnqueue([]() { ScanCurrentThreadForCommandBars(); });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks of File Explorer's WinUI 3 code.

// void CommandBarManager::CommandBar(muxc::CommandBar const& value)
//
// Called with the command bar element itself, which is the most direct way to
// get hold of it.
using CommandBarManager_CommandBar_t = void(WINAPI*)(void* pThis,
                                                     void* commandBar);
CommandBarManager_CommandBar_t CommandBarManager_CommandBar_Original;
void WINAPI CommandBarManager_CommandBar_Hook(void* pThis, void* commandBar) {
    Wh_Log(L">");

    CommandBarManager_CommandBar_Original(pThis, commandBar);

    if (g_unloading || !commandBar) {
        return;
    }

    try {
        auto const& element =
            *reinterpret_cast<muxc::CommandBar const*>(commandBar);
        if (!element) {
            return;
        }

        Wh_Log(L"Command bar %s, thread %u", element.Name().c_str(),
               GetCurrentThreadId());
        OnCommandBarAdded(element);

        // The secondary command bar has no manager of its own, and the bar
        // isn't necessarily attached to the tree yet, so also scan the island
        // once the current layout pass is done.
        ScheduleXamlRootScan(element);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

// void CommandBarControl::OnApplyTemplate()
//
// Runs when the control which hosts the command bar builds its contents, both
// for new windows and tabs and when Explorer rebuilds it.
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

// void CommandBarControl::CommandBarControlGotFocusHandler(
//     IInspectable const& sender, RoutedEventArgs const&)
//
// A cheap extra chance to pick up a command bar we haven't seen yet, e.g. in a
// window which was already open when the mod was loaded.
using CommandBarControl_GotFocusHandler_t = void(WINAPI*)(void* pThis,
                                                          void* sender,
                                                          void* args);
CommandBarControl_GotFocusHandler_t
    CommandBarControl_GotFocusHandler_Original;
CommandBarControl_GotFocusHandler_t
    CommandBarControl_Wave1_GotFocusHandler_Original;

void HandleCommandBarControlGotFocus(void* sender) {
    if (g_unloading || !sender) {
        return;
    }

    // This runs on every focus change, and the scan walks the whole visual
    // tree, so skip it once this thread's command bars are known. A rebuilt
    // command bar comes back through OnApplyTemplate / Loaded instead.
    if (g_threadScanned && GetKnownCommandBarForCurrentThread()) {
        return;
    }

    try {
        auto const& inspectable = *reinterpret_cast<wf::IInspectable const*>(
            sender);
        if (auto element = inspectable ? inspectable.try_as<mux::UIElement>()
                                       : nullptr) {
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

std::atomic<bool> g_symbolsHooked;

enum class SymbolHookResult {
    Success,
    // Symbol resolution itself failed, e.g. the PDB couldn't be downloaded.
    // Transient, so trying again later can succeed.
    ResolutionFailed,
    // The symbols resolved, but this build has none of the functions the mod
    // needs. Permanent for this Explorer, so it must not be retried: each
    // HookSymbols call for a module invalidates its symbol cache and forces a
    // re-resolution.
    NoSymbolFound,
};

SymbolHookResult HookFileExplorerExtensionsSymbols(HMODULE module) {
    // All hooks are optional, since the set of functions differs between
    // Windows builds, but at least one of the discovery hooks must be found.
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const & __ptr64) __ptr64)",
            },
            &CommandBarManager_CommandBar_Original,
            CommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void) __ptr64)",
            },
            &CommandBarControl_OnApplyTemplate_Original,
            CommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void) __ptr64)",
            },
            &CommandBarControl_Wave1_OnApplyTemplate_Original,
            CommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &CommandBarControl_GotFocusHandler_Original,
            CommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const & __ptr64,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const & __ptr64) __ptr64)",
            },
            &CommandBarControl_Wave1_GotFocusHandler_Original,
            CommandBarControl_Wave1_GotFocusHandler_Hook,
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

// Returns false only if the module is loaded but hooking it failed.
bool HookFileExplorerExtensionsIfLoaded(bool applyHooks) {
    if (g_symbolsHooked) {
        return true;
    }

    HMODULE module = GetFileExplorerExtensionsModuleHandle();
    if (!module) {
        return true;
    }

    if (g_symbolsHooked.exchange(true)) {
        return true;
    }

    Wh_Log(L"Hooking FileExplorerExtensions.dll");

    switch (HookFileExplorerExtensionsSymbols(module)) {
        case SymbolHookResult::Success:
            break;

        case SymbolHookResult::ResolutionFailed:
            // Transient, so let a later attempt try again instead of leaving
            // the mod disabled for this process.
            g_symbolsHooked = false;
            return false;

        case SymbolHookResult::NoSymbolFound:
            // Keep the flag set: this build doesn't have what the mod needs,
            // and re-resolving the symbols on every module load would only
            // make Explorer slow for the rest of the session.
            return false;
    }

    if (applyHooks) {
        Wh_ApplyHookOperations();
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading || !lpLibFileName) {
        return module;
    }

    // Explorer loads modules constantly (shell extensions, thumbnail and
    // preview handlers), and this runs inline on whatever thread did the load,
    // so only look at the one module the mod cares about.
    PCWSTR fileName = lpLibFileName;
    for (PCWSTR p = lpLibFileName; *p; p++) {
        if (*p == L'\\' || *p == L'/') {
            fileName = p + 1;
        }
    }

    // LoadLibraryEx appends the default extension itself, so the caller may
    // have left it out.
    if (_wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0 ||
        _wcsicmp(fileName, L"FileExplorerExtensions") == 0) {
        HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/true);
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Initialization plumbing.

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
    if (dwThreadId == 0) {
        return false;
    }

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
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

std::vector<HWND> GetFileExplorerWnds() {
    std::vector<HWND> hWnds;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& hWnds = *(std::vector<HWND>*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[64];
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                hWnds.push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&hWnds);

    return hWnds;
}

// Levels: command bar items (0) -> menu items (1) -> submenu items (2).
constexpr int kMaxMenuDepth = 2;

ActionItem LoadActionItem(PCWSTR prefix, int depth, bool* isEmpty) {
    auto name = WindhawkUtils::StringSetting::make(L"%s.name", prefix);
    auto command = WindhawkUtils::StringSetting::make(L"%s.command", prefix);
    auto parameters =
        WindhawkUtils::StringSetting::make(L"%s.parameters", prefix);
    auto iconGlyph =
        WindhawkUtils::StringSetting::make(L"%s.iconGlyph", prefix);

    ActionItem item;
    item.name = name.get();
    item.command = command.get();
    item.parameters = parameters.get();
    item.icon = iconGlyph.get();
    item.hideIcon = Wh_GetIntSetting(L"%s.hideIcon", prefix) != 0;
    item.separatorAfter =
        Wh_GetIntSetting(L"%s.separatorAfter", prefix) != 0;

    // The deepest level can't hold a submenu, so it declares no type at all.
    if (depth < kMaxMenuDepth) {
        auto type = WindhawkUtils::StringSetting::make(L"%s.type", prefix);
        item.isMenu = wcscmp(type.get(), L"menu") == 0;

        for (int i = 0; i < 100; i++) {
            WCHAR subPrefix[256];
            swprintf(subPrefix, ARRAYSIZE(subPrefix), L"%s.subItems[%d]",
                     prefix, i);

            bool subEmpty = false;
            ActionItem subItem = LoadActionItem(subPrefix, depth + 1,
                                                &subEmpty);
            if (subEmpty) {
                break;
            }

            item.subItems.push_back(std::move(subItem));
        }
    }

    *isEmpty =
        item.name.empty() && item.command.empty() && item.subItems.empty();
    return item;
}

void LoadSettings() {
    {
        // The items, and with them the icons they ask for, may have changed.
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    std::lock_guard<std::mutex> lock(g_settings.mutex);

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

    g_settings.newPlus = NewPlusSettings{};
    g_settings.newPlus.enabled = Wh_GetIntSetting(L"newPlus.enabled") != 0;
    g_settings.newPlus.showLabel =
        Wh_GetIntSetting(L"newPlus.showLabel") != 0;
    g_settings.newPlus.buttonLabel =
        WindhawkUtils::StringSetting::make(L"newPlus.buttonLabel").get();
    g_settings.newPlus.buttonIcon =
        WindhawkUtils::StringSetting::make(L"newPlus.buttonIcon").get();
    g_settings.newPlus.templateFolder = TrimQuotesAndSpaces(
        WindhawkUtils::StringSetting::make(L"newPlus.templateFolder").get());
    g_settings.newPlus.showIcons =
        Wh_GetIntSetting(L"newPlus.showIcons") != 0;
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
        WindhawkUtils::StringSetting::make(
            L"contextMenuItem.buttonLabel").get();
    g_settings.contextMenuItem.buttonIcon = TrimQuotesAndSpaces(
        WindhawkUtils::StringSetting::make(
            L"contextMenuItem.buttonIcon").get());

    g_settings.items.clear();
    for (int i = 0; i < 100; i++) {
        WCHAR prefix[64];
        swprintf(prefix, ARRAYSIZE(prefix), L"items[%d]", i);

        bool isEmpty = false;
        ActionItem item = LoadActionItem(prefix, 0, &isEmpty);
        if (isEmpty) {
            break;
        }

        g_settings.items.push_back(std::move(item));
    }

    // No fallback item if the list is empty: using the mod only to hide
    // built-in buttons is a supported configuration.
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (GetFileExplorerExtensionsModuleHandle()) {
        if (!HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/false)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"FileExplorerExtensions.dll isn't loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW) {
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    // Last, so that no failure path above can leave the class registered:
    // Wh_ModUninit, which unregisters it, isn't called when Wh_ModInit fails.
    RegisterContextMenuOwnerClass();

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HookFileExplorerExtensionsIfLoaded(/*applyHooks=*/true);

    // Windows which were already open when the mod was loaded won't
    // necessarily rebuild their command bar, so look for it explicitly.
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { ScanCurrentThreadForCommandBars(); }, nullptr);
    }
}

// Our function hooks are gone by the time Wh_ModUninit runs, but the XAML
// delegates aren't - they stay live until they're unregistered below. Setting
// the flag one callback earlier shortens the window in which they still act.
void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");

    g_unloading = true;

    DismissOpenContextMenus();

    for (HWND hWnd : GetFileExplorerWnds()) {
        Wh_Log(L"Removing buttons for window %08X", (DWORD)(ULONG_PTR)hWnd);
        if (!RunFromWindowThread(
                hWnd, [](PVOID) { RemoveButtonsForCurrentThread(); },
                nullptr)) {
            Wh_Log(L"Couldn't reach the thread of window %08X",
                   (DWORD)(ULONG_PTR)hWnd);
        }
    }

    // The per-thread state (g_entries, g_managedElements, g_hoverTimers,
    // g_pendingUpdates) was released above, on each thread which owns it - it's
    // thread_local, so there's nothing here to clean up for other threads.
    {
        std::lock_guard<std::mutex> lock(g_iconCacheMutex);
        g_iconCache.clear();
    }

    // Any owner window the loop above didn't get to - its Explorer window may
    // already be gone, or RunFromWindowThread may have failed - has to be
    // destroyed from its own thread, so ask that thread through the owner
    // window itself. Leaving one behind would keep the class registered with a
    // window procedure which is about to be unmapped.
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

            Wh_Log(L"Destroying leftover context menu owner window of thread %u",
                   threadId);
            if (!RunFromWindowThread(
                    hWnd,
                    [](PVOID) { DestroyContextMenuOwnerWindowForCurrentThread(); },
                    nullptr)) {
                Wh_Log(L"Couldn't reach thread %u", threadId);
            }
        }
    }

    // Unconditionally: a class left registered outlives this image, and the
    // next load of the mod would find a stale window procedure behind it. If
    // some window still survived above, this fails - the next load detects that
    // as ERROR_CLASS_ALREADY_EXISTS and disables the context menu item rather
    // than crashing on it.
    if (g_contextMenuOwnerClassRegistered) {
        if (!UnregisterClassW(ContextMenuOwnerClassName().c_str(),
                              GetCurrentModuleHandle())) {
            std::lock_guard<std::mutex> lock(g_contextMenuOwnersMutex);
            Wh_Log(L"UnregisterClass failed: %u, %zu owner window(s) left "
                   L"behind",
                   GetLastError(), g_contextMenuOwners.size());
        }

        g_contextMenuOwnerClassRegistered = false;
    }

    // Last, since the launch threads run our code: the DLL can't be unmapped
    // while one of them is still working. The wait has no timeout, and a
    // launch thread can be inside a synchronous ShellExecuteExW - an elevation
    // consent prompt, which SEE_MASK_FLAG_NO_UI doesn't suppress, blocks it
    // until the user answers. Logged so that a stalled unload is diagnosable.
    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        if (!g_launchThreads.empty()) {
            Wh_Log(L"Waiting for %zu launch thread(s) - a command still being "
                   L"launched, such as one showing an elevation prompt, holds "
                   L"this up until it's done",
                   g_launchThreads.size());
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

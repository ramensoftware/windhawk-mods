// ==WindhawkMod==
// @id              dynamic-explorer-command-bar
// @name            Dynamic Explorer Command Bar
// @description     Add custom buttons and menus to the Windows 11 File Explorer command bar with dynamic file-type filtering, tokens, and secondary bar docking
// @version         1.1.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @homepage        https://github.com/ArvindSaini978/my-windhawk-mods
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lgdi32 -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid
// @license         MIT
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# Dynamic Explorer Command Bar

Customize, automate, and extend the **Windows 11 File Explorer command bar** with your own buttons, dropdown menus, smart argument tokens, and dynamic extension-based visibility rules.

![Dynamic Command Bar Demo](https://raw.githubusercontent.com/ArvindSaini978/my-windhawk-mods/refs/heads/master/assets/decb_preview.gif)

Designed for Windows 11 with the WinAppSDK (WinUI 3) File Explorer.

---

## Key Features

* **Custom Buttons & Dropdown Menus** — Add single-click toolbar buttons or nested dropdown menus (supporting up to 8 levels of hierarchy via parent referencing) running commands, scripts, or internal shell actions.
* **Flexible Ordering & Sorting** — Order buttons and menu items using priority numbers (`order`). Supports integers, arbitrary decimals (`1.2`, `1.25`) to easily insert items in-between without renumbering, and negative numbers (`-1`) to place items at the very beginning.
* **Context-Aware Dynamic Visibility** — Automatically show or hide individual buttons depending on the file extensions or item types highlighted in Explorer, including single (`single`) or multi-item (`multiple`) selection filters.
* **Dynamic UI Labels & Clamping** — Embed live tokens (`%base%`, `%name%`, `%c%`, `%ext%`, `%d_name%`) directly in button and dropdown labels that update in real time. Labels truncate cleanly with an ellipsis (`...`) past 24 characters on toolbar buttons and 48 characters in menus, with the complete label preserved in the hover tooltip.
* **Flexible Inactive State** — Choose between completely hiding (`Collapsed`) or graying out (`Disabled`) buttons when selection conditions are not met.
* **Advanced Command Tokens** — Pass active folder paths, file names, counts, extensions, parent directories, or selected file lists straight into your apps.
* **Batch Spawning (`%sel_each%`)** — Automatically launch an application instance for each highlighted item with built-in safety throttling.
* **Secondary Command Bar Placement** — Dock custom actions on the right side of the command bar (next to the Details/Preview pane toggles) for a clean dual-toolbar layout.
* **Theme-Aware & Executable Icons** — Support fluent Segoe glyphs (browse codes on [Segoe Fluent Icons](https://learn.microsoft.com/en-us/windows/apps/design/iconography/segoe-fluent-icons-font)), short application names runnable via Run or Terminal (`cmd.exe`, `notepad.exe`, `winrar.exe`), local file paths (`.exe` / `.dll` / `.ico`), Store apps (`shell:AppsFolder\...`), and dual Light/Dark icons via `LightPath | DarkPath` syntax.
* **Hide Built-in Explorer Buttons** — Declutter your toolbar by selectively hiding native Windows 11 commands (Cut, Copy, Paste, Share, Sort, View, 'See more'), contextual tools (Rotate, Extract all), the Details pane toggle, and Recycle Bin actions (Restore selected, Restore all, Empty Recycle Bin).
* **Rename Native Buttons** — Save horizontal space by customizing the text labels of default system actions (e.g., shorten "Empty Recycle Bin" to "Empty bin", "Details pane" to "Info", or "Rotate right" to "Right").
* **Open on Hover** — Open dropdown menus smoothly on cursor hover.

---

## Built-in Shell Actions

Instead of an executable path, you can set **Command** to one of these built-in actions.

> **Tip:** Both `internal:` and `int:` prefixes are supported. 
> All command names are completely **case-insensitive** (e.g., `internal:openWith`, `internal:openwith`, `internal:OpeNWitH`, and `INT:OPENWITH` all work identically).

* `internal:OpenWith` — Opens Windows native Open With dialog for the active item.
* `internal:FolderOptions` — Opens Classic Folder Options dialog.
* `internal:TogglePreview` — Toggles the File Explorer Preview pane.
* `internal:ToggleDetails` — Toggles the File Explorer Details pane.

---

## Command Parameters & Dynamic Tokens

The mod supports rich parameter expansions that dynamically resolve the state of the active File Explorer tab:

> **Tip:** Tokens work with or without a closing `%` (e.g., `%base%` or `%base`). When passing standard Windows shell environment variables (such as `%cd%` or `%date%`) in commands, use `%%` to escape literal percent signs (e.g., `%%cd%%`), or use the full bracketed token forms (e.g., `%count%` instead of `%c`) to avoid collisions.

| Token | Description | Example Output |
| :--- | :--- | :--- |
| `%base%` / `%basename%` | Selected item base names without extension | `Archive` |
| `%file_name%` | First item base name (for extract/output folders) | `Archive` |
| `%path%` / `%dir%` / `%d%` | Full physical path of the currently active folder | `C:\Projects\App` |
| `%sel%` / `%f%` | All selected items as space-separated quoted paths | `"C:\a.txt" "C:\b.png"` |
| `%1%` / `%1` | Full path of the first selected item | `"C:\Projects\App\Notes.txt"` |
| `%sel_each%` | Executes a distinct process instance per selected item (capped at 20) | `"C:\Item.txt"` |
| `%files%` | Space-separated quoted paths of selected regular files only | `"C:\doc.txt"` |
| `%folders%` | Space-separated quoted paths of selected directories only | `"C:\FolderA"` |
| `%name%` / `%n%` | Selected file names only (without directory path) | `"Notes.txt"` |
| `%ext%` | File extension of the first selected item | `.png` |
| `%count%` / `%c%` | Total number of selected items | `3` |
| `%d_smart%` | Selected directory if one is chosen, otherwise falls back to active folder | `C:\Projects\App\Sub` |
| `%d_name%` | Display name of the active folder or virtual location | `App` or `This PC` |
| `%parent%` / `%p%` | Parent folder path of the active tab | `C:\Projects` |
| `%%` | Literal percent sign escape | `%` |

Wrap placeholders in double quotes when necessary (e.g., `-d "%path%"` or `code.exe "%sel%"`).

> * **Virtual Shell Locations:** Locations such as *This PC*, *Recycle Bin*, and *Control Panel* do not possess a physical filesystem path. For safety, folder path argument tokens (`%path%`, `%dir%`, `%d%`, `%d_smart%`, `%p%`) resolve to an empty string in these views to prevent destructive scripts from unintentionally targeting user directories. The working directory (`%USERPROFILE%`) is still used in the background so applications launch cleanly. Use `%d_name%` to retrieve the native shell title (e.g. `This PC` or `Recycle Bin`).
> * **UI Labels:** To prevent toolbar buttons from overflowing, multi-item tokens like `%base%`, `%name%`, and `%1%` automatically evaluate to the **first highlighted item's name** rather than full multi-line absolute paths.

---

## Dynamic File Extension Filtering

The **File extension filter** (`matchExtensions`) field lets you display toolbar actions contextually:

> **How Evaluation Works:** 
> * Multiple comma-separated tokens evaluate using **`OR`** logic (matches if *any* rule succeeds).
> * Use **`+`** within a rule for **`AND`** logic (e.g. `files + single`).
> * Negative tokens prefixed with **`-`** or **`~`** (e.g. `-zip`, `-exe`, `-folder`) act as an **absolute veto** — if any highlighted item matches an exclusion rule, the button is immediately hidden.
> * **`hide` Override:** Adding `hide` (or `hidden`) anywhere in the filter permanently disables and hides the button under all conditions. This is ideal if you want to temporarily shelve a button or menu without having to delete its configuration from settings.

### Available Filter Keywords

* **`hide` / `hidden`** — Absolute override. Keeps the button permanently hidden until removed.
* **Leave blank or `*`** — Always show the button regardless of selection.
* **`none` / `empty` / `nothing` / `unselected`** — Show **only** when nothing is selected (0 items highlighted). Hides as soon as any item is clicked.
* **`selected` / `any`** — Show whenever at least one item of any kind is highlighted.
* **`files` / `file`** — Show if regular files are selected (regardless of count).
* **`folders` / `folder` / `dir`** — Show if folders/directories are selected (regardless of count).
* **`mixed` / `mixed_selection`** — Show exclusively when both regular files and folders are highlighted at the same time.
* **`single`** — Show only when exactly 1 item (file or folder) is selected.
* **`multiple` / `multi`** — Show only when 2 or more items are selected.
* **`single_file`** *(or `single_files`)* — Show exclusively when exactly 1 regular file is selected.
* **`multi_file`** *(or `multi_files`)* — Show exclusively when 2 or more regular files are selected.
* **`single_folder`** *(or `single_folders`)* — Show exclusively when exactly 1 folder is selected.
* **`multi_folder`** *(or `multi_folders`)* — Show exclusively when 2 or more folders are selected.
* **`only <ext>`** *(e.g. `only txt`, `only:cpp`, `only txt, only cpp`, `only(txt, cpp)`)* — Strict whitelist. Shows exclusively when **every single highlighted item** matches one of the specified extensions (vetoes mixed selections and folders).
* **`txt, js, cpp`** — Standard extension filter (shows if *any* selected file matches).

### Combining Rules (`OR`, `AND`, and Exclusions)

* **`folders, multi_file`** — Shows when any folder is selected **OR** 2+ files are selected (ideal for batch actions or directory tools).
* **`files, mixed`** — Shows when regular files are selected **OR** when a mixed bundle of files and folders is selected (hiding only when purely folders or nothing is chosen).
* **`none, folders`** — Shows in empty directory backgrounds **OR** when folders are selected.
* **`single_file, single_folder`** — Shows for any single item, but automatically hides on multi-selection.
* **`files + single`** — Same as `single_file` (uses `+` for strict intersection).
* **`folders, multi_file, -zip`** — Matches folders or multiple files, but immediately hides if any item in the selection is a `.zip` file.
* **`files, hide`** — Kept permanently hidden because `hide` takes absolute precedence over all other tokens.
* **`only txt, only md`** — Shows when only `.txt` and/or `.md` files are highlighted (hides immediately if a `.png` or folder is included).

---

## Button Ordering & Positioning (`order`)

Instead of cutting and pasting configuration code to reorder items:
* Set **Display Order / Priority** (`order`) to any number (e.g. `1`, `2`, `3`). Lower numbers appear first.
* Use decimals (e.g. `1.5`) to insert an item between `1` and `2` without having to renumber subsequent buttons.
* Use negative numbers (e.g. `-1`) to position an item before unnumbered buttons.
* Inside dropdown menus, `order` sorts items strictly within that specific menu container.

---

## Creating Dropdown Menus & Hierarchies

To place buttons inside a dropdown menu:
1. Create a menu container item with **Item Type** set to `menu`, and assign it a unique **Item ID** (e.g. `tools_menu`).
2. For any button or submenu you want nested inside it, set its **Parent Menu ID** to that menu's ID (`tools_menu`).
3. Leave **Parent Menu ID** blank for items that should appear directly on the top toolbar.

---

## Decluttering & Customizing Native Explorer Buttons

You can manage native Windows 11 toolbar buttons directly from the Windhawk Settings tab:

### 1. Hiding Built-in Buttons (`hideDefaultButtons`)
Toggle any built-in action to hide it completely from File Explorer:
* **Core Actions:** `New`, `Cut`, `Copy`, `Paste`, `Rename`, `Share`, `Delete`, `Sort`, `View`, and `See more` (`...` menu).
* **Separators:** Selectively remove the vertical separator line after `New`, after `Delete`, or after `View`.
* **Contextual Tools:** `Set as background`, `Rotate left`, `Rotate right`, and `Extract all`.
* **Panes:** `Details pane toggle`.
* **Recycle Bin:** `Restore selected items`, `Restore all items`, and `Empty Recycle Bin`.

### 2. Renaming Native Labels (`systemButtonLabels`)
Default Windows command bar buttons with text labels can be overridden to save horizontal toolbar space:
* Set a custom short string (e.g., `"Left"` for Rotate Left, `"Empty bin"` for Empty Recycle Bin, or `"Info"` for Details pane).
* Leave the setting blank (`""`) to keep the default localized Windows text.

---

## Comparison with Existing Mods

* **vs. `explorer-command-bar` (DanRotaru)**: 
  The original mod provides static command buttons and hides built-in buttons. **Dynamic Explorer Command Bar** extends this with real-time selection-based extension filtering, live UI label tokens, custom native button labels, per-item execution (`%sel_each%`), secondary toolbar placement, dual Light/Dark icons, and an expanded token engine (`%files%`, `%folders%`, `%n%`, `%ext%`, `%c%`, `%d_name%`, `%p%`).
* **vs. `explorer-custom-shortcuts` (ArvindSaini978)**:
  While *Explorer Custom Shortcuts* provides background keyboard hotkeys, this mod brings dynamic parameter evaluation directly to the visual WinUI command bar.

---

## Author & Attribution

* **Created and maintained by ArvindSaini978 ([GitHub](https://github.com/ArvindSaini978)).**
* Extended from *Explorer Command Bar* by DanRotaru (MIT), with token expansion adapted from *Explorer Custom Shortcuts* (ArvindSaini978) and thread dispatch from *Explorer Folder Bookmarks Bar* (Maxim Fomin).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- items:
  - - type: button
      $name: Item Type
      $options:
      - button: Single button
      - menu: Dropdown menu container
    - id: "folder_options"
      $name: Item ID
      $description: Unique identifier. Sub-items reference this in their Parent ID field.
    - parent: ""
      $name: Parent Menu ID
      $description: Leave blank for the top toolbar. Set to a Menu ID to place this item inside that menu.
    - order: ""
      $name: Display Order / Priority
      $description: >-
        Optional sorting position. Lower numbers appear first (e.g. 1, 2, 3).
        Decimals (e.g. 1.2, 1.25) can insert items between existing ones, and negative numbers (e.g. -1) place items before unnumbered buttons.
        Leave empty to keep the default list order.
    - command: internal:FolderOptions
      $name: Command
      $description: >-
        The executable to run, or an internal command using 'internal:' or 'int:' prefix (case-insensitive, e.g. internal:openWith, internal:openwith, internal:OpeNWitH, and INT:OPENWITH).
        See Details Tab for full list of internal commands. 
    - parameters: ""
      $name: Parameters
      $description: >-
        Command line arguments. Common tokens: %path% / %dir% / %d% (folder), %sel% (selected items), %sel_each% (batch launch), %files%, %folders%, %name%, %base%, %d_name%, %parent%, etc. Use %% for literal %. 
        All work with or without trailing %. See Details Tab for full list. 
    - matchExtensions: ""
      $name: "File extension filter"
      $description: >-
        Comma-separated rules evaluate using OR logic. Use '+' for AND (e.g. files + single). Prefix with '-' to exclude (e.g. -exe, -zip).
        Supports extensions (txt, png), categories (files, folders), counts (single, multi, single_file), 'none' (unselected), or 'hide'. 
        Leave blank or '*' to always show.
        See Details Tab for full list
    - iconGlyph: E713
      $name: Icon glyph or icon path
      $description: >-
        Fluent icon glyph (e.g. E713), file path (.ico/.dll/.exe), app executable name (cmd.exe, notepad.exe), or empty for executable icon. 
        Separate with | for theme-aware icons (Light | Dark, or | Dark).
    - hideIcon: false
      $name: Hide icon
      $description: Don't show an icon for this item.
    - showLabel: false
      $name: Show label
      $description: Show text label next to the icon.
    - labelText: ""
      $name: Custom Label Text
      $description: >-
        Text displayed next to the icon. Supports live tokens: %base% / %basename% / %file_name%, %name% / %n% / %1%, %ext%, %count% / %c%, %d_name%, %dir% / %path% / %d%. Use %% for literal %. All work with or without trailing %. If empty, Item ID is used.
    - separatorBefore: false
      $name: Vertical separator before
      $description: Show a vertical separator line before this button.
    - separatorAfter: false
      $name: Vertical separator after
      $description: Show a vertical separator line after this button.
  - - type: button
    - id: "open_with"
    - parent: ""
    - order: ""
    - command: internal:OpenWith
    - parameters: ""
    - matchExtensions: "single_file"
    - iconGlyph: "E7AC"
    - hideIcon: false
    - showLabel: true
    - labelText: "Open With"
    - separatorBefore: false
    - separatorAfter: false
  - - type: button
    - id: "open_notepad"
    - parent: ""
    - order: ""
    - command: notepad.exe
    - parameters: "%sel_each%"
    - matchExtensions: ""
    - iconGlyph: "notepad.exe"
    - hideIcon: false
    - showLabel: false
    - labelText: "Notepad"
    - separatorBefore: true
    - separatorAfter: false
  - - type: button
    - id: "open_terminal"
    - parent: ""
    - order: ""
    - command: wt.exe
    - parameters: -d "%path%"
    - matchExtensions: ""
    - iconGlyph: "wt.exe"
    - hideIcon: false
    - showLabel: false
    - labelText: "Open Terminal Here"
    - separatorBefore: false
    - separatorAfter: false
  - - type: menu
    - id: "custom_menu"
    - parent: ""
    - order: ""
    - command: ""
    - parameters: ""
    - matchExtensions: ""
    - iconGlyph: "E70D"
    - hideIcon: false
    - showLabel: true
    - labelText: "Commands"
    - separatorBefore: false
    - separatorAfter: false
  - - type: button
    - id: "cmd_here"
    - parent: "custom_menu"
    - order: ""
    - command: cmd.exe
    - parameters: /k cd /d "%path%"
    - matchExtensions: ""
    - iconGlyph: "cmd.exe"
    - hideIcon: false
    - showLabel: false
    - labelText: "Command Prompt"
    - separatorBefore: false
    - separatorAfter: false
  - - type: button
    - id: "open_calc"
    - parent: "custom_menu"
    - order: ""
    - command: calc.exe
    - parameters: ""
    - matchExtensions: ""
    - iconGlyph: "E8EF"
    - hideIcon: false
    - showLabel: false
    - labelText: "Calculator"
    - separatorBefore: false
    - separatorAfter: false
  $name: Toolbar items
  $description: Custom buttons and dropdown menus added to the command bar.
- placeOnSecondaryBar: true
  $name: Place buttons on the right side
  $description: >-
    Show custom buttons on the right side of File Explorer (next to Details
    and Preview pane toggles) instead of the main left toolbar.
- disabledInsteadOfHidden: false
  $name: Disable buttons instead of hiding them
  $description: Gray out inactive buttons instead of removing them from the toolbar.
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
  - restoreSelected: false
    $name: Recycle Bin - Restore selected items
    $description: Shown inside the Recycle Bin when items are selected.
  - restoreAll: false
    $name: Recycle Bin - Restore all items
    $description: Shown inside the Recycle Bin when no items are selected.
  - emptyRecycleBin: false
    $name: Recycle Bin - Empty Recycle Bin
    $description: Shown inside the Recycle Bin.
  - details: false
    $name: Details pane toggle
  $name: Hide default toolbar buttons
  $description: Hide the built-in buttons of the File Explorer command bar.
- systemButtonLabels:
  - new: ""
    $name: New button label
    $description: Shorten the New button label. Leave blank to keep default.
  - setAsBackground: ""
    $name: Set as background label
    $description: Shorten the Set as background label. Leave blank to keep default.
  - extractAll: ""
    $name: Extract all label
    $description: Shorten the Extract all label. Leave blank to keep default.
  - rotateLeft: ""
    $name: Rotate left label override
    $description: Shorten the Rotate Left label (e.g., 'Left'). Leave blank to keep default.
  - rotateRight: ""
    $name: Rotate right label override
    $description: Shorten the Rotate Right label (e.g., 'Right'). Leave blank to keep default.
  - restoreSelected: ""
    $name: Recycle Bin - Restore selected items
    $description: Shorten the Recycle Bin restore selected items label (e.g., 'Restore selected'). Leave blank to keep default.
  - restoreAll: ""
    $name: Recycle Bin - Restore all items
    $description: Shorten the Recycle Bin restore all items label (e.g., 'Restore all'). Leave blank to keep default.
  - emptyRecycleBin: ""
    $name: Recycle Bin - Empty Recycle Bin
    $description: Shorten the Empty Recycle Bin button label (e.g., 'Empty bin'). Leave blank to keep default.
  - details: ""
    $name: Details pane button label
    $description: Shorten the Details pane button label (e.g., 'Details' or 'Info'). Leave blank to keep default.
  $name: System button label overrides
  $description: Shorten default Windows toolbar labels that already display text to save horizontal space.
- menuHover:
  - openOnHover: false
    $name: Open menus on hover
    $description: Open dropdown flyout menus automatically when hovering over toolbar buttons.
  - delay: 400
    $name: Hover delay (milliseconds)
    $description: Time in milliseconds to hover before opening the menu flyout.
  $name: Menu hover behavior
  $description: Configure automatic flyout expansion when hovering over dropdown buttons.
*/
// ==/WindhawkModSettings==

// clang-format on

#include <windhawk_utils.h>
#include <windows.h>

#include <Unknwn.h>
#include <exdisp.h>
#include <servprov.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>

// Undefine Windows SDK macro colliding with WinRT XAML animation methods
#undef GetCurrentTime

#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

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
#include <mutex>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Define AppExecLink reparse point tag if missing from older SDK headers
#ifndef IO_REPARSE_TAG_APPEXECLINK
#define IO_REPARSE_TAG_APPEXECLINK (0x8000001BL)
#endif

// ============================================================================
// Lifecycle Synchronization & Concurrency Guards
// ============================================================================

// Atomic flag signaling that the mod is detaching and all callbacks must abort
static std::atomic<bool> g_unloading{false};
// Atomic counter tracking queued DispatcherQueue tasks not yet executed
static std::atomic<int> g_pendingDispatches{0};
// Atomic counter tracking active UI thread callbacks currently executing mod
// code
static std::atomic<int> g_uiCallsInProgress{0};

// RAII guard incrementing pending dispatches until the queued lambda is
// destroyed
struct PendingDispatchScope {
    PendingDispatchScope() { g_pendingDispatches++; }
    ~PendingDispatchScope() { g_pendingDispatches--; }
    PendingDispatchScope(const PendingDispatchScope&) = delete;
    PendingDispatchScope& operator=(const PendingDispatchScope&) = delete;
};

// RAII guard tracking in-flight UI callbacks so Wh_ModUninit waits safely
// before DLL unload
struct UiCallScope {
    UiCallScope() { g_uiCallsInProgress++; }
    ~UiCallScope() { g_uiCallsInProgress--; }
    UiCallScope(const UiCallScope&) = delete;
    UiCallScope& operator=(const UiCallScope&) = delete;
};

// RAII wrapper managing COM STA initialization and cleanup on background worker
// threads
class ComApartmentScope {
   public:
    explicit ComApartmentScope(DWORD coInit)
        : m_hr(CoInitializeEx(nullptr, coInit)) {}
    ~ComApartmentScope() {
        if (SUCCEEDED(m_hr)) {
            CoUninitialize();
        }
    }
    ComApartmentScope(const ComApartmentScope&) = delete;
    ComApartmentScope& operator=(const ComApartmentScope&) = delete;

   private:
    HRESULT m_hr;
};

// ============================================================================
// Constants & Configuration Data Models
// ============================================================================

// ShellWindows COM class identifier used to enumerate active File Explorer tabs
static constexpr CLSID kCLSID_ShellWindows = {
    0x9ba05972,
    0xf6a8,
    0x11cf,
    {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};

// Top-level ShellBrowser service GUID used to query active IShellBrowser
// instances
static constexpr GUID kSID_STopLevelBrowser = {
    0x4c96be40,
    0x915c,
    0x11cf,
    {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

// Metadata entry mapping built-in Explorer buttons to SVG icons and Automation
// IDs
struct DefaultButtonDef {
    PCWSTR settingKey;
    PCWSTR svgFileName;
    PCWSTR automationId;
};

// Lookup table for identifying built-in Windows 11 File Explorer command bar
// buttons
static constexpr DefaultButtonDef kDefaultButtons[] = {
    {L"new", L"windows.newitem.svg", nullptr},                          // 0
    {L"cut", L"windows.cut.svg", nullptr},                              // 1
    {L"copy", L"windows.copy.svg", nullptr},                            // 2
    {L"paste", L"windows.paste.svg", nullptr},                          // 3
    {L"rename", L"windows.rename.svg", nullptr},                        // 4
    {L"share", L"windows.modernshare.svg", nullptr},                    // 5
    {L"delete", L"windows.ribbondelete.svg", nullptr},                  // 6
    {L"sort", L"sortby.svg", L"SortAndGroupButton"},                    // 7
    {L"view", L"view.svg", nullptr},                                    // 8
    {L"setAsBackground", L"windows.setdesktopwallpaper.svg", nullptr},  // 9
    {L"rotateLeft", L"windows.rotate270.svg", nullptr},                 // 10
    {L"rotateRight", L"windows.rotate90.svg", nullptr},                 // 11
    {L"extractAll", L"windows.compressedfile.extract.svg", nullptr},    // 12
    {L"details", L"windows.details.svg", L"DetailsPaneToggle"},         // 13
    {L"restoreSelected", L"windows.undelete.svg", L"RestoreSelected"},  // 14
    {L"restoreAll", L"windows.restoreall.svg", L"RestoreAll"},          // 15
    {L"emptyRecycleBin", L"windows.emptyrecyclebin.svg",
     L"EmptyRecycleBin"},  // 16
};

// Constants indexing specific built-in toolbar buttons and operational safety
// limits
static constexpr int kDefaultButtonCount = ARRAYSIZE(kDefaultButtons);
static constexpr int kNewButtonIndex = 0;
static constexpr int kDeleteButtonIndex = 6;
static constexpr int kViewButtonIndex = 8;
static constexpr int kDetailsButtonIndex = 13;
static constexpr size_t kMaxToolbarLabelLength = 24;
static constexpr size_t kMaxMenuItemLabelLength = 48;
static constexpr size_t kMaxBatchSpawn = 20;
static constexpr size_t kMaxMenuHierarchyDepth = 8;

// Unique XAML element name prefix tagging all controls injected by this mod
static const std::wstring kButtonNamePrefix =
    std::wstring(L"WH_") + WH_MOD_ID + L"_Btn";

// Configuration model representing a custom toolbar button or hierarchical menu
// item
struct ActionItem {
    std::wstring id;
    std::wstring parent;
    std::wstring labelText;
    std::wstring command;
    std::wstring parameters;
    std::wstring matchExtensions;
    std::wstring icon;
    double order = 0.0;
    bool hideIcon = false;
    bool showLabel = false;
    bool isMenu = false;
    bool separatorBefore = false;
    bool separatorAfter = false;
    std::vector<ActionItem> subItems;

    // Return custom labelText when configured or fall back to the item ID
    std::wstring GetDisplayLabel() const {
        return labelText.empty() ? id : labelText;
    }
};

// Thread-safe global configuration store holding all parsed Windhawk user
// settings
struct ModSettings {
    std::mutex mutex;
    bool placeOnSecondaryBar = true;
    bool disabledInsteadOfHidden = false;
    bool openMenuOnHover = false;
    int menuHoverDelay = 400;
    std::vector<ActionItem> items;
    bool hideDefaultButtons[kDefaultButtonCount] = {};
    bool hideSeparatorAfterButton[kDefaultButtonCount] = {};
    bool hideMoreButton = false;
    std::wstring customLabels[kDefaultButtonCount];
};

static ModSettings g_settings;

// ============================================================================
// Per-Thread CommandBar State & Event Revoker Tracking
// ============================================================================

// Per-CommandBar state tracking timers, event tokens, cached shell views, and
// label positions
struct CommandBarEntry {
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::CommandBar>
        commandBar;
    winrt::Microsoft::UI::Xaml::DispatcherTimer selectionTimer{nullptr};
    winrt::event_token selectionTickToken{};
    winrt::event_token vectorChangedToken{};
    bool hasVectorChangedToken = false;
    HWND cachedTabWnd = nullptr;
    winrt::com_ptr<IShellBrowser> cachedBrowser{nullptr};
    winrt::com_ptr<IShellView> cachedShellView{nullptr};
    int lastSelectionCount = -1;
    int lastFocusedItem = -1;
    bool hasModifiedDefaultLabelPosition = false;
    winrt::Microsoft::UI::Xaml::Controls::CommandBarDefaultLabelPosition
        originalDefaultLabelPosition{
            winrt::Microsoft::UI::Xaml::Controls::
                CommandBarDefaultLabelPosition::Bottom};
};

// Thread-local storage for active CommandBars, scan status, coalesced updates,
// and re-entrancy guards
static thread_local std::vector<CommandBarEntry> g_entries;
static thread_local bool g_threadScanned = false;
static thread_local std::unordered_map<void*, bool> g_pendingUpdates;
static thread_local int g_modifyingCommandsDepth = 0;

// Type-erased wrapper holding a WinRT auto-revoker tied to a weak XAML object
// source
struct TrackedRevoker {
    winrt::weak_ref<winrt::Windows::Foundation::IInspectable> source;
    std::function<void()> revoke;
};

static constexpr size_t kRevokersPruneMin = 64;
static thread_local std::vector<TrackedRevoker> g_revokers;
static thread_local size_t g_revokersPruneAt = kRevokersPruneMin;

// Register a WinRT event auto-revoker for automatic pruning and deterministic
// thread teardown
template <typename T, typename Revoker>
static void TrackRevoker(T const& source, Revoker&& revoker) {
    if (g_revokers.size() >= g_revokersPruneAt) {
        std::erase_if(g_revokers, [](TrackedRevoker const& tracked) {
            return !tracked.source.get();
        });
        g_revokersPruneAt = std::max(kRevokersPruneMin, g_revokers.size() * 2);
    }

    auto held =
        std::make_shared<std::decay_t<Revoker>>(std::forward<Revoker>(revoker));
    g_revokers.push_back(
        {winrt::weak_ref<winrt::Windows::Foundation::IInspectable>{source},
         [held]() { held->revoke(); }});
}

// Immediately revoke all registered WinRT event handlers on the current UI
// thread
static void RevokeHandlersForCurrentThread() {
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

// Stop the 250ms selection polling timer and release cached COM shell pointers
// for an entry
static void StopSelectionTimer(CommandBarEntry& entry) {
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

// Unsubscribe the PrimaryCommands VectorChanged token safely if still
// registered
static void DetachVectorChangedObserver(CommandBarEntry& entry) {
    if (!entry.hasVectorChangedToken) {
        return;
    }
    entry.hasVectorChangedToken = false;
    winrt::event_token token = entry.vectorChangedToken;
    entry.vectorChangedToken = {};
    if (auto cb = entry.commandBar.get()) {
        try {
            cb.PrimaryCommands().VectorChanged(token);
        } catch (...) {
        }
    }
}

// ============================================================================
// String Manipulation & Command Path Resolution Utilities
// ============================================================================

// Convert a wide string to lowercase in place and return it
static std::wstring ToLower(std::wstring str) {
    for (auto& c : str) {
        c = static_cast<WCHAR>(towlower(c));
    }
    return str;
}

// Expand `%ENV%` environment variable references using the process environment
// block
static std::wstring ExpandEnvVars(std::wstring const& str) {
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

// Strip leading/trailing whitespace and outermost matching double quotes from a
// string
static std::wstring TrimWhitespaceAndQuotes(std::wstring str) {
    size_t first = str.find_first_not_of(L" \t");
    if (first == std::wstring::npos) {
        return L"";
    }
    size_t last = str.find_last_not_of(L" \t");
    str = str.substr(first, last - first + 1);
    if (str.size() >= 2 && str.front() == L'"' && str.back() == L'"') {
        str = str.substr(1, str.size() - 2);
        first = str.find_first_not_of(L" \t");
        if (first == std::wstring::npos) {
            return L"";
        }
        last = str.find_last_not_of(L" \t");
        str = str.substr(first, last - first + 1);
    }
    return str;
}

// Clamp a UI label string to maxLen characters and append an ellipsis if
// truncated
static std::wstring TruncateWithEllipsis(
    std::wstring text,
    size_t maxLen = kMaxToolbarLabelLength) {
    if (text.length() > maxLen && maxLen > 3) {
        return text.substr(0, maxLen - 3) + L"...";
    }
    return text;
}

// Extract the file or folder base name without directory components or file
// extension
static std::wstring ExtractBaseName(std::wstring_view itemPath) {
    if (itemPath.empty()) {
        return L"";
    }
    std::wstring pathCopy(itemPath);
    PCWSTR fileName = PathFindFileNameW(pathCopy.c_str());
    if (!fileName || !*fileName) {
        return L"";
    }
    WCHAR buffer[MAX_PATH];
    wcsncpy_s(buffer, fileName, _TRUNCATE);
    PathRemoveExtensionW(buffer);
    return buffer;
}

// Parse a non-negative integer strictly from a string without accepting
// non-digits
static bool TryParseNonNegativeInt(std::wstring_view str, int* outVal) {
    if (str.empty() || !outVal) {
        return false;
    }
    int value = 0;
    for (WCHAR c : str) {
        if (c < L'0' || c > L'9') {
            return false;
        }
        int digit = c - L'0';
        if (value > (INT_MAX - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }
    *outVal = value;
    return true;
}

// Wrap a command-line argument in double quotes while escaping trailing
// backslashes
static std::wstring QuoteArg(std::wstring value) {
    if (!value.empty() && value.back() == L'\\') {
        value += L'\\';
    }
    return L"\"" + value + L"\"";
}

// Join multiple paths into a space-separated list of safely quoted arguments
static std::wstring FormatSelectedItemsQuoted(
    std::vector<std::wstring> const& paths) {
    std::wstring formatted;
    for (auto const& p : paths) {
        if (p.empty()) {
            continue;
        }
        std::wstring safe = p;
        if (safe.back() == L'\\') {
            safe += L'\\';
        }
        formatted += L"\"" + safe + L"\" ";
    }
    if (!formatted.empty()) {
        formatted.pop_back();
    }
    return formatted;
}

// Replace all occurrences of a token in parameters while preventing
// trailing-backslash quote escapes
static bool ReplacePlaceholder(std::wstring& parameters,
                               std::wstring_view placeholder,
                               std::wstring const& value) {
    size_t pos = parameters.find(placeholder);
    if (pos == std::wstring::npos) {
        return true;
    }

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

// Resolve short executable names (e.g. wt.exe, winrar.exe) via SearchPathW or
// Registry App Paths
static std::wstring ResolveCommandPath(std::wstring const& command) {
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
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + expanded;
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
            if (appPath.size() >= 2 && appPath.front() == L'"' &&
                appPath.back() == L'"') {
                appPath = appPath.substr(1, appPath.size() - 2);
            }
            return appPath;
        }
    }

    return expanded;
}

// Case-insensitively match `internal:<action>` or `int:<action>` without heap
// allocation
static bool IsCommandMatch(std::wstring_view cmd,
                           std::wstring_view actionName) {
    auto matchSuffix = [&](std::wstring_view prefix) -> bool {
        if (cmd.size() != prefix.size() + actionName.size()) {
            return false;
        }
        if (_wcsnicmp(cmd.data(), prefix.data(), prefix.size()) != 0) {
            return false;
        }
        return _wcsnicmp(cmd.data() + prefix.size(), actionName.data(),
                         actionName.size()) == 0;
    };
    return matchSuffix(L"internal:") || matchSuffix(L"int:");
}

// ============================================================================
// Zero-Stall Shell Inspection & Explorer Window Discovery
// ============================================================================

// Snapshot of an Explorer tab's active folder, selection paths, and
// virtual-location status
struct ExplorerContext {
    std::wstring folderPath;
    std::wstring selectedPath;
    std::wstring folderDisplayName;
    std::wstring firstItemDisplayName;
    std::vector<std::wstring> allSelectedPaths;
    std::vector<std::wstring> selectedFilesOnly;
    std::vector<std::wstring> selectedFoldersOnly;
    winrt::com_ptr<IShellView> shellView;
    size_t selectionCount = 0;
    bool isFilesystemFolder = false;
    bool hasClassifiedSelection = false;
};

// Lightweight in-memory summary of selected item counts and extensions for
// filter evaluation
struct SelectionSummary {
    size_t count = 0;
    size_t fileCount = 0;
    size_t folderCount = 0;
    std::vector<std::wstring> extensions;
};

// Verify whether a top-level HWND belongs to a File Explorer CabinetWClass
// window
static bool IsExplorerFrame(HWND window) {
    WCHAR className[64]{};
    return window && GetClassNameW(window, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"CabinetWClass") == 0;
}

// Resolve the hosting CabinetWClass HWND for a WinUI 3 XAML element via
// ContentIslandEnvironment
static HWND GetExplorerWindowForElement(
    winrt::Microsoft::UI::Xaml::FrameworkElement const& element) {
    try {
        if (auto root = element.XamlRoot()) {
            if (auto environment = root.ContentIslandEnvironment()) {
                HWND window =
                    GetAncestor(reinterpret_cast<HWND>(static_cast<uintptr_t>(
                                    environment.AppWindowId().Value)),
                                GA_ROOT);
                if (IsExplorerFrame(window)) {
                    return window;
                }
            }
        }
    } catch (...) {
    }

    HWND window = GetActiveWindow();
    window = window ? GetAncestor(window, GA_ROOT) : nullptr;
    return IsExplorerFrame(window) ? window : nullptr;
}

// Locate the currently visible ShellTabWindowClass HWND in a multi-tab Explorer
// window
static HWND FindActiveTabWindow(HWND hExplorerWnd,
                                HWND hCapturedFocus = nullptr) {
    if (!hExplorerWnd) {
        return nullptr;
    }

    DWORD threadId = GetWindowThreadProcessId(hExplorerWnd, nullptr);
    GUITHREADINFO gti = {sizeof(gti)};
    HWND hFocus = hCapturedFocus
                      ? hCapturedFocus
                      : ((GetGUIThreadInfo(threadId, &gti) && gti.hwndFocus)
                             ? gti.hwndFocus
                             : nullptr);

    for (HWND h = hFocus; h && h != hExplorerWnd; h = GetParent(h)) {
        WCHAR cls[64];
        if (GetClassNameW(h, cls, ARRAYSIZE(cls)) &&
            wcscmp(cls, L"ShellTabWindowClass") == 0 && IsWindowVisible(h)) {
            return h;
        }
    }

    HWND hTab = nullptr;
    while ((hTab = FindWindowExW(hExplorerWnd, hTab, L"ShellTabWindowClass",
                                 nullptr)) != nullptr) {
        if (IsWindowVisible(hTab)) {
            return hTab;
        }
    }
    return nullptr;
}

// Query ShellWindows to find the IShellBrowser matching the active visible
// Explorer tab
static winrt::com_ptr<IShellBrowser> GetActiveShellBrowser(
    HWND hExplorerWnd,
    HWND hCapturedFocus = nullptr) {
    winrt::com_ptr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(shellWindows.put()))) ||
        !shellWindows) {
        return nullptr;
    }

    long count = 0;
    if (FAILED(shellWindows->get_Count(&count)) || count <= 0) {
        return nullptr;
    }

    HWND hTargetRoot =
        hExplorerWnd ? GetAncestor(hExplorerWnd, GA_ROOT) : nullptr;
    HWND hActiveTabWnd = FindActiveTabWindow(hTargetRoot, hCapturedFocus);

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

        HWND hBrowserRoot =
            GetAncestor(reinterpret_cast<HWND>(hWndRaw), GA_ROOT);
        if (hTargetRoot && hBrowserRoot != hTargetRoot) {
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

        HWND hTabWnd = nullptr;
        if (SUCCEEDED(shellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
            if (hActiveTabWnd && hTabWnd != hActiveTabWnd) {
                continue;
            }
        }

        return shellBrowser;
    }

    return nullptr;
}

// Retrieve the active IShellView for an Explorer window on the UI thread
static winrt::com_ptr<IShellView> GetActiveShellView(HWND hExplorerWnd) {
    if (auto browser = GetActiveShellBrowser(GetAncestor(hExplorerWnd, GA_ROOT),
                                             GetFocus())) {
        winrt::com_ptr<IShellView> sv;
        if (SUCCEEDED(browser->QueryActiveShellView(sv.put())) && sv) {
            return sv;
        }
    }
    return nullptr;
}

// Extract active folder path and native title purely from PIDL attributes
// without disk I/O
static void PopulateFolderInfoFast(IShellView* pView, ExplorerContext& result) {
    if (!pView) {
        return;
    }
    winrt::com_ptr<IShellView> view;
    view.copy_from(pView);
    auto folderView = view.try_as<IFolderView>();
    if (!folderView) {
        return;
    }

    winrt::com_ptr<IPersistFolder2> persistFolder;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))) ||
        !persistFolder) {
        return;
    }

    LPITEMIDLIST pidl = nullptr;
    if (FAILED(persistFolder->GetCurFolder(&pidl)) || !pidl) {
        return;
    }

    winrt::com_ptr<IShellItem> folderItem;
    if (SUCCEEDED(
            SHCreateItemFromIDList(pidl, IID_PPV_ARGS(folderItem.put()))) &&
        folderItem) {
        PWSTR dispName = nullptr;
        if (SUCCEEDED(
                folderItem->GetDisplayName(SIGDN_NORMALDISPLAY, &dispName)) &&
            dispName) {
            result.folderDisplayName = dispName;
            CoTaskMemFree(dispName);
        }

        SFGAOF attrs = 0;
        if (SUCCEEDED(folderItem->GetAttributes(SFGAO_FILESYSTEM, &attrs)) &&
            (attrs & SFGAO_FILESYSTEM)) {
            PWSTR fsPath = nullptr;
            if (SUCCEEDED(
                    folderItem->GetDisplayName(SIGDN_FILESYSPATH, &fsPath)) &&
                fsPath) {
                if (!(fsPath[0] == L':' && fsPath[1] == L':')) {
                    result.folderPath = fsPath;
                    result.isFilesystemFolder = true;
                }
                CoTaskMemFree(fsPath);
            } else {
                PWSTR parsePath = nullptr;
                if (SUCCEEDED(folderItem->GetDisplayName(
                        SIGDN_DESKTOPABSOLUTEPARSING, &parsePath)) &&
                    parsePath) {
                    if (!(parsePath[0] == L':' && parsePath[1] == L':')) {
                        result.folderPath = parsePath;
                        result.isFilesystemFolder = true;
                    }
                    CoTaskMemFree(parsePath);
                }
            }
        }
    }

    CoTaskMemFree(pidl);
}

// Build a zero-disk-I/O context snapshot safe for the 250ms UI polling timer
// and live labels
static ExplorerContext GetLightweightContextFromView(IShellView* pView,
                                                     int selectionCount = -1) {
    ExplorerContext result;
    if (!pView) {
        return result;
    }

    result.shellView.copy_from(pView);
    if (selectionCount < 0) {
        if (auto fv = result.shellView.try_as<IFolderView>()) {
            int cnt = 0;
            if (SUCCEEDED(fv->ItemCount(SVGIO_SELECTION, &cnt)) && cnt > 0) {
                selectionCount = cnt;
            }
        }
    }
    result.selectionCount =
        (selectionCount > 0) ? static_cast<size_t>(selectionCount) : 0;

    PopulateFolderInfoFast(pView, result);

    if (result.selectionCount > 0) {
        winrt::com_ptr<IShellItemArray> items;
        if (SUCCEEDED(pView->GetItemObject(SVGIO_SELECTION,
                                           IID_PPV_ARGS(items.put()))) &&
            items) {
            winrt::com_ptr<IShellItem> firstItem;
            if (SUCCEEDED(items->GetItemAt(0, firstItem.put())) && firstItem) {
                PWSTR disp = nullptr;
                if (SUCCEEDED(firstItem->GetDisplayName(SIGDN_NORMALDISPLAY,
                                                        &disp)) &&
                    disp) {
                    result.firstItemDisplayName = disp;
                    CoTaskMemFree(disp);
                }

                SFGAOF attrs = 0;
                if (SUCCEEDED(
                        firstItem->GetAttributes(SFGAO_FILESYSTEM, &attrs)) &&
                    (attrs & SFGAO_FILESYSTEM)) {
                    PWSTR fsPath = nullptr;
                    if (SUCCEEDED(firstItem->GetDisplayName(SIGDN_FILESYSPATH,
                                                            &fsPath)) &&
                        fsPath) {
                        if (!(fsPath[0] == L':' && fsPath[1] == L':')) {
                            result.selectedPath = fsPath;
                            result.allSelectedPaths.push_back(fsPath);
                        }
                        CoTaskMemFree(fsPath);
                    } else {
                        PWSTR relParse = nullptr;
                        if (SUCCEEDED(firstItem->GetDisplayName(
                                SIGDN_PARENTRELATIVEPARSING, &relParse)) &&
                            relParse) {
                            if (!(relParse[0] == L':' && relParse[1] == L':')) {
                                std::wstring combined = result.folderPath;
                                if (!combined.empty()) {
                                    if (combined.back() != L'\\') {
                                        combined += L'\\';
                                    }
                                    combined += relParse;
                                } else {
                                    combined = relParse;
                                }
                                result.selectedPath = combined;
                                result.allSelectedPaths.push_back(
                                    std::move(combined));
                            }
                            CoTaskMemFree(relParse);
                        }
                    }
                }
            }
        }
    }

    return result;
}

// Fallback helper extracting physical file paths via CF_HDROP when
// IShellItemArray is unavailable
static std::vector<std::wstring> GetSelectedPathsHDROP(IShellView* psv) {
    std::vector<std::wstring> files;
    if (!psv) {
        return files;
    }

    winrt::com_ptr<IDataObject> pdo;
    if (SUCCEEDED(
            psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(pdo.put()))) &&
        pdo) {
        FORMATETC fmt = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1,
                         TYMED_HGLOBAL};
        STGMEDIUM stg = {};
        if (SUCCEEDED(pdo->GetData(&fmt, &stg))) {
            HDROP hDrop = static_cast<HDROP>(stg.hGlobal);
            UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            for (UINT j = 0; j < fileCount; j++) {
                UINT requiredLen = DragQueryFileW(hDrop, j, nullptr, 0);
                if (requiredLen > 0) {
                    std::vector<WCHAR> pathBuf(requiredLen + 1);
                    if (DragQueryFileW(hDrop, j, pathBuf.data(),
                                       requiredLen + 1)) {
                        files.push_back(pathBuf.data());
                    }
                }
            }
            ReleaseStgMedium(&stg);
        }
    }
    return files;
}

// Capture complete physical selection paths and folder classifications when
// invoking an action
static ExplorerContext GetFullExplorerContext(HWND hExplorerWnd) {
    ExplorerContext result;
    if (!hExplorerWnd) {
        return result;
    }

    result.shellView = GetActiveShellView(hExplorerWnd);
    if (!result.shellView) {
        return result;
    }

    PopulateFolderInfoFast(result.shellView.get(), result);

    winrt::com_ptr<IShellItemArray> items;
    if (SUCCEEDED(result.shellView->GetItemObject(SVGIO_SELECTION,
                                                  IID_PPV_ARGS(items.put()))) &&
        items) {
        DWORD count = 0;
        if (SUCCEEDED(items->GetCount(&count))) {
            result.selectionCount = count;
            result.hasClassifiedSelection = true;

            for (DWORD i = 0; i < count; i++) {
                winrt::com_ptr<IShellItem> item;
                if (FAILED(items->GetItemAt(i, item.put())) || !item) {
                    continue;
                }

                if (i == 0) {
                    PWSTR disp = nullptr;
                    if (SUCCEEDED(
                            item->GetDisplayName(SIGDN_NORMALDISPLAY, &disp)) &&
                        disp) {
                        result.firstItemDisplayName = disp;
                        CoTaskMemFree(disp);
                    }
                }

                SFGAOF attrs = 0;
                if (FAILED(item->GetAttributes(
                        SFGAO_FILESYSTEM | SFGAO_FOLDER | SFGAO_STREAM,
                        &attrs))) {
                    continue;
                }

                // Skip virtual items (e.g. This PC drives/Control Panel applets
                // lacking filesystem paths)
                if (!(attrs & SFGAO_FILESYSTEM)) {
                    continue;
                }

                PWSTR path = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) &&
                    path) {
                    std::wstring fsPath(path);
                    CoTaskMemFree(path);

                    result.allSelectedPaths.push_back(fsPath);
                    bool isDir =
                        (attrs & SFGAO_FOLDER) && !(attrs & SFGAO_STREAM);
                    if (isDir) {
                        result.selectedFoldersOnly.push_back(std::move(fsPath));
                    } else {
                        result.selectedFilesOnly.push_back(std::move(fsPath));
                    }
                }
            }
        }
    }

    if (result.allSelectedPaths.empty() && result.isFilesystemFolder) {
        result.allSelectedPaths = GetSelectedPathsHDROP(result.shellView.get());
        if (!result.allSelectedPaths.empty()) {
            result.hasClassifiedSelection = false;
        }
    }

    if (!result.allSelectedPaths.empty()) {
        result.selectedPath = result.allSelectedPaths[0];
    }

    return result;
}

// Summarize selected item counts and extensions in-memory without touching
// physical storage
static SelectionSummary SummarizeSelection(IShellView* psv) {
    SelectionSummary summary;
    if (!psv) {
        return summary;
    }

    winrt::com_ptr<IShellItemArray> items;
    if (FAILED(
            psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(items.put()))) ||
        !items) {
        return summary;
    }

    DWORD count = 0;
    if (FAILED(items->GetCount(&count)) || count == 0) {
        return summary;
    }

    summary.count = count;
    summary.extensions.reserve(count);

    for (DWORD i = 0; i < count; i++) {
        winrt::com_ptr<IShellItem> item;
        if (FAILED(items->GetItemAt(i, item.put())) || !item) {
            continue;
        }

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

// Resolve a safe working directory on the worker thread, falling back to
// %USERPROFILE% for virtual views
static std::wstring GetEffectiveWorkingDirectory(
    ExplorerContext const& context) {
    if (context.isFilesystemFolder && !context.folderPath.empty() &&
        PathFileExistsW(context.folderPath.c_str())) {
        return context.folderPath;
    }
    WCHAR userProfile[MAX_PATH];
    if (GetEnvironmentVariableW(L"USERPROFILE", userProfile,
                                ARRAYSIZE(userProfile))) {
        return userProfile;
    }
    WCHAR sysDir[MAX_PATH];
    if (GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir))) {
        return sysDir;
    }
    return L"C:\\";
}

// Enumerate all CabinetWClass Explorer windows belonging to the current
// explorer.exe process
static std::vector<HWND> GetFileExplorerWnds() {
    std::vector<HWND> hWnds;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& list = *reinterpret_cast<std::vector<HWND>*>(lParam);
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }
            WCHAR className[64];
            if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"CabinetWClass") == 0) {
                list.push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hWnds));
    return hWnds;
}

// ============================================================================
// Parameter Token Expansion, Filter Evaluation & Worker Process Execution
// ============================================================================

// Expand command-line placeholders safely while keeping virtual shell folder
// paths empty
static std::wstring BuildParameters(std::wstring parameters,
                                    ExplorerContext const& context) {
    if (parameters.empty() || parameters.find(L'%') == std::wstring::npos) {
        return parameters;
    }

    // Mask escaped literal '%%' sequences before evaluating dynamic
    // placeholders
    bool hasEscapedPercent = (parameters.find(L"%%") != std::wstring::npos);
    if (hasEscapedPercent) {
        ReplacePlaceholder(parameters, L"%%", L"\x01");
    }

    // Classify selected items into files vs. folders using pre-captured shell
    // attributes or worker fallback
    std::vector<std::wstring> filesOnly;
    std::vector<std::wstring> foldersOnly;
    bool needsItemClassification =
        (parameters.find(L"%files") != std::wstring::npos ||
         parameters.find(L"%folders") != std::wstring::npos ||
         parameters.find(L"%d_smart") != std::wstring::npos);

    if (needsItemClassification) {
        if (context.hasClassifiedSelection) {
            filesOnly = context.selectedFilesOnly;
            foldersOnly = context.selectedFoldersOnly;
        } else {
            for (const auto& item : context.allSelectedPaths) {
                DWORD attr = GetFileAttributesW(item.c_str());
                if (attr != INVALID_FILE_ATTRIBUTES &&
                    (attr & FILE_ATTRIBUTE_DIRECTORY)) {
                    foldersOnly.push_back(item);
                } else if (attr != INVALID_FILE_ATTRIBUTES) {
                    filesOnly.push_back(item);
                }
            }
        }
    }

    // Strip residual %sel_each% tokens so %sel does not partially match their
    // prefix
    ReplacePlaceholder(parameters, L"\"%sel_each%\"", L"");
    ReplacePlaceholder(parameters, L"%sel_each%", L"");
    ReplacePlaceholder(parameters, L"\"%sel_each\"", L"");
    ReplacePlaceholder(parameters, L"%sel_each", L"");

    // Expand %files% and %folders% filtered selection lists
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

    // Expand %base%, %basename%, and %file_name% tokens
    if (parameters.find(L"%base") != std::wstring::npos ||
        parameters.find(L"%file_name") != std::wstring::npos) {
        std::vector<std::wstring> baseNames;
        for (const auto& p : context.allSelectedPaths) {
            baseNames.push_back(ExtractBaseName(p));
        }
        std::wstring quotedBases = FormatSelectedItemsQuoted(baseNames);
        std::wstring firstBase =
            !context.allSelectedPaths.empty()
                ? ExtractBaseName(context.allSelectedPaths[0])
                : ExtractBaseName(context.firstItemDisplayName);

        ReplacePlaceholder(parameters, L"\"%basename%\"", quotedBases);
        ReplacePlaceholder(parameters, L"%basename%", quotedBases);
        ReplacePlaceholder(parameters, L"\"%basename\"", quotedBases);
        ReplacePlaceholder(parameters, L"%basename", quotedBases);

        ReplacePlaceholder(parameters, L"\"%base%\"", quotedBases);
        ReplacePlaceholder(parameters, L"%base%", quotedBases);
        ReplacePlaceholder(parameters, L"\"%base\"", quotedBases);
        ReplacePlaceholder(parameters, L"%base", quotedBases);

        std::wstring quotedFirstBase =
            firstBase.empty() ? L"" : QuoteArg(firstBase);
        ReplacePlaceholder(parameters, L"\"%file_name%\"", quotedFirstBase);
        ReplacePlaceholder(parameters, L"%file_name%", firstBase);
        ReplacePlaceholder(parameters, L"\"%file_name\"", quotedFirstBase);
        ReplacePlaceholder(parameters, L"%file_name", firstBase);
    }

    // Expand %name% and %n% item filename tokens
    if (parameters.find(L"%n") != std::wstring::npos ||
        parameters.find(L"%name") != std::wstring::npos) {
        std::vector<std::wstring> names;
        for (const auto& p : context.allSelectedPaths) {
            PCWSTR name = PathFindFileNameW(p.c_str());
            if (name && *name) {
                names.push_back(name);
            }
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

    // Expand %1% first selected full path token
    if (parameters.find(L"%1") != std::wstring::npos) {
        std::wstring firstItem = context.allSelectedPaths.empty()
                                     ? L""
                                     : QuoteArg(context.allSelectedPaths[0]);
        ReplacePlaceholder(parameters, L"\"%1%\"", firstItem);
        ReplacePlaceholder(parameters, L"%1%", firstItem);
        ReplacePlaceholder(parameters, L"\"%1\"", firstItem);
        ReplacePlaceholder(parameters, L"%1", firstItem);
    }

    // Expand %ext% extension of first selected item
    if (parameters.find(L"%ext") != std::wstring::npos) {
        std::wstring ext;
        if (!context.allSelectedPaths.empty()) {
            PCWSTR extPtr =
                PathFindExtensionW(context.allSelectedPaths[0].c_str());
            if (extPtr) {
                ext = extPtr;
            }
        }
        ReplacePlaceholder(parameters, L"%ext%", ext);
        ReplacePlaceholder(parameters, L"%ext", ext);
    }

    // Expand %count% and %c% selection count tokens
    size_t totalSelected = context.selectionCount > 0
                               ? context.selectionCount
                               : context.allSelectedPaths.size();
    std::wstring countStr = std::to_wstring(totalSelected);
    ReplacePlaceholder(parameters, L"%count%", countStr);
    ReplacePlaceholder(parameters, L"%count", countStr);
    ReplacePlaceholder(parameters, L"%c%", countStr);
    ReplacePlaceholder(parameters, L"%c", countStr);

    // Expand %d_name% using native shell display name (supports This PC /
    // Recycle Bin cleanly)
    if (parameters.find(L"%d_name") != std::wstring::npos) {
        std::wstring dName = context.folderDisplayName;
        if (dName.empty() && !context.folderPath.empty()) {
            std::wstring dir = context.folderPath;
            while (!dir.empty() &&
                   (dir.back() == L'\\' || dir.back() == L'/')) {
                dir.pop_back();
            }
            PCWSTR namePtr = PathFindFileNameW(dir.c_str());
            if (namePtr) {
                dName = namePtr;
            }
        }
        std::wstring quotedDName = dName.empty() ? L"" : QuoteArg(dName);
        ReplacePlaceholder(parameters, L"\"%d_name%\"", quotedDName);
        ReplacePlaceholder(parameters, L"%d_name%", dName);
        ReplacePlaceholder(parameters, L"\"%d_name\"", quotedDName);
        ReplacePlaceholder(parameters, L"%d_name", dName);
    }

    // Expand %sel% and %f% full selection list tokens
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

    // Resolve physical folder path strictly when SFGAO_FILESYSTEM was verified
    // (empty on virtual folders)
    std::wstring physicalDir =
        (context.isFilesystemFolder && !context.folderPath.empty())
            ? context.folderPath
            : L"";
    std::wstring quotedPhysicalPath =
        physicalDir.empty() ? L"" : QuoteArg(physicalDir);

    // Expand %d_smart% to first selected folder or active physical folder
    if (parameters.find(L"%d_smart") != std::wstring::npos) {
        std::wstring smartDir =
            !foldersOnly.empty() ? foldersOnly[0] : physicalDir;
        std::wstring quotedSmart = smartDir.empty() ? L"" : QuoteArg(smartDir);
        ReplacePlaceholder(parameters, L"\"%d_smart%\"", quotedSmart);
        ReplacePlaceholder(parameters, L"%d_smart%", smartDir);
        ReplacePlaceholder(parameters, L"\"%d_smart\"", quotedSmart);
        ReplacePlaceholder(parameters, L"%d_smart", smartDir);
    }

    // Expand %path%, %dir%, and %d% active directory tokens
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

    // Expand %parent% and %p% parent directory tokens
    if (parameters.find(L"%p") != std::wstring::npos ||
        parameters.find(L"%parent") != std::wstring::npos) {
        std::wstring p;
        if (!physicalDir.empty()) {
            std::wstring parent = physicalDir;
            while (!parent.empty() &&
                   (parent.back() == L'\\' || parent.back() == L'/')) {
                parent.pop_back();
            }
            if (parent.length() == 2 && parent[1] == L':') {
                p = parent + L'\\';
            } else {
                size_t slash = parent.find_last_of(L"\\/");
                if (slash != std::wstring::npos) {
                    p = parent.substr(0, slash);
                    if (p.length() == 2 && p[1] == L':') {
                        p += L'\\';
                    }
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

    // Restore escaped literal percent signs
    if (hasEscapedPercent) {
        ReplacePlaceholder(parameters, L"\x01", L"%");
    }

    return parameters;
}

// Resolve dynamic UI button/menu label tokens with zero disk I/O and virtual
// shell item support
static std::wstring ResolveLabelTokens(std::wstring label,
                                       const ExplorerContext& context) {
    if (label.find(L'%') == std::wstring::npos) {
        return label;
    }

    bool hasEscapedPercent = (label.find(L"%%") != std::wstring::npos);
    if (hasEscapedPercent) {
        ReplacePlaceholder(label, L"%%", L"\x01");
    }

    size_t countVal = context.selectionCount > 0
                          ? context.selectionCount
                          : context.allSelectedPaths.size();
    std::wstring countStr = std::to_wstring(countVal);
    ReplacePlaceholder(label, L"%count%", countStr);
    ReplacePlaceholder(label, L"%count", countStr);
    ReplacePlaceholder(label, L"%c%", countStr);
    ReplacePlaceholder(label, L"%c", countStr);

    // Determine primary item text from filesystem parsing name or virtual
    // normal display name
    std::wstring primaryItemRef = !context.allSelectedPaths.empty()
                                      ? context.allSelectedPaths[0]
                                      : context.firstItemDisplayName;

    std::wstring baseName = ExtractBaseName(primaryItemRef);
    ReplacePlaceholder(label, L"%basename%", baseName);
    ReplacePlaceholder(label, L"%basename", baseName);
    ReplacePlaceholder(label, L"%file_name%", baseName);
    ReplacePlaceholder(label, L"%file_name", baseName);
    ReplacePlaceholder(label, L"%base%", baseName);
    ReplacePlaceholder(label, L"%base", baseName);

    std::wstring fileName;
    if (!primaryItemRef.empty()) {
        PCWSTR n = PathFindFileNameW(primaryItemRef.c_str());
        fileName = (n && *n) ? n : primaryItemRef;
    }
    ReplacePlaceholder(label, L"%name%", fileName);
    ReplacePlaceholder(label, L"%name", fileName);
    ReplacePlaceholder(label, L"%n%", fileName);
    ReplacePlaceholder(label, L"%n", fileName);
    ReplacePlaceholder(label, L"%1%", fileName);
    ReplacePlaceholder(label, L"%1", fileName);

    std::wstring ext;
    if (!context.allSelectedPaths.empty()) {
        PCWSTR e = PathFindExtensionW(context.allSelectedPaths[0].c_str());
        if (e) {
            ext = e;
        }
    }
    ReplacePlaceholder(label, L"%ext%", ext);
    ReplacePlaceholder(label, L"%ext", ext);

    std::wstring dName = context.folderDisplayName;
    if (dName.empty() && !context.folderPath.empty()) {
        std::wstring dir = context.folderPath;
        while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
            dir.pop_back();
        }
        PCWSTR dirName = PathFindFileNameW(dir.c_str());
        if (dirName) {
            dName = dirName;
        }
    }
    ReplacePlaceholder(label, L"%d_name%", dName);
    ReplacePlaceholder(label, L"%d_name", dName);

    std::wstring safePath =
        context.isFilesystemFolder ? context.folderPath : L"";
    ReplacePlaceholder(label, L"%path%", safePath);
    ReplacePlaceholder(label, L"%path", safePath);
    ReplacePlaceholder(label, L"%dir%", safePath);
    ReplacePlaceholder(label, L"%dir", safePath);
    ReplacePlaceholder(label, L"%d%", safePath);
    ReplacePlaceholder(label, L"%d", safePath);

    if (hasEscapedPercent) {
        ReplacePlaceholder(label, L"\x01", L"%");
    }

    return label;
}

// Evaluate matchExtensions boolean filter expressions (OR `,`, AND `+`,
// exclusions `-`, `only`, `hide`)
static bool ItemMatchesExtensionFilter(const std::wstring& filterPattern,
                                       const SelectionSummary& summary) {
    std::wstring filter = TrimWhitespaceAndQuotes(filterPattern);
    if (filter.empty() || filter == L"*") {
        return true;
    }

    // Check global `hide` / `hidden` override token across word boundaries
    std::wstring lowerFilter = ToLower(filter);
    size_t hPos = 0;
    while ((hPos = lowerFilter.find_first_of(L"hH", hPos)) !=
           std::wstring::npos) {
        std::wstring_view sv(lowerFilter.c_str() + hPos);
        size_t matchLen = 0;
        if (sv.starts_with(L"hidden")) {
            matchLen = 6;
        } else if (sv.starts_with(L"hide")) {
            matchLen = 4;
        }

        if (matchLen > 0) {
            bool leftBoundary =
                (hPos == 0) ||
                (wcschr(L",;+|/ \t()", lowerFilter[hPos - 1]) != nullptr);
            bool rightBoundary =
                (hPos + matchLen == lowerFilter.size()) ||
                (wcschr(L",;+|/ \t()", lowerFilter[hPos + matchLen]) !=
                 nullptr);
            if (leftBoundary && rightBoundary) {
                return false;
            }
        }
        hPos++;
    }

    // Split top-level OR branches and negative veto rules while respecting
    // parentheses
    std::vector<std::wstring> orBranches;
    std::vector<std::wstring> negativeTokens;

    size_t start = 0;
    int parenDepth = 0;
    for (size_t i = 0; i <= filter.size(); i++) {
        if (i < filter.size()) {
            if (filter[i] == L'(') {
                parenDepth++;
            } else if (filter[i] == L')' && parenDepth > 0) {
                parenDepth--;
            }
        }

        if (i == filter.size() ||
            (parenDepth == 0 && (filter[i] == L',' || filter[i] == L';'))) {
            std::wstring rawToken =
                TrimWhitespaceAndQuotes(filter.substr(start, i - start));
            start = i + 1;

            if (rawToken.empty()) {
                continue;
            }

            std::wstring lower = ToLower(rawToken);
            if (lower == L"hide" || lower == L"hidden") {
                return false;
            }

            if (rawToken.front() == L'-' || rawToken.front() == L'~') {
                rawToken.erase(rawToken.begin());
                rawToken = TrimWhitespaceAndQuotes(rawToken);
                if (!rawToken.empty() && rawToken.front() == L'.') {
                    rawToken.erase(rawToken.begin());
                }
                rawToken = ToLower(rawToken);
                if (rawToken == L"dir" || rawToken == L"directory" ||
                    rawToken == L"folders") {
                    rawToken = L"folder";
                }
                if (rawToken == L"files") {
                    rawToken = L"file";
                }
                negativeTokens.push_back(rawToken);
            } else {
                orBranches.push_back(rawToken);
            }
        }
    }

    // Enforce global negative exclusions across all selected items
    if (summary.count > 0 && !negativeTokens.empty()) {
        for (const auto& ext : summary.extensions) {
            bool isDir = (ext == L"folder");
            for (const auto& neg : negativeTokens) {
                if (isDir && neg == L"folder") {
                    return false;
                }
                if (!isDir && (neg == L"file" || ext == neg)) {
                    return false;
                }
            }
        }
    }

    if (orBranches.empty()) {
        return summary.count > 0;
    }

    // Evaluate a single atomic filter predicate against the current selection
    // summary
    auto evalAtomicToken = [&](std::wstring token) -> bool {
        token = ToLower(TrimWhitespaceAndQuotes(token));
        if (token == L"hide" || token == L"hidden") {
            return false;
        }

        if (token.starts_with(L"only ") || token.starts_with(L"only:") ||
            token.starts_with(L"only=") || token.starts_with(L"only(")) {
            if (summary.count == 0 || summary.folderCount > 0) {
                return false;
            }

            std::wstring content;
            if (token.starts_with(L"only(")) {
                size_t closeParen = token.find(L')', 5);
                if (closeParen == std::wstring::npos) {
                    closeParen = token.size();
                }
                content = token.substr(5, closeParen - 5);
            } else {
                content = token.substr(4);
                if (!content.empty() &&
                    (content.front() == L':' || content.front() == L'=')) {
                    content = content.substr(1);
                }
            }

            std::vector<std::wstring> allowedExts;
            size_t cStart = 0;
            while (cStart < content.size()) {
                size_t cEnd = content.find_first_of(L",;|/ \t", cStart);
                if (cEnd == std::wstring::npos) {
                    cEnd = content.size();
                }
                std::wstring ext = TrimWhitespaceAndQuotes(
                    content.substr(cStart, cEnd - cStart));
                cStart = cEnd + 1;

                if (!ext.empty() && ext.front() == L'.') {
                    ext.erase(ext.begin());
                }
                if (!ext.empty()) {
                    allowedExts.push_back(ext);
                }
            }

            if (allowedExts.empty()) {
                return false;
            }

            for (const auto& ext : summary.extensions) {
                bool matchedAny = false;
                for (const auto& allowed : allowedExts) {
                    if (ext == allowed) {
                        matchedAny = true;
                        break;
                    }
                }
                if (!matchedAny) {
                    return false;
                }
            }
            return true;
        }

        if (!token.empty() && token.front() == L'.') {
            token.erase(token.begin());
        }

        if (token == L"none" || token == L"empty" || token == L"nothing" ||
            token == L"unselected") {
            return summary.count == 0;
        }

        if (summary.count == 0) {
            return false;
        }

        if (token == L"*" || token == L"any" || token == L"selected") {
            return true;
        }
        if (token == L"files" || token == L"file") {
            return summary.fileCount > 0 && summary.folderCount == 0;
        }
        if (token == L"folders" || token == L"folder" || token == L"dir" ||
            token == L"directory") {
            return summary.folderCount > 0 && summary.fileCount == 0;
        }
        if (token == L"mixed" || token == L"mixed_selection") {
            return summary.fileCount > 0 && summary.folderCount > 0;
        }
        if (token == L"single") {
            return summary.count == 1;
        }
        if (token == L"multi" || token == L"multiple") {
            return summary.count >= 2;
        }
        if (token == L"single_file" || token == L"single_files") {
            return summary.fileCount == 1 && summary.folderCount == 0;
        }
        if (token == L"multi_file" || token == L"multi_files") {
            return summary.fileCount >= 2 && summary.folderCount == 0;
        }
        if (token == L"single_folder" || token == L"single_folders") {
            return summary.folderCount == 1 && summary.fileCount == 0;
        }
        if (token == L"multi_folder" || token == L"multi_folders") {
            return summary.folderCount >= 2 && summary.fileCount == 0;
        }

        for (const auto& ext : summary.extensions) {
            if (ext != L"folder" && ext == token) {
                return true;
            }
        }
        return false;
    };

    // Evaluate OR branches where '+' requires all sub-predicates in that branch
    // to match
    for (const auto& branch : orBranches) {
        bool branchPassed = true;
        size_t bStart = 0;
        while (bStart < branch.size()) {
            size_t bEnd = branch.find(L'+', bStart);
            if (bEnd == std::wstring::npos) {
                bEnd = branch.size();
            }
            std::wstring subToken =
                TrimWhitespaceAndQuotes(branch.substr(bStart, bEnd - bStart));
            bStart = bEnd + 1;

            if (subToken.empty()) {
                continue;
            }
            std::wstring lowerSub = ToLower(subToken);
            if (lowerSub == L"hide" || lowerSub == L"hidden") {
                return false;
            }
            if (!evalAtomicToken(subToken)) {
                branchPassed = false;
                break;
            }
        }

        if (branchPassed) {
            return true;
        }
    }

    return false;
}

// Mutex and handle list tracking background worker threads so Wh_ModUninit
// joins cleanly
static std::mutex g_launchThreadsMutex;
static std::vector<HANDLE> g_launchThreads;

// Prune completed worker thread handles and record a newly spawned worker
// thread
static void TrackLaunchThread(HANDLE thread) {
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

// Wait for all active background command-launching worker threads to finish
// before DLL unload
static void WaitForLaunchThreads() {
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

// Spawn a COM-initialized background worker thread to execute shell or process
// operations off the UI thread
static void RunShellWorkOnWorkerThread(std::function<void()> work) {
    if (g_unloading) {
        return;
    }
    auto* pWork = new std::function<void()>(std::move(work));
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            std::unique_ptr<std::function<void()>> workPtr(
                reinterpret_cast<std::function<void()>*>(lpParam));
            if (g_unloading) {
                return 0;
            }
            ComApartmentScope comScope(COINIT_APARTMENTTHREADED |
                                       COINIT_DISABLE_OLE1DDE);
            try {
                (*workPtr)();
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

// Invoke ShellExecuteExW synchronously on the worker thread without blocking
// Explorer's UI
static void ExecuteProcess(std::wstring const& command,
                           std::wstring const& parameters,
                           std::wstring const& workingDir) {
    SHELLEXECUTEINFOW execInfo{};
    execInfo.cbSize = sizeof(execInfo);
    execInfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    execInfo.lpFile = command.c_str();
    execInfo.lpParameters = parameters.empty() ? nullptr : parameters.c_str();
    execInfo.lpDirectory = !workingDir.empty() ? workingDir.c_str() : nullptr;
    execInfo.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&execInfo)) {
        Wh_Log(L"ShellExecuteExW failed for %s: %u", command.c_str(),
               GetLastError());
    }
}

// Resolve command path, working directory, and batch/single parameter tokens on
// the worker thread
static void LaunchItemForWindow(HWND hExplorerWnd,
                                ActionItem const& item,
                                ExplorerContext const& capturedContext) {
    std::wstring command = ResolveCommandPath(item.command);
    Wh_Log(L"Launching %s for window %08X, path: %s", item.command.c_str(),
           static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hExplorerWnd)),
           capturedContext.folderPath.c_str());

    std::wstring workingDir = GetEffectiveWorkingDirectory(capturedContext);

    if (item.parameters.find(L"%sel_each%") != std::wstring::npos ||
        item.parameters.find(L"%sel_each") != std::wstring::npos) {
        if (capturedContext.allSelectedPaths.empty()) {
            // Skip launching empty per-item batch commands in virtual locations
            // with no physical files
            if (!capturedContext.isFilesystemFolder &&
                capturedContext.selectionCount > 0) {
                Wh_Log(
                    L"Skipping %%sel_each%% in virtual folder without "
                    L"filesystem items");
                return;
            }
            std::wstring param = item.parameters;
            ReplacePlaceholder(param, L"\"%sel_each%\"", L"");
            ReplacePlaceholder(param, L"%sel_each%", L"");
            ReplacePlaceholder(param, L"\"%sel_each\"", L"");
            ReplacePlaceholder(param, L"%sel_each", L"");
            param = BuildParameters(param, capturedContext);
            ExecuteProcess(command, param, workingDir);
            return;
        }

        size_t count =
            std::min(capturedContext.allSelectedPaths.size(), kMaxBatchSpawn);
        for (size_t i = 0; i < count; i++) {
            const auto& path = capturedContext.allSelectedPaths[i];
            std::wstring param = item.parameters;
            std::wstring quotedPath = QuoteArg(path);

            ReplacePlaceholder(param, L"\"%sel_each%\"", quotedPath);
            ReplacePlaceholder(param, L"\"%sel_each\"", quotedPath);
            ReplacePlaceholder(param, L"%sel_each%", quotedPath);
            ReplacePlaceholder(param, L"%sel_each", quotedPath);

            PCWSTR namePtr = PathFindFileNameW(path.c_str());
            PCWSTR extPtr = PathFindExtensionW(path.c_str());
            std::wstring baseName = ExtractBaseName(path);

            std::wstring nameStr = namePtr ? namePtr : L"";
            if (!nameStr.empty() && nameStr.find(L' ') != std::wstring::npos) {
                nameStr = QuoteArg(nameStr);
            }
            ReplacePlaceholder(param, L"%basename%", baseName);
            ReplacePlaceholder(param, L"%basename", baseName);
            ReplacePlaceholder(param, L"%file_name%", baseName);
            ReplacePlaceholder(param, L"%file_name", baseName);
            ReplacePlaceholder(param, L"%base%", baseName);
            ReplacePlaceholder(param, L"%base", baseName);
            ReplacePlaceholder(param, L"%name%", nameStr);
            ReplacePlaceholder(param, L"%name", nameStr);
            ReplacePlaceholder(param, L"%n%", nameStr);
            ReplacePlaceholder(param, L"%n", nameStr);
            ReplacePlaceholder(param, L"%ext%", extPtr ? extPtr : L"");
            ReplacePlaceholder(param, L"%ext", extPtr ? extPtr : L"");

            param = BuildParameters(param, capturedContext);
            ExecuteProcess(command, param, workingDir);
        }
        return;
    }

    std::wstring parameters = BuildParameters(item.parameters, capturedContext);
    ExecuteProcess(command, parameters, workingDir);
}

// Synthesize an Alt+P or Alt+Shift+P shortcut cleanly while neutralizing held
// Ctrl/Shift modifiers
static void SendExplorerPaneToggleShortcut(bool withShift) {
    std::vector<INPUT> inputs;
    inputs.reserve(10);

    auto pushKey = [&](WORD vk, DWORD flags) {
        INPUT in{};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = vk;
        in.ki.dwFlags = flags;
        inputs.push_back(in);
    };

    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
        pushKey(VK_CONTROL, KEYEVENTF_KEYUP);
    }
    if (!withShift && (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
        pushKey(VK_SHIFT, KEYEVENTF_KEYUP);
    }

    pushKey(VK_MENU, 0);
    if (withShift) {
        pushKey(VK_SHIFT, 0);
    }
    pushKey('P', 0);
    pushKey('P', KEYEVENTF_KEYUP);
    if (withShift) {
        pushKey(VK_SHIFT, KEYEVENTF_KEYUP);
    }
    pushKey(VK_MENU, KEYEVENTF_KEYUP);

    SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
}

// Handle toolbar button or menu item click and dispatch shell/process execution
// safely
static void OnActionInvoked(
    winrt::Microsoft::UI::Xaml::FrameworkElement const& elementForWindow,
    ActionItem const& item) {
    UiCallScope scope;
    if (item.command.empty() || g_unloading) {
        return;
    }

    HWND hWnd = GetExplorerWindowForElement(elementForWindow);
    std::wstring_view cmd = item.command;

    if (IsCommandMatch(cmd, L"TogglePreview")) {
        SendExplorerPaneToggleShortcut(false);
        return;
    }

    if (IsCommandMatch(cmd, L"ToggleDetails")) {
        SendExplorerPaneToggleShortcut(true);
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
        ExplorerContext context = GetFullExplorerContext(hWnd);
        std::wstring targetPath;
        if (!context.selectedFilesOnly.empty()) {
            targetPath = context.selectedFilesOnly[0];
        } else if (!context.hasClassifiedSelection &&
                   !context.selectedPath.empty()) {
            targetPath = context.selectedPath;
        }

        if (!targetPath.empty()) {
            RunShellWorkOnWorkerThread([hWnd, targetPath]() {
                if (PathIsDirectoryW(targetPath.c_str()) == FALSE) {
                    OPENASINFO oai = {};
                    oai.pcszFile = targetPath.c_str();
                    oai.oaifInFlags = OAIF_EXEC | OAIF_ALLOW_REGISTRATION;
                    SHOpenWithDialog(hWnd, &oai);
                }
            });
        }
        return;
    }

    ExplorerContext context = GetFullExplorerContext(hWnd);
    context.shellView = nullptr;
    RunShellWorkOnWorkerThread(
        [hWnd, item, context]() { LaunchItemForWindow(hWnd, item, context); });
}

// ============================================================================
// Icon Extraction, Premultiplied Bitmap Decoding & Theme-Aware Caching
// ============================================================================

// Premultiplied BGRA pixel buffer decoded from HICON or shell image factories
struct DecodedIcon {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> pixels;
    bool empty() const { return pixels.empty(); }
};

static std::mutex g_iconCacheMutex;
static std::unordered_map<std::wstring, std::shared_ptr<DecodedIcon>>
    g_iconCache;

// Flush all cached decoded icon bitmaps under lock
static void ClearIconCache() {
    std::lock_guard<std::mutex> lock(g_iconCacheMutex);
    g_iconCache.clear();
}

// Follow WindowsApps AppExecLink reparse points (e.g. wt.exe) to their real
// binary path for icon extraction
static std::wstring ResolveAppExecutionAlias(std::wstring const& path) {
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
        if (p + length >= end) {
            break;
        }
        if (i == 2) {
            return std::wstring(p, length);
        }
        p += length + 1;
    }

    return path;
}

// Extract the primary HICON from an executable command path
static HICON ExtractCommandIcon(std::wstring const& command) {
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
    Wh_Log(L"Couldn't get an icon for %s (%s)", command.c_str(), path.c_str());
    return nullptr;
}

// Read top-down 32-bpp RGB/BGRA pixels out of a GDI HBITMAP
static bool ReadBitmapPixels(HBITMAP hBitmap, DecodedIcon* decoded) {
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
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<uint8_t> pixels(static_cast<size_t>(width) * height * 4);
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

// Check whether a 32-bpp bitmap has an all-zero legacy GDI alpha channel
static bool HasNoAlphaChannel(std::vector<uint8_t> const& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        if (pixels[p]) {
            return false;
        }
    }
    return true;
}

// Force alpha bytes to 255 for opaque 24-bit GDI bitmaps lacking alpha data
static void MakeOpaque(std::vector<uint8_t>& pixels) {
    for (size_t p = 3; p < pixels.size(); p += 4) {
        pixels[p] = 255;
    }
}

// Decode a monochrome AND/XOR mask HBITMAP into an RGBA pixel buffer
static bool DecodeMonochromeIcon(HBITMAP hbmMask, DecodedIcon* decoded) {
    DecodedIcon mask;
    if (!ReadBitmapPixels(hbmMask, &mask) || mask.height % 2 != 0) {
        return false;
    }

    int width = mask.width;
    int height = mask.height / 2;

    DecodedIcon result;
    result.width = width;
    result.height = height;
    result.pixels.assign(static_cast<size_t>(width) * height * 4, 0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            size_t andIndex = (static_cast<size_t>(y) * width + x) * 4;
            size_t xorIndex = (static_cast<size_t>(y + height) * width + x) * 4;
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

// Convert a Win32 HICON into premultiplied BGRA pixels required by WinUI
// WriteableBitmap
static bool DecodeIcon(HICON hIcon, DecodedIcon* decoded) {
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

// Materialize a XAML WriteableBitmap ImageSource on the UI thread from cached
// BGRA pixels
static winrt::Microsoft::UI::Xaml::Media::ImageSource CreateImageSource(
    DecodedIcon const& decoded) try {
    if (decoded.empty()) {
        return nullptr;
    }
    winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap bitmap(
        decoded.width, decoded.height);
    memcpy(bitmap.PixelBuffer().data(), decoded.pixels.data(),
           decoded.pixels.size());
    bitmap.Invalidate();
    return bitmap;
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
    return nullptr;
}

// Parse a single literal glyph character or hex codepoint string (e.g. "E713")
static std::wstring ParseGlyphSetting(PCWSTR glyphSetting) {
    if (!glyphSetting[0]) {
        return std::wstring();
    }
    if (!glyphSetting[1]) {
        return std::wstring(1, glyphSetting[0]);
    }
    for (PCWSTR p = glyphSetting; *p; p++) {
        if (!iswxdigit(*p)) {
            Wh_Log(L"%s is not a glyph code point", glyphSetting);
            return std::wstring();
        }
    }
    unsigned long parsed = wcstoul(glyphSetting, nullptr, 16);
    if (parsed > 0 && parsed <= 0xFFFF) {
        return std::wstring(1, static_cast<WCHAR>(parsed));
    }
    return std::wstring();
}

// Detect whether an icon setting string represents a file/shell path or
// executable name
static bool LooksLikeIconPath(std::wstring const& iconSetting) {
    if (iconSetting.find(L'\\') != std::wstring::npos ||
        iconSetting.find(L'/') != std::wstring::npos ||
        iconSetting.find(L':') != std::wstring::npos) {
        return true;
    }

    std::wstring lower = ToLower(iconSetting);
    size_t comma = lower.rfind(L',');
    if (comma != std::wstring::npos) {
        lower.resize(comma);
    }

    return lower.ends_with(L".exe") || lower.ends_with(L".dll") ||
           lower.ends_with(L".ico");
}

// Load an HICON from an icon path with optional `,index` resource specifier
static HICON LoadIconFromPath(std::wstring const& iconPath) {
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

    expanded = ResolveAppExecutionAlias(ResolveCommandPath(expanded));

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

// Check if a string starts with the `shell:` virtual namespace prefix
static bool IsShellPath(std::wstring const& s) {
    return s.size() >= 6 && _wcsnicmp(s.c_str(), L"shell:", 6) == 0;
}

// Render UWP / Store `shell:AppsFolder\...` icons via IShellItemImageFactory
static bool DecodeShellPathIcon(std::wstring const& path,
                                DecodedIcon* decoded) {
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
    if (succeeded && HasNoAlphaChannel(decoded->pixels)) {
        MakeOpaque(decoded->pixels);
    }
    return succeeded;
}

// Decode an icon from a path, shell URI, or target command executable
static std::shared_ptr<DecodedIcon> ResolveIcon(std::wstring const& iconSetting,
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

// Retrieve a decoded icon from the thread-safe cache or decode and store it on
// miss
static std::shared_ptr<DecodedIcon> GetIcon(std::wstring const& iconSetting,
                                            std::wstring const& command) {
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

// Construct a Segoe Fluent Icons FontIcon element for a Unicode glyph string
static winrt::Microsoft::UI::Xaml::Controls::IconElement CreateGlyphIcon(
    PCWSTR glyph) {
    winrt::Microsoft::UI::Xaml::Controls::FontIcon fontIcon;
    fontIcon.FontFamily(
        winrt::Microsoft::UI::Xaml::Media::FontFamily(L"Segoe Fluent Icons"));
    fontIcon.Glyph(glyph);
    return fontIcon;
}

// Determine whether dark mode is active using the XAML ActualTheme or
// Personalize registry fallback
static bool IsDarkThemeActive(
    winrt::Microsoft::UI::Xaml::ElementTheme actualTheme =
        winrt::Microsoft::UI::Xaml::ElementTheme::Default) {
    if (actualTheme == winrt::Microsoft::UI::Xaml::ElementTheme::Dark) {
        return true;
    }
    if (actualTheme == winrt::Microsoft::UI::Xaml::ElementTheme::Light) {
        return false;
    }
    DWORD useLightTheme = 1;
    DWORD size = sizeof(useLightTheme);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &useLightTheme, &size);
    return useLightTheme == 0;
}

// Select the Light or Dark half of a `LightIcon | DarkIcon` configuration
// string
static std::wstring ResolveThemedIconString(
    const std::wstring& rawSetting,
    winrt::Microsoft::UI::Xaml::ElementTheme theme) {
    size_t pipePos = rawSetting.find(L'|');
    if (pipePos == std::wstring::npos) {
        return TrimWhitespaceAndQuotes(rawSetting);
    }

    std::wstring lightTheme =
        TrimWhitespaceAndQuotes(rawSetting.substr(0, pipePos));
    std::wstring darkTheme =
        TrimWhitespaceAndQuotes(rawSetting.substr(pipePos + 1));

    if (lightTheme.empty()) {
        lightTheme = darkTheme;
    }
    if (darkTheme.empty()) {
        darkTheme = lightTheme;
    }

    return IsDarkThemeActive(theme) ? darkTheme : lightTheme;
}

// Attempt to build an ImageIcon or FontIcon for an item, returning nullptr if
// neither resolves
static winrt::Microsoft::UI::Xaml::Controls::IconElement TryCreateIconElement(
    std::wstring const& iconSetting,
    std::wstring const& command,
    winrt::Microsoft::UI::Xaml::ElementTheme theme =
        winrt::Microsoft::UI::Xaml::ElementTheme::Default) {
    std::wstring effectiveSetting = ResolveThemedIconString(iconSetting, theme);

    bool isPath =
        !effectiveSetting.empty() && LooksLikeIconPath(effectiveSetting);
    if (isPath || effectiveSetting.empty()) {
        if (auto source =
                CreateImageSource(*GetIcon(effectiveSetting, command))) {
            winrt::Microsoft::UI::Xaml::Controls::ImageIcon imageIcon;
            imageIcon.Source(source);
            return imageIcon;
        }
    }

    std::wstring glyph;
    if (!isPath) {
        glyph = ParseGlyphSetting(effectiveSetting.c_str());
    }
    if (glyph.empty()) {
        return nullptr;
    }
    return CreateGlyphIcon(glyph.c_str());
}

// Create an icon element for a toolbar button with fallback to a default Segoe
// Fluent glyph
static winrt::Microsoft::UI::Xaml::Controls::IconElement MakeCommandButtonIcon(
    ActionItem const& item,
    winrt::Microsoft::UI::Xaml::ElementTheme theme) {
    if (item.hideIcon) {
        return nullptr;
    }

    PCWSTR defaultGlyph = L"\uE783";
    std::wstring_view cmd = item.command;

    if (IsCommandMatch(cmd, L"OpenWith")) {
        defaultGlyph = L"\uE7AC";
    } else if (IsCommandMatch(cmd, L"TogglePreview")) {
        defaultGlyph = L"\uE8A1";
    } else if (IsCommandMatch(cmd, L"ToggleDetails")) {
        defaultGlyph = L"\uE9F9";
    } else if (IsCommandMatch(cmd, L"FolderOptions")) {
        defaultGlyph = L"\uE713";
    }

    if (auto icon = TryCreateIconElement(item.icon, item.command, theme)) {
        return icon;
    }
    return CreateGlyphIcon(defaultGlyph);
}

// ============================================================================
// Forward Declarations for CommandBar Coordination
// ============================================================================

static bool IsOurElement(
    winrt::Microsoft::UI::Xaml::Controls::ICommandBarElement const& command);
static void QueueCommandBarUpdate(
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::CommandBar> const&
        weakCommandBar,
    bool fullUpdate);

// ============================================================================
// System Default Buttons Visibility & Custom Label Overrides
// ============================================================================

// Tracked state for built-in Explorer UIElements so visibility, identity, and
// labels restore cleanly on unload
struct ManagedElement {
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::UIElement> element;
    int identifiedIndex = -1;
    bool hasIdentifiedIndex = false;
    bool hasOriginalVisibility = false;
    winrt::Microsoft::UI::Xaml::Visibility originalVisibility =
        winrt::Microsoft::UI::Xaml::Visibility::Visible;
    bool hasOriginalOverflow = false;
    winrt::Microsoft::UI::Xaml::Controls::CommandBarOverflowButtonVisibility
        originalOverflow = winrt::Microsoft::UI::Xaml::Controls::
            CommandBarOverflowButtonVisibility::Auto;
    bool watchedVisibility = false;
    int64_t visibilityToken = 0;

    bool hasOriginalLabel = false;
    winrt::hstring originalLabel;
    winrt::hstring originalAutomationName;
    bool hasOriginalLabelPosition = false;
    winrt::Microsoft::UI::Xaml::Controls::CommandBarLabelPosition
        originalLabelPosition = winrt::Microsoft::UI::Xaml::Controls::
            CommandBarLabelPosition::Default;
    bool hasOriginalIsCompact = false;
    bool originalIsCompact = false;
    bool watchedLabel = false;
    int64_t labelToken = 0;
    int64_t labelPositionToken = 0;
    int64_t isCompactToken = 0;
};

// Categories of built-in command bar elements managed by hideDefaultButtons
// settings
enum class ManagedKind {
    Button,
    SeparatorAfter,
    GroupSeparator,
    OverflowElement,
};

// Entry in a separator group tracking neighboring built-in buttons
struct GroupMember {
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::UIElement> element;
    int defaultButtonIndex;
};

// Descriptor specifying why a built-in command bar element's visibility is
// managed
struct ManagedTarget {
    ManagedKind kind;
    int index = 0;
    std::shared_ptr<std::vector<GroupMember>> group;
};

static thread_local std::vector<ManagedElement> g_managedElements;
static thread_local int g_settingVisibilityDepth = 0;
static thread_local int g_settingLabelDepth = 0;

// Find an existing ManagedElement entry for a XAML UIElement on the current UI
// thread
static ManagedElement* FindManagedElement(
    winrt::Microsoft::UI::Xaml::UIElement const& element) {
    for (auto& entry : g_managedElements) {
        if (entry.element.get() == element) {
            return &entry;
        }
    }
    return nullptr;
}

// Retrieve or allocate a ManagedElement entry while pruning expired weak
// references
static ManagedElement& GetManagedElement(
    winrt::Microsoft::UI::Xaml::UIElement const& element) {
    std::erase_if(g_managedElements, [](ManagedElement const& entry) {
        return !entry.element.get();
    });

    if (auto* entry = FindManagedElement(element)) {
        return *entry;
    }

    g_managedElements.push_back({winrt::make_weak(element)});
    return g_managedElements.back();
}

// Extract the SVG or Bitmap URI string from a built-in AppBarButton's Icon
// property
static std::wstring GetButtonIconUri(
    winrt::Microsoft::UI::Xaml::Controls::AppBarButton const& button) try {
    auto icon = button.Icon();
    if (!icon) {
        return std::wstring();
    }

    winrt::Windows::Foundation::Uri uri{nullptr};
    if (auto imageIcon =
            icon.try_as<winrt::Microsoft::UI::Xaml::Controls::ImageIcon>()) {
        if (auto source = imageIcon.Source()) {
            if (auto svgSource =
                    source.try_as<winrt::Microsoft::UI::Xaml::Media::Imaging::
                                      SvgImageSource>()) {
                uri = svgSource.UriSource();
            }
        }
    } else if (auto iconSourceElement =
                   icon.try_as<winrt::Microsoft::UI::Xaml::Controls::
                                   IconSourceElement>()) {
        if (auto iconSource = iconSourceElement.IconSource()) {
            if (auto imageIconSource =
                    iconSource.try_as<winrt::Microsoft::UI::Xaml::Controls::
                                          ImageIconSource>()) {
                if (auto source = imageIconSource.ImageSource()) {
                    if (auto svgSource =
                            source.try_as<winrt::Microsoft::UI::Xaml::Media::
                                              Imaging::SvgImageSource>()) {
                        uri = svgSource.UriSource();
                    }
                }
            }
        }
    } else if (auto bitmapIcon =
                   icon.try_as<
                       winrt::Microsoft::UI::Xaml::Controls::BitmapIcon>()) {
        uri = bitmapIcon.UriSource();
    }

    if (!uri) {
        return std::wstring();
    }
    return std::wstring{uri.AbsoluteUri()};
} catch (...) {
    return std::wstring();
}

// Read the UI Automation AutomationId string attached to a FrameworkElement
static std::wstring GetAutomationId(
    winrt::Microsoft::UI::Xaml::FrameworkElement const& element) {
    try {
        return std::wstring{winrt::Microsoft::UI::Xaml::Automation::
                                AutomationProperties::GetAutomationId(element)};
    } catch (...) {
        return std::wstring();
    }
}

// Identify which kDefaultButtons index a built-in Explorer AppBarButton
// corresponds to, caching the result so custom labels never erase identity
static int IdentifyDefaultButton(
    winrt::Microsoft::UI::Xaml::Controls::AppBarButton const& button) {
    if (auto* managed = FindManagedElement(button);
        managed && managed->hasIdentifiedIndex) {
        return managed->identifiedIndex;
    }

    auto cacheAndReturn = [&](int idx) -> int {
        if (idx >= 0) {
            auto& entry = GetManagedElement(button);
            entry.identifiedIndex = idx;
            entry.hasIdentifiedIndex = true;
        }
        return idx;
    };

    std::wstring uri = ToLower(GetButtonIconUri(button));
    std::wstring autoId = GetAutomationId(button);

    std::wstring nameStr;
    try {
        nameStr = std::wstring(button.Name());
    } catch (...) {
    }

    std::wstring labelStr;
    try {
        labelStr = std::wstring(button.Label());
    } catch (...) {
    }

    std::wstring accessName;
    try {
        accessName = std::wstring(winrt::Microsoft::UI::Xaml::Automation::
                                      AutomationProperties::GetName(button));
    } catch (...) {
    }

    if (!uri.empty()) {
        size_t slash = uri.find_last_of(L'/');
        std::wstring_view fileName =
            slash == std::wstring::npos
                ? std::wstring_view(uri)
                : std::wstring_view(uri).substr(slash + 1);
        if (fileName.starts_with(L"windows.iconsize.")) {
            return cacheAndReturn(kViewButtonIndex);
        }
        if (fileName.find(L"emptyrecyclebin") != std::wstring_view::npos) {
            return cacheAndReturn(16);
        }
        if (fileName.find(L"restoreall") != std::wstring_view::npos) {
            return cacheAndReturn(15);
        }
        if (fileName.find(L"undelete") != std::wstring_view::npos ||
            fileName.find(L"restore") != std::wstring_view::npos) {
            return cacheAndReturn(14);
        }

        for (int i = 0; i < kDefaultButtonCount; i++) {
            if (kDefaultButtons[i].svgFileName &&
                fileName == kDefaultButtons[i].svgFileName) {
                return cacheAndReturn(i);
            }
        }
    }

    for (int i = 0; i < kDefaultButtonCount; i++) {
        if (kDefaultButtons[i].automationId) {
            std::wstring target = kDefaultButtons[i].automationId;
            if ((!autoId.empty() &&
                 autoId.find(target) != std::wstring::npos) ||
                (!nameStr.empty() &&
                 nameStr.find(target) != std::wstring::npos)) {
                return cacheAndReturn(i);
            }
        }
    }

    auto hasPhrase = [](const std::wstring& str, const std::wstring& word1,
                        const std::wstring& word2) {
        std::wstring lower = ToLower(str);
        return lower.find(word1) != std::wstring::npos &&
               lower.find(word2) != std::wstring::npos;
    };

    if (hasPhrase(labelStr, L"empty", L"recycle") ||
        hasPhrase(accessName, L"empty", L"recycle") ||
        hasPhrase(labelStr, L"empty", L"bin") ||
        hasPhrase(accessName, L"empty", L"bin")) {
        return cacheAndReturn(16);
    }

    if (hasPhrase(labelStr, L"restore", L"selected") ||
        hasPhrase(accessName, L"restore", L"selected")) {
        return cacheAndReturn(14);
    }

    if (hasPhrase(labelStr, L"restore", L"all") ||
        hasPhrase(accessName, L"restore", L"all")) {
        return cacheAndReturn(15);
    }

    return -1;
}

// Apply user-configured text label overrides to built-in Explorer buttons and
// track all 3 property tokens
static void ApplyCustomLabel(
    winrt::Microsoft::UI::Xaml::Controls::AppBarButton const& button,
    int defaultButtonIndex) {
    if (defaultButtonIndex < 0 || defaultButtonIndex >= kDefaultButtonCount) {
        return;
    }

    std::wstring overrideLabel;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        overrideLabel = g_settings.customLabels[defaultButtonIndex];
    }

    if (overrideLabel.empty()) {
        return;
    }

    auto& entry = GetManagedElement(button);
    entry.identifiedIndex = defaultButtonIndex;
    entry.hasIdentifiedIndex = true;

    // Save original label, automation name, label position, and compact state
    // once before overriding
    if (!entry.hasOriginalLabel) {
        winrt::hstring currentLabel = button.Label();
        if (currentLabel != overrideLabel) {
            entry.originalLabel = currentLabel;
            try {
                entry.originalAutomationName = winrt::Microsoft::UI::Xaml::
                    Automation::AutomationProperties::GetName(button);
            } catch (...) {
            }
            entry.hasOriginalLabel = true;
        }
    }
    if (!entry.hasOriginalLabelPosition) {
        entry.originalLabelPosition = button.LabelPosition();
        entry.hasOriginalLabelPosition = true;
    }
    if (!entry.hasOriginalIsCompact) {
        entry.originalIsCompact = button.IsCompact();
        entry.hasOriginalIsCompact = true;
    }

    // Register all three DependencyProperty callbacks with UiCallScope and
    // store every token for clean unload
    if (!entry.watchedLabel) {
        entry.labelToken = button.RegisterPropertyChangedCallback(
            winrt::Microsoft::UI::Xaml::Controls::AppBarButton::LabelProperty(),
            [defaultButtonIndex](
                winrt::Microsoft::UI::Xaml::DependencyObject const& sender,
                winrt::Microsoft::UI::Xaml::DependencyProperty const&) {
                UiCallScope scope;
                if (g_unloading || g_settingLabelDepth > 0) {
                    return;
                }
                if (auto btn = sender.try_as<winrt::Microsoft::UI::Xaml::
                                                 Controls::AppBarButton>()) {
                    ApplyCustomLabel(btn, defaultButtonIndex);
                }
            });

        entry.labelPositionToken = button.RegisterPropertyChangedCallback(
            winrt::Microsoft::UI::Xaml::Controls::AppBarButton::
                LabelPositionProperty(),
            [](winrt::Microsoft::UI::Xaml::DependencyObject const& sender,
               winrt::Microsoft::UI::Xaml::DependencyProperty const&) {
                UiCallScope scope;
                if (g_unloading || g_settingLabelDepth > 0) {
                    return;
                }
                if (auto btn = sender.try_as<winrt::Microsoft::UI::Xaml::
                                                 Controls::AppBarButton>()) {
                    if (btn.LabelPosition() !=
                        winrt::Microsoft::UI::Xaml::Controls::
                            CommandBarLabelPosition::Default) {
                        g_settingLabelDepth++;
                        try {
                            btn.LabelPosition(
                                winrt::Microsoft::UI::Xaml::Controls::
                                    CommandBarLabelPosition::Default);
                        } catch (...) {
                        }
                        g_settingLabelDepth--;
                    }
                }
            });

        entry.isCompactToken = button.RegisterPropertyChangedCallback(
            winrt::Microsoft::UI::Xaml::Controls::AppBarButton::
                IsCompactProperty(),
            [](winrt::Microsoft::UI::Xaml::DependencyObject const& sender,
               winrt::Microsoft::UI::Xaml::DependencyProperty const&) {
                UiCallScope scope;
                if (g_unloading || g_settingLabelDepth > 0) {
                    return;
                }
                if (auto btn = sender.try_as<winrt::Microsoft::UI::Xaml::
                                                 Controls::AppBarButton>()) {
                    if (btn.IsCompact()) {
                        g_settingLabelDepth++;
                        try {
                            btn.IsCompact(false);
                        } catch (...) {
                        }
                        g_settingLabelDepth--;
                    }
                }
            });

        entry.watchedLabel = true;
    }

    if (button.Label() != overrideLabel) {
        g_settingLabelDepth++;
        try {
            button.Label(overrideLabel);
            winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::
                SetName(button, overrideLabel);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
        g_settingLabelDepth--;
    }

    if (button.LabelPosition() != winrt::Microsoft::UI::Xaml::Controls::
                                      CommandBarLabelPosition::Default) {
        g_settingLabelDepth++;
        try {
            button.LabelPosition(winrt::Microsoft::UI::Xaml::Controls::
                                     CommandBarLabelPosition::Default);
        } catch (...) {
        }
        g_settingLabelDepth--;
    }

    if (button.IsCompact()) {
        g_settingLabelDepth++;
        try {
            button.IsCompact(false);
        } catch (...) {
        }
        g_settingLabelDepth--;
    }
}

// Query Explorer's underlying intended visibility for a managed element
static winrt::Microsoft::UI::Xaml::Visibility EffectiveVisibility(
    winrt::Microsoft::UI::Xaml::UIElement const& element) {
    auto* entry = FindManagedElement(element);
    if (entry && entry->hasOriginalVisibility) {
        return entry->originalVisibility;
    }
    return element.Visibility();
}

static bool ShouldHide(ManagedTarget const& target);

// Determine whether all built-in buttons following a group separator are
// currently hidden
static bool IsGroupHidden(
    std::shared_ptr<std::vector<GroupMember>> const& group) {
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
        if (EffectiveVisibility(element) ==
            winrt::Microsoft::UI::Xaml::Visibility::Collapsed) {
            continue;
        }
        return false;
    }
    return true;
}

// Evaluate whether a built-in button, separator, or overflow element should be
// collapsed
static bool ShouldHide(ManagedTarget const& target) {
    if (target.kind == ManagedKind::GroupSeparator) {
        return IsGroupHidden(target.group);
    }

    std::lock_guard<std::mutex> lock(g_settings.mutex);
    switch (target.kind) {
        case ManagedKind::Button:
            return g_settings.hideDefaultButtons[target.index];
        case ManagedKind::SeparatorAfter:
            return g_settings.hideSeparatorAfterButton[target.index];
        case ManagedKind::OverflowElement:
            return g_settings.hideMoreButton;
        case ManagedKind::GroupSeparator:
            break;
    }
    return false;
}

// Mutate a UIElement's Visibility while suppressing self-triggered visibility
// property callbacks
static void SetVisibilityInternal(
    winrt::Microsoft::UI::Xaml::UIElement const& element,
    winrt::Microsoft::UI::Xaml::Visibility visibility) {
    g_settingVisibilityDepth++;
    try {
        element.Visibility(visibility);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
    g_settingVisibilityDepth--;
}

// Subscribe to VisibilityProperty changes so Explorer cannot un-hide buttons
// suppressed by user settings
static void WatchVisibility(
    winrt::Microsoft::UI::Xaml::UIElement const& element,
    ManagedTarget const& target,
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::CommandBar> const&
        owner) {
    int64_t token = element.RegisterPropertyChangedCallback(
        winrt::Microsoft::UI::Xaml::UIElement::VisibilityProperty(),
        [target, owner](
            winrt::Microsoft::UI::Xaml::DependencyObject const& sender,
            winrt::Microsoft::UI::Xaml::DependencyProperty const&) {
            UiCallScope scope;
            if (g_unloading || g_settingVisibilityDepth > 0) {
                return;
            }
            auto uiElement =
                sender.try_as<winrt::Microsoft::UI::Xaml::UIElement>();
            if (!uiElement) {
                return;
            }

            winrt::Microsoft::UI::Xaml::Visibility visibility =
                uiElement.Visibility();
            {
                auto& entry = GetManagedElement(uiElement);
                entry.originalVisibility = visibility;
                entry.hasOriginalVisibility = true;
            }

            if (target.kind != ManagedKind::GroupSeparator &&
                visibility !=
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed &&
                ShouldHide(target)) {
                SetVisibilityInternal(
                    uiElement,
                    winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
            }
            QueueCommandBarUpdate(owner, false);
        });

    auto& entry = GetManagedElement(element);
    entry.watchedVisibility = true;
    entry.visibilityToken = token;
}

// Apply or restore managed visibility on a built-in UIElement
static void SetManagedVisibility(
    winrt::Microsoft::UI::Xaml::UIElement const& element,
    ManagedTarget const& target,
    bool forceShow,
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::CommandBar> const&
        owner) {
    winrt::Microsoft::UI::Xaml::Visibility original;
    bool watch;
    {
        auto& entry = GetManagedElement(element);
        if (!entry.hasOriginalVisibility) {
            entry.originalVisibility = element.Visibility();
            entry.hasOriginalVisibility = true;
        }
        original = entry.originalVisibility;
        watch = !forceShow && !entry.watchedVisibility;
    }

    if (watch) {
        WatchVisibility(element, target, owner);
    }
    SetVisibilityInternal(
        element, !forceShow && ShouldHide(target)
                     ? winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                     : original);
}

// Recursively locate a named child FrameworkElement inside a CommandBar's XAML
// template
static winrt::Microsoft::UI::Xaml::FrameworkElement FindDescendantByName(
    winrt::Microsoft::UI::Xaml::DependencyObject const& root,
    std::wstring_view name) {
    int count =
        winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(
            root);
    for (int i = 0; i < count; i++) {
        auto child =
            winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChild(root,
                                                                          i);
        if (auto element =
                child.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
            element && element.Name() == name) {
            return element;
        }
        if (auto found = FindDescendantByName(child, name)) {
            return found;
        }
    }
    return nullptr;
}

// Scan a CommandBar's PrimaryCommands to enforce built-in button hiding,
// separator hiding, and custom labels
static void ApplyDefaultButtonVisibility(
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar,
    bool forceShow = false) {
    bool hideMore;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        hideMore = g_settings.hideMoreButton;
    }

    bool isPrimary = commandBar.Name() == L"FileExplorerCommandBar";
    auto weakCommandBar = winrt::make_weak(commandBar);
    auto setVisibility =
        [&weakCommandBar, forceShow](
            winrt::Microsoft::UI::Xaml::UIElement const& element,
            ManagedTarget const& target) {
            SetManagedVisibility(element, target, forceShow, weakCommandBar);
        };

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    struct ScannedEntry {
        winrt::Microsoft::UI::Xaml::Controls::ICommandBarElement command{
            nullptr};
        bool isOurs = false;
        bool isSeparator = false;
        bool isDetailsToggle = false;
        int defaultIndex = -1;
    };

    std::vector<ScannedEntry> entries;
    entries.reserve(count);

    for (uint32_t i = 0; i < count; i++) {
        ScannedEntry entry;
        entry.command = commands.GetAt(i);
        entry.isOurs = IsOurElement(entry.command);
        entry.isSeparator =
            !entry.isOurs &&
            static_cast<bool>(
                entry.command.try_as<
                    winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator>());

        if (!entry.isOurs && !entry.isSeparator) {
            if (auto button =
                    entry.command.try_as<
                        winrt::Microsoft::UI::Xaml::Controls::AppBarButton>()) {
                int defIdx = IdentifyDefaultButton(button);
                if (defIdx == kDetailsButtonIndex ||
                    GetAutomationId(button) == L"DetailsPaneToggleButton") {
                    entry.isDetailsToggle = true;
                    entry.defaultIndex = kDetailsButtonIndex;
                } else {
                    entry.defaultIndex = defIdx;
                }
            }
        }
        entries.push_back(std::move(entry));
    }

    uint32_t firstCustomIndex = count;
    for (uint32_t i = 0; i < count; i++) {
        if (entries[i].isOurs) {
            firstCustomIndex = i;
            break;
        }
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
            if (entry.isSeparator || entry.isOurs) {
                break;
            }
            if (auto element =
                    entry.command
                        .try_as<winrt::Microsoft::UI::Xaml::UIElement>()) {
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
            prevIndex = -1;
            continue;
        }

        if (entry.isSeparator) {
            auto separator = entry.command.as<
                winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator>();
            int target = -1;
            if (prevIndex == kNewButtonIndex) {
                target = kNewButtonIndex;
            } else if (prevIndex == kDeleteButtonIndex) {
                target = kDeleteButtonIndex;
            } else if (i == viewSeparatorIndex) {
                target = kViewButtonIndex;
            }

            if (target >= 0) {
                setVisibility(separator, {ManagedKind::SeparatorAfter, target});
            } else {
                setVisibility(separator, {ManagedKind::GroupSeparator, -1,
                                          collectGroup(i)});
            }
            prevIndex = -1;
            continue;
        }

        if (entry.isDetailsToggle) {
            auto button =
                entry.command
                    .as<winrt::Microsoft::UI::Xaml::Controls::AppBarButton>();
            setVisibility(button, {ManagedKind::Button, kDetailsButtonIndex});
            if (!forceShow) {
                ApplyCustomLabel(button, kDetailsButtonIndex);
            }
            prevIndex = -1;
            continue;
        }

        if (entry.defaultIndex >= 0) {
            auto button =
                entry.command
                    .as<winrt::Microsoft::UI::Xaml::Controls::AppBarButton>();
            if (forceShow) {
                if (auto* managed = FindManagedElement(button);
                    managed && managed->hasOriginalVisibility) {
                    SetVisibilityInternal(button, managed->originalVisibility);
                }
            } else if (ShouldHide({ManagedKind::Button, entry.defaultIndex})) {
                setVisibility(button,
                              {ManagedKind::Button, entry.defaultIndex});
            }
            if (!forceShow) {
                ApplyCustomLabel(button, entry.defaultIndex);
            }
        }

        prevIndex = entry.defaultIndex;
    }

    if (isPrimary) {
        winrt::Microsoft::UI::Xaml::Controls::CommandBarOverflowButtonVisibility
            originalOverflow;
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
                ? winrt::Microsoft::UI::Xaml::Controls::
                      CommandBarOverflowButtonVisibility::Collapsed
                : originalOverflow);
        if (auto overflowSeparator =
                FindDescendantByName(commandBar, L"OverflowSeparator")) {
            setVisibility(overflowSeparator, {ManagedKind::OverflowElement});
        }
    }
}

// Unregister all DependencyProperty callbacks and restore modified system
// labels/states on current thread
static void UnwatchManagedElementsForCurrentThread() {
    for (auto& entry : g_managedElements) {
        auto element = entry.element.get();

        if (entry.watchedVisibility) {
            int64_t token = entry.visibilityToken;
            entry.watchedVisibility = false;
            entry.visibilityToken = 0;
            if (element) {
                try {
                    element.UnregisterPropertyChangedCallback(
                        winrt::Microsoft::UI::Xaml::UIElement::
                            VisibilityProperty(),
                        token);
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        }

        if (entry.watchedLabel) {
            int64_t lblToken = entry.labelToken;
            int64_t posToken = entry.labelPositionToken;
            int64_t cmpToken = entry.isCompactToken;
            entry.watchedLabel = false;
            entry.labelToken = 0;
            entry.labelPositionToken = 0;
            entry.isCompactToken = 0;
            if (element) {
                if (auto button =
                        element.try_as<winrt::Microsoft::UI::Xaml::Controls::
                                           AppBarButton>()) {
                    try {
                        button.UnregisterPropertyChangedCallback(
                            winrt::Microsoft::UI::Xaml::Controls::AppBarButton::
                                LabelProperty(),
                            lblToken);
                    } catch (...) {
                    }
                    try {
                        button.UnregisterPropertyChangedCallback(
                            winrt::Microsoft::UI::Xaml::Controls::AppBarButton::
                                LabelPositionProperty(),
                            posToken);
                    } catch (...) {
                    }
                    try {
                        button.UnregisterPropertyChangedCallback(
                            winrt::Microsoft::UI::Xaml::Controls::AppBarButton::
                                IsCompactProperty(),
                            cmpToken);
                    } catch (...) {
                    }
                }
            }
        }

        if (element) {
            if (auto button =
                    element.try_as<
                        winrt::Microsoft::UI::Xaml::Controls::AppBarButton>()) {
                g_settingLabelDepth++;
                if (entry.hasOriginalLabel) {
                    try {
                        button.Label(entry.originalLabel);
                        winrt::Microsoft::UI::Xaml::Automation::
                            AutomationProperties::SetName(
                                button, entry.originalAutomationName);
                    } catch (...) {
                    }
                    entry.hasOriginalLabel = false;
                }
                if (entry.hasOriginalLabelPosition) {
                    try {
                        button.LabelPosition(entry.originalLabelPosition);
                    } catch (...) {
                    }
                    entry.hasOriginalLabelPosition = false;
                }
                if (entry.hasOriginalIsCompact) {
                    try {
                        button.IsCompact(entry.originalIsCompact);
                    } catch (...) {
                    }
                    entry.hasOriginalIsCompact = false;
                }
                g_settingLabelDepth--;
            }
        }
    }
}

// Clear all tracked built-in element entries on the current thread
static void ForgetManagedElementsForCurrentThread() {
    g_managedElements.clear();
}

// ============================================================================
// Custom Toolbar Buttons, Dropdown Menus & Dynamic Selection Coordination
// ============================================================================

// Check whether a CommandBar element was created by this mod
static bool IsOurElement(
    winrt::Microsoft::UI::Xaml::Controls::ICommandBarElement const& command) {
    auto element =
        command.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
    if (!element) {
        return false;
    }
    std::wstring_view name{element.Name()};
    return name.starts_with(kButtonNamePrefix);
}

// Check whether a CommandBar already contains our custom toolbar elements
static bool HasOurElements(
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar) {
    for (auto const& command : commandBar.PrimaryCommands()) {
        if (IsOurElement(command)) {
            return true;
        }
    }
    return false;
}

// Timer entry managing delayed hover-open flyout activation on dropdown buttons
struct HoverTimerEntry {
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::AppBarButton> button;
    winrt::Microsoft::UI::Xaml::DispatcherTimer timer{nullptr};
    winrt::event_token tickToken{};
};

static thread_local std::vector<HoverTimerEntry> g_hoverTimers;

// Stop and detach a hover timer's Tick event handler safely
static void ReleaseHoverTimer(HoverTimerEntry const& entry) {
    if (!entry.timer) {
        return;
    }
    try {
        entry.timer.Stop();
        entry.timer.Tick(entry.tickToken);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

// Record a hover DispatcherTimer while cleaning up timers for destroyed buttons
static void TrackHoverTimer(
    winrt::Microsoft::UI::Xaml::Controls::AppBarButton const& button,
    winrt::Microsoft::UI::Xaml::DispatcherTimer const& timer,
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

// Stop and release all dropdown hover timers on the current UI thread
static void StopHoverTimersForCurrentThread() {
    std::vector<HoverTimerEntry> taken;
    taken.swap(g_hoverTimers);
    for (auto const& entry : taken) {
        ReleaseHoverTimer(entry);
    }
}

// Attach unload-safe pointer hover handlers to automatically open a menu
// button's flyout
static void SetUpOpenOnHover(
    winrt::Microsoft::UI::Xaml::Controls::AppBarButton const& button,
    int hoverDelayMs,
    std::function<void()> const& open) {
    if (hoverDelayMs <= 0) {
        TrackRevoker(button,
                     button.PointerEntered(
                         winrt::auto_revoke,
                         [open](winrt::Windows::Foundation::IInspectable const&,
                                winrt::Microsoft::UI::Xaml::Input::
                                    PointerRoutedEventArgs const&) {
                             UiCallScope scope;
                             if (!g_unloading) {
                                 open();
                             }
                         }));
        return;
    }

    winrt::Microsoft::UI::Xaml::DispatcherTimer timer;
    timer.Interval(std::chrono::milliseconds(hoverDelayMs));

    auto tickToken = timer.Tick(
        [open](winrt::Windows::Foundation::IInspectable const& sender,
               winrt::Windows::Foundation::IInspectable const&) {
            UiCallScope scope;
            if (auto t = sender.try_as<
                         winrt::Microsoft::UI::Xaml::DispatcherTimer>()) {
                t.Stop();
            }
            if (!g_unloading) {
                open();
            }
        });

    TrackHoverTimer(button, timer, tickToken);

    TrackRevoker(button,
                 button.PointerEntered(
                     winrt::auto_revoke,
                     [timer](winrt::Windows::Foundation::IInspectable const&,
                             winrt::Microsoft::UI::Xaml::Input::
                                 PointerRoutedEventArgs const&) {
                         UiCallScope scope;
                         if (!g_unloading) {
                             timer.Stop();
                             timer.Start();
                         }
                     }));

    auto stopTimer =
        [timer](
            winrt::Windows::Foundation::IInspectable const&,
            winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const&) {
            UiCallScope scope;
            if (!g_unloading) {
                timer.Stop();
            }
        };
    TrackRevoker(button, button.PointerExited(winrt::auto_revoke, stopTimer));
    TrackRevoker(button, button.PointerCanceled(winrt::auto_revoke, stopTimer));
}

// Create a weak-referenced callback that opens an AppBarButton's Flyout if not
// already open
static std::function<void()> MakeShowFlyoutAction(
    winrt::Microsoft::UI::Xaml::Controls::AppBarButton const& button) {
    return [weakButton = winrt::make_weak(button)]() {
        UiCallScope scope;
        auto btn = weakButton.get();
        if (!btn || g_unloading) {
            return;
        }
        if (auto flyout = btn.Flyout(); flyout && !flyout.IsOpen()) {
            flyout.ShowAt(btn);
        }
    };
}

// Instantiate a tagged AppBarButton with tooltip, icon, and label visibility
// configuration
static winrt::Microsoft::UI::Xaml::Controls::AppBarButton CreateBareButton(
    int index,
    std::wstring const& tooltip,
    std::wstring const& labelText,
    winrt::Microsoft::UI::Xaml::Controls::IconElement const& icon,
    bool showLabel = false) {
    std::wstring name = kButtonNamePrefix;
    name += L'_';
    name += std::to_wstring(index);

    winrt::Microsoft::UI::Xaml::Controls::AppBarButton button;
    button.Name(name.c_str());

    std::wstring displayLabel = labelText.empty() ? tooltip : labelText;
    button.Label(displayLabel.c_str());
    button.LabelPosition(showLabel ? winrt::Microsoft::UI::Xaml::Controls::
                                         CommandBarLabelPosition::Default
                                   : winrt::Microsoft::UI::Xaml::Controls::
                                         CommandBarLabelPosition::Collapsed);
    button.Icon(icon);

    if (!tooltip.empty()) {
        winrt::Microsoft::UI::Xaml::Controls::ToolTipService::SetToolTip(
            button, winrt::box_value(winrt::hstring{tooltip}));
    }

    return button;
}

// Create a clickable single-action toolbar AppBarButton
static winrt::Microsoft::UI::Xaml::Controls::AppBarButton CreateActionButton(
    ActionItem const& item,
    int index,
    winrt::Microsoft::UI::Xaml::ElementTheme theme) {
    std::wstring displayLabel = item.GetDisplayLabel();
    ExplorerContext emptyCtx;
    std::wstring fullResolved = (displayLabel.find(L'%') != std::wstring::npos)
                                    ? ResolveLabelTokens(displayLabel, emptyCtx)
                                    : displayLabel;
    std::wstring displayResolved =
        TruncateWithEllipsis(fullResolved, kMaxToolbarLabelLength);

    winrt::Microsoft::UI::Xaml::Controls::AppBarButton button =
        CreateBareButton(index, fullResolved, displayResolved,
                         MakeCommandButtonIcon(item, theme), item.showLabel);

    TrackRevoker(
        button,
        button.Click(
            winrt::auto_revoke,
            [item](winrt::Windows::Foundation::IInspectable const& sender,
                   winrt::Microsoft::UI::Xaml::RoutedEventArgs const&) {
                UiCallScope scope;
                if (g_unloading) {
                    return;
                }
                if (auto element =
                        sender.try_as<
                            winrt::Microsoft::UI::Xaml::FrameworkElement>()) {
                    OnActionInvoked(element, item);
                }
            }));

    return button;
}

static void AppendMenuEntries(
    std::vector<ActionItem> const& items,
    winrt::Windows::Foundation::Collections::IVector<
        winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase> const& target,
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::AppBarButton> const&
        weakButton,
    winrt::Microsoft::UI::Xaml::ElementTheme theme);

// Build a MenuFlyoutItem or nested MenuFlyoutSubItem for a dropdown menu entry
static winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase CreateMenuEntry(
    ActionItem const& item,
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::AppBarButton> const&
        weakButton,
    winrt::Microsoft::UI::Xaml::ElementTheme theme) {
    std::wstring displayLabel = item.GetDisplayLabel();

    if (item.isMenu && !item.subItems.empty()) {
        winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem subMenu;
        subMenu.Text(displayLabel.c_str());
        if (!item.hideIcon) {
            if (auto icon =
                    TryCreateIconElement(item.icon, item.command, theme)) {
                subMenu.Icon(icon);
            }
        }
        AppendMenuEntries(item.subItems, subMenu.Items(), weakButton, theme);
        return subMenu;
    }

    winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem menuItem;
    menuItem.Text(displayLabel.c_str());
    if (!item.hideIcon) {
        if (auto icon = TryCreateIconElement(item.icon, item.command, theme)) {
            menuItem.Icon(icon);
        }
    }

    TrackRevoker(
        menuItem,
        menuItem.Click(winrt::auto_revoke,
                       [item, weakButton](
                           winrt::Windows::Foundation::IInspectable const&,
                           winrt::Microsoft::UI::Xaml::RoutedEventArgs const&) {
                           UiCallScope scope;
                           if (g_unloading) {
                               return;
                           }
                           if (auto button = weakButton.get()) {
                               OnActionInvoked(button, item);
                           }
                       }));

    return menuItem;
}

// Populate a MenuFlyout vector with items and optional surrounding horizontal
// separators
static void AppendMenuEntries(
    std::vector<ActionItem> const& items,
    winrt::Windows::Foundation::Collections::IVector<
        winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase> const& target,
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::AppBarButton> const&
        weakButton,
    winrt::Microsoft::UI::Xaml::ElementTheme theme) {
    for (auto const& item : items) {
        if (item.separatorBefore) {
            target.Append(
                winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSeparator());
        }
        target.Append(CreateMenuEntry(item, weakButton, theme));
        if (item.separatorAfter) {
            target.Append(
                winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSeparator());
        }
    }
}

// Check recursively whether any item in a menu subtree embeds `%` label tokens
static bool SubTreeHasTokens(std::vector<ActionItem> const& items) {
    for (const auto& sub : items) {
        if (sub.GetDisplayLabel().find(L'%') != std::wstring::npos ||
            SubTreeHasTokens(sub.subItems)) {
            return true;
        }
    }
    return false;
}

// Resolve dynamic label tokens and clamp lengths recursively across an entire
// menu subtree prior to flyout display
static void ResolveSubTreeLabels(std::vector<ActionItem>& items,
                                 ExplorerContext const& ctx) {
    for (auto& entry : items) {
        std::wstring label = entry.GetDisplayLabel();
        std::wstring resolved = (label.find(L'%') != std::wstring::npos)
                                    ? ResolveLabelTokens(label, ctx)
                                    : label;
        entry.labelText =
            TruncateWithEllipsis(resolved, kMaxMenuItemLabelLength);
        if (!entry.subItems.empty()) {
            ResolveSubTreeLabels(entry.subItems, ctx);
        }
    }
}

// Create a dropdown menu toolbar button that lazily populates and updates
// sub-items on flyout open
static winrt::Microsoft::UI::Xaml::Controls::AppBarButton CreateMenuButton(
    ActionItem const& item,
    int index,
    bool openOnHover,
    int hoverDelayMs,
    winrt::Microsoft::UI::Xaml::ElementTheme theme) {
    static constexpr const WCHAR* kMenuArrow = L"\u25BE";

    std::wstring displayLabel = item.GetDisplayLabel();
    ExplorerContext emptyCtx;
    std::wstring fullResolved = (displayLabel.find(L'%') != std::wstring::npos)
                                    ? ResolveLabelTokens(displayLabel, emptyCtx)
                                    : displayLabel;

    std::wstring truncatedDisplay =
        TruncateWithEllipsis(fullResolved, kMaxToolbarLabelLength);

    std::wstring finalLabel = (item.showLabel && !truncatedDisplay.empty())
                                  ? (truncatedDisplay + L" " + kMenuArrow)
                                  : std::wstring(kMenuArrow);

    winrt::Microsoft::UI::Xaml::Controls::AppBarButton button =
        CreateBareButton(index, fullResolved, finalLabel,
                         MakeCommandButtonIcon(item, theme), true);

    auto weakButton = winrt::make_weak(button);

    winrt::Microsoft::UI::Xaml::Controls::MenuFlyout menu;
    menu.Placement(winrt::Microsoft::UI::Xaml::Controls::Primitives::
                       FlyoutPlacementMode::BottomEdgeAlignedLeft);

    bool hasChildTokens = SubTreeHasTokens(item.subItems);

    auto ensureMenuEntries =
        [subItems = item.subItems, weakButton, hasChildTokens](
            winrt::Microsoft::UI::Xaml::Controls::MenuFlyout const& flyout) {
            UiCallScope scope;
            if (g_unloading || !flyout) {
                return;
            }
            try {
                if (flyout.Items().Size() > 0 && !hasChildTokens) {
                    return;
                }

                winrt::Microsoft::UI::Xaml::ElementTheme currentTheme =
                    winrt::Microsoft::UI::Xaml::ElementTheme::Default;
                HWND hWnd = nullptr;
                if (auto btn = weakButton.get()) {
                    currentTheme = btn.ActualTheme();
                    hWnd = GetExplorerWindowForElement(btn);
                }

                ExplorerContext ctx;
                if (hasChildTokens && hWnd) {
                    if (auto sv = GetActiveShellView(hWnd)) {
                        ctx = GetLightweightContextFromView(sv.get());
                    }
                }

                flyout.Items().Clear();

                std::vector<ActionItem> resolvedList = subItems;
                ResolveSubTreeLabels(resolvedList, ctx);

                AppendMenuEntries(resolvedList, flyout.Items(), weakButton,
                                  currentTheme);
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
        };

    TrackRevoker(
        menu,
        menu.Opening(
            winrt::auto_revoke,
            [ensureMenuEntries](
                winrt::Windows::Foundation::IInspectable const& sender,
                winrt::Windows::Foundation::IInspectable const&) {
                ensureMenuEntries(
                    sender.try_as<
                        winrt::Microsoft::UI::Xaml::Controls::MenuFlyout>());
            }));

    button.Flyout(menu);
    if (openOnHover) {
        SetUpOpenOnHover(button, hoverDelayMs, MakeShowFlyoutAction(button));
    }
    return button;
}

// Inject configured custom buttons, menus, and separators into the target
// CommandBar
static void EnsureActionButtons(
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar) {
    if (g_unloading || HasOurElements(commandBar)) {
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
    winrt::Microsoft::UI::Xaml::ElementTheme theme = commandBar.ActualTheme();

    // Guard PrimaryCommands mutations to prevent self-triggering VectorChanged
    // update storms
    g_modifyingCommandsDepth++;
    try {
        auto insertSeparatorSafe = [&](uint32_t& index,
                                       const std::wstring& name) {
            if (index > 0 && commands.GetAt(index - 1)
                                 .try_as<winrt::Microsoft::UI::Xaml::Controls::
                                             AppBarSeparator>()) {
                return;
            }
            if (index < commands.Size() &&
                commands.GetAt(index)
                    .try_as<winrt::Microsoft::UI::Xaml::Controls::
                                AppBarSeparator>()) {
                return;
            }
            winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator separator;
            separator.Name(name.c_str());
            commands.InsertAt(index++, separator);
        };

        auto appendSeparatorSafe = [&](const std::wstring& name) {
            uint32_t count = commands.Size();
            if (count == 0 || commands.GetAt(count - 1)
                                  .try_as<winrt::Microsoft::UI::Xaml::Controls::
                                              AppBarSeparator>()) {
                return;
            }
            winrt::Microsoft::UI::Xaml::Controls::AppBarSeparator separator;
            separator.Name(name.c_str());
            commands.Append(separator);
        };

        if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
            uint32_t insertIndex = 0;
            for (size_t i = 0; i < items.size(); i++) {
                auto const& item = items[i];

                if (item.separatorBefore) {
                    insertSeparatorSafe(
                        insertIndex,
                        kButtonNamePrefix + L"_SepPre_" + std::to_wstring(i));
                }

                if (item.isMenu && !item.subItems.empty()) {
                    commands.InsertAt(
                        insertIndex++,
                        CreateMenuButton(item, static_cast<int>(i),
                                         openMenuOnHover, menuHoverDelay,
                                         theme));
                } else {
                    commands.InsertAt(
                        insertIndex++,
                        CreateActionButton(item, static_cast<int>(i), theme));
                }

                if (item.separatorAfter) {
                    insertSeparatorSafe(
                        insertIndex,
                        kButtonNamePrefix + L"_Sep_" + std::to_wstring(i));
                }
            }
        } else {
            for (size_t i = 0; i < items.size(); i++) {
                auto const& item = items[i];

                if (item.separatorBefore) {
                    appendSeparatorSafe(kButtonNamePrefix + L"_SepPre_" +
                                        std::to_wstring(i));
                }

                if (item.isMenu && !item.subItems.empty()) {
                    commands.Append(CreateMenuButton(item, static_cast<int>(i),
                                                     openMenuOnHover,
                                                     menuHoverDelay, theme));
                } else {
                    commands.Append(
                        CreateActionButton(item, static_cast<int>(i), theme));
                }

                if (item.separatorAfter) {
                    appendSeparatorSafe(kButtonNamePrefix + L"_Sep_" +
                                        std::to_wstring(i));
                }
            }
        }
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
    g_modifyingCommandsDepth--;
}

// Refresh custom button visibility, enabled state, separators, and dynamic
// labels in O(C) time without disk I/O
static void UpdateDynamicButtonStates(
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar,
    HWND hExplorerWnd,
    const ExplorerContext* optContext = nullptr) {
    if (g_unloading || !commandBar) {
        return;
    }

    if (!hExplorerWnd) {
        hExplorerWnd = GetExplorerWindowForElement(commandBar);
    }
    if (!hExplorerWnd) {
        return;
    }

    // Use lightweight zero-I/O context on the UI thread when no pre-built
    // context was supplied
    ExplorerContext localContext;
    if (!optContext) {
        if (auto sv = GetActiveShellView(hExplorerWnd)) {
            localContext = GetLightweightContextFromView(sv.get());
        }
    }
    const ExplorerContext& context = optContext ? *optContext : localContext;

    SelectionSummary summary = SummarizeSelection(context.shellView.get());

    struct ItemSnapshot {
        std::wstring rawLabel;
        std::wstring matchExtensions;
        bool isMenu;
        bool showLabel;
    };

    bool disabledInsteadOfHidden;
    std::vector<ItemSnapshot> itemSnapshots;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        disabledInsteadOfHidden = g_settings.disabledInsteadOfHidden;
        itemSnapshots.reserve(g_settings.items.size());
        for (const auto& itm : g_settings.items) {
            itemSnapshots.push_back({itm.GetDisplayLabel(), itm.matchExtensions,
                                     itm.isMenu, itm.showLabel});
        }
    }

    auto commands = commandBar.PrimaryCommands();
    uint32_t count = commands.Size();

    // Collect our custom buttons and separators in a single O(C) pass over
    // PrimaryCommands
    struct CustomButtonRef {
        winrt::Microsoft::UI::Xaml::Controls::AppBarButton button{nullptr};
        int itemIndex = -1;
    };
    std::vector<CustomButtonRef> ourButtons;
    std::unordered_map<std::wstring,
                       winrt::Microsoft::UI::Xaml::FrameworkElement>
        ourSeparators;

    for (uint32_t i = 0; i < count; i++) {
        auto cmd = commands.GetAt(i);
        auto element =
            cmd.try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>();
        if (!element) {
            continue;
        }

        std::wstring name{element.Name()};
        if (!name.starts_with(kButtonNamePrefix)) {
            continue;
        }

        if (auto button =
                cmd.try_as<
                    winrt::Microsoft::UI::Xaml::Controls::AppBarButton>()) {
            size_t underscore = name.rfind(L'_');
            int idx = -1;
            if (underscore != std::wstring::npos &&
                TryParseNonNegativeInt(
                    std::wstring_view(name).substr(underscore + 1), &idx)) {
                ourButtons.push_back({std::move(button), idx});
            }
        } else if (cmd.try_as<winrt::Microsoft::UI::Xaml::Controls::
                                  AppBarSeparator>()) {
            ourSeparators.emplace(std::move(name), std::move(element));
        }
    }

    for (const auto& btnRef : ourButtons) {
        if (btnRef.itemIndex < 0 ||
            static_cast<size_t>(btnRef.itemIndex) >= itemSnapshots.size()) {
            continue;
        }

        const auto& item = itemSnapshots[static_cast<size_t>(btnRef.itemIndex)];
        const auto& button = btnRef.button;
        bool isVisible =
            ItemMatchesExtensionFilter(item.matchExtensions, summary);

        if (item.rawLabel.find(L'%') != std::wstring::npos) {
            std::wstring fullResolved =
                ResolveLabelTokens(item.rawLabel, context);
            std::wstring displayResolved =
                TruncateWithEllipsis(fullResolved, kMaxToolbarLabelLength);

            if (item.isMenu) {
                if (item.showLabel && !displayResolved.empty()) {
                    displayResolved += L" \u25BE";
                } else {
                    displayResolved = L"\u25BE";
                }
                button.Label(displayResolved.c_str());
                button.LabelPosition(winrt::Microsoft::UI::Xaml::Controls::
                                         CommandBarLabelPosition::Default);
            } else {
                button.Label(displayResolved.c_str());
                button.LabelPosition(
                    item.showLabel ? winrt::Microsoft::UI::Xaml::Controls::
                                         CommandBarLabelPosition::Default
                                   : winrt::Microsoft::UI::Xaml::Controls::
                                         CommandBarLabelPosition::Collapsed);
            }
            winrt::Microsoft::UI::Xaml::Controls::ToolTipService::SetToolTip(
                button, winrt::box_value(winrt::hstring{fullResolved}));
        }

        if (disabledInsteadOfHidden) {
            button.Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
            button.IsEnabled(isVisible);
        } else {
            auto targetVis =
                isVisible ? winrt::Microsoft::UI::Xaml::Visibility::Visible
                          : winrt::Microsoft::UI::Xaml::Visibility::Collapsed;
            button.Visibility(targetVis);
            button.IsEnabled(isVisible);

            std::wstring sepAfter = kButtonNamePrefix + L"_Sep_" +
                                    std::to_wstring(btnRef.itemIndex);
            std::wstring sepBefore = kButtonNamePrefix + L"_SepPre_" +
                                     std::to_wstring(btnRef.itemIndex);

            if (auto it = ourSeparators.find(sepAfter);
                it != ourSeparators.end()) {
                it->second.Visibility(targetVis);
            }
            if (auto it = ourSeparators.find(sepBefore);
                it != ourSeparators.end()) {
                it->second.Visibility(targetVis);
            }
        }
    }
}

// Remove all mod-injected buttons/separators from a CommandBar and restore
// DefaultLabelPosition
static void RemoveOurButtons(
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar,
    CommandBarEntry* pEntry = nullptr) {
    g_modifyingCommandsDepth++;
    try {
        auto commands = commandBar.PrimaryCommands();
        for (uint32_t i = commands.Size(); i > 0; i--) {
            auto command = commands.GetAt(i - 1);
            if (!IsOurElement(command)) {
                continue;
            }

            if (auto button =
                    command.try_as<
                        winrt::Microsoft::UI::Xaml::Controls::AppBarButton>()) {
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
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
    g_modifyingCommandsDepth--;

    if (!pEntry) {
        for (auto& ent : g_entries) {
            if (ent.commandBar.get() == commandBar) {
                pEntry = &ent;
                break;
            }
        }
    }

    if (pEntry && pEntry->hasModifiedDefaultLabelPosition) {
        try {
            commandBar.DefaultLabelPosition(
                pEntry->originalDefaultLabelPosition);
        } catch (...) {
        }
        pEntry->hasModifiedDefaultLabelPosition = false;
    }
}

// Start a 250ms zero-I/O DispatcherTimer to track tab switches and selection
// changes on the active window
static void StartSelectionTimerIfNeeded(
    CommandBarEntry& entry,
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar) {
    if (entry.selectionTimer || g_unloading) {
        return;
    }

    bool placeOnSecondary;
    bool hasFilters = false;
    bool anyLabelHasTokens = false;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        placeOnSecondary = g_settings.placeOnSecondaryBar;
        for (const auto& itm : g_settings.items) {
            if (!TrimWhitespaceAndQuotes(itm.matchExtensions).empty()) {
                hasFilters = true;
            }
            if (itm.GetDisplayLabel().find(L'%') != std::wstring::npos) {
                hasFilters = true;
                anyLabelHasTokens = true;
            }
        }
    }

    bool isOurTargetBar =
        placeOnSecondary
            ? (commandBar.Name() == L"FileExplorerSecondaryCommandBar")
            : (commandBar.Name() == L"FileExplorerCommandBar");

    if (!isOurTargetBar || !hasFilters) {
        return;
    }

    winrt::Microsoft::UI::Xaml::DispatcherTimer selectionTimer;
    selectionTimer.Interval(std::chrono::milliseconds(250));

    auto weakBar = winrt::make_weak(commandBar);

    auto timerToken = selectionTimer.Tick(
        [weakBar, anyLabelHasTokens](
            winrt::Windows::Foundation::IInspectable const&,
            winrt::Windows::Foundation::IInspectable const&) {
            UiCallScope scope;
            if (g_unloading) {
                return;
            }

            auto cb = weakBar.get();
            if (!cb) {
                return;
            }

            HWND hWnd = GetExplorerWindowForElement(cb);
            if (!hWnd || !IsWindowVisible(hWnd) || IsIconic(hWnd)) {
                return;
            }

            HWND hForeground = GetForegroundWindow();
            if (hForeground != hWnd &&
                GetAncestor(hForeground, GA_ROOTOWNER) != hWnd) {
                return;
            }

            CommandBarEntry* currentEntry = nullptr;
            for (auto& ent : g_entries) {
                if (ent.commandBar.get() == cb) {
                    currentEntry = &ent;
                    break;
                }
            }
            if (!currentEntry) {
                return;
            }

            // Detect visible ShellTabWindowClass so switching tabs immediately
            // updates cachedBrowser
            HWND hActiveTab = FindActiveTabWindow(hWnd);
            if (hActiveTab != currentEntry->cachedTabWnd ||
                !currentEntry->cachedBrowser) {
                auto browser = GetActiveShellBrowser(hWnd);

                currentEntry = nullptr;
                for (auto& ent : g_entries) {
                    if (ent.commandBar.get() == cb) {
                        currentEntry = &ent;
                        break;
                    }
                }
                if (!currentEntry) {
                    return;
                }

                currentEntry->cachedTabWnd = hActiveTab;
                currentEntry->cachedBrowser = std::move(browser);
                currentEntry->cachedShellView = nullptr;
            }

            if (!currentEntry->cachedBrowser) {
                return;
            }

            winrt::com_ptr<IShellView> view;
            if (FAILED(currentEntry->cachedBrowser->QueryActiveShellView(
                    view.put())) ||
                !view) {
                currentEntry->cachedBrowser = nullptr;
                currentEntry->cachedShellView = nullptr;
                return;
            }

            auto fv = view.try_as<IFolderView>();
            int count = 0;
            int focused = -1;
            if (!fv || FAILED(fv->ItemCount(SVGIO_SELECTION, &count))) {
                return;
            }

            fv->GetFocusedItem(&focused);

            if (view == currentEntry->cachedShellView &&
                count == currentEntry->lastSelectionCount &&
                focused == currentEntry->lastFocusedItem) {
                return;
            }

            currentEntry->cachedShellView = view;
            currentEntry->lastSelectionCount = count;
            currentEntry->lastFocusedItem = focused;

            ExplorerContext ctx;
            if (anyLabelHasTokens) {
                ctx = GetLightweightContextFromView(view.get(), count);
            } else {
                ctx.shellView = view;
                ctx.selectionCount = static_cast<size_t>(count > 0 ? count : 0);
            }
            UpdateDynamicButtonStates(cb, hWnd, &ctx);
        });

    selectionTimer.Start();
    entry.selectionTimer = selectionTimer;
    entry.selectionTickToken = timerToken;
}

// Synchronize custom buttons, DefaultLabelPosition::Right on secondary bar, and
// built-in button visibility
static void UpdateCommandBar(
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar) {
    if (g_unloading) {
        return;
    }

    bool placeOnSecondary;
    {
        std::lock_guard<std::mutex> lock(g_settings.mutex);
        placeOnSecondary = g_settings.placeOnSecondaryBar;
    }

    if (commandBar.Name() == L"FileExplorerCommandBar") {
        if (!placeOnSecondary) {
            EnsureActionButtons(commandBar);
        }
    } else if (commandBar.Name() == L"FileExplorerSecondaryCommandBar") {
        if (placeOnSecondary) {
            for (auto& ent : g_entries) {
                if (ent.commandBar.get() == commandBar) {
                    if (!ent.hasModifiedDefaultLabelPosition) {
                        try {
                            ent.originalDefaultLabelPosition =
                                commandBar.DefaultLabelPosition();
                            ent.hasModifiedDefaultLabelPosition = true;
                        } catch (...) {
                        }
                    }
                    break;
                }
            }
            try {
                commandBar.DefaultLabelPosition(
                    winrt::Microsoft::UI::Xaml::Controls::
                        CommandBarDefaultLabelPosition::Right);
            } catch (...) {
            }
            EnsureActionButtons(commandBar);
        }
    }

    ApplyDefaultButtonVisibility(commandBar);

    bool isOurBar =
        (placeOnSecondary &&
         commandBar.Name() == L"FileExplorerSecondaryCommandBar") ||
        (!placeOnSecondary && commandBar.Name() == L"FileExplorerCommandBar");
    if (isOurBar) {
        UpdateDynamicButtonStates(commandBar, nullptr);
    }
}

// Enqueue a coalesced CommandBar update on the UI DispatcherQueue protected by
// PendingDispatchScope and UiCallScope
static void QueueCommandBarUpdate(
    winrt::weak_ref<winrt::Microsoft::UI::Xaml::Controls::CommandBar> const&
        weakCommandBar,
    bool fullUpdate) {
    if (g_unloading) {
        return;
    }
    auto commandBar = weakCommandBar.get();
    if (!commandBar) {
        return;
    }

    void* key = winrt::get_abi(commandBar);
    auto [it, inserted] = g_pendingUpdates.insert({key, fullUpdate});
    if (!inserted) {
        it->second = it->second || fullUpdate;
        return;
    }

    auto takePending = [key]() {
        bool full = false;
        auto mapIt = g_pendingUpdates.find(key);
        if (mapIt != g_pendingUpdates.end()) {
            full = mapIt->second;
            g_pendingUpdates.erase(mapIt);
        }
        return full;
    };

    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        takePending();
        return;
    }

    auto pending = std::make_shared<PendingDispatchScope>();
    if (!dispatcherQueue.TryEnqueue([weakCommandBar, takePending, pending]() {
            UiCallScope scope;
            bool full = takePending();
            if (g_unloading) {
                return;
            }
            if (auto cb = weakCommandBar.get()) {
                try {
                    if (full) {
                        UpdateCommandBar(cb);
                    } else {
                        ApplyDefaultButtonVisibility(cb);
                    }
                } catch (...) {
                    Wh_Log(L"Error %08X", winrt::to_hresult().value);
                }
            }
        })) {
        takePending();
    }
}

// Register a newly discovered CommandBar instance and attach theme, unload, and
// VectorChanged observers
static void OnCommandBarAdded(
    winrt::Microsoft::UI::Xaml::Controls::CommandBar const& commandBar) {
    if (g_unloading || !commandBar) {
        return;
    }

    for (auto it = g_entries.begin(); it != g_entries.end();) {
        if (!it->commandBar.get()) {
            StopSelectionTimer(*it);
            it = g_entries.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& ent : g_entries) {
        if (ent.commandBar.get() == commandBar) {
            if (!ent.selectionTimer) {
                StartSelectionTimerIfNeeded(ent, commandBar);
            }
            UpdateCommandBar(commandBar);
            return;
        }
    }

    CommandBarEntry& entry = g_entries.emplace_back();
    entry.commandBar = winrt::make_weak(commandBar);

    TrackRevoker(
        commandBar,
        commandBar.ActualThemeChanged(
            winrt::auto_revoke,
            [](winrt::Microsoft::UI::Xaml::FrameworkElement const& sender,
               winrt::Windows::Foundation::IInspectable const&) {
                UiCallScope scope;
                if (g_unloading) {
                    return;
                }

                if (auto cb = sender.try_as<winrt::Microsoft::UI::Xaml::
                                                Controls::CommandBar>()) {
                    try {
                        RemoveOurButtons(cb);
                        UpdateCommandBar(cb);
                    } catch (...) {
                        Wh_Log(L"Error %08X", winrt::to_hresult().value);
                    }
                }
            }));

    TrackRevoker(
        commandBar,
        commandBar.Unloaded(
            winrt::auto_revoke,
            [](winrt::Windows::Foundation::IInspectable const& sender,
               winrt::Microsoft::UI::Xaml::RoutedEventArgs const&) {
                UiCallScope scope;
                auto cb = sender.try_as<
                    winrt::Microsoft::UI::Xaml::Controls::CommandBar>();
                for (auto& ent : g_entries) {
                    if (ent.commandBar.get() == cb) {
                        StopSelectionTimer(ent);
                        DetachVectorChangedObserver(ent);
                        break;
                    }
                }
            }));

    entry.vectorChangedToken = commandBar.PrimaryCommands().VectorChanged(
        [weakCommandBar = winrt::make_weak(commandBar)](
            winrt::Windows::Foundation::Collections::IObservableVector<
                winrt::Microsoft::UI::Xaml::Controls::
                    ICommandBarElement> const&,
            winrt::Windows::Foundation::Collections::
                IVectorChangedEventArgs const&) {
            UiCallScope scope;
            if (g_unloading || g_modifyingCommandsDepth > 0) {
                return;
            }
            QueueCommandBarUpdate(weakCommandBar, false);
        });
    entry.hasVectorChangedToken = true;

    StartSelectionTimerIfNeeded(entry, commandBar);
    UpdateCommandBar(commandBar);
}

// Tear down all timers, revokers, custom buttons, and system overrides on the
// current UI thread
static void RemoveButtonsForCurrentThread() {
    UnwatchManagedElementsForCurrentThread();
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<CommandBarEntry> taken;
    taken.swap(g_entries);

    for (auto& entry : taken) {
        StopSelectionTimer(entry);
        DetachVectorChangedObserver(entry);
        auto commandBar = entry.commandBar.get();
        if (!commandBar) {
            continue;
        }
        try {
            RemoveOurButtons(commandBar, &entry);
            ApplyDefaultButtonVisibility(commandBar, true);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }

    ForgetManagedElementsForCurrentThread();
    g_pendingUpdates.clear();
    g_threadScanned = false;
}

// Rebuild all custom buttons and re-apply updated system button settings on the
// current UI thread
static void RefreshButtonsForCurrentThread() {
    UiCallScope scope;
    if (g_unloading) {
        return;
    }

    UnwatchManagedElementsForCurrentThread();
    StopHoverTimersForCurrentThread();
    RevokeHandlersForCurrentThread();

    std::vector<CommandBarEntry> taken;
    taken.swap(g_entries);

    std::vector<winrt::Microsoft::UI::Xaml::Controls::CommandBar> activeBars;
    for (auto& entry : taken) {
        StopSelectionTimer(entry);
        DetachVectorChangedObserver(entry);
        if (auto commandBar = entry.commandBar.get()) {
            try {
                RemoveOurButtons(commandBar, &entry);
                ApplyDefaultButtonVisibility(commandBar, true);
                activeBars.push_back(std::move(commandBar));
            } catch (...) {
                Wh_Log(L"Error %08X", winrt::to_hresult().value);
            }
        }
    }

    ForgetManagedElementsForCurrentThread();
    g_pendingUpdates.clear();

    for (auto const& commandBar : activeBars) {
        try {
            OnCommandBarAdded(commandBar);
        } catch (...) {
            Wh_Log(L"Error %08X", winrt::to_hresult().value);
        }
    }
}

// ============================================================================
// XAML Visual Tree Discovery & Thread Scan Scheduling
// ============================================================================

// Match the primary and secondary WinUI 3 File Explorer CommandBar element
// names
static bool IsTargetCommandBarName(std::wstring_view name) {
    return name == L"FileExplorerCommandBar" ||
           name == L"FileExplorerSecondaryCommandBar";
}

// Check if both primary and secondary CommandBars have already been located
static bool FoundAllCommandBars(
    std::vector<winrt::Microsoft::UI::Xaml::Controls::CommandBar> const&
        commandBars) {
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

// Traverse a XamlRoot visual tree up to depth 64 to locate File Explorer
// CommandBar controls
static void CollectCommandBars(
    winrt::Microsoft::UI::Xaml::DependencyObject const& root,
    int depth,
    std::vector<winrt::Microsoft::UI::Xaml::Controls::CommandBar>*
        commandBars) {
    if (depth > 64) {
        return;
    }
    int count =
        winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(
            root);
    for (int i = 0; i < count; i++) {
        if (FoundAllCommandBars(*commandBars)) {
            return;
        }
        auto child =
            winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetChild(root,
                                                                          i);
        if (auto commandBar =
                child
                    .try_as<winrt::Microsoft::UI::Xaml::Controls::CommandBar>();
            commandBar && IsTargetCommandBarName(commandBar.Name())) {
            commandBars->push_back(std::move(commandBar));
            continue;
        }
        CollectCommandBars(child, depth + 1, commandBars);
    }
}

// Scan an element's XamlRoot visual tree and register discovered CommandBars
static void ScanXamlRootForCommandBars(
    winrt::Microsoft::UI::Xaml::UIElement const& element) try {
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

    std::vector<winrt::Microsoft::UI::Xaml::Controls::CommandBar> commandBars;
    CollectCommandBars(content, 0, &commandBars);
    for (auto const& commandBar : commandBars) {
        OnCommandBarAdded(commandBar);
    }
    if (!commandBars.empty()) {
        g_threadScanned = true;
    }
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Enqueue an asynchronous XamlRoot visual tree scan guarded by
// PendingDispatchScope and UiCallScope
static void ScheduleXamlRootScan(
    winrt::Microsoft::UI::Xaml::UIElement const& element) try {
    if (g_unloading || !element) {
        return;
    }
    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanXamlRootForCommandBars(element);
        return;
    }
    auto pending = std::make_shared<PendingDispatchScope>();
    dispatcherQueue.TryEnqueue(
        [weakElement = winrt::make_weak(element), pending]() {
            UiCallScope scope;
            if (g_unloading) {
                return;
            }
            if (auto el = weakElement.get()) {
                ScanXamlRootForCommandBars(el);
            }
        });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Return any live CommandBar weak reference already registered on the current
// UI thread
static winrt::Microsoft::UI::Xaml::Controls::CommandBar
GetKnownCommandBarForCurrentThread() {
    for (auto const& entry : g_entries) {
        if (auto commandBar = entry.commandBar.get()) {
            return commandBar;
        }
    }
    return nullptr;
}

// Discover CommandBars on the current UI thread starting from a known bar or
// focused XAML element
static void ScanCurrentThreadForCommandBars() try {
    UiCallScope scope;
    if (g_unloading) {
        return;
    }
    if (auto knownCommandBar = GetKnownCommandBarForCurrentThread()) {
        ScanXamlRootForCommandBars(knownCommandBar);
        return;
    }
    auto focused =
        winrt::Microsoft::UI::Xaml::Input::FocusManager::GetFocusedElement();
    auto element = focused
                       ? focused.try_as<winrt::Microsoft::UI::Xaml::UIElement>()
                       : nullptr;
    if (!element) {
        Wh_Log(L"No XAML element to start from on thread %u",
               GetCurrentThreadId());
        return;
    }
    ScanXamlRootForCommandBars(element);
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// Enqueue a full current-thread XAML scan on the UI DispatcherQueue
static void ScheduleCurrentThreadScan() try {
    if (g_unloading) {
        return;
    }
    auto dispatcherQueue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::
        GetForCurrentThread();
    if (!dispatcherQueue) {
        ScanCurrentThreadForCommandBars();
        return;
    }
    auto pending = std::make_shared<PendingDispatchScope>();
    dispatcherQueue.TryEnqueue([pending]() {
        UiCallScope scope;
        if (g_unloading) {
            return;
        }
        ScanCurrentThreadForCommandBars();
    });
} catch (...) {
    Wh_Log(L"Error %08X", winrt::to_hresult().value);
}

// ============================================================================
// FileExplorerExtensions.dll Symbol Hooks & LoadLibraryExW Interceptor
// ============================================================================

using CommandBarManager_CommandBar_t = void(WINAPI*)(void* pThis,
                                                     void* commandBar);
static CommandBarManager_CommandBar_t CommandBarManager_CommandBar_Original =
    nullptr;

// Intercept CommandBarManager::CommandBar assignment to inject buttons as soon
// as bars initialize
static void WINAPI CommandBarManager_CommandBar_Hook(void* pThis,
                                                     void* commandBar) {
    UiCallScope scope;
    Wh_Log(L">");
    CommandBarManager_CommandBar_Original(pThis, commandBar);
    if (g_unloading || !commandBar) {
        return;
    }
    try {
        auto const& element = *reinterpret_cast<
            winrt::Microsoft::UI::Xaml::Controls::CommandBar const*>(
            commandBar);
        if (!element) {
            return;
        }
        Wh_Log(L"Command bar %s, thread %u", element.Name().c_str(),
               GetCurrentThreadId());
        OnCommandBarAdded(element);
        ScheduleXamlRootScan(element);
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

using CommandBarControl_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
static CommandBarControl_OnApplyTemplate_t
    CommandBarControl_OnApplyTemplate_Original = nullptr;
static CommandBarControl_OnApplyTemplate_t
    CommandBarControl_Wave1_OnApplyTemplate_Original = nullptr;

// Trigger a thread XAML scan when CommandBarControl applies its control
// template
static void WINAPI CommandBarControl_OnApplyTemplate_Hook(void* pThis) {
    UiCallScope scope;
    Wh_Log(L">");
    CommandBarControl_OnApplyTemplate_Original(pThis);
    if (!g_unloading) {
        ScheduleCurrentThreadScan();
    }
}

// Trigger a thread XAML scan when Wave1 CommandBarControl applies its control
// template
static void WINAPI CommandBarControl_Wave1_OnApplyTemplate_Hook(void* pThis) {
    UiCallScope scope;
    Wh_Log(L">");
    CommandBarControl_Wave1_OnApplyTemplate_Original(pThis);
    if (!g_unloading) {
        ScheduleCurrentThreadScan();
    }
}

using CommandBarControl_GotFocusHandler_t = void(WINAPI*)(void* pThis,
                                                          void* sender,
                                                          void* args);
static CommandBarControl_GotFocusHandler_t
    CommandBarControl_GotFocusHandler_Original = nullptr;
static CommandBarControl_GotFocusHandler_t
    CommandBarControl_Wave1_GotFocusHandler_Original = nullptr;

// Fallback handler scanning the XamlRoot if a CommandBarControl receives focus
// before initial scan
static void HandleCommandBarControlGotFocus(void* sender) {
    UiCallScope scope;
    if (g_unloading || !sender) {
        return;
    }
    if (g_threadScanned && GetKnownCommandBarForCurrentThread()) {
        return;
    }
    try {
        auto const& inspectable =
            *reinterpret_cast<winrt::Windows::Foundation::IInspectable const*>(
                sender);
        if (auto element =
                inspectable
                    ? inspectable
                          .try_as<winrt::Microsoft::UI::Xaml::UIElement>()
                    : nullptr) {
            ScanXamlRootForCommandBars(element);
        }
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult().value);
    }
}

// Hook CommandBarControlGotFocusHandler to catch un-scanned Explorer tabs
static void WINAPI CommandBarControl_GotFocusHandler_Hook(void* pThis,
                                                          void* sender,
                                                          void* args) {
    CommandBarControl_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

// Hook Wave1 CommandBarControlGotFocusHandler to catch un-scanned Explorer tabs
static void WINAPI CommandBarControl_Wave1_GotFocusHandler_Hook(void* pThis,
                                                                void* sender,
                                                                void* args) {
    CommandBarControl_Wave1_GotFocusHandler_Original(pThis, sender, args);
    HandleCommandBarControlGotFocus(sender);
}

static std::atomic<bool> g_symbolsHooked{false};

enum class SymbolHookResult {
    Success,
    ResolutionFailed,
    NoSymbolFound,
};

// Resolve and register internal C++/WinRT symbols inside
// FileExplorerExtensions.dll
static SymbolHookResult HookFileExplorerExtensionsSymbols(HMODULE module) {
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

// Check if FileExplorerExtensions.dll is already mapped into explorer.exe
static HMODULE GetFileExplorerExtensionsModuleHandle() {
    return GetModuleHandleW(L"FileExplorerExtensions.dll");
}

// Install FileExplorerExtensions.dll symbol hooks atomically once the module
// loads
static bool HookFileExplorerExtensionsIfLoaded(bool applyHooks) {
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
            g_symbolsHooked = false;
            return false;
        case SymbolHookResult::NoSymbolFound:
            return false;
    }

    if (applyHooks) {
        Wh_ApplyHookOperations();
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

// Intercept deferred loading of FileExplorerExtensions.dll when Explorer opens
// its first window
static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                          HANDLE hFile,
                                          DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading || !lpLibFileName) {
        return module;
    }

    PCWSTR fileName = lpLibFileName;
    for (PCWSTR p = lpLibFileName; *p; p++) {
        if (*p == L'\\' || *p == L'/') {
            fileName = p + 1;
        }
    }

    if (_wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0 ||
        _wcsicmp(fileName, L"FileExplorerExtensions") == 0) {
        HookFileExplorerExtensionsIfLoaded(true);
    }
    return module;
}

// ============================================================================
// Cross-Thread Window Dispatch & O(N log N) Hierarchical Settings Loader
// ============================================================================

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

// Marshal a synchronous callback onto a target Explorer HWND's owning UI thread
// via WH_CALLWNDPROC
static bool RunFromWindowThread(HWND hWnd,
                                RunFromWindowThreadProc_t proc,
                                PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunFromWindowThreadParam {
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

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = reinterpret_cast<RunFromWindowThreadParam*>(
                        cwp->lParam);
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);

    if (!hook) {
        return false;
    }

    RunFromWindowThreadParam param{proc, procParam};
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0,
                 reinterpret_cast<LPARAM>(&param));
    UnhookWindowsHookEx(hook);
    return true;
}

// Parse Windhawk settings and construct sorted dropdown menu trees in O(N log
// N) time using hash maps
static void LoadSettings() {
    ClearIconCache();

    std::lock_guard<std::mutex> lock(g_settings.mutex);

    g_settings.placeOnSecondaryBar =
        Wh_GetIntSetting(L"placeOnSecondaryBar") != 0;
    g_settings.disabledInsteadOfHidden =
        Wh_GetIntSetting(L"disabledInsteadOfHidden") != 0;

    for (int i = 0; i < kDefaultButtonCount; i++) {
        g_settings.hideDefaultButtons[i] =
            Wh_GetIntSetting(L"hideDefaultButtons.%s",
                             kDefaultButtons[i].settingKey) != 0;
    }

    g_settings.hideMoreButton =
        Wh_GetIntSetting(L"hideDefaultButtons.moreOptions") != 0;

    memset(g_settings.hideSeparatorAfterButton, 0,
           sizeof(g_settings.hideSeparatorAfterButton));
    g_settings.hideSeparatorAfterButton[kNewButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterNew") != 0;
    g_settings.hideSeparatorAfterButton[kDeleteButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterDelete") != 0;
    g_settings.hideSeparatorAfterButton[kViewButtonIndex] =
        Wh_GetIntSetting(L"hideDefaultButtons.separatorAfterView") != 0;

    for (int i = 0; i < kDefaultButtonCount; i++) {
        auto labelSetting = WindhawkUtils::StringSetting::make(
            L"systemButtonLabels.%s", kDefaultButtons[i].settingKey);
        g_settings.customLabels[i] =
            TrimWhitespaceAndQuotes(labelSetting.get());
    }

    g_settings.openMenuOnHover =
        Wh_GetIntSetting(L"menuHover.openOnHover") != 0;
    int menuHoverDelay = Wh_GetIntSetting(L"menuHover.delay");
    g_settings.menuHoverDelay = menuHoverDelay >= 0 ? menuHoverDelay : 0;

    std::vector<ActionItem> flatItems;
    std::unordered_map<std::wstring, std::vector<size_t>> childrenByParent;
    std::unordered_set<std::wstring> validMenuIds;

    for (int i = 0; i < 200; i++) {
        WCHAR prefix[64];
        swprintf(prefix, ARRAYSIZE(prefix), L"items[%d]", i);

        auto idSetting = WindhawkUtils::StringSetting::make(L"%s.id", prefix);
        auto cmdSetting =
            WindhawkUtils::StringSetting::make(L"%s.command", prefix);
        auto labelSetting =
            WindhawkUtils::StringSetting::make(L"%s.labelText", prefix);

        if (!*idSetting.get() && !*cmdSetting.get() && !*labelSetting.get()) {
            break;
        }

        ActionItem item;
        item.id = TrimWhitespaceAndQuotes(idSetting.get());
        if (item.id.empty()) {
            item.id = L"item_" + std::to_wstring(i);
        }

        auto typeStr = WindhawkUtils::StringSetting::make(L"%s.type", prefix);
        item.isMenu = (wcscmp(typeStr.get(), L"menu") == 0);

        item.parent = TrimWhitespaceAndQuotes(
            WindhawkUtils::StringSetting::make(L"%s.parent", prefix).get());
        item.labelText = labelSetting.get();
        item.command = cmdSetting.get();
        item.parameters =
            WindhawkUtils::StringSetting::make(L"%s.parameters", prefix).get();
        item.matchExtensions =
            WindhawkUtils::StringSetting::make(L"%s.matchExtensions", prefix)
                .get();
        item.icon =
            WindhawkUtils::StringSetting::make(L"%s.iconGlyph", prefix).get();

        auto orderSetting =
            WindhawkUtils::StringSetting::make(L"%s.order", prefix);
        item.order = (*orderSetting.get()) ? _wtof(orderSetting.get()) : 0.0;

        item.hideIcon = Wh_GetIntSetting(L"%s.hideIcon", prefix) != 0;
        item.showLabel = Wh_GetIntSetting(L"%s.showLabel", prefix) != 0;
        item.separatorBefore =
            Wh_GetIntSetting(L"%s.separatorBefore", prefix) != 0;
        item.separatorAfter =
            Wh_GetIntSetting(L"%s.separatorAfter", prefix) != 0;

        size_t currentIndex = flatItems.size();
        if (item.isMenu) {
            validMenuIds.insert(ToLower(item.id));
        }
        if (!item.parent.empty()) {
            childrenByParent[ToLower(item.parent)].push_back(currentIndex);
        }
        flatItems.push_back(std::move(item));
    }

    // Collect and sort top-level toolbar items while preserving original list
    // order on ties
    std::vector<ActionItem> rootItems;
    std::vector<bool> attachedItemIndices(flatItems.size(), false);
    for (size_t i = 0; i < flatItems.size(); i++) {
        if (flatItems[i].parent.empty()) {
            rootItems.push_back(flatItems[i]);
            attachedItemIndices[i] = true;
        }
    }

    std::stable_sort(rootItems.begin(), rootItems.end(),
                     [](const ActionItem& a, const ActionItem& b) {
                         return a.order < b.order;
                     });

    // Attach pre-indexed submenu children recursively with cycle and depth
    // protection
    std::unordered_set<std::wstring> activeAncestors;
    std::function<void(ActionItem&, size_t)> attachChildren =
        [&](ActionItem& node, size_t depth) {
            if (!node.isMenu || depth >= kMaxMenuHierarchyDepth) {
                return;
            }

            std::wstring lowerId = ToLower(node.id);
            auto mapIt = childrenByParent.find(lowerId);
            if (mapIt == childrenByParent.end()) {
                return;
            }

            activeAncestors.insert(lowerId);
            for (size_t childIdx : mapIt->second) {
                const auto& candidate = flatItems[childIdx];
                if (candidate.isMenu &&
                    activeAncestors.contains(ToLower(candidate.id))) {
                    Wh_Log(
                        L"Warning: Skipping cyclic menu reference '%s' -> '%s'",
                        node.id.c_str(), candidate.id.c_str());
                    continue;
                }

                attachedItemIndices[childIdx] = true;
                ActionItem child = candidate;
                attachChildren(child, depth + 1);
                node.subItems.push_back(std::move(child));
            }

            std::stable_sort(node.subItems.begin(), node.subItems.end(),
                             [](const ActionItem& a, const ActionItem& b) {
                                 return a.order < b.order;
                             });

            activeAncestors.erase(lowerId);
        };

    for (auto& root : rootItems) {
        attachChildren(root, 0);
    }

    // Warn in O(N) time if any child item was unreachable due to an unknown,
    // non-menu, or disconnected cyclic parent ID
    for (size_t i = 0; i < flatItems.size(); i++) {
        if (!flatItems[i].parent.empty() && !attachedItemIndices[i]) {
            const auto& item = flatItems[i];
            if (validMenuIds.find(ToLower(item.parent)) == validMenuIds.end()) {
                Wh_Log(
                    L"Warning: Item '%s' references unknown or non-menu parent "
                    L"ID '%s' - item will not be visible",
                    item.id.c_str(), item.parent.c_str());
            } else {
                Wh_Log(
                    L"Warning: Item '%s' (parent '%s') is unreachable from the "
                    L"root toolbar (disconnected cycle or depth > %zu) - item "
                    L"will not be visible",
                    item.id.c_str(), item.parent.c_str(),
                    kMaxMenuHierarchyDepth);
            }
        }
    }

    g_settings.items = std::move(rootItems);
}

// ============================================================================
// Windhawk Mod Lifecycle Entry Points
// ============================================================================

// Initialize mod configuration and register FileExplorerExtensions.dll or
// LoadLibraryExW hooks
BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_unloading = false;
    LoadSettings();

    if (GetFileExplorerExtensionsModuleHandle()) {
        if (!HookFileExplorerExtensionsIfLoaded(false)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"FileExplorerExtensions.dll isn't loaded yet");
        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            reinterpret_cast<decltype(&LoadLibraryExW)>(
                GetProcAddress(kernelBaseModule, "LoadLibraryExW"));
        if (!pKernelBaseLoadLibraryExW) {
            return FALSE;
        }
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

// Scan all existing File Explorer windows once Windhawk finishes applying
// initial hooks
void Wh_ModAfterInit() {
    Wh_Log(L">");
    HookFileExplorerExtensionsIfLoaded(true);
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { ScanCurrentThreadForCommandBars(); }, nullptr);
    }
}

// Signal imminent mod unload so hooks, timers, and queued callbacks cease new
// UI work
void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
}

// Drain queued dispatches, remove injected XAML elements on UI threads, and
// join worker threads
void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;

    while (g_pendingDispatches > 0) {
        Sleep(10);
    }

    for (HWND hWnd : GetFileExplorerWnds()) {
        Wh_Log(L"Removing buttons for window %08X",
               static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hWnd)));
        if (!RunFromWindowThread(
                hWnd, [](PVOID) { RemoveButtonsForCurrentThread(); },
                nullptr)) {
            Wh_Log(L"Couldn't reach the thread of window %08X",
                   static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(hWnd)));
        }
    }

    while (g_uiCallsInProgress > 0) {
        Sleep(10);
    }

    ClearIconCache();

    {
        std::lock_guard<std::mutex> lock(g_launchThreadsMutex);
        if (!g_launchThreads.empty()) {
            Wh_Log(L"Waiting for %zu launch thread(s)", g_launchThreads.size());
        }
    }

    WaitForLaunchThreads();
}

// Reload settings and refresh CommandBar elements across all open File Explorer
// windows
void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    if (g_unloading) {
        return;
    }
    LoadSettings();
    for (HWND hWnd : GetFileExplorerWnds()) {
        RunFromWindowThread(
            hWnd, [](PVOID) { RefreshButtonsForCurrentThread(); }, nullptr);
    }
}

// ==WindhawkMod==
// @id              start-menu-groups
// @name            Start Menu Groups
// @description     Replace the Windows 11 Start menu "Category" view with your own groups, apps and layout
// @version         1.1.0
// @author          DanRotaru
// @github          https://github.com/DanRotaru
// @donateUrl       https://buymeacoffee.com/widget/page/danrotaru
// @homepage        https://dan13.me/
// @include         StartMenuExperienceHost.exe
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid -lgdi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# Start Menu Groups

Add your own **app groups** to the **Windows 11 Start menu**, before or after the Pinned section (or in place of the built‑in **All → View: Category** grid), using the same look as Start: rounded tiles with an icon preview, an overflow cluster for the remaining apps and the group name underneath.

![Start Menu Groups demo](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/start-menu-groups/screenshots/start-menu.gif)

Designed for the Windows 11 Start menu (`StartMenuExperienceHost.exe`).

## Features

- **Your own groups** — create any number of groups and put any Start menu app in them, desktop and Store apps alike. An app can be in several groups.
- **Apps, programs and websites** — each item is a Start app (picked by name or app ID), a program, file or folder (full path with optional arguments), or a website opened in the default browser.
- **Custom names and icons** — every item can have its own label and icon: an `.ico`, `.exe`, `.dll` or image file, an image link (such as a website's favicon), a Fluent icon by its code (e.g. `E774`) or an emoji.
- **Native look** — rounded tiles, borders and colors match Start's own category view and follow the Windows light / dark theme.
- **Resizable tiles** — give each group its own tile size, from 1 × 1 up to 6 × 4 (24 apps). Bigger tiles take more cells of the group grid. A 1 × 1 tile is a single app on its own, without a group.
- **Overflow cluster** — when a group has more apps than fit, the last cell shows the rest as small icons; click it to open the apps that didn't fit.
- **Folders** — put any number of apps in a folder: it takes one cell of the tile, shows its first apps as small icons, and opens like a group when clicked.
- **Rearrange in Start** — drag a group to move it, or drag an app to reorder it or move it to another group, right in the Start menu. No settings window and no administrator prompt (see [Rearranging in Start](#rearranging-in-start)).
- **Right‑click menu** — right‑click an app in a group for **Edit**, **Run as administrator**, **Open file location**, **Pin to taskbar**, **App settings** and **Uninstall**, like Start's own apps.
- **Size presets** — Default, Small and Large presets for the tiles, icons and names, plus control over gaps, corner radius, colors and group names.
- **Placement** — show the groups before or after Pinned, after Recommended / Recent, or after the All section, with an optional section title.
- **Hide Start's sections** — optionally hide Start's own Pinned, Recommended and All sections and use only your groups.
- **Settings window** — right‑click the groups in Start → **Customize Start Groups** for a visual editor with drag & drop (see [Settings window](#settings-window)).
- **Import / export** — all settings can be exported to and imported from a JSON file.

## Screenshots

The settings window, with drag & drop editing of groups and apps:

![Settings window](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/start-menu-groups/screenshots/settings-window.gif)

## Setting it up

Everything is configured on the mod's **Settings** tab in Windhawk, in
sections:

* **Groups** — add a group with *Add new item*, give it a name, a tile size and its items. For a Start app, enter the app's name exactly as it appears in Start (for example `Calculator`, `Visual Studio Code`; case doesn't matter), or its app ID (AUMID, such as `Microsoft.WindowsCalculator_8wekyb3d8bbwe!App`). For a program, its full path (a program, file or folder) and optional arguments; for a website, its address. For a folder, its name (*Label*) and its apps under *Folder apps*. Reorder groups with the arrows.
* **Behavior** — where the groups appear, the section title, app names and tooltips, sorting, empty groups, the number of columns when a group is opened, and hiding Start's own sections.
* **Tiles**, **Icons**, **Group names** — sizes, gaps and colors. The defaults match Start's own category view.

Changes apply to Start as soon as you click **Save settings**.

## Settings window

Right‑click the groups in Start → **Customize Start Groups** (also added to Start's own right‑click menu, after *Start settings*) opens a settings window with the same settings, but easier to edit:

* **Groups** — shows the groups the way Start does. Drag apps to reorder them or move them to another group, drag a tile to reorder the groups, drag a tile's corner to resize it, and drag apps from the All Apps list onto a tile to add them. **Add app → Folder** adds a folder, and dropping an app onto the middle of another app puts both in a new folder: drop apps onto it to put them in, and click it to open it, then reorder or drag apps back out. The selected group and app are edited below the groups. Right‑click a tile to rename, duplicate or delete it.
* **Customization** — the size preset (Default, Small, Large), gaps, corner radius, colors and group names.
* **Behavior** — placement, section title, app names, tooltips, sorting and columns.
* **Built‑in sections** — hide Start's own Pinned, Recommended and All sections.

Start apps are picked by name with suggestions from the apps in Start; programs have a file picker. Groups and items can be duplicated, and an app can be added on its own, without a group. **Import** / **Export** at the bottom of the navigation pane save all settings to, or load them from, a JSON file.

The window is hosted by File Explorer — the mod also loads into `explorer.exe` for this, and does nothing else there — because the Start menu process can't host one. It edits the same settings as Windhawk. Windhawk keeps them in a protected registry key, so **Save** shows an administrator (UAC) prompt for Windows PowerShell, which imports the new settings, and Start updates right after. The import checks that the settings file is exactly the one the window wrote, so no other program can change what gets saved. If the settings were changed in Windhawk while the window was open, **Save** asks before replacing them. The portable version of Windhawk isn't supported by the window.

## Rearranging in Start

The groups can be rearranged right in Start, with the mouse:

* **Move a group** — drag its tile by its background (between the apps), or drag an app on its own (a 1 × 1 tile), and drop it before or after another tile.
* **Move an app** — drag it within its group to reorder it, or onto another group to move it there. Dropped on a group's overflow cluster, it goes at the end of the group. Dropped outside the tiles (between them, or below the last one), it becomes an app on its own (a 1 × 1 tile) there, as in the settings window. An app on its own dropped onto a group goes into that group. With *Sort apps by name* on, apps keep their sorted order, so they can only be moved to another group.
* **Folders** — drop an app onto the middle of another app to put both in a new folder, or onto a folder to add it. In an opened folder or group, drag its apps to reorder them, or out of it to drop them anywhere on the groups.

The new order shows right away and stays: Start keeps it in the mod's own storage, which needs no administrator prompt. Windhawk's **Settings** tab still shows the order from before. The settings window shows Start's order, and saving there writes it into the mod's settings (and Start's order is then no longer kept on its own).

Changes made in Windhawk are kept as well: Start's order is applied on top of them, matching groups by name and apps by their settings. New groups and apps go where they are in the settings. A drag in Start made while the settings window is open is replaced by what the window saves.

Not supported in Start (use the settings window): resizing tiles, adding or removing apps, making an app on its own into a group, renaming folders, and dragging with touch.

## Tile size and apps in the tile

Each group has two size settings:

* **Tile size** — width × height of the tile, in app spots. 2 × 2 is the size of one of Start's own tiles; 1 × 1 is an app on its own, without a group.
* **Apps in the tile** — apps per row × rows of apps, at most the tile size. *Default* fills the tile with one app per spot; the rest go in the overflow cluster.

A group can also hide its app names, its group name, or its tile background (no background, border or padding — just the apps, e.g. for a single app on its own).

## Colors

Colors are `#RRGGBB` or `#AARRGGBB`. Leave a color empty to follow the Windows theme and match Start.

## Network access

The mod doesn't connect to the internet by itself, and the default settings don't contact any server: the default websites show a globe icon.

An icon set to an image link (`http://` or `https://`, such as a website's favicon) is downloaded from that address by Windows' XAML image loader: a normal web request for the image, which sends your IP address and a user agent to that server. It happens when Start builds the tiles (when the mod loads, on a theme or settings change, and when a group is opened), and when the settings window shows the item. To avoid it, leave the icon empty or use a local file, a Fluent icon code or an emoji.

## Compatibility

The mod reaches Start's UI directly, without XAML diagnostics, so it works alongside mods and tools that use them, such as Windows 11 Start Menu Styler.

## Language

The groups are placed next to Start's pinned list, found by its element name, so they show in any display language. Two things go by Start's English titles: hiding Start's sections (*Pinned*, *Recommended* / *Recent*, *All*), and adding *Customize Start Groups* after *Start settings* in Start's own right‑click menu. In other languages, open the settings window by right‑clicking the groups instead.

## Default configuration

Out of the box the mod adds these groups before the Pinned section, under an **Apps** heading:

* **Group 1** — Microsoft Store, Settings, Notepad, Weather
* **Group 2** — Camera, Paint, Calculator, Calendar
* **Favorites** — Photos, Terminal, Notepad, Get Help (group name hidden)
* **Group 3** (3 × 3) — Sound Recorder, Voice access, Task Manager, Movies & TV, File Explorer, Sticky Notes
* **System** (3 × 1) — Control Panel, Services, Run (group name hidden)
* **Websites** (3 × 2) — YouTube, ChatGPT, Example, Google, Reddit, dan13.me

Empty groups are hidden. None of Start's own sections are hidden by default. Everything is configurable in the mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- groups:
  - - name: "Group 1"
      $name: Group name
    - span: 2x2
      $name: Tile size
      $description: >-
        Width × height of the tile, in app spots: 2 × 2 is the size of one
        of Start's own tiles. 1 × 1 is an app on its own, without a group.
      $options:
        - 1x1: 1 × 1
        - 1x2: 1 × 2
        - 1x3: 1 × 3
        - 1x4: 1 × 4
        - 1x5: 1 × 5
        - 1x6: 1 × 6
        - 2x1: 2 × 1
        - 3x1: 3 × 1
        - 4x1: 4 × 1
        - 5x1: 5 × 1
        - 6x1: 6 × 1
        - 2x2: 2 × 2
        - 3x2: 3 × 2
        - 3x3: 3 × 3
        - 4x2: 4 × 2
        - 4x4: 4 × 4
        - 6x2: 6 × 2
        - 6x4: 6 × 4
    - tileSize: auto
      $name: Apps in the tile
      $description: >-
        Apps per row × rows of apps, at most the tile size. Default fills
        the tile: one app per spot. The rest go in the overflow cluster.
      $options:
        - auto: Default (fills the tile)
        - 1x1: 1 × 1 (1 app)
        - 1x2: 1 × 2 (2 apps)
        - 1x3: 1 × 3 (3 apps)
        - 1x4: 1 × 4 (4 apps)
        - 1x5: 1 × 5 (5 apps)
        - 1x6: 1 × 6 (6 apps)
        - 2x1: 2 × 1 (2 apps)
        - 2x2: 2 × 2 (4 apps)
        - 2x3: 2 × 3 (6 apps)
        - 2x4: 2 × 4 (8 apps)
        - 3x1: 3 × 1 (3 apps)
        - 3x2: 3 × 2 (6 apps)
        - 3x3: 3 × 3 (9 apps)
        - 3x4: 3 × 4 (12 apps)
        - 4x1: 4 × 1 (4 apps)
        - 4x2: 4 × 2 (8 apps)
        - 4x3: 4 × 3 (12 apps)
        - 4x4: 4 × 4 (16 apps)
        - 5x1: 5 × 1 (5 apps)
        - 5x2: 5 × 2 (10 apps)
        - 5x3: 5 × 3 (15 apps)
        - 5x4: 5 × 4 (20 apps)
        - 6x1: 6 × 1 (6 apps)
        - 6x2: 6 × 2 (12 apps)
        - 6x3: 6 × 3 (18 apps)
        - 6x4: 6 × 4 (24 apps)
    - hideAppNames: false
      $name: Hide app names
      $description: >-
        Hide the app names in this group's tile (when "Show app names" is
        on in Behavior)
    - hideLabel: false
      $name: Hide the group name
      $description: The name under the tile (when group names are shown)
    - hideBackground: false
      $name: Hide the tile's background
      $description: >-
        No background, border or padding: just the apps, e.g. for a single
        app on its own
    - items:
      - - type: app
          $name: Type
          $options:
            - app: Start app
            - program: Program
            - website: Website
            - folder: Folder
        - app: Microsoft Store
          $name: Start app
          $description: >-
            For "Start app": the app's name exactly as Start shows it (case
            doesn't matter), or its app ID (AUMID)
        - label: ""
          $name: Label
          $description: Name shown in the tooltip. Empty = the app's or program's own name.
        - path: ""
          $name: Program path or website
          $description: >-
            For "Program": full path to the program, file, or folder. For
            "Website": its address, e.g. https://example.com
        - args: ""
          $name: Arguments
        - icon: ""
          $name: Icon
          $description: >-
            An .ico, .exe, .dll or image file, an image link, a Fluent icon by
            its code (e.g. E774) or an emoji. Empty = the default icon.
        - apps:
          - - type: app
              $name: Type
              $options:
                - app: Start app
                - program: Program
                - website: Website
            - app: ""
              $name: Start app
            - label: ""
              $name: Label
            - path: ""
              $name: Program path or website
            - args: ""
              $name: Arguments
            - icon: ""
              $name: Icon
          $name: Folder apps
          $description: >-
            For "Folder": the apps in it, opened like a group when the
            folder is clicked. The folder's name is its Label.
      - - type: app
        - app: "Settings"
      - - type: app
        - app: "Notepad"
      - - type: app
        - app: "Weather"
      $name: Items
  - - name: "Group 2"
    - span: 2x2
    - items:
      - - type: app
        - app: "Camera"
      - - type: app
        - app: "Paint"
      - - type: app
        - app: "Calculator"
      - - type: app
        - app: "Calendar"
  - - name: "Favorites"
    - span: 2x2
    - hideLabel: true
    - items:
      - - type: app
        - app: "Photos"
      - - type: app
        - app: "Terminal"
      - - type: app
        - app: "Notepad"
      - - type: app
        - app: "Get Help"
  - - name: "Group 3"
    - span: 3x3
    - items:
      - - type: app
        - app: "Sound Recorder"
      - - type: app
        - app: "Voice access"
      - - type: app
        - app: "Task Manager"
      - - type: app
        - app: "Movies & TV"
      - - type: app
        - app: "File Explorer"
      - - type: app
        - app: "Sticky Notes"
  - - name: "System"
    - span: 3x1
    - hideLabel: true
    - items:
      - - type: app
        - app: "Control Panel"
      - - type: app
        - app: "Services"
      - - type: app
        - app: "Run"
  - - name: "Websites"
    - span: 3x2
    - items:
      - - type: website
        - label: "YouTube"
        - path: "https://youtube.com/"
      - - type: website
        - label: "ChatGPT"
        - path: "https://chatgpt.com/"
      - - type: website
        - label: "Example"
        - path: "https://example.com/"
      - - type: website
        - label: "Google"
        - path: "https://google.com/"
      - - type: website
        - label: "Reddit"
        - path: "https://reddit.com/"
      - - type: website
        - label: "dan13.me"
        - path: "https://dan13.me/?utm_source=start-menu-groups-link"
  $name: Groups
  $description: >-
    Your groups, in display order. Easier to edit in the settings window:
    right-click the groups in Start, then "Customize Start Groups".
- behavior:
  - placement: beforePinned
    $name: Where to show the groups
    $options:
      - beforePinned: Before the Pinned section
      - afterPinned: After the Pinned section
      - afterRecent: After the Recommended / Recent section
      - afterAll: After the All section
  - hideEmptyGroups: true
    $name: Hide empty groups
  - hideAppTooltips: false
    $name: Hide app tooltips
    $description: >-
      The app's full name when hovering over it in Start, for apps whose
      name is cut off ("...") or hidden
  - sortAppsByName: false
    $name: Sort apps alphabetically
    $description: Ignore the order the apps are listed in
  - sectionTitle: Apps
    $name: Section title
    $description: Heading shown above the groups, styled like "Pinned". Empty = no heading.
  - showAppLabels: true
    $name: Show app names
    $description: Show each app's name under its icon inside the tiles
  - expandedColumns: 4
    $name: Columns when a group is opened
    $description: 2 to 5
  - hidePinned: false
    $name: Hide the Pinned section
    $description: Start's own Pinned apps
  - hideRecommended: false
    $name: Hide the Recommended section
    $description: Start's own Recommended / Recent section
  - hideAllApps: false
    $name: Hide the All section
    $description: Start's own list of all apps (grid, list or category view)
  $name: Behavior
- tiles:
  - size: 154
    $name: Tile size
    $description: Width and height of a group tile. Start's own tiles are 154. Tiles are made a little wider or narrower so the rows fill Start's width.
  - gapX: 25
    $name: Horizontal gap between tiles
  - gapY: 20
    $name: Vertical gap between rows
  - marginLeft: 0
    $name: Left spacing
    $description: Moves the groups' left edge from where Start lines them up, in DIPs (-100 to 100). Positive moves it in, negative out. 0 = Start's own.
  - marginRight: 0
    $name: Right spacing
    $description: Moves the groups' right edge from where Start lines them up, in DIPs (-100 to 100). Positive moves it in, negative out. 0 = Start's own.
  - padding: 8
    $name: Padding inside a tile
  - cornerRadius: 8
    $name: Corner radius
  - borderThickness: 1
    $name: Border thickness
  - background: ""
    $name: Background color
    $description: "#RRGGBB or #AARRGGBB. Empty = match Start."
  - border: ""
    $name: Border color
    $description: "#RRGGBB or #AARRGGBB. Empty = match Start."
  $name: Tiles
- icons:
  - size: 32
    $name: Icon size
  - gap: 5
    $name: Gap between apps
    $description: Space between the icons inside a tile
  - paddingX: 4
    $name: App padding (horizontal)
    $description: Space inside each app, left and right of its icon and name
  - paddingY: 4
    $name: App padding (vertical)
    $description: Space inside each app, above its icon and below its name
  - overflowCluster: true
    $name: Overflow cluster
    $description: When a group has more apps than fit, show the rest as small icons in the last cell
  $name: Icons
- labels:
  - show: true
    $name: Show group names
  - fontSize: 12
    $name: Font size
  - spacing: 12
    $name: Space between tile and name
  - color: ""
    $name: Color
    $description: "#RRGGBB or #AARRGGBB. Empty = match Start."
  $name: Group names
*/
// ==/WindhawkModSettings==

// clang-format on

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wincrypt.h>

#include <algorithm>
#include <deque>
#include <atomic>
#include <cmath>
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <thread>
#include <unordered_map>
#include <optional>
#include <tuple>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;
namespace wf = winrt::Windows::Foundation;
namespace wgi = winrt::Windows::Graphics::Imaging;
namespace wuc = winrt::Windows::UI::Core;
namespace xc = winrt::Windows::UI::Xaml::Controls;
namespace xcp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace xm = winrt::Windows::UI::Xaml::Media;
namespace wcomp = winrt::Windows::UI::Composition;
namespace xmi = winrt::Windows::UI::Xaml::Media::Imaging;

// ---------------------------------------------------------------------------
// Settings & config model
// ---------------------------------------------------------------------------

struct LayoutConfig {
    // Defaults match Start's built-in category view (in DIPs).
    double tileSize = 154;
    double tileSpacingX = 25;
    double tileSpacingY = 20;
    // Added to the space Start leaves left and right of the groups (see
    // HostMargin): positive moves that edge in, negative out.
    double marginLeft = 0;
    double marginRight = 0;
    double tilePadding = 8;
    double iconSize = 32;
    double iconSpacing = 5;
    double appPaddingX = 4;  // inside each app's button
    double appPaddingY = 4;
    double cornerRadius = 8;
    double labelFontSize = 12;
    double labelSpacing = 12;
    double borderThickness = 1;
    int flyoutColumns = 4;
    bool showLabels = true;
    bool hideEmptyGroups = true;
    bool showAppTooltips = true;  // the app's name on hover
    bool sortAppsByName = false;
    bool showOverflowCluster = true;
    bool showAppLabels = true;
    std::wstring tileBackground;  // "" = automatic for theme
    std::wstring tileBorder;
    std::wstring labelColor;
    std::wstring sectionTitle = L"Apps";
};

// A resolved, launchable tile entry.
struct AppEntry {
    std::wstring id;    // AUMID / AppsFolder parsing name (Start apps)
    std::wstring name;  // shown in tooltips and the expanded group
    std::wstring path;  // program path (programs); empty for Start apps
    std::wstring args;  // program arguments
    std::wstring iconSource;  // shell parsing name the icon is taken from
    std::wstring key;         // unique key for the icon cache
    bool website = false;     // `path` is a web address
    // A folder: a cell of its own in the tile that opens like a group,
    // showing `children` (Start apps, programs and websites; no folders).
    bool folder = false;
    std::vector<AppEntry> children;
    // Its group and item in the settings (Start's groups), for "Edit".
    int settingsGroup = -1;
    int settingsItem = -1;
    // Its group and item as saved, before the order arranged in Start (see
    // ItemSetting::rawGroup): what a drag in Start moves.
    int rawGroup = -1;
    int rawItem = -1;
    int rawSub = -1;
};

// A group item as configured.
struct ItemSetting {
    // "app", "program", "website" or "folder".
    std::wstring type = L"app";
    std::wstring app;            // Start app name or AUMID
    std::wstring label;          // optional display name override
    std::wstring path;           // program path
    std::wstring args;           // program arguments
    std::wstring icon;           // optional icon file (.ico/.exe/.dll/.png)
    // A folder's apps (saved as its "apps" list). Folders don't nest.
    std::vector<ItemSetting> children;
    // Where it is in the saved settings, before the order arranged in Start
    // is applied (see "Order arranged in Start"). Stays the same while it's
    // moved around in Start, until the settings are read again. An app in a
    // folder: rawSub, its index there (-1: an item of the group). A folder
    // made in Start: rawGroup -1, rawItem -2, -3, ...
    int rawGroup = -1;
    int rawItem = -1;
    int rawSub = -1;
};

// A folder's name when it has no label of its own.
constexpr PCWSTR kFolderName = L"Folder";

// Reads a list of items saved under `prefix` (e.g. "groups[0].items"):
// `get` returns a setting's value by its full name. A folder's apps are in
// its own "apps" list; folders inside a folder are left out.
std::vector<ItemSetting> ReadItems(
    std::function<std::wstring(std::wstring const&)> const& get,
    std::wstring const& prefix,
    bool folders = true) {
    std::vector<ItemSetting> items;
    for (int j = 0;; j++) {
        std::wstring ip = prefix + L"[" + std::to_wstring(j) + L"]";
        ItemSetting item;
        item.type = get(ip + L".type");
        item.app = get(ip + L".app");
        item.label = get(ip + L".label");
        item.path = get(ip + L".path");
        item.args = get(ip + L".args");
        item.icon = get(ip + L".icon");
        bool folder = item.type == L"folder";
        if (item.app.empty() && item.path.empty() && !folder) {
            break;
        }
        if (folder) {
            if (!folders) {
                continue;
            }
            item.children = ReadItems(get, ip + L".apps", false);
        } else if (item.type != L"program" && item.type != L"website") {
            item.type = L"app";
        }
        items.push_back(std::move(item));
    }
    return items;
}

// A group's tile: its size and the grid its apps are laid out in. Sizes
// count app spots: a cell of the group grid (the size of one of Start's own
// tiles) is 2 × 2 spots, and the grid is laid out in half cells, one per
// spot. The app grid is set separately, at most the size of the tile.
struct GroupLayout {
    int iconColumns = 2;
    int iconRows = 2;
    int spanHalfColumns = 2;  // spots across = half cells
    int spanHalfRows = 2;     // spots down = half rows
};

// Width of a tile spanning `halves` half cells: half cells are
// (tileSize + gap) / 2 wide, and the tile leaves out the trailing gap.
double TileWidth(int halves, double tileSize, double gap) {
    return halves * (tileSize + gap) / 2 - gap;
}

// A row holds as many tiles as fit, and at least as many as roughly fill a
// default-width Start menu, which holds 3 default tiles (154 wide, 30
// apart): 3 default, 4 small or 2 large tiles.
constexpr double kReferenceRowWidth = 3 * 154 + 2 * 30;

// Half cells per row of a default-width Start menu: whole tiles, rounded
// down, so big tiles (the Large preset) don't make the row wider than Start.
int MinRowHalves(double tileSize, double gap) {
    int n = (int)std::floor((kReferenceRowWidth + gap) / (tileSize + gap) +
                            0.05);
    return 2 * std::max(n, 1);
}

// How much wider or narrower than set a tile may be made so that its row
// fills Start's width. Past that (a Start narrower than the fewest tiles a
// row holds), the Viewbox around the row shrinks it to fit.
constexpr double kMinTileFit = 0.85, kMaxTileFit = 1.15;

// A row of the group grid fitted to a `width` wide space: the number of
// half cells closest to what fits, and the tiles made a little wider or
// narrower so the row ends exactly at the space's right edge, like Start's
// own grids. The gaps stay as set, and so do the tiles' heights.
struct RowFit {
    int halves;   // half cells per row
    double tile;  // a tile's size across (one cell, without its gap)
    double half;  // a half cell: (tile + gap) / 2
};

RowFit FitRow(double width, double tileSize, double gap) {
    int halves = MinRowHalves(tileSize, gap);
    double tile = tileSize;
    if (width > 0) {
        double nominal = (tileSize + gap) / 2;
        halves =
            std::max(halves, (int)std::lround((width + gap) / nominal));
        tile = std::clamp(2 * (width + gap) / halves - gap,
                          tileSize * kMinTileFit, tileSize * kMaxTileFit);
    }
    return {halves, tile, (tile + gap) / 2};
}

// Width of the frame holding a row of `halves` half cells: the tiles and
// the gaps between them. (The grid inside gets 1 px of slack, see
// kRowSlack, so layout rounding can't drop the last cell of a row.)
double RowFrameWidth(int halves, double half, double gap) {
    return halves * half - gap;
}
constexpr double kRowSlack = 1;

struct GroupSetting {
    std::wstring name;
    GroupLayout layout;
    bool showAppNames = true;
    bool showLabel = true;       // the name under the tile
    bool showBackground = true;  // off: no background, border or padding
    std::vector<ItemSetting> items;
    // Its index in the saved settings (see ItemSetting::rawGroup). Made in
    // Start, not saved yet: -2, -3, ... (see MakeStartGroup).
    int raw = -1;
};

// Read on the Windhawk thread, snapshotted by workers under the mutex.
struct ModSettings {
    LayoutConfig layout;
    // In the order arranged in Start, if any (see "Order arranged in Start").
    std::vector<GroupSetting> groups;
    // Keys of the groups and items as saved (by raw index), to save the
    // order arranged in Start. `generation` changes each time the settings
    // are read: raw indices of one reading don't apply to another.
    std::vector<std::wstring> rawGroupKeys;
    std::vector<std::vector<std::wstring>> rawItemKeys;
    std::vector<std::vector<std::vector<std::wstring>>> rawChildKeys;
    int generation = 0;
    // Where the groups go among Start's sections: afterPinned, beforePinned,
    // afterRecent or afterAll.
    std::wstring placement = L"beforePinned";
    std::vector<std::wstring> pinnedHeaders;
    std::vector<std::wstring> recommendedHeaders;
    std::vector<std::wstring> allHeaders;
    std::vector<std::wstring> startSettingsTexts;  // Start's menu item
    // Start's own sections.
    bool showPinned = true;
    bool showRecommended = true;
    bool showAllApps = true;
};
std::mutex g_settingsMutex;
ModSettings g_settings;

// --- Order arranged in Start ------------------------------------------------
// Groups and apps rearranged by dragging them in Start. Start can't change
// the mod's settings (they're in a protected registry key, see
// WriteModSettings), so the order is kept in the mod's own storage instead,
// which needs no administrator prompt, and applied on top of the settings
// whenever they're read: by Start, and by the settings window, which saves
// it into the settings (and then removes it).
//
// It's the order of the groups and of each group's items, by key: a group by
// its name, an item by all of its settings (each with the number of earlier
// ones alike, for duplicates), so it still fits after the settings change in
// Windhawk; an app in a folder by its folder's key and its own settings.
// Groups and items it doesn't list (new ones) stay where they are in the
// settings; keys that are no longer there are skipped.
//
// Stored as lines: a "v1" header, then "g\t<key>" for a group followed by
// "i\t<key>" for each of its items. A folder's item is followed by
// "c\t<key>" for each of its apps (apps of a folder not listed stay in it).
// Made in Start: an app dragged out of its group gets a group of its own
// (an app on its own, 1 x 1), "n\t<name>" instead of "g"; an app dropped
// onto another makes a folder of the two, "f\t<name>" instead of "i".
// Either is dropped when its apps are gone.

constexpr PCWSTR kStartOrderValue = L"startOrder";

template <typename G>
std::vector<std::wstring> OrderGroupKeys(std::vector<G> const& groups) {
    std::vector<std::wstring> keys;
    std::unordered_map<std::wstring, int> seen;
    for (auto const& g : groups) {
        int n = seen[g.name]++;
        keys.push_back(g.name + L'\x1F' + std::to_wstring(n));
    }
    return keys;
}

// An item's settings, for its key.
std::wstring ItemKeyBase(ItemSetting const& it) {
    return it.type + L'\x1F' + it.app + L'\x1F' + it.path + L'\x1F' +
           it.args + L'\x1F' + it.label + L'\x1F' + it.icon;
}

template <typename G>
std::vector<std::vector<std::wstring>> OrderItemKeys(
    std::vector<G> const& groups) {
    std::vector<std::vector<std::wstring>> keys;
    std::unordered_map<std::wstring, int> seen;
    for (auto const& g : groups) {
        auto& list = keys.emplace_back();
        for (auto const& it : g.items) {
            std::wstring c = ItemKeyBase(it);
            int n = seen[c]++;
            list.push_back(c + L'\x1F' + std::to_wstring(n));
        }
    }
    return keys;
}

// The keys of the folders' apps: [group][item][app].
using ChildKeys = std::vector<std::vector<std::vector<std::wstring>>>;
template <typename G>
ChildKeys OrderChildKeys(std::vector<G> const& groups) {
    auto items = OrderItemKeys(groups);
    ChildKeys keys;
    for (size_t g = 0; g < groups.size(); g++) {
        auto& gl = keys.emplace_back();
        for (size_t i = 0; i < groups[g].items.size(); i++) {
            auto& list = gl.emplace_back();
            std::unordered_map<std::wstring, int> seen;
            for (auto const& c : groups[g].items[i].children) {
                std::wstring base = ItemKeyBase(c);
                int n = seen[base]++;
                list.push_back(items[g][i] + L'\x1E' + base + L'\x1F' +
                               std::to_wstring(n));
            }
        }
    }
    return keys;
}

// The value, or "" when there's none. (Wh_GetStringValue gives "" also when
// the buffer is too small.)
std::wstring ReadStartOrderValue() {
    for (size_t size : {size_t{1} << 12, size_t{1} << 16, size_t{1} << 20}) {
        std::wstring buf(size, L'\0');
        size_t n = Wh_GetStringValue(kStartOrderValue, buf.data(), size);
        if (n > 0 && n < size) {
            buf.resize(n);
            return buf;
        }
    }
    return L"";
}

std::wstring EscapeOrderKey(std::wstring const& s) {
    std::wstring out;
    for (WCHAR c : s) {
        switch (c) {
            case L'\\': out += L"\\\\"; break;
            case L'\n': out += L"\\n"; break;
            case L'\r': out += L"\\r"; break;
            case L'\t': out += L"\\t"; break;
            default: out += c;
        }
    }
    return out;
}

std::wstring UnescapeOrderKey(std::wstring_view s) {
    std::wstring out;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] != L'\\' || i + 1 == s.size()) {
            out += s[i];
            continue;
        }
        WCHAR c = s[++i];
        out += c == L'n' ? L'\n' : c == L'r' ? L'\r' : c == L't' ? L'\t' : c;
    }
    return out;
}

// An item as saved: group, item and, for an app in a folder, its index
// there (-1: the item itself).
struct OrderedRef {
    int g = -1, i = -1, sub = -1;
    bool operator==(OrderedRef const&) const = default;
};

// An item in the order: a saved one (ref), or a folder made in Start
// (ref.g -1, named newName). A folder's apps are `children`.
struct OrderedItem {
    OrderedRef ref;
    bool folder = false;
    std::wstring newName;
    std::vector<OrderedRef> children;
};

// The order: the groups by their raw (saved) index, each with its items.
struct OrderedGroup {
    int group;  // -1: made in Start (an app on its own, named newName)
    std::vector<OrderedItem> items;
    std::wstring newName;
};

// The order `value` gives the groups as saved (`raw`). Always every group and
// every item (and every app of a folder), each once.
template <typename G>
std::vector<OrderedGroup> PlanStartOrder(std::vector<G> const& raw,
                                         std::wstring const& value) {
    const int n = (int)raw.size();
    std::vector<OrderedGroup> plan;
    std::vector<char> groupUsed(n);
    std::vector<std::vector<char>> itemUsed(n);
    std::vector<std::vector<std::vector<char>>> childUsed(n);
    for (int g = 0; g < n; g++) {
        itemUsed[g].resize(raw[g].items.size());
        childUsed[g].resize(raw[g].items.size());
        for (int i = 0; i < (int)raw[g].items.size(); i++) {
            childUsed[g][i].resize(raw[g].items[i].children.size());
        }
    }
    auto isFolder = [&](OrderedRef r) {
        return r.sub < 0 && raw[r.g].items[r.i].type == L"folder";
    };
    auto use = [&](OrderedRef r) -> bool {
        char& u = r.sub < 0 ? itemUsed[r.g][r.i] : childUsed[r.g][r.i][r.sub];
        if (u) {
            return false;
        }
        u = 1;
        return true;
    };

    if (value.rfind(L"v1\n", 0) == 0) {
        auto groupKeys = OrderGroupKeys(raw);
        auto itemKeys = OrderItemKeys(raw);
        auto childKeys = OrderChildKeys(raw);
        std::unordered_map<std::wstring, int> groupByKey;
        std::unordered_map<std::wstring, OrderedRef> itemByKey;
        for (int g = 0; g < n; g++) {
            groupByKey.emplace(groupKeys[g], g);
            for (int i = 0; i < (int)itemKeys[g].size(); i++) {
                itemByKey.emplace(itemKeys[g][i], OrderedRef{g, i, -1});
                for (int c = 0; c < (int)childKeys[g][i].size(); c++) {
                    itemByKey.emplace(childKeys[g][i][c], OrderedRef{g, i, c});
                }
            }
        }
        // Indices, not pointers: the vectors grow.
        int cur = -1;        // group in plan
        int curFolder = -1;  // folder item in plan[cur]
        size_t pos = 3;
        while (pos < value.size()) {
            size_t end = value.find(L'\n', pos);
            if (end == std::wstring::npos) {
                end = value.size();
            }
            std::wstring_view line(value.data() + pos, end - pos);
            pos = end + 1;
            if (line.size() < 2 || line[1] != L'\t') {
                continue;
            }
            std::wstring key = UnescapeOrderKey(line.substr(2));
            WCHAR kind = line[0];
            if (kind == L'g') {
                cur = -1;
                curFolder = -1;
                auto it = groupByKey.find(key);
                if (it != groupByKey.end() && !groupUsed[it->second]) {
                    groupUsed[it->second] = 1;
                    plan.push_back(OrderedGroup{it->second, {}, {}});
                    cur = (int)plan.size() - 1;
                }
            } else if (kind == L'n') {
                plan.push_back(OrderedGroup{-1, {}, key});
                cur = (int)plan.size() - 1;
                curFolder = -1;
            } else if (cur < 0) {
                continue;
            } else if (kind == L'i') {
                curFolder = -1;
                auto it = itemByKey.find(key);
                if (it != itemByKey.end() && use(it->second)) {
                    OrderedItem item;
                    item.ref = it->second;
                    item.folder = isFolder(it->second);
                    plan[cur].items.push_back(std::move(item));
                    if (item.folder) {
                        curFolder = (int)plan[cur].items.size() - 1;
                    }
                }
            } else if (kind == L'f') {
                OrderedItem item;
                item.folder = true;
                item.newName = key;
                plan[cur].items.push_back(std::move(item));
                curFolder = (int)plan[cur].items.size() - 1;
            } else if (kind == L'c' && curFolder >= 0) {
                // Folders don't nest.
                auto it = itemByKey.find(key);
                if (it != itemByKey.end() && !isFolder(it->second) &&
                    use(it->second)) {
                    plan[cur].items[curFolder].children.push_back(it->second);
                }
            }
        }
    }

    // Groups it doesn't list: after the group before them in the settings.
    for (int g = 0; g < n; g++) {
        if (groupUsed[g]) {
            continue;
        }
        size_t at = 0;
        for (size_t k = 0; k < plan.size(); k++) {
            if (g > 0 && plan[k].group == g - 1) {
                at = k + 1;
                break;
            }
        }
        plan.insert(plan.begin() + at, OrderedGroup{g, {}, {}});
        groupUsed[g] = 1;
    }
    // Items it doesn't list: at the end of their own group.
    for (auto& og : plan) {
        if (og.group < 0) {
            continue;
        }
        for (int i = 0; i < (int)raw[og.group].items.size(); i++) {
            if (use({og.group, i, -1})) {
                OrderedItem item;
                item.ref = {og.group, i, -1};
                item.folder = isFolder(item.ref);
                og.items.push_back(std::move(item));
            }
        }
    }
    // Apps of a folder it doesn't list: at the end of their folder.
    for (auto& og : plan) {
        for (auto& item : og.items) {
            if (!item.folder || item.ref.g < 0) {
                continue;
            }
            auto const& f = raw[item.ref.g].items[item.ref.i];
            for (int c = 0; c < (int)f.children.size(); c++) {
                if (use({item.ref.g, item.ref.i, c})) {
                    item.children.push_back({item.ref.g, item.ref.i, c});
                }
            }
        }
    }
    // Made in Start, with all of their apps gone.
    for (auto& og : plan) {
        std::erase_if(og.items, [](OrderedItem const& item) {
            return item.ref.g < 0 && item.children.empty();
        });
    }
    std::erase_if(plan, [](OrderedGroup const& og) {
        return og.group < 0 && og.items.empty();
    });
    return plan;
}

// The groups in the order `plan` gives them. A group made in Start is set up
// by MakeStartGroup (an overload for each G, found when this is used). A
// folder made in Start gets rawGroup -1 and rawItem -2, -3, ..., which tells
// them apart (see ItemSetting::rawSub).
template <typename G>
std::vector<G> ApplyStartOrder(std::vector<G> const& raw,
                               std::vector<OrderedGroup> const& plan) {
    std::vector<G> out;
    out.reserve(plan.size());
    int made = 0, madeFolders = 0;
    auto at = [&](OrderedRef r) -> ItemSetting const& {
        auto const& item = raw[r.g].items[r.i];
        return r.sub < 0 ? item : item.children[r.sub];
    };
    for (auto const& og : plan) {
        G g = og.group >= 0 ? raw[og.group]
                            : MakeStartGroup(og.newName, made++, (G*)nullptr);
        g.items.clear();
        for (auto const& oi : og.items) {
            ItemSetting item;
            if (oi.ref.g >= 0) {
                item = at(oi.ref);
            } else {
                item.type = L"folder";
                item.label = oi.newName;
                item.rawGroup = -1;
                item.rawItem = -2 - madeFolders++;
            }
            if (oi.folder) {
                item.children.clear();
                for (auto r : oi.children) {
                    item.children.push_back(at(r));
                }
                // A folder whose apps were all dragged out of it in Start.
                if (item.children.empty() && oi.ref.g >= 0 &&
                    !at(oi.ref).children.empty()) {
                    continue;
                }
            }
            g.items.push_back(std::move(item));
        }
        // An app on its own whose app was dragged into a group in Start.
        if (g.items.empty() && og.group >= 0 &&
            !raw[og.group].items.empty() && IsSingleApp(g)) {
            continue;
        }
        out.push_back(std::move(g));
    }
    return out;
}

// The value for the order of `s.groups` (moved around in Start), or "" when
// it's the saved order.
std::wstring StartOrderValue(ModSettings const& s) {
    const int n = (int)s.rawGroupKeys.size();
    bool same = (int)s.groups.size() == n;
    std::wstring out = L"v1\n";
    // An item's key; false if it isn't one of the saved ones.
    auto key = [&](ItemSetting const& it, std::wstring& k) {
        if (it.rawGroup < 0 || it.rawGroup >= n || it.rawItem < 0 ||
            it.rawItem >= (int)s.rawItemKeys[it.rawGroup].size()) {
            return false;
        }
        if (it.rawSub < 0) {
            k = s.rawItemKeys[it.rawGroup][it.rawItem];
            return true;
        }
        auto const& c = s.rawChildKeys[it.rawGroup][it.rawItem];
        if (it.rawSub >= (int)c.size()) {
            return false;
        }
        k = c[it.rawSub];
        return true;
    };
    for (int k = 0; k < (int)s.groups.size(); k++) {
        auto const& g = s.groups[k];
        if (g.raw >= n) {
            return L"";
        }
        if (g.raw < 0) {
            // Made in Start (see MakeStartGroup).
            same = false;
            out += L"n\t" + EscapeOrderKey(g.name) + L"\n";
        } else {
            same = same && g.raw == k &&
                   g.items.size() == s.rawItemKeys[k].size();
            out += L"g\t" + EscapeOrderKey(s.rawGroupKeys[g.raw]) + L"\n";
        }
        for (int j = 0; j < (int)g.items.size(); j++) {
            auto const& it = g.items[j];
            std::wstring itemKey;
            if (it.rawGroup < 0 && it.type == L"folder") {
                // Made in Start.
                same = false;
                out += L"f\t" + EscapeOrderKey(it.label) + L"\n";
            } else if (key(it, itemKey)) {
                same = same && it.rawGroup == k && it.rawItem == j &&
                       it.rawSub < 0;
                out += L"i\t" + EscapeOrderKey(itemKey) + L"\n";
            } else {
                return L"";
            }
            if (it.type != L"folder") {
                continue;
            }
            same = same && it.rawSub < 0 && it.rawGroup == k &&
                   it.rawItem == j &&
                   it.children.size() == s.rawChildKeys[k][j].size();
            for (int c = 0; c < (int)it.children.size(); c++) {
                auto const& child = it.children[c];
                std::wstring childKey;
                if (!key(child, childKey)) {
                    return L"";
                }
                same = same && child.rawGroup == k && child.rawItem == j &&
                       child.rawSub == c;
                out += L"c\t" + EscapeOrderKey(childKey) + L"\n";
            }
        }
    }
    return same ? L"" : out;
}

struct Group {
    std::wstring name;
    GroupLayout layout;
    bool showAppNames = true;
    bool showLabel = true;       // the name under the tile
    bool showBackground = true;  // off: no background, border or padding
    std::vector<AppEntry> apps;
    int rawGroup = -1;  // see GroupSetting::raw
};

// The tile sizes offered: "<spots across>x<spots down>".
constexpr PCWSTR kTileSizes[] = {
    L"1x1", L"1x2", L"1x3", L"1x4", L"1x5", L"1x6",
    L"2x1", L"3x1", L"4x1", L"5x1", L"6x1",
    L"2x2", L"3x2", L"3x3", L"4x2", L"4x4", L"6x2", L"6x4"};
// App grids: up to 6 apps per row and 4 rows, within the tile's size.
// Saved as "tileSize", the setting's name from before tiles had a size of
// their own.
constexpr int kMaxGridColumns = 6, kMaxGridRows = 6;
// The default app grid: one app per spot of the tile, whatever its size.
constexpr PCWSTR kAutoGrid = L"auto";

constexpr PCWSTR kAppGridHint =
    L"Apps per row \u00D7 rows of apps, at most the tile size. Default "
    L"fills the tile, one app per spot, and follows its size. The rest go "
    L"in the overflow cluster.";

constexpr PCWSTR kTileSizeHint =
    L"Width \u00D7 height, in app spots: 2 \u00D7 2 is the size of one "
    L"of Start's own tiles.";

// "Columns when a group is opened".
constexpr int kMinExpandedColumns = 2, kMaxExpandedColumns = 5;

constexpr PCWSTR kShowNamesHint =
    L"Each app's name under its icon in this tile. Needs \"Show app "
    L"names\" on in Behavior.";

bool IsTileSize(std::wstring const& s) {
    return std::any_of(std::begin(kTileSizes), std::end(kTileSizes),
                       [&](PCWSTR t) { return s == t; });
}

// "3x2" -> 3, 2. False when it isn't "<number>x<number>".
bool ParseAxB(std::wstring const& s, int& a, int& b) {
    size_t x = s.find(L'x');
    if (x == std::wstring::npos) {
        return false;
    }
    a = _wtoi(s.c_str());
    b = _wtoi(s.c_str() + x + 1);
    return a > 0 && b > 0;
}

std::wstring AxB(int a, int b) {
    return std::to_wstring(a) + L"x" + std::to_wstring(b);
}

// An app grid made to fit a tile size: fewer columns or rows if needed.
// Unknown grids fill the tile.
// "auto" (the default) and unknown grids fill the tile, and keep following
// its size.
std::wstring FitGrid(std::wstring const& grid, std::wstring const& size) {
    int w = 2, h = 2, c, r;
    if (IsTileSize(size)) {
        ParseAxB(size, w, h);
    }
    if (grid == kAutoGrid || !ParseAxB(grid, c, r)) {
        return kAutoGrid;
    }
    return AxB(std::min(c, w), std::min(r, h));
}

// A tile size and app grid -> the tile's layout. Unknown sizes are 2 x 2.
GroupLayout MakeLayout(std::wstring const& size, std::wstring const& grid) {
    GroupLayout l;
    int w = 2, h = 2;
    if (IsTileSize(size)) {
        ParseAxB(size, w, h);
    }
    l.spanHalfColumns = w;
    l.spanHalfRows = h;
    if (!ParseAxB(FitGrid(grid, size), l.iconColumns, l.iconRows)) {
        l.iconColumns = std::min(w, kMaxGridColumns);  // auto: one app per spot
        l.iconRows = std::min(h, kMaxGridRows);
    }
    return l;
}

// An app on its own: a one-spot (1 x 1) tile (see the EditGroup overload).
bool IsSingleApp(GroupSetting const& g) {
    return g.layout.spanHalfColumns == 1 && g.layout.spanHalfRows == 1;
}

// A group made in Start, the `index`-th (see "Order arranged in Start"): an
// app on its own, one spot without a name or background. Not saved in the
// settings: its raw index is -2, -3, ..., which tells the groups apart.
GroupSetting MakeStartGroup(std::wstring const& name, int index,
                            GroupSetting*) {
    GroupSetting g;
    g.name = name;
    g.layout = MakeLayout(L"1x1", kAutoGrid);
    g.showLabel = false;
    g.showBackground = false;
    g.raw = -2 - index;
    return g;
}

// A group's saved tile size. Settings from before tiles had a size of
// their own have none: the smallest size that holds the app grid.
std::wstring ResolveTileSize(std::wstring const& size,
                             std::wstring const& grid) {
    if (IsTileSize(size)) {
        return size;
    }
    int c = 2, r = 2;
    ParseAxB(grid, c, r);
    std::wstring best = L"2x2";
    int bestArea = INT_MAX;
    for (PCWSTR t : kTileSizes) {
        int w, h;
        ParseAxB(t, w, h);
        if (w >= c && h >= r && w * h < bestArea) {
            bestArea = w * h;
            best = t;
        }
    }
    return best;
}

// Height of a tile spanning `halfRows` half rows of the group grid: a row
// is a tile, its name and the gap below; the tile leaves out the last two.
double TileHeight(int halfRows, double tileSize, double gap, double labelH) {
    return halfRows * (tileSize + gap + labelH) / 2 - gap - labelH;
}

// A tile's size: across, in cells of the row it's in (`tileAcross`, the
// tile size fitted to the row, see FitRow); down, in rows of the set tile
// size. A tile whose name isn't shown also takes the space the name would
// have (it's taller). Every tile stays within its cells, so the gaps between
// tiles are the same everywhere and a row never gets wider than its cells.
struct TileBox {
    double width, height;
};

TileBox TileBoxFor(int halves,
                   int halfRows,
                   double tileAcross,
                   double tileSize,
                   double gapX,
                   double gapY,
                   double labelH,
                   bool nameShown) {
    TileBox b{TileWidth(halves, tileAcross, gapX),
              TileHeight(halfRows, tileSize, gapY, labelH)};
    if (!nameShown) {
        b.height += labelH;
    }
    return b;
}

// How a tile's apps are drawn. The tile keeps the same padding on all
// sides. A short tile (one spot high) has no room for the set padding
// around an icon and its name: the apps' own vertical padding goes first,
// then the icons get smaller. The names stay.
// App names follow the icon size (set by the size preset), in proportion:
// 11 px with the default 32 px icons, 22 px with the Large preset's 64 px.
double AppNameFontSize(double iconSize) {
    return std::clamp(std::round(11 * iconSize / 32), 9.0, 24.0);
}

// The opened group: its icons are at least the default size and follow
// the preset's, and its cells and names grow with them.
struct FlyoutLook {
    double icon, cell, font, textWidth;
};

FlyoutLook FlyoutLookFor(double iconSize) {
    double icon = std::clamp(iconSize, 32.0, 64.0);
    double k = icon / 32;
    double cell = std::round(96 * k);
    return {icon, cell, std::round(12 * k), cell - 12 * k};
}

struct TileFit {
    double iconSize;
    double padX, padY;  // the tile's padding
    double appPadY;     // each app's top and bottom padding
    double nameGap;     // between an icon and its name
};

TileFit FitTile(double iconSize,
                double tileW,
                double tileH,
                double padding,
                double gap,
                GroupLayout const& l,
                double appPadX,
                double appPadY,
                bool names) {
    // A tile one spot high is short: its names sit closer to the icons.
    const double nameGap = l.spanHalfRows == 1 ? 1 : 4;
    const double nameH = names ? nameGap + AppNameFontSize(iconSize) * 1.4 : 0;
    const int rows = std::max(1, l.iconRows);
    const int cols = std::max(1, l.iconColumns);
    TileFit f{iconSize, 0, 0, appPadY, nameGap};
    // Just enough padding for one icon in a narrow tile.
    f.padX = std::clamp((tileW - iconSize - 2 * appPadX) / 2, 0.0, padding);
    double need = iconSize + 2 * appPadY + nameH;
    f.padY = f.padX;
    double rowH = (tileH - 2 * f.padY - (rows - 1) * gap) / rows;
    if (rowH < need) {
        f.appPadY = std::clamp((rowH - iconSize - nameH) / 2, 0.0, appPadY);
    }
    double cw = (tileW - 2 * f.padX - (cols - 1) * gap) / cols - 2 * appPadX;
    double ch = rowH - 2 * f.appPadY - nameH;
    double fit = std::floor(std::min(cw, ch));
    f.iconSize = fit >= iconSize ? iconSize : std::max(12.0, fit);
    return f;
}

struct IconPixels {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> bgra;
};

struct Model {
    LayoutConfig layout;
    std::vector<Group> groups;
    std::vector<AppEntry> allApps;  // every Start app (settings window picker)
    std::unordered_map<std::wstring, std::shared_ptr<IconPixels>> icons;
    int settingsGeneration = 0;  // ModSettings::generation it was built from
};

std::mutex g_iconCacheMutex;
std::map<std::pair<std::wstring, int>, std::shared_ptr<IconPixels>>
    g_iconCache;

// ---------------------------------------------------------------------------
// Globals for the UI side (UI thread only unless noted)
// ---------------------------------------------------------------------------

std::atomic<bool> g_unloading{false};
// The model of the groups in Start. Not XAML, but released on the UI thread
// with everything else (Detach / UninstallStartUi), never at process exit.
[[clang::no_destroy]] std::shared_ptr<Model> g_model;  // UI thread

// A tile in Start, for dragging (see "Rearranging in Start").
struct StartTile {
    int group = -1;  // in g_model->groups
    winrt::weak_ref<FrameworkElement> outer;  // the tile and its name
    winrt::weak_ref<xc::Border> tile;
    std::vector<std::pair<winrt::weak_ref<xc::Button>, int>> apps;
    winrt::weak_ref<xc::Button> cluster;
};

// A group or an app being dragged in Start.
struct StartDrag {
    bool pressed = false;  // the left mouse button went down on a tile
    bool active = false;   // moved far enough to be a drag
    bool group = false;    // a whole tile, rather than one of its apps
    int tile = -1;         // in model->groups
    int app = -1;          // in its apps (app drags)
    std::shared_ptr<Model> model;  // the one the tiles were built from
    wf::Point start{};
    wf::Point grab{};  // where the ghost was taken, from its top left
    double scale = 1;  // the groups are shrunk to fit Start (Viewbox)
    Input::Pointer pointer{nullptr};
    winrt::weak_ref<UIElement> source;  // dimmed while it's dragged
    FrameworkElement ghost{nullptr};    // follows the pointer
    xc::Border indicator{nullptr};      // where it goes
    // Where it goes: before or after an app (targetApp) or a tile, or at
    // the end of a group (targetApp -1). targetTile -1: nowhere. newGroup:
    // an app out of its group, into a group of its own before or after the
    // tile targetTile.
    int targetTile = -1;
    int targetApp = -1;
    bool after = false;
    bool newGroup = false;
    // An app on its own (a group drag) into the group targetTile, as an app
    // drag goes; its own tile goes.
    bool intoGroup = false;
    // Onto the app targetApp: a folder of the two (makeFolder), or into it,
    // a folder (intoFolder), before or after its app targetChild (-1: last).
    bool makeFolder = false;
    bool intoFolder = false;
    int targetChild = -1;
    // An app of the folder `app` (its index there), dragged out of the
    // opened folder.
    int child = -1;
    // In the opened folder or group (StartFlyout), until it's dragged out.
    bool inFlyout = false;
};

// An app's identity in the settings (see ItemSetting::rawSub).
using RawId = std::tuple<int, int, int>;
RawId IdOf(AppEntry const& a) {
    return {a.rawGroup, a.rawItem, a.rawSub};
}
RawId IdOf(ItemSetting const& a) {
    return {a.rawGroup, a.rawItem, a.rawSub};
}

// The opened group or folder in Start (BuildGroupFlyout), for dragging its
// apps (see "Rearranging in Start"): the apps of group `tile`, from `first`
// on, or of its folder `folder` (an app index).
struct StartFlyout {
    std::shared_ptr<Model> model;
    int tile = -1;
    int folder = -1;
    size_t first = 0;
    int cols = 1;
    // To find it again in a rebuilt model (RefreshStartFlyout).
    int groupRaw = -1;
    RawId folderId{-1, -1, -1};
    winrt::weak_ref<FrameworkElement> anchor;
    winrt::weak_ref<xc::Flyout> fly;
    winrt::weak_ref<xc::Grid> root;  // drag coordinates are relative to it
    winrt::weak_ref<xc::Canvas> overlay;
    // Its apps' buttons, with their index in the list shown.
    std::vector<std::pair<winrt::weak_ref<xc::Button>, int>> items;
};

// Everything below holds XAML objects of Start's UI thread. They're released
// there, by Detach and UninstallStartUi; [[clang::no_destroy]] keeps the
// CRT from releasing them again at process exit, off the UI thread and after
// XAML has shut down (Wh_ModUninit doesn't run then).
struct Attachment {
    // The pinned list's row in Start's header grid: the groups go around it.
    winrt::weak_ref<FrameworkElement> target;
    winrt::weak_ref<xc::Panel> parent;
    FrameworkElement host{nullptr};
    // Row inserted into Start's "TopLevelHeader" grid, or (afterAll) the
    // All list whose footer holds the groups, below that grid's rows.
    winrt::weak_ref<xc::Grid> rowGrid;
    int insertedRow = -1;
    winrt::weak_ref<xc::ListViewBase> footerList;
    winrt::weak_ref<xc::TextBlock> pinnedHeader;  // style source for heading
    winrt::event_token themeToken{};
    // Start's own sections hidden by the "Show the ... section" settings,
    // with the visibility to restore.
    std::vector<std::pair<winrt::weak_ref<UIElement>, Visibility>> hidden;
    // Unloaded handlers on the elements we're attached to: Start rebuilds
    // parts of its tree (e.g. when switching the "All" view), and the
    // attachment must then be checked again.
    std::vector<std::pair<winrt::weak_ref<FrameworkElement>, winrt::event_token>>
        unloadedTokens;
    // The group grid and its frame.
    winrt::weak_ref<xc::VariableSizedWrapGrid> wrap;
    winrt::weak_ref<xc::Border> wrapFrame;
    // The host width the tiles were built for (whole DIPs, see HostRowWidth;
    // 0: before the host was laid out): the row is fitted to it (FitRow),
    // and built again when it changes.
    double builtWidth = -1;
    // The "Where to show the groups" setting it was attached for (before a
    // fallback), to move the groups when it changes.
    std::wstring placement;
    winrt::event_token sizeToken{};
    // Rearranging in Start: the tiles, the element holding the groups (drag
    // coordinates are relative to it), the layer the drag draws on, and the
    // drag.
    std::vector<StartTile> tiles;
    winrt::weak_ref<xc::Grid> dragRoot;
    winrt::weak_ref<xc::Canvas> dragOverlay;
    StartDrag drag;
    StartFlyout flyout;
    // The left inset of Start's "Pinned" title: the groups' spacing left and
    // right unless set (see HostMargin).
    double inset = 0;
};
[[clang::no_destroy]] Attachment g_attach;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

double Clamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// The left and right spacing settings' range, in DIPs from Start's own.
constexpr int kMaxSpacing = 100;

bool ParseColor(const std::wstring& s, winrt::Windows::UI::Color& out) {
    if (s.size() < 7 || s[0] != L'#') {
        return false;
    }
    unsigned long v = wcstoul(s.c_str() + 1, nullptr, 16);
    if (s.size() == 7) {
        out = {255, (uint8_t)(v >> 16), (uint8_t)(v >> 8), (uint8_t)v};
        return true;
    }
    if (s.size() == 9) {
        out = {(uint8_t)(v >> 24), (uint8_t)(v >> 16), (uint8_t)(v >> 8),
               (uint8_t)v};
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Shell: app enumeration, icons, launching (worker threads)
// ---------------------------------------------------------------------------

std::wstring ExpandEnv(const std::wstring& s) {
    if (s.find(L'%') == std::wstring::npos) {
        return s;
    }
    WCHAR buf[2048];
    DWORD n = ExpandEnvironmentStringsW(s.c_str(), buf, ARRAYSIZE(buf));
    return n && n <= ARRAYSIZE(buf) ? std::wstring(buf) : s;
}

AppEntry MakeStartAppEntry(std::wstring id, std::wstring name) {
    AppEntry e;
    e.iconSource = L"shell:AppsFolder\\" + id;
    e.key = L"a:" + id;
    e.id = std::move(id);
    e.name = std::move(name);
    return e;
}

std::vector<AppEntry> EnumerateStartApps() {
    std::vector<AppEntry> apps;
    winrt::com_ptr<IShellItem> folder;
    if (FAILED(SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT,
                                    nullptr, IID_PPV_ARGS(folder.put())))) {
        return apps;
    }
    winrt::com_ptr<IEnumShellItems> items;
    if (FAILED(folder->BindToHandler(nullptr, BHID_EnumItems,
                                     IID_PPV_ARGS(items.put())))) {
        return apps;
    }
    winrt::com_ptr<IShellItem> item;
    while (!g_unloading && items->Next(1, item.put(), nullptr) == S_OK) {
        PWSTR name = nullptr;
        PWSTR id = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) &&
            SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &id))) {
            apps.push_back(MakeStartAppEntry(id, name));
        }
        CoTaskMemFree(name);
        CoTaskMemFree(id);
        item = nullptr;
    }
    return apps;
}

// `source` is any shell parsing name: shell:AppsFolder\<AUMID>, an .exe,
// .ico, .dll, .lnk or image file. The shell picks the right icon/thumbnail.
std::shared_ptr<IconPixels> ExtractAppIcon(const std::wstring& source,
                                           int size) {
    const std::wstring& id = source;  // cache key
    {
        std::lock_guard lock(g_iconCacheMutex);
        auto it = g_iconCache.find({id, size});
        if (it != g_iconCache.end()) {
            return it->second;
        }
    }

    std::shared_ptr<IconPixels> result;
    std::wstring path = ExpandEnv(source);
    winrt::com_ptr<IShellItemImageFactory> factory;
    HBITMAP bmp = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                              IID_PPV_ARGS(factory.put()))) &&
        SUCCEEDED(factory->GetImage({size, size}, SIIGBF_ICONONLY, &bmp))) {
        BITMAP bm{};
        GetObjectW(bmp, sizeof(bm), &bm);
        if (bm.bmWidth > 0 && bm.bmHeight > 0) {
            BITMAPINFO bi{};
            bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
            bi.bmiHeader.biWidth = bm.bmWidth;
            bi.bmiHeader.biHeight = -bm.bmHeight;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            auto px = std::make_shared<IconPixels>();
            px->width = bm.bmWidth;
            px->height = bm.bmHeight;
            px->bgra.resize((size_t)bm.bmWidth * bm.bmHeight * 4);
            HDC dc = GetDC(nullptr);
            int lines = GetDIBits(dc, bmp, 0, bm.bmHeight, px->bgra.data(),
                                  &bi, DIB_RGB_COLORS);
            ReleaseDC(nullptr, dc);
            if (lines > 0) {
                bool anyAlpha = false;
                for (size_t i = 3; i < px->bgra.size(); i += 4) {
                    if (px->bgra[i]) {
                        anyAlpha = true;
                        break;
                    }
                }
                if (!anyAlpha) {
                    for (size_t i = 3; i < px->bgra.size(); i += 4) {
                        px->bgra[i] = 255;
                    }
                } else {
                    // The bitmap is shown as premultiplied BGRA, but the
                    // shell often returns straight alpha. Straight alpha shows
                    // as a light halo around antialiased edges. A color
                    // channel above its alpha can't be premultiplied, so that
                    // means straight alpha: premultiply it here.
                    bool straight = false;
                    for (size_t i = 0; i + 3 < px->bgra.size(); i += 4) {
                        uint8_t a = px->bgra[i + 3];
                        if (px->bgra[i] > a || px->bgra[i + 1] > a ||
                            px->bgra[i + 2] > a) {
                            straight = true;
                            break;
                        }
                    }
                    if (straight) {
                        for (size_t i = 0; i + 3 < px->bgra.size(); i += 4) {
                            unsigned a = px->bgra[i + 3];
                            for (int c = 0; c < 3; c++) {
                                px->bgra[i + c] =
                                    (uint8_t)((px->bgra[i + c] * a + 127) / 255);
                            }
                        }
                    }
                }
                result = px;
            }
        }
        DeleteObject(bmp);
    }

    // Failures aren't cached: the shell may simply not be ready yet.
    if (result) {
        std::lock_guard lock(g_iconCacheMutex);
        g_iconCache[{id, size}] = result;
    }
    return result;
}

// --- Worker threads -----------------------------------------------------------
// Slow work (launching, the shell, icons, saving) runs on worker threads. Each
// one's handle is kept, and Wh_ModUninit waits for all of them, with no time
// limit, so no code of the mod is still running when it's unloaded. Long
// waits in a worker also wait on g_stopEvent, set when the mod unloads.

std::mutex g_workersMutex;
std::vector<HANDLE> g_workers;  // under g_workersMutex
HANDLE g_stopEvent = nullptr;   // manual reset; set by Wh_ModBeforeUninit

// Waits up to `ms`; false if the mod is unloading.
bool WaitOrStop(DWORD ms) {
    return WaitForSingleObject(g_stopEvent, ms) == WAIT_TIMEOUT;
}

DWORD WINAPI WorkerThread(LPVOID param) {
    std::unique_ptr<std::function<void()>> fn(
        static_cast<std::function<void()>*>(param));
    HRESULT hr =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    try {
        (*fn)();
    } catch (...) {
        Wh_Log(L"Worker failed: %08X", winrt::to_hresult());
    }
    fn.reset();
    if (SUCCEEDED(hr)) {
        CoUninitialize();
    }
    return 0;
}

void RunWorker(std::function<void()> fn) {
    std::lock_guard lock(g_workersMutex);
    if (g_unloading) {
        return;
    }
    // Forget the ones that are done.
    std::erase_if(g_workers, [](HANDLE h) {
        if (WaitForSingleObject(h, 0) == WAIT_OBJECT_0) {
            CloseHandle(h);
            return true;
        }
        return false;
    });
    auto* param = new std::function<void()>(std::move(fn));
    HANDLE h = CreateThread(nullptr, 0, WorkerThread, param, 0, nullptr);
    if (!h) {
        delete param;
        return;
    }
    g_workers.push_back(h);
}

// Waits for every worker to finish (Wh_ModUninit, after g_unloading is set,
// so no new ones start).
void JoinWorkers() {
    for (;;) {
        std::vector<HANDLE> handles;
        {
            std::lock_guard lock(g_workersMutex);
            handles.swap(g_workers);
        }
        if (handles.empty()) {
            return;
        }
        for (HANDLE h : handles) {
            WaitForSingleObject(h, INFINITE);
            CloseHandle(h);
        }
    }
}

// --- Running code on Start's UI thread ----------------------------------------
// A message-only window of the mod on Start's UI thread (see InstallStartUi):
// workers post to it, and its timer looks for Start's sections. Destroying it
// (UninstallStartUi) drops whatever is still queued, so nothing of the mod
// runs on that thread after the mod unloads.

constexpr UINT kRunOnUiMsg = WM_APP + 30;  // lParam: std::function<void()>*
constexpr UINT kUninstallMsg = WM_APP + 31;  // sent by Wh_ModUninit
std::atomic<HWND> g_uiWnd{nullptr};

// The mod's own flyouts in Start (opened groups, menus), to close them when
// the mod unloads (UI thread).
[[clang::no_destroy]] std::vector<winrt::weak_ref<xcp::FlyoutBase>> g_flyouts;

void TrackFlyout(xcp::FlyoutBase const& fly) {
    if (g_flyouts.size() > 64) {
        std::erase_if(g_flyouts, [](auto& w) { return !w.get(); });
    }
    g_flyouts.push_back(winrt::make_weak(fly));
}

// Runs `fn` on Start's UI thread, later. Dropped if the mod is unloading.
void PostToUi(std::function<void()> fn) {
    HWND wnd = g_uiWnd;
    if (!wnd || g_unloading) {
        return;
    }
    auto* param = new std::function<void()>(std::move(fn));
    if (!PostMessageW(wnd, kRunOnUiMsg, 0, (LPARAM)param)) {
        delete param;
    }
}

// A web address (http, https): a website's address or icon link.
bool IsWebAddress(std::wstring const& s) {
    return _wcsnicmp(s.c_str(), L"http://", 7) == 0 ||
           _wcsnicmp(s.c_str(), L"https://", 8) == 0;
}

// "https://www.example.com/page" -> "www.example.com", a website's default
// name.
std::wstring WebsiteHost(std::wstring const& url) {
    size_t start = url.find(L"://");
    start = start == std::wstring::npos ? 0 : start + 3;
    size_t end = url.find_first_of(L"/?#", start);
    std::wstring host = url.substr(start, end == std::wstring::npos
                                              ? std::wstring::npos
                                              : end - start);
    return host.empty() ? url : host;
}

// An icon given as text instead of a file: a Segoe Fluent Icons glyph by its
// code ("E774", "U+E774", "\\uE774" or "0xE774"), or an emoji (any short
// text with something other than plain letters, e.g. pasted). `glyph`: in
// the icon font.
bool ParseTextIcon(std::wstring const& s, std::wstring& text, bool& glyph) {
    std::wstring v = s;
    while (!v.empty() && iswspace(v.front())) {
        v.erase(0, 1);
    }
    while (!v.empty() && iswspace(v.back())) {
        v.pop_back();
    }
    if (v.empty()) {
        return false;
    }
    std::wstring hex = v;
    for (PCWSTR prefix : {L"\\u", L"U+", L"u+", L"0x", L"0X"}) {
        if (hex.rfind(prefix, 0) == 0) {
            hex = hex.substr(wcslen(prefix));
            break;
        }
    }
    if ((hex.size() == 4 || hex.size() == 5) &&
        std::all_of(hex.begin(), hex.end(), iswxdigit)) {
        unsigned cp = (unsigned)wcstoul(hex.c_str(), nullptr, 16);
        glyph = cp >= 0xE000 && cp <= 0xF8FF;
        if (cp > 0xFFFF) {
            cp -= 0x10000;
            text = {(wchar_t)(0xD800 + (cp >> 10)),
                    (wchar_t)(0xDC00 + (cp & 0x3FF))};
        } else {
            text = std::wstring(1, (wchar_t)cp);
        }
        return true;
    }
    // A pasted glyph, an emoji or a few symbols; not a path or address.
    bool special = std::any_of(v.begin(), v.end(),
                               [](wchar_t c) { return c > 0x7F; });
    if (v.size() <= 8 && special &&
        v.find_first_of(L"\\/:.") == std::wstring::npos) {
        glyph = std::all_of(v.begin(), v.end(), [](wchar_t c) {
            return c >= 0xE000 && c <= 0xF8FF;
        });
        text = v;
        return true;
    }
    return false;
}

bool IsTextIcon(std::wstring const& s) {
    std::wstring text;
    bool glyph;
    return ParseTextIcon(s, text, glyph);
}

// Start closes when the app it opens comes to the front. Windows lets an
// app do that only with the foreground window's consent: a new process
// usually gets it, but an app that's already running (e.g. Notepad opening
// another window) doesn't, and Start then stays open. Start is the
// foreground window here, so it hands that right on before launching.
void AllowLaunchedAppToFront() {
    AllowSetForegroundWindow(ASFW_ANY);
}

// Start's window, where the mod's UI is (set on Start's UI thread, by
// InstallStartUi).
HWND g_coreWnd = nullptr;

// Start's window: the CoreWindow of this process (StartMenuExperienceHost).
HWND FindStartWindow() {
    struct Find {
        DWORD pid;
        HWND found;
    } f{GetCurrentProcessId(), nullptr};
    EnumWindows(
        [](HWND hwnd, LPARAM lp) -> BOOL {
            auto* f = (Find*)lp;
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            WCHAR cls[64];
            if (pid == f->pid && GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
                wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                f->found = hwnd;
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&f);
    return f.found;
}

bool IsStartOpen(HWND start) {
    if (!start || !IsWindowVisible(start)) {
        return false;
    }
    DWORD cloaked = 0;
    DwmGetWindowAttribute(start, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    // Start is cloaked (by the shell) while closed. Not the mod's own
    // cloak, which hides it while it closes after a launch.
    return !(cloaked & ~DWM_CLOAKED_APP);
}

std::vector<xcp::Popup> OpenPopups();

// Hides or shows what Start shows, without closing or opening Start (UI
// thread): its XAML content and open popups (an opened group). Hiding
// plays Start's own closing animation, a slide down as it fades out.
// Start's window can't be cloaked (DWMWA_CLOAK has no effect on it), and
// Start draws all of itself, background included, in XAML.
void HideStartContent(bool hide) {
    try {
        std::vector<UIElement> els;
        if (auto window = Window::Current()) {
            if (auto content = window.Content()) {
                els.push_back(content);
            }
        }
        for (auto const& popup : OpenPopups()) {
            if (auto child = popup.Child()) {
                els.push_back(child);
            }
        }
        using namespace std::chrono_literals;
        namespace xh = winrt::Windows::UI::Xaml::Hosting;
        for (auto const& el : els) {
            auto v = xh::ElementCompositionPreview::GetElementVisual(el);
            xh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
            // Also an opened group's own closing animation, which would
            // keep it in place a while.
            v.StopAnimation(L"Opacity");
            v.StopAnimation(L"Translation");
            if (!hide) {
                v.Properties().InsertVector3(L"Translation", {0, 0, 0});
                v.Opacity(1.0f);
                continue;
            }
            auto comp = v.Compositor();
            // Windows' "accelerate" curve, as Start closes.
            auto ease = comp.CreateCubicBezierEasingFunction({0.7f, 0.0f},
                                                             {1.0f, 0.5f});
            const wf::TimeSpan len(200ms);
            auto slide = comp.CreateVector3KeyFrameAnimation();
            slide.InsertKeyFrame(0, {0, 0, 0});
            slide.InsertKeyFrame(1, {0, 120, 0}, ease);
            slide.Duration(len);
            auto fade = comp.CreateScalarKeyFrameAnimation();
            fade.InsertExpressionKeyFrame(0, L"this.StartingValue");
            fade.InsertKeyFrame(1, 0, ease);
            fade.Duration(len);
            v.StartAnimation(L"Translation", slide);
            v.StartAnimation(L"Opacity", fade);
        }
    } catch (...) {
    }
}

// Whether a key or mouse button is held down.
bool AnyKeyDown() {
    for (int vk = 1; vk < 256; vk++) {
        if (GetAsyncKeyState(vk) & 0x8000) {
            return true;
        }
    }
    return false;
}

// The windows an app shows on the taskbar: visible, not cloaked, and not a
// tool window, a window that's never activated, or an owned window.
bool IsAppWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) {
        return false;
    }
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) ||
        (GetWindow(hwnd, GW_OWNER) && !(ex & WS_EX_APPWINDOW))) {
        return false;
    }
    BOOL cloaked = FALSE;
    DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    RECT rc;
    return !cloaked && GetWindowRect(hwnd, &rc) && rc.right > rc.left &&
           rc.bottom > rc.top;
}

// The app windows of other processes, sorted.
std::vector<HWND> OtherAppWindows() {
    std::vector<HWND> list;
    EnumWindows(
        [](HWND hwnd, LPARAM lp) -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid != GetCurrentProcessId() && IsAppWindow(hwnd)) {
                ((std::vector<HWND>*)lp)->push_back(hwnd);
            }
            return TRUE;
        },
        (LPARAM)&list);
    std::sort(list.begin(), list.end());
    return list;
}

// Start doesn't always close after launching an app. Start closes when the
// app comes to the front, but an app that takes a while to show its window
// can lose the right to (Windows then only flashes its taskbar button). So
// while Start is still in front, the first new app window that shows up is
// brought to the front from here (Start, being in front, may do that), and
// Start closes as it loses the focus. If none shows up (e.g. Notepad,
// already running, opening a tab), Start is closed like the Start key does.
// Up to `seconds` (longer for an administrator prompt), and only while the
// normal desktop is in front (not the secure desktop of the prompt).
//
// Start can't really be closed at once: no key is pressed for that early,
// as any input takes away the right Windows gave the app to come to the
// front (AllowLaunchedAppToFront), and Start closing hands the focus back to
// the window before it, leaving the app behind it. So Start is hidden at
// once instead (its content made transparent, like Start disappears when it
// launches an app itself), and shown again once it's closed.
void CloseStartAfterLaunch(int seconds = 3) {
    // Taken now, before the launch, to tell the app's windows apart.
    auto before = std::make_shared<std::vector<HWND>>(OtherAppWindows());
    // The window the mod's UI is in. Not found by its class: this process
    // can have other CoreWindows, and a hidden one would look like Start
    // closed, leaving Start up until the app's window came to the front.
    HWND start = g_coreWnd && IsWindow(g_coreWnd) ? g_coreWnd
                                                   : FindStartWindow();
    if (!start || !IsStartOpen(start)) {
        return;
    }
    HideStartContent(true);
    RunWorker([seconds, before, start] {
        // Shown again once Start is closed (its own cloak then hides it),
        // or still open after all.
        struct Uncloak {
            HWND start;
            ~Uncloak() {
                for (int i = 0; i < 30 && IsStartOpen(start); i++) {
                    if (!WaitOrStop(100)) {
                        break;
                    }
                }
                PostToUi([] { HideStartContent(false); });
            }
        } uncloak{start};
        constexpr int kStep = 100;
        for (int waited = kStep; waited <= seconds * 1000; waited += kStep) {
            if (!WaitOrStop(kStep)) {
                return;  // the mod is unloading
            }
            if (!IsStartOpen(start)) {
                return;  // closed by itself
            }
            HWND fg = GetForegroundWindow();
            DWORD fgPid = 0;
            if (fg) {
                GetWindowThreadProcessId(fg, &fgPid);
            }
            // Only while Start itself is in front (else it's closing on its
            // own, or a prompt is up).
            if (fgPid != GetCurrentProcessId()) {
                continue;
            }
            HWND app = nullptr;
            for (HWND hwnd : OtherAppWindows()) {
                if (!std::binary_search(before->begin(), before->end(),
                                        hwnd)) {
                    app = hwnd;
                    break;
                }
            }
            if (app) {
                if (IsIconic(app)) {
                    ShowWindow(app, SW_RESTORE);
                }
                SetForegroundWindow(app);
                return;
            }
            // The Start key only after a while: not sooner, see above (and
            // Start closing by itself, its animation, can take a moment,
            // and the Start key would then open it again). Not while a key
            // is held down, which it would turn into a Win+key shortcut.
            if (waited < 1500 || AnyKeyDown()) {
                continue;
            }
            if (!IsStartOpen(start)) {
                return;
            }
            INPUT in[2] = {};
            in[0].type = in[1].type = INPUT_KEYBOARD;
            in[0].ki.wVk = in[1].ki.wVk = VK_LWIN;
            in[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, in, sizeof(INPUT));
            return;
        }
    });
}

void ShellOpen(std::wstring target, std::wstring args = L"") {
    AllowLaunchedAppToFront();
    RunWorker([target = ExpandEnv(target), args = ExpandEnv(args)] {
        // Programs start in their own folder, like a shortcut would (not
        // web addresses).
        std::wstring dir;
        if (size_t slash = target.find_last_of(L"\\/");
            slash != std::wstring::npos && target.rfind(L"shell:", 0) != 0 &&
            target.find(L"://") == std::wstring::npos) {
            dir = target.substr(0, slash);
        }
        SHELLEXECUTEINFOW sei{sizeof(sei)};
        sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
        sei.lpVerb = L"open";
        sei.lpFile = target.c_str();
        sei.lpParameters = args.empty() ? nullptr : args.c_str();
        sei.lpDirectory = dir.empty() ? nullptr : dir.c_str();
        sei.nShow = SW_SHOWNORMAL;
        if (!ShellExecuteExW(&sei)) {
            Wh_Log(L"Launch failed for %s: %u", target.c_str(),
                   GetLastError());
        }
    });
}

void LaunchApp(const AppEntry& app) {
    CloseStartAfterLaunch();
    if (app.website) {
        ShellOpen(app.path);  // the default browser
    } else if (!app.path.empty()) {
        ShellOpen(app.path, app.args);
    } else if (app.id.find(L'!') != std::wstring::npos) {
        // A Store app: activated like Explorer does, which also brings an
        // already running one to the front.
        AllowLaunchedAppToFront();
        RunWorker([id = app.id, args = ExpandEnv(app.args)] {
            winrt::com_ptr<IApplicationActivationManager> mgr;
            DWORD pid = 0;
            HRESULT hr = CoCreateInstance(CLSID_ApplicationActivationManager,
                                          nullptr, CLSCTX_LOCAL_SERVER,
                                          IID_PPV_ARGS(mgr.put()));
            if (SUCCEEDED(hr)) {
                hr = mgr->ActivateApplication(
                    id.c_str(), args.empty() ? nullptr : args.c_str(),
                    AO_NONE, &pid);
            }
            if (FAILED(hr)) {
                Wh_Log(L"Activating %s failed: %08X", id.c_str(), hr);
                ShellOpen(L"shell:AppsFolder\\" + id, args);
            }
        });
    } else {
        ShellOpen(L"shell:AppsFolder\\" + app.id, app.args);
    }
}

// --- An app's right-click menu in Start ---------------------------------------
// Run as administrator, Open file location and Uninstall are Windows' own
// actions for the app: the verbs of its item in the Apps folder
// (shell:AppsFolder), offered only where Windows offers them too. For a
// program (a path), the file itself.

// The AppsFolder item's context menu, ready to invoke. `menu` gets its
// items (destroy it).
winrt::com_ptr<IContextMenu> AppsFolderContextMenu(std::wstring const& id,
                                                   HMENU menu) {
    winrt::com_ptr<IShellItem> apps;
    winrt::com_ptr<IShellFolder> folder;
    if (FAILED(SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT,
                                    nullptr, IID_PPV_ARGS(apps.put()))) ||
        FAILED(apps->BindToHandler(nullptr, BHID_SFObject,
                                   IID_PPV_ARGS(folder.put())))) {
        return nullptr;
    }
    PIDLIST_RELATIVE pidl = nullptr;
    if (FAILED(folder->ParseDisplayName(nullptr, nullptr,
                                        const_cast<PWSTR>(id.c_str()),
                                        nullptr, &pidl, nullptr))) {
        return nullptr;
    }
    winrt::com_ptr<IContextMenu> cm;
    PCUITEMID_CHILD child = (PCUITEMID_CHILD)pidl;
    HRESULT hr = folder->GetUIObjectOf(nullptr, 1, &child, IID_IContextMenu,
                                       nullptr, cm.put_void());
    CoTaskMemFree(pidl);
    if (FAILED(hr) || FAILED(cm->QueryContextMenu(menu, 0, 1, 0x7FFF,
                                                  CMF_NORMAL))) {
        return nullptr;
    }
    return cm;
}

// The verbs Windows offers for a Start app (lowercase), cached.
std::mutex g_appVerbsMutex;
std::unordered_map<std::wstring, std::vector<std::wstring>> g_appVerbs;

std::vector<std::wstring> AppShellVerbs(std::wstring const& id) {
    {
        std::lock_guard lock(g_appVerbsMutex);
        if (auto it = g_appVerbs.find(id); it != g_appVerbs.end()) {
            return it->second;
        }
    }
    std::vector<std::wstring> verbs;
    HMENU menu = CreatePopupMenu();
    if (auto cm = AppsFolderContextMenu(id, menu)) {
        int n = GetMenuItemCount(menu);
        for (int i = 0; i < n; i++) {
            UINT cmd = GetMenuItemID(menu, i);
            WCHAR verb[128] = {};
            if (cmd >= 1 && cmd <= 0x7FFF &&
                SUCCEEDED(cm->GetCommandString(cmd - 1, GCS_VERBW, nullptr,
                                               (LPSTR)verb,
                                               ARRAYSIZE(verb))) &&
                verb[0]) {
                CharLowerW(verb);
                verbs.push_back(verb);
            }
        }
    }
    DestroyMenu(menu);
    std::lock_guard lock(g_appVerbsMutex);
    g_appVerbs[id] = verbs;
    return verbs;
}

void InvokeAppShellVerb(std::wstring id, std::string verb) {
    AllowLaunchedAppToFront();
    RunWorker([id, verb] {
        HMENU menu = CreatePopupMenu();
        if (auto cm = AppsFolderContextMenu(id, menu)) {
            std::wstring verbW(verb.begin(), verb.end());
            CMINVOKECOMMANDINFOEX ici{sizeof(ici)};
            ici.fMask = CMIC_MASK_UNICODE | CMIC_MASK_ASYNCOK;
            ici.lpVerb = verb.c_str();
            ici.lpVerbW = verbW.c_str();
            ici.nShow = SW_SHOWNORMAL;
            HRESULT hr = cm->InvokeCommand((CMINVOKECOMMANDINFO*)&ici);
            if (FAILED(hr)) {
                Wh_Log(L"%S on %s failed: %08X", verb.c_str(), id.c_str(), hr);
            }
        }
        DestroyMenu(menu);
    });
}

// Pinning to the taskbar is offered (and done) by Windows only in File
// Explorer, so Start asks this mod's part there (the settings host, see
// HostWndProc) to pin or unpin, with a WM_COPYDATA of "<verb>\n<app id>".
constexpr ULONG_PTR kTaskbarVerbData = 0x534D4731;  // "SMG1"
PCWSTR HostClassName();

bool IsTaskbarVerb(std::string const& verb) {
    return verb == "taskbarpin" || verb == "taskbarunpin";
}

void InvokeTaskbarVerb(std::wstring id, std::string verb) {
    RunWorker([id, verb] {
        HWND host =
            FindWindowExW(HWND_MESSAGE, nullptr, HostClassName(), nullptr);
        if (!host) {
            InvokeAppShellVerb(id, verb);  // Windows may not allow it here
            return;
        }
        std::wstring payload(verb.begin(), verb.end());
        payload += L"\n" + id;
        COPYDATASTRUCT cds{kTaskbarVerbData,
                           (DWORD)(payload.size() * sizeof(WCHAR)),
                           (PVOID)payload.data()};
        DWORD_PTR result = 0;
        SendMessageTimeoutW(host, WM_COPYDATA, 0, (LPARAM)&cds,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 3000, &result);
    });
}

// The menu of an app button in Start (in a tile or an opened group).
// Items Windows doesn't offer for the app are hidden when it opens.
void RequestSettingsWindow(int group, int item);

xc::MenuFlyout BuildAppMenu(AppEntry const& app) {
    xc::MenuFlyout menu;
    auto add = [&](PCWSTR glyph, PCWSTR text, std::function<void()> fn) {
        xc::MenuFlyoutItem mi;
        mi.Text(text);
        xc::FontIcon icon;
        icon.Glyph(glyph);
        mi.Icon(icon);
        mi.Click([fn](auto&&, auto&&) { fn(); });
        menu.Items().Append(mi);
        return mi;
    };
    bool program = !app.path.empty() && !app.website;
    // Edit: the settings window, with this app selected.
    add(L"\uE70F", L"Edit", [group = app.settingsGroup,
                             item = app.settingsItem] {
        RequestSettingsWindow(group, item);
    });
    if (app.folder) {
        TrackFlyout(menu);
        return menu;  // Windows' own actions are for apps
    }
    xc::MenuFlyoutSeparator sepRun;
    menu.Items().Append(sepRun);
    auto runAs = add(L"\uE7EF", L"Run as administrator", [app, program] {
        CloseStartAfterLaunch(15);  // after the administrator prompt
        if (program) {
            RunWorker([path = ExpandEnv(app.path), args = ExpandEnv(app.args)] {
                SHELLEXECUTEINFOW sei{sizeof(sei)};
                sei.fMask = SEE_MASK_NOASYNC;
                sei.lpVerb = L"runas";
                sei.lpFile = path.c_str();
                sei.lpParameters = args.empty() ? nullptr : args.c_str();
                sei.nShow = SW_SHOWNORMAL;
                ShellExecuteExW(&sei);
            });
        } else {
            InvokeAppShellVerb(app.id, "runas");
        }
    });
    auto location = add(L"\uE838", L"Open file location", [app, program] {
        CloseStartAfterLaunch();
        if (program) {
            RunWorker([path = ExpandEnv(app.path)] {
                if (PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path.c_str())) {
                    SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
                    ILFree(pidl);
                }
            });
        } else {
            InvokeAppShellVerb(app.id, "openfilelocation");
        }
    });
    xc::MenuFlyoutSeparator sepPin;
    menu.Items().Append(sepPin);
    // Pin to / Unpin from taskbar: unpin when Windows says it's pinned,
    // else pin (which Windows offers only in Explorer, so it's done there).
    auto pin = add(L"\uE718", L"Pin to taskbar",
                   [app] { InvokeTaskbarVerb(app.id, "taskbarpin"); });
    auto unpin = add(L"\uE77A", L"Unpin from taskbar",
                     [app] { InvokeTaskbarVerb(app.id, "taskbarunpin"); });
    // App settings: the app's page in Settings (Store apps only, as in
    // Start), found by its package family name.
    bool packaged = !program && app.id.find(L'!') != std::wstring::npos;
    auto settings = add(L"\uE713", L"App settings", [app] {
        std::wstring family = app.id.substr(0, app.id.find(L'!'));
        ShellOpen(L"ms-settings:appsfeatures-app?" + family);
    });
    settings.Visibility(packaged ? Visibility::Visible : Visibility::Collapsed);
    // Windows asks before uninstalling (or opens its uninstall page).
    auto uninstall = add(L"\uE74D", L"Uninstall",
                         [app] { InvokeAppShellVerb(app.id, "uninstall"); });
    menu.Opening([app, program, packaged, runAs, location, pin, unpin,
                  uninstall, sepRun, sepPin](auto&&, auto&&) {
        bool admin = !app.website, folder = !app.website, remove = false;
        bool canPin = false, canUnpin = false;
        if (!program && !app.website) {
            auto verbs = AppShellVerbs(app.id);
            auto has = [&](PCWSTR v) {
                return std::find(verbs.begin(), verbs.end(), v) != verbs.end();
            };
            admin = has(L"runas");
            folder = has(L"openfilelocation");
            remove = has(L"uninstall");
            canUnpin = has(L"taskbarunpin");
            canPin = !canUnpin;
        }
        auto show = [](xc::MenuFlyoutItem const& mi, bool on) {
            mi.Visibility(on ? Visibility::Visible : Visibility::Collapsed);
        };
        show(runAs, admin);
        show(location, folder);
        show(pin, canPin);
        show(unpin, canUnpin);
        show(uninstall, remove);
        // A separator only before a section with something shown in it.
        auto shown = [](bool on) {
            return on ? Visibility::Visible : Visibility::Collapsed;
        };
        sepRun.Visibility(shown(admin || folder));
        sepPin.Visibility(
            shown(canPin || canUnpin || packaged || remove));
    });
    // Pinned or not changes with the click: ask Windows again next time.
    for (auto mi : {pin, unpin}) {
        mi.Click([id = app.id](auto&&, auto&&) {
            std::lock_guard lock(g_appVerbsMutex);
            g_appVerbs.erase(id);
        });
    }
    TrackFlyout(menu);
    return menu;
}

// ---------------------------------------------------------------------------
// Config loading (worker thread)
// ---------------------------------------------------------------------------

// A group's tile as set, without its apps.
Group GroupFromSetting(GroupSetting const& cs) {
    Group cat;
    cat.name = cs.name;
    cat.layout = cs.layout;
    cat.rawGroup = cs.raw;
    cat.showAppNames = cs.showAppNames;
    // A one-spot (1 x 1) tile holds a single app and shows just the
    // app: no name under it and no background (as in the settings
    // window).
    bool single = cs.layout.spanHalfColumns == 1 && cs.layout.spanHalfRows == 1;
    cat.showLabel = cs.showLabel && !single;
    cat.showBackground = cs.showBackground && !single;
    return cat;
}

// "Sort apps by name" and "Hide empty groups", on the groups of a model.
void FinishModelGroups(Model& model) {
    if (model.layout.sortAppsByName) {
        auto byName = [](auto& x, auto& y) {
            return _wcsicmp(x.name.c_str(), y.name.c_str()) < 0;
        };
        for (auto& cat : model.groups) {
            std::sort(cat.apps.begin(), cat.apps.end(), byName);
            for (auto& app : cat.apps) {
                std::sort(app.children.begin(), app.children.end(), byName);
            }
        }
    }
    if (model.layout.hideEmptyGroups) {
        std::erase_if(model.groups,
                      [](const Group& c) { return c.apps.empty(); });
    }
}

std::shared_ptr<Model> LoadModel() {
    auto model = std::make_shared<Model>();
    std::vector<GroupSetting> catSettings;
    {
        std::lock_guard lock(g_settingsMutex);
        model->layout = g_settings.layout;
        catSettings = g_settings.groups;
        model->settingsGeneration = g_settings.generation;
    }
    auto& L = model->layout;

    // Resolve each configured entry (display name or AUMID) to a Start app.
    std::vector<AppEntry> all = EnumerateStartApps();
    std::unordered_map<std::wstring, size_t> byId, byName;
    auto lower = [](std::wstring s) {
        CharLowerBuffW(s.data(), (DWORD)s.size());
        return s;
    };
    for (size_t i = 0; i < all.size(); i++) {
        byId.emplace(lower(all[i].id), i);
        byName.emplace(lower(all[i].name), i);
    }
    // An app can be in several groups (or twice in one), as on the
    // settings board.
    for (size_t gi = 0; gi < catSettings.size(); gi++) {
        const auto& cs = catSettings[gi];
        Group cat = GroupFromSetting(cs);
        // An item -> what Start shows; false if there's nothing to show
        // (an app not found, a folder without any).
        auto resolve = [&](auto& self, ItemSetting const& item,
                           AppEntry& entry) -> bool {
            if (item.type == L"folder") {
                entry.folder = true;
                entry.name = item.label.empty() ? kFolderName : item.label;
                for (auto const& child : item.children) {
                    AppEntry c;
                    if (self(self, child, c)) {
                        c.settingsGroup = (int)gi;
                        c.settingsItem = entry.settingsItem;
                        c.rawGroup = child.rawGroup;
                        c.rawItem = child.rawItem;
                        c.rawSub = child.rawSub;
                        entry.children.push_back(std::move(c));
                    }
                }
                if (entry.children.empty()) {
                    return false;
                }
                // Its icon is its apps' (see MakeFolderIcon), unless it has
                // one of its own.
                entry.key = L"f:" + entry.name;
            } else if (item.type == L"website") {
                // A web page, opened in the default browser. Its icon is a
                // file, an image link (e.g. a favicon) or, without one, a
                // globe (see MakeAppIcon).
                if (item.path.empty()) {
                    return false;
                }
                entry.website = true;
                entry.path = item.path;
                entry.name = item.label.empty() ? WebsiteHost(item.path)
                                                : item.label;
                entry.key = L"w:" + item.path;
            } else if (item.type == L"program") {
                if (item.path.empty()) {
                    return false;
                }
                entry.path = item.path;
                entry.args = item.args;
                entry.name = item.label;
                if (entry.name.empty()) {
                    // File name without extension.
                    std::wstring file = item.path.substr(
                        item.path.find_last_of(L"\\/") + 1);
                    entry.name = file.substr(0, file.find_last_of(L'.'));
                }
                entry.iconSource = item.path;
                entry.key = L"p:" + item.path + L"|" + item.args;
            } else {
                std::wstring key = lower(item.app);
                size_t index;
                if (auto n = byName.find(key); n != byName.end()) {
                    index = n->second;
                } else if (auto d = byId.find(key); d != byId.end()) {
                    index = d->second;
                } else {
                    Wh_Log(L"App not found in Start: %s", item.app.c_str());
                    return false;
                }
                entry = all[index];
                entry.args = item.args;
                if (!item.label.empty()) {
                    entry.name = item.label;
                }
            }
            if (!item.icon.empty()) {
                entry.iconSource = item.icon;
                entry.key += L"|i:" + item.icon;
            }
            return true;
        };
        for (size_t ii = 0; ii < cs.items.size(); ii++) {
            AppEntry entry;
            entry.settingsGroup = (int)gi;
            entry.settingsItem = (int)ii;
            if (resolve(resolve, cs.items[ii], entry)) {
                // Again: a Start app's entry is copied over them.
                entry.settingsGroup = (int)gi;
                entry.settingsItem = (int)ii;
                entry.rawGroup = cs.items[ii].rawGroup;
                entry.rawItem = cs.items[ii].rawItem;
                entry.rawSub = cs.items[ii].rawSub;
                cat.apps.push_back(std::move(entry));
            }
        }
        model->groups.push_back(std::move(cat));
    }

    FinishModelGroups(*model);

    // Icons, rasterized at physical pixel size.
    UINT dpi = GetDpiForSystem();
    int px = (int)Clamp(L.iconSize * dpi / 96.0, 16, 256);
    auto loadIcon = [&](AppEntry const& app) {
        // Image links are loaded by XAML (MakeAppIcon).
        if (!model->icons.count(app.key) && !IsWebAddress(app.iconSource) &&
            !app.iconSource.empty() && !IsTextIcon(app.iconSource)) {
            model->icons[app.key] = ExtractAppIcon(app.iconSource, px);
        }
    };
    for (auto& cat : model->groups) {
        for (auto& app : cat.apps) {
            if (g_unloading) {
                return nullptr;
            }
            loadIcon(app);
            for (auto const& child : app.children) {
                loadIcon(child);
            }
        }
    }
    model->allApps = std::move(all);
    return model;
}

// ---------------------------------------------------------------------------
// UI building (UI thread)
// ---------------------------------------------------------------------------

void RebuildHostContent();
winrt::Windows::UI::Xaml::Thickness HostMargin();

[[clang::no_destroy]] std::unordered_map<std::wstring, xm::ImageSource>
    g_imageSources;  // UI thread

xm::ImageSource GetImageSource(const Model& model, const std::wstring& id) {
    auto cached = g_imageSources.find(id);
    if (cached != g_imageSources.end()) {
        return cached->second;
    }
    xm::ImageSource result{nullptr};
    auto it = model.icons.find(id);
    if (it != model.icons.end() && it->second) {
        auto& px = *it->second;
        uint32_t n = (uint32_t)px.bgra.size();
        winrt::Windows::Storage::Streams::Buffer buf(n);
        memcpy(buf.data(), px.bgra.data(), n);
        buf.Length(n);
        auto sb = wgi::SoftwareBitmap::CreateCopyFromBuffer(
            buf, wgi::BitmapPixelFormat::Bgra8, px.width, px.height,
            wgi::BitmapAlphaMode::Premultiplied);
        xmi::SoftwareBitmapSource src;
        src.SetBitmapAsync(sb);
        result = src;
    }
    g_imageSources.insert_or_assign(id, result);
    return result;
}

xm::Brush MakeBrush(const std::wstring& spec,
                    winrt::Windows::UI::Color fallback) {
    winrt::Windows::UI::Color c = fallback;
    ParseColor(spec, c);
    return xm::SolidColorBrush(c);
}

xc::Image MakeIcon(const Model& model, const std::wstring& id, double size);

// A text icon (ParseTextIcon) at `size`: a glyph of the icon font, or an
// emoji in color.
FrameworkElement MakeTextIcon(std::wstring const& text,
                              bool glyph,
                              double size) {
    xc::Grid box;
    box.Width(size);
    box.Height(size);
    if (glyph) {
        xc::FontIcon icon;
        icon.Glyph(text);
        icon.FontFamily(
            xm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
        icon.FontSize(std::round(size * 0.8));
        box.Children().Append(icon);
    } else {
        xc::TextBlock t;
        t.Text(text);
        t.FontFamily(xm::FontFamily(L"Segoe UI Emoji"));
        t.IsColorFontEnabled(true);
        t.FontSize(std::round(size * 0.72));
        t.TextAlignment(TextAlignment::Center);
        t.HorizontalAlignment(HorizontalAlignment::Center);
        t.VerticalAlignment(VerticalAlignment::Center);
        t.TextLineBounds(TextLineBounds::Tight);
        box.Children().Append(t);
    }
    return box;
}

// An image link (e.g. a website's favicon), loaded by XAML; `fallback`
// (a glyph) only if it can't be loaded.
FrameworkElement MakeLinkIcon(std::wstring const& url,
                              PCWSTR fallback,
                              double size) {
    xc::Grid box;
    box.Width(size);
    box.Height(size);
    xc::FontIcon icon;
    icon.Glyph(fallback);
    icon.FontSize(std::round(size * 0.75));
    icon.Visibility(Visibility::Collapsed);
    box.Children().Append(icon);
    try {
        xmi::BitmapImage bmp;
        bmp.DecodePixelWidth((int)std::lround(size * 2));
        bmp.UriSource(wf::Uri(url));
        xc::Image img;
        img.Width(size);
        img.Height(size);
        img.Stretch(xm::Stretch::Uniform);
        img.Source(bmp);
        winrt::weak_ref<xc::FontIcon> weakIcon = icon;
        img.ImageFailed([weakIcon](auto&&, auto&&) {
            if (auto i = weakIcon.get()) {
                i.Visibility(Visibility::Visible);
            }
        });
        box.Children().Append(img);
    } catch (...) {
        icon.Visibility(Visibility::Visible);
    }
    return box;
}

FrameworkElement MakeAppIcon(const Model& model,
                             const AppEntry& app,
                             double size);

// The background of a folder's icon: a rounded square, faint in either
// theme.
xc::Border FolderIconBox(double size) {
    xc::Border box;
    box.Width(size);
    box.Height(size);
    box.CornerRadius(CornerRadiusHelper::FromUniformRadius(
        std::round(size * 0.22)));
    box.Background(xm::SolidColorBrush(
        winrt::Windows::UI::Color{0x30, 0x80, 0x80, 0x80}));
    return box;
}

// Up to four of a folder's icons, 2 x 2 in a rounded square, at `size`
// (like a folder of apps on a phone). `icon(k, size)` makes the k-th one.
FrameworkElement FolderPreview(
    size_t count,
    double size,
    std::function<FrameworkElement(size_t, double)> const& icon) {
    xc::Border box = FolderIconBox(size);
    double pad = std::round(size * 0.14);
    double gap = std::max(1.0, std::round(size * 0.06));
    double small = std::floor((size - 2 * pad - gap) / 2);
    box.Padding(ThicknessHelper::FromUniformLength(pad));
    xc::Grid grid;
    grid.RowSpacing(gap);
    grid.ColumnSpacing(gap);
    for (int k = 0; k < 2; k++) {
        xc::ColumnDefinition cd;
        cd.Width(GridLengthHelper::FromPixels(small));
        grid.ColumnDefinitions().Append(cd);
        xc::RowDefinition rd;
        rd.Height(GridLengthHelper::FromPixels(small));
        grid.RowDefinitions().Append(rd);
    }
    grid.HorizontalAlignment(HorizontalAlignment::Center);
    grid.VerticalAlignment(VerticalAlignment::Center);
    for (size_t k = 0; k < 4 && k < count; k++) {
        auto el = icon(k, small);
        xc::Grid::SetRow(el, (int)(k / 2));
        xc::Grid::SetColumn(el, (int)(k % 2));
        grid.Children().Append(el);
    }
    box.Child(grid);
    return box;
}

// An app's icon: a text icon (glyph or emoji), an image link, or from the
// shell (MakeIcon); a website without one, a globe; a folder without one,
// its first apps' icons.
FrameworkElement MakeAppIcon(const Model& model,
                             const AppEntry& app,
                             double size) {
    if (app.folder && app.iconSource.empty()) {
        return FolderPreview(app.children.size(), size,
                             [&](size_t k, double s) {
                                 return MakeAppIcon(model, app.children[k], s);
                             });
    }
    std::wstring text;
    bool glyph = false;
    if (ParseTextIcon(app.iconSource, text, glyph)) {
        return MakeTextIcon(text, glyph, size);
    }
    if (IsWebAddress(app.iconSource)) {
        return MakeLinkIcon(app.iconSource,
                            app.website ? L"\uE774" : L"\uF0E2", size);
    }
    if (app.website && app.iconSource.empty()) {
        xc::Grid box;
        box.Width(size);
        box.Height(size);
        xc::FontIcon globe;
        globe.Glyph(L"\uE774");
        globe.FontSize(std::round(size * 0.75));
        box.Children().Append(globe);
        return box;
    }
    return MakeIcon(model, app.key, size);
}

xc::Image MakeIcon(const Model& model, const std::wstring& id, double size) {
    xc::Image img;
    img.Width(size);
    img.Height(size);
    img.Stretch(xm::Stretch::Uniform);
    if (auto src = GetImageSource(model, id)) {
        img.Source(src);
    }
    return img;
}

// An app's name as its button's tooltip, for when it isn't fully shown:
// while `name` is cut off ("..."), or always when no name is shown.
void SetAppTooltip(xc::Button const& btn,
                   xc::TextBlock const& name,
                   std::wstring const& text,
                   LayoutConfig const& L) {
    if (!L.showAppTooltips) {
        return;
    }
    if (!name) {
        xc::ToolTipService::SetToolTip(btn, winrt::box_value(text));
        return;
    }
    // Known once laid out; it changes with the size of the tile.
    winrt::weak_ref<xc::Button> weakBtn = btn;
    name.IsTextTrimmedChanged([weakBtn, text](xc::TextBlock const& tb,
                                              auto&&) {
        if (auto b = weakBtn.get()) {
            xc::ToolTipService::SetToolTip(
                b, tb.IsTextTrimmed() ? winrt::box_value(text) : nullptr);
        }
    });
}

xc::Button MakeFlatButton() {
    xc::Button b;
    b.Background(xm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    b.BorderThickness(ThicknessHelper::FromUniformLength(0));
    b.Padding(ThicknessHelper::FromUniformLength(0));
    b.HorizontalAlignment(HorizontalAlignment::Stretch);
    b.VerticalAlignment(VerticalAlignment::Stretch);
    b.HorizontalContentAlignment(HorizontalAlignment::Center);
    b.VerticalContentAlignment(VerticalAlignment::Center);
    b.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
    return b;
}

// The opened group's sizes: app cells, spacing and the popup's padding.
// Like Start's own folders it has a fixed size (at least kFlyoutMinRows
// rows), whatever the number of apps, and sits centered over Start.
constexpr double kFlyoutGap = 4, kFlyoutPad = 16;
constexpr int kFlyoutMinRows = 4;
// Space kept between the popup and the edges of Start's app area.
constexpr double kFlyoutMargin = 24;

double FlyoutWidth(int cols, double cell) {
    return cols * cell + (cols - 1) * kFlyoutGap + 2 * kFlyoutPad +
           2;  // + the presenter's border
}

// `el`'s bounds in its window's (XamlRoot's) coordinates; empty if it isn't
// laid out.
wf::Rect RootRect(FrameworkElement const& el) {
    try {
        if (el && el.ActualWidth() > 0 && el.ActualHeight() > 0) {
            return el.TransformToVisual(nullptr).TransformBounds(
                {0, 0, (float)el.ActualWidth(), (float)el.ActualHeight()});
        }
    } catch (...) {
    }
    return {0, 0, 0, 0};
}

// The part of Start the opened group is centered on: the scrolling area
// holding Start's sections and the groups (Start's window is larger than
// the menu it shows). Empty if not found.
wf::Rect StartAppsArea() {
    auto host = g_attach.host;
    int depth = 0;
    for (DependencyObject p = host; p && depth < 16;
         p = xm::VisualTreeHelper::GetParent(p), depth++) {
        if (auto sv = p.try_as<xc::ScrollViewer>()) {
            auto r = RootRect(sv);
            if (r.Width > 200 && r.Height > 200) {
                return r;
            }
        }
    }
    return RootRect(host);
}

// The open and close animation of the opened group, like Start's folders:
// it grows out of the cell it was opened from (`origin`, in window
// coordinates) to its place, and shrinks back into it when it closes.
// Replaces the flyout's own animation (a slide).
void AnimateFromOrigin(xc::Flyout const& fly, wf::Rect origin) {
    try {
        fly.AreOpenCloseAnimationsEnabled(false);
    } catch (...) {
    }
    Style style = fly.FlyoutPresenterStyle();
    if (!style) {
        style = Style(winrt::xaml_typename<xc::FlyoutPresenter>());
        fly.FlyoutPresenterStyle(style);
    }
    if (!style.IsSealed()) {
        // Rounded like Start's folders.
        style.Setters().Append(
            Setter(xc::Control::CornerRadiusProperty(),
                   winrt::box_value(CornerRadiusHelper::FromUniformRadius(8))));
        // Hidden until its opening animation starts (else shown at once).
        style.Setters().Append(
            Setter(UIElement::OpacityProperty(), winrt::box_value(0.0)));
    }
    struct State {
        winrt::weak_ref<xc::FlyoutPresenter> presenter;
        bool animated = false;  // it opened with the animation
        // The offset (X, Y, from its top left) and scale (Width, Height)
        // that put it over `origin`.
        wf::Rect from{};
        bool closing = false;  // closing animation running (or done)
    };
    auto state = std::make_shared<State>();

    // `open`: from the origin to its place, else back. It's scaled by its
    // composition visual, not a XAML transform: XAML draws content under a
    // scale animation once, at the scale it starts from, and stretches it
    // (blurry text); composition only scales what XAML drew at full size.
    auto run = [](std::shared_ptr<State> const& st, bool open,
                  std::function<void()> done) {
        auto p = st->presenter.get();
        wcomp::Visual v{nullptr};
        if (p && st->animated) {
            try {
                v = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
                    GetElementVisual(p);
            } catch (...) {
            }
        }
        if (!v) {
            if (p) {
                p.Opacity(1);
            }
            if (done) {
                done();
            }
            return;
        }
        using namespace std::chrono_literals;
        auto comp = v.Compositor();
        const wf::Rect f = st->from;
        // The point that stays put while the scale changes, so that at the
        // origin's scale it's over the origin (scale then offset, eased
        // together).
        auto center = [](double offset, double scale) {
            return std::abs(1 - scale) > 0.001 ? (float)(offset / (1 - scale))
                                               : 0.0f;
        };
        v.CenterPoint({center(f.X, f.Width), center(f.Y, f.Height), 0});
        // Windows' "decelerate" curve to open, "accelerate" to close.
        auto ease = open ? comp.CreateCubicBezierEasingFunction({0.1f, 0.9f},
                                                                {0.2f, 1.0f})
                         : comp.CreateCubicBezierEasingFunction({0.7f, 0.0f},
                                                                {1.0f, 0.5f});
        auto linear = comp.CreateLinearEasingFunction();
        const wf::TimeSpan len = open ? wf::TimeSpan(300ms)
                                      : wf::TimeSpan(180ms);
        const winrt::Windows::Foundation::Numerics::float3 small{
            f.Width, f.Height, 1},
            full{1, 1, 1};
        auto scale = comp.CreateVector3KeyFrameAnimation();
        auto fade = comp.CreateScalarKeyFrameAnimation();
        if (open) {
            v.Scale(small);
            v.Opacity(0);
            p.Opacity(1);  // the style hid it until now
            scale.InsertKeyFrame(0, small);
            scale.InsertKeyFrame(1, full, ease);
            scale.Duration(len);
            // Fades in quickly as it starts growing.
            fade.InsertKeyFrame(0, 0);
            fade.InsertKeyFrame(1, 1, linear);
            fade.Duration(wf::TimeSpan(120ms));
        } else {
            // From wherever it is (it may still be opening).
            scale.InsertExpressionKeyFrame(0, L"this.StartingValue");
            scale.InsertKeyFrame(1, small, ease);
            scale.Duration(len);
            // Fades out as it ends shrinking.
            fade.InsertExpressionKeyFrame(0, L"this.StartingValue");
            fade.InsertExpressionKeyFrame(0.33f, L"this.StartingValue");
            fade.InsertKeyFrame(1, 0, linear);
            fade.Duration(len);
        }
        auto batch =
            comp.CreateScopedBatch(wcomp::CompositionBatchTypes::Animation);
        v.StartAnimation(L"Scale", scale);
        v.StartAnimation(L"Opacity", fade);
        batch.End();
        if (done) {
            batch.Completed([done](auto&&, auto&&) { done(); });
        }
    };

    fly.Opened([state, origin, run](wf::IInspectable const& sender, auto&&) {
        auto f = sender.try_as<xc::Flyout>();
        auto content = f ? f.Content().try_as<FrameworkElement>() : nullptr;
        xc::FlyoutPresenter presenter{nullptr};
        int depth = 0;
        for (DependencyObject p = content; p && depth < 8;
             p = xm::VisualTreeHelper::GetParent(p), depth++) {
            if (auto fp = p.try_as<xc::FlyoutPresenter>()) {
                presenter = fp;
                break;
            }
        }
        if (!presenter) {
            return;
        }
        state->presenter = presenter;
        presenter.CornerRadius(CornerRadiusHelper::FromUniformRadius(8));
        auto r = RootRect(presenter);
        if (r.Width <= 0 || r.Height <= 0 || origin.Width <= 0 ||
            origin.Height <= 0 || g_unloading) {
            presenter.Opacity(1);
            return;
        }
        // In the popup's own coordinates (r may be scaled from them).
        double kx = presenter.ActualWidth() / r.Width;
        double ky = presenter.ActualHeight() / r.Height;
        state->from = {(float)((origin.X - r.X) * kx),
                       (float)((origin.Y - r.Y) * ky), origin.Width / r.Width,
                       origin.Height / r.Height};
        state->animated = true;
        run(state, true, nullptr);
    });

    // Closing: shrinks back first, then closes for real.
    fly.Closing([state, run](xcp::FlyoutBase const& sender,
                             xcp::FlyoutBaseClosingEventArgs const& args) {
        if (state->closing || !state->animated || g_unloading ||
            !state->presenter.get()) {
            return;
        }
        state->closing = true;
        args.Cancel(true);
        winrt::weak_ref<xcp::FlyoutBase> weak = sender;
        run(state, false, [weak] {
            if (auto f = weak.get()) {
                f.Hide();
            }
        });
    });
}

void ShowAppsFlyout(FrameworkElement const& anchor,
                    wf::Rect origin,
                    std::wstring const& title,
                    std::vector<AppEntry> const& apps,
                    size_t first,
                    int tile = -1,
                    int folder = -1);
void HookStartFlyoutDrag(xc::Grid const& root);
void RequestSettingsWindow(int group, int item);

// Shows `fly` at `anchor`, centered over `area` (window coordinates) when
// its full `size` is known, else below the anchor; growing out of `origin`
// (AnimateFromOrigin; the anchor if empty).
void ShowCenteredOver(xc::Flyout const& fly,
                      FrameworkElement const& anchor,
                      wf::Rect area,
                      wf::Size size,
                      wf::Rect origin) {
    const wf::Rect tl = RootRect(anchor);
    // In the anchor's coordinates, which may be scaled (a Viewbox).
    double x = 0, y = anchor.ActualHeight();
    if (area.Width > 0 && size.Height > 0 && tl.Width > 0 &&
        anchor.ActualWidth() > 0) {
        double scale = tl.Width / anchor.ActualWidth();
        double left = area.X + (area.Width - size.Width) / 2;
        double top = area.Y + (area.Height - size.Height) / 2;
        if (scale > 0) {
            x = (left - tl.X) / scale;
            y = (top - tl.Y) / scale;
        }
    }
    AnimateFromOrigin(fly, origin.Width > 0 ? origin : tl);
    xcp::FlyoutShowOptions opts;
    opts.Position(wf::Point{(float)x, (float)y});
    opts.Placement(xcp::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    fly.ShowAt(anchor, opts);
}

// The content of an opened group or folder (see BuildGroupFlyout), for
// `fly`. Its apps can be dragged (StartFlyout) when it shows those of the
// group `tile` of g_model (or of its folder `folder`).
xc::Grid BuildGroupFlyoutContent(xc::Flyout const& fly,
                                 const Model& model,
                                 std::wstring const& titleText,
                                 std::vector<AppEntry> const& apps,
                                 int cols,
                                 size_t first,
                                 winrt::weak_ref<FrameworkElement> anchor,
                                 int tile,
                                 int folder) {
    const FlyoutLook look = FlyoutLookFor(model.layout.iconSize);
    bool draggable = tile >= 0 && g_model && g_model.get() == &model &&
                     tile < (int)model.groups.size();
    StartFlyout fs;
    if (draggable) {
        fs.model = g_model;
        fs.tile = tile;
        fs.folder = folder;
        fs.first = first;
        fs.cols = cols;
        fs.groupRaw = model.groups[tile].rawGroup;
        if (folder >= 0 && folder < (int)model.groups[tile].apps.size()) {
            fs.folderId = IdOf(model.groups[tile].apps[folder]);
        }
        fs.anchor = anchor;
        fs.fly = fly;
    }

    xc::Grid root;
    root.RowSpacing(12);
    xc::RowDefinition titleRow;
    titleRow.Height(GridLengthHelper::Auto());
    root.RowDefinitions().Append(titleRow);
    xc::RowDefinition appsRow;
    appsRow.Height(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    root.RowDefinitions().Append(appsRow);
    root.Width(cols * look.cell + (cols - 1) * kFlyoutGap);

    // The name, centered above the apps like a Start folder's.
    xc::TextBlock title;
    title.Text(titleText);
    title.FontSize(20);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    title.Margin(ThicknessHelper::FromLengths(0, 4, 0, 8));
    title.HorizontalAlignment(HorizontalAlignment::Center);
    title.TextAlignment(TextAlignment::Center);
    // A long name is cut short instead of widening the popup.
    title.TextWrapping(TextWrapping::NoWrap);
    title.TextTrimming(TextTrimming::CharacterEllipsis);
    title.MaxWidth(cols * look.cell + (cols - 1) * kFlyoutGap - 8);
    root.Children().Append(title);

    xc::Grid grid;
    grid.ColumnSpacing(kFlyoutGap);
    grid.RowSpacing(kFlyoutGap);
    grid.VerticalAlignment(VerticalAlignment::Top);
    for (int c = 0; c < cols; c++) {
        xc::ColumnDefinition cd;
        cd.Width(GridLengthHelper::FromPixels(look.cell));
        grid.ColumnDefinitions().Append(cd);
    }
    first = std::min(first, apps.size());
    const size_t shown = apps.size() - first;
    int rows = ((int)shown + cols - 1) / cols;
    for (int r = 0; r < rows; r++) {
        xc::RowDefinition rd;
        rd.Height(GridLengthHelper::Auto());
        grid.RowDefinitions().Append(rd);
    }

    for (size_t i = 0; i < shown; i++) {
        const auto& app = apps[first + i];
        auto btn = MakeFlatButton();
        btn.Padding(ThicknessHelper::FromLengths(4, 10, 4, 8));
        xc::StackPanel sp;
        sp.Spacing(6);
        auto img = MakeAppIcon(model, app, look.icon);
        img.HorizontalAlignment(HorizontalAlignment::Center);
        sp.Children().Append(img);
        xc::TextBlock tb;
        tb.Text(app.name);
        tb.FontSize(look.font);
        tb.TextAlignment(TextAlignment::Center);
        tb.TextWrapping(TextWrapping::Wrap);
        tb.MaxLines(2);
        tb.TextTrimming(TextTrimming::CharacterEllipsis);
        tb.Width(look.textWidth);
        sp.Children().Append(tb);
        btn.Content(sp);
        SetAppTooltip(btn, tb, app.name, model.layout);
        winrt::weak_ref<xc::Flyout> weakFly = fly;
        // A folder of the group (in its opened view) opens as one of it.
        int index = (int)(first + i);
        int folderTile = draggable && folder < 0 ? tile : -1;
        btn.Click([app, weakFly, anchor, folderTile, index](
                      wf::IInspectable const& sender, auto&&) {
            // A folder opens from this cell: taken before this popup starts
            // shrinking away.
            auto origin = RootRect(sender.try_as<FrameworkElement>());
            if (auto f = weakFly.get()) {
                f.Hide();
            }
            if (!app.folder) {
                LaunchApp(app);
            } else if (auto a = anchor.get()) {
                ShowAppsFlyout(a, origin, app.name, app.children, 0,
                               folderTile, folderTile >= 0 ? index : -1);
            }
        });
        if (draggable) {
            fs.items.emplace_back(btn, index);
        }
        btn.ContextFlyout(BuildAppMenu(app));
        xc::Grid::SetRow(btn, (int)(i / cols));
        xc::Grid::SetColumn(btn, (int)(i % cols));
        grid.Children().Append(btn);
    }

    xc::ScrollViewer sv;
    sv.VerticalScrollBarVisibility(xc::ScrollBarVisibility::Auto);
    sv.Content(grid);
    xc::Grid::SetRow(sv, 1);
    root.Children().Append(sv);
    // Right-click on its empty space: Edit, the settings window with the
    // folder (or the group) selected. Its apps have their own menus.
    if (draggable) {
        auto const& cat = model.groups[tile];
        int editGroup = -1, editItem = -1;
        if (folder >= 0 && folder < (int)cat.apps.size()) {
            editGroup = cat.apps[folder].settingsGroup;
            editItem = cat.apps[folder].settingsItem;
        } else if (!cat.apps.empty()) {
            editGroup = cat.apps.front().settingsGroup;
        }
        if (editGroup >= 0) {
            // A background, so the empty space gets the right click.
            root.Background(
                xm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
            xc::MenuFlyout menu;
            xc::MenuFlyoutItem mi;
            mi.Text(L"Edit");
            xc::FontIcon icon;
            icon.Glyph(L"\uE70F");
            mi.Icon(icon);
            winrt::weak_ref<xc::Flyout> weakFly = fly;
            mi.Click([editGroup, editItem, weakFly](auto&&, auto&&) {
                if (auto f = weakFly.get()) {
                    f.Hide();
                }
                RequestSettingsWindow(editGroup, editItem);
            });
            menu.Items().Append(mi);
            TrackFlyout(menu);
            root.ContextFlyout(menu);
        }
    }
    if (draggable) {
        // A layer on top for dragging (see "Rearranging in Start").
        xc::Canvas overlay;
        overlay.IsHitTestVisible(false);
        xc::Grid::SetRowSpan(overlay, 2);
        root.Children().Append(overlay);
        fs.root = root;
        fs.overlay = overlay;
        g_attach.flyout = std::move(fs);
        HookStartFlyoutDrag(root);
    }
    return root;
}

// An opened group or folder: `apps` from `first` on (for a group, the ones
// its tile has no room for), under `titleText`, in `cols` columns. A folder
// in it opens in its place, tied to `anchor` (the tile). See
// BuildGroupFlyoutContent for `tile` and `folder`.
xc::Flyout BuildGroupFlyout(const Model& model,
                            std::wstring const& titleText,
                            std::vector<AppEntry> const& apps,
                            int cols,
                            size_t first,
                            winrt::weak_ref<FrameworkElement> anchor,
                            int tile,
                            int folder) {
    xc::Flyout fly;
    // Start can only draw inside its own window, so keep the popup within
    // its bounds (it's shifted in instead of being cut off at the edge).
    // It's placed by its top left (see ShowAppsFlyout).
    fly.ShouldConstrainToRootBounds(true);
    fly.Placement(xcp::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    // Flyouts have a default max size (~456 wide) that would cut the grid.
    Style presenterStyle(winrt::xaml_typename<xc::FlyoutPresenter>());
    presenterStyle.Setters().Append(
        Setter(FrameworkElement::MaxWidthProperty(), winrt::box_value(10000.0)));
    presenterStyle.Setters().Append(Setter(FrameworkElement::MaxHeightProperty(),
                                           winrt::box_value(10000.0)));
    presenterStyle.Setters().Append(Setter(
        xc::Control::PaddingProperty(),
        winrt::box_value(ThicknessHelper::FromUniformLength(kFlyoutPad))));
    fly.FlyoutPresenterStyle(presenterStyle);
    TrackFlyout(fly);
    fly.Content(BuildGroupFlyoutContent(fly, model, titleText, apps, cols,
                                        first, anchor, tile, folder));
    return fly;
}

// The opened group or folder again, after its apps were rearranged in it
// (ApplyStartDrop): from the rebuilt model, at the same size.
void RefreshStartFlyout() {
    auto& fs = g_attach.flyout;
    auto fly = fs.fly.get();
    auto oldRoot = fs.root.get();
    auto model = g_model;
    if (!fly || !oldRoot || !model) {
        return;
    }
    int tile = -1;
    for (int k = 0; k < (int)model->groups.size(); k++) {
        if (model->groups[k].rawGroup == fs.groupRaw) {
            tile = k;
            break;
        }
    }
    int folder = -1;
    if (tile >= 0 && fs.folder >= 0) {
        auto const& apps = model->groups[tile].apps;
        for (int i = 0; i < (int)apps.size(); i++) {
            if (apps[i].folder && IdOf(apps[i]) == fs.folderId) {
                folder = i;
                break;
            }
        }
    }
    if (tile < 0 || (fs.folder >= 0 && folder < 0)) {
        fly.Hide();
        return;
    }
    auto const& cat = model->groups[tile];
    auto const& apps = folder >= 0 ? cat.apps[folder].children : cat.apps;
    std::wstring title = folder >= 0 ? cat.apps[folder].name : cat.name;
    // Tied to the group's new tile.
    winrt::weak_ref<FrameworkElement> anchor = fs.anchor;
    for (auto const& t : g_attach.tiles) {
        if (t.group == tile) {
            if (auto el = t.tile.get()) {
                anchor = winrt::make_weak(el.as<FrameworkElement>());
            }
            break;
        }
    }
    auto root = BuildGroupFlyoutContent(fly, *model, title, apps, fs.cols,
                                        fs.first, anchor, tile, folder);
    root.Height(oldRoot.Height());
    fly.Content(root);
}

// Opens a group's or folder's apps (BuildGroupFlyout), tied to `anchor`
// (its tile), growing out of `origin` (the cell clicked, in window
// coordinates). Like Start's folders, it's centered over Start's app area,
// with a fixed size: as many columns as set (or as fit the area) and at
// least kFlyoutMinRows rows; taller than the area, its apps scroll.
void ShowAppsFlyout(FrameworkElement const& anchor,
                    wf::Rect origin,
                    std::wstring const& title,
                    std::vector<AppEntry> const& apps,
                    size_t first,
                    int tile,
                    int folder) {
    auto t = anchor;
    auto model = g_model;
    if (!t || !model) {
        return;
    }
    const FlyoutLook look = FlyoutLookFor(model->layout.iconSize);
    const wf::Rect area = StartAppsArea();
    int cols = std::max(1, model->layout.flyoutColumns);
    if (area.Width > 0) {
        while (cols > 1 &&
               FlyoutWidth(cols, look.cell) > area.Width - 2 * kFlyoutMargin) {
            cols--;
        }
    }
    auto fly = BuildGroupFlyout(*model, title, apps, cols, first,
                                winrt::make_weak(t), tile, folder);

    // Its height: measured with the apps it has, plus empty rows up to
    // kFlyoutMinRows, within the area.
    const double chrome = 2 * kFlyoutPad + 2;  // padding and border
    const double width = FlyoutWidth(cols, look.cell);
    double height = 0;
    if (auto root = fly.Content().try_as<xc::Grid>()) {
        try {
            root.Measure({(float)(width - chrome), INFINITY});
            double h = root.DesiredSize().Height;
            int shown = (int)(apps.size() - std::min(first, apps.size()));
            int rows = (shown + cols - 1) / cols;
            double rowH = look.cell;
            if (auto sv = root.Children().GetAt(1).try_as<xc::ScrollViewer>()) {
                if (auto grid = sv.Content().try_as<xc::Grid>();
                    grid && rows > 0) {
                    double gh = grid.DesiredSize().Height;
                    if (gh > 0) {
                        rowH = (gh - (rows - 1) * kFlyoutGap) / rows;
                    }
                }
            }
            if (rows < kFlyoutMinRows) {
                h += (kFlyoutMinRows - rows) * (rowH + kFlyoutGap) -
                     (rows == 0 ? kFlyoutGap : 0);
            }
            if (area.Height > 0) {
                h = std::min(h, area.Height - 2 * kFlyoutMargin - chrome);
            }
            if (h > 0) {
                root.Height(h);
                height = h + chrome;
            }
        } catch (...) {
        }
    }

    ShowCenteredOver(fly, t, area, {(float)width, (float)height}, origin);
}

// The parts of a tile in Start that a drag finds under the pointer (see
// "Rearranging in Start").
struct TileParts {
    xc::Border tile{nullptr};
    std::vector<std::pair<xc::Button, int>> apps;  // with their app index
    xc::Button cluster{nullptr};                   // the overflow cluster
};

FrameworkElement BuildTile(const Model& model,
                           const Group& cat,
                           bool dark,
                           double tileAcross,
                           TileParts* parts = nullptr,
                           int groupIndex = -1) {
    const auto& L = model.layout;
    const auto& C = cat.layout;
    // This tile's icon grid; the icons keep the global size and the tile
    // grows by whole cells of the group grid to hold them.
    const int previewColumns = std::max(1, C.iconColumns);
    const int previewRows = std::max(1, C.iconRows);
    const int spanHalves = std::max(1, C.spanHalfColumns);
    const int spanHalfRows = std::max(1, C.spanHalfRows);
    // A tile spanning several spots also covers the gaps between them (and,
    // vertically, the name row of the cells it covers).
    const double labelH =
        L.showLabels ? L.labelSpacing + L.labelFontSize * 1.4 : 0;
    const bool nameShown = L.showLabels && cat.showLabel;
    const TileBox box =
        TileBoxFor(spanHalves, spanHalfRows, tileAcross, L.tileSize,
                   L.tileSpacingX, L.tileSpacingY, labelH, nameShown);
    const double tileW = box.width;
    const double tileH = box.height;
    // Without a background, no padding either: just the apps.
    const TileFit tf = FitTile(L.iconSize, tileW, tileH,
                               cat.showBackground ? L.tilePadding : 0,
                               L.iconSpacing, C, L.appPaddingX,
                               L.appPaddingY,
                               L.showAppLabels && cat.showAppNames);
    const double iconSize = tf.iconSize;
    const double padX = tf.padX;

    xc::StackPanel outer;
    outer.Spacing(L.labelSpacing);
    outer.Width(tileW);

    xc::Border tile;
    tile.Width(tileW);
    tile.Height(tileH);
    // Without a background, the rounding of the app inside (a flat button),
    // so the tile doesn't cut its corners.
    tile.CornerRadius(CornerRadiusHelper::FromUniformRadius(
        cat.showBackground ? L.cornerRadius : 4));
    tile.Padding(ThicknessHelper::FromLengths(padX, tf.padY, padX, tf.padY));
    if (cat.showBackground) {
        tile.BorderThickness(
            ThicknessHelper::FromUniformLength(L.borderThickness));
        tile.Background(MakeBrush(
            L.tileBackground, dark ? winrt::Windows::UI::Color{0x0D, 255, 255, 255}
                                   : winrt::Windows::UI::Color{0xB3, 255, 255, 255}));
        tile.BorderBrush(MakeBrush(
            L.tileBorder, dark ? winrt::Windows::UI::Color{0x0F, 255, 255, 255}
                               : winrt::Windows::UI::Color{0x0F, 0, 0, 0}));
    } else {
        tile.Background(xm::SolidColorBrush(
            winrt::Windows::UI::Colors::Transparent()));
    }


    xc::Grid grid;
    grid.RowSpacing(L.iconSpacing);
    grid.ColumnSpacing(L.iconSpacing);
    for (int c = 0; c < previewColumns; c++) {
        xc::ColumnDefinition cd;
        cd.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        grid.ColumnDefinitions().Append(cd);
    }
    for (int r = 0; r < previewRows; r++) {
        xc::RowDefinition rd;
        rd.Height(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        grid.RowDefinitions().Append(rd);
    }

    int cells = previewColumns * previewRows;
    int count = (int)cat.apps.size();
    bool overflow = L.showOverflowCluster && count > cells && cells > 1;
    int singles = overflow ? cells - 1 : std::min(count, cells);
    const auto appPadding = ThicknessHelper::FromLengths(
        L.appPaddingX, tf.appPadY, L.appPaddingX, tf.appPadY);

    for (int i = 0; i < singles; i++) {
        const auto& app = cat.apps[i];
        auto btn = MakeFlatButton();
        btn.Padding(appPadding);
        xc::TextBlock nameText{nullptr};
        if (L.showAppLabels && cat.showAppNames) {
            // Icon with the app's name underneath, sized to the cell.
            xc::StackPanel cell;
            cell.Spacing(tf.nameGap);
            auto img = MakeAppIcon(model, app, iconSize);
            img.HorizontalAlignment(HorizontalAlignment::Center);
            cell.Children().Append(img);
            xc::TextBlock name;
            nameText = name;
            name.Text(app.name);
            name.FontSize(AppNameFontSize(L.iconSize));
            name.TextAlignment(TextAlignment::Center);
            name.TextTrimming(TextTrimming::CharacterEllipsis);
            name.TextWrapping(TextWrapping::NoWrap);
            name.MaxWidth(
                std::max(24.0, (tileW - 2 * padX -
                                (previewColumns - 1) * L.iconSpacing) /
                                       previewColumns -
                                   2 * L.appPaddingX));
            cell.Children().Append(name);
            btn.Content(cell);
        } else {
            btn.Content(MakeAppIcon(model, app, iconSize));
        }
        SetAppTooltip(btn, nameText, app.name, L);
        // A folder opens like the group does.
        btn.Click([app, weakTile = winrt::weak_ref<xc::Border>(tile),
                   groupIndex, i](wf::IInspectable const& sender, auto&&) {
            if (!app.folder) {
                LaunchApp(app);
            } else if (auto t = weakTile.get()) {
                ShowAppsFlyout(t, RootRect(sender.try_as<FrameworkElement>()),
                               app.name, app.children, 0, groupIndex,
                               groupIndex >= 0 ? i : -1);
            }
        });
        btn.ContextFlyout(BuildAppMenu(app));
        xc::Grid::SetRow(btn, i / previewColumns);
        xc::Grid::SetColumn(btn, i % previewColumns);
        grid.Children().Append(btn);
        if (parts) {
            parts->apps.emplace_back(btn, i);
        }
    }

    if (overflow) {
        auto btn = MakeFlatButton();
        btn.Padding(appPadding);
        xc::Grid mini;
        double small = std::floor((iconSize - 4) / 2);
        mini.RowSpacing(4);
        mini.ColumnSpacing(4);
        mini.HorizontalAlignment(HorizontalAlignment::Center);
        mini.VerticalAlignment(VerticalAlignment::Center);
        for (int k = 0; k < 2; k++) {
            xc::ColumnDefinition cd;
            cd.Width(GridLengthHelper::Auto());
            mini.ColumnDefinitions().Append(cd);
            xc::RowDefinition rd;
            rd.Height(GridLengthHelper::Auto());
            mini.RowDefinitions().Append(rd);
        }
        for (int k = 0; k < 4 && singles + k < count; k++) {
            auto img = MakeAppIcon(model, cat.apps[singles + k], small);
            xc::Grid::SetRow(img, k / 2);
            xc::Grid::SetColumn(img, k % 2);
            mini.Children().Append(img);
        }
        btn.Content(mini);
        xc::ToolTipService::SetToolTip(
            btn, winrt::box_value(winrt::hstring(
                     L"+" + std::to_wstring(count - singles) + L" more")));
        btn.Click([weakTile = winrt::weak_ref<xc::Border>(tile), name = cat.name,
                   apps = cat.apps, singles,
                   groupIndex](wf::IInspectable const& sender, auto&&) {
            if (auto t = weakTile.get()) {
                ShowAppsFlyout(t, RootRect(sender.try_as<FrameworkElement>()),
                               name, apps, (size_t)singles, groupIndex);
            }
        });
        xc::Grid::SetRow(btn, (cells - 1) / previewColumns);
        xc::Grid::SetColumn(btn, (cells - 1) % previewColumns);
        grid.Children().Append(btn);
        if (parts) {
            parts->cluster = btn;
        }
    }

    tile.Child(grid);
    if (parts) {
        parts->tile = tile;
    }
    // The expanded view opens only from the overflow cluster (in the last
    // cell, only when there are more apps than cells) or a folder.
    // Taps must still stop here: otherwise they bubble on into Start's
    // AllAppsGrid, which reacts by moving focus and light-dismisses the
    // flyout the overflow button just opened.
    tile.Tapped([](auto&&, Input::TappedRoutedEventArgs const& e) {
        e.Handled(true);
    });
    outer.Children().Append(tile);

    if (nameShown) {
        xc::TextBlock label;
        label.Text(cat.name);
        label.FontSize(L.labelFontSize);
        label.TextAlignment(TextAlignment::Center);
        label.HorizontalAlignment(HorizontalAlignment::Center);
        label.TextTrimming(TextTrimming::CharacterEllipsis);
        winrt::Windows::UI::Color lc;
        if (ParseColor(L.labelColor, lc)) {
            label.Foreground(xm::SolidColorBrush(lc));
        }
        outer.Children().Append(label);
    }

    return outer;
}

void ReloadAsync();
void RequestSettingsWindow(int group = -1, int item = -1);

xc::MenuFlyoutItem MakeCustomizeMenuItem();

// The menu of the groups area (right-click between the tiles): the same as
// Start's own menu with this mod's item (see AddToStartMenu), so Start has
// one menu, wherever it's opened.
xc::MenuFlyout BuildContextMenu() {
    xc::MenuFlyout menu;
    auto add = [&](PCWSTR glyph, PCWSTR text, std::function<void()> fn) {
        xc::MenuFlyoutItem mi;
        mi.Text(text);
        xc::FontIcon icon;
        icon.Glyph(glyph);
        mi.Icon(icon);
        mi.Click([fn](auto&&, auto&&) { fn(); });
        menu.Items().Append(mi);
    };
    add(L"\uE713", L"Start settings",
        [] { ShellOpen(L"ms-settings:personalization-start"); });
    // The same item as added to Start's menu, with its tag: the code that
    // adds it there (AddToStartMenu) sees this menu has it already.
    menu.Items().Append(MakeCustomizeMenuItem());
    TrackFlyout(menu);
    return menu;
}

// --- Start's own sections (Pinned / Recommended / All) ----------------------

bool TextInList(std::wstring_view text, std::vector<std::wstring> const& list) {
    for (const auto& h : list) {
        if (h.size() == text.size() &&
            CompareStringOrdinal(h.data(), (int)h.size(), text.data(),
                                 (int)text.size(), TRUE) == CSTR_EQUAL) {
            return true;
        }
    }
    return false;
}

bool SubtreeHasText(DependencyObject const& root,
                    std::vector<std::wstring> const& texts,
                    int depth) {
    if (auto tb = root.try_as<xc::TextBlock>()) {
        return TextInList(tb.Text(), texts);
    }
    if (depth <= 0) {
        return false;
    }
    int n = xm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        if (SubtreeHasText(xm::VisualTreeHelper::GetChild(root, i), texts,
                           depth - 1)) {
            return true;
        }
    }
    return false;
}

void RestoreSections() {
    for (auto& [weak, vis] : g_attach.hidden) {
        if (auto el = weak.get()) {
            el.Visibility(vis);
        }
    }
    g_attach.hidden.clear();
}

void HideSectionElement(UIElement const& el) {
    if (!el) {
        return;
    }
    g_attach.hidden.emplace_back(winrt::make_weak(el), el.Visibility());
    el.Visibility(Visibility::Collapsed);
}

// Start's Pinned, Recommended and All headers are rows of the header grid we
// insert our row into. A section is its header row plus the rows after it, up
// to the next section header (or our row, which ends the Pinned section). The
// All section also covers the app list itself: the items panel of the
// GridView the header grid belongs to.
void ApplySectionVisibility() {
    RestoreSections();
    auto grid = g_attach.rowGrid.get();
    if (!grid || !g_attach.host ||
        (g_attach.insertedRow < 0 && !g_attach.footerList.get())) {
        return;
    }
    bool show[3];
    std::vector<std::wstring> headers[3];
    {
        std::lock_guard lock(g_settingsMutex);
        show[0] = g_settings.showPinned;
        show[1] = g_settings.showRecommended;
        show[2] = g_settings.showAllApps;
        headers[0] = g_settings.pinnedHeaders;
        headers[1] = g_settings.recommendedHeaders;
        headers[2] = g_settings.allHeaders;
    }
    if (show[0] && show[1] && show[2]) {
        return;
    }
    // Our row; in the footer, below all of the grid's rows.
    const int ins = g_attach.insertedRow >= 0 ? g_attach.insertedRow
                                              : (int)grid.RowDefinitions().Size() + 1;

    // Row of each section's header (-1 = not found).
    int headerRow[3] = {-1, -1, -1};
    for (auto child : grid.Children()) {
        if (child == g_attach.host) {
            continue;
        }
        auto fe = child.try_as<FrameworkElement>();
        if (!fe) {
            continue;
        }
        int r = xc::Grid::GetRow(fe);
        for (int s = 0; s < 3; s++) {
            if (headerRow[s] < 0 && SubtreeHasText(child, headers[s], 10)) {
                headerRow[s] = r;
            }
        }
    }
    if (headerRow[0] < 0) {
        if (auto pinned = g_attach.target.get()) {
            headerRow[0] = xc::Grid::GetRow(pinned);  // the pinned list
        }
    }
    auto sectionOf = [&](int r) {
        int best = -1;
        for (int s = 0; s < 3; s++) {
            int h = headerRow[s];
            bool sameSide = r < ins ? h < ins : h > ins;
            if (h >= 0 && h <= r && sameSide &&
                (best < 0 || h > headerRow[best])) {
                best = s;
            }
        }
        return best;
    };

    for (auto child : grid.Children()) {
        auto fe = child.try_as<FrameworkElement>();
        if (!fe || child == g_attach.host) {
            continue;
        }
        int r = xc::Grid::GetRow(fe);
        int span = xc::Grid::GetRowSpan(fe);
        if (r < ins && r + span - 1 >= ins) {
            continue;  // Spans our row too (e.g. a background).
        }
        int s = sectionOf(r);
        if (s >= 0 && !show[s]) {
            HideSectionElement(fe);
        }
    }

    if (!show[2]) {
        // The header sits in the list's own ScrollViewer, a few levels down.
        int depth = 0;
        for (auto p = xm::VisualTreeHelper::GetParent(grid); p && depth < 30;
             p = xm::VisualTreeHelper::GetParent(p), depth++) {
            if (auto list = p.try_as<xc::ListViewBase>()) {
                HideSectionElement(list.ItemsPanelRoot());
                break;
            }
        }
    }

    for (int s = 0; s < 3; s++) {
        if (!show[s] && headerRow[s] < 0) {
            Wh_Log(L"Section %d header not found", s);
        }
    }
}

// The width the groups get in Start, saved for the settings window (another
// process) so its preview lays the tiles out in rows like Start does.
constexpr PCWSTR kStartRowWidthValue = L"startRowWidth";

// The width the row of groups is fitted to: the host's, in whole DIPs
// (rounded down, so the row is never a fraction wider than the host and
// shrunk by the Viewbox for it). 0 before the host is laid out.
double HostRowWidth() {
    double w = g_attach.host ? g_attach.host.ActualWidth() : 0;
    return w > 0 ? std::floor(w) : 0;
}

// Lays the row out for the host's current width: the tiles are fitted to it
// (FitRow), so they're built again when the width changes. Called after
// every layout pass; returns early when nothing changed.
void ApplyColumns() {
    auto wrap = g_attach.wrap.get();
    auto frame = g_attach.wrapFrame.get();
    if (!wrap || !frame || !g_model || !g_attach.host) {
        return;
    }
    double width = HostRowWidth();
    if (width <= 0 || width == g_attach.builtWidth) {
        return;
    }
    static int published = -1;
    if ((int)width != published) {
        published = (int)width;
        Wh_SetIntValue(kStartRowWidthValue, published);
    }
    // Guard against a host whose width would follow the tiles: a few
    // rebuilds in a row, then the tiles stay as they are (the Viewbox still
    // fits them).
    static ULONGLONG windowStart = 0;
    static int rebuilds = 0;
    ULONGLONG now = GetTickCount64();
    if (now - windowStart > 2000) {
        windowStart = now;
        rebuilds = 0;
    }
    if (++rebuilds > 6) {
        if (rebuilds == 7) {
            Wh_Log(L"Row: the width keeps changing (%.1f), not refitting",
                   width);
        }
        return;
    }
    g_attach.builtWidth = width;  // once per change
    // Not from inside this layout pass.
    PostToUi([] { RebuildHostContent(); });
}

// --- Rearranging in Start -----------------------------------------------------
// Groups and apps are dragged with the left mouse button, right in Start: a
// tile by its background (or an app on its own, a 1 x 1 tile) moves the
// group, an app moves the app, within its group or to another one. Dropped
// on the middle of another app, the two make a folder; on a folder, it goes
// in. In an opened folder (or group), its apps are dragged to reorder them,
// or out of it, onto the groups. As in the settings window, dragging uses
// pointer capture rather than XAML drag and drop. The new order is saved in
// the mod's storage (see "Order arranged in Start") and shown right away.

// Defined with the settings window.
wf::Rect BoundsIn(FrameworkElement const& el, UIElement const& root);
bool RectContains(wf::Rect const& r, wf::Point const& p);
winrt::Windows::UI::Color AccentColor();

bool IsSingleAppTile(Group const& g) {
    return g.layout.spanHalfColumns == 1 && g.layout.spanHalfRows == 1;
}

// The app being dragged: an app on its own (a group drag of its tile), an
// app of a tile, or an app of one of its folders.
AppEntry const* DraggedEntry(StartDrag const& d) {
    if (!d.model || d.tile < 0 || d.tile >= (int)d.model->groups.size()) {
        return nullptr;
    }
    auto const& cat = d.model->groups[d.tile];
    if (d.group) {
        return IsSingleAppTile(cat) && !cat.apps.empty() ? &cat.apps.front()
                                                         : nullptr;
    }
    if (d.app < 0 || d.app >= (int)cat.apps.size()) {
        return nullptr;
    }
    auto const& app = cat.apps[d.app];
    if (d.child < 0) {
        return &app;
    }
    return d.child < (int)app.children.size() ? &app.children[d.child]
                                              : nullptr;
}

// A drop, applied once the drag's pointer events are done (see StartDrag).
struct StartDrop {
    bool group = false;
    bool intoGroup = false;
    int tile = -1, app = -1, child = -1;
    int targetTile = -1, targetApp = -1, targetChild = -1;
    bool after = false;
    bool newGroup = false;
    bool makeFolder = false;
    bool intoFolder = false;
    bool fromFlyout = false;  // dropped in the opened folder or group
    std::shared_ptr<Model> model;
};

// The model with its groups and apps in the order of `groups` (the settings
// after a drop). Its apps are already resolved and their icons loaded.
std::shared_ptr<Model> ReorderedModel(Model const& old,
                                      std::vector<GroupSetting> const& groups) {
    auto m = std::make_shared<Model>();
    m->layout = old.layout;
    m->allApps = old.allApps;
    m->icons = old.icons;
    m->settingsGeneration = old.settingsGeneration;
    std::map<RawId, AppEntry const*> apps;
    for (auto const& g : old.groups) {
        for (auto const& a : g.apps) {
            apps.emplace(IdOf(a), &a);
            for (auto const& c : a.children) {
                apps.emplace(IdOf(c), &c);
            }
        }
    }
    for (int k = 0; k < (int)groups.size(); k++) {
        Group cat = GroupFromSetting(groups[k]);
        for (int j = 0; j < (int)groups[k].items.size(); j++) {
            auto const& item = groups[k].items[j];
            auto it = apps.find(IdOf(item));
            AppEntry entry;
            if (it != apps.end()) {
                entry = *it->second;
            } else if (item.type == L"folder") {
                // Made in Start (see ApplyStartDrop).
                entry.folder = true;
                entry.name = item.label.empty() ? kFolderName : item.label;
                entry.key = L"f:" + entry.name;
            } else {
                continue;  // not found in Start
            }
            entry.rawGroup = item.rawGroup;
            entry.rawItem = item.rawItem;
            entry.rawSub = item.rawSub;
            entry.settingsGroup = k;
            entry.settingsItem = j;
            if (item.type == L"folder") {
                entry.folder = true;
                entry.children.clear();
                for (auto const& c : item.children) {
                    auto ci = apps.find(IdOf(c));
                    if (ci == apps.end() || ci->second->folder) {
                        continue;
                    }
                    AppEntry child = *ci->second;
                    child.rawGroup = c.rawGroup;
                    child.rawItem = c.rawItem;
                    child.rawSub = c.rawSub;
                    entry.children.push_back(std::move(child));
                }
                if (entry.children.empty()) {
                    continue;  // as in LoadModel
                }
            }
            // A folder's apps "Edit" the folder (see LoadModel).
            for (auto& child : entry.children) {
                child.settingsGroup = k;
                child.settingsItem = j;
            }
            cat.apps.push_back(std::move(entry));
        }
        m->groups.push_back(std::move(cat));
    }
    FinishModelGroups(*m);
    return m;
}

void RefreshStartFlyout();

void ApplyStartDrop(StartDrop const& d) {
    auto model = g_model;
    if (!model || model != d.model) {
        return;  // rebuilt since (settings changed)
    }
    auto const& mg = model->groups;
    auto validTile = [&](int t) { return t >= 0 && t < (int)mg.size(); };
    if (!validTile(d.tile) || !validTile(d.targetTile) ||
        d.targetApp >= (int)mg[d.targetTile].apps.size()) {
        return;
    }
    std::wstring value;
    std::vector<GroupSetting> groups;
    {
        std::lock_guard lock(g_settingsMutex);
        if (g_settings.generation != model->settingsGeneration) {
            return;  // read again since; the model follows shortly
        }
        // Changed on a copy, kept only if it all works out.
        auto gs = g_settings.groups;
        auto findGroup = [&](int raw) {
            for (int k = 0; k < (int)gs.size(); k++) {
                if (gs[k].raw == raw) {
                    return k;
                }
            }
            return -1;
        };
        auto findIn = [](std::vector<ItemSetting> const& items, RawId id) {
            for (int i = 0; i < (int)items.size(); i++) {
                if (IdOf(items[i]) == id) {
                    return i;
                }
            }
            return -1;
        };
        int sg = findGroup(mg[d.tile].rawGroup);
        int tg = findGroup(mg[d.targetTile].rawGroup);
        if (sg < 0 || tg < 0) {
            return;
        }
        if (d.group && !d.intoGroup) {
            // A group, before or after another.
            int to = d.after ? tg + 1 : tg;
            if (to > sg) {
                to--;
            }
            if (to == sg) {
                return;
            }
            GroupSetting moved = std::move(gs[sg]);
            gs.erase(gs.begin() + sg);
            gs.insert(gs.begin() + to, std::move(moved));
        } else {
            // An app (or an app on its own), found by its identity: the
            // places it's taken from and put at shift as it moves.
            StartDrag sd;
            sd.model = model;
            sd.group = d.group;
            sd.tile = d.tile;
            sd.app = d.app;
            sd.child = d.child;
            AppEntry const* src = DraggedEntry(sd);
            if (!src) {
                return;
            }
            const RawId srcId = IdOf(*src);
            const std::wstring srcName = src->name;
            std::optional<RawId> targetItem, targetChild;
            if (d.targetApp >= 0) {
                auto const& ta = mg[d.targetTile].apps[d.targetApp];
                targetItem = IdOf(ta);
                if (d.targetChild >= 0) {
                    if (d.targetChild >= (int)ta.children.size()) {
                        return;
                    }
                    targetChild = IdOf(ta.children[d.targetChild]);
                }
            }
            if (targetItem == srcId || targetChild == srcId) {
                return;  // onto itself
            }
            // Take it out, from its group or its folder.
            ItemSetting moved;
            std::optional<RawId> fromFolder;
            bool found = false;
            for (auto& g : gs) {
                for (int i = 0; i < (int)g.items.size() && !found; i++) {
                    auto& item = g.items[i];
                    if (IdOf(item) == srcId) {
                        moved = std::move(item);
                        g.items.erase(g.items.begin() + i);
                        found = true;
                        break;
                    }
                    int c = findIn(item.children, srcId);
                    if (c >= 0) {
                        moved = std::move(item.children[c]);
                        item.children.erase(item.children.begin() + c);
                        fromFolder = IdOf(item);
                        found = true;
                    }
                }
                if (found) {
                    break;
                }
            }
            if (!found) {
                return;
            }
            bool isFolder = moved.type == L"folder";
            auto& dest = gs[tg].items;
            if (d.newGroup) {
                // A group of its own (an app on its own), with a raw index
                // no other group has (see MakeStartGroup).
                int made = 0;
                for (auto const& g : gs) {
                    if (g.raw <= -2) {
                        made = std::max(made, -1 - g.raw);
                    }
                }
                GroupSetting g =
                    MakeStartGroup(srcName, made, (GroupSetting*)nullptr);
                g.items.push_back(std::move(moved));
                gs.insert(gs.begin() + (d.after ? tg + 1 : tg), std::move(g));
            } else if (d.makeFolder) {
                // Onto an app: a folder of the two, in its place, with a raw
                // index no other folder has (see ApplyStartOrder).
                int ti = targetItem ? findIn(dest, *targetItem) : -1;
                if (ti < 0 || isFolder || dest[ti].type == L"folder") {
                    return;
                }
                int id = -2;
                for (auto const& g : gs) {
                    for (auto const& item : g.items) {
                        if (item.rawGroup == -1) {
                            id = std::min(id, item.rawItem - 1);
                        }
                    }
                }
                ItemSetting folder;
                folder.type = L"folder";
                folder.label = kFolderName;
                folder.rawGroup = -1;
                folder.rawItem = id;
                folder.children.push_back(std::move(dest[ti]));
                folder.children.push_back(std::move(moved));
                dest[ti] = std::move(folder);
            } else if (d.intoFolder) {
                // Into a folder: before or after one of its apps, or last.
                int ti = targetItem ? findIn(dest, *targetItem) : -1;
                if (ti < 0 || isFolder || dest[ti].type != L"folder") {
                    return;
                }
                auto& apps = dest[ti].children;
                int at = (int)apps.size();
                if (targetChild) {
                    int ci = findIn(apps, *targetChild);
                    if (ci < 0) {
                        return;
                    }
                    at = d.after ? ci + 1 : ci;
                }
                apps.insert(apps.begin() + at, std::move(moved));
            } else {
                // Into a group: before or after one of its apps, or last.
                int at = (int)dest.size();
                if (targetItem) {
                    int ti = findIn(dest, *targetItem);
                    if (ti < 0) {
                        return;
                    }
                    at = d.after ? ti + 1 : ti;
                }
                dest.insert(dest.begin() + at, std::move(moved));
            }
            // What it leaves empty goes: the folder it was taken from, or
            // its app-on-its-own tile.
            if (fromFolder) {
                for (auto& g : gs) {
                    std::erase_if(g.items, [&](ItemSetting const& item) {
                        return IdOf(item) == *fromFolder &&
                               item.children.empty();
                    });
                }
            }
            int from = findGroup(mg[d.tile].rawGroup);
            if (from >= 0 && gs[from].items.empty() &&
                IsSingleApp(gs[from])) {
                gs.erase(gs.begin() + from);
            }
        }
        g_settings.groups = gs;
        value = StartOrderValue(g_settings);
        groups = std::move(gs);
    }
    // Back in the saved order: nothing to keep.
    if (value.empty()) {
        Wh_DeleteValue(kStartOrderValue);
    } else if (!Wh_SetStringValue(kStartOrderValue, value.c_str())) {
        Wh_Log(L"Saving the order arranged in Start failed");
    }
    g_model = ReorderedModel(*model, groups);
    RebuildHostContent();
    if (d.fromFlyout) {
        RefreshStartFlyout();
    }
}

// Where the drag draws: the opened folder's layer while it's in there, else
// the groups'.
xc::Grid DragSurfaceRoot(StartDrag const& d) {
    return d.inFlyout ? g_attach.flyout.root.get() : g_attach.dragRoot.get();
}
xc::Canvas DragSurfaceOverlay(StartDrag const& d) {
    return d.inFlyout ? g_attach.flyout.overlay.get()
                      : g_attach.dragOverlay.get();
}

// Ends the drag in Start: drops it (`drop`) or cancels it.
void EndStartDrag(bool drop) {
    StartDrag d = std::move(g_attach.drag);
    g_attach.drag = StartDrag{};
    for (auto overlay :
         {g_attach.dragOverlay.get(), g_attach.flyout.overlay.get()}) {
        if (overlay) {
            overlay.Children().Clear();
        }
    }
    if (auto src = d.source.get()) {
        src.Opacity(1);
    }
    if (!drop || !d.active || d.targetTile < 0) {
        return;
    }
    StartDrop dropped;
    dropped.group = d.group;
    dropped.intoGroup = d.intoGroup;
    dropped.tile = d.tile;
    dropped.app = d.app;
    dropped.child = d.child;
    dropped.targetTile = d.targetTile;
    dropped.targetApp = d.targetApp;
    dropped.targetChild = d.targetChild;
    dropped.after = d.after;
    dropped.newGroup = d.newGroup;
    dropped.makeFolder = d.makeFolder;
    dropped.intoFolder = d.intoFolder;
    dropped.fromFlyout = d.inFlyout;
    dropped.model = d.model;
    // After this pointer event: rebuilding the tiles removes the element
    // that has the pointer capture.
    PostToUi([dropped] { ApplyStartDrop(dropped); });
}

void StartDragVisuals() {
    auto& d = g_attach.drag;
    auto root = DragSurfaceRoot(d);
    auto overlay = DragSurfaceOverlay(d);
    if (!root || !overlay || !d.model || d.tile < 0 ||
        d.tile >= (int)d.model->groups.size()) {
        return;
    }
    auto const& cat = d.model->groups[d.tile];
    auto target = g_attach.target.get();
    bool dark = target ? target.ActualTheme() == ElementTheme::Dark : true;

    d.indicator = xc::Border();
    d.indicator.BorderBrush(xm::SolidColorBrush(AccentColor()));
    d.indicator.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
    d.indicator.Visibility(Visibility::Collapsed);
    d.indicator.IsHitTestVisible(false);
    overlay.Children().Append(d.indicator);

    FrameworkElement source{nullptr};
    if (auto src = d.source.get()) {
        source = src.try_as<FrameworkElement>();
    }
    xc::Border ghost;
    ghost.IsHitTestVisible(false);
    AppEntry const* app = d.group ? nullptr : DraggedEntry(d);
    if (app) {
        // The app's icon, at its size where it was grabbed (taken out of
        // the opened folder: its size in the tiles).
        wf::Rect sb{};
        if (source) {
            sb = BoundsIn(source, root);
        } else {
            float size = (float)((d.model->layout.iconSize +
                                  2 * d.model->layout.appPaddingX) *
                                 d.scale);
            sb = {d.start.X - size / 2, d.start.Y - size / 2, size, size};
        }
        ghost.Width(sb.Width);
        ghost.Height(sb.Height);
        ghost.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
        ghost.Background(xm::SolidColorBrush(
            dark ? winrt::Windows::UI::Color{0x30, 255, 255, 255}
                 : winrt::Windows::UI::Color{0x18, 0, 0, 0}));
        double icon = std::max(
            12.0, std::min((double)sb.Height,
                           d.model->layout.iconSize *
                               (d.inFlyout ? 1.0 : d.scale)));
        auto img = MakeAppIcon(*d.model, *app, icon);
        img.HorizontalAlignment(HorizontalAlignment::Center);
        img.VerticalAlignment(VerticalAlignment::Center);
        ghost.Child(img);
        ghost.Opacity(0.85);
        d.grab = {d.start.X - sb.X, d.start.Y - sb.Y};
    } else {
        // The group's name (an app on its own: the app's).
        ghost.Background(xm::SolidColorBrush(
            dark ? winrt::Windows::UI::Color{0xF0, 0x2B, 0x2B, 0x2B}
                 : winrt::Windows::UI::Color{0xF0, 0xFF, 0xFF, 0xFF}));
        ghost.BorderBrush(xm::SolidColorBrush(
            dark ? winrt::Windows::UI::Color{0x30, 255, 255, 255}
                 : winrt::Windows::UI::Color{0x20, 0, 0, 0}));
        ghost.BorderThickness(ThicknessHelper::FromUniformLength(1));
        ghost.CornerRadius(CornerRadiusHelper::FromUniformRadius(6));
        ghost.Padding(ThicknessHelper::FromLengths(10, 5, 10, 5));
        xc::TextBlock text;
        text.Text(IsSingleAppTile(cat) && !cat.apps.empty()
                      ? cat.apps.front().name
                      : cat.name);
        text.FontSize(12);
        text.TextWrapping(TextWrapping::NoWrap);
        ghost.Child(text);
        d.grab = {-14, -14};
    }
    d.ghost = ghost;
    overlay.Children().Append(ghost);
    if (auto src = d.source.get()) {
        src.Opacity(0.4);
    }
}

void UpdateStartDrag(wf::Point p);

// The drag of an app out of the opened folder (or group): on over the
// groups, where it goes on like a drag that started there. The popup closes.
void HandStartDragToGroups(wf::Point p) {
    auto& d = g_attach.drag;
    auto froot = g_attach.flyout.root.get();
    auto root = g_attach.dragRoot.get();
    if (!froot || !root) {
        EndStartDrag(false);
        return;
    }
    wf::Point gp, start;
    try {
        auto t = froot.TransformToVisual(root);
        gp = t.TransformPoint(p);
        start = gp;
    } catch (...) {
        EndStartDrag(false);
        return;
    }
    if (auto overlay = g_attach.flyout.overlay.get()) {
        overlay.Children().Clear();
    }
    if (auto src = d.source.get()) {
        src.Opacity(1);
    }
    d.source = nullptr;
    d.ghost = nullptr;
    d.indicator = nullptr;
    d.inFlyout = false;
    d.start = start;
    // The groups are shrunk to fit Start; by how much.
    d.scale = 1;
    for (auto const& t : g_attach.tiles) {
        if (auto tileEl = t.tile.get(); tileEl && tileEl.ActualWidth() > 0) {
            d.scale = BoundsIn(tileEl, root).Width / tileEl.ActualWidth();
            break;
        }
    }
    StartDragVisuals();
    if (d.pointer) {
        root.CapturePointer(d.pointer);
    }
    if (auto f = g_attach.flyout.fly.get()) {
        f.Hide();
    }
    UpdateStartDrag(gp);
}

// In the opened folder (or group): before or after one of its apps.
void UpdateStartFlyoutDrag(wf::Point p) {
    auto& d = g_attach.drag;
    auto& fs = g_attach.flyout;
    auto root = fs.root.get();
    if (!root) {
        return;
    }
    // Out of it (with a little margin): on over the groups.
    double w = root.ActualWidth(), h = root.ActualHeight();
    if (p.X < -8 || p.Y < -8 || p.X > w + 8 || p.Y > h + 8) {
        HandStartDragToGroups(p);
        return;
    }
    // The dragged app's place in the list shown.
    int self = fs.folder >= 0 ? d.child : d.app;
    for (auto const& [weakBtn, i] : fs.items) {
        auto btn = weakBtn.get();
        if (!btn) {
            continue;
        }
        auto cb = BoundsIn(btn, root);
        if (!RectContains(cb, p) || i == self) {
            continue;
        }
        d.after = p.X > cb.X + cb.Width / 2;
        d.targetTile = fs.tile;
        if (fs.folder >= 0) {
            d.intoFolder = true;
            d.targetApp = fs.folder;
            d.targetChild = i;
        } else {
            d.targetApp = i;
        }
        if (auto ind = d.indicator) {
            ind.Visibility(Visibility::Visible);
            ind.Width(3);
            ind.Height(std::max(0.0f, cb.Height - 8));
            xc::Canvas::SetLeft(
                ind, d.after ? cb.X + cb.Width - 1.5f : cb.X - 1.5f);
            xc::Canvas::SetTop(ind, cb.Y + 4);
            ind.Background(xm::SolidColorBrush(AccentColor()));
            ind.BorderThickness(ThicknessHelper::FromUniformLength(0));
        }
        return;
    }
}

void UpdateStartDrag(wf::Point p) {
    auto& d = g_attach.drag;
    auto root = DragSurfaceRoot(d);
    if (!root || !d.model) {
        return;
    }
    if (d.ghost) {
        xc::Canvas::SetLeft(d.ghost, p.X - d.grab.X);
        xc::Canvas::SetTop(d.ghost, p.Y - d.grab.Y);
    }
    auto show = [&](wf::Rect r, bool outline) {
        auto ind = d.indicator;
        if (!ind) {
            return;
        }
        if (r.Width <= 0 || r.Height <= 0) {
            ind.Visibility(Visibility::Collapsed);
            return;
        }
        ind.Visibility(Visibility::Visible);
        ind.Width(r.Width);
        ind.Height(r.Height);
        xc::Canvas::SetLeft(ind, r.X);
        xc::Canvas::SetTop(ind, r.Y);
        if (outline) {
            ind.Background(nullptr);
            ind.BorderThickness(ThicknessHelper::FromUniformLength(2));
        } else {
            ind.Background(xm::SolidColorBrush(AccentColor()));
            ind.BorderThickness(ThicknessHelper::FromUniformLength(0));
        }
    };
    auto const& groups = d.model->groups;
    auto const& L = d.model->layout;
    d.targetTile = -1;
    d.targetApp = -1;
    d.targetChild = -1;
    d.after = false;
    d.newGroup = false;
    d.intoGroup = false;
    d.makeFolder = false;
    d.intoFolder = false;

    if (d.inFlyout) {
        UpdateStartFlyoutDrag(p);
        if (d.targetTile < 0 && d.inFlyout) {
            show({}, false);
        }
        return;
    }

    AppEntry const* dragged = DraggedEntry(d);
    // Folders don't go into folders.
    bool draggingFolder = dragged && dragged->folder;

    // The nearest tile of the row under the pointer (on a tile, in a gap, or
    // under a tile where its name is), and its bounds; -1 if none.
    float gapX = (float)(L.tileSpacingX * d.scale / 2);
    float gapY = (float)(L.tileSpacingY * d.scale / 2);
    auto nearestInRow = [&](wf::Rect& best) {
        float bestDist = 1e9f;
        int bestTile = -1;
        for (auto const& t : g_attach.tiles) {
            auto outer = t.outer.get();
            auto tileEl = t.tile.get();
            if (!outer || !tileEl || t.group < 0 ||
                t.group >= (int)groups.size()) {
                continue;
            }
            auto ob = BoundsIn(outer, root);
            if (p.Y < ob.Y - gapY || p.Y > ob.Y + ob.Height + gapY) {
                continue;  // another row
            }
            auto b = BoundsIn(tileEl, root);
            float dist = p.X < b.X              ? b.X - p.X
                         : p.X > b.X + b.Width ? p.X - b.X - b.Width
                                               : 0;
            if (dist < bestDist) {
                bestDist = dist;
                best = b;
                bestTile = t.group;
            }
        }
        return bestTile;
    };
    // A bar in the gap before or after the tile at `b`.
    auto showBetween = [&](wf::Rect const& b, bool after) {
        show({after ? b.X + b.Width + gapX - 1.5f : b.X - gapX - 1.5f, b.Y, 3,
              b.Height},
             false);
    };
    // The app dragged (or an app on its own) dropped onto the tile `t` (at
    // `b`): over the middle of an app, a folder of the two (of a folder,
    // into it); else before or after the app under the pointer, and on the
    // overflow cluster or elsewhere on the tile, at the end. Onto an app on
    // its own, only the former.
    auto dropOnto = [&](StartTile const& t, wf::Rect const& b) {
        bool single = IsSingleAppTile(groups[t.group]);
        // In its own group (not out of one of its folders).
        bool sameGroup = t.group == d.tile && !d.group && d.child < 0;
        wf::Rect cb{};
        int hit = -1;
        for (auto const& [weakBtn, i] : t.apps) {
            auto btn = weakBtn.get();
            if (btn && RectContains(BoundsIn(btn, root), p)) {
                cb = BoundsIn(btn, root);
                hit = i;
                break;
            }
        }
        bool self = sameGroup && hit == d.app;
        if (hit >= 0 && !self && !draggingFolder &&
            hit < (int)groups[t.group].apps.size() &&
            p.X > cb.X + cb.Width / 4 && p.X < cb.X + cb.Width * 3 / 4) {
            d.targetTile = t.group;
            d.targetApp = hit;
            if (groups[t.group].apps[hit].folder) {
                d.intoFolder = true;
            } else {
                d.makeFolder = true;
            }
            show(cb, true);
            return;
        }
        if (single) {
            return;
        }
        if (L.sortAppsByName) {
            // Sorted anyway: only to another group, at the end.
            if (!sameGroup) {
                d.targetTile = t.group;
                show(b, true);
            }
            return;
        }
        if (self) {
            return;  // onto itself
        }
        d.targetTile = t.group;
        if (hit >= 0) {
            d.after = p.X > cb.X + cb.Width / 2;
            d.targetApp = hit;
            show({d.after ? cb.X + cb.Width - 1.5f : cb.X - 1.5f, cb.Y + 4, 3,
                  std::max(0.0f, cb.Height - 8)},
                 false);
        } else {
            show(b, true);
        }
    };

    if (!d.group) {
        bool overTile = false;
        for (auto const& t : g_attach.tiles) {
            auto tileEl = t.tile.get();
            if (!tileEl || t.group < 0 || t.group >= (int)groups.size()) {
                continue;
            }
            auto b = BoundsIn(tileEl, root);
            if (!RectContains(b, p)) {
                continue;
            }
            overTile = true;
            dropOnto(t, b);
            break;
        }
        // Out of the tiles, among the groups (or a little below them): a
        // group of its own, an app on its own (as in the settings window),
        // before or after the nearest tile of that row, or else at the end.
        float below = (float)(L.tileSize * d.scale);
        if (!overTile && !g_attach.tiles.empty() && p.X >= 0 &&
            p.X <= root.ActualWidth() && p.Y >= 0 &&
            p.Y <= root.ActualHeight() + below) {
            wf::Rect best{};
            int bestTile = nearestInRow(best);
            if (bestTile >= 0) {
                d.after = p.X >= best.X + best.Width / 2;
            } else {
                auto const& last = g_attach.tiles.back();
                auto tileEl = last.tile.get();
                if (tileEl && last.group >= 0 &&
                    last.group < (int)groups.size()) {
                    best = BoundsIn(tileEl, root);
                    bestTile = last.group;
                    d.after = true;
                }
            }
            if (bestTile >= 0) {
                d.targetTile = bestTile;
                d.newGroup = true;
                showBetween(best, d.after);
            }
        }
    } else {
        // An app on its own over another tile: into that group, or onto its
        // apps (as in the settings window). Near a group's sides, it's moved
        // before or after the tile, like a group.
        auto const& cat = groups[d.tile];
        if (IsSingleAppTile(cat) && !cat.apps.empty()) {
            for (auto const& t : g_attach.tiles) {
                auto tileEl = t.tile.get();
                if (!tileEl || t.group < 0 || t.group >= (int)groups.size() ||
                    t.group == d.tile) {
                    continue;
                }
                auto b = BoundsIn(tileEl, root);
                float edge = std::min(b.Width * 0.15f, 24.0f);
                if (RectContains(b, p) &&
                    (IsSingleAppTile(groups[t.group]) ||
                     (p.X > b.X + edge && p.X < b.X + b.Width - edge))) {
                    dropOnto(t, b);
                    d.intoGroup = d.targetTile >= 0;
                    break;
                }
            }
        }
        // Before or after the nearest tile of the row under the pointer.
        wf::Rect best{};
        int bestTile = d.intoGroup ? -1 : nearestInRow(best);
        if (bestTile >= 0 && bestTile != d.tile) {
            d.after = p.X >= best.X + best.Width / 2;
            d.targetTile = bestTile;
            showBetween(best, d.after);
        }
    }
    if (d.targetTile < 0) {
        show({}, false);
    }
}

void OnStartPointerPressed(Input::PointerRoutedEventArgs const& e) {
    auto root = g_attach.dragRoot.get();
    auto model = g_model;
    if (!root || !model || g_attach.drag.pressed ||
        e.Pointer().PointerDeviceType() !=
            winrt::Windows::Devices::Input::PointerDeviceType::Mouse) {
        return;
    }
    auto pt = e.GetCurrentPoint(root);
    if (!pt.Properties().IsLeftButtonPressed()) {
        return;
    }
    auto p = pt.Position();
    for (auto const& t : g_attach.tiles) {
        auto tileEl = t.tile.get();
        if (!tileEl || t.group < 0 || t.group >= (int)model->groups.size()) {
            continue;
        }
        auto b = BoundsIn(tileEl, root);
        if (!RectContains(b, p)) {
            continue;
        }
        StartDrag d;
        d.pressed = true;
        d.group = true;
        d.tile = t.group;
        d.model = model;
        d.start = p;
        d.pointer = e.Pointer();
        if (auto outer = t.outer.get()) {
            d.source = winrt::make_weak(outer.as<UIElement>());
        }
        // The tile is shrunk to fit Start; by how much.
        double w = tileEl.ActualWidth();
        d.scale = w > 0 && b.Width > 0 ? b.Width / w : 1;
        bool onButton = false;
        for (auto const& [weakBtn, i] : t.apps) {
            auto btn = weakBtn.get();
            if (btn && RectContains(BoundsIn(btn, root), p)) {
                onButton = true;
                // An app on its own moves as a group.
                if (!IsSingleAppTile(model->groups[t.group])) {
                    d.group = false;
                    d.app = i;
                    d.source = winrt::make_weak(btn.as<UIElement>());
                }
                break;
            }
        }
        if (auto c = t.cluster.get();
            c && RectContains(BoundsIn(c, root), p)) {
            onButton = true;  // the overflow cluster moves the group
        }
        g_attach.drag = std::move(d);
        // A button has the pointer captured already, and its moves reach
        // the root. Elsewhere on the tile, capture now so the moves are seen
        // over the gaps between the tiles too.
        if (!onButton) {
            root.CapturePointer(e.Pointer());
        }
        return;
    }
}

// The pointer events of a drag in Start, on the element holding the groups.
// Also the ones the tiles' buttons already handled.
void HookStartDrag(xc::Grid const& root) {
    root.AddHandler(
        UIElement::PointerPressedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [](wf::IInspectable const&,
               Input::PointerRoutedEventArgs const& e) {
                OnStartPointerPressed(e);
            })),
        true);
    root.AddHandler(
        UIElement::PointerMovedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [](wf::IInspectable const&,
               Input::PointerRoutedEventArgs const& e) {
                auto& d = g_attach.drag;
                auto root = g_attach.dragRoot.get();
                if (!d.pressed || d.inFlyout || !root) {
                    return;
                }
                auto p = e.GetCurrentPoint(root).Position();
                if (!d.active) {
                    float dx = p.X - d.start.X, dy = p.Y - d.start.Y;
                    if (dx * dx + dy * dy < 36) {
                        return;
                    }
                    if (d.model != g_model) {
                        EndStartDrag(false);
                        return;
                    }
                    d.active = true;
                    // Taken from the button (it then doesn't click).
                    root.CapturePointer(d.pointer);
                    StartDragVisuals();
                }
                e.Handled(true);
                UpdateStartDrag(p);
            })),
        true);
    root.AddHandler(
        UIElement::PointerReleasedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [](wf::IInspectable const&,
               Input::PointerRoutedEventArgs const& e) {
                auto root = g_attach.dragRoot.get();
                if (!g_attach.drag.pressed || g_attach.drag.inFlyout ||
                    !root) {
                    return;
                }
                if (!g_attach.drag.active) {
                    // A click: the button already did what it does.
                    g_attach.drag = StartDrag{};
                    root.ReleasePointerCapture(e.Pointer());
                    return;
                }
                e.Handled(true);
                UpdateStartDrag(e.GetCurrentPoint(root).Position());
                EndStartDrag(true);
                root.ReleasePointerCapture(e.Pointer());
            })),
        true);
    // Only the root's own capture: the button's, lost to the root when the
    // drag starts, bubbles here too.
    root.AddHandler(
        UIElement::PointerCaptureLostEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [](wf::IInspectable const&,
               Input::PointerRoutedEventArgs const& e) {
                auto root = g_attach.dragRoot.get();
                if (g_attach.drag.pressed && !g_attach.drag.inFlyout &&
                    root && e.OriginalSource() == root) {
                    EndStartDrag(false);
                }
            })),
        true);
}

// The pointer events of a drag in the opened folder (or group), on its
// content (`root`, see BuildGroupFlyout).
void HookStartFlyoutDrag(xc::Grid const& root) {
    winrt::weak_ref<xc::Grid> weakRoot = root;
    root.AddHandler(
        UIElement::PointerPressedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [weakRoot](wf::IInspectable const&,
                       Input::PointerRoutedEventArgs const& e) {
                auto root = weakRoot.get();
                auto& fs = g_attach.flyout;
                auto model = g_model;
                if (!root || root != fs.root.get() || !model ||
                    model != fs.model || g_attach.drag.pressed ||
                    fs.tile < 0 || fs.tile >= (int)model->groups.size() ||
                    e.Pointer().PointerDeviceType() !=
                        winrt::Windows::Devices::Input::PointerDeviceType::
                            Mouse) {
                    return;
                }
                auto pt = e.GetCurrentPoint(root);
                if (!pt.Properties().IsLeftButtonPressed()) {
                    return;
                }
                auto p = pt.Position();
                for (auto const& [weakBtn, i] : fs.items) {
                    auto btn = weakBtn.get();
                    if (!btn || !RectContains(BoundsIn(btn, root), p)) {
                        continue;
                    }
                    StartDrag d;
                    d.pressed = true;
                    d.inFlyout = true;
                    d.tile = fs.tile;
                    d.app = fs.folder >= 0 ? fs.folder : i;
                    d.child = fs.folder >= 0 ? i : -1;
                    d.model = model;
                    d.start = p;
                    d.pointer = e.Pointer();
                    d.source = winrt::make_weak(btn.as<UIElement>());
                    g_attach.drag = std::move(d);
                    return;
                }
            })),
        true);
    root.AddHandler(
        UIElement::PointerMovedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [weakRoot](wf::IInspectable const&,
                       Input::PointerRoutedEventArgs const& e) {
                auto& d = g_attach.drag;
                auto root = weakRoot.get();
                if (!d.pressed || !d.inFlyout || !root) {
                    return;
                }
                auto p = e.GetCurrentPoint(root).Position();
                if (!d.active) {
                    float dx = p.X - d.start.X, dy = p.Y - d.start.Y;
                    if (dx * dx + dy * dy < 36) {
                        return;
                    }
                    if (d.model != g_model) {
                        EndStartDrag(false);
                        return;
                    }
                    d.active = true;
                    // Taken from the button (it then doesn't click).
                    root.CapturePointer(d.pointer);
                    StartDragVisuals();
                }
                e.Handled(true);
                UpdateStartDrag(p);
            })),
        true);
    root.AddHandler(
        UIElement::PointerReleasedEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [weakRoot](wf::IInspectable const&,
                       Input::PointerRoutedEventArgs const& e) {
                auto root = weakRoot.get();
                if (!g_attach.drag.pressed || !g_attach.drag.inFlyout ||
                    !root) {
                    return;
                }
                if (!g_attach.drag.active) {
                    // A click: the button already did what it does.
                    g_attach.drag = StartDrag{};
                    root.ReleasePointerCapture(e.Pointer());
                    return;
                }
                e.Handled(true);
                UpdateStartDrag(e.GetCurrentPoint(root).Position());
                if (g_attach.drag.inFlyout) {
                    EndStartDrag(true);
                    root.ReleasePointerCapture(e.Pointer());
                }
            })),
        true);
    root.AddHandler(
        UIElement::PointerCaptureLostEvent(),
        winrt::box_value(Input::PointerEventHandler(
            [weakRoot](wf::IInspectable const&,
                       Input::PointerRoutedEventArgs const& e) {
                auto root = weakRoot.get();
                if (g_attach.drag.pressed && g_attach.drag.inFlyout && root &&
                    e.OriginalSource() == root) {
                    EndStartDrag(false);
                }
            })),
        true);
}

void RebuildHostContent() {
    auto host = g_attach.host;
    if (!host || !g_model) {
        return;
    }
    auto target = g_attach.target.get();
    bool dark = target ? target.ActualTheme() == ElementTheme::Dark : true;
    const auto& L = g_model->layout;

    // A drag in progress is of the tiles being replaced.
    if (g_attach.drag.pressed) {
        EndStartDrag(false);
    }
    // The left and right spacing may have changed; a new width builds the
    // tiles again (see ApplyColumns).
    host.Margin(HostMargin());
    g_attach.tiles.clear();
    g_imageSources.clear();

    // The row, fitted to the host's width (FitRow): the tiles are a little
    // wider or narrower than set so the row ends at the host's right edge.
    // Before the host is laid out, the set size; ApplyColumns builds the
    // tiles again once its width is known.
    const double rowWidth = HostRowWidth();
    const RowFit row = FitRow(rowWidth, L.tileSize, L.tileSpacingX);
    g_attach.builtWidth = rowWidth;

    xc::VariableSizedWrapGrid wrap;
    wrap.Orientation(xc::Orientation::Horizontal);
    wrap.ItemWidth(row.half);  // half cells
    double labelH = L.showLabels ? L.labelSpacing + L.labelFontSize * 1.4 : 0;
    wrap.ItemHeight((L.tileSize + labelH + L.tileSpacingY) / 2);  // half rows
    // Counted in half cells, so a narrow (1 x 2) tile still goes on a row
    // that has room for half a cell but not a whole one.
    wrap.MaximumRowsOrColumns(row.halves);
    wrap.HorizontalAlignment(HorizontalAlignment::Left);
    // Pull the trailing spacing out so the first/last tiles align to edges:
    // the frame is exactly as wide as the tiles of a row and their gaps
    // (with a pixel of slack for layout rounding).
    wrap.Margin(
        ThicknessHelper::FromLengths(0, 0, -(L.tileSpacingX + kRowSlack), 0));
    xc::Border frame;
    frame.HorizontalAlignment(HorizontalAlignment::Left);
    frame.Width(RowFrameWidth(row.halves, row.half, L.tileSpacingX));
    frame.Child(wrap);
    // A row that can't be fitted (Start narrower than the fewest tiles a
    // row holds) is scaled down as a whole, instead of wrapping to fewer
    // tiles per row. A fitted row is as wide as the host: not scaled.
    xc::Viewbox fit;
    fit.Stretch(xm::Stretch::Uniform);
    fit.StretchDirection(xc::StretchDirection::DownOnly);
    fit.HorizontalAlignment(HorizontalAlignment::Left);
    fit.Child(frame);
    g_attach.wrap = wrap;
    g_attach.wrapFrame = frame;

    for (int gi = 0; gi < (int)g_model->groups.size(); gi++) {
        // A tile wider than the row is built as wide as the row.
        Group cat = g_model->groups[gi];
        if (cat.layout.spanHalfColumns > row.halves) {
            cat.layout.spanHalfColumns = row.halves;
        }
        TileParts parts;
        auto tile = BuildTile(*g_model, cat, dark, row.tile, &parts, gi);
        StartTile st;
        st.group = gi;
        st.outer = tile;
        st.tile = parts.tile;
        for (auto const& [btn, i] : parts.apps) {
            st.apps.emplace_back(btn, i);
        }
        st.cluster = parts.cluster;
        g_attach.tiles.push_back(std::move(st));
        tile.HorizontalAlignment(HorizontalAlignment::Left);
        tile.VerticalAlignment(VerticalAlignment::Top);
        if (cat.layout.spanHalfColumns > 1) {
            xc::VariableSizedWrapGrid::SetColumnSpan(
                tile, cat.layout.spanHalfColumns);
        }
        if (cat.layout.spanHalfRows > 1) {
            xc::VariableSizedWrapGrid::SetRowSpan(tile,
                                                  cat.layout.spanHalfRows);
        }
        wrap.Children().Append(tile);
    }

    // Section heading, styled like Start's own "Pinned" heading.
    xc::StackPanel content;
    if (!L.sectionTitle.empty()) {
        xc::TextBlock heading;
        heading.Text(L.sectionTitle);
        heading.FontSize(14);
        heading.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        if (auto pinned = g_attach.pinnedHeader.get()) {
            heading.FontSize(pinned.FontSize());
            heading.FontWeight(pinned.FontWeight());
            heading.FontFamily(pinned.FontFamily());
            heading.Foreground(pinned.Foreground());
        }
        heading.Margin(ThicknessHelper::FromLengths(0, 0, 0, 16));
        content.Children().Append(heading);
    }
    content.Children().Append(fit);

    // With a layer on top for dragging (see "Rearranging in Start").
    xc::Grid root;
    root.Children().Append(content);
    xc::Canvas overlay;
    overlay.IsHitTestVisible(false);
    root.Children().Append(overlay);
    g_attach.dragRoot = root;
    g_attach.dragOverlay = overlay;
    HookStartDrag(root);

    if (auto sv = host.try_as<xc::ScrollViewer>()) {
        sv.Content(root);
    } else if (auto border = host.try_as<xc::Border>()) {
        border.Child(root);
    }

    // Checked after every layout pass, not just on SizeChanged: the grid can
    // be built before the host has its final width and then never see the
    // host resize. ApplyColumns returns early when nothing changed.
    ApplyColumns();
    if (!g_attach.sizeToken) {
        g_attach.sizeToken =
            host.LayoutUpdated([](auto&&, auto&&) { ApplyColumns(); });
    }

    // Right-click anywhere on the groups (tiles, icons, names).
    host.ContextFlyout(BuildContextMenu());

    ApplySectionVisibility();
}

// Space above and below the groups, like between Start's own sections.
constexpr double kGapTop = 24;
constexpr double kGapBottom = 8;

// The host's margin: Start's inset (aligned with its "Pinned" title, the
// same on the right), moved by the left and right spacing set.
winrt::Windows::UI::Xaml::Thickness HostMargin() {
    double left = g_attach.inset, right = g_attach.inset;
    if (g_model) {
        left += g_model->layout.marginLeft;
        right += g_model->layout.marginRight;
    }
    return {left, kGapTop, right, kGapBottom};
}

void Detach() {
    auto target = g_attach.target.get();
    RestoreSections();
    for (auto& [weak, token] : g_attach.unloadedTokens) {
        if (auto el = weak.get()) {
            el.Unloaded(token);
        }
    }
    g_attach.unloadedTokens.clear();
    if (g_attach.host && g_attach.sizeToken) {
        g_attach.host.LayoutUpdated(g_attach.sizeToken);
    }
    if (target && g_attach.themeToken) {
        target.ActualThemeChanged(g_attach.themeToken);
    }
    if (auto parent = g_attach.parent.get(); parent && g_attach.host) {
        uint32_t idx;
        if (parent.Children().IndexOf(g_attach.host, idx)) {
            parent.Children().RemoveAt(idx);
        }
    }
    if (auto list = g_attach.footerList.get();
        list && g_attach.host && list.Footer() == g_attach.host) {
        list.Footer(nullptr);
    }
    // Undo the inserted grid row: shift Start's rows back up.
    if (auto grid = g_attach.rowGrid.get(); grid && g_attach.insertedRow >= 0) {
        int ins = g_attach.insertedRow;
        for (auto child : grid.Children()) {
            auto c = child.try_as<FrameworkElement>();
            if (!c) {
                continue;
            }
            int r = xc::Grid::GetRow(c);
            int span = xc::Grid::GetRowSpan(c);
            if (r > ins) {
                xc::Grid::SetRow(c, r - 1);
            } else if (r < ins && r + span - 1 >= ins) {
                xc::Grid::SetRowSpan(c, span - 1);
            }
        }
        if ((int)grid.RowDefinitions().Size() > ins) {
            grid.RowDefinitions().RemoveAt(ins);
        }
    }
    g_attach = Attachment{};
    g_imageSources.clear();
}

void ReloadAsync();
void RequestScan();

// --- Keeping the attachment valid ---------------------------------------------
// Start rebuilds parts of its tree at runtime, e.g. when the "All" view is
// switched between Grid / List / Group. The elements the groups are attached
// to then get Unloaded; the check runs later (posted, coalesced), once Start
// is done changing its tree, and looks at what's actually there by then. If
// the groups' place is gone, they're removed, and the next scan of Start's
// tree (RequestScan) attaches them to the new one.

bool g_validateQueued = false;  // UI thread only

void ValidateAttachment() {
    if (!g_attach.host || g_unloading) {
        return;
    }
    auto target = g_attach.target.get();
    bool stale = !target;
    if (!stale) {
        if (g_attach.insertedRow >= 0) {
            auto grid = g_attach.rowGrid.get();
            uint32_t idx;
            stale = !grid || xm::VisualTreeHelper::GetParent(target) != grid ||
                    !grid.Children().IndexOf(g_attach.host, idx) ||
                    !grid.IsLoaded();
        } else {
            auto list = g_attach.footerList.get();
            stale = !list || list.Footer() != g_attach.host ||
                    !xm::VisualTreeHelper::GetParent(target) ||
                    !target.IsLoaded();
        }
    }
    if (!stale) {
        // Start may have re-created the app list's panel; hide it again.
        ApplySectionVisibility();
        return;
    }
    Wh_Log(L"Start's layout changed, attaching again");
    Detach();
    RequestScan();
}

void ScheduleValidate() {
    if (g_validateQueued || !g_attach.host) {
        return;
    }
    g_validateQueued = true;
    PostToUi([] {
        g_validateQueued = false;
        ValidateAttachment();
    });
}

void WatchUnloaded(FrameworkElement const& el) {
    if (!el) {
        return;
    }
    auto token = el.Unloaded([](auto&&, auto&&) { ScheduleValidate(); });
    g_attach.unloadedTokens.emplace_back(winrt::make_weak(el), token);
}

// Loads config + icons off the UI thread, then swaps the model in.
std::atomic<int> g_reloadGeneration{0};

void ReloadAsync() {
    int gen = ++g_reloadGeneration;
    RunWorker([gen] {
        auto model = LoadModel();
        if (!model || g_unloading || gen != g_reloadGeneration) {
            return;
        }
        PostToUi([model] {
            g_model = model;
            RebuildHostContent();
        });
    });
}

// --- Placing the groups -------------------------------------------------------
// Start's Pinned / Recommended / All headers and lists are rows of one Grid
// ("TopLevelHeader"). It's found from the "Pinned" header text, or else from
// the names of Start's pinned header and list (in any display language), see
// FindPinnedHeader. The groups go in a row of their own there.

bool IsPinnedHeaderText(std::wstring_view text) {
    std::lock_guard lock(g_settingsMutex);
    for (const auto& h : g_settings.pinnedHeaders) {
        if (h.size() == text.size() &&
            CompareStringOrdinal(h.data(), (int)h.size(), text.data(),
                                 (int)text.size(), TRUE) == CSTR_EQUAL) {
            return true;
        }
    }
    return false;
}

void LogAncestors(FrameworkElement el) {
    int depth = 0;
    for (DependencyObject p = el; p && depth < 14;
         p = xm::VisualTreeHelper::GetParent(p), depth++) {
        auto fe = p.try_as<FrameworkElement>();
        uint32_t children = 0;
        if (auto panel = p.try_as<xc::Panel>()) {
            children = panel.Children().Size();
        }
        Wh_Log(L"[ancestor %d] %s name=%s h=%.0f children=%u", depth,
               winrt::get_class_name(p).c_str(),
               fe ? fe.Name().c_str() : L"", fe ? fe.ActualHeight() : 0.0,
               children);
    }
}

bool SubtreeHasName(DependencyObject root, std::wstring_view name, int depth) {
    if (auto fe = root.try_as<FrameworkElement>(); fe && fe.Name() == name) {
        return true;
    }
    if (depth <= 0) {
        return false;
    }
    int n = xm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        if (SubtreeHasName(xm::VisualTreeHelper::GetChild(root, i), name,
                           depth - 1)) {
            return true;
        }
    }
    return false;
}

xc::Grid FindHeaderGrid(FrameworkElement el, FrameworkElement* branchOut);

// Start's Pinned / Recent / All headers are rows of one Grid
// ("TopLevelHeader", the header of the AllAppsGrid GridView). Insert a new
// Auto row where the "Where to show the groups" setting says (before or
// after the Pinned section, or before the All header), so Start's own layout
// pushes everything below down and it all scrolls together. After All, the
// groups go in the GridView's footer instead, below the whole app list.
bool AttachInHeaderGrid(FrameworkElement header) {
    FrameworkElement branch{nullptr};  // child of `grid` containing header
    auto grid = FindHeaderGrid(header, &branch);
    if (!grid) {
        Wh_Log(L"No header grid found above the Pinned header");
        return false;
    }

    // The row to insert after: the one holding the pinned list itself.
    FrameworkElement pinned{nullptr};
    for (auto c : grid.Children()) {
        if (SubtreeHasName(c, L"StartMenuPinnedList", 6) ||
            SubtreeHasName(c, L"PinnedList", 6)) {
            pinned = c.try_as<FrameworkElement>();
            break;
        }
    }
    if (!pinned) {
        pinned = branch;
    }
    int row = xc::Grid::GetRow(pinned) + xc::Grid::GetRowSpan(pinned) - 1;
    if ((int)grid.RowDefinitions().Size() <= row) {
        Wh_Log(L"Header grid has %u rows, pinned list is in row %d",
               grid.RowDefinitions().Size(), row);
        return false;
    }

    std::wstring placement;
    std::vector<std::wstring> allHeaders;
    {
        std::lock_guard lock(g_settingsMutex);
        placement = g_settings.placement;
        allHeaders = g_settings.allHeaders;
    }
    const std::wstring requestedPlacement = placement;

    // afterAll: the footer of the All list, which the header grid is the
    // header of. Falls back to after Pinned if there's no free footer.
    xc::ListViewBase footerList{nullptr};
    if (placement == L"afterAll") {
        int depth = 0;
        for (auto p = xm::VisualTreeHelper::GetParent(grid); p && depth < 30;
             p = xm::VisualTreeHelper::GetParent(p), depth++) {
            if (auto list = p.try_as<xc::ListViewBase>()) {
                if (!list.Footer()) {
                    footerList = list;
                }
                break;
            }
        }
        if (!footerList) {
            Wh_Log(L"No free footer on the All list, showing after Pinned");
            placement = L"afterPinned";
        }
    }

    // Where the row goes: before Pinned, at its header's row; after
    // Recommended / Recent, at the All header's row (the end without one);
    // else after the pinned list.
    int ins = row + 1;
    if (placement == L"beforePinned") {
        ins = std::min(xc::Grid::GetRow(branch), xc::Grid::GetRow(pinned));
    } else if (placement == L"afterRecent") {
        ins = (int)grid.RowDefinitions().Size();
        for (auto c : grid.Children()) {
            auto fe = c.try_as<FrameworkElement>();
            int r = fe ? xc::Grid::GetRow(fe) : -1;
            if (r > row && r < ins && SubtreeHasText(c, allHeaders, 10)) {
                ins = r;
            }
        }
    }

    if (g_attach.host) {
        Detach();
    }

    xc::Border host;
    host.Name(L"WindhawkStartMenuGroups");
    // Align with the "Pinned" title like Start's own sections do: its left
    // inset inside the header grid, mirrored on the right.
    double inset = pinned.Margin().Left;
    try {
        auto pt = header.TransformToVisual(grid).TransformPoint({0, 0});
        if (pt.X > 0 && pt.X < grid.ActualWidth() / 3) {
            inset = pt.X;
        }
    } catch (...) {
    }
    g_attach.inset = inset;
    host.Margin(HostMargin());
    host.HorizontalAlignment(HorizontalAlignment::Stretch);

    if (footerList) {
        footerList.Footer(host);
        g_attach.target = pinned;
        g_attach.host = host;
        g_attach.rowGrid = grid;
        g_attach.insertedRow = -1;
        g_attach.footerList = footerList;
        g_attach.placement = requestedPlacement;
        if (auto tb = header.try_as<xc::TextBlock>()) {
            g_attach.pinnedHeader = tb;
        }
        g_attach.themeToken = pinned.ActualThemeChanged(
            [](auto&&, auto&&) { RebuildHostContent(); });
        WatchUnloaded(grid);
        WatchUnloaded(pinned);
        if (g_model) {
            RebuildHostContent();
        } else {
            ReloadAsync();
        }
        Wh_Log(L"Inserted groups as the footer of the All list");
        return true;
    }

    for (auto child : grid.Children()) {
        auto c = child.try_as<FrameworkElement>();
        if (!c) {
            continue;
        }
        int r = xc::Grid::GetRow(c);
        int span = xc::Grid::GetRowSpan(c);
        if (r >= ins) {
            xc::Grid::SetRow(c, r + 1);
        } else if (r + span - 1 >= ins) {
            xc::Grid::SetRowSpan(c, span + 1);
        }
    }
    xc::RowDefinition rd;
    rd.Height(GridLengthHelper::Auto());
    grid.RowDefinitions().InsertAt(ins, rd);

    xc::Grid::SetRow(host, ins);
    xc::Grid::SetColumn(host, 0);
    xc::Grid::SetColumnSpan(
        host, std::max<int>(1, grid.ColumnDefinitions().Size()));
    grid.Children().Append(host);

    g_attach.target = pinned;
    g_attach.parent = grid.as<xc::Panel>();
    g_attach.host = host;
    g_attach.rowGrid = grid;
    g_attach.insertedRow = ins;
    g_attach.placement = requestedPlacement;
    if (auto tb = header.try_as<xc::TextBlock>()) {
        g_attach.pinnedHeader = tb;
    }
    g_attach.themeToken = pinned.ActualThemeChanged(
        [](auto&&, auto&&) { RebuildHostContent(); });
    WatchUnloaded(grid);
    WatchUnloaded(pinned);

    if (g_model) {
        RebuildHostContent();
    } else {
        ReloadAsync();
    }
    Wh_Log(L"Inserted groups as row %d of %s (%u rows)", ins,
           grid.Name().c_str(), grid.RowDefinitions().Size());
    return true;
}

// The header grid above `el` ("TopLevelHeader"), and its child that
// contains `el`.
xc::Grid FindHeaderGrid(FrameworkElement el, FrameworkElement* branchOut) {
    FrameworkElement branch = el;
    for (int depth = 0; branch && depth < 10; depth++) {
        auto p = xm::VisualTreeHelper::GetParent(branch);
        if (!p) {
            break;
        }
        if (auto g = p.try_as<xc::Grid>();
            g && (g.Name() == L"TopLevelHeader" ||
                  g.RowDefinitions().Size() >= 3)) {
            if (branchOut) {
                *branchOut = branch;
            }
            return g;
        }
        branch = p.try_as<FrameworkElement>();
    }
    return nullptr;
}

// The first TextBlock with text in `root` (depth-limited).
xc::TextBlock FindTextBlock(DependencyObject const& root, int depth) {
    if (auto tb = root.try_as<xc::TextBlock>(); tb && !tb.Text().empty()) {
        return tb;
    }
    if (depth <= 0) {
        return nullptr;
    }
    int n = xm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < n; i++) {
        if (auto tb =
                FindTextBlock(xm::VisualTreeHelper::GetChild(root, i), depth - 1)) {
            return tb;
        }
    }
    return nullptr;
}

// The Pinned header from the pinned list, without its text (so in any
// display language): the text in the header grid's row just above the list.
xc::TextBlock PinnedHeaderAbove(FrameworkElement pinnedList) {
    FrameworkElement branch{nullptr};
    auto grid = FindHeaderGrid(pinnedList, &branch);
    if (!grid || !branch) {
        return nullptr;
    }
    int listRow = xc::Grid::GetRow(branch);
    UIElement best{nullptr};
    int bestRow = -1;
    for (auto c : grid.Children()) {
        auto fe = c.try_as<FrameworkElement>();
        if (!fe || fe.Name() == L"WindhawkStartMenuGroups") {
            continue;
        }
        int r = xc::Grid::GetRow(fe);
        if (r < listRow && r > bestRow) {
            bestRow = r;
            best = c;
        }
    }
    return best ? FindTextBlock(best, 10) : nullptr;
}

// Looks through Start's tree for the Pinned header: its "Pinned" text, or
// else by the names of Start's elements, which aren't translated: the header
// row ("PinnedListHeaderGrid"), or the pinned list ("StartMenuPinnedList")
// and the header above it. Our own groups are skipped (a group may be named
// "Pinned").
xc::TextBlock FindPinnedHeader(DependencyObject const& root) {
    xc::TextBlock byText{nullptr};
    FrameworkElement headerGrid{nullptr};
    FrameworkElement pinnedList{nullptr};
    std::vector<DependencyObject> stack{root};
    for (int visited = 0; !stack.empty() && visited < 50000; visited++) {
        auto node = stack.back();
        stack.pop_back();
        if (auto fe = node.try_as<FrameworkElement>()) {
            auto name = fe.Name();
            if (name == L"WindhawkStartMenuGroups") {
                continue;
            }
            if (!headerGrid && name == L"PinnedListHeaderGrid") {
                headerGrid = fe;
            }
            if (!pinnedList &&
                (name == L"StartMenuPinnedList" || name == L"PinnedList")) {
                pinnedList = fe;
            }
        }
        if (auto tb = node.try_as<xc::TextBlock>()) {
            if (!byText && IsPinnedHeaderText(tb.Text())) {
                byText = tb;
            }
            continue;
        }
        int n = xm::VisualTreeHelper::GetChildrenCount(node);
        for (int i = n - 1; i >= 0; i--) {
            stack.push_back(xm::VisualTreeHelper::GetChild(node, i));
        }
    }
    // The first one that's in Start's header grid.
    xc::TextBlock candidates[] = {
        byText, headerGrid ? FindTextBlock(headerGrid, 6) : nullptr,
        pinnedList ? PinnedHeaderAbove(pinnedList) : nullptr};
    for (auto const& tb : candidates) {
        if (tb && FindHeaderGrid(tb, nullptr)) {
            return tb;
        }
    }
    return nullptr;
}

// --- "Customize Start Groups" in Start's own right-click menu ----------
// Start's menu (with "Start settings") is a MenuFlyout. When it's open (see
// CheckOpenMenus), our item is inserted after "Start settings". The menu is
// reused, so the item is added once; it's removed when the mod unloads.

constexpr PCWSTR kMenuItemTag = L"WindhawkStartMenuGroups";

struct AddedMenuItem {
    wf::Collections::IVector<xc::MenuFlyoutItemBase> items{nullptr};
    wf::Collections::IVector<wf::IInspectable> objects{nullptr};
    xc::MenuFlyoutItem item{nullptr};
};
[[clang::no_destroy]] std::vector<AddedMenuItem> g_addedMenuItems;  // UI thread

bool IsOurMenuItem(wf::IInspectable const& obj) {
    auto mi = obj.try_as<xc::MenuFlyoutItem>();
    return mi && winrt::unbox_value_or<winrt::hstring>(mi.Tag(), L"") ==
                     kMenuItemTag;
}

bool IsStartSettingsItem(wf::IInspectable const& obj) {
    auto mi = obj.try_as<xc::MenuFlyoutItem>();
    if (!mi) {
        return false;
    }
    std::lock_guard lock(g_settingsMutex);
    return TextInList(mi.Text(), g_settings.startSettingsTexts);
}

xc::MenuFlyoutItem MakeCustomizeMenuItem() {
    xc::MenuFlyoutItem item;
    item.Text(L"Customize Start Groups");
    item.Tag(winrt::box_value(winrt::hstring(kMenuItemTag)));
    xc::FontIcon icon;
    icon.Glyph(L"\uF0E2");  // GridView, like the settings window's title
    item.Icon(icon);
    item.Click([](auto&&, auto&&) { RequestSettingsWindow(); });
    return item;
}

// Inserts our item after "Start settings", if the menu has that item.
template <typename Vector>
bool InsertCustomizeItem(Vector const& items, AddedMenuItem& added) {
    int at = -1;
    for (uint32_t i = 0; i < items.Size(); i++) {
        auto obj = items.GetAt(i);
        if (IsOurMenuItem(obj)) {
            return false;  // added the last time the menu was shown
        }
        if (at < 0 && IsStartSettingsItem(obj)) {
            at = (int)i;
        }
    }
    if (at < 0) {
        return false;
    }
    added.item = MakeCustomizeMenuItem();
    items.InsertAt(at + 1, added.item);
    return true;
}

void AddToStartMenu(xc::MenuFlyoutPresenter const& presenter) {
    AddedMenuItem added;
    // A MenuFlyout shows its Items through the presenter's ItemsSource.
    if (auto items = presenter.ItemsSource()
                         .try_as<wf::Collections::IVector<
                             xc::MenuFlyoutItemBase>>()) {
        if (InsertCustomizeItem(items, added)) {
            added.items = items;
        }
    } else if (!presenter.ItemsSource()) {
        auto objects = presenter.Items().as<
            wf::Collections::IVector<wf::IInspectable>>();
        if (InsertCustomizeItem(objects, added)) {
            added.objects = objects;
        }
    }
    if (added.item) {
        g_addedMenuItems.push_back(std::move(added));
        Wh_Log(L"Added Customize Start Groups to Start's menu");
    }
}

void RemoveFromStartMenus() {
    for (auto& added : g_addedMenuItems) {
        try {
            uint32_t i;
            if (added.items && added.items.IndexOf(added.item, i)) {
                added.items.RemoveAt(i);
            } else if (added.objects && added.objects.IndexOf(added.item, i)) {
                added.objects.RemoveAt(i);
            }
        } catch (...) {
        }
    }
    g_addedMenuItems.clear();
}

// ---------------------------------------------------------------------------
// Start's UI thread
// ---------------------------------------------------------------------------
// Start's UI is a CoreWindow ("Windows.UI.Core.CoreWindow") with system XAML.
// The mod gets on its thread when that window is created (the
// CreateWindowInBand hooks) or, if Start is already running, through a
// thread hook (RunFromWindowThread), and creates a message-only window there
// (InstallStartUi). Its timer looks through Start's tree for the Pinned header
// until the groups are attached, and checks for Start's right-click menu
// while Start is open. (No XAML diagnostics: a process allows one
// diagnostics client, which other mods and tools need.)

constexpr UINT_PTR kScanTimerId = 1;
constexpr UINT_PTR kMenuTimerId = 2;
constexpr UINT kScanIntervalMs = 200;

// UI thread only:
bool g_startWasOpen = false;
DWORD g_lastScanTick = 0;
int g_failedScans = 0;
int g_menuChecks = 0;

// Right-click handlers on Start's root element, to look for its menu as soon
// as it opens.
struct RootHandlers {
    winrt::weak_ref<UIElement> root;
    wf::IInspectable rightTapped{nullptr};
    wf::IInspectable contextRequested{nullptr};
};
[[clang::no_destroy]] RootHandlers g_rootHandlers;

void RequestScan() {
    g_lastScanTick = 0;
    g_failedScans = 0;
}

std::vector<xcp::Popup> OpenPopups() {
    std::vector<xcp::Popup> out;
    auto window = Window::Current();
    if (!window) {
        return out;
    }
    for (auto p : xm::VisualTreeHelper::GetOpenPopups(window)) {
        out.push_back(p);
    }
    if (auto content = window.Content()) {
        try {
            if (auto root = content.XamlRoot()) {
                for (auto p :
                     xm::VisualTreeHelper::GetOpenPopupsForXamlRoot(root)) {
                    if (std::find(out.begin(), out.end(), p) == out.end()) {
                        out.push_back(p);
                    }
                }
            }
        } catch (...) {
        }
    }
    return out;
}

xc::MenuFlyoutPresenter FindMenuPresenter(DependencyObject const& el,
                                          int depth) {
    if (!el) {
        return nullptr;
    }
    if (auto p = el.try_as<xc::MenuFlyoutPresenter>()) {
        return p;
    }
    if (depth <= 0) {
        return nullptr;
    }
    int n = xm::VisualTreeHelper::GetChildrenCount(el);
    for (int i = 0; i < n; i++) {
        if (auto p = FindMenuPresenter(xm::VisualTreeHelper::GetChild(el, i),
                                       depth - 1)) {
            return p;
        }
    }
    return nullptr;
}

// Adds our item to Start's right-click menu if it's open.
void CheckOpenMenus() {
    for (auto const& popup : OpenPopups()) {
        if (!popup.IsOpen()) {
            continue;
        }
        if (auto presenter = FindMenuPresenter(popup.Child(), 3)) {
            AddToStartMenu(presenter);
        }
    }
}

void OnRootRightClick() {
    // The menu opens right after this event: look a few times, quickly.
    g_menuChecks = 10;
    if (HWND wnd = g_uiWnd) {
        SetTimer(wnd, kMenuTimerId, 30, nullptr);
    }
}

void RemoveRootHandlers() {
    if (auto root = g_rootHandlers.root.get()) {
        try {
            if (g_rootHandlers.rightTapped) {
                root.RemoveHandler(UIElement::RightTappedEvent(),
                                   g_rootHandlers.rightTapped);
            }
            if (g_rootHandlers.contextRequested) {
                root.RemoveHandler(UIElement::ContextRequestedEvent(),
                                   g_rootHandlers.contextRequested);
            }
        } catch (...) {
        }
    }
    g_rootHandlers = RootHandlers{};
}

void WatchRootForMenus(UIElement const& root) {
    if (!root || g_rootHandlers.root.get() == root) {
        return;
    }
    RemoveRootHandlers();
    g_rootHandlers.root = root;
    g_rootHandlers.rightTapped = winrt::box_value(
        winrt::Windows::UI::Xaml::Input::RightTappedEventHandler(
            [](auto&&, auto&&) { OnRootRightClick(); }));
    g_rootHandlers.contextRequested = winrt::box_value(
        wf::TypedEventHandler<
            UIElement,
            winrt::Windows::UI::Xaml::Input::ContextRequestedEventArgs>(
            [](auto&&, auto&&) { OnRootRightClick(); }));
    // Also when Start has handled the event itself (it opens its menu).
    root.AddHandler(UIElement::RightTappedEvent(), g_rootHandlers.rightTapped,
                    true);
    root.AddHandler(UIElement::ContextRequestedEvent(),
                    g_rootHandlers.contextRequested, true);
}

// Looks for the Pinned header and attaches the groups there.
void ScanStartTree() {
    auto window = Window::Current();
    auto root = window ? window.Content() : nullptr;
    if (!root) {
        return;
    }
    WatchRootForMenus(root);
    auto header = FindPinnedHeader(root);
    if (!header || !header.IsLoaded()) {
        g_failedScans++;
        return;
    }
    if (AttachInHeaderGrid(header)) {
        g_failedScans = 0;
        return;
    }
    // Start may still be building its sections: tried again later.
    if (g_failedScans++ == 0) {
        LogAncestors(header);
    }
}

void OnScanTimer() {
    if (g_unloading) {
        return;
    }
    try {
        bool open = IsStartOpen(g_coreWnd);
        if (open) {
            CheckOpenMenus();
            if (!g_startWasOpen && g_attach.host) {
                ValidateAttachment();  // each time Start opens
            }
        }
        g_startWasOpen = open;
        if (!g_attach.host) {
            // Every tick while Start is open, else once a second; slower
            // after many tries (e.g. a Start layout the mod doesn't know).
            DWORD now = GetTickCount();
            DWORD every = g_failedScans > 50 ? (open ? 2000 : 10000)
                                             : (open ? 0 : 1000);
            if (g_lastScanTick == 0 || now - g_lastScanTick >= every) {
                g_lastScanTick = now ? now : 1;
                ScanStartTree();
            }
        }
    } catch (...) {
        Wh_Log(L"Scan failed: %08X", winrt::to_hresult());
    }
}

PCWSTR UiWindowClassName() {
    static std::wstring name = std::wstring(L"WindhawkStartUi_") + WH_MOD_ID;
    return name.c_str();
}

void UninstallStartUi();

LRESULT CALLBACK UiWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == kRunOnUiMsg) {
        std::unique_ptr<std::function<void()>> fn(
            reinterpret_cast<std::function<void()>*>(lp));
        if (!g_unloading) {
            try {
                (*fn)();
            } catch (...) {
                Wh_Log(L"UI task failed: %08X", winrt::to_hresult());
            }
        }
        return 0;
    }
    if (msg == kUninstallMsg) {
        UninstallStartUi();
        return 0;
    }
    if (msg == WM_TIMER && wp == kScanTimerId) {
        OnScanTimer();
        return 0;
    }
    if (msg == WM_TIMER && wp == kMenuTimerId) {
        if (--g_menuChecks <= 0) {
            KillTimer(hWnd, kMenuTimerId);
        }
        try {
            if (!g_unloading) {
                CheckOpenMenus();
            }
        } catch (...) {
        }
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wp, lp);
}

HMODULE ThisModule() {
    HMODULE self = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&ThisModule, &self);
    return self;
}

// On Start's UI thread (`coreWnd`'s): the message window and its timer.
void InstallStartUi(HWND coreWnd) {
    if (g_uiWnd || g_unloading) {
        return;
    }
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = UiWndProc;
    wc.hInstance = ThisModule();
    wc.lpszClassName = UiWindowClassName();
    RegisterClassExW(&wc);
    HWND wnd = CreateWindowExW(0, UiWindowClassName(), L"", 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    if (!wnd) {
        Wh_Log(L"Creating the UI window failed: %u", GetLastError());
        return;
    }
    g_coreWnd = coreWnd;
    g_uiWnd = wnd;
    RequestScan();
    SetTimer(wnd, kScanTimerId, kScanIntervalMs, nullptr);
    Wh_Log(L"Watching Start's UI");
}

// On Start's UI thread: removes everything the mod added to Start, and the
// message window, dropping whatever is still queued for it.
void UninstallStartUi() {
    HWND wnd = g_uiWnd.exchange(nullptr);
    if (!wnd) {
        return;
    }
    KillTimer(wnd, kScanTimerId);
    KillTimer(wnd, kMenuTimerId);
    // Shown again if it was hidden after a launch (CloseStartAfterLaunch).
    HideStartContent(false);
    try {
        for (auto& weak : g_flyouts) {
            if (auto fly = weak.get()) {
                fly.Hide();
            }
        }
    } catch (...) {
    }
    g_flyouts.clear();
    try {
        RemoveRootHandlers();
        RemoveFromStartMenus();
        Detach();
    } catch (...) {
        Wh_Log(L"Cleanup failed: %08X", winrt::to_hresult());
    }
    g_model = nullptr;
    g_imageSources.clear();
    MSG msg;
    while (PeekMessageW(&msg, wnd, kRunOnUiMsg, kRunOnUiMsg, PM_REMOVE)) {
        delete reinterpret_cast<std::function<void()>*>(msg.lParam);
    }
    DestroyWindow(wnd);
    UnregisterClassW(UiWindowClassName(), ThisModule());
    g_coreWnd = nullptr;
}

// Runs `proc` on `hWnd`'s thread and waits for it.
using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

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
    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }
    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}

void OnWindowCreated(HWND hWnd, LPCWSTR className) {
    bool textual = ((ULONG_PTR)className & ~(ULONG_PTR)0xFFFF) != 0;
    if (g_unloading || !textual ||
        _wcsicmp(className, L"Windows.UI.Core.CoreWindow") != 0) {
        return;
    }
    // Created on its own thread, so this runs right here.
    RunFromWindowThread(
        hWnd, [](PVOID p) { InstallStartUi((HWND)p); }, hWnd);
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;

HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName);
    }
    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
                                             LPCWSTR lpClassName,
                                             LPCWSTR lpWindowName,
                                             DWORD dwStyle,
                                             int X,
                                             int Y,
                                             int nWidth,
                                             int nHeight,
                                             HWND hWndParent,
                                             HMENU hMenu,
                                             HINSTANCE hInstance,
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;

HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
                                      LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName,
                                      DWORD dwStyle,
                                      int X,
                                      int Y,
                                      int nWidth,
                                      int nHeight,
                                      HWND hWndParent,
                                      HMENU hMenu,
                                      HINSTANCE hInstance,
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName);
    }
    return hWnd;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

std::wstring GetString(PCWSTR name, int i = -1, int j = -1) {
    std::wstring s =
        WindhawkUtils::StringSetting(Wh_GetStringSetting(name, i, j)).get();
    // Trim surrounding whitespace.
    size_t b = s.find_first_not_of(L" \t");
    size_t e = s.find_last_not_of(L" \t");
    return b == std::wstring::npos ? L"" : s.substr(b, e - b + 1);
}

double GetNumber(PCWSTR name, double lo, double hi) {
    return Clamp(Wh_GetIntSetting(name), lo, hi);
}

void LoadSettings() {
    ModSettings s;
    auto& L = s.layout;
    // Tiles
    L.tileSize = GetNumber(L"tiles.size", 60, 600);
    L.tileSpacingX = GetNumber(L"tiles.gapX", 0, 200);
    L.tileSpacingY = GetNumber(L"tiles.gapY", 0, 200);
    L.marginLeft = GetNumber(L"tiles.marginLeft", -kMaxSpacing, kMaxSpacing);
    L.marginRight =
        GetNumber(L"tiles.marginRight", -kMaxSpacing, kMaxSpacing);
    L.tilePadding = GetNumber(L"tiles.padding", 0, 100);
    L.cornerRadius = GetNumber(L"tiles.cornerRadius", 0, 100);
    L.borderThickness = GetNumber(L"tiles.borderThickness", 0, 10);
    L.tileBackground = GetString(L"tiles.background");
    L.tileBorder = GetString(L"tiles.border");
    // Icons
    L.iconSize = GetNumber(L"icons.size", 12, 256);
    L.iconSpacing = GetNumber(L"icons.gap", 0, 100);
    L.appPaddingX = GetNumber(L"icons.paddingX", 0, 60);
    L.appPaddingY = GetNumber(L"icons.paddingY", 0, 60);
    L.showOverflowCluster = Wh_GetIntSetting(L"icons.overflowCluster") != 0;
    // Group names
    L.showLabels = Wh_GetIntSetting(L"labels.show") != 0;
    L.labelFontSize = GetNumber(L"labels.fontSize", 8, 40);
    L.labelSpacing = GetNumber(L"labels.spacing", 0, 100);
    L.labelColor = GetString(L"labels.color");
    L.sectionTitle = GetString(L"behavior.sectionTitle");
    L.showAppLabels = Wh_GetIntSetting(L"behavior.showAppLabels") != 0;
    // Behavior
    L.hideEmptyGroups =
        Wh_GetIntSetting(L"behavior.hideEmptyGroups") != 0;
    L.showAppTooltips = Wh_GetIntSetting(L"behavior.hideAppTooltips") == 0;
    L.sortAppsByName = Wh_GetIntSetting(L"behavior.sortAppsByName") != 0;
    L.flyoutColumns = (int)GetNumber(L"behavior.expandedColumns",
                                     kMinExpandedColumns, kMaxExpandedColumns);
    s.placement = GetString(L"behavior.placement");
    if (s.placement != L"afterPinned" && s.placement != L"afterRecent" &&
        s.placement != L"afterAll") {
        s.placement = L"beforePinned";  // the default
    }
    // "hide" settings, so a missing value (0) keeps Start as it is.
    s.showPinned = Wh_GetIntSetting(L"behavior.hidePinned") == 0;
    s.showRecommended = Wh_GetIntSetting(L"behavior.hideRecommended") == 0;
    s.showAllApps = Wh_GetIntSetting(L"behavior.hideAllApps") == 0;

    for (int i = 0;; i++) {
        std::wstring name = GetString(L"groups[%d].name", i);
        if (name.empty()) {
            break;
        }
        GroupSetting cat{name, {}, {}};
        {
            std::wstring grid = GetString(L"groups[%d].tileSize", i);
            cat.layout = MakeLayout(
                ResolveTileSize(GetString(L"groups[%d].span", i), grid), grid);
        }
        // Stored inverted: groups saved before they existed show them.
        cat.showAppNames =
            Wh_GetIntSetting(L"groups[%d].hideAppNames", i) == 0;
        cat.showLabel = Wh_GetIntSetting(L"groups[%d].hideLabel", i) == 0;
        cat.showBackground =
            Wh_GetIntSetting(L"groups[%d].hideBackground", i) == 0;
        cat.items = ReadItems(
            [](std::wstring const& key) { return GetString(key.c_str()); },
            L"groups[" + std::to_wstring(i) + L"].items");
        s.groups.push_back(std::move(cat));
    }

    // Start's section titles and menu item, as Start shows them in English.
    // (The groups are placed without them too, from the pinned list; see
    // FindPinnedHeader.)
    s.pinnedHeaders = {L"Pinned"};
    s.recommendedHeaders = {L"Recommended", L"Recent"};
    s.allHeaders = {L"All"};
    s.startSettingsTexts = {L"Start settings"};

    // The order arranged in Start, on top. Each group and item remembers
    // where it's saved, which is what a drag in Start moves.
    for (int g = 0; g < (int)s.groups.size(); g++) {
        s.groups[g].raw = g;
        for (int i = 0; i < (int)s.groups[g].items.size(); i++) {
            auto& item = s.groups[g].items[i];
            item.rawGroup = g;
            item.rawItem = i;
            for (int c = 0; c < (int)item.children.size(); c++) {
                item.children[c].rawGroup = g;
                item.children[c].rawItem = i;
                item.children[c].rawSub = c;
            }
        }
    }
    s.rawGroupKeys = OrderGroupKeys(s.groups);
    s.rawItemKeys = OrderItemKeys(s.groups);
    s.rawChildKeys = OrderChildKeys(s.groups);
    s.groups =
        ApplyStartOrder(s.groups, PlanStartOrder(s.groups, ReadStartOrderValue()));

    std::lock_guard lock(g_settingsMutex);
    s.generation = g_settings.generation + 1;
    g_settings = std::move(s);
}

// ===========================================================================
// Settings window
//
// Same design as the Explorer Command Bar mod: a Win32 window hosting a XAML
// island, with a navigation pane, card-style settings pages and a
// Save / Discard footer. It edits this mod's Windhawk settings directly
// (HKLM\SOFTWARE\Windhawk\Engine\Mods\<id>\Settings); since that key is
// protected, saving goes through an elevated import (one UAC prompt, see
// WriteModSettings). Windhawk then calls Wh_ModSettingsChanged, so Start
// updates right away.
//
// The window is a Windows.UI.Xaml island on the mod's own thread in File
// Explorer (Start's process can't host one), see StartSettingsHost.
// ===========================================================================

using EditItem = ItemSetting;

struct EditGroup {
    std::wstring name;
    std::wstring tileSize = L"2x2";  // spots, one of kTileSizes
    std::wstring appGrid = kAutoGrid;  // apps, "auto" or within tileSize
    bool showAppNames = true;
    bool showLabel = true;       // the name under the tile
    bool showBackground = true;  // off: no background, border or padding
    std::vector<EditItem> items;
};

// An app without a group: saved as a one-spot (1 x 1) tile with just that
// app, and shown as the app alone (no name, no background). In the window
// it's an app, not a group: no group settings, 1 x 1 isn't offered as a
// group's size, and removing its app removes it. "Add to group" makes it a
// group, "Remove from group" (a group's only app) makes one again.
bool IsSingleApp(EditGroup const& g) {
    return g.tileSize == L"1x1";
}

std::wstring NewGroupName(std::vector<EditGroup> const& groups) {
    std::wstring name = L"New group";
    for (int n = 2; std::any_of(groups.begin(), groups.end(),
                                [&](auto& c) { return c.name == name; });
         n++) {
        name = L"New group " + std::to_wstring(n);
    }
    return name;
}

std::wstring ItemTitle(EditItem const& item);

// An app on its own (see IsSingleApp).
EditGroup MakeSingleApp(EditItem item) {
    EditGroup g;
    g.name = ItemTitle(item);
    g.tileSize = L"1x1";
    g.appGrid = kAutoGrid;
    g.showLabel = false;
    g.showBackground = false;
    g.items.push_back(std::move(item));
    return g;
}

// A group made in Start (see the GroupSetting overload): an app on its own.
EditGroup MakeStartGroup(std::wstring const& name, int, EditGroup*) {
    EditGroup g;
    g.name = name;
    g.tileSize = L"1x1";
    g.appGrid = kAutoGrid;
    g.showLabel = false;
    g.showBackground = false;
    return g;
}

// An app on its own made a group (2 x 2, with its name and background).
void MakeGroupOf(EditGroup& g, std::wstring const& name) {
    g.name = name;
    g.tileSize = L"2x2";
    g.appGrid = kAutoGrid;
    g.showLabel = true;
    g.showBackground = true;
}

// A new tile size; made bigger than a single app, the group gets its name
// and background shown (as a single app it had neither).
void SetTileSize(EditGroup& g, std::wstring const& size) {
    bool wasSingle = IsSingleApp(g);
    g.tileSize = size;
    g.appGrid = FitGrid(g.appGrid, size);
    if (wasSingle && !IsSingleApp(g)) {
        g.showLabel = true;
        g.showBackground = true;
    }
}
bool ShowsLabel(EditGroup const& g) {
    return g.showLabel && !IsSingleApp(g);
}
bool ShowsBackground(EditGroup const& g) {
    return g.showBackground && !IsSingleApp(g);
}

GroupLayout LayoutOf(EditGroup const& g) {
    return MakeLayout(g.tileSize, g.appGrid);
}

struct EditSettings {
    std::vector<EditGroup> groups;
    // Behavior
    std::wstring placement = L"beforePinned";
    bool hideEmpty = true;
    bool showTooltips = true;
    bool sortByName = false;
    int expandedColumns = 4;
    bool hidePinned = false, hideRecommended = false, hideAllApps = false;
    // Tiles
    int tileSize = 154, gapX = 25, gapY = 20, padding = 8;
    int marginLeft = 0, marginRight = 0;  // from Start's own (HostMargin)
    int cornerRadius = 8, borderThickness = 1;
    std::wstring background, border;
    // Icons
    int iconSize = 32, iconGap = 5, appPadX = 4, appPadY = 4;
    bool overflowCluster = true;
    // Labels
    bool labelsShow = true;
    int fontSize = 12, labelSpacing = 12;
    std::wstring labelColor;
    std::wstring sectionTitle = L"Apps";
    bool showAppLabels = true;
};

std::vector<std::wstring> SplitLines(const std::wstring& text) {
    std::vector<std::wstring> lines;
    std::wstring cur;
    auto flush = [&] {
        size_t b = cur.find_first_not_of(L" \t");
        size_t e = cur.find_last_not_of(L" \t");
        if (b != std::wstring::npos) {
            lines.push_back(cur.substr(b, e - b + 1));
        }
        cur.clear();
    };
    for (WCHAR c : text) {
        if (c == L'\r' || c == L'\n') {
            flush();
        } else {
            cur += c;
        }
    }
    flush();
    return lines;
}

// The settings, with the order arranged in Start (if any) applied. `saved`
// gets the groups in their saved order.
EditSettings ReadEditSettings(std::vector<EditGroup>* saved = nullptr) {
    EditSettings s;
    for (int i = 0;; i++) {
        std::wstring name = GetString(L"groups[%d].name", i);
        if (name.empty()) {
            break;
        }
        EditGroup cat;
        cat.name = name;
        cat.appGrid = GetString(L"groups[%d].tileSize", i);
        cat.tileSize = ResolveTileSize(GetString(L"groups[%d].span", i),
                                       cat.appGrid);
        cat.appGrid = FitGrid(cat.appGrid, cat.tileSize);
        cat.showAppNames =
            Wh_GetIntSetting(L"groups[%d].hideAppNames", i) == 0;
        cat.showLabel = Wh_GetIntSetting(L"groups[%d].hideLabel", i) == 0;
        cat.showBackground =
            Wh_GetIntSetting(L"groups[%d].hideBackground", i) == 0;
        cat.items = ReadItems(
            [](std::wstring const& key) { return GetString(key.c_str()); },
            L"groups[" + std::to_wstring(i) + L"].items");
        s.groups.push_back(std::move(cat));
    }
    if (saved) {
        *saved = s.groups;
    }
    s.groups =
        ApplyStartOrder(s.groups, PlanStartOrder(s.groups, ReadStartOrderValue()));
    s.placement = GetString(L"behavior.placement");
    if (s.placement != L"afterPinned" && s.placement != L"afterRecent" &&
        s.placement != L"afterAll") {
        // The default; also for the old "replaceCategoryView".
        s.placement = L"beforePinned";
    }
    s.hideEmpty = Wh_GetIntSetting(L"behavior.hideEmptyGroups") != 0;
    s.showTooltips = Wh_GetIntSetting(L"behavior.hideAppTooltips") == 0;
    s.sortByName = Wh_GetIntSetting(L"behavior.sortAppsByName") != 0;
    s.expandedColumns =
        std::clamp(Wh_GetIntSetting(L"behavior.expandedColumns"),
                   kMinExpandedColumns, kMaxExpandedColumns);
    s.hidePinned = Wh_GetIntSetting(L"behavior.hidePinned") != 0;
    s.hideRecommended = Wh_GetIntSetting(L"behavior.hideRecommended") != 0;
    s.hideAllApps = Wh_GetIntSetting(L"behavior.hideAllApps") != 0;
    s.tileSize = Wh_GetIntSetting(L"tiles.size");
    s.gapX = Wh_GetIntSetting(L"tiles.gapX");
    s.gapY = Wh_GetIntSetting(L"tiles.gapY");
    s.marginLeft = std::clamp(Wh_GetIntSetting(L"tiles.marginLeft"),
                              -kMaxSpacing, kMaxSpacing);
    s.marginRight = std::clamp(Wh_GetIntSetting(L"tiles.marginRight"),
                               -kMaxSpacing, kMaxSpacing);
    s.padding = Wh_GetIntSetting(L"tiles.padding");
    s.cornerRadius = Wh_GetIntSetting(L"tiles.cornerRadius");
    s.borderThickness = Wh_GetIntSetting(L"tiles.borderThickness");
    s.background = GetString(L"tiles.background");
    s.border = GetString(L"tiles.border");
    s.iconSize = Wh_GetIntSetting(L"icons.size");
    s.iconGap = Wh_GetIntSetting(L"icons.gap");
    s.appPadX = Wh_GetIntSetting(L"icons.paddingX");
    s.appPadY = Wh_GetIntSetting(L"icons.paddingY");
    s.overflowCluster = Wh_GetIntSetting(L"icons.overflowCluster") != 0;
    s.labelsShow = Wh_GetIntSetting(L"labels.show") != 0;
    s.fontSize = Wh_GetIntSetting(L"labels.fontSize");
    s.labelSpacing = Wh_GetIntSetting(L"labels.spacing");
    s.labelColor = GetString(L"labels.color");
    s.sectionTitle = GetString(L"behavior.sectionTitle");
    s.showAppLabels = Wh_GetIntSetting(L"behavior.showAppLabels") != 0;
    return s;
}

// --- Field descriptions (drive both the pages and the registry export) ----

// Preset: the tile size presets, not a setting of its own (value points to
// the EditSettings; the preset shown is the one matching the values).
enum class FieldKind { Bool, Int, String, Choice, Preset };

struct Field {
    PCWSTR key;
    PCWSTR glyph;
    PCWSTR title;
    PCWSTR description;
    FieldKind kind;
    void* value;
    int min = 0, max = 0;
    std::vector<std::pair<std::wstring, std::wstring>> options;  // value, text
    bool inverted = false;  // Bool: saved as the opposite ("hide..." keys)
    // Not shown in the window: kept (saved, imported, set by the size
    // presets) at its value, but no longer offered to change.
    bool hidden = false;
};

Field Hidden(Field f) {
    f.hidden = true;
    return f;
}

struct SettingsPage {
    PCWSTR tag;
    PCWSTR glyph;
    PCWSTR title;
    PCWSTR description;
    std::vector<Field> fields;
};

Field BoolF(PCWSTR key, PCWSTR glyph, PCWSTR title, PCWSTR desc, bool* v) {
    return {key, glyph, title, desc, FieldKind::Bool, v};
}
// A switch shown as "Show ..." for a setting saved as "hide...".
Field ShowF(PCWSTR key, PCWSTR glyph, PCWSTR title, PCWSTR desc, bool* v) {
    Field f{key, glyph, title, desc, FieldKind::Bool, v};
    f.inverted = true;
    return f;
}
Field IntF(PCWSTR key, PCWSTR glyph, PCWSTR title, PCWSTR desc, int* v,
           int min, int max) {
    return {key, glyph, title, desc, FieldKind::Int, v, min, max};
}
Field StrF(PCWSTR key, PCWSTR glyph, PCWSTR title, PCWSTR desc,
           std::wstring* v) {
    return {key, glyph, title, desc, FieldKind::String, v};
}
Field ChoiceF(PCWSTR key, PCWSTR glyph, PCWSTR title, PCWSTR desc,
              std::wstring* v,
              std::vector<std::pair<std::wstring, std::wstring>> options) {
    Field f{key, glyph, title, desc, FieldKind::Choice, v};
    f.options = std::move(options);
    return f;
}
// Dimension presets offered at the top of the Customization page.
struct SizePreset {
    PCWSTR name;
    int tileSize, gapX, gapY, padding, cornerRadius;
    int iconSize, iconGap, appPadX, appPadY, fontSize, labelSpacing;
};

// Large is everything of Default doubled; Small about 3/4 of it. (App
// names and the opened group follow the icon size, see AppNameFontSize.)
constexpr SizePreset kSizePresets[] = {
    {L"Default", 154, 25, 20, 8, 8, 32, 5, 4, 4, 12, 12},
    {L"Small", 120, 20, 16, 6, 8, 24, 4, 4, 4, 11, 8},
    {L"Large", 308, 60, 40, 16, 16, 64, 10, 8, 8, 24, 24},
};

bool MatchesPreset(EditSettings const& s, SizePreset const& p) {
    return s.tileSize == p.tileSize && s.gapX == p.gapX && s.gapY == p.gapY &&
           s.padding == p.padding && s.cornerRadius == p.cornerRadius &&
           s.iconSize == p.iconSize && s.iconGap == p.iconGap &&
           s.appPadX == p.appPadX && s.appPadY == p.appPadY &&
           s.fontSize == p.fontSize && s.labelSpacing == p.labelSpacing;
}

void ApplyPreset(EditSettings& s, SizePreset const& p) {
    s.tileSize = p.tileSize;
    s.gapX = p.gapX;
    s.gapY = p.gapY;
    s.padding = p.padding;
    s.cornerRadius = p.cornerRadius;
    s.iconSize = p.iconSize;
    s.iconGap = p.iconGap;
    s.appPadX = p.appPadX;
    s.appPadY = p.appPadY;
    s.fontSize = p.fontSize;
    s.labelSpacing = p.labelSpacing;
}

// The preset the settings are: the one they match, else (a gap or name
// size changed since) the one with the nearest tile size.
int CurrentPreset(EditSettings const& s) {
    int best = 0;
    for (int i = 0; i < (int)ARRAYSIZE(kSizePresets); i++) {
        if (MatchesPreset(s, kSizePresets[i])) {
            return i;
        }
        if (std::abs(kSizePresets[i].tileSize - s.tileSize) <
            std::abs(kSizePresets[best].tileSize - s.tileSize)) {
            best = i;
        }
    }
    return best;
}

// "2 × 1": a tile size's cells, width × height.
std::wstring TileSizeText(std::wstring const& size) {
    auto l = MakeLayout(size, L"");
    return std::to_wstring(l.spanHalfColumns) + L" \u00D7 " +
           std::to_wstring(l.spanHalfRows);
}

// App grid choices for a tile size: value, "3 × 2 (6 apps)". Only grids
// that fit the tile.
std::vector<std::pair<std::wstring, std::wstring>> AppGridOptions(
    std::wstring const& size) {
    auto l = MakeLayout(size, kAutoGrid);
    auto text = [](int c, int r) {
        return std::to_wstring(c) + L" \u00D7 " + std::to_wstring(r) + L" (" +
               std::to_wstring(c * r) + (c * r == 1 ? L" app)" : L" apps)");
    };
    // First: the default, which follows the tile size.
    std::vector<std::pair<std::wstring, std::wstring>> out{
        {kAutoGrid, L"Default: " + text(l.iconColumns, l.iconRows)}};
    for (int c = 1; c <= std::min(l.spanHalfColumns, kMaxGridColumns); c++) {
        for (int r = 1; r <= std::min(l.spanHalfRows, kMaxGridRows); r++) {
            out.push_back({AxB(c, r), text(c, r)});
        }
    }
    return out;
}

// Tile size choices: value, "2 × 1".
std::vector<std::pair<std::wstring, std::wstring>> TileSizeOptions() {
    std::vector<std::pair<std::wstring, std::wstring>> out;
    for (PCWSTR t : kTileSizes) {
        if (std::wstring_view(t) != L"1x1") {  // an app without a group
            out.push_back({t, TileSizeText(t)});
        }
    }
    return out;
}

std::vector<SettingsPage> DescribePages(EditSettings& s) {
    constexpr PCWSTR kColorHint =
        L"#RRGGBB or #AARRGGBB. Empty = match Start.";
    std::vector<SettingsPage> pages;
    // Customization: the look of the groups. Sizes inside a tile (tile size,
    // padding, border, icons and the gaps between them) aren't offered on
    // their own any more; the size preset sets them, and they're kept as
    // they are otherwise (hidden fields).
    pages.push_back(
        {L"customization", L"\uE790", L"Customization",
         L"The look of the groups: their size, spacing, colors and names.",
         {
             Field{nullptr, L"\uE8A9", L"Size preset",
                   L"The size of the tiles, their icons and names, and the "
                   L"gaps between them, all at once. The gaps, corner radius "
                   L"and group names can still be changed below.",
                   FieldKind::Preset, &s},
             IntF(L"tiles.gapX", L"\uE89F", L"Horizontal gap", nullptr,
                  &s.gapX, 0, 120),
             IntF(L"tiles.gapY", L"\uE8A0", L"Vertical gap", nullptr, &s.gapY,
                  0, 120),
             IntF(L"tiles.marginLeft", L"\uE8A0", L"Left spacing",
                  L"Moves the left edge from Start's: positive in, negative "
                  L"out.",
                  &s.marginLeft, -kMaxSpacing, kMaxSpacing),
             IntF(L"tiles.marginRight", L"\uE89F", L"Right spacing",
                  L"Moves the right edge from Start's: positive in, negative "
                  L"out.",
                  &s.marginRight, -kMaxSpacing, kMaxSpacing),
             IntF(L"tiles.cornerRadius", L"\uE91F", L"Corner radius", nullptr,
                  &s.cornerRadius, 0, 40),
             StrF(L"tiles.background", L"\uE790", L"Background color",
                  kColorHint, &s.background),
             StrF(L"tiles.border", L"\uE790", L"Border color", kColorHint,
                  &s.border),
             BoolF(L"labels.show", L"\uE8D2", L"Show group names", nullptr,
                   &s.labelsShow),
             IntF(L"labels.fontSize", L"\uE8E9", L"Group name font size",
                  nullptr, &s.fontSize, 8, 32),
             IntF(L"labels.spacing", L"\uE8A0",
                  L"Space between tile and name", nullptr, &s.labelSpacing, 0,
                  60),
             StrF(L"labels.color", L"\uE790", L"Group name color", kColorHint,
                  &s.labelColor),
             Hidden(IntF(L"tiles.size", L"", L"Tile size", nullptr,
                         &s.tileSize, 60, 400)),
             Hidden(IntF(L"tiles.padding", L"", L"Padding inside a tile",
                         nullptr, &s.padding, 0, 60)),
             Hidden(IntF(L"tiles.borderThickness", L"", L"Border thickness",
                         nullptr, &s.borderThickness, 0, 6)),
             Hidden(IntF(L"icons.size", L"", L"Icon size", nullptr,
                         &s.iconSize, 16, 96)),
             Hidden(IntF(L"icons.gap", L"", L"Gap between apps", nullptr,
                         &s.iconGap, 0, 60)),
             Hidden(IntF(L"icons.paddingX", L"", L"App padding (horizontal)",
                         nullptr, &s.appPadX, 0, 40)),
             Hidden(IntF(L"icons.paddingY", L"", L"App padding (vertical)",
                         nullptr, &s.appPadY, 0, 40)),
             Hidden(BoolF(L"icons.overflowCluster", L"", L"Overflow cluster",
                          nullptr, &s.overflowCluster)),
         }});
    pages.push_back(
        {L"behavior", L"\uE713", L"Behavior",
         L"Where the groups appear and how apps are arranged.",
         {
             ChoiceF(L"behavior.placement", L"\uE8A1",
                     L"Where to show the groups",
                     L"Applied when the settings are saved", &s.placement,
                     {{L"beforePinned", L"Before the Pinned section (default)"},
                      {L"afterPinned", L"After the Pinned section"},
                      {L"afterRecent",
                       L"After the Recommended / Recent section"},
                      {L"afterAll", L"After the All section"}}),
             StrF(L"behavior.sectionTitle", L"\uE8D2", L"Section title",
                  L"Heading above the groups, styled like \"Pinned\". "
                  L"Empty = no heading.",
                  &s.sectionTitle),
             BoolF(L"behavior.showAppLabels", L"\uE8C1", L"Show app names",
                   L"Show each app's name under its icon inside the tiles",
                   &s.showAppLabels),
             BoolF(L"behavior.hideEmptyGroups", L"\uE7B3",
                   L"Hide empty groups", nullptr, &s.hideEmpty),
             ShowF(L"behavior.hideAppTooltips", L"\uE8BD",
                   L"Show app tooltips",
                   L"The app's full name when hovering over it in Start, "
                   L"for apps whose name is cut off (\"...\") or hidden",
                   &s.showTooltips),
             BoolF(L"behavior.sortAppsByName", L"\uE8CB",
                   L"Sort apps alphabetically",
                   L"Ignore the order the items are listed in",
                   &s.sortByName),
             IntF(L"behavior.expandedColumns", L"\uE8A9",
                  L"Columns when a group is opened", nullptr,
                  &s.expandedColumns, kMinExpandedColumns,
                  kMaxExpandedColumns),
         }});
    pages.push_back(
        {L"builtin", L"\uE7B3", L"Built-in sections",
         L"Start's own sections, around the groups.",
         {
             BoolF(L"behavior.hidePinned", L"\uE718",
                   L"Hide the Pinned section",
                   L"Start's own Pinned apps.",
                   &s.hidePinned),
             BoolF(L"behavior.hideRecommended", L"\uE823",
                   L"Hide the Recommended section",
                   L"Start's own Recommended / Recent section.",
                   &s.hideRecommended),
             BoolF(L"behavior.hideAllApps", L"\uE71D",
                   L"Hide the All section",
                   L"Start's own list of all apps (grid, list or category "
                   L"view).",
                   &s.hideAllApps),
         }});
    return pages;
}

// --- Registry export --------------------------------------------------------

struct RegEntry {
    std::wstring name;
    bool isString = false;
    std::wstring text;
    DWORD number = 0;
    bool isBool = false;  // only for the exported file
    bool operator==(RegEntry const&) const = default;
};

RegEntry StrEntry(std::wstring name, std::wstring const& value) {
    RegEntry e;
    e.name = std::move(name);
    e.isString = true;
    for (WCHAR c : value) {
        if (c != L'\r' && c != L'\n') {
            e.text += c;
        }
    }
    return e;
}

RegEntry NumEntry(std::wstring name, int value) {
    RegEntry e;
    e.name = std::move(name);
    e.number = (DWORD)value;
    return e;
}

std::vector<RegEntry> BuildRegEntries(EditSettings& s) {
    std::vector<RegEntry> out;
    int ci = 0;
    for (auto const& cat : s.groups) {
        if (cat.name.empty()) {
            continue;
        }
        std::wstring cp = L"groups[" + std::to_wstring(ci++) + L"]";
        out.push_back(StrEntry(cp + L".name", cat.name));
        out.push_back(StrEntry(cp + L".span", cat.tileSize));
        out.push_back(StrEntry(cp + L".tileSize", cat.appGrid));
        out.push_back(NumEntry(cp + L".hideAppNames", !cat.showAppNames));
        out.push_back(NumEntry(cp + L".hideLabel", !cat.showLabel));
        out.push_back(NumEntry(cp + L".hideBackground", !cat.showBackground));
        // Items without an app or path aren't saved (they'd end the list
        // when read back); folders are, with their apps.
        auto writeItems = [&](auto& self, std::wstring const& prefix,
                              std::vector<EditItem> const& items) -> void {
            int ii = 0;
            for (auto const& item : items) {
                bool folder = item.type == L"folder";
                if (item.app.empty() && item.path.empty() && !folder) {
                    continue;
                }
                std::wstring ip =
                    prefix + L"[" + std::to_wstring(ii++) + L"]";
                out.push_back(StrEntry(ip + L".type", item.type));
                out.push_back(StrEntry(ip + L".app", item.app));
                out.push_back(StrEntry(ip + L".label", item.label));
                out.push_back(StrEntry(ip + L".path", item.path));
                out.push_back(StrEntry(ip + L".args", item.args));
                out.push_back(StrEntry(ip + L".icon", item.icon));
                if (folder) {
                    self(self, ip + L".apps", item.children);
                }
            }
        };
        writeItems(writeItems, cp + L".items", cat.items);
    }
    for (auto& page : DescribePages(s)) {
        for (auto& f : page.fields) {
            switch (f.kind) {
                case FieldKind::Bool: {
                    auto e = NumEntry(f.key, *(bool*)f.value != f.inverted);
                    e.isBool = true;
                    out.push_back(std::move(e));
                    break;
                }
                case FieldKind::Preset:
                    break;
                case FieldKind::Int:
                    out.push_back(NumEntry(f.key, *(int*)f.value));
                    break;
                case FieldKind::String:
                case FieldKind::Choice:
                    out.push_back(StrEntry(f.key, *(std::wstring*)f.value));
                    break;
            }
        }
    }
    return out;
}

constexpr WCHAR kModRegistryPath[] =
    L"SOFTWARE\\Windhawk\\Engine\\Mods\\" WH_MOD_ID;

std::wstring EscapeRegString(std::wstring const& v) {
    std::wstring r;
    for (WCHAR c : v) {
        if (c == L'\\' || c == L'"') {
            r += L'\\';
        }
        r += c;
    }
    return r;
}

// The engine's SettingsChangeTime of this mod (0 if not found): Windhawk
// bumps it whenever the settings are saved, there or here.
DWORD ReadSettingsChangeTime() {
    DWORD changeTime = 0, size = sizeof(changeTime);
    RegGetValueW(HKEY_LOCAL_MACHINE, kModRegistryPath, L"SettingsChangeTime",
                 RRF_RT_REG_DWORD, nullptr, &changeTime, &size);
    return changeTime;
}

// A settings value this mod declares (or used to), so one the window may
// delete when it's no longer used (a removed group or item).
bool IsOwnSettingName(std::wstring_view name) {
    for (std::wstring_view prefix :
         {L"groups[", L"behavior.", L"tiles.", L"icons.", L"labels.",
          L"advanced."}) {
        if (name.size() > prefix.size() &&
            _wcsnicmp(name.data(), prefix.data(), prefix.size()) == 0) {
            return true;
        }
    }
    return false;
}

// SHA-256 of `data`, as uppercase hex; empty on failure.
std::wstring Sha256Hex(void const* data, DWORD size) {
    std::wstring hex;
    HCRYPTPROV prov = 0;
    HCRYPTHASH hash = 0;
    if (CryptAcquireContextW(&prov, nullptr, nullptr, PROV_RSA_AES,
                             CRYPT_VERIFYCONTEXT)) {
        if (CryptCreateHash(prov, CALG_SHA_256, 0, 0, &hash)) {
            BYTE digest[32];
            DWORD len = sizeof(digest);
            if (CryptHashData(hash, (BYTE const*)data, size, 0) &&
                CryptGetHashParam(hash, HP_HASHVAL, digest, &len, 0)) {
                static constexpr WCHAR kHex[] = L"0123456789ABCDEF";
                for (DWORD i = 0; i < len; i++) {
                    hex += kHex[digest[i] >> 4];
                    hex += kHex[digest[i] & 0xF];
                }
            }
            CryptDestroyHash(hash);
        }
        CryptReleaseContext(prov, 0);
    }
    return hex;
}

struct SaveResult {
    std::wstring error;  // empty: saved
    DWORD changeTime = 0;
};

// Windhawk keeps mod settings in a protected HKLM key, so the new values are
// imported elevated. Bumping SettingsChangeTime makes the engine call
// Wh_ModSettingsChanged, exactly like a change made in Windhawk.
//
// The .reg file is written to the user's temp folder, where any process of
// the user could change it before the elevated import. So the elevated step
// (PowerShell, one UAC prompt) reads the file once, checks its SHA-256
// against the one computed here (passed on its command line, visible in the
// prompt's details), writes those same bytes to a new file in
// %SystemRoot%\Temp, which standard users can't change, and imports that.
SaveResult WriteModSettings(std::vector<RegEntry> const& entries, HWND owner) {
    SaveResult result;
    HKEY modKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, kModRegistryPath, 0, KEY_READ,
                      &modKey) != ERROR_SUCCESS) {
        result.error =
            L"This mod's settings weren't found in the registry. Saving "
            L"from this window needs the installed (not portable) version "
            L"of Windhawk.";
        return result;
    }
    DWORD changeTime = 0, size = sizeof(changeTime);
    RegGetValueW(modKey, nullptr, L"SettingsChangeTime", RRF_RT_REG_DWORD,
                 nullptr, &changeTime, &size);
    // Values of removed groups and items. Only the mod's own settings.
    std::vector<std::wstring> stale;
    HKEY settingsKey;
    if (RegOpenKeyExW(modKey, L"Settings", 0, KEY_READ, &settingsKey) ==
        ERROR_SUCCESS) {
        for (DWORD i = 0;; i++) {
            WCHAR name[1024];
            DWORD len = ARRAYSIZE(name);
            LSTATUS st = RegEnumValueW(settingsKey, i, name, &len, nullptr,
                                       nullptr, nullptr, nullptr);
            if (st != ERROR_SUCCESS) {
                if (st == ERROR_MORE_DATA) {
                    continue;
                }
                break;
            }
            bool used = std::any_of(
                entries.begin(), entries.end(), [&](RegEntry const& e) {
                    return _wcsicmp(e.name.c_str(), name) == 0;
                });
            if (!used && IsOwnSettingName(name)) {
                stale.push_back(name);
            }
        }
        RegCloseKey(settingsKey);
    }
    RegCloseKey(modKey);

    DWORD newTime = std::max((DWORD)time(nullptr), changeTime + 1);
    std::wstring key = std::wstring(L"HKEY_LOCAL_MACHINE\\") + kModRegistryPath;
    std::wstring text = L"Windows Registry Editor Version 5.00\r\n\r\n";
    text += L"[" + key + L"\\Settings]\r\n";
    for (auto const& e : entries) {
        text += L"\"" + EscapeRegString(e.name) + L"\"=";
        if (e.isString) {
            text += L"\"" + EscapeRegString(e.text) + L"\"";
        } else {
            WCHAR num[16];
            swprintf(num, ARRAYSIZE(num), L"dword:%08x", e.number);
            text += num;
        }
        text += L"\r\n";
    }
    for (auto const& n : stale) {
        text += L"\"" + EscapeRegString(n) + L"\"=-\r\n";
    }
    WCHAR timeText[32];
    swprintf(timeText, ARRAYSIZE(timeText), L"dword:%08x", newTime);
    text += L"\r\n[" + key + L"]\r\n\"SettingsChangeTime\"=" + timeText +
            L"\r\n";

    // The file's bytes: UTF-16 with a byte order mark, as reg.exe expects.
    std::vector<BYTE> bytes(2 + text.size() * sizeof(WCHAR));
    bytes[0] = 0xFF;
    bytes[1] = 0xFE;
    memcpy(bytes.data() + 2, text.data(), text.size() * sizeof(WCHAR));
    std::wstring hash = Sha256Hex(bytes.data(), (DWORD)bytes.size());
    if (hash.empty()) {
        result.error = L"Preparing the settings failed.";
        return result;
    }

    WCHAR tempDir[MAX_PATH];
    GUID guid;
    WCHAR guidText[64];
    if (!GetTempPathW(ARRAYSIZE(tempDir), tempDir) ||
        FAILED(CoCreateGuid(&guid)) ||
        !StringFromGUID2(guid, guidText, ARRAYSIZE(guidText))) {
        result.error = L"The temporary folder wasn't found.";
        return result;
    }
    std::wstring regFile = std::wstring(tempDir) +
                           L"windhawk-start-menu-groups-" + guidText + L".reg";
    HANDLE f = CreateFileW(regFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                           nullptr, CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY,
                           nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        result.error = L"Couldn't write " + regFile + L".";
        return result;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(f, bytes.data(), (DWORD)bytes.size(), &written,
                        nullptr) &&
              written == bytes.size();
    CloseHandle(f);
    if (!ok) {
        DeleteFileW(regFile.c_str());
        result.error = L"Couldn't write " + regFile + L".";
        return result;
    }

    // In a single-quoted PowerShell string, ' is written ''.
    std::wstring quoted;
    for (WCHAR c : regFile) {
        quoted += c;
        if (c == L'\'') {
            quoted += L'\'';
        }
    }
    std::wstring script =
        L"try{$b=[IO.File]::ReadAllBytes('" + quoted + L"')}catch{exit 4};"
        L"$h=[BitConverter]::ToString([Security.Cryptography.SHA256]::"
        L"Create().ComputeHash($b)).Replace('-','');"
        L"if($h -ne '" + hash + L"'){exit 3};"
        L"$t=Join-Path $env:SystemRoot ('Temp\\windhawk-start-menu-groups-'+"
        L"[guid]::NewGuid().ToString()+'.reg');"
        L"try{[IO.File]::WriteAllBytes($t,$b)}catch{exit 5};"
        L"& (Join-Path $env:SystemRoot 'System32\\reg.exe') import $t *>$null;"
        L"$c=$LASTEXITCODE;"
        L"Remove-Item -LiteralPath $t -Force -ErrorAction SilentlyContinue;"
        L"exit $c";
    WCHAR sysDir[MAX_PATH];
    GetSystemDirectoryW(sysDir, ARRAYSIZE(sysDir));
    std::wstring powershell =
        std::wstring(sysDir) + L"\\WindowsPowerShell\\v1.0\\powershell.exe";
    std::wstring params =
        L"-NoProfile -NonInteractive -ExecutionPolicy Bypass -WindowStyle "
        L"Hidden -Command \"" +
        script + L"\"";
    SHELLEXECUTEINFOW sei{sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC;
    sei.hwnd = owner;
    sei.lpVerb = L"runas";
    sei.lpFile = powershell.c_str();
    sei.lpParameters = params.c_str();
    sei.nShow = SW_HIDE;
    // Blocks while the UAC prompt is up.
    if (!ShellExecuteExW(&sei)) {
        DWORD e = GetLastError();
        result.error =
            e == ERROR_CANCELLED
                ? L"The administrator prompt was canceled. Windhawk keeps mod "
                  L"settings in a protected registry key, so saving needs "
                  L"administrator approval."
                : L"Starting the import failed (error " + std::to_wstring(e) +
                      L").";
    } else if (sei.hProcess) {
        HANDLE waits[] = {sei.hProcess, g_stopEvent};
        DWORD w = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        DWORD exitCode = 1;
        if (w == WAIT_OBJECT_0) {
            GetExitCodeProcess(sei.hProcess, &exitCode);
        }
        CloseHandle(sei.hProcess);
        if (w != WAIT_OBJECT_0) {
            result.error = L"The mod was unloaded while saving.";
        } else if (exitCode == 3) {
            result.error =
                L"The settings file was changed by another program before "
                L"it could be imported, so nothing was saved.";
        } else if (exitCode != 0) {
            result.error = L"Importing the settings failed (exit code " +
                           std::to_wstring(exitCode) + L").";
        }
    }
    DeleteFileW(regFile.c_str());
    if (result.error.empty()) {
        result.changeTime = newTime;
    }
    return result;
}

// --- The window -------------------------------------------------------------

// {3cbcf1bf-2f76-4e9c-96ab-e84b37972554}
struct DECLSPEC_NOVTABLE IDesktopWindowXamlSourceNative : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE AttachToWindow(HWND parentWnd) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* hWnd) = 0;
};
constexpr IID IID_IDesktopWindowXamlSourceNative = {
    0x3cbcf1bf,
    0x2f76,
    0x4e9c,
    {0x96, 0xab, 0xe8, 0x4b, 0x37, 0x97, 0x25, 0x54}};

// {e3dcd8c7-3057-4692-99c3-7b7720afda31}
struct DECLSPEC_NOVTABLE IDesktopWindowXamlSourceNative2
    : IDesktopWindowXamlSourceNative {
    virtual HRESULT STDMETHODCALLTYPE PreTranslateMessage(const MSG* message,
                                                          BOOL* result) = 0;
};
constexpr IID IID_IDesktopWindowXamlSourceNative2 = {
    0xe3dcd8c7,
    0x3057,
    0x4692,
    {0x99, 0xc3, 0x7b, 0x77, 0x20, 0xaf, 0xda, 0x31}};

// The settings window lives in File Explorer (Start can't host XAML islands,
// InitializeForCurrentThread / DesktopWindowXamlSource fail there with
// ERROR_NOT_SUPPORTED). These run on the mod's settings thread in Explorer.
// XAML objects of the settings thread: released there when it ends
// (SettingsHostThread), never by the CRT at process exit.
[[clang::no_destroy]] winrt::Windows::UI::Xaml::Hosting::WindowsXamlManager
    g_xamlManager{nullptr};
std::vector<AppEntry> g_pickerApps;  // Start apps for the app picker
bool g_isExplorer = false;

constexpr UINT kSaveDoneMsg = WM_APP + 10;
constexpr UINT kCloseNowMsg = WM_APP + 11;
constexpr UINT kRunLaterMsg = WM_APP + 13;
constexpr UINT kAppsLoadedMsg = WM_APP + 14;  // lParam: std::vector<AppEntry>*
constexpr UINT_PTR kIconTimerId = 1;
std::atomic<bool> g_pickerAppsLoading{false};

// The Windows Settings app's logo as an icon, `px` square; null if missing.
HICON SettingsLogoIcon(int px) {
    WCHAR path[MAX_PATH];
    if (!GetWindowsDirectoryW(path, MAX_PATH)) {
        return nullptr;
    }
    wcscat_s(path, L"\\ImmersiveControlPanel\\images\\"
                   L"logo.targetsize-256_altform-unplated.png");
    winrt::com_ptr<IShellItemImageFactory> factory;
    HBITMAP bmp = nullptr;
    if (FAILED(SHCreateItemFromParsingName(path, nullptr,
                                           IID_PPV_ARGS(factory.put()))) ||
        FAILED(factory->GetImage({px, px}, SIIGBF_RESIZETOFIT, &bmp)) ||
        !bmp) {
        return nullptr;
    }
    BITMAP bm{};
    GetObjectW(bmp, sizeof(bm), &bm);
    HBITMAP mask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, nullptr);
    ICONINFO ii{};
    ii.fIcon = TRUE;
    ii.hbmColor = bmp;
    ii.hbmMask = mask;
    HICON icon = CreateIconIndirect(&ii);
    DeleteObject(mask);
    DeleteObject(bmp);
    return icon;
}

// The settings window's icons (settings thread).
bool g_windowIconsMade = false;
HICON g_iconBig = nullptr;
HICON g_iconSmall = nullptr;

void DestroyWindowIcons() {
    for (HICON* icon : {&g_iconBig, &g_iconSmall}) {
        if (*icon) {
            DestroyIcon(*icon);
            *icon = nullptr;
        }
    }
    g_windowIconsMade = false;
}

// The settings window's taskbar identity: its own app ID (so it isn't one
// of File Explorer's windows on the taskbar) and the Settings gear icon.
void SetSettingsWindowIdentity(HWND hwnd) {
    // PKEY_AppUserModel_ID
    constexpr PROPERTYKEY kAppIdKey = {
        {0x9F4C2855, 0x9F79, 0x4B39,
         {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
        5};
    winrt::com_ptr<IPropertyStore> store;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hwnd,
                                              IID_PPV_ARGS(store.put())))) {
        PROPVARIANT pv{};
        pv.vt = VT_LPWSTR;
        pv.pwszVal = const_cast<PWSTR>(L"Windhawk.StartMenuGroups.Settings");
        store->SetValue(kAppIdKey, pv);
        store->Commit();
    }
    // Made once, destroyed when the settings thread ends (DestroyWindowIcons).
    if (!g_windowIconsMade) {
        g_windowIconsMade = true;
        g_iconBig = SettingsLogoIcon(GetSystemMetrics(SM_CXICON));
        g_iconSmall = SettingsLogoIcon(GetSystemMetrics(SM_CXSMICON));
        if (!g_iconBig || !g_iconSmall) {
            // Fallback: the Settings app's icon from its executable.
            WCHAR exe[MAX_PATH];
            if (GetWindowsDirectoryW(exe, MAX_PATH)) {
                wcscat_s(exe, L"\\ImmersiveControlPanel\\SystemSettings.exe");
                HICON b = nullptr, sm = nullptr;
                if (ExtractIconExW(exe, 0, &b, &sm, 1)) {
                    if (!g_iconBig) {
                        g_iconBig = b;
                    } else if (b) {
                        DestroyIcon(b);
                    }
                    if (!g_iconSmall) {
                        g_iconSmall = sm;
                    } else if (sm) {
                        DestroyIcon(sm);
                    }
                }
            }
        }
    }
    if (g_iconBig) {
        SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_iconBig);
    }
    if (g_iconSmall) {
        SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_iconSmall);
    }
}

bool IsDarkMode() {
    DWORD light = 1, size = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\"
                 L"Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light,
                 &size);
    return light == 0;
}

xm::SolidColorBrush Brush(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return xm::SolidColorBrush(winrt::Windows::UI::Color{a, r, g, b});
}

wf::IInspectable Box(std::wstring_view s) {
    return winrt::box_value(winrt::hstring(s));
}

constexpr PCWSTR kDonateUrl = L"https://buymeacoffee.com/widget/page/danrotaru";

void OpenUrl(std::wstring url) {
    RunWorker([url = std::move(url)] {
        ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr,
                      SW_SHOWNORMAL);
    });
}

// Weak references to the settings window's elements are kept as
// FrameworkElement and cast back: weak_ref<xc::Border> and the like failed to
// resolve in File Explorer's island (the icons never loaded, renaming a
// group did nothing), weak_ref<FrameworkElement> works.
using WeakElement = winrt::weak_ref<FrameworkElement>;

template <typename T>
T Resolve(WeakElement const& weak) {
    auto fe = weak.get();
    return fe ? fe.try_as<T>() : T{nullptr};
}

class SettingsWindow {
   public:
    bool Create();
    HWND Hwnd() const { return m_hwnd; }
    // Shows a group's item (from "Edit" in Start) on the Groups page.
    void ShowItem(int group, int item);
    LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);

    // Lets the island handle Tab, arrow keys and accelerators.
    bool PreTranslate(MSG const* msg) {
        if (!m_source) {
            return false;
        }
        winrt::com_ptr<IDesktopWindowXamlSourceNative2> native;
        if (FAILED(winrt::get_unknown(m_source)->QueryInterface(
                IID_IDesktopWindowXamlSourceNative2, native.put_void()))) {
            return false;
        }
        BOOL handled = FALSE;
        native->PreTranslateMessage(msg, &handled);
        return handled != FALSE;
    }

   private:
    // Building blocks
    xc::TextBlock Text(std::wstring const& text,
                       double size = 14,
                       bool secondary = false);
    xc::FontIcon Glyph(PCWSTR glyph, double size = 16);
    xc::Border Card(FrameworkElement const& content);
    xc::Grid SettingRow(PCWSTR glyph,
                        std::wstring const& title,
                        PCWSTR description,
                        FrameworkElement const& control);
    xc::Button IconButton(PCWSTR glyph,
                          std::wstring const& tip,
                          std::function<void()> onClick);
    xc::Button TextButton(PCWSTR glyph,
                          std::wstring const& text,
                          std::function<void()> onClick);
    // `refresh` receives a function that reloads the control from the value.
    FrameworkElement FieldControl(Field const& f,
                                  std::function<void()>* refresh);
    xc::Button ResetButton(std::function<void()> onReset);
    // `field` with a short explanation underneath.
    FrameworkElement WithHint(FrameworkElement const& field,
                              std::wstring const& hint);
    std::vector<std::function<void()>> m_resetUpdaters;  // fields page
    EditSettings m_defaults;  // pointed to by DescribePages(m_defaults)

    // Structure
    UIElement BuildRoot();
    void ApplyTheme();
    void ShowPage(std::wstring const& tag);
    FrameworkElement BuildFieldsPage(SettingsPage const& page);
    void BuildItemEditor();

    // Groups board: the groups drawn like in Start, edited by drag
    // and drop. Built from the settings being edited (m_model).
    struct BoardSurface;
    enum class DragKind { App, Group, Resize, NewApp };
    struct DragState {
        bool pressed = false;
        bool active = false;  // moved far enough to be a drag
        DragKind kind = DragKind::App;
        int cat = -1;
        int item = -1;
        int sub = -1;      // App: an app in the folder `item`
        std::wstring app;  // NewApp: Start app name
        wf::Point start{};
        std::shared_ptr<BoardSurface> surface;
        FrameworkElement source{nullptr};
        FrameworkElement ghost{nullptr};
        xc::Border indicator{nullptr};
        xc::TextBlock ghostText{nullptr};
        double scale = 1;       // zoom of the board (it's shrunk to fit)
        wf::Rect tileBounds{};  // Resize: the tile, in surface coordinates
        double startW = 0;      // Resize: the tile's size at the start
        double startH = 0;
        std::wstring originalSize;  // Resize: to restore when canceled
        std::wstring newSize;       // Resize result
        std::function<void()> click;  // run when released without a drag
        Input::Pointer pointer{nullptr};    // captured when the drag starts
        wf::Point grab{};  // pointer position within the ghost
        int targetCat = -1;
        int targetIndex = -1;
        // App drags: into this folder of targetCat, at targetIndex.
        int targetFolder = -1;
        // Group drag of a single app (see BuildBoardTile): dropped on the
        // middle of another tile, its app goes into that group.
        bool intoGroup = false;
        // Dropped on the middle of an app (item targetIndex of targetCat):
        // the two make a folder in its place.
        bool makeFolder = false;
    };
    FrameworkElement BuildBoardPage();
    void RebuildBoard();
    FrameworkElement BuildBoardTile(int ci,
                                    std::shared_ptr<BoardSurface> const& surf);
    FrameworkElement BuildBoardApp(EditItem const& item,
                                   double iconSize,
                                   bool showName,
                                   double nameGap = 4);
    // The opened group (its apps), or with `folder`, that folder's apps.
    void ShowOverflowFlyout(int ci,
                            FrameworkElement const& anchor,
                            int folder = -1);
    void OpenFolder(int ci, int fi);
    void RebuildFlyout();
    void RefreshPalette();
    void LoadIconsLater();
    void LoadIconBatch(int gen, int count);
    void MakeDraggable(FrameworkElement const& el,
                       std::shared_ptr<BoardSurface> const& surf,
                       DragKind kind,
                       int cat,
                       int item,
                       std::wstring app = L"",
                       std::function<void()> onClick = nullptr,
                       int sub = -1);
    void HookSurface(std::shared_ptr<BoardSurface> const& surf);
    void BeginInlineEdit(FrameworkElement const& anchor,
                         std::wstring const& value,
                         double fontSize,
                         std::function<void(std::wstring)> commit);
    void UpdateBoardHeading();
    void EditBoardHeading();
    void UpdateDrag(wf::Point p);
    void StartDragVisuals();
    EditItem* DraggedItem(DragState const& d);
    struct AppGhostLook {
        double width = 0, height = 0;
        Thickness padding{};
        double spacing = 4;
        double fontSize = 11;
        int maxLines = 1;
        bool showText = true;
        bool centered = true;
    };
    FrameworkElement AppGhost(FrameworkElement const& icon,
                              std::wstring const& name,
                              AppGhostLook const& look);
    void EndDrag(bool drop);
    void ApplyDrop(DragState const& d);
    // Puts the app at item `ii` of group `ci` and `item` in a new folder,
    // in the app's place, and selects the folder.
    void MakeFolderAt(int ci, int ii, EditItem item);
    void ShowRenameFlyout(int ci, FrameworkElement const& anchor);
    xc::MenuFlyout TileMenu(int ci, FrameworkElement const& tile);
    xc::MenuFlyout AppMenu(int ci, int ii);
    xc::MenuFlyout FolderAppMenu(int ci, int fi, int si);
    void RefreshGroupsUi();
    // Details panel of the board page: the selected group and app.
    FrameworkElement BuildBoardDetails();
    void LoadBoardDetails();
    void LoadBoardGroupFields();
    void SelectBoardItem(int ci, int ii, int sub = -1);
    void ScheduleBoardRebuild(bool palette);
    void RefreshBoardAfterEdit();
    void SelectBoardGroup(int ci);
    void RemoveGroupKeepApp(int ci);
    void UpdateBoardSelection();
    void DuplicateSelected();
    void DeleteSelected();
    void Later(std::function<void()> fn);

    void OnChanged();
    void UpdateFooter();

    // Actions
    void AddGroup();
    void DuplicateGroup();
    void DeleteGroup();
    // "app", "program", "website" or "folder"; into a folder of a group
    // with `folderCat` and `folder`.
    void AddItem(std::wstring const& type, int folderCat = -1, int folder = -1);
    void DuplicateItem();
    void DeleteItem();
    void ExportSettings();
    void ImportSettings();
    std::wstring Browse(PCWSTR title, bool icons);
    void Save(bool closeAfter, bool overwrite = false);
    void Discard();
    void ReadModel();
    void RequestClose();
    // The App settings card, scrolled to when an app is clicked or edited.
    winrt::weak_ref<xc::Border> m_appCard;
    winrt::weak_ref<xc::ScrollViewer> m_pageScroll;  // the page's scrolling
    void ScrollToAppSettings();
    void ShowUnsavedChangesDialog();
    void ShowConfirmDialog(std::wstring const& title,
                           std::wstring const& text,
                           std::wstring const& action,
                           std::function<void()> onAction);
    void RemoveSelectedGroup();
    xc::ContentDialog m_closeDialog{nullptr};

    EditGroup* SelectedGroup();
    EditItem* SelectedItem();

    HWND m_hwnd = nullptr;
    HWND m_island = nullptr;
    winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource m_source{
        nullptr};
    bool m_dark = true;

    xc::Grid m_root{nullptr};
    xc::ListView m_nav{nullptr};
    xc::ContentControl m_pageHost{nullptr};
    xc::TextBlock m_status{nullptr};
    xc::Button m_saveButton{nullptr};
    xc::Button m_discardButton{nullptr};

    // Selection and the item editor (on the board page's details panel)
    xc::StackPanel m_editor{nullptr};
    xc::TextBlock m_appCaption{nullptr};  // the selected app's name
    xc::ToggleSwitch GroupToggle(PCWSTR header, bool EditGroup::*field);
    xc::Button AddAppButton();
    void FillAppGridCombo(xc::ComboBox const& combo,
                          std::vector<std::wstring>& values,
                          EditGroup const* cat);
    int m_selCat = -1;
    int m_selItem = -1;
    int m_selSub = -1;  // an app in the folder m_selItem, or -1
    bool m_refreshing = false;

    // Groups board
    std::shared_ptr<BoardSurface> m_board;
    xc::Border m_boardHost{nullptr};
    winrt::weak_ref<xc::VariableSizedWrapGrid> m_boardWrap;
    winrt::weak_ref<xc::Border> m_boardFrame;
    // Start's width for the groups (saved by Start, see ApplyColumns), and
    // the dashed outline showing it on the board.
    double m_startRowWidth = 0;
    // The Apps list next to the board: as many columns as the room the
    // board leaves (see LayoutAppsPanel).
    double m_boardNaturalWidth = 0;
    winrt::weak_ref<xc::Border> m_boardCard;
    winrt::weak_ref<xc::Grid> m_appsPanel;
    winrt::weak_ref<winrt::Windows::UI::Xaml::Shapes::Rectangle> m_startOutline;
    void FitBoardRow();
    // The board's row, fitted to Start's width as in Start (FitRow).
    int m_boardHalves = 0;      // half cells per row of the board
    double m_boardTile = 0;     // a tile's size across
    double m_boardHalf = 0;     // a half cell
    RowFit BoardRowFit() const {
        return FitRow(m_startRowWidth, m_model.tileSize, m_model.gapX);
    }
    double BoardRowWidth(RowFit const& fit) const {
        return RowFrameWidth(fit.halves, fit.half, m_model.gapX);
    }
    // A group's layout on the board: no wider than its row, as in Start.
    GroupLayout BoardLayoutOf(EditGroup const& g) {
        GroupLayout l = LayoutOf(g);
        if (m_boardHalves > 0 && l.spanHalfColumns > m_boardHalves) {
            l.spanHalfColumns = m_boardHalves;
        }
        return l;
    }
    // Drop target of a Start app on the board's empty space: a new group.
    static constexpr int kNewGroupTarget = -2;
    void LayoutAppsPanel();
    std::shared_ptr<BoardSurface> m_flyoutSurface;
    xc::Border m_flyoutHost{nullptr};
    int m_flyoutCat = -1;
    int m_flyoutFolder = -1;  // the opened folder (item), or -1: the group
    // Per group (index): its first app in the overflow cluster, as last
    // built on the board (0: none). The opened group shows those, like
    // Start's.
    std::map<int, int> m_overflowFirst;
    winrt::weak_ref<xc::Flyout> m_overflowFlyout;  // the opened group
    void HandDragToBoard(wf::Point p);
    xc::StackPanel m_paletteList{nullptr};
    xc::TextBox m_paletteSearch{nullptr};
    // The All Apps list as built: for which search and list of apps, and
    // its cells (index into g_pickerApps), updated in place.
    struct PaletteCell {
        xc::Border cell;
        xc::TextBlock name;
        size_t app;
    };
    std::vector<PaletteCell> m_paletteCells;
    std::wstring m_paletteQuery;
    size_t m_paletteAppCount = 0;
    bool m_paletteBuilt = false;
    std::deque<std::pair<xc::Border, std::wstring>>
        m_pendingIcons;
    int m_iconGeneration = 0;
    std::vector<std::function<void()>> m_later;  // see Later()
    xc::Border m_boardHeadingHost{nullptr};
    int m_boardSelCat = -1;  // group with the accent border
    int m_labelClickCat = -1;  // last clicked group name (double click)
    // Details panel (board page).
    xc::TextBox m_boardCatName{nullptr};
    xc::ComboBox m_boardTileSize{nullptr};
    xc::ToggleSwitch m_boardCatShowNames{nullptr};
    xc::ToggleSwitch m_boardCatShowLabel{nullptr};
    xc::ToggleSwitch m_boardCatShowBackground{nullptr};
    xc::ComboBox m_boardAppGrid{nullptr};
    std::vector<std::wstring> m_boardAppGridValues;
    std::vector<std::wstring> m_boardTileSizeValues;
    std::vector<xc::Control> m_boardCatControls;   // enabled with a group
    std::vector<xc::Control> m_boardItemControls;  // enabled with an app
    // Board page toolbar: acts on the selected app, or else the group.
    xc::Button m_boardAddApp{nullptr};
    xc::Button m_boardDup{nullptr};
    xc::Button m_boardDel{nullptr};
    int m_detailsCat = -2, m_detailsItem = -2;     // what the panel shows
    int m_detailsSub = -2;
    bool m_boardRebuildQueued = false;
    bool m_paletteRebuildQueued = false;
    ULONGLONG m_labelClickTime = 0;
    DragState m_drag;

    EditSettings m_model;
    std::vector<RegEntry> m_savedEntries;
    // Windhawk's SettingsChangeTime when the settings were read: a newer one
    // at Save means they were changed in Windhawk meanwhile.
    DWORD m_loadedChangeTime = 0;
    std::vector<std::wstring> m_pageTags;
    std::wstring m_page;
    bool m_dirty = false;
    bool m_saving = false;
    bool m_closeAfterSave = false;
};

SettingsWindow* g_settingsWindow = nullptr;  // settings thread only
bool g_settingsClassRegistered = false;

PCWSTR SettingsClassName() {
    static std::wstring name =
        std::wstring(L"WindhawkSettings_") + WH_MOD_ID;
    return name.c_str();
}

LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        SetWindowLongPtrW(hWnd, GWLP_USERDATA,
                          (LONG_PTR)((CREATESTRUCTW*)lp)->lpCreateParams);
    }
    auto* w = (SettingsWindow*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
    LRESULT r;
    try {
        r = w ? w->HandleMessage(hWnd, msg, wp, lp)
              : DefWindowProcW(hWnd, msg, wp, lp);
    } catch (...) {
        Wh_Log(L"Settings window error %08X", winrt::to_hresult());
        r = DefWindowProcW(hWnd, msg, wp, lp);
    }
    if (msg == WM_NCDESTROY && w) {
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
        if (g_settingsWindow == w) {
            g_settingsWindow = nullptr;
        }
        delete w;
    }
    return r;
}

bool SettingsWindow::Create() {
    m_loadedChangeTime = ReadSettingsChangeTime();
    ReadModel();
    // Known before the pages are built, so they're built once, in the right
    // colors (ApplyTheme rebuilds the page only when the theme changes).
    m_dark = IsDarkMode();

    HMODULE self;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&SettingsWndProc, &self);

    POINT pt{};
    GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{sizeof(mi)};
    GetMonitorInfoW(mon, &mi);
    RECT work = mi.rcWork;
    // The system DPI is fine for the initial size; WM_DPICHANGED fixes up a
    // move to a monitor with a different scale.
    UINT dpi = GetDpiForSystem();
    // Wide enough for the board (as wide as Start, which saves its width)
    // and two columns of All Apps next to it: the navigation pane, the
    // page's and the board card's padding, the gap, the apps and slack.
    int startWidth = Wh_GetIntValue(kStartRowWidthValue, 0);
    if (startWidth <= 0) {
        startWidth = (int)kReferenceRowWidth;
    }
    int wantDips = std::max(1140, startWidth + 232 + 64 + 48 + 24 + 24 + 2 * 92 +
                                      26 + 48);
    int ww = std::min(MulDiv(wantDips, dpi, 96), (int)(work.right - work.left));
    int wh = std::min(MulDiv(820, dpi, 96), (int)(work.bottom - work.top));

    m_hwnd = CreateWindowExW(
        0, SettingsClassName(), L"Start Menu Groups Settings",
        WS_OVERLAPPEDWINDOW, work.left + (work.right - work.left - ww) / 2,
        work.top + (work.bottom - work.top - wh) / 2, ww, wh, nullptr, nullptr,
        self, this);
    if (!m_hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return false;
    }

    // A taskbar button of its own, not grouped with File Explorer (whose
    // process hosts the window): its own app ID, and the Windows Settings
    // gear as its icon (as the Taskbar Fluent Media Player mod does).
    SetSettingsWindowIdentity(m_hwnd);

    try {
        if (!g_xamlManager) {
            g_xamlManager = winrt::Windows::UI::Xaml::Hosting::
                WindowsXamlManager::InitializeForCurrentThread();
        }
        m_source = winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource();
        winrt::com_ptr<IDesktopWindowXamlSourceNative> native;
        winrt::check_hresult(winrt::get_unknown(m_source)->QueryInterface(
            IID_IDesktopWindowXamlSourceNative, native.put_void()));
        winrt::check_hresult(native->AttachToWindow(m_hwnd));
        winrt::check_hresult(native->get_WindowHandle(&m_island));
        m_source.Content(BuildRoot());
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        SetWindowPos(m_island, nullptr, 0, 0, rc.right, rc.bottom,
                     SWP_SHOWWINDOW | SWP_NOZORDER);
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Creating the settings UI failed: %08X", hr);
        WCHAR text[256];
        swprintf(text, ARRAYSIZE(text),
                 L"The settings window couldn't be created (error %08X).\n\n"
                 L"The settings can still be changed on the mod's Settings "
                 L"tab in Windhawk.",
                 (unsigned)hr);
        MessageBoxW(m_hwnd, text, L"Start Menu Groups",
                    MB_ICONERROR | MB_SETFOREGROUND);
        DestroyWindow(m_hwnd);
        return false;
    }

    ApplyTheme();
    ShowWindow(m_hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(m_hwnd);
    return true;
}

LRESULT SettingsWindow::HandleMessage(HWND hWnd,
                                      UINT msg,
                                      WPARAM wp,
                                      LPARAM lp) {
    switch (msg) {
        case WM_SIZE:
            if (m_island) {
                SetWindowPos(m_island, nullptr, 0, 0, LOWORD(lp), HIWORD(lp),
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        case WM_GETMINMAXINFO: {
            UINT dpi = GetDpiForWindow(hWnd);
            auto* info = (MINMAXINFO*)lp;
            info->ptMinTrackSize.x = MulDiv(820, dpi ? dpi : 96, 96);
            info->ptMinTrackSize.y = MulDiv(540, dpi ? dpi : 96, 96);
            return 0;
        }
        case WM_DPICHANGED: {
            auto* r = (RECT*)lp;
            SetWindowPos(hWnd, nullptr, r->left, r->top, r->right - r->left,
                         r->bottom - r->top, SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        }
        case WM_ERASEBKGND: {
            // The island's color until it has drawn, instead of white.
            RECT rc;
            GetClientRect(hWnd, &rc);
            HBRUSH brush = CreateSolidBrush(m_dark ? RGB(0x20, 0x20, 0x20)
                                                   : RGB(0xF3, 0xF3, 0xF3));
            FillRect((HDC)wp, &rc, brush);
            DeleteObject(brush);
            return 1;
        }
        case WM_TIMER:
            if (wp == kIconTimerId) {
                LoadIconBatch(m_iconGeneration, 6);
            }
            return 0;
        case kAppsLoadedMsg: {
            std::unique_ptr<std::vector<AppEntry>> apps(
                (std::vector<AppEntry>*)lp);
            g_pickerAppsLoading = false;
            g_pickerApps = std::move(*apps);
            // Icons and suggestions of the shown page need the apps.
            m_paletteBuilt = false;  // a new list of apps
            if (m_page == L"board") {
                RebuildBoard();
                RefreshPalette();
            }
            return 0;
        }
        case WM_SETFOCUS:
            if (m_island) {
                SetFocus(m_island);
            }
            return 0;
        case WM_SETTINGCHANGE:
            if (lp && wcscmp((PCWSTR)lp, L"ImmersiveColorSet") == 0) {
                ApplyTheme();
            }
            break;
        case WM_CLOSE:
            RequestClose();
            return 0;
        case kCloseNowMsg:
            DestroyWindow(hWnd);
            return 0;
        case kRunLaterMsg: {
            auto queue = std::move(m_later);
            m_later.clear();
            for (auto& fn : queue) {
                fn();
            }
            return 0;
        }
        case kSaveDoneMsg: {
            std::unique_ptr<SaveResult> result((SaveResult*)lp);
            auto const& error = result->error;
            m_saving = false;
            if (error.empty()) {
                m_loadedChangeTime = result->changeTime;
                m_savedEntries = BuildRegEntries(m_model);
                OnChanged();
                m_status.Text(L"Saved. Start has been updated.");
                if (m_closeAfterSave) {
                    DestroyWindow(hWnd);
                    return 0;
                }
            } else {
                m_status.Text(error);
                UpdateFooter();
            }
            return 0;
        }
        case WM_DESTROY:
            if (m_source) {
                try {
                    m_source.Close();
                } catch (...) {
                }
                m_source = nullptr;
            }
            m_island = nullptr;
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wp, lp);
}

void SettingsWindow::ApplyTheme() {
    bool wasDark = m_dark;
    m_dark = IsDarkMode();
    BOOL darkCaption = m_dark;
    DwmSetWindowAttribute(m_hwnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */,
                          &darkCaption, sizeof(darkCaption));
    COLORREF caption = m_dark ? RGB(0x20, 0x20, 0x20) : RGB(0xF3, 0xF3, 0xF3);
    DwmSetWindowAttribute(m_hwnd, 35 /* DWMWA_CAPTION_COLOR */, &caption,
                          sizeof(caption));
    if (m_root) {
        m_root.RequestedTheme(m_dark ? ElementTheme::Dark : ElementTheme::Light);
        m_root.Background(m_dark ? Brush(255, 0x20, 0x20, 0x20)
                                 : Brush(255, 0xF3, 0xF3, 0xF3));
        // Re-render the current page with the new card colors.
        if (!m_page.empty() && m_dark != wasDark) {
            ShowPage(m_page);
        }
    }
}

// --- Building blocks --------------------------------------------------------

xc::TextBlock SettingsWindow::Text(std::wstring const& text,
                                   double size,
                                   bool secondary) {
    xc::TextBlock t;
    t.Text(text);
    t.FontSize(size);
    t.TextWrapping(TextWrapping::Wrap);
    if (secondary) {
        t.Opacity(0.72);
    }
    return t;
}

xc::FontIcon SettingsWindow::Glyph(PCWSTR glyph, double size) {
    xc::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(size);
    icon.FontFamily(xm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
    return icon;
}

xc::Border SettingsWindow::Card(FrameworkElement const& content) {
    xc::Border b;
    b.Background(m_dark ? Brush(0x0D, 255, 255, 255)
                        : Brush(0xB3, 255, 255, 255));
    b.BorderBrush(m_dark ? Brush(0x19, 0, 0, 0) : Brush(0x0F, 0, 0, 0));
    b.BorderThickness(ThicknessHelper::FromUniformLength(1));
    b.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
    b.Padding(ThicknessHelper::FromLengths(16, 12, 16, 12));
    b.Margin(ThicknessHelper::FromLengths(0, 0, 0, 4));
    b.MinHeight(66);
    b.Child(content);
    return b;
}

xc::Grid SettingsWindow::SettingRow(PCWSTR glyph,
                                    std::wstring const& title,
                                    PCWSTR description,
                                    FrameworkElement const& control) {
    xc::Grid g;
    g.ColumnSpacing(16);
    for (auto w : {GridLengthHelper::Auto(),
                   GridLengthHelper::FromValueAndType(1, GridUnitType::Star),
                   GridLengthHelper::Auto()}) {
        xc::ColumnDefinition cd;
        cd.Width(w);
        g.ColumnDefinitions().Append(cd);
    }
    auto icon = Glyph(glyph ? glyph : L"\uE713", 18);
    icon.VerticalAlignment(VerticalAlignment::Center);
    g.Children().Append(icon);

    xc::StackPanel text;
    text.VerticalAlignment(VerticalAlignment::Center);
    text.Children().Append(Text(title, 14));
    if (description) {
        text.Children().Append(Text(description, 12, true));
    }
    xc::Grid::SetColumn(text, 1);
    g.Children().Append(text);

    control.VerticalAlignment(VerticalAlignment::Center);
    xc::Grid::SetColumn(control, 2);
    g.Children().Append(control);
    return g;
}

xc::Button SettingsWindow::IconButton(PCWSTR glyph,
                                      std::wstring const& tip,
                                      std::function<void()> onClick) {
    xc::Button b;
    b.Content(Glyph(glyph, 14));
    b.Padding(ThicknessHelper::FromLengths(10, 7, 10, 7));
    xc::ToolTipService::SetToolTip(b, Box(tip));
    b.Click([onClick](auto&&, auto&&) { onClick(); });
    return b;
}

xc::Button SettingsWindow::TextButton(PCWSTR glyph,
                                      std::wstring const& text,
                                      std::function<void()> onClick) {
    xc::Button b;
    xc::StackPanel sp;
    sp.Orientation(xc::Orientation::Horizontal);
    sp.Spacing(8);
    if (glyph) {
        sp.Children().Append(Glyph(glyph, 14));
    }
    sp.Children().Append(Text(text, 14));
    b.Content(sp);
    b.Click([onClick](auto&&, auto&&) { onClick(); });
    return b;
}

FrameworkElement SettingsWindow::FieldControl(Field const& f,
                                              std::function<void()>* refresh) {
    switch (f.kind) {
        case FieldKind::Bool: {
            auto* v = (bool*)f.value;
            xc::ToggleSwitch t;
            t.IsOn(*v);
            t.MinWidth(0);
            t.OnContent(Box(L"On"));
            t.OffContent(Box(L"Off"));
            t.Toggled([this, v, t](auto&&, auto&&) {
                *v = t.IsOn();
                OnChanged();
            });
            *refresh = [v, t] { t.IsOn(*v); };
            return t;
        }
        case FieldKind::Int: {
            auto* v = (int*)f.value;
            xc::StackPanel sp;
            sp.Orientation(xc::Orientation::Horizontal);
            sp.Spacing(12);
            xc::Slider slider;
            slider.Width(200);
            slider.Minimum(f.min);
            slider.Maximum(f.max);
            slider.StepFrequency(1);
            slider.VerticalAlignment(VerticalAlignment::Center);
            xc::TextBox box;
            box.Width(72);
            slider.Value(*v);
            box.Text(std::to_wstring(*v));
            int lo = f.min, hi = f.max;
            auto syncing = std::make_shared<bool>(false);
            slider.ValueChanged([this, v, box, syncing](auto&&,
                                                         auto const& e) {
                if (*syncing) {
                    return;
                }
                *syncing = true;
                *v = (int)std::lround(e.NewValue());
                box.Text(std::to_wstring(*v));
                *syncing = false;
                OnChanged();
            });
            box.LostFocus([this, v, box, slider, lo, hi, syncing](auto&&,
                                                                  auto&&) {
                int n = (int)Clamp(_wtoi(box.Text().c_str()), lo, hi);
                *syncing = true;
                *v = n;
                box.Text(std::to_wstring(n));
                slider.Value(n);
                *syncing = false;
                OnChanged();
            });
            sp.Children().Append(slider);
            sp.Children().Append(box);
            *refresh = [v, box, slider, syncing] {
                *syncing = true;
                slider.Value(*v);
                box.Text(std::to_wstring(*v));
                *syncing = false;
            };
            return sp;
        }
        case FieldKind::String: {
            auto* v = (std::wstring*)f.value;
            xc::TextBox box;
            box.Width(240);
            box.Text(*v);
            box.TextChanged([this, v, box](auto&&, auto&&) {
                *v = box.Text();
                OnChanged();
            });
            *refresh = [v, box] { box.Text(*v); };
            return box;
        }
        case FieldKind::Choice: {
            auto* v = (std::wstring*)f.value;
            xc::ComboBox combo;
            combo.MinWidth(240);
            int sel = 0;
            for (size_t i = 0; i < f.options.size(); i++) {
                combo.Items().Append(Box(f.options[i].second));
                if (f.options[i].first == *v) {
                    sel = (int)i;
                }
            }
            combo.SelectedIndex(sel);
            auto values = std::make_shared<std::vector<std::wstring>>();
            for (auto& o : f.options) {
                values->push_back(o.first);
            }
            combo.SelectionChanged([this, v, combo, values](auto&&, auto&&) {
                int i = combo.SelectedIndex();
                if (i >= 0 && i < (int)values->size()) {
                    *v = (*values)[i];
                    OnChanged();
                }
            });
            *refresh = [v, combo, values] {
                for (size_t i = 0; i < values->size(); i++) {
                    if ((*values)[i] == *v) {
                        combo.SelectedIndex((int)i);
                    }
                }
            };
            return combo;
        }
        case FieldKind::Preset: {
            auto* s = (EditSettings*)f.value;
            xc::ComboBox combo;
            combo.MinWidth(240);
            for (auto const& p : kSizePresets) {
                combo.Items().Append(Box(p.name));
            }
            constexpr int count = (int)ARRAYSIZE(kSizePresets);
            auto syncing = std::make_shared<bool>(false);
            // Shows the preset of the current values.
            auto sync = [s, combo, syncing] {
                int want = CurrentPreset(*s);
                if (combo.SelectedIndex() != want) {
                    *syncing = true;
                    combo.SelectedIndex(want);
                    *syncing = false;
                }
            };
            sync();
            combo.SelectionChanged([this, s, combo, syncing](auto&&, auto&&) {
                int i = combo.SelectedIndex();
                if (*syncing || i < 0 || i >= count) {
                    return;
                }
                ApplyPreset(*s, kSizePresets[i]);
                OnChanged();
                // Rebuild the page so the other fields show the new values
                // (not from within this handler, the combo box is on it).
                Later([this] { ShowPage(m_page); });
            });
            *refresh = sync;
            return combo;
        }
    }
    *refresh = [] {};
    return xc::TextBlock();
}

xc::Button SettingsWindow::ResetButton(std::function<void()> onReset) {
    xc::Button b;
    b.Content(Glyph(L"", 14));  // Undo
    b.Padding(ThicknessHelper::FromLengths(8, 6, 8, 6));
    b.Background(xm::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    b.BorderThickness(ThicknessHelper::FromUniformLength(0));
    b.VerticalAlignment(VerticalAlignment::Center);
    xc::ToolTipService::SetToolTip(b, Box(L"Reset to default"));
    b.Click([onReset](auto&&, auto&&) { onReset(); });
    return b;
}

FrameworkElement SettingsWindow::WithHint(FrameworkElement const& field,
                                          std::wstring const& hint) {
    xc::StackPanel sp;
    sp.Spacing(4);
    sp.Children().Append(field);
    auto text = Text(hint, 12, true);
    text.Margin(ThicknessHelper::FromLengths(2, 0, 0, 0));
    sp.Children().Append(text);
    return sp;
}

bool FieldValueEquals(Field const& a, Field const& b) {
    switch (a.kind) {
        case FieldKind::Bool:
            return *(bool*)a.value == *(bool*)b.value;
        case FieldKind::Int:
            return *(int*)a.value == *(int*)b.value;
        default:
            return SplitLines(*(std::wstring*)a.value) ==
                   SplitLines(*(std::wstring*)b.value);
    }
}

void CopyFieldValue(Field const& to, Field const& from) {
    switch (to.kind) {
        case FieldKind::Bool:
            *(bool*)to.value = *(bool*)from.value;
            break;
        case FieldKind::Int:
            *(int*)to.value = *(int*)from.value;
            break;
        default:
            *(std::wstring*)to.value = *(std::wstring*)from.value;
            break;
    }
}

// --- Structure --------------------------------------------------------------

UIElement SettingsWindow::BuildRoot() {
    m_root = xc::Grid();
    // Delete on the Groups page removes the selected app, or else the
    // selected group (as the toolbar's Delete does). Not while typing in a
    // field, where it deletes text; also when something handled it already
    // (e.g. the navigation list having the focus).
    m_root.AddHandler(
        UIElement::KeyDownEvent(),
        winrt::box_value(Input::KeyEventHandler(
            [this](wf::IInspectable const&,
                   Input::KeyRoutedEventArgs const& e) {
                if (e.Key() != winrt::Windows::System::VirtualKey::Delete ||
                    m_page != L"board" || m_closeDialog || !m_root ||
                    !m_root.XamlRoot()) {
                    return;
                }
                auto focused =
                    Input::FocusManager::GetFocusedElement(m_root.XamlRoot());
                if (focused && (focused.try_as<xc::TextBox>() ||
                                focused.try_as<xc::AutoSuggestBox>() ||
                                focused.try_as<xc::ComboBox>() ||
                                focused.try_as<xc::PasswordBox>())) {
                    return;
                }
                e.Handled(true);
                Later([this] { DeleteSelected(); });
            })),
        true);
    xc::ColumnDefinition paneCol;
    paneCol.Width(GridLengthHelper::FromPixels(232));
    m_root.ColumnDefinitions().Append(paneCol);
    xc::ColumnDefinition contentCol;
    contentCol.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    m_root.ColumnDefinitions().Append(contentCol);

    // Navigation pane.
    xc::Grid pane;
    pane.Padding(ThicknessHelper::FromLengths(8, 16, 8, 16));
    xc::RowDefinition r0, r1, r2;
    r0.Height(GridLengthHelper::Auto());
    r1.Height(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    r2.Height(GridLengthHelper::Auto());
    pane.RowDefinitions().Append(r0);
    pane.RowDefinitions().Append(r1);
    pane.RowDefinitions().Append(r2);

    xc::StackPanel title;
    title.Orientation(xc::Orientation::Horizontal);
    title.Spacing(12);
    title.Margin(ThicknessHelper::FromLengths(12, 0, 0, 20));
    auto titleText = Text(L"Start Menu Groups", 18);
    titleText.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    title.Children().Append(titleText);
    pane.Children().Append(title);

    m_nav = xc::ListView();
    m_nav.SelectionMode(xc::ListViewSelectionMode::Single);
    xc::Grid::SetRow(m_nav, 1);
    auto addNav = [&](PCWSTR tag, PCWSTR glyph, PCWSTR text) {
        xc::ListViewItem item;
        xc::StackPanel sp;
        sp.Orientation(xc::Orientation::Horizontal);
        sp.Spacing(14);
        sp.Children().Append(Glyph(glyph, 16));
        sp.Children().Append(Text(text, 14));
        item.Content(sp);
        item.Height(40);
        m_nav.Items().Append(item);
        m_pageTags.push_back(tag);
    };
    addNav(L"board", L"\uF0E2", L"Groups");
    for (auto& page : DescribePages(m_model)) {
        addNav(page.tag, page.glyph, page.title);
    }
    m_nav.SelectionChanged([this](auto&&, auto&&) {
        int i = m_nav.SelectedIndex();
        if (i >= 0 && i < (int)m_pageTags.size()) {
            ShowPage(m_pageTags[i]);
        }
    });
    pane.Children().Append(m_nav);

    // Pane footer: Import / Export, Donate and the credit line, as in the
    // Explorer Command Bar mod.
    auto stretch = [](xc::Button b) {
        b.HorizontalAlignment(HorizontalAlignment::Stretch);
        b.HorizontalContentAlignment(HorizontalAlignment::Center);
        return b;
    };
    xc::Grid transfer;
    transfer.ColumnSpacing(8);
    for (int k = 0; k < 2; k++) {
        xc::ColumnDefinition cd;
        cd.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        transfer.ColumnDefinitions().Append(cd);
    }
    auto importButton =
        stretch(TextButton(nullptr, L"Import", [this] { ImportSettings(); }));
    xc::ToolTipService::SetToolTip(importButton,
                                   Box(L"Import settings from a file"));
    transfer.Children().Append(importButton);
    auto exportButton =
        stretch(TextButton(nullptr, L"Export", [this] { ExportSettings(); }));
    xc::ToolTipService::SetToolTip(exportButton,
                                   Box(L"Export all settings to a file"));
    xc::Grid::SetColumn(exportButton, 1);
    transfer.Children().Append(exportButton);

    auto donateButton =
        stretch(TextButton(nullptr, L"Donate", [] { OpenUrl(kDonateUrl); }));
    xc::ToolTipService::SetToolTip(
        donateButton, Box(L"Support the development of this mod"));

    // An inline Hyperlink can crash File Explorer's XAML when clicked, so the
    // author link is a HyperlinkButton next to the text.
    xc::StackPanel credit;
    credit.Orientation(xc::Orientation::Horizontal);
    credit.HorizontalAlignment(HorizontalAlignment::Center);
    auto creditText = Text(L"", 12, true);
    creditText.TextWrapping(TextWrapping::NoWrap);
    creditText.VerticalAlignment(VerticalAlignment::Center);
    winrt::Windows::UI::Xaml::Documents::Run createdWith;
#ifdef WH_MOD_VERSION
    createdWith.Text(L"v" WH_MOD_VERSION L" | Created with ");
#else
    createdWith.Text(L"Created with ");
#endif
    creditText.Inlines().Append(createdWith);
    winrt::Windows::UI::Xaml::Documents::Run heart;
    heart.Text(L"\u2665");
    heart.Foreground(Brush(255, 0xF0, 0x4A, 0x5E));
    creditText.Inlines().Append(heart);
    winrt::Windows::UI::Xaml::Documents::Run by;
    by.Text(L" by\u00A0");
    creditText.Inlines().Append(by);
    credit.Children().Append(creditText);
    xc::HyperlinkButton authorLink;
    xc::TextBlock authorName;
    authorName.Text(L"DanRotaru");
    authorName.FontSize(12);
    authorName.TextDecorations(
        winrt::Windows::UI::Text::TextDecorations::Underline);
    authorLink.Content(authorName);
    authorLink.Padding(ThicknessHelper::FromUniformLength(0));
    authorLink.MinHeight(0);
    authorLink.VerticalAlignment(VerticalAlignment::Center);
    xc::ToolTipService::SetToolTip(authorLink, Box(L"Developer homepage"));
    authorLink.Click([](auto&&, auto&&) {
        OpenUrl(L"https://dan13.me/?utm_source=start-menu-groups");
    });
    credit.Children().Append(authorLink);

    xc::StackPanel paneFooter;
    paneFooter.Spacing(8);
    paneFooter.Margin(ThicknessHelper::FromLengths(8, 12, 8, 0));
    paneFooter.Children().Append(transfer);
    paneFooter.Children().Append(donateButton);
    credit.Margin(ThicknessHelper::FromLengths(0, 4, 0, 0));
    paneFooter.Children().Append(credit);
    xc::Grid::SetRow(paneFooter, 2);
    pane.Children().Append(paneFooter);

    m_root.Children().Append(pane);

    // Content area: page + footer.
    xc::Grid content;
    xc::Grid::SetColumn(content, 1);
    xc::RowDefinition c0, c1;
    c0.Height(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    c1.Height(GridLengthHelper::Auto());
    content.RowDefinitions().Append(c0);
    content.RowDefinitions().Append(c1);

    m_pageHost = xc::ContentControl();
    m_pageHost.HorizontalContentAlignment(HorizontalAlignment::Stretch);
    m_pageHost.VerticalContentAlignment(VerticalAlignment::Stretch);
    content.Children().Append(m_pageHost);

    xc::Grid footer;
    footer.Padding(ThicknessHelper::FromLengths(32, 12, 32, 12));
    footer.ColumnSpacing(8);
    for (auto w : {GridLengthHelper::FromValueAndType(1, GridUnitType::Star),
                   GridLengthHelper::Auto(), GridLengthHelper::Auto()}) {
        xc::ColumnDefinition cd;
        cd.Width(w);
        footer.ColumnDefinitions().Append(cd);
    }
    m_status = Text(L"", 12, true);
    m_status.VerticalAlignment(VerticalAlignment::Center);
    footer.Children().Append(m_status);
    m_discardButton =
        TextButton(nullptr, L"Discard changes", [this] { Discard(); });
    xc::Grid::SetColumn(m_discardButton, 1);
    footer.Children().Append(m_discardButton);
    m_saveButton = TextButton(L"\uE74E", L"Save", [this] { Save(false); });
    try {
        if (auto style = Application::Current().Resources().TryLookup(
                Box(L"AccentButtonStyle"))) {
            m_saveButton.Style(style.as<Style>());
        }
    } catch (...) {
    }
    xc::Grid::SetColumn(m_saveButton, 2);
    footer.Children().Append(m_saveButton);
    xc::Grid::SetRow(footer, 1);
    content.Children().Append(footer);

    m_root.Children().Append(content);

    m_nav.SelectedIndex(0);
    UpdateFooter();
    return m_root;
}

void SettingsWindow::ShowPage(std::wstring const& tag) {
    m_page = tag;
    m_resetUpdaters.clear();
    xc::ScrollViewer sv;
    sv.VerticalScrollBarVisibility(xc::ScrollBarVisibility::Auto);
    sv.Padding(ThicknessHelper::FromLengths(32, 24, 32, 16));
    m_board = nullptr;
    m_boardHost = nullptr;
    // Controls of the other pages (they're built again with their page).
    m_editor = nullptr;
    m_appCaption = nullptr;
    m_appCard = nullptr;
    m_boardCatName = nullptr;
    m_boardTileSize = nullptr;
    m_boardCatShowNames = nullptr;
    m_boardCatShowLabel = nullptr;
    m_boardCatShowBackground = nullptr;
    m_boardAppGrid = nullptr;
    m_boardCatControls.clear();
    m_boardItemControls.clear();
    m_detailsCat = m_detailsItem = m_detailsSub = -2;
    m_paletteList = nullptr;
    m_paletteSearch = nullptr;
    m_paletteCells.clear();
    m_paletteBuilt = false;
    m_boardHeadingHost = nullptr;
    m_pendingIcons.clear();
    m_iconGeneration++;  // stop applying icons to the old page
    if (tag == L"board") {
        sv.Content(BuildBoardPage());
    } else {
        for (auto& page : DescribePages(m_model)) {
            if (tag == page.tag) {
                sv.Content(BuildFieldsPage(page));
            }
        }
    }
    m_pageHost.Content(sv);
    m_pageScroll = sv;
}

FrameworkElement SettingsWindow::BuildFieldsPage(SettingsPage const& page) {
    // The page's full width, as the Groups page. No MaxWidth: as the
    // ScrollViewer's content, a MaxWidth smaller than the viewport (a wide
    // window) shifts the page right and clips its controls.
    xc::StackPanel sp;
    sp.HorizontalAlignment(HorizontalAlignment::Stretch);
    auto title = Text(page.title, 28);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    sp.Children().Append(title);
    auto desc = Text(page.description, 14, true);
    desc.Margin(ThicknessHelper::FromLengths(0, 4, 0, 20));
    sp.Children().Append(desc);
    m_resetUpdaters.clear();
    // The same page described over the defaults, to compare and reset.
    std::vector<Field> defaults;
    for (auto& p : DescribePages(m_defaults)) {
        if (std::wstring_view(p.tag) == page.tag) {
            defaults = p.fields;
        }
    }
    for (size_t i = 0; i < page.fields.size(); i++) {
        Field f = page.fields[i];
        if (f.hidden) {
            continue;
        }
        auto refresh = std::make_shared<std::function<void()>>();
        auto control = FieldControl(f, refresh.get());

        xc::StackPanel right;
        right.Orientation(xc::Orientation::Horizontal);
        right.Spacing(4);
        if (f.kind == FieldKind::Preset) {
            // No reset button; follows the values edited elsewhere.
            m_resetUpdaters.push_back([refresh] { (*refresh)(); });
        } else if (i < defaults.size()) {
            Field d = defaults[i];
            auto reset = ResetButton([this, f, d, refresh] {
                CopyFieldValue(f, d);
                (*refresh)();
                OnChanged();
            });
            // Shown only while the value differs from the default.
            auto update = [f, d, reset] {
                reset.Visibility(FieldValueEquals(f, d) ? Visibility::Collapsed
                                                        : Visibility::Visible);
            };
            update();
            m_resetUpdaters.push_back(update);
            right.Children().Append(reset);
        }
        right.Children().Append(control);
        sp.Children().Append(
            Card(SettingRow(f.glyph, f.title, f.description, right)));
    }
    return sp;
}

// --- Selection and the item editor -------------------------------------

EditGroup* SettingsWindow::SelectedGroup() {
    return m_selCat >= 0 && m_selCat < (int)m_model.groups.size()
               ? &m_model.groups[m_selCat]
               : nullptr;
}

EditItem* SettingsWindow::SelectedItem() {
    auto* cat = SelectedGroup();
    if (!cat || m_selItem < 0 || m_selItem >= (int)cat->items.size()) {
        return nullptr;
    }
    auto& item = cat->items[m_selItem];
    if (m_selSub < 0) {
        return &item;
    }
    return item.type == L"folder" && m_selSub < (int)item.children.size()
               ? &item.children[m_selSub]
               : nullptr;
}

// Icons in the settings window (settings thread only).
[[clang::no_destroy]] std::unordered_map<std::wstring, xm::ImageSource>
    g_settingsIcons;  // settings thread

xm::ImageSource SettingsIcon(std::wstring const& source,
                             HWND hwnd,
                             double size = 24) {
    if (source.empty()) {
        return nullptr;
    }
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 0;
    int px = (int)std::lround(size * (dpi ? dpi : 96) / 96.0);
    std::wstring key = source + L"|" + std::to_wstring(px);
    if (auto it = g_settingsIcons.find(key); it != g_settingsIcons.end()) {
        return it->second;
    }
    xm::ImageSource result{nullptr};
    if (auto pixels = ExtractAppIcon(source, px)) {
        uint32_t n = (uint32_t)pixels->bgra.size();
        winrt::Windows::Storage::Streams::Buffer buf(n);
        memcpy(buf.data(), pixels->bgra.data(), n);
        buf.Length(n);
        auto bitmap = wgi::SoftwareBitmap::CreateCopyFromBuffer(
            buf, wgi::BitmapPixelFormat::Bgra8, pixels->width, pixels->height,
            wgi::BitmapAlphaMode::Premultiplied);
        xmi::SoftwareBitmapSource src;
        src.SetBitmapAsync(bitmap);
        result = src;
    }
    if (result) {  // a failure is tried again next time
        g_settingsIcons.insert_or_assign(key, result);
    }
    return result;
}

// The Start app an item refers to (by name or app ID), if installed.
AppEntry const* FindPickerApp(std::wstring const& nameOrId) {
    for (auto const& a : g_pickerApps) {
        if (_wcsicmp(a.name.c_str(), nameOrId.c_str()) == 0 ||
            _wcsicmp(a.id.c_str(), nameOrId.c_str()) == 0) {
            return &a;
        }
    }
    return nullptr;
}

// Same icon resolution as the tiles in Start.
// The icon of an item without one of its own.
PCWSTR ItemGlyph(EditItem const& item) {
    return item.type == L"folder"    ? L"\uE8B7"   // folder
           : item.type == L"program" ? L"\uE756"   // command prompt
           : item.type == L"website" ? L"\uE774"  // globe
                                     : L"\uF0E2";  // grid (a Start app)
}

std::wstring ItemIconSource(EditItem const& item) {
    if (!item.icon.empty()) {
        return item.icon;
    }
    if (item.type == L"folder") {
        return L"";  // its apps' icons, see ItemIconEl
    }
    if (item.type == L"program") {
        return item.path;
    }
    if (item.type == L"website") {
        return L"";  // a globe
    }
    if (auto const* app = FindPickerApp(item.app)) {
        return app->iconSource;
    }
    return L"";
}

// An icon (24×24 by default), or the given glyph when there's none (yet).
FrameworkElement IconOrGlyph(std::wstring const& source,
                             PCWSTR glyph,
                             HWND hwnd,
                             double size = 24) {
    std::wstring text;
    bool isGlyph = false;
    if (ParseTextIcon(source, text, isGlyph)) {
        return MakeTextIcon(text, isGlyph, size);
    }
    if (IsWebAddress(source)) {
        return MakeLinkIcon(source, glyph, size);
    }
    if (auto src = SettingsIcon(source, hwnd, size)) {
        xc::Image img;
        img.Width(size);
        img.Height(size);
        img.Source(src);
        return img;
    }
    xc::FontIcon icon;
    icon.Glyph(glyph);
    icon.FontSize(std::round(size * 2 / 3));
    icon.Width(size);
    icon.Height(size);
    return icon;
}

std::wstring ItemTitle(EditItem const& item) {
    if (!item.label.empty()) {
        return item.label;
    }
    if (item.type == L"folder") {
        return kFolderName;
    }
    if (item.type == L"program") {
        std::wstring file = item.path.substr(item.path.find_last_of(L"\\/") + 1);
        return file.empty() ? L"(new program)" : file;
    }
    if (item.type == L"website") {
        return item.path.empty() ? L"(new website)" : WebsiteHost(item.path);
    }
    return item.app.empty() ? L"(new app)" : item.app;
}

// An item's icon at `size`, as in Start: a folder without an icon of its
// own shows its first apps' (or, while empty, a folder glyph).
FrameworkElement ItemIconEl(EditItem const& item, HWND hwnd, double size) {
    if (item.type == L"folder" && item.icon.empty()) {
        if (item.children.empty()) {
            auto box = FolderIconBox(size);
            xc::FontIcon glyph;
            glyph.Glyph(ItemGlyph(item));
            glyph.FontSize(std::round(size * 0.45));
            box.Child(glyph);
            return box;
        }
        return FolderPreview(item.children.size(), size,
                             [&](size_t k, double s) {
                                 return ItemIconEl(item.children[k], hwnd, s);
                             });
    }
    return IconOrGlyph(ItemIconSource(item), ItemGlyph(item), hwnd, size);
}

void SettingsWindow::BuildItemEditor() {
    if (!m_editor) {
        return;
    }
    m_editor.Children().Clear();
    EditItem* item = SelectedItem();
    if (m_appCaption) {
        m_appCaption.Text(item ? ItemTitle(*item) : L"");
    }
    if (!item) {
        m_editor.Children().Append(
            Text(SelectedGroup()
                     ? L"Select an app to edit it, or add a Start app, a "
                       L"program, a website or a folder."
                     : L"Create or select a group first.",
                 14, true));
        return;
    }
    const std::wstring type = item->type;
    const bool program = type == L"program";
    const bool website = type == L"website";
    // Edits of the selected item go through this, which re-resolves the
    // pointer since the vector may have changed in between.
    // Only real changes count: filling the fields fires TextChanged too, and
    // that must not touch the list.
    auto edit = [this](std::function<void(EditItem&)> fn) {
        if (auto* it = SelectedItem()) {
            EditItem before = *it;
            fn(*it);
            if (it->type == before.type && it->app == before.app &&
                it->label == before.label && it->path == before.path &&
                it->args == before.args && it->icon == before.icon) {
                return;
            }
            if (m_appCaption) {
                m_appCaption.Text(ItemTitle(*it));
            }
            OnChanged();
            if (m_page == L"board") {
                // Board only: the Start apps panel would reload all its
                // icons on every keystroke.
                ScheduleBoardRebuild(false);
            }
        }
    };

    // A field as a card, like the settings pages (and Explorer Command
    // Bar's): icon, title and description, a control on the right, and/or
    // one below at full width.
    auto card = [this](PCWSTR glyph, PCWSTR title, PCWSTR description,
                       FrameworkElement right, FrameworkElement below) {
        xc::StackPanel content;
        content.Spacing(10);
        xc::StackPanel empty;
        content.Children().Append(
            SettingRow(glyph, title, description,
                       right ? right : empty.as<FrameworkElement>()));
        if (below) {
            // Lined up with the title, after the icon.
            below.Margin(ThicknessHelper::FromLengths(34, 0, 0, 0));
            content.Children().Append(below);
        }
        m_editor.Children().Append(Card(content));
    };

    // The app's fields have no reset buttons (an empty field is the
    // default: the app's own name, icon, and so on).
    // A text box bound to a field of the item.
    auto textBox = [edit](std::wstring const& value, PCWSTR placeholder,
                          std::function<void(EditItem&, std::wstring)> setter) {
        xc::TextBox box;
        box.PlaceholderText(placeholder);
        box.Text(value);
        box.TextChanged([edit, box, setter](auto&&, auto&&) {
            std::wstring v(box.Text());
            edit([setter, v](EditItem& it) { setter(it, v); });
        });
        return box;
    };
    // A text box with Browse… after it.
    auto withBrowse = [this](xc::TextBox box, PCWSTR title,
                                         bool icons) -> FrameworkElement {
        xc::Grid row;
        row.ColumnSpacing(8);
        xc::ColumnDefinition a, b;
        a.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        b.Width(GridLengthHelper::Auto());
        row.ColumnDefinitions().Append(a);
        row.ColumnDefinitions().Append(b);
        row.Children().Append(box);
        auto browse = TextButton(L"\uE8DA", L"Browse\u2026",
                                 [this, box, icons, title] {
                                     std::wstring p = Browse(title, icons);
                                     if (!p.empty()) {
                                         box.Text(p);
                                     }
                                 });
        xc::Grid::SetColumn(browse, 1);
        row.Children().Append(browse);
        return row;
    };

    // Name
    auto nameCard = [&] {
        auto label = textBox(
            item->label,
            website   ? L"Empty = the website's address"
            : program ? L"Empty = the program's file name"
                      : L"Empty = the app's name",
            [](EditItem& it, std::wstring v) { it.label = v; });
        card(L"\uE8D2", L"Name",
             L"Shown under the icon, in the tooltip and in the opened group.",
             nullptr, label);
    };

    if (type == L"folder") {
        // A folder: its name, its apps (edited in the opened folder) and
        // its icon.
        auto label = textBox(item->label, L"Folder",
                             [](EditItem& it, std::wstring v) { it.label = v; });
        card(L"\uE8D2", L"Name",
             L"Shown under the folder and as the title of the opened folder.",
             nullptr, label);
        size_t n = item->children.size();
        std::wstring count =
            n == 0 ? std::wstring(L"No apps yet.")
                   : std::to_wstring(n) + (n == 1 ? L" app." : L" apps.");
        auto open = TextButton(L"\uE8B7", L"Open folder", [this] {
            if (SelectedItem() && m_selSub < 0) {
                OpenFolder(m_selCat, m_selItem);
            }
        });
        card(L"\uE8B7", L"Apps",
             (count + L" Drop apps on the folder to add them, or open it "
                      L"(or click it) to reorder and edit them.")
                 .c_str(),
             open, nullptr);
        auto icon = textBox(item->icon, L"Empty = its apps' icons",
                            [](EditItem& it, std::wstring v) { it.icon = v; });
        card(L"\uE8B9", L"Icon",
             L"An .ico, .exe, .dll or image file, an image link, a Fluent "
             L"icon by its code (e.g. E8B7) or an emoji.",
             nullptr, withBrowse(icon, L"Icon", true));
        return;
    }

    // Type
    xc::ComboBox typeBox;
    typeBox.MinWidth(200);
    typeBox.Items().Append(Box(L"Start app"));
    typeBox.Items().Append(Box(L"Program"));
    typeBox.Items().Append(Box(L"Website"));
    typeBox.SelectedIndex(website ? 2 : program ? 1 : 0);
    typeBox.SelectionChanged([this, typeBox, edit](auto&&, auto&&) {
        int i = typeBox.SelectedIndex();
        std::wstring t = i == 2 ? L"website" : i == 1 ? L"program" : L"app";
        edit([t](EditItem& it) { it.type = t; });
        // Show the fields of the new type.
        Later([this] { BuildItemEditor(); });
    });
    card(L"\uE8FD", L"Type",
         L"A Start app is found by its name, a program by its path, and a "
         L"website opens in the default browser.",
         typeBox, nullptr);

    if (type == L"app") {
        // Start app, with suggestions from the apps in Start.
        xc::AutoSuggestBox app;
        app.PlaceholderText(L"Type to search the apps in Start");
        app.Text(item->app);
        app.QueryIcon(xc::SymbolIcon(xc::Symbol::Find));
        app.TextChanged([edit](xc::AutoSuggestBox const& sender,
                               xc::AutoSuggestBoxTextChangedEventArgs const&
                                   e) {
            std::wstring text(sender.Text());
            if (e.Reason() ==
                xc::AutoSuggestionBoxTextChangeReason::UserInput) {
                auto list = winrt::single_threaded_vector<wf::IInspectable>();
                if (!text.empty()) {
                    for (auto& a : g_pickerApps) {
                        if (StrStrIW(a.name.c_str(), text.c_str())) {
                            // Icon + name; the name rides along in Tag.
                            xc::StackPanel row;
                            row.Orientation(xc::Orientation::Horizontal);
                            row.Spacing(12);
                            auto icon = IconOrGlyph(a.iconSource, L"\uF0E2",
                                                    GetActiveWindow());
                            icon.VerticalAlignment(VerticalAlignment::Center);
                            row.Children().Append(icon);
                            xc::TextBlock name;
                            name.Text(a.name);
                            name.VerticalAlignment(VerticalAlignment::Center);
                            row.Children().Append(name);
                            row.Tag(Box(a.name));
                            list.Append(row);
                            if (list.Size() >= 12) {
                                break;
                            }
                        }
                    }
                }
                sender.ItemsSource(list);
            }
            edit([text](EditItem& it) { it.app = text; });
        });
        app.SuggestionChosen([](xc::AutoSuggestBox const& sender,
                                xc::AutoSuggestBoxSuggestionChosenEventArgs const&
                                    e) {
            if (auto row = e.SelectedItem().try_as<FrameworkElement>()) {
                sender.Text(winrt::unbox_value<winrt::hstring>(row.Tag()));
            }
        });
        // Suggestions are elements, not strings: set the text ourselves.
        app.UpdateTextOnSelect(false);
        card(L"\uF0E2", L"Start app",
             L"The app's name as Start shows it (case doesn't matter), or its "
             L"app ID (AUMID). Pick one from the suggestions.",
             nullptr, app);
    } else if (program) {
        auto path = textBox(item->path, L"C:\\Path\\To\\Program.exe",
                            [](EditItem& it, std::wstring v) { it.path = v; });
        card(L"\uE756", L"Program",
             L"Full path to a program, file or folder. Environment variables "
             L"such as %USERPROFILE% work.",
             nullptr, withBrowse(path, L"Program", false));
    } else {
        // A website: its name first, then its address.
        nameCard();
        auto url = textBox(item->path, L"https://example.com",
                           [](EditItem& it, std::wstring v) { it.path = v; });
        card(L"\uE774", L"Website",
             L"The address to open in the default browser.", nullptr,
             url);
    }

    if (!website) {
        nameCard();
    }

    // Arguments (not for a website)
    if (!website) {
        auto args = textBox(item->args, L"Optional",
                            [](EditItem& it, std::wstring v) { it.args = v; });
        card(L"\uE943", L"Arguments",
             program ? L"Command-line arguments passed to the program when "
                       L"it's launched."
                     : L"Arguments passed to the app when it's launched, if "
                       L"it accepts any.",
             nullptr, args);
    }

    // Icon
    auto icon = textBox(item->icon,
                        website ? L"Empty = a globe" : L"Empty = its own icon",
                        [](EditItem& it, std::wstring v) { it.icon = v; });
    card(L"\uE8B9", L"Icon",
         website ? L"An .ico, .png or other image file, an image link such "
                   L"as the website's favicon "
                   L"(https://example.com/favicon.ico), a Fluent icon by its "
                   L"code (e.g. E774) or an emoji."
                 : L"An .ico, .exe, .dll or image file, an image link, a "
                   L"Fluent icon by its code (e.g. E774) or an emoji.",
         nullptr, withBrowse(icon, L"Icon", true));
}

// --- Groups board ---------------------------------------------------------
// The groups drawn the way Start draws them, from the settings being
// edited, and edited with the mouse: drag apps to reorder them or move them
// to another group, drag a tile (by its background) to reorder the
// groups, drag a tile's corner to resize it, and drag Start apps from the
// side panel onto a tile to add them. Dragging uses pointer capture instead of
// XAML drag and drop, which relies on OLE drag and drop in the island's host.

struct SettingsWindow::BoardSurface {
    xc::Grid root{nullptr};       // drag coordinates are relative to it
    xc::Canvas overlay{nullptr};  // drag ghost and drop indicator
    struct Cell {
        FrameworkElement el{nullptr};
        int cat = -1;
        int index = -1;  // item index; for the overflow cluster, the first
        bool cluster = false;  // hidden item
        int sub = -1;          // an app in the folder `index` (opened folder)
        bool folder = false;   // the item is a folder (apps drop into it)
    };
    struct Tile {
        FrameworkElement el{nullptr};
        int cat = -1;
        FrameworkElement highlight{nullptr};  // accent border when selected
    };
    std::vector<Cell> cells;
    std::vector<Tile> tiles;
    // The box around each group's name (by group index), which the
    // rename text box replaces.
    std::map<int, xc::Border> labelHosts;
};

wf::Rect BoundsIn(FrameworkElement const& el, UIElement const& root) {
    try {
        return el.TransformToVisual(root).TransformBounds(
            {0, 0, (float)el.ActualWidth(), (float)el.ActualHeight()});
    } catch (...) {
        return {};
    }
}

bool RectContains(wf::Rect const& r, wf::Point const& p) {
    return r.Width > 0 && p.X >= r.X && p.X < r.X + r.Width && p.Y >= r.Y &&
           p.Y < r.Y + r.Height;
}

winrt::Windows::UI::Color AccentColor() {
    try {
        return winrt::Windows::UI::ViewManagement::UISettings().GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    } catch (...) {
        return {255, 0x00, 0x78, 0xD4};
    }
}

// Popups don't inherit the window's theme in an island.
void ThemeFlyout(xc::Flyout const& fly, bool dark) {
    Style style(winrt::xaml_typename<xc::FlyoutPresenter>());
    style.Setters().Append(Setter(FrameworkElement::RequestedThemeProperty(),
                                  winrt::box_value(dark ? ElementTheme::Dark
                                                        : ElementTheme::Light)));
    style.Setters().Append(
        Setter(FrameworkElement::MaxWidthProperty(), winrt::box_value(10000.0)));
    fly.FlyoutPresenterStyle(style);
}

void ThemeMenu(xc::MenuFlyout const& menu, bool dark) {
    Style style(winrt::xaml_typename<xc::MenuFlyoutPresenter>());
    style.Setters().Append(Setter(FrameworkElement::RequestedThemeProperty(),
                                  winrt::box_value(dark ? ElementTheme::Dark
                                                        : ElementTheme::Light)));
    menu.MenuFlyoutPresenterStyle(style);
}

// A subtle background while the pointer is over the element.
void AddHover(xc::Border const& b, bool dark) {
    WeakElement weak = b;
    b.PointerEntered([weak, dark](auto&&, auto&&) {
        if (auto x = Resolve<xc::Border>(weak)) {
            x.Background(dark ? Brush(0x15, 255, 255, 255)
                              : Brush(0x0C, 0, 0, 0));
        }
    });
    b.PointerExited([weak](auto&&, auto&&) {
        if (auto x = Resolve<xc::Border>(weak)) {
            x.Background(Brush(0, 0, 0, 0));
        }
    });
}

// Start apps on the groups board: a grid of icon + name cells.
constexpr double kPaletteCellWidth = 92;
constexpr double kPaletteCellHeight = 84;
constexpr double kBoardColumnGap = 24;  // between the board and the Apps list
// The Apps list's scroll bar, and its border and padding (4 + 1 a side).
constexpr double kAppsListChrome = 10;
constexpr double kAppsPanelSlack = 16 + kAppsListChrome;

double BoardLabelHeight(EditSettings const& s) {
    return s.labelsShow ? s.labelSpacing + s.fontSize * 1.4 : 0;
}

// Runs `fn` once the current event is done (e.g. after a pointer or menu
// event whose element `fn` removes). Queued with a window message: dispatcher
// callbacks aren't reliable in this XAML island.
void SettingsWindow::Later(std::function<void()> fn) {
    m_later.push_back(std::move(fn));
    if (m_later.size() == 1) {
        PostMessageW(m_hwnd, kRunLaterMsg, 0, 0);
    }
}

FrameworkElement SettingsWindow::BuildBoardPage() {
    m_board = std::make_shared<BoardSurface>();
    xc::Grid root;
    m_board->root = root;

    xc::StackPanel page;
    auto title = Text(L"Groups", 28);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    page.Children().Append(title);
    auto desc = Text(
        L"Your groups as Start shows them. Drag apps to reorder them or "
        L"to move them to another group, drag a group by its tile to "
        L"reorder the groups, and drag the corner of a tile to "
        L"resize it. Drag an app from the All Apps list onto a group to add "
        L"it. Double-click a name or the heading to rename it. "
        L"Click an app or a group to edit it below. Right-click "
        L"for more.",
        14, true);
    desc.Margin(ThicknessHelper::FromLengths(0, 4, 0, 16));
    page.Children().Append(desc);
    if (m_model.sortByName) {
        auto note = Text(
            L"Apps are sorted alphabetically (Behavior page), so Start "
            L"ignores their order within a group.",
            12, true);
        note.Margin(ThicknessHelper::FromLengths(0, -8, 0, 12));
        page.Children().Append(note);
    }


    xc::StackPanel bar;
    bar.Orientation(xc::Orientation::Horizontal);
    bar.Spacing(8);
    bar.Margin(ThicknessHelper::FromLengths(0, 0, 0, 12));
    bar.Children().Append(
        TextButton(L"", L"New group", [this] { AddGroup(); }));

    m_boardAddApp = AddAppButton();
    bar.Children().Append(m_boardAddApp);
    m_boardDup = TextButton(L"\uE8C8", L"Duplicate group",
                            [this] { DuplicateSelected(); });
    bar.Children().Append(m_boardDup);
    m_boardDel =
        TextButton(L"\uE74D", L"Delete group", [this] { DeleteSelected(); });
    bar.Children().Append(m_boardDel);
    page.Children().Append(bar);

    // The board, on a background like Start's.
    xc::Border boardCard;
    boardCard.Background(m_dark ? Brush(255, 0x2B, 0x2B, 0x2B)
                                : Brush(255, 0xEC, 0xEC, 0xEC));
    boardCard.BorderBrush(m_dark ? Brush(0x19, 0, 0, 0) : Brush(0x0F, 0, 0, 0));
    boardCard.BorderThickness(ThicknessHelper::FromUniformLength(1));
    boardCard.CornerRadius(CornerRadiusHelper::FromUniformRadius(8));
    boardCard.Padding(ThicknessHelper::FromLengths(24, 20, 24, 8));
    xc::StackPanel boardContent;
    m_boardHeadingHost = xc::Border();
    m_boardHeadingHost.HorizontalAlignment(HorizontalAlignment::Left);
    m_boardHeadingHost.Margin(ThicknessHelper::FromLengths(0, 0, 0, 16));
    boardContent.Children().Append(m_boardHeadingHost);
    UpdateBoardHeading();
    m_boardHost = xc::Border();
    // Tiles per row follow the board's width, as in Start: checked after every
    // layout pass (FitBoardRow returns early when nothing changed), since the
    // board can be built before the host has its final width.
    m_boardHost.LayoutUpdated([this](auto&&, auto&&) { FitBoardRow(); });
    // A click on the board's empty space (tiles handle their own clicks)
    // clears the selection: Add app then makes a group of its own.
    m_boardHost.Background(Brush(0, 0, 0, 0));  // to get the clicks
    m_boardHost.PointerPressed([this](auto&&,
                                      Input::PointerRoutedEventArgs const& e) {
        if (e.Handled() || m_drag.pressed) {
            return;
        }
        SelectBoardItem(-1, -1);
    });
    boardContent.Children().Append(m_boardHost);

    // On the right: all Start apps, searchable; drag one onto a group to
    // add it (see RefreshPalette).
    xc::Grid apps;
    apps.Width(2 * kPaletteCellWidth + kAppsPanelSlack);
    apps.RowSpacing(8);
    for (auto h : {GridLengthHelper::Auto(), GridLengthHelper::Auto(),
                   GridLengthHelper::Auto()}) {
        xc::RowDefinition rd;
        rd.Height(h);
        apps.RowDefinitions().Append(rd);
    }
    auto appsTitle = Text(L"All Apps", 14);
    appsTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    appsTitle.Margin(ThicknessHelper::FromLengths(0, 0, 0, 8));
    apps.Children().Append(appsTitle);
    m_paletteSearch = xc::TextBox();
    m_paletteSearch.PlaceholderText(L"Search apps");
    m_paletteSearch.TextChanged([this](auto&&, auto&&) { RefreshPalette(); });
    xc::Grid::SetRow(m_paletteSearch, 1);
    apps.Children().Append(m_paletteSearch);
    m_paletteList = xc::StackPanel();
    xc::ScrollViewer appsScroll;
    appsScroll.VerticalScrollBarVisibility(xc::ScrollBarVisibility::Auto);
    appsScroll.Content(m_paletteList);
    appsScroll.Height(320);
    // A subtle border around the list, like a card.
    xc::Border appsFrame;
    appsFrame.BorderThickness(ThicknessHelper::FromUniformLength(1));
    appsFrame.BorderBrush(m_dark ? Brush(0x30, 255, 255, 255)
                                 : Brush(0x30, 0, 0, 0));
    appsFrame.Background(m_dark ? Brush(0x08, 255, 255, 255)
                                : Brush(0x08, 0, 0, 0));
    appsFrame.CornerRadius(CornerRadiusHelper::FromUniformRadius(6));
    appsFrame.Padding(ThicknessHelper::FromUniformLength(4));
    appsFrame.Child(appsScroll);
    xc::Grid::SetRow(appsFrame, 2);
    apps.Children().Append(appsFrame);
    // As tall as the board next to it (at least a few rows of apps).
    winrt::weak_ref<xc::ScrollViewer> weakScroll = appsScroll;
    winrt::weak_ref<xc::TextBox> weakSearch = m_paletteSearch;
    boardContent.SizeChanged([weakScroll, weakSearch](auto&&,
                                                     winrt::Windows::UI::Xaml::SizeChangedEventArgs
                                                         const& e) {
        auto sv = weakScroll.get();
        auto search = weakSearch.get();
        if (!sv || !search) {
            return;
        }
        double used = 30 + 8 + search.ActualHeight() + 8 + kAppsListChrome;
        sv.Height(std::max(240.0, e.NewSize().Height - used));
    });

    xc::Grid boardColumns;
    boardColumns.ColumnSpacing(kBoardColumnGap);
    xc::ColumnDefinition bc0, bc1;
    // The board at its own width (smaller only if the window is too
    // narrow), the Apps list taking the rest.
    bc0.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    bc1.Width(GridLengthHelper::Auto());
    boardColumns.ColumnDefinitions().Append(bc0);
    boardColumns.ColumnDefinitions().Append(bc1);
    boardContent.VerticalAlignment(VerticalAlignment::Top);
    boardColumns.Children().Append(boardContent);
    apps.VerticalAlignment(VerticalAlignment::Top);
    xc::Grid::SetColumn(apps, 1);
    boardColumns.Children().Append(apps);
    boardCard.Child(boardColumns);
    m_boardCard = boardCard;
    m_appsPanel = apps;
    boardCard.SizeChanged([this](auto&&, auto&&) { LayoutAppsPanel(); });
    page.Children().Append(boardCard);

    // Configuration of the selected group and app, full width below.
    auto details = BuildBoardDetails();
    details.Margin(ThicknessHelper::FromLengths(0, 16, 0, 0));
    page.Children().Append(details);

    root.Children().Append(page);
    xc::Canvas overlay;
    overlay.IsHitTestVisible(false);
    m_board->overlay = overlay;
    root.Children().Append(overlay);
    HookSurface(m_board);

    // The group shown below is the selected one; give it its border.
    m_boardSelCat = m_selCat;
    RebuildBoard();
    RefreshPalette();
    LoadBoardDetails();
    return root;
}

// The section heading ("Groups"), renamed by double-clicking it.
void SettingsWindow::UpdateBoardHeading() {
    if (!m_boardHeadingHost) {
        return;
    }
    bool empty = m_model.sectionTitle.empty();
    auto heading =
        Text(empty ? L"No heading (double-click to add one)"
                   : m_model.sectionTitle,
             14, empty);
    heading.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    heading.TextWrapping(TextWrapping::NoWrap);
    heading.DoubleTapped([this](auto&&, auto&&) { EditBoardHeading(); });
    m_boardHeadingHost.Child(heading);
}

// The heading becomes a text box in its place. Enter or clicking away saves,
// Esc cancels.
void SettingsWindow::EditBoardHeading() {
    auto host = m_boardHeadingHost;
    if (!host || host.Child().try_as<xc::TextBox>()) {
        return;
    }
    xc::TextBox box;
    box.Text(m_model.sectionTitle);
    box.PlaceholderText(L"No heading");
    box.FontSize(14);
    box.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    box.MinWidth(220);
    // Strong references, dropped when done (breaks the handler cycle).
    auto state = std::make_shared<std::pair<xc::TextBox, bool>>(box, false);
    ULONGLONG shown = GetTickCount64();
    auto finish = [this, state](bool save) {
        auto& [b, done] = *state;
        if (done || !b) {
            return;
        }
        done = true;
        std::wstring v(b.Text());
        b = nullptr;
        if (save && v != m_model.sectionTitle) {
            m_model.sectionTitle = v;
            OnChanged();
        }
        // Not from within the box's own event.
        Later([this] { UpdateBoardHeading(); });
    };
    box.KeyDown([finish](auto&&, Input::KeyRoutedEventArgs const& e) {
        if (e.Key() == winrt::Windows::System::VirtualKey::Enter) {
            e.Handled(true);
            finish(true);
        } else if (e.Key() == winrt::Windows::System::VirtualKey::Escape) {
            e.Handled(true);
            finish(false);
        }
    });
    box.LostFocus([finish, state, shown](auto&&, auto&&) {
        // The double click that started the edit can take the focus right
        // away; take it back instead of ending the edit.
        if (GetTickCount64() - shown < 400) {
            if (auto& b = state->first) {
                b.Focus(FocusState::Programmatic);
            }
            return;
        }
        finish(true);
    });
    box.Loaded([state](auto&&, auto&&) {
        if (auto& b = state->first) {
            b.Focus(FocusState::Programmatic);
            b.SelectAll();
        }
    });
    host.Child(box);
}

// Swaps the text in `host` for a text box. Enter or leaving the box saves,
// Esc cancels.
void SettingsWindow::BeginInlineEdit(FrameworkElement const& anchor,
                                     std::wstring const& value,
                                     double fontSize,
                                     std::function<void(std::wstring)> commit) {
    // A text box floating above the name; the name itself stays as it is.
    // Enter or clicking away saves, Esc cancels.
    if (!anchor) {
        return;
    }
    xc::Flyout fly;
    ThemeFlyout(fly, m_dark);
    fly.Placement(xcp::FlyoutPlacementMode::Top);
    xc::TextBox box;
    box.Text(value);
    box.FontSize(std::max(fontSize, 14.0));
    box.MinWidth(220);
    auto canceled = std::make_shared<bool>(false);
    auto holder = std::make_shared<xc::Flyout>(fly);
    // PreviewKeyDown: the flyout would take Esc for itself.
    box.PreviewKeyDown([holder, canceled](auto&&,
                                          Input::KeyRoutedEventArgs const& e) {
        auto key = e.Key();
        if (key == winrt::Windows::System::VirtualKey::Enter ||
            key == winrt::Windows::System::VirtualKey::Escape) {
            e.Handled(true);
            *canceled = key == winrt::Windows::System::VirtualKey::Escape;
            if (*holder) {
                holder->Hide();
            }
        }
    });
    fly.Opened([box](auto&&, auto&&) {
        box.Focus(FocusState::Programmatic);
        box.SelectAll();
    });
    fly.Closed([this, box, canceled, commit, holder](auto&&, auto&&) {
        *holder = nullptr;  // no longer needed, breaks the reference cycle
        if (!*canceled) {
            std::wstring v(box.Text());
            Later([commit, v] { commit(v); });
        }
    });
    fly.Content(box);
    fly.ShowAt(anchor);
}

void SettingsWindow::RebuildBoard() {
    if (!m_board || !m_boardHost) {
        return;
    }
    m_board->cells.clear();
    m_board->labelHosts.clear();
    m_board->tiles.clear();
    auto const& s = m_model;
    if (s.groups.empty()) {
        auto empty =
            Text(L"No groups yet. Click New group to add one.", 14, true);
        empty.Margin(ThicknessHelper::FromLengths(0, 0, 0, 16));
        m_boardHost.Child(empty);
        return;
    }
    // Rows as in Start: for the width Start last had, or a default-width
    // Start menu's until it has been opened with the mod.
    m_startRowWidth = Wh_GetIntValue(kStartRowWidthValue, 0);
    if (m_startRowWidth <= 0) {
        m_startRowWidth = kReferenceRowWidth;
    }
    // Fitted to Start's width, as in Start (see RebuildHostContent).
    const RowFit row = BoardRowFit();
    const int halves = row.halves;
    m_boardHalves = halves;
    m_boardTile = row.tile;
    m_boardHalf = row.half;
    xc::VariableSizedWrapGrid wrap;
    wrap.Orientation(xc::Orientation::Horizontal);
    wrap.ItemWidth(row.half);  // half cells, as in Start
    wrap.ItemHeight((s.tileSize + BoardLabelHeight(s) + s.gapY) /
                    2);  // half rows, as in Start
    wrap.MaximumRowsOrColumns(halves);
    wrap.HorizontalAlignment(HorizontalAlignment::Left);
    wrap.Margin(ThicknessHelper::FromLengths(0, 0, -(s.gapX + kRowSlack), 0));
    for (int ci = 0; ci < (int)s.groups.size(); ci++) {
        auto tile = BuildBoardTile(ci, m_board);
        auto lay = BoardLayoutOf(s.groups[ci]);
        if (lay.spanHalfColumns > 1) {
            xc::VariableSizedWrapGrid::SetColumnSpan(tile,
                                                     lay.spanHalfColumns);
        }
        if (lay.spanHalfRows > 1) {
            xc::VariableSizedWrapGrid::SetRowSpan(tile, lay.spanHalfRows);
        }
        wrap.Children().Append(tile);
    }
    // Same layout as in Start (see RebuildHostContent), shrunk to fit.
    xc::Border frame;
    frame.Width(BoardRowWidth(row));
    frame.HorizontalAlignment(HorizontalAlignment::Left);
    frame.Child(wrap);
    m_boardWrap = wrap;
    m_boardFrame = frame;
    // A dashed outline of Start's width around the tiles: what doesn't fit
    // in it, Start shrinks to fit.
    constexpr double kOutlinePad = 8;
    frame.Margin(ThicknessHelper::FromUniformLength(kOutlinePad));
    winrt::Windows::UI::Xaml::Shapes::Rectangle outline;
    // Around the tiles, the same padding on both sides; at least Start's
    // width (the area grows with a row that's wider).
    outline.HorizontalAlignment(HorizontalAlignment::Stretch);
    outline.VerticalAlignment(VerticalAlignment::Stretch);
    outline.Stroke(m_dark ? Brush(0x60, 255, 255, 255) : Brush(0x60, 0, 0, 0));
    outline.StrokeThickness(1);
    xm::DoubleCollection dashes;
    dashes.Append(4);
    dashes.Append(3);
    outline.StrokeDashArray(dashes);
    outline.RadiusX(6);
    outline.RadiusY(6);
    outline.IsHitTestVisible(false);
    xc::ToolTipService::SetToolTip(outline, Box(L"Start's width"));
    m_startOutline = outline;
    xc::Grid area;
    area.HorizontalAlignment(HorizontalAlignment::Left);
    area.MinWidth(m_startRowWidth + 2 * kOutlinePad);
    m_boardNaturalWidth =
        std::max(m_startRowWidth, BoardRowWidth(row)) +
        2 * kOutlinePad;
    LayoutAppsPanel();
    area.Children().Append(outline);
    area.Children().Append(frame);
    xc::Viewbox fit;
    fit.Stretch(xm::Stretch::Uniform);
    fit.StretchDirection(xc::StretchDirection::DownOnly);
    fit.HorizontalAlignment(HorizontalAlignment::Left);
    fit.Child(area);
    m_boardHost.Child(fit);
    // The details panel follows: a new selection reloads it, otherwise just
    // the group fields (names and sizes can change on the board).
    if (m_selCat >= (int)m_model.groups.size()) {
        m_selCat = (int)m_model.groups.size() - 1;
    }
    if (m_selItem >= 0 && !SelectedItem()) {
        m_selItem = -1;
        m_selSub = -1;
    }
    if (m_selCat != m_detailsCat || m_selItem != m_detailsItem ||
        m_selSub != m_detailsSub) {
        LoadBoardDetails();
    } else {
        LoadBoardGroupFields();
    }
}

void SettingsWindow::FitBoardRow() {
    auto wrap = m_boardWrap.get();
    if (!wrap || !m_boardHost) {
        return;
    }
    // The tiles are built for the row's fit (BuildBoard): built again when
    // a change of the settings changes it. Not from inside this layout pass.
    RowFit fit = BoardRowFit();
    if (fit.halves != m_boardHalves ||
        std::abs(fit.tile - m_boardTile) > 0.25) {
        m_boardHalves = fit.halves;
        m_boardTile = fit.tile;
        Later([this] { RebuildBoard(); });
    }
}

// Sizes the Apps list to the room the board leaves in its card: as many
// columns of apps as fit, at least one.
void SettingsWindow::LayoutAppsPanel() {
    auto card = m_boardCard.get();
    auto apps = m_appsPanel.get();
    if (!card || !apps || card.ActualWidth() <= 0) {
        return;
    }
    auto pad = card.Padding();
    double room = card.ActualWidth() - pad.Left - pad.Right -
                  kBoardColumnGap - m_boardNaturalWidth - kAppsPanelSlack;
    int columns = std::max(1, (int)std::floor(room / kPaletteCellWidth));
    double width = columns * kPaletteCellWidth + kAppsPanelSlack;
    if (std::abs(apps.Width() - width) > 0.5) {
        apps.Width(width);
    }
}

// A tooltip with `text` on `el` only while `name` is cut short, as in Start
// (see SetAppTooltip); without a name shown, always.
void TooltipWhenTrimmed(FrameworkElement const& el,
                        xc::TextBlock const& name,
                        std::wstring const& text) {
    if (!name) {
        xc::ToolTipService::SetToolTip(el, winrt::box_value(text));
        return;
    }
    winrt::weak_ref<FrameworkElement> weakEl = el;
    name.IsTextTrimmedChanged([weakEl, text](xc::TextBlock const& tb, auto&&) {
        if (auto e = weakEl.get()) {
            xc::ToolTipService::SetToolTip(
                e, tb.IsTextTrimmed() ? winrt::box_value(text) : nullptr);
        }
    });
}

FrameworkElement SettingsWindow::BuildBoardApp(EditItem const& item,
                                               double iconSize,
                                               bool showName,
                                               double nameGap) {
    auto icon =
        ItemIconEl(item, m_hwnd, iconSize);
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    icon.VerticalAlignment(VerticalAlignment::Center);
    if (!showName) {
        return icon;
    }
    xc::StackPanel sp;
    sp.Spacing(nameGap);
    sp.VerticalAlignment(VerticalAlignment::Center);
    sp.Children().Append(icon);
    xc::TextBlock name;
    name.Text(ItemTitle(item));
    name.FontSize(AppNameFontSize(m_model.iconSize));
    name.TextAlignment(TextAlignment::Center);
    name.TextTrimming(TextTrimming::CharacterEllipsis);
    name.TextWrapping(TextWrapping::NoWrap);
    sp.Children().Append(name);
    return sp;
}

FrameworkElement SettingsWindow::BuildBoardTile(
    int ci,
    std::shared_ptr<BoardSurface> const& surf) {
    auto const& s = m_model;
    auto const& cat = s.groups[ci];
    auto lay = BoardLayoutOf(cat);
    double labelH = BoardLabelHeight(s);
    bool nameShown = s.labelsShow && ShowsLabel(cat);
    bool background = ShowsBackground(cat);
    TileBox box = TileBoxFor(lay.spanHalfColumns, lay.spanHalfRows,
                             m_boardTile > 0 ? m_boardTile : s.tileSize,
                             s.tileSize, s.gapX, s.gapY, labelH, nameShown);
    double tileW = box.width;
    double tileH = box.height;
    TileFit tf = FitTile(s.iconSize, tileW, tileH,
                         background ? s.padding : 0, s.iconGap, lay,
                         s.appPadX, s.appPadY,
                         s.showAppLabels && cat.showAppNames);
    double padX = tf.padX;
    bool showNames = s.showAppLabels && cat.showAppNames;
    double iconSize = tf.iconSize;

    xc::StackPanel outer;
    outer.Spacing(s.labelSpacing);
    outer.Width(tileW);
    outer.HorizontalAlignment(HorizontalAlignment::Left);
    outer.VerticalAlignment(VerticalAlignment::Top);

    xc::Grid tileGrid;
    tileGrid.Width(tileW);
    tileGrid.Height(tileH);
    xc::Border tile;
    // Without a background, the rounding of the apps inside, so the tile
    // doesn't cut their (selection) border at the corners.
    double tileRadius = background ? s.cornerRadius : 4;
    tile.CornerRadius(CornerRadiusHelper::FromUniformRadius(tileRadius));
    tile.Padding(ThicknessHelper::FromLengths(padX, tf.padY, padX, tf.padY));
    if (background) {
        tile.BorderThickness(
            ThicknessHelper::FromUniformLength(s.borderThickness));
        tile.Background(MakeBrush(
            s.background, m_dark ? winrt::Windows::UI::Color{0x0D, 255, 255, 255}
                                 : winrt::Windows::UI::Color{0xB3, 255, 255, 255}));
        tile.BorderBrush(MakeBrush(
            s.border, m_dark ? winrt::Windows::UI::Color{0x0F, 255, 255, 255}
                             : winrt::Windows::UI::Color{0x0F, 0, 0, 0}));
    } else {
        // Transparent, not null: it can still be dragged by its empty area.
        tile.Background(Brush(0, 0, 0, 0));
    }

    xc::Grid grid;
    grid.RowSpacing(s.iconGap);
    grid.ColumnSpacing(s.iconGap);
    for (int c = 0; c < lay.iconColumns; c++) {
        xc::ColumnDefinition cd;
        cd.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        grid.ColumnDefinitions().Append(cd);
    }
    for (int r = 0; r < lay.iconRows; r++) {
        xc::RowDefinition rd;
        rd.Height(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        grid.RowDefinitions().Append(rd);
    }
    int cells = lay.iconColumns * lay.iconRows;
    int count = (int)cat.items.size();
    bool overflow = s.overflowCluster && count > cells && cells > 1;
    int singles = overflow ? cells - 1 : std::min(count, cells);
    m_overflowFirst[ci] = overflow ? singles : 0;

    for (int i = 0; i < singles; i++) {
        auto const& item = cat.items[i];
        xc::Border cellEl;
        cellEl.Background(Brush(0, 0, 0, 0));
        cellEl.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
        cellEl.Padding(ThicknessHelper::FromLengths(s.appPadX, tf.appPadY,
                                                    s.appPadX, tf.appPadY));
        cellEl.Child(BuildBoardApp(item, iconSize, showNames, tf.nameGap));
        // Always 1 px (thinner than a tile's) and transparent unless
        // selected, so selecting an app doesn't shift it.
        cellEl.BorderBrush(ci == m_selCat && i == m_selItem && m_selSub < 0
                               ? xm::SolidColorBrush(AccentColor())
                               : Brush(0, 0, 0, 0));
        cellEl.BorderThickness(ThicknessHelper::FromUniformLength(1));
        bool folder = item.type == L"folder";
        {
            // The name under the icon, if it's shown (BuildBoardApp).
            xc::TextBlock nameText{nullptr};
            if (auto sp = cellEl.Child().try_as<xc::StackPanel>()) {
                for (auto c : sp.Children()) {
                    if (auto tb = c.try_as<xc::TextBlock>()) {
                        nameText = tb;
                    }
                }
            }
            TooltipWhenTrimmed(cellEl, nameText, ItemTitle(item));
        }
        cellEl.ContextFlyout(AppMenu(ci, i));
        AddHover(cellEl, m_dark);
        // A single app without a background fills its whole tile, which
        // then has nothing else to drag it by: dragging the app moves the
        // group (a click still selects the app).
        bool single = !background && cat.items.size() == 1;
        std::function<void()> open;
        if (folder) {
            open = [this, ci, i] { OpenFolder(ci, i); };
        }
        MakeDraggable(cellEl, surf, single ? DragKind::Group : DragKind::App,
                      ci, i, L"", open);
        xc::Grid::SetRow(cellEl, i / lay.iconColumns);
        xc::Grid::SetColumn(cellEl, i % lay.iconColumns);
        grid.Children().Append(cellEl);
        surf->cells.push_back({cellEl, ci, i, false, -1, folder});
    }

    if (overflow) {
        auto more = MakeFlatButton();
        more.Padding(ThicknessHelper::FromLengths(s.appPadX, tf.appPadY,
                                                  s.appPadX, tf.appPadY));
        xc::Grid mini;
        double small = std::floor((iconSize - 4) / 2.0);
        mini.RowSpacing(4);
        mini.ColumnSpacing(4);
        mini.HorizontalAlignment(HorizontalAlignment::Center);
        mini.VerticalAlignment(VerticalAlignment::Center);
        for (int k = 0; k < 2; k++) {
            xc::ColumnDefinition cd;
            cd.Width(GridLengthHelper::Auto());
            mini.ColumnDefinitions().Append(cd);
            xc::RowDefinition rd;
            rd.Height(GridLengthHelper::Auto());
            mini.RowDefinitions().Append(rd);
        }
        for (int k = 0; k < 4 && singles + k < count; k++) {
            auto const& it = cat.items[singles + k];
            auto img = ItemIconEl(it, m_hwnd, small);
            xc::Grid::SetRow(img, k / 2);
            xc::Grid::SetColumn(img, k % 2);
            mini.Children().Append(img);
        }
        more.Content(mini);
        xc::ToolTipService::SetToolTip(
            more, Box(L"+" + std::to_wstring(count - singles) +
                      L" more. Click to see them and reorder them."));
        WeakElement weakMore = more;
        more.Click([this, ci, weakMore](auto&&, auto&&) {
            if (auto b = weakMore.get()) {
                ShowOverflowFlyout(ci, b);
            }
        });
        xc::Grid::SetRow(more, (cells - 1) / lay.iconColumns);
        xc::Grid::SetColumn(more, (cells - 1) % lay.iconColumns);
        grid.Children().Append(more);
        surf->cells.push_back({more, ci, singles, true});
    }

    if (count == 0) {
        auto hint = Text(L"Drop apps here", 12, true);
        hint.TextAlignment(TextAlignment::Center);
        hint.HorizontalAlignment(HorizontalAlignment::Center);
        hint.VerticalAlignment(VerticalAlignment::Center);
        hint.IsHitTestVisible(false);
        xc::Grid::SetColumnSpan(hint, lay.iconColumns);
        xc::Grid::SetRowSpan(hint, lay.iconRows);
        grid.Children().Append(hint);
    }
    tile.Child(grid);
    tileGrid.Children().Append(tile);
    MakeDraggable(tile, surf, DragKind::Group, ci, -1);

    // Resize handle in the bottom-right corner.
    xc::Border handle;
    handle.Width(22);
    handle.Height(22);
    handle.HorizontalAlignment(HorizontalAlignment::Right);
    handle.VerticalAlignment(VerticalAlignment::Bottom);
    handle.Background(Brush(0, 0, 0, 0));
    // A rounded corner stroke following the tile's corner, like the resize
    // handle of Windows 11 widgets: down the right edge, around the corner,
    // along the bottom.
    {
        namespace xs = winrt::Windows::UI::Xaml::Shapes;
        const float edge = 17, r = 6, run = 1.5f;  // in the 22 x 22 handle
        xm::PathFigure fig;
        fig.StartPoint({edge, edge - r - run});
        fig.IsFilled(false);
        xm::LineSegment down;
        down.Point({edge, edge - r});
        fig.Segments().Append(down);
        xm::ArcSegment arc;
        arc.Point({edge - r, edge});
        arc.Size({r, r});
        arc.SweepDirection(xm::SweepDirection::Clockwise);
        fig.Segments().Append(arc);
        xm::LineSegment along;
        along.Point({edge - r - run, edge});
        fig.Segments().Append(along);
        xm::PathGeometry geo;
        geo.Figures().Append(fig);
        xs::Path grip;
        grip.Data(geo);
        grip.Stroke(m_dark ? Brush(0x99, 255, 255, 255) : Brush(0x80, 0, 0, 0));
        grip.StrokeThickness(2.5);
        grip.StrokeStartLineCap(xm::PenLineCap::Round);
        grip.StrokeEndLineCap(xm::PenLineCap::Round);
        handle.Child(grip);
    }
    xc::ToolTipService::SetToolTip(handle, Box(L"Drag to resize"));
    MakeDraggable(handle, surf, DragKind::Resize, ci, -1);
    if (!IsSingleApp(cat)) {  // an app on its own has no size of its own
        tileGrid.Children().Append(handle);
    }

    // Accent border of the selected (or resized) group.
    xc::Border highlight;
    highlight.BorderBrush(xm::SolidColorBrush(AccentColor()));
    highlight.BorderThickness(ThicknessHelper::FromUniformLength(2));
    highlight.CornerRadius(
        CornerRadiusHelper::FromUniformRadius(std::max(tileRadius, 2.0)));
    highlight.IsHitTestVisible(false);
    highlight.Visibility(ci == m_boardSelCat && m_selItem < 0
                             ? Visibility::Visible
                             : Visibility::Collapsed);
    tileGrid.Children().Append(highlight);

    tileGrid.ContextFlyout(TileMenu(ci, tileGrid));
    surf->tiles.push_back({tileGrid, ci, highlight});
    outer.Children().Append(tileGrid);

    if (nameShown) {
        xc::TextBlock label;
        label.Text(cat.name);
        label.FontSize(s.fontSize);
        label.TextAlignment(TextAlignment::Center);
        label.HorizontalAlignment(HorizontalAlignment::Center);
        label.TextTrimming(TextTrimming::CharacterEllipsis);
        winrt::Windows::UI::Color lc;
        if (ParseColor(s.labelColor, lc)) {
            label.Foreground(xm::SolidColorBrush(lc));
        }
        label.ContextFlyout(TileMenu(ci, tileGrid));
        xc::Border labelHost;
        labelHost.HorizontalAlignment(HorizontalAlignment::Center);
        // Transparent, not null: the whole box takes clicks, not just the
        // glyphs of the text.
        labelHost.Background(Brush(0, 0, 0, 0));
        labelHost.Child(label);
        surf->labelHosts[ci] = labelHost;
        // A click on a group's name isn't one on the empty board (which
        // clears the selection).
        labelHost.PointerPressed(
            [](auto&&, Input::PointerRoutedEventArgs const& e) { e.Handled(true); });
        double fontSize = s.fontSize;
        // Not draggable (the tile is). A double click is detected here from
        // two releases, rather than relying on DoubleTapped.
        labelHost.PointerReleased(
            [this, ci, fontSize](auto&&, Input::PointerRoutedEventArgs const&) {
                ULONGLONG now = GetTickCount64();
                bool second = m_labelClickCat == ci &&
                              now - m_labelClickTime <= GetDoubleClickTime();
                m_labelClickCat = second ? -1 : ci;
                m_labelClickTime = now;
                if (!second || m_drag.active || !m_board ||
                    ci >= (int)m_model.groups.size()) {
                    return;
                }
                auto it = m_board->labelHosts.find(ci);
                if (it == m_board->labelHosts.end()) {
                    return;
                }
                BeginInlineEdit(it->second, m_model.groups[ci].name,
                                fontSize, [this, ci](std::wstring v) {
                                    if (ci < (int)m_model.groups.size() &&
                                        !v.empty() &&
                                        v != m_model.groups[ci].name) {
                                        m_model.groups[ci].name = v;
                                        OnChanged();
                                    }
                                    RebuildBoard();
                                    RefreshPalette();
                                });
            });
        outer.Children().Append(labelHost);
    }
    return outer;
}

void SettingsWindow::ShowOverflowFlyout(int ci,
                                        FrameworkElement const& anchor,
                                        int folder) {
    if (ci < 0 || ci >= (int)m_model.groups.size()) {
        return;
    }
    auto const& items = m_model.groups[ci].items;
    if (folder >= (int)items.size() ||
        (folder >= 0 && items[folder].type != L"folder")) {
        return;
    }
    // One at a time: a folder opened from the opened group replaces it.
    if (auto open = m_overflowFlyout.get()) {
        open.Hide();
    }
    // Where it grows out of: the cell it's opened from.
    const wf::Rect origin = RootRect(anchor);
    xc::Flyout fly;
    ThemeFlyout(fly, m_dark);
    if (auto style = fly.FlyoutPresenterStyle()) {
        style.Setters().Append(Setter(
            xc::Control::PaddingProperty(),
            winrt::box_value(ThicknessHelper::FromUniformLength(kFlyoutPad))));
    }
    auto surf = std::make_shared<BoardSurface>();
    xc::Grid root;
    root.RequestedTheme(m_dark ? ElementTheme::Dark : ElementTheme::Light);
    // As wide as its apps' grid, like in Start.
    const int cols = std::clamp(m_model.expandedColumns, kMinExpandedColumns,
                                kMaxExpandedColumns);
    const FlyoutLook look = FlyoutLookFor(m_model.iconSize);
    root.Width(cols * look.cell + (cols - 1) * kFlyoutGap);
    surf->root = root;
    xc::StackPanel content;
    content.Spacing(8);
    auto title = Text(folder >= 0 ? ItemTitle(items[folder])
                                  : m_model.groups[ci].name,
                      20);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    title.HorizontalAlignment(HorizontalAlignment::Center);
    title.TextAlignment(TextAlignment::Center);
    title.TextTrimming(TextTrimming::CharacterEllipsis);
    content.Children().Append(title);
    content.Children().Append(
        Text(folder >= 0
                 ? L"Drag the apps to reorder them, or out of the folder to "
                   L"move them, and drop apps on the folder to add them. "
                   L"Click one to edit it below, right-click for more."
                 : L"The apps that don't fit in the tile. Drag them to "
                   L"reorder them. Click one to edit it below, right-click "
                   L"for more.",
             12, true));
    m_flyoutHost = xc::Border();
    content.Children().Append(m_flyoutHost);
    root.Children().Append(content);
    xc::Canvas overlay;
    overlay.IsHitTestVisible(false);
    surf->overlay = overlay;
    root.Children().Append(overlay);
    m_flyoutSurface = surf;
    m_flyoutCat = ci;
    m_flyoutFolder = folder;
    m_overflowFlyout = fly;
    HookSurface(surf);
    RebuildFlyout();
    fly.Content(root);
    fly.Closed([this, weakFly = winrt::make_weak(fly)](auto&&, auto&&) {
        // Not when another one has opened in its place since.
        auto me = weakFly.get();
        auto current = m_overflowFlyout.get();
        if (me && current && me != current) {
            return;
        }
        m_flyoutSurface = nullptr;
        m_flyoutHost = nullptr;
        m_flyoutCat = -1;
        m_flyoutFolder = -1;
        // Show the new order in the tile.
        Later([this] { RebuildBoard(); });
    });
    // Centered over the board, like Start's folders over Start.
    wf::Rect area{0, 0, 0, 0};
    int depth = 0;
    for (DependencyObject p = m_boardHost; p && depth < 12;
         p = xm::VisualTreeHelper::GetParent(p), depth++) {
        if (auto sv = p.try_as<xc::ScrollViewer>()) {
            area = RootRect(sv);
            break;
        }
    }
    if (area.Width <= 0 && m_boardHost) {
        area = RootRect(m_boardHost);
    }
    wf::Size size{0, 0};
    try {
        root.Measure({(float)root.Width(), INFINITY});
        size = {(float)(root.Width() + 2 * kFlyoutPad + 2),
                root.DesiredSize().Height + (float)(2 * kFlyoutPad + 2)};
    } catch (...) {
    }
    ShowCenteredOver(fly, anchor, area, size, origin);
}

// Opens a folder on the board (from the cell showing it, or from the
// overflow cluster it's hidden in).
void SettingsWindow::OpenFolder(int ci, int fi) {
    if (!m_board) {
        return;
    }
    FrameworkElement anchor{nullptr};
    for (auto const& c : m_board->cells) {
        if (c.cat != ci) {
            continue;
        }
        if ((c.index == fi && !c.cluster) ||
            (c.cluster && c.index <= fi && !anchor)) {
            anchor = c.el;
        }
    }
    for (auto const& t : m_board->tiles) {
        if (!anchor && t.cat == ci) {
            anchor = t.el;
        }
    }
    if (anchor) {
        ShowOverflowFlyout(ci, anchor, fi);
    }
}

// The opened group, like Start's expanded view: every app, in order. Or
// the opened folder: its apps.
void SettingsWindow::RebuildFlyout() {
    auto surf = m_flyoutSurface;
    if (!surf || !m_flyoutHost || m_flyoutCat < 0 ||
        m_flyoutCat >= (int)m_model.groups.size()) {
        return;
    }
    surf->cells.clear();
    surf->tiles.clear();
    int ci = m_flyoutCat;
    int fi = m_flyoutFolder;
    auto const& group = m_model.groups[ci];
    if (fi >= (int)group.items.size() ||
        (fi >= 0 && group.items[fi].type != L"folder")) {
        // The folder is gone (removed, or moved elsewhere).
        if (auto f = m_overflowFlyout.get()) {
            f.Hide();
        }
        return;
    }
    auto const& list = fi >= 0 ? group.items[fi].children : group.items;
    // A group: the apps in its overflow cluster (all, if it has none).
    int first = 0;
    if (fi < 0) {
        if (auto it = m_overflowFirst.find(ci); it != m_overflowFirst.end()) {
            first = std::clamp(it->second, 0, (int)list.size());
        }
    }
    const int shown = (int)list.size() - first;
    int cols = std::clamp(m_model.expandedColumns, kMinExpandedColumns,
                          kMaxExpandedColumns);
    const FlyoutLook look = FlyoutLookFor(m_model.iconSize);  // as in Start
    xc::Grid grid;
    grid.ColumnSpacing(4);
    grid.RowSpacing(4);
    for (int c = 0; c < cols; c++) {
        xc::ColumnDefinition cd;
        cd.Width(GridLengthHelper::FromPixels(look.cell));
        grid.ColumnDefinitions().Append(cd);
    }
    int rows = (shown + cols - 1) / cols;
    for (int r = 0; r < rows; r++) {
        xc::RowDefinition rd;
        rd.Height(GridLengthHelper::Auto());
        grid.RowDefinitions().Append(rd);
    }
    for (int i = first; i < (int)list.size(); i++) {
        auto const& item = list[i];
        xc::Border cellEl;
        cellEl.Background(Brush(0, 0, 0, 0));
        cellEl.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
        cellEl.Padding(ThicknessHelper::FromLengths(4, 10, 4, 8));
        xc::StackPanel sp;
        sp.Spacing(6);
        auto icon = ItemIconEl(item, m_hwnd, look.icon);
        icon.HorizontalAlignment(HorizontalAlignment::Center);
        sp.Children().Append(icon);
        xc::TextBlock name;
        name.Text(ItemTitle(item));
        name.FontSize(look.font);
        name.TextAlignment(TextAlignment::Center);
        name.TextWrapping(TextWrapping::Wrap);
        name.MaxLines(2);
        name.TextTrimming(TextTrimming::CharacterEllipsis);
        name.Width(look.textWidth);
        sp.Children().Append(name);
        cellEl.Child(sp);
        TooltipWhenTrimmed(cellEl, name, ItemTitle(item));
        // Selected like on the board: a 1 px border, transparent otherwise.
        cellEl.BorderThickness(ThicknessHelper::FromUniformLength(1));
        bool selected = ci == m_selCat &&
                        (fi >= 0 ? fi == m_selItem && i == m_selSub
                                 : i == m_selItem && m_selSub < 0);
        cellEl.BorderBrush(selected ? xm::SolidColorBrush(AccentColor())
                                    : Brush(0, 0, 0, 0));
        cellEl.ContextFlyout(fi >= 0 ? FolderAppMenu(ci, fi, i)
                                     : AppMenu(ci, i));
        AddHover(cellEl, m_dark);
        bool folder = fi < 0 && item.type == L"folder";
        std::function<void()> open;
        if (folder) {
            open = [this, ci, i] { OpenFolder(ci, i); };
        }
        if (fi >= 0) {
            MakeDraggable(cellEl, surf, DragKind::App, ci, fi, L"", nullptr, i);
        } else {
            MakeDraggable(cellEl, surf, DragKind::App, ci, i, L"", open);
        }
        xc::Grid::SetRow(cellEl, (i - first) / cols);
        xc::Grid::SetColumn(cellEl, (i - first) % cols);
        grid.Children().Append(cellEl);
        surf->cells.push_back(
            {cellEl, ci, fi >= 0 ? fi : i, false, fi >= 0 ? i : -1, folder});
    }
    surf->tiles.push_back({grid, ci, nullptr});
    xc::ScrollViewer sv;
    sv.MaxHeight(480);
    // A fixed size like Start's: room for kFlyoutMinRows rows of apps.
    double rowH = look.cell;
    try {
        grid.Measure({INFINITY, INFINITY});
        if (rows > 0 && grid.DesiredSize().Height > 0) {
            rowH = (grid.DesiredSize().Height - (rows - 1) * 4) / rows;
        }
    } catch (...) {
    }
    sv.MinHeight(std::min(480.0, kFlyoutMinRows * rowH +
                                     (kFlyoutMinRows - 1) * 4.0));
    sv.VerticalScrollBarVisibility(xc::ScrollBarVisibility::Auto);
    sv.Content(grid);
    m_flyoutHost.Child(sv);
}

// The All Apps list. Edits to the groups only change the groups an app's
// tooltip names: those update the cells in place, since building the list
// again would reload every icon (and flicker). It's built again only for a
// new search or a new list of apps.
void SettingsWindow::RefreshPalette() {
    if (!m_paletteList || !m_board) {
        return;
    }
    std::wstring query =
        m_paletteSearch ? std::wstring(m_paletteSearch.Text()) : L"";
    auto lower = [](std::wstring v) {
        CharLowerBuffW(v.data(), (DWORD)v.size());
        return v;
    };
    // The group each Start app is in already.
    std::unordered_map<std::wstring, std::wstring> placed;
    for (auto const& c : m_model.groups) {
        for (auto const& it : c.items) {
            if (it.type == L"app" && !it.app.empty()) {
                placed.emplace(lower(it.app), c.name);
            }
            for (auto const& child : it.children) {
                if (child.type == L"app" && !child.app.empty()) {
                    placed.emplace(lower(child.app),
                                   c.name + L" \u203A " + ItemTitle(it));
                }
            }
        }
    }
    auto groupOf = [&](AppEntry const& a) -> std::wstring const* {
        auto where = placed.find(lower(a.name));
        if (where == placed.end()) {
            where = placed.find(lower(a.id));
        }
        return where != placed.end() ? &where->second : nullptr;
    };
    auto mark = [](xc::Border const& cell, xc::TextBlock const& name,
                   AppEntry const& a, std::wstring const* group) {
        xc::ToolTipService::SetToolTip(
            cell, Box(a.name + (group ? L"\nIn " + *group : std::wstring()) +
                      L"\nDrag onto a group to add it"));
    };

    if (m_paletteBuilt && query == m_paletteQuery &&
        g_pickerApps.size() == m_paletteAppCount) {
        for (auto const& c : m_paletteCells) {
            if (c.app < g_pickerApps.size()) {
                auto const& a = g_pickerApps[c.app];
                mark(c.cell, c.name, a, groupOf(a));
            }
        }
        return;
    }
    m_paletteBuilt = true;
    m_paletteQuery = query;
    m_paletteAppCount = g_pickerApps.size();
    m_paletteCells.clear();
    m_paletteList.Children().Clear();
    m_pendingIcons.clear();
    xc::VariableSizedWrapGrid grid;
    grid.Orientation(xc::Orientation::Horizontal);
    grid.ItemWidth(kPaletteCellWidth);
    grid.ItemHeight(kPaletteCellHeight);
    int shown = 0;
    for (size_t ai = 0; ai < g_pickerApps.size(); ai++) {
        auto const& a = g_pickerApps[ai];
        if (!query.empty() && !StrStrIW(a.name.c_str(), query.c_str())) {
            continue;
        }
        shown++;
        xc::Border cell;
        cell.Width(kPaletteCellWidth - 4);
        cell.Height(kPaletteCellHeight - 4);
        cell.Background(Brush(0, 0, 0, 0));
        cell.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
        cell.Padding(ThicknessHelper::FromLengths(4, 8, 4, 4));
        xc::StackPanel sp;
        sp.Spacing(6);
        xc::Border iconHost;
        iconHost.Width(32);
        iconHost.Height(32);
        iconHost.HorizontalAlignment(HorizontalAlignment::Center);
        // The first icons are created right here, like the board's; the rest
        // load a few at a time, so the page opens right away.
        // Icons load from a timer, after the page has been drawn.
        iconHost.Child(IconOrGlyph(L"", L"\uF0E2", m_hwnd, 32));
        m_pendingIcons.push_back({iconHost, a.iconSource});
        sp.Children().Append(iconHost);
        xc::TextBlock name;
        name.Text(a.name);
        name.FontSize(11);
        name.TextAlignment(TextAlignment::Center);
        name.TextWrapping(TextWrapping::Wrap);
        name.MaxLines(2);
        name.TextTrimming(TextTrimming::CharacterEllipsis);
        sp.Children().Append(name);
        cell.Child(sp);
        mark(cell, name, a, groupOf(a));
        m_paletteCells.push_back({cell, name, ai});
        AddHover(cell, m_dark);
        MakeDraggable(cell, m_board, DragKind::NewApp, -1, -1, a.name);
        grid.Children().Append(cell);
    }
    m_paletteList.Children().Append(grid);
    if (shown == 0) {
        m_paletteList.Children().Append(
            Text(g_pickerAppsLoading ? L"Loading apps\u2026" : L"No apps found.",
                 12, true));
    }
    LoadIconsLater();
}

// Loads the pending Start app icons on this (the window's) thread, where
// the shell extracts them reliably: the first ones (the visible rows) right
// away, the rest a batch at a time through Later(), so the page stays
// responsive.
void SettingsWindow::LoadIconsLater() {
    ++m_iconGeneration;
    if (m_pendingIcons.empty()) {
        KillTimer(m_hwnd, kIconTimerId);
    } else {
        SetTimer(m_hwnd, kIconTimerId, 10, nullptr);
    }
}

void SettingsWindow::LoadIconBatch(int gen, int count) {
    if (gen != m_iconGeneration) {
        return;  // the palette has been rebuilt since
    }
    for (int n = 0; n < count && !m_pendingIcons.empty(); n++) {
        auto [host, source] = m_pendingIcons.front();
        m_pendingIcons.pop_front();
        host.Child(IconOrGlyph(source, L"\uF0E2", m_hwnd, 32));
    }
    if (m_pendingIcons.empty()) {
        KillTimer(m_hwnd, kIconTimerId);
    }
}

// --- Dragging ---------------------------------------------------------------

void SettingsWindow::MakeDraggable(FrameworkElement const& el,
                                   std::shared_ptr<BoardSurface> const& surf,
                                   DragKind kind,
                                   int cat,
                                   int item,
                                   std::wstring app,
                                   std::function<void()> onClick,
                                   int sub) {
    winrt::weak_ref<FrameworkElement> weak = el;
    std::weak_ptr<BoardSurface> weakSurf = surf;
    el.PointerPressed([this, weak, weakSurf, kind, cat, item, app,
                       onClick, sub](auto&&,
                                      Input::PointerRoutedEventArgs const& e) {
        auto el = weak.get();
        auto surf = weakSurf.lock();
        if (m_drag.pressed && !m_drag.active) {
            m_drag = DragState{};  // a press whose release went elsewhere
        }
        if (!el || !surf || !surf->root || m_drag.pressed ||
            !e.GetCurrentPoint(el).Properties().IsLeftButtonPressed()) {
            return;
        }
        e.Handled(true);
        if ((surf == m_board || surf == m_flyoutSurface) && cat >= 0) {
            // The app pressed (also a single app that drags its group).
            bool app = kind == DragKind::App ||
                       (kind == DragKind::Group && item >= 0);
            SelectBoardItem(cat, app ? item : -1, app ? sub : -1);
        }
        m_drag = DragState{};
        // A click (a press released without dragging) opens a folder.
        m_drag.click = onClick;
        m_drag.pressed = true;
        m_drag.kind = kind;
        m_drag.cat = cat;
        m_drag.item = item;
        m_drag.sub = sub;
        m_drag.app = app;
        m_drag.surface = surf;
        m_drag.source = el;
        m_drag.start = e.GetCurrentPoint(surf->root).Position();
        // Captured once it's really a drag (see UpdateDrag): capturing on
        // every press would keep XAML from recognizing double taps.
        m_drag.pointer = e.Pointer();
    });
}

// The pointer events of a drag, captured by the surface's root.
void SettingsWindow::HookSurface(std::shared_ptr<BoardSurface> const& surf) {
    auto root = surf->root;
    std::weak_ptr<BoardSurface> weakSurf = surf;
    auto mine = [this, weakSurf] {
        auto s = weakSurf.lock();
        return m_drag.pressed && s && m_drag.surface == s;
    };
    root.PointerMoved(
        [this, mine](auto&&, Input::PointerRoutedEventArgs const& e) {
            if (!mine()) {
                return;
            }
            e.Handled(true);
            UpdateDrag(e.GetCurrentPoint(m_drag.surface->root).Position());
        });
    // Also when something below already handled the release: a click that
    // didn't become a drag isn't captured, and its release would otherwise
    // never get here.
    root.AddHandler(
        UIElement::PointerReleasedEvent(),
        winrt::box_value(Input::PointerEventHandler(
        [this, mine](wf::IInspectable const&,
                     Input::PointerRoutedEventArgs const& e) {
            if (!mine()) {
                return;
            }
            e.Handled(true);
            auto root = m_drag.surface->root;
            UpdateDrag(e.GetCurrentPoint(root).Position());
            // A click on a folder opens it.
            auto click = m_drag.active ? nullptr : m_drag.click;
            // A click (no drag) on an app on the board: show its settings.
            bool appClick = !m_drag.active && m_drag.surface == m_board &&
                            (m_drag.kind == DragKind::App ||
                             (m_drag.kind == DragKind::Group &&
                              m_drag.item >= 0));
            // Not for a folder: a click opens it, over the board, which then
            // stays where it is.
            if (EditItem const* item = DraggedItem(m_drag);
                item && item->type == L"folder") {
                appClick = false;
            }
            EndDrag(m_drag.active);
            root.ReleasePointerCapture(e.Pointer());
            if (click) {
                Later(click);
            } else if (appClick) {
                Later([this] { ScrollToAppSettings(); });
            }
        })),
        true);
    root.PointerCaptureLost([this, mine](auto&&, auto&&) {
        if (mine()) {
            EndDrag(false);
        }
    });
}

// What follows the pointer while an app is dragged: its icon and name.
// What follows the pointer while an app is dragged: a copy of the cell it
// was taken from, at the same size (and board zoom), text trimmed alike.
FrameworkElement SettingsWindow::AppGhost(FrameworkElement const& icon,
                                          std::wstring const& name,
                                          AppGhostLook const& look) {
    xc::Border b;
    b.Width(look.width);
    b.Height(look.height);
    b.Background(m_dark ? Brush(0x15, 255, 255, 255) : Brush(0x0C, 0, 0, 0));
    b.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
    b.Padding(look.padding);
    xc::StackPanel sp;
    sp.Spacing(look.spacing);
    sp.VerticalAlignment(look.centered ? VerticalAlignment::Center
                                       : VerticalAlignment::Top);
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    sp.Children().Append(icon);
    if (look.showText) {
        xc::TextBlock text;
        text.Text(name);
        text.FontSize(look.fontSize);
        text.TextAlignment(TextAlignment::Center);
        text.TextTrimming(TextTrimming::CharacterEllipsis);
        if (look.maxLines > 1) {
            text.TextWrapping(TextWrapping::Wrap);
            text.MaxLines(look.maxLines);
        } else {
            text.TextWrapping(TextWrapping::NoWrap);
        }
        text.Width(std::max(1.0, look.width - look.padding.Left -
                                     look.padding.Right));
        sp.Children().Append(text);
    }
    b.Child(sp);
    return b;
}

// The app an App drag moves (in a group, or in a folder), if still there.
EditItem* SettingsWindow::DraggedItem(DragState const& d) {
    auto& cats = m_model.groups;
    if (d.cat < 0 || d.cat >= (int)cats.size() || d.item < 0 ||
        d.item >= (int)cats[d.cat].items.size()) {
        return nullptr;
    }
    auto& item = cats[d.cat].items[d.item];
    if (d.sub < 0) {
        return &item;
    }
    return d.sub < (int)item.children.size() ? &item.children[d.sub]
                                             : nullptr;
}

void SettingsWindow::StartDragVisuals() {
    auto& d = m_drag;
    auto surf = d.surface;
    if (!surf || !surf->overlay) {
        return;
    }
    // The board is shrunk to fit; measure how much from a tile.
    for (auto const& t : surf->tiles) {
        double w = t.el.ActualWidth();
        if (w <= 0) {
            continue;
        }
        auto b = BoundsIn(t.el, surf->root);
        if (t.cat == d.cat) {
            d.tileBounds = b;
            d.startW = w;
            d.startH = t.el.ActualHeight();
        }
        d.scale = b.Width / w;
    }
    if (d.scale <= 0) {
        d.scale = 1;
    }
    if (d.kind == DragKind::Resize && d.cat >= 0 &&
        d.cat < (int)m_model.groups.size()) {
        d.originalSize = m_model.groups[d.cat].tileSize;
        d.newSize = d.originalSize;
    }

    d.indicator = xc::Border();
    d.indicator.BorderBrush(xm::SolidColorBrush(AccentColor()));
    d.indicator.CornerRadius(CornerRadiusHelper::FromUniformRadius(4));
    d.indicator.Visibility(Visibility::Collapsed);
    d.indicator.IsHitTestVisible(false);
    surf->overlay.Children().Append(d.indicator);

    auto const& cats = m_model.groups;
    FrameworkElement ghost{nullptr};
    double iconSize = m_model.iconSize * d.scale;
    switch (d.kind) {
        case DragKind::App:
            if (EditItem const* dragged = DraggedItem(d)) {
                auto const& item = *dragged;
                AppGhostLook look;
                bool board = surf == m_board;
                double k = board ? d.scale : 1;  // the board is zoomed
                look.width = d.source.ActualWidth() * k;
                look.height = d.source.ActualHeight() * k;
                if (board) {
                    // As in BuildBoardTile / BuildBoardApp.
                    look.fontSize = AppNameFontSize(m_model.iconSize) * k;
                    look.spacing = 4 * k;
                    look.showText = m_model.showAppLabels &&
                                    cats[d.cat].showAppNames;
                } else {
                    // As in RebuildFlyout.
                    look.padding = ThicknessHelper::FromLengths(4, 10, 4, 8);
                    look.spacing = 6;
                    look.fontSize = 12;
                    look.maxLines = 2;
                    look.centered = false;
                }
                ghost = AppGhost(
                    ItemIconEl(item, m_hwnd, board ? iconSize : 32),
                    ItemTitle(item), look);
            }
            break;
        case DragKind::NewApp: {
            auto const* a = FindPickerApp(d.app);
            // As in RefreshPalette.
            AppGhostLook look;
            look.width = d.source.ActualWidth();
            look.height = d.source.ActualHeight();
            look.padding = ThicknessHelper::FromLengths(4, 8, 4, 4);
            look.spacing = 6;
            look.maxLines = 2;
            look.centered = false;
            ghost = AppGhost(IconOrGlyph(a ? a->iconSource : L"", L"\uF0E2",
                                         m_hwnd, 32),
                             d.app, look);
            break;
        }
        case DragKind::Group:
        case DragKind::Resize: {
            xc::Border b;
            b.Background(m_dark ? Brush(0xF0, 0x2B, 0x2B, 0x2B)
                                : Brush(0xF0, 0xFF, 0xFF, 0xFF));
            b.BorderBrush(m_dark ? Brush(0x30, 255, 255, 255)
                                 : Brush(0x20, 0, 0, 0));
            b.BorderThickness(ThicknessHelper::FromUniformLength(1));
            b.CornerRadius(CornerRadiusHelper::FromUniformRadius(6));
            b.Padding(ThicknessHelper::FromLengths(10, 5, 10, 5));
            // A group's name; an app on its own (dragged by its app), the
            // app's, as its group's name isn't shown (and may be empty).
            std::wstring title;
            if (d.kind == DragKind::Group && d.cat >= 0 &&
                d.cat < (int)cats.size()) {
                auto const& cat = cats[d.cat];
                title = cat.name;
                if (d.item >= 0 && d.item < (int)cat.items.size() &&
                    (IsSingleApp(cat) || cat.items.size() == 1 ||
                     title.empty())) {
                    title = ItemTitle(cat.items[d.item]);
                }
            }
            d.ghostText = Text(title, 12);
            d.ghostText.TextWrapping(TextWrapping::NoWrap);
            b.Child(d.ghostText);
            ghost = b;
            break;
        }
    }
    if (ghost) {
        ghost.IsHitTestVisible(false);
        if ((d.kind == DragKind::App || d.kind == DragKind::NewApp) &&
            d.source) {
            ghost.Opacity(0.85);
            // Where the cell was grabbed stays under the pointer.
            auto sb = BoundsIn(d.source, surf->root);
            d.grab = {d.start.X - sb.X, d.start.Y - sb.Y};
        } else {
            d.grab = {-14, -14};
        }
        d.ghost = ghost;
        surf->overlay.Children().Append(ghost);
    }
    if (d.source && d.kind != DragKind::Resize) {
        d.source.Opacity(0.4);
    }
}

// An app dragged out of the opened group: the group closes and the drag
// goes on over the board (to another group, or empty space for a group of
// its own). `p` is in the opened group's coordinates.
void SettingsWindow::HandDragToBoard(wf::Point p) {
    auto& d = m_drag;
    auto from = d.surface;
    auto board = m_board;
    if (!from || !board || !board->root || !from->root) {
        return;
    }
    wf::Point bp, start;
    try {
        auto toBoard = from->root.TransformToVisual(board->root);
        bp = toBoard.TransformPoint(p);
        start = toBoard.TransformPoint(d.start);
    } catch (...) {
        return;
    }
    if (from->overlay) {
        from->overlay.Children().Clear();
    }
    d.ghost = nullptr;
    d.indicator = nullptr;
    d.ghostText = nullptr;
    d.surface = board;
    d.start = start;
    StartDragVisuals();  // on the board, while the app's cell still exists
    if (d.pointer) {
        board->root.CapturePointer(d.pointer);
    }
    if (auto f = m_overflowFlyout.get()) {
        f.Hide();
    }
    UpdateDrag(bp);
}

void SettingsWindow::UpdateDrag(wf::Point p) {
    auto& d = m_drag;
    auto surf = d.surface;
    if (!surf) {
        return;
    }
    // Out of the opened group (with a little margin): on over the board.
    if (d.active && d.kind == DragKind::App && surf == m_flyoutSurface &&
        m_board && surf->root) {
        double w = surf->root.ActualWidth(), h = surf->root.ActualHeight();
        if (p.X < -8 || p.Y < -8 || p.X > w + 8 || p.Y > h + 8) {
            HandDragToBoard(p);
            return;
        }
    }
    if (!d.active) {
        float dx = p.X - d.start.X, dy = p.Y - d.start.Y;
        if (dx * dx + dy * dy < 36) {
            return;
        }
        d.active = true;
        // Captured by the surface, which stays while the board is rebuilt
        // (a live resize rebuilds the tile being dragged).
        if (d.pointer) {
            surf->root.CapturePointer(d.pointer);
        }
        StartDragVisuals();
    }
    if (d.ghost) {
        xc::Canvas::SetLeft(d.ghost, p.X - d.grab.X);
        xc::Canvas::SetTop(d.ghost, p.Y - d.grab.Y);
    }
    auto show = [&](wf::Rect r, bool outline) {
        auto ind = d.indicator;
        if (!ind) {
            return;
        }
        if (r.Width <= 0 || r.Height <= 0) {
            ind.Visibility(Visibility::Collapsed);
            return;
        }
        ind.Visibility(Visibility::Visible);
        ind.Width(r.Width);
        ind.Height(r.Height);
        xc::Canvas::SetLeft(ind, r.X);
        xc::Canvas::SetTop(ind, r.Y);
        if (outline) {
            ind.Background(nullptr);
            ind.BorderThickness(ThicknessHelper::FromUniformLength(2));
        } else {
            ind.Background(xm::SolidColorBrush(AccentColor()));
            ind.BorderThickness(ThicknessHelper::FromUniformLength(0));
        }
    };
    auto const& s = m_model;
    d.targetCat = -1;
    d.targetIndex = -1;
    d.targetFolder = -1;
    d.makeFolder = false;
    // Over the middle half of a cell (an app or a folder): into it, rather
    // than before or after it.
    auto overMiddle = [&](wf::Rect const& cb) {
        return p.X > cb.X + cb.Width / 4 && p.X < cb.X + cb.Width * 3 / 4;
    };
    // The apps of a folder of group `c` (an item index), if it is one.
    auto folderApps = [&](int c, int f) -> std::vector<EditItem> const* {
        if (c < 0 || c >= (int)s.groups.size() || f < 0 ||
            f >= (int)s.groups[c].items.size() ||
            s.groups[c].items[f].type != L"folder") {
            return nullptr;
        }
        return &s.groups[c].items[f].children;
    };
    switch (d.kind) {
        case DragKind::App:
        case DragKind::NewApp: {
            // In the opened folder: among its apps.
            bool inFolder = surf == m_flyoutSurface && m_flyoutFolder >= 0;
            // A folder doesn't go into a folder.
            EditItem const* dragged =
                d.kind == DragKind::App ? DraggedItem(d) : nullptr;
            bool draggingFolder = dragged && dragged->type == L"folder";
            for (auto const& t : surf->tiles) {
                auto b = BoundsIn(t.el, surf->root);
                if (!RectContains(b, p) || t.cat < 0 ||
                    t.cat >= (int)s.groups.size()) {
                    continue;
                }
                d.targetCat = t.cat;
                d.targetIndex = (int)s.groups[t.cat].items.size();
                if (inFolder) {
                    auto const* apps = folderApps(t.cat, m_flyoutFolder);
                    if (!apps) {
                        d.targetCat = -1;
                        break;
                    }
                    d.targetFolder = m_flyoutFolder;
                    d.targetIndex = (int)apps->size();
                }
                wf::Rect ind = b;
                bool outline = true;
                // Before or after the app under the pointer.
                for (auto const& c : surf->cells) {
                    if (c.cat != t.cat) {
                        continue;
                    }
                    auto cb = BoundsIn(c.el, surf->root);
                    if (!RectContains(cb, p)) {
                        continue;
                    }
                    // Over the middle of a folder: into it.
                    bool self = d.kind == DragKind::App && d.sub < 0 &&
                                d.cat == c.cat && d.item == c.index;
                    if (c.folder && !c.cluster && !draggingFolder && !self &&
                        overMiddle(cb)) {
                        if (auto const* apps = folderApps(c.cat, c.index)) {
                            d.targetFolder = c.index;
                            d.targetIndex = (int)apps->size();
                            ind = cb;
                            break;
                        }
                    }
                    // Over the middle of an app: the two make a folder.
                    if (!c.folder && !c.cluster && c.sub < 0 && !inFolder &&
                        !draggingFolder && !self && overMiddle(cb)) {
                        d.makeFolder = true;
                        d.targetIndex = c.index;
                        ind = cb;
                        break;
                    }
                    bool after = !c.cluster && p.X > cb.X + cb.Width / 2;
                    d.targetIndex =
                        (c.sub >= 0 ? c.sub : c.index) + (after ? 1 : 0);
                    ind = {after ? cb.X + cb.Width - 1.5f : cb.X - 1.5f,
                           cb.Y + 4, 3, std::max(0.0f, cb.Height - 8)};
                    outline = false;
                    break;
                }
                show(ind, outline);
                break;
            }
            // A Start app on the board's empty space: a new group with it.
            if (d.targetCat < 0 &&
                (d.kind == DragKind::NewApp || d.kind == DragKind::App) &&
                surf == m_board && m_boardHost) {
                auto b = BoundsIn(m_boardHost, surf->root);
                if (RectContains(b, p)) {
                    d.targetCat = kNewGroupTarget;
                    show(b, true);
                }
            }
            break;
        }
        case DragKind::Group: {
            d.intoGroup = false;
            bool singleApp = d.item >= 0 && d.cat >= 0 &&
                             d.cat < (int)s.groups.size() &&
                             s.groups[d.cat].items.size() == 1;
            // A single app (not a folder) over the middle of another
            // group's app: the two make a folder; of a folder: into it.
            if (singleApp && s.groups[d.cat].items[0].type != L"folder") {
                for (auto const& c : surf->cells) {
                    if (c.cat == d.cat || c.cluster || c.sub >= 0 ||
                        c.cat < 0 || c.cat >= (int)s.groups.size()) {
                        continue;
                    }
                    auto cb = BoundsIn(c.el, surf->root);
                    if (!RectContains(cb, p) || !overMiddle(cb)) {
                        continue;
                    }
                    d.targetCat = c.cat;
                    d.intoGroup = true;
                    if (auto const* apps = folderApps(c.cat, c.index)) {
                        d.targetFolder = c.index;
                        d.targetIndex = (int)apps->size();
                    } else {
                        d.makeFolder = true;
                        d.targetIndex = c.index;
                    }
                    show(cb, true);
                    break;
                }
            }
            for (auto const& t : surf->tiles) {
                if (d.targetCat >= 0) {
                    break;  // onto an app (above)
                }
                auto b = BoundsIn(t.el, surf->root);
                if (!RectContains(b, p)) {
                    continue;
                }
                // A single app over the middle of another tile: into it.
                if (singleApp && t.cat != d.cat &&
                    p.X > b.X + b.Width / 4 && p.X < b.X + b.Width * 3 / 4) {
                    d.targetCat = t.cat;
                    d.intoGroup = true;
                    show(b, true);
                    break;
                }
                bool before = p.X < b.X + b.Width / 2;
                d.targetCat = t.cat;
                d.targetIndex = before ? t.cat : t.cat + 1;
                float gap = (float)(s.gapX * d.scale / 2);
                show({before ? b.X - gap - 1.5f : b.X + b.Width + gap - 1.5f,
                      b.Y, 3, b.Height},
                     false);
                break;
            }
            // Between tiles (in a gap, or under a tile where its name is):
            // the nearest tile of that row decides, before or after it.
            if (d.targetCat < 0) {
                float labelBand =
                    (float)((BoardLabelHeight(s) + s.gapY) * d.scale);
                float gapAbove = (float)(s.gapY * d.scale / 2);
                float bestDist = 1e9f;
                wf::Rect best{};
                int bestCat = -1;
                for (auto const& t : surf->tiles) {
                    if (t.cat < 0 || t.cat >= (int)s.groups.size()) {
                        continue;
                    }
                    auto b = BoundsIn(t.el, surf->root);
                    if (p.Y < b.Y - gapAbove ||
                        p.Y > b.Y + b.Height + labelBand) {
                        continue;  // another row
                    }
                    float dist = p.X < b.X                ? b.X - p.X
                                 : p.X > b.X + b.Width ? p.X - b.X - b.Width
                                                       : 0;
                    if (dist < bestDist) {
                        bestDist = dist;
                        best = b;
                        bestCat = t.cat;
                    }
                }
                if (bestCat >= 0) {
                    bool before = p.X < best.X + best.Width / 2;
                    d.targetCat = bestCat;
                    d.targetIndex = before ? bestCat : bestCat + 1;
                    float gap = (float)(s.gapX * d.scale / 2);
                    show({before ? best.X - gap - 1.5f
                                 : best.X + best.Width + gap - 1.5f,
                          best.Y, 3, best.Height},
                         false);
                }
            }
            break;
        }
        case DragKind::Resize: {
            // The pointer drags the tile's bottom-right corner, and the
            // tile snaps to the nearest offered size, in app spots (half
            // cells). Only its size changes; its app grid only gets smaller
            // if it no longer fits (on drop). The tile is re-laid out live
            // as the size snaps.
            double labelH = BoardLabelHeight(s);
            double w = d.startW + (p.X - d.start.X) / d.scale;
            double h = d.startH + (p.Y - d.start.Y) / d.scale;
            double spotW =
                m_boardHalf > 0 ? m_boardHalf : (s.tileSize + s.gapX) / 2;
            double spotH = (s.tileSize + s.gapY + labelH) / 2;
            double c = Clamp((w + s.gapX) / spotW, 1, 6);
            double r = Clamp((h + s.gapY + labelH) / spotH, 1, 6);
            std::wstring size = d.newSize;
            double best = 1e9;
            for (PCWSTR t : kTileSizes) {
                if (std::wstring_view(t) == L"1x1") {
                    continue;  // an app without a group, not a group size
                }
                auto l = MakeLayout(t, L"");
                double dist = std::hypot(l.spanHalfColumns - c,
                                         l.spanHalfRows - r);
                if (dist < best) {
                    best = dist;
                    size = t;
                }
            }
            d.targetCat = d.cat;
            if (d.ghostText) {
                d.ghostText.Text(TileSizeText(size));
            }
            if (size != d.newSize && d.cat >= 0 &&
                d.cat < (int)m_model.groups.size()) {
                d.newSize = size;
                m_model.groups[d.cat].tileSize = size;
                RebuildBoard();
            }
            break;
        }
    }
    if (d.targetCat < 0 && d.targetCat != kNewGroupTarget) {
        show({}, false);
    }
}

void SettingsWindow::EndDrag(bool drop) {
    DragState d = m_drag;
    m_drag = DragState{};
    if (d.surface && d.surface->overlay) {
        d.surface->overlay.Children().Clear();
    }
    if (d.source) {
        d.source.Opacity(1);
    }
    if (d.kind == DragKind::Resize && d.active && d.cat >= 0 &&
        d.cat < (int)m_model.groups.size()) {
        // The size was applied live; keep it, or put it back if canceled.
        auto& cat = m_model.groups[d.cat];
        if (!drop && !d.originalSize.empty() &&
            cat.tileSize != d.originalSize) {
            cat.tileSize = d.originalSize;
            RebuildBoard();
        } else if (drop && cat.tileSize != d.originalSize) {
            // Applied from the size it had before the drag.
            std::wstring size = cat.tileSize;
            cat.tileSize = d.originalSize;
            SetTileSize(cat, size);
            OnChanged();
            RebuildBoard();
            LoadBoardGroupFields();
        }
        return;
    }
    if (!drop || !d.active) {
        return;
    }
    // Applied after this pointer event: rebuilding the board removes the
    // element that has the pointer capture.
    Later([this, d] { ApplyDrop(d); });
}

void SettingsWindow::MakeFolderAt(int ci, int ii, EditItem item) {
    auto& items = m_model.groups[ci].items;
    EditItem folder;
    folder.type = L"folder";
    folder.label = kFolderName;
    folder.children.push_back(std::move(items[ii]));
    folder.children.push_back(std::move(item));
    items[ii] = std::move(folder);
    m_selCat = ci;
    m_selItem = ii;
    m_selSub = -1;
    SelectBoardGroup(m_selCat);
}

void SettingsWindow::ApplyDrop(DragState const& d) {
    auto& cats = m_model.groups;
    auto validCat = [&](int c) { return c >= 0 && c < (int)cats.size(); };
    bool changed = false;
    switch (d.kind) {
        case DragKind::App:
        case DragKind::NewApp: {
            // Takes the dragged app out of its group or folder.
            auto eraseSource = [&] {
                auto& items = cats[d.cat].items;
                if (d.sub < 0) {
                    items.erase(items.begin() + d.item);
                } else {
                    auto& apps = items[d.item].children;
                    apps.erase(apps.begin() + d.sub);
                }
            };
            if (d.kind == DragKind::App && d.targetCat == kNewGroupTarget) {
                // Out of its group to empty space: a group of its own.
                EditItem const* src = DraggedItem(d);
                if (!src) {
                    break;
                }
                EditGroup g = MakeSingleApp(*src);
                eraseSource();
                cats.push_back(std::move(g));
                m_selCat = (int)cats.size() - 1;
                m_selItem = 0;
                m_selSub = -1;
                m_boardSelCat = m_selCat;
                changed = true;
                break;
            }
            if (d.kind == DragKind::NewApp && d.targetCat == kNewGroupTarget) {
                // A group of its own: one spot, without a name or a
                // background, so it looks like a single app.
                EditGroup g;
                auto const* a = FindPickerApp(d.app);
                g.name = a ? a->name : d.app;
                g.tileSize = L"1x1";
                g.appGrid = kAutoGrid;
                g.showLabel = false;
                g.showBackground = false;
                EditItem item;
                item.type = L"app";
                item.app = d.app;
                g.items.push_back(std::move(item));
                cats.push_back(std::move(g));
                m_selCat = (int)cats.size() - 1;
                m_selItem = 0;
                m_selSub = -1;
                m_boardSelCat = m_selCat;
                changed = true;
                break;
            }
            if (!validCat(d.targetCat)) {
                break;
            }
            if (d.makeFolder) {
                // Onto an app: the two make a folder in its place.
                int ti = d.targetIndex;
                if (ti < 0 || ti >= (int)cats[d.targetCat].items.size() ||
                    cats[d.targetCat].items[ti].type == L"folder") {
                    break;
                }
                EditItem item;
                if (d.kind == DragKind::App) {
                    EditItem const* src = DraggedItem(d);
                    if (!src || src->type == L"folder" ||
                        (d.sub < 0 && d.cat == d.targetCat && d.item == ti)) {
                        break;
                    }
                    item = *src;
                    // Indices after the app taken out move up by one.
                    if (d.sub < 0 && d.cat == d.targetCat && d.item < ti) {
                        ti--;
                    }
                    eraseSource();
                } else {
                    item.type = L"app";
                    item.app = d.app;
                }
                MakeFolderAt(d.targetCat, ti, std::move(item));
                changed = true;
                break;
            }
            int tf = d.targetFolder;  // into this folder, or -1
            if (tf >= (int)cats[d.targetCat].items.size() ||
                (tf >= 0 && cats[d.targetCat].items[tf].type != L"folder")) {
                break;
            }
            EditItem item;
            bool move = d.kind == DragKind::App;
            if (move) {
                EditItem const* src = DraggedItem(d);
                // Folders don't go into folders (nor into themselves).
                if (!src || (tf >= 0 && src->type == L"folder")) {
                    break;
                }
                item = *src;
            } else {
                // Always a new item: an app can be in several groups, or
                // more than once in one.
                item.type = L"app";
                item.app = d.app;
            }
            // Onto an app without a group: the two make a group.
            if (tf < 0 && IsSingleApp(cats[d.targetCat])) {
                MakeGroupOf(cats[d.targetCat], NewGroupName(cats));
            }
            int ti = d.targetIndex;
            if (move) {
                // Indices after the app taken out move up by one.
                bool sameList = d.cat == d.targetCat &&
                                (d.sub < 0 ? tf < 0 : tf == d.item);
                int si = d.sub < 0 ? d.item : d.sub;
                if (sameList && ti > si) {
                    ti--;
                }
                if (tf >= 0 && d.sub < 0 && d.cat == d.targetCat &&
                    d.item < tf) {
                    tf--;
                }
                eraseSource();
            }
            auto& dest = tf >= 0 ? cats[d.targetCat].items[tf].children
                                 : cats[d.targetCat].items;
            ti = std::clamp(ti, 0, (int)dest.size());
            dest.insert(dest.begin() + ti, std::move(item));
            m_selCat = d.targetCat;
            m_selItem = tf >= 0 ? tf : ti;
            m_selSub = tf >= 0 ? ti : -1;
            SelectBoardGroup(m_selCat);
            changed = true;
            break;
        }
        case DragKind::Group: {
            int from = d.cat, to = d.targetIndex;
            if (d.intoGroup) {
                // A single app into another group: its own group goes.
                if (!validCat(from) || !validCat(d.targetCat) ||
                    from == d.targetCat || cats[from].items.empty()) {
                    break;
                }
                EditItem item = cats[from].items.front();
                int target = d.targetCat;
                auto& targetItems = cats[target].items;
                int tf = d.targetFolder;
                if (d.makeFolder || tf >= 0) {
                    // Onto an app (a folder of the two) or into a folder.
                    int ti = d.makeFolder ? d.targetIndex : tf;
                    if (item.type == L"folder" || ti < 0 ||
                        ti >= (int)targetItems.size() ||
                        (targetItems[ti].type == L"folder") != (tf >= 0)) {
                        break;
                    }
                    cats.erase(cats.begin() + from);
                    if (from < target) {
                        target--;
                    }
                    if (d.makeFolder) {
                        MakeFolderAt(target, ti, std::move(item));
                    } else {
                        auto& apps = cats[target].items[ti].children;
                        apps.push_back(std::move(item));
                        m_selCat = target;
                        m_selItem = ti;
                        m_selSub = (int)apps.size() - 1;
                        SelectBoardGroup(m_selCat);
                    }
                    changed = true;
                    break;
                }
                // Onto an app without a group: the two make a group.
                if (IsSingleApp(cats[d.targetCat])) {
                    MakeGroupOf(cats[d.targetCat], NewGroupName(cats));
                }
                auto& dest = cats[d.targetCat].items;
                dest.push_back(std::move(item));
                int index = (int)dest.size() - 1;
                cats.erase(cats.begin() + from);
                if (from < target) {
                    target--;
                }
                m_selCat = target;
                m_selItem = index;
                m_selSub = -1;
                SelectBoardGroup(m_selCat);
                changed = true;
                break;
            }
            if (!validCat(from) || to < 0 || to > (int)cats.size()) {
                break;
            }
            if (to > from) {
                to--;
            }
            if (to == from) {
                break;
            }
            EditGroup moved = std::move(cats[from]);
            cats.erase(cats.begin() + from);
            bool single = IsSingleApp(moved);
            cats.insert(cats.begin() + to, std::move(moved));
            m_selCat = to;
            m_selItem = single ? 0 : -1;
            m_selSub = -1;  // an app on its own stays the app
            SelectBoardGroup(m_selCat);
            changed = true;
            break;
        }
        case DragKind::Resize:
            break;  // applied live, see EndDrag
    }
    if (!changed) {
        return;
    }
    OnChanged();
    if (d.surface && d.surface == m_flyoutSurface) {
        RebuildFlyout();  // the board follows when the flyout closes
        LoadBoardDetails();
    } else {
        RebuildBoard();
        if (d.kind == DragKind::NewApp || d.kind == DragKind::App) {
            RefreshPalette();
        }
    }
}

// --- Menus ------------------------------------------------------------------

void SettingsWindow::ShowRenameFlyout(int ci, FrameworkElement const& anchor) {
    if (ci < 0 || ci >= (int)m_model.groups.size()) {
        return;
    }
    xc::Flyout fly;
    ThemeFlyout(fly, m_dark);
    xc::StackPanel sp;
    sp.Spacing(8);
    xc::TextBox box;
    box.Header(Box(L"Group name"));
    box.Width(240);
    box.Text(m_model.groups[ci].name);
    sp.Children().Append(box);
    auto holder = std::make_shared<xc::Flyout>(fly);
    fly.Closed([holder](auto&&, auto&&) { *holder = nullptr; });
    auto commit = [this, ci, box, holder] {
        std::wstring name(box.Text());
        if (ci < (int)m_model.groups.size() && !name.empty() &&
            name != m_model.groups[ci].name) {
            m_model.groups[ci].name = name;
            OnChanged();
            Later([this] {
                RebuildBoard();
                RefreshPalette();
            });
        }
        if (*holder) {
            holder->Hide();
        }
    };
    box.KeyDown([commit](auto&&, Input::KeyRoutedEventArgs const& e) {
        if (e.Key() == winrt::Windows::System::VirtualKey::Enter) {
            e.Handled(true);
            commit();
        }
    });
    auto ok = TextButton(nullptr, L"Rename", commit);
    ok.HorizontalAlignment(HorizontalAlignment::Right);
    sp.Children().Append(ok);
    fly.Content(sp);
    fly.Opened([box](auto&&, auto&&) {
        box.Focus(FocusState::Programmatic);
        box.SelectAll();
    });
    fly.ShowAt(anchor);
}

xc::MenuFlyout SettingsWindow::TileMenu(int ci, FrameworkElement const& tile) {
    xc::MenuFlyout menu;
    ThemeMenu(menu, m_dark);
    winrt::weak_ref<FrameworkElement> weakTile = tile;
    auto add = [&](PCWSTR glyph, PCWSTR text, std::function<void()> fn) {
        xc::MenuFlyoutItem mi;
        mi.Text(text);
        xc::FontIcon icon;
        icon.Glyph(glyph);
        mi.Icon(icon);
        mi.Click([this, fn](auto&&, auto&&) { Later(fn); });
        menu.Items().Append(mi);
    };
    add(L"", L"Rename…", [this, ci, weakTile] {
        if (auto t = weakTile.get()) {
            ShowRenameFlyout(ci, t);
        }
    });
    xc::MenuFlyoutSubItem sizes;
    sizes.Text(L"Tile size");
    xc::FontIcon sizeIcon;
    sizeIcon.Glyph(L"");
    sizes.Icon(sizeIcon);
    std::wstring current = m_model.groups[ci].tileSize;
    for (auto const& [value, text] : TileSizeOptions()) {
        xc::ToggleMenuFlyoutItem t;
        t.Text(text);
        t.IsChecked(value == current);
        std::wstring v = value;
        t.Click([this, ci, v](auto&&, auto&&) {
            Later([this, ci, v] {
                if (ci < (int)m_model.groups.size() &&
                    m_model.groups[ci].tileSize != v) {
                    SetTileSize(m_model.groups[ci], v);
                    OnChanged();
                    RebuildBoard();
                    LoadBoardGroupFields();
                }
            });
        });
        sizes.Items().Append(t);
    }
    menu.Items().Append(sizes);
    xc::MenuFlyoutSubItem grids;
    grids.Text(L"Apps in the tile");
    xc::FontIcon gridIcon;
    gridIcon.Glyph(L"");
    grids.Icon(gridIcon);
    std::wstring currentGrid = m_model.groups[ci].appGrid;
    for (auto const& [value, text] :
         AppGridOptions(m_model.groups[ci].tileSize)) {
        xc::ToggleMenuFlyoutItem t;
        t.Text(text);
        t.IsChecked(value == currentGrid);
        std::wstring v = value;
        t.Click([this, ci, v](auto&&, auto&&) {
            Later([this, ci, v] {
                if (ci < (int)m_model.groups.size() &&
                    m_model.groups[ci].appGrid != v) {
                    m_model.groups[ci].appGrid = v;
                    OnChanged();
                    RebuildBoard();
                    LoadBoardGroupFields();
                }
            });
        });
        grids.Items().Append(t);
    }
    menu.Items().Append(grids);
    add(L"", L"Duplicate", [this, ci] {
        if (ci < (int)m_model.groups.size()) {
            m_selCat = ci;
            DuplicateGroup();
        }
    });
    // A group of a single app: Remove from group dissolves the group,
    // leaving the app on its own.
    if (ci < (int)m_model.groups.size() &&
        m_model.groups[ci].items.size() == 1 &&
        !IsSingleApp(m_model.groups[ci])) {
        add(L"\uE8E6", L"Remove from group",
            [this, ci] { RemoveGroupKeepApp(ci); });
    }
    add(L"", L"Delete", [this, ci] {
        if (ci < (int)m_model.groups.size()) {
            m_selCat = ci;
            DeleteGroup();
        }
    });
    return menu;
}

xc::MenuFlyout SettingsWindow::AppMenu(int ci, int ii) {
    xc::MenuFlyout menu;
    ThemeMenu(menu, m_dark);
    auto add = [&](auto& items, PCWSTR glyph, std::wstring const& text,
                   std::function<void()> fn) {
        xc::MenuFlyoutItem mi;
        mi.Text(text);
        if (glyph) {
            xc::FontIcon icon;
            icon.Glyph(glyph);
            mi.Icon(icon);
        }
        mi.Click([this, fn](auto&&, auto&&) { Later(fn); });
        items.Append(mi);
    };
    auto valid = [this, ci, ii] {
        return ci < (int)m_model.groups.size() &&
               ii < (int)m_model.groups[ci].items.size();
    };
    auto items = menu.Items();
    bool isFolder = valid() && m_model.groups[ci].items[ii].type == L"folder";
    if (isFolder) {
        add(items, L"", L"Open", [this, ci, ii, valid] {
            if (valid()) {
                SelectBoardItem(ci, ii);
                OpenFolder(ci, ii);
            }
        });
    }
    add(items, L"", L"Edit…", [this, ci, ii] {
        // Selects it: its settings are below the board.
        if (ci < (int)m_model.groups.size() &&
            ii < (int)m_model.groups[ci].items.size()) {
            SelectBoardItem(ci, ii);
            ScrollToAppSettings();
        }
    });
    // Move left / right: one place earlier or later in the group.
    auto moveBy = [this, ci, ii, valid](int delta) {
        if (!valid()) {
            return;
        }
        auto& list = m_model.groups[ci].items;
        int j = ii + delta;
        if (j < 0 || j >= (int)list.size()) {
            return;
        }
        std::swap(list[ii], list[j]);
        m_selCat = ci;
        m_selItem = j;
        m_selSub = -1;
        OnChanged();
        RefreshBoardAfterEdit();
    };
    int count = ci < (int)m_model.groups.size()
                    ? (int)m_model.groups[ci].items.size()
                    : 0;
    if (ii > 0) {
        add(items, L"\uE72B", L"Move left", [moveBy] { moveBy(-1); });
    }
    if (ii + 1 < count) {
        add(items, L"\uE72A", L"Move right", [moveBy] { moveBy(1); });
    }
    bool single = ci < (int)m_model.groups.size() &&
                  IsSingleApp(m_model.groups[ci]);
    // An app without a group: Add to group makes it a group (2 x 2).
    if (single) {
        add(items, L"\uF0E2", L"Add to group", [this, ci, valid] {
            if (!valid() || !IsSingleApp(m_model.groups[ci])) {
                return;
            }
            MakeGroupOf(m_model.groups[ci], NewGroupName(m_model.groups));
            m_selCat = ci;
            m_selItem = 0;
            m_selSub = -1;
            m_boardSelCat = ci;
            OnChanged();
            RefreshBoardAfterEdit();
        });
    }
    // Move to another group (not an app without one); an app without a
    // group then doesn't stay behind empty.
    bool anyGroup = false;
    for (int j = 0; j < (int)m_model.groups.size(); j++) {
        anyGroup |= j != ci && !IsSingleApp(m_model.groups[j]);
    }
    if (anyGroup) {
        xc::MenuFlyoutSubItem moveTo;
        moveTo.Text(L"Move to");
        xc::FontIcon moveIcon;
        moveIcon.Glyph(L"");
        moveTo.Icon(moveIcon);
        auto moveItems = moveTo.Items();
        for (int j = 0; j < (int)m_model.groups.size(); j++) {
            if (j == ci || IsSingleApp(m_model.groups[j])) {
                continue;
            }
            add(moveItems, nullptr, m_model.groups[j].name,
                [this, ci, ii, j, valid] {
                    if (!valid() || j >= (int)m_model.groups.size()) {
                        return;
                    }
                    auto& from = m_model.groups[ci].items;
                    EditItem item = from[ii];
                    from.erase(from.begin() + ii);
                    m_model.groups[j].items.push_back(std::move(item));
                    int target = j;
                    if (IsSingleApp(m_model.groups[ci]) &&
                        m_model.groups[ci].items.empty()) {
                        m_model.groups.erase(m_model.groups.begin() + ci);
                        if (ci < target) {
                            target--;
                        }
                    }
                    m_selCat = target;
                    m_selItem = -1;
                    m_selSub = -1;
                    OnChanged();
                    RebuildBoard();
                    RebuildFlyout();
                    RefreshPalette();
                    LoadBoardDetails();
                });
        }
        items.Append(moveTo);
    }
    // Into a folder of the same group (folders don't go in folders).
    if (!isFolder && ci < (int)m_model.groups.size()) {
        auto const& list = m_model.groups[ci].items;
        xc::MenuFlyoutSubItem toFolder;
        toFolder.Text(L"Move to folder");
        xc::FontIcon folderIcon;
        folderIcon.Glyph(L"\uE8B7");
        toFolder.Icon(folderIcon);
        auto folderItems = toFolder.Items();
        for (int j = 0; j < (int)list.size(); j++) {
            if (list[j].type != L"folder") {
                continue;
            }
            add(folderItems, nullptr, ItemTitle(list[j]),
                [this, ci, ii, j, valid] {
                    if (!valid() ||
                        j >= (int)m_model.groups[ci].items.size() ||
                        m_model.groups[ci].items[j].type != L"folder") {
                        return;
                    }
                    auto& from = m_model.groups[ci].items;
                    EditItem item = from[ii];
                    from.erase(from.begin() + ii);
                    int f = ii < j ? j - 1 : j;
                    from[f].children.push_back(std::move(item));
                    m_selCat = ci;
                    m_selItem = f;
                    m_selSub = (int)from[f].children.size() - 1;
                    OnChanged();
                    RefreshBoardAfterEdit();
                });
        }
        if (toFolder.Items().Size() > 0) {
            items.Append(toFolder);
        }
    }
    add(items, L"", L"Duplicate", [this, ci, ii, valid] {
        if (!valid()) {
            return;
        }
        // The copy goes right after it and becomes the selection.
        m_selCat = ci;
        m_selItem = ii;
        m_selSub = -1;
        DuplicateItem();
    });
    // A group's only app: Remove from group dissolves the group, leaving
    // the app on its own.
    if (!single && count == 1) {
        add(items, L"\uE8E6", L"Remove from group",
            [this, ci] { RemoveGroupKeepApp(ci); });
    }
    add(items, L"", L"Remove", [this, ci, ii, valid] {
        if (!valid()) {
            return;
        }
        // A single app's group goes with it (see DeleteItem).
        if (IsSingleApp(m_model.groups[ci]) &&
            m_model.groups[ci].items.size() == 1) {
            m_selCat = ci;
            m_selItem = -1;
            m_selSub = -1;
            RemoveSelectedGroup();
            return;
        }
        auto& from = m_model.groups[ci].items;
        from.erase(from.begin() + ii);
        m_selItem = -1;
        m_selSub = -1;
        OnChanged();
        RebuildBoard();
        RebuildFlyout();
        RefreshPalette();
        LoadBoardDetails();
    });
    return menu;
}

// The menu of an app in an opened folder: `si` of the folder `fi`.
xc::MenuFlyout SettingsWindow::FolderAppMenu(int ci, int fi, int si) {
    xc::MenuFlyout menu;
    ThemeMenu(menu, m_dark);
    auto add = [&](PCWSTR glyph, std::wstring const& text,
                   std::function<void()> fn) {
        xc::MenuFlyoutItem mi;
        mi.Text(text);
        if (glyph) {
            xc::FontIcon icon;
            icon.Glyph(glyph);
            mi.Icon(icon);
        }
        mi.Click([this, fn](auto&&, auto&&) { Later(fn); });
        menu.Items().Append(mi);
    };
    // The folder's apps, if it's all still there.
    auto apps = [this, ci, fi, si]() -> std::vector<EditItem>* {
        if (ci >= (int)m_model.groups.size() ||
            fi >= (int)m_model.groups[ci].items.size()) {
            return nullptr;
        }
        auto& folder = m_model.groups[ci].items[fi];
        return folder.type == L"folder" && si < (int)folder.children.size()
                   ? &folder.children
                   : nullptr;
    };
    auto done = [this](int c, int item, int sub) {
        m_selCat = c;
        m_selItem = item;
        m_selSub = sub;
        OnChanged();
        RefreshBoardAfterEdit();
    };
    add(L"\uE70F", L"Edit…", [this, ci, fi, si, apps] {
        if (apps()) {
            SelectBoardItem(ci, fi, si);
            ScrollToAppSettings();
        }
    });
    int count = apps() ? (int)apps()->size() : 0;
    auto moveBy = [ci, fi, si, apps, done](int delta) {
        auto* list = apps();
        int j = si + delta;
        if (!list || j < 0 || j >= (int)list->size()) {
            return;
        }
        std::swap((*list)[si], (*list)[j]);
        done(ci, fi, j);
    };
    if (si > 0) {
        add(L"\uE72B", L"Move left", [moveBy] { moveBy(-1); });
    }
    if (si + 1 < count) {
        add(L"\uE72A", L"Move right", [moveBy] { moveBy(1); });
    }
    // Out of the folder: into its group, right after the folder.
    add(L"\uE8E6", L"Move out of folder", [this, ci, fi, si, apps, done] {
        auto* list = apps();
        if (!list) {
            return;
        }
        EditItem item = (*list)[si];
        list->erase(list->begin() + si);
        auto& items = m_model.groups[ci].items;
        items.insert(items.begin() + fi + 1, std::move(item));
        done(ci, fi + 1, -1);
    });
    add(L"\uE8C8", L"Duplicate", [ci, fi, si, apps, done] {
        auto* list = apps();
        if (!list) {
            return;
        }
        EditItem copy = (*list)[si];
        list->insert(list->begin() + si + 1, std::move(copy));
        done(ci, fi, si + 1);
    });
    add(L"\uE74D", L"Remove", [ci, fi, si, apps, done] {
        auto* list = apps();
        if (!list) {
            return;
        }
        list->erase(list->begin() + si);
        done(ci, fi, -1);
    });
    return menu;
}

// Dissolves a group of a single app, keeping the app on its own.
void SettingsWindow::RemoveGroupKeepApp(int ci) {
    if (ci < 0 || ci >= (int)m_model.groups.size() ||
        m_model.groups[ci].items.size() != 1 ||
        IsSingleApp(m_model.groups[ci])) {
        return;
    }
    auto& g = m_model.groups[ci];
    g = MakeSingleApp(g.items.front());
    m_selCat = ci;
    m_selItem = 0;
    m_selSub = -1;
    m_boardSelCat = ci;
    OnChanged();
    RefreshBoardAfterEdit();
}

void SettingsWindow::SelectBoardGroup(int ci) {
    m_boardSelCat = ci;
    UpdateBoardSelection();
}

// Accent borders: on the selected app, or on its group when no app is
// selected, so it's clear what Duplicate and Delete act on.
void SettingsWindow::UpdateBoardSelection() {
    if (!m_board) {
        return;
    }
    for (auto const& t : m_board->tiles) {
        if (t.highlight) {
            t.highlight.Visibility(t.cat == m_boardSelCat && m_selItem < 0
                                       ? Visibility::Visible
                                       : Visibility::Collapsed);
        }
    }
    for (auto const& surf : {m_board, m_flyoutSurface}) {
        if (!surf) {
            continue;
        }
        for (auto const& c : surf->cells) {
            if (auto b = c.el.try_as<xc::Border>(); b && !c.cluster) {
                b.BorderBrush(c.cat == m_selCat && c.index == m_selItem &&
                                      c.sub == m_selSub
                                  ? xm::SolidColorBrush(AccentColor())
                                  : Brush(0, 0, 0, 0));
            }
        }
    }
}

// --- Board details panel ------------------------------------------------------

FrameworkElement SettingsWindow::BuildBoardDetails() {
    m_boardCatControls.clear();
    m_boardItemControls.clear();
    xc::StackPanel sp;
    sp.Spacing(12);

    auto heading = [&](PCWSTR text) {
        auto t = Text(text, 16);
        t.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
        sp.Children().Append(t);
    };

    // Group
    heading(L"Group");
    m_boardCatName = xc::TextBox();
    m_boardCatName.Header(Box(L"Name"));
    m_boardCatName.TextChanged([this](auto&&, auto&&) {
        auto* cat = SelectedGroup();
        if (m_refreshing || !cat || !m_boardCatName) {
            return;
        }
        std::wstring name(m_boardCatName.Text());
        if (name == cat->name) {
            return;
        }
        cat->name = name;
        OnChanged();
        ScheduleBoardRebuild(false);
    });
    auto addApp = AddAppButton();
    auto dupCat =
        TextButton(L"", L"Duplicate", [this] { DuplicateGroup(); });
    auto delCat =
        TextButton(L"", L"Delete", [this] { DeleteGroup(); });
    for (xc::Control c : {xc::Control(addApp), xc::Control(dupCat),
                          xc::Control(delCat)}) {
        m_boardCatControls.push_back(c);
    }

    // Name, with the group's buttons on the same row (level with the box).
    xc::Grid nameRow;
    nameRow.ColumnSpacing(8);
    {
        xc::ColumnDefinition c0, c1;
        c0.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
        c1.Width(GridLengthHelper::Auto());
        nameRow.ColumnDefinitions().Append(c0);
        nameRow.ColumnDefinitions().Append(c1);
    }
    nameRow.Children().Append(m_boardCatName);
    xc::StackPanel catButtons;
    catButtons.Orientation(xc::Orientation::Horizontal);
    catButtons.Spacing(8);
    catButtons.VerticalAlignment(VerticalAlignment::Bottom);
    for (auto b : {addApp, dupCat, delCat}) {
        catButtons.Children().Append(b);
    }
    xc::Grid::SetColumn(catButtons, 1);
    nameRow.Children().Append(catButtons);
    sp.Children().Append(WithHint(
        nameRow, L"Shown under the tile and as the title of the opened group."));
    m_boardCatControls.push_back(m_boardCatName);

    m_boardTileSize = xc::ComboBox();
    m_boardTileSize.Header(Box(L"Tile size"));
    m_boardTileSize.HorizontalAlignment(HorizontalAlignment::Stretch);
    m_boardTileSizeValues.clear();
    for (auto const& [value, text] : TileSizeOptions()) {
        m_boardTileSize.Items().Append(Box(text));
        m_boardTileSizeValues.push_back(value);
    }
    m_boardTileSize.SelectionChanged([this](auto&&, auto&&) {
        auto* cat = SelectedGroup();
        int i = m_boardTileSize ? m_boardTileSize.SelectedIndex() : -1;
        if (m_refreshing || !cat || i < 0 ||
            i >= (int)m_boardTileSizeValues.size() ||
            cat->tileSize == m_boardTileSizeValues[i]) {
            return;
        }
        SetTileSize(*cat, m_boardTileSizeValues[i]);
        OnChanged();
        ScheduleBoardRebuild(false);
        // The app grids offered follow the size.
        Later([this] { LoadBoardGroupFields(); });
    });
    auto tileField = WithHint(
        m_boardTileSize, std::wstring(kTileSizeHint) +
                             L" You can also drag the tile's corner.");
    m_boardCatControls.push_back(m_boardTileSize);
    m_boardAppGrid = xc::ComboBox();
    m_boardAppGrid.Header(Box(L"Apps in the tile"));
    m_boardAppGrid.HorizontalAlignment(HorizontalAlignment::Stretch);
    m_boardAppGridValues.clear();  // filled for the selected group
    m_boardAppGrid.SelectionChanged([this](auto&&, auto&&) {
        auto* cat = SelectedGroup();
        int i = m_boardAppGrid ? m_boardAppGrid.SelectedIndex() : -1;
        if (m_refreshing || !cat || i < 0 ||
            i >= (int)m_boardAppGridValues.size() ||
            cat->appGrid == m_boardAppGridValues[i]) {
            return;
        }
        cat->appGrid = m_boardAppGridValues[i];
        OnChanged();
        ScheduleBoardRebuild(false);
    });
    auto gridField = WithHint(m_boardAppGrid, kAppGridHint);
    m_boardCatControls.push_back(m_boardAppGrid);
    m_boardCatShowNames =
        GroupToggle(L"Show app names", &EditGroup::showAppNames);
    auto namesField = WithHint(m_boardCatShowNames, kShowNamesHint);
    m_boardCatControls.push_back(m_boardCatShowNames);
    m_boardCatShowLabel = GroupToggle(L"Show group name", &EditGroup::showLabel);
    auto labelField = WithHint(
        m_boardCatShowLabel,
        L"The name under the tile. Needs \"Show group names\" on in "
        L"Customization.");
    m_boardCatControls.push_back(m_boardCatShowLabel);
    m_boardCatShowBackground =
        GroupToggle(L"Show background", &EditGroup::showBackground);
    auto backgroundField = WithHint(
        m_boardCatShowBackground,
        L"Off: no background, border or padding, just the apps.");
    m_boardCatControls.push_back(m_boardCatShowBackground);

    // Fields side by side, in equal columns.
    auto fieldRow = [&](std::initializer_list<FrameworkElement> fields) {
        xc::Grid row;
        row.ColumnSpacing(16);
        int col = 0;
        for (auto field : fields) {
            xc::ColumnDefinition cd;
            cd.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
            row.ColumnDefinitions().Append(cd);
            field.VerticalAlignment(VerticalAlignment::Top);
            xc::Grid::SetColumn(field, col++);
            row.Children().Append(field);
        }
        sp.Children().Append(row);
    };
    // The tile's size and apps; then what it shows.
    fieldRow({tileField, gridField});
    fieldRow({namesField, labelField, backgroundField});

    // The app section goes in its own column.
    xc::StackPanel appSp;
    appSp.Spacing(12);

    // App: its name above the heading (as in Explorer Command Bar's
    // settings), the fields as cards below (BuildItemEditor).
    xc::Grid appHeader;
    xc::ColumnDefinition h0, h1;
    h0.Width(GridLengthHelper::FromValueAndType(1, GridUnitType::Star));
    h1.Width(GridLengthHelper::Auto());
    appHeader.ColumnDefinitions().Append(h0);
    appHeader.ColumnDefinitions().Append(h1);
    xc::StackPanel appTitles;
    appTitles.VerticalAlignment(VerticalAlignment::Bottom);
    m_appCaption = Text(L"", 12, true);
    appTitles.Children().Append(m_appCaption);
    auto appTitle = Text(L"App", 20);
    appTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    appTitles.Children().Append(appTitle);
    appHeader.Children().Append(appTitles);
    xc::StackPanel appBar;
    appBar.Orientation(xc::Orientation::Horizontal);
    appBar.VerticalAlignment(VerticalAlignment::Bottom);
    appBar.Spacing(4);
    for (auto b : {IconButton(L"", L"Duplicate",
                              [this] { DuplicateItem(); }),
                   IconButton(L"", L"Remove", [this] { DeleteItem(); })}) {
        appBar.Children().Append(b);
        m_boardItemControls.push_back(b);
    }
    xc::Grid::SetColumn(appBar, 1);
    appHeader.Children().Append(appBar);
    appSp.Children().Append(appHeader);

    // The selected item's editor (BuildItemEditor).
    m_editor = xc::StackPanel();
    appSp.Children().Append(m_editor);

    // The group's settings and the app's, each in its own card, one under
    // the other.
    xc::StackPanel sections;
    sections.Spacing(16);
    sections.Children().Append(Card(sp));
    // The app's fields are cards of their own, on the page.
    xc::Border appCard;
    appCard.Margin(ThicknessHelper::FromLengths(0, 8, 0, 0));
    appCard.Child(appSp);
    m_appCard = appCard;
    sections.Children().Append(appCard);
    return sections;
}

// A switch of the selected group (`field` of EditGroup), on the details
// panel.
xc::ToggleSwitch SettingsWindow::GroupToggle(PCWSTR header,
                                             bool EditGroup::*field) {
    xc::ToggleSwitch t;
    t.Header(Box(header));
    t.MinWidth(0);
    t.OnContent(Box(L"On"));
    t.OffContent(Box(L"Off"));
    t.IsOn(true);
    t.Toggled([this, t, field](auto&&, auto&&) {
        auto* cat = SelectedGroup();
        if (m_refreshing || !cat || cat->*field == t.IsOn()) {
            return;
        }
        cat->*field = t.IsOn();
        OnChanged();
        ScheduleBoardRebuild(false);
    });
    return t;
}

// Fills an "Apps in the tile" list with the grids that fit the group's
// tile and selects its grid. Call with m_refreshing set.
void SettingsWindow::FillAppGridCombo(xc::ComboBox const& combo,
                                      std::vector<std::wstring>& values,
                                      EditGroup const* cat) {
    auto options = AppGridOptions(cat ? cat->tileSize : L"2x2");
    bool same = options.size() == values.size();
    for (size_t i = 0; same && i < options.size(); i++) {
        same = options[i].first == values[i];
    }
    if (!same) {
        combo.Items().Clear();
        values.clear();
        for (auto const& [value, text] : options) {
            combo.Items().Append(Box(text));
            values.push_back(value);
        }
    }
    int sel = -1;
    for (size_t i = 0; cat && i < values.size(); i++) {
        if (values[i] == cat->appGrid) {
            sel = (int)i;
        }
    }
    if (combo.SelectedIndex() != sel) {
        combo.SelectedIndex(sel);
    }
}

// "Add app" with a menu (chevron): a Start app or a program, added to the
// selected group. On the board's toolbar and in the group's settings.
xc::Button SettingsWindow::AddAppButton() {
    xc::MenuFlyout menu;
    ThemeMenu(menu, m_dark);
    auto add = [&](PCWSTR glyph, PCWSTR text, std::wstring type) {
        xc::MenuFlyoutItem mi;
        mi.Text(text);
        xc::FontIcon icon;
        icon.Glyph(glyph);
        mi.Icon(icon);
        mi.Click([this, type](auto&&, auto&&) {
            Later([this, type] { AddItem(type); });
        });
        menu.Items().Append(mi);
    };
    add(L"\uF0E2", L"Start app", L"app");
    add(L"\uE756", L"Program", L"program");
    add(L"\uE774", L"Website", L"website");
    menu.Items().Append(xc::MenuFlyoutSeparator());
    // A folder: opens like a group, with as many apps as it's given.
    add(L"\uE8B7", L"Folder", L"folder");
    menu.Placement(xcp::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    xc::Button addApp;
    xc::StackPanel content;
    content.Orientation(xc::Orientation::Horizontal);
    content.Spacing(8);
    content.Children().Append(Glyph(L"\uE710", 14));
    content.Children().Append(Text(L"Add app", 14));
    auto chevron = Glyph(L"\uE70D", 10);
    chevron.VerticalAlignment(VerticalAlignment::Center);
    chevron.Margin(ThicknessHelper::FromLengths(4, 0, 0, 0));
    content.Children().Append(chevron);
    addApp.Content(content);
    addApp.Flyout(menu);
    xc::ToolTipService::SetToolTip(
        addApp, Box(L"Add a Start app, a program, a website or a folder to the selected group (or, with none selected, as a group of its own)"));
    return addApp;
}

// The selected group's fields, without touching a field being typed in.
void SettingsWindow::LoadBoardGroupFields() {
    if (!m_boardCatName) {
        return;
    }
    // An app without a group has no group settings: shown as none selected.
    auto* cat = SelectedGroup();
    if (cat && IsSingleApp(*cat)) {
        cat = nullptr;
    }
    m_refreshing = true;
    std::wstring name = cat ? cat->name : L"";
    if (std::wstring(m_boardCatName.Text()) != name) {
        m_boardCatName.Text(name);
    }
    int sel = -1;
    for (size_t i = 0; cat && i < m_boardTileSizeValues.size(); i++) {
        if (m_boardTileSizeValues[i] == cat->tileSize) {
            sel = (int)i;
        }
    }
    if (m_boardTileSize && m_boardTileSize.SelectedIndex() != sel) {
        m_boardTileSize.SelectedIndex(sel);
    }
    for (auto [t, field] :
         {std::pair{m_boardCatShowNames, &EditGroup::showAppNames},
          std::pair{m_boardCatShowLabel, &EditGroup::showLabel},
          std::pair{m_boardCatShowBackground, &EditGroup::showBackground}}) {
        bool on = cat ? cat->*field : true;
        if (t && t.IsOn() != on) {
            t.IsOn(on);
        }
    }
    if (m_boardAppGrid) {
        FillAppGridCombo(m_boardAppGrid, m_boardAppGridValues, cat);
    }
    m_refreshing = false;
    for (auto& c : m_boardCatControls) {
        c.IsEnabled(cat != nullptr);
    }
}

// Reloads the whole panel for the current selection.
void SettingsWindow::LoadBoardDetails() {
    if (!m_boardCatName) {
        return;
    }
    m_detailsCat = m_selCat;
    m_detailsItem = m_selItem;
    m_detailsSub = m_selSub;
    LoadBoardGroupFields();
    for (auto& c : m_boardItemControls) {
        c.IsEnabled(SelectedItem() != nullptr);
    }
    bool hasGroup = SelectedGroup() != nullptr;
    bool hasItem = SelectedItem() != nullptr;
    // Add app works without a group too (it makes one).
    for (auto b : {m_boardDup, m_boardDel}) {
        if (b) {
            b.IsEnabled(hasGroup);
        }
    }
    if (m_boardAddApp) {
        m_boardAddApp.IsEnabled(true);
    }
    // "Duplicate app" / "Delete app" with an app selected, else "... group".
    auto setLabel = [](xc::Button const& b, std::wstring const& text) {
        if (auto sp = b ? b.Content().try_as<xc::StackPanel>() : nullptr) {
            uint32_t n = sp.Children().Size();
            if (n > 0) {
                if (auto t = sp.Children().GetAt(n - 1).try_as<xc::TextBlock>()) {
                    t.Text(text);
                }
            }
        }
    };
    setLabel(m_boardDup, hasItem ? L"Duplicate app" : L"Duplicate group");
    setLabel(m_boardDel, hasItem ? L"Delete app" : L"Delete group");
    if (m_boardDup) {
        xc::ToolTipService::SetToolTip(
            m_boardDup, Box(hasItem ? L"Duplicate the selected app"
                                    : L"Duplicate the selected group"));
    }
    if (m_boardDel) {
        xc::ToolTipService::SetToolTip(
            m_boardDel, Box(hasItem ? L"Remove the selected app"
                                    : L"Delete the selected group"));
    }
    BuildItemEditor();
}

void SettingsWindow::SelectBoardItem(int ci, int ii, int sub) {
    bool same = ci == m_selCat && ii == m_selItem && sub == m_selSub;
    m_selCat = ci;
    m_selItem = ii;
    m_selSub = ii >= 0 ? sub : -1;
    SelectBoardGroup(ci);
    if (!same) {
        LoadBoardDetails();
    }
}

// Rebuilds the board (and the Start apps panel) once the current event is
// done; edits typed into the details panel call this on every change.
void SettingsWindow::ScheduleBoardRebuild(bool palette) {
    m_paletteRebuildQueued = m_paletteRebuildQueued || palette;
    if (m_boardRebuildQueued) {
        return;
    }
    m_boardRebuildQueued = true;
    Later([this] {
        m_boardRebuildQueued = false;
        bool palette = m_paletteRebuildQueued;
        m_paletteRebuildQueued = false;
        RebuildBoard();
        if (palette) {
            RefreshPalette();
        }
    });
}

// After an item action (add, move, duplicate, remove) on the board page.
void SettingsWindow::RefreshBoardAfterEdit() {
    if (m_page != L"board") {
        return;
    }
    m_boardSelCat = m_selCat;
    RebuildBoard();
    RebuildFlyout();  // the opened group, if any
    RefreshPalette();
    LoadBoardDetails();
}

// After adding, duplicating or deleting a group: refreshes the board.
void SettingsWindow::RefreshGroupsUi() {
    if (m_page == L"board") {
        m_boardSelCat = m_selCat;
        RebuildBoard();
        RefreshPalette();
        LoadBoardDetails();
    }
}

// --- Actions ----------------------------------------------------------------

void SettingsWindow::AddGroup() {
    std::wstring name = L"New group";
    for (int n = 2; std::any_of(m_model.groups.begin(),
                                m_model.groups.end(),
                                [&](auto& c) { return c.name == name; });
         n++) {
        name = L"New group " + std::to_wstring(n);
    }
    EditGroup cat;
    cat.name = name;
    m_model.groups.push_back(std::move(cat));
    m_selCat = (int)m_model.groups.size() - 1;
    m_selItem = -1;
    m_selSub = -1;
    RefreshGroupsUi();
    OnChanged();
}

void SettingsWindow::DuplicateGroup() {
    auto* cat = SelectedGroup();
    if (!cat) {
        return;
    }
    EditGroup copy = *cat;
    std::wstring base = cat->name + L" (copy)";
    copy.name = base;
    for (int n = 2; std::any_of(m_model.groups.begin(),
                                m_model.groups.end(),
                                [&](auto& c) { return c.name == copy.name; });
         n++) {
        copy.name = base.substr(0, base.size() - 1) + L" " +
                    std::to_wstring(n) + L")";
    }
    m_model.groups.insert(m_model.groups.begin() + m_selCat + 1,
                              std::move(copy));
    m_selCat++;
    m_selItem = -1;
    m_selSub = -1;
    RefreshGroupsUi();
    OnChanged();
}

void SettingsWindow::DeleteGroup() {
    auto* cat = SelectedGroup();
    if (!cat) {
        return;
    }
    if (!cat->items.empty()) {
        // Asked first, in a dialog like the rest of the window; the group
        // is looked up again after, in case the selection changed.
        int index = m_selCat;
        std::wstring name = cat->name;
        size_t count = cat->items.size();
        ShowConfirmDialog(
            L"Delete this group?",
            L"\u201C" + name + L"\u201D and its " + std::to_wstring(count) +
                (count == 1 ? L" app" : L" apps") + L" will be removed.",
            L"Delete", [this, index, name] {
                if (index < (int)m_model.groups.size() &&
                    m_model.groups[index].name == name) {
                    m_selCat = index;
                    RemoveSelectedGroup();
                }
            });
        return;
    }
    RemoveSelectedGroup();
}

void SettingsWindow::RemoveSelectedGroup() {
    if (!SelectedGroup()) {
        return;
    }
    m_model.groups.erase(m_model.groups.begin() + m_selCat);
    m_selCat = std::min(m_selCat, (int)m_model.groups.size() - 1);
    m_selItem = -1;
    m_selSub = -1;
    RefreshGroupsUi();
    OnChanged();
}

void SettingsWindow::AddItem(std::wstring const& type,
                             int folderCat,
                             int folder) {
    // With an app in a folder selected, a new app goes in that folder too.
    if (folder < 0 && m_selSub >= 0 && type != L"folder" && SelectedItem()) {
        folderCat = m_selCat;
        folder = m_selItem;
    }
    if (folder >= 0) {
        if (folderCat < 0 || folderCat >= (int)m_model.groups.size() ||
            folder >= (int)m_model.groups[folderCat].items.size() ||
            m_model.groups[folderCat].items[folder].type != L"folder") {
            return;
        }
        auto& apps = m_model.groups[folderCat].items[folder].children;
        EditItem item;
        item.type = type;
        apps.push_back(std::move(item));
        m_selCat = folderCat;
        m_selItem = folder;
        m_selSub = (int)apps.size() - 1;
        BuildItemEditor();
        OnChanged();
        RefreshBoardAfterEdit();
        return;
    }
    auto* cat = SelectedGroup();
    if (!cat || IsSingleApp(*cat)) {  // none, or an app without a group
        // No group selected: a group of its own, a single app (one spot,
        // without a name or a background).
        std::wstring name = L"New group";
        for (int n = 2; std::any_of(m_model.groups.begin(),
                                    m_model.groups.end(),
                                    [&](auto& c) { return c.name == name; });
             n++) {
            name = L"New group " + std::to_wstring(n);
        }
        EditGroup g;
        g.name = name;
        g.tileSize = L"1x1";
        g.appGrid = kAutoGrid;
        g.showLabel = false;
        g.showBackground = false;
        m_model.groups.push_back(std::move(g));
        m_selCat = (int)m_model.groups.size() - 1;
        cat = &m_model.groups.back();
    }
    EditItem item;
    item.type = type;
    if (type == L"folder") {
        item.label = kFolderName;
    }
    cat->items.push_back(item);
    m_selItem = (int)cat->items.size() - 1;
    m_selSub = -1;
    BuildItemEditor();
    OnChanged();
    RefreshBoardAfterEdit();
}

void SettingsWindow::DuplicateItem() {
    auto* cat = SelectedGroup();
    auto* item = SelectedItem();
    if (!item) {
        return;
    }
    if (m_selSub >= 0) {
        // An app in a folder: the copy goes in the folder, after it.
        EditItem copy = *item;
        auto& apps = cat->items[m_selItem].children;
        apps.insert(apps.begin() + m_selSub + 1, std::move(copy));
        m_selSub++;
        BuildItemEditor();
        OnChanged();
        RefreshBoardAfterEdit();
        return;
    }
    if (IsSingleApp(*cat)) {
        // An app without a group: a copy of its own, after it.
        cat = nullptr;
        m_model.groups.insert(m_model.groups.begin() + m_selCat + 1,
                              MakeSingleApp(*item));
        m_selCat++;
        m_selItem = 0;
        m_selSub = -1;
        m_boardSelCat = m_selCat;
        OnChanged();
        RefreshBoardAfterEdit();
        return;
    }
    EditItem copy = *item;
    cat->items.insert(cat->items.begin() + m_selItem + 1, std::move(copy));
    m_selItem++;
    BuildItemEditor();
    OnChanged();
    RefreshBoardAfterEdit();
}

// Board toolbar: the selected app if there is one, or else its group.
void SettingsWindow::DuplicateSelected() {
    if (SelectedItem()) {
        DuplicateItem();
    } else {
        DuplicateGroup();
    }
}

void SettingsWindow::DeleteSelected() {
    if (SelectedItem()) {
        DeleteItem();
    } else {
        DeleteGroup();
    }
}

void SettingsWindow::DeleteItem() {
    auto* cat = SelectedGroup();
    if (!SelectedItem()) {
        return;
    }
    if (m_selSub >= 0) {
        // An app in a folder; the folder stays (selected once empty).
        auto& apps = cat->items[m_selItem].children;
        apps.erase(apps.begin() + m_selSub);
        m_selSub = std::min(m_selSub, (int)apps.size() - 1);
        BuildItemEditor();
        OnChanged();
        RefreshBoardAfterEdit();
        return;
    }
    // A single app's group (1 x 1): the group goes with it, rather than
    // being left empty.
    if (IsSingleApp(*cat) && cat->items.size() == 1) {
        RemoveSelectedGroup();
        return;
    }
    cat->items.erase(cat->items.begin() + m_selItem);
    m_selItem = std::min(m_selItem, (int)cat->items.size() - 1);
    m_selSub = -1;
    BuildItemEditor();
    OnChanged();
    RefreshBoardAfterEdit();
}

std::wstring SettingsWindow::Browse(PCWSTR title, bool icons) {
    winrt::com_ptr<IFileOpenDialog> dlg;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dlg.put())))) {
        return L"";
    }
    COMDLG_FILTERSPEC iconFilters[] = {
        {L"Icons and programs", L"*.ico;*.exe;*.dll;*.png;*.jpg;*.bmp"},
        {L"All files", L"*.*"}};
    COMDLG_FILTERSPEC programFilters[] = {
        {L"Programs", L"*.exe;*.bat;*.cmd;*.lnk;*.msc;*.ps1"},
        {L"All files", L"*.*"}};
    if (icons) {
        dlg->SetFileTypes(ARRAYSIZE(iconFilters), iconFilters);
    } else {
        dlg->SetFileTypes(ARRAYSIZE(programFilters), programFilters);
    }
    dlg->SetTitle(title);
    // Don't resolve shortcuts: a .lnk is a fine program target.
    FILEOPENDIALOGOPTIONS opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_NODEREFERENCELINKS | FOS_FORCEFILESYSTEM);
    if (FAILED(dlg->Show(m_hwnd))) {
        return L"";
    }
    winrt::com_ptr<IShellItem> result;
    PWSTR path = nullptr;
    std::wstring out;
    if (SUCCEEDED(dlg->GetResult(result.put())) &&
        SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
        out = path;
        CoTaskMemFree(path);
    }
    return out;
}

// --- Import / export ----------------------------------------------------------
// The exported file is JSON, one Windhawk setting key per line, e.g.
// "groups[0].items[1].app": "Notepad". Importing also accepts nested
// objects and arrays laid out like the mod's settings.

namespace wdj = winrt::Windows::Data::Json;
using FlatSettings = std::map<std::wstring, std::wstring>;

std::wstring BuildSettingsJson(std::vector<RegEntry> const& entries) {
    std::wstring json = L"{\n";
    for (size_t i = 0; i < entries.size(); i++) {
        auto const& e = entries[i];
        json += L"  ";
        json += wdj::JsonValue::CreateStringValue(e.name).Stringify();
        json += L": ";
        if (e.isString) {
            json += wdj::JsonValue::CreateStringValue(e.text).Stringify();
        } else if (e.isBool) {
            json += e.number ? L"true" : L"false";
        } else {
            json += std::to_wstring((int)e.number);
        }
        json += i + 1 < entries.size() ? L",\n" : L"\n";
    }
    return json + L"}\n";
}

void FlattenJson(wdj::IJsonValue const& value,
                 std::wstring const& path,
                 FlatSettings& out) {
    switch (value.ValueType()) {
        case wdj::JsonValueType::Object:
            for (auto const& pair : value.GetObject()) {
                std::wstring key(pair.Key());
                FlattenJson(pair.Value(), path.empty() ? key : path + L"." + key,
                            out);
            }
            break;
        case wdj::JsonValueType::Array: {
            auto array = value.GetArray();
            for (uint32_t i = 0; i < array.Size(); i++) {
                FlattenJson(array.GetAt(i),
                            path + L"[" + std::to_wstring(i) + L"]", out);
            }
            break;
        }
        case wdj::JsonValueType::String:
            out[path] = std::wstring(value.GetString());
            break;
        case wdj::JsonValueType::Number:
            out[path] = std::to_wstring((long long)std::llround(value.GetNumber()));
            break;
        case wdj::JsonValueType::Boolean:
            out[path] = value.GetBoolean() ? L"1" : L"0";
            break;
        case wdj::JsonValueType::Null:
            out[path] = L"";
            break;
    }
}

std::wstring FlatString(FlatSettings const& f, std::wstring const& key) {
    auto it = f.find(key);
    return it != f.end() ? it->second : std::wstring();
}

int FlatInt(FlatSettings const& f, std::wstring const& key, int fallback) {
    auto it = f.find(key);
    if (it == f.end()) {
        return fallback;
    }
    std::wstring v = it->second;
    CharLowerBuffW(v.data(), (DWORD)v.size());
    if (v == L"true" || v == L"yes" || v == L"on") {
        return 1;
    }
    if (v == L"false" || v == L"no" || v == L"off") {
        return 0;
    }
    WCHAR* end = nullptr;
    long n = wcstol(v.c_str(), &end, 10);
    return v.empty() || *end ? fallback : (int)n;
}

// Reads exported settings over `s`. The groups are replaced if the file
// has any; other settings missing from the file keep their values.
bool ImportSettingsJson(std::wstring const& text,
                        EditSettings& s,
                        std::wstring& error) {
    wdj::JsonValue value{nullptr};
    if (!wdj::JsonValue::TryParse(text, value) ||
        value.ValueType() != wdj::JsonValueType::Object) {
        error = L"The file isn't a valid JSON object.";
        return false;
    }
    FlatSettings f;
    FlattenJson(value, L"", f);

    int recognized = 0;
    for (auto& page : DescribePages(s)) {
        for (auto& field : page.fields) {
            if (field.kind == FieldKind::Preset) {
                continue;
            }
            std::wstring key = field.key;
            if (!f.count(key)) {
                continue;
            }
            recognized++;
            switch (field.kind) {
                case FieldKind::Bool: {
                    auto* v = (bool*)field.value;
                    *v = (FlatInt(f, key, *v != field.inverted) != 0) !=
                         field.inverted;
                    break;
                }
                case FieldKind::Int: {
                    auto* v = (int*)field.value;
                    *v = (int)Clamp(FlatInt(f, key, *v), field.min, field.max);
                    break;
                }
                case FieldKind::Choice: {
                    std::wstring v = FlatString(f, key);
                    if (std::any_of(field.options.begin(), field.options.end(),
                                    [&](auto& o) { return o.first == v; })) {
                        *(std::wstring*)field.value = v;
                    }
                    break;
                }
                default:
                    *(std::wstring*)field.value = FlatString(f, key);
                    break;
            }
        }
    }

    std::vector<EditGroup> cats;
    for (int i = 0;; i++) {
        std::wstring cp = L"groups[" + std::to_wstring(i) + L"]";
        std::wstring name = FlatString(f, cp + L".name");
        if (name.empty()) {
            break;
        }
        EditGroup cat;
        cat.name = name;
        cat.appGrid = FlatString(f, cp + L".tileSize");
        cat.tileSize =
            ResolveTileSize(FlatString(f, cp + L".span"), cat.appGrid);
        cat.appGrid = FitGrid(cat.appGrid, cat.tileSize);
        cat.showAppNames = FlatInt(f, cp + L".hideAppNames", 0) == 0;
        cat.showLabel = FlatInt(f, cp + L".hideLabel", 0) == 0;
        cat.showBackground = FlatInt(f, cp + L".hideBackground", 0) == 0;
        cat.items = ReadItems(
            [&f](std::wstring const& key) { return FlatString(f, key); },
            cp + L".items");
        cats.push_back(std::move(cat));
    }

    if (recognized == 0 && cats.empty()) {
        error = L"The file doesn't contain Start Menu Groups settings.";
        return false;
    }
    if (!cats.empty()) {
        s.groups = std::move(cats);
    }
    return true;
}

bool WriteUtf8File(std::wstring const& path, std::wstring const& text) {
    int n = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(),
                                nullptr, 0, nullptr, nullptr);
    std::string utf8(n, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), utf8.data(),
                        n, nullptr, nullptr);
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(h, utf8.data(), (DWORD)utf8.size(), &written, nullptr);
    CloseHandle(h);
    return ok && written == utf8.size();
}

// Empty if the file can't be read or is larger than 1 MB.
std::wstring ReadUtf8File(std::wstring const& path) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                           nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return L"";
    }
    LARGE_INTEGER size{};
    std::string data;
    if (GetFileSizeEx(h, &size) && size.QuadPart > 0 &&
        size.QuadPart <= 1024 * 1024) {
        data.resize((size_t)size.QuadPart);
        DWORD read = 0;
        if (!ReadFile(h, data.data(), (DWORD)data.size(), &read, nullptr)) {
            data.clear();
        }
        data.resize(read);
    }
    CloseHandle(h);
    size_t skip = data.rfind("\xEF\xBB\xBF", 0) == 0 ? 3 : 0;  // BOM
    int n = MultiByteToWideChar(CP_UTF8, 0, data.data() + skip,
                                (int)(data.size() - skip), nullptr, 0);
    std::wstring text(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, data.data() + skip,
                        (int)(data.size() - skip), text.data(), n);
    return text;
}

constexpr COMDLG_FILTERSPEC kSettingsFileFilters[] = {
    {L"Start Menu Groups settings (JSON)", L"*.json"},
    {L"All files", L"*.*"},
};

void SettingsWindow::ExportSettings() {
    winrt::com_ptr<IFileSaveDialog> dlg;
    if (FAILED(CoCreateInstance(CLSID_FileSaveDialog, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dlg.put())))) {
        return;
    }
    dlg->SetFileTypes(ARRAYSIZE(kSettingsFileFilters), kSettingsFileFilters);
    dlg->SetDefaultExtension(L"json");
    dlg->SetFileName(L"start-menu-groups-settings.json");
    dlg->SetTitle(L"Export settings");
    FILEOPENDIALOGOPTIONS opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_OVERWRITEPROMPT | FOS_FORCEFILESYSTEM);
    if (FAILED(dlg->Show(m_hwnd)) || g_unloading) {
        return;
    }
    winrt::com_ptr<IShellItem> result;
    PWSTR path = nullptr;
    if (FAILED(dlg->GetResult(result.put())) ||
        FAILED(result->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
        return;
    }
    std::wstring file = path;
    CoTaskMemFree(path);
    if (WriteUtf8File(file, BuildSettingsJson(BuildRegEntries(m_model)))) {
        m_status.Text(L"Settings exported to " + file);
    } else {
        m_status.Text(L"Couldn't export the settings: writing " + file +
                      L" failed (error " + std::to_wstring(GetLastError()) +
                      L").");
    }
}

void SettingsWindow::ImportSettings() {
    winrt::com_ptr<IFileOpenDialog> dlg;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(dlg.put())))) {
        return;
    }
    dlg->SetFileTypes(ARRAYSIZE(kSettingsFileFilters), kSettingsFileFilters);
    dlg->SetTitle(L"Import settings");
    FILEOPENDIALOGOPTIONS opts = 0;
    dlg->GetOptions(&opts);
    dlg->SetOptions(opts | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);
    if (FAILED(dlg->Show(m_hwnd)) || g_unloading) {
        return;
    }
    winrt::com_ptr<IShellItem> result;
    PWSTR path = nullptr;
    if (FAILED(dlg->GetResult(result.put())) ||
        FAILED(result->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
        return;
    }
    std::wstring file = path;
    CoTaskMemFree(path);

    std::wstring text = ReadUtf8File(file);
    std::wstring error;
    EditSettings imported = m_model;
    if (text.empty()) {
        error = L"The file couldn't be read, or it's empty or larger than "
                L"1 MB.";
    } else if (ImportSettingsJson(text, imported, error)) {
        m_model = std::move(imported);
        m_selCat = m_model.groups.empty() ? -1 : 0;
        m_selItem = -1;
        m_selSub = -1;
        ShowPage(m_page.empty() ? L"board" : m_page);
        OnChanged();
        m_status.Text(L"Settings imported. Review them, then click Save to "
                      L"apply them.");
        return;
    }
    m_status.Text(L"Couldn't import the settings: " + error);
}

void SettingsWindow::OnChanged() {
    for (auto& update : m_resetUpdaters) {
        update();
    }
    m_dirty = BuildRegEntries(m_model) != m_savedEntries;
    UpdateFooter();
}

void SettingsWindow::UpdateFooter() {
    if (!m_saveButton) {
        return;
    }
    m_saveButton.IsEnabled(m_dirty && !m_saving);
    m_discardButton.IsEnabled(m_dirty && !m_saving);
    if (m_saving) {
        m_status.Text(L"Saving\u2026 confirm the administrator prompt.");
    } else if (m_dirty) {
        m_status.Text(L"Unsaved changes");
    } else if (m_status.Text().empty() ||
               m_status.Text() == L"Unsaved changes") {
        m_status.Text(L"Saving asks for administrator approval, since "
                      L"Windhawk keeps mod settings in a protected location.");
    }
}

void SettingsWindow::Save(bool closeAfter, bool overwrite) {
    if (m_saving) {
        return;
    }
    // Changed in Windhawk since this window read them: saving would replace
    // those changes, so ask first.
    if (!overwrite && ReadSettingsChangeTime() != m_loadedChangeTime) {
        ShowConfirmDialog(
            L"Settings changed in Windhawk",
            L"The mod's settings were changed in Windhawk after this window "
            L"opened. Saving replaces those changes with the ones made here. "
            L"To keep them instead, cancel and use Discard to load them.",
            L"Save anyway",
            [this, closeAfter] { Save(closeAfter, true); });
        return;
    }
    m_saving = true;
    m_closeAfterSave = closeAfter;
    UpdateFooter();
    auto entries = BuildRegEntries(m_model);
    HWND hwnd = m_hwnd;
    // The UAC prompt and the import block; keep the window's thread free.
    RunWorker([entries = std::move(entries), hwnd] {
        // The order arranged in Start is saved along (it's in m_model), so
        // it's removed first: Windhawk tells Start about the new settings as
        // soon as they're imported, and Start would put that order on top of
        // them again. Put back if the settings weren't saved.
        std::wstring startOrder = ReadStartOrderValue();
        if (!startOrder.empty()) {
            Wh_DeleteValue(kStartOrderValue);
        }
        auto* result = new SaveResult(WriteModSettings(entries, hwnd));
        if (!result->error.empty() && !startOrder.empty() &&
            ReadStartOrderValue().empty()) {
            Wh_SetStringValue(kStartOrderValue, startOrder.c_str());
        }
        if (!PostMessageW(hwnd, kSaveDoneMsg, 0, (LPARAM)result)) {
            delete result;
        }
    });
}

// The settings to edit, with the order arranged in Start: an order that
// isn't saved in the settings yet is an unsaved change (Save writes it).
void SettingsWindow::ReadModel() {
    std::vector<EditGroup> savedGroups;
    m_model = ReadEditSettings(&savedGroups);
    EditSettings saved = m_model;
    saved.groups = std::move(savedGroups);
    m_savedEntries = BuildRegEntries(saved);
    m_dirty = BuildRegEntries(m_model) != m_savedEntries;
}

// Back to the settings, in the order Start shows them.
void SettingsWindow::Discard() {
    m_loadedChangeTime = ReadSettingsChangeTime();
    ReadModel();
    m_selCat = m_model.groups.empty() ? -1 : 0;
    m_selItem = -1;
    m_selSub = -1;
    m_status.Text(L"");
    ShowPage(m_page.empty() ? L"board" : m_page);
    UpdateFooter();
}

void SettingsWindow::RequestClose() {
    if (m_saving) {
        return;
    }
    if (!m_dirty) {
        DestroyWindow(m_hwnd);
        return;
    }
    if (!m_closeDialog) {
        ShowUnsavedChangesDialog();
    }
}

// From "Edit" on an app in Start: the Groups page with the app selected and
// its settings scrolled to. Indices are those of the saved settings; ones
// that don't fit what's being edited just show the page.
void SettingsWindow::ShowItem(int group, int item) {
    // item -1: the group itself.
    if (group < 0 || group >= (int)m_model.groups.size() || item < -1 ||
        item >= (int)m_model.groups[group].items.size()) {
        return;
    }
    m_selCat = group;
    m_selItem = item;
    m_selSub = -1;
    m_boardSelCat = group;
    if (m_page != L"board") {
        for (size_t i = 0; i < m_pageTags.size(); i++) {
            if (m_pageTags[i] == L"board") {
                m_nav.SelectedIndex((int)i);  // builds the page
            }
        }
    } else {
        RebuildBoard();
        RefreshPalette();
        LoadBoardDetails();
    }
    // Once the page is laid out.
    Later([this] { Later([this] { ScrollToAppSettings(); }); });
}

// Scrolls the page down to the App settings, at the end of the page.
void SettingsWindow::ScrollToAppSettings() {
    if (m_page != L"board") {
        return;
    }
    auto sv = m_pageScroll.get();
    if (!sv) {
        return;
    }
    // To the end of the page, where the App settings are.
    try {
        sv.UpdateLayout();  // the card was just filled for the new app
        double end = sv.ScrollableHeight();
        sv.ChangeView(nullptr,
                      winrt::box_value(end).as<wf::IReference<double>>(),
                      nullptr, false);
    } catch (...) {
    }
}

// A question as a dialog inside the window (like "Save your changes?"):
// `action` (the accent button) or Cancel. One dialog at a time.
void SettingsWindow::ShowConfirmDialog(std::wstring const& title,
                                       std::wstring const& text,
                                       std::wstring const& action,
                                       std::function<void()> onAction) {
    if (m_closeDialog) {
        return;
    }
    try {
        xc::ContentDialog dialog;
        dialog.XamlRoot(m_root.XamlRoot());
        dialog.RequestedTheme(m_dark ? ElementTheme::Dark : ElementTheme::Light);
        dialog.Title(Box(title));
        dialog.Content(Box(text));
        dialog.PrimaryButtonText(action);
        dialog.CloseButtonText(L"Cancel");
        dialog.DefaultButton(xc::ContentDialogButton::Primary);
        m_closeDialog = dialog;
        dialog.ShowAsync().Completed(
            [this, onAction](
                wf::IAsyncOperation<xc::ContentDialogResult> const& op,
                wf::AsyncStatus status) {
                if (g_settingsWindow != this) {
                    return;  // the window is gone
                }
                m_closeDialog = nullptr;
                if (status == wf::AsyncStatus::Completed &&
                    op.GetResults() == xc::ContentDialogResult::Primary) {
                    Later(onAction);
                }
            });
    } catch (...) {
        Wh_Log(L"Showing a dialog failed: %08X", winrt::to_hresult());
        m_closeDialog = nullptr;
    }
}

// "Save your changes?" as a dialog inside the window, styled like the rest
// of it: Save, Don't save, Cancel.
void SettingsWindow::ShowUnsavedChangesDialog() {
    try {
        xc::ContentDialog dialog;
        dialog.XamlRoot(m_root.XamlRoot());
        dialog.RequestedTheme(m_dark ? ElementTheme::Dark : ElementTheme::Light);
        dialog.Title(Box(L"Save your changes?"));
        dialog.Content(Box(L"Your changes to the Start Menu Groups settings "
                           L"haven't been saved yet."));
        dialog.PrimaryButtonText(L"Save");
        dialog.SecondaryButtonText(L"Don't save");
        dialog.CloseButtonText(L"Cancel");
        dialog.DefaultButton(xc::ContentDialogButton::Primary);
        m_closeDialog = dialog;
        dialog.ShowAsync().Completed(
            [this](wf::IAsyncOperation<xc::ContentDialogResult> const& op,
                   wf::AsyncStatus status) {
                if (g_settingsWindow != this) {
                    return;  // the window is gone
                }
                m_closeDialog = nullptr;
                if (status != wf::AsyncStatus::Completed) {
                    return;
                }
                switch (op.GetResults()) {
                    case xc::ContentDialogResult::Primary:
                        Save(true);
                        break;
                    case xc::ContentDialogResult::Secondary:
                        DestroyWindow(m_hwnd);
                        break;
                    default:
                        break;  // Cancel: keep editing
                }
            });
    } catch (...) {
        Wh_Log(L"Showing the save dialog failed: %08X", winrt::to_hresult());
        m_closeDialog = nullptr;
    }
}

void RegisterSettingsWindowClass() {
    HMODULE self;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&SettingsWndProc, &self);
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = self;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = SettingsClassName();
    g_settingsClassRegistered = RegisterClassExW(&wc) != 0;
    if (!g_settingsClassRegistered) {
        Wh_Log(L"RegisterClassEx failed: %u", GetLastError());
    }
}

void UnregisterSettingsWindowClass() {
    if (!g_settingsClassRegistered) {
        return;
    }
    HMODULE self;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&SettingsWndProc, &self);
    UnregisterClassW(SettingsClassName(), self);
    g_settingsClassRegistered = false;
}

// Runs on the settings thread in File Explorer.
void OpenSettingsWindow(int group, int item) {
    if (g_settingsWindow && IsWindow(g_settingsWindow->Hwnd())) {
        HWND h = g_settingsWindow->Hwnd();
        if (IsIconic(h)) {
            ShowWindow(h, SW_RESTORE);
        }
        SetForegroundWindow(h);
        g_settingsWindow->ShowItem(group, item);
        return;
    }
    if (!g_settingsClassRegistered) {
        return;
    }
    auto* w = new SettingsWindow();
    g_settingsWindow = w;
    if (!w->Create()) {
        // If the window existed, destroying it already deleted the object
        // and cleared g_settingsWindow (WM_NCDESTROY). Otherwise free it.
        if (g_settingsWindow == w) {
            delete w;
        }
        g_settingsWindow = nullptr;
        return;
    }
    w->ShowItem(group, item);
    // The Start apps (for the picker, the app icons and the Start apps
    // panel) are listed in the background, the window shows right away.
    if (g_pickerApps.empty() && !g_pickerAppsLoading) {
        g_pickerAppsLoading = true;
        HWND hwnd = w->Hwnd();
        RunWorker([hwnd] {
            auto apps =
                std::make_unique<std::vector<AppEntry>>(EnumerateStartApps());
            std::sort(apps->begin(), apps->end(), [](auto& a, auto& b) {
                return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
            });
            if (PostMessageW(hwnd, kAppsLoadedMsg, 0, (LPARAM)apps.get())) {
                apps.release();
            } else {
                g_pickerAppsLoading = false;  // the window is gone
            }
        });
    }
}

void CloseSettingsWindow() {
    if (g_settingsWindow && IsWindow(g_settingsWindow->Hwnd())) {
        DestroyWindow(g_settingsWindow->Hwnd());
    }
    g_settingsWindow = nullptr;
}

// --- Settings host in File Explorer ----------------------------------------
//
// A message-only window on a dedicated STA thread in explorer.exe. Start finds
// it by class name and posts kOpenSettingsMsg; the window is then created on
// this thread, whose message loop also feeds the island its keyboard input.

constexpr UINT kOpenSettingsMsg = WM_APP + 20;
HANDLE g_hostThread = nullptr;
HANDLE g_hostReady = nullptr;  // set once the thread has its message queue
DWORD g_hostThreadId = 0;

PCWSTR HostClassName() {
    static std::wstring name =
        std::wstring(L"WindhawkSettingsHost_") + WH_MOD_ID;
    return name.c_str();
}

// Whether `id` (an app ID) is one of the Start apps in the user's groups,
// given by app ID or by name. Pin / unpin requests are only done for those:
// the host window takes messages from Start (a lower integrity level), so it
// takes them from other programs too, and Windows doesn't let programs pin
// apps by themselves.
bool IsAppInGroups(std::wstring const& id) {
    std::vector<std::wstring> apps;
    {
        std::lock_guard lock(g_settingsMutex);
        for (auto const& g : g_settings.groups) {
            for (auto const& item : g.items) {
                if (item.type == L"app" && !item.app.empty()) {
                    apps.push_back(item.app);
                }
                for (auto const& child : item.children) {
                    if (child.type == L"app" && !child.app.empty()) {
                        apps.push_back(child.app);
                    }
                }
            }
        }
    }
    auto listed = [&](std::wstring const& s) {
        return std::any_of(apps.begin(), apps.end(), [&](auto const& a) {
            return _wcsicmp(a.c_str(), s.c_str()) == 0;
        });
    };
    if (listed(id)) {
        return true;
    }
    for (auto const& app : EnumerateStartApps()) {
        if (_wcsicmp(app.id.c_str(), id.c_str()) == 0) {
            return listed(app.name);
        }
    }
    return false;
}

LRESULT CALLBACK HostWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    // Pin / unpin an app on the taskbar, for Start (see InvokeTaskbarVerb).
    // Only these two verbs, and only for apps in the user's groups.
    if (msg == WM_COPYDATA) {
        auto* cds = (COPYDATASTRUCT const*)lp;
        if (!cds || cds->dwData != kTaskbarVerbData || !cds->lpData ||
            cds->cbData > 4096) {
            return FALSE;
        }
        std::wstring payload((PCWSTR)cds->lpData,
                             cds->cbData / sizeof(WCHAR));
        size_t sep = payload.find(L'\n');
        if (sep == std::wstring::npos) {
            return FALSE;
        }
        std::string verb(payload.begin(), payload.begin() + sep);
        std::wstring id = payload.substr(sep + 1);
        if (!IsTaskbarVerb(verb) || id.empty()) {
            return FALSE;
        }
        RunWorker([id, verb] {
            if (IsAppInGroups(id)) {
                InvokeAppShellVerb(id, verb);
            } else {
                Wh_Log(L"Ignored %S for %s: not in the groups", verb.c_str(),
                       id.c_str());
            }
        });
        return TRUE;
    }
    if (msg == kOpenSettingsMsg) {
        try {
            OpenSettingsWindow((int)wp - 1, (int)lp - 1);
        } catch (...) {
            Wh_Log(L"Opening the settings window failed: %08X",
                   winrt::to_hresult());
        }
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wp, lp);
}

DWORD WINAPI SettingsHostThread(LPVOID) {
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // The thread's message queue, so StopSettingsHost can post WM_QUIT.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_hostReady);

    HMODULE self = ThisModule();
    RegisterSettingsWindowClass();
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = HostWndProc;
    wc.hInstance = self;
    wc.lpszClassName = HostClassName();
    RegisterClassExW(&wc);

    // Only one File Explorer process hosts the window.
    HWND host = nullptr;
    if (!g_unloading &&
        !FindWindowExW(HWND_MESSAGE, nullptr, HostClassName(), nullptr)) {
        host = CreateWindowExW(0, HostClassName(), L"", 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, self, nullptr);
    }
    if (host) {
        // Start runs at a lower integrity level; let its requests through.
        ChangeWindowMessageFilterEx(host, kOpenSettingsMsg, MSGFLT_ALLOW,
                                    nullptr);
        ChangeWindowMessageFilterEx(host, WM_COPYDATA, MSGFLT_ALLOW, nullptr);
        Wh_Log(L"Settings host ready");
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (g_settingsWindow && g_settingsWindow->PreTranslate(&msg)) {
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    CloseSettingsWindow();
    if (host) {
        DestroyWindow(host);
    }
    UnregisterClassW(HostClassName(), self);
    UnregisterSettingsWindowClass();
    DestroyWindowIcons();
    g_settingsIcons.clear();
    if (g_xamlManager) {
        try {
            g_xamlManager.Close();
        } catch (...) {
        }
        g_xamlManager = nullptr;
    }
    g_pickerApps.clear();
    winrt::uninit_apartment();
    return 0;
}

void StartSettingsHost() {
    g_hostReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hostThread = CreateThread(nullptr, 0, SettingsHostThread, nullptr, 0,
                                &g_hostThreadId);
}

// Ends the settings thread and waits for it (no time limit).
void StopSettingsHost() {
    if (g_hostThread) {
        // Its queue must exist for WM_QUIT to be posted.
        HANDLE waits[] = {g_hostReady, g_hostThread};
        WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        PostThreadMessageW(g_hostThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hostThread, INFINITE);
        CloseHandle(g_hostThread);
        g_hostThread = nullptr;
        g_hostThreadId = 0;
    }
    if (g_hostReady) {
        CloseHandle(g_hostReady);
        g_hostReady = nullptr;
    }
}

// The thread showing the "settings window isn't available" message, to close
// it if the mod unloads meanwhile.
std::atomic<DWORD> g_infoBoxThreadId{0};

void CloseInfoBox() {
    if (DWORD tid = g_infoBoxThreadId) {
        EnumThreadWindows(
            tid,
            [](HWND hwnd, LPARAM) -> BOOL {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                return TRUE;
            },
            0);
    }
}

// Called in Start (context menu): ask File Explorer to show the window.
void RequestSettingsWindow(int group, int item) {
    HWND host = FindWindowExW(HWND_MESSAGE, nullptr, HostClassName(), nullptr);
    if (!host) {
        Wh_Log(L"Settings host not found in File Explorer");
        // Not on Start's UI thread, which a message box would block.
        RunWorker([] {
            g_infoBoxThreadId = GetCurrentThreadId();
            if (!g_unloading) {
                MessageBoxW(nullptr,
                            L"The settings window is provided by this mod's "
                            L"File Explorer part, which isn't running.\n\n"
                            L"Make sure the mod is enabled and explorer.exe "
                            L"isn't excluded in its Advanced settings in "
                            L"Windhawk.",
                            L"Start Menu Groups",
                            MB_ICONINFORMATION | MB_SETFOREGROUND);
            }
            g_infoBoxThreadId = 0;
        });
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(host, &pid);
    // Start is in the foreground and may hand that right on to Explorer.
    AllowSetForegroundWindow(pid);
    // The app to select: group and item index + 1 (0 = none).
    PostMessageW(host, kOpenSettingsMsg, (WPARAM)(group + 1),
                 (LPARAM)(item + 1));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    LoadSettings();

    WCHAR exe[MAX_PATH];
    GetModuleFileNameW(nullptr, exe, ARRAYSIZE(exe));
    g_isExplorer = _wcsicmp(PathFindFileNameW(exe), L"explorer.exe") == 0;
    if (g_isExplorer) {
        // File Explorer only hosts the settings window.
        StartSettingsHost();
        return TRUE;
    }

    // Start's window, when Start (re)creates it.
    HMODULE user32 =
        LoadLibraryExW(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32) {
        if (auto p = (CreateWindowInBand_t)GetProcAddress(
                user32, "CreateWindowInBand")) {
            WindhawkUtils::SetFunctionHook(p, CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }
        if (auto p = (CreateWindowInBandEx_t)GetProcAddress(
                user32, "CreateWindowInBandEx")) {
            WindhawkUtils::SetFunctionHook(p, CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isExplorer) {
        return;
    }
    // Start is already running: get on its UI thread now.
    if (HWND coreWnd = FindStartWindow()) {
        RunFromWindowThread(
            coreWnd, [](PVOID p) { InstallStartUi((HWND)p); }, coreWnd);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    SetEvent(g_stopEvent);
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;
    SetEvent(g_stopEvent);
    CloseInfoBox();
    if (g_isExplorer) {
        StopSettingsHost();
    }
    // No worker posts to Start's UI thread any more (g_unloading), and none
    // is left running mod code.
    JoinWorkers();
    if (HWND wnd = g_uiWnd) {
        // On Start's UI thread, synchronously: everything the mod added to
        // Start is removed, and nothing of it stays queued there.
        SendMessageW(wnd, kUninstallMsg, 0, 0);
    }
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    if (g_isExplorer) {
        return;  // the settings window reads them when it opens
    }
    PostToUi([] {
        std::wstring placement;
        {
            std::lock_guard lock(g_settingsMutex);
            placement = g_settings.placement;
        }
        // A new "Where to show the groups": attached again, there.
        if (g_attach.host && g_attach.placement != placement) {
            Wh_Log(L"Moving the groups: %s", placement.c_str());
            Detach();
            RequestScan();
        }
        ReloadAsync();
    });
}

// ==WindhawkMod==
// @id              taskbar-folder-hover-tray
// @name            Taskbar Folder Hover Tray
// @description     Adds folder shortcut buttons flush inside the Windows 11 taskbar app icons. Hovering one instantly opens a grid of the folder's contents that you can move into and click.
// @version         2.0
// @author          Kiploom
// @github          https://github.com/Kiploom
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid -lgdi32 -lgdiplus -lcomctl32 -ldwmapi -luser32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Folder Hover Tray

Adds one or more **folder shortcut buttons** directly into the Windows 11 taskbar,
sitting flush with your pinned and running app icons. **Hover** a button and a grid
of that folder's contents appears instantly above the taskbar - move your mouse
straight up and click an item. No clicking the taskbar first.

Windows 11 only.

![Hover tray with cascading subfolders](https://raw.githubusercontent.com/Kiploom/images/main/taskbar-folder-hover-tray-cascade.png)

![Folder grid above the taskbar](https://raw.githubusercontent.com/Kiploom/images/main/taskbar-folder-hover-tray-grid.png)

## Features

- Buttons are real taskbar items - pin, unpin, drag to reorder, and they collapse
  into overflow like any other taskbar icon.
- The hover grid opens with **zero delay** - folder contents and icons are scanned
  and cached on a background thread.
- A "corridor" between the button and the grid keeps the grid open while you move
  your mouse up to it, so there is no dead zone.
- Subfolders cascade: rest on a subfolder and its own grid opens beside the first
  one, as deep as you like. **Maximum folder depth** caps the nesting - `-1` is
  unlimited, `0` stops subfolders from opening on hover, `1` allows one level.
- Subfolders are listed first in every grid and carry a folder badge in the
  accent colour on the corner of their icon, so what cascades is obvious at a
  glance. Folder shortcuts (`.lnk` pointing at a folder) cascade the same way.
- Set each button's icon to an **`.ico` file** or an **`app.exe,0`** / `.dll,N`
  icon resource - those are the specs Windows can turn into a real shortcut icon.
  An emoji or a `.png` is accepted in the field but falls back to the standard
  folder glyph on the button, same as leaving it empty.
- Left click an item to open it, right click for the full Windows shell context menu.
- Folder buttons are injected on the primary taskbar and every secondary-monitor taskbar.

## Adding folders

**Folders are not configured on the settings page.** They live in the
**Taskbar Folders** window instead, which you can open in either of two ways:

- **Right click a folder button on the taskbar** and choose **Manage folders...**.
  The buttons are real taskbar items now, so that menu is Windows' own jump list —
  the entry gets there through the folder's AppUserModelID rather than being drawn
  by this mod. **Unpin from taskbar** sits on the same menu, for free.
- Or right click any folder, file or the Desktop background, pick **Show more
  options** if you get the short Windows 11 menu, then
  **Taskbar Folders → Manage folders...**. This one works even when nothing is
  pinned yet, so it is the way in from a standing start.

The quickest way to add one in the first place is straight from Explorer:
right click any folder in Explorer or on the Desktop, pick **Show more
options** (or press Shift+F10 to go straight there), and choose
**Taskbar Folders -> Pin**. It becomes a button immediately, taking its name
and its custom icon (Properties > Customize > Change Icon) from the folder
itself. That same menu appears on files and shortcuts too — use it to move,
copy, or make a shortcut into a folder that already has a button.

The entry is on the classic context menu, the one behind **Show more
options**, and not on the short Windows 11 menu that opens first — that one is
XAML and takes no menu items from a mod like this. If you would rather it were
one click away, a mod such as **explorer-context-menu-classic** makes the
classic menu the default one.

If you would rather the mod stayed off the Explorer/Desktop menus, turn off
**Explorer right-click menu** in the settings; with it off, Pin / Move / Copy
disappear from that menu, leaving only **Manage folders...** — kept there on
purpose, since it is the only remaining way to open the manager once nothing
is pinned.

### The Taskbar Folders window

Folders are listed under two headings — **On the taskbar** and **Not pinned** —
and sorted by name within each. The window follows your Windows app theme, so it
is dark when the rest of Windows is. From there you can:

| Action | What it does |
|--------|--------------|
| **Add...** | Pick any folder and give it a name and icon |
| **Edit...** | Change the name, folder or icon, and tick or untick **Pinned to the taskbar**. Double-clicking a row does the same |
| **Remove** | Forget the entry entirely. The folder itself is never touched |
| **Pin** / **Unpin** | Moves the selected folder between the two sections. Unpinning takes the button off the taskbar **without deleting anything** — the entry keeps its name and icon and can be pinned again any time |

The list is not reorderable, and does not need to be: **button order is set by
dragging the buttons on the taskbar**, like any other pinned app. Renaming a
pinned folder unpins and re-pins its button under the new name, so it jumps to
the end of the taskbar and has to be dragged back into place.

Each entry has three fields:

| Field | Example | Notes |
|-------|---------|-------|
| Name  | `Apps` | Shown as the title at the top of the hover grid. Avoid naming a folder the same as (or a prefix of) a pinned app's name — the mod matches taskbar buttons to folders by their visible label, and a same-named app button can be mistaken for the folder's, suppressing its tooltip and opening the folder grid on hover instead |
| Folder | `%USERPROFILE%\Desktop` | Environment variables are expanded. `shell:` targets that map to a **filesystem folder** also work (e.g. `shell:Desktop`, `shell:Downloads`). Virtual namespaces such as Control Panel, Recycle Bin, or This PC are not supported — use [Taskbar Folder Menus](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-folder-menus.wh.cpp) for those. |
| Icon | `C:\icons\apps.ico` or `C:\Windows\explorer.exe,0` | Leave empty to use the folder's own icon. An emoji or `.png` is accepted here but shows as the standard folder glyph on the button |

**Open** next to the folder box opens whatever path is currently typed, so a
path can be checked before it is saved.

A good setup is to make a folder somewhere, fill it with shortcuts to the apps
you want grouped, and point a button at it.

A folder can only appear once. If it is already in the list as an unpinned
draft, pinning it again re-uses that entry rather than creating a second one.

### Why folders are not on the settings page

Short version: **Windhawk gives a mod no way to change its own settings**, so a
folder list living there could only ever be edited by hand, and nothing the mod
does — pinning, unpinning, renaming, reordering — could ever be written back
into it.

The longer version, because this trips people up:

A Windhawk mod gets two separate stores. **Settings** is the list you see on
this page; the mod API can only *read* it (`Wh_GetStringSetting` and friends).
**Storage** is a private per-mod area the mod can freely read and write
(`Wh_SetStringValue`). There is no supported call anywhere that writes a
setting.

That is a deliberate design choice, and Windows enforces it: Windhawk keeps mod
settings in `HKLM\SOFTWARE\Windhawk\Engine\Mods\...`, a registry key that grants
standard users **read access only**. This mod runs inside `explorer.exe`, which
is not elevated. Any attempt to open that key for writing fails outright with
access denied — it is not a matter of finding the right API.

So a settings-page folder list leaves two bad options:

1. **Two lists that never agree.** Folders added on the settings page and
   folders pinned from Explorer live in different places. Pin something and it
   does not appear on the settings page; unpin something configured there and
   the mod cannot remove the row, only learn to ignore it. Every one of those
   mismatches is a bug report waiting to happen.
2. **A UAC prompt for every change.** The write *can* be forced through by
   handing a `.reg` file to an elevated `reg.exe`. It works. It also means a
   Windows administrator prompt every single time you pin or unpin a folder,
   which is an absurd price for what is really just bookkeeping — and a scary
   one, since nothing about adding a folder shortcut should look like it needs
   admin rights.

Keeping the whole list in the mod's own Storage avoids both. One list, one
source of truth, edits apply instantly, and nothing ever asks for elevation.
The trade-off is that it needs its own window instead of riding along on the
settings page Windhawk already draws — hence **Taskbar Folders**.

## How it is positioned

It isn't. Each folder button is a **real pinned taskbar item**, so Windows creates,
positions and animates it exactly like any pinned app.

Under the hood each folder gets a shortcut in your Start Menu carrying its own
AppUserModelID, and that shortcut is pinned with the shell's ordinary
"pin to taskbar" verb. Everything else follows from the buttons being genuine:

- **Drag them** to reorder, like any other taskbar item. The order is the
  taskbar's, so it survives restarts and is not stored by this mod — but it
  is not preserved across a disable/enable or a settings-page save either,
  since those unpin and later re-pin every button at the end of the strip.
- **Right click** gives the normal Windows menu, including
  **Unpin from taskbar** and a **Manage folders...** entry this mod publishes to
  the folder's own jump list. Unpinning keeps the entry in the Taskbar Folders
  window as a draft, so its name and icon are not lost.
- They collapse into the overflow button when the taskbar fills up.
- Every animation is Windows' own.

## What this writes, and what happens when you disable it

Because the buttons are genuine pinned items, the mod cannot keep its state
entirely to itself. For each folder on the taskbar it writes:

- a shortcut in `%AppData%\Microsoft\Windows\Start Menu\Programs\Taskbar
  Folder Hover Tray`, carrying the folder's AppUserModelID and icon,
- the taskbar pin itself, made with the shell's ordinary "pin to taskbar" verb —
  which is the shell writing its own pinned-items folder and `Taskband` key, the
  same as pinning any app by hand,
- a jump list for that AppUserModelID, holding the **Manage folders...** task.

The folder list — names, paths, icons, pinned or not — lives in the mod's own
private Windhawk storage, and nowhere else.

One visible side effect of the Start Menu shortcuts: Windows enumerates that
folder for **Start → All apps** and for Start/taskbar search, so each folder
button also shows up there under a **Taskbar Folder Hover Tray** group. That is
the price of a real pin — the shell resolves a taskbar pin through its Start
Menu shortcut — and it disappears with the shortcuts when the mod is disabled.

**Disabling or uninstalling the mod unpins every button it created** and deletes
their Start Menu shortcuts and their jump lists, so nothing it put on the taskbar
or in your Start Menu is left behind.

**The folder list survives.** Windhawk runs the same unload path for a real
disable, for a mod update, for a settings-page save and for an engine restart,
and cannot tell them apart, so wiping the list there would mean an update
silently deleted your folders. Instead the entries are kept and re-pinned the
next time the mod loads. Uninstalling drops them too — Windhawk deletes a mod's
storage itself when the mod is removed — and **Remove all folder buttons...** in
the Taskbar Folders window is the explicit, confirmed way to forget everything
without uninstalling.

While the mod is disabled, nothing of it runs, so nothing is left running or
hooked; while it is *enabled*, the pins and shortcuts above are live. Note that
the taskbar order is the taskbar's own, so the buttons come back at the end of
the strip rather than where you dragged them.

The unpin sweep runs from `Wh_ModUninit`, so it does not run on an unclean exit
(Explorer crashing or being killed, or a hard reboot, while the mod is still
enabled), and it cannot run at all if the mod was uninstalled while Explorer was
not running. If that happens, the leftovers are the shortcuts under
`%AppData%\Microsoft\Windows\Start Menu\Programs\Taskbar Folder Hover Tray` —
delete that folder and unpin any of its buttons still on the taskbar by hand. As
long as the mod is enabled it also sweeps on its own: every reconcile unpins any
of its buttons that are no longer in the folder list, so leftovers from an
unclean exit are cleaned up the next time it loads.

Earlier versions drew an overlay instead, seated in a gap carved by widening a
neighbouring icon's margin. It could never be exactly right: taskbar positions are
driven by compositor-thread animations that no other window can sample mid-flight,
so an overlay is always at least a frame behind. That whole approach, and the
~2,000 lines that chased it, is gone.

One thing to know: a button's **icon comes from the shortcut**, which means it has
to be a real icon resource. `.ico` files, `app.exe,0` / `.dll,N` resource specs and
a folder's own custom icon all work. An **emoji or `.png` icon falls back** to the
standard folder glyph - Windows shortcuts can't point their icon at either.

## If you already use Taskbar Folder Menus

[Taskbar Folder Menus](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-folder-menus.wh.cpp)
(`taskbar-folder-menus` by sb4ssman) is a solid alternative for browsing folders
from the taskbar. Both support configurable folder/shortcut buttons, `shell:`
targets and environment-variable expansion, subfolders that expand on hover,
shell context menus on items, and browsing folder contents without minimizing
windows. Pick based on placement and UI — you typically want one or the other,
not both.

| | Taskbar Folder Hover Tray (this) | Taskbar Folder Menus |
|---|---|---|
| Placement | Real taskbar buttons in the **app icon strip** (flush with pinned/running apps) | Next to the **system tray** / notification area |
| Open | **Hover** opens immediately | **Click** opens |
| UI | Custom-drawn **icon grid** popup | **Native Shell** cascading menus |
| Buttons | Native taskbar buttons; `.ico`/resource icons optional | Compact tray buttons with emoji labels and extensive tray grid layout options |

Choose **Taskbar Folder Menus** for tray-side buttons and native Shell menus.
Choose **this mod** for a hover-opened icon grid seated in the app icon strip.

## Known Conflicts

**Windows 11 Taskbar Styler** can restyle these buttons if one of its rules matches
`Button`. If a folder button suddenly looks wrong, check for a broad rule there.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- behavior:
  - openFolderOnClick: true
    $name: Click opens the folder
    $description: >-
      Left clicking a folder button opens the folder in File Explorer. Turn this
      off to make the buttons hover-only — the click is swallowed and the grid is
      the only way in.

  - explorerMenu: true
    $name: Explorer right-click menu
    $description: >-
      Adds a "Taskbar Folders" submenu to the classic Explorer and Desktop
      context menus — the ones behind "Show more options" / Shift+F10, not the
      short Windows 11 menu, which is XAML and takes no items from a mod. Use it
      to pin a folder straight to the taskbar, or to move, copy or make a
      shortcut into a folder that already has a button. Turn this off to drop
      Pin / Move / Copy from that submenu; "Manage folders..." stays either way,
      since it is the only way to reach the manager when nothing is pinned yet.

  - hoverDelayMs: 0
    $name: Hover delay (ms)
    $description: Delay before the grid appears. 0 is instant.

  - closeDelayMs: 250
    $name: Close delay (ms)
    $description: >-
      How long the grid stays open after the mouse leaves the button and the grid.
      Moving onto another taskbar icon counts as leaving.
  $name: Behavior
  $description: How the buttons respond to the mouse, and whether the mod touches Explorer at all.

- content:
  - includeSubfolders: true
    $name: Show subfolders
    $description: Include subfolders as entries in the grid.

  - maxFolderDepth: -1
    $name: Maximum folder depth
    $description: >-
      How many levels of subfolder menus can cascade when you hover a subfolder.
      -1 is unlimited, 0 means subfolders never open on hover, 1 allows one level
      of subfolders, and so on. Turn off "Show subfolders" to hide them entirely.

  - submenuDelayMs: 150
    $name: Subfolder open delay (ms)
    $description: >-
      How long to rest on a subfolder before its menu opens. A small delay stops
      menus from firing off while you sweep across the grid. 0 is instant.

  - submenuCloseDelayMs: 300
    $name: Subfolder close delay (ms)
    $description: >-
      How long a cascaded subfolder menu stays open after the mouse leaves the
      cell that opened it. Gives you time to move diagonally into the submenu.

  - showHidden: false
    $name: Show hidden items
    $description: Include hidden and system files.

  - showExtensions: false
    $name: Show file extensions
    $description: Shortcut (.lnk) extensions are always hidden.

  - sortBy: name
    $name: Sort by
    $description: >-
      Folders always come first so the entries that cascade stay together at the
      top. This orders the items within each group.
    $options:
    - name: Name
    - modified: Date modified (newest first)

  - maxItems: 64
    $name: Maximum items
    $description: >-
      Limit how many entries the grid shows and caches. The cache also caps to
      roughly what can fit on screen (columns × rows, tighter for large icons)
      and a ~64 MB bitmap budget, so very large maxItems values do not keep
      hundreds of oversized icons resident. 0 uses that automatic on-screen cap.

  - columns: 0
    $name: Grid columns
    $description: 0 chooses a square-ish grid automatically.
  $name: Content
  $description: What the hover grid lists, and how much of it.

- appearance:
  - itemSize: medium
    $name: Item size
    $description: >-
      Preset size for each item's icon and grid cell. Every step scales both the
      icon and the overall tray size.
    $options:
    - smallest: Smallest
    - tiny: Tiny
    - xsmall: Extra small
    - small: Small
    - medium: Medium
    - large: Large

  - showLabels: true
    $name: Show item labels
    $description: >-
      Turn off for an icons-only grid. Icons are centred in the cell when labels
      are hidden.

  - fontSize: 13
    $name: Item label size (px)

  - itemFontWeight: regular
    $name: Item label weight
    $options:
    - regular: Regular
    - bold: Bold

  - showTitle: true
    $name: Show folder title
    $description: >-
      The folder name at the top of the main hover grid. Subfolder menus never
      show one.

  - titleFontSize: 14
    $name: Folder title size (px)
    $description: Size of the name shown at the top of the main hover grid.

  - titleFontWeight: bold
    $name: Folder title weight
    $options:
    - regular: Regular
    - bold: Bold

  - titleAlign: center
    $name: Folder title position
    $description: >-
      Horizontal position of the folder name on the main hover grid. Subfolder
      menus never show a title.
    $options:
    - left: Left
    - center: Center
    - right: Right

  - roundedCorners: true
    $name: Rounded grid corners
    $description: Rounds the corners of the hover grid. Turn off for square ones.

  - popupTheme: system
    $name: Grid theme
    $description: >-
      Background and text colours of the hover grid. Windows default follows the
      system app theme.
    $options:
    - system: Windows default
    - light: Light
    - dark: Dark

  - panelOpacity: 10
    $name: Grid opacity (%)
    $description: >-
      A contrasting outline is added behind the text as the background fades out.
      At 0 the background is invisible - icons and labels float straight over the
      desktop - but the grid still takes clicks rather than passing them through
      to whatever is behind it.

  - blurType: acrylic
    $name: Blur type
    $description: >-
      Gaussian captures the screen behind the grid and blurs it ourselves - a
      real, adjustable blur radius, but it costs a screen capture and a resample
      every ~32ms to track a moving background live. Acrylic uses Windows' own
      GPU-composited blur-behind instead - much cheaper (no capture, no per-frame
      resample, updates live for free) but the blur radius itself is fixed by
      Windows, not adjustable; the strength setting below controls the tint's
      opacity instead of the blur radius. None skips blur entirely - cheaper
      than either, same as setting the strength below to 0.
    $options:
    - acrylic: Acrylic (fixed radius, cheaper)
    - gaussian: Gaussian (adjustable, more expensive)
    - none: None

  - blurStrength: 10
    $name: Blur strength (%)
    $description: >-
      Transparent blur layered behind the grid, over whatever is on screen.
      Independent of the grid opacity above. Ignored when blur type above is
      None; 0 has the same effect. For Gaussian this is the blur radius; for
      Acrylic (fixed blur radius) it is instead how opaque the tint over that
      blur is.

  - gapAbove: 8
    $name: Gap above taskbar (px)
    $description: Distance between the taskbar and the grid.
  $name: Appearance
  $description: How the hover grid looks.
*/
// ==/WindhawkModSettings==

#undef GetCurrentTime

#include <windhawk_utils.h>

#include <commctrl.h>
#include <commoncontrols.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <windowsx.h>

#include <gdiplus.h>
#include <objidl.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef SEE_MASK_ASYNCOK
#define SEE_MASK_ASYNCOK 0x00100000
#endif
#ifndef CMIC_MASK_ASYNCOK
#define CMIC_MASK_ASYNCOK SEE_MASK_ASYNCOK
#endif
#ifndef CMIC_MASK_PTINVOKE
#define CMIC_MASK_PTINVOKE 0x20000000
#endif

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Hosting;

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif
// Windows 10 2004+. Marks a window invisible to BitBlt/PrintWindow/desktop
// capture, so CaptureBlurredBackdrop can grab the screen behind our own grid
// window without that window's own pixels showing up in the shot - no need
// to hide it first.
#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

// Undocumented SetWindowCompositionAttribute accent policy - the same
// mechanism Windows itself uses for acrylic flyouts. Not in any public
// header, so declared by hand and loaded dynamically (see
// GetSetWindowCompositionAttribute).
enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
};
struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;  // 0xAABBGGRR
    DWORD AnimationId;
};
enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19,
};
struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};
using SetWindowCompositionAttributeFn =
    BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

SetWindowCompositionAttributeFn GetSetWindowCompositionAttribute() {
    static SetWindowCompositionAttributeFn fn =
        reinterpret_cast<SetWindowCompositionAttributeFn>(GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));
    return fn;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

struct FolderEntry {
    std::wstring name;
    std::wstring path;          // Expanded settings path (may be shell:...)
    std::wstring resolvedPath;  // Filesystem path; shell: filled on an STA later
    std::wstring icon;
    bool likelyRemote = false;  // IsLikelyRemotePath(resolvedPath), after resolve
    bool resolveFailed = false; // shell: that does not map to a filesystem folder
    // True for folders added via the Explorer/Desktop "Pin to Taskbar" action
    // (mod storage), false for folders configured in the Settings UI. Only
    // pinned folders can be removed with a taskbar right-click.
    bool pinned = false;
    // Copied from the store. Ties this entry to its real taskbar item: the
    // shortcut's AppUserModelID is built from it, so it is how a taskbar button
    // is resolved back to the folder behind it.
    std::wstring pinId;
};

// The one rounded radius, at 96 DPI; ScaleForPopup takes it from there.
constexpr int kRoundedCornerRadius = 8;

enum class SortMode { Name, Modified };
enum class BlurType { None, Gaussian, Acrylic };

struct Settings {
    std::vector<FolderEntry> folders;
    int hoverDelayMs = 0;
    int closeDelayMs = 250;
    int columns = 0;
    int maxItems = 64;
    bool includeSubfolders = true;
    int maxFolderDepth = -1;
    int submenuDelayMs = 150;
    int submenuCloseDelayMs = 300;
    bool showHidden = false;
    bool showExtensions = false;
    SortMode sortBy = SortMode::Name;
    // Derived from itemSize in LoadSettings(); not settings-bound directly.
    int cellWidth = 92;
    int cellHeight = 88;
    int iconSize = 32;
    bool showLabels = true;
    int fontSize = 13;
    // LOGFONT lfWeight values. See MakePopupFont.
    int itemFontWeight = FW_NORMAL;
    bool showTitle = true;
    int titleFontSize = 14;
    int titleFontWeight = FW_BOLD;
    std::wstring titleAlign = L"center";
    // Kept as a pixel radius rather than a bool because three separate places
    // need the number: the window region, the blur region and the painted
    // background. The setting itself is just on or off.
    int cornerRadius = kRoundedCornerRadius;
    // -1 follows the system app theme; 0 forces light, 1 forces dark.
    int popupThemeOverride = -1;
    int panelOpacity = 10;
    BlurType blurType = BlurType::Acrylic;
    int blurStrength = 10;
    int gapAbove = 8;
    bool openFolderOnClick = true;
    // Read from an Explorer window thread inside the TrackPopupMenuEx hook
    // while LoadSettings may be writing it on another.
    std::atomic<bool> explorerMenu{true};
};

Settings g_settings;

// Guards FolderEntry::resolvedPath / likelyRemote fills from STA resolve helpers
// while UI and the scan worker may read them.
std::mutex g_foldersMutex;

std::atomic<bool> g_unloading{false};

////////////////////////////////////////////////////////////////////////////////
// Small helpers

// Returns this mod's DLL HINSTANCE. GetModuleHandle(nullptr) would return
// explorer.exe, which is wrong for RegisterClass/CreateWindowEx/UnregisterClass.
HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

// Registers one of the mod's window classes against the current DLL.
//
// Recompiling the mod unloads this DLL and loads a new copy, but a window
// class registered by the old one survives in the process with its
// lpfnWndProc still pointing at the freed image. Reusing it — which is what
// treating ERROR_CLASS_ALREADY_EXISTS as success amounts to — hands
// CreateWindowEx a dangling procedure and takes Explorer down with the first
// message. So a stale registration is torn down and replaced rather than
// adopted. Wh_ModUninit unregisters the classes for the same reason; this is
// the belt to that pair of braces, since an Explorer crash (or any unclean
// unload) skips the tidy path.
bool RegisterModClass(const WNDCLASSEXW& wc) {
    if (RegisterClassExW(&wc)) {
        return true;
    }
    if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW('%s') failed (error %u)", wc.lpszClassName,
               GetLastError());
        return false;
    }

    // Left behind by an earlier load of this mod. Unregistering fails while
    // any window of the class still exists, and there should be none.
    if (!UnregisterClassW(wc.lpszClassName, wc.hInstance)) {
        Wh_Log(L"stale class '%s' could not be unregistered (error %u)",
               wc.lpszClassName, GetLastError());
        return false;
    }
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"re-registering '%s' failed (error %u)", wc.lpszClassName,
               GetLastError());
        return false;
    }
    Wh_Log(L"replaced a stale '%s' class from a previous load",
           wc.lpszClassName);
    return true;
}

std::wstring ResolveFolderPath(const std::wstring& raw);
bool IsLikelyRemotePath(const std::wstring& p);
bool IsShellFolderPath(const std::wstring& path);
void ResolvePendingFolderEntries();
void ReloadAndRefreshUI();
void RequestReloadUI();

// Non-zero while a reload is executing. Wh_ModUninit waits this out: a reload
// parks the taskbar UI thread in a join that pumps sent messages, so uninit's
// own teardown send is dispatched re-entrantly and uninit would otherwise
// return — and let the image be unmapped — with the rest of ReloadAndRefreshUI
// still to run on that thread.
std::atomic<int> g_reloadInFlight{0};

std::wstring Trim(std::wstring s) {
    size_t b = s.find_first_not_of(L" \t\r\n");
    if (b == std::wstring::npos) {
        return L"";
    }
    size_t e = s.find_last_not_of(L" \t\r\n");
    return s.substr(b, e - b + 1);
}

std::wstring ExpandEnv(const std::wstring& s) {
    if (s.find(L'%') == std::wstring::npos) {
        return s;
    }
    DWORD needed = ExpandEnvironmentStringsW(s.c_str(), nullptr, 0);
    if (!needed) {
        return s;
    }
    std::wstring out(needed, L'\0');
    DWORD written = ExpandEnvironmentStringsW(s.c_str(), out.data(), needed);
    if (!written) {
        return s;
    }
    out.resize(written - 1);
    return out;
}

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

// Weight settings are stored as LOGFONT lfWeight values so MakePopupFont can
// hand them straight to the GDI font mapper.
int ParseFontWeight(const std::wstring& value, int fallback) {
    if (value == L"regular") {
        return FW_NORMAL;
    }
    if (value == L"bold") {
        return FW_BOLD;
    }
    return fallback;
}

std::wstring IconSpecFilePart(const std::wstring& spec);

// An icon setting is a file reference if it looks like a path or an
// "app.exe,<index>" resource spec, otherwise it is literal text to render (an
// emoji).
bool IconSettingIsFile(const std::wstring& icon) {
    if (icon.size() < 3) {
        return false;
    }
    if (icon.find(L'\\') != std::wstring::npos ||
        icon.find(L'/') != std::wstring::npos) {
        return true;
    }
    // Bare "explorer.exe,0" — no directory, no drive letter.
    if (IconSpecFilePart(icon) != icon) {
        return true;
    }
    return icon[1] == L':';
}

// 64-bit NTFS file id for a directory, stable across in-place renames; 0 on
// failure. Used to re-find a pinned folder after it gets renamed on disk.
uint64_t GetDirFileId(const std::wstring& path) {
    // CreateFileW on an offline share blocks for the network timeout. 0 is the
    // same "no rename recovery for this entry" BuildFolderEntry already gives
    // remote paths, so every caller — the edit dialog's Save, the Explorer pin
    // flow, FindRenamedSibling — degrades the same way.
    if (IsLikelyRemotePath(path)) {
        return 0;
    }
    HANDLE h = CreateFileW(path.c_str(), 0,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS,
                            nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return 0;
    }
    BY_HANDLE_FILE_INFORMATION info{};
    bool ok = GetFileInformationByHandle(h, &info);
    CloseHandle(h);
    if (!ok) {
        return 0;
    }
    return (static_cast<uint64_t>(info.nFileIndexHigh) << 32) | info.nFileIndexLow;
}

bool DirectoryExists(const std::wstring& path) {
    DWORD attrs = path.empty() ? INVALID_FILE_ATTRIBUTES
                               : GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES &&
           (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// A renamed folder keeps the same parent, so if oldPath no longer resolves,
// scan its parent for a subfolder whose file id still matches. Returns the
// new full path, or empty if no match.
// ponytail: 64-bit nFileIndex id, NTFS only. ReFS/64-bit-unstable volumes
// would need FILE_ID_INFO (128-bit) if that ever matters here.
constexpr int kRenameSearchBudget = 512;

std::wstring FindRenamedSibling(const std::wstring& oldPath, uint64_t fileId) {
    if (fileId == 0) {
        return L"";
    }
    size_t pos = oldPath.find_last_of(L"\\/");
    if (pos == std::wstring::npos) {
        return L"";
    }
    std::wstring parent = oldPath.substr(0, pos);
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW((parent + L"\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) {
        return L"";
    }
    std::wstring found;
    // One CreateFileW per subfolder, and this runs on Explorer's main thread
    // during Wh_ModInit and on the taskbar UI thread on every reload. A rename
    // hits within the first handful of candidates; the case that would sweep a
    // whole tree is a deleted pinned folder, which keeps its fileId and misses
    // forever. Bound the sweep rather than stall the taskbar on a parent with
    // thousands of subdirectories.
    int budget = kRenameSearchBudget;
    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
            wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }
        if (budget-- <= 0) {
            Wh_Log(L"Rename search for '%s' gave up after %d candidates under "
                   L"'%s'",
                   oldPath.c_str(), kRenameSearchBudget, parent.c_str());
            break;
        }
        std::wstring candidate = parent + L"\\" + fd.cFileName;
        if (GetDirFileId(candidate) == fileId) {
            found = candidate;
            break;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return found;
}

// A folder's own custom icon (Properties > Customize > Change Icon), read
// from its desktop.ini as the "path,index" spec the Icon setting already
// accepts. Empty when the folder has no custom icon.
std::wstring ReadFolderCustomIcon(const std::wstring& folderPath) {
    // GetPrivateProfileStringW opens the file, so an offline share blocks for
    // the network timeout — on the Explorer UI thread in the pin flow. An empty
    // spec is a valid entry: ResolveIcon falls back to the folder's own
    // shell icon.
    if (IsLikelyRemotePath(folderPath)) {
        return L"";
    }
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    WCHAR buf[1024]{};
    if (GetPrivateProfileStringW(L".ShellClassInfo", L"IconResource", L"", buf,
                                 ARRAYSIZE(buf), iniPath.c_str()) > 0) {
        return ExpandEnv(Trim(buf));
    }
    if (GetPrivateProfileStringW(L".ShellClassInfo", L"IconFile", L"", buf,
                                 ARRAYSIZE(buf), iniPath.c_str()) == 0) {
        return L"";
    }
    std::wstring file = ExpandEnv(Trim(buf));
    int index =
        GetPrivateProfileIntW(L".ShellClassInfo", L"IconIndex", 0,
                              iniPath.c_str());
    return file + L"," + std::to_wstring(index);
}

////////////////////////////////////////////////////////////////////////////////
// Folder store
//
// The single source of truth for every folder button, kept in the mod's own
// writable storage. Settings deliberately holds no folder list: a mod cannot
// write to its own Windhawk settings without administrator rights, so a list
// living there could never be edited from the manager window, from an Explorer
// right-click pin, or from an unpin on the taskbar.
//
// A compact array, no holes, because the order is the taskbar button order:
//   entryCount, entry[i].path / .name / .icon / .id / .pinned
// An unpinned entry is a draft — kept with its name and icon, just not shown
// on the taskbar, and still blocking a second copy of the same folder.
namespace FolderStore {

constexpr int kMaxEntries = 200;

// Read/modify/write runs on three threads: the manager thread, the taskbar UI
// thread (unpin) and an Explorer window thread (the context-menu pin). Write()
// rewrites the entries, deletes the tail, then updates entryCount, so an
// unlocked interleave can persist a truncated or mixed list.
//
// Recursive because the read-modify-write callers hold it across Read()+Write()
// and those lock it too. Lock order is g_foldersMutex then this one, which is
// what LoadFolders takes; nothing may hold this one across ReloadAndRefreshUI,
// since that reaches LoadFolders and would invert the order.
//
std::recursive_mutex g_mutex;

struct Entry {
    std::wstring path;
    std::wstring name;
    std::wstring icon;
    uint64_t fileId = 0;
    bool pinned = true;
    // Stable per-entry identity for the real taskbar pin. Becomes the shortcut's
    // AppUserModelID, which is what keeps the item from merging into the File
    // Explorer group (every pin shortcut targets explorer.exe — see Pins) and is
    // how a taskbar button is recognised as ours later. Generated once, on first
    // write, and never reused: reusing one would let a deleted entry's pin be
    // adopted by an unrelated folder.
    std::wstring pinId;
    // True once this entry's shortcut has actually been pinned. Without it the
    // reconcile cannot tell "pinned=1 and not on the taskbar yet" from "pinned=1
    // and the user just unpinned it from the taskbar" — the first wants pinning,
    // the second must not be re-pinned or native Unpin would look broken.
    bool pinApplied = false;
};

std::wstring GetString(PCWSTR format, int index) {
    WCHAR key[64];
    swprintf(key, ARRAYSIZE(key), format, index);
    WCHAR buf[1024]{};
    Wh_GetStringValue(key, buf, ARRAYSIZE(buf));
    return Trim(buf);
}

std::vector<Entry> Read() {
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    std::vector<Entry> entries;
    int count = Wh_GetIntValue(L"entryCount", 0);
    if (count > kMaxEntries) {
        count = kMaxEntries;
    }
    for (int i = 0; i < count; i++) {
        Entry entry;
        entry.path = GetString(L"entry[%d].path", i);
        if (entry.path.empty()) {
            continue;
        }
        entry.name = GetString(L"entry[%d].name", i);
        entry.icon = GetString(L"entry[%d].icon", i);
        std::wstring id = GetString(L"entry[%d].id", i);
        entry.fileId = id.empty() ? 0 : wcstoull(id.c_str(), nullptr, 16);

        entry.pinId = GetString(L"entry[%d].pinId", i);

        WCHAR key[64];
        swprintf(key, ARRAYSIZE(key), L"entry[%d].pinned", i);
        entry.pinned = Wh_GetIntValue(key, 1) != 0;
        swprintf(key, ARRAYSIZE(key), L"entry[%d].pinApplied", i);
        entry.pinApplied = Wh_GetIntValue(key, 0) != 0;
        entries.push_back(std::move(entry));
    }
    return entries;
}

void Write(const std::vector<Entry>& entries) {
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    int previous = Wh_GetIntValue(L"entryCount", 0);
    int count = (int)entries.size();
    if (count > kMaxEntries) {
        count = kMaxEntries;
    }

    for (int i = 0; i < count; i++) {
        const Entry& entry = entries[i];
        WCHAR key[64];
        swprintf(key, ARRAYSIZE(key), L"entry[%d].path", i);
        Wh_SetStringValue(key, entry.path.c_str());
        swprintf(key, ARRAYSIZE(key), L"entry[%d].name", i);
        Wh_SetStringValue(key, entry.name.c_str());
        swprintf(key, ARRAYSIZE(key), L"entry[%d].icon", i);
        Wh_SetStringValue(key, entry.icon.c_str());
        swprintf(key, ARRAYSIZE(key), L"entry[%d].id", i);
        WCHAR idBuf[20];
        swprintf(idBuf, ARRAYSIZE(idBuf), L"%llx",
                 (unsigned long long)entry.fileId);
        Wh_SetStringValue(key, idBuf);
        swprintf(key, ARRAYSIZE(key), L"entry[%d].pinId", i);
        Wh_SetStringValue(key, entry.pinId.c_str());
        swprintf(key, ARRAYSIZE(key), L"entry[%d].pinned", i);
        Wh_SetIntValue(key, entry.pinned ? 1 : 0);
        swprintf(key, ARRAYSIZE(key), L"entry[%d].pinApplied", i);
        Wh_SetIntValue(key, entry.pinApplied ? 1 : 0);
    }

    // The array shrank: drop the tail so a stale entry cannot reappear if it
    // later grows again.
    for (int i = count; i < previous && i < kMaxEntries; i++) {
        WCHAR key[64];
        for (PCWSTR field :
             {L"path", L"name", L"icon", L"id", L"pinId", L"pinned",
              L"pinApplied"}) {
            swprintf(key, ARRAYSIZE(key), L"entry[%d].%s", i, field);
            Wh_DeleteValue(key);
        }
    }

    Wh_SetIntValue(L"entryCount", count);
}

// The comparison key for a stored path: resolved to a filesystem path so that
// a shell: entry and its real folder count as the same thing.
std::wstring MatchKey(const std::wstring& path) {
    std::wstring key = ResolveFolderPath(ExpandEnv(path));
    return key.empty() ? ExpandEnv(path) : key;
}

// True if two stored paths name the same folder. Exposed so callers can find
// an entry by path rather than by a store index another thread may have
// shifted underneath them.
bool SamePath(const std::wstring& a, const std::wstring& b) {
    if (a.empty() || b.empty()) {
        return false;
    }
    return _wcsicmp(MatchKey(a).c_str(), MatchKey(b).c_str()) == 0;
}

// Index of the entry backing `path`, or -1 if none.
int IndexOfPath(const std::vector<Entry>& entries, const std::wstring& path) {
    std::wstring wanted = MatchKey(path);
    for (size_t i = 0; i < entries.size(); i++) {
        if (_wcsicmp(MatchKey(entries[i].path).c_str(), wanted.c_str()) == 0) {
            return (int)i;
        }
    }
    return -1;
}

}  // namespace FolderStore

////////////////////////////////////////////////////////////////////////////////
// Real taskbar pins
//
// Each pinned folder is a genuine pinned taskbar item, not a drawn overlay. That
// is what makes the animations exact and dragging work: Windows lays the button
// out and animates it, so there is nothing to chase.
//
// The shape below is not the obvious one, and each part of it was forced by
// measurement on 25H2 (build 26200) rather than chosen:
//
//   * The shortcut targets `explorer.exe <folder>`, never the folder itself. A
//     .lnk pointing at a bare folder is not pinnable at all — the shell answers
//     ERROR_NO_ASSOCIATION. Only the app-shaped form is accepted.
//
//   * Because every shortcut therefore points at explorer.exe, a unique
//     AppUserModelID per entry is load-bearing, not decorative: without it the
//     items collapse into the File Explorer group.
//
//   * Pinning goes through the documented shell verb (IContextMenu
//     "taskbarpin"), not IPinnedList3. The undocumented interface is not needed,
//     which removes a vtable whose slot order has shifted between Windows
//     versions before.
//
//   * All of it runs on a private STA thread. It must be an STA (the pin COM
//     class is ThreadingModel=Apartment with no marshaler), and it must not be a
//     thread dispatching an input-synchronous call — so RunFromWindowThread, which
//     arrives by SendMessage, cannot be used: every outgoing COM call from there
//     fails with RPC_E_CANTCALLOUT_ININPUTSYNCCALL having done nothing, which
//     looks exactly like a permissions refusal and is not one.

// Pumps the calling STA's message queue. Defined with the scan thread further
// down; forward-declared here so the pin worker (also an STA) can use it.
void PumpScanThreadMessages();

namespace Pins {

// Prefix shared by every AppUserModelID this mod creates. Recognising our own
// taskbar buttons later is a prefix test on this.
constexpr PCWSTR kAppIdPrefix = L"Kiploom.TaskbarFolderHoverTray.";
constexpr PCWSTR kPinSubDir = L"Taskbar Folder Hover Tray";
// Last-resort button icon: the standard Windows folder glyph.
constexpr PCWSTR kDefaultIconFile = L"%SystemRoot%\\system32\\imageres.dll";
constexpr int kDefaultIconIndex = 3;

// PKEY_AppUserModel_ID, spelled out so the mod needs neither INITGUID nor a link
// against propsys.
const PROPERTYKEY kPKEY_AppUserModel_ID = {
    {0x9F4C2855,
     0x9F79,
     0x4B39,
     {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
    5};

std::wstring AppIdFor(const std::wstring& pinId) {
    return std::wstring(kAppIdPrefix) + pinId;
}

bool IsOurAppId(const std::wstring& appId) {
    return appId.compare(0, wcslen(kAppIdPrefix), kAppIdPrefix) == 0;
}

// Where our own shortcuts live. Start Menu\Programs is not cosmetic: the shell
// builds shell:appsfolder from Start Menu shortcuts carrying an AUMID, and
// FavoritesResolve prunes pins that no longer resolve to one.
std::wstring SourceDir() {
    PWSTR programs = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_Programs, 0, nullptr, &programs)) ||
        !programs) {
        return L"";
    }
    std::wstring dir = programs;
    CoTaskMemFree(programs);
    return dir + L"\\" + kPinSubDir;
}

// Where the shell keeps a copy of every currently pinned item. Reading this is
// how a native "Unpin from taskbar" is noticed — the shell removes the item
// without telling the mod anything.
std::wstring PinnedDir() {
    PWSTR appData = nullptr;
    if (FAILED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr,
                                    &appData)) ||
        !appData) {
        return L"";
    }
    std::wstring dir = appData;
    CoTaskMemFree(appData);
    return dir +
           L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar";
}

// A filename that cannot upset the shell, derived from the folder's display
// name. The taskbar labels the button from the shortcut's file name, so this is
// what the user sees in the tooltip — keep it clean and keep it stable.
std::wstring SafeLeaf(const std::wstring& name) {
    std::wstring out;
    for (wchar_t c : name) {
        if (wcschr(L"\\/:*?\"<>|", c) || c < 0x20) {
            continue;
        }
        out += c;
    }
    out = Trim(out);
    // Trailing dots and spaces are legal in the string but not in a filename.
    while (!out.empty() && (out.back() == L'.' || out.back() == L' ')) {
        out.pop_back();
    }
    if (out.empty()) {
        out = L"Folder";
    }
    if (out.size() > 64) {
        out.resize(64);
    }
    return out;
}

// `taken` is the leaf names already claimed in this reconcile pass. Two folders
// may legitimately share a display name, and the shortcuts must not overwrite
// one another — but the suffix only appears in the genuinely ambiguous case,
// rather than being carried by every button.
std::wstring LnkPathFor(const FolderStore::Entry& entry,
                        std::vector<std::wstring>* taken) {
    std::wstring dir = SourceDir();
    if (dir.empty() || entry.pinId.empty()) {
        return L"";
    }
    std::wstring leaf = SafeLeaf(entry.name);
    std::wstring candidate = leaf;
    for (int suffix = 2; suffix < 100; suffix++) {
        bool clash = false;
        for (const auto& used : *taken) {
            if (_wcsicmp(used.c_str(), candidate.c_str()) == 0) {
                clash = true;
                break;
            }
        }
        if (!clash) {
            break;
        }
        candidate = leaf + L" (" + std::to_wstring(suffix) + L")";
    }
    taken->push_back(candidate);
    return dir + L"\\" + candidate + L".lnk";
}

// Splits an icon spec ("C:\x\y.dll,3", or a bare path) into file and index.
// Returns false when the file does not exist, so callers can fall through to the
// next candidate rather than producing a blank button.
bool ParseIconSpec(const std::wstring& spec, std::wstring* file, int* index) {
    if (spec.empty()) {
        return false;
    }
    std::wstring raw = ExpandEnv(Trim(spec));
    *index = 0;
    size_t comma = raw.find_last_of(L',');
    // Guard against "C:,3" style nonsense and against splitting a drive colon.
    if (comma != std::wstring::npos && comma > 2) {
        std::wstring tail = Trim(raw.substr(comma + 1));
        wchar_t* end = nullptr;
        long parsed = wcstol(tail.c_str(), &end, 10);
        if (end && *end == L'\0' && !tail.empty()) {
            *index = (int)parsed;
            raw = Trim(raw.substr(0, comma));
        }
    }
    if (GetFileAttributesW(raw.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    *file = raw;
    return true;
}

// A .lnk icon has to be a file on disk that the shell can load an icon resource
// from, so only .ico/.exe/.dll-shaped specs can be used directly.
//
// ponytail: emoji and .png icons fall back to the default folder glyph. Giving
// them real buttons means rendering to an .ico next to the shortcut - the mod
// already builds these images for the hover grid (HIconToBitmap), so the
// missing piece is only an HICON -> .ico writer.
void ResolveIcon(const FolderStore::Entry& entry,
                 const std::wstring& folderPath,
                 std::wstring* file,
                 int* index) {
    if (ParseIconSpec(entry.icon, file, index)) {
        return;
    }
    if (ParseIconSpec(ReadFolderCustomIcon(folderPath), file, index)) {
        return;
    }
    *file = kDefaultIconFile;
    *index = kDefaultIconIndex;
}

// Reads the AppUserModelID off an existing .lnk. Empty when it has none, which
// is the normal case for shortcuts that are not ours.
std::wstring ReadShortcutAppId(const std::wstring& lnkPath) {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, (void**)&link)) ||
        !link) {
        return L"";
    }

    std::wstring appId;
    IPersistFile* persist = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_IPersistFile, (void**)&persist)) &&
        persist) {
        if (SUCCEEDED(persist->Load(lnkPath.c_str(), STGM_READ))) {
            IPropertyStore* store = nullptr;
            if (SUCCEEDED(link->QueryInterface(IID_IPropertyStore,
                                               (void**)&store)) &&
                store) {
                PROPVARIANT pv;
                PropVariantInit(&pv);
                if (SUCCEEDED(store->GetValue(kPKEY_AppUserModel_ID, &pv)) &&
                    pv.vt == VT_LPWSTR && pv.pwszVal) {
                    appId = pv.pwszVal;
                }
                PropVariantClear(&pv);
                store->Release();
            }
        }
        persist->Release();
    }
    link->Release();
    return appId;
}

// pinId -> signature of what was last written for it. Reconcile runs on a
// single dedicated worker thread (see ThreadProc below), so this needs no
// lock. Lets a reorder-drag reconcile (which touches every pinned entry, not
// just the moved one) skip the .lnk rewrite and jump list COM round-trip for
// every entry whose content did not actually change.
std::unordered_map<std::wstring, std::wstring> g_shortcutSignatures;

std::wstring ShortcutSignature(const FolderStore::Entry& entry,
                               const std::wstring& folderPath,
                               const std::wstring& lnkPath) {
    return entry.name + L"\x1f" + folderPath + L"\x1f" + entry.icon + L"\x1f" +
           lnkPath;
}

// Writes (or rewrites) the shortcut behind one entry. Callers gate this on
// g_shortcutSignatures so it only actually runs when something changed, but
// this function itself always writes - it is how a renamed folder, a changed
// icon, or a moved target reach an already-pinned button.
bool WriteShortcut(const FolderStore::Entry& entry,
                   const std::wstring& folderPath,
                   const std::wstring& lnkPath) {
    std::wstring dir = lnkPath.substr(0, lnkPath.find_last_of(L'\\'));
    SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    if (GetFileAttributesW(dir.c_str()) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Pins: could not create %s", dir.c_str());
        return false;
    }

    IShellLinkW* link = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IShellLinkW, (void**)&link);
    if (FAILED(hr) || !link) {
        Wh_Log(L"Pins: CoCreateInstance(ShellLink) failed: 0x%08X", hr);
        return false;
    }

    WCHAR explorerPath[MAX_PATH];
    GetWindowsDirectoryW(explorerPath, ARRAYSIZE(explorerPath));
    wcscat_s(explorerPath, L"\\explorer.exe");
    link->SetPath(explorerPath);
    // Quoted: folder paths routinely contain spaces.
    std::wstring args = L"\"" + folderPath + L"\"";
    link->SetArguments(args.c_str());
    link->SetDescription(entry.name.empty() ? L"Taskbar folder"
                                            : entry.name.c_str());

    std::wstring iconFile;
    int iconIndex = 0;
    ResolveIcon(entry, folderPath, &iconFile, &iconIndex);
    link->SetIconLocation(iconFile.c_str(), iconIndex);

    bool ok = false;
    IPropertyStore* store = nullptr;
    hr = link->QueryInterface(IID_IPropertyStore, (void**)&store);
    if (SUCCEEDED(hr) && store) {
        std::wstring appId = AppIdFor(entry.pinId);
        PROPVARIANT pv;
        PropVariantInit(&pv);
        size_t bytes = (appId.size() + 1) * sizeof(WCHAR);
        pv.pwszVal = (PWSTR)CoTaskMemAlloc(bytes);
        if (pv.pwszVal) {
            memcpy(pv.pwszVal, appId.c_str(), bytes);
            pv.vt = VT_LPWSTR;
            if (SUCCEEDED(store->SetValue(kPKEY_AppUserModel_ID, pv)) &&
                SUCCEEDED(store->Commit())) {
                ok = true;
            }
        }
        PropVariantClear(&pv);
        store->Release();
    }
    if (!ok) {
        // Without the AUMID the item would merge into the File Explorer group,
        // which is worse than having no button: it would look like the mod
        // corrupted the user's existing File Explorer pin.
        Wh_Log(L"Pins: could not stamp the AppUserModelID, refusing to write %s",
               lnkPath.c_str());
        link->Release();
        return false;
    }

    IPersistFile* persist = nullptr;
    hr = link->QueryInterface(IID_IPersistFile, (void**)&persist);
    if (SUCCEEDED(hr) && persist) {
        hr = persist->Save(lnkPath.c_str(), TRUE);
        persist->Release();
    }
    link->Release();

    if (FAILED(hr)) {
        Wh_Log(L"Pins: saving %s failed: 0x%08X", lnkPath.c_str(), hr);
        return false;
    }
    SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, lnkPath.c_str(),
                   nullptr);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// "Manage folders..." on the button's own right-click menu
//
// That menu belongs to Windows now, and a mod cannot append to it. But it is a
// jump list, and a jump list is keyed by AppUserModelID — which is ours. So the
// entry goes on through the documented ICustomDestinationList route instead of
// being drawn by this mod at all.
//
// A jump list task can only launch a command line, and the manager window lives
// inside explorer.exe, so the task carries a sentinel token and the launch is
// caught in-process (see the CreateProcessW hook). If that interception ever
// stops working the task still runs, so it points at explorer.exe: the worst
// case is a stray Explorer window rather than a dead menu item.

constexpr PCWSTR kManageSentinel = L"--taskbar-folder-hover-tray-manage";

// PKEY_Title, spelled out for the same reason as the AppUserModelID key.
const PROPERTYKEY kPKEY_Title = {
    {0xF29F85E0,
     0x4FF9,
     0x1068,
     {0xAB, 0x91, 0x08, 0x00, 0x2B, 0x27, 0xB3, 0xD9}},
    2};

// A jump list task: a shell link plus a title, which is what the menu shows.
IShellLinkW* MakeManageTask() {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, (void**)&link)) ||
        !link) {
        return nullptr;
    }

    WCHAR explorerPath[MAX_PATH];
    GetWindowsDirectoryW(explorerPath, ARRAYSIZE(explorerPath));
    wcscat_s(explorerPath, L"\\explorer.exe");
    link->SetPath(explorerPath);
    link->SetArguments(kManageSentinel);
    link->SetIconLocation(L"%SystemRoot%\\system32\\imageres.dll", 3);

    // Without a title the shell shows nothing for the task.
    IPropertyStore* store = nullptr;
    if (SUCCEEDED(link->QueryInterface(IID_IPropertyStore, (void**)&store)) &&
        store) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        PCWSTR title = L"Manage folders...";
        size_t bytes = (wcslen(title) + 1) * sizeof(WCHAR);
        pv.pwszVal = (PWSTR)CoTaskMemAlloc(bytes);
        if (pv.pwszVal) {
            memcpy(pv.pwszVal, title, bytes);
            pv.vt = VT_LPWSTR;
            store->SetValue(kPKEY_Title, pv);
            store->Commit();
        }
        PropVariantClear(&pv);
        store->Release();
    }
    return link;
}

// Publishes the jump list for one of our pinned items.
// AppIDs whose jump list is already known to be on the shell's copy, so
// Reconcile (see below) does not redo the ICustomDestinationList round-trip
// for every pinned item on every pass.
std::unordered_set<std::wstring> g_jumpListWritten;

void WriteJumpList(const std::wstring& appId) {
    ICustomDestinationList* list = nullptr;
    if (FAILED(CoCreateInstance(CLSID_DestinationList, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_ICustomDestinationList, (void**)&list)) ||
        !list) {
        return;
    }
    list->SetAppID(appId.c_str());

    UINT slots = 0;
    IObjectArray* removed = nullptr;
    HRESULT hr = list->BeginList(&slots, IID_IObjectArray, (void**)&removed);
    if (removed) {
        removed->Release();
    }
    if (FAILED(hr)) {
        Wh_Log(L"Pins: BeginList for %s failed: 0x%08X", appId.c_str(), hr);
        list->Release();
        return;
    }

    IObjectCollection* tasks = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_EnumerableObjectCollection, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_IObjectCollection, (void**)&tasks)) &&
        tasks) {
        if (IShellLinkW* task = MakeManageTask()) {
            tasks->AddObject(task);
            task->Release();
        }
        if (IObjectArray* array = nullptr;
            SUCCEEDED(tasks->QueryInterface(IID_IObjectArray,
                                            (void**)&array)) &&
            array) {
            hr = list->AddUserTasks(array);
            array->Release();
            if (FAILED(hr)) {
                Wh_Log(L"Pins: AddUserTasks failed: 0x%08X", hr);
            }
        }
        tasks->Release();
    }

    hr = list->CommitList();
    if (FAILED(hr)) {
        Wh_Log(L"Pins: CommitList for %s failed: 0x%08X", appId.c_str(), hr);
    }
    list->Release();
}

// Undoes WriteJumpList: drops the AppID's jump list from the shell's
// CustomDestinations store entirely, rather than just emptying it.
void DeleteJumpList(const std::wstring& appId) {
    ICustomDestinationList* list = nullptr;
    if (FAILED(CoCreateInstance(CLSID_DestinationList, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_ICustomDestinationList, (void**)&list)) ||
        !list) {
        return;
    }
    HRESULT hr = list->DeleteList(appId.c_str());
    if (FAILED(hr)) {
        Wh_Log(L"Pins: DeleteList for %s failed: 0x%08X", appId.c_str(), hr);
    }
    list->Release();
}

// Invokes a shell verb on a path. Pinning and unpinning are both just verbs, so
// neither needs the undocumented IPinnedList3.
bool InvokeVerb(const std::wstring& path, PCSTR verb) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr);
    if (FAILED(hr) || !pidl) {
        Wh_Log(L"Pins: SHParseDisplayName(%s) failed: 0x%08X", path.c_str(), hr);
        return false;
    }

    IShellFolder* parent = nullptr;
    PCUITEMID_CHILD child = nullptr;
    hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&parent, &child);
    if (FAILED(hr) || !parent) {
        Wh_Log(L"Pins: SHBindToParent failed: 0x%08X", hr);
        ILFree(pidl);
        return false;
    }

    bool ok = false;
    IContextMenu* menu = nullptr;
    hr = parent->GetUIObjectOf(nullptr, 1, &child, IID_IContextMenu, nullptr,
                               (void**)&menu);
    if (SUCCEEDED(hr) && menu) {
        if (HMENU hMenu = CreatePopupMenu()) {
            // Must run first: QueryContextMenu is what makes the handler
            // enumerate and register its verbs. Without it InvokeCommand cannot
            // find "taskbarpin" and returns ERROR_NO_ASSOCIATION.
            menu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL);

            CMINVOKECOMMANDINFO info = {};
            info.cbSize = sizeof(info);
            info.lpVerb = verb;
            info.nShow = SW_SHOWNORMAL;
            hr = menu->InvokeCommand(&info);
            ok = SUCCEEDED(hr);
            if (!ok) {
                Wh_Log(L"Pins: %S on %s returned 0x%08X", verb, path.c_str(),
                       hr);
            }
            DestroyMenu(hMenu);
        }
        menu->Release();
    } else {
        Wh_Log(L"Pins: GetUIObjectOf(IContextMenu) failed: 0x%08X", hr);
    }

    parent->Release();
    ILFree(pidl);
    return ok;
}

// One of this mod's items as the shell currently holds it. `leaf` is the
// shortcut's file name without the extension, which is exactly what the taskbar
// labels the button with — and therefore the only reliable way to tie a XAML
// button back to the entry behind it without hooking the shell's own group type.
struct PinnedItem {
    std::wstring leaf;
    std::wstring appId;
    std::wstring path;
};

// Everything currently pinned that belongs to this mod, read from the shell's
// own copy of the pinned items rather than from anything the mod believes. This
// is what makes a native "Unpin from taskbar" visible, and what keeps the button
// labels honest when a shortcut has been renamed underneath a live pin.
// ok, if given, is set to false when the read could not be trusted (the
// pinned-items folder couldn't be resolved, or listing it failed for a
// reason other than "no shortcuts there yet"). Callers that treat an empty
// result as "user unpinned everything" must check this before acting on it.
std::vector<PinnedItem> ReadPinnedItems(bool* ok = nullptr) {
    std::vector<PinnedItem> items;
    if (ok) {
        *ok = true;
    }
    std::wstring dir = PinnedDir();
    if (dir.empty()) {
        if (ok) {
            *ok = false;
        }
        return items;
    }

    WIN32_FIND_DATAW find{};
    HANDLE handle = FindFirstFileW((dir + L"\\*.lnk").c_str(), &find);
    if (handle == INVALID_HANDLE_VALUE) {
        if (ok && GetLastError() != ERROR_FILE_NOT_FOUND) {
            *ok = false;
        }
        return items;
    }
    do {
        if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }
        std::wstring path = dir + L"\\" + find.cFileName;
        std::wstring appId = ReadShortcutAppId(path);
        if (appId.empty() || !IsOurAppId(appId)) {
            continue;
        }
        std::wstring leaf = find.cFileName;
        if (size_t dot = leaf.find_last_of(L'.'); dot != std::wstring::npos) {
            leaf.resize(dot);
        }
        items.push_back({std::move(leaf), std::move(appId), std::move(path)});
    } while (FindNextFileW(handle, &find));
    FindClose(handle);
    return items;
}

std::vector<std::wstring> ReadPinnedAppIds() {
    std::vector<std::wstring> appIds;
    for (auto& item : ReadPinnedItems()) {
        appIds.push_back(std::move(item.appId));
    }
    return appIds;
}

// Button label -> pin id, published for the taskbar UI thread.
//
// Built here rather than read on demand because the taskbar thread needs it
// during layout, and rebuilding it means opening every pinned shortcut through
// COM — far too expensive to do on a layout pass. The generation counter lets
// that thread notice a change without holding the lock to compare.
std::mutex g_labelMutex;
std::vector<std::pair<std::wstring, std::wstring>> g_labelToPinId;
std::atomic<uint32_t> g_labelGeneration{0};

void PublishLabels(const std::vector<PinnedItem>& items) {
    std::vector<std::pair<std::wstring, std::wstring>> map;
    for (const auto& item : items) {
        std::wstring pinId = item.appId.substr(wcslen(kAppIdPrefix));
        map.emplace_back(item.leaf, std::move(pinId));
    }
    {
        std::lock_guard<std::mutex> lock(g_labelMutex);
        if (map == g_labelToPinId) {
            return;
        }
        g_labelToPinId = std::move(map);
    }
    g_labelGeneration.fetch_add(1, std::memory_order_release);
}

// A taskbar button's accessible name is the item's label plus whatever state
// the shell wants to announce — "Games pinned", "Brave - 1 running window
// pinned", "Cursor - 1 running window" in English, but the annotation itself
// is locale text ("Games angeheftet" in German, etc). So rather than
// whitelist annotation strings, the label is treated as a prefix full stop,
// and PinIdForLabel below picks the exact/longest match among our own labels
// to keep "Games" from claiming a button actually named "Games Launcher".
bool LabelMatchesAccessibleName(const std::wstring& leaf,
                                const std::wstring& name) {
    if (leaf.empty() || name.size() < leaf.size()) {
        return false;
    }
    if (_wcsnicmp(name.c_str(), leaf.c_str(), leaf.size()) != 0) {
        return false;
    }
    // The shell's state annotation is always appended after a separator
    // ("Games pinned", "Games angeheftet"), so a bare prefix match still
    // claims unrelated apps whose name happens to start the same way
    // ("Discord" for a folder named "D"). Require the match to end on a
    // word boundary.
    return name.size() == leaf.size() || !iswalnum(name[leaf.size()]);
}

// Empty when this label is not one of ours, or when it is ambiguous between
// two of our own labels (e.g. folders named "Games" and "Games Launcher" both
// prefix-match the same button) — in that case, skip rather than guess.
//
// ponytail: only disambiguates against our own labels, not every other
// realized taskbar button's label. A folder literally named as a prefix of
// some other app's title ("Games" vs. a real "Games Launcher" app) can still
// mismatch. Add cross-button disambiguation (bridge XAML button -> AppID via
// CTaskGroup::GetAppID) if that turns out to matter in practice.
std::wstring PinIdForLabel(const std::wstring& accessibleName) {
    std::lock_guard<std::mutex> lock(g_labelMutex);
    const std::wstring* bestLeaf = nullptr;
    std::wstring bestPinId;
    bool ambiguous = false;
    for (const auto& [leaf, pinId] : g_labelToPinId) {
        if (!LabelMatchesAccessibleName(leaf, accessibleName)) {
            continue;
        }
        if (leaf.size() == accessibleName.size()) {
            // Exact match wins outright.
            return pinId;
        }
        if (!bestLeaf || leaf.size() > bestLeaf->size()) {
            bestLeaf = &leaf;
            bestPinId = pinId;
            ambiguous = false;
        } else if (leaf.size() == bestLeaf->size()) {
            ambiguous = true;
        }
    }
    if (!bestLeaf || ambiguous) {
        return L"";
    }
    return bestPinId;
}

bool Contains(const std::vector<std::wstring>& list, const std::wstring& value) {
    for (const auto& item : list) {
        if (_wcsicmp(item.c_str(), value.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

// Brings the real taskbar into line with the store: every pinned entry gets a
// shortcut and a pin, everything else of ours is removed, and an entry the user
// unpinned natively is written back to the store as a draft.
//
// Written as a reconcile rather than as pin/unpin calls threaded through the
// eight or so places that mutate the store: it is idempotent, it is the only
// thing that has to be correct, and it also repairs state after an explorer
// crash or a manual edit, which per-site calls never would.
void Reconcile() {
    std::wstring sourceDir = SourceDir();
    if (sourceDir.empty()) {
        Wh_Log(L"Pins: no Start Menu Programs folder, skipping");
        return;
    }

    // One read of the shell's pinned items, reused everywhere below that
    // nothing has yet changed pin state — this is what previously cost four
    // full ReadPinnedItems()/ReadPinnedAppIds() COM round-trips per pass.
    // Re-read only after something that can actually change what is pinned
    // (the unpin/pin loops further down).
    bool pinnedItemsOk = true;
    std::vector<PinnedItem> initialPinnedItems = ReadPinnedItems(&pinnedItemsOk);
    std::vector<std::wstring> initialPinnedAppIds;
    initialPinnedAppIds.reserve(initialPinnedItems.size());
    for (const auto& item : initialPinnedItems) {
        initialPinnedAppIds.push_back(item.appId);
    }

    struct Desired {
        std::wstring appId;
        std::wstring lnkPath;
        // The label the taskbar should be showing for this item.
        std::wstring leaf;
    };
    std::vector<Desired> desired;
    std::vector<std::wstring> keepFiles;
    std::vector<FolderStore::Entry> todo;
    std::vector<std::wstring> takenLeaves;

    bool unpinnedByUser = false;
    bool generatedPinIds = false;
    bool renamedOnDisk = false;

    {
        std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
        auto stored = FolderStore::Read();
        bool storeChanged = false;

        // A pinned entry with no id yet is either new or predates this mod
        // version; either way it needs one before it can have a shortcut. This
        // is also the whole of the upgrade path for existing users.
        for (auto& entry : stored) {
            if (entry.pinned && entry.pinId.empty()) {
                GUID guid{};
                if (FAILED(CoCreateGuid(&guid))) {
                    continue;
                }
                WCHAR buf[40];
                swprintf(buf, ARRAYSIZE(buf), L"%08lx%04hx%04hx", guid.Data1,
                         guid.Data2, guid.Data3);
                entry.pinId = buf;
                storeChanged = true;
                generatedPinIds = true;
            }
        }

        const std::vector<std::wstring>& live = initialPinnedAppIds;
        // The reverse sync below is only as good as this list. If a native
        // unpin is not reflected here, the shell is flushing that folder later
        // than it changes the taskbar, and pin state has to be read from
        // somewhere else instead.
        Wh_Log(L"Pins: reconcile - the shell reports %zu of our items pinned",
               live.size());

        for (auto& entry : stored) {
            if (!entry.pinned || entry.pinId.empty()) {
                continue;
            }

            // Reverse sync. The shell removes a natively-unpinned item without
            // telling the mod, so "we pinned this once and it is gone now" is
            // the only evidence that the user unpinned it. Demote it to a draft
            // rather than re-pinning: re-pinning would make native Unpin look
            // broken, and the entry keeps its name and icon either way.
            // Only trust this when the read itself succeeded — a failed read
            // to resolve/list the pinned-items folder also comes back empty,
            // and treating that the same way would demote every entry at once.
            if (pinnedItemsOk && entry.pinApplied &&
                !Contains(live, AppIdFor(entry.pinId))) {
                Wh_Log(L"Pins: '%s' was unpinned from the taskbar, keeping it "
                       L"as a draft",
                       entry.name.c_str());
                entry.pinned = false;
                entry.pinApplied = false;
                storeChanged = true;
                unpinnedByUser = true;
                continue;
            }
            // Follow an in-place rename done outside the mod (Explorer's own
            // F2/right-click Rename on the folder itself, not the mod's Name
            // field). BuildFolderEntry already does this, but only runs on
            // Wh_ModInit / a settings reload — nothing previously watched the
            // pinned folders themselves, so a plain Explorer rename never
            // reached the taskbar button until something unrelated forced a
            // reload. Reconcile already runs on every taskbar/pin change and
            // periodically (see kReconcilePollMs below), so doing the same
            // fileId lookup here catches it without waiting on that.
            std::wstring expandedPath = ExpandEnv(entry.path);
            if (!IsShellFolderPath(expandedPath) && entry.fileId != 0 &&
                !IsLikelyRemotePath(expandedPath) &&
                !DirectoryExists(ResolveFolderPath(expandedPath))) {
                std::wstring renamed =
                    FindRenamedSibling(expandedPath, entry.fileId);
                if (!renamed.empty()) {
                    size_t slash = expandedPath.find_last_of(L"\\/");
                    std::wstring oldLeaf = slash == std::wstring::npos
                                               ? expandedPath
                                               : expandedPath.substr(slash + 1);
                    bool nameWasLeaf = entry.name.empty() ||
                                       _wcsicmp(entry.name.c_str(),
                                                oldLeaf.c_str()) == 0;
                    Wh_Log(L"Pins: '%s' was renamed to '%s' on disk, following",
                           expandedPath.c_str(), renamed.c_str());
                    entry.path = renamed;
                    if (nameWasLeaf) {
                        size_t newSlash = renamed.find_last_of(L"\\/");
                        entry.name = newSlash == std::wstring::npos
                                         ? renamed
                                         : renamed.substr(newSlash + 1);
                    }
                    storeChanged = true;
                    renamedOnDisk = true;
                }
            }

            // Only a copy of what the slow work below needs. Resolving a path
            // and writing a shortcut both touch the filesystem, and the folder
            // may be a network share that blocks for the share's timeout — the
            // store lock is held by the manager window and by the Explorer
            // right-click pin, neither of which should wait on a dead share.
            todo.push_back(entry);
        }

        if (storeChanged) {
            FolderStore::Write(stored);
        }
    }

    for (const auto& entry : todo) {
        std::wstring folderPath = ResolveFolderPath(ExpandEnv(entry.path));
        if (folderPath.empty()) {
            folderPath = ExpandEnv(entry.path);
        }
        bool resolves =
            GetFileAttributesW(folderPath.c_str()) != INVALID_FILE_ATTRIBUTES;

        std::wstring lnkPath = LnkPathFor(entry, &takenLeaves);
        if (lnkPath.empty()) {
            continue;
        }

        if (!resolves) {
            // Offline share, unplugged drive, or a drive letter not yet
            // remapped at logon: this is likely transient, not gone. Keep
            // whatever shortcut was last written for it so the taskbar button
            // is neither unpinned nor swept below - losing its position is
            // worse than leaving a stale button up while the folder is
            // unreachable. Only skip entirely when there is nothing to keep.
            if (GetFileAttributesW(lnkPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                Wh_Log(L"Pins: '%s' does not exist and was never pinned, "
                       L"skipping",
                       folderPath.c_str());
                continue;
            }
            Wh_Log(L"Pins: '%s' does not currently resolve, keeping existing "
                   L"pin",
                   folderPath.c_str());
        } else {
            std::wstring signature = ShortcutSignature(entry, folderPath, lnkPath);
            auto cached = g_shortcutSignatures.find(entry.pinId);
            bool unchanged = cached != g_shortcutSignatures.end() &&
                             cached->second == signature &&
                             GetFileAttributesW(lnkPath.c_str()) !=
                                 INVALID_FILE_ATTRIBUTES;
            if (!unchanged) {
                if (!WriteShortcut(entry, folderPath, lnkPath)) {
                    continue;
                }
                g_shortcutSignatures[entry.pinId] = signature;
            }
        }
        keepFiles.push_back(lnkPath);

        std::wstring leaf = lnkPath.substr(lnkPath.find_last_of(L'\\') + 1);
        leaf.resize(leaf.find_last_of(L'.'));
        desired.push_back({AppIdFor(entry.pinId), lnkPath, leaf});
    }

    // Reconcile against the shell's own copies. Pinning copies the shortcut, so
    // the pinned item keeps whatever name it had when it was pinned — renaming
    // the source .lnk afterwards does not reach the taskbar, and the button
    // would sit there labelled with a name the mod no longer uses.
    //
    // renamedAppIds tracks which of our own appIds got unpinned here for a
    // stale label. Explorer does not appear to treat "unpin AppID X, pin AppID
    // X again" as two real operations when they land back to back on the same
    // pass with no real elapsed time between them - the re-pin silently has no
    // effect, even though both InvokeCommand calls return success. Waiting
    // below for the unpin to actually drop off the shell's own pinned list
    // before asking for the re-pin is what a manual unpin-then-later-pin from
    // the real taskbar menu gets for free just by not being instantaneous.
    std::unordered_set<std::wstring> renamedAppIds;
    for (const auto& item : initialPinnedItems) {
        const Desired* match = nullptr;
        for (const auto& want : desired) {
            if (_wcsicmp(want.appId.c_str(), item.appId.c_str()) == 0) {
                match = &want;
                break;
            }
        }
        if (!match) {
            if (InvokeVerb(item.path, "taskbarunpin")) {
                Wh_Log(L"Pins: unpinned %s", item.leaf.c_str());
            }
            continue;
        }
        // Same item, stale label: unpin so the loop below pins it afresh under
        // the right name. Costs its place in the taskbar order, which is why it
        // only happens when the name genuinely changed.
        if (_wcsicmp(match->leaf.c_str(), item.leaf.c_str()) != 0) {
            if (InvokeVerb(item.path, "taskbarunpin")) {
                Wh_Log(L"Pins: '%s' is now called '%s', re-pinning",
                       item.leaf.c_str(), match->leaf.c_str());
            }
            renamedAppIds.insert(item.appId);
        }
    }

    // Give each rename's unpin real wall-clock time to actually drop off the
    // shell's pinned list before the pin loop below runs - see the comment
    // above. Bounded so a share/permission hiccup that leaves the unpin
    // stuck cannot hang this worker thread indefinitely; the next debounced
    // pass will just try again.
    if (!renamedAppIds.empty()) {
        Wh_Log(L"Pins: %zu renamed item(s) to wait on before re-pinning",
               renamedAppIds.size());
    }
    for (const auto& appId : renamedAppIds) {
        ULONGLONG start = GetTickCount64();
        bool dropped = false;
        for (int waited = 0; waited < 2000; waited += 25) {
            if (!Contains(ReadPinnedAppIds(), appId)) {
                dropped = true;
                break;
            }
            PumpScanThreadMessages();
            Sleep(25);
        }
        Wh_Log(L"Pins: waited %llums for %s to drop from the pinned list: %s",
               GetTickCount64() - start, appId.c_str(),
               dropped ? L"dropped" : L"still there, giving up for now");
    }

    std::vector<std::wstring> live = ReadPinnedAppIds();

    for (const auto& item : desired) {
        bool wasRenamed = renamedAppIds.count(item.appId) != 0;
        if (Contains(live, item.appId) && !wasRenamed) {
            continue;
        }
        Wh_Log(L"Pins: pinning %s (renamed=%d, still-live=%d)",
               item.lnkPath.c_str(), wasRenamed, Contains(live, item.appId));
        if (InvokeVerb(item.lnkPath, "taskbarpin")) {
            Wh_Log(L"Pins: pinned %s", item.lnkPath.c_str());
        } else {
            Wh_Log(L"Pins: pin attempt for %s did not succeed",
                   item.lnkPath.c_str());
        }
    }

    // Sweep our own shortcut directory: a renamed entry leaves its old .lnk
    // behind, and those accumulate.
    WIN32_FIND_DATAW find{};
    HANDLE handle = FindFirstFileW((sourceDir + L"\\*.lnk").c_str(), &find);
    if (handle != INVALID_HANDLE_VALUE) {
        do {
            if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue;
            }
            std::wstring path = sourceDir + L"\\" + find.cFileName;
            if (!Contains(keepFiles, path)) {
                DeleteFileW(path.c_str());
                SHChangeNotify(SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSH,
                               path.c_str(), nullptr);
            }
        } while (FindNextFileW(handle, &find));
        FindClose(handle);
    }

    // Record what actually ended up on the taskbar. Done once at the end from
    // the shell's own list rather than from the return value of each pin,
    // because "the verb returned S_OK" and "there is a button" are not the same
    // claim — and pinApplied is what the reverse sync above trusts next run.
    std::vector<PinnedItem> finalItems = ReadPinnedItems();
    {
        std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
        auto stored = FolderStore::Read();
        std::vector<std::wstring> finalLive;
        for (const auto& item : finalItems) {
            finalLive.push_back(item.appId);
        }
        bool storeChanged = false;
        for (auto& entry : stored) {
            if (entry.pinId.empty()) {
                continue;
            }
            bool applied =
                entry.pinned && Contains(finalLive, AppIdFor(entry.pinId));
            if (entry.pinApplied != applied) {
                entry.pinApplied = applied;
                storeChanged = true;
            }
        }
        if (storeChanged) {
            FolderStore::Write(stored);
        }
    }

    // Hand the taskbar UI thread the label -> pin id map it needs to recognise
    // our buttons. Built from what the shell actually holds, so it stays right
    // even when a pinned copy's name has drifted from the source shortcut.
    PublishLabels(finalItems);

    // Put "Manage folders..." on each button's own right-click menu. Needs
    // rewriting after an item was unpinned and pinned again - the jump list
    // is stored per AppID by the shell, and it does not come back on its own
    // - so g_jumpListWritten is cleared for any AppID that drops off the
    // taskbar, forcing the next reconcile that sees it again to redo it. An
    // AppID that stayed pinned across this pass already has its jump list.
    {
        std::unordered_set<std::wstring> stillLive;
        for (const auto& item : finalItems) {
            stillLive.insert(item.appId);
            if (g_jumpListWritten.insert(item.appId).second) {
                WriteJumpList(item.appId);
            }
        }
        for (auto it = g_jumpListWritten.begin();
             it != g_jumpListWritten.end();) {
            if (stillLive.count(*it)) {
                ++it;
            } else {
                it = g_jumpListWritten.erase(it);
            }
        }
    }

    // Both of these mean the loaded folder list no longer matches the store.
    //
    // unpinnedByUser: the entry was demoted to a draft, so its button and its
    // row in the manager window are stale.
    //
    // generatedPinIds: Wh_ModInit loads the folder list before this worker has
    // ever run, so on the first load after an upgrade every entry in g_settings
    // has an empty pin id — and the pin id is exactly what ties a real taskbar
    // button back to its folder. Without this reload the buttons exist, the
    // labels resolve, and nothing matches. Reloading re-reads the store now that
    // the ids are persisted; the next reconcile generates none, so this does not
    // recur.
    //
    // renamedOnDisk: an Explorer rename updated the store's path/name under
    // the entry, so g_settings.folders still points hover and the scan
    // thread at the pre-rename path until this reloads it.
    if (unpinnedByUser || generatedPinIds || renamedOnDisk) {
        RequestReloadUI();
    }
}

////////////////////////////////////////////////////////////////////////////////
// The STA worker
//
// Everything above must run here. See the note at the top of the namespace for
// why it cannot be the taskbar UI thread.

HANDLE g_thread = nullptr;
HANDLE g_stopEvent = nullptr;  // manual-reset
HANDLE g_kickEvent = nullptr;  // auto-reset

// Set by Stop() before it signals g_stopEvent, run by ThreadProc itself right
// before it uninitializes COM and exits. This is a known-good STA — the pin
// verb's COM class is ThreadingModel=Apartment with no marshaler, so cleanup
// that invokes it must run on an apartment that is guaranteed to still be
// alive, not on whatever arbitrary thread called Stop(). Safe without a lock:
// the write happens-before the SetEvent that wakes the worker to read it.
std::function<void()> g_finalAction;

// Unpinning from the taskbar's own right-click menu is completely silent: it
// changes nothing the mod owns, so nothing calls RequestReconcile and the entry
// stays "pinned" in the store until something else happens to trigger a reload
// — in practice, the next Explorer restart. Watching the shell's own state is
// the only way to hear about it.
//
// Two sources because they can be updated at different moments: the pinned-items
// folder (which is what ReadPinnedAppIds reads) and the Taskband key the shell
// keeps the order in. Either firing is treated as "something changed, go look".
constexpr DWORD kPinWatchDebounceMs = 400;

// Nothing watches the pinned folders themselves - only the shell's own
// pinned-items copy and the Taskband key are watched, both of which are about
// pin *state*, not about the folder Explorer's F2/right-click Rename just
// renamed on disk. This is the safety net that catches that case: it bounds
// how long a plain Explorer rename can sit unreflected on the taskbar without
// needing an unrelated pin/unpin to shake a reconcile loose.
// A rename showing up within a minute or two is fine — this is only a safety
// net for a case the event-driven watches above (pinned-items copy, Taskband
// key) do not cover, not something that needs sub-minute latency. Long enough
// that a dozen-pin taskbar is not walked through COM every few seconds
// forever for the rare case this actually catches anything.
constexpr DWORD kReconcilePollMs = 120000;

DWORD WINAPI ThreadProc(void*) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        Wh_Log(L"Pins: CoInitializeEx(STA) failed: 0x%08X", hr);
        return 0;
    }

    HANDLE dirWatch = INVALID_HANDLE_VALUE;
    if (std::wstring pinnedDir = PinnedDir(); !pinnedDir.empty()) {
        dirWatch = FindFirstChangeNotificationW(
            pinnedDir.c_str(), FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
        if (dirWatch == INVALID_HANDLE_VALUE) {
            Wh_Log(L"Pins: cannot watch %s (%u); a native unpin will only be "
                   L"noticed on the next reload",
                   pinnedDir.c_str(), GetLastError());
        }
    }

    HKEY taskbandKey = nullptr;
    HANDLE regEvent = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
                      L"\\Taskband",
                      0, KEY_NOTIFY, &taskbandKey) == ERROR_SUCCESS) {
        regEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    }
    // Re-arms the registry watch. It is one-shot: every fire needs a new call or
    // the second change is never heard.
    auto armRegWatch = [&]() {
        if (taskbandKey && regEvent) {
            ResetEvent(regEvent);
            RegNotifyChangeKeyValue(taskbandKey, TRUE,
                                    REG_NOTIFY_CHANGE_LAST_SET |
                                        REG_NOTIFY_CHANGE_NAME,
                                    regEvent, TRUE);
        }
    };
    armRegWatch();

    HANDLE waits[4];
    DWORD count = 0;
    waits[count++] = g_stopEvent;
    waits[count++] = g_kickEvent;
    const DWORD dirIndex =
        dirWatch != INVALID_HANDLE_VALUE ? count : 0xFFFFFFFF;
    if (dirWatch != INVALID_HANDLE_VALUE) {
        waits[count++] = dirWatch;
    }
    const DWORD regIndex = regEvent ? count : 0xFFFFFFFF;
    if (regEvent) {
        waits[count++] = regEvent;
    }

    // Re-arms whichever watch just fired, so the wait below stays live.
    auto rearm = [&](DWORD signalled) {
        if (signalled == dirIndex && dirWatch != INVALID_HANDLE_VALUE) {
            FindNextChangeNotification(dirWatch);
        } else if (signalled == regIndex) {
            armRegWatch();
        }
    };

    bool stopping = false;
    while (!stopping) {
        DWORD result = MsgWaitForMultipleObjects(count, waits, FALSE,
                                                  kReconcilePollMs, QS_ALLINPUT);
        if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) {
            break;
        }
        if (result == WAIT_OBJECT_0 + count) {
            PumpScanThreadMessages();
            continue;
        }
        if (result != WAIT_TIMEOUT) {
            rearm(result - WAIT_OBJECT_0);
        }

        // Coalesce the burst. One pin or unpin rewrites several files and the
        // registry, and this mod's own reconcile writes to the same places, so
        // reacting to each notification separately would mean several redundant
        // passes per user action — and, worse, reconciling while the shell is
        // still half way through writing.
        for (;;) {
            DWORD more = MsgWaitForMultipleObjects(
                count, waits, FALSE, kPinWatchDebounceMs, QS_ALLINPUT);
            if (more == WAIT_TIMEOUT) {
                break;
            }
            if (more == WAIT_OBJECT_0 || more == WAIT_FAILED) {
                stopping = true;
                break;
            }
            if (more == WAIT_OBJECT_0 + count) {
                PumpScanThreadMessages();
                continue;
            }
            rearm(more - WAIT_OBJECT_0);
        }
        if (stopping) {
            break;
        }

        Reconcile();
    }

    if (dirWatch != INVALID_HANDLE_VALUE) {
        FindCloseChangeNotification(dirWatch);
    }
    if (regEvent) {
        CloseHandle(regEvent);
    }
    if (taskbandKey) {
        RegCloseKey(taskbandKey);
    }
    if (g_finalAction) {
        g_finalAction();
        g_finalAction = nullptr;
    }
    CoUninitialize();
    return 0;
}

void RequestReconcile() {
    if (g_kickEvent) {
        SetEvent(g_kickEvent);
    }
}

void Start() {
    if (g_thread) {
        RequestReconcile();
        return;
    }
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_kickEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_kickEvent) {
        return;
    }
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (g_thread) {
        RequestReconcile();
    }
}

// finalAction, if given, runs on the worker's own STA right before it
// uninitializes COM and exits — see g_finalAction above.
void Stop(std::function<void()> finalAction = nullptr) {
    if (finalAction) {
        if (g_thread) {
            g_finalAction = std::move(finalAction);
        } else {
            // Worker never started this session (Start() failed, or was never
            // called) — nowhere else to run it, so run it here directly.
            finalAction();
        }
    }
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_thread) {
        // Joined, not abandoned: the worker's return address is in this image,
        // and Windhawk unmaps it as soon as uninit returns.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_kickEvent) {
        CloseHandle(g_kickEvent);
        g_kickEvent = nullptr;
    }
}

}  // namespace Pins

// Fills in the derived fields (resolved path, remote flag) a stored entry
// needs before the UI can use it, and follows an in-place rename via the
// recorded NTFS file id. Returns false if `store` was updated and should be
// written back.
bool BuildFolderEntry(FolderStore::Entry* stored, FolderEntry* out) {
    bool unchanged = true;
    out->name = stored->name;
    out->icon = stored->icon;
    out->path = ExpandEnv(stored->path);
    out->pinned = stored->pinned;
    out->pinId = stored->pinId;

    if (IsShellFolderPath(out->path)) {
        // shell: targets resolve later, on an STA.
        out->resolvedPath.clear();
        out->likelyRemote = false;
        out->resolveFailed = false;
    } else {
        out->resolvedPath = ResolveFolderPath(out->path);
        out->likelyRemote = IsLikelyRemotePath(out->resolvedPath);
        out->resolveFailed = out->resolvedPath.empty();

        // Folder missing under its stored path: it may have been renamed in
        // place. Find it by file id under the same parent and follow it.
        //
        // The trigger is the folder not being on disk, not ResolveFolderPath
        // failing — for a filesystem path that only strips trailing slashes,
        // so it never fails. Remote paths are left alone: a stat on an offline
        // share blocks for the SMB timeout, and this runs on Explorer's main
        // thread during Wh_ModInit. A miss changes nothing (no rename found,
        // or a drive that is merely unplugged keeps its path and starts
        // working again when it comes back).
        if (!out->resolveFailed && !out->likelyRemote && stored->fileId != 0 &&
            !DirectoryExists(out->resolvedPath)) {
            std::wstring renamed =
                FindRenamedSibling(out->path, stored->fileId);
            if (!renamed.empty()) {
                size_t slash = renamed.find_last_of(L"\\/");
                std::wstring leaf = slash == std::wstring::npos
                                        ? renamed
                                        : renamed.substr(slash + 1);
                // Only replace a name the user never customised.
                bool nameWasLeaf =
                    stored->name.empty() ||
                    _wcsicmp(stored->name.c_str(),
                             out->path.substr(out->path.find_last_of(L"\\/") + 1)
                                 .c_str()) == 0;
                out->path = renamed;
                out->resolvedPath = ResolveFolderPath(out->path);
                out->likelyRemote = IsLikelyRemotePath(out->resolvedPath);
                out->resolveFailed = out->resolvedPath.empty();
                stored->path = renamed;
                if (nameWasLeaf) {
                    stored->name = leaf;
                    out->name = leaf;
                }
                unchanged = false;
            }
        }
    }

    if (out->name.empty()) {
        out->name = out->path;
    }
    return unchanged;
}

void LoadFolders(std::vector<FolderEntry>* out) {
    // String-only load: no CoInitialize / SHParseDisplayName. Wh_ModInit can
    // run on Explorer's main thread before the process starts executing; creating
    // then destroying an STA there is unsafe. shell: paths stay unresolved
    // until ResolvePendingFolderEntries on the taskbar UI STA or scan STA.
    std::lock_guard<std::mutex> lock(g_foldersMutex);
    out->clear();

    // Store lock second, and only around the read-modify-write: the rename
    // fix-up below writes back what it just read. See FolderStore::g_mutex for
    // why the order is g_foldersMutex first.
    size_t storedCount = 0;
    {
        std::lock_guard<std::recursive_mutex> storeLock(FolderStore::g_mutex);
        auto stored = FolderStore::Read();
        storedCount = stored.size();
        bool storeUnchanged = true;
        for (auto& entry : stored) {
            FolderEntry loaded;
            if (!BuildFolderEntry(&entry, &loaded)) {
                storeUnchanged = false;
            }
            // Drafts stay in the store but get no taskbar button.
            if (entry.pinned) {
                out->push_back(std::move(loaded));
            }
        }
        if (!storeUnchanged) {
            FolderStore::Write(stored);
        }
    }

    Wh_Log(L"LoadFolders: %zu button(s) from %zu stored folder(s)",
           out->size(), storedCount);

    // Every path that changes the folder list ends up here — init, a settings
    // change, a pin or unpin from Explorer, any edit in the manager window — so
    // this is the one place the real taskbar pins need to be brought back into
    // line. Asking per mutation instead would mean instrumenting eight call
    // sites and still missing the ones that only touch the store indirectly.
    // Non-blocking: it kicks the STA worker and returns.
    Pins::RequestReconcile();
}

// True if `path` already has a button on the taskbar. Used to hide the "Pin to
// Taskbar" menu item — a draft deliberately does not count, so a folder that
// was unpinned can be pinned again from Explorer.
bool IsPinnedTaskbarFolder(const std::wstring& path) {
    auto stored = FolderStore::Read();
    int index = FolderStore::IndexOfPath(stored, path);
    return index >= 0 && stored[index].pinned;
}

// Adds a folder as a new taskbar button, persisted in the mod's own storage.
// Caller must refresh (LoadSettings + UI teardown/rebuild) for it to appear.
// An existing draft is re-pinned in place, keeping its name and icon, rather
// than becoming a second entry for the same folder. No-op (returns false) only
// if the folder is already pinned.
bool AddPinnedFolder(const std::wstring& path, const std::wstring& name) {
    // Gathered before the lock: both do filesystem I/O (desktop.ini, a file
    // handle for the id), and the rest of the code carefully keeps that kind
    // of I/O off FolderStore::g_mutex (see the todo copy in Reconcile). Wasted
    // if this turns out to be a re-pin of an existing entry, but that is
    // cheaper than blocking every other store access on it.
    std::wstring icon = ReadFolderCustomIcon(path);
    uint64_t fileId = GetDirFileId(path);

    // Held across read-modify-write: the manager thread and the taskbar UI
    // thread mutate the same store.
    std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
    auto stored = FolderStore::Read();
    if (int index = FolderStore::IndexOfPath(stored, path); index >= 0) {
        if (stored[index].pinned) {
            return false;
        }
        stored[index].pinned = true;
        FolderStore::Write(stored);
        Wh_Log(L"Re-pinned the draft entry for '%s'", path.c_str());
        return true;
    }
    FolderStore::Entry entry;
    entry.path = path;
    entry.name = name;
    // No icon field in the right-click pin flow, so this is the only chance to
    // pick up a custom folder icon.
    entry.icon = std::move(icon);
    entry.fileId = fileId;
    entry.pinned = true;
    stored.push_back(std::move(entry));
    FolderStore::Write(stored);
    Wh_Log(L"Pinned '%s'", path.c_str());
    return true;
}

// Unpins without forgetting: the entry stays as a draft, keeping its name and
// icon, and still blocks a duplicate pin of the same folder.
void RemovePinnedFolder(const std::wstring& path) {
    std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
    auto stored = FolderStore::Read();
    int index = FolderStore::IndexOfPath(stored, path);
    if (index < 0) {
        Wh_Log(L"Unpin: '%s' has no stored entry", path.c_str());
        return;
    }
    stored[index].pinned = false;
    FolderStore::Write(stored);
    Wh_Log(L"Unpinned '%s' (kept as a draft)", path.c_str());
}

// Seeded from the setting's on-disk value at Wh_ModInit (LoadSettings runs
// there first), not lazily on first Wh_ModSettingsChanged call — that call
// only fires after a change, so a lazy static would initialize itself from
// the value the user just ticked and miss that very rising edge.
void LoadSettings() {
    LoadFolders(&g_settings.folders);

    // "type" was the old folders-first option, which is now unconditional.
    std::wstring sortBy = GetStringSetting(L"content.sortBy");
    g_settings.sortBy =
        sortBy == L"modified" ? SortMode::Modified : SortMode::Name;

    g_settings.hoverDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"behavior.hoverDelayMs"), 0, 5000);
    g_settings.closeDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"behavior.closeDelayMs"), 0, 5000);
    g_settings.columns = std::clamp<int>(Wh_GetIntSetting(L"content.columns"), 0, 24);
    g_settings.maxItems =
        std::clamp<int>(Wh_GetIntSetting(L"content.maxItems"), 0, 400);
    g_settings.includeSubfolders = Wh_GetIntSetting(L"content.includeSubfolders");
    g_settings.maxFolderDepth =
        std::clamp<int>(Wh_GetIntSetting(L"content.maxFolderDepth"), -1, 32);
    g_settings.submenuDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"content.submenuDelayMs"), 0, 5000);
    g_settings.submenuCloseDelayMs =
        std::clamp<int>(Wh_GetIntSetting(L"content.submenuCloseDelayMs"), 0, 5000);
    g_settings.showHidden = Wh_GetIntSetting(L"content.showHidden");
    g_settings.showExtensions = Wh_GetIntSetting(L"content.showExtensions");
    // Cell padding grows with the icon rather than staying fixed, so the label
    // keeps its room at the small end. The small/medium/large numbers are the
    // originals — the three steps below them were added later, so an existing
    // setting keeps the size it already had.
    std::wstring itemSize = GetStringSetting(L"appearance.itemSize");
    if (itemSize == L"smallest") {
        g_settings.iconSize = 12;
        g_settings.cellWidth = 56;
        g_settings.cellHeight = 52;
    } else if (itemSize == L"tiny") {
        g_settings.iconSize = 16;
        g_settings.cellWidth = 62;
        g_settings.cellHeight = 58;
    } else if (itemSize == L"xsmall") {
        g_settings.iconSize = 20;
        g_settings.cellWidth = 70;
        g_settings.cellHeight = 66;
    } else if (itemSize == L"small") {
        g_settings.iconSize = 24;
        g_settings.cellWidth = 76;
        g_settings.cellHeight = 72;
    } else if (itemSize == L"large") {
        g_settings.iconSize = 48;
        g_settings.cellWidth = 120;
        g_settings.cellHeight = 112;
    } else {
        // medium, and anything unrecognised
        g_settings.iconSize = 32;
        g_settings.cellWidth = 92;
        g_settings.cellHeight = 88;
    }
    g_settings.showLabels = Wh_GetIntSetting(L"appearance.showLabels");
    g_settings.fontSize = std::clamp<int>(Wh_GetIntSetting(L"appearance.fontSize"), 6, 48);
    g_settings.itemFontWeight = ParseFontWeight(
        GetStringSetting(L"appearance.itemFontWeight"), FW_NORMAL);
    g_settings.showTitle = Wh_GetIntSetting(L"appearance.showTitle");
    g_settings.titleFontSize =
        std::clamp<int>(Wh_GetIntSetting(L"appearance.titleFontSize"), 6, 48);
    g_settings.titleFontWeight =
        ParseFontWeight(GetStringSetting(L"appearance.titleFontWeight"), FW_BOLD);
    g_settings.titleAlign = GetStringSetting(L"appearance.titleAlign");
    if (g_settings.titleAlign != L"left" && g_settings.titleAlign != L"right") {
        g_settings.titleAlign = L"center";
    }
    g_settings.cornerRadius = Wh_GetIntSetting(L"appearance.roundedCorners")
                                  ? kRoundedCornerRadius
                                  : 0;
    std::wstring popupTheme = GetStringSetting(L"appearance.popupTheme");
    g_settings.popupThemeOverride =
        popupTheme == L"light" ? 0 : (popupTheme == L"dark" ? 1 : -1);
    g_settings.panelOpacity =
        std::clamp<int>(Wh_GetIntSetting(L"appearance.panelOpacity"), 0, 100);
    std::wstring blurType = GetStringSetting(L"appearance.blurType");
    g_settings.blurType = blurType == L"gaussian" ? BlurType::Gaussian
                          : blurType == L"none"   ? BlurType::None
                                                  : BlurType::Acrylic;
    g_settings.blurStrength =
        std::clamp<int>(Wh_GetIntSetting(L"appearance.blurStrength"), 0, 100);
    g_settings.gapAbove =
        std::clamp<int>(Wh_GetIntSetting(L"appearance.gapAbove"), 0, 200);
    g_settings.openFolderOnClick = Wh_GetIntSetting(L"behavior.openFolderOnClick");
    g_settings.explorerMenu = Wh_GetIntSetting(L"behavior.explorerMenu") != 0;
}

////////////////////////////////////////////////////////////////////////////////
// taskbar.dll plumbing
//
// Shared boilerplate from the Windhawk taskbar mods: reach the taskbar's
// XamlRoot through CTaskBand -> TaskbarHost, and marshal work onto the taskbar
// UI thread.

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void* result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void* result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] || !taskbarHostSharedPtr[1]) {
        if (taskbarHostSharedPtr[1] && std__Ref_count_base__Decref_Original) {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }
    if (!TaskbarHost_FrameHeight_Original ||
        !std__Ref_count_base__Decref_Original) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetPrimaryTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (!CTaskBand_GetTaskbarHost_Original ||
        !CTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    if (!CSecondaryTaskBand_GetTaskbarHost_Original ||
        !CSecondaryTaskBand_ITaskListWndSite_vftable) {
        return nullptr;
    }

    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    WCHAR className[32];
    if (!GetClassName(hTaskbarWnd, className, ARRAYSIZE(className))) {
        return nullptr;
    }
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return GetPrimaryTaskbarXamlRoot(hTaskbarWnd);
    }
    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return GetSecondaryTaskbarXamlRoot(hTaskbarWnd);
    }
    return nullptr;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
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
                    // The hook is thread-wide, so a second in-flight call from
                    // another thread puts a second copy of this proc in the
                    // chain — and both copies match this one message and this
                    // one param. Running the callback twice would double-run
                    // the caller's proc against a param it may have already
                    // torn down. Claim it instead: the param lives on
                    // the sender's stack and both copies run synchronously on
                    // this thread, so no lock is needed (and a lock held across
                    // SendMessage is the hazard the rest of the mod avoids).
                    if (auto proc = param->proc) {
                        param->proc = nullptr;
                        proc(param->procParam);
                    }
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

    // A window destroyed between GetWindowThreadProcessId and the send makes
    // SendMessage return without dispatching, so the hook never fires. Report
    // that honestly: a caller that hands ownership over on a true return would
    // leak whatever it passed, and Wh_ModUninit's teardown call would skip its
    // warning on a false negative. Whichever hook copy ran the callback
    // cleared param.proc, so this is already the answer.
    return param.proc == nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Visual tree helpers

FrameworkElement FindChildByName(FrameworkElement const& element,
                                 std::wstring_view name,
                                 int maxDepth = 24) {
    if (!element || maxDepth <= 0) {
        return nullptr;
    }
    int count = VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
        if (auto found = FindChildByName(child, name, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    if (!root) {
        return nullptr;
    }
    FrameworkElement taskbarFrame = nullptr;
    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (child && winrt::get_class_name(child) == L"Taskbar.TaskbarFrame") {
            taskbarFrame = child;
            break;
        }
    }
    if (!taskbarFrame) {
        return nullptr;
    }
    auto rootGrid = FindChildByName(taskbarFrame, L"RootGrid", 4);
    return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
}

// The repeater's realized children are not in layout order, so anything that
// depends on "leftmost" or "rightmost" is resolved geometrically instead.
void EnumRepeaterChildren(FrameworkElement const& repeater,
                          std::vector<FrameworkElement>* out) {
    out->clear();
    if (!repeater) {
        return;
    }
    int count = VisualTreeHelper::GetChildrenCount(repeater);
    for (int i = 0; i < count; i++) {
        auto child =
            VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (child) {
            out->push_back(child);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// GDI+ and icon helpers

ULONG_PTR g_gdiplusToken = 0;

struct ThemeCache {
    bool dark = true;
    BYTE accentR = 0;
    BYTE accentG = 120;
    BYTE accentB = 215;
    bool variableFontAvailable = false;
    bool resolved = false;
};

ThemeCache g_themeCache;

void RefreshThemeCache() {
    try {
        winrt::Windows::UI::ViewManagement::UISettings settings;
        auto bg = settings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Background);
        g_themeCache.dark = ((int)bg.R + bg.G + bg.B) < 384;

        auto accent = settings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        g_themeCache.accentR = accent.R;
        g_themeCache.accentG = accent.G;
        g_themeCache.accentB = accent.B;
    } catch (...) {
        g_themeCache.dark = true;
        g_themeCache.accentR = 0;
        g_themeCache.accentG = 120;
        g_themeCache.accentB = 215;
    }

    Gdiplus::FontFamily probe(L"Segoe UI Variable Text");
    g_themeCache.variableFontAvailable = probe.IsAvailable() != FALSE;
    g_themeCache.resolved = true;
}

bool IsDarkTheme() {
    if (!g_themeCache.resolved) {
        RefreshThemeCache();
    }
    return g_themeCache.dark;
}

// Theme the hover grid paints itself with: the popupTheme setting when it
// forces light or dark, the system app theme otherwise. The taskbar button
// highlight keeps following the system unconditionally so it stays consistent
// with the shell's own icons.
bool IsDarkPopupTheme() {
    if (g_settings.popupThemeOverride >= 0) {
        return g_settings.popupThemeOverride != 0;
    }
    return IsDarkTheme();
}

Gdiplus::Color GetAccentGdipColor(BYTE alpha) {
    if (!g_themeCache.resolved) {
        RefreshThemeCache();
    }
    return Gdiplus::Color(alpha, g_themeCache.accentR, g_themeCache.accentG,
                          g_themeCache.accentB);
}

PCWSTR PopupFontName() {
    if (!g_themeCache.resolved) {
        RefreshThemeCache();
    }
    return g_themeCache.variableFontAvailable ? L"Segoe UI Variable Text"
                                              : L"Segoe UI";
}

// Built from a LOGFONT rather than a FontFamily + FontStyle so lfWeight goes to
// the GDI font mapper, which resolves it against the family's real members.
// Falls back to the FontFamily path if the mapper gives us something GDI+
// cannot use.
std::unique_ptr<Gdiplus::Font> MakePopupFont(int sizePx, int weight) {
    // Segoe UI Variable Text matches the surrounding shell and carries the CJK
    // /Thai/Devanagari/emoji coverage a grid of filenames needs; Arial would
    // drop those to GDI+ per-glyph font linking.
    std::wstring name = PopupFontName();

    LOGFONTW lf{};
    // Negative lfHeight is the em size in pixels, matching what UnitPixel meant
    // to the FontFamily constructor this replaced.
    lf.lfHeight = -sizePx;
    lf.lfWeight = weight;
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfOutPrecision = OUT_TT_PRECIS;
    lf.lfQuality = CLEARTYPE_QUALITY;
    wcsncpy_s(lf.lfFaceName, name.c_str(), _TRUNCATE);

    if (HDC hdc = GetDC(nullptr)) {
        auto font = std::make_unique<Gdiplus::Font>(hdc, &lf);
        ReleaseDC(nullptr, hdc);
        if (font && font->GetLastStatus() == Gdiplus::Ok) {
            return font;
        }
    }

    Gdiplus::FontFamily fallbackFamily(name.c_str());
    return std::make_unique<Gdiplus::Font>(
        &fallbackFamily, (Gdiplus::REAL)sizePx,
        weight >= FW_BOLD ? Gdiplus::FontStyleBold
                              : Gdiplus::FontStyleRegular,
        Gdiplus::UnitPixel);
}

// True when the alpha channel carries real transparency (partial alpha, or a
// mix of fully transparent and opaque). All-0 or all-255 means the AND mask
// must supply transparency instead — common for classic XOR+mask icons.
bool IconPixelsHaveUsefulAlpha(const BYTE* bgra,
                               int width,
                               int height,
                               int stride) {
    if (!bgra || width <= 0 || height <= 0) {
        return false;
    }
    bool sawTransparent = false;
    bool sawOpaque = false;
    bool sawPartial = false;
    for (int y = 0; y < height; y++) {
        const BYTE* row = bgra + (size_t)y * stride;
        for (int x = 0; x < width; x++) {
            BYTE a = row[(size_t)x * 4 + 3];
            if (a == 0) {
                sawTransparent = true;
            } else if (a == 255) {
                sawOpaque = true;
            } else {
                sawPartial = true;
            }
            if (sawPartial || (sawTransparent && sawOpaque)) {
                return true;
            }
        }
    }
    return false;
}

// AND mask bit: 1/white = transparent, 0/black = opaque. Bits are MSB-first
// within each byte; rows are DWORD-aligned.
bool IconMaskBitTransparent(const BYTE* maskBits,
                            int maskWidthBytes,
                            int x,
                            int y) {
    const BYTE* row = maskBits + (size_t)y * maskWidthBytes;
    BYTE b = row[x >> 3];
    return (b & (0x80 >> (x & 7))) != 0;
}

// Pull a top-down 32bpp BGRA copy of an HBITMAP via GetDIBits (preserves the
// alpha byte for existing 32bpp sources on modern Windows).
bool CopyHbitmapToBgra32(HBITMAP hbm,
                         std::vector<BYTE>* out,
                         int* outW,
                         int* outH) {
    if (!hbm || !out || !outW || !outH) {
        return false;
    }
    BITMAP bm{};
    if (!GetObject(hbm, sizeof(bm), &bm) || bm.bmWidth <= 0 ||
        bm.bmHeight <= 0) {
        return false;
    }
    const int w = bm.bmWidth;
    const int h = bm.bmHeight;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    out->assign((size_t)w * h * 4, 0);
    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return false;
    }
    int got = GetDIBits(hdc, hbm, 0, h, out->data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, hdc);
    if (got != h) {
        return false;
    }
    *outW = w;
    *outH = h;
    return true;
}

// Clear junk RGB under transparent pixels and premultiply for PARGB drawing.
// Classic XOR icons often leave white/gray in alpha==0 cells; SourceOver onto a
// dark PARGB panel then reads as a faint lighter veil. Near-zero alpha with
// near-white/near-black RGB (mask/DrawIconEx noise) is treated as fully clear
// so soft edges of modern icons are preserved.
void SanitizeAndPremultiplyBgra(BYTE* bgra,
                                int width,
                                int height,
                                int stride) {
    if (!bgra || width <= 0 || height <= 0) {
        return;
    }
    for (int y = 0; y < height; y++) {
        BYTE* row = bgra + (size_t)y * stride;
        for (int x = 0; x < width; x++) {
            BYTE* p = row + (size_t)x * 4;
            BYTE b = p[0], g = p[1], r = p[2], a = p[3];
            if (a == 0) {
                p[0] = p[1] = p[2] = 0;
                continue;
            }
            if (a < 8) {
                const int mx = (std::max)((int)r, (std::max)((int)g, (int)b));
                const int mn = (std::min)((int)r, (std::min)((int)g, (int)b));
                const bool nearWhite = mn >= 220;
                const bool nearBlack = mx <= 35;
                if (nearWhite || nearBlack) {
                    p[0] = p[1] = p[2] = p[3] = 0;
                    continue;
                }
            }
            p[0] = (BYTE)((int)b * a / 255);
            p[1] = (BYTE)((int)g * a / 255);
            p[2] = (BYTE)((int)r * a / 255);
        }
    }
}

// After scaling/filtering a PARGB bitmap, re-clear fully transparent pixels and
// clamp channels so rgb never exceeds alpha (invalid premul from bicubic).
void SanitizeParGbBitmap(Gdiplus::Bitmap* bmp) {
    if (!bmp) {
        return;
    }
    const int w = (int)bmp->GetWidth();
    const int h = (int)bmp->GetHeight();
    if (w <= 0 || h <= 0) {
        return;
    }
    Gdiplus::BitmapData bd{};
    Gdiplus::Rect lockRect(0, 0, w, h);
    if (bmp->LockBits(&lockRect,
                      (Gdiplus::ImageLockMode)(Gdiplus::ImageLockModeRead |
                                               Gdiplus::ImageLockModeWrite),
                      PixelFormat32bppPARGB, &bd) != Gdiplus::Ok) {
        return;
    }
    for (int y = 0; y < h; y++) {
        BYTE* row = (BYTE*)bd.Scan0 + (size_t)y * bd.Stride;
        for (int x = 0; x < w; x++) {
            BYTE* p = row + (size_t)x * 4;
            BYTE a = p[3];
            if (a == 0) {
                p[0] = p[1] = p[2] = 0;
                continue;
            }
            if (a < 8) {
                const int mx =
                    (std::max)((int)p[0], (std::max)((int)p[1], (int)p[2]));
                // Premultiplied near-white junk is still bright relative to alpha.
                const bool brightJunk = mx >= (int)a * 200 / 255 && mx >= 6;
                const bool nearBlack = mx <= 2;
                if (brightJunk || nearBlack) {
                    p[0] = p[1] = p[2] = p[3] = 0;
                    continue;
                }
            }
            if (p[0] > a) {
                p[0] = a;
            }
            if (p[1] > a) {
                p[1] = a;
            }
            if (p[2] > a) {
                p[2] = a;
            }
        }
    }
    bmp->UnlockBits(&bd);
}

// Apply the icon's 1-bit AND mask as alpha. For color icons the mask matches
// the color size (or a double-height legacy layout); for monochrome icons the
// AND plane is the top half. Destination may differ in size from the mask —
// coordinates are mapped so scaled DrawIconEx buffers still work.
void ApplyIconAndMaskToBgra(BYTE* bgra,
                            int width,
                            int height,
                            HBITMAP hbmMask,
                            bool monoIcon) {
    if (!bgra || !hbmMask || width <= 0 || height <= 0) {
        return;
    }

    BITMAP bmMask{};
    if (!GetObject(hbmMask, sizeof(bmMask), &bmMask) || bmMask.bmWidth <= 0 ||
        bmMask.bmHeight <= 0) {
        return;
    }

    const int maskW = bmMask.bmWidth;
    const int maskH = bmMask.bmHeight;
    int andRows;
    if (monoIcon) {
        andRows = maskH / 2;
    } else if (maskH >= height * 2 && height > 0) {
        andRows = height;
    } else {
        andRows = maskH;
    }
    if (andRows <= 0) {
        return;
    }

    struct {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD bmiColors[2];
    } bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = maskW;
    bi.bmiHeader.biHeight = -maskH;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 1;
    bi.bmiHeader.biCompression = BI_RGB;
    bi.bmiColors[0] = {0, 0, 0, 0};
    bi.bmiColors[1] = {255, 255, 255, 0};

    const int maskStride = ((maskW + 31) / 32) * 4;
    std::vector<BYTE> maskBits((size_t)maskStride * maskH, 0);

    HDC hdc = GetDC(nullptr);
    if (!hdc) {
        return;
    }
    int got = GetDIBits(hdc, hbmMask, 0, maskH, maskBits.data(),
                        reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);
    ReleaseDC(nullptr, hdc);
    if (got <= 0) {
        return;
    }

    for (int y = 0; y < height; y++) {
        const int my = (y * andRows) / height;
        BYTE* row = bgra + (size_t)y * width * 4;
        for (int x = 0; x < width; x++) {
            const int mx = (x * maskW) / width;
            if (IconMaskBitTransparent(maskBits.data(), maskStride, mx, my)) {
                row[(size_t)x * 4 + 0] = 0;
                row[(size_t)x * 4 + 1] = 0;
                row[(size_t)x * 4 + 2] = 0;
                row[(size_t)x * 4 + 3] = 0;
            } else if (row[(size_t)x * 4 + 3] == 0) {
                // Opaque under the mask but alpha was never filled — make solid.
                row[(size_t)x * 4 + 3] = 255;
            }
        }
    }
}

// `bgra` must already be premultiplied (SanitizeAndPremultiplyBgra). Output is
// PixelFormat32bppPARGB to match RebuildLevelBase / UpdateLayeredWindow.
std::shared_ptr<Gdiplus::Bitmap> BitmapFromBgraScaled(const BYTE* bgra,
                                                      int srcW,
                                                      int srcH,
                                                      int size) {
    if (!bgra || srcW <= 0 || srcH <= 0 || size <= 0) {
        return nullptr;
    }

    std::shared_ptr<Gdiplus::Bitmap> result;
    try {
        Gdiplus::Bitmap source(srcW, srcH, srcW * 4, PixelFormat32bppPARGB,
                               const_cast<BYTE*>(bgra));
        if (source.GetLastStatus() != Gdiplus::Ok) {
            return nullptr;
        }
        result =
            std::make_shared<Gdiplus::Bitmap>(size, size, PixelFormat32bppPARGB);
        if (!result || result->GetLastStatus() != Gdiplus::Ok) {
            return nullptr;
        }
        Gdiplus::Graphics g(result.get());
        if (g.GetLastStatus() != Gdiplus::Ok) {
            return nullptr;
        }
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));
        g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        if (g.DrawImage(&source, 0, 0, size, size) != Gdiplus::Ok) {
            return nullptr;
        }
        SanitizeParGbBitmap(result.get());
    } catch (...) {
        return nullptr;
    }
    return result;
}

// An icon is treated as artwork centred in an oversized slot, rather than as an
// icon with its own generous padding, only when it covers less than this much of
// its canvas. Real icons run 80-100%; a 48px icon in a 256px slot is 19%.
constexpr int kIconCanvasFillPercent = 62;

// SHIL_JUMBO image-list entries are 256x256 slots, and an app whose icon has no
// 256px variant gets its smaller artwork centred in that slot instead of scaled
// up to fill it. Scaling the whole slot down to the requested size then renders
// the artwork at a fraction of that size. Crop back to the artwork when it fills
// only a small part of the canvas; ordinary icons are left untouched.
bool CropIconCanvasPadding(std::vector<BYTE>& bgra, int* w, int* h) {
    int width = *w;
    int height = *h;
    if (width <= 0 || height <= 0 ||
        bgra.size() < (size_t)width * height * 4) {
        return false;
    }

    int minX = width;
    int minY = height;
    int maxX = -1;
    int maxY = -1;
    for (int y = 0; y < height; y++) {
        const BYTE* row = bgra.data() + (size_t)y * width * 4;
        for (int x = 0; x < width; x++) {
            if (row[x * 4 + 3] == 0) {
                continue;
            }
            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            minY = std::min(minY, y);
            maxY = std::max(maxY, y);
        }
    }
    if (maxX < minX || maxY < minY) {
        return false;  // Fully transparent; nothing to crop to.
    }

    int contentW = maxX - minX + 1;
    int contentH = maxY - minY + 1;
    if (contentW * 100 > width * kIconCanvasFillPercent ||
        contentH * 100 > height * kIconCanvasFillPercent) {
        return false;
    }

    // Square, centred on the artwork, so the aspect ratio survives the scale.
    int side = std::min({std::max(contentW, contentH), width, height});
    int left = std::clamp((minX + maxX) / 2 - side / 2, 0, width - side);
    int top = std::clamp((minY + maxY) / 2 - side / 2, 0, height - side);

    std::vector<BYTE> cropped((size_t)side * side * 4);
    for (int y = 0; y < side; y++) {
        memcpy(cropped.data() + (size_t)y * side * 4,
               bgra.data() + (size_t)(top + y) * width * 4 + (size_t)left * 4,
               (size_t)side * 4);
    }
    bgra.swap(cropped);
    *w = side;
    *h = side;
    return true;
}

// Convert HICON to a sized 32bpp PARGB bitmap for every icon type:
// 1) Prefer color-plane pixels that already carry useful alpha (modern ARGB).
// 2) Otherwise apply the 1-bit AND mask (black=opaque, white=transparent) so
//    classic XOR+mask icons never keep white/black bogus backgrounds.
// 3) Sanitize transparent RGB + premultiply, then scale as PARGB.
// 4) Last resort: DrawIconEx onto a zero-cleared 32bpp DIB, then mask if needed.
std::shared_ptr<Gdiplus::Bitmap> HIconToBitmap(HICON hIcon, int size) {
    if (!hIcon || size <= 0) {
        return nullptr;
    }

    ICONINFO ii{};
    if (GetIconInfo(hIcon, &ii)) {
        const bool monoIcon = (ii.hbmColor == nullptr);
        std::vector<BYTE> bgra;
        int srcW = 0;
        int srcH = 0;
        bool havePixels = false;

        if (ii.hbmColor) {
            havePixels = CopyHbitmapToBgra32(ii.hbmColor, &bgra, &srcW, &srcH);
        } else if (ii.hbmMask) {
            // Monochrome: reconstruct color from the XOR (bottom) half via
            // DrawIconEx onto a cleared DIB at the mask's logical size.
            BITMAP bmMask{};
            if (GetObject(ii.hbmMask, sizeof(bmMask), &bmMask) &&
                bmMask.bmWidth > 0 && bmMask.bmHeight >= 2) {
                srcW = bmMask.bmWidth;
                srcH = bmMask.bmHeight / 2;
                BITMAPINFO bi{};
                bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bi.bmiHeader.biWidth = srcW;
                bi.bmiHeader.biHeight = -srcH;
                bi.bmiHeader.biPlanes = 1;
                bi.bmiHeader.biBitCount = 32;
                bi.bmiHeader.biCompression = BI_RGB;

                HDC hScreen = GetDC(nullptr);
                if (hScreen) {
                    HDC hMem = CreateCompatibleDC(hScreen);
                    ReleaseDC(nullptr, hScreen);
                    if (hMem) {
                        void* bits = nullptr;
                        HBITMAP hDib = CreateDIBSection(hMem, &bi, DIB_RGB_COLORS,
                                                        &bits, nullptr, 0);
                        if (hDib && bits) {
                            HGDIOBJ old = SelectObject(hMem, hDib);
                            memset(bits, 0, (size_t)srcW * srcH * 4);
                            DrawIconEx(hMem, 0, 0, hIcon, srcW, srcH, 0, nullptr,
                                       DI_NORMAL);
                            SelectObject(hMem, old);
                            GdiFlush();
                            bgra.assign((BYTE*)bits,
                                        (BYTE*)bits + (size_t)srcW * srcH * 4);
                            havePixels = true;
                            DeleteObject(hDib);
                        }
                        DeleteDC(hMem);
                    }
                }
            }
        }

        if (havePixels && srcW > 0 && srcH > 0) {
            if (!IconPixelsHaveUsefulAlpha(bgra.data(), srcW, srcH, srcW * 4) &&
                ii.hbmMask) {
                ApplyIconAndMaskToBgra(bgra.data(), srcW, srcH, ii.hbmMask,
                                       monoIcon);
            }
            CropIconCanvasPadding(bgra, &srcW, &srcH);
            SanitizeAndPremultiplyBgra(bgra.data(), srcW, srcH, srcW * 4);
            auto scaled =
                BitmapFromBgraScaled(bgra.data(), srcW, srcH, size);
            if (ii.hbmColor) {
                DeleteObject(ii.hbmColor);
            }
            if (ii.hbmMask) {
                DeleteObject(ii.hbmMask);
            }
            if (scaled) {
                return scaled;
            }
        } else {
            if (ii.hbmColor) {
                DeleteObject(ii.hbmColor);
            }
            if (ii.hbmMask) {
                DeleteObject(ii.hbmMask);
            }
        }
    }

    // Last resort: DrawIconEx into a zero-cleared 32bpp DIB, then re-apply the
    // AND mask when the draw left a useless alpha channel (XOR garbage RGB
    // such as opaque white in transparent cells).
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) {
        return nullptr;
    }
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    ReleaseDC(nullptr, hScreenDC);
    if (!hMemDC) {
        return nullptr;
    }

    void* bits = nullptr;
    HBITMAP hBmp =
        CreateDIBSection(hMemDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        DeleteDC(hMemDC);
        return nullptr;
    }

    HGDIOBJ hOld = SelectObject(hMemDC, hBmp);
    memset(bits, 0, (size_t)size * size * 4);
    DrawIconEx(hMemDC, 0, 0, hIcon, size, size, 0, nullptr, DI_NORMAL);
    SelectObject(hMemDC, hOld);
    GdiFlush();

    BYTE* px = (BYTE*)bits;
    if (!IconPixelsHaveUsefulAlpha(px, size, size, size * 4)) {
        ICONINFO maskInfo{};
        if (GetIconInfo(hIcon, &maskInfo)) {
            ApplyIconAndMaskToBgra(px, size, size, maskInfo.hbmMask,
                                   maskInfo.hbmColor == nullptr);
            if (maskInfo.hbmColor) {
                DeleteObject(maskInfo.hbmColor);
            }
            if (maskInfo.hbmMask) {
                DeleteObject(maskInfo.hbmMask);
            }
        } else {
            // No mask available: opaque where any color was drawn.
            for (size_t i = 0; i < (size_t)size * size * 4; i += 4) {
                if (px[i] || px[i + 1] || px[i + 2]) {
                    px[i + 3] = 255;
                }
            }
        }
    }
    SanitizeAndPremultiplyBgra(px, size, size, size * 4);

    std::shared_ptr<Gdiplus::Bitmap> result;
    try {
        result =
            std::make_shared<Gdiplus::Bitmap>(size, size, PixelFormat32bppPARGB);
        if (result->GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::BitmapData bd{};
            Gdiplus::Rect lockRect(0, 0, size, size);
            if (result->LockBits(&lockRect, Gdiplus::ImageLockModeWrite,
                                 PixelFormat32bppPARGB, &bd) == Gdiplus::Ok) {
                for (int y = 0; y < size; y++) {
                    memcpy((BYTE*)bd.Scan0 + (size_t)y * bd.Stride,
                           px + (size_t)y * size * 4, (size_t)size * 4);
                }
                result->UnlockBits(&bd);
                SanitizeParGbBitmap(result.get());
            } else {
                result.reset();
            }
        } else {
            result.reset();
        }
    } catch (...) {
        result.reset();
    }

    DeleteObject(hBmp);
    DeleteDC(hMemDC);

    return result;
}

// Not exported by the MinGW uuid library, so it is spelled out here.
constexpr GUID kIImageListGuid = {
    0x46eb5926,
    0x582e,
    0x4017,
    {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};

// Pick the system image list whose native size best covers the requested size.
int ShilForSize(int pixelSize) {
    if (pixelSize <= 16) {
        return SHIL_SMALL;
    }
    if (pixelSize <= 32) {
        return SHIL_LARGE;
    }
    if (pixelSize <= 48) {
        return SHIL_EXTRALARGE;
    }
    return SHIL_JUMBO;
}

bool PathIsLnkFile(const std::wstring& path) {
    return path.size() >= 4 &&
           _wcsicmp(path.c_str() + (path.size() - 4), L".lnk") == 0;
}

// SHDefExtractIconW does not composite the shortcut overlay arrow.
HICON ExtractIconDef(const std::wstring& path, int index, int pixelSize) {
    HICON hLarge = nullptr;
    HICON hSmall = nullptr;
    if (SUCCEEDED(SHDefExtractIconW(path.c_str(), index, 0, &hLarge, &hSmall,
                                    (UINT)pixelSize)) &&
        hLarge) {
        if (hSmall) {
            DestroyIcon(hSmall);
        }
        return hLarge;
    }
    if (hSmall) {
        return hSmall;
    }
    return nullptr;
}

// System image list index without drawing overlays (no SHGFI_LINKOVERLAY /
// SHGFI_ICON, which bake the shortcut arrow into .lnk icons).
HICON GetShellIconFromImageList(const std::wstring& path, int pixelSize) {
    SHFILEINFOW sfi{};
    if (!SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                        SHGFI_SYSICONINDEX)) {
        return nullptr;
    }

    HICON hIcon = nullptr;
    IImageList* imageList = nullptr;
    if (SUCCEEDED(SHGetImageList(ShilForSize(pixelSize), kIImageListGuid,
                                 (void**)&imageList)) &&
        imageList) {
        imageList->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
        imageList->Release();
    }
    return hIcon;
}

// Resolve a .lnk's custom icon location or target path and extract without the
// shell link overlay.
HICON GetIconFromShortcutFile(const std::wstring& lnkPath, int pixelSize) {
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, (void**)&link)) ||
        !link) {
        return nullptr;
    }

    IPersistFile* persist = nullptr;
    HRESULT hr = link->QueryInterface(IID_IPersistFile, (void**)&persist);
    if (FAILED(hr) || !persist) {
        link->Release();
        return nullptr;
    }

    hr = persist->Load(lnkPath.c_str(), STGM_READ);
    persist->Release();
    if (FAILED(hr)) {
        link->Release();
        return nullptr;
    }

    WCHAR iconLoc[MAX_PATH]{};
    int iconIndex = 0;
    hr = link->GetIconLocation(iconLoc, ARRAYSIZE(iconLoc), &iconIndex);
    if (SUCCEEDED(hr) && iconLoc[0]) {
        WCHAR expanded[MAX_PATH]{};
        PCWSTR file = iconLoc;
        DWORD expandedLen =
            ExpandEnvironmentStringsW(iconLoc, expanded, ARRAYSIZE(expanded));
        if (expandedLen > 0 && expandedLen <= ARRAYSIZE(expanded)) {
            file = expanded;
        }
        HICON hIcon = ExtractIconDef(file, iconIndex, pixelSize);
        if (hIcon) {
            link->Release();
            return hIcon;
        }
    }

    WCHAR target[MAX_PATH]{};
    hr = link->GetPath(target, ARRAYSIZE(target), nullptr, SLGP_RAWPATH);
    if (FAILED(hr) || !target[0]) {
        hr = link->GetPath(target, ARRAYSIZE(target), nullptr, 0);
    }
    link->Release();
    if (FAILED(hr) || !target[0]) {
        return nullptr;
    }

    WCHAR expandedTarget[MAX_PATH]{};
    PCWSTR useTarget = target;
    DWORD targetLen = ExpandEnvironmentStringsW(target, expandedTarget,
                                                ARRAYSIZE(expandedTarget));
    if (targetLen > 0 && targetLen <= ARRAYSIZE(expandedTarget)) {
        useTarget = expandedTarget;
    }

    if (HICON hIcon = ExtractIconDef(useTarget, 0, pixelSize)) {
        return hIcon;
    }
    return GetShellIconFromImageList(useTarget, pixelSize);
}

HICON GetShellIconForPath(const std::wstring& path, int pixelSize) {
    // Prefer paths that never composite SHGFI_LINKOVERLAY / shortcut arrows.
    if (PathIsLnkFile(path)) {
        if (HICON hIcon = GetIconFromShortcutFile(path, pixelSize)) {
            return hIcon;
        }
        // Extracting from the .lnk itself still avoids the overlay arrow.
        if (HICON hIcon = ExtractIconDef(path, 0, pixelSize)) {
            return hIcon;
        }
    } else if (HICON hIcon = ExtractIconDef(path, 0, pixelSize)) {
        return hIcon;
    }

    if (HICON hIcon = GetShellIconFromImageList(path, pixelSize)) {
        return hIcon;
    }

    // Last resort — SHGFI_ICON may include the shortcut overlay for .lnk files.
    SHFILEINFOW sfi{};
    UINT flags =
        SHGFI_ICON | (pixelSize <= 16 ? SHGFI_SMALLICON : SHGFI_LARGEICON);
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), flags)) {
        return sfi.hIcon;
    }
    return nullptr;
}

// Accepts "C:\path\app.exe,3" as well as a plain image or icon file path.
HICON ExtractIconFromResourceSpec(const std::wstring& spec, int pixelSize) {
    std::wstring file = spec;
    int index = 0;

    size_t comma = spec.find_last_of(L',');
    if (comma != std::wstring::npos && comma + 1 < spec.size()) {
        PCWSTR numStart = spec.c_str() + comma + 1;
        WCHAR* end = nullptr;
        long parsed = wcstol(numStart, &end, 10);
        if (end && *end == L'\0') {
            file = spec.substr(0, comma);
            index = (int)parsed;
        }
    }

    if (HICON hIcon = ExtractIconDef(file, index, pixelSize)) {
        return hIcon;
    }
    return GetShellIconFromImageList(file, pixelSize);
}

////////////////////////////////////////////////////////////////////////////////
// Folder scanning and caching

struct GridItem {
    std::wstring displayName;
    std::wstring fullPath;
    // When a .lnk points at a folder we cascade against the target path, but
    // keep the shortcut itself here so its custom icon still shows.
    std::wstring iconPath;
    bool isFolder = false;
    ULONGLONG modified = 0;
    std::shared_ptr<Gdiplus::Bitmap> icon;
};

struct FolderData {
    std::vector<GridItem> items;
    bool ready = false;
    ULONGLONG scannedAtTick = 0;
    // LRU bookkeeping for FolderCache eviction (separate from scan freshness).
    ULONGLONG lastUsedTick = 0;
    int scannedIconSize = 0;
    // Approximate resident bytes of icon pixel buffers (width*height*4).
    size_t approxBytes = 0;
};

// Bound folder-path → FolderData entries. PrefetchSubfolders can enqueue many
// paths; without a cap the map grows without bound across a session.
constexpr size_t kMaxCachedFolders = 32;
// Soft ceiling on cached icon bitmaps across all folders (~64 MB).
constexpr size_t kMaxCachedFolderBytes = 64ull * 1024 * 1024;

struct ScanRequest {
    std::wstring path;
    int iconPixelSize = 32;
};

// Keyed by resolved folder path so subfolders opened on hover share the same
// cache as the folders configured on the taskbar.
std::mutex g_cacheMutex;
[[clang::no_destroy]] std::optional<
    std::unordered_map<std::wstring, std::shared_ptr<FolderData>>>
    g_folderCache{std::in_place};

std::unordered_map<std::wstring, std::shared_ptr<FolderData>>& FolderCache() {
    if (!g_folderCache) {
        g_folderCache.emplace();
    }
    return *g_folderCache;
}

// How many GridItems (with bitmaps) to retain per cached folder. Prefer an
// on-screen capacity related to ComputeLevelLayout (columns × ~rows) over the
// raw maxItems ceiling — large iconSize values leave few cells visible, and
// caching hundreds of 256–512 px bitmaps is pure RAM waste.
int MaxCachedItemsPerFolder() {
    int cols = g_settings.columns > 0 ? g_settings.columns : 12;
    int maxRows = 16;
    if (g_settings.iconSize >= 128) {
        maxRows = 6;
    } else if (g_settings.iconSize >= 64) {
        maxRows = 10;
    }
    int screenCap = std::clamp(cols * maxRows, 24, 192);
    if (g_settings.maxItems > 0) {
        return std::min(g_settings.maxItems, screenCap);
    }
    return screenCap;
}

size_t EstimateFolderDataBytes(const FolderData& data) {
    size_t bytes = 0;
    const int fallbackPx = data.scannedIconSize > 0 ? data.scannedIconSize : 32;
    const size_t fallbackIconBytes =
        (size_t)fallbackPx * (size_t)fallbackPx * 4;
    for (const auto& item : data.items) {
        if (!item.icon) {
            continue;
        }
        const UINT w = item.icon->GetWidth();
        const UINT h = item.icon->GetHeight();
        if (w > 0 && h > 0) {
            bytes += (size_t)w * (size_t)h * 4;
        } else {
            bytes += fallbackIconBytes;
        }
    }
    return bytes;
}

// Caller must hold g_cacheMutex. Drops least-recently-used entries until both
// the entry cap and the approximate byte budget are satisfied. Never erases
// keepKey (the entry just inserted/used). Erasing only releases the cache's
// shared_ptr; open PopupLevels that copied items (and their shared_ptr icons)
// keep working.
void EvictFolderCacheIfNeeded(const std::wstring& keepKey) {
    auto& cache = FolderCache();
    auto totalBytes = [&]() -> size_t {
        size_t sum = 0;
        for (const auto& entry : cache) {
            if (entry.second) {
                sum += entry.second->approxBytes;
            }
        }
        return sum;
    };

    while (cache.size() > kMaxCachedFolders ||
           (cache.size() > 1 && totalBytes() > kMaxCachedFolderBytes)) {
        auto oldest = cache.end();
        for (auto it = cache.begin(); it != cache.end(); ++it) {
            if (it->first == keepKey) {
                continue;
            }
            if (oldest == cache.end() ||
                it->second->lastUsedTick < oldest->second->lastUsedTick) {
                oldest = it;
            }
        }
        if (oldest == cache.end()) {
            break;
        }
        cache.erase(oldest);
    }
}

// UNC shares and mapped/unavailable network drives — shell I/O on these can
// block for the full SMB/WebDAV timeout. Used to skip sync work on the UI
// thread and to bound scan-thread hangs.
bool IsLikelyRemotePath(const std::wstring& p) {
    if (p.empty()) {
        return false;
    }
    if (PathIsUNCW(p.c_str())) {
        return true;
    }
    if (p.size() >= 2 && p[1] == L':') {
        WCHAR root[4] = {p[0], L':', L'\\', 0};
        UINT type = GetDriveTypeW(root);
        return type == DRIVE_REMOTE || type == DRIVE_NO_ROOT_DIR;
    }
    return false;
}

// File portion of an "app.exe,3" icon resource spec (or the whole string).
std::wstring IconSpecFilePart(const std::wstring& spec) {
    size_t comma = spec.find_last_of(L',');
    if (comma != std::wstring::npos && comma + 1 < spec.size()) {
        PCWSTR numStart = spec.c_str() + comma + 1;
        WCHAR* end = nullptr;
        (void)wcstol(numStart, &end, 10);
        if (end && *end == L'\0') {
            return spec.substr(0, comma);
        }
    }
    return spec;
}

std::mutex g_scanMutex;
std::vector<ScanRequest> g_scanQueue;
// Atomic: ScanFolderInto reads this outside the mutex during icon extraction.
std::atomic<bool> g_scanThreadStop{false};
// True while StopScanThread has swapped the worker out and is joining — blocks
// StartScanThread from spawning a sibling against handles about to be closed.
std::atomic<bool> g_scanThreadJoining{false};
// Manual-reset stop + auto-reset work. Idle wait is MsgWaitForMultipleObjects
// with QS_ALLINPUT (no 50 ms poll). Created lazily with the scan thread.
HANDLE g_scanStopEvent = nullptr;
HANDLE g_scanWorkEvent = nullptr;
// optional + no_destroy: a bare std::thread aborts Explorer on process exit if
// it is still joinable when globals are destroyed.
[[clang::no_destroy]] std::optional<std::thread> g_scanThread;

// Shell icon handlers on an STA thread may create windows and post messages.
// Pump while idle/between extractions so a wait without a message loop cannot
// hang forever; stop still wakes via g_scanStopEvent + g_scanThreadStop.
void PumpScanThreadMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            // Do not tear down the scan thread on an unexpected WM_QUIT.
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

std::wstring CacheKey(const std::wstring& path) {
    std::wstring key = path;
    CharLowerBuffW(key.data(), (DWORD)key.size());
    return key;
}

bool IsShellFolderPath(const std::wstring& path) {
    return path.size() > 6 && _wcsnicmp(path.c_str(), L"shell:", 6) == 0;
}

// A `shell:` target is resolved to a filesystem path so the plain directory
// walk below can handle every configured folder the same way. The shell:
// branch needs a live COM apartment — call only from the scan STA or the
// taskbar UI thread, never from Wh_ModInit / LoadFolders.
std::wstring ResolveFolderPath(const std::wstring& raw) {
    if (IsShellFolderPath(raw)) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(
                SHParseDisplayName(raw.c_str(), nullptr, &pidl, 0, nullptr)) &&
            pidl) {
            WCHAR buf[MAX_PATH]{};
            bool ok = SHGetPathFromIDListW(pidl, buf);
            CoTaskMemFree(pidl);
            if (ok && buf[0]) {
                return buf;
            }
        }
        return L"";
    }

    std::wstring path = raw;
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

void ResolveFolderEntry(FolderEntry& entry) {
    if (entry.resolveFailed || !entry.resolvedPath.empty()) {
        return;
    }
    std::wstring resolved = ResolveFolderPath(entry.path);
    if (resolved.empty()) {
        entry.resolveFailed = true;
        Wh_Log(L"Folder path does not resolve to a filesystem folder: %s",
               entry.path.c_str());
        return;
    }
    entry.resolvedPath = std::move(resolved);
    entry.likelyRemote = IsLikelyRemotePath(entry.resolvedPath);
}

// Fill FolderEntry::resolvedPath for any still-pending shell: entries. Must run
// on an STA that already owns COM (taskbar UI or scan worker) — no CoInit here.
void ResolvePendingFolderEntries() {
    std::lock_guard<std::mutex> lock(g_foldersMutex);
    for (auto& entry : g_settings.folders) {
        if (entry.resolvedPath.empty() && !entry.resolveFailed) {
            ResolveFolderEntry(entry);
        }
    }
}

std::wstring MakeDisplayName(const std::wstring& fileName, bool isFolder) {
    if (isFolder) {
        return fileName;
    }
    size_t dot = fileName.find_last_of(L'.');
    if (dot == std::wstring::npos || dot == 0) {
        return fileName;
    }
    std::wstring ext = fileName.substr(dot);
    // Windows always hides shortcut extensions, regardless of the user's
    // setting.
    if (_wcsicmp(ext.c_str(), L".lnk") == 0 ||
        _wcsicmp(ext.c_str(), L".url") == 0) {
        return fileName.substr(0, dot);
    }
    if (!g_settings.showExtensions) {
        return fileName.substr(0, dot);
    }
    return fileName;
}

// Folder shortcuts (.lnk whose target is a directory) are files to
// FindFirstFile, but Explorer treats them as folders. Resolve the target so
// they can cascade.
std::wstring ResolveFolderShortcutTarget(const std::wstring& path) {
    size_t len = path.size();
    if (len < 5 || _wcsicmp(path.c_str() + (len - 4), L".lnk") != 0) {
        return {};
    }

    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, (void**)&link)) ||
        !link) {
        return {};
    }

    IPersistFile* persist = nullptr;
    HRESULT hr = link->QueryInterface(IID_IPersistFile, (void**)&persist);
    if (FAILED(hr) || !persist) {
        link->Release();
        return {};
    }

    hr = persist->Load(path.c_str(), STGM_READ);
    persist->Release();
    if (FAILED(hr)) {
        link->Release();
        return {};
    }

    // Skip Resolve(): it can hang on unreachable network targets and may show
    // UI.
    WCHAR target[MAX_PATH]{};
    hr = link->GetPath(target, ARRAYSIZE(target), nullptr, SLGP_RAWPATH);
    if (FAILED(hr) || !target[0]) {
        hr = link->GetPath(target, ARRAYSIZE(target), nullptr, 0);
    }
    link->Release();

    if (FAILED(hr) || !target[0]) {
        return {};
    }

    // GetFileAttributesW on an offline share blocks for the network timeout.
    if (IsLikelyRemotePath(target)) {
        return {};
    }

    DWORD attrs = GetFileAttributesW(target);
    if (attrs == INVALID_FILE_ATTRIBUTES ||
        (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        return {};
    }

    std::wstring resolved = target;
    while (resolved.size() > 3 &&
           (resolved.back() == L'\\' || resolved.back() == L'/')) {
        resolved.pop_back();
    }
    return resolved;
}

void ScanFolderInto(const std::wstring& path,
                    int iconPixelSize,
                    FolderData* out) {
    out->items.clear();
    out->scannedIconSize = iconPixelSize;
    out->approxBytes = 0;

    if (path.empty()) {
        return;
    }

    // FindFirstFileExW on an offline share blocks StopScanThread's join.
    if (IsLikelyRemotePath(path)) {
        Wh_Log(L"Skipping scan of remote/unavailable path %s", path.c_str());
        return;
    }

    std::wstring pattern = path + L"\\*";
    WIN32_FIND_DATAW fd{};
    HANDLE hFind = FindFirstFileExW(pattern.c_str(), FindExInfoBasic, &fd,
                                    FindExSearchNameMatch, nullptr,
                                    FIND_FIRST_EX_LARGE_FETCH);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (g_unloading || g_scanThreadStop) {
            break;
        }
        if (wcscmp(fd.cFileName, L".") == 0 ||
            wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }

        bool isRealDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        std::wstring itemPath = path + L"\\" + fd.cFileName;
        std::wstring iconPath;

        bool isFolder = isRealDir;
        if (!isFolder) {
            std::wstring target = ResolveFolderShortcutTarget(itemPath);
            if (!target.empty()) {
                isFolder = true;
                iconPath = itemPath;
                itemPath = std::move(target);
            }
        }

        if (isFolder && !g_settings.includeSubfolders) {
            continue;
        }
        if (!g_settings.showHidden &&
            (fd.dwFileAttributes &
             (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            continue;
        }
        if (_wcsicmp(fd.cFileName, L"desktop.ini") == 0) {
            continue;
        }

        GridItem item;
        item.isFolder = isFolder;
        item.fullPath = std::move(itemPath);
        item.iconPath = std::move(iconPath);
        // Use the on-disk kind for the label so .lnk extensions stay stripped.
        item.displayName = MakeDisplayName(fd.cFileName, isRealDir);
        item.modified = ((ULONGLONG)fd.ftLastWriteTime.dwHighDateTime << 32) |
                        fd.ftLastWriteTime.dwLowDateTime;
        out->items.push_back(std::move(item));
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    // Folders always lead, so the things that cascade sit together at the top
    // of the grid. The sort setting then orders within each group.
    std::sort(out->items.begin(), out->items.end(),
              [](const GridItem& a, const GridItem& b) {
                  if (a.isFolder != b.isFolder) {
                      return a.isFolder;
                  }
                  if (g_settings.sortBy == SortMode::Modified &&
                      a.modified != b.modified) {
                      return a.modified > b.modified;
                  }
                  return StrCmpLogicalW(a.displayName.c_str(),
                                        b.displayName.c_str()) < 0;
              });

    // Trim before icon extraction so we never allocate bitmaps we will discard.
    const int cacheCap = MaxCachedItemsPerFolder();
    if ((int)out->items.size() > cacheCap) {
        out->items.resize(cacheCap);
    }

    for (auto& item : out->items) {
        if (g_unloading || g_scanThreadStop) {
            break;
        }
        const std::wstring& iconSource =
            item.iconPath.empty() ? item.fullPath : item.iconPath;
        if (IsLikelyRemotePath(iconSource)) {
            // Leave null icon; PaintLevel draws without it.
            PumpScanThreadMessages();
            continue;
        }
        HICON hIcon = GetShellIconForPath(iconSource, iconPixelSize);
        if (hIcon) {
            item.icon = HIconToBitmap(hIcon, iconPixelSize);
            DestroyIcon(hIcon);
        }
        // Keep the STA responsive for in-process shell icon handlers.
        PumpScanThreadMessages();
    }

    out->approxBytes = EstimateFolderDataBytes(*out);
}

void StartScanThread();

void ScanThreadMain() {
    // Shell icon extractors (SHGetFileInfo / IExtractIcon) expect STA. MTA
    // causes many extractions to fail with the generic document icon.
    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // Resolve shell: folder settings on this STA (no CoInitializeEx). Also
    // re-run when settings reload restarts the worker.
    ResolvePendingFolderEntries();

    // Capture event handles once; StopScanThread closes them only after join.
    HANDLE stopEvent = g_scanStopEvent;
    HANDLE workEvent = g_scanWorkEvent;

    for (;;) {
        ScanRequest request;
        {
            std::unique_lock<std::mutex> lock(g_scanMutex);
            for (;;) {
                if (g_scanThreadStop.load(std::memory_order_relaxed)) {
                    winrt::uninit_apartment();
                    return;
                }
                if (!g_scanQueue.empty()) {
                    break;
                }

                lock.unlock();
                // Event-driven idle: sleep until stop, work, or an STA message.
                // No timed poll — keeps Explorer out of a permanent 20 Hz wake.
                HANDLE waits[] = {stopEvent, workEvent};
                DWORD w = MsgWaitForMultipleObjects(2, waits, FALSE, INFINITE,
                                                    QS_ALLINPUT);
                if (w == WAIT_OBJECT_0) {
                    winrt::uninit_apartment();
                    return;
                }
                if (w == WAIT_OBJECT_0 + 2) {
                    PumpScanThreadMessages();
                } else if (w == WAIT_FAILED) {
                    Wh_Log(L"Scan thread MsgWaitForMultipleObjects failed (%u)",
                           GetLastError());
                    Sleep(10);
                }
                // WAIT_OBJECT_0+1 (work) or after pump: re-check the queue.
                lock.lock();
            }

            request = std::move(g_scanQueue.front());
            g_scanQueue.erase(g_scanQueue.begin());
        }

        // Settings may have reloaded with new shell: entries while we idled.
        ResolvePendingFolderEntries();

        if (request.iconPixelSize <= 0) {
            request.iconPixelSize = 32;
        }

        std::wstring scanPath = request.path;
        if (IsShellFolderPath(scanPath)) {
            scanPath = ResolveFolderPath(scanPath);
        }
        if (scanPath.empty()) {
            Wh_Log(L"Skipping scan; unresolved path %s", request.path.c_str());
            continue;
        }

        auto fresh = std::make_shared<FolderData>();
        ScanFolderInto(scanPath, request.iconPixelSize, fresh.get());
        fresh->ready = true;
        fresh->scannedAtTick = GetTickCount64();
        fresh->lastUsedTick = fresh->scannedAtTick;

        if (g_unloading ||
            g_scanThreadStop.load(std::memory_order_relaxed)) {
            break;
        }

        {
            std::lock_guard<std::mutex> lock(g_cacheMutex);
            const std::wstring key = CacheKey(scanPath);
            FolderCache()[key] = fresh;
            EvictFolderCacheIfNeeded(key);
        }

        Wh_Log(L"Scanned %s: %d items", scanPath.c_str(),
               (int)fresh->items.size());
    }

    winrt::uninit_apartment();
}

void RequestScan(const std::wstring& path, int iconPixelSize) {
    if (path.empty() || g_unloading) {
        return;
    }

    // Lazy start (and restart after StopScanThread). Safe from the UI thread:
    // StartScanThread only creates the worker; Stop during uninit sets
    // g_unloading / g_scanThreadStop first so a raced enqueue is dropped.
    StartScanThread();

    std::lock_guard<std::mutex> lock(g_scanMutex);
    if (g_unloading || g_scanThreadStop.load(std::memory_order_relaxed) ||
        g_scanThreadJoining.load(std::memory_order_relaxed)) {
        return;
    }
    for (const auto& queued : g_scanQueue) {
        if (queued.iconPixelSize == iconPixelSize &&
            _wcsicmp(queued.path.c_str(), path.c_str()) == 0) {
            return;
        }
    }
    ScanRequest req;
    req.path = path;
    req.iconPixelSize = iconPixelSize;
    g_scanQueue.push_back(std::move(req));
    if (g_scanWorkEvent) {
        SetEvent(g_scanWorkEvent);
    }
}

std::shared_ptr<FolderData> GetFolderData(const std::wstring& path) {
    if (path.empty()) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    auto found = FolderCache().find(CacheKey(path));
    if (found == FolderCache().end()) {
        return nullptr;
    }
    found->second->lastUsedTick = GetTickCount64();
    return found->second;
}

// Returns the cached contents, queueing a scan when the entry is missing,
// stale, or was rendered at a different icon size. Never blocks.
std::shared_ptr<FolderData> GetFolderDataAndRefresh(const std::wstring& path,
                                                    int iconPixelSize) {
    auto data = GetFolderData(path);
    if (!data || !data->ready || data->scannedIconSize != iconPixelSize ||
        GetTickCount64() - data->scannedAtTick > 5000) {
        RequestScan(path, iconPixelSize);
    }
    return data;
}

void StartScanThread() {
    std::lock_guard<std::mutex> lock(g_scanMutex);
    if (g_unloading ||
        g_scanThreadJoining.load(std::memory_order_relaxed)) {
        return;
    }
    // Already running. Never clear stop while a join is in progress —
    // g_scanThreadJoining blocks spawn; RequestScan also drops work.
    if (g_scanThread) {
        return;
    }

    if (!g_scanStopEvent) {
        g_scanStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_scanStopEvent) {
            Wh_Log(L"CreateEventW for scan stop failed (%u)", GetLastError());
            return;
        }
    } else {
        ResetEvent(g_scanStopEvent);
    }
    if (!g_scanWorkEvent) {
        g_scanWorkEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_scanWorkEvent) {
            Wh_Log(L"CreateEventW for scan work failed (%u)", GetLastError());
            CloseHandle(g_scanStopEvent);
            g_scanStopEvent = nullptr;
            return;
        }
    }

    g_scanThreadStop.store(false, std::memory_order_relaxed);
    g_scanThread.emplace(ScanThreadMain);
}

void StopScanThread() {
    std::optional<std::thread> thread;
    HANDLE stopEvent = nullptr;
    HANDLE workEvent = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_scanMutex);
        g_scanThreadStop.store(true, std::memory_order_relaxed);
        g_scanThreadJoining.store(true, std::memory_order_relaxed);
        g_scanQueue.clear();
        if (g_scanStopEvent) {
            SetEvent(g_scanStopEvent);
        }
        // Move globals out under the lock so RequestScan/StartScanThread cannot
        // touch the thread or SetEvent a handle we are about to close.
        thread.swap(g_scanThread);
        std::swap(stopEvent, g_scanStopEvent);
        std::swap(workEvent, g_scanWorkEvent);
    }
    if (thread && thread->joinable()) {
        // Pump sent messages while waiting: this worker runs an STA apartment,
        // and COM's cross-apartment marshaling reaches STA threads via sent
        // messages, so a plain join() risks deadlocking whatever thread is
        // blocked sending into it. Same shape as StopRetryThread.
        HANDLE handle = (HANDLE)thread->native_handle();
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(1, &handle, FALSE, INFINITE,
                                               QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG msg;
                PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
            } else if (result == WAIT_FAILED) {
                Wh_Log(L"StopScanThread: MsgWaitForMultipleObjects failed (%u)",
                       GetLastError());
                break;
            }
        } while (result == WAIT_OBJECT_0 + 1);
        thread->join();
    }
    if (stopEvent) {
        CloseHandle(stopEvent);
    }
    if (workEvent) {
        CloseHandle(workEvent);
    }
    g_scanThreadJoining.store(false, std::memory_order_relaxed);
}

void ResetFolderData() {
    std::lock_guard<std::mutex> lock(g_cacheMutex);
    FolderCache().clear();
}

////////////////////////////////////////////////////////////////////////////////
// The hover grid window
//
// A XAML Popup would be clipped to the taskbar's XAML island, which is only as
// tall as the taskbar, so the grid is a real top-level layered window instead.
// Per-pixel alpha gives antialiased rounded corners and makes hit testing
// outside the panel fall through automatically.

constexpr PCWSTR kPopupClassName = L"WH_TaskbarFolderHoverTray_Grid";
constexpr PCWSTR kMenuOwnerClassName = L"WH_TaskbarFolderHoverTray_MenuOwner";
constexpr UINT_PTR kTickTimerId = 1;
constexpr UINT_PTR kOpenTimerId = 2;
// Separate from kTickTimerId so the blur's refresh rate isn't tied to (or
// capped by) the hover/close/submenu poll cadence.
constexpr UINT_PTR kBlurTimerId = 3;
constexpr UINT_PTR kItemTooltipTimerId = 4;
constexpr UINT kItemTooltipDelayMs = 200;
constexpr UINT kTickTimerMs = 16;
// 32ms = ~30fps. Fixed rather than matched to the monitor: simpler, and this
// is capture+resample cost paid on a timer, not a true frame rate - no need
// to chase 144/240Hz for a hover popup's backdrop.
constexpr UINT kBlurTimerMs = 32;
// Hard cap on cascade depth so a symlink loop cannot exhaust window handles.
constexpr int kMaxLevels = 16;

// One grid in the cascade. Level 0 hangs off a taskbar button; deeper levels
// hang off the cell in their parent that opened them.
struct PopupLevel {
    HWND hwnd = nullptr;
    int depth = 0;
    std::wstring path;
    // Only filled for the root (taskbar) grid; subfolder menus leave this
    // empty.
    std::wstring title;
    std::vector<GridItem> items;
    std::vector<RECT> cellRects;
    // Parallel to items: whether the label was long enough to lose text to
    // the ellipsis at last paint. Drives ItemTooltip - see there.
    std::vector<bool> labelTruncated;
    // Cell kItemTooltipTimerId is currently counting down for, so the fire
    // can be ignored if the hover has since moved to a different cell.
    int tooltipPendingCell = -1;
    RECT rect{};
    RECT anchorRect{};
    int spawnerCell = -1;
    int hoverCell = -1;
    int pressedCell = -1;
    bool loading = false;
    // Static content (panel, icons, labels) without hover/press chrome. Hover
    // changes blit this and overlay the highlight instead of repainting all cells.
    std::unique_ptr<Gdiplus::Bitmap> cachedBase;
    int cachedBaseW = 0;
    int cachedBaseH = 0;
    bool baseDirty = true;
    // Screen capture behind the panel's rect, softened by CaptureBlurredBackdrop.
    // Baked into cachedBase by RebuildLevelBase, not drawn separately - see
    // PresentLevel for when this is (re)captured.
    std::unique_ptr<Gdiplus::Bitmap> blurBackdrop;
    // Size and radius the rounded window region was last built for, so hover
    // repaints do not rebuild it. See ApplyRoundedRegion. The radius is part of
    // the key because a settings change can alter it without the window
    // resizing, which used to leave the old shape in place until the popup
    // happened to open at a different size.
    int regionW = 0;
    int regionH = 0;
    int regionRadius = -1;
};

// Created on the taskbar UI thread, read from the Windhawk engine thread, an
// Explorer browser thread and the folder manager thread when they ask for a
// reload — see RequestReloadUI.
std::atomic<HWND> g_menuOwnerWnd{nullptr};
HWND g_taskbarWnd = nullptr;

// Windows are created lazily, one per depth, and reused across cascades.
std::vector<HWND> g_levelWindows;
[[clang::no_destroy]] std::optional<std::vector<std::unique_ptr<PopupLevel>>>
    g_levels{std::in_place};

std::vector<std::unique_ptr<PopupLevel>>& Levels() {
    if (!g_levels) {
        g_levels.emplace();
    }
    return *g_levels;
}

// Leaving WDA_EXCLUDEFROMCAPTURE set permanently kept the grid out of every
// screenshot tool, not just our own BitBlt in CaptureBlurredBackdrop - the
// whole tray would vanish from Snipping Tool/PrtScn/OBS any time it was open.
// Toggled on for the few milliseconds our own capture runs, then back off
// immediately after, so only that instant is blind to it.
void SetOwnWindowsCaptureExcluded(bool excluded) {
    DWORD affinity = excluded ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE;
    for (HWND hWnd : g_levelWindows) {
        if (hWnd) {
            SetWindowDisplayAffinity(hWnd, affinity);
        }
    }
}

RECT g_rootAnchorRect{};
int g_popupDpi = 96;
std::atomic<bool> g_menuActive{false};
// True while a shell verb is queued (PostMessage) or running via InvokeCommand.
std::atomic<bool> g_invokeActive{false};
ULONGLONG g_outsideSinceTick = 0;

std::wstring g_pendingRootPath;
std::wstring g_pendingRootTitle;
RECT g_pendingRootRect{};

int g_pendingSubDepth = -1;
int g_pendingSubCell = -1;
ULONGLONG g_pendingSubSinceTick = 0;
ULONGLONG g_closeDeeperSinceTick = 0;

IContextMenu2* g_activeContextMenu2 = nullptr;
IContextMenu3* g_activeContextMenu3 = nullptr;

// Deferred shell verb: stash after TrackPopupMenuEx so PopupWndProc /
// ShowItemContextMenu can return before InvokeCommand (and any modal UI).
struct PendingShellCommand {
    IContextMenu* contextMenu = nullptr;
    int commandOffset = -1;
    POINT invokePoint{};
};
PendingShellCommand g_pendingShellCommand;

// Posted to g_menuOwnerWnd after the context menu loop unwinds.
constexpr UINT WM_APP_INVOKE_SHELL_VERB = WM_APP + 41;

// Posted to g_menuOwnerWnd so a reload asked for by a XAML flyout item runs
// after that item's own Click handler has returned. ReloadAndRefreshUI tears
// down the very button the flyout is anchored to and unhooks the handlers it
// is running inside, so it must not run inline from there.
constexpr UINT WM_APP_RELOAD_UI = WM_APP + 42;

// The only way anything should ask for a reload.
//
// Four threads want one — the Windhawk engine on a settings change, the
// taskbar UI thread on unpin, an Explorer browser thread on a right-click
// pin, the manager thread on any edit — and ReloadAndRefreshUI is not safe to
// run concurrently or inline on most of them: it joins the scan and retry
// workers (so a slow shell icon handler would freeze the Explorer window the
// user just right-clicked in) and it rebuilds g_settings, which two threads
// doing it at once is a data race on the string members.
//
// Posting to the one window owned by the taskbar UI thread serializes every
// request through that thread's message loop for free. A mutex could not:
// ReloadAndRefreshUI SendMessages to the taskbar thread, so if that thread
// were the one blocked in lock(), it would never dispatch the send and both
// threads would hang.
void RequestReloadUI() {
    HWND owner = g_menuOwnerWnd.load();
    if (!owner || !PostMessageW(owner, WM_APP_RELOAD_UI, 0, 0)) {
        // Before the taskbar UI is up (a settings change during init) there is
        // no message loop to defer to, and no other thread to race with.
        Wh_Log(L"RequestReloadUI: no owner window; reloading inline");
        ReloadAndRefreshUI();
    }
}

// Both take their arguments by value: reopening a level destroys the PopupLevel
// the caller may have read them from.
void OpenRootLevel(std::wstring path, RECT anchorRect, std::wstring title);
void OpenSubLevel(int parentDepth, int cell);
void CloseLevelsFrom(int depth);
void CloseChain();
void HideItemTooltip();
void DestroyItemTooltip();
void StartRetryThread();
bool EnsurePopupClasses();
HWND EnsureMenuOwnerWindow();

int ScaleForPopup(int value) {
    return MulDiv(value, g_popupDpi, 96);
}

// Physical icon size in the grid. With labels hidden the icon grows to fill the
// cell instead of leaving the label's share of it empty. Icons are extracted at
// this size too, not upscaled from a cached bitmap made at the configured one.
int PopupIconPixelSize() {
    int size = ScaleForPopup(g_settings.iconSize);
    if (!g_settings.showLabels) {
        int fit = std::min<int>(ScaleForPopup(g_settings.cellWidth),
                                ScaleForPopup(g_settings.cellHeight)) -
                  ScaleForPopup(8) * 2;
        size = std::max<int>(size, fit);
    }
    return size;
}

// Gaussian blur is rendered ourselves (see CaptureBlurredBackdrop): capturing
// and blurring the screen by hand costs a BitBlt + resample every
// kBlurTimerMs, but gives a real, adjustable, untinted blur radius. Acrylic
// instead hands the backdrop to DWM's own SetWindowCompositionAttribute
// accent below - much cheaper (no capture, no resample, updates live for
// free) but the blur radius itself is fixed by Windows; only the tint's
// opacity is ours to adjust, via GradientColor's alpha.
void ApplyBackdrop(HWND hWnd) {
    // Capture exclusion is applied transiently around our own BitBlt instead
    // of here - see SetOwnWindowsCaptureExcluded - so the window stays
    // screenshot/recording-visible the rest of the time.

    int corner = g_settings.cornerRadius > 0 ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                          sizeof(corner));

    // DWM draws its own 1px border on a rounded window, independent of anything
    // we paint. That is the outline that survived at opacity 0 even with our
    // own border pen gone, so it is suppressed here too.
    COLORREF border = DWMWA_COLOR_NONE;
    DwmSetWindowAttribute(hWnd, DWMWA_BORDER_COLOR, &border, sizeof(border));

    if (auto setWindowCompositionAttribute = GetSetWindowCompositionAttribute()) {
        ACCENT_POLICY accent{};
        if (g_settings.blurType == BlurType::Acrylic &&
            g_settings.blurStrength > 0) {
            accent.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
            // Grayscale tint (R=G=B), so byte order within GradientColor
            // doesn't matter. Strength is the tint's opacity, not the blur
            // radius - Windows fixes that itself.
            bool dark = IsDarkPopupTheme();
            DWORD rgb = dark ? 0x2B2B2Bu : 0xF9F9F9u;
            BYTE alpha =
                (BYTE)(std::clamp(g_settings.blurStrength, 1, 100) * 255 / 100);
            accent.GradientColor = ((DWORD)alpha << 24) | rgb;
        } else {
            accent.AccentState = ACCENT_DISABLED;
        }
        WINDOWCOMPOSITIONATTRIBDATA data{WCA_ACCENT_POLICY, &accent,
                                         sizeof(accent)};
        setWindowCompositionAttribute(hWnd, &data);
    }
}

void AddRoundedRect(Gdiplus::GraphicsPath* path,
                    const Gdiplus::Rect& rect,
                    int radius) {
    if (radius <= 0) {
        path->AddRectangle(rect);
        return;
    }
    int d = radius * 2;
    d = std::min<int>(d, std::min<int>(rect.Width, rect.Height));
    path->AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path->AddArc(rect.GetRight() - d, rect.Y, d, d, 270.0f, 90.0f);
    path->AddArc(rect.GetRight() - d, rect.GetBottom() - d, d, d, 0.0f, 90.0f);
    path->AddArc(rect.X, rect.GetBottom() - d, d, d, 90.0f, 90.0f);
    path->CloseFigure();
}

// Signed distance from (px, py) to the boundary of an axis-aligned rounded
// rect of half-extents (hw, hh) and corner radius r, both centered on the
// origin. <=0 inside/on the shape, growing positive outward - the standard
// rounded-box SDF. Gives the panel's own Gaussian blur a smoothly
// antialiased edge (PaintLevel), rather than GDI+'s aliased region-clip.
float RoundedBoxSDF(float px, float py, float hw, float hh, float r) {
    float qx = std::abs(px) - (hw - r);
    float qy = std::abs(py) - (hh - r);
    float ax = std::max(qx, 0.0f);
    float ay = std::max(qy, 0.0f);
    return std::sqrt(ax * ax + ay * ay) + std::min(std::max(qx, qy), 0.0f) - r;
}

// A minimal folder silhouette: a body whose top edge steps up on the left to
// form the tab. Kept as straight edges so it stays crisp at badge sizes.
void AddFolderGlyphPath(Gdiplus::GraphicsPath* path,
                        const Gdiplus::RectF& box) {
    Gdiplus::REAL step = box.Height * 0.24f;
    Gdiplus::PointF pts[6] = {
        {box.X, box.Y},
        {box.X + box.Width * 0.42f, box.Y},
        {box.X + box.Width * 0.54f, box.Y + step},
        {box.X + box.Width, box.Y + step},
        {box.X + box.Width, box.Y + box.Height},
        {box.X, box.Y + box.Height},
    };
    path->AddPolygon(pts, 6);
}

// Extra top band reserved for the root folder title. Subfolder menus skip this.
int RootTitleBandHeight(const PopupLevel* level) {
    if (!level || !g_settings.showTitle || level->depth != 0 ||
        level->title.empty()) {
        return 0;
    }
    return ScaleForPopup(8) + ScaleForPopup(g_settings.titleFontSize) +
           ScaleForPopup(10);
}

// Fills level->cellRects and returns the required window size in physical
// pixels. When maxHeight > 0, auto columns are derived from how many rows fit
// in that height so large folders go wide instead of tall-and-narrow. When
// maxWidth > 0, columns are capped so the grid stays on the monitor; items
// beyond cols*rows are trimmed from level->items.
SIZE ComputeLevelLayout(PopupLevel* level,
                        int maxHeight = 0,
                        int maxWidth = 0) {
    int itemCount = (int)level->items.size();
    int padding = ScaleForPopup(8);
    int cellW = ScaleForPopup(g_settings.cellWidth);
    int cellH = ScaleForPopup(g_settings.cellHeight);
    int titleBand = RootTitleBandHeight(level);

    int count = std::max<int>(itemCount, 1);
    int cols;
    if (g_settings.columns > 0) {
        cols = std::min<int>(g_settings.columns, count);
    } else if (maxHeight > 0 && cellH > 0) {
        int usable = std::max(0, maxHeight - padding * 2 - titleBand);
        int maxRows = std::max<int>(1, usable / cellH);
        cols = (count + maxRows - 1) / maxRows;
        cols = std::clamp<int>(cols, 1, count);
        // Prefer a squarer grid when it still fits the available height.
        int squareCols = (int)std::ceil(std::sqrt((double)count));
        squareCols = std::clamp<int>(squareCols, cols, count);
        int squareRows = (count + squareCols - 1) / squareCols;
        if (squareRows <= maxRows) {
            cols = squareCols;
        }
    } else {
        cols = (int)std::ceil(std::sqrt((double)count));
        cols = std::clamp<int>(cols, 1, 6);
        cols = std::min<int>(cols, count);
    }

    if (maxWidth > 0 && cellW > 0) {
        int maxCols = std::max(1, (maxWidth - padding * 2) / cellW);
        cols = std::min(cols, maxCols);
    }

    int rows = (count + cols - 1) / cols;

    if (maxHeight > 0 && cellH > 0) {
        int usable = maxHeight - padding * 2 - titleBand;
        int maxRows = std::max<int>(1, usable / cellH);
        while (rows > maxRows && cols < count) {
            cols++;
            rows = (count + cols - 1) / cols;
        }
        // Height growth can widen past the monitor — re-apply the width cap.
        if (maxWidth > 0 && cellW > 0) {
            int maxCols = std::max(1, (maxWidth - padding * 2) / cellW);
            if (cols > maxCols) {
                cols = maxCols;
                rows = (count + cols - 1) / cols;
            }
        }
        if (rows > maxRows) {
            rows = maxRows;
        }
    }

    // Drop items that would fall outside the visible cols*rows grid.
    int capacity = cols * std::max(rows, 1);
    if (itemCount > capacity) {
        level->items.resize(capacity);
        itemCount = capacity;
        count = std::max<int>(itemCount, 1);
        rows = itemCount > 0 ? (itemCount + cols - 1) / cols : 1;
    }

    level->cellRects.clear();
    level->cellRects.reserve(itemCount);
    for (int i = 0; i < itemCount; i++) {
        int r = i / cols;
        int c = i % cols;
        RECT cell;
        cell.left = padding + c * cellW;
        cell.top = padding + titleBand + r * cellH;
        cell.right = cell.left + cellW;
        cell.bottom = cell.top + cellH;
        level->cellRects.push_back(cell);
    }

    SIZE size;
    size.cx = padding * 2 + cols * cellW;
    size.cy = padding * 2 + titleBand + rows * cellH;
    if (itemCount == 0) {
        // Leave room for the "Loading" / "Empty folder" message.
        size.cx = padding * 2 + cellW * 2;
        size.cy = std::max<int>(size.cy, padding * 2 + titleBand + cellH);
    }
    if (maxHeight > 0) {
        size.cy = std::min<int>(size.cy, maxHeight);
    }
    if (maxWidth > 0) {
        size.cx = std::min<int>(size.cx, maxWidth);
    }
    return size;
}

bool CanExpand(int depth) {
    if (g_settings.maxFolderDepth < 0) {
        return depth + 1 < kMaxLevels;
    }
    return depth < g_settings.maxFolderDepth && depth + 1 < kMaxLevels;
}

// Clips the window itself to the same rounded rect the panel is painted with.
// The blur background and DWM's border are drawn against the window shape, not
// against our alpha, so without this they kept square corners while the painted
// background was round. Rebuilt only when the size or the radius changes - hover
// repaints run through PaintLevel too.
//
// Two calls, because the window is layered and the acrylic is not ours:
//
//  * SetWindowRgn shapes the window, which is what clips our own painting, the
//    hit testing and DWM's border.
//  * DwmEnableBlurBehindWindow's hRgnBlur shapes the *blur*. DWM composites the
//    acrylic as its own layer behind the layered surface, filling the window
//    rect, and our per-pixel alpha is what lets it show through the corners we
//    left transparent — so the window region alone never reached it. This is the
//    documented way to say which part of a window the blur covers.
void ApplyRoundedRegion(PopupLevel* level, int width, int height) {
    if (!level || !level->hwnd || width <= 0 || height <= 0) {
        return;
    }
    int radius = ScaleForPopup(g_settings.cornerRadius);
    if (level->regionW == width && level->regionH == height &&
        level->regionRadius == radius) {
        return;
    }

    // CreateRoundRectRgn's ellipse size is the full diameter, and its right and
    // bottom edges are exclusive.
    auto makeRegion = [&]() -> HRGN {
        return radius > 0 ? CreateRoundRectRgn(0, 0, width + 1, height + 1,
                                               radius * 2, radius * 2)
                          : CreateRectRgn(0, 0, width, height);
    };

    HRGN region = makeRegion();
    if (!region) {
        return;
    }
    // On success the window owns the region; do not delete it here.
    if (SetWindowRgn(level->hwnd, region, FALSE)) {
        level->regionW = width;
        level->regionH = height;
        level->regionRadius = radius;
    } else {
        DeleteObject(region);
    }

    // A second, independent region: DwmEnableBlurBehindWindow copies it rather
    // than taking ownership, so this one is ours to delete.
    if (HRGN blurRegion = makeRegion()) {
        DWM_BLURBEHIND blur{};
        blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        blur.fEnable = TRUE;
        blur.hRgnBlur = blurRegion;
        DwmEnableBlurBehindWindow(level->hwnd, &blur);
        DeleteObject(blurRegion);
    }
}

void InvalidateLevelBase(PopupLevel* level) {
    if (!level) {
        return;
    }
    level->baseDirty = true;
    level->cachedBase.reset();
    level->cachedBaseW = 0;
    level->cachedBaseH = 0;
}

// Floor for the panel fill's alpha. UpdateLayeredWindow hit-tests against the
// alpha channel: a 0-alpha pixel is click-through straight to whatever is
// behind the grid, so without a floor the gaps between icons stop taking
// clicks/WM_MOUSEMOVE. 1/255 is the standard "invisible but still hit-tests"
// value for layered windows - premultiplied, it rounds a near-black or
// near-white fill down to a 0 or 1 RGB component, which is not distinguishable
// from true transparency on screen.
constexpr int kHitTestAlpha = 1;

BYTE PanelAlpha() {
    return (BYTE)std::clamp<int>(g_settings.panelOpacity * 255 / 100,
                                 kHitTestAlpha, 255);
}


// Grid text is white in both themes, so the outline is always black. Eight
// offsets at one scaled pixel: corners covered, edge crisp.
//
// On a dark panel the outline only has to work once the desktop starts showing
// through, so it fades in as the panel fades out. On a light panel white text
// needs the outline at every opacity or it disappears into the panel, so there
// it stays at full strength.
void DrawStringWithShadow(Gdiplus::Graphics& g,
                          PCWSTR text,
                          Gdiplus::Font& font,
                          const Gdiplus::RectF& rect,
                          Gdiplus::StringFormat& format,
                          const Gdiplus::Color& textColor,
                          BYTE panelAlpha) {
    int outline = IsDarkPopupTheme() ? 255 - panelAlpha : 255;
    if (outline > 0) {
        Gdiplus::SolidBrush outlineBrush(Gdiplus::Color((BYTE)outline, 0, 0, 0));
        Gdiplus::REAL d = (Gdiplus::REAL)std::max<int>(1, ScaleForPopup(1));
        const Gdiplus::REAL offsets[8][2] = {{-d, -d}, {0, -d}, {d, -d}, {-d, 0},
                                             {d, 0},   {-d, d}, {0, d},  {d, d}};
        for (const auto& off : offsets) {
            Gdiplus::RectF outlineRect = rect;
            outlineRect.Offset(off[0], off[1]);
            g.DrawString(text, -1, &font, outlineRect, &format, &outlineBrush);
        }
    }
    Gdiplus::SolidBrush textBrush(textColor);
    g.DrawString(text, -1, &font, rect, &format, &textBrush);
}

// Icon, folder badge, and label for one grid cell. Shared by the static base
// paint and the hover/press cell overlay so the two stay visually identical.
void DrawCell(Gdiplus::Graphics& g,
              PopupLevel* level,
              int index,
              BYTE panelAlpha,
              const Gdiplus::Color& textColor,
              const Gdiplus::Color& badgeFillColor,
              const Gdiplus::Color& badgeMarkColor,
              const Gdiplus::Color& badgeRingColor,
              Gdiplus::Font& font,
              Gdiplus::StringFormat& format) {
    if (!level || index < 0 || index >= (int)level->cellRects.size() ||
        index >= (int)level->items.size()) {
        return;
    }

    const RECT& cell = level->cellRects[index];
    int cellW = cell.right - cell.left;
    int cellH = cell.bottom - cell.top;
    const GridItem& item = level->items[index];

    int iconSize = PopupIconPixelSize();
    int labelGap = ScaleForPopup(6);
    // With labels hidden the icon owns the whole cell, so centre it instead of
    // leaving it parked at the top over empty space.
    int iconTop = g_settings.showLabels ? ScaleForPopup(10)
                                        : (cellH - iconSize) / 2;
    bool expandable = CanExpand(level->depth);

    Gdiplus::Rect iconRect(cell.left + (cellW - iconSize) / 2,
                           cell.top + iconTop, iconSize, iconSize);
    if (item.icon) {
        g.DrawImage(item.icon.get(), iconRect);
    }

    if (item.isFolder && expandable) {
        int badge = std::max<int>(14, (iconSize * 54 + 50) / 100);
        int badgeLeft = iconRect.GetRight() - badge + badge / 4;
        int badgeTop = iconRect.GetBottom() - badge + badge / 4;
        Gdiplus::Rect badgeRect(badgeLeft, badgeTop, badge, badge);

        Gdiplus::GraphicsPath badgePath;
        AddRoundedRect(&badgePath, badgeRect, badge / 2);
        Gdiplus::SolidBrush badgeBrush(badgeFillColor);
        g.FillPath(&badgeBrush, &badgePath);
        Gdiplus::Pen badgeRing(badgeRingColor, 1.0f);
        g.DrawPath(&badgeRing, &badgePath);

        Gdiplus::REAL glyphW = badge * 0.58f;
        Gdiplus::REAL glyphH = glyphW * 0.80f;
        Gdiplus::RectF glyphBox(
            badgeLeft + (badge - glyphW) / 2.0f,
            badgeTop + (badge - glyphH) / 2.0f + badge * 0.02f, glyphW, glyphH);
        Gdiplus::GraphicsPath glyphPath;
        AddFolderGlyphPath(&glyphPath, glyphBox);
        Gdiplus::SolidBrush markBrush(badgeMarkColor);
        g.FillPath(&markBrush, &glyphPath);
    }

    Gdiplus::RectF labelRect(
        (Gdiplus::REAL)(cell.left + ScaleForPopup(4)),
        (Gdiplus::REAL)(cell.top + iconTop + iconSize + labelGap),
        (Gdiplus::REAL)(cellW - ScaleForPopup(8)),
        (Gdiplus::REAL)(cellH - iconTop - iconSize - labelGap -
                        ScaleForPopup(4)));
    if (g_settings.showLabels && labelRect.Height > 0) {
        DrawStringWithShadow(g, item.displayName.c_str(), font, labelRect,
                             format, textColor, panelAlpha);

        Gdiplus::RectF measuredBox;
        INT codepointsFitted = 0;
        INT linesFilled = 0;
        g.MeasureString(item.displayName.c_str(), -1, &font, labelRect,
                        &format, &measuredBox, &codepointsFitted,
                        &linesFilled);
        if (index < (int)level->labelTruncated.size()) {
            level->labelTruncated[index] =
                codepointsFitted < (INT)item.displayName.size();
        }
    }
}

// Captures the desktop behind screenRect and softens it with a cheap,
// tint-free blur: shrink heavily (the resampling itself is what blurs it),
// then stretch back up. Higher strength shrinks further, so there is more
// for the upscale to smooth over - a real, adjustable blur radius, unlike
// DWM's fixed-radius accent blur. Safe to call with the level's own window
// visible and covering screenRect: capture exclusion is toggled on for just
// the BitBlt below (see SetOwnWindowsCaptureExcluded), so that window's
// pixels don't show up in its own blur without staying invisible to
// screenshot tools the rest of the time.
std::unique_ptr<Gdiplus::Bitmap> CaptureBlurredBackdrop(const RECT& screenRect,
                                                        int strength) {
    int width = screenRect.right - screenRect.left;
    int height = screenRect.bottom - screenRect.top;
    if (width <= 0 || height <= 0 || strength <= 0) {
        return nullptr;
    }

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) {
        return nullptr;
    }
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBmp =
        hMemDC ? CreateCompatibleBitmap(hScreenDC, width, height) : nullptr;

    std::unique_ptr<Gdiplus::Bitmap> captured;
    if (hMemDC && hBmp) {
        HGDIOBJ old = SelectObject(hMemDC, hBmp);
        SetOwnWindowsCaptureExcluded(true);
        BitBlt(hMemDC, 0, 0, width, height, hScreenDC, screenRect.left,
              screenRect.top, SRCCOPY);
        SetOwnWindowsCaptureExcluded(false);
        SelectObject(hMemDC, old);
        captured.reset(Gdiplus::Bitmap::FromHBITMAP(hBmp, nullptr));
    }
    if (hBmp) {
        DeleteObject(hBmp);
    }
    if (hMemDC) {
        DeleteDC(hMemDC);
    }
    ReleaseDC(nullptr, hScreenDC);

    if (!captured || captured->GetLastStatus() != Gdiplus::Ok) {
        return nullptr;
    }

    float shrink = 1.0f + (float)std::clamp(strength, 0, 100) / 100.0f * 14.0f;
    int smallW = std::max(1, (int)(width / shrink));
    int smallH = std::max(1, (int)(height / shrink));

    Gdiplus::Bitmap small(smallW, smallH, PixelFormat32bppRGB);
    if (small.GetLastStatus() != Gdiplus::Ok) {
        return nullptr;
    }
    {
        Gdiplus::Graphics g(&small);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBilinear);
        g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        g.DrawImage(captured.get(), 0, 0, smallW, smallH);
    }

    auto result = std::make_unique<Gdiplus::Bitmap>(width, height,
                                                     PixelFormat32bppRGB);
    if (!result || result->GetLastStatus() != Gdiplus::Ok) {
        return nullptr;
    }
    Gdiplus::Graphics g(result.get());
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.DrawImage(&small, 0, 0, width, height);
    return result;
}

// Paints the static panel (no hover/press chrome) into level->cachedBase.
bool RebuildLevelBase(PopupLevel* level, int width, int height) {
    auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height,
                                                    PixelFormat32bppPARGB);
    if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok) {
        return false;
    }
    level->labelTruncated.assign(level->items.size(), false);

    Gdiplus::Graphics g(bitmap.get());
    g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    // GridFit, not plain AntiAlias: without grid fitting GDI+ places stems on
    // fractional pixels, which is what made labels look thin and grainy. Only
    // the GridFit variants hint the outline onto the pixel grid. ClearType is
    // not an option here - subpixel AA has no defined result on the
    // transparent PARGB surface a layered window needs.
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    g.Clear(Gdiplus::Color(0, 0, 0, 0));

    bool dark = IsDarkPopupTheme();
    // 0 = gamma 1.0. The GDI+ default of 4 eats the edge coverage of light
    // glyphs, and grid text is always light. Tuning knob if a font renders off.
    g.SetTextContrast(0);
    BYTE panelAlpha = PanelAlpha();
    Gdiplus::Color panelColor = dark
                                    ? Gdiplus::Color(panelAlpha, 43, 43, 43)
                                    : Gdiplus::Color(panelAlpha, 249, 249, 249);
    // White in both themes - the black outline carries the contrast, so the
    // light theme does not need dark text.
    Gdiplus::Color textColor(255, 255, 255, 255);
    Gdiplus::Color badgeFillColor = GetAccentGdipColor(255);
    bool lightAccent = ((int)badgeFillColor.GetR() + badgeFillColor.GetG() +
                        badgeFillColor.GetB()) > 500;
    Gdiplus::Color badgeMarkColor = lightAccent
                                        ? Gdiplus::Color(255, 20, 20, 20)
                                        : Gdiplus::Color(255, 255, 255, 255);
    Gdiplus::Color badgeRingColor =
        dark ? Gdiplus::Color(160, 0, 0, 0) : Gdiplus::Color(70, 255, 255, 255);

    int radius = ScaleForPopup(g_settings.cornerRadius);
    Gdiplus::Rect panelRect(0, 0, width - 1, height - 1);
    Gdiplus::GraphicsPath panelPath;
    AddRoundedRect(&panelPath, panelRect, radius);

    // Blur is not baked in here - see PaintLevel. It is re-captured far more
    // often than icons/text/fill change, so it is composited straight into
    // the final DIB every repaint instead of forcing a full RebuildLevelBase
    // (icon/badge/text redraws) on every blur tick.
    //
    // The fill still runs at opacity 0 - see PanelAlpha() - so nothing of the
    // panel need be left on screen.
    //
    // SourceCopy, not the default SourceOver: this Graphics runs at
    // CompositingQualityHighQuality, which is GDI+'s gamma-corrected blend, and
    // gamma-correcting an alpha of kHitTestAlpha against the transparent
    // surface rounds it straight back down to 0 - which is exactly the
    // click-through hole PanelAlpha()'s floor exists to prevent. The fill is
    // the first thing drawn onto a just-cleared bitmap, so there is nothing
    // underneath for SourceOver to preserve anyway; copying writes the alpha
    // byte verbatim. Antialiasing still applies, so the rounded corners keep
    // their partial-alpha edge.
    Gdiplus::SolidBrush panelBrush(panelColor);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
    g.FillPath(&panelBrush, &panelPath);
    g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);

    auto font = MakePopupFont(ScaleForPopup(g_settings.fontSize),
                              g_settings.itemFontWeight);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentNear);
    format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
    format.SetFormatFlags(Gdiplus::StringFormatFlagsLineLimit);

    int titleBand = RootTitleBandHeight(level);
    if (titleBand > 0) {
        auto titleFont = MakePopupFont(ScaleForPopup(g_settings.titleFontSize),
                                       g_settings.titleFontWeight);
        Gdiplus::StringFormat titleFormat;
        if (g_settings.titleAlign == L"left") {
            titleFormat.SetAlignment(Gdiplus::StringAlignmentNear);
        } else if (g_settings.titleAlign == L"right") {
            titleFormat.SetAlignment(Gdiplus::StringAlignmentFar);
        } else {
            titleFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
        }
        titleFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        titleFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        titleFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

        int pad = ScaleForPopup(12);
        Gdiplus::RectF titleRect(
            (Gdiplus::REAL)pad, (Gdiplus::REAL)ScaleForPopup(4),
            (Gdiplus::REAL)(width - pad * 2),
            (Gdiplus::REAL)(titleBand - ScaleForPopup(4)));
        DrawStringWithShadow(g, level->title.c_str(), *titleFont, titleRect,
                             titleFormat, textColor, panelAlpha);
    }

    if (level->items.empty()) {
        Gdiplus::StringFormat centered;
        centered.SetAlignment(Gdiplus::StringAlignmentCenter);
        centered.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        PCWSTR message = level->loading ? L"Loading..." : L"Empty folder";
        Gdiplus::RectF area(0.0f, (Gdiplus::REAL)titleBand, (Gdiplus::REAL)width,
                            (Gdiplus::REAL)(height - titleBand));
        DrawStringWithShadow(g, message, *font, area, centered, textColor,
                             panelAlpha);
    }

    for (size_t i = 0; i < level->items.size() && i < level->cellRects.size();
         i++) {
        DrawCell(g, level, (int)i, panelAlpha, textColor, badgeFillColor,
                 badgeMarkColor, badgeRingColor, *font, format);
    }

    level->cachedBase = std::move(bitmap);
    level->cachedBaseW = width;
    level->cachedBaseH = height;
    level->baseDirty = false;
    return true;
}

void PaintLevel(PopupLevel* level) {
    if (!level || !level->hwnd) {
        return;
    }

    int width = level->rect.right - level->rect.left;
    int height = level->rect.bottom - level->rect.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    if (level->baseDirty || !level->cachedBase || level->cachedBaseW != width ||
        level->cachedBaseH != height) {
        if (!RebuildLevelBase(level, width, height)) {
            return;
        }
    }

    HDC hScreenDC = GetDC(nullptr);
    if (!hScreenDC) {
        return;
    }
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    if (!hMemDC) {
        ReleaseDC(nullptr, hScreenDC);
        return;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hBmp =
        CreateDIBSection(hMemDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBmp || !bits) {
        if (hBmp) {
            DeleteObject(hBmp);
        }
        DeleteDC(hMemDC);
        ReleaseDC(nullptr, hScreenDC);
        return;
    }
    HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);

    {
        Gdiplus::Bitmap surface(width, height, width * 4, PixelFormat32bppPARGB,
                                (BYTE*)bits);
        Gdiplus::Graphics g(&surface);
        g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
        g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        // Blur goes down first, edged to the same rounded shape cachedBase's
        // own fill/border/icons are clipped to (see RebuildLevelBase), so it
        // never shows square corners under the panel. Composited fresh every
        // repaint - re-captured far more often than icons/text change - while
        // cachedBase underneath is cheap to reuse as-is.
        //
        // Written by hand via RoundedBoxSDF, not a GDI+ region clip: region
        // clipping is always hard-edged in GDI+ regardless of SmoothingMode.
        // Small (~1px) antialiasing band, just enough to not be a hard cut,
        // matching the fill/border's own AntiAlias smoothing on top.
        if (level->blurBackdrop &&
            level->blurBackdrop->GetWidth() == (UINT)width &&
            level->blurBackdrop->GetHeight() == (UINT)height) {
            Gdiplus::Rect full(0, 0, width, height);
            Gdiplus::BitmapData srcData;
            if (level->blurBackdrop->LockBits(&full, Gdiplus::ImageLockModeRead,
                                              PixelFormat32bppRGB,
                                              &srcData) == Gdiplus::Ok) {
                float radius = (float)ScaleForPopup(g_settings.cornerRadius);
                float hw = (width - 1) / 2.0f;
                float hh = (height - 1) / 2.0f;
                constexpr float kEdgeAA = 1.0f;
                for (int y = 0; y < height; y++) {
                    auto* src =
                        (uint32_t*)((BYTE*)srcData.Scan0 + y * srcData.Stride);
                    auto* dst = (uint32_t*)((BYTE*)bits + y * width * 4);
                    for (int x = 0; x < width; x++) {
                        float d = RoundedBoxSDF((float)x - hw, (float)y - hh,
                                                hw, hh, radius);
                        float alpha =
                            255.0f * (1.0f - std::clamp((d + kEdgeAA * 0.5f) /
                                                        kEdgeAA,
                                                    0.0f, 1.0f));
                        if (alpha <= 0.0f) {
                            continue;
                        }
                        uint32_t px = src[x];
                        BYTE a = (BYTE)(alpha + 0.5f);
                        BYTE b = (BYTE)(px & 0xFF);
                        BYTE g8 = (BYTE)((px >> 8) & 0xFF);
                        BYTE r = (BYTE)((px >> 16) & 0xFF);
                        dst[x] = ((uint32_t)a << 24) | ((r * a / 255) << 16) |
                                ((g8 * a / 255) << 8) | (b * a / 255);
                    }
                }
                level->blurBackdrop->UnlockBits(&srcData);
            }
        }

        // Point overload, not the dest-rect one: with the default
        // PixelOffsetModeNone a dest-rect DrawImage lands the 1:1 blit on a
        // half-pixel offset and resamples the whole base, smearing every glyph.
        // SourceOver, not SourceCopy: cachedBase's own alpha (panel fill,
        // transparent corners) has to blend with the blur just drawn, not
        // stomp it.
        g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        g.DrawImage(level->cachedBase.get(), 0, 0);

        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
        // Must match RebuildLevelBase - a hovered cell is repainted here and
        // would otherwise not look like the same text as its neighbours.
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

        bool dark = IsDarkPopupTheme();
        // Must match RebuildLevelBase, or a hovered cell's text would not look
        // like its neighbours'.
        g.SetTextContrast(0);
        Gdiplus::Color hoverColor =
            dark ? Gdiplus::Color(28, 255, 255, 255) : Gdiplus::Color(18, 0, 0, 0);
        Gdiplus::Color pressColor =
            dark ? Gdiplus::Color(46, 255, 255, 255) : Gdiplus::Color(32, 0, 0, 0);
        Gdiplus::Color textColor(255, 255, 255, 255);
        Gdiplus::Color badgeFillColor = GetAccentGdipColor(255);
        bool lightAccent = ((int)badgeFillColor.GetR() + badgeFillColor.GetG() +
                            badgeFillColor.GetB()) > 500;
        Gdiplus::Color badgeMarkColor = lightAccent
                                            ? Gdiplus::Color(255, 20, 20, 20)
                                            : Gdiplus::Color(255, 255, 255, 255);
        Gdiplus::Color badgeRingColor = dark ? Gdiplus::Color(160, 0, 0, 0)
                                             : Gdiplus::Color(70, 255, 255, 255);

        auto font = MakePopupFont(ScaleForPopup(g_settings.fontSize),
                                  g_settings.itemFontWeight);
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentNear);
        format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
        format.SetFormatFlags(Gdiplus::StringFormatFlagsLineLimit);

        // Highlight under icon/label: clear only inside the rounded path, then
        // fill highlight and DrawCell. A full-cell SourceCopy FillRectangle
        // (added with cachedBase in v1.5) left a sharp translucent fringe in the
        // ScaleForPopup(2) inset margin that did not match the base panel alpha.
        auto repaintCell = [&](int cellIndex, bool pressed) {
            if (cellIndex < 0 || cellIndex >= (int)level->cellRects.size() ||
                cellIndex >= (int)level->items.size()) {
                return;
            }
            const RECT& cell = level->cellRects[cellIndex];
            int cellW = cell.right - cell.left;
            int cellH = cell.bottom - cell.top;

            Gdiplus::Rect highlight(
                cell.left + ScaleForPopup(2), cell.top + ScaleForPopup(2),
                cellW - ScaleForPopup(4), cellH - ScaleForPopup(4));
            Gdiplus::GraphicsPath highlightPath;
            AddRoundedRect(&highlightPath, highlight, ScaleForPopup(6));

            BYTE panelAlpha = PanelAlpha();

            // SourceOver straight onto whatever is already there (panel fill,
            // and/or the blur backdrop composited further up) instead of
            // erasing to a solid panelColor and recomputing alpha from
            // panelAlpha alone. That erase used to throw away the blur
            // backdrop's own opacity underneath the cell - with panelOpacity
            // low, the recomputed alpha came out far below
            // the blur's, punching a hole straight through to the real desktop
            // instead of the blurred one. SourceOver only ever raises alpha
            // (outA = srcA + dstA*(1-srcA)), so it can't create that hole.
            const Gdiplus::Color& overlay = pressed ? pressColor : hoverColor;
            Gdiplus::SolidBrush highlightBrush(overlay);
            g.FillPath(&highlightBrush, &highlightPath);

            DrawCell(g, level, cellIndex, panelAlpha, textColor, badgeFillColor,
                     badgeMarkColor, badgeRingColor, *font, format);
        };

        if (level->hoverCell >= 0 && level->hoverCell != level->pressedCell) {
            repaintCell(level->hoverCell, false);
        }
        if (level->pressedCell >= 0) {
            repaintCell(level->pressedCell, true);
        }
    }

    POINT ptSrc{0, 0};
    POINT ptDst{level->rect.left, level->rect.top};
    SIZE size{width, height};
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(level->hwnd, hScreenDC, &ptDst, &size, hMemDC, &ptSrc, 0,
                        &blend, ULW_ALPHA);
    ApplyRoundedRegion(level, width, height);

    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);
    ReleaseDC(nullptr, hScreenDC);
}

// Only captures for Gaussian - Acrylic's blur comes live from DWM's own
// compositor via the accent policy set in ApplyBackdrop, so there is nothing
// for us to capture or composite per-frame.
void CaptureLevelBlur(PopupLevel* level) {
    if (g_settings.blurType != BlurType::Gaussian ||
        g_settings.blurStrength <= 0) {
        level->blurBackdrop.reset();
        return;
    }
    level->blurBackdrop =
        CaptureBlurredBackdrop(level->rect, g_settings.blurStrength);
}

PopupLevel* LevelFromHwnd(HWND hWnd) {
    for (auto& level : Levels()) {
        if (level->hwnd == hWnd) {
            return level.get();
        }
    }
    return nullptr;
}

int CellFromClientPoint(PopupLevel* level, POINT pt) {
    for (size_t i = 0; i < level->cellRects.size() && i < level->items.size();
         i++) {
        if (PtInRect(&level->cellRects[i], pt)) {
            return (int)i;
        }
    }
    return -1;
}

// Hides every level at or below `depth`, deepest first.
void CloseLevelsFrom(int depth) {
    if (depth < 0) {
        depth = 0;
    }
    while ((int)Levels().size() > depth) {
        auto& level = Levels().back();
        if (level->hwnd) {
            KillTimer(level->hwnd, kItemTooltipTimerId);
            ShowWindow(level->hwnd, SW_HIDE);
        }
        Levels().pop_back();
    }
    if (g_pendingSubDepth >= depth - 1) {
        g_pendingSubDepth = -1;
        g_pendingSubCell = -1;
    }
    g_closeDeeperSinceTick = 0;
}

void CloseChain() {
    if (!Levels().empty() && Levels()[0]->hwnd) {
        KillTimer(Levels()[0]->hwnd, kTickTimerId);
        KillTimer(Levels()[0]->hwnd, kBlurTimerId);
    }
    CloseLevelsFrom(0);
    g_outsideSinceTick = 0;
    HideItemTooltip();
}

// The seam between two side-by-side cascade grids: just the strip of
// horizontal gap facing edge-to-edge, clipped to the vertical span where the
// two rects actually overlap. Used to be the full bounding box of both rects
// (min/max of every edge), which - whenever one grid was taller than the
// other, as any parent/subfolder pair with a different item count is -
// swallowed the dead space below (or above) the shorter one into the live
// path too, so hovering there kept the whole cascade open instead of closing
// it.
RECT BridgingRect(const RECT& a, const RECT& b) {
    RECT bridge{};
    if (b.left >= a.right) {
        bridge.left = a.right;
        bridge.right = b.left;
    } else if (a.left >= b.right) {
        bridge.left = b.right;
        bridge.right = a.left;
    } else {
        // Not actually side-by-side (shouldn't happen for cascade levels) -
        // fall back to the old full-bounds behavior rather than a bogus
        // empty/negative-width strip.
        bridge.left = std::min<LONG>(a.left, b.left);
        bridge.right = std::max<LONG>(a.right, b.right);
    }
    bridge.top = std::max<LONG>(a.top, b.top);
    bridge.bottom = std::min<LONG>(a.bottom, b.bottom);
    if (bridge.bottom <= bridge.top) {
        // PtInRect excludes the bottom edge; inflate a zero-height overlap
        // into a 1px seam so two rects that just touch top/bottom still
        // bridge, same trick as CursorInRootCorridor.
        bridge.bottom = bridge.top + 1;
    }
    return bridge;
}

// The dead-zone-free path from the taskbar button up into the root grid. Only
// the gap BETWEEN them counts - the taskbar strip itself is excluded, so moving
// to another app on the taskbar closes the menu instead of keeping it stuck
// open.
bool CursorInRootCorridor(POINT pt) {
    if (Levels().empty()) {
        return false;
    }

    const RECT& anchor = g_rootAnchorRect;
    const RECT& popup = Levels()[0]->rect;

    RECT gap{};
    gap.left = std::min<LONG>(anchor.left, popup.left);
    gap.right = std::max<LONG>(anchor.right, popup.right);

    if (popup.bottom <= anchor.top) {
        // Normal bottom taskbar: popup sits above the button.
        gap.top = popup.bottom;
        gap.bottom = anchor.top;
    } else if (popup.top >= anchor.bottom) {
        // Top taskbar: popup sits below the button.
        gap.top = anchor.bottom;
        gap.bottom = popup.top;
    } else {
        return false;
    }

    // PtInRect excludes the bottom edge; inflate a zero-height seam so a
    // gapAbove of 0 still lets the cursor cross into the grid.
    if (gap.bottom <= gap.top) {
        gap.bottom = gap.top + 1;
    }

    return PtInRect(&gap, pt);
}

// The cascade stays open while the cursor is over the taskbar button, over any
// open grid, in the gap from the button up into the root grid, or in the band
// bridging two consecutive grids. The taskbar strip outside the button is NOT
// part of the live path.
bool CursorIsInLivePath() {
    POINT pt;
    if (!GetCursorPos(&pt) || Levels().empty()) {
        return false;
    }

    if (PtInRect(&g_rootAnchorRect, pt)) {
        return true;
    }

    for (const auto& level : Levels()) {
        if (PtInRect(&level->rect, pt)) {
            return true;
        }
    }

    if (CursorInRootCorridor(pt)) {
        return true;
    }

    for (size_t i = 1; i < Levels().size(); i++) {
        RECT bridge = BridgingRect(Levels()[i - 1]->rect, Levels()[i]->rect);
        if (PtInRect(&bridge, pt)) {
            return true;
        }
    }

    return false;
}

// The deepest open grid containing the cursor, or -1 when it is elsewhere.
int LevelIndexUnderCursor() {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return -1;
    }
    for (int i = (int)Levels().size() - 1; i >= 0; i--) {
        if (PtInRect(&Levels()[i]->rect, pt)) {
            return i;
        }
    }
    return -1;
}

void ClearPendingShellCommand() {
    if (g_pendingShellCommand.contextMenu) {
        g_pendingShellCommand.contextMenu->Release();
    }
    g_pendingShellCommand = {};
    g_invokeActive = false;
}

void InvokePendingShellCommand(HWND owner) {
    IContextMenu* contextMenu = g_pendingShellCommand.contextMenu;
    int commandOffset = g_pendingShellCommand.commandOffset;
    POINT invokePoint = g_pendingShellCommand.invokePoint;
    g_pendingShellCommand = {};
    if (!contextMenu || commandOffset < 0) {
        if (contextMenu) {
            contextMenu->Release();
        }
        g_invokeActive = false;
        return;
    }

    // Keep g_invokeActive true for the duration of InvokeCommand so uninit
    // waits. CMIC_MASK_ASYNCOK lets many verbs return quickly; some still show
    // modal UI on this thread despite the flag.
    struct InvokeActiveGuard {
        InvokeActiveGuard() { g_invokeActive = true; }
        ~InvokeActiveGuard() { g_invokeActive = false; }
        InvokeActiveGuard(const InvokeActiveGuard&) = delete;
        InvokeActiveGuard& operator=(const InvokeActiveGuard&) = delete;
    } invokeActiveGuard;

    CMINVOKECOMMANDINFOEX info{};
    info.cbSize = sizeof(info);
    info.fMask = CMIC_MASK_UNICODE | CMIC_MASK_ASYNCOK | CMIC_MASK_PTINVOKE;
    info.hwnd = owner;
    info.lpVerb = MAKEINTRESOURCEA(commandOffset);
    info.lpVerbW = MAKEINTRESOURCEW(commandOffset);
    info.nShow = SW_SHOWNORMAL;
    info.ptInvoke = invokePoint;

    HRESULT hr = contextMenu->InvokeCommand(
        reinterpret_cast<CMINVOKECOMMANDINFO*>(&info));
    if (FAILED(hr)) {
        Wh_Log(L"InvokeCommand failed hr=0x%08X offset=%d", hr, commandOffset);
    }
    contextMenu->Release();
}

void ShowItemContextMenu(std::wstring path, POINT screenPt) {
    if (g_unloading) {
        return;
    }
    if (!g_menuOwnerWnd ||
        GetWindowThreadProcessId(g_menuOwnerWnd, nullptr) !=
            GetCurrentThreadId()) {
        return;
    }
    if (path.empty()) {
        return;
    }

    struct CoTaskMemDeleter {
        void operator()(void* p) const noexcept { CoTaskMemFree(p); }
    };
    using UniquePidlAbs =
        std::unique_ptr<ITEMIDLIST, CoTaskMemDeleter>;

    PIDLIST_ABSOLUTE rawPidl = nullptr;
    if (FAILED(SHParseDisplayName(path.c_str(), nullptr, &rawPidl, 0,
                                  nullptr)) ||
        !rawPidl) {
        return;
    }
    UniquePidlAbs pidl(rawPidl);

    winrt::com_ptr<IShellFolder> parentFolder;
    PCUITEMID_CHILD child = nullptr;
    if (FAILED(SHBindToParent(pidl.get(), IID_PPV_ARGS(parentFolder.put()),
                              &child)) ||
        !parentFolder) {
        return;
    }

    winrt::com_ptr<IContextMenu> contextMenu;
    if (FAILED(parentFolder->GetUIObjectOf(g_menuOwnerWnd, 1, &child,
                                           IID_IContextMenu, nullptr,
                                           contextMenu.put_void())) ||
        !contextMenu) {
        return;
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }
    struct MenuGuard {
        HMENU h = nullptr;
        explicit MenuGuard(HMENU menu) : h(menu) {}
        ~MenuGuard() {
            if (h) {
                DestroyMenu(h);
            }
        }
        MenuGuard(const MenuGuard&) = delete;
        MenuGuard& operator=(const MenuGuard&) = delete;
    } menuGuard(menu);

    if (FAILED(contextMenu->QueryContextMenu(menu, 0, 1, 0x7FFF,
                                             CMF_NORMAL | CMF_EXPLORE))) {
        return;
    }

    UINT cmd = 0;
    {
        // Raised only while TrackPopupMenuEx runs — cleared before the verb
        // is invoked so Wh_ModUninit does not EndMenu-wait on verb dialogs.
        // RAII so a shell-extension exception cannot leave the flag stuck.
        struct MenuActiveGuard {
            MenuActiveGuard() { g_menuActive = true; }
            ~MenuActiveGuard() { g_menuActive = false; }
            MenuActiveGuard(const MenuActiveGuard&) = delete;
            MenuActiveGuard& operator=(const MenuActiveGuard&) = delete;
        } menuActiveGuard;

        g_activeContextMenu3 = nullptr;
        g_activeContextMenu2 = nullptr;
        if (FAILED(contextMenu->QueryInterface(IID_IContextMenu3,
                                               (void**)&g_activeContextMenu3))) {
            contextMenu->QueryInterface(IID_IContextMenu2,
                                        (void**)&g_activeContextMenu2);
        }

        // A never-shown 0x0 owner cannot become foreground; park a 1x1
        // window at the click so TrackPopupMenuEx dismisses correctly.
        SetWindowPos(g_menuOwnerWnd, HWND_TOPMOST, screenPt.x, screenPt.y, 1, 1,
                     SWP_SHOWWINDOW);
        SetForegroundWindow(g_menuOwnerWnd);
        cmd = TrackPopupMenuEx(menu,
                               TPM_RETURNCMD | TPM_RIGHTBUTTON |
                                   TPM_LEFTALIGN | TPM_BOTTOMALIGN,
                               screenPt.x, screenPt.y, g_menuOwnerWnd,
                               nullptr);
        PostMessageW(g_menuOwnerWnd, WM_NULL, 0, 0);

        if (g_activeContextMenu3) {
            g_activeContextMenu3->Release();
            g_activeContextMenu3 = nullptr;
        }
        if (g_activeContextMenu2) {
            g_activeContextMenu2->Release();
            g_activeContextMenu2 = nullptr;
        }
    }  // g_menuActive = false before any verb runs

    ShowWindow(g_menuOwnerWnd, SW_HIDE);

    if (!cmd || g_unloading) {
        return;
    }

    // Dismiss the hover chain before the shell verb runs so Explorer
    // windows are not covered by stale grids.
    CloseChain();

    // Defer InvokeCommand until after PopupWndProc (WM_RBUTTONUP) returns —
    // same pattern as taskbar-folder-menus. Keep IContextMenu alive across
    // the post; ASYNCOK lets many verbs leave this thread quickly.
    ClearPendingShellCommand();
    g_pendingShellCommand.contextMenu = contextMenu.detach();
    g_pendingShellCommand.commandOffset = (int)cmd - 1;
    g_pendingShellCommand.invokePoint = screenPt;
    g_invokeActive = true;
    if (!PostMessageW(g_menuOwnerWnd, WM_APP_INVOKE_SHELL_VERB, 0, 0)) {
        Wh_Log(L"PostMessage WM_APP_INVOKE_SHELL_VERB failed (error %u); "
               L"invoking inline",
               GetLastError());
        InvokePendingShellCommand(g_menuOwnerWnd);
    }
}

// Plain verb-based open - no SEE_MASK_INVOKEIDLIST. INVOKEIDLIST binds the
// shell namespace object and invokes it via IContextMenu, which contends with
// the background scan/icon-extraction threads' own shell-namespace COM calls;
// combined with SEE_MASK_ASYNCOK (which returns success before that invoke
// finishes), a contended invoke could get silently dropped - ShellExecuteExW
// reports success but no window ever appears. A folder has no custom verb
// handler to invoke, so the fast lpVerb path is both simpler and correct.
void LaunchPath(const std::wstring& path) {
    if (path.empty()) {
        return;
    }

    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_FLAG_NO_UI;
    info.lpFile = path.c_str();
    info.lpVerb = L"open";
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"ShellExecuteEx failed for %s (error %u)", path.c_str(),
               GetLastError());
    }
}

LRESULT CALLBACK MenuOwnerWndProc(HWND hWnd,
                                  UINT uMsg,
                                  WPARAM wParam,
                                  LPARAM lParam) {
    // New monitors create Shell_SecondaryTrayWnd without StartTaskbar; kick a
    // retry so secondary taskbars still get folder buttons.
    if (uMsg == WM_DISPLAYCHANGE && !g_unloading) {
        StartRetryThread();
    }

    if (uMsg == WM_APP_INVOKE_SHELL_VERB) {
        // Runs after ShowItemContextMenu / PopupWndProc have returned.
        if (g_unloading) {
            ClearPendingShellCommand();
        } else {
            InvokePendingShellCommand(hWnd);
        }
        return 0;
    }

    if (uMsg == WM_APP_RELOAD_UI) {
        // Runs after the button flyout's Click handler has returned, so the
        // teardown is not re-entering the flyout item it was invoked from.
        if (!g_unloading) {
            ReloadAndRefreshUI();
        }
        return 0;
    }

    if (uMsg == WM_INITMENUPOPUP || uMsg == WM_DRAWITEM ||
        uMsg == WM_MEASUREITEM || uMsg == WM_MENUCHAR) {
        if (g_activeContextMenu3) {
            LRESULT result = 0;
            if (SUCCEEDED(g_activeContextMenu3->HandleMenuMsg2(
                    uMsg, wParam, lParam, &result))) {
                return result;
            }
        }
        if (g_activeContextMenu2 &&
            SUCCEEDED(
                g_activeContextMenu2->HandleMenuMsg(uMsg, wParam, lParam))) {
            return uMsg == WM_INITMENUPOPUP ? 0 : TRUE;
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Re-copies any level still waiting on its first scan. Levels resize as their
// contents arrive, so this reopens rather than just repainting.
void RefreshLoadingLevels() {
    for (size_t i = 0; i < Levels().size(); i++) {
        auto* level = Levels()[i].get();
        if (!level->loading) {
            continue;
        }
        auto data = GetFolderData(level->path);
        if (!data || !data->ready) {
            continue;
        }
        // Copied out first: reopening destroys the level these came from.
        std::wstring path = level->path;
        std::wstring title = level->title;
        RECT anchorRect = level->anchorRect;
        int spawnerCell = level->spawnerCell;
        if (i == 0) {
            OpenRootLevel(std::move(path), anchorRect, std::move(title));
        } else {
            OpenSubLevel((int)i - 1, spawnerCell);
        }
        return;
    }
}

// Queues scans for the subfolders of a level so cascading into one is instant.
// Capped so a folder full of folders does not flood the scan thread.
void PrefetchSubfolders(PopupLevel* level) {
    if (!CanExpand(level->depth)) {
        return;
    }
    constexpr int kMaxPrefetch = 12;
    int iconPixelSize = PopupIconPixelSize();
    int queued = 0;
    for (const auto& item : level->items) {
        if (queued >= kMaxPrefetch) {
            break;
        }
        if (item.isFolder && !GetFolderData(item.fullPath)) {
            RequestScan(item.fullPath, iconPixelSize);
            queued++;
        }
    }
}

// Re-captures every open level's blurred backdrop and repaints it, so the
// blur tracks a moving/changing background live instead of freezing whatever
// was behind the grid at the moment it opened. Runs off its own kBlurTimerId
// (kBlurTimerMs), separate from kTickTimerMs so raising/lowering the
// hover/close poll rate doesn't also have to raise (or cap) the blur's.
// Gaussian only - Acrylic's blur is live from DWM already, nothing to
// re-capture (see CaptureLevelBlur).
//
// Deliberately does not touch level->baseDirty: blur is composited straight
// into PaintLevel's DIB (see there), not baked into cachedBase, so this stays
// a cheap BitBlt + resample + composite - no icon/text/badge GDI+ redraw -
// on every one of these ticks.
void RefreshBlurBackdrops() {
    if (g_settings.blurType != BlurType::Gaussian ||
        g_settings.blurStrength <= 0) {
        return;
    }
    for (auto& levelPtr : Levels()) {
        PopupLevel* level = levelPtr.get();
        if (!level || !level->hwnd || !IsWindowVisible(level->hwnd)) {
            continue;
        }
        CaptureLevelBlur(level);
        PaintLevel(level);
    }
}

// Drives the cascade: opens a pending submenu once its dwell time elapses,
// retires levels the cursor has moved away from, and dismisses the chain when
// the cursor leaves the live path. Runs on level 0's window only.
void OnTick() {
    if (g_menuActive) {
        g_outsideSinceTick = 0;
        g_closeDeeperSinceTick = 0;
        return;
    }

    RefreshLoadingLevels();

    ULONGLONG now = GetTickCount64();

    if (!CursorIsInLivePath()) {
        if (g_outsideSinceTick == 0) {
            g_outsideSinceTick = now;
        } else if (now - g_outsideSinceTick >=
                   (ULONGLONG)g_settings.closeDelayMs) {
            CloseChain();
        }
        return;
    }
    g_outsideSinceTick = 0;

    int cursorLevel = LevelIndexUnderCursor();

    // Retire anything deeper than the grid the cursor is in, unless the cursor
    // is still resting on the cell that opened the next one down.
    if (cursorLevel >= 0 && (int)Levels().size() > cursorLevel + 1) {
        auto* level = Levels()[cursorLevel].get();
        bool onSpawner =
            level->hoverCell >= 0 &&
            level->hoverCell == Levels()[cursorLevel + 1]->spawnerCell;
        if (onSpawner) {
            g_closeDeeperSinceTick = 0;
        } else if (g_closeDeeperSinceTick == 0) {
            g_closeDeeperSinceTick = now;
        } else if (now - g_closeDeeperSinceTick >=
                   (ULONGLONG)g_settings.submenuCloseDelayMs) {
            CloseLevelsFrom(cursorLevel + 1);
        }
    } else {
        g_closeDeeperSinceTick = 0;
    }

    if (g_pendingSubDepth >= 0 &&
        now - g_pendingSubSinceTick >= (ULONGLONG)g_settings.submenuDelayMs) {
        int depth = g_pendingSubDepth;
        int cell = g_pendingSubCell;
        g_pendingSubDepth = -1;
        g_pendingSubCell = -1;
        OpenSubLevel(depth, cell);
    }
}

// One shared standard tooltip control, tracked to the cursor, for grid item
// labels the ellipsis cut off. Lazily created; DestroyItemTooltip below tears
// it down on unload, since the HWND belongs to this DLL's image and a fresh
// one would otherwise be created (and leaked) on every reload.
HWND g_itemTooltip = nullptr;

HWND EnsureItemTooltip() {
    if (g_itemTooltip) {
        return g_itemTooltip;
    }
    HWND tooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                   WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                                   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                   CW_USEDEFAULT, nullptr, nullptr,
                                   GetCurrentModuleHandle(), nullptr);
    if (!tooltip) {
        return nullptr;
    }
    TOOLINFOW info{sizeof(info)};
    info.uFlags = TTF_TRACK | TTF_ABSOLUTE;
    info.hwnd = nullptr;
    info.uId = 1;
    info.lpszText = const_cast<LPWSTR>(L"");
    SendMessageW(tooltip, TTM_ADDTOOL, 0, (LPARAM)&info);
    g_itemTooltip = tooltip;
    return tooltip;
}

// Must run on the tooltip's creating UI thread, like the rest of
// teardownOnUiThread.
void DestroyItemTooltip() {
    if (g_itemTooltip) {
        DestroyWindow(g_itemTooltip);
        g_itemTooltip = nullptr;
    }
}

void HideItemTooltip() {
    // No-op rather than EnsureItemTooltip: nothing was ever shown, so there is
    // nothing to hide, and creating one here just to hide it is wasted on
    // every teardown.
    if (!g_itemTooltip) {
        return;
    }
    TOOLINFOW info{sizeof(info)};
    info.hwnd = nullptr;
    info.uId = 1;
    SendMessageW(g_itemTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&info);
}

// screenPt is where the cursor is; the tooltip is offset below-right of it so
// the pointer doesn't sit on top of its own tip.
void ShowItemTooltip(const std::wstring& text, POINT screenPt) {
    HWND tooltip = EnsureItemTooltip();
    if (!tooltip) {
        return;
    }
    bool dark = IsDarkPopupTheme();
    SendMessageW(tooltip, TTM_SETTIPBKCOLOR, 0,
                (LPARAM)(dark ? RGB(43, 43, 43) : RGB(249, 249, 249)));
    SendMessageW(tooltip, TTM_SETTIPTEXTCOLOR, 0,
                (LPARAM)(dark ? RGB(255, 255, 255) : RGB(0, 0, 0)));

    TOOLINFOW info{sizeof(info)};
    info.hwnd = nullptr;
    info.uId = 1;
    info.lpszText = const_cast<LPWSTR>(text.c_str());
    SendMessageW(tooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&info);
    SendMessageW(tooltip, TTM_TRACKPOSITION, 0,
                (LPARAM)MAKELONG(screenPt.x + 16, screenPt.y + 20));
    SendMessageW(tooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&info);
}

LRESULT CALLBACK PopupWndProc(HWND hWnd,
                              UINT uMsg,
                              WPARAM wParam,
                              LPARAM lParam) {
    // The open-delay timer fires before any level exists, so it is handled
    // before the level lookup.
    if (uMsg == WM_TIMER && wParam == kOpenTimerId) {
        KillTimer(hWnd, kOpenTimerId);
        std::wstring path = std::move(g_pendingRootPath);
        std::wstring title = std::move(g_pendingRootTitle);
        g_pendingRootPath.clear();
        g_pendingRootTitle.clear();
        POINT cursor{};
        // The cursor may have moved on during the delay.
        if (!path.empty() && GetCursorPos(&cursor) &&
            PtInRect(&g_pendingRootRect, cursor)) {
            OpenRootLevel(std::move(path), g_pendingRootRect, std::move(title));
        }
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == kTickTimerId) {
        OnTick();
        return 0;
    }

    if (uMsg == WM_TIMER && wParam == kBlurTimerId) {
        RefreshBlurBackdrops();
        return 0;
    }

    // Theme/display messages do not need a live level; handle them even when
    // the HWND is a reused shell with no PopupLevel attached yet. Top-level
    // popups receive WM_SETTINGCHANGE broadcasts even while hidden, so only
    // dismiss an open chain — otherwise every SPI broadcast would CloseChain
    // and cancel pending opens. Monitor reinject is owned by MenuOwnerWndProc.
    if (uMsg == WM_DISPLAYCHANGE || uMsg == WM_SETTINGCHANGE ||
        uMsg == WM_THEMECHANGED) {
        RefreshThemeCache();
        if (!Levels().empty()) {
            CloseChain();
        }
        return 0;
    }

    PopupLevel* level = LevelFromHwnd(hWnd);
    if (!level) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_MOUSEMOVE: {
            TRACKMOUSEEVENT tme{sizeof(tme)};
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);

            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int cell = CellFromClientPoint(level, pt);
            if (cell != level->hoverCell) {
                level->hoverCell = cell;
                PaintLevel(level);

                bool expandable =
                    cell >= 0 && cell < (int)level->items.size() &&
                    level->items[cell].isFolder && CanExpand(level->depth);
                int alreadyOpen = (int)Levels().size() > level->depth + 1
                                      ? Levels()[level->depth + 1]->spawnerCell
                                      : -1;
                if (expandable && cell != alreadyOpen) {
                    g_pendingSubDepth = level->depth;
                    g_pendingSubCell = cell;
                    g_pendingSubSinceTick = GetTickCount64();
                } else {
                    g_pendingSubDepth = -1;
                    g_pendingSubCell = -1;
                }
            }

            bool truncated =
                cell >= 0 && cell < (int)level->labelTruncated.size() &&
                level->labelTruncated[cell];
            if (cell != level->tooltipPendingCell) {
                HideItemTooltip();
                KillTimer(hWnd, kItemTooltipTimerId);
                level->tooltipPendingCell = truncated ? cell : -1;
                if (truncated) {
                    SetTimer(hWnd, kItemTooltipTimerId, kItemTooltipDelayMs,
                             nullptr);
                }
            }

            g_outsideSinceTick = 0;
            return 0;
        }

        case WM_TIMER:
            if (wParam == kItemTooltipTimerId) {
                KillTimer(hWnd, kItemTooltipTimerId);
                int cell = level->tooltipPendingCell;
                if (cell == level->hoverCell && cell >= 0 &&
                    cell < (int)level->items.size()) {
                    POINT screenPt;
                    GetCursorPos(&screenPt);
                    ShowItemTooltip(level->items[cell].displayName, screenPt);
                }
                return 0;
            }
            break;

        case WM_MOUSELEAVE:
            HideItemTooltip();
            KillTimer(hWnd, kItemTooltipTimerId);
            level->tooltipPendingCell = -1;
            return 0;

        case WM_LBUTTONDOWN: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            level->pressedCell = CellFromClientPoint(level, pt);
            PaintLevel(level);
            HideItemTooltip();
            KillTimer(hWnd, kItemTooltipTimerId);
            level->tooltipPendingCell = -1;
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int cell = CellFromClientPoint(level, pt);
            int pressed = level->pressedCell;
            level->pressedCell = -1;
            if (cell >= 0 && cell == pressed &&
                cell < (int)level->items.size()) {
                std::wstring path = level->items[cell].fullPath;
                CloseChain();
                LaunchPath(path);
            } else {
                PaintLevel(level);
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int cell = CellFromClientPoint(level, pt);
            if (cell >= 0 && cell < (int)level->items.size()) {
                POINT screenPt = pt;
                ClientToScreen(hWnd, &screenPt);
                const GridItem& item = level->items[cell];
                // iconPath holds the .lnk when fullPath was replaced by its
                // resolved folder target; the menu must act on what the user
                // actually sees in the cell.
                const std::wstring& menuPath =
                    item.iconPath.empty() ? item.fullPath : item.iconPath;
                ShowItemContextMenu(menuPath, screenPt);
            }
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hWnd, kTickTimerId);
            KillTimer(hWnd, kOpenTimerId);
            KillTimer(hWnd, kBlurTimerId);
            HideItemTooltip();
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool EnsurePopupClasses() {
    // Only success is cached. A failure is not latched: it can come from a
    // class a previous load left registered behind windows that outlived it,
    // and latching would disable every hover grid for the rest of the session.
    static bool registered = false;
    if (registered) {
        return true;
    }

    HINSTANCE instance = GetCurrentModuleHandle();
    WNDCLASSEXW popupClass{};
    popupClass.cbSize = sizeof(popupClass);
    popupClass.lpfnWndProc = PopupWndProc;
    popupClass.hInstance = instance;
    popupClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    popupClass.lpszClassName = kPopupClassName;

    WNDCLASSEXW ownerClass{};
    ownerClass.cbSize = sizeof(ownerClass);
    ownerClass.lpfnWndProc = MenuOwnerWndProc;
    ownerClass.hInstance = instance;
    ownerClass.lpszClassName = kMenuOwnerClassName;

    if (!RegisterModClass(popupClass) || !RegisterModClass(ownerClass)) {
        Wh_Log(L"hover grid classes unavailable; hover grids disabled");
        return false;
    }

    registered = true;
    return true;
}

HWND EnsureMenuOwnerWindow() {
    if (g_menuOwnerWnd) {
        // DestroyWindow/CreateWindow must run on the owning thread; a wrong-
        // thread caller cannot replace the window safely.
        if (GetWindowThreadProcessId(g_menuOwnerWnd, nullptr) !=
            GetCurrentThreadId()) {
            Wh_Log(L"Menu owner window belongs to a different thread; refusing "
                   L"to use or recreate it from this thread");
            return nullptr;
        }
        return g_menuOwnerWnd;
    }
    if (!EnsurePopupClasses()) {
        return nullptr;
    }

    // Top-level so it receives WM_DISPLAYCHANGE when a monitor is plugged in.
    // Kept hidden until a shell context menu needs an activatable owner.
    // Must be created on the taskbar UI thread (via InjectHostGrids /
    // EnsureLevelWindow).
    g_menuOwnerWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW, kMenuOwnerClassName, L"", WS_POPUP, 0, 0, 0, 0,
        nullptr, nullptr, GetCurrentModuleHandle(), nullptr);
    if (!g_menuOwnerWnd) {
        Wh_Log(L"Failed to create menu owner window (error %u)", GetLastError());
    }
    return g_menuOwnerWnd;
}

// Returns the reusable window for the given cascade depth, creating it on first
// use. Depth 0's window also owns the tick timer.
HWND EnsureLevelWindow(int depth) {
    if (depth < 0 || depth >= kMaxLevels) {
        return nullptr;
    }
    if (depth < (int)g_levelWindows.size() && g_levelWindows[depth]) {
        return g_levelWindows[depth];
    }

    if (!EnsurePopupClasses()) {
        return nullptr;
    }
    // Best-effort: owner is only required for shell context menus. A failure
    // must not block creating the hover grid window itself.
    EnsureMenuOwnerWindow();

    HWND hWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        kPopupClassName, L"", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr,
        GetCurrentModuleHandle(), nullptr);
    if (!hWnd) {
        Wh_Log(L"Failed to create the hover grid window (error %u)",
               GetLastError());
        return nullptr;
    }

    ApplyBackdrop(hWnd);

    if ((int)g_levelWindows.size() <= depth) {
        g_levelWindows.resize(depth + 1, nullptr);
    }
    g_levelWindows[depth] = hWnd;
    return hWnd;
}

void UpdatePopupDpi() {
    // Sub-levels intentionally keep the root's DPI so a cascade that spills
    // onto another monitor does not suddenly change cell sizes mid-gesture.
    HWND reference = g_taskbarWnd;
    if (!reference && !g_levelWindows.empty()) {
        reference = g_levelWindows[0];
    }
    if (reference) {
        int dpi = (int)GetDpiForWindow(reference);
        g_popupDpi = dpi > 0 ? dpi : 96;
    }
}

// Seats a level at its depth and retires anything deeper. The window at this
// depth stays mapped, so reopening the same depth resizes in place instead of
// blinking.
PopupLevel* InstallLevel(std::unique_ptr<PopupLevel> level) {
    int depth = level->depth;
    CloseLevelsFrom(depth + 1);
    if ((int)Levels().size() <= depth) {
        Levels().resize(depth);
        Levels().push_back(std::move(level));
    } else {
        Levels()[depth] = std::move(level);
    }
    return Levels()[depth].get();
}

// Paints a level's window, then shows it - deliberately in that order. The
// window at this depth is reused across opens, so it can still be carrying
// the previous open's bitmap; showing it before PaintLevel/UpdateLayeredWindow
// replaces that content flashes the stale frame at the new position/size for
// however long capture+repaint takes. That flash reads as a glitch most
// visibly when blur is on, since the stale backdrop can differ sharply from
// the real one, but it existed for plain content too. PaintLevel's own
// UpdateLayeredWindow call already places and sizes the window (that is what
// ptDst/size are for); the SetWindowPos below only has to raise it topmost
// and make it visible, hidden window content updates just fine.
void PresentLevel(PopupLevel* level, const SIZE& size) {
    level->rect.right = level->rect.left + size.cx;
    level->rect.bottom = level->rect.top + size.cy;
    InvalidateLevelBase(level);

    // SetOwnWindowsCaptureExcluded keeps this window's own pixels out of its
    // own Gaussian capture only for the instant that capture runs, so no need
    // to hide it first. RefreshBlurBackdrops re-captures this on every tick
    // to track a moving/changing background live (Gaussian only - Acrylic's
    // blur is live from DWM already).
    CaptureLevelBlur(level);

    PaintLevel(level);
    SetWindowPos(level->hwnd, HWND_TOPMOST, level->rect.left, level->rect.top,
                 size.cx, size.cy, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void OpenRootLevel(std::wstring path, RECT anchorRect, std::wstring title) {
    if (g_unloading || path.empty()) {
        return;
    }

    HWND hWnd = EnsureLevelWindow(0);
    if (!hWnd) {
        return;
    }

    UpdatePopupDpi();

    auto level = std::make_unique<PopupLevel>();
    level->hwnd = hWnd;
    level->depth = 0;
    level->path = path;
    level->title = std::move(title);
    level->anchorRect = anchorRect;

    auto data =
        GetFolderDataAndRefresh(path, PopupIconPixelSize());
    level->loading = true;
    if (data && data->ready) {
        level->items = data->items;
        level->loading = false;
    }

    g_rootAnchorRect = anchorRect;
    g_outsideSinceTick = 0;

    HMONITOR monitor = MonitorFromRect(&anchorRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    const RECT& screen = monitorInfo.rcMonitor;

    int gap = ScaleForPopup(g_settings.gapAbove);
    int availableAbove = anchorRect.top - gap - screen.top;
    int availableBelow = screen.bottom - anchorRect.bottom - gap;
    int maxHeight = std::max(availableAbove, availableBelow);
    if (maxHeight < ScaleForPopup(96)) {
        maxHeight = std::max(ScaleForPopup(96),
                             (int)(screen.bottom - screen.top) - ScaleForPopup(16));
    }
    int maxWidth =
        std::max(0, (int)(screen.right - screen.left) - ScaleForPopup(8));

    SIZE size = ComputeLevelLayout(level.get(), maxHeight, maxWidth);

    int centerX = (anchorRect.left + anchorRect.right) / 2;
    int left = centerX - size.cx / 2;

    // Prefer above the taskbar; never flip below a bottom taskbar where the
    // grid would land past the monitor edge.
    if (availableAbove >= size.cy) {
        level->rect.top = anchorRect.top - gap - size.cy;
    } else if (availableBelow >= size.cy) {
        level->rect.top = anchorRect.bottom + gap;
    } else if (availableAbove >= availableBelow) {
        level->rect.top = screen.top;
    } else {
        level->rect.top = std::max<int>(screen.top, screen.bottom - size.cy);
    }
    level->rect.top = std::clamp<int>(
        level->rect.top, screen.top,
        std::max<int>(screen.top, screen.bottom - size.cy));
    int loLeft = screen.left + ScaleForPopup(4);
    int hiLeft = screen.right - size.cx - ScaleForPopup(4);
    level->rect.left =
        hiLeft <= loLeft ? screen.left : std::clamp(left, loLeft, hiLeft);

    auto* installed = InstallLevel(std::move(level));
    PresentLevel(installed, size);
    PrefetchSubfolders(installed);

    SetTimer(hWnd, kTickTimerId, kTickTimerMs, nullptr);
    SetTimer(hWnd, kBlurTimerId, kBlurTimerMs, nullptr);
}

// Opens the grid for the subfolder in `cell` of the level at `parentDepth`,
// cascading to the side of the parent like a normal submenu.
void OpenSubLevel(int parentDepth, int cell) {
    if (g_unloading || parentDepth < 0 || parentDepth >= (int)Levels().size()) {
        return;
    }

    auto* parent = Levels()[parentDepth].get();
    if (cell < 0 || cell >= (int)parent->items.size() ||
        !parent->items[cell].isFolder || !CanExpand(parentDepth)) {
        return;
    }

    int depth = parentDepth + 1;
    HWND hWnd = EnsureLevelWindow(depth);
    if (!hWnd) {
        return;
    }

    auto level = std::make_unique<PopupLevel>();
    level->hwnd = hWnd;
    level->depth = depth;
    level->path = parent->items[cell].fullPath;
    level->spawnerCell = cell;

    RECT cellScreenRect = parent->cellRects[cell];
    OffsetRect(&cellScreenRect, parent->rect.left, parent->rect.top);
    level->anchorRect = cellScreenRect;

    auto data = GetFolderDataAndRefresh(level->path, PopupIconPixelSize());
    level->loading = true;
    if (data && data->ready) {
        level->items = data->items;
        level->loading = false;
    }

    HMONITOR monitor = MonitorFromRect(&parent->rect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    const RECT& screen = monitorInfo.rcMonitor;
    const RECT& screenWork = monitorInfo.rcWork;

    // Cap height to the work area (excludes the taskbar) and width to the
    // parent monitor so cascade grids stay on-screen.
    int maxHeight = std::max(
        0, (int)(screenWork.bottom - screenWork.top) - ScaleForPopup(16));
    int maxWidth =
        std::max(0, (int)(screen.right - screen.left) - ScaleForPopup(8));
    SIZE size = ComputeLevelLayout(level.get(), maxHeight, maxWidth);

    int seam = ScaleForPopup(2);
    int left = parent->rect.right + seam;
    if (left + size.cx > screen.right) {
        int flipped = parent->rect.left - seam - size.cx;
        left = flipped >= screen.left
                   ? flipped
                   : std::max<int>(screen.left, screen.right - size.cx);
    }

    // Top-aligned with the cell that opened it, kept within the work area.
    int top = cellScreenRect.top - ScaleForPopup(8);
    top = std::clamp<int>(
        top, screenWork.top,
        std::max<int>(screenWork.top, screenWork.bottom - size.cy));

    int loLeft = screen.left;
    int hiLeft = screen.right - size.cx;
    level->rect.left =
        hiLeft <= loLeft ? screen.left : std::clamp(left, loLeft, hiLeft);
    level->rect.top = top;

    auto* installed = InstallLevel(std::move(level));
    PresentLevel(installed, size);
    PrefetchSubfolders(installed);
}

////////////////////////////////////////////////////////////////////////////////
// The taskbar buttons
//
// There are none any more, and that is the point. The folder buttons are real
// pinned taskbar items now (see the Pins namespace), so Windows creates, lays
// out and animates them. All this code has to do is notice which of the shell's
// buttons are ours and attach hover to them.
//
// What used to be here was an overlay: a Grid appended to the taskbar's root,
// with the neighbouring icon's margin widened to carve a gap, and roughly two
// thousand lines keeping the overlay seated in that gap — anchor resolution, a
// strip census, slide animations, settle holds, parked-anchor holds, drag
// freezes. None of it could ever be exactly right. Taskbar item positions are
// driven by implicit Windows.UI.Composition animations that interpolate on the
// compositor thread, and no other HWND can sample an animation mid-flight, so
// the overlay was always at least a frame behind and worse across inserts and
// reorders. It was chasing a value it could only predict.
//
// Deleting it is what fixed the animations, and dragging came free with it.

// Hover attached to one genuine Taskbar.TaskListButton. Held per host because
// the containers belong to that taskbar's repeater and must be released on the
// thread that owns them.
struct PinBinding {
    FrameworkElement button{nullptr};
    int folderIndex = -1;
    winrt::event_token enterToken{};
    winrt::event_token exitToken{};
    // The shell's tooltip, saved before we clear it, so unbinding can put it
    // back rather than leaving the container tooltip-less for whatever the
    // shell realizes there next.
    winrt::Windows::Foundation::IInspectable savedTooltip{nullptr};
};

// One per taskbar window (primary + each secondary monitor).
struct TaskbarHost {
    HWND hwnd = nullptr;
    Grid trackedRootGrid{nullptr};
    FrameworkElement cachedRepeater{nullptr};
    winrt::event_token layoutUpdatedToken{};
    // Hover on the real pinned items. Rebuilt whenever the realized button set
    // changes, since ItemsRepeater recycles containers between items.
    std::vector<PinBinding> pinBindings;
    int lastPinBindCount = -1;
    // Identity of the realized children as of the last bind, so the common case
    // (a layout pass that changed nothing) costs one comparison rather than a
    // full detach/reattach.
    std::vector<void*> lastRealizedChildren;
    uint32_t lastLabelGeneration = 0;
};

[[clang::no_destroy]] std::optional<std::vector<std::unique_ptr<TaskbarHost>>>
    g_taskbarHosts{std::in_place};

std::vector<std::unique_ptr<TaskbarHost>>& TaskbarHosts() {
    if (!g_taskbarHosts) {
        g_taskbarHosts.emplace();
    }
    return *g_taskbarHosts;
}

std::atomic<bool> g_injectionLive{false};

// Lifetime retry worker: stays alive until StopRetryThread (settings/uninit).
// StartRetryThread ensures the worker exists and SetEvent(kick) — never joins
// from a window proc. A HANDLE has no destructor that aborts at process exit.
// StopRetryThread joins with MsgWaitForMultipleObjects so settings/uninit can
// wait without deadlocking on inbound SendMessage.
HANDLE g_retryThread = nullptr;
HANDLE g_retryStopEvent = nullptr;   // manual-reset
HANDLE g_retryKickEvent = nullptr;   // auto-reset
SRWLOCK g_retryLock = SRWLOCK_INIT;

RECT GetElementScreenRect(FrameworkElement const& element) {
    RECT rect{};
    if (!element || !g_taskbarWnd) {
        return rect;
    }
    try {
        auto transform = element.TransformToVisual(nullptr);
        auto point = transform.TransformPoint({0, 0});

        double scale = 1.0;
        if (auto xamlRoot = element.XamlRoot()) {
            scale = xamlRoot.RasterizationScale();
        }
        if (scale <= 0.0) {
            scale = 1.0;
        }

        POINT origin{0, 0};
        ClientToScreen(g_taskbarWnd, &origin);

        rect.left = origin.x + (LONG)std::lround(point.X * scale);
        rect.top = origin.y + (LONG)std::lround(point.Y * scale);
        rect.right =
            rect.left + (LONG)std::lround(element.ActualWidth() * scale);
        rect.bottom =
            rect.top + (LONG)std::lround(element.ActualHeight() * scale);
    } catch (...) {
        rect = RECT{};
    }
    return rect;
}

FrameworkElement FindTaskbarRepeater(Grid const& rootGrid) {
    return FindChildByName(rootGrid, L"TaskbarFrameRepeater", 6);
}

// The vertical extent comes from the taskbar window rather than the button, so
// the corridor covers the whole taskbar height under the button. Horizontally
// the XAML transform is trusted only if it lands inside the taskbar; otherwise
// the cursor position, which is definitely over the button, is used instead.
RECT ComputeHoverAnchorRect(FrameworkElement const& button) {
    RECT taskbarRect{};
    if (!g_taskbarWnd || !GetWindowRect(g_taskbarWnd, &taskbarRect)) {
        return RECT{};
    }

    RECT rect = GetElementScreenRect(button);
    LONG width = rect.right - rect.left;

    if (width <= 0 || rect.left < taskbarRect.left ||
        rect.right > taskbarRect.right) {
        POINT cursor{};
        if (!GetCursorPos(&cursor)) {
            return RECT{};
        }
        LONG half = width > 0 ? width / 2 : 22;
        if (half <= 0) {
            half = 22;
        }
        rect.left = cursor.x - half;
        rect.right = cursor.x + half;
        Wh_Log(L"Button rect fell outside the taskbar, using the cursor instead");
    }

    rect.top = taskbarRect.top;
    rect.bottom = taskbarRect.bottom;
    return rect;
}

std::wstring FolderPathForButton(int folderIndex) {
    std::lock_guard<std::mutex> lock(g_foldersMutex);
    if (folderIndex < 0 || folderIndex >= (int)g_settings.folders.size()) {
        return L"";
    }
    const FolderEntry& entry = g_settings.folders[folderIndex];
    if (!entry.resolvedPath.empty()) {
        return entry.resolvedPath;
    }
    // Unresolved shell: not a filesystem path — OpenRootLevel / FindFirstFile
    // must wait for ResolvePendingFolderEntries on an STA.
    if (IsShellFolderPath(entry.path)) {
        return L"";
    }
    return entry.path;
}

std::wstring FolderNameForButton(int folderIndex) {
    std::lock_guard<std::mutex> lock(g_foldersMutex);
    if (folderIndex < 0 || folderIndex >= (int)g_settings.folders.size()) {
        return L"";
    }
    return g_settings.folders[folderIndex].name;
}

// `button` is either the mod's own overlay Button or a real Taskbar.TaskListButton
// once hover is bound to the genuine pinned item, so it is taken as the common
// base. ComputeHoverAnchorRect only needs TransformToVisual, which both provide —
// and on the real button that transform is the one Windows is itself animating,
// which is the whole point of the switch.
void OnPointerEnteredButton(int folderIndex,
                             FrameworkElement const& button,
                             HWND taskbarWnd) {
    if (g_unloading) {
        return;
    }

    if (taskbarWnd) {
        g_taskbarWnd = taskbarWnd;
    }

    // Taskbar UI STA: finish any pending shell: resolve before open/scan.
    ResolvePendingFolderEntries();
    std::wstring path = FolderPathForButton(folderIndex);
    if (path.empty()) {
        return;
    }
    std::wstring title = FolderNameForButton(folderIndex);

    RECT buttonRect = ComputeHoverAnchorRect(button);
    if (buttonRect.right <= buttonRect.left) {
        return;
    }

    if (g_settings.hoverDelayMs <= 0) {
        OpenRootLevel(std::move(path), buttonRect, std::move(title));
        return;
    }

    HWND hWnd = EnsureLevelWindow(0);
    if (!hWnd) {
        return;
    }
    g_pendingRootPath = std::move(path);
    g_pendingRootTitle = std::move(title);
    g_pendingRootRect = buttonRect;
    SetTimer(hWnd, kOpenTimerId, (UINT)g_settings.hoverDelayMs, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Hover on the real pinned buttons
//
// The genuine taskbar item is a Taskbar.TaskListButton that Windows creates,
// lays out and animates. Attaching hover to it — rather than to a drawn stand-in
// parked next to it — is what makes the grid open in exactly the right place
// while the strip is mid-animation: TransformToVisual is read once, at open
// time, from the element the compositor is actually moving.
//
// Matching resolves the button's label through the shell's own pinned copies to
// an AppUserModelID, and from there to the folder entry. Comparing the label
// straight against the name the mod would like to use does not work: pinning
// copies the shortcut, so a pinned item keeps the name it had when it was
// pinned even after the source is renamed, and the two drift apart.
//
// The label is still the only handle XAML gives us on a taskbar button without
// hooking the shell's group type, but going through the pinned copy means the
// comparison is against what is really there rather than what should be.

std::wstring AutomationNameOf(FrameworkElement const& element) {
    try {
        auto name = winrt::Windows::UI::Xaml::Automation::AutomationProperties::
            GetName(element);
        return std::wstring(name);
    } catch (...) {
        return L"";
    }
}

// -1 when this button is not one of ours.
int FolderIndexForTaskListButton(FrameworkElement const& button) {
    std::wstring label = AutomationNameOf(button);
    if (label.empty()) {
        return -1;
    }
    std::wstring pinId = Pins::PinIdForLabel(label);
    if (pinId.empty()) {
        return -1;
    }
    std::lock_guard<std::mutex> lock(g_foldersMutex);
    for (size_t i = 0; i < g_settings.folders.size(); i++) {
        if (_wcsicmp(g_settings.folders[i].pinId.c_str(), pinId.c_str()) == 0) {
            return (int)i;
        }
    }
    // The shell has a pin the loaded folder list knows nothing about. Normally
    // means g_settings was built before the pin ids existed — see the reload
    // Pins::Reconcile asks for when it generates them.
    Wh_Log(L"Pins: '%s' resolves to pin %s, but no loaded folder claims it",
           label.c_str(), pinId.c_str());
    return -1;
}

void OnPointerExitedButton();

// Drops every handler this host attached to a real taskbar button. Must run
// before rebinding: ItemsRepeater recycles its containers, so the element that
// was our button a moment ago may now be showing an unrelated app.
void UnbindPinButtons(TaskbarHost* host) {
    for (auto& binding : host->pinBindings) {
        if (!binding.button) {
            continue;
        }
        try {
            if (binding.enterToken.value) {
                binding.button.PointerEntered(binding.enterToken);
            }
            if (binding.exitToken.value) {
                binding.button.PointerExited(binding.exitToken);
            }
            if (binding.savedTooltip ==
                winrt::Windows::UI::Xaml::DependencyProperty::UnsetValue()) {
                binding.button.ClearValue(ToolTipService::ToolTipProperty());
            } else {
                binding.button.SetValue(ToolTipService::ToolTipProperty(),
                                        binding.savedTooltip);
            }
        } catch (...) {
            // The element can already be torn down; nothing to release then.
        }
    }
    host->pinBindings.clear();
}

// Attaches hover to whichever realized taskbar buttons are ours right now.
void RebindPinButtons(TaskbarHost* host) {
    if (!host || !host->cachedRepeater || g_unloading) {
        return;
    }

    std::vector<FrameworkElement> children;
    EnumRepeaterChildren(host->cachedRepeater, &children);

    // LayoutUpdated fires constantly — on every animation frame of every icon —
    // and almost none of those passes change which containers exist. Rebinding
    // each time would detach and reattach handlers dozens of times a second for
    // no reason, so compare identities first and do nothing in the common case.
    std::vector<void*> identities;
    identities.reserve(children.size());
    for (const auto& child : children) {
        identities.push_back(winrt::get_abi(child));
    }
    // The map is published by the pin worker after a reconcile, which is a
    // different thread and usually lands when the taskbar is otherwise idle. A
    // pure identity check would then never rebind — the containers did not
    // change, only our knowledge of what they mean did.
    uint32_t labels = Pins::g_labelGeneration.load(std::memory_order_acquire);
    bool identityUnchanged = identities == host->lastRealizedChildren &&
                              labels == host->lastLabelGeneration;

    // Neither of the above catches a drag-to-reorder: ItemsRepeater recycles
    // TaskListButton containers in place, so the realized identity set can
    // stay byte-for-byte the same while a container starts showing a
    // different pinned item — and PublishLabels does not bump the generation
    // either, since a reorder does not change the (leaf, pinId) map, only
    // which container each leaf is realized on. Re-reading each already-bound
    // button's live label is the only way to notice that; it is a handful of
    // AutomationNameOf() calls on bound buttons only, not a rebuild of
    // anything.
    bool bindingsStale = false;
    if (identityUnchanged) {
        for (const auto& binding : host->pinBindings) {
            if (!binding.button || FolderIndexForTaskListButton(
                                        binding.button) != binding.folderIndex) {
                bindingsStale = true;
                break;
            }
        }
    }

    if (identityUnchanged && !bindingsStale) {
        return;
    }
    host->lastRealizedChildren = std::move(identities);
    host->lastLabelGeneration = labels;

    UnbindPinButtons(host);

    // Resolve every realized button's folder index up front so a folder whose
    // label prefix-matches two different realized buttons (e.g. our own
    // "Games" next to an unrelated app whose title happens to start the same
    // way) can be caught before either one is bound. Binding the wrong one
    // would silently steal hover from that other app's button; binding
    // neither just means this folder's button does nothing until the name
    // collision goes away.
    std::unordered_map<int, int> folderIndexCounts;
    std::vector<int> childFolderIndex(children.size(), -1);
    for (size_t i = 0; i < children.size(); i++) {
        const auto& child = children[i];
        if (!child || winrt::get_class_name(child) !=
                          L"Taskbar.TaskListButton") {
            continue;
        }
        int folderIndex = FolderIndexForTaskListButton(child);
        childFolderIndex[i] = folderIndex;
        if (folderIndex >= 0) {
            folderIndexCounts[folderIndex]++;
        }
    }

    HWND taskbarWnd = host->hwnd;
    int matched = 0;
    for (size_t i = 0; i < children.size(); i++) {
        const auto& child = children[i];
        int folderIndex = childFolderIndex[i];
        if (folderIndex < 0) {
            continue;
        }
        if (folderIndexCounts[folderIndex] > 1) {
            Wh_Log(L"Pins: '%s' matches more than one realized taskbar "
                   L"button; leaving all of them unbound",
                   AutomationNameOf(child).c_str());
            continue;
        }

        // Explorer's own tooltip echoes the pinned label, which fights the
        // grid we open on hover with a second, redundant popup naming the
        // same folder. Suppress it for buttons we've claimed, saving the
        // shell's value so UnbindPinButtons can restore it later.
        auto savedTooltip = child.ReadLocalValue(ToolTipService::ToolTipProperty());
        ToolTipService::SetToolTip(child, nullptr);

        PinBinding binding;
        binding.button = child;
        binding.folderIndex = folderIndex;
        binding.savedTooltip = savedTooltip;
        binding.enterToken = child.PointerEntered(
            [folderIndex, taskbarWnd](
                winrt::Windows::Foundation::IInspectable const& sender,
                winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
                Wh_Log(L"Pins: pointer entered folder button %d", folderIndex);
                if (auto element = sender.try_as<FrameworkElement>()) {
                    OnPointerEnteredButton(folderIndex, element, taskbarWnd);
                }
            });
        binding.exitToken = child.PointerExited(
            [](winrt::Windows::Foundation::IInspectable const&,
               winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const&) {
                OnPointerExitedButton();
            });
        host->pinBindings.push_back(std::move(binding));
        matched++;
    }

    size_t knownLabels = 0;
    {
        std::lock_guard<std::mutex> lock(Pins::g_labelMutex);
        knownLabels = Pins::g_labelToPinId.size();
    }

    // Logged on every real rebind pass, not only when the count changes: a
    // rebind that keeps failing is exactly the case worth seeing, and this only
    // runs when the container set or the label map actually changed.
    Wh_Log(L"Pins: hover bound to %d real taskbar button(s) of %zu realized, "
           L"%zu label(s) known",
           matched, children.size(), knownLabels);

    // Say what was on the taskbar and what was being looked for — but only once
    // the label map exists. Before the first reconcile finishes there is
    // nothing to match against and the dump would be pure noise.
    if (matched == 0 && knownLabels > 0) {
        for (const auto& child : children) {
            if (child &&
                winrt::get_class_name(child) == L"Taskbar.TaskListButton") {
                Wh_Log(L"  saw button named '%s'",
                       AutomationNameOf(child).c_str());
            }
        }
        std::lock_guard<std::mutex> lock(Pins::g_labelMutex);
        for (const auto& [leaf, pinId] : Pins::g_labelToPinId) {
            Wh_Log(L"  wanted '%s' (pin %s)", leaf.c_str(), pinId.c_str());
        }
    }
    host->lastPinBindCount = matched;
}

// Leaving the button cancels a pending delayed open. Closing an already-open
// cascade is left to OnTick so moving up into the grid does not dismiss it.
void OnPointerExitedButton() {
    if (g_pendingRootPath.empty()) {
        return;
    }
    g_pendingRootPath.clear();
    g_pendingRootTitle.clear();
    if (!g_levelWindows.empty() && g_levelWindows[0]) {
        KillTimer(g_levelWindows[0], kOpenTimerId);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Folder manager window
//
// The only place folder buttons are configured. Windhawk's settings page
// cannot hold the list: a mod cannot write to its own settings without
// administrator rights, so a list living there could never be edited by the
// Explorer right-click pin, by an unpin on the taskbar, or from here.
// A plain Win32 window on its own thread, touching only the mod's own
// storage, needs no elevation at all.
namespace FolderManager {

constexpr PCWSTR kClassName = L"WH_TaskbarFolderHoverTray_Manager";
constexpr PCWSTR kEditClassName = L"WH_TaskbarFolderHoverTray_ManagerEdit";

constexpr int kIdList = 1001;
constexpr int kIdAdd = 1002;
constexpr int kIdEdit = 1003;
constexpr int kIdRemove = 1004;
constexpr int kIdTogglePin = 1005;
constexpr int kIdClose = 1008;
constexpr int kIdHint = 1009;
constexpr int kIdRemoveAll = 1010;

// Edit-dialog controls.
constexpr int kIdEditName = 1101;
constexpr int kIdEditPath = 1102;
constexpr int kIdEditIcon = 1103;
constexpr int kIdEditPinned = 1104;
constexpr int kIdEditBrowsePath = 1105;
constexpr int kIdEditBrowseIcon = 1106;
constexpr int kIdEditOk = 1107;
constexpr int kIdEditCancel = 1108;
constexpr int kIdEditPreview = 1109;
constexpr int kIdEditPinnedHint = 1110;
// Static labels carry ids too, so WM_DPICHANGED can lay them out from the same
// table WM_CREATE built them from.
constexpr int kIdEditNameLabel = 1111;
constexpr int kIdEditPathLabel = 1112;
constexpr int kIdEditIconLabel = 1113;
constexpr int kIdEditIconHint = 1114;
constexpr int kIdEditOpenPath = 1115;

// Every dimension in this window is written at 96 DPI and goes through
// Scale()/ScaleFor(). explorer.exe is manifested Per-Monitor-DPI-Aware V2 and
// threads it creates inherit that context, so plain Win32 windows here get no
// automatic scaling at all: without this a 150% display would show a small
// window wrapped around a large system font.
constexpr int kRowHeight = 40;
constexpr int kRowIconSize = 24;
constexpr int kIconLeft = 8;      // Row icon inset from the row's left edge.
constexpr int kTextGap = 10;      // Icon-to-text gap.
constexpr int kTitleTop = 4;      // Row title band, measured from the row top.
constexpr int kTitleBottom = 21;  // Also the top of the path band.
constexpr int kPathBottom = 3;    // Path band inset from the row bottom.
constexpr int kPreviewIcon = 32;  // Edit-dialog icon preview.
constexpr int kPreviewInset = 4;
// Section caption band, drawn on top of the row that opens a section rather
// than as a list item of its own — a header that is not an item cannot be
// selected, arrowed onto, or counted in an index.
constexpr int kSectionHeader = 24;

// 96-DPI client size of each window, before AdjustWindowRect.
constexpr int kMainWidth = 476;
constexpr int kMainHeight = 434;
constexpr int kEditWidth = 470;
constexpr int kEditHeight = 306;

int Scale(int value, int dpi) {
    return MulDiv(value, dpi, 96);
}

// The DPI of the monitor a window is on. Read per use rather than cached in a
// global, so everything stays correct after the window is dragged to another
// monitor; GetDpiForWindow is a cheap cached window property.
int DpiOf(HWND hWnd) {
    UINT dpi = hWnd ? GetDpiForWindow(hWnd) : 0;
    return dpi ? (int)dpi : 96;
}

// Where a child sits at 96 DPI. One table per window, so WM_CREATE and
// WM_DPICHANGED lay out from the same numbers.
struct ChildRect {
    int id;
    int x;
    int y;
    int cx;
    int cy;
};

void LayoutChildren(HWND hWnd, const ChildRect* items, size_t count, int dpi) {
    for (size_t i = 0; i < count; i++) {
        HWND child = GetDlgItem(hWnd, items[i].id);
        if (child) {
            MoveWindow(child, Scale(items[i].x, dpi), Scale(items[i].y, dpi),
                       Scale(items[i].cx, dpi), Scale(items[i].cy, dpi), TRUE);
        }
    }
}

// Owned by the manager thread once it starts; only ever read elsewhere to
// find an existing window or ask it to close.
std::atomic<HWND> g_wnd{nullptr};
std::atomic<bool> g_active{false};

// 96-DPI layout of the main window's children.
constexpr ChildRect kMainLayout[] = {
    {kIdList, 12, 12, 452, 300},      {kIdAdd, 12, 324, 76, 26},
    {kIdEdit, 94, 324, 76, 26},       {kIdRemove, 176, 324, 76, 26},
    {kIdTogglePin, 258, 324, 76, 26}, {kIdClose, 388, 324, 76, 26},
    {kIdHint, 12, 360, 452, 32},
    {kIdRemoveAll, 12, 396, 200, 26},
};

// The store as last read, in listbox order. Manager-thread only.
std::vector<FolderStore::Entry> g_rows;
// One icon per row, parallel to g_rows, owned here and freed on rebuild.
std::vector<HICON> g_rowIcons;

// Defined with the rest of the row commands, below; PopulateList needs it.
void UpdatePinButton(HWND hWnd);

// True if this row opens a section. g_rows is sorted pinned-first, so the
// boundary is wherever the flag changes — plus row 0, which opens the first
// section whichever kind it is.
bool StartsSection(size_t index) {
    if (index >= g_rows.size()) {
        return false;
    }
    return index == 0 || g_rows[index].pinned != g_rows[index - 1].pinned;
}

PCWSTR SectionCaption(bool pinned) {
    return pinned ? L"On the taskbar" : L"Not pinned";
}

////////////////////////////////////////////////////////////////////////////////
// Dark mode
//
// Plain Win32 controls have no dark mode of their own. Three separate things
// have to agree: DWM paints the caption, uxtheme paints the controls, and the
// window paints its own background and owner-drawn rows. Miss any one and the
// window comes out half-light.
//
// The theme is read from the same UISettings cache the hover grid uses, so the
// manager follows the system app theme without a setting of its own.

bool Dark() {
    return IsDarkTheme();
}

COLORREF ClrWindow() {
    return Dark() ? RGB(32, 32, 32) : GetSysColor(COLOR_WINDOW);
}
COLORREF ClrWindowText() {
    return Dark() ? RGB(255, 255, 255) : GetSysColor(COLOR_WINDOWTEXT);
}
COLORREF ClrGrayText() {
    return Dark() ? RGB(155, 155, 155) : GetSysColor(COLOR_GRAYTEXT);
}
COLORREF ClrFace() {
    return Dark() ? RGB(43, 43, 43) : GetSysColor(COLOR_BTNFACE);
}
COLORREF ClrHighlight() {
    return Dark() ? RGB(0, 90, 158) : GetSysColor(COLOR_HIGHLIGHT);
}
COLORREF ClrHighlightText() {
    return Dark() ? RGB(255, 255, 255) : GetSysColor(COLOR_HIGHLIGHTTEXT);
}
COLORREF ClrSeparator() {
    return Dark() ? RGB(64, 64, 64) : RGB(200, 200, 200);
}
COLORREF ClrFieldBorder() {
    return Dark() ? RGB(72, 72, 72) : GetSysColor(COLOR_3DSHADOW);
}

// WS_EX_CLIENTEDGE is the classic sunken 3D border, drawn in the non-client
// area from COLOR_3DHIGHLIGHT and friends. Those are light in both themes and
// nothing in uxtheme repaints them, which is why the edit boxes and the list
// kept a white ring on a dark window. In dark mode the style is dropped and the
// border is painted by the parent instead — see DrawFieldBorders.
DWORD FieldExStyle() {
    return Dark() ? 0 : WS_EX_CLIENTEDGE;
}

// The 1px ring around each borderless field. Drawn by the parent because the
// pixels sit just outside the child, in the parent's own client area — which
// also means no subclassing and no WM_NCPAINT.
void DrawFieldBorders(HWND hWnd, HDC dc, const int* ids, size_t count) {
    HBRUSH brush = CreateSolidBrush(ClrFieldBorder());
    if (!brush) {
        return;
    }
    for (size_t i = 0; i < count; i++) {
        HWND child = GetDlgItem(hWnd, ids[i]);
        if (!child) {
            continue;
        }
        RECT rc{};
        GetWindowRect(child, &rc);
        MapWindowPoints(nullptr, hWnd, (LPPOINT)&rc, 2);
        InflateRect(&rc, 1, 1);
        FrameRect(dc, &rc, brush);
    }
    DeleteObject(brush);
}

// Background brushes handed back from WM_CTLCOLOR*, which is called on every
// paint — so they are cached rather than created per message. Freed in
// Wh_ModUninit alongside g_fontCache; a brush left behind would leak into
// Explorer on every disable/enable cycle.
HBRUSH g_windowBrush = nullptr;
HBRUSH g_faceBrush = nullptr;

HBRUSH WindowBrush() {
    if (!g_windowBrush) {
        g_windowBrush = CreateSolidBrush(ClrWindow());
    }
    return g_windowBrush;
}

HBRUSH FaceBrush() {
    if (!g_faceBrush) {
        g_faceBrush = CreateSolidBrush(ClrFace());
    }
    return g_faceBrush;
}

// Dark rendering is per window and has to be asked for before the control
// paints. AllowDarkModeForWindow is uxtheme ordinal 133 — undocumented, and the
// same call every dark-mode Win32 app makes.
//
// ponytail: if a future build drops or renumbers the ordinal, GetProcAddress
// returns null and the controls simply stay light. Nothing else breaks, so this
// is not worth a version check.
void AllowDarkModeForWindow(HWND hWnd, bool allow) {
    using Fn = BOOL(WINAPI*)(HWND, BOOL);
    static Fn fn = []() -> Fn {
        HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
        return uxtheme ? (Fn)GetProcAddress(uxtheme, MAKEINTRESOURCEA(133))
                       : nullptr;
    }();
    if (fn) {
        fn(hWnd, allow);
    }
}

// Applied to the window and every child it already has, so it runs at the end
// of WM_CREATE rather than before the controls exist.
void ApplyTheme(HWND hWnd) {
    BOOL dark = Dark();
    AllowDarkModeForWindow(hWnd, dark != FALSE);
    // DWMWA_USE_IMMERSIVE_DARK_MODE. Named rather than the literal 20 so the
    // intent survives; the attribute is documented from Windows 11 on.
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
                          sizeof(dark));
    EnumChildWindows(
        hWnd,
        [](HWND child, LPARAM darkParam) -> BOOL {
            BOOL childDark = (BOOL)darkParam;
            AllowDarkModeForWindow(child, childDark != FALSE);
            SetWindowTheme(child, childDark ? L"DarkMode_Explorer" : nullptr,
                           nullptr);
            return TRUE;
        },
        (LPARAM)dark);
}

// The one WM_CTLCOLOR* answer every control here wants: our text colours on our
// background, and the matching brush so the control fills with it. `onFace` is
// for the ones sitting on the window itself (labels, buttons) rather than in a
// white field (edits, the listbox).
LRESULT ThemedCtlColor(HDC dc, bool onFace) {
    SetTextColor(dc, ClrWindowText());
    SetBkColor(dc, onFace ? ClrFace() : ClrWindow());
    return (LRESULT)(onFace ? FaceBrush() : WindowBrush());
}

// Registers one of the manager's window classes against the current DLL.
// RegisterModClass handles a stale registration left by a previous load.
bool EnsureClass(PCWSTR name, WNDPROC proc) {
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = proc;
    wc.hInstance = GetCurrentModuleHandle();
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = name;
    return RegisterModClass(wc);
}

// Top-left corner that centres a width x height window on the monitor
// `anchor` falls on, clamped to that monitor's work area so the title bar can
// never land under a taskbar.
POINT CenterOnMonitor(POINT anchor, int width, int height) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    HMONITOR monitor = MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST);
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return POINT{CW_USEDEFAULT, CW_USEDEFAULT};
    }
    const RECT& work = info.rcWork;
    POINT pos{work.left + ((work.right - work.left) - width) / 2,
              work.top + ((work.bottom - work.top) - height) / 2};
    if (pos.x < work.left) {
        pos.x = work.left;
    }
    if (pos.y < work.top) {
        pos.y = work.top;
    }
    return pos;
}

// The message font at a given DPI. SystemParametersInfoW would hand back
// metrics sized for the *system* DPI, which is what made a scaled window show
// an oversized font. Cached per DPI so moving between monitors does not leak
// an HFONT per WM_DPICHANGED; the manager only ever sees a couple of values.
// Freed in Wh_ModUninit — the vector's own destructor is a plain heap free, so
// it needs no no_destroy, but the handles it holds would leak into Explorer
// across every disable/enable cycle.
std::vector<std::pair<int, HFONT>> g_fontCache;

HFONT UiFont(int dpi) {
    for (const auto& cached : g_fontCache) {
        if (cached.first == dpi) {
            return cached.second;
        }
    }
    HFONT font = nullptr;
    NONCLIENTMETRICSW ncm{};
    ncm.cbSize = sizeof(ncm);
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm,
                                   0, (UINT)dpi)) {
        font = CreateFontIndirectW(&ncm.lfMessageFont);
    }
    if (!font) {
        font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    }
    g_fontCache.emplace_back(dpi, font);
    return font;
}

void ApplyUiFont(HWND parent, int dpi) {
    EnumChildWindows(
        parent,
        [](HWND child, LPARAM f) -> BOOL {
            SendMessageW(child, WM_SETFONT, (WPARAM)f, TRUE);
            return TRUE;
        },
        (LPARAM)UiFont(dpi));
}

// A single-line edit control centers its text within whatever height it is
// given, but the fixed row heights in the layout tables are taller than the
// UI font needs — the extra room lands below the text rather than split
// evenly, since the control's own centering math is anchored to its internal
// leading. Shrinking the control to the font's real line height and
// recentering it in its original slot removes that slack instead of fighting
// the control's own centering.
void CenterSingleLineFields(HWND hWnd, int dpi, const int* ids, size_t count) {
    HDC dc = GetDC(hWnd);
    HFONT old = (HFONT)SelectObject(dc, UiFont(dpi));
    TEXTMETRICW tm{};
    GetTextMetricsW(dc, &tm);
    SelectObject(dc, old);
    ReleaseDC(hWnd, dc);

    int desired = tm.tmHeight + Scale(6, dpi);
    for (size_t i = 0; i < count; i++) {
        HWND field = GetDlgItem(hWnd, ids[i]);
        if (!field) {
            continue;
        }
        RECT rc{};
        GetWindowRect(field, &rc);
        MapWindowPoints(nullptr, hWnd, (LPPOINT)&rc, 2);
        int slot = rc.bottom - rc.top;
        int height = std::min(desired, slot);
        int top = rc.top + (slot - height) / 2;
        MoveWindow(field, rc.left, top, rc.right - rc.left, height, TRUE);
        // A single-line edit only recomputes where it vertically centers
        // its text in response to WM_SETFONT, not to a plain resize — so
        // without resending it here the control keeps centering against the
        // slot's original, taller height.
        SendMessageW(field, WM_SETFONT, (WPARAM)UiFont(dpi), TRUE);
    }
}

// The icon a row should show: its configured icon spec if it has one that is
// a file/resource, otherwise the folder's own shell icon. Emoji icons have no
// HICON — those are drawn as text instead, so this returns null for them.
HICON IconForEntry(const FolderStore::Entry& entry, int pixelSize) {
    std::wstring icon = entry.icon;
    if (!icon.empty()) {
        if (!IconSettingIsFile(icon)) {
            return nullptr;  // Emoji or other literal text.
        }
        if (!IsLikelyRemotePath(IconSpecFilePart(icon))) {
            if (HICON hIcon = ExtractIconFromResourceSpec(icon, pixelSize)) {
                return hIcon;
            }
        }
    }
    std::wstring target = ResolveFolderPath(ExpandEnv(entry.path));
    if (target.empty()) {
        target = ExpandEnv(entry.path);
    }
    // SHGetFileInfoW on an offline share blocks for the network timeout, and a
    // manager thread stuck in there cannot process the WM_CLOSE CloseAndWait
    // posts — so uninit would hang with it. DrawRow copes with a null icon.
    if (IsLikelyRemotePath(target)) {
        return nullptr;
    }
    return GetShellIconForPath(target, pixelSize);
}

void FreeRowIcons() {
    for (HICON icon : g_rowIcons) {
        if (icon) {
            DestroyIcon(icon);
        }
    }
    g_rowIcons.clear();
}

void PopulateList(HWND hWnd) {
    HWND list = GetDlgItem(hWnd, kIdList);
    if (!list) {
        return;
    }
    int dpi = DpiOf(hWnd);
    // Kept by path rather than by index: the rows are sorted by name, so an
    // edit that renames a folder — or a pin toggle, once the row order is no
    // longer the store's — can move the selected row somewhere else.
    int previous = (int)SendMessageW(list, LB_GETCURSEL, 0, 0);
    std::wstring previousPath;
    if (previous >= 0 && previous < (int)g_rows.size()) {
        previousPath = g_rows[previous].path;
    }

    SendMessageW(list, LB_RESETCONTENT, 0, 0);
    FreeRowIcons();
    g_rows = FolderStore::Read();

    // The taskbar owns button order now — it is set by dragging the buttons
    // themselves — so store order says nothing a reader of this list would want.
    // Pinned first so the two sections are contiguous, then by name within each
    // so a folder is findable. StrCmpLogicalW rather than a plain compare, so
    // "Folder 2" lands before "Folder 10" the way Explorer sorts.
    std::sort(g_rows.begin(), g_rows.end(),
              [](const FolderStore::Entry& a, const FolderStore::Entry& b) {
                  if (a.pinned != b.pinned) {
                      return a.pinned;
                  }
                  const std::wstring& an = a.name.empty() ? a.path : a.name;
                  const std::wstring& bn = b.name.empty() ? b.path : b.name;
                  return StrCmpLogicalW(an.c_str(), bn.c_str()) < 0;
              });

    for (const auto& entry : g_rows) {
        g_rowIcons.push_back(IconForEntry(entry, Scale(kRowIconSize, dpi)));
        // Text comes from g_rows during WM_DRAWITEM; the item itself only
        // needs to exist.
        SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)L"");
    }

    if (!g_rows.empty()) {
        int selected = -1;
        if (!previousPath.empty()) {
            for (size_t i = 0; i < g_rows.size(); i++) {
                if (FolderStore::SamePath(g_rows[i].path, previousPath)) {
                    selected = (int)i;
                    break;
                }
            }
        }
        // Gone (removed, or its path was edited): fall back to wherever the
        // selection was, clamped. Only when something was selected before -
        // a deliberate click into empty space (previous < 0) stays cleared
        // rather than snapping back to row 0.
        if (selected < 0 && previous >= 0) {
            int last = (int)g_rows.size() - 1;
            selected = previous < last ? previous : last;
        }
        if (selected >= 0) {
            SendMessageW(list, LB_SETCURSEL, selected, 0);
        }
    }

    bool any = !g_rows.empty();
    UpdatePinButton(hWnd);
    SetWindowTextW(GetDlgItem(hWnd, kIdHint),
                   any ? L"Drag the buttons on the taskbar to reorder them. "
                         L"Double-click a row to edit it."
                       : L"No folders yet. Add one here, or right-click any "
                         L"folder in Explorer and pick Taskbar Folders > Pin.");
    InvalidateRect(list, nullptr, TRUE);
}

int SelectedIndex(HWND hWnd) {
    int sel = (int)SendMessageW(GetDlgItem(hWnd, kIdList), LB_GETCURSEL, 0, 0);
    return (sel >= 0 && sel < (int)g_rows.size()) ? sel : -1;
}

// Refreshes the taskbar and this window after the store was changed.
//
// The taskbar half is deferred to the taskbar UI thread (see RequestReloadUI),
// so this returns without blocking the manager thread on two worker joins. The
// window's own list is repopulated here and now, from the store this thread
// just wrote; the reload posts a WM_APP back afterwards, which repopulates
// again from the same store and keeps the selected index either way.
void RefreshAfterStoreChange(HWND hWnd) {
    if (!g_unloading) {
        RequestReloadUI();
    }
    PopulateList(hWnd);
}

void DrawRow(LPDRAWITEMSTRUCT dis) {
    if (dis->itemID == (UINT)-1 || dis->itemID >= g_rows.size()) {
        return;
    }
    const FolderStore::Entry& entry = g_rows[dis->itemID];
    bool selected = (dis->itemState & ODS_SELECTED) != 0;
    int dpi = DpiOf(dis->hwndItem);
    int iconSize = Scale(kRowIconSize, dpi);

    HDC dc = dis->hDC;
    RECT rc = dis->rcItem;

    // A section-opening row is measured one caption band taller than the rest.
    // The caption is painted first, on the window background, and then the row
    // proper is drawn in what is left — so the selection highlight stops at the
    // caption instead of swallowing it.
    if (StartsSection(dis->itemID)) {
        RECT band{rc.left, rc.top, rc.right, rc.top + Scale(kSectionHeader, dpi)};
        HBRUSH bg = CreateSolidBrush(ClrWindow());
        FillRect(dc, &band, bg);
        DeleteObject(bg);

        RECT text{band.left + Scale(kIconLeft, dpi), band.top, band.right,
                  band.bottom};
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, ClrGrayText());
        DrawTextW(dc, SectionCaption(entry.pinned), -1, &text,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        // Hairline under the caption, so the two groups read as separate lists
        // rather than one list with a label floating in it.
        RECT rule{band.left + Scale(kIconLeft, dpi), band.bottom - 1,
                  band.right - Scale(kIconLeft, dpi), band.bottom};
        HBRUSH line = CreateSolidBrush(ClrSeparator());
        FillRect(dc, &rule, line);
        DeleteObject(line);

        rc.top = band.bottom;
    }

    HBRUSH rowBg = CreateSolidBrush(selected ? ClrHighlight() : ClrWindow());
    FillRect(dc, &rc, rowBg);
    DeleteObject(rowBg);

    int iconLeft = rc.left + Scale(kIconLeft, dpi);

    int iconTop = rc.top + (Scale(kRowHeight, dpi) - iconSize) / 2;
    HICON icon = dis->itemID < g_rowIcons.size() ? g_rowIcons[dis->itemID]
                                                 : nullptr;
    if (icon) {
        DrawIconEx(dc, iconLeft, iconTop, icon, iconSize, iconSize, 0, nullptr,
                   DI_NORMAL);
    } else if (!entry.icon.empty()) {
        // Emoji icon: draw the text itself, centred in the icon slot.
        RECT iconRect{iconLeft, iconTop, iconLeft + iconSize,
                      iconTop + iconSize};
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, selected ? ClrHighlightText() : ClrWindowText());
        DrawTextW(dc, entry.icon.c_str(), -1, &iconRect,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    }

    int textLeft = iconLeft + iconSize + Scale(kTextGap, dpi);
    int textRight = rc.right - Scale(kIconLeft, dpi);
    int bandSplit = rc.top + Scale(kTitleBottom, dpi);
    SetBkMode(dc, TRANSPARENT);

    // No "(not pinned)" suffix any more — the section the row sits in says it.
    std::wstring title = entry.name.empty() ? entry.path : entry.name;
    RECT titleRect{textLeft, rc.top + Scale(kTitleTop, dpi), textRight,
                   bandSplit};
    SetTextColor(dc, selected ? ClrHighlightText() : ClrWindowText());
    DrawTextW(dc, title.c_str(), -1, &titleRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                  DT_NOPREFIX);

    RECT pathRect{textLeft, bandSplit, textRight,
                  rc.bottom - Scale(kPathBottom, dpi)};
    SetTextColor(dc, selected ? ClrHighlightText() : ClrGrayText());
    DrawTextW(dc, entry.path.c_str(), -1, &pathRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS |
                  DT_NOPREFIX);

    // The listbox keeps a caret/focus index even after LB_SETCURSEL(-1)
    // clears the selection, so ODS_FOCUS alone would leave a focus rect
    // behind on a deliberately deselected row. Tying it to ODS_SELECTED too
    // means the rect only ever appears on the actually-selected row.
    if ((dis->itemState & ODS_FOCUS) && selected) {
        DrawFocusRect(dc, &rc);
    }
}

// Shell folder picker, seeded with `initial`. Empty return means cancelled.
std::wstring BrowseForFolder(HWND owner, const std::wstring& initial) {
    std::wstring chosen;
    IFileDialog* dialog = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC,
                                IID_PPV_ARGS(&dialog)))) {
        return chosen;
    }

    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);

    if (!initial.empty()) {
        IShellItem* item = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(initial.c_str(), nullptr,
                                                  IID_PPV_ARGS(&item)))) {
            dialog->SetFolder(item);
            item->Release();
        }
    }

    if (SUCCEEDED(dialog->Show(owner))) {
        IShellItem* result = nullptr;
        if (SUCCEEDED(dialog->GetResult(&result))) {
            PWSTR path = nullptr;
            if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                chosen = path;
                CoTaskMemFree(path);
            }
            result->Release();
        }
    }
    dialog->Release();
    return chosen;
}

std::wstring BrowseForIconFile(HWND owner) {
    std::wstring chosen;
    IFileDialog* dialog = nullptr;
    if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC,
                                IID_PPV_ARGS(&dialog)))) {
        return chosen;
    }

    COMDLG_FILTERSPEC filters[] = {
        {L"Icons and images", L"*.ico;*.png;*.jpg;*.jpeg;*.bmp"},
        {L"Programs and libraries", L"*.exe;*.dll"},
        {L"All files", L"*.*"}};
    dialog->SetFileTypes(ARRAYSIZE(filters), filters);

    DWORD options = 0;
    dialog->GetOptions(&options);
    dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);

    if (SUCCEEDED(dialog->Show(owner))) {
        IShellItem* result = nullptr;
        if (SUCCEEDED(dialog->GetResult(&result))) {
            PWSTR path = nullptr;
            if (SUCCEEDED(result->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                chosen = path;
                CoTaskMemFree(path);
            }
            result->Release();
        }
    }
    dialog->Release();
    return chosen;
}

////////////////////////////////////////////////////////////////////////////////
// Edit dialog

// Passed to the edit window and written back in place when it is accepted.
struct EditContext {
    FolderStore::Entry entry;
    // Path of the entry being edited, empty when adding — so a collision with
    // the entry's own row is not treated as a duplicate. Matched by path
    // rather than by a store index, because the store can change on another
    // thread while this dialog is up.
    std::wstring originalPath;
    bool accepted = false;
    HICON preview = nullptr;
};

// 96-DPI layout of the edit dialog. Every control has an id so WM_DPICHANGED
// can re-run the same table.
constexpr ChildRect kEditLayout[] = {
    {kIdEditNameLabel, 14, 14, 300, 16},
    {kIdEditName, 14, 32, 442, 24},
    {kIdEditPathLabel, 14, 62, 300, 16},
    {kIdEditPath, 14, 80, 286, 24},
    {kIdEditBrowsePath, 308, 80, 70, 24},
    {kIdEditOpenPath, 386, 80, 70, 24},
    {kIdEditIconLabel, 14, 110, 300, 16},
    {kIdEditIcon, 14, 128, 286, 24},
    {kIdEditBrowseIcon, 308, 128, 70, 24},
    {kIdEditIconHint, 14, 158, 442, 32},
    {kIdEditPinned, 14, 198, 340, 22},
    {kIdEditPinnedHint, 32, 222, 378, 32},
    {kIdEditPreview, 416, 196, 40, 40},
    {kIdEditOk, 278, 266, 86, 26},
    {kIdEditCancel, 370, 266, 86, 26},
};

std::wstring GetControlText(HWND hWnd, int id) {
    HWND control = GetDlgItem(hWnd, id);
    int length = GetWindowTextLengthW(control);
    std::wstring text(length + 1, L'\0');
    GetWindowTextW(control, text.data(), length + 1);
    text.resize(length);
    return Trim(text);
}

void RefreshPreview(HWND hWnd, EditContext* ctx) {
    FolderStore::Entry probe;
    probe.path = GetControlText(hWnd, kIdEditPath);
    probe.icon = GetControlText(hWnd, kIdEditIcon);
    if (ctx->preview) {
        DestroyIcon(ctx->preview);
    }
    ctx->preview = probe.path.empty()
                       ? nullptr
                       : IconForEntry(probe, Scale(kPreviewIcon, DpiOf(hWnd)));
    InvalidateRect(GetDlgItem(hWnd, kIdEditPreview), nullptr, TRUE);
}

LRESULT CALLBACK EditWndProc(HWND hWnd, UINT msg, WPARAM wParam,
                             LPARAM lParam) {
    auto* ctx = (EditContext*)GetWindowLongPtrW(hWnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CREATE: {
            auto* create = (CREATESTRUCTW*)lParam;
            ctx = (EditContext*)create->lpCreateParams;
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)ctx);

            HINSTANCE instance = GetCurrentModuleHandle();
            // Created at 0,0 and positioned by LayoutChildren below, so the
            // creation path and WM_DPICHANGED share one set of numbers.
            struct Spec {
                PCWSTR cls;
                PCWSTR text;
                DWORD style;
                DWORD exStyle;
                int id;
            };
            const Spec specs[] = {
                {L"STATIC", L"Name", SS_NOTIFY, 0, kIdEditNameLabel},
                {L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, FieldExStyle(),
                 kIdEditName},
                {L"STATIC", L"Folder", SS_NOTIFY, 0, kIdEditPathLabel},
                {L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, FieldExStyle(),
                 kIdEditPath},
                {L"BUTTON", L"Browse...", WS_TABSTOP | BS_PUSHBUTTON, 0,
                 kIdEditBrowsePath},
                {L"BUTTON", L"Open", WS_TABSTOP | BS_PUSHBUTTON, 0,
                 kIdEditOpenPath},
                {L"STATIC", L"Icon", SS_NOTIFY, 0, kIdEditIconLabel},
                {L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, FieldExStyle(),
                 kIdEditIcon},
                {L"BUTTON", L"Browse...", WS_TABSTOP | BS_PUSHBUTTON, 0,
                 kIdEditBrowseIcon},
                {L"STATIC",
                 L"Emoji, an .ico / .png file, or a resource such as "
                 L"C:\\Windows\\explorer.exe,0. Leave empty to use the "
                 L"folder's own icon.",
                 SS_NOTIFY, 0, kIdEditIconHint},
                // Pin state lives here rather than as a list button, so that
                // what "unpinned" actually means — kept, not deleted — is
                // spelled out right next to the switch.
                {L"BUTTON", L"Pinned to the taskbar",
                 WS_TABSTOP | BS_AUTOCHECKBOX, 0, kIdEditPinned},
                {L"STATIC",
                 L"Unticked keeps this folder in the list but takes its button "
                 L"off the taskbar. Nothing is deleted, and the folder still "
                 L"cannot be added twice.",
                 SS_NOTIFY, 0, kIdEditPinnedHint},
                {L"STATIC", L"", SS_OWNERDRAW, 0, kIdEditPreview},
                {L"BUTTON", L"Save", WS_TABSTOP | BS_DEFPUSHBUTTON, 0,
                 kIdEditOk},
                {L"BUTTON", L"Cancel", WS_TABSTOP | BS_PUSHBUTTON, 0,
                 kIdEditCancel},
            };
            for (const auto& s : specs) {
                CreateWindowExW(s.exStyle, s.cls, s.text,
                                WS_CHILD | WS_VISIBLE | s.style, 0, 0, 0, 0,
                                hWnd, (HMENU)(INT_PTR)s.id, instance, nullptr);
            }
            LayoutChildren(hWnd, kEditLayout, ARRAYSIZE(kEditLayout),
                           DpiOf(hWnd));

            SetDlgItemTextW(hWnd, kIdEditName, ctx->entry.name.c_str());
            SetDlgItemTextW(hWnd, kIdEditPath, ctx->entry.path.c_str());
            SetDlgItemTextW(hWnd, kIdEditIcon, ctx->entry.icon.c_str());
            CheckDlgButton(hWnd, kIdEditPinned,
                           ctx->entry.pinned ? BST_CHECKED : BST_UNCHECKED);
            ApplyUiFont(hWnd, DpiOf(hWnd));
            static constexpr int kSingleLineFields[] = {kIdEditName,
                                                         kIdEditPath,
                                                         kIdEditIcon};
            CenterSingleLineFields(hWnd, DpiOf(hWnd), kSingleLineFields,
                                   ARRAYSIZE(kSingleLineFields));
            ApplyTheme(hWnd);
            RefreshPreview(hWnd, ctx);
            return 0;
        }

        // Dragged to a monitor at another scale: take the frame Windows
        // suggests, then re-run the layout, the font and the preview icon at
        // the new DPI.
        case WM_DPICHANGED: {
            const RECT* suggested = (const RECT*)lParam;
            SetWindowPos(hWnd, nullptr, suggested->left, suggested->top,
                         suggested->right - suggested->left,
                         suggested->bottom - suggested->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            int dpi = LOWORD(wParam);
            LayoutChildren(hWnd, kEditLayout, ARRAYSIZE(kEditLayout), dpi);
            ApplyUiFont(hWnd, dpi);
            static constexpr int kSingleLineFields[] = {kIdEditName,
                                                         kIdEditPath,
                                                         kIdEditIcon};
            CenterSingleLineFields(hWnd, dpi, kSingleLineFields,
                                   ARRAYSIZE(kSingleLineFields));
            // The field borders are painted around where the children were, so
            // moving them leaves the old rings behind.
            InvalidateRect(hWnd, nullptr, TRUE);
            if (ctx) {
                RefreshPreview(hWnd, ctx);
            }
            return 0;
        }

        case WM_DRAWITEM: {
            auto* dis = (LPDRAWITEMSTRUCT)lParam;
            if (dis->CtlID == kIdEditPreview) {
                FillRect(dis->hDC, &dis->rcItem, FaceBrush());
                if (ctx && ctx->preview) {
                    int dpi = DpiOf(hWnd);
                    int inset = Scale(kPreviewInset, dpi);
                    int size = Scale(kPreviewIcon, dpi);
                    DrawIconEx(dis->hDC, dis->rcItem.left + inset,
                               dis->rcItem.top + inset, ctx->preview, size,
                               size, 0, nullptr, DI_NORMAL);
                } else if (ctx) {
                    std::wstring icon = GetControlText(hWnd, kIdEditIcon);
                    if (!icon.empty()) {
                        SetBkMode(dis->hDC, TRANSPARENT);
                        SetTextColor(dis->hDC, ClrWindowText());
                        DrawTextW(dis->hDC, icon.c_str(), -1, &dis->rcItem,
                                  DT_CENTER | DT_VCENTER | DT_SINGLELINE |
                                      DT_NOPREFIX);
                    }
                }
                return TRUE;
            }
            break;
        }

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
            if (!Dark()) {
                break;
            }
            return ThemedCtlColor((HDC)wParam, true);

        case WM_CTLCOLOREDIT:
            if (!Dark()) {
                break;
            }
            return ThemedCtlColor((HDC)wParam, false);

        case WM_ERASEBKGND: {
            if (!Dark()) {
                break;
            }
            RECT client{};
            GetClientRect(hWnd, &client);
            FillRect((HDC)wParam, &client, FaceBrush());
            static constexpr int kFields[] = {kIdEditName, kIdEditPath,
                                              kIdEditIcon};
            DrawFieldBorders(hWnd, (HDC)wParam, kFields, ARRAYSIZE(kFields));
            return TRUE;
        }

        // Clicking a label, hint or the preview swatch has nothing of its own
        // to do, but it is the only way to click off a text box without
        // hitting another control — labels don't take focus on their own, so
        // this hands it to the dialog itself instead, which drops the edit
        // box's selection highlight the same as WM_LBUTTONDOWN below does for
        // the gaps between controls.
        case WM_LBUTTONDOWN: {
            SetFocus(hWnd);
            return 0;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == kIdEditNameLabel || id == kIdEditPathLabel ||
                id == kIdEditIconLabel || id == kIdEditIconHint ||
                id == kIdEditPinnedHint || id == kIdEditPreview) {
                SetFocus(hWnd);
            } else if (id == kIdEditBrowsePath) {
                std::wstring start =
                    ExpandEnv(GetControlText(hWnd, kIdEditPath));
                std::wstring picked = BrowseForFolder(hWnd, start);
                if (!picked.empty()) {
                    SetDlgItemTextW(hWnd, kIdEditPath, picked.c_str());
                    // Fill a blank name and icon from the folder itself, the
                    // same way an Explorer pin does.
                    if (GetControlText(hWnd, kIdEditName).empty()) {
                        size_t slash = picked.find_last_of(L"\\/");
                        SetDlgItemTextW(hWnd, kIdEditName,
                                        slash == std::wstring::npos
                                            ? picked.c_str()
                                            : picked.c_str() + slash + 1);
                    }
                    if (GetControlText(hWnd, kIdEditIcon).empty()) {
                        SetDlgItemTextW(hWnd, kIdEditIcon,
                                        ReadFolderCustomIcon(picked).c_str());
                    }
                    RefreshPreview(hWnd, ctx);
                }
            } else if (id == kIdEditOpenPath) {
                // Opens whatever is typed, not what was saved: the point is to
                // check a path before committing to it.
                std::wstring typed =
                    ExpandEnv(GetControlText(hWnd, kIdEditPath));
                if (!typed.empty()) {
                    std::wstring target = ResolveFolderPath(typed);
                    LaunchPath(target.empty() ? typed : target);
                }
            } else if (id == kIdEditBrowseIcon) {
                std::wstring picked = BrowseForIconFile(hWnd);
                if (!picked.empty()) {
                    SetDlgItemTextW(hWnd, kIdEditIcon, picked.c_str());
                    RefreshPreview(hWnd, ctx);
                }
            } else if ((id == kIdEditPath || id == kIdEditIcon) &&
                       HIWORD(wParam) == EN_KILLFOCUS) {
                RefreshPreview(hWnd, ctx);
            } else if (id == kIdEditOk) {
                std::wstring path = GetControlText(hWnd, kIdEditPath);
                if (path.empty()) {
                    MessageBoxW(hWnd, L"A folder path is required.",
                                L"Taskbar Folders", MB_OK | MB_ICONWARNING);
                    return 0;
                }
                // Same folder as another row would mean two buttons for one
                // folder, which is also what blocks re-pinning a draft.
                // Compared by path, not by index: the store may have been
                // changed on another thread since this dialog opened.
                auto stored = FolderStore::Read();
                int clash = FolderStore::IndexOfPath(stored, path);
                if (clash >= 0 &&
                    !FolderStore::SamePath(path, ctx->originalPath)) {
                    MessageBoxW(hWnd,
                                L"That folder is already in the list. Edit "
                                L"the existing entry instead.",
                                L"Taskbar Folders", MB_OK | MB_ICONWARNING);
                    return 0;
                }
                ctx->entry.name = GetControlText(hWnd, kIdEditName);
                ctx->entry.path = path;
                ctx->entry.icon = GetControlText(hWnd, kIdEditIcon);
                ctx->entry.pinned =
                    IsDlgButtonChecked(hWnd, kIdEditPinned) == BST_CHECKED;
                ctx->entry.fileId = GetDirFileId(ExpandEnv(path));
                ctx->accepted = true;
                DestroyWindow(hWnd);
            } else if (id == kIdEditCancel) {
                DestroyWindow(hWnd);
            }
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        // No PostQuitMessage: RunEditDialog's nested loop ends on this window
        // going away instead. A WM_QUIT here would be consumed by that loop,
        // and if the main window had been closed at the same time (which is
        // what mod teardown does) its own quit would be the one swallowed,
        // leaving the outer loop blocked in GetMessage forever.
        case WM_DESTROY:
            if (ctx && ctx->preview) {
                DestroyIcon(ctx->preview);
                ctx->preview = nullptr;
            }
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Runs the edit window modally against `owner`, using a nested message loop
// (there is no dialog resource to feed DialogBox). Returns true if saved.
bool RunEditDialog(HWND owner, EditContext* ctx, PCWSTR title) {
    HINSTANCE instance = GetCurrentModuleHandle();
    if (!EnsureClass(kEditClassName, EditWndProc)) {
        return false;
    }

    // Sized at the owner's DPI: this window opens centred over it, so that is
    // the monitor it lands on.
    int dpi = DpiOf(owner);
    RECT rect{0, 0, Scale(kEditWidth, dpi), Scale(kEditHeight, dpi)};
    AdjustWindowRectExForDpi(&rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                             FALSE, WS_EX_DLGMODALFRAME, (UINT)dpi);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Centre over the manager, then let CenterOnMonitor pull it back onto the
    // work area of whichever monitor that lands on.
    RECT ownerRect{};
    GetWindowRect(owner, &ownerRect);
    POINT ownerCenter{(ownerRect.left + ownerRect.right) / 2,
                      (ownerRect.top + ownerRect.bottom) / 2};
    POINT pos = CenterOnMonitor(ownerCenter, width, height);

    HWND hWnd = CreateWindowExW(WS_EX_DLGMODALFRAME, kEditClassName, title,
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, pos.x,
                                pos.y, width, height, owner, nullptr, instance,
                                ctx);
    if (!hWnd) {
        return false;
    }

    EnableWindow(owner, FALSE);
    ShowWindow(hWnd, SW_SHOW);
    SetFocus(GetDlgItem(hWnd, kIdEditName));

    // Ends when this window is destroyed, rather than on WM_QUIT. DestroyWindow
    // runs synchronously inside the DispatchMessageW below, so the check right
    // after it is what breaks the loop. A WM_QUIT that does arrive belongs to
    // the outer loop (mod teardown posts WM_CLOSE to every window on this
    // thread) and is put back, so the outer loop still sees it.
    MSG msg;
    while (IsWindow(hWnd)) {
        BOOL got = GetMessageW(&msg, nullptr, 0, 0);
        if (got == 0) {
            PostQuitMessage((int)msg.wParam);
            break;
        }
        if (got == -1) {
            break;
        }
        if (!IsDialogMessageW(hWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (IsWindow(owner)) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }
    return ctx->accepted;
}

// Flips the pinned flag on the selected row. Same as the tick box in the edit
// dialog, reachable in one click — and, like it, unpinning keeps the entry with
// its name and icon rather than deleting anything.
//
// Committed by path for the same reason edit and remove are: the store is shared
// with the taskbar UI thread and an Explorer window thread, so an index taken
// before the read could point at a different folder by the time it is written.
void TogglePinSelected(HWND hWnd) {
    int index = SelectedIndex(hWnd);
    if (index < 0 || g_unloading) {
        return;
    }
    std::wstring path = g_rows[index].path;

    bool gone = false;
    {
        std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
        auto stored = FolderStore::Read();
        int at = FolderStore::IndexOfPath(stored, path);
        if (at < 0) {
            gone = true;
        } else {
            stored[at].pinned = !stored[at].pinned;
            FolderStore::Write(stored);
        }
    }
    if (gone) {
        Wh_Log(L"Pin toggle: '%s' is no longer in the store", path.c_str());
        PopulateList(hWnd);
        return;
    }
    RefreshAfterStoreChange(hWnd);
}

// "Pin" or "Unpin", whichever the selected row is not. Disabled with nothing
// selected, so the label it happens to be showing then does not matter.
void UpdatePinButton(HWND hWnd) {
    int index = SelectedIndex(hWnd);
    bool selected = index >= 0;
    for (int id : {kIdEdit, kIdRemove, kIdTogglePin}) {
        EnableWindow(GetDlgItem(hWnd, id), selected);
    }
    bool pinned = selected && g_rows[index].pinned;
    SetDlgItemTextW(hWnd, kIdTogglePin, pinned ? L"Unpin" : L"Pin");
}

////////////////////////////////////////////////////////////////////////////////
// Main window

// Rows are committed by path rather than by their listbox index. The store is
// shared with the taskbar UI thread (unpin) and an Explorer window thread (the
// context-menu pin), so it can gain or lose entries while a modal dialog or a
// confirmation box is up — and an index taken before that would then write to,
// or delete, a different folder than the one on screen.
void EditSelected(HWND hWnd) {
    int index = SelectedIndex(hWnd);
    if (index < 0) {
        return;
    }
    EditContext ctx;
    ctx.entry = g_rows[index];
    ctx.originalPath = g_rows[index].path;
    if (!RunEditDialog(hWnd, &ctx, L"Edit folder") || g_unloading) {
        return;
    }

    bool gone = false;
    {
        std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
        auto stored = FolderStore::Read();
        int at = FolderStore::IndexOfPath(stored, ctx.originalPath);
        if (at < 0) {
            gone = true;
        } else {
            stored[at] = ctx.entry;
            FolderStore::Write(stored);
        }
    }
    if (gone) {
        Wh_Log(L"Edit: '%s' is no longer in the store; discarding the edit",
               ctx.originalPath.c_str());
        PopulateList(hWnd);
        return;
    }
    RefreshAfterStoreChange(hWnd);
}

void AddNew(HWND hWnd) {
    EditContext ctx;
    ctx.entry.pinned = true;
    if (!RunEditDialog(hWnd, &ctx, L"Add folder") || g_unloading) {
        return;
    }

    bool duplicate = false;
    {
        std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
        auto stored = FolderStore::Read();
        // The dialog already checked, but another thread may have pinned the
        // same folder while it was open.
        if (FolderStore::IndexOfPath(stored, ctx.entry.path) >= 0) {
            duplicate = true;
        } else {
            stored.push_back(ctx.entry);
            FolderStore::Write(stored);
        }
    }
    if (duplicate) {
        Wh_Log(L"Add: '%s' was added elsewhere first", ctx.entry.path.c_str());
        PopulateList(hWnd);
        return;
    }
    RefreshAfterStoreChange(hWnd);
}

void RemoveSelected(HWND hWnd) {
    int index = SelectedIndex(hWnd);
    if (index < 0) {
        return;
    }
    std::wstring path = g_rows[index].path;
    std::wstring name = g_rows[index].name.empty() ? path : g_rows[index].name;
    std::wstring prompt = L"Remove \"" + name +
                          L"\" from the list?\n\nThe folder itself is not "
                          L"touched — only this button. To keep the entry but "
                          L"take it off the taskbar, use Unpin instead.";
    if (MessageBoxW(hWnd, prompt.c_str(), L"Taskbar Folders",
                    MB_OKCANCEL | MB_ICONQUESTION) != IDOK ||
        g_unloading) {
        return;
    }

    bool gone = false;
    {
        std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
        auto stored = FolderStore::Read();
        int at = FolderStore::IndexOfPath(stored, path);
        if (at < 0) {
            gone = true;
        } else {
            stored.erase(stored.begin() + at);
            FolderStore::Write(stored);
        }
    }
    if (gone) {
        Wh_Log(L"Remove: '%s' is no longer in the store", path.c_str());
        PopulateList(hWnd);
        return;
    }
    RefreshAfterStoreChange(hWnd);
}

// Unpins every real taskbar button, deletes their Start Menu shortcuts and
// their per-AppID jump lists — everything InvokeVerb's "taskbarpin" and
// WriteJumpList put in place. Shared by the user-facing "Remove all folder
// buttons" action and the automatic cleanup Wh_ModUninit runs on every unload,
// so nothing this mod wrote outside its own storage outlives either path.
//
// clearStore separates the two callers. The user-facing action means "forget
// everything" and empties the folder list. The unload path must not: Windhawk
// routes a mod update, a settings-page save and an engine restart through the
// same Wh_ModUninit as a real disable, and wiping the list there would delete
// the user's configuration every time the mod is updated. Windhawk removes the
// mod's storage itself when the mod is uninstalled, so the unload path only has
// to undo what it put outside that storage.
//
// CoInitializeEx is refcounted per thread, so this is safe to call whether or
// not the calling thread already has COM up (Wh_ModUninit's does not).
void RemoveAllFolderButtonsCore(bool clearStore) {
    struct ComInit {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        ~ComInit() {
            if (SUCCEEDED(hr)) {
                CoUninitialize();
            }
        }
    } com;

    // The pin verb's COM class is ThreadingModel=Apartment with no marshaler,
    // so every InvokeVerb below silently fails if this thread is not really
    // an STA (RPC_E_CHANGED_MODE means it is already an MTA, which is exactly
    // that case). Bail loudly rather than grinding through a no-op sweep that
    // looks like it ran.
    if (FAILED(com.hr)) {
        Wh_Log(L"RemoveAllFolderButtonsCore: CoInitializeEx(STA) failed: "
               L"0x%08X; skipping unpin sweep",
               com.hr);
        return;
    }

    for (const auto& item : Pins::ReadPinnedItems()) {
        Pins::InvokeVerb(item.path, "taskbarunpin");
        Pins::DeleteJumpList(item.appId);
    }

    std::wstring dir = Pins::SourceDir();
    if (!dir.empty()) {
        WIN32_FIND_DATAW find{};
        HANDLE handle = FindFirstFileW((dir + L"\\*.lnk").c_str(), &find);
        if (handle != INVALID_HANDLE_VALUE) {
            do {
                if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    continue;
                }
                std::wstring path = dir + L"\\" + find.cFileName;
                DeleteFileW(path.c_str());
                SHChangeNotify(SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSH,
                              path.c_str(), nullptr);
            } while (FindNextFileW(handle, &find));
            FindClose(handle);
        }
        // Only removes it if it is now empty, which it is unless something
        // else put a file there.
        RemoveDirectoryW(dir.c_str());
    }

    std::lock_guard<std::recursive_mutex> lock(FolderStore::g_mutex);
    if (clearStore) {
        FolderStore::Write({});
        return;
    }

    // Keep every entry, including its pinned flag, and only clear pinApplied.
    // That flag is what Reconcile's reverse sync trusts: "we pinned this and it
    // is gone now" is how a native Unpin from taskbar is detected. Leaving it
    // set would make the next load read the sweep above as the user unpinning
    // everything and demote every entry to a draft.
    auto stored = FolderStore::Read();
    bool changed = false;
    for (auto& entry : stored) {
        if (entry.pinApplied) {
            entry.pinApplied = false;
            changed = true;
        }
    }
    if (changed) {
        FolderStore::Write(stored);
    }
}

void RemoveAllFolderButtons(HWND hWnd) {
    std::wstring prompt =
        L"Remove all folder buttons from the taskbar?\n\nThis unpins every "
        L"folder button this mod created, deletes their Start Menu "
        L"shortcuts, and clears the folder list. This cannot be undone.";
    if (MessageBoxW(hWnd, prompt.c_str(), L"Taskbar Folders",
                    MB_OKCANCEL | MB_ICONWARNING) != IDOK ||
        g_unloading) {
        return;
    }

    RemoveAllFolderButtonsCore(/*clearStore=*/true);
    RefreshAfterStoreChange(hWnd);
}

// Windhawk's core principle is that disabling a mod puts the system back the
// way it was. Called from Wh_ModUninit, which fires on every unload alike — a
// user disable, an uninstall, a mod update, a settings-page save and an engine
// restart are indistinguishable from here — so everything this mod put outside
// its own storage goes: the real taskbar pins, their Start Menu shortcuts and
// their jump lists.
//
// The folder list itself stays. It is the user's configuration, it lives in the
// mod's private Windhawk storage (which Windhawk deletes on its own when the mod
// is uninstalled), and it is the one thing here that cannot be reconstructed.
// Wiping it on every unload would mean publishing an update silently deleted
// every user's folders. A re-enable therefore re-pins what was configured, and
// "Remove all folder buttons..." in the manager stays the explicit, confirmed
// way to forget everything.
void UnpinAllForDisable() {
    RemoveAllFolderButtonsCore(/*clearStore=*/false);
}

// A single-select listbox never lets go of its selection on its own — a click
// below the last row just does nothing, and there is no other way to clear
// it. LB_ITEMFROMPOINT's high word flags a point outside every row's client
// area, which is what turns that click into an explicit deselect. Clicks
// that land on a row are left to the default handling (including on the
// already-selected row) so a click always does the one obvious thing instead
// of alternating select/deselect on repeat clicks at the same spot.
LRESULT CALLBACK ListSubclassProc(HWND hWnd, UINT msg, WPARAM wParam,
                                  LPARAM lParam, UINT_PTR /*subclassId*/,
                                  DWORD_PTR /*refData*/) {
    if (msg == WM_LBUTTONDOWN) {
        LRESULT hit = SendMessageW(hWnd, LB_ITEMFROMPOINT, 0, lParam);
        if (HIWORD(hit) != 0) {
            SendMessageW(hWnd, LB_SETCURSEL, (WPARAM)-1, 0);
            HWND parent = GetParent(hWnd);
            SendMessageW(parent, WM_COMMAND,
                        MAKEWPARAM(GetDlgCtrlID(hWnd), LBN_SELCHANGE),
                        (LPARAM)hWnd);
            return 0;
        }
    }
    return DefSubclassProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            HINSTANCE instance = GetCurrentModuleHandle();
            HWND list = CreateWindowExW(
                FieldExStyle(), L"LISTBOX", nullptr,
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | LBS_NOTIFY |
                    LBS_OWNERDRAWVARIABLE,
                0, 0, 0, 0, hWnd, (HMENU)(INT_PTR)kIdList, instance, nullptr);
            SetWindowSubclass(list, ListSubclassProc, 0, 0);

            struct ButtonSpec {
                int id;
                PCWSTR text;
            };
            const ButtonSpec buttons[] = {{kIdAdd, L"Add..."},
                                          {kIdEdit, L"Edit..."},
                                          {kIdRemove, L"Remove"},
                                          {kIdTogglePin, L"Pin"},
                                          {kIdClose, L"Close"},
                                          {kIdRemoveAll,
                                           L"Remove all folder buttons..."}};
            for (const auto& b : buttons) {
                CreateWindowExW(0, L"BUTTON", b.text,
                                WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                    BS_PUSHBUTTON,
                                0, 0, 0, 0, hWnd, (HMENU)(INT_PTR)b.id,
                                instance, nullptr);
            }

            CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 0, 0, 0,
                            0, hWnd, (HMENU)(INT_PTR)kIdHint, instance,
                            nullptr);

            LayoutChildren(hWnd, kMainLayout, ARRAYSIZE(kMainLayout),
                           DpiOf(hWnd));
            ApplyUiFont(hWnd, DpiOf(hWnd));
            ApplyTheme(hWnd);
            PopulateList(hWnd);
            return 0;
        }

        // One message per item, sent as it is added — by which point g_rows is
        // already filled, so the section boundary is known. A row that opens a
        // section carries its caption band as extra height rather than the
        // caption being a list item of its own.
        //
        // The parent's DPI, not the listbox's: this can arrive before the
        // listbox has an HWND. Same monitor either way.
        case WM_MEASUREITEM: {
            auto* mis = (LPMEASUREITEMSTRUCT)lParam;
            if (mis->CtlID == kIdList) {
                int dpi = DpiOf(hWnd);
                mis->itemHeight = Scale(kRowHeight, dpi);
                if (StartsSection(mis->itemID)) {
                    mis->itemHeight += Scale(kSectionHeader, dpi);
                }
                return TRUE;
            }
            break;
        }

        // The listbox and the buttons paint themselves; these are the surfaces
        // they ask the parent about. Without them a dark window still shows
        // white label and edit backgrounds.
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
            if (!Dark()) {
                break;
            }
            return ThemedCtlColor((HDC)wParam, true);

        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLOREDIT:
            if (!Dark()) {
                break;
            }
            return ThemedCtlColor((HDC)wParam, false);

        // The class background brush is COLOR_BTNFACE, which is light. Painting
        // it here rather than swapping the class brush keeps one window class
        // working for both themes.
        case WM_ERASEBKGND: {
            if (!Dark()) {
                break;
            }
            RECT client{};
            GetClientRect(hWnd, &client);
            FillRect((HDC)wParam, &client, FaceBrush());
            static constexpr int kFields[] = {kIdList};
            DrawFieldBorders(hWnd, (HDC)wParam, kFields, ARRAYSIZE(kFields));
            return TRUE;
        }

        // Dragged to a monitor at another scale. PopulateList is what fixes the
        // rows: it resets the list content, which makes the listbox ask for
        // every item's height again, and it re-extracts the row icons at the new
        // pixel size.
        case WM_DPICHANGED: {
            const RECT* suggested = (const RECT*)lParam;
            SetWindowPos(hWnd, nullptr, suggested->left, suggested->top,
                         suggested->right - suggested->left,
                         suggested->bottom - suggested->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            int dpi = LOWORD(wParam);
            LayoutChildren(hWnd, kMainLayout, ARRAYSIZE(kMainLayout), dpi);
            ApplyUiFont(hWnd, dpi);
            InvalidateRect(hWnd, nullptr, TRUE);
            PopulateList(hWnd);
            return 0;
        }

        case WM_DRAWITEM: {
            auto* dis = (LPDRAWITEMSTRUCT)lParam;
            if (dis->CtlID == kIdList) {
                DrawRow(dis);
                return TRUE;
            }
            break;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);
            if (id == kIdAdd) {
                AddNew(hWnd);
            } else if (id == kIdEdit) {
                EditSelected(hWnd);
            } else if (id == kIdRemove) {
                RemoveSelected(hWnd);
            } else if (id == kIdTogglePin) {
                TogglePinSelected(hWnd);
            } else if (id == kIdClose) {
                DestroyWindow(hWnd);
            } else if (id == kIdRemoveAll) {
                RemoveAllFolderButtons(hWnd);
            } else if (id == kIdList && code == LBN_DBLCLK) {
                EditSelected(hWnd);
            } else if (id == kIdList && code == LBN_SELCHANGE) {
                // Pin/Unpin names the action, so it has to follow the
                // selection rather than only the store.
                UpdatePinButton(hWnd);
            }
            return 0;
        }

        // Posted when the folder list changed underneath an open window.
        case WM_APP:
            PopulateList(hWnd);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            FreeRowIcons();
            g_wnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Runs the whole window — class, creation, message loop — on one dedicated
// thread, so nothing here can stall the taskbar UI thread that opened it.
// `anchor` is where the user invoked it from, captured before the thread
// started, so the window lands on the monitor they were working on rather
// than wherever Windows would have cascaded it.
void ThreadMain(POINT anchor) {
    // Shell icon extraction and the folder picker both need an apartment.
    HRESULT comInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    HINSTANCE instance = GetCurrentModuleHandle();
    if (!EnsureClass(kClassName, WndProc)) {
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        g_active = false;
        return;
    }

    // explorer.exe is Per-Monitor-DPI-Aware V2 and this thread inherits that,
    // so nothing here scales on its own. The window is created at the anchor
    // with a provisional size purely to put it on the right monitor; its DPI
    // is only knowable once it exists, so the real size is applied below,
    // before it is ever shown. WM_CREATE already lays the children out at the
    // final DPI — GetDpiForWindow is correct from creation.
    const DWORD style =
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    HWND hWnd = CreateWindowExW(WS_EX_APPWINDOW, kClassName, L"Taskbar Folders",
                                style, anchor.x, anchor.y, kMainWidth,
                                kMainHeight, nullptr, nullptr, instance,
                                nullptr);
    if (hWnd) {
        int dpi = DpiOf(hWnd);
        RECT rect{0, 0, Scale(kMainWidth, dpi), Scale(kMainHeight, dpi)};
        AdjustWindowRectExForDpi(&rect, style, FALSE, WS_EX_APPWINDOW,
                                 (UINT)dpi);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        POINT pos = CenterOnMonitor(anchor, width, height);
        SetWindowPos(hWnd, nullptr, pos.x, pos.y, width, height,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }
    if (!hWnd) {
        Wh_Log(L"Manager: CreateWindowExW failed (error %u)", GetLastError());
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        g_active = false;
        return;
    }

    g_wnd = hWnd;
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessageW(hWnd, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    g_wnd = nullptr;
    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }
    g_active = false;
}

// The manager thread itself, kept rather than detached so CloseAndWait can
// wait on the thread instead of polling a flag that is cleared before the
// thread has finished unwinding through this DLL's image.
//
// no_destroy for the same reason as g_scanThread: destroying a joinable
// std::thread calls std::terminate, and CRT teardown at Explorer exit must not
// touch it.
[[clang::no_destroy]] std::optional<std::thread> g_thread;
// Guards the slot above — Open() can be called from the taskbar UI thread
// while Wh_ModUninit runs CloseAndWait on another.
std::mutex g_threadMutex;

// Brings an already-open window forward instead of opening a second one.
void Open() {
    if (HWND existing = g_wnd.load()) {
        SetForegroundWindow(existing);
        return;
    }
    if (g_unloading || g_active.exchange(true)) {
        return;
    }
    // Read on the caller's thread, while the click that opened this is still
    // where it happened, to pick the monitor the user is actually looking at.
    POINT anchor{};
    if (!GetCursorPos(&anchor)) {
        anchor = POINT{0, 0};
    }
    try {
        std::lock_guard<std::mutex> lock(g_threadMutex);
        // Re-checked under the lock, the same way StartScanThread and
        // StartRetryThread do it: CloseAndWait takes this mutex after
        // g_unloading is set, so either it sees this thread and joins it, or
        // this sees the flag and never starts one. The check above the lock is
        // only a fast path — on its own it loses the race where CloseAndWait
        // swaps out an empty slot between that check and the emplace below,
        // leaving a fresh message loop running in an image about to be
        // unmapped.
        if (g_unloading) {
            g_active = false;
            return;
        }
        // Reap the previous run before reusing the slot: assigning over a
        // joinable std::thread calls std::terminate.
        if (g_thread) {
            if (g_thread->joinable()) {
                g_thread->join();
            }
            g_thread.reset();
        }
        g_thread.emplace(ThreadMain, anchor);
    } catch (...) {
        Wh_Log(L"Manager: could not start its thread");
        g_active = false;
    }
}

// Asks the window to close and waits for its thread to unwind, so the DLL is
// not unloaded out from under a live message loop.
void CloseAndWait() {
    std::optional<std::thread> thread;
    {
        std::lock_guard<std::mutex> lock(g_threadMutex);
        thread.swap(g_thread);
    }
    if (!thread) {
        return;
    }
    if (!thread->joinable() ||
        GetThreadId((HANDLE)thread->native_handle()) == GetCurrentThreadId()) {
        // Never reached today (only Wh_ModUninit calls this), but joining
        // ourselves would deadlock outright.
        thread->detach();
        return;
    }

    // WM_CLOSE to *every* window on that thread, not only the main one: a
    // Remove confirmation MessageBox or an open IFileDialog runs its own modal
    // loop, and nothing but closing that window dismisses it — which is what
    // used to leave the thread running past the old five-second give-up, with
    // the DLL then unloaded out from under it. The main window goes last so a
    // modal child is never orphaned by its owner being destroyed first.
    HANDLE handle = (HANDLE)thread->native_handle();
    auto closeAllWindows = [handle] {
        HWND main = g_wnd.load();
        if (DWORD threadId = GetThreadId(handle)) {
            EnumThreadWindows(
                threadId,
                [](HWND hWnd, LPARAM mainWnd) -> BOOL {
                    if (hWnd != (HWND)mainWnd) {
                        PostMessageW(hWnd, WM_CLOSE, 0, 0);
                    }
                    return TRUE;
                },
                (LPARAM)main);
        }
        if (main) {
            PostMessageW(main, WM_CLOSE, 0, 0);
        }
    };

    closeAllWindows();
    // Pumping sent messages matters as much here as in StopScanThread: this
    // wait must not be what blocks a thread trying to SendMessage into it. The
    // two-second timeout only exists to re-close a modal that appeared after
    // the first round; the wait itself is unbounded on purpose, because
    // returning early is exactly what would let Windhawk unload this DLL with
    // the manager thread still executing inside it.
    for (int elapsedMs = 0;;) {
        DWORD result =
            MsgWaitForMultipleObjects(1, &handle, FALSE, 2000, QS_SENDMESSAGE);
        if (result == WAIT_OBJECT_0) {
            break;
        }
        if (result == WAIT_OBJECT_0 + 1) {
            MSG msg;
            PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
            continue;
        }
        if (result == WAIT_FAILED) {
            Wh_Log(L"CloseAndWait: MsgWaitForMultipleObjects failed (%u)",
                   GetLastError());
            break;
        }
        elapsedMs += 2000;
        if (elapsedMs % 10000 == 0) {
            Wh_Log(L"Uninit: still waiting on the folder manager thread (%d s)",
                   elapsedMs / 1000);
        }
        closeAllWindows();
    }
    thread->join();
}

}  // namespace FolderManager

TaskbarHost* FindTaskbarHost(HWND taskbarWnd) {
    if (!g_taskbarHosts) {
        return nullptr;
    }
    for (auto& host : *g_taskbarHosts) {
        if (host && host->hwnd == taskbarWnd) {
            return host.get();
        }
    }
    return nullptr;
}

// Releases everything this host attached to elements the taskbar owns. Nothing
// of ours is in the visual tree any more, so this is only handlers.
void RemoveHostGrid(TaskbarHost* host) {
    if (!host) {
        return;
    }

    if (host->trackedRootGrid && host->layoutUpdatedToken.value) {
        try {
            host->trackedRootGrid.LayoutUpdated(host->layoutUpdatedToken);
        } catch (...) {
        }
    }
    host->layoutUpdatedToken = {};

    // The taskbar keeps its buttons after the mod is gone, so a handler left
    // attached would outlive the image it lives in.
    UnbindPinButtons(host);
    host->lastRealizedChildren.clear();
    host->lastPinBindCount = -1;
    host->lastLabelGeneration = 0;

    host->trackedRootGrid = nullptr;
    host->cachedRepeater = nullptr;
}

void RemoveAllHostGrids() {
    if (!g_taskbarHosts) {
        g_injectionLive = false;
        return;
    }
    for (auto& host : *g_taskbarHosts) {
        RemoveHostGrid(host.get());
    }
    g_taskbarHosts->clear();
    g_injectionLive = false;
}

// Keeps hover attached to the real pinned buttons as the taskbar changes.
//
// This is all that is left of a function that used to be ~270 lines: anchor
// resolution, a strip census, reserve holds, settle holds, drag freezes and a
// slide animation, all of it trying to predict where Windows was about to put
// an icon. None of that is needed once the button belongs to the shell.
void OnRootGridLayoutUpdated(TaskbarHost* host) {
    if (g_unloading || !host || !host->trackedRootGrid) {
        return;
    }

    // Reached from a XAML delegate, so a WinRT failure must not unwind into the
    // framework's own frame.
    try {
        FrameworkElement repeater = host->cachedRepeater;
        bool repeaterLooksDead = false;
        try {
            repeaterLooksDead = !repeater || repeater.ActualWidth() <= 1.0;
        } catch (...) {
            repeaterLooksDead = true;
        }
        if (repeaterLooksDead) {
            repeater = FindTaskbarRepeater(host->trackedRootGrid);
            host->cachedRepeater = repeater;
        }
        if (!repeater) {
            return;
        }

        // Cheap: compares the realized container identities and the label map
        // generation, and returns immediately unless something actually
        // changed. LayoutUpdated fires on every animation frame.
        RebindPinButtons(host);
    } catch (...) {
    }
}

bool InjectHostGridForTaskbar(HWND taskbarWnd) {
    if (!taskbarWnd) {
        return false;
    }

    auto xamlRoot = GetTaskbarXamlRoot(taskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"No taskbar XamlRoot yet for %p", taskbarWnd);
        return false;
    }

    auto root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) {
        return false;
    }

    auto rootGrid = FindTaskbarRootGrid(root);
    if (!rootGrid) {
        Wh_Log(L"Taskbar RootGrid not found yet for %p", taskbarWnd);
        return false;
    }

    auto repeater = FindTaskbarRepeater(rootGrid);
    if (!repeater) {
        Wh_Log(L"TaskbarFrameRepeater not found yet for %p", taskbarWnd);
        return false;
    }

    // Leave a live host alone on retry; only rebuild when the taskbar root was
    // replaced, which is what an Explorer rebuild looks like from here.
    if (auto* existing = FindTaskbarHost(taskbarWnd)) {
        bool stillLive = false;
        try {
            stillLive = existing->trackedRootGrid == rootGrid;
        } catch (...) {
            stillLive = false;
        }
        if (stillLive) {
            return true;
        }

        RemoveHostGrid(existing);
        auto& hosts = TaskbarHosts();
        hosts.erase(std::remove_if(hosts.begin(), hosts.end(),
                                   [taskbarWnd](
                                       const std::unique_ptr<TaskbarHost>& h) {
                                       return h && h->hwnd == taskbarWnd;
                                   }),
                    hosts.end());
    }

    auto host = std::make_unique<TaskbarHost>();
    host->hwnd = taskbarWnd;
    host->cachedRepeater = repeater;
    host->trackedRootGrid = rootGrid;

    // Nothing is added to the visual tree. The buttons are real pinned taskbar
    // items that Windows already created; all that is attached here is hover on
    // the ones that are ours, plus a layout hook to keep that attached as the
    // repeater recycles its containers.
    RebindPinButtons(host.get());

    TaskbarHost* raw = host.get();
    host->layoutUpdatedToken = rootGrid.LayoutUpdated(
        [raw](winrt::Windows::Foundation::IInspectable const&,
              winrt::Windows::Foundation::IInspectable const&) {
            OnRootGridLayoutUpdated(raw);
        });

    Wh_Log(L"Watching taskbar %p for folder buttons", taskbarWnd);

    TaskbarHosts().push_back(std::move(host));
    return true;
}

bool InjectHostGridsOnTaskbarThread() {
    // Reached via RunFromWindowThread / WH_CALLWNDPROC — not a XAML delegate —
    // so WinRT failures must not unwind through user32's callback frame.
    try {
        // Register classes and create the menu owner on this (taskbar) thread so
        // DestroyWindow / UnregisterClass later stay same-thread-safe.
        EnsureMenuOwnerWindow();

        // Taskbar UI thread already has an STA — resolve shell: paths here so
        // hover/open and RequestScan see filesystem paths (no CoInitializeEx).
        ResolvePendingFolderEntries();

        // An empty folder list is not an early-out: InjectHostGridForTaskbar
        // still seats a host so a later settings reload that adds folders can
        // bind buttons immediately, without waiting on the retry cycle.
        //
        // Drop hosts for taskbar HWNDs that no longer exist (unplugged monitors)
        // without tearing down every seated host on each retry.
        if (g_taskbarHosts) {
            auto& hosts = *g_taskbarHosts;
            for (auto it = hosts.begin(); it != hosts.end();) {
                if (*it && !IsWindow((*it)->hwnd)) {
                    RemoveHostGrid(it->get());
                    it = hosts.erase(it);
                } else {
                    ++it;
                }
            }
        }

        struct EnumState {
            int found = 0;
            int injected = 0;
        } state;

        // Primary and secondary tray windows share the explorer taskbar UI thread
        // on Windows 11, matching other Windhawk taskbar mods.
        EnumThreadWindows(
            GetCurrentThreadId(),
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                auto* state = reinterpret_cast<EnumState*>(lParam);
                WCHAR className[32];
                if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                    return TRUE;
                }
                if (_wcsicmp(className, L"Shell_TrayWnd") != 0 &&
                    _wcsicmp(className, L"Shell_SecondaryTrayWnd") != 0) {
                    return TRUE;
                }
                state->found++;
                if (InjectHostGridForTaskbar(hWnd)) {
                    state->injected++;
                } else {
                    Wh_Log(L"Inject failed for %s %p", className, hWnd);
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&state));

        if (g_taskbarHosts && !g_taskbarHosts->empty()) {
            g_taskbarWnd = (*g_taskbarHosts)[0]->hwnd;
            UpdatePopupDpi();
            int iconPixelSize = PopupIconPixelSize();
            for (size_t i = 0; i < g_settings.folders.size(); i++) {
                RequestScan(FolderPathForButton((int)i), iconPixelSize);
            }
        }

        g_injectionLive = state.found > 0 && state.injected == state.found;
        Wh_Log(L"Taskbar inject: %d/%d windows, %d hosts", state.injected,
               state.found,
               g_taskbarHosts ? (int)g_taskbarHosts->size() : 0);
        return g_injectionLive;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Injection failed: %s", e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Injection failed");
        return false;
    }
}

bool ApplyOnWindowThread(RunFromWindowThreadProc_t proc) {
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd) {
        Wh_Log(L"ApplyOnWindowThread: no taskbar window found");
        return false;
    }
    if (!g_taskbarWnd) {
        g_taskbarWnd = taskbarWnd;
    }
    if (!RunFromWindowThread(taskbarWnd, proc, nullptr)) {
        Wh_Log(L"ApplyOnWindowThread: RunFromWindowThread failed (error %u)",
               GetLastError());
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Injection retries
//
// The taskbar is rebuilt on theme changes, Explorer restarts and DPI changes,
// so injection is retried with a backoff until it sticks.

DWORD WINAPI RetryThreadProc(void* param) {
    HANDLE stopEvent = static_cast<HANDLE>(param);
    // Capture kick once; StopRetryThread closes it only after this thread joins.
    HANDLE kickEvent = g_retryKickEvent;
    static const DWORD delays[] = {0,    500,  1000,  2000,
                                   4000, 8000, 15000, 30000};

    for (;;) {
        HANDLE waits[] = {stopEvent, kickEvent};
        DWORD wake = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (wake == WAIT_OBJECT_0 || wake == WAIT_FAILED) {
            return 0;  // stop (or wait failed)
        }
        if (g_unloading ||
            WaitForSingleObject(stopEvent, 0) != WAIT_TIMEOUT) {
            return 0;
        }

        g_injectionLive = false;

        for (size_t i = 0; i < ARRAYSIZE(delays) && !g_unloading; i++) {
            if (delays[i] &&
                WaitForSingleObject(stopEvent, delays[i]) != WAIT_TIMEOUT) {
                return 0;
            }
            if (g_unloading) {
                return 0;
            }

            ApplyOnWindowThread([](void*) { InjectHostGridsOnTaskbarThread(); });

            if (g_injectionLive || g_unloading) {
                break;
            }
        }
        // Success or exhaustion: wait for the next kick (e.g. secondary
        // TrayUI::StartTaskbar) instead of exiting — avoids the one-shot
        // exit race that could drop a re-injection request.
    }
}

void StopRetryThread() {
    AcquireSRWLockExclusive(&g_retryLock);
    HANDLE retryThread = g_retryThread;
    HANDLE retryStopEvent = g_retryStopEvent;
    HANDLE retryKickEvent = g_retryKickEvent;
    g_retryThread = nullptr;
    g_retryStopEvent = nullptr;
    g_retryKickEvent = nullptr;
    if (retryStopEvent) {
        SetEvent(retryStopEvent);
    }
    ReleaseSRWLockExclusive(&g_retryLock);

    if (retryThread) {
        // Pump sent messages while waiting so a worker blocked in SendMessage
        // to this UI thread cannot deadlock the join. Only settings/uninit
        // should call this; window procs use StartRetryThread (non-blocking).
        DWORD result;
        do {
            result = MsgWaitForMultipleObjects(1, &retryThread, FALSE, INFINITE,
                                               QS_SENDMESSAGE);
            if (result == WAIT_OBJECT_0 + 1) {
                MSG msg;
                PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
            }
        } while (result == WAIT_OBJECT_0 + 1);
        CloseHandle(retryThread);
    }
    if (retryStopEvent) {
        CloseHandle(retryStopEvent);
    }
    if (retryKickEvent) {
        CloseHandle(retryKickEvent);
    }
}

// Non-blocking: safe from window procedures. Ensures the lifetime worker exists
// and kicks it. StopRetryThread still joins for Wh_ModSettingsChanged / uninit.
void StartRetryThread() {
    AcquireSRWLockExclusive(&g_retryLock);
    if (g_unloading) {
        ReleaseSRWLockExclusive(&g_retryLock);
        return;
    }

    if (!g_retryStopEvent) {
        g_retryStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_retryStopEvent) {
            Wh_Log(L"CreateEventW for retry stop failed (%u)", GetLastError());
            ReleaseSRWLockExclusive(&g_retryLock);
            return;
        }
    }
    if (!g_retryKickEvent) {
        g_retryKickEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!g_retryKickEvent) {
            Wh_Log(L"CreateEventW for retry kick failed (%u)", GetLastError());
            CloseHandle(g_retryStopEvent);
            g_retryStopEvent = nullptr;
            ReleaseSRWLockExclusive(&g_retryLock);
            return;
        }
    }

    if (!g_retryThread) {
        HANDLE stopEvent = g_retryStopEvent;
        g_retryThread =
            CreateThread(nullptr, 0, RetryThreadProc, stopEvent, 0, nullptr);
        if (!g_retryThread) {
            Wh_Log(L"CreateThread for retry failed (%u)", GetLastError());
            CloseHandle(g_retryStopEvent);
            CloseHandle(g_retryKickEvent);
            g_retryStopEvent = nullptr;
            g_retryKickEvent = nullptr;
            ReleaseSRWLockExclusive(&g_retryLock);
            return;
        }
    }

    SetEvent(g_retryKickEvent);
    ReleaseSRWLockExclusive(&g_retryLock);
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L"TrayUI::StartTaskbar");
    TrayUI_StartTaskbar_Original(pThis);
    if (!g_unloading) {
        StartRetryThread();
    }
}

// Click interception.
//
// The shortcut behind a folder button targets `explorer.exe "<folder>"`, so a
// left click opens the folder — which is exactly what `openFolderOnClick: true`
// means, and it costs no hook at all. Only the false case needs anything: swallow
// the activation so the button is hover-only, as it was before the buttons were
// real.
//
// Right click does not come through here (it goes via _HandleShellContextMenu),
// so the native jump list is unaffected. The eCLICKACTION value is logged rather
// than filtered on: its members are not in the public PDB, and every action that
// does reach this function is an activation we want swallowed.
//
// All three symbols are optional. If a future build renames one, the click falls
// through and Explorer opens the folder — the same thing the true setting does,
// which is a sane place to degrade to.
using CTaskBtnGroup_GetGroup_t = void*(WINAPI*)(void* pThis);
CTaskBtnGroup_GetGroup_t CTaskBtnGroup_GetGroup_Original;

using CTaskGroup_GetAppID_t = PCWSTR(WINAPI*)(void* pThis);
CTaskGroup_GetAppID_t CTaskGroup_GetAppID_Original;

using CTaskListWnd__HandleClick_t = void(WINAPI*)(void* pThis,
                                                 void* taskBtnGroup,
                                                 int taskItemIndex,
                                                 int clickAction,
                                                 int a5,
                                                 int a6);
CTaskListWnd__HandleClick_t CTaskListWnd__HandleClick_Original;

void WINAPI CTaskListWnd__HandleClick_Hook(void* pThis,
                                           void* taskBtnGroup,
                                           int taskItemIndex,
                                           int clickAction,
                                           int a5,
                                           int a6) {
    if (!g_unloading && !g_settings.openFolderOnClick && taskBtnGroup &&
        CTaskBtnGroup_GetGroup_Original && CTaskGroup_GetAppID_Original) {
        // GetAppID lives on CTaskGroup, which is the class behind ITaskGroup;
        // the interface is its first base, so the pointer passes straight
        // through. Same call the other taskbar mods make.
        void* taskGroup = CTaskBtnGroup_GetGroup_Original(taskBtnGroup);
        PCWSTR appId = taskGroup ? CTaskGroup_GetAppID_Original(taskGroup)
                                 : nullptr;
        if (appId && Pins::IsOurAppId(appId)) {
            Wh_Log(L"Swallowing click (action %d) on folder button %s",
                   clickAction, appId);
            return;
        }
    }

    CTaskListWnd__HandleClick_Original(pThis, taskBtnGroup, taskItemIndex,
                                       clickAction, a5, a6);
}

// Getting out of the shell's way.
//
// Two things the taskbar shows on its own overlap the hover grid, and neither
// clears itself, because from the shell's point of view nothing has happened:
//
//  * The thumbnail preview of whatever app was hovered before. Moving from an
//    app button onto a folder button leaves it up, floating over the grid that
//    just opened. _SetHotItem is where the shell decides which button is hot, so
//    it is the exact moment to tell it to drop the preview.
//
//  * The hover grid itself, when the folder button is right-clicked. The pointer
//    never leaves the button, so PointerExited never fires and the grid sits
//    under the jump list. _OnJumpViewShown fires whichever way the jump list was
//    summoned.
//
// Both hooks are optional. Without them the two surfaces overlap, which is what
// happens today — annoying, not broken.
using CTaskListWnd_DismissAllSecondaryUI_t = void(WINAPI*)(void* pThis);
CTaskListWnd_DismissAllSecondaryUI_t CTaskListWnd_DismissAllSecondaryUI_Original;

using CTaskListWnd__SetHotItem_t = void(WINAPI*)(void* pThis,
                                                void* taskBtnGroup,
                                                int taskItemIndex,
                                                int flags);
CTaskListWnd__SetHotItem_t CTaskListWnd__SetHotItem_Original;

using CTaskListWnd__OnJumpViewShown_t = void(WINAPI*)(void* pThis,
                                                     void* appId);
CTaskListWnd__OnJumpViewShown_t CTaskListWnd__OnJumpViewShown_Original;

// True if this button group is one of ours. Null-safe on both hooks.
bool TaskBtnGroupIsOurs(void* taskBtnGroup) {
    if (!taskBtnGroup || !CTaskBtnGroup_GetGroup_Original ||
        !CTaskGroup_GetAppID_Original) {
        return false;
    }
    void* taskGroup = CTaskBtnGroup_GetGroup_Original(taskBtnGroup);
    PCWSTR appId =
        taskGroup ? CTaskGroup_GetAppID_Original(taskGroup) : nullptr;
    return appId && Pins::IsOurAppId(appId);
}

// The last group the shell made hot, so the dismiss fires once on arrival
// rather than on every mouse move within the same button. Taskbar UI thread
// only, and only ever compared for identity — never dereferenced, so a freed
// group is harmless.
void* g_lastHotGroup = nullptr;

void WINAPI CTaskListWnd__SetHotItem_Hook(void* pThis,
                                          void* taskBtnGroup,
                                          int taskItemIndex,
                                          int flags) {
    bool arriving = taskBtnGroup && taskBtnGroup != g_lastHotGroup;
    g_lastHotGroup = taskBtnGroup;

    CTaskListWnd__SetHotItem_Original(pThis, taskBtnGroup, taskItemIndex, flags);

    // After the original, so the shell has finished its own hot-item work
    // before being asked to drop the preview — and only on arrival, or a right
    // click that re-hots the same button would dismiss the jump list it just
    // opened.
    if (arriving && !g_unloading && CTaskListWnd_DismissAllSecondaryUI_Original &&
        TaskBtnGroupIsOurs(taskBtnGroup)) {
        CTaskListWnd_DismissAllSecondaryUI_Original(pThis);
    }
}

void WINAPI CTaskListWnd__OnJumpViewShown_Hook(void* pThis, void* appId) {
    CTaskListWnd__OnJumpViewShown_Original(pThis, appId);
    if (g_unloading) {
        return;
    }
    // Closed for any jump list, not only ours: the grid is a hover surface, and
    // a menu opening anywhere on the taskbar means the user has moved on from
    // it. Runs on the taskbar UI thread, which is what owns the grid windows.
    Wh_Log(L"A jump list opened; closing the hover grid");
    OnPointerExitedButton();
    CloseChain();
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_HandleClick(struct ITaskBtnGroup *,int,enum CTaskListWnd::eCLICKACTION,int,int))"},
            &CTaskListWnd__HandleClick_Original,
            CTaskListWnd__HandleClick_Hook,
            true,  // optional
        },
        {
            {LR"(public: virtual struct ITaskGroup * __cdecl CTaskBtnGroup::GetGroup(void))"},
            &CTaskBtnGroup_GetGroup_Original,
            nullptr,
            true,  // optional
        },
        {
            {LR"(public: virtual unsigned short const * __cdecl CTaskGroup::GetAppID(void))"},
            &CTaskGroup_GetAppID_Original,
            nullptr,
            true,  // optional
        },
        {
            {LR"(public: virtual void __cdecl CTaskListWnd::DismissAllSecondaryUI(void))"},
            &CTaskListWnd_DismissAllSecondaryUI_Original,
            nullptr,
            true,  // optional
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_SetHotItem(struct ITaskBtnGroup *,int,enum SetHotItemFlags))"},
            &CTaskListWnd__SetHotItem_Original,
            CTaskListWnd__SetHotItem_Hook,
            true,  // optional
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_OnJumpViewShown(struct HSTRING__ *))"},
            &CTaskListWnd__OnJumpViewShown_Original,
            CTaskListWnd__OnJumpViewShown_Hook,
            true,  // optional
        },
    };

    return WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

////////////////////////////////////////////////////////////////////////////////
// Explorer/Desktop right-click integration
//
// Hooks TrackPopupMenuEx, which explorer.exe calls right before showing the
// classic ("Show more options") context menu for a shell view (an Explorer
// window or the desktop). A right-clicked file or shortcut gets
// move/copy-into-folder entries (files also get "Copy as shortcut"); a
// right-clicked plain folder gets a pin option plus a shortcut-into-folder
// entry. Same technique as the published "Explorer Context Menu Custom
// Items" Windhawk mod.

namespace AddToTaskbar {

constexpr UINT kMinId = 0xC901;
// Sits above every dynamic id (pin, then three folder-count-sized ranges for
// move/copy/shortcut), so it cannot collide however many folders are configured
// — FolderStore caps the list at 200.
constexpr UINT kIdManage = kMinId + 1000;

std::wstring LeafName(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    std::wstring leaf = pos == std::wstring::npos ? path : path.substr(pos + 1);
    if (PathIsLnkFile(leaf)) {
        leaf.resize(leaf.size() - 4);
    }
    return leaf;
}

// Win32 menus treat '&' as an accelerator marker; double it so names like
// "R&D" render literally instead of showing an underlined "D".
std::wstring EscapeMenuLabel(const std::wstring& s) {
    std::wstring out;
    for (wchar_t c : s) {
        if (c == L'&') {
            out += L'&';
        }
        out += c;
    }
    return out;
}

// Position (by-position index) of the top-level item whose visible text
// matches `label`, ignoring '&' mnemonics and any trailing "\taccelerator".
// Returns -1 if not found.
int FindMenuItemPositionByLabel(HMENU menu, PCWSTR label) {
    int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; i++) {
        WCHAR buf[256];
        int len = GetMenuStringW(menu, i, buf, ARRAYSIZE(buf), MF_BYPOSITION);
        if (len <= 0) {
            continue;
        }
        std::wstring text(buf, len);
        if (PCWSTR tab = wcschr(text.c_str(), L'\t')) {
            text.resize(tab - text.c_str());
        }
        std::wstring stripped;
        for (wchar_t c : text) {
            if (c != L'&') {
                stripped += c;
            }
        }
        if (_wcsicmp(stripped.c_str(), label) == 0) {
            return i;
        }
    }
    return -1;
}

// The shell view that owns `hwnd`, or null if there is not one — which is also
// what says "this is an Explorer or Desktop context menu, not some other
// TrackPopupMenuEx caller in the process".
HWND FindShellViewWindow(HWND hwnd) {
    for (HWND h = hwnd; h; h = GetParent(h)) {
        WCHAR cls[64]{};
        GetClassNameW(h, cls, ARRAYSIZE(cls));
        if (wcscmp(cls, L"SHELLDLL_DefView") == 0) {
            return h;
        }
    }
    return nullptr;
}

// CWM_GETISHELLBROWSER. Undocumented, and WM_USER-relative, so it means
// something entirely different to every window class that does not implement
// it — which is why it is only ever sent to the classes known to answer it,
// rather than up an arbitrary parent chain that ends at Progman or WorkerW.
constexpr UINT CWM_GETISHELLBROWSER = WM_USER + 7;

bool AnswersGetIShellBrowser(HWND hwnd) {
    WCHAR cls[64]{};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"SHELLDLL_DefView") == 0 ||
           wcscmp(cls, L"ShellTabWindowClass") == 0 ||
           wcscmp(cls, L"CabinetWClass") == 0;
}

// Path of the single selected item in the shell view `defView`, or empty (no
// selection, multi-selection, or no browser). The returned IShellBrowser is
// not reference-counted by the message, so it is not released here.
std::wstring GetSelectedPath(HWND defView) {
    std::wstring result;
    IShellBrowser* browser = nullptr;
    for (HWND h = defView; !browser && h; h = GetParent(h)) {
        if (AnswersGetIShellBrowser(h)) {
            browser = (IShellBrowser*)SendMessageW(h, CWM_GETISHELLBROWSER, 0,
                                                   0);
        }
    }
    if (!browser) {
        return result;
    }

    IShellView* view = nullptr;
    if (FAILED(browser->QueryActiveShellView(&view)) || !view) {
        return result;
    }

    IDataObject* dataObj = nullptr;
    if (SUCCEEDED(view->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(&dataObj))) &&
        dataObj) {
        FORMATETC fmt{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM stg{};
        if (SUCCEEDED(dataObj->GetData(&fmt, &stg))) {
            if (HDROP drop = (HDROP)GlobalLock(stg.hGlobal)) {
                if (DragQueryFileW(drop, 0xFFFFFFFF, nullptr, 0) == 1) {
                    WCHAR buf[MAX_PATH]{};
                    DragQueryFileW(drop, 0, buf, ARRAYSIZE(buf));
                    result = buf;
                }
                GlobalUnlock(stg.hGlobal);
            }
            ReleaseStgMedium(&stg);
        }
        dataObj->Release();
    }
    view->Release();
    return result;
}

// `owner` is the Explorer window the menu came from: it owns the shell's
// progress and error dialogs, so they cannot end up behind it or ownerless.
// ponytail: SHFileOperation, not IFileOperation — the flags below cover undo
// and error reporting; move to IFileOperation if this ever needs per-item
// results or an elevation prompt.
bool MoveOrCopy(HWND owner,
                const std::wstring& src,
                const std::wstring& destDir,
                bool move) {
    if (destDir.empty()) {
        return false;
    }
    // pFrom/pTo need double-null termination.
    std::wstring from = src + L'\0';
    std::wstring to = destDir + L'\0';
    SHFILEOPSTRUCTW op{};
    op.hwnd = owner;
    op.wFunc = move ? FO_MOVE : FO_COPY;
    op.pFrom = from.c_str();
    op.pTo = to.c_str();
    // FOF_ALLOWUNDO: a mis-click on a menu that sits right next to Explorer's
    // own commands has to be undoable with Ctrl+Z, and a move without it is
    // permanent. No FOF_NOERRORUI either — access denied or a file in use is
    // the shell's error to report, and swallowing it looks like nothing
    // happened.
    op.fFlags =
        FOF_NOCONFIRMMKDIR | FOF_ALLOWUNDO | FOF_RENAMEONCOLLISION;
    return SHFileOperationW(&op) == 0 && !op.fAnyOperationsAborted;
}

// Creates a .lnk to `target` inside `destDir`. Returns the new .lnk path, or
// empty on failure.
std::wstring CreateShortcutIn(const std::wstring& target,
                              const std::wstring& destDir) {
    if (destDir.empty()) {
        return L"";
    }
    IShellLinkW* link = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_IShellLinkW, (void**)&link)) ||
        !link) {
        return L"";
    }
    link->SetPath(target.c_str());

    IPersistFile* persist = nullptr;
    if (FAILED(link->QueryInterface(IID_IPersistFile, (void**)&persist)) ||
        !persist) {
        link->Release();
        return L"";
    }

    std::wstring name = LeafName(target);
    std::wstring lnkPath = destDir + L"\\" + name + L".lnk";
    for (int i = 2; GetFileAttributesW(lnkPath.c_str()) != INVALID_FILE_ATTRIBUTES &&
                    i < 100;
         i++) {
        lnkPath = destDir + L"\\" + name + L" (" + std::to_wstring(i) + L").lnk";
    }

    HRESULT hr = persist->Save(lnkPath.c_str(), TRUE);
    persist->Release();
    link->Release();
    if (FAILED(hr)) {
        return L"";
    }
    // Writing the file is not enough: a window already showing destDir only
    // learns about the new shortcut from a change notification.
    SHChangeNotify(SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSH, lnkPath.c_str(),
                   nullptr);
    return lnkPath;
}

// Theme for the menu popups created while the CBT hook below is installed.
// A WH_CBT hook procedure has to be a plain function pointer, so the value it
// needs is parked here instead of captured.
std::atomic<bool> g_menuDark{true};

// An Explorer/Desktop context menu opened through this hook keeps a frame of
// this DLL live for as long as the menu is up (TrackPopupMenuEx_orig runs the
// menu's modal loop), and for part of that time a WH_CBT hook whose proc is also
// in this DLL is installed. Wh_ModUninit has to wait that out — or Windhawk
// FreeLibrary's the image and dismissing the menu returns into unmapped memory.
// Not hypothetical: Wh_ModSettingsChanged asks for a reload whenever
// explorerMenu changes.
//
// Every path through the hook is counted, including the ones that pass straight
// to the original — those block in the modal loop just the same, with the return
// address still inside this DLL. Note the frame outlives the menu: a chosen
// Move/Copy runs SHFileOperationW after TrackPopupMenuEx_orig has returned, so
// an entry can stay live for as long as that operation takes.
//
// One entry per live call rather than a count and a single owner: the owner is
// the hwnd handed to TrackPopupMenuEx, which belongs to the thread running that
// menu, and EndMenu only cancels the calling thread's menu — so uninit needs
// every owner, not whichever one entered last.
std::mutex g_explorerMenuMutex;
std::vector<HWND> g_explorerMenuOwners;

bool ExplorerMenuOwnersSnapshot(std::vector<HWND>* out) {
    std::lock_guard<std::mutex> lock(g_explorerMenuMutex);
    *out = g_explorerMenuOwners;
    return !out->empty();
}

decltype(&TrackPopupMenuEx) TrackPopupMenuEx_orig;

BOOL WINAPI TrackPopupMenuExHook(HMENU hMenu,
                                 UINT flags,
                                 int x,
                                 int y,
                                 HWND hwnd,
                                 LPTPMPARAMS params) {
    // Teardown is already waiting on every registered owner in
    // g_explorerMenuOwners (see Wh_ModUninit); injecting menu items this late
    // would just add one more owner to wait out for no benefit.
    if (g_unloading) {
        return TrackPopupMenuEx_orig(hMenu, flags, x, y, hwnd, params);
    }

    struct MenuActiveGuard {
        HWND owner;
        explicit MenuActiveGuard(HWND ownerWnd) : owner(ownerWnd) {
            std::lock_guard<std::mutex> lock(g_explorerMenuMutex);
            g_explorerMenuOwners.push_back(owner);
        }
        ~MenuActiveGuard() {
            std::lock_guard<std::mutex> lock(g_explorerMenuMutex);
            auto it = std::find(g_explorerMenuOwners.begin(),
                                g_explorerMenuOwners.end(), owner);
            if (it != g_explorerMenuOwners.end()) {
                g_explorerMenuOwners.erase(it);
            }
        }
    } menuActive(hwnd);

    // TPM_RETURNCMD is a precondition for injecting anything, not just for
    // reading the result below: without it the menu posts WM_COMMAND with the
    // chosen id to hwnd instead of returning it, so the mod's own action would
    // silently never run and an id in the kMinId range would be handed to a
    // window proc that may map an unrecognised command onto an IContextMenu
    // verb offset. CDefView passes the flag, but this hook is process-wide and
    // FindShellViewWindow only checks that hwnd sits under a SHELLDLL_DefView —
    // any other caller in explorer.exe that happens to satisfy that (a shell
    // extension with its own menu, a future shell build, another mod) must be
    // left alone.
    HWND defView =
        (flags & TPM_RETURNCMD) ? FindShellViewWindow(hwnd) : nullptr;
    if (!defView) {
        return TrackPopupMenuEx_orig(hMenu, flags, x, y, hwnd, params);
    }

    struct ComInit {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        ~ComInit() {
            if (SUCCEEDED(hr)) {
                CoUninitialize();
            }
        }
    } com;

    std::wstring path = GetSelectedPath(defView);
    // This runs on the Explorer browser thread with that window's context menu
    // waiting on us to return, and Explorer's own QueryContextMenu work is
    // already done — a GetFileAttributesW on an offline share would freeze the
    // whole window for the network timeout purely to decide whether to add our
    // item. Degrade to no submenu, same as everywhere else the mod meets a
    // remote path.
    if (path.empty() || IsLikelyRemotePath(path)) {
        return TrackPopupMenuEx_orig(hMenu, flags, x, y, hwnd, params);
    }
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        return TrackPopupMenuEx_orig(hMenu, flags, x, y, hwnd, params);
    }
    bool isLnk = PathIsLnkFile(path);
    bool isDir = (attrs & FILE_ATTRIBUTE_DIRECTORY) && !isLnk;

    std::vector<FolderEntry> folders;
    {
        std::lock_guard<std::mutex> lock(g_foldersMutex);
        folders = g_settings.folders;
    }

    // One "Taskbar Folders" item on the real menu: Pin (folders only, and only
    // if not already a taskbar folder), then Move/Copy (any non-folder) or
    // Copy as shortcut (folders and regular files), each hoverable into the
    // folder list. Submenus attach via MF_POPUP, so explorer.exe's own
    // DestroyMenu(hMenu) tears them all down — nothing here owns cleanup.
    size_t n = folders.size();
    UINT moveBase = kMinId + 1;
    UINT copyBase = moveBase + (UINT)n;
    UINT shortcutBase = copyBase + (UINT)n;

    // "Manage folders..." below is always offered regardless of this setting —
    // it is the only entrance to the manager when nothing is pinned yet. What
    // the setting actually suppresses is everything that writes into Explorer:
    // Pin / Move / Copy / Copy as shortcut.
    bool menuEnabled = g_settings.explorerMenu;
    bool canPin = menuEnabled && isDir && !IsPinnedTaskbarFolder(path);
    bool showMoveCopy = menuEnabled && !isDir && n > 0;
    bool showShortcut = menuEnabled && !isLnk && n > 0;

    auto buildDestinationSubmenu = [&](PCWSTR header, UINT idBase) {
        HMENU sub = CreatePopupMenu();
        AppendMenuW(sub, MF_STRING | MF_GRAYED | MF_DISABLED, 0, header);
        AppendMenuW(sub, MF_SEPARATOR, 0, nullptr);
        for (size_t i = 0; i < n; i++) {
            AppendMenuW(sub, MF_STRING, idBase + (UINT)i,
                        EscapeMenuLabel(folders[i].name).c_str());
        }
        return sub;
    };

    HMENU foldersMenu = CreatePopupMenu();
    bool anyItem = false;
    if (canPin) {
        std::wstring label =
            L"Pin \"" + EscapeMenuLabel(LeafName(path)) + L"\" to Taskbar";
        AppendMenuW(foldersMenu, MF_STRING, kMinId, label.c_str());
        anyItem = true;
        if (showMoveCopy || showShortcut) {
            AppendMenuW(foldersMenu, MF_SEPARATOR, 0, nullptr);
        }
    }
    if (showMoveCopy) {
        HMENU moveSub = buildDestinationSubmenu(L"Move to...", moveBase);
        AppendMenuW(foldersMenu, MF_POPUP, (UINT_PTR)moveSub, L"Move");
        HMENU copySub = buildDestinationSubmenu(L"Copy to...", copyBase);
        AppendMenuW(foldersMenu, MF_POPUP, (UINT_PTR)copySub, L"Copy");
        anyItem = true;
    }
    if (showShortcut) {
        HMENU shortcutSub =
            buildDestinationSubmenu(L"Make shortcut in...", shortcutBase);
        AppendMenuW(foldersMenu, MF_POPUP, (UINT_PTR)shortcutSub,
                    L"Copy as shortcut");
        anyItem = true;
    }
    // Always offered, and the only remaining way in. The folder buttons are real
    // taskbar items now, so their right-click menu belongs to the shell — it
    // gives a native "Unpin from taskbar" for free, but there is nowhere on it
    // to hang "Manage folders...". This entry is the replacement, and unlike the
    // old one it is reachable when nothing is pinned at all.
    if (anyItem) {
        AppendMenuW(foldersMenu, MF_SEPARATOR, 0, nullptr);
    }
    AppendMenuW(foldersMenu, MF_STRING, kIdManage, L"Manage folders...");
    anyItem = true;
    if (!anyItem) {
        DestroyMenu(foldersMenu);
    } else {
        // Sit right under "Pin to taskbar" so it reads as a related action,
        // falling back to right under "Give access to" when Explorer didn't
        // offer a pin item (e.g. the item is already pinned).
        int afterPos = FindMenuItemPositionByLabel(hMenu, L"Pin to taskbar");
        if (afterPos < 0) {
            afterPos = FindMenuItemPositionByLabel(hMenu, L"Give access to");
        }
        if (afterPos >= 0) {
            InsertMenuW(hMenu, afterPos + 1, MF_BYPOSITION | MF_POPUP,
                        (UINT_PTR)foldersMenu, L"Taskbar Folders");
        } else {
            if (GetMenuItemCount(hMenu) > 0) {
                AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            }
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)foldersMenu,
                        L"Taskbar Folders");
        }

        // Explorer applies dark-mode/background MENUINFO across the whole
        // menu tree it builds via SetMenuInfo(..., MIM_APPLYTOSUBMENUS)
        // before handing off to TrackPopupMenuEx. Our submenu is created
        // afterward, inside this hook, so it never inherits that pass —
        // copying the parent's MENUINFO onto it (with MIM_APPLYTOSUBMENUS so
        // it cascades down to Move/Copy/Copy as shortcut too) is what fixes
        // the classic light-mode square border that otherwise shows up only
        // on menus this mod injects.
        MENUINFO parentInfo{sizeof(parentInfo)};
        parentInfo.fMask = MIM_STYLE | MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
        if (GetMenuInfo(hMenu, &parentInfo)) {
            SetMenuInfo(foldersMenu, &parentInfo);
        }
    }

    // Submenus we inject (MF_POPUP with our own CreatePopupMenu() handles)
    // aren't part of explorer's IContextMenu tree, so explorer's own
    // dark-mode/rounded-corner treatment for nested popups skips them — they
    // render with the classic light-mode square border. Each nested popup
    // spawns a new "#32768" window while TrackPopupMenuEx_orig pumps its own
    // message loop below, so a scoped CBT hook catches all of them.
    //
    // The hook is thread-wide for the duration of that call, so it also sees
    // Explorer's own submenus (Open with, Send to, New) — forcing dark on
    // those would darken them on a light theme. Resolved once here rather than
    // inside the hook, which cannot capture and should not be doing WinRT work
    // while a menu is coming up.
    //
    // Scoped so the hook comes out even if TrackPopupMenuEx_orig unwinds
    // unexpectedly — its proc lives in this DLL and must not stay registered
    // once the menu is done with.
    g_menuDark = IsDarkTheme();
    UINT result;
    {
        struct CbtHookGuard {
            HHOOK hook = nullptr;
            ~CbtHookGuard() {
                if (hook) {
                    UnhookWindowsHookEx(hook);
                }
            }
        } cbtGuard;
        cbtGuard.hook = SetWindowsHookExW(
            WH_CBT,
            [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
                if (nCode == HCBT_CREATEWND) {
                    HWND hWnd = (HWND)wParam;
                    CBT_CREATEWNDW* cbt = (CBT_CREATEWNDW*)lParam;
                    LPCWSTR cls = cbt->lpcs->lpszClass;
                    if (IS_INTRESOURCE(cls) &&
                        LOWORD((ULONG_PTR)cls) == 32768) {
                        BOOL dark = g_menuDark.load() ? TRUE : FALSE;
                        DwmSetWindowAttribute(hWnd,
                                              DWMWA_USE_IMMERSIVE_DARK_MODE,
                                              &dark, sizeof(dark));
                        int corner = DWMWCP_ROUND;
                        DwmSetWindowAttribute(hWnd,
                                              DWMWA_WINDOW_CORNER_PREFERENCE,
                                              &corner, sizeof(corner));
                    }
                }
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            },
            nullptr, GetCurrentThreadId());

        result = TrackPopupMenuEx_orig(hMenu, flags, x, y, hwnd, params);
    }

    // TPM_RETURNCMD is guaranteed here — nothing is injected without it — so
    // result is the chosen command id, and anything below kMinId belongs to
    // Explorer.
    if (result < kMinId) {
        return result;
    }

    if (result == kIdManage) {
        FolderManager::Open();
        return 0;
    }

    if (canPin && result == kMinId) {
        AddPinnedFolder(path, LeafName(path));
        // Deferred: this is the UI thread of the Explorer window the user just
        // right-clicked in, and a reload joins the scan worker, which can sit
        // in a slow shell icon handler for a while.
        RequestReloadUI();
        return 0;
    }

    bool move = false;
    bool asShortcut = false;
    size_t idx = 0;
    if (result >= moveBase && result < moveBase + n) {
        move = true;
        idx = result - moveBase;
    } else if (result >= copyBase && result < copyBase + n) {
        move = false;
        idx = result - copyBase;
    } else if (result >= shortcutBase && result < shortcutBase + n) {
        move = false;
        asShortcut = true;
        idx = result - shortcutBase;
    } else {
        return result;
    }

    std::wstring destDir = folders[idx].resolvedPath.empty() ? folders[idx].path
                                                              : folders[idx].resolvedPath;
    if (asShortcut) {
        CreateShortcutIn(path, destDir);
        return 0;
    }
    MoveOrCopy(hwnd, path, destDir, move);
    return 0;
}

}  // namespace AddToTaskbar

////////////////////////////////////////////////////////////////////////////////
// The jump list's "Manage folders..." task
//
// The shell launches a jump list task as an ordinary command line, and the
// manager window lives in this process, so the launch is caught here instead of
// being allowed to start anything. CreateProcessW is the choke point: whatever
// the shell does above it — ShellExecute, link resolution, its own launcher —
// it has to come through here to start a process.

decltype(&CreateProcessW) CreateProcessW_Original;

BOOL WINAPI CreateProcessW_Hook(LPCWSTR applicationName,
                                LPWSTR commandLine,
                                LPSECURITY_ATTRIBUTES processAttributes,
                                LPSECURITY_ATTRIBUTES threadAttributes,
                                BOOL inheritHandles,
                                DWORD creationFlags,
                                LPVOID environment,
                                LPCWSTR currentDirectory,
                                LPSTARTUPINFOW startupInfo,
                                LPPROCESS_INFORMATION processInformation) {
    // Deliberately the cheapest possible test, and first: explorer.exe starts a
    // lot of processes and this hook is on all of them.
    if (commandLine && wcsstr(commandLine, Pins::kManageSentinel)) {
        Wh_Log(L"Opening the folder manager from the jump list");
        FolderManager::Open();
        // Nothing is launched. Reported as a cancelled launch rather than a
        // failure so the shell treats it as the user backing out, which it does
        // silently, instead of showing an error for a task that did exactly
        // what it was asked to.
        SetLastError(ERROR_CANCELLED);
        return FALSE;
    }

    return CreateProcessW_Original(applicationName, commandLine,
                                   processAttributes, threadAttributes,
                                   inheritHandles, creationFlags, environment,
                                   currentDirectory, startupInfo,
                                   processInformation);
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    // LoadFolders ends with Pins::RequestReconcile, which is a no-op with no
    // worker yet. Pins::Start() below kicks a reconcile itself once it starts
    // the thread, so nothing depends on starting the worker this early -- and
    // starting it late means a return FALSE below leaves no thread running in
    // an image Windhawk is about to unmap.
    LoadSettings();
    ResetFolderData();

    Gdiplus::GdiplusStartupInput startupInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) !=
        Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed");
        return FALSE;
    }

    // Only the hover half needs these symbols, and they are the part a Windows
    // update breaks. Everything else — the folder store, the pin reconcile, the
    // Explorer menu, the jump-list verb — does not, so carry on without them
    // rather than returning FALSE. Returning FALSE here would strand the pinned
    // buttons from the last working session with no in-mod way to remove them:
    // no manager entry, no jump-list handler, and no disable-time sweep either,
    // since Wh_ModUninit only runs for a mod that loaded. Every symbol below is
    // null-checked at its call sites, and the hooks that bind hover simply never
    // install.
    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"Failed to hook taskbar.dll, the taskbar XamlRoot is "
               L"unreachable; hover will not bind, the folder buttons and the "
               L"manager still work");
    }

    Pins::Start();

    // Always hooked: TrackPopupMenuExHook reads g_settings.explorerMenu live
    // on every call and uses it only to suppress the Pin / Move / Copy items,
    // never the "Manage folders..." entry — that stays reachable even with
    // the setting off, since it is the only way into the manager from a
    // standing start. Hooking unconditionally also means the setting takes
    // effect immediately, with no reload needed.
    if (!WindhawkUtils::SetFunctionHook(
            TrackPopupMenuEx, AddToTaskbar::TrackPopupMenuExHook,
            &AddToTaskbar::TrackPopupMenuEx_orig)) {
        // Non-fatal: the rest of the mod works without the right-click
        // integration.
        Wh_Log(L"Failed to hook TrackPopupMenuEx; Explorer right-click "
               L"integration will be unavailable");
    }

    // Catches the "Manage folders..." jump list task. Every launch path ends at
    // CreateProcessW, whatever the shell does above it, so this is the one
    // place that cannot be missed. Non-fatal: without it the task falls through
    // and opens an Explorer window instead of the manager.
    //
    // Resolved from kernelbase.dll rather than linked via kernel32's
    // CreateProcessW: the implementation lives in kernelbase, and callers that
    // bind through the API set (api-ms-win-core-processthreads-l1-*) reach it
    // directly, bypassing the kernel32 export this hook would otherwise patch.
    auto pCreateProcessW = (decltype(&CreateProcessW))GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"), "CreateProcessW");
    if (!pCreateProcessW || !WindhawkUtils::SetFunctionHook(
                                 pCreateProcessW, CreateProcessW_Hook,
                                 &CreateProcessW_Original)) {
        Wh_Log(L"Failed to hook CreateProcessW; the jump list's Manage "
               L"folders task will not open the manager");
    }

    // Scan thread starts lazily on the first RequestScan (hover / inject).
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");
    // Menu owner / popup classes are created on the taskbar UI thread during
    // InjectHostGridsOnTaskbarThread (and EnsureLevelWindow), not here.
    StartRetryThread();
}

// Tears down UI, reloads settings (+ pinned folders), and restarts. Shared by
// Wh_ModSettingsChanged and the Explorer right-click "pin to taskbar" action.
void ReloadAndRefreshUI() {
    struct InFlightGuard {
        InFlightGuard() { g_reloadInFlight++; }
        ~InFlightGuard() { g_reloadInFlight--; }
    } inFlight;

    StopRetryThread();
    // The scan thread reads g_settings, so it has to be parked before
    // reloading.
    StopScanThread();

    if (!ApplyOnWindowThread([](void*) {
            CloseChain();
            RemoveAllHostGrids();
            // Popup HWNDs keep the DWM corner/border attributes ApplyBackdrop
            // set at create time; destroy them so the next hover recreates
            // with the current settings. Blur strength itself is re-read
            // every PresentLevel and needs no recreation.
            for (HWND hWnd : g_levelWindows) {
                if (hWnd) {
                    DestroyWindow(hWnd);
                }
            }
            g_levelWindows.clear();
        })) {
        Wh_Log(L"ReloadAndRefreshUI: failed to tear down UI on taskbar thread");
    }

    LoadSettings();
    ResetFolderData();

    // Scan thread restarts lazily on the next RequestScan.
    StartRetryThread();

    // Every store change routes through here — the Explorer pin, the taskbar
    // unpin, a settings reload — so this is where an open manager window finds
    // out its list is stale. Without it the window keeps showing (and
    // committing against) rows that no longer match the store.
    //
    // Since reloads run on the taskbar UI thread, this always posts when the
    // manager is open — including for the manager's own edits, which already
    // repopulated synchronously. The extra pass is a no-op beyond a redraw:
    // PopulateList reads the same store and keeps the selected index.
    if (HWND manager = FolderManager::g_wnd.load()) {
        if (GetWindowThreadProcessId(manager, nullptr) !=
            GetCurrentThreadId()) {
            PostMessageW(manager, WM_APP, 0, 0);
        }
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"SettingsChanged");

    RequestReloadUI();

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    g_unloading = true;
    // Before anything is torn down: the manager runs its own message loop on
    // its own thread, and unloading the DLL under it would crash Explorer.
    FolderManager::CloseAndWait();
    StopRetryThread();
    StopScanThread();
    // Disabling (or uninstalling — Wh_ModUninit does not see a difference)
    // must leave the system as it was: real taskbar pins, Start Menu
    // shortcuts and jump lists this mod created do not get to outlive it.
    // Stop the reconcile worker, but run the disable-time unpin sweep as its
    // very last action before it joins: it can be mid-Reconcile() (pinned-dir
    // watch, Taskband watch, or the periodic poll — the unpins below even
    // fire the dir watch) and would otherwise re-create pins and Start Menu
    // shortcuts the sweep just removed. Running the sweep there, rather than
    // here on this arbitrary Windhawk engine thread, also guarantees it runs
    // on a real STA — the pin verb's COM class has no marshaler, so every
    // InvokeVerb from a thread that isn't one silently fails and leaves every
    // pinned button, Start Menu shortcut and jump list behind.
    Pins::Stop(&FolderManager::UnpinAllForDisable);

    // DestroyWindow / XAML teardown must run on the creating UI thread. Prefer
    // a mod-owned window (level popup or menu owner) when the taskbar HWND is
    // already gone — never DestroyWindow from this Windhawk callback thread.
    auto teardownOnUiThread = [](void*) {
        ClearPendingShellCommand();
        CloseChain();
        DestroyItemTooltip();
        RemoveAllHostGrids();

        for (HWND hWnd : g_levelWindows) {
            if (hWnd) {
                DestroyWindow(hWnd);
            }
        }
        g_levelWindows.clear();

        if (g_menuOwnerWnd) {
            DestroyWindow(g_menuOwnerWnd);
            g_menuOwnerWnd = nullptr;
        }

        g_taskbarHosts.reset();
    };

    // Every one of these waits is unbounded on purpose, for the reason
    // FolderManager::CloseAndWait spells out: Wh_ModUninit runs on a Windhawk
    // engine thread, not on any Explorer UI thread, so blocking here does not
    // freeze the shell — while falling through early is exactly what unmaps the
    // image with one of these frames still executing inside it. Cancellation is
    // attempted for the first couple of seconds only; past that, whatever is
    // still live is work that has to finish on its own (a folder copy can run
    // for minutes) and re-asking every 20ms is pointless.
    constexpr int kUninitWaitPollMs = 50;
    constexpr int kUninitCancelForMs = 2000;
    constexpr int kUninitLogEveryMs = 5000;
    auto waitOut = [](PCWSTR what, auto stillLive, auto attemptCancel) {
        for (int elapsedMs = 0; stillLive(); elapsedMs += kUninitWaitPollMs) {
            if (elapsedMs < kUninitCancelForMs) {
                attemptCancel();
            } else if (elapsedMs % kUninitLogEveryMs == 0) {
                Wh_Log(L"Uninit: still waiting on %s (%d s)", what,
                       elapsedMs / 1000);
            }
            Sleep(kUninitWaitPollMs);
        }
    };

    // First, before any window handle is picked: a reload already past
    // MenuOwnerWndProc's g_unloading check destroys every level window and
    // clears g_levelWindows, so a handle latched ahead of this wait would be
    // dead by the time it is used — and RunFromWindowThread would then skip the
    // teardown entirely, leaving g_menuOwnerWnd (which no reload destroys) alive
    // with its WndProc in the image about to be unmapped. The reload also parks
    // the taskbar UI thread in joins that pump sent messages, so waiting here
    // keeps the teardown send below from running re-entrantly inside one.
    // Nothing to cancel, and it cannot restart a worker behind us:
    // StartRetryThread bails on g_unloading.
    waitOut(L"a UI reload", [] { return g_reloadInFlight.load() > 0; }, [] {});

    // g_menuOwnerWnd first: it lives on the same taskbar UI thread and nothing
    // but the teardown itself destroys it, so it is the stable choice. A level
    // window and then the taskbar itself are the fallbacks.
    HWND threadWnd = g_menuOwnerWnd;
    if (!threadWnd) {
        for (HWND h : g_levelWindows) {
            if (h) {
                threadWnd = h;
                break;
            }
        }
    }
    if (!threadWnd) {
        threadWnd = FindCurrentProcessTaskbarWnd();
    }

    // ShowItemContextMenu runs a modal TrackPopupMenuEx loop on the UI thread.
    // Teardown via RunFromWindowThread would destroy windows and let Windhawk
    // unload the DLL while that frame is still live — EndMenu/CANCELMODE first,
    // then wait for g_menuActive to clear (same pattern as taskbar-folder-menus).
    if (threadWnd) {
        waitOut(
            L"a shell context menu", [] { return g_menuActive.load(); },
            [threadWnd] {
                RunFromWindowThread(
                    threadWnd,
                    [](void*) {
                        if (g_menuActive && !EndMenu() && g_menuOwnerWnd) {
                            SendMessageW(g_menuOwnerWnd, WM_CANCELMODE, 0, 0);
                        }
                    },
                    nullptr);
            });
    }

    // Same problem, other menu: a classic Explorer/Desktop context menu opened
    // through TrackPopupMenuExHook is pumping its modal loop inside a frame of
    // this DLL, with no mod-owned window involved. EndMenu only cancels the
    // calling thread's menu, so it has to be driven onto each owner window's
    // thread — those are Explorer windows the user right-clicked in, not the
    // taskbar. A live entry can also be post-menu work (SHFileOperationW for a
    // Move/Copy), which no amount of EndMenu will cut short.
    std::vector<HWND> menuOwners;
    waitOut(
        L"an Explorer context menu",
        [&menuOwners] {
            return AddToTaskbar::ExplorerMenuOwnersSnapshot(&menuOwners);
        },
        [&menuOwners] {
            for (HWND owner : menuOwners) {
                if (!IsWindow(owner)) {
                    continue;
                }
                RunFromWindowThread(
                    owner,
                    [](void* param) {
                        if (!EndMenu()) {
                            SendMessageW((HWND)param, WM_CANCELMODE, 0, 0);
                        }
                    },
                    owner);
            }
        });

    // Also wait out a deferred/synchronous shell verb. ASYNCOK usually makes
    // this short; verbs that ignore it and show modal UI take as long as the
    // user does — see CMIC_MASK_ASYNCOK on the invoke path. Nothing to cancel.
    waitOut(L"a shell verb", [] { return g_invokeActive.load(); }, [] {});

    // The waits above run for as long as the user does, so re-validate rather
    // than trust a handle picked before them. The teardown is the one thing
    // that must not be skipped.
    if (threadWnd && !IsWindow(threadWnd)) {
        threadWnd = FindCurrentProcessTaskbarWnd();
    }

    if (threadWnd) {
        if (!RunFromWindowThread(threadWnd, teardownOnUiThread, nullptr)) {
            Wh_Log(L"Uninit: RunFromWindowThread failed (error %u); windows "
                   L"may leak and UnregisterClass may fail",
                   GetLastError());
        }
    } else {
        // No mod-owned or taskbar window — nothing left that can dangle into
        // unmapped code; retain no_destroy host state rather than tearing it
        // down from the wrong thread.
        Wh_Log(L"No mod-owned or taskbar window at uninit; retaining host state");
    }

    g_levels.reset();
    {
        std::lock_guard<std::mutex> lock(g_cacheMutex);
        g_folderCache.reset();
    }

    // The window procedures live in this DLL, so the classes must not outlive
    // it. A class left behind keeps pointing at the unloaded image, and the
    // next load would build a window on a dangling procedure.
    HINSTANCE instance = GetCurrentModuleHandle();
    UnregisterClassW(kPopupClassName, instance);
    UnregisterClassW(kMenuOwnerClassName, instance);
    UnregisterClassW(FolderManager::kClassName, instance);
    UnregisterClassW(FolderManager::kEditClassName, instance);

    // The manager thread is joined by now, so nothing has these selected into a
    // DC. DeleteObject is a no-op on the DEFAULT_GUI_FONT stock fallback.
    for (auto& [dpi, font] : FolderManager::g_fontCache) {
        DeleteObject(font);
    }
    FolderManager::g_fontCache.clear();

    // Same reasoning for the dark-mode background brushes.
    for (HBRUSH* brush :
         {&FolderManager::g_windowBrush, &FolderManager::g_faceBrush}) {
        if (*brush) {
            DeleteObject(*brush);
            *brush = nullptr;
        }
    }

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

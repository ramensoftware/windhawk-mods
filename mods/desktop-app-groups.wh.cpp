// ==WindhawkMod==
// @id              desktop-app-groups
// @name            Desktop App Groups
// @description     Group desktop icons into Start-menu-style folders by dragging one onto another
// @version         1.0
// @author          Markc719
// @github          https://github.com/Markc719
// @homepage        https://github.com/Markc719/desktop-app-groups
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luuid -lole32 -lshell32 -lshlwapi -lcomctl32 -lgdi32 -lmsimg32 -ldwmapi -lshcore -ladvapi32 -luser32 -lcomdlg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop App Groups

Group your desktop icons into folders that work like Start menu folders.
Drag one app onto another to make a group, then click the group to open it
in a popup beside it.

![A group open on the desktop](https://raw.githubusercontent.com/Markc719/desktop-app-groups/main/Screenshot%202026-11-09%20103044.png)

![The group settings panel](https://raw.githubusercontent.com/Markc719/desktop-app-groups/main/Screenshot%202026-11-09%20103051.png)

![A group's icon on the desktop](https://raw.githubusercontent.com/Markc719/desktop-app-groups/main/Screenshot%202026-11-09%20103033.png)

## Features

- **Make a group:** drag an app onto another app, hold it there for a
  moment, and let go. The new group appears in that spot, and its icon
  shows a preview of the first four apps inside.
- **Open a group:** click it. A popup opens beside it with a blurred
  background, growing out of the group's icon like a Start menu folder.
  Click an app to launch it. Click elsewhere, press Esc, or click the group
  again to close it.
- **Add apps:** drop more apps onto the group.
- **Rearrange apps:** drag them around inside the popup. The group's icon
  shows the first four.
- **Take an app out:** drag it past the edge of the popup, onto the desktop
  or anywhere else you can drop files. A group that's down to its last app
  is removed, and the app goes back where the group was.
- **Rename a group:** click its name at the top of the popup, or press F2.
- **Right-click** an app in the popup for its usual menu: Open file
  location, Pin to taskbar, Properties and so on. Shift+right-click shows
  extra options. Right-click the group's name for the group's own menu.
- **Colour-code groups:** give a group a colour, from eight presets or any
  colour you pick. Its desktop icon and its popup are tinted to match, so
  groups are easy to tell apart at a glance.
- **Group settings:** the gear button next to the group's name opens a panel
  with apps per row, icon size, app names, colour, sort order (your
  arrangement or A to Z), and whether a group is removed when one app is
  left. Each can be set for that group or for all groups.

## How groups are stored

A group is an ordinary folder on your desktop with your shortcuts in it, so
if you uninstall the mod, nothing is lost. Inside it are two hidden files
that Windows needs to show the group's icon: the icon itself and a
`desktop.ini` that points to it.

The mod's own data for each group (that it's a group, your app order and
the group's own settings) is kept in Windhawk's storage on this PC. If you'd
like groups to keep that data when your desktop is synced to another PC,
copied or backed up, turn on "Keep group data inside the group folders" in
the Settings tab: it's then kept in each group's `desktop.ini` instead (see
the setting's description for the side effects).

## Uninstalling

Groups are normal folders, so they stay on your desktop with your shortcuts
in them. Each one keeps what Windows needs to show its custom icon: its last
group icon, the hidden `desktop.ini` and `group-icon-....ico` files inside
it, and the folder's read-only flag (which, for folders, only tells Windows
to look for a `desktop.ini`; it doesn't make anything read-only). If you
turned on "Keep group data inside the group folders", each `desktop.ini`
also still has the group's app order and settings.

To turn a group back into a plain folder, either move the shortcuts out and
delete the folder, or:

1. Right-click the group, then Properties, then Customize, then Change Icon,
   and click Restore Defaults.
2. In File Explorer, turn on View, then Show, then Hidden items, open the
   folder, and delete `desktop.ini` and the `group-icon-....ico` file.

## Tips

- Ctrl+click a group to select it without opening it. Right-click it for
  the usual folder menu, including Open to see it as a normal folder.
- Ctrl+Z on the desktop undoes creating or renaming a group.
- With "Auto arrange icons" turned on (right-click the desktop, then View),
  Windows decides where icons go, so new groups appear in the next free spot.
- Some installers put shortcuts on the shared Public desktop, for all users.
  Moving one of those into a group needs administrator permission, and
  Windows will ask for it.

## Settings

In this Settings tab:

- **How to create a group:** hover over an icon for a moment before
  dropping (the default), hold Shift while dropping, or always.
- **Hover time** for the hover option.
- **Open groups with** a single click (the default, like the Start menu) or
  a double-click (like normal folders).
- **Blurred popup background.** The popup uses a solid background if
  Windows' Transparency effects setting is off.
- **Animate the popup** when it opens and closes.
- **Keep group data inside the group folders:** off by default. Turn it on
  if your desktop is synced between PCs (for example with OneDrive) and you
  want your groups to stay groups everywhere.

Layout and behaviour settings for groups are in each popup's gear panel.

Made for Windows 11.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- groupTrigger: hover
  $name: How to create a group
  $options:
  - hover: Hover over an icon for a moment, then drop
  - shift: Hold Shift while dropping
  - always: Always group when dropping onto an icon
- hoverDelay: 600
  $name: Hover time (milliseconds)
  $description: Only used with the "hover" option. 1000 = one second.
- openWith: single
  $name: Open groups with
  $options:
  - single: A single click, like Start menu folders
  - double: A double-click, like normal folders
- blurredPopup: true
  $name: Blurred popup background
  $description: A see-through, blurred background like the Start menu's. Uses a solid background instead if Windows' Transparency effects setting is off.
- animatePopup: true
  $name: Animate the popup
  $description: Grow out of the group's icon when opening, and shrink back into it when closing, like Start menu folders.
- groupDataInFolder: false
  $name: Keep group data inside the group folders (for syncing)
  $description: "Off (recommended): each group's app order, its own settings and its group status are kept in Windhawk's storage on this PC. On: they're kept in the hidden desktop.ini file inside each group instead, so they go with the folder when your desktop is synced (for example by OneDrive), copied or backed up, and the folders are still groups on another PC or after reinstalling the mod. Side effects of turning it on: the mod writes to that file every time you rearrange a group or change its settings, so syncing services upload it again each time; and the data stays in the folders if you uninstall the mod. Changing this moves your existing groups' data over."
*/
// ==/WindhawkModSettings==

// -----------------------------------------------------------------------------
// HOW THIS FILE IS ORGANISED
//
// 1. Settings      - values the user can change in Windhawk's Settings tab
// 2. Finding       - locating the desktop's icon list inside Explorer
// 3. Reading       - asking that list for icon names, positions, selection
// 4. Desktop icons as files - finding the real file behind each icon
// 5. Groups        - creating groups, recognising them, listing their apps
// 6. Group icons   - drawing the 2x2 preview icons
// 7. Watching groups - updating icons, removing one-app groups
// 8. Watching drags - spotting icon-onto-icon drops and making groups
// 9. The group popup - the Start-menu-style window, and its settings panel
// 10. Opening groups - catching double-clicks on groups
// 11. Worker thread - finds the desktop in the background at startup
// 12. Windhawk callbacks - the functions Windhawk calls to start/stop the mod
// -----------------------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>        // GET_X_LPARAM / GET_Y_LPARAM
#include <commctrl.h>        // list-view messages (LVM_...)
#include <ole2.h>            // drag and drop (DoDragDrop, IDropSource)
#include <shlobj.h>          // shell folders and known folders (the Desktop path)
#include <shobjidl.h>        // IFolderView, IFileOperation, IShellItemImageFactory
#include <exdisp.h>          // IShellWindows
#include <shlwapi.h>         // StrRetToBufW, StrCmpLogicalW
#include <dwmapi.h>          // rounded corners for the popup
#include <shellscalingapi.h> // GetDpiForMonitor
#include <sddl.h>            // ConvertSidToStringSidW: your user account's ID as text
#include <commdlg.h>         // ChooseColorW: Windows' colour picker
#include <wincodec.h>        // WIC: saving pictures as PNG
#include <wrl/client.h>      // ComPtr: releases COM objects for us automatically

#include <windhawk_utils.h>  // WindhawkUtils::SetWindowSubclassFromAnyThread

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

// ---- Staying safe while unloading -------------------------------------------
//
// Windhawk can unload the mod at any time (when you disable or update it).
// Some of the mod's code can be in the middle of running then: a right-click
// menu, the colour picker, a drag, or a "Do you want to allow this?" prompt
// all wait for you inside the mod's code. If the mod's code were removed from
// Explorer while that's happening, Explorer would crash when it returned.
//
// So every entry point that Windows calls into (window message handlers, and
// the DoDragDrop hook) counts itself in and out with a BusyScope, and
// Wh_ModUninit waits until the count is back to zero before letting the mod
// be unloaded.

std::atomic<int> g_busyCount{0};

struct BusyScope {
    BusyScope() { g_busyCount++; }
    ~BusyScope() { g_busyCount--; }  // Runs when the function returns, however it returns.
};

// ---- 1. Settings ------------------------------------------------------------

enum class GroupTrigger { Hover, Shift, Always };

struct {
    GroupTrigger trigger;
    int hoverDelayMs;
    bool openWithSingleClick;
    bool blurredPopup;
    bool animatePopup;
} g_settings;

// Whether each group's data is kept in its desktop.ini (true) or in
// Windhawk's storage (false). See section 5. "atomic" because the group
// watcher thread reads it too.
std::atomic<bool> g_dataInFolder{false};

void LoadSettings() {
    // Text settings must be freed after reading them.
    PCWSTR trigger = Wh_GetStringSetting(L"groupTrigger");
    if (wcscmp(trigger, L"shift") == 0) {
        g_settings.trigger = GroupTrigger::Shift;
    } else if (wcscmp(trigger, L"always") == 0) {
        g_settings.trigger = GroupTrigger::Always;
    } else {
        g_settings.trigger = GroupTrigger::Hover;
    }
    Wh_FreeStringSetting(trigger);

    g_settings.hoverDelayMs = Wh_GetIntSetting(L"hoverDelay");
    if (g_settings.hoverDelayMs < 0) {
        g_settings.hoverDelayMs = 0;
    }

    // Booleans come back from Windhawk as 0 or 1.
    PCWSTR openWith = Wh_GetStringSetting(L"openWith");
    g_settings.openWithSingleClick = wcscmp(openWith, L"double") != 0;
    Wh_FreeStringSetting(openWith);

    g_settings.blurredPopup = Wh_GetIntSetting(L"blurredPopup") != 0;
    g_settings.animatePopup = Wh_GetIntSetting(L"animatePopup") != 0;
    g_dataInFolder = Wh_GetIntSetting(L"groupDataInFolder") != 0;
}

// ---- 2. Finding the desktop icon list ---------------------------------------
//
// The desktop is made of nested windows:
//
//   Progman (or sometimes a "WorkerW" window)   <- the desktop itself
//     └─ SHELLDLL_DefView                       <- Explorer's folder view
//          └─ SysListView32                     <- the list of icons

// Once found, these are remembered here. "atomic" makes it safe to set them on
// one thread (the worker) and read them on another (the desktop's).
std::atomic<HWND> g_desktopList{nullptr};
std::atomic<HWND> g_desktopDefView{nullptr};

// The search can end three ways: found it, "not yet" (try again shortly), or
// "never" (this explorer.exe doesn't own the desktop, so stop looking).
enum class SearchResult { Found, NotYet, NotThisProcess };

// When `explain` is true, the function writes to the log why it couldn't
// find the list. Messages like these are the main way to debug a mod.
SearchResult FindDesktopListView(HWND* listOut, bool explain) {
    // GetShellWindow() returns the main desktop window (Progman).
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        if (explain) Wh_Log(L"Step 1 failed: the desktop window doesn't exist yet");
        return SearchResult::NotYet;
    }

    // Windhawk loads this mod into every explorer.exe. Only one of them owns
    // the desktop, so the others can stop here.
    DWORD ownerProcessId = 0;
    GetWindowThreadProcessId(shellWindow, &ownerProcessId);
    if (ownerProcessId != GetCurrentProcessId()) {
        Wh_Log(L"The desktop belongs to explorer.exe process %u, not this one. "
               L"Nothing to do here.", (unsigned)ownerProcessId);
        return SearchResult::NotThisProcess;
    }

    HWND defView = FindWindowExW(shellWindow, nullptr, L"SHELLDLL_DefView", nullptr);

    if (!defView) {
        // Windows sometimes moves the icons into a "WorkerW" window (for
        // example after Win+Tab or with a wallpaper slideshow). Depending on
        // the Windows version, WorkerW is either a top-level window or a
        // child of Progman, so check both places.
        HWND parents[] = {nullptr, shellWindow};
        for (HWND parent : parents) {
            HWND workerW = nullptr;
            while (!defView &&
                   (workerW = FindWindowExW(parent, workerW, L"WorkerW", nullptr))) {
                defView = FindWindowExW(workerW, nullptr, L"SHELLDLL_DefView", nullptr);
            }
            if (defView) {
                break;
            }
        }
    }

    if (!defView) {
        if (explain) Wh_Log(L"Step 2 failed: found the desktop window but not "
                            L"SHELLDLL_DefView inside it");
        return SearchResult::NotYet;
    }

    HWND list = FindWindowExW(defView, nullptr, L"SysListView32", nullptr);
    if (!list) {
        if (explain) Wh_Log(L"Step 3 failed: found SHELLDLL_DefView but not "
                            L"SysListView32 inside it");
        return SearchResult::NotYet;
    }

    *listOut = list;
    return SearchResult::Found;
}

// ---- 3. Reading the icons ---------------------------------------------------
//
// Windows controls are driven by "messages". To ask the list how many icons
// it has, we send it LVM_GETITEMCOUNT; to get an icon's name, LVM_GETITEMTEXTW.
// We use SendMessageTimeoutW so that if Explorer is busy, we give up after a
// second instead of freezing.

bool SendToList(HWND list, UINT message, WPARAM wParam, LPARAM lParam,
                LRESULT* result = nullptr) {
    DWORD_PTR value = 0;
    if (!SendMessageTimeoutW(list, message, wParam, lParam,
                             SMTO_ABORTIFHUNG, 1000, &value)) {
        return false;
    }
    if (result) {
        *result = (LRESULT)value;
    }
    return true;
}

std::wstring GetIconName(HWND list, int index) {
    WCHAR name[MAX_PATH] = L"";

    LVITEMW item = {};
    item.iSubItem = 0;
    item.pszText = name;
    item.cchTextMax = ARRAYSIZE(name);
    SendToList(list, LVM_GETITEMTEXTW, index, (LPARAM)&item);

    return name;
}

// Returns the numbers of all currently selected icons. When you drag, the
// selected icons are the ones being dragged.
std::vector<int> GetSelectedIcons(HWND list) {
    std::vector<int> selected;
    LRESULT index = -1;
    while (SendToList(list, LVM_GETNEXTITEM, (WPARAM)index, LVNI_SELECTED, &index) &&
           index != -1) {
        selected.push_back((int)index);
    }
    return selected;
}

// Returns the number of the icon under the mouse pointer, or -1 if the
// pointer is over empty desktop.
int GetIconUnderCursor(HWND list) {
    LVHITTESTINFO hit = {};
    GetCursorPos(&hit.pt);           // Screen coordinates...
    ScreenToClient(list, &hit.pt);   // ...converted to the list's own.

    LRESULT index = -1;
    SendToList(list, LVM_HITTEST, 0, (LPARAM)&hit, &index);
    if (index >= 0 && (hit.flags & LVHT_ONITEM)) {
        return (int)index;
    }
    return -1;
}

void LogDesktopIcons(HWND list) {
    LRESULT count = 0;
    if (SendToList(list, LVM_GETITEMCOUNT, 0, 0, &count)) {
        Wh_Log(L"Found the desktop (%d icons)", (int)count);
    } else {
        Wh_Log(L"The desktop didn't respond");
    }
}

// ---- 4. Desktop icons as files ----------------------------------------------
//
// The icon list only knows each icon's label, like "Car Mechanic Simulator
// 2021". To move things we need the real file behind each icon, like
// "C:\Users\Mark\Desktop\Car Mechanic Simulator 2021.lnk".
//
// Explorer offers an official way to ask: the desktop's "folder view"
// (IFolderView). We reach it through IShellWindows, which lists every Explorer
// window, including the desktop.
//
// Objects like these are "COM objects". You use them through pointers, and
// must "Release" each one when finished. ComPtr does the releasing for us.

struct DesktopItem {
    std::wstring name;        // The label under the icon.
    std::wstring path;        // Full path on disk. Empty for Recycle Bin, This PC, etc.
    bool isDirectory = false; // True for folders (including groups).
};

ComPtr<IFolderView> GetDesktopFolderView() {
    ComPtr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&shellWindows)))) {
        return nullptr;
    }

    // Ask for "the window showing the desktop".
    VARIANT location = {};
    location.vt = VT_I4;
    location.lVal = CSIDL_DESKTOP;
    VARIANT empty = {};
    empty.vt = VT_EMPTY;
    long windowHandle = 0;
    ComPtr<IDispatch> desktopWindow;
    if (shellWindows->FindWindowSW(&location, &empty, SWC_DESKTOP, &windowHandle,
                                   SWFO_NEEDDISPATCH, &desktopWindow) != S_OK ||
        !desktopWindow) {
        return nullptr;
    }

    // Window -> browser -> current view -> folder view.
    ComPtr<IServiceProvider> serviceProvider;
    ComPtr<IShellBrowser> browser;
    ComPtr<IShellView> view;
    ComPtr<IFolderView> folderView;
    if (FAILED(desktopWindow.As(&serviceProvider)) ||
        FAILED(serviceProvider->QueryService(SID_STopLevelBrowser,
                                             IID_PPV_ARGS(&browser))) ||
        FAILED(browser->QueryActiveShellView(&view)) ||
        FAILED(view.As(&folderView))) {
        return nullptr;
    }
    return folderView;
}

// Reads one item of the folder view: its label, and if it's a real file or
// folder, its path.
bool ReadFolderViewItem(IFolderView* folderView, int index, DesktopItem* out) {
    ComPtr<IShellFolder> folder;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(&folder)))) {
        return false;
    }

    // An "item ID" is how Explorer identifies an item, file or not.
    PITEMID_CHILD itemId = nullptr;
    if (FAILED(folderView->Item(index, &itemId))) {
        return false;
    }

    WCHAR buffer[MAX_PATH] = L"";
    STRRET text;

    if (SUCCEEDED(folder->GetDisplayNameOf(itemId, SHGDN_INFOLDER, &text))) {
        StrRetToBufW(&text, itemId, buffer, ARRAYSIZE(buffer));
    }
    out->name = buffer;

    // Is it a real file or folder on disk?
    PCUITEMID_CHILD items[] = {itemId};
    SFGAOF attributes = SFGAO_FILESYSTEM;
    folder->GetAttributesOf(1, items, &attributes);

    out->path.clear();
    out->isDirectory = false;
    if ((attributes & SFGAO_FILESYSTEM) &&
        SUCCEEDED(folder->GetDisplayNameOf(itemId, SHGDN_FORPARSING, &text))) {
        StrRetToBufW(&text, itemId, buffer, ARRAYSIZE(buffer));
        out->path = buffer;

        DWORD fileAttributes = GetFileAttributesW(buffer);
        out->isDirectory = fileAttributes != INVALID_FILE_ATTRIBUTES &&
                           (fileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    CoTaskMemFree(itemId);
    return true;
}

// Finds the file behind icon number `index` in the icon list.
//
// Usually icon N in the list is item N in the folder view, but we double-check
// by comparing labels. If they don't match, we search by label instead. Safety
// checks like this matter: moving the wrong file would be a real problem.
bool GetDesktopItem(IFolderView* folderView, HWND list, int index,
                    DesktopItem* out) {
    std::wstring label = GetIconName(list, index);

    if (ReadFolderViewItem(folderView, index, out) && out->name == label) {
        return true;
    }

    int count = 0;
    folderView->ItemCount(SVGIO_ALLVIEW, &count);
    for (int i = 0; i < count; i++) {
        if (ReadFolderViewItem(folderView, i, out) && out->name == label) {
            Wh_Log(L"(Found \"%s\" by its label instead of its position)",
                   label.c_str());
            return true;
        }
    }

    Wh_Log(L"Couldn't find the file behind \"%s\"", label.c_str());
    return false;
}

// Something can only go into a group if it's a real file that isn't a folder.
bool CanBeGrouped(const DesktopItem& item) {
    return !item.path.empty() && !item.isDirectory;
}

// --- Placing icons -----------------------------------------------------------
//
// Windows puts new desktop items in the next free spot. We'd rather a new
// group appear where you dropped, and a removed group's last app appear
// where the group was.
//
// The folder view can tell us where an item is (GetItemPosition) and move it
// (SelectAndPositionItems). But a new item only shows up in the view a moment
// after its file appears, so we keep a short to-do list, "put this item here
// once it shows up", and check it every 50 ms (for up to 3 seconds) using a
// timer on the desktop window.
//
// The folder view belongs to the desktop's thread, so this all runs there.
// The group watcher thread asks for it by sending the desktop window our own
// messages. Those message numbers come from RegisterWindowMessageW, which
// hands out numbers no other program uses. (Fixed numbers like WM_APP + 1
// are only safe in our own windows; the desktop window is Explorer's, and
// Explorer may already use them.)

UINT g_msgGetItemPosition = 0;  // lParam: std::shared_ptr<PositionRequest>*
UINT g_msgPlaceItem = 0;        // lParam: Placement*
UINT g_msgStopPlacing = 0;      // Clears the to-do list (when the mod stops).
constexpr UINT_PTR kPlacementTimer = 0x5DA6;  // An unusual number, to avoid
                                              // clashing with DefView's timers.

void RegisterPlacementMessages() {
    g_msgGetItemPosition = RegisterWindowMessageW(L"DesktopAppGroups_GetItemPosition");
    g_msgPlaceItem = RegisterWindowMessageW(L"DesktopAppGroups_PlaceItem");
    g_msgStopPlacing = RegisterWindowMessageW(L"DesktopAppGroups_StopPlacing");
}

struct Placement {
    std::wstring path;
    POINT position;
    ULONGLONG giveUpTime;
};
std::vector<Placement> g_placements;  // Only used on the desktop's thread.

// Finds an item in the desktop's folder view by its path. Returns its number,
// or -1.
int FindViewItem(IFolderView* folderView, const std::wstring& path) {
    int count = 0;
    folderView->ItemCount(SVGIO_ALLVIEW, &count);
    for (int i = 0; i < count; i++) {
        DesktopItem item;
        if (ReadFolderViewItem(folderView, i, &item) &&
            _wcsicmp(item.path.c_str(), path.c_str()) == 0) {
            return i;
        }
    }
    return -1;
}

// Where is this item on the desktop? (Desktop's thread only.)
bool GetViewItemPosition(const std::wstring& path, POINT* position) {
    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    int index = folderView ? FindViewItem(folderView.Get(), path) : -1;
    PITEMID_CHILD itemId = nullptr;
    if (index < 0 || FAILED(folderView->Item(index, &itemId))) {
        return false;
    }
    HRESULT result = folderView->GetItemPosition(itemId, position);
    CoTaskMemFree(itemId);
    return SUCCEEDED(result);
}

// Moves an item to a position, if it's in the view yet. (Desktop's thread.)
bool PlaceViewItem(const std::wstring& path, POINT position) {
    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    int index = folderView ? FindViewItem(folderView.Get(), path) : -1;
    PITEMID_CHILD itemId = nullptr;
    if (index < 0 || FAILED(folderView->Item(index, &itemId))) {
        return false;
    }
    PCUITEMID_CHILD items[] = {itemId};
    HRESULT result = folderView->SelectAndPositionItems(1, items, &position, SVSI_POSITIONITEM);
    CoTaskMemFree(itemId);
    return SUCCEEDED(result);
}

// Adds to the to-do list. (Desktop's thread; `desktopWindow` is DefView.)
void QueuePlacement(HWND desktopWindow, const std::wstring& path, POINT position) {
    g_placements.push_back({path, position, GetTickCount64() + 3000});
    SetTimer(desktopWindow, kPlacementTimer, 50, nullptr);
}

// Called by the timer: places whatever has shown up, drops anything that's
// taken too long, and stops the timer once the list is empty.
void ProcessPlacements(HWND desktopWindow) {
    ULONGLONG now = GetTickCount64();
    for (size_t i = 0; i < g_placements.size();) {
        if (PlaceViewItem(g_placements[i].path, g_placements[i].position) ||
            now > g_placements[i].giveUpTime) {
            g_placements.erase(g_placements.begin() + i);
        } else {
            i++;
        }
    }
    if (g_placements.empty()) {
        KillTimer(desktopWindow, kPlacementTimer);
    }
}

// For other threads (the group watcher): the same, by asking the desktop's
// thread with a message.
//
// Careful: a message must never point at data the sender might throw away
// before the message is handled. SendMessageTimeoutW gives up waiting after
// 2 seconds if the desktop is busy, but the message stays queued, and the
// desktop's thread handles it later. So each request lives on the heap
// (created with "new"), and the desktop's thread frees it when it's done.

// The request and its answer. Both threads keep a reference to it (a
// shared_ptr, which counts its users), and it's freed when the last one lets
// go. So if the asker gives up, the desktop's thread can still answer safely.
struct PositionRequest {
    std::wstring path;
    POINT position = {};
    bool found = false;
};

bool AskDesktopForPosition(const std::wstring& path, POINT* position) {
    HWND desktopWindow = g_desktopDefView;
    if (!desktopWindow) {
        return false;
    }
    auto request = std::make_shared<PositionRequest>();
    request->path = path;
    // The desktop's thread's own reference, which it deletes once it answers.
    auto* message = new std::shared_ptr<PositionRequest>(request);

    DWORD_PTR ignored = 0;
    if (!SendMessageTimeoutW(desktopWindow, g_msgGetItemPosition, 0, (LPARAM)message,
                             SMTO_ABORTIFHUNG, 2000, &ignored)) {
        // Gave up waiting. If the message is handled later, the desktop's
        // thread frees it then. (If it never is, a few bytes are lost, which
        // is harmless.)
        return false;
    }
    *position = request->position;
    return request->found;
}

// No answer is needed here, so the message is posted (queued without
// waiting) instead of sent.
void AskDesktopToPlace(const std::wstring& path, POINT position) {
    HWND desktopWindow = g_desktopDefView;
    if (!desktopWindow) {
        return;
    }
    auto* request = new Placement{path, position, 0};
    if (!PostMessageW(desktopWindow, g_msgPlaceItem, 0, (LPARAM)request)) {
        delete request;  // Not queued, so nobody else will free it.
    }
}

// ---- 5. Groups --------------------------------------------------------------
//
// A group is a normal folder on your desktop. That keeps it simple and safe:
// if you uninstall the mod, your shortcuts are still in an ordinary folder.
//
// Each group also has some data of the mod's own: a marker saying "this
// folder is a group", the order you arranged its apps in, its own settings
// from the gear panel, and which icon it has. Where that's kept depends on
// the "Keep group data inside the group folders" setting:
//
//   - Off (the default): in the mod's storage in Windhawk, on this PC. Each
//     group is recognised by its folder's file ID, a number Windows gives
//     every file and folder, which stays the same when the folder is renamed
//     or moved around the same drive.
//   - On: in the group's hidden "desktop.ini" file (a standard Windows file
//     for folder settings), so the data goes wherever the folder goes.
//
// Either way, desktop.ini holds the line Windows needs to show the group's
// icon. With the setting on, the mod's data is in there too:
//
//   [.ShellClassInfo]
//   IconResource=C:\...\group-icon-123.ico,0   <- the group's icon
//   [DesktopAppGroups]                         <- only with the setting on:
//   IsGroup=1                                  <- "this folder is a group"
//   IconVersion=2                              <- which icon style it has
//   IconFingerprint=123456                     <- which apps the icon shows
//   Order=Notepad.lnk|Paint.lnk                <- your arrangement
//   Tint=255                                   <- the group's own settings
//
// When the setting changes, each group's data is moved over (MoveGroupData).
// If the drive has no file IDs (some network drives), the data stays in
// desktop.ini whatever the setting, since there's nothing else to recognise
// the folder by.

const WCHAR kGroupIniSection[] = L"DesktopAppGroups";

std::wstring ReadGroupIniValue(const std::wstring& folderPath, PCWSTR section,
                               PCWSTR key) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    // Big enough for a long list of app names (the saved order).
    std::vector<WCHAR> value(32 * 1024, L'\0');
    GetPrivateProfileStringW(section, key, L"", value.data(), (DWORD)value.size(),
                             iniPath.c_str());
    return value.data();
}

// Makes sure a group's desktop.ini is stored as Unicode, so app names in any
// alphabet (Japanese, Greek...) survive being saved in it. Windows decides by
// the first two bytes: 0xFF 0xFE (a "byte order mark") means Unicode.
// Otherwise it uses the older one-byte-per-letter format, which garbles
// letters outside your PC's language. So: create the file with the mark, or
// convert an existing old-format file.
void EnsureUnicodeGroupIni(const std::wstring& folderPath) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    std::vector<char> oldBytes;

    HANDLE file = CreateFileW(iniPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                              OPEN_EXISTING, 0, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER size = {};
        GetFileSizeEx(file, &size);
        if (size.QuadPart > 0 && size.QuadPart < 1024 * 1024) {
            oldBytes.resize((size_t)size.QuadPart);
            DWORD read = 0;
            ReadFile(file, oldBytes.data(), (DWORD)oldBytes.size(), &read, nullptr);
            oldBytes.resize(read);
        }
        CloseHandle(file);
        if (oldBytes.size() >= 2 && (BYTE)oldBytes[0] == 0xFF && (BYTE)oldBytes[1] == 0xFE) {
            return;  // Already Unicode.
        }
    }

    // Convert the old text (if any) to Unicode, and write it back after the mark.
    std::wstring text;
    if (!oldBytes.empty()) {
        int length = MultiByteToWideChar(CP_ACP, 0, oldBytes.data(), (int)oldBytes.size(),
                                         nullptr, 0);
        text.resize(length);
        MultiByteToWideChar(CP_ACP, 0, oldBytes.data(), (int)oldBytes.size(), text.data(),
                            length);
    }
    text.insert(text.begin(), (WCHAR)0xFEFF);

    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_NORMAL);
    file = CreateFileW(iniPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(file, text.data(), (DWORD)(text.size() * sizeof(WCHAR)), &written, nullptr);
        CloseHandle(file);
    }
    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

// Windows won't let programs overwrite hidden system files, so the hidden
// and system flags come off while we write, and go back on afterwards.
void WriteGroupIniValue(const std::wstring& folderPath, PCWSTR section,
                        PCWSTR key, PCWSTR value) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    EnsureUnicodeGroupIni(folderPath);
    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_NORMAL);
    WritePrivateProfileStringW(section, key, value, iniPath.c_str());
    WritePrivateProfileStringW(nullptr, nullptr, nullptr, iniPath.c_str());  // Save now.
    SetFileAttributesW(iniPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
}

// Lists the names in a group's [DesktopAppGroups] section (none if it has
// no such section).
std::vector<std::wstring> ReadGroupIniNames(const std::wstring& folderPath) {
    std::wstring iniPath = folderPath + L"\\desktop.ini";
    std::vector<WCHAR> buffer(8 * 1024, L'\0');
    // Asking for a value with no name (nullptr) gives the list of names,
    // each ending with a 0.
    DWORD length = GetPrivateProfileStringW(kGroupIniSection, nullptr, L"", buffer.data(),
                                            (DWORD)buffer.size(), iniPath.c_str());
    std::vector<std::wstring> names;
    for (DWORD start = 0; start < length;) {
        std::wstring name = buffer.data() + start;
        if (name.empty()) {
            break;
        }
        names.push_back(name);
        start += (DWORD)name.size() + 1;
    }
    return names;
}

// --- Group data in Windhawk's storage ----------------------------------------
//
// Each group's data is one stored text value, named "Group-" plus the
// folder's ID, holding "name=value" entries (the same names as in
// desktop.ini), each ending with "*":
//   IsGroup=1*Order=Notepad.lnk|Paint.lnk*Tint=255*
// "*" can't appear in a file name, so it can't appear in a value. (Not line
// breaks: portable Windhawk keeps its storage in an .ini file, which can't
// hold them.) A list of all the IDs is kept as well, in "Groups", so the mod
// can tidy up after groups deleted while it wasn't running.
//
// Windhawk's storage for a mod is shared by everyone who uses the PC, but
// each person has their own desktop and their own groups. So the names of
// these values start with the ID of your Windows account (your "SID", like
// "S-1-5-21-...-1001-"). Each account has exactly one Explorer showing its
// desktop, so that Explorer is the only one writing its values.
//
// Windhawk doesn't promise its storage functions are safe to use from two
// threads at once, and both the desktop's thread and the group watcher need
// group data. So every use of the storage holds g_storageLock, and a copy of
// all group data is kept in memory (g_groupData) for quick reading.

std::mutex g_storageLock;
using GroupValues = std::map<std::wstring, std::wstring>;
std::map<std::wstring, GroupValues> g_groupData;  // Folder ID -> its values.

// The start of the names of this account's values (see above). Set once in
// Wh_ModInit. "" if Windows couldn't tell us the account's ID.
std::wstring g_userPrefix;

std::wstring CurrentUserPrefix() {
    std::wstring prefix;
    HANDLE token;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
        DWORD size = 0;
        PWSTR sid = nullptr;
        if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
            ConvertSidToStringSidW(((TOKEN_USER*)buffer)->User.Sid, &sid)) {
            prefix = std::wstring(sid) + L"-";
            LocalFree(sid);
        }
        CloseHandle(token);
    }
    return prefix;
}

// The full stored name of one of this account's values.
std::wstring UserValueName(const std::wstring& name) {
    return g_userPrefix + name;
}

// Reads a stored text value ("" if there's none). Hold g_storageLock.
std::wstring ReadStoredText(const std::wstring& name) {
    std::vector<WCHAR> buffer(64 * 1024, L'\0');
    Wh_GetStringValue(name.c_str(), buffer.data(), buffer.size());
    return buffer.data();
}

// Saves a text value, and checks it reads back the same. Hold g_storageLock.
bool WriteStoredText(const std::wstring& name, const std::wstring& text) {
    if (!Wh_SetStringValue(name.c_str(), text.c_str()) || ReadStoredText(name) != text) {
        Wh_Log(L"Couldn't save \"%s\" in Windhawk's storage", name.c_str());
        return false;
    }
    return true;
}

// For single values of this account's (UnfinishedGroup), without holding
// the lock.
std::wstring LoadStoredText(PCWSTR name) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    return ReadStoredText(UserValueName(name));
}

void StoreText(PCWSTR name, PCWSTR text) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    WriteStoredText(UserValueName(name), text);
}

void DeleteStoredValue(PCWSTR name) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    Wh_DeleteValue(UserValueName(name).c_str());
}

// A file or folder's ID, as text: "V", its drive's serial number (16
// hexadecimal digits), "F", and its file ID (32 hexadecimal digits). "" if
// Windows can't tell us (for example, the drive doesn't have file IDs).
std::wstring FileIdText(HANDLE file) {
    FILE_ID_INFO info = {};
    if (!GetFileInformationByHandleEx(file, FileIdInfo, &info, sizeof(info))) {
        return L"";
    }
    bool allZero = true;
    for (BYTE b : info.FileId.Identifier) {
        allZero = allZero && b == 0;
    }
    if (allZero) {
        return L"";  // Some drives answer, but with no real ID.
    }
    const WCHAR digits[] = L"0123456789ABCDEF";
    auto hex = [&](const BYTE* bytes, size_t count) {
        std::wstring text;
        for (size_t i = 0; i < count; i++) {
            text += digits[bytes[i] >> 4];
            text += digits[bytes[i] & 15];
        }
        return text;
    };
    return L"V" + hex((const BYTE*)&info.VolumeSerialNumber, sizeof(info.VolumeSerialNumber)) +
           L"F" + hex(info.FileId.Identifier, sizeof(info.FileId.Identifier));
}

// Opens a folder just to read its information. FILE_FLAG_BACKUP_SEMANTICS
// is what allows opening folders (not just files).
HANDLE OpenForInfo(const std::wstring& path) {
    return CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
                       FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                       OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
}

std::wstring GroupIdOf(const std::wstring& folderPath) {
    HANDLE folder = OpenForInfo(folderPath);
    if (folder == INVALID_HANDLE_VALUE) {
        return L"";
    }
    std::wstring id = FileIdText(folder);
    CloseHandle(folder);
    return id;
}

// Turns a stored group back into its values.
GroupValues ParseStoredGroup(const std::wstring& text) {
    GroupValues values;
    for (size_t start = 0; start < text.size();) {
        // Entries end with "*". (Line breaks are accepted too, from test
        // versions of the mod.)
        size_t end = text.find_first_of(L"*\n", start);
        if (end == std::wstring::npos) {
            end = text.size();
        }
        std::wstring entry = text.substr(start, end - start);
        start = end + 1;
        size_t equals = entry.find(L'=');
        if (equals != std::wstring::npos && equals > 0) {
            values[entry.substr(0, equals)] = entry.substr(equals + 1);
        }
    }
    return values;
}

// Splits a list of IDs separated by "|".
std::vector<std::wstring> SplitIdList(const std::wstring& list) {
    std::vector<std::wstring> ids;
    for (size_t start = 0; start < list.size();) {
        size_t end = list.find(L'|', start);
        if (end == std::wstring::npos) {
            end = list.size();
        }
        if (end > start) {
            ids.push_back(list.substr(start, end - start));
        }
        start = end + 1;
    }
    return ids;
}

bool SaveGroupIdList();
bool SaveStoredGroup(const std::wstring& id);

// Loads all of this account's stored group data into memory. Called once, at
// startup.
void LoadStoredGroups() {
    std::lock_guard<std::mutex> lock(g_storageLock);
    g_groupData.clear();
    for (const std::wstring& id : SplitIdList(ReadStoredText(UserValueName(L"Groups")))) {
        GroupValues values = ParseStoredGroup(ReadStoredText(UserValueName(L"Group-" + id)));
        if (!values.empty()) {
            g_groupData[id] = values;
        }
    }

    // Test versions of the mod saved groups without the account's ID in the
    // names. Take those over, under this account's names.
    if (g_userPrefix.empty()) {
        return;
    }
    std::vector<std::wstring> oldIds = SplitIdList(ReadStoredText(L"Groups"));
    if (oldIds.empty()) {
        return;
    }
    bool allSaved = true;
    for (const std::wstring& id : oldIds) {
        GroupValues values = ParseStoredGroup(ReadStoredText(L"Group-" + id));
        if (!values.empty() && g_groupData.find(id) == g_groupData.end()) {
            g_groupData[id] = values;
            allSaved = SaveStoredGroup(id) && allSaved;
        }
    }
    if (allSaved && SaveGroupIdList()) {
        for (const std::wstring& id : oldIds) {
            Wh_DeleteValue((L"Group-" + id).c_str());
        }
        Wh_DeleteValue(L"Groups");
        Wh_Log(L"Moved %d group(s) from an earlier test version", (int)oldIds.size());
    }
}

// Saves the list of IDs. Hold g_storageLock.
bool SaveGroupIdList() {
    std::wstring list;
    for (const auto& group : g_groupData) {
        if (!list.empty()) {
            list += L"|";
        }
        list += group.first;
    }
    return WriteStoredText(UserValueName(L"Groups"), list);
}

// Saves one group's data from memory. Hold g_storageLock.
bool SaveStoredGroup(const std::wstring& id) {
    std::wstring text;
    for (const auto& value : g_groupData[id]) {
        text += value.first + L"=" + value.second + L"*";
    }
    return WriteStoredText(UserValueName(L"Group-" + id), text);
}

bool GetStoredGroupValue(const std::wstring& id, PCWSTR name, std::wstring* value) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    auto group = g_groupData.find(id);
    if (group == g_groupData.end()) {
        return false;
    }
    auto entry = group->second.find(name);
    if (entry == group->second.end()) {
        return false;
    }
    *value = entry->second;
    return true;
}

// Sets (or, with nullptr, removes) one value of a group's stored data.
void SetStoredGroupValue(const std::wstring& id, PCWSTR name, PCWSTR value) {
    std::lock_guard<std::mutex> lock(g_storageLock);
    bool isNew = g_groupData.find(id) == g_groupData.end();
    if (value) {
        g_groupData[id][name] = value;
    } else if (isNew) {
        return;
    } else {
        g_groupData[id].erase(name);
        if (g_groupData[id].empty()) {
            g_groupData.erase(id);
            Wh_DeleteValue(UserValueName(L"Group-" + id).c_str());
            SaveGroupIdList();
            return;
        }
    }
    // (A failed save is logged by WriteStoredText. The value stays in
    // memory, so it still works until Explorer restarts.)
    if (SaveStoredGroup(id) && isNew) {
        SaveGroupIdList();
    }
}

// Removes all of a group's stored data, when the group is removed.
void ForgetStoredGroup(const std::wstring& id) {
    if (id.empty()) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_storageLock);
    if (g_groupData.erase(id)) {
        Wh_DeleteValue(UserValueName(L"Group-" + id).c_str());
        SaveGroupIdList();
    }
}

// --- Group data, wherever it's kept ------------------------------------------

// Should this group's data be in its desktop.ini? `id` is the folder's ID.
bool WantDataInFolder(const std::wstring& id) {
    return g_dataInFolder || id.empty();
}

// Moves a group's data to where the setting says it belongs, if it's in the
// other place. Each value is copied first, and only removed from the old
// place once it's safely saved in the new one.
void MoveGroupData(const std::wstring& folderPath, const std::wstring& id) {
    if (WantDataInFolder(id)) {
        if (id.empty()) {
            return;
        }
        GroupValues values;
        {
            std::lock_guard<std::mutex> lock(g_storageLock);
            auto group = g_groupData.find(id);
            if (group == g_groupData.end()) {
                return;  // Nothing stored: already in the folder, or not a group.
            }
            values = group->second;
        }
        for (const auto& value : values) {
            WriteGroupIniValue(folderPath, kGroupIniSection, value.first.c_str(),
                               value.second.c_str());
        }
        if (ReadGroupIniNames(folderPath).size() < values.size()) {
            Wh_Log(L"Couldn't move the data for \"%s\" into its folder", folderPath.c_str());
            return;
        }
        ForgetStoredGroup(id);
        Wh_Log(L"Moved the data for \"%s\" into its desktop.ini", folderPath.c_str());
    } else {
        std::vector<std::wstring> names = ReadGroupIniNames(folderPath);
        if (names.empty()) {
            return;  // Nothing in desktop.ini: already in storage, or not a group.
        }
        {
            std::lock_guard<std::mutex> lock(g_storageLock);
            bool isNew = g_groupData.find(id) == g_groupData.end();
            for (const std::wstring& name : names) {
                g_groupData[id][name] = ReadGroupIniValue(folderPath, kGroupIniSection,
                                                          name.c_str());
            }
            if (!SaveStoredGroup(id)) {
                Wh_Log(L"Couldn't save the data for \"%s\" in storage", folderPath.c_str());
                return;
            }
            if (isNew) {
                SaveGroupIdList();
            }
        }
        // Writing "no name" (nullptr) removes the whole section.
        WriteGroupIniValue(folderPath, kGroupIniSection, nullptr, nullptr);
        Wh_Log(L"Moved the data for \"%s\" into Windhawk's storage", folderPath.c_str());
    }
}

// Reads one value of a group's data ("" if it has none).
std::wstring ReadGroupValue(const std::wstring& folderPath, PCWSTR name) {
    std::wstring id = GroupIdOf(folderPath);
    std::wstring value;
    if (!id.empty() && GetStoredGroupValue(id, name, &value)) {
        return value;
    }
    return ReadGroupIniValue(folderPath, kGroupIniSection, name);
}

// Sets (or, with nullptr, removes) one value of a group's data, in the place
// the setting says. Any data still in the other place is moved first, so
// the group's data is never split between the two.
void WriteGroupValue(const std::wstring& folderPath, PCWSTR name, PCWSTR value) {
    std::wstring id = GroupIdOf(folderPath);
    MoveGroupData(folderPath, id);
    if (WantDataInFolder(id)) {
        WriteGroupIniValue(folderPath, kGroupIniSection, name, value);
    } else {
        SetStoredGroupValue(id, name, value);
    }
}

bool IsGroupFolder(const std::wstring& folderPath) {
    return ReadGroupValue(folderPath, L"IsGroup") == L"1";
}

void MarkAsGroup(const std::wstring& folderPath) {
    WriteGroupValue(folderPath, L"IsGroup", L"1");

    // Windows only reads a folder's desktop.ini (for the icon) if the folder
    // has the "read-only" flag. (For folders this flag doesn't make anything
    // read-only; it just means "has custom settings".)
    DWORD folderAttributes = GetFileAttributesW(folderPath.c_str());
    if (folderAttributes != INVALID_FILE_ATTRIBUTES) {
        SetFileAttributesW(folderPath.c_str(), folderAttributes | FILE_ATTRIBUTE_READONLY);
    }
}

// --- Group settings ----------------------------------------------------------
//
// The settings you change with the gear button in the popup. Each one can be
// set for all groups, or for one group only:
//
//   - The all-groups value is kept in the mod's own storage (Wh_SetIntValue),
//     which Windhawk keeps safe between restarts.
//   - A group's own value is kept with the rest of the group's data (see the
//     start of this section). A group with no value of its own uses the
//     all-groups one.

enum GroupSettingId {
    kSettingColumns,
    kSettingIconSize,
    kSettingShowNames,
    kSettingSort,
    kSettingRemoveWhenOneLeft,
    kSettingTint,
    kGroupSettingCount
};

struct GroupSettingInfo {
    PCWSTR key;        // Its name in storage and in desktop.ini.
    int defaultValue;
};

const GroupSettingInfo kGroupSettings[kGroupSettingCount] = {
    {L"Columns", 4},            // Apps per row, 3 to 6.
    {L"IconSize", 48},          // 32, 48 or 64 pixels.
    {L"ShowNames", 1},          // 1 = show app names under the icons.
    {L"Sort", 0},               // 0 = your arrangement, 1 = A to Z.
    {L"RemoveWhenOneLeft", 1},  // 1 = remove a group that's down to one app.
    {L"Tint", -1},              // The group's colour, or -1 for none.
};

// Group colours. A colour is stored as a COLORREF, a number of the form
// 0x00BBGGRR (blue, green, red), which is never negative, so -1 can mean
// "no colour". These are the preset swatches in the settings panel; any
// other colour can be picked with "Custom".
constexpr int kNoTint = -1;
constexpr int kCustomTint = -2;  // Only used for the "Custom" swatch itself.
const COLORREF kTintPresets[] = {
    RGB(209, 52, 56),   // Red
    RGB(247, 99, 12),   // Orange
    RGB(255, 185, 0),   // Yellow
    RGB(16, 137, 62),   // Green
    RGB(0, 153, 188),   // Teal
    RGB(0, 120, 212),   // Blue
    RGB(136, 23, 152),  // Purple
    RGB(227, 0, 140),   // Pink
};

bool IsPresetTint(int tint) {
    for (COLORREF preset : kTintPresets) {
        if ((int)preset == tint) {
            return true;
        }
    }
    return false;
}

// The all-groups values, kept in memory so they're quick to read from any
// thread. ("atomic" makes it safe for one thread to read while another
// writes.) Like group data, they're stored separately for each Windows
// account (see "Group data in Windhawk's storage" above).
std::atomic<int> g_allGroupsValues[kGroupSettingCount];

void LoadAllGroupsSettings() {
    std::lock_guard<std::mutex> lock(g_storageLock);
    for (int id = 0; id < kGroupSettingCount; id++) {
        // If this account hasn't saved one yet, start from the value saved by
        // an earlier version (which wasn't per account), or the default.
        int earlierValue =
            Wh_GetIntValue(kGroupSettings[id].key, kGroupSettings[id].defaultValue);
        g_allGroupsValues[id] =
            Wh_GetIntValue(UserValueName(kGroupSettings[id].key).c_str(), earlierValue);
    }
}

int GetAllGroupsSetting(GroupSettingId id) {
    return g_allGroupsValues[id];
}

void SetAllGroupsSetting(GroupSettingId id, int value) {
    g_allGroupsValues[id] = value;
    std::lock_guard<std::mutex> lock(g_storageLock);
    Wh_SetIntValue(UserValueName(kGroupSettings[id].key).c_str(), value);
}

// Does this group have its own value for a setting? If so, it's put in `value`.
bool GetGroupOwnSetting(const std::wstring& folderPath, GroupSettingId id, int* value) {
    std::wstring text = ReadGroupValue(folderPath, kGroupSettings[id].key);
    if (text.empty()) {
        return false;
    }
    *value = _wtoi(text.c_str());
    return true;
}

void SetGroupOwnSetting(const std::wstring& folderPath, GroupSettingId id, int value) {
    WriteGroupValue(folderPath, kGroupSettings[id].key, std::to_wstring(value).c_str());
}

void ClearGroupOwnSetting(const std::wstring& folderPath, GroupSettingId id) {
    WriteGroupValue(folderPath, kGroupSettings[id].key, nullptr);  // nullptr: remove it.
}

// The value that applies to a group: its own if it has one, otherwise the
// all-groups value.
int GetGroupSetting(const std::wstring& folderPath, GroupSettingId id) {
    int value;
    if (GetGroupOwnSetting(folderPath, id, &value)) {
        return value;
    }
    return GetAllGroupsSetting(id);
}

// (CreateGroup is at the end of section 6, because it draws the group's
// icon.)

struct GroupEntry {
    std::wstring name;  // What Explorer shows, e.g. "Notepad" (no ".lnk").
    std::wstring path;  // Full path on disk.
};

std::wstring FileNameOf(const std::wstring& path) {
    return path.substr(path.find_last_of(L'\\') + 1);
}

// Lists what's in a group, in the order you arranged them in the popup.
// Anything not in the saved order (like apps added since) goes at the end,
// sorted the way Explorer sorts ("App 2" before "App 10"). Hidden files,
// like desktop.ini and the icon, are skipped.
//
// The order is saved with the group's data as file names separated by "|",
// a character that can't appear in file names:
//   Order=Notepad.lnk|Paint.lnk|Calculator.lnk
std::vector<GroupEntry> ListGroupFiles(const std::wstring& folderPath) {
    std::vector<GroupEntry> entries;

    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((folderPath + L"\\*").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return entries;
    }

    do {
        std::wstring fileName = found.cFileName;
        if (fileName == L"." || fileName == L".." ||
            (found.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))) {
            continue;
        }

        GroupEntry entry;
        entry.path = folderPath + L"\\" + fileName;
        entry.name = fileName;

        ComPtr<IShellItem> shellItem;
        PWSTR displayName = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(entry.path.c_str(), nullptr,
                                                  IID_PPV_ARGS(&shellItem))) &&
            SUCCEEDED(shellItem->GetDisplayName(SIGDN_NORMALDISPLAY, &displayName))) {
            entry.name = displayName;
            CoTaskMemFree(displayName);
        }

        entries.push_back(entry);
    } while (FindNextFileW(search, &found));
    FindClose(search);

    // Turn the saved order into "file name -> position". Names are compared
    // without caring about capitals, like Windows does.
    // With the "A to Z" setting, the saved order is ignored.
    std::vector<std::wstring> savedOrder;
    std::wstring orderText;
    if (GetGroupSetting(folderPath, kSettingSort) == 0) {
        orderText = ReadGroupValue(folderPath, L"Order");
    }
    for (size_t start = 0; start < orderText.size();) {
        size_t end = orderText.find(L'|', start);
        if (end == std::wstring::npos) {
            end = orderText.size();
        }
        savedOrder.push_back(orderText.substr(start, end - start));
        start = end + 1;
    }
    auto positionOf = [&](const GroupEntry& entry) {
        std::wstring fileName = FileNameOf(entry.path);
        for (size_t i = 0; i < savedOrder.size(); i++) {
            if (_wcsicmp(savedOrder[i].c_str(), fileName.c_str()) == 0) {
                return i;
            }
        }
        return savedOrder.size();  // Not in the saved order: after the rest.
    };

    std::sort(entries.begin(), entries.end(),
              [&](const GroupEntry& a, const GroupEntry& b) {
                  size_t positionA = positionOf(a), positionB = positionOf(b);
                  if (positionA != positionB) {
                      return positionA < positionB;
                  }
                  return StrCmpLogicalW(a.name.c_str(), b.name.c_str()) < 0;
              });
    return entries;
}

// Gets an item's icon as a picture of the given size, with see-through edges.
HBITMAP GetItemIcon(const std::wstring& path, int pixels) {
    ComPtr<IShellItem> shellItem;
    ComPtr<IShellItemImageFactory> imageFactory;
    HBITMAP bitmap = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                              IID_PPV_ARGS(&shellItem))) &&
        SUCCEEDED(shellItem.As(&imageFactory))) {
        SIZE size = {pixels, pixels};
        imageFactory->GetImage(size, SIIGBF_ICONONLY, &bitmap);
    }
    return bitmap;
}

// ---- 6. Group icons ---------------------------------------------------------
//
// A group's icon is a rounded, see-through tile with the first four apps
// inside in a 2x2 grid, like a Start menu folder.
//
// Windows shows desktop icons at many sizes (small, medium, large, and scaled
// for high-DPI screens), so an .ico file holds several pictures: 16x16 up to
// 256x256. We draw each size separately, so small sizes stay sharp.
//
// We draw pixel by pixel instead of using GDI, because GDI can't draw
// see-through colours. Each pixel has four parts: blue, green, red, and alpha
// (how opaque it is). We use "premultiplied" alpha, the format Windows icons
// come in: each colour is already multiplied by the pixel's opacity. That
// makes layering one picture over another a simple formula (see BlendPixel).

// One pixel, in the order Windows stores them.
struct Pixel {
    BYTE b, g, r, a;
};

// Text drawn as a "coverage map": for each pixel, how much of it the text
// covers (0 = none, 255 = fully). See RenderTextMask in section 9.
struct TextMask {
    int width = 0;
    int height = 0;
    std::vector<BYTE> coverage;
};

// A picture we draw into pixel by pixel. Used for group icons (square), and
// for the popup and settings panel (any size), because it can draw
// see-through colours, which their blurred backgrounds need.
class IconCanvas {
public:
    explicit IconCanvas(int size) : IconCanvas(size, size) {}
    IconCanvas(int width, int height)
        : m_width(width), m_height(height),
          m_pixels((size_t)width * height, Pixel{0, 0, 0, 0}) {}

    int Size() const { return m_width; }  // For square icons.
    int Width() const { return m_width; }
    int Height() const { return m_height; }
    const std::vector<Pixel>& Pixels() const { return m_pixels; }

    // Layer a (premultiplied) pixel over what's already there.
    void BlendPixel(int x, int y, Pixel source) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
            return;
        }
        Pixel& target = m_pixels[(size_t)y * m_width + x];
        int keep = 255 - source.a;  // How much of the old pixel shows through.
        target.b = (BYTE)(source.b + target.b * keep / 255);
        target.g = (BYTE)(source.g + target.g * keep / 255);
        target.r = (BYTE)(source.r + target.r * keep / 255);
        target.a = (BYTE)(source.a + target.a * keep / 255);
    }

    // Layer a colour at a given strength (0 to 1) over a pixel.
    void BlendColor(int x, int y, COLORREF color, float opacity) {
        if (opacity <= 0) {
            return;
        }
        BYTE a = (BYTE)std::lround(std::min(opacity, 1.0f) * 255);
        BlendPixel(x, y, {(BYTE)(GetBValue(color) * a / 255),
                          (BYTE)(GetGValue(color) * a / 255),
                          (BYTE)(GetRValue(color) * a / 255), a});
    }

    // Paint every pixel one solid colour.
    void FillAll(COLORREF color) {
        for (Pixel& p : m_pixels) {
            p = {GetBValue(color), GetGValue(color), GetRValue(color), 255};
        }
    }

    // A rounded rectangle with soft (anti-aliased) edges, plus an optional
    // border. For each pixel we work out how far it is from the shape's
    // edge: inside is negative, outside positive. Pixels right on the edge
    // get partly coloured, which is what makes the edge look smooth.
    void FillRoundedRect(float left, float top, float right, float bottom,
                         float radius, COLORREF fill, float fillOpacity,
                         COLORREF border = 0, float borderOpacity = 0,
                         float borderWidth = 0) {
        float centerX = (left + right) / 2, centerY = (top + bottom) / 2;
        float halfWidth = (right - left) / 2, halfHeight = (bottom - top) / 2;

        // Only visit pixels near the shape, not the whole picture.
        int firstX = std::max(0, (int)std::floor(left) - 1);
        int lastX = std::min(m_width - 1, (int)std::ceil(right) + 1);
        int firstY = std::max(0, (int)std::floor(top) - 1);
        int lastY = std::min(m_height - 1, (int)std::ceil(bottom) + 1);

        for (int y = firstY; y <= lastY; y++) {
            for (int x = firstX; x <= lastX; x++) {
                float qx = std::fabs(x + 0.5f - centerX) - (halfWidth - radius);
                float qy = std::fabs(y + 0.5f - centerY) - (halfHeight - radius);
                float outside = std::hypot(std::max(qx, 0.0f), std::max(qy, 0.0f));
                float distance = outside + std::min(std::max(qx, qy), 0.0f) - radius;

                float inShape = std::clamp(0.5f - distance, 0.0f, 1.0f);
                float inInner = std::clamp(0.5f - (distance + borderWidth), 0.0f, 1.0f);

                BlendColor(x, y, fill, fillOpacity * inShape);
                if (borderOpacity > 0) {
                    BlendColor(x, y, border, borderOpacity * (inShape - inInner));
                }
            }
        }
    }

    // Draw text (as a coverage map) in a colour, with its top-left at
    // (left, top).
    void BlendMask(const TextMask& mask, int left, int top, COLORREF color) {
        for (int y = 0; y < mask.height; y++) {
            for (int x = 0; x < mask.width; x++) {
                BYTE covered = mask.coverage[(size_t)y * mask.width + x];
                if (covered) {
                    BlendColor(left + x, top + y, color, covered / 255.0f);
                }
            }
        }
    }

    // Draw an icon picture at (left, top), scaled to `size` if needed.
    void DrawIcon(HBITMAP bitmap, int left, int top, int size) {
        BITMAP info;
        if (!GetObjectW(bitmap, sizeof(info), &info) || info.bmWidth <= 0) {
            return;
        }
        int width = info.bmWidth, height = std::abs(info.bmHeight);

        // Copy the icon's pixels out of Windows' bitmap into a plain list.
        std::vector<Pixel> source((size_t)width * height);
        BITMAPINFO format = {};
        format.bmiHeader.biSize = sizeof(format.bmiHeader);
        format.bmiHeader.biWidth = width;
        format.bmiHeader.biHeight = -height;  // Negative = top row first.
        format.bmiHeader.biPlanes = 1;
        format.bmiHeader.biBitCount = 32;
        format.bmiHeader.biCompression = BI_RGB;
        HDC screen = GetDC(nullptr);
        int rows = GetDIBits(screen, bitmap, 0, height, source.data(), &format,
                             DIB_RGB_COLORS);
        ReleaseDC(nullptr, screen);
        if (rows != height) {
            return;
        }

        // Shell icons usually come back at exactly the size we asked for. If
        // not, pick the nearest source pixel for each target pixel.
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                int sourceX = x * width / size, sourceY = y * height / size;
                BlendPixel(left + x, top + y, source[(size_t)sourceY * width + sourceX]);
            }
        }
    }

    // PNG files store colours not multiplied by opacity ("straight" alpha),
    // so undo the premultiplication.
    std::vector<Pixel> StraightPixels() const {
        std::vector<Pixel> result = m_pixels;
        for (Pixel& p : result) {
            if (p.a > 0 && p.a < 255) {
                p.b = (BYTE)std::min(255, p.b * 255 / p.a);
                p.g = (BYTE)std::min(255, p.g * 255 / p.a);
                p.r = (BYTE)std::min(255, p.r * 255 / p.a);
            }
        }
        return result;
    }

private:
    int m_width;
    int m_height;
    std::vector<Pixel> m_pixels;
};

// Draws the whole group icon at one size.
// A colour `amount` (0 to 1) of the way from `from` to `to`.
COLORREF MixColors(COLORREF from, COLORREF to, float amount) {
    auto mix = [&](BYTE a, BYTE b) { return (BYTE)std::lround(a + (b - a) * amount); };
    return RGB(mix(GetRValue(from), GetRValue(to)), mix(GetGValue(from), GetGValue(to)),
               mix(GetBValue(from), GetBValue(to)));
}

// Draws the whole group icon at one size. `tint` is the group's colour, or
// kNoTint.
IconCanvas DrawGroupIcon(int size, const std::vector<GroupEntry>& apps, int tint) {
    IconCanvas canvas(size);
    float s = (float)size;

    // The tile: frosted white, so it shows up on dark and colourful
    // wallpapers. With a group colour, frosted in that colour instead, a bit
    // stronger so the colour is easy to tell apart.
    COLORREF fill = RGB(255, 255, 255), border = RGB(255, 255, 255);
    float fillOpacity = 0.30f, borderOpacity = 0.55f;
    if (tint != kNoTint) {
        fill = (COLORREF)tint;
        border = MixColors((COLORREF)tint, RGB(255, 255, 255), 0.5f);
        fillOpacity = 0.60f;
        borderOpacity = 0.70f;
    }
    float inset = s * 0.03f;
    canvas.FillRoundedRect(inset, inset, s - inset, s - inset, s * 0.2f, fill, fillOpacity,
                           border, borderOpacity, std::max(1.0f, s / 48.0f));

    // The 2x2 grid of app icons inside it.
    float padding = s * 0.13f;
    float gap = s * 0.06f;
    float cell = (s - 2 * padding - gap) / 2;
    int cellPixels = std::max(1, (int)cell);

    int count = std::min((int)apps.size(), 4);
    for (int i = 0; i < count; i++) {
        int column = i % 2, row = i / 2;
        int left = (int)std::lround(padding + column * (cell + gap));
        int top = (int)std::lround(padding + row * (cell + gap));

        HBITMAP icon = GetItemIcon(apps[i].path, cellPixels);
        if (icon) {
            canvas.DrawIcon(icon, left, top, cellPixels);
            DeleteObject(icon);
        }
    }
    return canvas;
}

// Turns a picture into PNG file data, using WIC (Windows Imaging Component).
bool EncodePng(const IconCanvas& canvas, std::vector<BYTE>* pngOut) {
    std::vector<Pixel> pixels = canvas.StraightPixels();
    UINT size = canvas.Size();

    ComPtr<IWICImagingFactory> factory;
    ComPtr<IStream> stream;
    ComPtr<IWICBitmapEncoder> encoder;
    ComPtr<IWICBitmapFrameEncode> frame;
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;

    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory))) ||
        FAILED(CreateStreamOnHGlobal(nullptr, TRUE, &stream)) ||
        FAILED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder)) ||
        FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)) ||
        FAILED(encoder->CreateNewFrame(&frame, nullptr)) ||
        FAILED(frame->Initialize(nullptr)) ||
        FAILED(frame->SetSize(size, size)) ||
        FAILED(frame->SetPixelFormat(&format)) ||
        format != GUID_WICPixelFormat32bppBGRA ||
        FAILED(frame->WritePixels(size, size * 4, size * size * 4,
                                  (BYTE*)pixels.data())) ||
        FAILED(frame->Commit()) || FAILED(encoder->Commit())) {
        return false;
    }

    // Copy the finished PNG out of the in-memory stream.
    HGLOBAL memory = nullptr;
    STATSTG stats = {};
    if (FAILED(GetHGlobalFromStream(stream.Get(), &memory)) ||
        FAILED(stream->Stat(&stats, STATFLAG_NONAME))) {
        return false;
    }
    BYTE* data = (BYTE*)GlobalLock(memory);
    if (!data) {
        return false;
    }
    pngOut->assign(data, data + stats.cbSize.LowPart);
    GlobalUnlock(memory);
    return true;
}

// An .ico file is a short header listing each picture's size and where its
// data starts, followed by the pictures themselves (PNGs, in our case).
#pragma pack(push, 2)
struct IconFileHeader {
    WORD reserved;  // Always 0.
    WORD type;      // 1 = icon.
    WORD count;     // How many pictures.
};
struct IconFileEntry {
    BYTE width;     // 0 means 256.
    BYTE height;
    BYTE colorCount;
    BYTE reserved;
    WORD planes;
    WORD bitCount;
    DWORD dataSize;
    DWORD dataOffset;
};
#pragma pack(pop)

// The .ico format needs these exact sizes. If they were wrong, Windows would
// misread the file, so make the compiler check them for us.
static_assert(sizeof(IconFileHeader) == 6, "icon header must be 6 bytes");
static_assert(sizeof(IconFileEntry) == 16, "icon entry must be 16 bytes");

// The classic way to store an icon picture, used for every size except
// 256x256: a BITMAPINFOHEADER, then the pixels starting from the bottom row,
// then a 1-bit "mask" marking fully see-through pixels. (The header's height
// is doubled because it counts the pixels and the mask together.)
std::vector<BYTE> EncodeIconBitmap(const IconCanvas& canvas) {
    int size = canvas.Size();
    std::vector<Pixel> pixels = canvas.StraightPixels();
    int maskRowBytes = ((size + 31) / 32) * 4;  // Rows are padded to 4 bytes.

    BITMAPINFOHEADER header = {};
    header.biSize = sizeof(header);
    header.biWidth = size;
    header.biHeight = size * 2;
    header.biPlanes = 1;
    header.biBitCount = 32;
    header.biCompression = BI_RGB;
    header.biSizeImage = size * size * 4 + maskRowBytes * size;

    std::vector<BYTE> data(sizeof(header) + header.biSizeImage, 0);
    memcpy(data.data(), &header, sizeof(header));
    BYTE* colorData = data.data() + sizeof(header);
    BYTE* maskData = colorData + size * size * 4;

    for (int y = 0; y < size; y++) {
        int sourceRow = size - 1 - y;  // Bottom row first.
        memcpy(colorData + y * size * 4, &pixels[sourceRow * size], size * 4);
        for (int x = 0; x < size; x++) {
            if (pixels[sourceRow * size + x].a == 0) {
                maskData[y * maskRowBytes + x / 8] |= (BYTE)(0x80 >> (x % 8));
            }
        }
    }
    return data;
}

bool WriteGroupIconFile(const std::wstring& iconPath,
                        const std::vector<GroupEntry>& apps, int tint) {
    const int sizes[] = {16, 20, 24, 32, 40, 48, 64, 96, 128, 256};
    const int count = ARRAYSIZE(sizes);

    // 256x256 is stored as PNG to keep the file small; the others use the
    // classic format, which every part of Windows can read.
    std::vector<std::vector<BYTE>> pictures(count);
    for (int i = 0; i < count; i++) {
        IconCanvas canvas = DrawGroupIcon(sizes[i], apps, tint);
        if (sizes[i] == 256) {
            if (!EncodePng(canvas, &pictures[i])) {
                Wh_Log(L"Couldn't save the 256x256 picture as PNG");
                return false;
            }
        } else {
            pictures[i] = EncodeIconBitmap(canvas);
        }
    }

    IconFileHeader header = {0, 1, (WORD)count};
    std::vector<IconFileEntry> entries(count);
    DWORD offset = sizeof(header) + count * sizeof(IconFileEntry);
    for (int i = 0; i < count; i++) {
        entries[i] = {};
        entries[i].width = (BYTE)(sizes[i] == 256 ? 0 : sizes[i]);
        entries[i].height = entries[i].width;
        entries[i].planes = 1;
        entries[i].bitCount = 32;
        entries[i].dataSize = (DWORD)pictures[i].size();
        entries[i].dataOffset = offset;
        offset += entries[i].dataSize;
    }

    HANDLE file = CreateFileW(iconPath.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(file, &header, sizeof(header), &written, nullptr) &&
              WriteFile(file, entries.data(), count * sizeof(IconFileEntry),
                        &written, nullptr);
    for (int i = 0; ok && i < count; i++) {
        ok = WriteFile(file, pictures[i].data(), (DWORD)pictures[i].size(),
                       &written, nullptr);
    }
    CloseHandle(file);
    if (!ok) {
        DeleteFileW(iconPath.c_str());
    }
    return ok;
}

const WCHAR kIconFilePrefix[] = L"group-icon-";

// Bump this whenever the icon's look or format changes, and every group gets
// a fresh icon the next time the mod starts.
const int kIconVersion = 2;

// Does this group already have an up-to-date icon file that exists?
bool GroupHasIcon(const std::wstring& folderPath) {
    if (_wtoi(ReadGroupValue(folderPath, L"IconVersion").c_str()) != kIconVersion) {
        return false;
    }
    std::wstring resource = ReadGroupIniValue(folderPath, L".ShellClassInfo",
                                              L"IconResource");
    std::wstring iconPath = resource.substr(0, resource.find(L','));
    if (iconPath.empty()) {
        return false;
    }
    if (PathIsRelativeW(iconPath.c_str())) {
        iconPath = folderPath + L"\\" + iconPath;
    }
    return GetFileAttributesW(iconPath.c_str()) != INVALID_FILE_ATTRIBUTES;
}

// A short "fingerprint" of what the icon shows: the first four apps' file
// names, and the group's colour. If it hasn't changed, the icon doesn't need
// redrawing. (std::hash turns any text into a number; equal text always gives
// the same number.)
std::wstring IconFingerprint(const std::vector<GroupEntry>& apps, int tint) {
    std::wstring shown = L"tint " + std::to_wstring(tint) + L"|";
    for (size_t i = 0; i < apps.size() && i < 4; i++) {
        shown += apps[i].path.substr(apps[i].path.find_last_of(L'\\') + 1) + L"|";
    }
    return std::to_wstring(std::hash<std::wstring>{}(shown));
}

// Draws an icon showing `apps`, saves it in the group folder, sets it as the
// folder's icon, and tells Explorer to show it straight away.
//
// Explorer remembers ("caches") icons by file name, so if we overwrote the
// same file it might keep showing the old picture. Instead each new icon gets
// a new name (with a number that changes every time), and old ones are
// deleted.
bool ApplyGroupIcon(const std::wstring& folderPath,
                    const std::vector<GroupEntry>& apps) {
    ULONGLONG startTime = GetTickCount64();
    int tint = GetGroupSetting(folderPath, kSettingTint);
    std::wstring fingerprint = IconFingerprint(apps, tint);
    std::wstring iconName = kIconFilePrefix + std::to_wstring(GetTickCount64()) + L".ico";
    std::wstring iconPath = folderPath + L"\\" + iconName;

    if (!WriteGroupIconFile(iconPath, apps, tint)) {
        Wh_Log(L"Couldn't save the icon for \"%s\"", folderPath.c_str());
        return false;
    }
    SetFileAttributesW(iconPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

    // Check Windows can actually read the file we just wrote.
    HICON testIcon = nullptr;
    HRESULT loaded = SHDefExtractIconW(iconPath.c_str(), 0, 0, &testIcon, nullptr, 32);
    if (testIcon) {
        DestroyIcon(testIcon);
    } else {
        Wh_Log(L"  Windows can't read the new icon file (error 0x%08X)", (unsigned)loaded);
    }

    // Set the folder's icon with Windows' official function for it, the one
    // behind Properties > Customize > Change Icon. It updates desktop.ini and
    // tells Explorer to forget its cached copy of the folder's settings.
    // (It only changes the icon line, so any [DesktopAppGroups] data stays.)
    EnsureUnicodeGroupIni(folderPath);  // Before Windows creates it the old way.
    std::vector<WCHAR> iconPathBuffer(iconPath.begin(), iconPath.end());
    iconPathBuffer.push_back(L'\0');
    SHFOLDERCUSTOMSETTINGS settings = {};
    settings.dwSize = sizeof(settings);
    settings.dwMask = FCSM_ICONFILE;
    settings.pszIconFile = iconPathBuffer.data();
    settings.iIconIndex = 0;
    HRESULT result = SHGetSetFolderCustomSettings(&settings, folderPath.c_str(),
                                                  FCS_FORCEWRITE);
    if (FAILED(result)) {
        Wh_Log(L"  Couldn't set the folder icon (error 0x%08X)", (unsigned)result);
        return false;
    }
    WriteGroupValue(folderPath, L"IconVersion", std::to_wstring(kIconVersion).c_str());
    WriteGroupValue(folderPath, L"IconFingerprint", fingerprint.c_str());

    // Delete older icon files.
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW(
        (folderPath + L"\\" + kIconFilePrefix + L"*.ico").c_str(), &found);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            if (iconName != found.cFileName) {
                std::wstring oldPath = folderPath + L"\\" + found.cFileName;
                SetFileAttributesW(oldPath.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(oldPath.c_str());
            }
        } while (FindNextFileW(search, &found));
        FindClose(search);
    }

    // Tell Explorer this folder's look has changed. Windows normally collects
    // these notices and delivers them in a batch a moment later;
    // SHCNF_FLUSHNOWAIT asks for delivery right now.
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, folderPath.c_str(),
                   nullptr);

    Wh_Log(L"Updated the icon for \"%s\" (%d apps) in %u ms", folderPath.c_str(),
           (int)apps.size(), (unsigned)(GetTickCount64() - startTime));
    return true;
}

// Redraws a group's icon, unless it already shows these apps. Skipping
// unneeded redraws also stops our own writes (which the watcher can see as
// changes) from causing endless redraws.
void RefreshGroupIcon(const std::wstring& folderPath,
                      const std::vector<GroupEntry>& apps) {
    if (GroupHasIcon(folderPath) &&
        ReadGroupValue(folderPath, L"IconFingerprint") ==
            IconFingerprint(apps, GetGroupSetting(folderPath, kSettingTint))) {
        return;
    }
    ApplyGroupIcon(folderPath, apps);
}

std::wstring GetDesktopFolderPath() {
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &path))) {
        result = path;
    }
    CoTaskMemFree(path);
    return result;
}

// Lists the names of all groups on the desktop.
std::vector<std::wstring> FindAllGroups(const std::wstring& desktopPath) {
    std::vector<std::wstring> groups;
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((desktopPath + L"\\*").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return groups;
    }
    do {
        std::wstring name = found.cFileName;
        if ((found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && name != L"." &&
            name != L".." && IsGroupFolder(desktopPath + L"\\" + name)) {
            groups.push_back(name);
        }
    } while (FindNextFileW(search, &found));
    FindClose(search);
    return groups;
}

// --- Creating a group --------------------------------------------------------
//
// The icon is drawn *before* the group appears, so it never shows up as a
// plain folder first:
//
//   1. Create the folder, hidden.
//   2. Draw its icon from the apps, while they're still on the desktop.
//   3. Move the apps in.
//   4. Mark it as a group and unhide it. It appears looking like a group.
//
// The apps keep their file names when they move, so the icon's fingerprint
// still matches afterwards and the group watcher won't redraw it.

// Takes flags off a file or folder, such as hidden or read-only.
void RemoveAttributes(const std::wstring& path, DWORD attributesToRemove) {
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return;
    }
    attributes &= ~(attributesToRemove | FILE_ATTRIBUTE_DIRECTORY);
    SetFileAttributesW(path.c_str(), attributes ? attributes : FILE_ATTRIBUTE_NORMAL);
}

// Tidies up a half-made group after something went wrong: our own files, then
// the folder if it's empty. If some apps did get moved in, the folder stays
// (unhidden), so nothing disappears from view.
void CleanUpFailedGroup(const std::wstring& groupPath) {
    ForgetStoredGroup(GroupIdOf(groupPath));  // Its icon details, if any were saved.

    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((groupPath + L"\\*").c_str(), &found);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            std::wstring name = found.cFileName;
            if (_wcsicmp(name.c_str(), L"desktop.ini") == 0 ||
                _wcsnicmp(name.c_str(), kIconFilePrefix, wcslen(kIconFilePrefix)) == 0) {
                std::wstring path = groupPath + L"\\" + name;
                SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(path.c_str());
            }
        } while (FindNextFileW(search, &found));
        FindClose(search);
    }
    RemoveAttributes(groupPath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY);
    if (!RemoveDirectoryW(groupPath.c_str())) {
        Wh_Log(L"Kept \"%s\" because some files were moved into it", groupPath.c_str());
    }
}

void RequestGroupCheck(const std::wstring& folderPath);  // In section 7.

bool CreateGroup(const std::vector<DesktopItem>& items, std::wstring* groupPathOut) {
    std::wstring desktopPath = GetDesktopFolderPath();
    if (desktopPath.empty()) {
        Wh_Log(L"Couldn't find the Desktop folder");
        return false;
    }

    // 1. Create "New group", or "New group (2)" if that name is taken, etc.,
    //    and hide it while we set it up.
    std::wstring groupPath;
    for (int number = 1; number < 100; number++) {
        std::wstring candidate = desktopPath + L"\\New group";
        if (number > 1) {
            candidate += L" (" + std::to_wstring(number) + L")";
        }
        if (CreateDirectoryW(candidate.c_str(), nullptr)) {
            groupPath = candidate;
            break;
        }
        if (GetLastError() != ERROR_ALREADY_EXISTS) {
            break;
        }
    }
    if (groupPath.empty()) {
        Wh_Log(L"Couldn't create the group folder");
        return false;
    }
    SetFileAttributesW(groupPath.c_str(), FILE_ATTRIBUTE_HIDDEN);

    // Note the folder in the mod's storage until it's finished. If Explorer
    // stops before then (say it crashes while Windows is asking for
    // permission), the folder would be left hidden with some shortcuts
    // already in it. At the next start, RecoverUnfinishedGroup finds this note
    // and puts things right.
    StoreText(L"UnfinishedGroup", groupPath.c_str());
    Wh_Log(L"Created \"%s\"", groupPath.c_str());

    // 2. Draw the icon from the apps where they are now, in the same order
    //    the popup will show them. If this fails, carry on: the group watcher
    //    will draw it a moment later instead.
    std::vector<GroupEntry> apps;
    for (const DesktopItem& item : items) {
        apps.push_back({item.name, item.path});
    }
    std::sort(apps.begin(), apps.end(), [](const GroupEntry& a, const GroupEntry& b) {
        return StrCmpLogicalW(a.name.c_str(), b.name.c_str()) < 0;
    });
    ApplyGroupIcon(groupPath, apps);

    // 3. Move the apps in using IFileOperation, the same engine Explorer uses
    //    when you move files yourself. It handles undo (Ctrl+Z), OneDrive,
    //    and asks for permission if a shortcut belongs to all users.
    ComPtr<IFileOperation> operation;
    ComPtr<IShellItem> destination;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&operation))) ||
        FAILED(SHCreateItemFromParsingName(groupPath.c_str(), nullptr,
                                           IID_PPV_ARGS(&destination)))) {
        Wh_Log(L"Couldn't start the file move");
        CleanUpFailedGroup(groupPath);
        DeleteStoredValue(L"UnfinishedGroup");
        return false;
    }

    operation->SetOperationFlags(FOF_ALLOWUNDO | FOF_NOCONFIRMMKDIR);

    for (const DesktopItem& item : items) {
        ComPtr<IShellItem> source;
        if (SUCCEEDED(SHCreateItemFromParsingName(item.path.c_str(), nullptr,
                                                  IID_PPV_ARGS(&source)))) {
            operation->MoveItem(source.Get(), destination.Get(), nullptr, nullptr);
        }
    }

    HRESULT result = operation->PerformOperations();
    BOOL aborted = FALSE;
    operation->GetAnyOperationsAborted(&aborted);

    if (FAILED(result) || aborted) {
        Wh_Log(L"The move didn't finish (error 0x%08X)", (unsigned)result);
        CleanUpFailedGroup(groupPath);
        DeleteStoredValue(L"UnfinishedGroup");
        return false;
    }

    // 4. Only mark it as a group once everything is safely inside (so the
    //    watcher never sees a "group" with one app mid-move and removes it),
    //    then unhide it.
    MarkAsGroup(groupPath);
    RemoveAttributes(groupPath, FILE_ATTRIBUTE_HIDDEN);
    *groupPathOut = groupPath;
    DeleteStoredValue(L"UnfinishedGroup");  // Finished.
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, groupPath.c_str(),
                   nullptr);

    // Have the group watcher look at the new group once it's complete. (It
    // also hears about the apps arriving, but that's before the group was
    // marked, so it might have skipped it.)
    RequestGroupCheck(groupPath);
    return true;
}

// ---- 7. Watching groups -----------------------------------------------------
//
// Group icons should update whenever a group's contents change, however that
// happens: our own drop, dragging something into a group, deleting something
// in an Explorer window...
//
// So a background thread asks Windows to report every file added, removed or
// renamed anywhere inside the Desktop folder (ReadDirectoryChangesW). When
// something changes inside a group, it waits until things have been quiet
// for 100 ms (one drop can cause several changes within a few
// milliseconds), then checks that group once.
//
// Writing a new icon file counts as a change too, so icon files are ignored.
// Otherwise each new icon would trigger another new icon, forever.
//
// Like Start menu folders, a group that's down to one app is removed (unless
// that's turned off in the group settings). The app goes back on the desktop
// and the empty group is deleted.

HANDLE g_watcherThread = nullptr;
HANDLE g_watcherStopEvent = nullptr;

// Asking the watcher to check a group straight away.
//
// The watcher only hears about files being added, removed or renamed. Some
// changes, like saving a new app order, don't add or remove any file, so it
// never hears about those. Instead, the popup asks it directly with
// RequestGroupCheck.
//
// The popup and the watcher run on different threads, at the same time. If
// both used the list of requests at once, it could get corrupted. A "mutex"
// (mutual exclusion) prevents that: whoever holds the lock has the list to
// themselves, and the other waits a moment. Then an "event" wakes the watcher
// up, like a doorbell.
std::mutex g_checkRequestsLock;
std::vector<std::wstring> g_checkRequests;  // Group folder paths to check.
HANDLE g_watcherWakeEvent = nullptr;

void RequestGroupCheck(const std::wstring& folderPath) {
    if (!g_watcherWakeEvent) {
        return;  // The watcher isn't running.
    }
    {
        std::lock_guard<std::mutex> lock(g_checkRequestsLock);  // Released at "}".
        g_checkRequests.push_back(folderPath);
    }
    SetEvent(g_watcherWakeEvent);
}

bool IsIconFile(const std::wstring& fileName) {
    return _wcsnicmp(fileName.c_str(), kIconFilePrefix, wcslen(kIconFilePrefix)) == 0;
}

// Removes a group with one app (or none) left. It only ever deletes the
// group's own hidden files (desktop.ini and the icon). If the folder holds
// anything else, like a subfolder or some other hidden file, the group is
// left alone rather than risk deleting something of yours.
void DissolveGroup(const std::wstring& folderPath,
                   const std::vector<GroupEntry>& entries) {
    ULONGLONG startTime = GetTickCount64();
    std::wstring id = GroupIdOf(folderPath);  // While the folder still exists.

    // Safety check: is everything in the folder either the last app, or one
    // of our own files?
    WIN32_FIND_DATAW found;
    HANDLE search = FindFirstFileW((folderPath + L"\\*").c_str(), &found);
    if (search == INVALID_HANDLE_VALUE) {
        return;
    }
    bool onlyExpectedFiles = true;
    size_t visibleFiles = 0;
    do {
        std::wstring name = found.cFileName;
        if (name == L"." || name == L"..") {
            continue;
        }
        bool isFolder = found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
        bool isHidden = found.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
        bool isOurs = _wcsicmp(name.c_str(), L"desktop.ini") == 0 || IsIconFile(name);
        if (isFolder || (isHidden && !isOurs)) {
            onlyExpectedFiles = false;
        } else if (!isHidden) {
            visibleFiles++;
        }
    } while (FindNextFileW(search, &found));
    FindClose(search);

    // Also make sure nothing was added since we looked (visibleFiles should
    // match what CheckGroup saw).
    if (!onlyExpectedFiles || visibleFiles != entries.size()) {
        Wh_Log(L"Not removing \"%s\": it has other files in it", folderPath.c_str());
        return;
    }

    // Remember where the group is, so its last app can go there.
    POINT groupPosition = {};
    bool knowPosition = AskDesktopForPosition(folderPath, &groupPosition);

    // Hide the group first and tell Explorer right away, so
    // it disappears from the desktop the moment removal starts, instead of
    // when Explorer eventually notices it's gone.
    DWORD originalAttributes = GetFileAttributesW(folderPath.c_str());
    SetFileAttributesW(folderPath.c_str(),
                       (originalAttributes & ~FILE_ATTRIBUTE_DIRECTORY) | FILE_ATTRIBUTE_HIDDEN);
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, folderPath.c_str(),
                   nullptr);

    // If anything goes wrong from here, unhide the group again.
    auto unhide = [&]() {
        RemoveAttributes(folderPath, FILE_ATTRIBUTE_HIDDEN);
        SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT,
                       folderPath.c_str(), nullptr);
    };

    // Move the last app back onto the desktop. FOF_RENAMEONCOLLISION picks a
    // new name ("App (2)") if the desktop already has one with the same name.
    if (!entries.empty()) {
        std::wstring desktopPath = folderPath.substr(0, folderPath.find_last_of(L'\\'));
        ComPtr<IFileOperation> operation;
        ComPtr<IShellItem> source;
        ComPtr<IShellItem> destination;
        if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                    IID_PPV_ARGS(&operation))) ||
            FAILED(SHCreateItemFromParsingName(entries[0].path.c_str(), nullptr,
                                               IID_PPV_ARGS(&source))) ||
            FAILED(SHCreateItemFromParsingName(desktopPath.c_str(), nullptr,
                                               IID_PPV_ARGS(&destination)))) {
            unhide();
            return;
        }
        operation->SetOperationFlags(FOF_ALLOWUNDO | FOF_RENAMEONCOLLISION | FOF_SILENT);
        operation->MoveItem(source.Get(), destination.Get(), nullptr, nullptr);
        HRESULT result = operation->PerformOperations();
        BOOL aborted = FALSE;
        operation->GetAnyOperationsAborted(&aborted);
        if (FAILED(result) || aborted) {
            Wh_Log(L"Couldn't move the last app out of \"%s\" (error 0x%08X)",
                   folderPath.c_str(), (unsigned)result);
            unhide();
            return;
        }

        // Deliver the "app moved" notice now. (Event 0 means "no new notice,
        // just send the waiting ones".)
        SHChangeNotify(0, SHCNF_FLUSHNOWAIT, nullptr, nullptr);

        // Put it where the group was. (If the desktop already had an app with
        // the same name, Windows renamed this one, and it lands in the next
        // free spot instead.)
        if (knowPosition) {
            AskDesktopToPlace(desktopPath + L"\\" + FileNameOf(entries[0].path), groupPosition);
        }
    }

    // Delete our own files, then the (now empty) folder. RemoveDirectoryW
    // refuses to remove a folder that isn't empty, as a last safety net.
    search = FindFirstFileW((folderPath + L"\\*").c_str(), &found);
    if (search != INVALID_HANDLE_VALUE) {
        do {
            std::wstring name = found.cFileName;
            if (_wcsicmp(name.c_str(), L"desktop.ini") == 0 || IsIconFile(name)) {
                std::wstring path = folderPath + L"\\" + name;
                SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_NORMAL);
                DeleteFileW(path.c_str());
            }
        } while (FindNextFileW(search, &found));
        FindClose(search);
    }
    DWORD folderAttributes = GetFileAttributesW(folderPath.c_str());
    if (folderAttributes != INVALID_FILE_ATTRIBUTES) {
        // Take off the read-only flag we added in MarkAsGroup.
        SetFileAttributesW(folderPath.c_str(),
                           folderAttributes & ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_DIRECTORY));
    }
    if (RemoveDirectoryW(folderPath.c_str())) {
        ForgetStoredGroup(id);

        // Tell Explorer the folder is gone, right now. Without this, the
        // desktop waits for its own file watching to catch up, and if
        // Explorer still had the folder open, Windows only finishes deleting
        // it once Explorer lets go.
        SHChangeNotify(SHCNE_RMDIR, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, folderPath.c_str(),
                       nullptr);
        Wh_Log(L"Removed group \"%s\" (%s) in %u ms", folderPath.c_str(),
               entries.empty() ? L"it was empty" : L"its last app went back on the desktop",
               (unsigned)(GetTickCount64() - startTime));
    } else {
        Wh_Log(L"Couldn't remove the group folder (error %u)", (unsigned)GetLastError());
        unhide();
    }
}

// Looks at a group after it changed: remove it if it's down to one app,
// otherwise make sure its icon is up to date.
void CheckGroup(const std::wstring& folderPath) {
    if (!IsGroupFolder(folderPath)) {
        return;  // Not a group (or no longer exists).
    }
    // Make sure its data is where the setting says (it may have just been
    // changed, or the group may be from an older version of the mod).
    MoveGroupData(folderPath, GroupIdOf(folderPath));
    std::vector<GroupEntry> entries = ListGroupFiles(folderPath);
    if (GetGroupSetting(folderPath, kSettingRemoveWhenOneLeft) && entries.size() <= 1) {
        DissolveGroup(folderPath, entries);
        return;
    }
    RefreshGroupIcon(folderPath, entries);
}

DWORD WINAPI GroupWatcherThread(LPVOID) {
    // COM must be set up on each thread that uses it (for the icon drawing).
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    std::wstring desktopPath = GetDesktopFolderPath();
    if (desktopPath.empty()) {
        Wh_Log(L"Group watcher: couldn't find the Desktop folder");
        CoUninitialize();
        return 0;
    }

    // First, check every group once: draw icons for any that need one, and
    // remove any that are down to one app.
    for (const std::wstring& group : FindAllGroups(desktopPath)) {
        CheckGroup(desktopPath + L"\\" + group);
    }

    // FILE_FLAG_OVERLAPPED lets us wait for "changes arrived" and "stop" at
    // the same time, instead of being stuck waiting for changes.
    HANDLE directory = CreateFileW(
        desktopPath.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
    if (directory == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Group watcher: couldn't watch the Desktop folder");
        CoUninitialize();
        return 0;
    }

    alignas(DWORD) BYTE buffer[16 * 1024];
    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    std::set<std::wstring> changedGroups;
    bool reading = false;

    while (true) {
        if (!reading) {
            ResetEvent(overlapped.hEvent);
            if (!ReadDirectoryChangesW(directory, buffer, sizeof(buffer), TRUE,
                                       FILE_NOTIFY_CHANGE_FILE_NAME |
                                           FILE_NOTIFY_CHANGE_DIR_NAME,
                                       nullptr, &overlapped, nullptr)) {
                Wh_Log(L"Group watcher: stopped (error %u)", (unsigned)GetLastError());
                break;
            }
            reading = true;
        }

        // Wait for: the mod stopping, changes arriving, a request from the
        // popup, or (if changes are waiting to be handled) 100 ms of quiet.
        HANDLE events[] = {g_watcherStopEvent, overlapped.hEvent, g_watcherWakeEvent};
        DWORD timeout = changedGroups.empty() ? INFINITE : 100;
        DWORD woke = WaitForMultipleObjects(3, events, FALSE, timeout);

        if (woke == WAIT_OBJECT_0) {
            break;  // The mod is stopping.
        }

        if (woke == WAIT_OBJECT_0 + 2) {
            // The popup asked us to check some groups. Take the list (holding
            // the lock only briefly), then check them straight away.
            std::vector<std::wstring> requests;
            {
                std::lock_guard<std::mutex> lock(g_checkRequestsLock);
                requests.swap(g_checkRequests);
            }
            for (const std::wstring& groupPath : requests) {
                CheckGroup(groupPath);
            }
            continue;
        }

        if (woke == WAIT_TIMEOUT) {
            for (const std::wstring& group : changedGroups) {
                CheckGroup(desktopPath + L"\\" + group);
            }
            changedGroups.clear();
            continue;
        }

        // Changes arrived.
        reading = false;
        DWORD bytes = 0;
        if (!GetOverlappedResult(directory, &overlapped, &bytes, FALSE)) {
            break;
        }
        if (bytes == 0) {
            // Too many changes at once to list. Just check every group.
            for (const std::wstring& group : FindAllGroups(desktopPath)) {
                changedGroups.insert(group);
            }
            continue;
        }

        // The buffer holds a chain of FILE_NOTIFY_INFORMATION records, each
        // with a path relative to the Desktop, like "New group\Notepad.lnk".
        BYTE* position = buffer;
        while (true) {
            auto* change = (FILE_NOTIFY_INFORMATION*)position;
            std::wstring path(change->FileName, change->FileNameLength / sizeof(WCHAR));

            size_t firstSlash = path.find(L'\\');
            if (firstSlash == std::wstring::npos) {
                // Something directly on the desktop. If a group was renamed,
                // refresh it so it's tracked under its new name.
                if (change->Action == FILE_ACTION_RENAMED_NEW_NAME) {
                    changedGroups.insert(path);
                }
            } else {
                // Something inside a folder. Only direct contents matter, and
                // not icon files.
                std::wstring fileName = path.substr(firstSlash + 1);
                if (fileName.find(L'\\') == std::wstring::npos && !IsIconFile(fileName)) {
                    changedGroups.insert(path.substr(0, firstSlash));
                }
            }

            if (change->NextEntryOffset == 0) {
                break;
            }
            position += change->NextEntryOffset;
        }
    }

    if (reading) {
        CancelIoEx(directory, &overlapped);
        DWORD bytes = 0;
        GetOverlappedResult(directory, &overlapped, &bytes, TRUE);
    }
    CloseHandle(overlapped.hEvent);
    CloseHandle(directory);
    CoUninitialize();
    return 0;
}

void StartGroupWatcher() {
    if (g_watcherThread) {
        return;  // Already running.
    }
    if (!g_watcherStopEvent) {
        g_watcherStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    }
    ResetEvent(g_watcherStopEvent);
    if (!g_watcherWakeEvent) {
        // "Auto-reset": it switches itself off again once the watcher wakes.
        g_watcherWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    }
    g_watcherThread = CreateThread(nullptr, 0, GroupWatcherThread, nullptr, 0, nullptr);
}

void StopGroupWatcher() {
    if (g_watcherThread) {
        SetEvent(g_watcherStopEvent);
        WaitForSingleObject(g_watcherThread, INFINITE);
        CloseHandle(g_watcherThread);
        g_watcherThread = nullptr;
    }
    if (g_watcherStopEvent) {
        CloseHandle(g_watcherStopEvent);
        g_watcherStopEvent = nullptr;
    }
    if (g_watcherWakeEvent) {
        CloseHandle(g_watcherWakeEvent);
        g_watcherWakeEvent = nullptr;
    }
}

// ---- 8. Watching drags ------------------------------------------------------
//
// Every drag-and-drop in Windows goes through one function, DoDragDrop. It
// takes an "IDropSource" object, which DoDragDrop keeps asking "should the
// drag continue?" every time the mouse moves or a key/button changes.
//
// We "hook" DoDragDrop so Windhawk sends every call to our function first. If
// the drag started on the desktop, we wrap Explorer's IDropSource in our own
// object (DropSourceSpy) that passes every question through to Explorer.
//
// When you let go over another icon and the group trigger is met, the spy
// answers "cancel" instead of "drop". Windows then does nothing, and we create
// the group ourselves after the drag ends.

class DropSourceSpy final : public IDropSource {
public:
    DropSourceSpy(IDropSource* explorerDropSource, HWND list,
                  IFolderView* folderView, std::vector<int> draggedIcons,
                  bool canGroup)
        : m_inner(explorerDropSource),
          m_list(list),
          m_folderView(folderView),
          m_draggedIcons(std::move(draggedIcons)),
          m_canGroup(canGroup) {
        m_inner->AddRef();
    }

    // Results, read after the drag finishes.
    bool dropped = false;
    int dropTargetIcon = -1;
    ULONGLONG hoveredMsAtDrop = 0;
    bool groupRequested = false;
    DesktopItem groupTarget;

    // --- IUnknown: reference counting, required by every COM object ---------

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        if (riid == IID_IUnknown || riid == IID_IDropSource) {
            *object = static_cast<IDropSource*>(this);
            AddRef();
            return S_OK;
        }
        // Anything else we don't know about, Explorer's object can answer.
        return m_inner->QueryInterface(riid, object);
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return ++m_refCount; }

    ULONG STDMETHODCALLTYPE Release() override {
        ULONG count = --m_refCount;
        if (count == 0) {
            delete this;
        }
        return count;
    }

    // --- IDropSource: the two questions DoDragDrop asks ---------------------

    // "Should the drag continue, drop, or cancel?"
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL escapePressed,
                                                DWORD keyState) override {
        HRESULT answer = m_inner->QueryContinueDrag(escapePressed, keyState);

        // Keep track of which icon we're over, and since when.
        int hovered = GetOtherIconUnderCursor();
        if (hovered != m_lastHovered) {
            m_lastHovered = hovered;
            m_hoverStartTime = GetTickCount64();
        }

        if (answer == DRAGDROP_S_DROP) {
            dropped = true;
            dropTargetIcon = hovered;
            hoveredMsAtDrop = GetTickCount64() - m_hoverStartTime;

            if (hovered >= 0 && m_canGroup && TriggerMet(keyState)) {
                DesktopItem target;
                if (GetDesktopItem(m_folderView, m_list, hovered, &target) &&
                    CanBeGrouped(target)) {
                    groupRequested = true;
                    groupTarget = target;
                    return DRAGDROP_S_CANCEL;  // Stop Windows' normal drop.
                }
            }
        }
        return answer;
    }

    // "What should the mouse pointer look like?" We don't change it.
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD effect) override {
        return m_inner->GiveFeedback(effect);
    }

private:
    // "virtual" makes `delete this` in Release() safe. Without it, the
    // compiler warns that the wrong destructor could run.
    virtual ~DropSourceSpy() { m_inner->Release(); }

    bool HoveredLongEnough() {
        return GetTickCount64() - m_hoverStartTime >=
               (ULONGLONG)g_settings.hoverDelayMs;
    }

    // Has the user done what the "How to create a group" setting asks?
    bool TriggerMet(DWORD keyState) {
        switch (g_settings.trigger) {
            case GroupTrigger::Always:
                return true;
            case GroupTrigger::Shift:
                return (keyState & MK_SHIFT) != 0;
            case GroupTrigger::Hover:
                return HoveredLongEnough();
        }
        return false;
    }

    // Like GetIconUnderCursor, but ignores the icons being dragged, since the
    // pointer starts out on top of them.
    int GetOtherIconUnderCursor() {
        int icon = GetIconUnderCursor(m_list);
        for (int dragged : m_draggedIcons) {
            if (icon == dragged) {
                return -1;
            }
        }
        return icon;
    }

    IDropSource* m_inner;
    HWND m_list;
    IFolderView* m_folderView;
    std::vector<int> m_draggedIcons;
    bool m_canGroup;
    int m_lastHovered = -1;
    ULONGLONG m_hoverStartTime = 0;
    ULONG m_refCount = 1;
};

// The real DoDragDrop. Windhawk fills this in when it sets up the hook, and
// we call it to let the drag actually happen.
using DoDragDrop_t = HRESULT(WINAPI*)(IDataObject*, IDropSource*, DWORD, DWORD*);
DoDragDrop_t DoDragDrop_Original;

// True while an app is being dragged out of the group popup (section 9). The
// popup runs on the desktop's thread too, so without this the hook below
// would mistake that drag for a desktop icon drag.
bool g_draggingFromPopup = false;

HRESULT WINAPI DoDragDrop_Hook(IDataObject* dataObject, IDropSource* dropSource,
                               DWORD allowedEffects, DWORD* effect) {
    BusyScope busy;  // See "Staying safe while unloading" near the top.
    // Only interested in drags that start on the desktop. The desktop's list
    // runs on its own thread, so if we're on that thread, it's a desktop drag.
    HWND list = g_desktopList;
    bool isDesktopDrag = list && IsWindow(list) && !g_draggingFromPopup &&
                         GetWindowThreadProcessId(list, nullptr) == GetCurrentThreadId();
    if (!isDesktopDrag) {
        return DoDragDrop_Original(dataObject, dropSource, allowedEffects, effect);
    }

    // Work out which files are being dragged, and whether they can be grouped.
    std::vector<int> draggedIcons = GetSelectedIcons(list);
    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    if (!folderView) {
        Wh_Log(L"Couldn't reach the desktop's folder view, so grouping is off "
               L"for this drag");
    }

    std::vector<DesktopItem> draggedItems;
    bool canGroup = folderView && !draggedIcons.empty();
    for (int icon : draggedIcons) {
        DesktopItem item;
        if (folderView && GetDesktopItem(folderView.Get(), list, icon, &item)) {
            if (!CanBeGrouped(item)) {
                canGroup = false;  // A folder, or a special icon like the Recycle Bin.
            }
            draggedItems.push_back(item);
        } else {
            canGroup = false;
        }
    }

    DropSourceSpy* spy = new DropSourceSpy(dropSource, list, folderView.Get(),
                                           draggedIcons, canGroup);
    HRESULT result = DoDragDrop_Original(dataObject, spy, allowedEffects, effect);

    if (spy->groupRequested) {
        // The target goes first, then whatever was dragged onto it.
        std::vector<DesktopItem> members = {spy->groupTarget};
        members.insert(members.end(), draggedItems.begin(), draggedItems.end());

        // Remember where the target icon is, so the new group can take its
        // place.
        POINT targetPosition = {};
        bool knowPosition = GetViewItemPosition(spy->groupTarget.path, &targetPosition);

        std::wstring groupPath;
        if (CreateGroup(members, &groupPath) && knowPosition) {
            if (HWND desktopWindow = g_desktopDefView) {
                QueuePlacement(desktopWindow, groupPath, targetPosition);
            }
        }
    }

    spy->Release();
    return result;
}

// ---- 9. The group popup -----------------------------------------------------
//
// The popup is our own window. Creating a window in Windows takes three steps:
//
//   1. Register a "window class": a name plus the function that handles the
//      window's messages (PopupWindowProc below).
//   2. Create the window with CreateWindowExW.
//   3. Handle its messages: WM_PAINT to draw it, WM_MOUSEMOVE for hover
//      effects, WM_LBUTTONUP for clicks, and so on.
//
// We draw everything ourselves with GDI, Windows' classic drawing library.
// To avoid flicker we draw into an off-screen picture first, then copy it to
// the screen in one go ("double buffering").
//
// What you can do in it:
//   - Click the title (or press F2) to rename the group. The text box is a
//     standard Windows "EDIT" control, placed over the title.
//   - Press on an app and move the mouse to drag it out of the group, onto
//     the desktop or anywhere else you can drop files.

// The popup's window class name. It's made unique each time the mod loads
// (see RegisterWindowClasses in section 12).
std::wstring g_popupClassName;
bool g_windowClassesRegistered = false;
HMODULE g_module = nullptr;  // This mod's DLL, needed to register the class.
std::atomic<HWND> g_popupWindow{nullptr};

// Which group's popup closed most recently, and when. Used so that clicking a
// group while its popup is open closes it, instead of closing and reopening.
std::wstring g_lastClosedGroup;
ULONGLONG g_lastClosedTime = 0;

// A message of our own that the rename box sends the popup: "finish
// renaming". wParam is 1 to keep the new name, 0 to cancel. Numbers from
// WM_APP upwards are free for programs to use for their own messages.
constexpr UINT kMsgFinishRename = WM_APP + 1;

// Another message of our own: "close right now, no animation". Used when the
// mod is being unloaded, since its code is about to be removed from Explorer.
constexpr UINT kMsgCloseNow = WM_APP + 2;

// The settings panel (further down) is opened from the popup.
void ToggleSettingsPanel(HWND popup);
void CloseSettingsPanel();
void PositionSettingsPanel();
void RefreshSettingsPanel();
void RetintSettingsPanel();
bool IsSettingsPanelOpen();

struct PopupItem {
    std::wstring name;
    std::wstring path;
    HBITMAP icon = nullptr;
    float x = 0, y = 0;  // Where it's drawn right now (top-left corner). This
                         // differs from its slot while it slides into place.
    TextMask labelMask;  // The name, ready to draw (see RenderTextMask).
};

// Everything the popup needs. Only touched on the desktop's thread.
struct {
    std::wstring title;
    std::wstring folderPath;      // Which group is showing.
    ULONGLONG openedTime = 0;     // When it opened (GetTickCount64).
    std::vector<PopupItem> items;
    int hovered = -1;
    int pressed = -1;             // The app the mouse button went down on, or -1.
    POINT pressPoint = {};        // Where it went down, to tell clicks from drags.
    bool titleHovered = false;
    bool titlePressed = false;
    HWND renameBox = nullptr;     // The text box, while renaming.
    HBRUSH renameBoxBrush = nullptr;
    bool draggingOut = false;     // An app is being dragged out.
    bool reordering = false;      // An app is lifted, being rearranged.
    int dragged = -1;             // Which app is lifted.
    int reorderSlot = -1;         // Where it would land if let go now.
    POINT grabOffset = {};        // Where on the app the mouse grabbed it.
    POINT cursor = {};            // The mouse position while rearranging.
    bool menuOpen = false;        // A right-click menu (or its command) is running.
    IContextMenu2* contextMenu2 = nullptr;  // The open right-click menu's
    IContextMenu3* contextMenu3 = nullptr;  // helpers for submenus (see below).
    UINT dpi = 96;
    bool dark = false;
    bool glass = false;           // Using the blurred background.
    SIZE layoutSize = {};         // The popup's full size. While it's
                                  // animating, the window is smaller.
    int animation = 0;            // 0 = none, 1 = opening, 2 = closing.
    ULONGLONG animationStart = 0;
    RECT iconRect = {};           // The group's icon on screen...
    RECT fullRect = {};           // ...and the popup's full position.
    float contentOpacity = 1;     // Fades the contents in and out.
    RECT itemRect = {};           // The group's icon and label on screen.
    RECT workArea = {};           // The screen, minus the taskbar.
    int columns = 4;              // Layout settings for this group
    int iconSize = 48;            // (see "Group settings" in section 5).
    bool showLabels = true;
    int tint = -1;                // The group's colour (kNoTint for none).
    bool gearHovered = false;
    bool gearPressed = false;
    TextMask titleMask;           // The title, ready to draw...
    std::wstring titleMaskText;   // ...and which text that was.
    bool trackingMouse = false;
    HFONT titleFont = nullptr;
    HFONT labelFont = nullptr;
    HFONT symbolFont = nullptr;   // Windows' icon font, for the pencil symbol.
} g_popup;

// Sizes, in pixels at 100% display scaling. Px() scales them for the
// monitor's actual scaling (150%, 200%, ...).
namespace Layout {
constexpr int padding = 16;
constexpr int titleHeight = 36;
constexpr int iconTop = 10;
constexpr int labelGap = 6;
constexpr int minWidth = 280;
constexpr int screenMargin = 8;
}  // namespace Layout

int Px(int value) {
    return MulDiv(value, g_popup.dpi, 96);
}

// Each app's space in the grid depends on the icon size and whether names
// are shown (both adjustable in the settings panel).
int CellWidth() {
    return g_popup.iconSize + 44;
}

int CellHeight() {
    return g_popup.showLabels ? g_popup.iconSize + 52 : g_popup.iconSize + 20;
}

int PopupColumns() {
    return std::clamp((int)g_popup.items.size(), 1, g_popup.columns);
}

int PopupRows() {
    int count = (int)g_popup.items.size();
    return count == 0 ? 1 : (count + PopupColumns() - 1) / PopupColumns();
}

SIZE PopupSize() {
    int gridWidth = PopupColumns() * Px(CellWidth());
    SIZE size;
    size.cx = std::max(Px(Layout::minWidth), gridWidth + 2 * Px(Layout::padding));
    size.cy = Px(Layout::padding) + Px(Layout::titleHeight) +
              PopupRows() * Px(CellHeight()) + Px(Layout::padding);
    return size;
}

// The title area, which you click to rename the group. The gear button sits
// to its right, on the same row.
RECT PopupTitleRect(int clientWidth) {
    RECT title;
    title.left = Px(Layout::padding);
    title.top = Px(Layout::padding) - Px(4);
    title.right = clientWidth - Px(Layout::padding) - Px(40);
    title.bottom = Px(Layout::padding) + Px(Layout::titleHeight) - Px(8);
    return title;
}

RECT PopupGearRect(int clientWidth) {
    RECT title = PopupTitleRect(clientWidth);
    RECT gear;
    gear.right = clientWidth - Px(Layout::padding);
    gear.left = gear.right - Px(32);
    gear.top = title.top;
    gear.bottom = title.bottom;
    return gear;
}

// Where app number `index` goes inside the popup.
RECT PopupCellRect(int index, int clientWidth) {
    int columns = PopupColumns();
    int gridLeft = (clientWidth - columns * Px(CellWidth())) / 2;
    int column = index % columns;
    int row = index / columns;

    RECT cell;
    cell.left = gridLeft + column * Px(CellWidth());
    cell.top = Px(Layout::padding) + Px(Layout::titleHeight) +
               row * Px(CellHeight());
    cell.right = cell.left + Px(CellWidth());
    cell.bottom = cell.top + Px(CellHeight());
    return cell;
}

int PopupHitTest(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        RECT cell = PopupCellRect(i, client.right);
        if (PtInRect(&cell, point)) {
            return i;
        }
    }
    return -1;
}

bool PopupTitleHitTest(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT title = PopupTitleRect(client.right);
    return PtInRect(&title, point) != FALSE;
}

bool PopupGearHitTest(HWND window, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT gear = PopupGearRect(client.right);
    return PtInRect(&gear, point) != FALSE;
}

// Match the Start menu, which follows the Windows (not app) light/dark mode.
bool IsSystemDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return false;
    }
    return value == 0;
}

// The popup's colours. Hover and pressed highlights are a see-through shade
// (white in dark mode, black in light mode) rather than a solid colour, so
// they look right on both the blurred and the solid background.
struct PopupColors {
    COLORREF background;    // Only used for the solid background.
    COLORREF field;         // Behind the rename box.
    COLORREF text;
    COLORREF dimText;
    COLORREF accent;
    COLORREF shade;
    float hoverOpacity;
    float pressedOpacity;
};

// The popup's background colour, mixed with the group's colour if it has
// one. Not at full strength, so text stays easy to read: a bit less in light
// mode, where strong colours would wash out the dark text.
COLORREF TintedBackground(COLORREF themeColor) {
    if (g_popup.tint == kNoTint) {
        return themeColor;
    }
    return MixColors(themeColor, (COLORREF)g_popup.tint, g_popup.dark ? 0.45f : 0.35f);
}

PopupColors GetPopupColors() {
    COLORREF accent = GetSysColor(COLOR_HIGHLIGHT);
    if (g_popup.dark) {
        return {TintedBackground(RGB(44, 44, 44)), RGB(50, 50, 50), RGB(255, 255, 255),
                RGB(170, 170, 170), accent, RGB(255, 255, 255), 0.10f, 0.05f};
    }
    return {TintedBackground(RGB(249, 249, 249)), RGB(255, 255, 255), RGB(0, 0, 0),
            RGB(96, 96, 96), accent, RGB(0, 0, 0), 0.07f, 0.035f};
}

void CreatePopupFonts() {
    // Start from the font Windows uses for message boxes (Segoe UI), at the
    // right size for this monitor's scaling.
    NONCLIENTMETRICSW metrics = {};
    metrics.cbSize = sizeof(metrics);
    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics,
                               0, g_popup.dpi);

    // Grayscale smoothing instead of ClearType's coloured edges: see
    // RenderTextMask below for why.
    LOGFONTW label = metrics.lfMessageFont;
    label.lfQuality = ANTIALIASED_QUALITY;
    g_popup.labelFont = CreateFontIndirectW(&label);

    LOGFONTW title = metrics.lfMessageFont;
    title.lfHeight = MulDiv(title.lfHeight, 4, 3);  // A third bigger.
    title.lfWeight = FW_SEMIBOLD;
    title.lfQuality = ANTIALIASED_QUALITY;
    g_popup.titleFont = CreateFontIndirectW(&title);

    // "Segoe Fluent Icons" is the Windows 11 font of symbols (pencil, gear...).
    // Windows 10 doesn't have it, but has the older "Segoe MDL2 Assets", with
    // the same symbols at the same codes. If Windows gave us a different font
    // than we asked for, it doesn't have the first one, so use the second.
    for (PCWSTR fontName : {L"Segoe Fluent Icons", L"Segoe MDL2 Assets"}) {
        if (g_popup.symbolFont) {
            DeleteObject(g_popup.symbolFont);
        }
        g_popup.symbolFont = CreateFontW(-Px(14), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                         CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                                         DEFAULT_PITCH, fontName);
        WCHAR actualName[LF_FACESIZE] = L"";
        HDC dc = GetDC(nullptr);
        HGDIOBJ previousFont = SelectObject(dc, g_popup.symbolFont);
        GetTextFaceW(dc, LF_FACESIZE, actualName);
        SelectObject(dc, previousFont);
        ReleaseDC(nullptr, dc);
        if (_wcsicmp(actualName, fontName) == 0) {
            break;
        }
    }
}

// Reads the group's apps (see ListGroupFiles in section 5) and each one's
// icon at the right size. Same order as the group's icon.
void LoadGroupItems(const std::wstring& folderPath, int iconPixels) {
    for (const GroupEntry& entry : ListGroupFiles(folderPath)) {
        PopupItem item;
        item.name = entry.name;
        item.path = entry.path;
        item.icon = GetItemIcon(entry.path, iconPixels);
        g_popup.items.push_back(item);
    }
}

void FreePopupResources() {
    for (PopupItem& item : g_popup.items) {
        if (item.icon) {
            DeleteObject(item.icon);
        }
    }
    g_popup.items.clear();
    for (HFONT* font : {&g_popup.titleFont, &g_popup.labelFont, &g_popup.symbolFont}) {
        if (*font) {
            DeleteObject(*font);
            *font = nullptr;
        }
    }
    if (g_popup.renameBoxBrush) {
        DeleteObject(g_popup.renameBoxBrush);
        g_popup.renameBoxBrush = nullptr;
    }
    g_popup.titleMask = {};
    g_popup.titleMaskText.clear();
    g_popup.renameBox = nullptr;  // Windows destroys child windows for us.
    g_popup.hovered = -1;
    g_popup.pressed = -1;
    g_popup.titleHovered = false;
    g_popup.titlePressed = false;
    g_popup.draggingOut = false;
    g_popup.reordering = false;
    g_popup.dragged = -1;
    g_popup.menuOpen = false;
    g_popup.contextMenu2 = nullptr;
    g_popup.contextMenu3 = nullptr;
    g_popup.animation = 0;
    g_popup.contentOpacity = 1;
    g_popup.trackingMouse = false;
}

// --- Drawing text on a see-through background --------------------------------
//
// GDI can't draw see-through text: on the blurred background its letters
// would come out broken. So we let GDI draw the text white-on-black into a
// scratch picture first. How bright each pixel came out says how much the
// text covers it (its "coverage"), and IconCanvas::BlendMask then paints the
// real colour in by that much. The fonts use grayscale smoothing rather than
// ClearType, whose coloured edges would muddle "brightness" and "coverage".
//
// Drawing text this way takes a little work, so each label's coverage is
// worked out once and remembered (PopupItem::labelMask), which keeps the
// sliding animations smooth.
TextMask RenderTextMask(HFONT font, const std::wstring& text, int width, int height,
                        UINT format) {
    TextMask mask;
    if (width <= 0 || height <= 0) {
        return mask;
    }

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;  // Negative = top row first.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        return mask;
    }
    memset(bits, 0, (size_t)width * height * 4);  // Black.

    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ previousBitmap = SelectObject(dc, bitmap);
    HGDIOBJ previousFont = SelectObject(dc, font);
    SetTextColor(dc, RGB(255, 255, 255));
    SetBkMode(dc, TRANSPARENT);
    RECT area = {0, 0, width, height};
    DrawTextW(dc, text.c_str(), -1, &area, format);
    GdiFlush();  // Make sure GDI has finished drawing before we read the pixels.

    mask.width = width;
    mask.height = height;
    mask.coverage.resize((size_t)width * height);
    const Pixel* pixels = (const Pixel*)bits;
    for (size_t i = 0; i < mask.coverage.size(); i++) {
        mask.coverage[i] = std::max({pixels[i].r, pixels[i].g, pixels[i].b});
    }

    SelectObject(dc, previousFont);
    SelectObject(dc, previousBitmap);
    DeleteDC(dc);
    DeleteObject(bitmap);
    return mask;
}

// Draws one app: an optional rounded background, the icon, and the name
// underneath (up to two lines).
void DrawPopupApp(IconCanvas& canvas, PopupItem& item, const RECT& cell,
                  COLORREF fillColor, float fillOpacity, const PopupColors& colors) {
    if (fillOpacity > 0) {
        canvas.FillRoundedRect((float)cell.left + Px(2), (float)cell.top + Px(2),
                               (float)cell.right - Px(2), (float)cell.bottom - Px(2),
                               (float)Px(6), fillColor, fillOpacity);
    }

    int iconPixels = Px(g_popup.iconSize);
    if (item.icon) {
        canvas.DrawIcon(item.icon, cell.left + (Px(CellWidth()) - iconPixels) / 2,
                        cell.top + Px(Layout::iconTop), iconPixels);
    }

    if (!g_popup.showLabels) {
        return;
    }
    RECT label = {cell.left + Px(4),
                  cell.top + Px(Layout::iconTop) + iconPixels + Px(Layout::labelGap),
                  cell.right - Px(4), cell.bottom - Px(4)};
    int width = label.right - label.left, height = label.bottom - label.top;
    if (item.labelMask.width != width || item.labelMask.height != height) {
        item.labelMask = RenderTextMask(
            g_popup.labelFont, item.name, width, height,
            DT_CENTER | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL | DT_NOPREFIX);
    }
    canvas.BlendMask(item.labelMask, label.left, label.top, colors.text);
}

// Where the lifted app is drawn while rearranging: under the mouse, held at
// the same spot you grabbed it.
RECT FloatingCellRect() {
    RECT cell;
    cell.left = g_popup.cursor.x - g_popup.grabOffset.x;
    cell.top = g_popup.cursor.y - g_popup.grabOffset.y;
    cell.right = cell.left + Px(CellWidth());
    cell.bottom = cell.top + Px(CellHeight());
    return cell;
}

// Draws the whole popup into a canvas, then copies it to the screen in one go
// (so there's no flicker).
// For the open/close animation: resizes the finished picture to the window's
// current size, and fades it. Each target pixel blends the four nearest
// source pixels ("bilinear" sampling), so it looks smooth rather than blocky.
std::vector<Pixel> ScaleAndFade(const IconCanvas& canvas, int width, int height,
                                float opacity) {
    std::vector<Pixel> result((size_t)width * height);
    const std::vector<Pixel>& source = canvas.Pixels();
    int sourceWidth = canvas.Width(), sourceHeight = canvas.Height();

    for (int y = 0; y < height; y++) {
        float sourceY = std::clamp((y + 0.5f) * sourceHeight / height - 0.5f, 0.0f,
                                   (float)(sourceHeight - 1));
        int y0 = (int)sourceY, y1 = std::min(y0 + 1, sourceHeight - 1);
        float fy = sourceY - y0;
        for (int x = 0; x < width; x++) {
            float sourceX = std::clamp((x + 0.5f) * sourceWidth / width - 0.5f, 0.0f,
                                       (float)(sourceWidth - 1));
            int x0 = (int)sourceX, x1 = std::min(x0 + 1, sourceWidth - 1);
            float fx = sourceX - x0;

            const Pixel& a = source[(size_t)y0 * sourceWidth + x0];
            const Pixel& b = source[(size_t)y0 * sourceWidth + x1];
            const Pixel& c = source[(size_t)y1 * sourceWidth + x0];
            const Pixel& d = source[(size_t)y1 * sourceWidth + x1];
            auto mix = [&](BYTE Pixel::*channel) {
                float top = a.*channel + (b.*channel - a.*channel) * fx;
                float bottom = c.*channel + (d.*channel - c.*channel) * fx;
                return (BYTE)std::lround((top + (bottom - top) * fy) * opacity);
            };
            result[(size_t)y * width + x] = {mix(&Pixel::b), mix(&Pixel::g),
                                             mix(&Pixel::r), mix(&Pixel::a)};
        }
    }
    return result;
}

void PaintPopup(HWND window, HDC screen) {
    RECT client;
    GetClientRect(window, &client);
    if (client.right <= 0 || client.bottom <= 0 || g_popup.layoutSize.cx <= 0) {
        return;
    }
    PopupColors colors = GetPopupColors();

    // Always draw the popup at its full size. While it's animating, the
    // window is smaller, and the finished picture is scaled down to fit.
    int fullWidth = g_popup.layoutSize.cx, fullHeight = g_popup.layoutSize.cy;
    IconCanvas canvas(fullWidth, fullHeight);

    // With the blur, the background is left fully see-through, and Windows
    // shows the blurred desktop there. Otherwise, fill it in.
    if (!g_popup.glass) {
        canvas.FillAll(colors.background);
    }

    // Title: the group's name. Hovering shows a highlight and a pencil, to
    // hint that you can click to rename. While renaming, the text box sits on
    // top, and we draw a field behind it with a coloured line underneath, like
    // Windows 11 text fields.
    RECT titleRect = PopupTitleRect(fullWidth);
    bool renaming = g_popup.renameBox != nullptr;
    if (renaming) {
        canvas.FillRoundedRect((float)titleRect.left, (float)titleRect.top,
                               (float)titleRect.right, (float)titleRect.bottom,
                               (float)Px(6), colors.field, 1.0f);
        canvas.FillRoundedRect((float)titleRect.left + Px(4), (float)titleRect.bottom - Px(2),
                               (float)titleRect.right - Px(4), (float)titleRect.bottom,
                               (float)Px(1), colors.accent, 1.0f);
    } else if (g_popup.titleHovered) {
        canvas.FillRoundedRect((float)titleRect.left, (float)titleRect.top,
                               (float)titleRect.right, (float)titleRect.bottom,
                               (float)Px(6), colors.shade,
                               g_popup.titlePressed ? colors.pressedOpacity
                                                    : colors.hoverOpacity);
    }

    if (!renaming) {
        RECT text = {titleRect.left + Px(8), titleRect.top, titleRect.right - Px(32),
                     titleRect.bottom};
        int width = text.right - text.left, height = text.bottom - text.top;
        if (g_popup.titleMaskText != g_popup.title || g_popup.titleMask.width != width ||
            g_popup.titleMask.height != height) {
            g_popup.titleMask = RenderTextMask(
                g_popup.titleFont, g_popup.title, width, height,
                DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            g_popup.titleMaskText = g_popup.title;
        }
        canvas.BlendMask(g_popup.titleMask, text.left, text.top, colors.text);

        if (g_popup.titleHovered) {
            RECT pencil = {titleRect.right - Px(32), titleRect.top, titleRect.right - Px(8),
                           titleRect.bottom};
            TextMask symbol = RenderTextMask(g_popup.symbolFont, L"\xE70F",
                                             pencil.right - pencil.left,
                                             pencil.bottom - pencil.top,
                                             DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            canvas.BlendMask(symbol, pencil.left, pencil.top, colors.dimText);
        }
    }

    // The gear button: highlighted when hovered, and while its panel is open.
    RECT gear = PopupGearRect(fullWidth);
    bool panelOpen = IsSettingsPanelOpen();
    if (g_popup.gearHovered || panelOpen) {
        float strength = (g_popup.gearPressed || panelOpen) ? colors.pressedOpacity * 2
                                                            : colors.hoverOpacity;
        canvas.FillRoundedRect((float)gear.left, (float)gear.top, (float)gear.right,
                               (float)gear.bottom, (float)Px(6), colors.shade, strength);
    }
    TextMask gearSymbol = RenderTextMask(g_popup.symbolFont, L"\xE713", gear.right - gear.left,
                                         gear.bottom - gear.top,
                                         DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    canvas.BlendMask(gearSymbol, gear.left, gear.top,
                     (g_popup.gearHovered || panelOpen) ? colors.text : colors.dimText);

    if (g_popup.items.empty()) {
        RECT message = {0, Px(Layout::padding) + Px(Layout::titleHeight), fullWidth,
                        fullHeight - Px(Layout::padding)};
        TextMask text = RenderTextMask(g_popup.labelFont, L"This group is empty",
                                       message.right - message.left,
                                       message.bottom - message.top,
                                       DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        canvas.BlendMask(text, message.left, message.top, colors.dimText);
    }

    // Each app, drawn where it currently is. The highlight shows which app
    // the mouse is over, a little fainter while the button is held down on
    // it, so you can see a click is in progress.
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        if (g_popup.reordering && i == g_popup.dragged) {
            continue;  // The lifted app is drawn last, on top of the others.
        }
        PopupItem& item = g_popup.items[i];
        RECT cell;
        cell.left = (int)std::lround(item.x);
        cell.top = (int)std::lround(item.y);
        cell.right = cell.left + Px(CellWidth());
        cell.bottom = cell.top + Px(CellHeight());

        float highlight = 0;
        if (!g_popup.reordering && i == g_popup.hovered) {
            highlight = (i == g_popup.pressed) ? colors.pressedOpacity : colors.hoverOpacity;
        }
        DrawPopupApp(canvas, item, cell, colors.shade, highlight, colors);
    }

    // The app being rearranged, "lifted" under the mouse. It gets a mostly
    // solid tile, so it stays readable as it passes over the other apps.
    if (g_popup.reordering) {
        DrawPopupApp(canvas, g_popup.items[g_popup.dragged], FloatingCellRect(),
                     colors.field, 0.92f, colors);
    }

    // Copy the canvas to the screen (scaled and faded, if animating). Its
    // see-through pixels stay see-through, which is what lets the blur show.
    std::vector<Pixel> scaled;
    const Pixel* finished = canvas.Pixels().data();
    if (client.right != fullWidth || client.bottom != fullHeight ||
        g_popup.contentOpacity < 1) {
        scaled = ScaleAndFade(canvas, client.right, client.bottom, g_popup.contentOpacity);
        finished = scaled.data();
    }

    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = client.right;
    info.bmiHeader.biHeight = -client.bottom;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP buffer = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!buffer || !bits) {
        return;
    }
    memcpy(bits, finished, (size_t)client.right * client.bottom * sizeof(Pixel));
    HDC dc = CreateCompatibleDC(screen);
    HGDIOBJ previousBitmap = SelectObject(dc, buffer);
    BitBlt(screen, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, previousBitmap);
    DeleteDC(dc);
    DeleteObject(buffer);
}

void SetPopupHovered(HWND window, int index) {
    if (index != g_popup.hovered) {
        g_popup.hovered = index;
        InvalidateRect(window, nullptr, FALSE);  // Ask Windows to repaint.
    }
}

void LaunchPopupItem(HWND window, int index) {
    // Copy the path first: closing the popup frees the item list.
    std::wstring path = g_popup.items[index].path;
    DestroyWindow(window);

    SHELLEXECUTEINFOW info = {};
    info.cbSize = sizeof(info);
    info.lpFile = path.c_str();
    info.nShow = SW_SHOWNORMAL;
    if (!ShellExecuteExW(&info)) {
        Wh_Log(L"Couldn't launch it (error %u)", (unsigned)GetLastError());
    }
}

// --- Renaming ----------------------------------------------------------------

// Windows doesn't allow some characters in file and folder names, or names
// that end in a space or dot. Tidy up whatever was typed.
std::wstring CleanGroupName(std::wstring name) {
    name.erase(std::remove_if(name.begin(), name.end(),
                              [](wchar_t c) {
                                  return c < 32 || wcschr(L"\\/:*?\"<>|", c) != nullptr;
                              }),
               name.end());
    size_t start = name.find_first_not_of(L' ');
    if (start == std::wstring::npos) {
        return L"";
    }
    name = name.substr(start);
    while (!name.empty() && (name.back() == L' ' || name.back() == L'.')) {
        name.pop_back();
    }
    return name;
}

// Renames the group folder. The desktop label updates by itself, and the
// group watcher (section 7) notices the rename and fixes up the icon.
void RenameGroup(const std::wstring& typedName) {
    std::wstring oldPath = g_popup.folderPath;
    size_t lastSlash = oldPath.find_last_of(L'\\');
    std::wstring parent = oldPath.substr(0, lastSlash);
    std::wstring oldName = oldPath.substr(lastSlash + 1);

    std::wstring newName = CleanGroupName(typedName);
    if (newName.empty() || newName == oldName) {
        return;
    }
    std::wstring newPath = parent + L"\\" + newName;

    // Changing only capital letters ("games" to "Games") is fine, but any
    // other name that's already taken isn't.
    if (_wcsicmp(newName.c_str(), oldName.c_str()) != 0 &&
        GetFileAttributesW(newPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Can't rename the group: \"%s\" already exists", newName.c_str());
        MessageBeep(MB_ICONWARNING);
        return;
    }

    // IFileOperation again, so Ctrl+Z on the desktop undoes the rename.
    ComPtr<IFileOperation> operation;
    ComPtr<IShellItem> folder;
    if (FAILED(CoCreateInstance(CLSID_FileOperation, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&operation))) ||
        FAILED(SHCreateItemFromParsingName(oldPath.c_str(), nullptr,
                                           IID_PPV_ARGS(&folder)))) {
        Wh_Log(L"Couldn't start renaming the group");
        return;
    }
    operation->SetOperationFlags(FOF_ALLOWUNDO);
    operation->RenameItem(folder.Get(), newName.c_str(), nullptr);
    HRESULT result = operation->PerformOperations();
    BOOL aborted = FALSE;
    operation->GetAnyOperationsAborted(&aborted);
    if (FAILED(result) || aborted) {
        Wh_Log(L"Couldn't rename the group (error 0x%08X)", (unsigned)result);
        return;
    }

    Wh_Log(L"Renamed group \"%s\" to \"%s\"", oldName.c_str(), newName.c_str());
    g_popup.folderPath = newPath;
    g_popup.title = newName;
    // The apps' paths include the folder name, so update those too.
    for (PopupItem& item : g_popup.items) {
        item.path = newPath + item.path.substr(oldPath.size());
    }
}

// The rename box is a standard Windows EDIT control. It does almost
// everything for us (typing, selecting, copy and paste). We subclass it only
// to catch Enter (keep the name), Esc (cancel) and clicking away (keep).
// Instead of finishing straight away, it posts a message to the popup: a
// window shouldn't be destroyed while it's still handling one of its own
// messages.
LRESULT CALLBACK RenameBoxProc(HWND box, UINT message, WPARAM wParam, LPARAM lParam,
                               UINT_PTR id, DWORD_PTR) {
    BusyScope busy;
    switch (message) {
        case WM_KEYDOWN:
            if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                PostMessageW(GetParent(box), kMsgFinishRename, wParam == VK_RETURN, 0);
                return 0;
            }
            break;

        case WM_CHAR:
            // Stop the "ding" Windows plays for Enter/Esc in a one-line box.
            if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
                return 0;
            }
            break;

        case WM_KILLFOCUS:
            PostMessageW(GetParent(box), kMsgFinishRename, 1, 0);
            break;

        case WM_NCDESTROY:
            RemoveWindowSubclass(box, RenameBoxProc, id);
            break;
    }
    return DefSubclassProc(box, message, wParam, lParam);
}

void StartRename(HWND window) {
    if (g_popup.renameBox) {
        return;
    }

    // Size the box to one line of title text, centred in the title area.
    RECT client;
    GetClientRect(window, &client);
    RECT titleRect = PopupTitleRect(client.right);
    HDC dc = GetDC(window);
    HGDIOBJ previousFont = SelectObject(dc, g_popup.titleFont);
    TEXTMETRICW metrics = {};
    GetTextMetricsW(dc, &metrics);
    SelectObject(dc, previousFont);
    ReleaseDC(window, dc);
    int height = metrics.tmHeight;
    int top = titleRect.top + (titleRect.bottom - titleRect.top - height) / 2;

    HWND box = CreateWindowExW(0, L"EDIT", g_popup.title.c_str(),
                               WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                               titleRect.left + Px(8), top,
                               titleRect.right - titleRect.left - Px(16), height,
                               window, nullptr, g_module, nullptr);
    if (!box) {
        return;
    }
    g_popup.renameBox = box;
    g_popup.renameBoxBrush = CreateSolidBrush(GetPopupColors().field);

    // The text box draws itself with GDI, which on the blurred background
    // would come out see-through and garbled. Making it a "layered" window
    // with full opacity (255) tells Windows to show it solid.
    if (g_popup.glass) {
        SetWindowLongPtrW(box, GWL_EXSTYLE, GetWindowLongPtrW(box, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(box, 0, 255, LWA_ALPHA);
    }

    SendMessageW(box, WM_SETFONT, (WPARAM)g_popup.titleFont, FALSE);
    SendMessageW(box, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, 0);
    SendMessageW(box, EM_LIMITTEXT, 100, 0);
    SetWindowSubclass(box, RenameBoxProc, 1, 0);
    SendMessageW(box, EM_SETSEL, 0, -1);  // Select all, ready to type over.
    SetFocus(box);
    InvalidateRect(window, nullptr, FALSE);
}

// Ends renaming. `keep` applies the typed name. `refocus` gives the keyboard
// back to the popup (so Esc closes it), which only makes sense if the popup
// is still the active window.
void FinishRename(HWND window, bool keep, bool refocus) {
    HWND box = g_popup.renameBox;
    if (!box) {
        return;  // Already finished (this can be asked twice; see above).
    }
    g_popup.renameBox = nullptr;

    WCHAR typed[MAX_PATH] = L"";
    GetWindowTextW(box, typed, ARRAYSIZE(typed));
    if (refocus) {
        SetFocus(window);
    }
    DestroyWindow(box);
    if (g_popup.renameBoxBrush) {
        DeleteObject(g_popup.renameBoxBrush);
        g_popup.renameBoxBrush = nullptr;
    }

    if (keep) {
        RenameGroup(typed);
    }
    InvalidateRect(window, nullptr, FALSE);
}

// --- Dragging apps out -------------------------------------------------------
//
// To drag a file, Windows needs an "IDataObject" describing it. The file's
// IShellItem can make one for us (BindToHandler with BHID_DataObject), and
// SHDoDragDrop runs the drag. Whatever you drop onto (the desktop, a folder,
// the taskbar...) decides what happens; dropping onto the desktop moves the
// app out of the group.
//
// The popup hides while you drag, so you can drop onto the desktop under it.

void DragAppOut(HWND window, int index) {
    std::wstring path = g_popup.items[index].path;
    g_popup.pressed = -1;
    if (GetCapture() == window) {
        ReleaseCapture();
    }

    ComPtr<IShellItem> item;
    ComPtr<IDataObject> data;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) ||
        FAILED(item->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&data)))) {
        Wh_Log(L"Couldn't start dragging \"%s\"", path.c_str());
        return;
    }

    // Show the app's icon under the pointer while dragging.
    ComPtr<IDragSourceHelper> dragImageHelper;
    if (SUCCEEDED(CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&dragImageHelper)))) {
        int size = Px(g_popup.iconSize);
        SHDRAGIMAGE image = {};
        image.sizeDragImage = {size, size};
        image.ptOffset = {size / 2, size / 2};
        image.hbmpDragImage = GetItemIcon(path, size);
        image.crColorKey = CLR_NONE;
        // On success the helper owns the picture; otherwise we delete it.
        if (image.hbmpDragImage &&
            FAILED(dragImageHelper->InitializeFromBitmap(&image, data.Get()))) {
            DeleteObject(image.hbmpDragImage);
        }
    }

    CloseSettingsPanel();
    g_popup.draggingOut = true;
    g_draggingFromPopup = true;
    ShowWindow(window, SW_HIDE);

    DWORD effect = DROPEFFECT_NONE;
    HRESULT result = SHDoDragDrop(window, data.Get(), nullptr,
                                  DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK,
                                  &effect);

    g_draggingFromPopup = false;
    (void)result;  // Whatever happened, the popup closes now.
    DestroyWindow(window);
}

// --- Rearranging apps --------------------------------------------------------
//
// Pressing on an app and moving the mouse "lifts" it: it follows the pointer,
// and the other apps slide aside to show where it would go. Letting go puts
// it there and saves the new order. Moving past the popup's edge switches to
// dragging the app out of the group instead (DragAppOut, above).
//
// The sliding is a simple animation. A timer fires about 60 times a second,
// and each time, every app moves a third of the way from where it's drawn to
// where it belongs. That gives a quick start and a gentle stop.

constexpr UINT_PTR kAnimationTimer = 1;

// The order apps are shown in right now. Normally that's the item list's
// order, but while rearranging, the lifted app is moved to the slot under the
// mouse and the others shuffle along to make room.
std::vector<int> DisplayOrder() {
    std::vector<int> order;
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        if (!g_popup.reordering || i != g_popup.dragged) {
            order.push_back(i);
        }
    }
    if (g_popup.reordering) {
        order.insert(order.begin() + g_popup.reorderSlot, g_popup.dragged);
    }
    return order;
}

// The slot whose centre is nearest the mouse.
int SlotNearest(POINT point, int clientWidth) {
    int nearest = 0;
    long long nearestDistance = LLONG_MAX;
    for (int slot = 0; slot < (int)g_popup.items.size(); slot++) {
        RECT cell = PopupCellRect(slot, clientWidth);
        long long dx = (cell.left + cell.right) / 2 - point.x;
        long long dy = (cell.top + cell.bottom) / 2 - point.y;
        if (dx * dx + dy * dy < nearestDistance) {
            nearestDistance = dx * dx + dy * dy;
            nearest = slot;
        }
    }
    return nearest;
}

// Moves every app one step closer to its slot. Returns false once they've
// all arrived.
bool StepAnimation(HWND window) {
    RECT client;
    GetClientRect(window, &client);
    bool stillMoving = false;
    auto step = [&](float& position, float goal) {
        float distance = goal - position;
        if (std::fabs(distance) < 0.5f) {
            position = goal;
        } else {
            position += distance * 0.35f;
            stillMoving = true;
        }
    };

    std::vector<int> order = DisplayOrder();
    for (int slot = 0; slot < (int)order.size(); slot++) {
        PopupItem& item = g_popup.items[order[slot]];
        RECT target = PopupCellRect(slot, client.right);
        step(item.x, (float)target.left);
        step(item.y, (float)target.top);
    }
    return stillMoving;
}

void StartAnimation(HWND window) {
    SetTimer(window, kAnimationTimer, 15, nullptr);  // Every 15 ms.
}

void SaveGroupOrder(const std::wstring& folderPath, const std::vector<PopupItem>& items) {
    std::wstring order;
    for (const PopupItem& item : items) {
        if (!order.empty()) {
            order += L"|";
        }
        order += FileNameOf(item.path);
    }
    WriteGroupValue(folderPath, L"Order", order.c_str());

    // Ask the group watcher (section 7) to redraw the group's icon now, in
    // the background, so the popup's animation stays smooth. (It only
    // redraws if the first four apps are different.)
    RequestGroupCheck(folderPath);
}

void StartReorder(HWND window, int index, POINT point) {
    RECT client;
    GetClientRect(window, &client);
    RECT cell = PopupCellRect(index, client.right);

    g_popup.reordering = true;
    g_popup.dragged = index;
    g_popup.reorderSlot = index;
    g_popup.grabOffset = {g_popup.pressPoint.x - cell.left, g_popup.pressPoint.y - cell.top};
    g_popup.cursor = point;
    g_popup.pressed = -1;
    g_popup.hovered = -1;
    InvalidateRect(window, nullptr, FALSE);
}

// Ends rearranging. `keep` applies the new order; otherwise the app goes
// back where it was.
void FinishReorder(HWND window, bool keep) {
    if (!g_popup.reordering) {
        return;
    }
    std::vector<int> order = DisplayOrder();

    // Let the lifted app glide from under the mouse into its slot.
    RECT floating = FloatingCellRect();
    g_popup.items[g_popup.dragged].x = (float)floating.left;
    g_popup.items[g_popup.dragged].y = (float)floating.top;
    g_popup.reordering = false;
    g_popup.dragged = -1;

    bool changed = false;
    for (int slot = 0; slot < (int)order.size(); slot++) {
        changed = changed || order[slot] != slot;
    }
    if (keep && changed) {
        std::vector<PopupItem> rearranged;
        for (int index : order) {
            rearranged.push_back(g_popup.items[index]);
        }
        g_popup.items = rearranged;

        // Arranging apps by hand only makes sense with "My order", so if this
        // group was sorted A to Z, switch it.
        if (GetGroupSetting(g_popup.folderPath, kSettingSort) != 0) {
            SetGroupOwnSetting(g_popup.folderPath, kSettingSort, 0);
            RefreshSettingsPanel();
        }
        SaveGroupOrder(g_popup.folderPath, g_popup.items);
    }

    StartAnimation(window);
    InvalidateRect(window, nullptr, FALSE);
}

// --- Right-click menus -------------------------------------------------------
//
// Windows can build the same right-click menu Explorer shows for any file or
// folder. We ask the item's IShellItem for its "IContextMenu" object, have it
// fill an empty menu with its commands, show the menu, and then ask it to run
// whichever command was chosen.
//
// Some entries, like "Open with" and "Send to", only build their submenus
// when you open them, and draw their own icons. For that, the menu object
// needs to see a few of our window's messages while the menu is open
// (IContextMenu2 and IContextMenu3 are versions of it that can take them).
// See the top of PopupWindowProc.

// Command numbers the menu object may use. 0 means "nothing chosen".
constexpr UINT kFirstMenuCommand = 1;
constexpr UINT kLastMenuCommand = 0x7FFF;

void ShowContextMenu(HWND window, const std::wstring& path, POINT screenPoint) {
    ComPtr<IShellItem> item;
    ComPtr<IContextMenu> menu;
    if (FAILED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(&item))) ||
        FAILED(item->BindToHandler(nullptr, BHID_SFUIObject, IID_PPV_ARGS(&menu)))) {
        Wh_Log(L"Couldn't get the right-click menu for \"%s\"", path.c_str());
        return;
    }

    HMENU popupMenu = CreatePopupMenu();
    if (!popupMenu) {
        return;
    }

    // Shift+right-click shows extra commands, as it does on the desktop.
    bool shiftHeld = GetKeyState(VK_SHIFT) < 0;
    UINT flags = CMF_NORMAL | (shiftHeld ? CMF_EXTENDEDVERBS : 0);
    if (FAILED(menu->QueryContextMenu(popupMenu, 0, kFirstMenuCommand, kLastMenuCommand,
                                      flags))) {
        DestroyMenu(popupMenu);
        return;
    }

    ComPtr<IContextMenu2> menu2;
    ComPtr<IContextMenu3> menu3;
    menu.As(&menu2);
    menu.As(&menu3);
    g_popup.contextMenu2 = menu2.Get();
    g_popup.contextMenu3 = menu3.Get();
    g_popup.menuOpen = true;

    // Show the menu and wait until something is chosen (or it's dismissed).
    // TPM_RETURNCMD makes it return the chosen command's number.
    UINT command = TrackPopupMenuEx(popupMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                    screenPoint.x, screenPoint.y, window, nullptr);
    g_popup.contextMenu2 = nullptr;
    g_popup.contextMenu3 = nullptr;

    if (command < kFirstMenuCommand) {
        // Dismissed without choosing anything. If that was by clicking
        // somewhere else (so the popup isn't the active window any more),
        // close the popup too, as clicking away normally would. Otherwise
        // keep it open.
        DestroyMenu(popupMenu);
        g_popup.menuOpen = false;
        if (GetForegroundWindow() != window) {
            DestroyWindow(window);
        }
        return;
    }

    // Like the Start menu, the popup goes away once you pick a command. Hide
    // it now, and destroy it after the command has run.
    CloseSettingsPanel();
    ShowWindow(window, SW_HIDE);


    // Any windows the command opens (like Properties, or "Are you sure?")
    // belong to the desktop rather than the popup, so they don't vanish
    // when the popup is destroyed.
    HWND owner = g_desktopDefView ? (HWND)g_desktopDefView : GetShellWindow();

    CMINVOKECOMMANDINFOEX invoke = {};
    invoke.cbSize = sizeof(invoke);
    invoke.fMask = CMIC_MASK_UNICODE | CMIC_MASK_PTINVOKE |
                   (shiftHeld ? CMIC_MASK_SHIFT_DOWN : 0) |
                   (GetKeyState(VK_CONTROL) < 0 ? CMIC_MASK_CONTROL_DOWN : 0);
    invoke.hwnd = owner;
    // The command is identified by its number, counted from the first one.
    invoke.lpVerb = MAKEINTRESOURCEA(command - kFirstMenuCommand);
    invoke.lpVerbW = MAKEINTRESOURCEW(command - kFirstMenuCommand);
    invoke.nShow = SW_SHOWNORMAL;
    invoke.ptInvoke = screenPoint;
    HRESULT result = menu->InvokeCommand((CMINVOKECOMMANDINFO*)&invoke);
    if (FAILED(result) && result != HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        Wh_Log(L"  ...that command failed (error 0x%08X)", (unsigned)result);
    }

    DestroyMenu(popupMenu);
    DestroyWindow(window);
}

// --- Opening and closing animation -------------------------------------------
//
// Like a Start menu folder, the popup grows out of the group's icon as it
// opens, with its contents fading in, and shrinks back into the icon as it
// closes. A timer moves the window a step at a time (about 60 times a
// second). How far along it should be is worked out from the time since the
// animation started, so it takes the same time even if a step runs late.

constexpr UINT_PTR kOpenCloseTimer = 2;
constexpr int kOpenMilliseconds = 200;
constexpr int kCloseMilliseconds = 150;

// A point `progress` (0 to 1) of the way from one rectangle to another.
RECT BlendRects(const RECT& from, const RECT& to, float progress) {
    auto blend = [&](LONG a, LONG b) { return (LONG)std::lround(a + (b - a) * progress); };
    return {blend(from.left, to.left), blend(from.top, to.top), blend(from.right, to.right),
            blend(from.bottom, to.bottom)};
}

void StepOpenClose(HWND window) {
    bool opening = g_popup.animation == 1;
    int duration = opening ? kOpenMilliseconds : kCloseMilliseconds;
    float time = std::min(1.0f, (GetTickCount64() - g_popup.animationStart) / (float)duration);

    // "Easing" curves, so the motion doesn't start and stop abruptly:
    // opening starts fast and slows down as it settles; closing starts
    // slowly and speeds up as it disappears.
    float progress = opening ? 1 - std::pow(1 - time, 3.0f) : time * time;

    RECT rect = opening ? BlendRects(g_popup.iconRect, g_popup.fullRect, progress)
                        : BlendRects(g_popup.fullRect, g_popup.iconRect, progress);
    g_popup.contentOpacity = opening ? progress : 1 - progress;
    SetWindowPos(window, nullptr, rect.left, rect.top,
                 std::max(1L, rect.right - rect.left), std::max(1L, rect.bottom - rect.top),
                 SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(window, nullptr, FALSE);

    if (time >= 1) {
        KillTimer(window, kOpenCloseTimer);
        if (opening) {
            g_popup.animation = 0;
            g_popup.contentOpacity = 1;
        } else {
            DestroyWindow(window);
        }
    }
}

void StartOpenClose(HWND window, int animation) {
    g_popup.animation = animation;
    g_popup.animationStart = GetTickCount64();
    SetTimer(window, kOpenCloseTimer, 15, nullptr);
    StepOpenClose(window);
}

// Closes the popup by shrinking it back into the group's icon (or straight
// away, if animations are turned off).
void ClosePopupAnimated(HWND window) {
    if (g_popup.animation == 2) {
        return;  // Already closing.
    }
    CloseSettingsPanel();
    FinishReorder(window, false);
    if (GetCapture() == window) {
        ReleaseCapture();
    }
    if (!g_settings.animatePopup || !IsWindowVisible(window)) {
        DestroyWindow(window);
        return;
    }
    // If it's still opening, carry on from wherever it's got to.
    RECT current;
    GetWindowRect(window, &current);
    g_popup.fullRect = current;
    g_popup.hovered = -1;
    StartOpenClose(window, 2);
}

// --- The popup's message handler ---------------------------------------------

// Cancels any dialog box on this thread that belongs to the popup (directly,
// or through the settings panel), as if you'd clicked Cancel.
void CloseDialogsOwnedBy(HWND popup) {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM popupParam) -> BOOL {
            WCHAR className[16] = L"";
            GetClassNameW(window, className, ARRAYSIZE(className));
            if (wcscmp(className, L"#32770") != 0) {  // The class of dialog boxes.
                return TRUE;
            }
            for (HWND owner = GetWindow(window, GW_OWNER); owner;
                 owner = GetWindow(owner, GW_OWNER)) {
                if (owner == (HWND)popupParam) {
                    SendMessageW(window, WM_COMMAND, IDCANCEL, 0);
                    break;
                }
            }
            return TRUE;
        },
        (LPARAM)popup);
}

LRESULT CALLBACK PopupWindowProc(HWND window, UINT message, WPARAM wParam,
                                 LPARAM lParam) {
    BusyScope busy;  // Covers right-click menus, renaming and dragging out.
    // While a right-click menu is open, pass the messages it needs for its
    // submenus and icons to the menu object.
    if ((g_popup.contextMenu2 || g_popup.contextMenu3) &&
        (message == WM_INITMENUPOPUP || message == WM_DRAWITEM ||
         message == WM_MEASUREITEM || message == WM_MENUCHAR)) {
        LRESULT result = 0;
        if (g_popup.contextMenu3 &&
            SUCCEEDED(g_popup.contextMenu3->HandleMenuMsg2(message, wParam, lParam, &result))) {
            return result;
        }
        if (g_popup.contextMenu2 && message != WM_MENUCHAR &&
            SUCCEEDED(g_popup.contextMenu2->HandleMenuMsg(message, wParam, lParam))) {
            return message == WM_INITMENUPOPUP ? 0 : TRUE;
        }
    }

    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            PaintPopup(window, dc);
            EndPaint(window, &paint);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;  // WM_PAINT draws the background, so skip this step.

        case WM_CTLCOLOREDIT: {
            // The rename box asks what colours to use. Match the popup.
            HDC dc = (HDC)wParam;
            PopupColors colors = GetPopupColors();
            SetTextColor(dc, colors.text);
            SetBkColor(dc, colors.field);
            return (LRESULT)g_popup.renameBoxBrush;
        }

        case WM_MOUSEMOVE: {
            if (g_popup.animation) {
                return 0;  // Ignore the mouse while opening or closing.
            }
            // Ask for a WM_MOUSELEAVE message when the mouse leaves.
            if (!g_popup.trackingMouse) {
                TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, window, 0};
                TrackMouseEvent(&track);
                g_popup.trackingMouse = true;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT client;
            GetClientRect(window, &client);

            // While rearranging, the lifted app follows the mouse, and the
            // others make room. Past the popup's edge, it becomes a drag out
            // of the group. (We get mouse messages even outside the popup,
            // because the mouse is "captured" while the button is down.)
            if (g_popup.reordering) {
                if (!PtInRect(&client, point)) {
                    int index = g_popup.dragged;
                    FinishReorder(window, false);
                    DragAppOut(window, index);
                    return 0;
                }
                g_popup.cursor = point;
                int slot = SlotNearest(point, client.right);
                if (slot != g_popup.reorderSlot) {
                    g_popup.reorderSlot = slot;
                    StartAnimation(window);
                }
                InvalidateRect(window, nullptr, FALSE);
                return 0;
            }

            // Pressing on an app and moving a few pixels lifts it to be
            // rearranged (or drags it out, if the mouse is already outside the
            // popup). The distance is Windows' own drag threshold, so it feels
            // the same as dragging anything else.
            if (g_popup.pressed >= 0 && (wParam & MK_LBUTTON) &&
                (std::abs(point.x - g_popup.pressPoint.x) > GetSystemMetrics(SM_CXDRAG) ||
                 std::abs(point.y - g_popup.pressPoint.y) > GetSystemMetrics(SM_CYDRAG))) {
                if (PtInRect(&client, point)) {
                    StartReorder(window, g_popup.pressed, point);
                } else {
                    DragAppOut(window, g_popup.pressed);
                }
                return 0;
            }

            SetPopupHovered(window, PopupHitTest(window, point));
            bool overTitle = PopupTitleHitTest(window, point);
            bool overGear = PopupGearHitTest(window, point);
            if (overTitle != g_popup.titleHovered || overGear != g_popup.gearHovered) {
                g_popup.titleHovered = overTitle;
                g_popup.gearHovered = overGear;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_popup.trackingMouse = false;
            g_popup.titleHovered = false;
            g_popup.gearHovered = false;
            SetPopupHovered(window, -1);
            InvalidateRect(window, nullptr, FALSE);
            return 0;

        // A click only counts if the button went down AND came back up on the
        // same thing, like a real button. This matters because the popup can
        // open right under the mouse: the release of the click that opened it
        // lands on the popup, and must not launch anything.
        case WM_LBUTTONDOWN: {
            if (g_popup.animation) {
                return 0;
            }
            // If you double-click a group out of habit, the first click opens
            // the popup and the second lands on it. Ignore presses that come
            // within Windows' double-click time of the popup opening.
            if (GetTickCount64() - g_popup.openedTime < GetDoubleClickTime()) {
                g_popup.pressed = -1;
                return 0;
            }

            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            g_popup.pressPoint = point;
            g_popup.pressed = PopupHitTest(window, point);
            g_popup.titlePressed = g_popup.pressed < 0 && !g_popup.renameBox &&
                                   PopupTitleHitTest(window, point);
            g_popup.gearPressed = g_popup.pressed < 0 && PopupGearHitTest(window, point);
            if (g_popup.pressed >= 0 || g_popup.titlePressed || g_popup.gearPressed) {
                // "Capture" the mouse, so we still hear about the release
                // even if the pointer is dragged outside the popup first.
                SetCapture(window);
            }
            InvalidateRect(window, nullptr, FALSE);
            return 0;
        }

        case WM_LBUTTONUP: {
            if (g_popup.animation) {
                return 0;
            }
            if (g_popup.reordering) {
                FinishReorder(window, true);  // Drop the lifted app here.
                if (GetCapture() == window) {
                    ReleaseCapture();
                }
                return 0;
            }

            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index = PopupHitTest(window, point);
            int pressed = g_popup.pressed;
            bool titlePressed = g_popup.titlePressed;
            bool gearPressed = g_popup.gearPressed;
            g_popup.pressed = -1;
            g_popup.titlePressed = false;
            g_popup.gearPressed = false;
            if (GetCapture() == window) {
                ReleaseCapture();
            }

            if (gearPressed && PopupGearHitTest(window, point)) {
                ToggleSettingsPanel(window);
            } else if (titlePressed && PopupTitleHitTest(window, point)) {
                StartRename(window);
            } else if (index >= 0 && index == pressed) {
                LaunchPopupItem(window, index);
            } else {
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            // Something else took the mouse (e.g. a window popped up), so
            // the click or rearrange in progress is cancelled.
            FinishReorder(window, false);
            if (g_popup.pressed >= 0 || g_popup.titlePressed || g_popup.gearPressed) {
                g_popup.pressed = -1;
                g_popup.titlePressed = false;
                g_popup.gearPressed = false;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE && g_popup.reordering) {
                // Esc while rearranging puts the app back where it was.
                FinishReorder(window, false);
                if (GetCapture() == window) {
                    ReleaseCapture();
                }
            } else if (wParam == VK_ESCAPE && IsSettingsPanelOpen()) {
                CloseSettingsPanel();  // Esc closes the settings panel first.
            } else if (wParam == VK_ESCAPE) {
                ClosePopupAnimated(window);
            } else if (wParam == VK_F2) {
                StartRename(window);  // F2 renames, as it does in Explorer.
            }
            return 0;

        case kMsgFinishRename:
            FinishRename(window, wParam != 0, true);
            return 0;

        case WM_CLOSE:
            // Posted when the popup loses focus, and sent when the group is
            // clicked again. Shrink back into the icon instead of vanishing.
            ClosePopupAnimated(window);
            return 0;

        case kMsgCloseNow:
            // Sent when the mod is unloading. A right-click menu or a window
            // Windows opened for us (the colour picker, a delete
            // confirmation) would keep the mod's code waiting underneath it,
            // so close those first; then the popup itself.
            EndMenu();
            CloseDialogsOwnedBy(window);
            DestroyWindow(window);
            return 0;

        case WM_TIMER:
            if (wParam == kOpenCloseTimer) {
                StepOpenClose(window);
                return 0;
            }
            if (wParam == kAnimationTimer) {
                if (!StepAnimation(window)) {
                    KillTimer(window, kAnimationTimer);  // Everything's in place.
                }
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;

        case WM_CONTEXTMENU: {
            // Right-click (or the Menu key). On an app: that app's menu.
            // Anywhere else: the group folder's menu.
            if (g_popup.reordering || g_popup.menuOpen || g_popup.animation) {
                return 0;
            }
            FinishRename(window, true, true);

            POINT screenPoint = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index;
            if (screenPoint.x == -1 && screenPoint.y == -1) {
                // From the keyboard, so there's no click position. Use the
                // app under the mouse, and show the menu over it.
                index = g_popup.hovered;
                RECT client;
                GetClientRect(window, &client);
                RECT area = index >= 0 ? PopupCellRect(index, client.right)
                                       : PopupTitleRect(client.right);
                screenPoint = {(area.left + area.right) / 2, (area.top + area.bottom) / 2};
                ClientToScreen(window, &screenPoint);
            } else {
                POINT clientPoint = screenPoint;
                ScreenToClient(window, &clientPoint);
                index = PopupHitTest(window, clientPoint);
            }

            std::wstring path = index >= 0 ? g_popup.items[index].path : g_popup.folderPath;
            ShowContextMenu(window, path, screenPoint);
            return 0;
        }

        case WM_ACTIVATE:
            // While a right-click menu or its command is running, the popup
            // may lose focus (to an "Are you sure?" box, say). It closes
            // itself afterwards, so don't close it here.
            if (LOWORD(wParam) == WA_INACTIVE && !g_popup.menuOpen) {
                // Clicking away while renaming keeps the new name, like in
                // Explorer. Then close, like a menu (unless an app is being
                // dragged out, which hides the popup on purpose). Posting the
                // close lets Windows finish switching windows first.
                FinishRename(window, true, false);
                if (!g_popup.draggingOut) {
                    PostMessageW(window, WM_CLOSE, 0, 0);
                }
            }
            break;

        case WM_NCDESTROY:
            // The very last message a window gets. Clean up here.
            g_lastClosedGroup = g_popup.folderPath;
            g_lastClosedTime = GetTickCount64();
            FreePopupResources();
            g_popupWindow = nullptr;
            break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RegisterPopupClass() {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_DROPSHADOW;  // A soft shadow, like menus have.
    windowClass.lpfnWndProc = PopupWindowProc;
    windowClass.hInstance = g_module;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = g_popupClassName.c_str();
    return RegisterClassExW(&windowClass) != 0;
}

// --- The blurred background --------------------------------------------------
//
// Windows has an official way to ask for a blurred "acrylic" background
// (DWMWA_SYSTEMBACKDROP_TYPE), but on borderless popups like ours it can
// quietly fall back to a plain colour.
//
// Instead we use the "accent policy", an older method that Windows' own
// taskbar uses, as do many Windhawk mods. It's undocumented: Microsoft hasn't
// published it, so we declare its structures ourselves (below) and look the
// function up by name. If it ever disappears, GetProcAddress gives us
// nullptr and the popup simply uses the solid background.

struct AccentPolicy {
    int state;            // 4 = ACCENT_ENABLE_ACRYLICBLURBEHIND.
    DWORD flags;          // 2 = use our tint colour.
    DWORD tintColor;      // 0xAABBGGRR: alpha (AA) is how strong the tint is.
    DWORD animationId;
};

struct WindowCompositionAttributeData {
    int attribute;        // 19 = WCA_ACCENT_POLICY.
    void* data;
    SIZE_T dataSize;
};

using SetWindowCompositionAttribute_t =
    BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

// Is Windows' "Transparency effects" setting (Settings > Personalization >
// Colors) turned on? When it's off, Windows avoids blur everywhere.
bool IsTransparencyEnabled() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value != 0;
}

SetWindowCompositionAttribute_t GetSetWindowCompositionAttribute() {
    // GetProcAddress returns a generic function pointer; going through void*
    // turns it into the specific kind we declared.
    return (SetWindowCompositionAttribute_t)(void*)GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute");
}

// Sets (or changes) the blur and the colour laid over it: the theme's
// background colour, mixed with the group's colour if it has one, at 60%
// strength (0x99) so what's behind still shows through softly.
bool SetBlurTint(HWND window) {
    auto setCompositionAttribute = GetSetWindowCompositionAttribute();
    if (!setCompositionAttribute) {
        return false;
    }
    AccentPolicy accent = {};
    accent.state = 4;
    accent.flags = 2;
    accent.tintColor =
        0x99000000 | TintedBackground(g_popup.dark ? RGB(32, 32, 32) : RGB(243, 243, 243));
    WindowCompositionAttributeData data = {19, &accent, sizeof(accent)};
    return setCompositionAttribute(window, &data) != FALSE;
}

bool EnableBlurBehind(HWND window) {
    if (!IsTransparencyEnabled()) {
        Wh_Log(L"Windows' Transparency effects setting is off, so the popup uses a "
               L"solid background");
        return false;
    }
    if (!GetSetWindowCompositionAttribute()) {
        Wh_Log(L"Blur isn't available on this version of Windows");
        return false;
    }

    // Let Windows use our see-through pixels across the whole window, so the
    // blur shows wherever we leave the canvas see-through.
    MARGINS everywhere = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(window, &everywhere);

    if (!SetBlurTint(window)) {
        Wh_Log(L"Couldn't turn on the blur (error %u)", (unsigned)GetLastError());
        return false;
    }
    return true;
}

// --- Applying group settings to the popup ------------------------------------

// Reads this group's layout settings (see "Group settings" in section 5).
void ReadPopupLayoutSettings() {
    g_popup.columns = std::clamp(GetGroupSetting(g_popup.folderPath, kSettingColumns), 3, 6);
    int iconSize = GetGroupSetting(g_popup.folderPath, kSettingIconSize);
    g_popup.iconSize = iconSize <= 32 ? 32 : (iconSize >= 64 ? 64 : 48);
    g_popup.showLabels = GetGroupSetting(g_popup.folderPath, kSettingShowNames) != 0;
    g_popup.tint = GetGroupSetting(g_popup.folderPath, kSettingTint);
}

// Where the popup goes: beside the group, with its top level with the top of
// the group's icon. To the right if there's room, otherwise to the left, and
// always fully on screen (above the taskbar).
RECT ComputePopupRect(SIZE size) {
    const RECT& work = g_popup.workArea;
    int margin = Px(Layout::screenMargin);
    int gap = Px(8);

    int x = g_popup.itemRect.right + gap;
    if (x + size.cx > work.right - margin) {
        int leftSide = g_popup.itemRect.left - gap - size.cx;
        if (leftSide >= work.left + margin) {
            x = leftSide;
        }
    }
    int y = g_popup.iconRect.top;

    x = std::max((int)(work.left + margin), std::min(x, (int)(work.right - size.cx - margin)));
    y = std::max((int)(work.top + margin), std::min(y, (int)(work.bottom - size.cy - margin)));
    return {x, y, x + size.cx, y + size.cy};
}

// After a setting changes: re-read the layout, fetch icons again if their
// size changed, put the apps in the right order, resize the popup, and let
// the apps slide into their new places.
void ApplyPopupSettings(HWND window) {
    int oldIconSize = g_popup.iconSize;
    int oldTint = g_popup.tint;
    ReadPopupLayoutSettings();

    // A new colour: change the colour over the blur, for the popup and the
    // settings panel. (Without the blur, the next repaint uses it.)
    if (g_popup.tint != oldTint) {
        if (g_popup.glass) {
            SetBlurTint(window);
        }
        RetintSettingsPanel();
    }

    if (g_popup.iconSize != oldIconSize) {
        for (PopupItem& item : g_popup.items) {
            if (item.icon) {
                DeleteObject(item.icon);
            }
            item.icon = GetItemIcon(item.path, Px(g_popup.iconSize));
        }
    }

    // Reorder to match the group's sort setting, keeping each app's icon and
    // current position (so it can slide to its new place).
    std::vector<PopupItem> sorted;
    for (const GroupEntry& entry : ListGroupFiles(g_popup.folderPath)) {
        for (size_t i = 0; i < g_popup.items.size(); i++) {
            if (_wcsicmp(g_popup.items[i].path.c_str(), entry.path.c_str()) == 0) {
                sorted.push_back(g_popup.items[i]);
                g_popup.items.erase(g_popup.items.begin() + i);
                break;
            }
        }
    }
    sorted.insert(sorted.end(), g_popup.items.begin(), g_popup.items.end());
    g_popup.items = sorted;

    SIZE size = PopupSize();
    g_popup.layoutSize = size;
    g_popup.fullRect = ComputePopupRect(size);
    SetWindowPos(window, nullptr, g_popup.fullRect.left, g_popup.fullRect.top, size.cx,
                 size.cy, SWP_NOZORDER | SWP_NOACTIVATE);

    StartAnimation(window);
    PositionSettingsPanel();
    InvalidateRect(window, nullptr, FALSE);
}

// --- The settings panel ------------------------------------------------------
//
// Clicking the gear button opens a panel beside the popup, with the group's
// settings. A switch at the top chooses whether changes are for "This group"
// or "All groups". Changes apply straight away, so you can watch the popup
// change as you go.
//
// The panel is a second window, "owned" by the popup: Windows keeps it above
// the popup and destroys it along with the popup. It's also
// WS_EX_NOACTIVATE, so clicking it doesn't take the focus from the popup
// (which would make the popup close itself, as if you'd clicked away).
//
// Everything in it (the switches, buttons and text) is drawn on a canvas,
// like the popup. To work out what's where, BuildPanelLayout lists every
// piece with its position. The same list is used for drawing and for
// working out what the mouse is over, so the two always agree.

std::wstring g_panelClassName;  // Unique per load, like g_popupClassName.
constexpr UINT_PTR kPanelTimer = 3;
constexpr int kPanelOpenMilliseconds = 150;

enum class PanelAction { Scope, Columns, IconSize, ShowNames, Tint, Sort, RemoveWhenOneLeft, Reset };

// Something you can click: one option of a choice, a toggle switch, or the
// reset button.
struct PanelControl {
    PanelAction action;
    int value = 0;           // For choices: the value this option stands for.
    RECT rect = {};
    std::wstring caption;    // For choices and the reset button.
    bool isToggle = false;
    bool isSwatch = false;   // A colour swatch; `value` is its colour.
};

enum class PanelTextStyle { Title, Section, Row, Note };

struct PanelText {
    std::wstring text;
    RECT rect = {};
    PanelTextStyle style = PanelTextStyle::Row;
    int settingId = -1;      // For rows: which setting (to show the dot).
};

struct PanelLayout {
    SIZE size = {};
    std::vector<PanelControl> controls;
    std::vector<PanelText> texts;
    std::vector<RECT> choiceBackgrounds;  // Behind each set of choices.
};

struct {
    HWND window = nullptr;
    bool allGroups = false;   // The "This group / All groups" switch.
    int hovered = -1;         // Which control the mouse is over.
    int pressed = -1;         // Which control the button went down on.
    bool trackingMouse = false;
    bool glass = false;
    ULONGLONG openedTime = 0;
    float opacity = 1;
    RECT finalRect = {};
    int slideDirection = 1;   // +1: slides out to the right; -1: to the left.
} g_panel;

bool IsSettingsPanelOpen() {
    return g_panel.window != nullptr;
}

// Redraws the panel (if it's open), e.g. after a setting changed elsewhere.
void RefreshSettingsPanel() {
    if (g_panel.window) {
        InvalidateRect(g_panel.window, nullptr, FALSE);
    }
}

// Updates the colour over the panel's blur, after the group's colour changed.
void RetintSettingsPanel() {
    if (g_panel.window) {
        if (g_panel.glass) {
            SetBlurTint(g_panel.window);
        }
        InvalidateRect(g_panel.window, nullptr, FALSE);
    }
}

GroupSettingId SettingForAction(PanelAction action) {
    switch (action) {
        case PanelAction::Columns:
            return kSettingColumns;
        case PanelAction::IconSize:
            return kSettingIconSize;
        case PanelAction::ShowNames:
            return kSettingShowNames;
        case PanelAction::Sort:
            return kSettingSort;
        case PanelAction::Tint:
            return kSettingTint;
        default:
            return kSettingRemoveWhenOneLeft;
    }
}

// The value a control shows: for "All groups", the all-groups value; for
// "This group", whatever applies to this group.
int PanelValue(PanelAction action) {
    if (action == PanelAction::Scope) {
        return g_panel.allGroups ? 1 : 0;
    }
    GroupSettingId id = SettingForAction(action);
    return g_panel.allGroups ? GetAllGroupsSetting(id) : GetGroupSetting(g_popup.folderPath, id);
}

bool GroupHasAnyOwnSettings() {
    for (int id = 0; id < kGroupSettingCount; id++) {
        int value;
        if (GetGroupOwnSetting(g_popup.folderPath, (GroupSettingId)id, &value)) {
            return true;
        }
    }
    return false;
}

PanelLayout BuildPanelLayout() {
    PanelLayout layout;
    int width = Px(320);
    int padding = Px(16);
    int right = width - padding;
    int y = padding;

    auto addText = [&](const std::wstring& text, RECT rect, PanelTextStyle style,
                       int settingId = -1) {
        layout.texts.push_back({text, rect, style, settingId});
    };

    // A row of choices that fill `area`, one option per {value, caption}.
    auto addChoices = [&](PanelAction action,
                          const std::vector<std::pair<int, std::wstring>>& options, RECT area) {
        layout.choiceBackgrounds.push_back(area);
        int count = (int)options.size();
        for (int i = 0; i < count; i++) {
            PanelControl control;
            control.action = action;
            control.value = options[i].first;
            control.caption = options[i].second;
            control.rect = {area.left + (area.right - area.left) * i / count, area.top,
                            area.left + (area.right - area.left) * (i + 1) / count, area.bottom};
            layout.controls.push_back(control);
        }
    };

    // A setting's row: its name on the left, its choices on the right.
    auto addChoiceRow = [&](const std::wstring& name, PanelAction action,
                            const std::vector<std::pair<int, std::wstring>>& options,
                            int optionWidth) {
        int choicesWidth = Px(optionWidth) * (int)options.size();
        addText(name, {padding, y, right - choicesWidth - Px(8), y + Px(40)},
                PanelTextStyle::Row, SettingForAction(action));
        addChoices(action, options, {right - choicesWidth, y + Px(6), right, y + Px(34)});
        y += Px(40);
    };

    auto addToggleRow = [&](const std::wstring& name, PanelAction action) {
        addText(name, {padding, y, right - Px(48), y + Px(40)}, PanelTextStyle::Row,
                SettingForAction(action));
        PanelControl toggle;
        toggle.action = action;
        toggle.isToggle = true;
        toggle.rect = {right - Px(40), y + Px(10), right, y + Px(30)};
        layout.controls.push_back(toggle);
        y += Px(40);
    };

    addText(L"Group settings", {padding, y, right, y + Px(28)}, PanelTextStyle::Title);
    y += Px(36);

    addChoices(PanelAction::Scope, {{0, L"This group"}, {1, L"All groups"}},
               {padding, y, right, y + Px(32)});
    y += Px(48);

    addText(L"Layout", {padding, y, right, y + Px(24)}, PanelTextStyle::Section);
    y += Px(24);
    addChoiceRow(L"Apps per row", PanelAction::Columns,
                 {{3, L"3"}, {4, L"4"}, {5, L"5"}, {6, L"6"}}, 36);
    addChoiceRow(L"Icon size", PanelAction::IconSize,
                 {{32, L"Small"}, {48, L"Medium"}, {64, L"Large"}}, 60);
    addToggleRow(L"Show app names", PanelAction::ShowNames);

    // A row of colour swatches: None, the presets, and Custom.
    y += Px(8);
    addText(L"Colour", {padding, y, right, y + Px(24)}, PanelTextStyle::Section, kSettingTint);
    y += Px(28);
    {
        std::vector<int> swatches = {kNoTint};
        for (COLORREF preset : kTintPresets) {
            swatches.push_back((int)preset);
        }
        swatches.push_back(kCustomTint);

        int count = (int)swatches.size();
        int size = Px(24);
        int spare = (right - padding) - count * size;  // Shared out as gaps.
        for (int i = 0; i < count; i++) {
            int left = padding + i * size + spare * i / (count - 1);
            PanelControl swatch;
            swatch.action = PanelAction::Tint;
            swatch.value = swatches[i];
            swatch.isSwatch = true;
            swatch.rect = {left, y, left + size, y + size};
            layout.controls.push_back(swatch);
        }
        y += Px(32);
    }

    y += Px(8);
    addText(L"Behaviour", {padding, y, right, y + Px(24)}, PanelTextStyle::Section);
    y += Px(24);
    addChoiceRow(L"Sort apps", PanelAction::Sort, {{0, L"My order"}, {1, L"A to Z"}}, 72);
    addToggleRow(L"Remove when one app is left", PanelAction::RemoveWhenOneLeft);

    y += Px(12);
    if (g_panel.allGroups) {
        addText(L"Changes apply to every group.", {padding, y, right, y + Px(32)},
                PanelTextStyle::Note);
    } else {
        PanelControl reset;
        reset.action = PanelAction::Reset;
        reset.caption = L"Reset to all-groups settings";
        reset.rect = {padding, y, right, y + Px(32)};
        layout.controls.push_back(reset);
    }
    y += Px(32) + padding;

    layout.size = {width, y};
    return layout;
}

// A Windows 11-style on/off switch: a pill with a round knob.
void DrawToggle(IconCanvas& canvas, const RECT& rect, bool on, bool hovered,
                const PopupColors& colors) {
    float left = (float)rect.left, top = (float)rect.top;
    float right = (float)rect.right, bottom = (float)rect.bottom;
    float radius = (bottom - top) / 2;

    if (on) {
        canvas.FillRoundedRect(left, top, right, bottom, radius, colors.accent, 1.0f);
    } else {
        canvas.FillRoundedRect(left, top, right, bottom, radius, colors.shade,
                               hovered ? colors.hoverOpacity : colors.pressedOpacity,
                               colors.dimText, 1.0f, (float)Px(1));
    }

    float knob = (bottom - top) - Px(on ? 8 : 10);
    float centerX = on ? right - radius : left + radius;
    float centerY = (top + bottom) / 2;
    canvas.FillRoundedRect(centerX - knob / 2, centerY - knob / 2, centerX + knob / 2,
                           centerY + knob / 2, knob / 2,
                           on ? RGB(255, 255, 255) : colors.dimText, 1.0f);
}

// A colour swatch: a filled circle, with a ring around the chosen one. "None"
// is an empty circle. "Custom" shows a plus sign, or the custom colour once
// one has been picked.
void DrawSwatch(IconCanvas& canvas, const PanelControl& control, bool hovered,
                const PopupColors& colors) {
    const RECT& r = control.rect;
    float centerX = (r.left + r.right) / 2.0f, centerY = (r.top + r.bottom) / 2.0f;
    float outer = (r.right - r.left) / 2.0f;
    float inner = outer - Px(4);

    int current = PanelValue(PanelAction::Tint);
    bool customChosen = current != kNoTint && !IsPresetTint(current);
    bool selected = control.value == kCustomTint ? customChosen : current == control.value;

    // The ring. (A rounded rectangle with no fill, just a border, and a
    // radius of half its width, is a circle outline.)
    if (selected || hovered) {
        canvas.FillRoundedRect(centerX - outer, centerY - outer, centerX + outer,
                               centerY + outer, outer, 0, 0,
                               selected ? colors.text : colors.dimText, 1.0f,
                               (float)Px(selected ? 2 : 1));
    }

    int fill = control.value;
    if (control.value == kCustomTint && customChosen) {
        fill = current;
    }
    if (fill >= 0) {
        canvas.FillRoundedRect(centerX - inner, centerY - inner, centerX + inner,
                               centerY + inner, inner, (COLORREF)fill, 1.0f);
    } else {
        canvas.FillRoundedRect(centerX - inner, centerY - inner, centerX + inner,
                               centerY + inner, inner, 0, 0, colors.dimText, 1.0f,
                               (float)Px(1));
        if (control.value == kCustomTint) {
            TextMask plus = RenderTextMask(g_popup.symbolFont, L"\xE710", r.right - r.left,
                                           r.bottom - r.top,
                                           DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            canvas.BlendMask(plus, r.left, r.top, colors.dimText);
        }
    }
}

void PaintPanel(HWND window, HDC screen) {
    RECT client;
    GetClientRect(window, &client);
    if (client.right <= 0 || client.bottom <= 0) {
        return;
    }
    PopupColors colors = GetPopupColors();
    PanelLayout layout = BuildPanelLayout();
    IconCanvas canvas(client.right, client.bottom);
    if (!g_panel.glass) {
        canvas.FillAll(colors.background);
    }

    // Text: the title, section names, row names, and the note.
    for (const PanelText& text : layout.texts) {
        HFONT font = text.style == PanelTextStyle::Title ? g_popup.titleFont : g_popup.labelFont;
        COLORREF color = (text.style == PanelTextStyle::Section ||
                          text.style == PanelTextStyle::Note)
                             ? colors.dimText
                             : colors.text;
        TextMask mask = RenderTextMask(font, text.text, text.rect.right - text.rect.left,
                                       text.rect.bottom - text.rect.top,
                                       DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                                           DT_NOPREFIX);
        canvas.BlendMask(mask, text.rect.left, text.rect.top, color);

        // A small dot beside settings this group has its own value for.
        int ownValue;
        if (!g_panel.allGroups && text.settingId >= 0 &&
            GetGroupOwnSetting(g_popup.folderPath, (GroupSettingId)text.settingId, &ownValue)) {
            float centerX = (float)text.rect.left - Px(8);
            float centerY = (text.rect.top + text.rect.bottom) / 2.0f;
            float dot = (float)Px(3);
            canvas.FillRoundedRect(centerX - dot, centerY - dot, centerX + dot, centerY + dot,
                                   dot, colors.accent, 1.0f);
        }
    }

    // A faint rounded background behind each set of choices.
    for (const RECT& area : layout.choiceBackgrounds) {
        canvas.FillRoundedRect((float)area.left, (float)area.top, (float)area.right,
                               (float)area.bottom, (float)Px(6), colors.shade,
                               colors.pressedOpacity);
    }

    for (int i = 0; i < (int)layout.controls.size(); i++) {
        const PanelControl& control = layout.controls[i];
        bool hovered = (i == g_panel.hovered);
        const RECT& r = control.rect;

        if (control.isToggle) {
            DrawToggle(canvas, r, PanelValue(control.action) != 0, hovered, colors);
            continue;
        }
        if (control.isSwatch) {
            DrawSwatch(canvas, control, hovered, colors);
            continue;
        }

        COLORREF textColor = colors.text;
        if (control.action == PanelAction::Reset) {
            // A text button. Faded when there's nothing to reset.
            bool enabled = GroupHasAnyOwnSettings();
            if (enabled && hovered) {
                canvas.FillRoundedRect((float)r.left, (float)r.top, (float)r.right,
                                       (float)r.bottom, (float)Px(6), colors.shade,
                                       colors.hoverOpacity);
            }
            textColor = enabled ? colors.accent : colors.dimText;
        } else if (PanelValue(control.action) == control.value) {
            // The chosen option: filled with the accent colour.
            canvas.FillRoundedRect((float)r.left + Px(2), (float)r.top + Px(2),
                                   (float)r.right - Px(2), (float)r.bottom - Px(2),
                                   (float)Px(4), colors.accent, 1.0f);
            textColor = RGB(255, 255, 255);
        } else if (hovered) {
            canvas.FillRoundedRect((float)r.left + Px(2), (float)r.top + Px(2),
                                   (float)r.right - Px(2), (float)r.bottom - Px(2),
                                   (float)Px(4), colors.shade, colors.hoverOpacity);
        }

        TextMask caption = RenderTextMask(g_popup.labelFont, control.caption,
                                          r.right - r.left, r.bottom - r.top,
                                          DT_CENTER | DT_VCENTER | DT_SINGLELINE |
                                              DT_END_ELLIPSIS | DT_NOPREFIX);
        canvas.BlendMask(caption, r.left, r.top, textColor);
    }

    // Copy to the screen, faded while it slides open.
    std::vector<Pixel> faded;
    const Pixel* finished = canvas.Pixels().data();
    if (g_panel.opacity < 1) {
        faded = ScaleAndFade(canvas, client.right, client.bottom, g_panel.opacity);
        finished = faded.data();
    }
    BITMAPINFO info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = client.right;
    info.bmiHeader.biHeight = -client.bottom;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP buffer = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!buffer || !bits) {
        return;
    }
    memcpy(bits, finished, (size_t)client.right * client.bottom * sizeof(Pixel));
    HDC dc = CreateCompatibleDC(screen);
    HGDIOBJ previousBitmap = SelectObject(dc, buffer);
    BitBlt(screen, 0, 0, client.right, client.bottom, dc, 0, 0, SRCCOPY);
    SelectObject(dc, previousBitmap);
    DeleteDC(dc);
    DeleteObject(buffer);
}

// Where the panel goes: beside the popup, on the side away from the group if
// there's room, otherwise the other side. Always kept on screen.
RECT ComputePanelRect(SIZE size) {
    const RECT& popup = g_popup.fullRect;
    const RECT& work = g_popup.workArea;
    int gap = Px(8);
    int margin = Px(Layout::screenMargin);

    bool popupIsRightOfGroup = (popup.left + popup.right) / 2 >=
                               (g_popup.itemRect.left + g_popup.itemRect.right) / 2;
    int rightSide = popup.right + gap;
    int leftSide = popup.left - gap - size.cx;
    int x;
    if (popupIsRightOfGroup) {
        x = (rightSide + size.cx <= work.right - margin) ? rightSide : leftSide;
    } else {
        x = (leftSide >= work.left + margin) ? leftSide : rightSide;
    }
    g_panel.slideDirection = (x >= popup.right) ? 1 : -1;

    x = std::max((int)(work.left + margin), std::min(x, (int)(work.right - size.cx - margin)));
    int y = std::max((int)(work.top + margin),
                     std::min((int)popup.top, (int)(work.bottom - size.cy - margin)));
    return {x, y, x + size.cx, y + size.cy};
}

void PositionSettingsPanel() {
    if (!g_panel.window) {
        return;
    }
    PanelLayout layout = BuildPanelLayout();
    g_panel.finalRect = ComputePanelRect(layout.size);
    SetWindowPos(g_panel.window, nullptr, g_panel.finalRect.left, g_panel.finalRect.top,
                 layout.size.cx, layout.size.cy, SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(g_panel.window, nullptr, FALSE);
}

// Changes a setting, for this group or for all groups (whichever the switch
// says), then updates the popup so you can see the result.
void ChangeGroupSetting(GroupSettingId id, int value) {
    if (g_panel.allGroups) {
        SetAllGroupsSetting(id, value);
        // "All groups" means every group, so any group's own value for this
        // setting is removed. Sorting and colour change how group icons look,
        // so ask the group watcher to redraw those.
        std::wstring desktopPath = GetDesktopFolderPath();
        for (const std::wstring& group : FindAllGroups(desktopPath)) {
            std::wstring groupPath = desktopPath + L"\\" + group;
            int ownValue;
            if (GetGroupOwnSetting(groupPath, id, &ownValue)) {
                ClearGroupOwnSetting(groupPath, id);
            }
            if (id == kSettingSort || id == kSettingTint) {
                RequestGroupCheck(groupPath);
            }
        }
        Wh_Log(L"Set %s to %d for all groups", kGroupSettings[id].key, value);
    } else {
        SetGroupOwnSetting(g_popup.folderPath, id, value);
        if (id == kSettingSort || id == kSettingTint) {
            RequestGroupCheck(g_popup.folderPath);
        }
        Wh_Log(L"Set %s to %d for \"%s\"", kGroupSettings[id].key, value,
               g_popup.title.c_str());
    }

    if (HWND popup = g_popupWindow) {
        ApplyPopupSettings(popup);
    }
}

// Opens Windows' standard colour picker. While it's open, the popup loses
// focus, which would normally make it close; menuOpen tells it to stay open,
// as for right-click menus. Returns false if the picker was cancelled.
bool PickCustomTint(int* tint) {
    // The picker's 16 "custom colours" boxes. "static" keeps them between
    // uses (until Explorer restarts).
    static COLORREF customColors[16] = {};

    HWND popup = g_popupWindow;
    int current = PanelValue(PanelAction::Tint);

    CHOOSECOLORW picker = {};
    picker.lStructSize = sizeof(picker);
    picker.hwndOwner = popup;
    picker.lpCustColors = customColors;
    picker.rgbResult = current >= 0 ? (COLORREF)current : RGB(0, 120, 212);
    picker.Flags = CC_FULLOPEN | CC_RGBINIT | CC_ANYCOLOR;

    g_popup.menuOpen = true;
    BOOL chosen = ChooseColorW(&picker);
    g_popup.menuOpen = false;
    if (popup && IsWindow(popup)) {
        SetForegroundWindow(popup);
    }

    if (!chosen) {
        return false;
    }
    *tint = (int)(picker.rgbResult & 0x00FFFFFF);
    return true;
}

void ActivatePanelControl(const PanelControl& control) {
    switch (control.action) {
        case PanelAction::Scope:
            g_panel.allGroups = (control.value == 1);
            break;

        case PanelAction::Reset:
            if (!GroupHasAnyOwnSettings()) {
                break;
            }
            for (int id = 0; id < kGroupSettingCount; id++) {
                ClearGroupOwnSetting(g_popup.folderPath, (GroupSettingId)id);
            }
            Wh_Log(L"\"%s\" now uses the all-groups settings", g_popup.title.c_str());
            RequestGroupCheck(g_popup.folderPath);
            if (HWND popup = g_popupWindow) {
                ApplyPopupSettings(popup);
            }
            break;

        case PanelAction::Tint: {
            int tint = control.value;
            if (tint == kCustomTint && !PickCustomTint(&tint)) {
                break;
            }
            ChangeGroupSetting(kSettingTint, tint);
            break;
        }

        default: {
            int value = control.isToggle ? (PanelValue(control.action) ? 0 : 1) : control.value;
            ChangeGroupSetting(SettingForAction(control.action), value);
            break;
        }
    }
    if (g_panel.window) {
        InvalidateRect(g_panel.window, nullptr, FALSE);
    }
}

int PanelHitTest(POINT point) {
    PanelLayout layout = BuildPanelLayout();
    for (int i = 0; i < (int)layout.controls.size(); i++) {
        if (PtInRect(&layout.controls[i].rect, point)) {
            return i;
        }
    }
    return -1;
}

// The panel slides a little way out from the popup while fading in.
void StepPanelAnimation(HWND window) {
    float time = std::min(1.0f, (GetTickCount64() - g_panel.openedTime) /
                                    (float)kPanelOpenMilliseconds);
    float progress = 1 - std::pow(1 - time, 3.0f);
    int offset = (int)std::lround((1 - progress) * Px(12)) * -g_panel.slideDirection;
    g_panel.opacity = progress;
    SetWindowPos(window, nullptr, g_panel.finalRect.left + offset, g_panel.finalRect.top, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(window, nullptr, FALSE);
    if (time >= 1) {
        KillTimer(window, kPanelTimer);
        g_panel.opacity = 1;
    }
}

LRESULT CALLBACK PanelWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    BusyScope busy;  // Covers the colour picker.
    switch (message) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;  // Clicking doesn't take the focus.

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            PaintPanel(window, dc);
            EndPaint(window, &paint);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_MOUSEMOVE: {
            if (!g_panel.trackingMouse) {
                TRACKMOUSEEVENT track = {sizeof(track), TME_LEAVE, window, 0};
                TrackMouseEvent(&track);
                g_panel.trackingMouse = true;
            }
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int hovered = PanelHitTest(point);
            if (hovered != g_panel.hovered) {
                g_panel.hovered = hovered;
                InvalidateRect(window, nullptr, FALSE);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_panel.trackingMouse = false;
            g_panel.hovered = -1;
            InvalidateRect(window, nullptr, FALSE);
            return 0;

        case WM_LBUTTONDOWN: {
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            g_panel.pressed = PanelHitTest(point);
            if (g_panel.pressed >= 0) {
                SetCapture(window);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int index = PanelHitTest(point);
            int pressed = g_panel.pressed;
            g_panel.pressed = -1;
            if (GetCapture() == window) {
                ReleaseCapture();
            }
            // Same rule as the popup: press and release on the same control.
            if (index >= 0 && index == pressed) {
                PanelLayout layout = BuildPanelLayout();
                ActivatePanelControl(layout.controls[index]);
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            g_panel.pressed = -1;
            return 0;

        case WM_TIMER:
            if (wParam == kPanelTimer) {
                StepPanelAnimation(window);
            }
            return 0;

        case WM_NCDESTROY:
            g_panel.window = nullptr;
            g_panel.hovered = -1;
            g_panel.pressed = -1;
            g_panel.trackingMouse = false;
            if (HWND popup = g_popupWindow) {
                InvalidateRect(popup, nullptr, FALSE);  // Un-highlight the gear.
            }
            break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RegisterPanelClass() {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_DROPSHADOW;
    windowClass.lpfnWndProc = PanelWindowProc;
    windowClass.hInstance = g_module;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = g_panelClassName.c_str();
    return RegisterClassExW(&windowClass) != 0;
}

void OpenSettingsPanel(HWND popup) {
    if (g_panel.window || !g_windowClassesRegistered) {
        return;
    }
    g_panel.allGroups = false;
    g_panel.hovered = -1;
    g_panel.pressed = -1;

    PanelLayout layout = BuildPanelLayout();
    g_panel.finalRect = ComputePanelRect(layout.size);
    RECT start = g_panel.finalRect;

    // The popup is the "owner" (the parent argument, for a WS_POPUP window).
    HWND panel = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, g_panelClassName.c_str(),
                                 L"Group settings", WS_POPUP, start.left, start.top,
                                 layout.size.cx, layout.size.cy, popup, nullptr, g_module,
                                 nullptr);
    if (!panel) {
        Wh_Log(L"Couldn't open the settings panel (error %u)", (unsigned)GetLastError());
        return;
    }
    g_panel.window = panel;

    // Same styling as the popup: rounded corners, dark border, and blur.
    DWORD cornerPreference = 2;
    DwmSetWindowAttribute(panel, 33, &cornerPreference, sizeof(cornerPreference));
    BOOL darkMode = g_popup.dark;
    DwmSetWindowAttribute(panel, 20, &darkMode, sizeof(darkMode));
    g_panel.glass = g_popup.glass && EnableBlurBehind(panel);

    if (g_settings.animatePopup) {
        g_panel.opacity = 0;
        g_panel.openedTime = GetTickCount64();
        SetTimer(panel, kPanelTimer, 15, nullptr);
        StepPanelAnimation(panel);
    } else {
        g_panel.opacity = 1;
    }
    ShowWindow(panel, SW_SHOWNOACTIVATE);
    InvalidateRect(popup, nullptr, FALSE);  // Highlight the gear.
}

void CloseSettingsPanel() {
    if (HWND panel = g_panel.window) {
        DestroyWindow(panel);
    }
}

void ToggleSettingsPanel(HWND popup) {
    if (g_panel.window) {
        CloseSettingsPanel();
    } else {
        OpenSettingsPanel(popup);
    }
}

// Opens the popup for a group, beside it on the desktop. `anchor` is the
// group's icon and `item` is the icon plus its label, both in screen
// coordinates.
void ShowGroupPopup(const std::wstring& folderPath, const std::wstring& title,
                    const RECT& anchor, const RECT& item) {
    if (HWND existing = g_popupWindow) {
        DestroyWindow(existing);
    }
    if (!g_windowClassesRegistered) {
        return;
    }

    // Which monitor is the group on, and what's its display scaling?
    POINT center = {(anchor.left + anchor.right) / 2, (anchor.top + anchor.bottom) / 2};
    HMONITOR monitor = MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

    g_popup.dpi = dpiX;
    g_popup.dark = IsSystemDarkMode();
    g_popup.title = title;
    g_popup.folderPath = folderPath;
    g_popup.openedTime = GetTickCount64();
    g_popup.hovered = -1;
    g_popup.pressed = -1;
    g_popup.titleHovered = false;
    g_popup.titlePressed = false;
    g_popup.draggingOut = false;
    g_popup.reordering = false;
    g_popup.dragged = -1;
    g_popup.gearHovered = false;
    g_popup.gearPressed = false;
    ReadPopupLayoutSettings();
    LoadGroupItems(folderPath, Px(g_popup.iconSize));
    CreatePopupFonts();

    SIZE size = PopupSize();

    // Every app starts in its own slot, so there's nothing to animate yet.
    // (A WS_POPUP window has no border, so its inside is as wide as it is.)
    for (int i = 0; i < (int)g_popup.items.size(); i++) {
        RECT cell = PopupCellRect(i, size.cx);
        g_popup.items[i].x = (float)cell.left;
        g_popup.items[i].y = (float)cell.top;
    }
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);
    g_popup.workArea = monitorInfo.rcWork;
    g_popup.iconRect = anchor;
    g_popup.itemRect = item;

    g_popup.layoutSize = size;
    g_popup.fullRect = ComputePopupRect(size);
    g_popup.animation = 0;
    g_popup.contentOpacity = 1;

    // With the animation, the window starts out exactly over the group's
    // icon, and grows from there.
    RECT start = g_settings.animatePopup ? anchor : g_popup.fullRect;

    // WS_POPUP: no title bar or border. WS_EX_TOOLWINDOW: no taskbar button.
    HWND window = CreateWindowExW(
        WS_EX_TOOLWINDOW, g_popupClassName.c_str(), title.c_str(), WS_POPUP, start.left, start.top,
        std::max(1L, start.right - start.left), std::max(1L, start.bottom - start.top),
        nullptr, nullptr, g_module, nullptr);
    if (!window) {
        Wh_Log(L"Couldn't create the popup (error %u)", (unsigned)GetLastError());
        FreePopupResources();
        return;
    }
    g_popupWindow = window;

    // Windows 11 window styling: rounded corners, and a dark border in dark
    // mode. The numbers are DWMWA_WINDOW_CORNER_PREFERENCE (33) with
    // DWMWCP_ROUND (2), and DWMWA_USE_IMMERSIVE_DARK_MODE (20).
    DWORD cornerPreference = 2;
    DwmSetWindowAttribute(window, 33, &cornerPreference, sizeof(cornerPreference));
    BOOL darkMode = g_popup.dark;
    DwmSetWindowAttribute(window, 20, &darkMode, sizeof(darkMode));

    // The blurred, see-through background, like the Start menu (see
    // EnableBlurBehind above). If it can't be turned on, the popup is solid.
    g_popup.glass = g_settings.blurredPopup && EnableBlurBehind(window);

    if (g_settings.animatePopup) {
        g_popup.contentOpacity = 0;  // Fully faded until the first step.
    }
    ShowWindow(window, SW_SHOW);
    SetForegroundWindow(window);
    if (g_settings.animatePopup) {
        StartOpenClose(window, 1);
    }
}

// ---- 10. Opening groups ----------------------------------------------------
//
// The icon list tells its parent window, SHELLDLL_DefView, about clicks by
// sending it WM_NOTIFY messages:
//
//   NM_CLICK         - an icon was clicked (pressed and released without
//                      dragging, so clicks that start a drag don't count)
//   LVN_ITEMACTIVATE - an icon was "activated": double-clicked, or Enter
//                      pressed. DefView opens the item when it gets this.
//
// We "subclass" DefView: put our own function in front of its message handler.
// If a group was clicked (or activated), we show our popup and don't pass the
// message on, so DefView never opens the folder window. Everything else passes
// straight through.

// `isClick` is true for a single click (NM_CLICK), false for activation.
bool TryOpenGroup(HWND list, int index, bool isClick) {
    if (index < 0) {
        // Keyboard activation may not say which icon; use the focused one.
        LRESULT focused = -1;
        SendToList(list, LVM_GETNEXTITEM, (WPARAM)-1, LVNI_FOCUSED, &focused);
        index = (int)focused;
    }
    if (index < 0 || GetSelectedIcons(list).size() != 1) {
        return false;  // Nothing, or several icons, opened at once: leave it.
    }

    ComPtr<IFolderView> folderView = GetDesktopFolderView();
    DesktopItem item;
    if (!folderView || !GetDesktopItem(folderView.Get(), list, index, &item) ||
        !item.isDirectory || !IsGroupFolder(item.path)) {
        return false;  // Not a group: let Windows open it normally.
    }

    ULONGLONG now = GetTickCount64();
    HWND openPopup = g_popupWindow;
    if (openPopup && g_popup.folderPath == item.path) {
        // This group's popup is already open. If it only just opened, this is
        // the same click arriving a second time (Windows can send both
        // NM_CLICK and LVN_ITEMACTIVATE for one click), so do nothing.
        // Otherwise it's a fresh click on the group: close the popup, like
        // clicking a Start menu folder again.
        if (now - g_popup.openedTime >= GetDoubleClickTime() && isClick) {
            PostMessageW(openPopup, WM_CLOSE, 0, 0);  // Shrinks back into the icon.
        }
        return true;
    }
    if (isClick && item.path == g_lastClosedGroup && now - g_lastClosedTime < 300) {
        // Clicking the desktop closed this group's popup a moment ago, and
        // this is that same click. Leave it closed.
        return true;
    }

    // Where's the group on screen? The popup opens beside it, and grows out
    // of its icon. LVIR_ICON asks for just the icon; LVIR_BOUNDS for the icon
    // and its label together.
    RECT iconRect = {};
    iconRect.left = LVIR_ICON;
    SendToList(list, LVM_GETITEMRECT, index, (LPARAM)&iconRect);
    MapWindowPoints(list, nullptr, (POINT*)&iconRect, 2);

    RECT itemRect = {};
    itemRect.left = LVIR_BOUNDS;
    SendToList(list, LVM_GETITEMRECT, index, (LPARAM)&itemRect);
    MapWindowPoints(list, nullptr, (POINT*)&itemRect, 2);

    ShowGroupPopup(item.path, item.name, iconRect, itemRect);
    return true;
}

LRESULT CALLBACK DefViewSubclassProc(HWND window, UINT message, WPARAM wParam,
                                     LPARAM lParam, DWORD_PTR) {
    BusyScope busy;
    // Placing icons (section 4): requests from other threads, and the timer.
    // Registered message numbers aren't fixed, so they can't go in a
    // "switch"; plain "if"s instead.
    if (message == g_msgGetItemPosition && message) {
        auto* request = (std::shared_ptr<PositionRequest>*)lParam;
        (*request)->found = GetViewItemPosition((*request)->path, &(*request)->position);
        delete request;  // Our reference; see AskDesktopForPosition.
        return 0;
    }
    if (message == g_msgPlaceItem && message) {
        auto* request = (Placement*)lParam;
        QueuePlacement(window, request->path, request->position);
        delete request;  // See AskDesktopToPlace.
        return 0;
    }
    if (message == g_msgStopPlacing && message) {
        g_placements.clear();
        KillTimer(window, kPlacementTimer);
        return 0;
    }
    if (message == WM_TIMER && wParam == kPlacementTimer) {
        ProcessPlacements(window);
        return 0;
    }

    if (message == WM_NOTIFY) {
        NMHDR* header = (NMHDR*)lParam;
        HWND list = g_desktopList;
        if (header && header->hwndFrom == list) {
            NMITEMACTIVATE* details = (NMITEMACTIVATE*)lParam;

            if (header->code == NM_CLICK && g_settings.openWithSingleClick &&
                details->iItem >= 0) {
                // Ctrl/Shift/Alt+click are for selecting, so leave those alone.
                bool modifierHeld = details->uKeyFlags &
                                    (LVKF_CONTROL | LVKF_SHIFT | LVKF_ALT);
                if (!modifierHeld && TryOpenGroup(list, details->iItem, true)) {
                    return 0;  // Handled.
                }
            }

            if (header->code == LVN_ITEMACTIVATE &&
                TryOpenGroup(list, details->iItem, false)) {
                return 0;  // Handled: don't let DefView open a folder window.
            }
        }
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

// ---- 11. Worker thread ------------------------------------------------------

// If Explorer stopped while a group was being created, CreateGroup's note of
// the folder is still in the mod's storage. If that folder was finished (it's
// marked as a group), it only needs unhiding. Otherwise it's tidied up like a
// failed group: removed if it's empty, or unhidden if shortcuts were already
// moved into it, so they're never left out of sight.
void RecoverUnfinishedGroup() {
    std::wstring pathText = LoadStoredText(L"UnfinishedGroup");
    if (pathText.empty()) {
        return;
    }
    DeleteStoredValue(L"UnfinishedGroup");
    PCWSTR path = pathText.c_str();

    DWORD attributes = GetFileAttributesW(path);
    if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return;
    }
    if (IsGroupFolder(path)) {
        RemoveAttributes(path, FILE_ATTRIBUTE_HIDDEN);
    } else {
        CleanUpFailedGroup(path);
    }
    SHChangeNotify(SHCNE_UPDATEITEM, SHCNF_PATHW | SHCNF_FLUSHNOWAIT, path, nullptr);
    Wh_Log(L"Recovered \"%s\", left unfinished when Explorer stopped", path);
}

// Forgets the stored data of groups whose folders no longer exist: deleted
// while the mod wasn't running, or emptied from the Recycle Bin. (A folder
// in the Recycle Bin still exists, so a group you restore from there is
// still a group.)
//
// Windows can open a file by its ID (OpenFileById), which tells us whether a
// folder with that ID still exists, wherever it is on the drive. To be safe,
// this first checks that it works for the Desktop folder itself, and only
// looks at groups on the same drive as the Desktop.
void ForgetDeletedGroups() {
    HANDLE desktop = OpenForInfo(GetDesktopFolderPath());
    if (desktop == INVALID_HANDLE_VALUE) {
        return;
    }
    std::wstring desktopId = FileIdText(desktop);

    // Turns the hexadecimal digits back into an ID for OpenFileById.
    // (The parts are always in the same places: "V" + 16 digits + "F" + 32.)
    const size_t kDriveLength = 17;
    auto descriptorFor = [&](const std::wstring& id, FILE_ID_DESCRIPTOR* descriptor) {
        size_t start = kDriveLength;
        if (id.size() != kDriveLength + 1 + 32 || id[start] != L'F') {
            return false;
        }
        *descriptor = {};
        descriptor->dwSize = sizeof(*descriptor);
        descriptor->Type = ExtendedFileIdType;
        for (int i = 0; i < 16; i++) {
            std::wstring pair = id.substr(start + 1 + i * 2, 2);
            descriptor->ExtendedFileId.Identifier[i] = (BYTE)wcstoul(pair.c_str(), nullptr, 16);
        }
        return true;
    };
    // 1 = exists, 0 = doesn't, -1 = couldn't tell.
    auto exists = [&](const std::wstring& id) {
        FILE_ID_DESCRIPTOR descriptor;
        if (!descriptorFor(id, &descriptor)) {
            return -1;
        }
        HANDLE file = OpenFileById(desktop, &descriptor, FILE_READ_ATTRIBUTES,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, FILE_FLAG_BACKUP_SEMANTICS);
        if (file != INVALID_HANDLE_VALUE) {
            CloseHandle(file);
            return 1;
        }
        DWORD error = GetLastError();
        return (error == ERROR_INVALID_PARAMETER || error == ERROR_FILE_NOT_FOUND) ? 0 : -1;
    };

    std::vector<std::wstring> gone;
    if (!desktopId.empty() && exists(desktopId) == 1) {
        std::wstring drive = desktopId.substr(0, kDriveLength);
        std::vector<std::wstring> ids;
        {
            std::lock_guard<std::mutex> lock(g_storageLock);
            for (const auto& group : g_groupData) {
                ids.push_back(group.first);
            }
        }
        for (const std::wstring& id : ids) {
            if (id.compare(0, drive.size(), drive) == 0 && exists(id) == 0) {
                gone.push_back(id);
            }
        }
    }
    CloseHandle(desktop);

    for (const std::wstring& id : gone) {
        ForgetStoredGroup(id);
    }
    if (!gone.empty()) {
        Wh_Log(L"Forgot the data of %d deleted group(s)", (int)gone.size());
    }
}

//
// When Windows starts, Windhawk loads the mod before the desktop exists, so we
// keep checking every half second for up to a minute.

HANDLE g_stopEvent = nullptr;     // Signalled when the mod is being unloaded.
HANDLE g_workerThread = nullptr;

DWORD WINAPI WorkerThread(LPVOID) {
    const int maxAttempts = 120;  // 120 x 500 ms = 1 minute
    for (int attempt = 1; attempt <= maxAttempts; attempt++) {
        // Only explain failures on the last attempt, so the log isn't flooded.
        bool lastAttempt = (attempt == maxAttempts);

        HWND list = nullptr;
        switch (FindDesktopListView(&list, lastAttempt)) {
            case SearchResult::Found: {
                // Only the explorer.exe that owns the desktop gets here, so
                // only one copy of the mod does this.
                RecoverUnfinishedGroup();
                ForgetDeletedGroups();
                g_desktopList = list;
                LogDesktopIcons(list);

                // Start watching DefView for double-clicks. The window belongs
                // to another thread, which is why we need Windhawk's
                // "FromAnyThread" version of SetWindowSubclass.
                HWND defView = GetParent(list);
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                        defView, DefViewSubclassProc, 0)) {
                    g_desktopDefView = defView;
                } else {
                    Wh_Log(L"Couldn't watch the desktop for double-clicks");
                }

                // Start keeping group icons up to date. Only the explorer.exe
                // that owns the desktop gets here, so only one copy runs.
                StartGroupWatcher();
                return 0;
            }
            case SearchResult::NotThisProcess:
                return 0;
            case SearchResult::NotYet:
                break;  // Keep trying.
        }

        // Wait 500 ms, but wake up immediately if the mod is being unloaded.
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    Wh_Log(L"Gave up looking for the desktop after 1 minute");
    return 0;
}

void StartWorker() {
    ResetEvent(g_stopEvent);
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
}

void StopWorker() {
    if (!g_workerThread) {
        return;
    }
    SetEvent(g_stopEvent);
    WaitForSingleObject(g_workerThread, INFINITE);
    CloseHandle(g_workerThread);
    g_workerThread = nullptr;
}

// ---- 12. Windhawk callbacks -------------------------------------------------

// Registers the popup's and settings panel's window classes, once per load.
// The names get a number that's different every time the mod loads, so a
// class left behind by an earlier load (whose message handler no longer
// exists) can never be picked up by mistake.
void RegisterWindowClasses() {
    std::wstring suffix = std::to_wstring(GetTickCount64());
    g_popupClassName = L"DesktopAppGroups_Popup_" + suffix;
    g_panelClassName = L"DesktopAppGroups_Settings_" + suffix;
    g_windowClassesRegistered = RegisterPopupClass() && RegisterPanelClass();
    if (!g_windowClassesRegistered) {
        Wh_Log(L"Couldn't register the popup's window classes (error %u)",
               (unsigned)GetLastError());
    }
}

// Called first. Returning FALSE tells Windhawk to unload the mod.
BOOL Wh_ModInit() {
    Wh_Log(L"Desktop App Groups: starting in explorer.exe process %u",
           (unsigned)GetCurrentProcessId());
    LoadSettings();
    g_userPrefix = CurrentUserPrefix();
    LoadAllGroupsSettings();
    LoadStoredGroups();
    RegisterPlacementMessages();

    // Remember which DLL we are, for registering the popup's window class.
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&Wh_ModInit, &g_module);
    RegisterWindowClasses();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    // Find DoDragDrop inside ole32.dll and ask Windhawk to hook it.
    HMODULE ole32 = LoadLibraryW(L"ole32.dll");
    void* doDragDrop = ole32 ? (void*)GetProcAddress(ole32, "DoDragDrop") : nullptr;
    if (!doDragDrop ||
        !Wh_SetFunctionHook(doDragDrop, (void*)DoDragDrop_Hook,
                            (void**)&DoDragDrop_Original)) {
        Wh_Log(L"Couldn't hook DoDragDrop");
        return FALSE;
    }

    return TRUE;
}

// Called once Windhawk has finished setting the mod up, including the hook.
// A safe place to start background work.
void Wh_ModAfterInit() {
    StartWorker();
}

// Called when the user changes a setting. Re-read settings and list the icons
// again so you can see the difference straight away.
void Wh_ModSettingsChanged() {
    bool wasInFolder = g_dataInFolder;
    StopWorker();
    LoadSettings();
    StartWorker();

    // If "Keep group data inside the group folders" changed, have the group
    // watcher move every group's data over now. (If the watcher isn't running
    // yet, it checks every group when it starts anyway.)
    if (g_dataInFolder != wasInFolder && g_watcherThread) {
        std::wstring desktopPath = GetDesktopFolderPath();
        for (const std::wstring& group : FindAllGroups(desktopPath)) {
            RequestGroupCheck(desktopPath + L"\\" + group);
        }
    }
}

// Called when the mod is disabled or Explorer exits. Everything the mod set up
// must be undone here, because the mod's code is about to be removed from
// Explorer. (Windhawk removes the DoDragDrop hook for us.)
void Wh_ModUninit() {
    Wh_Log(L"Desktop App Groups: stopping");
    StopWorker();

    // Stop watching the desktop. First clear the placing to-do list and its
    // timer: timers can only be stopped from the window's own thread, which
    // SendMessage runs this on.
    if (HWND defView = g_desktopDefView) {
        SendMessageW(defView, g_msgStopPlacing, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(defView, DefViewSubclassProc);
        g_desktopDefView = nullptr;
    }

    // Close the popup if it's open. SendMessage runs this on the popup's own
    // thread, which is required for destroying a window. Closing it also ends
    // its right-click menu or colour picker, if one is open.
    if (HWND popup = g_popupWindow) {
        SendMessageW(popup, kMsgCloseNow, 0, 0);
    }

    // Wait for any of the mod's code that's still running to finish (see
    // "Staying safe while unloading" near the top). Usually there's none; if
    // you're mid-drag or answering a prompt, this waits until you're done.
    while (g_busyCount > 0) {
        Sleep(100);
    }

    if (g_windowClassesRegistered) {
        UnregisterClassW(g_popupClassName.c_str(), g_module);
        UnregisterClassW(g_panelClassName.c_str(), g_module);
        g_windowClassesRegistered = false;
    }

    // Stop the watcher last, since the popup can send it requests.
    StopGroupWatcher();

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

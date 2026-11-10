// ==WindhawkMod==
// @id              bookmark-bar-for-file-explorer
// @name            Bookmark Bar for File Explorer
// @description     Adds a Chrome-style bookmarks bar to File Explorer with nested folders, drag & drop and a full Bookmark Manager
// @version         2.5.28
// @author          classiclover75-netizen
// @github          https://github.com/classiclover75-netizen
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lole32 -loleaut32 -lshell32 -luuid -lruntimeobject -lwindowscodecs -lcomctl32 -lgdi32 -luxtheme -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Bookmark Bar for File Explorer

Adds a Google Chrome-style bookmarks bar to Windows 11 File Explorer, placed
between the address bar and the command bar, together with a full-featured
Bookmark Manager modeled on Chrome's.

## Features

### Bookmarks bar
- Bookmark anything you use often, not just folders: apps (.exe), shortcuts
  (.lnk), documents, text files, images, scripts or any other file type.
- Click a folder bookmark to open it in File Explorer; click any other bookmark
  to open it with its default program, just like double-clicking it.
- Organize bookmarks into folders, including folders nested inside folders.
- Folders open as dropdown menus below the bar, with submenus for nested
  folders, just like in Chrome.
- Dropdowns stay open while you rearrange, rename or edit their contents.
- Bookmark folders use a distinct blue folder icon, so they are easy to tell
  apart from regular Windows folders in both light and dark mode.

### Drag & drop
- Drag files or folders from File Explorer onto the bar to bookmark them.
- Reorder bookmarks on the bar and inside folder dropdowns, with a drop
  indicator showing exactly where the item will land.
- Move bookmarks into and out of folders by dragging.

### Right-click menu
- Open in new tab / Open in new window
- Open all / Open all in new window (for folders)
- Edit and Rename
- Cut, Copy, Paste and Delete
- Add Page, Add Folder
- Quick access to the Bookmark Manager

### Bookmark Manager
- Chrome-like layout: folder tree on the left, bookmark list on the right.
- Search across all bookmarks.
- Organize menu (⋮): sort by name, add new bookmark or folder, import and
  export bookmarks.
- Multi-select with a selection bar for bulk delete.
- Drag & drop to reorder bookmarks and move them between folders.
- Undo support for recent changes.

### Keyboard support
- **Ctrl+B** in File Explorer bookmarks the selected item(s), or the current
  folder if nothing is selected (can be disabled in settings).
- **Ctrl+Click** a bookmark to open it in a new tab.
- Bookmark Manager shortcuts: **Ctrl+A**, **Ctrl+X**, **Ctrl+C**, **Ctrl+V**,
  **Ctrl+Z**, **Delete**, **F2** (rename), **Enter** (open), **Backspace**
  (go to parent folder).
- Full keyboard navigation with **Tab** / **Shift+Tab**, the arrow keys, and
  the **Menu** key or **Shift+F10** for context menus.
- Folder dropdowns can be navigated with the arrow keys: **Right** opens a
  submenu, **Left** closes it, **Enter** opens the selected item.

### Theme
- The Bookmark Manager and dialogs follow the Windows light/dark mode
  automatically, or can be set to Light or Dark manually in the mod settings.

## Screenshots
**The bookmarks bar below the address bar**

![The bookmarks bar below the address bar](https://raw.githubusercontent.com/classiclover75-netizen/bookmark-bar-for-file-explorer/main/01.png)

**Folder dropdown**

![Folder dropdown](https://raw.githubusercontent.com/classiclover75-netizen/bookmark-bar-for-file-explorer/main/02.png)

**Right-click menu on an empty area of the bar**

![Right-click menu on an empty area of the bar](https://raw.githubusercontent.com/classiclover75-netizen/bookmark-bar-for-file-explorer/main/03.png)

**Right-click menu on a bookmark**

![Right-click menu on a bookmark](https://raw.githubusercontent.com/classiclover75-netizen/bookmark-bar-for-file-explorer/main/04.png)

**Right-click menu on a folder**

![Right-click menu on a folder](https://raw.githubusercontent.com/classiclover75-netizen/bookmark-bar-for-file-explorer/main/05.png)

**Bookmark Manager**

![Bookmark Manager](https://raw.githubusercontent.com/classiclover75-netizen/bookmark-bar-for-file-explorer/main/06.png)

## Requirements
- Windows 11 with the modern (tabbed) File Explorer.

## Notes
- **Restart File Explorer once after installing or updating the mod.** The bar
  is usually added to already open windows right away, but a restart makes sure
  every window, and the taskbar's Explorer process, starts cleanly with the
  current version. To restart File Explorer:
  1. Press **Ctrl+Shift+Esc** to open Task Manager.
  2. On the **Processes** tab, find **Windows Explorer**.
  3. Right-click it and choose **Restart** (the taskbar disappears for a moment).

  Or from Command Prompt: press **Win+R**, type `cmd` and press **Enter**, then
  run this line (administrator rights are not needed; a normal Command Prompt
  is best, because File Explorer started from an elevated one would also run
  as administrator):

  ```
  taskkill /f /im explorer.exe & start explorer.exe
  ```

  Signing out and back in, or restarting the PC, works too.
- Disabling and enabling the mod again doesn't need a restart: the bar is
  removed as soon as the mod is disabled and comes back when it is enabled.
- Bookmarks are saved in a file in the mod's storage folder, separately for
  each Windows account, and persist across restarts.
- Bookmarks pointing to network locations show a generic icon, so that an
  unreachable server never slows down File Explorer.
- This mod and *Explorer Folder Bookmarks Bar* both add a bar to the same place
  in File Explorer, so please do not enable them at the same time.

## Credits
This mod is based on the source code of
[Explorer Folder Bookmarks Bar](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-folder-bookmarks-bar.wh.cpp)
by Maxim Fomin ([MaxITService](https://github.com/MaxITService)), which is licensed
under the MIT license. It was then modified and extended (bookmarks of any file
type, nested folders, Bookmark Manager, theming). Many thanks to Maxim Fomin for
the original work.

The way the bar is attached to File Explorer windows that are already open
when the mod is enabled follows the approach of
[Explorer Command Bar](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/explorer-command-bar.wh.cpp)
by [DanRotaru](https://github.com/DanRotaru) (MIT license).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- bookmarkHotkey: true
  $name: Enable Ctrl+B Shortcut
  $description: Press Ctrl+B in File Explorer to bookmark the selected items, or the current folder if nothing is selected.
- theme: system
  $name: Theme
  $description: Color mode of the Bookmark Manager and bookmark dialogs.
  $options:
  - light: Light Mode
  - dark: Dark Mode
  - system: System Default
*/
// ==/WindhawkModSettings==

// This mod is based on "Explorer Folder Bookmarks Bar" by Maxim Fomin
// (https://github.com/MaxITService), MIT license, which was modified and
// extended by the author of this mod.

#include <windows.h>
#include <sddl.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <exdisp.h>
#include <oleauto.h>
#include <ocidl.h>
#include <servprov.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <robuffer.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <wincodec.h>
#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cstring>
#include <cwctype>
#include <deque>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <string>
#include <vector>

namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxi = winrt::Microsoft::UI::Xaml::Input;
namespace muxp = winrt::Microsoft::UI::Xaml::Controls::Primitives;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace muxmi = winrt::Microsoft::UI::Xaml::Media::Imaging;
namespace wjson = winrt::Windows::Data::Json;
namespace wdt = winrt::Windows::ApplicationModel::DataTransfer;
namespace wf = winrt::Windows::Foundation;

// Shown in the Bookmark Manager (taken from @version in the header)
constexpr const wchar_t* kModVersion = WH_MOD_VERSION;

constexpr wchar_t kBarName[] = L"WindhawkExplorerChromeBookmarksBar";
constexpr wchar_t kFrameProperty[] = L"WindhawkExplorerChromeBookmarksFrame";
constexpr wchar_t kExportFileName[] = L"explorer-bookmarks-backup.json";
constexpr wchar_t kMgrClass[] = L"WindhawkChromeBookmarkManagerWnd2";
constexpr wchar_t kEditDlgClass[] = L"WindhawkChromeEditBookmarkDlg2";
constexpr wchar_t kChoiceDlgClass[] = L"WindhawkChromeBookmarkChoiceDlg";
constexpr wchar_t kDropLineClass[] = L"WindhawkChromeBookmarkDropLine";

constexpr double kRowHeight = 38.0;
constexpr double kButtonHeight = 30.0;
constexpr double kButtonVerticalInset = (kRowHeight - kButtonHeight) / 2.0;
constexpr double kRowOpticalLift = 3.0;
constexpr int kNavigationOverhang = 6;
constexpr int kMeasuredHostHeightAt96Dpi = 136;
constexpr float kDragThreshold = 6.0f;
constexpr int kMaxDepth = 32;
constexpr size_t kOpenConfirmThreshold = 15;

constexpr UINT kMgrRefreshMsg = WM_APP + 0x51;
constexpr UINT kMgrNavigateMsg = WM_APP + 0x52;
constexpr UINT kMgrThemeMsg = WM_APP + 0x53;

// Bookmark-folder icon tint (Google blue: readable on light and dark backgrounds)
constexpr BYTE kFolderTintR = 66;
constexpr BYTE kFolderTintG = 133;
constexpr BYTE kFolderTintB = 244;
constexpr UINT_PTR kOpenTimerId = 0x57484231;
// Asks Explorer to refresh its window backdrop shortly after the bar was added
constexpr UINT_PTR kBackdropTimerId = 0x57484233;

constexpr CLSID kShellWindows = {
    0x9ba05972, 0xf6a8, 0x11cf,
    {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};
constexpr GUID kTopLevelBrowser = {
    0x4c96be40, 0x915c, 0x11cf,
    {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}};

// ============================================================================
// Data model (recursive: folders can contain folders)
// ============================================================================

enum class NodeType {
    Item,       // File, Folder, or App Shortcut (.exe, .lnk, .txt, etc.)
    FolderBox   // Chrome-style Bookmark Folder
};

struct BookmarkNode {
    NodeType type = NodeType::Item;
    std::wstring name;
    std::wstring path;
    std::vector<BookmarkNode> children;
};

// Path of indices from the bar root. {} = bar root, {2} = 3rd bar item, {2,0} = first child of it.
using NodePath = std::vector<int>;

struct ClipboardState {
    bool hasData = false;
    bool isCut = false;
    std::vector<BookmarkNode> nodes;
};

std::mutex g_dataMutex;
ClipboardState g_clipboard;
std::vector<std::wstring> g_undoStack;
// Per-account bookmarks file in the mod storage folder (set in InitStorage).
std::wstring g_storageFile;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_extensionHooked = false;
std::atomic<bool> g_frameHooked = false;
std::atomic<bool> g_extensionHookAttempted = false;
std::atomic<bool> g_frameHookAttempted = false;
std::atomic<bool> g_bookmarkHotkey = true;
std::atomic<int> g_themeSetting = 0;  // 0 = system, 1 = light, 2 = dark
// Window classes registered by this load (reset on every load, see Wh_ModInit)

// Drag state for rows inside a folder dropdown
struct FlyoutDragCtx {
    bool pressed = false;
    bool dragging = false;
    bool suppressClick = false;
    uint32_t pointerId = 0;
    wf::Point start{};
    bool hasDrop = false;
    NodePath dropFolder;
    int dropIndex = -1;
    std::vector<bool> barFolders;
    muxc::Button indBtn{nullptr};
    mux::Thickness indOldThick{};
    muxm::Brush indOldBrush{nullptr};
};

struct OpenFlyout {
    uint64_t id = 0;
    NodePath folder;
    muxc::Flyout flyout{nullptr};
    std::function<void()> rebuild;
    // Revoked before the flyout is hidden on unload, so no handler (mod code)
    // can run after the bar is removed
    winrt::event_token openedToken{};
    winrt::event_token closedToken{};
};

struct BarState {
    HWND frame = nullptr;
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
    winrt::weak_ref<muxc::Grid> grid;
    winrt::weak_ref<muxc::Grid> hostGrid;
    winrt::weak_ref<mux::FrameworkElement> navControl;
    winrt::weak_ref<muxc::Grid> barRoot;
    winrt::weak_ref<muxc::ScrollViewer> strip;
    winrt::weak_ref<muxc::StackPanel> buttons;
    muxc::RowDefinition addedRow{nullptr};
    muxc::RowDefinition commandRow{nullptr};
    mux::GridLength oldCommandRowHeight{1.0, mux::GridUnitType::Star};
    bool createdFirstRow = false;
    double oldGridMinHeight = 0;
    double oldNavMinHeight = 0;
    double appliedGridMinHeight = 0;
    double appliedNavMinHeight = 0;
    winrt::event_token rootDragOverToken{};
    winrt::event_token rootDropToken{};

    // Bar drag & drop
    int dragSourceIndex = -1;
    uint32_t dragPointerId = 0;
    wf::Point dragStartPt{};
    bool isDragging = false;
    int suppressClickIndex = -1;
    int dropTargetFolderBoxIndex = -1;
    int dropReorderInsertIndex = -1;
    winrt::weak_ref<muxc::Button> highlightedBtn;
    mux::Thickness highlightedOldThickness{};
    muxm::Brush highlightedOldBrush{nullptr};

    // Open folder dropdowns (stay open, refresh in place)
    std::vector<OpenFlyout> openFlyouts;
    bool pendingRefresh = false;
    uint64_t flyoutGen = 0;
};

[[clang::no_destroy]] thread_local std::optional<std::list<BarState>> g_bars{std::in_place};
thread_local unsigned g_frameRows = 0;
thread_local HHOOK g_messageHook = nullptr;
thread_local std::vector<HWND> g_frames;
[[clang::no_destroy]] thread_local std::deque<std::function<void()>> g_frameTasks;
thread_local std::optional<NodePath> g_pendingScope;

void NotifyExplorerBars();
void RefreshThreadBars();
void RefreshPanel(const muxc::StackPanel& panel, const std::optional<NodePath>& scope);
void OpenBookmarkManagerWindow(HWND ownerExplorer, const NodePath& folder);
void RestoreNudgedIslands();
void AddPathsAsBookmarks(HWND owner, const NodePath& folder, const std::vector<std::wstring>& paths);
using ChoiceButtons = std::vector<std::pair<int, std::wstring>>;
int ShowChoiceDialog(HWND owner, const std::wstring& heading, const std::wstring& body, const ChoiceButtons& buttons,
                     int defaultId, int cancelId);

// Win32 callbacks (crash-guarded wrappers; the real code is in *Impl)
LRESULT CALLBACK EditBookmarkWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SearchEditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR idSubclass, DWORD_PTR refData);
LRESULT CALLBACK BookmarkManagerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK FrameSubclassProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, DWORD_PTR);
LRESULT CALLBACK ThreadMessageHook(int code, WPARAM wParam, LPARAM lParam);

BarState* FindBarState(const muxc::StackPanel& panel) {
    if (!g_bars) return nullptr;
    for (auto& state : *g_bars) {
        if (state.buttons.get() == panel) return &state;
    }
    return nullptr;
}

// ============================================================================
// Generic helpers
// ============================================================================

std::wstring NormalizePath(std::wstring path) {
    while (path.size() > 3 && (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

std::wstring DefaultLabel(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos || pos + 1 == path.size()) {
        return path;
    }
    return path.substr(pos + 1);
}

// Network share (\\server\share) or mapped network drive: disk/shell calls on these
// can block the UI for seconds when the server is offline (Explorer "Not responding").
bool IsSlowPath(const std::wstring& path) {
    if (path.size() >= 2 && (path[0] == L'\\' || path[0] == L'/') && (path[1] == L'\\' || path[1] == L'/')) {
        return true;
    }
    if (path.size() >= 2 && path[1] == L':') {
        wchar_t root[4] = {path[0], L':', L'\\', 0};
        return GetDriveTypeW(root) == DRIVE_REMOTE;
    }
    return false;
}

bool IsDirectoryPath(const std::wstring& path) {
    if (path.empty()) return false;
    if (IsSlowPath(path)) {
        // Guess without touching the network: no extension means folder
        size_t slash = path.find_last_of(L"\\/");
        size_t dot = path.find_last_of(L'.');
        return dot == std::wstring::npos || (slash != std::wstring::npos && dot < slash);
    }
    DWORD attr = GetFileAttributesW(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool IsExplorerFrame(HWND window) {
    wchar_t className[64]{};
    return window && GetClassNameW(window, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"CabinetWClass") == 0;
}

bool ContainsIgnoreCase(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty()) return true;
    auto it = std::search(
        haystack.begin(), haystack.end(),
        needle.begin(), needle.end(),
        [](wchar_t a, wchar_t b) { return towlower(a) == towlower(b); });
    return it != haystack.end();
}

HWND IslandWindow(const mux::UIElement& element) {
    try {
        if (auto root = element.XamlRoot()) {
            if (auto env = root.ContentIslandEnvironment()) {
                HWND window = GetAncestor(
                    reinterpret_cast<HWND>(static_cast<uintptr_t>(env.AppWindowId().Value)),
                    GA_ROOT);
                return IsExplorerFrame(window) ? window : nullptr;
            }
        }
    } catch (...) {}
    return nullptr;
}

HWND ExplorerWindowForElement(const mux::FrameworkElement& element) {
    if (element) {
        if (HWND w = IslandWindow(element)) return w;
    }
    HWND w = GetActiveWindow();
    w = w ? GetAncestor(w, GA_ROOT) : nullptr;
    if (IsExplorerFrame(w)) return w;
    if (!g_frames.empty() && IsExplorerFrame(g_frames.back())) {
        return g_frames.back();
    }
    return nullptr;
}

// ============================================================================
// Tree path helpers
// ============================================================================

NodePath ParentOf(const NodePath& p) {
    if (p.empty()) return {};
    return NodePath(p.begin(), p.end() - 1);
}

NodePath ChildOf(const NodePath& p, int i) {
    NodePath r = p;
    r.push_back(i);
    return r;
}

bool IsPrefixOf(const NodePath& prefix, const NodePath& p) {
    return prefix.size() <= p.size() && std::equal(prefix.begin(), prefix.end(), p.begin());
}

std::vector<BookmarkNode>* FolderChildren(std::vector<BookmarkNode>& root, const NodePath& folder) {
    std::vector<BookmarkNode>* cur = &root;
    for (int idx : folder) {
        if (idx < 0 || static_cast<size_t>(idx) >= cur->size()) return nullptr;
        BookmarkNode& n = (*cur)[idx];
        if (n.type != NodeType::FolderBox) return nullptr;
        cur = &n.children;
    }
    return cur;
}

const std::vector<BookmarkNode>* FolderChildren(const std::vector<BookmarkNode>& root, const NodePath& folder) {
    return FolderChildren(const_cast<std::vector<BookmarkNode>&>(root), folder);
}

BookmarkNode* NodeAt(std::vector<BookmarkNode>& root, const NodePath& path) {
    if (path.empty()) return nullptr;
    auto* kids = FolderChildren(root, ParentOf(path));
    if (!kids) return nullptr;
    int idx = path.back();
    if (idx < 0 || static_cast<size_t>(idx) >= kids->size()) return nullptr;
    return &(*kids)[idx];
}

const BookmarkNode* NodeAt(const std::vector<BookmarkNode>& root, const NodePath& path) {
    return NodeAt(const_cast<std::vector<BookmarkNode>&>(root), path);
}

// Removes empty paths, duplicates, and paths whose ancestor is also selected. Result is sorted.
std::vector<NodePath> NormalizeSelection(std::vector<NodePath> paths) {
    paths.erase(std::remove_if(paths.begin(), paths.end(), [](const NodePath& p) { return p.empty(); }),
                paths.end());
    std::sort(paths.begin(), paths.end());
    paths.erase(std::unique(paths.begin(), paths.end()), paths.end());
    std::vector<NodePath> out;
    for (const auto& p : paths) {
        bool covered = false;
        for (const auto& q : out) {
            if (IsPrefixOf(q, p)) {
                covered = true;
                break;
            }
        }
        if (!covered) out.push_back(p);
    }
    return out;
}

bool RemoveNodes(std::vector<BookmarkNode>& list, const std::vector<NodePath>& paths) {
    auto sel = NormalizeSelection(paths);
    bool changed = false;
    for (auto it = sel.rbegin(); it != sel.rend(); ++it) {
        auto* kids = FolderChildren(list, ParentOf(*it));
        int idx = it->back();
        if (!kids || idx < 0 || static_cast<size_t>(idx) >= kids->size()) continue;
        kids->erase(kids->begin() + idx);
        changed = true;
    }
    return changed;
}

void AdjustForRemoval(NodePath& folder, int& index, const NodePath& removed) {
    if (removed.empty()) return;
    size_t k = removed.size() - 1;
    if (folder.size() == k && std::equal(folder.begin(), folder.end(), removed.begin())) {
        if (removed[k] < index) --index;
    } else if (folder.size() > k && std::equal(removed.begin(), removed.begin() + k, folder.begin()) &&
               folder[k] > removed[k]) {
        --folder[k];
    }
}

void InsertNodes(std::vector<BookmarkNode>& list, const NodePath& folder, int index,
                 const std::vector<BookmarkNode>& nodes) {
    auto* kids = FolderChildren(list, folder);
    if (!kids) kids = &list;
    size_t pos = (index < 0) ? kids->size() : std::min<size_t>(static_cast<size_t>(index), kids->size());
    kids->insert(kids->begin() + pos, nodes.begin(), nodes.end());
}

// Moves nodes to destFolder at destIndex (-1 = end). Refuses to move a folder into itself.
bool MoveNodes(std::vector<BookmarkNode>& list, const std::vector<NodePath>& srcs,
               NodePath destFolder, int destIndex) {
    auto sel = NormalizeSelection(srcs);
    if (sel.empty() || !FolderChildren(list, destFolder)) return false;
    for (const auto& s : sel) {
        if (IsPrefixOf(s, destFolder)) return false;
    }
    std::vector<BookmarkNode> moving;
    for (const auto& s : sel) {
        const BookmarkNode* n = NodeAt(list, s);
        if (!n) return false;
        moving.push_back(*n);
    }
    if (destIndex < 0) destIndex = INT_MAX;
    for (auto it = sel.rbegin(); it != sel.rend(); ++it) {
        auto* kids = FolderChildren(list, ParentOf(*it));
        if (!kids) return false;
        kids->erase(kids->begin() + it->back());
        AdjustForRemoval(destFolder, destIndex, *it);
    }
    auto* dest = FolderChildren(list, destFolder);
    if (!dest) return false;
    size_t pos = std::min<size_t>(static_cast<size_t>(std::max(destIndex, 0)), dest->size());
    dest->insert(dest->begin() + pos, moving.begin(), moving.end());
    return true;
}

// Direct bookmark items inside the targets (Chrome "Open all")
std::vector<std::wstring> CollectOpenPaths(const std::vector<BookmarkNode>& list, const std::vector<NodePath>& targets) {
    std::vector<std::wstring> out;
    for (const auto& t : targets) {
        const std::vector<BookmarkNode>* kids = nullptr;
        if (t.empty()) {
            kids = &list;
        } else if (const BookmarkNode* n = NodeAt(list, t)) {
            if (n->type == NodeType::Item) {
                if (!n->path.empty()) out.push_back(n->path);
                continue;
            }
            kids = &n->children;
        }
        if (!kids) continue;
        for (const auto& c : *kids) {
            if (c.type == NodeType::Item && !c.path.empty()) out.push_back(c.path);
        }
    }
    return out;
}

std::wstring UniqueFolderName(const std::vector<BookmarkNode>& kids, const std::wstring& base) {
    std::wstring candidate = base;
    int counter = 1;
    while (std::any_of(kids.begin(), kids.end(), [&](const BookmarkNode& n) {
        return n.type == NodeType::FolderBox && n.name == candidate;
    })) {
        ++counter;
        candidate = base + L" (" + std::to_wstring(counter) + L")";
    }
    return candidate;
}

// ============================================================================
// JSON Serialization (v3 recursive, reads v2 too)
// ============================================================================

wjson::JsonObject NodeToJson(const BookmarkNode& node, int depth) {
    wjson::JsonObject obj;
    obj.SetNamedValue(L"type", wjson::JsonValue::CreateStringValue(
        node.type == NodeType::FolderBox ? L"box" : L"item"));
    obj.SetNamedValue(L"name", wjson::JsonValue::CreateStringValue(node.name));
    if (node.type == NodeType::Item) {
        obj.SetNamedValue(L"path", wjson::JsonValue::CreateStringValue(node.path));
    } else {
        wjson::JsonArray arr;
        if (depth < kMaxDepth) {
            for (const auto& c : node.children) arr.Append(NodeToJson(c, depth + 1));
        }
        obj.SetNamedValue(L"children", arr);
    }
    return obj;
}

std::wstring SerializeNodes(const std::vector<BookmarkNode>& nodes) {
    wjson::JsonObject root;
    root.SetNamedValue(L"format", wjson::JsonValue::CreateStringValue(L"explorer-chrome-bookmarks"));
    root.SetNamedValue(L"version", wjson::JsonValue::CreateNumberValue(3));
    wjson::JsonArray arr;
    for (const auto& node : nodes) arr.Append(NodeToJson(node, 0));
    root.SetNamedValue(L"nodes", arr);
    return std::wstring(root.Stringify().c_str());
}

void JsonToNodes(const wjson::JsonArray& arr, std::vector<BookmarkNode>& out, int depth) {
    if (depth > kMaxDepth) return;
    for (uint32_t i = 0; i < arr.Size(); ++i) {
        try {
            auto val = arr.GetAt(i);
            if (val.ValueType() != wjson::JsonValueType::Object) continue;
            auto obj = val.GetObject();
            BookmarkNode node;
            std::wstring typeStr(obj.GetNamedString(L"type", L"item").c_str());
            node.type = (typeStr == L"box") ? NodeType::FolderBox : NodeType::Item;
            node.name = std::wstring(obj.GetNamedString(L"name", L"").c_str());
            if (node.type == NodeType::Item) {
                node.path = NormalizePath(std::wstring(obj.GetNamedString(L"path", L"").c_str()));
                if (node.path.empty()) continue;
                if (node.name.empty()) node.name = DefaultLabel(node.path);
            } else {
                if (node.name.empty()) node.name = L"Folder";
                if (obj.HasKey(L"children")) {
                    auto cv = obj.GetNamedValue(L"children");
                    if (cv.ValueType() == wjson::JsonValueType::Array) {
                        JsonToNodes(cv.GetArray(), node.children, depth + 1);
                    }
                }
            }
            out.push_back(std::move(node));
        } catch (...) {}
    }
}

// Returns false when the text is not a valid bookmarks document.
bool TryDeserializeNodes(const std::wstring& jsonText, std::vector<BookmarkNode>& result) {
    result.clear();
    if (jsonText.empty()) return false;
    wjson::JsonObject root{nullptr};
    if (!wjson::JsonObject::TryParse(jsonText, root)) return false;
    try {
        if (!root.HasKey(L"nodes")) return false;
        auto v = root.GetNamedValue(L"nodes");
        if (v.ValueType() != wjson::JsonValueType::Array) return false;
        JsonToNodes(v.GetArray(), result, 0);
    } catch (...) {
        result.clear();
        return false;
    }
    return true;
}

std::vector<BookmarkNode> DeserializeNodes(const std::wstring& jsonText) {
    std::vector<BookmarkNode> result;
    TryDeserializeNodes(jsonText, result);
    return result;
}

// ---------------------------------------------------------------------------
// Storage
//
// Bookmarks are kept in a UTF-8 JSON file per Windows account under the mod's
// storage folder. Writes go to a temporary file first and then replace the
// real file in one step, so a crash during a write can't corrupt it. When the
// file exists but can't be read or parsed, saving is refused, so a broken read
// never overwrites the stored bookmarks with an empty list.
// ---------------------------------------------------------------------------

constexpr size_t kStorageMaxBytes = 16 * 1024 * 1024;
constexpr size_t kUndoLimit = 30;

enum class StoreRead { Ok, Missing, Error };

std::wstring CurrentUserSid() {
    std::wstring result;
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) return result;
    alignas(TOKEN_USER) BYTE buffer[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE] = {};
    DWORD size = 0;
    PWSTR sid = nullptr;
    if (GetTokenInformation(token, TokenUser, buffer, sizeof(buffer), &size) &&
        ConvertSidToStringSidW(reinterpret_cast<TOKEN_USER*>(buffer)->User.Sid, &sid)) {
        result = sid;
        LocalFree(sid);
    }
    CloseHandle(token);
    return result;
}

std::string WideToUtf8(const std::wstring& text) {
    if (text.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    if (len <= 0) return {};
    std::string out(static_cast<size_t>(len), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), out.data(), len, nullptr, nullptr);
    return out;
}

bool Utf8ToWide(const std::string& bytes, std::wstring& out) {
    out.clear();
    size_t start = (bytes.size() >= 3 && static_cast<unsigned char>(bytes[0]) == 0xEF &&
                    static_cast<unsigned char>(bytes[1]) == 0xBB && static_cast<unsigned char>(bytes[2]) == 0xBF)
                       ? 3 : 0;
    if (bytes.size() == start) return true;
    const char* data = bytes.data() + start;
    int count = static_cast<int>(bytes.size() - start);
    int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, data, count, nullptr, 0);
    if (len <= 0) return false;
    out.resize(static_cast<size_t>(len));
    return MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, data, count, out.data(), len) == len;
}

StoreRead ReadStoreFileLocked(std::wstring& text) {
    text.clear();
    if (g_storageFile.empty()) return StoreRead::Error;
    HANDLE file = CreateFileW(g_storageFile.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        return (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) ? StoreRead::Missing : StoreRead::Error;
    }
    StoreRead result = StoreRead::Error;
    LARGE_INTEGER size{};
    if (GetFileSizeEx(file, &size) && size.QuadPart > 0 && static_cast<unsigned long long>(size.QuadPart) <= kStorageMaxBytes) {
        std::string bytes(static_cast<size_t>(size.QuadPart), '\0');
        DWORD read = 0;
        if (ReadFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &read, nullptr) && read == bytes.size() &&
            Utf8ToWide(bytes, text) && !text.empty()) {
            result = StoreRead::Ok;
        }
    }
    CloseHandle(file);
    if (result != StoreRead::Ok) text.clear();
    return result;
}

bool WriteStoreFileLocked(const std::wstring& json) {
    if (g_storageFile.empty() || json.empty()) return false;
    std::string bytes = WideToUtf8(json);
    if (bytes.empty()) return false;
    // A per-process temporary name, since two Explorer processes may save at once.
    std::wstring temp = g_storageFile + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";
    HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        Wh_Log(L"Storage: can't create the temporary file (%u)", GetLastError());
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(file, bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr) &&
              written == bytes.size() && FlushFileBuffers(file);
    CloseHandle(file);
    if (ok) ok = MoveFileExW(temp.c_str(), g_storageFile.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != FALSE;
    if (!ok) {
        Wh_Log(L"Storage: can't write the bookmarks file (%u)", GetLastError());
        DeleteFileW(temp.c_str());
    }
    return ok;
}

// Reads the stored JSON and checks that it parses. A missing file is a valid,
// empty state; anything else that isn't a valid document is an error.
bool ReadValidStoreLocked(std::wstring& text, std::vector<BookmarkNode>* nodes) {
    std::vector<BookmarkNode> parsed;
    switch (ReadStoreFileLocked(text)) {
        case StoreRead::Missing:
            text.clear();
            if (nodes) nodes->clear();
            return true;
        case StoreRead::Ok:
            if (TryDeserializeNodes(text, parsed)) {
                if (nodes) *nodes = std::move(parsed);
                return true;
            }
            break;
        case StoreRead::Error:
            break;
    }
    Wh_Log(L"Storage: the bookmarks file can't be read or parsed; saving is disabled to protect it");
    text.clear();
    if (nodes) nodes->clear();
    return false;
}

// ---------------------------------------------------------------------------
// In-memory copy and change detection
//
// The parsed bookmarks are kept in memory and reused while the file's size and
// last-write time are unchanged, so frequent callers (drag moves, dropdowns,
// menus) don't read and parse the file each time. A change made by another
// window thread or another explorer.exe process changes the stamp, and the file
// is read again.
//
// Each thread remembers the stamp of the file its last load came from. An edit
// is load -> modify -> save, so SaveBookmarks refuses to write when the file has
// changed since that load (someone else saved in between), or when that load
// failed (an edit built on an empty list must never replace the file).
// ---------------------------------------------------------------------------

struct StoreStamp {
    bool missing = false;
    ULONGLONG size = 0;
    FILETIME lastWrite{};
};

bool SameStamp(const StoreStamp& a, const StoreStamp& b) {
    if (a.missing || b.missing) return a.missing == b.missing;
    return a.size == b.size && CompareFileTime(&a.lastWrite, &b.lastWrite) == 0;
}

bool GetStoreStampLocked(StoreStamp& stamp) {
    stamp = StoreStamp{};
    if (g_storageFile.empty()) return false;
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(g_storageFile.c_str(), GetFileExInfoStandard, &data)) {
        DWORD error = GetLastError();
        stamp.missing = (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND);
        return stamp.missing;
    }
    stamp.size = (static_cast<ULONGLONG>(data.nFileSizeHigh) << 32) | data.nFileSizeLow;
    stamp.lastWrite = data.ftLastWriteTime;
    return true;
}

struct StoreCache {
    bool valid = false;
    StoreStamp stamp;
    std::wstring text;
    std::vector<BookmarkNode> nodes;
};
StoreCache g_storeCache;  // guarded by g_dataMutex

struct LoadBase {
    bool set = false;    // this thread has loaded at least once
    bool valid = false;  // the last load succeeded
    StoreStamp stamp;
};
thread_local LoadBase t_loadBase;

// Reads the stored bookmarks, from memory when the file is unchanged. Returns
// false when the file can't be read; `stamp` receives the file's stamp.
bool ReadStoreCachedLocked(std::wstring* text, std::vector<BookmarkNode>* nodes, StoreStamp& stamp) {
    const bool stamped = GetStoreStampLocked(stamp);
    if (stamped && g_storeCache.valid && SameStamp(g_storeCache.stamp, stamp)) {
        if (text) *text = g_storeCache.text;
        if (nodes) *nodes = g_storeCache.nodes;
        return true;
    }
    std::wstring raw;
    std::vector<BookmarkNode> parsed;
    const bool ok = stamped && ReadValidStoreLocked(raw, &parsed);
    g_storeCache.valid = false;
    if (ok) {
        // The stamp was taken before reading: if the file changed in between,
        // the next call sees a different stamp and reads it again.
        g_storeCache.valid = true;
        g_storeCache.stamp = stamp;
        g_storeCache.text = raw;
        g_storeCache.nodes = parsed;
    }
    if (text) *text = std::move(raw);
    if (nodes) *nodes = std::move(parsed);
    return ok;
}

// After this process wrote the file: the writing thread's base is the new file.
void AfterOwnWriteLocked() {
    g_storeCache.valid = false;
    StoreStamp stamp;
    t_loadBase.set = true;
    t_loadBase.valid = GetStoreStampLocked(stamp);
    t_loadBase.stamp = stamp;
}

void InitStorage() {
    std::lock_guard lock(g_dataMutex);
    g_storageFile.clear();
    g_undoStack.clear();
    g_storeCache = StoreCache{};
    std::wstring dir(32768, L'\0');
    size_t chars = Wh_GetModStoragePath(dir.data(), dir.size());
    if (chars == 0 || chars >= dir.size()) {
        Wh_Log(L"Storage: Wh_GetModStoragePath failed");
        return;
    }
    dir.resize(chars);
    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) dir.pop_back();
    int created = SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    if (created != ERROR_SUCCESS && created != ERROR_ALREADY_EXISTS && created != ERROR_FILE_EXISTS) {
        Wh_Log(L"Storage: can't create the storage folder (%d)", created);
    }
    std::wstring sid = CurrentUserSid();
    if (sid.empty()) Wh_Log(L"Storage: can't read the account SID; using a shared file");
    g_storageFile = dir + L"\\bookmarks" + (sid.empty() ? std::wstring() : L"-" + sid) + L".json";
}

std::vector<BookmarkNode> LoadBookmarks() {
    std::vector<BookmarkNode> nodes;
    StoreStamp stamp;
    std::lock_guard lock(g_dataMutex);
    const bool ok = ReadStoreCachedLocked(nullptr, &nodes, stamp);
    t_loadBase.set = true;
    t_loadBase.valid = ok;
    t_loadBase.stamp = stamp;
    return nodes;
}

// replaceUnreadable: used only by Import, where the user explicitly picks a
// complete replacement, so it may overwrite a file that can't be read.
bool SaveBookmarks(const std::vector<BookmarkNode>& nodes, bool replaceUnreadable) {
    std::wstring json = SerializeNodes(nodes);
    bool ok = false;
    bool refreshOnly = false;
    {
        std::lock_guard lock(g_dataMutex);
        std::wstring prev;
        StoreStamp current;
        const bool prevValid = ReadStoreCachedLocked(&prev, nullptr, current);
        if (!replaceUnreadable) {
            if (!prevValid) return false;
            if (t_loadBase.set && (!t_loadBase.valid || !SameStamp(t_loadBase.stamp, current))) {
                Wh_Log(L"Storage: the bookmarks changed since they were loaded; this edit is not saved");
                refreshOnly = true;
            }
        }
        if (!refreshOnly) {
            ok = WriteStoreFileLocked(json);
            if (ok) {
                if (prevValid && prev != json) {
                    g_undoStack.push_back(prev.empty() ? SerializeNodes({}) : prev);
                    if (g_undoStack.size() > kUndoLimit) g_undoStack.erase(g_undoStack.begin());
                }
                AfterOwnWriteLocked();
            }
        }
    }
    // A refused edit still refreshes the bars, so they show the current bookmarks
    if (ok || refreshOnly) NotifyExplorerBars();
    return ok;
}

bool SaveBookmarks(const std::vector<BookmarkNode>& nodes) {
    return SaveBookmarks(nodes, false);
}

bool UndoLastChange() {
    bool ok = false;
    {
        std::lock_guard lock(g_dataMutex);
        if (g_undoStack.empty()) return false;
        std::wstring prev = std::move(g_undoStack.back());
        g_undoStack.pop_back();
        ok = WriteStoreFileLocked(prev);
        if (ok) AfterOwnWriteLocked();
    }
    if (ok) NotifyExplorerBars();
    return ok;
}

// ============================================================================
// Shell Navigation & "Open in New Tab"
// ============================================================================

struct ComScope {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    ~ComScope() { if (SUCCEEDED(hr)) CoUninitialize(); }
};

winrt::com_ptr<IShellBrowser> ActiveShellBrowser(HWND explorerWindow) {
    winrt::com_ptr<IShellWindows> shellWindows;
    if (FAILED(CoCreateInstance(kShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(shellWindows.put()))))
        return nullptr;
    HWND activeTab = FindWindowExW(explorerWindow, nullptr, L"ShellTabWindowClass", nullptr);
    long count = 0;
    if (FAILED(shellWindows->get_Count(&count))) return nullptr;
    for (long i = 0; i < count; ++i) {
        VARIANT idx{}; idx.vt = VT_I4; idx.lVal = i;
        winrt::com_ptr<IDispatch> disp;
        if (FAILED(shellWindows->Item(idx, disp.put())) || !disp) continue;
        auto wb = disp.try_as<IWebBrowser2>();
        SHANDLE_PTR hwndVal = 0;
        if (!wb || FAILED(wb->get_HWND(&hwndVal)) || reinterpret_cast<HWND>(hwndVal) != explorerWindow)
            continue;
        auto sp = disp.try_as<IServiceProvider>();
        if (!sp) continue;
        winrt::com_ptr<IShellBrowser> browser;
        if (FAILED(sp->QueryService(kTopLevelBrowser, IID_PPV_ARGS(browser.put())))) continue;
        HWND tabWnd = nullptr;
        if (FAILED(browser->GetWindow(&tabWnd)) || !tabWnd) continue;
        if ((activeTab && tabWnd != activeTab) || (!activeTab && !IsWindowVisible(tabWnd))) continue;
        return browser;
    }
    return nullptr;
}

std::wstring CurrentFolder(HWND explorerWindow) {
    ComScope com;
    auto browser = ActiveShellBrowser(explorerWindow);
    if (!browser) return {};
    winrt::com_ptr<IShellView> view;
    if (FAILED(browser->QueryActiveShellView(view.put())) || !view) return {};
    auto fv = view.try_as<IFolderView>();
    winrt::com_ptr<IPersistFolder2> pf;
    if (!fv || FAILED(fv->GetFolder(IID_PPV_ARGS(pf.put())))) return {};
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(pf->GetCurFolder(&pidl)) || !pidl) return {};
    std::vector<wchar_t> path(32768);
    bool ok = SHGetPathFromIDListEx(pidl, path.data(), path.size(), GPFIDL_DEFAULT);
    CoTaskMemFree(pidl);
    return ok ? NormalizePath(path.data()) : std::wstring{};
}

std::vector<std::wstring> SelectedItems(HWND explorerWindow) {
    std::vector<std::wstring> result;
    ComScope com;
    auto browser = ActiveShellBrowser(explorerWindow);
    winrt::com_ptr<IShellView> view;
    if (!browser || FAILED(browser->QueryActiveShellView(view.put())) || !view) return result;
    auto fv = view.try_as<IFolderView>();
    winrt::com_ptr<IShellItemArray> items;
    DWORD count = 0;
    if (!fv || FAILED(fv->Items(SVGIO_SELECTION, IID_PPV_ARGS(items.put()))) || !items || FAILED(items->GetCount(&count)))
        return result;
    for (DWORD i = 0; i < std::min<DWORD>(count, 25); ++i) {
        winrt::com_ptr<IShellItem> item;
        if (FAILED(items->GetItemAt(i, item.put()))) continue;
        PWSTR path = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path)) && path) {
            result.push_back(NormalizePath(path));
        }
        CoTaskMemFree(path);
    }
    return result;
}

std::wstring ResolveFolderShortcut(const std::wstring& path) {
    if (IsDirectoryPath(path)) return path;
    if (path.size() > 4 && _wcsicmp(path.c_str() + path.size() - 4, L".lnk") == 0) {
        ComScope com;
        winrt::com_ptr<IShellLinkW> link;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(link.put())))) {
            if (auto pf = link.try_as<IPersistFile>()) {
                if (SUCCEEDED(pf->Load(path.c_str(), STGM_READ))) {
                    wchar_t target[MAX_PATH]{};
                    if (SUCCEEDED(link->GetPath(target, MAX_PATH, nullptr, 0)) && target[0] != L'\0') {
                        std::wstring resolved = NormalizePath(target);
                        if (IsDirectoryPath(resolved)) return resolved;
                    }
                }
            }
        }
    }
    return {};
}

bool OpenFolderInExplorerNewTab(HWND explorerWindow, const std::wstring& folderPath) {
    ComScope com;
    auto browser = ActiveShellBrowser(explorerWindow);
    winrt::com_ptr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(folderPath.c_str(), nullptr, IID_PPV_ARGS(item.put()))) && item) {
        winrt::com_ptr<IContextMenu> cm;
        if (SUCCEEDED(item->BindToHandler(nullptr, BHID_SFUIObject, IID_PPV_ARGS(cm.put()))) && cm) {
            auto withSite = cm.try_as<IObjectWithSite>();
            if (browser && withSite) withSite->SetSite(browser.get());
            HMENU hMenu = CreatePopupMenu();
            HRESULT hr = E_FAIL;
            if (hMenu) {
                if (SUCCEEDED(cm->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {
                    CMINVOKECOMMANDINFOEX ici{};
                    ici.cbSize = sizeof(ici);
                    // NOTE: no CMIC_MASK_ASYNCOK -> command finishes before we release the menu (crash fix)
                    ici.fMask = CMIC_MASK_UNICODE | CMIC_MASK_FLAG_NO_UI;
                    ici.hwnd = explorerWindow;
                    ici.lpVerb = "opennewtab";
                    ici.lpVerbW = L"opennewtab";
                    ici.nShow = SW_SHOWNORMAL;
                    hr = cm->InvokeCommand(reinterpret_cast<LPCMINVOKECOMMANDINFO>(&ici));
                }
                DestroyMenu(hMenu);
            }
            if (withSite) withSite->SetSite(nullptr);
            if (SUCCEEDED(hr)) return true;
        }
    }

    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    sei.hwnd = explorerWindow;
    sei.lpVerb = L"opennewtab";
    sei.lpFile = folderPath.c_str();
    sei.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&sei) != FALSE;
}

enum class OpenDisposition {
    CurrentTab,
    NewTab,
    NewWindow
};

void OpenBookmarkTarget(HWND explorerWindow, const std::wstring& path, OpenDisposition disp) {
    if (path.empty()) return;

    std::wstring folderTarget = ResolveFolderShortcut(path);
    if (!folderTarget.empty()) {
        if (disp == OpenDisposition::NewWindow) {
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
            sei.hwnd = explorerWindow;
            sei.lpVerb = L"opennewwindow";
            sei.lpFile = folderTarget.c_str();
            sei.nShow = SW_SHOWNORMAL;
            if (!ShellExecuteExW(&sei)) {
                ShellExecuteW(nullptr, L"explore", folderTarget.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }
            return;
        }
        if (disp == OpenDisposition::NewTab && explorerWindow) {
            OpenFolderInExplorerNewTab(explorerWindow, folderTarget);
            return;
        }
        ComScope com;
        if (explorerWindow) {
            if (auto browser = ActiveShellBrowser(explorerWindow)) {
                PIDLIST_ABSOLUTE pidl = nullptr;
                if (SUCCEEDED(SHParseDisplayName(folderTarget.c_str(), nullptr, &pidl, 0, nullptr)) && pidl) {
                    browser->BrowseObject(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
                    CoTaskMemFree(pidl);
                    return;
                }
            }
        }
    }
    ShellExecuteW(explorerWindow, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

// ============================================================================
// Deferred work on the Explorer frame thread (crash fix)
// Opening tabs / showing dialogs directly inside XAML event handlers re-enters
// XAML and can crash Explorer. Everything is posted to the frame window instead,
// and multiple tabs are opened one at a time.
// ============================================================================

UINT RefreshBarsMessage() {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_ExplorerChromeBookmarksBar_Refresh");
    return msg;
}

// Sent on unload: cleans up each Explorer window on its own thread
UINT UninitBarsMessage() {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_ExplorerChromeBookmarksBar_Uninit");
    return msg;
}

// Message hooks of all threads (all removed on unload)
std::mutex g_hooksMutex;
std::vector<HHOOK> g_allHooks;

// Work queued on a thread's DispatcherQueue holds one of these tokens, whether
// it runs or the queue discards it. Wh_ModUninit waits until none is left.
std::atomic<unsigned> g_pendingScans = 0;

struct PendingScanToken {
    PendingScanToken() { ++g_pendingScans; }
    ~PendingScanToken() { --g_pendingScans; }
    PendingScanToken(const PendingScanToken&) = delete;
    PendingScanToken& operator=(const PendingScanToken&) = delete;
};

// ---------------------------------------------------------------------------
// XAML event handlers
//
// Every handler the mod adds to a XAML object is recorded here (per UI thread)
// and removed before the mod is unloaded. A XAML object that still held one
// would call into the unloaded DLL when it raises the event, or when it is
// destroyed later and releases the handler. Explorer then fails fast
// (0xC0000409, a Control Flow Guard check on a call into unloaded code).
// ---------------------------------------------------------------------------

struct HandlerRevoker {
    winrt::weak_ref<wf::IInspectable> source;
    std::function<void(const wf::IInspectable&)> revoke;
};
[[clang::no_destroy]] thread_local std::vector<HandlerRevoker> g_handlerRevokers;
thread_local size_t g_revokersAdded = 0;

// Objects that are gone have released their handlers already
void PruneHandlerRevokers() {
    std::erase_if(g_handlerRevokers, [](const HandlerRevoker& r) { return !r.source.get(); });
}

template <typename T, typename Revoke>
void TrackRevoker(const T& element, Revoke revoke) {
    try {
        HandlerRevoker r;
        r.source = winrt::make_weak(element.template as<wf::IInspectable>());
        r.revoke = [revoke](const wf::IInspectable& source) {
            if (auto e = source.try_as<T>()) revoke(e);
        };
        g_handlerRevokers.push_back(std::move(r));
        if (++g_revokersAdded >= 256) {
            g_revokersAdded = 0;
            PruneHandlerRevokers();
        }
    } catch (...) {
        Wh_Log(L"TrackRevoker: exception");
    }
}

// Records an event handler (token) so it can be removed later; returns the token.
template <typename T, typename Revoke>
winrt::event_token TrackEvent(const T& element, winrt::event_token token, Revoke revoke) {
    TrackRevoker(element, [token, revoke](const T& e) { revoke(e, token); });
    return token;
}

// AddHandler with removal recorded (used for handledEventsToo handlers).
template <typename T>
void TrackAddHandler(const T& element, const mux::RoutedEvent& routedEvent, const wf::IInspectable& handler,
                     bool handledEventsToo) {
    element.AddHandler(routedEvent, handler, handledEventsToo);
    TrackRevoker(element, [routedEvent, handler](const T& e) { e.RemoveHandler(routedEvent, handler); });
}

// Removes every recorded handler of this thread's XAML objects.
void RevokeThreadHandlers() {
    auto list = std::move(g_handlerRevokers);
    g_handlerRevokers.clear();
    g_revokersAdded = 0;
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        try {
            if (auto source = it->source.get()) it->revoke(source);
        } catch (...) {}
    }
}

// ---------------------------------------------------------------------------
// Modal operations
//
// Some mod functions run a nested message loop on an Explorer UI thread: the
// edit dialog, message boxes, popup menus and file dialogs. If the mod were
// unloaded while one of them is still on a thread's stack, Explorer would
// crash when it returns. Each one is counted; Wh_ModUninit closes them on
// their own thread and waits until the count reaches zero. Once unloading has
// begun, no new one starts.
// ---------------------------------------------------------------------------

std::mutex g_operationMutex;
std::condition_variable g_operationFinished;
unsigned g_activeOperations = 0;
thread_local IFileDialog* g_threadFileDialog = nullptr;
thread_local HWND g_threadEditDialog = nullptr;
thread_local unsigned g_threadMenuDepth = 0;
thread_local unsigned g_threadMessageBoxes = 0;

constexpr wchar_t kMsgBoxTitle[] = L"Bookmarks";
constexpr wchar_t kImportMsgBoxTitle[] = L"Import bookmarks";

struct ModalScope {
    bool active = false;
    ModalScope() {
        std::lock_guard lock(g_operationMutex);
        if (!g_unloading) {
            ++g_activeOperations;
            active = true;
        }
    }
    ~ModalScope() {
        if (!active) return;
        {
            std::lock_guard lock(g_operationMutex);
            --g_activeOperations;
        }
        g_operationFinished.notify_all();
    }
    ModalScope(const ModalScope&) = delete;
    ModalScope& operator=(const ModalScope&) = delete;
};

int ModMessageBox(HWND owner, const wchar_t* text, const wchar_t* title, UINT flags) {
    ModalScope scope;
    if (!scope.active) return IDCANCEL;
    ++g_threadMessageBoxes;
    int result = MessageBoxW(owner, text, title, flags);
    --g_threadMessageBoxes;
    return result;
}

int ModTrackPopupMenu(HMENU menu, UINT flags, int x, int y, HWND owner) {
    ModalScope scope;
    if (!scope.active) return 0;
    ++g_threadMenuDepth;
    int result = TrackPopupMenu(menu, flags, x, y, 0, owner, nullptr);
    --g_threadMenuDepth;
    return result;
}

// Runs on each Explorer UI thread during unload: closes whatever modal UI of
// this mod is open on the thread, so its loop returns.
void CloseThreadModals() {
    if (g_threadMenuDepth) EndMenu();
    if (g_threadFileDialog) g_threadFileDialog->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
    if (g_threadMessageBoxes) {
        EnumThreadWindows(GetCurrentThreadId(), [](HWND w, LPARAM) -> BOOL {
            wchar_t cls[16]{}, title[64]{};
            if (GetClassNameW(w, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32770") == 0 &&
                GetWindowTextW(w, title, ARRAYSIZE(title)) &&
                (wcscmp(title, kMsgBoxTitle) == 0 || wcscmp(title, kImportMsgBoxTitle) == 0)) {
                PostMessageW(w, WM_CLOSE, 0, 0);
            }
            return TRUE;
        }, 0);
    }
    if (g_threadEditDialog && IsWindow(g_threadEditDialog)) {
        DestroyWindow(g_threadEditDialog);
        // Wake the dialog's GetMessageW loop so it notices the window is gone
        PostThreadMessageW(GetCurrentThreadId(), WM_NULL, 0, 0);
    }
}

// Runs a callback on the UI thread that owns a window, through a short-lived
// WH_CALLWNDPROC hook. Works for windows that this mod never subclassed.
UINT ThreadCallMessage() {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_ExplorerChromeBookmarksBar_ThreadCall");
    return msg;
}

LRESULT CALLBACK ThreadCallHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* cwp = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (cwp && cwp->message == ThreadCallMessage() && cwp->lParam) {
            try {
                reinterpret_cast<void (*)()>(cwp->lParam)();
            } catch (...) {
                Wh_Log(L"ThreadCallHook: exception");
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void RunOnWindowThread(HWND window, void (*callback)()) {
    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) return;
    if (threadId == GetCurrentThreadId()) {
        callback();
        return;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, ThreadCallHook, nullptr, threadId);
    if (!hook) return;
    SendMessageW(window, ThreadCallMessage(), 0, reinterpret_cast<LPARAM>(callback));
    UnhookWindowsHookEx(hook);
}

// Runs a callback once on the thread of every File Explorer window (and of
// every Bookmark Manager window) in this process.
void ForOwnUiThreads(void (*callback)()) {
    std::vector<HWND> windows;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid != GetCurrentProcessId()) return TRUE;
        wchar_t cls[64]{};
        if (GetClassNameW(w, cls, ARRAYSIZE(cls)) &&
            (wcscmp(cls, L"CabinetWClass") == 0 || wcscmp(cls, kMgrClass) == 0)) {
            reinterpret_cast<std::vector<HWND>*>(lp)->push_back(w);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    std::vector<DWORD> done;
    for (HWND w : windows) {
        DWORD tid = GetWindowThreadProcessId(w, nullptr);
        if (!tid || std::find(done.begin(), done.end(), tid) != done.end()) continue;
        done.push_back(tid);
        RunOnWindowThread(w, callback);
    }
}

// Pending reads of dropped items. A completion that ran after unloading would
// call into the unloaded DLL, so Wh_ModUninit cancels them and waits.
std::mutex g_dropReadsMutex;
[[clang::no_destroy]] std::optional<std::vector<wf::IAsyncInfo>> g_dropReads{std::in_place};
std::atomic<unsigned> g_pendingDrops = 0;

// Unregisters a pending drop read when its completion handler finishes.
struct DropReadGuard {
    wf::IAsyncInfo info{nullptr};
    explicit DropReadGuard(wf::IAsyncInfo i) : info(std::move(i)) {}
    DropReadGuard(const DropReadGuard&) = delete;
    DropReadGuard& operator=(const DropReadGuard&) = delete;
    ~DropReadGuard() {
        {
            std::lock_guard lock(g_dropReadsMutex);
            if (g_dropReads) std::erase(*g_dropReads, info);
        }
        --g_pendingDrops;
    }
};

template <typename Handler>
void ReadDroppedItems(const mux::DragEventArgs& args, Handler handler) {
    if (g_unloading) return;
    auto op = args.DataView().GetStorageItemsAsync();
    wf::IAsyncInfo info = op;
    ++g_pendingDrops;
    {
        std::lock_guard lock(g_dropReadsMutex);
        if (g_dropReads) g_dropReads->push_back(info);
    }
    op.Completed([handler = std::move(handler), info](
                     wf::IAsyncOperation<wf::Collections::IVectorView<winrt::Windows::Storage::IStorageItem>> const& asyncInfo,
                     wf::AsyncStatus status) {
        DropReadGuard done{info};
        if (status != wf::AsyncStatus::Completed || g_unloading) return;
        try {
            handler(asyncInfo.GetResults());
        } catch (...) {
            Wh_Log(L"Dropped items: exception");
        }
    });
}

// Reads the dropped items' paths in the background, then adds them as bookmarks
// on the UI thread (where the duplicate question can be shown).
void ReadDroppedPaths(const mux::DragEventArgs& args, HWND frame, const NodePath& folder) {
    auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!queue) return;
    ReadDroppedItems(args, [queue, frame, folder](auto const& items) {
        std::vector<std::wstring> paths;
        for (const auto& item : items) {
            std::wstring p(item.Path().c_str());
            if (!p.empty()) paths.push_back(std::move(p));
        }
        if (paths.empty() || g_unloading) return;
        queue.TryEnqueue([token = std::make_shared<PendingScanToken>(), frame, folder, paths]() {
            if (g_unloading) return;
            try {
                AddPathsAsBookmarks(frame, folder, paths);
            } catch (...) {
                Wh_Log(L"Adding dropped items: exception");
            }
        });
    });
}

UINT FrameTaskMessage() {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_ExplorerChromeBookmarksBar_Task");
    return msg;
}

UINT OpenQueueMessage() {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_ExplorerChromeBookmarksBar_OpenQueue");
    return msg;
}

// With "Launch folder windows in a separate process", the mod runs in more than
// one explorer.exe. Lookups that pick a window to act on must stay in this process.
bool IsOwnProcessWindow(HWND w) {
    DWORD pid = 0;
    return w && GetWindowThreadProcessId(w, &pid) && pid == GetCurrentProcessId();
}

HWND FindOwnProcessWindow(const wchar_t* className) {
    for (HWND w = FindWindowExW(nullptr, nullptr, className, nullptr); w;
         w = FindWindowExW(nullptr, w, className, nullptr)) {
        if (IsOwnProcessWindow(w)) return w;
    }
    return nullptr;
}

bool IsTrackedFrame(HWND w) {
    return w && IsWindow(w) && GetPropW(w, kFrameProperty) != nullptr && IsOwnProcessWindow(w);
}

HWND FindAnyTrackedFrame() {
    HWND found = nullptr;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        if (GetPropW(w, kFrameProperty) && IsWindowVisible(w) && IsOwnProcessWindow(w)) {
            *reinterpret_cast<HWND*>(lp) = w;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&found));
    return found;
}

void PostToFrame(HWND frame, std::function<void()> fn) {
    if (frame && IsTrackedFrame(frame) &&
        GetWindowThreadProcessId(frame, nullptr) == GetCurrentThreadId()) {
        g_frameTasks.push_back(std::move(fn));
        PostMessageW(frame, FrameTaskMessage(), 0, 0);
        return;
    }
    fn();
}

void RunFrameTasks() {
    std::deque<std::function<void()>> tasks;
    tasks.swap(g_frameTasks);
    for (auto& t : tasks) {
        try {
            if (t) t();
        } catch (...) {}
    }
}

struct OpenRequest {
    std::wstring path;
    OpenDisposition disp = OpenDisposition::CurrentTab;
    int confirmCount = 0;
};

std::mutex g_openMutex;
std::map<HWND, std::deque<OpenRequest>> g_openQueues;

void QueueOpenTargets(HWND frame, const std::vector<std::wstring>& paths, OpenDisposition disp) {
    if (paths.empty() || g_unloading) return;
    if (!IsTrackedFrame(frame)) frame = FindAnyTrackedFrame();
    if (!frame) {
        for (const auto& p : paths) {
            ShellExecuteW(nullptr, L"open", p.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
        return;
    }
    {
        std::lock_guard lock(g_openMutex);
        auto& q = g_openQueues[frame];
        if (paths.size() > kOpenConfirmThreshold) {
            OpenRequest c;
            c.disp = disp;
            c.confirmCount = static_cast<int>(paths.size());
            q.push_back(c);
        }
        for (const auto& p : paths) {
            OpenRequest r;
            r.path = p;
            r.disp = disp;
            q.push_back(r);
        }
    }
    PostMessageW(frame, OpenQueueMessage(), 0, 0);
}

void ProcessOpenQueue(HWND frame) {
    ModalScope scope;  // opening can show shell error dialogs
    if (!scope.active) return;
    OpenRequest req;
    bool more = false;
    {
        std::lock_guard lock(g_openMutex);
        auto it = g_openQueues.find(frame);
        if (it == g_openQueues.end() || it->second.empty()) return;
        req = it->second.front();
        it->second.pop_front();
        more = !it->second.empty();
        if (!more) g_openQueues.erase(it);
    }
    if (req.confirmCount > 0) {
        std::wstring msg = L"You're about to open " + std::to_wstring(req.confirmCount) +
                           L" items. Are you sure?";
        // OK/Cancel (not Yes/No) so the box can be closed with WM_CLOSE on unload
        if (ModMessageBox(frame, msg.c_str(), kMsgBoxTitle, MB_OKCANCEL | MB_ICONQUESTION) != IDOK) {
            std::lock_guard lock(g_openMutex);
            g_openQueues.erase(frame);
            return;
        }
    } else {
        try {
            OpenBookmarkTarget(frame, req.path, req.disp);
        } catch (...) {}
    }
    if (more) SetTimer(frame, kOpenTimerId, req.confirmCount > 0 ? 10 : 350, nullptr);
}

// ============================================================================
// Icon extraction
// ============================================================================

muxmi::WriteableBitmap ConvertWicSourceToWriteableBitmap(IWICImagingFactory* factory, IWICBitmapSource* source) {
    if (!factory || !source) return nullptr;
    UINT w = 0, h = 0;
    if (FAILED(source->GetSize(&w, &h)) || w == 0 || h == 0 || w > 256 || h > 256) return nullptr;

    winrt::com_ptr<IWICFormatConverter> conv;
    if (FAILED(factory->CreateFormatConverter(conv.put()))) return nullptr;
    if (FAILED(conv->Initialize(source, GUID_WICPixelFormat32bppBGRA,
                                WICBitmapDitherTypeNone, nullptr, 0.0,
                                WICBitmapPaletteTypeCustom))) {
        return nullptr;
    }

    std::vector<BYTE> pixels(w * h * 4);
    if (FAILED(conv->CopyPixels(nullptr, w * 4, static_cast<UINT>(pixels.size()), pixels.data()))) {
        return nullptr;
    }

    muxmi::WriteableBitmap bmp(static_cast<int>(w), static_cast<int>(h));
    auto buf = bmp.PixelBuffer();
    auto access = buf.as<::Windows::Storage::Streams::IBufferByteAccess>();
    BYTE* dst = nullptr;
    if (SUCCEEDED(access->Buffer(&dst)) && dst) {
        std::memcpy(dst, pixels.data(), pixels.size());
        bmp.Invalidate();
        return bmp;
    }
    return nullptr;
}

// Windows folder icon re-colored blue, so bookmark folders look different from real folders.
// Output: premultiplied BGRA, size x size.
bool GetTintedFolderPixels(int size, std::vector<BYTE>& px) {
    ComScope com;
    winrt::com_ptr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.put()))))
        return false;
    SHSTOCKICONINFO sii{};
    sii.cbSize = sizeof(sii);
    UINT flags = SHGSI_ICON | (size <= 16 ? SHGSI_SMALLICON : SHGSI_LARGEICON);
    if (FAILED(SHGetStockIconInfo(SIID_FOLDER, flags, &sii)) || !sii.hIcon) return false;
    winrt::com_ptr<IWICBitmap> wicBmp;
    HRESULT hr = factory->CreateBitmapFromHICON(sii.hIcon, wicBmp.put());
    DestroyIcon(sii.hIcon);
    if (FAILED(hr) || !wicBmp) return false;
    winrt::com_ptr<IWICBitmapScaler> scaler;
    if (FAILED(factory->CreateBitmapScaler(scaler.put())) ||
        FAILED(scaler->Initialize(wicBmp.get(), size, size, WICBitmapInterpolationModeFant)))
        return false;
    winrt::com_ptr<IWICFormatConverter> conv;
    if (FAILED(factory->CreateFormatConverter(conv.put())) ||
        FAILED(conv->Initialize(scaler.get(), GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, nullptr, 0.0,
                                WICBitmapPaletteTypeCustom)))
        return false;
    px.assign(static_cast<size_t>(size) * size * 4, 0);
    if (FAILED(conv->CopyPixels(nullptr, size * 4, static_cast<UINT>(px.size()), px.data()))) return false;
    for (size_t i = 0; i + 3 < px.size(); i += 4) {
        double b = px[i], g = px[i + 1], r = px[i + 2], a = px[i + 3] / 255.0;
        double lum = (0.299 * r + 0.587 * g + 0.114 * b) / 255.0;
        double f = std::min(1.15, 0.55 + 0.6 * lum);  // keep the icon's shading
        auto tint = [&](BYTE t) { return std::min(255.0, t * f) * a; };
        px[i] = static_cast<BYTE>(tint(kFolderTintB));
        px[i + 1] = static_cast<BYTE>(tint(kFolderTintG));
        px[i + 2] = static_cast<BYTE>(tint(kFolderTintR));
    }
    return true;
}

HBITMAP CreateBitmapFromPixels(const std::vector<BYTE>& px, int size) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP h = CreateDIBSection(nullptr, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (h && bits) std::memcpy(bits, px.data(), px.size());
    return h;
}

muxmi::WriteableBitmap PixelsToWriteableBitmap(const std::vector<BYTE>& px, int w, int h) {
    muxmi::WriteableBitmap bmp(w, h);
    auto access = bmp.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
    BYTE* dst = nullptr;
    if (SUCCEEDED(access->Buffer(&dst)) && dst) {
        std::memcpy(dst, px.data(), std::min<size_t>(px.size(), static_cast<size_t>(w) * h * 4));
        bmp.Invalidate();
        return bmp;
    }
    return nullptr;
}

muxmi::WriteableBitmap ExtractShellBitmap(const std::wstring& path, bool isFolderBox) {
    try {
        if (isFolderBox) {
            std::vector<BYTE> px;
            if (GetTintedFolderPixels(32, px)) {
                if (auto b = PixelsToWriteableBitmap(px, 32, 32)) return b;
            }
        }
        ComScope com;
        winrt::com_ptr<IWICImagingFactory> factory;
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(factory.put())))) {
            return nullptr;
        }

        if (!isFolderBox && !path.empty() && !IsSlowPath(path)) {
            winrt::com_ptr<IShellItemImageFactory> imgFactory;
            if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(imgFactory.put()))) && imgFactory) {
                HBITMAP hbm = nullptr;
                if (SUCCEEDED(imgFactory->GetImage(SIZE{20, 20}, SIIGBF_ICONONLY, &hbm)) && hbm) {
                    winrt::com_ptr<IWICBitmap> wicBmp;
                    HRESULT hr = factory->CreateBitmapFromHBITMAP(hbm, nullptr, WICBitmapUseAlpha, wicBmp.put());
                    DeleteObject(hbm);
                    if (SUCCEEDED(hr) && wicBmp) {
                        if (auto res = ConvertWicSourceToWriteableBitmap(factory.get(), wicBmp.get())) {
                            return res;
                        }
                    }
                }
            }

            SHFILEINFOW sfi{};
            if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON) && sfi.hIcon) {
                winrt::com_ptr<IWICBitmap> wicBmp;
                HRESULT hr = factory->CreateBitmapFromHICON(sfi.hIcon, wicBmp.put());
                DestroyIcon(sfi.hIcon);
                if (SUCCEEDED(hr) && wicBmp) {
                    if (auto res = ConvertWicSourceToWriteableBitmap(factory.get(), wicBmp.get())) {
                        return res;
                    }
                }
            }
        }

        SHSTOCKICONINFO sii{};
        sii.cbSize = sizeof(sii);
        SHSTOCKICONID stockId = (isFolderBox || IsDirectoryPath(path)) ? SIID_FOLDER : SIID_DOCNOASSOC;
        if (SUCCEEDED(SHGetStockIconInfo(stockId, SHGSI_ICON | SHGSI_SMALLICON, &sii)) && sii.hIcon) {
            winrt::com_ptr<IWICBitmap> wicBmp;
            HRESULT hr = factory->CreateBitmapFromHICON(sii.hIcon, wicBmp.put());
            DestroyIcon(sii.hIcon);
            if (SUCCEEDED(hr) && wicBmp) {
                if (auto res = ConvertWicSourceToWriteableBitmap(factory.get(), wicBmp.get())) {
                    return res;
                }
            }
        }
    } catch (...) {}
    return nullptr;
}

// Icon cache (one per Explorer UI thread): extracting icons from the shell on every
// bar/dropdown refresh is expensive. One bitmap can be shared by many Image elements.
[[clang::no_destroy]] thread_local std::map<std::wstring, muxmi::WriteableBitmap> g_iconCache;

void ClearIconCache() {
    g_iconCache.clear();
}

mux::UIElement GetShellIconElement(const std::wstring& path, bool isFolderBox) {
    std::wstring key = isFolderBox ? std::wstring(L"<bookmark-folder>") : path;
    std::transform(key.begin(), key.end(), key.begin(), ::towlower);
    muxmi::WriteableBitmap bmp{nullptr};
    if (auto it = g_iconCache.find(key); it != g_iconCache.end()) {
        bmp = it->second;
    } else {
        bmp = ExtractShellBitmap(path, isFolderBox);
        if (bmp) {
            if (g_iconCache.size() > 400) g_iconCache.clear();
            g_iconCache.emplace(key, bmp);
        }
    }
    if (bmp) {
        muxc::Image img;
        img.Source(bmp);
        img.Width(16);
        img.Height(16);
        img.VerticalAlignment(mux::VerticalAlignment::Center);
        return img;
    }

    muxc::FontIcon fallback;
    fallback.Glyph((isFolderBox || IsDirectoryPath(path)) ? L"\uE8B7" : L"\uE7C3");
    fallback.FontFamily(muxm::FontFamily{L"Segoe Fluent Icons"});
    fallback.FontSize(16);
    fallback.VerticalAlignment(mux::VerticalAlignment::Center);
    if (isFolderBox) {
        fallback.Foreground(muxm::SolidColorBrush(winrt::Windows::UI::Color{255, kFolderTintR, kFolderTintG, kFolderTintB}));
    }
    return fallback;
}

int AddShellIconToImageList(HIMAGELIST hImgList, const std::wstring& path, bool isFolderBox, int size) {
    if (!hImgList) return -1;
    if (isFolderBox) {
        std::vector<BYTE> px;
        if (GetTintedFolderPixels(size, px)) {
            if (HBITMAP hbm = CreateBitmapFromPixels(px, size)) {
                int idx = ImageList_Add(hImgList, hbm, nullptr);
                DeleteObject(hbm);
                if (idx >= 0) return idx;
            }
        }
    }
    if (!isFolderBox && !path.empty() && !IsSlowPath(path)) {
        ComScope com;
        winrt::com_ptr<IShellItemImageFactory> imgFactory;
        if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr, IID_PPV_ARGS(imgFactory.put()))) && imgFactory) {
            HBITMAP hbm = nullptr;
            if (SUCCEEDED(imgFactory->GetImage(SIZE{size, size}, SIIGBF_ICONONLY, &hbm)) && hbm) {
                int idx = ImageList_Add(hImgList, hbm, nullptr);
                DeleteObject(hbm);
                if (idx >= 0) return idx;
            }
        }
        SHFILEINFOW sfi{};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_SMALLICON) && sfi.hIcon) {
            int idx = ImageList_ReplaceIcon(hImgList, -1, sfi.hIcon);
            DestroyIcon(sfi.hIcon);
            if (idx >= 0) return idx;
        }
    }
    SHSTOCKICONINFO sii{};
    sii.cbSize = sizeof(sii);
    SHSTOCKICONID stockId = (isFolderBox || IsDirectoryPath(path)) ? SIID_FOLDER : SIID_DOCNOASSOC;
    if (SUCCEEDED(SHGetStockIconInfo(stockId, SHGSI_ICON | SHGSI_SMALLICON, &sii)) && sii.hIcon) {
        int idx = ImageList_ReplaceIcon(hImgList, -1, sii.hIcon);
        DestroyIcon(sii.hIcon);
        return idx;
    }
    return -1;
}

// ============================================================================
// File / Folder Picker & Backup
// ============================================================================

std::wstring PickFileOrFolder(HWND owner, bool pickFolder, bool saveDialog = false, bool jsonOnly = false) {
    ComScope com;
    winrt::com_ptr<IFileDialog> dlg;
    CLSID clsid = saveDialog ? CLSID_FileSaveDialog : CLSID_FileOpenDialog;
    if (FAILED(CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(dlg.put()))))
        return {};
    FILEOPENDIALOGOPTIONS opts = 0;
    dlg->GetOptions(&opts);
    opts |= FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_NODEREFERENCELINKS;
    if (pickFolder) opts |= FOS_PICKFOLDERS;
    if (saveDialog) opts |= FOS_OVERWRITEPROMPT;
    dlg->SetOptions(opts);

    if (jsonOnly) {
        const COMDLG_FILTERSPEC filters[] = {{L"JSON Backup (*.json)", L"*.json"}, {L"All Files (*.*)", L"*.*"}};
        dlg->SetFileTypes(ARRAYSIZE(filters), filters);
        dlg->SetDefaultExtension(L"json");
        dlg->SetFileName(kExportFileName);
    }

    HRESULT shown = E_FAIL;
    {
        ModalScope scope;
        if (!scope.active) return {};
        IFileDialog* previous = g_threadFileDialog;
        g_threadFileDialog = dlg.get();
        shown = dlg->Show(owner);
        g_threadFileDialog = previous;
    }
    if (FAILED(shown)) return {};
    winrt::com_ptr<IShellItem> item;
    if (FAILED(dlg->GetResult(item.put()))) return {};
    PWSTR raw = nullptr;
    if (FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &raw)) || !raw) return {};
    std::wstring res(raw);
    CoTaskMemFree(raw);
    return NormalizePath(res);
}

void ExportBackupFile(HWND owner) {
    std::wstring path = PickFileOrFolder(owner, false, true, true);
    if (path.empty()) return;
    std::wstring json = SerializeNodes(LoadBookmarks());
    int len = WideCharToMultiByte(CP_UTF8, 0, json.data(), static_cast<int>(json.size()), nullptr, 0, nullptr, nullptr);
    if (len <= 0) return;
    std::string utf8(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, json.data(), static_cast<int>(json.size()), utf8.data(), len, nullptr, nullptr);
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(hFile, utf8.data(), static_cast<DWORD>(utf8.size()), &written, nullptr);
        CloseHandle(hFile);
    }
}

void ImportBackupFile(HWND owner) {
    std::wstring path = PickFileOrFolder(owner, false, false, true);
    if (path.empty()) return;
    HANDLE hFile = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return;
    LARGE_INTEGER sz{};
    if (GetFileSizeEx(hFile, &sz) && sz.QuadPart > 0 && sz.QuadPart < 4 * 1024 * 1024) {
        std::string utf8(static_cast<size_t>(sz.QuadPart), '\0');
        DWORD read = 0;
        if (ReadFile(hFile, utf8.data(), static_cast<DWORD>(utf8.size()), &read, nullptr) && read == utf8.size()) {
            int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), nullptr, 0);
            if (wlen > 0) {
                std::wstring wide(wlen, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.size()), wide.data(), wlen);
                if (!wide.empty() && wide[0] == 0xFEFF) wide.erase(0, 1);  // UTF-8 BOM
                std::vector<BookmarkNode> nodes;
                if (!TryDeserializeNodes(wide, nodes) || nodes.empty()) {
                    // Never replace the current bookmarks with nothing because of a bad file
                    ModMessageBox(owner, L"The selected file is not a valid bookmarks backup.", kImportMsgBoxTitle,
                                MB_OK | MB_ICONWARNING);
                } else if (!SaveBookmarks(nodes, true)) {
                    ModMessageBox(owner, L"The bookmarks could not be saved.", kImportMsgBoxTitle,
                                MB_OK | MB_ICONWARNING);
                }
            }
        }
    }
    CloseHandle(hFile);
}

void CopyTextToClipboard(HWND owner, const std::wstring& text) {
    if (!OpenClipboard(owner)) return;
    EmptyClipboard();
    size_t bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (h) {
        if (void* p = GlobalLock(h)) {
            std::memcpy(p, text.c_str(), bytes);
            GlobalUnlock(h);
            if (!SetClipboardData(CF_UNICODETEXT, h)) GlobalFree(h);
        } else {
            GlobalFree(h);
        }
    }
    CloseClipboard();
}

// ============================================================================
// Bookmark operations (Cut / Copy / Paste / Delete / Sort)
// ============================================================================

bool ClipboardHasData() {
    std::lock_guard lock(g_dataMutex);
    return g_clipboard.hasData;
}

// ---------------------------------------------------------------------------
// Duplicate bookmarks
//
// Adding a file or folder that is already bookmarked (anywhere: on the bar or in
// any folder) asks first. The same path may be kept in several places on
// purpose, so the user decides each time.
// ---------------------------------------------------------------------------

// Collects where a bookmark of `path` already is, e.g. "Bookmarks bar" or "t-1 > Sub".
void FindItemLocations(const std::vector<BookmarkNode>& nodes, const std::wstring& path,
                       const std::wstring& where, std::vector<std::wstring>& out) {
    for (const auto& n : nodes) {
        if (n.type == NodeType::Item && _wcsicmp(n.path.c_str(), path.c_str()) == 0) {
            if (std::find(out.begin(), out.end(), where) == out.end()) out.push_back(where);
        } else if (n.type == NodeType::FolderBox) {
            const std::wstring sub = (where == L"Bookmarks bar") ? n.name : where + L" \u203A " + n.name;
            FindItemLocations(n.children, path, sub, out);
        }
    }
}

std::wstring JoinLocations(const std::vector<std::wstring>& locations) {
    std::wstring text;
    for (size_t i = 0; i < locations.size(); ++i) {
        if (i) text += L", ";
        text += locations[i];
    }
    return text;
}

// Asks before adding bookmarks whose path is already bookmarked. Returns the
// bookmarks to add: all of them, only the new ones, or none. Runs a message box,
// so it must be called on a UI thread outside of hooks and XAML event handlers.
std::vector<BookmarkNode> ConfirmDuplicates(HWND owner, const std::vector<BookmarkNode>& existing,
                                            std::vector<BookmarkNode> candidates) {
    struct Dup {
        size_t index;
        std::vector<std::wstring> locations;
    };
    std::vector<Dup> dups;
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (candidates[i].type != NodeType::Item || candidates[i].path.empty()) continue;
        std::vector<std::wstring> locations;
        FindItemLocations(existing, candidates[i].path, L"Bookmarks bar", locations);
        if (!locations.empty()) dups.push_back({i, std::move(locations)});
    }
    if (dups.empty()) return candidates;

    constexpr int kAddAll = 101;
    constexpr int kAddNew = 102;
    int answer = IDCANCEL;
    if (candidates.size() == 1) {
        const std::wstring heading = L"\u201C" + candidates[0].name + L"\u201D is already bookmarked";
        const std::wstring body = L"It is already in: " + JoinLocations(dups[0].locations) +
                                  L".\n\nDo you want to add it again?";
        answer = ShowChoiceDialog(owner, heading, body, {{kAddAll, L"Add again"}, {IDCANCEL, L"Don\u2019t add"}},
                                  IDCANCEL, IDCANCEL);
    } else {
        const std::wstring heading = std::to_wstring(dups.size()) +
                                     (dups.size() == 1 ? L" item is already bookmarked" : L" items are already bookmarked");
        std::wstring body;
        const size_t shown = std::min<size_t>(dups.size(), 10);
        for (size_t i = 0; i < shown; ++i) {
            body += L"\u2022  " + candidates[dups[i].index].name + L"  (" + JoinLocations(dups[i].locations) + L")\n";
        }
        if (dups.size() > shown) body += L"\u2026 and " + std::to_wstring(dups.size() - shown) + L" more\n";
        body += L"\nDo you want to add them again?";
        answer = ShowChoiceDialog(owner, heading, body,
                                  {{kAddAll, L"Add all"}, {kAddNew, L"Add only new items"}, {IDCANCEL, L"Cancel"}},
                                  kAddNew, IDCANCEL);
    }
    if (answer == kAddAll) return candidates;
    if (answer == kAddNew) {
        std::vector<BookmarkNode> fresh;
        size_t d = 0;
        for (size_t i = 0; i < candidates.size(); ++i) {
            if (d < dups.size() && dups[d].index == i) {
                ++d;
                continue;
            }
            fresh.push_back(std::move(candidates[i]));
        }
        return fresh;
    }
    return {};
}

// Adds file/folder paths as bookmarks to `folder` (empty = the bar), asking
// about duplicates first. Runs on a UI thread.
void AddPathsAsBookmarks(HWND owner, const NodePath& folder, const std::vector<std::wstring>& paths) {
    if (g_unloading || paths.empty()) return;
    std::vector<BookmarkNode> candidates;
    for (const auto& raw : paths) {
        if (raw.empty()) continue;
        BookmarkNode n;
        n.type = NodeType::Item;
        n.path = NormalizePath(raw);
        n.name = DefaultLabel(n.path);
        candidates.push_back(std::move(n));
    }
    if (candidates.empty()) return;
    candidates = ConfirmDuplicates(owner, LoadBookmarks(), std::move(candidates));
    if (candidates.empty() || g_unloading) return;
    // Loaded again after the question, so a change made meanwhile isn't lost
    auto list = LoadBookmarks();
    auto* kids = FolderChildren(list, folder);
    if (!kids) return;
    for (auto& n : candidates) kids->push_back(std::move(n));
    SaveBookmarks(list);
}

void DoCutOrCopy(const std::vector<NodePath>& rawPaths, bool isCut) {
    auto paths = NormalizeSelection(rawPaths);
    if (paths.empty()) return;
    auto list = LoadBookmarks();
    std::vector<BookmarkNode> nodes;
    for (const auto& p : paths) {
        if (const BookmarkNode* n = NodeAt(list, p)) nodes.push_back(*n);
    }
    if (nodes.empty()) return;
    {
        std::lock_guard lock(g_dataMutex);
        g_clipboard.hasData = true;
        g_clipboard.isCut = isCut;
        g_clipboard.nodes = std::move(nodes);
    }
    if (isCut && RemoveNodes(list, paths)) {
        SaveBookmarks(list);
    }
}

// Where Chrome pastes: into a selected folder, or right after the selected bookmark.
void PasteTargetFor(const std::vector<BookmarkNode>& list, const std::vector<NodePath>& targets,
                    const NodePath& fallbackFolder, NodePath& outFolder, int& outIndex) {
    outFolder = fallbackFolder;
    outIndex = -1;
    if (targets.empty()) return;
    const NodePath& last = targets.back();
    if (last.empty()) {
        outFolder = {};
        return;
    }
    const BookmarkNode* n = NodeAt(list, last);
    if (!n) return;
    if (targets.size() == 1 && n->type == NodeType::FolderBox) {
        outFolder = last;
        outIndex = -1;
    } else {
        outFolder = ParentOf(last);
        outIndex = last.back() + 1;
    }
}

void DoPaste(const NodePath& folder, int index, HWND owner = nullptr) {
    ClipboardState clip;
    {
        std::lock_guard lock(g_dataMutex);
        if (!g_clipboard.hasData) return;
        clip = g_clipboard;
        if (g_clipboard.isCut) {
            g_clipboard.hasData = false;
            g_clipboard.isCut = false;
            g_clipboard.nodes.clear();
        }
    }
    std::vector<BookmarkNode> nodes = std::move(clip.nodes);
    if (!clip.isCut) {
        // Pasting a copy adds bookmarks that already exist: ask first
        nodes = ConfirmDuplicates(owner ? owner : GetActiveWindow(), LoadBookmarks(), std::move(nodes));
        if (nodes.empty() || g_unloading) return;
    }
    auto list = LoadBookmarks();
    if (!FolderChildren(list, folder)) return;
    InsertNodes(list, folder, index, nodes);
    SaveBookmarks(list);
}

void DoDelete(const std::vector<NodePath>& paths) {
    auto list = LoadBookmarks();
    if (RemoveNodes(list, paths)) SaveBookmarks(list);
}

void SortFolderByName(const NodePath& folder) {
    auto list = LoadBookmarks();
    auto* kids = FolderChildren(list, folder);
    if (!kids || kids->size() < 2) return;
    std::stable_sort(kids->begin(), kids->end(), [](const BookmarkNode& a, const BookmarkNode& b) {
        bool af = a.type == NodeType::FolderBox;
        bool bf = b.type == NodeType::FolderBox;
        if (af != bf) return af;  // folders first, like Chrome
        int r = CompareStringEx(LOCALE_NAME_USER_DEFAULT, NORM_IGNORECASE | SORT_DIGITSASNUMBERS,
                                a.name.c_str(), -1, b.name.c_str(), -1, nullptr, nullptr, 0);
        return r == CSTR_LESS_THAN;
    });
    SaveBookmarks(list);
}

// The mod's own module handle. The window classes are registered with it, so
// they belong to the mod and not to explorer.exe.
HINSTANCE ModuleInstance() {
    HMODULE mod = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ModuleInstance), &mod);
    return mod;
}

// ============================================================================
// Theme (Light / Dark / System default)
// ============================================================================

struct UiColors {
    COLORREF bg;
    COLORREF text;
    COLORREF subText;
    COLORREF inputBg;
    COLORREF headerLine;
    COLORREF divider;
};

int ReadThemeSetting() {
    auto theme = WindhawkUtils::StringSetting::make(L"theme");
    if (wcscmp(theme.get(), L"light") == 0) return 1;
    if (wcscmp(theme.get(), L"dark") == 0) return 2;
    return 0;
}

bool SystemUsesDarkApps() {
    DWORD v = 1, sz = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS) {
        return v == 0;
    }
    return false;
}

bool IsDarkThemeActive() {
    int t = g_themeSetting;
    if (t == 1) return false;
    if (t == 2) return true;
    return SystemUsesDarkApps();
}

UiColors ThemeColors(bool dark) {
    if (dark) {
        return {RGB(32, 33, 36), RGB(232, 234, 237), RGB(154, 160, 166), RGB(48, 49, 52), RGB(60, 64, 67),
                RGB(130, 134, 139)};
    }
    return {RGB(255, 255, 255), RGB(32, 33, 36), RGB(95, 99, 104), RGB(255, 255, 255), RGB(218, 220, 224),
            RGB(0, 0, 0)};
}

void ApplyTitleBarTheme(HWND hwnd, bool dark) {
    BOOL v = dark ? TRUE : FALSE;
    if (FAILED(DwmSetWindowAttribute(hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &v, sizeof(v)))) {
        DwmSetWindowAttribute(hwnd, 19, &v, sizeof(v));
    }
}

// Buttons, edits and list boxes of a window follow the theme
void ThemeChildControls(HWND parent, bool dark) {
    EnumChildWindows(parent, [](HWND c, LPARAM lp) -> BOOL {
        bool dk = lp != 0;
        wchar_t cls[32]{};
        GetClassNameW(c, cls, ARRAYSIZE(cls));
        if (_wcsicmp(cls, L"Button") == 0 || _wcsicmp(cls, L"ListBox") == 0) {
            SetWindowTheme(c, dk ? L"DarkMode_Explorer" : L"Explorer", nullptr);
        } else if (_wcsicmp(cls, L"Edit") == 0) {
            SetWindowTheme(c, dk ? L"DarkMode_CFD" : nullptr, nullptr);
        }
        return TRUE;
    }, dark ? 1 : 0);
}

// ============================================================================
// Chrome-style "Edit bookmark" / "Add folder" / "Rename folder" dialog
// ============================================================================

// ---------------------------------------------------------------------------
// DPI helpers. Explorer is per-monitor DPI aware, so Windows doesn't scale the
// mod's Win32 windows; sizes are given at 96 DPI and scaled here.
// ---------------------------------------------------------------------------

UINT WindowDpi(HWND hwnd) {
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 0;
    return dpi ? dpi : GetDpiForSystem();
}

int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

HFONT CreateUiFont(int heightAt96, int weight, UINT dpi) {
    return CreateFontW(-ScaleForDpi(heightAt96, dpi), 0, 0, 0, weight, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

// Keeps a window of the given size (at 96 DPI) inside the work area of the
// monitor of `near`, centered on it.
RECT CenteredWindowRect(HWND nearWindow, int widthAt96, int heightAt96, UINT dpi) {
    int width = ScaleForDpi(widthAt96, dpi);
    int height = ScaleForDpi(heightAt96, dpi);
    RECT anchor{};
    if (!nearWindow || !GetWindowRect(nearWindow, &anchor)) {
        POINT pt{};
        GetCursorPos(&pt);
        anchor = RECT{pt.x, pt.y, pt.x, pt.y};
    }
    MONITORINFO mi{sizeof(mi)};
    HMONITOR mon = MonitorFromRect(&anchor, MONITOR_DEFAULTTONEAREST);
    if (mon && GetMonitorInfoW(mon, &mi)) {
        width = std::min<int>(width, mi.rcWork.right - mi.rcWork.left);
        height = std::min<int>(height, mi.rcWork.bottom - mi.rcWork.top);
    }
    int x = anchor.left + ((anchor.right - anchor.left) - width) / 2;
    int y = anchor.top + ((anchor.bottom - anchor.top) - height) / 2;
    if (mon) {
        x = std::clamp<int>(x, mi.rcWork.left, std::max<int>(mi.rcWork.left, mi.rcWork.right - width));
        y = std::clamp<int>(y, mi.rcWork.top, std::max<int>(mi.rcWork.top, mi.rcWork.bottom - height));
    }
    return RECT{x, y, x + width, y + height};
}

struct EditDialogData {
    bool isNew = false;
    bool folderOnly = false;
    std::wstring name;
    std::wstring path;
    NodePath folder;  // parent folder
    bool confirmed = false;

    HWND hName = nullptr;
    HWND hPath = nullptr;
    HWND hFolders = nullptr;
    HFONT hFontTitle = nullptr;
    HFONT hFont = nullptr;
    UINT dpi = 96;
    std::vector<NodePath> listPaths;
    bool dark = false;
    UiColors colors{};
    HBRUSH hBg = nullptr;
    HBRUSH hInput = nullptr;
};

constexpr int IDC_EDIT_NAME = 1001;
constexpr int IDC_EDIT_PATH = 1002;
constexpr int IDC_BTN_BROWSE_FILE = 1003;
constexpr int IDC_LIST_FOLDERS = 1004;
constexpr int IDC_BTN_NEW_FOLDER = 1005;
constexpr int IDC_BTN_SAVE = 1006;
constexpr int IDC_BTN_CANCEL = 1007;
constexpr int IDC_BTN_BROWSE_DIR = 1008;

void AddFolderRows(EditDialogData* d, const std::vector<BookmarkNode>& kids, NodePath& cur, int depth, int& selIdx) {
    if (depth > kMaxDepth) return;
    for (size_t i = 0; i < kids.size(); ++i) {
        if (kids[i].type != NodeType::FolderBox) continue;
        cur.push_back(static_cast<int>(i));
        std::wstring label(static_cast<size_t>(depth) * 4, L' ');
        label += L"\U0001F4C1  " + kids[i].name;
        int idx = static_cast<int>(SendMessageW(d->hFolders, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str())));
        SendMessageW(d->hFolders, LB_SETITEMDATA, idx, static_cast<LPARAM>(d->listPaths.size()));
        d->listPaths.push_back(cur);
        if (cur == d->folder) selIdx = idx;
        AddFolderRows(d, kids[i].children, cur, depth + 1, selIdx);
        cur.pop_back();
    }
}

void PopulateFolderList(EditDialogData* d) {
    SendMessageW(d->hFolders, LB_RESETCONTENT, 0, 0);
    d->listPaths.clear();
    int rootIdx = static_cast<int>(SendMessageW(d->hFolders, LB_ADDSTRING, 0,
        reinterpret_cast<LPARAM>(L"\U0001F4C1  Bookmarks bar")));
    SendMessageW(d->hFolders, LB_SETITEMDATA, rootIdx, 0);
    d->listPaths.push_back({});
    int selIdx = rootIdx;
    auto list = LoadBookmarks();
    NodePath cur;
    AddFolderRows(d, list, cur, 1, selIdx);
    SendMessageW(d->hFolders, LB_SETCURSEL, selIdx, 0);
}

NodePath SelectedDialogFolder(EditDialogData* d) {
    int sel = static_cast<int>(SendMessageW(d->hFolders, LB_GETCURSEL, 0, 0));
    if (sel == LB_ERR) return d->folder;
    size_t idx = static_cast<size_t>(SendMessageW(d->hFolders, LB_GETITEMDATA, sel, 0));
    return idx < d->listPaths.size() ? d->listPaths[idx] : NodePath{};
}

HWND MakeChild(HWND parent, const wchar_t* cls, const wchar_t* text, DWORD style, int x, int y, int w, int h,
               int id, HFONT font, DWORD exStyle = 0) {
    // Positions are given at 96 DPI
    const UINT dpi = WindowDpi(parent);
    x = ScaleForDpi(x, dpi);
    y = ScaleForDpi(y, dpi);
    w = ScaleForDpi(w, dpi);
    h = ScaleForDpi(h, dpi);
    HWND c = CreateWindowExW(exStyle, cls, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, parent,
                             reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), nullptr, nullptr);
    if (c && font) SendMessageW(c, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    return c;
}

static LRESULT EditBookmarkWndProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* data = reinterpret_cast<EditDialogData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            data = reinterpret_cast<EditDialogData*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));
            data->dark = IsDarkThemeActive();
            data->colors = ThemeColors(data->dark);
            data->hBg = CreateSolidBrush(data->colors.bg);
            data->hInput = CreateSolidBrush(data->colors.inputBg);

            data->dpi = WindowDpi(hwnd);
            data->hFontTitle = CreateUiFont(20, FW_SEMIBOLD, data->dpi);
            data->hFont = CreateUiFont(14, FW_NORMAL, data->dpi);

            const wchar_t* heading = data->folderOnly
                ? (data->isNew ? L"Add folder" : L"Rename folder")
                : (data->isNew ? L"Add bookmark" : L"Edit bookmark");

            MakeChild(hwnd, L"STATIC", heading, 0, 20, 16, 410, 28, 0, data->hFontTitle);
            MakeChild(hwnd, L"STATIC", L"Name", 0, 20, 54, 410, 20, 0, data->hFont);
            data->hName = MakeChild(hwnd, L"EDIT", data->name.c_str(), WS_TABSTOP | ES_AUTOHSCROLL,
                                    20, 76, 415, 26, IDC_EDIT_NAME, data->hFont, WS_EX_CLIENTEDGE);

            int btnY = 120;
            if (!data->folderOnly) {
                MakeChild(hwnd, L"STATIC", L"File or folder path", 0, 20, 112, 410, 20, 0, data->hFont);
                data->hPath = MakeChild(hwnd, L"EDIT", data->path.c_str(), WS_TABSTOP | ES_AUTOHSCROLL,
                                        20, 134, 225, 26, IDC_EDIT_PATH, data->hFont, WS_EX_CLIENTEDGE);
                MakeChild(hwnd, L"BUTTON", L"File...", WS_TABSTOP | BS_PUSHBUTTON,
                          251, 133, 88, 28, IDC_BTN_BROWSE_FILE, data->hFont);
                MakeChild(hwnd, L"BUTTON", L"Folder...", WS_TABSTOP | BS_PUSHBUTTON,
                          345, 133, 90, 28, IDC_BTN_BROWSE_DIR, data->hFont);
                MakeChild(hwnd, L"STATIC", L"Folder", 0, 20, 172, 410, 20, 0, data->hFont);
                data->hFolders = MakeChild(hwnd, L"LISTBOX", nullptr,
                                           WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT,
                                           20, 194, 415, 145, IDC_LIST_FOLDERS, data->hFont, WS_EX_CLIENTEDGE);
                PopulateFolderList(data);
                MakeChild(hwnd, L"BUTTON", L"New folder", WS_TABSTOP | BS_PUSHBUTTON,
                          20, 355, 105, 30, IDC_BTN_NEW_FOLDER, data->hFont);
                btnY = 355;
            }

            MakeChild(hwnd, L"BUTTON", L"Save", WS_TABSTOP | BS_DEFPUSHBUTTON,
                      255, btnY, 85, 30, IDC_BTN_SAVE, data->hFont);
            MakeChild(hwnd, L"BUTTON", L"Cancel", WS_TABSTOP | BS_PUSHBUTTON,
                      350, btnY, 85, 30, IDC_BTN_CANCEL, data->hFont);

            ApplyTitleBarTheme(hwnd, data->dark);
            ThemeChildControls(hwnd, data->dark);
            SetFocus(data->hName);
            SendMessageW(data->hName, EM_SETSEL, 0, -1);
            return 0;
        }

        case WM_ERASEBKGND:
            if (data && data->hBg) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                FillRect(reinterpret_cast<HDC>(wParam), &rc, data->hBg);
                return 1;
            }
            break;

        case WM_CTLCOLORSTATIC: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetBkMode(hdc, TRANSPARENT);
            if (data && data->hBg) {
                SetTextColor(hdc, data->colors.text);
                SetBkColor(hdc, data->colors.bg);
                return reinterpret_cast<LRESULT>(data->hBg);
            }
            return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
        }

        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
            if (data && data->hInput) {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                SetTextColor(hdc, data->colors.text);
                SetBkColor(hdc, data->colors.inputBg);
                return reinterpret_cast<LRESULT>(data->hInput);
            }
            break;

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (!data) break;
            if (id == IDC_BTN_BROWSE_FILE || id == IDC_BTN_BROWSE_DIR) {
                std::wstring picked = PickFileOrFolder(hwnd, id == IDC_BTN_BROWSE_DIR);
                if (!picked.empty()) {
                    SetWindowTextW(data->hPath, picked.c_str());
                    wchar_t curName[512]{};
                    GetWindowTextW(data->hName, curName, ARRAYSIZE(curName));
                    if (wcslen(curName) == 0) {
                        std::wstring defName = DefaultLabel(picked);
                        SetWindowTextW(data->hName, defName.c_str());
                    }
                }
                return 0;
            }
            if (id == IDC_BTN_NEW_FOLDER && data->hFolders) {
                NodePath parent = SelectedDialogFolder(data);
                auto list = LoadBookmarks();
                auto* kids = FolderChildren(list, parent);
                if (!kids) {
                    parent.clear();
                    kids = &list;
                }
                BookmarkNode box;
                box.type = NodeType::FolderBox;
                box.name = UniqueFolderName(*kids, L"New folder");
                kids->push_back(std::move(box));
                data->folder = ChildOf(parent, static_cast<int>(kids->size()) - 1);
                SaveBookmarks(list);
                PopulateFolderList(data);
                return 0;
            }
            if (id == IDC_BTN_SAVE || id == IDOK) {
                wchar_t nameBuf[1024]{};
                GetWindowTextW(data->hName, nameBuf, ARRAYSIZE(nameBuf));
                std::wstring name = nameBuf;
                std::wstring path;
                if (!data->folderOnly) {
                    wchar_t pathBuf[4096]{};
                    GetWindowTextW(data->hPath, pathBuf, ARRAYSIZE(pathBuf));
                    path = NormalizePath(pathBuf);
                    if (path.empty()) {
                        MessageBeep(MB_ICONWARNING);
                        SetFocus(data->hPath);
                        return 0;
                    }
                    if (name.empty()) name = DefaultLabel(path);
                    data->folder = SelectedDialogFolder(data);
                } else if (name.empty()) {
                    MessageBeep(MB_ICONWARNING);
                    SetFocus(data->hName);
                    return 0;
                }
                data->name = name;
                data->path = path;
                data->confirmed = true;
                DestroyWindow(hwnd);
                return 0;
            }
            if (id == IDC_BTN_CANCEL || id == IDCANCEL) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }

        case WM_DPICHANGED:
            if (data) {
                const UINT newDpi = HIWORD(wParam);
                const UINT oldDpi = data->dpi ? data->dpi : 96;
                HFONT oldTitle = data->hFontTitle;
                HFONT oldFont = data->hFont;
                data->dpi = newDpi;
                data->hFontTitle = CreateUiFont(20, FW_SEMIBOLD, newDpi);
                data->hFont = CreateUiFont(14, FW_NORMAL, newDpi);
                struct Ctx { HWND parent; UINT oldDpi, newDpi; HFONT oldTitle, newTitle, newFont; } ctx{
                    hwnd, oldDpi, newDpi, oldTitle, data->hFontTitle, data->hFont};
                EnumChildWindows(hwnd, [](HWND child, LPARAM lp) -> BOOL {
                    auto* c = reinterpret_cast<Ctx*>(lp);
                    if (GetParent(child) != c->parent) return TRUE;
                    RECT r{};
                    GetWindowRect(child, &r);
                    MapWindowPoints(nullptr, c->parent, reinterpret_cast<POINT*>(&r), 2);
                    auto sc = [c](LONG v) { return MulDiv(v, static_cast<int>(c->newDpi), static_cast<int>(c->oldDpi)); };
                    MoveWindow(child, sc(r.left), sc(r.top), sc(r.right - r.left), sc(r.bottom - r.top), FALSE);
                    HFONT cur = reinterpret_cast<HFONT>(SendMessageW(child, WM_GETFONT, 0, 0));
                    SendMessageW(child, WM_SETFONT,
                                 reinterpret_cast<WPARAM>(cur == c->oldTitle ? c->newTitle : c->newFont), FALSE);
                    return TRUE;
                }, reinterpret_cast<LPARAM>(&ctx));
                if (oldTitle) DeleteObject(oldTitle);
                if (oldFont) DeleteObject(oldFont);
                auto* suggested = reinterpret_cast<RECT*>(lParam);
                SetWindowPos(hwnd, nullptr, suggested->left, suggested->top, suggested->right - suggested->left,
                             suggested->bottom - suggested->top, SWP_NOZORDER | SWP_NOACTIVATE);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (data) {
                if (data->hFontTitle) DeleteObject(data->hFontTitle);
                if (data->hFont) DeleteObject(data->hFont);
                data->hFontTitle = nullptr;
                data->hFont = nullptr;
                if (data->hBg) DeleteObject(data->hBg);
                if (data->hInput) DeleteObject(data->hInput);
                data->hBg = nullptr;
                data->hInput = nullptr;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Crash guards: an exception must never escape into Explorer (it would terminate the process)
LRESULT CALLBACK EditBookmarkWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    try {
        return EditBookmarkWndProcImpl(hwnd, msg, wParam, lParam);
    } catch (...) {
        Wh_Log(L"EditBookmarkWndProc: exception (msg %u)", msg);
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

bool ShowEditModal(HWND owner, EditDialogData& data) {
    ModalScope scope;
    if (!scope.active) return false;

    const RECT wr = CenteredWindowRect(owner, 465, data.folderOnly ? 200 : 440, WindowDpi(owner));
    const int x = wr.left, y = wr.top, width = wr.right - wr.left, height = wr.bottom - wr.top;

    const wchar_t* winTitle = data.folderOnly
        ? (data.isNew ? L"Add folder" : L"Rename folder")
        : (data.isNew ? L"Add bookmark" : L"Edit bookmark");

    HWND hDlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME, kEditDlgClass, winTitle,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x, y, width, height, owner, nullptr, ModuleInstance(), &data);

    if (!hDlg) return false;
    if (owner) EnableWindow(owner, FALSE);
    HWND previousDialog = g_threadEditDialog;
    g_threadEditDialog = hDlg;

    MSG msg{};
    while (IsWindow(hDlg) && GetMessageW(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            DestroyWindow(hDlg);
            break;
        }
        if (!IsDialogMessageW(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    g_threadEditDialog = previousDialog;
    if (msg.message == WM_QUIT) PostQuitMessage(static_cast<int>(msg.wParam));

    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }
    return data.confirmed;
}

// ---------------------------------------------------------------------------
// Question dialog (e.g. duplicate bookmarks), styled like the edit dialog: a
// larger heading, a warning icon, buttons with clear labels, and light/dark
// colors from the mod's Theme setting (System default follows Windows).
// ---------------------------------------------------------------------------

struct ChoiceDialogData {
    std::wstring heading;
    std::wstring body;
    ChoiceButtons buttons;
    int defaultId = IDCANCEL;
    int cancelId = IDCANCEL;
    int result = IDCANCEL;
    UINT dpi = 96;
    bool dark = false;
    UiColors colors{};
    HBRUSH hBg = nullptr;
    HFONT hHeading = nullptr;
    HFONT hBody = nullptr;
    HICON hIcon = nullptr;
    RECT iconRc{};
    RECT headingRc{};
    RECT bodyRc{};
    std::vector<HWND> buttonWnds;
};

void ChoiceFreeResources(ChoiceDialogData* d) {
    if (d->hHeading) DeleteObject(d->hHeading);
    if (d->hBody) DeleteObject(d->hBody);
    if (d->hIcon) DestroyIcon(d->hIcon);
    d->hHeading = nullptr;
    d->hBody = nullptr;
    d->hIcon = nullptr;
}

// Creates the DPI-dependent resources, measures the text, places the buttons and
// sizes the window. `suggested` is the rectangle from WM_DPICHANGED, if any.
void ChoiceLayout(HWND hwnd, ChoiceDialogData* d, HWND owner, const RECT* suggested) {
    ChoiceFreeResources(d);
    auto S = [d](int v) { return ScaleForDpi(v, d->dpi); };
    d->hHeading = CreateUiFont(20, FW_SEMIBOLD, d->dpi);
    d->hBody = CreateUiFont(14, FW_NORMAL, d->dpi);
    const int iconSize = S(32);
    LoadIconWithScaleDown(nullptr, IDI_WARNING, iconSize, iconSize, &d->hIcon);

    const int pad = S(24);
    const int clientW = S(460);
    const int textX = pad + iconSize + S(16);
    const int textW = clientW - textX - pad;

    HDC hdc = GetDC(hwnd);
    RECT hr{0, 0, textW, 0};
    HFONT old = reinterpret_cast<HFONT>(SelectObject(hdc, d->hHeading));
    DrawTextW(hdc, d->heading.c_str(), -1, &hr, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    RECT br{0, 0, textW, 0};
    SelectObject(hdc, d->hBody);
    DrawTextW(hdc, d->body.c_str(), -1, &br, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    std::vector<int> widths;
    for (const auto& [id, text] : d->buttons) {
        SIZE sz{};
        GetTextExtentPoint32W(hdc, text.c_str(), static_cast<int>(text.size()), &sz);
        widths.push_back(std::max<int>(S(96), sz.cx + S(32)));
    }
    SelectObject(hdc, old);
    ReleaseDC(hwnd, hdc);

    int y = pad;
    d->iconRc = RECT{pad, y, pad + iconSize, y + iconSize};
    d->headingRc = RECT{textX, y, textX + textW, y + (hr.bottom - hr.top)};
    y = d->headingRc.bottom + S(10);
    d->bodyRc = RECT{textX, y, textX + textW, y + (br.bottom - br.top)};
    y = std::max<int>(d->bodyRc.bottom, d->iconRc.bottom) + S(24);

    const int btnH = S(32);
    const int gap = S(8);
    int x = clientW - pad;
    for (size_t i = d->buttons.size(); i-- > 0;) {
        x -= widths[i];
        if (i < d->buttonWnds.size() && d->buttonWnds[i]) {
            MoveWindow(d->buttonWnds[i], x, y, widths[i], btnH, TRUE);
            SendMessageW(d->buttonWnds[i], WM_SETFONT, reinterpret_cast<WPARAM>(d->hBody), TRUE);
        }
        x -= gap;
    }
    const int clientH = y + btnH + pad;

    const DWORD style = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE));
    const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    RECT wr{0, 0, clientW, clientH};
    AdjustWindowRectExForDpi(&wr, style, FALSE, exStyle, d->dpi);
    const int w = wr.right - wr.left;
    const int h = wr.bottom - wr.top;
    int px = 0, py = 0;
    if (suggested) {
        px = suggested->left;
        py = suggested->top;
    } else {
        RECT anchor{};
        if (!owner || !GetWindowRect(owner, &anchor)) {
            POINT pt{};
            GetCursorPos(&pt);
            anchor = RECT{pt.x, pt.y, pt.x, pt.y};
        }
        px = anchor.left + ((anchor.right - anchor.left) - w) / 2;
        py = anchor.top + ((anchor.bottom - anchor.top) - h) / 2;
        MONITORINFO mi{sizeof(mi)};
        if (GetMonitorInfoW(MonitorFromRect(&anchor, MONITOR_DEFAULTTONEAREST), &mi)) {
            px = std::clamp<int>(px, mi.rcWork.left, std::max<int>(mi.rcWork.left, mi.rcWork.right - w));
            py = std::clamp<int>(py, mi.rcWork.top, std::max<int>(mi.rcWork.top, mi.rcWork.bottom - h));
        }
    }
    SetWindowPos(hwnd, nullptr, px, py, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
    InvalidateRect(hwnd, nullptr, TRUE);
}

static LRESULT ChoiceDialogWndProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* d = reinterpret_cast<ChoiceDialogData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    switch (msg) {
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            d = reinterpret_cast<ChoiceDialogData*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(d));
            d->dpi = WindowDpi(hwnd);
            d->dark = IsDarkThemeActive();
            d->colors = ThemeColors(d->dark);
            d->hBg = CreateSolidBrush(d->colors.bg);
            for (const auto& [id, text] : d->buttons) {
                DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | (id == d->defaultId ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON);
                d->buttonWnds.push_back(CreateWindowExW(0, L"BUTTON", text.c_str(), style, 0, 0, 10, 10, hwnd,
                                                        reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                                                        nullptr, nullptr));
            }
            ChoiceLayout(hwnd, d, cs->hwndParent, nullptr);
            ApplyTitleBarTheme(hwnd, d->dark);
            ThemeChildControls(hwnd, d->dark);
            return 0;
        }
        case DM_GETDEFID:
            if (d) return MAKELRESULT(d->defaultId, DC_HASDEFID);
            break;
        case WM_ERASEBKGND:
            if (d && d->hBg) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                FillRect(reinterpret_cast<HDC>(wParam), &rc, d->hBg);
                return 1;
            }
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            if (d) {
                if (d->hIcon) {
                    DrawIconEx(hdc, d->iconRc.left, d->iconRc.top, d->hIcon, d->iconRc.right - d->iconRc.left,
                               d->iconRc.bottom - d->iconRc.top, 0, nullptr, DI_NORMAL);
                }
                SetBkMode(hdc, TRANSPARENT);
                HFONT old = reinterpret_cast<HFONT>(SelectObject(hdc, d->hHeading));
                SetTextColor(hdc, d->colors.text);
                RECT hr = d->headingRc;
                DrawTextW(hdc, d->heading.c_str(), -1, &hr, DT_WORDBREAK | DT_NOPREFIX);
                SelectObject(hdc, d->hBody);
                RECT br = d->bodyRc;
                DrawTextW(hdc, d->body.c_str(), -1, &br, DT_WORDBREAK | DT_NOPREFIX);
                SelectObject(hdc, old);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CTLCOLORBTN:
            if (d && d->hBg) return reinterpret_cast<LRESULT>(d->hBg);
            break;
        case WM_COMMAND:
            if (d && HIWORD(wParam) == BN_CLICKED) {
                const int id = LOWORD(wParam);
                for (const auto& button : d->buttons) {
                    if (button.first == id) {
                        d->result = id;
                        DestroyWindow(hwnd);
                        return 0;
                    }
                }
                if (id == IDCANCEL) {
                    d->result = d->cancelId;
                    DestroyWindow(hwnd);
                    return 0;
                }
            }
            break;
        case WM_DPICHANGED:
            if (d) {
                d->dpi = HIWORD(wParam);
                ChoiceLayout(hwnd, d, nullptr, reinterpret_cast<RECT*>(lParam));
            }
            return 0;
        case WM_CLOSE:
            if (d) d->result = d->cancelId;
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            if (d) {
                ChoiceFreeResources(d);
                if (d->hBg) DeleteObject(d->hBg);
                d->hBg = nullptr;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK ChoiceDialogWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    try {
        return ChoiceDialogWndProcImpl(hwnd, msg, wParam, lParam);
    } catch (...) {
        Wh_Log(L"ChoiceDialogWndProc: exception");
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

// Shows the question and returns the id of the chosen button (cancelId when the
// dialog is closed, or when it can't be shown because the mod is unloading).
int ShowChoiceDialog(HWND owner, const std::wstring& heading, const std::wstring& body, const ChoiceButtons& buttons,
                     int defaultId, int cancelId) {
    ModalScope scope;
    if (!scope.active) return cancelId;
    if (!owner) owner = GetActiveWindow();
    if (owner) owner = GetAncestor(owner, GA_ROOT);

    ChoiceDialogData data;
    data.heading = heading;
    data.body = body;
    data.buttons = buttons;
    data.defaultId = defaultId;
    data.cancelId = cancelId;
    data.result = cancelId;

    HWND hDlg = CreateWindowExW(WS_EX_DLGMODALFRAME, kChoiceDlgClass, kMsgBoxTitle,
                                WS_POPUP | WS_CAPTION | WS_SYSMENU, 0, 0, 10, 10, owner, nullptr,
                                ModuleInstance(), &data);
    if (!hDlg) return cancelId;
    if (owner) EnableWindow(owner, FALSE);
    HWND previousDialog = g_threadEditDialog;
    g_threadEditDialog = hDlg;  // closed by CloseThreadModals on unload
    MessageBeep(MB_ICONWARNING);
    ShowWindow(hDlg, SW_SHOW);
    for (size_t i = 0; i < data.buttons.size() && i < data.buttonWnds.size(); ++i) {
        if (data.buttons[i].first == defaultId) SetFocus(data.buttonWnds[i]);
    }

    MSG msg{};
    while (IsWindow(hDlg) && GetMessageW(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            data.result = cancelId;
            DestroyWindow(hDlg);
            break;
        }
        if (!IsDialogMessageW(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    g_threadEditDialog = previousDialog;
    if (msg.message == WM_QUIT) PostQuitMessage(static_cast<int>(msg.wParam));
    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }
    return data.result;
}

void EditNodeFlow(HWND owner, const NodePath& path) {
    auto list = LoadBookmarks();
    const BookmarkNode* n = NodeAt(list, path);
    if (!n) return;
    EditDialogData d;
    d.isNew = false;
    d.folderOnly = (n->type == NodeType::FolderBox);
    d.name = n->name;
    d.path = n->path;
    d.folder = ParentOf(path);
    if (!ShowEditModal(owner, d)) return;

    list = LoadBookmarks();
    BookmarkNode* node = NodeAt(list, path);
    if (!node) return;
    node->name = d.name;
    if (!d.folderOnly) {
        node->path = d.path;
        if (d.folder != ParentOf(path)) MoveNodes(list, {path}, d.folder, -1);
    }
    SaveBookmarks(list);
}

void AddBookmarkFlow(HWND owner, const NodePath& folder, int index, const std::wstring& presetPath = L"") {
    EditDialogData d;
    d.isNew = true;
    d.path = presetPath;
    d.name = presetPath.empty() ? L"" : DefaultLabel(presetPath);
    d.folder = folder;
    if (!ShowEditModal(owner, d)) return;

    BookmarkNode n;
    n.type = NodeType::Item;
    n.name = d.name;
    n.path = d.path;
    if (ConfirmDuplicates(owner, LoadBookmarks(), {n}).empty()) return;
    auto list = LoadBookmarks();
    NodePath dest = d.folder;
    if (!FolderChildren(list, dest)) dest.clear();
    if (dest != folder) index = -1;
    InsertNodes(list, dest, index, {n});
    SaveBookmarks(list);
}

void AddFolderFlow(HWND owner, const NodePath& parent, int index) {
    auto list = LoadBookmarks();
    const auto* kids = FolderChildren(list, parent);
    EditDialogData d;
    d.isNew = true;
    d.folderOnly = true;
    d.name = UniqueFolderName(kids ? *kids : list, L"New folder");
    if (!ShowEditModal(owner, d)) return;

    list = LoadBookmarks();
    NodePath dest = parent;
    if (!FolderChildren(list, dest)) {
        dest.clear();
        index = -1;
    }
    BookmarkNode box;
    box.type = NodeType::FolderBox;
    box.name = d.name;
    InsertNodes(list, dest, index, {box});
    SaveBookmarks(list);
}

// ============================================================================
// Chrome-style Bookmark Manager
//   Header: "Bookmarks" + search + ⋮ (Organize menu)
//   Left:   folder tree     Right: bookmarks list (name, path, ⋮)
//   Right-click menus, drag & drop, multi-select, keyboard shortcuts
// ============================================================================

struct ManagerWindowData {
    HWND hOwnerExplorer = nullptr;
    HWND hTitle = nullptr;
    HWND hSearch = nullptr;
    HWND hMore = nullptr;
    HWND hTree = nullptr;
    HWND hList = nullptr;
    HWND hEmpty = nullptr;
    HWND hSelLabel = nullptr;
    HWND hSelDelete = nullptr;
    HWND hSelClear = nullptr;
    HWND hDropLine = nullptr;
    HIMAGELIST hListImages = nullptr;
    HIMAGELIST hTreeImages = nullptr;
    HFONT hFontTitle = nullptr;
    HFONT hFontUI = nullptr;
    HFONT hFontIcon = nullptr;
    HFONT hFontSmall = nullptr;
    UINT dpi = 96;

    NodePath currentFolder;
    std::vector<NodePath> displayed;
    std::vector<NodePath> treePaths;
    bool searching = false;
    bool ignoreSearchChange = false;
    bool suppressTreeSel = false;

    bool dragging = false;
    std::vector<NodePath> dragPaths;
    std::vector<BookmarkNode> dragSnapshot;
    bool dropValid = false;
    NodePath dropFolder;
    int dropIndex = -1;
    int dropHiliteItem = -1;

    bool dark = false;
    UiColors colors{};
    HBRUSH hBg = nullptr;
    HBRUSH hInput = nullptr;

    // Lifetime: the window can be destroyed while one of its handlers is still
    // running (e.g. inside a menu or dialog loop). The data is freed only after
    // the outermost handler has returned.
    int callDepth = 0;
    bool destroyed = false;
};

void MgrFreeData(ManagerWindowData* d) {
    if (d->hListImages) ImageList_Destroy(d->hListImages);
    if (d->hTreeImages) ImageList_Destroy(d->hTreeImages);
    if (d->hFontTitle) DeleteObject(d->hFontTitle);
    if (d->hFontUI) DeleteObject(d->hFontUI);
    if (d->hFontIcon) DeleteObject(d->hFontIcon);
    if (d->hFontSmall) DeleteObject(d->hFontSmall);
    if (d->hBg) DeleteObject(d->hBg);
    if (d->hInput) DeleteObject(d->hInput);
    delete d;
}

constexpr int kMgrHeaderH = 60;
constexpr int kMgrTreeW = 270;
constexpr int kMgrMargin = 12;
constexpr int kMgrFooterH = 22;  // bottom strip with the mod version

// Scales a size given at 96 DPI to the Bookmark Manager's current DPI.
int MgrS(const ManagerWindowData* d, int value) {
    return ScaleForDpi(value, d ? d->dpi : 96);
}

// True only while a manager button is being activated with the Enter key
thread_local bool g_mgrMenuByKeyboard = false;

constexpr int IDC_MGR_SEARCH = 2001;
constexpr int IDC_MGR_TREE = 2002;
constexpr int IDC_MGR_LIST = 2003;
constexpr int IDC_MGR_MORE = 2004;
constexpr int IDC_MGR_EMPTY = 2005;
constexpr int IDC_MGR_SEL_LABEL = 2006;
constexpr int IDC_MGR_SEL_DELETE = 2007;
constexpr int IDC_MGR_SEL_CLEAR = 2008;
constexpr int IDC_MGR_TITLE = 2009;

enum MgrCmd : int {
    CMD_OPEN_NEW_TAB = 3001,
    CMD_OPEN_NEW_WINDOW,
    CMD_OPEN_ALL,
    CMD_OPEN_ALL_WINDOW,
    CMD_EDIT,
    CMD_COPY_PATH,
    CMD_CUT,
    CMD_COPY,
    CMD_PASTE,
    CMD_DELETE,
    CMD_SHOW_IN_FOLDER,
    CMD_SORT,
    CMD_ADD_BOOKMARK,
    CMD_ADD_FOLDER,
    CMD_IMPORT,
    CMD_EXPORT,
};

std::mutex g_mgrNavMutex;
NodePath g_mgrNavPath;

HWND MgrFrame(ManagerWindowData* d) {
    if (d && IsTrackedFrame(d->hOwnerExplorer)) return d->hOwnerExplorer;
    return FindAnyTrackedFrame();
}

std::vector<NodePath> MgrSelectedPaths(ManagerWindowData* d) {
    std::vector<NodePath> refs;
    int sel = -1;
    while ((sel = ListView_GetNextItem(d->hList, sel, LVNI_SELECTED)) >= 0) {
        if (static_cast<size_t>(sel) < d->displayed.size()) refs.push_back(d->displayed[sel]);
    }
    return refs;
}

void MgrUpdateSelectionBar(ManagerWindowData* d) {
    int cnt = ListView_GetSelectedCount(d->hList);
    bool show = cnt >= 2;
    int normal = show ? SW_HIDE : SW_SHOW;
    int selMode = show ? SW_SHOW : SW_HIDE;
    ShowWindow(d->hTitle, normal);
    ShowWindow(d->hSearch, normal);
    ShowWindow(d->hMore, normal);
    ShowWindow(d->hSelLabel, selMode);
    ShowWindow(d->hSelDelete, selMode);
    ShowWindow(d->hSelClear, selMode);
    if (show) {
        std::wstring t = std::to_wstring(cnt) + L" selected";
        SetWindowTextW(d->hSelLabel, t.c_str());
    }
}

HTREEITEM MgrInsertTreeNode(ManagerWindowData* d, HTREEITEM parent, const std::wstring& text, const NodePath& path) {
    TVINSERTSTRUCTW tis{};
    tis.hParent = parent;
    tis.hInsertAfter = TVI_LAST;
    tis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
    tis.item.pszText = const_cast<LPWSTR>(text.c_str());
    tis.item.iImage = 0;
    tis.item.iSelectedImage = 0;
    tis.item.lParam = static_cast<LPARAM>(d->treePaths.size());
    d->treePaths.push_back(path);
    return TreeView_InsertItem(d->hTree, &tis);
}

void MgrAddTreeChildren(ManagerWindowData* d, HTREEITEM parent, const std::vector<BookmarkNode>& kids,
                        NodePath& cur, HTREEITEM& selItem) {
    if (cur.size() > static_cast<size_t>(kMaxDepth)) return;
    for (size_t i = 0; i < kids.size(); ++i) {
        if (kids[i].type != NodeType::FolderBox) continue;
        cur.push_back(static_cast<int>(i));
        HTREEITEM h = MgrInsertTreeNode(d, parent, kids[i].name, cur);
        if (cur == d->currentFolder) selItem = h;
        MgrAddTreeChildren(d, h, kids[i].children, cur, selItem);
        TreeView_Expand(d->hTree, h, TVE_EXPAND);
        cur.pop_back();
    }
}

void MgrRebuildTree(ManagerWindowData* d, const std::vector<BookmarkNode>& list) {
    d->suppressTreeSel = true;
    SendMessageW(d->hTree, WM_SETREDRAW, FALSE, 0);
    TreeView_DeleteAllItems(d->hTree);
    d->treePaths.clear();
    HTREEITEM root = MgrInsertTreeNode(d, TVI_ROOT, L"Bookmarks bar", {});
    HTREEITEM selItem = d->currentFolder.empty() ? root : nullptr;
    NodePath cur;
    MgrAddTreeChildren(d, root, list, cur, selItem);
    TreeView_Expand(d->hTree, root, TVE_EXPAND);
    TreeView_SelectItem(d->hTree, selItem ? selItem : root);
    SendMessageW(d->hTree, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(d->hTree, nullptr, TRUE);
    d->suppressTreeSel = false;
}

void MgrRebuildList(ManagerWindowData* d, const std::vector<BookmarkNode>& list) {
    SendMessageW(d->hList, WM_SETREDRAW, FALSE, 0);
    ListView_DeleteAllItems(d->hList);
    if (d->hListImages) ImageList_RemoveAll(d->hListImages);
    d->displayed.clear();

    wchar_t searchBuf[512]{};
    GetWindowTextW(d->hSearch, searchBuf, ARRAYSIZE(searchBuf));
    std::wstring query = searchBuf;
    d->searching = !query.empty();

    auto addRow = [&](const BookmarkNode& n, const NodePath& p) {
        bool isFolder = n.type == NodeType::FolderBox;
        int icon = AddShellIconToImageList(d->hListImages, n.path, isFolder, MgrS(d, 20));
        LVITEMW lvi{};
        lvi.mask = LVIF_TEXT | LVIF_IMAGE;
        lvi.iItem = static_cast<int>(d->displayed.size());
        lvi.iImage = icon;
        lvi.pszText = const_cast<LPWSTR>(n.name.c_str());
        int row = ListView_InsertItem(d->hList, &lvi);
        std::wstring second = isFolder ? L"" : n.path;
        ListView_SetItemText(d->hList, row, 1, const_cast<LPWSTR>(second.c_str()));
        ListView_SetItemText(d->hList, row, 2, const_cast<LPWSTR>(L"\u22EE"));
        d->displayed.push_back(p);
    };

    if (d->searching) {
        std::function<void(const std::vector<BookmarkNode>&, NodePath&)> walk =
            [&](const std::vector<BookmarkNode>& kids, NodePath& cur) {
                if (cur.size() > static_cast<size_t>(kMaxDepth)) return;
                for (size_t i = 0; i < kids.size(); ++i) {
                    cur.push_back(static_cast<int>(i));
                    const auto& n = kids[i];
                    if (ContainsIgnoreCase(n.name, query) ||
                        (n.type == NodeType::Item && ContainsIgnoreCase(n.path, query))) {
                        addRow(n, cur);
                    }
                    if (n.type == NodeType::FolderBox) walk(n.children, cur);
                    cur.pop_back();
                }
            };
        NodePath cur;
        walk(list, cur);
    } else if (const auto* kids = FolderChildren(list, d->currentFolder)) {
        for (size_t i = 0; i < kids->size(); ++i) {
            addRow((*kids)[i], ChildOf(d->currentFolder, static_cast<int>(i)));
        }
    }

    bool empty = d->displayed.empty();
    SetWindowTextW(d->hEmpty, d->searching ? L"No search results found" : L"No bookmarks");
    ShowWindow(d->hEmpty, empty ? SW_SHOW : SW_HIDE);
    if (empty) SetWindowPos(d->hEmpty, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    SendMessageW(d->hList, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(d->hList, nullptr, TRUE);
}

void MgrSelectPaths(ManagerWindowData* d, const std::vector<NodePath>& paths) {
    ListView_SetItemState(d->hList, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
    bool first = true;
    for (size_t i = 0; i < d->displayed.size(); ++i) {
        if (std::find(paths.begin(), paths.end(), d->displayed[i]) != paths.end()) {
            UINT state = LVIS_SELECTED | (first ? LVIS_FOCUSED : 0);
            ListView_SetItemState(d->hList, static_cast<int>(i), state, LVIS_SELECTED | LVIS_FOCUSED);
            if (first) ListView_EnsureVisible(d->hList, static_cast<int>(i), FALSE);
            first = false;
        }
    }
}

// selectPaths == nullptr -> keep current selection
void MgrRefresh(ManagerWindowData* d, const std::vector<NodePath>* selectPaths = nullptr) {
    std::vector<NodePath> keep = selectPaths ? *selectPaths : MgrSelectedPaths(d);
    auto list = LoadBookmarks();
    if (!FolderChildren(list, d->currentFolder)) d->currentFolder.clear();
    MgrRebuildTree(d, list);
    MgrRebuildList(d, list);
    MgrSelectPaths(d, keep);
    MgrUpdateSelectionBar(d);
}

void MgrClearSearch(ManagerWindowData* d) {
    d->ignoreSearchChange = true;
    SetWindowTextW(d->hSearch, L"");
    d->ignoreSearchChange = false;
}

void MgrNavigate(ManagerWindowData* d, const NodePath& folder, const std::vector<NodePath>& select = {}) {
    d->currentFolder = folder;
    MgrClearSearch(d);
    MgrRefresh(d, &select);
}

void MgrLayout(HWND hwnd, ManagerWindowData* d) {
    RECT rc{};
    GetClientRect(hwnd, &rc);
    const int cw = rc.right;
    const int ch = rc.bottom;
    auto S = [d](int v) { return MgrS(d, v); };
    const int headerH = S(kMgrHeaderH);

    MoveWindow(d->hTitle, S(24), S(14), S(180), S(32), TRUE);
    int searchW = std::max(S(160), std::min(S(560), cw - S(220) - S(80)));
    int searchX = std::max(S(220), (cw - searchW) / 2);
    MoveWindow(d->hSearch, searchX, S(15), searchW, S(30), TRUE);
    MoveWindow(d->hMore, cw - S(60), S(13), S(40), S(34), TRUE);
    MoveWindow(d->hSelLabel, S(24), S(18), S(220), S(26), TRUE);
    MoveWindow(d->hSelDelete, cw - S(160), S(14), S(90), S(32), TRUE);
    MoveWindow(d->hSelClear, cw - S(60), S(13), S(40), S(34), TRUE);

    const int margin = S(kMgrMargin);
    const int top = headerH + S(8);
    const int treeW = S(kMgrTreeW);
    const int h = std::max(S(50), ch - top - margin - S(kMgrFooterH));
    MoveWindow(d->hTree, margin, top, treeW, h, TRUE);
    const int lx = margin + treeW + margin;
    const int lw = std::max(S(120), cw - lx - margin);
    MoveWindow(d->hList, lx, top, lw, h, TRUE);
    MoveWindow(d->hEmpty, lx, top + S(60), lw, S(30), TRUE);

    RECT lr{};
    GetClientRect(d->hList, &lr);
    int w = lr.right;
    int c2 = S(36);
    int c0 = std::max(S(120), (w - c2) * 45 / 100);
    int c1 = std::max(S(60), w - c2 - c0);
    ListView_SetColumnWidth(d->hList, 0, c0);
    ListView_SetColumnWidth(d->hList, 1, c1);
    ListView_SetColumnWidth(d->hList, 2, c2);
}

// Fonts and image lists depend on the window's DPI; they are created again
// when the window moves to a monitor with a different scale.
void MgrCreateFonts(ManagerWindowData* d) {
    for (HFONT* f : {&d->hFontTitle, &d->hFontUI, &d->hFontIcon, &d->hFontSmall}) {
        if (*f) DeleteObject(*f);
        *f = nullptr;
    }
    d->hFontTitle = CreateUiFont(22, FW_SEMIBOLD, d->dpi);
    d->hFontUI = CreateUiFont(14, FW_NORMAL, d->dpi);
    d->hFontIcon = CreateUiFont(20, FW_BOLD, d->dpi);
    d->hFontSmall = CreateUiFont(12, FW_NORMAL, d->dpi);
}

void MgrCreateTreeImages(ManagerWindowData* d) {
    TreeView_SetItemHeight(d->hTree, MgrS(d, 28));
    const int size = MgrS(d, 16);
    HIMAGELIST images = ImageList_Create(size, size, ILC_COLOR32 | ILC_MASK, 1, 1);
    AddShellIconToImageList(images, L"", true, size);
    TreeView_SetImageList(d->hTree, images, TVSIL_NORMAL);
    if (d->hTreeImages) ImageList_Destroy(d->hTreeImages);
    d->hTreeImages = images;
}

void MgrCreateListImages(ManagerWindowData* d) {
    const int size = MgrS(d, 20);
    HIMAGELIST images = ImageList_Create(size, size, ILC_COLOR32 | ILC_MASK, 32, 32);
    ListView_SetImageList(d->hList, images, LVSIL_SMALL);
    if (d->hListImages) ImageList_Destroy(d->hListImages);
    d->hListImages = images;
}

void MgrApplyTheme(HWND hwnd, ManagerWindowData* d) {
    d->dark = IsDarkThemeActive();
    d->colors = ThemeColors(d->dark);
    if (d->hBg) DeleteObject(d->hBg);
    if (d->hInput) DeleteObject(d->hInput);
    d->hBg = CreateSolidBrush(d->colors.bg);
    d->hInput = CreateSolidBrush(d->colors.inputBg);

    ApplyTitleBarTheme(hwnd, d->dark);
    const wchar_t* listTheme = d->dark ? L"DarkMode_Explorer" : L"Explorer";
    SetWindowTheme(d->hTree, listTheme, nullptr);
    SetWindowTheme(d->hList, listTheme, nullptr);
    TreeView_SetBkColor(d->hTree, d->colors.bg);
    TreeView_SetTextColor(d->hTree, d->colors.text);
    ListView_SetBkColor(d->hList, d->colors.bg);
    ListView_SetTextBkColor(d->hList, d->colors.bg);
    ListView_SetTextColor(d->hList, d->colors.text);
    for (HWND b : {d->hMore, d->hSelDelete, d->hSelClear}) {
        SetWindowTheme(b, listTheme, nullptr);
    }
    SetWindowTheme(d->hSearch, d->dark ? L"DarkMode_CFD" : nullptr, nullptr);
    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
}

int MgrShowContextMenu(HWND hwnd, ManagerWindowData* d, const std::vector<NodePath>& targets, POINT screen,
                       bool fromTree) {
    auto list = LoadBookmarks();
    bool canPaste = ClipboardHasData();
    HMENU m = CreatePopupMenu();
    if (!m) return 0;
    auto add = [&](int id, const std::wstring& text, bool enabled = true) {
        AppendMenuW(m, MF_STRING | (enabled ? 0 : MF_GRAYED), id, text.c_str());
    };
    auto sep = [&]() { AppendMenuW(m, MF_SEPARATOR, 0, nullptr); };

    if (targets.empty()) {
        add(CMD_SORT, L"Sort by name");
        add(CMD_ADD_BOOKMARK, L"Add new bookmark");
        add(CMD_ADD_FOLDER, L"Add new folder");
        sep();
        add(CMD_PASTE, L"Paste\tCtrl+V", canPaste);
    } else if (targets.size() == 1 && targets[0].empty()) {
        // Root "Bookmarks bar" in the tree
        size_t n = CollectOpenPaths(list, targets).size();
        add(CMD_OPEN_ALL, L"Open all (" + std::to_wstring(n) + L")", n > 0);
        add(CMD_OPEN_ALL_WINDOW, L"Open all in new window", n > 0);
        sep();
        add(CMD_PASTE, L"Paste\tCtrl+V", canPaste);
        sep();
        add(CMD_SORT, L"Sort by name");
        add(CMD_ADD_BOOKMARK, L"Add new bookmark");
        add(CMD_ADD_FOLDER, L"Add new folder");
    } else if (targets.size() == 1) {
        const BookmarkNode* node = NodeAt(list, targets[0]);
        if (!node) {
            DestroyMenu(m);
            return 0;
        }
        if (node->type == NodeType::FolderBox) {
            size_t n = CollectOpenPaths(list, targets).size();
            add(CMD_OPEN_ALL, L"Open all (" + std::to_wstring(n) + L")", n > 0);
            add(CMD_OPEN_ALL_WINDOW, L"Open all in new window", n > 0);
            sep();
            add(CMD_EDIT, L"Rename...\tF2");
            if (fromTree) {
                add(CMD_SORT, L"Sort by name");
                add(CMD_ADD_BOOKMARK, L"Add new bookmark");
                add(CMD_ADD_FOLDER, L"Add new folder");
            }
        } else {
            add(CMD_OPEN_NEW_TAB, L"Open in new tab");
            add(CMD_OPEN_NEW_WINDOW, L"Open in new window");
            sep();
            add(CMD_EDIT, L"Edit...\tF2");
            add(CMD_COPY_PATH, L"Copy path");
        }
        sep();
        add(CMD_CUT, L"Cut\tCtrl+X");
        add(CMD_COPY, L"Copy\tCtrl+C");
        add(CMD_PASTE, L"Paste\tCtrl+V", canPaste);
        sep();
        add(CMD_DELETE, L"Delete\tDelete");
        if (d->searching && !fromTree) {
            sep();
            add(CMD_SHOW_IN_FOLDER, L"Show in folder");
        }
    } else {
        size_t n = CollectOpenPaths(list, targets).size();
        add(CMD_OPEN_ALL, L"Open all (" + std::to_wstring(n) + L")", n > 0);
        add(CMD_OPEN_ALL_WINDOW, L"Open all in new window", n > 0);
        sep();
        add(CMD_COPY_PATH, L"Copy paths");
        sep();
        add(CMD_CUT, L"Cut\tCtrl+X");
        add(CMD_COPY, L"Copy\tCtrl+C");
        add(CMD_PASTE, L"Paste\tCtrl+V", canPaste);
        sep();
        add(CMD_DELETE, L"Delete\tDelete");
    }

    int cmd = ModTrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN,
                                screen.x, screen.y, hwnd);
    DestroyMenu(m);
    return cmd;
}

// After items are removed, keep the manager on the same folder: indices of later
// siblings shift down, and a removed folder (or its parent chain) falls back to its parent.
void MgrAdjustCurrentFolderAfterRemoval(ManagerWindowData* d, const std::vector<NodePath>& removed) {
    auto sel = NormalizeSelection(removed);
    for (auto it = sel.rbegin(); it != sel.rend(); ++it) {
        if (IsPrefixOf(*it, d->currentFolder)) {
            d->currentFolder = ParentOf(*it);
            continue;
        }
        int unusedIndex = 0;
        AdjustForRemoval(d->currentFolder, unusedIndex, *it);
    }
}

void MgrExecute(HWND hwnd, ManagerWindowData* d, int cmd, const std::vector<NodePath>& targets, bool fromTree) {
    auto list = LoadBookmarks();
    NodePath folderTarget = d->currentFolder;
    if (fromTree && targets.size() == 1) folderTarget = targets[0];

    switch (cmd) {
        case CMD_OPEN_NEW_TAB:
        case CMD_OPEN_NEW_WINDOW: {
            auto paths = CollectOpenPaths(list, targets);
            QueueOpenTargets(MgrFrame(d), paths,
                             cmd == CMD_OPEN_NEW_TAB ? OpenDisposition::NewTab : OpenDisposition::NewWindow);
            break;
        }
        case CMD_OPEN_ALL:
        case CMD_OPEN_ALL_WINDOW: {
            auto paths = CollectOpenPaths(list, targets);
            if (cmd == CMD_OPEN_ALL_WINDOW && !paths.empty()) {
                // First one in a new window, the rest follow it
                QueueOpenTargets(MgrFrame(d), {paths.front()}, OpenDisposition::NewWindow);
                paths.erase(paths.begin());
                QueueOpenTargets(MgrFrame(d), paths, OpenDisposition::NewWindow);
            } else {
                QueueOpenTargets(MgrFrame(d), paths, OpenDisposition::NewTab);
            }
            break;
        }
        case CMD_EDIT:
            if (targets.size() == 1 && !targets[0].empty()) {
                EditNodeFlow(hwnd, targets[0]);
                MgrRefresh(d);
            }
            break;
        case CMD_COPY_PATH: {
            std::wstring text;
            for (const auto& t : targets) {
                if (const BookmarkNode* n = NodeAt(list, t)) {
                    if (n->type != NodeType::Item) continue;
                    if (!text.empty()) text += L"\r\n";
                    text += n->path;
                }
            }
            if (!text.empty()) CopyTextToClipboard(hwnd, text);
            break;
        }
        case CMD_CUT:
        case CMD_COPY:
            DoCutOrCopy(targets, cmd == CMD_CUT);
            if (cmd == CMD_CUT) {
                MgrAdjustCurrentFolderAfterRemoval(d, targets);
                std::vector<NodePath> none;
                MgrRefresh(d, &none);
            }
            break;
        case CMD_PASTE: {
            NodePath folder;
            int index = -1;
            if (fromTree && targets.size() == 1) {
                folder = targets[0];
            } else {
                PasteTargetFor(list, targets, d->currentFolder, folder, index);
            }
            DoPaste(folder, index, hwnd);
            MgrRefresh(d);
            break;
        }
        case CMD_DELETE: {
            DoDelete(targets);
            MgrAdjustCurrentFolderAfterRemoval(d, targets);
            std::vector<NodePath> none;
            MgrRefresh(d, &none);
            break;
        }
        case CMD_SHOW_IN_FOLDER:
            if (targets.size() == 1 && !targets[0].empty()) {
                MgrNavigate(d, ParentOf(targets[0]), {targets[0]});
            }
            break;
        case CMD_SORT:
            SortFolderByName(folderTarget);
            MgrRefresh(d);
            break;
        case CMD_ADD_BOOKMARK:
            AddBookmarkFlow(hwnd, folderTarget, -1);
            MgrRefresh(d);
            break;
        case CMD_ADD_FOLDER:
            AddFolderFlow(hwnd, folderTarget, -1);
            MgrRefresh(d);
            break;
        case CMD_IMPORT:
            ImportBackupFile(hwnd);
            d->currentFolder.clear();
            MgrRefresh(d);
            break;
        case CMD_EXPORT:
            ExportBackupFile(hwnd);
            break;
    }
}

void MgrActivateRows(ManagerWindowData* d, const std::vector<NodePath>& rows) {
    if (rows.empty()) return;
    auto list = LoadBookmarks();
    if (rows.size() == 1) {
        const BookmarkNode* n = NodeAt(list, rows[0]);
        if (!n) return;
        if (n->type == NodeType::FolderBox) {
            MgrNavigate(d, rows[0]);
        } else {
            QueueOpenTargets(MgrFrame(d), {n->path}, OpenDisposition::CurrentTab);
        }
        return;
    }
    QueueOpenTargets(MgrFrame(d), CollectOpenPaths(list, rows), OpenDisposition::NewTab);
}

// --- Drag & drop inside the manager ---------------------------------------

void MgrClearDropVisuals(ManagerWindowData* d) {
    if (d->dropHiliteItem >= 0) {
        ListView_SetItemState(d->hList, d->dropHiliteItem, 0, LVIS_DROPHILITED);
        d->dropHiliteItem = -1;
    }
    TreeView_SelectDropTarget(d->hTree, nullptr);
    if (d->hDropLine) ShowWindow(d->hDropLine, SW_HIDE);
}

void MgrShowDropLine(ManagerWindowData* d, int y) {
    if (!d->hDropLine) return;
    RECT lr{};
    GetClientRect(d->hList, &lr);
    SetWindowPos(d->hDropLine, HWND_TOP, 0, std::max(0, y - 1), lr.right, 2,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
}

void MgrBeginDrag(HWND hwnd, ManagerWindowData* d, const std::vector<NodePath>& paths) {
    auto norm = NormalizeSelection(paths);
    if (norm.empty()) return;
    d->dragging = true;
    d->dragPaths = norm;
    d->dragSnapshot = LoadBookmarks();
    d->dropValid = false;
    SetCapture(hwnd);
}

void MgrUpdateDrop(ManagerWindowData* d, POINT screenPt) {
    MgrClearDropVisuals(d);
    d->dropValid = false;
    const auto& list = d->dragSnapshot;

    RECT rl{}, rt{};
    GetWindowRect(d->hList, &rl);
    GetWindowRect(d->hTree, &rt);

    if (PtInRect(&rl, screenPt)) {
        POINT pt = screenPt;
        ScreenToClient(d->hList, &pt);
        LVHITTESTINFO ht{};
        ht.pt = pt;
        int i = ListView_HitTest(d->hList, &ht);
        if (i >= 0 && static_cast<size_t>(i) < d->displayed.size()) {
            RECT ir{};
            ListView_GetItemRect(d->hList, i, &ir, LVIR_BOUNDS);
            int h = std::max(1, static_cast<int>(ir.bottom - ir.top));
            int y = pt.y - ir.top;
            const NodePath& target = d->displayed[i];
            const BookmarkNode* n = NodeAt(list, target);
            bool inDrag = std::find(d->dragPaths.begin(), d->dragPaths.end(), target) != d->dragPaths.end();
            if (n && n->type == NodeType::FolderBox && !inDrag && y >= h / 4 && y <= h * 3 / 4) {
                d->dropFolder = target;
                d->dropIndex = -1;
                d->dropValid = true;
                ListView_SetItemState(d->hList, i, LVIS_DROPHILITED, LVIS_DROPHILITED);
                d->dropHiliteItem = i;
            } else if (!d->searching) {
                bool after = y >= h / 2;
                d->dropFolder = d->currentFolder;
                d->dropIndex = target.back() + (after ? 1 : 0);
                d->dropValid = true;
                MgrShowDropLine(d, after ? ir.bottom : ir.top);
            }
        } else if (!d->searching) {
            const auto* kids = FolderChildren(list, d->currentFolder);
            d->dropFolder = d->currentFolder;
            d->dropIndex = kids ? static_cast<int>(kids->size()) : -1;
            d->dropValid = true;
            int count = ListView_GetItemCount(d->hList);
            int lineY = 0;
            if (count > 0) {
                RECT ir{};
                ListView_GetItemRect(d->hList, count - 1, &ir, LVIR_BOUNDS);
                lineY = ir.bottom;
            }
            MgrShowDropLine(d, lineY);
        }
    } else if (PtInRect(&rt, screenPt)) {
        TVHITTESTINFO ht{};
        ht.pt = screenPt;
        ScreenToClient(d->hTree, &ht.pt);
        HTREEITEM item = TreeView_HitTest(d->hTree, &ht);
        if (item && (ht.flags & TVHT_ONITEM)) {
            TVITEMW tvi{};
            tvi.mask = TVIF_PARAM;
            tvi.hItem = item;
            if (TreeView_GetItem(d->hTree, &tvi) && static_cast<size_t>(tvi.lParam) < d->treePaths.size()) {
                d->dropFolder = d->treePaths[tvi.lParam];
                d->dropIndex = -1;
                d->dropValid = true;
                TreeView_SelectDropTarget(d->hTree, item);
            }
        }
    }

    if (d->dropValid) {
        for (const auto& s : d->dragPaths) {
            if (IsPrefixOf(s, d->dropFolder)) {
                d->dropValid = false;
                MgrClearDropVisuals(d);
                break;
            }
        }
    }
    SetCursor(LoadCursorW(nullptr, d->dropValid ? IDC_ARROW : IDC_NO));
}

void MgrEndDrag(HWND hwnd, ManagerWindowData* d, bool drop) {
    if (!d->dragging) return;
    d->dragging = false;
    if (GetCapture() == hwnd) ReleaseCapture();
    MgrClearDropVisuals(d);
    SetCursor(LoadCursorW(nullptr, IDC_ARROW));
    if (drop && d->dropValid) {
        auto list = LoadBookmarks();
        if (MoveNodes(list, d->dragPaths, d->dropFolder, d->dropIndex)) {
            SaveBookmarks(list);
        }
        std::vector<NodePath> none;
        MgrRefresh(d, &none);
    }
    d->dragPaths.clear();
    d->dragSnapshot.clear();
    d->dropValid = false;
}

// Shared keyboard handling for list + tree
bool MgrHandleKey(HWND hwnd, ManagerWindowData* d, WORD vk, const std::vector<NodePath>& targets, bool fromTree) {
    const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    if (vk == VK_ESCAPE && d->dragging) {
        MgrEndDrag(hwnd, d, false);
        return true;
    }
    if (ctrl && vk == 'Z') {
        UndoLastChange();
        MgrRefresh(d);
        return true;
    }
    if (ctrl && (vk == 'X' || vk == 'C')) {
        if (!targets.empty()) MgrExecute(hwnd, d, vk == 'X' ? CMD_CUT : CMD_COPY, targets, fromTree);
        return true;
    }
    if (ctrl && vk == 'V') {
        MgrExecute(hwnd, d, CMD_PASTE, targets, fromTree);
        return true;
    }
    if (vk == VK_DELETE) {
        if (!targets.empty()) MgrExecute(hwnd, d, CMD_DELETE, targets, fromTree);
        return true;
    }
    if (vk == VK_F2) {
        if (targets.size() == 1) MgrExecute(hwnd, d, CMD_EDIT, targets, fromTree);
        return true;
    }
    return false;
}

static LRESULT SearchEditSubclassProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                          UINT_PTR idSubclass, DWORD_PTR refData) {
    if (msg == WM_PAINT) {
        // Placeholder styled like Explorer's "Search This PC". The system cue banner
        // color can't be changed, so the hint is drawn manually while the box is empty.
        LRESULT r = DefSubclassProc(hwnd, msg, wParam, lParam);
        auto* md = reinterpret_cast<ManagerWindowData*>(refData);
        if (md && GetFocus() != hwnd && GetWindowTextLengthW(hwnd) == 0) {
            if (HDC hdc = GetDC(hwnd)) {
                RECT rc{};
                SendMessageW(hwnd, EM_GETRECT, 0, reinterpret_cast<LPARAM>(&rc));
                RECT cr{};
                GetClientRect(hwnd, &cr);
                IntersectClipRect(hdc, cr.left, cr.top, cr.right, cr.bottom);
                HFONT old = reinterpret_cast<HFONT>(SelectObject(hdc, md->hFontUI));
                SetBkMode(hdc, TRANSPARENT);
                // WinUI TextFillColorSecondary: light ~ #616161, dark ~ #CECECE
                SetTextColor(hdc, md->dark ? RGB(206, 206, 206) : RGB(97, 97, 97));
                DrawTextW(hdc, L"Search bookmarks", -1, &rc,
                          DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
                SelectObject(hdc, old);
                ReleaseDC(hwnd, hdc);
            }
        }
        return r;
    }
    if (msg == WM_SETFOCUS || msg == WM_KILLFOCUS) {
        // Hide the placeholder while focused, show it again on focus loss.
        LRESULT r = DefSubclassProc(hwnd, msg, wParam, lParam);
        InvalidateRect(hwnd, nullptr, TRUE);
        return r;
    }
    if (msg == WM_KEYDOWN && wParam == 'A' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        SendMessageW(hwnd, EM_SETSEL, 0, -1);
        return 0;
    }
    if (msg == WM_CHAR && wParam == 1) return 0;
    if (msg == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, SearchEditSubclassProc, idSubclass);
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK SearchEditSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                        UINT_PTR idSubclass, DWORD_PTR refData) {
    try {
        return SearchEditSubclassProcImpl(hwnd, msg, wParam, lParam, idSubclass, refData);
    } catch (...) {
        Wh_Log(L"SearchEditSubclassProc: exception (msg %u)", msg);
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }
}

LRESULT CALLBACK DropLineWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT BookmarkManagerWndProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* d = reinterpret_cast<ManagerWindowData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_CREATE: {
            auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            d = reinterpret_cast<ManagerWindowData*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(d));

            d->dpi = WindowDpi(hwnd);
            MgrCreateFonts(d);

            d->hTitle = MakeChild(hwnd, L"STATIC", L"Bookmarks", 0, 0, 0, 10, 10, IDC_MGR_TITLE, d->hFontTitle);
            d->hSearch = MakeChild(hwnd, L"EDIT", L"", WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 10, 10,
                                   IDC_MGR_SEARCH, d->hFontUI, WS_EX_CLIENTEDGE);
            SetWindowSubclass(d->hSearch, SearchEditSubclassProc, 1, reinterpret_cast<DWORD_PTR>(d));
            d->hMore = MakeChild(hwnd, L"BUTTON", L"\u22EE", WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 10, 10,
                                 IDC_MGR_MORE, d->hFontIcon);

            d->hSelLabel = MakeChild(hwnd, L"STATIC", L"", 0, 0, 0, 10, 10, IDC_MGR_SEL_LABEL, d->hFontTitle);
            d->hSelDelete = MakeChild(hwnd, L"BUTTON", L"Delete", WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 10, 10,
                                      IDC_MGR_SEL_DELETE, d->hFontUI);
            d->hSelClear = MakeChild(hwnd, L"BUTTON", L"\u2715", WS_TABSTOP | BS_PUSHBUTTON, 0, 0, 10, 10,
                                     IDC_MGR_SEL_CLEAR, d->hFontUI);
            ShowWindow(d->hSelLabel, SW_HIDE);
            ShowWindow(d->hSelDelete, SW_HIDE);
            ShowWindow(d->hSelClear, SW_HIDE);

            d->hTree = MakeChild(hwnd, WC_TREEVIEWW, nullptr,
                                 WS_TABSTOP | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT,
                                 0, 0, 10, 10, IDC_MGR_TREE, d->hFontUI);
            SetWindowTheme(d->hTree, L"Explorer", nullptr);
            TreeView_SetExtendedStyle(d->hTree, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);
            MgrCreateTreeImages(d);

            d->hList = MakeChild(hwnd, WC_LISTVIEWW, nullptr,
                                 WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | LVS_REPORT | LVS_SHOWSELALWAYS |
                                     LVS_NOCOLUMNHEADER | LVS_SHAREIMAGELISTS,
                                 0, 0, 10, 10, IDC_MGR_LIST, d->hFontUI);
            SetWindowTheme(d->hList, L"Explorer", nullptr);
            ListView_SetExtendedListViewStyle(d->hList, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
            MgrCreateListImages(d);

            LVCOLUMNW col{};
            col.mask = LVCF_TEXT | LVCF_WIDTH;
            col.cx = 250;
            col.pszText = const_cast<LPWSTR>(L"Name");
            ListView_InsertColumn(d->hList, 0, &col);
            col.cx = 300;
            col.pszText = const_cast<LPWSTR>(L"Path");
            ListView_InsertColumn(d->hList, 1, &col);
            col.cx = 36;
            col.pszText = const_cast<LPWSTR>(L"");
            ListView_InsertColumn(d->hList, 2, &col);

            d->hEmpty = MakeChild(hwnd, L"STATIC", L"No bookmarks", SS_CENTER, 0, 0, 10, 10,
                                  IDC_MGR_EMPTY, d->hFontUI);

            d->hDropLine = CreateWindowExW(0, kDropLineClass, nullptr, WS_CHILD, 0, 0, 10, 2,
                                           d->hList, nullptr, ModuleInstance(), nullptr);

            MgrApplyTheme(hwnd, d);
            MgrLayout(hwnd, d);
            MgrRefresh(d);
            SetFocus(d->hList);
            return 0;
        }

        case WM_ERASEBKGND:
            if (d && d->hBg) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                FillRect(reinterpret_cast<HDC>(wParam), &rc, d->hBg);
                return 1;
            }
            break;

        case WM_SETTINGCHANGE:
            if (d && lParam && g_themeSetting == 0 &&
                wcscmp(reinterpret_cast<LPCWSTR>(lParam), L"ImmersiveColorSet") == 0) {
                MgrApplyTheme(hwnd, d);
            }
            break;

        case kMgrThemeMsg:
            if (d) MgrApplyTheme(hwnd, d);
            return 0;

        case WM_SIZE:
            if (d) {
                MgrLayout(hwnd, d);
                InvalidateRect(hwnd, nullptr, TRUE);  // the divider and version text move with the size
            }
            return 0;

        case WM_DPICHANGED:
            if (d) {
                d->dpi = HIWORD(wParam);
                MgrCreateFonts(d);
                const std::pair<HWND, HFONT> fonts[] = {
                    {d->hTitle, d->hFontTitle},  {d->hSelLabel, d->hFontTitle}, {d->hSearch, d->hFontUI},
                    {d->hMore, d->hFontIcon},    {d->hSelDelete, d->hFontUI},   {d->hSelClear, d->hFontUI},
                    {d->hTree, d->hFontUI},      {d->hList, d->hFontUI},        {d->hEmpty, d->hFontUI},
                };
                for (const auto& [ctl, font] : fonts) {
                    if (ctl) SendMessageW(ctl, WM_SETFONT, reinterpret_cast<WPARAM>(font), FALSE);
                }
                MgrCreateTreeImages(d);
                MgrCreateListImages(d);
                auto* suggested = reinterpret_cast<RECT*>(lParam);
                SetWindowPos(hwnd, nullptr, suggested->left, suggested->top, suggested->right - suggested->left,
                             suggested->bottom - suggested->top, SWP_NOZORDER | SWP_NOACTIVATE);
                MgrLayout(hwnd, d);
                MgrRefresh(d);
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;

        case WM_GETMINMAXINFO: {
            auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
            const UINT dpi = d ? d->dpi : WindowDpi(hwnd);
            mmi->ptMinTrackSize.x = ScaleForDpi(720, dpi);
            mmi->ptMinTrackSize.y = ScaleForDpi(420, dpi);
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            COLORREF headerColor = d ? d->colors.headerLine : RGB(218, 220, 224);
            COLORREF dividerColor = d ? d->colors.divider : RGB(0, 0, 0);
            const int headerH = MgrS(d, kMgrHeaderH);
            const int margin = MgrS(d, kMgrMargin);
            const int footerH = MgrS(d, kMgrFooterH);
            RECT line{0, headerH, rc.right, headerH + 1};
            HBRUSH br = CreateSolidBrush(headerColor);
            FillRect(hdc, &line, br);
            DeleteObject(br);
            // Separator line between the folder tree (left) and the list (right)
            int x = margin + MgrS(d, kMgrTreeW) + margin / 2;
            RECT divider{x, headerH + MgrS(d, 8), x + 1, rc.bottom - margin - footerH};
            HBRUSH br2 = CreateSolidBrush(dividerColor);
            FillRect(hdc, &divider, br2);
            DeleteObject(br2);
            // Mod version, bottom-right corner
            if (d) {
                std::wstring ver = std::wstring(L"Version ") + kModVersion;
                RECT vr{margin, rc.bottom - footerH - MgrS(d, 4), rc.right - MgrS(d, 16), rc.bottom - MgrS(d, 4)};
                HFONT oldFont = reinterpret_cast<HFONT>(SelectObject(hdc, d->hFontSmall ? d->hFontSmall : d->hFontUI));
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, d->colors.subText);
                DrawTextW(hdc, ver.c_str(), -1, &vr, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                SelectObject(hdc, oldFont);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = reinterpret_cast<HDC>(wParam);
            SetBkMode(hdc, TRANSPARENT);
            if (d && d->hBg) {
                SetTextColor(hdc, reinterpret_cast<HWND>(lParam) == d->hEmpty ? d->colors.subText : d->colors.text);
                SetBkColor(hdc, d->colors.bg);
                return reinterpret_cast<LRESULT>(d->hBg);
            }
            return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
        }

        case WM_CTLCOLOREDIT:
            if (d && d->hInput) {
                HDC hdc = reinterpret_cast<HDC>(wParam);
                SetTextColor(hdc, d->colors.text);
                SetBkColor(hdc, d->colors.inputBg);
                return reinterpret_cast<LRESULT>(d->hInput);
            }
            break;

        case WM_SETFOCUS:
            if (d) SetFocus(d->hList);
            return 0;

        case WM_NOTIFY: {
            if (!d) break;
            auto* nm = reinterpret_cast<LPNMHDR>(lParam);
            if (!nm) break;

            if (nm->idFrom == IDC_MGR_LIST) {
                switch (nm->code) {
                    case NM_CUSTOMDRAW: {
                        auto* cd = reinterpret_cast<NMLVCUSTOMDRAW*>(lParam);
                        switch (cd->nmcd.dwDrawStage) {
                            case CDDS_PREPAINT:
                                return CDRF_NOTIFYITEMDRAW;
                            case CDDS_ITEMPREPAINT:
                                return CDRF_NOTIFYSUBITEMDRAW;
                            case CDDS_ITEMPREPAINT | CDDS_SUBITEM: {
                                int item = static_cast<int>(cd->nmcd.dwItemSpec);
                                bool sel = (ListView_GetItemState(d->hList, item, LVIS_SELECTED) & LVIS_SELECTED) != 0;
                                if (!sel) {
                                    cd->clrText = (cd->iSubItem >= 1) ? d->colors.subText : d->colors.text;
                                    cd->clrTextBk = d->colors.bg;
                                }
                                return CDRF_NEWFONT;
                            }
                        }
                        return CDRF_DODEFAULT;
                    }
                    case LVN_ITEMCHANGED: {
                        auto* pnmv = reinterpret_cast<NMLISTVIEW*>(lParam);
                        if ((pnmv->uChanged & LVIF_STATE) && ((pnmv->uOldState ^ pnmv->uNewState) & LVIS_SELECTED)) {
                            MgrUpdateSelectionBar(d);
                        }
                        return 0;
                    }
                    case LVN_BEGINDRAG:
                        MgrBeginDrag(hwnd, d, MgrSelectedPaths(d));
                        return 0;
                    case LVN_KEYDOWN: {
                        auto* kd = reinterpret_cast<NMLVKEYDOWN*>(lParam);
                        const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                        if (ctrl && kd->wVKey == 'A') {
                            ListView_SetItemState(d->hList, -1, LVIS_SELECTED, LVIS_SELECTED);
                            return 0;
                        }
                        if (kd->wVKey == VK_ESCAPE && !d->dragging) {
                            ListView_SetItemState(d->hList, -1, 0, LVIS_SELECTED);
                            return 0;
                        }
                        if (kd->wVKey == VK_BACK && !d->searching && !d->currentFolder.empty()) {
                            NodePath from = d->currentFolder;
                            MgrNavigate(d, ParentOf(from), {from});
                            return 0;
                        }
                        MgrHandleKey(hwnd, d, kd->wVKey, MgrSelectedPaths(d), false);
                        return 0;
                    }
                    case NM_RETURN:
                        MgrActivateRows(d, MgrSelectedPaths(d));
                        return 0;
                    case NM_DBLCLK: {
                        auto* ia = reinterpret_cast<NMITEMACTIVATE*>(lParam);
                        if (ia->iItem >= 0 && static_cast<size_t>(ia->iItem) < d->displayed.size()) {
                            MgrActivateRows(d, {d->displayed[ia->iItem]});
                        }
                        return 0;
                    }
                    case NM_CLICK: {
                        // Click on the ⋮ column opens the row menu (like Chrome)
                        auto* ia = reinterpret_cast<NMITEMACTIVATE*>(lParam);
                        LVHITTESTINFO ht{};
                        ht.pt = ia->ptAction;
                        ListView_SubItemHitTest(d->hList, &ht);
                        if (ht.iItem >= 0 && ht.iSubItem == 2 && static_cast<size_t>(ht.iItem) < d->displayed.size()) {
                            if (!(ListView_GetItemState(d->hList, ht.iItem, LVIS_SELECTED) & LVIS_SELECTED)) {
                                ListView_SetItemState(d->hList, -1, 0, LVIS_SELECTED);
                                ListView_SetItemState(d->hList, ht.iItem, LVIS_SELECTED | LVIS_FOCUSED,
                                                      LVIS_SELECTED | LVIS_FOCUSED);
                            }
                            RECT cell{};
                            ListView_GetSubItemRect(d->hList, ht.iItem, 2, LVIR_BOUNDS, &cell);
                            POINT pt{cell.left, cell.bottom};
                            ClientToScreen(d->hList, &pt);
                            auto targets = MgrSelectedPaths(d);
                            int cmd = MgrShowContextMenu(hwnd, d, targets, pt, false);
                            if (cmd) MgrExecute(hwnd, d, cmd, targets, false);
                        }
                        return 0;
                    }
                    case NM_RCLICK: {
                        auto* ia = reinterpret_cast<NMITEMACTIVATE*>(lParam);
                        if (ia->iItem >= 0) {
                            if (!(ListView_GetItemState(d->hList, ia->iItem, LVIS_SELECTED) & LVIS_SELECTED)) {
                                ListView_SetItemState(d->hList, -1, 0, LVIS_SELECTED);
                                ListView_SetItemState(d->hList, ia->iItem, LVIS_SELECTED | LVIS_FOCUSED,
                                                      LVIS_SELECTED | LVIS_FOCUSED);
                            }
                        } else {
                            ListView_SetItemState(d->hList, -1, 0, LVIS_SELECTED);
                        }
                        POINT pt{};
                        GetCursorPos(&pt);
                        auto targets = MgrSelectedPaths(d);
                        int cmd = MgrShowContextMenu(hwnd, d, targets, pt, false);
                        if (cmd) MgrExecute(hwnd, d, cmd, targets, false);
                        return 0;
                    }
                }
            } else if (nm->idFrom == IDC_MGR_TREE) {
                switch (nm->code) {
                    case TVN_SELCHANGEDW: {
                        if (d->suppressTreeSel) return 0;
                        auto* tv = reinterpret_cast<NMTREEVIEWW*>(lParam);
                        size_t idx = static_cast<size_t>(tv->itemNew.lParam);
                        if (idx < d->treePaths.size()) {
                            d->currentFolder = d->treePaths[idx];
                            MgrClearSearch(d);
                            MgrRebuildList(d, LoadBookmarks());
                            MgrUpdateSelectionBar(d);
                        }
                        return 0;
                    }
                    case TVN_BEGINDRAGW: {
                        auto* tv = reinterpret_cast<NMTREEVIEWW*>(lParam);
                        size_t idx = static_cast<size_t>(tv->itemNew.lParam);
                        if (idx < d->treePaths.size() && !d->treePaths[idx].empty()) {
                            MgrBeginDrag(hwnd, d, {d->treePaths[idx]});
                        }
                        return 0;
                    }
                    case TVN_KEYDOWN: {
                        auto* kd = reinterpret_cast<NMTVKEYDOWN*>(lParam);
                        std::vector<NodePath> targets;
                        if (HTREEITEM sel = TreeView_GetSelection(d->hTree)) {
                            TVITEMW tvi{};
                            tvi.mask = TVIF_PARAM;
                            tvi.hItem = sel;
                            if (TreeView_GetItem(d->hTree, &tvi) && static_cast<size_t>(tvi.lParam) < d->treePaths.size()) {
                                const NodePath& p = d->treePaths[tvi.lParam];
                                if (!p.empty()) targets.push_back(p);
                            }
                        }
                        const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                        if (ctrl && kd->wVKey == 'V' && targets.empty()) {
                            DoPaste({}, -1, hwnd);
                            MgrRefresh(d);
                            return 0;
                        }
                        MgrHandleKey(hwnd, d, kd->wVKey, targets, true);
                        return 0;
                    }
                    case NM_RCLICK: {
                        POINT screen{};
                        GetCursorPos(&screen);
                        TVHITTESTINFO ht{};
                        ht.pt = screen;
                        ScreenToClient(d->hTree, &ht.pt);
                        HTREEITEM item = TreeView_HitTest(d->hTree, &ht);
                        if (item && (ht.flags & TVHT_ONITEM)) {
                            TVITEMW tvi{};
                            tvi.mask = TVIF_PARAM;
                            tvi.hItem = item;
                            if (TreeView_GetItem(d->hTree, &tvi) && static_cast<size_t>(tvi.lParam) < d->treePaths.size()) {
                                std::vector<NodePath> targets{d->treePaths[tvi.lParam]};
                                TreeView_SelectDropTarget(d->hTree, item);
                                int cmd = MgrShowContextMenu(hwnd, d, targets, screen, true);
                                TreeView_SelectDropTarget(d->hTree, nullptr);
                                if (cmd) MgrExecute(hwnd, d, cmd, targets, true);
                            }
                        }
                        return 1;
                    }
                }
            }
            break;
        }

        case WM_CONTEXTMENU: {
            // Menu key / Shift+F10 sends x,y = -1. Mouse right-clicks are already
            // handled in NM_RCLICK, so only the keyboard case is handled here.
            if (!d || static_cast<short>(LOWORD(lParam)) != -1 || static_cast<short>(HIWORD(lParam)) != -1) break;
            HWND src = reinterpret_cast<HWND>(wParam);

            if (src == d->hTree) {
                HTREEITEM sel = TreeView_GetSelection(d->hTree);
                if (!sel) return 0;
                TVITEMW tvi{};
                tvi.mask = TVIF_PARAM;
                tvi.hItem = sel;
                if (!TreeView_GetItem(d->hTree, &tvi) || static_cast<size_t>(tvi.lParam) >= d->treePaths.size()) return 0;
                std::vector<NodePath> targets{d->treePaths[tvi.lParam]};
                RECT r{};
                TreeView_GetItemRect(d->hTree, sel, &r, TRUE);
                POINT pt{r.left, r.bottom};
                ClientToScreen(d->hTree, &pt);
                PostMessageW(hwnd, WM_KEYDOWN, VK_DOWN, 0);  // preselect the first item
                int cmd = MgrShowContextMenu(hwnd, d, targets, pt, true);
                if (cmd) MgrExecute(hwnd, d, cmd, targets, true);
                return 0;
            }

            if (src == d->hList) {
                int item = ListView_GetNextItem(d->hList, -1, LVNI_FOCUSED);
                if (item >= 0 && !(ListView_GetItemState(d->hList, item, LVIS_SELECTED) & LVIS_SELECTED)) {
                    ListView_SetItemState(d->hList, -1, 0, LVIS_SELECTED);
                    ListView_SetItemState(d->hList, item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
                }
                if (item < 0) item = ListView_GetNextItem(d->hList, -1, LVNI_SELECTED);
                POINT pt{8, 8};
                if (item >= 0) {
                    RECT r{};
                    ListView_GetItemRect(d->hList, item, &r, LVIR_LABEL);
                    pt = POINT{r.left, r.bottom};
                }
                ClientToScreen(d->hList, &pt);
                auto targets = MgrSelectedPaths(d);
                PostMessageW(hwnd, WM_KEYDOWN, VK_DOWN, 0);  // preselect the first item
                int cmd = MgrShowContextMenu(hwnd, d, targets, pt, false);
                if (cmd) MgrExecute(hwnd, d, cmd, targets, false);
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE:
            if (d && d->dragging) {
                POINT pt{};
                GetCursorPos(&pt);
                MgrUpdateDrop(d, pt);
                return 0;
            }
            break;

        case WM_LBUTTONDOWN:
            // Click on an empty area (header, margins): take focus away from the
            // search box so its placeholder comes back.
            if (d && !d->dragging && GetFocus() == d->hSearch) {
                SetFocus(d->hList);
            }
            break;

        case WM_LBUTTONUP:
            if (d && d->dragging) {
                POINT pt{};
                GetCursorPos(&pt);
                MgrUpdateDrop(d, pt);
                MgrEndDrag(hwnd, d, true);
                return 0;
            }
            break;

        case WM_CAPTURECHANGED:
            if (d && d->dragging && reinterpret_cast<HWND>(lParam) != hwnd) {
                MgrEndDrag(hwnd, d, false);
            }
            return 0;

        case WM_COMMAND: {
            if (!d) break;
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);
            if (id == IDC_MGR_SEARCH && code == EN_CHANGE) {
                if (!d->ignoreSearchChange) {
                    MgrRebuildList(d, LoadBookmarks());
                    MgrUpdateSelectionBar(d);
                }
                return 0;
            }
            if (id == IDC_MGR_MORE && code == BN_CLICKED) {
                RECT br{};
                GetWindowRect(d->hMore, &br);
                HMENU m = CreatePopupMenu();
                AppendMenuW(m, MF_STRING, CMD_SORT, L"Sort by name");
                AppendMenuW(m, MF_STRING, CMD_ADD_BOOKMARK, L"Add new bookmark");
                AppendMenuW(m, MF_STRING, CMD_ADD_FOLDER, L"Add new folder");
                AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
                AppendMenuW(m, MF_STRING, CMD_IMPORT, L"Import bookmarks");
                AppendMenuW(m, MF_STRING, CMD_EXPORT, L"Export bookmarks");
                // Opened with Enter: preselect the first item
                const bool byKeyboard = g_mgrMenuByKeyboard;
                g_mgrMenuByKeyboard = false;
                if (byKeyboard) PostMessageW(hwnd, WM_KEYDOWN, VK_DOWN, 0);
                int cmd = ModTrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_TOPALIGN,
                                            br.right, br.bottom, hwnd);
                DestroyMenu(m);
                if (cmd) MgrExecute(hwnd, d, cmd, {}, false);
                return 0;
            }
            if (id == IDC_MGR_SEL_DELETE && code == BN_CLICKED) {
                MgrExecute(hwnd, d, CMD_DELETE, MgrSelectedPaths(d), false);
                SetFocus(d->hList);
                return 0;
            }
            if (id == IDC_MGR_SEL_CLEAR && code == BN_CLICKED) {
                ListView_SetItemState(d->hList, -1, 0, LVIS_SELECTED);
                SetFocus(d->hList);
                return 0;
            }
            break;
        }

        case kMgrRefreshMsg:
            if (d && !d->dragging) MgrRefresh(d);
            return 0;

        case kMgrNavigateMsg:
            if (d) {
                NodePath p;
                {
                    std::lock_guard lock(g_mgrNavMutex);
                    p = g_mgrNavPath;
                }
                MgrNavigate(d, p);
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            if (d) {
                // Freed by BookmarkManagerWndProc once no handler is using it
                d->destroyed = true;
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK BookmarkManagerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* d = reinterpret_cast<ManagerWindowData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (msg == WM_CREATE) {
        d = reinterpret_cast<ManagerWindowData*>(reinterpret_cast<CREATESTRUCTW*>(lParam)->lpCreateParams);
    }
    if (d) d->callDepth++;
    LRESULT result = 0;
    try {
        result = BookmarkManagerWndProcImpl(hwnd, msg, wParam, lParam);
    } catch (...) {
        Wh_Log(L"BookmarkManagerWndProc: exception (msg %u)", msg);
        result = DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    if (d && --d->callDepth == 0 && d->destroyed) MgrFreeData(d);
    return result;
}

void OpenBookmarkManagerWindow(HWND ownerExplorer, const NodePath& folder) {
    {
        std::lock_guard lock(g_mgrNavMutex);
        g_mgrNavPath = folder;
    }
    if (HWND existing = FindOwnProcessWindow(kMgrClass)) {
        ShowWindow(existing, IsIconic(existing) ? SW_RESTORE : SW_SHOW);
        SetForegroundWindow(existing);
        PostMessageW(existing, kMgrNavigateMsg, 0, 0);
        return;
    }

    const RECT wr = CenteredWindowRect(ownerExplorer, 1040, 660, WindowDpi(ownerExplorer));
    const int x = wr.left, y = wr.top, width = wr.right - wr.left, height = wr.bottom - wr.top;

    auto* data = new ManagerWindowData();
    data->hOwnerExplorer = ownerExplorer;
    data->currentFolder = folder;

    HWND w = CreateWindowExW(0, kMgrClass, L"Bookmarks - File Explorer",
                             WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
                             x, y, width, height, nullptr, nullptr, ModuleInstance(), data);
    if (!w) delete data;
}

// Registered once in Wh_ModInit and unregistered in Wh_ModUninit.
HBRUSH g_dropLineBrush = nullptr;

bool RegisterModClass(const WNDCLASSEXW& wc) {
    if (RegisterClassExW(&wc)) return true;
    if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
        // Left over from a previous load that ended abnormally
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        if (RegisterClassExW(&wc)) return true;
    }
    Wh_Log(L"RegisterClassExW(%s) failed: %u", wc.lpszClassName, GetLastError());
    return false;
}

void RegisterWindowClasses() {
    INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES};
    InitCommonControlsEx(&icc);
    const HINSTANCE inst = ModuleInstance();

    WNDCLASSEXW edit{};
    edit.cbSize = sizeof(edit);
    edit.lpfnWndProc = EditBookmarkWndProc;
    edit.hInstance = inst;
    edit.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    edit.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    edit.lpszClassName = kEditDlgClass;
    RegisterModClass(edit);

    WNDCLASSEXW mgr{};
    mgr.cbSize = sizeof(mgr);
    mgr.lpfnWndProc = BookmarkManagerWndProc;
    mgr.hInstance = inst;
    mgr.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    mgr.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    mgr.lpszClassName = kMgrClass;
    mgr.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    RegisterModClass(mgr);

    WNDCLASSEXW choice{};
    choice.cbSize = sizeof(choice);
    choice.lpfnWndProc = ChoiceDialogWndProc;
    choice.hInstance = inst;
    choice.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    choice.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    choice.lpszClassName = kChoiceDlgClass;
    RegisterModClass(choice);

    if (!g_dropLineBrush) g_dropLineBrush = CreateSolidBrush(RGB(26, 115, 232));  // Chrome blue
    WNDCLASSEXW line{};
    line.cbSize = sizeof(line);
    line.lpfnWndProc = DropLineWndProc;
    line.hInstance = inst;
    line.hbrBackground = g_dropLineBrush;
    line.lpszClassName = kDropLineClass;
    RegisterModClass(line);
}

void UnregisterWindowClasses() {
    const HINSTANCE inst = ModuleInstance();
    UnregisterClassW(kMgrClass, inst);
    UnregisterClassW(kDropLineClass, inst);
    UnregisterClassW(kEditDlgClass, inst);
    UnregisterClassW(kChoiceDlgClass, inst);
    if (g_dropLineBrush) {
        DeleteObject(g_dropLineBrush);
        g_dropLineBrush = nullptr;
    }
}

// ============================================================================
// Chrome-style right-click menu for the bar and the folder dropdowns
// ============================================================================

muxc::MenuFlyout BuildChromeBookmarkContextMenu(const mux::FrameworkElement& anchor, const NodePath& path) {
    muxc::MenuFlyout menu;
    auto list = LoadBookmarks();
    const BookmarkNode* node = path.empty() ? nullptr : NodeAt(list, path);
    const bool canPaste = ClipboardHasData();
    auto weakAnchor = winrt::make_weak(anchor);

    // Where "Add..." / "Paste" go (Chrome rules)
    NodePath insertFolder;
    int insertIndex = -1;
    if (node && node->type == NodeType::FolderBox) {
        insertFolder = path;
    } else if (node) {
        insertFolder = ParentOf(path);
        insertIndex = path.back() + 1;
    }
    NodePath scope = node ? ParentOf(path) : NodePath{};
    NodePath managerFolder = (node && node->type == NodeType::FolderBox) ? path : ParentOf(path);

    auto addItem = [&](const std::wstring& text, bool enabled, std::function<void(HWND)> action) {
        muxc::MenuFlyoutItem mi;
        mi.Text(text);
        mi.IsEnabled(enabled);
        // Highlight follows the mouse (instead of staying stuck on the first item).
        // MenuFlyoutItem handles pointer events internally, so a plain PointerEntered
        // handler may never run: register with handledEventsToo = true, and also on
        // PointerMoved (in case the mouse is already over an item when the menu opens).
        auto hoverFocus = muxi::PointerEventHandler{
            [](wf::IInspectable const& s, muxi::PointerRoutedEventArgs const& e) {
                if (e.Pointer().PointerDeviceType() != winrt::Microsoft::UI::Input::PointerDeviceType::Mouse &&
                    e.Pointer().PointerDeviceType() != winrt::Microsoft::UI::Input::PointerDeviceType::Pen) {
                    return;
                }
                auto c = s.try_as<muxc::Control>();
                if (!c || !c.IsEnabled()) return;
                if (c.FocusState() == mux::FocusState::Unfocused) {
                    c.Focus(mux::FocusState::Pointer);
                }
            }};
        TrackAddHandler(mi, mux::UIElement::PointerEnteredEvent(), winrt::box_value(hoverFocus), true);
        TrackAddHandler(mi, mux::UIElement::PointerMovedEvent(), winrt::box_value(hoverFocus), true);
        TrackEvent(mi, mi.Click([weakAnchor, action, scope](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            mux::FrameworkElement a = weakAnchor.get();
            HWND frame = ExplorerWindowForElement(a);
            PostToFrame(frame, [frame, action, scope]() {
                g_pendingScope = scope;
                action(frame);
            });
        }),
           [](const auto& e, winrt::event_token t) { e.Click(t); });
        menu.Items().Append(mi);
    };
    auto sep = [&]() { menu.Items().Append(muxc::MenuFlyoutSeparator{}); };

    if (node && node->type == NodeType::Item) {
        std::wstring p = node->path;
        addItem(L"Open in new tab", true, [p](HWND f) { QueueOpenTargets(f, {p}, OpenDisposition::NewTab); });
        addItem(L"Open in new window", true, [p](HWND f) { QueueOpenTargets(f, {p}, OpenDisposition::NewWindow); });
        sep();
        addItem(L"Edit\u2026", true, [path](HWND f) { EditNodeFlow(f, path); });
        sep();
    } else if (node) {
        size_t n = CollectOpenPaths(list, {path}).size();
        addItem(L"Open all (" + std::to_wstring(n) + L")", n > 0, [path](HWND f) {
            QueueOpenTargets(f, CollectOpenPaths(LoadBookmarks(), {path}), OpenDisposition::NewTab);
        });
        addItem(L"Open all in new window", n > 0, [path](HWND f) {
            QueueOpenTargets(f, CollectOpenPaths(LoadBookmarks(), {path}), OpenDisposition::NewWindow);
        });
        sep();
        addItem(L"Rename\u2026", true, [path](HWND f) { EditNodeFlow(f, path); });
        sep();
    }

    if (node) {
        addItem(L"Cut", true, [path](HWND) { DoCutOrCopy({path}, true); });
        addItem(L"Copy", true, [path](HWND) { DoCutOrCopy({path}, false); });
    }
    addItem(L"Paste", canPaste, [insertFolder, insertIndex](HWND f) { DoPaste(insertFolder, insertIndex, f); });
    if (node) {
        addItem(L"Delete", true, [path](HWND) { DoDelete({path}); });
    }
    sep();
    addItem(L"Add Page", true, [insertFolder, insertIndex](HWND f) {
        AddBookmarkFlow(f, insertFolder, insertIndex);
    });
    addItem(L"Add Folder", true, [insertFolder, insertIndex](HWND f) {
        AddFolderFlow(f, insertFolder, insertIndex);
    });
    sep();
    addItem(L"Bookmark manager", true, [managerFolder](HWND f) { OpenBookmarkManagerWindow(f, managerFolder); });

    return menu;
}

// The right-click menu that is open on this thread, if any. Its item handlers are
// mod code, so it is closed (handlers revoked first) when the bar is removed.
struct OpenContextMenu {
    muxc::MenuFlyout menu{nullptr};
    winrt::event_token openedToken{};
    winrt::event_token closedToken{};
};
[[clang::no_destroy]] thread_local std::optional<OpenContextMenu> g_openContextMenu;

void CloseOpenContextMenu() {
    if (!g_openContextMenu) return;
    OpenContextMenu open = std::move(*g_openContextMenu);
    g_openContextMenu.reset();
    if (!open.menu) return;
    try {
        if (open.openedToken) open.menu.Opened(open.openedToken);
    } catch (...) {}
    try {
        if (open.closedToken) open.menu.Closed(open.closedToken);
    } catch (...) {}
    try {
        open.menu.Hide();
    } catch (...) {}
}

// Show a context menu at the pointer in "standard" mode (pointer, not keyboard focus)
void ShowContextMenuAt(const muxc::MenuFlyout& menu, const mux::FrameworkElement& fe,
                       const muxi::ContextRequestedEventArgs& args) {
    if (g_unloading) return;
    CloseOpenContextMenu();
    g_openContextMenu.emplace();
    g_openContextMenu->menu = menu;
    {
        auto weakClosing = winrt::make_weak(menu);
        g_openContextMenu->closedToken = TrackEvent(menu, menu.Closed([weakClosing](wf::IInspectable const&, wf::IInspectable const&) {
            auto m = weakClosing.get();
            if (!m || !g_openContextMenu || g_openContextMenu->menu != m) return;
            try {
                if (g_openContextMenu->openedToken) m.Opened(g_openContextMenu->openedToken);
            } catch (...) {}
            g_openContextMenu.reset();
        }),
           [](const auto& e, winrt::event_token t) { e.Closed(t); });
    }
    try {
        menu.ShouldConstrainToRootBounds(false);
    } catch (...) {}
    muxp::FlyoutShowOptions opts;
    opts.ShowMode(muxp::FlyoutShowMode::Standard);
    wf::Point pt{};
    const bool openedByMouse = args.TryGetPosition(fe, pt);
    if (openedByMouse) {
        opts.Position(wf::IReference<wf::Point>(pt));

        // Like Chrome: when opened with the mouse, no item is highlighted initially.
        // Focus goes to the menu (presenter) instead of an item; Up/Down then moves
        // the highlight to the first/last item. When opened with the keyboard
        // (Shift+F10 / Menu key) the first item stays highlighted.
        auto weakMenu = winrt::make_weak(menu);
        auto dq = fe.DispatcherQueue();
        g_openContextMenu->openedToken = TrackEvent(menu, menu.Opened([weakMenu, dq](wf::IInspectable const&, wf::IInspectable const&) {
            if (!dq) return;
            dq.TryEnqueue(winrt::Microsoft::UI::Dispatching::DispatcherQueuePriority::Low,
                          [token = std::make_shared<PendingScanToken>(), weakMenu]() {
                try {
                    auto m = weakMenu.get();
                    if (!m || m.Items().Size() == 0) return;
                    auto first = m.Items().GetAt(0).try_as<mux::DependencyObject>();
                    mux::DependencyObject cur = first;
                    muxc::MenuFlyoutPresenter presenter{nullptr};
                    for (int i = 0; cur && i < 12; i++) {
                        cur = muxm::VisualTreeHelper::GetParent(cur);
                        if (!cur) break;
                        presenter = cur.try_as<muxc::MenuFlyoutPresenter>();
                        if (presenter) break;
                    }
                    if (!presenter) return;
                    // The mouse is already over an item: keep its hover focus
                    for (auto const& it : m.Items()) {
                        auto c = it.try_as<muxc::Control>();
                        if (c && c.FocusState() == mux::FocusState::Pointer) return;
                    }
                    // Up/Down while the menu itself has focus: select the first/last item
                    auto weakM2 = winrt::make_weak(m);
                    TrackAddHandler(presenter, 
                        mux::UIElement::KeyDownEvent(),
                        winrt::box_value(muxi::KeyEventHandler{
                            [weakM2](wf::IInspectable const& sender, muxi::KeyRoutedEventArgs const& e) {
                                auto key = e.Key();
                                if (key != winrt::Windows::System::VirtualKey::Down &&
                                    key != winrt::Windows::System::VirtualKey::Up) return;
                                auto p = sender.try_as<muxc::Control>();
                                auto m2 = weakM2.get();
                                if (!p || !m2 || p.FocusState() == mux::FocusState::Unfocused) return;
                                std::vector<muxc::Control> items;
                                for (auto const& it : m2.Items()) {
                                    auto c = it.try_as<muxc::Control>();
                                    if (c && c.IsEnabled() && !it.try_as<muxc::MenuFlyoutSeparator>()) {
                                        items.push_back(c);
                                    }
                                }
                                if (items.empty()) return;
                                auto target = key == winrt::Windows::System::VirtualKey::Down ? items.front() : items.back();
                                target.Focus(mux::FocusState::Keyboard);
                                e.Handled(true);
                            }}),
                        false);
                    presenter.IsTabStop(true);
                    presenter.Focus(mux::FocusState::Programmatic);
                } catch (...) {}
            });
        }),
           [](const auto& e, winrt::event_token t) { e.Opened(t); });
    }
    menu.ShowAt(fe, opts);
}

// ============================================================================
// Drag highlight helpers
// ============================================================================

void ClearHighlightVisual(BarState& state) {
    if (auto btn = state.highlightedBtn.get()) {
        try {
            btn.BorderBrush(state.highlightedOldBrush);
            btn.BorderThickness(state.highlightedOldThickness);
        } catch (...) {}
    }
    state.highlightedBtn = nullptr;
    state.highlightedOldBrush = nullptr;
    state.dropTargetFolderBoxIndex = -1;
    state.dropReorderInsertIndex = -1;
}

void ApplyHighlightVisual(BarState& state, const muxc::Button& targetBtn, mux::Thickness thickness) {
    ClearHighlightVisual(state);
    try {
        state.highlightedBtn = winrt::make_weak(targetBtn);
        state.highlightedOldBrush = targetBtn.BorderBrush();
        state.highlightedOldThickness = targetBtn.BorderThickness();
        targetBtn.BorderBrush(muxm::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 200, 50}));
        targetBtn.BorderThickness(thickness);
    } catch (...) {
        ClearHighlightVisual(state);
    }
}

void ClearFlyoutIndicator(FlyoutDragCtx& ctx) {
    if (ctx.indBtn) {
        try {
            ctx.indBtn.BorderBrush(ctx.indOldBrush);
            ctx.indBtn.BorderThickness(ctx.indOldThick);
        } catch (...) {}
    }
    ctx.indBtn = nullptr;
    ctx.indOldBrush = nullptr;
}

void ShowFlyoutIndicator(FlyoutDragCtx& ctx, const muxc::Button& target, mux::Thickness thickness) {
    ClearFlyoutIndicator(ctx);
    try {
        ctx.indBtn = target;
        ctx.indOldBrush = target.BorderBrush();
        ctx.indOldThick = target.BorderThickness();
        target.BorderBrush(muxm::SolidColorBrush(winrt::Windows::UI::Color{255, 255, 200, 50}));
        target.BorderThickness(thickness);
    } catch (...) {
        ctx.indBtn = nullptr;
        ctx.indOldBrush = nullptr;
    }
}

// ============================================================================
// Folder dropdowns (nested submenus like Chrome, stay open while editing)
// ============================================================================

void ShowFolderFlyout(const winrt::weak_ref<muxc::StackPanel>& weakPanel, const NodePath& folder,
                      const mux::FrameworkElement& target, const muxc::Button& windowAnchor, bool submenu);

// ---- Keyboard navigation inside folder dropdowns -------------------------
// True when a dropdown is being opened with the keyboard (Enter / Right arrow):
// its first item is then selected right away. Opened with the mouse, nothing is selected.
thread_local bool g_flyoutOpenByKeyboard = false;

muxm::SolidColorBrush FlyoutKbHighlightBrush() {
    // Neutral grey with alpha: visible in both light and dark mode
    return muxm::SolidColorBrush(winrt::Windows::UI::Color{0x40, 0x80, 0x80, 0x80});
}

muxm::SolidColorBrush FlyoutClearBrush() {
    return muxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
}

int FlyoutRowCount(const muxc::StackPanel& lp) {
    uint32_t n = lp.Children().Size();
    if (n == 0 || !lp.Children().GetAt(0).try_as<muxc::Button>()) return 0;  // "(empty)"
    return static_cast<int>(n);
}

int FlyoutFocusedRow(const muxc::StackPanel& lp) {
    int n = FlyoutRowCount(lp);
    for (int i = 0; i < n; ++i) {
        auto b = lp.Children().GetAt(i).try_as<muxc::Button>();
        if (b && b.FocusState() != mux::FocusState::Unfocused) return i;
    }
    return -1;
}

void FlyoutClearKbHighlight(const muxc::StackPanel& lp) {
    int n = FlyoutRowCount(lp);
    for (int i = 0; i < n; ++i) {
        if (auto b = lp.Children().GetAt(i).try_as<muxc::Button>()) b.Background(FlyoutClearBrush());
    }
}

void FlyoutKbSelect(const muxc::StackPanel& lp, int k) {
    int n = FlyoutRowCount(lp);
    if (k < 0 || k >= n) return;
    auto b = lp.Children().GetAt(k).try_as<muxc::Button>();
    if (!b) return;
    FlyoutClearKbHighlight(lp);
    b.Background(FlyoutKbHighlightBrush());
    b.Focus(mux::FocusState::Keyboard);
    try {
        b.StartBringIntoView();
    } catch (...) {}
}

void HideAllFlyouts(const winrt::weak_ref<muxc::StackPanel>& weakPanel) {
    auto p = weakPanel.get();
    auto* st = p ? FindBarState(p) : nullptr;
    if (!st) return;
    auto copies = st->openFlyouts;
    for (auto& f : copies) {
        try {
            if (f.flyout) f.flyout.Hide();
        } catch (...) {}
    }
}

void BuildFolderFlyoutList(const muxc::StackPanel& listPanel, const NodePath& folder,
                           const winrt::weak_ref<muxc::StackPanel>& weakPanel,
                           const muxc::Flyout& fly, const muxc::Button& windowAnchor) {
    listPanel.Children().Clear();

    auto list = LoadBookmarks();
    const auto* kidsPtr = folder.empty() ? nullptr : FolderChildren(list, folder);
    if (!kidsPtr) {
        fly.Hide();
        return;
    }
    const std::vector<BookmarkNode> kids = *kidsPtr;
    auto weakList = winrt::make_weak(listPanel);
    auto weakAnchor = winrt::make_weak(windowAnchor);

    if (kids.empty()) {
        muxc::TextBlock emptyTxt;
        emptyTxt.Text(L"(empty)");
        emptyTxt.FontSize(13);
        emptyTxt.Opacity(0.6);
        emptyTxt.Margin(mux::Thickness{10, 6, 10, 6});
        listPanel.Children().Append(emptyTxt);
        return;
    }

    auto folderFlags = std::make_shared<std::vector<bool>>();
    for (const auto& k : kids) folderFlags->push_back(k.type == NodeType::FolderBox);

    for (size_t cIdx = 0; cIdx < kids.size(); ++cIdx) {
        const auto& child = kids[cIdx];
        const bool isFolder = child.type == NodeType::FolderBox;
        const int childIdx = static_cast<int>(cIdx);
        const NodePath childPath = ChildOf(folder, childIdx);

        muxc::Button row;
        row.HorizontalAlignment(mux::HorizontalAlignment::Stretch);
        row.HorizontalContentAlignment(mux::HorizontalAlignment::Stretch);
        row.Height(32);
        row.Padding(mux::Thickness{10, 0, 10, 0});
        row.Background(muxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
        row.BorderThickness(mux::Thickness{0, 0, 0, 0});

        muxc::Grid g;
        muxc::ColumnDefinition c0;
        c0.Width(mux::GridLength{1, mux::GridUnitType::Auto});
        muxc::ColumnDefinition c1;
        c1.Width(mux::GridLength{1, mux::GridUnitType::Star});
        muxc::ColumnDefinition c2;
        c2.Width(mux::GridLength{1, mux::GridUnitType::Auto});
        g.ColumnDefinitions().Append(c0);
        g.ColumnDefinitions().Append(c1);
        g.ColumnDefinitions().Append(c2);

        auto icon = GetShellIconElement(child.path, isFolder);
        if (auto fe = icon.try_as<mux::FrameworkElement>()) muxc::Grid::SetColumn(fe, 0);
        g.Children().Append(icon);

        muxc::TextBlock lbl;
        lbl.Text(child.name);
        lbl.FontSize(13);
        lbl.MaxWidth(240);
        lbl.Margin(mux::Thickness{8, 0, 8, 0});
        lbl.VerticalAlignment(mux::VerticalAlignment::Center);
        lbl.TextTrimming(mux::TextTrimming::CharacterEllipsis);
        muxc::Grid::SetColumn(lbl, 1);
        g.Children().Append(lbl);

        if (isFolder) {
            muxc::FontIcon chevron;
            chevron.Glyph(L"\uE76C");
            chevron.FontFamily(muxm::FontFamily{L"Segoe Fluent Icons"});
            chevron.FontSize(10);
            chevron.Opacity(0.75);
            chevron.VerticalAlignment(mux::VerticalAlignment::Center);
            muxc::Grid::SetColumn(chevron, 2);
            g.Children().Append(chevron);
        }

        row.Content(g);
        muxc::ToolTipService::SetToolTip(row, winrt::box_value(
            isFolder ? child.name : (child.name + L"\n" + child.path)));

        // Keyboard selection (like Chrome): shown only for arrow-key navigation, using
        // our own highlight instead of the system focus rectangle; cleared on focus loss.
        row.UseSystemFocusVisuals(false);
        TrackEvent(row, row.LostFocus([](wf::IInspectable const& s, mux::RoutedEventArgs const&) {
            if (auto b = s.try_as<muxc::Button>()) b.Background(FlyoutClearBrush());
        }),
           [](const auto& e, winrt::event_token t) { e.LostFocus(t); });

        auto dragCtx = std::make_shared<FlyoutDragCtx>();
        std::wstring cPath = child.path;

        TrackEvent(row, row.Click([weakPanel, weakAnchor, cPath, isFolder, childPath, dragCtx](wf::IInspectable const& sender,
                                                                               mux::RoutedEventArgs const&) {
            if (dragCtx->suppressClick) {
                dragCtx->suppressClick = false;
                return;
            }
            auto rowFe = sender.try_as<mux::FrameworkElement>();
            auto anchor = weakAnchor.get();
            if (isFolder) {
                if (auto rb = sender.try_as<muxc::Control>()) {
                    g_flyoutOpenByKeyboard = rb.FocusState() == mux::FocusState::Keyboard;
                }
                if (rowFe && anchor) ShowFolderFlyout(weakPanel, childPath, rowFe, anchor, true);
                return;
            }
            const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            HWND frame = ExplorerWindowForElement(anchor);
            HideAllFlyouts(weakPanel);
            QueueOpenTargets(frame, {cPath}, ctrl ? OpenDisposition::NewTab : OpenDisposition::CurrentTab);
        }),
           [](const auto& e, winrt::event_token t) { e.Click(t); });

        TrackAddHandler(row, 
            mux::UIElement::PointerPressedEvent(),
            winrt::box_value(muxi::PointerEventHandler{
                [dragCtx](wf::IInspectable const& sender, muxi::PointerRoutedEventArgs const& args) {
                    auto b = sender.try_as<muxc::Button>();
                    if (!b) return;
                    auto pt = args.GetCurrentPoint(b);
                    if (!pt.Properties().IsLeftButtonPressed()) return;
                    dragCtx->pressed = true;
                    dragCtx->dragging = false;
                    dragCtx->suppressClick = false;
                    dragCtx->hasDrop = false;
                    dragCtx->pointerId = pt.PointerId();
                    dragCtx->start = args.GetCurrentPoint(nullptr).Position();
                    b.CapturePointer(args.Pointer());
                }}),
            true);

        TrackAddHandler(row, 
            mux::UIElement::PointerMovedEvent(),
            winrt::box_value(muxi::PointerEventHandler{
                [weakPanel, dragCtx, folder, childIdx, weakList, folderFlags](
                    wf::IInspectable const& sender, muxi::PointerRoutedEventArgs const& args) {
                    if (!dragCtx->pressed) return;
                    auto b = sender.try_as<muxc::Button>();
                    auto p = weakPanel.get();
                    auto* st = p ? FindBarState(p) : nullptr;
                    if (!b || !st) return;

                    auto cur = args.GetCurrentPoint(nullptr);
                    if (cur.PointerId() != dragCtx->pointerId || !cur.Properties().IsLeftButtonPressed()) return;

                    auto pos = cur.Position();
                    float dx = pos.X - dragCtx->start.X;
                    float dy = pos.Y - dragCtx->start.Y;
                    if (!dragCtx->dragging && (dx * dx + dy * dy >= kDragThreshold * kDragThreshold)) {
                        dragCtx->dragging = true;
                        dragCtx->suppressClick = true;
                        b.Opacity(0.55);
                        dragCtx->barFolders.clear();
                        for (const auto& n : LoadBookmarks()) {
                            dragCtx->barFolders.push_back(n.type == NodeType::FolderBox);
                        }
                    }
                    if (!dragCtx->dragging) return;

                    args.Handled(true);
                    ClearHighlightVisual(*st);
                    ClearFlyoutIndicator(*dragCtx);
                    dragCtx->hasDrop = false;

                    // 1) Rows of this dropdown: reorder line, or drop INTO a sub-folder
                    if (auto lp = weakList.get()) {
                        uint32_t cnt = lp.Children().Size();
                        for (uint32_t k = 0; k < cnt; ++k) {
                            auto tb = lp.Children().GetAt(k).try_as<muxc::Button>();
                            if (!tb) continue;
                            auto local = args.GetCurrentPoint(tb).Position();
                            double w = tb.ActualWidth();
                            double h = tb.ActualHeight();
                            if (local.X >= -4 && local.X <= w + 4 && local.Y >= -2 && local.Y <= h + 2) {
                                bool isF = k < folderFlags->size() && (*folderFlags)[k];
                                if (isF && static_cast<int>(k) != childIdx && local.Y >= h * 0.25 && local.Y <= h * 0.75) {
                                    dragCtx->hasDrop = true;
                                    dragCtx->dropFolder = ChildOf(folder, static_cast<int>(k));
                                    dragCtx->dropIndex = -1;
                                    ShowFlyoutIndicator(*dragCtx, tb, mux::Thickness{2, 2, 2, 2});
                                } else {
                                    bool after = local.Y >= h / 2.0;
                                    int ins = after ? static_cast<int>(k) + 1 : static_cast<int>(k);
                                    if (ins != childIdx && ins != childIdx + 1) {
                                        dragCtx->hasDrop = true;
                                        dragCtx->dropFolder = folder;
                                        dragCtx->dropIndex = ins;
                                        ShowFlyoutIndicator(*dragCtx, tb,
                                                            after ? mux::Thickness{0, 0, 0, 3} : mux::Thickness{0, 3, 0, 0});
                                    }
                                }
                                return;
                            }
                        }
                    }

                    // 2) Bar buttons: into a bar folder, or onto the bar
                    muxc::StackPanel barRow{nullptr};
                    if (p.Children().Size() > 0) barRow = p.Children().GetAt(0).try_as<muxc::StackPanel>();
                    if (!barRow) return;

                    const auto& barFolders = dragCtx->barFolders;
                    uint32_t n = barRow.Children().Size();
                    bool overButton = false;
                    for (uint32_t j = 0; j < n && j < barFolders.size(); ++j) {
                        auto targetBtn = barRow.Children().GetAt(j).try_as<muxc::Button>();
                        if (!targetBtn) continue;
                        auto local = args.GetCurrentPoint(targetBtn).Position();
                        double w = targetBtn.ActualWidth();
                        double h = targetBtn.ActualHeight();
                        if (local.X >= -4 && local.X <= w + 4 && local.Y >= -8 && local.Y <= h + 8) {
                            overButton = true;
                            if (barFolders[j] && local.X >= w * 0.12 && local.X <= w * 0.88) {
                                ApplyHighlightVisual(*st, targetBtn, mux::Thickness{2, 2, 2, 2});
                                dragCtx->hasDrop = true;
                                dragCtx->dropFolder = NodePath{static_cast<int>(j)};
                                dragCtx->dropIndex = -1;
                            } else {
                                bool after = local.X >= w / 2.0;
                                ApplyHighlightVisual(*st, targetBtn,
                                                     after ? mux::Thickness{0, 0, 3, 0} : mux::Thickness{3, 0, 0, 0});
                                dragCtx->hasDrop = true;
                                dragCtx->dropFolder = NodePath{};
                                dragCtx->dropIndex = after ? static_cast<int>(j) + 1 : static_cast<int>(j);
                            }
                            break;
                        }
                    }

                    if (!overButton) {
                        if (auto barRoot = st->barRoot.get()) {
                            auto lp2 = args.GetCurrentPoint(barRoot).Position();
                            if (lp2.X >= 0 && lp2.X <= barRoot.ActualWidth() &&
                                lp2.Y >= 0 && lp2.Y <= barRoot.ActualHeight()) {
                                dragCtx->hasDrop = true;
                                dragCtx->dropFolder = NodePath{};
                                dragCtx->dropIndex = static_cast<int>(barFolders.size());
                                if (n > 0) {
                                    if (auto lastBtn = barRow.Children().GetAt(n - 1).try_as<muxc::Button>()) {
                                        ApplyHighlightVisual(*st, lastBtn, mux::Thickness{0, 0, 3, 0});
                                    }
                                }
                            }
                        }
                    }
                }}),
            true);

        TrackAddHandler(row, 
            mux::UIElement::PointerReleasedEvent(),
            winrt::box_value(muxi::PointerEventHandler{
                [weakPanel, dragCtx, folder, childIdx](wf::IInspectable const& sender,
                                                       muxi::PointerRoutedEventArgs const& args) {
                    if (!dragCtx->pressed) return;
                    dragCtx->pressed = false;

                    if (auto b = sender.try_as<muxc::Button>()) {
                        b.ReleasePointerCapture(args.Pointer());
                        b.Opacity(1.0);
                    }
                    auto p = weakPanel.get();
                    auto* st = p ? FindBarState(p) : nullptr;
                    if (st) ClearHighlightVisual(*st);

                    bool wasDragging = dragCtx->dragging;
                    bool hasDrop = dragCtx->hasDrop;
                    NodePath dropFolder = dragCtx->dropFolder;
                    int dropIndex = dragCtx->dropIndex;
                    ClearFlyoutIndicator(*dragCtx);
                    dragCtx->dragging = false;
                    dragCtx->hasDrop = false;

                    if (!wasDragging) return;
                    args.Handled(true);
                    if (!hasDrop) return;

                    auto list = LoadBookmarks();
                    NodePath src = ChildOf(folder, childIdx);
                    if (!MoveNodes(list, std::vector<NodePath>{src}, dropFolder, dropIndex)) return;

                    // Stays inside this dropdown (reorder / into sub-folder) -> keep it open
                    bool staysInside = IsPrefixOf(folder, dropFolder);
                    g_pendingScope = folder;
                    if (!staysInside) HideAllFlyouts(weakPanel);
                    SaveBookmarks(list);
                }}),
            true);

        TrackEvent(row, row.ContextRequested([childPath](mux::UIElement const& sender, muxi::ContextRequestedEventArgs const& args) {
            args.Handled(true);
            auto fe = sender.try_as<mux::FrameworkElement>();
            if (!fe) return;
            ShowContextMenuAt(BuildChromeBookmarkContextMenu(fe, childPath), fe, args);
        }),
           [](const auto& e, winrt::event_token t) { e.ContextRequested(t); });

        listPanel.Children().Append(row);
    }
}

void ShowFolderFlyout(const winrt::weak_ref<muxc::StackPanel>& weakPanel, const NodePath& folder,
                      const mux::FrameworkElement& target, const muxc::Button& windowAnchor, bool submenu) {
    auto p = weakPanel.get();
    auto* st = p ? FindBarState(p) : nullptr;

    // Close sibling / deeper submenus first (like Chrome)
    if (st) {
        auto copies = st->openFlyouts;
        for (auto& of : copies) {
            if (!submenu || of.folder.size() >= folder.size()) {
                try {
                    if (of.flyout) of.flyout.Hide();
                } catch (...) {}
            }
        }
    }

    muxc::Flyout fly;
    muxc::ScrollViewer sv;
    sv.MaxHeight(420);
    sv.HorizontalScrollBarVisibility(muxc::ScrollBarVisibility::Disabled);
    sv.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Auto);

    muxc::StackPanel listPanel;
    listPanel.Orientation(muxc::Orientation::Vertical);
    listPanel.Spacing(2);
    listPanel.MinWidth(220);

    BuildFolderFlyoutList(listPanel, folder, weakPanel, fly, windowAnchor);
    sv.Content(listPanel);
    fly.Content(sv);

    // ---- Keyboard + mouse selection (same as the right-click menu) ----
    const bool openedByKeyboard = g_flyoutOpenByKeyboard;
    g_flyoutOpenByKeyboard = false;
    sv.IsTabStop(true);
    sv.UseSystemFocusVisuals(false);
    winrt::event_token flyOpenedToken{};
    {
        auto weakSv = winrt::make_weak(sv);
        auto weakList = winrt::make_weak(listPanel);
        auto weakFly = winrt::make_weak(fly);
        auto weakAnchor = winrt::make_weak(windowAnchor);

        // Up/Down: move the selection. Right/Enter: open a sub-folder. Left: close the submenu.
        TrackEvent(sv, sv.PreviewKeyDown([weakList, weakFly, weakAnchor, weakPanel, folder, submenu](
                              wf::IInspectable const&, muxi::KeyRoutedEventArgs const& e) {
            using VK = winrt::Windows::System::VirtualKey;
            auto lp = weakList.get();
            if (!lp) return;
            const int n = FlyoutRowCount(lp);
            const int cur = FlyoutFocusedRow(lp);
            switch (e.Key()) {
                case VK::Down:
                    if (n > 0) FlyoutKbSelect(lp, cur < 0 ? 0 : (cur + 1) % n);
                    e.Handled(true);
                    break;
                case VK::Up:
                    if (n > 0) FlyoutKbSelect(lp, cur < 0 ? n - 1 : (cur - 1 + n) % n);
                    e.Handled(true);
                    break;
                case VK::Home:
                    if (n > 0) FlyoutKbSelect(lp, 0);
                    e.Handled(true);
                    break;
                case VK::End:
                    if (n > 0) FlyoutKbSelect(lp, n - 1);
                    e.Handled(true);
                    break;
                case VK::Right: {
                    if (cur < 0) break;
                    auto list = LoadBookmarks();
                    const auto* kids = FolderChildren(list, folder);
                    auto row = lp.Children().GetAt(cur).try_as<mux::FrameworkElement>();
                    auto anchor = weakAnchor.get();
                    if (kids && cur < static_cast<int>(kids->size()) &&
                        (*kids)[cur].type == NodeType::FolderBox && row && anchor) {
                        g_flyoutOpenByKeyboard = true;
                        ShowFolderFlyout(weakPanel, ChildOf(folder, cur), row, anchor, true);
                    }
                    e.Handled(true);
                    break;
                }
                case VK::Left:
                    if (submenu) {
                        if (auto f = weakFly.get()) f.Hide();
                        e.Handled(true);
                    }
                    break;
                default:
                    break;
            }
        }),
           [](const auto& e, winrt::event_token t) { e.PreviewKeyDown(t); });

        // Any mouse movement clears the keyboard selection (like Chrome). The position is
        // compared so that synthetic events without real movement are ignored.
        auto lastPt = std::make_shared<std::optional<wf::Point>>();
        TrackAddHandler(sv, 
            mux::UIElement::PointerMovedEvent(),
            winrt::box_value(muxi::PointerEventHandler{
                [weakSv, weakList, lastPt](wf::IInspectable const&, muxi::PointerRoutedEventArgs const& e) {
                    auto pt = e.GetCurrentPoint(nullptr).Position();
                    if (*lastPt && (*lastPt)->X == pt.X && (*lastPt)->Y == pt.Y) return;
                    const bool firstEvent = !lastPt->has_value();
                    *lastPt = pt;
                    if (firstEvent) return;  // first event: only remember the position
                    auto lp = weakList.get();
                    auto s = weakSv.get();
                    if (!lp || !s) return;
                    int cur = FlyoutFocusedRow(lp);
                    if (cur < 0) return;
                    auto b = lp.Children().GetAt(cur).try_as<muxc::Button>();
                    if (!b || b.FocusState() != mux::FocusState::Keyboard) return;
                    FlyoutClearKbHighlight(lp);
                    s.Focus(mux::FocusState::Programmatic);
                }}),
            true);

        // On open: via mouse -> no selection; via keyboard -> select the first item
        auto dq = sv.DispatcherQueue();
        flyOpenedToken = TrackEvent(fly, fly.Opened([weakSv, weakList, dq, openedByKeyboard](wf::IInspectable const&, wf::IInspectable const&) {
            if (!dq) return;
            dq.TryEnqueue(winrt::Microsoft::UI::Dispatching::DispatcherQueuePriority::Low,
                          [token = std::make_shared<PendingScanToken>(), weakSv, weakList, openedByKeyboard]() {
                try {
                    auto s = weakSv.get();
                    auto lp = weakList.get();
                    if (!s || !lp) return;
                    if (openedByKeyboard && FlyoutRowCount(lp) > 0) {
                        FlyoutKbSelect(lp, 0);
                    } else {
                        FlyoutClearKbHighlight(lp);
                        s.Focus(mux::FocusState::Programmatic);
                    }
                } catch (...) {}
            });
        }),
           [](const auto& e, winrt::event_token t) { e.Opened(t); });
    }

    uint64_t id = 0;
    if (st) {
        id = ++st->flyoutGen;
        OpenFlyout of;
        of.id = id;
        of.folder = folder;
        of.flyout = fly;
        auto weakFly = winrt::make_weak(fly);
        auto weakList = winrt::make_weak(listPanel);
        auto weakAnchor = winrt::make_weak(windowAnchor);
        of.rebuild = [weakList, folder, weakPanel, weakFly, weakAnchor]() {
            auto lp = weakList.get();
            auto f = weakFly.get();
            auto a = weakAnchor.get();
            if (lp && f && a) BuildFolderFlyoutList(lp, folder, weakPanel, f, a);
        };
        of.openedToken = flyOpenedToken;
        st->openFlyouts.push_back(std::move(of));
    }

    winrt::event_token flyClosedToken = TrackEvent(fly, fly.Closed([weakPanel, id](wf::IInspectable const&, wf::IInspectable const&) {
        bool needRefresh = false;
        if (auto p2 = weakPanel.get()) {
            if (auto* st2 = FindBarState(p2)) {
                auto& v = st2->openFlyouts;
                v.erase(std::remove_if(v.begin(), v.end(), [id](const OpenFlyout& o) { return o.id == id; }), v.end());
                if (v.empty() && st2->pendingRefresh) {
                    st2->pendingRefresh = false;
                    needRefresh = true;
                }
            }
        }
        if (needRefresh) NotifyExplorerBars();
    }),
           [](const auto& e, winrt::event_token t) { e.Closed(t); });
    if (st) {
        for (auto& o : st->openFlyouts) {
            if (o.id == id) o.closedToken = flyClosedToken;
        }
    }

    try {
        fly.ShouldConstrainToRootBounds(false);
    } catch (...) {}

    muxp::FlyoutShowOptions showOpts;
    if (submenu) {
        showOpts.Placement(muxp::FlyoutPlacementMode::RightEdgeAlignedTop);
        showOpts.Position(wf::IReference<wf::Point>(wf::Point{static_cast<float>(target.ActualWidth()), 0.0f}));
    } else {
        showOpts.Placement(muxp::FlyoutPlacementMode::BottomEdgeAlignedLeft);
        showOpts.Position(wf::IReference<wf::Point>(wf::Point{0.0f, static_cast<float>(target.ActualHeight())}));
    }
    fly.ShowAt(target, showOpts);
}

// ============================================================================
// Bar Rendering
// ============================================================================

void RefreshPanel(const muxc::StackPanel& panel, const std::optional<NodePath>& scope) try {
    if (g_unloading) return;

    // A dropdown is open: don't rebuild the bar (that would close it).
    // Refresh dropdown contents in place; submenus below the changed folder close.
    if (auto* openSt = FindBarState(panel); openSt && !openSt->openFlyouts.empty()) {
        openSt->pendingRefresh = true;
        auto copies = openSt->openFlyouts;
        for (auto& of : copies) {
            bool keep = of.folder.size() <= 1 ||
                        (scope.has_value() && !(IsPrefixOf(*scope, of.folder) && of.folder.size() > scope->size()));
            try {
                if (keep) {
                    if (of.rebuild) of.rebuild();
                } else if (of.flyout) {
                    of.flyout.Hide();
                }
            } catch (...) {}
        }
        return;
    }

    panel.Children().Clear();

    muxc::StackPanel row;
    row.Orientation(muxc::Orientation::Horizontal);
    row.Spacing(4);
    row.Margin(mux::Thickness{6, 0, 6, 0});
    panel.Children().Append(row);

    auto nodes = LoadBookmarks();
    auto weakPanel = winrt::make_weak(panel);

    for (size_t i = 0; i < nodes.size(); ++i) {
        const auto& node = nodes[i];
        const bool isFolder = node.type == NodeType::FolderBox;
        muxc::Button btn;
        muxc::StackPanel content;
        content.Orientation(muxc::Orientation::Horizontal);
        content.Spacing(6);

        content.Children().Append(GetShellIconElement(node.path, isFolder));

        muxc::TextBlock lbl;
        lbl.Text(node.name);
        lbl.FontSize(13);
        lbl.MaxWidth(150);
        lbl.VerticalAlignment(mux::VerticalAlignment::Center);
        lbl.TextTrimming(mux::TextTrimming::CharacterEllipsis);
        content.Children().Append(lbl);

        if (isFolder) {
            muxc::FontIcon chevron;
            chevron.Glyph(L"\uE70D");
            chevron.FontFamily(muxm::FontFamily{L"Segoe Fluent Icons"});
            chevron.FontSize(10);
            chevron.Opacity(0.75);
            chevron.VerticalAlignment(mux::VerticalAlignment::Center);
            content.Children().Append(chevron);
        }

        btn.Content(content);
        btn.Height(kButtonHeight);
        btn.Padding(mux::Thickness{10, 0, 10, 0});
        btn.Margin(mux::Thickness{0, kButtonVerticalInset, 4, kButtonVerticalInset});
        btn.AllowDrop(true);

        const int itemIdx = static_cast<int>(i);
        const NodePath nodePath{itemIdx};

        TrackEvent(btn, btn.ContextRequested([nodePath](mux::UIElement const& sender, muxi::ContextRequestedEventArgs const& args) {
            args.Handled(true);
            auto fe = sender.try_as<mux::FrameworkElement>();
            if (!fe) return;
            ShowContextMenuAt(BuildChromeBookmarkContextMenu(fe, nodePath), fe, args);
        }),
           [](const auto& e, winrt::event_token t) { e.ContextRequested(t); });

        TrackAddHandler(btn, 
            mux::UIElement::PointerPressedEvent(),
            winrt::box_value(muxi::PointerEventHandler{
                [weakPanel, row, itemIdx](wf::IInspectable const& sender, muxi::PointerRoutedEventArgs const& args) {
                    auto p = weakPanel.get();
                    auto* st = p ? FindBarState(p) : nullptr;
                    auto b = sender.try_as<muxc::Button>();
                    if (!st || !b) return;
                    auto pt = args.GetCurrentPoint(row);
                    if (pt.Properties().IsLeftButtonPressed()) {
                        ClearHighlightVisual(*st);
                        st->dragSourceIndex = itemIdx;
                        st->dragPointerId = pt.PointerId();
                        st->dragStartPt = pt.Position();
                        st->isDragging = false;
                        st->suppressClickIndex = -1;
                        b.CapturePointer(args.Pointer());
                    }
                }}),
            true);

        TrackAddHandler(btn, 
            mux::UIElement::PointerMovedEvent(),
            winrt::box_value(muxi::PointerEventHandler{
                [weakPanel, row, itemIdx](wf::IInspectable const& sender, muxi::PointerRoutedEventArgs const& args) {
                    auto p = weakPanel.get();
                    auto* st = p ? FindBarState(p) : nullptr;
                    auto b = sender.try_as<muxc::Button>();
                    if (!st || !b || st->dragSourceIndex != itemIdx) return;
                    auto pt = args.GetCurrentPoint(row);
                    if (pt.PointerId() != st->dragPointerId || !pt.Properties().IsLeftButtonPressed()) return;

                    auto pos = pt.Position();
                    float dx = pos.X - st->dragStartPt.X;
                    float dy = pos.Y - st->dragStartPt.Y;
                    if (!st->isDragging && (dx * dx + dy * dy >= kDragThreshold * kDragThreshold)) {
                        st->isDragging = true;
                        st->suppressClickIndex = itemIdx;
                        b.Opacity(0.55);
                    }
                    if (!st->isDragging) return;

                    args.Handled(true);
                    ClearHighlightVisual(*st);

                    auto curNodes = LoadBookmarks();
                    if (static_cast<size_t>(itemIdx) >= curNodes.size()) return;

                    uint32_t childCount = row.Children().Size();
                    for (uint32_t j = 0; j < childCount && j < curNodes.size(); ++j) {
                        auto targetBtn = row.Children().GetAt(j).try_as<muxc::Button>();
                        if (!targetBtn || static_cast<int>(j) == itemIdx) continue;

                        auto localPt = args.GetCurrentPoint(targetBtn).Position();
                        double w = targetBtn.ActualWidth();
                        double h = targetBtn.ActualHeight();
                        if (localPt.X >= -4 && localPt.X <= w + 4 && localPt.Y >= -8 && localPt.Y <= h + 8) {
                            if (curNodes[j].type == NodeType::FolderBox &&
                                localPt.X >= w * 0.12 && localPt.X <= w * 0.88) {
                                ApplyHighlightVisual(*st, targetBtn, mux::Thickness{2, 2, 2, 2});
                                st->dropTargetFolderBoxIndex = static_cast<int>(j);
                                st->dropReorderInsertIndex = -1;
                            } else {
                                bool after = (localPt.X >= w / 2.0);
                                ApplyHighlightVisual(*st, targetBtn,
                                                     after ? mux::Thickness{0, 0, 3, 0} : mux::Thickness{3, 0, 0, 0});
                                st->dropTargetFolderBoxIndex = -1;
                                st->dropReorderInsertIndex = after ? static_cast<int>(j) + 1 : static_cast<int>(j);
                            }
                            break;
                        }
                    }
                }}),
            true);

        TrackAddHandler(btn, 
            mux::UIElement::PointerReleasedEvent(),
            winrt::box_value(muxi::PointerEventHandler{
                [weakPanel, itemIdx](wf::IInspectable const& sender, muxi::PointerRoutedEventArgs const& args) {
                    auto p = weakPanel.get();
                    auto* st = p ? FindBarState(p) : nullptr;
                    auto b = sender.try_as<muxc::Button>();
                    if (!st || !b || st->dragSourceIndex != itemIdx) return;

                    b.ReleasePointerCapture(args.Pointer());
                    b.Opacity(1.0);

                    bool wasDragging = st->isDragging;
                    int targetBox = st->dropTargetFolderBoxIndex;
                    int reorderIdx = st->dropReorderInsertIndex;

                    ClearHighlightVisual(*st);
                    st->dragSourceIndex = -1;
                    st->isDragging = false;

                    if (!wasDragging) return;
                    args.Handled(true);

                    auto list = LoadBookmarks();
                    if (itemIdx < 0 || static_cast<size_t>(itemIdx) >= list.size()) return;
                    std::vector<NodePath> src{NodePath{itemIdx}};

                    if (targetBox >= 0 && targetBox != itemIdx) {
                        if (MoveNodes(list, src, NodePath{targetBox}, -1)) {
                            g_pendingScope = NodePath{};
                            SaveBookmarks(list);
                        }
                        return;
                    }
                    if (reorderIdx >= 0 && reorderIdx != itemIdx && reorderIdx != itemIdx + 1) {
                        if (MoveNodes(list, src, NodePath{}, reorderIdx)) {
                            g_pendingScope = NodePath{};
                            SaveBookmarks(list);
                        }
                    }
                }}),
            true);

        if (!isFolder) {
            muxc::ToolTipService::SetToolTip(btn, winrt::box_value(node.name + L"\n" + node.path));
            std::wstring targetPath = node.path;
            TrackEvent(btn, btn.Click([weakPanel, itemIdx, targetPath](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
                if (auto p = weakPanel.get()) {
                    if (auto* st = FindBarState(p)) {
                        if (st->suppressClickIndex == itemIdx) {
                            st->suppressClickIndex = -1;
                            return;
                        }
                    }
                }
                const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                auto fe = sender.try_as<mux::FrameworkElement>();
                HWND frame = ExplorerWindowForElement(fe);
                QueueOpenTargets(frame, {targetPath}, ctrl ? OpenDisposition::NewTab : OpenDisposition::CurrentTab);
            }),
           [](const auto& e, winrt::event_token t) { e.Click(t); });
        } else {
            muxc::ToolTipService::SetToolTip(btn, winrt::box_value(node.name));

            // External File Explorer drag & drop onto this folder
            const std::wstring dropCaption = L"Add to folder (" + node.name + L")";
            TrackEvent(btn, btn.DragOver([dropCaption](wf::IInspectable const&, mux::DragEventArgs const& args) {
                if (args.DataView().Contains(wdt::StandardDataFormats::StorageItems())) {
                    args.AcceptedOperation(wdt::DataPackageOperation::Link);
                    args.DragUIOverride().Caption(dropCaption);
                    args.DragUIOverride().IsCaptionVisible(true);
                    args.Handled(true);
                }
            }),
           [](const auto& e, winrt::event_token t) { e.DragOver(t); });

            TrackEvent(btn, btn.Drop([itemIdx](wf::IInspectable const& sender, mux::DragEventArgs const& args) {
                args.Handled(true);
                if (!args.DataView().Contains(wdt::StandardDataFormats::StorageItems())) return;
                HWND frame = nullptr;
                if (auto fe = sender.try_as<mux::FrameworkElement>()) frame = ExplorerWindowForElement(fe);
                ReadDroppedPaths(args, frame, NodePath{itemIdx});
            }),
           [](const auto& e, winrt::event_token t) { e.Drop(t); });

            TrackEvent(btn, btn.Click([weakPanel, itemIdx](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
                if (auto p = weakPanel.get()) {
                    if (auto* st = FindBarState(p)) {
                        if (st->suppressClickIndex == itemIdx) {
                            st->suppressClickIndex = -1;
                            return;
                        }
                    }
                }
                auto b = sender.try_as<muxc::Button>();
                if (!b) return;
                g_flyoutOpenByKeyboard = b.FocusState() == mux::FocusState::Keyboard;
                ShowFolderFlyout(weakPanel, NodePath{itemIdx}, b, b, false);
            }),
           [](const auto& e, winrt::event_token t) { e.Click(t); });
        }

        row.Children().Append(btn);
    }
} catch (...) {
    Wh_Log(L"RefreshPanel failed");
}

void RefreshThreadBars() {
    if (!g_bars) return;
    std::optional<NodePath> scope = g_pendingScope;
    g_pendingScope.reset();
    for (const auto& bar : *g_bars) {
        if (auto panel = bar.buttons.get()) {
            RefreshPanel(panel, scope);
        }
    }
}

void NotifyExplorerBars() {
    if (g_unloading) return;
    EnumWindows([](HWND w, LPARAM) -> BOOL {
        if (GetPropW(w, kFrameProperty)) {
            PostMessageW(w, RefreshBarsMessage(), 0, 0);
        } else {
            wchar_t cls[64]{};
            if (GetClassNameW(w, cls, ARRAYSIZE(cls)) && wcscmp(cls, kMgrClass) == 0) {
                PostMessageW(w, kMgrRefreshMsg, 0, 0);
            }
        }
        return TRUE;
    }, 0);
}

// External Drag & Drop onto the bar
void OnBarDragOver(const mux::DragEventArgs& args) {
    if (g_unloading || !args.DataView().Contains(wdt::StandardDataFormats::StorageItems())) {
        args.AcceptedOperation(wdt::DataPackageOperation::None);
        return;
    }
    args.AcceptedOperation(wdt::DataPackageOperation::Link);
    args.DragUIOverride().Caption(L"Add to Bookmarks Bar");
    args.DragUIOverride().IsCaptionVisible(true);
}

void OnBarDrop(const mux::DragEventArgs& args, HWND frame) {
    if (g_unloading || !args.DataView().Contains(wdt::StandardDataFormats::StorageItems())) return;
    ReadDroppedPaths(args, frame, NodePath{});
}

muxc::Grid FindNavigationGrid(const mux::DependencyObject& root, int depth) {
    if (!root || depth > 64) return nullptr;
    if (auto grid = root.try_as<muxc::Grid>(); grid && grid.Name() == L"NavigationBarControlGrid") {
        return grid;
    }
    int count = muxm::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        if (auto grid = FindNavigationGrid(muxm::VisualTreeHelper::GetChild(root, i), depth + 1)) {
            return grid;
        }
    }
    return nullptr;
}

// Removes all bars of this thread from Explorer and restores the original layout.
// Required on unload: otherwise the bar's event handlers (mod code) could run after
// the mod is unloaded and crash Explorer.
// Revokes a dropdown's Opened/Closed handlers, then hides it.
void CloseTrackedFlyout(OpenFlyout& f) {
    if (!f.flyout) return;
    try {
        if (f.openedToken) f.flyout.Opened(f.openedToken);
    } catch (...) {}
    try {
        if (f.closedToken) f.flyout.Closed(f.closedToken);
    } catch (...) {}
    f.openedToken = {};
    f.closedToken = {};
    try {
        f.flyout.Hide();
    } catch (...) {}
}

// Posts a WM_SIZE with the window's current size, so Explorer lays out its
// header again after the current XAML layout pass (no mod code runs for it).
void PostFrameRelayout(HWND window) {
    if (!window || !IsWindow(window) || IsIconic(window)) return;
    RECT rc{};
    if (GetClientRect(window, &rc)) {
        PostMessageW(window, WM_SIZE, IsZoomed(window) ? SIZE_MAXIMIZED : SIZE_RESTORED,
                     MAKELPARAM(rc.right, rc.bottom));
    }
}

void PostThreadFramesRelayout() {
    for (HWND w : g_frames) PostFrameRelayout(w);
}

// The header island is made taller (DesiredSizeHook) only while this thread
// really shows a bar. When that changes, the windows are laid out again, so an
// island never keeps extra height without a bar in it (a black strip).
void UpdateFrameRowCount() {
    unsigned rows = 0;
    if (g_bars) {
        for (const auto& st : *g_bars) {
            if (st.strip.get()) {
                rows = 1;
                break;
            }
        }
    }
    if (rows != g_frameRows) {
        g_frameRows = rows;
        if (!g_unloading) PostThreadFramesRelayout();
    }
}

// Removes one bar from Explorer's header and restores what was changed. Values
// are restored only if they are still the ones the mod set, so a header that
// Explorer rebuilt or changed meanwhile is left alone.
void RemoveBarVisuals(BarState& st) {
    auto root = st.barRoot.get();
    if (root) {
        try {
            if (st.rootDragOverToken) root.DragOver(st.rootDragOverToken);
            if (st.rootDropToken) root.Drop(st.rootDropToken);
        } catch (...) {}
    }
    st.rootDragOverToken = {};
    st.rootDropToken = {};
    if (auto grid = st.grid.get()) {
        try {
            if (root) {
                uint32_t idx = 0;
                if (grid.Children().IndexOf(root, idx)) grid.Children().RemoveAt(idx);
            }
            auto rows = grid.RowDefinitions();
            if (st.addedRow) {
                uint32_t r = 0;
                if (rows.IndexOf(st.addedRow, r)) rows.RemoveAt(r);
            }
            if (st.createdFirstRow && rows.Size() == 1) rows.Clear();
            if (grid.MinHeight() == st.appliedGridMinHeight) grid.MinHeight(st.oldGridMinHeight);
            grid.InvalidateMeasure();
        } catch (...) {}
    }
    if (auto nav = st.navControl.get()) {
        try {
            if (nav.MinHeight() == st.appliedNavMinHeight) nav.MinHeight(st.oldNavMinHeight);
            nav.InvalidateMeasure();
        } catch (...) {}
    }
    if (auto host = st.hostGrid.get()) {
        try {
            auto rows = host.RowDefinitions();
            if (st.commandRow && rows.Size() == 3 && rows.GetAt(2) == st.commandRow &&
                st.commandRow.Height().GridUnitType == mux::GridUnitType::Auto) {
                st.commandRow.Height(st.oldCommandRowHeight);
            }
            host.InvalidateMeasure();
        } catch (...) {}
    }
    st.grid = nullptr;
    st.hostGrid = nullptr;
    st.navControl = nullptr;
    st.barRoot = nullptr;
    st.strip = nullptr;
    st.buttons = nullptr;
    st.addedRow = nullptr;
    st.commandRow = nullptr;
    st.createdFirstRow = false;
}

void RemoveThreadBars() {
    CloseOpenContextMenu();
    if (!g_bars) return;
    for (auto& st : *g_bars) {
        try {
            for (auto& f : st.openFlyouts) CloseTrackedFlyout(f);
            st.openFlyouts.clear();

            if (auto cb = st.commandBar.get(); cb && st.loadedToken) {
                cb.Loaded(st.loadedToken);  // revoke
            }
            RemoveBarVisuals(st);
        } catch (...) {}
    }
    try {
        RevokeThreadHandlers();
    } catch (...) {}
    try {
        RestoreNudgedIslands();
    } catch (...) {}
    g_bars->clear();
    g_frames.clear();  // windows get subclassed again if the mod is re-enabled
    g_frameRows = 0;
    ClearIconCache();
    g_frameTasks.clear();
}

// Unhooks this thread's message hook and forgets it.
void RemoveThreadMessageHook() {
    if (!g_messageHook) return;
    UnhookWindowsHookEx(g_messageHook);
    {
        std::lock_guard lock(g_hooksMutex);
        std::erase(g_allHooks, g_messageHook);
    }
    g_messageHook = nullptr;
}

// Called when an Explorer window is destroyed. Drops that window's bar state;
// when it was the thread's last tracked window, releases everything the thread
// holds (bars, icon cache, pending tasks, message hook). The window's XAML is
// already going away, so the layout is not restored here.
void CleanupClosedFrameState(HWND window) {
    std::erase(g_frames, window);
    CloseOpenContextMenu();
    if (g_bars) {
        for (auto it = g_bars->begin(); it != g_bars->end();) {
            if (it->frame == window || (g_frames.empty())) {
                for (auto& f : it->openFlyouts) CloseTrackedFlyout(f);
                it = g_bars->erase(it);
            } else {
                ++it;
            }
        }
    }
    if (!g_frames.empty()) {
        UpdateFrameRowCount();
        PruneHandlerRevokers();
        return;
    }
    try {
        RevokeThreadHandlers();
        RestoreNudgedIslands();
    } catch (...) {}
    g_frameRows = 0;
    ClearIconCache();
    g_frameTasks.clear();
    g_pendingScope.reset();
    RemoveThreadMessageHook();
}

static LRESULT FrameSubclassProcImpl(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == UninitBarsMessage()) {
        RemoveThreadBars();
        KillTimer(window, kOpenTimerId);
        KillTimer(window, kBackdropTimerId);
        RemoveThreadMessageHook();
        // Ask Explorer to recalculate its stock header now that the bar is gone
        if (!IsIconic(window)) {
            SetWindowPos(window, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            PostFrameRelayout(window);
        }
        return 0;
    }
    if (message == RefreshBarsMessage() && !g_unloading) {
        RefreshThreadBars();
        return 0;
    }
    if (message == FrameTaskMessage()) {
        if (!g_unloading) RunFrameTasks();
        return 0;
    }
    if (message == OpenQueueMessage()) {
        if (!g_unloading) ProcessOpenQueue(window);
        return 0;
    }
    if (message == WM_TIMER && wParam == kOpenTimerId) {
        KillTimer(window, kOpenTimerId);
        if (!g_unloading) ProcessOpenQueue(window);
        return 0;
    }
    if (message == WM_TIMER && wParam == kBackdropTimerId) {
        KillTimer(window, kBackdropTimerId);
        // The header became taller after Explorer had set up the window's
        // backdrop (Mica) for the stock height; in light mode the line under
        // the command bar then stays black until the window is deactivated.
        // This notification makes Explorer set the backdrop up again.
        if (!g_unloading) {
            LRESULT r = DefSubclassProc(window, WM_DWMCOMPOSITIONCHANGED, 0, 0);
            (void)r;
            RedrawWindow(window, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
        }
        return 0;
    }
    if (message == WM_NCDESTROY) {
        RemovePropW(window, kFrameProperty);
        KillTimer(window, kOpenTimerId);
        KillTimer(window, kBackdropTimerId);
        {
            std::lock_guard lock(g_openMutex);
            g_openQueues.erase(window);
        }
        try {
            CleanupClosedFrameState(window);
        } catch (...) {
            Wh_Log(L"CleanupClosedFrameState: exception");
        }
    }
    return DefSubclassProc(window, message, wParam, lParam);
}

LRESULT CALLBACK FrameSubclassProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    try {
        return FrameSubclassProcImpl(window, message, wParam, lParam);
    } catch (...) {
        Wh_Log(L"FrameSubclassProc: exception (msg %u)", message);
        return DefSubclassProc(window, message, wParam, lParam);
    }
}

bool TrackFrame(HWND window) {
    if (!window || GetWindowThreadProcessId(window, nullptr) != GetCurrentThreadId()) return false;
    if (std::find(g_frames.begin(), g_frames.end(), window) != g_frames.end()) return true;
    g_frames.push_back(window);
    WindhawkUtils::SetWindowSubclassFromAnyThread(window, FrameSubclassProc, 0);
    SetPropW(window, kFrameProperty, reinterpret_cast<HANDLE>(1));
    return true;
}

void TryInstallBar(const muxc::CommandBar& commandBar) {
    BarState* state = nullptr;
    try {
        if (g_unloading || !g_frameHooked) return;
        auto root = commandBar.XamlRoot();
        if (!root || !root.Content()) return;
        auto grid = FindNavigationGrid(root.Content(), 0);
        if (!grid) return;

        for (auto& item : *g_bars) {
            if (item.commandBar.get() == commandBar) {
                state = &item;
                break;
            }
        }
        if (!state || (state->strip.get() && state->grid.get() == grid)) return;
        if (HWND frame = IslandWindow(commandBar); TrackFrame(frame)) {
            state->frame = frame;
        }

        for (const auto& child : grid.Children()) {
            auto el = child.try_as<mux::FrameworkElement>();
            if (el && el.Name() == kBarName) return;
        }
        if (state->addedRow || state->strip.get()) {
            // Explorer rebuilt its navigation bar (e.g. after a theme change):
            // undo the stale installation before adding the bar to the new one
            for (auto& f : state->openFlyouts) CloseTrackedFlyout(f);
            state->openFlyouts.clear();
            RemoveBarVisuals(*state);
        }

        auto nav = muxm::VisualTreeHelper::GetParent(grid).try_as<mux::FrameworkElement>();
        auto host = nav ? muxm::VisualTreeHelper::GetParent(nav).try_as<muxc::Grid>() : nullptr;
        if (!host) return;
        auto hostRows = host.RowDefinitions();
        // The known Explorer header has rows [Auto, *, *] with the navigation bar
        // in row 1. Any other layout (another Windows build) is left untouched.
        if (hostRows.Size() != 3 || muxc::Grid::GetRow(nav) != 1 ||
            hostRows.GetAt(0).Height().GridUnitType != mux::GridUnitType::Auto ||
            hostRows.GetAt(1).Height().GridUnitType != mux::GridUnitType::Star ||
            hostRows.GetAt(2).Height().GridUnitType != mux::GridUnitType::Star) {
            return;
        }

        const double origGridH = std::max<double>(grid.ActualHeight(), grid.DesiredSize().Height);
        const double origNavH = std::max<double>(nav.ActualHeight(), nav.DesiredSize().Height);

        muxc::RowDefinition row;
        row.Height(mux::GridLength{kRowHeight, mux::GridUnitType::Pixel});
        auto rows = grid.RowDefinitions();
        state->grid = winrt::make_weak(grid);
        if (rows.Size() == 0) {
            muxc::RowDefinition origRow;
            rows.Append(origRow);
            state->createdFirstRow = true;
        }
        unsigned rowIndex = rows.Size();
        state->hostGrid = winrt::make_weak(host);
        state->addedRow = row;
        rows.Append(row);

        state->commandRow = hostRows.GetAt(2);
        state->oldCommandRowHeight = state->commandRow.Height();
        state->commandRow.Height(mux::GridLength{1.0, mux::GridUnitType::Auto});

        muxc::StackPanel buttons;
        buttons.Orientation(muxc::Orientation::Horizontal);

        muxc::ScrollViewer strip;
        strip.Height(kRowHeight);
        strip.HorizontalScrollBarVisibility(muxc::ScrollBarVisibility::Hidden);
        strip.VerticalScrollBarVisibility(muxc::ScrollBarVisibility::Disabled);
        strip.HorizontalScrollMode(muxc::ScrollMode::Enabled);
        strip.VerticalScrollMode(muxc::ScrollMode::Disabled);
        strip.Content(buttons);

        muxc::Grid barRoot;
        barRoot.Name(kBarName);
        barRoot.Height(kRowHeight);
        barRoot.Background(muxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
        barRoot.AllowDrop(true);
        barRoot.Children().Append(strip);

        TrackEvent(barRoot, barRoot.ContextRequested([](mux::UIElement const& sender, muxi::ContextRequestedEventArgs const& args) {
            args.Handled(true);
            auto fe = sender.try_as<mux::FrameworkElement>();
            if (!fe) return;
            ShowContextMenuAt(BuildChromeBookmarkContextMenu(fe, NodePath{}), fe, args);
        }),
           [](const auto& e, winrt::event_token t) { e.ContextRequested(t); });

        muxm::TranslateTransform barOffset;
        barOffset.Y(-kRowOpticalLift);
        barRoot.RenderTransform(barOffset);
        muxc::Grid::SetRow(barRoot, static_cast<int>(rowIndex));
        muxc::Grid::SetColumnSpan(barRoot, static_cast<int>(std::max(1u, grid.ColumnDefinitions().Size())));

        state->navControl = winrt::make_weak(nav);
        state->barRoot = winrt::make_weak(barRoot);
        state->strip = winrt::make_weak(strip);
        state->buttons = winrt::make_weak(buttons);
        state->oldGridMinHeight = grid.MinHeight();
        state->appliedGridMinHeight = std::max(state->oldGridMinHeight, origGridH + kRowHeight);
        grid.MinHeight(state->appliedGridMinHeight);

        if (nav) {
            state->oldNavMinHeight = nav.MinHeight();
            state->appliedNavMinHeight = std::max(state->oldNavMinHeight, origNavH + kRowHeight);
            nav.MinHeight(state->appliedNavMinHeight);
        }

        state->rootDragOverToken =
            TrackEvent(barRoot, barRoot.DragOver([](wf::IInspectable const&, mux::DragEventArgs const& args) { OnBarDragOver(args); }),
           [](const auto& e, winrt::event_token t) { e.DragOver(t); });
        state->rootDropToken = TrackEvent(barRoot, barRoot.Drop([](wf::IInspectable const& sender, mux::DragEventArgs const& args) {
            HWND frame = nullptr;
            if (auto fe = sender.try_as<mux::FrameworkElement>()) frame = ExplorerWindowForElement(fe);
            OnBarDrop(args, frame);
        }),
           [](const auto& e, winrt::event_token t) { e.Drop(t); });

        grid.Children().Append(barRoot);
        grid.InvalidateMeasure();
        host.InvalidateMeasure();

        RefreshPanel(buttons, std::nullopt);
        UpdateFrameRowCount();
        // This window's island must take the new height even when the thread
        // already showed a bar in another window
        PostFrameRelayout(state->frame);
        if (state->frame) SetTimer(state->frame, kBackdropTimerId, 300, nullptr);
    } catch (...) {
        Wh_Log(L"TryInstallBar failed: %08X", static_cast<unsigned>(winrt::to_hresult().value));
        if (state) {
            try {
                RemoveBarVisuals(*state);
            } catch (...) {}
            UpdateFrameRowCount();
        }
    }
}

// Is this element, or one of its parents, a text field?
bool IsInsideTextField(mux::DependencyObject element) {
    for (int depth = 0; element && depth < 12; ++depth) {
        if (element.try_as<muxc::TextBox>() || element.try_as<muxc::RichEditBox>() ||
            element.try_as<muxc::PasswordBox>() || element.try_as<muxc::AutoSuggestBox>()) {
            return true;
        }
        element = muxm::VisualTreeHelper::GetParent(element);
    }
    return false;
}

// Is the keyboard focus inside File Explorer's XAML (address bar, search box,
// tabs, command bar) rather than in the file list or navigation pane?
bool IsXamlHostWindow(HWND focus, HWND frame) {
    for (HWND h = focus; h && h != frame; h = GetParent(h)) {
        wchar_t cls[96]{};
        if (!GetClassNameW(h, cls, ARRAYSIZE(cls))) continue;
        if (wcsncmp(cls, L"Microsoft.UI.", 13) == 0 || wcsstr(cls, L"InputSite") ||
            wcsstr(cls, L"DesktopWindowXamlSource")) {
            return true;
        }
    }
    return false;
}

// Decides whether Ctrl+B is left to the focused control. True in a Win32 edit
// box (e.g. renaming a file), in a XAML text field (address bar, search box),
// and in the XAML part of the window when the focused element can't be found.
bool IsTextInputFocused(HWND focus, HWND frame) {
    wchar_t cls[64]{};
    if (focus && GetClassNameW(focus, cls, ARRAYSIZE(cls)) &&
        (_wcsicmp(cls, L"Edit") == 0 || _wcsnicmp(cls, L"RichEdit", 8) == 0)) {
        return true;
    }
    if (!IsXamlHostWindow(focus, frame)) return false;
    bool found = false;
    try {
        if (g_bars) {
            for (const auto& state : *g_bars) {
                auto bar = state.commandBar.get();
                auto root = bar ? bar.XamlRoot() : nullptr;
                if (!root) continue;
                auto focused = muxi::FocusManager::GetFocusedElement(root).try_as<mux::DependencyObject>();
                if (!focused) continue;
                found = true;
                if (IsInsideTextField(focused)) return true;
            }
        }
        if (auto focused = muxi::FocusManager::GetFocusedElement().try_as<mux::DependencyObject>()) {
            found = true;
            if (IsInsideTextField(focused)) return true;
        }
    } catch (...) {}
    return !found;
}

static LRESULT ThreadMessageHookImpl(int code, WPARAM wParam, LPARAM lParam) {
    auto msg = reinterpret_cast<MSG*>(lParam);

    // Bookmark Manager: Tab / Shift+Tab moves focus between controls. Explorer's
    // message loop doesn't call IsDialogMessage and routes Tab to its own (XAML)
    // tab bar, so the key is intercepted here and handled manually.
    if (code == HC_ACTION && wParam == PM_REMOVE && !g_unloading &&
        msg->message == WM_KEYDOWN && (msg->wParam == VK_TAB || msg->wParam == VK_RETURN)) {
        auto isMgr = [](HWND h) {
            HWND r = h ? GetAncestor(h, GA_ROOT) : nullptr;
            wchar_t c[64] = {};
            return r && GetClassNameW(r, c, ARRAYSIZE(c)) && wcscmp(c, kMgrClass) == 0;
        };
        GUITHREADINFO gti{};
        gti.cbSize = sizeof(gti);
        const bool inMenu = GetGUIThreadInfo(GetCurrentThreadId(), &gti) &&
                            (gti.flags & (GUI_INMENUMODE | GUI_POPUPMENUMODE));
        if (inMenu) {
            // A manager menu (⋮ or right-click) is open: Tab only closes the menu and
            // doesn't move focus. Enter is handled by the menu itself.
            if (msg->wParam == VK_TAB && isMgr(gti.hwndMenuOwner)) {
                msg->message = WM_NULL;
                EndMenu();
                return CallNextHookEx(nullptr, code, wParam, lParam);
            }
        } else if (msg->wParam == VK_RETURN) {
            // In a regular (non-dialog) window Enter doesn't click buttons:
            // Enter on a focused button (⋮, Delete, ✕) acts as a click.
            HWND f = GetFocus();
            wchar_t fc[32] = {};
            if (f && isMgr(f) && GetClassNameW(f, fc, ARRAYSIZE(fc)) && _wcsicmp(fc, L"Button") == 0) {
                msg->message = WM_NULL;
                // Posted, so the menu's modal loop doesn't run inside the hook
                const int id = GetDlgCtrlID(f);
                if (id == IDC_MGR_MORE) g_mgrMenuByKeyboard = true;
                PostMessageW(GetParent(f), WM_COMMAND, MAKEWPARAM(id, BN_CLICKED), reinterpret_cast<LPARAM>(f));
                return CallNextHookEx(nullptr, code, wParam, lParam);
            }
        }
    }

    if (code == HC_ACTION && wParam == PM_REMOVE && !g_unloading && msg->hwnd &&
        msg->message == WM_KEYDOWN && msg->wParam == VK_TAB &&
        !(GetKeyState(VK_CONTROL) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) {
        HWND root = GetAncestor(msg->hwnd, GA_ROOT);
        wchar_t cls[64] = {};
        if (root && GetClassNameW(root, cls, ARRAYSIZE(cls)) && wcscmp(cls, kMgrClass) == 0) {
            msg->message = WM_NULL;
            const bool back = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
            HWND cur = GetFocus();
            if (!cur || (cur != root && !IsChild(root, cur))) cur = nullptr;
            if (cur == root) cur = nullptr;
            HWND next = GetNextDlgTabItem(root, cur, back ? TRUE : FALSE);
            if (next) {
                SetFocus(next);
                wchar_t ncls[32] = {};
                if (GetClassNameW(next, ncls, ARRAYSIZE(ncls)) && _wcsicmp(ncls, L"Edit") == 0) {
                    SendMessageW(next, EM_SETSEL, 0, -1);
                } else if (_wcsicmp(ncls, WC_LISTVIEWW) == 0) {
                    // Tabbed into the bookmark list with nothing selected:
                    // select the first (or focused) bookmark right away
                    int count = ListView_GetItemCount(next);
                    if (count > 0 && ListView_GetSelectedCount(next) == 0) {
                        int item = ListView_GetNextItem(next, -1, LVNI_FOCUSED);
                        if (item < 0) item = 0;
                        ListView_SetItemState(next, item, LVIS_SELECTED | LVIS_FOCUSED,
                                              LVIS_SELECTED | LVIS_FOCUSED);
                        ListView_EnsureVisible(next, item, FALSE);
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        }
    }

    if (code == HC_ACTION && wParam == PM_REMOVE && !g_unloading && g_bookmarkHotkey &&
        msg->message == WM_KEYDOWN && msg->wParam == 'B' &&
        !(msg->lParam & (1 << 30)) && (GetKeyState(VK_CONTROL) & 0x8000) &&
        !(GetKeyState(VK_SHIFT) & 0x8000) && !(GetKeyState(VK_MENU) & 0x8000)) {
        HWND frame = msg->hwnd ? GetAncestor(msg->hwnd, GA_ROOT) : nullptr;
        // While typing (address bar, search box, rename box) Ctrl+B is left alone
        if (IsExplorerFrame(frame) && !IsTextInputFocused(msg->hwnd, frame)) {
            msg->message = WM_NULL;
            auto sel = SelectedItems(frame);
            if (sel.empty()) {
                auto cur = CurrentFolder(frame);
                if (!cur.empty()) sel.push_back(cur);
            }
            if (!sel.empty()) {
                // Posted: the duplicate question must not run inside this hook
                PostToFrame(frame, [frame, sel]() { AddPathsAsBookmarks(frame, NodePath{}, sel); });
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT CALLBACK ThreadMessageHook(int code, WPARAM wParam, LPARAM lParam) {
    try {
        return ThreadMessageHookImpl(code, wParam, lParam);
    } catch (...) {
        Wh_Log(L"ThreadMessageHook: exception");
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }
}

void TrackCommandBar(const muxc::CommandBar& commandBar) try {
    if (g_unloading || !commandBar || commandBar.Name() != L"FileExplorerCommandBar") return;
    for (const auto& state : *g_bars) {
        if (state.commandBar.get() == commandBar) {
            TryInstallBar(commandBar);
            return;
        }
    }
    // Tabs come and go: forget bars whose command bar and strip are both gone
    g_bars->remove_if([](BarState& st) {
        if (st.commandBar.get() || st.strip.get()) return false;
        for (auto& f : st.openFlyouts) CloseTrackedFlyout(f);
        return true;
    });
    g_bars->emplace_back();
    BarState& state = g_bars->back();
    state.commandBar = winrt::make_weak(commandBar);
    auto weakBar = state.commandBar;
    state.loadedToken = TrackEvent(commandBar, commandBar.Loaded([weakBar](wf::IInspectable const&, mux::RoutedEventArgs const&) {
        if (auto bar = weakBar.get()) TryInstallBar(bar);
    }),
           [](const auto& e, winrt::event_token t) { e.Loaded(t); });
    TryInstallBar(commandBar);
    if (!g_messageHook) {
        g_messageHook = SetWindowsHookExW(WH_GETMESSAGE, ThreadMessageHook, nullptr, GetCurrentThreadId());
        if (g_messageHook) {
            std::lock_guard lock(g_hooksMutex);
            g_allHooks.push_back(g_messageHook);
        }
    }
} catch (...) {}

// ============================================================================
// Already-open Explorer windows: attach without restart
// ============================================================================
// The command bar hook only sees windows created after the mod is loaded. For
// windows that are already open, each Explorer UI thread is searched, in order:
//   1. starting from a command bar the thread already knows (another window);
//   2. starting from the XAML element that has keyboard focus;
//   3. by widening the window's XAML island by one pixel for a moment. Explorer's
//      command bar reacts to that in its XamlRoot "Changed" handler (OnDpiChanged),
//      and a hook on that handler hands over the XamlRoot to search.
// Hooks on the command bar's GotFocus and OnApplyTemplate catch any window that
// is still missed as soon as it is used. This approach follows the one used by
// Explorer Command Bar by DanRotaru (MIT license).

void CollectCommandBars(const mux::DependencyObject& element, int depth, std::vector<muxc::CommandBar>& bars) {
    if (!element || depth > 64) return;
    if (auto bar = element.try_as<muxc::CommandBar>(); bar && bar.Name() == L"FileExplorerCommandBar") {
        bars.push_back(bar);
        return;
    }
    const int count = muxm::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        CollectCommandBars(muxm::VisualTreeHelper::GetChild(element, i), depth + 1, bars);
    }
}

void ScanXamlRoot(const mux::XamlRoot& root) try {
    if (g_unloading || !root) return;
    auto content = root.Content();
    if (!content) return;
    std::vector<muxc::CommandBar> bars;
    CollectCommandBars(content, 0, bars);
    for (const auto& bar : bars) TrackCommandBar(bar);
} catch (...) {
    Wh_Log(L"ScanXamlRoot: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
}

void ScanXamlRootOf(const mux::UIElement& element) {
    if (element) ScanXamlRoot(element.XamlRoot());
}

bool IsXamlRootKnownForCurrentThread(const mux::XamlRoot& root) {
    if (!root || !g_bars) return false;
    for (const auto& state : *g_bars) {
        if (auto bar = state.commandBar.get(); bar && bar.XamlRoot() == root) return true;
    }
    return false;
}

bool IsWindowKnownForCurrentThread(HWND window) {
    if (!window || !g_bars) return false;
    for (const auto& state : *g_bars) {
        if (state.frame == window) return true;
        if (auto bar = state.commandBar.get(); bar && IslandWindow(bar) == window) return true;
    }
    return false;
}

void ScanCurrentThreadForCommandBars() try {
    if (g_unloading || !g_bars) return;
    std::vector<winrt::weak_ref<muxc::CommandBar>> known;
    for (const auto& state : *g_bars) known.push_back(state.commandBar);
    for (const auto& weak : known) {
        if (auto bar = weak.get()) ScanXamlRootOf(bar);
    }
    auto focused = muxi::FocusManager::GetFocusedElement();
    if (auto element = focused ? focused.try_as<mux::UIElement>() : nullptr) ScanXamlRootOf(element);
} catch (...) {
    Wh_Log(L"ScanCurrentThreadForCommandBars: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
}

// --- Step 3: one-pixel island nudge ---------------------------------------

struct IslandNudge {
    HWND island;
    int width;
    int height;
};
thread_local std::vector<IslandNudge> g_islandNudges;
thread_local UINT_PTR g_nudgeTimer = 0;

// Puts nudged islands back to their size (unless something else resized them
// meanwhile) and stops the timer. Also called on unload and window close.
void RestoreNudgedIslands() {
    if (g_nudgeTimer) {
        KillTimer(nullptr, g_nudgeTimer);
        g_nudgeTimer = 0;
    }
    auto taken = std::move(g_islandNudges);
    g_islandNudges.clear();
    for (const auto& nudge : taken) {
        RECT rc{};
        if (!IsWindow(nudge.island) || !GetWindowRect(nudge.island, &rc)) continue;
        if (rc.right - rc.left != nudge.width + 1 || rc.bottom - rc.top != nudge.height) continue;
        SetWindowPos(nudge.island, nullptr, 0, 0, nudge.width, nudge.height,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
}

void CALLBACK NudgeTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    try {
        RestoreNudgedIslands();
    } catch (...) {
        Wh_Log(L"NudgeTimerProc: exception");
    }
}

void NudgeXamlIslandsOfWindow(HWND window) {
    std::vector<HWND> islands;
    EnumChildWindows(window, [](HWND child, LPARAM lp) -> BOOL {
        wchar_t cls[64]{};
        if (GetWindowThreadProcessId(child, nullptr) == GetCurrentThreadId() &&
            GetClassNameW(child, cls, ARRAYSIZE(cls)) &&
            wcscmp(cls, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0) {
            reinterpret_cast<std::vector<HWND>*>(lp)->push_back(child);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&islands));

    for (HWND island : islands) {
        RECT rc{};
        if (!IsWindowVisible(island) || !GetWindowRect(island, &rc)) continue;
        const int width = rc.right - rc.left;
        const int height = rc.bottom - rc.top;
        if (width <= 0 || height <= 0) continue;
        g_islandNudges.push_back({island, width, height});
        SetWindowPos(island, nullptr, 0, 0, width + 1, height,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }
    if (!g_islandNudges.empty() && !g_nudgeTimer) {
        g_nudgeTimer = SetTimer(nullptr, 0, 100, NudgeTimerProc);
        if (!g_nudgeTimer) RestoreNudgedIslands();
    }
}

// Runs on an Explorer UI thread: finds the bar for each File Explorer window
// of this thread, nudging the ones that the scan didn't find.
void DiscoverCurrentThreadWindows() {
    if (g_unloading) return;
    std::vector<HWND> windows;
    EnumThreadWindows(GetCurrentThreadId(), [](HWND w, LPARAM lp) -> BOOL {
        if (IsExplorerFrame(w)) reinterpret_cast<std::vector<HWND>*>(lp)->push_back(w);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    if (windows.empty()) return;
    ScanCurrentThreadForCommandBars();
    for (HWND w : windows) {
        if (!g_unloading && !IsWindowKnownForCurrentThread(w)) {
            Wh_Log(L"Nudging the XAML island of an open File Explorer window");
            NudgeXamlIslandsOfWindow(w);
        }
    }
}

// --- Deferred scans --------------------------------------------------------
// Scans and bar installs never run inside Explorer's own XAML code (template,
// focus and XamlRoot handlers, or the unload/load hook messages). They are
// queued on the thread's DispatcherQueue and run a moment later, when XAML is
// idle. Each queued task holds a token; Wh_ModUninit waits until no token is
// left, whether the task ran or the queue discarded it.

void ScheduleCurrentThreadScan() try {
    if (g_unloading) return;
    auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!queue) return;
    queue.TryEnqueue([token = std::make_shared<PendingScanToken>()]() {
        if (!g_unloading) ScanCurrentThreadForCommandBars();
    });
} catch (...) {
    Wh_Log(L"ScheduleCurrentThreadScan: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
}

void ScheduleXamlRootScan(const mux::XamlRoot& root) try {
    if (g_unloading || !root) return;
    auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!queue) return;
    queue.TryEnqueue([token = std::make_shared<PendingScanToken>(), root]() {
        if (g_unloading) return;
        try {
            if (!IsXamlRootKnownForCurrentThread(root)) ScanXamlRoot(root);
        } catch (...) {
            Wh_Log(L"Queued XamlRoot scan: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
        }
    });
} catch (...) {
    Wh_Log(L"ScheduleXamlRootScan: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
}

// Called through RunOnWindowThread when the mod loads: the discovery itself
// runs from the thread's queue, outside the hook message.
void ScheduleDiscoverCurrentThreadWindows() try {
    if (g_unloading) return;
    auto queue = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
    if (!queue) return;
    queue.TryEnqueue([token = std::make_shared<PendingScanToken>()]() {
        if (g_unloading) return;
        try {
            DiscoverCurrentThreadWindows();
        } catch (...) {
            Wh_Log(L"DiscoverCurrentThreadWindows: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
        }
    });
} catch (...) {
    Wh_Log(L"ScheduleDiscoverCurrentThreadWindows: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
}

// --- Command bar hooks (all optional) ---------------------------------------

using OnApplyTemplate_t = void(WINAPI*)(void*);
OnApplyTemplate_t g_onApplyTemplateOriginal = nullptr;
OnApplyTemplate_t g_onApplyTemplateWave1Original = nullptr;

void WINAPI OnApplyTemplateHook(void* self) {
    g_onApplyTemplateOriginal(self);
    ScheduleCurrentThreadScan();
}

void WINAPI OnApplyTemplateWave1Hook(void* self) {
    g_onApplyTemplateWave1Original(self);
    ScheduleCurrentThreadScan();
}

using GotFocusHandler_t = void(WINAPI*)(void*, void*, void*);
GotFocusHandler_t g_gotFocusOriginal = nullptr;
GotFocusHandler_t g_gotFocusWave1Original = nullptr;

void HandleCommandBarGotFocus(void* sender) {
    if (g_unloading || !sender) return;
    try {
        const auto& inspectable = *reinterpret_cast<const wf::IInspectable*>(sender);
        auto element = inspectable ? inspectable.try_as<mux::UIElement>() : nullptr;
        if (!element) return;
        HWND window = IslandWindow(element);
        if (window && IsWindowKnownForCurrentThread(window)) return;
        ScheduleXamlRootScan(element.XamlRoot());
    } catch (...) {
        Wh_Log(L"HandleCommandBarGotFocus: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
    }
}

void WINAPI GotFocusHook(void* self, void* sender, void* args) {
    g_gotFocusOriginal(self, sender, args);
    HandleCommandBarGotFocus(sender);
}

void WINAPI GotFocusWave1Hook(void* self, void* sender, void* args) {
    g_gotFocusWave1Original(self, sender, args);
    HandleCommandBarGotFocus(sender);
}

// OnDpiChanged is a coroutine member returning a class type, so it gets a hidden
// return pointer after `this`, and its by-value XamlRoot parameter is passed as
// a pointer to a temporary. The XamlRoot is read before the original runs,
// because the coroutine moves it into its frame. It is queried, not assumed, so
// a different layout only makes the hook do nothing.
using OnDpiChanged_t = void*(WINAPI*)(void*, void*, void*, void*);
OnDpiChanged_t g_onDpiChangedOriginal = nullptr;
OnDpiChanged_t g_onDpiChangedWave1Original = nullptr;

mux::XamlRoot XamlRootFromByValueParam(void* param) {
    if (g_unloading || !param) return nullptr;
    try {
        wf::IInspectable inspectable;
        winrt::copy_from_abi(inspectable, *reinterpret_cast<void**>(param));
        return inspectable ? inspectable.try_as<mux::XamlRoot>() : nullptr;
    } catch (...) {
        return nullptr;
    }
}

void HandleCommandBarXamlRootChanged(const mux::XamlRoot& root) {
    if (g_unloading || !root) return;
    try {
        if (!IsXamlRootKnownForCurrentThread(root)) ScheduleXamlRootScan(root);
    } catch (...) {
        Wh_Log(L"HandleCommandBarXamlRootChanged: error %08X", static_cast<unsigned>(winrt::to_hresult().value));
    }
}

void* WINAPI OnDpiChangedHook(void* self, void* ret, void* sender, void* args) {
    auto root = XamlRootFromByValueParam(sender);
    void* result = g_onDpiChangedOriginal(self, ret, sender, args);
    HandleCommandBarXamlRootChanged(root);
    return result;
}

void* WINAPI OnDpiChangedWave1Hook(void* self, void* ret, void* sender, void* args) {
    auto root = XamlRootFromByValueParam(sender);
    void* result = g_onDpiChangedWave1Original(self, ret, sender, args);
    HandleCommandBarXamlRootChanged(root);
    return result;
}

using CommandBarSetter = void(WINAPI*)(void*, void*);
CommandBarSetter g_commandBarSetterOriginal = nullptr;
void WINAPI CommandBarSetterHook(void* self, void* value) {
    g_commandBarSetterOriginal(self, value);
    if (!g_unloading && value) {
        try {
            TrackCommandBar(*reinterpret_cast<muxc::CommandBar*>(value));
        } catch (...) {
            Wh_Log(L"CommandBarSetterHook: exception");
        }
    }
}

using DesiredSizeGetter = HRESULT(WINAPI*)(void*, SIZE*);
DesiredSizeGetter g_desiredSizeOriginal = nullptr;
HRESULT WINAPI DesiredSizeHook(void* self, SIZE* size) {
    HRESULT result = g_desiredSizeOriginal(self, size);
    if (FAILED(result) || !size || g_unloading || g_frameRows == 0) return result;
    const LONG origH = size->cy;
    if (origH >= 80 && origH <= 600) {
        const int extraAt96 = static_cast<int>(kRowHeight) + kNavigationOverhang;
        size->cy += MulDiv(origH, extraAt96, kMeasuredHostHeightAt96Dpi);
    }
    return result;
}

bool HookExplorerFrame(bool apply) {
    if (g_frameHooked) return true;
    HMODULE mod = GetModuleHandleW(L"Windows.UI.FileExplorer.dll");
    if (!mod) return true;
    bool expected = false;
    if (!g_frameHookAttempted.compare_exchange_strong(expected, true)) return g_frameHooked;
    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK fileExplorerFrameHooks[] = {{
        {LR"(public: virtual long __cdecl XamlIslandViewAdapter::get_DesiredSizeInPhysicalPixels(struct tagSIZE *))"},
        &g_desiredSizeOriginal, DesiredSizeHook}};
    if (!WindhawkUtils::HookSymbols(mod, fileExplorerFrameHooks, ARRAYSIZE(fileExplorerFrameHooks)) || !g_desiredSizeOriginal) return false;
    g_frameHooked = true;
    if (apply) Wh_ApplyHookOperations();
    return true;
}

bool HookExplorerExtension(bool apply) {
    if (g_extensionHooked) return true;
    HMODULE mod = GetModuleHandleW(L"FileExplorerExtensions.dll");
    if (!mod) return true;
    bool expected = false;
    if (!g_extensionHookAttempted.compare_exchange_strong(expected, true)) return g_extensionHooked;
    // FileExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsHooks[] = {
        {{LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))"},
         &g_commandBarSetterOriginal, CommandBarSetterHook},
        // The rest only help to find windows that were open before the mod loaded
        {{LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))"},
         &g_onApplyTemplateOriginal, OnApplyTemplateHook, true},
        {{LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))"},
         &g_onApplyTemplateWave1Original, OnApplyTemplateWave1Hook, true},
        {{LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))"},
         &g_gotFocusOriginal, GotFocusHook, true},
        {{LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))"},
         &g_gotFocusWave1Original, GotFocusWave1Hook, true},
        {{LR"(public: struct winrt::fire_and_forget __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnDpiChanged(struct winrt::Microsoft::UI::Xaml::XamlRoot,struct winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs))"},
         &g_onDpiChangedOriginal, OnDpiChangedHook, true},
        {{LR"(public: struct winrt::fire_and_forget __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnDpiChanged(struct winrt::Microsoft::UI::Xaml::XamlRoot,struct winrt::Microsoft::UI::Xaml::XamlRootChangedEventArgs))"},
         &g_onDpiChangedWave1Original, OnDpiChangedWave1Hook, true},
    };
    if (!WindhawkUtils::HookSymbols(mod, fileExplorerExtensionsHooks, ARRAYSIZE(fileExplorerExtensionsHooks)) || !g_commandBarSetterOriginal) return false;
    g_extensionHooked = true;
    if (apply) Wh_ApplyHookOperations();
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t g_loadLibraryOriginal = nullptr;
HMODULE WINAPI LoadLibraryExWHook(LPCWSTR file, HANDLE handle, DWORD flags) {
    HMODULE mod = g_loadLibraryOriginal(file, handle, flags);
    if (mod && file && !g_unloading && (!g_extensionHookAttempted || !g_frameHookAttempted)) {
        try {
            const wchar_t* base = file;
            for (const wchar_t* p = file; *p; ++p) {
                if (*p == L'\\' || *p == L'/') base = p + 1;
            }
            if (_wcsicmp(base, L"Windows.UI.FileExplorer.dll") == 0) HookExplorerFrame(true);
            else if (_wcsicmp(base, L"FileExplorerExtensions.dll") == 0) HookExplorerExtension(true);
        } catch (...) {
            Wh_Log(L"LoadLibraryExWHook: exception");
        }
    }
    return mod;
}

BOOL Wh_ModInit() try {
    // Start from a clean state, in case this module instance is reused after a
    // previous unload.
    g_unloading = false;
    g_frameHooked = false;
    g_extensionHooked = false;
    g_frameHookAttempted = false;
    g_extensionHookAttempted = false;
    {
        std::lock_guard lock(g_dropReadsMutex);
        g_dropReads.emplace();
    }
    g_bookmarkHotkey = Wh_GetIntSetting(L"bookmarkHotkey") != 0;
    g_themeSetting = ReadThemeSetting();
    InitStorage();
    RegisterWindowClasses();
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLib = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW")) : nullptr;
    if (!loadLib || !WindhawkUtils::SetFunctionHook(loadLib, LoadLibraryExWHook, &g_loadLibraryOriginal)) {
        return FALSE;
    }
    if (!GetModuleHandleW(L"Windows.UI.FileExplorer.dll")) {
        LoadLibraryExW(L"Windows.UI.FileExplorer.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    HookExplorerFrame(false);
    HookExplorerExtension(false);
    return TRUE;
} catch (...) {
    Wh_Log(L"Wh_ModInit: exception");
    return FALSE;
}

void Wh_ModAfterInit() try {
    HookExplorerFrame(true);
    HookExplorerExtension(true);
    // Attach to File Explorer windows that are already open
    if (g_extensionHooked && !g_unloading) ForOwnUiThreads(ScheduleDiscoverCurrentThreadWindows);
} catch (...) {
    Wh_Log(L"Wh_ModAfterInit: exception");
}

void Wh_ModSettingsChanged() try {
    g_bookmarkHotkey = Wh_GetIntSetting(L"bookmarkHotkey") != 0;
    g_themeSetting = ReadThemeSetting();
    if (HWND m = FindOwnProcessWindow(kMgrClass)) {
        PostMessageW(m, kMgrThemeMsg, 0, 0);
    }
} catch (...) {
    Wh_Log(L"Wh_ModSettingsChanged: exception");
}

void BeginUnloading() {
    std::lock_guard lock(g_operationMutex);
    g_unloading = true;
}

// Cleanup for threads whose window was never tracked (e.g. TrackFrame failed).
void CleanupCurrentThread() {
    RemoveThreadBars();
    RemoveThreadMessageHook();
}

void Wh_ModBeforeUninit() try {
    BeginUnloading();
} catch (...) {
    Wh_Log(L"Wh_ModBeforeUninit: exception");
}

void Wh_ModUninit() try {
    BeginUnloading();

    // 1. Close every modal UI of this mod on its own thread and wait until none
    //    is left on any thread's stack.
    for (unsigned pass = 0;; ++pass) {
        ForOwnUiThreads(CloseThreadModals);
        std::unique_lock lock(g_operationMutex);
        if (g_activeOperations == 0) break;
        if (pass == 50) Wh_Log(L"Wh_ModUninit: still waiting for %u dialog(s)", g_activeOperations);
        g_operationFinished.wait_for(lock, std::chrono::milliseconds(100));
    }

    // 2. Cancel pending reads of dropped items and wait for their completions.
    for (unsigned pass = 0; g_pendingDrops != 0; ++pass) {
        std::vector<wf::IAsyncInfo> reads;
        {
            std::lock_guard lock(g_dropReadsMutex);
            if (g_dropReads) reads = *g_dropReads;
        }
        for (const auto& read : reads) {
            try {
                read.Cancel();
            } catch (...) {}
        }
        if (pass == 50) Wh_Log(L"Wh_ModUninit: still waiting for %u dropped-item read(s)", g_pendingDrops.load());
        Sleep(100);
    }
    {
        std::lock_guard lock(g_dropReadsMutex);
        g_dropReads.reset();
    }
    for (unsigned pass = 0; g_pendingScans != 0; ++pass) {
        if (pass == 50) Wh_Log(L"Wh_ModUninit: still waiting for %u queued scan(s)", g_pendingScans.load());
        Sleep(100);
    }

    // 3. Close the Bookmark Manager windows (blocking, on their threads).
    for (HWND m; (m = FindOwnProcessWindow(kMgrClass)) != nullptr;) {
        SendMessageW(m, WM_CLOSE, 0, 0);
        if (IsWindow(m)) DestroyWindow(m);  // only works on its own thread; never loops forever
        if (IsWindow(m)) break;
    }

    // 4. Remove the bar from every tracked window (on its own thread), then the
    //    subclass, so nothing calls into mod code after it is unloaded.
    std::vector<HWND> frames;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        if (GetPropW(w, kFrameProperty) && IsOwnProcessWindow(w)) reinterpret_cast<std::vector<HWND>*>(lp)->push_back(w);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&frames));
    for (HWND w : frames) {
        SendMessageW(w, UninitBarsMessage(), 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(w, FrameSubclassProc);
        RemovePropW(w, kFrameProperty);
    }

    // 5. Threads without a tracked window may still hold a bar or a hook.
    ForOwnUiThreads(CleanupCurrentThread);

    // Remove any remaining message hooks
    {
        std::lock_guard lock(g_hooksMutex);
        for (HHOOK h : g_allHooks) UnhookWindowsHookEx(h);
        g_allHooks.clear();
    }

    UnregisterWindowClasses();
} catch (...) {
    Wh_Log(L"Wh_ModUninit: exception");
}

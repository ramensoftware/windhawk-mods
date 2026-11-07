// ==WindhawkMod==
// @id              bookmark-bar-for-file-explorer
// @name            Bookmark Bar for File Explorer
// @description     Adds a Chrome-style bookmarks bar to File Explorer with nested folders, drag & drop and a full Bookmark Manager
// @version         2.5.6
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
- No File Explorer restart is needed: the bar is added to already open
  windows as soon as the mod is enabled, and removed when it is disabled.
- Bookmarks are saved in the mod's own storage and persist across restarts.
- Bookmarks pointing to network locations show a generic icon, so that an
  unreachable server never slows down File Explorer.
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

#include <windows.h>
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
#include <xamlom.h>
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

// Shown in the Bookmark Manager. Keep in sync with @version in the header above.
constexpr wchar_t kModVersion[] = L"2.5.6";

constexpr wchar_t kBarName[] = L"WindhawkExplorerChromeBookmarksBar";
constexpr wchar_t kFrameProperty[] = L"WindhawkExplorerChromeBookmarksFrame";
constexpr wchar_t kStorageKeyBase[] = L"chromeBookmarksJson";
constexpr wchar_t kExportFileName[] = L"explorer-bookmarks-backup.json";
constexpr wchar_t kMgrClass[] = L"WindhawkChromeBookmarkManagerWnd2";
constexpr wchar_t kEditDlgClass[] = L"WindhawkChromeEditBookmarkDlg2";
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
[[clang::no_destroy]] std::vector<std::wstring> g_undoStack;
std::wstring g_storageKey = kStorageKeyBase;
std::atomic<bool> g_unloading = false;
std::atomic<bool> g_extensionHooked = false;
std::atomic<bool> g_frameHooked = false;
std::atomic<bool> g_extensionHookAttempted = false;
std::atomic<bool> g_frameHookAttempted = false;
std::atomic<bool> g_bookmarkHotkey = true;
std::atomic<int> g_themeSetting = 0;  // 0 = system, 1 = light, 2 = dark
// Window classes registered by this load (reset on every load, see Wh_ModInit)
bool g_editClassRegistered = false;
bool g_mgrClassRegistered = false;

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
[[clang::no_destroy]] thread_local std::vector<HWND> g_frames;
[[clang::no_destroy]] thread_local std::deque<std::function<void()>> g_frameTasks;
[[clang::no_destroy]] thread_local std::optional<NodePath> g_pendingScope;

void NotifyExplorerBars();
void RefreshThreadBars();
void RefreshPanel(const muxc::StackPanel& panel, const std::optional<NodePath>& scope);
void OpenBookmarkManagerWindow(HWND ownerExplorer, const NodePath& folder);
void FlushDiagReleasesForCurrentThread();

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

std::vector<BookmarkNode> DeserializeNodes(const std::wstring& jsonText) {
    std::vector<BookmarkNode> result;
    if (jsonText.empty()) return result;
    wjson::JsonObject root{nullptr};
    if (!wjson::JsonObject::TryParse(jsonText, root)) return result;
    try {
        if (root.HasKey(L"nodes")) {
            auto v = root.GetNamedValue(L"nodes");
            if (v.ValueType() == wjson::JsonValueType::Array) JsonToNodes(v.GetArray(), result, 0);
        }
    } catch (...) {}
    return result;
}

constexpr size_t kStorageInitialChars = 16384;
constexpr size_t kStorageMaxChars = 16 * 1024 * 1024;

// Reads the stored JSON. Starts with a small buffer and grows it when the value
// fills the buffer completely, so large bookmark collections are never truncated.
std::wstring ReadRawJsonLocked() {
    for (size_t cap = kStorageInitialChars; cap <= kStorageMaxChars; cap *= 4) {
        std::wstring buf(cap, L'\0');
        size_t chars = Wh_GetStringValue(g_storageKey.c_str(), buf.data(), cap);
        if (chars == 0) return {};
        if (chars + 1 < cap) {
            buf.resize(chars);
            return buf;
        }
    }
    return {};
}

std::vector<BookmarkNode> LoadBookmarks() {
    std::wstring raw;
    {
        std::lock_guard lock(g_dataMutex);
        raw = ReadRawJsonLocked();
    }
    return DeserializeNodes(raw);
}

bool SaveBookmarks(const std::vector<BookmarkNode>& nodes) {
    std::wstring json = SerializeNodes(nodes);
    bool ok = false;
    {
        std::lock_guard lock(g_dataMutex);
        std::wstring prev = ReadRawJsonLocked();
        ok = Wh_SetStringValue(g_storageKey.c_str(), json.c_str());
        if (ok && prev != json) {
            g_undoStack.push_back(prev);
            if (g_undoStack.size() > 30) g_undoStack.erase(g_undoStack.begin());
        }
    }
    if (ok) NotifyExplorerBars();
    return ok;
}

bool UndoLastChange() {
    bool ok = false;
    {
        std::lock_guard lock(g_dataMutex);
        if (g_undoStack.empty()) return false;
        std::wstring prev = std::move(g_undoStack.back());
        g_undoStack.pop_back();
        ok = Wh_SetStringValue(g_storageKey.c_str(), prev.c_str());
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
[[clang::no_destroy]] std::vector<HHOOK> g_allHooks;

UINT FrameTaskMessage() {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_ExplorerChromeBookmarksBar_Task");
    return msg;
}

UINT OpenQueueMessage() {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_ExplorerChromeBookmarksBar_OpenQueue");
    return msg;
}

bool IsTrackedFrame(HWND w) {
    return w && IsWindow(w) && GetPropW(w, kFrameProperty) != nullptr;
}

HWND FindAnyTrackedFrame() {
    HWND found = nullptr;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        if (GetPropW(w, kFrameProperty) && IsWindowVisible(w)) {
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
[[clang::no_destroy]] std::map<HWND, std::deque<OpenRequest>> g_openQueues;

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
        if (MessageBoxW(frame, msg.c_str(), L"Bookmarks", MB_YESNO | MB_ICONQUESTION) != IDYES) {
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

    if (FAILED(dlg->Show(owner))) return {};
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
                auto nodes = DeserializeNodes(wide);
                if (nodes.empty()) {
                    // Never replace the current bookmarks with nothing because of a bad file
                    MessageBoxW(owner, L"The selected file is not a valid bookmarks backup.", L"Import bookmarks",
                                MB_OK | MB_ICONWARNING);
                } else {
                    SaveBookmarks(nodes);
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

void DoPaste(const NodePath& folder, int index) {
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
    auto list = LoadBookmarks();
    if (!FolderChildren(list, folder)) return;
    InsertNodes(list, folder, index, clip.nodes);
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

void RegisterFreshClass(WNDCLASSEXW& wc) {
    // Old registration (from a previous mod load) may point at unloaded code
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    RegisterClassExW(&wc);
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
    int r = 0;
    if (PCWSTR v = Wh_GetStringSetting(L"theme")) {
        if (wcscmp(v, L"light") == 0) r = 1;
        else if (wcscmp(v, L"dark") == 0) r = 2;
        Wh_FreeStringSetting(v);
    }
    return r;
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

            data->hFontTitle = CreateFontW(-20, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            data->hFont = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

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
    bool& registered = g_editClassRegistered;
    if (!registered) {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = EditBookmarkWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = kEditDlgClass;
        RegisterFreshClass(wc);
        registered = true;
    }

    int width = 465;
    int height = data.folderOnly ? 200 : 440;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    RECT ownerRc{};
    if (owner && GetWindowRect(owner, &ownerRc)) {
        x = ownerRc.left + ((ownerRc.right - ownerRc.left) - width) / 2;
        y = ownerRc.top + ((ownerRc.bottom - ownerRc.top) - height) / 2;
    }

    const wchar_t* winTitle = data.folderOnly
        ? (data.isNew ? L"Add folder" : L"Rename folder")
        : (data.isNew ? L"Add bookmark" : L"Edit bookmark");

    HWND hDlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME, kEditDlgClass, winTitle,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x, y, width, height, owner, nullptr, GetModuleHandleW(nullptr), &data);

    if (!hDlg) return false;
    if (owner) EnableWindow(owner, FALSE);

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
    if (msg.message == WM_QUIT) PostQuitMessage(static_cast<int>(msg.wParam));

    if (owner) {
        EnableWindow(owner, TRUE);
        SetForegroundWindow(owner);
    }
    return data.confirmed;
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

    auto list = LoadBookmarks();
    NodePath dest = d.folder;
    if (!FolderChildren(list, dest)) dest.clear();
    if (dest != folder) index = -1;
    BookmarkNode n;
    n.type = NodeType::Item;
    n.name = d.name;
    n.path = d.path;
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

[[clang::no_destroy]] std::mutex g_mgrNavMutex;
[[clang::no_destroy]] NodePath g_mgrNavPath;

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
        int icon = AddShellIconToImageList(d->hListImages, n.path, isFolder, 20);
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
    const int headerH = 60;

    MoveWindow(d->hTitle, 24, 14, 180, 32, TRUE);
    int searchW = std::max(160, std::min(560, cw - 220 - 80));
    int searchX = std::max(220, (cw - searchW) / 2);
    MoveWindow(d->hSearch, searchX, 15, searchW, 30, TRUE);
    MoveWindow(d->hMore, cw - 60, 13, 40, 34, TRUE);
    MoveWindow(d->hSelLabel, 24, 18, 220, 26, TRUE);
    MoveWindow(d->hSelDelete, cw - 160, 14, 90, 32, TRUE);
    MoveWindow(d->hSelClear, cw - 60, 13, 40, 34, TRUE);

    const int top = headerH + 8;
    const int treeW = kMgrTreeW;
    const int h = std::max(50, ch - top - 12 - kMgrFooterH);
    MoveWindow(d->hTree, 12, top, treeW, h, TRUE);
    const int lx = 12 + treeW + 12;
    const int lw = std::max(120, cw - lx - 12);
    MoveWindow(d->hList, lx, top, lw, h, TRUE);
    MoveWindow(d->hEmpty, lx, top + 60, lw, 30, TRUE);

    RECT lr{};
    GetClientRect(d->hList, &lr);
    int w = lr.right;
    int c2 = 36;
    int c0 = std::max(120, (w - c2) * 45 / 100);
    int c1 = std::max(60, w - c2 - c0);
    ListView_SetColumnWidth(d->hList, 0, c0);
    ListView_SetColumnWidth(d->hList, 1, c1);
    ListView_SetColumnWidth(d->hList, 2, c2);
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

    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN,
                             screen.x, screen.y, 0, hwnd, nullptr);
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
            DoPaste(folder, index);
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

            d->hFontTitle = CreateFontW(-22, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            d->hFontUI = CreateFontW(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            d->hFontIcon = CreateFontW(-20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            d->hFontSmall = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

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
            TreeView_SetItemHeight(d->hTree, 28);
            d->hTreeImages = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1);
            AddShellIconToImageList(d->hTreeImages, L"", true, 16);
            TreeView_SetImageList(d->hTree, d->hTreeImages, TVSIL_NORMAL);

            d->hList = MakeChild(hwnd, WC_LISTVIEWW, nullptr,
                                 WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | LVS_REPORT | LVS_SHOWSELALWAYS |
                                     LVS_NOCOLUMNHEADER | LVS_SHAREIMAGELISTS,
                                 0, 0, 10, 10, IDC_MGR_LIST, d->hFontUI);
            SetWindowTheme(d->hList, L"Explorer", nullptr);
            ListView_SetExtendedListViewStyle(d->hList, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
            d->hListImages = ImageList_Create(20, 20, ILC_COLOR32 | ILC_MASK, 32, 32);
            ListView_SetImageList(d->hList, d->hListImages, LVSIL_SMALL);

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
                                           d->hList, nullptr, GetModuleHandleW(nullptr), nullptr);

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

        case WM_GETMINMAXINFO: {
            auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
            mmi->ptMinTrackSize.x = 720;
            mmi->ptMinTrackSize.y = 420;
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc{};
            GetClientRect(hwnd, &rc);
            COLORREF headerColor = d ? d->colors.headerLine : RGB(218, 220, 224);
            COLORREF dividerColor = d ? d->colors.divider : RGB(0, 0, 0);
            RECT line{0, kMgrHeaderH, rc.right, kMgrHeaderH + 1};
            HBRUSH br = CreateSolidBrush(headerColor);
            FillRect(hdc, &line, br);
            DeleteObject(br);
            // Separator line between the folder tree (left) and the list (right)
            int x = kMgrMargin + kMgrTreeW + kMgrMargin / 2;
            RECT divider{x, kMgrHeaderH + 8, x + 1, rc.bottom - kMgrMargin - kMgrFooterH};
            HBRUSH br2 = CreateSolidBrush(dividerColor);
            FillRect(hdc, &divider, br2);
            DeleteObject(br2);
            // Mod version, bottom-right corner
            if (d) {
                std::wstring ver = std::wstring(L"Version ") + kModVersion;
                RECT vr{kMgrMargin, rc.bottom - kMgrFooterH - 4, rc.right - 16, rc.bottom - 4};
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
                            DoPaste({}, -1);
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
                int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTALIGN | TPM_TOPALIGN,
                                         br.right, br.bottom, 0, hwnd, nullptr);
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
    if (HWND existing = FindWindowW(kMgrClass, nullptr)) {
        ShowWindow(existing, IsIconic(existing) ? SW_RESTORE : SW_SHOW);
        SetForegroundWindow(existing);
        PostMessageW(existing, kMgrNavigateMsg, 0, 0);
        return;
    }

    bool& registered = g_mgrClassRegistered;
    if (!registered) {
        INITCOMMONCONTROLSEX icc{sizeof(icc), ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES};
        InitCommonControlsEx(&icc);

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = BookmarkManagerWndProc;
        wc.hInstance = GetModuleHandleW(nullptr);
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wc.lpszClassName = kMgrClass;
        wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
        RegisterFreshClass(wc);

        WNDCLASSEXW lc{};
        lc.cbSize = sizeof(lc);
        lc.lpfnWndProc = DropLineWndProc;
        lc.hInstance = GetModuleHandleW(nullptr);
        lc.hbrBackground = CreateSolidBrush(RGB(26, 115, 232));  // Chrome blue
        lc.lpszClassName = kDropLineClass;
        RegisterFreshClass(lc);
        registered = true;
    }

    int width = 1040;
    int height = 660;
    int x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    RECT rc{};
    if (ownerExplorer && GetWindowRect(ownerExplorer, &rc)) {
        x = rc.left + ((rc.right - rc.left) - width) / 2;
        y = rc.top + ((rc.bottom - rc.top) - height) / 2;
    }

    auto* data = new ManagerWindowData();
    data->hOwnerExplorer = ownerExplorer;
    data->currentFolder = folder;

    HWND w = CreateWindowExW(0, kMgrClass, L"Bookmarks - File Explorer",
                             WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
                             x, y, width, height, nullptr, nullptr, GetModuleHandleW(nullptr), data);
    if (!w) delete data;
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
        mi.AddHandler(mux::UIElement::PointerEnteredEvent(), winrt::box_value(hoverFocus), true);
        mi.AddHandler(mux::UIElement::PointerMovedEvent(), winrt::box_value(hoverFocus), true);
        mi.Click([weakAnchor, action, scope](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            mux::FrameworkElement a = weakAnchor.get();
            HWND frame = ExplorerWindowForElement(a);
            PostToFrame(frame, [frame, action, scope]() {
                g_pendingScope = scope;
                action(frame);
            });
        });
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
    addItem(L"Paste", canPaste, [insertFolder, insertIndex](HWND) { DoPaste(insertFolder, insertIndex); });
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

// Show a context menu at the pointer in "standard" mode (pointer, not keyboard focus)
void ShowContextMenuAt(const muxc::MenuFlyout& menu, const mux::FrameworkElement& fe,
                       const muxi::ContextRequestedEventArgs& args) {
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
        menu.Opened([weakMenu, dq](wf::IInspectable const&, wf::IInspectable const&) {
            if (!dq) return;
            dq.TryEnqueue(winrt::Microsoft::UI::Dispatching::DispatcherQueuePriority::Low, [weakMenu]() {
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
                    presenter.AddHandler(
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
        });
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
        row.LostFocus([](wf::IInspectable const& s, mux::RoutedEventArgs const&) {
            if (auto b = s.try_as<muxc::Button>()) b.Background(FlyoutClearBrush());
        });

        auto dragCtx = std::make_shared<FlyoutDragCtx>();
        std::wstring cPath = child.path;

        row.Click([weakPanel, weakAnchor, cPath, isFolder, childPath, dragCtx](wf::IInspectable const& sender,
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
        });

        row.AddHandler(
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

        row.AddHandler(
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

        row.AddHandler(
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

        row.ContextRequested([childPath](mux::UIElement const& sender, muxi::ContextRequestedEventArgs const& args) {
            args.Handled(true);
            auto fe = sender.try_as<mux::FrameworkElement>();
            if (!fe) return;
            ShowContextMenuAt(BuildChromeBookmarkContextMenu(fe, childPath), fe, args);
        });

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
    {
        auto weakSv = winrt::make_weak(sv);
        auto weakList = winrt::make_weak(listPanel);
        auto weakFly = winrt::make_weak(fly);
        auto weakAnchor = winrt::make_weak(windowAnchor);

        // Up/Down: move the selection. Right/Enter: open a sub-folder. Left: close the submenu.
        sv.PreviewKeyDown([weakList, weakFly, weakAnchor, weakPanel, folder, submenu](
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
        });

        // Any mouse movement clears the keyboard selection (like Chrome). The position is
        // compared so that synthetic events without real movement are ignored.
        auto lastPt = std::make_shared<std::optional<wf::Point>>();
        sv.AddHandler(
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
        fly.Opened([weakSv, weakList, dq, openedByKeyboard](wf::IInspectable const&, wf::IInspectable const&) {
            if (!dq) return;
            dq.TryEnqueue(winrt::Microsoft::UI::Dispatching::DispatcherQueuePriority::Low,
                          [weakSv, weakList, openedByKeyboard]() {
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
        });
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
        st->openFlyouts.push_back(std::move(of));
    }

    fly.Closed([weakPanel, id](wf::IInspectable const&, wf::IInspectable const&) {
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
    });

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

        btn.ContextRequested([nodePath](mux::UIElement const& sender, muxi::ContextRequestedEventArgs const& args) {
            args.Handled(true);
            auto fe = sender.try_as<mux::FrameworkElement>();
            if (!fe) return;
            ShowContextMenuAt(BuildChromeBookmarkContextMenu(fe, nodePath), fe, args);
        });

        btn.AddHandler(
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

        btn.AddHandler(
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

        btn.AddHandler(
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
            btn.Click([weakPanel, itemIdx, targetPath](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
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
            });
        } else {
            muxc::ToolTipService::SetToolTip(btn, winrt::box_value(node.name));

            // External File Explorer drag & drop onto this folder
            const std::wstring dropCaption = L"Add to folder (" + node.name + L")";
            btn.DragOver([dropCaption](wf::IInspectable const&, mux::DragEventArgs const& args) {
                if (args.DataView().Contains(wdt::StandardDataFormats::StorageItems())) {
                    args.AcceptedOperation(wdt::DataPackageOperation::Link);
                    args.DragUIOverride().Caption(dropCaption);
                    args.DragUIOverride().IsCaptionVisible(true);
                    args.Handled(true);
                }
            });

            btn.Drop([itemIdx](wf::IInspectable const&, mux::DragEventArgs const& args) {
                args.Handled(true);
                if (!args.DataView().Contains(wdt::StandardDataFormats::StorageItems())) return;
                auto op = args.DataView().GetStorageItemsAsync();
                op.Completed([itemIdx](auto const& asyncInfo, wf::AsyncStatus status) {
                    if (status != wf::AsyncStatus::Completed) return;
                    auto items = asyncInfo.GetResults();
                    auto list = LoadBookmarks();
                    auto* kids = FolderChildren(list, NodePath{itemIdx});
                    if (!kids) return;
                    for (const auto& item : items) {
                        std::wstring p(item.Path().c_str());
                        if (!p.empty()) {
                            p = NormalizePath(p);
                            BookmarkNode n;
                            n.type = NodeType::Item;
                            n.name = DefaultLabel(p);
                            n.path = p;
                            kids->push_back(std::move(n));
                        }
                    }
                    SaveBookmarks(list);
                });
            });

            btn.Click([weakPanel, itemIdx](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
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
            });
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

void OnBarDrop(const mux::DragEventArgs& args) {
    if (g_unloading || !args.DataView().Contains(wdt::StandardDataFormats::StorageItems())) return;
    auto op = args.DataView().GetStorageItemsAsync();
    op.Completed([](auto const& asyncInfo, wf::AsyncStatus status) {
        if (status != wf::AsyncStatus::Completed) return;
        auto items = asyncInfo.GetResults();
        auto list = LoadBookmarks();
        for (const auto& item : items) {
            std::wstring p(item.Path().c_str());
            if (!p.empty()) {
                p = NormalizePath(p);
                BookmarkNode n;
                n.type = NodeType::Item;
                n.path = p;
                n.name = DefaultLabel(p);
                list.push_back(std::move(n));
            }
        }
        SaveBookmarks(list);
    });
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
void RemoveThreadBars() {
    if (!g_bars) return;
    for (auto& st : *g_bars) {
        try {
            for (auto& f : st.openFlyouts) {
                try {
                    if (f.flyout) f.flyout.Hide();
                } catch (...) {}
            }
            st.openFlyouts.clear();

            if (auto cb = st.commandBar.get(); cb && st.loadedToken) {
                cb.Loaded(st.loadedToken);  // revoke
            }

            auto grid = st.grid.get();
            auto barRoot = st.barRoot.get();
            if (grid && barRoot) {
                uint32_t idx = 0;
                if (grid.Children().IndexOf(barRoot, idx)) grid.Children().RemoveAt(idx);
                if (st.addedRow) {
                    uint32_t r = 0;
                    if (grid.RowDefinitions().IndexOf(st.addedRow, r)) grid.RowDefinitions().RemoveAt(r);
                }
                if (st.createdFirstRow && grid.RowDefinitions().Size() == 1) grid.RowDefinitions().Clear();
                grid.MinHeight(st.oldGridMinHeight);
                if (auto nav = st.navControl.get()) nav.MinHeight(st.oldNavMinHeight);
                if (st.commandRow) st.commandRow.Height(st.oldCommandRowHeight);
                grid.InvalidateMeasure();
                if (auto host = st.hostGrid.get()) host.InvalidateMeasure();
            }
        } catch (...) {}
    }
    try {
        FlushDiagReleasesForCurrentThread();
    } catch (...) {}
    g_bars->clear();
    g_frames.clear();  // windows get subclassed again if the mod is re-enabled
    g_frameRows = 0;
    ClearIconCache();
    g_frameTasks.clear();
}

static LRESULT FrameSubclassProcImpl(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == UninitBarsMessage()) {
        RemoveThreadBars();
        KillTimer(window, kOpenTimerId);
        if (g_messageHook) {
            UnhookWindowsHookEx(g_messageHook);
            {
                std::lock_guard lock(g_hooksMutex);
                std::erase(g_allHooks, g_messageHook);
            }
            g_messageHook = nullptr;
        }
        RECT rc{};
        if (GetClientRect(window, &rc)) {
            PostMessageW(window, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
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
    if (message == WM_NCDESTROY) {
        RemovePropW(window, kFrameProperty);
        std::erase(g_frames, window);
        std::lock_guard lock(g_openMutex);
        g_openQueues.erase(window);
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
    try {
        if (g_unloading || !g_frameHooked) return;
        auto root = commandBar.XamlRoot();
        if (!root || !root.Content()) return;
        auto grid = FindNavigationGrid(root.Content(), 0);
        if (!grid) return;

        BarState* state = nullptr;
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

        auto nav = muxm::VisualTreeHelper::GetParent(grid).try_as<mux::FrameworkElement>();
        auto host = nav ? muxm::VisualTreeHelper::GetParent(nav).try_as<muxc::Grid>() : nullptr;
        if (!host) return;
        auto hostRows = host.RowDefinitions();
        if (hostRows.Size() != 3 || muxc::Grid::GetRow(nav) != 1) return;

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

        barRoot.ContextRequested([](mux::UIElement const& sender, muxi::ContextRequestedEventArgs const& args) {
            args.Handled(true);
            auto fe = sender.try_as<mux::FrameworkElement>();
            if (!fe) return;
            ShowContextMenuAt(BuildChromeBookmarkContextMenu(fe, NodePath{}), fe, args);
        });

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

        barRoot.DragOver([](wf::IInspectable const&, mux::DragEventArgs const& args) { OnBarDragOver(args); });
        barRoot.Drop([](wf::IInspectable const&, mux::DragEventArgs const& args) { OnBarDrop(args); });

        grid.Children().Append(barRoot);
        grid.InvalidateMeasure();
        host.InvalidateMeasure();

        g_frameRows = 1;
        RefreshPanel(buttons, std::nullopt);

        if (state->frame) {
            RECT rc{};
            if (GetClientRect(state->frame, &rc)) {
                PostMessageW(state->frame, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));
            }
        }
    } catch (...) {
        Wh_Log(L"TryInstallBar failed");
    }
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
        !(msg->lParam & (1 << 30)) && (GetKeyState(VK_CONTROL) & 0x8000)) {
        HWND frame = msg->hwnd ? GetAncestor(msg->hwnd, GA_ROOT) : nullptr;
        if (IsExplorerFrame(frame)) {
            msg->message = WM_NULL;
            auto sel = SelectedItems(frame);
            if (sel.empty()) {
                auto cur = CurrentFolder(frame);
                if (!cur.empty()) sel.push_back(cur);
            }
            if (!sel.empty()) {
                auto list = LoadBookmarks();
                for (const auto& p : sel) {
                    BookmarkNode n;
                    n.type = NodeType::Item;
                    n.path = p;
                    n.name = DefaultLabel(p);
                    list.push_back(std::move(n));
                }
                SaveBookmarks(list);
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
    g_bars->emplace_back();
    BarState& state = g_bars->back();
    state.commandBar = winrt::make_weak(commandBar);
    auto weakBar = state.commandBar;
    state.loadedToken = commandBar.Loaded([weakBar](wf::IInspectable const&, mux::RoutedEventArgs const&) {
        if (auto bar = weakBar.get()) TryInstallBar(bar);
    });
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
// Already-open Explorer windows: attach without restart (XAML Diagnostics)
// ============================================================================
// The CommandBar hook only sees windows created after the mod is loaded. To reach
// the XAML of windows that are already open, the public XAML Diagnostics API is
// used (the same API used by XAML debugging tools). The mod DLL is loaded as a
// small "TAP" COM object, a visual tree watcher reports every window's
// "FileExplorerCommandBar", and the regular TrackCommandBar() runs on it. The
// watcher stops after a few seconds; new windows keep using the normal hook.

// {9B5C2F7E-3D41-4C8A-A6E2-5F1B7D0C4E93}
constexpr CLSID CLSID_BookmarksBarTAP = {0x9b5c2f7e, 0x3d41, 0x4c8a, {0xa6, 0xe2, 0x5f, 0x1b, 0x7d, 0x0c, 0x4e, 0x93}};

// Also implemented by XAML Diagnostics, but not declared in xamlom.h. The
// diagnostics keep a reference to every reported element; UnregisterInstance
// releases it, otherwise elements would stay in memory (even after their window
// is closed).
constexpr GUID IID_XamlDiagTestHooks = {0x735941a2, 0x3ee3, 0x495a, {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};
struct IXamlDiagTestHooks : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetDispatcherQueueForObject(InstanceHandle handle, void** dispatcherQueue) = 0;
};

std::mutex g_tapMutex;
[[clang::no_destroy]] winrt::com_ptr<IXamlDiagTestHooks> g_diagTestHooks;
// Handles reported on each UI thread (released later on that same thread)
[[clang::no_destroy]] thread_local std::vector<InstanceHandle> g_diagPendingRelease;
thread_local bool g_diagThreadRegistered = false;
[[clang::no_destroy]] std::vector<winrt::Microsoft::UI::Dispatching::DispatcherQueue> g_diagThreads;

// Releases the diagnostics references of all handles reported on this thread.
// Must run on the UI thread, outside XAML's tree walk (from the dispatcher or a message).
void FlushDiagReleasesForCurrentThread() {
    if (g_diagPendingRelease.empty()) return;
    auto pending = std::move(g_diagPendingRelease);
    g_diagPendingRelease.clear();
    winrt::com_ptr<IXamlDiagTestHooks> hooks;
    {
        std::lock_guard lock(g_tapMutex);
        hooks = g_diagTestHooks;
    }
    if (!hooks) return;
    std::sort(pending.begin(), pending.end());
    pending.erase(std::unique(pending.begin(), pending.end()), pending.end());
    for (InstanceHandle h : pending) {
        if (h) hooks->UnregisterInstance(h);
    }
}

struct VisualTreeWatcher : winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile> {
    explicit VisualTreeWatcher(winrt::com_ptr<IXamlDiagnostics> diag) : m_diag(std::move(diag)) {}

    HRESULT Advise() {
        HRESULT hr = m_diag.as<IVisualTreeService3>()->AdviseVisualTreeChange(this);
        if (SUCCEEDED(hr)) m_advised = true;
        return hr;
    }

    void Unadvise() {
        if (!m_advised.exchange(false)) return;
        try {
            m_diag.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
        } catch (...) {}
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element,
                                                 VisualMutationType mutationType) override {
        try {
            // Remember each thread's dispatcher (used later to release the references)
            if (!g_diagThreadRegistered) {
                g_diagThreadRegistered = true;
                if (auto dq = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread()) {
                    std::lock_guard lock(g_tapMutex);
                    g_diagThreads.push_back(dq);
                }
            }
            if (mutationType == Add) {
                g_diagPendingRelease.push_back(element.Handle);
                if (relation.Parent) g_diagPendingRelease.push_back(relation.Parent);
            }

            if (g_unloading || mutationType != Add || !element.Name) return S_OK;
            if (wcscmp(element.Name, L"FileExplorerCommandBar") != 0) return S_OK;

            wf::IInspectable obj;
            if (FAILED(m_diag->GetIInspectableFromHandle(
                    element.Handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj)))) || !obj) {
                return S_OK;
            }
            auto cb = obj.try_as<muxc::CommandBar>();
            if (!cb) return S_OK;
            auto dq = cb.DispatcherQueue();
            if (!dq) return S_OK;
            Wh_Log(L"XAML Diagnostics: found an open File Explorer window");
            // Install the bar outside the tree walk, on the window's own UI thread
            dq.TryEnqueue([cb]() {
                if (g_unloading) return;
                try {
                    TrackCommandBar(cb);
                } catch (...) {}
            });
        } catch (...) {}
        return S_OK;  // returning an error could stop further notifications
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept override {
        return S_OK;
    }

private:
    winrt::com_ptr<IXamlDiagnostics> m_diag;
    std::atomic<bool> m_advised{false};
};

[[clang::no_destroy]] winrt::com_ptr<VisualTreeWatcher> g_treeWatcher;
HANDLE g_tapStopEvent = nullptr;
HANDLE g_tapThread = nullptr;
constexpr DWORD kTreeWatchMs = 15000;  // how long to look for already-open windows

DWORD WINAPI TreeWatcherThread(LPVOID) {
    try {
        winrt::com_ptr<VisualTreeWatcher> watcher;
        HANDLE stopEvent = nullptr;
        {
            std::lock_guard lock(g_tapMutex);
            watcher = g_treeWatcher;
            stopEvent = g_tapStopEvent;
        }
        if (!watcher || !stopEvent) return 0;

        // Advise from a separate thread, not a UI thread (that can hang)
        HRESULT coHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        HRESULT hr = watcher->Advise();
        Wh_Log(L"XAML Diagnostics: AdviseVisualTreeChange = 0x%08X", static_cast<unsigned>(hr));
        if (SUCCEEDED(hr)) {
            bool stoppedEarly = WaitForSingleObject(stopEvent, kTreeWatchMs) == WAIT_OBJECT_0;
            watcher->Unadvise();
            if (!stoppedEarly && !g_unloading) {
                // Release the references on each UI thread
                std::vector<winrt::Microsoft::UI::Dispatching::DispatcherQueue> threads;
                {
                    std::lock_guard lock(g_tapMutex);
                    threads = g_diagThreads;
                }
                for (auto& dq : threads) {
                    try {
                        dq.TryEnqueue([]() {
                            if (g_unloading) return;
                            try {
                                FlushDiagReleasesForCurrentThread();
                            } catch (...) {}
                        });
                    } catch (...) {}
                }
                Wh_Log(L"XAML Diagnostics: watcher stopped");
            }
        }
        if (SUCCEEDED(coHr)) CoUninitialize();
    } catch (...) {}
    return 0;
}

void StartTreeWatcher(const winrt::com_ptr<IXamlDiagnostics>& diag) {
    std::lock_guard lock(g_tapMutex);
    if (g_unloading || g_treeWatcher || g_tapThread) return;
    g_diagTestHooks = nullptr;
    diag->QueryInterface(IID_XamlDiagTestHooks, g_diagTestHooks.put_void());
    if (!g_diagTestHooks) Wh_Log(L"XAML Diagnostics: test hooks unavailable");
    g_treeWatcher = winrt::make_self<VisualTreeWatcher>(diag);
    g_tapStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_tapThread = g_tapStopEvent ? CreateThread(nullptr, 0, TreeWatcherThread, nullptr, 0, nullptr) : nullptr;
    if (!g_tapThread) g_treeWatcher = nullptr;
}

// Called first on unload. The remaining references are released in
// RemoveThreadBars, on each window's thread.
void StopTreeWatcher() {
    HANDLE thread = nullptr;
    HANDLE stopEvent = nullptr;
    winrt::com_ptr<VisualTreeWatcher> watcher;
    {
        std::lock_guard lock(g_tapMutex);
        thread = g_tapThread;
        stopEvent = g_tapStopEvent;
        watcher = g_treeWatcher;
        g_tapThread = nullptr;
        g_tapStopEvent = nullptr;
        g_treeWatcher = nullptr;
        g_diagThreads.clear();
    }
    if (stopEvent) SetEvent(stopEvent);
    if (thread) {
        WaitForSingleObject(thread, 5000);
        CloseHandle(thread);
    }
    if (stopEvent) CloseHandle(stopEvent);
    if (watcher) watcher->Unadvise();
}

HMODULE ThisModule() {
    HMODULE mod = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ThisModule), &mod);
    return mod;
}

// Created by XAML Diagnostics, which passes its diagnostics interface to SetSite()
struct BookmarksBarTAP : winrt::implements<BookmarksBarTAP, IObjectWithSite, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) override {
        try {
            if (!site) return S_OK;
            // InitializeXamlDiagnosticsEx loaded the DLL one extra time; release that
            // reference so the mod can unload cleanly when disabled
            FreeLibrary(ThisModule());
            if (g_unloading) return S_OK;
            winrt::com_ptr<IXamlDiagnostics> diag;
            if (FAILED(site->QueryInterface(__uuidof(IXamlDiagnostics), diag.put_void())) || !diag) return S_OK;
            Wh_Log(L"XAML Diagnostics: connected");
            StartTreeWatcher(diag);
        } catch (...) {}
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID, void** ppvSite) noexcept override {
        if (ppvSite) *ppvSite = nullptr;
        return E_NOTIMPL;
    }
};

struct BookmarksBarTAPFactory : winrt::implements<BookmarksBarTAPFactory, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override {
        try {
            if (!ppv) return E_POINTER;
            *ppv = nullptr;
            if (outer) return CLASS_E_NOAGGREGATION;
            return winrt::make<BookmarksBarTAP>().as(riid, ppv);
        } catch (...) {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};

// XAML Diagnostics loads the mod DLL by path and calls this export
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
extern "C" __declspec(dllexport) HRESULT STDAPICALLTYPE DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    try {
        if (!ppv) return E_POINTER;
        *ppv = nullptr;
        if (rclsid != CLSID_BookmarksBarTAP) return CLASS_E_CLASSNOTAVAILABLE;
        return winrt::make<BookmarksBarTAPFactory>().as(riid, ppv);
    } catch (...) {
        return winrt::to_hresult();
    }
}
#pragma clang diagnostic pop

// Is any File Explorer window open in this explorer.exe process?
bool HasOpenExplorerWindows() {
    bool found = false;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(w, &pid);
        if (pid == GetCurrentProcessId() && IsExplorerFrame(w)) {
            *reinterpret_cast<bool*>(lp) = true;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&found));
    return found;
}

using InitializeXamlDiagnosticsEx_t = HRESULT(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, CLSID, LPCWSTR);

void AttachToOpenExplorerWindows() {
    if (g_unloading || !HasOpenExplorerWindows()) return;  // no open windows: nothing to do

    // Diagnostics entry point of WinUI 3 (used by File Explorer)
    HMODULE udk = GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");
    auto ixde = udk ? reinterpret_cast<InitializeXamlDiagnosticsEx_t>(GetProcAddress(udk, "InitializeXamlDiagnosticsEx"))
                    : nullptr;
    if (!ixde) {
        Wh_Log(L"XAML Diagnostics: InitializeXamlDiagnosticsEx not found");
        return;
    }

    wchar_t path[MAX_PATH] = {};
    DWORD len = GetModuleFileNameW(ThisModule(), path, ARRAYSIZE(path));
    if (len == 0 || len >= ARRAYSIZE(path)) return;

    // WinUI connections are named "WinUIVisualDiagConnection<N>"; N isn't known
    // in advance, so the numbers are tried in order
    HRESULT hr = E_FAIL;
    for (int i = 1; i <= 10000; i++) {
        std::wstring connection = L"WinUIVisualDiagConnection" + std::to_wstring(i);
        hr = ixde(connection.c_str(), GetCurrentProcessId(), L"", path, CLSID_BookmarksBarTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) break;
    }
    Wh_Log(L"XAML Diagnostics: InitializeXamlDiagnosticsEx = 0x%08X", static_cast<unsigned>(hr));
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
    WindhawkUtils::SYMBOL_HOOK hook[] = {{
        {LR"(public: virtual long __cdecl XamlIslandViewAdapter::get_DesiredSizeInPhysicalPixels(struct tagSIZE *))"},
        &g_desiredSizeOriginal, DesiredSizeHook}};
    if (!WindhawkUtils::HookSymbols(mod, hook, ARRAYSIZE(hook)) || !g_desiredSizeOriginal) return false;
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
    WindhawkUtils::SYMBOL_HOOK hook[] = {{
        {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))"},
        &g_commandBarSetterOriginal, CommandBarSetterHook}};
    if (!WindhawkUtils::HookSymbols(mod, hook, ARRAYSIZE(hook)) || !g_commandBarSetterOriginal) return false;
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
    g_editClassRegistered = false;
    g_mgrClassRegistered = false;
    g_bookmarkHotkey = Wh_GetIntSetting(L"bookmarkHotkey") != 0;
    g_themeSetting = ReadThemeSetting();
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
    AttachToOpenExplorerWindows();
} catch (...) {
    Wh_Log(L"Wh_ModAfterInit: exception");
}

void Wh_ModSettingsChanged() try {
    g_bookmarkHotkey = Wh_GetIntSetting(L"bookmarkHotkey") != 0;
    g_themeSetting = ReadThemeSetting();
    if (HWND m = FindWindowW(kMgrClass, nullptr)) {
        PostMessageW(m, kMgrThemeMsg, 0, 0);
    }
} catch (...) {
    Wh_Log(L"Wh_ModSettingsChanged: exception");
}

void Wh_ModUninit() try {
    g_unloading = true;
    StopTreeWatcher();
    // Close an open "Edit bookmark" dialog: its message loop runs inside mod code
    if (HWND dlg = FindWindowW(kEditDlgClass, nullptr)) {
        SendMessageTimeoutW(dlg, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 2000, nullptr);
    }
    // Close the manager before our code is unloaded
    if (HWND m = FindWindowW(kMgrClass, nullptr)) {
        SendMessageTimeoutW(m, WM_CLOSE, 0, 0, SMTO_ABORTIFHUNG, 2000, nullptr);
    }

    // For each Explorer window: remove the bar (on its own thread), then the subclass,
    // so nothing calls into mod code after it is unloaded.
    std::vector<HWND> frames;
    EnumWindows([](HWND w, LPARAM lp) -> BOOL {
        if (GetPropW(w, kFrameProperty)) reinterpret_cast<std::vector<HWND>*>(lp)->push_back(w);
        return TRUE;
    }, reinterpret_cast<LPARAM>(&frames));
    for (HWND w : frames) {
        SendMessageTimeoutW(w, UninitBarsMessage(), 0, 0, SMTO_ABORTIFHUNG, 3000, nullptr);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(w, FrameSubclassProc);
        RemovePropW(w, kFrameProperty);
    }

    // Remove any remaining message hooks (e.g. a thread whose window wasn't found)
    {
        std::lock_guard lock(g_hooksMutex);
        for (HHOOK h : g_allHooks) UnhookWindowsHookEx(h);
        g_allHooks.clear();
    }

    HINSTANCE inst = GetModuleHandleW(nullptr);
    UnregisterClassW(kMgrClass, inst);
    UnregisterClassW(kDropLineClass, inst);
    UnregisterClassW(kEditDlgClass, inst);

    {
        std::lock_guard lock(g_tapMutex);
        g_diagTestHooks = nullptr;
    }
} catch (...) {
    Wh_Log(L"Wh_ModUninit: exception");
}

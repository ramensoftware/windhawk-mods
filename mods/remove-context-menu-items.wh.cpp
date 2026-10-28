// ==WindhawkMod==
// @id              remove-context-menu-items
// @name            Remove Context Menu Items
// @description     Removes unwanted items from file context menus with configurable options and context-aware filtering
// @version         1.12.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lshlwapi -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Context Menu Items

⚠️ **Windows 11 Users:** This mod currently works only with the classic context menu. Please install the [Classic context menu on Windows 11](https://windhawk.net/mods/explorer-context-menu-classic) mod to use this mod.

---

This mod removes unwanted items from file context menus with configurable options. It hooks into the context menu creation process and removes unwanted menu items by checking their text labels before they are displayed. It also automatically cleans up duplicate separator lines.

## Screenshots

**Before and after right-clicking on an empty space:**

![Before and after on empty space](https://raw.githubusercontent.com/armaninyow/Remove-Unwanted-Context-Menu-Items/refs/heads/main/Blank%20Space.png)

**Before and after right-clicking a video file:**

![Before and after on video file](https://raw.githubusercontent.com/armaninyow/Remove-Unwanted-Context-Menu-Items/refs/heads/main/Video%20File.png)

## Features

Clean up your Windows context menus by removing bloatware and unwanted items:

| Bloatware Items                | Basic Items                 | App-specific Items                   |
|--------------------------------|-----------------------------|--------------------------------------|
| `Move to OneDrive`             | `Open`                      | `Add to VLC media player's Playlist` |
| `Ask Copilot`                  | `Cast to Device`            | `Edit in Notepad`                    |
| `Scan with Microsoft Defender` | `Include in library`        | `Edit with Paint`                    |
| `Create with Designer`         | `Restore previous versions` | `NVIDIA Control Panel`               |
| ...and more!                   | ...and more!                | ...and more!                         |

### Context-Aware Filtering
Some items like `Edit in Notepad` and `WinRAR` can be filtered based on file extensions. For example, you can configure the mod to only show `Edit in Notepad` for text files or `WinRAR` items for archives, hiding them for all other file types.

### Modifier Key Override
Hold `Alt` while right-clicking to temporarily bypass the mod and see all original context menu items. Useful when you need access to a hidden item without changing settings.

### Custom Items
You can also add your own custom menu items to remove by entering their text in the settings.

**Basic Usage (Exact Match):**
- `Copy` - Removes the "Copy" option (exact match)
- `Pin to Quick access` - Removes the "Pin to Quick access" option (exact match)
- `Open` - Removes only "Open" (exact match, won't remove "Open in new tab")

**Wildcard Usage (Prefix Match):**
Add an asterisk (*) at the end to match items that start with the given text:
- `Open*` - Removes all menu items that start with "Open":
  - Open
  - Open with...
  - Open in new tab
  - Open in Terminal (and more)
- `Pin to*` - Removes all menu items that start with "Pin to":
  - Pin to Start
  - Pin to Quick access (and more)
- `C*` - Removes all menu items that start with "C":
  - Cut
  - Copy
  - Create shortcut (and more, so be careful!)

> **Tip:** Right-click a file/folder, note the exact text of the menu item you want to remove, then add it to Custom Items. Use the asterisk (*) only if you want to remove multiple items with the same prefix.

## Supported Languages

- `cs-CZ`
- `de-DE` (added by [Schleifenkratzer](https://github.com/Schleifenkratzer))
- `en-AU`, `en-GB`
- `es-MX` (added by [frankh93](https://github.com/frankh93))
- `fr-FR` (added by [Catif](https://github.com/Catif))
- `pl-PL` (added by [FadeMind](https://github.com/FadeMind))
- `pt-BR`, `pt-PT`
- `ru-RU` (added by [VitalityV1nT](https://github.com/VitalityV1nT))
- `tr-TR` (added by [bcrtvkcs](https://github.com/bcrtvkcs))
- `ja-JP` (added by [haru612](https://github.com/haru612))

If you find a mistake and for additional details, please click [here](https://github.com/armaninyow/Remove-Unwanted-Context-Menu-Items).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- bloatwareItems:
  - removeOneDrive: true
    $name: Move to OneDrive
  - removeCopilot: true
    $name: Ask Copilot
  - removeDefender: true
    $name: Scan with Microsoft Defender...
  - removeDesigner: true
    $name: Create with Designer
  - removeClipchamp: true
    $name: Edit with Clipchamp
  - removeMicrosoft365Copilot: true
    $name: Ask Microsoft 365 Copilot
  $name: Bloatware Items
  $description: Enabled by default

- basicItems:
  - removeOpen: false
    $name: Open
  - removeOpenWith: false
    $name: Open with
  - removeCut: false
    $name: Cut
  - removeCopy: false
    $name: Copy
  - removeCreateShortcut: false
    $name: Create shortcut
  - removeDelete: false
    $name: Delete
  - removeRename: false
    $name: Rename
  - removeSendTo: false
    $name: Send to
  - removeOpenInNewTab: false
    $name: Open in new tab
  - removeOpenInNewWindow: false
    $name: Open in new window
  - removeEdit: false
    $name: Edit
  - removePlay: false
    $name: Play
  - removePreview: false
    $name: Preview
  - removePrint: false
    $name: Print
  - removeShare: false
    $name: Share
  - removeRefresh: false
    $name: Refresh
  - removeCopyAsPath: false
    $name: Copy as path
  - removeCustomizeFolder: false
    $name: Customize this folder...
  - removeFavorites: false
    $name: Add to Favorites
  - removePinToQuickAccess: false
    $name: Pin to Quick access
  - removePinToStart: false
    $name: Pin to Start
  - removeCast: false
    $name: Cast to Device
  - removeGiveAccess: false
    $name: Give access to
  - removeRestoreVersions: false
    $name: Restore previous versions
  - removeIncludeInLibrary: false
    $name: Include in library
  - removeRotate: false
    $name: Rotate options
    $description: The "Rotate right" and "Rotate left" items
  - removeDisplaySettings: false
    $name: Display settings
    $description: Desktop menu item
  - removePersonalize: false
    $name: Personalize
    $description: Desktop menu item
  - removeSetAsDesktopBackground: false
    $name: Set as desktop background
  - removeView: false
    $name: View
  - removeSortBy: false
    $name: Sort by
  - removeGroupBy: false
    $name: Group by
  - removeNew: false
    $name: New
  - removeProperties: false
    $name: Properties
  - removePaste: false
    $name: Paste
    $description: Only when greyed-out
  - removeExtractAll: false
    $name: Extract All...
  $name: Basic Items
  $description: Disabled by default

- appSpecificItems:
  - removeSendWithQuickShare: false
    $name: Send with Quick Share
  - removeVLCPlaylist: false
    $name: Add to VLC media player's Playlist
  - removeVLCPlay: false
    $name: Play with VLC media player
  - removeAddToMediaPlayerQueue: false
    $name: Add to Media Player play queue
  - removePlayWithMediaPlayer: false
    $name: Play with Media Player
  - removeEditInNotepad: false
    $name: Edit in Notepad
    $description: Enable extension filtering below to show this only for relevant file types
  - removeEditInNotepadPlusPlus: false
    $name: Edit with Notepad++
    $description: Enable extension filtering below to show this only for relevant file types
  - removeEditWithPhotos: false
    $name: Edit with Photos
  - removeEditWithPaint: false
    $name: Edit with Paint
  - removeNvidiaControlPanel: false
    $name: NVIDIA Control Panel
    $description: Desktop menu item
  - removeOpenInTerminal: false
    $name: Open in Terminal
  - removeAlwaysKeepOnThisDevice: false
    $name: Always keep on this device
    $description: OneDrive menu item
  - removeFreeUpSpace: false
    $name: Free up space
    $description: OneDrive menu item
  - removeWinRAR: false
    $name: WinRAR
    $description: Enable extension filtering below to show this only for relevant file types
  $name: App-specific Items
  $description: Disabled by default

- customItems:
  - ""
  $name: Custom items to remove
  $description: >-
    Basic Usage (Exact Match): Enter exact menu item names.

    Wildcard Usage (Prefix Match): Add * at the end for prefix matching (e.g., "Open*" removes all items starting with "Open")

- modifierKeyOverride:
  - enableModifierOverride: false
    $name: Enable Alt key bypass
    $description: >-
      When enabled, holding Alt while right-clicking temporarily bypasses the mod and shows all original context menu items.
  $name: Modifier Key Override
  $description: Hold Alt while right-clicking to see all original menu items

- extensionFiltering:
  - enableExtensionFiltering: false
    $name: Enable Notepad extension filtering
    $description: >-
      When enabled, "Edit in Notepad" and "Edit with Notepad++" will ONLY appear for files whose extensions are in the Notepad whitelist below. They are hidden for all other file types. (Note: Requires "Edit in Notepad" or "Edit with Notepad++" menu items to be enabled.)
  - notepadExtensions:
    - ".txt"
    - ".log"
    - ".json"
    - ".xml"
    - ".html"
    - ".css"
    - ".js"
    - ".ts"
    - ".md"
    - ".csv"
    - ".sql"
    - ".yaml"
    - ".yml"
    - ".py"
    - ".java"
    - ".c"
    - ".cpp"
    - ".cs"
    $name: Extensions for Notepad/Notepad++
    $description: Use lowercase with dot (e.g., .txt)
  - enableWinRARFiltering: false
    $name: Enable WinRAR extension filtering
    $description: >-
      When enabled, the WinRAR menu item will ONLY appear for files whose extensions are in the WinRAR whitelist below. It is hidden for all other file types. (Note: Requires "WinRAR" menu item to be enabled.)
  - winrarExtensions:
    - ".zip"
    - ".rar"
    - ".7z"
    - ".iso"
    - ".tar"
    - ".gz"
    - ".tgz"
    - ".bz2"
    - ".xz"
    - ".zst"
    - ".cab"
    $name: Extensions for WinRAR
    $description: Use lowercase with dot (e.g., .zip)
  $name: Extension Filtering
  $description: >-
    On Windows 11, Notepad and WinRAR items can appear even when right-clicking unrelated files. Use the Extension Filtering to show each only for relevant file types.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <shlguid.h>
#include <servprov.h>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <cwchar>
#include <windhawk_utils.h>

// Thread-local storage for file paths
thread_local std::vector<std::wstring> tl_filePaths;

// RemoveMenu() detaches a filtered submenu without destroying it, passing
// ownership to us. We stash the handle here and destroy it in
// ExitMenuTracking(), after the session's tracking call has returned.
thread_local std::vector<HMENU> tl_detachedSubmenus;

// Result of a single ancestor-chain classification of a popup menu's owner
// window: whether to filter the menu at all, and (for ShellDefView) enough
// to skip a redundant re-walk later when looking up the selected files.
enum class ShellViewKind {
    None,           // not a real file/folder context menu (taskbar, tray, Start menu, Explorer-frame toolbars, etc.)
    Desktop,        // right-click on the desktop
    ShellDefView,   // right-click in the main file list view -- has a selection to read
    NavPane,        // right-click in the navigation pane -- no IFolderView selection to read
};

// True only for the desktop: the root window is the shell desktop window
// itself, or a top-level Progman/WorkerW that actually hosts the desktop's
// SHELLDLL_DefView -- checking that child excludes other WorkerW/Progman roots reused elsewhere in Explorer.
bool IsDesktopWindow(HWND hwnd) {
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) {
        return false;
    }
    if (root == GetShellWindow()) {
        return true;
    }
    wchar_t className[256] = {0};
    GetClassNameW(root, className, ARRAYSIZE(className));
    // Progman at creation time; a slideshow or wallpaper tool can reparent
    // the desktop shell view onto a top-level WorkerW afterwards. Either
    // way, the desktop's root always hosts a SHELLDLL_DefView child.
    if (wcscmp(className, L"Progman") != 0 && wcscmp(className, L"WorkerW") != 0) {
        return false;
    }
    return FindWindowExW(root, nullptr, L"SHELLDLL_DefView", nullptr) != nullptr;
}

// Classifies hwnd with a single ancestor walk. Desktop check MUST run
// first: desktop's icon view is itself hosted in a SHELLDLL_DefView, so
// checking the loop first misclassifies desktop, breaking extension filtering.
ShellViewKind ClassifyShellView(HWND hwnd) {
    if (IsDesktopWindow(hwnd)) {
        return ShellViewKind::Desktop;
    }
    for (HWND h = hwnd; h; h = GetAncestor(h, GA_PARENT)) {
        wchar_t className[256] = {0};
        GetClassNameW(h, className, ARRAYSIZE(className));
        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            return ShellViewKind::ShellDefView;
        }
        if (wcscmp(className, L"NamespaceTreeControl") == 0) {
            return ShellViewKind::NavPane;
        }
    }
    return ShellViewKind::None;
}

// Sends CWM_GETISHELLBROWSER (WM_USER+7) only to known frame/tab window
// classes. WM_USER-range values are only meaningful within a specific
// class's own message set, so an unknown class risks misinterpreting it.
IShellBrowser* GetShellBrowser(HWND hwnd) {
    // SHELLDLL_DefView isn't included: it never answers this message and
    // has its own private WM_USER-range messages, risking a misread reply.
    // Progman/WorkerW route to GetDesktopShellBrowser() before this is called.
    struct ShellWindowClass {
        const wchar_t* name;
        bool isFrame;  // true if this class also bounds the ancestor walk
    };
    static const ShellWindowClass kShellWindowClasses[] = {
        {L"ShellTabWindowClass", false},
        {L"CabinetWClass", true},
        {L"ExploreWClass", true},
    };

    IShellBrowser* pShellBrowser = nullptr;
    for (HWND h = hwnd; h; h = GetAncestor(h, GA_PARENT)) {
        wchar_t className[256] = {0};
        GetClassNameW(h, className, ARRAYSIZE(className));

        for (const ShellWindowClass& cls : kShellWindowClasses) {
            if (wcscmp(className, cls.name) != 0) {
                continue;
            }

            LRESULT result = SendMessageW(h, WM_USER + 7 /* CWM_GETISHELLBROWSER */, 0, 0);
            pShellBrowser = (IShellBrowser*)result;
            if (pShellBrowser) {
                return pShellBrowser;
            }

            if (cls.isFrame) {
                return nullptr;
            }
            break;
        }
    }
    return nullptr;
}

// Set by GetSelectedFilesFromExplorer() to distinguish a genuine "nothing
// is selected" from a lookup failure, so ShouldRemoveByExtension() only
// fails closed (hides) on the former -- an error shouldn't hide a match.
thread_local bool tl_selectionLookupFailed = false;

// The desktop doesn't expose an IShellBrowser via CWM_GETISHELLBROWSER --
// use IShellWindows::FindWindowSW(SWC_DESKTOP) then IServiceProvider ->
// SID_STopLevelBrowser. Returns a new reference; the caller must Release() it.
IShellBrowser* GetDesktopShellBrowser() {
    IShellWindows* pShellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                 IID_IShellWindows, (void**)&pShellWindows)) || !pShellWindows) {
        return nullptr;
    }
    
    IShellBrowser* pShellBrowser = nullptr;
    VARIANT vEmpty = {};
    long lhwnd = 0;
    IDispatch* pDisp = nullptr;
    if (SUCCEEDED(pShellWindows->FindWindowSW(&vEmpty, &vEmpty, SWC_DESKTOP, &lhwnd,
                                               SWFO_NEEDDISPATCH, &pDisp)) && pDisp) {
        IServiceProvider* pServiceProvider = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_IServiceProvider, (void**)&pServiceProvider)) && pServiceProvider) {
            pServiceProvider->QueryService(SID_STopLevelBrowser, IID_IShellBrowser, (void**)&pShellBrowser);
            pServiceProvider->Release();
        }
        pDisp->Release();
    }
    
    pShellWindows->Release();
    return pShellBrowser;
}

// Extracts the currently-selected file paths from a live IShellBrowser.
// Shared by both the desktop and regular-folder-window lookup paths.
// Reference-counting of pShellBrowser is the caller's responsibility.
std::vector<std::wstring> GetSelectedFilesFromShellBrowser(IShellBrowser* pShellBrowser) {
    std::vector<std::wstring> files;
    if (!pShellBrowser) {
        return files;
    }
    
    IShellView* pShellView = nullptr;
    if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)) && pShellView) {
        IFolderView* pFolderView = nullptr;
        if (SUCCEEDED(pShellView->QueryInterface(IID_IFolderView, (void**)&pFolderView)) && pFolderView) {
            // Get the folder interface
            IShellFolder* pFolder = nullptr;
            if (SUCCEEDED(pFolderView->GetFolder(IID_IShellFolder, (void**)&pFolder)) && pFolder) {
                // Get selected items
                IEnumIDList* pEnum = nullptr;
                if (SUCCEEDED(pFolderView->Items(SVGIO_SELECTION, IID_IEnumIDList, (void**)&pEnum)) && pEnum) {
                    LPITEMIDLIST pidl = nullptr;
                    while (pEnum->Next(1, &pidl, nullptr) == S_OK) {
                        // Use StrRetToStrW (heap-allocated, no length cap)
                        // instead of a fixed MAX_PATH buffer, since a
                        // truncated deep path would lose its extension.
                        STRRET strret;
                        if (SUCCEEDED(pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &strret))) {
                            LPWSTR pszPath = nullptr;
                            if (SUCCEEDED(StrRetToStrW(&strret, pidl, &pszPath)) && pszPath) {
                                if (pszPath[0]) {
                                    files.push_back(std::wstring(pszPath));
                                }
                                CoTaskMemFree(pszPath);
                            }
                        }
                        CoTaskMemFree(pidl);
                    }
                    pEnum->Release();
                }
                pFolder->Release();
            }
            
            pFolderView->Release();
        }
        pShellView->Release();
    }
    
    return files;
}

// Function to get selected files from Explorer, using the exact hWnd that
// TrackPopupMenu(Ex) was called with, so no cross-process/cross-tab
// guessing is needed. kind is the caller's ClassifyShellView() result.
std::vector<std::wstring> GetSelectedFilesFromExplorer(HWND hwnd, ShellViewKind kind) {
    std::vector<std::wstring> files;
    tl_selectionLookupFailed = false;
    
    if (!hwnd) {
        Wh_Log(L"GetSelectedFilesFromExplorer: No hWnd provided");
        tl_selectionLookupFailed = true;
        return files;
    }
    
    if (kind == ShellViewKind::Desktop) {
        IShellBrowser* pShellBrowser = GetDesktopShellBrowser();
        if (!pShellBrowser) {
            Wh_Log(L"Could not get desktop IShellBrowser");
            tl_selectionLookupFailed = true;
            return files;
        }
        files = GetSelectedFilesFromShellBrowser(pShellBrowser);
        // Released here, on the creating thread and still inside the COM
        // apartment -- no session-scoped caching needed since this is the
        // only call site per session.
        pShellBrowser->Release();
        return files;

    }
    
    if (kind != ShellViewKind::ShellDefView) {
        // Either not a shell view menu at all, or a right-click in the
        // navigation pane, which has no IFolderView selection to read.
        // This is "no file context", not a failure -- filtering fails closed for it.
        Wh_Log(L"GetSelectedFilesFromExplorer: not a ShellDefView window, skipping");
        return files;
    }
    
    IShellBrowser* pShellBrowser = GetShellBrowser(hwnd);
    if (!pShellBrowser) {
        Wh_Log(L"Could not get IShellBrowser from window");
        tl_selectionLookupFailed = true;
        return files;
    }
    
    // CWM_GETISHELLBROWSER returns this pointer without an added reference
    // (it's owned by the frame window), so take a temporary one for the
    // duration of use -- cheap insurance against it being released elsewhere mid-call.
    pShellBrowser->AddRef();
    files = GetSelectedFilesFromShellBrowser(pShellBrowser);
    pShellBrowser->Release();
    
    return files;
}

// Settings structures
struct BloatwareSettings {
    bool removeOneDrive;
    bool removeCopilot;
    bool removeDefender;
    bool removeDesigner;
    bool removeClipchamp;
    bool removeMicrosoft365Copilot;
};

struct BasicSettings {
    bool removeOpen;
    bool removeOpenWith;
    bool removeCut;
    bool removeCopy;
    bool removeCreateShortcut;
    bool removeDelete;
    bool removeRename;
    bool removeSendTo;
    bool removeOpenInNewTab;
    bool removeOpenInNewWindow;
    bool removeEdit;
    bool removePlay;
    bool removePreview;
    bool removePrint;
    bool removeShare;
    bool removeRefresh;
    bool removeCopyAsPath;
    bool removeCustomizeFolder;
    bool removeFavorites;
    bool removePinToQuickAccess;
    bool removePinToStart;
    bool removeCast;
    bool removeGiveAccess;
    bool removeRestoreVersions;
    bool removeIncludeInLibrary;
    bool removeRotate;
    bool removeDisplaySettings;
    bool removePersonalize;
    bool removeSetAsDesktopBackground;
    bool removeView;
    bool removeSortBy;
    bool removeGroupBy;
    bool removeNew;
    bool removeProperties;
    bool removePaste;
    bool removeExtractAll;
};

struct AppSpecificSettings {
    bool removeSendWithQuickShare;
    bool removeVLCPlaylist;
    bool removeVLCPlay;
    bool removeAddToMediaPlayerQueue;
    bool removePlayWithMediaPlayer;
    bool removeEditInNotepad;
    bool removeEditInNotepadPlusPlus;
    bool removeEditWithPhotos;
    bool removeEditWithPaint;
    bool removeNvidiaControlPanel;
    bool removeOpenInTerminal;
    bool removeAlwaysKeepOnThisDevice;
    bool removeFreeUpSpace;
    bool removeWinRAR;
};

struct ExtensionFilteringSettings {
    bool enableExtensionFiltering;
    std::vector<std::wstring> notepadExtensions;
    bool enableWinRARFiltering;
    std::vector<std::wstring> winrarExtensions;
};

struct ModifierKeySettings {
    bool enableModifierOverride; // When true, holding Alt while right-clicking bypasses the mod.
};

struct {
    BloatwareSettings bloatwareItems;
    BasicSettings basicItems;
    AppSpecificSettings appSpecificItems;
    ExtensionFilteringSettings extensionFiltering;
    ModifierKeySettings modifierKeyOverride;
    std::vector<std::wstring> customItems;
} g_settings;

// Structure to hold menu item info
// Forward declarations (definitions further below) -- needed because
// InitializeMenuItems() below precomputes each entry's normalized text.
std::wstring RemoveAmpersands(const std::wstring& str);
std::wstring NormalizeString(const std::wstring& str);

// Forward declaration of the thread-local "should ProcessMenu() recurse
// into submenus itself" flag (fully defined further below, alongside
// tl_hMenuHook) -- see its own comment for why this isn't just !tl_hMenuHook.
extern thread_local bool tl_recurseSubmenus;

// Forward declarations of the per-session filter-toggle snapshots (fully
// defined further below, alongside tl_sessionNeedFiles) -- see their own
// comment for why ShouldRemoveMenuItem() uses these instead of a live read.
extern thread_local bool tl_sessionExtensionFilterOn;
extern thread_local bool tl_sessionWinRARFilterOn;

struct MenuItem {
    std::wstring text;
    bool* enabled;
    std::vector<std::wstring>* allowedExtensions;  // nullptr = no extension check
    const bool* filterEnabled;                     // toggle gating the check above
    bool greyedOnly; // If true, only remove when the item is greyed out (disabled)
};

// List of predefined menu items to check
std::vector<MenuItem> g_menuItems;

// Indexes g_menuItems by normalizedText for O(1) lookup. Stores an index
// rather than a pointer so it can't dangle if g_menuItems is ever resized.
std::unordered_map<std::wstring, size_t> g_menuItemsByText;

// Guards g_settings and g_menuItems against concurrent access from
// LoadSettings() (settings-change thread) and the popup menu hooks
// (Explorer UI threads).
std::mutex g_settingsMutex;

// Forward declaration; full definition (and the reason it exists instead
// of raw ::towlower) is further below, near the other string-matching helpers.
std::wstring ToLower(const std::wstring& str);

// Function to get file extension from path
std::wstring GetFileExtension(const std::wstring& path) {
    // Only look at the last path component, so a folder named "v1.0"
    // doesn't get mistaken for an extension. No MAX_PATH limit, unlike
    // PathFindExtensionW, and no Windows 8+ dependency, unlike PathCchFindExtension.
    size_t nameStart = path.find_last_of(L"\\/");
    nameStart = (nameStart == std::wstring::npos) ? 0 : nameStart + 1;
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring::npos || dot < nameStart || dot + 1 == path.size()) {
        return L"";
    }
    std::wstring ext = ToLower(path.substr(dot));
    return ext;
}

// Function to check if extension is in list
// extList entries are already lowercased once in LoadSettings(); compare directly.
bool IsExtensionInList(const std::wstring& ext, const std::vector<std::wstring>& extList) {
    if (ext.empty() || extList.empty()) {
        return false;
    }
    return std::ranges::find(extList, ext) != extList.end();
}

// Check if any of the current files has an extension in the list.
// Deliberately any-match, not all-match: a mixed selection keeps the item
// visible since it still works on the matching file (see extensionFiltering description).
bool AnyFileHasExtensionInList(const std::vector<std::wstring>& extList) {
    for (const auto& filePath : tl_filePaths) {
        std::wstring ext = GetFileExtension(filePath);
        if (IsExtensionInList(ext, extList)) {
            return true;
        }
    }
    return false;
}

// Function to initialize menu items based on settings
void InitializeMenuItems() {
    g_menuItems.clear();
    g_menuItems = {
        // Bloatware Items
        {L"Move to OneDrive", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"OneDrive に移動", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // ja-JP
        {L"Mover para o OneDrive", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Mover a OneDrive", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // es-MX
        {L"Přesunout na OneDrive", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // cs-CZ
        {L"Auf OneDrive verschieben", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // de-DE
        {L"OneDrive'a taşı", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // tr-TR
        {L"Przenieś do usługi OneDrive", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // pl-PL
        {L"Déplacer vers OneDrive", &g_settings.bloatwareItems.removeOneDrive, nullptr, nullptr, false}, // fr-FR
        
        {L"Always keep on this device", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"このデバイス上に常に保持する", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // ja-JP
        {L"Sempre manter neste dispositivo", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // pt-BR
        {L"Manter sempre neste dispositivo", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // pt-PT
        {L"Mantener siempre en este dispositivo", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // es-MX
        {L"Vždy ponechat na tomto zařízení", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // cs-CZ
        {L"Immer auf diesem Gerät behalten", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // de-DE
        {L"Her zaman bu cihazda tut", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // tr-TR
        {L"Zawsze przechowuj na tym urządzeniu", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // pl-PL
        {L"Toujours conserver sur cet appareil", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, nullptr, nullptr, false}, // fr-FR
        
        {L"Free up space", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"空き領域を増やす", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // ja-JP
        {L"Liberar espaço", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // pt-BR
        {L"Libertar espaço", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // pt-PT
        {L"Liberar espacio", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // es-MX
        {L"Uvolnit místo", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // cs-CZ
        {L"Bereinigen", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // de-DE
        {L"Alan boşalt", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // tr-TR
        {L"Zwolnij miejsce", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // pl-PL
        {L"Libérer de l'espace", &g_settings.appSpecificItems.removeFreeUpSpace, nullptr, nullptr, false}, // fr-FR
        
        {L"Ask Copilot", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"Copilot とチャットする", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // ja-JP
        {L"Perguntar ao Copilot", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Preguntar a Copilot", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // es-MX
        {L"Zeptat se Copilota", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // cs-CZ
        {L"Copilot fragen", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // de-DE
        {L"Copilot'a sor", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // tr-TR
        {L"Zapytaj aplikację Copilot", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // pl-PL
        {L"Demander à Copilot", &g_settings.bloatwareItems.removeCopilot, nullptr, nullptr, false}, // fr-FR
        
        {L"Scan with Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"Microsoft Defenderでスキャンする...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // ja-JP
        {L"Verificar com o Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // pt-BR
        {L"Analisar com o Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // pt-PT
        {L"Analizar con Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // es-MX
        {L"Prohledat pomocí programu Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // cs-CZ
        {L"Mit Microsoft Defender überprüfen...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // de-DE
        {L"Microsoft Defender ile tara...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // tr-TR
        {L"Skanuj za pomocą programu Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // pl-PL
        {L"Analyser avec Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // fr-FR
        {L"Проверка с использованием Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, nullptr, nullptr, false}, // ru-RU
        
        {L"Create with Designer", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"Designerで作成", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // ja-JP
        {L"Criar com o Designer", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Crear con Designer", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // es-MX
        {L"Vytvořit pomocí Designeru", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // cs-CZ
        {L"Mit Designer erstellen", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // de-DE
        {L"Designer ile oluştur", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // tr-TR
        {L"Utwórz za pomocą aplikacji Designer", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // pl-PL
        {L"Créer avec Designer", &g_settings.bloatwareItems.removeDesigner, nullptr, nullptr, false}, // fr-FR
        
        {L"Edit with Clipchamp", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"Clipchampで編集", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // ja-JP
        {L"Editar com o Clipchamp", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Editar con Clipchamp", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // es-MX
        {L"Upravit pomocí Clipchampu", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // cs-CZ
        {L"Mit Clipchamp bearbeiten", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // de-DE
        {L"Clipchamp ile düzenle", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // tr-TR
        {L"Edytuj za pomocą aplikacji Clipchamp", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // pl-PL
        {L"Modifier avec Clipchamp", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // fr-FR
        {L"Редактировать в Clipchamp", &g_settings.bloatwareItems.removeClipchamp, nullptr, nullptr, false}, // ru-RU
        
        {L"Ask Microsoft 365 Copilot", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // en-US, en-GB, en-AU, ja-JP
        {L"Perguntar ao Microsoft 365 Copilot", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Preguntar a Microsoft 365 Copilot", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // es-MX
        {L"Zeptat se Microsoft 365 Copilota", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // cs-CZ
        {L"Microsoft 365 Copilot fragen", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // de-DE
        {L"Microsoft 365 Copilot'a sor", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // tr-TR
        {L"Zapytaj aplikację Microsoft 365 Copilot", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // pl-PL
        {L"Demander à Microsoft 365 Copilot", &g_settings.bloatwareItems.removeMicrosoft365Copilot, nullptr, nullptr, false}, // fr-FR
        
        // Basic Items
        {L"Open", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"開く", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // ja-JP
        {L"Abrir", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // pt-BR, pt-PT, es-MX
        {L"Otevřít", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // cs-CZ
        {L"Öffnen", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // de-DE
        {L"Aç", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // tr-TR
        {L"Otwórz", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // pl-PL
        {L"Ouvrir", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // fr-FR
        {L"Открыть", &g_settings.basicItems.removeOpen, nullptr, nullptr, false}, // ru-RU
        
        {L"Open with", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"プログラムから開く", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // ja-JP
        {L"Abrir com", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Abrir con", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // es-MX
        {L"Otevřít v aplikaci", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // cs-CZ
        {L"Öffnen mit", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // de-DE
        {L"Birlikte aç", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // tr-TR
        {L"Otwórz za pomocą", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // pl-PL
        {L"Ouvrir avec", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // fr-FR
        {L"Открыть с помощью", &g_settings.basicItems.removeOpenWith, nullptr, nullptr, false}, // ru-RU
        
        {L"Cut", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"切り取り", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // ja-JP
        {L"Recortar", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // pt-BR
        {L"Cortar", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // pt-PT, es-MX
        {L"Vyjmout", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // cs-CZ
        {L"Ausschneiden", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // de-DE
        {L"Kes", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // tr-TR
        {L"Wytnij", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // pl-PL
        {L"Couper", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // fr-FR
        {L"Вырезать", &g_settings.basicItems.removeCut, nullptr, nullptr, false}, // ru-RU
        
        {L"Copy", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"コピー", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // ja-JP
        {L"Copiar", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // pt-BR, pt-PT, es-MX
        {L"Kopírovat", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // cs-CZ
        {L"Kopieren", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // de-DE
        {L"Kopyala", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // tr-TR
        {L"Kopiuj", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // pl-PL
        {L"Copier", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // fr-FR
        {L"Копировать", &g_settings.basicItems.removeCopy, nullptr, nullptr, false}, // ru-RU
        
        {L"Create shortcut", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"ショートカットの作成", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // ja-JP
        {L"Criar atalho", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Crear acceso directo", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // es-MX
        {L"Vytvořit zástupce", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // cs-CZ
        {L"Verknüpfung erstellen", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // de-DE
        {L"Kısayol oluştur", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // tr-TR
        {L"Utwórz skrót", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // pl-PL
        {L"Créer un raccourci", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // fr-FR
        {L"Создать ярлык", &g_settings.basicItems.removeCreateShortcut, nullptr, nullptr, false}, // ru-RU
        
        {L"Delete", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"削除", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // ja-JP
        {L"Excluir", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // pt-BR
        {L"Eliminar", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // pt-PT, es-MX
        {L"Odstranit", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // cs-CZ
        {L"Löschen", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // de-DE
        {L"Sil", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // tr-TR
        {L"Usuń", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // pl-PL
        {L"Supprimer", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // fr-FR
        {L"Удалить", &g_settings.basicItems.removeDelete, nullptr, nullptr, false}, // ru-RU
        
        {L"Rename", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"名前の変更", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // ja-JP
        {L"Renomear", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // pt-BR
        {L"Mudar o nome", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // pt-PT
        {L"Cambiar nombre", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // es-MX
        {L"Přejmenovat", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // cs-CZ
        {L"Umbenennen", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // de-DE
        {L"Yeniden adlandır", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // tr-TR
        {L"Zmień nazwę", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // pl-PL
        {L"Renommer", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // fr-FR
        {L"Переименовать", &g_settings.basicItems.removeRename, nullptr, nullptr, false}, // ru-RU
        
        {L"Send to", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"送る", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // ja-JP
        {L"Enviar para", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Enviar a", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // es-MX
        {L"Odeslat do", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // cs-CZ
        {L"Senden an", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // de-DE
        {L"Gönder", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // tr-TR
        {L"Wyślij do", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // pl-PL
        {L"Envoyer vers", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // fr-FR
        {L"Отправить", &g_settings.basicItems.removeSendTo, nullptr, nullptr, false}, // ru-RU
        
        {L"Open in new tab", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"新しいタブで開く", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // ja-JP
        {L"Abrir em uma nova guia", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // pt-BR
        {L"Abrir num novo separador", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // pt-PT
        {L"Abrir en nueva pestaña", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // es-MX
        {L"Otevřít na nové kartě", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // cs-CZ
        {L"In neuer Registerkarte öffnen", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // de-DE
        {L"Yeni sekmede aç", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // tr-TR
        {L"Otwórz w nowej karcie", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // pl-PL
        {L"Ouvrir dans un nouvel onglet", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // fr-FR
        {L"Открыть в новой вкладке", &g_settings.basicItems.removeOpenInNewTab, nullptr, nullptr, false}, // ru-RU
        
        {L"Open in new window", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"新しいウィンドウで開く", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // ja-JP
        {L"Abrir em uma nova janela", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // pt-BR
        {L"Abrir numa nova janela", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // pt-PT
        {L"Abrir en nueva ventana", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // es-MX
        {L"Otevřít v novém okně", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // cs-CZ
        {L"In neuem Fenster öffnen", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // de-DE
        {L"Yeni pencerede aç", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // tr-TR
        {L"Otwórz w nowym oknie", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // pl-PL
        {L"Ouvrir dans une nouvelle fenêtre", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // fr-FR
        {L"Открыть в новом окне", &g_settings.basicItems.removeOpenInNewWindow, nullptr, nullptr, false}, // ru-RU
        
        {L"Edit", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"編集", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // ja-JP
        {L"Editar", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // pt-BR, pt-PT, es-MX
        {L"Upravit", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // cs-CZ
        {L"Bearbeiten", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // de-DE
        {L"Düzenle", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // tr-TR
        {L"Edytuj", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // pl-PL
        {L"Modifier", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // fr-FR
        {L"Изменить", &g_settings.basicItems.removeEdit, nullptr, nullptr, false}, // ru-RU
        
        {L"Play", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"再生", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // ja-JP
        {L"Reproduzir", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Reproducir", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // es-MX
        {L"Přehrát", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // cs-CZ
        {L"Wiedergabe", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // de-DE
        {L"Oynat", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // tr-TR
        {L"Odtwarzaj", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // pl-PL
        {L"Lire", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // fr-FR
        {L"Воспроизвести", &g_settings.basicItems.removePlay, nullptr, nullptr, false}, // ru-RU
        
        {L"Preview", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"プレビュー", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // ja-JP
        {L"Visualizar", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // pt-BR
        {L"Pré-visualizar", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // pt-PT
        {L"Vista previa", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // es-MX
        {L"Náhled", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // cs-CZ
        {L"Vorschau", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // de-DE
        {L"Önizleme", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // tr-TR
        {L"Podgląd", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // pl-PL
        {L"Aperçu", &g_settings.basicItems.removePreview, nullptr, nullptr, false}, // fr-FR
        
        {L"Print", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"印刷", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // ja-JP
        {L"Imprimir", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // pt-BR, pt-PT, es-MX
        {L"Tisk", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // cs-CZ
        {L"Drucken", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // de-DE
        {L"Yazdır", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // tr-TR
        {L"Drukuj", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // pl-PL
        {L"Imprimer", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // fr-FR
        {L"Печать", &g_settings.basicItems.removePrint, nullptr, nullptr, false}, // ru-RU
        
        {L"Share", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"共有", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // ja-JP
        {L"Compartilhar", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // pt-BR
        {L"Partilhar", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // pt-PT
        {L"Compartir", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // es-MX
        {L"Sdílet", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // cs-CZ
        {L"Freigabe", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // de-DE
        {L"Paylaş", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // tr-TR
        {L"Udostępnij", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // pl-PL
        {L"Partager", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // fr-FR
        {L"Поделиться", &g_settings.basicItems.removeShare, nullptr, nullptr, false}, // ru-RU
        
        {L"Send with Quick Share", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"Enviar com o Compartilhamento Rápido", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // pt-BR
        {L"Enviar com Partilha Rápida", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // pt-PT
        {L"Enviar con Quick Share", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // es-MX
        {L"Odeslat pomocí Rychlého sdílení", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // cs-CZ
        {L"Mit Quick Share senden", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // de-DE
        {L"Quick Share ile gönder", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // tr-TR
        {L"Wyślij za pomocą funkcji Quick Share", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // pl-PL
        {L"Envoyer avec Quick Share", &g_settings.appSpecificItems.removeSendWithQuickShare, nullptr, nullptr, false}, // fr-FR
        
        {L"Refresh", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"最新の情報に更新", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // ja-JP
        {L"Atualizar", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Actualizar", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // es-MX
        {L"Aktualizovat", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // cs-CZ
        {L"Aktualisieren", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // de-DE
        {L"Yenile", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // tr-TR
        {L"Odśwież", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // pl-PL
        {L"Actualiser", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // fr-FR
        {L"Обновить", &g_settings.basicItems.removeRefresh, nullptr, nullptr, false}, // ru-RU
        
        {L"Copy as path", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"パスのコピー", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // ja-JP
        {L"Copiar como caminho", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Copiar como ruta de acceso", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // es-MX
        {L"Kopírovat jako cestu", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // cs-CZ
        {L"Als Pfad kopieren", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // de-DE
        {L"Yolu kopyala", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // tr-TR
        {L"Kopiuj jako ścieżkę", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // pl-PL
        {L"Copier comme chemin", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // fr-FR
        {L"Копировать как путь", &g_settings.basicItems.removeCopyAsPath, nullptr, nullptr, false}, // ru-RU
        
        {L"Customize this folder...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // en-US
        {L"このフォルダーのカスタマイズ...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // ja-JP
        {L"Customise this folder...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // en-GB, en-AU
        {L"Personalizar esta pasta...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Personalizar esta carpeta...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // es-MX
        {L"Přizpůsobit tuto složku...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // cs-CZ
        {L"Ordner anpassen...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // de-DE
        {L"Bu klasörü özelleştir...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // tr-TR
        {L"Dostosuj ten folder...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // pl-PL
        {L"Personnaliser ce dossier...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // fr-FR
        {L"Настроить папку...", &g_settings.basicItems.removeCustomizeFolder, nullptr, nullptr, false}, // ru-RU
        
        {L"Add to Favorites", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // en-US
        {L"お気に入りに追加", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // ja-JP
        {L"Add to Favourites", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // en-GB, en-AU
        {L"Adicionar aos Favoritos", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Agregar a favoritos", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // es-MX
        {L"Přidat k oblíbeným položkám", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // cs-CZ
        {L"Zu Favoriten hinzufügen", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // de-DE
        {L"Sık kullanılanlara ekle", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // tr-TR
        {L"Dodaj do ulubionych", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // pl-PL
        {L"Ajouter aux favoris", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // fr-FR
        {L"Добавить в избранное", &g_settings.basicItems.removeFavorites, nullptr, nullptr, false}, // ru-RU
        
        {L"Pin to Quick access", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"クイック アクセスにピン留めする", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // ja-JP
        {L"Fixar no Acesso Rápido", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // pt-BR
        {L"Afixar no Acesso rápido", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // pt-PT
        {L"Anclar a Acceso rápido", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // es-MX
        {L"Připnout na Rychlý přístup", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // cs-CZ
        {L"An Schnellzugriff anheften", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // de-DE
        {L"Hızlı erişime sabitle", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // tr-TR
        {L"Przypnij do obszaru Szybki dostęp", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // pl-PL
        {L"Épingler à Accès rapide", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // fr-FR
        {L"Закрепить на панели быстрого доступа", &g_settings.basicItems.removePinToQuickAccess, nullptr, nullptr, false}, // ru-RU
        
        {L"Pin to Start", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"スタートにピン留めする", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // ja-JP
        {L"Fixar em Iniciar", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // pt-BR
        {L"Afixar no Iniciar", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // pt-PT
        {L"Anclar a Inicio", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // es-MX
        {L"Připnout na Start", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // cs-CZ
        {L"An \"Start\" anheften", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // de-DE
        {L"Başlat'a sabitle", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // tr-TR
        {L"Przypnij do menu Start", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // pl-PL
        {L"Épingler au menu Démarrer", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // fr-FR
        {L"Закрепить в меню \"Пуск\"", &g_settings.basicItems.removePinToStart, nullptr, nullptr, false}, // ru-RU
        
        {L"Cast to Device", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"デバイス キャスト", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // ja-JP
        {L"Transmitir para Dispositivo", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // pt-BR
        {L"Transmitir para o Dispositivo", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // pt-PT
        {L"Transmitir en dispositivo", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // es-MX
        {L"Přetypovat do zařízení", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // cs-CZ
        {L"Wiedergabe auf Gerät", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // de-DE
        {L"Cihaza yayınla", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // tr-TR
        {L"Przesyłanie na urządzenie", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // pl-PL
        {L"Diffuser vers un appareil", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // fr-FR
        {L"Передать на устройство", &g_settings.basicItems.removeCast, nullptr, nullptr, false}, // ru-RU
        
        {L"Give access to", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"アクセスを許可する", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // ja-JP
        {L"Conceder acesso a", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Dar acceso a", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // es-MX
        {L"Poskytnout přístup k", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // cs-CZ
        {L"Freigeben für", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // de-DE
        {L"Erişim ver", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // tr-TR
        {L"Udziel dostępu do", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // pl-PL
        {L"Donner l'accès à", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // fr-FR
        {L"Предоставить доступ к", &g_settings.basicItems.removeGiveAccess, nullptr, nullptr, false}, // ru-RU
        
        {L"Restore previous versions", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"以前のバージョンの復元", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // ja-JP
        {L"Restaurar versões anteriores", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Restaurar versiones anteriores", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // es-MX
        {L"Obnovit předchozí verze", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // cs-CZ
        {L"Vorgängerversionen wiederhestellen", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // de-DE
        {L"Önceki sürümleri geri yükle", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // tr-TR
        {L"Przywróć poprzednie wersje", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // pl-PL
        {L"Restaurer les versions précédentes", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // fr-FR
        {L"Восстановить прежнюю версию", &g_settings.basicItems.removeRestoreVersions, nullptr, nullptr, false}, // ru-RU
        
        {L"Include in library", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"ライブラリに追加", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // ja-JP
        {L"Incluir na biblioteca", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Incluir en biblioteca", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // es-MX
        {L"Zahrnout do knihovny", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // cs-CZ
        {L"In Bibliothek aufnehmen", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // de-DE
        {L"Kitaplığa ekle", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // tr-TR
        {L"Dołącz do biblioteki", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // pl-PL
        {L"Inclure dans la bibliothèque", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // fr-FR
        {L"Добавить в библиотеку", &g_settings.basicItems.removeIncludeInLibrary, nullptr, nullptr, false}, // ru-RU
        
        {L"Rotate right", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"右に回転", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // ja-JP
        {L"Girar para a direita", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // pt-BR
        {L"Rodar para a direita", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // pt-PT
        {L"Girar a la derecha", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // es-MX
        {L"Otočit doprava", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // cs-CZ
        {L"Nach rechts drehen", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // de-DE
        {L"Sağa döndür", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // tr-TR
        {L"Obróć w prawo", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // pl-PL
        {L"Faire pivoter vers la droite", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // fr-FR
        {L"Повернуть вправо", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // ru-RU
        
        {L"Rotate left", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"左に回転", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // ja-JP
        {L"Girar para a esquerda", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // pt-BR
        {L"Rodar para a esquerda", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // pt-PT
        {L"Girar a la izquierda", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // es-MX
        {L"Otočit doleva", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // cs-CZ
        {L"Nach links drehen", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // de-DE
        {L"Sola döndür", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // tr-TR
        {L"Obróć w lewo", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // pl-PL
        {L"Faire pivoter vers la gauche", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // fr-FR
        {L"Повернуть влево", &g_settings.basicItems.removeRotate, nullptr, nullptr, false}, // ru-RU
        
        {L"Display settings", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"ディスプレイ設定", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // ja-JP
        {L"Configurações de vídeo", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // pt-BR
        {L"Definições do ecrã", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // pt-PT
        {L"Configuración de pantalla", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // es-MX
        {L"Nastavení zobrazení", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // cs-CZ
        {L"Anzeigeeinstellungen", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // de-DE
        {L"Görüntü ayarları", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // tr-TR
        {L"Ustawienia ekranu", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // pl-PL
        {L"Paramètres d'affichage", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // fr-FR
        {L"Параметры экрана", &g_settings.basicItems.removeDisplaySettings, nullptr, nullptr, false}, // ru-RU
        
        {L"Personalize", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // en-US
        {L"個人用設定", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // ja-JP
        {L"Personalise", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // en-GB, en-AU
        {L"Personalizar", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // pt-BR, pt-PT, es-MX
        {L"Přizpůsobit", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // cs-CZ
        {L"Anpassen", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // de-DE
        {L"Kişiselleştir", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // tr-TR
        {L"Personalizuj", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // pl-PL
        {L"Personnaliser", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // fr-FR
        {L"Персонализация", &g_settings.basicItems.removePersonalize, nullptr, nullptr, false}, // ru-RU
        
        {L"Set as desktop background", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"デスクトップの背景として設定", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // ja-JP
        {L"Definir como plano de fundo da área de trabalho", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // pt-BR
        {L"Definir como fundo do ambiente de trabalho", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // pt-PT
        {L"Establecer como fondo de escritorio", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // es-MX
        {L"Nastavit jako pozadí plochy", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // cs-CZ
        {L"Als Desktophintergrund festlegen", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // de-DE
        {L"Masaüstü arka planı olarak ayarla", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // tr-TR
        {L"Ustaw jako tło pulpitu", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // pl-PL
        {L"Définir comme arrière-plan du Bureau", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // fr-FR
        {L"Сделать фоном рабочего стола", &g_settings.basicItems.removeSetAsDesktopBackground, nullptr, nullptr, false}, // ru-RU
        
        {L"View", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"表示", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // ja-JP
        {L"Exibir", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // pt-BR
        {L"Ver", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // pt-PT, es-MX
        {L"Zobrazení", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // cs-CZ
        {L"Ansicht", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // de-DE
        {L"Görünüm", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // tr-TR
        {L"Widok", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // pl-PL
        {L"Affichage", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // fr-FR
        {L"Вид", &g_settings.basicItems.removeView, nullptr, nullptr, false}, // ru-RU
        
        {L"Sort by", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"並べ替え", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // ja-JP
        {L"Classificar por", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // pt-BR
        {L"Ordenar por", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // pt-PT, es-MX
        {L"Seřadit podle", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // cs-CZ
        {L"Sortieren nach", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // de-DE
        {L"Sıralama ölçütü", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // tr-TR
        {L"Sortuj według", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // pl-PL
        {L"Trier par", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // fr-FR
        {L"Сортировка", &g_settings.basicItems.removeSortBy, nullptr, nullptr, false}, // ru-RU
        
        {L"Group by", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"グループで表示", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // ja-JP
        {L"Agrupar por", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // pt-BR, pt-PT, es-MX
        {L"Seskupit podle", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // cs-CZ
        {L"Gruppieren nach", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // de-DE
        {L"Gruplama ölçütü", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // tr-TR
        {L"Grupuj według", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // pl-PL
        {L"Regrouper par", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // fr-FR
        {L"Группировка", &g_settings.basicItems.removeGroupBy, nullptr, nullptr, false}, // ru-RU
        
        {L"New", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"新規作成", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // ja-JP
        {L"Novo", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Nuevo", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // es-MX
        {L"Nový", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // cs-CZ
        {L"Neu", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // de-DE
        {L"Yeni", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // tr-TR
        {L"Nowy", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // pl-PL
        {L"Nouveau", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // fr-FR
        {L"Создать", &g_settings.basicItems.removeNew, nullptr, nullptr, false}, // ru-RU
        
        {L"Properties", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"プロパティ", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // ja-JP
        {L"Propriedades", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Propiedades", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // es-MX
        {L"Vlastnosti", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // cs-CZ
        {L"Eigenschaften", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // de-DE
        {L"Özellikler", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // tr-TR
        {L"Właściwości", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // pl-PL
        {L"Propriétés", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // fr-FR
        {L"Свойства", &g_settings.basicItems.removeProperties, nullptr, nullptr, false}, // ru-RU
        
        {L"Paste", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // en-US, en-GB, en-AU
        {L"貼り付け", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // ja-JP
        {L"Colar", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // pt-BR, pt-PT
        {L"Pegar", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // es-MX
        {L"Vložit", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // cs-CZ
        {L"Einfügen", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // de-DE
        {L"Yapıştır", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // tr-TR
        {L"Wklej", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // pl-PL
        {L"Coller", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // fr-FR
        {L"Вставить", &g_settings.basicItems.removePaste, nullptr, nullptr, true}, // ru-RU
        
        {L"Extract All...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"すべて展開...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // ja-JP
        {L"Extrair Tudo...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // pt-BR
        {L"Extrair Todos...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // pt-PT
        {L"Extraer todo...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // es-MX
        {L"Extrahovat vše...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // cs-CZ
        {L"Alle extrahieren...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // de-DE
        {L"Tümünü ayıkla...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // tr-TR
        {L"Wyodrębnij wszystkie...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // pl-PL
        {L"Extraire tout...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // fr-FR
        {L"Извлечь все...", &g_settings.basicItems.removeExtractAll, nullptr, nullptr, false}, // ru-RU
        
        // App-specific Items
        {L"Add to VLC media player's Playlist", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"VLCメディアプレイヤーのプレイリストに追加", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // ja-JP
        {L"Adicionar à lista de reprodução do VLC media player", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Agregar a la lista de reproducción de VLC media player", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // es-MX
        {L"Přidat do seznamu stop přehrávače médií VLC", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // cs-CZ
        {L"Zur VLC media player Wiedergabeliste hinzufügen", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // de-DE
        {L"VLC media player oynatma listesine ekle", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // tr-TR
        {L"Dodaj do listy odtwarzania programu VLC media player", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // pl-PL
        {L"Ajouter à la liste de lecture de VLC", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // fr-FR
        {L"Добавить в плейлист VLC", &g_settings.appSpecificItems.removeVLCPlaylist, nullptr, nullptr, false}, // ru-RU
        
        {L"Play with VLC media player", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"VLCメディアプレイヤーで再生", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // ja-JP
        {L"Reproduzir com o VLC media player", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Reproducir con VLC media player", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // es-MX
        {L"Přehrát přehrávačem médií VLC", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // cs-CZ
        {L"Mit VLC media player wiedergeben", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // de-DE
        {L"VLC media player ile oynat", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // tr-TR
        {L"Odtwórz w programie VLC media player", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // pl-PL
        {L"Lire avec VLC", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // fr-FR
        {L"Воспроизвести в VLC", &g_settings.appSpecificItems.removeVLCPlay, nullptr, nullptr, false}, // ru-RU
        
        {L"Add to Media Player play queue", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"メディアプレイヤーの再生キューに追加", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // ja-JP
        {L"Adicionar à fila de reprodução do Media Player", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Agregar a la cola de reproducción del Reproductor multimedia", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // es-MX
        {L"Přidat do fronty přehrávání přehrávače médií", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // cs-CZ
        {L"Zur Windows Media Player-Wiedergabeliste hinzufügen", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // de-DE
        {L"Media Player oynatma kuyruğuna ekle", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // tr-TR
        {L"Dodaj do kolejki odtwarzania programu Media Player", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // pl-PL
        {L"Ajouter à la file d'attente de Media Player", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, nullptr, nullptr, false}, // fr-FR
        
        {L"Play with Media Player", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"メディアプレイヤーで再生", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // ja-JP
        {L"Reproduzir com o Media Player", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Reproducir con el Reproductor de Windows Media", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // es-MX
        {L"Přehrát přehrávačem médií", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // cs-CZ
        {L"Mit Windows Media Player wiedergeben", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // de-DE
        {L"Media Player ile oynat", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // tr-TR
        {L"Odtwórz w programie Media Player", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // pl-PL
        {L"Lire avec Media Player", &g_settings.appSpecificItems.removePlayWithMediaPlayer, nullptr, nullptr, false}, // fr-FR
        
        {L"Edit in Notepad", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // en-US, en-GB, en-AU
        {L"メモ帳で編集", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // ja-JP
        {L"Editar no Bloco de Notas", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // pt-BR, pt-PT
        {L"Editar en el Bloc de notas", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // es-MX
        {L"Upravit v Poznámkovém bloku", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // cs-CZ
        {L"Im Editor bearbeiten", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // de-DE
        {L"Not Defteri'nde düzenle", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // tr-TR
        {L"Edytuj w Notatniku", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // pl-PL
        {L"Modifier dans le Bloc-notes", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // fr-FR
        {L"Изменить в Блокноте", &g_settings.appSpecificItems.removeEditInNotepad, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // ru-RU
        
        {L"Edit with Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // en-US, en-GB, en-AU, ja-JP
        {L"Editar no Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // pt-BR, pt-PT
        {L"Editar con Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // es-MX
        {L"Upravit v aplikaci Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // cs-CZ
        {L"Mit Notepad++ bearbeiten", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // de-DE
        {L"Notepad++ ile düzenle", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // tr-TR
        {L"Edytuj w Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // pl-PL
        {L"Modifier avec Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // fr-FR
        {L"Редактировать в Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, &g_settings.extensionFiltering.notepadExtensions, &g_settings.extensionFiltering.enableExtensionFiltering, false}, // ru-RU
        
        {L"Edit with Photos", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"フォトで編集", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // ja-JP
        {L"Editar com Fotos", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // pt-BR
        {L"Editar com Fotografias", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // pt-PT
        {L"Editar con Fotos", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // es-MX
        {L"Upravit pomocí Fotky", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // cs-CZ
        {L"Mit Fotos bearbeiten", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // de-DE
        {L"Fotoğraflar ile düzenle", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // tr-TR
        {L"Edytuj w Zdjęcia", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // pl-PL
        {L"Modifier avec Photos", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // fr-FR
        {L"Изменить с помощью приложения \"Фотографии\"", &g_settings.appSpecificItems.removeEditWithPhotos, nullptr, nullptr, false}, // ru-RU
        
        {L"Edit with Paint", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"ペイントで編集する", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // ja-JP
        {L"Editar com o Paint", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Editar con Paint", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // es-MX
        {L"Upravit pomocí Malování", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // cs-CZ
        {L"Mit Paint bearbeiten", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // de-DE
        {L"Paint ile düzenle", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // tr-TR
        {L"Edytuj w Paint", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // pl-PL
        {L"Modifier avec Paint", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // fr-FR
        {L"Редактирование с помощью приложения Paint", &g_settings.appSpecificItems.removeEditWithPaint, nullptr, nullptr, false}, // ru-RU
        
        {L"NVIDIA Control Panel", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // en-US, en-GB, en-AU
        {L"NVIDIA コントロールパネル", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // ja-JP
        {L"Painel de Controle NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // pt-BR
        {L"Painel de Controlo da NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // pt-PT
        {L"Panel de control de NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // es-MX
        {L"Ovládací panely NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // cs-CZ
        {L"NVIDIA Systemsteuerung", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // de-DE
        {L"NVIDIA Denetim Masası", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // tr-TR
        {L"Panel sterowania NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // pl-PL
        {L"Panneau de configuration NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // fr-FR
        {L"Панель управления NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, nullptr, nullptr, false}, // ru-RU
        
        {L"Open in Terminal", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // en-US, en-GB, en-AU, ja-JP
        {L"Abrir no Terminal", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // pt-BR, pt-PT
        {L"Abrir en Terminal", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // es-MX
        {L"Otevřít v terminálu", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // cs-CZ
        {L"In Terminal öffnen", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // de-DE
        {L"Terminal'de aç", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // tr-TR
        {L"Otwórz w Terminalu", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // pl-PL
        {L"Ouvrir dans le Terminal", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // fr-FR
        {L"Открыть в Терминале", &g_settings.appSpecificItems.removeOpenInTerminal, nullptr, nullptr, false}, // ru-RU
        
        {L"WinRAR", &g_settings.appSpecificItems.removeWinRAR, &g_settings.extensionFiltering.winrarExtensions, &g_settings.extensionFiltering.enableWinRARFiltering, false} // all
    };
    
    // Index by normalized text so lookup is a single hash-map probe
    // instead of a linear scan over all ~564 entries.
    g_menuItemsByText.clear();
    for (size_t i = 0; i < g_menuItems.size(); i++) {
        std::wstring normalizedText = NormalizeString(RemoveAmpersands(g_menuItems[i].text));
        auto insertResult = g_menuItemsByText.insert({normalizedText, i});
        if (!insertResult.second) {
            // insert() keeps the first entry and silently drops this one on
            // a collision, leaving entry i's toggle unreachable. No
            // collisions exist today, but this warns whoever adds the next translation, instead of a silently dead toggle.
            Wh_Log(L"WARNING: Menu item text '%s' (entry %d) collides with an "
                   L"already-registered entry (%d) after normalization -- the "
                   L"newer entry's toggle will never be reachable by lookup",
                   g_menuItems[i].text.c_str(), (int)i, (int)insertResult.first->second);
        }
    }
}


// Utility function to remove ampersands for hotkey underlines. A single '&'
// is Windows' hotkey-underline escape (stripped); a doubled '&&' is a
// literal '&' in visible text, which must collapse to one, not vanish.
std::wstring RemoveAmpersands(const std::wstring& str) {
    std::wstring result;
    result.reserve(str.length());
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == L'&') {
            if (i + 1 < str.length() && str[i + 1] == L'&') {
                // Literal '&&' -> keep a single '&' and skip both source chars.
                result += L'&';
                i++;
            }
            // else: lone '&' hotkey-underline escape -- drop it.
        } else {
            result += str[i];
        }
    }
    return result;
}

// Utility function to convert string to lowercase
std::wstring ToLower(const std::wstring& str) {
    // CharLowerBuffW covers the full Unicode range, unlike ::towlower in
    // the CRT's default "C" locale, which only maps ASCII A-Z -- needed
    // so non-Latin Custom Items (Cyrillic, Greek, Turkish, etc.) can match.
    std::wstring result = str;
    if (!result.empty()) {
        CharLowerBuffW(result.data(), (DWORD)result.size());
    }
    return result;
}

// Utility function to normalize string for comparison
std::wstring NormalizeString(const std::wstring& str) {
    // Menu items can render with a tab-separated accelerator suffix (e.g.
    // "Copy\tCtrl+C"). Cut that off before comparing, so entries still match on just the visible label.
    std::wstring text = str;
    size_t tabPos = text.find(L'\t');
    if (tabPos != std::wstring::npos) {
        text = text.substr(0, tabPos);
    }
    
    // Some shell entries use the single-character ellipsis (U+2026)
    // instead of three ASCII dots -- normalize to the latter so both
    // spellings compare equal against the predefined table.
    size_t ellipsisPos;
    while ((ellipsisPos = text.find(L'\u2026')) != std::wstring::npos) {
        text.replace(ellipsisPos, 1, L"...");
    }
    
    std::wstring result = ToLower(text);
    // Remove leading/trailing whitespace
    size_t start = result.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) return L"";
    size_t end = result.find_last_not_of(L" \t\r\n");
    return result.substr(start, end - start + 1);
}

// Check if text matches a custom item pattern
bool MatchesCustomItem(const std::wstring& text, const std::wstring& pattern) {
    if (pattern.empty()) return false;
    
    // Check if pattern ends with wildcard
    if (pattern.back() == L'*') {
        // Prefix matching - remove the asterisk and compare
        std::wstring prefix = pattern.substr(0, pattern.length() - 1);
        return text.compare(0, prefix.length(), prefix) == 0;
    } else {
        // Exact matching
        return text == pattern;
    }
}

// Checks if a menu item should be removed based on extension filtering.
// Called only when the caller has already confirmed the relevant filter
// toggle is on and item.allowedExtensions is set.
bool ShouldRemoveByExtension(const MenuItem& item) {
    if (tl_filePaths.empty()) {
        if (tl_selectionLookupFailed) {
            // The lookup itself failed (no shell browser found, etc.), not
            // a genuine empty selection. Don't fail closed here, since
            // the file might match the whitelist and we couldn't check.
            return false;
        }
        // No file context at all (right-clicking empty space, in a regular
        // folder or a virtual one like This PC/Recycle Bin/Libraries).
        // Explicitly hide, so behavior is the same everywhere regardless of whether a path happened to be available.
        return true;
    }
    
    // Check if ANY of the selected files has an extension in the whitelist
    bool hasMatchingExtension = AnyFileHasExtensionInList(*item.allowedExtensions);
    
    // Whitelist mode: Remove if NO file has a matching extension
    return !hasMatchingExtension;
}

// Function to check if a menu item should be removed
bool ShouldRemoveMenuItem(const std::wstring& text, bool isGreyed) {
    std::wstring cleanText = NormalizeString(RemoveAmpersands(text));
    
    // Custom items are user-authored and additive: they must apply even
    // when the same label also exists in the predefined table with its
    // toggle off. Checking them first is why "Copy"/"Open*" README examples work.
    for (const auto& cleanCustomItem : g_settings.customItems) {
        if (MatchesCustomItem(cleanText, cleanCustomItem)) {
            return true;
        }
    }
    
    // Check against predefined items (single hash lookup instead of a
    // linear scan over all ~564 entries)
    auto it = g_menuItemsByText.find(cleanText);
    if (it != g_menuItemsByText.end()) {
        const MenuItem& item = g_menuItems[it->second];
        bool isEnabled = *(item.enabled);
        
        // If this item should only be removed when greyed out, check the state first
        if (item.greyedOnly && !isGreyed) {
            return false;
        }
        
        // Special handling for extension-filtered items (Notepad and WinRAR)
        if (item.allowedExtensions) {
            if (isEnabled) {
                // filterEnabled is set explicitly per entry (not inferred
                // from allowedExtensions), and the two session snapshots
                // avoid a live re-read that could disagree mid-session.
                bool filterOn = (item.filterEnabled == &g_settings.extensionFiltering.enableWinRARFiltering)
                    ? tl_sessionWinRARFilterOn
                    : tl_sessionExtensionFilterOn;
                
                if (filterOn) {
                    // Filter is on: only remove if extension is not in whitelist
                    return ShouldRemoveByExtension(item);
                } else {
                    // Filter is off: removal toggle wins, hide globally
                    return true;
                }
            }
            // If the removal setting is OFF, never remove this item
            return false;
        }
        
        // Normal behavior for non-extension-filtered items
        return isEnabled;
    }
    
    return false;
}

// Function to check if the modifier key bypass is active
bool IsModifierKeyBypassActive() {
    if (!g_settings.modifierKeyOverride.enableModifierOverride) {
        return false;
    }
    
    // Alt is the only bypass key. Ctrl was intentionally dropped: it's also
    // used to build multi-selections in Explorer, so it's easy to still be
    // holding it down when you right-click, silently triggering the bypass.
    return (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
}

// Function to process a menu and remove unwanted items
void ProcessMenu(HMENU hMenu) {
    if (!hMenu) return;
    
    int itemCount = GetMenuItemCount(hMenu);
    bool anyRemoved = false;
    
    // Iterate through menu items in reverse to safely remove items
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE | MIIM_STATE;
        
        bool deleted = false;
        
        // Get the length of the menu item text (this call also fills
        // fState, since MIIM_STATE is in fMask -- no separate round trip
        // needed to check greyed state).
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            std::wstring text;
            bool haveText = false;
            
            if (mii.cch > 0) {
                // Allocate buffer and get the actual text
                text.assign(mii.cch + 1, L'\0');
                mii.dwTypeData = &text[0];
                mii.cch++;
                
                if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
                    text.resize(wcslen(text.c_str()));
                    haveText = true;
                }
            } else {
                // mii.cch == 0 means a separator or an owner-drawn
                // (MFT_OWNERDRAW) item; GetMenuStringW returns nothing
                // useful for either, so there's no text to fall back to.
                haveText = false;
            }
            
            if (haveText) {
                bool isGreyed = (mii.fState & MFS_GRAYED) != 0;
                
                // Check if this item should be removed
                if (ShouldRemoveMenuItem(text, isGreyed)) {
                    // RemoveMenu detaches (doesn't destroy) the submenu so a
                    // third-party owner can still manage it; we now own the
                    // handle otherwise, so stash it for ExitMenuTracking().
                    HMENU hDetached = mii.hSubMenu;
                    RemoveMenu(hMenu, i, MF_BYPOSITION);
                    if (hDetached) {
                        tl_detachedSubmenus.push_back(hDetached);
                    }
                    deleted = true;
                    anyRemoved = true;
                }
            }
            
            // Recursively process submenus, but only if we didn't just
            // delete this item, and only when nothing else will re-filter
            // this submenu itself (see tl_recurseSubmenus).
            if (!deleted && mii.hSubMenu && tl_recurseSubmenus) {
                ProcessMenu(mii.hSubMenu);
            }
        }
    }
    
    // Separator cleanup below only ever matters if something was actually
    // removed from this menu -- an untouched menu's separators are already
    // however Explorer built it, so skip mutating it at all in that case.
    if (!anyRemoved) {
        return;
    }
    
    // Remove consecutive separators
    itemCount = GetMenuItemCount(hMenu);
    bool lastWasSeparator = false;
    
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            bool isSeparator = (mii.fType & MFT_SEPARATOR) != 0;
            
            if (isSeparator && lastWasSeparator) {
                // Remove consecutive separator
                DeleteMenu(hMenu, i, MF_BYPOSITION);
            }
            
            lastWasSeparator = isSeparator;
        }
    }
    
    // Remove leading separator if no items exist before it
    itemCount = GetMenuItemCount(hMenu);
    if (itemCount > 0) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, 0, TRUE, &mii)) {
            if (mii.fType & MFT_SEPARATOR) {
                DeleteMenu(hMenu, 0, MF_BYPOSITION);
            }
        }
    }
    
    // Remove trailing separator if exists
    itemCount = GetMenuItemCount(hMenu);
    if (itemCount > 0) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE;
        
        if (GetMenuItemInfoW(hMenu, itemCount - 1, TRUE, &mii)) {
            if (mii.fType & MFT_SEPARATOR) {
                DeleteMenu(hMenu, itemCount - 1, MF_BYPOSITION);
            }
        }
    }
}

// Original function pointers
using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

using TrackPopupMenu_t = decltype(&TrackPopupMenu);
TrackPopupMenu_t TrackPopupMenu_Original;

// Tracks active thread-local menu hooks so Wh_ModUninit can force-unhook
// any still active -- a HHOOK outliving the mod's loaded image is a crash.
// g_uninitInProgress ensures no hook installs after Wh_ModUninit's sweep.
std::mutex g_activeMenuHooksMutex;
std::vector<HHOOK> g_activeMenuHooks;
bool g_uninitInProgress = false;

// WM_INITMENUPOPUP is a *sent* message delivered straight to the owner
// window's WndProc during the menu's modal loop, so a thread-local
// WH_CALLWNDPROCRET hook is how to intercept it (same as other merged mods).
thread_local HHOOK tl_hMenuHook = nullptr;
thread_local int tl_menuDepth = 0;
// Whether ProcessMenu() should recurse into submenus itself, rather than
// leaving them to MenuCallWndProcRetHook via WM_INITMENUPOPUP. True when
// no hook is installed, or when TPM_NONOTIFY means that message won't come.
thread_local bool tl_recurseSubmenus = true;
// Decided once per tracking session in EnterMenuTracking(), so the
// top-level menu and every submenu agree on whether the bypass is active,
// even if the Alt key state changes while the menu is still open.
thread_local bool tl_menuBypassed = false;
// True if this session successfully called CoInitializeEx (see
// EnterMenuTracking) and owns a matching CoUninitialize call in ExitMenuTracking.
thread_local bool tl_sessionComInitialized = false;
// Whether extension filtering needs the selected-file lookup, computed
// once per session in EnterMenuTracking() and reused by ProcessPopupMenu()
// instead of a second read that could disagree if settings change mid-session.
thread_local bool tl_sessionNeedFiles = false;
// Snapshots of the two filter toggles, taken alongside tl_sessionNeedFiles
// so a settings change mid-session can't disagree with the file lookup
// that already ran (or didn't) based on the toggles' state at menu-open time.
thread_local bool tl_sessionExtensionFilterOn = false;
thread_local bool tl_sessionWinRARFilterOn = false;

LRESULT CALLBACK MenuCallWndProcRetHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        CWPRETSTRUCT* cwp = (CWPRETSTRUCT*)lParam;
        if (cwp->message == WM_INITMENUPOPUP) {
            HMENU hSubMenu = (HMENU)cwp->wParam;
            if (hSubMenu) {
                // tl_filePaths was populated before tracking started and
                // stays valid for the whole session. Don't remove the
                // parent item when filtering empties a submenu -- it's already displayed, so mutating it risks a handle/layout bug.
                std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
                ProcessMenu(hSubMenu);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// Installs the thread-local hook on the outermost TrackPopupMenu(Ex) call
// (nested calls bump the depth counter). SetWindowsHookEx runs outside
// g_activeMenuHooksMutex; COM inits once per session outside that lock too.
void EnterMenuTracking() {
    if (tl_menuDepth == 0) {
        tl_selectionLookupFailed = false;
        bool needFiles = false;
        {
            std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
            tl_menuBypassed = IsModifierKeyBypassActive();
            tl_sessionExtensionFilterOn = g_settings.extensionFiltering.enableExtensionFiltering;
            tl_sessionWinRARFilterOn = g_settings.extensionFiltering.enableWinRARFiltering;
            needFiles = tl_sessionExtensionFilterOn || tl_sessionWinRARFilterOn;
        }
        tl_sessionNeedFiles = needFiles;
        tl_recurseSubmenus = true;
        if (!tl_menuBypassed) {
            HHOOK hHook = SetWindowsHookEx(WH_CALLWNDPROCRET, MenuCallWndProcRetHook,
                                            nullptr, GetCurrentThreadId());
            if (hHook) {
                bool uninitRacing = false;
                {
                    std::lock_guard<std::mutex> lock(g_activeMenuHooksMutex);
                    uninitRacing = g_uninitInProgress;
                    if (!uninitRacing) {
                        tl_hMenuHook = hHook;
                        g_activeMenuHooks.push_back(hHook);
                        // The hook will re-filter submenus via
                        // WM_INITMENUPOPUP; ProcessPopupMenu() flips this
                        // back if TPM_NONOTIFY means that won't happen.
                        tl_recurseSubmenus = false;
                    }
                }
                // Wh_ModUninit's sweep can't have seen this handle -- it
                // wasn't in the registry yet -- so unhook it ourselves.
                if (uninitRacing) {
                    UnhookWindowsHookEx(hHook);
                }
            } else {
                Wh_Log(L"SetWindowsHookEx failed (err %lu); submenus won't be filtered",
                       GetLastError());
            }
            if (needFiles) {
                HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
                tl_sessionComInitialized = SUCCEEDED(hrCom);
            }
        }
    }
    tl_menuDepth++;
}

// Tears down the hook and clears the selected-file cache once the
// outermost TrackPopupMenu(Ex) call has returned.
void ExitMenuTracking() {
    tl_menuDepth--;
    if (tl_menuDepth <= 0) {
        tl_menuDepth = 0;
        if (tl_hMenuHook) {
            HHOOK hook = tl_hMenuHook;
            tl_hMenuHook = nullptr;
            // Unhook while still holding the lock, so the handle is always
            // either registered-and-hooked or removed-and-unhooked, never
            // caught in between -- see EnterMenuTracking() for the other half.
            std::lock_guard<std::mutex> lock(g_activeMenuHooksMutex);
            if (std::erase(g_activeMenuHooks, hook) > 0) {
                UnhookWindowsHookEx(hook);
            }
        }
        if (tl_sessionComInitialized) {
            CoUninitialize();
            tl_sessionComInitialized = false;
        }
        // Runs after TrackPopupMenu(Ex)_Original has returned, so a handle
        // here is already unreachable from the shell's own destroyed tree.
        // RemoveMenu's contract makes us the sole owner of each handle.
        for (HMENU h : tl_detachedSubmenus) {
            DestroyMenu(h);
        }
        tl_detachedSubmenus.clear();
        tl_filePaths.clear();
    }
}

// RAII pairing for Enter/ExitMenuTracking, so a thrown exception between
// them (e.g. std::bad_alloc) can't leave the hook installed forever.
struct MenuTrackingGuard {
    MenuTrackingGuard() { EnterMenuTracking(); }
    ~MenuTrackingGuard() { ExitMenuTracking(); }
};

// Shared logic for both TrackPopupMenu(Ex) hooks: file lookup and, as a
// fallback, menu filtering for the top-level menu. kind is the caller's
// already-computed ClassifyShellView() result, reused to avoid a re-walk.
void ProcessPopupMenu(HMENU hMenu, HWND hWnd, UINT uFlags, ShellViewKind kind, const wchar_t* logPrefix) {
    if (tl_menuBypassed) {
        Wh_Log(L"%s: Modifier key bypass active, skipping menu processing", logPrefix);
        return;
    }
    
    // TPM_NONOTIFY suppresses WM_INITMENUPOPUP, so MenuCallWndProcRetHook
    // will never see this menu's submenus -- fall back to recursing here.
    if (uFlags & TPM_NONOTIFY) {
        tl_recurseSubmenus = true;
    }
    
    // Only do the (COM-based) selected-file lookup on the outermost call
    // of this session: TrackPopupMenu is implemented on top of
    // TrackPopupMenuEx, so a nested call would repeat the same COM round trip for an identical result.
    if (tl_menuDepth == 1) {
        if (tl_sessionNeedFiles) {
            tl_filePaths = GetSelectedFilesFromExplorer(hWnd, kind);
            
            if (!tl_filePaths.empty()) {
                Wh_Log(L"%s called with %d files:", logPrefix, (int)tl_filePaths.size());
                for (const auto& path : tl_filePaths) {
                    Wh_Log(L"  - %s (ext: %s)", path.c_str(), GetFileExtension(path).c_str());
                }
            } else {
                Wh_Log(L"%s called with no file context", logPrefix);
            }
        }
    }
    
    // MenuCallWndProcRetHook re-processes this menu via WM_INITMENUPOPUP,
    // but TPM_NONOTIFY can suppress that message, so run this pass
    // unconditionally too -- cheap, since an already-filtered menu removes nothing.
    {
        std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
        ProcessMenu(hMenu);
    }
    
    // NOTE: tl_filePaths is deliberately NOT cleared here -- it stays
    // populated for submenus processed later via MenuCallWndProcRetHook,
    // cleared in ExitMenuTracking() once the outermost call returns.
}

// Hook function for TrackPopupMenuEx
BOOL WINAPI TrackPopupMenuEx_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    HWND hWnd,
    LPTPMPARAMS lptpm
) {
    // Only filter genuine file/folder context menus. TrackPopupMenuEx is
    // hooked process-wide, so hWnd can also belong to the taskbar, tray
    // icons, or the Start menu.
    ShellViewKind kind = ClassifyShellView(hWnd);
    std::optional<MenuTrackingGuard> guard;
    if (kind != ShellViewKind::None) {
        guard.emplace();
        if (tl_menuDepth == 1) {
            // Outermost call for this session -- reset tl_filePaths here,
            // not before guard.emplace(), since clearing there would wipe
            // what an outer call already collected before a nested one skips repopulating it.
            tl_filePaths.clear();
        }
        ProcessPopupMenu(hMenu, hWnd, uFlags, kind, L"TrackPopupMenuEx");
    }
    
    BOOL bRes = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    
    return bRes;
}

// Hook function for TrackPopupMenu
BOOL WINAPI TrackPopupMenu_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    int nReserved,
    HWND hWnd,
    const RECT* prcRect
) {
    ShellViewKind kind = ClassifyShellView(hWnd);
    std::optional<MenuTrackingGuard> guard;
    if (kind != ShellViewKind::None) {
        guard.emplace();
        if (tl_menuDepth == 1) {
            tl_filePaths.clear();
        }
        ProcessPopupMenu(hMenu, hWnd, uFlags, kind, L"TrackPopupMenu");
    }
    
    BOOL bRes = TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    
    return bRes;
}

// Shared cap for all settings arrays (customItems, notepadExtensions, winrarExtensions).
constexpr int kMaxSettingArrayItems = 100;

// Logs a warning if a settings array has more entries configured than the
// cap, so an overflowed 101st+ entry doesn't just silently vanish.
void WarnIfSettingArrayOverflows(const wchar_t* keyFormat, const wchar_t* displayName) {
    auto overflowCheck = WindhawkUtils::StringSetting::make(keyFormat, kMaxSettingArrayItems);
    if (*overflowCheck) {
        Wh_Log(L"WARNING: More than %d entries are configured for '%s'; entries beyond the first %d are ignored",
               kMaxSettingArrayItems, displayName, kMaxSettingArrayItems);
    }
}

// Load settings
void LoadSettings() {
    // Guard against a hook thread reading g_settings/g_menuItems while this
    // rebuilds them (see g_settingsMutex comment).
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
    
    // Bloatware items
    g_settings.bloatwareItems.removeOneDrive = Wh_GetIntSetting(L"bloatwareItems.removeOneDrive");
    g_settings.bloatwareItems.removeCopilot = Wh_GetIntSetting(L"bloatwareItems.removeCopilot");
    g_settings.bloatwareItems.removeDefender = Wh_GetIntSetting(L"bloatwareItems.removeDefender");
    g_settings.bloatwareItems.removeDesigner = Wh_GetIntSetting(L"bloatwareItems.removeDesigner");
    g_settings.bloatwareItems.removeClipchamp = Wh_GetIntSetting(L"bloatwareItems.removeClipchamp");
    g_settings.bloatwareItems.removeMicrosoft365Copilot = Wh_GetIntSetting(L"bloatwareItems.removeMicrosoft365Copilot");
    
    // Basic items
    g_settings.basicItems.removeOpen = Wh_GetIntSetting(L"basicItems.removeOpen");
    g_settings.basicItems.removeOpenWith = Wh_GetIntSetting(L"basicItems.removeOpenWith");
    g_settings.basicItems.removeCut = Wh_GetIntSetting(L"basicItems.removeCut");
    g_settings.basicItems.removeCopy = Wh_GetIntSetting(L"basicItems.removeCopy");
    g_settings.basicItems.removeCreateShortcut = Wh_GetIntSetting(L"basicItems.removeCreateShortcut");
    g_settings.basicItems.removeDelete = Wh_GetIntSetting(L"basicItems.removeDelete");
    g_settings.basicItems.removeRename = Wh_GetIntSetting(L"basicItems.removeRename");
    g_settings.basicItems.removeSendTo = Wh_GetIntSetting(L"basicItems.removeSendTo");
    g_settings.basicItems.removeOpenInNewTab = Wh_GetIntSetting(L"basicItems.removeOpenInNewTab");
    g_settings.basicItems.removeOpenInNewWindow = Wh_GetIntSetting(L"basicItems.removeOpenInNewWindow");
    g_settings.basicItems.removeEdit = Wh_GetIntSetting(L"basicItems.removeEdit");
    g_settings.basicItems.removePlay = Wh_GetIntSetting(L"basicItems.removePlay");
    g_settings.basicItems.removePreview = Wh_GetIntSetting(L"basicItems.removePreview");
    g_settings.basicItems.removePrint = Wh_GetIntSetting(L"basicItems.removePrint");
    g_settings.basicItems.removeShare = Wh_GetIntSetting(L"basicItems.removeShare");
    g_settings.basicItems.removeRefresh = Wh_GetIntSetting(L"basicItems.removeRefresh");
    g_settings.basicItems.removeCopyAsPath = Wh_GetIntSetting(L"basicItems.removeCopyAsPath");
    g_settings.basicItems.removeCustomizeFolder = Wh_GetIntSetting(L"basicItems.removeCustomizeFolder");
    g_settings.basicItems.removeFavorites = Wh_GetIntSetting(L"basicItems.removeFavorites");
    g_settings.basicItems.removePinToQuickAccess = Wh_GetIntSetting(L"basicItems.removePinToQuickAccess");
    g_settings.basicItems.removePinToStart = Wh_GetIntSetting(L"basicItems.removePinToStart");
    g_settings.basicItems.removeCast = Wh_GetIntSetting(L"basicItems.removeCast");
    g_settings.basicItems.removeGiveAccess = Wh_GetIntSetting(L"basicItems.removeGiveAccess");
    g_settings.basicItems.removeRestoreVersions = Wh_GetIntSetting(L"basicItems.removeRestoreVersions");
    g_settings.basicItems.removeIncludeInLibrary = Wh_GetIntSetting(L"basicItems.removeIncludeInLibrary");
    g_settings.basicItems.removeRotate = Wh_GetIntSetting(L"basicItems.removeRotate");
    g_settings.basicItems.removeDisplaySettings = Wh_GetIntSetting(L"basicItems.removeDisplaySettings");
    g_settings.basicItems.removePersonalize = Wh_GetIntSetting(L"basicItems.removePersonalize");
    g_settings.basicItems.removeSetAsDesktopBackground = Wh_GetIntSetting(L"basicItems.removeSetAsDesktopBackground");
    g_settings.basicItems.removeView = Wh_GetIntSetting(L"basicItems.removeView");
    g_settings.basicItems.removeSortBy = Wh_GetIntSetting(L"basicItems.removeSortBy");
    g_settings.basicItems.removeGroupBy = Wh_GetIntSetting(L"basicItems.removeGroupBy");
    g_settings.basicItems.removeNew = Wh_GetIntSetting(L"basicItems.removeNew");
    g_settings.basicItems.removeProperties = Wh_GetIntSetting(L"basicItems.removeProperties");
    g_settings.basicItems.removePaste = Wh_GetIntSetting(L"basicItems.removePaste");
    g_settings.basicItems.removeExtractAll = Wh_GetIntSetting(L"basicItems.removeExtractAll");
    
    // App-specific items
    g_settings.appSpecificItems.removeSendWithQuickShare = Wh_GetIntSetting(L"appSpecificItems.removeSendWithQuickShare");
    g_settings.appSpecificItems.removeVLCPlaylist = Wh_GetIntSetting(L"appSpecificItems.removeVLCPlaylist");
    g_settings.appSpecificItems.removeVLCPlay = Wh_GetIntSetting(L"appSpecificItems.removeVLCPlay");
    g_settings.appSpecificItems.removeAddToMediaPlayerQueue = Wh_GetIntSetting(L"appSpecificItems.removeAddToMediaPlayerQueue");
    g_settings.appSpecificItems.removePlayWithMediaPlayer = Wh_GetIntSetting(L"appSpecificItems.removePlayWithMediaPlayer");
    g_settings.appSpecificItems.removeEditInNotepad = Wh_GetIntSetting(L"appSpecificItems.removeEditInNotepad");
    g_settings.appSpecificItems.removeEditInNotepadPlusPlus = Wh_GetIntSetting(L"appSpecificItems.removeEditInNotepadPlusPlus");
    g_settings.appSpecificItems.removeEditWithPhotos = Wh_GetIntSetting(L"appSpecificItems.removeEditWithPhotos");
    g_settings.appSpecificItems.removeEditWithPaint = Wh_GetIntSetting(L"appSpecificItems.removeEditWithPaint");
    g_settings.appSpecificItems.removeNvidiaControlPanel = Wh_GetIntSetting(L"appSpecificItems.removeNvidiaControlPanel");
    g_settings.appSpecificItems.removeOpenInTerminal = Wh_GetIntSetting(L"appSpecificItems.removeOpenInTerminal");
    g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice = Wh_GetIntSetting(L"appSpecificItems.removeAlwaysKeepOnThisDevice");
    g_settings.appSpecificItems.removeFreeUpSpace = Wh_GetIntSetting(L"appSpecificItems.removeFreeUpSpace");
    g_settings.appSpecificItems.removeWinRAR = Wh_GetIntSetting(L"appSpecificItems.removeWinRAR");
    
    // Extension filtering settings
    g_settings.extensionFiltering.enableExtensionFiltering = Wh_GetIntSetting(L"extensionFiltering.enableExtensionFiltering");
    
    // Load extension lists (lowercased once here instead of on every
    // IsExtensionInList() comparison)
    g_settings.extensionFiltering.notepadExtensions.clear();
    for (int i = 0; i < kMaxSettingArrayItems; i++) {
        auto ext = WindhawkUtils::StringSetting::make(L"extensionFiltering.notepadExtensions[%d]", i);
        if (!*ext) continue;
        std::wstring lowerExt = ToLower(ext.get());
        g_settings.extensionFiltering.notepadExtensions.push_back(lowerExt);
    }
    WarnIfSettingArrayOverflows(L"extensionFiltering.notepadExtensions[%d]", L"Notepad extensions");
    
    // Load custom items (normalized once here instead of on every
    // ShouldRemoveMenuItem() comparison)
    g_settings.customItems.clear();
    for (int i = 0; i < kMaxSettingArrayItems; i++) {
        auto customItem = WindhawkUtils::StringSetting::make(L"customItems[%d]", i);
        if (!*customItem) continue;
        
        // RemoveAmpersands is deliberately NOT applied here: it models
        // Windows' menu-text escaping, which applies to menu text, not to
        // what a user types (they type the visible text as-is).
        std::wstring cleanItem = NormalizeString(customItem.get());
        
        // A bare "*" matches every menu item and would empty the whole
        // context menu -- almost certainly a mistake, so refuse to load
        // it rather than let a stray keystroke leave no right-click menu.
        if (cleanItem == L"*") {
            Wh_Log(L"Ignoring custom item %d: a bare '*' would remove every menu item", i);
            continue;
        }
        
        // Only a trailing '*' is treated as a wildcard (see
        // MatchesCustomItem); a '*' anywhere else is matched literally
        // and will never match real menu text, so flag it for the user.
        size_t starPos = cleanItem.find(L'*');
        if (starPos != std::wstring::npos && starPos != cleanItem.length() - 1) {
            Wh_Log(L"WARNING: Custom item %d ('%s') has '*' before the end; only a trailing '*' is a wildcard, so this will be matched literally",
                   i, customItem.get());
        }
        
        g_settings.customItems.push_back(cleanItem);
        Wh_Log(L"Loaded custom item %d: %s", i, customItem.get());
    }
    WarnIfSettingArrayOverflows(L"customItems[%d]", L"Custom items");
    
    // WinRAR filtering settings (nested under extensionFiltering)
    g_settings.extensionFiltering.enableWinRARFiltering = Wh_GetIntSetting(L"extensionFiltering.enableWinRARFiltering");
    
    g_settings.extensionFiltering.winrarExtensions.clear();
    for (int i = 0; i < kMaxSettingArrayItems; i++) {
        auto ext = WindhawkUtils::StringSetting::make(L"extensionFiltering.winrarExtensions[%d]", i);
        if (!*ext) continue;
        std::wstring lowerExt = ToLower(ext.get());
        g_settings.extensionFiltering.winrarExtensions.push_back(lowerExt);
    }
    WarnIfSettingArrayOverflows(L"extensionFiltering.winrarExtensions[%d]", L"WinRAR extensions");
    
    // Modifier key override settings (Alt is the only bypass key -- see IsModifierKeyBypassActive)
    g_settings.modifierKeyOverride.enableModifierOverride = Wh_GetIntSetting(L"modifierKeyOverride.enableModifierOverride");
    
    Wh_Log(L"Total custom items loaded: %d", (int)g_settings.customItems.size());
    Wh_Log(L"Notepad extension filtering enabled: %d", g_settings.extensionFiltering.enableExtensionFiltering);
    Wh_Log(L"Notepad extensions count: %d", (int)g_settings.extensionFiltering.notepadExtensions.size());
    Wh_Log(L"WinRAR extension filtering enabled: %d", g_settings.extensionFiltering.enableWinRARFiltering);
    Wh_Log(L"WinRAR extensions count: %d", (int)g_settings.extensionFiltering.winrarExtensions.size());
    Wh_Log(L"Modifier key override enabled: %d (Alt)", g_settings.modifierKeyOverride.enableModifierOverride);
    
    InitializeMenuItems();
}

// True if the mod's current settings would actually remove anything. If
// nothing is toggled and there are no custom items, every popup gets
// hooked and walked for zero effect. Must be called after LoadSettings().
bool AnyRemovalConfigured() {
    for (const auto& item : g_menuItems) {
        if (*item.enabled) {
            return true;
        }
    }
    return !g_settings.customItems.empty();
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing context menu cleaner mod");
    
    LoadSettings();
    
    if (!AnyRemovalConfigured()) {
        // Nothing configured to remove -- unload rather than hook
        // TrackPopupMenu(Ex) process-wide for no effect. Windhawk reloads
        // an unloaded mod automatically once settings change again.
        Wh_Log(L"No items are configured to remove (all toggles off, no custom "
               L"items) -- unloading until settings change");
        return FALSE;
    }
    
    if (!WindhawkUtils::SetFunctionHook(
        TrackPopupMenuEx,
        TrackPopupMenuEx_Hook,
        &TrackPopupMenuEx_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
        return FALSE;
    }
    
    if (!WindhawkUtils::SetFunctionHook(
        TrackPopupMenu,
        TrackPopupMenu_Hook,
        &TrackPopupMenu_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenu");
        return FALSE;
    }
    
    Wh_Log(L"Context menu cleaner mod initialized successfully");
    return TRUE;
}

// Settings changed callback
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
    *bReload = FALSE;
    // If nothing is left to remove, unload until settings change again
    // instead of an already-loaded mod keeping both hooks forever.
    return AnyRemovalConfigured();
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"Uninitializing context menu cleaner mod");
    
    // Force-unhook any thread-local menu hooks still installed -- a HHOOK
    // callback pointing into an unloaded mod image is a crash.
    // g_uninitInProgress ensures no new hook can slip past this sweep.
    std::lock_guard<std::mutex> lock(g_activeMenuHooksMutex);
    g_uninitInProgress = true;
    for (HHOOK hook : g_activeMenuHooks) {
        UnhookWindowsHookEx(hook);
    }
    g_activeMenuHooks.clear();
}

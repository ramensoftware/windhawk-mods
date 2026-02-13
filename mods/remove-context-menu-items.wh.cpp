// ==WindhawkMod==
// @id              remove-context-menu-items
// @name            Remove Unwanted Context Menu Items (Classic Menu Only)
// @description     Removes unwanted items from file context menus with configurable options and context-aware filtering
// @version         1.6.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lshlwapi -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Unwanted Context Menu Items (Classic Menu Only)

⚠️ **Windows 11 Users:** This mod currently works only with the classic context menu. Please install the [Classic context menu on Windows 11](https://windhawk.net/mods/explorer-context-menu-classic) mod to use this mod.

---

This mod removes unwanted items from file context menus with configurable options. It hooks into the context menu creation process and removes unwanted menu items by checking their text labels before they are displayed. It also automatically cleans up duplicate separator lines. Custom items use exact matching by default for precision, with optional wildcard support for flexibility.

## Screenshots

**Before and after right-clicking on an empty space:**

![Before and after on empty space](https://i.imgur.com/xEyRTk1.png)

**Before and after right-clicking a video file:**

![Before and after on video file](https://i.imgur.com/CPNiFtQ.png)

## Features

Clean up your Windows context menus by removing bloatware and unwanted items:

### Bloatware Items
- `Move to OneDrive`
- `Ask Copilot`
- `Scan with Microsoft Defender`
- `Create with Designer`
- `Edit with Clipchamp`

### Basic Items
- `Open`
- `Cast to Device`
- `Include in library`
- `Restore previous versions`
- ...and many more!

### App-specific Items
- `Add to VLC media player's Playlist`
- `Edit in Notepad`
- `Edit with Paint`
- `NVIDIA Control Panel`
- ...and many more!

### Context-Aware Filtering
Some items like `Edit in Notepad` can be filtered based on file extensions. For example, you can configure the mod to only show `Edit in Notepad` for text files (.txt, .log, .json) but hide it for images and other files.

### Custom Items
You can also add your own custom menu items to remove by entering their text in the settings.

**Basic Usage (Exact Match):**
- "Copy" - Removes the `Copy` option (exact match)
- "Pin to Quick access" - Removes the `Pin to Quick access` option (exact match)
- "Open" - Removes only `Open` (exact match, won't remove `Open in new tab`)

**Wildcard Usage (Prefix Match):**
Add an asterisk (*) at the end to match items that start with the given text:
- "Open*" - Removes `Open`, `Open with...`, `Open in Terminal`, `Open in new tab`, etc.
- "Pin to*" - Removes `Pin to Quick access`, `Pin to Start`, etc.
- "C*" - Removes all menu items that start with C (`Cut`, `Copy`, `Create shortcut`). Be careful!

**Tip:** Right-click a file/folder, note the exact text of the menu item you want to remove, then add it to Custom Items. Use the asterisk (*) only if you want to remove multiple items with the same prefix.

## Supported Languages

- `cs-CZ`
- `de-DE` (added by [Schleifenkratzer](https://github.com/Schleifenkratzer))
- `en-AU`, `en-GB`
- `pt-BR`, `pt-PT`

If you find a mistake and for additional details, please click [here](https://github.com/armaninyow/Remove-Unwanted-Context-Menu-Items).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# NOTE: Predefined options below are in English only. For other languages, use Custom Items at the bottom.

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
    $description: Removes the "Rotate right" and "Rotate left" items
  - removeDisplaySettings: false
    $name: Display settings
  - removePersonalize: false
    $name: Personalize
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
    $description: See the notice below for more information
  - removeEditInNotepadPlusPlus: false
    $name: Edit in Notepad++
    $description: See the notice below for more information
  - removeEditWithPhotos: false
    $name: Edit with Photos
  - removeEditWithPaint: false
    $name: Edit with Paint
  - removeNvidiaControlPanel: false
    $name: NVIDIA Control Panel
  - removeOpenInTerminal: false
    $name: Open in Terminal
  - removeAlwaysKeepOnThisDevice: false
    $name: Always keep on this device
    $description: OneDrive submenu item
  - removeFreeUpSpace: false
    $name: Free up space
    $description: OneDrive submenu item
  - removeWinRAR: false
    $name: WinRAR
  $name: App-specific Items
  $description: Disabled by default

- customItems:
  - ""
  $name: Custom items to remove
  $description: >-
    Basic Usage (Exact Match): Enter exact menu item names.

    Wildcard Usage (Prefix Match): Add * at the end for prefix matching (e.g., "Open*" removes all items starting with "Open")

- extensionFiltering:
  - enableExtensionFiltering: false
    $name: Enable extension-based filtering
    $description: >-
      When enabled, the Notepad menu items will ONLY appear for files with extensions in the whitelist.

      Note: Requires the "Edit in Notepad" or "Edit in Notepad++" items to be enabled


      Known Limitation: In Explorer window with multiple tabs, the mod currently retrieves file context from the first tab (active primary tab) rather than the other tabs currently being viewed or clicked.
  - notepadExtensions:
    - ".txt"
    - ".log"
    - ".ini"
    - ".cfg"
    - ".conf"
    - ".inf"
    - ".bat"
    - ".cmd"
    - ".reg"
    - ".json"
    - ".xml"
    - ".html"
    - ".htm"
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
    - ".h"
    - ".cs"
    - ".php"
    - ".rb"
    - ".sh"
    - ".ps1"
    - ".vbs"
    - ".lua"
    - ".asm"
    - ".makefile"
    - ".htaccess"
    - ".gitconfig"
    - ".env"
    - ".dockerfile"
    $name: Extensions for Notepad/Notepad++
    $description: Notepad menu items will ONLY appear for these file extensions. Use lowercase with dot (e.g., .txt)
  $name: Note for Notepad menu items
  $description: In Windows 11, the "Edit in Notepad" and "Edit in Notepad++" items appear even when right-clicking files that are not text or code. The toggles above remove these items globally. To display them only for relevant text/code files, enable this filtering feature.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <exdisp.h>
#include <shlguid.h>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>

#pragma comment(lib, "shlwapi.lib")

// Thread-local storage for file paths
thread_local std::vector<std::wstring> g_threadFilePaths;

// Structure for EnumWindows callback
struct FindExplorerWindowData {
    DWORD processId;
    HWND explorerWindow;
};

// Callback function to find Explorer windows
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    FindExplorerWindowData* data = (FindExplorerWindowData*)lParam;
    
    DWORD windowProcessId = 0;
    GetWindowThreadProcessId(hwnd, &windowProcessId);
    
    if (windowProcessId == data->processId) {
        wchar_t className[256] = {0};
        GetClassNameW(hwnd, className, 256);
        
        // Look for Explorer windows (CabinetWClass for folder windows, Progman for desktop)
        if (wcscmp(className, L"CabinetWClass") == 0 || 
            wcscmp(className, L"ExploreWClass") == 0) {
            data->explorerWindow = hwnd;
            return FALSE; // Stop enumeration
        }
    }
    
    return TRUE; // Continue enumeration
}

// Function to get IShellBrowser from Explorer window
IShellBrowser* GetShellBrowser(HWND hwnd) {
    IShellBrowser* pShellBrowser = nullptr;
    
    Wh_Log(L"GetShellBrowser: Trying to get IShellBrowser from window %p", hwnd);
    
    // Method 1: Direct message to main window
    LRESULT result = SendMessageTimeoutW(hwnd, WM_USER + 7, 0, 0, 
                                          SMTO_ABORTIFHUNG, 1000, (PDWORD_PTR)&pShellBrowser);
    if (pShellBrowser) {
        Wh_Log(L"GetShellBrowser: Got IShellBrowser via WM_USER+7 on main window");
        return pShellBrowser;
    }
    
    // Method 2: Find child windows and try there
    HWND hwndChild = NULL;
    const wchar_t* childClasses[] = {
        L"ShellTabWindowClass",
        L"DUIViewWndClassName", 
        L"DirectUIHWND",
        L"SHELLDLL_DefView",
        NULL
    };
    
    hwndChild = hwnd;
    for (int i = 0; childClasses[i] != NULL; i++) {
        HWND hwndNext = FindWindowExW(hwndChild, NULL, childClasses[i], NULL);
        if (hwndNext) {
            Wh_Log(L"GetShellBrowser: Found child window class: %s", childClasses[i]);
            hwndChild = hwndNext;
        } else {
            Wh_Log(L"GetShellBrowser: Could not find child window class: %s", childClasses[i]);
            break;
        }
    }
    
    if (hwndChild != hwnd) {
        result = SendMessageTimeoutW(hwndChild, WM_USER + 7, 0, 0,
                                     SMTO_ABORTIFHUNG, 1000, (PDWORD_PTR)&pShellBrowser);
        if (pShellBrowser) {
            Wh_Log(L"GetShellBrowser: Got IShellBrowser via child window");
            return pShellBrowser;
        }
    }
    
    Wh_Log(L"GetShellBrowser: Failed to get IShellBrowser");
    return nullptr;
}

// Alternative method: Get files using IShellWindows automation
std::vector<std::wstring> GetSelectedFilesViaAutomation() {
    std::vector<std::wstring> files;
    
    IShellWindows* pShellWindows = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, NULL, CLSCTX_ALL,
                                   IID_IShellWindows, (void**)&pShellWindows);
    
    if (SUCCEEDED(hr) && pShellWindows) {
        long count = 0;
        pShellWindows->get_Count(&count);
        
        Wh_Log(L"GetSelectedFilesViaAutomation: Found %d shell windows", count);
        
        // Get cursor position to find the window we're actually clicking in
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        HWND hwndAtCursor = WindowFromPoint(cursorPos);
        
        // Find the top-level window containing this point
        HWND hwndTopLevel = hwndAtCursor;
        while (GetParent(hwndTopLevel)) {
            hwndTopLevel = GetParent(hwndTopLevel);
        }
        
        Wh_Log(L"GetSelectedFilesViaAutomation: Window at cursor: %p, Top-level: %p", hwndAtCursor, hwndTopLevel);
        
        for (long i = 0; i < count; i++) {
            VARIANT v;
            VariantInit(&v);
            v.vt = VT_I4;
            v.lVal = i;
            
            IDispatch* pDisp = nullptr;
            if (SUCCEEDED(pShellWindows->Item(v, &pDisp)) && pDisp) {
                IWebBrowserApp* pWebBrowser = nullptr;
                if (SUCCEEDED(pDisp->QueryInterface(IID_IWebBrowserApp, (void**)&pWebBrowser)) && pWebBrowser) {
                    // Check if this is the window under the cursor
                    HWND hwnd = NULL;
                    pWebBrowser->get_HWND((SHANDLE_PTR*)&hwnd);
                    
                    // Match against the top-level window at cursor position
                    if (hwnd == hwndTopLevel) {
                        Wh_Log(L"GetSelectedFilesViaAutomation: Found Explorer window at cursor position");
                        
                        // Get the document (IShellFolderViewDual)
                        IDispatch* pDocDisp = nullptr;
                        if (SUCCEEDED(pWebBrowser->get_Document(&pDocDisp)) && pDocDisp) {
                            IShellFolderViewDual* pFolderView = nullptr;
                            if (SUCCEEDED(pDocDisp->QueryInterface(IID_IShellFolderViewDual, (void**)&pFolderView)) && pFolderView) {
                                // Get selected items
                                FolderItems* pItems = nullptr;
                                if (SUCCEEDED(pFolderView->SelectedItems(&pItems)) && pItems) {
                                    long itemCount = 0;
                                    pItems->get_Count(&itemCount);
                                    
                                    Wh_Log(L"GetSelectedFilesViaAutomation: Found %d selected items", itemCount);
                                    
                                    for (long j = 0; j < itemCount; j++) {
                                        VARIANT vIndex;
                                        VariantInit(&vIndex);
                                        vIndex.vt = VT_I4;
                                        vIndex.lVal = j;
                                        
                                        FolderItem* pItem = nullptr;
                                        if (SUCCEEDED(pItems->Item(vIndex, &pItem)) && pItem) {
                                            BSTR path = nullptr;
                                            if (SUCCEEDED(pItem->get_Path(&path)) && path) {
                                                files.push_back(std::wstring(path));
                                                Wh_Log(L"GetSelectedFilesViaAutomation: File: %s", path);
                                                SysFreeString(path);
                                            }
                                            pItem->Release();
                                        }
                                    }
                                    pItems->Release();
                                }
                                pFolderView->Release();
                            }
                            pDocDisp->Release();
                        }
                    }
                    pWebBrowser->Release();
                }
                pDisp->Release();
            }
            
            if (!files.empty()) {
                break; // Found files, no need to continue
            }
        }
        
        pShellWindows->Release();
    }
    
    return files;
}

// Function to get selected files from Explorer window
std::vector<std::wstring> GetSelectedFilesFromExplorer(HWND hwnd) {
    std::vector<std::wstring> files;
    
    // Try automation method first (most reliable)
    files = GetSelectedFilesViaAutomation();
    if (!files.empty()) {
        return files;
    }
    
    // Fallback to IShellBrowser method
    // Find the foreground Explorer window
    if (!hwnd) {
        hwnd = GetForegroundWindow();
    }
    
    // Check if it's an Explorer window
    wchar_t className[256] = {0};
    GetClassNameW(hwnd, className, 256);
    
    Wh_Log(L"GetSelectedFilesFromExplorer: Window class: %s", className);
    
    // If not an Explorer window, try to find one in the current process
    if (wcscmp(className, L"CabinetWClass") != 0 && 
        wcscmp(className, L"ExploreWClass") != 0 &&
        wcscmp(className, L"Progman") != 0) {
        
        FindExplorerWindowData data = {0};
        data.processId = GetCurrentProcessId();
        data.explorerWindow = NULL;
        
        EnumWindows(EnumWindowsProc, (LPARAM)&data);
        
        if (data.explorerWindow) {
            hwnd = data.explorerWindow;
            Wh_Log(L"GetSelectedFilesFromExplorer: Found Explorer window via enumeration");
        } else {
            Wh_Log(L"Could not find Explorer window");
            return files;
        }
    }
    
    IShellBrowser* pShellBrowser = GetShellBrowser(hwnd);
    if (!pShellBrowser) {
        Wh_Log(L"Could not get IShellBrowser from window");
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
                        // Get the full path
                        STRRET strret;
                        if (SUCCEEDED(pFolder->GetDisplayNameOf(pidl, SHGDN_FORPARSING, &strret))) {
                            wchar_t szPath[MAX_PATH] = {0};
                            StrRetToBufW(&strret, pidl, szPath, MAX_PATH);
                            if (szPath[0]) {
                                files.push_back(std::wstring(szPath));
                                Wh_Log(L"Found selected file: %s", szPath);
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

// Settings structures
struct BloatwareSettings {
    bool removeOneDrive;
    bool removeCopilot;
    bool removeDefender;
    bool removeDesigner;
    bool removeClipchamp;
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
};

struct {
    BloatwareSettings bloatwareItems;
    BasicSettings basicItems;
    AppSpecificSettings appSpecificItems;
    ExtensionFilteringSettings extensionFiltering;
    std::vector<std::wstring> customItems;
} g_settings;

// Structure to hold menu item info
struct MenuItem {
    std::wstring text;
    bool* enabled;
    bool requiresExtensionCheck;
    std::vector<std::wstring>* allowedExtensions;
};

// List of predefined menu items to check
std::vector<MenuItem> g_menuItems;

// Function to get file extension from path
std::wstring GetFileExtension(const std::wstring& path) {
    size_t dotPos = path.find_last_of(L'.');
    if (dotPos == std::wstring::npos || dotPos == path.length() - 1) {
        return L"";
    }
    std::wstring ext = path.substr(dotPos);
    // Convert to lowercase for comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    return ext;
}

// Function to check if extension is in list
bool IsExtensionInList(const std::wstring& ext, const std::vector<std::wstring>& extList) {
    if (ext.empty() || extList.empty()) {
        return false;
    }
    
    for (const auto& allowedExt : extList) {
        std::wstring lowerAllowed = allowedExt;
        std::transform(lowerAllowed.begin(), lowerAllowed.end(), lowerAllowed.begin(), ::towlower);
        if (ext == lowerAllowed) {
            return true;
        }
    }
    return false;
}

// Check if any of the current files has an extension in the list
bool AnyFileHasExtensionInList(const std::vector<std::wstring>& extList) {
    Wh_Log(L"AnyFileHasExtensionInList: Checking %d files against %d extensions", 
           g_threadFilePaths.size(), extList.size());
    
    for (const auto& filePath : g_threadFilePaths) {
        std::wstring ext = GetFileExtension(filePath);
        Wh_Log(L"  File: %s, Extension: %s", filePath.c_str(), ext.c_str());
        
        if (IsExtensionInList(ext, extList)) {
            Wh_Log(L"  -> Extension MATCHED in whitelist!");
            return true;
        }
    }
    
    Wh_Log(L"  -> No extensions matched in whitelist");
    return false;
}

// Function to initialize menu items based on settings
void InitializeMenuItems() {
    g_menuItems.clear();
    g_menuItems = {
        // Bloatware Items - Move to OneDrive (en-US, en-GB, en-AU same)
        {L"Move to OneDrive", &g_settings.bloatwareItems.removeOneDrive, false, nullptr},
        {L"Mover para o OneDrive", &g_settings.bloatwareItems.removeOneDrive, false, nullptr}, // pt-BR, pt-PT
        {L"Přesunout na OneDrive", &g_settings.bloatwareItems.removeOneDrive, false, nullptr}, // cs-CZ
        {L"Auf OneDrive verschieben", &g_settings.bloatwareItems.removeOneDrive, false, nullptr}, // de-DE
        
        // Always keep on this device (en-US, en-GB, en-AU same)
        {L"Always keep on this device", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, false, nullptr},
        {L"Sempre manter neste dispositivo", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, false, nullptr}, // pt-BR
        {L"Manter sempre neste dispositivo", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, false, nullptr}, // pt-PT
        {L"Vždy ponechat na tomto zařízení", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, false, nullptr}, // cs-CZ
        {L"Immer auf diesem Gerät behalten", &g_settings.appSpecificItems.removeAlwaysKeepOnThisDevice, false, nullptr}, // de-DE
        
        // Free up space (en-US, en-GB, en-AU same)
        {L"Free up space", &g_settings.appSpecificItems.removeFreeUpSpace, false, nullptr},
        {L"Liberar espaço", &g_settings.appSpecificItems.removeFreeUpSpace, false, nullptr}, // pt-BR
        {L"Libertar espaço", &g_settings.appSpecificItems.removeFreeUpSpace, false, nullptr}, // pt-PT
        {L"Uvolnit místo", &g_settings.appSpecificItems.removeFreeUpSpace, false, nullptr}, // cs-CZ
        {L"Bereinigen", &g_settings.appSpecificItems.removeFreeUpSpace, false, nullptr}, // de-DE
        
        // Ask Copilot (en-US, en-GB, en-AU same)
        {L"Ask Copilot", &g_settings.bloatwareItems.removeCopilot, false, nullptr},
        {L"Perguntar ao Copilot", &g_settings.bloatwareItems.removeCopilot, false, nullptr}, // pt-BR, pt-PT
        {L"Zeptat se Copilota", &g_settings.bloatwareItems.removeCopilot, false, nullptr}, // cs-CZ
        {L"Copilot fragen", &g_settings.bloatwareItems.removeCopilot, false, nullptr}, // de-DE
        
        // Scan with Microsoft Defender (en-US, en-GB, en-AU same)
        {L"Scan with Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, false, nullptr},
        {L"Verificar com o Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, false, nullptr}, // pt-BR
        {L"Analisar com o Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, false, nullptr}, // pt-PT
        {L"Prohledat pomocí programu Microsoft Defender...", &g_settings.bloatwareItems.removeDefender, false, nullptr}, // cs-CZ
        {L"Mit Microsoft Defender überprüfen...", &g_settings.bloatwareItems.removeDefender, false, nullptr}, // de-DE
        
        // Create with Designer (en-US, en-GB, en-AU same)
        {L"Create with Designer", &g_settings.bloatwareItems.removeDesigner, false, nullptr},
        {L"Criar com o Designer", &g_settings.bloatwareItems.removeDesigner, false, nullptr}, // pt-BR, pt-PT
        {L"Vytvořit pomocí Designeru", &g_settings.bloatwareItems.removeDesigner, false, nullptr}, // cs-CZ
        {L"Mit Designer erstellen", &g_settings.bloatwareItems.removeDesigner, false, nullptr}, // de-DE
        
        // Edit with Clipchamp (en-US, en-GB, en-AU same)
        {L"Edit with Clipchamp", &g_settings.bloatwareItems.removeClipchamp, false, nullptr},
        {L"Editar com o Clipchamp", &g_settings.bloatwareItems.removeClipchamp, false, nullptr}, // pt-BR, pt-PT
        {L"Upravit pomocí Clipchampu", &g_settings.bloatwareItems.removeClipchamp, false, nullptr}, // cs-CZ
        {L"Mit Clipchamp bearbeiten", &g_settings.bloatwareItems.removeClipchamp, false, nullptr}, // de-DE
        
        // Basic Items - Open (en-US, en-GB, en-AU same)
        {L"Open", &g_settings.basicItems.removeOpen, false, nullptr},
        {L"Abrir", &g_settings.basicItems.removeOpen, false, nullptr}, // pt-BR, pt-PT
        {L"Otevřít", &g_settings.basicItems.removeOpen, false, nullptr}, // cs-CZ
        {L"Öffnen", &g_settings.basicItems.removeOpen, false, nullptr}, // de-DE
        
        // Open with (en-US, en-GB, en-AU same)
        {L"Open with", &g_settings.basicItems.removeOpenWith, false, nullptr},
        {L"Abrir com", &g_settings.basicItems.removeOpenWith, false, nullptr}, // pt-BR, pt-PT
        {L"Otevřít v aplikaci", &g_settings.basicItems.removeOpenWith, false, nullptr}, // cs-CZ
        {L"Öffnen mit", &g_settings.basicItems.removeOpenWith, false, nullptr}, // de-DE
        
        // Cut (en-US, en-GB, en-AU same)
        {L"Cut", &g_settings.basicItems.removeCut, false, nullptr},
        {L"Recortar", &g_settings.basicItems.removeCut, false, nullptr}, // pt-BR
        {L"Cortar", &g_settings.basicItems.removeCut, false, nullptr}, // pt-PT
        {L"Vyjmout", &g_settings.basicItems.removeCut, false, nullptr}, // cs-CZ
        {L"Ausschneiden", &g_settings.basicItems.removeCut, false, nullptr}, // de-DE
        
        // Copy (en-US, en-GB, en-AU same)
        {L"Copy", &g_settings.basicItems.removeCopy, false, nullptr},
        {L"Copiar", &g_settings.basicItems.removeCopy, false, nullptr}, // pt-BR, pt-PT
        {L"Kopírovat", &g_settings.basicItems.removeCopy, false, nullptr}, // cs-CZ
        {L"Kopieren", &g_settings.basicItems.removeCopy, false, nullptr}, // de-DE
        
        // Create shortcut (en-US, en-GB, en-AU same)
        {L"Create shortcut", &g_settings.basicItems.removeCreateShortcut, false, nullptr},
        {L"Criar atalho", &g_settings.basicItems.removeCreateShortcut, false, nullptr}, // pt-BR, pt-PT
        {L"Vytvořit zástupce", &g_settings.basicItems.removeCreateShortcut, false, nullptr}, // cs-CZ
        {L"Verknüpfung erstellen", &g_settings.basicItems.removeCreateShortcut, false, nullptr}, // de-DE
        
        // Delete (en-US, en-GB, en-AU same)
        {L"Delete", &g_settings.basicItems.removeDelete, false, nullptr},
        {L"Excluir", &g_settings.basicItems.removeDelete, false, nullptr}, // pt-BR
        {L"Eliminar", &g_settings.basicItems.removeDelete, false, nullptr}, // pt-PT
        {L"Odstranit", &g_settings.basicItems.removeDelete, false, nullptr}, // cs-CZ
        {L"Löschen", &g_settings.basicItems.removeDelete, false, nullptr}, // de-DE
        
        // Rename (en-US, en-GB, en-AU same)
        {L"Rename", &g_settings.basicItems.removeRename, false, nullptr},
        {L"Renomear", &g_settings.basicItems.removeRename, false, nullptr}, // pt-BR
        {L"Mudar o nome", &g_settings.basicItems.removeRename, false, nullptr}, // pt-PT
        {L"Přejmenovat", &g_settings.basicItems.removeRename, false, nullptr}, // cs-CZ
        {L"Umbenennen", &g_settings.basicItems.removeRename, false, nullptr}, // de-DE
        
        // Send to (en-US, en-GB, en-AU same)
        {L"Send to", &g_settings.basicItems.removeSendTo, false, nullptr},
        {L"Enviar para", &g_settings.basicItems.removeSendTo, false, nullptr}, // pt-BR, pt-PT
        {L"Odeslat do", &g_settings.basicItems.removeSendTo, false, nullptr}, // cs-CZ
        {L"Senden an", &g_settings.basicItems.removeSendTo, false, nullptr}, // de-DE
        
        // Open in new tab (en-US, en-GB, en-AU same)
        {L"Open in new tab", &g_settings.basicItems.removeOpenInNewTab, false, nullptr},
        {L"Abrir em uma nova guia", &g_settings.basicItems.removeOpenInNewTab, false, nullptr}, // pt-BR
        {L"Abrir num novo separador", &g_settings.basicItems.removeOpenInNewTab, false, nullptr}, // pt-PT
        {L"Otevřít na nové kartě", &g_settings.basicItems.removeOpenInNewTab, false, nullptr}, // cs-CZ
        {L"In neuer Registerkarte öffnen", &g_settings.basicItems.removeOpenInNewTab, false, nullptr}, // de-DE
        
        // Open in new window (en-US, en-GB, en-AU same)
        {L"Open in new window", &g_settings.basicItems.removeOpenInNewWindow, false, nullptr},
        {L"Abrir em uma nova janela", &g_settings.basicItems.removeOpenInNewWindow, false, nullptr}, // pt-BR
        {L"Abrir numa nova janela", &g_settings.basicItems.removeOpenInNewWindow, false, nullptr}, // pt-PT
        {L"Otevřít v novém okně", &g_settings.basicItems.removeOpenInNewWindow, false, nullptr}, // cs-CZ
        {L"In neuem Fenster öffnen", &g_settings.basicItems.removeOpenInNewWindow, false, nullptr}, // de-DE
        
        // Edit (en-US, en-GB, en-AU same)
        {L"Edit", &g_settings.basicItems.removeEdit, false, nullptr},
        {L"Editar", &g_settings.basicItems.removeEdit, false, nullptr}, // pt-BR, pt-PT
        {L"Upravit", &g_settings.basicItems.removeEdit, false, nullptr}, // cs-CZ
        {L"Bearbeiten", &g_settings.basicItems.removeEdit, false, nullptr}, // de-DE
        
        // Play (en-US, en-GB, en-AU same)
        {L"Play", &g_settings.basicItems.removePlay, false, nullptr},
        {L"Reproduzir", &g_settings.basicItems.removePlay, false, nullptr}, // pt-BR, pt-PT
        {L"Přehrát", &g_settings.basicItems.removePlay, false, nullptr}, // cs-CZ
        {L"Wiedergabe", &g_settings.basicItems.removePlay, false, nullptr}, // de-DE
        
        // Preview (en-US, en-GB, en-AU same)
        {L"Preview", &g_settings.basicItems.removePreview, false, nullptr},
        {L"Visualizar", &g_settings.basicItems.removePreview, false, nullptr}, // pt-BR
        {L"Pré-visualizar", &g_settings.basicItems.removePreview, false, nullptr}, // pt-PT
        {L"Náhled", &g_settings.basicItems.removePreview, false, nullptr}, // cs-CZ
        {L"Vorschau", &g_settings.basicItems.removePreview, false, nullptr}, // de-DE
        
        // Print (en-US, en-GB, en-AU same)
        {L"Print", &g_settings.basicItems.removePrint, false, nullptr},
        {L"Imprimir", &g_settings.basicItems.removePrint, false, nullptr}, // pt-BR, pt-PT
        {L"Tisk", &g_settings.basicItems.removePrint, false, nullptr}, // cs-CZ
        {L"Drucken", &g_settings.basicItems.removePrint, false, nullptr}, // de-DE
        
        // Share (en-US, en-GB, en-AU same)
        {L"Share", &g_settings.basicItems.removeShare, false, nullptr},
        {L"Compartilhar", &g_settings.basicItems.removeShare, false, nullptr}, // pt-BR
        {L"Partilhar", &g_settings.basicItems.removeShare, false, nullptr}, // pt-PT
        {L"Sdílet", &g_settings.basicItems.removeShare, false, nullptr}, // cs-CZ
        {L"Freigabe", &g_settings.basicItems.removeShare, false, nullptr}, // de-DE
        
        // Send with Quick Share (en-US, en-GB, en-AU same)
        {L"Send with Quick Share", &g_settings.appSpecificItems.removeSendWithQuickShare, false, nullptr},
        {L"Enviar com o Compartilhamento Rápido", &g_settings.appSpecificItems.removeSendWithQuickShare, false, nullptr}, // pt-BR
        {L"Enviar com Partilha Rápida", &g_settings.appSpecificItems.removeSendWithQuickShare, false, nullptr}, // pt-PT
        {L"Odeslat pomocí Rychlého sdílení", &g_settings.appSpecificItems.removeSendWithQuickShare, false, nullptr}, // cs-CZ
        {L"Mit Quick Share senden", &g_settings.appSpecificItems.removeSendWithQuickShare, false, nullptr}, // de-DE
        
        // Refresh (en-US, en-GB, en-AU same)
        {L"Refresh", &g_settings.basicItems.removeRefresh, false, nullptr},
        {L"Atualizar", &g_settings.basicItems.removeRefresh, false, nullptr}, // pt-BR, pt-PT
        {L"Aktualizovat", &g_settings.basicItems.removeRefresh, false, nullptr}, // cs-CZ
        {L"Aktualisieren", &g_settings.basicItems.removeRefresh, false, nullptr}, // de-DE
        
        // Copy as path (en-US, en-GB, en-AU same)
        {L"Copy as path", &g_settings.basicItems.removeCopyAsPath, false, nullptr},
        {L"Copiar como caminho", &g_settings.basicItems.removeCopyAsPath, false, nullptr}, // pt-BR, pt-PT
        {L"Kopírovat jako cestu", &g_settings.basicItems.removeCopyAsPath, false, nullptr}, // cs-CZ
        {L"Als Pfad kopieren", &g_settings.basicItems.removeCopyAsPath, false, nullptr}, // de-DE
        
        // Customize this folder
        {L"Customize this folder...", &g_settings.basicItems.removeCustomizeFolder, false, nullptr}, // en-US
        {L"Customise this folder...", &g_settings.basicItems.removeCustomizeFolder, false, nullptr}, // en-GB, en-AU
        {L"Personalizar esta pasta...", &g_settings.basicItems.removeCustomizeFolder, false, nullptr}, // pt-BR, pt-PT
        {L"Přizpůsobit tuto složku...", &g_settings.basicItems.removeCustomizeFolder, false, nullptr}, // cs-CZ
        {L"Ordner anpassen...", &g_settings.basicItems.removeCustomizeFolder, false, nullptr}, // de-DE
        
        // Add to Favorites
        {L"Add to Favorites", &g_settings.basicItems.removeFavorites, false, nullptr}, // en-US
        {L"Add to Favourites", &g_settings.basicItems.removeFavorites, false, nullptr}, // en-GB, en-AU
        {L"Adicionar aos Favoritos", &g_settings.basicItems.removeFavorites, false, nullptr}, // pt-BR, pt-PT
        {L"Přidat k oblíbeným položkám", &g_settings.basicItems.removeFavorites, false, nullptr}, // cs-CZ
        {L"Zu Favoriten hinzufügen", &g_settings.basicItems.removeFavorites, false, nullptr}, // de-DE
        
        // Pin to Quick access (en-US, en-GB, en-AU same)
        {L"Pin to Quick access", &g_settings.basicItems.removePinToQuickAccess, false, nullptr},
        {L"Fixar no Acesso Rápido", &g_settings.basicItems.removePinToQuickAccess, false, nullptr}, // pt-BR
        {L"Afixar no Acesso rápido", &g_settings.basicItems.removePinToQuickAccess, false, nullptr}, // pt-PT
        {L"Připnout na Rychlý přístup", &g_settings.basicItems.removePinToQuickAccess, false, nullptr}, // cs-CZ
        {L"An Schnellzugriff anheften", &g_settings.basicItems.removePinToQuickAccess, false, nullptr}, // de-DE
        
        // Pin to Start (en-US, en-GB, en-AU same)
        {L"Pin to Start", &g_settings.basicItems.removePinToStart, false, nullptr},
        {L"Fixar em Iniciar", &g_settings.basicItems.removePinToStart, false, nullptr}, // pt-BR
        {L"Afixar no Iniciar", &g_settings.basicItems.removePinToStart, false, nullptr}, // pt-PT
        {L"Připnout na Start", &g_settings.basicItems.removePinToStart, false, nullptr}, // cs-CZ
        {L"An \"Start\" anheften", &g_settings.basicItems.removePinToStart, false, nullptr}, // de-DE
        
        // Cast to Device (en-US, en-GB, en-AU same)
        {L"Cast to Device", &g_settings.basicItems.removeCast, false, nullptr},
        {L"Transmitir para Dispositivo", &g_settings.basicItems.removeCast, false, nullptr}, // pt-BR
        {L"Transmitir para o Dispositivo", &g_settings.basicItems.removeCast, false, nullptr}, // pt-PT
        {L"Přetypovat do zařízení", &g_settings.basicItems.removeCast, false, nullptr}, // cs-CZ
        {L"Wiedergabe auf Gerät", &g_settings.basicItems.removeCast, false, nullptr}, // de-DE
        
        // Give access to (en-US, en-GB, en-AU same)
        {L"Give access to", &g_settings.basicItems.removeGiveAccess, false, nullptr},
        {L"Conceder acesso a", &g_settings.basicItems.removeGiveAccess, false, nullptr}, // pt-BR, pt-PT
        {L"Poskytnout přístup k", &g_settings.basicItems.removeGiveAccess, false, nullptr}, // cs-CZ
        {L"Freigeben für", &g_settings.basicItems.removeGiveAccess, false, nullptr}, // de-DE
        
        // Restore previous versions (en-US, en-GB, en-AU same)
        {L"Restore previous versions", &g_settings.basicItems.removeRestoreVersions, false, nullptr},
        {L"Restaurar versões anteriores", &g_settings.basicItems.removeRestoreVersions, false, nullptr}, // pt-BR, pt-PT
        {L"Obnovit předchozí verze", &g_settings.basicItems.removeRestoreVersions, false, nullptr}, // cs-CZ
        {L"Vorgängerversionen wiederhestellen", &g_settings.basicItems.removeRestoreVersions, false, nullptr}, // de-DE
        
        // Include in library (en-US, en-GB, en-AU same)
        {L"Include in library", &g_settings.basicItems.removeIncludeInLibrary, false, nullptr},
        {L"Incluir na biblioteca", &g_settings.basicItems.removeIncludeInLibrary, false, nullptr}, // pt-BR, pt-PT
        {L"Zahrnout do knihovny", &g_settings.basicItems.removeIncludeInLibrary, false, nullptr}, // cs-CZ
        {L"In Bibliothek aufnehmen", &g_settings.basicItems.removeIncludeInLibrary, false, nullptr}, // de-DE
        
        // Rotate right (en-US, en-GB, en-AU same)
        {L"Rotate right", &g_settings.basicItems.removeRotate, false, nullptr},
        {L"Girar para a direita", &g_settings.basicItems.removeRotate, false, nullptr}, // pt-BR
        {L"Rodar para a direita", &g_settings.basicItems.removeRotate, false, nullptr}, // pt-PT
        {L"Otočit doprava", &g_settings.basicItems.removeRotate, false, nullptr}, // cs-CZ
        {L"Nach rechts drehen", &g_settings.basicItems.removeRotate, false, nullptr}, // de-DE
        
        // Rotate left (en-US, en-GB, en-AU same)
        {L"Rotate left", &g_settings.basicItems.removeRotate, false, nullptr},
        {L"Girar para a esquerda", &g_settings.basicItems.removeRotate, false, nullptr}, // pt-BR
        {L"Rodar para a esquerda", &g_settings.basicItems.removeRotate, false, nullptr}, // pt-PT
        {L"Otočit doleva", &g_settings.basicItems.removeRotate, false, nullptr}, // cs-CZ
        {L"Nach links drehen", &g_settings.basicItems.removeRotate, false, nullptr}, // de-DE
        
        // Display settings (en-US, en-GB, en-AU same)
        {L"Display settings", &g_settings.basicItems.removeDisplaySettings, false, nullptr},
        {L"Configurações de vídeo", &g_settings.basicItems.removeDisplaySettings, false, nullptr}, // pt-BR
        {L"Definições do ecrã", &g_settings.basicItems.removeDisplaySettings, false, nullptr}, // pt-PT
        {L"Nastavení zobrazení", &g_settings.basicItems.removeDisplaySettings, false, nullptr}, // cs-CZ
        {L"Anzeigeeinstellungen", &g_settings.basicItems.removeDisplaySettings, false, nullptr}, // de-DE
        
        // Personalize
        {L"Personalize", &g_settings.basicItems.removePersonalize, false, nullptr}, // en-US
        {L"Personalise", &g_settings.basicItems.removePersonalize, false, nullptr}, // en-GB, en-AU
        {L"Personalizar", &g_settings.basicItems.removePersonalize, false, nullptr}, // pt-BR, pt-PT
        {L"Přizpůsobit", &g_settings.basicItems.removePersonalize, false, nullptr}, // cs-CZ
        {L"Anpassen", &g_settings.basicItems.removePersonalize, false, nullptr}, // de-DE
        
        // Set as desktop background (en-US, en-GB, en-AU same)
        {L"Set as desktop background", &g_settings.basicItems.removeSetAsDesktopBackground, false, nullptr},
        {L"Definir como plano de fundo da área de trabalho", &g_settings.basicItems.removeSetAsDesktopBackground, false, nullptr}, // pt-BR
        {L"Definir como fundo do ambiente de trabalho", &g_settings.basicItems.removeSetAsDesktopBackground, false, nullptr}, // pt-PT
        {L"Nastavit jako pozadí plochy", &g_settings.basicItems.removeSetAsDesktopBackground, false, nullptr}, // cs-CZ
        {L"Als Desktophintergrund festlegen", &g_settings.basicItems.removeSetAsDesktopBackground, false, nullptr}, // de-DE
        
        // View (en-US, en-GB, en-AU same)
        {L"View", &g_settings.basicItems.removeView, false, nullptr},
        {L"Exibir", &g_settings.basicItems.removeView, false, nullptr}, // pt-BR
        {L"Ver", &g_settings.basicItems.removeView, false, nullptr}, // pt-PT
        {L"Zobrazení", &g_settings.basicItems.removeView, false, nullptr}, // cs-CZ
        {L"Ansicht", &g_settings.basicItems.removeView, false, nullptr}, // de-DE
        
        // Sort by (en-US, en-GB, en-AU same)
        {L"Sort by", &g_settings.basicItems.removeSortBy, false, nullptr},
        {L"Classificar por", &g_settings.basicItems.removeSortBy, false, nullptr}, // pt-BR
        {L"Ordenar por", &g_settings.basicItems.removeSortBy, false, nullptr}, // pt-PT
        {L"Seřadit podle", &g_settings.basicItems.removeSortBy, false, nullptr}, // cs-CZ
        {L"Sortieren nach", &g_settings.basicItems.removeSortBy, false, nullptr}, // de-DE
        
        // Group by (en-US, en-GB, en-AU same)
        {L"Group by", &g_settings.basicItems.removeGroupBy, false, nullptr},
        {L"Agrupar por", &g_settings.basicItems.removeGroupBy, false, nullptr}, // pt-BR, pt-PT
        {L"Seskupit podle", &g_settings.basicItems.removeGroupBy, false, nullptr}, // cs-CZ
        {L"Gruppieren nach", &g_settings.basicItems.removeGroupBy, false, nullptr}, // de-DE
        
        // New (en-US, en-GB, en-AU same)
        {L"New", &g_settings.basicItems.removeNew, false, nullptr},
        {L"Novo", &g_settings.basicItems.removeNew, false, nullptr}, // pt-BR, pt-PT
        {L"Nový", &g_settings.basicItems.removeNew, false, nullptr}, // cs-CZ
        {L"Neu", &g_settings.basicItems.removeNew, false, nullptr}, // de-DE
        
        // Properties (en-US, en-GB, en-AU same)
        {L"Properties", &g_settings.basicItems.removeProperties, false, nullptr},
        {L"Propriedades", &g_settings.basicItems.removeProperties, false, nullptr}, // pt-BR, pt-PT
        {L"Vlastnosti", &g_settings.basicItems.removeProperties, false, nullptr}, // cs-CZ
        {L"Eigenschaften", &g_settings.basicItems.removeProperties, false, nullptr}, // de-DE
        
        // Paste (en-US, en-GB, en-AU same)
        {L"Paste", &g_settings.basicItems.removePaste, false, nullptr},
        {L"Colar", &g_settings.basicItems.removePaste, false, nullptr}, // pt-BR, pt-PT
        {L"Vložit", &g_settings.basicItems.removePaste, false, nullptr}, // cs-CZ
        {L"Einfügen", &g_settings.basicItems.removePaste, false, nullptr}, // de-DE
        
        // Extract All (en-US, en-GB, en-AU same)
        {L"Extract All...", &g_settings.basicItems.removeExtractAll, false, nullptr},
        {L"Extrair Tudo...", &g_settings.basicItems.removeExtractAll, false, nullptr}, // pt-BR
        {L"Extrair Todos...", &g_settings.basicItems.removeExtractAll, false, nullptr}, // pt-PT
        {L"Extrahovat vše...", &g_settings.basicItems.removeExtractAll, false, nullptr}, // cs-CZ
        {L"Alle extrahieren...", &g_settings.basicItems.removeExtractAll, false, nullptr}, // de-DE
        
        // App-specific Items
        // Add to VLC media player's Playlist (en-US, en-GB, en-AU same)
        {L"Add to VLC media player's Playlist", &g_settings.appSpecificItems.removeVLCPlaylist, false, nullptr},
        {L"Adicionar à lista de reprodução do VLC media player", &g_settings.appSpecificItems.removeVLCPlaylist, false, nullptr}, // pt-BR, pt-PT
        {L"Přidat do seznamu stop přehrávače médií VLC", &g_settings.appSpecificItems.removeVLCPlaylist, false, nullptr}, // cs-CZ
        {L"Zur VLC media player Wiedergabeliste hinzufügen", &g_settings.appSpecificItems.removeVLCPlaylist, false, nullptr}, // de-DE
        
        // Play with VLC media player (en-US, en-GB, en-AU same)
        {L"Play with VLC media player", &g_settings.appSpecificItems.removeVLCPlay, false, nullptr},
        {L"Reproduzir com o VLC media player", &g_settings.appSpecificItems.removeVLCPlay, false, nullptr}, // pt-BR, pt-PT
        {L"Přehrát přehrávačem médií VLC", &g_settings.appSpecificItems.removeVLCPlay, false, nullptr}, // cs-CZ
        {L"Mit VLC media player wiedergeben", &g_settings.appSpecificItems.removeVLCPlay, false, nullptr}, // de-DE
        
        // Add to Media Player play queue (en-US, en-GB, en-AU same)
        {L"Add to Media Player play queue", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, false, nullptr},
        {L"Adicionar à fila de reprodução do Media Player", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, false, nullptr}, // pt-BR, pt-PT
        {L"Přidat do fronty přehrávání přehrávače médií", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, false, nullptr}, // cs-CZ
        {L"Zur Windows Media Player-Wiedergabeliste hinzufügen", &g_settings.appSpecificItems.removeAddToMediaPlayerQueue, false, nullptr}, // de-DE
        
        // Play with Media Player (en-US, en-GB, en-AU same)
        {L"Play with Media Player", &g_settings.appSpecificItems.removePlayWithMediaPlayer, false, nullptr},
        {L"Reproduzir com o Media Player", &g_settings.appSpecificItems.removePlayWithMediaPlayer, false, nullptr}, // pt-BR, pt-PT
        {L"Přehrát přehrávačem médií", &g_settings.appSpecificItems.removePlayWithMediaPlayer, false, nullptr}, // cs-CZ
        {L"Mit Windows Media Player wiedergeben", &g_settings.appSpecificItems.removePlayWithMediaPlayer, false, nullptr}, // de-DE
        
        // Edit in Notepad (en-US, en-GB, en-AU same)
        {L"Edit in Notepad", &g_settings.appSpecificItems.removeEditInNotepad, true, &g_settings.extensionFiltering.notepadExtensions},
        {L"Editar no Bloco de Notas", &g_settings.appSpecificItems.removeEditInNotepad, true, &g_settings.extensionFiltering.notepadExtensions}, // pt-BR, pt-PT
        {L"Upravit v Poznámkovém bloku", &g_settings.appSpecificItems.removeEditInNotepad, true, &g_settings.extensionFiltering.notepadExtensions}, // cs-CZ
        {L"Im Editor bearbeiten", &g_settings.appSpecificItems.removeEditInNotepad, true, &g_settings.extensionFiltering.notepadExtensions}, // de-DE
        
        // Edit in Notepad++ (en-US, en-GB, en-AU same)
        {L"Edit in Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, true, &g_settings.extensionFiltering.notepadExtensions},
        {L"Editar no Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, true, &g_settings.extensionFiltering.notepadExtensions}, // pt-BR, pt-PT
        {L"Upravit v aplikaci Notepad++", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, true, &g_settings.extensionFiltering.notepadExtensions}, // cs-CZ
        {L"Mit Notepad++ bearbeiten", &g_settings.appSpecificItems.removeEditInNotepadPlusPlus, true, &g_settings.extensionFiltering.notepadExtensions}, // de-DE
        
        // Edit with Photos (en-US, en-GB, en-AU same)
        {L"Edit with Photos", &g_settings.appSpecificItems.removeEditWithPhotos, false, nullptr},
        {L"Editar com Fotos", &g_settings.appSpecificItems.removeEditWithPhotos, false, nullptr}, // pt-BR
        {L"Editar com Fotografias", &g_settings.appSpecificItems.removeEditWithPhotos, false, nullptr}, // pt-PT
        {L"Upravit pomocí Fotky", &g_settings.appSpecificItems.removeEditWithPhotos, false, nullptr}, // cs-CZ
        {L"Mit Fotos bearbeiten", &g_settings.appSpecificItems.removeEditWithPhotos, false, nullptr}, // de-DE
        
        // Edit with Paint (en-US, en-GB, en-AU same)
        {L"Edit with Paint", &g_settings.appSpecificItems.removeEditWithPaint, false, nullptr},
        {L"Editar com o Paint", &g_settings.appSpecificItems.removeEditWithPaint, false, nullptr}, // pt-BR, pt-PT
        {L"Upravit pomocí Malování", &g_settings.appSpecificItems.removeEditWithPaint, false, nullptr}, // cs-CZ
        {L"Mit Paint bearbeiten", &g_settings.appSpecificItems.removeEditWithPaint, false, nullptr}, // de-DE
        
        // NVIDIA Control Panel (en-US, en-GB, en-AU same)
        {L"NVIDIA Control Panel", &g_settings.appSpecificItems.removeNvidiaControlPanel, false, nullptr},
        {L"Painel de Controle NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, false, nullptr}, // pt-BR
        {L"Painel de Controlo da NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, false, nullptr}, // pt-PT
        {L"Ovládací panely NVIDIA", &g_settings.appSpecificItems.removeNvidiaControlPanel, false, nullptr}, // cs-CZ
        {L"NVIDIA Systemsteuerung", &g_settings.appSpecificItems.removeNvidiaControlPanel, false, nullptr}, // de-DE
        
        // Open in Terminal (en-US, en-GB, en-AU same)
        {L"Open in Terminal", &g_settings.appSpecificItems.removeOpenInTerminal, false, nullptr},
        {L"Abrir no Terminal", &g_settings.appSpecificItems.removeOpenInTerminal, false, nullptr}, // pt-BR, pt-PT
        {L"Otevřít v terminálu", &g_settings.appSpecificItems.removeOpenInTerminal, false, nullptr}, // cs-CZ
        {L"In Terminal öffnen", &g_settings.appSpecificItems.removeOpenInTerminal, false, nullptr}, // de-DE
        
        // WinRAR (same in all languages)
        {L"WinRAR", &g_settings.appSpecificItems.removeWinRAR, false, nullptr}
    };
}


// Utility function to remove ampersands for hotkey underlines
std::wstring RemoveAmpersands(const std::wstring& str) {
    std::wstring result;
    result.reserve(str.length());
    for (wchar_t c : str) {
        if (c != L'&') {
            result += c;
        }
    }
    return result;
}

// Utility function to convert string to lowercase
std::wstring ToLower(const std::wstring& str) {
    std::wstring result = str;
    for (auto& c : result) {
        c = towlower(c);
    }
    return result;
}

// Utility function to normalize string for comparison
std::wstring NormalizeString(const std::wstring& str) {
    std::wstring result = ToLower(str);
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

// Function to check if a menu item should be removed based on extension filtering
bool ShouldRemoveByExtension(const MenuItem& item) {
    // If extension filtering is disabled, don't apply extension logic
    if (!g_settings.extensionFiltering.enableExtensionFiltering) {
        return false;
    }
    
    // If this item doesn't require extension check, don't filter it
    if (!item.requiresExtensionCheck || !item.allowedExtensions) {
        return false;
    }
    
    // If no file paths are available (e.g., right-clicking on empty space), don't filter
    if (g_threadFilePaths.empty()) {
        Wh_Log(L"ShouldRemoveByExtension: No file paths available");
        return false;
    }
    
    // Check if ANY of the selected files has an extension in the whitelist
    bool hasMatchingExtension = AnyFileHasExtensionInList(*item.allowedExtensions);
    
    Wh_Log(L"ShouldRemoveByExtension for '%s': hasMatchingExtension=%d, will remove=%d", 
           item.text.c_str(), hasMatchingExtension, !hasMatchingExtension);
    
    // Whitelist mode: Remove if NO file has a matching extension
    return !hasMatchingExtension;
}

// Function to check if a menu item should be removed
bool ShouldRemoveMenuItem(const std::wstring& text) {
    std::wstring cleanText = NormalizeString(RemoveAmpersands(text));
    
    // Check against predefined items
    for (const auto& item : g_menuItems) {
        std::wstring cleanItemText = NormalizeString(RemoveAmpersands(item.text));
        if (cleanText == cleanItemText) {
            bool isEnabled = *(item.enabled);
            
            Wh_Log(L"ShouldRemoveMenuItem for '%s': isEnabled=%d, requiresExtCheck=%d, extFilterEnabled=%d",
                   item.text.c_str(), isEnabled, item.requiresExtensionCheck, 
                   g_settings.extensionFiltering.enableExtensionFiltering);
            
            // Special handling for extension-filtered items
            if (item.requiresExtensionCheck && g_settings.extensionFiltering.enableExtensionFiltering) {
                // When extension filtering is enabled for this item:
                // - If the setting is ON (removeEditInNotepad = true), apply whitelist logic
                // - Remove the item if extension is NOT in whitelist
                if (isEnabled) {
                    bool shouldRemove = ShouldRemoveByExtension(item);
                    Wh_Log(L"  -> Extension filtering active, final decision: remove=%d", shouldRemove);
                    return shouldRemove;
                }
                // If the setting is OFF, never remove this item
                Wh_Log(L"  -> Setting is OFF, keeping item");
                return false;
            }
            
            // Normal behavior for non-extension-filtered items
            Wh_Log(L"  -> Normal removal logic, remove=%d", isEnabled);
            return isEnabled;
        }
    }
    
    // Check custom items with exact/wildcard matching
    for (const auto& customItem : g_settings.customItems) {
        if (!customItem.empty()) {
            std::wstring cleanCustomItem = NormalizeString(RemoveAmpersands(customItem));
            
            if (MatchesCustomItem(cleanText, cleanCustomItem)) {
                Wh_Log(L"Removing custom item: %s (matched: %s)", cleanText.c_str(), cleanCustomItem.c_str());
                return true;
            }
        }
    }
    
    return false;
}

// Function to process a menu and remove unwanted items
void ProcessMenu(HMENU hMenu) {
    if (!hMenu) return;
    
    int itemCount = GetMenuItemCount(hMenu);
    
    // Iterate through menu items in reverse to safely remove items
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {0};
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_STRING | MIIM_SUBMENU | MIIM_FTYPE;
        
        // Get the length of the menu item text
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
            if (mii.cch > 0) {
                // Allocate buffer and get the actual text
                std::wstring text(mii.cch + 1, L'\0');
                mii.dwTypeData = &text[0];
                mii.cch++;
                
                if (GetMenuItemInfoW(hMenu, i, TRUE, &mii)) {
                    text.resize(wcslen(text.c_str()));
                    
                    // Check if this item should be removed
                    if (ShouldRemoveMenuItem(text)) {
                        DeleteMenu(hMenu, i, MF_BYPOSITION);
                    }
                }
            }
            
            // Recursively process submenus
            if (mii.hSubMenu) {
                ProcessMenu(mii.hSubMenu);
            }
        }
    }
    
    // Remove consecutive separators
    itemCount = GetMenuItemCount(hMenu);
    bool lastWasSeparator = false;
    
    for (int i = itemCount - 1; i >= 0; i--) {
        MENUITEMINFOW mii = {0};
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
        MENUITEMINFOW mii = {0};
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
        MENUITEMINFOW mii = {0};
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

// Hook function for TrackPopupMenuEx
BOOL WINAPI TrackPopupMenuEx_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    HWND hWnd,
    LPTPMPARAMS lptpm
) {
    // Initialize COM on this thread (hook runs on different thread than Wh_ModInit)
    HRESULT hrCom = CoInitialize(NULL);
    bool comInitialized = SUCCEEDED(hrCom);
    
    // Clear any stale file paths from previous calls
    g_threadFilePaths.clear();
    
    // Get selected files from Explorer
    g_threadFilePaths = GetSelectedFilesFromExplorer(hWnd);
    
    // Log current file paths
    if (!g_threadFilePaths.empty()) {
        Wh_Log(L"TrackPopupMenuEx called with %d files:", g_threadFilePaths.size());
        for (const auto& path : g_threadFilePaths) {
            Wh_Log(L"  - %s (ext: %s)", path.c_str(), GetFileExtension(path).c_str());
        }
    } else {
        Wh_Log(L"TrackPopupMenuEx called with no file context");
    }
    
    ProcessMenu(hMenu);
    
    // Clear file paths after processing
    g_threadFilePaths.clear();
    
    // Uninitialize COM on this thread
    if (comInitialized) {
        CoUninitialize();
    }
    
    return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
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
    // Initialize COM on this thread (hook runs on different thread than Wh_ModInit)
    HRESULT hrCom = CoInitialize(NULL);
    bool comInitialized = SUCCEEDED(hrCom);
    
    // Clear any stale file paths from previous calls
    g_threadFilePaths.clear();
    
    // Get selected files from Explorer
    g_threadFilePaths = GetSelectedFilesFromExplorer(hWnd);
    
    // Log current file paths
    if (!g_threadFilePaths.empty()) {
        Wh_Log(L"TrackPopupMenu called with %d files:", g_threadFilePaths.size());
        for (const auto& path : g_threadFilePaths) {
            Wh_Log(L"  - %s (ext: %s)", path.c_str(), GetFileExtension(path).c_str());
        }
    } else {
        Wh_Log(L"TrackPopupMenu called with no file context");
    }
    
    ProcessMenu(hMenu);
    
    // Clear file paths after processing
    g_threadFilePaths.clear();
    
    // Uninitialize COM on this thread
    if (comInitialized) {
        CoUninitialize();
    }
    
    return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

// Load settings
void LoadSettings() {
    // Bloatware items
    g_settings.bloatwareItems.removeOneDrive = Wh_GetIntSetting(L"bloatwareItems.removeOneDrive");
    g_settings.bloatwareItems.removeCopilot = Wh_GetIntSetting(L"bloatwareItems.removeCopilot");
    g_settings.bloatwareItems.removeDefender = Wh_GetIntSetting(L"bloatwareItems.removeDefender");
    g_settings.bloatwareItems.removeDesigner = Wh_GetIntSetting(L"bloatwareItems.removeDesigner");
    g_settings.bloatwareItems.removeClipchamp = Wh_GetIntSetting(L"bloatwareItems.removeClipchamp");
    
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
    
    // Load extension lists
    g_settings.extensionFiltering.notepadExtensions.clear();
    int maxItems = 100;
    for (int i = 0; i < maxItems; i++) {
        PCWSTR ext = Wh_GetStringSetting(L"extensionFiltering.notepadExtensions[%d]", i);
        if (!ext) break;
        std::wstring extension(ext);
        Wh_FreeStringSetting(ext);
        if (!extension.empty()) {
            g_settings.extensionFiltering.notepadExtensions.push_back(extension);
        }
    }
    
    // Load custom items
    g_settings.customItems.clear();
    for (int i = 0; i < maxItems; i++) {
        PCWSTR customItem = Wh_GetStringSetting(L"customItems[%d]", i);
        if (!customItem) break;
        
        std::wstring item(customItem);
        Wh_FreeStringSetting(customItem);
        
        if (!item.empty()) {
            g_settings.customItems.push_back(item);
            Wh_Log(L"Loaded custom item %d: %s", i, item.c_str());
        }
    }
    
    Wh_Log(L"Total custom items loaded: %d", (int)g_settings.customItems.size());
    Wh_Log(L"Extension filtering enabled: %d", g_settings.extensionFiltering.enableExtensionFiltering);
    Wh_Log(L"Notepad extensions count: %d", (int)g_settings.extensionFiltering.notepadExtensions.size());
    
    InitializeMenuItems();
}

// Windhawk mod initialization
BOOL Wh_ModInit() {
    Wh_Log(L"Initializing context menu cleaner mod");
    Wh_Log(L"NOTE: Predefined options are in English. For other languages, use Custom Items in settings.");
    
    LoadSettings();
    
    if (!Wh_SetFunctionHook(
        (void*)TrackPopupMenuEx,
        (void*)TrackPopupMenuEx_Hook,
        (void**)&TrackPopupMenuEx_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
        return FALSE;
    }
    
    if (!Wh_SetFunctionHook(
        (void*)TrackPopupMenu,
        (void*)TrackPopupMenu_Hook,
        (void**)&TrackPopupMenu_Original
    )) {
        Wh_Log(L"Failed to hook TrackPopupMenu");
        return FALSE;
    }
    
    Wh_Log(L"Context menu cleaner mod initialized successfully");
    return TRUE;
}

// Settings changed callback
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    LoadSettings();
}

// Windhawk mod cleanup
void Wh_ModUninit() {
    Wh_Log(L"Uninitializing context menu cleaner mod");
}

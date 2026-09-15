// ==WindhawkMod==
// @id              explorer-custom-shortcuts
// @name            Explorer Custom Shortcuts
// @description     Adds app-style keyboard shortcuts to File Explorer with dynamic tokens, selection modes, and internal commands.
// @version         1.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lole32 -loleaut32 -luuid -lshlwapi -lcomctl32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Custom Shortcuts

Adds customizable, app-style keyboard shortcuts to Windows File Explorer with parameter substitution, built-in shell commands, and custom token expansion.

> **Input Protection:** All custom shortcuts and configured Escape key actions are automatically suppressed while renaming files, typing into the Address/Breadcrumb bar, typing into the Search box, or while focus is actively inside dialogs (such as Properties, Delete/Replace confirmations). When focus returns to the main File Explorer window, shortcuts resume immediately.

---

### Internal Explorer Commands

Instead of specifying an executable path, set **Executable Path** to one of the following `internal:` keywords:

* **`internal:newTextFile`**: Creates a `New Text Document.txt` in the active folder and automatically selects/focuses it without UI freezes.
* **`internal:newFolder`**: Creates a `New Folder` in the active folder and enters inline rename mode immediately.
* **`internal:openRecycleBin`**: Navigates to the Recycle Bin in the current active tab.
* **`internal:emptyRecycleBin`**: Silently empties the Recycle Bin after a native confirmation prompt.
* **`internal:toggleHiddenFiles`**: Instantly toggle visibility of hidden files and folders with immediate view refresh.
* **`internal:toggleFileExtensions`**: Instantly toggle file name extensions on or off with immediate view refresh.
* **`internal:folderOptions`**: Opens the native File Explorer Folder Options dialog.

---

### Available Tokens for Arguments & Examples

* **`%f`** — All selected items as space-separated quoted paths (`"C:\a.txt" "C:\b.png"`).
* **`%files`** — Selected regular files only (skips selected folders).
* **`%folders`** — Selected folders only (skips selected regular files).
* **`%1`** — Quoted path of the first selected file or directory (`"C:\a.txt"`).
* **`%n`** — File or folder names only without absolute directory paths (`"a.txt"`).
* **`%c`** — Total number of selected items as an integer (`3`).
* **`%ext`** — File extension of the first selected item (`.png`).
* **`%s`** — Space-separated paths with **smart-quoting** (automatically adds double-quotes only to paths containing spaces).
* **`%d`** — Active directory open in the current tab (`C:\Users\Name\Documents`).
* **`%d_smart`** — Selected folder if one is highlighted; otherwise falls back to the current active directory.
* **`%p`** — Parent directory path of the active tab.

---

### Launch Modes & Use Cases

* **`batch`** (Default): Executes the application exactly once and passes the resolved command-line arguments.
  * *Use Case:* Code editors (opening a directory), Media players (queuing files), or bulk tools like PowerRename.
* **`loop_files`**: Executes the application once per selected file.
  * *Use Case:* Single-file image editors, batch converters, or per-file processing scripts.
* **`loop_folders`**: Executes the application once per selected folder.
  * *Use Case:* Batch archiving folders individually or folder-specific terminal operations.

---

### Useful Default File Explorer Shortcuts

* **`Alt + P`**: Native toggle for Preview Pane
* **`Alt + Shift + P`**: Native toggle for Details Pane
* **`Ctrl + D`** / **`Delete`**: Delete selected item(s)
* **`Shift + Delete`**: Permanently delete selected item(s)
* **`Ctrl + Shift + N`**: Create a new folder
* **`F2`**: Rename selected item
* **`Ctrl + T`**: Open a new Explorer tab
* **`Ctrl + W`**: Close current tab

---

### Preset Ideas & External App Examples

You can add your own shortcuts using these templates in the settings:

* **Open Windows Terminal Here**
  * Path: `wt.exe` | Args: `-d "%d"` | Mode: `batch`
* **Open with Notepad**
  * Path: `notepad.exe` | Args: `%1` | Mode: `loop_files`
* **PowerToys PowerRename** (Bulk rename selected items)
  * Path: `PowerToys.PowerRename.exe` | Args: `%f` | Mode: `batch`
* **Search with Everything** (Search current folder or selection)
  * Path: `C:\Program Files\Everything\Everything.exe` | Args: `-path "%d_smart"` | Mode: `batch`
* **Open in VS Code**
  * Path: `code` | Args: `"%d_smart"` | Mode: `batch`
* **Queue in VLC Media Player**
  * Path: `vlc.exe` | Args: `%f` | Mode: `batch`
* **Copy Just File Names to Clipboard**
  * Path: `powershell.exe` | Args: `-WindowStyle Hidden -Command "Set-Clipboard -Value '%n'"` | Mode: `batch`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shortcuts:
  - - name: "Open Terminal Here"
      $name: "Action Name"
    - enabled: true
      $name: "Enabled"
    - key: "T"
      $name: "Key (A-Z, 0-9, F1-F12)"
    - ctrl: true
      $name: "Require Ctrl"
    - shift: false
      $name: "Require Shift"
    - alt: true
      $name: "Require Alt"
    - path: "wt.exe"
      $name: "Executable Path or Internal Command"
      $description: "Executable path, alias, or internal command (e.g. internal:newTextFile, internal:toggleHiddenFiles, wt.exe, notepad.exe)."
    - args: "-d \"%d\""
      $name: "Arguments / Tokens"
      $description: "Supported tokens: %f, %files, %folders, %1, %n, %c, %ext, %s, %d, %d_smart, %p."
    - mode: "batch"
      $name: "Launch Mode"
      $description: "batch, loop_files, or loop_folders."
  - - name: "Open with Notepad"
    - enabled: true
    - key: "N"
    - ctrl: false
    - shift: false
    - alt: true
    - path: "notepad.exe"
    - args: "%1"
    - mode: "loop_files"
  - - name: "Toggle Hidden Files"
    - enabled: true
    - key: "H"
    - ctrl: true
    - shift: false
    - alt: false
    - path: "internal:toggleHiddenFiles"
    - args: ""
    - mode: "batch"
  - - name: "Open Recycle Bin"
    - enabled: true
    - key: "B"
    - ctrl: true
    - shift: true
    - alt: false
    - path: "internal:openRecycleBin"
    - args: ""
    - mode: "batch"
  $name: "Custom Shortcuts"
  $description: "List of customizable shortcuts. Supported keys: A-Z, 0-9, and F1-F12. Letters and digits require at least one modifier key (Ctrl, Shift, or Alt). Function keys (F1-F12) can be used standalone."
- escAction: "disabled"
  $name: "Escape Key Action"
  $description: "Action to perform when pressing Escape in File Explorer."
  $options:
    - disabled: "Disabled (Default Explorer behavior)"
    - close_tab: "Close Active Tab"
    - close_window: "Close Entire Window"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <initguid.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shlguid.h>
#include <shlwapi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <atomic>
#include <shellapi.h>
#include <commctrl.h>

struct CustomShortcut {
    std::wstring name;
    bool enabled = true;
    int vkCode = 0;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    std::wstring path;
    std::wstring argsPattern;
    std::wstring launchMode;
};

std::vector<CustomShortcut> g_shortcuts;
std::wstring g_escAction = L"disabled";
static std::atomic<bool> g_isExecutingInternal{false};

static constexpr CLSID kCLSID_ShellWindows = {
    0x9ba05972, 0xf6a8, 0x11cf, {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}
};

static constexpr GUID kSID_STopLevelBrowser = {
    0x4c96be40, 0x915c, 0x11cf, {0x99, 0xd3, 0x00, 0xaa, 0x00, 0x4a, 0xe8, 0x37}
};

std::wstring ExpandEnv(const std::wstring& input) {
    WCHAR buf[MAX_PATH * 2];
    DWORD len = ExpandEnvironmentStringsW(input.c_str(), buf, ARRAYSIZE(buf));
    return (len > 0 && len <= ARRAYSIZE(buf)) ? buf : input;
}

std::wstring ResolveCommandPath(const std::wstring& command) {
    std::wstring expanded = ExpandEnv(command);
    if (expanded.find(L'\\') != std::wstring::npos) {
        return expanded;
    }

    WCHAR resolved[MAX_PATH];
    if (SearchPathW(nullptr, expanded.c_str(), L".exe", ARRAYSIZE(resolved), resolved, nullptr)) {
        return resolved;
    }

    std::wstring keyPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + expanded;
    if (keyPath.size() < 4 || _wcsicmp(keyPath.c_str() + keyPath.size() - 4, L".exe") != 0) {
        keyPath += L".exe";
    }

    for (HKEY rootKey : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        WCHAR buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (RegGetValueW(rootKey, keyPath.c_str(), nullptr, RRF_RT_REG_SZ, nullptr, buffer, &size) == ERROR_SUCCESS && buffer[0]) {
            std::wstring appPath = ExpandEnv(buffer);
            if (appPath.size() >= 2 && appPath.front() == L'"' && appPath.back() == L'"') {
                appPath = appPath.substr(1, appPath.size() - 2);
            }
            return appPath;
        }
    }
    return expanded;
}

int ParseKey(std::wstring keyStr) {
    keyStr.erase(keyStr.begin(), std::find_if(keyStr.begin(), keyStr.end(), [](wchar_t ch) { return !iswspace(ch); }));
    keyStr.erase(std::find_if(keyStr.rbegin(), keyStr.rend(), [](wchar_t ch) { return !iswspace(ch); }).base(), keyStr.end());
    std::transform(keyStr.begin(), keyStr.end(), keyStr.begin(), ::towupper);
    if (keyStr.empty()) return 0;

    if (keyStr.length() > 1 && keyStr[0] == L'F') {
        try {
            int fNum = std::stoi(keyStr.substr(1));
            if (fNum >= 1 && fNum <= 12) {
                return VK_F1 + (fNum - 1);
            }
        } catch (...) {}
        return 0;
    }

    if (_wcsicmp(keyStr.c_str(), L"DELETE") == 0 || _wcsicmp(keyStr.c_str(), L"DEL") == 0) return VK_DELETE;
    if (_wcsicmp(keyStr.c_str(), L"SPACE") == 0) return VK_SPACE;
    if (_wcsicmp(keyStr.c_str(), L"ENTER") == 0) return VK_RETURN;
    if (_wcsicmp(keyStr.c_str(), L"TAB") == 0) return VK_TAB;
    
    if (keyStr.length() == 1) {
        wchar_t c = keyStr[0];
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return c;
        }
    }
    return 0;
}

void LoadSettings() {
    g_shortcuts.clear();

    PCWSTR escStr = Wh_GetStringSetting(L"escAction");
    if (escStr) {
        g_escAction = escStr;
        Wh_FreeStringSetting(escStr);
    } else {
        g_escAction = L"disabled";
    }

    for (int i = 0; i < 100; i++) {
        PCWSTR pathStr = Wh_GetStringSetting(L"shortcuts[%d].path", i);
        if (!pathStr) break;

        CustomShortcut sc;
        sc.path = pathStr;
        Wh_FreeStringSetting(pathStr);

        sc.enabled = Wh_GetIntSetting(L"shortcuts[%d].enabled", i) != 0;

        PCWSTR nameStr = Wh_GetStringSetting(L"shortcuts[%d].name", i);
        if (nameStr) { sc.name = nameStr; Wh_FreeStringSetting(nameStr); }

        PCWSTR keyStr = Wh_GetStringSetting(L"shortcuts[%d].key", i);
        if (keyStr) { sc.vkCode = ParseKey(keyStr); Wh_FreeStringSetting(keyStr); }

        sc.ctrl = Wh_GetIntSetting(L"shortcuts[%d].ctrl", i) != 0;
        sc.shift = Wh_GetIntSetting(L"shortcuts[%d].shift", i) != 0;
        sc.alt = Wh_GetIntSetting(L"shortcuts[%d].alt", i) != 0;

        PCWSTR argsStr = Wh_GetStringSetting(L"shortcuts[%d].args", i);
        if (argsStr) { sc.argsPattern = argsStr; Wh_FreeStringSetting(argsStr); }

        PCWSTR modeStr = Wh_GetStringSetting(L"shortcuts[%d].mode", i);
        if (modeStr) { sc.launchMode = modeStr; Wh_FreeStringSetting(modeStr); }
        if (sc.launchMode.empty()) sc.launchMode = L"batch";

        bool isFunctionKey = (sc.vkCode >= VK_F1 && sc.vkCode <= VK_F12);
        bool hasModifier = (sc.ctrl || sc.shift || sc.alt);

        if (sc.enabled && sc.vkCode != 0 && (hasModifier || isFunctionKey)) {
            g_shortcuts.push_back(sc);
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

IShellView* GetActiveShellView(HWND hExplorerWnd) {
    IShellWindows* pShellWindows = nullptr;
    if (FAILED(CoCreateInstance(kCLSID_ShellWindows, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pShellWindows))) || !pShellWindows) {
        return nullptr;
    }

    long count = 0;
    pShellWindows->get_Count(&count);

    HWND hActiveTabWnd = nullptr;
    if (hExplorerWnd) {
        HWND hFocus = GetFocus();
        for (HWND h = hFocus; h && h != hExplorerWnd; h = GetParent(h)) {
            WCHAR cls[64];
            if (GetClassNameW(h, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"ShellTabWindowClass") == 0) {
                hActiveTabWnd = h;
                break;
            }
        }
        if (!hActiveTabWnd) {
            hActiveTabWnd = FindWindowExW(hExplorerWnd, nullptr, L"ShellTabWindowClass", nullptr);
        }
    }

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* pDispatch = nullptr;
        if (FAILED(pShellWindows->Item(index, &pDispatch)) || !pDispatch) continue;

        IWebBrowser2* pWebBrowser = nullptr;
        if (SUCCEEDED(pDispatch->QueryInterface(IID_PPV_ARGS(&pWebBrowser)))) {
            SHANDLE_PTR hWndRaw = 0;
            if (SUCCEEDED(pWebBrowser->get_HWND(&hWndRaw)) && (!hExplorerWnd || (HWND)hWndRaw == hExplorerWnd)) {
                IServiceProvider* pServiceProvider = nullptr;
                if (SUCCEEDED(pDispatch->QueryInterface(IID_PPV_ARGS(&pServiceProvider)))) {
                    IShellBrowser* pShellBrowser = nullptr;
                    if (SUCCEEDED(pServiceProvider->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&pShellBrowser)))) {
                        HWND hTabWnd = nullptr;
                        if (SUCCEEDED(pShellBrowser->GetWindow(&hTabWnd)) && hTabWnd) {
                            if (hActiveTabWnd && hTabWnd != hActiveTabWnd) {
                                pShellBrowser->Release();
                                pServiceProvider->Release();
                                pWebBrowser->Release();
                                pDispatch->Release();
                                continue;
                            }
                        }

                        IShellView* pShellView = nullptr;
                        if (SUCCEEDED(pShellBrowser->QueryActiveShellView(&pShellView)) && pShellView) {
                            pShellBrowser->Release();
                            pServiceProvider->Release();
                            pWebBrowser->Release();
                            pDispatch->Release();
                            pShellWindows->Release();
                            return pShellView;
                        }
                        pShellBrowser->Release();
                    }
                    pServiceProvider->Release();
                }
            }
            pWebBrowser->Release();
        }
        pDispatch->Release();
    }

    pShellWindows->Release();
    return nullptr;
}

std::wstring GetActiveFolderPath(HWND hwndExplorer) {
    std::wstring result;
    IShellView* psv = GetActiveShellView(hwndExplorer);
    if (!psv) return result;

    IFolderView* pfv = nullptr;
    if (SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&pfv)))) {
        IPersistFolder2* ppf2 = nullptr;
        if (SUCCEEDED(pfv->GetFolder(IID_PPV_ARGS(&ppf2)))) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(ppf2->GetCurFolder(&pidl)) && pidl) {
                WCHAR path[MAX_PATH];
                if (SHGetPathFromIDListEx(pidl, path, ARRAYSIZE(path), GPFIDL_DEFAULT)) {
                    result = path;
                }
                CoTaskMemFree(pidl);
            }
            ppf2->Release();
        }
        pfv->Release();
    }
    psv->Release();
    return result;
}

std::vector<std::wstring> GetSelectedPaths(HWND hwndExplorer) {
    std::vector<std::wstring> files;
    IShellView* psv = GetActiveShellView(hwndExplorer);
    if (!psv) return files;

    IDataObject* pdo = nullptr;
    if (SUCCEEDED(psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(&pdo)))) {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg = { 0 };
        if (SUCCEEDED(pdo->GetData(&fmt, &stg))) {
            HDROP hDrop = (HDROP)stg.hGlobal;
            UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            for (UINT j = 0; j < fileCount; j++) {
                WCHAR path[MAX_PATH];
                if (DragQueryFileW(hDrop, j, path, MAX_PATH)) {
                    files.push_back(path);
                }
            }
            ReleaseStgMedium(&stg);
        }
        pdo->Release();
    }
    psv->Release();
    return files;
}

void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::wstring JoinPaths(const std::vector<std::wstring>& paths, bool forceQuotes = true) {
    std::wstring res;
    for (const auto& p : paths) {
        if (!res.empty()) res += L" ";
        if (forceQuotes || p.find(L' ') != std::wstring::npos) {
            res += L"\"" + p + L"\"";
        } else {
            res += p;
        }
    }
    return res;
}

std::wstring GetFileNamesOnly(const std::vector<std::wstring>& paths, bool forceQuotes = true) {
    std::wstring res;
    for (const auto& p : paths) {
        if (!res.empty()) res += L" ";
        WCHAR nameBuf[MAX_PATH];
        wcscpy_s(nameBuf, p.c_str());
        PathStripPathW(nameBuf);
        std::wstring name(nameBuf);
        if (forceQuotes || name.find(L' ') != std::wstring::npos) {
            res += L"\"" + name + L"\"";
        } else {
            res += name;
        }
    }
    return res;
}

std::wstring GetFileExtension(const std::vector<std::wstring>& paths) {
    if (paths.empty()) return L"";
    PCWSTR ext = PathFindExtensionW(paths[0].c_str());
    return ext ? ext : L"";
}

void ExecuteApp(const std::wstring& cmd, const std::wstring& params, const std::wstring& workDir, HWND hwndParent) {
    std::wstring targetPath = ResolveCommandPath(cmd);

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOASYNC;
    sei.hwnd = hwndParent;
    sei.lpVerb = L"open";
    sei.lpFile = targetPath.c_str();
    sei.lpParameters = params.empty() ? nullptr : params.c_str();
    sei.lpDirectory = workDir.empty() ? nullptr : workDir.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"ShellExecuteExW failed for %s (error %lu)", targetPath.c_str(), GetLastError());
    }
}

void ExecuteInternalCommand(const std::wstring& command, HWND rootHwnd) {
    // 1. Open Folder Options
    if (_wcsicmp(command.c_str(), L"internal:folderOptions") == 0) {
        SHELLEXECUTEINFOW sei = { sizeof(sei) };
        sei.fMask = SEE_MASK_NOASYNC;
        sei.hwnd = rootHwnd;
        sei.lpVerb = L"open";
        sei.lpFile = L"rundll32.exe";
        sei.lpParameters = L"shell32.dll,Options_RunDLL 0";
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
        return;
    }

    // 2. Create New Text Document & Focus/Select
    if (_wcsicmp(command.c_str(), L"internal:newTextFile") == 0) {
        if (g_isExecutingInternal.exchange(true)) return;
        struct AutoReset { ~AutoReset() { g_isExecutingInternal = false; } } autoReset;

        std::wstring currentDir = GetActiveFolderPath(rootHwnd);
        if (currentDir.empty()) return;

        std::wstring targetFilePath = currentDir + L"\\New Text Document.txt";
        int counter = 2;
        while (GetFileAttributesW(targetFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            targetFilePath = currentDir + L"\\New Text Document (" + std::to_wstring(counter++) + L").txt";
        }

        HANDLE hFile = CreateFileW(targetFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW, targetFilePath.c_str(), nullptr);
            Sleep(80);

            IShellView* psv = GetActiveShellView(rootHwnd);
            if (psv) {
                PIDLIST_ABSOLUTE pidlTarget = nullptr;
                if (SUCCEEDED(SHParseDisplayName(targetFilePath.c_str(), nullptr, &pidlTarget, 0, nullptr)) && pidlTarget) {
                    PITEMID_CHILD pidlChild = ILFindLastID(pidlTarget);
                    if (pidlChild) {
                        psv->SelectItem(pidlChild, SVSI_SELECT | SVSI_FOCUSED | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE);
                    }
                    CoTaskMemFree(pidlTarget);
                }
                psv->Release();
            }
        }
        return;
    }

    // 3. Create New Folder & Enter Inline Rename Mode
    if (_wcsicmp(command.c_str(), L"internal:newFolder") == 0) {
        if (g_isExecutingInternal.exchange(true)) return;
        struct AutoReset { ~AutoReset() { g_isExecutingInternal = false; } } autoReset;

        std::wstring currentDir = GetActiveFolderPath(rootHwnd);
        if (currentDir.empty()) return;

        std::wstring targetFolderPath = currentDir + L"\\New Folder";
        int counter = 2;
        while (GetFileAttributesW(targetFolderPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            targetFolderPath = currentDir + L"\\New Folder (" + std::to_wstring(counter++) + L")";
        }

        if (CreateDirectoryW(targetFolderPath.c_str(), nullptr)) {
            SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, targetFolderPath.c_str(), nullptr);
            Sleep(80);

            IShellView* psv = GetActiveShellView(rootHwnd);
            if (psv) {
                PIDLIST_ABSOLUTE pidlTarget = nullptr;
                if (SUCCEEDED(SHParseDisplayName(targetFolderPath.c_str(), nullptr, &pidlTarget, 0, nullptr)) && pidlTarget) {
                    PITEMID_CHILD pidlChild = ILFindLastID(pidlTarget);
                    if (pidlChild) {
                        psv->SelectItem(pidlChild, SVSI_SELECT | SVSI_EDIT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE);
                    }
                    CoTaskMemFree(pidlTarget);
                }
                psv->Release();
            }
        }
        return;
    }

    // 4. Open Recycle Bin in Active Tab
    if (_wcsicmp(command.c_str(), L"internal:openRecycleBin") == 0 ||
        _wcsicmp(command.c_str(), L"internal:openRecycleBinTab") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd);
        if (psv) {
            PIDLIST_ABSOLUTE pidlRecycle = nullptr;
            if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0, nullptr, &pidlRecycle)) && pidlRecycle) {
                IServiceProvider* psp = nullptr;
                if (SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&psp)))) {
                    IShellBrowser* psb = nullptr;
                    if (SUCCEEDED(psp->QueryService(kSID_STopLevelBrowser, IID_PPV_ARGS(&psb)))) {
                        psb->BrowseObject(pidlRecycle, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
                        psb->Release();
                    }
                    psp->Release();
                }
                CoTaskMemFree(pidlRecycle);
            }
            psv->Release();
        }
        return;
    }

    // 5. Empty Recycle Bin Safely (Using Native OS Confirmation Prompt)
    if (_wcsicmp(command.c_str(), L"internal:emptyRecycleBin") == 0) {
        SHEmptyRecycleBinW(rootHwnd, nullptr, 0);
        return;
    }

    // 6. Toggle Hidden Files & Force Refresh Active View
    if (_wcsicmp(command.c_str(), L"internal:toggleHiddenFiles") == 0) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            DWORD val = 1, size = sizeof(val);
            if (RegQueryValueExW(hKey, L"Hidden", nullptr, nullptr, (LPBYTE)&val, &size) == ERROR_SUCCESS) {
                val = (val == 1) ? 2 : 1;
                RegSetValueExW(hKey, L"Hidden", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));
            }
            RegCloseKey(hKey);

            SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Policy", SMTO_ABORTIFHUNG, 50, nullptr);
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

            IShellView* psv = GetActiveShellView(rootHwnd);
            if (psv) {
                psv->Refresh();
                psv->Release();
            }
        }
        return;
    }

    // 7. Toggle File Name Extensions & Force Refresh Active View
    if (_wcsicmp(command.c_str(), L"internal:toggleFileExtensions") == 0) {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            DWORD val = 0, size = sizeof(val);
            if (RegQueryValueExW(hKey, L"HideFileExt", nullptr, nullptr, (LPBYTE)&val, &size) == ERROR_SUCCESS) {
                val = (val == 0) ? 1 : 0;
                RegSetValueExW(hKey, L"HideFileExt", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));
            }
            RegCloseKey(hKey);

            SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Policy", SMTO_ABORTIFHUNG, 50, nullptr);
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

            IShellView* psv = GetActiveShellView(rootHwnd);
            if (psv) {
                psv->Refresh();
                psv->Release();
            }
        }
        return;
    }

    Wh_Log(L"Unknown internal command: %s", command.c_str());
}

void RunShortcut(const CustomShortcut& sc, HWND rootHwnd) {
    CreateThread(nullptr, 0, [](LPVOID lpParam) -> DWORD {
        auto* pData = reinterpret_cast<std::pair<CustomShortcut, HWND>*>(lpParam);
        CustomShortcut sc = pData->first;
        HWND rootHwnd = pData->second;
        delete pData;

        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        if (sc.path.rfind(L"internal:", 0) == 0) {
            ExecuteInternalCommand(sc.path, rootHwnd);
            CoUninitialize();
            return 0;
        }

        std::wstring activeDir = GetActiveFolderPath(rootHwnd);
        std::vector<std::wstring> allSelected = GetSelectedPaths(rootHwnd);

        std::vector<std::wstring> filesOnly;
        std::vector<std::wstring> foldersOnly;

        for (const auto& item : allSelected) {
            DWORD attr = GetFileAttributesW(item.c_str());
            if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
                foldersOnly.push_back(item);
            } else {
                filesOnly.push_back(item);
            }
        }

        if (sc.launchMode == L"loop_files") {
            for (const auto& file : filesOnly) {
                std::vector<std::wstring> singleFile = { file };
                std::wstring param = sc.argsPattern;
                ReplaceAll(param, L"%1", L"\"" + file + L"\"");
                ReplaceAll(param, L"%f", L"\"" + file + L"\"");
                ReplaceAll(param, L"%n", GetFileNamesOnly(singleFile, true));
                ReplaceAll(param, L"%c", L"1");
                ReplaceAll(param, L"%ext", GetFileExtension(singleFile));
                ReplaceAll(param, L"%s", file);
                ReplaceAll(param, L"%d", activeDir);
                ExecuteApp(sc.path, param, activeDir, rootHwnd);
            }
            CoUninitialize();
            return 0;
        }

        if (sc.launchMode == L"loop_folders") {
            for (const auto& folder : foldersOnly) {
                std::vector<std::wstring> singleFolder = { folder };
                std::wstring param = sc.argsPattern;
                ReplaceAll(param, L"%1", L"\"" + folder + L"\"");
                ReplaceAll(param, L"%n", GetFileNamesOnly(singleFolder, true));
                ReplaceAll(param, L"%c", L"1");
                ReplaceAll(param, L"%s", folder);
                ReplaceAll(param, L"%d", folder);
                ExecuteApp(sc.path, param, folder, rootHwnd);
            }
            CoUninitialize();
            return 0;
        }

        std::wstring param = sc.argsPattern;
        ReplaceAll(param, L"%f", JoinPaths(allSelected, true));
        ReplaceAll(param, L"%files", JoinPaths(filesOnly, true));
        ReplaceAll(param, L"%folders", JoinPaths(foldersOnly, true));
        ReplaceAll(param, L"%1", allSelected.empty() ? L"" : (L"\"" + allSelected[0] + L"\""));
        ReplaceAll(param, L"%n", GetFileNamesOnly(allSelected, true));
        ReplaceAll(param, L"%c", std::to_wstring(allSelected.size()));
        ReplaceAll(param, L"%ext", GetFileExtension(allSelected));
        ReplaceAll(param, L"%s", JoinPaths(allSelected, false));

        std::wstring smartDir = !foldersOnly.empty() ? foldersOnly[0] : activeDir;

        if (!smartDir.empty() && smartDir.back() == L'\\') smartDir += L'\\';
        if (!activeDir.empty() && activeDir.back() == L'\\') activeDir += L'\\';

        ReplaceAll(param, L"%d_smart", smartDir);
        ReplaceAll(param, L"%d", activeDir);

        WCHAR parentBuf[MAX_PATH];
        wcscpy_s(parentBuf, activeDir.c_str());
        PathRemoveBackslashW(parentBuf);
        PathRemoveFileSpecW(parentBuf);
        ReplaceAll(param, L"%p", parentBuf);

        ExecuteApp(sc.path, param, activeDir, rootHwnd);

        CoUninitialize();
        return 0;
    }, new std::pair<CustomShortcut, HWND>(sc, rootHwnd), 0, nullptr);
}

bool IsInlineEditingActive(HWND rootHwnd) {
    HWND hFocus = GetFocus();
    if (!hFocus) return false;

    HWND hFocusRoot = GetAncestor(hFocus, GA_ROOT);
    if (hFocusRoot && hFocusRoot != rootHwnd) {
        if (GetWindow(hFocusRoot, GW_OWNER) == rootHwnd) {
            return true;
        }
    }

    WCHAR cls[128] = {};
    if (GetClassNameW(hFocus, cls, ARRAYSIZE(cls))) {
        if (wcsstr(cls, L"Edit") || wcsstr(cls, L"TextBox") || wcsstr(cls, L"InputSite")) {
            return true;
        }
    }

    for (HWND h = hFocus; h && h != rootHwnd; h = GetParent(h)) {
        WCHAR parentCls[128] = {};
        if (GetClassNameW(h, parentCls, ARRAYSIZE(parentCls))) {
            if (wcsstr(parentCls, L"Address Band Root") ||
                wcsstr(parentCls, L"ComboBoxEx32") ||
                wcsstr(parentCls, L"SearchEditBoxWrapperClass") ||
                wcsstr(parentCls, L"Windows.UI.Input.InputSite.WindowClass")) {
                return true;
            }
        }
    }

    GUITHREADINFO gti = { sizeof(gti) };
    if (GetGUIThreadInfo(GetWindowThreadProcessId(hFocus, nullptr), &gti)) {
        if (gti.flags & GUI_CARETBLINKING) {
            return true;
        }
    }

    return false;
}

bool ProcessHotKey(HWND hwnd, WPARAM key) {
    HWND rootHwnd = GetAncestor(hwnd, GA_ROOT);
    WCHAR className[256];
    GetClassNameW(rootHwnd, className, ARRAYSIZE(className));
    
    if (wcscmp(className, L"CabinetWClass") == 0 || wcscmp(className, L"ExploreWClass") == 0) {
        bool ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;
        bool alt   = (GetAsyncKeyState(VK_MENU)    & 0x8000) != 0;

        if (key == VK_ESCAPE && !ctrl && !shift && !alt && g_escAction != L"disabled") {
            if (!IsInlineEditingActive(rootHwnd)) {
                if (g_escAction == L"close_tab") {
                    PostMessageW(rootHwnd, WM_CLOSE, 0, 0);
                    return true;
                } else if (g_escAction == L"close_window") {
                    PostMessageW(rootHwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
                    return true;
                }
            }
            return false;
        }

        if (IsInlineEditingActive(rootHwnd)) {
            return false;
        }

        static ULONGLONG s_lastTriggerTime = 0;
        static WPARAM s_lastTriggerKey = 0;

        for (const auto& sc : g_shortcuts) {
            if (sc.vkCode != 0 && key == (WPARAM)sc.vkCode &&
                ctrl == sc.ctrl && shift == sc.shift && alt == sc.alt) {
                
                ULONGLONG now = GetTickCount64();
                if (key == s_lastTriggerKey && (now - s_lastTriggerTime) < 400) {
                    return true;
                }
                s_lastTriggerTime = now;
                s_lastTriggerKey = key;

                RunShortcut(sc, rootHwnd);
                return true;
            }
        }
    }
    return false;
}

using TranslateMessage_t = BOOL (WINAPI*)(const MSG* lpMsg);
TranslateMessage_t TranslateMessage_Original;

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    return TranslateMessage_Original(lpMsg);
}

using TranslateAcceleratorW_t = int (WINAPI*)(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg);
TranslateAcceleratorW_t TranslateAcceleratorW_Original;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg) {
    if (lpMsg && (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN)) {
        if (!(lpMsg->lParam & 0x40000000)) {
            if (ProcessHotKey(lpMsg->hwnd, lpMsg->wParam)) {
                return 1;
            }
        }
    }
    return TranslateAcceleratorW_Original(hWnd, hAccTable, lpMsg);
}

BOOL Wh_ModInit() {
    LoadSettings();

    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "TranslateMessage"),
        (void*)TranslateMessage_Hook,
        (void**)&TranslateMessage_Original
    );
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "TranslateAcceleratorW"),
        (void*)TranslateAcceleratorW_Hook,
        (void**)&TranslateAcceleratorW_Original
    );
    return TRUE;
}

void Wh_ModUninit() {}

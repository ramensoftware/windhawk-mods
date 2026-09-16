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

### Attribution & Acknowledgments
Shell window inspection logic and COM GUID declarations adapt techniques from `explorer-command-bar` (m417z, MIT). Settings toggling follows patterns established in `toggle-hidden-files` (m417z, MIT).
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
#include <functional>
#include <mutex>
#include <string>
#include <algorithm>
#include <atomic>
#include <shellapi.h>
#include <commctrl.h>
#include <memory>

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
static std::mutex g_threadsMutex;
static std::vector<HANDLE> g_threads;
static std::atomic<bool> g_unloading{false};

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
        DWORD threadId = GetWindowThreadProcessId(hExplorerWnd, nullptr);
        GUITHREADINFO gti = { sizeof(gti) };
        HWND hFocus = (GetGUIThreadInfo(threadId, &gti) && gti.hwndFocus) ? gti.hwndFocus : GetFocus();

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

std::wstring GetActiveFolderPath(IShellView* psv) {
    std::wstring result;
    if (!psv) return result;

    IFolderView* pfv = nullptr;
    if (SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&pfv)))) {
        IPersistFolder2* ppf2 = nullptr;
        if (SUCCEEDED(pfv->GetFolder(IID_PPV_ARGS(&ppf2)))) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(ppf2->GetCurFolder(&pidl)) && pidl) {
                std::vector<WCHAR> buffer(UNICODE_STRING_MAX_CHARS);
                if (SHGetPathFromIDListEx(pidl, buffer.data(), static_cast<DWORD>(buffer.size()), GPFIDL_DEFAULT)) {
                    result = buffer.data();
                    if (result.rfind(L"\\\\?\\", 0) == 0) {
                        result = result.substr(4);
                    }
                }
                CoTaskMemFree(pidl);
            }
            ppf2->Release();
        }
        pfv->Release();
    }
    return result;
}

std::vector<std::wstring> GetSelectedPaths(IShellView* psv) {
    std::vector<std::wstring> files;
    if (!psv) return files;

    IDataObject* pdo = nullptr;
    if (SUCCEEDED(psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(&pdo)))) {
        FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM stg = { 0 };
        if (SUCCEEDED(pdo->GetData(&fmt, &stg))) {
            HDROP hDrop = (HDROP)stg.hGlobal;
            UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            for (UINT j = 0; j < fileCount; j++) {
                UINT requiredLen = DragQueryFileW(hDrop, j, nullptr, 0);
                if (requiredLen > 0) {
                    std::vector<WCHAR> pathBuf(requiredLen + 1);
                    if (DragQueryFileW(hDrop, j, pathBuf.data(), requiredLen + 1)) {
                        files.push_back(pathBuf.data());
                    }
                }
            }
            ReleaseStgMedium(&stg);
        }
        pdo->Release();
    }
    return files;
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
        PCWSTR namePtr = PathFindFileNameW(p.c_str());
std::wstring name = namePtr ? namePtr : L"";
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
std::wstring ExpandTokens(
    const std::wstring& pattern,
    const std::wstring& activeDir,
    const std::vector<std::wstring>& allItems,
    const std::vector<std::wstring>& filesOnly,
    const std::vector<std::wstring>& foldersOnly
) {
    std::wstring result;
    result.reserve(pattern.size() * 2);

    for (size_t i = 0; i < pattern.size(); ) {
        if (pattern[i] != L'%') {
            result.push_back(pattern[i++]);
            continue;
        }

        if (pattern.compare(i, 6, L"%files") == 0) {
            result += JoinPaths(filesOnly, true);
            i += 6;
        } else if (pattern.compare(i, 8, L"%folders") == 0) {
            result += JoinPaths(foldersOnly, true);
            i += 8;
        } else if (pattern.compare(i, 8, L"%d_smart") == 0) {
            std::wstring smartDir = !foldersOnly.empty() ? foldersOnly[0] : activeDir;
            if (!smartDir.empty() && smartDir.back() == L'\\') smartDir += L'\\';
            result += smartDir;
            i += 8;
        } else if (pattern.compare(i, 4, L"%ext") == 0) {
            result += GetFileExtension(allItems);
            i += 4;
        } else if (pattern.compare(i, 2, L"%f") == 0) {
            result += JoinPaths(allItems, true);
            i += 2;
        } else if (pattern.compare(i, 2, L"%1") == 0) {
            result += allItems.empty() ? L"" : (L"\"" + allItems[0] + L"\"");
            i += 2;
        } else if (pattern.compare(i, 2, L"%n") == 0) {
            result += GetFileNamesOnly(allItems, true);
            i += 2;
        } else if (pattern.compare(i, 2, L"%c") == 0) {
            result += std::to_wstring(allItems.size());
            i += 2;
        } else if (pattern.compare(i, 2, L"%s") == 0) {
            result += JoinPaths(allItems, false);
            i += 2;
        } else if (pattern.compare(i, 2, L"%d") == 0) {
            std::wstring activeWithTrailing = activeDir;
            if (!activeWithTrailing.empty() && activeWithTrailing.back() == L'\\') activeWithTrailing += L'\\';
            result += activeWithTrailing;
            i += 2;
        } else if (pattern.compare(i, 2, L"%p") == 0) {
            std::wstring parent = activeDir;
            while (!parent.empty() && parent.back() == L'\\') parent.pop_back();
            size_t slash = parent.find_last_of(L'\\');
            result += (slash == std::wstring::npos) ? std::wstring() : parent.substr(0, slash);
            i += 2;
        } else {
            result.push_back(pattern[i++]);
        }
    }
    return result;
}

void ExecuteApp(const std::wstring& cmd, const std::wstring& params, const std::wstring& workDir) {
    std::wstring targetPath = ResolveCommandPath(cmd);

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.hwnd = nullptr;
    sei.lpVerb = L"open";
    sei.lpFile = targetPath.c_str();
    sei.lpParameters = params.empty() ? nullptr : params.c_str();
    sei.lpDirectory = workDir.empty() ? nullptr : workDir.c_str();
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"ShellExecuteExW failed for %s (error %lu)", targetPath.c_str(), GetLastError());
    }
}

void QueueBackgroundWork(std::function<void()> task) {
    auto* fnPtr = new std::function<void()>(std::move(task));

    HANDLE hThread = CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        std::unique_ptr<std::function<void()>> fn(reinterpret_cast<std::function<void()>*>(param));
        
        (*fn)();

        CoUninitialize();
        return 0;
    }, fnPtr, 0, nullptr);

    if (hThread) {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        auto it = g_threads.begin();
        while (it != g_threads.end()) {
            if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
                CloseHandle(*it);
                it = g_threads.erase(it);
            } else {
                ++it;
            }
        }
        g_threads.push_back(hThread);
    } else {
        delete fnPtr;
    }
}

void ExecuteInternalCommand(const std::wstring& command, HWND rootHwnd) {
// 1. Open Folder Options (Async via background worker)
if (_wcsicmp(command.c_str(), L"internal:folderOptions") == 0) {
    QueueBackgroundWork([]() {
        ExecuteApp(L"rundll32.exe", L"shell32.dll,Options_RunDLL 0", L"");
    });
    return;
}


    // 2. Create New Text Document & Focus/Select
    if (_wcsicmp(command.c_str(), L"internal:newTextFile") == 0) {
        if (g_isExecutingInternal.exchange(true)) return;
        struct AutoReset { ~AutoReset() { g_isExecutingInternal = false; } } autoReset;

        IShellView* psv = GetActiveShellView(rootHwnd);
        if (!psv) return;

        std::wstring currentDir = GetActiveFolderPath(psv);
        if (currentDir.empty()) {
            psv->Release();
            return;
        }

        std::wstring targetFilePath = currentDir + L"\\New Text Document.txt";
        int counter = 2;
        while (GetFileAttributesW(targetFilePath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            targetFilePath = currentDir + L"\\New Text Document (" + std::to_wstring(counter++) + L").txt";
        }

        HANDLE hFile = CreateFileW(targetFilePath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW, targetFilePath.c_str(), nullptr);

            PIDLIST_ABSOLUTE pidlTarget = nullptr;
            if (SUCCEEDED(SHParseDisplayName(targetFilePath.c_str(), nullptr, &pidlTarget, 0, nullptr)) && pidlTarget) {
                PITEMID_CHILD pidlChild = ILFindLastID(pidlTarget);
                if (pidlChild) {
                    psv->SelectItem(pidlChild, SVSI_SELECT | SVSI_FOCUSED | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE);
                }
                CoTaskMemFree(pidlTarget);
            }
        }
        psv->Release();
        return;
    }

    // 3. Create New Folder & Enter Inline Rename Mode
    if (_wcsicmp(command.c_str(), L"internal:newFolder") == 0) {
        if (g_isExecutingInternal.exchange(true)) return;
        struct AutoReset { ~AutoReset() { g_isExecutingInternal = false; } } autoReset;

        IShellView* psv = GetActiveShellView(rootHwnd);
        if (!psv) return;

        std::wstring currentDir = GetActiveFolderPath(psv);
        if (currentDir.empty()) {
            psv->Release();
            return;
        }

        std::wstring targetFolderPath = currentDir + L"\\New Folder";
        int counter = 2;
        while (GetFileAttributesW(targetFolderPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
            targetFolderPath = currentDir + L"\\New Folder (" + std::to_wstring(counter++) + L")";
        }

        if (CreateDirectoryW(targetFolderPath.c_str(), nullptr)) {
            SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, targetFolderPath.c_str(), nullptr);

            PIDLIST_ABSOLUTE pidlTarget = nullptr;
            if (SUCCEEDED(SHParseDisplayName(targetFolderPath.c_str(), nullptr, &pidlTarget, 0, nullptr)) && pidlTarget) {
                PITEMID_CHILD pidlChild = ILFindLastID(pidlTarget);
                if (pidlChild) {
                    psv->SelectItem(pidlChild, SVSI_SELECT | SVSI_EDIT | SVSI_DESELECTOTHERS | SVSI_ENSUREVISIBLE);
                }
                CoTaskMemFree(pidlTarget);
            }
        }
        psv->Release();
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
    QueueBackgroundWork([rootHwnd]() {
        SHEmptyRecycleBinW(rootHwnd, nullptr, 0);
    });
    return;
}

    // 6. Toggle Hidden Files & Force Refresh Active View
    if (_wcsicmp(command.c_str(), L"internal:toggleHiddenFiles") == 0) {
    SHELLSTATE ss{};
    SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS, FALSE);
    ss.fShowAllObjects = !ss.fShowAllObjects;
    SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS, TRUE);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return;
}

    // 7. Toggle File Name Extensions & Force Refresh Active View
    if (_wcsicmp(command.c_str(), L"internal:toggleFileExtensions") == 0) {
    SHELLSTATE ss{};
    SHGetSetSettings(&ss, SSF_SHOWEXTENSIONS, FALSE);
    ss.fShowExtensions = !ss.fShowExtensions;
    SHGetSetSettings(&ss, SSF_SHOWEXTENSIONS, TRUE);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return;
}

    Wh_Log(L"Unknown internal command: %s", command.c_str());
}

struct LaunchTask {
    std::wstring path;
    std::vector<std::wstring> commands;
    std::wstring workDir;
};

void RunShortcut(const CustomShortcut& sc, HWND rootHwnd) {
    // 1. Internal commands execute immediately on UI thread
    if (sc.path.rfind(L"internal:", 0) == 0) {
        ExecuteInternalCommand(sc.path, rootHwnd);
        return;
    }

    // 2. Query COM/Explorer state ONCE on the UI thread
    IShellView* psv = GetActiveShellView(rootHwnd);
    std::wstring activeDir = GetActiveFolderPath(psv);
    std::vector<std::wstring> allSelected = GetSelectedPaths(psv);
    if (psv) {
        psv->Release();
    }

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

    // 3. Build launch task parameters using single-pass ExpandTokens
    auto* task = new LaunchTask();
    task->path = sc.path;
    task->workDir = activeDir;

    if (sc.launchMode == L"loop_files") {
        for (const auto& file : filesOnly) {
            std::vector<std::wstring> cur = { file };
            task->commands.push_back(ExpandTokens(sc.argsPattern, activeDir, cur, cur, {}));
        }
    } else if (sc.launchMode == L"loop_folders") {
        for (const auto& folder : foldersOnly) {
            std::vector<std::wstring> cur = { folder };
            task->commands.push_back(ExpandTokens(sc.argsPattern, folder, cur, {}, cur));
        }
    } else {
        task->commands.push_back(ExpandTokens(sc.argsPattern, activeDir, allSelected, filesOnly, foldersOnly));
    }

    // 4. Launch safely in worker thread with COM init and unload tracking
    HANDLE hThread = CreateThread(nullptr, 0, [](LPVOID param) -> DWORD {
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        std::unique_ptr<LaunchTask> t(reinterpret_cast<LaunchTask*>(param));

        for (const auto& cmdArgs : t->commands) {
            if (g_unloading.load()) {
                break;
            }
            ExecuteApp(t->path, cmdArgs, t->workDir);
        }

        CoUninitialize();
        return 0;
    }, task, 0, nullptr);

    if (hThread) {
        std::lock_guard<std::mutex> lock(g_threadsMutex);

        // Lazily reap handles of threads that finished executing
        auto it = g_threads.begin();
        while (it != g_threads.end()) {
            if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
                CloseHandle(*it);
                it = g_threads.erase(it);
            } else {
                ++it;
            }
        }

        // Store active handle so Wh_ModUninit can join safely
        g_threads.push_back(hThread);
    } else {
        delete task;
    }
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
    if (!hwnd) return false;

    HWND rootHwnd = GetAncestor(hwnd, GA_ROOT);
    if (!rootHwnd) return false;

    WCHAR className[256] = {};
    if (!GetClassNameW(rootHwnd, className, ARRAYSIZE(className))) {
        return false;
    }
    
    if (wcscmp(className, L"CabinetWClass") == 0 || wcscmp(className, L"ExploreWClass") == 0) {
        bool ctrl  = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetAsyncKeyState(VK_SHIFT)   & 0x8000) != 0;
        bool alt   = (GetAsyncKeyState(VK_MENU)    & 0x8000) != 0;

        if (key == VK_ESCAPE && !ctrl && !shift && !alt && g_escAction != L"disabled") {
            if (!IsInlineEditingActive(rootHwnd)) {
                if (g_escAction == L"close_tab") {
                    // Synthesize Ctrl+W to close active tab safely without closing entire window
                    INPUT inputs[4] = {};
                    inputs[0].type = INPUT_KEYBOARD;
                    inputs[0].ki.wVk = VK_CONTROL;

                    inputs[1].type = INPUT_KEYBOARD;
                    inputs[1].ki.wVk = 'W';

                    inputs[2].type = INPUT_KEYBOARD;
                    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
                    inputs[2].ki.wVk = 'W';

                    inputs[3].type = INPUT_KEYBOARD;
                    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
                    inputs[3].ki.wVk = VK_CONTROL;

                    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
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

        // thread_local prevents multi-window race conditions across Explorer UI threads
        thread_local ULONGLONG s_lastTriggerTime = 0;
        thread_local WPARAM s_lastTriggerKey = 0;

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
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "TranslateAcceleratorW"),
        (void*)TranslateAcceleratorW_Hook,
        (void**)&TranslateAcceleratorW_Original
    );
    return TRUE;
}

void Wh_ModUninit() {
    g_unloading = true;
    std::vector<HANDLE> threadsToJoin;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        threadsToJoin.swap(g_threads);
    }
    for (HANDLE h : threadsToJoin) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }
}

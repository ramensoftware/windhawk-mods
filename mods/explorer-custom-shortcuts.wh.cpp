// ==WindhawkMod==
// @id              explorer-custom-shortcuts
// @name            Explorer Custom Shortcuts
// @description     Adds app-style keyboard shortcuts to File Explorer with dynamic tokens, selection modes, and internal commands.
// @version         1.4.0
// @author          ArvindSaini978
// @github          https://github.com/ArvindSaini978
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lole32 -loleaut32 -lshlwapi -lgdiplus -lgdi32
// @architecture    x86-64
// ==/WindhawkMod==

// clang-format off

// ==WindhawkModReadme==
/*
# Explorer Custom Shortcuts

Adds customizable, app-style keyboard shortcuts to Windows File Explorer with parameter substitution, built-in shell commands, and custom token expansion.


## Key Features

* **Custom Keyboard Shortcuts** — Assign shortcuts using letters, numbers, function keys (`F1–F12`), or navigation keys (`Delete`, `Backspace`, `Space`, `Enter`, etc.) combined with `Ctrl`, `Shift`, or `Alt`.
* **Smart Explorer Arguments** — Pass active folder paths, selected file names, extensions, or file lists directly into your custom apps or scripts.
* **Flexible Run Modes** — Run an application once with all selected items, or launch it individually for each selected file or folder.
* **Built-in Shell Actions** — Quick built-in commands to create text files or folders, copy paths/names to clipboard, navigate to parent folders or the Recycle Bin, and toggle hidden files or extensions.
* **On-Screen Action Popups** — Shows a clean, brief notification on your screen whenever background actions finish (like copying a path or emptying the Recycle Bin). Automatically matches your Windows accent color and Dark/Light theme, and can be disabled in settings.
* **Typing & Rename Protection** — Shortcuts automatically pause while you are renaming a file, typing in the address bar, searching, or interacting with dialogs so they never interfere with your typing.


> **Windows 10 Ribbon Note:** Default shortcuts using `Alt` (such as `Alt+H` or `Alt+C`) take precedence over Windows 10 Explorer ribbon access keys. You can remap or disable these bindings in the mod settings if you rely on ribbon mnemonics.

---

### Internal Explorer Commands

Instead of specifying an executable path, set **Executable Path** to one of the following `internal:` keywords:

* **`internal:newTextFile`**: Creates a `New Text Document.txt` in the active folder and automatically selects/focuses it without UI freezes.
* **`internal:newFolder`**: Creates a `New Folder` in the active folder and enters inline rename mode immediately.
* **`internal:openParentFolder`**: Navigates the current active tab up one level to its parent directory.
* **`internal:openRecycleBin`**: Navigates to the Recycle Bin in the current active tab.
* **`internal:emptyRecycleBin`**: Empties the Recycle Bin with confirmation dialog.
* **`internal:toggleHiddenFiles`**: Toggles visibility of hidden files and folders with immediate view refresh. *(Note: Changes persistent system-wide Windows Explorer settings).*
* **`internal:toggleFileExtensions`**: Toggles file name extensions on or off with immediate view refresh. *(Note: Changes persistent system-wide Windows Explorer settings).*
* **`internal:openWith`**: Opens the native Windows "How do you want to open this file?" dialog for the selected file.
* **`internal:folderOptions`**: Opens the native File Explorer Folder Options dialog.
* **`internal:copyName`**: Copies the file name(s) with extension without quotes, separated by newlines.
* **`internal:copyPath`**: Copies the absolute path(s) of selected item(s) without quotes, separated by newlines. If no items are selected, it automatically copies the current open folder's path. *(Note: Applies only to physical file-system locations, not virtual shell folders like This PC or Recycle Bin).*

> **Persistent Settings Notice:** `internal:toggleHiddenFiles` and `internal:toggleFileExtensions` flip the native Windows Explorer shell settings directly (`SHGetSetSettings`). These changes affect all File Explorer surfaces globally and persist even if this mod is disabled or uninstalled.

---

### Available Tokens for Arguments & Examples

* **`%f`** — All selected items as space-separated quoted paths (`"C:\a.txt" "C:\b.png"`).
* **`%files`** — Selected regular files only (skips selected folders).
* **`%folders`** — Selected folders only (skips selected regular files).
* **`%1`** — Quoted path of the first selected file or directory (`"C:\a.txt"`).
* **`%n`** — File or folder names only without absolute directory paths (`"a.txt"`).
* **`%c`** — Total number of selected items as an integer (`3`). In loop modes, this resolves to `1`.
* **`%ext`** — File extension of the first selected item (`.png`).
* **`%s`** — Space-separated paths with **smart-quoting** (automatically adds double-quotes only to paths containing spaces).
* **`%d`** — Active directory open in the current tab (`C:\Users\Name\Documents`). Virtual folders like *This PC* or *Recycle Bin* resolve to an empty string.
* **`%d_smart`** — Selected folder if one is highlighted; otherwise falls back to the current active directory.
* **`%p`** — Parent directory path of the active tab.

> **Shell Interpreter Security Notice:** Path tokens are quoted according to standard Windows CRT command-line (`argv`) rules. If arguments are passed to script interpreters (e.g. `cmd.exe /c` or `powershell.exe -Command`), parameters may be re-parsed by that interpreter. Use native binary arguments or pass paths directly to target programs.

---

### Launch Modes & Use Cases

* **`batch`** (Default): Executes the application exactly once and passes the resolved command-line arguments.
  * *Use Case:* Code editors (opening a directory), Media players (queuing files), or bulk tools like PowerRename.
* **`loop_files`**: Executes the application once per selected file.
  * *Use Case:* Single-file image editors, batch converters, or per-file processing scripts.
* **`loop_folders`**: Executes the application once per selected folder.
  * *Use Case:* Batch archiving folders individually or folder-specific terminal operations.

---

### Preset Ideas & External App Examples

You can add your own shortcuts using these templates in the settings:

* **Open Windows Terminal Here**
  * Path: `wt.exe` | Args: `-d "%d"` | Mode: `batch`
* **Open with Notepad**
  * Path: `notepad.exe` | Args: `%1` | Mode: `loop_files`
* **PowerToys PowerRename** (Bulk rename selected items)
  * Path: `C:\Program Files\PowerToys\WinUI3Apps\PowerToys.PowerRename.exe` | Args: `%f` | Mode: `batch`
* **Search with Everything** (Search current active directory)
  * Path: `C:\Program Files\Everything\Everything.exe` | Args: `"%d"` | Mode: `batch`
* **Open in VS Code**
  * Path: `code` | Args: `"%d_smart"` | Mode: `batch`
* **Queue in VLC Media Player**
  * Path: `vlc.exe` | Args: `%f` | Mode: `batch`

---

### Comparison with Existing Mods

While related mods exist in the Windhawk repository, **Explorer Custom Shortcuts** provides a distinct, keyboard-first workflow engine:
* **`explorer-command-bar`**: Adds visual custom toolbar buttons exclusively to the modern Windows 11 command bar. In contrast, this mod provides a pure, zero-UI, low-latency keyboard shortcut accelerator engine operating across both classic and tabbed Explorer windows. It introduces specialized batch looping capabilities (`loop_files`, `loop_folders`), rich token expansions (`%n`, `%ext`, `%c`, `%files`, `%folders`), and context-aware suppression during inline edits.
* **`keyboard-shortcut-actions`**: A general-purpose desktop/window hotkey dispatcher without Explorer context awareness. This mod parses active Explorer tab navigation, item selections, and folder paths directly into CLI parameters.
* **`toggle-hidden-files` / `explorer-ctrln-newfile` / `explorer-ctrlq-new-folder`**: Single-purpose mods; here, internal shell actions are optional presets within a single hotkey table, allowing users to bind them to whatever key combinations they prefer without installing multiple separate hooks.

### Attribution & Acknowledgments
Process execution concepts (`ResolveCommandPath`, `ExecuteApp`) and shell window inspection adapt techniques from `explorer-command-bar` (DanRotaru, MIT). Settings toggling follows patterns established in `toggle-hidden-files` (Asteski).
*/
// ==/WindhawkModReadme==

// clang-format on

// clang-format off

// ==WindhawkModSettings==
/*
- shortcuts:
  - - name: "Open Terminal Here"
      $name: "Action Name"
    - enabled: true
      $name: "Enabled"
    - key: "T"
      $name: "Key (A-Z, 0-9, F1-F12, Backspace/Back, Delete/Del, Insert/Ins, Home, End, PageUp/PgUp, PageDown/PgDn, Space, Enter/Return, Tab)"
    - ctrl: true
      $name: "Require Ctrl"
    - shift: false
      $name: "Require Shift"
    - alt: true
      $name: "Require Alt"
    - path: "wt.exe"
      $name: "Executable Path or Internal Command"
      $description: "Executable path, alias, or internal command. Note: internal:toggleHiddenFiles and internal:toggleFileExtensions flip persistent system-wide Windows settings."
    - args: "-d \"%d\""
      $name: "Arguments / Tokens"
      $description: "Supported tokens: %f, %files, %folders, %1, %n, %c, %ext, %s, %d, %d_smart, %p."
    - mode: "batch"
      $name: "Launch Mode"
      $options:
        - batch: "batch: Run once with all selected items"
        - loop_files: "loop_files: Run once per selected file"
        - loop_folders: "loop_folders: Run once per selected folder"
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
    - enabled: false
    - key: "H"
    - ctrl: true
    - shift: false
    - alt: false
    - path: "internal:toggleHiddenFiles"
    - args: ""
    - mode: "batch"
  - - name: "Toggle File Extensions"
    - enabled: false
    - key: "E"
    - ctrl: true
    - shift: false
    - alt: true
    - path: "internal:toggleFileExtensions"
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
  - - name: "Copy File Path"
    - enabled: false
    - key: "C"
    - ctrl: true
    - shift: true
    - alt: false
    - path: "internal:copyPath"
    - args: ""
    - mode: "batch"
  - - name: "Copy File Name"
    - enabled: true
    - key: "C"
    - ctrl: false
    - shift: false
    - alt: true
    - path: "internal:copyName"
    - args: ""
    - mode: "batch"
  - - name: "Open With"
    - enabled: true
    - key: "H"
    - ctrl: false
    - shift: false
    - alt: true
    - path: "internal:openWith"
    - args: ""
    - mode: "batch"
  $name: "Custom Shortcuts"
  $description: "List of customizable shortcuts. Supported keys: A-Z, 0-9, F1-F12, Backspace/Back, Delete/Del, Insert/Ins, Home, End, PageUp/PgUp, PageDown/PgDn, Space, Enter/Return, and Tab. Modifiers (Ctrl, Shift, or Alt) are required for all keys except F1-F12."
- showActionToasts: true
  $name: "Show Action Notifications"
  $description: "Displays a brief floating popup notification for completed actions (e.g. copied paths/names, emptied Recycle Bin)."
*/
// ==/WindhawkModSettings==

// clang-format on

// clang-format off

#include <windows.h>
#include <initguid.h>
#include <shlobj.h>
#include <exdisp.h>
#include <shlguid.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <gdiplus.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <vector>

#include <windhawk_utils.h>

// clang-format on

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

static std::vector<CustomShortcut> g_shortcuts;
static std::shared_mutex g_shortcutsMutex;

static std::mutex g_threadsMutex;
static std::vector<HANDLE> g_threads;
static std::atomic<bool> g_unloading{false};
static std::atomic<bool> g_showActionToasts{true};
static ULONG_PTR g_gdiplusToken = 0;

std::wstring ExpandEnv(const std::wstring& input) {
    DWORD required = ExpandEnvironmentStringsW(input.c_str(), nullptr, 0);
    if (required > 0) {
        std::vector<WCHAR> buf(required);
        if (ExpandEnvironmentStringsW(input.c_str(), buf.data(), required) >
            0) {
            return buf.data();
        }
    }
    return input;
}

std::wstring ResolveCommandPath(const std::wstring& command) {
    std::wstring expanded = ExpandEnv(command);
    if (expanded.find(L'\\') != std::wstring::npos) {
        return expanded;
    }

    WCHAR resolved[MAX_PATH];
    DWORD searchRes = SearchPathW(nullptr, expanded.c_str(), L".exe",
                                  ARRAYSIZE(resolved), resolved, nullptr);
    if (searchRes > 0 && searchRes < ARRAYSIZE(resolved)) {
        return resolved;
    }

    std::wstring keyPath =
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\" + expanded;
    if (keyPath.size() < 4 ||
        _wcsicmp(keyPath.c_str() + keyPath.size() - 4, L".exe") != 0) {
        keyPath += L".exe";
    }

    for (HKEY rootKey : {HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE}) {
        WCHAR buffer[MAX_PATH];
        DWORD size = sizeof(buffer);
        if (RegGetValueW(rootKey, keyPath.c_str(), nullptr, RRF_RT_REG_SZ,
                         nullptr, buffer, &size) == ERROR_SUCCESS &&
            buffer[0]) {
            std::wstring appPath = ExpandEnv(buffer);
            if (appPath.size() >= 2 && appPath.front() == L'"' &&
                appPath.back() == L'"') {
                appPath = appPath.substr(1, appPath.size() - 2);
            }
            return appPath;
        }
    }
    return expanded;
}

int ParseKey(std::wstring keyStr) {
    // Trim leading whitespace and quotes
    auto isTrimChar = [](wchar_t ch) {
        return iswspace(ch) || ch == L'"' || ch == L'\'';
    };
    keyStr.erase(keyStr.begin(),
                 std::find_if(keyStr.begin(), keyStr.end(),
                              [&](wchar_t ch) { return !isTrimChar(ch); }));
    // Trim trailing whitespace and quotes
    keyStr.erase(std::find_if(keyStr.rbegin(), keyStr.rend(),
                              [&](wchar_t ch) { return !isTrimChar(ch); })
                     .base(),
                 keyStr.end());
    // Convert to uppercase
    std::transform(keyStr.begin(), keyStr.end(), keyStr.begin(), ::towupper);
    if (keyStr.empty())
        return 0;

    // F1 - F12
    if (keyStr.length() > 1 && keyStr[0] == L'F') {
        try {
            int fNum = std::stoi(keyStr.substr(1));
            if (fNum >= 1 && fNum <= 12) {
                return VK_F1 + (fNum - 1);
            }
        } catch (...) {
        }
        return 0;
    }

    // Special and Navigation Keys
    if (_wcsicmp(keyStr.c_str(), L"DELETE") == 0 ||
        _wcsicmp(keyStr.c_str(), L"DEL") == 0)
        return VK_DELETE;
    if (_wcsicmp(keyStr.c_str(), L"BACKSPACE") == 0 ||
        _wcsicmp(keyStr.c_str(), L"BACK") == 0)
        return VK_BACK;
    if (_wcsicmp(keyStr.c_str(), L"SPACE") == 0)
        return VK_SPACE;
    if (_wcsicmp(keyStr.c_str(), L"ENTER") == 0 ||
        _wcsicmp(keyStr.c_str(), L"RETURN") == 0)
        return VK_RETURN;
    if (_wcsicmp(keyStr.c_str(), L"TAB") == 0)
        return VK_TAB;
    if (_wcsicmp(keyStr.c_str(), L"INSERT") == 0 ||
        _wcsicmp(keyStr.c_str(), L"INS") == 0)
        return VK_INSERT;
    if (_wcsicmp(keyStr.c_str(), L"HOME") == 0)
        return VK_HOME;
    if (_wcsicmp(keyStr.c_str(), L"END") == 0)
        return VK_END;
    if (_wcsicmp(keyStr.c_str(), L"PAGEUP") == 0 ||
        _wcsicmp(keyStr.c_str(), L"PGUP") == 0)
        return VK_PRIOR;
    if (_wcsicmp(keyStr.c_str(), L"PAGEDOWN") == 0 ||
        _wcsicmp(keyStr.c_str(), L"PGDN") == 0)
        return VK_NEXT;

    // Single Alphanumeric (A-Z, 0-9)
    if (keyStr.length() == 1) {
        wchar_t c = keyStr[0];
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            return c;
        }
    }

    return 0;
}

void LoadSettings() {
    Wh_Log(L"Loading mod settings...");
    g_showActionToasts = Wh_GetIntSetting(L"showActionToasts", 1) != 0;

    std::vector<CustomShortcut> newShortcuts;

    for (int i = 0; i < 100; i++) {
        WindhawkUtils::StringSetting pathStr =
            WindhawkUtils::StringSetting::make(L"shortcuts[%d].path", i);
        if (!*pathStr.get()) {
            break;
        }

        CustomShortcut sc;
        sc.path = pathStr.get();
        sc.enabled = Wh_GetIntSetting(L"shortcuts[%d].enabled", i) != 0;

        WindhawkUtils::StringSetting nameStr =
            WindhawkUtils::StringSetting::make(L"shortcuts[%d].name", i);
        sc.name = nameStr.get();

        WindhawkUtils::StringSetting keyStr =
            WindhawkUtils::StringSetting::make(L"shortcuts[%d].key", i);
        sc.vkCode = ParseKey(keyStr.get());

        sc.ctrl = Wh_GetIntSetting(L"shortcuts[%d].ctrl", i) != 0;
        sc.shift = Wh_GetIntSetting(L"shortcuts[%d].shift", i) != 0;
        sc.alt = Wh_GetIntSetting(L"shortcuts[%d].alt", i) != 0;

        WindhawkUtils::StringSetting argsStr =
            WindhawkUtils::StringSetting::make(L"shortcuts[%d].args", i);
        sc.argsPattern = argsStr.get();

        WindhawkUtils::StringSetting modeStr =
            WindhawkUtils::StringSetting::make(L"shortcuts[%d].mode", i);
        sc.launchMode = modeStr.get();
        if (sc.launchMode.empty())
            sc.launchMode = L"batch";

        bool isFunctionKey = (sc.vkCode >= VK_F1 && sc.vkCode <= VK_F12);
        bool hasModifier = (sc.ctrl || sc.shift || sc.alt);

        if (sc.enabled && sc.vkCode != 0 && (hasModifier || isFunctionKey)) {
            newShortcuts.push_back(sc);
        } else if (sc.enabled && sc.vkCode != 0 && !hasModifier &&
                   !isFunctionKey) {
            Wh_Log(
                L"Shortcut '%s' requires at least one modifier key (Ctrl, "
                L"Shift, Alt). Dropped.",
                sc.name.c_str());
        }
    }

    {
        std::unique_lock<std::shared_mutex> lock(g_shortcutsMutex);
        g_shortcuts = std::move(newShortcuts);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Mod settings changed, reloading...");
    LoadSettings();
}

void QueueBackgroundWork(std::function<void()> task) {
    auto* fnPtr = new std::function<void()>(std::move(task));

    HANDLE hThread = CreateThread(
        nullptr, 0,
        [](LPVOID param) -> DWORD {
            CoInitializeEx(nullptr,
                           COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            std::unique_ptr<std::function<void()>> fn(
                reinterpret_cast<std::function<void()>*>(param));

            (*fn)();

            CoUninitialize();
            return 0;
        },
        fnPtr, 0, nullptr);

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

// Safely quote a path, doubling any trailing backslash (e.g. C:\ -> "C:\\") to
// prevent escaping the closing quote
std::wstring SafeQuote(const std::wstring& p) {
    std::wstring res = L"\"" + p;
    if (!p.empty() && p.back() == L'\\') {
        res += L'\\';
    }
    res += L'\"';
    return res;
}

std::wstring JoinPaths(const std::vector<std::wstring>& paths,
                       bool forceQuotes = true) {
    std::wstring res;
    for (const auto& p : paths) {
        if (!res.empty())
            res += L" ";
        if (forceQuotes || p.find(L' ') != std::wstring::npos) {
            res += SafeQuote(p);
        } else {
            res += p;
        }
    }
    return res;
}

std::wstring GetFileNamesOnly(const std::vector<std::wstring>& paths,
                              bool forceQuotes = true) {
    std::wstring res;
    for (const auto& p : paths) {
        if (!res.empty())
            res += L" ";
        PCWSTR namePtr = PathFindFileNameW(p.c_str());
        std::wstring name = namePtr ? namePtr : L"";
        if (forceQuotes || name.find(L' ') != std::wstring::npos) {
            res += SafeQuote(name);
        } else {
            res += name;
        }
    }
    return res;
}

std::wstring GetFileExtension(const std::vector<std::wstring>& paths) {
    if (paths.empty())
        return L"";
    PCWSTR ext = PathFindExtensionW(paths[0].c_str());
    return ext ? ext : L"";
}

IShellView* GetActiveShellView(HWND hExplorerWnd, HWND hCapturedFocus) {
    IShellWindows* pShellWindows = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&pShellWindows))) ||
        !pShellWindows) {
        return nullptr;
    }

    long count = 0;
    pShellWindows->get_Count(&count);

    HWND hActiveTabWnd = nullptr;
    if (hExplorerWnd) {
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
                wcscmp(cls, L"ShellTabWindowClass") == 0) {
                hActiveTabWnd = h;
                break;
            }
        }
        if (!hActiveTabWnd) {
            hActiveTabWnd = FindWindowExW(hExplorerWnd, nullptr,
                                          L"ShellTabWindowClass", nullptr);
        }
    }

    for (long i = 0; i < count; i++) {
        VARIANT index;
        VariantInit(&index);
        index.vt = VT_I4;
        index.lVal = i;

        IDispatch* pDispatch = nullptr;
        if (FAILED(pShellWindows->Item(index, &pDispatch)) || !pDispatch)
            continue;

        IWebBrowser2* pWebBrowser = nullptr;
        if (SUCCEEDED(pDispatch->QueryInterface(IID_PPV_ARGS(&pWebBrowser)))) {
            SHANDLE_PTR hWndRaw = 0;
            if (SUCCEEDED(pWebBrowser->get_HWND(&hWndRaw)) &&
                (!hExplorerWnd || (HWND)hWndRaw == hExplorerWnd)) {
                IServiceProvider* pServiceProvider = nullptr;
                if (SUCCEEDED(pDispatch->QueryInterface(
                        IID_PPV_ARGS(&pServiceProvider)))) {
                    IShellBrowser* pShellBrowser = nullptr;
                    if (SUCCEEDED(pServiceProvider->QueryService(
                            SID_STopLevelBrowser,
                            IID_PPV_ARGS(&pShellBrowser)))) {
                        HWND hTabWnd = nullptr;
                        if (SUCCEEDED(pShellBrowser->GetWindow(&hTabWnd)) &&
                            hTabWnd) {
                            if (hActiveTabWnd && hTabWnd != hActiveTabWnd) {
                                pShellBrowser->Release();
                                pServiceProvider->Release();
                                pWebBrowser->Release();
                                pDispatch->Release();
                                continue;
                            }
                        }

                        IShellView* pShellView = nullptr;
                        if (SUCCEEDED(pShellBrowser->QueryActiveShellView(
                                &pShellView)) &&
                            pShellView) {
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
    if (!psv)
        return result;

    IFolderView* pfv = nullptr;
    if (SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&pfv)))) {
        IPersistFolder2* ppf2 = nullptr;
        if (SUCCEEDED(pfv->GetFolder(IID_PPV_ARGS(&ppf2)))) {
            PIDLIST_ABSOLUTE pidl = nullptr;
            if (SUCCEEDED(ppf2->GetCurFolder(&pidl)) && pidl) {
                std::vector<WCHAR> buffer(UNICODE_STRING_MAX_CHARS);
                if (SHGetPathFromIDListEx(pidl, buffer.data(),
                                          static_cast<DWORD>(buffer.size()),
                                          GPFIDL_DEFAULT)) {
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
    if (!psv)
        return files;

    IDataObject* pdo = nullptr;
    if (SUCCEEDED(psv->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(&pdo)))) {
        FORMATETC fmt = {CF_HDROP, nullptr, DVASPECT_CONTENT, -1,
                         TYMED_HGLOBAL};
        STGMEDIUM stg = {0};
        if (SUCCEEDED(pdo->GetData(&fmt, &stg))) {
            HDROP hDrop = (HDROP)stg.hGlobal;
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
        pdo->Release();
    }
    return files;
}

std::wstring ExpandTokens(const std::wstring& pattern,
                          const std::wstring& activeDir,
                          const std::vector<std::wstring>& allItems,
                          const std::vector<std::wstring>& filesOnly,
                          const std::vector<std::wstring>& foldersOnly) {
    std::wstring result;
    result.reserve(pattern.size() * 2);

    for (size_t i = 0; i < pattern.size();) {
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
            std::wstring smartDir =
                !foldersOnly.empty() ? foldersOnly[0] : activeDir;
            if (!smartDir.empty() && smartDir.back() == L'\\')
                smartDir += L'\\';
            result += smartDir;
            i += 8;
        } else if (pattern.compare(i, 4, L"%ext") == 0) {
            result += GetFileExtension(allItems);
            i += 4;
        } else if (pattern.compare(i, 2, L"%f") == 0) {
            result += JoinPaths(allItems, true);
            i += 2;
        } else if (pattern.compare(i, 2, L"%1") == 0) {
            result += allItems.empty() ? L"" : SafeQuote(allItems[0]);
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
            if (!activeWithTrailing.empty() &&
                activeWithTrailing.back() == L'\\')
                activeWithTrailing += L'\\';
            result += activeWithTrailing;
            i += 2;
        } else if (pattern.compare(i, 2, L"%p") == 0) {
            std::wstring parent = activeDir;
            while (!parent.empty() && parent.back() == L'\\')
                parent.pop_back();
            size_t slash = parent.find_last_of(L'\\');
            if (slash == std::wstring::npos) {
                result += std::wstring();
            } else {
                std::wstring p = parent.substr(0, slash);
                if (p.length() == 2 && p[1] == L':')
                    p += L'\\';
                result += p;
            }
            i += 2;
        } else {
            result.push_back(pattern[i++]);
        }
    }
    return result;
}

void ExecuteApp(const std::wstring& cmd,
                const std::wstring& params,
                const std::wstring& workDir) {
    std::wstring targetPath = ResolveCommandPath(cmd);
    bool isPath = targetPath.find(L'\\') != std::wstring::npos;

    Wh_Log(L"ExecuteApp: File='%s'", PathFindFileNameW(targetPath.c_str()));

    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.hwnd = nullptr;
    sei.lpVerb = L"open";
    sei.lpFile = targetPath.c_str();
    sei.lpParameters = params.empty() ? nullptr : params.c_str();
    sei.lpDirectory = (isPath && !workDir.empty()) ? workDir.c_str() : nullptr;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"ShellExecuteExW failed for %s (error %lu)", targetPath.c_str(),
               GetLastError());
    }
}

bool IsSystemInLightTheme() {
    DWORD useLight = 0;
    DWORD size = sizeof(useLight);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &useLight, &size);
    return (status == ERROR_SUCCESS && useLight == 1);
}

Gdiplus::Color GetSystemAccentColor(BYTE alpha = 255) {
    DWORD color = 0;
    DWORD size = sizeof(color);
    // Reads the active Windows Accent Palette color (format: 0xAABBGGRR)
    LSTATUS status =
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &color, &size);

    if (status == ERROR_SUCCESS && color != 0) {
        BYTE r = color & 0xFF;
        BYTE g = (color >> 8) & 0xFF;
        BYTE b = (color >> 16) & 0xFF;
        return Gdiplus::Color(alpha, r, g, b);
    }

    // Windows 11 default accent blue fallback (#0067C0)
    return Gdiplus::Color(alpha, 0, 103, 192);
}

void ShowActionToast(HWND hOwner, const wchar_t* message) {
    if (!g_showActionToasts.load() || !message || g_unloading.load())
        return;

    std::wstring textStr = message;
    Wh_Log(L"ShowActionToast: Displaying '%s'", textStr.c_str());

    // Resolve per-monitor DPI for the active Explorer window
    UINT dpi = 96;
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto pfnGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(
            GetProcAddress(hUser32, "GetDpiForWindow"));
        if (pfnGetDpiForWindow && hOwner) {
            dpi = pfnGetDpiForWindow(hOwner);
        }
    }
    if (dpi == 0) dpi = 96;

    auto ScaleDPI = [dpi](int val) -> int {
        return MulDiv(val, static_cast<int>(dpi), 96);
    };

    // ============================================================
    // 1. THEME-AWARE COLOR PALETTES
    // ============================================================
    bool isLight = IsSystemInLightTheme();

    const Gdiplus::Color DARK_BG(250, 50, 50, 60);
    const Gdiplus::Color DARK_TEXT(255, 245, 245, 245);
    const Gdiplus::Color LIGHT_BG(240, 230, 230, 230);
    const Gdiplus::Color LIGHT_TEXT(255, 24, 24, 24);

    const Gdiplus::Color BG_COLOR = isLight ? LIGHT_BG : DARK_BG;
    const Gdiplus::Color TEXT_COLOR = isLight ? LIGHT_TEXT : DARK_TEXT;

    // ============================================================
    // 2. DPI-SCALED GEOMETRY & TYPOGRAPHY
    // ============================================================
    const wchar_t* FONT_FAMILY = L"Segoe UI Semibold";
    const int FONT_PIXEL_SIZE = ScaleDPI(15);
    const auto FONT_STYLE = Gdiplus::FontStyleRegular;

    const int TOAST_HEIGHT = ScaleDPI(46);
    const int PAD_X = ScaleDPI(46);
    const int MIN_WIDTH = ScaleDPI(95);
    const int CORNER_RADIUS = ScaleDPI(10);
    const int OFFSET_BOTTOM = ScaleDPI(80);

    static const wchar_t* CLASS_NAME = L"ExplorerShortcutToastOSD";
    static bool classRegistered = false;

    HINSTANCE hInstance = GetModuleHandle(nullptr);
    if (!classRegistered) {
        WNDCLASSEXW wc = {sizeof(wc)};
        wc.lpfnWndProc = DefWindowProcW;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        RegisterClassExW(&wc);
        classRegistered = true;
    }

    HDC hdcScreen = GetDC(nullptr);
    Gdiplus::FontFamily fontFamily(FONT_FAMILY);
    Gdiplus::Font font(&fontFamily, static_cast<Gdiplus::REAL>(FONT_PIXEL_SIZE), FONT_STYLE, Gdiplus::UnitPixel);

    int width = MIN_WIDTH;
    int height = TOAST_HEIGHT;

    // Scope gMeasure so it frees its HDC reference before ReleaseDC
    {
        Gdiplus::Graphics gMeasure(hdcScreen);
        Gdiplus::RectF boundRect;
        gMeasure.MeasureString(textStr.c_str(), -1, &font,
                               Gdiplus::RectF(0, 0, 1000.0f, 100.0f), &boundRect);
        width = static_cast<int>(boundRect.Width) + PAD_X;
        if (width < MIN_WIDTH) width = MIN_WIDTH;
    }

    POINT pt = {0, 0};
    RECT rcOwner = {};
    if (hOwner && GetWindowRect(hOwner, &rcOwner)) {
        pt.x = rcOwner.left + (rcOwner.right - rcOwner.left - width) / 2;
        pt.y = rcOwner.bottom - height - OFFSET_BOTTOM;
    } else {
        GetCursorPos(&pt);
        pt.x += ScaleDPI(12);
        pt.y += ScaleDPI(12);
    }

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        CLASS_NAME, nullptr, WS_POPUP, pt.x, pt.y, width, height,
        nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) {
        Wh_Log(L"ShowActionToast: CreateWindowExW failed (%lu)", GetLastError());
        ReleaseDC(nullptr, hdcScreen);
        return;
    }

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP hBmp = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    HGDIOBJ hOldBmp = SelectObject(hdcMem, hBmp);

    {
        Gdiplus::Graphics g(hdcMem);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        Gdiplus::GraphicsPath path;
        int r = CORNER_RADIUS;
        if (r <= 0) {
            path.AddRectangle(Gdiplus::Rect(0, 0, width, height));
        } else {
            if (r > height / 2) r = height / 2;
            path.AddArc(0, 0, r * 2, r * 2, 180, 90);
            path.AddArc(width - (r * 2), 0, r * 2, r * 2, 270, 90);
            path.AddArc(width - (r * 2), height - (r * 2), r * 2, r * 2, 0, 90);
            path.AddArc(0, height - (r * 2), r * 2, r * 2, 90, 90);
            path.CloseFigure();
        }

        Gdiplus::SolidBrush bgBrush(BG_COLOR);
        g.FillPath(&bgBrush, &path);

        // Scaled left vertical accent pill
        Gdiplus::Color accentColor = GetSystemAccentColor(255);
        Gdiplus::SolidBrush accentBrush(accentColor);

        int pillBarWidth = ScaleDPI(4);
        int pillBarHeight = height - ScaleDPI(14);
        int pillBarX = ScaleDPI(8);
        int pillBarY = (height - pillBarHeight) / 2;

        Gdiplus::GraphicsPath accentPath;
        accentPath.AddArc(pillBarX, pillBarY, pillBarWidth, pillBarWidth, 180, 180);
        accentPath.AddArc(pillBarX, pillBarY + pillBarHeight - pillBarWidth, pillBarWidth, pillBarWidth, 0, 180);
        accentPath.CloseFigure();

        g.FillPath(&accentBrush, &accentPath);

        // Centered label with text bounds
        Gdiplus::SolidBrush textBrush(TEXT_COLOR);
        Gdiplus::StringFormat sf;
        sf.SetAlignment(Gdiplus::StringAlignmentCenter);
        sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);

        int textLeftOffset = pillBarX + pillBarWidth + ScaleDPI(6);
        Gdiplus::RectF drawRect(
            static_cast<Gdiplus::REAL>(textLeftOffset), 0.0f,
            static_cast<Gdiplus::REAL>(width - textLeftOffset - ScaleDPI(14)),
            static_cast<Gdiplus::REAL>(height));
        g.DrawString(textStr.c_str(), -1, &font, drawRect, &sf, &textBrush);
    }

    POINT ptZero = {0, 0};
    SIZE size = {width, height};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, hdcScreen, &pt, &size, hdcMem, &ptZero, 0, &bf, ULW_ALPHA);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);

    // Sleep in small increments to exit promptly if the mod is unloading
    for (int i = 0; i < 28; ++i) {
        if (g_unloading.load()) break;
        Sleep(50);
    }

    // Smooth fade out
    for (int a = 255; a >= 0; a -= 25) {
        if (g_unloading.load()) break;
        bf.SourceConstantAlpha = static_cast<BYTE>(a);
        UpdateLayeredWindow(hwnd, nullptr, nullptr, nullptr, nullptr, nullptr, 0, &bf, ULW_ALPHA);
        Sleep(15);
    }

    DestroyWindow(hwnd);
    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

bool SetClipboardTextHelper(const std::wstring& text) {
    size_t bytes = (text.size() + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (!hMem)
        return false;

    void* pMem = GlobalLock(hMem);
    if (!pMem) {
        GlobalFree(hMem);
        return false;
    }
    memcpy(pMem, text.c_str(), bytes);
    GlobalUnlock(hMem);

    bool opened = false;
    for (int i = 0; i < 5; ++i) {
        if (OpenClipboard(nullptr)) {  // Use nullptr instead of rootHwnd
            opened = true;
            break;
        }
        Sleep(20);
    }

    if (!opened) {
        GlobalFree(hMem);
        return false;
    }

    EmptyClipboard();
    if (!SetClipboardData(CF_UNICODETEXT, hMem)) {
        GlobalFree(hMem);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

void ExecuteInternalCommand(const std::wstring& command,
                            HWND rootHwnd,
                            HWND capturedFocus) {
    if (_wcsicmp(command.c_str(), L"internal:folderOptions") == 0) {
        Wh_Log(L"internal:folderOptions: Opening Folder Options dialog");
        ExecuteApp(L"rundll32.exe", L"shell32.dll,Options_RunDLL 0", L"");
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:toggleHiddenFiles") == 0) {
        SHELLSTATE ss{};
        SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS, FALSE);
        ss.fShowAllObjects = !ss.fShowAllObjects;
        SHGetSetSettings(&ss, SSF_SHOWALLOBJECTS, TRUE);
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
        Wh_Log(L"internal:toggleHiddenFiles: ShowAllObjects set to %d",
               ss.fShowAllObjects ? 1 : 0);
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:toggleFileExtensions") == 0) {
        SHELLSTATE ss{};
        SHGetSetSettings(&ss, SSF_SHOWEXTENSIONS, FALSE);
        ss.fShowExtensions = !ss.fShowExtensions;
        SHGetSetSettings(&ss, SSF_SHOWEXTENSIONS, TRUE);
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
        Wh_Log(L"internal:toggleFileExtensions: ShowExtensions set to %d",
               ss.fShowExtensions ? 1 : 0);
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:copyPath") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        if (!psv) {
            Wh_Log(L"internal:copyPath: Failed to get IShellView");
            return;
        }

        std::vector<std::wstring> allSelected = GetSelectedPaths(psv);
        std::wstring text;
        bool isFolderPath = false;

        if (!allSelected.empty()) {
            for (const auto& p : allSelected) {
                if (!text.empty())
                    text += L"\r\n";
                text += p;
            }
            Wh_Log(L"internal:copyPath: Copied %zu selected item path(s)",
                   allSelected.size());
        } else {
            text = GetActiveFolderPath(psv);
            isFolderPath = true;
            Wh_Log(
                L"internal:copyPath: Nothing selected, copied active folder: "
                L"%s",
                text.c_str());
        }

        psv->Release();

        if (!text.empty() && SetClipboardTextHelper(text)) {
            ShowActionToast(rootHwnd, isFolderPath
                                          ? L"Current folder path copied"
                                          : L"Path copied");
        } else {
            Wh_Log(
                L"internal:copyPath: SetClipboardTextHelper failed or path "
                L"empty");
        }
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:copyName") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        if (!psv) {
            Wh_Log(L"internal:copyName: Failed to get active IShellView");
            return;
        }

        std::vector<std::wstring> allSelected = GetSelectedPaths(psv);
        psv->Release();

        if (allSelected.empty()) {
            Wh_Log(L"internal:copyName: No items selected");
            return;
        }

        std::wstring text;
        for (const auto& p : allSelected) {
            if (!text.empty())
                text += L"\r\n";
            PCWSTR name = PathFindFileNameW(p.c_str());
            text += (name ? name : L"");
        }
        Wh_Log(L"internal:copyName: Copied %zu item name(s) to clipboard",
               allSelected.size());

        if (SetClipboardTextHelper(text)) {
            ShowActionToast(rootHwnd, L"Name copied");
        } else {
            Wh_Log(L"internal:copyName: SetClipboardTextHelper failed");
        }
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:openWith") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        if (!psv) {
            Wh_Log(L"internal:openWith: Failed to get active IShellView");
            return;
        }

        std::vector<std::wstring> allSelected = GetSelectedPaths(psv);
        psv->Release();

        if (allSelected.empty()) {
            Wh_Log(L"internal:openWith: No items selected");
            return;
        }

        std::wstring targetPath = allSelected[0];
        DWORD attr = GetFileAttributesW(targetPath.c_str());

        if (attr == INVALID_FILE_ATTRIBUTES ||
            (attr & FILE_ATTRIBUTE_DIRECTORY)) {
            Wh_Log(
                L"internal:openWith: Selected item is a directory or "
                L"inaccessible.");
            return;
        }

        OPENASINFO oai = {};
        oai.pcszFile = targetPath.c_str();
        oai.oaifInFlags = OAIF_EXEC | OAIF_ALLOW_REGISTRATION;
        HRESULT hr = SHOpenWithDialog(nullptr, &oai);
        if (SUCCEEDED(hr)) {
            Wh_Log(L"internal:openWith: Dialog displayed successfully");
        } else {
            Wh_Log(L"internal:openWith: Failed to open dialog (hr=0x%08X)", hr);
        }
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:openRecycleBin") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        if (!psv) {
            Wh_Log(L"internal:openRecycleBin: Failed to get active IShellView");
            return;
        }

        PIDLIST_ABSOLUTE pidlRecycle = nullptr;
        if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0,
                                             nullptr, &pidlRecycle)) &&
            pidlRecycle) {
            IServiceProvider* psp = nullptr;
            if (SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&psp)))) {
                IShellBrowser* psb = nullptr;
                if (SUCCEEDED(psp->QueryService(SID_STopLevelBrowser,
                                                IID_PPV_ARGS(&psb)))) {
                    HRESULT hr = psb->BrowseObject(
                        pidlRecycle, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
                    if (SUCCEEDED(hr)) {
                        Wh_Log(
                            L"internal:openRecycleBin: Navigated successfully");
                    } else {
                        Wh_Log(
                            L"internal:openRecycleBin: Navigation failed "
                            L"(hr=0x%08X)",
                            hr);
                    }
                    psb->Release();
                }
                psp->Release();
            }
            CoTaskMemFree(pidlRecycle);
        }
        psv->Release();

        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:emptyRecycleBin") == 0) {
        Wh_Log(L"internal:emptyRecycleBin invoked");
        SHQUERYRBINFO rbInfo = {sizeof(rbInfo)};
        if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &rbInfo))) {
            // If the Recycle Bin has 0 items, do nothing
            if (rbInfo.i64NumItems == 0) {
                Wh_Log(
                    L"internal:emptyRecycleBin: Recycle Bin is already empty");
                return;
            }
        }

        int res = MessageBoxW(
            nullptr, L"Permanently delete all items in the Recycle Bin?",
            L"Explorer Custom Shortcuts",
            MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1 | MB_SETFOREGROUND);

        if (res == IDOK && !g_unloading.load()) {
            HRESULT hr = SHEmptyRecycleBinW(
                nullptr, nullptr, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI);
            Wh_Log(
                L"internal:emptyRecycleBin: SHEmptyRecycleBin result "
                L"(hr=0x%08X)",
                hr);
            if (SUCCEEDED(hr)) {
                ShowActionToast(rootHwnd, L"Recycle Bin emptied");
            }
        } else {
            Wh_Log(L"internal:emptyRecycleBin: Operation cancelled by user");
        }
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:newTextFile") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        if (!psv) {
            Wh_Log(L"internal:newTextFile: Failed to get active IShellView");
            return;
        }

        std::wstring currentDir = GetActiveFolderPath(psv);
        if (currentDir.empty()) {
            Wh_Log(L"internal:newTextFile: Active folder path is empty");
            psv->Release();
            return;
        }

        std::wstring targetFilePath = currentDir + L"\\New Text Document.txt";
        int counter = 2;
        while (GetFileAttributesW(targetFilePath.c_str()) !=
               INVALID_FILE_ATTRIBUTES) {
            targetFilePath = currentDir + L"\\New Text Document (" +
                             std::to_wstring(counter++) + L").txt";
        }

        HANDLE hFile =
            CreateFileW(targetFilePath.c_str(), GENERIC_WRITE, 0, nullptr,
                        CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE) {
            CloseHandle(hFile);
            SHChangeNotify(SHCNE_CREATE, SHCNF_PATHW, targetFilePath.c_str(),
                           nullptr);
            Wh_Log(L"internal:newTextFile: Created file successfully");

            PIDLIST_ABSOLUTE pidlTarget = nullptr;
            if (SUCCEEDED(SHParseDisplayName(targetFilePath.c_str(), nullptr,
                                             &pidlTarget, 0, nullptr)) &&
                pidlTarget) {
                PITEMID_CHILD pidlChild = ILFindLastID(pidlTarget);
                if (pidlChild) {
                    for (int r = 0; r < 5; ++r) {
                        if (SUCCEEDED(psv->SelectItem(
                                pidlChild, SVSI_SELECT | SVSI_FOCUSED |
                                               SVSI_DESELECTOTHERS |
                                               SVSI_ENSUREVISIBLE))) {
                            break;
                        }
                        Sleep(40);
                    }
                }
                CoTaskMemFree(pidlTarget);
            }
        } else {
            Wh_Log(L"internal:newTextFile: CreateFileW failed (err=%lu)",
                   GetLastError());
        }
        psv->Release();
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:newFolder") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        if (!psv) {
            Wh_Log(L"internal:newFolder: Failed to get active IShellView");
            return;
        }
        std::wstring currentDir = GetActiveFolderPath(psv);
        if (currentDir.empty()) {
            Wh_Log(L"internal:newFolder: Active folder path is empty");
            psv->Release();
            return;
        }

        std::wstring targetFolderPath = currentDir + L"\\New Folder";
        int counter = 2;
        while (GetFileAttributesW(targetFolderPath.c_str()) !=
               INVALID_FILE_ATTRIBUTES) {
            targetFolderPath = currentDir + L"\\New Folder (" +
                               std::to_wstring(counter++) + L")";
        }

        if (CreateDirectoryW(targetFolderPath.c_str(), nullptr)) {
            SHChangeNotify(SHCNE_MKDIR, SHCNF_PATHW, targetFolderPath.c_str(),
                           nullptr);
            Wh_Log(L"internal:newFolder: Created folder successfully");

            PIDLIST_ABSOLUTE pidlTarget = nullptr;
            if (SUCCEEDED(SHParseDisplayName(targetFolderPath.c_str(), nullptr,
                                             &pidlTarget, 0, nullptr)) &&
                pidlTarget) {
                PITEMID_CHILD pidlChild = ILFindLastID(pidlTarget);
                if (pidlChild) {
                    for (int r = 0; r < 5; ++r) {
                        if (SUCCEEDED(psv->SelectItem(
                                pidlChild, SVSI_SELECT | SVSI_EDIT |
                                               SVSI_DESELECTOTHERS |
                                               SVSI_ENSUREVISIBLE))) {
                            break;
                        }
                        Sleep(40);
                    }
                }
                CoTaskMemFree(pidlTarget);
            }
        } else {
            Wh_Log(L"internal:newFolder: CreateDirectoryW failed (err=%lu)",
                   GetLastError());
        }
        psv->Release();
        return;
    }

    if (_wcsicmp(command.c_str(), L"internal:openParentFolder") == 0) {
        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        if (!psv) {
            Wh_Log(
                L"internal:openParentFolder: Failed to get active IShellView");
            return;
        }

        IServiceProvider* psp = nullptr;
        if (SUCCEEDED(psv->QueryInterface(IID_PPV_ARGS(&psp))) && psp) {
            IShellBrowser* psb = nullptr;
            if (SUCCEEDED(psp->QueryService(SID_STopLevelBrowser,
                                            IID_PPV_ARGS(&psb))) &&
                psb) {
                HRESULT hr =
                    psb->BrowseObject(nullptr, SBSP_SAMEBROWSER | SBSP_PARENT);
                if (SUCCEEDED(hr)) {
                    Wh_Log(
                        L"internal:openParentFolder: Navigated to parent "
                        L"successfully");
                } else {
                    Wh_Log(
                        L"internal:openParentFolder: Navigation failed "
                        L"(hr=0x%08X)",
                        hr);
                }
                psb->Release();
            }
            psp->Release();
        }
        psv->Release();
        return;
    }

    Wh_Log(L"Unknown internal command: %s", command.c_str());
}

void DispatchShortcutExecution(CustomShortcut sc,
                               HWND rootHwnd,
                               HWND capturedFocus) {
    QueueBackgroundWork([sc = std::move(sc), rootHwnd, capturedFocus]() {
        if (g_unloading.load())
            return;

        if (sc.path.rfind(L"internal:", 0) == 0) {
            ExecuteInternalCommand(sc.path, rootHwnd, capturedFocus);
            return;
        }

        IShellView* psv = GetActiveShellView(rootHwnd, capturedFocus);
        std::wstring activeDir = GetActiveFolderPath(psv);
        std::vector<std::wstring> allSelected = GetSelectedPaths(psv);
        if (psv) {
            psv->Release();
        }

        std::vector<std::wstring> filesOnly;
        std::vector<std::wstring> foldersOnly;
        for (const auto& item : allSelected) {
            DWORD attr = GetFileAttributesW(item.c_str());
            if (attr != INVALID_FILE_ATTRIBUTES &&
                (attr & FILE_ATTRIBUTE_DIRECTORY)) {
                foldersOnly.push_back(item);
            } else {
                filesOnly.push_back(item);
            }
        }

        std::vector<std::wstring> commands;
        if (sc.launchMode == L"loop_files") {
            for (const auto& file : filesOnly) {
                std::vector<std::wstring> cur = {file};
                commands.push_back(
                    ExpandTokens(sc.argsPattern, activeDir, cur, cur, {}));
            }
        } else if (sc.launchMode == L"loop_folders") {
            for (const auto& folder : foldersOnly) {
                std::vector<std::wstring> cur = {folder};
                commands.push_back(
                    ExpandTokens(sc.argsPattern, folder, cur, {}, cur));
            }
        } else {
            commands.push_back(ExpandTokens(sc.argsPattern, activeDir,
                                            allSelected, filesOnly,
                                            foldersOnly));
        }

        for (const auto& cmdArgs : commands) {
            if (g_unloading.load())
                break;
            ExecuteApp(sc.path, cmdArgs, activeDir);
        }
    });
}

bool IsInlineEditingActive(HWND rootHwnd) {
    HWND hFocus = GetFocus();
    if (!hFocus)
        return false;

    HWND hFocusRoot = GetAncestor(hFocus, GA_ROOT);
    if (hFocusRoot && hFocusRoot != rootHwnd) {
        if (GetWindow(hFocusRoot, GW_OWNER) == rootHwnd) {
            return true;
        }
    }

    WCHAR cls[128] = {};
    if (GetClassNameW(hFocus, cls, ARRAYSIZE(cls))) {
        if (wcscmp(cls, L"Edit") == 0 || wcscmp(cls, L"RichEditD2DPT") == 0 ||
            wcsstr(cls, L"TextBox") != nullptr) {
            return true;
        }
    }

    for (HWND h = hFocus; h && h != rootHwnd; h = GetParent(h)) {
        WCHAR parentCls[128] = {};
        if (GetClassNameW(h, parentCls, ARRAYSIZE(parentCls))) {
            if (wcsstr(parentCls, L"Address Band Root") ||
                wcsstr(parentCls, L"ComboBoxEx32") ||
                wcsstr(parentCls, L"SearchEditBoxWrapperClass")) {
                return true;
            }
        }
    }

    GUITHREADINFO gti = {sizeof(gti)};
    if (GetGUIThreadInfo(GetWindowThreadProcessId(hFocus, nullptr), &gti)) {
        if (gti.hwndCaret &&
            (gti.hwndCaret == hFocus || IsChild(hFocus, gti.hwndCaret))) {
            return true;
        }
    }

    return false;
}

bool ProcessHotKey(HWND hwnd, WPARAM key) {
    if (!hwnd)
        return false;

    HWND rootHwnd = GetAncestor(hwnd, GA_ROOT);
    if (!rootHwnd)
        return false;

    WCHAR className[256] = {};
    if (!GetClassNameW(rootHwnd, className, ARRAYSIZE(className))) {
        return false;
    }

    if (wcscmp(className, L"CabinetWClass") == 0 ||
        wcscmp(className, L"ExploreWClass") == 0) {
        std::shared_lock<std::shared_mutex> lock(g_shortcutsMutex);
        if (g_shortcuts.empty())
            return false;

        bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;

        for (const auto& sc : g_shortcuts) {
            if (sc.vkCode != 0 && key == (WPARAM)sc.vkCode && ctrl == sc.ctrl &&
                shift == sc.shift && alt == sc.alt) {
                if (IsInlineEditingActive(rootHwnd)) {
                    Wh_Log(
                        L"Shortcut '%s' suppressed (inline edit/rename active)",
                        sc.name.c_str());
                    return false;
                }

                Wh_Log(L"Shortcut triggered: '%s' -> Target: '%s'",
                       sc.name.c_str(), sc.path.c_str());
                HWND capturedFocus = GetFocus();
                DispatchShortcutExecution(sc, rootHwnd, capturedFocus);
                return true;
            }
        }
    }
    return false;
}

using TranslateAcceleratorW_t = int(WINAPI*)(HWND hWnd,
                                             HACCEL hAccTable,
                                             LPMSG lpMsg);
TranslateAcceleratorW_t TranslateAcceleratorW_Original;

int WINAPI TranslateAcceleratorW_Hook(HWND hWnd,
                                      HACCEL hAccTable,
                                      LPMSG lpMsg) {
    if (lpMsg &&
        (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_SYSKEYDOWN)) {
        if (!(lpMsg->lParam & 0x40000000)) {
            if (ProcessHotKey(lpMsg->hwnd, lpMsg->wParam)) {
                return 1;
            }
        }
    }
    return TranslateAcceleratorW_Original(hWnd, hAccTable, lpMsg);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing mod...");
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);

    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(TranslateAcceleratorW,
                                        TranslateAcceleratorW_Hook,
                                        &TranslateAcceleratorW_Original)) {
        Wh_Log(L"Failed to hook TranslateAcceleratorW");
        return FALSE;
    }
    Wh_Log(L"Mod initialized successfully.");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Unloading mod...");
    g_unloading = true;
    std::vector<HANDLE> threadsToJoin;
    {
        std::lock_guard<std::mutex> lock(g_threadsMutex);
        threadsToJoin.swap(g_threads);
    }

    for (HANDLE h : threadsToJoin) {
        DWORD tid = GetThreadId(h);
        int rounds = 0;
        while (WaitForSingleObject(h, 100) == WAIT_TIMEOUT) {
            EnumThreadWindows(
                tid,
                [](HWND hWnd, LPARAM) -> BOOL {
                    if (IsWindowVisible(hWnd)) {
                        PostMessageW(hWnd, WM_CLOSE, 0, 0);
                    }
                    return TRUE;
                },
                0);

            // After a few attempts, escalate to WM_QUIT to unwind any stubborn
            // modal dialogs
            if (++rounds > 10) {
                PostThreadMessageW(tid, WM_QUIT, 0, 0);
            }
        }
        CloseHandle(h);
    }
    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    Wh_Log(L"Mod uninitialized successfully.");
}

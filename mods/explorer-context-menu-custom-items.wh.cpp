// ==WindhawkMod==
// @id              explorer-context-menu-custom-items
// @name            Explorer Context Menu Custom Items
// @description     Adds filtered custom user-defined options to the classic (Show more options) context menu.
// @version         1.0
// @author          Deepak
// @github          https://github.com/deepak-raven
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Context Menu Custom Items

Easily add, filter, and manage custom shortcut actions inside the classic context menu (**Show more options**).

### Key Features:
* **Dynamic Commands:** Run any CLI tool or executable (like `whisperx`, `ffmpeg`, or `python`) directly on a highlighted file.
* **Extension Filtering:** Restrict commands to specific formats (e.g., `.mp4, .mkv`) so your menu stays clean and relevant.
* **Folder Toggle:** Choose whether specific shortcuts show up when right-clicking folders or folder backdrops.

### How to use variables:
* Use `%1` in your command string to automatically pass the absolute file path of your selected file. It is recommended to use double quotes around it in your command settings (e.g., whisperx "%1") to handle spaces in file paths properly.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- items:
  - - label: ""
      $name: Label
      $description: The text to show in the context menu.
    - command: ""
      $name: Command
      $description: The command line to execute. Use %1 for the selected file path.
    - extensions: ""
      $name: File Extensions (Optional)
      $description: Comma-separated list of extensions (e.g., .mp4, .mkv). Leave empty for all files.
    - allowFolders: true
      $name: Show on Folders
      $description: Toggle whether this option should appear when right-clicking folders or folder backgrounds.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <commctrl.h>
#include <mutex>
#include <shldisp.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <string>
#include <vector>
#include <windows.h>
#include <sstream>

#define MIN_CUSTOM_ID 0xBF31

struct CustomCommand {
    std::wstring label;
    std::wstring command;
    std::wstring extensions;
    bool allowFolders;
};

std::vector<CustomCommand> g_customCommands;
std::mutex g_settingsMutex;

decltype(&TrackPopupMenuEx) TrackPopupMenuEx_orig;

struct ComInit {
    HRESULT hr;
    ComInit() { hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); }
    ~ComInit() { if (SUCCEEDED(hr)) CoUninitialize(); }
};

// Clean whitespace helper for extension token lists
std::wstring TrimString(const std::wstring& str) {
    size_t first = str.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return L"";
    size_t last = str.find_last_not_of(L" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool MatchExtensionExact(const std::wstring& multiExts, const std::wstring& targetExt) {
    if (multiExts.empty()) return true;
    if (targetExt.empty()) return false;

    std::wstringstream ss(multiExts);
    std::wstring token;
    
    std::wstring lowerTarget = targetExt;
    std::transform(lowerTarget.begin(), lowerTarget.end(), lowerTarget.begin(), ::towlower);
    if (lowerTarget[0] != L'.') lowerTarget = L"." + lowerTarget;

    while (std::getline(ss, token, L',')) {
        std::wstring cleanToken = TrimString(token);
        std::transform(cleanToken.begin(), cleanToken.end(), cleanToken.begin(), ::towlower);
        if (cleanToken[0] != L'.') cleanToken = L"." + cleanToken;
        if (cleanToken == lowerTarget) return true;
    }
    return false;
}

// Verifies if the context window chain originates from a legitimate shell pane view
bool IsShellViewWindow(HWND hwnd) {
    HWND hwndCurrent = hwnd;
    while (hwndCurrent) {
        WCHAR szClassName[256] = {};
        GetClassNameW(hwndCurrent, szClassName, 256);
        if (wcscmp(szClassName, L"SHELLDLL_DefView") == 0) {
            return true;
        }
        hwndCurrent = GetParent(hwndCurrent);
    }
    return false;
}

std::wstring ExpandEnv(const std::wstring& input) {
    DWORD size = ExpandEnvironmentStringsW(input.c_str(), nullptr, 0);
    if (size == 0) return input;
    std::vector<wchar_t> buffer(size);
    ExpandEnvironmentStringsW(input.c_str(), buffer.data(), size);
    return std::wstring(buffer.data());
}

void ExecuteCommand(const std::wstring& commandLine, const std::wstring& workingDirectory) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWNORMAL;

    std::wstring wrappedCommand = L"cmd.exe /c " + commandLine;
    std::vector<wchar_t> cmdBuffer(wrappedCommand.begin(), wrappedCommand.end());
    cmdBuffer.push_back(L'\0');

    LPCWSTR pWorkDir = workingDirectory.empty() ? nullptr : workingDirectory.c_str();

    if (CreateProcessW(nullptr, cmdBuffer.data(), nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE, nullptr, pWorkDir, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

// Obtains the true target file systems paths using precise shell folder enumerations
std::vector<std::wstring> GetSelectedPaths(HWND hwndTarget) {
    std::vector<std::wstring> paths;
    ComInit com;

    IShellBrowser* pBrowser = (IShellBrowser*)SendMessageW(hwndTarget, WM_USER + 7, 0, 0);
    if (!pBrowser) {
        HWND hwndParent = GetParent(hwndTarget);
        while (hwndParent && !pBrowser) {
            pBrowser = (IShellBrowser*)SendMessageW(hwndParent, WM_USER + 7, 0, 0);
            hwndParent = GetParent(hwndParent);
        }
    }
    if (!pBrowser) return paths;

    IShellView* pShellView = nullptr;
    // Fixed: Changed QueryActiveViewer to QueryActiveShellView
    if (SUCCEEDED(pBrowser->QueryActiveShellView(&pShellView)) && pShellView) {
        IDataObject* pDataObject = nullptr;
        if (SUCCEEDED(pShellView->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(&pDataObject))) && pDataObject) {
            FORMATETC fmt = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stg;
            if (SUCCEEDED(pDataObject->GetData(&fmt, &stg))) {
                HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal);
                if (hDrop) {
                    UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
                    for (UINT i = 0; i < fileCount; ++i) {
                        UINT size = DragQueryFileW(hDrop, i, nullptr, 0);
                        if (size > 0) {
                            std::vector<wchar_t> filePath(size + 1);
                            DragQueryFileW(hDrop, i, filePath.data(), size + 1);
                            paths.push_back(filePath.data());
                        }
                    }
                    GlobalUnlock(stg.hGlobal);
                }
                ReleaseStgMedium(&stg);
            }
            pDataObject->Release();
        }

        // Fallback context: Handles internal directory layouts when clicking background spaces empty of items
        if (paths.empty()) {
            IFolderView* pFolderView = nullptr;
            if (SUCCEEDED(pShellView->QueryInterface(IID_PPV_ARGS(&pFolderView))) && pFolderView) {
                IPersistFolder2* pPersistFolder = nullptr;
                if (SUCCEEDED(pFolderView->GetFolder(IID_PPV_ARGS(&pPersistFolder))) && pPersistFolder) {
                    PIDLIST_ABSOLUTE pidl = nullptr;
                    if (SUCCEEDED(pPersistFolder->GetCurFolder(&pidl)) && pidl) {
                        wchar_t szPath[MAX_PATH];
                        if (SHGetPathFromIDListW(pidl, szPath)) {
                            paths.push_back(szPath);
                        }
                        CoTaskMemFree(pidl);
                    }
                    pPersistFolder->Release();
                }
                pFolderView->Release();
            }
        }
        pShellView->Release();
    }
    return paths;
}

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_customCommands.clear();
    for (int i = 0; ; i++) {
        PCWSTR szLabel = Wh_GetStringSetting(L"items[%d].label", i);
        PCWSTR szCommand = Wh_GetStringSetting(L"items[%d].command", i);
        PCWSTR szExts = Wh_GetStringSetting(L"items[%d].extensions", i);
        int nAllowFolders = Wh_GetIntSetting(L"items[%d].allowFolders", i);

        if ((!szLabel || *szLabel == L'\0') || (!szCommand || *szCommand == L'\0')) {
            Wh_FreeStringSetting(szLabel);
            Wh_FreeStringSetting(szCommand);
            Wh_FreeStringSetting(szExts);
            break;
        }

        g_customCommands.push_back({ 
            szLabel, 
            szCommand, 
            szExts ? szExts : L"", 
            nAllowFolders != 0 
        });

        Wh_FreeStringSetting(szLabel);
        Wh_FreeStringSetting(szCommand);
        Wh_FreeStringSetting(szExts);
    }
}

void ExecuteCustomCommand(int index, const std::vector<std::wstring>& paths) {
    std::wstring commandTemplate;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        if (index >= 0 && index < (int)g_customCommands.size()) {
            commandTemplate = g_customCommands[index].command;
        }
    }
    if (commandTemplate.empty() || paths.empty()) return;

    for (const auto& path : paths) {
        std::wstring cmd = ExpandEnv(commandTemplate);
        size_t pos = cmd.find(L"%1");
        while (pos != std::wstring::npos) {
            cmd.replace(pos, 2, path);
            pos = cmd.find(L"%1", pos + path.length());
        }
        std::wstring workingDir = L"";
        size_t lastSlash = path.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            workingDir = path.substr(0, lastSlash);
        }
        ExecuteCommand(cmd, workingDir);
    }
}

BOOL WINAPI HookedTrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm) {
    if (!IsShellViewWindow(hwnd)) {
        return TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hwnd, lptpm);
    }

    std::vector<CustomCommand> activeCommands;
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        activeCommands = g_customCommands;
    }

    if (activeCommands.empty()) {
        return TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hwnd, lptpm);
    }

    std::vector<std::wstring> selectedPaths = GetSelectedPaths(hwnd);
    std::wstring targetPath = selectedPaths.empty() ? L"" : selectedPaths[0];
    
    bool isFolder = false;
    std::wstring targetExt = L"";
    
    if (!targetPath.empty()) {
        DWORD dwAttrs = GetFileAttributesW(targetPath.c_str());
        if (dwAttrs != INVALID_FILE_ATTRIBUTES && (dwAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            isFolder = true;
        } else {
            size_t dotPos = targetPath.find_last_of(L".");
            size_t slashPos = targetPath.find_last_of(L"\\/");
            if (dotPos != std::wstring::npos && (slashPos == std::wstring::npos || dotPos > slashPos)) {
                targetExt = targetPath.substr(dotPos);
            }
        }
    }

    bool addedAny = false;
    for (size_t i = 0; i < activeCommands.size(); ++i) {
        const auto& cmd = activeCommands[i];

        if (isFolder && !cmd.allowFolders) continue;
        if (!cmd.extensions.empty() && (isFolder || !MatchExtensionExact(cmd.extensions, targetExt))) continue;

        if (!addedAny && GetMenuItemCount(hMenu) > 0) {
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            addedAny = true;
        }

        AppendMenuW(hMenu, MF_STRING, MIN_CUSTOM_ID + i, cmd.label.c_str());
    }

    UINT result = TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hwnd, lptpm);

    if ((uFlags & TPM_RETURNCMD) && result >= MIN_CUSTOM_ID && result < MIN_CUSTOM_ID + activeCommands.size()) {
        ExecuteCustomCommand(result - MIN_CUSTOM_ID, selectedPaths);
        return 0; 
    }

    return result;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

BOOL Wh_ModInit() {
    LoadSettings();
    if (!Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)HookedTrackPopupMenuEx, (void**)&TrackPopupMenuEx_orig)) {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {}

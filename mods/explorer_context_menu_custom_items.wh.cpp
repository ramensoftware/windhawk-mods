// ==WindhawkMod==
// @id              explorer-context-menu-custom-items
// @name            Explorer Context Menu Custom Items
// @description     Adds filtered custom user-defined options to the classic
// (Show more options) File Explorer context menu.
// @version         1.0
// @author          Deepak
// @github          https://github.com/deepak-raven
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lshlwapi -lole32 -loleaut32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Context Menu Custom Items

Easily add, filter, and manage custom shortcut actions inside the classic
context menu (**Show more options**).

### Key Features:
* **Dynamic Commands:** Run any CLI tool or executable (like `whisperx`,
`ffmpeg`, or `python`) directly on a highlighted file.
* **Extension Filtering:** Restrict commands to specific formats (e.g., `.mp4,
.mkv`) so your menu stays clean and relevant.
* **Folder Toggle:** Choose whether specific shortcuts show up when
right-clicking folders or folder backdrops.

### How to use variables:
* Use `%1` in your command string to automatically pass the absolute file path
of your selected file.
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
      $description: The command line to execute. Use %1 for the selected file
path.
    - extensions: ""
      $name: File Extensions (Optional)
      $description: Comma-separated list of extensions (e.g., .mp4, .mkv). Leave
empty for all files.
    - allowFolders: true
      $name: Show on Folders
      $description: Toggle whether this option should appear when right-clicking
folders or folder backgrounds.
*/
// ==/WindhawkModSettings==

#include <algorithm>
#include <commctrl.h>
#include <mutex>
#include <shldisp.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <string>
#include <vector>
#include <windows.h>

#define SUBCLASS_ID 48271
#define MIN_CUSTOM_ID 0x8000

const GUID My_CLSID_ShellWindows = {
    0x9ba05972,
    0xf6a8,
    0x11cf,
    {0xa4, 0x42, 0x00, 0xa0, 0xc9, 0x0a, 0x8f, 0x39}};

struct CustomCommand {
  std::wstring label;
  std::wstring command;
  std::wstring extensions;
  bool allowFolders;
};

std::vector<CustomCommand> g_customCommands;
std::mutex g_settingsMutex;

decltype(&TrackPopupMenuEx) TrackPopupMenuEx_orig;

LRESULT CALLBACK ExplorerMenuSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam, UINT_PTR uIdSubclass,
                                          DWORD_PTR dwRefData);
void ExecuteCustomCommand(int index, HWND hwnd);

struct ComInit {
  HRESULT hr;
  ComInit() { hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED); }
  ~ComInit() {
    if (SUCCEEDED(hr))
      CoUninitialize();
  }
};

bool ContainsStringNoCase(std::wstring mainStr, std::wstring toFind) {
  std::transform(mainStr.begin(), mainStr.end(), mainStr.begin(), ::towlower);
  std::transform(toFind.begin(), toFind.end(), toFind.begin(), ::towlower);
  return mainStr.find(toFind) != std::wstring::npos;
}

std::wstring ExpandEnv(const std::wstring &input) {
  DWORD size = ExpandEnvironmentStringsW(input.c_str(), nullptr, 0);
  if (size == 0)
    return input;
  std::vector<wchar_t> buffer(size);
  ExpandEnvironmentStringsW(input.c_str(), buffer.data(), size);
  return std::wstring(buffer.data());
}

void ExecuteCommand(const std::wstring &commandLine,
                    const std::wstring &workingDirectory) {
  STARTUPINFOW si = {sizeof(si)};
  PROCESS_INFORMATION pi = {};
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOWNORMAL;

  std::wstring wrappedCommand = L"cmd.exe /c " + commandLine;
  std::vector<wchar_t> cmdBuffer(wrappedCommand.begin(), wrappedCommand.end());
  cmdBuffer.push_back(L'\0');

  LPCWSTR pWorkDir =
      workingDirectory.empty() ? nullptr : workingDirectory.c_str();

  if (CreateProcessW(nullptr, cmdBuffer.data(), nullptr, nullptr, FALSE,
                     CREATE_NEW_CONSOLE, nullptr, pWorkDir, &si, &pi)) {
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
}

std::vector<std::wstring> GetSelectedPaths(HWND hwndTarget) {
  std::vector<std::wstring> paths;
  ComInit com;

  HWND hwndFore = GetForegroundWindow();
  HWND hwndTop = GetAncestor(hwndTarget, GA_ROOTOWNER);
  if (!hwndTop)
    hwndTop = hwndTarget;
  HWND hwndForeTop = hwndFore ? GetAncestor(hwndFore, GA_ROOTOWNER) : nullptr;

  IShellWindows *pShellWindows = nullptr;
  HRESULT hr = CoCreateInstance(My_CLSID_ShellWindows, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&pShellWindows));

  if (SUCCEEDED(hr) && pShellWindows) {
    long count = 0;
    pShellWindows->get_Count(&count);

    for (long i = 0; i < count; ++i) {
      VARIANT vt;
      VariantInit(&vt);
      vt.vt = VT_I4;
      vt.lVal = i;
      IDispatch *pDisp = nullptr;
      if (SUCCEEDED(pShellWindows->Item(vt, &pDisp)) && pDisp) {
        IWebBrowserApp *pApp = nullptr;
        if (SUCCEEDED(pDisp->QueryInterface(IID_PPV_ARGS(&pApp))) && pApp) {
          SHANDLE_PTR shWnd = 0;
          if (SUCCEEDED(pApp->get_HWND(&shWnd))) {
            HWND hwndWBA = (HWND)shWnd;
            if (hwndWBA == hwndTop || hwndWBA == hwndTarget ||
                hwndWBA == hwndFore || hwndWBA == hwndForeTop) {
              IDispatch *pDocDisp = nullptr;
              if (SUCCEEDED(pApp->get_Document(&pDocDisp)) && pDocDisp) {
                IShellFolderViewDual *pFolderViewDual = nullptr;
                if (SUCCEEDED(pDocDisp->QueryInterface(
                        IID_PPV_ARGS(&pFolderViewDual))) &&
                    pFolderViewDual) {
                  FolderItems *pSelectedItems = nullptr;
                  if (SUCCEEDED(
                          pFolderViewDual->SelectedItems(&pSelectedItems)) &&
                      pSelectedItems) {
                    long itemCount = 0;
                    pSelectedItems->get_Count(&itemCount);
                    for (long j = 0; j < itemCount; ++j) {
                      VARIANT index;
                      VariantInit(&index);
                      index.vt = VT_I4;
                      index.lVal = j;
                      FolderItem *pItem = nullptr;
                      if (SUCCEEDED(pSelectedItems->Item(index, &pItem)) &&
                          pItem) {
                        BSTR bstrPath = nullptr;
                        if (SUCCEEDED(pItem->get_Path(&bstrPath)) && bstrPath) {
                          paths.push_back(bstrPath);
                          SysFreeString(bstrPath);
                        }
                        pItem->Release();
                      }
                    }
                    pSelectedItems->Release();
                  }
                  if (paths.empty()) {
                    Folder *pFolder = nullptr;
                    if (SUCCEEDED(pFolderViewDual->get_Folder(&pFolder)) &&
                        pFolder) {
                      BSTR bstrTitle = nullptr;
                      if (SUCCEEDED(pFolder->get_Title(&bstrTitle)) &&
                          bstrTitle) {
                        paths.push_back(bstrTitle);
                        SysFreeString(bstrTitle);
                      }
                      pFolder->Release();
                    }
                  }
                  pFolderViewDual->Release();
                }
                pDocDisp->Release();
              }
              pApp->Release();
              pDisp->Release();
              break;
            }
          }
          pApp->Release();
        }
        pDisp->Release();
      }
    }
    pShellWindows->Release();
  }
  return paths;
}

void LoadSettings() {
  std::lock_guard<std::mutex> lock(g_settingsMutex);
  g_customCommands.clear();
  for (int i = 0;; i++) {
    PCWSTR szLabel = Wh_GetStringSetting(L"items[%d].label", i);
    PCWSTR szCommand = Wh_GetStringSetting(L"items[%d].command", i);
    PCWSTR szExts = Wh_GetStringSetting(L"items[%d].extensions", i);
    int nAllowFolders = Wh_GetIntSetting(L"items[%d].allowFolders", i);

    if ((!szLabel || *szLabel == L'\0') ||
        (!szCommand || *szCommand == L'\0')) {
      if (szLabel)
        Wh_FreeStringSetting(szLabel);
      if (szCommand)
        Wh_FreeStringSetting(szCommand);
      if (szExts)
        Wh_FreeStringSetting(szExts);
      break;
    }

    g_customCommands.push_back(
        {szLabel, szCommand, szExts ? szExts : L"", nAllowFolders != 0});

    Wh_FreeStringSetting(szLabel);
    Wh_FreeStringSetting(szCommand);
    if (szExts)
      Wh_FreeStringSetting(szExts);
  }
}

void ExecuteCustomCommand(int index, HWND hwnd) {
  std::wstring commandTemplate;
  {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    if (index >= 0 && index < (int)g_customCommands.size()) {
      commandTemplate = g_customCommands[index].command;
    }
  }
  if (commandTemplate.empty())
    return;

  std::vector<std::wstring> paths = GetSelectedPaths(hwnd);
  if (paths.empty())
    return;

  for (const auto &path : paths) {
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

LRESULT CALLBACK ExplorerMenuSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                          LPARAM lParam, UINT_PTR uIdSubclass,
                                          DWORD_PTR dwRefData) {
  if (uMsg == WM_COMMAND) {
    WORD wId = LOWORD(wParam);
    if (wId >= MIN_CUSTOM_ID) {
      std::vector<CustomCommand> activeCommands;
      {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        activeCommands = g_customCommands;
      }
      if (wId < MIN_CUSTOM_ID + activeCommands.size()) {
        ExecuteCustomCommand(wId - MIN_CUSTOM_ID, hWnd);
        return 0;
      }
    }
  } else if (uMsg == WM_NCDESTROY) {
    RemoveWindowSubclass(hWnd, ExplorerMenuSubclassProc, uIdSubclass);
  }
  return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

BOOL WINAPI HookedTrackPopupMenuEx(HMENU hMenu, UINT uFlags, int x, int y,
                                   HWND hwnd, LPTPMPARAMS lptpm) {
  std::vector<CustomCommand> activeCommands;
  {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    activeCommands = g_customCommands;
  }

  if (activeCommands.empty())
    return TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hwnd, lptpm);

  std::vector<std::wstring> selectedPaths = GetSelectedPaths(hwnd);
  std::wstring targetPath = selectedPaths.empty() ? L"" : selectedPaths[0];

  std::wstring targetExt = L"";
  size_t dotPos = targetPath.find_last_of(L".");
  size_t slashPos = targetPath.find_last_of(L"\\/");

  bool isFolder = false;
  if (dotPos == std::wstring::npos ||
      (slashPos != std::wstring::npos && dotPos < slashPos)) {
    isFolder = true;
  } else {
    targetExt = targetPath.substr(dotPos);
  }

  bool addedAny = false;
  for (size_t i = 0; i < activeCommands.size(); ++i) {
    const auto &cmd = activeCommands[i];

    if (isFolder && !cmd.allowFolders) {
      continue;
    }

    if (!cmd.extensions.empty()) {
      if (isFolder || targetExt.empty() ||
          !ContainsStringNoCase(cmd.extensions, targetExt)) {
        continue;
      }
    }

    if (!addedAny && GetMenuItemCount(hMenu) > 0) {
      AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
      addedAny = true;
    }

    AppendMenuW(hMenu, MF_STRING, MIN_CUSTOM_ID + i, cmd.label.c_str());
  }

  BOOL isSubclassed = FALSE;
  if (!(uFlags & TPM_RETURNCMD))
    isSubclassed = SetWindowSubclass(hwnd, ExplorerMenuSubclassProc,
                                     SUBCLASS_ID, (DWORD_PTR)hwnd);
  BOOL result = TrackPopupMenuEx_orig(hMenu, uFlags, x, y, hwnd, lptpm);
  if (isSubclassed && IsWindow(hwnd))
    RemoveWindowSubclass(hwnd, ExplorerMenuSubclassProc, SUBCLASS_ID);

  if ((uFlags & TPM_RETURNCMD) && result >= MIN_CUSTOM_ID &&
      result < MIN_CUSTOM_ID + (int)activeCommands.size()) {
    ExecuteCustomCommand(result - MIN_CUSTOM_ID, hwnd);
  }
  return result;
}

void Wh_ModSettingsChanged() { LoadSettings(); }

BOOL Wh_ModInit() {
  LoadSettings();

  if (!Wh_SetFunctionHook((void *)TrackPopupMenuEx,
                          (void *)HookedTrackPopupMenuEx,
                          (void **)&TrackPopupMenuEx_orig)) {
    return FALSE;
  }

  return TRUE;
}

void Wh_ModUninit() {}
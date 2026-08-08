// ==WindhawkMod==
// @id              explorer-gallery-photos-launcher
// @name            Gallery -> Photos App Launcher (ExplorerPatcher Win10 Fix)
// @description     ExplorerPatcher Win10 UI fix: intercepts Gallery navigation in File Explorer and launches the Microsoft Photos app (ms-photos:) instead of the built-in Gallery view.
// @version         2.0
// @author          Jäkubix
// @include         %SystemRoot%\explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lcomctl32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Gallery → Photos App Launcher (ExplorerPatcher Win10 Fix)

**A fix for ExplorerPatcher users running the Windows 10 Ribbon/UI overlay.**

Intercepts navigation to the "Gallery" folder in the Windows File Explorer navigation pane
and launches the Microsoft Photos app (`ms-photos:`) instead of opening the built-in Gallery view.

## Why is this needed?

Standard TreeView hooking methods (`WM_LBUTTONDOWN`, `TVN_SELCHANGING`, `SetWindowSubclass`)
do **not** work with ExplorerPatcher, because the Win10 UI overlay intercepts and handles
all mouse/keyboard events before they reach the underlying `SysTreeView32` control.

This mod bypasses the problem entirely by hooking the COM interface `IShellBrowser::BrowseObject`
directly via vtable patching. This is the single chokepoint through which **all** Explorer
navigation passes — regardless of which UI layer is active.

## Features

- Works with **ExplorerPatcher** (Win10 overlay) and without it
- Hooks at the COM level — completely UI-independent
- Configurable target CLSID and launch command
- Detailed debug logging via `OutputDebugString` (viewable in DbgView)

## How it works

1. Finds an open Explorer window (`CabinetWClass`)
2. Retrieves the `IShellBrowser` interface via the undocumented `WM_USER+7` message
3. Reads the vtable and hooks `BrowseObject` (vtable index 11)
4. When the target PIDL matches the Gallery CLSID, blocks navigation and launches `ms-photos:`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- settings:
  - targetClsid: "{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}"
  - targetCommand: "ms-photos:"
  - debugLog: true
  $name: Ustawienia
  $description: CLSID elementu do przechwycenia i komenda do uruchomienia.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <string>
#include <windows.h>

struct {
  std::wstring targetClsid;
  std::wstring targetCommand;
  bool debugLog;
} g_settings;

// ---- Typ i oryginał BrowseObject ----
typedef HRESULT(STDMETHODCALLTYPE *BrowseObject_t)(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags);
static BrowseObject_t g_BrowseObject_Original = nullptr;
static bool g_hooked = false;
static volatile bool g_stopWorker = false;
static HANDLE g_workerThread = NULL;

static void LoadSettings() {
  PCWSTR s = Wh_GetStringSetting(L"settings.targetClsid");
  g_settings.targetClsid =
      s && *s ? s : L"{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}";
  Wh_FreeStringSetting(s);

  s = Wh_GetStringSetting(L"settings.targetCommand");
  g_settings.targetCommand = s && *s ? s : L"ms-photos:";
  Wh_FreeStringSetting(s);

  g_settings.debugLog = !!Wh_GetIntSetting(L"settings.debugLog");
}

// Case-insensitive substring search
static bool contains_ci(const std::wstring &hay, const std::wstring &needle) {
  if (needle.empty())
    return false;
  std::wstring h = hay, n = needle;
  for (auto &c : h)
    c = towlower(c);
  for (auto &c : n)
    c = towlower(c);
  return h.find(n) != std::wstring::npos;
}

// Sprawdza czy dany PIDL odpowiada naszemu docelowemu CLSID
static bool IsTargetPidl(PCUIDLIST_RELATIVE pidl) {
  if (!pidl)
    return false;

  // Metoda 1: Konwertuj PIDL na ścieżkę tekstową i szukaj CLSID
  PWSTR pszName = nullptr;
  HRESULT hr = SHGetNameFromIDList(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl),
                                   SIGDN_DESKTOPABSOLUTEPARSING, &pszName);
  if (SUCCEEDED(hr) && pszName) {
    std::wstring name(pszName);
    CoTaskMemFree(pszName);

    if (g_settings.debugLog) {
      Wh_Log(L"[GalleryLauncher] BrowseObject PIDL parsing name: %s",
             name.c_str());
    }

    // Usuń klamry z CLSID do porównania
    std::wstring clsidNoBraces = g_settings.targetClsid;
    // Usuń { i }
    std::wstring clean;
    for (wchar_t c : clsidNoBraces) {
      if (c != L'{' && c != L'}')
        clean += c;
    }

    if (contains_ci(name, clean)) {
      return true;
    }
  }

  return false;
}

// Nasz hook na BrowseObject
static HRESULT STDMETHODCALLTYPE BrowseObject_Hook(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags) {
  if (g_settings.debugLog) {
    Wh_Log(L"[GalleryLauncher] BrowseObject called, wFlags=0x%X, pidl=%p",
           wFlags, pidl);
  }

  if (IsTargetPidl(pidl)) {
    Wh_Log(L"[GalleryLauncher] === PRZECHWYCONO GALERIE! Uruchamianie: %s ===",
           g_settings.targetCommand.c_str());
    ShellExecuteW(NULL, L"open", g_settings.targetCommand.c_str(), NULL, NULL,
                  SW_SHOWNORMAL);
    return S_OK; // Zwracamy sukces, ale NIE nawigujemy
  }

  return g_BrowseObject_Original(pThis, pidl, wFlags);
}

// ---- Znajdowanie okien Eksploratora ----
static HWND FindExplorerWindow() {
  HWND hwnd = NULL;
  DWORD myPid = GetCurrentProcessId();

  while ((hwnd = FindWindowExW(NULL, hwnd, L"CabinetWClass", NULL)) != NULL) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == myPid && IsWindowVisible(hwnd)) {
      return hwnd;
    }
  }
  return NULL;
}

// IShellBrowser vtable layout:
// Index 0:  QueryInterface
// Index 1:  AddRef
// Index 2:  Release
// Index 3:  GetWindow (IOleWindow)
// Index 4:  ContextSensitiveHelp (IOleWindow)
// Index 5:  InsertMenusSB
// Index 6:  SetMenuSB
// Index 7:  RemoveMenusSB
// Index 8:  SetStatusTextSB
// Index 9:  EnableModelessSB
// Index 10: TranslateAcceleratorSB
// Index 11: BrowseObject  <-- TO HOOKUJEMY
#define BROWSEOBJECT_VTABLE_INDEX 11

// Worker thread - keeps retrying until Explorer window appears and hook is installed
static DWORD WINAPI WorkerProc(LPVOID) {
  Wh_Log(L"[GalleryLauncher] Worker thread started, waiting for Explorer window...");

  while (!g_stopWorker && !g_hooked) {
    HWND hwndExplorer = FindExplorerWindow();

    if (!hwndExplorer) {
      // No Explorer window yet - wait and retry
      for (int ms = 0; ms < 2000 && !g_stopWorker; ms += 100) {
        Sleep(100);
      }
      continue;
    }

    Wh_Log(L"[GalleryLauncher] Found Explorer window: %p", hwndExplorer);

    // Get IShellBrowser via undocumented WM_USER+7 message
    IShellBrowser *pShellBrowser = reinterpret_cast<IShellBrowser *>(
        SendMessageW(hwndExplorer, WM_USER + 7, 0, 0));

    if (!pShellBrowser) {
      // Fallback: try ShellTabWindowClass
      HWND hwndTab =
          FindWindowExW(hwndExplorer, NULL, L"ShellTabWindowClass", NULL);
      if (hwndTab) {
        pShellBrowser = reinterpret_cast<IShellBrowser *>(
            SendMessageW(hwndTab, WM_USER + 7, 0, 0));
      }
    }

    if (!pShellBrowser) {
      Wh_Log(L"[GalleryLauncher] IShellBrowser not ready yet, retrying...");
      for (int ms = 0; ms < 2000 && !g_stopWorker; ms += 100) {
        Sleep(100);
      }
      continue;
    }

    Wh_Log(L"[GalleryLauncher] Got IShellBrowser: %p", pShellBrowser);

    // Read vtable
    void **vtable = *reinterpret_cast<void ***>(pShellBrowser);
    void *pBrowseObjectFunc = vtable[BROWSEOBJECT_VTABLE_INDEX];

    Wh_Log(L"[GalleryLauncher] BrowseObject function at: %p", pBrowseObjectFunc);

    // Hook BrowseObject
    if (Wh_SetFunctionHook(pBrowseObjectFunc, (void *)BrowseObject_Hook,
                           (void **)&g_BrowseObject_Original)) {
      if (Wh_ApplyHookOperations()) {
        g_hooked = true;
        Wh_Log(L"[GalleryLauncher] === HOOK INSTALLED SUCCESSFULLY! ===");
      } else {
        Wh_Log(L"[GalleryLauncher] Wh_ApplyHookOperations FAILED, retrying...");
      }
    } else {
      Wh_Log(L"[GalleryLauncher] Wh_SetFunctionHook FAILED, retrying...");
    }

    if (!g_hooked) {
      for (int ms = 0; ms < 3000 && !g_stopWorker; ms += 100) {
        Sleep(100);
      }
    }
  }

  Wh_Log(L"[GalleryLauncher] Worker thread finished (hooked=%s)", g_hooked ? L"YES" : L"NO");
  return 0;
}

BOOL Wh_ModInit() {
  LoadSettings();
  Wh_Log(L"[GalleryLauncher] Inicjalizacja moda v2.0");
  Wh_Log(L"[GalleryLauncher] Target CLSID: %s", g_settings.targetClsid.c_str());
  Wh_Log(L"[GalleryLauncher] Target Command: %s",
         g_settings.targetCommand.c_str());

  g_stopWorker = false;
  g_workerThread = CreateThread(nullptr, 0, WorkerProc, nullptr, 0, nullptr);

  return TRUE;
}

void Wh_ModUninit() {
  Wh_Log(L"[GalleryLauncher] Uninit");
  g_stopWorker = true;
  if (g_workerThread) {
    WaitForSingleObject(g_workerThread, 5000);
    CloseHandle(g_workerThread);
    g_workerThread = NULL;
  }
}

void Wh_ModSettingsChanged() { LoadSettings(); }

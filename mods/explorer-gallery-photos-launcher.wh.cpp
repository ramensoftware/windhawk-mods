// ==WindhawkMod==
// @id              explorer-gallery-photos-launcher
// @name            Open Photos App instead of Gallery
// @description     Intercepts Gallery navigation in File Explorer and launches the Microsoft Photos app (ms-photos:) instead.
// @version         2.0
// @author          Jäkubix
// @github          https://github.com/jakubix30
// @include         %SystemRoot%\explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid -lcomctl32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Open Photos App instead of Gallery

Intercepts navigation to the "Gallery" folder in the Windows File Explorer navigation pane
and launches the Microsoft Photos app (`ms-photos:`) instead of opening the built-in Gallery view.

## Features

- Works with **ExplorerPatcher** (Win10 Ribbon overlay fix) and standard Windows 11
- Hooks at the COM/Symbol level (`CShellBrowser::BrowseObject`) — completely UI-independent
- Configurable target CLSID and launch command
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- settings:
  - targetClsid: "{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}"
  - targetCommand: "ms-photos:"
  $name: Settings
  $name:pl-PL: Ustawienia
  $description: Target item CLSID to intercept and command to execute.
  $description:pl-PL: CLSID elementu do przechwycenia i komenda do uruchomienia.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <string>
#include <windows.h>
#include <windhawk_utils.h>

struct {
    std::wstring targetClsid;
    std::wstring targetCommand;
} g_settings;

static PIDLIST_ABSOLUTE g_targetPidl = nullptr;
static bool g_hookAttempted = false;

// ---- BrowseObject Original & Hook ----
typedef HRESULT(STDMETHODCALLTYPE *BrowseObject_t)(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags);
static BrowseObject_t g_BrowseObject_Original = nullptr;

static void FreeTargetPidl() {
    if (g_targetPidl) {
        CoTaskMemFree(g_targetPidl);
        g_targetPidl = nullptr;
    }
}

static void LoadSettings() {
    PCWSTR s = Wh_GetStringSetting(L"settings.targetClsid");
    g_settings.targetClsid =
        s && *s ? s : L"{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}";
    Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"settings.targetCommand");
    g_settings.targetCommand = s && *s ? s : L"ms-photos:";
    Wh_FreeStringSetting(s);

    // Re-parse target PIDL
    FreeTargetPidl();
    std::wstring parsingName = L"::" + g_settings.targetClsid;
    SHParseDisplayName(parsingName.c_str(), nullptr, &g_targetPidl, 0, nullptr);
}

// Substring search (case-insensitive) for module name check
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

// Asynchronous launch parameters
static WCHAR g_cmdToLaunch[MAX_PATH] = L"";
static HANDLE g_launchEvent = NULL;
static HANDLE g_workerThread = NULL;
static volatile bool g_stopWorker = false;

static DWORD WINAPI LaunchWorkerProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    while (!g_stopWorker) {
        DWORD waitRes = WaitForSingleObject(g_launchEvent, 500);
        if (waitRes == WAIT_OBJECT_0) {
            WCHAR cmd[MAX_PATH];
            cmd[0] = L'\0';
            wcsncpy_s(cmd, g_cmdToLaunch, _TRUNCATE);
            if (cmd[0] != L'\0') {
                Wh_Log(L"Launching app asynchronously: %s", cmd);
                ShellExecuteW(NULL, L"open", cmd, NULL, NULL, SW_SHOWNORMAL);
            }
        }
    }

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }
    return 0;
}

// Hook on BrowseObject
static HRESULT STDMETHODCALLTYPE BrowseObject_Hook(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags) {
    // Ignore relative or navigation-history PIDLs
    if (wFlags & (SBSP_RELATIVE | SBSP_PARENT | SBSP_NAVIGATEBACK | SBSP_NAVIGATEFORWARD)) {
        return g_BrowseObject_Original(pThis, pidl, wFlags);
    }

    if (g_targetPidl && pidl) {
        if (ILIsEqual(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl), g_targetPidl) ||
            ILIsParent(g_targetPidl, reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl), FALSE)) {
            
            Wh_Log(L"Intercepted Gallery navigation! Triggering async launch: %s",
                   g_settings.targetCommand.c_str());

            // Trigger async launch off the UI thread
            wcsncpy_s(g_cmdToLaunch, g_settings.targetCommand.c_str(), _TRUNCATE);
            SetEvent(g_launchEvent);

            return S_OK; // Intercepted, block original navigation
        }
    }

    return g_BrowseObject_Original(pThis, pidl, wFlags);
}

// Hook explorerframe.dll symbols
static void HookExplorerFrame(HMODULE hEF, bool applyNow) {
    if (!hEF || g_hookAttempted)
        return;

    g_hookAttempted = true;

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {
            {
                LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const __unaligned *,unsigned int))",
                LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const * __ptr64,unsigned int))",
                LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST const *,unsigned int))",
            },
            (void**)&g_BrowseObject_Original,
            (void*)BrowseObject_Hook,
        },
    };

    if (WindhawkUtils::HookSymbols(hEF, explorerframe_dll_hooks,
                                   ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Successfully registered BrowseObject hooks");
        if (applyNow) {
            if (!Wh_ApplyHookOperations()) {
                Wh_Log(L"Wh_ApplyHookOperations failed");
            }
        }
    }
}

// LoadLibraryExW hook to catch late loading of explorerframe.dll
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                          HANDLE hFile,
                                          DWORD dwFlags) {
    HMODULE hMod = g_LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (hMod && lpLibFileName) {
        if (contains_ci(lpLibFileName, L"explorerframe.dll")) {
            HookExplorerFrame(hMod, true);
        }
    }
    return hMod;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Initializing mod v2.0");

    // Create background worker thread for asynchronous protocol launching
    g_launchEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    g_stopWorker = false;
    g_workerThread = CreateThread(NULL, 0, LaunchWorkerProc, NULL, 0, NULL);

    // Hook LoadLibraryExW from kernelbase.dll
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(hKernelBase, "LoadLibraryExW");
        if (pLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook(pLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &g_LoadLibraryExW_Original);
        }
    }

    // If explorerframe.dll is already loaded, hook it immediately (applyNow = false because Windhawk applies after Init)
    HMODULE hEF = GetModuleHandleW(L"explorerframe.dll");
    if (hEF) {
        HookExplorerFrame(hEF, false);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing mod");

    g_stopWorker = true;
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = NULL;
    }

    if (g_launchEvent) {
        CloseHandle(g_launchEvent);
        g_launchEvent = NULL;
    }

    FreeTargetPidl();
}

void Wh_ModSettingsChanged() { LoadSettings(); }

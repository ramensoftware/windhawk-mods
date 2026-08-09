// ==WindhawkMod==
// @id                  explorer-gallery-photos-launcher
// @name                Open Photos App instead of Gallery
// @description         Intercepts Gallery navigation in File Explorer and launches the Microsoft Photos app (ms-photos:) instead.
// @version             2.0
// @author              Jäkubix
// @github              https://github.com/jakubix30
// @include             %SystemRoot%\explorer.exe
// @architecture        x86-64
// @compilerOptions     -lole32 -luuid -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Open Photos App instead of Gallery

Intercepts navigation to the "Gallery" folder in the Windows File Explorer navigation pane
and launches the Microsoft Photos app (`ms-photos:`) instead of opening the built-in Gallery view.

![Demonstration](https://raw.githubusercontent.com/jakubix30/explorer-gallery-photos-launcher/main/demo.gif)

## Features

- Works with **ExplorerPatcher** (Win10 Ribbon overlay fix) and standard Windows 11
- Hooks at the COM/Symbol level (`CShellBrowser::BrowseObject`) — completely UI-independent
- Configurable launch command (useful if you prefer a different photo viewer)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetCommand: "ms-photos:"
  $name: Command to launch
  $name:pl-PL: Komenda do uruchomienia
  $description: Protocol or program launched instead of navigating to the Gallery folder.
  $description:pl-PL: Protokół lub program uruchamiany zamiast nawigacji do folderu Galerii.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <shobjidl.h>
#include <string>
#include <windows.h>
#include <windhawk_utils.h>
#include <atomic>

struct {
    std::wstring targetCommand;
} g_settings;

static PIDLIST_ABSOLUTE g_targetPidl = nullptr;
static std::atomic<bool> g_explorerFrameHooked{false};

// ---- BrowseObject Original & Hook ----
typedef HRESULT(STDMETHODCALLTYPE *BrowseObject_t)(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags);
static BrowseObject_t g_BrowseObject_Original = nullptr;

static PIDLIST_ABSOLUTE EnsureTargetPidl() {
    if (!g_targetPidl) {
        // Hardcoded Gallery CLSID
        std::wstring parsingName = L"::{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}";
        HRESULT hr = SHParseDisplayName(parsingName.c_str(), nullptr,
                                        &g_targetPidl, 0, nullptr);
        if (FAILED(hr)) {
            Wh_Log(L"SHParseDisplayName(%s) failed: 0x%08X", parsingName.c_str(), (unsigned)hr);
        }
    }
    return g_targetPidl;
}

static void FreeTargetPidl() {
    if (g_targetPidl) {
        CoTaskMemFree(g_targetPidl);
        g_targetPidl = nullptr;
    }
}

static void LoadSettings() {
    PCWSTR s = Wh_GetStringSetting(L"targetCommand");
    g_settings.targetCommand = s && *s ? s : L"ms-photos:";
    Wh_FreeStringSetting(s);
}

// Asynchronous launch parameters
static WCHAR g_cmdToLaunch[MAX_PATH] = L"";
static HANDLE g_launchEvent = NULL;
static HANDLE g_stopEvent = NULL;
static HANDLE g_workerThread = NULL;

static DWORD WINAPI LaunchWorkerProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    HANDLE handles[] = {g_stopEvent, g_launchEvent};
    for (;;) {
        DWORD r = MsgWaitForMultipleObjectsEx(ARRAYSIZE(handles), handles, INFINITE,
                                              QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (r == WAIT_OBJECT_0) {
            break;  // stop requested
        }
        if (r == WAIT_OBJECT_0 + 1) {
            WCHAR cmd[MAX_PATH];
            cmd[0] = L'\0';
            wcsncpy_s(cmd, g_cmdToLaunch, _TRUNCATE);
            if (cmd[0] != L'\0') {
                Wh_Log(L"Launching app asynchronously: %s", cmd);
                
                SHELLEXECUTEINFOW sei = {sizeof(sei)};
                sei.fMask = SEE_MASK_ASYNCOK; // No SEE_MASK_FLAG_NO_UI so system missing-app prompts still appear
                sei.lpVerb = L"open";
                sei.lpFile = cmd;
                sei.nShow = SW_SHOWNORMAL;
                
                if (!ShellExecuteExW(&sei)) {
                    Wh_Log(L"ShellExecuteExW(%s) failed: %u", cmd, GetLastError());
                }
            }
        } else if (r == WAIT_OBJECT_0 + ARRAYSIZE(handles)) {
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        } else {
            break;  // WAIT_FAILED
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

    // Lazy load the PIDL on the UI thread to guarantee COM readiness
    PIDLIST_ABSOLUTE target = EnsureTargetPidl();

    if (target && pidl) {
        if (ILIsEqual(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl), target) ||
            ILIsParent(target, reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl), FALSE)) {
            
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
    if (!hEF || g_explorerFrameHooked.exchange(true))
        return;

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
    if (hMod && hMod == GetModuleHandleW(L"explorerframe.dll")) {
        HookExplorerFrame(hMod, true);
    }
    return hMod;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Initializing mod v2.0");

    // Create background worker thread for asynchronous protocol launching
    g_launchEvent = CreateEventW(NULL, FALSE, FALSE, NULL); // auto-reset
    g_stopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);    // manual-reset
    
    if (!g_launchEvent || !g_stopEvent) {
        Wh_Log(L"Failed to create thread synchronization events.");
        return FALSE;
    }

    g_workerThread = CreateThread(NULL, 0, LaunchWorkerProc, NULL, 0, NULL);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread.");
        return FALSE;
    }

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

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = NULL;
    }

    if (g_launchEvent) {
        CloseHandle(g_launchEvent);
        g_launchEvent = NULL;
    }
    
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = NULL;
    }

    FreeTargetPidl();
}

void Wh_ModSettingsChanged() { 
    LoadSettings(); 
}

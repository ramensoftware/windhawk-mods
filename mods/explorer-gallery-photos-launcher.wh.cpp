// ==WindhawkMod==
// @id                  explorer-gallery-photos-launcher
// @name                Open Photos App instead of Gallery
// @name:pl-PL          Otwieraj aplikację Zdjęcia zamiast Galerii
// @description         Intercepts Gallery navigation in File Explorer and launches the Microsoft Photos app (ms-photos:) instead.
// @description:pl-PL   Przechwytuje nawigację do Galerii w Eksploratorze plików i uruchamia zamiast niej aplikację Zdjęcia (ms-photos:).
// @version             2.0
// @author              Jäkubix
// @github              https://github.com/jakubix30
// @include             %SystemRoot%\explorer.exe
// @architecture        x86-64
// @compilerOptions     -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Open Photos App instead of Gallery

Intercepts navigation to the "Gallery" folder in the Windows File Explorer navigation pane
and launches the Microsoft Photos app (`ms-photos:`) instead of opening the built-in Gallery view.

*(Note: Only in-Explorer navigation is redirected. Opening Gallery from a pinned Start menu shortcut won't be intercepted. Using the Back/Forward history buttons to return to Gallery will also bypass the hook.)*

![Demonstration](https://raw.githubusercontent.com/jakubix30/explorer-gallery-photos-launcher/main/demo.gif)

## Features

- Works with **ExplorerPatcher** (Win10 Ribbon overlay fix) and standard Windows 11
- Hooks at the COM/Symbol level (`CShellBrowser::BrowseObject`) — completely UI-independent
- Configurable launch command and arguments (useful if you prefer a different photo viewer). 
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetCommand: "ms-photos:"
  $name: Command to launch
  $name:pl-PL: Komenda do uruchomienia
  $description: "Protocol or program launched instead of navigating to the Gallery folder."
  $description:pl-PL: Protokół lub program uruchamiany zamiast nawigacji do folderu Galerii.
- targetArguments: ""
  $name: Command arguments
  $name:pl-PL: Argumenty komendy
  $description: "Optional arguments to pass to the command (only applies when the command is an executable, not a protocol)."
  $description:pl-PL: Opcjonalne argumenty przekazywane do komendy.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <atomic>

static std::atomic<PIDLIST_ABSOLUTE> g_targetPidl{nullptr};
static std::atomic<bool> g_targetPidlFailed{false};
static std::atomic<bool> g_explorerFrameHooked{false};
static std::atomic<bool> g_unloading{false};

// ---- BrowseObject Original & Hook ----
typedef HRESULT(STDMETHODCALLTYPE *BrowseObject_t)(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags);
static BrowseObject_t g_BrowseObject_Original = nullptr;

static PIDLIST_ABSOLUTE EnsureTargetPidl() {
    PIDLIST_ABSOLUTE pidl = g_targetPidl.load(std::memory_order_acquire);
    if (pidl) {
        return pidl;
    }

    // Don't retry if a previous attempt already failed
    if (g_targetPidlFailed.load(std::memory_order_acquire)) {
        return nullptr;
    }

    constexpr WCHAR kGalleryParsingName[] = L"::{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}";
    PIDLIST_ABSOLUTE newPidl = nullptr;
    HRESULT hr = SHParseDisplayName(kGalleryParsingName, nullptr, &newPidl, 0, nullptr);
    if (FAILED(hr) || !newPidl) {
        Wh_Log(L"SHParseDisplayName failed: 0x%08X", (unsigned)hr);
        g_targetPidlFailed.store(true, std::memory_order_release);
        return nullptr;
    }

    PIDLIST_ABSOLUTE expected = nullptr;
    if (!g_targetPidl.compare_exchange_strong(expected, newPidl,
                                              std::memory_order_release,
                                              std::memory_order_acquire)) {
        CoTaskMemFree(newPidl);  // lost the race
        return expected;
    }
    return newPidl;
}

// Asynchronous launch parameters — created once in Wh_ModInit,
// so CleanupResources (on the Windhawk engine thread) never races
// with a half-initialized state from the hook's UI thread.
static HANDLE g_launchEvent = NULL;
static HANDLE g_stopEvent = NULL;
static HANDLE g_workerThread = NULL;

static void CleanupResources() {
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
}

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
            // Don't start a new launch if a stop was requested in the meantime.
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                break;
            }

            // Read target dynamically, preventing use-after-free and threading issues.
            WindhawkUtils::StringSetting cmd = WindhawkUtils::StringSetting::make(L"targetCommand");
            WindhawkUtils::StringSetting args = WindhawkUtils::StringSetting::make(L"targetArguments");
            
            PCWSTR target = cmd.get()[0] ? cmd.get() : L"ms-photos:";
            PCWSTR targetArgs = args.get()[0] ? args.get() : nullptr;
            
            Wh_Log(L"Launching app asynchronously: %s %s", target, targetArgs ? targetArgs : L"");
            
            SHELLEXECUTEINFOW sei = {sizeof(sei)};
            // SEE_MASK_ASYNCOK hints that the shell may offload slow work (e.g. UAC) to
            // another thread, but the documentation notes this is not guaranteed in all
            // cases. SEE_MASK_FLAG_NO_UI ensures the shell fails silently and returns
            // immediately if the target is invalid, rather than hanging the worker thread
            // with a modal error dialog which would block Wh_ModUninit indefinitely.
            sei.fMask = SEE_MASK_ASYNCOK | SEE_MASK_FLAG_NO_UI;
            sei.lpVerb = nullptr; // Default verb is used for maximum compatibility
            sei.lpFile = target;
            sei.lpParameters = targetArgs;
            sei.nShow = SW_SHOWNORMAL;
            
            if (!ShellExecuteExW(&sei)) {
                Wh_Log(L"ShellExecuteExW(%s) failed: %u", target, GetLastError());
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

static bool StartWorkerThread() {
    g_launchEvent = CreateEventW(NULL, FALSE, FALSE, NULL); // auto-reset
    g_stopEvent = CreateEventW(NULL, TRUE, FALSE, NULL);    // manual-reset

    if (!g_launchEvent || !g_stopEvent) {
        Wh_Log(L"Failed to create thread synchronization events.");
        return false;
    }

    g_workerThread = CreateThread(NULL, 0, LaunchWorkerProc, NULL, 0, NULL);
    if (!g_workerThread) {
        Wh_Log(L"Failed to create worker thread.");
        return false;
    }

    return true;
}

// Hook on BrowseObject
static HRESULT STDMETHODCALLTYPE BrowseObject_Hook(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags) {
    // Ignore relative, navigation-history, or NULL PIDLs early
    if (!pidl || (wFlags & (SBSP_RELATIVE | SBSP_PARENT | SBSP_NAVIGATEBACK | SBSP_NAVIGATEFORWARD))) {
        return g_BrowseObject_Original(pThis, pidl, wFlags);
    }

    // Lazy load the Gallery PIDL on the UI thread to guarantee COM readiness
    PIDLIST_ABSOLUTE target = EnsureTargetPidl();

    if (target && ILIsEqual(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl), target)) {
        
        Wh_Log(L"Intercepted Gallery navigation! Triggering async launch.");
        SetEvent(g_launchEvent);

        // Block original navigation without tricking the address bar into switching state
        return HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }

    return g_BrowseObject_Original(pThis, pidl, wFlags);
}

// Hook explorerframe.dll symbols
static bool HookExplorerFrame(HMODULE hEF, bool applyNow) {
    // Bail out if module is invalid or mod is unloading
    if (!hEF || g_unloading.load(std::memory_order_acquire)) return false;

    bool expected = false;
    // Fast path: bail if we already successfully hooked or attempted to hook.
    if (!g_explorerFrameHooked.compare_exchange_strong(expected, true)) {
        return true;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {
            {
                LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const __unaligned *,unsigned int))",
                LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST_RELATIVE const * __ptr64,unsigned int))",
                LR"(public: virtual long __cdecl CShellBrowser::BrowseObject(struct _ITEMIDLIST const *,unsigned int))",
            },
            &g_BrowseObject_Original,
            BrowseObject_Hook,
        },
    };

    if (WindhawkUtils::HookSymbols(hEF, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Successfully registered BrowseObject hooks");
        if (applyNow) {
            if (!Wh_ApplyHookOperations()) {
                Wh_Log(L"Wh_ApplyHookOperations failed");
            }
        }
        return true;
    }

    // Hook failed - latch remains true to prevent repeated HookSymbols failure retries
    Wh_Log(L"Failed to hook ExplorerFrame symbols. Mod will remain inactive until restarted/reloaded.");
    return false;
}

// LoadLibraryExW hook to catch late loading of explorerframe.dll
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t g_LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                          HANDLE hFile,
                                          DWORD dwFlags) {
    HMODULE hMod = g_LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    
    // Check if explorerframe.dll is now available (whether it was the direct target or an indirect dependency)
    if (hMod && !g_unloading.load(std::memory_order_acquire) &&
        !g_explorerFrameHooked.load(std::memory_order_relaxed)) {
        if (HMODULE hEF = GetModuleHandleW(L"explorerframe.dll")) {
            HookExplorerFrame(hEF, true);
        }
    }
    
    return hMod;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing mod v" WH_MOD_VERSION);

    // Create the worker thread up front so that BrowseObject_Hook never
    // touches these globals — CleanupResources on the engine thread is
    // then race-free.  One idle waiting thread is cheap.
    if (!StartWorkerThread()) {
        CleanupResources();
        return FALSE;
    }

    bool delayLoadingNeeded = true;
    
    // If explorerframe.dll is already loaded, hook it immediately
    HMODULE hEF = GetModuleHandleW(L"explorerframe.dll");
    if (hEF) {
        if (HookExplorerFrame(hEF, false)) {
            delayLoadingNeeded = false;
        } else {
            CleanupResources();
            return FALSE;
        }
    }

    if (delayLoadingNeeded) {
        // Hook LoadLibraryExW from kernelbase.dll to monitor for explorerframe.dll late-loading
        HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
        if (hKernelBase) {
            auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(hKernelBase, "LoadLibraryExW");
            if (pLoadLibraryExW) {
                WindhawkUtils::SetFunctionHook(pLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &g_LoadLibraryExW_Original);
            }
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    // Catch cases where explorerframe.dll loaded during mod initialization
    HMODULE hEF = GetModuleHandleW(L"explorerframe.dll");
    if (hEF) {
        HookExplorerFrame(hEF, true);
    }
}

void Wh_ModBeforeUninit() {
    // Let hooks know we are shutting down to prevent a late-loading race condition
    g_unloading.store(true, std::memory_order_release);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing mod");

    CleanupResources();

    PIDLIST_ABSOLUTE target = g_targetPidl.exchange(nullptr, std::memory_order_acquire);
    if (target) {
        CoTaskMemFree(target);
    }
}

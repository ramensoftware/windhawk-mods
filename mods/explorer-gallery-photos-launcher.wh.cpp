// ==WindhawkMod==
// @id              explorer-gallery-photos-launcher
// @name            Gallery -> Photos App Launcher (ExplorerPatcher Win10 Fix)
// @description     ExplorerPatcher Win10 UI fix: intercepts Gallery navigation in File Explorer and launches the Microsoft Photos app (ms-photos:) instead of the built-in Gallery view.
// @version         2.0
// @author          Jäkubix
// @github          https://github.com/jakubix30
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

This mod bypasses the problem entirely by hooking `CShellBrowser::BrowseObject` in `explorerframe.dll`.
This is the single chokepoint through which **all** Explorer navigation passes — regardless of which UI layer is active.

## Features

- Works with **ExplorerPatcher** (Win10 overlay) and without it
- Hooks at the COM/Symbol level — completely UI-independent
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

// ---- BrowseObject Original & Hook ----
typedef HRESULT(STDMETHODCALLTYPE *BrowseObject_t)(void *pThis,
                                                   PCUIDLIST_RELATIVE pidl,
                                                   UINT wFlags);
static BrowseObject_t g_BrowseObject_Original = nullptr;

static void LoadSettings() {
    PCWSTR s = Wh_GetStringSetting(L"settings.targetClsid");
    g_settings.targetClsid =
        s && *s ? s : L"{e88865ea-0e1c-4e20-9aa6-edcd0212c87c}";
    Wh_FreeStringSetting(s);

    s = Wh_GetStringSetting(L"settings.targetCommand");
    g_settings.targetCommand = s && *s ? s : L"ms-photos:";
    Wh_FreeStringSetting(s);
}

// Substring search (case-insensitive)
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

// Checks if the given PIDL matches our target CLSID
static bool IsTargetPidl(PCUIDLIST_RELATIVE pidl) {
    if (!pidl)
        return false;

    PWSTR pszName = nullptr;
    HRESULT hr = SHGetNameFromIDList(reinterpret_cast<PCIDLIST_ABSOLUTE>(pidl),
                                     SIGDN_DESKTOPABSOLUTEPARSING, &pszName);
    if (SUCCEEDED(hr) && pszName) {
        std::wstring name(pszName);
        CoTaskMemFree(pszName);

        Wh_Log(L"BrowseObject PIDL parsing name: %s", name.c_str());

        // Remove braces for comparison
        std::wstring clean;
        for (wchar_t c : g_settings.targetClsid) {
            if (c != L'{' && c != L'}')
                clean += c;
        }

        if (contains_ci(name, clean)) {
            return true;
        }
    }

    return false;
}

// Asynchronous launch parameters
static WCHAR g_cmdToLaunch[MAX_PATH] = L"";
static HANDLE g_launchEvent = NULL;
static HANDLE g_workerThread = NULL;
static volatile bool g_stopWorker = false;

static DWORD WINAPI LaunchWorkerProc(LPVOID) {
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

    Wh_Log(L"BrowseObject called, wFlags=0x%X", wFlags);

    if (IsTargetPidl(pidl)) {
        Wh_Log(L"Intercepted Gallery navigation! Triggering async launch: %s",
               g_settings.targetCommand.c_str());

        // Trigger async launch off the UI thread
        wcsncpy_s(g_cmdToLaunch, g_settings.targetCommand.c_str(), _TRUNCATE);
        SetEvent(g_launchEvent);

        return S_OK; // Intercepted, block original navigation
    }

    return g_BrowseObject_Original(pThis, pidl, wFlags);
}

// Hook explorerframe.dll symbols
static void HookExplorerFrame(HMODULE hEF) {
    if (!hEF || g_BrowseObject_Original != nullptr)
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
        Wh_Log(L"Successfully hooked BrowseObject in explorerframe.dll!");
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
            Wh_Log(L"explorerframe.dll loaded dynamically, applying symbol hooks...");
            HookExplorerFrame(hMod);
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

    // Hook LoadLibraryExW from kernelbase.dll to catch dynamic loading
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(hKernelBase, "LoadLibraryExW");
        if (pLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook(pLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &g_LoadLibraryExW_Original);
        }
    }

    // Check if explorerframe.dll is already loaded or force load it to hook immediately
    HMODULE hEF = GetModuleHandleW(L"explorerframe.dll");
    if (!hEF) {
        hEF = LoadLibraryW(L"explorerframe.dll");
    }

    if (hEF) {
        HookExplorerFrame(hEF);
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
}

void Wh_ModSettingsChanged() { LoadSettings(); }

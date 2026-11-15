// ==WindhawkMod==
// @id              taskbar-hung-rearrangement-fix
// @name            Taskbar hung windows rearrangement fix
// @description     Fixes a taskbar bug which causes taskbar items of hung windows to move to the end of the taskbar
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar hung windows rearrangement fix

Fixes a taskbar bug which causes taskbar items of hung windows to move to the
end of the taskbar. For more details about the bug, check out the following blog
post: [Hung windows and taskbar buttons
rearrangement](https://ramensoftware.com/hung-windows-and-taskbar-buttons-rearrangement).

Only Windows 10 64-bit and Windows 11 are supported. For other Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

![Demonstration](https://i.imgur.com/8WU4YCX.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>

#include <atomic>

struct {
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

bool IsGhostWindowClass(HWND hWnd) {
    static ATOM ghostAtom;

    if (!ghostAtom) {
        WNDCLASS wndClass;
        ghostAtom = (ATOM)GetClassInfo(NULL, L"Ghost", &wndClass);
        if (!ghostAtom) {
            return false;
        }
    }

    ATOM atom = (ATOM)GetClassLong(hWnd, GCW_ATOM);
    return atom && atom == ghostAtom;
}

HWND MyGhostWindowFromHungWindow(HWND hwndHung) {
    using GhostWindowFromHungWindow_t = HWND(WINAPI*)(HWND hwndHung);
    static GhostWindowFromHungWindow_t pGhostWindowFromHungWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GhostWindowFromHungWindow_t)GetProcAddress(
                hUser32, "GhostWindowFromHungWindow");
        }

        return (GhostWindowFromHungWindow_t) nullptr;
    }();

    if (!pGhostWindowFromHungWindow) {
        return nullptr;
    }

    return pGhostWindowFromHungWindow(hwndHung);
}

HWND MyHungWindowFromGhostWindow(HWND hwndGhost) {
    using HungWindowFromGhostWindow_t = HWND(WINAPI*)(HWND hwndGhost);
    static HungWindowFromGhostWindow_t pHungWindowFromGhostWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (HungWindowFromGhostWindow_t)GetProcAddress(
                hUser32, "HungWindowFromGhostWindow");
        }

        return (HungWindowFromGhostWindow_t) nullptr;
    }();

    if (!pHungWindowFromGhostWindow) {
        return nullptr;
    }

    return pHungWindowFromGhostWindow(hwndGhost);
}

using CTaskBand__HandleReplaceWindow_t = LRESULT(WINAPI*)(void* pThis,
                                                          HWND hFromWnd,
                                                          HWND hToWnd);
CTaskBand__HandleReplaceWindow_t CTaskBand__HandleReplaceWindow_Original;
LRESULT WINAPI CTaskBand__HandleReplaceWindow_Hook(void* pThis,
                                                   HWND hFromWnd,
                                                   HWND hToWnd) {
    Wh_Log(L">");

    if (hToWnd && !IsWindow(hToWnd)) {
        return 0;
    }

    LRESULT ret =
        CTaskBand__HandleReplaceWindow_Original(pThis, hFromWnd, hToWnd);

    return ret;
}

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow_Original;

using CWindowTaskItem_SetWindow_t = HRESULT(WINAPI*)(void* pThis, HWND hWnd);
CWindowTaskItem_SetWindow_t CWindowTaskItem_SetWindow_Original;

HWND WINAPI CWindowTaskItem_GetWindow_Hook(void* pThis) {
    HWND ret = CWindowTaskItem_GetWindow_Original(pThis);

    if ((LONG_PTR)ret & 1) {
        Wh_Log(L">");

        ret = (HWND)((LONG_PTR)ret & ~1);

        HWND hGhostWnd = MyGhostWindowFromHungWindow(ret);
        if (!hGhostWnd) {
            CWindowTaskItem_SetWindow_Original(pThis, ret);
        } else {
            ret = hGhostWnd;
        }
    }

    return ret;
}

HRESULT WINAPI CWindowTaskItem_SetWindow_Hook(void* pThis, HWND hWnd) {
    HWND hOldWnd = CWindowTaskItem_GetWindow_Original(pThis);

    if (hWnd != hOldWnd && IsGhostWindowClass(hWnd) &&
        MyHungWindowFromGhostWindow(hWnd) == hOldWnd) {
        Wh_Log(L">");

        hWnd = (HWND)((LONG_PTR)hOldWnd | 1);
    }

    HRESULT ret = CWindowTaskItem_SetWindow_Original(pThis, hWnd);

    return ret;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    struct EXPLORER_PATCHER_HOOK {
        PCSTR symbol;
        void** pOriginalFunction;
        void* hookFunction = nullptr;
        bool optional = false;
    };

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?_HandleReplaceWindow@CTaskBand@@IEAA_JPEAUHWND__@@0@Z)",
         (void**)&CTaskBand__HandleReplaceWindow_Original,
         (void*)CTaskBand__HandleReplaceWindow_Hook},
        {R"(?GetWindow@CWindowTaskItem@@UEAAPEAUHWND__@@XZ)",
         (void**)&CWindowTaskItem_GetWindow_Original,
         (void*)CWindowTaskItem_GetWindow_Hook},
        {R"(?SetWindow@CWindowTaskItem@@UEAAJPEAUHWND__@@@Z)",
         (void**)&CWindowTaskItem_SetWindow_Original,
         (void*)CWindowTaskItem_SetWindow_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool HandleModuleIfExplorerPatcher(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) !=
        0) {
        return true;
    }

    Wh_Log(L"ExplorerPatcher taskbar loaded: %s", moduleFileName);
    return HookExplorerPatcherSymbols(module);
}

void HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            HandleModuleIfExplorerPatcher(hMods[i]);
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        HandleModuleIfExplorerPatcher(module);
    }

    return module;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibrary(L"taskbar.dll");
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(protected: __int64 __cdecl CTaskBand::_HandleReplaceWindow(struct HWND__ *,struct HWND__ *))"},
            &CTaskBand__HandleReplaceWindow_Original,
            CTaskBand__HandleReplaceWindow_Hook,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow_Original,
            CWindowTaskItem_GetWindow_Hook,
        },
        {
            {LR"(public: virtual long __cdecl CWindowTaskItem::SetWindow(struct HWND__ *))"},
            &CWindowTaskItem_SetWindow_Original,
            CWindowTaskItem_SetWindow_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void LoadSettings() {
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    HandleLoadedExplorerPatcher();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    FARPROC pKernelBaseLoadLibraryExW =
        GetProcAddress(kernelBaseModule, "LoadLibraryExW");
    Wh_SetFunctionHook((void*)pKernelBaseLoadLibraryExW,
                       (void*)LoadLibraryExW_Hook,
                       (void**)&LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}

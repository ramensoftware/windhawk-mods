// ==WindhawkMod==
// @id              taskbar-thumbnails
// @name            Disable Taskbar Thumbnails
// @description     Disable taskbar thumbnails on hover, or replace them with a list
// @version         1.1
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
# Disable Taskbar Thumbnails

Disable taskbar thumbnails on hover, or replace them with a list.

Only Windows 10 64-bit and Windows 11 are supported. For other Windows versions
check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

**Note:** To customize the old taskbar on Windows 11 (if using ExplorerPatcher
or a similar tool), enable the relevant option in the mod's settings.

![Demonstration](https://i.imgur.com/62DSgxs.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: disabled
  $name: Preview on hover
  $options:
  - disabled: Disabled
  - list: List
  - thumbnails: Thumbnails
- noTooltips: false
  $name: Disable tooltips on hover
  $description: >-
    Only works for classic thumbnail previews, not for the new Windows 11
    implementation of thumbnail previews
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

enum class Mode {
    disabled,
    list,
    thumbnails,
};

struct {
    Mode mode;
    bool noTooltips;
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

using HoverFlyoutModel_TransitionToFlyoutVisibleState_t =
    void(WINAPI*)(void* pThis, void* param1);
HoverFlyoutModel_TransitionToFlyoutVisibleState_t
    HoverFlyoutModel_TransitionToFlyoutVisibleState_Original;
void WINAPI HoverFlyoutModel_TransitionToFlyoutVisibleState_Hook(void* pThis,
                                                                 void* param1) {
    Wh_Log(L">");

    if (g_settings.mode == Mode::disabled) {
        return;
    }

    HoverFlyoutModel_TransitionToFlyoutVisibleState_Original(pThis, param1);
}

using FlyoutFrame_CanFitAndUpdateScaleFactor_t = bool(WINAPI*)(void* pThis,
                                                               void* param1);
FlyoutFrame_CanFitAndUpdateScaleFactor_t
    FlyoutFrame_CanFitAndUpdateScaleFactor_Original;
bool WINAPI FlyoutFrame_CanFitAndUpdateScaleFactor_Hook(void* pThis,
                                                        void* param1) {
    Wh_Log(L">");

    if (g_settings.mode == Mode::list) {
        return false;
    }

    return FlyoutFrame_CanFitAndUpdateScaleFactor_Original(pThis, param1);
}

using CTaskListWnd__DisplayExtendedUI_t = HRESULT(WINAPI*)(void* pThis,
                                                           void* taskBtnGroup,
                                                           int param2,
                                                           DWORD flags,
                                                           int param4);
CTaskListWnd__DisplayExtendedUI_t CTaskListWnd__DisplayExtendedUI_Original;
HRESULT WINAPI CTaskListWnd__DisplayExtendedUI_Hook(void* pThis,
                                                    void* taskBtnGroup,
                                                    int param2,
                                                    DWORD flags,
                                                    int param4) {
    Wh_Log(L"> %x", flags);

    bool persistent = flags & 2;
    if (!persistent && g_settings.mode == Mode::disabled) {
        return S_OK;
    }

    HRESULT ret = CTaskListWnd__DisplayExtendedUI_Original(
        pThis, taskBtnGroup, param2, flags, param4);

    return ret;
}

using CTaskListThumbnailWnd__CanShowThumbnails_t = BOOL(WINAPI*)(void* pThis,
                                                                 void* param1,
                                                                 int param2,
                                                                 int param3);
CTaskListThumbnailWnd__CanShowThumbnails_t
    CTaskListThumbnailWnd__CanShowThumbnails_Original;
BOOL WINAPI CTaskListThumbnailWnd__CanShowThumbnails_Hook(void* pThis,
                                                          void* param1,
                                                          int param2,
                                                          int param3) {
    Wh_Log(L">");

    if (g_settings.mode == Mode::list) {
        return FALSE;
    }

    BOOL ret = CTaskListThumbnailWnd__CanShowThumbnails_Original(
        pThis, param1, param2, param3);

    return ret;
}

using CTaskListWnd__ShowToolTip_t = void(WINAPI*)(void* pThis, DWORD flags);
CTaskListWnd__ShowToolTip_t CTaskListWnd__ShowToolTip_Original;
void WINAPI CTaskListWnd__ShowToolTip_Hook(void* pThis, DWORD flags) {
    Wh_Log(L"> %x", flags);

    if (g_settings.noTooltips) {
        return;
    }

    CTaskListWnd__ShowToolTip_Original(pThis, flags);
}

bool HookTaskbarViewDllSymbols() {
    WCHAR dllPath[MAX_PATH];
    if (!GetWindowsDirectory(dllPath, ARRAYSIZE(dllPath))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    wcscat_s(
        dllPath, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");

    HMODULE module =
        LoadLibraryEx(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module) {
        Wh_Log(L"Taskbar view module couldn't be loaded");
        return false;
    }

    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutVisibleState(struct winrt::hstring))"},
            &HoverFlyoutModel_TransitionToFlyoutVisibleState_Original,
            HoverFlyoutModel_TransitionToFlyoutVisibleState_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
        {
            {LR"(private: bool __cdecl winrt::Taskbar::implementation::FlyoutFrame::CanFitAndUpdateScaleFactor(struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::Foundation::IInspectable> const &))"},
            &FlyoutFrame_CanFitAndUpdateScaleFactor_Original,
            FlyoutFrame_CanFitAndUpdateScaleFactor_Hook,
            true,  // New XAML thumbnails, enabled in late Windows 11 24H2.
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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
            {LR"(protected: long __cdecl CTaskListWnd::_DisplayExtendedUI(struct ITaskBtnGroup *,int,unsigned long,int))"},
            &CTaskListWnd__DisplayExtendedUI_Original,
            CTaskListWnd__DisplayExtendedUI_Hook,
        },
        {
            {LR"(private: int __cdecl CTaskListThumbnailWnd::_CanShowThumbnails(class CDPA<struct ITaskThumbnail,class CTContainer_PolicyUnOwned<struct ITaskThumbnail> > const *,int,int))"},
            &CTaskListThumbnailWnd__CanShowThumbnails_Original,
            CTaskListThumbnailWnd__CanShowThumbnails_Hook,
        },
        {
            {LR"(protected: void __cdecl CTaskListWnd::_ShowToolTip(enum ShowToolTipFlags))"},
            &CTaskListWnd__ShowToolTip_Original,
            CTaskListWnd__ShowToolTip_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?_DisplayExtendedUI@CTaskListWnd@@IEAAJPEAUITaskBtnGroup@@HKH@Z)",
         &CTaskListWnd__DisplayExtendedUI_Original,
         CTaskListWnd__DisplayExtendedUI_Hook},
        {R"(?_CanShowThumbnails@CTaskListThumbnailWnd@@AEAAHPEBV?$CDPA@UITaskThumbnail@@V?$CTContainer_PolicyUnOwned@UITaskThumbnail@@@@@@HH@Z)",
         &CTaskListThumbnailWnd__CanShowThumbnails_Original,
         CTaskListThumbnailWnd__CanShowThumbnails_Hook},
        {R"(?_ShowToolTip@CTaskListWnd@@IEAAXW4ShowToolTipFlags@@@Z)",
         &CTaskListWnd__ShowToolTip_Original, CTaskListWnd__ShowToolTip_Hook},
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

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
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

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }

    return module;
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = Mode::disabled;
    if (wcscmp(mode, L"list") == 0) {
        g_settings.mode = Mode::list;
    } else if (wcscmp(mode, L"thumbnails") == 0) {
        g_settings.mode = Mode::thumbnails;
    }
    Wh_FreeStringSetting(mode);

    g_settings.noTooltips = Wh_GetIntSetting(L"noTooltips");

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
    } else if (g_winVersion >= WinVersion::Win11) {
        // For the new XAML thumbnail.
        if (g_winVersion >= WinVersion::Win11_24H2 &&
            !HookTaskbarViewDllSymbols()) {
            return FALSE;
        }

        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    } else {
        if (!HookTaskbarSymbols()) {
            return FALSE;
        }
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

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

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}

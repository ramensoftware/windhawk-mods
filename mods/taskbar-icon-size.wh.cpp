// ==WindhawkMod==
// @id              taskbar-icon-size
// @name            Large Taskbar Icons
// @description     Make the taskbar icons large and crisp, or small and compact (Windows 11 only)
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Large Taskbar Icons
Make the taskbar icons large and crisp, or small and compact.

By default, the Windows 11 taskbar shows taskbar icons with the 24x24 size.
Since icons in Windows are either 16x16 or 32x32, the 24x24 icons are downscaled
versions of the 32x32 variants, which makes them blurry. This mods allows to
change the size of icons, and so the original quality 32x32 icons can be used,
as well as any other icon size.

Before - blurry 24x24 icons:

![Before screenshot](https://i.imgur.com/CILOJ7M.png)

After - original quality 32x32 icons:

![After screenshot](https://i.imgur.com/Tu4DQT3.png)

Only Windows 11 is supported. For older Windows versions check out [7+ Taskbar
Tweaker](https://tweaker.ramensoftware.com/).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- IconSize: 32
  $name: Icon size
  $description: >-
    The size, in pixels, of icons on the taskbar (Windows 11 default: 24)
- TaskbarHeight: 56
  $name: Taskbar height
  $description: >-
    The height, in pixels, of the taskbar (Windows 11 default: 48)
*/
// ==/WindhawkModSettings==

#include <initguid.h>  // must come before knownfolders.h
#include <knownfolders.h>
#include <shlobj.h>

#include <regex>

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

struct {
    int iconSize;
    int taskbarHeight;
} g_settings;

bool g_applyingSettings = false;
bool g_unloading = false;

double* pOriginal_double_48_value;

using IconUtils_GetIconSize_t = void(WINAPI*)(bool small, int type, SIZE* size);
IconUtils_GetIconSize_t pOriginal_IconUtils_GetIconSize;
void WINAPI IconUtils_GetIconSize_Hook(bool small, int type, SIZE* size) {
    pOriginal_IconUtils_GetIconSize(small, type, size);

    if (!g_unloading && !small) {
        size->cx = MulDiv(size->cx, g_settings.iconSize, 24);
        size->cy = MulDiv(size->cy, g_settings.iconSize, 24);
    }
}

using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    pOriginal_IconContainer_IsStorageRecreationRequired;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    if (g_applyingSettings) {
        return true;
    }

    return pOriginal_IconContainer_IsStorageRecreationRequired(pThis, param1,
                                                               flags);
}

using TaskListItemViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                           void* param1,
                                                           double* iconHeight);
TaskListItemViewModel_GetIconHeight_t
    pOriginal_TaskListItemViewModel_GetIconHeight;
int WINAPI TaskListItemViewModel_GetIconHeight_Hook(void* pThis,
                                                    void* param1,
                                                    double* iconHeight) {
    int ret = pOriginal_TaskListItemViewModel_GetIconHeight(pThis, param1,
                                                            iconHeight);

    if (!g_unloading) {
        *iconHeight = g_settings.iconSize;
    }

    return ret;
}

using TaskbarConfiguration_GetIconHeightInViewPixels_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetIconHeightInViewPixels_t
    pOriginal_TaskbarConfiguration_GetIconHeightInViewPixels;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_Hook(int enumTaskbarSize) {
    if (!g_unloading) {
        return g_settings.iconSize;
    }

    return TaskbarConfiguration_GetIconHeightInViewPixels_Hook(enumTaskbarSize);
}

void LoadSettings() {
    g_settings.iconSize = Wh_GetIntSetting(L"IconSize");
    g_settings.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
}

void FreeSettings() {
    // Nothing for now.
}

void ApplySettings() {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd) {
        return;
    }

    g_applyingSettings = true;

    double currentTaskbarHeight = *pOriginal_double_48_value;
    double newTaskbarHeight = g_unloading ? 48 : g_settings.taskbarHeight;

    DWORD oldProtect;
    VirtualProtect(pOriginal_double_48_value,
                   sizeof(*pOriginal_double_48_value), PAGE_READWRITE,
                   &oldProtect);

    // If the height doesn't change, temporarily change it to zero to force a UI
    // refresh.
    if (newTaskbarHeight == currentTaskbarHeight) {
        *pOriginal_double_48_value = 0;

        // Trigger TrayUI::_HandleSettingChange.
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);

        // Wait for the change to apply.
        int counter = 0;
        RECT rc;
        while (GetWindowRect(hTaskbarWnd, &rc) && rc.bottom > rc.top) {
            if (++counter >= 100) {
                break;
            }
            Sleep(100);
        }
    }

    *pOriginal_double_48_value = newTaskbarHeight;
    VirtualProtect(pOriginal_double_48_value,
                   sizeof(*pOriginal_double_48_value), oldProtect, &oldProtect);

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
            // Trigger CTaskBand::_HandleSyncDisplayChange.
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }

    g_applyingSettings = false;
}

struct SYMBOL_HOOK {
    std::wregex symbolRegex;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount) {
    WH_FIND_SYMBOL symbol;
    HANDLE findSymbol = Wh_FindFirstSymbol(module, nullptr, &symbol);
    if (!findSymbol) {
        return false;
    }

    do {
        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (!*symbolHooks[i].pOriginalFunction &&
                std::regex_match(symbol.symbol, symbolHooks[i].symbolRegex)) {
                if (symbolHooks[i].hookFunction) {
                    Wh_SetFunctionHook(symbol.address,
                                       symbolHooks[i].hookFunction,
                                       symbolHooks[i].pOriginalFunction);
                    Wh_Log(L"Hooked %p (%s)", symbol.address, symbol.symbol);
                } else {
                    *symbolHooks[i].pOriginalFunction = symbol.address;
                    Wh_Log(L"Found %p (%s)", symbol.address, symbol.symbol);
                }
                break;
            }
        }
    } while (Wh_FindNextSymbol(findSymbol, &symbol));

    Wh_FindCloseSymbol(findSymbol);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (!symbolHooks[i].optional && !*symbolHooks[i].pOriginalFunction) {
            Wh_Log(L"Missing symbol: %d", i);
            return false;
        }
    }

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    SYMBOL_HOOK symbolHooks[] = {
        {std::wregex(
             LR"(void __cdecl IconUtils::GetIconSize\(bool,enum IconUtils::IconType,struct tagSIZE \* __ptr64\))"),
         (void**)&pOriginal_IconUtils_GetIconSize,
         (void*)IconUtils_GetIconSize_Hook},
        {std::wregex(
             LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired\(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const & __ptr64,enum IconContainerFlags\) __ptr64)"),
         (void**)&pOriginal_IconContainer_IsStorageRecreationRequired,
         (void*)IconContainer_IsStorageRecreationRequired_Hook}};

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarViewDllSymbols() {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    bool windowsVersionIdentified = false;
    HMODULE module;

    WCHAR szTargetDllPath[MAX_PATH];
    wcscpy_s(szTargetDllPath, szWindowsDirectory);
    wcscat_s(
        szTargetDllPath,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(szTargetDllPath) != INVALID_FILE_ATTRIBUTES) {
        // Windows 11 version 22H2.
        windowsVersionIdentified = true;

        module = GetModuleHandle(szTargetDllPath);
        if (!module) {
            // Try to load dependency DLLs. At process start, if they're not
            // loaded, loading the taskbar view DLL fails.
            WCHAR szRuntimeDllPath[MAX_PATH];

            wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
            wcscat_s(
                szRuntimeDllPath,
                LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\vcruntime140_app.dll)");
            LoadLibrary(szRuntimeDllPath);

            wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
            wcscat_s(
                szRuntimeDllPath,
                LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\vcruntime140_1_app.dll)");
            LoadLibrary(szRuntimeDllPath);

            wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
            wcscat_s(
                szRuntimeDllPath,
                LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\msvcp140_app.dll)");
            LoadLibrary(szRuntimeDllPath);

            module = LoadLibrary(szTargetDllPath);
        }
    }

    if (!windowsVersionIdentified) {
        wcscpy_s(szTargetDllPath, szWindowsDirectory);
        wcscat_s(
            szTargetDllPath,
            LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
        if (GetFileAttributes(szTargetDllPath) != INVALID_FILE_ATTRIBUTES) {
            // Windows 11 version 21H2.
            windowsVersionIdentified = true;

            module = GetModuleHandle(szTargetDllPath);
            if (!module) {
                // Try to load dependency DLLs. At process start, if they're not
                // loaded, loading the ExplorerExtensions DLL fails.
                WCHAR szRuntimeDllPath[MAX_PATH];

                PWSTR pProgramFilesDirectory;
                if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0,
                                                   nullptr,
                                                   &pProgramFilesDirectory))) {
                    wcscpy_s(szRuntimeDllPath, pProgramFilesDirectory);
                    wcscat_s(
                        szRuntimeDllPath,
                        LR"(\WindowsApps\Microsoft.VCLibs.140.00_14.0.29231.0_x64__8wekyb3d8bbwe\vcruntime140_app.dll)");
                    LoadLibrary(szRuntimeDllPath);

                    wcscpy_s(szRuntimeDllPath, pProgramFilesDirectory);
                    wcscat_s(
                        szRuntimeDllPath,
                        LR"(\WindowsApps\Microsoft.VCLibs.140.00_14.0.29231.0_x64__8wekyb3d8bbwe\vcruntime140_1_app.dll)");
                    LoadLibrary(szRuntimeDllPath);

                    wcscpy_s(szRuntimeDllPath, pProgramFilesDirectory);
                    wcscat_s(
                        szRuntimeDllPath,
                        LR"(\WindowsApps\Microsoft.VCLibs.140.00_14.0.29231.0_x64__8wekyb3d8bbwe\msvcp140_app.dll)");
                    LoadLibrary(szRuntimeDllPath);

                    CoTaskMemFree(pProgramFilesDirectory);

                    module = LoadLibrary(szTargetDllPath);
                }
            }
        }
    }

    if (!module) {
        Wh_Log(L"Failed to load module");
        return FALSE;
    }

    SYMBOL_HOOK symbolHooks[] = {
        {std::wregex(LR"(__real@4048000000000000)"),
         (void**)&pOriginal_double_48_value, nullptr},
        {std::wregex(
             LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight\(void \* __ptr64,double \* __ptr64\) __ptr64)"),
         (void**)&pOriginal_TaskListItemViewModel_GetIconHeight,
         (void*)TaskListItemViewModel_GetIconHeight_Hook},
        {std::wregex(
             LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels\(enum winrt::WindowsUdk::UI::Shell::TaskbarSize\))"),
         (void**)&pOriginal_TaskbarConfiguration_GetIconHeightInViewPixels,
         (void*)TaskbarConfiguration_GetIconHeightInViewPixels_Hook}};

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (!HookTaskbarViewDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit(void) {
    Wh_Log(L">");

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;
    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    FreeSettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    FreeSettings();
    LoadSettings();

    ApplySettings();
}

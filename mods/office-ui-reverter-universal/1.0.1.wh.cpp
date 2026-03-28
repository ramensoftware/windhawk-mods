// ==WindhawkMod==
// @id            office-ui-reverter-universal
// @name          Office UI Reverter (32/64-bit Universal)
// @name:zh-CN    Office 还原旧版 UI 界面（32/64 位通用）
// @description   Reverts Office 2022+/365 UI to Office 2016/2019
// @description:zh-CN 将 Office 2022+/365 的 UI 界面外观还原为 Office 2016/2019
// @version       1.0.1
// @author        Joe Ye
// @github        https://github.com/JoeYe-233
// @include       WINWORD.EXE
// @include       EXCEL.EXE
// @include       POWERPNT.EXE
// @include       OUTLOOK.EXE
// @include       ONENOTE.EXE
// @include       VISIO.EXE
// @include       WINPROJ.EXE
// @include       MSPUB.EXE
// @include       MSACCESS.EXE
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Office UI Reverter (32/64-bit Universal)
*(Updated to support both 32-bit and 64-bit via PDB Symbol Hooking)*

*This mod is an enhanced fork of the original [Office UI Reverter](https://windhawk.net/mods/office-ui-reverter) created by **Amrsatrio**.*

Reverts the **Office 2022+/Microsoft 365 UI** back to the look of **Office 2019** or **Office 2016**. Can be changed through mod settings.

*Note: this mod needs pdb symbol of `mso40uiWin32Client.dll` to work. The symbol file is expected to be a bit large (~40MB in size). Windhawk will download it automatically when you launch Office apps first time after you installed the mod (the popup at right bottom corner of your screen, please make sure that it shows percentage like "Loading symbols... 0% (mso40uiWin32Client.dll)", wait until it reaches 100% and the pop up disappears, otherwise please switch your network and try again) please wait patiently and **relaunch your Office app AS ADMINISTRATOR at least once** after it finishes, this is to write symbols being used to SymbolCache, which speeds up launching later on.*

## ⚠️ Note:
- It is advised to **turn off automatic updates** for Office applications, as PDB may need to be downloaded every time after updates.
- Please relaunch your Office app as administrator *at least once* after installing the mod and wait for symbol download to complete, this is to write symbols being used to SymbolCache, which speeds up launching later on.
- Early injection to Office processes is required to patch the UI style. This means the process used to launch Office must not be excluded from Windhawk injection.
- Please close all windows of an Office program and relaunch it to apply the new style.
- Older styles may lack icons for new features such as *Copilot*. Those are not covered by this mod.
- Be advised that Microsoft can remove the older style remaining in the application anytime in the future. When that happens, this mod will no longer work until a new workaround is found.

## Credits & Acknowledgements
- **[Amrsatrio](https://github.com/Amrsatrio)**: For the original concept, the initial discovery of the `GetVisualStyleForSurface` function, and the UI style enumeration. 
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- visualStyle: office2019
  $name: Visual style
  $name:zh-CN: 界面样式
  $description: "Note: Older styles may lack icons of newer actions such as Copilot, of which are not covered by this mod.\n
  Changes will take effect on new Office processes."
  $description:zh-CN: "注意：旧版界面外观可能缺少新版里功能的图标（如 Copilot），本 Mod 无法解决此类问题。\n
  所作更改将在下一次启动 Office 应用程序时生效。"
  $options:
    - default: Default (don't modify)
    - office2019: Office 2019
    - office2016: Office 2016
  $options:zh-CN:
    - default: 默认（不修改）
    - office2019: Office 2019
    - office2016: Office 2016
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <atomic>

std::atomic<bool> g_bMso40UILoaded{false};

// -------------------------------------------------------------------------
// Architecture-dependent calling conventions and mangled names
// -------------------------------------------------------------------------
#ifdef _WIN64
    #define WH_CALLCONV
    #define SYM_GetVisualStyle L"?GetVisualStyleForSurface@VisualVersion@@YA?AW4VisualStyle@1@W4VisualSurface@1@@Z"
#else
    #define WH_CALLCONV __stdcall
    #define SYM_GetVisualStyle L"?GetVisualStyleForSurface@VisualVersion@@YG?AW4VisualStyle@1@W4VisualSurface@1@@Z"
#endif

enum class VisualStyle : int { Office2016 = 0, Office2019 = 1 };
enum class VisualSurface : int { Default = 0 };

struct { VisualStyle visualStyle = (VisualStyle)-1; } g_settings;

// -------------------------------------------------------------------------
// Core Hook Logic
// -------------------------------------------------------------------------
typedef VisualStyle (WH_CALLCONV *GetVisualStyleForSurface_t)(VisualSurface);
GetVisualStyleForSurface_t pOrig_GetVisualStyleForSurface = nullptr;

VisualStyle WH_CALLCONV Hook_GetVisualStyleForSurface(VisualSurface surface) {
    if (g_settings.visualStyle != (VisualStyle)-1) {
        return g_settings.visualStyle;
    }
    return pOrig_GetVisualStyleForSurface(surface);
}

// -------------------------------------------------------------------------
// Symbol Hook Initialization
// -------------------------------------------------------------------------
void ScanAndHookMso40UI() {
    HMODULE hMso40UI = GetModuleHandleW(L"mso40uiWin32Client.dll");
    if (!hMso40UI || g_bMso40UILoaded.exchange(true)) return;

    if (g_settings.visualStyle == (VisualStyle)-1) return;

    WindhawkUtils::SYMBOL_HOOK mso40uiWin32ClientDllHook[] = {
        {
            { SYM_GetVisualStyle },
            (void**)&pOrig_GetVisualStyleForSurface,
            (void*)Hook_GetVisualStyleForSurface,
            false
        }
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {0};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE; // Bypass DIA undecoration quirks
    options.onlineCacheUrl = L""; // Gracefully stops it from trying to get online symbols. We will rely entirely on local symbol cache, which is populated when you launch Word as administrator after installing the mod and waiting for symbol download to complete.

    if (WindhawkUtils::HookSymbols(hMso40UI, mso40uiWin32ClientDllHook, ARRAYSIZE(mso40uiWin32ClientDllHook), &options)) {
        Wh_ApplyHookOperations();
        Wh_Log(L"[Success] GetVisualStyleForSurface hooked successfully.");
    } else {
        Wh_Log(L"[Error] Failed to hook symbols in mso40uiWin32Client.dll.");
    }
}

// -------------------------------------------------------------------------
// Intercept LoadLibraryExW to elegantly monitor module loading
// -------------------------------------------------------------------------
typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);
LoadLibraryExW_t pOrig_LoadLibraryExW = nullptr;

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = pOrig_LoadLibraryExW(lpLibFileName, hFile, dwFlags);

    if (hModule && (dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0 && lpLibFileName && !g_bMso40UILoaded.load()) {
        const wchar_t* fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;

        if (_wcsicmp(fileName, L"mso40uiWin32Client.dll") == 0) {
            ScanAndHookMso40UI();
        }
    }

    return hModule;
}

// -------------------------------------------------------------------------
// Lifecycle Management
// -------------------------------------------------------------------------
void LoadSettings() {
    LPCWSTR pszVisualStyle = Wh_GetStringSetting(L"visualStyle");
    if (wcscmp(pszVisualStyle, L"office2019") == 0) g_settings.visualStyle = VisualStyle::Office2019;
    else if (wcscmp(pszVisualStyle, L"office2016") == 0) g_settings.visualStyle = VisualStyle::Office2016;
    else g_settings.visualStyle = (VisualStyle)-1;
    Wh_FreeStringSetting(pszVisualStyle);
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"Office UI Reverter Loaded");

    if (GetModuleHandleW(L"mso40uiWin32Client.dll")) {
        // If already loaded, start the hook thread directly
        ScanAndHookMso40UI();
    } else {
        // Not loaded yet, stand guard on LoadLibraryExW
        Wh_SetFunctionHook((void*)LoadLibraryExW, (void*)Hook_LoadLibraryExW, (void**)&pOrig_LoadLibraryExW);
    }

    return TRUE;
}

void Wh_ModUninit() {}
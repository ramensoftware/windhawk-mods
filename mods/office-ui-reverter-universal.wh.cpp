// ==WindhawkMod==
// @id            office-ui-reverter-universal
// @name          Office UI Reverter (32/64-bit Universal)
// @name:zh-CN    Office 还原旧版 UI 界面（32/64 位通用）
// @description   Reverts Office 2022+/365 UI to Office 2016/2019
// @description:zh-CN 将 Office 2022+/365 的 UI 界面外观还原为 Office 2016/2019
// @version       1.1.0
// @author        Joe Ye
// @github        https://github.com/JoeYe-233
// @include       winword.exe
// @include       excel.exe
// @include       powerpnt.exe
// @include       outlook.exe
// @include       onenote.exe
// @include       visio.exe
// @include       winproj.exe
// @include       mspub.exe
// @include       msaccess.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Office UI Reverter (32/64-bit Universal)
*(Updated to support both 32-bit and 64-bit via PDB Symbol Hooking, added support for splash screen.)*

*This mod is an enhanced fork of the original [Office UI Reverter](https://windhawk.net/mods/office-ui-reverter) created by **Amrsatrio**.*

Reverts the **Office 2022+/Microsoft 365 UI** and **Fluent Splash Screen** back to the look of **Office 2019/2016**. Can be changed through mod settings.

The legacy Office 2016/2019 splash screen is available in both Microsoft 365 Subscription and non-Subscription variants. The non-Subscription variant comes with an extra version without Office logo on the top-left corner. **Note:** On version `2603 Build 19822` (e.g., 2603 Build 19822.20000), this feature may display a blank white splash screen due to underlying implementation changes. This issue has been resolved in later versions of Office, so if you encounter this issue, simply **update Office to the latest version (Recommended)** e.g., 2605, or downgrade to 2602 Build 19725.20190.

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
- **[Rounak](https://github.com/rounk-ctrl)**: For the discovery of the splash screen style function `CSplashUser::FluentSplashScreenUIEnabled`.

## Subscription vs Non-Subscription Splash Screen Variants
![](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/office-ui-reverter-universal-splashscreen.png)

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
    - default: 默认 (不修改)
    - office2019: Office 2019
    - office2016: Office 2016
- splashScreenStyle: legacySub
  $name: Splash screen style
  $name:zh-CN: 启动画面样式
  $description: "Choose the style of the splash screen."
  $description:zh-CN: "选择应用启动画面的样式。"
  $options:
    - default: Default (don't modify)
    - legacySub: Legacy (Office 2016/2019) Subscription
    - legacyNonSub: Legacy (Office 2016/2019) non-Subscription
    - legacyNonSubNoLogo: Legacy non-Subscription without Office Logo
    - fluent: Fluent (Office 2022+ splash screen)

  $options:zh-CN:
    - default: 默认 (不修改)
    - legacySub: 传统 (Office 2016/2019) Microsoft 365 订阅
    - legacyNonSub: 传统 (Office 2016/2019) 非订阅
    - legacyNonSubNoLogo: 传统 (Office 2016/2019) 非订阅 (左上角无 Office 标志)
    - fluent: 新版 Office 2022+ 启动画面
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
    #define SYM_GetSplashScreenType L"?GetSplashScreenType@CSplashUser@@UEAA?AW4SplashScreenType@Mso@@XZ"
#else
    #define WH_CALLCONV __stdcall
    #define SYM_GetVisualStyle L"?GetVisualStyleForSurface@VisualVersion@@YG?AW4VisualStyle@1@W4VisualSurface@1@@Z"
    #define SYM_GetSplashScreenType L"?GetSplashScreenType@CSplashUser@@UAG?AW4SplashScreenType@Mso@@XZ"
#endif

enum class VisualStyle : int { Office2016 = 0, Office2019 = 1 };
enum class VisualSurface : int { Default = 0 };
enum class SplashScreenStyle : int {
    Default = 0,
    LegacyNonSub = 1,
    LegacySub = 2,
    Fluent = 3,
    LegacyNonSubNoLogo = 4,
};

struct { 
    VisualStyle visualStyle = (VisualStyle)-1; 
    SplashScreenStyle splashScreenStyle = SplashScreenStyle::Default;
} g_settings;

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

// Splash Screen Hook
// Function returns an enum Mso::SplashScreenType.
// 0 is LegacyNonSub, 1 is LegacySub, 2 is Fluent, 3 or bigger = LegacyNonSub w/o Office logo at left top corner.
typedef int (WH_CALLCONV *GetSplashScreenType_t)(void* pThis);
GetSplashScreenType_t pOrig_GetSplashScreenType = nullptr;

int WH_CALLCONV Hook_GetSplashScreenType(void* pThis) {
    if (g_settings.splashScreenStyle == SplashScreenStyle::LegacyNonSub) {
        return 0; // LegacyNonSub Splash Screen
    } else if (g_settings.splashScreenStyle == SplashScreenStyle::LegacySub) {
        return 1; // Modern/LegacySub Splash Screen
    } else if (g_settings.splashScreenStyle == SplashScreenStyle::Fluent) {
        return 2; // Fluent Splash Screen
    } else if (g_settings.splashScreenStyle == SplashScreenStyle::LegacyNonSubNoLogo) {
        return 3; // LegacyNonSub without Office logo
    }
    return pOrig_GetSplashScreenType(pThis);
}

// -------------------------------------------------------------------------
// Symbol Hook Initialization
// -------------------------------------------------------------------------
void ScanAndHookMso40UI() {
    HMODULE hMso40UI = GetModuleHandleW(L"mso40uiWin32Client.dll");
    if (!hMso40UI || g_bMso40UILoaded.exchange(true)) return;

    if (g_settings.visualStyle == (VisualStyle)-1 && g_settings.splashScreenStyle == SplashScreenStyle::Default) return;

    WindhawkUtils::SYMBOL_HOOK mso40uiWin32ClientDllHook[] = {
        {
            { SYM_GetVisualStyle },
            (void**)&pOrig_GetVisualStyleForSurface,
            (void*)Hook_GetVisualStyleForSurface,
            false
        },
        {
            { SYM_GetSplashScreenType },
            (void**)&pOrig_GetSplashScreenType,
            (void*)Hook_GetSplashScreenType,
            true // Making this optional so it doesn't break initialization on extremely old Office builds
        }
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {0};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE; // Bypass DIA undecoration quirks
    options.onlineCacheUrl = L""; // Gracefully stops it from trying to get online symbols. We will rely entirely on local symbol cache, which is populated when you launch Word as administrator after installing the mod and waiting for symbol download to complete.

    if (WindhawkUtils::HookSymbols(hMso40UI, mso40uiWin32ClientDllHook, ARRAYSIZE(mso40uiWin32ClientDllHook), &options)) {
        Wh_ApplyHookOperations();
        Wh_Log(L"[Success] Symbols hooked successfully.");
    } else {
        Wh_Log(L"[Error] Failed to hook some symbols in mso40uiWin32Client.dll.");
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

    LPCWSTR pszSplash = Wh_GetStringSetting(L"splashScreenStyle");
    if (wcscmp(pszSplash, L"legacyNonSub") == 0) g_settings.splashScreenStyle = SplashScreenStyle::LegacyNonSub;
    else if (wcscmp(pszSplash, L"legacySub") == 0) g_settings.splashScreenStyle = SplashScreenStyle::LegacySub;
    else if (wcscmp(pszSplash, L"fluent") == 0) g_settings.splashScreenStyle = SplashScreenStyle::Fluent;
    else if (wcscmp(pszSplash, L"legacyNonSubNoLogo") == 0) g_settings.splashScreenStyle = SplashScreenStyle::LegacyNonSubNoLogo;
    else g_settings.splashScreenStyle = SplashScreenStyle::Default;
    Wh_FreeStringSetting(pszSplash);
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
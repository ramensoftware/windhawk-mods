// ==WindhawkMod==
// @id              uxtheme-hook
// @name            UXTheme hook
// @description     Allows you to apply custom themes
// @version         1.4
// @author          rounk-ctrl
// @github          https://github.com/rounk-ctrl
// @include         winlogon.exe
// @include         explorer.exe
// @include         systemsettings.exe
// @include         logonui.exe
// @include         rundll32.exe
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# UXTheme hook
apply custom themes.

## ⚠ Important ⚠
in order for this mod to work properly, you must include `winlogon.exe` and `logonui.exe` in windhawk's process inclusion list, in its advanced settings.

## settings
- **prevent settings on theme apply**: prevents settings from launching every time you apply a theme. this hooks into rundll32.exe, so if any other app uses rundll32 to launch any uwp app, it might break.


# FAQ

## why is it not applying?
you need to first patch the theme with an invalid signature, use themetool from secureuxtheme, select your theme and click patch, then you can apply.


## theme stops applying after restart
this is caused by logonui resetting the default colors, by reading the values located in `HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Themes\DefaultColors\Standard`.
this mod should fix that. if it still doesn't work, try renaming `DefaultColors` to something else.


## bugs
the colors in control panel might be weird sometimes. works fine if you leave control panel open, then switch themes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- preventSettings: false
  $name: Prevent settings from launching on theme apply
  $description: Prevents settings from launching every time you apply a new theme
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <initguid.h>

#ifdef _WIN64
#define STDCALL  __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL  __thiscall
#define SSTDCALL L"__thiscall"
#endif

struct {
    bool preventSettings;
} settings;

#pragma region common
typedef HRESULT (STDCALL *CThemeSignature_Verify_t)(PVOID, PVOID);
CThemeSignature_Verify_t CThemeSignature_Verify;
HRESULT STDCALL CThemeSignature_VerifyHook(
    PVOID pThis,
    PVOID hFile
)
{
    return ERROR_SUCCESS;
}
#pragma endregion

using SetSysColors_t = decltype(&SetSysColors);
SetSysColors_t SetSysColors_orig;

int WINAPI SetSysColors_hook(int cElements, const INT *lpaElements, const COLORREF *lpaRgbValues)
{
    // default fallback
    DWORD elemCount = 13;

    // get the number of values
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\DefaultColors\\Standard", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, &elemCount, NULL, NULL, NULL, NULL);
        RegCloseKey(hKey);
    }

    // logonui
    if (cElements == (int)elemCount) return TRUE;
    return SetSysColors_orig(cElements, lpaElements, lpaRgbValues);
}

// compatibility with windhawk 1.4
DEFINE_GUID(CLSID_ApplicationActivationManager, 0x45ba127d, 0x10a8, 0x46ea, 0x8a,0xb7, 0x56,0xea,0x90,0x78,0x94,0x3c);
DEFINE_GUID(IID_IApplicationActivationManager, 0x2e941141, 0x7f97, 0x4756, 0xba,0x1d, 0x9d,0xec,0xde,0x89,0x4a,0x3d);

using CoCreateInstance_t = decltype(&CoCreateInstance);
CoCreateInstance_t CoCreateInstance_orig;
HRESULT WINAPI CoCreateInstance_hook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv){
    if (IsEqualCLSID(rclsid, CLSID_ApplicationActivationManager) 
        && IsEqualIID(riid, IID_IApplicationActivationManager))
    {
        return E_NOINTERFACE;
    }
    return CoCreateInstance_orig(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}


void LoadSettings() {
    settings.preventSettings = Wh_GetIntSetting(L"preventSettings");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    // uxtheme.dll, uxinit.dll, themeui.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {L"public: long " SSTDCALL " CThemeSignature::Verify(void *)"},
        &CThemeSignature_Verify,
        CThemeSignature_VerifyHook,
        false
    }};

    LPCWSTR files[] = {L"uxtheme.dll", L"uxinit.dll", L"themeui.dll"};

    for (int i = 0; i < int(ARRAYSIZE(files)); ++i)
    {
        HMODULE hMod = LoadLibraryEx(files[i], NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!WindhawkUtils::HookSymbols(hMod, hooks, ARRAYSIZE(hooks)))
        {
            Wh_Log(L"Failed to hook one or more symbol functions from %s", files[i]);
            return FALSE;
        }
        else
        {
            Wh_Log(L"Hooked %s", files[i]);
        }
    }
    
    // for logonui
    WindhawkUtils::SetFunctionHook(SetSysColors, SetSysColors_hook, &SetSysColors_orig);

    if (settings.preventSettings)
    {
        WindhawkUtils::SetFunctionHook(CoCreateInstance, CoCreateInstance_hook, &CoCreateInstance_orig);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
    *bReload = TRUE;
    return TRUE;
}

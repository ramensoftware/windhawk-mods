// ==WindhawkMod==
// @id              accent-color-sync
// @name            Accent Color Sync
// @description     Synchronises OpenGlass and Control Panel color settings
// @version         1.1
// @author          CatmanFan / Mr._Lechkar
// @github          https://github.com/CatmanFan
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Brings back the functionality of the 'Color intensity' slider by synchronising OpenGlass's Aero settings with the slider value.

## ⚠️ Requirements
**This mod will not function properly without [kfh83](https://github.com/kfh83)'s OpenGlass-legacy fork installed.** This is because the mod writes values to the registry's DWM section that are used only by this specific software. This requires the GlassType value in the registry to be set to 0x01 (Aero), and it will be automatically changed as such if it isn't.

To get this mod to function fully, perform the following steps:
1. Grab the source code of OpenGlass-legacy from **[here](https://github.com/ALTaleX531/OpenGlass/tree/legacy)** and compile, or get an existing binary version from **[this GitHub post](https://github.com/ALTaleX531/OpenGlass/pull/14#issue-2415161314)**.
2. Afterwards, go to *HKCU\SOFTWARE\Microsoft\Windows\DWM* in the registry and add any one of the three DWORD values of *og_ColorizationColorBalance*, *og_ColorizationAfterglowBalance* and *og_ColorizationBlurBalance* before applying this mod. These will be handled automatically.

You may need to try changing the accent color manually if changes do not automatically take effect.

## Current bugs
* When changing the slider, there is a rare chance of DWM crashing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- transparency: true
  $name: Enable transparency
  $description: Replicates the option found in Windows 7. Note this may not take effect immediately until after the color has been changed.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <versionhelpers.h>

struct
{
    int opacity;
    bool boolTransparency;
} settings;

const std::wstring dwmKey = L"SOFTWARE\\Microsoft\\Windows\\DWM";
const std::wstring opacityValue = L"og_Opacity";

#pragma region ## Registry functions ##
std::wstring read_SZ(std::wstring sk, std::wstring v, std::wstring defaultValue)
{
    const LPCTSTR subkey = sk.c_str();
    const LPCTSTR value = v.c_str();
    WCHAR szBuffer[512];
    DWORD size(sizeof(szBuffer));

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);
    if (openRes == ERROR_SUCCESS)
    {
        LONG setRes = RegQueryValueEx(hKey, value, 0, NULL, (LPBYTE)&szBuffer, &size);
        RegCloseKey(hKey);

        if (setRes == ERROR_SUCCESS)
        {
            defaultValue = szBuffer;
        }
    }

    return defaultValue;
}

DWORD read_DWORD(std::wstring sk, std::wstring v)
{
    const LPCTSTR subkey = sk.c_str();
    const LPCTSTR value = v.c_str();
    DWORD data(0);
    DWORD size(sizeof(DWORD));

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey);
    if (openRes != ERROR_SUCCESS)
    {
        return NULL;
    }

    LONG setRes = RegQueryValueEx(hKey, value, 0, NULL, reinterpret_cast<LPBYTE>(&data), &size);
    RegCloseKey(hKey);

    if (setRes == ERROR_SUCCESS)
    {
        return data;
    }
    else
    {
        return NULL;
    }
}

BOOL exists_DWORD(std::wstring sk, std::wstring v)
{
    const LPCTSTR subkey = sk.c_str();
    const LPCTSTR value = v.c_str();
    DWORD data(0);
    DWORD size(sizeof(DWORD));

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey);
    LONG setRes = RegQueryValueEx(hKey, value, 0, NULL, reinterpret_cast<LPBYTE>(&data), &size);
    RegCloseKey(hKey);
    
    if (openRes != ERROR_SUCCESS || setRes != ERROR_SUCCESS)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL exists_Key(std::wstring sk)
{
    const LPCTSTR subkey = sk.c_str();

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey);
    if (openRes != ERROR_SUCCESS)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL set_DWORD(std::wstring sk, std::wstring v, unsigned long data)
{
    const LPCTSTR subkey = sk.c_str();
    const LPCTSTR value = v.c_str();

    HKEY hKey;
    LONG openRes = RegOpenKeyEx(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey);
    if (openRes != ERROR_SUCCESS)
    {
        Wh_Log(L"Failed to open registry key");
        return FALSE;
    }

    LONG setRes = data < 0 ? RegDeleteValue(hKey, value) : RegSetValueEx(hKey, value, 0, REG_DWORD, (const BYTE*)&data, sizeof(data));
    RegCloseKey(hKey);

    if (setRes == ERROR_SUCCESS)
    {
        return TRUE;
    }
    else
    {
        Wh_Log(L"Failed writing to registry");
        return FALSE;
    }
}
#pragma endregion

#pragma region ## Windows7 opacity ##
int getOpacityFromBlur(int bB, bool returnOutOf100 = false)
{
    bool found = false;
    if (bB == 28) found = true;
    int x = 0;

    x = (bB - 103.6842f) / -0.526316f;
    if (x >= 26 && x < 102) found = true;

    x = (bB - 76.093f) / -0.255814f;
    if (x >= 102 && x < 188) found = true;

    x = (bB - 131.25f) / -0.535714f;
    if (x >= 189 && x <= 217) found = true;

    if (found)
    {
        if (returnOutOf100) x = (x - 26.0) / (217.0 - 26.0) * 100.0;
        return x;
    }
    return 0;
}

void getColorBalances(int sliderPosition, int &primaryBalance, int &secondaryBalance, int &blurBalance)
{
    auto roundNum = [&](float a) -> int {
    float rem = a - (int)a;
    return (int)a + (rem >= 0.5f ? 1 : 0);
    };
    int pB = 0, sB = 0, bB = 0;
    int x = sliderPosition;
    //primary
    if (x >= 26 && x < 103)
        pB = 5;
    else if (x >= 103 && x < 188)
        pB = roundNum(0.776471*x - 74.9765f);
    else if (x == 188) 
        pB = 71;
    else if (x >= 189 && x <= 217)
        pB = roundNum(0.535714*x - 31.25f);

    //secondary
    if (x >= 26 && x < 102)
        sB = roundNum(0.526316*x - 8.68421f);
    else if (x >= 102 && x < 189)
        sB = roundNum(-0.517241*x + 97.7586f);
    else if (x >= 189 && x <= 217)
        sB = 0;

    //blur 
    if (x >= 26 && x < 102)
        bB = roundNum(-0.526316*x + 103.6842f);
    else if (x >= 102 && x < 188)
        bB = roundNum(-0.255814f*x + 76.093f);
    else if (x == 188)
        bB = 28;
    else if (x >= 189 && x <= 217)
        bB = roundNum(-0.535714f*x + 131.25f);

    primaryBalance = pB;
    secondaryBalance = sB;
    blurBalance = bB;
}
#pragma endregion

void WriteNewColor()
{
    if (settings.opacity < 0 || settings.opacity > 100)
        settings.opacity = getOpacityFromBlur(0x31);
    set_DWORD(dwmKey, opacityValue, settings.opacity);

    // *********************************************
    // Create Aero intensity values
    // *********************************************
    int colour = 100;
    int afterglow = 0;
    int blur = 0;
    
    if (settings.boolTransparency)
    {
        int opacity = (settings.opacity * 0.01) * (217.0 - 26.0) + 26.0;

        getColorBalances
        (
            opacity,
            colour,
            afterglow,
            blur
        );
        
        // Changing the ColorizationColor and Afterglow also affects the intensity slider
    }
    
    // *********************************************
    // Actually do the registry editing
    // *********************************************
    set_DWORD(dwmKey, L"og_ColorizationColorBalance", colour);
    set_DWORD(dwmKey, L"og_ColorizationAfterglowBalance", afterglow);
    set_DWORD(dwmKey, L"og_ColorizationBlurBalance", blur);

    // Other registry values
    if (exists_DWORD(dwmKey, L"GlassOpacity")) set_DWORD(dwmKey, L"GlassOpacity", 0);

    PostMessage(FindWindow(TEXT("dwm"), nullptr), WM_DWMCOLORIZATIONCOLORCHANGED, 0, 0);
}

#pragma region ## DirectUI hooks ##
typedef ATOM WINAPI (*StrToId_T)(unsigned const short*);
StrToId_T StrToID;

// Pointers to DUI elements
intptr_t intensitySlider = 0;

typedef unsigned short(*__cdecl Element_GetID_T)(class Element*, void*);
Element_GetID_T Element_GetID;

typedef void(* __cdecl Element_OnPropertyChanged_T)(class Element*, class PropertyInfo const *,int,class Value *,class Value *);
Element_OnPropertyChanged_T Element_OnPropertyChanged;

void __cdecl Element_OnPropertyChanged_hook(class Element* This, class PropertyInfo const *prop, int integer, class Value *valueA, class Value *valueB)
{
    Element_OnPropertyChanged(This, prop, integer, valueA, valueB);

    ATOM id = Element_GetID(This, &This);
    if (intensitySlider != id && id == StrToID((unsigned const short*)L"IntensitySlider"))
    {
        intensitySlider = reinterpret_cast<intptr_t>(This);
    }
}

long(*__cdecl CCTrackBar_SetThumbPosition)(class CCTrackBar*, int);
long __cdecl CCTrackBar_SetThumbPosition_hook(class CCTrackBar* This, int value)
{
    intptr_t current = reinterpret_cast<intptr_t>(This);

    // Track bar value
    if (current > 0 && current == intensitySlider)
    {
        settings.opacity = (value - 10) / 75.0 * 100.0;
        WriteNewColor();
    }

    return CCTrackBar_SetThumbPosition(This, value);
}
#pragma endregion

WindhawkUtils::SYMBOL_HOOK dui70dll_hooks[] = {
    {
        {L"StrToID"},
        (void**)&StrToID,
    },

    {
        {L"public: unsigned short __cdecl DirectUI::Element::GetID(void)"},
        (void**)&Element_GetID
    },

    {
        {L"public: virtual void __cdecl DirectUI::Element::OnPropertyChanged(struct DirectUI::PropertyInfo const *,int,class DirectUI::Value *,class DirectUI::Value *)"},
        (void**)&Element_OnPropertyChanged,
        (void*)Element_OnPropertyChanged_hook
    },

    {
        {L"public: long __cdecl DirectUI::CCTrackBar::SetThumbPosition(int)"},
        (void**)&CCTrackBar_SetThumbPosition,
        (void*)CCTrackBar_SetThumbPosition_hook
    }
};

BOOL LoadSettings()
{
    bool regSetup = false;

    if (!exists_DWORD(dwmKey, L"og_ColorizationColorBalance")
    && !exists_DWORD(dwmKey, L"og_ColorizationAfterglowBalance")
    && !exists_DWORD(dwmKey, L"og_ColorizationBlurBalance"))
    {
        Wh_Log(L"Did not find OpenGlass Aero balance values, cannot continue");
        return FALSE;
    }
    else if (exists_DWORD(dwmKey, L"og_ColorizationColorBalance")
    && exists_DWORD(dwmKey, L"og_ColorizationAfterglowBalance")
    && exists_DWORD(dwmKey, L"og_ColorizationBlurBalance"))
    {
        regSetup = false;
    }
    else
    {
        regSetup = true;
    }

    if (!exists_DWORD(dwmKey, opacityValue)) regSetup = true;
    else settings.opacity = read_DWORD(dwmKey, opacityValue);

    if (regSetup)
    {
        Wh_Log(L"Setting up registry");
        set_DWORD(dwmKey, L"og_ColorizationColorBalance", 0x08);
        set_DWORD(dwmKey, L"og_ColorizationAfterglowBalance", 0x2b);
        set_DWORD(dwmKey, L"og_ColorizationBlurBalance", 0x31);

        settings.opacity = getOpacityFromBlur(0x31);
        if (!set_DWORD(dwmKey, opacityValue, settings.opacity)) return FALSE;
        if (!exists_DWORD(dwmKey, opacityValue)) return FALSE;
    }

    settings.boolTransparency = Wh_GetIntSetting(L"transparency");

    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed");

    *bReload = TRUE;
    return TRUE;
}

BOOL Wh_ModInit() {
    if (!IsWindows10OrGreater())
    {
        Wh_Log(L"Cannot run on Windows 8.1 or earlier");
        return FALSE;
    }

    std::wstring username = read_SZ(L"Volatile Environment", L"USERNAME", L"???");
    if (username == L"???")
    {
        Wh_Log(L"Warning: Local username not detected, unloading");
        return FALSE;
    }

    if (!LoadSettings())
    {
        Wh_Log(L"Failed to load settings");
        return FALSE;
    }

    HMODULE hDui = LoadLibraryW(L"dui70.dll");
    if (!hDui)
    {
        Wh_Log(L"Failed to load dui70.dll");
        return FALSE;
    }
    if (!WindhawkUtils::HookSymbols(hDui, dui70dll_hooks, ARRAYSIZE(dui70dll_hooks)))
    {
        Wh_Log(L"Failed to hook DUI symbols");
        return FALSE;
    }
    
    if (exists_DWORD(dwmKey, L"GlassType") && read_DWORD(dwmKey, L"GlassType") == 0)
    {
        Wh_Log(L"Glass type detected as Blur, automatically setting to Aero");
        set_DWORD(dwmKey, L"GlassType", 1);
    }

    PostMessage(FindWindow(TEXT("dwm"), nullptr), WM_DWMCOLORIZATIONCOLORCHANGED, 0, 0);
    return TRUE;
}
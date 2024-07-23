// ==WindhawkMod==
// @id              accent-color-sync
// @name            Accent Color Sync
// @description     Synchronises OpenGlass and Control Panel color settings
// @version         1.2
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
1. Install the OpenGlass-legacy fork; this can be done by grabbing the source code from **[the official repo](https://github.com/ALTaleX531/OpenGlass/tree/legacy)** and compile, or get an existing binary version from **[this GitHub post](https://github.com/ALTaleX531/OpenGlass/pull/14#issue-2415161314)**.
   * If you are updating this mod from version 1.0, it is required to disable or uninstall any other existing DWM shader software (such as regular OpenGlass or DWMBlurGlass).
2. Afterwards, go to *HKCU\SOFTWARE\Microsoft\Windows\DWM* in the registry and add any one of the three DWORD values of *og_ColorizationColorBalance*, *og_ColorizationAfterglowBalance* and *og_ColorizationBlurBalance* before applying this mod. These will be handled automatically.

You may need to try changing the accent color manually if changes do not automatically take effect.

## Current known bugs
* *Sync with DWM* mod setting:
   * While this option is enabled, the opacity shown by the Control Panel theme preview's color icon does not always reflect the value set by the color intensity slider when it is changed, unless after the theme preview is regenerated by changing the color RGB values or the desktop background.
   * The OpenGlass color balances do not update after changing the theme itself to one with a different color intensity value. This is because the feature to do so has not been implemented yet for the mod.
* When changing the intensity slider, there is a very rare chance of DWM crashing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- transparency: TRUE
  $name: Enable transparency
  $description: Replicates the option found in Windows 7. At present, this only sets the OpenGlass solid color balance to 100 if the option is disabled.
- syncDWM: FALSE
  $name: Sync with DWM
  $description: Writes the opacity value to DWM's color and afterglow variables. This makes it so that the opacity is also written to the theme alongside the color's RGB. Otherwise, Windows automatically sets it to remain stationary at 0xc4 (196).
*/
// ==/WindhawkModSettings==

#if _WIN64
#define THISCALL  __cdecl
#define STHISCALL L"__cdecl"
#define STRTOID_NAME L"StrToID"
#else
#define THISCALL  __thiscall
#define STHISCALL L"__thiscall"
#define STRTOID_NAME L"_StrToID@4"
#endif

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <versionhelpers.h>
#include <winbase.h>
#include <cstring>
#include <sstream>
#include <iomanip>

struct
{
    int opacity;
    bool boolTransparency;
    bool boolSyncDWM;
} settings;

const std::wstring dwmKey = L"SOFTWARE\\Microsoft\\Windows\\DWM";
const std::wstring opacityValue = L"og_Opacity";

#pragma region ## Registry functions ##
/**
 * Reads a string value from a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @param v The name of the value.
 * @return The string value if it is found, otherwise `NULL`.
 */
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

/**
 * Reads a DWORD value from a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @param v The name of the value.
 * @return The DWORD value if it is found, otherwise `NULL`.
 */
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

/**
 * Checks for the existence of a DWORD value within an HKCU registry key.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @param v The name of the value.
 * @return `TRUE` if found, otherwise `FALSE`.
 */
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

/**
 * Checks for the existence of a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @return `TRUE` if found, otherwise `FALSE`.
 */
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

/**
 * Writes a DWORD value to a registry key within HKCU.
 *
 * @param sk The path to the key, not including "HKCU\".
 * @param v The name of the value.
 * @param data The DWORD value to write.
 * @return `TRUE` if the operation succeeded, otherwise `FALSE`.
 */
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
int getOpacityFromBlur(int bB, bool returnOutOf100 = FALSE)
{
    bool found = FALSE;
    if (bB == 28) found = TRUE;
    int x = 0;

    x = (bB - 103.6842f) / -0.526316f;
    if (x >= 26 && x < 102) found = TRUE;

    x = (bB - 76.093f) / -0.255814f;
    if (x >= 102 && x < 188) found = TRUE;

    x = (bB - 131.25f) / -0.535714f;
    if (x >= 189 && x <= 217) found = TRUE;

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

int intensitySliderMin = 10;
int intensitySliderMax = 85;
int valueTo100(int value) { return value - intensitySliderMin / (intensitySliderMax - intensitySliderMin) * 100; }
int valueFrom100(int value) { return (value / 100) * (intensitySliderMax - intensitySliderMin) + intensitySliderMin; }

void SyncOpacity()
{
    if (!settings.boolSyncDWM) return;

    const std::wstring value = L"ColorizationColor";
    if (!exists_DWORD(dwmKey, value)) return;
    DWORD x = read_DWORD(dwmKey, value);

    std::stringstream ssA;
    ssA << std::hex << std::setfill('0') << std::setw(8) << x;
    int sysOpacity = std::stoul(ssA.str().substr(0, 2), nullptr, 16);

    int ogOpacity = valueTo100(settings.opacity) / 100.0 * 255.0 + 3;

    if (sysOpacity != ogOpacity)
    {
        std::stringstream ssB;
        ssB << std::hex << std::setfill('0') << std::setw(2) << ogOpacity;
        DWORD newColor = std::stoul("0x" + ssB.str() + ssA.str().substr(2), nullptr, 16);

        set_DWORD(dwmKey, L"ColorizationColor", newColor);
        set_DWORD(dwmKey, L"ColorizationAfterglow", newColor);
    }
}

/**
 * Calculates the color, afterglow and blur intensity values from an integer out of 100, then writes them to the registry for use with OpenGlass.
 * @param opacity An integer value ranging between 0 and 100.
 */
void WriteNewColor(int opacity = -1)
{
    if (!settings.boolTransparency)
        opacity = 100;
    else
    {
        if (opacity < 0 || opacity > 100)
            opacity = settings.opacity;
        if (opacity < 0 || opacity > 100)
            settings.opacity = opacity = getOpacityFromBlur(0x31);
    }
    set_DWORD(dwmKey, opacityValue, settings.opacity);

    // *********************************************
    // Create Aero intensity values
    // *********************************************
    int colour = 100;
    int afterglow = 0;
    int blur = 0;
    
    if (settings.boolTransparency)
    {
        getColorBalances
        (
            (opacity * 0.01) * (217.0 - 26.0) + 26.0,
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

    SyncOpacity();
    PostMessage(FindWindow(TEXT("dwm"), nullptr), WM_DWMCOLORIZATIONCOLORCHANGED, 0, 0);
}

#pragma region ## DirectUI hooks ##
typedef ATOM WINAPI (*StrToId_T)(unsigned const short*);
StrToId_T StrToID;

// Pointers to DUI elements
intptr_t intensitySlider = 0;

typedef unsigned short(*THISCALL Element_GetID_T)(class Element*, void*);
Element_GetID_T Element_GetID;

typedef void(*THISCALL Element_OnPropertyChanged_T)(class Element*, class PropertyInfo const *,int,class Value *,class Value *);
Element_OnPropertyChanged_T Element_OnPropertyChanged;

void THISCALL Element_OnPropertyChanged_hook(class Element* This, class PropertyInfo const *prop, int integer, class Value *valueA, class Value *valueB)
{
    Element_OnPropertyChanged(This, prop, integer, valueA, valueB);

    ATOM id = Element_GetID(This, &This);
    if (intensitySlider != id && id == StrToID((unsigned const short*)L"IntensitySlider"))
    {
        intensitySlider = reinterpret_cast<intptr_t>(This);
    }
    else
    {
        SyncOpacity();
    }
}

long(*THISCALL CCTrackBar_SetThumbPosition)(class CCTrackBar*, int);
long THISCALL CCTrackBar_SetThumbPosition_hook(class CCTrackBar* This, int value)
{
    intptr_t current = reinterpret_cast<intptr_t>(This);

    // Track bar value
    if (current > 0 && current == intensitySlider)
    {
        settings.opacity = value;
        WriteNewColor(valueTo100(value));
    }

    return CCTrackBar_SetThumbPosition(This, value);
}
#pragma endregion

// 
WindhawkUtils::SYMBOL_HOOK dui70dll_hooks[] = {
    {
        {STRTOID_NAME},
        (void**)&StrToID,
    },

    {
        {L"public: unsigned short " STHISCALL " DirectUI::Element::GetID(void)"},
        (void**)&Element_GetID
    },

    {
        {L"public: virtual void " STHISCALL " DirectUI::Element::OnPropertyChanged(struct DirectUI::PropertyInfo const *,int,class DirectUI::Value *,class DirectUI::Value *)"},
        (void**)&Element_OnPropertyChanged,
        (void*)Element_OnPropertyChanged_hook
    },

    {
        {L"public: long " STHISCALL " DirectUI::CCTrackBar::SetThumbPosition(int)"},
        (void**)&CCTrackBar_SetThumbPosition,
        (void*)CCTrackBar_SetThumbPosition_hook
    }
};

BOOL LoadSettings()
{
    bool regSetup = FALSE;

    if (!exists_DWORD(dwmKey, L"og_ColorizationColorBalance")
    && !exists_DWORD(dwmKey, L"og_ColorizationAfterglowBalance")
    && !exists_DWORD(dwmKey, L"og_ColorizationBlurBalance"))
    {
        Wh_Log(L"OpenGlass Legacy is not detected, cannot continue");
        return FALSE;
    }
    else if (exists_DWORD(dwmKey, L"og_ColorizationColorBalance")
    && exists_DWORD(dwmKey, L"og_ColorizationAfterglowBalance")
    && exists_DWORD(dwmKey, L"og_ColorizationBlurBalance"))
    {
        regSetup = FALSE;
    }
    else
    {
        regSetup = TRUE;
    }

    if (!exists_DWORD(dwmKey, opacityValue)) regSetup = TRUE;
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
    settings.boolSyncDWM = Wh_GetIntSetting(L"syncDWM");

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

    WriteNewColor();
    return TRUE;
}
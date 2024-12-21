// ==WindhawkMod==
// @id                  accent-color-sync
// @name                Accent color sync
// @description         Syncs OpenGlass & StartIsBack++ colors with DWM
// @version             1.0
// @author              CatmanFan
// @github              https://github.com/CatmanFan
// @include             explorer.exe
// @compilerOptions     -lcomdlg32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod syncs accent colors used by OpenGlass and StartIsBack++ (if found) with that of the DWM every time a change to the latter is effectuated.
It does this by hooking to DefWindowProc and listening for any changes to the accent color.

It also changes the color opacity based on the value that has been set in the Mod Settings.

***This will not function properly without OpenGlass installed.***

### Bugs so far
* When changing the mod settings, the SIB taskbar and start menu opacity may occassionally not match that of the window titlebar. This can be fixed by changing the theme color from Windows itself.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacitySlider: 25
  $name: Color intensity
  $description: Window titlebar color intensity. This needs to be manually set for now.
- isTransparent: true
  $name: Enable transparency
  $description: Replicates the option found in Windows 7.
*/
// ==/WindhawkModSettings==

#include <iomanip>
#include <sstream>
#include <iostream>

#include <libloaderapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <taskschd.h>
#include <dwmapi.h>
#include <winnt.h>
#include <winreg.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

struct {
    int opacity;
    bool isTransparent;
    bool isDWMBG;
} settings;

#pragma region -- DWM functions --
typedef struct COLORIZATIONPARAMS
{
    COLORREF        clrColor;           //ColorizationColor
    COLORREF        clrAftGlow;	        //ColorizationAfterglow
    UINT            nIntensity;	        //ColorizationColorBalance -> 0-100
    UINT            clrAftGlowBal;      //ColorizationAfterglowBalance
    UINT		    clrBlurBal;         //ColorizationBlurBalance
    UINT            clrGlassReflInt;    //ColorizationGlassReflectionIntensity
    BOOL            fOpaque;
} DWMColor;
// Opacity is permanently fixed, as is intensity

DWMColor tempSettings;
DWMColor newSettings;

HRESULT (WINAPI *DwmSetColorizationParameters) (COLORIZATIONPARAMS *colorparam,UINT unknown);
HRESULT (WINAPI *DwmGetColorizationParameters) (COLORIZATIONPARAMS *colorparam);
#pragma endregion

#pragma region -- Registry functions --
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

BOOL keyExists(std::wstring sk)
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

BOOL set_DWORD(std::wstring sk, std::wstring v, DWORD data)
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

    LONG setRes = RegSetValueEx(hKey, value, 0, REG_DWORD, (const BYTE*)&data, sizeof(data));
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

#pragma region -- Aero intensity calculation --
typedef struct AEROINTPARAMETERS
{
    double      color;
    double      afterglow;
    double      blur;
} AEROINT;

AEROINT CalculateAeroIntensity(double value)
{
    float t = 26 + (217 * (value / 100.0f));

    double primary = (t < 103.0) ? 5 : (t < 188.0) ? 0.776471 * t - 74.976471 : (t < 189.0) ? 71 : 0.535714 * t - 31.25;
    double secondary = (t < 102.0) ? 0.526316 * t - 8.684211 : (t < 189.0) ? -0.517241 * t + 97.758621 : 0.0;
    double blur = (t < 102.0) ? -0.526316 * t + 103.684211 : (t < 188.0) ? -0.255814 * t + 76.093023 : (t < 189.0) ? 28.0 : -0.535714 * t + 131.25;

    return AEROINTPARAMETERS {primary, secondary, blur};
}
#pragma endregion

// Maximum value rendered by DWM color is 250
// double factor =  255.0 / 250.0;

COLORREF current;
BOOL isWorking = false;
int msgCount = 0;

void RefreshDWM()
{
    HWND hWnd = FindWindow(TEXT("Dwm"), nullptr);
    
    PostMessage(hWnd, WM_THEMECHANGED, 0, 0);                 // refresh part of the settings related to theme
    PostMessage(hWnd, WM_DWMCOLORIZATIONCOLORCHANGED, 0, 0);  // refresh part of the settings related to color/backdrop
    PostMessage(hWnd, WM_SYSCOLORCHANGE, 0, 0);
}

#pragma region -- Color read and write functions --
BOOL WriteNewColor(COLORREF input)
{
    const COLORREF color = RGB(GetRValue(input), GetGValue(input), GetBValue(input));

    const int opacityValue = settings.isTransparent ? settings.opacity : 100;
    const int opacity255 = (opacityValue / 100.0) * 255.0;
    const AEROINT aeroValues = CalculateAeroIntensity(opacityValue);

    std::stringstream ss1;
    std::stringstream ss2;
    ss1 << std::hex << std::setfill('0') << std::setw(6) << color;
    ss2 << std::hex << std::setfill('0') << std::setw(2) << opacity255;

    const DWORD newColor = std::stoul("0x" + ss2.str() + ss1.str(), nullptr, 16);

    // Write to settings
    newSettings = COLORIZATIONPARAMS();
    newSettings.clrAftGlow      = newSettings.clrColor = newColor;
    newSettings.nIntensity      = static_cast<UINT>(aeroValues.color);
    newSettings.clrAftGlowBal   = static_cast<UINT>(aeroValues.afterglow);
    newSettings.clrBlurBal      = static_cast<UINT>(aeroValues.blur);
    newSettings.clrGlassReflInt = tempSettings.clrGlassReflInt;
    newSettings.fOpaque         = tempSettings.fOpaque;
    
    set_DWORD(L"SOFTWARE\\Microsoft\\Windows\\DWM", L"ColorizationColorBalanceOverride", newSettings.nIntensity);
    set_DWORD(L"SOFTWARE\\Microsoft\\Windows\\DWM", L"ColorizationAfterglowBalanceOverride", newSettings.clrAftGlowBal);
    set_DWORD(L"SOFTWARE\\Microsoft\\Windows\\DWM", L"GlassOpacity", newSettings.clrBlurBal);
    set_DWORD(L"SOFTWARE\\Microsoft\\Windows\\DWM", L"GlassType", settings.isTransparent ? 1 : 4);
    set_DWORD(L"SOFTWARE\\Microsoft\\Windows\\DWM", L"GlassOverrideAccent", settings.isTransparent ? 1 : 0);

    if (keyExists(L"SOFTWARE\\StartIsBack"))
    {
        set_DWORD(L"SOFTWARE\\StartIsBack", L"StartMenuColor", RGB(GetBValue(newColor), GetGValue(newColor), GetRValue(newColor)));
        set_DWORD(L"SOFTWARE\\StartIsBack", L"TaskbarColor", RGB(GetBValue(newColor), GetGValue(newColor), GetRValue(newColor)));
        set_DWORD(L"SOFTWARE\\StartIsBack", L"StartMenuAlpha", settings.isTransparent ? newSettings.nIntensity / 100.0 * 255.0 : 255.0);
        set_DWORD(L"SOFTWARE\\StartIsBack", L"TaskbarAlpha", settings.isTransparent ? newSettings.nIntensity / 100.0 * 255.0: 255.0);
    }

    RefreshDWM();
    return TRUE;
}

BOOL isChangeable(bool force)
{
    if (isWorking && !force) return FALSE;

    current = tempSettings.clrColor;
    DwmGetColorizationParameters(&tempSettings);

    if (tempSettings.clrColor == current)
    {
        return FALSE;
    }

    if (current != 0 || force)
    {
        isWorking = true;

        /* Each unconverted color value contains the opacity in AArrggbb */
        return TRUE;
    }

    return FALSE;
}

void ColorFunction(BOOL force = false)
{
    if (!isWorking) 
    {
        if (isChangeable(force))
        {
            if (isWorking)
            {
                WriteNewColor(tempSettings.clrColor);

                isWorking = false;
            }
        }
    }
}
#pragma endregion

using DefWindowProc_t = decltype(&DefWindowProc);
DefWindowProc_t DefWindowProc_orig;
LRESULT CALLBACK DefWindowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SYSCOLORCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            if (msgCount == 0)
            {
                ColorFunction();
            }
            else
            {
                return DefWindowProc_orig(NULL, WM_NULL, 0, 0);
            }
            msgCount++;
            break;

        default:
            if (msgCount > 0 && uMsg != WM_NULL)
            {
                msgCount = 0;
            }
            break;
    }

    return DefWindowProc_orig(hWnd, uMsg, wParam, lParam);
}

BOOL LoadSettings() {
    settings.opacity = fmin(Wh_GetIntSetting(L"opacitySlider"), (217.0 / 255.0) * 100.0);
    settings.isTransparent = Wh_GetIntSetting(L"isTransparent");
    settings.isDWMBG = false; // !wcscmp(Wh_GetStringSetting(L"dwmSoftware"), L"DWMBlurGlass");

    if (settings.isDWMBG)
    {
        MessageBoxW(NULL, L"DWMBlurGlass functionality not yet implemented", L"Warning", MB_ICONWARNING | MB_OK);
        return FALSE;
    }

    // Load current color
    DWMColor oldSettings;
    DwmGetColorizationParameters(&oldSettings);
    current = oldSettings.clrColor;

    ColorFunction(true);
	RefreshDWM();
    return TRUE;
}

BOOL Wh_ModInit() {
    std::wstring username = read_SZ(L"Volatile Environment", L"USERNAME", L"???");
    if (username == L"???")
    {
        Wh_Log(L"Warning: Local username not detected, unloading");
        return FALSE;
    }

    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (!dwmapi)
    {
        Wh_Log(L"Failed to load dwmapi.dll");
        return FALSE;
    }
    *(FARPROC *)&DwmGetColorizationParameters = GetProcAddress(dwmapi, (LPCSTR)127);
    *(FARPROC *)&DwmSetColorizationParameters = GetProcAddress(dwmapi, (LPCSTR)131);
    BOOL hasComposition;
    DwmIsCompositionEnabled(&hasComposition);
    if (!hasComposition)
    {
        Wh_Log(L"DWM composition not loaded");
        return FALSE;
    }

    if (!WindhawkUtils::Wh_SetFunctionHookT(DefWindowProc, DefWindowProc_hook, &DefWindowProc_orig))
    {
        Wh_Log(L"Failed to hook DefWindowProc");
        return FALSE;
    }

    if (keyExists(L"SOFTWARE\\StartIsBack"))
    {
        Wh_Log(L"Detected StartIsBack++ installation");
    }

    if (!LoadSettings())
    {
        Wh_Log(L"Failed to load settings");
        return FALSE;
    }

	RefreshDWM();
    Wh_Log(L"Loaded as %s", username.c_str());
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L"Settings changed");

    *bReload = TRUE;
    return TRUE;
}
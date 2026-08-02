// ==WindhawkMod==
// @id           translucent-taskbar
// @name         Translucent Taskbar
// @description  Customizes the taskbar appearance with transparent or acrylic glass styling.
// @version      1.0.0
// @author       LukeAmazing
// @github       https://github.com/LukeAmazing
// @include      explorer.exe
// @compilerOptions -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModSettings==
// |- themeMode: 1
//   $name: Theme Style
//   $description: Select the visual theme style for your taskbar.
//   $enum:
//     - 1: Fully Transparent
//     - 2: Acrylic Glass Blur
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>

typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;

typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;

typedef struct _WINDOWCOMPOSITIONATTRIBData {
    DWORD Attribute;
    PVOID pData;
    SIZE_T cbData;
} WINDOWCOMPOSITIONATTRIBData;

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, WINDOWCOMPOSITIONATTRIBData*);
SetWindowCompositionAttribute_t SetWindowCompositionAttribute_Original;

// Check if the target window is a primary or secondary taskbar
BOOL IsTaskbarWindow(HWND hwnd) {
    wchar_t className[256];
    if (GetClassName(hwnd, className, 256)) {
        if (wcscmp(className, L"Shell_TrayWnd") == 0 || wcscmp(className, L"SecondaryTrayWnd") == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// Intercept Windows background updates and apply custom style
BOOL WINAPI SetWindowCompositionAttribute_Hook(HWND hwnd, WINDOWCOMPOSITIONATTRIBData* pData) {
    if (pData && pData->Attribute == 19 && IsTaskbarWindow(hwnd)) {
        int mode = Wh_GetIntSetting(L"themeMode");
        
        ACCENT_POLICY customPolicy = {
            (mode == 1) ? ACCENT_ENABLE_TRANSPARENTGRADIENT : ACCENT_ENABLE_ACRYLICBLURBEHIND,
            2,
            0x00FFFFFF,
            0
        };

        WINDOWCOMPOSITIONATTRIBData customData = *pData;
        customData.pData = &customPolicy;
        customData.cbData = sizeof(customPolicy);

        return SetWindowCompositionAttribute_Original(hwnd, &customData);
    }

    return SetWindowCompositionAttribute_Original(hwnd, pData);
}

void RefreshTaskbar() {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (hTaskbar && SetWindowCompositionAttribute_Original) {
        int mode = Wh_GetIntSetting(L"themeMode");
        ACCENT_POLICY policy = {
            (mode == 1) ? ACCENT_ENABLE_TRANSPARENTGRADIENT : ACCENT_ENABLE_ACRYLICBLURBEHIND,
            2,
            0x00FFFFFF,
            0
        };
        WINDOWCOMPOSITIONATTRIBData data = { 19, &policy, sizeof(policy) };
        SetWindowCompositionAttribute_Original(hTaskbar, &data);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Translucent Taskbar loaded!");

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) return FALSE;

    void* pSetWindowCompositionAttribute = (void*)GetProcAddress(hUser32, "SetWindowCompositionAttribute");
    if (pSetWindowCompositionAttribute) {
        Wh_SetFunctionHook(
            pSetWindowCompositionAttribute,
            (void*)SetWindowCompositionAttribute_Hook,
            (void**)&SetWindowCompositionAttribute_Original
        );
        
        RefreshTaskbar();
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    RefreshTaskbar();
}

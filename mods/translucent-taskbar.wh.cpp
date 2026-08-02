// ==WindhawkMod==
// @id              translucent-taskbar
// @name            Translucent Taskbar
// @description     Makes the taskbar translucent, acrylic, or fully transparent.
// @version         1.0.0
// @author          LukeAmazing
// @github          https://github.com/LukeAmazing
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- style: blur
  $name: Style
  $options:
  - clear: Fully transparent
  - blur: Blur
  - acrylic: Acrylic
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Translucent Taskbar

A lightweight mod that customizes the Windows taskbar appearance with translucent, acrylic, or fully transparent styling.

## Features
- Supports **Blur**, **Acrylic**, and **Fully Transparent** modes.
- Full multi-monitor support across all connected displays.
- Persistent across Explorer restarts and Windows reboots.
- Cleanly restores default taskbar styling when disabled.
*/
// ==/WindhawkModReadme==

#include <windows.h>

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
};

struct ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    DWORD Attribute;
    PVOID pData;
    SIZE_T SizeOfData;
};

typedef BOOL (WINAPI *SetWindowCompositionAttribute_t)(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA* pAttrData);
SetWindowCompositionAttribute_t pfnSetWindowCompositionAttribute = nullptr;

ACCENT_STATE g_accentState = ACCENT_ENABLE_BLURBEHIND;

void LoadSettings() {
    PCWSTR styleStr = Wh_GetStringSetting(L"style");
    if (styleStr) {
        if (lstrcmpW(styleStr, L"clear") == 0) {
            g_accentState = ACCENT_ENABLE_TRANSPARENTGRADIENT;
        } else if (lstrcmpW(styleStr, L"acrylic") == 0) {
            g_accentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        } else {
            g_accentState = ACCENT_ENABLE_BLURBEHIND;
        }
        Wh_FreeStringSetting(styleStr);
    } else {
        g_accentState = ACCENT_ENABLE_BLURBEHIND;
    }
}

bool IsTaskbarWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;

    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId != GetCurrentProcessId()) return false;

    WCHAR className[256];
    if (GetClassNameW(hwnd, className, 256) > 0) {
        if (lstrcmpW(className, L"Shell_TrayWnd") == 0 ||
            lstrcmpW(className, L"Shell_SecondaryTrayWnd") == 0) {
            return true;
        }
    }
    return false;
}

void ApplyTaskbarStyle(HWND hwnd) {
    if (!pfnSetWindowCompositionAttribute || !IsTaskbarWindow(hwnd)) return;

    ACCENT_POLICY policy = { g_accentState, 2, 0x00FFFFFF, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { 19, &policy, sizeof(policy) };
    pfnSetWindowCompositionAttribute(hwnd, &data);
}

void RestoreTaskbarStyle(HWND hwnd) {
    if (!pfnSetWindowCompositionAttribute || !IsTaskbarWindow(hwnd)) return;

    ACCENT_POLICY policy = { ACCENT_ENABLE_TRANSPARENTGRADIENT, 0x13, 0, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { 19, &policy, sizeof(policy) };
    pfnSetWindowCompositionAttribute(hwnd, &data);
}

BOOL WINAPI SetWindowCompositionAttribute_Hook(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA* pAttrData) {
    if (pAttrData && pAttrData->Attribute == 19 && IsTaskbarWindow(hwnd)) {
        ACCENT_POLICY policy = { g_accentState, 2, 0x00FFFFFF, 0 };
        WINDOWCOMPOSITIONATTRIBDATA customData = *pAttrData;
        customData.pData = &policy;
        customData.SizeOfData = sizeof(policy);
        return pfnSetWindowCompositionAttribute(hwnd, &customData);
    }
    return pfnSetWindowCompositionAttribute(hwnd, pAttrData);
}

BOOL CALLBACK EnumWindowsApplyProc(HWND hwnd, LPARAM lParam) {
    if (IsTaskbarWindow(hwnd)) {
        bool restore = (bool)lParam;
        if (restore) {
            RestoreTaskbarStyle(hwnd);
        } else {
            ApplyTaskbarStyle(hwnd);
        }
    }
    return TRUE;
}

void RefreshAllTaskbars(bool restore = false) {
    EnumWindows(EnumWindowsApplyProc, (LPARAM)restore);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) return FALSE;

    void* pfn = (void*)GetProcAddress(hUser32, "SetWindowCompositionAttribute");
    if (!pfn) return FALSE;

    Wh_SetFunctionHook(pfn, (void*)SetWindowCompositionAttribute_Hook, (void**)&pfnSetWindowCompositionAttribute);
    return TRUE;
}

void Wh_ModAfterInit() {
    RefreshAllTaskbars(false);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshAllTaskbars(false);
}

void Wh_ModUninit() {
    Wh_Log(L">");
    RefreshAllTaskbars(true);
}

// ==WindhawkMod==
// @id              translucent-taskbar
// @name            Translucent Taskbar
// @description     Makes the taskbar translucent, acrylic, or fully transparent.
// @version         1.0.1
// @author          LukeAmazing
// @github          https://github.com/LukeAmazing
// @license         GPL-3.0
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

A standalone, lightweight mod focused specifically on simple taskbar transparency and blur effects.

![Preview](https://raw.githubusercontent.com/ramensoftware/windhawk-mods/main/mods/translucent-taskbar.png)

## Features
- Choose between **Fully Transparent**, **Blur**, and **Acrylic** styles.
- Multi-monitor support for secondary taskbars.

## Compatibility & Requirements
- **Windows 10:** Works out of the box.
- **Windows 11:** The Windows 11 taskbar uses a XAML layer on top of `Shell_TrayWnd`. This mod sets the window accent policy, which will only be visible if the XAML background is transparent (e.g., when used alongside **Windows 11 Taskbar Styler**).
- **Conflicts:** Do not use simultaneously with *Taskbar Background Helper* or *Dynamic Taskbar Transparency*, as both mods modify the same window attributes.

## How it differs from Taskbar Background Helper
While *Taskbar Background Helper* offers an extensive suite of features (dark mode variants, maximized-window detection, and per-app exclusions), **Translucent Taskbar** is designed as a minimal, standalone alternative for users who want zero-configuration taskbar styling without extra options.

*Derived in part from win32 composition attribute patterns licensed under GPL-3.0.*
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <atomic>
#include <windhawk_utils.h>

enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
    ACCENT_INVALID_STATE = 6
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_UNDEFINED = 0,
    WCA_ACCENT_POLICY = 19
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
SetWindowCompositionAttribute_t g_pfnSetWindowCompositionAttribute = nullptr;

std::atomic<ACCENT_STATE> g_accentState{ACCENT_ENABLE_BLURBEHIND};

void LoadSettings() {
    auto style = WindhawkUtils::StringSetting::make(L"style");
    if (wcscmp(style, L"clear") == 0) {
        g_accentState.store(ACCENT_ENABLE_TRANSPARENTGRADIENT);
    } else if (wcscmp(style, L"acrylic") == 0) {
        g_accentState.store(ACCENT_ENABLE_ACRYLICBLURBEHIND);
    } else {
        g_accentState.store(ACCENT_ENABLE_BLURBEHIND);
    }
}

bool IsTaskbarWindow(HWND hwnd) {
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId != GetCurrentProcessId()) return false;

    WCHAR className[256];
    if (GetClassNameW(hwnd, className, 256) > 0) {
        if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
            return true;
        }
    }
    return false;
}

void ApplyTaskbarStyle(HWND hwnd) {
    if (!g_pfnSetWindowCompositionAttribute || !IsTaskbarWindow(hwnd)) return;

    ACCENT_POLICY policy = { g_accentState.load(), 2, 0x00FFFFFF, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
    g_pfnSetWindowCompositionAttribute(hwnd, &data);
}

void RestoreTaskbarStyle(HWND hwnd) {
    if (!g_pfnSetWindowCompositionAttribute || !IsTaskbarWindow(hwnd)) return;

    ACCENT_POLICY policy = { ACCENT_ENABLE_TRANSPARENTGRADIENT, 0x13, 0, 0 };
    WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(policy) };
    g_pfnSetWindowCompositionAttribute(hwnd, &data);
}

BOOL WINAPI SetWindowCompositionAttribute_Hook(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA* pAttrData) {
    if (pAttrData && pAttrData->Attribute == WCA_ACCENT_POLICY && IsTaskbarWindow(hwnd)) {
        ACCENT_POLICY policy = { g_accentState.load(), 2, 0x00FFFFFF, 0 };
        WINDOWCOMPOSITIONATTRIBDATA customData = *pAttrData;
        customData.pData = &policy;
        customData.SizeOfData = sizeof(policy);
        return g_pfnSetWindowCompositionAttribute(hwnd, &customData);
    }
    return g_pfnSetWindowCompositionAttribute(hwnd, pAttrData);
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

    auto pSetWindowCompositionAttribute = (SetWindowCompositionAttribute_t)GetProcAddress(
        hUser32, "SetWindowCompositionAttribute");
    if (!pSetWindowCompositionAttribute) return FALSE;

    WindhawkUtils::SetFunctionHook(
        pSetWindowCompositionAttribute,
        SetWindowCompositionAttribute_Hook,
        &g_pfnSetWindowCompositionAttribute
    );
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

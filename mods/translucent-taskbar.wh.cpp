// ==WindhawkMod==
// @id              translucent-taskbar
// @name            Translucent Taskbar
// @description     Makes the taskbar translucent or fully transparent.
// @version         1.0.0
// @author          LukeAmazing
// @github          https://github.com/LukeAmazing
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Translucent Taskbar

Customizes the taskbar appearance on Windows by applying a clean translucent or transparent style.

## Features
- Sleek, modern taskbar appearance.
- Lightweight and efficient performance.
*/
// ==/WindhawkModReadme==

#include <windows.h>

// Undocumented Windows Composition Attribute APIs
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
    ULONG SizeOfData;
};

typedef BOOL(WINAPI* pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

void ApplyTaskbarStyle() {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (!hTaskbar) return;

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (!hUser) return;

    pfnSetWindowCompositionAttribute SetWindowCompositionAttribute =
        (pfnSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");

    if (SetWindowCompositionAttribute) {
        // ACCENT_ENABLE_BLURBEHIND creates the translucent effect
        ACCENT_POLICY policy = { ACCENT_ENABLE_BLURBEHIND, 2, 0x00FFFFFF, 0 };
        WINDOWCOMPOSITIONATTRIBDATA data = { 19, &policy, sizeof(policy) }; // 19 = WCA_ACCENT_POLICY
        SetWindowCompositionAttribute(hTaskbar, &data);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Translucent Taskbar initialized.");
    ApplyTaskbarStyle();
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Translucent Taskbar uninitialized.");
}

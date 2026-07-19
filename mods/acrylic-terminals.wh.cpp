// ==WindhawkMod==
// @id              acrylic-terminals
// @name            Acrylic Terminals (CMD & PowerShell)
// @description     Beautiful transparent acrylic glass effect for CMD, PowerShell, and Windows Terminal
// @version         1.0
// @author          adrianzgoated
// @github          https://github.com/adrianzgoated
// @include         conhost.exe
// @include         WindowsTerminal.exe
// @compilerOptions -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- Alpha: 0xB0
  $name: Transparency Level
  $description: Opacity of the acrylic effect (0x00 = invisible, 0xFF = fully opaque)
- TintR: 12
  $name: Tint Red
  $description: Red component of the tint color (0-255)
- TintG: 0
  $name: Tint Green
  $description: Green component of the tint color (0-255)
- TintB: 30
  $name: Tint Blue
  $description: Blue component of the tint color (0-255)
- ExtendFrame: TRUE
  $name: Extend frame into client area
  $description: Fills the entire window with the acrylic effect
- DarkTitlebar: TRUE
  $name: Force dark titlebar
  $description: Uses a dark titlebar for better contrast with the acrylic effect
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
Acrylic Terminals

A high-performance Windhawk modification that applies native Windows Acrylic
blur-behind composition to legacy console subsystem windows. Leveraging the
undocumented `SetWindowCompositionAttribute` API, this mod intercepts the
DWM (Desktop Window Manager) composition pipeline to render real-time
acrylic transparency on `conhost.exe`-hosted terminals, including Command
Prompt, Windows PowerShell, and PowerShell 7 (pwsh).

Features:
  - Native Acrylic blur-behind via `ACCENT_ENABLE_ACRYLICBLURBEHIND`
  - Full client-area coverage through `DwmExtendFrameIntoClientArea`
  - Configurable RGB tint channels and alpha opacity
  - Dark titlebar enforcement for optimal contrast rendering
  - Clean teardown: full accent state restoration on mod unload
  - Compatible with Windows 10 (1809+) and Windows 11

Requirements:
  - Windhawk (https://windhawk.net)
  - Windows 10 version 1809 or later / Windows 11
  - Desktop Window Manager (DWM) must be active

Notes:
  - Targets `conhost.exe`, the host process responsible for rendering
    legacy console windows. This ensures proper HWND ownership for
    composition attribute calls.
  - The acrylic effect inherits system backdrop parameters from DWM.
    For best results, ensure "Transparency effects" is enabled in
    Windows Personalization settings.

Author: adrianzgoated
License: Free to use, modify, and distribute.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>

struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    int Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
};

typedef BOOL(WINAPI *pfnSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA *);

static pfnSetWindowCompositionAttribute pSWCA = nullptr;

static const DWORD WCA_ACCENT_POLICY = 19;

static void ApplyAcrylic(HWND hwnd) {
    if (!pSWCA) return;
    if (!IsWindowVisible(hwnd)) return;

    int alpha   = Wh_GetIntSetting(L"Alpha");
    int tintR   = Wh_GetIntSetting(L"TintR");
    int tintG   = Wh_GetIntSetting(L"TintG");
    int tintB   = Wh_GetIntSetting(L"TintB");

    ACCENT_POLICY accent = {};
    accent.AccentState   = 4; // ACCENT_ENABLE_ACRYLICBLURBEHIND
    accent.AccentFlags   = 2;
    accent.GradientColor = (alpha << 24) | ((tintB & 0xFF) << 16) | ((tintG & 0xFF) << 8) | (tintR & 0xFF);

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attribute   = WCA_ACCENT_POLICY;
    data.Data        = &accent;
    data.SizeOfData  = sizeof(accent);

    pSWCA(hwnd, &data);

    if (Wh_GetIntSetting(L"ExtendFrame")) {
        MARGINS margins = { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    if (Wh_GetIntSetting(L"DarkTitlebar")) {
        BOOL dark = TRUE;
        DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    }
}

static void RemoveAcrylic(HWND hwnd) {
    if (!pSWCA) return;

    ACCENT_POLICY accent = {};
    accent.AccentState = 0; // ACCENT_DISABLED

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attribute  = WCA_ACCENT_POLICY;
    data.Data       = &accent;
    data.SizeOfData = sizeof(accent);

    pSWCA(hwnd, &data);

    if (Wh_GetIntSetting(L"ExtendFrame")) {
        MARGINS margins = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }
}

static BOOL CALLBACK EnumApplyProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == (DWORD)lParam)
        ApplyAcrylic(hwnd);
    return TRUE;
}

static BOOL CALLBACK EnumRemoveProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == (DWORD)lParam)
        RemoveAcrylic(hwnd);
    return TRUE;
}

BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) return FALSE;

    pSWCA = reinterpret_cast<pfnSetWindowCompositionAttribute>(
        GetProcAddress(hUser32, "SetWindowCompositionAttribute"));
    if (!pSWCA) return FALSE;

    DWORD myPid = GetCurrentProcessId();
    EnumWindows(EnumApplyProc, myPid);

    return TRUE;
}

void Wh_ModUninit() {
    DWORD myPid = GetCurrentProcessId();
    EnumWindows(EnumRemoveProc, myPid);
}



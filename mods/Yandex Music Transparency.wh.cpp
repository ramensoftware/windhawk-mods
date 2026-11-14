// ==WindhawkMod==
// @id              yandex-music-transparency
// @name            Yandex Music Transparency
// @description     Makes the Yandex Music window transparent
// @version         1.1
// @author          ChatGPT with tester 'NIKTO'
// @github          NIKTO-2
// @include         Яндекс Музыка.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Yandex Music Transparency

Makes the Yandex Music desktop application window transparent. You can adjust the opacity level in the mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Alpha: 220
  $name: Window Opacity
  $description: Transparency level from 50 (more transparent) to 255 (opaque).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <wchar.h>

struct Settings {
    int alpha;
};

Settings g_settings;
bool g_unloading = false;

void LoadSettings() {
    g_settings.alpha = Wh_GetIntSetting(L"Alpha");
    if (g_settings.alpha < 50) g_settings.alpha = 50;
    if (g_settings.alpha > 255) g_settings.alpha = 255;
}

bool IsTargetWindow(HWND hwnd) {
    if (!IsWindow(hwnd)) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;
    if (GetParent(hwnd) != NULL) return false;

    WCHAR className;
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0 ||
        wcscmp(className, L"Chrome_WidgetWin_1") != 0) {
        return false;
    }

    return true;
}

bool UsesColorKey(HWND hwnd) {
    DWORD flags = 0;
    if (GetLayeredWindowAttributes(hwnd, NULL, NULL, &flags)) {
        return (flags & LWA_COLORKEY) != 0;
    }
    return false;
}

using SetLayeredWindowAttributes_t = decltype(&SetLayeredWindowAttributes);
SetLayeredWindowAttributes_t SetLayeredWindowAttributes_Original;

BOOL WINAPI SetLayeredWindowAttributes_Hook(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags) {
    if (!g_unloading && IsTargetWindow(hwnd) && (dwFlags & LWA_ALPHA) && !(dwFlags & LWA_COLORKEY)) {
        bAlpha = (BYTE)g_settings.alpha;
    }
    return SetLayeredWindowAttributes_Original(hwnd, crKey, bAlpha, dwFlags);
}

void ApplyTransparency(HWND hwnd) {
    if (!IsTargetWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_LAYERED) {
        if (UsesColorKey(hwnd)) {
            return;
        }
    } else {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }

    SetLayeredWindowAttributes_Original(hwnd, 0, (BYTE)g_settings.alpha, LWA_ALPHA);
}

void RestoreTransparency(HWND hwnd) {
    if (!IsTargetWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_LAYERED) && !UsesColorKey(hwnd)) {
        SetLayeredWindowAttributes_Original(hwnd, 0, 255, LWA_ALPHA);
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;

    if (lParam == 1) {
        RestoreTransparency(hwnd);
    } else {
        ApplyTransparency(hwnd);
    }
    return TRUE;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;

BOOL WINAPI ShowWindow_Hook(HWND hwnd, int nCmdShow) {
    BOOL result = ShowWindow_Original(hwnd, nCmdShow);
    if (!g_unloading && IsWindowVisible(hwnd)) {
        ApplyTransparency(hwnd);
    }
    return result;
}

BOOL Wh_ModInit() {
    LPCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && wcsstr(cmdLine, L"--type=") != NULL) {
        return FALSE;
    }

    LoadSettings();

    Wh_SetFunctionHook((void*)SetLayeredWindowAttributes, (void*)SetLayeredWindowAttributes_Hook, (void**)&SetLayeredWindowAttributes_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModUninit() {
    g_unloading = true;
    EnumWindows(EnumWindowsProc, 1);
}

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

struct Settings {
    int alpha;
};

Settings g_settings;
HANDLE g_hStopEvent = NULL;
HANDLE g_hThread = NULL;

void LoadSettings() {
    g_settings.alpha = Wh_GetIntSetting(L"Alpha");
    if (g_settings.alpha < 50) g_settings.alpha = 50;
    if (g_settings.alpha > 255) g_settings.alpha = 255;
}

void ApplyTransparency(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_LAYERED)) {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }

    SetLayeredWindowAttributes(hwnd, 0, (BYTE)g_settings.alpha, LWA_ALPHA);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    if (pid != GetCurrentProcessId()) return TRUE;
    if (GetParent(hwnd) != NULL) return TRUE;
    if (!IsWindowVisible(hwnd)) return TRUE;

    ApplyTransparency(hwnd);
    return TRUE;
}

DWORD WINAPI ForceTransparencyThread(LPVOID lpParam) {
    while (WaitForSingleObject(g_hStopEvent, 1000) == WAIT_TIMEOUT) {
        EnumWindows(EnumWindowsProc, 0);
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) {
        return FALSE;
    }

    g_hThread = CreateThread(NULL, 0, ForceTransparencyThread, NULL, 0, NULL);
    if (!g_hThread) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
        return FALSE;
    }

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
    if (g_hStopEvent) {
        SetEvent(g_hStopEvent);
    }
    if (g_hThread) {
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
    }
    if (g_hStopEvent) {
        CloseHandle(g_hStopEvent);
        g_hStopEvent = NULL;
    }
}

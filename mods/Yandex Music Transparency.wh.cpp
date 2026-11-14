// ==WindhawkMod==
// @id              yandex-music-transparency
// @name            Yandex Music Transparency
// @description     Makes Yandex Music window transparent
// @version         1.1
// @author          ChatGPT with tester 'NIKTO'
// @include         Яндекс Музыка.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <windows.h>

struct Settings {
    int Alpha;
};

Settings g_settings;

void LoadSettings() {
    g_settings.Alpha = Wh_GetIntSetting(L"Alpha");
    if (g_settings.Alpha < 50) g_settings.Alpha = 50;
    if (g_settings.Alpha > 255) g_settings.Alpha = 255;
}

void ApplyTransparency(HWND hwnd) {
    if (!IsWindow(hwnd)) return;

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_LAYERED)) {
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    }

    SetLayeredWindowAttributes(hwnd, 0, (BYTE)g_settings.Alpha, LWA_ALPHA);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    // Ищем окна только нашего процесса
    if (pid != GetCurrentProcessId()) return TRUE;

    // Ищем только главные окна (без родителей)
    if (GetParent(hwnd) != NULL) return TRUE;

    // Ищем только видимые окна
    if (!IsWindowVisible(hwnd)) return TRUE;

    // Принудительно применяем прозрачность
    ApplyTransparency(hwnd);

    return TRUE;
}

// Поток, который каждую секунду пробивает прозрачность (для защиты от сброса самим приложением)
DWORD WINAPI ForceTransparencyThread(LPVOID lpParam) {
    while (true) {
        EnumWindows(EnumWindowsProc, 0);
        Sleep(1000); // пауза 1 секунда
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();

    // Запускаем фоновый процесс, который будет следить за прозрачностью
    CreateThread(NULL, 0, ForceTransparencyThread, NULL, 0, NULL);

    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(EnumWindowsProc, 0);
}

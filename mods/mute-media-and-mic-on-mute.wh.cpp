Mute Media & Mic on System Mute
mute-media-and-mic-on-mute
1.2
SP_ENJIXX
explorer.exe
Automatically pauses media playback and toggles microphone mute when system audio is muted.
Описание
Настройки
Исходный код
Дополнительно
Скрыть настройки и Readme

// ==WindhawkMod==
// @id           mute-media-and-mic-on-mute
// @name         Mute Media & Mic on System Mute
// @description  Automatically pauses media playback and toggles microphone mute when system audio is muted.
// @version      1.2
// @author       SP_ENJIXX
// @include      explorer.exe
// @compilerOptions -lole32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

HANDLE g_hThread = NULL;
BOOL g_bRunning = TRUE;

// Функция проверки состояния звука
bool IsSystemMuted() {
    bool isMuted = false;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioEndpointVolume* pEndpointVolume = NULL;

    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator
    );

    if (SUCCEEDED(hr)) {
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    }
    if (SUCCEEDED(hr)) {
        hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&pEndpointVolume);
    }
    if (SUCCEEDED(hr)) {
        BOOL muted = FALSE;
        if (SUCCEEDED(pEndpointVolume->GetMute(&muted))) {
            isMuted = (muted == TRUE);
        }
    }

    if (pEndpointVolume) pEndpointVolume->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();

    return isMuted;
}

// Вспомогательная функция отправки сочетания клавиш
void TriggerActions() {
    // 1. Поставить на паузу / Возобновить медиа
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, 0, 0);
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_KEYUP, 0);

    // 2. Переключить микрофон (Win + Alt + M)
    keybd_event(VK_LWIN, 0, 0, 0);
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event('M', 0, 0, 0);
    keybd_event('M', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
}

DWORD WINAPI MonitorAudioThread(LPVOID lpParam) {
    CoInitialize(NULL);
    bool lastState = false;

    while (g_bRunning) {
        bool currentState = IsSystemMuted();

        if (currentState != lastState) {
            TriggerActions();
            lastState = currentState;
        }

        Sleep(200);
    }

    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    g_bRunning = TRUE;
    g_hThread = CreateThread(NULL, 0, MonitorAudioThread, NULL, 0, NULL);
    return (g_hThread != NULL);
}

void Wh_ModUninit() {
    g_bRunning = FALSE;
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 1000);
        CloseHandle(g_hThread);
    }
}

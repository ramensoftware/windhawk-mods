// ==WindhawkMod==
// @id           mute-media-and-mic-on-mute
// @name         Mute Media & Mic on System Mute
// @description  Automatically pauses media playback and toggles microphone mute when system audio is muted.
// @version      1.2
// @author       SP_ENJIXX
// @include      windhawk.exe
// @compilerOptions -lole32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mute Media & Mic on System Mute

Automatically synchronizes your default microphone and media playback state with the primary system audio mute setting.

### Features
* **Microphone Mute:** When primary audio output is muted, the default recording endpoint (microphone) is automatically muted at the Windows endpoint level.
* **Media Pause:** Sends standard playback toggles when audio status transitions between muted and active states.
* **Lightweight Tool:** Runs as an isolated background utility thread without injecting into system processes like Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <atomic>

HANDLE g_hThread = NULL;
HANDLE g_hStopEvent = NULL;

void SetMicrophoneMute(bool mute) {
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pMic = NULL;
    IAudioEndpointVolume* pVol = NULL;

    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator
    );

    if (SUCCEEDED(hr)) {
        hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pMic);
    }
    if (SUCCEEDED(hr)) {
        hr = pMic->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&pVol);
    }
    if (SUCCEEDED(hr)) {
        pVol->SetMute(mute ? TRUE : FALSE, NULL);
    }

    if (pVol) pVol->Release();
    if (pMic) pMic->Release();
    if (pEnumerator) pEnumerator->Release();
}

bool IsSystemMuted(bool* pSuccess) {
    bool isMuted = false;
    *pSuccess = false;

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
            *pSuccess = true;
        }
    }

    if (pEndpointVolume) pEndpointVolume->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();

    return isMuted;
}

void TriggerMediaPause() {
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, 0, 0);
    keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_KEYUP, 0);
}

DWORD WINAPI MonitorAudioThread(LPVOID lpParam) {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    bool success = false;
    bool lastState = IsSystemMuted(&success);

    while (WaitForSingleObject(g_hStopEvent, 200) == WAIT_TIMEOUT) {
        bool querySuccess = false;
        bool currentState = IsSystemMuted(&querySuccess);

        if (!querySuccess) {
            continue;
        }

        if (currentState != lastState) {
            TriggerMediaPause();
            SetMicrophoneMute(currentState);
            lastState = currentState;
        }
    }

    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    g_hStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!g_hStopEvent) return FALSE;

    g_hThread = CreateThread(NULL, 0, MonitorAudioThread, NULL, 0, NULL);
    return (g_hThread != NULL);
}

void WhTool_ModUninit() {
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

// Boilerplate launcher code for Windhawk Tool Mod
BOOL Wh_ModInit() {
    return WhTool_ModInit();
}

void Wh_ModUninit() {
    WhTool_ModUninit();
}

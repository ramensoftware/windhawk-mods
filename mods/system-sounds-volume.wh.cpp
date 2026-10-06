// ==WindhawkMod==
// @id system-sounds-volume
// @name System Sounds Volume
// @description Control Windows system and notification sounds independently.
// @version 1.3.0
// @author The.BARDIA
// @include explorer.exe
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# System Sounds Volume

Controls the volume of Windows System Sounds independently.

Affected:
- Notification sounds
- Error sounds
- Warning sounds
- Exclamation sounds
- Windows UI sounds

Not affected:
- Games
- Music
- Browsers
- Discord
- Videos
- Master volume

Default volume: 40%

Author: The.BARDIA
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Volume: 40
  $name: System Sounds Volume
  $description: System sound volume. 0 = muted, 100 = full volume.
  $options:
    - 0: 0%
    - 10: 10%
    - 20: 20%
    - 30: 30%
    - 40: 40%
    - 50: 50%
    - 60: 60%
    - 70: 70%
    - 80: 80%
    - 90: 90%
    - 100: 100%

- TestSound: false
  $name: Test Sound
  $description: Enable this option and save the settings to play a test sound.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <windhawk_api.h>

static HANDLE g_stopEvent = nullptr;
static HANDLE g_thread = nullptr;

static int g_volume = 40;
static bool g_testSound = false;

static void LoadSettings()
{
    g_volume = Wh_GetIntSetting(L"Volume");

    if (g_volume < 0)
        g_volume = 0;

    if (g_volume > 100)
        g_volume = 100;

    g_testSound = Wh_GetIntSetting(L"TestSound") != 0;
}

static bool SetSystemSoundsVolume()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    bool initialized = SUCCEEDED(hr);

    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
        return false;

    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioSessionManager2* sessionManager = nullptr;
    IAudioSessionEnumerator* sessionEnumerator = nullptr;

    bool success = false;

    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&deviceEnumerator)
    );

    if (SUCCEEDED(hr))
    {
        hr = deviceEnumerator->GetDefaultAudioEndpoint(
            eRender,
            eConsole,
            &device
        );
    }

    if (SUCCEEDED(hr))
    {
        hr = device->Activate(
            __uuidof(IAudioSessionManager2),
            CLSCTX_ALL,
            nullptr,
            reinterpret_cast<void**>(&sessionManager)
        );
    }

    if (SUCCEEDED(hr))
    {
        hr = sessionManager->GetSessionEnumerator(
            &sessionEnumerator
        );
    }

    if (SUCCEEDED(hr))
    {
        int count = 0;

        if (SUCCEEDED(sessionEnumerator->GetCount(&count)))
        {
            for (int i = 0; i < count; i++)
            {
                IAudioSessionControl* session = nullptr;

                if (FAILED(
                    sessionEnumerator->GetSession(i, &session)
                ))
                {
                    continue;
                }

                IAudioSessionControl2* session2 = nullptr;

                if (SUCCEEDED(
                    session->QueryInterface(
                        __uuidof(IAudioSessionControl2),
                        reinterpret_cast<void**>(&session2)
                    )
                ))
                {
                    if (session2->IsSystemSoundsSession() == S_OK)
                    {
                        ISimpleAudioVolume* simpleVolume = nullptr;

                        if (SUCCEEDED(
                            session->QueryInterface(
                                __uuidof(ISimpleAudioVolume),
                                reinterpret_cast<void**>(&simpleVolume)
                            )
                        ))
                        {
                            float volume =
                                static_cast<float>(g_volume) / 100.0f;

                            simpleVolume->SetMasterVolume(
                                volume,
                                nullptr
                            );

                            simpleVolume->Release();

                            success = true;
                        }
                    }

                    session2->Release();
                }

                session->Release();
            }
        }
    }

    if (sessionEnumerator)
        sessionEnumerator->Release();

    if (sessionManager)
        sessionManager->Release();

    if (device)
        device->Release();

    if (deviceEnumerator)
        deviceEnumerator->Release();

    if (initialized)
        CoUninitialize();

    return success;
}

static void PlayTestSound()
{
    MessageBeep(MB_ICONEXCLAMATION);
}

static DWORD WINAPI VolumeThread(LPVOID)
{
    while (
        WaitForSingleObject(
            g_stopEvent,
            1000
        ) == WAIT_TIMEOUT
    )
    {
        SetSystemSoundsVolume();
    }

    return 0;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"System Sounds Volume - The.BARDIA");

    LoadSettings();

    g_stopEvent = CreateEventW(
        nullptr,
        TRUE,
        FALSE,
        nullptr
    );

    if (!g_stopEvent)
        return FALSE;

    SetSystemSoundsVolume();

    if (g_testSound)
    {
        Sleep(200);
        PlayTestSound();
    }

    g_thread = CreateThread(
        nullptr,
        0,
        VolumeThread,
        nullptr,
        0,
        nullptr
    );

    if (!g_thread)
    {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;

        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit()
{
    if (g_stopEvent)
    {
        SetEvent(g_stopEvent);
    }

    if (g_thread)
    {
        WaitForSingleObject(
            g_thread,
            2000
        );

        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_stopEvent)
    {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

void Wh_ModSettingsChanged()
{
    LoadSettings();

    SetSystemSoundsVolume();

    if (g_testSound)
    {
        Sleep(200);
        PlayTestSound();
    }
}
